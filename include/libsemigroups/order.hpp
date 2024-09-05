//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019-2024 James D. Mitchell
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

// This file contains some functionality for generating elements in the free
// monoid over an alphabet with a given number of letters up to a given length
// in a particular order.

// TODO(later)
// 1. add the others (recursive path words) from test-todd-coxeter.cpp
// 2. add some documentation

#ifndef LIBSEMIGROUPS_ORDER_HPP_
#define LIBSEMIGROUPS_ORDER_HPP_

#include <cstddef>  // for size_t
#include <vector>   // for vector

#include "ranges.hpp"  // for shortlex_compare

#include "ranges.hpp"

namespace libsemigroups {
  // TODO(v3): redo the doc
  //! The possible arguments for standardize(order).
  //!
  //! The values in this enum can be used as the argument for
  //! standardize(order) to specify which ordering should be used.  The
  //! normal forms for congruence classes are given with respect to one of
  //! the orders specified by the values in this enum.
  enum class Order : uint8_t {
    //! No standardization has been done.
    none = 0,
    //! Normal forms are the short-lex least word belonging to a given
    //! congruence class.
    shortlex,
    //! The congruence classes are ordered lexicographically by their
    //! normal form. The normal forms themselves are essentially arbitrary
    //! because there is not necessarily a lexicographically least word in
    //! every class.
    lex,
    //! Normal forms are the recursive-path least word belonging to a given
    //! congruence class.
    recursive
    // wreath TODO(later)
  };

  //! Compare two objects of the same type using
  //! [std::lexicographical_compare].
  //!
  //! Defined in ``order.hpp``.
  //!
  //! \par Possible Implementation
  //! \code
  //! std::lexicographical_compare(
  //!   x.cbegin(), x.cend(), y.cbegin(), y.cend());
  //! \endcode
  //!
  //! \tparam T the type of the objects to be compared.
  //!
  //! \param x const reference to the first object for comparison
  //! \param y const reference to the second object for comparison
  //!
  //! \returns A `bool`.
  //!
  //! \exceptions
  //! See [std::lexicographical_compare].
  //!
  //! \complexity
  //! See [std::lexicographical_compare].
  //!
  //! [std::lexicographical_compare]:
  //! https://en.cppreference.com/w/cpp/algorithm/lexicographical_compare
  template <typename T, typename = std::enable_if_t<!rx::is_input_or_sink_v<T>>>
  bool lexicographical_compare(T const& x, T const& y) {
    return std::lexicographical_compare(
        x.cbegin(), x.cend(), y.cbegin(), y.cend());
  }

  //! Compare two objects via their pointers using [lexicographical_compare].
  //!
  //! Defined in ``order.hpp``.
  //!
  //! \par Possible Implementation
  //! \code
  //! lexicographical_compare(
  //!   x->cbegin(), x->cend(), y->cbegin(), y->cend());
  //! \endcode
  //!
  //! \tparam T the type of the objects to be compared.
  //!
  //! \param x pointer to the first object for comparison
  //! \param y pointer to the second object for comparison
  //!
  //! \returns A `bool`.
  //!
  //! \sa
  //! [std::lexicographical_compare].
  //!
  //! [std::lexicographical_compare]:
  //! https://en.cppreference.com/w/cpp/algorithm/lexicographical_compare
  template <typename T>
  bool lexicographical_compare(T* const x, T* const y) {
    return std::lexicographical_compare(
        x->cbegin(), x->cend(), y->cbegin(), y->cend());
  }

  //! Defined in ``order.hpp``.
  //!
  //! A stateless struct with binary call operator using
  //! [std::lexicographical_compare].
  //!
  //! This only exists to be used as a template parameter, and has no
  //! advantages over using [std::lexicographical_compare] otherwise.
  //!
  //! \tparam T the type of the objects to be compared.
  //!
  //! \sa
  //! [std::lexicographical_compare].
  //!
  //! [std::lexicographical_compare]:
  //! https://en.cppreference.com/w/cpp/algorithm/lexicographical_compare
  struct LexicographicalCompare {
    //! Call operator that compares \p x and \p y using
    //! [std::lexicographical_compare].
    //!
    //! \param x const reference to the first object for comparison
    //! \param y const reference to the second object for comparison
    //!
    //! \returns A `bool`.
    //!
    //! \exceptions
    //! See [std::lexicographical_compare].
    //!
    //! \complexity
    //! See [std::lexicographical_compare].
    //!
    //! [std::lexicographical_compare]:
    //! https://en.cppreference.com/w/cpp/algorithm/lexicographical_compare
    template <typename T>
    bool operator()(T const& x, T const& y) const {
      return lexicographical_compare(x, y);
    }

    template <typename T>
    bool operator()(std::initializer_list<T> x,
                    std::initializer_list<T> y) const {
      return std::lexicographical_compare(
          x.begin(), x.end(), y.begin(), y.end());
    }

