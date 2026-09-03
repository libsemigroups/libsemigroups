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

// This file contains declarations for classes related to words and strings in
// libsemigroups.

#ifndef LIBSEMIGROUPS_WORD_RANGE_CLASS_HPP_
#define LIBSEMIGROUPS_WORD_RANGE_CLASS_HPP_

#include <cstddef>  // for size_t
#include <string>   // for basic_string
#include <variant>  // for variant, visit, operator==
#include <vector>   // for vector, operator==

#include "order.hpp"    // for Order
#include "ranges.hpp"   // for begin, end
#include "to-word.hpp"  // for ToWord
#include "types.hpp"    // for word_type

#include "detail/word-iterators.hpp"  // for const_wilo_iterator, const_wisl...

namespace libsemigroups {
  //! \ingroup words_group
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
  //! \exceptions
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

  //! \ingroup words_group
  //! \brief Returns a forward iterator pointing to the 3rd parameter \p first.
  //! \copydoc cbegin_wilo(size_t, size_t, word_type&&, word_type&&)
  [[nodiscard]] detail::const_wilo_iterator cbegin_wilo(size_t n,
                                                        size_t upper_bound,
                                                        word_type const& first,
                                                        word_type const& last);

  //! \ingroup words_group
  //! \brief Returns a forward iterator pointing to one after the end of the
  //! range from \p first to \p last.
  //!
  //! The iterator returned by this function is still dereferenceable and
  //! incrementable, but does not point to a word in the correct range.
  //!
  //! \sa cbegin_wilo
  [[nodiscard]] detail::const_wilo_iterator
  cend_wilo(size_t n, size_t upper_bound, word_type&& first, word_type&& last);

  //! \ingroup words_group
  //! \brief Returns a forward iterator pointing to one after the end of the
  //! range from \p first to \p last.
  //! \copydoc cend_wilo(size_t, size_t, word_type&&, word_type&&)
  [[nodiscard]] detail::const_wilo_iterator cend_wilo(size_t n,
                                                      size_t upper_bound,
                                                      word_type const& first,
                                                      word_type const& last);

  //! \ingroup words_group
  //! \brief Returns a forward iterator pointing to the 2nd parameter \p first.
  //!
  //! Returns a forward iterator used to iterate over words in
  //! lenlex order (wislo). If incremented, the iterator will point
  //! to the next least lenlex word after \p w over an \p n letter alphabet.
  //! Iterators of the type returned by this function are equal whenever they
  //! are obtained by advancing the return value of any call to \c cbegin_wislo
  //! by the same amount, or they are both obtained by any call to
  //! \c cend_wislo.
  //!
  //! \param n the number of letters in the alphabet.
  //! \param first the starting point for the iteration.
  //! \param last the ending point for the iteration.
  //!
  //! \returns An iterator pointing to \p first.
  //!
  //! \exceptions
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

  //! \ingroup words_group
  //! \brief Returns a forward iterator pointing to the 2nd parameter \p first.
  //! \copydoc cbegin_wislo(size_t const, word_type&&, word_type&&)
  [[nodiscard]] detail::const_wislo_iterator
  cbegin_wislo(size_t n, word_type const& first, word_type const& last);

  //! \ingroup words_group
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

  //! \ingroup words_group
  //! \brief Returns a forward iterator pointing to one after the end of the
  //! range from \p first to \p last.
  //! \copydoc cend_wislo(size_t const, word_type&&, word_type&&)
  [[nodiscard]] detail::const_wislo_iterator cend_wislo(size_t           n,
                                                        word_type const& first,
                                                        word_type const& last);

  //! \ingroup words_group
  //! \brief Class for generating words in a given range and in a particular
  //! order.
  //!
  //! Defined in `word-range.hpp`.
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
  //! * \ref alphabet_size
  //! * \ref min
  //! * \ref max
  //! * \ref first
  //! * \ref last
  //!
  //! \par Example
  //! \code
  //! WordRange words;
  //! words.order(Order::lenlex)  // words in lenlex order
  //!      .alphabet_size(2)      // on 2 letters
  //!      .min(1)                // of length in the range from 1
  //!      .max(5);               // to 5
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

