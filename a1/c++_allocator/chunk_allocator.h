/******************************************************************************
 * Assignment 1 Solution provided by Paul Miller as part of COSC1112/1114 
 * - Operating Systems Principles, Semester 2, 2019. 
 *****************************************************************************/

#include "alloc.h"
#include <memory>
#ifndef CHUNK_ALLOCATOR_H
#define CHUNK_ALLOCATOR_H
namespace osp::memory
{
    /**
     * allocator template used for allowing standard template library classes
     * to use our allocator
     **/
    template <typename T>
    struct chunk_allocator
    {
        /* typedefs required by the allocator template */
        using value_type = T;
        using pointer = value_type*;
        using const_pointer = const value_type*;
        using reference = value_type&;
        using const_reference = const value_type&;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;
        using type = T;

        /**
         * rebind struct that allows conversion from one allocator type to
         *another
         **/
        template <typename U>
        struct rebind
        {
            using other = chunk_allocator<U>;
        };

        /* take the address of a type allocated by this allocator */
        pointer address(reference r) const
        {
            return &r;
        }

        /* take the address of a type allocated by this allocator */
        const_pointer address(const_reference r)
        {
            return &r;
        }

        /* how do we allocate space ? */
        pointer allocate(size_type size, const_pointer = 0)
        {
            void* memory = sbrk(size * sizeof(value_type));
            if (memory == (void*)EOF)
            {
                throw std::bad_alloc();
            }
            return static_cast<pointer>(memory);
        }

        void deallocate(pointer memory, size_type)
        {
        }

        /**
         * what is the maximum space we can allocate?
         **/
        size_type max_size(void) const
        {
            /* -1 cast to an unsigned gives us the largest possible value */
            return static_cast<size_type>(EOF) / sizeof(value_type);
        }

        /**
         * construct using already available memory
         **/
        void construct(pointer memory, const value_type& value)
        {
            new (memory) value_type(value);
        }

        /* deallocate memory space */
        void destroy(pointer memory)
        {
            memory->~value_type();
        }

        /**
         * constructor - in this case we don't do anything
         **/
        chunk_allocator()
        {
        }

        chunk_allocator(const chunk_allocator<T>&)
        {
        }

        ~chunk_allocator(void)
        {
        }

      private:
        void operator=(const chunk_allocator<T>&);
    };

    /**
     * all allocators to the same type are considered equivalent
     **/
    template <typename T>
    inline bool operator==(const chunk_allocator<T>&, const chunk_allocator<T>&)
    {
        return true;
    }

    template <typename T>
    inline bool operator!=(const chunk_allocator<T>&, const chunk_allocator<T>&)
    {
        return false;
    }
}
#endif
