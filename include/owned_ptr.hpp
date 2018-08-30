
//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2018 James D. Mitchell
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

// This file contains a base class for congruence-like classes.

#ifndef LIBSEMIGROUPS_INCLUDE_OWNED_PTR_HPP_
#define LIBSEMIGROUPS_INCLUDE_OWNED_PTR_HPP_

#include <cstddef>  // for std::nullptr_t

namespace libsemigroups {
  namespace internal {
    template <class TPtrType>
    class owned_ptr {
     public:
      explicit owned_ptr(TPtrType* ptr, void* owner = nullptr)
          : _ptr(ptr), _owner(owner) {}

      owned_ptr(owned_ptr const&) = default;
      owned_ptr& operator=(owned_ptr const&) = default;
      owned_ptr(owned_ptr&&)                 = default;
      owned_ptr& operator=(owned_ptr&&) = default;
      ~owned_ptr()                      = default;

      owned_ptr& operator=(std::nullptr_t const&) {
        _ptr   = nullptr;
        _owner = nullptr;
        return *this;
      }

      TPtrType& operator*() {
        return *_ptr;
      }

      void free_from(void* obj) {
        if (obj == _owner) {
          delete _ptr;
          _ptr = nullptr;
        }
      }

      bool operator!=(std::nullptr_t const&) {
        return _ptr != nullptr;
      }

      bool operator==(std::nullptr_t const&) {
        return _ptr == nullptr;
      }

     private:
      TPtrType* _ptr;
      void*     _owner;
    };
  }  // namespace internal
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_INCLUDE_OWNED_PTR_HPP_
