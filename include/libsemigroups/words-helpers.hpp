//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2026 Joseph Edwards
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

// This file contains declarations for functions related to words (counting, and
// converting) in libsemigroups.

#ifndef LIBSEMIGROUPS_WORDS_HELPERS_HPP_
#define LIBSEMIGROUPS_WORDS_HELPERS_HPP_

#include <cstddef>           // for size_t
#include <cstdint>           // for uint64_t, uint8_t
#include <initializer_list>  // for initializer_list
#include <limits>            // for numeric_limits
#include <string>            // for basic_string
#include <string_view>       // for string_view
#include <vector>            // for vector, operator==

#include "debug.hpp"      // for LIBSEMIGROUPS_ASSERT
#include "exception.hpp"  // for LibsemigroupsException
#include "ranges.hpp"     // for begin, end
#include "types.hpp"      // for word_type

namespace libsemigroups {
  namespace detail {
    std::string const& chars_in_human_readable_order();
  }

  //! \ingroup words_group
  //! \brief Reverse an object.
  //!
  //! This function just calls `std::reverse(w.begin(), w.end())`
  //! and is for convenience.
  //!
  //! \tparam Word.
  //!
  //! \param w the word to reverse.
  //!
  //! \returns A reference to the parameter \p w.
  template <typename Word>
  Word& reverse(Word&& w) {
    std::reverse(w.begin(), w.end());
    return w;
  }

  ////////////////////////////////////////////////////////////////////////
  // Words
  ////////////////////////////////////////////////////////////////////////

  //! \defgroup words_group Words
  //! This page contains details of the functionality in \c libsemigroups
  //! related to generating words in a given range and in a given order.
  //!
  //! This file contains documentation for functionality for:
  //!
  //! * generating words and strings in a given range and in a certain order:
  //!  - \ref WordRange
  //!  - \ref StringRange
  //!  - \ref random_word
  //!  - \ref random_string
  //!  - \ref random_strings
  //!
  //! * counting words:
  //!
  //!   - \ref number_of_words
  //!
  //! * converting to and from strings and words:
  //!
  //!   - \ref ToWord
  //!   - \ref ToString
  //!
  //! * parsing algebraic expressions in a string:
  //!
  //!   - \ref literal_operator_p "operator\"\"_p"

  //! \ingroup words_group
  //! \brief Returns the number of words over an alphabet with a given number of
  //! letters with length in a specified range.
  //!
  //! Returns the number of words over an alphabet with a given number of
  //! letters with length in a specified range.
  //!
  //! \param n the number of letters in the alphabet.
  //! \param min the minimum length of a word.
  //! \param max one greater than the maximum length of a word.
  //!
  //! \returns
  //! A value of type \c uint64_t.
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  //!
  //! \warning If the number of words exceeds 2 ^ 64 - 1, then the return value
  //! of this function will not be correct.
  [[nodiscard]] uint64_t number_of_words(size_t n, size_t min, size_t max);

  //! \ingroup words_group
  //! \brief Returns a random word.
  //!
  //! Returns a random word on \f$\{0, \ldots, n - 1\}\f$ of length \p length
  //! where \f$n\f$ is \p nr_letters.
  //!
  //! \param length the length of the word.
  //! \param nr_letters the size of the alphabet.
  //!
  //! \returns A random word, value of `word_type`.
  //!
  //! \throws LibsemigroupsException if \p nr_letters is \c 0.
  //!
  //! \sa \ref random_string
  [[nodiscard]] word_type random_word(size_t length, size_t nr_letters);

  ////////////////////////////////////////////////////////////////////////
  // Words
  ////////////////////////////////////////////////////////////////////////

  namespace detail {
    void throw_if_random_string_should_throw(std::string const& alphabet,
                                             size_t             min,
                                             size_t             max);
  }  // namespace detail

  //! \ingroup words_group
  //! \brief Returns a random string.
  //!
  //! Returns a random string with length \p length over alphabet \p alphabet.
  //!
  //! \param alphabet the alphabet over which the string is constructed.
  //! \param length the length of the string.
  //!
  //! \returns A random string, value of `std::string`.
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  //!
  //! \sa \ref random_word
  std::string random_string(std::string const& alphabet, size_t length);

