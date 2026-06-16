//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2026 James D. Mitchell
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

#ifndef LIBSEMIGROUPS_ALPHABET_CLASS_HPP_
#define LIBSEMIGROUPS_ALPHABET_CLASS_HPP_

#include <cctype>         // for isprint
#include <cstddef>        // for size_t
#include <limits>         // for numeric_limits
#include <numeric>        // for iota
#include <string>         // for basic_string
#include <type_traits>    // for is_same_v, decay_t
#include <unordered_map>  // for unordered_map
#include <utility>        // for move
#include <vector>         // for vector

#include "debug.hpp"       // for LIBSEMIGROUPS_ASSERT
#include "exception.hpp"   // for LIBSEMIGROUPS_EXCEPTION, Libs...
#include "word-range.hpp"  // for human_readable_letter

#include "detail/fmt.hpp"    // for format
#include "detail/print.hpp"  // for to_printable, isprint

namespace libsemigroups {
  //! \brief Class for storing and indexing an alphabet.
  //!
  //! An Alphabet object stores an ordered list of distinct letters, represented
  //! by a word type \p Word, and maintains a map from each letter to its
  //! position in that list. The order of the letters is significant: it is the
  //! order used by \ref letter, \ref letter_no_checks, \ref Alphabet::index,
  //! and \ref index_no_checks.
  //!
  //! \tparam Word the type used to store the letters of the alphabet. The
  //! value type of \p Word is the letter type.
  template <typename Word>
  class Alphabet {
   public:
    //! \brief Type used to store the letters of an Alphabet object.
    using native_word_type = Word;

    //! \brief Type of the letters in the alphabet.
    using native_letter_type = typename native_word_type::value_type;

    //! \brief Type used for sizes and indices.
    using size_type = typename native_word_type::size_type;

   private:
    native_word_type                                  _letters;
    std::unordered_map<native_letter_type, size_type> _letters_map;

   public:
    ////////////////////////////////////////////////////////////////////////
    // Standard constructors + initializers
    ////////////////////////////////////////////////////////////////////////

    //! \brief Default constructor.
    //!
    //! Constructs an empty alphabet.
    Alphabet();

    //! \brief Remove all letters in the alphabet.
    //!
    //! This function clears the alphabet, putting it back into the state it
    //! would be in if it was newly constructed.
    //!
    //! \returns A reference to \c *this.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    Alphabet& init();

    //! \brief Default copy constructor.
    //!
    //! Default copy constructor.
    Alphabet(Alphabet const&);

    //! \brief Default move constructor.
    //!
    //! Default move constructor.
    Alphabet(Alphabet&&);

    //! \brief Default copy assignment operator.
    //!
    //! Default copy assignment operator.
    //!
    //! \returns A reference to \c *this.
    Alphabet& operator=(Alphabet const&);

    //! \brief Default move assignment operator.
    //!
    //! Default move assignment operator.
    //!
    //! \returns A reference to \c *this.
    Alphabet& operator=(Alphabet&&);

    //! \brief Default destructor.
    //!
    //! Default destructor.
    ~Alphabet();

    ////////////////////////////////////////////////////////////////////////
    // Alphabet specific constructors + initializers
    ////////////////////////////////////////////////////////////////////////

    //! \brief Construct the alphabet by size.
    //!
    //! Constructs an alphabet containing the first \p n human-readable letters
    //! for \p Word, as returned by `words::human_readable_letter<Word>`.
    //!
    //! \param n the size of the alphabet.
    //!
    //! \throws LibsemigroupsException if the value of \p n is greater than the
    //! maximum number of letters supported by \ref native_letter_type.
    explicit Alphabet(size_type n);

    //! \brief Re-initialize the alphabet by size.
    //!
    //! Replaces the alphabet by the first \p n human-readable letters for
    //! \p Word, as returned by `words::human_readable_letter<Word>`.
    //!
    //! \param n the size of the alphabet.
    //!
    //! \returns A reference to \c *this.
    //!
    //! \throws LibsemigroupsException if the value of \p n is greater than the
    //! maximum number of letters supported by \ref native_letter_type.
    Alphabet& init(size_type n);

