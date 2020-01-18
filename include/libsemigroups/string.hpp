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

#ifndef LIBSEMIGROUPS_INCLUDE_STRING_HPP_
#define LIBSEMIGROUPS_INCLUDE_STRING_HPP_

#include <algorithm>  // for equal
#include <array>      // for array
#include <cstddef>    // for size_t
#include <sstream>    // for ostream, ostr...
#include <string>     // for string
#include <utility>    // for make_pair, pair
#include <vector>     // for vector

#include "libsemigroups-debug.hpp"  // for LIBSEMIGROUPS_ASSERT

namespace libsemigroups {

  namespace detail {
    // Forward declaration
    template <typename T>
    std::string to_string(const T& n);
  }  // namespace detail

  // A << function for vectors
  template <typename T>
  std::ostream& operator<<(std::ostream& os, std::vector<T> const& vec) {
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
    // TODO preprocess the next 3 functions out if using clang or gcc > 5
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

    template <typename... TArgs>
    struct Stringify;

    template <typename TFirst, typename... TArgs>
    struct Stringify<TFirst, TArgs...> final {
      std::string operator()(TFirst first, TArgs... args) const {
        return to_string(first) + Stringify<TArgs...>()(args...);
      }
    };

    template <>
    struct Stringify<> final {
      std::string operator()() const noexcept {
        return "";
      }
    };

    template <typename... TArgs>
    std::string stringify(TArgs... args) {
      return Stringify<TArgs...>()(args...);
    }

    static inline std::string wrap(size_t const indent, std::string& s) {
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
    static inline bool
    is_prefix(std::string::const_iterator const& first_word,
              std::string::const_iterator const& last_word,
              std::string::const_iterator const& first_prefix,
              std::string::const_iterator const& last_prefix) {
      LIBSEMIGROUPS_ASSERT(first_word <= last_word);
      // Check if [first_prefix, last_prefix) equals [first_word, first_word +
      // (last_suffix - first_suffix))
      return first_prefix <= last_prefix &&
             last_prefix - first_prefix <= last_word - first_word &&
             std::equal(first_prefix, last_prefix, first_word);
    }

    static inline std::pair<std::string::const_iterator,
                            std::string::const_iterator>
    maximum_common_prefix(std::string::const_iterator        first_word1,
                          std::string::const_iterator const& last_word1,
                          std::string::const_iterator        first_word2,
                          std::string::const_iterator const& last_word2) {
      while (*first_word1 == *first_word2 && first_word1 < last_word1
             && first_word2 < last_word2) {
        ++first_word1;
        ++first_word2;
      }
      return std::make_pair(first_word1, first_word2);
    }

#ifdef LIBSEMIGROUPS_DEBUG
    // Returns true if [first_suffix, last_suffix) is a suffix of [first_word,
    // last_word).
    static inline bool
    is_suffix(std::string::const_iterator const& first_word,
              std::string::const_iterator const& last_word,
              std::string::const_iterator const& first_suffix,
              std::string::const_iterator const& last_suffix) {
      LIBSEMIGROUPS_ASSERT(first_word <= last_word);
      // We don't care if first_suffix > last_suffix
      if (last_suffix - first_suffix > last_word - first_word) {
        return false;
      }

      // Check if [first_suffix, last_suffix) equals [last_word - (last_suffix -
      // first_suffix), end_word).
      // The following seems faster than calling std::equal.
      auto it_suffix = last_suffix - 1;
      auto it_word   = last_word - 1;
      while ((it_suffix > first_suffix) && (*it_suffix == *it_word)) {
        --it_suffix;
        --it_word;
      }
      return *it_suffix == *it_word;
    }
#endif
  }  // namespace detail
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_INCLUDE_STRING_HPP_
