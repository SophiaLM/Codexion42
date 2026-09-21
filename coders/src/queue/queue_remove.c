/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   queue_remove.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sophluna <sophluna@student.42madrid.com>   +#+  +:+        +#+       */
/*                                                +#+#+#+#+#+     +#+         */
/*   Created: 2026/08/23 19:09:12 by sophluna            #+#    #+#           */
/*   Updated: 2026/08/23 19:34:27 by sophluna           ###   ########.fr     */
/*                                                                            */
/* ************************************************************************** */

/* heap_remove: removes from the heap the first request whose coder_id
 * matches. Used when the simulation stops and a coder must
 * leave the waiting queue without having received the dongle (its
 * request may be at any position, not only at the root). */
#include "../../includes/codexion.h"

static void	sift_up(t_heap *heap, int i)
{
	int	parent;

	while (i > 0)
	{
		parent = (i - 1) / 2;
		if (heap->cmp(heap->data[i], heap->data[parent]) >= 0)
			break ;
		heap_swap(&heap->data[i], &heap->data[parent]);
		i = parent;
	}
}

void	heap_remove(t_heap *heap, int coder_id)
{
	int	i;

	i = 0;
	while (i < heap->size && heap->data[i].coder_id != coder_id)
		i++;
	if (i == heap->size)
		return ;
	heap->size--;
	heap->data[i] = heap->data[heap->size];
	if (i > 0 && heap->cmp(heap->data[i], heap->data[(i - 1) / 2]) < 0)
		sift_up(heap, i);
	else
		heap_sift_down(heap, i);
}
