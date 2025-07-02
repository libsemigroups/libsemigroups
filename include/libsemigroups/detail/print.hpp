//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2025 James D. Mitchell
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
#ifndef LIBSEMIGROUPS_DETAIL_PRINT_HPP_
#define LIBSEMIGROUPS_DETAIL_PRINT_HPP_

#include <algorithm>  // for all_of
#include <cctype>     // for isprint
#include <string>     // for basic_string

#include "fmt.hpp"

namespace libsemigroups {
  namespace detail {
    // TODO cpp file
    [[nodiscard]] bool static inline isprint(std::string const& alphabet) {
      return std::all_of(alphabet.cbegin(), alphabet.cend(), [](auto c) {
        return std::isprint(c);
      });
    }

    [[nodiscard]] static inline std::string to_printable(char c) {
      if (std::isprint(c)) {
        return fmt::format("\'{:c}\'", c);
      } else {
        return fmt::format("(char with value) {}", static_cast<int>(c));
      }
    }

    [[nodiscard]] static inline std::string
    to_printable(std::string const& alphabet) {
      if (isprint(alphabet)) {
        return fmt::format("\"{}\"",
                           std::string(alphabet.begin(), alphabet.end()));
      }
      LIBSEMIGROUPS_ASSERT(!alphabet.empty());

      int start = alphabet[0];
      // TODO could do this repeatedly to indicate multiple ranges
      if (alphabet.size() > 2
          && std::all_of(alphabet.begin(), alphabet.end(), [&start](int val) {
               return val == start++;
             })) {
        return fmt::format("(char values) [{}, ..., {}]",
                           static_cast<int>(alphabet[0]),
                           start - 1);
      }
      return fmt::format("(char values) {}",
                         std::vector<int>(alphabet.begin(), alphabet.end()));
    }

    template <typename Int>
    [[nodiscard]] static inline std::string
    to_printable(std::basic_string<Int> const& alphabet) {
      return fmt::format("{}",
                         std::vector<int>(alphabet.begin(), alphabet.end()));
    }

    template <typename Thing>
    [[nodiscard]] std::string to_printable(Thing const& thing) {
      return fmt::format("{}", thing);
    }
  }  // namespace detail
}  // namespace libsemigroups

#endif  // LIBSEMIGROUPS_DETAIL_PRINT_HPP_
