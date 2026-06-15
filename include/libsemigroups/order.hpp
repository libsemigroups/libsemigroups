//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019-2026 James D. Mitchell + James W. Swent
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

// This file contains the declarations of several functions and structs
// defining linear orders on words.

// TODO
// * dry it out
// * noexcept

#ifndef LIBSEMIGROUPS_ORDER_HPP_
#define LIBSEMIGROUPS_ORDER_HPP_

#include <algorithm>         // for std::find_if, std::lexicographical_compare
#include <cstddef>           // for size_t
#include <cstdint>           // for uint8_t
#include <initializer_list>  // for initializer_list
#include <iterator>          // for distance
#include <numeric>           // for accumulate
#include <type_traits>       // for enable_if_t
#include <utility>           // for move
#include <vector>            // for vector

#include "exception.hpp"  // for LIBSEMIGROUPS_EXCEPTION
#include "ranges.hpp"     // for shortlex_compare

namespace libsemigroups {
  //! \ingroup types_group
  //!
  //! \brief The possible orderings of words and strings.
  //!
  //! The values in this enum can be used as the arguments for functions such as
  //! \ref ToddCoxeter::standardize(Order) or \ref WordRange::order(Order) to
  //! specify which ordering should be used. The normal forms for congruence
  //! classes are given with respect to one of the orders specified by the
  //! values in this enum.
  //!
  //! \sa orders_group
  enum class Order : uint8_t {
    //! No ordering.
    none = 0,

    //! The len-lex ordering. Words are first ordered by length, and then
    //! lexicographically.
    lenlex,

    //! The short-lex ordering. Words are first ordered by length, and then
    //! lexicographically.
    //!
    //! \deprecated_warning{value} Use \ref Order::lenlex instead.
    shortlex [[deprecated("Use lenlex instead!")]] = lenlex,

    //! The lexicographic ordering. Note that this is not a well-order, so there
    //! may not be a lexicographically least word in a given congruence class of
    //! words.
    lex,

    //! The recursive-path ordering, as described in \cite Jantzen2012aa
    //! (Definition 1.2.14, page 24).
    rpo,

    //! The recursive-path ordering, as described in \cite Jantzen2012aa
    //! (Definition 1.2.14, page 24).
    //!
    //! \deprecated_warning{value} Use \ref Order::rpo instead.
    recursive [[deprecated("Use rpo instead!")]] = rpo

    // wreath TODO(later)
  };

  //! \defgroup orders_group Orders
  //! This page contains the documentation for several class and function
  //! templates for comparing words or strings with respect to certain reduction
  //! orderings.
  //!
  //! \sa \ref Order
  //!
  //! @{

  //////////////////////////////////////////////////////////////////////
  // Lex
  //////////////////////////////////////////////////////////////////////

  //! \brief Compare two objects of the same type using
  //! std::lexicographical_compare.
  //!
  //! Defined in `order.hpp`.
  //!
  //! This function compares two objects of the same type using
  //! std::lexicographical_compare.
  //!
  //! \tparam Thing the type of the objects to be compared.
  //!
  //! \param x const reference to the first object for comparison.
  //! \param y const reference to the second object for comparison.
  //!
  //! \returns The boolean value \c true if \p x is lexicographically less than
  //! \p y, and \c false otherwise.
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  //! See std::lexicographical_compare.
  //!
  //! \complexity
  //! See std::lexicographical_compare.
  //!
  //! \par Possible Implementation
  //! \code_no_test
  //! lexicographical_compare(x.cbegin(),x.cend(),y.cbegin(),y.cend());
  //! \end_code_no_test
  template <typename Thing,
            typename = std::enable_if_t<!rx::is_input_or_sink_v<Thing>>>
  [[nodiscard]] bool lex_cmp(Thing const& x, Thing const& y) {
    return std::lexicographical_compare(
        x.cbegin(), x.cend(), y.cbegin(), y.cend());
  }

  //! \brief Compare two objects via their pointers using
  //! std::lexicographical_compare.
  //!
  //! Defined in `order.hpp`.
  //!
  //! This function compares two objects via their pointers using
  //! std::lexicographical_compare.
  //!
  //! \tparam Thing the type of the objects to be compared.
  //!
  //! \param x pointer to the first object for comparison.
  //! \param y pointer to the second object for comparison.
  //!
  //! \returns The boolean value \c true if \p x is lexicographically less than
  //! \p y, and \c false otherwise.
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  //! See std::lexicographical_compare.
  //!
  //! \complexity
  //! See std::lexicographical_compare.
  //!
  //! \par Possible Implementation
  //! \code_no_test
  //! lexicographical_compare(x.cbegin(),x.cend(),y.cbegin(),y.cend());
  //! \end_code_no_test
  template <typename Thing>
  [[nodiscard]] bool lex_cmp(Thing* const x, Thing* const y) {
    return std::lexicographical_compare(
        x->cbegin(), x->cend(), y->cbegin(), y->cend());
  }

  //! \brief A stateless struct with binary call operator using
  //! std::lexicographical_compare.
  //!
  //! Defined in `order.hpp`.
  //!
  //! A stateless struct with binary call operator using
  //! std::lexicographical_compare.
  //!
  //! This only exists to be used as a template parameter, and has no
  //! advantages over using std::lexicographical_compare otherwise.
  //!
  //! \sa
  //! std::lexicographical_compare.
  struct LexCmp {
    //! \brief Call operator that compares \p x and \p y using
    //! std::lexicographical_compare.
    //!
    //! Call operator that compares \p x and \p y using
    //! std::lexicographical_compare.
    //!
    //! \tparam Thing the type of the parameters.
    //!
    //! \param x const reference to the first object for comparison.
    //! \param y const reference to the second object for comparison.
    //!
    //! \returns The boolean value \c true if \p x is lexicographically less
    //! than \p y, and \c false otherwise.
    //!
    //! \exceptions
    //! See std::lexicographical_compare.
    //!
    //! \complexity
    //! See std::lexicographical_compare.
    template <typename Thing>
    [[nodiscard]] bool operator()(Thing const& x, Thing const& y) const {
      return lex_cmp(x, y);
    }

    //! \brief Call operator that compares \p x and \p y given initializer lists
    //! using std::lexicographical_compare.
    //!
    //! Call operator that compares \p x and
    //! \p y given initializer lists using std::lexicographical_compare.
    //!
    //! \tparam T the items in the arguments.
    //!
    //! \param x initializer list for the first object for comparison.
    //! \param y initializer list for the second object for comparison.
    //!
    //! \returns The boolean value \c true if \p x is lexicographically less
    //! than \p y, and \c false otherwise.
    //!
    //! \exceptions
    //! See std::lexicographical_compare.
    //!
    //! \complexity
    //! See std::lexicographical_compare.
    // TODO(v4) is this really necessary?
    template <typename T>
    [[nodiscard]] bool operator()(std::initializer_list<T> x,
                                  std::initializer_list<T> y) const {
      return std::lexicographical_compare(
          x.begin(), x.end(), y.begin(), y.end());
    }

    //! \brief Call operator that compares iterators using
    //! std::lexicographical_compare.
    //!
    //! Call operator that compares iterators using
    //! std::lexicographical_compare.
    //!
    //! \tparam Iterator the type of the parameters.
    //!
    //! \param first1 the start of the first object to compare.
    //! \param last1 one beyond the end of the first object to compare.
    //! \param first2 the start of the second object to compare.
    //! \param last2 one beyond the end of the second object to compare.
    //!
    //! \returns The boolean value \c true if the range `[first1, last1)` is
    //! lexicographically less than the range `[first2, last2)`, and \c false
    //! otherwise.
    //!
    //! \exceptions
    //! See std::lexicographical_compare.
    //!
    //! \complexity
    //! See std::lexicographical_compare.
    // TODO(v4) remove this?
    template <typename Iterator>
    [[nodiscard]] bool operator()(Iterator first1,
                                  Iterator last1,
                                  Iterator first2,
                                  Iterator last2) const {
      return std::lexicographical_compare(first1, last1, first2, last2);
    }
  };

  //////////////////////////////////////////////////////////////////////
  // Lexicographical - deprecated
  //////////////////////////////////////////////////////////////////////

  //! \brief Compare two objects of the same type using
  //! std::lexicographical_compare.
  //!
  //! Defined in `order.hpp`.
  //!
  //! This function compares two objects of the same type using
  //! std::lexicographical_compare.
  //!
  //! \tparam Thing the type of the objects to be compared.
  //!
  //! \param x const reference to the first object for comparison.
  //! \param y const reference to the second object for comparison.
  //!
  //! \returns The boolean value \c true if \p x is lexicographically less than
  //! \p y, and \c false otherwise.
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  //! See std::lexicographical_compare.
  //!
  //! \complexity
  //! See std::lexicographical_compare.
  //!
  //! \par Possible Implementation
  //! \code_no_test
  //! lexicographical_compare(x.cbegin(),x.cend(),y.cbegin(),y.cend());
  //! \end_code_no_test
  //!
  //! \deprecated_warning{function} Use \ref lex_cmp instead.
  template <typename Thing,
            typename = std::enable_if_t<!rx::is_input_or_sink_v<Thing>>>
  [[nodiscard]] [[deprecated("Use lex_cmp instead!")]] bool
  lexicographical_compare(Thing const& x, Thing const& y) {
    return std::lexicographical_compare(
        x.cbegin(), x.cend(), y.cbegin(), y.cend());
  }

  //! \brief Compare two objects via their pointers using
  //! std::lexicographical_compare.
  //!
  //! Defined in `order.hpp`.
  //!
  //! This function compares two objects via their pointers using
  //! std::lexicographical_compare.
  //!
  //! \tparam Thing the type of the objects to be compared.
  //!
  //! \param x pointer to the first object for comparison.
  //! \param y pointer to the second object for comparison.
  //!
  //! \returns The boolean value \c true if \p x is lexicographically less than
  //! \p y, and \c false otherwise.
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  //! See std::lexicographical_compare.
  //!
  //! \complexity
  //! See std::lexicographical_compare.
  //!
  //! \par Possible Implementation
  //! \code_no_test
  //! lexicographical_compare(x.cbegin(),x.cend(),y.cbegin(),y.cend());
  //! \end_code_no_test
  //!
  //! \deprecated_warning{function} Use \ref lex_cmp instead.
  template <typename Thing>
  [[nodiscard]] [[deprecated("Use lex_cmp instead!")]] bool
  lexicographical_compare(Thing* const x, Thing* const y) {
    return std::lexicographical_compare(
        x->cbegin(), x->cend(), y->cbegin(), y->cend());
  }

  //! \brief A stateless struct with binary call operator using
  //! std::lexicographical_compare.
  //!
  //! Defined in `order.hpp`.
  //!
  //! A stateless struct with binary call operator using
  //! std::lexicographical_compare.
  //!
  //! This only exists to be used as a template parameter, and has no
  //! advantages over using std::lexicographical_compare otherwise.
  //!
  //! \sa
  //! std::lexicographical_compare.
  //!
  //! \deprecated_warning{struct} Use \ref lex_cmp instead.
  using LexicographicalCompare [[deprecated("Use LexCmp instead")]] = LexCmp;

  //////////////////////////////////////////////////////////////////////
  // Len-lex
  //////////////////////////////////////////////////////////////////////

  //! \brief Compare two objects of the same type using the len-lex reduction
  //! ordering.
  //!
  //! Defined in `order.hpp`.
  //!
  //! This function compares two objects of the same type using the len-lex
  //! reduction ordering.
  //!
  //! \tparam Iterator the type of iterators that are the parameters.
  //!
  //! \param first1 beginning iterator of first object for comparison.
  //! \param last1 ending iterator of first object for comparison.
  //! \param first2 beginning iterator of second object for comparison.
  //! \param last2 ending iterator of second object for comparison.
  //!
  //! \returns The boolean value \c true if the range `[first1, last1)` is
  //! len-lex less than the range `[first2, last2)`, and \c false
  //! otherwise.
  //!
  //! \exceptions
  //! Throws if std::lexicographical_compare does.
  //!
  //! \complexity
  //! At most \f$O(n)\f$ where \f$n\f$ is the minimum of the distance between
  //! \p last1 and \p first1, and the distance between \p last2 and \p first2.
  //!
  //! \par Possible Implementation
  //! \code_no_test
  //! template <typename Iterator>
  //! bool lenlex_cmp(Iterator first1,
  //!                       Iterator last1,
  //!                       Iterator first2,
  //!                       Iterator last2) {
  //!   return (last1 - first1) < (last2 - first2)
  //!          || ((last1 - first1) == (last2 - first2)
  //!              && std::lexicographical_compare
  //!                   (first1, last1, first2, last2));
  //! }
  //! \end_code_no_test
  template <typename Iterator,
            typename = std::enable_if_t<!rx::is_input_or_sink_v<Iterator>>>
  [[nodiscard]] bool
  lenlex_cmp(Iterator first1, Iterator last1, Iterator first2, Iterator last2) {
    return (last1 - first1) < (last2 - first2)
           || ((last1 - first1) == (last2 - first2)
               && std::lexicographical_compare(first1, last1, first2, last2));
  }

  //! \brief Compare two objects of the same type using \ref lenlex_cmp.
  //!
  //! Defined in `order.hpp`.
  //!
  //! This function compares two objects of the same type using
  //! \ref lenlex_cmp.
  //!
  //! \tparam Thing the type of the objects to be compared.
  //!
  //! \param x const reference to the first object for comparison.
  //! \param y const reference to the second object for comparison.
  //!
  //! \returns The boolean value \c true if \p x is len-lex less than \p y,
  //! and \c false otherwise.
  //!
  //! \exceptions
  //! See \ref lenlex_cmp(Iterator, Iterator, Iterator, Iterator).
  //!
  //! \complexity
  //! At most \f$O(n)\f$ where \f$n\f$ is the minimum of the length of \p x and
  //! the length of \p y.
  //!
  //! \par Possible Implementation
  //! \code_no_test
  //! lenlex_cmp(x.cbegin(), x.cend(), y.cbegin(), y.cend());
  //! \end_code_no_test
  //!
  //! \sa
  //! lenlex_cmp(Iterator, Iterator, Iterator, Iterator).
  template <typename Thing,
            typename = std::enable_if_t<!rx::is_input_or_sink_v<Thing>>>
  [[nodiscard]] bool lenlex_cmp(Thing const& x, Thing const& y) {
    return lenlex_cmp(x.cbegin(), x.cend(), y.cbegin(), y.cend());
  }

