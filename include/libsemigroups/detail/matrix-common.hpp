//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2026 James D. Mitchell
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

#ifndef LIBSEMIGROUPS_DETAIL_MATRIX_COMMON_HPP_
#define LIBSEMIGROUPS_DETAIL_MATRIX_COMMON_HPP_

namespace libsemigroups::detail {
  // This function is required for exceptions and to_human_readable_repr, so
  // that if we encounter an entry of a matrix (Scalar type), then it can be
  // printed correctly. If we just did fmt::format("{}", val) and val ==
  // POSITIVE_INFINITY, but the type of val is, say, size_t, then this
  // wouldn't use the formatter for PositiveInfinity.
  //
  // Also in fmt v11.1.4 the custom formatter for POSITIVE_INFINITY and
  // NEGATIVE_INFINITY stopped working (and I wasn't able to figure out why)
  template <typename Scalar>
  [[nodiscard]] std::string entry_repr(Scalar a) {
    if constexpr (std::is_same_v<Scalar, NegativeInfinity>
                  || std::is_signed_v<Scalar>) {
      if (a == NEGATIVE_INFINITY) {
        return u8"-\u221E";
      }
    }
    if (a == POSITIVE_INFINITY) {
      return u8"+\u221E";
    }
    return fmt::format("{}", a);
  }

}  // namespace libsemigroups::detail

#include "matrix-common.tpp"

#endif  // LIBSEMIGROUPS_DETAIL_MATRIX_COMMON_HPP_
