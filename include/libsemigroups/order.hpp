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

#ifndef LIBSEMIGROUPS_ORDER_HPP_
#define LIBSEMIGROUPS_ORDER_HPP_

#include <algorithm>         // for std::find_if, std::lexicographical_compare
#include <cstddef>           // for size_t
#include <cstdint>           // for uint8_t
#include <initializer_list>  // for initializer_list
#include <iterator>          // for distance
#include <numeric>           // for accumulate
#include <string_view>       // for string_view
#include <tuple>             // for tie
#include <type_traits>       // for enable_if_t
#include <utility>           // for move
#include <vector>            // for vector

#include "alphabet-class.hpp"  // for Alphabet
#include "exception.hpp"       // for LIBSEMIGROUPS_EXCEPTION
#include "ranges.hpp"          // for shortlex_compare

#include "detail/citow.hpp"  // for citow

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

    //! The reversed recursive-path ordering, based on the description in
    //! \cite Jantzen2012aa (Definition 1.2.14, page 24), where words are read
    //! right-to-left before ordering.
    rev_rpo,

    //! The recursive-path ordering, as described in \cite Jantzen2012aa
    //! (Definition 1.2.14, page 24).
    //!
    //! \deprecated_warning{value} Use \ref Order::rpo instead.
    recursive [[deprecated("Use rpo instead!")]] = rev_rpo

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

  //! \brief Compare two ranges lexicographically without checking an alphabet.
  //!
  //! This overload maps letters through \p alphabet and compares the resulting
  //! indices using std::lexicographical_compare. It does not check that the
  //! letters in the ranges belong to \p alphabet.
  //!
  //! \param alphabet the alphabet used to map letters to indices.
  //! \param first1 beginning iterator of first object for comparison.
  //! \param last1 ending iterator of first object for comparison.
  //! \param first2 beginning iterator of second object for comparison.
  //! \param last2 ending iterator of second object for comparison.
  //!
  //! \returns The boolean value \c true if the mapped range `[first1, last1)`
  //! is lexicographically less than the mapped range `[first2, last2)`, and
  //! \c false otherwise.
  template <typename Word, typename Iterator>
  [[nodiscard]] bool lex_cmp_no_checks(Alphabet<Word> const& alphabet,
                                       Iterator              first1,
                                       Iterator              last1,
                                       Iterator              first2,
                                       Iterator              last2) {
    return std::lexicographical_compare(detail::citow(alphabet, first1),
                                        detail::citow(alphabet, last1),
                                        detail::citow(alphabet, first2),
                                        detail::citow(alphabet, last2));
  }

  //! \brief Compare two ranges lexicographically with respect to an alphabet.
  //!
  //! This overload checks that both ranges contain only letters belonging to
  //! \p alphabet, then compares the corresponding alphabet indices using
  //! std::lexicographical_compare.
  //!
  //! \param alphabet the alphabet used to map letters to indices.
  //! \param first1 beginning iterator of first object for comparison.
  //! \param last1 ending iterator of first object for comparison.
  //! \param first2 beginning iterator of second object for comparison.
  //! \param last2 ending iterator of second object for comparison.
  //!
  //! \returns The boolean value \c true if the mapped range `[first1, last1)`
  //! is lexicographically less than the mapped range `[first2, last2)`, and
  //! \c false otherwise.
  //!
  //! \throws LibsemigroupsException if any letter in either range does not
  //! belong to \p alphabet.
  template <typename Word, typename Iterator>
  [[nodiscard]] bool lex_cmp(Alphabet<Word> const& alphabet,
                             Iterator              first1,
                             Iterator              last1,
                             Iterator              first2,
                             Iterator              last2) {
    alphabet.throw_if_letter_not_in_alphabet(first1, last1);
    alphabet.throw_if_letter_not_in_alphabet(first2, last2);
    return lex_cmp_no_checks(alphabet, first1, last1, first2, last2);
  }

  //! \brief Compare two objects of the same type using
  //! std::lexicographical_compare.
  //!
  //! Defined in `order.hpp`.
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
  template <typename Word,
            typename = std::enable_if_t<!rx::is_input_or_sink_v<Word>>>
  [[nodiscard]] bool lex_cmp(Word const& x, Word const& y) {
    return std::lexicographical_compare(
        x.cbegin(), x.cend(), y.cbegin(), y.cend());
  }

  //! \brief Compare two objects lexicographically without checking an alphabet.
  //!
  //! This overload maps letters through \p alphabet and compares the resulting
  //! indices using std::lexicographical_compare. It does not check that the
  //! letters in \p x or \p y belong to \p alphabet.
  //!
  //! \param alphabet the alphabet used to map letters to indices.
  //! \param x const reference to the first object for comparison.
  //! \param y const reference to the second object for comparison.
  //!
  //! \returns The boolean value \c true if \p x is lexicographically less
  //! than \p y with respect to \p alphabet, and \c false otherwise.
  template <typename Word>
  [[nodiscard]] bool lex_cmp_no_checks(Alphabet<Word> const& alphabet,
                                       Word const&           x,
                                       Word const&           y) {
    return lex_cmp_no_checks(
        alphabet, x.cbegin(), x.cend(), y.cbegin(), y.cend());
  }

  //! \brief Compare two objects lexicographically with respect to an alphabet.
  //!
  //! This overload checks that both objects contain only letters belonging to
  //! \p alphabet, then compares the corresponding alphabet indices using
  //! std::lexicographical_compare.
  //!
  //! \param alphabet the alphabet used to map letters to indices.
  //! \param x const reference to the first object for comparison.
  //! \param y const reference to the second object for comparison.
  //!
  //! \returns The boolean value \c true if \p x is lexicographically less
  //! than \p y with respect to \p alphabet, and \c false otherwise.
  //!
  //! \throws LibsemigroupsException if any letter in either object does not
  //! belong to \p alphabet.
  template <typename Word>
  [[nodiscard]] bool lex_cmp(Alphabet<Word> const& alphabet,
                             Word const&           x,
                             Word const&           y) {
    return lex_cmp(alphabet, x.cbegin(), x.cend(), y.cbegin(), y.cend());
  }

  //! \brief Marker type used for stateless comparison specializations.
  using Default = void;

  template <typename Word = Default, bool check = true>
  class LexCmp;

  //! \brief Stateful lexicographic comparison functor.
  //!
  //! This class stores an alphabet and compares words lexicographically with
  //! respect to that alphabet.
  //!
  //! \tparam Word the word type associated with the alphabet.
  //! \tparam check whether to check that letters belong to the alphabet.
  template <typename Word, bool check>
  class LexCmp {
    Alphabet<Word> _alphabet;

   public:
    //! \brief Deleted default constructor.
    LexCmp() = delete;

    //! \brief Copy constructor.
    LexCmp(LexCmp const&) = default;

    //! \brief Move constructor.
    LexCmp(LexCmp&&) = default;

    //! \brief Copy assignment operator.
    LexCmp& operator=(LexCmp const&) = default;

    //! \brief Move assignment operator.
    LexCmp& operator=(LexCmp&&) = default;

    ~LexCmp() = default;

    //! \brief Construct from an alphabet.
    //!
    //! The alphabet is copied and used by the call operator.
    //!
    //! \param alphabet the alphabet used to compare letters.
    explicit LexCmp(Alphabet<Word> const& alphabet) : _alphabet(alphabet) {}

    //! \brief Construct from an alphabet rvalue reference.
    //!
    //! The alphabet is moved into the comparison object and used by the call
    //! operator.
    //!
    //! \param alphabet the alphabet used to compare letters.
    explicit LexCmp(Alphabet<Word>&& alphabet)
        : _alphabet(std::move(alphabet)) {}

    //! \brief Reinitialize from an alphabet.
    //!
    //! Replaces the stored alphabet with a copy of \p alphabet.
    //!
    //! \param alphabet the alphabet used to compare letters.
    //!
    //! \returns A reference to \c *this.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    LexCmp& init(Alphabet<Word> const& alphabet);

    //! \brief Reinitialize from an alphabet rvalue.
    //!
    //! Replaces the stored alphabet by moving from \p alphabet.
    //!
    //! \param alphabet the alphabet used to compare letters.
    //!
    //! \returns A reference to \c *this.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    LexCmp& init(Alphabet<Word>&& alphabet);

    //! \brief Call operator that compares \p x and \p y using
    //! std::lexicographical_compare.
    //!
    //! Call operator that compares \p x and \p y using
    //! std::lexicographical_compare.
    //!
    //! \tparam Word the type of the parameters.
    //!
    //! \param x const reference to the first object for comparison.
    //! \param y const reference to the second object for comparison.
    //!
    //! \returns The boolean value \c true if \p x is lexicographically less
    //! than \p y, and \c false otherwise.
    //!
    //! \exceptions
    //! Throws if the corresponding comparison function throws.
    //! In particular, throws LibsemigroupsException if \c check is \c true
    //! and a letter in \p x or \p y does not belong to the stored alphabet.
    //!
    //! \complexity
    //! See std::lexicographical_compare.
    [[nodiscard]] bool operator()(Word const& x, Word const& y) const {
      if constexpr (check) {
        return lex_cmp(_alphabet, x, y);
      } else {
        return lex_cmp_no_checks(_alphabet, x, y);
      }
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
    //! Throws if the corresponding comparison function throws.
    //! In particular, throws LibsemigroupsException if \c check is \c true
    //! and a letter in either range does not belong to the stored alphabet.
    //!
    //! \complexity
    //! See std::lexicographical_compare.
    //!
    template <typename Iterator>
    [[nodiscard]] bool operator()(Iterator first1,
                                  Iterator last1,
                                  Iterator first2,
                                  Iterator last2) const {
      if constexpr (check) {
        return lex_cmp(_alphabet, first1, last1, first2, last2);
      } else {
        return lex_cmp_no_checks(_alphabet, first1, last1, first2, last2);
      }
    }

    //! \brief Returns the alphabet.
    //!
    //! \returns The stored alphabet.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] Alphabet<Word> const& alphabet() const noexcept {
      return _alphabet;
    }
  };  // class LexCmp

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
  template <>
  struct LexCmp<Default, true> {
    //! \brief Reinitialize the comparison object.
    //!
    //! This function has no effect because this specialization is stateless.
    //!
    //! \returns A reference to \c *this.
    //!
    //! \exceptions
    //! \noexcept
    LexCmp& init() noexcept {
      return *this;
    }

    //! \brief Call operator that compares \p x and \p y using
    //! std::lexicographical_compare.
    //!
    //! Call operator that compares \p x and \p y using
    //! std::lexicographical_compare.
    //!
    //! \tparam Word the type of the parameters.
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
    template <typename Word>
    [[nodiscard]] bool operator()(Word const& x, Word const& y) const {
      return lex_cmp(x, y);
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
    template <typename Iterator>
    [[nodiscard]] bool operator()(Iterator first1,
                                  Iterator last1,
                                  Iterator first2,
                                  Iterator last2) const {
      return std::lexicographical_compare(first1, last1, first2, last2);
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
    //!
    //! \deprecated_warning{function}
    template <typename T>
    [[nodiscard]] [[deprecated(
        "This function will be removed in v4, and no alternative "
        "provided.")]] bool
    operator()(std::initializer_list<T> x, std::initializer_list<T> y) const {
      return std::lexicographical_compare(
          x.begin(), x.end(), y.begin(), y.end());
    }
  };  // struct LexCmp<Default, true>

  template <>
  struct LexCmp<Default, false> : LexCmp<Default, true> {
    //! \brief Reinitialize the comparison object.
    //!
    //! This function has no effect because this specialization is stateless.
    //!
    //! \returns A reference to \c *this.
    //!
    //! \exceptions
    //! \noexcept
    LexCmp& init() noexcept {
      return *this;
    }
  };  // struct LexCmp<Default, false>

  //////////////////////////////////////////////////////////////////////
  // Reversed lex
  //////////////////////////////////////////////////////////////////////

  //! \brief Compare two ranges using reversed lexicographic order.
  //!
  //! This function applies \ref lex_cmp to the ranges read from right to
  //! left.
  //!
  //! \param first1 beginning iterator of first object for comparison.
  //! \param last1 ending iterator of first object for comparison.
  //! \param first2 beginning iterator of second object for comparison.
  //! \param last2 ending iterator of second object for comparison.
  //!
  //! \returns The boolean value \c true if the first range is reversed
  //! lexicographically less than the second range, and \c false otherwise.
  template <typename Iterator>
  [[nodiscard]] bool rev_lex_cmp(Iterator first1,
                                 Iterator last1,
                                 Iterator first2,
                                 Iterator last2) {
    return LexCmp<>()(std::make_reverse_iterator(last1),
                      std::make_reverse_iterator(first1),
                      std::make_reverse_iterator(last2),
                      std::make_reverse_iterator(first2));
  }

  //! \brief Compare two ranges using reversed lexicographic order without
  //! checking an alphabet.
  //!
  //! This function applies \ref lex_cmp_no_checks to the ranges read from
  //! right to left.
  template <typename Word, typename Iterator>
  [[nodiscard]] bool rev_lex_cmp_no_checks(Alphabet<Word> const& alphabet,
                                           Iterator              first1,
                                           Iterator              last1,
                                           Iterator              first2,
                                           Iterator              last2) {
    return lex_cmp_no_checks(alphabet,
                             std::make_reverse_iterator(last1),
                             std::make_reverse_iterator(first1),
                             std::make_reverse_iterator(last2),
                             std::make_reverse_iterator(first2));
  }

  //! \brief Compare two ranges using reversed lexicographic order with
  //! respect to an alphabet.
  //!
  //! This function applies \ref lex_cmp to the ranges read from right to
  //! left.
  template <typename Word, typename Iterator>
  [[nodiscard]] bool rev_lex_cmp(Alphabet<Word> const& alphabet,
                                 Iterator              first1,
                                 Iterator              last1,
                                 Iterator              first2,
                                 Iterator              last2) {
    return lex_cmp(alphabet,
                   std::make_reverse_iterator(last1),
                   std::make_reverse_iterator(first1),
                   std::make_reverse_iterator(last2),
                   std::make_reverse_iterator(first2));
  }

  //! \brief Compare two objects using reversed lexicographic order.
  template <typename Word,
            typename = std::enable_if_t<!rx::is_input_or_sink_v<Word>>>
  [[nodiscard]] bool rev_lex_cmp(Word const& x, Word const& y) {
    return rev_lex_cmp(x.cbegin(), x.cend(), y.cbegin(), y.cend());
  }

  //! \brief Compare two objects using reversed lexicographic order without
  //! checking an alphabet.
  template <typename Word>
  [[nodiscard]] bool rev_lex_cmp_no_checks(Alphabet<Word> const& alphabet,
                                           Word const&           x,
                                           Word const&           y) {
    return rev_lex_cmp_no_checks(
        alphabet, x.cbegin(), x.cend(), y.cbegin(), y.cend());
  }

  //! \brief Compare two objects using reversed lexicographic order with
  //! respect to an alphabet.
  template <typename Word>
  [[nodiscard]] bool rev_lex_cmp(Alphabet<Word> const& alphabet,
                                 Word const&           x,
                                 Word const&           y) {
    return rev_lex_cmp(alphabet, x.cbegin(), x.cend(), y.cbegin(), y.cend());
  }

  template <typename Word = Default, bool check = true>
  class RevLexCmp;

  //! \brief Stateful reversed lexicographic comparison functor.
  template <typename Word, bool check>
  class RevLexCmp {
    LexCmp<Word, check> _lex;

   public:
    RevLexCmp()                            = delete;
    RevLexCmp(RevLexCmp const&)            = default;
    RevLexCmp(RevLexCmp&&)                 = default;
    RevLexCmp& operator=(RevLexCmp const&) = default;
    RevLexCmp& operator=(RevLexCmp&&)      = default;
    ~RevLexCmp()                           = default;

    //! \brief Construct from an alphabet.
    explicit RevLexCmp(Alphabet<Word> const& alphabet) : _lex(alphabet) {}

    //! \brief Construct from an alphabet rvalue reference.
    explicit RevLexCmp(Alphabet<Word>&& alphabet) : _lex(std::move(alphabet)) {}

    //! \brief Reinitialize from an alphabet.
    RevLexCmp& init(Alphabet<Word> const& alphabet) {
      _lex.init(alphabet);
      return *this;
    }

    //! \brief Reinitialize from an alphabet rvalue.
    RevLexCmp& init(Alphabet<Word>&& alphabet) {
      _lex.init(std::move(alphabet));
      return *this;
    }

    //! \brief Compare two words using reversed lexicographic order.
    [[nodiscard]] bool operator()(Word const& x, Word const& y) const {
      return operator()(x.cbegin(), x.cend(), y.cbegin(), y.cend());
    }

    //! \brief Compare two iterator ranges using reversed lexicographic order.
    template <typename Iterator>
    [[nodiscard]] bool operator()(Iterator first1,
                                  Iterator last1,
                                  Iterator first2,
                                  Iterator last2) const {
      return _lex(std::make_reverse_iterator(last1),
                  std::make_reverse_iterator(first1),
                  std::make_reverse_iterator(last2),
                  std::make_reverse_iterator(first2));
    }

    //! \brief Returns the alphabet.
    [[nodiscard]] Alphabet<Word> const& alphabet() const noexcept {
      return _lex.alphabet();
    }
  };  // class RevLexCmp

  //! \brief Stateless reversed lexicographic comparison functor.
  template <>
  struct RevLexCmp<Default, true> {
    //! \brief Reinitialize the comparison object.
    RevLexCmp& init() noexcept {
      return *this;
    }

    //! \brief Compare two words using reversed lexicographic order.
    template <typename Word>
    [[nodiscard]] bool operator()(Word const& x, Word const& y) const {
      return operator()(x.cbegin(), x.cend(), y.cbegin(), y.cend());
    }

    //! \brief Compare two iterator ranges using reversed lexicographic order.
    template <typename Iterator>
    [[nodiscard]] bool operator()(Iterator first1,
                                  Iterator last1,
                                  Iterator first2,
                                  Iterator last2) const {
      return LexCmp<Default, true>()(std::make_reverse_iterator(last1),
                                     std::make_reverse_iterator(first1),
                                     std::make_reverse_iterator(last2),
                                     std::make_reverse_iterator(first2));
    }
  };  // struct RevLexCmp<Default, true>

  template <>
  struct RevLexCmp<Default, false> : RevLexCmp<Default, true> {
    //! \brief Reinitialize the comparison object.
    RevLexCmp& init() noexcept {
      return *this;
    }
  };  // struct RevLexCmp<Default, false>

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
  //!                 Iterator last1,
  //!                 Iterator first2,
  //!                 Iterator last2) {
  //!   return (last1 - first1) < (last2 - first2)
  //!          || ((last1 - first1) == (last2 - first2)
  //!              && std::lexicographical_compare(
  //!                  first1, last1, first2, last2));
  //! }
  //! \end_code_no_test
  // NOTE no alphabet means nothing can go wrong here, so this is both the
  // checks and no_checks version
  template <typename Iterator>
  [[nodiscard]] bool
  lenlex_cmp(Iterator first1, Iterator last1, Iterator first2, Iterator last2) {
    return (last1 - first1) < (last2 - first2)
           || ((last1 - first1) == (last2 - first2)
               && std::lexicographical_compare(first1, last1, first2, last2));
  }

  //! \brief Compare two ranges using len-lex without checking an alphabet.
  //!
  //! This overload orders first by length and then lexicographically using
  //! \p alphabet to map letters to indices. It does not check that the
  //! letters in the ranges belong to \p alphabet.
  //!
  //! \param alphabet the alphabet used to map letters to indices.
  //! \param first1 beginning iterator of first object for comparison.
  //! \param last1 ending iterator of first object for comparison.
  //! \param first2 beginning iterator of second object for comparison.
  //! \param last2 ending iterator of second object for comparison.
  //!
  //! \returns The boolean value \c true if the first range is len-lex less
  //! than the second range, and \c false otherwise.
  template <typename Word, typename Iterator>
  [[nodiscard]] bool lenlex_cmp_no_checks(Alphabet<Word> const& alphabet,
                                          Iterator              first1,
                                          Iterator              last1,
                                          Iterator              first2,
                                          Iterator              last2) {
    return (last1 - first1) < (last2 - first2)
           || ((last1 - first1) == (last2 - first2)
               && lex_cmp_no_checks(alphabet, first1, last1, first2, last2));
  }

  //! \brief Compare two ranges using len-lex with respect to an alphabet.
  //!
  //! This overload checks that both ranges contain only letters belonging to
  //! \p alphabet, then orders first by length and then lexicographically using
  //! the corresponding alphabet indices.
  //!
  //! \param alphabet the alphabet used to map letters to indices.
  //! \param first1 beginning iterator of first object for comparison.
  //! \param last1 ending iterator of first object for comparison.
  //! \param first2 beginning iterator of second object for comparison.
  //! \param last2 ending iterator of second object for comparison.
  //!
  //! \returns The boolean value \c true if the first range is len-lex less
  //! than the second range, and \c false otherwise.
  //!
  //! \throws LibsemigroupsException if any letter in either range does not
  //! belong to \p alphabet.
  template <typename Word, typename Iterator>
  [[nodiscard]] bool lenlex_cmp(Alphabet<Word> const& alphabet,
                                Iterator              first1,
                                Iterator              last1,
                                Iterator              first2,
                                Iterator              last2) {
    alphabet.throw_if_letter_not_in_alphabet(first1, last1);
    alphabet.throw_if_letter_not_in_alphabet(first2, last2);
    return lenlex_cmp_no_checks(alphabet, first1, last1, first2, last2);
  }

  //! \brief Compare two objects of the same type using \ref lenlex_cmp.
  //!
  //! Defined in `order.hpp`.
  //!
  //! This function compares two objects of the same type using
  //! \ref lenlex_cmp.
  //!
  //! \tparam Word the type of the objects to be compared.
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
  template <typename Word,
            typename = std::enable_if_t<!rx::is_input_or_sink_v<Word>>>
  [[nodiscard]] bool lenlex_cmp(Word const& x, Word const& y) {
    return lenlex_cmp(x.cbegin(), x.cend(), y.cbegin(), y.cend());
  }

  //! \brief Compare two objects using len-lex with respect to an alphabet.
  //!
  //! This overload checks that both objects contain only letters belonging to
  //! \p alphabet, then orders first by length and then lexicographically using
  //! the corresponding alphabet indices.
  //!
  //! \param alphabet the alphabet used to map letters to indices.
  //! \param x const reference to the first object for comparison.
  //! \param y const reference to the second object for comparison.
  //!
  //! \returns The boolean value \c true if \p x is len-lex less than \p y
  //! with respect to \p alphabet, and \c false otherwise.
  //!
  //! \throws LibsemigroupsException if any letter in either object does not
  //! belong to \p alphabet.
  template <typename Word>
  [[nodiscard]] bool lenlex_cmp(Alphabet<Word> const& alphabet,
                                Word const&           x,
                                Word const&           y) {
    return lenlex_cmp(alphabet, x.cbegin(), x.cend(), y.cbegin(), y.cend());
  }

  //! \brief Compare two objects using len-lex without checking an alphabet.
  //!
  //! This overload orders first by length and then lexicographically using
  //! \p alphabet to map letters to indices. It does not check that the
  //! letters in \p x or \p y belong to \p alphabet.
  //!
  //! \param alphabet the alphabet used to map letters to indices.
  //! \param x const reference to the first object for comparison.
  //! \param y const reference to the second object for comparison.
  //!
  //! \returns The boolean value \c true if \p x is len-lex less than \p y
  //! with respect to \p alphabet, and \c false otherwise.
  template <typename Word>
  [[nodiscard]] bool lenlex_cmp_no_checks(Alphabet<Word> const& alphabet,
                                          Word const&           x,
                                          Word const&           y) {
    return lenlex_cmp_no_checks(
        alphabet, x.cbegin(), x.cend(), y.cbegin(), y.cend());
  }

  template <typename Word = Default, bool check = true>
  class LenLexCmp;

  //! \brief Stateful len-lex comparison functor.
  //!
  //! This class stores an alphabet and compares words in len-lex order with
  //! respect to that alphabet.
  //!
  //! \tparam Word the word type associated with the alphabet.
  //! \tparam check whether to check that letters belong to the alphabet.
  template <typename Word, bool check>
  class LenLexCmp {
    Alphabet<Word> _alphabet;

   public:
    //! \brief Deleted default constructor.
    LenLexCmp() = delete;

    //! \brief Copy constructor.
    LenLexCmp(LenLexCmp const&) = default;

    //! \brief Move constructor.
    LenLexCmp(LenLexCmp&&) = default;

    //! \brief Copy assignment operator.
    LenLexCmp& operator=(LenLexCmp const&) = default;

    //! \brief Move assignment operator.
    LenLexCmp& operator=(LenLexCmp&&) = default;

    //! \brief Destructor.
    ~LenLexCmp() = default;

    //! \brief Construct from an alphabet.
    //!
    //! The alphabet is copied and used by the call operator.
    //!
    //! \param alphabet the alphabet used to compare letters.
    explicit LenLexCmp(Alphabet<Word> const& alphabet) : _alphabet(alphabet) {}

    //! \brief Construct from an alphabet rvalue reference.
    //!
    //! The alphabet is moved into the comparison object and used by the call
    //! operator.
    //!
    //! \param alphabet the alphabet used to compare letters.
    explicit LenLexCmp(Alphabet<Word>&& alphabet)
        : _alphabet(std::move(alphabet)) {}

    //! \brief Reinitialize from an alphabet.
    //!
    //! Replaces the stored alphabet with a copy of \p alphabet.
    //!
    //! \param alphabet the alphabet used to compare letters.
    //!
    //! \returns A reference to \c *this.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    LenLexCmp& init(Alphabet<Word> const& alphabet);

    //! \brief Reinitialize from an alphabet rvalue.
    //!
    //! Replaces the stored alphabet by moving from \p alphabet.
    //!
    //! \param alphabet the alphabet used to compare letters.
    //!
    //! \returns A reference to \c *this.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    LenLexCmp& init(Alphabet<Word>&& alphabet);

    //! \brief Call operator that compares \p x and \p y using
    //! \ref lenlex_cmp.
    //!
    //! Call operator that compares \p x and \p y using \ref lenlex_cmp.
    //!
    //! \param x const reference to the first object for comparison.
    //! \param y const reference to the second object for comparison.
    //!
    //! \returns The boolean value \c true if \p x is len-lex less than \p y,
    //! and \c false otherwise.
    //!
    //! \exceptions
    //! Throws if the corresponding comparison function throws.
    //! In particular, throws LibsemigroupsException if \c check is \c true
    //! and a letter in \p x or \p y does not belong to the stored alphabet.
    //!
    //! \complexity
    //! See \ref lenlex_cmp(Iterator, Iterator, Iterator, Iterator).
    [[nodiscard]] bool operator()(Word const& x, Word const& y) const {
      if constexpr (check) {
        return lenlex_cmp(_alphabet, x, y);
      } else {
        return lenlex_cmp_no_checks(_alphabet, x, y);
      }
    }

    //! \brief Call operator that compares two iterator ranges.
    //!
    //! \param first1 beginning iterator of first object for comparison.
    //! \param last1 ending iterator of first object for comparison.
    //! \param first2 beginning iterator of second object for comparison.
    //! \param last2 ending iterator of second object for comparison.
    //!
    //! \returns The boolean value \c true if the first range is less than
    //! the second range with respect to this comparison object, and \c false
    //! otherwise.
    //!
    //! \throws LibsemigroupsException if \c check is \c true and a letter in
    //! either range does not belong to the stored alphabet.
    template <typename Iterator>
    [[nodiscard]] bool operator()(Iterator first1,
                                  Iterator last1,
                                  Iterator first2,
                                  Iterator last2) const {
      if constexpr (check) {
        return lenlex_cmp(_alphabet, first1, last1, first2, last2);
      } else {
        return lenlex_cmp_no_checks(_alphabet, first1, last1, first2, last2);
      }
    }

    //! \brief Returns the alphabet.
    //!
    //! \returns The stored alphabet.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] Alphabet<Word> const& alphabet() const noexcept {
      return _alphabet;
    }
  };  // class LenLexCmp

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
  template <>
  struct LenLexCmp<Default, true> {
    //! \brief Reinitialize the comparison object.
    //!
    //! This function has no effect because this specialization is stateless.
    //!
    //! \returns A reference to \c *this.
    //!
    //! \exceptions
    //! \noexcept
    LenLexCmp& init() noexcept {
      return *this;
    }

    //! \brief Call operator that compares \p x and \p y using
    //! \ref lenlex_cmp.
    //!
    //! Call operator that compares \p x and \p y using \ref lenlex_cmp.
    //!
    //! \tparam Word the type of the objects to be compared.
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
    template <typename Word>
    [[nodiscard]] bool operator()(Word const& x, Word const& y) const {
      return lenlex_cmp(x, y);
    }

    //! \brief Call operator that compares two iterator ranges.
    //!
    //! \param first1 beginning iterator of first object for comparison.
    //! \param last1 ending iterator of first object for comparison.
    //! \param first2 beginning iterator of second object for comparison.
    //! \param last2 ending iterator of second object for comparison.
    //!
    //! \returns The boolean value \c true if the first range is less than
    //! the second range with respect to this comparison object, and \c false
    //! otherwise.
    template <typename Iterator>
    [[nodiscard]] bool operator()(Iterator first1,
                                  Iterator last1,
                                  Iterator first2,
                                  Iterator last2) const {
      return lenlex_cmp(first1, last1, first2, last2);
    }
  };  // struct LenLexCmp<Default, true>

  template <>
  struct LenLexCmp<Default, false> : LenLexCmp<Default, true> {
    //! \brief Reinitialize the comparison object.
    //!
    //! This function has no effect because this specialization is stateless.
    //!
    //! \returns A reference to \c *this.
    //!
    //! \exceptions
    //! \noexcept
    LenLexCmp& init() noexcept {
      return *this;
    }
  };  // struct LenLexCmp<Default, false>

  //////////////////////////////////////////////////////////////////////
  // Reversed len-lex
  //////////////////////////////////////////////////////////////////////

  //! \brief Compare two ranges using reversed len-lex.
  //!
  //! This function applies \ref lenlex_cmp to the ranges read from right to
  //! left.
  //!
  //! \param first1 beginning iterator of first object for comparison.
  //! \param last1 ending iterator of first object for comparison.
  //! \param first2 beginning iterator of second object for comparison.
  //! \param last2 ending iterator of second object for comparison.
  //!
  //! \returns The boolean value \c true if the first range is reversed
  //! len-lex less than the second range, and \c false otherwise.
  template <typename Iterator>
  [[nodiscard]] bool rev_lenlex_cmp(Iterator first1,
                                    Iterator last1,
                                    Iterator first2,
                                    Iterator last2) {
    return lenlex_cmp(std::make_reverse_iterator(last1),
                      std::make_reverse_iterator(first1),
                      std::make_reverse_iterator(last2),
                      std::make_reverse_iterator(first2));
  }

  //! \brief Compare two ranges using reversed len-lex without checking an
  //! alphabet.
  //!
  //! This function applies \ref lenlex_cmp_no_checks to the ranges read from
  //! right to left.
  template <typename Word, typename Iterator>
  [[nodiscard]] bool rev_lenlex_cmp_no_checks(Alphabet<Word> const& alphabet,
                                              Iterator              first1,
                                              Iterator              last1,
                                              Iterator              first2,
                                              Iterator              last2) {
    return lenlex_cmp_no_checks(alphabet,
                                std::make_reverse_iterator(last1),
                                std::make_reverse_iterator(first1),
                                std::make_reverse_iterator(last2),
                                std::make_reverse_iterator(first2));
  }

  //! \brief Compare two ranges using reversed len-lex with respect to an
  //! alphabet.
  //!
  //! This function applies \ref lenlex_cmp to the ranges read from right to
  //! left.
  template <typename Word, typename Iterator>
  [[nodiscard]] bool rev_lenlex_cmp(Alphabet<Word> const& alphabet,
                                    Iterator              first1,
                                    Iterator              last1,
                                    Iterator              first2,
                                    Iterator              last2) {
    return lenlex_cmp(alphabet,
                      std::make_reverse_iterator(last1),
                      std::make_reverse_iterator(first1),
                      std::make_reverse_iterator(last2),
                      std::make_reverse_iterator(first2));
  }

  //! \brief Compare two objects using reversed len-lex.
  template <typename Word,
            typename = std::enable_if_t<!rx::is_input_or_sink_v<Word>>>
  [[nodiscard]] bool rev_lenlex_cmp(Word const& x, Word const& y) {
    return rev_lenlex_cmp(x.cbegin(), x.cend(), y.cbegin(), y.cend());
  }

  //! \brief Compare two objects using reversed len-lex without checking an
  //! alphabet.
  template <typename Word>
  [[nodiscard]] bool rev_lenlex_cmp_no_checks(Alphabet<Word> const& alphabet,
                                              Word const&           x,
                                              Word const&           y) {
    return rev_lenlex_cmp_no_checks(
        alphabet, x.cbegin(), x.cend(), y.cbegin(), y.cend());
  }

  //! \brief Compare two objects using reversed len-lex with respect to an
  //! alphabet.
  template <typename Word>
  [[nodiscard]] bool rev_lenlex_cmp(Alphabet<Word> const& alphabet,
                                    Word const&           x,
                                    Word const&           y) {
    return rev_lenlex_cmp(alphabet, x.cbegin(), x.cend(), y.cbegin(), y.cend());
  }

  template <typename Word = Default, bool check = true>
  class RevLenLexCmp;

  //! \brief Stateful reversed len-lex comparison functor.
  template <typename Word, bool check>
  class RevLenLexCmp {
    LenLexCmp<Word, check> _lenlex;

   public:
    RevLenLexCmp()                               = delete;
    RevLenLexCmp(RevLenLexCmp const&)            = default;
    RevLenLexCmp(RevLenLexCmp&&)                 = default;
    RevLenLexCmp& operator=(RevLenLexCmp const&) = default;
    RevLenLexCmp& operator=(RevLenLexCmp&&)      = default;
    ~RevLenLexCmp()                              = default;

    //! \brief Construct from an alphabet.
    explicit RevLenLexCmp(Alphabet<Word> const& alphabet) : _lenlex(alphabet) {}

    //! \brief Construct from an alphabet rvalue reference.
    explicit RevLenLexCmp(Alphabet<Word>&& alphabet)
        : _lenlex(std::move(alphabet)) {}

    //! \brief Reinitialize from an alphabet.
    RevLenLexCmp& init(Alphabet<Word> const& alphabet) {
      _lenlex.init(alphabet);
      return *this;
    }

    //! \brief Reinitialize from an alphabet rvalue.
    RevLenLexCmp& init(Alphabet<Word>&& alphabet) {
      _lenlex.init(std::move(alphabet));
      return *this;
    }

    //! \brief Compare two words using reversed len-lex.
    [[nodiscard]] bool operator()(Word const& x, Word const& y) const {
      return operator()(x.cbegin(), x.cend(), y.cbegin(), y.cend());
    }

    //! \brief Compare two iterator ranges using reversed len-lex.
    template <typename Iterator>
    [[nodiscard]] bool operator()(Iterator first1,
                                  Iterator last1,
                                  Iterator first2,
                                  Iterator last2) const {
      return _lenlex(std::make_reverse_iterator(last1),
                     std::make_reverse_iterator(first1),
                     std::make_reverse_iterator(last2),
                     std::make_reverse_iterator(first2));
    }

    //! \brief Returns the alphabet.
    [[nodiscard]] Alphabet<Word> const& alphabet() const noexcept {
      return _lenlex.alphabet();
    }
  };  // class RevLenLexCmp

  //! \brief Stateless reversed len-lex comparison functor.
  template <>
  struct RevLenLexCmp<Default, true> {
    //! \brief Reinitialize the comparison object.
    RevLenLexCmp& init() noexcept {
      return *this;
    }

    //! \brief Compare two words using reversed len-lex.
    template <typename Word>
    [[nodiscard]] bool operator()(Word const& x, Word const& y) const {
      return operator()(x.cbegin(), x.cend(), y.cbegin(), y.cend());
    }

    //! \brief Compare two iterator ranges using reversed len-lex.
    template <typename Iterator>
    [[nodiscard]] bool operator()(Iterator first1,
                                  Iterator last1,
                                  Iterator first2,
                                  Iterator last2) const {
      return LenLexCmp<Default, true>()(std::make_reverse_iterator(last1),
                                        std::make_reverse_iterator(first1),
                                        std::make_reverse_iterator(last2),
                                        std::make_reverse_iterator(first2));
    }
  };  // struct RevLenLexCmp<Default, true>

  template <>
  struct RevLenLexCmp<Default, false> : RevLenLexCmp<Default, true> {
    //! \brief Reinitialize the comparison object.
    RevLenLexCmp& init() noexcept {
      return *this;
    }
  };  // struct RevLenLexCmp<Default, false>

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
  //!   2. \f$a < b\f$ and \f$u'  < v\f$; or
  //!   3. \f$a > b\f$ and \f$u < v'\f$.
  //!
  //! This documentation and the implementation of \ref rpo_cmp
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
  template <typename Iterator>
  [[nodiscard]] bool rpo_cmp(Iterator first1,
                             Iterator last1,
                             Iterator first2,
                             Iterator last2) noexcept;

  //! \brief Compare two ranges using recursive path order without checks.
  //!
  //! This overload maps letters through \p alphabet before applying recursive
  //! path order. It does not check that the letters in the ranges belong to
  //! \p alphabet.
  //!
  //! \param alphabet the alphabet used to map letters to indices.
  //! \param first1 beginning iterator of first object for comparison.
  //! \param last1 ending iterator of first object for comparison.
  //! \param first2 beginning iterator of second object for comparison.
  //! \param last2 ending iterator of second object for comparison.
  //!
  //! \returns The boolean value \c true if the first range is less than the
  //! second range with respect to recursive path order, and \c false otherwise.
  template <typename Word, typename Iterator>
  [[nodiscard]] bool rpo_cmp_no_checks(Alphabet<Word> const& alphabet,
                                       Iterator              first1,
                                       Iterator              last1,
                                       Iterator              first2,
                                       Iterator              last2);

  //! \brief Compare two ranges using recursive path order and an alphabet.
  //!
  //! This overload checks that both ranges contain only letters belonging to
  //! \p alphabet, then applies recursive path order to the corresponding
  //! alphabet indices.
  //!
  //! \param alphabet the alphabet used to map letters to indices.
  //! \param first1 beginning iterator of first object for comparison.
  //! \param last1 ending iterator of first object for comparison.
  //! \param first2 beginning iterator of second object for comparison.
  //! \param last2 ending iterator of second object for comparison.
  //!
  //! \returns The boolean value \c true if the first range is less than the
  //! second range with respect to recursive path order, and \c false otherwise.
  //!
  //! \throws LibsemigroupsException if any letter in either range does not
  //! belong to \p alphabet.
  template <typename Word, typename Iterator>
  [[nodiscard]] bool rpo_cmp(Alphabet<Word> const& alphabet,
                             Iterator              first1,
                             Iterator              last1,
                             Iterator              first2,
                             Iterator              last2);

  //! \brief Compare two objects of the same type using \ref rpo_cmp.
  //!
  //! Defined in `order.hpp`.
  //!
  //! This function compares two objects of the same type using \ref rpo_cmp.
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
  //! rpo_cmp(x.cbegin(), x.cend(), y.cbegin(), y.cend());
  //! \end_code_no_test
  //!
  //! \sa
  //! rpo_cmp(Iterator, Iterator, Iterator, Iterator)
  template <typename Word>
  [[nodiscard]] bool rpo_cmp(Word const& x, Word const& y) noexcept {
    return rpo_cmp(x.cbegin(), x.cend(), y.cbegin(), y.cend());
  }

  //! \brief Compare two objects using recursive path order and an alphabet.
  //!
  //! \param alphabet the alphabet used to map letters to indices.
  //! \param x const reference to the first object for comparison.
  //! \param y const reference to the second object for comparison.
  //!
  //! \returns The boolean value \c true if \p x is less than \p y with
  //! respect to recursive path order, and \c false otherwise.
  //!
  //! \throws LibsemigroupsException if any letter in either object does not
  //! belong to \p alphabet.
  template <typename Word>
  [[nodiscard]] bool rpo_cmp(Alphabet<Word> const& alphabet,
                             Word const&           x,
                             Word const&           y) {
    return rpo_cmp(alphabet, x.cbegin(), x.cend(), y.cbegin(), y.cend());
  }

  //! \brief Compare two objects using recursive path order without checks.
  //!
  //! This overload maps letters through \p alphabet before applying recursive
  //! path order. It does not check that letters in \p x or \p y belong to
  //! \p alphabet.
  //!
  //! \param alphabet the alphabet used to map letters to indices.
  //! \param x const reference to the first object for comparison.
  //! \param y const reference to the second object for comparison.
  //!
  //! \returns The boolean value \c true if \p x is less than \p y with
  //! respect to recursive path order, and \c false otherwise.
  template <typename Word>
  [[nodiscard]] bool rpo_cmp_no_checks(Alphabet<Word> const& alphabet,
                                       Word const&           x,
                                       Word const&           y) {
    return rpo_cmp_no_checks(
        alphabet, x.cbegin(), x.cend(), y.cbegin(), y.cend());
  }

  template <typename Word = Default, bool check = true>
  class RPOCmp;

  //! \brief Stateful recursive path order comparison functor.
  //!
  //! This class stores an alphabet and compares words in recursive path order
  //! with respect to that alphabet.
  //!
  //! \tparam Word the word type associated with the alphabet.
  //! \tparam check whether to check that letters belong to the alphabet.
  template <typename Word, bool check>
  class RPOCmp {
    Alphabet<Word> _alphabet;

   public:
    //! \brief Deleted default constructor.
    RPOCmp() = delete;

    //! \brief Copy constructor.
    RPOCmp(RPOCmp const&) = default;

    //! \brief Move constructor.
    RPOCmp(RPOCmp&&) = default;

    //! \brief Copy assignment operator.
    RPOCmp& operator=(RPOCmp const&) = default;

    //! \brief Move assignment operator.
    RPOCmp& operator=(RPOCmp&&) = default;

    //! \brief Destructor.
    ~RPOCmp() = default;

    //! \brief Construct from an alphabet.
    //!
    //! The alphabet is copied and used by the call operator.
    //!
    //! \param alphabet the alphabet used to compare letters.
    explicit RPOCmp(Alphabet<Word> const& alphabet) : _alphabet(alphabet) {}

    //! \brief Construct from an alphabet rvalue reference.
    //!
    //! The alphabet is moved into the comparison object and used by the call
    //! operator.
    //!
    //! \param alphabet the alphabet used to compare letters.
    explicit RPOCmp(Alphabet<Word>&& alphabet)
        : _alphabet(std::move(alphabet)) {}

    //! \brief Reinitialize from an alphabet.
    //!
    //! Replaces the stored alphabet with a copy of \p alphabet.
    //!
    //! \param alphabet the alphabet used to compare letters.
    //!
    //! \returns A reference to \c *this.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    RPOCmp& init(Alphabet<Word> const& alphabet);

    //! \brief Reinitialize from an alphabet rvalue.
    //!
    //! Replaces the stored alphabet by moving from \p alphabet.
    //!
    //! \param alphabet the alphabet used to compare letters.
    //!
    //! \returns A reference to \c *this.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    RPOCmp& init(Alphabet<Word>&& alphabet);

    //! \brief  Call operator that compares \p x and \p y using
    //! \ref rpo_cmp.
    //!
    //! Call operator that compares \p x and \p y using
    //! \ref rpo_cmp.
    //!
    //! \param x const reference to the first object for comparison.
    //! \param y const reference to the second object for comparison.
    //!
    //! \returns The boolean value \c true if \p x is less than \p y with
    //! respect to the recursive path ordering, and \c false otherwise.
    //!
    //! \throws LibsemigroupsException if \c check is \c true and a letter in
    //! \p x or \p y does not belong to the stored alphabet.
    [[nodiscard]] bool operator()(Word const& x, Word const& y) const {
      if constexpr (check) {
        return rpo_cmp(_alphabet, x, y);
      } else {
        return rpo_cmp_no_checks(_alphabet, x, y);
      }
    }

    //! \brief Call operator that compares two iterator ranges.
    //!
    //! \param first1 beginning iterator of first object for comparison.
    //! \param last1 ending iterator of first object for comparison.
    //! \param first2 beginning iterator of second object for comparison.
    //! \param last2 ending iterator of second object for comparison.
    //!
    //! \returns The boolean value \c true if the first range is less than
    //! the second range with respect to this comparison object, and \c false
    //! otherwise.
    //!
    //! \throws LibsemigroupsException if \c check is \c true and a letter in
    //! either range does not belong to the stored alphabet.
    template <typename Iterator>
    [[nodiscard]] bool operator()(Iterator first1,
                                  Iterator last1,
                                  Iterator first2,
                                  Iterator last2) const {
      if constexpr (check) {
        return rpo_cmp(_alphabet, first1, last1, first2, last2);
      } else {
        return rpo_cmp_no_checks(_alphabet, first1, last1, first2, last2);
      }
    }

    //! \brief Returns the alphabet.
    //!
    //! \returns The stored alphabet.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] Alphabet<Word> const& alphabet() const noexcept {
      return _alphabet;
    }
  };  // class RPOCmp

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
  template <>
  struct RPOCmp<Default, true> {
    //! \brief Reinitialize the comparison object.
    //!
    //! This function has no effect because this specialization is stateless.
    //!
    //! \returns A reference to \c *this.
    //!
    //! \exceptions
    //! \noexcept
    RPOCmp& init() noexcept {
      return *this;
    }

    //! \brief  Call operator that compares \p x and \p y using
    //! \ref rpo_cmp.
    //!
    //! Call operator that compares \p x and \p y using
    //! \ref rpo_cmp.
    //!
    //! \tparam Word the type of the objects to be compared.
    //!
    //! \param x const reference to the first object for comparison.
    //! \param y const reference to the second object for comparison.
    //!
    //! \returns The boolean value \c true if \p x is less than \p y with
    //! respect to the recursive path ordering, and \c false otherwise.
    //!
    //! \exceptions
    //! \noexcept
    template <typename Word>
    [[nodiscard]] bool operator()(Word const& x, Word const& y) const noexcept {
      return rpo_cmp(x, y);
    }

    //! \brief Call operator that compares two iterator ranges.
    //!
    //! \param first1 beginning iterator of first object for comparison.
    //! \param last1 ending iterator of first object for comparison.
    //! \param first2 beginning iterator of second object for comparison.
    //! \param last2 ending iterator of second object for comparison.
    //!
    //! \returns The boolean value \c true if the first range is less than
    //! the second range with respect to this comparison object, and \c false
    //! otherwise.
    //!
    //! \exceptions
    //! \noexcept
    template <typename Iterator>
    [[nodiscard]] bool operator()(Iterator first1,
                                  Iterator last1,
                                  Iterator first2,
                                  Iterator last2) const noexcept {
      return rpo_cmp(first1, last1, first2, last2);
    }
  };  // struct RPOCmp<Default, true>

  template <>
  struct RPOCmp<Default, false> : RPOCmp<Default, true> {
    //! \brief Reinitialize the comparison object.
    //!
    //! This function has no effect because this specialization is stateless.
    //!
    //! \returns A reference to \c *this.
    //!
    //! \exceptions
    //! \noexcept
    RPOCmp& init() noexcept {
      return *this;
    }
  };  // struct RPOCmp<Default, false>

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
  //!   2. \f$a < b\f$ and \f$u'  < v\f$; or
  //!   3. \f$a > b\f$ and \f$u < v'\f$.
  //!
  //! This documentation and the implementation of \ref rev_rpo_cmp
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
  template <typename Iterator>
  [[nodiscard]] bool rev_rpo_cmp(Iterator first1,
                                 Iterator last1,
                                 Iterator first2,
                                 Iterator last2) noexcept;

  //! \brief Compare two ranges using reversed recursive path order without
  //! checks.
  //!
  //! This overload maps letters through \p alphabet before applying reversed
  //! recursive path order. It does not check that the letters in the ranges
  //! belong to \p alphabet.
  //!
  //! \param alphabet the alphabet used to map letters to indices.
  //! \param first1 beginning iterator of first object for comparison.
  //! \param last1 ending iterator of first object for comparison.
  //! \param first2 beginning iterator of second object for comparison.
  //! \param last2 ending iterator of second object for comparison.
  //!
  //! \returns The boolean value \c true if the first range is less than the
  //! second range with respect to reversed recursive path order, and \c false
  //! otherwise.
  template <typename Word, typename Iterator>
  [[nodiscard]] bool rev_rpo_cmp_no_checks(Alphabet<Word> const& alphabet,
                                           Iterator              first1,
                                           Iterator              last1,
                                           Iterator              first2,
                                           Iterator              last2);

  //! \brief Compare two ranges using reversed recursive path order and an
  //! alphabet.
  //!
  //! This overload checks that both ranges contain only letters belonging to
  //! \p alphabet, then applies reversed recursive path order to the
  //! corresponding alphabet indices.
  //!
  //! \param alphabet the alphabet used to map letters to indices.
  //! \param first1 beginning iterator of first object for comparison.
  //! \param last1 ending iterator of first object for comparison.
  //! \param first2 beginning iterator of second object for comparison.
  //! \param last2 ending iterator of second object for comparison.
  //!
  //! \returns The boolean value \c true if the first range is less than the
  //! second range with respect to reversed recursive path order, and \c false
  //! otherwise.
  //!
  //! \throws LibsemigroupsException if any letter in either range does not
  //! belong to \p alphabet.
  template <typename Word, typename Iterator>
  [[nodiscard]] bool rev_rpo_cmp(Alphabet<Word> const& alphabet,
                                 Iterator              first1,
                                 Iterator              last1,
                                 Iterator              first2,
                                 Iterator              last2);

  //! \brief Compare two objects of the same type using \ref rev_rpo_cmp.
  //!
  //! Defined in `order.hpp`.
  //!
  //! This function compares two objects of the same type using
  //! \ref rev_rpo_cmp.
  //!
  //! \tparam Word the type of the objects to be compared.
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
  template <typename Word,
            typename = std::enable_if_t<!rx::is_input_or_sink_v<Word>>>
  [[nodiscard]] bool rev_rpo_cmp(Word const& x, Word const& y) noexcept {
    return rev_rpo_cmp(x.cbegin(), x.cend(), y.cbegin(), y.cend());
  }

  //! \brief Compare two objects using reversed recursive path order without
  //! checks.
  //!
  //! This overload maps letters through \p alphabet before applying reversed
  //! recursive path order. It does not check that letters in \p x or \p y
  //! belong to \p alphabet.
  //!
  //! \param alphabet the alphabet used to map letters to indices.
  //! \param x const reference to the first object for comparison.
  //! \param y const reference to the second object for comparison.
  //!
  //! \returns The boolean value \c true if \p x is less than \p y with
  //! respect to reversed recursive path order, and \c false otherwise.
  template <typename Word>
  [[nodiscard]] bool rev_rpo_cmp_no_checks(Alphabet<Word> const& alphabet,
                                           Word const&           x,
                                           Word const&           y) {
    return rev_rpo_cmp_no_checks(
        alphabet, x.cbegin(), x.cend(), y.cbegin(), y.cend());
  }

  //! \brief Compare two objects using reversed recursive path order and an
  //! alphabet.
  //!
  //! \param alphabet the alphabet used to map letters to indices.
  //! \param x const reference to the first object for comparison.
  //! \param y const reference to the second object for comparison.
  //!
  //! \returns The boolean value \c true if \p x is less than \p y with
  //! respect to reversed recursive path order, and \c false otherwise.
  //!
  //! \throws LibsemigroupsException if any letter in either object does not
  //! belong to \p alphabet.
  template <typename Word>
  [[nodiscard]] bool rev_rpo_cmp(Alphabet<Word> const& alphabet,
                                 Word const&           x,
                                 Word const&           y) {
    return rev_rpo_cmp(alphabet, x.cbegin(), x.cend(), y.cbegin(), y.cend());
  }

  template <typename Word = Default, bool check = true>
  class RevRPOCmp;

  //! \brief Stateful reversed recursive path order comparison functor.
  //!
  //! This class stores an alphabet and compares words in reversed recursive
  //! path order with respect to that alphabet.
  //!
  //! \tparam Word the word type associated with the alphabet.
  //! \tparam check whether to check that letters belong to the alphabet.
  template <typename Word, bool check>
  class RevRPOCmp {
    Alphabet<Word> _alphabet;

   public:
    //! \brief Deleted default constructor.
    RevRPOCmp() = delete;

    //! \brief Copy constructor.
    RevRPOCmp(RevRPOCmp const&) = default;

    //! \brief Move constructor.
    RevRPOCmp(RevRPOCmp&&) = default;

    //! \brief Copy assignment operator.
    RevRPOCmp& operator=(RevRPOCmp const&) = default;

    //! \brief Move assignment operator.
    RevRPOCmp& operator=(RevRPOCmp&&) = default;

    //! \brief Destructor.
    ~RevRPOCmp() = default;

    //! \brief Construct from an alphabet.
    //!
    //! The alphabet is copied and used by the call operator.
    //!
    //! \param alphabet the alphabet used to compare letters.
    explicit RevRPOCmp(Alphabet<Word> const& alphabet) : _alphabet(alphabet) {}

    //! \brief Construct from an alphabet rvalue reference.
    //!
    //! The alphabet is moved into the comparison object and used by the call
    //! operator.
    //!
    //! \param alphabet the alphabet used to compare letters.
    explicit RevRPOCmp(Alphabet<Word>&& alphabet)
        : _alphabet(std::move(alphabet)) {}

    //! \brief Reinitialize from an alphabet.
    //!
    //! Replaces the stored alphabet with a copy of \p alphabet.
    //!
    //! \param alphabet the alphabet used to compare letters.
    //!
    //! \returns A reference to \c *this.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    RevRPOCmp& init(Alphabet<Word> const& alphabet);

    //! \brief Reinitialize from an alphabet rvalue.
    //!
    //! Replaces the stored alphabet by moving from \p alphabet.
    //!
    //! \param alphabet the alphabet used to compare letters.
    //!
    //! \returns A reference to \c *this.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    RevRPOCmp& init(Alphabet<Word>&& alphabet);

    //! \brief  Call operator that compares \p x and \p y using
    //! \ref rev_rpo_cmp.
    //!
    //! Call operator that compares \p x and \p y using
    //! \ref rev_rpo_cmp.
    //!
    //! \param x const reference to the first object for comparison.
    //! \param y const reference to the second object for comparison.
    //!
    //! \returns The boolean value \c true if \p x is less than \p y with
    //! respect to the reversed recursive path ordering, and \c false otherwise.
    //!
    //! \throws LibsemigroupsException if \c check is \c true and a letter in
    //! \p x or \p y does not belong to the stored alphabet.
    [[nodiscard]] bool operator()(Word const& x, Word const& y) const {
      if constexpr (check) {
        return rev_rpo_cmp(_alphabet, x, y);
      } else {
        return rev_rpo_cmp_no_checks(_alphabet, x, y);
      }
    }

    //! \brief Call operator that compares two iterator ranges.
    //!
    //! \param first1 beginning iterator of first object for comparison.
    //! \param last1 ending iterator of first object for comparison.
    //! \param first2 beginning iterator of second object for comparison.
    //! \param last2 ending iterator of second object for comparison.
    //!
    //! \returns The boolean value \c true if the first range is less than
    //! the second range with respect to this comparison object, and \c false
    //! otherwise.
    //!
    //! \throws LibsemigroupsException if \c check is \c true and a letter in
    //! either range does not belong to the stored alphabet.
    template <typename Iterator>
    [[nodiscard]] bool operator()(Iterator first1,
                                  Iterator last1,
                                  Iterator first2,
                                  Iterator last2) const {
      if constexpr (check) {
        return rev_rpo_cmp(_alphabet, first1, last1, first2, last2);
      } else {
        return rev_rpo_cmp_no_checks(_alphabet, first1, last1, first2, last2);
      }
    }

    //! \brief Returns the alphabet.
    //!
    //! \returns The stored alphabet.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] Alphabet<Word> const& alphabet() const noexcept {
      return _alphabet;
    }
  };  // class RevRPOCmp

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
  template <>
  struct RevRPOCmp<Default, true> {
    //! \brief Reinitialize the comparison object.
    //!
    //! This function has no effect because this specialization is stateless.
    //!
    //! \returns A reference to \c *this.
    //!
    //! \exceptions
    //! \noexcept
    RevRPOCmp& init() noexcept {
      return *this;
    }

    //! \brief  Call operator that compares \p x and \p y using
    //! \ref rev_rpo_cmp.
    //!
    //! Call operator that compares \p x and \p y using
    //! \ref rev_rpo_cmp.
    //!
    //! \tparam Word the type of the objects to be compared.
    //!
    //! \param x const reference to the first object for comparison.
    //! \param y const reference to the second object for comparison.
    //!
    //! \returns The boolean value \c true if \p x is less than \p y with
    //! respect to the reversed recursive path ordering, and \c false otherwise.
    //!
    //! \exceptions
    //! \noexcept
    template <typename Word>
    [[nodiscard]] bool operator()(Word const& x, Word const& y) const noexcept {
      return rev_rpo_cmp(x, y);
    }

    //! \brief Call operator that compares two iterator ranges.
    //!
    //! \param first1 beginning iterator of first object for comparison.
    //! \param last1 ending iterator of first object for comparison.
    //! \param first2 beginning iterator of second object for comparison.
    //! \param last2 ending iterator of second object for comparison.
    //!
    //! \returns The boolean value \c true if the first range is less than
    //! the second range with respect to this comparison object, and \c false
    //! otherwise.
    //!
    //! \exceptions
    //! \noexcept
    template <typename Iterator>
    [[nodiscard]] bool operator()(Iterator first1,
                                  Iterator last1,
                                  Iterator first2,
                                  Iterator last2) const noexcept {
      return rev_rpo_cmp(first1, last1, first2, last2);
    }
  };  // struct RevRPOCmp<Default, true>

  template <>
  struct RevRPOCmp<Default, false> : RevRPOCmp<Default, true> {
    //! \brief Reinitialize the comparison object.
    //!
    //! This function has no effect because this specialization is stateless.
    //!
    //! \returns A reference to \c *this.
    //!
    //! \exceptions
    //! \noexcept
    RevRPOCmp& init() noexcept {
      return *this;
    }
  };  // struct RevRPOCmp<Default, false>

  //////////////////////////////////////////////////////////////////////
  // Wreath-product
  //////////////////////////////////////////////////////////////////////

  //! \brief Compare two objects of the same type using the wreath-product
  //! ordering without checks.
  //!
  //! Defined in `order.hpp`.
  //!
  //! This function compares two objects of the same type using a
  //! wreath-product of len-lex comparisons as described in \cite Sims1994aa
  //! (Chapter 2.1). Generators are assigned levels. Differences between
  //! generators at higher levels dominate differences at lower levels.
  //! Differences within the same level are determined by len-lex.
  //!
  //! Suppose that \f$X\f$ is the disjoint union of non-empty sets
  //! \f$X_1, \dots, X_n\f$ referred to as levels, and for \f$1 \leq i \leq
  //! n\f$, let \f$<_i\f$ be a len-lex ordering of \f$X_i\f$. We define \f$<\f$
  //! to be \f$<_1 \wr \dots \wr <_n\f$. Next suppose that \f$U, V\in X ^
  //! {*}\f$. If \f$U\f$ and \f$V\f$ have a common prefix, that is \f$U = AB\f$
  //! and \f$V = AC\f$, then \f$U < V\f$ if and only if \f$B < C\f$. Therefore,
  //! we may assume that \f$U\f$ and \f$V\f$ do not share a common prefix.
  //!
  //! Let \f$m\f$ be the largest index such that \f$U\f$ contains a generator
  //! at level \f$m\f$.Therefore, we may write \f$U = A_0x_1A_1\dots
  //! x_{s-1}A_{s-1}x_sA_s\f$ where \f$x_i \in X_m\f$ and \f$A_i \in \{X_1 \cup
  //! \dots X_{m-1}\} ^ *\f$. Similarly, we may write \f$V = B_0y_1B_1\dots
  //! y_{t-1}B_{t-1}y_tB_t\f$ where \f$y_i \in X_n\f$ and \f$B_i \in \{X_1 \cup
  //! \dots X_{n-1}\} ^ *\f$ for a maximal choice of \f$n\f$. Then \f$U<V\f$ and
  //! only if one of the following conditions hold:
  //! 1. \f$m < n\f$; or
  //! 2. \f$m = n \f$ and \f$x_1x_2 \dots x_s <_m y_1y_2 \dots y_t\f$; or
  //! 3. \f$m = n \f$, \f$x_1x_2 \dots x_s = y_1y_2 \dots y_t\f$ and
  //! \f$A_0 < B_0\f$.
  //!
  //! The implementation of this function is inspired by the source code of
  //! \cite Holt2018aa, specifically the function `wreath_compare`.
  //!
  //! In the case where each generator has a unique level, this function
  //! produces the same output as \ref rev_rpo_cmp. In the case where each
  //! generator has the same level, this function produces the same output as
  //! \ref lenlex_cmp.
  //!
  //! \tparam Iterator the type of iterators that are the arguments.
  //!
  //! \param levels the level of each generator.
  //! \param first1 beginning iterator of first object for comparison.
  //! \param last1 ending iterator of first object for comparison.
  //! \param first2 beginning iterator of second object for comparison.
  //! \param last2 ending iterator of second object for comparison.
  //!
  //! \returns The boolean value \c true if the range `[first1, last1)` is less
  //! than the range `[first2, last2)` with respect to the wreath-product
  //! ordering, and \c false otherwise.
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  //!
  //! \warning
  //! This function has significantly worse performance than all
  //! the variants of \ref lenlex_cmp and std::lexicographical_compare.
  //!
  //! \warning No checks on the arguments are performed, and it is required
  //! that every letter pointed at by the iterators is less than the
  //! length of \p levels.
  template <typename Iterator>
  [[nodiscard]] bool wreath_cmp_no_checks(std::vector<size_t> const& levels,
                                          Iterator                   first1,
                                          Iterator                   last1,
                                          Iterator                   first2,
                                          Iterator                   last2);

  //! \brief Compare two ranges using the wreath-product ordering without
  //! checks and with a specified alphabet.
  //!
  //! This overload is the same as
  //! \ref wreath_cmp_no_checks(std::vector<size_t> const&, Iterator, Iterator,
  //! Iterator, Iterator), except that letters are mapped to indices using
  //! \p alphabet.
  //!
  //! \tparam Word the type of words for \p alphabet.
  //! \tparam Iterator the type of iterators that are the arguments.
  //!
  //! \param alphabet the alphabet used to map letters to level indices.
  //! \param levels the level of each generator.
  //! \param first1 beginning iterator of first object for comparison.
  //! \param last1 ending iterator of first object for comparison.
  //! \param first2 beginning iterator of second object for comparison.
  //! \param last2 ending iterator of second object for comparison.
  //!
  //! \returns The boolean value \c true if the range `[first1, last1)` is less
  //! than the range `[first2, last2)` with respect to the wreath-product
  //! ordering, and \c false otherwise.
  //!
  //! \warning It is not checked that the letters belong to \p alphabet or
  //! that their indices are valid indices into \p levels.
  template <typename Word, typename Iterator>
  [[nodiscard]] bool wreath_cmp_no_checks(Alphabet<Word> const&      alphabet,
                                          std::vector<size_t> const& levels,
                                          Iterator                   first1,
                                          Iterator                   last1,
                                          Iterator                   first2,
                                          Iterator                   last2);

  //! \brief Compare two objects of the same type using
  //! \ref wreath_cmp_no_checks without checks.
  //!
  //! Defined in `order.hpp`.
  //!
  //! This function compares two objects of the same type using the
  //! wreath-product ordering determined by \p levels.
  //!
  //! \tparam Thing the type of the objects to be compared.
  //!
  //! \param levels the level of each generator.
  //! \param x const reference to the first object for comparison.
  //! \param y const reference to the second object for comparison.
  //!
  //! \returns The boolean value \c true if \p x is less than \p y with
  //! respect to the wreath-product ordering, and \c false otherwise.
  //!
  //! \exceptions
  //! See \ref wreath_cmp_no_checks(std::vector<size_t> const&, Iterator,
  //! Iterator, Iterator, Iterator).
  //!
  //! \complexity
  //! At most \f$O(n + m)\f$ where \f$n\f$ is the length of \p x and \f$m\f$
  //! is the length of \p y.
  //!
  //! \par Possible Implementation
  //! \code_no_test
  //! wreath_cmp_no_checks(
  //!   levels, x.cbegin(), x.cend(), y.cbegin(), y.cend());
  //! \end_code_no_test
  //!
  //! \warning
  //! No checks on the arguments are performed, and it is required that every
  //! letter in \p x and \p y is less than the length of \p levels.
  //!
  //! \sa
  //! wreath_cmp_no_checks(std::vector<size_t> const&, Iterator, Iterator,
  //! Iterator, Iterator).
  template <typename Thing>
  [[nodiscard]] bool wreath_cmp_no_checks(std::vector<size_t> const& levels,
                                          Thing const&               x,
                                          Thing const&               y) {
    return wreath_cmp_no_checks(
        levels, x.cbegin(), x.cend(), y.cbegin(), y.cend());
  }

  //! \brief Compare two objects using the wreath-product ordering without
  //! checks and with a specified alphabet.
  //!
  //! \tparam Word the type of the objects and the alphabet.
  //!
  //! \param alphabet the alphabet used to map letters to level indices.
  //! \param levels the level of each generator.
  //! \param x const reference to the first object for comparison.
  //! \param y const reference to the second object for comparison.
  //!
  //! \returns The boolean value \c true if \p x is less than \p y with
  //! respect to the wreath-product ordering, and \c false otherwise.
  //!
  //! \warning It is not checked that the letters in \p x and \p y belong to
  //! \p alphabet or that their indices are valid indices into \p levels.
  template <typename Word>
  [[nodiscard]] bool wreath_cmp_no_checks(Alphabet<Word> const&      alphabet,
                                          std::vector<size_t> const& levels,
                                          Word const&                x,
                                          Word const&                y) {
    return wreath_cmp_no_checks(
        alphabet, levels, x.cbegin(), x.cend(), y.cbegin(), y.cend());
  }

  //! \brief Compare two ranges using the wreath-product ordering and check
  //! validity.
  //!
  //! Defined in `order.hpp`.
  //!
  //! After checking that every letter in both ranges is a valid index into
  //! \p levels, this function performs the same comparison as
  //! \ref wreath_cmp_no_checks.
  //!
  //! \tparam Iterator the type of iterators that are the arguments.
  //!
  //! \param levels the level of each generator.
  //! \param first1 beginning iterator of first object for comparison.
  //! \param last1 ending iterator of first object for comparison.
  //! \param first2 beginning iterator of second object for comparison.
  //! \param last2 ending iterator of second object for comparison.
  //!
  //! \returns The boolean value \c true if the range `[first1, last1)` is less
  //! than the range `[first2, last2)` with respect to the wreath-product
  //! ordering, and \c false otherwise.
  //!
  //! \throws LibsemigroupsException if any letter in either range is greater
  //! than or equal to `levels.size()`.
  //!
  //! \complexity
  //! At most \f$O(n + m)\f$ where \f$n\f$ is the distance between \p first1
  //! and \p last1, and \f$m\f$ is the distance between \p first2 and \p last2.
  //!
  //! \sa
  //! wreath_cmp_no_checks(std::vector<size_t> const&, Iterator, Iterator,
  //! Iterator, Iterator).
  template <typename Iterator>
  [[nodiscard]] bool wreath_cmp(std::vector<size_t> const& levels,
                                Iterator                   first1,
                                Iterator                   last1,
                                Iterator                   first2,
                                Iterator                   last2);

  //! \brief Compare two ranges using the wreath-product ordering, check
  //! validity, and use a specified alphabet.
  //!
  //! This overload is the same as
  //! \ref wreath_cmp(std::vector<size_t> const&, Iterator, Iterator, Iterator,
  //! Iterator), except that letters are mapped to indices using \p alphabet.
  //!
  //! \tparam Word the type of words for \p alphabet.
  //! \tparam Iterator the type of iterators that are the arguments.
  //!
  //! \param alphabet the alphabet used to map letters to level indices.
  //! \param levels the level of each generator.
  //! \param first1 beginning iterator of first object for comparison.
  //! \param last1 ending iterator of first object for comparison.
  //! \param first2 beginning iterator of second object for comparison.
  //! \param last2 ending iterator of second object for comparison.
  //!
  //! \returns The boolean value \c true if the range `[first1, last1)` is less
  //! than the range `[first2, last2)` with respect to the wreath-product
  //! ordering, and \c false otherwise.
  //!
  //! \throws LibsemigroupsException if a letter does not belong to
  //! \p alphabet or its index is greater than or equal to `levels.size()`.
  template <typename Word, typename Iterator>
  [[nodiscard]] bool wreath_cmp(Alphabet<Word> const&      alphabet,
                                std::vector<size_t> const& levels,
                                Iterator                   first1,
                                Iterator                   last1,
                                Iterator                   first2,
                                Iterator                   last2);

  //! \brief Compare two objects of the same type using the wreath-product
  //! ordering and check validity.
  //!
  //! Defined in `order.hpp`.
  //!
  //! After checking that every letter in both objects is a valid index into
  //! \p levels, this function performs the same comparison as
  //! \ref wreath_cmp_no_checks.
  //!
  //! \tparam Thing the type of the objects to be compared.
  //!
  //! \param levels the level of each generator.
  //! \param x const reference to the first object for comparison.
  //! \param y const reference to the second object for comparison.
  //!
  //! \returns The boolean value \c true if \p x is less than \p y with
  //! respect to the wreath-product ordering, and \c false otherwise.
  //!
  //! \throws LibsemigroupsException if any letter in \p x or \p y is greater
  //! than or equal to `levels.size()`.
  //!
  //! \complexity
  //! At most \f$O(n + m)\f$ where \f$n\f$ is the length of \p x and \f$m\f$
  //! is the length of \p y.
  //!
  //! \par Possible Implementation
  //! \code_no_test
  //! wreath_cmp(levels, x.cbegin(), x.cend(), y.cbegin(), y.cend());
  //! \end_code_no_test
  //!
  //! \sa
  //! wreath_cmp(std::vector<size_t> const&, Iterator, Iterator, Iterator,
  //! Iterator).
  template <typename Thing>
  [[nodiscard]] bool wreath_cmp(std::vector<size_t> const& levels,
                                Thing const&               x,
                                Thing const&               y) {
    return wreath_cmp(levels, x.cbegin(), x.cend(), y.cbegin(), y.cend());
  }

  //! \brief Compare two objects using the wreath-product ordering, check
  //! validity, and use a specified alphabet.
  //!
  //! \tparam Word the type of the objects and the alphabet.
  //!
  //! \param alphabet the alphabet used to map letters to level indices.
  //! \param levels the level of each generator.
  //! \param x const reference to the first object for comparison.
  //! \param y const reference to the second object for comparison.
  //!
  //! \returns The boolean value \c true if \p x is less than \p y with
  //! respect to the wreath-product ordering, and \c false otherwise.
  //!
  //! \throws LibsemigroupsException if a letter in \p x or \p y does not
  //! belong to \p alphabet or its index is greater than or equal to
  //! `levels.size()`.
  template <typename Word>
  [[nodiscard]] bool wreath_cmp(Alphabet<Word> const&      alphabet,
                                std::vector<size_t> const& levels,
                                Word const&                x,
                                Word const&                y) {
    return wreath_cmp(
        alphabet, levels, x.cbegin(), x.cend(), y.cbegin(), y.cend());
  }

  //////////////////////////////////////////////////////////////////////
  // WreathCmp
  //////////////////////////////////////////////////////////////////////

  namespace detail {

    template <typename Word>
    void throw_if_incompat_weights_or_levels(
        Alphabet<Word> const&      alphabet,
        std::vector<size_t> const& weights_or_levels,
        std::string_view           msg);
  }  // namespace detail

  //! \brief Forward declaration of \ref WreathCmp.
  template <typename Word = Default, bool check = true>
  class WreathCmp;

  //! \brief Stateful wreath-product comparison functor.
  //!
  //! This class stores an alphabet and a levels vector and compares words by
  //! applying \ref wreath_cmp with that alphabet and levels vector. The
  //! alphabet and levels vector must have the same size.
  //!
  //! \tparam Word the word type associated with the alphabet.
  //! \tparam check whether to check that letters belong to the alphabet.
  template <typename Word, bool check>
  class WreathCmp {
    Alphabet<Word>      _alphabet;
    std::vector<size_t> _levels;

   public:
    //! \brief Deleted default constructor.
    WreathCmp() = delete;

    //! \brief Copy constructor.
    WreathCmp(WreathCmp const&) = default;

    //! \brief Move constructor.
    WreathCmp(WreathCmp&&) = default;

    //! \brief Copy assignment operator.
    WreathCmp& operator=(WreathCmp const&) = default;

    //! \brief Move assignment operator.
    WreathCmp& operator=(WreathCmp&&) = default;

    //! \brief Destructor.
    ~WreathCmp() = default;

    //! \brief Construct from an alphabet and levels vector.
    //!
    //! Constructs a comparison object that stores copies of \p alphabet and
    //! \p levels. The `i`th entry of \p levels is the level of the `i`th
    //! letter in \p alphabet.
    //!
    //! \param alphabet the alphabet used to map letters to level indices.
    //! \param levels the level of each generator.
    //!
    //! \throws LibsemigroupsException if \p alphabet and \p levels do not
    //! have the same size.
    WreathCmp(Alphabet<Word> const& alphabet, std::vector<size_t> const& levels)
        : _alphabet(alphabet), _levels(levels) {
      detail::throw_if_incompat_weights_or_levels(_alphabet, _levels, "levels");
    }

    //! \brief Construct from an alphabet rvalue and levels vector rvalue.
    //!
    //! Constructs a comparison object by moving from \p alphabet and
    //! \p levels.
    //!
    //! \param alphabet the alphabet used to map letters to level indices.
    //! \param levels the level of each generator.
    //!
    //! \throws LibsemigroupsException if \p alphabet and \p levels do not
    //! have the same size.
    WreathCmp(Alphabet<Word>&& alphabet, std::vector<size_t>&& levels)
        : _alphabet(std::move(alphabet)), _levels(std::move(levels)) {
      detail::throw_if_incompat_weights_or_levels(_alphabet, _levels, "levels");
    }

    //! \brief Reinitialize from an alphabet and levels vector.
    //!
    //! \param alphabet the alphabet used to map letters to level indices.
    //! \param levels the level of each generator.
    //!
    //! \returns A reference to \c *this.
    //!
    //! \throws LibsemigroupsException if \p alphabet and \p levels do not
    //! have the same size.
    WreathCmp& init(Alphabet<Word> const&      alphabet,
                    std::vector<size_t> const& levels);

    //! \brief Reinitialize from an alphabet rvalue and levels vector rvalue.
    //!
    //! \param alphabet the alphabet used to map letters to level indices.
    //! \param levels the level of each generator.
    //!
    //! \returns A reference to \c *this.
    //!
    //! \throws LibsemigroupsException if \p alphabet and \p levels do not
    //! have the same size.
    WreathCmp& init(Alphabet<Word>&& alphabet, std::vector<size_t>&& levels);

    //! \brief Compare two words using wreath-product order.
    //!
    //! \param x const reference to the first word for comparison.
    //! \param y const reference to the second word for comparison.
    //!
    //! \returns The boolean value \c true if \p x is less than \p y, and
    //! \c false otherwise.
    //!
    //! \throws LibsemigroupsException if \c check is \c true and a letter in
    //! \p x or \p y does not belong to the stored alphabet.
    [[nodiscard]] bool operator()(Word const& x, Word const& y) const {
      return operator()(x.cbegin(), x.cend(), y.cbegin(), y.cend());
    }

    //! \brief Compare two iterator ranges using wreath-product order.
    //!
    //! \param first1 beginning iterator of first object for comparison.
    //! \param last1 ending iterator of first object for comparison.
    //! \param first2 beginning iterator of second object for comparison.
    //! \param last2 ending iterator of second object for comparison.
    //!
    //! \returns The boolean value \c true if the first range is less than
    //! the second range, and \c false otherwise.
    //!
    //! \throws LibsemigroupsException if \c check is \c true and a letter in
    //! either range does not belong to the stored alphabet.
    template <typename Iterator>
    [[nodiscard]] bool operator()(Iterator first1,
                                  Iterator last1,
                                  Iterator first2,
                                  Iterator last2) const {
      if constexpr (check) {
        _alphabet.throw_if_letter_not_in_alphabet(first1, last1);
        _alphabet.throw_if_letter_not_in_alphabet(first2, last2);
      }
      return wreath_cmp_no_checks(
          _alphabet, _levels, first1, last1, first2, last2);
    }

    //! \brief Returns the alphabet.
    //!
    //! \returns The stored alphabet.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] Alphabet<Word> const& alphabet() const noexcept {
      return _alphabet;
    }

    //! \brief Returns the levels.
    //!
    //! \returns The stored levels vector.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] std::vector<size_t> const& levels() const noexcept {
      return _levels;
    }
  };  // class WreathCmp

  //! \brief Stateful wreath-product comparison functor.
  //!
  //! This specialization stores a levels vector and compares words whose
  //! letters are indices by applying \ref wreath_cmp.
  //!
  //! \tparam check whether to check that letters are valid indices into the
  //! levels vector.
  template <bool check>
  class WreathCmp<Default, check> {
    std::vector<size_t> _levels;

   public:
    //! \brief Default constructor.
    //!
    //! Constructs a comparison object with an empty levels vector.
    WreathCmp() = default;

    //! \brief Copy constructor.
    WreathCmp(WreathCmp const&) = default;

    //! \brief Move constructor.
    WreathCmp(WreathCmp&&) = default;

    //! \brief Copy assignment operator.
    WreathCmp& operator=(WreathCmp const&) = default;

    //! \brief Move assignment operator.
    WreathCmp& operator=(WreathCmp&&) = default;

    //! \brief Destructor.
    ~WreathCmp() = default;

    //! \brief Reinitialize the comparison object.
    //!
    //! Clears the stored levels vector.
    //!
    //! \returns A reference to \c *this.
    //!
    //! \exceptions
    //! \noexcept
    WreathCmp& init() noexcept {
      _levels.clear();
      return *this;
    }

    //! \brief Construct from a levels vector reference.
    //!
    //! \param levels the level of each generator.
    explicit WreathCmp(std::vector<size_t> const& levels) : _levels(levels) {}

    //! \brief Construct from a levels vector rvalue reference.
    //!
    //! \param levels the level of each generator.
    explicit WreathCmp(std::vector<size_t>&& levels)
        : _levels(std::move(levels)) {}

    //! \brief Reinitialize from a levels vector reference.
    //!
    //! \param levels the level of each generator.
    //!
    //! \returns A reference to \c *this.
    WreathCmp& init(std::vector<size_t> const& levels) {
      _levels = levels;
      return *this;
    }

    //! \brief Reinitialize from a levels vector rvalue reference.
    //!
    //! \param levels the level of each generator.
    //!
    //! \returns A reference to \c *this.
    WreathCmp& init(std::vector<size_t>&& levels) {
      _levels = std::move(levels);
      return *this;
    }

    //! \brief Compare two objects using wreath-product order.
    //!
    //! \tparam Word the type of the objects to be compared.
    //! \param x const reference to the first object for comparison.
    //! \param y const reference to the second object for comparison.
    //!
    //! \returns The boolean value \c true if \p x is less than \p y, and
    //! \c false otherwise.
    //!
    //! \throws LibsemigroupsException if \c check is \c true and a letter is
    //! not a valid index into the levels vector.
    template <typename Word>
    [[nodiscard]] bool operator()(Word const& x, Word const& y) const {
      if constexpr (check) {
        return wreath_cmp(_levels, x, y);
      } else {
        return wreath_cmp_no_checks(_levels, x, y);
      }
    }

    //! \brief Compare two iterator ranges using wreath-product order.
    //!
    //! \param first1 beginning iterator of first object for comparison.
    //! \param last1 ending iterator of first object for comparison.
    //! \param first2 beginning iterator of second object for comparison.
    //! \param last2 ending iterator of second object for comparison.
    //!
    //! \returns The boolean value \c true if the first range is less than
    //! the second range, and \c false otherwise.
    //!
    //! \throws LibsemigroupsException if \c check is \c true and a letter is
    //! not a valid index into the levels vector.
    template <typename Iterator>
    [[nodiscard]] bool operator()(Iterator first1,
                                  Iterator last1,
                                  Iterator first2,
                                  Iterator last2) const {
      if constexpr (check) {
        return wreath_cmp(_levels, first1, last1, first2, last2);
      } else {
        return wreath_cmp_no_checks(_levels, first1, last1, first2, last2);
      }
    }

    //! \brief Returns the levels.
    //!
    //! \returns The stored levels vector.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] std::vector<size_t> const& levels() const noexcept {
      return _levels;
    }
  };  // class WreathCmp<Default, check>

  //! \brief Deduction guide from a levels vector reference.
  WreathCmp(std::vector<size_t> const&)->WreathCmp<>;

  //! \brief Deduction guide from a levels vector rvalue reference.
  WreathCmp(std::vector<size_t>&&)->WreathCmp<>;

  //! \brief Deduction guide from an alphabet and levels vector.
  template <typename Word>
  WreathCmp(Alphabet<Word> const&, std::vector<size_t> const&)
      -> WreathCmp<Word>;

  //! \brief Deduction guide from alphabet and levels vector rvalues.
  template <typename Word>
  WreathCmp(Alphabet<Word>&&, std::vector<size_t>&&) -> WreathCmp<Word>;

  //////////////////////////////////////////////////////////////////////
  // Reversed wreath-product order
  //////////////////////////////////////////////////////////////////////

  //! \brief Compare two ranges using reversed wreath-product order without
  //! checks.
  //!
  //! This function applies \ref wreath_cmp_no_checks to the ranges read from
  //! right to left.
  template <typename Iterator>
  [[nodiscard]] bool rev_wreath_cmp_no_checks(std::vector<size_t> const& levels,
                                              Iterator                   first1,
                                              Iterator                   last1,
                                              Iterator                   first2,
                                              Iterator last2) {
    return wreath_cmp_no_checks(levels,
                                std::make_reverse_iterator(last1),
                                std::make_reverse_iterator(first1),
                                std::make_reverse_iterator(last2),
                                std::make_reverse_iterator(first2));
  }

  //! \brief Compare two ranges using reversed wreath-product order without
  //! checks and with a specified alphabet.
  template <typename Word, typename Iterator>
  [[nodiscard]] bool rev_wreath_cmp_no_checks(Alphabet<Word> const& alphabet,
                                              std::vector<size_t> const& levels,
                                              Iterator                   first1,
                                              Iterator                   last1,
                                              Iterator                   first2,
                                              Iterator last2) {
    return wreath_cmp_no_checks(alphabet,
                                levels,
                                std::make_reverse_iterator(last1),
                                std::make_reverse_iterator(first1),
                                std::make_reverse_iterator(last2),
                                std::make_reverse_iterator(first2));
  }

  //! \brief Compare two objects using reversed wreath-product order without
  //! checks.
  template <typename Thing>
  [[nodiscard]] bool rev_wreath_cmp_no_checks(std::vector<size_t> const& levels,
                                              Thing const&               x,
                                              Thing const&               y) {
    return rev_wreath_cmp_no_checks(
        levels, x.cbegin(), x.cend(), y.cbegin(), y.cend());
  }

  //! \brief Compare two objects using reversed wreath-product order without
  //! checks and with a specified alphabet.
  template <typename Word>
  [[nodiscard]] bool rev_wreath_cmp_no_checks(Alphabet<Word> const& alphabet,
                                              std::vector<size_t> const& levels,
                                              Word const&                x,
                                              Word const&                y) {
    return rev_wreath_cmp_no_checks(
        alphabet, levels, x.cbegin(), x.cend(), y.cbegin(), y.cend());
  }

  //! \brief Compare two ranges using reversed wreath-product order and check
  //! validity.
  template <typename Iterator>
  [[nodiscard]] bool rev_wreath_cmp(std::vector<size_t> const& levels,
                                    Iterator                   first1,
                                    Iterator                   last1,
                                    Iterator                   first2,
                                    Iterator                   last2) {
    return wreath_cmp(levels,
                      std::make_reverse_iterator(last1),
                      std::make_reverse_iterator(first1),
                      std::make_reverse_iterator(last2),
                      std::make_reverse_iterator(first2));
  }

  //! \brief Compare two ranges using reversed wreath-product order and a
  //! specified alphabet.
  template <typename Word, typename Iterator>
  [[nodiscard]] bool rev_wreath_cmp(Alphabet<Word> const&      alphabet,
                                    std::vector<size_t> const& levels,
                                    Iterator                   first1,
                                    Iterator                   last1,
                                    Iterator                   first2,
                                    Iterator                   last2) {
    return wreath_cmp(alphabet,
                      levels,
                      std::make_reverse_iterator(last1),
                      std::make_reverse_iterator(first1),
                      std::make_reverse_iterator(last2),
                      std::make_reverse_iterator(first2));
  }

  //! \brief Compare two objects using reversed wreath-product order and check
  //! validity.
  template <typename Thing>
  [[nodiscard]] bool rev_wreath_cmp(std::vector<size_t> const& levels,
                                    Thing const&               x,
                                    Thing const&               y) {
    return rev_wreath_cmp(levels, x.cbegin(), x.cend(), y.cbegin(), y.cend());
  }

  //! \brief Compare two objects using reversed wreath-product order and a
  //! specified alphabet.
  template <typename Word>
  [[nodiscard]] bool rev_wreath_cmp(Alphabet<Word> const&      alphabet,
                                    std::vector<size_t> const& levels,
                                    Word const&                x,
                                    Word const&                y) {
    return rev_wreath_cmp(
        alphabet, levels, x.cbegin(), x.cend(), y.cbegin(), y.cend());
  }

  //! \brief Forward declaration of \ref RevWreathCmp.
  template <typename Word = Default, bool check = true>
  class RevWreathCmp;

  //! \brief Stateful reversed wreath-product comparison functor.
  template <typename Word, bool check>
  class RevWreathCmp {
    WreathCmp<Word, check> _wreath;

   public:
    RevWreathCmp()                               = delete;
    RevWreathCmp(RevWreathCmp const&)            = default;
    RevWreathCmp(RevWreathCmp&&)                 = default;
    RevWreathCmp& operator=(RevWreathCmp const&) = default;
    RevWreathCmp& operator=(RevWreathCmp&&)      = default;
    ~RevWreathCmp()                              = default;

    //! \brief Construct from an alphabet and levels vector.
    RevWreathCmp(Alphabet<Word> const&      alphabet,
                 std::vector<size_t> const& levels)
        : _wreath(alphabet, levels) {}

    //! \brief Construct from alphabet and levels vector rvalues.
    RevWreathCmp(Alphabet<Word>&& alphabet, std::vector<size_t>&& levels)
        : _wreath(std::move(alphabet), std::move(levels)) {}

    //! \brief Reinitialize from an alphabet and levels vector.
    RevWreathCmp& init(Alphabet<Word> const&      alphabet,
                       std::vector<size_t> const& levels) {
      _wreath.init(alphabet, levels);
      return *this;
    }

    //! \brief Reinitialize from alphabet and levels vector rvalues.
    RevWreathCmp& init(Alphabet<Word>&&      alphabet,
                       std::vector<size_t>&& levels) {
      _wreath.init(std::move(alphabet), std::move(levels));
      return *this;
    }

    //! \brief Compare two words using reversed wreath-product order.
    [[nodiscard]] bool operator()(Word const& x, Word const& y) const {
      return operator()(x.cbegin(), x.cend(), y.cbegin(), y.cend());
    }

    //! \brief Compare two iterator ranges using reversed wreath-product order.
    template <typename Iterator>
    [[nodiscard]] bool operator()(Iterator first1,
                                  Iterator last1,
                                  Iterator first2,
                                  Iterator last2) const {
      return _wreath(std::make_reverse_iterator(last1),
                     std::make_reverse_iterator(first1),
                     std::make_reverse_iterator(last2),
                     std::make_reverse_iterator(first2));
    }

    //! \brief Returns the alphabet.
    [[nodiscard]] Alphabet<Word> const& alphabet() const noexcept {
      return _wreath.alphabet();
    }

    //! \brief Returns the levels.
    [[nodiscard]] std::vector<size_t> const& levels() const noexcept {
      return _wreath.levels();
    }
  };  // class RevWreathCmp

  //! \brief Reversed wreath-product comparison functor using index words.
  template <bool check>
  class RevWreathCmp<Default, check> {
    WreathCmp<Default, check> _wreath;

   public:
    RevWreathCmp()                               = default;
    RevWreathCmp(RevWreathCmp const&)            = default;
    RevWreathCmp(RevWreathCmp&&)                 = default;
    RevWreathCmp& operator=(RevWreathCmp const&) = default;
    RevWreathCmp& operator=(RevWreathCmp&&)      = default;
    ~RevWreathCmp()                              = default;

    //! \brief Construct from a levels vector.
    explicit RevWreathCmp(std::vector<size_t> const& levels)
        : _wreath(levels) {}

    //! \brief Construct from a levels vector rvalue.
    explicit RevWreathCmp(std::vector<size_t>&& levels)
        : _wreath(std::move(levels)) {}

    //! \brief Reinitialize with an empty levels vector.
    RevWreathCmp& init() noexcept {
      _wreath.init();
      return *this;
    }

    //! \brief Reinitialize from a levels vector.
    RevWreathCmp& init(std::vector<size_t> const& levels) {
      _wreath.init(levels);
      return *this;
    }

    //! \brief Reinitialize from a levels vector rvalue.
    RevWreathCmp& init(std::vector<size_t>&& levels) {
      _wreath.init(std::move(levels));
      return *this;
    }

    //! \brief Compare two words using reversed wreath-product order.
    template <typename Word>
    [[nodiscard]] bool operator()(Word const& x, Word const& y) const {
      return operator()(x.cbegin(), x.cend(), y.cbegin(), y.cend());
    }

    //! \brief Compare two iterator ranges using reversed wreath-product order.
    template <typename Iterator>
    [[nodiscard]] bool operator()(Iterator first1,
                                  Iterator last1,
                                  Iterator first2,
                                  Iterator last2) const {
      return _wreath(std::make_reverse_iterator(last1),
                     std::make_reverse_iterator(first1),
                     std::make_reverse_iterator(last2),
                     std::make_reverse_iterator(first2));
    }

    //! \brief Returns the levels.
    [[nodiscard]] std::vector<size_t> const& levels() const noexcept {
      return _wreath.levels();
    }
  };  // class RevWreathCmp<Default, check>

  //! \brief Deduction guide from a levels vector.
  RevWreathCmp(std::vector<size_t> const&)->RevWreathCmp<>;

  //! \brief Deduction guide from a levels vector rvalue.
  RevWreathCmp(std::vector<size_t>&&)->RevWreathCmp<>;

  //! \brief Deduction guide from an alphabet and levels vector.
  template <typename Word>
  RevWreathCmp(Alphabet<Word> const&, std::vector<size_t> const&)
      -> RevWreathCmp<Word>;

  //! \brief Deduction guide from alphabet and levels vector rvalues.
  template <typename Word>
  RevWreathCmp(Alphabet<Word>&&, std::vector<size_t>&&) -> RevWreathCmp<Word>;

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
  //! \param weights the weights vector.
  //! \param first1 beginning iterator of first object for comparison.
  //! \param last1 ending iterator of first object for comparison.
  //! \param first2 beginning iterator of second object for comparison.
  //! \param last2 ending iterator of second object for comparison.
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
  //! wt_lenlex_cmp(std::vector<size_t> const&, Iterator, Iterator, Iterator,
  //! Iterator).
  template <typename Iterator>
  [[nodiscard]] bool wt_lenlex_cmp_no_checks(std::vector<size_t> const& weights,
                                             Iterator                   first1,
                                             Iterator                   last1,
                                             Iterator                   first2,
                                             Iterator                   last2);

  //! \brief Compare two ranges using the weighted len-lex ordering without
  //! checks and with a specified alphabet.
  //!
  //! This overload is the same as
  //! \ref wt_lenlex_cmp_no_checks(std::vector<size_t> const&, Iterator,
  //! Iterator, Iterator, Iterator), except that letters are first mapped to
  //! indices using \p alphabet.
  //!
  //! \tparam Word the type of words for \p alphabet.
  //! \tparam Iterator the type of iterators that are the arguments.
  //!
  //! \param alphabet the alphabet used to map letters to weight indices.
  //! \param weights the weights vector.
  //! \param first1 beginning iterator of first object for comparison.
  //! \param last1 ending iterator of first object for comparison.
  //! \param first2 beginning iterator of second object for comparison.
  //! \param last2 ending iterator of second object for comparison.
  //!
  //! \returns The boolean value \c true if the range `[first1, last1)` is
  //! weighted len-lex less than the range `[first2, last2)`, and \c false
  //! otherwise.
  //!
  //! \warning
  //! It is not checked that the letters in the ranges belong to \p alphabet or
  //! that their indices are valid indices into the \p weights vector.
  template <typename Word, typename Iterator>
  [[nodiscard]] bool wt_lenlex_cmp_no_checks(Alphabet<Word> const& alphabet,
                                             std::vector<size_t> const& weights,
                                             Iterator                   first1,
                                             Iterator                   last1,
                                             Iterator                   first2,
                                             Iterator                   last2);

  //! \brief Compare two objects of the same type using
  //! \ref wt_lenlex_cmp_no_checks without checks.
  //!
  //! Defined in `order.hpp`.
  //!
  //! This function compares two objects of the same type using
  //! \ref wt_lenlex_cmp_no_checks, where the `i`th index of the weights
  //! vector corresponds to the weight of the `i`th letter in the alphabet.
  //!
  //! \tparam Word the type of the objects to be compared.
  //!
  //! \param weights the weights vector.
  //! \param x const reference to the first object for comparison.
  //! \param y const reference to the second object for comparison.
  //!
  //! \returns The boolean value \c true if \p x is weighted len-lex less
  //! than \p y, and \c false otherwise.
  //!
  //! \exceptions
  //! See \ref wt_lenlex_cmp_no_checks(std::vector<size_t> const&, Iterator,
  //! Iterator, Iterator, Iterator).
  //!
  //! \complexity
  //! At most \f$O(n + m)\f$ where \f$n\f$ is the length of \p x and \f$m\f$
  //! is the length of \p y.
  //!
  //! \par Possible Implementation
  //! \code_no_test
  //! wt_lenlex_cmp_no_checks(
  //!   weights, x.cbegin(), x.cend(), y.cbegin(), y.cend());
  //! \end_code_no_test
  //!
  //! \warning
  //! It is not checked that the letters in \p x and \p y are valid indices
  //! into the weights vector.
  //!
  //! \sa
  //! wt_lenlex_cmp_no_checks(std::vector<size_t> const&, Iterator, Iterator,
  //! Iterator, Iterator).
  template <typename Word>
  [[nodiscard]] bool wt_lenlex_cmp_no_checks(std::vector<size_t> const& weights,
                                             Word const&                x,
                                             Word const&                y) {
    return wt_lenlex_cmp_no_checks(
        weights, x.cbegin(), x.cend(), y.cbegin(), y.cend());
  }

  //! \brief Compare two objects using
  //! \ref wt_lenlex_cmp_no_checks without checks and with a specified
  //! alphabet.
  //!
  //! \tparam Word the type of the objects to be compared.
  //!
  //! \param alphabet the alphabet used to map letters to weight indices.
  //! \param weights the weights vector.
  //! \param x const reference to the first object for comparison.
  //! \param y const reference to the second object for comparison.
  //!
  //! \returns The boolean value \c true if \p x is weighted len-lex less than
  //! \p y, and \c false otherwise.
  //!
  //! \warning
  //! It is not checked that the letters in \p x and \p y belong to \p alphabet
  //! or that their indices are valid indices into the \p weights vector.
  template <typename Word>
  [[nodiscard]] bool wt_lenlex_cmp_no_checks(Alphabet<Word> const& alphabet,
                                             std::vector<size_t> const& weights,
                                             Word const&                x,
                                             Word const&                y) {
    return wt_lenlex_cmp_no_checks(
        alphabet, weights, x.cbegin(), x.cend(), y.cbegin(), y.cend());
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
  //! `wt_lenlex_cmp_no_checks(weights, first1, last1, first2, last2)`.
  //!
  //! \tparam Iterator the type of iterators to the first object to be compared.
  //!
  //! \param weights the weights vector.
  //! \param first1 beginning iterator of first object for comparison.
  //! \param last1 ending iterator of first object for comparison.
  //! \param first2 beginning iterator of second object for comparison.
  //! \param last2 ending iterator of second object for comparison.
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
  //! wt_lenlex_cmp_no_checks(std::vector<size_t> const&, Iterator, Iterator,
  //! Iterator, Iterator).
  template <typename Iterator>
  [[nodiscard]] bool wt_lenlex_cmp(std::vector<size_t> const& weights,
                                   Iterator                   first1,
                                   Iterator                   last1,
                                   Iterator                   first2,
                                   Iterator                   last2);

  //! \brief Compare two ranges using the weighted len-lex ordering and with a
  //! specified alphabet.
  //!
  //! This overload is the same as
  //! \ref wt_lenlex_cmp(std::vector<size_t> const&, Iterator, Iterator,
  //! Iterator, Iterator), except that letters are first mapped to indices
  //! using \p alphabet.
  //!
  //! \tparam Word the type of words for \p alphabet.
  //! \tparam Iterator the type of iterators that are the arguments.
  //!
  //! \param alphabet the alphabet used to map letters to weight indices.
  //! \param weights the weights vector.
  //! \param first1 beginning iterator of first object for comparison.
  //! \param last1 ending iterator of first object for comparison.
  //! \param first2 beginning iterator of second object for comparison.
  //! \param last2 ending iterator of second object for comparison.
  //!
  //! \returns The boolean value \c true if the range `[first1, last1)` is
  //! weighted len-lex less than the range `[first2, last2)`, and \c false
  //! otherwise.
  //!
  //! \throws LibsemigroupsException if any letter in either range has an index
  //! greater than or equal to `weights.size()`.
  template <typename Word, typename Iterator>
  [[nodiscard]] bool wt_lenlex_cmp(Alphabet<Word> const&      alphabet,
                                   std::vector<size_t> const& weights,
                                   Iterator                   first1,
                                   Iterator                   last1,
                                   Iterator                   first2,
                                   Iterator                   last2);

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
  //! `wt_lenlex_cmp_no_checks(weights, x, y)`.
  //!
  //! \tparam Word the type of the objects to be compared.
  //!
  //! \param weights the weights vector.
  //! \param x const reference to the first object for comparison.
  //! \param y const reference to the second object for comparison.
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
  //!   weights, x.cbegin(), x.cend(), y.cbegin(), y.cend());
  //! \end_code_no_test
  //!
  //! \sa
  //! wt_lenlex_cmp(std::vector<size_t> const&, Iterator, Iterator, Iterator,
  //! Iterator).
  template <typename Word>
  [[nodiscard]] bool wt_lenlex_cmp(std::vector<size_t> const& weights,
                                   Word const&                x,
                                   Word const&                y) {
    return wt_lenlex_cmp(weights, x.cbegin(), x.cend(), y.cbegin(), y.cend());
  }

  //! \brief Compare two objects using \ref wt_lenlex_cmp and with a specified
  //! alphabet.
  //!
  //! \tparam Word the type of the objects to be compared.
  //!
  //! \param alphabet the alphabet used to map letters to weight indices.
  //! \param weights the weights vector.
  //! \param x const reference to the first object for comparison.
  //! \param y const reference to the second object for comparison.
  //!
  //! \returns The boolean value \c true if \p x is weighted len-lex less than
  //! \p y, and \c false otherwise.
  //!
  //! \throws LibsemigroupsException if any letter in \p x or \p y has an
  //! index greater than or equal to `weights.size()`.
  template <typename Word>
  [[nodiscard]] bool wt_lenlex_cmp(Alphabet<Word> const&      alphabet,
                                   std::vector<size_t> const& weights,
                                   Word const&                x,
                                   Word const&                y) {
    return wt_lenlex_cmp(
        alphabet, weights, x.cbegin(), x.cend(), y.cbegin(), y.cend());
  }

  ////////////////////////////////////////////////////////////////////////
  // WtLenLexCmp
  ////////////////////////////////////////////////////////////////////////

  //! \brief Forward declaration of \ref WtLenLexCmp.
  template <typename Word = Default, bool check = true>
  class WtLenLexCmp;

  //! \brief Stateful weighted len-lex comparison functor.
  //!
  //! This class stores an alphabet and a weights vector and compares words by
  //! applying \ref wt_lenlex_cmp with that alphabet and weights vector. The
  //! alphabet and weights vector must have the same size.
  //!
  //! \tparam Word the word type associated with the alphabet.
  //! \tparam check whether to check that letters belong to the alphabet.
  template <typename Word, bool check>
  class WtLenLexCmp {
    Alphabet<Word>      _alphabet;
    std::vector<size_t> _weights;

   public:
    //! \brief Deleted default constructor.
    WtLenLexCmp() = delete;

    //! \brief Copy constructor.
    WtLenLexCmp(WtLenLexCmp const&) = default;

    //! \brief Move constructor.
    WtLenLexCmp(WtLenLexCmp&&) = default;

    //! \brief Copy assignment operator.
    WtLenLexCmp& operator=(WtLenLexCmp const&) = default;

    //! \brief Move assignment operator.
    WtLenLexCmp& operator=(WtLenLexCmp&&) = default;

    ~WtLenLexCmp() = default;

    //! \brief Construct from an alphabet and weights vector.
    //!
    //! Constructs a comparison object that stores copies of \p alphabet and
    //! \p weights. The `i`th entry of \p weights is the weight of the `i`th
    //! letter in \p alphabet.
    //!
    //! \param alphabet the alphabet used to map letters to weight indices.
    //! \param weights the weights vector.
    //!
    //! \throws LibsemigroupsException if \p alphabet and \p weights do not
    //! have the same size.
    WtLenLexCmp(Alphabet<Word> const&      alphabet,
                std::vector<size_t> const& weights)
        : _alphabet(alphabet), _weights(weights) {
      detail::throw_if_incompat_weights_or_levels(
          _alphabet, _weights, "weights");
    }

    //! \brief Construct from an alphabet rvalue and weights vector rvalue.
    //!
    //! Constructs a comparison object that stores \p alphabet and \p weights by
    //! moving from the arguments.
    //!
    //! \param alphabet the alphabet used to map letters to weight indices.
    //! \param weights the weights vector.
    //!
    //! \throws LibsemigroupsException if \p alphabet and \p weights do not
    //! have the same size.
    WtLenLexCmp(Alphabet<Word>&& alphabet, std::vector<size_t>&& weights)
        : _alphabet(std::move(alphabet)), _weights(std::move(weights)) {
      detail::throw_if_incompat_weights_or_levels(
          _alphabet, _weights, "weights");
    }

    //! \brief Reinitialize from an alphabet and weights vector.
    //!
    //! Replaces the stored alphabet and weights vector with copies of
    //! \p alphabet and \p weights.
    //!
    //! \param alphabet the alphabet used to map letters to weight indices.
    //! \param weights the weights vector.
    //!
    //! \returns A reference to \c *this.
    //!
    //! \throws LibsemigroupsException if \p alphabet and \p weights do not
    //! have the same size.
    WtLenLexCmp& init(Alphabet<Word> const&      alphabet,
                      std::vector<size_t> const& weights);

    //! \brief Reinitialize from an alphabet rvalue and weights vector rvalue.
    //!
    //! Replaces the stored alphabet and weights vector by moving from
    //! \p alphabet and \p weights.
    //!
    //! \param alphabet the alphabet used to map letters to weight indices.
    //! \param weights the weights vector.
    //!
    //! \returns A reference to \c *this.
    //!
    //! \throws LibsemigroupsException if \p alphabet and \p weights do not
    //! have the same size.
    WtLenLexCmp& init(Alphabet<Word>&& alphabet, std::vector<size_t>&& weights);

    //! \brief Call operator that compares two words.
    //!
    //! \param x const reference to the first word for comparison.
    //! \param y const reference to the second word for comparison.
    //!
    //! \returns The boolean value \c true if \p x is weighted len-lex less
    //! than \p y, and \c false otherwise.
    //!
    //! \throws LibsemigroupsException if \c check is \c true and a letter in
    //! \p x or \p y does not belong to the stored alphabet.
    [[nodiscard]] bool operator()(Word const& x, Word const& y) const {
      return operator()(x.begin(), x.end(), y.begin(), y.end());
    }

    //! \brief Call operator that compares two iterator ranges.
    //!
    //! \param first1 beginning iterator of first object for comparison.
    //! \param last1 ending iterator of first object for comparison.
    //! \param first2 beginning iterator of second object for comparison.
    //! \param last2 ending iterator of second object for comparison.
    //!
    //! \returns The boolean value \c true if the first range is weighted
    //! len-lex less than the second range, and \c false otherwise.
    //!
    //! \throws LibsemigroupsException if \c check is \c true and a letter in
    //! either range does not belong to the stored alphabet.
    template <typename Iterator>
    [[nodiscard]] bool operator()(Iterator first1,
                                  Iterator last1,
                                  Iterator first2,
                                  Iterator last2) const {
      if constexpr (check) {
        // NOTE: only need to check that the iterators point at things
        // that are inbounds, don't need to check _alphabet and _weights are
        // compatible again.
        _alphabet.throw_if_letter_not_in_alphabet(first1, last1);
        _alphabet.throw_if_letter_not_in_alphabet(first2, last2);
      }
      return wt_lenlex_cmp_no_checks(
          _alphabet, _weights, first1, last1, first2, last2);
    }

    //! \brief Returns the alphabet.
    //!
    //! \returns The stored alphabet.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] Alphabet<Word> const& alphabet() const noexcept {
      return _alphabet;
    }

    //! \brief Returns the weights.
    //!
    //! \returns The stored weights vector.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] std::vector<size_t> const& weights() const noexcept {
      return _weights;
    }
  };  // class WtLenLexCmp

  //! \brief Stateful weighted len-lex comparison functor.
  //!
  //! Defined in `order.hpp`.
  //!
  //! This specialization stores a copy of a weights vector and compares words
  //! by applying \ref wt_lenlex_cmp. It can be used as a template parameter for
  //! standard library containers or algorithms that require a comparison
  //! functor.
  //!
  //! \tparam check whether to check that letters are valid indices into the
  //! weights vector.
  //!
  //! \sa
  //! * wt_lenlex_cmp(std::vector<size_t> const&, Word const&, Word const&)
  //! * wt_lenlex_cmp_no_checks(std::vector<size_t> const&, Word const&,
  //! Word const&)
  template <bool check>
  class WtLenLexCmp<Default, check> {
   private:
    std::vector<size_t> _weights;

   public:
    //! \brief Default constructor.
    //!
    //! Constructs a comparison object with an empty weights vector.
    WtLenLexCmp() = default;

    //! \brief Copy constructor.
    WtLenLexCmp(WtLenLexCmp const&) = default;

    //! \brief Move constructor.
    WtLenLexCmp(WtLenLexCmp&&) = default;

    //! \brief Copy assignment operator.
    WtLenLexCmp& operator=(WtLenLexCmp const&) = default;

    //! \brief Move assignment operator.
    WtLenLexCmp& operator=(WtLenLexCmp&&) = default;

    //! \brief Destructor.
    ~WtLenLexCmp() = default;

    //! \brief Reinitialize the comparison object.
    //!
    //! Clears the stored weights vector.
    //!
    //! \returns A reference to \c *this.
    //!
    //! \exceptions
    //! \noexcept
    WtLenLexCmp& init() noexcept {
      _weights.clear();
      return *this;
    }

    //! \brief Construct from weights vector reference.
    //!
    //! Constructs a comparison object that stores a copy of the provided
    //! weights vector, where the `i`th index corresponds to the weight of the
    //! `i`th letter in the alphabet.
    //!
    //! \param weights the weights vector.
    //! \exceptions
    //! \no_libsemigroups_except
    explicit WtLenLexCmp(std::vector<size_t> const& weights)
        : _weights(weights) {}

    //! \brief Reinitialize an existing WtLenLexCmp object.
    //!
    //! This function reinitializes an existing WtLenLexCmp object so that
    //! it is in the same state as if it was newly constructed using the same
    //! arguments.
    //!
    //! \param weights the weights vector.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    WtLenLexCmp& init(std::vector<size_t> const& weights) {
      _weights = weights;
      return *this;
    }

    //! \brief Construct from weights vector rvalue reference.
    //!
    //! Constructs a comparison object that takes ownership of the provided
    //! weights vector, where the `i`th index corresponds to the weight of the
    //! `i`th letter in the alphabet.
    //!
    //! \param weights the weights vector.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    explicit WtLenLexCmp(std::vector<size_t>&& weights)
        : _weights(std::move(weights)) {}

    //! \brief Reinitialize an existing WtLenLexCmp object.
    //!
    //! This function reinitializes an existing WtLenLexCmp object so that
    //! it is in the same state as if it was newly constructed using the same
    //! arguments.
    //!
    //! \param weights the weights vector.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    WtLenLexCmp& init(std::vector<size_t>&& weights) {
      _weights = std::move(weights);
      return *this;
    }

    //! \brief Call operator that compares \p x and \p y using
    //! \ref wt_lenlex_cmp.
    //!
    //! Call operator that compares \p x and \p y using \ref wt_lenlex_cmp.
    //!
    //! \tparam Word the type of the objects to be compared.
    //!
    //! \param x const reference to the first object for comparison.
    //! \param y const reference to the second object for comparison.
    //!
    //! \returns The boolean value \c true if \p x is weighted len-lex less
    //! than \p y, and \c false otherwise.
    //!
    //! \throws LibsemigroupsException if \c check is \c true and a letter is
    //! not a valid index into the weights vector.
    //!
    //! \complexity
    //! See
    //! * wt_lenlex_cmp(std::vector<size_t> const&, Iterator, Iterator,
    //! Iterator, Iterator);
    //! * wt_lenlex_cmp_no_checks(std::vector<size_t> const&, Iterator,
    //! Iterator, Iterator, Iterator).
    //!
    template <typename Word>
    [[nodiscard]] bool operator()(Word const& x, Word const& y) const {
      if constexpr (check) {
        return wt_lenlex_cmp(_weights, x, y);
      } else {
        return wt_lenlex_cmp_no_checks(_weights, x, y);
      }
    }

    //! \brief Call operator that compares two iterator ranges.
    //!
    //! \param first1 beginning iterator of first object for comparison.
    //! \param last1 ending iterator of first object for comparison.
    //! \param first2 beginning iterator of second object for comparison.
    //! \param last2 ending iterator of second object for comparison.
    //!
    //! \returns The boolean value \c true if the first range is weighted
    //! len-lex less than the second range, and \c false otherwise.
    //!
    //! \throws LibsemigroupsException if \c check is \c true and a letter is
    //! not a valid index into the weights vector.
    template <typename Iterator>
    [[nodiscard]] bool operator()(Iterator first1,
                                  Iterator last1,
                                  Iterator first2,
                                  Iterator last2) const {
      if constexpr (check) {
        return wt_lenlex_cmp(_weights, first1, last1, first2, last2);
      } else {
        return wt_lenlex_cmp_no_checks(_weights, first1, last1, first2, last2);
      }
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
  };  // class WtLenLexCmp<Default, check>

  //! \brief Deduction guide for constructing \ref WtLenLexCmp from weights.
  WtLenLexCmp(std::vector<size_t> const&)->WtLenLexCmp<>;

  //! \brief Deduction guide for constructing \ref WtLenLexCmp from weights.
  WtLenLexCmp(std::vector<size_t>&&)->WtLenLexCmp<>;

  //! \brief Deduction guide for constructing \ref WtLenLexCmp from an
  //! alphabet and weights.
  template <typename Word>
  WtLenLexCmp(Alphabet<Word> const&, std::vector<size_t> const&)
      -> WtLenLexCmp<Word>;

  //! \brief Deduction guide for constructing \ref WtLenLexCmp from an
  //! alphabet and weights.
  template <typename Word>
  WtLenLexCmp(Alphabet<Word>&&, std::vector<size_t>&&) -> WtLenLexCmp<Word>;

  //////////////////////////////////////////////////////////////////////
  // Reversed weighted len-lex
  //////////////////////////////////////////////////////////////////////

  //! \brief Compare two ranges using reversed weighted len-lex without
  //! checks.
  //!
  //! This function applies \ref wt_lenlex_cmp_no_checks to the ranges read
  //! from right to left.
  template <typename Iterator>
  [[nodiscard]] bool
  rev_wt_lenlex_cmp_no_checks(std::vector<size_t> const& weights,
                              Iterator                   first1,
                              Iterator                   last1,
                              Iterator                   first2,
                              Iterator                   last2) {
    return wt_lenlex_cmp_no_checks(weights,
                                   std::make_reverse_iterator(last1),
                                   std::make_reverse_iterator(first1),
                                   std::make_reverse_iterator(last2),
                                   std::make_reverse_iterator(first2));
  }

  //! \brief Compare two ranges using reversed weighted len-lex without
  //! checks and with a specified alphabet.
  template <typename Word, typename Iterator>
  [[nodiscard]] bool
  rev_wt_lenlex_cmp_no_checks(Alphabet<Word> const&      alphabet,
                              std::vector<size_t> const& weights,
                              Iterator                   first1,
                              Iterator                   last1,
                              Iterator                   first2,
                              Iterator                   last2) {
    return wt_lenlex_cmp_no_checks(alphabet,
                                   weights,
                                   std::make_reverse_iterator(last1),
                                   std::make_reverse_iterator(first1),
                                   std::make_reverse_iterator(last2),
                                   std::make_reverse_iterator(first2));
  }

  //! \brief Compare two objects using reversed weighted len-lex without
  //! checks.
  template <typename Word>
  [[nodiscard]] bool
  rev_wt_lenlex_cmp_no_checks(std::vector<size_t> const& weights,
                              Word const&                x,
                              Word const&                y) {
    return rev_wt_lenlex_cmp_no_checks(
        weights, x.cbegin(), x.cend(), y.cbegin(), y.cend());
  }

  //! \brief Compare two objects using reversed weighted len-lex without
  //! checks and with a specified alphabet.
  template <typename Word>
  [[nodiscard]] bool
  rev_wt_lenlex_cmp_no_checks(Alphabet<Word> const&      alphabet,
                              std::vector<size_t> const& weights,
                              Word const&                x,
                              Word const&                y) {
    return rev_wt_lenlex_cmp_no_checks(
        alphabet, weights, x.cbegin(), x.cend(), y.cbegin(), y.cend());
  }

  //! \brief Compare two ranges using reversed weighted len-lex and check
  //! validity.
  template <typename Iterator>
  [[nodiscard]] bool rev_wt_lenlex_cmp(std::vector<size_t> const& weights,
                                       Iterator                   first1,
                                       Iterator                   last1,
                                       Iterator                   first2,
                                       Iterator                   last2) {
    return wt_lenlex_cmp(weights,
                         std::make_reverse_iterator(last1),
                         std::make_reverse_iterator(first1),
                         std::make_reverse_iterator(last2),
                         std::make_reverse_iterator(first2));
  }

  //! \brief Compare two ranges using reversed weighted len-lex and a
  //! specified alphabet.
  template <typename Word, typename Iterator>
  [[nodiscard]] bool rev_wt_lenlex_cmp(Alphabet<Word> const&      alphabet,
                                       std::vector<size_t> const& weights,
                                       Iterator                   first1,
                                       Iterator                   last1,
                                       Iterator                   first2,
                                       Iterator                   last2) {
    return wt_lenlex_cmp(alphabet,
                         weights,
                         std::make_reverse_iterator(last1),
                         std::make_reverse_iterator(first1),
                         std::make_reverse_iterator(last2),
                         std::make_reverse_iterator(first2));
  }

  //! \brief Compare two objects using reversed weighted len-lex and check
  //! validity.
  template <typename Word>
  [[nodiscard]] bool rev_wt_lenlex_cmp(std::vector<size_t> const& weights,
                                       Word const&                x,
                                       Word const&                y) {
    return rev_wt_lenlex_cmp(
        weights, x.cbegin(), x.cend(), y.cbegin(), y.cend());
  }

  //! \brief Compare two objects using reversed weighted len-lex and a
  //! specified alphabet.
  template <typename Word>
  [[nodiscard]] bool rev_wt_lenlex_cmp(Alphabet<Word> const&      alphabet,
                                       std::vector<size_t> const& weights,
                                       Word const&                x,
                                       Word const&                y) {
    return rev_wt_lenlex_cmp(
        alphabet, weights, x.cbegin(), x.cend(), y.cbegin(), y.cend());
  }

  //! \brief Forward declaration of \ref RevWtLenLexCmp.
  template <typename Word = Default, bool check = true>
  class RevWtLenLexCmp;

  //! \brief Stateful reversed weighted len-lex comparison functor.
  template <typename Word, bool check>
  class RevWtLenLexCmp {
    WtLenLexCmp<Word, check> _wt_lenlex;

   public:
    RevWtLenLexCmp()                                 = delete;
    RevWtLenLexCmp(RevWtLenLexCmp const&)            = default;
    RevWtLenLexCmp(RevWtLenLexCmp&&)                 = default;
    RevWtLenLexCmp& operator=(RevWtLenLexCmp const&) = default;
    RevWtLenLexCmp& operator=(RevWtLenLexCmp&&)      = default;
    ~RevWtLenLexCmp()                                = default;

    //! \brief Construct from an alphabet and weights vector.
    RevWtLenLexCmp(Alphabet<Word> const&      alphabet,
                   std::vector<size_t> const& weights)
        : _wt_lenlex(alphabet, weights) {}

    //! \brief Construct from an alphabet and weights vector rvalues.
    RevWtLenLexCmp(Alphabet<Word>&& alphabet, std::vector<size_t>&& weights)
        : _wt_lenlex(std::move(alphabet), std::move(weights)) {}

    //! \brief Reinitialize from an alphabet and weights vector.
    RevWtLenLexCmp& init(Alphabet<Word> const&      alphabet,
                         std::vector<size_t> const& weights) {
      _wt_lenlex.init(alphabet, weights);
      return *this;
    }

    //! \brief Reinitialize from alphabet and weights vector rvalues.
    RevWtLenLexCmp& init(Alphabet<Word>&&      alphabet,
                         std::vector<size_t>&& weights) {
      _wt_lenlex.init(std::move(alphabet), std::move(weights));
      return *this;
    }

    //! \brief Compare two words using reversed weighted len-lex.
    [[nodiscard]] bool operator()(Word const& x, Word const& y) const {
      return operator()(x.cbegin(), x.cend(), y.cbegin(), y.cend());
    }

    //! \brief Compare two iterator ranges using reversed weighted len-lex.
    template <typename Iterator>
    [[nodiscard]] bool operator()(Iterator first1,
                                  Iterator last1,
                                  Iterator first2,
                                  Iterator last2) const {
      return _wt_lenlex(std::make_reverse_iterator(last1),
                        std::make_reverse_iterator(first1),
                        std::make_reverse_iterator(last2),
                        std::make_reverse_iterator(first2));
    }

    //! \brief Returns the alphabet.
    [[nodiscard]] Alphabet<Word> const& alphabet() const noexcept {
      return _wt_lenlex.alphabet();
    }

    //! \brief Returns the weights.
    [[nodiscard]] std::vector<size_t> const& weights() const noexcept {
      return _wt_lenlex.weights();
    }
  };  // class RevWtLenLexCmp

  //! \brief Reversed weighted len-lex comparison functor using index words.
  template <bool check>
  class RevWtLenLexCmp<Default, check> {
    WtLenLexCmp<Default, check> _wt_lenlex;

   public:
    RevWtLenLexCmp()                                 = default;
    RevWtLenLexCmp(RevWtLenLexCmp const&)            = default;
    RevWtLenLexCmp(RevWtLenLexCmp&&)                 = default;
    RevWtLenLexCmp& operator=(RevWtLenLexCmp const&) = default;
    RevWtLenLexCmp& operator=(RevWtLenLexCmp&&)      = default;
    ~RevWtLenLexCmp()                                = default;

    //! \brief Construct from a weights vector.
    explicit RevWtLenLexCmp(std::vector<size_t> const& weights)
        : _wt_lenlex(weights) {}

    //! \brief Construct from a weights vector rvalue.
    explicit RevWtLenLexCmp(std::vector<size_t>&& weights)
        : _wt_lenlex(std::move(weights)) {}

    //! \brief Reinitialize with an empty weights vector.
    RevWtLenLexCmp& init() noexcept {
      _wt_lenlex.init();
      return *this;
    }

    //! \brief Reinitialize from a weights vector.
    RevWtLenLexCmp& init(std::vector<size_t> const& weights) {
      _wt_lenlex.init(weights);
      return *this;
    }

    //! \brief Reinitialize from a weights vector rvalue.
    RevWtLenLexCmp& init(std::vector<size_t>&& weights) {
      _wt_lenlex.init(std::move(weights));
      return *this;
    }

    //! \brief Compare two words using reversed weighted len-lex.
    template <typename Word>
    [[nodiscard]] bool operator()(Word const& x, Word const& y) const {
      return operator()(x.cbegin(), x.cend(), y.cbegin(), y.cend());
    }

    //! \brief Compare two iterator ranges using reversed weighted len-lex.
    template <typename Iterator>
    [[nodiscard]] bool operator()(Iterator first1,
                                  Iterator last1,
                                  Iterator first2,
                                  Iterator last2) const {
      return _wt_lenlex(std::make_reverse_iterator(last1),
                        std::make_reverse_iterator(first1),
                        std::make_reverse_iterator(last2),
                        std::make_reverse_iterator(first2));
    }

    //! \brief Returns the weights.
    [[nodiscard]] std::vector<size_t> const& weights() const noexcept {
      return _wt_lenlex.weights();
    }
  };  // class RevWtLenLexCmp<Default, check>

  //! \brief Deduction guide from a weights vector.
  RevWtLenLexCmp(std::vector<size_t> const&)->RevWtLenLexCmp<>;

  //! \brief Deduction guide from a weights vector rvalue.
  RevWtLenLexCmp(std::vector<size_t>&&)->RevWtLenLexCmp<>;

  //! \brief Deduction guide from an alphabet and weights vector.
  template <typename Word>
  RevWtLenLexCmp(Alphabet<Word> const&, std::vector<size_t> const&)
      -> RevWtLenLexCmp<Word>;

  //! \brief Deduction guide from alphabet and weights vector rvalues.
  template <typename Word>
  RevWtLenLexCmp(Alphabet<Word>&&, std::vector<size_t>&&)
      -> RevWtLenLexCmp<Word>;

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
  //! \param weights the weights vector.
  //! \param first1 beginning iterator of first object for comparison.
  //! \param last1 ending iterator of first object for comparison.
  //! \param first2 beginning iterator of second object for comparison.
  //! \param last2 ending iterator of second object for comparison.
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
  //! wt_lex_cmp(std::vector<size_t> const&, Iterator, Iterator, Iterator,
  //! Iterator).
  template <typename Iterator>
  [[nodiscard]] bool wt_lex_cmp_no_checks(std::vector<size_t> const& weights,
                                          Iterator                   first1,
                                          Iterator                   last1,
                                          Iterator                   first2,
                                          Iterator                   last2);

  //! \brief Compare two ranges using weighted lex without checking an alphabet.
  //!
  //! This overload maps letters through \p alphabet before applying weighted
  //! lex order. It does not check that the letters in the ranges belong to
  //! \p alphabet or that their indices are valid for \p weights.
  //!
  //! \param alphabet the alphabet used to map letters to weight indices.
  //! \param weights the weights vector.
  //! \param first1 beginning iterator of first object for comparison.
  //! \param last1 ending iterator of first object for comparison.
  //! \param first2 beginning iterator of second object for comparison.
  //! \param last2 ending iterator of second object for comparison.
  //!
  //! \returns The boolean value \c true if the first range is weighted lex
  //! less than the second range, and \c false otherwise.
  template <typename Word, typename Iterator>
  [[nodiscard]] bool wt_lex_cmp_no_checks(Alphabet<Word> const&      alphabet,
                                          std::vector<size_t> const& weights,
                                          Iterator                   first1,
                                          Iterator                   last1,
                                          Iterator                   first2,
                                          Iterator                   last2);

  //! \brief Compare two objects of the same type using
  //! \ref wt_lex_cmp_no_checks without checks.
  //!
  //! Defined in `order.hpp`.
  //!
  //! This function compares two objects of the same type using
  //! \ref wt_lex_cmp_no_checks, where the `i`th index of the weights
  //! vector corresponds to the weight of the `i`th letter in the alphabet.
  //!
  //! \tparam Word the type of the objects to be compared.
  //!
  //! \param weights the weights vector.
  //! \param x const reference to the first object for comparison.
  //! \param y const reference to the second object for comparison.
  //!
  //! \returns The boolean value \c true if \p x is weighted lex less
  //! than \p y, and \c false otherwise.
  //!
  //! \exceptions
  //! See \ref wt_lex_cmp_no_checks(std::vector<size_t> const&, Iterator,
  //! Iterator, Iterator, Iterator).
  //!
  //! \complexity
  //! At most \f$O(n + m)\f$ where \f$n\f$ is the length of \p x and \f$m\f$
  //! is the length of \p y.
  //!
  //! \par Possible Implementation
  //! \code_no_test
  //! wt_lex_cmp_no_checks(
  //!   weights, x.cbegin(), x.cend(), y.cbegin(), y.cend());
  //! \end_code_no_test
  //!
  //! \warning
  //! It is not checked that the letters in \p x and \p y are valid indices
  //! into the weights vector.
  //!
  //! \sa
  //! wt_lex_cmp_no_checks(std::vector<size_t> const&, Iterator, Iterator,
  //! Iterator, Iterator).
  template <typename Word>
  [[nodiscard]] bool wt_lex_cmp_no_checks(std::vector<size_t> const& weights,
                                          Word const&                x,
                                          Word const&                y) {
    return wt_lex_cmp_no_checks(
        weights, x.cbegin(), x.cend(), y.cbegin(), y.cend());
  }

  //! \brief Compare two objects using weighted lex without checking an
  //! alphabet.
  //!
  //! This overload maps letters through \p alphabet before applying weighted
  //! lex order. It does not check that letters in \p x or \p y belong to
  //! \p alphabet or that their indices are valid for \p weights.
  //!
  //! \param alphabet the alphabet used to map letters to weight indices.
  //! \param weights the weights vector.
  //! \param x const reference to the first object for comparison.
  //! \param y const reference to the second object for comparison.
  //!
  //! \returns The boolean value \c true if \p x is weighted lex less than
  //! \p y, and \c false otherwise.
  template <typename Word>
  [[nodiscard]] bool wt_lex_cmp_no_checks(Alphabet<Word> const&      alphabet,
                                          std::vector<size_t> const& weights,
                                          Word const&                x,
                                          Word const&                y) {
    return wt_lex_cmp_no_checks(
        alphabet, weights, x.cbegin(), x.cend(), y.cbegin(), y.cend());
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
  //! `wt_lex_cmp_no_checks(weights, first1, last1, first2, last2)`.
  //!
  //! \tparam Iterator the type of iterators to the first object to be compared.
  //!
  //! \param weights the weights vector.
  //! \param first1 beginning iterator of first object for comparison.
  //! \param last1 ending iterator of first object for comparison.
  //! \param first2 beginning iterator of second object for comparison.
  //! \param last2 ending iterator of second object for comparison.
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
  //! wt_lex_cmp_no_checks(std::vector<size_t> const&, Iterator, Iterator,
  //! Iterator, Iterator)
  template <typename Iterator>
  [[nodiscard]] bool wt_lex_cmp(std::vector<size_t> const& weights,
                                Iterator                   first1,
                                Iterator                   last1,
                                Iterator                   first2,
                                Iterator                   last2);

  //! \brief Compare two ranges using weighted lex and an alphabet.
  //!
  //! This overload checks that both ranges contain only letters belonging to
  //! \p alphabet and that their alphabet indices are valid for \p weights,
  //! then applies weighted lex order.
  //!
  //! \param alphabet the alphabet used to map letters to weight indices.
  //! \param weights the weights vector.
  //! \param first1 beginning iterator of first object for comparison.
  //! \param last1 ending iterator of first object for comparison.
  //! \param first2 beginning iterator of second object for comparison.
  //! \param last2 ending iterator of second object for comparison.
  //!
  //! \returns The boolean value \c true if the first range is weighted lex
  //! less than the second range, and \c false otherwise.
  //!
  //! \throws LibsemigroupsException if any letter is invalid for \p alphabet
  //! or \p weights.
  template <typename Word, typename Iterator>
  [[nodiscard]] bool wt_lex_cmp(Alphabet<Word> const&      alphabet,
                                std::vector<size_t> const& weights,
                                Iterator                   first1,
                                Iterator                   last1,
                                Iterator                   first2,
                                Iterator                   last2);

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
  //! `wt_lex_cmp_no_checks(weights, x, y)`.
  //!
  //! \tparam Word the type of the objects to be compared.
  //!
  //! \param weights the weights vector.
  //! \param x const reference to the first object for comparison.
  //! \param y const reference to the second object for comparison.
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
  //!   weights, x.cbegin(), x.cend(), y.cbegin(), y.cend());
  //! \end_code_no_test
  //!
  //! \sa
  //! wt_lex_cmp(std::vector<size_t> const&, Iterator, Iterator, Iterator,
  //! Iterator).
  template <typename Word>
  [[nodiscard]] bool wt_lex_cmp(std::vector<size_t> const& weights,
                                Word const&                x,
                                Word const&                y) {
    return wt_lex_cmp(weights, x.cbegin(), x.cend(), y.cbegin(), y.cend());
  }

  //! \brief Compare two objects using weighted lex and an alphabet.
  //!
  //! \param alphabet the alphabet used to map letters to weight indices.
  //! \param weights the weights vector.
  //! \param x const reference to the first object for comparison.
  //! \param y const reference to the second object for comparison.
  //!
  //! \returns The boolean value \c true if \p x is weighted lex less than
  //! \p y, and \c false otherwise.
  //!
  //! \throws LibsemigroupsException if any letter is invalid for \p alphabet
  //! or \p weights.
  template <typename Word>
  [[nodiscard]] bool wt_lex_cmp(Alphabet<Word> const&      alphabet,
                                std::vector<size_t> const& weights,
                                Word const&                x,
                                Word const&                y) {
    return wt_lex_cmp(
        alphabet, weights, x.cbegin(), x.cend(), y.cbegin(), y.cend());
  }

  //////////////////////////////////////////////////////////////////////
  // Length then weighted lex
  //////////////////////////////////////////////////////////////////////

  //! \brief Compare two ranges first by length and then by weighted lex
  //! without checks.
  //!
  //! If the ranges have different lengths, the shorter range is less. Ranges
  //! of equal length are compared using \ref wt_lex_cmp_no_checks.
  //!
  //! \param weights the weights vector.
  //! \param first1 beginning iterator of first object for comparison.
  //! \param last1 ending iterator of first object for comparison.
  //! \param first2 beginning iterator of second object for comparison.
  //! \param last2 ending iterator of second object for comparison.
  //!
  //! \returns The boolean value \c true if the first range is less than the
  //! second range, and \c false otherwise.
  template <typename Iterator>
  [[nodiscard]] bool
  len_wt_lex_cmp_no_checks(std::vector<size_t> const& weights,
                           Iterator                   first1,
                           Iterator                   last1,
                           Iterator                   first2,
                           Iterator                   last2);

  //! \brief Compare two ranges first by length and then by weighted lex
  //! without checks and with a specified alphabet.
  template <typename Word, typename Iterator>
  [[nodiscard]] bool
  len_wt_lex_cmp_no_checks(Alphabet<Word> const&      alphabet,
                           std::vector<size_t> const& weights,
                           Iterator                   first1,
                           Iterator                   last1,
                           Iterator                   first2,
                           Iterator                   last2);

  //! \brief Compare two objects first by length and then by weighted lex
  //! without checks.
  template <typename Word>
  [[nodiscard]] bool
  len_wt_lex_cmp_no_checks(std::vector<size_t> const& weights,
                           Word const&                x,
                           Word const&                y) {
    return len_wt_lex_cmp_no_checks(
        weights, x.cbegin(), x.cend(), y.cbegin(), y.cend());
  }

  //! \brief Compare two objects first by length and then by weighted lex
  //! without checks and with a specified alphabet.
  template <typename Word>
  [[nodiscard]] bool
  len_wt_lex_cmp_no_checks(Alphabet<Word> const&      alphabet,
                           std::vector<size_t> const& weights,
                           Word const&                x,
                           Word const&                y) {
    return len_wt_lex_cmp_no_checks(
        alphabet, weights, x.cbegin(), x.cend(), y.cbegin(), y.cend());
  }

  //! \brief Compare two ranges first by length and then by weighted lex and
  //! check validity.
  //!
  //! All letters are checked for compatibility with \p weights before the
  //! comparison is performed.
  template <typename Iterator>
  [[nodiscard]] bool len_wt_lex_cmp(std::vector<size_t> const& weights,
                                    Iterator                   first1,
                                    Iterator                   last1,
                                    Iterator                   first2,
                                    Iterator                   last2);

  //! \brief Compare two ranges first by length and then by weighted lex,
  //! check validity, and use a specified alphabet.
  template <typename Word, typename Iterator>
  [[nodiscard]] bool len_wt_lex_cmp(Alphabet<Word> const&      alphabet,
                                    std::vector<size_t> const& weights,
                                    Iterator                   first1,
                                    Iterator                   last1,
                                    Iterator                   first2,
                                    Iterator                   last2);

  //! \brief Compare two objects first by length and then by weighted lex and
  //! check validity.
  template <typename Word>
  [[nodiscard]] bool len_wt_lex_cmp(std::vector<size_t> const& weights,
                                    Word const&                x,
                                    Word const&                y) {
    return len_wt_lex_cmp(weights, x.cbegin(), x.cend(), y.cbegin(), y.cend());
  }

  //! \brief Compare two objects first by length and then by weighted lex,
  //! check validity, and use a specified alphabet.
  template <typename Word>
  [[nodiscard]] bool len_wt_lex_cmp(Alphabet<Word> const&      alphabet,
                                    std::vector<size_t> const& weights,
                                    Word const&                x,
                                    Word const&                y) {
    return len_wt_lex_cmp(
        alphabet, weights, x.cbegin(), x.cend(), y.cbegin(), y.cend());
  }

  //! \brief Forward declaration of \ref WtLexCmp.
  template <typename Word = Default, bool check = true>
  class WtLexCmp;

  //! \brief Stateful weighted lex comparison functor.
  //!
  //! Defined in `order.hpp`.
  //!
  //! This class stores an alphabet and a weights vector and compares words by
  //! applying \ref wt_lex_cmp with that alphabet and weights vector. The
  //! alphabet and weights vector must have the same size.
  //!
  //! \tparam Word the word type associated with the alphabet.
  //! \tparam check whether to check that letters belong to the alphabet.
  //!
  //! \sa
  //! * wt_lex_cmp(std::vector<size_t> const&, Word const&, Word const&)
  //! * wt_lex_cmp_no_checks(std::vector<size_t> const&, Word const&,
  //! Word const&)
  template <typename Word, bool check>
  class WtLexCmp {
    Alphabet<Word>      _alphabet;
    std::vector<size_t> _weights;

   public:
    //! \brief Deleted default constructor.
    WtLexCmp() = delete;

    //! \brief Copy constructor.
    WtLexCmp(WtLexCmp const&) = default;

    //! \brief Move constructor.
    WtLexCmp(WtLexCmp&&) = default;

    //! \brief Copy assignment operator.
    WtLexCmp& operator=(WtLexCmp const&) = default;

    //! \brief Move assignment operator.
    WtLexCmp& operator=(WtLexCmp&&) = default;

    //! \brief Destructor.
    ~WtLexCmp() = default;

    //! \brief Construct from an alphabet and weights vector.
    //!
    //! Constructs a comparison object that stores copies of \p alphabet and
    //! \p weights. The `i`th entry of \p weights is the weight of the `i`th
    //! letter in \p alphabet.
    //!
    //! \param alphabet the alphabet used to map letters to weight indices.
    //! \param weights the weights vector.
    //!
    //! \throws LibsemigroupsException if \p alphabet and \p weights do not
    //! have the same size.
    WtLexCmp(Alphabet<Word> const& alphabet, std::vector<size_t> const& weights)
        : _alphabet(alphabet), _weights(weights) {
      detail::throw_if_incompat_weights_or_levels(
          _alphabet, _weights, "weights");
    }

    //! \brief Construct from an alphabet rvalue and weights vector rvalue.
    //!
    //! Constructs a comparison object that stores \p alphabet and \p weights by
    //! moving from the arguments.
    //!
    //! \param alphabet the alphabet used to map letters to weight indices.
    //! \param weights the weights vector.
    //!
    //! \throws LibsemigroupsException if \p alphabet and \p weights do not
    //! have the same size.
    WtLexCmp(Alphabet<Word>&& alphabet, std::vector<size_t>&& weights)
        : _alphabet(std::move(alphabet)), _weights(std::move(weights)) {
      detail::throw_if_incompat_weights_or_levels(
          _alphabet, _weights, "weights");
    }

    //! \brief Reinitialize from an alphabet and weights vector.
    //!
    //! Replaces the stored alphabet and weights vector with copies of
    //! \p alphabet and \p weights.
    //!
    //! \param alphabet the alphabet used to map letters to weight indices.
    //! \param weights the weights vector.
    //!
    //! \returns A reference to \c *this.
    //!
    //! \throws LibsemigroupsException if \p alphabet and \p weights do not
    //! have the same size.
    WtLexCmp& init(Alphabet<Word> const&      alphabet,
                   std::vector<size_t> const& weights);

    //! \brief Reinitialize from an alphabet rvalue and weights vector rvalue.
    //!
    //! Replaces the stored alphabet and weights vector by moving from
    //! \p alphabet and \p weights.
    //!
    //! \param alphabet the alphabet used to map letters to weight indices.
    //! \param weights the weights vector.
    //!
    //! \returns A reference to \c *this.
    //!
    //! \throws LibsemigroupsException if \p alphabet and \p weights do not
    //! have the same size.
    WtLexCmp& init(Alphabet<Word>&& alphabet, std::vector<size_t>&& weights);

    //! \brief Call operator that compares two words.
    //!
    //! \param x const reference to the first word for comparison.
    //! \param y const reference to the second word for comparison.
    //!
    //! \returns The boolean value \c true if \p x is weighted lex less than
    //! \p y, and \c false otherwise.
    //!
    //! \throws LibsemigroupsException if \c check is \c true and a letter in
    //! \p x or \p y does not belong to the stored alphabet.
    [[nodiscard]] bool operator()(Word const& x, Word const& y) const {
      return operator()(x.begin(), x.end(), y.begin(), y.end());
    }

    //! \brief Call operator that compares two iterator ranges.
    //!
    //! \param first1 beginning iterator of first object for comparison.
    //! \param last1 ending iterator of first object for comparison.
    //! \param first2 beginning iterator of second object for comparison.
    //! \param last2 ending iterator of second object for comparison.
    //!
    //! \returns The boolean value \c true if the first range is weighted lex
    //! less than the second range, and \c false otherwise.
    //!
    //! \throws LibsemigroupsException if \c check is \c true and a letter in
    //! either range does not belong to the stored alphabet.
    template <typename Iterator>
    [[nodiscard]] bool operator()(Iterator first1,
                                  Iterator last1,
                                  Iterator first2,
                                  Iterator last2) const {
      if constexpr (check) {
        // NOTE: only need to check that the iterators point at things
        // that are inbounds, don't need to check _alphabet and _weights are
        // compatible again, which we would if we called wt_lenlex_cmp below.
        _alphabet.throw_if_letter_not_in_alphabet(first1, last1);
        _alphabet.throw_if_letter_not_in_alphabet(first2, last2);
      }
      return wt_lex_cmp_no_checks(
          _alphabet, _weights, first1, last1, first2, last2);
    }

    //! \brief Returns the alphabet.
    //!
    //! \returns The stored alphabet.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] Alphabet<Word> const& alphabet() const noexcept {
      return _alphabet;
    }

    //! \brief Returns the weights.
    //!
    //! \returns The stored weights vector.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] std::vector<size_t> const& weights() const noexcept {
      return _weights;
    }
  };  // class WtLexCmp

  //! \brief Stateful weighted lex comparison functor.
  //!
  //! This specialization stores a copy of a weights vector and compares words
  //! by applying \ref wt_lex_cmp. It can be used as a template parameter for
  //! standard library containers or algorithms that require a comparison
  //! functor.
  //!
  //! \tparam check whether to check that letters are valid indices into the
  //! weights vector.
  template <bool check>
  class WtLexCmp<Default, check> {
   private:
    std::vector<size_t> _weights;

   public:
    //! \brief Default constructor.
    //!
    //! Constructs a comparison object with an empty weights vector.
    WtLexCmp() = default;

    //! \brief Copy constructor.
    WtLexCmp(WtLexCmp const&) = default;

    //! \brief Move constructor.
    WtLexCmp(WtLexCmp&&) = default;

    //! \brief Copy assignment operator.
    WtLexCmp& operator=(WtLexCmp const&) = default;

    //! \brief Move assignment operator.
    WtLexCmp& operator=(WtLexCmp&&) = default;

    //! \brief Destructor.
    ~WtLexCmp() = default;

    //! \brief Reinitialize the comparison object.
    //!
    //! Clears the stored weights vector.
    //!
    //! \returns A reference to \c *this.
    //!
    //! \exceptions
    //! \noexcept
    WtLexCmp& init() noexcept {
      _weights.clear();
      return *this;
    }

    //! \brief Construct from weights vector reference.
    //!
    //! Constructs a comparison object that stores a copy of the provided
    //! weights vector, where the `i`th index corresponds to the weight of the
    //! `i`th letter in the alphabet.
    //!
    //! \param weights the weights vector.
    //! \exceptions
    //! \no_libsemigroups_except
    explicit WtLexCmp(std::vector<size_t> const& weights) : _weights(weights) {}

    //! \brief Reinitialize an existing WtLexCmp object.
    //!
    //! This function reinitializes an existing WtLexCmp object so that
    //! it is in the same state as if it was newly constructed using the same
    //! arguments.
    //!
    //! \param weights the weights vector.
    //! \exceptions
    //! \no_libsemigroups_except
    WtLexCmp& init(std::vector<size_t> const& weights) {
      _weights = weights;
      return *this;
    }

    //! \brief Construct from weights vector rvalue reference.
    //!
    //! Constructs a comparison object that takes ownership of the provided
    //! weights vector, where the `i`th index corresponds to the weight of the
    //! `i`th letter in the alphabet.
    //!
    //! \param weights the weights vector.
    //! \exceptions
    //! \no_libsemigroups_except
    explicit WtLexCmp(std::vector<size_t>&& weights)
        : _weights(std::move(weights)) {}

    //! \brief Reinitialize an existing WtLexCmp object.
    //!
    //! This function reinitializes an existing WtLexCmp object so that
    //! it is in the same state as if it was newly constructed using the same
    //! arguments.
    //!
    //! \param weights the weights vector.
    //! \exceptions
    //! \no_libsemigroups_except
    WtLexCmp& init(std::vector<size_t>&& weights) {
      _weights = std::move(weights);
      return *this;
    }

    //! \brief Call operator that compares \p x and \p y using
    //! \ref wt_lex_cmp.
    //!
    //! Call operator that compares \p x and \p y using \ref wt_lex_cmp.
    //!
    //! \tparam Word the type of the objects to be compared.
    //!
    //! \param x const reference to the first object for comparison.
    //! \param y const reference to the second object for comparison.
    //!
    //! \returns The boolean value \c true if \p x is weighted lex less
    //! than \p y, and \c false otherwise.
    //!
    //! \throws LibsemigroupsException if \c check is \c true and a letter is
    //! not a valid index into the weights vector.
    //!
    //! \complexity
    //! See:
    //! * wt_lex_cmp(std::vector<size_t> const&, Iterator, Iterator, Iterator,
    //! Iterator)
    //! * wt_lex_cmp_no_checks(std::vector<size_t> const&, Iterator, Iterator,
    //! Iterator, Iterator).
    //!
    template <typename Word>
    [[nodiscard]] bool operator()(Word const& x, Word const& y) const {
      if constexpr (check) {
        return wt_lex_cmp(_weights, x, y);
      } else {
        return wt_lex_cmp_no_checks(_weights, x, y);
      }
    }

    //! \brief Call operator that compares two iterator ranges.
    //!
    //! \param first1 beginning iterator of first object for comparison.
    //! \param last1 ending iterator of first object for comparison.
    //! \param first2 beginning iterator of second object for comparison.
    //! \param last2 ending iterator of second object for comparison.
    //!
    //! \returns The boolean value \c true if the first range is weighted lex
    //! less than the second range, and \c false otherwise.
    //!
    //! \throws LibsemigroupsException if \c check is \c true and a letter is
    //! not a valid index into the weights vector.
    template <typename Iterator>
    [[nodiscard]] bool operator()(Iterator first1,
                                  Iterator last1,
                                  Iterator first2,
                                  Iterator last2) const {
      if constexpr (check) {
        return wt_lex_cmp(_weights, first1, last1, first2, last2);
      } else {
        return wt_lex_cmp_no_checks(_weights, first1, last1, first2, last2);
      }
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
  };  // class WtLexCmp<Default, check>

  //! \brief Deduction guide for constructing \ref WtLexCmp from weights.
  WtLexCmp(std::vector<size_t> const&)->WtLexCmp<>;

  //! \brief Deduction guide for constructing \ref WtLexCmp from weights.
  WtLexCmp(std::vector<size_t>&&)->WtLexCmp<>;

  //! \brief Deduction guide for constructing \ref WtLexCmp from an alphabet
  //! and weights.
  template <typename Word>
  WtLexCmp(Alphabet<Word> const&, std::vector<size_t> const&) -> WtLexCmp<Word>;

  //! \brief Deduction guide for constructing \ref WtLexCmp from an alphabet
  //! and weights.
  template <typename Word>
  WtLexCmp(Alphabet<Word>&&, std::vector<size_t>&&) -> WtLexCmp<Word>;

  //////////////////////////////////////////////////////////////////////
  // Reversed weighted lex
  //////////////////////////////////////////////////////////////////////

  //! \brief Compare two ranges using reversed weighted lex without checks.
  //!
  //! This function applies \ref wt_lex_cmp_no_checks to the ranges read from
  //! right to left.
  template <typename Iterator>
  [[nodiscard]] bool
  rev_wt_lex_cmp_no_checks(std::vector<size_t> const& weights,
                           Iterator                   first1,
                           Iterator                   last1,
                           Iterator                   first2,
                           Iterator                   last2) {
    return wt_lex_cmp_no_checks(weights,
                                std::make_reverse_iterator(last1),
                                std::make_reverse_iterator(first1),
                                std::make_reverse_iterator(last2),
                                std::make_reverse_iterator(first2));
  }

  //! \brief Compare two ranges using reversed weighted lex without checks and
  //! with a specified alphabet.
  template <typename Word, typename Iterator>
  [[nodiscard]] bool
  rev_wt_lex_cmp_no_checks(Alphabet<Word> const&      alphabet,
                           std::vector<size_t> const& weights,
                           Iterator                   first1,
                           Iterator                   last1,
                           Iterator                   first2,
                           Iterator                   last2) {
    return wt_lex_cmp_no_checks(alphabet,
                                weights,
                                std::make_reverse_iterator(last1),
                                std::make_reverse_iterator(first1),
                                std::make_reverse_iterator(last2),
                                std::make_reverse_iterator(first2));
  }

  //! \brief Compare two objects using reversed weighted lex without checks.
  template <typename Word>
  [[nodiscard]] bool
  rev_wt_lex_cmp_no_checks(std::vector<size_t> const& weights,
                           Word const&                x,
                           Word const&                y) {
    return rev_wt_lex_cmp_no_checks(
        weights, x.cbegin(), x.cend(), y.cbegin(), y.cend());
  }

  //! \brief Compare two objects using reversed weighted lex without checks and
  //! with a specified alphabet.
  template <typename Word>
  [[nodiscard]] bool
  rev_wt_lex_cmp_no_checks(Alphabet<Word> const&      alphabet,
                           std::vector<size_t> const& weights,
                           Word const&                x,
                           Word const&                y) {
    return rev_wt_lex_cmp_no_checks(
        alphabet, weights, x.cbegin(), x.cend(), y.cbegin(), y.cend());
  }

  //! \brief Compare two ranges using reversed weighted lex and check validity.
  template <typename Iterator>
  [[nodiscard]] bool rev_wt_lex_cmp(std::vector<size_t> const& weights,
                                    Iterator                   first1,
                                    Iterator                   last1,
                                    Iterator                   first2,
                                    Iterator                   last2) {
    return wt_lex_cmp(weights,
                      std::make_reverse_iterator(last1),
                      std::make_reverse_iterator(first1),
                      std::make_reverse_iterator(last2),
                      std::make_reverse_iterator(first2));
  }

  //! \brief Compare two ranges using reversed weighted lex and a specified
  //! alphabet.
  template <typename Word, typename Iterator>
  [[nodiscard]] bool rev_wt_lex_cmp(Alphabet<Word> const&      alphabet,
                                    std::vector<size_t> const& weights,
                                    Iterator                   first1,
                                    Iterator                   last1,
                                    Iterator                   first2,
                                    Iterator                   last2) {
    return wt_lex_cmp(alphabet,
                      weights,
                      std::make_reverse_iterator(last1),
                      std::make_reverse_iterator(first1),
                      std::make_reverse_iterator(last2),
                      std::make_reverse_iterator(first2));
  }

  //! \brief Compare two objects using reversed weighted lex and check
  //! validity.
  template <typename Word>
  [[nodiscard]] bool rev_wt_lex_cmp(std::vector<size_t> const& weights,
                                    Word const&                x,
                                    Word const&                y) {
    return rev_wt_lex_cmp(weights, x.cbegin(), x.cend(), y.cbegin(), y.cend());
  }

  //! \brief Compare two objects using reversed weighted lex and a specified
  //! alphabet.
  template <typename Word>
  [[nodiscard]] bool rev_wt_lex_cmp(Alphabet<Word> const&      alphabet,
                                    std::vector<size_t> const& weights,
                                    Word const&                x,
                                    Word const&                y) {
    return rev_wt_lex_cmp(
        alphabet, weights, x.cbegin(), x.cend(), y.cbegin(), y.cend());
  }

  //! \brief Forward declaration of \ref RevWtLexCmp.
  template <typename Word = Default, bool check = true>
  class RevWtLexCmp;

  //! \brief Stateful reversed weighted lex comparison functor.
  template <typename Word, bool check>
  class RevWtLexCmp {
    WtLexCmp<Word, check> _wt_lex;

   public:
    RevWtLexCmp()                              = delete;
    RevWtLexCmp(RevWtLexCmp const&)            = default;
    RevWtLexCmp(RevWtLexCmp&&)                 = default;
    RevWtLexCmp& operator=(RevWtLexCmp const&) = default;
    RevWtLexCmp& operator=(RevWtLexCmp&&)      = default;
    ~RevWtLexCmp()                             = default;

    //! \brief Construct from an alphabet and weights vector.
    RevWtLexCmp(Alphabet<Word> const&      alphabet,
                std::vector<size_t> const& weights)
        : _wt_lex(alphabet, weights) {}

    //! \brief Construct from an alphabet and weights vector rvalues.
    RevWtLexCmp(Alphabet<Word>&& alphabet, std::vector<size_t>&& weights)
        : _wt_lex(std::move(alphabet), std::move(weights)) {}

    //! \brief Reinitialize from an alphabet and weights vector.
    RevWtLexCmp& init(Alphabet<Word> const&      alphabet,
                      std::vector<size_t> const& weights) {
      _wt_lex.init(alphabet, weights);
      return *this;
    }

    //! \brief Reinitialize from alphabet and weights vector rvalues.
    RevWtLexCmp& init(Alphabet<Word>&&      alphabet,
                      std::vector<size_t>&& weights) {
      _wt_lex.init(std::move(alphabet), std::move(weights));
      return *this;
    }

    //! \brief Compare two words using reversed weighted lex.
    [[nodiscard]] bool operator()(Word const& x, Word const& y) const {
      return operator()(x.cbegin(), x.cend(), y.cbegin(), y.cend());
    }

    //! \brief Compare two iterator ranges using reversed weighted lex.
    template <typename Iterator>
    [[nodiscard]] bool operator()(Iterator first1,
                                  Iterator last1,
                                  Iterator first2,
                                  Iterator last2) const {
      return _wt_lex(std::make_reverse_iterator(last1),
                     std::make_reverse_iterator(first1),
                     std::make_reverse_iterator(last2),
                     std::make_reverse_iterator(first2));
    }

    //! \brief Returns the alphabet.
    [[nodiscard]] Alphabet<Word> const& alphabet() const noexcept {
      return _wt_lex.alphabet();
    }

    //! \brief Returns the weights.
    [[nodiscard]] std::vector<size_t> const& weights() const noexcept {
      return _wt_lex.weights();
    }
  };  // class RevWtLexCmp

  //! \brief Reversed weighted lex comparison functor using index words.
  template <bool check>
  class RevWtLexCmp<Default, check> {
    WtLexCmp<Default, check> _wt_lex;

   public:
    RevWtLexCmp()                              = default;
    RevWtLexCmp(RevWtLexCmp const&)            = default;
    RevWtLexCmp(RevWtLexCmp&&)                 = default;
    RevWtLexCmp& operator=(RevWtLexCmp const&) = default;
    RevWtLexCmp& operator=(RevWtLexCmp&&)      = default;
    ~RevWtLexCmp()                             = default;

    //! \brief Construct from a weights vector.
    explicit RevWtLexCmp(std::vector<size_t> const& weights)
        : _wt_lex(weights) {}

    //! \brief Construct from a weights vector rvalue.
    explicit RevWtLexCmp(std::vector<size_t>&& weights)
        : _wt_lex(std::move(weights)) {}

    //! \brief Reinitialize with an empty weights vector.
    RevWtLexCmp& init() noexcept {
      _wt_lex.init();
      return *this;
    }

    //! \brief Reinitialize from a weights vector.
    RevWtLexCmp& init(std::vector<size_t> const& weights) {
      _wt_lex.init(weights);
      return *this;
    }

    //! \brief Reinitialize from a weights vector rvalue.
    RevWtLexCmp& init(std::vector<size_t>&& weights) {
      _wt_lex.init(std::move(weights));
      return *this;
    }

    //! \brief Compare two words using reversed weighted lex.
    template <typename Word>
    [[nodiscard]] bool operator()(Word const& x, Word const& y) const {
      return operator()(x.cbegin(), x.cend(), y.cbegin(), y.cend());
    }

    //! \brief Compare two iterator ranges using reversed weighted lex.
    template <typename Iterator>
    [[nodiscard]] bool operator()(Iterator first1,
                                  Iterator last1,
                                  Iterator first2,
                                  Iterator last2) const {
      return _wt_lex(std::make_reverse_iterator(last1),
                     std::make_reverse_iterator(first1),
                     std::make_reverse_iterator(last2),
                     std::make_reverse_iterator(first2));
    }

    //! \brief Returns the weights.
    [[nodiscard]] std::vector<size_t> const& weights() const noexcept {
      return _wt_lex.weights();
    }
  };  // class RevWtLexCmp<Default, check>

  //! \brief Deduction guide from a weights vector.
  RevWtLexCmp(std::vector<size_t> const&)->RevWtLexCmp<>;

  //! \brief Deduction guide from a weights vector rvalue.
  RevWtLexCmp(std::vector<size_t>&&)->RevWtLexCmp<>;

  //! \brief Deduction guide from an alphabet and weights vector.
  template <typename Word>
  RevWtLexCmp(Alphabet<Word> const&, std::vector<size_t> const&)
      -> RevWtLexCmp<Word>;

  //! \brief Deduction guide from alphabet and weights vector rvalues.
  template <typename Word>
  RevWtLexCmp(Alphabet<Word>&&, std::vector<size_t>&&) -> RevWtLexCmp<Word>;

#include "order-deprecated.hpp"

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
  // TODO these should not only be specialised for Default
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
    template <bool check>
    struct is_length_non_increasing<LenLexCmp<Default, check>>
        : std::true_type {};

    //! \brief Reversed len-lex order is length non-increasing.
    template <bool check>
    struct is_length_non_increasing<RevLenLexCmp<Default, check>>
        : std::true_type {};

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
    template <bool check>
    struct is_well_founded<LenLexCmp<Default, check>> : std::true_type {};

    //! \brief Reversed len-lex order is well-founded.
    template <bool check>
    struct is_well_founded<RevLenLexCmp<Default, check>> : std::true_type {};

    //! \brief Recursive path order is well-founded.
    //!
    //! Specialization of \ref is_well_founded for \ref RPOCmp.
    template <bool check>
    struct is_well_founded<RPOCmp<Default, check>> : std::true_type {};

    //! \brief Reverse recursive path order is well-founded.
    //!
    //! Specialization of \ref is_well_founded for \ref RevRPOCmp.
    template <bool check>
    struct is_well_founded<RevRPOCmp<Default, check>> : std::true_type {};

    //! \brief Wreath-product order is well-founded.
    //!
    //! Specialization of \ref is_well_founded for \ref WreathCmp.
    template <bool check>
    struct is_well_founded<WreathCmp<Default, check>> : std::true_type {};

    //! \brief Reversed wreath-product order is well-founded.
    template <bool check>
    struct is_well_founded<RevWreathCmp<Default, check>> : std::true_type {};

    //! \brief Weighted short-lex order is well-founded.
    //!
    //! Specialization of \ref is_well_founded for \ref WtLenLexCmp.
    template <bool check>
    struct is_well_founded<WtLenLexCmp<Default, check>> : std::true_type {};

    //! \brief Reversed weighted len-lex order is well-founded.
    template <bool check>
    struct is_well_founded<RevWtLenLexCmp<Default, check>> : std::true_type {};

    //! \brief Weighted lex order is well-founded.
    //!
    //! Specialization of \ref is_well_founded for \ref WtLexCmp.
    template <bool check>
    struct is_well_founded<WtLexCmp<Default, check>> : std::true_type {};

    //! \brief Reversed weighted lex order is well-founded.
    template <bool check>
    struct is_well_founded<RevWtLexCmp<Default, check>> : std::true_type {};

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
