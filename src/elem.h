//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2018 Florent Hivert
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

// This file contains the declaration of the element class and its subclasses.

#ifndef LIBSEMIGROUPS_SRC_ELEM_H_
#define LIBSEMIGROUPS_SRC_ELEM_H_

namespace libsemigroups {

  class PermArray<
    typename TValueType,
    size_t Size,
    typename Allocator> {

      std::array<TValueType, Size> *parray;

  public:

  PermArray(Allocator alloc) : parray(alloc.allocate(1)) {}

  explicit PermArray(std::array<TValueType, Size> * ar) : parray(ar) {}

  explicit PermArray(std::array<TValueType, Size> const &ar,
                     Allocator alloc = Allocator() )
    : parray(alloc.allocate(1)) {
        alloc.construct(parray, ar);
  }

  bool operator==(PermArray<TValueType, Size, Allocator> const& other)
    const {
    return *(other->parray) == *(this->parray);
  }

  // change self to be the identity
  void identity() {
    for (size_t i=0; i < Size; i++) {
      *parray[i] = i;
  }

  void redefine(PermArray<TValueType, Size, Allocator> const &x,
                PermArray<TValueType, Size, Allocator> const &y) {
    for (size_t i=0; i < Size; i++) {
      *parray[i] = x->parray[y->parray[i]];
    }
  }


  template <typename T, size_t Size>
    static inline size_t array_hash(std::array<T, Size> const *ar) {
      size_t seed = 0;
      for (auto const& x : *ar) {
        seed ^= std::hash<T>{}(x) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
      }
      return seed;
    }

  };

}
#endif LIBSEMIGROUPS_SRC_ELEM_H_
