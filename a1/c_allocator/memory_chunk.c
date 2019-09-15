/******************************************************************************
* Assignment 1 Solution provided by Paul Miller as part of COSC1112/1114 
* - Operating Systems Principles, Semester 2, 2019. 
******************************************************************************/

#include "memory_chunk.h"
#include "alloc.h"
/**
 * allocates a new memory chunk which includes both the memory pointer and the
 * size of the chunk
 **/
struct memory_chunk* new_memory_chunk(size_t size)
{
    struct memory_chunk* new_chunk;
    /* store the heap frontier in case we have to undo the allocation */
    void* start = sbrk(0);
    /* allocate the space for the chunk */
    new_chunk = sbrk(sizeof(struct memory_chunk));
    if (new_chunk == (void*)EOF)
    {
        return NULL;
    }
    /* allocate the space for the memory itself */
    new_chunk->memory = sbrk(size);
    if (new_chunk->memory == (void*)EOF)
    {
        /* rewind the memory allocation done in this function */
        brk(start);
        return NULL;
    }
    new_chunk->size = size;
    return new_chunk;
}

/**
 * comparison between chunks based on their sizes
 **/
int chunk_cmp(const void* first, const void* second)
{
    /* store in memory_chunk pointers as we can't dereference a void pointer */
    const struct memory_chunk* mfirst = first;
    const struct memory_chunk* msecond = second;
    return ((int)mfirst->size) - ((int)msecond->size);
}

void memory_chunk_destroy(void* chunk)
{
    /* do nothing as we won't deallocate chunks */
}

/**
 * prints out the data for memory chunk - this is for use with the list_print
 * function
 **/
int memory_chunk_print(const void* ptr)
{
    const struct memory_chunk* thechunk = ptr;
    return printf(
        "the chunk is %ld bytes and the address of the memory is %p\n",
        thechunk->size, thechunk->memory);
}

/**
 * split the chunk passed in so the chunk returned contains whatever is left
 * if sizefirst is stored in the first chunk
 **/
struct memory_chunk* chunk_from_split(struct memory_chunk* oldchunk,
                                      size_t sizefirst)
{
    assert(oldchunk->size > sizefirst);
    /* allocate space for the node for the new chunk */
    struct memory_chunk* newchunk = sbrk(sizeof(struct memory_chunk));
    if (newchunk == (void*)EOF)
    {
        perror("sbrk");
        return NULL;
    }
    /* set the size for the new chunk and grab the memory for the new chunk*/
    newchunk->size = oldchunk->size - sizefirst;
    newchunk->memory = ((char*)oldchunk->memory) + sizefirst;
    /* store the new size for the old chunk */
    oldchunk->size = sizefirst;
    return newchunk;
}
