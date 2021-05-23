//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2020 James D. Mitchell
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

// This file contains the implementation of functionality for generating words
// in the free monoid over an alphabet with a given number of letters between a
// given pair of words.
//
// WILO = Words In Lexicographic Order

#ifndef LIBSEMIGROUPS_WILO_HPP_
#define LIBSEMIGROUPS_WILO_HPP_

#include <cstddef>   // for size_t
#include <iterator>  // for forward_iterator_tag
#include <vector>    // for vector

#include "constants.hpp"  // for UNDEFINED
#include "order.hpp"      // for lexicographical_compare
#include "types.hpp"      // for word_type

// TODO(later)
// 1. const_wilo_iterator using letters other than 0, 1, ... , n - 1. Same
//    approach as const_silo_iterator.

namespace libsemigroups {
  //! No doc
  class const_wilo_iterator {
   public:
    //! No doc
    using size_type = typename std::vector<word_type>::size_type;
    //! No doc
    using difference_type = typename std::vector<word_type>::difference_type;
    //! No doc
    using const_pointer = typename std::vector<word_type>::const_pointer;
    //! No doc
    using pointer = typename std::vector<word_type>::pointer;
    //! No doc
    using const_reference = typename std::vector<word_type>::const_reference;
    //! No doc
    using reference = typename std::vector<word_type>::reference;
    //! No doc
    using value_type = word_type;
    //! No doc
    using iterator_category = std::forward_iterator_tag;

    // None of the constructors are noexcept because the corresponding
    // constructors for std::vector aren't (until C++17).
    //! No doc
    const_wilo_iterator() = default;
    //! No doc
    const_wilo_iterator(const_wilo_iterator const&);
    //! No doc
    const_wilo_iterator(const_wilo_iterator&&) = default;
    //! No doc
    const_wilo_iterator& operator=(const_wilo_iterator const&) = default;
    //! No doc
    const_wilo_iterator& operator=(const_wilo_iterator&&) = default;

    //! No doc
    const_wilo_iterator(size_type   n,
                        size_type   upper_bound,
                        word_type&& first,
                        word_type&& last)
        : _current(std::move(first)),
          _index(),
          _letter(0),
          _upper_bound(upper_bound - 1),
          _last(std::move(last)),
          _number_letters(n) {
      _index = (_current == _last ? UNDEFINED : size_type(0));
    }

    //! No doc
    ~const_wilo_iterator();

    //! No doc
    bool operator==(const_wilo_iterator const& that) const noexcept {
      return _index == that._index;
    }

    //! No doc
    bool operator!=(const_wilo_iterator const& that) const noexcept {
      return !(this->operator==(that));
    }

    //! No doc
    const_reference operator*() const noexcept {
      return _current;
    }

    //! No doc
    const_pointer operator->() const noexcept {
      return &_current;
    }

    // prefix
    //! No doc
    const_wilo_iterator const& operator++() noexcept {
      if (_index != UNDEFINED) {
        ++_index;
      begin:
        if (_current.size() < _upper_bound && _letter != _number_letters) {
          _current.push_back(_letter);
          _letter = 0;
          if (lexicographical_compare(_current, _last)) {
            return *this;
          }
        } else if (!_current.empty()) {
          _letter = ++_current.back();
          _current.pop_back();
          goto begin;
        }
        _index = UNDEFINED;
      }
      return *this;
    }

    //! No doc
    // postfix
    const_wilo_iterator operator++(int) noexcept {
      const_wilo_iterator copy(*this);
      ++(*this);
      return copy;
    }

    //! No doc
    void swap(const_wilo_iterator& that) noexcept {
      std::swap(_letter, that._letter);
      std::swap(_index, that._index);
      std::swap(_upper_bound, that._upper_bound);
      std::swap(_last, that._last);
      std::swap(_number_letters, that._number_letters);
      _current.swap(that._current);
    }

   private:
    word_type   _current;
    size_type   _index;
    letter_type _letter;
    size_type   _upper_bound;
    word_type   _last;
    size_type   _number_letters;
  };

  // Assert that the forward iterator requirements are met
  static_assert(std::is_default_constructible<const_wilo_iterator>::value,
                "forward iterator requires default-constructible");
  static_assert(std::is_copy_constructible<const_wilo_iterator>::value,
                "forward iterator requires copy-constructible");
  static_assert(std::is_copy_assignable<const_wilo_iterator>::value,
                "forward iterator requires copy-assignable");
  static_assert(std::is_destructible<const_wilo_iterator>::value,
                "forward iterator requires destructible");

  //! No doc
  inline void swap(const_wilo_iterator& x, const_wilo_iterator& y) noexcept {
    x.swap(y);
  }

  //! Returns a forward iterator pointing to the 3rd parameter \p first.
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
  const_wilo_iterator cbegin_wilo(size_t      n,
                                  size_t      upper_bound,
                                  word_type&& first,
                                  word_type&& last);

  //! \copydoc cbegin_wilo(size_t const, size_t const, word_type&&, word_type&&)
  const_wilo_iterator cbegin_wilo(size_t           n,
                                  size_t           upper_bound,
                                  word_type const& first,
                                  word_type const& last);

  //! Returns a forward iterator pointing to one after the end of the range
  //! from \p first to \p last.
  //!
  //! The iterator returned by this function is still dereferencable and
  //! incrementable, but does not point to a word in the correct range.
  //!
  //! \sa cbegin_wilo
  const_wilo_iterator
  cend_wilo(size_t n, size_t upper_bound, word_type&& first, word_type&& last);

  //! \copydoc cend_wilo(size_t const, size_t const, word_type&&, word_type&&)
  const_wilo_iterator cend_wilo(size_t           n,
                                size_t           upper_bound,
                                word_type const& first,
                                word_type const& last);

}  // namespace libsemigroups

namespace std {
  template <>
  inline void swap(libsemigroups::const_wilo_iterator& x,
                   libsemigroups::const_wilo_iterator& y) noexcept {
    x.swap(y);
  }
}  // namespace std
#endif  // LIBSEMIGROUPS_WILO_HPP_
