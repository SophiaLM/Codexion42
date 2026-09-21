/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sim_monitor.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sophluna <sophluna@student.42madrid.com>   +#+  +:+        +#+       */
/*                                                +#+#+#+#+#+     +#+         */
/*   Created: 2026/09/11 12:00:00 by sophluna            #+#    #+#           */
/*   Updated: 2026/09/11 12:00:00 by sophluna           ###   ########.fr     */
/*                                                                            */
/* ************************************************************************** */

/* monitor_routine: the watchdog thread. Polls every COND_POLL_MS:
 * - if a coder exceeds time_to_burnout (+ BURNOUT_TOLERANCE_MS) since its
 *   last compile, prints "burned out" BEFORE stopping the simulation,
 *   so log_state still sees the simulation running (never hold stop_mutex
 *   while calling log_state, see codexion.h).
 * - if every coder reached its compile quota, returns and the simulation
 *   ends naturally.
 * The main thread joins it after joining the coders, so it must always
 * return and never leave the simulation running. */
#include "../../includes/codexion.h"

static int	all_compiles_done(t_sim *sim)
{
	int	i;
	int	done;

	i = -1;
	while (++i < sim->config.number_of_coders)
	{
		pthread_mutex_lock(&sim->coders[i].state_mutex);
		done = sim->coders[i].compile_count
			>= sim->config.number_of_compiles_required;
		pthread_mutex_unlock(&sim->coders[i].state_mutex);
		if (!done)
			return (0);
	}
	return (1);
}

static int	burn_first_starved(t_sim *sim)
{
	long long	now;
	t_coder		*me;
	int			i;

	now = now_ms();
	i = -1;
	while (++i < sim->config.number_of_coders)
	{
		me = &sim->coders[i];
		pthread_mutex_lock(&me->state_mutex);
		if (now - me->last_compile_start
			> sim->config.time_to_burnout + BURNOUT_TOLERANCE_MS)
		{
			pthread_mutex_unlock(&me->state_mutex);
			log_state(sim, me->id, "burned out");
			sim_stop(sim);
			return (1);
		}
		pthread_mutex_unlock(&me->state_mutex);
	}
	return (0);
}

void	*monitor_routine(void *arg)
{
	t_sim	*sim;

	sim = (t_sim *)arg;
	while (!sim_stopped(sim))
	{
		if (all_compiles_done(sim) || burn_first_starved(sim))
			break ;
		smart_sleep(COND_POLL_MS, sim);
	}
	return (NULL);
}
