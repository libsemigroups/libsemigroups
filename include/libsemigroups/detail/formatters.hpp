//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2023 James D. Mitchell
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

// This file contains various formatters for libsemigroups types.

#ifndef LIBSEMIGROUPS_DETAIL_FORMATTERS_HPP_
#define LIBSEMIGROUPS_DETAIL_FORMATTERS_HPP_

#include <magic_enum/magic_enum.hpp>

#include "containers.hpp"
#include "fmt.hpp"
#include "string.hpp"
#include "timer.hpp"

template <typename T, typename Char>
struct fmt::formatter<T, Char, std::enable_if_t<std::is_enum_v<T>>>
    : fmt::formatter<std::string> {
  template <typename FormatContext>
  auto format(T knd, FormatContext& ctx) const {
    auto name = magic_enum::enum_name(knd);
    return formatter<string_view>::format(name, ctx);
  }
};

template <>
struct fmt::formatter<libsemigroups::detail::Timer>
    : fmt::formatter<std::string> {
  template <typename FormatContext>
  auto format(libsemigroups::detail::Timer const& v, FormatContext& ctx) const {
    return formatter<string_view>::format(v.string(), ctx);
  }
};

#endif  // LIBSEMIGROUPS_DETAIL_FORMATTERS_HPP_
