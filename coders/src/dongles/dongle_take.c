/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle_take.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sophluna <marvin@42.fr>                    +#+  +:+        +#+       */
/*                                                +#+#+#+#+#+     +#+         */
/*   Created: 2026/08/23 19:09:12 by sophluna            #+#    #+#           */
/*   Updated: 2026/08/23 19:34:27 by sophluna           ###   ########.fr     */
/*                                                                            */
/* ************************************************************************** */

#include <time.h>
#include "../../includes/codexion.h"

static struct timespec	deadline_ms(long long ms)
{
	struct timespec	ts;

	clock_gettime(CLOCK_REALTIME, &ts);
	ts.tv_nsec += ms * 1000000;
	ts.tv_sec += ts.tv_nsec / 1000000000;
	ts.tv_nsec %= 1000000000;
	return (ts);
}

static void	wait_for_turn(t_coder *me, t_dongle *d)
{
	struct timespec	ts;

	while (!my_turn(me, d) && !sim_stopped(me->sim))
	{
		ts = deadline_ms(COND_POLL_MS);
		pthread_cond_timedwait(&d->cond, &d->mtx, &ts);
	}
}

static void	leave_or_take(t_coder *me, t_dongle *d)
{
	if (sim_stopped(me->sim))
	{
		heap_remove(&d->queue, me->id);
		pthread_mutex_unlock(&d->mtx);
	}
	else
	{
		heap_pop(&d->queue);
		d->holder = me->id;
		pthread_mutex_unlock(&d->mtx);
		log_state(me->sim, me->id, "has taken a dongle");
	}
}

void	take_dongle(t_coder *me, t_dongle *d)
{
	t_request	req;

	pthread_mutex_lock(&d->mtx);
	req.coder_id = me->id;
	req.key = compute_key(me, d);
	heap_push(&d->queue, req);
	wait_for_turn(me, d);
	leave_or_take(me, d);
}

void	take_dongles(t_coder *me)
{
	t_dongle	*low;
	t_dongle	*high;

	if (me->first->id < me->second->id)
	{
		low = me->first;
		high = me->second;
	}
	else
	{
		low = me->second;
		high = me->first;
	}
	if (low == high)
	{
		handle_only_dongle(me, low);
		return ;
	}
	take_dongle(me, low);
	take_dongle(me, high);
}
