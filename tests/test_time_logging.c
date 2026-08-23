#include "../coders/includes/codexion.h"
#include <stdio.h>

static int	g_fail;

static void	init_test_sim(t_sim *sim)
{
	sim->start_time_ms = now_ms();
	sim->stop = 0;
	pthread_mutex_init(&sim->print_mutex, NULL);
	pthread_mutex_init(&sim->stop_mutex, NULL);
}

static void	test_now_ms(void)
{
	long long	a;
	long long	b;

	printf("--- test_now_ms: now_ms() debe devolver timestamp en ms > 0 ---\n");
	a = now_ms();
	b = now_ms();
	printf("  now_ms() = %lld, now_ms() de nuevo = %lld\n", a, b);
	if (a > 0 && b >= a)
		printf("  OK: ambos valores > 0 y b >= a\n\n");
	else
	{
		printf("  FAIL: esperaba ambos > 0 y b >= a\n\n");
		g_fail = 1;
	}
}

static void	test_elapsed_ms(void)
{
	t_sim		sim;
	long long	elapsed;

	printf("--- test_elapsed_ms: debe calcular tiempo transcurrido desde start ---\n");
	init_test_sim(&sim);
	printf("  start_time_ms = %lld\n", sim.start_time_ms);
	usleep(30000);
	elapsed = elapsed_ms(&sim);
	pthread_mutex_destroy(&sim.print_mutex);
	pthread_mutex_destroy(&sim.stop_mutex);
	printf("  usleep(30000), elapsed_ms() = %lldms (esperado ~30ms, rango 20-50ms)\n",
		elapsed);
	if (elapsed >= 20 && elapsed <= 50)
		printf("  OK: elapsed dentro del rango esperado\n\n");
	else
	{
		printf("  FAIL: elapsed fuera de rango\n\n");
		g_fail = 1;
	}
}

static void	test_sim_stopped_default(void)
{
	t_sim	sim;

	printf("--- test_sim_stopped_default: sim_stopped() debe devolver 0 si stop=0 ---\n");
	init_test_sim(&sim);
	printf("  sim.stop = %d\n", sim.stop);
	if (sim_stopped(&sim) == 0)
		printf("  OK: sim_stopped() = 0\n\n");
	else
	{
		printf("  FAIL: sim_stopped() devolvio %d, esperaba 0\n\n",
			sim_stopped(&sim));
		g_fail = 1;
	}
	pthread_mutex_destroy(&sim.print_mutex);
	pthread_mutex_destroy(&sim.stop_mutex);
}

static void	test_sim_stop_sets_flag(void)
{
	t_sim	sim;

	printf("--- test_sim_stop_sets_flag: sim_stop() debe poner stop=1 ---\n");
	init_test_sim(&sim);
	printf("  antes: sim.stop = %d\n", sim.stop);
	sim_stop(&sim);
	printf("  despues de sim_stop(): sim.stop = %d (esperado 1)\n", sim.stop);
	if (sim_stopped(&sim) == 1)
		printf("  OK: sim_stopped() = 1\n\n");
	else
	{
		printf("  FAIL: sim_stopped() devolvio %d, esperaba 1\n\n",
			sim_stopped(&sim));
		g_fail = 1;
	}
	pthread_mutex_destroy(&sim.print_mutex);
	pthread_mutex_destroy(&sim.stop_mutex);
}

static void	test_log_state(void)
{
	t_sim	sim;

	printf("--- test_log_state: log_state() debe imprimir sin crashear ---\n");
	init_test_sim(&sim);
	printf("  salida de log_state(&sim, 1, \"hola\"): ");
	log_state(&sim, 1, "hola");
	pthread_mutex_destroy(&sim.print_mutex);
	pthread_mutex_destroy(&sim.stop_mutex);
	printf("  OK: log_state no crasheo\n\n");
}

static void	test_smart_sleep(void)
{
	t_sim		sim;
	long long	before;
	long long	after;

	printf("--- test_smart_sleep: smart_sleep(50) debe esperar ~50ms ---\n");
	init_test_sim(&sim);
	before = now_ms();
	smart_sleep(50, &sim);
	after = now_ms();
	pthread_mutex_destroy(&sim.print_mutex);
	pthread_mutex_destroy(&sim.stop_mutex);
	printf("  smart_sleep(50), duracion real = %lldms (esperado >= 30ms)\n",
		after - before);
	if (after - before >= 30)
		printf("  OK: duracion dentro del rango esperado\n\n");
	else
	{
		printf("  FAIL: duracion fuera de rango\n\n");
		g_fail = 1;
	}
}

int	main(void)
{
	g_fail = 0;
	printf("\n=== TESTS: time_logging ===\n\n");
	test_now_ms();
	test_elapsed_ms();
	test_sim_stopped_default();
	test_sim_stop_sets_flag();
	test_log_state();
	test_smart_sleep();
	if (g_fail)
		printf("=== ALGUN TEST FALLO ===\n");
	else
		printf("=== TODOS LOS TESTS PASARON ===\n");
	return (g_fail);
}
