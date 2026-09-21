/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   set_compile_start.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sophluna <sophluna@student.42madrid.com>   +#+  +:+        +#+       */
/*                                                +#+#+#+#+#+     +#+         */
/*   Created: 2026/08/23 19:09:12 by sophluna            #+#    #+#           */
/*   Updated: 2026/08/23 19:34:27 by sophluna           ###   ########.fr     */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/codexion.h"

void	set_compile_start(t_coder *me)
{
	pthread_mutex_lock(&me->state_mutex);
	me->last_compile_start = now_ms();
	me->compile_count++;
	pthread_mutex_unlock(&me->state_mutex);
}
