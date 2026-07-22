#include "../../includes/codexion.h"

int	ft_atoll(const char *str, long long *out)
{
	int	tmp;

	if (ft_atoi(str, &tmp) == -1)
		return (-1);
	*out = (long long)tmp;
	return (0);
}
