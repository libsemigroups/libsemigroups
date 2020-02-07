//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019 James D. Mitchell
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

// This file contains a class for FunctionRef, a light-weight wrapper for
// callables, from:
//    https://vittorioromeo.info/index/blog/passing_functions_to_functions.html
//
// Note that it is ok to use FunctionRef's as parameters for functions, and in
// other situations where the callable is guaranteed to exist when it is
// called. For example, the following is not valid:
//
//    auto foo = FunctionRef([](){ return 42; });
//    ...
//    foo();
//
// since the lambda which is the argument to the FunctionRef constructor is a
// temporary, and so the FunctionRef is not valid after the line where it is
// created.

#ifndef LIBSEMIGROUPS_FUNCTION_REF_HPP_
#define LIBSEMIGROUPS_FUNCTION_REF_HPP_

#include "libsemigroups-debug.hpp"  // for LIBSEMIGROUPS_ASSERT
#include "stl.hpp"                  // for IsCallable

namespace libsemigroups {
  namespace detail {
    template <typename TSignature>
    class FunctionRef;

    template <typename TReturn, typename... TArgs>
    class FunctionRef<TReturn(TArgs...)> final {
     private:
      void* _ptr;
      TReturn (*_erased_fn)(void*, TArgs...);

     public:
      FunctionRef() noexcept : _ptr(nullptr) {}

      template <typename T,
                typename = typename std::enable_if<
                    IsCallable<T&(TArgs...)>{}
                    && !std::is_same<typename std::decay<T>::type,
                                     FunctionRef>{}>::type>
      FunctionRef(T&& x) noexcept
          : _ptr{reinterpret_cast<void*>(std::addressof(x))} {
        _erased_fn = [](void* ptr, TArgs... xs) -> TReturn {
          return (*reinterpret_cast<typename std::add_pointer<T>::type>(ptr))(
              std::forward<TArgs>(xs)...);
        };
      }

      // On the linked page above, decltype(auto) is used and not TReturn, but
      // since we are only using callables with return type bool, and
      // decltype(auto) is C++14, we replace it with TReturn.
      TReturn operator()(TArgs... xs) const
          noexcept(noexcept(_erased_fn(_ptr, std::forward<TArgs>(xs)...))) {
        LIBSEMIGROUPS_ASSERT(valid());
        return _erased_fn(_ptr, std::forward<TArgs>(xs)...);
      }

      inline bool valid() const noexcept {
        return _ptr != nullptr;
      }

      inline void invalidate() noexcept {
        _ptr = nullptr;
      }
    };
  }  // namespace detail
}  // namespace libsemigroups

#endif  // LIBSEMIGROUPS_FUNCTION_REF_HPP_
