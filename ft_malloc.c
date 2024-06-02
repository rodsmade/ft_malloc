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

// new calls to mmap need to follow the available in getrlimit, even if it's infinite. remember you're not using sbreak 

void *TINY__ZONE = NULL;
void *SMALL__ZONE = NULL;
void *LARGE__ZONE = NULL;

void *LEDGER = NULL;

// Constructor function
__attribute__((constructor))
void mylib_init() {
	AllocationEntry *entry;

	// alloc TINY zone
	// initially one page only
	TINY__ZONE = mmap(NULL, 4 * getpagesize(), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
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
		AllocationEntry *head = (AllocationEntry *) TINY__ZONE;
		while (head->in_use) {
			head += sizeof(AllocationEntry) + head->size;
		}
		head->in_use = TRUE;
		head->size = size;
		((AllocationEntry *) head + sizeof(AllocationEntry) + size)->in_use = FALSE;

		head += sizeof(AllocationEntry);
		int i = -1;
		while (((void **)LEDGER)[++i])
			continue ;
		((void **)LEDGER)[i] = head;
		return (head);
	}
	if (size > TINY_ZONE_THRESHOLD && size <= SMALL_ZONE_THRESHOLD) {
		AllocationEntry *head = (AllocationEntry *) SMALL__ZONE;
		while (head->in_use) {
			head += sizeof(AllocationEntry) + head->size;
		}
		head->in_use = TRUE;
		head->size = size;
		((AllocationEntry *) head + sizeof(AllocationEntry) + size)->in_use = FALSE;

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
			return ;
		}
	}

	// o ptr passado não é uma alocação que consta no meu Ledger.
	ft_putstr_fd("Free in invalid pointer\n", 2);
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
	AllocationEntry *head;
	int total = 0;

	ft_putendl_fd("=================================================", 1);
	ft_putendl_fd("=               MEMORY LAYOUT                   =", 1);
	ft_putendl_fd("=================================================", 1);

	// Print allocations in TINY__ZONE
	ft_putstr_fd("TINY : ", 1);
	ft_putptr_fd(TINY__ZONE, 1);
	ft_putchar_fd('\n', 1);

	head = (AllocationEntry *) TINY__ZONE;
	while (head->in_use) {
		ft_putptr_fd((void *) head + sizeof(AllocationEntry), 1);
		ft_putstr_fd(" - ", 1);
		ft_putptr_fd((void *) head + sizeof(AllocationEntry) + head->size, 1);
		ft_putstr_fd(" : ", 1);
		ft_putnbr_fd(head->size, 1);
		total += head->size;
		ft_putendl_fd(" bytes", 1);
		head += sizeof(AllocationEntry) + head->size;
	}

	// Print allocations in SMALL__ZONE
	ft_putstr_fd("SMALL : ", 1);
	ft_putptr_fd(SMALL__ZONE, 1);
	ft_putchar_fd('\n', 1);

	head = (AllocationEntry *) SMALL__ZONE;
	while (head->in_use) {
		ft_putptr_fd((void *) head + sizeof(AllocationEntry), 1);
		ft_putstr_fd(" - ", 1);
		ft_putptr_fd((void *) head + sizeof(AllocationEntry) + head->size, 1);
		ft_putstr_fd(" : ", 1);
		ft_putnbr_fd(head->size, 1);
		total += head->size;
		ft_putendl_fd(" bytes", 1);
		head += sizeof(AllocationEntry) + head->size;
	}

	// Print allocations in LARGE__ZONE
	ft_putstr_fd("LARGE : ", 1);
	ft_putptr_fd(LARGE__ZONE, 1);
	ft_putchar_fd('\n', 1);

	ft_putstr_fd("Total : ", 1);
	ft_putnbr_fd(total, 1);
	ft_putendl_fd(" bytes", 1);

	ft_putendl_fd("=================================================", 1);
	return ;
}
