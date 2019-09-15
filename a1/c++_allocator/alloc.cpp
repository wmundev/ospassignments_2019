/******************************************************************************
 * Assignment 1 Solution provided by Paul Miller as part of COSC1112/1114 
 * - Operating Systems Principles, Semester 2, 2019. 
 *****************************************************************************/

#include "alloc.h"
#include "chunk_allocator.h"
#include "memory_chunk.h"
#include "memory_list.h"
#include <algorithm>
#include <cassert>
#include <utility>

/* stores the value of the heap frontier (program break) at the start of the
 * program
 */
void* starting_break;
/* 'typedefs' used to shorten the allocator type code a bit */
using memory_chunk = osp::memory::memory_chunk;
using chunk_allocator = osp::memory::chunk_allocator<memory_chunk*>;

/* pointers to our two linked lists. I've used pointers here as I can more
 * easily set their lifetime so I can do a global free of memory before I
 * exit the program.
 */
static osp::memory::memory_list<chunk_allocator>* alloc_list;
static osp::memory::memory_list<chunk_allocator>* free_list;

/* the strategy of memory allocation used - firstfit, bestfit or worstfit.
 * Please note that 'static' in this context (for global variables) means that
 * these variales are not globally visible but only in this module. As we are
 * outside a struct or a class, the concept of private has no meaning. */
static strategy thestrategy;
/**
 * pair of memory chunks that are the result of splitting a memory chunk
 **/

using memory_pair = std::pair<memory_chunk*, memory_chunk*>;

/**
 * returns a pair os sizes being the size of the alloc_list and the
 * size of the free_list
 **/
size_pair get_list_sizes()
{
    return size_pair(alloc_list->size(), free_list->size());
}

/**
 * splits a memory chunk into two based on the desired size
 **/
static memory_pair split(memory_chunk* orig_chunk, std::size_t size_first)
{
    auto first = orig_chunk;
    auto second = new memory_chunk(
        orig_chunk->size - size_first,
        /* cast to a char pointer as chars occupy one byte */
        (reinterpret_cast<char*>(orig_chunk->memory)) + size_first);
    first->size = size_first;
    auto thepair = memory_pair(first, second);
    return thepair;
}

/**
 * abstract functor that defines the three methods for deciding how to search
 * the free list
 **/
struct alloc_approach
{
    virtual void* operator()(std::size_t) const = 0;
};

/**
 * strategy for allocating based on the first node we find that is big enough
 **/
struct first_fit : public alloc_approach
{
    virtual void* operator()(std::size_t size) const override
    {
        /* find the first node with enough space */
        auto results = std::find_if(free_list->begin(), free_list->end(),
                                    [size](memory_chunk * achunk)->bool
        { return achunk->size >= size; });
        /* if we could not find a node with enough space, we need to allocate
         * new space
         */
        if (results == free_list->end())
        {
            auto chunk = new memory_chunk(size);
            /* add the new chunk to the allocated list */
            alloc_list->push_back(chunk);
            return alloc_list->back()->memory;
        }
        /* grab the first result from the matching results */
        auto chunk = *results;
        if (chunk->size > size)
        {
            /* if the size we need is smaller than the first chunk we find, we
             * split it */
            auto memory_pair = split(chunk, size);
            free_list->remove(memory_pair.first);
            alloc_list->push_back(memory_pair.first);
            free_list->push_back(memory_pair.second);
        }
        else
        {
            /* remove it from the free list and add it to the allocations list
             * if the allocation size is a perfect match for our needs */
            free_list->erase(results);
            alloc_list->push_back(chunk);
        }
        return alloc_list->back()->memory;
    }
};

/**
 * function that implements the best fit memory allocation strategy
 **/
struct best_fit : public alloc_approach
{
    virtual void* operator()(std::size_t size) const override
    {
        memory_chunk* best_chunk = nullptr;

        /* search for a block that is the best match for our size requirements
         */
        std::for_each(free_list->begin(), free_list->end(),
                      [size, &best_chunk](memory_chunk* achunk)
        {
            /* search for chunks that are greater than or equal to
             * our size requirements */
            if (achunk->size >= size)
            {
                /* if we have a previous best chunk */
                if (best_chunk)
                {
                    /* is this chunk smaller than the last one we
                     * found ? */
                    if (achunk->size < best_chunk->size)
                    {
                        /* if so we store it as our new best
                         * chunk*/
                        best_chunk = achunk;
                    }
                }
                else
                {
                    /* this is the first chunk we have come across
                     * that matches our size requirements */
                    best_chunk = achunk;
                }
            }
        });
        /* if we could not find a matching chunk, allocate new memory and add it
         * to our allocations list */
        if (!best_chunk)
        {
            auto newchunk = new memory_chunk(size);
            alloc_list->push_back(newchunk);
            return newchunk->memory;
        }
        /* remove the best chunk that we found from the free list */
        auto newchunk = best_chunk;
        free_list->remove(best_chunk);
        /* if the chunk we found is bigger than our required size, split it and
         * reinsert the leftover chunk onto the free list */
        if (newchunk->size > size)
        {
            auto memory_pair = split(newchunk, size);
            alloc_list->push_back(memory_pair.first);
            free_list->push_back(std::move(memory_pair.second));
        }
        else
        {
            /* if the chunk was a perfect match for our needs, just insert it
             * into the allocated list */
            alloc_list->push_back(newchunk);
        }
        /* in either case, return the memory block of the size that was
         * requested */
        return alloc_list->back()->memory;
    }
};

