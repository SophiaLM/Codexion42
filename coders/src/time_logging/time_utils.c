/* now_ms: 
 * obtiene la hora actual del sistema y la devuelve en milisegundos.
 * USA gettimeofday() que almacena segundos y microsegundos en struct timeval.
 * Convierte los segundos a ms y suma los ms procedentes de los microsegundos.
 * elapsed_ms:
 * obtiene el tiempo actual, calcula los milisegundos
 * transcurridos desde el inicio de la simulación y los devuelve. */

#include "codexion.h"

long long	now_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((long long)tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

long long	elapsed_ms(t_sim *sim)
{
	return (now_ms() - sim->start_time_ms);
}
