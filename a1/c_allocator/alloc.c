/******************************************************************************
 * Assignment 1 Solution provided by Paul Miller as part of COSC1112/1114
 * - Operating Systems Principles, Semester 2, 2019.
 ******************************************************************************/

#include "alloc.h"
#include "memory_chunk.h"
/**
 * the strategy used for allocation
 **/
enum strategy thestrategy;

/**
 * the list of allocated blocks
 **/
struct list alloc_list;

/**
 * the list of free blocks
 **/
struct list free_list;
/**
 * function pointer type that points to one of the three allocatioin approaches
 **/
typedef void* (*alloc_approach)(size_t);

/**
 * actual function pointer variable
 **/
alloc_approach theapproach;

/**
 * memory pair returned from splitting
 **/
struct memory_pair
{
    struct memory_chunk *first, *second;
};

/**
 * function that splits a chunk and returns the split pair
 **/
struct memory_pair
split(struct memory_chunk* mem, size_t sizefirst)
{
    struct memory_pair thepair;
    /* store the pointers to the memory chunks */
    thepair.first = mem;
    thepair.second = chunk_from_split(mem, sizefirst);
    /* reduce the size of the first chunk by the size of the second chunk */
    return thepair;
}

/**
 * function that does the general allocation of memory
 **/
void*
alloc(size_t size)
{
    /**
     * always allocate new memory if the list of free blocks is empty
     **/
    if (free_list.size == 0)
    {
        /**
         * grab the new chunk
         **/
        struct memory_chunk* newchunk = new_memory_chunk(size);
        if (newchunk == NULL)
        {
            fprintf(stderr, "Error: unable to allocate memory\n");
            return NULL;
        }
        /* add to the allocatoins list */
        if (!list_push_back(&alloc_list, newchunk))
        {
            fprintf(stderr, "Error: cannot add to the alloc list.\n");
            return NULL;
        }
        /* return the memory allocated */
        return newchunk->memory;
    }
    /* if the free list is not empty, use the allocation approach to allocate
     * the memory */
    return theapproach(size);
}

/**
 * general memory comparison function - are the two memory addresses in question
 *the same ?
 **/
int
mymemcmp(const void* chunk, const void* mem)
{
    const struct memory_chunk* mchunk = chunk;
    return ((ssize_t)mchunk->memory) - ((ssize_t)mem);
}

/**
 * general deallocation algorithm - search for the chunk requested and if we
 * can't find it, check if it's a double - free (look in the free list). In
 * either of these cased abort. Otherwise, remove the chunk from the allocations
 * list and add it to the free list.
 **/
void
dealloc(void* mem)
{
    /**
     * find the chunk
     **/
    struct memory_chunk* foundchunk =
        list_pop_find_cmp(&alloc_list, mem, mymemcmp);
    if (!foundchunk)
    {
        /* if not found, there is an attempt to free memory we don't have */
        /* check for a double free */
        foundchunk = list_pop_find_cmp(&free_list, mem, mymemcmp);
        if (!foundchunk)
        {
            fprintf(stderr, "Error: memory not allocated by us.\n");
        }
        else
        {
            fprintf(stderr, "Error: double free of memory detected.\n");
        }
#ifdef DEBUG
        /**
         * if we are in debug mode, print the allocations and free list so we
         *can debug any problems.
         **/
        fprintf(stderr, "memory to free: %p\n", mem);
        fprintf(stderr, "alloc_list: \n");
        list_print(&alloc_list, memory_chunk_print);
        fprintf(stderr, "free list: \n");
        list_print(&free_list, memory_chunk_print);
#endif
        /* abort() as there was an attempt for an invaid free */
        abort();
    }
    /* move the freed block to the free list */
    if (!list_push_back(&free_list, foundchunk))
    {
        fprintf(stderr, "Error: could not add to the free list.\n");
    }
}

/**
 * comparison function for first fit - we want the first chunk that matches
 **/
int
first_fit_cmp(const void* chunk, const void* sizeptr)
{
    const struct memory_chunk* thechunk = chunk;
    const size_t* size = sizeptr;
    return ((int)thechunk->size) - (int)(*size) >= 0 ? 0 : EOF;
}

/**
 * implementation of the first fit algorithm
 **/
void*
first_fit(size_t size)
{
    /* find the first chunk that meets the allocation request */
    struct memory_chunk* foundchunk =
        list_pop_find_cmp(&free_list, &size, first_fit_cmp);
    /* if we can't find a chunk, that meets the request, allocate new memory */
    if (!foundchunk)
    {
        struct memory_chunk* newchunk = new_memory_chunk(size);
        list_push_back(&alloc_list, newchunk);
        return newchunk->memory;
    }
    /* it the chunk is bigger than what we need, split it*/
    if (foundchunk->size > size)
    {
        struct memory_pair splitpair = split(foundchunk, size);
        list_push_back(&alloc_list, splitpair.first);
        list_push_back(&free_list, splitpair.second);
        return splitpair.first->memory;
    }
    else
    {
        /* if it matches, just insert it */
        list_push_back(&alloc_list, foundchunk);
    }
    return foundchunk->memory;
}

