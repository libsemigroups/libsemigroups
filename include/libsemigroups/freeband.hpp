//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2021-2025 James D. Mitchell
//                    Tom D. Conti-Leslie
//                    Murray T. Whyte
//                    Reinis Cirpons
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

// This file contains a declaration of the function freeband_equal_to which
// implements the algorithm described in:
// "Efficient Testing of Equivalence of Words in a Free Idempotent Semigroup"
// by J. Radoszewski and W. Rytter
// in SOFSEM 2010: Theory and Practice of Computer Science. Jan. 2010,
// pp. 664-671
// doi: 10.1007/978-3-642-11266-9_55

#ifndef LIBSEMIGROUPS_FREEBAND_HPP_
#define LIBSEMIGROUPS_FREEBAND_HPP_

#include "types.hpp"  // word_type

namespace libsemigroups {

  //! \defgroup freeband_group Radoszewski-Rytter
  //!
  //! `libsemigroups` contains an implementation of the Radoszewski-Rytter
  //! Algorithm \cite Radoszewski2010aa for testing equivalence of words in free
  //! bands.
  //!
  //! The documentation for the functions in `libsemigroups` for free bands
  //! are linked to below.

  //! \ingroup freeband_group
  //!
  //! \brief Check if two words represent the same element of a free
  //! band.
  //!
  //! The free band is the free object in the variety of bands or idempotent
  //! semigroups. The free band \f$\textrm{FB}(A)\f$ generated by some set
  //! \f$A\f$ is the largest semigroup all of whose elements \f$x\f$ are
  //! idempotent, i.e. satisfy \f$x^2=x\f$. This function efficiently compares
  //! whether two words in the generators of \f$\textrm{FB}(A)\f$ are the same
  //! as elements of the free band.
  //!
  //! \param x the first word to compare in the free band.
  //! \param y the second word to compare in the free band.
  //!
  //! \returns
  //! Return `true` if both words are the same as elements of the free band and
  //! `false` otherwise.
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  //!
  //! \complexity
  //! The time complexity is \f$O(mn)\f$ and space complexity is \f$O(n)\f$
  //! where \f$n\f$ is the total length of \p x and \p y, and \f$m\f$ is the
  //! number of distinct letters appearing in \p x and \p y.
  //!
  //! \par Example
  //! \code
  //! using namespace libsemigroups;
  //! freeband_equal_to({0, 1, 2, 3, 2, 1, 0},
  //!                   {0, 1, 2, 3, 2, 3, 2, 1, 0}); // true
  //! freeband_equal_to({1, 2, 3}, {0, 1, 2}); // false
  //! freeband_equal_to({1, 4, 2, 3, 10}, {1, 4, 1, 4, 2, 3, 10}) // true
  //! freeband_equal_to({0, 1, 2, 3, 4, 0, 1, 2, 3, 4},
  //!                   {4, 3, 2, 1, 0, 4, 3, 2, 1, 0}); // false
  //! freeband_equal_to({0, 1, 2, 1, 0, 1, 2}, {0, 1, 2}); // true
  //! freeband_equal_to({0, 1, 2, 3, 0, 1},
  //!                   {0, 1, 2, 3, 3, 2, 2, 1, 0, 2, 1, 0, 2, 3,
  //!                    0, 2, 1, 3, 2, 1, 2, 3, 2, 1, 0, 2, 0, 1,
  //!                    0, 2, 0, 3, 2, 0, 1, 2, 2, 3, 0, 1}); // true
  //! \endcode
  bool freeband_equal_to(word_type const& x, word_type const& y);

  //! \ingroup freeband_group
  //!
  //! \brief Check if two words represent the same element of a free
  //! band (non-`word_type`).
  //!
  //! \tparam T any type that can be converted to `word_type`.
  //!
  //! \param x the first word to compare in the free band.
  //! \param y the second word to compare in the free band.
  //!
  //! \returns
  //! Return `true` if both words are the same as elements of the free band and
  //! `false` otherwise.
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  //!
  //! \complexity
  //! The time complexity is \f$O(mn)\f$ and space complexity is \f$O(n)\f$
  //! where \f$n\f$ is the total length of \p x and \p y, and \f$m\f$ is the
  //! number of distinct letters appearing in \p x and \p y.
  template <typename T>
  bool freeband_equal_to(T x, T y) {
    word_type x1(x), y1(y);
    return freeband_equal_to(x1, y1);
  }

  //! \ingroup freeband_group
  //!
  //! \brief Check if two words represent the same element of a free
  //! band (iterators).
  //!
  //! \tparam T any type that can be converted to `word_type`.
  //!
  //! \param first1 iterator to start of the first word.
  //! \param last1 iterator to end of the first word.
  //! \param first2 iterator to start of the second word.
  //! \param last2 iterator to end of the second word.
  //!
  //! \returns
  //! Return `true` if both words are the same as elements of the free band and
  //! `false` otherwise.
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  //!
  //! \complexity
  //! The time complexity is \f$O(mn)\f$ and space complexity is \f$O(n)\f$
  //! where \f$n\f$ is the total length of \p x and \p y, and \f$m\f$ is the
  //! number of distinct letters appearing in \p x and \p y.
  template <typename T>
  bool freeband_equal_to(T first1, T last1, T first2, T last2) {
    return freeband_equal_to(word_type(first1, last1),
                             word_type(first2, last2));
  }

}  // namespace libsemigroups

#endif  // LIBSEMIGROUPS_FREEBAND_HPP_
