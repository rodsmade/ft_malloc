/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_malloc.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: roaraujo <roaraujo@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/31 17:18:59 by roaraujo          #+#    #+#             */
/*   Updated: 2024/06/03 23:16:36 by roaraujo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc.h"

// new calls to mmap need to follow the available in getrlimit, even if it's infinite. remember you're not using sbreak 

void *TINY__ZONE = NULL;
void *SMALL__ZONE = NULL;
void *LARGE__ZONE = NULL;

void *LEDGER = NULL;
void *LARGE_ALLOCS_LEDGER = NULL;

// Constructor function
__attribute__((constructor))
void prologue() {
	AllocationMetadata *entry;

	// alloc TINY zone
	// initially one page only
	TINY__ZONE = mmap(NULL, get_tiny_zone_size(), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
	entry = (AllocationMetadata *) TINY__ZONE;
	entry->in_use = FALSE;
	entry->size = 0;

	// alloc SMALL zone
	// initially one page only
	SMALL__ZONE = mmap(NULL, get_small_zone_size(), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
	entry = (AllocationMetadata *) SMALL__ZONE;
	entry->in_use = FALSE;
	entry->size = 0;

	// LEDGER
	// one page only
	LEDGER = mmap(NULL, 1 * getpagesize(), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
	unsigned long i = -1;
	while (++i < getpagesize() / sizeof(void *))
		((void **)LEDGER)[i] = NULL;

	// LARGE LEDGER
	// one page only
	LARGE_ALLOCS_LEDGER = mmap(NULL, 1 * getpagesize(), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
	i = -1;
	while (++i < getpagesize() / sizeof(void *))
		((void **)LARGE_ALLOCS_LEDGER)[i] = NULL;
}

void *push_to_back(void *array, void *ptr) {
	int i = -1;
	while (((void **)array)[++i])
		continue ;
	((void **)array)[i] = ptr;
	return (array);
}

void	*allocate_ptr(size_t size, e_zone zone) {
	void *zone_start = NULL;
	switch (zone) {
		case TINY:
				zone_start = TINY__ZONE; break;
		case SMALL:
				zone_start = SMALL__ZONE; break;
		default:
				return NULL; break;
	}
	AllocationMetadata *entry = zone_start;
	while (entry->in_use) {
		entry += sizeof(AllocationMetadata) + entry->size;
	}
	// Marks memory chunk as used, next chunk as unused
	entry->in_use = TRUE;
	entry->size = size;
	// aqui potencialmente pode dar ruim qnd chegar no final da tiny/página
	AllocationMetadata *next_entry = (void *) entry + sizeof(AllocationMetadata) + size;
	next_entry->in_use = FALSE;

	void *ptr = (void *) entry + sizeof(AllocationMetadata);

	// Add new allocation ptr to ledger
	LEDGER = push_to_back(LEDGER, ptr);
	return (ptr);
}

void	*malloc(size_t size)
{
	void *ptr = NULL;

	if (size <= TINY_ZONE_THRESHOLD)
		ptr = allocate_ptr(size, TINY);
	if (size > TINY_ZONE_THRESHOLD && size <= SMALL_ZONE_THRESHOLD)
		ptr = allocate_ptr(size, SMALL);
	if (size > SMALL_ZONE_THRESHOLD) {
		void *chunk = mmap(NULL, 1 * getpagesize(), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);

		int i = -1;
		while (((LargeAllocationMetadata *) LARGE_ALLOCS_LEDGER)[++i].address != NULL)
			continue ;
		((LargeAllocationMetadata *) LARGE_ALLOCS_LEDGER)[i].address = chunk;
		((LargeAllocationMetadata *) LARGE_ALLOCS_LEDGER)[i].size = size;
		ptr = chunk;
	}
	return (ptr);
}

void	free(void *ptr)
{
	if (!ptr)
		return ;

	// checar se o ptr passado de fato é um ponteiro que eu dei malloc.
	int i = -1;
	while (((void **)LEDGER)[++i]) {
		if (((void **)LEDGER)[i] == ptr) {
			// dar free
			AllocationMetadata *metadata = ptr - sizeof(AllocationMetadata);
			metadata->in_use = FALSE;
			return ;
		}
	}

	// checar se o ptr passado tá no LARGE_ALLOCS_LEDGER
	i = -1;
	while (((LargeAllocationMetadata *)LARGE_ALLOCS_LEDGER)[++i].address) {
		if (((LargeAllocationMetadata *)LARGE_ALLOCS_LEDGER)[i].address == ptr) {
			munmap(ptr, ((LargeAllocationMetadata *)LARGE_ALLOCS_LEDGER)[++i].size);
			return ;
		}
	}

	// // o ptr passado não é uma alocação que consta no meu Ledger.
	// ft_putstr_fd("Free in invalid pointer\n", 2);
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
	AllocationMetadata *head;
	int total = 0;

	ft_putendl_fd("=================================================", 1);
	ft_putendl_fd("=               MEMORY LAYOUT                   =", 1);
	ft_putendl_fd("=================================================", 1);

	// Print allocations in TINY_
	ft_putstr_fd("TINY : ", 1);
	ft_putptr_fd(TINY__ZONE, 1);
	ft_putchar_fd('\n', 1);

	head = (AllocationMetadata *) TINY__ZONE;
	while (head->in_use) {
		ft_putptr_fd((void *) head + sizeof(AllocationMetadata), 1);
		ft_putstr_fd(" - ", 1);
		ft_putptr_fd((void *) head + sizeof(AllocationMetadata) + head->size, 1);
		ft_putstr_fd(" : ", 1);
		ft_putnbr_fd(head->size, 1);
		total += head->size;
		ft_putendl_fd(" bytes", 1);
		head += sizeof(AllocationMetadata) + head->size;
	}

	// Print allocations in SMALL_
	ft_putstr_fd("SMALL : ", 1);
	ft_putptr_fd(SMALL__ZONE, 1);
	ft_putchar_fd('\n', 1);

	head = (AllocationMetadata *) SMALL__ZONE;
	while (head->in_use) {
		ft_putptr_fd((void *) head + sizeof(AllocationMetadata), 1);
		ft_putstr_fd(" - ", 1);
		ft_putptr_fd((void *) head + sizeof(AllocationMetadata) + head->size, 1);
		ft_putstr_fd(" : ", 1);
		ft_putnbr_fd(head->size, 1);
		total += head->size;
		ft_putendl_fd(" bytes", 1);
		head += sizeof(AllocationMetadata) + head->size;
	}

	// Print allocations in LARGE_
	ft_putstr_fd("LARGE : ", 1);
	ft_putptr_fd(LARGE_ALLOCS_LEDGER, 1);
	ft_putchar_fd('\n', 1);
	LargeAllocationMetadata *largeEntry = LARGE_ALLOCS_LEDGER;
	int i = -1;
	while (largeEntry[++i].address != NULL) {
		ft_putptr_fd((void *) largeEntry[i].address, 1);
		ft_putstr_fd(" - ", 1);
		ft_putptr_fd((void *) largeEntry[i].address + largeEntry[i].size, 1);
		ft_putstr_fd(" : ", 1);
		ft_putnbr_fd(largeEntry[i].size, 1);
		total += largeEntry[i].size;
		ft_putendl_fd(" bytes", 1);
	}

	ft_putstr_fd("Total : ", 1);
	ft_putnbr_fd(total, 1);
	ft_putendl_fd(" bytes", 1);

	ft_putendl_fd("=================================================", 1);
	return ;
}

void epilogue() __attribute__((destructor));

void epilogue() {
	munmap(TINY__ZONE, get_tiny_zone_size());
	munmap(SMALL__ZONE, get_small_zone_size());
}