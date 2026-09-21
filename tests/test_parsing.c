#include "test_framework.h"

/* construye un comando de prueba: el binario + parametros, terminado en NULL */
static tf_cmd mk(tf_opts *o, ...)
{
	va_list ap;
	tf_cmd c;
	const char *arg;

	c.argc = 0;
	c.argv[c.argc++] = o->bin;

	va_start(ap, o);
	while ((arg = va_arg(ap, const char *)) != NULL && c.argc < TF_MAXARGS)
		c.argv[c.argc++] = arg;
	va_end(ap);
	c.argv[c.argc] = NULL;
	return c;
}

int main(int argc, char **argv)
{
	tf_opts o;
	char exp[512];

	tf_init(&o, argc, argv);

	tf_dash();
	tf_banner(TF_ART_PARSING, TF_ARRLEN(TF_ART_PARSING),
		  "4. TODOS LOS CASOS DONDE DEBE FALLAR EL PARSING");
	tf_dash();
	printf("\n");

	/* ==================== fallos de parsing (run_fail) ==================== */
	{
		tf_cmd cmds[] = { mk(&o, NULL) };
		snprintf(exp, sizeof exp, "exit != 0 y mensaje de error a stderr");
		tf_run_fail(&o, 18, "Sin argumentos", exp, cmds, 1);
	}

	{
		tf_cmd cmds[] = { mk(&o, "4", "800", "200", "100", NULL) };
		snprintf(exp, sizeof exp, "exit != 0 y mensaje de error a stderr");
		tf_run_fail(&o, 19, "Argumentos faltantes", exp, cmds, 1);
	}

	{
		tf_cmd cmds[] = { mk(&o, "4", "800", "200", "100", "100", "5", "10", "fifo", "extra", NULL) };
		snprintf(exp, sizeof exp, "exit != 0 y mensaje de error a stderr");
		tf_run_fail(&o, 20, "Argumentos de mas", exp, cmds, 1);
	}

	{
		tf_cmd cmds[] = { mk(&o, "-5", "800", "200", "100", "100", "5", "10", "fifo", NULL) };
		snprintf(exp, sizeof exp, "exit != 0 y mensaje de error a stderr");
		tf_run_fail(&o, 21, "Numero de coders negativo", exp, cmds, 1);
	}

	{
		tf_cmd cmds[] = { mk(&o, "4", "-800", "200", "100", "100", "5", "10", "fifo", NULL) };
		snprintf(exp, sizeof exp, "exit != 0 y mensaje de error a stderr");
		tf_run_fail(&o, 22, "Tiempo negativo", exp, cmds, 1);
	}

	{
		tf_cmd cmds[] = { mk(&o, "4", "800", "200", "100", "100", "5", "-10", "fifo", NULL) };
		snprintf(exp, sizeof exp, "exit != 0 y mensaje de error a stderr");
		tf_run_fail(&o, 23, "Cooldown negativo", exp, cmds, 1);
	}

	{
		tf_cmd cmds[] = { mk(&o, "0", "800", "200", "100", "100", "5", "10", "fifo", NULL) };
		snprintf(exp, sizeof exp, "exit != 0 y mensaje de error a stderr");
		tf_run_fail(&o, 24, "Cero en numero de coders", exp, cmds, 1);
	}

	{
		tf_cmd cmds[] = {
			mk(&o, "4", "0", "200", "100", "100", "5", "10", "fifo", NULL),
			mk(&o, "4", "800", "0", "100", "100", "5", "10", "fifo", NULL),
			mk(&o, "4", "800", "200", "0", "100", "5", "10", "fifo", NULL)
		};
		snprintf(exp, sizeof exp, "exit != 0 y mensaje de error a stderr (3 variantes)");
		tf_run_fail(&o, 25, "Cero en tiempos", exp, cmds, 3);
	}

	{
		tf_cmd cmds[] = { mk(&o, "4", "800", "abc", "100", "100", "5", "10", "edf", NULL) };
		snprintf(exp, sizeof exp, "exit != 0 y mensaje de error a stderr");
		tf_run_fail(&o, 26, "Entrada no numerica", exp, cmds, 1);
	}

	{
		tf_cmd cmds[] = {
			mk(&o, "4", "800", "1.5", "100", "100", "5", "10", "edf", NULL),
			mk(&o, "4", "3.14", "200", "100", "100", "5", "10", "fifo", NULL)
		};
		snprintf(exp, sizeof exp, "exit != 0 y mensaje de error a stderr (2 variantes)");
		tf_run_fail(&o, 27, "Valores no enteros (flotantes)", exp, cmds, 2);
	}

	{
		tf_cmd cmds[] = {
			mk(&o, "4", "800", "200ms", "100", "100", "5", "10", "edf", NULL),
			mk(&o, "4", "800", "200", "100", "100", "5", "10a", "edf", NULL)
		};
		snprintf(exp, sizeof exp, "exit != 0 y mensaje de error a stderr (2 variantes)");
		tf_run_fail(&o, 28, "Sufijo / mezcla basura", exp, cmds, 2);
	}

	{
		tf_cmd cmds[] = {
			mk(&o, " 4", "800", "200", "100", "100", "5", "10", "fifo", NULL),
			mk(&o, "+4", "800", "200", "100", "100", "5", "10", "fifo", NULL),
			mk(&o, "", "800", "200", "100", "100", "5", "10", "fifo", NULL)
		};
		snprintf(exp, sizeof exp, "exit != 0 y mensaje de error a stderr (3 variantes)");
		tf_run_fail(&o, 29, "Espacios, signo y cadena vacia", exp, cmds, 3);
	}

	{
		tf_cmd cmds[] = {
			mk(&o, "2147483648", "800", "200", "100", "100", "5", "10", "fifo", NULL),
			mk(&o, "999999999999", "800", "200", "100", "100", "5", "10", "fifo", NULL)
		};
		snprintf(exp, sizeof exp, "exit != 0 y mensaje de error a stderr (2 variantes)");
		tf_run_fail(&o, 30, "Overflow (mas alla de INT_MAX)", exp, cmds, 2);
	}

	{
		tf_cmd cmds[] = { mk(&o, "4", "800", "200", "100", "100", "5", "10", "round_robin", NULL) };
		snprintf(exp, sizeof exp, "exit != 0 y mensaje de error a stderr");
		tf_run_fail(&o, 31, "Scheduler invalido", exp, cmds, 1);
	}

	{
		tf_cmd cmds[] = {
			mk(&o, "4", "800", "200", "100", "100", "5", "10", "FIFO", NULL),
			mk(&o, "4", "800", "200", "100", "100", "5", "10", "fif", NULL),
			mk(&o, "4", "800", "200", "100", "100", "5", "10", "edf ", NULL)
		};
		snprintf(exp, sizeof exp, "exit != 0 y mensaje de error a stderr (3 variantes)");
		tf_run_fail(&o, 32, "Scheduler case / parcial / con espacios", exp, cmds, 3);
	}

	{
		tf_cmd cmds[] = {
			mk(&o, "4", "800", "200", "100", "100", "5", "10", "1", NULL),
			mk(&o, "4", "800", "200", "100", "100", "5", "10", "", NULL)
		};
		snprintf(exp, sizeof exp, "exit != 0 y mensaje de error a stderr (2 variantes)");
		tf_run_fail(&o, 33, "Scheduler numerico o vacio", exp, cmds, 2);
	}

	/* TEST 34: frontera valida — cooldown=0 se acepta (contraejemplo) */
	{
		char log[PATH_MAX], fname[32], disp[512], got[512];
		char *content;
		int rc;
		double t0;

		snprintf(fname, sizeof fname, "34.log");
		tf_tmp_path(log, sizeof log, fname);
		tf_cmd_display(o.bin, "timeout 15", "2 800 200 100 100 3 0 fifo", disp, sizeof disp);

		t0 = tf_now();
		rc = tf_run_args(&o, "2 800 200 100 100 3 0 fifo", 15, log);
		(void)t0;

		content = tf_read(log);
		tf_drop_partial(content);

		tf_test_header(34, "Frontera valida: cooldown=0 se acepta (no es error de parseo)", disp);
		snprintf(exp, sizeof exp,
			 "parseo acepta cooldown=0 y arranca (exit!=1): simulacion valida o timeout(124)");
		tf_expected("%s", exp);

		if (rc != 0 && rc != 124)
			snprintf(got, sizeof got,
				 "exit=%d sin ejecutar: el parsing rechazo el cooldown 0?", rc);
		else if (tf_check_viable(content, 2, 3) == 0)
			snprintf(got, sizeof got, "exit=%d, parseo ok y simulacion valida (C1-C5)", rc);
		else if (rc == 124)
			snprintf(got, sizeof got, "exit=124: arranco y timeout lo corto — parseo OK");
		else
			snprintf(got, sizeof got, "exit=0 pero log invalido tras aceptar cooldown 0");
		tf_got("%s", got);

		if (rc != 0 && rc != 124)
			tf_result("FAIL", "(exit=%d sin ejecutar: el parsing rechazo el cooldown 0?)", rc);
		else if (tf_check_viable(content, 2, 3) == 0)
			tf_result("PASS", "(parseo acepta cooldown 0; simulacion valida exit=%d)", rc);
		else if (rc == 124)
			tf_result("N/D", "(%s arranco y timeout lo corto — parseo OK)", o.bin);
		else
			tf_result("FAIL", "(log invalido tras aceptar cooldown 0)");
		printf("\n");
		free(content);
	}

	tf_sec_summary();
	tf_totals(&o);
	return 0;
}