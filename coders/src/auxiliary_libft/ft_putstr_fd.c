#include "../../includes/codexion.h"

void	ft_putstr_fd(const char *str, int fd)
{
	int	i;

	i = 0;
	if (str != NULL)
	{
		while (str[i])
			write(fd, &str[i++], 1);
	}
}
