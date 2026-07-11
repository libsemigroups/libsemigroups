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

// This file contains deprecated order functions and aliases.

#ifndef LIBSEMIGROUPS_ORDER_DEPRECATED_HPP_
#define LIBSEMIGROUPS_ORDER_DEPRECATED_HPP_

//////////////////////////////////////////////////////////////////////
// Lexicographical - deprecated
//////////////////////////////////////////////////////////////////////

//! \brief Compare two objects of the same type using
//! std::lexicographical_compare.
//!
//! Defined in `order-deprecated.hpp`.
//!
//! This function compares two objects of the same type using
//! std::lexicographical_compare.
//!
//! \tparam Word the type of the objects to be compared.
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
template <typename Word>
[[nodiscard]] [[deprecated("Use lex_cmp instead!")]] bool
lexicographical_compare(Word const& x, Word const& y) {
  return lex_cmp(x, y);
}

//! \brief Compare two objects via their pointers using
//! std::lexicographical_compare.
//!
//! Defined in `order-deprecated.hpp`.
//!
//! This function compares two objects via their pointers using
//! std::lexicographical_compare.
//!
//! \tparam Word the type of the objects to be compared.
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
//! \deprecated_warning{function}.
template <typename Word>
[[nodiscard]] [[deprecated(
    "This function will be removed in v4, and no alternative "
    "provided.")]] bool
lexicographical_compare(Word const* x, Word const* y) {
  return lex_cmp(x, y);
}

//! \brief A stateless struct with binary call operator using
//! std::lexicographical_compare.
//!
//! Defined in `order-deprecated.hpp`.
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
//! \deprecated_warning{struct} Use \ref LexCmp instead.
using LexicographicalCompare [[deprecated("Use LexCmp instead")]]
= LexCmp<void>;

//////////////////////////////////////////////////////////////////////
// Short-lex - deprecated
//////////////////////////////////////////////////////////////////////

//! \brief Compare two objects of the same type using the short-lex reduction
//! ordering.
//!
//! Defined in `order-deprecated.hpp`.
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
template <typename Iterator>
[[nodiscard]] [[deprecated("Use lenlex_cmp instead!")]] bool
shortlex_compare(Iterator first1,
                 Iterator last1,
                 Iterator first2,
                 Iterator last2) {
  return lenlex_cmp(first1, last1, first2, last2);
}

//! \brief Compare two objects of the same type using \ref shortlex_compare.
//!
//! Defined in `order-deprecated.hpp`.
//!
//! This function compares two objects of the same type using
//! \ref shortlex_compare.
//!
//! \tparam Word the type of the objects to be compared.
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
template <typename Word>
[[nodiscard]] [[deprecated("Use lenlex_cmp instead!")]] bool
shortlex_compare(Word const& x, Word const& y) {
  return lenlex_cmp(x, y);
}

//! \brief Compare two objects via their pointers using \ref shortlex_compare.
//!
//! Defined in `order-deprecated.hpp`.
//!
//! This function compares two objects via their pointers using
//! \ref shortlex_compare.
//!
//! \tparam Word the type of the objects to be compared.
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
template <typename Word>
[[nodiscard]] [[deprecated(
    "This function will be removed in v4, and no alternative "
    "provided.")]] bool
shortlex_compare(Word const* x, Word const* y) {
  return lenlex_cmp(x, y);
}

//! \brief A stateless struct with binary call operator using
//! \ref shortlex_compare.
//!
//! Defined in `order-deprecated.hpp`.
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
using ShortLexCompare [[deprecated("Use LenLexCmp instead!")]] = LenLexCmp<>;

//////////////////////////////////////////////////////////////////////
// Recursive path order (RPO) - deprecated
//////////////////////////////////////////////////////////////////////

//! \brief Compare two objects of the same type using the reversed recursive
//! path comparison.
//!
//! Defined in `order-deprecated.hpp`.
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
template <typename Iterator>
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
//! Defined in `order-deprecated.hpp`.
//!
//! This function compares two objects of the same type using
//! \ref recursive_path_compare.
//!
//! \tparam Word the type of the objects to be compared.
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
template <typename Word>
[[nodiscard]] [[deprecated("Use rev_rpo_cmp instead!")]] bool
recursive_path_compare(Word const& x, Word const& y) noexcept {
  return rev_rpo_cmp(x, y);
}

//! \brief Compare two objects via their pointers using
//! \ref recursive_path_compare.
//!
//! Defined in `order-deprecated.hpp`.
//!
//! This function compares two objects via their pointers using
//! \ref recursive_path_compare.
//!
//! \tparam Word the type of the objects to be compared.
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
template <typename Word>
[[nodiscard]] [[deprecated(
    "This function will be removed in v4, and no alternative "
    "provided.")]] bool
recursive_path_compare(Word const* x, Word const* y) noexcept {
  return rev_rpo_cmp(x, y);
}

//! \brief A stateless struct with binary call operator using
//! \ref recursive_path_compare.
//!
//! Defined in `order-deprecated.hpp`.
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
= RevRPOCmp<>;

