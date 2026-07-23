/* sim_stopped: lee el flag de parada de forma segura usando mutex.
 * Bloquea print_mutex, lee el estado y lo desbloquea.
 * sim_stop: marca la simulación como finalizada de forma segura.
 * Bloquea print_mutex, pone stop = 1 y lo desbloquea. */

#include "codexion.h"

int	sim_stopped(t_sim *sim)
{
	int	stopped;

	pthread_mutex_lock(&sim->print_mutex);
	stopped = sim->stop;
	pthread_mutex_unlock(&sim->print_mutex);
	return (stopped);
}

void	sim_stop(t_sim *sim)
{
	pthread_mutex_lock(&sim->print_mutex);
	sim->stop = 1;
	pthread_mutex_unlock(&sim->print_mutex);
}
