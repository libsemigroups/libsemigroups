//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2024-2026 Joseph Edwards
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

// This file contains declarations of the impl version of iterator classes for
// words.

#include "libsemigroups/detail/word-iterators-impl.hpp"

#include "libsemigroups/constants.hpp"  // for UNDEFINED
#include "libsemigroups/exception.hpp"  // for LIBSEMIGROUPS_ASSERT
#include "libsemigroups/order.hpp"      // for lex_cmp
#include "libsemigroups/types.hpp"      // for word_type

namespace libsemigroups {
  namespace detail {

    //////////////////////////////////////////////////////////////////
    // wilo
    //////////////////////////////////////////////////////////////////

    const_wilo_iterator_impl::const_wilo_iterator_impl() noexcept = default;
    const_wilo_iterator_impl::const_wilo_iterator_impl(
        const_wilo_iterator_impl const&) noexcept
        = default;
    const_wilo_iterator_impl::const_wilo_iterator_impl(
        const_wilo_iterator_impl&&) noexcept
        = default;
    const_wilo_iterator_impl& const_wilo_iterator_impl::operator=(
        const_wilo_iterator_impl const&) noexcept
        = default;
    const_wilo_iterator_impl&
    const_wilo_iterator_impl::operator=(const_wilo_iterator_impl&&) noexcept
        = default;
    const_wilo_iterator_impl::~const_wilo_iterator_impl() noexcept = default;

    void const_wilo_iterator_impl::init(size_type n, size_type upper_bound) {
      _letter         = 0;
      _upper_bound    = upper_bound - 1;
      _number_letters = n;
      _index          = (!lex_cmp(_current, _last) ? UNDEFINED : size_type(0));

      // Increment if _current is out of bounds
      if (_index != UNDEFINED && _current.size() >= upper_bound) {
        operator++();
      }
    }

    const_wilo_iterator_impl::const_wilo_iterator_impl(
        size_type                 n,
        size_type                 upper_bound,
        word_type::const_iterator first_begin,
        word_type::const_iterator first_end,
        word_type::const_iterator last_begin,
        word_type::const_iterator last_end)
        : const_wilo_iterator_impl() {
      init(n, upper_bound, first_begin, first_end, last_begin, last_end);
    }

    const_wilo_iterator_impl::const_wilo_iterator_impl(size_type   n,
                                                       size_type   upper_bound,
                                                       word_type&& first,
                                                       word_type&& last)
        : const_wilo_iterator_impl() {
      init(n, upper_bound, std::move(first), std::move(last));
    }

    void const_wilo_iterator_impl::init(size_type   n,
                                        size_type   upper_bound,
                                        word_type&& first,
                                        word_type&& last) {
      _current = std::move(first);
      _last    = std::move(last);
      init(n, upper_bound);
    }

    void const_wilo_iterator_impl::init(size_type                 n,
                                        size_type                 upper_bound,
                                        word_type::const_iterator first_begin,
                                        word_type::const_iterator first_end,
                                        word_type::const_iterator last_begin,
                                        word_type::const_iterator last_end) {
      _current.assign(first_begin, first_end);
      _last.assign(last_begin, last_end);
      init(n, upper_bound);
    }

