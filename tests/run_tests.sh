#!/usr/bin/env bash
# =============================================================================
#  run_tests.sh — ORQUESTADOR DE LA SUITE MODULAR DE TESTS PARA CODEXION
# -----------------------------------------------------------------------------
#  Uso:  bash tests/run_tests.sh [--full | --smoke] [--bin <ruta>] [--cc cc]
#
#  Compila los modulos de prueba (test_*.c), los ejecuta en secuencia y
#  consolida los resultados en el reporte final. NO modifica coders/.
# =============================================================================

set -u

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR" || exit 1

# ---------------------------------------------------------------- colores ---
# Fuente unica del tema: tests/theme.sh (tambien deriva tests/theme.h para C).
source "$SCRIPT_DIR/theme.sh"

# ---------------------------------------------------------------- defaults ---
MODE="full"
MODE_FLAG=""
BIN="$SCRIPT_DIR/../coders/codexion"
CC="${CC:-cc}"
CFLAGS="${CFLAGS:--O1 -Wall -Wextra -Werror}"
MODULES=(test_small_coders test_large_coders test_burnout test_parsing test_valgrind)

usage()
{
	cat <<EOF
Uso: $0 [--full | --smoke] [--bin <ruta>] [--cc <compilador>] [--help]

  --full, (default)  suite completa (incluye gran escala 08-11 y valgrind 50)
  --smoke, -s        variante liviana (omite tests 08-11 y valgrind test 37)
  --bin <ruta>       ruta al binario codexion (default: coders/codexion)
  --cc <compilador>  compilador C para los modulos (default: \$CC o cc)
  --help, -h         muestra esta ayuda

No compila ni modifica nada dentro de coders/: solo se prueba el binario
tal como esta. Los tests que fallen se reportan en el resumen final.
EOF
	exit 0
}

while [ $# -gt 0 ]; do
	case "$1" in
		--full)     MODE="full"; MODE_FLAG="" ;;
		--smoke|-s) MODE="smoke"; MODE_FLAG="--smoke" ;;
		--bin)      BIN="$2"; shift ;;
		--cc)       CC="$2"; shift ;;
		--help|-h)  usage ;;
		*) echo -e "${TF_RED}[ERROR]${TF_RESET} argumento desconocido: $1" >&2; usage >&2; exit 1 ;;
	esac
	shift
done

if [ ! -x "$BIN" ]; then
	echo -e "${TF_RED}[ERROR]${TF_RESET} binario no encontrado o no ejecutable: $BIN" >&2
	echo "  (no se compila ni se modifica coders/; usa --bin para indicar otro)." >&2
	exit 1
fi

TMP_DIR="$(mktemp -d "$SCRIPT_DIR/.suite_XXXXXX")"
trap 'rm -rf "$TMP_DIR"' EXIT

# ============================================================== ASCII art ====
art_lines()
{
    case "$1" in

    CODEXION) cat <<'EOF'
 ██████╗ ██████╗ ██████╗ ███████╗██╗  ██╗██╗ ██████╗ ███╗   ██╗
██╔════╝██╔═══██╗██╔══██╗██╔════╝╚██╗██╔╝██║██╔═══██╗████╗  ██║
██║     ██║   ██║██║  ██║█████╗   ╚███╔╝ ██║██║   ██║██╔██╗ ██║
██║     ██║   ██║██║  ██║██╔══╝   ██╔██╗ ██║██║   ██║██║╚██╗██║
╚██████╗╚██████╔╝██████╔╝███████╗██╔╝ ██╗██║╚██████╔╝██║ ╚████║
 ╚═════╝ ╚═════╝ ╚═════╝ ╚══════╝╚═╝  ╚═╝╚═╝ ╚═════╝ ╚═╝  ╚═══╝
EOF
        ;;

    RESUMEN) cat <<'EOF'
██████╗ ███████╗███████╗██╗   ██╗███╗   ███╗███████╗███╗   ██╗
██╔══██╗██╔════╝██╔════╝██║   ██║████╗ ████║██╔════╝████╗  ██║
██████╔╝█████╗  ███████╗██║   ██║██╔████╔██║█████╗  ██╔██╗ ██║
██╔══██╗██╔══╝  ╚════██║██║   ██║██║╚██╔╝██║██╔══╝  ██║╚██╗██║
██║  ██║███████╗███████║╚██████╔╝██║ ╚═╝ ██║███████╗██║ ╚████║
╚═╝  ╚═╝╚══════╝╚══════╝ ╚═════╝ ╚═╝     ╚═╝╚══════╝╚═╝  ╚═══╝
EOF
        ;;

    esac
}

banner()
{
	local art ln
	art=$(art_lines "$1")
	while IFS= read -r ln; do
		[ -n "$ln" ] && echo -e "${TF_PINK}${ln}${TF_RESET}"
	done <<<"$art"
	echo -e "     ${TF_STRONG}${2}${TF_RESET}"
}

sep()   { echo -e "${TF_LILAC}$(printf '%*s' "$TF_SEP_W" '' | tr ' ' '=')${TF_RESET}"; }
dash()  { echo -e "${TF_LILAC}$(printf '%*s' "$TF_SEP_W" '' | tr ' ' '-')${TF_RESET}"; }
muted() { echo "  $*"; }

