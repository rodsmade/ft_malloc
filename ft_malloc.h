/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_malloc.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: roaraujo <roaraujo@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/31 17:18:54 by roaraujo          #+#    #+#             */
/*   Updated: 2024/06/03 23:13:51 by roaraujo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_MALLOC_H
# define FT_MALLOC_H

// STD LIBS
# include <sys/mman.h> // mmap(), munmap()
# include <unistd.h> // getpagesize()
# include <sys/time.h>
# include <sys/resource.h> // getrlimit()
# include <stdio.h> // perror()

// CUSTOM HEADERS
# include "libft.h"

// MACROS
# define TINY_ZONE_THRESHOLD 128
# define SMALL_ZONE_THRESHOLD 4096
# define MIN_NB_ENTRIES 1024
# define CUSTOM_MALLOC_UPPER_LIMIT 8589934592
# define TRUE 1
# define FALSE 0
#if defined(__APPLE__) && defined(__MACH__)
    # define PAGE_SIZE getpagesize()
#else
    # define PAGE_SIZE sysconf(_SC_PAGESIZE)
#endif

// ENUMS
typedef enum {
	__TINY,
	__SMALL,
	__LARGE,
	__LEDGER,
} e_tags;

// TYPEDEFS
typedef char bool;

// DATA STRUCTURES
typedef struct AllocationMetadata {
	void	*ptr;
	size_t	size;
	char	in_use;
} AllocationMetadata;

typedef struct GlobalData {
	void *ZONES[3];
	void *LEDGERS[3];
	size_t CAPACITIES[4];
} t_global_data;

// INLINE FUNCTIONS
static inline size_t get_tiny_zone_size_in_bytes() {
	static size_t tiny_zone_size = 0;
	if (tiny_zone_size == 0)
		tiny_zone_size = (((MIN_NB_ENTRIES * TINY_ZONE_THRESHOLD + PAGE_SIZE - 1) / PAGE_SIZE) * PAGE_SIZE);
	return tiny_zone_size;
}

static inline size_t get_small_zone_size_in_bytes() {
	static size_t small_zone_size = 0;
	if (small_zone_size == 0)
		small_zone_size = (((MIN_NB_ENTRIES * SMALL_ZONE_THRESHOLD + PAGE_SIZE - 1) / PAGE_SIZE) * PAGE_SIZE);
	return small_zone_size;
}

static inline size_t get_ledger_size_in_bytes() {
	static size_t ledger_size = 0;
	if (ledger_size == 0)
		ledger_size = (((MIN_NB_ENTRIES * sizeof(AllocationMetadata) + PAGE_SIZE - 1) / PAGE_SIZE) * PAGE_SIZE);
	return ledger_size;
}

static inline size_t get_max_rlimit_data() {
	static size_t max_rlimit_data = 0;
	if (max_rlimit_data == 0) {
		struct rlimit rlimits_data = {0};
		getrlimit(RLIMIT_DATA, &rlimits_data);
		max_rlimit_data  = rlimits_data.rlim_cur < CUSTOM_MALLOC_UPPER_LIMIT ? rlimits_data.rlim_cur : CUSTOM_MALLOC_UPPER_LIMIT;
	}
	return (max_rlimit_data);
}

// GLOBAL VARIABLE
extern t_global_data g_data;

// PROTOTYPES
void	free(void *ptr);
void	*malloc(size_t size);
void	*realloc(void *ptr, size_t size);
void	show_alloc_mem(void);

// UTILS
void	*allocate_in_zone(size_t size, e_tags zone);
void	*allocate_out_of_zone(size_t size);
bool	contains(void *array, void *ptr);
void	*pop(e_tags zone, void *ptr);
void	*push_to_back(void *array, void *ptr);
void	*safe_mmap(size_t size);

#endif
