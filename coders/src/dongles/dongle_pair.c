/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle_pair.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sophluna <sophluna@student.42madrid.com>   +#+  +:+        +#+       */
/*                                                +#+#+#+#+#+     +#+         */
/*   Created: 2026/09/14 10:00:00 by sophluna            #+#    #+#           */
/*   Updated: 2026/09/14 10:00:00 by sophluna           ###   ########.fr     */
/*                                                                            */
/* ************************************************************************** */

/* Atomic pair acquisition (see dongle_take.c for the single-dongle path).
 * A coder queues its request on BOTH dongles and only takes the pair when
 * it is the head of both queues at the same instant, so nobody ever holds
 * the first dongle while waiting for the second: no hold-and-wait convoy,
 * therefore no starvation (tests 08-11). acq_mutex serializes the whole
 * all-or-nothing attempt and droqu_requests every failed one. */
#include "../../includes/codexion.h"

static void	push_request(t_coder *me, t_dongle *d)
{
	t_request	req;

	req.coder_id = me->id;
	req.key = compute_key(me, d);
	heap_push(&d->queue, req);
}

static void	drop_requests(t_coder *me, t_dongle *low, t_dongle *high)
{
	heap_remove(&low->queue, me->id);
	heap_remove(&high->queue, me->id);
}

static int	claim_pair(t_coder *me, t_dongle *low, t_dongle *high)
{
	if (!my_turn(me, low) || !my_turn(me, high))
		return (0);
	heap_pop(&low->queue);
	heap_pop(&high->queue);
	low->holder = me->id;
	high->holder = me->id;
	log_state(me->sim, me->id, "has taken a dongle");
	log_state(me->sim, me->id, "has taken a dongle");
	return (1);
}

static int	try_take_pair(t_coder *me, t_dongle *low, t_dongle *high)
{
	int	taken;

	taken = 0;
	pthread_mutex_lock(&me->sim->acq_mutex);
	pthread_mutex_lock(&low->mtx);
	pthread_mutex_lock(&high->mtx);
	push_request(me, low);
	push_request(me, high);
	if (!sim_stopped(me->sim) && claim_pair(me, low, high))
		taken = 1;
	else
		drop_requests(me, low, high);
	pthread_mutex_unlock(&high->mtx);
	pthread_mutex_unlock(&low->mtx);
	pthread_mutex_unlock(&me->sim->acq_mutex);
	return (taken);
}

void	take_dongles(t_coder *me)
{
	t_dongle	*low;
	t_dongle	*high;

	low = me->first;
	high = me->second;
	if (low->id > high->id)
	{
		low = me->second;
		high = me->first;
	}
	if (low == high)
	{
		handle_only_dongle(me, low);
		return ;
	}
	while (!sim_stopped(me->sim))
	{
		if (try_take_pair(me, low, high))
			return ;
		smart_sleep(1, me->sim);
	}
}
