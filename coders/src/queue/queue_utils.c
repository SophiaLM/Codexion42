/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   queue_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sophluna <marvin@42.fr>                    +#+  +:+        +#+       */
/*                                                +#+#+#+#+#+     +#+         */
/*   Created: 2026/08/23 19:09:12 by sophluna            #+#    #+#           */
/*   Updated: 2026/08/23 19:34:27 by sophluna           ###   ########.fr     */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/codexion.h"

int	cmp_min(t_request a, t_request b)
{
	if (a.key < b.key)
		return (-1);
	if (a.key > b.key)
		return (1);
	return (0);
}

int	smallest_child(t_heap *heap, int i)
{
	int	left;
	int	right;
	int	smallest;

	left = 2 * i + 1;
	right = 2 * i + 2;
	smallest = i;
	if (left < heap->size && heap->cmp(heap->data[left],
			heap->data[smallest]) < 0)
		smallest = left;
	if (right < heap->size && heap->cmp(heap->data[right],
			heap->data[smallest]) < 0)
		smallest = right;
	return (smallest);
}

void	heap_sift_down(t_heap *heap, int i)
{
	int	smallest;

	while (1)
	{
		smallest = smallest_child(heap, i);
		if (smallest == i)
			break ;
		heap_swap(&heap->data[i], &heap->data[smallest]);
		i = smallest;
	}
}

void	heap_swap(t_request *a, t_request *b)
{
	t_request	tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}
