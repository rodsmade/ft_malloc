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

int	main(void)
{
	printf("Hello, dynamic library!\n");
	// function3();
	void *ptr1;
	void *ptr2;
	void *ptr3;
	void *ptr4;
	void *ptr5;
	void *ptr6;
	void *ptr7;

	ptr1 = malloc(25 * sizeof(char));
	ptr2 = malloc(250 * sizeof(char));
	ptr3 = malloc(77 * sizeof(char));
	ptr4 = malloc(23 * sizeof(char));
	ptr5 = malloc(52 * sizeof(char));
	ptr6 = malloc(643 * sizeof(char));
	ptr7 = malloc(2500 * sizeof(char));
	
	ptr1 = strcpy(ptr1, "Hello world from malloc!");
	((char *)ptr1)[24] = '\0';

	printf("msg: >>%s<<\n", (char *) ptr1);

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

	return (0);
}
