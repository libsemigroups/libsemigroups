//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019-2025 James D. Mitchell
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

// This file contains some string related functionality.

#ifndef LIBSEMIGROUPS_DETAIL_STRING_HPP_
#define LIBSEMIGROUPS_DETAIL_STRING_HPP_

#include <algorithm>  // for equal
#include <array>      // for array
#include <cstddef>    // for size_t
#include <cstdint>    // for int64_t
#include <cstdio>     // for snprintf
#include <cstring>    // for size_t, strlen
#include <iterator>   // for reverse_iterator
#include <memory>     // for unique_ptr
#include <sstream>    // for string, ostream, operator<<
#include <stdexcept>  // for runtime_error
#include <string>     // for string
#include <utility>    // for make_pair, pair
#include <vector>     // for vector

#include "libsemigroups/debug.hpp"  // for LIBSEMIGROUPS_ASSERT

namespace libsemigroups {

  namespace detail {

    void throw_if_nullptr(char const* w, std::string_view arg = "1st");

    // Forward declaration
    template <typename T>
    std::string to_string(T const& n);
  }  // namespace detail

  // A << function for vectors
  template <typename T>
  std::ostream& operator<<(std::ostream& os, std::vector<T> const& vec) {
    if (vec.empty()) {
      os << "{}";
      return os;
    }
    os << "{{";  // {{ is an escaped single { for fmt
    for (auto it = vec.cbegin(); it < vec.cend() - 1; ++it) {
      os << detail::to_string(*it) << ", ";
    }
    os << detail::to_string(*(vec.cend() - 1)) << "}}";
    return os;
  }

  // A << function for arrays
  template <typename T, size_t N>
  std::ostream& operator<<(std::ostream& os, std::array<T, N> const& rry) {
    os << "{{";
    for (auto it = rry.cbegin(); it < rry.cend() - 1; ++it) {
      os << detail::to_string(*it) << ", ";
    }
    os << detail::to_string(*(rry.cend() - 1)) << "}}";
    return os;
  }

  // A << function for pairs
  template <typename T, typename S>
  std::ostream& operator<<(std::ostream& os, std::pair<T, S> const& p) {
    os << "{{" << detail::to_string(p.first) << ", "
       << detail::to_string(p.second) << "}}";
    return os;
  }

  namespace detail {
    // TODO to cpp
    static inline std::string to_visible(char x) {
      if (x == '\n') {
        return std::string("\\n");
      } else if (x == '\r') {
        return std::string("\\r");
      } else if (x == '\b') {
        return std::string("\\b");
      } else if (x == '\t') {
        return std::string("\\t");
      } else if (x == '\a') {
        return std::string("\\a");
      } else if (x == '\v') {
        return std::string("\\v");
      } else if (x == '\0') {
        return std::string("\\0");
      }
      return std::string(x, 1);
    }

    // test_action doesn't compile if the function below is not present,
    // it complains that it doesn't know how to format PPerm's, I don't
    // understand the issue
    template <typename T>
    std::ostream& operator<<(std::ostream& os, std::vector<T> const& vec) {
      return ::libsemigroups::operator<<(os, vec);
    }
    // and test-knuth-bendix doesn't compile without this one
    template <typename T, typename S>
    std::ostream& operator<<(std::ostream& os, std::pair<T, S> const& p) {
      return ::libsemigroups::operator<<(os, p);
    }

    static inline size_t unicode_string_length(std::string const& s) {
      size_t count = 0;
      for (auto p = s.cbegin(); *p != 0; ++p) {
        count += ((*p & 0xc0) != 0x80);
      }
      return count;
    }

    size_t visible_length(std::string_view s);

    // Returns a string representing an object of type \c T.
    template <typename T>
    std::string to_string(T const& n) {
      std::ostringstream stm;
      stm << n;
      return stm.str();
    }

    // Returns true if [first_prefix, last_prefix) is a prefix of [first_word,
    // last_word).
    // Not noexcept, because std::equal isn't
    template <typename S, typename T>
    bool is_prefix(S first_word, S last_word, T first_prefix, T last_prefix) {
      LIBSEMIGROUPS_ASSERT(first_word <= last_word);
      // Check if [first_prefix, last_prefix) equals [first_word, first_word +
      // (last_prefix - first_prefix))
      if (!(first_prefix <= last_prefix)) {
        return false;
      } else if (!(last_prefix - first_prefix <= last_word - first_word)) {
        return false;
      } else if (!std::equal(first_prefix, last_prefix, first_word)) {
        return false;
      }
      return true;
    }

    template <typename Word>
    // Check if v is a prefix of u
    static inline bool is_prefix(Word const& u, Word const& v) {
      return is_prefix(u.cbegin(), u.cend(), v.cbegin(), v.cend());
    }

    // Check if v is a prefix of u
    static inline bool is_prefix(char const* u, char const* v) {
      return is_prefix(u, u + std::strlen(u), v, v + std::strlen(v));
    }

    template <typename S, typename T>
    std::pair<S, T> maximum_common_prefix(S first_word1,
                                          S last_word1,
                                          T first_word2,
                                          T last_word2) {
      // TODO there must be a better way of doing the static_casts in the next
      // line
      while (first_word1 < last_word1 && first_word2 < last_word2
             && static_cast<uint64_t>(*first_word1)
                    == static_cast<uint64_t>(*first_word2)) {
        ++first_word1;
        ++first_word2;
      }
      return std::make_pair(first_word1, first_word2);
    }

    template <typename T>
    std::pair<T, T> maximum_common_suffix(T first_word1,
                                          T last_word1,
                                          T first_word2,
                                          T last_word2) {
      auto p = maximum_common_prefix(std::make_reverse_iterator(last_word1),
                                     std::make_reverse_iterator(first_word1),
                                     std::make_reverse_iterator(last_word2),
                                     std::make_reverse_iterator(first_word2));
      return std::make_pair(p.first.base(), p.second.base());
    }

    template <typename W>
    static inline auto maximum_common_suffix(W const& u, W const& v) {
      return W(maximum_common_suffix(u.cbegin(), u.cend(), v.cbegin(), v.cend())
                   .first,
               u.cend());
    }

    static inline auto maximum_common_suffix(char const* u, char const* v) {
      return std::string(
          maximum_common_suffix(u, u + std::strlen(u), v, v + std::strlen(v))
              .first,
          u + std::strlen(u));
    }

    template <typename W>
    static inline auto maximum_common_prefix(W const& u, W const& v) {
      return maximum_common_prefix(u.cbegin(), u.cend(), v.cbegin(), v.cend());
    }

    // Returns true if [first_suffix, last_suffix) is a suffix of [first_word,
    // last_word).
    template <typename S, typename T>
    bool is_suffix(S first_word, S last_word, T first_suffix, T last_suffix) {
      return is_prefix(std::make_reverse_iterator(last_word),
                       std::make_reverse_iterator(first_word),
                       std::make_reverse_iterator(last_suffix),
                       std::make_reverse_iterator(first_suffix));
    }

    // Check if v is a suffix of u
    static inline bool is_suffix(std::string const& u, std::string const& v) {
      return is_suffix(u.cbegin(), u.cend(), v.cbegin(), v.cend());
    }

    // Returns the string s to the power N, not optimized, complexity is O(N *
    // |s|)
    // TODO move to word-range.hpp and/or remove
    std::string power_string(std::string const& s, size_t N);

    std::string group_digits(int64_t num);
    std::string signed_group_digits(int64_t num);

  }  // namespace detail
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_DETAIL_STRING_HPP_
