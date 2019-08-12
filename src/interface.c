//
// Created by filip on 08.07.19.
//

#include "interface.h"

/*
 * Function checking whether the argument is correct history. Correct argument is a
 * string consisting of only 0, 1, 2 and 3. (For STATES 4)
 * Also, depending on the value of lineEnd argument, function will expect either
 * '\n' or '\0' at the end. True means argument is correct, false means the
 * opposite. Empty strings are considered erroneous.
 */
static bool isCorrectHistory(char *argument, bool lineEnd);

/*
 * Function checking whether the argument is correct number. Also, depending on
 * the value of lineEnd argument, function will expect either '\n' or '\0' at
 * the end. True means argument is correct, false means the opposite.
 * Empty strings are considered erroneous.
 */
static bool isCorrectNumber(char *argument, bool lineEnd);

/*
 * Function resetting all chars in given string to '\0'
 */
static void resetString(char *string);

/*
 * Function removes endl from string, so that strlen returns proper value and
 * following functions don`t have to check for special case with unexpected '\n'.
 * Function assumes that given string passed isCorrectHistory test, and so it has only
 * one '\n', at strlen(argument) - 1 index.
 */
static void removeEndl(char *argument);

/*
 * Function checks whether given char is included in the range. Note that "from"
 * and "to" are inclusive. Returns true if char is in range, false otherwise
 */
static bool isCharBetween(char checked, char from, char to);

/*
 * Returns count of how many spaces there are in given string. Stops counting
 * when the amount of spaces found reaches max.
 */
static int countSpaces(char *input, unsigned max);

void
analyzeInput(char *input, char **argument1, char **argument2, int *operation)
{
    if (input == NULL || input[0] == '#' || input[0] == '\n')
    {
        *operation = PASS;
        return;
    }

    // We must count spaces before strtok() gets rid of them
    int spacesCount = countSpaces(input, SPACES_LONG_INPUT + 1);

    strtok(input, " ");
    *argument1 = strtok(NULL, " ");
    *argument2 = strtok(NULL, " ");
    *operation = ERROR;

    // DECLARE X
    if (strcmp(input, "DECLARE") == 0)
    {
        if (!isCorrectHistory(*argument1, true) ||
            spacesCount > SPACES_SHORT_INPUT)
        {
            *operation = ERROR;
            return;
        }
        else
        {
            removeEndl(*argument1);
            *operation = DECLARE;
        }
    }

    // REMOVE X
    else if (strcmp(input, "REMOVE") == 0)
    {
        if (!isCorrectHistory(*argument1, true) ||
            spacesCount > SPACES_SHORT_INPUT)
        {
            *operation = ERROR;
            return;
        }
        else
        {
            removeEndl(*argument1);
            *operation = REMOVE;
        }
    }

    // VALID X
    else if (strcmp(input, "VALID") == 0)
    {
        if (!isCorrectHistory(*argument1, true) ||
            spacesCount > SPACES_SHORT_INPUT)
        {
            *operation = ERROR;
            return;
        }
        else
        {
            removeEndl(*argument1);
            *operation = VALID;
        }
        *operation = VALID;
    }

    // ENERGY X1 (X2)
    else if (strcmp(input, "ENERGY") == 0)
    {
        if (*argument2 == NULL)
        {
            if (!isCorrectHistory(*argument1, true) ||
                spacesCount > SPACES_SHORT_INPUT)
            {
                *operation = ERROR;
                return;
            }
            else
            {
                removeEndl(*argument1);
                *operation = ENERGY_SHORT;
            }
        }
        else // 2 argument version of Energy
        {
            if (!isCorrectHistory(*argument1, false) ||
                !isCorrectNumber(*argument2, true) ||
                spacesCount > SPACES_LONG_INPUT)
            {
                *operation = ERROR;
            }
            else
            {
                removeEndl(*argument2);
                *operation = ENERGY;
            }
        }
    }

    // EQUAL X1 X2
    else if (strcmp(input, "EQUAL") == 0)
    {
        // Equal has two arguments
        if (!isCorrectHistory(*argument1, false) ||
            !isCorrectHistory(*argument2, true) ||
            spacesCount > SPACES_LONG_INPUT)
        {
            *operation = ERROR;
        }
        else
        {
            removeEndl(*argument2);
            *operation = EQUAL;
        }
    }

    // Unrecognised input
    else
    {
        *operation = ERROR;
    }
}

static int countSpaces(char *input, unsigned max)
{
    unsigned spaces = 0;
    unsigned length = strlen(input);

    for (unsigned i = 0; i < length && spaces < max; ++i)
    {
        if (input[i] == ' ') ++spaces;
    }

    return spaces;
}

static void removeEndl(char *argument)
{
    argument[strlen(argument) - 1] = '\0';
}

char *getInput(bool *terminate, char *buffer, unsigned bufferSize)
{
    resetString(buffer);

    char *command = malloc(sizeof(char) * bufferSize);
    for (unsigned i = 0; i < bufferSize; ++i)
    {
        buffer[i] = '\0';
    }

    if (fgets(buffer, bufferSize, stdin) == NULL)
    {
        *terminate = true;
        free(command);
        return NULL;
    }

    else
    {
        strcpy(command, buffer);
        return command;
        // TODO: not entire line read check, UNEXPECTED EOF, script, warnings
    }
}

static bool isCorrectHistory(char *argument, bool lineEnd)
{
    if (argument == NULL) return false;
    unsigned length = strlen(argument);

    for (unsigned i = 0; i < length; ++i)
    {
        if ((i == length - 1) && (lineEnd))
        {
            if (argument[i] != '\n')
            {
                return false;
            }
        }

        // STATES - 1 so we don`t include '4'
        else if (!isCharBetween(argument[i], '0', '0' + STATES - 1))
        {
            return false;
        }
    }

    return true;
}

static void resetString(char *string)
{
    if (string != NULL)
    {
        string[0] = '\0';
    }
}

static bool isCorrectNumber(char *argument, bool lineEnd)
{
    if (argument == NULL) return false;
    unsigned length = strlen(argument);

    for (unsigned i = 0; i < length; ++i)
    {
        if ((i == length - 1) && (lineEnd))
        {
            if (argument[i] != '\n')
            {
                return false;
            }
        }

        else if (!isCharBetween(argument[i], '0', '9'))
        {
            return false;
        }
    }

    return true;
}

static bool isCharBetween(char checked, char from, char to)
{
    if (checked < from || checked > to) return false;
    else return true;
}

bool entireLineRead(char *command)
{
    if (command[0] == '\0') return true; // unexpected EOFs are others` problem
    return command[strlen(command) - 1] == '\n' ? true : false;
}

void
readNextPart(char **command, char **buffer, unsigned *bufferSize, bool *endOfFile)
{
    // double the buffer size
    *bufferSize = (*bufferSize) * 2;

    // increase size of buffer
    *buffer = realloc(*buffer, sizeof(char) * (*bufferSize));

    // buffer contents are saved in command, so can be discarded to save space
    resetString(*buffer);

    // unexpected end of file
    if (fgets(*buffer, (*bufferSize), stdin) == NULL)
    {
        *endOfFile = true;
        return;
    }

    else
    {
        unsigned oldCommandSize = strlen(*command);
        // increase command size by just the amount we need: what we have read
        // now + terminating character. (We also add the size of what was
        // already read to realloc() so we won`t lose anything)
        *command = realloc(*command, sizeof(char) *
                                     (oldCommandSize + strlen(*buffer) + 1));
        strcpy(*command + oldCommandSize, *buffer);
        return;
    }
}