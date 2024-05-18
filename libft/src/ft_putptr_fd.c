#include "libft.h"

/**
 * @brief Writes the pointer 'ptr' as a string (hexadecimal address) to the
 * file descriptor specified in 'fd'.
 * 
 * @param ptr a pointer
 * @param fd a file descriptor in which to write the string
 */
void	ft_putptr_fd(void *ptr, int fd)
{
	char		buffer[20];
	int			i;
	uintptr_t	address;

	address = (uintptr_t) ptr;
	buffer[0] = '0';
	buffer[1] = 'x';
	i = 2;
	if (address == 0)
	{
		buffer[i++] = '0';
	}
	else
	{
		for (int shift = (sizeof(address) * 8) - 4; shift >= 0; shift -= 4)
		{
			int digit = (address >> shift) & 0xf;
			if (i > 2 || digit > 0)
				buffer[i++] = HEX_DIGITS[digit];
		}
	}
	buffer[i] = '\0';
	write(fd, buffer, i + 1);
}