    //! \brief Construct the alphabet from a const reference.
    //!
    //! Sets the alphabet to be the letters in \p lphbt.
    //!
    //! \param lphbt the alphabet.
    //!
    //! \throws LibsemigroupsException if there are duplicate letters in
    //! \p lphbt.
    //!
    explicit Alphabet(native_word_type const& lphbt);

    //! \brief Re-initialize the alphabet from a const reference.
    //!
    //! Sets the alphabet to be the letters in \p lphbt.
    //!
    //! \param lphbt the alphabet.
    //!
    //! \returns A reference to \c *this.
    //!
    //! \throws LibsemigroupsException if there are duplicate letters in
    //! \p lphbt.
    //!
    Alphabet& init(native_word_type const& lphbt);

    //! \brief Construct the alphabet from an rvalue reference.
    //!
    //! Constructs the alphabet by moving the letters in \p lphbt.
    //!
    //! \param lphbt the alphabet.
    //!
    //! \throws LibsemigroupsException if there are duplicate letters in
    //! \p lphbt.
    //!
    explicit Alphabet(native_word_type&& lphbt);

    //! \brief Re-initialize the alphabet from an rvalue reference.
    //!
    //! Replaces the alphabet by moving the letters in \p lphbt.
    //!
    //! \param lphbt the alphabet.
    //!
    //! \returns A reference to \c *this.
    //!
    //! \throws LibsemigroupsException if there are duplicate letters in
    //! \p lphbt.
    //!
    Alphabet& init(native_word_type&& lphbt);

    ////////////////////////////////////////////////////////////////////////
    // Validation
    ////////////////////////////////////////////////////////////////////////

    //! \brief Check if a letter belongs to the alphabet or not.
    //!
    //! Checks whether \p c belongs to the alphabet.
    //!
    //! \param c the letter to check.
    //!
    //! \throws LibsemigroupsException if \p c does not belong to the alphabet.
    //!
    //! \complexity
    //! Constant on average, worst case linear in the size of the alphabet.
    void throw_if_letter_not_in_alphabet(native_letter_type c) const;

    //! \brief Check if every letter in a range belongs to the alphabet.
    //!
    //! Checks whether every letter in the range `[first, last)` belongs to the
    //! alphabet.
    //!
    //! \tparam Iterator1 the type of \p first.
    //! \tparam Iterator2 the type of \p last.
    //!
    //! \param first iterator pointing at the first letter to check.
    //! \param last iterator pointing one beyond the last letter to check.
    //!
    //! \throws LibsemigroupsException if any letter in `[first, last)` does
    //! not belong to the alphabet.
    //!
    //! \complexity
    //! Linear in the length of the range on average, worst case proportional
    //! to the product of the length of the range and the size of the alphabet.
    template <typename Iterator1, typename Iterator2>
    void throw_if_letter_not_in_alphabet(Iterator1 first, Iterator2 last) const;

    //! \brief Check if the alphabet contains duplicates.
    //!
    //! Check if the alphabet is duplicate-free.
    //!
    //! \throws LibsemigroupsException if there are duplicate letters in the
    //! alphabet.
    //!
    //! \complexity
    //! Linear in the length of the alphabet.
    void throw_if_duplicate_letters() const {
      decltype(_letters_map) letters_map;
      throw_if_duplicate_letters(letters_map);
    }

    ////////////////////////////////////////////////////////////////////////
    // Container type attributes
    ////////////////////////////////////////////////////////////////////////

    //! \brief Return the size of the alphabet.
    //!
    //! Returns the number of letters in the alphabet.
    //!
    //! \returns The number of letters in the alphabet.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    [[nodiscard]] size_type size() const noexcept {
      return _letters.size();
    }

