#!/bin/bash
# test_log_format.sh — valida el formato de log del binario codexion:
#   <timestamp> <coder_id> <mensaje>
# Comprueba el formato, la monotonia de las marcas de tiempo y que cada
# compilacion venga precedida de exactamente 2 tomas de dongle.

BIN="../coders/codexion"
LOG="log_format_tmp.txt"
N_CODERS=5
TIMEOUT=3

cd "$(dirname "$0")" || exit 1

echo "Compilando..."
make -C ../coders > /dev/null 2>&1
if [ ! -x "$BIN" ]; then
    echo "FALLO: no se pudo compilar el binario $BIN"
    exit 1
fi

echo "Ejecutando..."
timeout $TIMEOUT "$BIN" $N_CODERS 800 200 200 100 3 50 fifo > "$LOG" 2>&1

# Si el timeout corta la escritura a mitad de linea, descartamos esa
# ultima linea incompleta (sin salto de linea final) para no dar falsos fallos.
if [ -s "$LOG" ] && [ -n "$(tail -c 1 "$LOG")" ]; then
    head -n -1 "$LOG" > "$LOG.tmp"
    mv "$LOG.tmp" "$LOG"
fi

if [ ! -s "$LOG" ]; then
    echo "  FAIL: salida vacia"
    rm -f "$LOG"
    echo "TEST FAILED"
    exit 1
fi

awk -v n="$N_CODERS" '
BEGIN {
    fail = 0
    print "=== TESTS: formato de log ==="
    print "--- formato: cada linea debe ser <timestamp> <coder_id> <mensaje> ---"
    print "--- timestamps monotonicos (global y por coder) ---"
    print "--- proporcion: 2 tomas de dongle por compilacion ---"
}
{
    if (NF < 3) {
        print "  FAIL: linea " NR " sin formato <ts> <id> <msg>: [" $0 "]"
        fail = 1
        next
    }
    if ($1 !~ /^[0-9]+$/ || $2 !~ /^[0-9]+$/) {
        print "  FAIL: linea " NR " con timestamp o id no numerico: [" $0 "]"
        fail = 1
        next
    }
    msg = $0
    sub(/^[0-9]+ [0-9]+ /, "", msg)
    if (msg != "has taken a dongle" && msg != "is compiling" \
        && msg != "has released the dongles" && msg != "is debugging" \
        && msg != "is refactoring") {
        print "  FAIL: mensaje no reconocido en linea " NR ": [" msg "]"
        fail = 1
        next
    }
    if ($1 < prev) {
        print "  FAIL: timestamp global no monotonico en linea " NR
        fail = 1
    }
    prev = $1
    if (($2 in ts) && $1 < ts[$2]) {
        print "  FAIL: timestamp no monotonico para el coder " $2 " en linea " NR
        fail = 1
    }
    ts[$2] = $1
    if (msg == "has taken a dongle")
        takes[$2]++
    if (msg == "is compiling") {
        compiles[$2]++
        if (takes[$2] != 2) {
            print "  FAIL: coder " $2 " compilo con " takes[$2] " tomas (esperado 2)"
            fail = 1
        }
        takes[$2] = 0
    }
    coders[$2] = 1
}
END {
    if (!fail)
        print "  OK: " NR " lineas con formato valido"
    for (c = 1; c <= n; c++) {
        if (!(c in coders)) {
            print "  FAIL: el coder " c " no aparece en el log"
            fail = 1
        }
        else if (compiles[c] == 0) {
            print "  FAIL: el coder " c " no completo ninguna compilacion"
            fail = 1
        }
        else
            print "  OK: coder " c " con " compiles[c] " compilaciones (2 tomas cada una)"
    }
    exit (fail ? 1 : 0)
}
' "$LOG"
RESULT=$?

rm -f "$LOG"

if [ $RESULT -eq 0 ]; then
    echo "TEST PASSED"
else
    echo "TEST FAILED"
fi
exit $RESULT
