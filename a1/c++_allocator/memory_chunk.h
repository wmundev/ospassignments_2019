/******************************************************************************
 * Assignment 1 Solution provided by Paul Miller as part of COSC1112/1114 
 * - Operating Systems Principles, Semester 2, 2019. 
 *****************************************************************************/

#include <iostream>

#include "alloc.h"

#ifndef MEMORY_CHUNK_H
#define MEMORY_CHUNK_H
namespace osp::memory
{
    /**
     * the memory_chunk type represents each memory allocation in the system
     **/
    struct memory_chunk
    {
        std::size_t size;
        void* memory;

      public:
        static void* operator new(std::size_t);
        static void operator delete(void*);
        memory_chunk(void);
        /* disallow copying of memory chunks */
        memory_chunk(const memory_chunk&) = delete;
        /* construction by move */
        memory_chunk(memory_chunk&& oldchunk)
            : size(std::move(oldchunk.size)), memory(std::move(oldchunk.memory))
        {
        }

        /* move assignment operator */
        memory_chunk& operator=(memory_chunk&& achunk)
        {
            size = std::move(achunk.size);
            memory = std::move(achunk.memory);
            return *this;
        }
        memory_chunk(std::size_t, void*);
        memory_chunk(std::size_t);
        bool operator==(const memory_chunk&) const;
        bool operator!=(const memory_chunk&) const;
        operator bool(void) const;
        ~memory_chunk(void)
        {
#ifdef DEBUG
            std::cerr << "deallocating chunk " << memory << "\n";
#endif
            dealloc(memory);
        }
    };
} // namespace osp::memory
#endif
