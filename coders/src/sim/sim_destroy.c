/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sim_destroy.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sophluna <marvin@42.fr>                    +#+  +:+        +#+       */
/*                                                +#+#+#+#+#+     +#+         */
/*   Created: 2026/08/23 19:09:12 by sophluna            #+#    #+#           */
/*   Updated: 2026/08/23 19:34:27 by sophluna           ###   ########.fr     */
/*                                                                            */
/* ************************************************************************** */

/* destroy_coders: static helper function. Destroys
 * each coder's mutex before freeing its memory.
 * destroy_sim: frees every resource created by
 * init_sim() in the correct order; it is called at the end
 * of the program, after waiting for all the threads.
 */
#include "../../includes/codexion.h"

static void	destroy_coders(t_sim *sim)
{
	int	i;

	i = -1;
	while (++i < sim->config.number_of_coders)
		pthread_mutex_destroy(&sim->coders[i].state_mutex);
}

void	destroy_sim(t_sim *sim)
{
	if (sim->coders)
		destroy_coders(sim);
	pthread_mutex_destroy(&sim->stop_mutex);
	pthread_mutex_destroy(&sim->print_mutex);
	destroy_dongles(sim);
	free(sim->coders);
	sim->coders = NULL;
}
