#include "../../includes/codexion.h"

int	ft_atoi(const char *str, int *out)
{
	long long	res;

	if (!str || !*str)
		return (-1);
	while (*str == 32 || (*str >= 9 && *str <= 13))
		str++;
	if (*str == '-')
		return (-1);
	if (*str == '+')
		str++;
	if (*str < '0' || *str > '9')
		return (-1);
	res = 0;
	while (*str >= '0' && *str <= '9')
	{
		res = res * 10 + (*str - '0');
		if (res > INT_MAX)
			return (-1);
		str++;
	}
	if (*str != '\0')
		return (-1);
	*out = (int)res;
	return (0);
}
