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

#include "semigroups.h"

namespace libsemigroups {

  template <>
  Element*
  Semigroup<>::multiply(Element* xy, Element* x, Element* y, size_t tid) const {
    xy->redefine(x, y, tid);
    return xy;
  }

  template <> Element* Semigroup<>::copy(Element* x) const {
    return x->really_copy();
  }

  template <>
  Element* Semigroup<>::copy(Element* x, size_t increase_deg_by) const {
    return x->really_copy(increase_deg_by);
  }

  template <> void Semigroup<>::free(Element* x) const {
    const_cast<Element*>(x)->really_delete();
    delete x;
  }

  template <> void Semigroup<>::swap(Element* x, Element* y) const {
    x->swap(y);
  }

  template <> Element* Semigroup<>::one(Element* x) const {
    return x->identity();
  }

  template <> size_t Semigroup<>::degree(Element* x) const {
    return x->degree();
  }

  template <> size_t Semigroup<>::complexity(Element* x) const {
    return x->degree();
  }
}  // namespace libsemigroups
