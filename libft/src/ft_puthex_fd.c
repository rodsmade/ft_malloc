#include "libft.h"

/**
 * @brief Writes the number 'nbr' to the file descriptor specified in 'fd'
 * in the hexadecimal representation.
 * 
 * @param s a string
 * @param fd a file descriptor in which to write the string
 */
void ft_puthex_fd(size_t nbr, int fd) {
	char *hex = "0123456789ABCDEF";
	char buffer[16];  // Buffer for hex digits
	int index = 15;

	// Handle zero as a special case
	if (nbr == 0) {
		ft_putchar_fd('0', fd);
		return;
	}

	// Fill buffer with hex digits (from right to left)
	buffer[index--] = '\0';  // Null-terminate string
	while (nbr > 0) {
		buffer[index--] = hex[nbr % 16];
		nbr /= 16;
	}

	// Print the buffer (skip leading zeros)
	ft_putstr_fd(&buffer[index + 1], fd);
}
