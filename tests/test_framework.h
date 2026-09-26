#ifndef TEST_FRAMEWORK_H
#define TEST_FRAMEWORK_H

#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>
#include <time.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

/* =============================================================== colores === */
/* Paleta y layout centralizados en tests/theme.sh, que run_tests.sh deriva a
 * tests/theme.h (generado; no editar aca los colores). */
#include "theme.h"

/* ============================================================= contadores === */
static int TF_P, TF_F, TF_ND, TF_SK;

static int    TF_TESTID;
static char   TF_TESTNAME[160];
static char   TF_TESTCMD[1024];
static char   TF_PENDING[4096];
static int    TF_FAIL_IDS[512];
static int    TF_FAIL_N;
static double TF_LAST_DUR;          /* duracion (s) de la ultima ejecucion */

#define TF_ARRLEN(a) ((int)(sizeof(a) / sizeof((a)[0])))
#define TF_MAXARGS 20

/* ============================================================ opciones === */
typedef struct tf_options {
	int smoke;
	const char *bin;
	const char *totals_file;
} tf_opts;

/* ============================================================ comandos === */
typedef struct tf_cmd {
	int argc;
	const char *argv[TF_MAXARGS];
} tf_cmd;

/* ============================================================== ASCII art === */
static const char *const TF_ART_NUCLEO[] = {
"													 ",
"███╗   ██╗██╗   ██╗ ██████╗██╗     ███████╗ ██████╗ ",
"████╗  ██║██║   ██║██╔════╝██║     ██╔════╝██╔═══██╗",
"██╔██╗ ██║██║   ██║██║     ██║     █████╗  ██║   ██║",
"██║╚██╗██║██║   ██║██║     ██║     ██╔══╝  ██║   ██║",
"██║ ╚████║╚██████╔╝╚██████╗███████╗███████╗╚██████╔╝",
"╚═╝  ╚═══╝ ╚═════╝  ╚═════╝╚══════╝╚══════╝ ╚═════╝ ",
"													 "
};

static const char *const TF_ART_ESCALA[] = {
"												 ",
"███████╗███████╗ ██████╗ █████╗ ██╗      █████╗ ",
"██╔════╝██╔════╝██╔════╝██╔══██╗██║     ██╔══██╗",
"█████╗  ███████╗██║     ███████║██║     ███████║",
"██╔══╝  ╚════██║██║     ██╔══██║██║     ██╔══██║",
"███████╗███████║╚██████╗██║  ██║███████╗██║  ██║",
"╚══════╝╚══════╝ ╚═════╝╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝",
"												 "
};

static const char *const TF_ART_BURNOUT[] = {
"													 		   ",
"██████╗ ██╗   ██╗██████╗ ███╗   ██╗ ██████╗ ██╗   ██╗████████╗",
"██╔══██╗██║   ██║██╔══██╗████╗  ██║██╔═══██╗██║   ██║╚══██╔══╝",
"██████╔╝██║   ██║██████╔╝██╔██╗ ██║██║   ██║██║   ██║   ██║   ",
"██╔══██╗██║   ██║██╔══██╗██║╚██╗██║██║   ██║██║   ██║   ██║   ",
"██████╔╝╚██████╔╝██║  ██║██║ ╚████║╚██████╔╝╚██████╔╝   ██║   ",
"╚═════╝  ╚═════╝ ╚═╝  ╚═╝╚═╝  ╚═══╝ ╚═════╝  ╚═════╝    ╚═╝   ",
"													 		   "
};

static const char *const TF_ART_PARSING[] = {
"													   ",
"██████╗  █████╗ ██████╗ ███████╗██╗███╗   ██╗ ██████╗ ",
"██╔══██╗██╔══██╗██╔══██╗██╔════╝██║████╗  ██║██╔════╝ ",
"██████╔╝███████║██████╔╝███████╗██║██╔██╗ ██║██║  ███╗",
"██╔═══╝ ██╔══██║██╔══██╗╚════██║██║██║╚██╗██║██║   ██║",
"██║     ██║  ██║██║  ██║███████║██║██║ ╚████║╚██████╔╝",
"╚═╝     ╚═╝  ╚═╝╚═╝  ╚═╝╚══════╝╚═╝╚═╝  ╚═══╝ ╚═════╝ ",
"													   ",
};

