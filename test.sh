#!/bin/bash
if (($# != 2)); then
    echo "correct usage: ./test.sh <prog> <dir>"
    exit 1
fi

VALGRIND="valgrind --error-exitcode=15 --leak-check=full --show-leak-kinds=all --errors-for-leak-kinds=all --quiet"
DEBUG=0
PROG=$1
DIR=$2

TOTAL_TESTS=$(ls -1q $DIR/*.in | wc -l)
CORRECT_COUNT=0
MEMORY_OK_COUNT=0

for f in $DIR/*.in ; do
    echo -n "Test ${f#*$DIR/}  "
    if (($DEBUG == 1)); then
        $VALGRIND $PROG <$f 2>current.err >current.out
        EXIT_CODE=$?
        if (($EXIT_CODE == 15)); then
            echo -n "MEMORY LEAK  "
            cp current.err ${f%in}memerr
        elif (($EXIT_CODE == 0)); then 
            echo -n "NO LEAKS  "
            ((MEMORY_OK_COUNT++))
        else 
            echo -n "UNKNOWN ERROR, EXIT CODE $EXIT_CODE  "
        fi
    else
        $PROG <$f 2>current.err >current.out
    fi
    if cmp -s ${f%in}out current.out; then
        if cmp -s ${f%in}err current.err; then
            echo "OK"
            ((CORRECT_COUNT++))
        else
            echo "FAILED"
        fi
    else
        echo "FAILED"
    fi
done

echo "$CORRECT_COUNT/$TOTAL_TESTS passed"
if (($DEBUG == 1)); then
    echo "$MEMORY_OK_COUNT/$TOTAL_TESTS have no leaks"
fi

rm -f current.err current.out