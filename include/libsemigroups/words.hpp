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
// * doc
// * code coverage
// * iwyu
// * nodiscard

#ifndef LIBSEMIGROUPS_WORDS_HPP_
#define LIBSEMIGROUPS_WORDS_HPP_

#include <array>             // for array
#include <cstddef>           // for size_t
#include <cstdint>           // for uint64_t, uint8_t
#include <initializer_list>  // for initializer_list
#include <iterator>          // for distance
#include <string>            // for basic_string
#include <type_traits>       // for decay_t
#include <utility>           // for forward
#include <variant>           // for visit, operator==
#include <vector>            // for vector, operator==

#include "constants.hpp"  // for UNDEFINED
#include "exception.hpp"  // for LibsemigroupsException
#include "types.hpp"      // for word_type

#include "detail/iterator.hpp"  // for default_postfix_increment

#include <rx/ranges.hpp>  // for begin, end

namespace libsemigroups {

  enum class Order : uint8_t;  // forward decl

  ////////////////////////////////////////////////////////////////////////
  // Words
  ////////////////////////////////////////////////////////////////////////

  //! \defgroup Words
  //! This page contains details of the functionality in \c libsemigroups
  //! related to generating words in a given range and in a given order.

  //! \ingroup Words
  //! \brief Returns the number of words over an alphabet with a given number of
  //! letters with length in a specified range.
  //!
  //! \param n the number of letters in the alphabet
  //! \param min the minimum length of a word
  //! \param max the maximum length of a word
  //!
  //! \returns
  //! A value of type \c uint64_t.
  //!
  //! \exception
  //! \no_libsemigroups_except
  //!
  //! \warning If the number of words exceeds 2 ^ 64 - 1, then
  //! the return value of this function will not be correct.
  [[nodiscard]] uint64_t number_of_words(size_t n, size_t min, size_t max);

  //! \ingroup Words
  //! \brief Returns a random word.
  //!
  //! \param length the length of the word
  //! \param nr_letters the size of the alphabet
  //!
  //! \returns
  //! A random word on \f$\{0, \ldots, n - 1\}\f$ of length \p length where
  //! \f$n\f$ is \p nr_letters.
  //!
  //! \throws LibsemigroupsException if \p nr_letters is \c 0.
  //!
  //! \sa \ref random_string
  [[nodiscard]] word_type random_word(size_t length, size_t nr_letters);

#ifndef DOXYGEN_SHOULD_SKIP_THIS

  class const_wilo_iterator {
   public:
    using size_type         = typename std::vector<word_type>::size_type;
    using difference_type   = typename std::vector<word_type>::difference_type;
    using const_pointer     = typename std::vector<word_type>::const_pointer;
    using pointer           = typename std::vector<word_type>::pointer;
    using const_reference   = typename std::vector<word_type>::const_reference;
    using reference         = typename std::vector<word_type>::reference;
    using value_type        = word_type;
    using iterator_category = std::forward_iterator_tag;

   private:
    word_type   _current;
    size_type   _index;
    letter_type _letter;
    size_type   _upper_bound;
    word_type   _last;
    size_type   _number_letters;

   public:
    const_wilo_iterator() noexcept;
    const_wilo_iterator(const_wilo_iterator const&);
    const_wilo_iterator(const_wilo_iterator&&) noexcept;
    const_wilo_iterator& operator=(const_wilo_iterator const&);
    const_wilo_iterator& operator=(const_wilo_iterator&&) noexcept;
    ~const_wilo_iterator();

    const_wilo_iterator(size_type   n,
                        size_type   upper_bound,
                        word_type&& first,
                        word_type&& last);

    [[nodiscard]] bool
    operator==(const_wilo_iterator const& that) const noexcept {
      return _index == that._index;
    }

    [[nodiscard]] bool
    operator!=(const_wilo_iterator const& that) const noexcept {
      return !(this->operator==(that));
    }

    [[nodiscard]] const_reference operator*() const noexcept {
      return _current;
    }

    [[nodiscard]] const_pointer operator->() const noexcept {
      return &_current;
    }

    // prefix
    const_wilo_iterator const& operator++() noexcept;

    // postfix
    const_wilo_iterator operator++(int) noexcept {
      return detail::default_postfix_increment<const_wilo_iterator>(*this);
    }

    void swap(const_wilo_iterator& that) noexcept;
  };

  inline void swap(const_wilo_iterator& x, const_wilo_iterator& y) noexcept {
    x.swap(y);
  }

#endif  // DOXYGEN_SHOULD_SKIP_THIS

  //! \ingroup Words
  //! \brief Returns a forward iterator pointing to the 3rd parameter \p first.
  //!
  //! If incremented, the iterator will point to the next least lexicographic
  //! word after \p w over an \p n letter alphabet with length less than \p
  //! upper_bound.  Iterators of the type returned by this function are equal
  //! whenever they are obtained by advancing the return value of any call to
  //! \c cbegin_wilo by the same amount, or they are both obtained by any call
  //! to \c cend_wilo.
  //!
  //! \param n the number of letters in the alphabet;
  //! \param upper_bound   only words of length less than this value are
  //! considered;
  //! \param first the starting point for the iteration;
  //! \param last the value one past the end of the last value in the
  //! iteration.
  //!
  //! \returns An iterator of type \c const_wilo_iterator.
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
  [[nodiscard]] const_wilo_iterator cbegin_wilo(size_t      n,
                                                size_t      upper_bound,
                                                word_type&& first,
                                                word_type&& last);