/**
 * implementation of the worst fit memory allocation strategy
 **/
struct worst_fit : public alloc_approach
{
    virtual void* operator()(std::size_t size) const override
    {
        memory_chunk* worst_chunk = nullptr;

        /* find the largest chunk in the list free list */
        std::for_each(free_list->begin(), free_list->end(),
                      [size, &worst_chunk](memory_chunk* newchunk)
        {
            if (newchunk->size >= size)
            {
                if (worst_chunk)
                {
                    if (worst_chunk->size < size)
                    {
                        worst_chunk = newchunk;
                    }
                }
                else
                {
                    worst_chunk = newchunk;
                }
            }
        });
        /* if we did not find a chunk big enough, allocate a new chunk */
        if (!worst_chunk)
        {
            auto newchunk = new memory_chunk(size);
            alloc_list->push_back(newchunk);
            return newchunk->memory;
        }
        else
        {
            /* otherwise, check the chunk we got back. If it is larger than what
             * we need, split it, pushing the first part onto our allocated list
             * and the second part back onto the free list */
            if (worst_chunk->size > size)
            {
                auto memory_pair = split(worst_chunk, size);
                alloc_list->push_back(memory_pair.first);
                free_list->remove(memory_pair.first);
                free_list->push_back(std::move(memory_pair.second));
            }
            else
            {
                /* otherwise push the whole block onto the allocated list as it
                 * is a perfect fit for our needs */
                alloc_list->push_back(worst_chunk);
                free_list->remove(worst_chunk);
            }
        }
        return alloc_list->back()->memory;
    }
};

void* alloc(std::size_t size)
{
    /**
     * if the free list is not empty, try to find the best matching chunk
     **/
    if (!free_list->empty())
    {
        constexpr first_fit thefirstfit;
        constexpr best_fit thebestfit;
        constexpr worst_fit theworstfit;
        switch (thestrategy)
        {
            case FIRSTFIT:
            {
                return thefirstfit(size);
            }
            break;
            case BESTFIT:
            {
                return thebestfit(size);
            }
            break;
            case WORSTFIT:
            {
                return theworstfit(size);
            }
            break;
            default:
                throw std::invalid_argument(
                    "strategy not set to a valid value");
        }
    }
    else
    {
        /* otherwise we ask the os for memory */
        auto chunk = new memory_chunk(size);
        alloc_list->push_back(chunk);
        return alloc_list->back()->memory;
    }
    /* dead code */
    return nullptr;
}

/**
 * find the memory specified, and if we can't find it, abort as it's an attempt
 * to free memory we didn't allocate
 **/
void dealloc(void* memory)
{
    /* try to find the chunk that this memory belongs to */
    auto alloc_iter = std::find_if(alloc_list->begin(), alloc_list->end(),
                                   [memory](const memory_chunk * achunk)->bool
    { return achunk->memory == memory; });
    /* if we got to the end of the list, the chunk could not be found */
    if (alloc_iter == alloc_list->end())
    {
        /* search the free list for the memory block. If we find it, it is
         * a double free
         */
        auto free_iter =
            std::find_if(free_list->begin(), free_list->end(),
                         [memory](const memory_chunk * achunk)->bool
        { return achunk->memory == memory; });
        if (free_iter != free_list->end())
        {
            std::cerr << "Error: mismatching free.\n";
            abort();
        }
        std::cerr << "Error: freeing memory that was never allocated.\n";
        abort();
    }
    assert((*alloc_iter)->memory == memory);
    auto chunk = *alloc_iter;
    alloc_list->erase(alloc_iter);
    free_list->push_back(chunk);
}

/**
 * on a normal exit, we want to delete reset the memory so we are not using any
 **/
void reset_heap(void)
{
    delete alloc_list;
    delete free_list;
    brk(starting_break);
}

/**
 * this function initializes the allocator
 **/
void set_strategy(strategy astrategy)
{
    thestrategy = astrategy;
    starting_break = sbrk(0);
    alloc_list = new osp::memory::memory_list<chunk_allocator>;
    free_list = new osp::memory::memory_list<chunk_allocator>;
    atexit(reset_heap);
}
