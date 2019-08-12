//
// Created by filip on 08.07.19.
//

#include <errno.h>
#include "quantum_operations.h"


/*
 * Structure used to mark nodes as already visited in given run of energy update
 * in entire equality relation
 */
struct Visited
{
    Tree *this;
    struct Visited *next;
};
typedef struct Visited Visited;

/*
 * This function returns index associated with given char, necessary to access
 * proper node in the histories tree. For example: for char '0' int 0 is returned,
 * for char '1' int 1 etc. Function takes advantage of the fact chars are in
 * order from lowest to highest, so for example if we subtract '0' from '1' we get 1.
 */
static int charToIndex(char argument);

/*
 * Helper function for history removal. It calls itself on all available "next"
 * nodes, and then removes node given as argument.
 */
static void recurrentRemoval(Tree *histories);

/*
 * Function sets all "next" pointers to NULL, and energy to 0, which means no
 * energy assigned. Intended to be used on newly made nodes
 */
static void allNull(Tree *newNode);

/*
 * Function parses given string to Energy value, sets "error" to true if there
 * were any errors.
 */
static Energy parseToEnergy(char *argument, bool **error);

/*
 * Function walks through histories tree and returns node described with "argument" string
 * Sets "error" to true if there is no such history
 */
static Tree *getHistory(char *argument, Tree *histories, bool **error);

/*
 * Makes new Equals data structure, used to connect two histories in equality
 * relation. Sets "memFail" to true if there is not enough memory available
 */
static Equals *makeNewEquals(Tree *historyA, Tree *historyB, bool **memFail);

/*
 * Adds Equals data structure to list kept by given history.
 * Sets "memFail" to true if there is not enough memory available
 */
static void addToEquals(Equals *newEquals, Tree *history, bool **memFail);

/*
 * Updates all nodes which are in equality relation with given node
 */
static void
updateEquals(Tree *node, Energy energy, Visited *visited, bool *memFail);

/*
 * Checks if given node has been already visited in current run of energy update
 * in entire equality relation
 */
static bool isVisited(Tree *node, Visited *visited);

/*
 * Marks node as visited in current run of energy update in entire equality relation
 */
static void markVisited(Tree *node, Visited *visited, bool *memFail);

/*
 * Runs updateEquals for all nodes equalized with given node
 */
static void
visitAllEquals(Tree *node, Energy energy, Visited *visited, bool *memFail);

/*
 * This function removes all EqualsList and Equals associated with given node.
 * It also removes appropriate EqualsList from each node it was equalized with
 */
static void removeAllEquals(Tree *node);

/*
 * Removes given equality  node`s equality list. Note that "Equals"
 * data structure itself is not removed, by this function, just the EqualsList.
 * "Equals" is by default removed by removeAllEquals called in the node which is
 * currently being removed
 */
static void removeFromEquals(Tree *node, Equals *equals);

/*
 * Checks if histories are already equalized. Returns true if they are, false
 * otherwise
 */
static bool alreadyEqual(Tree *historyA, Tree *historyB);

/*
 * Checks if given history has any energy assigned
 */
static bool hasEnergy(Tree *history);

/*
 * Updates energy on two equalized histories, meant to be used on newly equated
 * histories
 */
static void updateEnergy(Tree *historyA, Tree *historyB, bool *memFail);

/*
 * Removes list of visited nodes, meant to be used after it is no longer needed.
 */
static void removeVisited(Visited *visited);

/*
 * Calculates average of two energy values
 */
static Energy average(Energy energyA, Energy energyB);

Tree *initializeTree()
{
    Tree *start = malloc(sizeof(Tree));

    // mem alloc fail
    if (start == NULL)
    {
        return NULL;
    }

    allNull(start);

    return start;
}

static void allNull(Tree *newNode)
{
    for (unsigned i = 0; i < STATES; ++i)
    {
        newNode->next[i] = NULL;
    }

    newNode->equalsList = NULL;
    newNode->energy = 0;
}

void removeTree(Tree *histories)
{
    for (unsigned i = 0; i < STATES; ++i)
    {
        if (histories->next[i] != NULL)
        {
            removeTree(histories->next[i]);
        }
    }

    removeAllEquals(histories);
    free(histories);
}

static int charToIndex(char argument)
{
    return argument - '0';
}

void declareHistory(char *argument, Tree *histories, bool *memFail)
{
    unsigned length = strlen(argument);
    for (unsigned i = 0; i < length; ++i)
    {
        // History was not already declared, so we must make new one
        if (histories->next[charToIndex(argument[i])] == NULL)
        {
            histories->next[charToIndex(argument[i])] = malloc(sizeof(Tree));
            // Memory allocation unsuccessful
            if (histories->next[charToIndex(argument[i])] == NULL)
            {
                *memFail = true;
                return;
            }

            allNull(histories->next[charToIndex(argument[i])]);
        }

        histories = histories->next[charToIndex(argument[i])];
    }
}

