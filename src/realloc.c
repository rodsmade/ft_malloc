#include "ft_malloc.h"

void	*realloc(void *ptr, size_t size) {
	pthread_mutex_lock(&g_data.MUTEX);  // Lock the mutex for thread safety

	if (ptr && !size) {
		pthread_mutex_unlock(&g_data.MUTEX);  // Unlock the mutex after realloc
		return (free(ptr), NULL);
	}

	if (!ptr) {
		pthread_mutex_unlock(&g_data.MUTEX);  // Unlock the mutex after realloc
		return (malloc(size));
	}

	void *rptr = NULL;

	size_t old_size = -1;
	if (contains(g_data.LEDGERS[__TINY], ptr))
		old_size = get_entry(__TINY, ptr)->size;
	if (contains(g_data.LEDGERS[__SMALL], ptr))
		old_size = get_entry(__SMALL, ptr)->size;
	if (contains(g_data.LEDGERS[__LARGE], ptr))
		old_size = get_entry(__LARGE, ptr)->size;
	
	if (old_size == (size_t) -1) {
		pthread_mutex_unlock(&g_data.MUTEX);  // Unlock the mutex after realloc
		return (ft_putstr_fd("Free in invalid pointer\n", 2), rptr);
	}

	if (size > old_size) {
		rptr = malloc(size);
		ft_memcpy(rptr, ptr, old_size);
		free(ptr);
	} else {
		free(ptr);
		rptr = malloc(size);
	}

	pthread_mutex_unlock(&g_data.MUTEX);  // Unlock the mutex after realloc
	return (rptr);
}
