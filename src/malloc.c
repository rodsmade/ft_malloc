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

// Global variable
t_global_data g_data = {
	.ZONES = { NULL, NULL, NULL },
	.LEDGERS = { NULL, NULL, NULL },
	.CAPACITIES = { 0x0, 0x0, 0x0, 0x0 }
};

static void *allocate_in_zone(size_t size, e_tags zone) {
	t_ledger_entry *ledger = g_data.LEDGERS[zone];

	size_t currently_allocated_size = 0;
	int i = -1;
	while (ledger[++i].ptr) {
		// Checks for available space
		currently_allocated_size += ledger[i].size;
		if (currently_allocated_size + size > g_data.CAPACITIES[zone])
			return (NULL);

		// Checks for the possibility to reuse previously allocated chunks
		if (!ledger[i].in_use && ledger[i].size >= size) {
			ledger[i].in_use = TRUE;
			return (ledger[i].ptr);
		}
	}
	// Marks memory chunk as used
	ledger[i].ptr = (void *) g_data.ZONES[zone] + currently_allocated_size;
	ledger[i].in_use = TRUE;
	ledger[i].size = size;

	return (ledger[i].ptr);
}

static void *allocate_out_of_zone(size_t size) {
	void *chunk = safe_mmap(size);
	if (chunk) {
		t_ledger_entry *ledger_entry = g_data.LEDGERS[__LARGE];
		// Find next empty ledger entry
		int i = -1;
		while (ledger_entry[++i].in_use)
			continue ;
		// Add new entry to ledger
		ledger_entry[i].ptr = chunk;
		ledger_entry[i].in_use = TRUE;
		ledger_entry[i].size = size;
	}
	return (chunk);
}

void	*malloc(size_t size) {
	void *ptr = NULL;

	pthread_mutex_lock(&g_data.MUTEX);

	if (size > get_max_rlimit_data()) {  // exceeds single allocation size limit
		pthread_mutex_unlock(&g_data.MUTEX);  // Unlock the mutex before returning
		return ptr;
	}

	if (size <= TINY_ZONE_THRESHOLD)
		ptr = allocate_in_zone(size, __TINY);
	if (size > TINY_ZONE_THRESHOLD && size <= SMALL_ZONE_THRESHOLD)
		ptr = allocate_in_zone(size, __SMALL);
	if (size > SMALL_ZONE_THRESHOLD)
		ptr = allocate_out_of_zone(size);

	pthread_mutex_unlock(&g_data.MUTEX);  // Unlock the mutex

	return (ptr);
}
