/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   queue.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sophluna <sophluna@student.42madrid.com>   +#+  +:+        +#+       */
/*                                                +#+#+#+#+#+     +#+         */
/*   Created: 2026/08/23 19:09:12 by sophluna            #+#    #+#           */
/*   Updated: 2026/08/23 19:34:27 by sophluna           ###   ########.fr     */
/*                                                                            */
/* ************************************************************************** */

#ifndef QUEUE_H
# define QUEUE_H

typedef struct s_request
{
	int			coder_id;
	long long	key;
}	t_request;

typedef int	(*t_cmp)(t_request a, t_request b);

typedef struct s_heap
{
	t_request	*data;
	int			size;
	int			capacity;
	t_cmp		cmp;
}	t_heap;

void		heap_init(t_heap *heap, int capacity, t_cmp cmp);
void		heap_free(t_heap *heap);
void		heap_push(t_heap *heap, t_request req);
t_request	heap_pop(t_heap *heap);
void		heap_swap(t_request *a, t_request *b);
int			smallest_child(t_heap *heap, int i);
void		heap_sift_down(t_heap *heap, int i);
void		heap_remove(t_heap *heap, int coder_id);
int			cmp_min(t_request a, t_request b);

#endif
