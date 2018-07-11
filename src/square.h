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

// This file contains two wrappers for a square arrays of arrays. The first
// wrapper makes the array of arrays behave like an array of vectors, and the
// second is a very thin wrapper around a square arrary of arrays, which exists
// mostly just for the assertions.

#ifndef LIBSEMIGROUPS_SRC_SQUARE_H_
#define LIBSEMIGROUPS_SRC_SQUARE_H_

#include <algorithm>
#include <array>

#include "libsemigroups-debug.h"

namespace libsemigroups {

  template <typename T, size_t N, class A = std::allocator<T>>
    //FIXME remove or use the template parameter A
  class SquareVector {
    // So that SquareVector<T, N, A> can access private data members of
    // SquareVector<S, M, B> and vice versa.
    template <typename S, size_t M, class B> friend class SquareVector;

   public:
    SquareVector() : _arrays(), _sizes() {
      clear();
    }

    void clear() {
      _sizes.fill(0);
    }

    void push_back(size_t depth, T x) {
      LIBSEMIGROUPS_ASSERT(depth < N);
      LIBSEMIGROUPS_ASSERT(_sizes[depth] < N);
      _arrays[depth][_sizes[depth]] = x;
      _sizes[depth]++;
    }

    inline T back(size_t depth) const noexcept {
      LIBSEMIGROUPS_ASSERT(depth < N);
      return _arrays[depth][_sizes[depth] - 1];
    }

    inline T const& at(size_t depth, size_t index) const {
      LIBSEMIGROUPS_ASSERT(depth < N);
      LIBSEMIGROUPS_ASSERT(index < _sizes[depth]);
      return _arrays[depth][index];
    }

    inline size_t size(size_t depth) const noexcept {
      LIBSEMIGROUPS_ASSERT(depth < N);
      return _sizes[depth];
    }

    inline typename std::array<T, N>::const_iterator
    cbegin(size_t depth) const {
      LIBSEMIGROUPS_ASSERT(depth < N);
      return _arrays[depth].cbegin();
    }

    inline typename std::array<T, N>::const_iterator cend(size_t depth) const {
      LIBSEMIGROUPS_ASSERT(depth < N);
      return _arrays[depth].cbegin() + _sizes[depth];
    }

    inline typename std::array<T, N>::iterator begin(size_t depth) {
      LIBSEMIGROUPS_ASSERT(depth < N);
      return _arrays[depth].begin();
    }

    inline typename std::array<T, N>::iterator end(size_t depth) const {
      LIBSEMIGROUPS_ASSERT(depth < N);
      return _arrays[depth].begin() + _sizes[depth];
    }

   private:
    std::array<std::array<T, N>, N> _arrays;
    std::array<size_t, N>           _sizes;
  };

  template <typename T, size_t N, class A = std::allocator<T>>
  class SquareArray {
    // So that SquareArray<T, N, A> can access private data members of
    // SquareArray<S, M, B> and vice versa.
    template <typename S, size_t M, class B> friend class SquareArray;

   public:
    SquareArray() : _arrays() {}

    inline void fill(T const& value) {
      for (auto& x : _arrays) {
        x.fill(value);
      }
    }

    inline std::array<T, N>& operator[](size_t depth) noexcept {
      LIBSEMIGROUPS_ASSERT(depth < N);
      return _arrays[depth];
    }

    inline T const& at(size_t depth, size_t index) const {
      LIBSEMIGROUPS_ASSERT(depth < N);
      LIBSEMIGROUPS_ASSERT(index < N);
      return _arrays.at(depth).at(index);
    }

    inline typename std::array<T, N>::const_iterator
    cbegin(size_t depth) const {
      LIBSEMIGROUPS_ASSERT(depth < N);
      return _arrays[depth].cbegin();
    }

    inline typename std::array<T, N>::const_iterator cend(size_t depth) const {
      LIBSEMIGROUPS_ASSERT(depth < N);
      return _arrays[depth].cend();
    }

    inline typename std::array<T, N>::iterator begin(size_t depth) {
      LIBSEMIGROUPS_ASSERT(depth < N);
      return _arrays[depth].begin();
    }

    inline typename std::array<T, N>::iterator end(size_t depth) const {
      LIBSEMIGROUPS_ASSERT(depth < N);
      return _arrays[depth].end();
    }

   private:
    std::array<std::array<T, N>, N> _arrays;
  };
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_SRC_SQUARE_H_
