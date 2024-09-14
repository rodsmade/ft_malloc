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

// CUSTOM HEADERS
# include "libft.h"

// DEBUGGING
# include <stdio.h>

// GLOBAL VARIABLES FOR TESTING
extern void *ZONES[3];
extern void *ALLOCATIONS_LEDGER;
extern void *LARGE_ALLOCS_LEDGER;

// MACROS
# define TINY_ZONE_THRESHOLD 128
# define SMALL_ZONE_THRESHOLD 4096
# define CUSTOM_MALLOC_UPPER_LIMIT 8589934592
# define TRUE 1
# define FALSE 0

#if defined(__APPLE__) && defined(__MACH__)
    # define PAGE_SIZE getpagesize()
#else
    # define PAGE_SIZE sysconf(_SC_PAGESIZE)
#endif

// Define the enum
typedef enum {
	__TINY,
	__SMALL,
	__LARGE,
	__LEDGER,
} e_tags;

typedef char bool;

// INLINE FUNCTIONS
static inline int get_tiny_zone_size() {
	static int tiny_zone_size = 0;
	if (tiny_zone_size == 0) {
		tiny_zone_size = 4 * PAGE_SIZE;
	}
	return tiny_zone_size;
}

static inline int get_small_zone_size() {
	static int small_zone_size = 0;
	if (small_zone_size == 0) {
		small_zone_size = 101 * PAGE_SIZE;
	}
	return small_zone_size;
}

static inline int get_ledger_size() {
	static int ledger_size = 0;
	if (ledger_size == 0) {
		ledger_size = 1 * PAGE_SIZE;
	}
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

// DATA STRUCTURES
typedef struct AllocationMetadata {
	char		in_use;
	size_t		size;
} AllocationMetadata;

typedef struct LargeAllocationMetadata {
	size_t	size;
} LargeAllocationMetadata;

// PROTOTYPES
void	free(void *ptr);
void	*malloc(size_t size);
void	*realloc(void *ptr, size_t size);
void	show_alloc_mem();

#endif
