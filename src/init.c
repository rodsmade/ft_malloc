#include "ft_malloc.h"

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

	// Mutex
	if (pthread_mutex_init(&g_data.MUTEX, NULL) != 0) {
		// Handle error
		perror("call to pthread_mutex_init failed");
		exit(EXIT_FAILURE);
	}
}
