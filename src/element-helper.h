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

// This file contains some helper structs for determining the most space
// efficient types for various kinds of semigroup elements.

#ifndef LIBSEMIGROUPS_SRC_ELEMENT_HELPER_H_
#define LIBSEMIGROUPS_SRC_ELEMENT_HELPER_H_

#include "element.h"
#include "hpcombi.h"
#include "libsemigroups-config.h"

namespace libsemigroups {

  //! Provides a type giving the smallest unsigned integer type capable of
  //! representing the template \c N.
  //!
  //! The type SmallestInteger<N>::type contains the smallest (in terms of
  //! memory required) unsigned integer type which can represent the
  //! non-negative integer \c N.
  // (i.e. >= or >)
  template <size_t N> struct SmallestInteger {
    //! The smallest (in terms of memory required) unsigned integer type which
    //! can represent \c N.
    using type = typename std::conditional<
        N >= 0x100000000,
        u_int64_t,
        typename std::conditional<
            N >= 0x10000,
            u_int32_t,
            typename std::conditional<N >= 0x100, u_int16_t, u_int8_t>::type>::
            type>::type;
  };

  template <size_t N> struct Transf {
#ifdef LIBSEMIGROUPS_HPCOMBI
    using type = typename std::conditional<
        N >= 17,
        Transformation<typename SmallestInteger<N>::type>,
        HPCombi::Transf16>::type;
#else
    using type = Transformation<typename SmallestInteger<N>::type>;
#endif
  };

  template <size_t N> struct PPerm {
#ifdef LIBSEMIGROUPS_HPCOMBI
    using type = typename std::conditional<
        N >= 17,
        PartialPerm<typename SmallestInteger<N>::type>,
        HPCombi::PTransf16>::type;
#else
    using type = PartialPerm<typename SmallestInteger<N>::type>;
#endif
  };

  template <size_t N> struct Perm {
#ifdef LIBSEMIGROUPS_HPCOMBI
    using type = typename std::conditional<
        N >= 17,
        Permutation<typename SmallestInteger<N>::type>,
        HPCombi::Perm16>::type;
#else
    using type = Permutation<typename SmallestInteger<N>::type>;
#endif
  };

  class BMat8;
  template <size_t N> struct BMat {
    using type = typename std::conditional<N >= 9, BooleanMat, BMat8>::type;
  };
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_SRC_ELEMENT_HELPER_H_