static const char *const TF_ART_VALGRIND[] = {
"														        ",
"██╗   ██╗ █████╗ ██╗      ██████╗ ██████╗ ██╗███╗   ██╗██████╗ ",
"██║   ██║██╔══██╗██║     ██╔════╝ ██╔══██╗██║████╗  ██║██╔══██╗",
"██║   ██║███████║██║     ██║  ███╗██████╔╝██║██╔██╗ ██║██║  ██║",
"╚██╗ ██╔╝██╔══██║██║     ██║   ██║██╔══██╗██║██║╚██╗██║██║  ██║",
" ╚████╔╝ ██║  ██║███████╗╚██████╔╝██║  ██║██║██║ ╚████║██████╔╝",
"  ╚═══╝  ╚═╝  ╚═╝╚══════╝ ╚═════╝ ╚═╝  ╚═╝╚═╝╚═╝  ╚═══╝╚═════╝ ",
"													 			",
};

/* ======================================================== utilidades === */
static inline void tf_sep(void)
{
	int i;
	printf("%s", TF_LILAC);
	for (i = 0; i < TF_SEP_W; i++) putchar('=');
	printf("%s\n", TF_RESET);
}

static inline void tf_dash(void)
{
	int i;
	printf("%s", TF_LILAC);
	for (i = 0; i < TF_SEP_W; i++) putchar('-');
	printf("%s\n", TF_RESET);
}

static inline void tf_muted(const char *s)
{
	printf("  %s\n", s);
}

static inline void tf_banner(const char *const art[], int nart, const char *title)
{
	int i;
	for (i = 0; i < nart; i++)
		printf("%s%s%s\n", TF_PINK, art[i], TF_RESET);
	printf("     %s%s%s\n", TF_STRONG, title, TF_RESET);
}

static inline const char *tf_signal_name(int sig)
{
	switch (sig) {
	case 2:  return "SIGINT";
	case 3:  return "SIGQUIT";
	case 4:  return "SIGILL";
	case 5:  return "SIGTRAP";
	case 6:  return "SIGABRT";
	case 7:  return "SIGBUS";
	case 8:  return "SIGFPE";
	case 9:  return "SIGKILL";
	case 11: return "SIGSEGV";
	case 13: return "SIGPIPE";
	case 15: return "SIGTERM";
	default: return NULL;
	}
}

static inline void tf_last_lines(const char *content, int maxn,
 char *out, size_t outsz)
{
	char *copy;
	char *save = NULL;
	char *line;
	char *lines_buf[128];
	int nlines = 0, start, i;
	size_t used = 0;

	out[0] = '\0';
	if (!content || !*content) {
		snprintf(out, outsz, "(sin log)");
		return;
	}
	copy = strdup(content);
	if (!copy) {
		snprintf(out, outsz, "(sin log)");
		return;
	}
	line = strtok_r(copy, "\n", &save);
	while (line && nlines < 128) {
		lines_buf[nlines++] = line;
		line = strtok_r(NULL, "\n", &save);
	}
	if (nlines == 0) {
		snprintf(out, outsz, "(sin log)");
		free(copy);
		return;
	}
	start = nlines > maxn ? nlines - maxn : 0;
	for (i = start; i < nlines; i++) {
		char *p = lines_buf[i];
		char *end;
		while (*p == ' ' || *p == '\t') p++;
		end = p + strlen(p);
		while (end > p && (end[-1] == ' ' || end[-1] == '\t'
 || end[-1] == '\r'))
			end--;
		if (p == end)
			continue;
		if (used > 0 && used + 2 < outsz) {
			out[used++] = ',';
			out[used++] = ' ';
		}
		{
			size_t len = (size_t)(end - p);
			if (used + len >= outsz)
				break;
			memcpy(out + used, p, len);
			used += len;
		}
	}
	out[used] = '\0';
	free(copy);
}

static inline void tf_expected(const char *fmt, ...)
{
	char tmp[512];
	char line[600];
	va_list ap;
	size_t used;

	va_start(ap, fmt);
	vsnprintf(tmp, sizeof tmp, fmt, ap);
	va_end(ap);

	snprintf(line, sizeof line, "  |-- EXPECTED : %s\n", tmp);
	used = strlen(TF_PENDING);
	if (used < sizeof(TF_PENDING) - 1)
		strncat(TF_PENDING, line, sizeof(TF_PENDING) - used - 1);
}

static inline void tf_got(const char *fmt, ...)
{
	char tmp[512];
	char line[600];
	va_list ap;
	size_t used;

	va_start(ap, fmt);
	vsnprintf(tmp, sizeof tmp, fmt, ap);
	va_end(ap);

	snprintf(line, sizeof line, "  |-- GOT      : %s\n", tmp);
	used = strlen(TF_PENDING);
	if (used < sizeof(TF_PENDING) - 1)
		strncat(TF_PENDING, line, sizeof(TF_PENDING) - used - 1);
}

