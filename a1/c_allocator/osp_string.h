/******************************************************************************
* Assignment 1 Solution provided by Paul Miller as part of COSC1112/1114 
* - Operating Systems Principles, Semester 2, 2019. 
******************************************************************************/

#include "alloc.h"
#include <stdio.h>
#include <string.h>
#ifndef OSPSTRING_H
#define OSPSTRING_H

#define MINSTRINGCAPACITY 16

/**
 * definition of the new string type
 **/
struct string
{
    /* the char data */
    char* c_str;
    /* total space allocated */
    size_t capacity;
    /* current length of the string not counting the nul terminator */
    size_t length;
};

struct string make_string(const char*);

struct string make_empty_string(void);

struct string make_string_size(size_t);

struct string get_string(FILE*);

void free_string(struct string*);

#endif
