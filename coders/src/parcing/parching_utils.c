#include "../../includes/codexion.h"

t_error	ft_check_overflow(const char *str)
{
	size_t	len;

	len = ft_strlen(str);
	if (len > 10)
		return (ERR_OVERFLOW);
	if (len < 10)
		return (ERR_OK);
	if (ft_strncmp(str, "2147483647", 10) > 0)
		return (ERR_OVERFLOW);
	return (ERR_OK);
}

t_error	ft_is_valid_positive(const char *str)
{
	t_error	err;

	if (!str || !str[0])
		return (ERR_NOT_NUMERIC);
	if (str[0] == '-')
		return (ERR_NEGATIVE);
	if (!ft_validnumber(str))
		return (ERR_NOT_NUMERIC);
	err = ft_check_overflow(str);
	if (err != ERR_OK)
		return (err);
	if (str[0] == '0')
		return (ERR_ZERO);
	return (ERR_OK);
}

t_error	ft_is_valid_non_negative(const char *str)
{
	if (!str || !str[0])
		return (ERR_NOT_NUMERIC);
	if (str[0] == '-')
		return (ERR_NEGATIVE);
	if (!ft_validnumber(str))
		return (ERR_NOT_NUMERIC);
	return (ft_check_overflow(str));
}

t_error	ft_is_valid_scheduler(const char *str)
{
	if (ft_strlen(str) == 4 && ft_strncmp(str, "fifo", 4) == 0)
		return (ERR_OK);
	if (ft_strlen(str) == 3 && ft_strncmp(str, "edf", 3) == 0)
		return (ERR_OK);
	return (ERR_SCHEDULER);
}
