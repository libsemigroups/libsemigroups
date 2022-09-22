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

// This file contains some functionality missing in some implementations of the
// stl, or to augment the stl implementations.

#ifndef LIBSEMIGROUPS_STL_HPP_
#define LIBSEMIGROUPS_STL_HPP_

#include <cstddef>      // for size_t
#include <memory>       // for unique_ptr
#include <thread>       // for std::thread
#include <type_traits>  // for enable_if, forward, hash, is_function, is_same
#include <vector>       // for vector

namespace libsemigroups {

  namespace detail {
    // Pass parameter p by value because this function modifies p.
    template <typename TContainer, typename TPerm>
    void apply_permutation(TContainer& cont, TPerm p) {
      size_t const n = p.size();
      for (size_t i = 0; i < n; ++i) {
        size_t current = i;
        while (i != p[current]) {
          size_t next = p[current];
          std::swap(cont[current], cont[next]);
          p[current] = current;
          current    = next;
        }
        p[current] = current;
      }
    }

    template <typename TContainer, typename TPerm>
    void apply_permutation(TContainer& cont1, TContainer& cont2, TPerm p) {
      size_t const n = p.size();
      for (size_t i = 0; i < n; ++i) {
        size_t current = i;
        while (i != p[current]) {
          size_t next = p[current];
          std::swap(cont1[current], cont1[next]);
          std::swap(cont2[current], cont2[next]);
          p[current] = current;
          current    = next;
        }
        p[current] = current;
      }
    }

    // Since std::is_invocable is only introduced in C++17, we use this
    //
    // from: https://stackoverflow.com/q/15393938/
    // Only works if there are no overloads of operator() in type T.
    template <typename T, typename = void>
    struct IsCallable : std::is_function<T> {};

    template <typename T>
    struct IsCallable<
        T,
        std::enable_if_t<
            std::is_same<decltype(void(&T::operator())), void>::value>>
        : std::true_type {};

    // From p, 275, Section 8 of C++ concurrency in action, 2nd edition, by
    // Anthony Williams.
    class JoinThreads {
      std::vector<std::thread>& _threads;

     public:
      explicit JoinThreads(std::vector<std::thread>& threads)
          : _threads(threads) {}

      ~JoinThreads() {
        for (size_t i = 0; i < _threads.size(); ++i) {
          if (_threads[i].joinable()) {
            _threads[i].join();
          }
        }
      }
    };

  }  // namespace detail
}  // namespace libsemigroups

namespace std {
  template <typename TValueType, size_t N>
  struct hash<std::array<TValueType, N>> {
    size_t operator()(std::array<TValueType, N> const& ar) const {
      size_t seed = 0;
      for (auto const& x : ar) {
        seed ^= std::hash<TValueType>{}(x) + 0x9e3779b9 + (seed << 6)
                + (seed >> 2);
      }
      return seed;
    }
  };

  template <typename TValueType>
  struct hash<std::vector<TValueType>> {
    size_t operator()(std::vector<TValueType> const& vec) const {
      size_t seed = 0;
      for (auto const& x : vec) {
        seed ^= std::hash<TValueType>{}(x) + 0x9e3779b9 + (seed << 6)
                + (seed >> 2);
      }
      return seed;
    }
  };
}  // namespace std
#endif  // LIBSEMIGROUPS_STL_HPP_
