//
// Created by filip on 08.07.19.
//

#ifndef QUANTIZATION_QUANTUM_OPERATIONS_H
#define QUANTIZATION_QUANTUM_OPERATIONS_H

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "types.h"

/*
 * Every history that is prefix of history passed as argument, will be considered
 * valid after executing this function. memFail is used to signal if there
 * was failure allocating memory, in this case it is set to true. In all other
 * cases it should be false
 */
void declareHistory(char *argument, Tree *histories, bool *memFail);

/*
 * Every history that is postfix of history passed as argument, will be no longer
 * considered valid after executing this function.
 */
void removeHistory(char *argument, Tree *histories);

/*
 * Checks if given history is valid, returns true if it is, false if it isn`t
 */
bool validHistory(char *argument, Tree *histories);

/*
 * Assigns energy given as argument2 to history given as argument.
 * Sets "error" to true if history is not declared, or "memFail" if out of memory
 */
void
energyHistory(char *argument, char *argument2, Tree *histories, bool *error);

/*
 * Returns the energy value for given history, or 0 if no energy assigned or no
 * such history
 */
Energy energyShortHistory(char *argument, Tree *histories);

/*
 * Function puts two histories given as "argument" and "argument2" into equality
 * relation, their energies will be the same from now on. "memFail" is set to
 * true if out of memory, "error" is set in case of remaining errors.
 */
void equalHistory(char *argument, char *argument2, Tree *histories, bool *error,
                  bool *memFail);

/*
 * Function creates new data structure for holding histories.
 * Returns pointer to data structure entry point or NULL if allocation failed.
 * Note that first node is just an entry point, and should not be considered part
 * of any history nor assigned any energy.
 */
Tree *initializeTree();

/*
 * Function removes the data structure made for holding histories.
 */
void removeTree(Tree *histories);

#endif //QUANTIZATION_QUANTUM_OPERATIONS_H