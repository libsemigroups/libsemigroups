//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019-2023 James D. Mitchell
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

// This file contains some functionality missing in some implementations of the
// stl, or to augment the stl implementations.

#include "libsemigroups/words.hpp"

#include <algorithm>    // for lexicographical_...
#include <cmath>        // for pow
#include <cstring>      // for strlen
#include <string>       // for allocator
#include <type_traits>  // for is_destructible
#include <utility>      // for move

#include "libsemigroups/config.hpp"     // for LIBSEMIGROUPS_DEBUG
#include "libsemigroups/constants.hpp"  // for UNDEFINED
#include "libsemigroups/debug.hpp"      // for LIBSEMIGROUPS_ASSERT
#include "libsemigroups/exception.hpp"  // for word_type
#include "libsemigroups/order.hpp"      // for order
#include "libsemigroups/present.hpp"    // for index
#include "libsemigroups/types.hpp"      // for word_type

#include "libsemigroups/detail/report.hpp"  // for magic_enum formatting

#include "rx/ranges.hpp"  // for count, operator|

namespace libsemigroups {
  namespace {
    uint64_t geometric_progression(size_t n, size_t a, size_t r) {
      return a * ((1 - std::pow(r, n)) / (1 - static_cast<float>(r)));
    }
  }  // namespace

  uint64_t number_of_words(size_t n, size_t min, size_t max) {
    if (max <= min) {
      return 0;
    }
    return geometric_progression(max, 1, n) - geometric_progression(min, 1, n);
  }

  void detail::word_to_string(std::string const& alphabet,
                              word_type const&   input,
                              std::string&       output) {
    output.clear();
    output.reserve(input.size());
    for (auto const x : input) {
      output.push_back(alphabet[x]);
    }
  }

  void detail::StringToWord::operator()(std::string const& input,
                                        word_type&         output) const {
    output.clear();
    output.reserve(input.size());
    for (auto const& c : input) {
      output.push_back(_lookup[c]);
    }
  }

  word_type detail::StringToWord::operator()(std::string const& input) const {
    word_type output;
    operator()(input, output);
    return output;
  }

  namespace literals {
    word_type operator"" _w(const char* w, size_t n) {
      word_type result;
#if LIBSEMIGROUPS_DEBUG
      static const std::string valid_chars = "0123456789";
#endif
      for (size_t i = 0; i < n; ++i) {
        LIBSEMIGROUPS_ASSERT(valid_chars.find(w[i]) != std::string::npos);
        if (48 <= w[i] && w[i] < 58) {
          result.push_back(static_cast<letter_type>(w[i] - 48));
        } else {
          result.push_back(presentation::index(w[i]));
        }
      }
      return result;
    }

    word_type operator"" _w(const char* w) {
      return operator"" _w(w, std::strlen(w));
    }
  }  // namespace literals

  const_wilo_iterator::const_wilo_iterator() = default;
  const_wilo_iterator::const_wilo_iterator(const_wilo_iterator const&)
      = default;
  const_wilo_iterator::const_wilo_iterator(const_wilo_iterator&&) = default;
  const_wilo_iterator&
  const_wilo_iterator::operator=(const_wilo_iterator const&)
      = default;
  const_wilo_iterator& const_wilo_iterator::operator=(const_wilo_iterator&&)
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

  const_wilo_iterator cbegin_wilo(size_t      n,
                                  size_t      upper_bound,
                                  word_type&& first,
                                  word_type&& last) {
    if (!lexicographical_compare(
            first.cbegin(), first.cend(), last.cbegin(), last.cend())) {
      return cend_wilo(n, upper_bound, std::move(first), std::move(last));
    }
    if (first.size() >= upper_bound) {
      return ++const_wilo_iterator(
          n, upper_bound, std::move(first), std::move(last));
    }
    return const_wilo_iterator(
        n, upper_bound, std::move(first), std::move(last));
  }

  const_wilo_iterator cbegin_wilo(size_t           n,
                                  size_t           upper_bound,
                                  word_type const& first,
                                  word_type const& last) {
    return cbegin_wilo(n, upper_bound, word_type(first), word_type(last));
  }