  //! \ingroup Words
  //! \brief Returns a forward iterator pointing to the 3rd parameter \p first.
  //! \copydoc cbegin_wilo(size_t, size_t, word_type&&, word_type&&)
  [[nodiscard]] const_wilo_iterator cbegin_wilo(size_t           n,
                                                size_t           upper_bound,
                                                word_type const& first,
                                                word_type const& last);

  //! \ingroup Words
  //! \brief Returns a forward iterator pointing to one after the end of the
  //! range from \p first to \p last.
  //!
  //! The iterator returned by this function is still dereferenceable and
  //! incrementable, but does not point to a word in the correct range.
  //!
  //! \sa cbegin_wilo
  [[nodiscard]] const_wilo_iterator
  cend_wilo(size_t n, size_t upper_bound, word_type&& first, word_type&& last);

  //! \ingroup Words
  //! \brief Returns a forward iterator pointing to one after the end of the
  //! range from \p first to \p last.
  //! \copydoc cend_wilo(size_t, size_t, word_type&&, word_type&&)
  [[nodiscard]] const_wilo_iterator cend_wilo(size_t           n,
                                              size_t           upper_bound,
                                              word_type const& first,
                                              word_type const& last);

#ifndef DOXYGEN_SHOULD_SKIP_THIS
  class const_wislo_iterator {
   public:
    using size_type         = typename std::vector<word_type>::size_type;
    using difference_type   = typename std::vector<word_type>::difference_type;
    using const_pointer     = typename std::vector<word_type>::const_pointer;
    using pointer           = typename std::vector<word_type>::pointer;
    using const_reference   = typename std::vector<word_type>::const_reference;
    using reference         = typename std::vector<word_type>::reference;
    using value_type        = word_type;
    using iterator_category = std::forward_iterator_tag;

   private:
    word_type _current;
    size_type _index;
    word_type _last;
    size_type _number_letters;

   public:
    const_wislo_iterator() noexcept;
    const_wislo_iterator(const_wislo_iterator const&);
    const_wislo_iterator(const_wislo_iterator&&) noexcept;
    const_wislo_iterator& operator=(const_wislo_iterator const&);
    const_wislo_iterator& operator=(const_wislo_iterator&&) noexcept;

    const_wislo_iterator(size_type n, word_type&& first, word_type&& last);

    ~const_wislo_iterator();

    [[nodiscard]] bool
    operator==(const_wislo_iterator const& that) const noexcept {
      return _index == that._index;
    }

    [[nodiscard]] bool
    operator!=(const_wislo_iterator const& that) const noexcept {
      return !(this->operator==(that));
    }

    [[nodiscard]] const_reference operator*() const noexcept {
      return _current;
    }

    [[nodiscard]] const_pointer operator->() const noexcept {
      return &_current;
    }

    // prefix
    const_wislo_iterator const& operator++() noexcept;

    // postfix
    const_wislo_iterator operator++(int) noexcept {
      return detail::default_postfix_increment<const_wislo_iterator>(*this);
    }

    void swap(const_wislo_iterator& that) noexcept;
  };

  inline void swap(const_wislo_iterator& x, const_wislo_iterator& y) noexcept {
    x.swap(y);
  }

#endif  // DOXYGEN_SHOULD_SKIP_THIS

  //! \ingroup Words
  //! \brief Returns a forward iterator pointing to the 2nd parameter \p first.
  //!
  //! If incremented, the iterator will point to the next least short-lex
  //! word after \p w over an \p n letter alphabet. Iterators of the type
  //! returned by this function are equal whenever they are obtained by
  //! advancing the return value of any call to \c cbegin_wislo by the same
  //! amount, or they are both obtained by any call to \c cend_wislo.
  //!
  //! \param n the number of letters in the alphabet;
  //! \param first the starting point for the iteration;
  //! \param last the ending point for the iteration.
  //!
  //! \returns An iterator of type \c const_wislo_iterator.
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
  [[nodiscard]] const_wislo_iterator cbegin_wislo(size_t      n,
                                                  word_type&& first,
                                                  word_type&& last);

  //! \ingroup Words
  //! \brief Returns a forward iterator pointing to the 2nd parameter \p first.
  //! \copydoc cbegin_wislo(size_t const, word_type&&, word_type&&)
  [[nodiscard]] const_wislo_iterator cbegin_wislo(size_t           n,
                                                  word_type const& first,
                                                  word_type const& last);

  //! \ingroup Words
  //! \brief Returns a forward iterator pointing to one after the end of the
  //! range from \p first to \p last.
  //!
  //! The iterator returned by this is still dereferenceable and incrementable,
  //! but does not point to a word in the correct range.
  //!
  //! \sa cbegin_wislo
  [[nodiscard]] const_wislo_iterator cend_wislo(size_t      n,
                                                word_type&& first,
                                                word_type&& last);

