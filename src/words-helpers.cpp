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
#include <vector>         // for vector
#include <iostream>

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

    // helpers for string compress
    // bucket helper (suffix array construction)
    void get_bound(std::vector<int> const& s, std::vector<int>& b, int k, bool tail) {
      int n = s.size();
      fill(b.begin(), b.end(), 0);
      for (int i = 0; i < n; i++) b[s[i]]++;
      for (int i = 0, sum = 0; i <= k; i++) {
        sum += b[i];
        b[i] = tail ? sum : sum - b[i];
      }
    }

    // implied S = 0, L = 1 as defined in sa_is (suffix array construction)
    void induced_sort(std::vector<int> const& s, std::vector<int>& sa, std::vector<bool> const& t, int k) {
      int n = s.size();
      std::vector<int> b(k + 1, 0);

      // l-type, insert at head
      get_bound(s, b, k, false);
      for (int i = 0; i < n; i++) {
        int prev = sa[i] - 1;
        if (prev >= 0 && t[prev] == 1) {
          sa[b[s[prev]]++] = prev;
        } 
      }

      // s-type, insert at tail
      get_bound(s, b, k, true);
      for (int i = n - 1; i >= 0; i--) {
        int prev = sa[i] - 1;
        if (prev >= 0 && t[prev] == 0) {
          sa[--b[s[prev]]] = prev;
        } 
      }
    }

    // SA-IS construction
    // decided higher memory overhead for indexing as opposed to pointer manipulation
    void sa_is(std::vector<int> const& s, std::vector<int>& sa, size_t k) {
      int n = s.size();
      const bool S_TYPE = 0;
      const bool L_TYPE = 1; 
      std::vector<bool> t(n, S_TYPE); 
      std::vector<int> lms; 

      // classify characters and find LMS characters
      t[n - 1] = S_TYPE;  // sentinel is immediately S_TYPE
      for (int i = n - 2; i >= 0; i--) {
        if (s[i] < s[i + 1]) t[i] = S_TYPE; 
        else if (s[i] > s[i + 1]) t[i] = L_TYPE;
        else t[i] = t[i + 1];
      }

      for (int i = 1; i < n; i++) {
        if (t[i] == S_TYPE && t[i - 1] == L_TYPE) lms.push_back(i);
      }

      // first pass: add LMS to end of buckets
      fill(sa.begin(), sa.end(), -1);
      std::vector<int> b(k + 1, 0);
      get_bound(s, b, k, true);
      for (int i = 0; i < (int)lms.size(); i++) {
        int lms_i = lms[i];
        sa[--b[s[lms_i]]] = lms_i;
      }
      induced_sort(s, sa, t, k);

      // assign lexicographical ranks to produce a reduced array
      int rank = 0;
      std::vector<int> ranked(n, -1); // sparse ranking look-up array
      std::vector<int> reduced;
      int prev = -1; 
      for (int i = 0; i < n; i++) {
        int curr = sa[i];

        if (curr > 0 && t[curr] == S_TYPE && t[curr - 1] == L_TYPE) {
          if (prev == -1) ranked[curr] = 0;
          else {
            int step = 0;
            while (true) {
              if (s[curr + step] == s[prev + step] && t[curr + step] == t[prev + step]) step++;
              else { rank++; break; }
              bool curr_lms = step > 0 && t[curr + step] == S_TYPE && t[curr + step - 1] == L_TYPE; 
              bool prev_lms = step > 0 && t[prev + step] == S_TYPE && t[prev + step - 1] == L_TYPE; 
              if (curr_lms && prev_lms) break;
              if (curr_lms || prev_lms) { rank++; break; }
            }
          }
          ranked[curr] = rank;
          prev = curr;
        }
      }
      for (int i = 0; i < (int)lms.size(); i++) {
        reduced.push_back(ranked[lms[i]]);
      }
      // recursive call with reduced array
      std::vector<int> r_sa(reduced.size(), 0);
      // base case: all reduced characters are unique, map to r_sa
      if (rank + 1 == (int)reduced.size()) {
        for (int i = 0; i < (int)reduced.size(); i++) {
          r_sa[reduced[i]] = i;
        }
      }
      else {
        sa_is(reduced, r_sa, rank + 1);
      }

      // substitute into final sa array
      fill(sa.begin(), sa.end(), -1);
      get_bound(s, b, k, true);
      for (int i = r_sa.size() - 1; i >= 0; i--) {
        int lms_i = lms[r_sa[i]];
        sa[--b[s[lms_i]]] = lms_i;
      }
      induced_sort(s, sa, t, k);
    }
    
    // int rmq(int l, int r, const std::vector<int>& height, const SparseTable& st) {

    // }
  }

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

    // namespace libsemigroups::detail::string_compress, specified for component testing
    namespace string_compress{
      // define sentinel as lexicographically smallest symbol
      std::vector<int> build_sa(std::string const& word) {
        int n = word.size();
        if (n == 0) return {0}; // empty string base case
        std::vector<int> s(n + 1, 0);
        std::vector<int> sa(n + 1, 0);

        for (int i = 0; i < n; i++) {
          s[i] = (unsigned char)word[i] + 1;
        }
        s[n] = 0; // sentinel $
        sa_is(s, sa, 257);
        return sa; 
      }

      std::vector<int> build_isa(std::vector<int> const& pos) {
        int n = pos.size();
        std::vector<int> isa(n);
        for (int i = 0; i < n; i++) {
          isa[pos[i]] = i;
        }
        return isa;
      }
      
      // defined in kasai 2001, stores length of longest common prefix between ADJACENT suffixes in sorted suffix array
      std::vector<int> get_height(std::vector<int> const& a, std::vector<int> const& pos, std::vector<int> const& rank) {
        int n = a.size();
        std::vector<int> height(n, 0);

        int h = 0;
        // iterate in order of appearance in original string
        for (int i = 0; i < n; i++) {
          // std::cout << "...current i = " << i << std::endl;
          if (rank[i] > 0) {
            int k = pos[rank[i] - 1];   // look at the left adjacent leaf
            while ((i + h) < n && (k + h) < n && a[i + h] == a[k + h]) {
              // std::cout << a[i + h] << " == " << a[k + h] << ", NOW H = " << h + 1 << std::endl;
              h++;
            }
            height[rank[i]] = h;
            if (h > 0) h = h - 1;
          }
        }
        return height;
      } 

      // returns length of lcp: tentative implementation O(n^2)
      int lcp(std::vector<int> const& isa, std::vector<int> const& height, int x, int z) {
        if (x == z) return 0;
        
        int rank_x = isa[x];
        int rank_z = isa[z];
        if (rank_x > rank_z) std::swap(rank_x, rank_z);

        int min = INT_MAX;
        for (int y = rank_x + 1; y <= rank_z; y++) {
          if (height[y] < min) {
            min = height[y]; 
          }
        }
        return min;
      }

      int lcs(std::vector<int> const& rev_isa, std::vector<int> const& rev_height, int x, int z) {
        return lcp(rev_isa, rev_height, rev_height.size() - x - 2, rev_height.size() - z - 2);
      }

      // helper for build_lyndon_arr invoking lcp
      bool is_lex_smaller(std::vector<int> const& w, std::vector<int> const& isa, std::vector<int> const& height, int i, int j, int i_v, int j_v, bool reverse) {
        int len = j - i + 1;
        int len_v = j_v - i_v + 1;

        int lcp_len = lcp(isa, height, i, i_v);
        if (lcp_len < len && lcp_len < len_v) {
          if (reverse) return w[i + lcp_len] > w[i_v + lcp_len]; // compare first different character, sentinel = 0 guarantees no equivalence
          return w[i + lcp_len] < w[i_v + lcp_len];
        }
        return len < len_v; // shorter string is lexicographically smaller
      }

      // outputs lyndon array with longest lyndon word at each index (storing end index j at i)
      std::vector<int> build_lyndon_arr(std::vector<int> const& w, std::vector<int> const& isa, std::vector<int> const& height, bool reverse) {
        int n = w.size();
        std::vector<int> ly_arr(n);
        std::stack<std::pair<int,int>> S;

        ly_arr[n - 1] = n - 1;
        S.push({n - 1, n - 1}); // push sentinel

        for (int i = n - 2; i >= 0; i--) {
          int j = i;
          while (!S.empty()) {
            int i_v = S.top().first;
            int j_v = S.top().second;
            if (!is_lex_smaller(w, isa, height, i, j, i_v, j_v, reverse)) break; // fails merge
            j = j_v;  // merge string uv using previously established lyndon word at right (top of stack)
            S.pop();
          }
          S.push({i, j});
          ly_arr[i] = j;
        }
        return ly_arr;
      }

      std::vector<Run> max_runs(std::vector<int> const& isa, std::vector<int> const& height, 
                                std::vector<int> const& rev_isa, std::vector<int> const& rev_height, 
                                std::vector<int>& ly_arr) {
        std::vector<Run> runs; 
        int n = isa.size();

        for (int i = 0; i < n - 1; i++) {
          int j = ly_arr[i];
          int p = j - i + 1; 
          int i_prime; 
          int j_prime; 
          
          i_prime = (i > 0) ? i - lcs(rev_isa, rev_height, i - 1, j) : i; // candidate left extension
          j_prime = (j < n - 1) ? j + lcp(isa, height, i, j + 1): j;      // candidate right extension
          std::cout << "~~~~~~~~~testing i = " << i << ", i_prime = " << i_prime << ", j_prime = " << j_prime << std::endl;
          if ((j_prime - i_prime + 1) >= 2*p && i_prime <= i && i <= i_prime + p) {
            std::cout << "adding run @ i = " << i << ", (" << i_prime << ", " << j_prime << ", " << p << ")" << std::endl;
            runs.push_back({i_prime, j_prime, p});
          }
        }
        return runs;          
      }

      /**
       * We choose a policy that outputs the shortest expression of form (x)^e. 
       * This is executed as a dp-style algorithm maximising compression with
       *    compression factor (cf) = length - period
       * 
       * We accept only alpha characters, else exception. 
       * Inverses (uppercase characters) are currently considered lexicographically smaller by way of ASCII mapping
       */
      std::string to_expression(std::string const& w, int n) {
        // input format validation
        if (w == "") return "";   // TODO: see if i need another empty string check modeling build_sa
        for (char const& c: w) {
          if (!std::isalpha(c)) {
            LIBSEMIGROUPS_EXCEPTION("expects string argument with alphabet \"a-zA-Z\", "
                                    "all other symbols invalid");
          }
        }

        // map string to ASCII value, append sentinel, and build forward suffix array
        // alphabet size is 256 + 1 (sentinel)
        std::vector<int> mapped_w(n + 1, 0);
        std::vector<int> sa(n + 1, 0);
        std::vector<int> height(n + 1, 0);
        for (int i = 0; i < n; i++) {
          mapped_w[i] = (unsigned char)w[i] + 1;
        }
        mapped_w[n] = 0; // sentinel $
        sa_is(mapped_w, sa, 257);
        std::cout << std::endl << "sa = {";
        for (auto e: sa) {
          std::cout << e << ", ";
        }
        std::cout << "}" << std::endl;
        std::vector<int> isa = build_isa(sa);
        height = get_height(mapped_w, sa, isa);
        std::cout << "height = {";
        for (auto e: height) {
          std::cout << e << ", ";
        }
        std::cout << "}" << std::endl;

        // construct same structures for the reverse string
        std::vector<int> rev_mw(n + 1, 0); 
        std::vector<int> rev_sa(n + 1, 0);
        std::vector<int> rev_height(n + 1, 0);
        for (int i = 0; i < n; i++) {
          rev_mw[n - i - 1] = mapped_w[i];
        }
        rev_mw[n] = 0;
        std::cout << "ORIGINAL STRING SANITY CHECK: " << mapped_w << std::endl;
        std::cout << "REVERSE STRING SANITY CHECK: " << rev_mw << std::endl;
        sa_is(rev_mw, rev_sa, 257);
        std::vector<int> rev_isa = build_isa(rev_sa); 
        rev_height = get_height(rev_mw, rev_sa, rev_isa);

        std::vector<int> ly_arr = build_lyndon_arr(mapped_w, isa, height, false);
        std::vector<int> rev_ly_arr = build_lyndon_arr(mapped_w, isa, height, true);
        std::cout << "ly_arr = {";
        for (auto e: ly_arr) {
          std::cout << e << ", ";
        }
        std::cout << "}" << std::endl;
        std::cout << "REVVVVly_arr = {";
        for (auto e: rev_ly_arr) {
          std::cout << e << ", ";
        }
        std::cout << "}" << std::endl;
        std::vector<Run> runs1 = max_runs(isa, height, rev_isa, rev_height, ly_arr);
        std::vector<Run> runs2 = max_runs(isa, height, rev_isa, rev_height, rev_ly_arr);

        // bucket by run end index
        std::vector<std::vector<Run>> runs_by_end(n);
        for (auto const& r: runs1) {
          runs_by_end[r.j].push_back(r);
        }
        for (auto const& r: runs2) {
          runs_by_end[r.j].push_back(r);
        }
        
        // 1D DP table storing (compression factor, run)
        auto cf = [](Run const& r) { return (r.j - r.i + 1) - r.p; };
        std::vector<std::pair<int, Run>> dp(n, {0, Run()});
        for (int i = 0; i < n; i++) {
          dp[i] = (i > 0) ? dp[i - 1] : std::pair<int, Run>{0, Run()};
          for (auto const& r: runs_by_end[i]) {
            int prev = (r.i > 0) ? dp[r.i - 1].first : 0;
            int candidate = prev + cf(r);
            if (candidate > dp[i].first) {
              dp[i] = {candidate, r}; 
            }
          }
        }

        // build final string by traversing dp array backwards
        std::string result = "";
        std::stack<Run> selected;
        int p = n - 1; 
        while (p >= 0) {
          Run r = dp[p].second;
          if (r.p > 0) {
            selected.push(r);
            p = r.i - 1;
          }
          else p--;
        }

        p = 0;
        while (p < n) {
          if (!selected.empty() && p == selected.top().i) {
            Run r = selected.top();
            int exponent = (r.j - r.i + 1) / r.p;
            result += "(" + w.substr(r.i, r.p) + ")^" + std::to_string(exponent);
            p = r.i + (r.p * exponent); 
            selected.pop();
          }
          else {
            result += w[p];
            p++;
          }
        }
        return result;
      }
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
      return detail::string_compress::to_expression(w, n);
    }

    std::string operator""_c(char const* w) {
      return operator""_c(w, std::strlen(w));
    }
  }  // namespace literals
}  // namespace libsemigroups
