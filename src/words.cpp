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
#include <cctype>       // for isalpha, isdigit
#include <cmath>        // for pow
#include <cstring>      // for strlen
#include <limits>       // for iota
#include <numeric>      // for iota
#include <random>       // for std::mt19937
#include <stack>        // for iota
#include <string>       // for allocator
#include <type_traits>  // for is_destructible
#include <utility>      // for move

#include "libsemigroups/config.hpp"        // for LIBSEMIGROUPS_DEBUG
#include "libsemigroups/constants.hpp"     // for UNDEFINED
#include "libsemigroups/debug.hpp"         // for LIBSEMIGROUPS_ASSERT
#include "libsemigroups/exception.hpp"     // for word_type
#include "libsemigroups/order.hpp"         // for order
#include "libsemigroups/presentation.hpp"  // for index
#include "libsemigroups/ranges.hpp"        // for count, operator|
#include "libsemigroups/types.hpp"         // for word_type

#include "libsemigroups/detail/formatters.hpp"      // for magic_enum formatting
#include "libsemigroups/detail/word-iterators.hpp"  // for const_wilo_iterator

namespace libsemigroups {

  namespace detail {
    std::string const& chars_in_human_readable_order() {
      // Choose visible characters a-zA-Z0-9 first before anything else
      // The ascii ranges for these characters are: [97, 123), [65, 91),
      // [48, 58) so the remaining range of chars that are appended to the end
      // after these chars are [0,48), [58, 65), [91, 97), [123, 255]
      static std::string letters
          = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
      static bool first_call = true;
      if (first_call) {
        letters.resize(256);
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
                             == 1 + std::numeric_limits<char>::max()
                                    - std::numeric_limits<char>::min());
        LIBSEMIGROUPS_ASSERT(letters.end() == letters.begin() + 256);
      }
      return letters;
    }
  }  // namespace detail

  namespace words {
    size_t human_readable_index(char c) {
      static bool first_call = true;
      // It might be preferable to use an array here but char is sometimes
      // signed and so chars[i] can be negative in the loop below.
      static std::unordered_map<Presentation<std::string>::letter_type,
                                Presentation<word_type>::letter_type>
          map;
      if (first_call) {
        first_call        = false;
        auto const& chars = detail::chars_in_human_readable_order();
        for (letter_type i = 0; i < chars.size(); ++i) {
          map.emplace(chars[i], i);
        }
      }
      LIBSEMIGROUPS_ASSERT(map.size() == 256);

      auto it = map.find(c);
      // There are only 256 chars and so it shouldn't be possible that <c> is
      // not in the map.
      LIBSEMIGROUPS_ASSERT(it != map.cend());
      return it->second;
    }
  }  // namespace words

  ////////////////////////////////////////////////////////////////////////
  // 1. WordRange
  ////////////////////////////////////////////////////////////////////////

  namespace {
    uint64_t geometric_progression(size_t n, size_t a, size_t r) {
      LIBSEMIGROUPS_ASSERT(r != 1);  // to avoid division by 0
      return a * ((1 - std::pow(r, n)) / (1 - static_cast<float>(r)));
    }

    bool word_in_language(size_t n, word_type const& w) {
      return std::all_of(
          w.cbegin(), w.cend(), [&](letter_type x) { return x < n; });
    }
  }  // namespace

  uint64_t number_of_words(size_t n, size_t min, size_t max) {
    if (max <= min) {
      return 0;
    } else if (n == 1) {
      return max - min;
    }
    return geometric_progression(max, 1, n) - geometric_progression(min, 1, n);
  }

  word_type random_word(size_t length, size_t nr_letters) {
    static std::random_device rd;
    std::mt19937              mt(rd());

    if (nr_letters == 0) {
      LIBSEMIGROUPS_EXCEPTION(
          "the 2nd argument (number of letters) must be non-zero, found 0");
    }

    std::uniform_int_distribution<uint64_t> dist(0, nr_letters - 1);
    word_type                               out(length);
    std::generate(out.begin(), out.end(), [&dist, &mt]() { return dist(mt); });
    return out;
  }

  detail::const_wilo_iterator cbegin_wilo(size_t      n,
                                          size_t      upper_bound,
                                          word_type&& first,
                                          word_type&& last) {
    if (!word_in_language(n, first)
        || !lexicographical_compare(
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
        || !shortlex_compare(
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

  void WordRange::set_iterator() const {
    if (!_current_valid) {
      _current_valid = true;
      if (_order == Order::shortlex) {
        _current = cbegin_wislo(_alphabet_size, _first, _last);
        _end     = cend_wislo(_alphabet_size, _first, _last);
      } else if (_order == Order::lex) {
        _current = cbegin_wilo(_alphabet_size, _upper_bound, _first, _last);
        _end     = cend_wilo(_alphabet_size, _upper_bound, _first, _last);
      }
    }
  }

  size_t WordRange::count() const noexcept {
    if (_order == Order::shortlex) {
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
    _order         = Order::shortlex;
    _upper_bound   = 0;  // does nothing if _order is shortlex
    return *this;
  }

  WordRange::WordRange(WordRange const&)            = default;
  WordRange::WordRange(WordRange&&)                 = default;
  WordRange& WordRange::operator=(WordRange const&) = default;
  WordRange& WordRange::operator=(WordRange&&)      = default;
  WordRange::~WordRange()                           = default;

  WordRange& WordRange::order(Order val) {
    if (val != Order::shortlex && val != Order::lex) {
      LIBSEMIGROUPS_EXCEPTION(
          "the argument must be Order::shortlex or Order::lex, found {}", val);
    }
    _current_valid &= (val == _order);
    _order = val;
    return *this;
  }

  [[nodiscard]] std::string to_human_readable_repr(WordRange const& wr) {
    return fmt::format("<WordRange between {} and {} with letters in [0, {})>",
                       wr.first(),
                       wr.last(),
                       wr.alphabet_size());
  }

  ////////////////////////////////////////////////////////////////////////
  // 2. Strings -> Words
  ////////////////////////////////////////////////////////////////////////

  ToWord::ToWord(ToWord const&)            = default;
  ToWord::ToWord(ToWord&&)                 = default;
  ToWord& ToWord::operator=(ToWord const&) = default;
  ToWord& ToWord::operator=(ToWord&&)      = default;
  ToWord::~ToWord()                        = default;

  ToWord& ToWord::init(std::string const& alphabet) {
    if (alphabet.size() > 256) {
      LIBSEMIGROUPS_EXCEPTION(
          "The argument (alphabet) is too big, expected at most 256, found {}",
          alphabet.size());
    }
    auto _old_alphabet_map = _alphabet_map;
    init();
    LIBSEMIGROUPS_ASSERT(_alphabet_map.empty());
    for (letter_type l = 0; l < alphabet.size(); ++l) {
      auto it = _alphabet_map.emplace(alphabet[l], l);
      if (!it.second) {
        // Strong exception guarantee
        std::swap(_old_alphabet_map, _alphabet_map);
        LIBSEMIGROUPS_EXCEPTION("invalid alphabet {}, duplicate letter {}!",
                                detail::to_printable(alphabet),
                                detail::to_printable(alphabet[l]));
      }
    }
    return *this;
  }

  [[nodiscard]] std::string ToWord::alphabet() const {
    if (empty()) {
      return "";
    }
    std::string output(_alphabet_map.size(), '\0');
    for (auto it : _alphabet_map) {
      output[it.second] = it.first;
    }
    return output;
  }

  void ToWord::call_no_checks(word_type&         output,
                              std::string const& input) const {
    // Empty alphabet implies conversion should use human_readable_index
    if (empty()) {
      output.resize(input.size(), 0);
      std::transform(input.cbegin(), input.cend(), output.begin(), [](char c) {
        return words::human_readable_index(c);
      });
    } else {  // Non-empty alphabet implies conversion should use the alphabet.
      output.clear();
      output.reserve(input.size());
      for (auto const& c : input) {
        output.push_back(_alphabet_map.at(c));
      }
    }
  }

  void ToWord::operator()(word_type& output, std::string const& input) const {
    if (!empty()) {
      for (auto const& c : input) {
        if (_alphabet_map.find(c) == _alphabet_map.cend()) {
          LIBSEMIGROUPS_EXCEPTION(
              "invalid letter \'{}\' in the 2nd argument (input word), "
              "expected letters in the alphabet {}!",
              c,
              detail::to_printable(alphabet()));
        }
      }
    }
    call_no_checks(output, input);
  }

  ////////////////////////////////////////////////////////////////////////
  // 3. Words -> Strings
  ////////////////////////////////////////////////////////////////////////

  ToString::ToString(ToString const&)            = default;
  ToString::ToString(ToString&&)                 = default;
  ToString& ToString::operator=(ToString const&) = default;
  ToString& ToString::operator=(ToString&&)      = default;
  ToString::~ToString()                          = default;

  ToString& ToString::init(std::string const& alphabet) {
    if (alphabet.size() > 256) {
      LIBSEMIGROUPS_EXCEPTION(
          "The argument (alphabet) is too big, expected at most 256, found {}",
          alphabet.size());
    }
    auto _old_alphabet_map = _alphabet_map;
    init();
    LIBSEMIGROUPS_ASSERT(_alphabet_map.empty());
    for (letter_type i = 0; i < alphabet.size(); ++i) {
      auto it = _alphabet_map.emplace(i, alphabet[i]);
      if (!it.second) {
        std::swap(_old_alphabet_map, _alphabet_map);
        LIBSEMIGROUPS_EXCEPTION("invalid alphabet {}, duplicate letter {}!",
                                detail::to_printable(alphabet),
                                detail::to_printable(alphabet[i]));
      }
    }
    return *this;
  }

  [[nodiscard]] std::string ToString::alphabet() const {
    if (empty()) {
      return "";
    }

    std::string output(_alphabet_map.size(), '\0');
    for (auto it : _alphabet_map) {
      output[it.first] = it.second;
    }
    return output;
  }

  void ToString::call_no_checks(std::string&     output,
                                word_type const& input) const {
    // Empty alphabet implies conversion should use human_readable_index
    if (empty()) {
      output.resize(input.size(), 0);
      std::transform(
          input.cbegin(), input.cend(), output.begin(), [](letter_type c) {
            return words::human_readable_letter<>(c);
          });
    } else {  // Non-empty alphabet implies conversion should use the alphabet.
      output.clear();
      output.reserve(input.size());
      for (letter_type const& l : input) {
        output.push_back(_alphabet_map.at(l));
      }
    }
  }

  void ToString::operator()(std::string& output, word_type const& input) const {
    if (!empty()) {
      for (letter_type const& l : input) {
        if (_alphabet_map.find(l) == _alphabet_map.cend()) {
          LIBSEMIGROUPS_EXCEPTION(
              "invalid letter \'{}\' in the 2nd argument (input word), "
              "expected letters in the range  [0, {})!",
              l,
              _alphabet_map.size());
        }
      }
    }
    call_no_checks(output, input);
  }

  ////////////////////////////////////////////////////////////////////////
  // 4. StringRange
  ////////////////////////////////////////////////////////////////////////

  namespace detail {
    // This is not in an unnamed namespace because it is used by random_strings.
    // The random_strings return type is particularly nasty, and random_strings
    // should therefore probably remain an `auto inline` function defined in the
    // hpp file.
    void throw_if_random_string_should_throw(std::string const& alphabet,
                                             size_t             min,
                                             size_t             max) {
      if (min >= max) {
        LIBSEMIGROUPS_EXCEPTION(
            "the 2nd argument (min) must be less than the 3rd argument (max)");
      } else if (alphabet.empty() && min != 0) {
        LIBSEMIGROUPS_EXCEPTION("expected non-empty 1st argument (alphabet)");
      }
    }
  }  // namespace detail

  std::string random_string(std::string const& alphabet, size_t length) {
    static std::random_device       rd;
    static std::mt19937             generator(rd());
    std::uniform_int_distribution<> distribution(0, alphabet.size() - 1);

    std::string result;

    for (size_t i = 0; i < length; ++i) {
      result += alphabet[distribution(generator)];
    }

    return result;
  }

  // Random string with random length in the range [min, max) over <alphabet>
  std::string random_string(std::string const& alphabet,
                            size_t             min,
                            size_t             max) {
    detail::throw_if_random_string_should_throw(alphabet, min, max);
    if (max == min + 1) {
      return random_string(alphabet, min);
    }
    static std::random_device       rd;
    static std::mt19937             generator(rd());
    std::uniform_int_distribution<> distribution(min, max - 1);
    return random_string(alphabet, distribution(generator));
  }

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

  [[nodiscard]] std::string to_human_readable_repr(StringRange const& sr) {
    return fmt::format(
        "<StringRange between \"{}\" and \"{}\" with letters in \"{}\">",
        sr.first(),
        sr.last(),
        sr.alphabet());
  }

  ////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////

  namespace literals {
    word_type operator"" _w(char const* w, size_t n) {
      word_type result;
#ifdef LIBSEMIGROUPS_DEBUG
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
        } else if (97 <= w[i] && w[i] < 123) {
          if (mode == 0) {
            mode = 2;
          } else if (mode == 1) {
            LIBSEMIGROUPS_EXCEPTION("cannot mix numbers and letters, expected "
                                    "digits in 0123456789, found {}",
                                    w[i]);
          }
          result.push_back(words::human_readable_index(w[i]));
        } else {
          LIBSEMIGROUPS_EXCEPTION(
              "the argument contains the character \'{}\', expected only "
              "digits in 0123456789 or characters in \"a-zA-Z\"",
              detail::to_visible(w[i]));
        }
      }
      return result;
    }

    word_type operator"" _w(const char* w) {
      return operator"" _w(w, std::strlen(w));
    }

    namespace {
      // The next function implements the Shunting Yard Algorithm to convert
      // the expression in input to reverse Polish notation, as described
      // here: https://en.wikipedia.org/wiki/Shunting_yard_algorithm
      std::string shunting_yard(char const* input, size_t len) {
        std::string input_copy;
        if (len == 0) {
          return input_copy;
        }

        for (size_t i = 0; i < len - 1; ++i) {
          if (input[i] == '*') {
            LIBSEMIGROUPS_EXCEPTION(
                "Illegal character \'*\' in position {} of \"{}\"", i, input);
          }
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
              LIBSEMIGROUPS_EXCEPTION(
                  "Unmatched closing \')\' in position {} of \"{}\"",
                  i - std::count(input_copy.begin(), input_copy.end(), '*'),
                  input);
            }
            while (!ops.empty() && ops.top() != '(') {
              output += ops.top();
              ops.pop();
            }
            if (ops.empty()) {
              LIBSEMIGROUPS_EXCEPTION(
                  "Unmatched closing \')\' in position {} of \"{}\"",
                  i - std::count(input_copy.begin(), input_copy.end(), '*'),
                  input);
            }
            ops.pop();  // pop the '(' from the stack and discard
          } else if (input_copy[i] != ' ') {
            LIBSEMIGROUPS_EXCEPTION(
                "Illegal character \'{}\' in position {} of \"{}\"",
                input_copy[i],
                i - std::count(input_copy.begin(), input_copy.end(), '*'),
                input);
          }
        }
        while (!ops.empty()) {
          if (ops.top() == '(' || ops.top() == ')') {
            LIBSEMIGROUPS_EXCEPTION("Unmatched opening \'(\' in {}", input);
          }
          output += ops.top();
          ops.pop();
        }
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

      std::string inline evaluate_rpn(std::string const& rpn,
                                      std::string const& orig) {
        using namespace words;  // NOLINT(build/namespaces)
        std::stack<std::string>             stck;
        bool                                in_digits = false;
        std::pair<std::string, std::string> pr;

        for (auto const& term : rpn) {
          if (term == '^') {
            in_digits = false;
            if (try_pop_two(stck, pr)) {
              auto it = std::find_if_not(
                  pr.first.begin(), pr.first.end(), [](auto const& c) {
                    return std::isdigit(c);
                  });
              if (it != pr.first.end()) {
                LIBSEMIGROUPS_EXCEPTION(
                    "Incorrect arguments for operator \'^\', expected only "
                    "digits, found \"^{}\"  in \"{}\"",
                    *it,
                    orig);
              }
              stck.push(pow(pr.second, std::stol(pr.first)));
            } else {
              LIBSEMIGROUPS_EXCEPTION(
                  "Missing argument(s) for operator \'^\', "
                  "expected 2 arguments found {} in \"{}\"",
                  stck.empty() ? "0" : fmt::format("\"{}\"", stck.top()),
                  orig);
            }
          } else if (term == '*') {
            in_digits = false;
            if (try_pop_two(stck, pr)) {
              stck.push(pr.second + pr.first);
            } else {
              LIBSEMIGROUPS_EXCEPTION(
                  "Missing argument(s) for operator \'*\', "
                  "expected 2 arguments found {} in \"{}\"",
                  stck.empty() ? "0" : fmt::format("\"{}\"", stck.top()),
                  orig);
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
        std::string result;
        while (!stck.empty()) {
          result = stck.top() + result;
          stck.pop();
        }
        return result;
      }
    }  // namespace

    std::string operator""_p(char const* w, size_t n) {
      return evaluate_rpn(shunting_yard(w, n), w);
    }

    std::string operator""_p(char const* w) {
      return operator""_p(w, std::strlen(w));
    }
  }  // namespace literals
}  // namespace libsemigroups