//////////////////////////////////////////////////////////////////////
// Weighted short-lex - deprecated
//////////////////////////////////////////////////////////////////////

//! \brief Compare two objects of the same type using the weighted short-lex
//! ordering without checks.
//!
//! Defined in `order-deprecated.hpp`.
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
template <typename Iterator>
[[nodiscard]] [[deprecated("Use wt_lenlex_cmp_no_checks instead!")]] bool
wt_shortlex_compare_no_checks(Iterator                   first1,
                              Iterator                   last1,
                              Iterator                   first2,
                              Iterator                   last2,
                              std::vector<size_t> const& weights) {
  return wt_lenlex_cmp_no_checks(weights, first1, last1, first2, last2);
}

//! \brief Compare two objects of the same type using
//! \ref wt_shortlex_compare_no_checks without checks.
//!
//! Defined in `order-deprecated.hpp`.
//!
//! This function compares two objects of the same type using
//! \ref wt_shortlex_compare_no_checks, where the `i`th index of the weights
//! vector corresponds to the weight of the `i`th letter in the alphabet.
//!
//! \tparam Word the type of the objects to be compared.
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
template <typename Word>
[[nodiscard]] [[deprecated("Use wt_lenlex_cmp_no_checks instead!")]] bool
wt_shortlex_compare_no_checks(Word const&                x,
                              Word const&                y,
                              std::vector<size_t> const& weights) {
  return wt_lenlex_cmp_no_checks(weights, x, y);
}

//! \brief Compare two objects via their pointers using
//! \ref wt_shortlex_compare_no_checks without checks.
//!
//! Defined in `order-deprecated.hpp`.
//!
//! This function compares two objects via their pointers using
//! \ref wt_shortlex_compare_no_checks, where the `i`th index of the weights
//! vector corresponds to the weight of the `i`th letter in the alphabet.
//!
//! \tparam Word the type of the objects to be compared.
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
template <typename Word>
[[nodiscard]] [[deprecated(
    "This function will be removed in v4, and no alternative "
    "provided.")]] bool
wt_shortlex_compare_no_checks(Word const*                x,
                              Word const*                y,
                              std::vector<size_t> const& weights) {
  return wt_lenlex_cmp(weights, x, y);
}

//! \brief Compare two objects of the same type using the weighted short-lex
//! ordering and check validity.
//!
//! Defined in `order-deprecated.hpp`.
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
template <typename Iterator>
[[nodiscard]] [[deprecated("Use wt_lenlex_cmp instead!")]] bool
wt_shortlex_compare(Iterator                   first1,
                    Iterator                   last1,
                    Iterator                   first2,
                    Iterator                   last2,
                    std::vector<size_t> const& weights) {
  return wt_lenlex_cmp(weights, first1, last1, first2, last2);
}

//! \brief Compare two objects of the same type using \ref wt_shortlex_compare
//! and check validity.
//!
//! Defined in `order-deprecated.hpp`.
//!
//! This function compares two objects of the same type using
//! \ref wt_shortlex_compare, where the `i`th index of the weights vector
//! corresponds to the weight of the `i`th letter in the alphabet.
//!
//! After checking that all letters in both objects are valid indices into
//! the weights vector, this function performs the same as
//! `wt_shortlex_compare_no_checks(x, y, weights)`.
//!
//! \tparam Word the type of the objects to be compared.
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
template <typename Word>
[[nodiscard]] [[deprecated("Use wt_lenlex_cmp instead!")]] bool
wt_shortlex_compare(Word const&                x,
                    Word const&                y,
                    std::vector<size_t> const& weights) {
  return wt_lenlex_cmp(weights, x, y);
}

//! \brief Compare two objects via their pointers using
//! \ref wt_shortlex_compare and check validity.
//!
//! Defined in `order-deprecated.hpp`.
//!
//! This function compares two objects via their pointers using
//! \ref wt_shortlex_compare, where the `i`th index of the weights vector
//! corresponds to the weight of the `i`th letter in the alphabet.
//!
//! After checking that all letters are valid indices into the weights
//! vector, this function performs the same as
//! `wt_shortlex_compare_no_checks(*x, *y, weights)`.
//!
//! \tparam Word the type of the objects to be compared.
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
template <typename Word>
[[nodiscard]] [[deprecated(
    "This function will be removed in v4, and no alternative "
    "provided.")]] bool
wt_shortlex_compare(Word const*                x,
                    Word const*                y,
                    std::vector<size_t> const& weights) {
  return wt_lenlex_cmp(weights, x, y);
}

//! \brief A stateful struct with binary call operator using
//! \ref wt_shortlex_compare or \ref wt_shortlex_compare_no_checks.
//!
//! Defined in `order-deprecated.hpp`.
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
//! * wt_shortlex_compare(Word const&, Word const&, std::vector<size_t>
//! const&)
//! * wt_shortlex_compare_no_checks(Word const&, Word const&,
//! std::vector<size_t> const&)
//!
//! \deprecated_warning{struct} Use \ref WtLenLexCmp instead.
using WtShortLexCompare [[deprecated("Use WtLenLexCmp instead!")]]
= WtLenLexCmp<>;

