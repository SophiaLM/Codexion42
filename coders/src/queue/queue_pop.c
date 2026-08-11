#include "../../includes/codexion.h"
/* heap_pop: extrae la raiz del heap (elemento con prioridad minima), *
 * mueve el ultimo elemento a la raiz y lo hunde (sift-down) *
 * hasta restaurar la propiedad de heap. */
t_request	heap_pop(t_heap *heap)
{
	t_request	root;
	int			i;
	int			smallest;

	root = heap->data[0];
	heap->size--;
	heap->data[0] = heap->data[heap->size];
	i = 0;
	while (1)
	{
		smallest = smallest_child(heap, i);
		if (smallest == i)
			break ;
		heap_swap(&heap->data[i], &heap->data[smallest]);
		i = smallest;
	}
	return (root);
}
