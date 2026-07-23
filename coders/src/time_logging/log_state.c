/* log_state: imprime el estado actual de la simulación de forma segura.
 * Bloquea print_mutex para evitar que otros hilos interrumpan la salida.
 * Si la simulación no ha terminado, imprime el tiempo transcurrido,
 * el id del coder y el mensaje proporcionado. */

#include "codexion.h"

void	log_state(t_sim *sim, int coder_id, const char *msg)
{
	pthread_mutex_lock(&sim->print_mutex);
	if (!sim->stop)
	{
		ft_putnbr(elapsed_ms(sim));
		ft_putchar(' ');
		ft_putnbr((long long)coder_id);
		ft_putchar(' ');
		ft_putendl(msg);
	}
	pthread_mutex_unlock(&sim->print_mutex);
}