  //! \brief Compare two objects via their pointers using \ref lenlex_cmp.
  //!
  //! Defined in `order.hpp`.
  //!
  //! This function compares two objects via their pointers using
  //! \ref lenlex_cmp.
  //!
  //! \tparam Thing the type of the objects to be compared.
  //!
  //! \param x pointer to the first object for comparison.
  //! \param y pointer to the second object for comparison.
  //!
  //! \returns The boolean value \c true if \p x points to a word len-lex less
  //! than the word pointed to by \p y, and \c false otherwise.
  //!
  //! \exceptions
  //! See \ref lenlex_cmp(Iterator, Iterator, Iterator, Iterator).
  //!
  //! \complexity
  //! At most \f$O(n)\f$ where \f$n\f$ is the minimum of the length of the word
  //! pointed to by \p x and the length of word pointed to by \p y.
  //!
  //! \par Possible Implementation
  //! \code_no_test
  //! lenlex_cmp(x->cbegin(), x->cend(), y->cbegin(), y->cend());
  //! \end_code_no_test
  //!
  //! \sa
  //! lenlex_cmp(Iterator, Iterator, Iterator, Iterator).
  template <typename Thing>
  [[nodiscard]] bool lenlex_cmp(Thing* const x, Thing* const y) {
    return lenlex_cmp(x->cbegin(), x->cend(), y->cbegin(), y->cend());
  }

  //! \brief A stateless struct with binary call operator using
  //! \ref lenlex_cmp.
  //!
  //! Defined in `order.hpp`.
  //!
  //! A stateless struct with binary call operator using \ref lenlex_cmp.
  //!
  //! This only exists to be used as a template parameter, and has no
  //! advantages over using \ref lenlex_cmp otherwise.
  //!
  //! \sa
  //! lenlex_cmp(Iterator, Iterator, Iterator, Iterator)
  struct LenLexCmp {
    //! \brief Call operator that compares \p x and \p y using
    //! \ref lenlex_cmp.
    //!
    //! Call operator that compares \p x and \p y using \ref lenlex_cmp.
    //!
    //! \tparam Thing the type of the objects to be compared.
    //!
    //! \param x const reference to the first object for comparison.
    //! \param y const reference to the second object for comparison.
    //!
    //! \returns The boolean value \c true if \p x is len-lex less than \p y,
    //! and \c false otherwise.
    //!
    //! \exceptions
    //! See \ref lenlex_cmp(Iterator, Iterator, Iterator, Iterator).
    //!
    //! \complexity
    //! See \ref lenlex_cmp(Iterator, Iterator, Iterator, Iterator).
    template <typename Thing>
    [[nodiscard]] bool operator()(Thing const& x, Thing const& y) const {
      return lenlex_cmp(x, y);
    }
  };

  //////////////////////////////////////////////////////////////////////
  // Short-lex - deprecated
  //////////////////////////////////////////////////////////////////////

  //! \brief Compare two objects of the same type using the short-lex reduction
  //! ordering.
  //!
  //! Defined in `order.hpp`.
  //!
  //! This function compares two objects of the same type using the short-lex
  //! reduction ordering.
  //!
  //! \tparam Iterator the type of iterators that are the parameters.
  //!
  //! \param first1 beginning iterator of first object for comparison.
  //! \param last1 ending iterator of first object for comparison.
  //! \param first2 beginning iterator of second object for comparison.
  //! \param last2 ending iterator of second object for comparison.
  //!
  //! \returns The boolean value \c true if the range `[first1, last1)` is
  //! short-lex less than the range `[first2, last2)`, and \c false
  //! otherwise.
  //!
  //! \exceptions
  //! Throws if std::lexicographical_compare does.
  //!
  //! \complexity
  //! At most \f$O(n)\f$ where \f$n\f$ is the minimum of the distance between
  //! \p last1 and \p first1, and the distance between \p last2 and \p first2.
  //!
  //! \par Possible Implementation
  //! \code_no_test
  //! template <typename Iterator>
  //! bool shortlex_compare(Iterator first1,
  //!                       Iterator last1,
  //!                       Iterator first2,
  //!                       Iterator last2) {
  //!   return (last1 - first1) < (last2 - first2)
  //!          || ((last1 - first1) == (last2 - first2)
  //!              && std::lexicographical_compare
  //!                   (first1, last1, first2, last2));
  //! }
  //! \end_code_no_test
  //!
  //! \deprecated_warning{function} Use \ref lenlex_cmp instead.
  template <typename Iterator,
            typename = std::enable_if_t<!rx::is_input_or_sink_v<Iterator>>>
  [[nodiscard]] [[deprecated("Use lenlex_cmp instead!")]] bool
  shortlex_compare(Iterator first1,
                   Iterator last1,
                   Iterator first2,
                   Iterator last2) {
    return lenlex_cmp(first1, last1, first2, last2);
  }

  //! \brief Compare two objects of the same type using \ref shortlex_compare.
  //!
  //! Defined in `order.hpp`.
  //!
  //! This function compares two objects of the same type using
  //! \ref shortlex_compare.
  //!
  //! \tparam Thing the type of the objects to be compared.
  //!
  //! \param x const reference to the first object for comparison.
  //! \param y const reference to the second object for comparison.
  //!
  //! \returns The boolean value \c true if \p x is short-lex less than \p y,
  //! and \c false otherwise.
  //!
  //! \exceptions
  //! See \ref shortlex_compare(Iterator, Iterator, Iterator, Iterator).
  //!
  //! \complexity
  //! At most \f$O(n)\f$ where \f$n\f$ is the minimum of the length of \p x and
  //! the length of \p y.
  //!
  //! \par Possible Implementation
  //! \code_no_test
  //! shortlex_compare(
  //!   x.cbegin(), x.cend(), y.cbegin(), y.cend());
  //! \end_code_no_test
  //!
  //! \sa
  //! shortlex_compare(Iterator, Iterator, Iterator, Iterator).
  //!
  //! \deprecated_warning{function} Use \ref lenlex_cmp instead.
  template <typename Thing,
            typename = std::enable_if_t<!rx::is_input_or_sink_v<Thing>>>
  [[nodiscard]] [[deprecated("Use lenlex_cmp instead!")]] bool
  shortlex_compare(Thing const& x, Thing const& y) {
    return shortlex_compare(x.cbegin(), x.cend(), y.cbegin(), y.cend());
  }

  //! \brief Compare two objects via their pointers using \ref shortlex_compare.
  //!
  //! Defined in `order.hpp`.
  //!
  //! This function compares two objects via their pointers using
  //! \ref shortlex_compare.
  //!
  //! \tparam Thing the type of the objects to be compared.
  //!
  //! \param x pointer to the first object for comparison.
  //! \param y pointer to the second object for comparison.
  //!
  //! \returns The boolean value \c true if \p x points to a word short-lex less
  //! than the word pointed to by \p y, and \c false otherwise.
  //!
  //! \exceptions
  //! See \ref shortlex_compare(Iterator, Iterator, Iterator, Iterator).
  //!
  //! \complexity
  //! At most \f$O(n)\f$ where \f$n\f$ is the minimum of the length of the word
  //! pointed to by \p x and the length of word pointed to by \p y.
  //!
  //! \par Possible Implementation
  //! \code_no_test
  //! shortlex_compare(
  //!   x->cbegin(), x->cend(), y->cbegin(), y->cend());
  //! \end_code_no_test
  //!
  //! \sa
  //! shortlex_compare(Iterator, Iterator, Iterator, Iterator).
  //!
  //! \deprecated_warning{function} Use \ref lenlex_cmp instead.
  template <typename Thing>
  [[nodiscard]] [[deprecated("Use lenlex_cmp instead!")]] bool
  shortlex_compare(Thing* const x, Thing* const y) {
    return shortlex_compare(x->cbegin(), x->cend(), y->cbegin(), y->cend());
  }

  //! \brief A stateless struct with binary call operator using
  //! \ref shortlex_compare.
  //!
  //! Defined in `order.hpp`.
  //!
  //! A stateless struct with binary call operator using \ref shortlex_compare.
  //!
  //! This only exists to be used as a template parameter, and has no
  //! advantages over using \ref shortlex_compare otherwise.
  //!
  //! \sa
  //! shortlex_compare(Iterator, Iterator, Iterator, Iterator)
  //!
  //! \deprecated_warning{struct} Use \ref LenLexCmp instead.
  using ShortLexCompare [[deprecated("Use LenLexCmp instead!")]] = LenLexCmp;

  //////////////////////////////////////////////////////////////////////
  // Recursive path order (RPO)
  //////////////////////////////////////////////////////////////////////

  //! \brief Compare two objects of the same type using the recursive path
  //! comparison.
  //!
  //! Defined in `order.hpp`.
  //!
  //! This function compares two objects of the same type using the recursive
  //! path comparison described in \cite Jantzen2012aa (Definition 1.2.14, page
  //! 24).
  //!
  //! If \f$u, v\in X ^ {*}\f$, then
  //! \f$u < v\f$ if and only if one of the following conditions holds:
  //! 1. \f$u\f$ is empty and \f$v\f$ is not empty; or
  //! 2. \f$u = au'\f$ and \f$v = bv'\f$ for some \f$a,b \in X\f$, \f$u',v'\in
  //!    X ^ {*}\f$ and:
  //!   1. \f$a = b\f$ and \f$u' < v'\f$; or
  //!   2. \f$a < b\f$ and \f$u  < v'\f$; or
  //!   3. \f$a > b\f$ and \f$u' < v\f$.
  //!
  //! This documentation and the implementation of \ref recursive_path_compare
  //! is based on the source code of \cite Holt2018aa, specifically the function
  //! `rec_compare`.
  //!
  //! \tparam Iterator the type of iterators that are the arguments.
  //!
  //! \param first1 beginning iterator of first object for comparison.
  //! \param last1 ending iterator of first object for comparison.
  //! \param first2 beginning iterator of second object for comparison.
  //! \param last2 ending iterator of second object for comparison.
  //!
  //! \returns The boolean value \c true if the range `[first1, last1)` is less
  //! than the range `[first2, last2)` with respect to the recursive path
  //! ordering, and \c false otherwise.
  //!
  //! \exceptions
  //! \noexcept
  //!
  //! \warning
  //! This function has significantly worse performance than all
  //! the variants of \ref lenlex_cmp and std::lexicographical_compare.
  template <typename Iterator,
            typename = std::enable_if_t<!rx::is_input_or_sink_v<Iterator>>>
  [[nodiscard]] bool rpo_cmp(Iterator first1,
                             Iterator last1,
                             Iterator first2,
                             Iterator last2) noexcept;

  //! \brief Compare two objects of the same type using //! \ref rpo_cmp.
  //!
  //! Defined in `order.hpp`.
  //!
  //! This function compares two objects of the same type using \ref rpo_cmp.
  //!
  //! \tparam Thing the type of the objects to be compared.
  //!
  //! \param x const reference to the first object for comparison.
  //! \param y const reference to the second object for comparison.
  //!
  //! \returns The boolean value \c true if \p x is less than \p y with respect
  //! to the recursive path ordering, and \c false otherwise.
  //!
  //! \exceptions
  //! \noexcept
  //!
  //! \par Possible Implementation
  //! \code_no_test
  //! rpo_cmp(x.cbegin(), x.cend(), y.cbegin(), y.cend());
  //! \end_code_no_test
  //!
  //! \sa
  //! rpo_cmp(Iterator, Iterator, Iterator, Iterator)
  template <typename Thing,
            typename = std::enable_if_t<!rx::is_input_or_sink_v<Thing>>>
  [[nodiscard]] bool rpo_cmp(Thing const& x, Thing const& y) noexcept {
    return rpo_cmp(x.cbegin(), x.cend(), y.cbegin(), y.cend());
  }

  //! \brief Compare two objects via their pointers using \ref rpo_cmp.
  //!
  //! Defined in `order.hpp`.
  //!
  //! This function compares two objects via their pointers using \ref rpo_cmp.
  //!
  //! \tparam Thing the type of the objects to be compared.
  //!
  //! \param x pointer to the first object for comparison.
  //! \param y pointer to the second object for comparison.
  //!
  //! \returns The boolean value \c true if the value pointed to by \p x is less
  //! than the value pointed to by \p y with r to the recursive path
  //! ordering, and \c false otherwise.
  //!
  //! \exceptions
  //! \noexcept
  //!
  //! \par Possible Implementation
  //! \code_no_test
  //! rpo_cmp(x->cbegin(), x->cend(), y->cbegin(), y->cend());
  //! \end_code_no_test
  //!
  //! \sa
  //! rpo_cmp(Iterator, Iterator, Iterator, Iterator)
  template <typename Thing>
  [[nodiscard]] bool rpo_cmp(Thing* const x, Thing* const y) noexcept {
    return rpo_cmp(x->cbegin(), x->cend(), y->cbegin(), y->cend());
  }

