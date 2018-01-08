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

// This file contains the declaration of an element container, i.e. an object
// that contains some kind of element that can be multiplied etc.

#ifndef LIBSEMIGROUPS_SRC_ELTCONT_H_
#define LIBSEMIGROUPS_SRC_ELTCONT_H_

#include "elements.h"

namespace libsemigroups {

  template <typename TElementType> struct ElementContainer {
    inline TElementType multiply(TElementType xy,
                                 TElementType x,
                                 TElementType y,
                                 size_t       tid = 0) const {
      (void) xy;
      (void) tid;
      return x * y;
    }

    inline TElementType copy(TElementType x, size_t increase_deg_by = 0) const {
      (void) increase_deg_by;
      return x;
    }

    inline void free(TElementType x) const {
      (void) x;
    }

    inline void swap(TElementType x, TElementType y) const {
      std::swap(x, y);
    }

    inline TElementType one(TElementType x) const {
      return x.one();
    }

    inline size_t element_degree(TElementType x) const {
      (void) x;
      return 0;
    }

    inline size_t complexity(TElementType x) const {
      (void) x;
      return 1;
    }

    inline bool cmp(TElementType x, TElementType y) const {
      return x < y;
    }

#if defined(LIBSEMIGROUPS_HAVE_DENSEHASHMAP) \
    && defined(LIBSEMIGROUPS_USE_DENSEHASHMAP)
    inline TElementType empty_key(TElementType x) const {
      (void) x;
      return TElementType(-1);
    }
#endif
  };

  template <typename TElementPointerType> struct ElementPointerContainer {
    inline TElementPointerType multiply(TElementPointerType xy,
                                        TElementPointerType x,
                                        TElementPointerType y,
                                        size_t              tid = 0) const {
      xy->redefine(x, y, tid);
      return xy;
    }

    inline TElementPointerType copy(TElementPointerType x,
                                    size_t increase_deg_by = 0) const {
      return x->really_copy(increase_deg_by);
    }

    inline void free(TElementPointerType x) const {
      x->really_delete();
      delete x;
    }

    inline void swap(TElementPointerType x, TElementPointerType y) const {
      x->swap(y);
    }

    inline TElementPointerType one(TElementPointerType x) const {
      return static_cast<TElementPointerType>(x->identity());
    }

    inline size_t element_degree(TElementPointerType x) const {
      return x->degree();
    }

    inline size_t complexity(TElementPointerType x) const {
      return x->complexity();
    }

    inline bool cmp(TElementPointerType x, TElementPointerType y) const {
      return *x < *y;
    }

#if defined(LIBSEMIGROUPS_HAVE_DENSEHASHMAP) \
    && defined(LIBSEMIGROUPS_USE_DENSEHASHMAP)
    inline TElementPointerType empty_key(TElementPointerType x) const {
      return x->empty_key();
    }
#endif
  };

  template <>
  struct ElementContainer<Element*> : public ElementPointerContainer<Element*> {
  };

}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_SRC_ELTCONT_H_
