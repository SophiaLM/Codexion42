#include "../../includes/codexion.h"

void	heap_push(t_heap *heap, t_request req)
{
	int	i;
	int	parent;

	heap->data[heap->size] = req;
	i = heap->size;
	heap->size++;
	while (i > 0)
	{
		parent = (i - 1) / 2;
		if (heap->cmp(heap->data[i], heap->data[parent]) >= 0)
			break ;
		heap_swap(&heap->data[i], &heap->data[parent]);
		i = parent;
	}
}
