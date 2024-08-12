//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2020-2023 James D. Mitchell
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

// This file contains declarations for function related to words (counting, and
// converting) in libsemigroups.

// TODO:
// * iwyu
// * nodiscard
// * tests for code coverage

#ifndef LIBSEMIGROUPS_WORDS_HPP_
#define LIBSEMIGROUPS_WORDS_HPP_

#ifndef PARSED_BY_DOXYGEN
#define NOT_PARSED_BY_DOXYGEN
#endif

#include <array>             // for array
#include <cstddef>           // for size_t
#include <cstdint>           // for uint64_t, uint8_t
#include <initializer_list>  // for initializer_list
#include <iterator>          // for distance
#include <string>            // for basic_string
#include <utility>           // for forward
#include <variant>           // for visit, operator==
#include <vector>            // for vector, operator==

#include "constants.hpp"  // for UNDEFINED
#include "exception.hpp"  // for LibsemigroupsException
#include "types.hpp"      // for word_type

#include "detail/iterator.hpp"        // for default_postfix_increment
#include "detail/word-iterators.hpp"  // for const_wilo_iterator

#include "ranges.hpp"  // for begin, end

namespace libsemigroups {

  enum class Order : uint8_t;  // forward decl

  ////////////////////////////////////////////////////////////////////////
  // WordRange
  ////////////////////////////////////////////////////////////////////////

  //! \defgroup word_range_group WordRange
  //! This page contains details of the functionality in \c libsemigroups
  //! related to generating words in a given range and in a given order.
  //!
  //! This file contains documentation for functionality for:
  //!
  //! * generating words and strings in a given range and in a certain order:
  //!  - \ref WordRange
  //!  - \ref Strings
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
  //!   - \ref ToStrings
  //!   - \ref to_string(std::string_view,Iterator,Iterator,std::string&)
  //!   "to_string"
  //!

  // TODO(later)
  // * parsing algebraic expressions in a string;

  //! \ingroup word_range_group
  //! \brief Returns the number of words over an alphabet with a given number of
  //! letters with length in a specified range.
  //!
  //! Returns the number of words over an alphabet with a given number of
  //! letters with length in a specified range.
  //!
  //! \param n the number of letters in the alphabet.
  //! \param min the minimum length of a word.
  //! \param max the maximum length of a word.
  //!
  //! \returns
  //! A value of type \c uint64_t.
  //!
  //! \exception
  //! \no_libsemigroups_except
  //!
  //! \warning If the number of words exceeds 2 ^ 64 - 1, then the return value
  //! of this function will not be correct.
  [[nodiscard]] uint64_t number_of_words(size_t n, size_t min, size_t max);

  //! \ingroup word_range_group
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

  //! \ingroup word_range_group
  //! \brief Returns a forward iterator pointing to the 3rd parameter \p first.
  //!
  //! Returns a forward iterator used to iterate over words in lexicographic
  //! order (wilo). If incremented, the iterator will point to the next least
  //! lexicographic word after \p first over an \p n letter alphabet with length
  //! less than \p upper_bound.  Iterators of the type returned by this function
  //! are equal whenever they are obtained by advancing the return value of any
  //! call to \c cbegin_wilo by the same amount, or they are both obtained by
  //! any call to \c cend_wilo.
  //!
  //! \param n the number of letters in the alphabet.
  //! \param upper_bound   only words of length less than this value are
  //! considered.
  //! \param first the starting point for the iteration.
  //! \param last the value one past the end of the last value in the
  //! iteration.
  //!
  //! \returns An iterator pointing to \p first.
  //!
  //! \exception
  //! \no_libsemigroups_except
  //!
  //! \note
  //! The parameter \p upper_bound is required because lexicographical
  //! ordering is not a well-ordering, and there might be infinitely many words
  //! between a given pair of words.
  //!
  //! \warning
  //! Copying iterators of this type is expensive.  As a consequence, prefix
  //! incrementing \c ++it the iterator \c it returned by \c cbegin_wilo is
  //! significantly cheaper than postfix incrementing \c it++.
  //!
  //! \warning
  //! Iterators constructed using different parameters may not be equal, so
  //! best not to loop over them.
  //!
  //! \sa cend_wilo
  //!
  //! \par Example
  //! \code
  //! std::vector<word_type>(cbegin_wilo(2, 3, {0}, {1, 1, 1}),
  //!                        cend_wilo(2, 3, {0}, {1, 1, 1}));
  //! // {{0}, {0, 0}, {0, 1}, {1}, {1, 0}, {1, 1}};
  //! \endcode
  [[nodiscard]] detail::const_wilo_iterator cbegin_wilo(size_t      n,
                                                        size_t      upper_bound,
                                                        word_type&& first,
                                                        word_type&& last);

  //! \ingroup word_range_group
  //! \brief Returns a forward iterator pointing to the 3rd parameter \p first.
  //! \copydoc cbegin_wilo(size_t, size_t, word_type&&, word_type&&)
  [[nodiscard]] detail::const_wilo_iterator cbegin_wilo(size_t n,
                                                        size_t upper_bound,
                                                        word_type const& first,
                                                        word_type const& last);

  //! \ingroup word_range_group
  //! \brief Returns a forward iterator pointing to one after the end of the
  //! range from \p first to \p last.
  //!
  //! The iterator returned by this function is still dereferenceable and
  //! incrementable, but does not point to a word in the correct range.
  //!
  //! \sa cbegin_wilo
  [[nodiscard]] detail::const_wilo_iterator
  cend_wilo(size_t n, size_t upper_bound, word_type&& first, word_type&& last);

  //! \ingroup word_range_group
  //! \brief Returns a forward iterator pointing to one after the end of the
  //! range from \p first to \p last.
  //! \copydoc cend_wilo(size_t, size_t, word_type&&, word_type&&)
  [[nodiscard]] detail::const_wilo_iterator cend_wilo(size_t n,
                                                      size_t upper_bound,
                                                      word_type const& first,
                                                      word_type const& last);

  //! \ingroup word_range_group
  //! \brief Returns a forward iterator pointing to the 2nd parameter \p first.
  //!
  //! Returns a forward iterator used to iterate over words in
  //! short-lexicographic order (wislo). If incremented, the iterator will point
  //! to the next least short-lex word after \p w over an \p n letter alphabet.
  //! Iterators of the type returned by this function are equal whenever they
  //! are obtained by advancing the return value of any call to \c cbegin_wislo
  //! by the same amount, or they are both obtained by any call to \c
  //! cend_wislo.
  //!
  //! \param n the number of letters in the alphabet.
  //! \param first the starting point for the iteration.
  //! \param last the ending point for the iteration.
  //!
  //! \returns An iterator pointing to \p first.
  //!
  //! \exception
  //! \no_libsemigroups_except
  //!
  //! \warning
  //! Copying iterators of this type is expensive.  As a consequence, prefix
  //! incrementing \c ++it the iterator \c it returned by \c cbegin_wislo is
  //! significantly cheaper than postfix incrementing \c it++.
  //!
  //! \warning
  //! Iterators constructed using different parameters may not be equal, so
  //! best not to loop over them.
  //!
  //! \sa cend_wislo
  //!
  //! \par Example
  //! \code
  //! std::vector<word_type>(cbegin_wislo(2, {0}, {0, 0, 0}),
  //!                        cend_wislo(2,  {0}, {0, 0, 0}));
  //! // {{0}, {1}, {0, 0}, {0, 1}, {1, 0}, {1, 1}};
  //! \endcode
  [[nodiscard]] detail::const_wislo_iterator cbegin_wislo(size_t      n,
                                                          word_type&& first,
                                                          word_type&& last);

