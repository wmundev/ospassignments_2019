/******************************************************************************
* Assignment 1 Solution provided by Paul Miller as part of COSC1112/1114 
* - Operating Systems Principles, Semester 2, 2019. 
******************************************************************************/

#include "osp_string.h"

/**
 * allocate a string struct based on copying the char* pointer passed in
 **/
struct string make_string(const char* stringdata)
{
    struct string newstring = make_string_size(strlen(stringdata));
    strcpy(newstring.c_str, stringdata);
    return newstring;
}

/**
 * allocate an empty string whose size is specified by the constant
 * MINSTRINGCAPACITY
 **/
struct string make_empty_string(void)
{
    struct string newstring;
    /* allocate space */
    newstring.c_str = alloc(MINSTRINGCAPACITY * sizeof(char));
    assert(newstring.c_str);
    /* set the capacity and length, ensuring the string is zero terminated */
    newstring.capacity = MINSTRINGCAPACITY;
    newstring.length = 0;
    *newstring.c_str = 0;
    return newstring;
}

long minpowerof2(long num)
{
    long result = 1;
    while (result < num)
    {
        result <<= 1;
    }
    return result;
}

/**
 * make a string which is the minimum of size bytes but in all cases ensure it
 * is a power of two.
 **/
struct string make_string_size(size_t size)
{
    struct string newstring;
    size_t capacity;
    capacity = MINSTRINGCAPACITY;
    while (capacity < size + 1)
    {
        capacity <<= 1;
    }
    newstring.capacity = capacity;
    newstring.length = size;
    newstring.c_str = alloc(size);
    assert(newstring.c_str);
    *newstring.c_str = 0;
    return newstring;
}

/**
 * resize the string to be the minimum power of two that is larger than newsize
 **/
bool resize_string(struct string* thestring, size_t newsize)
{
    size_t newcapacity = minpowerof2(newsize);
    size_t oldsize = thestring->length;
    char* olddata = thestring->c_str;
    /* allocate the space */
    thestring->c_str = alloc(newcapacity);
    if (!thestring->c_str)
    {
        return false;
    }
    /* copy the data */
    strncpy(thestring->c_str, olddata, newsize);
    if (oldsize > newsize)
    {
        thestring->c_str[newsize] = 0;
    }
    /* deallocate the old data */
    dealloc(olddata);
    return true;
}

/**
 * dealloc space for the char * data in the string struct
 **/
void free_string(struct string* astring)
{
    dealloc(astring->c_str);
}

/**
 * reads a strig from an open file
 **/
struct string get_string(FILE* infile)
{
    struct string newstring = make_empty_string();
    int ch;
    /* read a line from the file */
    while (ch = getc(infile), ch != '\n' && ch != EOF)
    {
        /* if the string's space is overrun, resize it */
        if (newstring.length + 1 == newstring.capacity)
        {
            assert(resize_string(&newstring, 2 * newstring.length));
        }
        /* store each char */
        newstring.c_str[newstring.length++] = ch;
    }
    /* zero terminate the string */
    newstring.c_str[newstring.length] = 0;
    return newstring;
}