  //! \ingroup Words
  //! \brief Returns a forward iterator pointing to one after the end of the
  //! range from \p first to \p last.
  //! \copydoc cend_wislo(size_t const, word_type&&, word_type&&)
  [[nodiscard]] const_wislo_iterator cend_wislo(size_t           n,
                                                word_type const& first,
                                                word_type const& last);

  //! \ingroup Words
  //! \brief Class for generating words in a given range and in a particular
  //! order.
  //!
  //! Defined in \c words.hpp\c .
  //!
  //! This class implements a range object for the lower level
  //! functions \ref cbegin_wislo and \ref cbegin_wilo. The purpose of this
  //! class is to provide a more user-friendly interface with \ref cbegin_wislo
  //! and \ref cbegin_wilo.
  //!
  //! \note
  //! There is a small overhead to using a Words object rather than using \ref
  //! cbegin_wislo or \ref cbegin_wilo directly.
  //!
  //! The order and range of the words in a Words instance can be set using the
  //! member functions:
  //! * \ref order
  //! * \ref letters
  //! * \ref min
  //! * \ref max
  //! * \ref first
  //! * \ref last
  //!
  //! \par Example
  //! \code
  //! Words words;
  //! words.order(Order::shortlex) // words in shortlex order
  //!      .letters(2)             // on 2 letters
  //!      .min(1)                 // of length in the range from 1
  //!      .max(5);                // to 5
  //! \endcode
  class Words {
   public:
    //! Alias for the size type.
    using size_type = typename std::vector<word_type>::size_type;

    //! The type of the output of a Words object.
    using output_type = word_type const&;

   private:
    using const_iterator
        = std::variant<const_wilo_iterator, const_wislo_iterator>;

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
    //! Returns the current word in a Words object.
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
      return std::visit([](auto& it) -> auto const& { return *it; }, _current);
    }

    //! \brief Advance to the next value.
    //!
    //! Advances a Words object to the next value (if any).
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
    //! Returns \c true if a Words object is exhausted, and \c false if not.
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
    //! Returns the number of words in a Words object if order() is
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
    //! Returns the number of words in a Words object. If order() is
    //! Order::shortlex, then size_hint() is used. If order() is not
    //! Order::shortlex, then a copy of the range may have to be looped over in
    //! order to find the return value of this function.
    //!
    //! \returns A value of type \c size_t.
    //!
    //! \exception
    //! \noexcept
    [[nodiscard]] size_t count() const noexcept;

    //! Value indicating that the range is finite.
    static constexpr bool is_finite = true;  // This may not always be true

    //! Value indicating that if get() is called twice on a Words object that
    //! is not changed between the two calls, then the return value of get() is
    //! the same both times.
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
    Words() {
      init();
    }

    //! \brief Initialize an existing Words object.
    //!
    //! This function puts a Words object back into the same state as if it had
    //! been newly default constructed.
    //!
    //! \returns A reference to \c *this.
    //!
    //! \exception
    //! \no_libsemigroups_except
    Words& init();

    //! \brief Default copy constructor.
    Words(Words const&);

    //! \brief Default move constructor.
    Words(Words&&);

    //! \brief Default copy assignment operator.
    Words& operator=(Words const&);

    //! \brief Default move assignment operator.
    Words& operator=(Words&&);

    //! \brief Default destructor.
    ~Words();

    //! \brief Set the number of letters in the alphabet.
    //!
    //! Sets the number of letters in a Words object to \p n.
    //!
    //! \param n the number of letters.
    //!
    //! \returns A reference to \c *this.
    //!
    //! \exception
    //! \noexcept
    Words& letters(size_type n) noexcept {
      _current_valid &= (n == _number_of_letters);
      _number_of_letters = n;
      return *this;
    }

    //! \brief The current number of letters in the alphabet.
    //!
    //! Returns the current number of letters in a Words object.
    //!
    //! \returns A value of type \ref size_type.
    //!
    //! \exception
    //! \noexcept
    [[nodiscard]] size_type letters() const noexcept {
      return _number_of_letters;
    }

    //! \brief Set the first word in the range.
    //!
    //! Sets the first word in a Words object to be \p frst. This function
    //! performs no checks on its arguments. If \p frst contains letters greater
    //! than letters(), then the Words object will be empty. Similarly, if
    //! first() is greater than last() with respect to order(), then the object
    //! will be empty.
    //!
    //! \param frst the first word.
    //!
    //! \returns A reference to \c *this.
    //!
    //! \sa \ref min
    Words& first(word_type const& frst) {
      _current_valid &= (frst == _first);
      _first = frst;
      return *this;
    }

    //! \brief The current first word in the range.
    //!
    //! Returns the first word in a Words object.
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
    //! Sets one past the last word in a Words object to be \p lst. This
    //! function performs no checks on its arguments. If \p lst contains
    //! letters greater than letters(), then the Words object will be empty.
    //!
    //! \param lst the first word.
    //!
    //! \returns A reference to \c *this.
    //!
    //! \sa \ref max
    Words& last(word_type const& lst) {
      _current_valid &= (lst == _last);
      _last = lst;
      return *this;
    }

