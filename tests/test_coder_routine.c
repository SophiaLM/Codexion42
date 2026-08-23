#include "../coders/includes/codexion.h"
#include <stdio.h>
#include <string.h>

static int	g_fail;

static void	init_sim_minimal(t_sim *sim, t_config *cfg)
{
	cfg->number_of_coders = 1;
	cfg->time_to_burnout = 800;
	cfg->time_to_compile = 10;
	cfg->time_to_debug = 10;
	cfg->time_to_refactor = 10;
	cfg->number_of_compiles_required = 999999;
	cfg->dongle_cooldown = 100;
	cfg->scheduler = SCHEDULER_FIFO;
	if (!init_sim(sim, cfg))
	{
		printf("  FAIL: init_sim fallo\n");
		exit(1);
	}
}

static void	cleanup_sim(t_sim *sim)
{
	destroy_sim(sim);
}

static void	test_set_compile_start_count(void)
{
	t_sim		sim;
	t_config	cfg;
	t_coder		coder;

	printf("--- test_set_compile_start_count: compile_count debe incrementarse ---\n");
	init_sim_minimal(&sim, &cfg);
	coder = sim.coders[0];
	printf("  compile_count antes: %d\n", coder.compile_count);
	set_compile_start(&coder);
	printf("  compile_count despues 1: %d (esperado 1)\n", coder.compile_count);
	set_compile_start(&coder);
	printf("  compile_count despues 2: %d (esperado 2)\n", coder.compile_count);
	if (coder.compile_count == 2)
		printf("  OK: compile_count incrementa correctamente\n\n");
	else
	{
		printf("  FAIL: esperaba 2, obtuve %d\n\n", coder.compile_count);
		g_fail = 1;
	}
	cleanup_sim(&sim);
}

static void	test_set_compile_start_timestamp(void)
{
	t_sim		sim;
	t_config	cfg;
	t_coder		coder;
	long long	before;
	long long	after;

	printf("--- test_set_compile_start_timestamp: last_compile_start debe ser now_ms() ---\n");
	init_sim_minimal(&sim, &cfg);
	coder = sim.coders[0];
	before = now_ms();
	usleep(5000);
	set_compile_start(&coder);
	after = now_ms();
	printf("  last_compile_start = %lld, rango esperado [%lld, %lld]\n",
		coder.last_compile_start, before, after);
	if (coder.last_compile_start >= before
		&& coder.last_compile_start <= after)
		printf("  OK: timestamp dentro del rango\n\n");
	else
	{
		printf("  FAIL: timestamp fuera de rango\n\n");
		g_fail = 1;
	}
	cleanup_sim(&sim);
}

static void	test_take_release_single_coder(void)
{
	t_sim		sim;
	t_config	cfg;

	printf("--- test_take_release_single_coder: take/release sincrono con 1 coder ---\n");
	init_sim_minimal(&sim, &cfg);
	if (sim.coders[0].first != &sim.dongles[0]
		|| sim.coders[0].second != &sim.dongles[0])
	{
		printf("  FAIL: first/second no apuntan a dongles[0]\n\n");
		g_fail = 1;
	}
	else
		printf("  OK: first == second == &dongles[0]\n");
	take_dongles(&sim.coders[0]);
	if (sim.dongles[0].holder == 1)
		printf("  OK: holder == 1 tras take_dongles\n");
	else
	{
		printf("  FAIL: holder == %d tras take_dongles (esperado 1)\n\n",
			sim.dongles[0].holder);
		g_fail = 1;
	}
	release_dongles(&sim.coders[0]);
	if (sim.dongles[0].holder == 0)
		printf("  OK: holder == 0 tras release_dongles\n\n");
	else
	{
		printf("  FAIL: holder == %d tras release_dongles (esperado 0)\n\n",
			sim.dongles[0].holder);
		g_fail = 1;
	}
	cleanup_sim(&sim);
}