  //! \brief A stateless struct with binary call operator using
  //! \ref rpo_cmp.
  //!
  //! Defined in `order.hpp`.
  //!
  //! A stateless struct with binary call operator using
  //! \ref rpo_cmp.
  //!
  //! This only exists to be used as a template parameter, and has no
  //! advantages over using \ref rpo_cmp otherwise.
  //!
  //! \sa
  //! rpo_cmp(Iterator, Iterator, Iterator, Iterator)
  struct RPOCmp {
    //! \brief  Call operator that compares \p x and \p y using
    //! \ref rpo_cmp.
    //!
    //! Call operator that compares \p x and \p y using
    //! \ref rpo_cmp.
    //!
    //! \tparam Thing the type of the objects to be compared.
    //!
    //! \param x const reference to the first object for comparison.
    //! \param y const reference to the second object for comparison.
    //!
    //! \returns The boolean value \c true if \p x is less than \p y with
    //! respect to the recursive path ordering, and \c false otherwise.
    //!
    //! \exceptions
    //! \noexcept
    template <typename Thing>
    [[nodiscard]] bool operator()(Thing const& x,
                                  Thing const& y) const noexcept {
      return rpo_cmp(x, y);
    }  // namespace libsemigroups
  };

  //////////////////////////////////////////////////////////////////////
  // Reversed recursive path order (RPO)
  //////////////////////////////////////////////////////////////////////

  //! \brief Compare two objects of the same type using the reversed recursive
  //! path comparison.
  //!
  //! Defined in `order.hpp`.
  //!
  //! This function compares two objects of the same type using the reversed
  //! recursive path comparison. This is the same as applying the recursive path
  //! comparison described in \cite Jantzen2012aa (Definition 1.2.14, page 24)
  //! to the reversed words in `[first1, last1)`, `[first2, last2)`.
  //!
  //! If \f$u, v\in X ^ {*}\f$, then
  //! \f$u < v\f$ if and only if one of the following conditions holds:
  //! 1. \f$u\f$ is empty and \f$v\f$ is not empty; or
  //! 2. \f$u = u'a\f$ and \f$v = v'b\f$ for some \f$a,b \in X\f$, \f$u',v'\in
  //!    X ^ {*}\f$ and:
  //!   1. \f$a = b\f$ and \f$u' < v'\f$; or
  //!   2. \f$a < b\f$ and \f$u  < v'\f$; or
  //!   3. \f$a > b\f$ and \f$u' < v\f$.
  //!
  //! This documentation and the implementation of \ref recursive_path_compare
  //! is based on the source code of \cite Holt2018aa, specifically the function
  //! `rt_rec_compare`.
  //!
  //! \tparam Iterator the type of iterators that are the arguments.
  //!
  //! \param first1 beginning iterator of first object for comparison.
  //! \param last1 ending iterator of first object for comparison.
  //! \param first2 beginning iterator of second object for comparison.
  //! \param last2 ending iterator of second object for comparison.
  //!
  //! \returns The boolean value \c true if the range `[first1, last1)` is less
  //! than the range `[first2, last2)` with respect to the reversed recursive
  //! path ordering, and \c false otherwise.
  //!
  //! \exceptions
  //! \noexcept
  //!
  //! \warning
  //! This function has significantly worse performance than all
  //! the variants of \ref lenlex_cmp and std::lexicographical_compare.
  template <typename Iterator,
            typename = std::enable_if_t<!rx::is_input_or_sink_v<Iterator>>>
  [[nodiscard]] bool rev_rpo_cmp(Iterator first1,
                                 Iterator last1,
                                 Iterator first2,
                                 Iterator last2) noexcept;

  //! \brief Compare two objects of the same type using \ref rev_rpo_cmp.
  //!
  //! Defined in `order.hpp`.
  //!
  //! This function compares two objects of the same type using
  //! \ref rev_rpo_cmp.
  //!
  //! \tparam Thing the type of the objects to be compared.
  //!
  //! \param x const reference to the first object for comparison.
  //! \param y const reference to the second object for comparison.
  //!
  //! \returns The boolean value \c true if \p x is less than \p y with respect
  //! to the reversed recursive path ordering, and \c false otherwise.
  //!
  //! \exceptions
  //! \noexcept
  //!
  //! \par Possible Implementation
  //! \code_no_test
  //! rev_rpo_cmp(x.cbegin(), x.cend(), y.cbegin(), y.cend());
  //! \end_code_no_test
  //!
  //! \sa
  //! rev_rpo_cmp(Iterator, Iterator, Iterator, Iterator)
  template <typename Iterator,
            typename = std::enable_if_t<!rx::is_input_or_sink_v<Iterator>>>
  [[nodiscard]] bool rev_rpo_cmp(Iterator const& x,
                                 Iterator const& y) noexcept {
    return rev_rpo_cmp(x.cbegin(), x.cend(), y.cbegin(), y.cend());
  }

  //! \brief Compare two objects via their pointers using \ref rev_rpo_cmp.
  //!
  //! Defined in `order.hpp`.
  //!
  //! This function compares two objects via their pointers using
  //! \ref rev_rpo_cmp.
  //!
  //! \tparam Thing the type of the objects to be compared.
  //!
  //! \param x pointer to the first object for comparison.
  //! \param y pointer to the second object for comparison.
  //!
  //! \returns The boolean value \c true if the value pointed to by \p x is less
  //! than the value pointed to by \p y with r to the reversed recursive path
  //! ordering, and \c false otherwise.
  //!
  //! \exceptions
  //! \noexcept
  //!
  //! \par Possible Implementation
  //! \code_no_test
  //! rev_rpo_cmp(x->cbegin(), x->cend(), y->cbegin(), y->cend());
  //! \end_code_no_test
  //!
  //! \sa
  //! rev_rpo_cmp(Iterator, Iterator, Iterator, Iterator)
  template <typename Thing>
  [[nodiscard]] bool rev_rpo_cmp(Thing* const x, Thing* const y) noexcept {
    return rev_rpo_cmp(x->cbegin(), x->cend(), y->cbegin(), y->cend());
  }

  //! \brief A stateless struct with binary call operator using
  //! \ref rev_rpo_cmp.
  //!
  //! Defined in `order.hpp`.
  //!
  //! A stateless struct with binary call operator using
  //! \ref rev_rpo_cmp.
  //!
  //! This only exists to be used as a template parameter, and has no
  //! advantages over using \ref rev_rpo_cmp otherwise.
  //!
  //! \sa
  //! rev_rpo_cmp(Iterator, Iterator, Iterator, Iterator)
  struct RevRPOCmp {
    //! \brief  Call operator that compares \p x and \p y using
    //! \ref rev_rpo_cmp.
    //!
    //! Call operator that compares \p x and \p y using
    //! \ref rev_rpo_cmp.
    //!
    //! \tparam Thing the type of the objects to be compared.
    //!
    //! \param x const reference to the first object for comparison.
    //! \param y const reference to the second object for comparison.
    //!
    //! \returns The boolean value \c true if \p x is less than \p y with
    //! respect to the reversed recursive path ordering, and \c false otherwise.
    //!
    //! \exceptions
    //! \noexcept
    template <typename Thing>
    [[nodiscard]] bool operator()(Thing const& x,
                                  Thing const& y) const noexcept {
      return rev_rpo_cmp(x, y);
    }  // namespace libsemigroups
  };

  //////////////////////////////////////////////////////////////////////
  // Recursive path order (RPO) - deprecated
  //////////////////////////////////////////////////////////////////////

  //! \brief Compare two objects of the same type using the reversed recursive
  //! path comparison.
  //!
  //! Defined in `order.hpp`.
  //!
  //! This function compares two objects of the same type using the reversed
  //! recursive path comparison. This is the same as applying the recursive path
  //! comparison described in \cite Jantzen2012aa (Definition 1.2.14, page 24)
  //! to the reversed words in `[first1, last1)`, `[first2, last2)`.
  //!
  //! If \f$u, v\in X ^ {*}\f$, then
  //! \f$u < v\f$ if and only if one of the following conditions holds:
  //! 1. \f$u\f$ is empty and \f$v\f$ is not empty; or
  //! 2. \f$u = u'a\f$ and \f$v = v'b\f$ for some \f$a,b \in X\f$, \f$u',v'\in
  //!    X ^ {*}\f$ and:
  //!   1. \f$a = b\f$ and \f$u' < v'\f$; or
  //!   2. \f$a < b\f$ and \f$u  < v'\f$; or
  //!   3. \f$a > b\f$ and \f$u' < v\f$.
  //!
  //! This documentation and the implementation of \ref recursive_path_compare
  //! is based on the source code of \cite Holt2018aa, specifically the function
  //! `rt_rec_compare`.
  //!
  //! \tparam Iterator the type of iterators that are the arguments.
  //!
  //! \param first1 beginning iterator of first object for comparison.
  //! \param last1 ending iterator of first object for comparison.
  //! \param first2 beginning iterator of second object for comparison.
  //! \param last2 ending iterator of second object for comparison.
  //!
  //! \returns The boolean value \c true if the range `[first1, last1)` is less
  //! than the range `[first2, last2)` with respect to the reversed recursive
  //! path ordering, and \c false otherwise.
  //!
  //! \exceptions
  //! \noexcept
  //!
  //! \warning
  //! This function has significantly worse performance than all
  //! the variants of \ref lenlex_cmp and std::lexicographical_compare.
  //!
  //! \deprecated_warning{function} Use \ref rev_rpo_cmp instead.
  template <typename Iterator,
            typename = std::enable_if_t<!rx::is_input_or_sink_v<Iterator>>>
  [[nodiscard]] [[deprecated("Use rev_rpo_cmp instead!")]] bool
  recursive_path_compare(Iterator first1,
                         Iterator last1,
                         Iterator first2,
                         Iterator last2) noexcept {
    return rev_rpo_cmp(first1, last1, first2, last2);
  }

  //! \brief Compare two objects of the same type using
  //! \ref recursive_path_compare.
  //!
  //! Defined in `order.hpp`.
  //!
  //! This function compares two objects of the same type using
  //! \ref recursive_path_compare.
  //!
  //! \tparam Thing the type of the objects to be compared.
  //!
  //! \param x const reference to the first object for comparison.
  //! \param y const reference to the second object for comparison.
  //!
  //! \returns The boolean value \c true if \p x is less than \p y with respect
  //! to the recursive path ordering, and \c false otherwise.
  //!
  //! \exceptions
  //! \noexcept
  //!
  //! \par Possible Implementation
  //! \code_no_test
  //! recursive_path_compare(
  //!   x.cbegin(), x.cend(), y.cbegin(), y.cend());
  //! \end_code_no_test
  //!
  //! \sa
  //! recursive_path_compare(Iterator, Iterator, Iterator, Iterator)
  //!
  //! \deprecated_warning{function} Use \ref rev_rpo_cmp instead.
  template <typename Thing,
            typename = std::enable_if_t<!rx::is_input_or_sink_v<Thing>>>
  [[nodiscard]] [[deprecated("Use rev_rpo_cmp instead!")]] bool
  recursive_path_compare(Thing const& x, Thing const& y) noexcept {
    return recursive_path_compare(x.cbegin(), x.cend(), y.cbegin(), y.cend());
  }

  //! \brief Compare two objects via their pointers using
  //! \ref recursive_path_compare.
  //!
  //! Defined in `order.hpp`.
  //!
  //! This function compares two objects via their pointers using
  //! \ref recursive_path_compare.
  //!
  //! \tparam Thing the type of the objects to be compared.
  //!
  //! \param x pointer to the first object for comparison.
  //! \param y pointer to the second object for comparison.
  //!
  //! \returns The boolean value \c true if the value pointed to by \p x is less
  //! than the value pointed to by \p y with r to the recursive path
  //! ordering, and \c false otherwise.
  //!
  //! \exceptions
  //! \noexcept
  //!
  //! \par Possible Implementation
  //! \code_no_test
  //! recursive_path_compare(
  //!   x->cbegin(), x->cend(), y->cbegin(), y->cend());
  //! \end_code_no_test
  //!
  //! \sa
  //! recursive_path_compare(Iterator, Iterator, Iterator, Iterator)
  //!
  //! \deprecated_warning{function} Use \ref rev_rpo_cmp instead.
  template <typename Thing>
  [[nodiscard]] [[deprecated("Use rev_rpo_cmp instead!")]] bool
  recursive_path_compare(Thing* const x, Thing* const y) noexcept {
    return recursive_path_compare(
        x->cbegin(), x->cend(), y->cbegin(), y->cend());
  }

  //! \brief A stateless struct with binary call operator using
  //! \ref recursive_path_compare.
  //!
  //! Defined in `order.hpp`.
  //!
  //! A stateless struct with binary call operator using
  //! \ref recursive_path_compare.
  //!
  //! This only exists to be used as a template parameter, and has no
  //! advantages over using \ref recursive_path_compare otherwise.
  //!
  //! \sa
  //! recursive_path_compare(Iterator, Iterator, Iterator, Iterator)
  //!
  //! \deprecated_warning{struct} Use \ref RevRPOCmp instead.
  using RecursivePathCompare [[deprecated("Use RevRPOCmp instead!")]]
  = RevRPOCmp;

  //////////////////////////////////////////////////////////////////////
  // Weighted len-lex
  //////////////////////////////////////////////////////////////////////

