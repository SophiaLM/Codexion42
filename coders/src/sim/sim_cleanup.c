/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sim_cleanup.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sophluna <marvin@42.fr>                    +#+  +:+        +#+       */
/*                                                +#+#+#+#+#+     +#+         */
/*   Created: 2026/08/23 19:09:12 by sophluna            #+#    #+#           */
/*   Updated: 2026/08/23 19:34:27 by sophluna           ###   ########.fr     */
/*                                                                            */
/* ************************************************************************** */

/* cleanup: main's error path. Stops the simulation,
 * joins the threads that were already created (0 to n_created)
 * and frees every resource before exiting with code != 0. */
#include "../../includes/codexion.h"

void	cleanup(t_sim *sim, int n_created)
{
	int	i;

	sim_stop(sim);
	i = -1;
	while (++i < n_created)
		pthread_join(sim->coders[i].thread, NULL);
	destroy_sim(sim);
}
