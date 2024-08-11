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
extern void *TINY__ZONE;
extern void *SMALL__ZONE;
extern void *LARGE__ZONE;
extern void *LEDGER;
extern void *LARGE_ALLOCS_LEDGER;

// MACROS
# define TINY_ZONE_THRESHOLD 128
# define SMALL_ZONE_THRESHOLD 4096
# define CUSTOM_MALLOC_UPPER_LIMIT 8589934592
# define TRUE 1
# define FALSE 0

// Define the enum
typedef enum {
	TINY,
	SMALL,
	LARGE
} e_zone;
typedef char bool;

// INLINE FUNCTIONS
static inline int get_tiny_zone_size() {
	static int tiny_zone_size = 0;
	if (tiny_zone_size == 0) {
		tiny_zone_size = 4 * getpagesize();
	}
	return tiny_zone_size;
}

static inline int get_small_zone_size() {
	static int small_zone_size = 0;
	if (small_zone_size == 0) {
		small_zone_size = 100 * getpagesize();
	}
	return small_zone_size;
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
	char	in_use;
	int		size;
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
