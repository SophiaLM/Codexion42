/* smart_sleep: espera el tiempo indicado de forma interrumpible.
 * Guarda el instante de inicio y duerme en pasos cortos (SLEEP_STEP_US),
 * comprobando en cada ciclo si la simulacion ha terminado (sim_stopped).
 * Si se detecta parada, sale inmediatamente sin esperar el tiempo restante. */

#include "codexion.h"

void	smart_sleep(long long duration_ms, t_sim *sim)
{
	long long	start;

	start = now_ms();
	while (now_ms() - start < duration_ms)
	{
		if (sim_stopped(sim))
			return ;
		usleep(SLEEP_STEP_US);
	}
}