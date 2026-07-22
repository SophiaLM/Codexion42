#include "../../includes/codexion.h"

t_request	heap_pop(t_heap *heap)
{
	t_request	root;
	int			i;
	int			left;
	int			right;
	int			smallest;

	root = heap->data[0];
	heap->size--;
	heap->data[0] = heap->data[heap->size];
	i = 0;
	while (1)
	{
		left = 2 * i + 1;
		right = 2 * i + 2;
		smallest = i;
		if (left < heap->size
			&& heap->cmp(heap->data[left], heap->data[smallest]) < 0)
			smallest = left;
		if (right < heap->size
			&& heap->cmp(heap->data[right], heap->data[smallest]) < 0)
			smallest = right;
		if (smallest == i)
			break ;
		heap_swap(&heap->data[i], &heap->data[smallest]);
		i = smallest;
	}
	return (root);
}
