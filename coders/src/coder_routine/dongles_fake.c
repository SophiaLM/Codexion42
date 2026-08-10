#include "../../includes/codexion.h"

void	take_dongles_fake(t_coder *me)
{
	log_state(me->sim, me->id, "has taken a dongle");
	log_state(me->sim, me->id, "has taken a dongle");
}

void	release_dongles_fake(t_coder *me)
{
	log_state(me->sim, me->id, "has released the dongles");
}
