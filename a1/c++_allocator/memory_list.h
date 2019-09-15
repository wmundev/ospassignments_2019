/******************************************************************************
 * Assignment 1 Solution provided by Paul Miller as part of COSC1112/1114 
 * - Operating Systems Principles, Semester 2, 2019. 
 *****************************************************************************/

#include <list>
#include <memory>

#include "memory_chunk.h"
/* definition of the list type used for memory allocation which inherits from
 * std::list and only exposes the functions necessary*/
namespace osp::memory
{
    using value_type = memory_chunk*;
    template <typename ALLOCATOR = std::allocator<value_type> >
    class memory_list : std::list<value_type, ALLOCATOR>
    {
      public:
        using std::list<value_type, ALLOCATOR>::list;
        using std::list<value_type, ALLOCATOR>::empty;
        using std::list<value_type, ALLOCATOR>::begin;
        using std::list<value_type, ALLOCATOR>::end;
        using std::list<value_type, ALLOCATOR>::remove;
        using std::list<value_type, ALLOCATOR>::emplace_back;
        using std::list<value_type, ALLOCATOR>::back;
        using std::list<value_type, ALLOCATOR>::push_back;
        using std::list<value_type, ALLOCATOR>::erase;
        using std::list<value_type, ALLOCATOR>::size;
    };
} // namespace osp::memory