  //! \ingroup words_group
  //! \brief Returns a random string.
  //!
  //! Returns a random string with random length in the range `[min, max)` over
  //! alphabet \p alphabet.
  //!
  //! \param alphabet the alphabet over which the string is constructed.
  //! \param min the minimum length of the returned string.
  //! \param max one above the maximum length of the returned string.
  //!
  //! \returns A random string, value of `std::string`.
  //!
  //! \throws LibsemigroupsException if either:
  //! * `min > max`; or
  //! * `alphabet.size() == 0` and `min != 0`.
  //!
  //! \sa \ref random_word
  std::string random_string(std::string const& alphabet,
                            size_t             min,
                            size_t             max);

  //! \ingroup words_group
  //! \brief Returns a range object of random strings.
  //!
  //! Returns a range object of random strings, each of which with random length
  //! in the range `[min, max)` over alphabet \p alphabet.
  //!
  //! \param alphabet the alphabet over which the string is constructed.
  //! \param number the number of random strings to construct.
  //! \param min the minimum length of the returned string.
  //! \param max one above the maximum length of the returned string.
  //!
  //! \returns A range of random strings.
  //!
  //! \throws LibsemigroupsException if either:
  //! * `min > max`; or
  //! * `alphabet.size() == 0` and `min != 0`.
  //!
  //! \sa \ref random_word
  auto inline random_strings(std::string const& alphabet,
                             size_t             number,
                             size_t             min,
                             size_t             max) {
    detail::throw_if_random_string_should_throw(alphabet, min, max);

    // Lambda must capture by copy, as the lambda will exist outside the scope
    // of this function once the range is returned.
    return rx::generate([alphabet, min, max] {
             return random_string(alphabet, min, max);
           })
           | rx::take(number);
  }

  ////////////////////////////////////////////////////////////////////////
  // Literals
  ////////////////////////////////////////////////////////////////////////

  //! \ingroup words_group
  //!
  //! \brief Namespace containing some custom literals for creating words.
  //!
  //! Defined in `word-range.hpp`.
  //!
  //! This namespace contains some functions for creating \ref word_type objects
  //! in a compact form.
  //! \par Example
  //! \code_no_test
  //! 012_w      //-> word_type({0, 1, 2})
  //! "abc"_w    //-> word_type({0, 1, 2})
  //! "(ab)^3"_p //-> "ababab"
  //! \end_code_no_test
  namespace literals {
    //! \anchor literal_operator_w
    //! \brief Literal for defining \ref word_type over integers less than 10.
    //!
    //! This operator provides a convenient brief means of constructing a
    //! \ref word_type from an sequence of literal integer digits or a string.
    //! For example, \c 0123_w produces the same output as
    //! `word_type({0, 1, 2, 3})` and so too does `"abcd"_w`.
    //!
    //! There are some gotchas and this operator should be used with some care:
    //!
    //! * the parameter \p w must consist of the integers
    //!   \f$\{0, \ldots, 9\}\f$ or the characters in `a-zA-Z` but not both.
    //! * if \p w starts with \c 0 and is follows by a value greater than \c 7,
    //!   then it is necessary to enclose \p w in quotes. For example, \c 08_w
    //!   will not compile because it is interpreted as an invalid octal.
    //!   However `"08"_w` behaves as expected.
    //! * if \p w consists of characters in `a-zA-Z`, then the output is
    //!   the same as that of `ToWord::operator()(w)`, see
    //!   \ref ToWord::operator()()
    //!
    //! \param w the letters of the word.
    //! \param n the length of \p w (defaults to the length of \p w).
    //!
    //! \returns A value of type \ref word_type.
    //!
    //! \throws LibsemigroupsException if the input contains a mixture of
    //! integers and non-integers.
    word_type operator""_w(const char* w, size_t n);

    //! \brief Literal for defining \ref word_type over integers less than 10.
    //!
    //! See \ref literal_operator_w "operator\"\"_w" for details.
    word_type operator""_w(const char* w);

