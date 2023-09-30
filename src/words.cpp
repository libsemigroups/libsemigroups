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

#include <iostream>

#include <algorithm>    // for lexicographical_...
#include <cmath>        // for pow
#include <cstring>      // for strlen
#include <string>       // for allocator
#include <type_traits>  // for is_destructible
#include <utility>      // for move

#include "libsemigroups/config.hpp"        // for LIBSEMIGROUPS_DEBUG
#include "libsemigroups/constants.hpp"     // for UNDEFINED
#include "libsemigroups/debug.hpp"         // for LIBSEMIGROUPS_ASSERT
#include "libsemigroups/exception.hpp"     // for word_type
#include "libsemigroups/order.hpp"         // for order
#include "libsemigroups/presentation.hpp"  // for index
#include "libsemigroups/types.hpp"         // for word_type

#include "libsemigroups/detail/report.hpp"  // for magic_enum formatting

#include "rx/ranges.hpp"  // for count, operator|

namespace libsemigroups {
  namespace {
    uint64_t geometric_progression(size_t n, size_t a, size_t r) {
      return a * ((1 - std::pow(r, n)) / (1 - static_cast<float>(r)));
    }

    // The next function implements the Shunting Yard Algorithm to convert the
    // expression in input to reverse Polish notation, as described here:
    // https://en.wikipedia.org/wiki/Shunting_yard_algorithm
    std::string shunting_yard(char const* input, size_t len) {
      std::string input_copy;
      if (len == 0) {
        return input_copy;
      }

      for (size_t i = 0; i < len - 1; ++i) {
        input_copy += input[i];
        if ((std::isalpha(input[i])
             && (std::isalpha(input[i + 1]) || input[i + 1] == '('))
            || (std::isdigit(input[i]) && !std::isdigit(input[i + 1])
                && input[i + 1] != ')')
            || (input[i] == ')' && std::isalpha(input[i + 1]))) {
          input_copy += "*";
        }
      }
      input_copy += input[len - 1];
      // std::cout << input_copy << std::endl;

      std::string      output;
      std::stack<char> ops;

      for (size_t i = 0; i < input_copy.size(); ++i) {
        if (std::isalpha(input_copy[i])) {
          output += input_copy[i];
        } else if (std::isdigit(input_copy[i])) {
          output += input_copy[i];
        } else if (input_copy[i] == '(' || input_copy[i] == '^') {
          ops.push(input_copy[i]);
        } else if (input_copy[i] == '*') {
          while (!ops.empty() && ops.top() != '(') {
            output += ops.top();
            ops.pop();
          }
          ops.push(input_copy[i]);
        } else if (input_copy[i] == ')') {
          if (ops.empty()) {
            LIBSEMIGROUPS_EXCEPTION("TODO1");
          }
          while (!ops.empty() && ops.top() != '(') {
            output += ops.top();
            ops.pop();
          }
          if (ops.empty()) {
            LIBSEMIGROUPS_EXCEPTION("TODO2");
          }
          ops.pop();  // pop the '(' from the stack and discard
        } else if (input_copy[i] != ' ') {
          // ignore spaces
          LIBSEMIGROUPS_EXCEPTION("TODO3");
        }
      }
      while (!ops.empty()) {
        if (ops.top() == '(' || ops.top() == ')') {
          LIBSEMIGROUPS_EXCEPTION("TODO4");
        }
        output += ops.top();
        ops.pop();
      }
      // std::cout << output << std::endl;
      return output;
    }

    bool try_pop_two(std::stack<std::string>&             stck,
                     std::pair<std::string, std::string>& pr) {
      if (stck.size() < 2) {
        return false;
      }
      pr.first = std::move(stck.top());
      stck.pop();
      pr.second = std::move(stck.top());
      stck.pop();
      return true;
    }

