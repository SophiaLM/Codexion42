#!/usr/bin/env bash
# =============================================================================
#  theme.sh — FUENTE UNICA DEL TEMA VISUAL DE LA SUITE
# -----------------------------------------------------------------------------
#  - run_tests.sh lo hace "source" para sus propios mensajes.
#  - run_tests.sh ademas deriva tests/theme.h (incluido por test_framework.h)
#    desde estas variables, para que bash y C compartan colores y layout.
#  NO edites colores en el .h ni en los .c: cualquier cambio va aca.
# =============================================================================

# Colores ANSI de 256 tonos: el valor es el codigo completo "\033[...]m".
TF_PINK='\033[38;5;218m'
TF_STRONG='\033[38;5;204m'
TF_LILAC='\033[38;5;141m'
TF_GOLD='\033[38;5;215m'
TF_GREEN='\033[38;5;113m'
TF_RED='\033[38;5;196m'
TF_RESET='\033[0m'

# Ancho de los separadores (sep/dash).
TF_SEP_W=94