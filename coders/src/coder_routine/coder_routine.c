/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_routine.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sophluna <marvin@42.fr>                    +#+  +:+        +#+       */
/*                                                +#+#+#+#+#+     +#+         */
/*   Created: 2026/08/23 19:09:12 by sophluna            #+#    #+#           */
/*   Updated: 2026/08/23 19:34:27 by sophluna           ###   ########.fr     */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/codexion.h"

static int	compiles_done(t_coder *me)
{
	int	done;

	pthread_mutex_lock(&me->state_mutex);
	done = me->compile_count >= me->sim->config.number_of_compiles_required;
	pthread_mutex_unlock(&me->state_mutex);
	return (done);
}

void	*coder_routine(void *arg)
{
	t_coder		*me;
	t_config	*cfg;

	me = (t_coder *)arg;
	cfg = &me->sim->config;
	while (!sim_stopped(me->sim) && !compiles_done(me))
	{
		take_dongles(me);
		if (sim_stopped(me->sim))
			break ;
		set_compile_start(me);
		log_state(me->sim, me->id, "is compiling");
		smart_sleep(cfg->time_to_compile, me->sim);
		release_dongles(me);
		log_state(me->sim, me->id, "is debugging");
		smart_sleep(cfg->time_to_debug, me->sim);
		log_state(me->sim, me->id, "is refactoring");
		smart_sleep(cfg->time_to_refactor, me->sim);
	}
	return (NULL);
}
