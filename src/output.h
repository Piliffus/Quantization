//
// Created by filip on 09.07.19.
//

#ifndef QUANTIZATION_OUTPUT_H
#define QUANTIZATION_OUTPUT_H

#include <stdbool.h>
#include <stdio.h>
#include "types.h"

/*
 * Prints error message to stderr
 */
void printError();

/*
 * Prints "YES" or "NO" dependent on value of valid. Meant to be used with
 * function checking whether given history is valid or not
 */
void printValid(bool valid);

/*
 * Prints given energy value
 */
void printEnergy(Energy energy);

/*
 * Prints "OK"
 */
void printConfirmation();

#endif //QUANTIZATION_OUTPUT_H