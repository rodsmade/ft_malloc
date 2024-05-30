/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_malloc.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: roaraujo <roaraujo@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/31 17:18:59 by roaraujo          #+#    #+#             */
/*   Updated: 2024/04/04 17:15:29 by roaraujo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc.h"

// global variable is a pointer to a LegderEntry object that holds a pointer to "next"
// "next" is a pointer to the next allocated memory pointer. before it there is metadata containing a pointer to "next"
// which means for every pointer, size + sizeof(LedgerEntry) is actually allocated under the hood
// i guess that works

/*
	as per the zones, I'm thinking. 4096 bytes is a LOT. 
	zone 1 - 0 to 4096 bytes, needs defrag/coalesce strats
		buckets of size 16, 64, 256 and 1024
		a page is 4096. which means a page can contain
			- 4 buckets of size 1024
			- 16 buckets of size 256
			- 64 buckets of size 64
			- 256 buckets of size 16
		this could be the first zone.

	zone 2 - 4097 to 4MB, needs defrag/coalesce strats

	zone 3 - 4MB+, not preallocated, allocated on demand actually, munmaped immediately btw.
*/

// new calls to mmap need to follow the available in getrlimit, even if it's infinite. remember you're not using sbreak 

void *TINY__ZONE = NULL;
void *SMALL__ZONE = NULL;
// sizeof(LEDGER ) = getpagesize();
// sizeof(void *) = 8;
// total available allocations = getpagesize() / sizeof(void *) = 4096 / 8 = 512;
void *LEDGER = NULL;

// Constructor function
__attribute__((constructor))
void mylib_init() {
	write(1, "|||||||||||||||| Library initialized!\n", 38);

	AllocationEntry *entry;

	// alloc TINY zone
	// initially one page only
	TINY__ZONE = mmap(NULL, 2 * getpagesize(), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
	entry = (AllocationEntry *) TINY__ZONE;
	entry->in_use = FALSE;
	entry->size = 0;

	// alloc SMALL zone
	// initially one page only
	SMALL__ZONE = mmap(NULL, 100 * getpagesize(), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
	entry = (AllocationEntry *) SMALL__ZONE;
	entry->in_use = FALSE;
	entry->size = 0;

	// LEDGER
	// initially one page only
	LEDGER = mmap(NULL, 1 * getpagesize(), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
	unsigned long i = -1;
	while (++i < getpagesize() / sizeof(void *))
		((void **)LEDGER)[i] = NULL;
}

void	*malloc(size_t size)
{
	if (size <= TINY_ZONE_THRESHOLD) {
		ft_putstr_fd("vai alocar na tiny\n", 1);
		AllocationEntry *head = (AllocationEntry *) TINY__ZONE;
		while (head->in_use) {
			head += sizeof(AllocationEntry) + head->size;
		}
		// head tá no próximo endereço vazio
		head->in_use = TRUE;
		head->size = size;

		head += sizeof(AllocationEntry);
		int i = -1;
		while (((void **)LEDGER)[++i])
			continue ;
		((void **)LEDGER)[i] = head;
		return (head);
	}
	if (size <= SMALL_ZONE_THRESHOLD) {
		ft_putstr_fd("vai alocar na small\n", 1);
		AllocationEntry *head = (AllocationEntry *) SMALL__ZONE;
		while (head->in_use) {
			head += sizeof(AllocationEntry) + head->size;
		}
		// head tá no próximo endereço vazio
		head->in_use = TRUE;
		head->size = size;

		head += sizeof(AllocationEntry);
		int i = -1;
		while (((void **)LEDGER)[++i])
			continue ;
		((void **)LEDGER)[i] = head;
		return (head);
	}
	if (size > SMALL_ZONE_THRESHOLD) {
		ft_putstr_fd("tem que alocar na LARGE\n", 1);
		return NULL;
	}
	return NULL;
}

void	free(void *ptr)
{
	(void) ptr;
	// checar se o ptr passado de fato é um ponteiro que eu dei malloc.
	int i = -1;
	while (((void **)LEDGER)[++i]) {
		if (((void **)LEDGER)[i] == ptr) {
			// dar free
			AllocationEntry *metadata = ptr - sizeof(AllocationEntry);
			metadata->in_use = FALSE;
			ft_putstr_fd("Successful memory deallocation\n", 1);
			return ;
		}
	}

	// o ptr passado não é uma alocação que consta no meu Ledger.
	ft_putstr_fd("Free in invalid pointer\n", 1);
	return ;
}

void	*realloc(void *ptr, size_t size)
{
	(void) ptr;
	(void) size;
	return (NULL);
}

void show_alloc_mem()
{
	// write(1, "TINY : 0xA0000\n", ft_len("TINY : 0xA0000\n"));
	// write(1, "0xA0020 - 0xA004A : 42 bytes\n", ft_len("0xA0020 - 0xA004A : 42 bytes\n"));
	// write(1, "0xA006A - 0xA00BE : 84 bytes\n", ft_len("0xA006A - 0xA00BE : 84 bytes\n"));
	// write(1, "SMALL : 0xAD000\n", ft_len("SMALL : 0xAD000\n"));
	// write(1, "0xAD020 - 0xADEAD : 3725 bytes\n", ft_len("0xAD020 - 0xADEAD : 3725 bytes\n"));
	// write(1, "LARGE : 0xB0000\n", ft_len("LARGE : 0xB0000\n"));
	// write(1, "0xB0020 - 0xBBEEF : 48847 bytes\n", ft_len("0xB0020 - 0xBBEEF : 48847 bytes\n"));
	// write(1, "Total : 52698 bytes\n", ft_len("Total : 52698 bytes\n"));
	return ;
}
