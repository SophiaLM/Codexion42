#!/bin/bash

TIME_SRC="../coders/src/time_logging/time_utils.c \
          ../coders/src/time_logging/sim_stopped.c \
          ../coders/src/time_logging/smart_sleep.c \
          ../coders/src/time_logging/log_state.c \
          ../coders/src/auxiliary_libft/ft_putnbr.c \
          ../coders/src/auxiliary_libft/ft_putchar.c \
          ../coders/src/auxiliary_libft/ft_putendl.c \
          ../coders/src/auxiliary_libft/ft_putstr.c"
TEST_MAIN="test_time_logging.c"
FLAGS="-Wall -Wextra -Werror -lpthread -I../coders/includes"
OUTPUT="test_time_logging_bin"

cd "$(dirname "$0")" || exit 1
echo "Compilando..."
cc $FLAGS $TIME_SRC $TEST_MAIN -o $OUTPUT
if [ $? -ne 0 ]; then
    echo "FALLO: la compilacion no funciono"
    exit 1
fi

echo "Ejecutando..."
./$OUTPUT
RESULT=$?

rm -f $OUTPUT

if [ $RESULT -eq 0 ]; then
    echo "TEST PASSED"
else
    echo "TEST FAILED"
fi
exit $RESULT
