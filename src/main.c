#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "interface.h"
#include "quantum_operations.h"
#include "output.h"
#include "types.h"

int main()
{
    char *buffer = malloc(sizeof(char) * CHAR_BUFFER);
    if (buffer == NULL) return 1;
    for (int i = 0; i < CHAR_BUFFER; ++i)
    {
        buffer[i] = '\0';
    }

    Tree *histories = initializeTree();
    if (histories == NULL)
    {
        free(buffer);
        return 1; // Failed to allocate memory for main data structure
    }

    unsigned bufferSize = CHAR_BUFFER;
    bool terminate = false;

    while (!terminate)
    {
        bool unexpectedFileEnd = false;

        char *command = getInput(&terminate, buffer,
                                 bufferSize); // todo memfail?
        if (command == NULL) break;

        while (!entireLineRead(command) && !unexpectedFileEnd)
        {
            readNextPart(&command, &buffer, &bufferSize, &unexpectedFileEnd);
            if (unexpectedFileEnd)
            {
                printError();
                terminate = true;
                free(command);
            }
        }
        if (terminate) break;

        char *argument1 = NULL;
        char *argument2 = NULL;
        bool memFail = false;
        bool error = false;
        int operation = ERROR;

        analyzeInput(command, &argument1, &argument2, &operation);

        switch (operation)
        {
            case DECLARE:
                declareHistory(argument1, histories, &memFail);
                if (!memFail) printConfirmation();
                break;
            case REMOVE:
                removeHistory(argument1, histories);
                printConfirmation();
                break;
            case VALID:
                printValid(validHistory(argument1, histories));
                break;
            case ENERGY:
                energyHistory(argument1, argument2, histories, &error,
                              &memFail);
                if (!error && !memFail) printConfirmation();
                break;
            case ENERGY_SHORT:
                printEnergy(energyShortHistory(argument1, histories));
                break;
            case EQUAL:
                equalHistory(argument1, argument2, histories, &error, &memFail);
                if (!error && !memFail) printConfirmation();
                break;
            case PASS:
                break;
            case ERROR:
            default:
                printError();
                break;
        }

        if (memFail) // out of memory is critical error and terminates program
        {
            removeTree(histories);
            free(command);
            free(buffer);
            return 1;
        }

        if (error)
        {
            printError();
        }

        free(command);
    }

    free(buffer);
    removeTree(histories);
    return 0;
}

