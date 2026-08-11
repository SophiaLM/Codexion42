#include "../../includes/codexion.h"

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

void	heap_swap(t_request *a, t_request *b)
{
	t_request	tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}

t_request	heap_peek(t_heap *heap)
{
	return (heap->data[0]);
}

int	heap_size(t_heap *heap)
{
	return (heap->size);
}
