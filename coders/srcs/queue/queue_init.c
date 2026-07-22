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