    const_wilo_iterator_impl const&
    const_wilo_iterator_impl::operator++() noexcept {
      if (_index != UNDEFINED) {
        ++_index;
      begin:
        if (_current.size() < _upper_bound && _letter != _number_letters) {
          _current.push_back(_letter);
          _letter = 0;
          if (lex_cmp(_current, _last)) {
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

    void
    const_wilo_iterator_impl::swap(const_wilo_iterator_impl& that) noexcept {
      std::swap(_letter, that._letter);
      std::swap(_index, that._index);
      std::swap(_upper_bound, that._upper_bound);
      std::swap(_last, that._last);
      std::swap(_number_letters, that._number_letters);
      _current.swap(that._current);
    }

    // Assert that the forward iterator requirements are met
    static_assert(
        std::is_default_constructible<const_wilo_iterator_impl>::value,
        "forward iterator requires default-constructible");
    static_assert(std::is_copy_constructible<const_wilo_iterator_impl>::value,
                  "forward iterator requires copy-constructible");
    static_assert(std::is_copy_assignable<const_wilo_iterator_impl>::value,
                  "forward iterator requires copy-assignable");
    static_assert(std::is_destructible<const_wilo_iterator_impl>::value,
                  "forward iterator requires destructible");

    //////////////////////////////////////////////////////////////////
    // wislo
    //////////////////////////////////////////////////////////////////

    const_wislo_iterator_impl::const_wislo_iterator_impl() noexcept = default;
    const_wislo_iterator_impl::const_wislo_iterator_impl(
        const_wislo_iterator_impl const&) noexcept
        = default;
    const_wislo_iterator_impl::const_wislo_iterator_impl(
        const_wislo_iterator_impl&&) noexcept
        = default;
    const_wislo_iterator_impl& const_wislo_iterator_impl::operator=(
        const_wislo_iterator_impl const&) noexcept
        = default;
    const_wislo_iterator_impl&
    const_wislo_iterator_impl::operator=(const_wislo_iterator_impl&&) noexcept
        = default;
    const_wislo_iterator_impl::~const_wislo_iterator_impl() noexcept = default;

    void const_wislo_iterator_impl::init(size_type n) {
      _number_letters = n;
      _current.reserve(_last.size());
      _index = (!lenlex_cmp(_current, _last) ? UNDEFINED : size_t(0));
    }

    const_wislo_iterator_impl::const_wislo_iterator_impl(size_type   n,
                                                         word_type&& first,
                                                         word_type&& last)
        : const_wislo_iterator_impl() {
      init(n, std::move(first), std::move(last));
    }

    const_wislo_iterator_impl::const_wislo_iterator_impl(
        size_type                 n,
        word_type::const_iterator first_begin,
        word_type::const_iterator first_end,
        word_type::const_iterator last_begin,
        word_type::const_iterator last_end)
        : const_wislo_iterator_impl() {
      init(n, first_begin, first_end, last_begin, last_end);
    }

    void const_wislo_iterator_impl::init(size_type   n,
                                         word_type&& first,
                                         word_type&& last) {
      _current = std::move(first);
      _last    = std::move(last);
      init(n);
    }

    void const_wislo_iterator_impl::init(size_type                 n,
                                         word_type::const_iterator first_begin,
                                         word_type::const_iterator first_end,
                                         word_type::const_iterator last_begin,
                                         word_type::const_iterator last_end) {
      _current.assign(first_begin, first_end);
      _last.assign(last_begin, last_end);
      init(n);
    }

    const_wislo_iterator_impl const&
    const_wislo_iterator_impl::operator++() noexcept {
      if (_index != UNDEFINED) {
        ++_index;
        size_t n = _current.size();
        while (!_current.empty() && ++_current.back() == _number_letters) {
          _current.pop_back();
        }
        _current.resize((_current.empty() ? n + 1 : n), 0);
        // TODO lenlex_cmp -> lenlex_cmp_no_checks
        if (!lenlex_cmp(_current, _last)) {
          _index = UNDEFINED;
        }
      }
      return *this;
    }

    void
    const_wislo_iterator_impl::swap(const_wislo_iterator_impl& that) noexcept {
      std::swap(_current, that._current);
      std::swap(_index, that._index);
      std::swap(_last, that._last);
      std::swap(_number_letters, that._number_letters);
    }

    // Assert that the forward iterator requirements are met
    static_assert(
        std::is_default_constructible<const_wislo_iterator_impl>::value,
        "forward iterator requires default-constructible");
    static_assert(std::is_copy_constructible<const_wislo_iterator_impl>::value,
                  "forward iterator requires copy-constructible");
    static_assert(std::is_copy_assignable<const_wislo_iterator_impl>::value,
                  "forward iterator requires copy-assignable");
    static_assert(std::is_destructible<const_wislo_iterator_impl>::value,
                  "forward iterator requires destructible");
  }  // namespace detail
}  // namespace libsemigroups