void removeHistory(char *argument, Tree *histories)
{
    Tree *lastNotRemoved = histories; // We must set its "next" to NULL
    unsigned length = strlen(argument);

    for (unsigned i = 0; i < length; ++i)
    {
        if (histories->next[charToIndex(argument[i])] == NULL) return;

        if (i == length - 1)
        {
            lastNotRemoved = histories;
        }

        histories = histories->next[charToIndex(argument[i])];
    }

    lastNotRemoved->next[charToIndex(argument[length - 1])] = NULL;

    recurrentRemoval(histories);
}

static void recurrentRemoval(Tree *histories)
{
    for (unsigned i = 0; i < STATES; ++i)
    {
        if (histories->next[i] != NULL)
        {
            recurrentRemoval(histories->next[i]);
            histories->next[i] = NULL;
        }
    }

    removeAllEquals(histories);
    free(histories);
}

static void removeAllEquals(Tree *node)
{
    EqualsList *equals = node->equalsList;

    // we must remove each Equality from the other history`s list, because
    // otherwise it would be equated with nonexistent history
    while (equals != NULL)
    {
        Tree *otherHistory = equals->this->historyA == node ?
                             equals->this->historyB :
                             equals->this->historyA;

        // we don`t remove it from node`s list, because node is set to be removed
        // soon anyway and it`s entire equalsList with it
        removeFromEquals(otherHistory, equals->this);
        EqualsList *toRemove = equals;
        equals = equals->next;
        free(toRemove->this);
        free(toRemove);

    }
}

static void removeFromEquals(Tree *node, Equals *equals)
{
    // special case - first node is set to be removed. It`s different because we
    // modify node parameter
    if (node->equalsList->this == equals)
    {
        EqualsList *toRemove = node->equalsList;
        node->equalsList = node->equalsList->next;
        free(toRemove); // Equals will be removed by removeAllEquals, here we just remove node
    }
    else
    {
        EqualsList *previous = node->equalsList;
        EqualsList *this = previous->next;

        while (this->this != equals)
        {
            previous = this;
            this = this->next;
        }
        previous->next = this->next;
        free(this);
    }
}

bool validHistory(char *argument, Tree *histories)
{
    bool error = false;
    bool *pError = &error;
    getHistory(argument, histories, &pError);

    if (*pError) return false;

    else return true;
}

void
energyHistory(char *argument, char *argument2, Tree *histories, bool *error,
              bool *memFail)
{
    Energy energy = parseToEnergy(argument2, &error);
    if (*error) return;

    Tree *energyHolder = getHistory(argument, histories, &error);
    if (*error) return;

    energyHolder->energy = energy;
    Visited *visited = malloc(sizeof(Visited));
    if (visited == NULL)
    {
        *memFail = true;
        return;
    }

    visited->this = energyHolder;
    visited->next = NULL;

    visitAllEquals(energyHolder, energy, visited, memFail);

    removeVisited(visited);
}

static void
updateEquals(Tree *node, Energy energy, Visited *visited, bool *memFail)
{
    if (isVisited(node, visited) || *memFail) return;

    else
    {
        node->energy = energy;
        markVisited(node, visited, memFail);
        visitAllEquals(node, energy, visited, memFail);
    }
}

static void
visitAllEquals(Tree *node, Energy energy, Visited *visited, bool *memFail)
{
    EqualsList *equals = node->equalsList;

    while (equals != NULL)
    {
        equals->this->historyA == node ?
        updateEquals(equals->this->historyB, energy, visited, memFail) :
        updateEquals(equals->this->historyA, energy, visited, memFail);

        equals = equals->next;
    }
}

static void markVisited(Tree *node, Visited *visitedList, bool *memFail)
{
    while (visitedList->next != NULL)
    {
        visitedList = visitedList->next;
    }

    visitedList->next = malloc(sizeof(Visited));
    if (visitedList->next == NULL)
    {
        *memFail = true;
        return;
    }

    visitedList = visitedList->next;
    visitedList->next = NULL;
    visitedList->this = node;
}

static bool isVisited(Tree *node, Visited *visited)
{
    while (visited != NULL)
    {
        if (visited->this == node)
        {
            return true;
        }

        else visited = visited->next;
    }

    return false;
}

