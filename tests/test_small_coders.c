#include "test_framework.h"

#define EXIT_OKALOGA \
	"exit 0, log con formato C1-C5, 2 tomas de dongle por compile, >=%d compiles por coder"

int main(int argc, char **argv)
{
	tf_opts o;
	char exp[256];

	tf_init(&o, argc, argv);

	tf_dash();
	tf_banner(TF_ART_NUCLEO, TF_ARRLEN(TF_ART_NUCLEO),
		  "1. CASOS VALIDOS CON NUMEROS DE CODERS PEQUENOS");
	tf_dash();
	printf("\n");

	snprintf(exp, sizeof exp, EXIT_OKALOGA, 3);
	tf_run_viable(&o, 1, "2 coders, FIFO, cooldown 0",
		      "2 800 200 100 100 3 0 fifo", exp, 2, 3, 20);

	snprintf(exp, sizeof exp, EXIT_OKALOGA, 3);
	tf_run_viable(&o, 2, "2 coders, EDF con cooldown 50",
		      "2 800 200 100 100 3 50 edf", exp, 2, 3, 20);

	snprintf(exp, sizeof exp, EXIT_OKALOGA, 4);
	tf_run_viable(&o, 3, "3 coders, FIFO, mesa circular",
		      "3 800 150 100 100 4 0 fifo", exp, 3, 4, 20);

	snprintf(exp, sizeof exp, EXIT_OKALOGA, 5);
	tf_run_viable(&o, 4, "4 coders, EDF con cooldown 100",
		      "4 900 200 150 150 5 100 edf", exp, 4, 5, 30);

	snprintf(exp, sizeof exp, EXIT_OKALOGA, 5);
	tf_run_viable(&o, 5, "5 coders, FIFO regimen ajustado",
		      "5 800 200 200 200 5 0 fifo", exp, 5, 5, 20);

	snprintf(exp, sizeof exp, EXIT_OKALOGA, 4);
	tf_run_viable(&o, 6, "8 coders, EDF cooldown 50",
		      "8 1500 300 200 200 4 50 edf", exp, 8, 4, 40);

	snprintf(exp, sizeof exp, EXIT_OKALOGA, 3);
	tf_run_viable(&o, 7, "10 coders, FIFO baja contencion",
		      "10 2000 200 100 100 3 0 fifo", exp, 10, 3, 20);

	tf_sec_summary();
	tf_totals(&o);
	return 0;
}