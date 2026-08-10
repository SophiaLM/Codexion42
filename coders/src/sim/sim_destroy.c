/* Destroy_coders: Función auxiliar (static). Destruye
 * los mutex de cada coder antes de liberar su memoria.
 * Destroy_sim: Libera todos los recursos creados por
 * init_sim() en el orden correcto y se llama al final
 * del programa, tras esperar a todos los hilos.
 */
#include "../../includes/codexion.h"

static void	destroy_coders(t_sim *sim)
{
	int	i;

	i = -1;
	while (++i < sim->config.number_of_coders)
		pthread_mutex_destroy(&sim->coders[i].state_mutex);
}

void	destroy_sim(t_sim *sim)
{
	if (sim->coders)
		destroy_coders(sim);
	pthread_mutex_destroy(&sim->stop_mutex);
	pthread_mutex_destroy(&sim->print_mutex);
	free(sim->coders);
	free(sim->dongles);
	sim->coders = NULL;
	sim->dongles = NULL;
}
