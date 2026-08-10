#include "../../includes/codexion.h"

void	*coder_routine(void *arg)
{
	t_coder		*me;
	t_config	*cfg;

	me = (t_coder *)arg;
	cfg = &me->sim->config;
	while (!sim_stopped(me->sim))
	{
		take_dongles_fake(me);
		set_compile_start(me);
		log_state(me->sim, me->id, "is compiling");
		smart_sleep(cfg->time_to_compile, me->sim);
		release_dongles_fake(me);
		log_state(me->sim, me->id, "is debugging");
		smart_sleep(cfg->time_to_debug, me->sim);
		log_state(me->sim, me->id, "is refactoring");
		smart_sleep(cfg->time_to_refactor, me->sim);
	}
	return (NULL);
}
