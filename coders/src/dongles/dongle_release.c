/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle_release.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sophluna <sophluna@student.42madrid.com>   +#+  +:+        +#+       */
/*                                                +#+#+#+#+#+     +#+         */
/*   Created: 2026/08/23 19:09:12 by sophluna            #+#    #+#           */
/*   Updated: 2026/08/23 19:34:27 by sophluna           ###   ########.fr     */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/codexion.h"

void	release_dongle(t_dongle *d, long long cooldown)
{
	pthread_mutex_lock(&d->mtx);
	d->holder = 0;
	d->available_at_ms = now_ms() + cooldown;
	pthread_cond_broadcast(&d->cond);
	pthread_mutex_unlock(&d->mtx);
}

void	release_dongles(t_coder *me)
{
	long long	cooldown;

	cooldown = me->sim->config.dongle_cooldown;
	release_dongle(me->first, cooldown);
	if (me->second != me->first)
		release_dongle(me->second, cooldown);
}
