/* sim_stopped: lee el flag de parada de forma segura usando mutex.
 * Bloquea stop_mutex, lee el estado y lo desbloquea.
 * sim_stop: marca la simulación como finalizada de forma segura.
 * Bloquea stop_mutex, pone stop = 1 y lo desbloquea. */

#include "../../includes/codexion.h"

int	sim_stopped(t_sim *sim)
{
	int	stopped;

	pthread_mutex_lock(&sim->stop_mutex);
	stopped = sim->stop;
	pthread_mutex_unlock(&sim->stop_mutex);
	return (stopped);
}

void	sim_stop(t_sim *sim)
{
	pthread_mutex_lock(&sim->stop_mutex);
	sim->stop = 1;
	pthread_mutex_unlock(&sim->stop_mutex);
}
