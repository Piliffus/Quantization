//
// Created by filip on 09.07.19.
//

#include "output.h"

void printError()
{
    fprintf(stderr, "ERROR\n");
}

void printValid(bool valid)
{
    fprintf(stdout, valid ? "YES\n" : "NO\n");
}

void printEnergy(Energy energy)
{
    if (energy == 0) printError();

    else fprintf(stdout, "%" PRIu64 "\n", energy);
}

void printConfirmation()
{
    fprintf(stdout, "OK\n");
}