  //! \brief Compare two objects of the same type using the weighted len-lex
  //! ordering without checks.
  //!
  //! Defined in `order.hpp`.
  //!
  //! This function compares two objects of the same type using the weighted
  //! len-lex ordering. The weight of a word is computed by adding up the
  //! weights of the letters in the word, where the `i`th index of the weights
  //! vector corresponds to the weight of the `i`th letter in the alphabet.
  //! Heavier words come later in the ordering than all lighter words. Amongst
  //! words of equal weight, len-lex ordering is used.
  //!
  //! \tparam Iterator the type of iterators that are the arguments.
  //!
  //! \param first1 beginning iterator of first object for comparison.
  //! \param last1 ending iterator of first object for comparison.
  //! \param first2 beginning iterator of second object for comparison.
  //! \param last2 ending iterator of second object for comparison.
  //! \param weights the weights vector.
  //!
  //! \returns The boolean value \c true if the range `[first1, last1)` is
  //! weighted len-lex less than the range `[first2, last2)`, and \c false
  //! otherwise.
  //!
  //! \exceptions
  //! Throws if std::lexicographical_compare does.
  //!
  //! \complexity
  //! At most \f$O(n + m)\f$ where \f$n\f$ is the distance between \p last1
  //! and \p first1, and \f$m\f$ is the distance between \p last2 and
  //! \p first2.
  //!
  //! \warning
  //! It is not checked that the letters in the ranges are valid indices into
  //! the \p weights vector.
  //!
  //! \sa
  //! wt_lenlex_cmp(Iterator, Iterator, Iterator, Iterator,std::vector<size_t>
  //! const&).
  template <typename Iterator,
            typename = std::enable_if_t<!rx::is_input_or_sink_v<Iterator>>>
  [[nodiscard]] bool
  wt_lenlex_cmp_no_checks(Iterator                   first1,
                          Iterator                   last1,
                          Iterator                   first2,
                          Iterator                   last2,
                          std::vector<size_t> const& weights);

  //! \brief Compare two objects of the same type using
  //! \ref wt_lenlex_cmp_no_checks without checks.
  //!
  //! Defined in `order.hpp`.
  //!
  //! This function compares two objects of the same type using
  //! \ref wt_lenlex_cmp_no_checks, where the `i`th index of the weights
  //! vector corresponds to the weight of the `i`th letter in the alphabet.
  //!
  //! \tparam Thing the type of the objects to be compared.
  //!
  //! \param x const reference to the first object for comparison.
  //! \param y const reference to the second object for comparison.
  //! \param weights the weights vector.
  //!
  //! \returns The boolean value \c true if \p x is weighted len-lex less
  //! than \p y, and \c false otherwise.
  //!
  //! \exceptions
  //! See \ref wt_lenlex_cmp_no_checks(Iterator, Iterator, Iterator,
  //! Iterator, std::vector<size_t> const&).
  //!
  //! \complexity
  //! At most \f$O(n + m)\f$ where \f$n\f$ is the length of \p x and \f$m\f$
  //! is the length of \p y.
  //!
  //! \par Possible Implementation
  //! \code_no_test
  //! wt_lenlex_cmp_no_checks(
  //!   x.cbegin(), x.cend(), y.cbegin(), y.cend(), weights);
  //! \end_code_no_test
  //!
  //! \warning
  //! It is not checked that the letters in \p x and \p y are valid indices
  //! into the weights vector.
  //!
  //! \sa
  //! wt_lenlex_cmp_no_checks(Iterator, Iterator, Iterator, Iterator,
  //! std::vector<size_t> const&).
  template <typename Thing,
            typename = std::enable_if_t<!rx::is_input_or_sink_v<Thing>>>
  [[nodiscard]] bool
  wt_lenlex_cmp_no_checks(Thing const&               x,
                          Thing const&               y,
                          std::vector<size_t> const& weights) {
    return wt_lenlex_cmp_no_checks(
        x.cbegin(), x.cend(), y.cbegin(), y.cend(), weights);
  }

  //! \brief Compare two objects via their pointers using
  //! \ref wt_lenlex_cmp_no_checks without checks.
  //!
  //! Defined in `order.hpp`.
  //!
  //! This function compares two objects via their pointers using
  //! \ref wt_lenlex_cmp_no_checks, where the `i`th index of the weights
  //! vector corresponds to the weight of the `i`th letter in the alphabet.
  //!
  //! \tparam Thing the type of the objects to be compared.
  //!
  //! \param x pointer to the first object for comparison.
  //! \param y pointer to the second object for comparison.
  //! \param weights the weights vector.
  //!
  //! \returns The boolean value \c true if \p x points to a word weighted
  //! len-lex less than the word pointed to by \p y, and \c false otherwise.
  //!
  //! \exceptions
  //! See \ref wt_lenlex_cmp_no_checks(Iterator, Iterator, Iterator,
  //! Iterator, std::vector<size_t> const&).
  //!
  //! \complexity
  //! At most \f$O(n + m)\f$ where \f$n\f$ is the length of the word pointed
  //! to by \p x and \f$m\f$ is the length of word pointed to by \p y.
  //!
  //! \par Possible Implementation
  //! \code_no_test
  //! wt_lenlex_cmp_no_checks(
  //!   x->cbegin(), x->cend(), y->cbegin(), y->cend(), weights);
  //! \end_code_no_test
  //!
  //! \warning
  //! It is not checked that the letters are valid indices into the weights
  //! vector.
  //!
  //! \sa
  //! wt_lenlex_cmp_no_checks(Iterator, Iterator, Iterator, Iterator,
  //! std::vector<size_t> const&).
  template <typename Thing>
  [[nodiscard]] bool
  wt_lenlex_cmp_no_checks(Thing* const               x,
                          Thing* const               y,
                          std::vector<size_t> const& weights) {
    return wt_lenlex_cmp_no_checks(
        x->cbegin(), x->cend(), y->cbegin(), y->cend(), weights);
  }

  //! \brief Compare two objects of the same type using the weighted len-lex
  //! ordering and check validity.
  //!
  //! Defined in `order.hpp`.
  //!
  //! This function compares two objects of the same type using the weighted
  //! len-lex ordering. The weight of a word is computed by adding up the
  //! weights of the letters in the word, where the `i`th index of the weights
  //! vector corresponds to the weight of the `i`th letter in the alphabet.
  //! Heavier words come later in the ordering than all lighter words. Amongst
  //! words of equal weight, len-lex ordering is used.
  //!
  //! After checking that all letters in both ranges are valid indices into
  //! the weights vector, this function performs the same as
  //! `wt_lenlex_cmp_no_checks(first1, last1, first2, last2, weights)`.
  //!
  //! \tparam Iterator the type of iterators to the first object to be compared.
  //!
  //! \param first1 beginning iterator of first object for comparison.
  //! \param last1 ending iterator of first object for comparison.
  //! \param first2 beginning iterator of second object for comparison.
  //! \param last2 ending iterator of second object for comparison.
  //! \param weights the weights vector.
  //!
  //! \returns The boolean value \c true if the range `[first1, last1)` is
  //! weighted len-lex less than the range `[first2, last2)`, and \c false
  //! otherwise.
  //!
  //! \throws LibsemigroupsException if any letter in either range is not a
  //! valid index into the weights vector (i.e., if any letter is greater
  //! than or equal to `weights.size()`).
  //!
  //! \complexity
  //! At most \f$O(n + m)\f$ where \f$n\f$ is the distance between \p last1
  //! and \p first1, and \f$m\f$ is the distance between \p last2 and
  //! \p first2.
  //!
  //! \sa
  //! wt_lenlex_cmp_no_checks(Iterator, Iterator, Iterator, Iterator,
  //! std::vector<size_t> const&).
  template <typename Iterator,
            typename = std::enable_if_t<!rx::is_input_or_sink_v<Iterator>>>
  [[nodiscard]] bool wt_lenlex_cmp(Iterator                   first1,
                                   Iterator                   last1,
                                   Iterator                   first2,
                                   Iterator                   last2,
                                   std::vector<size_t> const& weights);

  //! \brief Compare two objects of the same type using \ref wt_lenlex_cmp
  //! and check validity.
  //!
  //! Defined in `order.hpp`.
  //!
  //! This function compares two objects of the same type using
  //! \ref wt_lenlex_cmp, where the `i`th index of the weights vector
  //! corresponds to the weight of the `i`th letter in the alphabet.
  //!
  //! After checking that all letters in both objects are valid indices into
  //! the weights vector, this function performs the same as
  //! `wt_lenlex_cmp_no_checks(x, y, weights)`.
  //!
  //! \tparam Thing the type of the objects to be compared.
  //!
  //! \param x const reference to the first object for comparison.
  //! \param y const reference to the second object for comparison.
  //! \param weights the weights vector.
  //!
  //! \returns The boolean value \c true if \p x is weighted len-lex less
  //! than \p y, and \c false otherwise.
  //!
  //! \throws LibsemigroupsException if any letter in \p x or \p y is not a
  //! valid index into the weights vector.
  //!
  //! \complexity
  //! At most \f$O(n + m)\f$ where \f$n\f$ is the length of \p x and \f$m\f$
  //! is the length of \p y.
  //!
  //! \par Possible Implementation
  //! \code_no_test
  //! wt_lenlex_cmp(
  //!   x.cbegin(), x.cend(), y.cbegin(), y.cend(), weights);
  //! \end_code_no_test
  //!
  //! \sa
  //! wt_lenlex_cmp(Iterator, Iterator, Iterator, Iterator,
  //! std::vector<size_t> const&).
  template <typename Thing,
            typename = std::enable_if_t<!rx::is_input_or_sink_v<Thing>>>
  [[nodiscard]] bool wt_lenlex_cmp(Thing const&               x,
                                   Thing const&               y,
                                   std::vector<size_t> const& weights) {
    return wt_lenlex_cmp(x.cbegin(), x.cend(), y.cbegin(), y.cend(), weights);
  }

  //! \brief Compare two objects via their pointers using
  //! \ref wt_lenlex_cmp and check validity.
  //!
  //! Defined in `order.hpp`.
  //!
  //! This function compares two objects via their pointers using
  //! \ref wt_lenlex_cmp, where the `i`th index of the weights vector
  //! corresponds to the weight of the `i`th letter in the alphabet.
  //!
  //! After checking that all letters are valid indices into the weights
  //! vector, this function performs the same as
  //! `wt_lenlex_cmp_no_checks(*x, *y, weights)`.
  //!
  //! \tparam Thing the type of the objects to be compared.
  //!
  //! \param x pointer to the first object for comparison.
  //! \param y pointer to the second object for comparison.
  //! \param weights the weights vector.
  //!
  //! \returns The boolean value \c true if \p x points to a word weighted
  //! len-lex less than the word pointed to by \p y, and \c false otherwise.
  //!
  //! \throws LibsemigroupsException if any letter is not a valid index into
  //! the weights vector.
  //!
  //! \complexity
  //! At most \f$O(n + m)\f$ where \f$n\f$ is the length of the word pointed
  //! to by \p x and \f$m\f$ is the length of word pointed to by \p y.
  //!
  //! \par Possible Implementation
  //! \code_no_test
  //! wt_lenlex_cmp(
  //!   x->cbegin(), x->cend(), y->cbegin(), y->cend(), weights);
  //! \end_code_no_test
  //!
  //! \sa
  //! wt_lenlex_cmp(Iterator, Iterator, Iterator, Iterator,
  //! std::vector<size_t> const&).
  template <typename Thing>
  [[nodiscard]] bool wt_lenlex_cmp(Thing* const               x,
                                   Thing* const               y,
                                   std::vector<size_t> const& weights) {
    return wt_lenlex_cmp(
        x->cbegin(), x->cend(), y->cbegin(), y->cend(), weights);
  }

  //! \brief A stateful struct with binary call operator using
  //! \ref wt_lenlex_cmp or \ref wt_lenlex_cmp_no_checks.
  //!
  //! Defined in `order.hpp`.
  //!
  //! A stateful struct with binary call operator using
  //! \ref wt_lenlex_cmp or \ref wt_lenlex_cmp_no_checks,
  //! depending on the value of the constructor parameter \c should_check. This
  //! struct stores a copy of a weights vector and can be used as a template
  //! parameter for standard library containers or algorithms that require a
  //! comparison functor.
  //!
  //! \warning
  //! When the constructor parameter \c should_check is \c false, the call
  //! operator does not check that letters are valid indices into the weights
  //! vector. Use the constructor with \c should_check set to \c true
  //! (\ref checks) to enable argument checking in the call operator.
  //!
  //! \sa
  //! * wt_lenlex_cmp(Thing const&, Thing const&, std::vector<size_t>
  //! const&)
  //! * wt_lenlex_cmp_no_checks(Thing const&, Thing const&,
  //! std::vector<size_t> const&)
  struct WtLenLexCmp {
    //! \brief Constant to enable validity checks.
    //!
    //! This constant can be used in the constructors to indicate that
    //! checks should be performed on the arguments to the call operator.
    static constexpr bool checks = true;

    //! \brief Constant to disable validity checks.
    //!
    //! This constant can be used in the constructors to indicate that no
    //! checks should be performed on the arguments to the call operator.
    static constexpr bool no_checks = false;

    //! \brief Construct from weights vector reference and specify whether or
    //! not the call operator should check its arguments.
    //!
    //! Constructs a comparison object that stores a copy of the provided
    //! weights vector, where the `i`th index corresponds to the weight of the
    //! `i`th letter in the alphabet. The \p should_check parameter determines
    //! whether the call operator will validate that letters are valid indices.
    //!
    //! \param weights the weights vector.
    //! \param should_check if \c true (\ref checks), the call operator will
    //! check validity; if \c false (\ref no_checks), it will not.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    WtLenLexCmp(std::vector<size_t> const& weights, bool should_check)
        : _weights(weights), _should_check(should_check) {}

    //! \brief Reinitialize an existing WtLenLexCmp object.
    //!
    //! This function reinitializes an existing WtLenLexCmp object so that
    //! it is in the same state as if it was newly constructed using the same
    //! arguments.
    //!
    //! \param weights the weights vector.
    //! \param should_check if \c true (\ref checks), the call operator will
    //! check validity; if \c false (\ref no_checks), it will not.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    WtLenLexCmp& init(std::vector<size_t> const& weights, bool should_check) {
      _weights      = weights;
      _should_check = should_check;
      return *this;
    }

