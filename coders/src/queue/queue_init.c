/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   queue_init.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sophluna <marvin@42.fr>                    +#+  +:+        +#+       */
/*                                                +#+#+#+#+#+     +#+         */
/*   Created: 2026/08/23 19:09:12 by sophluna            #+#    #+#           */
/*   Updated: 2026/08/23 19:34:27 by sophluna           ###   ########.fr     */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/codexion.h"

void	heap_init(t_heap *heap, int capacity, t_cmp cmp)
{
	heap->data = ft_calloc(capacity, sizeof(t_request));
	heap->size = 0;
	heap->capacity = capacity;
	heap->cmp = cmp;
}

void	heap_free(t_heap *heap)
{
	free(heap->data);
	heap->data = NULL;
	heap->size = 0;
	heap->capacity = 0;
}