    size_type              _alphabet_size;
    mutable const_iterator _current;
    mutable const_iterator _end;
    mutable bool           _current_valid;
    word_type              _first;
    word_type              _last;
    Order                  _order;
    size_type              _upper_bound;
    mutable size_type      _visited;

    void set_iterator() const;

   public:
    //! \brief Get the current value.
    //!
    //! Returns the current word in a WordRange object.
    //!
    //! \returns A value of type \ref output_type.
    //!
    //! \exceptions
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
    //! \exceptions
    //! \noexcept
    //!
    //! \sa \ref at_end
    void next() noexcept {
      set_iterator();
      if (!at_end()) {
        ++_visited;
      }
      std::visit([](auto& it) { ++it; }, _current);
    }

    //! \brief Check if the range object is exhausted.
    //!
    //! Returns \c true if a WordRange object is exhausted, and \c false if not.
    //! \returns A value of type \c bool.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] bool at_end() const noexcept {
      set_iterator();
      return _current == _end;
    }

    //! \brief The possible size of the range.
    //!
    //! Returns the number of words in a WordRange object if order() is
    //! Order::lenlex. If order() is not Order::lenlex, then the return
    //! value of this function is meaningless.
    //!
    //! \returns A value of type \c size_t.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] size_t size_hint() const noexcept {
      return number_of_words(_alphabet_size, _first.size(), _last.size())
             - _visited;
      // This is only the actual size if _order is lenlex
    }

    //! \brief The actual size of the range.
    //!
    //! Returns the number of words in a WordRange object. If order() is
    //! Order::lenlex, then size_hint() is used. If order() is not
    //! Order::lenlex, then a copy of the range may have to be looped over in
    //! order to find the return value of this function.
    //!
    //! \returns A value of type \c size_t.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] size_t count() const noexcept;

    // For some reason, there needs to be two doxygen comment lines here for
    // this to render.
    //! Value indicating that the range is finite.
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
    //! * order() equal to Order::lenlex;
    //! * first() equal to the empty word;
    //! * last() equal to the empty word;
    //! * upper_bound() equal to \c 0;
    //! * alphabet_size() equal to \c 0.
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
    //! \exceptions
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
    //! \exceptions
    //! \noexcept
    WordRange& alphabet_size(size_type n) noexcept {
      _current_valid &= (n == _alphabet_size);
      _alphabet_size = n;
      return *this;
    }

    //! \brief The current number of letters in the alphabet.
    //!
    //! Returns the current number of letters in a WordRange object.
    //!
    //! \returns A value of type \ref size_type.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] size_type alphabet_size() const noexcept {
      return _alphabet_size;
    }

    //! \brief Set the first word in the range.
    //!
    //! Sets the first word in a WordRange object to be \p frst. This function
    //! performs no checks on its arguments. If \p frst contains letters greater
    //! than alphabet_size(), then the WordRange object will be empty.
    //! Similarly, if first() is greater than last() with respect to order(),
    //! then the object will be empty.
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
    //! \exceptions
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
    //! letters greater than alphabet_size(), then the WordRange object will be
    //! empty.
    //!
    //! \param lst one past the last word.
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
    //! Returns one past the last word in a WordRange object.
    //!
    //! \returns A const reference to a \ref word_type.
    //!
    //! \exceptions
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
    //! \throws LibsemigroupsException if val is not Order::lenlex or
    //! Order::lex.
    WordRange& order(Order val);

    //! \brief The current order of the words in the range.
    //!
    //! Returns the current order of the words in a WordRange object.
    //!
    //! \returns A value of type \ref Order.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] Order order() const noexcept {
      return _order;
    }

    template <typename Cmp>
    WordRange& order(Cmp&& cmp);

    //! \brief Set an upper bound for the length of a word in the range.
    //!
    //! Sets an upper bound for the length of a word in a WordRange object.
    //! This setting is only used if order() is Order::lex.
    //!
    //! \param n the upper bound.
    //!
    //! \returns A reference to \c *this.
    //!
    //! \exceptions
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
    //! \exceptions
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
    //! \exceptions
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
    //! \exceptions
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
    //! \exceptions
    //! \noexcept
    //!
    //! \note The return type of \ref end might be different from the return
    //! type of \ref begin.
    //!
    //! \sa \ref end.
    // REQUIRED so that we can use StringRange in range based loops
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
    //! \exceptions
    //! \noexcept
    //!
    //! \note The return type of \ref end might be different from the return
    //! type of \ref begin.
    //!
    //! \sa \ref begin.
    // REQUIRED so that we can use StringRange in range based loops
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
    //! * \ref alphabet_size(size_type)
    //! * \ref min(size_type)
    //! * \ref max(size_type)
    //! * \ref first(word_type const&)
    //! * \ref last(word_type const&)
    //! * \ref upper_bound(size_type)
    //!
    //! This function returns \c true if none of the above settings have been
    //! changed since the last time \ref next or \ref get is called, and
    //! \c false otherwise.
    //!
    //! \returns A value of type `bool`.
    // Required so StringRange can accurately set _current_valid
    bool valid() const noexcept {
      return _current_valid;
    }
  };

  //! \ingroup words_group
  //!
  //! \brief Return a human readable representation of a WordRange object.
  //!
  //! Return a human readable representation of a WordRange object.
  //!
  //! \param wr the WordRange object.
  //! \param max_width the maximum width of the returned string (default:
  //! \c 72).
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  [[nodiscard]] std::string to_human_readable_repr(WordRange const& wr,
                                                   size_t max_width = 72);

  ////////////////////////////////////////////////////////////////////////
  // StringRange
  ////////////////////////////////////////////////////////////////////////

  //! \ingroup words_group
  //! \brief Class for generating strings in a given range and in a particular
  //! order.
  //!
  //! Defined in `word-range.hpp`.
  //!
  //! This class implements a range object for strings and produces the same
  //! output as `WordRange() | ToString("ab")`, but is more convenient in some
  //! cases.
  //!
  //! \note There is a small overhead to using a StringRange object rather than
  //! using \ref cbegin_wislo or \ref cbegin_wilo directly.
  //!
  //! The order and range of the words in a StringRange instance can be set
  //! using the member functions:
  //! * \ref order
  //! * \ref alphabet
  //! * \ref min
  //! * \ref max
  //! * \ref first
  //! * \ref last
  //!
  //! \par Example
  //! \code
  //! StringRange strings;
  //! strings.order(Order::lenlex) // strings in lenlex order
  //!        .alphabet("ab")       // on 2 letters
  //!        .min(1)               // of length in the range from 1
  //!        .max(5);              // to 5
  //! \endcode
  //!
  //! \sa WordRange
  // This can in many places be replaced by "WordRange | ToString" but this
  // makes some things more awkward and so we retain this class for its
  // convenience.
  class StringRange {
   public:
    //! Alias for the size type.
    using size_type = typename std::vector<std::string>::size_type;

    //! The type of the output of the range object.
    using output_type = std::string const&;

   private:
    mutable std::string     _current;
    mutable bool            _current_valid;
    std::string             _letters;
    v4::ToWord<std::string> _to_word;
    ToString                _to_string;
    WordRange               _word_range;

    void init_current() const {
      if (!_current_valid) {
        _current       = _to_string(_word_range.get());
        _current_valid = true;
      }
    }

   public:
    //! \brief Get the current value.
    //!
    //! Returns the current string in a StringRange object.
    //!
    //! \returns A value of type \ref output_type.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \warning If at_end() returns \c true, then the return value of this
    //! function could be anything.
    output_type get() const {
      init_current();
      return _current;
    }

    //! \brief Advance to the next value.
    //!
    //! Advances a StringRange object to the next value (if any).
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \sa \ref at_end
    void next() noexcept {
      _word_range.next();
      _current_valid = false;
    }

    //! \brief Check if the range object is exhausted.
    //!
    //! Returns \c true if a StringRange object is exhausted, and \c false if
    //! not.
    //!
    //! \returns A value of type \c bool.
    //!
    //! \exceptions
    //! \noexcept
    bool at_end() const noexcept {
      return _word_range.at_end();
    }

    //! \brief The possible size of the range.
    //!
    //! Returns the number of words in a StringRange object if order() is
    //! Order::lenlex. If order() is not Order::lenlex, then the return
    //! value of this function is meaningless.
    //!
    //! \returns A value of type \c size_t.
    //!
    //! \exceptions
    //! \noexcept
    size_t size_hint() const noexcept {
      return _word_range.size_hint();
    }

    //! \brief The actual size of the range.
    //!
    //! Returns the number of words in the range object.
    //! If order() is Order::lenlex, then size_hint() is used. If order() is
    //! not Order::lenlex, then a copy of the range may have to be looped over
    //! in order to find the return value of this function.
    //!
    //! \returns A value of type \c size_t.
    //!
    //! \exceptions
    //! \noexcept
    size_t count() const noexcept {
      return _word_range.count();
    }

    //! Value indicating that the range is finite.
    static constexpr bool is_finite = true;  // This may not always be true

    //! Value indicating that if get() is called twice on a StringRange object
    //! that is not changed between the two calls, then the return value of
    //! get() is the same both times.
    static constexpr bool is_idempotent = true;

    //! \brief Default constructor.
    //!
    //! Constructs an empty range with:
    //! * min() equal to \c 0;
    //! * max() equal to \c 0;
    //! * order() equal to Order::lenlex;
    //! * first() equal to the empty string;
    //! * last() equal to the empty string;
    //! * upper_bound() equal to \c 0;
    //! * alphabet() equal to `""`.
    StringRange() {
      init();
    }

    //! \brief Initialize an existing StringRange object.
    //!
    //! This function puts a StringRange object back into the same state as if
    //! it had been newly default constructed.
    //!
    //! \returns A reference to \c *this.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    StringRange& init();

    //! \brief Default copy constructor.
    StringRange(StringRange const&);

    //! \brief Default move constructor.
    StringRange(StringRange&&);

    //! \brief Default copy assignment operator.
    StringRange& operator=(StringRange const&);

    //! \brief Default move assignment operator.
    StringRange& operator=(StringRange&&);

    //! \brief Default destructor.
    ~StringRange();

    //! \brief Set the alphabet.
    //!
    //! Sets the alphabet in a StringRange object.
    //!
    //! \param x the alphabet.
    //!
    //! \returns A reference to \c *this.
    //!
    //! \throws LibsemigroupsException if \p x contains repeated letters.
    StringRange& alphabet(std::string const& x);

    //! \brief The current alphabet.
    //!
    //! Returns the current alphabet in a StringRange object.
    //!
    //! \returns A value of type \ref std::string.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] std::string const& alphabet() const noexcept {
      return _letters;
    }

    //! \brief Set the first string in the range.
    //!
    //! Sets the first string in a StringRange object to be \p frst.
    //!
    //! \param frst the first string.
    //!
    //! \returns A reference to \c *this.
    //!
    //! \note Unlike WordRange::first, this function will throw if \p frst
    //! contains letters not belonging to alphabet().
    //!
    //! \sa \ref min
    StringRange& first(std::string const& frst) {
      _word_range.first(_to_word(frst));
      _current_valid &= _word_range.valid();
      return *this;
    }

    //! \brief The current first string in the range.
    //!
    //! Returns the first string in a StringRange object.
    //!
    //! \returns A \ref std::string by value.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \sa \ref min
    [[nodiscard]] std::string first() const noexcept {
      return _to_string(_word_range.first());
    }

    //! \brief Set one past the last string in the range.
    //!
    //! Sets one past the last string in a StringRange object to be \p lst.
    //!
    //! \param lst one past the last string.
    //!
    //! \returns A reference to \c *this.
    //!
    //! \throws LibsemigroupsException if \p lst contains letters not belonging
    //! to alphabet().
    //!
    //! \note The behaviour of this function is not exactly the same as
    //! `WordRange::last(word_type const&)`. That function will not throw if a
    //! word contains letters not in the alphabet.
    //!
    //! \sa \ref max
    StringRange& last(std::string const& lst) {
      _word_range.last(_to_word(lst));
      _current_valid &= _word_range.valid();
      return *this;
    }

    //! \brief The current one past the last string in the range.
    //!
    //! Returns one past the last string in a StringRange object.
    //!
    //! \returns A \ref std::string by value.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \sa \ref max
    [[nodiscard]] std::string last() const noexcept {
      return _to_string(_word_range.last());
    }

    //! \brief Set the order of the strings in the range.
    //!
    //! Sets the order of the strings in a StringRange object to \p val.
    //!
    //! \param val the order.
    //!
    //! \returns A reference to \c *this.
    //!
    //! \throws LibsemigroupsException if val is not Order::lenlex or
    //! Order::lex.
    StringRange& order(Order val) {
      _word_range.order(val);
      _current_valid &= _word_range.valid();
      return *this;
    }

    //! \brief The current order of the strings in the range.
    //!
    //! Returns the current order of the strings in a StringRange object.
    //!
    //! \returns A value of type \ref Order.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] Order order() const noexcept {
      return _word_range.order();
    }

    //! \brief Set an upper bound for the length of a string in the range.
    //!
    //! Sets an upper bound for the length of a string in a StringRange object.
    //! This setting is only used if order() is Order::lex.
    //!
    //! \param n the upper bound.
    //!
    //! \returns A reference to \c *this.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    StringRange& upper_bound(size_type n) {
      _word_range.upper_bound(n);
      _current_valid &= _word_range.valid();
      return *this;
    }

    //! \brief The current upper bound on the length of a string in the range.
    //!
    //! Returns the current upper bound on the length of a string in a
    //! StringRange object. This setting is only used if order() is Order::lex.
    //!
    //! \returns A value of type \ref size_type.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] size_type upper_bound() const noexcept {
      return _word_range.upper_bound();
    }

    //! \brief Set the first string in the range by length.
    //!
    //! Sets the first string in a StringRange object to be `pow("a", val)` (the
    //! string consisting of \p val letters equal to \c "a").
    //!
    //! \param val the exponent.
    //!
    //! \returns A reference to \c *this.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    StringRange& min(size_type val) {
      _word_range.min(val);
      _current_valid &= _word_range.valid();
      return *this;
    }

    // No corresponding getter for min, because what would it mean? Could be the
    // length of first(), but that doesn't correspond well to what happens with
    // the setter.

    //! \brief Set one past the last string in the range by length.
    //!
    //! Sets one past the last string in a StringRange object to be
    //! \f$\alpha^n\f$ where \f$\alpha\f$ is the first letter of
    //! `alphabet()` (or `"a"` if the alphabet is empty) and
    //! \f$n\f$ corresponds to \p val.
    //!
    //! \param val the exponent.
    //!
    //! \returns A reference to \c *this.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    StringRange& max(size_type val) {
      _word_range.max(val);
      _current_valid &= _word_range.valid();
      return *this;
    }

    //! \brief Returns an input iterator pointing to the first string in the
    //! range.
    //!
    //! This function returns an input iterator pointing to the first string in
    //! a StringRange object.
    //!
    //! \returns An input iterator.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \note The return type of \ref end might be different from the return
    //! type of \ref begin.
    //!
    //! \sa \ref end.
    // REQUIRED so that we can use StringRange in range based loops
    auto begin() const noexcept {
      return rx::begin(*this);
    }

    //! \brief Returns an input iterator pointing one beyond the last string in
    //! the range.
    //!
    //! This function returns an input iterator pointing one beyond the last
    //! string in a StringRange object.
    //!
    //! \returns An input iterator.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \note The return type of \ref end might be different from the return
    //! type of \ref begin.
    //!
    //! \sa \ref begin.
    // REQUIRED so that we can use StringRange in range based loops
    auto end() const noexcept {
      return rx::end(*this);
    }
  };

  //! \ingroup words_group
  //!
  //! \brief Return a human readable representation of a StringRange object.
  //!
  //! Return a human readable representation of a StringRange object.
  //!
  //! \param sr the StringRange object.
  //! \param max_width the maximum width of the returned string (default:
  //! \c 72).
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  [[nodiscard]] std::string to_human_readable_repr(StringRange const& sr,
                                                   size_t max_width = 72);

}  // namespace libsemigroups

#endif  // LIBSEMIGROUPS_WORD_RANGE_CLASS_HPP_