    //! \anchor literal_operator_p
    //! \brief Literal for defining std::string by parsing an algebraic
    //! expression.
    //!
    //! This operator provides a convenient concise means of constructing a
    //! std::string from an algebraic expression.
    //! For example, \c "((ab)^3cc)^2"_p equals
    //! \c "abababccabababcc", \c "(ab,ba)" equals \c "BAABabba" and \c "a^0"_p
    //! equals the empty string \c "".
    //!
    //! This function has the following behaviour:
    //! * arbitrarily nested brackets;
    //! * spaces are ignored;
    //! * redundant matched brackets are ignored;
    //! * `^` is treated as the power binary operator;
    //! * `,` is treated as the commutator binary operator;
    //! * only the characters in `()^, ` and in \c a-zA-Z0-9 are allowed.
    //!
    //! When using `,` as the commutator operator, it is not possible to
    //! specify what the inverse of each letter should be. Instead, it is
    //! assumed that the inverse of a lowercase letter is the corresponding
    //! uppercase letter, and the inverse of an uppercase letter is the
    //! corresponding lowercase letter. If this is requirement is not applicable
    //! for your use case, see \ref presentation::commutator instead.
    //!
    //! Additionally, it is not possible to specify commutators using square
    //! brackets. Round brackets must be used instead.
    //!
    //! \param w the letters of the word.
    //! \param n the length of \p w (defaults to the length of \p w).
    //!
    //! \returns A value of type \ref std::string.
    //!
    //! \throws LibsemigroupsException if the string cannot be parsed.
    //!
    //! \sa words::parse
    std::string operator""_p(const char* w, size_t n);

    //! \brief Literal for defining \ref word_type by parsing an algebraic
    //! expression.
    //!
    //! See \ref literal_operator_p "operator\"\"_p" for details.
    std::string operator""_p(const char* w);

  }  // namespace literals

  ////////////////////////////////////////////////////////////////////////

  //! \ingroup words_group
  //!
  //! \brief Namespace containing some operators for creating words.
  //!
  //! Defined in `word-range.hpp`.
  //!
  //! This namespace contains some functions for creating \ref word_type objects
  //! in a compact form.
  //! \par Example
  //! \code_no_test
  //! using namespace words;
  //! pow("a", 5)            //-> "aaaaa"
  //! 01_w + 2               //-> 012_w
  //! 01_w + 01_w            //-> 0101_w
  //! prod(0123_w, 0, 16, 3) //-> 032103_w
  //! \end_code_no_test
  namespace words {

    //! \brief Non-literal version of \ref literal_operator_p "operator\"\"_p"
    //!
    //! This function returns the same value as
    //! \ref literal_operator_p "operator\"\"_p" and is included to allow things
    //! like:
    //!
    //! \code_no_test
    //! parse(fmt::format("baa(ba)^{}", N);
    //! \end_code_no_test
    //! for variable values of \c N.
    //!
    //! \param w the string to parse.
    //! \returns the parsed string.
    std::string parse(std::string const& w);

    //! \brief Returns the index of a character in human readable order.
    //!
    //! Defined in `word-range.hpp`.
    //!
    //! This function is the inverse of \ref words::human_readable_letter, see
    //! the documentation of that function for more details.
    //!
    //! \param c character whose index is sought.
    //!
    //! \returns A value of type \ref letter_type.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \sa human_readable_letter
    [[nodiscard]] letter_type human_readable_index(char c);

    //! \brief Returns a character by index in human readable order.
    //!
    //! This function exists to map the numbers \c 0 to \c 255 to the possible
    //! values of a \c char, in such a way that the first characters are
    //! \c a-zA-Z0-9. The ascii ranges for these characters are: \f$[97,
    //! 123)\f$, \f$[65, 91)\f$, \f$[48, 58)\f$ so the remaining range of chars
    //! that are appended to the end after these chars are \f$[0,48)\f$, \f$[58,
    //! 65)\f$, \f$[91, 97)\f$, \f$[123, 255]\f$.
    //!
    //! This function is the inverse of \ref words::human_readable_index.
    //!
    //! \param i the index of the character.
    //!
    //! \returns A value of type \c char.
    //!
    //! \throws LibsemigroupsException if \p i exceeds the number of characters.
    template <typename Word = std::string>
    typename Word::value_type human_readable_letter(size_t i) {
      // This check ensures that i is not too large to be converted to a
      // Word::value_type. This is check is only needed if the number of
      // distinct Word::value objects is less than the number of distinct size_t
      // objects.
      if constexpr (sizeof(typename Word::value_type) < sizeof(size_t)) {
        if (i > std::numeric_limits<typename Word::value_type>::max()
                    - std::numeric_limits<typename Word::value_type>::min()) {
          LIBSEMIGROUPS_EXCEPTION(
              "expected the argument to be in the range [0, {}), found {}",
              1 + std::numeric_limits<typename Word::value_type>::max()
                  - std::numeric_limits<typename Word::value_type>::min(),
              i);
        }
      }
      if constexpr (!std::is_same_v<Word, std::string>) {
        return static_cast<typename Word::value_type>(i);
      } else {
        // Choose visible characters a-zA-Z0-9 first before anything else
        // The ascii ranges for these characters are: [97, 123), [65, 91),
        // [48, 58) so the remaining range of chars that are appended to the end
        // after these chars are [0,48), [58, 65), [91, 97), [123, 255]
        return detail::chars_in_human_readable_order()[i];
      }
    }

