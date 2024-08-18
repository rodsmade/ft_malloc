/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: roaraujo <roaraujo@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/31 17:18:48 by roaraujo          #+#    #+#             */
/*   Updated: 2024/06/03 23:17:44 by roaraujo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <string.h>
#include "ft_malloc.h"
#include "libft.h"

int	main(void)
// {
// 	void *ptr1;
// 	void *ptr2;
// 	void *ptr3;
// 	void *ptr4;
// 	void *ptr5;
// 	void *ptr6;
// 	void *ptr7;
// 	void *ptr8;
// 	void *ptr9;
// 	void *ptr10;
// 	void *ptr11;

// 	ptr1 = malloc(25 * sizeof(char));
// 	ptr2 = malloc(250 * sizeof(char));
// 	ptr3 = malloc(77 * sizeof(char));
// 	ptr4 = malloc(23 * sizeof(char));
// 	ptr5 = malloc(52 * sizeof(char));
// 	ptr6 = malloc(643 * sizeof(char));
// 	ptr7 = malloc(2500 * sizeof(char));
// 	ptr8 = malloc(4096 * sizeof(char));
// 	ptr9 = malloc(40960 * sizeof(char));
// 	ptr10 = malloc(409600 * sizeof(char));
// 	ptr11 = malloc(4096000 * sizeof(char));

// 	show_alloc_mem();

// 	free(ptr1 + 1);
// 	free(ptr2 + 1);
// 	free(ptr3 + 1);
// 	free(ptr4 + 1);
// 	free(ptr5 + 1);
// 	free(ptr6 + 1);
// 	free(ptr1);
// 	free(ptr2);
// 	free(ptr3);
// 	free(ptr4);
// 	free(ptr5);
// 	free(ptr6);
// 	free(ptr7);
// 	free(ptr8);

// 	return (0);
// }
{
    // Arrange
    void *allocs[1000];

    // Act
    for (int i = 0; i < 1000; i++) {
        ft_putnbr_fd(i, 2);
        ft_putchar_fd('\n', 2);
        allocs[i] = malloc(TINY_ZONE_THRESHOLD);
		show_alloc_mem();
    }
	return 0;
}
