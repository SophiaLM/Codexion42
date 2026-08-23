#!/bin/bash
# test_fifo.sh — valida el scheduler FIFO de codexion.
#
# PARTE 1: corrida detallada (timeout 8s) que comprueba el formato del log,
#          la monotonia de timestamps, el invariante de exactamente 2 tomas
#          de dongle por compilacion y que cada coder complete al menos
#          COMPILES_REQ compilaciones.
# PARTE 2: estres con STRESS_RUNS ejecuciones cortas (timeout 2s) buscando
#          crashes (segfault/abort) o deadlocks (log vacio o sin progreso).
#
# NOTA: hoy NO existe monitor de parada, el programa corre para siempre.
#       Que timeout lo mate (exit 124) es lo normal y esperado; un crash
#       (139/134) o un cuelgue si es fallo.

BIN="../coders/codexion"
N_CODERS=5
BURNOUT=800
COMPILE=200
DEBUG=200
REFACTOR=200
COMPILES_REQ=5
COOLDOWN=0
SCHEDULER="fifo"

STRESS_RUNS="${STRESS_RUNS:-20}"
DETAIL_TIMEOUT=8
STRESS_TIMEOUT=2

WORKDIR="$(dirname "$0")"
TMPDIR_LOGS="$WORKDIR/test_fifo_tmp"
DETAIL_LOG="$TMPDIR_LOGS/detail.log"
FAIL=0

cd "$WORKDIR" || exit 1
mkdir -p "$TMPDIR_LOGS"
trap 'rm -rf "$TMPDIR_LOGS"' EXIT

# ------------------------- PARTE 1: corrida detallada ------------------------

echo "=== PARTE 1: corrida detallada (timeout ${DETAIL_TIMEOUT}s) ==="

echo "Compilando (make -C ../coders)..."
make -C ../coders > /dev/null 2>&1
if [ ! -x "$BIN" ]; then
    echo "FALLO: no se pudo compilar el binario $BIN"
    exit 1
fi

echo "Ejecutando: timeout $DETAIL_TIMEOUT $BIN $N_CODERS $BURNOUT $COMPILE $DEBUG $REFACTOR $COMPILES_REQ $COOLDOWN $SCHEDULER"
timeout "$DETAIL_TIMEOUT" "$BIN" $N_CODERS $BURNOUT $COMPILE $DEBUG $REFACTOR $COMPILES_REQ $COOLDOWN $SCHEDULER > "$DETAIL_LOG" 2>&1
rc=$?

if [ "$rc" -ne 0 ] && [ "$rc" -ne 124 ]; then
    echo "FALLO: el proceso termino con codigo $rc (crash)"
    exit 1
fi

# Si el timeout corto la escritura a mitad de linea, descartamos esa ultima
# linea incompleta (sin salto de linea final) para no dar un falso fallo.
if [ -s "$DETAIL_LOG" ] && [ -n "$(tail -c 1 "$DETAIL_LOG")" ]; then
    head -n -1 "$DETAIL_LOG" > "$DETAIL_LOG.tmp"
    mv "$DETAIL_LOG.tmp" "$DETAIL_LOG"
fi

if [ ! -s "$DETAIL_LOG" ]; then
    echo "FALLO: log vacio (cuelgue o sin progreso)"
    exit 1
fi

awk -v n="$N_CODERS" -v req="$COMPILES_REQ" '
BEGIN {
    fail = 0
    print "  --- formato <timestamp> <coder_id> <mensaje> ---"
    print "  --- timestamps monotonos (global y por coder) ---"
    print "  --- exactamente 2 tomas de dongle antes de cada compilacion ---"
    print "  --- al menos " req " compilaciones por coder ---"
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
        else if (compiles[c] < req) {
            print "  FAIL: el coder " c " completo " compiles[c] " compilaciones (< " req ")"
            fail = 1
        }
        else
            print "  OK: coder " c " con " compiles[c] " compilaciones (2 tomas cada una)"
    }
    exit (fail ? 1 : 0)
}
' "$DETAIL_LOG"
detail_rc=$?

if [ "$detail_rc" -ne 0 ]; then
    echo ""
    echo "TEST FAILED"
    exit 1
fi

# ------------------------- PARTE 2: estres -------------------------

echo ""
echo "=== PARTE 2: estres ($STRESS_RUNS ejecuciones de ${STRESS_TIMEOUT}s) ==="

case "$STRESS_RUNS" in
    ''|*[!0-9]*)
        echo "FALLO: STRESS_RUNS no es un numero positivo: '$STRESS_RUNS'"
        echo "TEST FAILED"
        exit 1
        ;;
esac

n=0
while [ "$n" -lt "$STRESS_RUNS" ]; do
    n=$((n + 1))
    LOG="$TMPDIR_LOGS/stress_$n.log"

    timeout "$STRESS_TIMEOUT" "$BIN" $N_CODERS $BURNOUT $COMPILE $DEBUG $REFACTOR $COMPILES_REQ $COOLDOWN $SCHEDULER > "$LOG" 2>&1
    rc=$?

    if [ "$rc" -ne 0 ] && [ "$rc" -ne 124 ]; then
        echo "run $n/$STRESS_RUNS FALLO (codigo de salida $rc)"
        FAIL=1
        continue
    fi

    # Igual que en la parte 1: descartamos una ultima linea cortada por timeout.
    if [ -s "$LOG" ] && [ -n "$(tail -c 1 "$LOG")" ]; then
        head -n -1 "$LOG" > "$LOG.tmp"
        mv "$LOG.tmp" "$LOG"
    fi

    if [ ! -s "$LOG" ]; then
        echo "run $n/$STRESS_RUNS FALLO (log vacio, posible cuelgue)"
        FAIL=1
        continue
    fi

    reasons=$(awk -v n="$N_CODERS" '
    {
        if (NF < 3 || $1 !~ /^[0-9]+$/ || $2 !~ /^[0-9]+$/) {
            print "linea con formato invalido: [" $0 "]"
            fail = 1
            next
        }
        msg = $0
        sub(/^[0-9]+ [0-9]+ /, "", msg)
        if (msg != "has taken a dongle" && msg != "is compiling" \
            && msg != "has released the dongles" && msg != "is debugging" \
            && msg != "is refactoring") {
            print "mensaje no reconocido: [" msg "]"
            fail = 1
            next
        }
        if (msg == "is compiling")
            compiles[$2]++
        coders[$2] = 1
    }
    END {
        for (c = 1; c <= n; c++) {
            if (!(c in coders)) {
                print "coder " c " ausente"
                fail = 1
            }
            else if (compiles[c] == 0) {
                print "coder " c " sin compilaciones (posible deadlock)"
                fail = 1
            }
        }
        exit (fail ? 1 : 0)
    }
    ' "$LOG")
    rc=$?

    if [ "$rc" -ne 0 ]; then
        first_reason=$(printf '%s\n' "$reasons" | head -n 1)
        echo "run $n/$STRESS_RUNS FALLO ($first_reason)"
        FAIL=1
    else
        echo "run $n/$STRESS_RUNS OK"
    fi
done

# ------------------------- resumen -------------------------

echo ""
if [ "$FAIL" -eq 0 ]; then
    echo "TEST PASSED"
    exit 0
else
    echo "TEST FAILED"
    exit 1
fi
