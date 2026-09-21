#include "test_framework.h"

int main(int argc, char **argv)
{
	tf_opts o;
	char exp[512];

	tf_init(&o, argc, argv);

	tf_dash();
	tf_banner(TF_ART_VALGRIND, TF_ARRLEN(TF_ART_VALGRIND),
		  "5. MEMORIA: VALGRIND --leak-check=full");
	tf_dash();
	printf("\n");

	snprintf(exp, sizeof exp,
		 "valgrind sin errores de memoria y 0 'definitely lost' (entrada viable)");
	tf_run_valgrind(&o, 35, "Valgrind: ejecucion normal (viable)",
			"4 800 200 100 100 3 10 edf", exp);

	snprintf(exp, sizeof exp,
		 "valgrind sin errores de memoria aun con salida temprana por burnout");
	tf_run_valgrind(&o, 36, "Valgrind: salida temprana por burnout",
			"2 300 500 100 100 5 0 fifo", exp);

	if (!o.smoke) {
		snprintf(exp, sizeof exp,
			 "valgrind sin errores de memoria a gran escala (50 coders EDF)");
		tf_run_valgrind(&o, 37, "Valgrind: gran escala (50 coders EDF)",
				"50 2000 200 100 100 5 10 edf", exp);
	} else {
		printf("%s[NOTE] Modo smoke: test 37 (valgrind gran escala) omitido%s\n\n",
		       TF_GOLD, TF_RESET);
	}

	snprintf(exp, sizeof exp,
		 "valgrind sin errores en la ruta de error de parsing (exit != 0 esperado)");
	tf_run_valgrind(&o, 38, "Valgrind: ruta de error de parsing",
			"4 800 200 100 100", exp);

	tf_sec_summary();
	tf_totals(&o);
	return 0;
}