    //! \brief Construct from weights vector rvalue reference and specify
    //! whether or not the call operator should check its arguments.
    //!
    //! Constructs a comparison object that takes ownership of the provided
    //! weights vector, where the `i`th index corresponds to the weight of the
    //! `i`th letter in the alphabet. The \p should_check parameter determines
    //! whether the call operator will validate that letters are valid indices.
    //!
    //! \param weights the weights vector.
    //! \param should_check if \c true (\ref checks), the call operator will
    //! check validity; if \c false (\ref no_checks), it will not.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    WtLenLexCmp(std::vector<size_t>&& weights, bool should_check)
        : _weights(std::move(weights)), _should_check(should_check) {}

    //! \brief Reinitialize an existing WtLenLexCmp object.
    //!
    //! This function reinitializes an existing WtLenLexCmp object so that
    //! it is in the same state as if it was newly constructed using the same
    //! arguments.
    //!
    //! \param weights the weights vector.
    //! \param should_check if \c true (\ref checks), the call operator will
    //! check validity; if \c false (\ref no_checks), it will not.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    WtLenLexCmp& init(std::vector<size_t>&& weights, bool should_check) {
      _weights      = std::move(weights);
      _should_check = should_check;
      return *this;
    }

    //! \brief Call operator that compares \p x and \p y using either
    //! \ref wt_lenlex_cmp or \ref wt_lenlex_cmp_no_checks.
    //!
    //! Call operator that compares \p x and \p y using
    //! \ref wt_lenlex_cmp (if the constructor parameter \c should_check
    //! is \c true) or \ref wt_lenlex_cmp_no_checks (if \c should_check is
    //! \c false).
    //!
    //! \tparam Thing the type of the objects to be compared.
    //!
    //! \param x const reference to the first object for comparison.
    //! \param y const reference to the second object for comparison.
    //!
    //! \returns The boolean value \c true if \p x is weighted len-lex less
    //! than \p y, and \c false otherwise.
    //!
    //! \throws LibsemigroupsException if the constructor parameter
    //! \c should_check is \c true and any letter is not a valid index into the
    //! weights vector.
    //!
    //! \complexity
    //! See
    //! * wt_lenlex_cmp(Iterator, Iterator, Iterator, Iterator,
    //! std::vector<size_t> const&);
    //! * wt_lenlex_cmp_no_checks(Iterator, Iterator, Iterator, Iterator,
    //! std::vector<size_t> const&).
    //!
    //! \warning
    //! If the constructor parameter \c should_check is \c false, it is not
    //! checked that the letters are valid indices into the weights vector.
    template <typename Thing>
    [[nodiscard]] bool operator()(Thing const& x, Thing const& y) const {
      if (_should_check) {
        return wt_lenlex_cmp(x, y, _weights);
      } else {
        return wt_lenlex_cmp_no_checks(x, y, _weights);
      }
    }

    //! \brief Call operator that does no checks.
    //!
    //! This member function always uses \ref wt_lenlex_cmp_no_checks to
    //! compare \p x and \p y, regardless of the value of the constructor
    //! parameter \c should_check. Use this when you want to ensure validation
    //! is not performed.
    //!
    //! \tparam Thing the type of the objects to be compared.
    //!
    //! \param x const reference to the first object for comparison.
    //! \param y const reference to the second object for comparison.
    //!
    //! \returns The boolean value \c true if \p x is weighted len-lex less
    //! than \p y, and \c false otherwise.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! See wt_lenlex_cmp_no_checks(Iterator, Iterator, Iterator,
    //! Iterator, std::vector<size_t> const&)
    template <typename Thing>
    [[nodiscard]] bool call_no_checks(Thing const& x, Thing const& y) const {
      return wt_lenlex_cmp_no_checks(x, y, _weights);
    }

    //! \brief Returns the value of the constructor parameter \c should_check.
    //!
    //! This function returns the current value of the constructor parameter
    //! \c should_check.
    //!
    //! \returns Whether or not the call operator is checking its arguments.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \sa should_check(bool)
    [[nodiscard]] bool should_check() const noexcept {
      return _should_check;
    }

    //! \brief Set the value of the constructor parameter \c should_check.
    //!
    //! This function sets the value of \c should_check to \p val. This
    //! parameter determines whether or not the call operator is checking its
    //! arguments.
    //!
    //! \param val the new value of \c should_check.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \sa should_check()
    WtLenLexCmp& should_check(bool val) noexcept {
      _should_check = val;
      return *this;
    }

    //! \brief Returns the weights.
    //!
    //! This function returns the current value of the weights used to define
    //! the comparison implemented by WtLenLexCmp.
    //!
    //! \returns The current weights.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] std::vector<size_t> const& weights() const noexcept {
      return _weights;
    }

    //! \brief Set the weights.
    //!
    //! This function can be used to redefine the weights used to define the
    //! comparison implemented by WtLenLexCmp.
    //!
    //! \param val the new weights to use.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \exceptions
    //! \noexcept
    WtLenLexCmp& weights(std::vector<size_t> const& val) {
      _weights = val;
      return *this;
    }

   private:
    std::vector<size_t> _weights;
    bool                _should_check;
  };

  //////////////////////////////////////////////////////////////////////
  // Weighted lex
  //////////////////////////////////////////////////////////////////////

  //! \brief Compare two objects of the same type using the weighted lex
  //! ordering without checks.
  //!
  //! Defined in `order.hpp`.
  //!
  //! This function compares two objects of the same type using the weighted
  //! lex ordering. The weight of a word is computed by adding up the
  //! weights of the letters in the word, where the `i`th index of the weights
  //! vector corresponds to the weight of the `i`th letter in the alphabet.
  //! Heavier words come later in the ordering than all lighter words. Amongst
  //! words of equal weight, lexicographic ordering is used.
  //!
  //! \tparam Iterator the type of iterators to the first object to be compared.
  //!
  //! \param first1 beginning iterator of first object for comparison.
  //! \param last1 ending iterator of first object for comparison.
  //! \param first2 beginning iterator of second object for comparison.
  //! \param last2 ending iterator of second object for comparison.
  //! \param weights the weights vector.
  //!
  //! \returns The boolean value \c true if the range `[first1, last1)` is
  //! weighted lex less than the range `[first2, last2)`, and \c false
  //! otherwise.
  //!
  //! \exceptions
  //! Throws if std::lexicographical_compare does.
  //!
  //! \complexity
  //! At most \f$O(n + m)\f$ where \f$n\f$ is the distance between \p last1
  //! and \p first1, and \f$m\f$ is the distance between \p last2 and
  //! \p first2.
  //!
  //! \warning
  //! It is not checked that the letters in the ranges are valid indices into
  //! the weights vector.
  //!
  //! \sa
  //! wt_lex_cmp(Iterator, Iterator, Iterator, Iterator,
  //! std::vector<size_t> const&).
  template <typename Iterator,
            typename = std::enable_if_t<!rx::is_input_or_sink_v<Iterator>>>
  [[nodiscard]] bool wt_lex_cmp_no_checks(Iterator                   first1,
                                          Iterator                   last1,
                                          Iterator                   first2,
                                          Iterator                   last2,
                                          std::vector<size_t> const& weights);

  //! \brief Compare two objects of the same type using
  //! \ref wt_lex_cmp_no_checks without checks.
  //!
  //! Defined in `order.hpp`.
  //!
  //! This function compares two objects of the same type using
  //! \ref wt_lex_cmp_no_checks, where the `i`th index of the weights
  //! vector corresponds to the weight of the `i`th letter in the alphabet.
  //!
  //! \tparam Thing the type of the objects to be compared.
  //!
  //! \param x const reference to the first object for comparison.
  //! \param y const reference to the second object for comparison.
  //! \param weights the weights vector.
  //!
  //! \returns The boolean value \c true if \p x is weighted lex less
  //! than \p y, and \c false otherwise.
  //!
  //! \exceptions
  //! See \ref wt_lex_cmp_no_checks(Iterator, Iterator, Iterator,
  //! Iterator, std::vector<size_t> const&).
  //!
  //! \complexity
  //! At most \f$O(n + m)\f$ where \f$n\f$ is the length of \p x and \f$m\f$
  //! is the length of \p y.
  //!
  //! \par Possible Implementation
  //! \code_no_test
  //! wt_lex_cmp_no_checks(
  //!   x.cbegin(), x.cend(), y.cbegin(), y.cend(), weights);
  //! \end_code_no_test
  //!
  //! \warning
  //! It is not checked that the letters in \p x and \p y are valid indices
  //! into the weights vector.
  //!
  //! \sa
  //! wt_lex_cmp_no_checks(Iterator, Iterator, Iterator, Iterator,
  //! std::vector<size_t> const&).
  template <typename Thing,
            typename = std::enable_if_t<!rx::is_input_or_sink_v<Thing>>>
  [[nodiscard]] bool wt_lex_cmp_no_checks(Thing const&               x,
                                          Thing const&               y,
                                          std::vector<size_t> const& weights) {
    return wt_lex_cmp_no_checks(
        x.cbegin(), x.cend(), y.cbegin(), y.cend(), weights);
  }

  //! \brief Compare two objects via their pointers using
  //! \ref wt_lex_cmp_no_checks without checks.
  //!
  //! Defined in `order.hpp`.
  //!
  //! This function compares two objects via their pointers using
  //! \ref wt_lex_cmp_no_checks, where the `i`th index of the weights
  //! vector corresponds to the weight of the `i`th letter in the alphabet.
  //!
  //! \tparam Thing the type of the objects to be compared.
  //!
  //! \param x pointer to the first object for comparison.
  //! \param y pointer to the second object for comparison.
  //! \param weights the weights vector.
  //!
  //! \returns The boolean value \c true if \p x points to a word weighted
  //! lex less than the word pointed to by \p y, and \c false otherwise.
  //!
  //! \exceptions
  //! See \ref wt_lex_cmp_no_checks(Iterator, Iterator, Iterator,
  //! Iterator, std::vector<size_t> const&).
  //!
  //! \complexity
  //! At most \f$O(n + m)\f$ where \f$n\f$ is the length of the word pointed
  //! to by \p x and \f$m\f$ is the length of word pointed to by \p y.
  //!
  //! \par Possible Implementation
  //! \code_no_test
  //! wt_lex_cmp_no_checks(
  //!   x->cbegin(), x->cend(), y->cbegin(), y->cend(), weights);
  //! \end_code_no_test
  //!
  //! \warning
  //! It is not checked that the letters are valid indices into the weights
  //! vector.
  //!
  //! \sa
  //! wt_lex_cmp_no_checks(Iterator, Iterator, Iterator, Iterator,
  //! std::vector<size_t> const&).
  template <typename Thing>
  [[nodiscard]] bool wt_lex_cmp_no_checks(Thing* const               x,
                                          Thing* const               y,
                                          std::vector<size_t> const& weights) {
    return wt_lex_cmp_no_checks(
        x->cbegin(), x->cend(), y->cbegin(), y->cend(), weights);
  }

  //! \brief Compare two objects of the same type using the weighted lex
  //! ordering and check validity.
  //!
  //! Defined in `order.hpp`.
  //!
  //! This function compares two objects of the same type using the weighted
  //! lex ordering. The weight of a word is computed by adding up the
  //! weights of the letters in the word, where the `i`th index of the weights
  //! vector corresponds to the weight of the `i`th letter in the alphabet.
  //! Heavier words come later in the ordering than all lighter words. Amongst
  //! words of equal weight, lexicographic ordering is used.
  //!
  //! After checking that all letters in both ranges are valid indices into
  //! the weights vector, this function performs the same as
  //! `wt_lex_cmp_no_checks(first1, last1, first2, last2, weights)`.
  //!
  //! \tparam Iterator the type of iterators to the first object to be compared.
  //!
  //! \param first1 beginning iterator of first object for comparison.
  //! \param last1 ending iterator of first object for comparison.
  //! \param first2 beginning iterator of second object for comparison.
  //! \param last2 ending iterator of second object for comparison.
  //! \param weights the weights vector.
  //!
  //! \returns The boolean value \c true if the range `[first1, last1)` is
  //! weighted lex less than the range `[first2, last2)`, and \c false
  //! otherwise.
  //!
  //! \throws LibsemigroupsException if any letter in either range is not a
  //! valid index into the weights vector (i.e., if any letter is greater
  //! than or equal to `weights.size()`).
  //!
  //! \complexity
  //! At most \f$O(n + m)\f$ where \f$n\f$ is the distance between \p last1
  //! and \p first1, and \f$m\f$ is the distance between \p last2 and
  //! \p first2.
  //!
  //! \sa
  //! wt_lex_cmp_no_checks(Iterator, Iterator, Iterator, Iterator,
  //! std::vector<size_t> const&)
  template <typename Iterator,
            typename = std::enable_if_t<!rx::is_input_or_sink_v<Iterator>>>
  [[nodiscard]] bool wt_lex_cmp(Iterator                   first1,
                                Iterator                   last1,
                                Iterator                   first2,
                                Iterator                   last2,
                                std::vector<size_t> const& weights);

  //! \brief Compare two objects of the same type using \ref wt_lex_cmp
  //! and check validity.
  //!
  //! Defined in `order.hpp`.
  //!
  //! This function compares two objects of the same type using
  //! \ref wt_lex_cmp, where the `i`th index of the weights vector
  //! corresponds to the weight of the `i`th letter in the alphabet.
  //!
  //! After checking that all letters in both objects are valid indices into
  //! the weights vector, this function performs the same as
  //! `wt_lex_cmp_no_checks(x, y, weights)`.
  //!
  //! \tparam Thing the type of the objects to be compared.
  //!
  //! \param x const reference to the first object for comparison.
  //! \param y const reference to the second object for comparison.
  //! \param weights the weights vector.
  //!
  //! \returns The boolean value \c true if \p x is weighted lex less
  //! than \p y, and \c false otherwise.
  //!
  //! \throws LibsemigroupsException if any letter in \p x or \p y is not a
  //! valid index into the weights vector.
  //!
  //! \complexity
  //! At most \f$O(n + m)\f$ where \f$n\f$ is the length of \p x and \f$m\f$
  //! is the length of \p y.
  //!
  //! \par Possible Implementation
  //! \code_no_test
  //! wt_lex_cmp(
  //!   x.cbegin(), x.cend(), y.cbegin(), y.cend(), weights);
  //! \end_code_no_test
  //!
  //! \sa
  //! wt_lex_cmp(Iterator, Iterator, Iterator, Iterator,
  //! std::vector<size_t> const&).
  template <typename Thing,
            typename = std::enable_if_t<!rx::is_input_or_sink_v<Thing>>>
  [[nodiscard]] bool wt_lex_cmp(Thing const&               x,
                                Thing const&               y,
                                std::vector<size_t> const& weights) {
    return wt_lex_cmp(x.cbegin(), x.cend(), y.cbegin(), y.cend(), weights);
  }

  //! \brief Compare two objects via their pointers using
  //! \ref wt_lex_cmp and check validity.
  //!
  //! Defined in `order.hpp`.
  //!
  //! This function compares two objects via their pointers using
  //! \ref wt_lex_cmp, where the `i`th index of the weights vector
  //! corresponds to the weight of the `i`th letter in the alphabet.
  //!
  //! After checking that all letters are valid indices into the weights
  //! vector, this function performs the same as
  //! `wt_lex_cmp_no_checks(*x, *y, weights)`.
  //!
  //! \tparam Thing the type of the objects to be compared.
  //!
  //! \param x pointer to the first object for comparison.
  //! \param y pointer to the second object for comparison.
  //! \param weights the weights vector.
  //!
  //! \returns The boolean value \c true if \p x points to a word weighted
  //! lex less than the word pointed to by \p y, and \c false otherwise.
  //!
  //! \throws LibsemigroupsException if any letter is not a valid index into
  //! the weights vector.
  //!
  //! \complexity
  //! At most \f$O(n + m)\f$ where \f$n\f$ is the length of the word pointed
  //! to by \p x and \f$m\f$ is the length of word pointed to by \p y.
  //!
  //! \par Possible Implementation
  //! \code_no_test
  //! wt_lex_cmp(
  //!   x->cbegin(), x->cend(), y->cbegin(), y->cend(), weights);
  //! \end_code_no_test
  //!
  //! \sa
  //! wt_lex_cmp(Iterator, Iterator, Iterator, Iterator,
  //! std::vector<size_t> const&).
  template <typename Thing>
  [[nodiscard]] bool wt_lex_cmp(Thing* const               x,
                                Thing* const               y,
                                std::vector<size_t> const& weights) {
    return wt_lex_cmp(x->cbegin(), x->cend(), y->cbegin(), y->cend(), weights);
  }

  //! \brief A stateful struct with binary call operator using
  //! \ref wt_lex_cmp or \ref wt_lex_cmp_no_checks.
  //!
  //! Defined in `order.hpp`.
  //!
  //! A stateful struct with binary call operator using
  //! \ref wt_lex_cmp or \ref wt_lex_cmp_no_checks,
  //! depending on the value of the constructor parameter \c should_check. This
  //! struct stores a copy of a weights vector and can be used as a template
  //! parameter for standard library containers or algorithms that require a
  //! comparison functor.
  //!
  //! \warning
  //! When the constructor parameter \c should_check is \c false, the call
  //! operator does not check that letters are valid indices into the weights
  //! vector. Use the constructor with \c should_check set to \c true
  //! (\ref checks) to enable argument checking in the call operator.
  //!
  //! \sa
  //! * wt_lex_cmp(Thing const&, Thing const&, std::vector<size_t> const&)
  //! * wt_lex_cmp_no_checks(Thing const&, Thing const&, std::vector<size_t>
  //! const&)
  struct WtLexCmp {
    //! \brief Constant to enable validity checks.
    //!
    //! This constant can be used in the constructors to indicate that
    //! checks should be performed on the arguments to the call operator.
    static constexpr bool checks = true;

    //! \brief Constant to disable validity checks.
    //!
    //! This constant can be used in the constructors to indicate that no
    //! checks should be performed on the arguments to the call operator.
    static constexpr bool no_checks = false;

    //! \brief Construct from weights vector reference and specify whether or
    //! not the call operator should check its arguments.
    //!
    //! Constructs a comparison object that stores a copy of the provided
    //! weights vector, where the `i`th index corresponds to the weight of the
    //! `i`th letter in the alphabet. The \p should_check parameter determines
    //! whether the call operator will validate that letters are valid indices.
    //!
    //! \param weights the weights vector.
    //! \param should_check if \c true (\ref checks), the call operator will
    //! check validity; if \c false (\ref no_checks), it will not.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    WtLexCmp(std::vector<size_t> const& weights, bool should_check)
        : _weights(weights), _should_check(should_check) {}

    //! \brief Reinitialize an existing WtLexCmp object.
    //!
    //! This function reinitializes an existing WtLexCmp object so that
    //! it is in the same state as if it was newly constructed using the same
    //! arguments.
    //!
    //! \param weights the weights vector.
    //! \param should_check if \c true (\ref checks), the call operator will
    //! check validity; if \c false (\ref no_checks), it will not.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    WtLexCmp& init(std::vector<size_t> const& weights, bool should_check) {
      _weights      = std::move(weights);
      _should_check = should_check;
      return *this;
    }

    //! \brief Construct from weights vector rvalue reference and specify
    //! whether or not the call operator should check its arguments.
    //!
    //! Constructs a comparison object that takes ownership of the provided
    //! weights vector, where the `i`th index corresponds to the weight of the
    //! `i`th letter in the alphabet. The \p should_check parameter determines
    //! whether the call operator will validate that letters are valid indices.
    //!
    //! \param weights the weights vector.
    //! \param should_check if \c true (\ref checks), the call operator will
    //! check validity; if \c false (\ref no_checks), it will not.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    WtLexCmp(std::vector<size_t>&& weights, bool should_check)
        : _weights(std::move(weights)), _should_check(should_check) {}

    //! \brief Reinitialize an existing WtLexCmp object.
    //!
    //! This function reinitializes an existing WtLexCmp object so that
    //! it is in the same state as if it was newly constructed using the same
    //! arguments.
    //!
    //! \param weights the weights vector.
    //! \param should_check if \c true (\ref checks), the call operator will
    //! check validity; if \c false (\ref no_checks), it will not.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    WtLexCmp& init(std::vector<size_t>&& weights, bool should_check) {
      _weights      = std::move(weights);
      _should_check = should_check;
      return *this;
    }

    //! \brief Call operator that compares \p x and \p y using either
    //! \ref wt_lex_cmp or \ref wt_lex_cmp_no_checks.
    //!
    //! Call operator that compares \p x and \p y using
    //! \ref wt_lex_cmp (if the constructor parameter \c should_check is
    //! \c true) or \ref wt_lex_cmp_no_checks (if \c should_check is
    //! \c false).
    //!
    //! \tparam Thing the type of the objects to be compared.
    //!
    //! \param x const reference to the first object for comparison.
    //! \param y const reference to the second object for comparison.
    //!
    //! \returns The boolean value \c true if \p x is weighted lex less
    //! than \p y, and \c false otherwise.
    //!
    //! \throws LibsemigroupsException if \p should_check is \c true and any
    //! letter is not a valid index into the weights vector.
    //!
    //! \complexity
    //! See:
    //! * wt_lex_cmp(Iterator, Iterator, Iterator, Iterator,
    //! std::vector<size_t> const&)
    //! * wt_lex_cmp_no_checks(Iterator, Iterator, Iterator, Iterator,
    //! std::vector<size_t> const&).
    //!
    //! \warning
    //! If the constructor parameter \c should_check is \c false, it is not
    //! checked that the letters are valid indices into the weights vector.
    template <typename Thing>
    [[nodiscard]] bool operator()(Thing const& x, Thing const& y) const {
      if (_should_check) {
        return wt_lex_cmp(x, y, _weights);
      } else {
        return wt_lex_cmp_no_checks(x, y, _weights);
      }
    }

    //! \brief Call operator that does no checks.
    //!
    //! This member function always uses \ref wt_lex_cmp_no_checks to
    //! compare \p x and \p y, regardless of the value of the constructor
    //! parameter \c should_check. Use this when you want to ensure validation
    //! is not performed.
    //!
    //! \tparam Thing the type of the objects to be compared.
    //!
    //! \param x const reference to the first object for comparison.
    //! \param y const reference to the second object for comparison.
    //!
    //! \returns The boolean value \c true if \p x is weighted lex less
    //! than \p y, and \c false otherwise.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! See wt_lex_cmp_no_checks(Iterator, Iterator, Iterator, Iterator,
    //! std::vector<size_t> const&).
    template <typename Thing>
    [[nodiscard]] bool call_no_checks(Thing const& x, Thing const& y) const {
      return wt_lex_cmp_no_checks(x, y, _weights);
    }

    //! \brief Returns the value of the constructor parameter \c should_check.
    //!
    //! This function returns the current value of the constructor parameter
    //! \c should_check.
    //!
    //! \returns Whether or not the call operator is checking its arguments.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \sa should_check(bool)
    [[nodiscard]] bool should_check() const noexcept {
      return _should_check;
    }

    //! \brief Set the value of the constructor parameter \c should_check.
    //!
    //! This function sets the value of \c should_check to \p val. This
    //! parameter determines whether or not the call operator is checking its
    //! arguments.
    //!
    //! \param val the new value of \c should_check.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \sa should_check()
    WtLexCmp& should_check(bool val) noexcept {
      _should_check = val;
      return *this;
    }

    //! \brief Returns the weights.
    //!
    //! This function returns the current value of the weights used to define
    //! the comparison implemented by WtLexCmp.
    //!
    //! \returns The current weights.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] std::vector<size_t> const& weights() const noexcept {
      return _weights;
    }

    //! \brief Set the weights.
    //!
    //! This function can be used to redefine the weights used to define the
    //! comparison implemented by WtLexCmp.
    //!
    //! \param val the new weights to use.
    //!
    //! \returns A reference to `*this`.
    WtLexCmp& weights(std::vector<size_t> const& val) {
      _weights = val;
      return *this;
    }

   private:
    std::vector<size_t> _weights;
    bool                _should_check;
  };

  //////////////////////////////////////////////////////////////////////
  // Weighted short-lex - deprecated
  //////////////////////////////////////////////////////////////////////

  //! \brief Compare two objects of the same type using the weighted short-lex
  //! ordering without checks.
  //!
  //! Defined in `order.hpp`.
  //!
  //! This function compares two objects of the same type using the weighted
  //! short-lex ordering. The weight of a word is computed by adding up the
  //! weights of the letters in the word, where the `i`th index of the weights
  //! vector corresponds to the weight of the `i`th letter in the alphabet.
  //! Heavier words come later in the ordering than all lighter words. Amongst
  //! words of equal weight, short-lex ordering is used.
  //!
  //! \tparam Iterator the type of iterators that are the arguments.
  //!
  //! \param first1 beginning iterator of first object for comparison.
  //! \param last1 ending iterator of first object for comparison.
  //! \param first2 beginning iterator of second object for comparison.
  //! \param last2 ending iterator of second object for comparison.
  //! \param weights the weights vector.
  //!
  //! \returns The boolean value \c true if the range `[first1, last1)` is
  //! weighted short-lex less than the range `[first2, last2)`, and \c false
  //! otherwise.
  //!
  //! \exceptions
  //! Throws if std::lexicographical_compare does.
  //!
  //! \complexity
  //! At most \f$O(n + m)\f$ where \f$n\f$ is the distance between \p last1
  //! and \p first1, and \f$m\f$ is the distance between \p last2 and
  //! \p first2.
  //!
  //! \warning
  //! It is not checked that the letters in the ranges are valid indices into
  //! the \p weights vector.
  //!
  //! \sa
  //! wt_shortlex_compare(Iterator, Iterator, Iterator, Iterator,
  //! std::vector<size_t> const&).
  //!
  //! \deprecated_warning{function} Use \ref wt_lenlex_cmp_no_checks instead.
  template <typename Iterator,
            typename = std::enable_if_t<!rx::is_input_or_sink_v<Iterator>>>
  [[nodiscard]] [[deprecated("Use wt_lenlex_cmp_no_checks instead!")]] bool
  wt_shortlex_compare_no_checks(Iterator                   first1,
                                Iterator                   last1,
                                Iterator                   first2,
                                Iterator                   last2,
                                std::vector<size_t> const& weights) {
    return wt_lenlex_cmp_no_checks(first1, last1, first2, last2, weights);
  }

  //! \brief Compare two objects of the same type using
  //! \ref wt_shortlex_compare_no_checks without checks.
  //!
  //! Defined in `order.hpp`.
  //!
  //! This function compares two objects of the same type using
  //! \ref wt_shortlex_compare_no_checks, where the `i`th index of the weights
  //! vector corresponds to the weight of the `i`th letter in the alphabet.
  //!
  //! \tparam Thing the type of the objects to be compared.
  //!
  //! \param x const reference to the first object for comparison.
  //! \param y const reference to the second object for comparison.
  //! \param weights the weights vector.
  //!
  //! \returns The boolean value \c true if \p x is weighted short-lex less
  //! than \p y, and \c false otherwise.
  //!
  //! \exceptions
  //! See \ref wt_shortlex_compare_no_checks(Iterator, Iterator, Iterator,
  //! Iterator, std::vector<size_t> const&).
  //!
  //! \complexity
  //! At most \f$O(n + m)\f$ where \f$n\f$ is the length of \p x and \f$m\f$
  //! is the length of \p y.
  //!
  //! \par Possible Implementation
  //! \code_no_test
  //! wt_shortlex_compare_no_checks(
  //!   x.cbegin(), x.cend(), y.cbegin(), y.cend(), weights);
  //! \end_code_no_test
  //!
  //! \warning
  //! It is not checked that the letters in \p x and \p y are valid indices
  //! into the weights vector.
  //!
  //! \sa
  //! wt_shortlex_compare_no_checks(Iterator, Iterator, Iterator, Iterator,
  //! std::vector<size_t> const&).
  //!
  //! \deprecated_warning{function} Use \ref wt_lenlex_cmp_no_checks instead.
  template <typename Thing,
            typename = std::enable_if_t<!rx::is_input_or_sink_v<Thing>>>
  [[nodiscard]] [[deprecated("Use wt_lenlex_cmp_no_checks instead!")]] bool
  wt_shortlex_compare_no_checks(Thing const&               x,
                                Thing const&               y,
                                std::vector<size_t> const& weights) {
    return wt_shortlex_compare_no_checks(
        x.cbegin(), x.cend(), y.cbegin(), y.cend(), weights);
  }

  //! \brief Compare two objects via their pointers using
  //! \ref wt_shortlex_compare_no_checks without checks.
  //!
  //! Defined in `order.hpp`.
  //!
  //! This function compares two objects via their pointers using
  //! \ref wt_shortlex_compare_no_checks, where the `i`th index of the weights
  //! vector corresponds to the weight of the `i`th letter in the alphabet.
  //!
  //! \tparam Thing the type of the objects to be compared.
  //!
  //! \param x pointer to the first object for comparison.
  //! \param y pointer to the second object for comparison.
  //! \param weights the weights vector.
  //!
  //! \returns The boolean value \c true if \p x points to a word weighted
  //! short-lex less than the word pointed to by \p y, and \c false otherwise.
  //!
  //! \exceptions
  //! See \ref wt_shortlex_compare_no_checks(Iterator, Iterator, Iterator,
  //! Iterator, std::vector<size_t> const&).
  //!
  //! \complexity
  //! At most \f$O(n + m)\f$ where \f$n\f$ is the length of the word pointed
  //! to by \p x and \f$m\f$ is the length of word pointed to by \p y.
  //!
  //! \par Possible Implementation
  //! \code_no_test
  //! wt_shortlex_compare_no_checks(
  //!   x->cbegin(), x->cend(), y->cbegin(), y->cend(), weights);
  //! \end_code_no_test
  //!
  //! \warning
  //! It is not checked that the letters are valid indices into the weights
  //! vector.
  //!
  //! \sa
  //! wt_shortlex_compare_no_checks(Iterator, Iterator, Iterator, Iterator,
  //! std::vector<size_t> const&).
  //!
  //! \deprecated_warning{function} Use \ref wt_lenlex_cmp_no_checks instead.
  template <typename Thing>
  [[nodiscard]] [[deprecated("Use wt_lenlex_cmp_no_checks instead!")]] bool
  wt_shortlex_compare_no_checks(Thing* const               x,
                                Thing* const               y,
                                std::vector<size_t> const& weights) {
    return wt_shortlex_compare_no_checks(
        x->cbegin(), x->cend(), y->cbegin(), y->cend(), weights);
  }

  //! \brief Compare two objects of the same type using the weighted short-lex
  //! ordering and check validity.
  //!
  //! Defined in `order.hpp`.
  //!
  //! This function compares two objects of the same type using the weighted
  //! short-lex ordering. The weight of a word is computed by adding up the
  //! weights of the letters in the word, where the `i`th index of the weights
  //! vector corresponds to the weight of the `i`th letter in the alphabet.
  //! Heavier words come later in the ordering than all lighter words. Amongst
  //! words of equal weight, short-lex ordering is used.
  //!
  //! After checking that all letters in both ranges are valid indices into
  //! the weights vector, this function performs the same as
  //! `wt_shortlex_compare_no_checks(first1, last1, first2, last2, weights)`.
  //!
  //! \tparam Iterator the type of iterators to the first object to be compared.
  //!
  //! \param first1 beginning iterator of first object for comparison.
  //! \param last1 ending iterator of first object for comparison.
  //! \param first2 beginning iterator of second object for comparison.
  //! \param last2 ending iterator of second object for comparison.
  //! \param weights the weights vector.
  //!
  //! \returns The boolean value \c true if the range `[first1, last1)` is
  //! weighted short-lex less than the range `[first2, last2)`, and \c false
  //! otherwise.
  //!
  //! \throws LibsemigroupsException if any letter in either range is not a
  //! valid index into the weights vector (i.e., if any letter is greater
  //! than or equal to `weights.size()`).
  //!
  //! \complexity
  //! At most \f$O(n + m)\f$ where \f$n\f$ is the distance between \p last1
  //! and \p first1, and \f$m\f$ is the distance between \p last2 and
  //! \p first2.
  //!
  //! \sa
  //! wt_shortlex_compare_no_checks(Iterator, Iterator, Iterator, Iterator,
  //! std::vector<size_t> const&).
  //!
  //! \deprecated_warning{function} Use \ref wt_lenlex_cmp instead.
  template <typename Iterator,
            typename = std::enable_if_t<!rx::is_input_or_sink_v<Iterator>>>
  [[nodiscard]] [[deprecated("Use wt_lenlex_cmp instead!")]] bool
  wt_shortlex_compare(Iterator                   first1,
                      Iterator                   last1,
                      Iterator                   first2,
                      Iterator                   last2,
                      std::vector<size_t> const& weights) {
    return wt_lenlex_cmp(first1, last1, first2, last2);
  }

  //! \brief Compare two objects of the same type using \ref wt_shortlex_compare
  //! and check validity.
  //!
  //! Defined in `order.hpp`.
  //!
  //! This function compares two objects of the same type using
  //! \ref wt_shortlex_compare, where the `i`th index of the weights vector
  //! corresponds to the weight of the `i`th letter in the alphabet.
  //!
  //! After checking that all letters in both objects are valid indices into
  //! the weights vector, this function performs the same as
  //! `wt_shortlex_compare_no_checks(x, y, weights)`.
  //!
  //! \tparam Thing the type of the objects to be compared.
  //!
  //! \param x const reference to the first object for comparison.
  //! \param y const reference to the second object for comparison.
  //! \param weights the weights vector.
  //!
  //! \returns The boolean value \c true if \p x is weighted short-lex less
  //! than \p y, and \c false otherwise.
  //!
  //! \throws LibsemigroupsException if any letter in \p x or \p y is not a
  //! valid index into the weights vector.
  //!
  //! \complexity
  //! At most \f$O(n + m)\f$ where \f$n\f$ is the length of \p x and \f$m\f$
  //! is the length of \p y.
  //!
  //! \par Possible Implementation
  //! \code_no_test
  //! wt_shortlex_compare(
  //!   x.cbegin(), x.cend(), y.cbegin(), y.cend(), weights);
  //! \end_code_no_test
  //!
  //! \sa
  //! wt_shortlex_compare(Iterator, Iterator, Iterator, Iterator,
  //! std::vector<size_t> const&).
  //!
  //! \deprecated_warning{function} Use \ref wt_lenlex_cmp instead.
  template <typename Thing,
            typename = std::enable_if_t<!rx::is_input_or_sink_v<Thing>>>
  [[nodiscard]] [[deprecated("Use wt_lenlex_cmp instead!")]] bool
  wt_shortlex_compare(Thing const&               x,
                      Thing const&               y,
                      std::vector<size_t> const& weights) {
    return wt_shortlex_compare(
        x.cbegin(), x.cend(), y.cbegin(), y.cend(), weights);
  }

  //! \brief Compare two objects via their pointers using
  //! \ref wt_shortlex_compare and check validity.
  //!
  //! Defined in `order.hpp`.
  //!
  //! This function compares two objects via their pointers using
  //! \ref wt_shortlex_compare, where the `i`th index of the weights vector
  //! corresponds to the weight of the `i`th letter in the alphabet.
  //!
  //! After checking that all letters are valid indices into the weights
  //! vector, this function performs the same as
  //! `wt_shortlex_compare_no_checks(*x, *y, weights)`.
  //!
  //! \tparam Thing the type of the objects to be compared.
  //!
  //! \param x pointer to the first object for comparison.
  //! \param y pointer to the second object for comparison.
  //! \param weights the weights vector.
  //!
  //! \returns The boolean value \c true if \p x points to a word weighted
  //! short-lex less than the word pointed to by \p y, and \c false otherwise.
  //!
  //! \throws LibsemigroupsException if any letter is not a valid index into
  //! the weights vector.
  //!
  //! \complexity
  //! At most \f$O(n + m)\f$ where \f$n\f$ is the length of the word pointed
  //! to by \p x and \f$m\f$ is the length of word pointed to by \p y.
  //!
  //! \par Possible Implementation
  //! \code_no_test
  //! wt_shortlex_compare(
  //!   x->cbegin(), x->cend(), y->cbegin(), y->cend(), weights);
  //! \end_code_no_test
  //!
  //! \sa
  //! wt_shortlex_compare(Iterator, Iterator, Iterator, Iterator,
  //! std::vector<size_t> const&).
  //!
  //! \deprecated_warning{function} Use \ref wt_lenlex_cmp instead.
  template <typename Thing>
  [[nodiscard]] [[deprecated("Use wt_lenlex_cmp instead!")]] bool
  wt_shortlex_compare(Thing* const               x,
                      Thing* const               y,
                      std::vector<size_t> const& weights) {
    return wt_shortlex_compare(
        x->cbegin(), x->cend(), y->cbegin(), y->cend(), weights);
  }

  //! \brief A stateful struct with binary call operator using
  //! \ref wt_shortlex_compare or \ref wt_shortlex_compare_no_checks.
  //!
  //! Defined in `order.hpp`.
  //!
  //! A stateful struct with binary call operator using
  //! \ref wt_shortlex_compare or \ref wt_shortlex_compare_no_checks,
  //! depending on the value of the constructor parameter \c should_check. This
  //! struct stores a copy of a weights vector and can be used as a template
  //! parameter for standard library containers or algorithms that require a
  //! comparison functor.
  //!
  //! \warning
  //! When the constructor parameter \c should_check is \c false, the call
  //! operator does not check that letters are valid indices into the weights
  //! vector. Use the constructor with \c should_check set to \c true
  //! (\ref checks) to enable argument checking in the call operator.
  //!
  //! \sa
  //! * wt_shortlex_compare(Thing const&, Thing const&, std::vector<size_t>
  //! const&)
  //! * wt_shortlex_compare_no_checks(Thing const&, Thing const&,
  //! std::vector<size_t> const&)
  //!
  //! \deprecated_warning{struct} Use \ref WtLenLexCmp instead.
  using WtShortLexCompare [[deprecated("Use WtLenLexCmp instead!")]]
  = WtLenLexCmp;

  //////////////////////////////////////////////////////////////////////
  // Weighted lex - deprecated
  //////////////////////////////////////////////////////////////////////

  //! \brief Compare two objects of the same type using the weighted lex
  //! ordering without checks.
  //!
  //! Defined in `order.hpp`.
  //!
  //! This function compares two objects of the same type using the weighted
  //! lex ordering. The weight of a word is computed by adding up the
  //! weights of the letters in the word, where the `i`th index of the weights
  //! vector corresponds to the weight of the `i`th letter in the alphabet.
  //! Heavier words come later in the ordering than all lighter words. Amongst
  //! words of equal weight, lexicographic ordering is used.
  //!
  //! \tparam Iterator the type of iterators to the first object to be compared.
  //!
  //! \param first1 beginning iterator of first object for comparison.
  //! \param last1 ending iterator of first object for comparison.
  //! \param first2 beginning iterator of second object for comparison.
  //! \param last2 ending iterator of second object for comparison.
  //! \param weights the weights vector.
  //!
  //! \returns The boolean value \c true if the range `[first1, last1)` is
  //! weighted lex less than the range `[first2, last2)`, and \c false
  //! otherwise.
  //!
  //! \exceptions
  //! Throws if std::lexicographical_compare does.
  //!
  //! \complexity
  //! At most \f$O(n + m)\f$ where \f$n\f$ is the distance between \p last1
  //! and \p first1, and \f$m\f$ is the distance between \p last2 and
  //! \p first2.
  //!
  //! \warning
  //! It is not checked that the letters in the ranges are valid indices into
  //! the weights vector.
  //!
  //! \sa
  //! wt_lex_compare(Iterator, Iterator, Iterator, Iterator,
  //! std::vector<size_t> const&).
  //!
  //! \deprecated_warning{function} Use \ref wt_lex_cmp_no_checks instead.
  template <typename Iterator,
            typename = std::enable_if_t<!rx::is_input_or_sink_v<Iterator>>>
  [[nodiscard]] [[deprecated("Use wt_lex_cmp_no_checks instead!")]] bool
  wt_lex_compare_no_checks(Iterator                   first1,
                           Iterator                   last1,
                           Iterator                   first2,
                           Iterator                   last2,
                           std::vector<size_t> const& weights) {
    return wt_lex_cmp_no_checks(first1, last1, first2, last2, weights);
  }

  //! \brief Compare two objects of the same type using
  //! \ref wt_lex_compare_no_checks without checks.
  //!
  //! Defined in `order.hpp`.
  //!
  //! This function compares two objects of the same type using
  //! \ref wt_lex_compare_no_checks, where the `i`th index of the weights
  //! vector corresponds to the weight of the `i`th letter in the alphabet.
  //!
  //! \tparam Thing the type of the objects to be compared.
  //!
  //! \param x const reference to the first object for comparison.
  //! \param y const reference to the second object for comparison.
  //! \param weights the weights vector.
  //!
  //! \returns The boolean value \c true if \p x is weighted lex less
  //! than \p y, and \c false otherwise.
  //!
  //! \exceptions
  //! See \ref wt_lex_compare_no_checks(Iterator, Iterator, Iterator,
  //! Iterator, std::vector<size_t> const&).
  //!
  //! \complexity
  //! At most \f$O(n + m)\f$ where \f$n\f$ is the length of \p x and \f$m\f$
  //! is the length of \p y.
  //!
  //! \par Possible Implementation
  //! \code_no_test
  //! wt_lex_compare_no_checks(
  //!   x.cbegin(), x.cend(), y.cbegin(), y.cend(), weights);
  //! \end_code_no_test
  //!
  //! \warning
  //! It is not checked that the letters in \p x and \p y are valid indices
  //! into the weights vector.
  //!
  //! \sa
  //! wt_lex_compare_no_checks(Iterator, Iterator, Iterator, Iterator,
  //! std::vector<size_t> const&).
  //!
  //! \deprecated_warning{function} Use \ref wt_lex_cmp_no_checks instead.
  template <typename Thing,
            typename = std::enable_if_t<!rx::is_input_or_sink_v<Thing>>>
  [[nodiscard]] [[deprecated("Use wt_lex_cmp_no_checks instead!")]] bool
  wt_lex_compare_no_checks(Thing const&               x,
                           Thing const&               y,
                           std::vector<size_t> const& weights) {
    return wt_lex_compare_no_checks(
        x.cbegin(), x.cend(), y.cbegin(), y.cend(), weights);
  }

  //! \brief Compare two objects via their pointers using
  //! \ref wt_lex_compare_no_checks without checks.
  //!
  //! Defined in `order.hpp`.
  //!
  //! This function compares two objects via their pointers using
  //! \ref wt_lex_compare_no_checks, where the `i`th index of the weights
  //! vector corresponds to the weight of the `i`th letter in the alphabet.
  //!
  //! \tparam Thing the type of the objects to be compared.
  //!
  //! \param x pointer to the first object for comparison.
  //! \param y pointer to the second object for comparison.
  //! \param weights the weights vector.
  //!
  //! \returns The boolean value \c true if \p x points to a word weighted
  //! lex less than the word pointed to by \p y, and \c false otherwise.
  //!
  //! \exceptions
  //! See \ref wt_lex_compare_no_checks(Iterator, Iterator, Iterator,
  //! Iterator, std::vector<size_t> const&).
  //!
  //! \complexity
  //! At most \f$O(n + m)\f$ where \f$n\f$ is the length of the word pointed
  //! to by \p x and \f$m\f$ is the length of word pointed to by \p y.
  //!
  //! \par Possible Implementation
  //! \code_no_test
  //! wt_lex_compare_no_checks(
  //!   x->cbegin(), x->cend(), y->cbegin(), y->cend(), weights);
  //! \end_code_no_test
  //!
  //! \warning
  //! It is not checked that the letters are valid indices into the weights
  //! vector.
  //!
  //! \sa
  //! wt_lex_compare_no_checks(Iterator, Iterator, Iterator, Iterator,
  //! std::vector<size_t> const&).
  //!
  //! \deprecated_warning{function} Use \ref wt_lex_cmp_no_checks instead.
  template <typename Thing>
  [[nodiscard]] [[deprecated("Use wt_lex_cmp_no_checks instead!")]] bool
  wt_lex_compare_no_checks(Thing* const               x,
                           Thing* const               y,
                           std::vector<size_t> const& weights) {
    return wt_lex_compare_no_checks(
        x->cbegin(), x->cend(), y->cbegin(), y->cend(), weights);
  }

  //! \brief Compare two objects of the same type using the weighted lex
  //! ordering and check validity.
  //!
  //! Defined in `order.hpp`.
  //!
  //! This function compares two objects of the same type using the weighted
  //! lex ordering. The weight of a word is computed by adding up the
  //! weights of the letters in the word, where the `i`th index of the weights
  //! vector corresponds to the weight of the `i`th letter in the alphabet.
  //! Heavier words come later in the ordering than all lighter words. Amongst
  //! words of equal weight, lexicographic ordering is used.
  //!
  //! After checking that all letters in both ranges are valid indices into
  //! the weights vector, this function performs the same as
  //! `wt_lex_compare_no_checks(first1, last1, first2, last2, weights)`.
  //!
  //! \tparam Iterator the type of iterators to the first object to be compared.
  //!
  //! \param first1 beginning iterator of first object for comparison.
  //! \param last1 ending iterator of first object for comparison.
  //! \param first2 beginning iterator of second object for comparison.
  //! \param last2 ending iterator of second object for comparison.
  //! \param weights the weights vector.
  //!
  //! \returns The boolean value \c true if the range `[first1, last1)` is
  //! weighted lex less than the range `[first2, last2)`, and \c false
  //! otherwise.
  //!
  //! \throws LibsemigroupsException if any letter in either range is not a
  //! valid index into the weights vector (i.e., if any letter is greater
  //! than or equal to `weights.size()`).
  //!
  //! \complexity
  //! At most \f$O(n + m)\f$ where \f$n\f$ is the distance between \p last1
  //! and \p first1, and \f$m\f$ is the distance between \p last2 and
  //! \p first2.
  //!
  //! \sa
  //! wt_lex_compare_no_checks(Iterator, Iterator, Iterator, Iterator,
  //! std::vector<size_t> const&)
  //!
  //! \deprecated_warning{function} Use \ref wt_lex_cmp instead.
  template <typename Iterator,
            typename = std::enable_if_t<!rx::is_input_or_sink_v<Iterator>>>
  [[nodiscard]] [[deprecated("Use wt_lex_cmp instead!")]] bool
  wt_lex_compare(Iterator                   first1,
                 Iterator                   last1,
                 Iterator                   first2,
                 Iterator                   last2,
                 std::vector<size_t> const& weights) {
    return wt_lex_cmp(first1, last1, first2, last2, weights);
  }

  //! \brief Compare two objects of the same type using \ref wt_lex_compare
  //! and check validity.
  //!
  //! Defined in `order.hpp`.
  //!
  //! This function compares two objects of the same type using
  //! \ref wt_lex_compare, where the `i`th index of the weights vector
  //! corresponds to the weight of the `i`th letter in the alphabet.
  //!
  //! After checking that all letters in both objects are valid indices into
  //! the weights vector, this function performs the same as
  //! `wt_lex_compare_no_checks(x, y, weights)`.
  //!
  //! \tparam Thing the type of the objects to be compared.
  //!
  //! \param x const reference to the first object for comparison.
  //! \param y const reference to the second object for comparison.
  //! \param weights the weights vector.
  //!
  //! \returns The boolean value \c true if \p x is weighted lex less
  //! than \p y, and \c false otherwise.
  //!
  //! \throws LibsemigroupsException if any letter in \p x or \p y is not a
  //! valid index into the weights vector.
  //!
  //! \complexity
  //! At most \f$O(n + m)\f$ where \f$n\f$ is the length of \p x and \f$m\f$
  //! is the length of \p y.
  //!
  //! \par Possible Implementation
  //! \code_no_test
  //! wt_lex_compare(
  //!   x.cbegin(), x.cend(), y.cbegin(), y.cend(), weights);
  //! \end_code_no_test
  //!
  //! \sa
  //! wt_lex_compare(Iterator, Iterator, Iterator, Iterator,
  //! std::vector<size_t> const&).
  //!
  //! \deprecated_warning{function} Use \ref wt_lex_cmp instead.
  template <typename Thing,
            typename = std::enable_if_t<!rx::is_input_or_sink_v<Thing>>>
  [[nodiscard]] [[deprecated("Use wt_lex_cmp instead!")]] bool
  wt_lex_compare(Thing const&               x,
                 Thing const&               y,
                 std::vector<size_t> const& weights) {
    return wt_lex_compare(x.cbegin(), x.cend(), y.cbegin(), y.cend(), weights);
  }

  //! \brief Compare two objects via their pointers using
  //! \ref wt_lex_compare and check validity.
  //!
  //! Defined in `order.hpp`.
  //!
  //! This function compares two objects via their pointers using
  //! \ref wt_lex_compare, where the `i`th index of the weights vector
  //! corresponds to the weight of the `i`th letter in the alphabet.
  //!
  //! After checking that all letters are valid indices into the weights
  //! vector, this function performs the same as
  //! `wt_lex_compare_no_checks(*x, *y, weights)`.
  //!
  //! \tparam Thing the type of the objects to be compared.
  //!
  //! \param x pointer to the first object for comparison.
  //! \param y pointer to the second object for comparison.
  //! \param weights the weights vector.
  //!
  //! \returns The boolean value \c true if \p x points to a word weighted
  //! lex less than the word pointed to by \p y, and \c false otherwise.
  //!
  //! \throws LibsemigroupsException if any letter is not a valid index into
  //! the weights vector.
  //!
  //! \complexity
  //! At most \f$O(n + m)\f$ where \f$n\f$ is the length of the word pointed
  //! to by \p x and \f$m\f$ is the length of word pointed to by \p y.
  //!
  //! \par Possible Implementation
  //! \code_no_test
  //! wt_lex_compare(
  //!   x->cbegin(), x->cend(), y->cbegin(), y->cend(), weights);
  //! \end_code_no_test
  //!
  //! \sa
  //! wt_lex_compare(Iterator, Iterator, Iterator, Iterator,
  //! std::vector<size_t> const&).
  //!
  //! \deprecated_warning{function} Use \ref wt_lex_cmp instead.
  template <typename Thing>
  [[nodiscard]] [[deprecated("Use wt_lex_cmp instead!")]] bool
  wt_lex_compare(Thing* const               x,
                 Thing* const               y,
                 std::vector<size_t> const& weights) {
    return wt_lex_compare(
        x->cbegin(), x->cend(), y->cbegin(), y->cend(), weights);
  }

  //! \brief A stateful struct with binary call operator using
  //! \ref wt_lex_compare or \ref wt_lex_compare_no_checks.
  //!
  //! Defined in `order.hpp`.
  //!
  //! A stateful struct with binary call operator using
  //! \ref wt_lex_compare or \ref wt_lex_compare_no_checks,
  //! depending on the value of the constructor parameter \c should_check. This
  //! struct stores a copy of a weights vector and can be used as a template
  //! parameter for standard library containers or algorithms that require a
  //! comparison functor.
  //!
  //! \warning
  //! When the constructor parameter \c should_check is \c false, the call
  //! operator does not check that letters are valid indices into the weights
  //! vector. Use the constructor with \c should_check set to \c true
  //! (\ref checks) to enable argument checking in the call operator.
  //!
  //! \sa
  //! * wt_lex_compare(Thing const&, Thing const&, std::vector<size_t> const&)
  //! * wt_lex_compare_no_checks(Thing const&, Thing const&, std::vector<size_t>
  //! const&)
  //!
  //! \deprecated_warning{struct} Use \ref WtLexCmp instead.
  using WtLexCompare [[deprecated("Use WtLexCmp instead!")]] = WtLexCmp;
  // end orders_group
  //! @}

  //////////////////////////////////////////////////////////////////////
  // Helpers
  //////////////////////////////////////////////////////////////////////

  //! \ingroup orders_group
  //!
  //! Defined in ``order.hpp``.
  //!
  //! This namespace contains compile-time helpers for detecting properties of
  //! reduction order comparison types.
  namespace order {
    //! \brief Helper used to indicate whether or not an order is length
    //! non-increasing.
    //!
    //! This helper has a single static data member \c value which is \c true
    //! if \p Thing represents a length non-increasing reduction ordering, and
    //! \c false otherwise.
    //!
    //! \tparam Thing the reduction order type.
    template <typename Thing>
    struct is_length_non_increasing : std::false_type {};

    //! \brief len-lex order is length non-increasing.
    //!
    //! Specialization of \ref is_length_non_increasing for
    //! \ref LenLexCmp.
    template <>
    struct is_length_non_increasing<LenLexCmp> : std::true_type {};

    //! \brief Helper variable template for \ref is_length_non_increasing.
    //!
    //! This helper is \c true if \p Thing represents a length non-increasing
    //! reduction ordering, and \c false otherwise.
    //!
    //! \tparam Thing the reduction order type.
    template <typename Thing>
    static constexpr bool is_length_non_increasing_v
        = is_length_non_increasing<Thing>::value;

    //! \brief Helper used to indicate whether or not an order is well-founded.
    //!
    //! This helper has a single static data member \c value which is \c true
    //! if \p Thing represents a well-founded reduction ordering, and \c false
    //! otherwise. An ordering is well-founded if it contains no infinite
    //! descending chains.
    //!
    //! \tparam Thing the reduction order type.
    template <typename Thing>
    struct is_well_founded : std::false_type {};

    //! \brief len-lex order is well-founded.
    //!
    //! Specialization of \ref is_well_founded for \ref LenLexCmp.
    template <>
    struct is_well_founded<LenLexCmp> : std::true_type {};

    //! \brief Recursive path order is well-founded.
    //!
    //! Specialization of \ref is_well_founded for \ref RPOCmp.
    template <>
    struct is_well_founded<RPOCmp> : std::true_type {};

    //! \brief Weighted short-lex order is well-founded.
    //!
    //! Specialization of \ref is_well_founded for \ref WtShortLexCompare.
    template <>
    struct is_well_founded<WtLenLexCmp> : std::true_type {};

    //! \brief Weighted lex order is well-founded.
    //!
    //! Specialization of \ref is_well_founded for \ref WtLexCompare.
    template <>
    struct is_well_founded<WtLexCmp> : std::true_type {};

    //! \brief Helper variable template for \ref is_well_founded.
    //!
    //! This helper is \c true if \p Thing represents a well-founded reduction
    //! ordering, and \c false otherwise.
    //!
    //! \tparam Thing the reduction order type.
    template <typename Thing>
    static constexpr bool is_well_founded_v = is_well_founded<Thing>::value;

  }  // namespace order

}  // namespace libsemigroups

#include "order.tpp"

#endif  // LIBSEMIGROUPS_ORDER_HPP_
