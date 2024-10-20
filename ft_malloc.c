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
	g_data.CAPACITIES[__TINY] = get_tiny_zone_size_in_bytes();
	g_data.CAPACITIES[__SMALL] = get_small_zone_size_in_bytes();
	g_data.CAPACITIES[__LEDGER] = get_ledger_size_in_bytes();

	t_ledger_entry *entry;

	// Set up tiny zone
	g_data.ZONES[__TINY] = safe_mmap(g_data.CAPACITIES[__TINY]);
	entry = (t_ledger_entry *) g_data.ZONES[__TINY];
	entry->in_use = FALSE;
	entry->size = 0;

	// alloc SMALL zone
	g_data.ZONES[__SMALL] = safe_mmap(g_data.CAPACITIES[__SMALL]);
	entry = (t_ledger_entry *) g_data.ZONES[__SMALL];
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
	t_ledger_entry *ledger = g_data.LEDGERS[__TINY];
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
	while (((t_ledger_entry *) g_data.LEDGERS[__LARGE])[++i].ptr) {
		if (((t_ledger_entry *) g_data.LEDGERS[__LARGE])[i].ptr == ptr) {
			// return memory to system
			munmap(((t_ledger_entry *) g_data.LEDGERS[__LARGE])[i].ptr, ((t_ledger_entry *) g_data.LEDGERS[__LARGE])[i].size);

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

	if (ptr && !size)
		return (free(ptr), NULL);

	if (!ptr) {
		return (malloc(size));
	}

	void *rptr = NULL;

	size_t old_size = -1;
	if (contains(g_data.LEDGERS[__TINY], ptr))
		old_size = get_entry(__TINY, ptr).size;
	if (contains(g_data.LEDGERS[__SMALL], ptr))
		old_size = get_entry(__SMALL, ptr).size;
	if (contains(g_data.LEDGERS[__LARGE], ptr))
		old_size = get_entry(__LARGE, ptr).size;
	
	if (old_size == (size_t) -1)
		return (ft_putstr_fd("Free in invalid pointer\n", 2), rptr);

	if (size > old_size) {
		rptr = malloc(size);
		ft_memcpy(rptr, ptr, old_size);
		free(ptr);
	} else {
		free(ptr);
		rptr = malloc(size);
	}

	return (rptr);
}

void show_alloc_mem(void) {

	ft_putendl_fd("=================================================", 1);
	ft_putendl_fd("=               MEMORY LAYOUT                   =", 1);
	ft_putendl_fd("=================================================", 1);

	t_ledger_entry *head;
	size_t total_allocated_bytes = 0;

	// Print allocations in tiny zone
	ft_putstr_fd("TINY : ", 1);
	ft_putptr_fd(g_data.ZONES[__TINY], 1);
	ft_putchar_fd('\n', 1);
	head = g_data.LEDGERS[__TINY];
	for (size_t i = 0; (i < MIN_NB_ENTRIES && head[0].ptr); i++) {
		if (head[i].in_use) {
			ft_putptr_fd((void *) head[i].ptr, 1);
			ft_putstr_fd(" - ", 1);
			ft_putptr_fd((void *) head[i].ptr +  head[i].size, 1);
			ft_putstr_fd(" : ", 1);
			ft_putnbr_fd(head[i].size, 1);
			ft_putendl_fd(" bytes", 1);
			total_allocated_bytes += head[i].size;
		}
	}
	// Print allocations in small zone
	ft_putstr_fd("SMALL : ", 1);
	ft_putptr_fd(g_data.ZONES[__SMALL], 1);
	ft_putchar_fd('\n', 1);
	head = g_data.LEDGERS[__SMALL];
	for (size_t i = 0; (i < MIN_NB_ENTRIES && head[0].ptr); i++) {
		if (head[i].in_use) {
			ft_putptr_fd((void *) head[i].ptr, 1);
			ft_putstr_fd(" - ", 1);
			ft_putptr_fd((void *) head[i].ptr +  head[i].size, 1);
			ft_putstr_fd(" : ", 1);
			ft_putnbr_fd(head[i].size, 1);
			ft_putendl_fd(" bytes", 1);
			total_allocated_bytes += head[i].size;
		}
	}
	// Print allocations in tiny zone
	ft_putstr_fd("LARGE : ", 1);
	ft_putptr_fd(g_data.ZONES[__LARGE], 1);
	ft_putchar_fd('\n', 1);
	head = g_data.LEDGERS[__LARGE];
	for (size_t i = 0; (i < MIN_NB_ENTRIES && head[0].ptr); i++) {
		if (head[i].in_use) {
			ft_putptr_fd((void *) head[i].ptr, 1);
			ft_putstr_fd(" - ", 1);
			ft_putptr_fd((void *) head[i].ptr +  head[i].size, 1);
			ft_putstr_fd(" : ", 1);
			ft_putnbr_fd(head[i].size, 1);
			ft_putendl_fd(" bytes", 1);
			total_allocated_bytes += head[i].size;
		}
	}

	ft_putstr_fd("Total : ", 1);
	ft_putnbr_fd(total_allocated_bytes, 1);
	ft_putendl_fd(" bytes", 1);

	ft_putendl_fd("=================================================", 1);
	return ;
}

void epilogue() __attribute__((destructor));

void epilogue() {
	munmap(g_data.ZONES[__TINY], get_tiny_zone_size_in_bytes());
	munmap(g_data.ZONES[__SMALL], get_small_zone_size_in_bytes());

	// Go through large allocs ledgers and unmap
	for (int i = 0; ((void **)g_data.LEDGERS[__LARGE])[i]; i++) {
		void *ptr = ((void **)g_data.LEDGERS[__LARGE])[i];
		void *allocation_head = (void *)ptr - sizeof(t_ledger_entry);
		size_t alloc_size = ((t_ledger_entry *)allocation_head)->size;
		munmap(ptr, alloc_size);
	}
}