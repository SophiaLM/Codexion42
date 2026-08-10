#include "../includes/codexion.h"

static int	run_coders(t_sim *sim, t_config *cfg)
{
	int	i;

	i = -1;
	while (++i < cfg->number_of_coders)
	{
		if (pthread_create(&sim->coders[i].thread, NULL,
				coder_routine, &sim->coders[i]) != 0)
		{
			cleanup(sim, i);
			ft_putendl_fd("Error: no se pudo crear un hilo", 2);
			return (0);
		}
	}
	i = -1;
	while (++i < cfg->number_of_coders)
		pthread_join(sim->coders[i].thread, NULL);
	return (1);
}

int	main(int argc, char **argv)
{
	t_config	cfg;
	t_sim		sim;

	if (!parse_args(argc, argv, &cfg))
		return (1);
	if (!init_sim(&sim, &cfg))
	{
		ft_putendl_fd("Error: no se pudo inicializar la simulacion", 2);
		return (1);
	}
	if (!run_coders(&sim, &cfg))
		return (1);
	destroy_sim(&sim);
	return (0);
}
