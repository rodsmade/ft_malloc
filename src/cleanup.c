#include "ft_malloc.h"

void epilogue() {
	pthread_mutex_lock(&g_data.MUTEX);

	munmap(g_data.ZONES[__TINY], get_tiny_zone_size_in_bytes());
	munmap(g_data.ZONES[__SMALL], get_small_zone_size_in_bytes());

	// Go through large allocs ledgers and unmap
	t_ledger_entry *ledger = g_data.LEDGERS[__LARGE];
	for (int i = 0; (i < MIN_NB_ENTRIES && ledger[0].ptr); i++) {
		munmap(ledger[i].ptr, ledger[i].size);
	}

	// dar unmap nos ledgers ?????
	munmap(g_data.LEDGERS[__TINY], g_data.CAPACITIES[__LEDGER]);
	munmap(g_data.LEDGERS[__SMALL], g_data.CAPACITIES[__LEDGER]);
	munmap(g_data.LEDGERS[__LARGE], g_data.CAPACITIES[__LEDGER]);

	pthread_mutex_unlock(&g_data.MUTEX);

	pthread_mutex_destroy(&g_data.MUTEX);
}
