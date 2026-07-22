#include "../includes/codexion.h"

int	main(int argc, char **argv)
{
	t_config	cfg;

	if (parse_args(argc, argv, &cfg))
		ft_putendl("ok");
	return (0);
}
