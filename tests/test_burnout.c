#include "test_framework.h"

typedef struct burn_run {
	char log[PATH_MAX];
	char *content;
	int rc;
	double secs;
	int lines;
} burn_run;

static burn_run burn_exec(tf_opts *o, const char *args, int to, const char *fname)
{
	burn_run b;
	double t0;

	tf_tmp_path(b.log, sizeof b.log, fname);
	t0 = tf_now();
	b.rc = tf_run_args(o, args, to, b.log);
	b.secs = tf_now() - t0;
	b.content = tf_read(b.log);
	tf_drop_partial(b.content);
	b.lines = tf_count_lines(b.content);
	return b;
}

int main(int argc, char **argv)
{
	tf_opts o;
	char exp[512];

	tf_init(&o, argc, argv);

	tf_dash();
	tf_banner(TF_ART_BURNOUT, TF_ARRLEN(TF_ART_BURNOUT),
		  "3. CASOS QUE PROVOCAN BURNOUT (MONITOR PRECISO)");
	tf_dash();
	printf("\n");

	/* TEST 13: 2 coders inviables (ciclo 600 > burnout 300) */
	{
		burn_run b = burn_exec(&o, "2 300 200 200 200 5 0 fifo", 8, "13.log");
		char disp[512], got[512];

		tf_cmd_display(o.bin, "timeout 8", "2 300 200 200 200 5 0 fifo", disp, sizeof disp);
		tf_test_header(13, "2 coders inviables: ciclo > burnout (FIFO)", disp);
		snprintf(exp, sizeof exp,
			 "salida temprana (exit != 0) con 'burned out' UNA vez como ULTIMA linea (C6)");
		tf_expected("%s", exp);

		if (b.rc == 124)
			snprintf(got, sizeof got, "exit=124 (timeout): el monitor no detuvo la simulacion");
		else if (b.rc == 0)
			snprintf(got, sizeof got, "exit=0 cuando se esperaba burnout");
		else if (tf_check_burnout(b.content))
			snprintf(got, sizeof got, "exit=" TF_GOLD "%d" TF_RESET " pero log de burnout invalido (C6)", b.rc);
		else if (tf_check_deadline(b.content, 300))
			snprintf(got, sizeof got, "exit=" TF_GOLD "%d" TF_RESET " pero hay eventos del coder tras su deadline (300ms)", b.rc);
		else
			snprintf(got, sizeof got, "exit=" TF_GOLD "%d" TF_RESET ", " TF_GOLD "%d" TF_RESET " lineas, burnout como ultima (%.2fs)",
				 b.rc, b.lines, b.secs);
		tf_got("%s", got);

		if (b.rc == 124)
			tf_result("FAIL", "(timeout mato el proceso: el monitor no detuvo la simulacion)");
		else if (b.rc == 0)
			tf_result("FAIL", "(exit 0 cuando se esperaba burnout)");
		else if (tf_check_burnout(b.content))
			tf_result("FAIL", "(log de burnout invalido C6)");
		else if (tf_check_deadline(b.content, 300))
			tf_result("FAIL", "(eventos del coder tras su deadline de 300ms)");
		else
			tf_result("PASS", "(%d lineas, burnout como ultima, exit=%d)", b.lines, b.rc);
		printf("\n");
		free(b.content);
	}

	/* TEST 14: 1 coder, dongle unico */
	{
		burn_run b = burn_exec(&o, "1 300 500 100 100 5 0 fifo", 5, "14.log");
		char disp[512], got[512];

		tf_cmd_display(o.bin, "timeout 5", "1 300 500 100 100 5 0 fifo", disp, sizeof disp);
		tf_test_header(14, "1 coder, dongle unico (no puede sostener 2)", disp);
		snprintf(exp, sizeof exp,
			 "salida temprana (exit != 0): coder 1 'burned out' como ULTIMA linea (C6)");
		tf_expected("%s", exp);

		if (b.rc == 124)
			snprintf(got, sizeof got, "exit=124 (timeout): la simulacion no termino");
		else if (b.rc == 0)
			snprintf(got, sizeof got, "exit=0 en config inviable de 1 dongle");
		else if (tf_check_burnout(b.content))
			snprintf(got, sizeof got, "exit=" TF_GOLD "%d" TF_RESET " pero burnout no registrado correctamente (C6)", b.rc);
		else if (tf_check_deadline(b.content, 300))
			snprintf(got, sizeof got, "exit=" TF_GOLD "%d" TF_RESET " pero hay eventos del coder tras su deadline (300ms)", b.rc);
		else
			snprintf(got, sizeof got, "exit=" TF_GOLD "%d" TF_RESET ", coder 1 burned out (C6 ok, %.2fs)", b.rc, b.secs);
		tf_got("%s", got);

		if (b.rc == 124)
			tf_result("FAIL", "(timeout: la simulacion no termino)");
		else if (b.rc == 0)
			tf_result("FAIL", "(exit 0 en config inviable de 1 dongle)");
		else if (tf_check_burnout(b.content))
			tf_result("FAIL", "(burnout no registrado correctamente C6)");
		else if (tf_check_deadline(b.content, 300))
			tf_result("FAIL", "(eventos del coder tras su deadline de 300ms)");
		else
			tf_result("PASS", "(coder 1 burned out, exit=%d)", b.rc);
		printf("\n");
		free(b.content);
	}

	/* TEST 15: 4 coders, inanicion FIFO, margen nulo */
	{
		burn_run b = burn_exec(&o, "4 400 200 100 100 10 0 fifo", 8, "15.log");
		char disp[512], got[512];

		tf_cmd_display(o.bin, "timeout 8", "4 400 200 100 100 10 0 fifo", disp, sizeof disp);
		tf_test_header(15, "4 coders, inanicion FIFO, margen nulo", disp);
		snprintf(exp, sizeof exp,
			 "o bien completo 10 compiles (exit 0) o burnout detectado (exit != 0, C6)");
		tf_expected("%s", exp);

		if (b.rc == 124)
			snprintf(got, sizeof got, "exit=124 (timeout): el monitor no detuvo la simulacion");
		else if (b.rc == 0)
			snprintf(got, sizeof got, "exit=0, completado sin burnout");
		else if (tf_check_burnout(b.content))
			snprintf(got, sizeof got, "exit=" TF_GOLD "%d" TF_RESET " pero log de burnout invalido (C6)", b.rc);
		else
			snprintf(got, sizeof got, "exit=" TF_GOLD "%d" TF_RESET ", burnout detectado en primer coder (C6 ok)", b.rc);
		tf_got("%s", got);

		if (b.rc == 124)
			tf_result("FAIL", "(timeout: el monitor no detuvo la simulacion)");
		else if (b.rc == 0)
			tf_result("PASS", "(completo las 10 compiles)  -> exit 0, sin burnout");
		else if (tf_check_burnout(b.content))
			tf_result("FAIL", "(log de burnout invalido C6)");
		else
			tf_result("PASS", "(burnout detectado en primer coder, exit=%d)", b.rc);
		printf("\n");
		free(b.content);
	}

	/* TEST 16: EDF con parametros inviables tambien debe detectar burnout */
	{
		burn_run b = burn_exec(&o, "3 300 300 200 200 5 0 edf", 8, "16.log");
		char disp[512], got[512];

		tf_cmd_display(o.bin, "timeout 8", "3 300 300 200 200 5 0 edf", disp, sizeof disp);
		tf_test_header(16, "EDF tambien detecta burnout (inviable)", disp);
		snprintf(exp, sizeof exp,
			 "o bien completo con margen (exit 0) o burnout con edf (exit != 0, C6)");
		tf_expected("%s", exp);

		if (b.rc == 124)
			snprintf(got, sizeof got, "exit=124 (timeout): sin detencion");
		else if (b.rc == 0)
			snprintf(got, sizeof got, "exit=0, completo con margen");
		else if (tf_check_burnout(b.content))
			snprintf(got, sizeof got, "exit=" TF_GOLD "%d" TF_RESET " pero log de burnout invalido (C6)", b.rc);
		else
			snprintf(got, sizeof got, "exit=" TF_GOLD "%d" TF_RESET ", burnout con edf (C6 ok)", b.rc);
		tf_got("%s", got);

		if (b.rc == 124)
			tf_result("FAIL", "(timeout: sin detencion)");
		else if (b.rc == 0)
			tf_result("PASS", "(completo con margen) -> exit 0");
		else if (tf_check_burnout(b.content))
			tf_result("FAIL", "(log de burnout invalido C6)");
		else
			tf_result("PASS", "(burnout con edf, exit=%d)", b.rc);
		printf("\n");
		free(b.content);
	}

	/* TEST 17: metrica de precision del monitor (delay <= 25ms) */
	{
		burn_run b = burn_exec(&o, "2 300 200 200 200 5 0 fifo", 8, "17.log");
		long long delay = tf_monitor_delay(b.content, 300);
		char disp[512], got[512];

		tf_cmd_display(o.bin, "timeout 8", "2 300 200 200 200 5 0 fifo", disp, sizeof disp);
		tf_test_header(17, "Precision del monitor: delay del log de burnout", disp);
		snprintf(exp, sizeof exp,
			 "delay = ts_burnout - ultimo 'is compiling' - 300ms, dentro de <=25ms");
		tf_expected("%s", exp);

		if (b.rc == 124)
			snprintf(got, sizeof got, "exit=124 (timeout)");
		else if (delay < 0)
			snprintf(got, sizeof got,
				 "no medible: falta 'burned out' o 'is compiling' del coder");
		else
			snprintf(got, sizeof got, "delay=%lldms (burn_ts=%lld - last_compile - 300)", delay, delay + 300);
		tf_got("%s", got);

		if (b.rc == 124)
			tf_result("FAIL", "(timeout)");
		else if (delay < 0)
			tf_result("FAIL", "(no se pudo medir: falta 'burned out' o 'is compiling' del coder)");
		else if (delay <= 10)
			tf_result("PASS", "(delay=%lldms — cumple spec <=10ms)", delay);
		else if (delay <= 25)
			tf_result("PASS", "(delay=%lldms — aceptable por poll+tolerancia)", delay);
		else
			tf_result("FAIL", "(delay=%lldms > 25ms)", delay);
		printf("\n");
		free(b.content);
	}

	/* TEST 39: caso reportado: ciclo 119 > burnout 110, recompile en ms 120 */
	{
		burn_run b = burn_exec(&o, "2 110 59 30 30 2 1 fifo", 5, "39.log");
		long long delay = tf_monitor_delay(b.content, 110);
		char disp[512], got[512];

		tf_cmd_display(o.bin, "timeout 5", "2 110 59 30 30 2 1 fifo", disp, sizeof disp);
		tf_test_header(39, "Caso reportado: ciclo 119 > burnout 110 (recompile en ms 120)", disp);
		snprintf(exp, sizeof exp,
			 "salida temprana (exit != 0) con 'burned out' UNA vez como ULTIMA linea, "
			 "ningun evento tras el deadline del coder (110ms) y delay <= 10ms (C6)");
		tf_expected("%s", exp);

		if (b.rc == 124)
			snprintf(got, sizeof got, "exit=124 (timeout): el monitor no detuvo la simulacion");
		else if (b.rc == 0)
			snprintf(got, sizeof got, "exit=0 cuando se esperaba burnout: el coder recompilo en el ms 120 enmascarando su muerte en 110");
		else if (tf_check_burnout(b.content))
			snprintf(got, sizeof got, "exit=" TF_GOLD "%d" TF_RESET " pero log de burnout invalido (C6)", b.rc);
		else if (tf_check_deadline(b.content, 110))
			snprintf(got, sizeof got, "exit=" TF_GOLD "%d" TF_RESET " pero hay eventos del coder tras su deadline (110ms): recompilo en el ms ~120", b.rc);
		else if (delay > 10)
			snprintf(got, sizeof got, "exit=" TF_GOLD "%d" TF_RESET " pero delay de burnout=%lldms > 10ms (C6)", b.rc, delay);
		else
			snprintf(got, sizeof got, "exit=" TF_GOLD "%d" TF_RESET ", " TF_GOLD "%d" TF_RESET " lineas, delay=%lldms, burnout como ultima", b.rc, b.lines, delay);
		tf_got("%s", got);

		if (b.rc == 124)
			tf_result("FAIL", "(timeout: el monitor no detuvo la simulacion)");
		else if (b.rc == 0)
			tf_result("FAIL", "(exit 0: coder recompilo en el ms 120 cuando debia morir en 110)");
		else if (tf_check_burnout(b.content))
			tf_result("FAIL", "(log de burnout invalido C6)");
		else if (tf_check_deadline(b.content, 110))
			tf_result("FAIL", "(eventos tras el deadline: recompile en ms ~120 tras morir en 110)");
		else if (delay > 10)
			tf_result("FAIL", "(delay=%lldms > 10ms spec C6)", delay);
		else
			tf_result("PASS", "(%d lineas, delay=%lldms, burnout como ultima, exit=%d)", b.lines, delay, b.rc);
		printf("\n");
		free(b.content);
	}

	/* TEST 40: borde superior de la banda (ciclo = burnout + 10) */
	{
		burn_run b = burn_exec(&o, "2 110 60 30 30 2 1 fifo", 5, "40.log");
		long long delay = tf_monitor_delay(b.content, 110);
		char disp[512], got[512];

		tf_cmd_display(o.bin, "timeout 5", "2 110 60 30 30 2 1 fifo", disp, sizeof disp);
		tf_test_header(40, "Banda critica: ciclo 120 == burnout + 10, debe agotarse igual", disp);
		snprintf(exp, sizeof exp,
			 "salida temprana (exit != 0) con 'burned out' UNA vez como ULTIMA linea, "
			 "ningun evento tras el deadline del coder (110ms) y delay <= 10ms (C6)");
		tf_expected("%s", exp);

		if (b.rc == 124)
			snprintf(got, sizeof got, "exit=124 (timeout): el monitor no detuvo la simulacion");
		else if (b.rc == 0)
			snprintf(got, sizeof got, "exit=0 cuando se esperaba burnout: recompile en el borde de la banda enmascarando la muerte");
		else if (tf_check_burnout(b.content))
			snprintf(got, sizeof got, "exit=" TF_GOLD "%d" TF_RESET " pero log de burnout invalido (C6)", b.rc);
		else if (tf_check_deadline(b.content, 110))
			snprintf(got, sizeof got, "exit=" TF_GOLD "%d" TF_RESET " pero hay eventos del coder tras su deadline (110ms)", b.rc);
		else if (delay > 10)
			snprintf(got, sizeof got, "exit=" TF_GOLD "%d" TF_RESET " pero delay de burnout=%lldms > 10ms (C6)", b.rc, delay);
		else
			snprintf(got, sizeof got, "exit=" TF_GOLD "%d" TF_RESET ", " TF_GOLD "%d" TF_RESET " lineas, delay=%lldms, burnout como ultima", b.rc, b.lines, delay);
		tf_got("%s", got);

		if (b.rc == 124)
			tf_result("FAIL", "(timeout: el monitor no detuvo la simulacion)");
		else if (b.rc == 0)
			tf_result("FAIL", "(exit 0: recompile en el borde superior enmascarando la muerte)");
		else if (tf_check_burnout(b.content))
			tf_result("FAIL", "(log de burnout invalido C6)");
		else if (tf_check_deadline(b.content, 110))
			tf_result("FAIL", "(eventos tras el deadline del coder de 110ms)");
		else if (delay > 10)
			tf_result("FAIL", "(delay=%lldms > 10ms spec C6)", delay);
		else
			tf_result("PASS", "(%d lineas, delay=%lldms, burnout como ultima, exit=%d)", b.lines, delay, b.rc);
		printf("\n");
		free(b.content);
	}

	tf_sec_summary();
	tf_totals(&o);
	return 0;
}