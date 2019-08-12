CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2
LDFLAGS =

.PHONY: all clean

all: main

main: main.o interface.o quantum_operations.o output.o
	$(CC) $(LDFLAGS) -o $@ $^

interface.o: interface.c interface.h types.h
	$(CC) $(CFLAGS) -c $<

quantum_operations.o: quantum_operations.c quantum_operations.h types.h
	$(CC) $(CFLAGS) -c $<

output.o: output.c output.h types.h
	$(CC) $(CFLAGS) -c $<

main.o: main.c interface.h quantum_operations.h output.h types.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o main
