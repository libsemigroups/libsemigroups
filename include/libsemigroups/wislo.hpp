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

// This file contains some functionality for generating elements in the free
// monoid over an alphabet with a given number of letters up to a given length
// in short-lex order.
//
// WISLO = Words In Short-Lex Order

#ifndef LIBSEMIGROUPS_WISLO_HPP_
#define LIBSEMIGROUPS_WISLO_HPP_

#include <cstddef>   // for size_t
#include <iterator>  // for forward_iterator_tag
#include <vector>    // for vector

#include "constants.hpp"  // for UNDEFINED
#include "order.hpp"      // for shortlex_compare
#include "types.hpp"      // for word_type

// TODO:
// 2. rvalue reference cend_wislo and cbegin_wislo

namespace libsemigroups {
  //! No doc
  class const_wislo_iterator final {
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
    const_wislo_iterator() = default;
    //! No doc
    const_wislo_iterator(const_wislo_iterator const&) = default;
    //! No doc
    const_wislo_iterator(const_wislo_iterator&&) = default;
    //! No doc
    const_wislo_iterator& operator=(const_wislo_iterator const&) = default;
    //! No doc
    const_wislo_iterator& operator=(const_wislo_iterator&&) = default;

    //! No doc
    const_wislo_iterator(size_type const n, word_type&& first, word_type&& last)
        : _current(std::move(first)),
          _index(),
          _last(std::move(last)),
          _number_letters(n) {
      _current.reserve(last.size());
      _index = (_current == _last ? UNDEFINED : size_t(0));
    }

    //! No doc
    ~const_wislo_iterator() = default;

    //! No doc
    bool operator==(const_wislo_iterator const& that) const noexcept {
      return _index == that._index;
    }

    //! No doc
    bool operator!=(const_wislo_iterator const& that) const noexcept {
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
    const_wislo_iterator const& operator++() noexcept {
      if (_index != UNDEFINED) {
        ++_index;
        size_t n = _current.size();
        while (!_current.empty() && ++_current.back() == _number_letters) {
          _current.pop_back();
        }
        _current.resize((_current.empty() ? n + 1 : n), 0);
        if (!shortlex_compare(_current, _last)) {
          _index = UNDEFINED;
        }
      }
      return *this;
    }

    // postfix
    //! No doc
    const_wislo_iterator operator++(int) noexcept {
      const_wislo_iterator copy(*this);
      ++(*this);
      return copy;
    }

    //! No doc
    void swap(const_wislo_iterator& that) noexcept {
      std::swap(_current, that._current);
      std::swap(_index, that._index);
      std::swap(_last, that._last);
      std::swap(_number_letters, that._number_letters);
    }

   private:
    word_type _current;
    size_type _index;
    word_type _last;
    size_type _number_letters;
  };

  // Assert that the forward iterator requirements are met
  static_assert(std::is_default_constructible<const_wislo_iterator>::value,
                "forward iterator requires default-constructible");
  static_assert(std::is_copy_constructible<const_wislo_iterator>::value,
                "forward iterator requires copy-constructible");
  static_assert(std::is_copy_assignable<const_wislo_iterator>::value,
                "forward iterator requires copy-assignable");
  static_assert(std::is_destructible<const_wislo_iterator>::value,
                "forward iterator requires destructible");

  //! Returns a forward iterator pointing to the 2nd parameter \p first.
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
  const_wislo_iterator cbegin_wislo(size_t const n,
                                    word_type&&  first,
                                    word_type&&  last);

  //! \copydoc cbegin_wislo(size_t const, word_type&&, word_type&&)
  const_wislo_iterator cbegin_wislo(size_t const     n,
                                    word_type const& first,
                                    word_type const& last);

  //! Returns a forward iterator pointing to one after the end of the range
  //! from \p first to \p last.
  //!
  //! The iterator returned by this is still dereferencable and incrementable,
  //! but does not point to a word in the correct range.
  //!
  //! \sa cbegin_wislo
  const_wislo_iterator cend_wislo(size_t const n,
                                  word_type&&  first,
                                  word_type&&  last);

  //! \copydoc cend_wislo(size_t const, word_type&&, word_type&&)
  const_wislo_iterator cend_wislo(size_t const     n,
                                  word_type const& first,
                                  word_type const& last);
  //! No doc
  inline void swap(const_wislo_iterator& x, const_wislo_iterator& y) noexcept {
    x.swap(y);
  }

}  // namespace libsemigroups

namespace std {
  template <>
  inline void swap(libsemigroups::const_wislo_iterator& x,
                   libsemigroups::const_wislo_iterator& y) noexcept {
    x.swap(y);
  }
}  // namespace std
#endif  // LIBSEMIGROUPS_WISLO_HPP_
