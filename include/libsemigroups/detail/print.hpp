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

#include <string>  // for basic_string
#include <vector>  // for vector

#include "fmt.hpp"

namespace libsemigroups {
  namespace detail {
    [[nodiscard]] bool isprint(std::string const& alphabet);

    [[nodiscard]] std::string to_printable(char c);

    [[nodiscard]] std::string to_printable(char16_t c);

    [[nodiscard]] std::string to_printable(std::string const& alphabet);

    template <typename Int>
    [[nodiscard]] std::string
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
