#!/bin/bash
if (($# != 2)); then
    echo "Wrong usage! Correct usage: ./test.sh ./<program> <directory>"
    exit 1
fi

VALGRIND="valgrind --error-exitcode=15 --leak-check=full --show-leak-kinds=all --errors-for-leak-kinds=all --quiet"
PROGRAM=$1
DIRECTORY=$2

TESTS_AMOUNT=$(ls -1q $DIRECTORY/*.in | wc -l)
PASSED_AMOUNT=0
MEMORY_PASSED_COUNT=0

for i in $DIRECTORY/*.in ; do
    echo -n "Now testing ${i#*$DIRECTORY/}... "
    
    $VALGRIND $PROGRAM <$i 2>current.err >current.out
    EXIT_CODE=$?

    #Memory    
    if (($EXIT_CODE == 15)); then
        echo -n "DETECTED MEMORY LEAKS,  "
    elif (($EXIT_CODE == 0)); then 
        echo -n "MEMORY OK,  "
        ((MEMORY_PASSED_COUNT++))
    else 
        echo -n "VALGRIND ERROR, EXIT CODE $EXIT_CODE  "
    fi

    #Output
    if cmp -s ${i%in}out current.out; then
        if cmp -s ${i%in}err current.err; then
            echo "OUTPUT OK"
            ((PASSED_AMOUNT++))
        else
            echo "OUTPUT WRONG"
        fi
    else
        echo "OUTPUT WRONG"
    fi
done

#Summary
echo "IN $PASSED_AMOUNT/$TESTS_AMOUNT OUTPUT OK"
echo "IN $MEMORY_PASSED_COUNT/$TESTS_AMOUNT MEMORY OK"

#Cleaning
rm -f current.err current.out