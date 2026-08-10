#include "../../includes/codexion.h"
/* smallest_child: devuelve el indice del hijo mas pequeno (izq, der *
 * o el propio i) segun heap->cmp, para saber hacia donde baja el nodo i. *
 * heap_pop: extrae la raiz del heap (elemento con prioridad minima), *
 * mueve el ultimo elemento a la raiz y lo hunde (sift-down) *
 * hasta restaurar la propiedad de heap. */
static int	smallest_child(t_heap *heap, int i)
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