    //! \brief The current one past the last word in the range.
    //!
    //! Returns the last word in a Words object.
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
    //! Sets the order of the words in a Words object to \p val.
    //!
    //! \param val the order.
    //!
    //! \returns A reference to \c *this.
    //!
    //! \throws LibsemigroupsException if val is not Order::shortlex or
    //! Order::lex.
    Words& order(Order val);

    //! \brief The current order of the words in the range.
    //!
    //! Returns the current order of the words in a Words object.
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
    //! Sets an upper bound for the length of a word in a Words object.
    //! This setting is only used if order() is Order::lex.
    //!
    //! \param val the upper bound.
    //!
    //! \returns A reference to \c *this.
    //!
    //! \exception
    //! \no_libsemigroups_except
    Words& upper_bound(size_type n) {
      _current_valid &= (n == _upper_bound);
      _upper_bound = n;
      return *this;
    }

    //! \brief The current upper bound on the length of a word in the range.
    //!
    //! Returns the current upper bound on the length of a word in a Words
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
    //! Sets the first word in a Words object to be \c pow(0_w, val) (the word
    //! consisting of \p val letters equal to \c 0).
    //!
    //! \param val the exponent.
    //!
    //! \returns A reference to \c *this.
    //!
    //! \exception
    //! \no_libsemigroups_except
    Words& min(size_type val) {
      first(word_type(val, 0));
      return *this;
    }

    // No corresponding getter for min, because what would it mean? Could be the
    // length of first(), but that doesn't correspond well to what happens with
    // the setter.

    //! \brief Set one past the last word in the range by length.
    //!
    //! Sets one past the last word in a Words object to be \c pow(0_w, val)
    //! (the word consisting of \p val letters equal to \c 0).
    //!
    //! \param val the exponent.
    //!
    //! \returns A reference to \c *this.
    //!
    //! \exception
    //! \no_libsemigroups_except
    Words& max(size_type val) {
      last(word_type(val, 0));
      return *this;
    }

    //! \brief Returns an input iterator pointing to the first word in the
    //! range.
    //!
    //! This function returns an input iterator pointing to the first word in
    //! a Words object.
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
    //! word in a Words object.
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

