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

AllocationEntry first_entry;

void	*malloc(size_t size)
{
	void						*ptr;
	AllocationEntry	*entry;

	ptr = mmap(NULL, sizeof(AllocationEntry) + size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
	if (ptr == MAP_FAILED)
		return (NULL);
	
	entry = (AllocationEntry *) ptr;
	entry->address = ptr + sizeof(AllocationEntry);
	entry->size = size;
	return (entry->address);
}

void	free(void *ptr)
{
	AllocationEntry *entry;
	
	entry = (AllocationEntry *) (ptr - sizeof(AllocationEntry));
	if (ptr && ptr == entry->address)
	{
		munmap(entry, entry->size);
	}
	return ;
}

void	*realloc(void *ptr, size_t size)
{
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