  const_wilo_iterator
  cend_wilo(size_t n, size_t upper_bound, word_type&&, word_type&& last) {
    return const_wilo_iterator(
        n, upper_bound, word_type(last), std::move(last));
  }

  const_wilo_iterator cend_wilo(size_t n,
                                size_t upper_bound,
                                word_type const&,
                                word_type const& last) {
    return cend_wilo(n, upper_bound, word_type(), word_type(last));
  }

  const_wislo_iterator::const_wislo_iterator() = default;
  const_wislo_iterator::const_wislo_iterator(const_wislo_iterator const&)
      = default;
  const_wislo_iterator::const_wislo_iterator(const_wislo_iterator&&) = default;
  const_wislo_iterator&
  const_wislo_iterator::operator=(const_wislo_iterator const&)
      = default;
  const_wislo_iterator& const_wislo_iterator::operator=(const_wislo_iterator&&)
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

  const_wislo_iterator cbegin_wislo(size_t      n,
                                    word_type&& first,
                                    word_type&& last) {
    if (!shortlex_compare(
            first.cbegin(), first.cend(), last.cbegin(), last.cend())) {
      return cend_wislo(n, std::move(first), std::move(last));
    }
    return const_wislo_iterator(n, std::move(first), std::move(last));
  }

  const_wislo_iterator cbegin_wislo(size_t           n,
                                    word_type const& first,
                                    word_type const& last) {
    return cbegin_wislo(n, word_type(first), word_type(last));
  }

  const_wislo_iterator cend_wislo(size_t n, word_type&&, word_type&& last) {
    return const_wislo_iterator(n, word_type(last), std::move(last));
  }

  const_wislo_iterator cend_wislo(size_t n,
                                  word_type const&,
                                  word_type const& last) {
    return cend_wislo(n, word_type(), word_type(last));
  }

  void Words::set_iterator() const {
    if (!_current_valid) {
      _current_valid = true;
      if (_order == Order::shortlex) {
        _current = cbegin_wislo(_number_of_letters, _first, _last);
        _end     = cend_wislo(_number_of_letters, _first, _last);
      } else if (_order == Order::lex) {
        _current = cbegin_wilo(_number_of_letters, _upper_bound, _first, _last);
        _end     = cend_wilo(_number_of_letters, _upper_bound, _first, _last);
      }
    }
  }

  size_t Words::count() const noexcept {
    if (_order == Order::shortlex) {
      return size_hint();
    } else {
      return (*this | rx::count());
    }
  }

  Words& Words::init() {
    _number_of_letters = 0;
    _current_valid     = false;
    _first             = {};
    _last              = {};
    _order             = Order::shortlex;
    _upper_bound       = 0;  // does nothing if _order is shortlex
    return *this;
  }

  Words::Words(Words const&)            = default;
  Words::Words(Words&&)                 = default;
  Words& Words::operator=(Words const&) = default;
  Words& Words::operator=(Words&&)      = default;
  Words::~Words()                       = default;

  Words& Words::order(Order val) {
    if (val != Order::shortlex && val != Order::lex) {
      LIBSEMIGROUPS_EXCEPTION(
          "the argument must be Order::shortlex or Order::lex, found {}", val);
    }
    _current_valid &= (val == _order);
    _order = val;
    return *this;
  }

  Strings& Strings::init() {
    _current.clear();
    _current_valid = false;
    _letters.clear();
    _string_to_word.clear();
    _words.init();
    return *this;
  }

  Strings::Strings(Strings const&)            = default;
  Strings::Strings(Strings&&)                 = default;
  Strings& Strings::operator=(Strings const&) = default;
  Strings& Strings::operator=(Strings&&)      = default;
  Strings::~Strings()                         = default;

  Strings& Strings::letters(std::string const& x) {
    _current_valid = false;
    _words.letters(x.size());
    _letters = x;
    _string_to_word.init(x);
    return *this;
  }

  void Strings::swap(Strings& that) noexcept {
    std::swap(_current, that._current);
    std::swap(_current_valid, that._current_valid);
    std::swap(_letters, that._letters);
    std::swap(_string_to_word, that._string_to_word);
    std::swap(_words, that._words);
  }
}  // namespace libsemigroups
