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

t_global_data g_data = {
	.ZONES = { NULL, NULL, NULL },
	.LEDGERS = { NULL, NULL, NULL },
	.CAPACITIES = { 0x0, 0x0, 0x0, 0x0 }
};

// Constructor function
__attribute__((constructor))
void prologue() {
	// Initialize capacities
	g_data.CAPACITIES[__TINY] = get_tiny_zone_size();
	g_data.CAPACITIES[__SMALL] = get_small_zone_size();
	g_data.CAPACITIES[__LEDGER] = get_ledger_size();

	AllocationMetadata *entry;

	// Set up tiny zone
	g_data.ZONES[__TINY] = safe_mmap(g_data.CAPACITIES[__TINY]);
	entry = (AllocationMetadata *) g_data.ZONES[__TINY];
	entry->in_use = FALSE;
	entry->size = 0;

	// alloc SMALL zone
	g_data.ZONES[__SMALL] = safe_mmap(g_data.CAPACITIES[__SMALL]);
	entry = (AllocationMetadata *) g_data.ZONES[__SMALL];
	entry->in_use = FALSE;
	entry->size = 0;

	// LEDGER
	g_data.LEDGERS[__TINY] = safe_mmap(g_data.CAPACITIES[__LEDGER]);
	ft_bzero(g_data.LEDGERS[__TINY], g_data.CAPACITIES[__LEDGER]);
	g_data.LEDGERS[__SMALL] = safe_mmap(g_data.CAPACITIES[__LEDGER]);
	ft_bzero(g_data.LEDGERS[__SMALL], g_data.CAPACITIES[__LEDGER]);
	g_data.LEDGERS[__LARGE] = safe_mmap(g_data.CAPACITIES[__LEDGER]);
	ft_bzero(g_data.LEDGERS[__LARGE], g_data.CAPACITIES[__LEDGER]);
}

void	*malloc(size_t size) {
	void *ptr = NULL;

	if (size > get_max_rlimit_data()) // exceeds single allocation size limit
		return ptr;

	if (size <= TINY_ZONE_THRESHOLD)
		ptr = allocate_in_zone(size, __TINY);
	if (size > TINY_ZONE_THRESHOLD && size <= SMALL_ZONE_THRESHOLD)
		ptr = allocate_in_zone(size, __SMALL);
	if (size > SMALL_ZONE_THRESHOLD)
		ptr = allocate_out_of_zone(size);

	return (ptr);
}

void	free(void *ptr) {
	if (!ptr)
		return ;

	// checar se o ptr passado de fato é um ponteiro que eu dei malloc.
	int i = -1;
	while (((void **)g_data.LEDGERS[__TINY])[++i]) {
		if (((void **)g_data.LEDGERS[__TINY])[i] == ptr) {
			// dar free
			AllocationMetadata *metadata = ptr - sizeof(AllocationMetadata);
			metadata->in_use = FALSE;
			pop(g_data.LEDGERS[__TINY], ptr);
			return ;
		}
	}
	i = -1;
	while (((void **)g_data.LEDGERS[__SMALL])[++i]) {
		if (((void **)g_data.LEDGERS[__SMALL])[i] == ptr) {
			// dar free
			AllocationMetadata *metadata = ptr - sizeof(AllocationMetadata);
			metadata->in_use = FALSE;
			pop(g_data.LEDGERS[__SMALL], ptr);
			return ;
		}
	}
	i = -1;
	while (((void **)g_data.LEDGERS[__LARGE])[++i]) {
		if (((void **)g_data.LEDGERS[__LARGE])[i] == ptr) {
			// dar free
			void *allocation_head = (void *)ptr - sizeof(AllocationMetadata);
			size_t alloc_size = ((AllocationMetadata *)allocation_head)->size;

			// return memory to system
			munmap(allocation_head, sizeof(AllocationMetadata) + alloc_size);

			// remove entry from ledger
			g_data.LEDGERS[__LARGE] = pop(g_data.LEDGERS[__LARGE], ptr);

			return ;
		}
	}

	// o ptr passado não é uma alocação que consta em nenhum Ledger.
	ft_putstr_fd("Free in invalid pointer\n", 2);
	return ;
}

void	*realloc(void *ptr, size_t size) {
	void *rptr = NULL;

	if (ptr && !size)
		free(ptr);

	if (!ptr) {
		rptr = malloc(size);
	}

	if (ptr && size) {
		if (contains(g_data.LEDGERS[__TINY], ptr)
			|| contains(g_data.LEDGERS[__SMALL], ptr)
			|| contains(g_data.LEDGERS[__LARGE], ptr)) {
			free(ptr);
			rptr = malloc(size);
		} else {
			free(ptr);
		}
	}

	return (rptr);
}

void show_alloc_mem(void) {
	AllocationMetadata *head;
	int total = 0;

	ft_putendl_fd("=================================================", 1);
	ft_putendl_fd("=               MEMORY LAYOUT                   =", 1);
	ft_putendl_fd("=================================================", 1);

	// Print allocations in TINY_
	ft_putstr_fd("TINY : ", 1);
	ft_putptr_fd(g_data.ZONES[__TINY], 1);
	ft_putchar_fd('\n', 1);

	head = (AllocationMetadata *) g_data.ZONES[__TINY];
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
	ft_putptr_fd(g_data.ZONES[__SMALL], 1);
	ft_putchar_fd('\n', 1);

	head = (AllocationMetadata *) g_data.ZONES[__SMALL];
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
	ft_putptr_fd(g_data.LEDGERS[__LARGE], 1);
	ft_putchar_fd('\n', 1);
	void **largeEntry = g_data.LEDGERS[__LARGE];
	int i = -1;
	while (largeEntry[++i] != NULL) {
		ft_putptr_fd((void *) largeEntry[i], 1);
		ft_putstr_fd(" - ", 1);
		size_t alloc_size = ((AllocationMetadata *)(((void *) largeEntry[i]) - sizeof(AllocationMetadata)))->size;
		ft_putptr_fd((void *) largeEntry[i] + alloc_size, 1);
		ft_putstr_fd(" : ", 1);
		ft_putnbr_fd(alloc_size, 1);
		total += alloc_size;
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
	munmap(g_data.ZONES[__TINY], get_tiny_zone_size());
	munmap(g_data.ZONES[__SMALL], get_small_zone_size());

	// Go through large allocs ledgers and unmap
	for (int i = 0; ((void **)g_data.LEDGERS[__LARGE])[i]; i++) {
		void *ptr = ((void **)g_data.LEDGERS[__LARGE])[i];
		void *allocation_head = (void *)ptr - sizeof(AllocationMetadata);
		size_t alloc_size = ((AllocationMetadata *)allocation_head)->size;
		munmap(ptr, alloc_size);
	}
}