  //! \ingroup word_range_group
  //! \brief Returns a forward iterator pointing to the 2nd parameter \p first.
  //! \copydoc cbegin_wislo(size_t const, word_type&&, word_type&&)
  [[nodiscard]] detail::const_wislo_iterator
  cbegin_wislo(size_t n, word_type const& first, word_type const& last);

  //! \ingroup word_range_group
  //! \brief Returns a forward iterator pointing to one after the end of the
  //! range from \p first to \p last.
  //!
  //! The iterator returned by this is still dereferenceable and incrementable,
  //! but does not point to a word in the correct range.
  //!
  //! \sa cbegin_wislo
  [[nodiscard]] detail::const_wislo_iterator cend_wislo(size_t      n,
                                                        word_type&& first,
                                                        word_type&& last);

  //! \ingroup word_range_group
  //! \brief Returns a forward iterator pointing to one after the end of the
  //! range from \p first to \p last.
  //! \copydoc cend_wislo(size_t const, word_type&&, word_type&&)
  [[nodiscard]] detail::const_wislo_iterator cend_wislo(size_t           n,
                                                        word_type const& first,
                                                        word_type const& last);

  //! \ingroup word_range_group
  //! \brief Class for generating words in a given range and in a particular
  //! order.
  //!
  //! Defined in `words.hpp`.
  //!
  //! This class implements a range object for the lower level
  //! functions \ref cbegin_wislo and \ref cbegin_wilo. The purpose of this
  //! class is to provide a more user-friendly interface with \ref cbegin_wislo
  //! and \ref cbegin_wilo.
  //!
  //! \note
  //! There is a small overhead to using a WordRange object rather than using
  //! \ref cbegin_wislo or \ref cbegin_wilo directly.
  //!
  //! The order and range of the words in a WordRange instance can be set using
  //! the member functions:
  //! * \ref order
  //! * \ref number_of_letters
  //! * \ref min
  //! * \ref max
  //! * \ref first
  //! * \ref last
  //!
  //! \par Example
  //! \code
  //! WordRange words;
  //! words.order(Order::shortlex) // words in shortlex order
  //!      .number_of_letters(2)   // on 2 letters
  //!      .min(1)                 // of length in the range from 1
  //!      .max(5);                // to 5
  //! \endcode
  class WordRange {
   public:
    //! Alias for the size type.
    using size_type = typename std::vector<word_type>::size_type;

    //! The type of the output of a WordRange object.
    using output_type = word_type const&;

   private:
    using const_iterator = std::variant<detail::const_wilo_iterator,
                                        detail::const_wislo_iterator>;

    size_type              _number_of_letters;
    mutable const_iterator _current;
    mutable const_iterator _end;
    mutable bool           _current_valid;
    word_type              _first;
    word_type              _last;
    Order                  _order;
    size_type              _upper_bound;

    void set_iterator() const;

   public:
    //! \brief Get the current value.
    //!
    //! Returns the current word in a WordRange object.
    //!
    //! \returns A value of type \ref output_type.
    //!
    //! \exception
    //! \noexcept
    //!
    //! \warning If at_end() returns \c true, then the return value of this
    //! function could be anything.
    [[nodiscard]] output_type get() const noexcept {
      set_iterator();
      return std::visit(
          [](auto& it) -> auto const& { return *it; }, _current);
    }

    //! \brief Advance to the next value.
    //!
    //! Advances a WordRange object to the next value (if any).
    //!
    //! \exception
    //! \noexcept
    //!
    //! \sa \ref at_end
    void next() noexcept {
      set_iterator();
      std::visit([](auto& it) { ++it; }, _current);
    }

    //! \brief Check if the range object is exhausted.
    //!
    //! Returns \c true if a WordRange object is exhausted, and \c false if not.
    //! \returns A value of type \c bool.
    //!
    //! \exception
    //! \noexcept
    [[nodiscard]] bool at_end() const noexcept {
      set_iterator();
      return _current == _end;
    }

    //! \brief The possible size of the range.
    //!
    //! Returns the number of words in a WordRange object if order() is
    //! Order::shortlex. If order() is not Order::shortlex, then the return
    //! value of this function is meaningless.
    //!
    //! \returns A value of type \c size_t.
    //!
    //! \exception
    //! \noexcept
    [[nodiscard]] size_t size_hint() const noexcept {
      return number_of_words(_number_of_letters, _first.size(), _last.size());
      // This is only the actual size if _order is shortlex
    }

    //! \brief The actual size of the range.
    //!
    //! Returns the number of words in a WordRange object. If order() is
    //! Order::shortlex, then size_hint() is used. If order() is not
    //! Order::shortlex, then a copy of the range may have to be looped over in
    //! order to find the return value of this function.
    //!
    //! \returns A value of type \c size_t.
    //!
    //! \exception
    //! \noexcept
    [[nodiscard]] size_t count() const noexcept;

    // For some reason, there needs to be two doxygen comment lines here for
    // this to render.
    //! Value indicating that the range is finite.
    //!
    static constexpr bool is_finite = true;  // This may not always be true

    //! Value indicating that if get() is called twice on a WordRange object
    //! that is not changed between the two calls, then the return value of
    //! get() is the same both times.
    static constexpr bool is_idempotent = true;

    //! \brief Default constructor.
    //!
    //! Constructs an empty range with:
    //! * min() equal to \c 0;
    //! * max() equal to \c 0;
    //! * order() equal to Order::shortlex;
    //! * first() equal to the empty word;
    //! * last() equal to the empty word;
    //! * upper_bound() equal to \c 0;
    //! * letters() equal to \c 0.
    WordRange() {
      init();
    }

    //! \brief Initialize an existing WordRange object.
    //!
    //! This function puts a WordRange object back into the same state as if it
    //! had been newly default constructed.
    //!
    //! \returns A reference to \c *this.
    //!
    //! \exception
    //! \no_libsemigroups_except
    WordRange& init();

    //! \brief Default copy constructor.
    //!
    //! Default copy constructor.
    WordRange(WordRange const&);

    //! \brief Default move constructor.
    //!
    //! Default move constructor.
    WordRange(WordRange&&);

    //! \brief Default copy assignment operator.
    //!
    //! Default copy assignment operator.
    WordRange& operator=(WordRange const&);

    //! \brief Default move assignment operator.
    //!
    //! Default move assignment operator.
    WordRange& operator=(WordRange&&);

    //! \brief Default destructor.
    //!
    //! Default destructor.
    ~WordRange();

    //! \brief Set the number of letters in the alphabet.
    //!
    //! Sets the number of letters in a WordRange object to \p n.
    //!
    //! \param n the number of letters.
    //!
    //! \returns A reference to \c *this.
    //!
    //! \exception
    //! \noexcept
    WordRange& number_of_letters(size_type n) noexcept {
      _current_valid &= (n == _number_of_letters);
      _number_of_letters = n;
      return *this;
    }

    //! \brief The current number of letters in the alphabet.
    //!
    //! Returns the current number of letters in a WordRange object.
    //!
    //! \returns A value of type \ref size_type.
    //!
    //! \exception
    //! \noexcept
    [[nodiscard]] size_type number_of_letters() const noexcept {
      return _number_of_letters;
    }

    //! \brief Set the first word in the range.
    //!
    //! Sets the first word in a WordRange object to be \p frst. This function
    //! performs no checks on its arguments. If \p frst contains letters greater
    //! than letters(), then the WordRange object will be empty. Similarly, if
    //! first() is greater than last() with respect to order(), then the object
    //! will be empty.
    //!
    //! \param frst the first word.
    //!
    //! \returns A reference to \c *this.
    //!
    //! \sa \ref min
    WordRange& first(word_type const& frst) {
      _current_valid &= (frst == _first);
      _first = frst;
      return *this;
    }

