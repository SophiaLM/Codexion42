/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle_init.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sophluna <marvin@42.fr>                    +#+  +:+        +#+       */
/*                                                +#+#+#+#+#+     +#+         */
/*   Created: 2026/08/23 19:09:12 by sophluna            #+#    #+#           */
/*   Updated: 2026/08/23 19:34:27 by sophluna           ###   ########.fr     */
/*                                                                            */
/* ************************************************************************** */

/* destroy_partial_dongles: rollback of init_dongles. Destroys the
 * first n already-initialized dongles, frees the array and sets it to NULL. */
/* init_dongles: creates the dongles array and its mutexes/conds/heaps.
 * The heap capacity is number_of_coders: a coder can only wait
 * in ONE dongle's queue at a time (take_dongle is blocking
 * and take_dongles takes the dongles sequentially), so the worst case
 * is that all coders wait for the same dongle. heap_push does NOT check
 * size == capacity, so this bound must be exact. */
#include "../../includes/codexion.h"

static int	init_one_dongle(t_dongle *d, int id, int capacity)
{
	d->id = id;
	d->holder = 0;
	d->available_at_ms = 0;
	d->next_ticket = 0;
	if (pthread_mutex_init(&d->mtx, NULL) != 0)
		return (0);
	if (pthread_cond_init(&d->cond, NULL) != 0)
	{
		pthread_mutex_destroy(&d->mtx);
		return (0);
	}
	heap_init(&d->queue, capacity, cmp_min);
	return (1);
}

static void	destroy_partial_dongles(t_sim *sim, int n)
{
	int	i;

	i = -1;
	while (++i < n)
	{
		pthread_mutex_destroy(&sim->dongles[i].mtx);
		pthread_cond_destroy(&sim->dongles[i].cond);
		heap_free(&sim->dongles[i].queue);
	}
	free(sim->dongles);
	sim->dongles = NULL;
}

int	init_dongles(t_sim *sim, t_config *cfg)
{
	int	i;

	sim->dongles = ft_calloc(cfg->number_of_coders, sizeof(t_dongle));
	if (!sim->dongles)
		return (0);
	i = -1;
	while (++i < cfg->number_of_coders)
	{
		if (!init_one_dongle(&sim->dongles[i], i, cfg->number_of_coders))
		{
			destroy_partial_dongles(sim, i);
			return (0);
		}
	}
	return (1);
}
