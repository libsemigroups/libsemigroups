//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2026 James Mitchell + Joseph Edwards
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

// This file contains the implementations of the functionality declared in
// word-range-class.hpp

#include "libsemigroups/word-range-class.hpp"

#include <algorithm>  // for lexicographical_...
#include <string>     // for allocator
#include <utility>    // for move

#include "libsemigroups/debug.hpp"      // for LIBSEMIGROUPS_ASSERT
#include "libsemigroups/exception.hpp"  // for LibsemigroupsException
#include "libsemigroups/order.hpp"      // for order

#include "libsemigroups/detail/formatters.hpp"  // for magic_enum formatting

namespace libsemigroups {
  namespace {
    bool word_in_language(size_t n, word_type const& w) {
      return std::all_of(
          w.cbegin(), w.cend(), [&](letter_type x) { return x < n; });
    }
  }  // namespace

  ////////////////////////////////////////////////////////////////////////
  // Word iterators
  ////////////////////////////////////////////////////////////////////////

  detail::const_wilo_iterator cbegin_wilo(size_t      n,
                                          size_t      upper_bound,
                                          word_type&& first,
                                          word_type&& last) {
    if (!word_in_language(n, first)
        || !std::lexicographical_compare(
            first.cbegin(), first.cend(), last.cbegin(), last.cend())) {
      return cend_wilo(n, upper_bound, std::move(first), std::move(last));
    }
    if (first.size() >= upper_bound) {
      return ++detail::const_wilo_iterator(
          n, upper_bound, std::move(first), std::move(last));
    }
    return detail::const_wilo_iterator(
        n, upper_bound, std::move(first), std::move(last));
  }

  detail::const_wilo_iterator cbegin_wilo(size_t           n,
                                          size_t           upper_bound,
                                          word_type const& first,
                                          word_type const& last) {
    return cbegin_wilo(n, upper_bound, word_type(first), word_type(last));
  }

  detail::const_wilo_iterator
  cend_wilo(size_t n, size_t upper_bound, word_type&&, word_type&& last) {
    return detail::const_wilo_iterator(
        n, upper_bound, word_type(last), std::move(last));
  }

  detail::const_wilo_iterator cend_wilo(size_t n,
                                        size_t upper_bound,
                                        word_type const&,
                                        word_type const& last) {
    return cend_wilo(n, upper_bound, word_type(), word_type(last));
  }

  detail::const_wislo_iterator cbegin_wislo(size_t      n,
                                            word_type&& first,
                                            word_type&& last) {
    if (!word_in_language(n, first)
        || !lenlex_cmp(
            first.cbegin(), first.cend(), last.cbegin(), last.cend())) {
      return cend_wislo(n, std::move(first), std::move(last));
    }
    return detail::const_wislo_iterator(n, std::move(first), std::move(last));
  }

  detail::const_wislo_iterator cbegin_wislo(size_t           n,
                                            word_type const& first,
                                            word_type const& last) {
    return cbegin_wislo(n, word_type(first), word_type(last));
  }

  detail::const_wislo_iterator cend_wislo(size_t n,
                                          word_type&&,
                                          word_type&& last) {
    return detail::const_wislo_iterator(n, word_type(last), std::move(last));
  }

  detail::const_wislo_iterator cend_wislo(size_t n,
                                          word_type const&,
                                          word_type const& last) {
    return cend_wislo(n, word_type(), word_type(last));
  }

  ////////////////////////////////////////////////////////////////////////
  // WordRange
  ////////////////////////////////////////////////////////////////////////

  void WordRange::set_iterator() const {
    if (!_current_valid) {
      _current_valid = true;
      _visited       = 0;
      if (_order == Order::lenlex) {
        _current = cbegin_wislo(_alphabet_size, _first, _last);
        _end     = cend_wislo(_alphabet_size, _first, _last);
      } else if (_order == Order::lex) {
        _current = cbegin_wilo(_alphabet_size, _upper_bound, _first, _last);
        _end     = cend_wilo(_alphabet_size, _upper_bound, _first, _last);
      }
    }
  }

