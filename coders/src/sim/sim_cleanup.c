/* cleanup: camino de error de main. Detiene la simulacion,
 * hace join de los hilos que ya se crearon (de 0 a n_created)
 * y libera todos los recursos antes de salir con codigo != 0. */
#include "../../includes/codexion.h"

void	cleanup(t_sim *sim, int n_created)
{
	int	i;

	sim_stop(sim);
	i = -1;
	while (++i < n_created)
		pthread_join(sim->coders[i].thread, NULL);
	destroy_sim(sim);
}
