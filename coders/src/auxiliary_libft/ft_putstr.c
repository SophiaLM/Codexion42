#include "../../includes/codexion.h"

void	ft_putstr(const char *str)
{
	int	i;

	i = 0;
	if (str != NULL)
	{
		while (str[i])
			write(1, &str[i++], 1);
	}
}
