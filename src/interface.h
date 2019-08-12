//
// Created by filip on 08.07.19.
//

#ifndef QUANTIZATION_INTERFACE_H
#define QUANTIZATION_INTERFACE_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"

#define DECLARE 1
#define REMOVE 2
#define VALID 3
#define ENERGY 4
#define ENERGY_SHORT 5
#define EQUAL 6
#define PASS 7
#define ERROR 8

#define SPACES_SHORT_INPUT 1
#define SPACES_LONG_INPUT 2

#define CHAR_BUFFER 4096

/*
 * Function for analyzing user input.
 * input - input string to be analyzed
 * argument1 - first argument, returned to be used by next functions
 * argument2 - second argument, returned to be used by next functions
 * operation - information which function should be executed
 */
void
analyzeInput(char *input, char **argument1, char **argument2, int *operation);

/*
 * Function for reading user input
 * Reads line of text from standard input. If it was the last one, sets terminate
 * to true.
 */
char *getInput(bool *terminate, char *buffer, unsigned bufferSize);

/*
 * Checks whether entire line of text from stdin was read, or it was too large
 */
bool entireLineRead(char *command);

/*
 * If line of text was too long, then expands command so the rest of fits too.
 * Terminates if file ends unexpectedly, and sets endOFFile to true.
 */
void
readNextPart(char **command, char **buffer, unsigned *bufferSize, bool *endOfFile);

#endif //QUANTIZATION_INTERFACE_H
