#include "../../includes/codexion.h"

void	print_error(t_error code)
{
	const char	*msgs[7];

	msgs[ERR_ARGC] = "Error: numero de argumentos incorrecto";
	msgs[ERR_NOT_NUMERIC] = "Error: argumento no numerico";
	msgs[ERR_OVERFLOW] = "Error: numero demasiado grande";
	msgs[ERR_ZERO] = "Error: el valor no puede ser cero";
	msgs[ERR_NEGATIVE] = "Error: el valor no puede ser negativo";
	msgs[ERR_SCHEDULER] = "Error: scheduler debe ser 'fifo' o 'edf'";
	ft_putendl(msgs[code]);
}

t_error	check_numeric_args(char **argv)
{
	t_error	err;
	int		i;

	i = 1;
	while (i <= 6)
	{
		err = ft_is_valid_positive(argv[i]);
		if (err != ERR_OK)
			return (err);
		i++;
	}
	err = ft_is_valid_non_negative(argv[7]);
	if (err != ERR_OK)
		return (err);
	return (ERR_OK);
}

int	parse_args(int argc, char **argv)
{
	t_error	err;

	if (argc != 9)
	{
		print_error(ERR_ARGC);
		return (0);
	}
	err = check_numeric_args(argv);
	if (err != ERR_OK)
	{
		print_error(err);
		return (0);
	}
	err = ft_is_valid_scheduler(argv[8]);
	if (err != ERR_OK)
	{
		print_error(err);
		return (0);
	}
	return (1);
}
