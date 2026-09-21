/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sim_init.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sophluna <sophluna@student.42madrid.com>   +#+  +:+        +#+       */
/*                                                +#+#+#+#+#+     +#+         */
/*   Created: 2026/08/23 19:09:12 by sophluna            #+#    #+#           */
/*   Updated: 2026/08/23 19:34:27 by sophluna           ###   ########.fr     */
/*                                                                            */
/* ************************************************************************** */

/* init_coders: helper function, visible only inside
 * this file (static). It is solely in charge of
 * filling the coders array one by one.
 * init_sim: the main function of this part: it builds
 * the ENTIRE shared world before the first thread exists. */
/* rollback_coders: frees whatever init_sim created before
 * failing. The array mutexes are zeroed by ft_calloc, and destroying
 * a mutex still in PTHREAD_MUTEX_INITIALIZER state is valid in POSIX. */
#include "../../includes/codexion.h"

static int	init_coders(t_sim *sim)
{
	int	i;

	i = -1;
	while (++i < sim->config.number_of_coders)
	{
		sim->coders[i].id = i + 1;
		sim->coders[i].sim = sim;
		sim->coders[i].last_compile_start = sim->start_time_ms;
		sim->coders[i].compile_count = 0;
		if (pthread_mutex_init(&sim->coders[i].state_mutex, NULL) != 0)
			return (0);
	}
	return (1);
}

static void	rollback_coders(t_sim *sim)
{
	int	i;

	i = -1;
	while (++i < sim->config.number_of_coders)
		pthread_mutex_destroy(&sim->coders[i].state_mutex);
}

static int	init_sim_mutexes(t_sim *sim)
{
	if (pthread_mutex_init(&sim->stop_mutex, NULL) != 0)
		return (0);
	if (pthread_mutex_init(&sim->print_mutex, NULL) != 0)
	{
		pthread_mutex_destroy(&sim->stop_mutex);
		return (0);
	}
	if (pthread_mutex_init(&sim->acq_mutex, NULL) != 0)
	{
		pthread_mutex_destroy(&sim->print_mutex);
		pthread_mutex_destroy(&sim->stop_mutex);
		return (0);
	}
	return (1);
}

static void	cleanup_sim_fail(t_sim *sim, int rollback)
{
	if (rollback)
		rollback_coders(sim);
	pthread_mutex_destroy(&sim->print_mutex);
	pthread_mutex_destroy(&sim->stop_mutex);
	pthread_mutex_destroy(&sim->acq_mutex);
	free(sim->coders);
}

int	init_sim(t_sim *sim, t_config *cfg)
{
	sim->config = *cfg;
	sim->coders = ft_calloc(cfg->number_of_coders, sizeof(t_coder));
	sim->dongles = NULL;
	if (!sim->coders)
		return (0);
	if (!init_sim_mutexes(sim))
	{
		free(sim->coders);
		return (0);
	}
	sim->start_time_ms = now_ms();
	sim->stop = 0;
	if (!init_coders(sim))
		return (cleanup_sim_fail(sim, 0), 0);
	if (!init_dongles(sim, cfg))
		return (cleanup_sim_fail(sim, 1), 0);
	assign_dongles(sim);
	return (1);
}