    // TODO impl swap for consistency with Strings
  };

  ////////////////////////////////////////////////////////////////////////
  // Strings -> Words
  ////////////////////////////////////////////////////////////////////////

  //! \ingroup Words
  //! \brief Returns the index of a character in human readable order.
  //!
  //! This function is the inverse of \ref human_readable_char, see the
  //! documentation of that function for more details.
  //!
  //! \param c character whose index is sought.
  //!
  //! \exception
  //! \no_libsemigroups_except
  //!
  //! \returns A value of type \ref letter_type.
  [[nodiscard]] letter_type human_readable_index(char c);

  //! \ingroup Words
  //! \brief Convert a string to a word_type.
  //!
  //! This function converts its second argument \p input into a word_type and
  //! stores the result in the first argument \p output. The characters of \p
  //! input are converted using \ref human_readable_index, so that \c 'a' is
  //! mapped to \c 0, 'b' to \c 1, and so on.
  //!
  //! The contents of the first argument \p output, if any, is removed.
  //!
  //! \param output word to hold the result
  //! \param input the string to convert
  //!
  //! \exception
  //! \no_libsemigroups_except
  //!
  //! \sa
  //! * human_readable_index
  //! * ToWord
  //! * literals::operator""_w
  void to_word(std::string const& input, word_type& output);

  //! \ingroup Words
  //! \brief Convert a string to a word_type.
  //!
  //! This function converts its argument \p s into a word_type. The characters
  //! of \p s are converted using \ref human_readable_index, so that \c 'a' is
  //! mapped to \c 0, 'b' to \c 1, and so on.
  //!
  //! The contents of the first argument \p w, if any, is removed.
  //!
  //! \param s the string to convert
  //! \returns A value of type \ref word_type.
  //!
  //! \exception
  //! \no_libsemigroups_except
  //!
  //! \sa
  //! * human_readable_index
  //! * ToWord
  //! * literals::operator""_w
  [[nodiscard]] word_type to_word(std::string const& s);

  //! \ingroup Words
  //! \brief Class for converting strings to word_type with specified alphabet.
  //!
  //! Defined in \c words.hpp\c .
  //!
  //! An instance of this class can be used like to_word but where the
  //! characters in the string are converted to integers according to their
  //! position in alphabet used to construct a ToWord instance.
  //!
  //! \par Example
  //! \code
  //! ToWord toword("bac");
  //! toword("bac");        // returns {0, 1, 2}
  //! toword("bababbbcbc"); // returns { 0, 1, 0, 1, 0, 0, 0, 2, 0, 2}
  //! \endcode
  // TODO a version that takes a word_type, so that we can permute the letters
  // in a word
  // TODO a version of ToWords that is similar, i.e. takes a word_type and so
  // can be used to change the letters in a word.
  class ToWord {
   public:
    //! \brief Default constructor.
    //!
    //! Constructs an empty object with no alphabet set.
    ToWord() : _lookup() {
      init();
    }

    //! \brief Initialize an existing ToWord object.
    //!
    //! This function puts a ToWord object back into the same state as if it had
    //! been newly default constructed.
    //!
    //! \returns A reference to \c *this.
    //!
    //! \exception
    //! \no_libsemigroups_except
    ToWord& init() {
      _lookup.fill(UNDEFINED);
      _lookup.back() = 0;
      return *this;
    }

    //! \brief Construct with given alphabet.
    //!
    //! \param alphabet the alphabet
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
    //! This function converts its first argument \p input into a word_type and
    //! stores the result in the second argument \p output. The characters of \p
    //! input are converted using the alphabet used to construct the object or
    //! set via init().
    //!
    //! The contents of the first argument \p output, if any, is removed.
    //!
    //! \param input the string to convert
    //! \param output word to hold the result
    //!
    //! \throws LibsemigroupsException
    //! If \p input contains any letters that letters that do not correspond to
    //! letters of the alphabet used to define an instance of ToWord.
    //!
    //! \sa
    //! * to_word
    //! * to_string
    //! * literals::operator""_w
    void operator()(std::string const& input, word_type& output) const;

    //! \brief Convert a string to a word_type.
    //!
    //! This function converts its argument \p input into a word_type The
    //! characters of \p input are converted using the alphabet used to
    //! construct the object or set via init().
    //!
    //! \param input the string to convert
    //!
    //! \exception
    //! \no_libsemigroups_except
    //!
    //! \warning No checks on the arguments are performed, if \p input contains
    //! letters that do not correspond to letters of the alphabet used to define
    //! an instance of ToWord, then these letters are mapped to UNDEFINED.
    //!
    //! \sa
    //! * to_word
    //! * to_string
    //! * literals::operator""_w
    [[nodiscard]] word_type operator()(std::string const& input) const;

   private:
    std::array<letter_type, 256> _lookup;
  };

  ////////////////////////////////////////////////////////////////////////
  // Words -> Strings
  ////////////////////////////////////////////////////////////////////////

  //! \ingroup Words
  //! \brief Returns a character by index in human readable order.
  //!
  //! This function exists to map the numbers \c 0 to \c 254 to the possible
  //! values of a \c char, in such a way that the first characters are \c
  //! a-zA-Z0-9 The ascii ranges for these characters are: \f$[97, 123)\f$,
  //! \f$[65, 91)\f$, \f$[48, 58)\f$ so the remaining range of chars that are
  //! appended to the end after these chars are \f$[0,48)\f$, \f$[58, 65)\f$,
  //! \f$[91, 97)\f$, \f$[123, 255)\f$.
  //!
  //! This function is the inverse of \ref human_readable_letter.
  //!
  //! \param i the index of the character.
  //!
  //! \returns A value of type \c char.
  //!
  //! \throws LibsemigroupsException if \p i exceeds the number of characters.
  [[nodiscard]] char human_readable_char(size_t i);

  //! \ingroup Words
  //! \brief Convert a word_type to a string.
  //!
  //! This function converts its the word_type pointed to by the 2nd and 3rd
  //! arguments \p first and \p last into a \ref std::string, and stores the
  //! result in the final argument \p output. The characters of \p input are
  //! converted using \ref human_readable_index, so that \c i is mapped to \p
  //! alphabet[i] for each letter \c i.
  //!
  //! The contents of the first argument \p output, if any, is removed.
  //!
  //! \tparam Iterator the type of the 2nd and 3rd arguments
  //!
  //! \param alphabet the alphabet to use for the conversion
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
  //! * literals::operator""_w
  template <typename Iterator>
  void to_string(std::string const& alphabet,
                 Iterator           input_first,
                 Iterator           input_last,
                 std::string&       output) {
    output.resize(std::distance(input_first, input_last));
    size_t i = 0;
    for (auto it = input_first; it != input_last; ++it) {
      output[i++] = alphabet[*it];
    }
  }

  //! \ingroup Words
  //! \brief Convert a word_type to a string.
  //!
  //! See to_string(std::string const&, Iterator, Iterator, std::string&)
  static inline void to_string(std::string const& alphabet,
                               word_type const&   input,
                               std::string&       output) {
    return to_string(alphabet, input.cbegin(), input.cend(), output);
  }

  //! \ingroup Words
  //! \brief Convert a word_type to a string.
  //!
  //! See to_string(std::string const&, Iterator, Iterator, std::string&)
  //! the difference is that this function returns a new string.
  template <typename Iterator>
  [[nodiscard]] std::string to_string(std::string const& alphabet,
                                      Iterator           first,
                                      Iterator           last) {
    std::string output;
    to_string(alphabet, first, last, output);
    return output;
  }

  //! \ingroup Words
  //! \brief Convert a word_type to a string.
  //!
  //! See to_string(std::string const&, Iterator, Iterator, std::string&)
  //! the difference is that this function returns a new string.
  [[nodiscard]] static inline std::string to_string(std::string const& alphabet,
                                                    word_type const&   input) {
    return to_string(alphabet, input.cbegin(), input.cend());
  }

  // TODO single arg to_string(word_type const&).
  // TODO ToString object that stores the alphabet

  ////////////////////////////////////////////////////////////////////////
  // Strings
  ////////////////////////////////////////////////////////////////////////

  //! \ingroup Words
  //! \brief Class for generating strings in a given range and in a particular
  //! order.
  //!
  //! Defined in \c words.hpp.
  //!
  //! This class implements a range object for strings and produces the same
  //! output as `Words() | ToStrings("ab")`, but is more convenient in some
  //! cases.
  //!
  //!\note There is a small overhead to using a Strings object rather than
  //! using \ref cbegin_wislo or \ref cbegin_wilo directly.
  //!
  //! The order and range of the words in a Strings instance can be set using
  //! the member functions:
  //! * \ref order
  //! * \ref letters
  //! * \ref min
  //! * \ref max
  //! * \ref first
  //! * \ref last
  //!
  //! \par Example
  //! \code
  //! Strings strings;
  //! strings.order(Order::shortlex) // strings in shortlex order
  //!        .letters(2)             // on 2 letters
  //!        .min(1)                 // of length in the range from 1
  //!        .max(5);                // to 5
  //! \endcode
  //!
  //! \sa Words
  // This can in many places be replaced by "Words | ToStrings" but this
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
    Words               _words;

    void init_current() const {
      if (!_current_valid) {
        _current = to_string(_letters, _words.get());
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
      _current_valid = false;
      _words.next();
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
      return _words.at_end();
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
      return _words.size_hint();
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
      return _words.count();
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

    //! \brief Initialize an existing Words object.
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
    //! \exception
    //! \noexcept
    // TODO rename to alphabet for consistence with Presentation
    Strings& letters(std::string const& x);

    //! \brief The current alphabet.
    //!
    //! Returns the current alphabet in a Strings object.
    //!
    //! \returns A value of type \ref std::string.
    //!
    //! \exception
    //! \noexcept
    // TODO rename to alphabet for consistence with Presentation
    [[nodiscard]] std::string const& letters() const noexcept {
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
    //! \note Unlike Words::first, this function will throw if \p frst
    //! contains letters not belonging to alphabet().
    Strings& first(std::string const& frst) {
      _current_valid = false;
      _words.first(_to_word(frst));
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
      return to_string(_letters, _words.first());
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
    //! \note Unlike Words::last, this function will throw if \p lst
    //! contains letters not belonging to alphabet().
    Strings& last(std::string const& lst) {
      _current_valid = false;
      _words.last(_to_word(lst));
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
      return to_string(_letters, _words.last());
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
      _current_valid = false;
      _words.order(val);
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
      return _words.order();
    }

    //! \brief Set an upper bound for the length of a string in the range.
    //!
    //! Sets an upper bound for the length of a string in a Strings object.
    //! This setting is only used if order() is Order::lex.
    //!
    //! \param val the upper bound.
    //!
    //! \returns A reference to \c *this.
    //!
    //! \exception
    //! \no_libsemigroups_except
    Strings& upper_bound(size_type n) {
      _current_valid = false;
      _words.upper_bound(n);
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
      return _words.upper_bound();
    }

    //! \brief Set the first string in the range by length.
    //!
    //! Sets the first string in a Strings object to be \c pow("a", val) (the
    //! string consisting of \p val letters equal to \c "a").
    //!
    //! \param val the exponent.
    //!
    //! \returns A reference to \c *this.
    //!
    //! \exception
    //! \no_libsemigroups_except
    Strings& min(size_type val) {
      _current_valid = false;
      _words.min(val);
      return *this;
    }

    // No corresponding getter for min, because what would it mean? Could be the
    // length of first(), but that doesn't correspond well to what happens with
    // the setter.

    //! \brief Set one past the last string in the range by length.
    //!
    //! Sets one past the last string in a Strings object to be \c pow("a", val)
    //! (the string consisting of \p val letters equal to \c "a").
    //!
    //! \param val the exponent.
    //!
    //! \returns A reference to \c *this.
    //!
    //! \exception
    //! \no_libsemigroups_except
    Strings& max(size_type val) {
      _current_valid = false;
      _words.max(val);
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
    auto begin() const {
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
    auto end() const {
      return rx::end(*this);
    }

    // TODO doc
    // HERE next do code coverage for Strings
    void swap(Strings& that) noexcept;
  };

  ////////////////////////////////////////////////////////////////////////
  // Ranges
  ////////////////////////////////////////////////////////////////////////

  // The next class is a custom combinator for rx::ranges to convert the output
  // of a Strings object into words
  template <typename = void>
  struct ToWords {
    ToWords() = default;

    explicit ToWords(std::string const& lttrs) : _letters(lttrs) {}
    explicit ToWords(std::string&& lttrs) : _letters(std::move(lttrs)) {}
    explicit ToWords(char const* lttrs) : _letters(lttrs) {}

    std::string _letters;

    template <typename InputRange>
    struct Range {
      using output_type = word_type;

      static constexpr bool is_finite     = true;
      static constexpr bool is_idempotent = true;

      InputRange _input;
      ToWord     _to_word;

      explicit constexpr Range(InputRange input, ToWords t_wrds) noexcept
          : _input(input), _to_word(t_wrds._letters) {}

      explicit constexpr Range(Strings const& input)
          : _input(input), _to_word(input.letters()) {}

      [[nodiscard]] output_type get() const noexcept {
        return _to_word(_input.get());
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

    [[nodiscard]] constexpr auto operator()(Strings const& input) const {
      return Range<Strings>(input);
    }

    template <typename InputRange,
              typename = std::enable_if_t<
                  !std::is_same_v<std::decay_t<InputRange>, Strings>>>
    [[nodiscard]] constexpr auto operator()(InputRange&& input) const {
      using Inner = rx::get_range_type_t<InputRange>;
      return Range<Inner>(std::forward<InputRange>(input), *this);
    }
  };

  // The next class is a custom combinator for rx::ranges to convert the output
  // of a Words object into strings
  template <typename = void>
  struct ToStrings {
    explicit ToStrings(std::string const& lttrs) : _letters(lttrs) {}
    explicit ToStrings(std::string&& lttrs) : _letters(std::move(lttrs)) {}
    explicit ToStrings(char const* lttrs) : _letters(lttrs) {}

    std::string _letters;

    template <typename InputRange>
    struct Range {
      using output_type = std::string;

      static constexpr bool is_finite     = true;
      static constexpr bool is_idempotent = true;

      InputRange _input;
      // TODO reference?
      ToStrings _to_string;

      constexpr Range(InputRange&& input, ToStrings&& t_strng) noexcept
          : _input(std::move(input)), _to_string(std::move(t_strng)) {}

      constexpr Range(InputRange const& input,
                      ToStrings const&  t_strng) noexcept
          : _input(input), _to_string(t_strng) {}

      [[nodiscard]] output_type get() const noexcept {
        return to_string(_to_string._letters, _input.get());
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

      [[nodiscard]] constexpr auto begin() const noexcept {
        return rx::begin(*this);
      }

      [[nodiscard]] constexpr auto end() const noexcept {
        return rx::end(*this);
      }
    };

    template <typename InputRange>
    [[nodiscard]] constexpr auto operator()(InputRange&& input) const {
      using Inner = rx::get_range_type_t<InputRange>;
      return Range<Inner>(std::forward<InputRange>(input), *this);
    }
  };

  ////////////////////////////////////////////////////////////////////////
  // Literals
  ////////////////////////////////////////////////////////////////////////

  namespace literals {
    //! Literal for defining \ref word_type over integers less than 10.
    //!
    //! This operator provides a convenient brief means of constructing a  \ref
    //! word_type from an sequence of literal integer digits or a string. For
    //! example, \c 0123_w produces the same output as `word_type({0, 1, 2,
    //! 3})`.
    //!
    //! There are some gotchas and this operator should be used with some care:
    //!
    //! * the parameter \p w must only consist of the integers \f$\{0, \ldots,
    //! 9\}\f$. For example, there are no guarantees about the value of
    //! `"abc"_w`.
    //! * if \p w starts with \c 0 and is follows by a value greater than \c 7,
    //! then it is necessary to enclose \p w in quotes. For example, \c 08_w
    //! will not compile because it is interpreted as an invalid octal. However
    //! `"08"_w` behaves as expected.
    //!
    //! \param w the letters of the word
    //! \param n the length of \p w (defaults to the length of \p w)
    //!
    //! \warning
    //! This operator performs no checks on its arguments whatsoever.
    //!
    //! \returns A value of type \ref word_type.
    //!
    //! \exception \no_libsemigroups_except
    word_type operator"" _w(const char* w, size_t n);

    //! \copydoc operator""_w
    word_type operator"" _w(const char* w);

    // TODO doc
    std::string operator"" _p(const char* w, size_t n);

    // TODO doc
    std::string operator"" _p(const char* w);
  }  // namespace literals

  ////////////////////////////////////////////////////////////////////////

  namespace words {

    ////////////////////////////////////////////////////////////////////////
    // Operators - declarations
    ////////////////////////////////////////////////////////////////////////

    //! \anchor operator_plus
    //! Concatenate two words or strings.
    //!
    //! Returns the concatenation of \c u and \c w.
    //!
    //! \param u a word or string
    //! \param w a word or string
    //!
    //! \returns A word_type or string
    //!
    //! \exception
    //! \noexcept
    word_type operator+(word_type const& u, word_type const& w);

    //! See \ref operator_plus "operator+".
    word_type operator+(word_type const& u, letter_type w);

    //! See \ref operator_plus "operator+".
    word_type operator+(letter_type w, word_type const& u);

    //! Concatenate a word/string with another word/string in-place.
    //!
    //! Changes \c u to `u + w` in-place. See \ref operator_plus "operator+".
    //!
    //! \param u a word or string
    //! \param w a word or string
    //!
    //! \exception
    //! \noexcept
    //!
    //! \sa \ref operator_plus "operator+"
    void operator+=(word_type& u, word_type const& w);

    // TODO doc
    inline void operator+=(word_type& u, letter_type a) {
      u.push_back(a);
    }

    // TODO doc
    inline void operator+=(letter_type a, word_type& u) {
      u.insert(u.begin(), a);
    }

#define ENABLE_IF_IS_WORD(Word)                                 \
  typename = std::enable_if_t < std::is_same_v<Word, word_type> \
             || std::is_same_v < Word,                          \
  std::string >>

    //! Take a power of a word or string.
    //!
    //! Returns the \c nth power of the word/string given by \c w .
    //!
    //! \param w a word
    //! \param n the power
    //!
    //! \returns A word_type
    //!
    //! \exception
    //! \noexcept
    template <typename Word, ENABLE_IF_IS_WORD(Word)>
    Word pow(Word const& w, size_t n);

    //! Take a power of a word.
    //!
    //! Change the word/string \c w to its \c nth power, in-place.
    //!
    //! \param w the word
    //! \param n the power
    //!
    //! \returns
    //! (None)
    //!
    //! \exception
    //! \noexcept
    template <typename Word, ENABLE_IF_IS_WORD(Word)>
    void pow_inplace(Word& w, size_t n);

    //! Take a power of a word.
    //!
    //! Returns the \c nth power of the word corresponding to the initializer
    //! list \c ilist.
    //!
    //! \param ilist the initializer list
    //! \param n the power
    //!
    //! \returns A \ref word_type or \ref std::string
    //!
    //! \exception
    //! \noexcept
    word_type pow(std::initializer_list<size_t> ilist, size_t n);

    //! See \ref pow(Word const&, size_t)
    std::string pow(char const* w, size_t n);

    //! \anchor prod
    //! Take a product from a collection of letters.
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
    //! \param elts the ordered set
    //! \param first the first index
    //! \param last the last index
    //! \param step the step
    //!
    //! \return A \ref word_type or \ref std::string
    //!
    //! \throws LibsemigroupsException if `step = 0`
    //! \throws LibsemigroupsException if \p elts is empty, but the specified
    //! range is not
    //!
    //! \par Examples
    //! \code
    //! word_type w = 012345_w
    //! prod(w, 0, 5, 2)  // Gives the word {0, 2, 4}
    //! prod(w, 1, 9, 2)  // Gives the word {1, 3, 5, 1}
    //! prod(std::string("abcde", 4, 1, -1)  // Gives the string "edc")
    //! \endcode
    template <typename Container,
              typename Word = Container,
              ENABLE_IF_IS_WORD(Word)>
    Word prod(Container const& elts, int first, int last, int step = 1);

    //! Returns the output of \c prod where \p elts is treated as a word_type
    //! instead of a vector. See \ref prod "prod".
    template <typename Word, ENABLE_IF_IS_WORD(Word)>
    Word
    prod(std::vector<Word> const& elts, int first, int last, int step = 1) {
      return prod<std::vector<Word>, Word, void>(elts, first, last, step);
    }

    //! Returns `prod(elts, 0, last, 1)` -- see \ref prod "prod".
    template <typename Word, ENABLE_IF_IS_WORD(Word)>
    Word prod(Word const& elts, size_t last) {
      return prod(elts, 0, static_cast<int>(last), 1);
    }

    //! See \ref prod "prod".
    word_type
    prod(std::initializer_list<size_t> ilist, int first, int last, int step);

#undef ENABLE_IF_IS_WORD

    ////////////////////////////////////////////////////////////////////////
    // Operators - implementations
    ////////////////////////////////////////////////////////////////////////

    template <typename Word, typename>
    Word pow(Word const& x, size_t n) {
      Word y(x);
      pow_inplace(y, n);
      return y;
    }

    template <typename Word, typename>
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

    // Note: we could do a version of the below using insert on words, where
    // the step is +/- 1.
    template <typename Container, typename Word, typename>
    Word prod(Container const& elts, int first, int last, int step) {
      if (step == 0) {
        LIBSEMIGROUPS_EXCEPTION("the 4th argument must not be 0");
      } else if (((first < last && step > 0) || (first > last && step < 0))
                 && elts.size() == 0) {
        LIBSEMIGROUPS_EXCEPTION(
            "1st argument must be empty if the given range is not empty");
      }
      Word result;

      if (first < last) {
        if (step < 0) {
          return result;
        }
        result.reserve((last - first) / step);

        int i = first;
        for (; i < last && i < 0; i += step) {
          size_t a = ((-i / elts.size()) + 1) * elts.size() + i;
          result += elts[a];
        }
        for (; i < last; i += step) {
          result += elts[i % elts.size()];
        }
      } else {
        if (step > 0) {
          return result;
        }
        size_t steppos = static_cast<size_t>(-step);
        result.reserve((first - last) / steppos);
        int i = first;
        for (; i > last && i >= 0; i += step) {
          result += elts[i % elts.size()];
        }
        for (; i > last; i += step) {
          size_t a = ((-i / elts.size()) + 1) * elts.size() + i;
          result += elts[a];
        }
      }
      return result;
    }
  }  // namespace words
}  // namespace libsemigroups

#endif  // LIBSEMIGROUPS_WORDS_HPP_