    //! \brief Check if the alphabet is empty.
    //!
    //! Returns \c true if the alphabet contains no letters, and \c false
    //! otherwise.
    //!
    //! \returns Whether or not the alphabet is empty.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    [[nodiscard]] bool empty() const noexcept {
      return _letters.empty();
    }

    // TODO begin, end?

    ////////////////////////////////////////////////////////////////////////
    // Alphabet attributes
    ////////////////////////////////////////////////////////////////////////

    //! \brief Return the letters of the alphabet.
    //!
    //! Returns the letters of the alphabet.
    //!
    //! \returns A const reference to \c Alphabet::native_word_type.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    [[nodiscard]] native_word_type const& letters() const noexcept {
      return _letters;
    }

    //! \brief Return a letter in the alphabet by index.
    //!
    //! Returns the letter of the alphabet in position \p i.
    //!
    //! \param i the index.
    //!
    //! \returns A value of type \ref native_letter_type.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \warning
    //! This function performs no bound checks on the argument \p i.
    [[nodiscard]] native_letter_type letter_no_checks(size_type i) const {
      LIBSEMIGROUPS_ASSERT(i < size());
      return _letters[i];
    }

    //! \brief Return a letter in the alphabet by index.
    //!
    //! After checking that \p i is in the range \f$[0, n)\f$, where \f$n\f$ is
    //! the length of the alphabet, this function returns the same value as
    //! `letter_no_checks(size_type i) const`.
    //!
    //! \throws LibsemigroupsException if \p i is not in the range \f$[0, n)\f$.
    //!
    //! \sa
    //! * \ref letter_no_checks
    [[nodiscard]] native_letter_type letter(size_type i) const;

    //! \brief Return the index of a letter in the alphabet.
    //!
    //! Get the index of a letter in the alphabet.
    //!
    //! \param val the letter.
    //!
    //! \returns A value of type \ref size_type.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Constant.
    //!
    //! \warning This function does not verify that its argument belongs to the
    //! alphabet.
    [[nodiscard]] size_type index_no_checks(native_letter_type val) const {
      return _letters_map.find(val)->second;
    }

    //! \brief Return the index of a letter in the alphabet.
    //!
    //! After checking that \p val is in the alphabet, this function returns
    //! the same value as `index_no_checks(native_letter_type val) const`.
    //!
    //! \throws LibsemigroupsException if \p val does not belong to the
    //! alphabet.
    //!
    //! \sa
    //! * \ref index_no_checks
    [[nodiscard]] size_type index(native_letter_type val) const {
      throw_if_letter_not_in_alphabet(val);
      return _letters_map.find(val)->second;
    }

    //! \brief Check if a letter belongs to the alphabet or not.
    //!
    //! Checks whether \p val belongs to the alphabet.
    //!
    //! \param val the letter to check.
    //!
    //! \returns A value of type \c bool.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Constant on average, worst case linear in the size of the alphabet.
    [[nodiscard]] bool contains(native_letter_type val) const {
      return _letters_map.find(val) != _letters_map.cend();
    }

    ////////////////////////////////////////////////////////////////////////
    // Alphabet modifiers
    ////////////////////////////////////////////////////////////////////////

    //! \brief Add a letter.
    //!
    //! This function adds the letter \p x to the alphabet.
    //!
    //! \param x the letter to add.
    //!
    //! \returns A reference to \c *this.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \warning This function does no checks on its arguments whatsoever. In
    //! particular, if the letter \p x is already in the alphabet, then it will
    //! be added again, and the alphabet may no longer be in a valid state.
    Alphabet& add_letter_no_checks(native_letter_type x);

    //! \brief Add a letter.
    //!
    //! This function adds the letter \p x to the alphabet.
    //!
    //! \param x the letter to add.
    //!
    //! \returns A reference to \c *this.
    //!
    //! \throws LibsemigroupsException if \p x is in `letters()`.
    Alphabet& add_letter(native_letter_type x);

    //! \brief Remove a letter.
    //!
    //! This function removes the letter \p x from the alphabet.
    //!
    //! \param x the letter to remove.
    //!
    //! \returns A reference to \c *this.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Average case: linear in the length of the alphabet, worst case:
    //! quadratic in the length of the alphabet.
    //!
    //! \warning This function does no checks on its arguments whatsoever. In
    //! particular, if the letter \p x does not belong to the alphabet, then bad
    //! things will happen.
    Alphabet& remove_letter_no_checks(native_letter_type x);

    //! \brief Remove a letter.
    //!
    //! This function removes the letter \p x from the alphabet.
    //!
    //! \param x the letter to remove.
    //!
    //! \returns A reference to \c *this.
    //!
    //! \throws LibsemigroupsException if \p x is not in `letters()`.
    //!
    //! \complexity
    //! Average case: linear in the length of the alphabet, worst case:
    //! quadratic in the length of the alphabet.
    Alphabet& remove_letter(native_letter_type x);

   private:
    void try_set_letters(decltype(_letters_map)& letters_map,
                         native_word_type&       old_letters);
    void throw_if_duplicate_letters(decltype(_letters_map)& letters_map) const;
  };  // class Alphabet

  ////////////////////////////////////////////////////////////////////////
  // Validation
  ////////////////////////////////////////////////////////////////////////

  //! \relates Alphabet
  //!
  //! \brief Check if an alphabet is valid.
  //!
  //! Checks whether \p alphabet is valid.
  //!
  //! \tparam Word the type of the words in the alphabet.
  //!
  //! \param alphabet the alphabet to validate.
  //!
  //! \throws LibsemigroupsException if \p alphabet contains duplicate letters.
  //!
  //! \complexity
  //! Linear in the size of \p alphabet.
  template <typename Word>
  void validate(Alphabet<Word> const& alphabet) {
    alphabet.throw_if_duplicate_letters();
  }

  ////////////////////////////////////////////////////////////////////////
  // Comparisons
  ////////////////////////////////////////////////////////////////////////

  //! \ingroup presentations_group
  //! \relates Alphabet
  //!
  //! \brief Compare alphabets for equality.
  //!
  //! This function returns \c true if and only if the letters in \p lhop and
  //! \p rhop are equal and occur in the same order.
  //!
  //! \tparam Word the type of the words in the presentations.
  //! \param lhop the first alphabet for comparison.
  //! \param rhop the second alphabet for comparison.
  //!
  //! \returns Whether or not the alphabets are equal.
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  //!
  //! \complexity
  //! At worst linear in the size of the alphabets.
  template <typename Word>
  [[nodiscard]] bool operator==(Alphabet<Word> const& lhop,
                                Alphabet<Word> const& rhop) {
    return lhop.letters() == rhop.letters();
  }

  //! \relates Alphabet
  //!
  //! \brief Compare alphabets for equality.
  //!
  //! This function returns \c true if and only if the letters in \p lhop and
  //! \p rhop are not equal or occur in a different order.
  //!
  //! \tparam Word the type of the words in the presentations.
  //! \param lhop the first alphabet for comparison.
  //! \param rhop the second alphabet for comparison.
  //!
  //! \returns Whether or not the alphabets are equal.
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  //!
  //! \complexity
  //! At worst linear in the size of the alphabets.
  template <typename Word>
  [[nodiscard]] bool operator!=(Alphabet<Word> const& lhop,
                                Alphabet<Word> const& rhop) {
    return lhop.letters() != rhop.letters();
  }

  // TODO more? or delegate to the Cmp structs?

  ////////////////////////////////////////////////////////////////////////
  // String representations
  ////////////////////////////////////////////////////////////////////////

  //! \relates Alphabet
  //!
  //! \brief Return a human readable representation of an alphabet.
  //!
  //! This function returns a human readable representation of an alphabet.
  //!
  //! \tparam Word the type of the words in the alphabet.
  //! \param alphabet the alphabet.
  //!
  //! \returns A string containing the representation.
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  template <typename Word>
  [[nodiscard]] std::string
  to_human_readable_repr(Alphabet<Word> const& alphabet);

  //! \relates Alphabet
  //!
  //! \brief Return a string that can be used to recreate an alphabet.
  //!
  //! This function returns a std::string containing the input required to
  //! construct a copy of the argument \p alphabet.
  //!
  //! \tparam Word the type of the words in the alphabet.
  //! \param alphabet the alphabet.
  //!
  //! \returns A string containing the input required to recreate the alphabet.
  // TODO doc exception
  template <typename Word>
  [[nodiscard]] std::string to_input_string(Alphabet<Word> const& alphabet,
                                            std::string const& braces = "{}");

  //! \relates Alphabet
  //!
  //! \brief Return a string that can be used to recreate a string alphabet.
  //!
  //! This function returns a std::string containing the input required to
  //! construct a copy of the argument \p alphabet. If every letter in
  //! \p alphabet is printable, then the letters are represented as a string
  //! literal; otherwise, they are represented by their character values.
  //!
  //! \param alphabet the alphabet.
  //!
  //! \returns A string containing the input required to recreate the alphabet.
  // TODO doc exception
  [[nodiscard]] std::string
  to_input_string(Alphabet<std::string> const& alphabet,
                  std::string const&           braces = "{}");

  //! \relates Alphabet
  //!
  //! \brief Insert the output of \ref to_input_string(Alphabet<Word> const&)
  //! into a std::ostream.
  //!
  //! This function inserts the output of
  //! \ref to_input_string(Alphabet<Word> const&) into a std::ostream.
  //!
  //! \tparam Word the type of the words in the alphabet.
  //! \param os the std::ostream.
  //! \param alphabet the alphabet.
  //!
  //! \returns A reference to the first parameter \p os.
  template <typename Word>
  std::ostream& operator<<(std::ostream& os, Alphabet<Word> const& alphabet) {
    os << to_input_string(alphabet);
    return os;
  }

  ////////////////////////////////////////////////////////////////////////
  // Alphabet + function -> Alphabet
  ////////////////////////////////////////////////////////////////////////

  //! \defgroup to_alphabet_group to<Alphabet>
  //!
  //! This file contains documentation for creating alphabets in
  //! `libsemigroups` from other types of alphabet.
  //!
  //! \sa \ref to_group for an overview of possible conversions between
  //! `libsemigroups` types.

  //! \ingroup to_alphabet_group
  //!
  //! \brief Transform an alphabet to another alphabet.
  //!
  //! Constructs an alphabet of type \p Result by applying \p f to every letter
  //! in \p alphabet, preserving the order of the letters.
  //!
  //! \tparam Result the return type. Must be an `Alphabet<WordOutput>` for
  //! some word type `WordOutput`.
  //! \tparam Word the type of the words in the input alphabet.
  //! \tparam Func the type of the letter transformation function.
  //!
  //! \param alphabet the alphabet to transform.
  //! \param f the function to apply to each letter.
  //!
  //! \returns An alphabet of type \p Result.
  //!
  //! \throws LibsemigroupsException if \p alphabet contains duplicate letters
  //! or if the transformed alphabet contains duplicate letters.
  template <typename Result, typename Word, typename Func>
  auto to(Alphabet<Word> const& alphabet, Func&& f) -> std::enable_if_t<
      std::is_same_v<Alphabet<typename Result::native_word_type>, Result>,
      Result>;

  ////////////////////////////////////////////////////////////////////////
  // Alphabet -> Alphabet
  ////////////////////////////////////////////////////////////////////////

  //! \ingroup to_alphabet_group
  //!
  //! \brief Convert an alphabet to another alphabet type.
  //!
  //! Constructs an alphabet of type \p Result by replacing each letter in
  //! \p alphabet by the human-readable letter with the same index in the
  //! output word type.
  //!
  //! \tparam Result the return type. Must be an `Alphabet<WordOutput>` for
  //! some word type `WordOutput`, where `WordOutput` is not \p Word.
  //! \tparam Word the type of the words in the input alphabet.
  //!
  //! \param alphabet the alphabet to convert.
  //!
  //! \returns An alphabet of type \p Result.
  //!
  //! \throws LibsemigroupsException if \p alphabet contains duplicate letters
  //! or if the converted alphabet contains duplicate letters.
  template <typename Result, typename Word>
  auto to(Alphabet<Word> const& alphabet) -> std::enable_if_t<
      std::is_same_v<Alphabet<typename Result::native_word_type>, Result>
          && !std::is_same_v<typename Result::native_word_type, Word>,
      Result> {
    using WordOutput = typename Result::native_word_type;
    return to<Result>(alphabet, [&alphabet](auto val) {
      return words::human_readable_letter<WordOutput>(alphabet.index(val));
    });
  }

  //! \ingroup to_alphabet_group
  //!
  //! \brief Return an alphabet as its own type.
  //!
  //! Returns a const reference to \p alphabet when \p Result is the same
  //! alphabet type.
  //!
  //! \tparam Result the return type. Must be `Alphabet<Word>`.
  //! \tparam Word the type of the words in the alphabet.
  //!
  //! \param alphabet the alphabet.
  //!
  //! \returns A const reference to \p alphabet.
  //!
  //! \exceptions
  //! \noexcept
  template <typename Result, typename Word>
  auto to(Alphabet<Word> const& alphabet) noexcept
      -> std::enable_if_t<std::is_same_v<Alphabet<Word>, Result>,
                          Result const&> {
    return alphabet;
  }

}  // namespace libsemigroups

#include "alphabet-class.tpp"

#endif  // LIBSEMIGROUPS_ALPHABET_CLASS_HPP_