  size_t WordRange::count() const noexcept {
    if (_order == Order::lenlex) {
      return size_hint();
    } else {
      return (*this | rx::count());
    }
  }

  WordRange& WordRange::init() {
    _alphabet_size = 0;
    _current_valid = false;
    _first         = {};
    _last          = {};
    _order         = Order::lenlex;
    _upper_bound   = 0;  // does nothing if _order is lenlex
    _visited       = 0;
    return *this;
  }

  WordRange::WordRange(WordRange const&)            = default;
  WordRange::WordRange(WordRange&&)                 = default;
  WordRange& WordRange::operator=(WordRange const&) = default;
  WordRange& WordRange::operator=(WordRange&&)      = default;
  WordRange::~WordRange()                           = default;

  WordRange& WordRange::order(Order val) {
    if (val != Order::lenlex && val != Order::lex) {
      LIBSEMIGROUPS_EXCEPTION(
          "the argument must be Order::lenlex or Order::lex, found {}", val);
    }
    _current_valid &= (val == _order);
    _order = val;
    return *this;
  }

  std::string to_human_readable_repr(WordRange const& wr, size_t max_width) {
    using detail::group_digits;
    word_type const& first = wr.first();
    word_type const& last  = wr.last();
    size_t const     count = wr.count();
    std::string      out;

    bool print_short = false;

    if (first.size() > max_width || last.size() > max_width) {
      print_short = true;
    }

    if (!print_short) {
      out = fmt::format("<WordRange of length {} between {} and {} with "
                        "letters in [0, {}) in {} order>",
                        group_digits(count),
                        first,
                        last,
                        group_digits(wr.alphabet_size()),
                        wr.order());
    }

    if (out.size() > max_width) {
      print_short = true;
    }

    if (print_short) {
      out = fmt::format(
          "<WordRange of length {} with letters in [0, {}) in {} order>",
          group_digits(count),
          group_digits(wr.alphabet_size()),
          wr.order());
    }

    return out;
  }

  ////////////////////////////////////////////////////////////////////////
  // StringRange
  ////////////////////////////////////////////////////////////////////////

  StringRange& StringRange::init() {
    _current.clear();
    _current_valid = false;
    _letters.clear();
    _to_word.init();
    _to_string.init();
    _word_range.init();
    return *this;
  }

  StringRange::StringRange(StringRange const&)            = default;
  StringRange::StringRange(StringRange&&)                 = default;
  StringRange& StringRange::operator=(StringRange const&) = default;
  StringRange& StringRange::operator=(StringRange&&)      = default;
  StringRange::~StringRange()                             = default;

  StringRange& StringRange::alphabet(std::string const& x) {
    // Need to do this _to_word.init(x) first, because if this throws then the
    // rest should remain unchanged.
    _to_word.init(x);
    _to_string.init(x);
    _word_range.alphabet_size(x.size());
    _current_valid = _word_range.valid();
    _letters       = x;
    return *this;
  }

  std::string to_human_readable_repr(StringRange const& sr, size_t max_width) {
    using detail::group_digits;

    std::string const& first    = sr.first();
    std::string const& last     = sr.last();
    std::string const& alphabet = sr.alphabet();
    size_t const       count    = sr.count();
    std::string        out;

    bool print_short = false;

    if (first.size() > max_width || last.size() > max_width
        || alphabet.size() > max_width) {
      print_short = true;
    }

    if (!print_short) {
      out = fmt::format(
          "<StringRange of length {} between \"{}\" and \"{}\" with letters "
          "in \"{}\" in {} order>",
          group_digits(count),
          first,
          last,
          alphabet,
          sr.order());
    }

    if (out.size() > max_width) {
      print_short = true;
    }

    if (print_short) {
      out = fmt::format("<StringRange of length {} in {} order>",
                        group_digits(count),
                        sr.order());
    }

    return out;
  }
}  // namespace libsemigroups
