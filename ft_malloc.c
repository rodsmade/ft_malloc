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

void *safe_mmap(int size) {
	void *allocation = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
	if (allocation == MAP_FAILED) {
		perror("call to mmap failed");
		exit(EXIT_FAILURE);
	}
	return (allocation);
}

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

void *push_to_back(void *array, void *ptr) {
	int i = -1;
	while (((void **)array)[++i])
		continue ;
	((void **)array)[i] = ptr;
	return (array);
}

void *pop(void *array, void *ptr) {
	int i = -1;
	while (((void **)array)[++i] && ((void **)array)[i] != ptr)
		;

	if (((void **)array)[i]) {
		while (((void **)array)[i + 1]) {
			((void **)array)[i] = ((void **)array)[i + 1];
			i++;
		}
		((void **)array)[i] = NULL;
	}

	return (array);
}

void	*allocate_ptr(size_t size, e_tags zone) {
	void *zone_start = NULL;
	switch (zone) {
		case __TINY:
				zone_start = g_data.ZONES[__TINY]; break;
		case __SMALL:
				zone_start = g_data.ZONES[__SMALL]; break;
		default:
				return NULL; break;
	}
	AllocationMetadata *entry = zone_start;
	size_t current_allocated_size = 0;
	while (entry->in_use) {
		// Checks for available space
		current_allocated_size += entry->size + sizeof(AllocationMetadata);
		if (current_allocated_size + size > g_data.CAPACITIES[zone])
			return (NULL);

		entry = (void *) entry + sizeof(AllocationMetadata) + entry->size;

		// Checks for the possibility to reuse previously allocated chunks
		if (!entry->in_use && entry->size >= size) {
			break ;
		}
	}
	// Marks memory chunk as used
	entry->in_use = TRUE;
	entry->size = size;

	void *ptr = (void *) entry + sizeof(AllocationMetadata);

	// Add new allocation ptr to ledger
	g_data.LEDGERS[zone] = push_to_back(g_data.LEDGERS[zone], ptr);
	return (ptr);
}

void	*malloc(size_t size)
{
	void *ptr = NULL;

	if (size > get_max_rlimit_data())
		return ptr;
	if (size <= TINY_ZONE_THRESHOLD)
		ptr = allocate_ptr(size, __TINY);
	if (size > TINY_ZONE_THRESHOLD && size <= SMALL_ZONE_THRESHOLD)
		ptr = allocate_ptr(size, __SMALL);
	if (size > SMALL_ZONE_THRESHOLD) {
		void *chunk = mmap(NULL, size + sizeof(LargeAllocationMetadata), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
		if (chunk) {
			// Fills in metadata
			((LargeAllocationMetadata *) chunk)->size = size;

			// Push pointer to g_data.LEDGERS[__LARGE]
			ptr = (void *) chunk + sizeof(LargeAllocationMetadata);
			g_data.LEDGERS[__LARGE] = push_to_back(g_data.LEDGERS[__LARGE], ptr);
		}
	}
	return (ptr);
}

bool contains(void *array, void *ptr) {
	for (int i = 0; ((void **)array)[i]; i++) {
		if (((void **)array)[i] == ptr)
			return TRUE;
	}
	return FALSE;
}

void	free(void *ptr)
{
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
			void *allocation_head = (void *)ptr - sizeof(LargeAllocationMetadata);
			size_t alloc_size = ((LargeAllocationMetadata *)allocation_head)->size;

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

void	*realloc(void *ptr, size_t size)
{
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

void show_alloc_mem()
{
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
		size_t alloc_size = ((LargeAllocationMetadata *)(((void *) largeEntry[i]) - sizeof(LargeAllocationMetadata)))->size;
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
		void *allocation_head = (void *)ptr - sizeof(LargeAllocationMetadata);
		size_t alloc_size = ((LargeAllocationMetadata *)allocation_head)->size;
		munmap(ptr, alloc_size);
	}
}