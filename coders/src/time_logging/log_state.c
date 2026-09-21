/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   log_state.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sophluna <sophluna@student.42madrid.com>   +#+  +:+        +#+       */
/*                                                +#+#+#+#+#+     +#+         */
/*   Created: 2026/08/23 19:09:12 by sophluna            #+#    #+#           */
/*   Updated: 2026/08/23 19:34:27 by sophluna           ###   ########.fr     */
/*                                                                            */
/* ************************************************************************** */

/* log_state: prints the current state of the simulation safely.
 * Locks print_mutex so other threads cannot interleave the output.
 * If the simulation has not ended, prints the elapsed time,
 * the coder id and the provided message. */

#include "../../includes/codexion.h"

void	log_state(t_sim *sim, int coder_id, const char *msg)
{
	pthread_mutex_lock(&sim->print_mutex);
	if (!sim_stopped(sim))
	{
		ft_putnbr(elapsed_ms(sim));
		ft_putchar(' ');
		ft_putnbr((long long)coder_id);
		ft_putchar(' ');
		ft_putendl(msg);
	}
	pthread_mutex_unlock(&sim->print_mutex);
}
