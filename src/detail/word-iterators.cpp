//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2024 Joseph Edwards
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

// This file contains declarations of the iterator classes for words.

#include "libsemigroups/detail/word-iterators.hpp"

// #include <algorithm>  // for lexicographical_...

#include "libsemigroups/constants.hpp"  // for UNDEFINED
#include "libsemigroups/order.hpp"      // for lexicographical_compare
#include "libsemigroups/types.hpp"      // for word_type

namespace libsemigroups {
  namespace detail {
    const_wilo_iterator::const_wilo_iterator() noexcept = default;
    const_wilo_iterator::const_wilo_iterator(const_wilo_iterator const&)
        = default;
    const_wilo_iterator::const_wilo_iterator(const_wilo_iterator&&) noexcept
        = default;
    const_wilo_iterator&
    const_wilo_iterator::operator=(const_wilo_iterator const&)
        = default;
    const_wilo_iterator&
    const_wilo_iterator::operator=(const_wilo_iterator&&) noexcept
        = default;
    const_wilo_iterator::~const_wilo_iterator() = default;

    const_wilo_iterator::const_wilo_iterator(size_type   n,
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

    const_wilo_iterator const& const_wilo_iterator::operator++() noexcept {
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

    void const_wilo_iterator::swap(const_wilo_iterator& that) noexcept {
      std::swap(_letter, that._letter);
      std::swap(_index, that._index);
      std::swap(_upper_bound, that._upper_bound);
      std::swap(_last, that._last);
      std::swap(_number_letters, that._number_letters);
      _current.swap(that._current);
    }

    // Assert that the forward iterator requirements are met
    static_assert(std::is_default_constructible<const_wilo_iterator>::value,
                  "forward iterator requires default-constructible");
    static_assert(std::is_copy_constructible<const_wilo_iterator>::value,
                  "forward iterator requires copy-constructible");
    static_assert(std::is_copy_assignable<const_wilo_iterator>::value,
                  "forward iterator requires copy-assignable");
    static_assert(std::is_destructible<const_wilo_iterator>::value,
                  "forward iterator requires destructible");

    const_wislo_iterator::const_wislo_iterator() noexcept = default;
    const_wislo_iterator::const_wislo_iterator(const_wislo_iterator const&)
        = default;
    const_wislo_iterator::const_wislo_iterator(const_wislo_iterator&&) noexcept
        = default;
    const_wislo_iterator&
    const_wislo_iterator::operator=(const_wislo_iterator const&)
        = default;
    const_wislo_iterator&
    const_wislo_iterator::operator=(const_wislo_iterator&&) noexcept
        = default;
    const_wislo_iterator::~const_wislo_iterator() = default;

    const_wislo_iterator::const_wislo_iterator(size_type   n,
                                               word_type&& first,
                                               word_type&& last)
        : _current(std::move(first)),
          _index(),
          _last(std::move(last)),
          _number_letters(n) {
      _current.reserve(last.size());
      _index = (_current == _last ? UNDEFINED : size_t(0));
    }

    const_wislo_iterator const& const_wislo_iterator::operator++() noexcept {
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

    void const_wislo_iterator::swap(const_wislo_iterator& that) noexcept {
      std::swap(_current, that._current);
      std::swap(_index, that._index);
      std::swap(_last, that._last);
      std::swap(_number_letters, that._number_letters);
    }

    // Assert that the forward iterator requirements are met
    static_assert(std::is_default_constructible<const_wislo_iterator>::value,
                  "forward iterator requires default-constructible");
    static_assert(std::is_copy_constructible<const_wislo_iterator>::value,
                  "forward iterator requires copy-constructible");
    static_assert(std::is_copy_assignable<const_wislo_iterator>::value,
                  "forward iterator requires copy-assignable");
    static_assert(std::is_destructible<const_wislo_iterator>::value,
                  "forward iterator requires destructible");
  }  // namespace detail
}  // namespace libsemigroups