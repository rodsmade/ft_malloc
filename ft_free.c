#include "ft_malloc.h"

void	free(void *ptr) {
	if (!ptr)
		return ;

	pthread_mutex_lock(&g_data.MUTEX);  // Lock the mutex

	// Check if pointer is registered in ledger
	int i = -1;
	t_ledger_entry *ledger = g_data.LEDGERS[__TINY];
	while (ledger[++i].ptr) {
		if (ledger[i].ptr == ptr) {
			// Mark as unused
			ledger[i].in_use = FALSE;
			pthread_mutex_unlock(&g_data.MUTEX);  // Unlock the mutex before returning
			return ;
		}
	}
	i = -1;
	ledger = g_data.LEDGERS[__SMALL];
	while (ledger[++i].ptr) {
		if (ledger[i].ptr == ptr) {
			// Mark as unused
			ledger[i].in_use = FALSE;
			pthread_mutex_unlock(&g_data.MUTEX);  // Unlock the mutex before returning
			return ;
		}
	}
	i = -1;
	while (((t_ledger_entry *) g_data.LEDGERS[__LARGE])[++i].ptr) {
		if (((t_ledger_entry *) g_data.LEDGERS[__LARGE])[i].ptr == ptr) {
			// Return memory to system
			munmap(((t_ledger_entry *) g_data.LEDGERS[__LARGE])[i].ptr, ((t_ledger_entry *) g_data.LEDGERS[__LARGE])[i].size);

			// Pop entry from ledger;
			pop(__LARGE, ptr);
			pthread_mutex_unlock(&g_data.MUTEX);  // Unlock the mutex before returning
			return ;
		}
	}

	// ptr has not been allocated by malloc
	ft_putstr_fd("Free in invalid pointer\n", 2);
	pthread_mutex_unlock(&g_data.MUTEX);  // Unlock the mutex before returning
	return ;
}
