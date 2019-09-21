/******************************************************************************
* Assignment 1 Solution provided by Paul Miller as part of COSC1112/1114
* - Operating Systems Principles, Semester 2, 2019.
******************************************************************************/

#include "alloc.h"
#include "memory_chunk.h"
#include "osp_string.h"
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <float.h>
#include <limits.h>
#include <stdbool.h>
#include <string.h>
#include <sys/resource.h>
#include <time.h>
#include <unistd.h>

#ifndef MAIN_H
#define MAIN_H
#define TOTAL_NAMES 4945
#define NAME_LEN 15
#define DECIMAL 10
#define EXTRA_CHARS 2
#define MAXITERS 5000

#define NAMES_FILE "first-names.txt"
typedef char names_list[TOTAL_NAMES][NAME_LEN + 1];

/**
 * a falsible number - it allows us to return a 'fail' result from a
 *function meant to extract a whole number from a string
 **/
struct falsible_long
{
        bool success;
        long data;
};

/**
 * an allowed range for a number that is to be extracted from a string
 **/
typedef struct
{
        long first, second;
} range;
/**
 * the data we get back from a call to process_args() when it succeeds.
 * array_size is the number of array elements to allocate on each loop, and
 * thestrategy is the allocation strategy - one of worstfit, bestfit or firstfit
 **/
struct args_result
{
        size_t array_size;
        enum strategy thestrategy;
};

/* default allowed array size range - between 0 and the largest int */

/**
 * accepted arguments in this program are s for the array sizes and a for the
 * allocation approach.
 **/
enum arg_option
{
        AO_SIZE = 's',
        AO_APPROACH = 'a'
};
#endif
