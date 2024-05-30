/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: roaraujo <roaraujo@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/31 17:18:48 by roaraujo          #+#    #+#             */
/*   Updated: 2024/04/04 17:15:50 by roaraujo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <string.h>
#include "ft_malloc.h"
#include "libft.h"

int	main(void)
{
	write(1, "Hello, dynamic library!\n", 24);

	// function3();
	void *ptr1;
	void *ptr2;
	void *ptr3;
	void *ptr4;
	void *ptr5;
	void *ptr6;
	void *ptr7;
	void *ptr8;

	ptr1 = strcpy(ptr1, "Hello world from malloc!");
	((char *)ptr1)[24] = '\0';

	ptr1 = malloc(25 * sizeof(char));
	ptr2 = malloc(250 * sizeof(char));
	ptr3 = malloc(77 * sizeof(char));
	ptr4 = malloc(23 * sizeof(char));
	ptr5 = malloc(52 * sizeof(char));
	ptr6 = malloc(643 * sizeof(char));
	ptr7 = malloc(2500 * sizeof(char));
	ptr8 = malloc(2500 * sizeof(char));

	ft_putstr_fd("total_allocd_bytes: ", 1);
	ft_putstr_fd("msg: >>", 1);
	ft_putstr_fd((char *) ptr1, 1);
	ft_putstr_fd("<<\n", 1);

	free(ptr1 + 1);
	free(ptr2 + 1);
	free(ptr3 + 1);
	free(ptr4 + 1);
	free(ptr5 + 1);
	free(ptr6 + 1);
	free(ptr1);
	free(ptr2);
	free(ptr3);
	free(ptr4);
	free(ptr5);
	free(ptr6);
	free(ptr7);
	free(ptr8);

	return (0);
}
