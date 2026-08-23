/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sophluna <marvin@42.fr>                    +#+  +:+        +#+       */
/*                                                +#+#+#+#+#+     +#+         */
/*   Created: 2026/08/23 19:09:12 by sophluna            #+#    #+#           */
/*   Updated: 2026/08/23 19:34:27 by sophluna           ###   ########.fr     */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/codexion.h"

void	assign_dongles(t_sim *sim)
{
	int	i;
	int	n;

	n = sim->config.number_of_coders;
	i = -1;
	while (++i < n)
	{
		sim->coders[i].first = &sim->dongles[i];
		sim->coders[i].second = &sim->dongles[(i + 1) % n];
	}
}

int	my_turn(t_coder *me, t_dongle *d)
{
	t_request	head;

	if (d->holder != 0)
		return (0);
	if (now_ms() < d->available_at_ms)
		return (0);
	if (d->queue.size == 0)
		return (0);
	head = d->queue.data[0];
	return (head.coder_id == me->id);
}

long long	compute_key(t_coder *me, t_dongle *d)
{
	long long	deadline;

	if (me->sim->config.scheduler == SCHEDULER_FIFO)
		return (d->next_ticket++);
	pthread_mutex_lock(&me->state_mutex);
	deadline = me->last_compile_start + me->sim->config.time_to_burnout;
	pthread_mutex_unlock(&me->state_mutex);
	return (deadline);
}
