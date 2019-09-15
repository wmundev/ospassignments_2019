/******************************************************************************
 * Assignment 1 Solution provided by Paul Miller as part of COSC1112/1114 
 * - Operating Systems Principles, Semester 2, 2019. 
 *****************************************************************************/

#include "memory_chunk.h"

/**
 * new operator for the memory chunk - allocates a chunk of the size specified
 **/
void* osp::memory::memory_chunk::operator new(std::size_t size)
{
    void* memory = sbrk(size);
    if (memory != (void*)EOF)
    {
        return memory;
    }
    throw std::bad_alloc();
}

/**
 * delete has been defined as a non-operation
 **/
void osp::memory::memory_chunk::operator delete(void* ptr)
{
}

/**
 * default constructor - creates a chunk of no size
 **/
osp::memory::memory_chunk::memory_chunk(void) : size(0), memory(nullptr)
{
}

/**
 * this constructor does not allocate new memory other than for the object
 *itself but just copies in the ones already allocated.
 **/
osp::memory::memory_chunk::memory_chunk(std::size_t wanted_size,
                                        void* newmemory)
    : size(wanted_size), memory(newmemory)
{
}

/**
 * constructor that allocates memory by calling sbrk()
 **/
osp::memory::memory_chunk::memory_chunk(std::size_t wanted_size)
    : size(wanted_size), memory(sbrk(wanted_size))
{
}

/**
 * two chunks are equal if their memory pointer is the same
 **/
bool osp::memory::memory_chunk::operator==(const memory_chunk& other_chunk)
    const
{
    return memory == other_chunk.memory;
}

/**
 * the != operator is defined in terms of the above == operator
 **/
bool osp::memory::memory_chunk::operator!=(const memory_chunk& other_chunk)
    const
{
    return !(*this == other_chunk);
}

/**
 * this allows us to use ! on a chunk the same way we can on a memory address
 **/
osp::memory::memory_chunk::operator bool(void) const
{
    return memory != nullptr;
}
