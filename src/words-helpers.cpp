//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019-2026 James Mitchell + Joseph Edwards
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
// words-helpers.hpp

#include "libsemigroups/words-helpers.hpp"

#include <cctype>         // for isalpha, isdigit
#include <cmath>          // for pow
#include <cstring>        // for strlen
#include <numeric>        // for iota
#include <random>         // for std::mt19937
#include <stack>          // for stack
#include <unordered_map>  // for unordered_map, operator==
#include <utility>        // for move
#include <vector> 
#include <algorithm>
#include <stack> 

#include "libsemigroups/config.hpp"        // for LIBSEMIGROUPS_DEBUG
#include "libsemigroups/presentation.hpp"  // for index

#include "libsemigroups/detail/formatters.hpp"  // for magic_enum formatting

namespace libsemigroups {
  namespace {
    uint64_t geometric_progression(size_t n, size_t a, size_t r) {
      LIBSEMIGROUPS_ASSERT(r != 1);  // to avoid division by 0
      return a * ((1 - std::pow(r, n)) / (1 - static_cast<float>(r)));
    }

    bool is_op(const char val) {
      return std::string_view{",*^"}.find(val) != std::string_view::npos;
    }

    // Check op1 < op2 with respect to the order , < * < ^
    bool compare_ops(const char op1, const char op2) {
      LIBSEMIGROUPS_ASSERT(is_op(op1));
      LIBSEMIGROUPS_ASSERT(is_op(op2));
      std::string_view ops{",*^"};
      return ops.find(op1) < ops.find(op2);
    }

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
        // Add a <*> after input[i] if input[i] is:
        //  - a digit or letter followed by a letter or an open bracket; or
        //  - a close bracket followed by a letter
        if (((std::isdigit(input[i]) || std::isalpha(input[i]))
             && (std::isalpha(input[i + 1]) || input[i + 1] == '('))

            || (input[i] == ')' && std::isalpha(input[i + 1]))) {
          input_copy += "*";
        }
      }
      input_copy += input[len - 1];

      std::string      output;
      std::stack<char> ops;