    //! \brief The current first word in the range.
    //!
    //! Returns the first word in a WordRange object.
    //!
    //! \returns A const reference to a \ref word_type.
    //!
    //! \exception
    //! \noexcept
    //!
    //! \sa \ref min
    [[nodiscard]] word_type const& first() const noexcept {
      return _first;
    }

    //! \brief Set one past the last word in the range.
    //!
    //! Sets one past the last word in a WordRange object to be \p lst. This
    //! function performs no checks on its arguments. If \p lst contains
    //! letters greater than letters(), then the WordRange object will be empty.
    //!
    //! \param lst the first word.
    //!
    //! \returns A reference to \c *this.
    //!
    //! \sa \ref max
    WordRange& last(word_type const& lst) {
      _current_valid &= (lst == _last);
      _last = lst;
      return *this;
    }

    //! \brief The current one past the last word in the range.
    //!
    //! Returns the last word in a WordRange object.
    //!
    //! \returns A const reference to a \ref word_type.
    //!
    //! \exception
    //! \noexcept
    //!
    //! \sa \ref max
    [[nodiscard]] word_type const& last() const noexcept {
      return _last;
    }

    //! \brief Set the order of the words in the range.
    //!
    //! Sets the order of the words in a WordRange object to \p val.
    //!
    //! \param val the order.
    //!
    //! \returns A reference to \c *this.
    //!
    //! \throws LibsemigroupsException if val is not Order::shortlex or
    //! Order::lex.
    WordRange& order(Order val);

    //! \brief The current order of the words in the range.
    //!
    //! Returns the current order of the words in a WordRange object.
    //!
    //! \returns A value of type \ref Order.
    //!
    //! \exception
    //! \noexcept
    [[nodiscard]] Order order() const noexcept {
      return _order;
    }

    //! \brief Set an upper bound for the length of a word in the range.
    //!
    //! Sets an upper bound for the length of a word in a WordRange object.
    //! This setting is only used if order() is Order::lex.
    //!
    //! \param n the upper bound.
    //!
    //! \returns A reference to \c *this.
    //!
    //! \exception
    //! \no_libsemigroups_except
    WordRange& upper_bound(size_type n) {
      _current_valid &= (n == _upper_bound);
      _upper_bound = n;
      return *this;
    }

    //! \brief The current upper bound on the length of a word in the range.
    //!
    //! Returns the current upper bound on the length of a word in a WordRange
    //! object. This setting is only used if order() is Order::lex.
    //!
    //! \returns A value of type \ref size_type.
    //!
    //! \exception
    //! \noexcept
    [[nodiscard]] size_type upper_bound() const noexcept {
      return _upper_bound;
    }

    //! \brief Set the first word in the range by length.
    //!
    //! Sets the first word in a WordRange object to be  `pow(0_w, val)` (the
    //! word consisting of \p val letters equal to \c 0).
    //!
    //! \param val the exponent.
    //!
    //! \returns A reference to \c *this.
    //!
    //! \exception
    //! \no_libsemigroups_except
    WordRange& min(size_type val) {
      first(word_type(val, 0));
      return *this;
    }

    // No corresponding getter for min, because what would it mean? Could be the
    // length of first(), but that doesn't correspond well to what happens with
    // the setter.

    //! \brief Set one past the last word in the range by length.
    //!
    //! Sets one past the last word in a WordRange object to be `pow(0_w, val)`
    //! (the word consisting of \p val letters equal to \c 0).
    //!
    //! \param val the exponent.
    //!
    //! \returns A reference to \c *this.
    //!
    //! \exception
    //! \no_libsemigroups_except
    WordRange& max(size_type val) {
      last(word_type(val, 0));
      return *this;
    }

    //! \brief Returns an input iterator pointing to the first word in the
    //! range.
    //!
    //! This function returns an input iterator pointing to the first word in
    //! a WordRange object.
    //!
    //! \returns An input iterator.
    //!
    //! \exception
    //! \noexcept
    //!
    //! \note The return type of \ref end might be different from the return
    //! type of \ref begin.
    //!
    //! \sa \ref end.
    // REQUIRED so that we can use Strings in range based loops
    auto begin() const noexcept {
      return rx::begin(*this);
    }

    //! \brief Returns an input iterator pointing one beyond the last word in
    //! the range.
    //!
    //! This function returns an input iterator pointing one beyond the last
    //! word in a WordRange object.
    //!
    //! \returns An input iterator.
    //!
    //! \exception
    //! \noexcept
    //!
    //! \note The return type of \ref end might be different from the return
    //! type of \ref begin.
    //!
    //! \sa \ref begin.
    // REQUIRED so that we can use Strings in range based loops
    auto end() const noexcept {
      return rx::end(*this);
    }

    // TODO(now) this doc doesn't feel nice, but JDE can't think of a good way
    // to write it.
    //! \brief Returns whether or not the settings have been changed since the
    //! last time either \ref next or \ref get has been called.
    //!
    //! Other than by calling \ref next, the value returned by \ref get may be
    //! altered by a call to one of the following:
    //! * \ref order(Order)
    //! * \ref number_of_letters(size_type)
    //! * \ref min(size_type)
    //! * \ref max(size_type)
    //! * \ref first(word_type const&)
    //! * \ref last(word_type const&)
    //! * \ref upper_bound(size_type)
    //!
    //! This function returns \c true if none of the above settings have been
    //! changed since the last time \ref next or \ref get is called, and \c
    //! false otherwise.
    //!
    //! \returns A value of type `bool`.
    // Required so Strings can accurately set _current_valid
    bool valid() const noexcept {
      return _current_valid;
    }
  };

  ////////////////////////////////////////////////////////////////////////
  // Strings -> WordRange
  ////////////////////////////////////////////////////////////////////////

  //! \ingroup word_range_group
  //! \brief Returns the index of a character in human readable order.
  //!
  //! Defined in `words.hpp`.
  //!
  //! This function is the inverse of \ref human_readable_char, see the
  //! documentation of that function for more details.
  //!
  //! \param c character whose index is sought.
  //!
  //! \returns A value of type \ref letter_type.
  //!
  //! \exception
  //! \no_libsemigroups_except
  //!
  //! \sa human_readable_char
  [[nodiscard]] letter_type human_readable_index(char c);

  //! \ingroup word_range_group
  //! \brief Class for converting strings to \ref word_type with specified
  //! alphabet.
  //!
  //! Defined in `words.hpp`.
  //!
  //! An instance of this class is used to convert from std::string to \ref
  //! word_type. The characters in the string are converted to integers
  //! according to their position in alphabet used to construct a ToWord
  //! instance if one is provided, or using \ref human_readable_index otherwise.
  //!
  //! \par Example
  //! \code
  //! ToWord toword("bac");
  //! toword("bac");        // returns {0, 1, 2}
  //! toword("bababbbcbc"); // returns { 0, 1, 0, 1, 0, 0, 0, 2, 0, 2}
  //!
  //! toword.init();
  //! toword("bac");        // returns {1, 0, 2}
  //! \endcode
  // TODO (later) a version that takes a word_type, so that we can permute the
  // letters in a word
  class ToWord {
   public:
    //! \brief Default constructor.
    //!
    //! Constructs an empty object with no alphabet set.
    // Not noexcept because std::array::fill isn't
    ToWord() : _lookup() {
      init();
    }

    // TODO (later) noexcept?
    //! \brief Default copy constructor.
    //!
    //! Default copy constructor.
    ToWord(ToWord const&);

