/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   queue_pop.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sophluna <sophluna@student.42madrid.com>   +#+  +:+        +#+       */
/*                                                +#+#+#+#+#+     +#+         */
/*   Created: 2026/08/23 19:09:12 by sophluna            #+#    #+#           */
/*   Updated: 2026/08/23 19:34:27 by sophluna           ###   ########.fr     */
/*                                                                            */
/* ************************************************************************** */

/* heap_pop: extracts the heap root (element with minimum priority),
 * moves the last element to the root and sifts it down (sift-down)
 * until the heap property is restored.
 * IMPORTANT: it must never be called with size == 0. In this project
 * that is guaranteed by construction (heap_push of the requester's
 * own request is always done before waiting). */
#include "../../includes/codexion.h"

t_request	heap_pop(t_heap *heap)
{
	t_request	root;

	root = heap->data[0];
	heap->size--;
	heap->data[0] = heap->data[heap->size];
	heap_sift_down(heap, 0);
	return (root);
}
