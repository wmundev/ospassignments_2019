/******************************************************************************
 * Assignment 1 Solution provided by Paul Miller as part of COSC1112/1114
 * - Operating Systems Principles, Semester 2, 2019.
 ******************************************************************************/

#include "list.h"
#include <stdio.h>
#include <unistd.h>
#ifndef ALLOC_H
#define ALLOC_H
void* alloc(size_t);
void dealloc(void*);

enum strategy
{
        ST_FIRSTFIT,
        ST_BESTFIT,
        ST_WORSTFIT,
        ST_INVALID
};

struct size_pair
{
        size_t alloc, free;
};

struct size_pair get_list_sizes(void);

void set_strategy(enum strategy);

void print_alloc_list(void);
void print_free_list(void);
#endif