    //! Call operator that compares iterators using
    //! [std::lexicographical_compare].
    //!
    //! \param first1 the start of the first object to compare
    //! \param last1 one beyond the end of the first object to compare
    //! \param first2 the start of the second object to compare
    //! \param last2 one beyond the end of the second object to compare
    //!
    //! \returns A `bool`.
    //!
    //! \exceptions
    //! See [std::lexicographical_compare].
    //!
    //! \complexity
    //! See [std::lexicographical_compare].
    //!
    //! [std::lexicographical_compare]:
    //! https://en.cppreference.com/w/cpp/algorithm/lexicographical_compare
    // TODO(v3) remove this?
    template <typename S>
    bool operator()(S first1, S last1, S first2, S last2) const {
      return std::lexicographical_compare(first1, last1, first2, last2);
    }
  };

  //! Compare two objects of the same type using the short-lex reduction
  //! ordering.
  //!
  //! Defined in ``order.hpp``.
  //!
  //! \par Possible Implementation
  //! \code
  //! template <typename T, typename S>
  //! bool shortlex_compare(T const& first1,
  //!                       T const& last1,
  //!                       S const& first2,
  //!                       S const& last2) {
  //!   return (last1 - first1) < (last2 - first2)
  //!          || ((last1 - first1) == (last2 - first2)
  //!              && std::lexicographical_compare
  //!                   (first1, last1, first2, last2));
  //! }
  //! \endcode
  //!
  //! \tparam T the type of iterators to the first object to be compared.
  //! \tparam S the type of iterators to the second object to be compared.
  //!
  //! \param first1 beginning iterator of first object for comparison.
  //! \param last1 ending iterator of first object for comparison.
  //! \param first2 beginning iterator of second object for comparison.
  //! \param last2 ending iterator of second object for comparison.
  //!
  //! \returns A `bool`.
  //!
  //! \exceptions
  //! Throws if [std::lexicographical_compare] does.
  //!
  //! \complexity
  //! At most \f$O(n)\f$ where \f$n\f$ is the minimum of the distance between
  //! \p last1 and \p first1, and the distance between \p last2 and \p first2.
  //!
  //! [std::lexicographical_compare]:
  //! https://en.cppreference.com/w/cpp/algorithm/lexicographical_compare
  template <typename T,
            typename S,
            typename = std::enable_if_t<!rx::is_input_or_sink_v<T>>>
  bool shortlex_compare(T const& first1,
                        T const& last1,
                        S const& first2,
                        S const& last2) {
    return (last1 - first1) < (last2 - first2)
           || ((last1 - first1) == (last2 - first2)
               && std::lexicographical_compare(first1, last1, first2, last2));
  }

  //! Compare two objects of the same type using shortlex_compare.
  //!
  //! Defined in ``order.hpp``.
  //!
  //! \par Possible Implementation
  //! \code
  //! shortlex_compare(
  //!   x.cbegin(), x.cend(), y.cbegin(), y.cend());
  //! \endcode
  //!
  //! \tparam T the type of the objects to be compared.
  //!
  //! \param x const reference to the first object for comparison
  //! \param y const reference to the second object for comparison
  //!
  //! \returns A `bool`.
  //!
  //! \sa shortlex_compare(T const, T const, S const, S const)
  template <typename T, typename = std::enable_if_t<!rx::is_input_or_sink_v<T>>>
  bool shortlex_compare(T const& x, T const& y) {
    return shortlex_compare(x.cbegin(), x.cend(), y.cbegin(), y.cend());
  }

  //! Compare two objects via their pointers using shortlex_compare.
  //!
  //! Defined in ``order.hpp``.
  //!
  //! \par Possible Implementation
  //! \code
  //! shortlex_compare(
  //!   x->cbegin(), x->cend(), y->cbegin(), y->cend());
  //! \endcode
  //!
  //! \tparam T the type of the objects to be compared.
  //!
  //! \param x pointer to the first object for comparison
  //! \param y pointer to the second object for comparison
  //!
  //! \returns A `bool`.
  //!
  //! \sa shortlex_compare(T const, T const, S const, S const)
  template <typename T>
  bool shortlex_compare(T* const x, T* const y) {
    return shortlex_compare(x->cbegin(), x->cend(), y->cbegin(), y->cend());
  }

  //! Defined in ``order.hpp``.
  //!
  //! A stateless struct with binary call operator using
  //! \ref shortlex_compare.
  //!
  //! This only exists to be used as a template parameter, and has no
  //! advantages over using \ref shortlex_compare otherwise.
  //!
  //! \tparam T the type of the objects to be compared.
  //!
  //! \sa
  //! shortlex_compare(T const, T const, S const, S const)
  struct ShortLexCompare {
    //! Call operator that compares \p x and \p y using
    //! \ref shortlex_compare.
    //!
    //! \param x const reference to the first object for comparison
    //! \param y const reference to the second object for comparison
    //!
    //! \returns A `bool`.
    //!
    //! \exceptions
    //! See shortlex_compare(T const, T const, S const, S const).
    //!
    //! \complexity
    //! See shortlex_compare(T const, T const, S const, S const).
    template <typename T>
    bool operator()(T const& x, T const& y) const {
      return shortlex_compare(x, y);
    }
  };