/**
 * implementation of the worst fit algorithm
 **/
void*
worst_fit(size_t size)
{
    /* find the largest chunk */
    struct memory_chunk* foundchunk = list_pop_max(&free_list);
    if (!foundchunk || foundchunk->size < size)
    {
        /* when the largest chunk is not big enough or the list is empty,
         * allocate a new chunk */
        struct memory_chunk* newchunk = new_memory_chunk(size);
        if (!newchunk)
        {
            fprintf(stderr, "Error: memory allocation failure.\n");
            abort();
        }
        /* if we found a chunk but it's too small, readd it to the free list */
        if (foundchunk)
        {
            if (!list_push_back(&free_list, foundchunk))
            {
                fprintf(stderr, "Error: could not readd to free list.\n");
                abort();
            }
        }
        /* add the new chunk to the allocations list */
        if (!list_push_back(&alloc_list, newchunk))
        {
            fprintf(stderr, "Error: could not add to the alloc list.\n");
            abort();
        }
        /* return the new memory block back to the requestor */
        return newchunk->memory;
    }
    /* if the new chunk is bigger than what we need, split it */
    if (foundchunk->size > size)
    {
        struct memory_pair splitpair = split(foundchunk, size);
        list_push_back(&alloc_list, splitpair.first);
        list_push_back(&free_list, splitpair.second);
        return splitpair.first->memory;
    }
    list_push_back(&alloc_list, foundchunk);
    return foundchunk->memory;
}

/**
 * helper function that decided whether the chunk we are currently looking at
 * is the best we have seen so far
 **/
void
setbestfit(void* chunk, void** best, const void* desired)
{
    /* we need to assign each pointer to the appropriate pointer type so we can
     * get at the data stored therein */
    struct memory_chunk* thechunk = chunk;
    struct memory_chunk* bestchunk = *best;
    size_t desired_size = *(size_t*)desired;

    /* if we haven't set a best chunk yet, this is the best chunk we have seend
     * so far */
    if (!bestchunk)
    {
        *best = thechunk;
        return;
    }
    /* otherwise, if this chunk is smaller than the best so far but greater than
     * or equal to the desired chunk size, this is the best chunk */
    if (thechunk->size >= desired_size && thechunk->size < bestchunk->size)
    {
        *best = thechunk;
    }
}

/**
 * implementation of the best fir algorithm
 **/
void*
best_fit(size_t size)
{
    struct memory_chunk* bestchunk = NULL;
    void* best = bestchunk;
    /**
     * iterate over the list to find the best chunk
     **/
    list_for_each(&alloc_list, &size, &best, setbestfit);
    /* if we can find a matching chunk, remove it from the free list */
    if (bestchunk)
    {
        list_remove(&free_list, bestchunk, chunk_cmp);
        /* if the chunk is larger than we need, we should split it */
        if (bestchunk->size > size)
        {
            struct memory_pair splitpair = split(bestchunk, size);
            list_push_back(&alloc_list, splitpair.first);
            list_push_back(&free_list, splitpair.second);
            return splitpair.first->memory;
        }
        /* add to the allocations list */
        list_push_back(&alloc_list, bestchunk);
        return bestchunk->memory;
    }
    else
    {
        /* if we cannot find a matching chunk, allocate some new space */
        struct memory_chunk* newchunk = new_memory_chunk(size);
        list_push_back(&alloc_list, newchunk);
        return newchunk->memory;
    }
}

/**
 * sets the strategy and does initial setup of the allocator
 **/
void
set_strategy(enum strategy astrategy)
{
    thestrategy = astrategy;
    /* initialise the lists used for memory management */
    list_init(&alloc_list, chunk_cmp, memory_chunk_destroy);
    list_init(&free_list, chunk_cmp, memory_chunk_destroy);
    /* set the fitness function */
    switch (thestrategy)
    {
        case ST_FIRSTFIT:
            theapproach = first_fit;
            break;
        case ST_BESTFIT:
            theapproach = best_fit;
            break;
        case ST_WORSTFIT:
            theapproach = worst_fit;
            break;
        default:
            fprintf(stderr, "Error: invalid strategy specified.\n");
            abort();
    }
}

/**
 * returns the list sizes back to main or whereever it's needed.
 **/
struct size_pair
get_list_sizes(void)
{
    struct size_pair thesizes;
    thesizes.alloc = alloc_list.size;
    thesizes.free = free_list.size;
    return thesizes;
}

void
print_alloc_list(void)
{
    list_print(&alloc_list, memory_chunk_print);
}
void
print_free_list(void);
