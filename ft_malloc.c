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

	// Check if pointer is registered in ledger
	int i = -1;
	AllocationMetadata *ledger = g_data.LEDGERS[__TINY];
	while (ledger[++i].ptr) {
		if (ledger[i].ptr == ptr) {
			// Mark as unused
			ledger[i].in_use = FALSE;
			return ;
		}
	}
	i = -1;
	ledger = g_data.LEDGERS[__SMALL];
	while (ledger[++i].ptr) {
		if (ledger[i].ptr == ptr) {
			// Mark as unused
			ledger[i].in_use = FALSE;
			return ;
		}
	}
	i = -1;
	while (((AllocationMetadata *) g_data.LEDGERS[__LARGE])[++i].ptr) {
		if (((AllocationMetadata *) g_data.LEDGERS[__LARGE])[i].ptr == ptr) {
			// return memory to system
			munmap(((AllocationMetadata *) g_data.LEDGERS[__LARGE])[i].ptr, ((AllocationMetadata *) g_data.LEDGERS[__LARGE])[i].size);

			// pop entry from ledger;
			pop(__LARGE, ptr);
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