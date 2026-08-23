#!/bin/bash

HEAP_SRC="../coders/src/queue/queue_init.c \
          ../coders/src/queue/queue_utils.c \
          ../coders/src/queue/queue_push.c \
          ../coders/src/queue/queue_pop.c \
          ../coders/src/queue/queue_remove.c \
          ../coders/src/auxiliary_libft/ft_calloc.c \
          ../coders/src/auxiliary_libft/ft_bzero.c"
TEST_MAIN="test_heap_main.c"
FLAGS="-Wall -Wextra -Werror"
OUTPUT="test_heap_bin"

cd "$(dirname "$0")" || exit 1
echo "Compilando..."
cc $FLAGS $HEAP_SRC $TEST_MAIN -o $OUTPUT
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
