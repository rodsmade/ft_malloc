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

// Constructor function
__attribute__((constructor))
void mylib_init() {
    write(1, "|||||||||||||||| Library initialized!\n", 38);
}

void	**get_allocations_ledger(void) {
	static void **allocations_ledger = NULL;

	// memory dedicated to keeping track of allocations
	if (!allocations_ledger) {
		allocations_ledger = (void **) mmap(NULL, getpagesize(), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
		// ledger is of size 4096
		// it can hold 4096 / 8 = 512 pointers
		size_t i = -1;
		size_t total_amount_of_ptrs_ledger_can_hold = getpagesize() / sizeof(*allocations_ledger);
		while (++i < total_amount_of_ptrs_ledger_can_hold)
			allocations_ledger[i] = 0; //highly inefficient?
	}

	return allocations_ledger;
}

void	*page = NULL;
void	*heap_head = NULL;
size_t	total_allocd_bytes = 0;

void	*malloc(size_t size)
{
	ft_putstr_fd(">>>>>>>>>> entrou malloc\n", 1);
	ft_putstr_fd("total_allocd_bytes: ", 1);
	ft_putnbr_fd(total_allocd_bytes, 1);
	ft_putchar_fd('\n', 1);
	ft_putstr_fd("size to be alloc'd: ", 1);
	ft_putnbr_fd(size, 1);
	ft_putchar_fd('\n', 1);
	if (total_allocd_bytes + size > (size_t) getpagesize()) {
		ft_putstr_fd("[error] allocation is bigger than available space\n", 2);
		return (NULL);
	}
	// Actually allocated memory
	if (!page /* or if size is bigger than available space... */) {

		// // check system limits (max data a pgm can have)
		// struct rlimit limit;
		// // Get the resource limit for memory usage
		// if (getrlimit(RLIMIT_DATA, &limit) == 0) {
		// 	// Print the soft and hard limits
		// 	ft_putstr_fd("Soft limit for data segment: ", 1);
		// 	ft_putnbr_fd(limit.rlim_cur, 1);
		// 	ft_putchar_fd('\n', 1);
		// 	ft_putstr_fd("Hard limit for data segment: ", 1);
		// 	ft_putnbr_fd(limit.rlim_max, 1);
		// 	ft_putchar_fd('\n', 1);
		// } else {
		// 	return (NULL);
		// }

		page = mmap(NULL, getpagesize(), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
		if (page == MAP_FAILED) // system failed to provide any mapped pages
			return (NULL);
		// page is of size 4096.
		// this means i can allocate 4096 bytes.
		heap_head = page;
	}


	void *newly_allocated_memory = heap_head;

	void **allocations_ledger = get_allocations_ledger();
	size_t i = -1;
	while (allocations_ledger[++i] && i < ((size_t) getpagesize() / sizeof(*allocations_ledger)))
		continue;
	allocations_ledger[i] = newly_allocated_memory;

	heap_head = heap_head + size;
	total_allocd_bytes += size;

	return (newly_allocated_memory);
}

void	free(void *ptr)
{
	ft_putstr_fd(">>>>>>>>>> entrou free\n", 1);
	void **allocations_ledger = get_allocations_ledger();

	if (ptr) {
		size_t i = -1;
		while (++i < (getpagesize() / sizeof(*allocations_ledger))) {
			if (allocations_ledger[i] && allocations_ledger[i] == ptr) {
				ft_putstr_fd("gotta free the following pointer `", 1);
				ft_putptr_fd(ptr, 1);
				ft_putstr_fd("`\n", 1);
				return ;
			}
		}
	}
	ft_putstr_fd("pointer `", 1);
	ft_putptr_fd(ptr, 1);
	ft_putstr_fd("` not found in the allocations ledger!!\n", 1);
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
