/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: roaraujo <roaraujo@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/31 17:18:48 by roaraujo          #+#    #+#             */
/*   Updated: 2024/04/03 14:52:22 by roaraujo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <string.h>
#include "ft_malloc.h"

int	main(void)
{
	printf("Hello, dynamic library!\n");
	// function3();
	void *ptr;

	ptr = malloc(25 * sizeof(char));
	
	ptr = strcpy(ptr, "Hello world from malloc!");
	((char *)ptr)[24] = '\0';

	printf("msg: >>%s<<\n", (char *) ptr);

	free(ptr);

	return (0);
}
