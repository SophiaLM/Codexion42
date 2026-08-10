#include "../../includes/codexion.h"

void	set_compile_start(t_coder *me)
{
	pthread_mutex_lock(&me->state_mutex);
	me->last_compile_start = now_ms();
	me->compile_count++;
	pthread_mutex_unlock(&me->state_mutex);
}
