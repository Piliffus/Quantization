//
// Created by filip on 12.07.19.
//

#ifndef QUANTIZATION_TYPES_H
#define QUANTIZATION_TYPES_H

#include <stdint.h>
#include <inttypes.h>

/*
 * Number of possible quantum states, default is 4: "0", "1", "2" and "3"
 */
#define STATES 4

/*
 * Type used to store energy value information - unsigned 64-bit integer
 */
typedef uint64_t Energy;

/*
 * Structure used to store histories
 */
struct Tree
{
    struct EqualsList *equalsList;
    struct Tree *next[STATES];
    Energy energy;
};
typedef struct Tree Tree;

/*
 * Structure used to store Equality relation information
 */
struct Equals
{
    struct Tree *historyA;
    struct Tree *historyB;
};
typedef struct Equals Equals;

/*
 * List for storing Equals
 */
struct EqualsList
{
    struct EqualsList *next;
    struct Equals *this;
};
typedef struct EqualsList EqualsList;

#endif //QUANTIZATION_TYPES_H