static inline void tf_test_header(int id, const char *name, const char *cmd)
{
	TF_TESTID = id;
	snprintf(TF_TESTNAME, sizeof TF_TESTNAME, "%s", name);
	TF_PENDING[0] = '\0';
	if (cmd && *cmd) {
		snprintf(TF_TESTCMD, sizeof TF_TESTCMD, "%s", cmd);
	} else {
		TF_TESTCMD[0] = '\0';
	}
}

static inline int tf_vislen(const char *s)
{
	int n = 0;

	for (; *s; s++) {
		if (*s == '\x1b') {
			while (*s && *s != 'm') s++;
			if (*s) s++;
		} else {
			n++;
		}
	}
	return n;
}

static inline void tf_result(const char *state, const char *fmt, ...)
{
	const char *col;
	char wb[8];
	int no_detail = 0;

	(void)fmt;

	if (!strcmp(state, "PASS"))      { col = TF_GREEN;  strcpy(wb, " OK "); no_detail = 1; TF_P++; }
	else if (!strcmp(state, "FAIL")) { col = TF_RED;    strcpy(wb, "FAIL");                  TF_F++; }
	else if (!strcmp(state, "N/D"))  { col = TF_GOLD;   strcpy(wb, "N/D");                   TF_ND++; }
	else                            { col = TF_LILAC;  strcpy(wb, "SKIP");                   TF_SK++; }

	printf("  %s[%s]%s  TEST %02d:\n", col, wb, TF_RESET, TF_TESTID);
	printf("          ARGUMENT:    %s\n", TF_TESTCMD);
	printf("          DESCRIPTION: %s", TF_TESTNAME);
	if (TF_LAST_DUR > 0) {
		int tvis = tf_vislen(TF_TESTNAME);
		int sp = 72 - (23 + tvis);
		if (sp > 0)
			printf("%*s", sp, "");
		printf("(%.2fs)", TF_LAST_DUR);
	}
	printf("\n");

	if (TF_PENDING[0] && !no_detail)
		fputs(TF_PENDING, stdout);

	if (!strcmp(state, "FAIL") && TF_FAIL_N < 512)
		TF_FAIL_IDS[TF_FAIL_N++] = TF_TESTID;

	TF_PENDING[0] = '\0';
}

static inline void tf_sec_summary(void)
{
	int total = TF_P + TF_F + TF_ND + TF_SK;
	printf("  [SECTION SUMMARY]: %s%d%s/%d Passed\n\n", TF_GREEN, TF_P, TF_RESET, total);
}

static inline double tf_now(void)
{
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (double)ts.tv_sec + (double)ts.tv_nsec / 1e9;
}

/* ==================================================== directorio temporal === */
static char TF_TMPROOT[64];
static int TF_TMP_OK = 0;

static inline void tf_tmp_setup(void)
{
	char pat[] = "/tmp/codexion_test_XXXXXX";
	if (TF_TMP_OK)
		return;
	if (!mkdtemp(pat)) {
		perror("mkdtemp");
		exit(2);
	}
	snprintf(TF_TMPROOT, sizeof TF_TMPROOT, "%s", pat);
	TF_TMP_OK = 1;
}

static inline void tf_tmp_path(char *out, size_t n, const char *name)
{
	tf_tmp_setup();
	snprintf(out, n, "%s/%s", TF_TMPROOT, name);
}

/* ====================================================== lectura de archivos */
static inline char *tf_read(const char *path)
{
	FILE *f = fopen(path, "r");
	char *buf;
	long sz;
	size_t rd;

	if (!f)
		return strdup("");
	fseek(f, 0, SEEK_END);
	sz = ftell(f);
	fseek(f, 0, SEEK_SET);
	buf = malloc((size_t)sz + 1);
	if (!buf) { fclose(f); return strdup(""); }
	rd = fread(buf, 1, (size_t)sz, f);
	buf[rd] = '\0';
	fclose(f);
	return buf;
}

static inline int tf_count_lines(const char *s)
{
	int n = 0;
	for (; *s; s++)
		if (*s == '\n')
			n++;
	return n;
}

/* C9: si timeout corta la ultima linea, se descarta antes de validar */
static inline void tf_drop_partial(char *buf)
{
	size_t n = strlen(buf);
	char *nl;

	if (n == 0)
		return;
	if (buf[n - 1] == '\n')
		return;
	nl = strrchr(buf, '\n');
	if (nl)
		*nl = '\0';
	else
		buf[0] = '\0';
}

