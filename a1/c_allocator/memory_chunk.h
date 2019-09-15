/******************************************************************************
* Assignment 1 Solution provided by Paul Miller as part of COSC1112/1114 
* - Operating Systems Principles, Semester 2, 2019. 
******************************************************************************/

#include <unistd.h>
#ifndef MEMORY_CHUNK_H
#define MEMORY_CHUNK_H
/**
 * the structure for memory chunk, storing the size of the memory allocated
 * along with the pointer to the memory itself
 **/
struct memory_chunk
{
    size_t size;
    void* memory;
};

struct memory_chunk* new_memory_chunk(size_t size);
int chunk_cmp(const void*, const void*);
void memory_chunk_destroy(void*);
int memory_chunk_print(const void*);
struct memory_chunk* chunk_from_split(struct memory_chunk*, size_t);
#endif