  //! Compare two objects of the same type using the recursive path comparison
  //! described in [Jan12](../biblio.html#Jantzen2012aa) (Definition 1.2.14,
  //! page 24).
  //!
  //! Defined in ``order.hpp``.
  //!
  //! If \f$u, v\in X ^ {*}\f$, \f$u \neq v\f$, and \f$u = a'u\f$,
  //! \f$v = bv'\f$ for some \f$a,b \in X\f$, \f$u',v'\in X ^ {*}\f$, then
  //! \f$u > v\f$ if one of the following conditions holds:
  //! 1. \f$a = b\f$ and \f$u' \geq v'\f$;
  //! 2. \f$a > b\f$ and \f$u  > v'\f$;
  //! 3. \f$b > a\f$ and \f$u' > v\f$.
  //!
  //! This documentation and the implementation of \ref recursive_path_compare
  //! is based on the source code of [Hol19](../biblio.html#Holt2019aa).
  //!
  //! \tparam T the type of iterators to the first object to be compared.
  //! \tparam S the type of iterators to the second object to be compared.
  //!
  //! \param first1 beginning iterator of first object for comparison.
  //! \param last1 ending iterator of first object for comparison.
  //! \param first2 beginning iterator of second object for comparison.
  //! \param last2 ending iterator of second object for comparison.
  //!
  //! \returns A `bool`.
  //!
  //! \exceptions
  //! \noexcept
  //!
  //! \warning
  //! This function has significantly worse performance than all
  //! the variants of \ref shortlex_compare
  //! and [std::lexicographical_compare].
  //!
  //! [std::lexicographical_compare]:
  //! https://en.cppreference.com/w/cpp/algorithm/lexicographical_compare
  template <typename T, typename S>
  bool recursive_path_compare(T const& first1,
                              T        last1,
                              S const& first2,
                              S        last2) noexcept {
    bool lastmoved = false;
    --last1;
    --last2;
    while (true) {
      if (last1 < first1) {
        if (last2 < first2) {
          return lastmoved;
        }
        return true;
      }
      if (last2 < first2) {
        return false;
      }
      if (*last1 == *last2) {
        last1--;
        last2--;
      } else if (*last1 < *last2) {
        last1--;
        lastmoved = false;
      } else if (*last2 < *last1) {
        last2--;
        lastmoved = true;
      }
    }
  }

  //! Compare two objects of the same type using recursive_path_compare.
  //!
  //! Defined in ``order.hpp``.
  //!
  //! \par Possible Implementation
  //! \code
  //! recursive_path_compare(
  //!   x.cbegin(), x.cend(), y.cbegin(), y.cend());
  //! \endcode
  //!
  //! \tparam T the type of the objects to be compared.
  //!
  //! \param x const reference to the first object for comparison
  //! \param y const reference to the second object for comparison
  //!
  //! \returns A `bool`.
  //!
  //! \exceptions
  //! \noexcept
  //!
  //! \sa recursive_path_compare(T const, T, S const, S)
  template <typename T>
  bool recursive_path_compare(T const& x, T const& y) noexcept {
    return recursive_path_compare(x.cbegin(), x.cend(), y.cbegin(), y.cend());
  }

  //! Compare two objects via their pointers using recursive_path_compare.
  //!
  //! Defined in ``order.hpp``.
  //!
  //! \par Possible Implementation
  //! \code
  //! recursive_path_compare(
  //!   x->cbegin(), x->cend(), y->cbegin(), y->cend());
  //! \endcode
  //!
  //! \tparam T the type of the objects to be compared.
  //!
  //! \param x pointer to the first object for comparison
  //! \param y pointer to the second object for comparison
  //!
  //! \returns A `bool`.
  //!
  //! \exceptions
  //! \noexcept
  //!
  //! \sa recursive_path_compare(T const, T, S const, S)
  template <typename T>
  bool recursive_path_compare(T* const x, T* const y) noexcept {
    return recursive_path_compare(
        x->cbegin(), x->cend(), y->cbegin(), y->cend());
  }

  //! Defined in ``order.hpp``.
  //!
  //! A stateless struct with binary call operator using
  //! \ref recursive_path_compare.
  //!
  //! This only exists to be used as a template parameter, and has no
  //! advantages over using \ref recursive_path_compare otherwise.
  //!
  //! \tparam T the type of the objects to be compared.
  //!
  //! \sa
  //! recursive_path_compare(T const, T const, S const, S const)
  struct RecursivePathCompare {
    //! Call operator that compares \p x and \p y using
    //! \ref recursive_path_compare.
    //!
    //! \param x const reference to the first object for comparison
    //! \param y const reference to the second object for comparison
    //!
    //! \returns A `bool`.
    //!
    //! \exceptions
    //! \noexcept
    template <typename T>
    bool operator()(T const& x, T const& y) noexcept {
      return recursive_path_compare(x.cbegin(), x.cend(), y.cbegin(), y.cend());
    }
  };

}  // namespace libsemigroups

#endif  // LIBSEMIGROUPS_ORDER_HPP_