/* =================================================== ejecucion de comandos */
static inline int tf_exec(const tf_cmd *c, int timeout_s,
			  const char *out_path, const char *err_path)
{
	int fo = -1, fe = -1;
	pid_t pid;
	int st = 0;
	double t0 = 0;

	if (out_path)
		fo = open(out_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (err_path) {
		if (fo >= 0 && !strcmp(out_path, err_path))
			fe = fo;
		else
			fe = open(err_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	}

	t0 = tf_now();
	pid = fork();
	if (pid < 0) {
		if (fo >= 0) close(fo);
		return -1;
	}
	if (pid == 0) {
		int dn = open("/dev/null", O_RDONLY);
		if (dn >= 0) dup2(dn, 0);
		if (fo >= 0) dup2(fo, 1);
		if (fe >= 0) dup2(fe, 2);

		if (timeout_s > 0) {
			char secbuf[16];
			const char *tv[TF_MAXARGS + 2];
			int k = 0, j;
			snprintf(secbuf, sizeof secbuf, "%d", timeout_s);
			tv[k++] = "timeout";
			tv[k++] = secbuf;
			for (j = 0; j < c->argc; j++)
				tv[k++] = c->argv[j];
			tv[k] = NULL;
			execvp("timeout", (char *const *)tv);
			/* sin 'timeout' -> intento directo (sin limite) */
		}
		execvp(c->argv[0], (char *const *)c->argv);
		_exit(127);
	}

	if (waitpid(pid, &st, 0) < 0) {
		if (fo >= 0 && fo != fe) close(fo);
		if (fe >= 0) close(fe);
		return -1;
	}
	if (fo >= 0 && fo != fe) close(fo);
	if (fe >= 0) close(fe);

	TF_LAST_DUR = tf_now() - t0;
	if (WIFEXITED(st)) return WEXITSTATUS(st);
	if (WIFSIGNALED(st)) return 128 + WTERMSIG(st);
	return -1;
}

/* construye un cmd a partir de la lista plana de argumentos */
static inline void tf_cmd_from_args(tf_opts *o, const char *args, tf_cmd *c)
{
	char tmp[512];
	char *save = NULL;
	char *p;

	strncpy(tmp, args, sizeof(tmp) - 1);
	tmp[sizeof(tmp) - 1] = '\0';

	c->argc = 0;
	c->argv[c->argc++] = o->bin;
	p = strtok_r(tmp, " \t\n", &save);
	while (p && c->argc < TF_MAXARGS) {
		c->argv[c->argc++] = strdup(p);
		p = strtok_r(NULL, " \t\n", &save);
	}
	c->argv[c->argc] = NULL;
}

static inline int tf_run_args(tf_opts *o, const char *args, int timeout_s, const char *logpath)
{
	tf_cmd c;
	tf_cmd_from_args(o, args, &c);
	return tf_exec(&c, timeout_s, logpath, logpath);
}

/* nombre corto del binario para mostrar: "/codexion" (solo basename) */
static inline void tf_shortbin(const char *bin, char *out, size_t n)
{
	const char *b = strrchr(bin, '/');

	b = b ? b + 1 : bin;
	snprintf(out, n, "/%s", b);
}

/* arma la cadena de comando para mostrar (prefix = ej. "timeout 8") */
static inline void tf_cmd_display(const char *bin, const char *prefix,
				   const char *args, char *out, size_t n)
{
	char sb[128];

	tf_shortbin(bin, sb, sizeof sb);
	if (prefix && *prefix)
		snprintf(out, n, "%s %s %s", prefix, sb, args);
	else
		snprintf(out, n, "%s %s", sb, args);
}

/* ========================================================== validadores === */
static inline int tf_all_digits(const char *s)
{
	if (!s || !*s)
		return 0;
	for (; *s; s++)
		if (*s < '0' || *s > '9')
			return 0;
	return 1;
}

/* parsea una linea: <ts> <id> <msg> . Devuelve 0 si ok, -1 si formato malo. */
static inline int tf_parse_line(char *ln, long long *ts, int *id, char **msg)
{
	char *p = ln;
	char *t1, *t2;

	while (*p == ' ' || *p == '\t') p++;
	t1 = p;
	while (*p && *p != ' ' && *p != '\t') p++;
	if (!*p) return -1;
	*p = '\0'; p++;
	while (*p == ' ' || *p == '\t') p++;
	t2 = p;
	while (*p && *p != ' ' && *p != '\t') p++;
	if (!*p) return -1;
	*p = '\0'; p++;
	while (*p == ' ' || *p == '\t') p++;

	if (!tf_all_digits(t1) || !tf_all_digits(t2))
		return -1;
	*ts = strtoll(t1, NULL, 10);
	*id = (int)strtoll(t2, NULL, 10);
	*msg = p;
	return 0;
}

static inline int tf_valid_message(const char *m)
{
	return !strcmp(m, "has taken a dongle") || !strcmp(m, "is compiling")
	    || !strcmp(m, "is debugging") || !strcmp(m, "is refactoring");
}

/* C1-C5: formato estricto, mensajes validos, 2 tomas antes de compilar,
 * timestamps monotonicos, todos los coders 1..N completan >= req compiles. */
static inline int tf_check_viable(const char *content, int n, int req)
{
	char *copy = strdup(content);
	int *compiles = calloc((size_t)n + 2, sizeof(int));
	int *takes = calloc((size_t)n + 2, sizeof(int));
	long long *cts = malloc(((size_t)n + 2) * sizeof(long long));
	char *save = NULL;
	char *line;
	int fail = 0;
	long long gts = 0;
	int c;

	for (c = 0; c < n + 2; c++) cts[c] = -1;

	line = strtok_r(copy, "\n", &save);
	while (line) {
		long long ts;
		int id;
		char *msg = NULL;
		if (tf_parse_line(line, &ts, &id, &msg) == -1) {
			fail = 1;
		} else {
			if (ts < gts) fail = 1;
			else gts = ts;
			if (id >= 1 && id <= n) {
				if (cts[id] >= 0 && ts < cts[id]) fail = 1;
				cts[id] = ts;
				if (!strcmp(msg, "has taken a dongle"))
					takes[id]++;
				else if (!strcmp(msg, "is compiling")) {
					if (takes[id] != 2) fail = 1;
					compiles[id]++;
					takes[id] = 0;
				} else if (!strcmp(msg, "burned out"))
					fail = 1;
				else if (!tf_valid_message(msg))
					fail = 1;
			}
		}
		line = strtok_r(NULL, "\n", &save);
	}

	if (!fail) {
		for (c = 1; c <= n; c++)
			if (cts[c] < 0 || compiles[c] < req) { fail = 1; break; }
	}

	free(copy);
	free(compiles);
	free(takes);
	free(cts);
	return fail;
}

/* C6: 'burned out' exactamente UNA vez y como ULTIMA linea. */
static inline int tf_check_burnout(const char *content)
{
	char *copy = strdup(content);
	char *save = NULL;
	char *line = strtok_r(copy, "\n", &save);
	int fail = 0, burns = 0, last = 0;

	while (line) {
		long long ts;
		int id;
		char *msg = NULL;
		if (tf_parse_line(line, &ts, &id, &msg) == -1) {
			fail = 1;
		} else {
			if (!strcmp(msg, "burned out")) {
				burns++;
				last = 1;
			} else {
				if (last) fail = 1;
				last = 0;
			}
		}
		line = strtok_r(NULL, "\n", &save);
	}
	if (burns != 1) fail = 1;
	free(copy);
	return fail;
}

/* Delay del monitor: burn_ts - ultimo "is compiling" del coder - burnout. */
static inline long long tf_monitor_delay(const char *content, long long burnout)
{
	enum { MAXID = 4096 };
	long long lastc[MAXID];
	char *copy = strdup(content);
	char *save = NULL;
	char *line;
	long long burn_ts = -1;
	int burn_id = -1;
	int i;

	for (i = 0; i < MAXID; i++) lastc[i] = -1;

	line = strtok_r(copy, "\n", &save);
	while (line) {
		long long ts;
		int id;
		char *msg = NULL;
		if (tf_parse_line(line, &ts, &id, &msg) == -1) {
			line = strtok_r(NULL, "\n", &save);
			continue;
		}
		if (!strcmp(msg, "is compiling") && id >= 1 && id < MAXID)
			lastc[id] = ts;
		if (!strcmp(msg, "burned out")) {
			burn_ts = ts;
			burn_id = id;
		}
		line = strtok_r(NULL, "\n", &save);
	}
	free(copy);

	if (burn_id < 1 || burn_id >= MAXID || lastc[burn_id] < 0)
		return -1;
	return burn_ts - lastc[burn_id] - burnout;
}

/* C6 temporizado: ningun coder puede emitir un evento propio despues de su
 * deadline teorico (inicio de su ultima compilacion + time_to_burnout, o el
 * arranque de la simulacion si aun no compilo) ANTES del 'burned out'.
 * Detecta el caso reportado: un coder que recompila en el ms ~120 cuando su
 * deadline era 110 queda enmascarado aunque el log termine bien. */
static inline int tf_check_deadline(const char *content, long long burnout)
{
	enum { MAXID = 4096 };
	long long lastc[MAXID];
	char *copy;
	char *save = NULL;
	char *line;
	int i;
	int fail = 0;

	for (i = 0; i < MAXID; i++)
		lastc[i] = 0;
	copy = strdup(content);
	line = strtok_r(copy, "\n", &save);
	while (line) {
		long long ts;
		int id;
		char *msg = NULL;
		long long dl;

		if (tf_parse_line(line, &ts, &id, &msg) == -1) {
			fail = 1;
			line = strtok_r(NULL, "\n", &save);
			continue;
		}
		if (!strcmp(msg, "burned out"))
			break;
		if (id >= 1 && id < MAXID) {
			dl = lastc[id] + burnout;
			if (ts > dl)
				fail = 1;
			if (!strcmp(msg, "is compiling"))
				lastc[id] = ts;
		}
		line = strtok_r(NULL, "\n", &save);
	}
	free(copy);
	return fail;
}

/* ========================================================= run_viable === */
static inline void tf_run_viable(tf_opts *o, int id, const char *name,
				 const char *args, const char *expected,
				 int n, int req, int timeout_s)
{
	char log[PATH_MAX], fname[32], disp[512], got[2048];
	char last[1024];
	double t0, d;
	int rc, lines;
	char *content;

	snprintf(fname, sizeof fname, "%02d.log", id);
	tf_tmp_path(log, sizeof log, fname);
	tf_cmd_display(o->bin, "", args, disp, sizeof disp);

	t0 = tf_now();
	rc = tf_run_args(o, args, timeout_s, log);
	d = tf_now() - t0;

	content = tf_read(log);
	tf_drop_partial(content);
	lines = tf_count_lines(content);
	tf_last_lines(content, 2, last, sizeof last);

	tf_test_header(id, name, disp);
	tf_expected("%s", expected);

	if (rc == 124) {
		snprintf(got, sizeof got,
		    "exit=124 (timeout) tras %.2fs — "
		    TF_GOLD "%d" TF_RESET
		    " lineas logueadas; ultima: \"%s\"",
		    d, lines, last);
	} else if (rc != 0) {
		int sig = rc - 128;
		if (rc >= 128) {
			const char *sn = tf_signal_name(sig);
			if (sn)
				snprintf(got, sizeof got,
				    "exit=" TF_GOLD "%d" TF_RESET
				    " (%s, señal "
				    TF_GOLD "%d" TF_RESET
				    ") tras %.2fs — "
				    TF_GOLD "%d" TF_RESET
				    " lineas logueadas; ultima: \"%s\"",
				    rc, sn, sig, d, lines, last);
			else
				snprintf(got, sizeof got,
				    "exit=" TF_GOLD "%d" TF_RESET
				    " (SEÑAL "
				    TF_GOLD "%d" TF_RESET
				    ") tras %.2fs — "
				    TF_GOLD "%d" TF_RESET
				    " lineas logueadas; ultima: \"%s\"",
				    rc, sig, d, lines, last);
		} else {
			snprintf(got, sizeof got,
			    "exit=" TF_GOLD "%d" TF_RESET
			    " (exit() normal) tras %.2fs — "
			    TF_GOLD "%d" TF_RESET
			    " lineas logueadas; ultima: \"%s\"",
			    rc, d, lines, last);
		}
	} else if (tf_check_viable(content, n, req)) {
		snprintf(got, sizeof got, "exit=0 pero log invalido (C1-C5)");
	} else {
		snprintf(got, sizeof got, "exit=0, %d lineas, log valido (C1-C5)", lines);
	}
	tf_got("%s", got);

	if (rc == 124)
		tf_result("N/D", "(timeout %ds, Time: %.2fs)", timeout_s, d);
	else if (rc != 0) {
		int sig = rc - 128;
		if (rc >= 128) {
			const char *sn = tf_signal_name(sig);
			if (sn)
				tf_result("FAIL",
				    "(" TF_GOLD "exit=%d" TF_RESET
				    " %s, señal %d, "
				    TF_GOLD "%d lineas" TF_RESET
				    " | Time: %.2fs)",
				    rc, sn, sig, lines, d);
			else
				tf_result("FAIL",
				    "(" TF_GOLD "exit=%d" TF_RESET
				    " SEÑAL %d, "
				    TF_GOLD "%d lineas" TF_RESET
				    " | Time: %.2fs)",
				    rc, sig, lines, d);
		} else {
			tf_result("FAIL",
			    "(" TF_GOLD "exit=%d" TF_RESET
			    " exit() normal, "
			    TF_GOLD "%d lineas" TF_RESET
			    " | Time: %.2fs)",
			    rc, lines, d);
		}
	} else if (tf_check_viable(content, n, req))
		tf_result("FAIL", "(log invalido C1-C5, exit 0 | Time: %.2fs)", d);
	else
		tf_result("PASS", "(Time: %.2fs)", d);
	printf("\n");
	free(content);
}

/* =========================================================== run_fail === */
static inline void tf_run_fail(tf_opts *o, int id, const char *name,
			       const char *expected, const tf_cmd *variants, int nv)
{
	char disp[1024] = "";
	char bad[1024] = "";
	int v, k;

	for (v = 0; v < nv; v++) {
		char part[512] = "";
		for (k = 0; k < variants[v].argc; k++) {
			char tok[256];
			size_t used = strlen(part);
			if (k) strncat(part, " ", sizeof(part) - used - 1);
			if (k == 0)
				tf_cmd_display(variants[v].argv[0], "", "",
					       tok, sizeof tok);
			else
				strncpy(tok, variants[v].argv[k], sizeof tok - 1);
			tok[sizeof tok - 1] = '\0';
			strncat(part, tok, sizeof(part) - strlen(part) - 1);
		}
		if (v) strncat(disp, "  |  ", sizeof(disp) - strlen(disp) - 1);
		strncat(disp, part, sizeof(disp) - strlen(disp) - 1);
	}

	(void)o;

	tf_test_header(id, name, disp);
	tf_expected("%s", expected);

	for (v = 0; v < nv; v++) {
		char o_path[PATH_MAX], e_path[PATH_MAX], fname[64];
		int rc;
		char *err;
		const char *reason = NULL;
		char argvstr[256] = "bin";

		snprintf(fname, sizeof fname, "f_%d_%d", id, v);
		tf_tmp_path(o_path, sizeof o_path, fname);
		snprintf(fname, sizeof fname, "fe_%d_%d", id, v);
		tf_tmp_path(e_path, sizeof e_path, fname);

		rc = tf_exec(&variants[v], 15, o_path, e_path);

		if (rc == 0) {
			reason = "entrada aceptada (exit 0)";
		} else {
			err = tf_read(e_path);
			if (rc == 124)
				reason = "timeout (posible cuelgue del binario)";
			else if (!err[0])
				reason = "sin mensaje a stderr";
			free(err);
		}

		if (reason) {
			for (k = 1; k < variants[v].argc; k++) {
				size_t used = strlen(argvstr);
				strncat(argvstr, " ", sizeof(argvstr) - used - 1);
				strncat(argvstr, variants[v].argv[k],
					sizeof(argvstr) - strlen(argvstr) - 1);
			}
			if (!bad[0])
				snprintf(bad, sizeof bad, "variante %d '%s': %s",
					 v + 1, argvstr, reason);
			else {
				char entry[512];
				snprintf(entry, sizeof entry, "; variante %d '%s': %s",
					 v + 1, argvstr, reason);
				strncat(bad, entry, sizeof(bad) - strlen(bad) - 1);
			}
		}
	}

	if (bad[0])
		tf_got("%s", bad);
	else
		tf_got("exit != 0 y error a stderr en todas las variantes");

	if (!bad[0])
		tf_result("PASS", "(exit != 0 y error a stderr en las %d variantes)", nv);
	else
		tf_result("FAIL", "(%s)", bad);
	printf("\n");
}

/* ======================================================== run_valgrind === */
static inline int tf_have_valgrind(void)
{
	char *path = getenv("PATH");
	char *copy, *save = NULL, *dir;

	if (!path)
		return 0;
	copy = strdup(path);
	dir = strtok_r(copy, ":", &save);
	while (dir) {
		char full[1024];
		snprintf(full, sizeof full, "%s/valgrind", dir);
		if (access(full, X_OK) == 0) { free(copy); return 1; }
		dir = strtok_r(NULL, ":", &save);
	}
	free(copy);
	return 0;
}

static inline int tf_definitely_lost(const char *s)
{
	const char *p = s;
	while ((p = strstr(p, "definitely lost:")) != NULL) {
		const char *q = p + strlen("definitely lost:");
		while (*q == ' ') q++;
		if (*q >= '1' && *q <= '9')
			return 1;
		p = q;
	}
	return 0;
}

static inline void tf_grep_detail(const char *content, char *out, size_t n)
{
	char *copy = strdup(content);
	char *save = NULL;
	char *line = strtok_r(copy, "\n", &save);
	size_t used = 0;
	int got = 0;

	out[0] = '\0';
	while (line && got < 2) {
		if (strstr(line, "definitely lost") || strstr(line, "ERROR SUMMARY")) {
			size_t l = strlen(line);
			if (got && used + 1 < n) out[used++] = ' ';
			if (used + l + 1 < n) {
				memcpy(out + used, line, l);
				used += l;
				out[used] = '\0';
			}
			got++;
		}
		line = strtok_r(NULL, "\n", &save);
	}
	free(copy);
}

static inline void tf_run_valgrind(tf_opts *o, int id, const char *name,
				   const char *args, const char *expected)
{
	char log[PATH_MAX], fname[32], disp[512], got[1024];
	tf_cmd c;
	char *content;
	int rc;
	char *save = NULL;
	char *p;

	snprintf(fname, sizeof fname, "%02d.vg", id);
	tf_tmp_path(log, sizeof log, fname);
	tf_cmd_display(o->bin, "valgrind --leak-check=full -s", args, disp, sizeof disp);

	tf_test_header(id, name, disp);
	tf_expected("%s", expected);

	if (!tf_have_valgrind()) {
		tf_got("valgrind no disponible en PATH");
		tf_result("SKIP", "(valgrind no instalado)");
		printf("\n");
		return;
	}

	c.argc = 0;
	c.argv[c.argc++] = "valgrind";
	c.argv[c.argc++] = "--leak-check=full";
	c.argv[c.argc++] = "-s";
	c.argv[c.argc++] = o->bin;
	{
		char tmp[512];
		strncpy(tmp, args, sizeof(tmp) - 1);
		tmp[sizeof(tmp) - 1] = '\0';
		p = strtok_r(tmp, " \t\n", &save);
		while (p && c.argc < TF_MAXARGS) {
			c.argv[c.argc++] = strdup(p);
			p = strtok_r(NULL, " \t\n", &save);
		}
	}
	c.argv[c.argc] = NULL;

	rc = tf_exec(&c, 0, log, log);
	content = tf_read(log);

	if (strstr(content, "ERROR SUMMARY: 0 errors") && !tf_definitely_lost(content)) {
		snprintf(got, sizeof got, "exit=%d, valgrind sin errores ni fugas", rc);
		tf_got("%s", got);
		tf_result("PASS", "(heap ok, 0 errores; exit=%d)", rc);
	} else {
		char detail[512] = "";
		tf_grep_detail(content, detail, sizeof detail);
		snprintf(got, sizeof got, "exit=%d, valgrind detecta problemas (ver RESULT)", rc);
		tf_got("%s", got);
		if (!detail[0])
			snprintf(detail, sizeof detail, "salida vacia (rc=%d)", rc);
		tf_result("FAIL", "%s", detail);
	}
	printf("\n");
	free(content);
}

/* ============================================================= resumen === */
static inline void tf_totals(tf_opts *o)
{
	if (o->totals_file) {
		FILE *f = fopen(o->totals_file, "w");
		if (f) {
			fprintf(f, "PASS=%d FAIL=%d ND=%d SKIP=%d\n",
				TF_P, TF_F, TF_ND, TF_SK);
			if (TF_FAIL_N > 0) {
				int i;
				fprintf(f, "FAILED_IDS=");
				for (i = 0; i < TF_FAIL_N; i++) {
					if (i > 0) fprintf(f, ",");
					fprintf(f, "%02d", TF_FAIL_IDS[i]);
				}
				fprintf(f, "\n");
			}
			fclose(f);
		}
	}
}

/* ========================================================== limpieza === */
static inline void tf_rmrf(const char *path)
{
	DIR *d = opendir(path);
	struct dirent *e;

	if (!d)
		return;
	while ((e = readdir(d)) != NULL) {
		struct stat st;
		char child[PATH_MAX * 2];
		int need;
		if (!strcmp(e->d_name, ".") || !strcmp(e->d_name, ".."))
			continue;
		need = snprintf(child, sizeof child, "%s/%s", path, e->d_name);
		if (need < 0 || (size_t)need >= sizeof child)
			continue;
		if (lstat(child, &st) == 0 && S_ISDIR(st.st_mode))
			tf_rmrf(child);
		else
			remove(child);
	}
	closedir(d);
	rmdir(path);
}

static void tf_cleanup_atexit(void)
{
	if (TF_TMP_OK)
		tf_rmrf(TF_TMPROOT);
}

/* ==================================================== arranque de modulos === */
static inline void tf_init(tf_opts *o, int argc, char **argv)
{
	int i;

	o->smoke = 0;
	o->bin = getenv("CODEXION_BIN");
	if (!o->bin)
		o->bin = "../coders/codexion";
	o->totals_file = NULL;

	for (i = 1; i < argc; i++) {
		if (!strcmp(argv[i], "--smoke"))
			o->smoke = 1;
		else if (!strcmp(argv[i], "--bin") && i + 1 < argc)
			o->bin = argv[++i];
		else if (!strcmp(argv[i], "--totals-file") && i + 1 < argc)
			o->totals_file = argv[++i];
	}

	tf_tmp_setup();
	atexit(tf_cleanup_atexit);

	if (access(o->bin, X_OK) != 0) {
		fprintf(stderr, "[ERROR] binario no accesible: %s\n", o->bin);
		exit(1);
	}
}

#endif /* TEST_FRAMEWORK_H */
