//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019 James D. Mitchell
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

#ifndef LIBSEMIGROUPS_STRING_HPP_
#define LIBSEMIGROUPS_STRING_HPP_

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

#include "debug.hpp"  // for LIBSEMIGROUPS_ASSERT

namespace libsemigroups {

  namespace detail {
    static inline std::string const& string_format(std::string const& format) {
      return format;
    }

    template <typename... Args>
    std::string string_format(std::string const& format, Args... args) {
      size_t size = snprintf(nullptr, 0, format.c_str(), args...)
                    + 1;  // Extra space for '\0'
      if (size <= 0) {
        throw std::runtime_error("Error during formatting.");
      }
      std::unique_ptr<char[]> buf(new char[size]);
      snprintf(buf.get(), size, format.c_str(), args...);
      return std::string(
          buf.get(), buf.get() + size - 1);  // We don't want the '\0' inside
    }

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
    // TODO(later) preprocess the next 3 functions out if using clang or gcc > 5
    // A << function for vectors - to keep gcc 5 happy
    template <typename T>
    std::ostream& operator<<(std::ostream& os, std::vector<T> const& vec) {
      return ::libsemigroups::operator<<(os, vec);
    }

    // A << function for arrays - to keep gcc 5 happy
    template <typename T, size_t N>
    std::ostream& operator<<(std::ostream& os, std::array<T, N> const& rry) {
      return ::libsemigroups::operator<<(os, rry);
    }

    // A << function for pairs - to keep gcc 5 happy
    template <typename T, typename S>
    std::ostream& operator<<(std::ostream& os, std::pair<T, S> const& p) {
      return ::libsemigroups::operator<<(os, p);
    }

    static inline std::string wrap(size_t indent, std::string& s) {
      size_t      pos = 100 - indent;
      std::string prefix(indent, ' ');
      while (pos < s.size()) {
        auto last_space = s.find_last_of(' ', pos);
        if (last_space != std::string::npos) {
          s.replace(last_space, 1, "\n");
          s.insert(last_space + 1, prefix);
        }
        pos += (100 - indent);
      }
      return s;
    }

    static inline size_t unicode_string_length(std::string const& s) {
      size_t count = 0;
      for (auto p = s.cbegin(); *p != 0; ++p) {
        count += ((*p & 0xc0) != 0x80);
      }
      return count;
    }

    // Returns a string representing an object of type \c T.
    //
    // It appears that GCC 4.9.1 (at least) does not have std::to_string
    // implemented, so we implement our own. This requires the operator \c <<
    // to be implemented for an \c ostringstream& and const T& element.
    template <typename T>
    std::string to_string(T const& n) {
      std::ostringstream stm;
      stm << n;
      return stm.str();
    }

    // Replace [it1_begin .. it1_begin + (it2_end - it2_begin)] by
    // [it2_begin .. it2_end], no checks performed.
    static inline void string_replace(std::string::iterator       it1_begin,
                                      std::string::const_iterator it2_begin,
                                      std::string::const_iterator it2_end) {
      while (it2_begin < it2_end) {
        *it1_begin = *it2_begin;
        ++it1_begin;
        ++it2_begin;
      }
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

    // Check if v is a prefix of u
    static inline bool is_prefix(std::string const& u, std::string const& v) {
      return is_prefix(u.cbegin(), u.cend(), v.cbegin(), v.cend());
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

    // Random string with length <length> over <alphabet>.
    std::string random_string(std::string const& alphabet, size_t length);

    // Random string with random length in the range [min, max) over <alphabet>
    std::string random_string(std::string const& alphabet,
                              size_t             min,
                              size_t             max);

    std::vector<std::string> random_strings(std::string const& alphabet,
                                            size_t             number,
                                            size_t             min,
                                            size_t             max);

    // Returns the string s to the power N, not optimized, complexity is O(N *
    // |s|)
    std::string power_string(std::string const& s, size_t N);

    std::string group_digits(int64_t num);

  }  // namespace detail
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_STRING_HPP_
