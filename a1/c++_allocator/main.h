/******************************************************************************
 * Assignment 1 Solution provided by Paul Miller as part of COSC1112/1114 
 * - Operating Systems Principles, Semester 2, 2019. 
 *****************************************************************************/

#include "alloc.h"
#include "chunk_allocator.h"
#include "memory_chunk.h"
#include <cassert>
#include <cctype>
#include <cfloat>
#include <climits>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <sys/resource.h>
#include <time.h>
#include <unistd.h>

#define TOTAL_NAMES 4945
#define NAME_LEN 15
const int DECIMAL = 10;
#define EXTRA_CHARS 2
const int MAXITERS = 5000;

constexpr const char* NAMES_FILE = "first-names.txt";
using names_list = char[TOTAL_NAMES][NAME_LEN + 1];
using chunk_allocator = osp::memory::chunk_allocator<char>;
using string = std::basic_string<char, std::char_traits<char>, chunk_allocator>;

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
using range = std::pair<long, long>;

/**
 * the data we get back from a call to process_args() when it succeeds.
 * array_size is the number of array elements to allocate on each loop, and
 * thestrategy is the allocation strategy - one of worstfit, bestfit or firstfit
 **/
struct args_result
{
    std::size_t array_size;
    enum strategy thestrategy;
};

/* default allowed array size range - between 0 and the largest int */
constexpr range array_size_range = { 0, INT_MAX };

/**
 * accepted arguments in this program are s for the array sizes and a for the
 * allocation approach.
 **/
enum arg_option
{
    AO_SIZE = 's',
    AO_APPROACH = 'a'
};
