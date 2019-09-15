/******************************************************************************
 * Assignment 1 Solution provided by Paul Miller as part of COSC1112/1114 
 * - Operating Systems Principles, Semester 2, 2019. 
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#ifndef ALLOC_H
#define ALLOC_H

/* three types of memory allocation strategy */

const int NUM_STRATEGIES = 3;

/* the allowed memory allocation strategies */
enum strategy
{
    FIRSTFIT,
    BESTFIT,
    WORSTFIT,
    INVALID_STRATEGY = EOF
};

/* size_pair - used for reporting the size of the allocation and the free list
 */
struct size_pair
{
    size_t alloc, free;
    size_pair(void) : alloc(0), free(0) {}
    size_pair(std::size_t thealloc, std::size_t thefree)
        : alloc(thealloc), free(thefree)
    {
    }
};
/* starting break - this is the value of the program break (heap frontier or
 * heap edge at the start of the program so we can reset it on a normal exit */
extern void* starting_break;
/* global functions defined in alloc.cpp - look for the details there */
void set_strategy(strategy);
void* alloc(size_t);
void
dealloc(void*);
struct size_pair
get_list_sizes();
#endif