//////////////////////////////////////////////////////////////////////
// Weighted lex - deprecated
//////////////////////////////////////////////////////////////////////

//! \brief Compare two objects of the same type using the weighted lex
//! ordering without checks.
//!
//! Defined in `order-deprecated.hpp`.
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
template <typename Iterator>
[[nodiscard]] [[deprecated("Use wt_lex_cmp_no_checks instead!")]] bool
wt_lex_compare_no_checks(Iterator                   first1,
                         Iterator                   last1,
                         Iterator                   first2,
                         Iterator                   last2,
                         std::vector<size_t> const& weights) {
  return wt_lex_cmp_no_checks(weights, first1, last1, first2, last2);
}

//! \brief Compare two objects of the same type using
//! \ref wt_lex_compare_no_checks without checks.
//!
//! Defined in `order-deprecated.hpp`.
//!
//! This function compares two objects of the same type using
//! \ref wt_lex_compare_no_checks, where the `i`th index of the weights
//! vector corresponds to the weight of the `i`th letter in the alphabet.
//!
//! \tparam Word the type of the objects to be compared.
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
template <typename Word>
[[nodiscard]] [[deprecated("Use wt_lex_cmp_no_checks instead!")]] bool
wt_lex_compare_no_checks(Word const&                x,
                         Word const&                y,
                         std::vector<size_t> const& weights) {
  return wt_lex_cmp_no_checks(weights, x, y);
}

//! \brief Compare two objects via their pointers using
//! \ref wt_lex_compare_no_checks without checks.
//!
//! Defined in `order-deprecated.hpp`.
//!
//! This function compares two objects via their pointers using
//! \ref wt_lex_compare_no_checks, where the `i`th index of the weights
//! vector corresponds to the weight of the `i`th letter in the alphabet.
//!
//! \tparam Word the type of the objects to be compared.
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
template <typename Word>
[[nodiscard]] [[deprecated(
    "This function will be removed in v4, and no alternative "
    "provided.")]] bool
wt_lex_compare_no_checks(Word const*                x,
                         Word const*                y,
                         std::vector<size_t> const& weights) {
  return wt_lex_cmp_no_checks(weights, x, y);
}

//! \brief Compare two objects of the same type using the weighted lex
//! ordering and check validity.
//!
//! Defined in `order-deprecated.hpp`.
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
template <typename Iterator>
[[nodiscard]] [[deprecated("Use wt_lex_cmp instead!")]] bool
wt_lex_compare(Iterator                   first1,
               Iterator                   last1,
               Iterator                   first2,
               Iterator                   last2,
               std::vector<size_t> const& weights) {
  return wt_lex_cmp(weights, first1, last1, first2, last2);
}

//! \brief Compare two objects of the same type using \ref wt_lex_compare
//! and check validity.
//!
//! Defined in `order-deprecated.hpp`.
//!
//! This function compares two objects of the same type using
//! \ref wt_lex_compare, where the `i`th index of the weights vector
//! corresponds to the weight of the `i`th letter in the alphabet.
//!
//! After checking that all letters in both objects are valid indices into
//! the weights vector, this function performs the same as
//! `wt_lex_compare_no_checks(x, y, weights)`.
//!
//! \tparam Word the type of the objects to be compared.
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
template <typename Word>
[[nodiscard]] [[deprecated("Use wt_lex_cmp instead!")]] bool
wt_lex_compare(Word const&                x,
               Word const&                y,
               std::vector<size_t> const& weights) {
  return wt_lex_cmp(weights, x, y);
}

//! \brief Compare two objects via their pointers using
//! \ref wt_lex_compare and check validity.
//!
//! Defined in `order-deprecated.hpp`.
//!
//! This function compares two objects via their pointers using
//! \ref wt_lex_compare, where the `i`th index of the weights vector
//! corresponds to the weight of the `i`th letter in the alphabet.
//!
//! After checking that all letters are valid indices into the weights
//! vector, this function performs the same as
//! `wt_lex_compare_no_checks(*x, *y, weights)`.
//!
//! \tparam Word the type of the objects to be compared.
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
template <typename Word>
[[nodiscard]] [[deprecated(
    "This function will be removed in v4, and no alternative "
    "provided.")]] bool
wt_lex_compare(Word const*                x,
               Word const*                y,
               std::vector<size_t> const& weights) {
  return wt_lex_cmp(weights, x, y);
}

//! \brief A stateful struct with binary call operator using
//! \ref wt_lex_compare or \ref wt_lex_compare_no_checks.
//!
//! Defined in `order-deprecated.hpp`.
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
//! * wt_lex_compare(Word const&, Word const&, std::vector<size_t> const&)
//! * wt_lex_compare_no_checks(Word const&, Word const&, std::vector<size_t>
//! const&)
//!
//! \deprecated_warning{struct} Use \ref WtLexCmp instead.
using WtLexCompare [[deprecated("Use WtLexCmp instead!")]] = WtLexCmp<>;

//! @}

#endif  // LIBSEMIGROUPS_ORDER_DEPRECATED_HPP_
