#include "../../includes/codexion.h"

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
