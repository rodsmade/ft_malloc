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


void	**get_allocations_ledger(void) {
	static void **allocations_ledger = NULL;

	// memory dedicated to keeping track of allocations
	if (!allocations_ledger) {
		allocations_ledger = (void **) mmap(NULL, getpagesize(), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
		// ledger is of size 4096
		// it can hold 4096 / 8 = 512 pointers
		int i = -1;
		int total_amount_of_ptrs_ledger_can_hold = getpagesize() / sizeof(*allocations_ledger);
		while (++i < total_amount_of_ptrs_ledger_can_hold)
			allocations_ledger[i] = 0; //highly inefficient?
	}

	return allocations_ledger;
}

void	*page = NULL;
void	*heap_head = NULL;
long	total_allocd_bytes = 0;

void	*malloc(size_t size)
{
	if (total_allocd_bytes + size > getpagesize()) {
		printf("[error] allocation is bigger than available space\n");
		return (NULL);
	}
	// Actually allocated memory
	if (!page /* or if size is bigger than available space... */) {
		page = mmap(NULL, getpagesize(), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
		if (page == MAP_FAILED) // system failed to provide any mapped pages
			return (NULL);
		// page is of size 4096.
		// this means i can allocate 4096 bytes.
		heap_head = page;
	}


	void *newly_allocated_memory = heap_head;

	void **allocations_ledger = get_allocations_ledger();
	int i = -1;
	while (allocations_ledger[++i] && i < (getpagesize() / sizeof(*allocations_ledger)))
		continue;
	allocations_ledger[i] = newly_allocated_memory;

	heap_head = heap_head + size;
	total_allocd_bytes += size;

	return (newly_allocated_memory);
}

void	free(void *ptr)
{
	void **allocations_ledger = get_allocations_ledger();

	if (ptr) {
		int i = -1;
		while (++i < (getpagesize() / sizeof(*allocations_ledger))) {
			if (allocations_ledger[i] && allocations_ledger[i] == ptr) {
				printf("gotta free the following pointer `%p`\n", ptr);
				return ;
			}
		}
	}
	printf("pointer `%p` not found in the allocations ledger!!\n", ptr);
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