    ////////////////////////////////////////////////////////////////////////
    // operator+
    ////////////////////////////////////////////////////////////////////////

    //! \anchor operator_plus
    //! \brief Concatenate two words.
    //!
    //! Returns the concatenation of \c u and \c w.
    //!
    //! \param u a word.
    //! \param w a word.
    //!
    //! \returns A \ref word_type.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    word_type operator+(word_type const& u, word_type const& w);

    //! \brief Concatenate a word and a letter.
    //!
    //! See \ref operator_plus "operator+".
    word_type operator+(word_type const& u, letter_type w);

    //! \brief Concatenate a letter and a word.
    //!
    //! See \ref operator_plus "operator+".
    word_type operator+(letter_type w, word_type const& u);

    ////////////////////////////////////////////////////////////////////////
    // operator+=
    ////////////////////////////////////////////////////////////////////////

    //! \anchor operator_plus_equals
    //! \brief Concatenate a word with another word in-place.
    //!
    //! Changes \c u to `u + v` in-place. See \ref operator_plus "operator+".
    //!
    //! \param u a word.
    //! \param v a word.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \sa \ref operator_plus "operator+"
    static inline void operator+=(word_type& u, word_type const& v) {
      u.insert(u.end(), v.cbegin(), v.cend());
    }

    //! \brief Concatenate a word and a letter in-place.
    //!
    //! See \ref operator_plus_equals "operator+=".
    inline void operator+=(word_type& u, letter_type a) {
      u.push_back(a);
    }

    //! \brief Concatenate a letter and a word in-place.
    //!
    //! See \ref operator_plus_equals "operator+=".
    inline void operator+=(letter_type a, word_type& u) {
      u.insert(u.begin(), a);
    }

    ////////////////////////////////////////////////////////////////////////
    // pow
    ////////////////////////////////////////////////////////////////////////

    //! \brief Power a word in-place.
    //!
    //! Modify the Word \c w to contains its `n`th power.
    //!
    //! \tparam Word the type of the first parameter.
    //!
    //! \param w the word.
    //! \param n the power.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    template <typename Word>
    void pow_inplace(Word& w, size_t n);
    // No pow_inplace for string_view or initializer_list because there's no
    // where to store the result.

    //! \brief Returns the power of a word.
    //!
    //! Returns the Word \c w to the power \p n.
    //!
    //! \tparam Word the type of the first parameter.
    //!
    //! \param x the word to power.
    //! \param n the power.
    //!
    //! \returns A Word.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    template <typename Word>
    Word pow(Word const& x, size_t n) {
      Word y(x);
      pow_inplace(y, n);
      return y;
    }

    //! \brief Returns the power of a word.
    //!
    //! See pow(Word const&, size_t) for details.
    word_type pow(std::initializer_list<letter_type> ilist, size_t n);

    //! \brief Returns the power of a string.
    //!
    //! See pow(Word const&, size_t) for details.
    std::string pow(std::string_view w, size_t n);

    ////////////////////////////////////////////////////////////////////////
    // prod
    ////////////////////////////////////////////////////////////////////////