# ============================================================ PROGRESS BAR ===
# TTY guard: the animated progress bar only renders when stdout is a terminal.
# In non-interactive mode (pipes, redirection, CI), modules execute directly
# in the foreground with no buffering — identical to the original behavior.
# ---------------------------------------------------------------------------

N_MOD=${#MODULES[@]}

draw_bar()
{
	local completed=$1 total=$2 label="$3" elapsed=${4:-0} tests_so_far=${5:-0}
	local width=20

	local expected=$((elapsed * 3))
	[ "$expected" -lt 15 ] && expected=15
	[ "$expected" -gt 25 ] && expected=25
	local fraction
	fraction=$(awk -v e="$elapsed" -v x="$expected" 'BEGIN{ f=e/x; if(f>0.95) f=0.95; printf "%.2f", f }')

	local prog
	prog=$(awk -v c="$completed" -v t="$total" -v f="$fraction" 'BEGIN{ printf "%.2f", (c+f)/t }')

	local filled
	filled=$(awk -v p="$prog" -v w="$width" 'BEGIN{ printf "%d", p*w }')
	local empty=$((width - filled))

	local fill_part="" empty_part=""
	[ "$filled" -gt 0 ] && fill_part=$(printf '%*s' "$filled" '' | tr ' ' '=')
	[ "$empty" -gt 0 ] && empty_part=$(printf '%*s' "$empty" '' | tr ' ' '-')
	if [ "$filled" -gt 0 ] && [ "$empty" -gt 0 ]; then
		fill_part="${fill_part%?}>"
	fi
	local bar="${fill_part}${empty_part}"

	printf "\r\033[2K${TF_GOLD}[%s]${TF_RESET} ${TF_LILAC}modulo %d/%d: %s${TF_RESET} ${TF_GOLD}T+%ds${TF_RESET} ${TF_LILAC}(%d tests)${TF_RESET}  " \
		"$bar" $((completed+1)) "$total" "$label" "$elapsed" "$tests_so_far"
}

# Execute a single module with optional animated progress bar (TTY only).
# Returns the module's exit code. Caller reads totals file and accumulates
# global counters (GP/GF/GND/GSKIP) after each call.
run_module_with_bar()
{
	local m="$1" completed=$2 total=$3 tests_so_far=${4:-0}

	if [ ! -t 1 ]; then
		"$TMP_DIR/$m" $MODE_FLAG --bin "$BIN" --totals-file "$TMP_DIR/$m.totals"
		return $?
	fi

	"$TMP_DIR/$m" $MODE_FLAG --bin "$BIN" --totals-file "$TMP_DIR/$m.totals" \
		> "$TMP_DIR/$m.out" 2>&1 &
	local pid=$!

	local start_time=$SECONDS
	while kill -0 "$pid" 2>/dev/null; do
		local elapsed=$((SECONDS - start_time))
		draw_bar "$completed" "$total" "$m" "$elapsed" "$tests_so_far"
		sleep 0.25
	done

	wait "$pid"
	local rc=$?

	printf "\r\033[2K"
	cat "$TMP_DIR/$m.out"

	return $rc
}

# ===================================================== generador de theme.h ===
# Deriva tests/theme.h (incluido por test_framework.h) desde la unica fuente
# theme.sh, para que bash y C compartan colores y layout sin duplicarlos.
gen_theme()
{
	{
		echo "/* GENERADO AUTOMATICAMENTE por run_tests.sh desde theme.sh — NO EDITAR. */"
		echo "/* Fuente unica del tema: tests/theme.sh (colores y layout van ahi). */"
		echo "#ifndef TEST_THEME_H"
		echo "#define TEST_THEME_H"
		echo ""
		sed -n -e "s/^\(TF_[A-Z_]*\)='\\\\033\[\([0-9;]*\)m'\$/#define \1 \"\\\\x1b[\2m\"/p" \
		       -e "s/^TF_SEP_W=\([0-9]*\)\$/#define TF_SEP_W \1/p" "$SCRIPT_DIR/theme.sh"
		echo ""
		echo "#endif /* TEST_THEME_H */"
	} > "$SCRIPT_DIR/theme.h"
}

# ================================================================ COMPILAR ===
gen_theme
echo -e "${TF_GREEN}[+]${TF_RESET} COMPILANDO MODULOS DE PRUEBA (${CC} ${CFLAGS})"
for m in "${MODULES[@]}"; do
	if ! "$CC" $CFLAGS -o "$TMP_DIR/$m" "$m.c"; then
		echo -e "${TF_RED}[ERROR]${TF_RESET} fallo al compilar $m.c" >&2
		exit 1
	fi
	echo -e "${TF_GREEN}[ OK ]${TF_RESET} $m.c -> $TMP_DIR/$m"
done

# ================================================================= CABECERA ===
SECONDS=0
sep
banner CODEXION "SUITE GENERAL DE PRUEBAS PARA TODO EL PROYECTO"
sep
echo -e "${TF_GREEN}[+]${TF_RESET} DATE    : $(date +%Y-%m-%d)"
echo -e "${TF_GREEN}[+]${TF_RESET} TARGET  : $BIN  (NO se modifica coders/: sin rebuild)"
echo -e "${TF_GREEN}[+]${TF_RESET} COMPILER: $CC $CFLAGS"
echo -e "${TF_GREEN}[+]${TF_RESET} Sujeto  : codexion_subject.md v1.0 (fuente oficial)"
echo -e "${TF_GREEN}[+]${TF_RESET} MODE    : $MODE"
echo -e "${TF_GREEN}[+]${TF_RESET} Coverage: pequenos / grandes / burnout / parsing / valgrind"
echo ""
echo -e "${TF_PINK}>>> CHEQUEOS TRANSVERSALES (aplican a toda corrida viable)${TF_RESET}"
muted "[C1] formato estricto: <timestamp> <coder_id> <mensaje>"
muted "[C2] mensajes validos: has taken a dongle | is compiling | is debugging | is refactoring | burned out"
muted "[C3] 2 tomas de dongle antes de cada compilacion (dos dongles simultaneos)"
muted "[C4] timestamps monotonicos (global y por coder)"
muted "[C5] todos los coders 1..N aparecen y completan sus compilaciones"
muted "[C6] 'burned out' exactamente UNA vez y como ULTIMA linea"
muted "[C7] exit code: 0 si completo / !=0 si burnout o error de parseo"
muted "[C8] timeout(124)=No Definido; 139/134 o log vacio=FAIL"
muted "[C9] si timeout corta la ultima linea, se descarta antes de validar"
echo ""

# ================================================================ EJECUTAR ===
GP=0; GF=0; GND=0; GSKIP=0; GFAILED=""
tests_so_far=0
for i in "${!MODULES[@]}"; do
	m="${MODULES[$i]}"
	run_module_with_bar "$m" "$i" "$N_MOD" "$tests_so_far"
	rc=$?
	if [ "$rc" -ne 0 ]; then
		echo -e "${TF_RED}[WARN]${TF_RESET} modulo $m salio con rc=$rc"
	fi
	if [ -f "$TMP_DIR/$m.totals" ]; then
		read -r p f nd sk < <(sed -n 's/^PASS=\([0-9]*\) FAIL=\([0-9]*\) ND=\([0-9]*\) SKIP=\([0-9]*\)$/\1 \2 \3 \4/p' "$TMP_DIR/$m.totals")
		GP=$((GP + ${p:-0}))
		GF=$((GF + ${f:-0}))
		GND=$((GND + ${nd:-0}))
		GSKIP=$((GSKIP + ${sk:-0}))
	else
		echo -e "${TF_RED}[WARN]${TF_RESET} sin archivo de totales para $m"
	fi
	_fids=$(sed -n 's/^FAILED_IDS=\(.*\)$/\1/p' "$TMP_DIR/$m.totals" 2>/dev/null | tr -d ' ')
	if [ -n "$_fids" ]; then
		[ -n "${GFAILED:-}" ] && GFAILED="${GFAILED},${_fids}" || GFAILED="${_fids}"
	fi
	tests_so_far=$((GP + GF + GND + GSKIP))
	if [ -t 1 ]; then
		echo -e "${TF_GOLD}[PROGRESO]${TF_RESET} modulo $((i+1))/${N_MOD} completado"
	fi
done

# =============================================================== RESULTADO ===
sep
banner RESUMEN "FINAL TEST RESULTS"
sep
echo -e "  TOTAL TESTS RAN      : ${TF_GOLD}[ $((GP+GF+GND+GSKIP)) ]${TF_RESET}"
echo -e "  PASSED               : ${TF_GREEN}[ ${GP} ]${TF_RESET}"
echo -e "  FAILED               : ${TF_RED}[ ${GF} ]${TF_RESET}"
echo -e "  NO DEFINIDO (timeout): ${TF_GOLD}[ ${GND} ]${TF_RESET}"
echo -e "  SKIPPED (sin valgrind): ${TF_LILAC}[ ${GSKIP} ]${TF_RESET}"
if [ $((GP+GF)) -gt 0 ]; then
	rate=$(awk -v p="$GP" -v f="$GF" 'BEGIN{ printf "%.1f%%", p*100/(p+f) }')
else
	rate="-"
fi
echo -e "  SUCCESS RATE         : ${TF_GOLD}[ ${rate} ]${TF_RESET}"
sep
if [ -n "${GFAILED:-}" ]; then
	_fdisplay=$(echo "$GFAILED" | tr ',' '\n' | paste -sd ',' - | sed 's/,/, /g')
	echo -e "  ${TF_RED}FALLARON: ${_fdisplay}  (ver detalle arriba)${TF_RESET}"
else
	echo -e "  ${TF_GREEN}FALLARON: ninguno${TF_RESET}"
fi
echo ""
echo -e "${TF_PINK}  Corrida terminada en ${SECONDS}s. Los que fallan marcan que arreglar en coders/.${TF_RESET}"
echo ""
exit 0