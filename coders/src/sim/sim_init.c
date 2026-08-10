/* Init_coders: Función auxiliar, solo visible dentro 
 * de este archivo (static). Se encarga únicamente de 
 * rellenar el array de coders uno por uno.
 * Init_sim: La función principal de esta parte: construye 
 * TODO el mundo compartido antes de que exista el primer hilo. */
#include "../../includes/codexion.h"

static int	init_coders(t_sim *sim)
{
	int	i;

	i = -1;
	while (++i < sim->config.number_of_coders)
	{
		sim->coders[i].id = i + 1;
		sim->coders[i].sim = sim;
		sim->coders[i].last_compile_start = sim->start_time_ms;
		sim->coders[i].compile_count = 0;
		if (pthread_mutex_init(&sim->coders[i].state_mutex, NULL) != 0)
			return (0);
	}
	return (1);
}

int	init_sim(t_sim *sim, t_config *cfg)
{
	sim->config = *cfg;
	sim->coders = ft_calloc(cfg->number_of_coders, sizeof(t_coder));
	sim->dongles = ft_calloc(cfg->number_of_coders, sizeof(t_dongle));
	if (!sim->coders || !sim->dongles)
		return (0);
	if (pthread_mutex_init(&sim->stop_mutex, NULL) != 0)
		return (0);
	if (pthread_mutex_init(&sim->print_mutex, NULL) != 0)
		return (0);
	sim->start_time_ms = now_ms();
	sim->stop = 0;
	if (!init_coders(sim))
		return (0);
	return (1);
}