    //! \anchor prod
    //! \brief Returns a product of letters or words.
    //!
    //! Let \p elts correspond to the ordered set \f$a_0, a_1, \ldots, a_{n -
    //! 1}\f$, \p first to \f$f\f$, \p last to \f$l\f$, and \p step to
    //! \f$s\f$. If \f$f \leq l\f$, let \f$k\f$ be the greatest positive
    //! integer such that \f$f + ks < l\f$. Then the function \c prod returns
    //! the word corresponding to \f$a_f a_{f + s} a_{f + 2s} \cdots a_{f +
    //! ks}\f$. All subscripts are taken modulo \f$n\f$.
    //!
    //! If there is no such \f$k\f$ (i.e. \f$s < 0\f$, or \f$f = l\f$), then
    //! the empty word is returned. Where \f$f > l\f$, the function works
    //! analogously, where \f$k\f$ is the greatest positive integer such that
    //! \f$f + k s > l\f$.
    //!
    //! \tparam Container the type of the 1st argument \c elts.
    //! \tparam Word the return type (defaults to Container).
    //!
    //! \param elts the ordered set.
    //! \param first the first index.
    //! \param last the last index.
    //! \param step the step.
    //!
    //! \return A Word.
    //!
    //! \throws LibsemigroupsException if `step = 0`
    //! \throws LibsemigroupsException if \p elts is empty, but the specified
    //! range is not
    //!
    //! \par Examples
    //! \code
    //! using namespace words;
    //! word_type w = 012345_w;
    //! prod(w, 0, 5, 2);              // {0, 2, 4}
    //! prod(w, 1, 9, 2);              // {1, 3, 5, 1}
    //! prod("abcde", 4, 1, -1);       // "edc"
    //! prod({"aba", "xyz"}, 0, 4, 1); // "abaxyzabaxyz"
    //! \endcode
    template <typename Container, typename Word = Container>
    Word prod(Container const& elts, int first, int last, int step = 1);

    //! \brief Returns a product of letters.
    //!
    //! See \ref prod(Container const&, int, int, int).
    static inline word_type prod(std::initializer_list<letter_type> ilist,
                                 int                                first,
                                 int                                last,
                                 int                                step = 1) {
      return prod<word_type>(word_type(ilist), first, last, step);
    }

    //! \brief Returns a product of letters.
    //!
    //! See \ref prod(Container const&, int, int, int).
    static inline std::string
    prod(std::string_view sv, int first, int last, int step = 1) {
      return prod<std::string>(std::string(sv), first, last, step);
    }

    //! \brief Returns a product of words.
    //!
    //! See \ref prod(Container const&, int, int, int).
    static inline word_type prod(std::initializer_list<word_type> const& elts,
                                 int                                     first,
                                 int                                     last,
                                 int step = 1) {
      return prod<std::vector<word_type>, word_type>(
          std::vector<word_type>(elts), first, last, step);
    }

    //! \brief Returns a product of strings.
    //!
    //! See \ref prod(Container const&, int, int, int).
    static inline std::string
    prod(std::initializer_list<std::string_view> const& sv,
         int                                            first,
         int                                            last,
         int                                            step = 1) {
      return prod<std::vector<std::string_view>, std::string>(
          sv, first, last, step);
    }

    //! \brief Returns a product of letters or words.
    //!
    //! Returns the same as `prod(elts, 0, last, 1)`.
    //!
    //! See \ref prod(Container const&, int, int, int).
    template <typename Container, typename Word = Container>
    Word prod(Container const& elts, size_t last) {
      return prod(elts, 0, static_cast<int>(last), 1);
    }

    //! \brief Returns a product of letters.
    //!
    //! Returns the same as `prod(elts, 0, last, 1)`.
    //!
    //! See \ref prod(Container const&, int, int, int).
    static inline word_type prod(std::initializer_list<letter_type> const& elts,
                                 size_t last) {
      return prod(elts, 0, static_cast<int>(last), 1);
    }

    //! \brief Returns a product of letters.
    //!
    //! Returns the same as `prod(elts, 0, last, 1)`.
    //!
    //! See \ref prod(Container const&, int, int, int).
    static inline std::string prod(std::string_view elts, size_t last) {
      return prod<std::string_view, std::string>(
          elts, 0, static_cast<int>(last), 1);
    }

    //! \brief Returns a product of words.
    //!
    //! Returns the same as `prod(elts, 0, last, 1)`.
    //!
    //! See \ref prod(Container const&, int, int, int).
    static inline word_type prod(std::initializer_list<word_type> const& elts,
                                 size_t                                  last) {
      return prod(elts, 0, static_cast<int>(last), 1);
    }

    //! \brief Returns a product of words.
    //!
    //! Returns the same as `prod(elts, 0, last, 1)`.
    //!
    //! See \ref prod(Container const&, int, int, int).
    static inline std::string
    prod(std::initializer_list<std::string_view> const& elts, size_t last) {
      return prod<std::vector<std::string_view>, std::string>(
          std::vector<std::string_view>(elts), 0, static_cast<int>(last), 1);
    }

  }  // namespace words
}  // namespace libsemigroups

#include "words-helpers.tpp"
#endif  // LIBSEMIGROUPS_WORDS_HELPERS_HPP_
