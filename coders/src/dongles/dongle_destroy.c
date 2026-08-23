/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle_destroy.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sophluna <marvin@42.fr>                    +#+  +:+        +#+       */
/*                                                +#+#+#+#+#+     +#+         */
/*   Created: 2026/08/23 19:09:12 by sophluna            #+#    #+#           */
/*   Updated: 2026/08/23 19:34:27 by sophluna           ###   ########.fr     */
/*                                                                            */
/* ************************************************************************** */

/* destroy_dongles: frees everything created by init_dongles. Symmetric
 * to init: destroys each dongle's mutex, cond and heap, and frees the
 * array. Leaves sim->dongles set to NULL so destroy_sim does not
 * double free. */
#include "../../includes/codexion.h"

void	destroy_dongles(t_sim *sim)
{
	int	i;

	if (!sim->dongles)
		return ;
	i = -1;
	while (++i < sim->config.number_of_coders)
	{
		pthread_mutex_destroy(&sim->dongles[i].mtx);
		pthread_cond_destroy(&sim->dongles[i].cond);
		heap_free(&sim->dongles[i].queue);
	}
	free(sim->dongles);
	sim->dongles = NULL;
}