    //! \brief Default move constructor.
    //!
    //! Default move constructor.
    ToWord(ToWord&&);

    //! \brief Default copy assignment.
    //!
    //! Default copy assignment.
    ToWord& operator=(ToWord const&);

    //! \brief Default move assignment.
    //!
    //! Default move assignment.
    ToWord& operator=(ToWord&&);

    //! \brief Default destructor.
    //!
    //! Default destructor.
    ~ToWord();

    //! \brief Initialize an existing ToWord object.
    //!
    //! This function puts a ToWord object back into the same state as if it had
    //! been newly default constructed.
    //!
    //! \returns A reference to \c *this.
    //!
    //! \exception
    //! \no_libsemigroups_except
    //!
    //! \sa ToWord()
    ToWord& init() {
      _lookup.fill(UNDEFINED);
      _lookup.back() = 0;
      return *this;
    }

    //! \brief Construct with given alphabet.
    //!
    //! Construct a ToWord object with the given alphabet.
    //!
    //! \param alphabet the alphabet.
    //!
    //! \throws LibsemigroupsException if there are repeated letters in
    //! \p alphabet.
    explicit ToWord(std::string const& alphabet) : _lookup() {
      init(alphabet);
    }

    //! \brief Initialize an existing ToWord object.
    //!
    //! This function puts a ToWord object back into the same state as if it had
    //! been newly constructed from \p alphabet.
    //!
    //! \param alphabet the alphabet
    //!
    //! \returns A reference to \c *this.
    //!
    //! \throws LibsemigroupsException if there are repeated letters in
    //! \p alphabet.
    //!
    //! \sa ToWord(std::string const& alphabet)
    ToWord& init(std::string const& alphabet);

    //! \brief Check if the alphabet is defined.
    //!
    //! This function return \c true if no alphabet has been defined, and \c
    //! false otherwise.
    //!
    //! \returns A value of type \c bool.
    //!
    //! \exception
    //! \no_libsemigroups_except
    [[nodiscard]] bool empty() const {
      return _lookup.back() == 0;
    }

    //! \brief Convert a string to a word_type.
    //!
    //! This function converts its second argument \p input into a word_type and
    //! stores the result in the first argument \p output. The characters of \p
    //! input are converted using the alphabet used to construct the object or
    //! set via init(), or with \ref human_readable_index if \ref empty returns
    //! `true`.
    //!
    //! The contents of the first argument \p output, if any, is removed.
    //!
    //! \param output word to hold the result
    //! \param input the string to convert
    //!
    //! \warning This functions performs no checks on its arguments. In
    //! particular, if the alphabet used to define an instance of ToWord is not
    //! empty, and \p input contains letters that do not correspond to letters
    //! of the alphabet, then bad things will happen.
    //!
    //! \sa
    //! * to_word
    //! * to_string
    //! * \ref literals
    void call_no_checks(word_type& output, std::string const& input) const;

    //! \brief Convert a string to a word_type.
    //!
    //! This function converts its argument \p input into a word_type The
    //! characters of \p input are converted using the alphabet used to
    //! construct the object or set via init(), or with \ref
    //! human_readable_index if \ref empty returns `true`.
    //!
    //! \param input the string to convert
    //!
    //! \warning This functions performs no checks on its arguments. In
    //! particular, if the alphabet used to define an instance of ToWord is not
    //! empty, and \p input contains letters that do not correspond to letters
    //! of the alphabet, then bad things will happen.
    //!
    //! \sa
    //! * to_word
    //! * to_string
    //! * \ref literals
    [[nodiscard]] word_type call_no_checks(std::string const& input) const {
      word_type output;
      call_no_checks(output, input);
      return output;
    }

    //! \brief Convert a string to a word_type.
    //!
    //! This function converts its second argument \p input into a word_type and
    //! stores the result in the first argument \p output. The characters of \p
    //! input are converted using the alphabet used to construct the object or
    //! set via init(), or with \ref human_readable_index if \ref empty returns
    //! `true`.
    //!
    //! The contents of the first argument \p output, if any, is removed.
    //!
    //! \param output word to hold the result
    //! \param input the string to convert
    //!
    //! \throw LibsemigroupsException if the alphabet used to define an instance
    //! of ToWord is not empty and \p input contains letters that do not
    //! correspond to letters of the alphabet.
    //!
    //! \sa
    //! * to_word
    //! * to_string
    //! * \ref literals
    void operator()(word_type& output, std::string const& input) const;

    //! \brief Convert a string to a word_type.
    //!
    //! This function converts its argument \p input into a word_type The
    //! characters of \p input are converted using the alphabet used to
    //! construct the object or set via init(), or with \ref
    //! human_readable_index if \ref empty returns `true`.
    //!
    //! \param input the string to convert
    //!
    //! \throw LibsemigroupsException if the alphabet used to define an instance
    //! of ToWord is not empty and \p input contains letters that do not
    //! correspond to letters of the alphabet.
    //!
    //! \sa
    //! * \ref literals
    [[nodiscard]] word_type operator()(std::string const& input) const {
      word_type output;
                operator()(output, input);
      return output;
    }

   private:
    template <typename InputRange>
    struct Range {
      using output_type = word_type;

      static constexpr bool is_finite     = rx::is_finite_v<InputRange>;
      static constexpr bool is_idempotent = rx::is_idempotent_v<InputRange>;

      InputRange    _input;
      ToWord const* _to_word;

      explicit Range(InputRange const& input, ToWord const& t_wrd) noexcept
          : _input(input), _to_word(&t_wrd) {}

      explicit Range(InputRange&& input, ToWord const& t_wrd) noexcept
          : _input(std::move(input)), _to_word(&t_wrd) {}

      // Not noexcept because ToWord()() isn't
      [[nodiscard]] output_type get() const {
        return _to_word->operator()(_input.get());
      }

      constexpr void next() noexcept {
        _input.next();
      }

      [[nodiscard]] constexpr bool at_end() const noexcept {
        return _input.at_end();
      }

      [[nodiscard]] constexpr size_t size_hint() const noexcept {
        return _input.size_hint();
      }
    };

   public:
    //! \brief Call operator for combining with other range objects.
    //!
    //! A custom combinator for rx::ranges to convert the output of a Strings
    //! object into \ref word_type, that can be combined with other combinators
    //! using `operator|`.
    //!
    //! \par Example
    //! \code
    //!  Strings strings;
    //!  strings.letters("ab").first("a").last("bbbb");
    //!  auto words = (strings | ToWord("ba"));
    //!  // contains the words
    //!  // {1_w,    0_w,    11_w,   10_w,   01_w,   00_w,   111_w,
    //!  //  110_w,  101_w,  100_w,  011_w,  010_w,  001_w,  000_w,
    //!  //  1111_w, 1110_w, 1101_w, 1100_w, 1011_w, 1010_w, 1001_w,
    //!  //  1000_w, 0111_w, 0110_w, 0101_w, 0100_w, 0011_w, 0010_w,
    //!  //  0001_w}));
    //! \endcode
    template <typename InputRange,
              typename = std::enable_if_t<rx::is_input_or_sink_v<InputRange>>>
    [[nodiscard]] constexpr auto operator()(InputRange&& input) const {
      using Inner = rx::get_range_type_t<InputRange>;
      return Range<Inner>(std::forward<InputRange>(input), *this);
    }

   private:
    std::array<letter_type, 256> _lookup;
  };

  ////////////////////////////////////////////////////////////////////////
  // WordRange -> Strings
  ////////////////////////////////////////////////////////////////////////

