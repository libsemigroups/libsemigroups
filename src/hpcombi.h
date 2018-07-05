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

// This file contains specializations of the SemigroupTraits class, and
// adapters, for the element types in HPCombi.

#ifndef LIBSEMIGROUPS_SRC_HPCOMBI_H_
#define LIBSEMIGROUPS_SRC_HPCOMBI_H_
#ifdef LIBSEMIGROUPS_HPCOMBI

#include "adapters.h"
#include "semigroup-traits.h"

namespace libsemigroups {
  using Perm16 = HPCombi::Perm16;

  template <> struct degree<Perm16> {
    inline constexpr size_t operator()(Perm16 const&) const {
      return 16;
    }
  };

  template <> struct inverse<Perm16> {
    inline Perm16 operator()(Perm16 const& x) const {
      return x.inverse();
    }
  };

  template <> struct one<Perm16> {
    inline Perm16 operator()(size_t) const {
      return Perm16::one();
    }

    inline Perm16 operator()(Perm16 const&) const {
      return Perm16::one();
    }
  };

  template <> struct product<Perm16> {
    void operator()(Perm16& xy, Perm16 const& x, Perm16 const& y, size_t = 0) {
      xy = x * y;
    }
  };

  template <> struct swap<Perm16> {
    void operator()(Perm16& x, Perm16& y) const {
      std::swap(x, y);
    }
  };

  template <typename TValueType> struct action<Perm16, TValueType> {
    inline TValueType operator()(Perm16 x, TValueType i) const {
      return x[i];
    }
  };

#ifdef LIBSEMIGROUPS_DENSEHASHMAP
  template <> struct empty_key<Transf16> {
    Transf16 operator()(Transf16 const&) const {
      return {FE, FE, FE, FE, FE, FE, FE, FE, FE, FE, FE, FE, FE, FE, FE, FE};
    }
  };
#endif
}  // namespace libsemigroups

#endif  // LIBSEMIGROUPS_HPCOMBI
#endif  // LIBSEMIGROUPS_SRC_HPCOMBI_H_