    std::string inline evaluate_rpn(std::string const& rpn) {
      using namespace words;
      std::stack<std::string>             stck;
      bool                                in_digits = false;
      std::pair<std::string, std::string> pr;

      for (auto const& term : rpn) {
        if (term == '^') {
          in_digits = false;
          if (try_pop_two(stck, pr)) {
            stck.push(pow(pr.second, std::stol(pr.first)));
          } else {
            LIBSEMIGROUPS_EXCEPTION("TODO6");
          }
        } else if (term == '*') {
          in_digits = false;
          if (try_pop_two(stck, pr)) {
            stck.push(pr.second + pr.first);
          } else {
            LIBSEMIGROUPS_EXCEPTION("TODO7");
          }
        } else if (std::isdigit(term)) {
          if (in_digits) {
            LIBSEMIGROUPS_ASSERT(!stck.empty());
            stck.top() += term;
          } else {
            in_digits = true;
            stck.emplace(&term, 1);
          }
        } else {
          in_digits = false;
          stck.emplace(&term, 1);
        }
      }
      std::string result("");
      while (!stck.empty()) {
        result = stck.top() + result;
        stck.pop();
      }
      return result;
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
    word_type operator"" _w(char const* w, size_t n) {
      word_type result;
#if LIBSEMIGROUPS_DEBUG
      static const std::string valid_chars = "0123456789";
#endif
      // 0 is unset, 1 is reading integers, 2 is parsing a string
      int mode = 0;
      for (size_t i = 0; i < n; ++i) {
        if (48 <= w[i] && w[i] < 58) {
          if (mode == 0) {
            mode = 1;
          } else if (mode == 2) {
            LIBSEMIGROUPS_EXCEPTION("cannot mix numbers and letters, expected "
                                    "digits in 0123456789, found {}",
                                    w[i]);
          }
          LIBSEMIGROUPS_ASSERT(valid_chars.find(w[i]) != std::string::npos);
          result.push_back(static_cast<letter_type>(w[i] - 48));
        } else {
          if (mode == 0) {
            mode = 2;
          } else if (mode == 1) {
            LIBSEMIGROUPS_EXCEPTION("cannot mix numbers and letters, expected "
                                    "digits in 0123456789, found {}",
                                    w[i]);
          }
          result.push_back(human_readable_index(w[i]));
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

  std::string parse(char const* w, size_t n) {
    return evaluate_rpn(shunting_yard(w, n));
  }

  namespace {
    std::string const& chars_in_human_readable_order() {
      // Choose visible characters a-zA-Z0-9 first before anything else
      // The ascii ranges for these characters are: [97, 123), [65, 91),
      // [48, 58) so the remaining range of chars that are appended to the end
      // after these chars are [0,48), [58, 65), [91, 97), [123, 255)
      static std::string letters
          = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
      static bool first_call = true;
      if (first_call) {
        letters.resize(255);
        std::iota(
            letters.begin() + 62, letters.begin() + 110, static_cast<char>(0));
        std::iota(letters.begin() + 110,
                  letters.begin() + 117,
                  static_cast<char>(58));
        std::iota(letters.begin() + 117,
                  letters.begin() + 123,
                  static_cast<char>(91));
        std::iota(letters.begin() + 123, letters.end(), static_cast<char>(123));
        first_call = false;
        LIBSEMIGROUPS_ASSERT(letters.size()
                             == std::numeric_limits<char>::max()
                                    - std::numeric_limits<char>::min());
        LIBSEMIGROUPS_ASSERT(letters.end() == letters.begin() + 255);
      }
      return letters;
    }

  }  // namespace

  char human_readable_char(size_t i) {
    using letter_type = typename Presentation<std::string>::letter_type;
    // Choose visible characters a-zA-Z0-9 first before anything else
    // The ascii ranges for these characters are: [97, 123), [65, 91),
    // [48, 58) so the remaining range of chars that are appended to the end
    // after these chars are [0,48), [58, 65), [91, 97), [123, 255)
    if (i >= std::numeric_limits<letter_type>::max()
                 - std::numeric_limits<letter_type>::min()) {
      LIBSEMIGROUPS_EXCEPTION("expected a value in the range [0, {}) found {}",
                              std::numeric_limits<letter_type>::max()
                                  - std::numeric_limits<letter_type>::min(),
                              i);
    }
    return chars_in_human_readable_order()[i];
  }

  size_t human_readable_index(char c) {
    static bool first_call = true;
    static std::unordered_map<Presentation<std::string>::letter_type,
                              Presentation<word_type>::letter_type>
        map;
    if (first_call) {
      first_call        = false;
      auto const& chars = chars_in_human_readable_order();
      for (letter_type i = 0; i < chars.size(); ++i) {
        map.emplace(chars[i], i);
      }
    }

    auto it = map.find(c);
    if (it == map.cend()) {
      LIBSEMIGROUPS_EXCEPTION(
          "unexpected character, cannot convert \'{}\' to a letter", c);
    }

    return it->second;
  }

  void to_word(word_type& w, std::string const& s) {
    w.resize(s.size(), 0);
    std::transform(s.cbegin(), s.cend(), w.begin(), [](char c) {
      return human_readable_index(c);
    });
  }

  word_type to_word(std::string const& s) {
    word_type w;
    to_word(w, s);
    return w;
  }

  // The following functions belong to the words namespace so as to only have
  // them apply when explicitly wanted.
  namespace words {
    word_type operator+(word_type const& u, word_type const& w) {
      word_type result(u);
      result.insert(result.end(), w.cbegin(), w.cend());
      return result;
    }

    word_type operator+(word_type const& u, size_t w) {
      word_type result(u);
      result.push_back(w);
      return result;
    }

    word_type operator+(size_t w, word_type const& u) {
      return word_type({w}) + u;
    }

    void operator+=(word_type& u, word_type const& v) {
      u.insert(u.end(), v.cbegin(), v.cend());
    }

    std::string pow(char const* w, size_t n) {
      return pow(std::string(w), n);
    }

    word_type pow(std::initializer_list<size_t> ilist, size_t n) {
      return pow(word_type(ilist), n);
    }

    word_type
    prod(std::initializer_list<size_t> ilist, int first, int last, int step) {
      return prod(word_type(ilist), first, last, step);
    }

  }  // namespace words
}  // namespace libsemigroups