  //! \ingroup word_range_group
  //! \brief Returns a character by index in human readable order.
  //!
  //! This function exists to map the numbers \c 0 to \c 254 to the possible
  //! values of a \c char, in such a way that the first characters are \c
  //! a-zA-Z0-9. The ascii ranges for these characters are: \f$[97, 123)\f$,
  //! \f$[65, 91)\f$, \f$[48, 58)\f$ so the remaining range of chars that are
  //! appended to the end after these chars are \f$[0,48)\f$, \f$[58, 65)\f$,
  //! \f$[91, 97)\f$, \f$[123, 255)\f$.
  //!
  //! This function is the inverse of \ref human_readable_index.
  //!
  //! \param i the index of the character.
  //!
  //! \returns A value of type \c char.
  //!
  //! \throws LibsemigroupsException if \p i exceeds the number of characters.
  [[nodiscard]] char human_readable_char(size_t i);

  //! \ingroup word_range_group
  //! \brief Check if the word is valid over the given alphabet
  //!
  //! This function checks if every value in the range `[first, last)` is in the
  //! range \f$[0, n)\f$.
  //!
  //! \tparam Iterator the type of the second and third arguments.
  //!
  //! \param n one beyond the upper bound of the valid alphabet.
  //! \param first iterator pointing to the first letter of the word to
  //! validate.
  //! \param last iterator pointing one beyond the last letter of the
  //! word to validate.
  //!
  //! \throws LibsemigroupsException if any letter in the range `[first, last)`
  //! is not in the range \f$[0, n)\f$.
  template <typename Iterator>
  void validate_word(size_t n, Iterator first, Iterator last) {
    for (auto it = first; it != last; ++it) {
      if (*it < 0 || *it >= n) {
        LIBSEMIGROUPS_EXCEPTION(fmt::format(
            "invalid letter {}, letters must be in the range [0, {})", *it, n));
      }
    }
  }

  //! \ingroup word_range_group
  //! \brief Convert a word_type to a string.
  //!
  //! This function converts the word_type pointed to by the second and third
  //! arguments \p input_first and \p inpput_last into a \ref std::string, and
  //! stores the result in the final argument \p output. The characters in
  //! `[input_first, input_last)` are converted using the fourth parameter \p
  //! alphabet, so that \c i is mapped to \c alphabet[i] for each letter \c i.
  //!
  //! The contents of the fourth argument \p output, if any, are removed.
  //!
  //! \tparam Iterator the type of the second and third arguments.
  //!
  //! \param alphabet the alphabet to use for the conversion.
  //! \param input_first iterator pointing at the first letter of the word to
  //! convert
  //! \param input_last iterator pointing one beyond the last letter of
  //! the word to convert
  //! \param output word to hold the result
  //!
  //! \exception
  //! \no_libsemigroups_except
  //!
  //! \warning This function performs no checks on its arguments, and so in
  //! particular if any letter in the word being converted is not less than \p
  //! alphabet.size(), then bad things may happen.
  //!
  //! \sa
  //! * ToWord
  //! * to_word
  //! * \ref literals
  template <typename Iterator>
  void to_string_no_checks(std::string_view alphabet,
                           Iterator         input_first,
                           Iterator         input_last,
                           std::string&     output) {
    output.resize(std::distance(input_first, input_last));
    size_t i = 0;
    for (auto it = input_first; it != input_last; ++it) {
      output[i++] = alphabet[*it];
    }
  }

  //! TODO doc
  template <typename Iterator>
  void to_string(std::string_view alphabet,
                 Iterator         input_first,
                 Iterator         input_last,
                 std::string&     output) {
    validate_word(alphabet.size(), input_first, input_last);
    to_string_no_checks(alphabet, input_first, input_last, output);
  }

  //! \ingroup word_range_group
  //! \brief Convert a word_type to a string.
  //!
  //! See to_string_no_checks(std::string const&, Iterator, Iterator,
  //! std::string&). The difference is that this takes a \ref word_type to
  //! convert, rather than a pair of iterators.
  //!
  //! \warning This function performs no checks on its arguments, and so in
  //! particular if any letter in the word being converted is not less than \p
  //! alphabet.size(), then bad things may happen.
  static inline void to_string_no_checks(std::string_view alphabet,
                                         word_type const& input,
                                         std::string&     output) {
    to_string_no_checks(alphabet, input.cbegin(), input.cend(), output);
  }

  //! TODO doc
  static inline void to_string(std::string_view alphabet,
                               word_type const& input,
                               std::string&     output) {
    validate_word(alphabet.size(), input.cbegin(), input.cend());
    to_string_no_checks(alphabet, input, output);
  }

  //! \ingroup word_range_group
  //! \brief Convert a word_type to a string.
  //!
  //! See to_string_no_checks(std::string const&, Iterator, Iterator,
  //! std::string&). The difference is that this function returns a new string.
  //!
  //! \warning This function performs no checks on its arguments, and so in
  //! particular if any letter in the word being converted is not less than \p
  //! alphabet.size(), then bad things may happen.
  template <typename Iterator>
  [[nodiscard]] std::string to_string_no_checks(std::string_view alphabet,
                                                Iterator         first,
                                                Iterator         last) {
    std::string output;
    to_string_no_checks(alphabet, first, last, output);
    return output;
  }

  //! TODO doc
  template <typename Iterator>
  [[nodiscard]] std::string to_string(std::string_view alphabet,
                                      Iterator         first,
                                      Iterator         last) {
    validate_word(alphabet.size(), first, last);
    return to_string_no_checks(alphabet, first, last);
  }

  //! \ingroup word_range_group
  //! \brief Convert a word_type to a string.
  //!
  //! See to_string(std::string_view, word_type const&, std::string&).
  //! The difference is that this function returns a new string.
  //!
  //! \warning This function performs no checks on its arguments, and so in
  //! particular if any letter in the word being converted is not less than \p
  //! alphabet.size(), then bad things may happen.
  [[nodiscard]] static inline std::string
  to_string_no_checks(std::string_view alphabet, word_type const& input) {
    return to_string_no_checks(alphabet, input.cbegin(), input.cend());
  }

  //! TODO doc
  [[nodiscard]] static inline std::string to_string(std::string_view alphabet,
                                                    word_type const& input) {
    validate_word(alphabet.size(), input.cbegin(), input.cend());
    return to_string_no_checks(alphabet, input);
  }

  // TODO (later) single arg to_string(word_type const&).
  // TODO (later) ToString object that stores the alphabet

  ////////////////////////////////////////////////////////////////////////
  // Strings
  ////////////////////////////////////////////////////////////////////////

  //! \ingroup word_range_group
  //! \brief Returns a random string.
  //!
  //! Returns a random string with length \p length over alphabet \p alphabet.
  //!
  //! \param alphabet the alphabet over which the string is constructed.
  //! \param length the length of the string.
  //!
  //! \returns A random string, value of `std::string`.
  //!
  //! \exception
  //! \no_libsemigroups_except
  //!
  //! \sa \ref random_word
  std::string random_string(std::string const& alphabet, size_t length);

  //! \ingroup word_range_group
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