static void	test_shutdown_wakes_waiters(void)
{
	t_sim		sim;
	t_config	cfg;
	pthread_t	threads[2];
	long long	elapsed;
	int			i;

	printf("--- test_shutdown_wakes_waiters: sim_stop despierta a los coders bloqueados ---\n");
	cfg.number_of_coders = 2;
	cfg.time_to_burnout = 800;
	cfg.time_to_compile = 10;
	cfg.time_to_debug = 10;
	cfg.time_to_refactor = 10;
	cfg.number_of_compiles_required = 999999;
	cfg.dongle_cooldown = 0;
	cfg.scheduler = SCHEDULER_FIFO;
	if (!init_sim(&sim, &cfg))
	{
		printf("  FAIL: init_sim fallo\n\n");
		g_fail = 1;
		return ;
	}
	i = -1;
	while (++i < 2)
		pthread_create(&threads[i], NULL, coder_routine, &sim.coders[i]);
	usleep(150000);
	sim_stop(&sim);
	elapsed = now_ms();
	pthread_join(threads[0], NULL);
	pthread_join(threads[1], NULL);
	elapsed = now_ms() - elapsed;
	printf("  joins tras sim_stop: %lld ms\n", elapsed);
	if (elapsed < 1000)
		printf("  OK: shutdown desperto a los coders bloqueados\n\n");
	else
	{
		printf("  FAIL: joins tardaron %lld ms (esperado < 1000)\n\n", elapsed);
		g_fail = 1;
	}
	cleanup_sim(&sim);
}

static void	test_coder_routine_exits_on_stop(void)
{
	t_sim		sim;
	t_config	cfg;
	t_coder		coder;
	void		*ret;

	printf("--- test_coder_routine_exits_on_stop: debe salir al instante con stop=1 ---\n");
	init_sim_minimal(&sim, &cfg);
	coder = sim.coders[0];
	sim_stop(&sim);
	coder_routine(&coder);
	ret = coder_routine(&coder);
	printf("  coder_routine retorno sin bloqueo (ret=%p)\n", ret);
	if (sim_stopped(&sim))
		printf("  OK: coder_routine termino correctamente con stop=1\n\n");
	else
	{
		printf("  FAIL: sim_stopped() no devuelve 1 despues de sim_stop()\n\n");
		g_fail = 1;
	}
	cleanup_sim(&sim);
}

static void	test_coder_routine_one_iteration(void)
{
	t_sim		sim;
	t_config	cfg;
	t_coder		coder;
	int			count_before;
	pthread_t	thread;

	printf("--- test_coder_routine_one_iteration: una iteracion completa antes de stop ---\n");
	init_sim_minimal(&sim, &cfg);
	coder = sim.coders[0];
	count_before = coder.compile_count;
	pthread_create(&thread, NULL, coder_routine, &coder);
	usleep(50000);
	sim_stop(&sim);
	pthread_join(thread, NULL);
	printf("  compile_count: %d -> %d (esperado incremento)\n",
		count_before, coder.compile_count);
	if (coder.compile_count > count_before)
		printf("  OK: al menos una iteracion completada\n\n");
	else
	{
		printf("  FAIL: compile_count no incremento\n\n");
		g_fail = 1;
	}
	cleanup_sim(&sim);
}

static void	test_compiles_required_stops(void)
{
	t_sim		sim;
	t_config	cfg;
	t_coder		coder;
	pthread_t	thread;
	long long	start;

	printf("--- test_compiles_required_stops: el hilo debe terminar SOLO tras N compilaciones (sin sim_stop) ---\n");
	init_sim_minimal(&sim, &cfg);
	sim.config.number_of_compiles_required = 2;
	coder = sim.coders[0];
	start = now_ms();
	pthread_create(&thread, NULL, coder_routine, &coder);
	pthread_join(thread, NULL);
	start = now_ms() - start;
	printf("  compile_count: %d (esperado 2), el hilo termino solo en %lld ms\n",
		coder.compile_count, start);
	if (coder.compile_count == 2 && start < 2000)
		printf("  OK: parada natural al alcanzar number_of_compiles_required\n\n");
	else
	{
		printf("  FAIL: compile_count=%d, t=%lldms (esperado 2 y <2000ms)\n\n",
			coder.compile_count, start);
		g_fail = 1;
	}
	cleanup_sim(&sim);
}

int	main(void)
{
	g_fail = 0;
	printf("\n=== TESTS: coder_routine ===\n\n");
	test_set_compile_start_count();
	test_set_compile_start_timestamp();
	test_take_release_single_coder();
	test_shutdown_wakes_waiters();
	test_coder_routine_exits_on_stop();
	test_coder_routine_one_iteration();
	test_compiles_required_stops();
	if (g_fail)
		printf("=== ALGUN TEST FALLO ===\n");
	else
		printf("=== TODOS LOS TESTS PASARON ===\n");
	return (g_fail);
}
