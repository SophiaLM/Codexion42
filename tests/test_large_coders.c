#include "test_framework.h"

#define EXIT_OKALOGA \
	"exit 0, log con formato C1-C5, 2 tomas de dongle por compile, >=%d compiles por coder"

int main(int argc, char **argv)
{
	tf_opts o;
	char exp[256];

	tf_init(&o, argc, argv);

	tf_dash();
	tf_banner(TF_ART_ESCALA, TF_ARRLEN(TF_ART_ESCALA),
		  "2. CASOS VALIDOS CON NUMEROS DE CODERS GRANDES");
	tf_dash();
	printf("\n");

	if (!o.smoke) {
		snprintf(exp, sizeof exp, EXIT_OKALOGA, 5);
		tf_run_viable(&o, 8, "50 coders, EDF cooldown 10",
			      "50 2000 200 100 100 5 10 edf", exp, 50, 5, 60);

		snprintf(exp, sizeof exp, EXIT_OKALOGA, 3);
		tf_run_viable(&o, 9, "100 coders, FIFO",
			      "100 3000 300 150 150 3 0 fifo", exp, 100, 3, 90);

		snprintf(exp, sizeof exp, EXIT_OKALOGA, 3);
		tf_run_viable(&o, 10, "200 coders, EDF stress",
			      "200 3000 300 200 200 3 0 edf", exp, 200, 3, 120);

		snprintf(exp, sizeof exp, EXIT_OKALOGA, 2);
		tf_run_viable(&o, 11, "500 coders, FIFO escala maxima",
			      "500 5000 400 200 200 2 0 fifo", exp, 500, 2, 180);
	} else {
		printf("%s[NOTE] Modo smoke: tests 08-11 (gran escala) omitidos%s\n\n",
		       TF_GOLD, TF_RESET);
	}

	/* TEST 12: estres en rafaga */
	{
		char disp[512], got[256], log[PATH_MAX];
		int runs = 10, crash = 0, empty = 0, i, rc;

		tf_cmd_display(o.bin, "timeout 2",
			       "30 1500 200 100 100 3 0 fifo  (repetido x10)",
			       disp, sizeof disp);
		tf_test_header(12, "Estres en rafaga: 10 corridas de 30 coders (2s c/u)", disp);
		tf_expected("10 corridas sin segfault(139)/abort(134) ni log vacio");

		for (i = 0; i < runs; i++) {
			char fname[64];
			snprintf(fname, sizeof fname, "stress_%d.log", i + 1);
			tf_tmp_path(log, sizeof log, fname);
			rc = tf_run_args(&o, "30 1500 200 100 100 3 0 fifo", 2, log);
			if (rc == 139 || rc == 134) {
				crash++;
			} else {
				char *content = tf_read(log);
				if (!content[0])
					empty++;
				free(content);
			}
		}

		snprintf(got, sizeof got, "%d corridas: %d crashes(139/134), %d logs vacios",
			 runs, crash, empty);
		tf_got("%s", got);

		if (crash == 0 && empty == 0)
			tf_result("PASS", "(%d corridas sin 139/134 ni log vacio)", runs);
		else
			tf_result("FAIL", "(%d crashes, %d logs vacios)", crash, empty);
		printf("\n");
	}

	tf_sec_summary();
	tf_totals(&o);
	return 0;
}