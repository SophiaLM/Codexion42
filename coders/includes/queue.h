#ifndef QUEUE_H
# define QUEUE_H

typedef struct s_request
{
	int			coder_id;
	long long	key;
}	t_request;

typedef int (*t_cmp)(t_request a, t_request b);

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
t_request	heap_peek(t_heap *heap);
int			heap_size(t_heap *heap);
void		heap_swap(t_request *a, t_request *b);
int			cmp_min(t_request a, t_request b);

#endif
