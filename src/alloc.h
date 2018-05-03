//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2018 James D. Mitchell and Florent Hivert
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#ifndef LIBSEMIGROUPS_SRC_ALLOC_H_
#define LIBSEMIGROUPS_SRC_ALLOC_H_

#include <vector>

#include "libsemigroups-debug.h"

namespace libsemigroups {

template <class T>
class allocator
{
public:
    using value_type    = T;

//     using pointer       = value_type*;
//     using const_pointer = typename std::pointer_traits<pointer>::template
//                                                     rebind<value_type const>;
//     using void_pointer       = typename std::pointer_traits<pointer>::template
//                                                           rebind<void>;
//     using const_void_pointer = typename std::pointer_traits<pointer>::template
//                                                           rebind<const void>;

//     using difference_type = typename std::pointer_traits<pointer>::difference_type;
//     using size_type       = std::make_unsigned_t<difference_type>;

//     template <class U> struct rebind {typedef allocator<U> other;};

  private:
    std::vector<value_type *> pool;
    size_t chunk_size;
    size_t next_free_chunk;

  public:
    allocator() noexcept : pool(), chunk_size(0), next_free_chunk(0) {}

    template <class U> allocator(allocator<U> const&) noexcept :
    pool(), chunk_size(0), next_free_chunk(0) { LIBSEMIGROUPS_ASSERT(false); }

    size_t new_chunk_size(size_t sz) {
        return (sz == 0) ? 16 : sz*2;
    }

    value_type* allocate(std::size_t) {
      // LIBSEMIGROUPS_ASSERT(n == 1);  // only allocate element on by one
      if (next_free_chunk == chunk_size) {   // allocate a new chunk
        chunk_size = new_chunk_size(chunk_size);
        pool.push_back(
          static_cast<value_type*>(::operator new (chunk_size*sizeof(value_type))));
        next_free_chunk = 0;
      }
      value_type* res = pool.back() + next_free_chunk;
      next_free_chunk++;
      return res;
    }

    void deallocate(value_type*, std::size_t) noexcept {
      LIBSEMIGROUPS_ASSERT(false);  // only allocate element on by one
    }

    void deallocate_all() {
      if (not pool.empty()) {
        chunk_size = 0;
        size_t i;
        for (i = 0; i < pool.size() - 1; i++) {
          chunk_size = new_chunk_size(chunk_size);
          for (size_t j = 0; j < chunk_size; j++) {
            pool[i][j].~value_type();
          }
          ::operator delete[] (pool[i]);
        }
        LIBSEMIGROUPS_ASSERT(next_free_chunk < new_chunk_size(chunk_size));
        for (size_t j = 0; j < next_free_chunk; j++) {
          pool[i][j].~value_type();
        }
        ::operator delete[] (pool[i]);
      }
      pool.clear();
      chunk_size = 0;
      next_free_chunk = 0;
    }

//     value_type*
//     allocate(std::size_t n, const_void_pointer)
//     {
//         return allocate(n);
//     }

//     template <class U, class ...Args>
//     void
//     construct(U* p, Args&& ...args)
//     {
//         ::new(p) U(std::forward<Args>(args)...);
//     }

//     template <class U>
//     void
//     destroy(U* p) noexcept
//     {
//         p->~U();
//     }

//     std::size_t
//     max_size() const noexcept
//     {
//         return std::numeric_limits<size_type>::max();
//     }

//     allocator
//     select_on_container_copy_construction() const
//     {
//         return *this;
//     }

//     using propagate_on_container_copy_assignment = std::false_type;
//     using propagate_on_container_move_assignment = std::false_type;
//     using propagate_on_container_swap            = std::false_type;
//     using is_always_equal                        = std::is_empty<allocator>;
};

template <class T, class U>
bool
operator==(allocator<T> const&, allocator<U> const&) noexcept
{
    return true;
}

template <class T, class U>
bool
operator!=(allocator<T> const& x, allocator<U> const& y) noexcept
{
    return !(x == y);
}

}

#endif  // LIBSEMIGROUPS_SRC_ALLOC_H_
