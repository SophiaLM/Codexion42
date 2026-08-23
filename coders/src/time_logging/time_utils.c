/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sophluna <marvin@42.fr>                    +#+  +:+        +#+       */
/*                                                +#+#+#+#+#+     +#+         */
/*   Created: 2026/08/23 19:09:12 by sophluna            #+#    #+#           */
/*   Updated: 2026/08/23 19:34:27 by sophluna           ###   ########.fr     */
/*                                                                            */
/* ************************************************************************** */

/* now_ms:
 * gets the current system time and returns it in milliseconds.
 * USES gettimeofday(), which stores seconds and microseconds in struct timeval.
 * Converts the seconds to ms and adds the ms coming from the microseconds.
 * elapsed_ms:
 * gets the current time, computes the milliseconds
 * elapsed since the start of the simulation and returns them. */

#include "../../includes/codexion.h"

long long	now_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((long long)tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

long long	elapsed_ms(t_sim *sim)
{
	return (now_ms() - sim->start_time_ms);
}
