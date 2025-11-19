//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019-2025 James D. Mitchell
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

// TODO(later)
// 1. add the others (recursive path words) from test-todd-coxeter.cpp
// 2. add some documentation

#ifndef LIBSEMIGROUPS_ORDER_HPP_
#define LIBSEMIGROUPS_ORDER_HPP_

#include <cstddef>  // for size_t
#include <numeric>  // for accumulate
#include <vector>   // for vector

#include "exception.hpp"  // for LIBSEMIGROUPS_EXCEPTION
#include "ranges.hpp"     // for shortlex_compare

#include "ranges.hpp"

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

    //! The short-lex ordering. Words are first ordered by length, and then
    //! lexicographically.
    shortlex,

    //! The lexicographic ordering. Note that this is not a well-order, so there
    //! may not be a lexicographically least word in a given congruence class of
    //! words.
    lex,

    //! The recursive-path ordering, as described in \cite Jantzen2012aa
    //! (Definition 1.2.14, page 24).
    recursive

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

  //! \brief Compare two objects of the same type using
  //! std::lexicographical_compare.
  //!
  //! Defined in `order.hpp`.
  //!
  //! This function compares two objects of the same type using
  //! std::lexicographical_compare.
  //!
  //! \tparam T the type of the objects to be compared.
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
  //! \code
  //! std::lexicographical_compare(
  //!   x.cbegin(), x.cend(), y.cbegin(), y.cend());
  //! \endcode
  template <typename T, typename = std::enable_if_t<!rx::is_input_or_sink_v<T>>>
  bool lexicographical_compare(T const& x, T const& y) {
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
  //! \tparam T the type of the objects to be compared.
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
  //! \code
  //! lexicographical_compare(
  //!   x->cbegin(), x->cend(), y->cbegin(), y->cend());
  //! \endcode
  template <typename T>
  bool lexicographical_compare(T* const x, T* const y) {
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
  struct LexicographicalCompare {
    //! \brief Call operator that compares \p x and \p y using
    //! std::lexicographical_compare.
    //!
    //! Call operator that compares \p x and \p y using
    //! std::lexicographical_compare.
    //!
    //! \tparam T the type of the parameters.
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
    template <typename T>
    bool operator()(T const& x, T const& y) const {
      return lexicographical_compare(x, y);
    }

    //! \brief Call operator that compares \p x and \p y given initializer lists
    //! using std::lexicographical_compare.
    //!
    //! Call operator that compares \p x and
    //! \p y given initializer lists using std::lexicographical_compare.
    //!
    //! \tparam T the type of the parameters.
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
    template <typename T>
    bool operator()(std::initializer_list<T> x,
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
    //! \tparam T the type of the parameters.
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
    // TODO(v3) remove this?
    template <typename T>
    bool operator()(T first1, T last1, T first2, T last2) const {
      return std::lexicographical_compare(first1, last1, first2, last2);
    }
  };

  //! \brief Compare two objects of the same type using the short-lex reduction
  //! ordering.
  //!
  //! Defined in `order.hpp`.
  //!
  //! This function compares two objects of the same type using the short-lex
  //! reduction ordering.
  //!
  //! \tparam T the type of iterators to the first object to be compared.
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
  template <typename T, typename = std::enable_if_t<!rx::is_input_or_sink_v<T>>>
  bool shortlex_compare(T const& first1,
                        T const& last1,
                        T const& first2,
                        T const& last2) {
    return (last1 - first1) < (last2 - first2)
           || ((last1 - first1) == (last2 - first2)
               && std::lexicographical_compare(first1, last1, first2, last2));
  }

  //! \brief Compare two objects of the same type using \ref shortlex_compare.
  //!
  //! Defined in `order.hpp`.
  //!
  //! This function compares two objects of the same type using
  //! \ref shortlex_compare.
  //!
  //! \tparam T the type of the objects to be compared.
  //!
  //! \param x const reference to the first object for comparison.
  //! \param y const reference to the second object for comparison.
  //!
  //! \returns The boolean value \c true if \p x is short-lex less than \p y,
  //! and \c false otherwise.
  //!
  //! \exceptions
  //! See \ref shortlex_compare(T const&, T const&, T const&, T const&).
  //!
  //! \complexity
  //! At most \f$O(n)\f$ where \f$n\f$ is the minimum of the length of \p x and
  //! the length of \p y.
  //!
  //! \par Possible Implementation
  //! \code
  //! shortlex_compare(
  //!   x.cbegin(), x.cend(), y.cbegin(), y.cend());
  //! \endcode
  //!
  //! \sa
  //! \ref shortlex_compare(T const&, T const&, T const&, T const&).
  template <typename T, typename = std::enable_if_t<!rx::is_input_or_sink_v<T>>>
  bool shortlex_compare(T const& x, T const& y) {
    return shortlex_compare(x.cbegin(), x.cend(), y.cbegin(), y.cend());
  }

  //! \brief Compare two objects via their pointers using \ref shortlex_compare.
  //!
  //! Defined in `order.hpp`.
  //!
  //! This function compares two objects via their pointers using
  //! \ref shortlex_compare.
  //!
  //! \tparam T the type of the objects to be compared.
  //!
  //! \param x pointer to the first object for comparison.
  //! \param y pointer to the second object for comparison.
  //!
  //! \returns The boolean value \c true if \p x points to a word short-lex less
  //! than the word pointed to by \p y, and \c false otherwise.
  //!
  //! \exceptions
  //! See \ref shortlex_compare(T const&, T const&, T const&, T const&).
  //!
  //! \complexity
  //! At most \f$O(n)\f$ where \f$n\f$ is the minimum of the length of the word
  //! pointed to by \p x and the length of word pointed to by \p y.
  //!
  //! \par Possible Implementation
  //! \code
  //! shortlex_compare(
  //!   x->cbegin(), x->cend(), y->cbegin(), y->cend());
  //! \endcode
  //!
  //! \sa
  //! \ref shortlex_compare(T const&, T const&, T const&, T const&).
  template <typename T>
  bool shortlex_compare(T* const x, T* const y) {
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
  //! shortlex_compare(T const, T const, S const, S const)
  struct ShortLexCompare {
    //! \brief Call operator that compares \p x and \p y using
    //! \ref shortlex_compare.
    //!
    //! Call operator that compares \p x and \p y using \ref shortlex_compare.
    //!
    //! \tparam T the type of the objects to be compared.
    //!
    //! \param x const reference to the first object for comparison.
    //! \param y const reference to the second object for comparison.
    //!
    //! \returns The boolean value \c true if \p x is short-lex less than \p y,
    //! and \c false otherwise.
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

  //! \brief Compare two objects of the same type using the recursive path
  //! comparison.
  //!
  //! Defined in `order.hpp`.
  //!
  //! This function compares two objects of the same type using the recursive
  //! path comparison described in \cite Jantzen2012aa (Definition 1.2.14, page
  //! 24).
  //!
  //! If \f$u, v\in X ^ {*}\f$, \f$u \neq v\f$, and \f$u = a'u\f$,
  //! \f$v = bv'\f$ for some \f$a,b \in X\f$, \f$u',v'\in X ^ {*}\f$, then
  //! \f$u > v\f$ if one of the following conditions holds:
  //! 1. \f$a = b\f$ and \f$u' \geq v'\f$;
  //! 2. \f$a > b\f$ and \f$u  > v'\f$;
  //! 3. \f$b > a\f$ and \f$u' > v\f$.
  //!
  //! This documentation and the implementation of \ref recursive_path_compare
  //! is based on the source code of \cite Holt2018aa.
  //!
  //! \tparam T the type of iterators to the first object to be compared.
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
  //! the variants of \ref shortlex_compare and std::lexicographical_compare.
  template <typename T, typename = std::enable_if_t<!rx::is_input_or_sink_v<T>>>
  bool recursive_path_compare(T const& first1,
                              T        last1,
                              T const& first2,
                              T        last2) noexcept {
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

  //! \brief Compare two objects of the same type using
  //! \ref recursive_path_compare.
  //!
  //! Defined in `order.hpp`.
  //!
  //! This function compares two objects of the same type using
  //! \ref recursive_path_compare.
  //!
  //! \tparam T the type of the objects to be compared.
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
  //! \code
  //! recursive_path_compare(
  //!   x.cbegin(), x.cend(), y.cbegin(), y.cend());
  //! \endcode
  //!
  //! \sa
  //! \ref recursive_path_compare(T const&, T, T const&, T)
  template <typename T, typename = std::enable_if_t<!rx::is_input_or_sink_v<T>>>
  bool recursive_path_compare(T const& x, T const& y) noexcept {
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
  //! \tparam T the type of the objects to be compared.
  //!
  //! \param x pointer to the first object for comparison.
  //! \param y pointer to the second object for comparison.
  //!
  //! \returns The boolean value \c true if the value pointed to by \p x is less
  //! than the value pointed to by \p y with respect to the recursive path
  //! ordering, and \c false otherwise.
  //!
  //! \exceptions
  //! \noexcept
  //!
  //! \par Possible Implementation
  //! \code
  //! recursive_path_compare(
  //!   x->cbegin(), x->cend(), y->cbegin(), y->cend());
  //! \endcode
  //!
  //! \sa
  //! \ref recursive_path_compare(T const&, T, T const&, T)
  template <typename T>
  bool recursive_path_compare(T* const x, T* const y) noexcept {
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
  //! \ref recursive_path_compare(T const&, T, T const&, T).
  struct RecursivePathCompare {
    //! \brief  Call operator that compares \p x and \p y using
    //! \ref recursive_path_compare.
    //!
    //! Call operator that compares \p x and \p y using
    //! \ref recursive_path_compare.
    //!
    //! \tparam T the type of the objects to be compared.
    //!
    //! \param x const reference to the first object for comparison.
    //! \param y const reference to the second object for comparison.
    //!
    //! \returns The boolean value \c true if \p x is less than \p y with
    //! respect to the recursive path ordering, and \c false otherwise.
    //!
    //! \exceptions
    //! \noexcept
    template <typename T>
    bool operator()(T const& x, T const& y) const noexcept {
      return recursive_path_compare(x, y);
    }
  };

  //! \brief Compare two objects of the same type using the weighted short-lex
  //! ordering without checks.
  //!
  //! Defined in `order.hpp`.
  //!
  //! This function compares two objects of the same type using the weighted
  //! short-lex ordering. The weight of a word is computed by adding up the
  //! weights of the letters in the word, where the ith index of the weights
  //! vector corresponds to the weight of the ith letter in the alphabet.
  //! Heavier words come later in the ordering than all lighter words. Amongst
  //! words of equal weight, short-lex ordering is used.
  //!
  //! \tparam T the type of iterators to the first object to be compared.
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
  //! and \p first1, and \f$m\f$ is the distance between \p last2 and \p
  //! first2.
  //!
  //! \warning
  //! It is not checked that the letters in the ranges are valid indices into
  //! the weights vector.
  //!
  //! \sa
  //! \ref wt_shortlex_compare(T const&, T const&, T const&, T const&,
  //! std::vector<size_t> const&).
  template <typename T, typename = std::enable_if_t<!rx::is_input_or_sink_v<T>>>
  bool wt_shortlex_compare_no_checks(T const&                   first1,
                                     T const&                   last1,
                                     T const&                   first2,
                                     T const&                   last2,
                                     std::vector<size_t> const& weights) {
    size_t weight1 = std::accumulate(
        first1, last1, size_t(0), [&weights](size_t sum, auto letter) {
          return sum + weights[letter];
        });

    size_t weight2 = std::accumulate(
        first2, last2, size_t(0), [&weights](size_t sum, auto letter) {
          return sum + weights[letter];
        });

    if (weight1 != weight2) {
      return weight1 < weight2;
    }

    return shortlex_compare(first1, last1, first2, last2);
  }

  //! \brief Compare two objects of the same type using
  //! \ref wt_shortlex_compare_no_checks without checks.
  //!
  //! Defined in `order.hpp`.
  //!
  //! This function compares two objects of the same type using
  //! \ref wt_shortlex_compare_no_checks, where the ith index of the weights
  //! vector corresponds to the weight of the ith letter in the alphabet.
  //!
  //! \tparam T the type of the objects to be compared.
  //!
  //! \param x const reference to the first object for comparison.
  //! \param y const reference to the second object for comparison.
  //! \param weights the weights vector.
  //!
  //! \returns The boolean value \c true if \p x is weighted short-lex less
  //! than \p y, and \c false otherwise.
  //!
  //! \exceptions
  //! See \ref wt_shortlex_compare_no_checks(T const&, T const&, T const&, T
  //! const&, std::vector<size_t> const&).
  //!
  //! \complexity
  //! At most \f$O(n + m)\f$ where \f$n\f$ is the length of \p x and \f$m\f$
  //! is the length of \p y.
  //!
  //! \par Possible Implementation
  //! \code
  //! wt_shortlex_compare_no_checks(
  //!   x.cbegin(), x.cend(), y.cbegin(), y.cend(), weights);
  //! \endcode
  //!
  //! \warning
  //! It is not checked that the letters in \p x and \p y are valid indices
  //! into the weights vector.
  //!
  //! \sa
  //! \ref wt_shortlex_compare_no_checks(T const&, T const&, T const&, T const&,
  //! std::vector<size_t> const&).
  template <typename T, typename = std::enable_if_t<!rx::is_input_or_sink_v<T>>>
  bool wt_shortlex_compare_no_checks(T const&                   x,
                                     T const&                   y,
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
  //! \ref wt_shortlex_compare_no_checks, where the ith index of the weights
  //! vector corresponds to the weight of the ith letter in the alphabet.
  //!
  //! \tparam T the type of the objects to be compared.
  //!
  //! \param x pointer to the first object for comparison.
  //! \param y pointer to the second object for comparison.
  //! \param weights the weights vector.
  //!
  //! \returns The boolean value \c true if \p x points to a word weighted
  //! short-lex less than the word pointed to by \p y, and \c false otherwise.
  //!
  //! \exceptions
  //! See \ref wt_shortlex_compare_no_checks(T const&, T const&, T const&, T
  //! const&, std::vector<size_t> const&).
  //!
  //! \complexity
  //! At most \f$O(n + m)\f$ where \f$n\f$ is the length of the word pointed
  //! to by \p x and \f$m\f$ is the length of word pointed to by \p y.
  //!
  //! \par Possible Implementation
  //! \code
  //! wt_shortlex_compare_no_checks(
  //!   x->cbegin(), x->cend(), y->cbegin(), y->cend(), weights);
  //! \endcode
  //!
  //! \warning
  //! It is not checked that the letters are valid indices into the weights
  //! vector.
  //!
  //! \sa
  //! \ref wt_shortlex_compare_no_checks(T const&, T const&, T const&, T const&,
  //! std::vector<size_t> const&).
  template <typename T>
  bool wt_shortlex_compare_no_checks(T* const                   x,
                                     T* const                   y,
                                     std::vector<size_t> const& weights) {
    return wt_shortlex_compare_no_checks(
        x->cbegin(), x->cend(), y->cbegin(), y->cend(), weights);
  }

  //! \brief A stateful struct with binary call operator using
  //! \ref wt_shortlex_compare_no_checks.
  //!
  //! Defined in `order.hpp`.
  //!
  //! A stateful struct with binary call operator using
  //! \ref wt_shortlex_compare_no_checks. This struct stores a reference to a
  //! weights vector and can be used as a template parameter for standard
  //! library containers or algorithms that require a comparison functor.
  //!
  //! \sa
  //! wt_shortlex_compare_no_checks(T const&, T const&, std::vector<size_t>
  //! const&)
  struct WtShortLexCompareNoChecks {
    //! \brief Construct from weights vector by reference.
    //!
    //! Constructs a comparison object that stores a copy of the provided
    //! weights vector, where the ith index corresponds to the weight of the
    //! ith letter in the alphabet.
    //!
    //! \param weights the weights vector.
    explicit WtShortLexCompareNoChecks(std::vector<size_t> const& weights)
        : _weights(weights) {}

    //! \brief Construct from weights vector by rvalue.
    //!
    //! Constructs a comparison object that takes ownership of the provided
    //! weights vector, where the ith index corresponds to the weight of the
    //! ith letter in the alphabet.
    //!
    //! \param weights the weights vector.
    explicit WtShortLexCompareNoChecks(std::vector<size_t>&& weights)
        : _weights(std::move(weights)) {}

    //! \brief Call operator that compares \p x and \p y using
    //! \ref wt_shortlex_compare_no_checks.
    //!
    //! Call operator that compares \p x and \p y using
    //! \ref wt_shortlex_compare_no_checks.
    //!
    //! \tparam T the type of the objects to be compared.
    //!
    //! \param x const reference to the first object for comparison.
    //! \param y const reference to the second object for comparison.
    //!
    //! \returns The boolean value \c true if \p x is weighted short-lex less
    //! than \p y, and \c false otherwise.
    //!
    //! \exceptions
    //! See wt_shortlex_compare_no_checks(T const&, T const&, T const&, T
    //! const&, std::vector<size_t> const&).
    //!
    //! \complexity
    //! See wt_shortlex_compare_no_checks(T const&, T const&, T const&, T
    //! const&, std::vector<size_t> const&).
    //!
    //! \warning
    //! It is not checked that the letters are valid indices into the weights
    //! vector.
    template <typename T>
    bool operator()(T const& x, T const& y) const {
      return wt_shortlex_compare_no_checks(x, y, _weights);
    }

   private:
    std::vector<size_t> _weights;
  };

  //! \brief Compare two objects of the same type using the weighted short-lex
  //! ordering and check validity.
  //!
  //! Defined in `order.hpp`.
  //!
  //! This function compares two objects of the same type using the weighted
  //! short-lex ordering. The weight of a word is computed by adding up the
  //! weights of the letters in the word, where the ith index of the weights
  //! vector corresponds to the weight of the ith letter in the alphabet.
  //! Heavier words come later in the ordering than all lighter words. Amongst
  //! words of equal weight, short-lex ordering is used.
  //!
  //! After checking that all letters in both ranges are valid indices into
  //! the weights vector, this function performs the same as
  //! `wt_shortlex_compare_no_checks(first1, last1, first2, last2, weights)`.
  //!
  //! \tparam T the type of iterators to the first object to be compared.
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
  //! and \p first1, and \f$m\f$ is the distance between \p last2 and \p
  //! first2.
  //!
  //! \sa
  //! \ref wt_shortlex_compare_no_checks(T const&, T const&, T const&, T const&,
  //! std::vector<size_t> const&).
  template <typename T, typename = std::enable_if_t<!rx::is_input_or_sink_v<T>>>
  bool wt_shortlex_compare(T const&                   first1,
                           T const&                   last1,
                           T const&                   first2,
                           T const&                   last2,
                           std::vector<size_t> const& weights) {
    size_t const alphabet_size = weights.size();

    std::accumulate(first1, last1, 0, [&alphabet_size](int, auto letter) {
      if (static_cast<size_t>(letter) >= alphabet_size) {
        LIBSEMIGROUPS_EXCEPTION(
            "invalid letter {}, valid letters are in range [0, {})",
            static_cast<size_t>(letter),
            alphabet_size);
      }
      return 0;
    });

    std::accumulate(first2, last2, 0, [&alphabet_size](int, auto letter) {
      if (static_cast<size_t>(letter) >= alphabet_size) {
        LIBSEMIGROUPS_EXCEPTION(
            "invalid letter {}, valid letters are in range [0, {})",
            static_cast<size_t>(letter),
            alphabet_size);
      }
      return 0;
    });

    return wt_shortlex_compare_no_checks(first1, last1, first2, last2, weights);
  }

  //! \brief Compare two objects of the same type using \ref wt_shortlex_compare
  //! and check validity.
  //!
  //! Defined in `order.hpp`.
  //!
  //! This function compares two objects of the same type using \ref
  //! wt_shortlex_compare, where the ith index of the weights vector corresponds
  //! to the weight of the ith letter in the alphabet.
  //!
  //! After checking that all letters in both objects are valid indices into
  //! the weights vector, this function performs the same as
  //! `wt_shortlex_compare_no_checks(x, y, weights)`.
  //!
  //! \tparam T the type of the objects to be compared.
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
  //! \code
  //! wt_shortlex_compare(
  //!   x.cbegin(), x.cend(), y.cbegin(), y.cend(), weights);
  //! \endcode
  //!
  //! \sa
  //! \ref wt_shortlex_compare(T const&, T const&, T const&, T const&,
  //! std::vector<size_t> const&).
  template <typename T, typename = std::enable_if_t<!rx::is_input_or_sink_v<T>>>
  bool wt_shortlex_compare(T const&                   x,
                           T const&                   y,
                           std::vector<size_t> const& weights) {
    return wt_shortlex_compare(
        x.cbegin(), x.cend(), y.cbegin(), y.cend(), weights);
  }

  //! \brief Compare two objects via their pointers using
  //! \ref wt_shortlex_compare and check validity.
  //!
  //! Defined in `order.hpp`.
  //!
  //! This function compares two objects via their pointers using \ref
  //! wt_shortlex_compare, where the ith index of the weights vector corresponds
  //! to the weight of the ith letter in the alphabet.
  //!
  //! After checking that all letters are valid indices into the weights
  //! vector, this function performs the same as
  //! `wt_shortlex_compare_no_checks(x, y, weights)`.
  //!
  //! \tparam T the type of the objects to be compared.
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
  //! \code
  //! wt_shortlex_compare(
  //!   x->cbegin(), x->cend(), y->cbegin(), y->cend(), weights);
  //! \endcode
  //!
  //! \sa
  //! \ref wt_shortlex_compare(T const&, T const&, T const&, T const&,
  //! std::vector<size_t> const&).
  template <typename T>
  bool wt_shortlex_compare(T* const                   x,
                           T* const                   y,
                           std::vector<size_t> const& weights) {
    return wt_shortlex_compare(
        x->cbegin(), x->cend(), y->cbegin(), y->cend(), weights);
  }

  //! \brief A stateful struct with binary call operator using
  //! \ref wt_shortlex_compare.
  //!
  //! Defined in `order.hpp`.
  //!
  //! A stateful struct with binary call operator using
  //! \ref wt_shortlex_compare. This struct stores a reference to a weights
  //! vector and can be used as a template parameter for standard library
  //! containers or algorithms that require a comparison functor.
  //!
  //! \sa
  //! wt_shortlex_compare(T const&, T const&, std::vector<size_t> const&)
  struct WtShortLexCompare {
    //! \brief Construct from weights vector by reference.
    //!
    //! Constructs a comparison object that stores a copy of the provided
    //! weights vector, where the ith index corresponds to the weight of the
    //! ith letter in the alphabet.
    //!
    //! \param weights the weights vector.
    explicit WtShortLexCompare(std::vector<size_t> const& weights)
        : _weights(weights) {}

    //! \brief Construct from weights vector by rvalue.
    //!
    //! Constructs a comparison object that takes ownership of the provided
    //! weights vector, where the ith index corresponds to the weight of the
    //! ith letter in the alphabet.
    //!
    //! \param weights the weights vector.
    explicit WtShortLexCompare(std::vector<size_t>&& weights)
        : _weights(std::move(weights)) {}

    //! \brief Call operator that compares \p x and \p y using
    //! \ref wt_shortlex_compare.
    //!
    //! Call operator that compares \p x and \p y using
    //! \ref wt_shortlex_compare.
    //!
    //! \tparam T the type of the objects to be compared.
    //!
    //! \param x const reference to the first object for comparison.
    //! \param y const reference to the second object for comparison.
    //!
    //! \returns The boolean value \c true if \p x is weighted short-lex less
    //! than \p y, and \c false otherwise.
    //!
    //! \throws LibsemigroupsException if any letter is not a valid index
    //! into the weights vector.
    //!
    //! \complexity
    //! See wt_shortlex_compare(T const&, T const&, T const&, T const&,
    //! std::vector<size_t> const&).
    template <typename T>
    bool operator()(T const& x, T const& y) const {
      return wt_shortlex_compare(x, y, _weights);
    }

   private:
    std::vector<size_t> _weights;
  };

  // end orders_group
  //! @}

}  // namespace libsemigroups

#endif  // LIBSEMIGROUPS_ORDER_HPP_
