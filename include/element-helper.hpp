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

// This file contains some helper classes for creating various types of
// elements in libsemigroups.

#ifndef LIBSEMIGROUPS_INCLUDE_ELEMENT_HELPER_HPP_
#define LIBSEMIGROUPS_INCLUDE_ELEMENT_HELPER_HPP_

#include "bmat8.hpp"    // for BMat8
#include "element.hpp"  // for Permutation ...
#include "hpcombi.hpp"  // for HPCombi::Perm16, Transf16, . . .

namespace libsemigroups {
  //! Defined in ``element-helper.hpp``.
  //!
  //! Helper for getting the type of the smallest size transformation in
  //! ``libsemigroups`` or ``HPCombi`` that are defined on \p N points.
  //!
  //! This stateless struct has a single public type named ``type`` that holds
  //! a type for representing transformations.
  //!
  //! \tparam N the number of points the transformation will be defined on.
  template <size_t N>
  struct TransfHelper {
#ifdef LIBSEMIGROUPS_HPCOMBI
    //! The type of the smallest sized transformation in ``libsemigroups`` or
    //! ``HPCombi``.
    using type = typename std::conditional<
        N >= 17,
        Transformation<typename SmallestInteger<N>::type>,
        HPCombi::Transf16>::type;
#else
    //! The type of the smallest sized transformation in ``libsemigroups`` or
    //! ``HPCombi``.
    using type = Transformation<typename SmallestInteger<N>::type>;
#endif
  };

  //! Defined in ``element-helper.hpp``.
  //!
  //! Helper for getting the type of the smallest size partial perm in
  //! ``libsemigroups`` or ``HPCombi`` that are defined on \p N points.
  //!
  //! This stateless struct has a single public type named ``type`` that holds
  //! a type for representing partial perms.
  //!
  //! \tparam N the maximum number of points the partial perms will be defined
  //! on.
  template <size_t N>
  struct PPermHelper {
#ifdef LIBSEMIGROUPS_HPCOMBI
    //! The type of the smallest sized partial perm in ``libsemigroups`` or
    //! ``HPCombi``.
    using type = typename std::conditional<
        N >= 17,
        PartialPerm<typename SmallestInteger<N>::type>,
        HPCombi::PPerm16>::type;
#else
    //! The type of the smallest sized partial perm in ``libsemigroups`` or
    //! ``HPCombi``.
    using type = PartialPerm<typename SmallestInteger<N>::type>;
#endif
  };

  //! Defined in ``element-helper.hpp``.
  //!
  //! Helper for getting the type of the smallest size permutation in
  //! ``libsemigroups`` or ``HPCombi`` that are defined on \p N points.
  //!
  //! This stateless struct has a single public type named ``type`` that holds
  //! a type for representing permutation.
  //!
  //! \tparam N the maximum number of points the permutations will be defined
  //! on.
  template <size_t N>
  struct PermHelper {
#ifdef LIBSEMIGROUPS_HPCOMBI
    //! The type of the smallest sized permutation in ``libsemigroups`` or
    //! ``HPCombi``.
    using type = typename std::conditional<
        N >= 17,
        Permutation<typename SmallestInteger<N>::type>,
        HPCombi::Perm16>::type;
#else
    //! The type of the smallest sized permutation in ``libsemigroups`` or
    //! ``HPCombi``.
    using type = Permutation<typename SmallestInteger<N>::type>;
#endif
  };

  //! Defined in ``element-helper.hpp``.
  //!
  //! Helper for getting the type of the smallest size boolean \p N by \p N
  //! matrix in ``libsemigroups`` or ``HPCombi``.
  //!
  //! This stateless struct has a single public type named ``type`` that holds
  //! a type for representing boolean matrices of the specified dimension.
  //!
  //! \tparam N the dimension of the boolean matrices.
  template <size_t N>
  struct BMatHelper {
#ifdef LIBSEMIGROUPS_HPCOMBI
    //! The type of the smallest sized boolean \p N by \p N
    //! matrix in ``libsemigroups`` or ``HPCombi``, if available.
    using type =
        typename std::conditional<N >= 9, BooleanMat, HPCombi::BMat8>::type;
#else
    //! The type of the smallest sized boolean \p N by \p N
    //! matrix in ``libsemigroups`` or ``HPCombi``, if available.
    using type = typename std::conditional<N >= 9, BooleanMat, BMat8>::type;
#endif
  };
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_INCLUDE_ELEMENT_HELPER_HPP_