      for (size_t i = 0; i < input_copy.size(); ++i) {
        if (std::isalpha(input_copy[i]) || (std::isdigit(input_copy[i]))) {
          output += input_copy[i];
        } else if (input_copy[i] == '(') {
          ops.push(input_copy[i]);
        } else if (is_op(input_copy[i])) {
          while (!ops.empty() && ops.top() != '('
                 && compare_ops(input_copy[i], ops.top())) {
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

    std::string get_unique_letters(std::string const& x) {
      std::unordered_set<char> seen(x.begin(), x.end());
      return std::string(seen.begin(), seen.end());
    }

    std::string swap_case(std::string s) {
      std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) {
        if (std::isupper(c)) {
          return std::tolower(c);
        }
        return std::toupper(c);
      });
      return s;
    }

    std::string evaluate_rpn(std::string const& rpn, std::string const& orig) {
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
        } else if (term == ',') {
          in_digits = false;
          if (try_pop_two(stck, pr)) {
            for (std::string word : {pr.first, pr.second}) {
              auto it = std::find_if_not(
                  word.begin(), word.end(), [](auto const& c) {
                    return std::isalpha(c);
                  });
              if (it != word.end()) {
                LIBSEMIGROUPS_EXCEPTION(
                    "Incorrect arguments for operator \',\', expected only "
                    "letters, found \"^{}\"  in \"{}\"",
                    *it,
                    orig);
              }
            }
            std::string alphabet = get_unique_letters(pr.second + pr.first);
            stck.push(presentation::commutator_no_checks(
                pr.second, pr.first, alphabet, swap_case(alphabet)));
          } else {
            LIBSEMIGROUPS_EXCEPTION(
                "Missing argument(s) for operator \',\', "
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

    // bucket helper
    void get_bound(std::vector<int> const& s, std::vector<int>& bucket, int k, bool tail) {
      int n = s.size();
      fill(bucket.begin(), bucket.end(), 0);
      for (int i = 0; i < n; i++) bucket[s[i]]++;
      for (int i = 0, sum = 0; i <= k; i++) {
        sum += bucket[i];
        bucket[i] = tail ? sum : sum - bucket[i];
      }
    }

    // implied S = 0, L = 1 as defined in sa_is
    void sa_pass(std::vector<int> const& s, std::vector<int>& sa, std::vector<bool> const& labeled, int k) {
      int n = s.size();
      std::vector<int> bucket(k + 1, 0);

      // l-type, insert at head
      get_bound(s, bucket, k, false);
      for (int i = 0; i < n; i++) {
        int prev = sa[i] - 1;
        if (prev >= 0 && labeled[prev] == 1) {
          sa[bucket[s[prev]]++] = prev;
        } 
      }

      // s-type, insert at tail
      get_bound(s, bucket, k, true);
      for (int i = n - 1; i >= 0; i--) {
        int prev = sa[i] - 1;
        if (prev >= 0 && labeled[prev] == 0) {
          sa[--bucket[s[prev]]] = prev;
        } 
      }
    }

    // an upheaval: SA-IS construction
    void sa_is(std::vector<int> const& s, std::vector<int>& sa, size_t k) {
      size_t n = s.size();
      const bool S_TYPE = 0;
      const bool L_TYPE = 1; 
      std::vector<bool> labeled(n, S_TYPE); 
      std::vector<int> lms; 

      // classify characters and find LMS characters
      labeled[n - 1] = S_TYPE;
      for (int i = n - 2; i >= 0; i--) {
        if (s[i] < s[i + 1]) labeled[i] = S_TYPE; 
        else if (s[i] > s[i + 1]) labeled[i] = L_TYPE;
        else labeled[i] = labeled[i + 1];
      }

      for (int i = 1; i < n; i++) {
        if (labeled[i] == S_TYPE && labeled[i - 1] == L_TYPE) lms.push_back(i);
      }

      // first pass: add LMS to end of buckets
      fill(sa.begin(), sa.end(), -1);
      std::vector<int> bucket(k + 1, 0);
      get_bound(s, bucket, k, true);
      for (int i = 0; i < lms.size(); i++) {
        int lms_i = lms[i];
        sa[--bucket[s[lms_i]]] = lms_i;
      }
      sa_pass(s, sa, labeled, k);

      // assign lexicographical ranks to produce a reduced array
      int rank = 0;
      std::vector<int> ranked(n, -1); // sparse ranking look-up array
      std::vector<int> reduced;
      int curr, prev = -1; 
      for (int i = 0; i < n; i++) {
        int curr = sa[i];

        if (curr > 0 && labeled[curr] == S_TYPE && labeled[curr - 1] == L_TYPE) {
          if (prev == -1) ranked[curr] = 0;
          else {
            int step = 0;
            while (true) {
              if (s[curr + step] == s[prev + step] && labeled[curr + step] == labeled[prev + step]) step++;
              else { rank++; break; }
              bool curr_lms = step > 0 && labeled[curr + step] == S_TYPE && labeled[curr + step - 1] == L_TYPE; 
              bool prev_lms = step > 0 && labeled[prev + step] == S_TYPE && labeled[prev + step - 1] == L_TYPE; 
              if (curr_lms && prev_lms) break;
              if (curr_lms || prev_lms) { rank++; break; }
            }
          }
          ranked[curr] = rank;
          prev = curr;
        }
      }
      for (int i = 0; i < lms.size(); i++) {
        reduced.push_back(ranked[lms[i]]);
      }
      // recursive call with reduced array
      std::vector<int> r_sa(reduced.size(), 0);
      // base case: all reduced characters are unique, map to r_sa
      if (rank + 1 == reduced.size()) {
        for (int i = 0; i < reduced.size(); i++) {
          r_sa[reduced[i]] = i;
        }
      }
      else {
        sa_is(reduced, r_sa, rank + 1);
      }

      // substitute into final sa array
      fill(sa.begin(), sa.end(), -1);
      get_bound(s, bucket, k, true);
      for (int i = r_sa.size() - 1; i >= 0; i--) {
        int lms_i = lms[r_sa[i]];
        sa[--bucket[s[lms_i]]] = lms_i;
      }
      sa_pass(s, sa, labeled, k);
    }

    std::vector<int> build_sa(std::string const& word) {
      int n = word.length();
      std::vector<int> s(n + 1, 0);
      std::vector<int> sa(n + 1, 0);

      for (int i = 0; i < n; i++) {
        s[i] = (unsigned char)word[i] + 1;
      }
      s[n] = 0; // sentinel $
      sa_is(s, sa, 256);
      
      return sa; 
    }

    // // substring type helper (start index, string length)
    // struct Substring {
    //   size_t start;
    //   size_t length;
      
    //   size_t end() const noexcept {
    //     return start + length;
    //   }
    //   Substring() = default;
    //   Substring(size_t s, size_t l) : start(s), length(l) {}
    // };

    // // outputs non-increasing list of lyndon factors (maximal runs)
    // // firstmost element is longest lyndon prefix
    // std::vector<Substring> lyndon_factorize(std::string const& s) {
    //   std::stack<Substring> factors;
    //   int n = s.size();
    //   for (int i = n - 1; i >= 0; i--) {
    //     while (factors.size() > 1) {
    //       Substring j = factors.top();
    //       factors.pop();
    //     }
    //     factors.push();
    //   } 
    // }

    // // optimize for ideal final expression
    // // policy: select in descending order for longest factor
    // std::string factors_to_expression(std::vector<Substring> const& factors) {

    // }

  }  // namespace

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

  ////////////////////////////////////////////////////////////////////////
  // Words
  ////////////////////////////////////////////////////////////////////////

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

  namespace words {

    std::string parse(std::string const& w) {
      return literals::operator""_p(w.c_str(), w.size());
    }

    std::string compress(std::string const& w) {
      return literals::operator""_c(w.c_str(), w.size());
    }

    word_type operator+(word_type const& u, word_type const& w) {
      word_type result(u);
      result.insert(result.end(), w.cbegin(), w.cend());
      return result;
    }

    word_type operator+(word_type const& u, letter_type w) {
      word_type result(u);
      result.push_back(w);
      return result;
    }

    word_type operator+(letter_type w, word_type const& u) {
      return word_type({w}) + u;
    }

    word_type pow(std::initializer_list<letter_type> ilist, size_t n) {
      return pow(word_type(ilist), n);
    }

    std::string pow(std::string_view w, size_t n) {
      return pow(std::string(w), n);
    }

    size_t human_readable_index(char c) {
      static bool first_call = true;
      // It might be preferable to use an array here but char is sometimes
      // signed and so chars[i] can be negative in the loop below.
      static std::unordered_map<Presentation<std::string>::native_letter_type,
                                Presentation<word_type>::native_letter_type>
          map;
      if (first_call) {
        first_call        = false;
        auto const& chars = detail::chars_in_human_readable_order();
        for (Presentation<word_type>::native_letter_type i = 0;
             i < chars.size();
             ++i) {
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
  // Literals
  ////////////////////////////////////////////////////////////////////////

  namespace literals {
    word_type operator""_w(char const* w, size_t n) {
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

    word_type operator""_w(const char* w) {
      return operator""_w(w, std::strlen(w));
    }

    std::string operator""_p(char const* w, size_t n) {
      return evaluate_rpn(shunting_yard(w, n), w);
    }

    std::string operator""_p(char const* w) {
      return operator""_p(w, std::strlen(w));
    }

    std::string operator""_c(char const* w, size_t n) {
      // TODO
      return "";
    }

    std::string operator""_c(char const* w) {
      return operator""_c(w, std::strlen(w));
    }
  }  // namespace literals
}  // namespace libsemigroups
