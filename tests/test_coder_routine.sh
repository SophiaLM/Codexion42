#!/bin/bash
# test_coder_routine.sh — compila y ejecuta tests unitarios para coder_routine

ROUTINE_SRC="../coders/src/coder_routine/coder_routine.c \
             ../coders/src/coder_routine/set_compile_start.c"
DONGLE_SRC="../coders/src/dongles/dongle_init.c \
            ../coders/src/dongles/dongle_destroy.c \
            ../coders/src/dongles/dongle_utils.c \
            ../coders/src/dongles/dongle_take.c \
            ../coders/src/dongles/dongle_release.c"
QUEUE_SRC="../coders/src/queue/queue_init.c \
          ../coders/src/queue/queue_utils.c \
          ../coders/src/queue/queue_push.c \
          ../coders/src/queue/queue_pop.c \
          ../coders/src/queue/queue_remove.c"
TIME_SRC="../coders/src/time_logging/time_utils.c \
         ../coders/src/time_logging/smart_sleep.c \
         ../coders/src/time_logging/sim_stopped.c \
         ../coders/src/time_logging/log_state.c"
SIM_SRC="../coders/src/sim/sim_init.c \
         ../coders/src/sim/sim_destroy.c"
LIBFT_SRC="../coders/src/auxiliary_libft/ft_calloc.c \
           ../coders/src/auxiliary_libft/ft_bzero.c \
           ../coders/src/auxiliary_libft/ft_putnbr.c \
           ../coders/src/auxiliary_libft/ft_putchar.c \
           ../coders/src/auxiliary_libft/ft_putchar_fd.c \
           ../coders/src/auxiliary_libft/ft_putendl.c \
           ../coders/src/auxiliary_libft/ft_putstr.c \
           ../coders/src/auxiliary_libft/ft_putstr_fd.c \
           ../coders/src/auxiliary_libft/ft_strlen.c"
TEST_MAIN="test_coder_routine.c"
FLAGS="-Wall -Wextra -Werror -pthread -I../coders/includes"
OUTPUT="test_coder_routine_bin"

cd "$(dirname "$0")" || exit 1
echo "Compilando..."
cc $FLAGS $ROUTINE_SRC $DONGLE_SRC $QUEUE_SRC $TIME_SRC $SIM_SRC $LIBFT_SRC $TEST_MAIN -o $OUTPUT
if [ $? -ne 0 ]; then
    echo "FALLO: la compilacion no funciono"
    exit 1
fi

echo "Ejecutando..."
timeout 15 ./$OUTPUT
RESULT=$?

rm -f $OUTPUT

if [ $RESULT -eq 0 ]; then
    echo "TEST PASSED"
else
    echo "TEST FAILED"
fi
exit $RESULT