static Energy parseToEnergy(char *argument, bool **error)
{
    errno = 0;
    Energy energy = strtoull(argument, NULL, 10);

    if (errno == EINVAL || errno == ERANGE || energy == 0)
    {
        **error = true;
        return 0;
    }

    else return energy;
}

Energy energyShortHistory(char *argument, Tree *histories)
{
    bool error = false;
    bool *pError = &error;

    Tree *energyHolder = getHistory(argument, histories, &pError);

    if (*pError == true) return 0;

        // 0 means no energy assigned, and it will be checked for by output function
    else return energyHolder->energy;
}

void equalHistory(char *argument, char *argument2, Tree *histories, bool *error,
                  bool *memFail)
{
    Tree *historyA = getHistory(argument, histories, &error);
    Tree *historyB = getHistory(argument2, histories, &error);
    if (*error) return;

    if (alreadyEqual(historyA, historyB)) return;
    if (historyA == historyB) return;

    if (!hasEnergy(historyA) && !hasEnergy(historyB))
    {
        *error = true;
        return;
    }

    Equals *newEquals = makeNewEquals(historyA, historyB, &memFail);
    if (*memFail) return;

    addToEquals(newEquals, historyA, &memFail);
    if (*memFail) return;

    addToEquals(newEquals, historyB, &memFail);
    if (*memFail) return;

    updateEnergy(historyA, historyB, memFail);
}

static void updateEnergy(Tree *historyA, Tree *historyB, bool *memFail)
{
    Energy energy;

    if (historyA->energy <= 0) // A has no energy
    {
        energy = historyB->energy;
        historyA->energy = energy;
    }
    else if (historyB->energy <= 0) // B has no energy
    {
        energy = historyA->energy;
        historyB->energy = energy;
    }
    else // Both have energy, so we must calculate average
    {
        energy = average(historyA->energy, historyB->energy);
        historyA->energy = energy;
        historyB->energy = energy;
    }

    Visited *visited = malloc(sizeof(Visited));
    if (visited == NULL)
    {
        *memFail = true;
        return;
    }

    visited->this = historyA;
    visited->next = NULL;

    // It doesnt matter whether we start with history A or B
    visitAllEquals(historyA, energy, visited, memFail);

    removeVisited(visited);
}

static Energy average(Energy energyA, Energy energyB)
{
    // We don`t want to overflow, so we can`t just do a+b/2
    return (energyA / 2) + (energyB / 2) + ((energyA % 2 + energyB % 2) / 2);
}

static void removeVisited(Visited *visited)
{
    while (visited != NULL)
    {
        Visited *toRemove = visited;
        visited = visited->next;
        free(toRemove);
    }
}

static bool hasEnergy(Tree *history)
{
    return history->energy > 0 ? true : false;
}

static bool alreadyEqual(Tree *historyA, Tree *historyB)
{
    EqualsList *equals = historyA->equalsList;

    while (equals != NULL)
    {
        if (equals->this->historyA == historyB ||
            equals->this->historyB == historyB)
            return true;

        else equals = equals->next;
    }

    return false;
}

static void addToEquals(Equals *newEquals, Tree *history, bool **memFail)
{
    EqualsList *equalsList = history->equalsList;
    EqualsList *previous = NULL;

    while (equalsList != NULL)
    {
        previous = equalsList;
        equalsList = equalsList->next;
    }

    // previous->next == NULL, new node will be added here
    if (previous != NULL)
    {
        previous->next = malloc(sizeof(EqualsList));
        if (previous->next == NULL)
        {
            **memFail = true;
            return;
        }

        equalsList = previous->next;
        equalsList->next = NULL;
        equalsList->this = newEquals;
    }

    // first node, history->equalsList == NULL
    else
    {
        history->equalsList = malloc(sizeof(EqualsList));
        if (history->equalsList == NULL)
        {
            **memFail = true;
            return;
        }

        equalsList = history->equalsList;
        equalsList->next = NULL;
        equalsList->this = newEquals;
    }
}

static Equals *makeNewEquals(Tree *historyA, Tree *historyB, bool **memFail)
{
    Equals *newEquals = malloc(sizeof(Equals));

    if (newEquals == NULL)
    {
        **memFail = true;
        return NULL;
    }

    else
    {
        newEquals->historyA = historyA;
        newEquals->historyB = historyB;
        return newEquals;
    }
}

static Tree *getHistory(char *argument, Tree *histories, bool **error)
{
    unsigned length = strlen(argument);
    for (unsigned i = 0; i < length; ++i)
    {
        if (histories->next[charToIndex(argument[i])] == NULL)
        {
            **error = true;
            return NULL;
        }
        else
        {
            histories = histories->next[charToIndex(argument[i])];
        }
    }

    return histories;
}