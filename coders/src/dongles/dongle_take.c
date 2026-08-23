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

/* deadline_ms: builds an absolute timespec (CLOCK_REALTIME)
 * ms in the future, for pthread_cond_timedwait. */
/* wait_for_turn: sleeps in the waiting room until it is my turn
 * or the simulation stops. timedwait is used with a short timeout (and
 * not plain cond_wait) because when the simulation shuts down nobody
 * broadcasts on these conds yet; the timeout makes the waiter re-check
 * sim_stopped periodically and exit promptly.
 * CAUTION: this loop runs with d->mtx locked; do NOT call
 * log_state or smart_sleep here. */
/* leave_or_take: if the simulation stopped, leave the queue cleanly
 * (heap_remove, because our own request may sit at any
 * position); otherwise the request is at the root (my_turn guarantees it)
 * and heap_pop is used. Logging happens AFTER unlocking d->mtx. */
/* take_dongles: acquires BOTH dongles in ascending id order
 * (resource ordering: breaks Coffman's circular wait).
 * With number_of_coders == 1, first == second: the lo != hi check
 * avoids locking the same mutex twice. */
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
	take_dongle(me, low);
	if (low != high)
		take_dongle(me, high);
}
