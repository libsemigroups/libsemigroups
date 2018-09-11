//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2018 James D. Mitchell
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

#ifndef LIBSEMIGROUPS_INCLUDE_STRING_HPP_
#define LIBSEMIGROUPS_INCLUDE_STRING_HPP_

#include <string>

namespace libsemigroups {
  namespace internal {
    //////////////////////////////////////////////////////////////////////////
    // Internal functions for handling strings
    //////////////////////////////////////////////////////////////////////////

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
      // We don't care if first_prefix > last_prefix
      if (last_prefix - first_prefix > last_word - first_word) {
        return false;
      }
      // Check if [first_prefix, last_prefix) equals [first_word, first_word +
      // (last_suffix - first_suffix))
      return std::equal(first_prefix, last_prefix, first_word);
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
  }  // namespace internal
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_INCLUDE_STRING_HPP_
