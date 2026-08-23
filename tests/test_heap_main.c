#include "../coders/includes/codexion.h"
#include <stdio.h>

/* cmp_min y heap_sift_down viven en src/queue/queue_utils.c. *
 * NOTA: heap_pop nunca debe llamarse sobre un heap vacio (lo garantiza *
 * la logica del proyecto); por eso no hay test de pop en vacio. */

static int	test_remove_from_middle(void)
{
	t_heap	h;

	heap_init(&h, 10, cmp_min);
	heap_push(&h, (t_request){1, 50});
	heap_push(&h, (t_request){2, 10});
	heap_push(&h, (t_request){3, 30});
	heap_push(&h, (t_request){4, 20});
	heap_remove(&h, 3);
	if (heap_size(&h) != 3)
		return (printf("FAIL: size tras remove = %d (esperado 3)\n",
				heap_size(&h)), 1);
	if (heap_pop(&h).coder_id != 2 || heap_pop(&h).coder_id != 4
		|| heap_pop(&h).coder_id != 1)
		return (printf("FAIL: orden tras remove incorrecto\n"), 1);
	printf("OK: remove del medio conserva el orden (10,20,50)\n");
	heap_free(&h);
	return (0);
}

static int	test_remove_head(void)
{
	t_heap	h;

	heap_init(&h, 10, cmp_min);
	heap_push(&h, (t_request){1, 50});
	heap_push(&h, (t_request){2, 10});
	heap_push(&h, (t_request){3, 30});
	heap_remove(&h, 2);
	if (heap_pop(&h).coder_id != 3 || heap_pop(&h).coder_id != 1)
		return (printf("FAIL: remove de la raiz incorrecto\n"), 1);
	printf("OK: remove de la raiz (10) deja (30,50)\n");
	heap_free(&h);
	return (0);
}

static int	test_remove_only_element(void)
{
	t_heap	h;

	heap_init(&h, 1, cmp_min);
	heap_push(&h, (t_request){7, 7});
	heap_remove(&h, 7);
	if (heap_size(&h) != 0)
		return (printf("FAIL: remove del unico elemento dejo size %d\n",
				heap_size(&h)), 1);
	printf("OK: remove del unico elemento deja el heap vacio\n");
	heap_free(&h);
	return (0);
}

static int	test_remove_absent(void)
{
	t_heap	h;

	heap_init(&h, 10, cmp_min);
	heap_push(&h, (t_request){1, 5});
	heap_push(&h, (t_request){2, 8});
	heap_remove(&h, 99);
	if (heap_size(&h) != 2)
		return (printf("FAIL: remove de ausente cambio el heap\n"), 1);
	if (heap_pop(&h).coder_id != 1)
		return (printf("FAIL: heap corrupto tras remove de ausente\n"), 1);
	printf("OK: remove de coder inexistente no altera el heap\n");
	heap_free(&h);
	return (0);
}

static int	test_size_one(void)
{
	t_heap	h;

	heap_init(&h, 1, cmp_min);
	heap_push(&h, (t_request){1, 42});
	printf("peek: %lld (esperado 42)\n", heap_peek(&h).key);
	printf("pop: %lld (esperado 42)\n", heap_pop(&h).key);
	heap_free(&h);
	return (0);
}

static int	test_all_equal(void)
{
	t_heap	h;
	int		i;

	heap_init(&h, 5, cmp_min);
	i = -1;
	while (++i < 5)
		heap_push(&h, (t_request){i, 7});
	i = -1;
	while (++i < 5)
		printf("pop igual #%d: %lld (esperado 7)\n", i, heap_pop(&h).key);
	heap_free(&h);
	return (0);
}

static int	test_interleaved(void)
{
	t_heap	h;

	heap_init(&h, 10, cmp_min);
	heap_push(&h, (t_request){1, 50});
	heap_push(&h, (t_request){2, 10});
	printf("pop: %lld (esperado 10)\n", heap_pop(&h).key);
	heap_push(&h, (t_request){3, 5});
	printf("pop: %lld (esperado 5)\n", heap_pop(&h).key);
	printf("pop: %lld (esperado 50)\n", heap_pop(&h).key);
	heap_free(&h);
	return (0);
}

static int	test_1000(void)
{
	t_heap		h;
	long long	prev;
	long long	cur;
	int			i;

	heap_init(&h, 1000, cmp_min);
	srand(42);
	i = -1;
	while (++i < 1000)
		heap_push(&h, (t_request){i, rand() % 10000});
	prev = -1;
	while (heap_size(&h) > 0)
	{
		cur = heap_pop(&h).key;
		if (cur < prev)
			return (printf("ORDER BROKEN\n"), 1);
		prev = cur;
	}
	printf("OK: 1000 elements popped in order\n");
	heap_free(&h);
	return (0);
}

int	main(void)
{
	int	result;

	result = 0;
	result |= test_1000();
	result |= test_size_one();
	result |= test_all_equal();
	result |= test_interleaved();
	result |= test_remove_from_middle();
	result |= test_remove_head();
	result |= test_remove_only_element();
	result |= test_remove_absent();
	if (result)
		printf("=== ALGUN TEST FALLO ===\n");
	else
		printf("=== TODOS LOS TESTS PASARON ===\n");
	return (result);
}
