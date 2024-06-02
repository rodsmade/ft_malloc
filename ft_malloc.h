/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_malloc.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: roaraujo <roaraujo@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/31 17:18:54 by roaraujo          #+#    #+#             */
/*   Updated: 2024/04/03 15:01:12 by roaraujo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_MALLOC_H
# define FT_MALLOC_H

# include <sys/mman.h> // mmap(), munmap()
# include <unistd.h> // getpagesize()
# include <sys/time.h>
# include <sys/resource.h> // getrlimit()

# include "libft.h"

// DEBUGGING
# include <stdio.h>


// MACROS
# define TINY_ZONE_THRESHOLD 128
# define SMALL_ZONE_THRESHOLD 4096
# define TRUE 1
# define FALSE 0

// Data Structures
typedef struct AllocationEntry {
	char	in_use;
	int		size;
} AllocationEntry;

// Function prototypes
void	free(void *ptr);
void	*malloc(size_t size);
void	*realloc(void *ptr, size_t size);
void	show_alloc_mem();

#endif
