//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2020 James D. Mitchell
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

// This file contains an implementation of a pool, which provide an easy
// way to manage temporary elements while avoiding unnecessary memory
// allocation.

#ifndef LIBSEMIGROUPS_POOL_HPP_
#define LIBSEMIGROUPS_POOL_HPP_

#include <list>           // for list
#include <stack>          // for stack
#include <type_traits>    // for is_pointer
#include <unordered_map>  // for unordered_map

#include "libsemigroups/exception.hpp"  // for LIBSEMIGROUPS_EXCEPTION

namespace libsemigroups {
  namespace detail {

    // Declarations
    template <typename T, typename = void>
    class Pool;

    template <typename T, typename = void>
    class PoolGuard;

    // Helpers
    template <typename T>
    using is_pointer_t = std::enable_if_t<std::is_pointer<T>::value>;

    template <typename T>
    using is_non_pointer_t = std::enable_if_t<!std::is_pointer<T>::value>;

    // Pool for non-pointer types, these are intended to be used with the
    // T = BruidhinnTraits::internal_value_type, and so the non-pointer objects
    // should be small and easy to copy. There are only versions for pointer
    // and non-pointer types as BruidhinnTraits::internal_value_type is always
    // one of these two. It's your responsibility to ensure that the
    // non-pointer type is actually small and easy to copy.
    template <typename T>
    class Pool<T, is_non_pointer_t<T>> final {
      static_assert(std::is_default_constructible<T>::value,
                    "Pool<T> requires T to be default-constructible");

     public:
      Pool()  = default;
      ~Pool() = default;

      // Deleted other constructors to avoid unintentional copying
      Pool(Pool const&) = delete;
      Pool(Pool&&)      = delete;
      Pool& operator=(Pool const&) = delete;
      Pool& operator=(Pool&&) = delete;

      T acquire() {
        return T();
      }

      void release(T&) {}

      void init(T const&) {}
    };

    template <typename T>
    class Pool<T, is_pointer_t<T>> final {
     public:
      // Not noexcept because default constructors of, say, std::list isn't
      Pool() = default;

      ~Pool() {
        shrink_to_fit();
        while (!_acquired.empty()) {
          delete _acquired.back();
          _acquired.pop_back();
        }
      }

      // Deleted other constructors to avoid unintentional copying
      Pool(Pool const&) = delete;
      Pool(Pool&&)      = delete;
      Pool& operator=(Pool const&) = delete;
      Pool& operator=(Pool&&) = delete;

      // Not noexcept because it can throw
      T acquire() {
        if (_acquirable.empty()) {
          if (_acquired.empty()) {
            LIBSEMIGROUPS_EXCEPTION(
                "the pool has not been initialised, cannot acquire!");
          }
          // double size
          push(_acquired.back(), _acquired.size());
        }
        T ptr = _acquirable.top();
        _acquirable.pop();
        _acquired.push_back(ptr);
        _map.emplace(ptr, --_acquired.end());
        return ptr;
      }

      // Not noexcept because it can throw
      void release(T ptr) {
        auto it = _map.find(ptr);
        if (it == _map.end()) {
          LIBSEMIGROUPS_EXCEPTION("attempted to release an object which is "
                                  "not owned by this pool");
        }
        _acquired.erase(it->second);
        _map.erase(it);
        _acquirable.push(ptr);
      }

      void init(T sample) {
        push(sample);
      }

      void shrink_to_fit() {
        while (!_acquirable.empty()) {
          delete _acquirable.top();
          _acquirable.pop();
        }
      }

     private:
      // Not noexcept
      void push(T x, size_t number = 1) {
        for (size_t i = 0; i < number; ++i) {
          _acquirable.push(new (typename std::remove_pointer<T>::type)(*x));
        }
      }

      std::stack<T>                                          _acquirable;
      std::list<T>                                           _acquired;
      std::unordered_map<T, typename std::list<T>::iterator> _map;
    };

    // A pool guard acquires an element from the pool on construction and
    // releases it on destruction. This version doesn't do anything, it's based
    // on the assumption that the type T is small and easy to copy.
    template <typename T>
    class PoolGuard<T, is_non_pointer_t<T>> final {
      static_assert(std::is_default_constructible<T>::value,
                    "PoolGuard<T> requires T to be default-constructible");

     public:
      explicit PoolGuard(Pool<T> const&) {}
      ~PoolGuard() = default;

      // Deleted other constructors to avoid unintentional copying
      PoolGuard()                 = delete;
      PoolGuard(PoolGuard const&) = delete;
      PoolGuard(PoolGuard&&)      = delete;
      PoolGuard& operator=(PoolGuard const&) = delete;
      PoolGuard& operator=(PoolGuard&&) = delete;

      // Get the element acquired from the pool
      T get() const noexcept {
        return T();
      }
      void swap(T) noexcept {}
      void shrink_to_fit() {}
    };

    // A pool guard acquires an element from the pool on construction and
    // releases it on destruction.
    template <typename T>
    class PoolGuard<T, is_pointer_t<T>> final {
     public:
      explicit PoolGuard(Pool<T>& pool) : _pool(pool), _tmp(pool.acquire()) {}

      ~PoolGuard() {
        _pool.release(_tmp);
      }

      // Deleted other constructors to avoid unintentional copying
      PoolGuard()                 = delete;
      PoolGuard(PoolGuard const&) = delete;
      PoolGuard(PoolGuard&&)      = delete;
      PoolGuard& operator=(PoolGuard const&) = delete;
      PoolGuard& operator=(PoolGuard&&) = delete;

      // Get the element acquired from the pool
      T get() const noexcept {
        return _tmp;
      }

      void swap(T other) noexcept {
        std::swap(_tmp, other);
      }

     private:
      Pool<T>& _pool;
      T        _tmp;
    };

  }  // namespace detail
}  // namespace libsemigroups

#endif  // LIBSEMIGROUPS_POOL_HPP_