  //! \ingroup word_range_group
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
    return rx::generate([&] { return random_string(alphabet, min, max); })
           | rx::take(number);
  }

  //! \ingroup word_range_group
  //! \brief Class for generating strings in a given range and in a particular
  //! order.
  //!
  //! Defined in `words.hpp`.
  //!
  //! This class implements a range object for strings and produces the same
  //! output as `WordRange() | ToStrings("ab")`, but is more convenient in some
  //! cases.
  //!
  //! \note There is a small overhead to using a Strings object rather than
  //! using \ref cbegin_wislo or \ref cbegin_wilo directly.
  //!
  //! The order and range of the words in a Strings instance can be set using
  //! the member functions:
  //! * \ref order
  //! * \ref alphabet
  //! * \ref min
  //! * \ref max
  //! * \ref first
  //! * \ref last
  //!
  //! \par Example
  //! \code
  //! Strings strings;
  //! strings.order(Order::shortlex) // strings in shortlex order
  //!        .alphabet("ab")         // on 2 letters
  //!        .min(1)                 // of length in the range from 1
  //!        .max(5);                // to 5
  //! \endcode
  //!
  //! \sa WordRange
  // This can in many places be replaced by "WordRange | ToStrings" but this
  // makes some things more awkward and so we retain this class for its
  // convenience.
  class Strings {
   public:
    //! Alias for the size type.
    using size_type = typename std::vector<std::string>::size_type;

    //! The type of the output of the range object.
    using output_type = std::string const&;

   private:
    mutable std::string _current;
    mutable bool        _current_valid;
    std::string         _letters;
    ToWord              _to_word;
    WordRange           _word_range;

    void init_current() const {
      if (!_current_valid) {
        _current       = to_string_no_checks(_letters, _word_range.get());
        _current_valid = true;
      }
    }

   public:
    //! \brief Get the current value.
    //!
    //! Returns the current string in a Strings object.
    //!
    //! \returns A value of type \ref output_type.
    //!
    //! \exception
    //! \noexcept
    //!
    //! \warning If at_end() returns \c true, then the return value of this
    //! function could be anything.
    output_type get() const noexcept {
      init_current();
      return _current;
    }

    //! \brief Advance to the next value.
    //!
    //! Advances a Strings object to the next value (if any).
    //!
    //! \exception
    //! \noexcept
    //!
    //! \sa \ref at_end
    void next() noexcept {
      _word_range.next();
      _current_valid = false;
    }

    //! \brief Check if the range object is exhausted.
    //!
    //! Returns \c true if a Strings object is exhausted, and \c false if not.
    //!
    //! \returns A value of type \c bool.
    //!
    //! \exception
    //! \noexcept
    bool at_end() const noexcept {
      return _word_range.at_end();
    }

    //! \brief The possible size of the range.
    //!
    //! Returns the number of words in a Strings object if order() is
    //! Order::shortlex. If order() is not Order::shortlex, then the return
    //! value of this function is meaningless.
    //!
    //! \returns A value of type \c size_t.
    //!
    //! \exception
    //! \noexcept
    size_t size_hint() const noexcept {
      return _word_range.size_hint();
    }

    //! \brief The actual size of the range.
    //!
    //! Returns the number of words in the range object.
    //! If order() is Order::shortlex, then size_hint() is used. If order() is
    //! not Order::shortlex, then a copy of the range may have to be looped over
    //! in order to find the return value of this function.
    //!
    //! \returns A value of type \c size_t.
    //!
    //! \exception
    //! \noexcept
    size_t count() const noexcept {
      return _word_range.count();
    }

    //! Value indicating that the range is finite.
    static constexpr bool is_finite = true;  // This may not always be true

    //! Value indicating that if get() is called twice on a Strings object that
    //! is not changed between the two calls, then the return value of get() is
    //! the same both times.
    static constexpr bool is_idempotent = true;

    //! \brief Default constructor.
    //!
    //! Constructs an empty range with:
    //! * min() equal to \c 0;
    //! * max() equal to \c 0;
    //! * order() equal to Order::shortlex;
    //! * first() equal to the empty string;
    //! * last() equal to the empty string;
    //! * upper_bound() equal to \c 0;
    //! * letters() equal to \c 0.
    Strings() {
      init();
    }

    //! \brief Initialize an existing Strings object.
    //!
    //! This function puts a Strings object back into the same state as if it
    //! had been newly default constructed.
    //!
    //! \returns A reference to \c *this.
    //!
    //! \exception
    //! \no_libsemigroups_except
    Strings& init();

    //! \brief Default copy constructor.
    Strings(Strings const&);

    //! \brief Default move constructor.
    Strings(Strings&&);

    //! \brief Default copy assignment operator.
    Strings& operator=(Strings const&);

    //! \brief Default move assignment operator.
    Strings& operator=(Strings&&);

    //! \brief Default destructor.
    ~Strings();

    //! \brief Set the alphabet.
    //!
    //! Sets the alphabet in a Strings object.
    //!
    //! \param x the alphabet.
    //!
    //! \returns A reference to \c *this.
    //!
    //! \throws LibsemigroupsException if \p x contains repeated letters.
    Strings& alphabet(std::string const& x);

    //! \brief The current alphabet.
    //!
    //! Returns the current alphabet in a Strings object.
    //!
    //! \returns A value of type \ref std::string.
    //!
    //! \exception
    //! \noexcept
    [[nodiscard]] std::string const& alphabet() const noexcept {
      return _letters;
    }

    //! \brief Set the first string in the range.
    //!
    //! Sets the first string in a Strings object to be \p frst.
    //!
    //! \param frst the first string.
    //!
    //! \returns A reference to \c *this.
    //!
    //! \sa \ref min
    //!
    //! \note Unlike WordRange::first, this function will throw if \p frst
    //! contains letters not belonging to alphabet().
    Strings& first(std::string const& frst) {
      _word_range.first(_to_word(frst));
      _current_valid = _word_range.valid();
      return *this;
    }

    //! \brief The current first string in the range.
    //!
    //! Returns the first string in a Strings object.
    //!
    //! \returns A \ref std::string by value.
    //!
    //! \exception
    //! \noexcept
    //!
    //! \sa \ref min
    [[nodiscard]] std::string first() const noexcept {
      return to_string_no_checks(_letters, _word_range.first());
    }

    //! \brief Set one past the last string in the range.
    //!
    //! Sets one past the last string in a Strings object to be \p lst. This
    //! function performs no checks on its arguments. If \p lst contains
    //! letters greater than letters(), then the Strings object will be empty.
    //!
    //! \param lst the first string.
    //!
    //! \returns A reference to \c *this.
    //!
    //! \sa \ref max
    //!
    //! \note Unlike WordRange::last, this function will throw if \p lst
    //! contains letters not belonging to alphabet().
    Strings& last(std::string const& lst) {
      _word_range.last(_to_word(lst));
      _current_valid = _word_range.valid();
      return *this;
    }

    //! \brief The current one past the last string in the range.
    //!
    //! Returns the last string in a Strings object.
    //!
    //! \returns A \ref std::string by value.
    //!
    //! \exception
    //! \noexcept
    //!
    //! \sa \ref max
    [[nodiscard]] std::string last() const noexcept {
      return to_string_no_checks(_letters, _word_range.last());
    }

    //! \brief Set the order of the strings in the range.
    //!
    //! Sets the order of the strings in a Strings object to \p val.
    //!
    //! \param val the order.
    //!
    //! \returns A reference to \c *this.
    //!
    //! \throws LibsemigroupsException if val is not Order::shortlex or
    //! Order::lex.
    Strings& order(Order val) {
      _word_range.order(val);
      _current_valid = _word_range.valid();
      return *this;
    }

    //! \brief The current order of the strings in the range.
    //!
    //! Returns the current order of the strings in a Strings object.
    //!
    //! \returns A value of type \ref Order.
    //!
    //! \exception
    //! \noexcept
    [[nodiscard]] Order order() const noexcept {
      return _word_range.order();
    }

    //! \brief Set an upper bound for the length of a string in the range.
    //!
    //! Sets an upper bound for the length of a string in a Strings object.
    //! This setting is only used if order() is Order::lex.
    //!
    //! \param n the upper bound.
    //!
    //! \returns A reference to \c *this.
    //!
    //! \exception
    //! \no_libsemigroups_except
    Strings& upper_bound(size_type n) {
      _word_range.upper_bound(n);
      _current_valid = _word_range.valid();
      return *this;
    }

    //! \brief The current upper bound on the length of a string in the range.
    //!
    //! Returns the current upper bound on the length of a string in a Strings
    //! object. This setting is only used if order() is Order::lex.
    //!
    //! \returns A value of type \ref size_type.
    //!
    //! \exception
    //! \noexcept
    [[nodiscard]] size_type upper_bound() const noexcept {
      return _word_range.upper_bound();
    }

    //! \brief Set the first string in the range by length.
    //!
    //! Sets the first string in a Strings object to be `pow("a", val)` (the
    //! string consisting of \p val letters equal to \c "a").
    //!
    //! \param val the exponent.
    //!
    //! \returns A reference to \c *this.
    //!
    //! \exception
    //! \no_libsemigroups_except
    Strings& min(size_type val) {
      _word_range.min(val);
      _current_valid = _word_range.valid();
      return *this;
    }

    // No corresponding getter for min, because what would it mean? Could be the
    // length of first(), but that doesn't correspond well to what happens with
    // the setter.

    //! \brief Set one past the last string in the range by length.
    //!
    //! Sets one past the last string in a Strings object to be `pow("a", val)`
    //! (the string consisting of \p val letters equal to \c "a").
    //!
    //! \param val the exponent.
    //!
    //! \returns A reference to \c *this.
    //!
    //! \exception
    //! \no_libsemigroups_except
    Strings& max(size_type val) {
      _word_range.max(val);
      _current_valid = _word_range.valid();
      return *this;
    }

    //! \brief Returns an input iterator pointing to the first string in the
    //! range.
    //!
    //! This function returns an input iterator pointing to the first string in
    //! a Strings object.
    //!
    //! \returns An input iterator.
    //!
    //! \exception
    //! \noexcept
    //!
    //! \note The return type of \ref end might be different from the return
    //! type of \ref begin.
    //!
    //! \sa \ref end.
    // REQUIRED so that we can use Strings in range based loops
    auto begin() const noexcept {
      return rx::begin(*this);
    }

    //! \brief Returns an input iterator pointing one beyond the last string in
    //! the range.
    //!
    //! This function returns an input iterator pointing one beyond the last
    //! string in a Strings object.
    //!
    //! \returns An input iterator.
    //!
    //! \exception
    //! \noexcept
    //!
    //! \note The return type of \ref end might be different from the return
    //! type of \ref begin.
    //!
    //! \sa \ref begin.
    // REQUIRED so that we can use Strings in range based loops
    auto end() const noexcept {
      return rx::end(*this);
    }
  };

  ////////////////////////////////////////////////////////////////////////
  // Ranges
  ////////////////////////////////////////////////////////////////////////

  //! \ingroup word_range_group
  //! \brief A custom combinator for rx::ranges to convert the output
  //! of a WordRange object into \ref std::string.
  //!
  //! Defined in `words.hpp`.
  //!
  //! A custom combinator for rx::ranges to convert the output of a WordRange
  //! object (or any other range with \c output_type equal to \ref
  //! word_type) into \ref std::string, that can be combined with other
  //! combinators using `operator|`.
  //!
  //! \par Example
  //! \code
  //!  WordRange words;
  //!  words.letters(2).first(0_w).last(1111_w);
  //!  auto strings = (words | ToStrings("ba"));
  //!  // contains the strings
  //!  // {"b",    "a",    "bb",   "ba",   "ab",   "aa",   "bbb",
  //!  //  "bba",  "bab",  "baa",  "abb",  "aba",  "aab",  "aaa",
  //!  //  "bbbb", "bbba", "bbab", "bbaa", "babb", "baba", "baab",
  //!  //  "baaa", "abbb", "abba", "abab", "abaa", "aabb", "aaba",
  //!  //  "aaab"}));
  //! \endcode
  class ToStrings {
   private:
    std::string _letters;

   public:
    //! \brief Deleted.
    //!
    //! The default constructor is deleted, since the alphabet must defined.
    ToStrings() = delete;

    //! \brief Default copy constructor.
    //!
    //! Default copy constructor.
    ToStrings(ToStrings const&);

    //! \brief Default move constructor.
    //!
    //! Default move constructor.
    ToStrings(ToStrings&&);

    //! \brief Default copy assignment operator.
    //!
    //! Default move assignment operator.
    ToStrings& operator=(ToStrings const&);

    //! \brief Default move assignment operator.
    //!
    //! Default move assignment operator.
    ToStrings& operator=(ToStrings&&);

    //! \brief Default destructor.
    //!
    //! Default destructor.
    ~ToStrings();

    //! \brief Construct from `std::string_view`
    //!
    //! Constructs a ToStrings object using the specified alphabet.
    //!
    //! \param lttrs the letters in the alphabet
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \warning  When combining a ToStrings object with another range, for
    //! example via `operator|`, no checks are done to ensure that the incoming
    //! words use the same letters as the alphabet used to define ToStrings.
    explicit ToStrings(std::string_view lttrs) : _letters(lttrs) {}

    template <typename InputRange>
    struct Range {
      using output_type = std::string;

      static constexpr bool is_finite     = true;
      static constexpr bool is_idempotent = true;

      InputRange  _input;
      std::string _letters;

      constexpr Range(InputRange&& input, ToStrings const& t_strng)
          : _input(std::move(input)), _letters(t_strng._letters) {}

      constexpr Range(InputRange const& input, ToStrings const& t_strng)
          : _input(input), _letters(t_strng._letters) {}

      [[nodiscard]] output_type get() const {
        return to_string_no_checks(_letters, _input.get());
      }

      constexpr void next() noexcept {
        _input.next();
      }

      [[nodiscard]] constexpr bool at_end() const noexcept {
        return _input.at_end();
      }

      [[nodiscard]] constexpr size_t size_hint() const noexcept {
        return _input.size_hint();
      }
    };

    //! \brief Call operator for combining with other range objects.
    //!
    //! This is the call operator that is used by `rx::ranges::operator|` for
    //! combining ranges.
    template <typename InputRange>
    [[nodiscard]] constexpr auto operator()(InputRange&& input) const {
      using Inner = rx::get_range_type_t<InputRange>;
      return Range<Inner>(std::forward<InputRange>(input), *this);
    }
  };

  ////////////////////////////////////////////////////////////////////////
  // Literals
  ////////////////////////////////////////////////////////////////////////

  //! \ingroup word_range_group
  //!
  //! \brief Namespace containing some custom literals for creating words.
  //!
  //! Defined in `words.hpp`.
  //!
  //! This namespace contains some functions for creating \ref word_type objects
  //! in a compact form.
  //! \par Example
  //! \code
  //! 012_w      \\ same as word_type({0, 1, 2})
  //! "abc"_w    \\ also same as word_type({0, 1, 2})
  //! "(ab)^3"_p \\ same as "ababab"
  //! \endcode
  namespace literals {
    //! \anchor literal_operator_w
    //! \brief Literal for defining \ref word_type over integers less than 10.
    //!
    //! This operator provides a convenient brief means of constructing a  \ref
    //! word_type from an sequence of literal integer digits or a string. For
    //! example, \c 0123_w produces the same output as `word_type({0, 1, 2,
    //! 3})` and so too does \c "abcd"_w.
    //!
    //! There are some gotchas and this operator should be used with some care:
    //!
    //! * the parameter \p w must consist of the integers \f$\{0, \ldots,
    //! 9\}\f$ or the characters in `a-zA-Z` but not both.
    //! * if \p w starts with \c 0 and is follows by a value greater than \c 7,
    //! then it is necessary to enclose \p w in quotes. For example, \c 08_w
    //! will not compile because it is interpreted as an invalid octal. However
    //! `"08"_w` behaves as expected.
    //! * if \p w consists of characters in `a-zA-Z`, then the output is
    //! the same as that of `to_word(w)`, see \ref to_word(std::string_view).
    //!
    //! \param w the letters of the word
    //! \param n the length of \p w (defaults to the length of \p w)
    //!
    //! \returns A value of type \ref word_type.
    //!
    //! \throws LibsemigroupsException if the input contains a mixture of
    //! integers and non-integers.
    word_type operator"" _w(const char* w, size_t n);

    //! \brief Literal for defining \ref word_type over integers less than 10.
    //!
    //! See \ref literal_operator_w "operator\"\"_w" for details.
    word_type operator"" _w(const char* w);

    //! \anchor literal_operator_p
    //! \brief Literal for defining \ref word_type by parsing an algebraic
    //! expression.
    //!
    //! This operator provides a convenient concise means of constructing a \ref
    //! word_type from an algebraic expression.
    //! For example, \c "((ab)^3cc)^2"_p equals
    //! \c "abababccabababcc" and \c "a^0"_p equals the empty string \c "".
    //!
    //! This function has the following behaviour:
    //! * arbitrarily nested brackets;
    //! * spaces are ignore;
    //! * redundant matched brackets are ignored;
    //! * only the characters in \c "()^ " and in \c a-zA-Z0-9 are allowed.
    //!
    //! \param w the letters of the word
    //! \param n the length of \p w (defaults to the length of \p w)
    //!
    //! \returns A value of type \ref std::string.
    //!
    //! \throws LibsemigroupsException if the string cannot be parsed.
    std::string operator"" _p(const char* w, size_t n);

    //! \brief Literal for defining \ref word_type by parsing an algebraic
    //! expression.
    //!
    //! See \ref literal_operator_p "operator\"\"_p" for details.
    std::string operator"" _p(const char* w);
  }  // namespace literals

  ////////////////////////////////////////////////////////////////////////

  //! \ingroup word_range_group
  //!
  //! \brief Namespace containing some operators for creating words.
  //!
  //! Defined in `words.hpp`.
  //!
  //! This namespace contains some functions for creating \ref word_type objects
  //! in a compact form.
  //! \par Example
  //! \code
  //! using namespace words;
  //! pow("a", 5)            \\ same as "aaaaa"
  //! 01_w + 2               \\ same as 012_w
  //! 01_w + 01_w            \\ same as 0110_w
  //! prod(0123_w, 0, 16, 3) \\ same as 032103_w
  //! \endcode
  namespace words {

    ////////////////////////////////////////////////////////////////////////
    // operator+
    ////////////////////////////////////////////////////////////////////////

    //! \anchor operator_plus
    //! \brief Concatenate two words.
    //!
    //! Returns the concatenation of \c u and \c w.
    //!
    //! \param u a word
    //! \param w a word
    //!
    //! \returns A \ref word_type.
    //!
    //! \exception
    //! \no_libsemigroups_except
    static inline word_type operator+(word_type const& u, word_type const& w) {
      word_type result(u);
      result.insert(result.end(), w.cbegin(), w.cend());
      return result;
    }

    //! \brief Concatenate a word and a letter.
    //!
    //! See \ref operator_plus "operator+".
    static inline word_type operator+(word_type const& u, letter_type w) {
      word_type result(u);
      result.push_back(w);
      return result;
    }

    //! \brief Concatenate a letter and a word.
    //!
    //! See \ref operator_plus "operator+".
    static inline word_type operator+(letter_type w, word_type const& u) {
      return word_type({w}) + u;
    }

    ////////////////////////////////////////////////////////////////////////
    // operator+=
    ////////////////////////////////////////////////////////////////////////

    //! \anchor operator_plus_equals
    //! \brief Concatenate a word with another word in-place.
    //!
    //! Changes \c u to `u + v` in-place. See \ref operator_plus "operator+".
    //!
    //! \param u a word
    //! \param v a word
    //!
    //! \exception
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
    //! \param w the word
    //! \param n the power
    //!
    //! \exception
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
    //! \param x the word to power
    //! \param n the power
    //!
    //! \returns A Word
    //!
    //! \exception
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
    static inline word_type pow(std::initializer_list<letter_type> ilist,
                                size_t                             n) {
      return pow(word_type(ilist), n);
    }

    //! \brief Returns the power of a string.
    //!
    //! See pow(Word const&, size_t) for details.
    static inline std::string pow(std::string_view w, size_t n) {
      return pow(std::string(w), n);
    }

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
    //! \tparam Container the type of the 1st argument \c elts
    //! \tparam Word the return type (defaults to Container).
    //!
    //! \param elts the ordered set
    //! \param first the first index
    //! \param last the last index
    //! \param step the step
    //!
    //! \return A Word.
    //!
    //! \throws LibsemigroupsException if `step = 0`
    //! \throws LibsemigroupsException if \p elts is empty, but the specified
    //! range is not
    //!
    //! \par Examples
    //! \code
    //! word_type w = 012345_w
    //! prod(w, 0, 5, 2)         // {0, 2, 4}
    //! prod(w, 1, 9, 2)         // {1, 3, 5, 1}
    //! prod("abcde", 4, 1, -1)  // "edc"
    //! prod({"aba", "xyz"}, 0, 4, 1) // "abaxyzabaxyz"
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

    ////////////////////////////////////////////////////////////////////////
    // pow - implementation
    ////////////////////////////////////////////////////////////////////////

    template <typename Word>
    void pow_inplace(Word& x, size_t n) {
      Word y(x);
      x.reserve(x.size() * n);
      if (n % 2 == 0) {
        x = Word({});
      }

      while (n > 1) {
        y += y;
        n /= 2;
        if (n % 2 == 1) {
          x += y;
        }
      }
    }

    ////////////////////////////////////////////////////////////////////////
    // prod - implementation
    ////////////////////////////////////////////////////////////////////////

    // Note: we could do a version of the below using insert on words, where
    // the step is +/- 1.
    template <typename Container, typename Word>
    Word prod(Container const& elts, int first, int last, int step) {
      if (step == 0) {
        LIBSEMIGROUPS_EXCEPTION("the 4th argument must not be 0");
      } else if (((first < last && step > 0) || (first > last && step < 0))
                 && elts.size() == 0) {
        LIBSEMIGROUPS_EXCEPTION("the 1st argument must not be empty if the "
                                "given range is not empty");
      } else if (elts.size() > std::numeric_limits<int>::max()) {
        LIBSEMIGROUPS_EXCEPTION(
            "the 1st argument must have size less than or equal to {}",
            std::numeric_limits<int>::max());
      }
      Word      result;
      int const s = elts.size();

      if (first < last) {
        if (step < 0) {
          return result;
        }
        result.reserve((last - first) / step);

        for (int i = first; i < last; i += step) {
          size_t const a = (i % s + s) % s;
          LIBSEMIGROUPS_ASSERT(static_cast<int>(a) < s);
          result += elts[a];
        }
      } else {
        if (step > 0) {
          return result;
        }
        size_t steppos = static_cast<size_t>(-step);
        result.reserve((first - last) / steppos);
        for (int i = first; i > last; i += step) {
          size_t const a = (i % s + s) % s;
          LIBSEMIGROUPS_ASSERT(static_cast<int>(a) < s);
          result += elts[a];
        }
      }
      return result;
    }
  }  // namespace words
  //
}  // namespace libsemigroups

#endif  // LIBSEMIGROUPS_WORDS_HPP_
