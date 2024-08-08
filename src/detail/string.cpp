// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2021 James D. Mitchell + Maria Tsalakou
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

#include "libsemigroups/detail/string.hpp"

#include <random>  // for mt19937

#include "libsemigroups/exception.hpp"  // for LIBSEMIGROUPS_EXCEPTION

#include "libsemigroups/detail/fmt.hpp"  // for group_digits

namespace libsemigroups {
  namespace detail {

    // Returns the string s to the power N, not optimized, complexity is O(N *
    // |s|)
    std::string power_string(std::string const& s, size_t N) {
      std::string result = s;
      for (size_t i = 0; i < N; ++i) {
        result += s;
      }
      return result;
    }

    namespace {
      std::string group_digits(uint64_t num) {
        return fmt::to_string(fmt::group_digits(num));
      }
    }  // namespace

    std::string group_digits(int64_t num) {
      if (num < 0) {
        return "-" + group_digits(static_cast<uint64_t>(-num));
      }
      return group_digits(static_cast<uint64_t>(num));
    }

    std::string signed_group_digits(int64_t num) {
      if (num < 0) {
        return "-" + group_digits(static_cast<uint64_t>(-num));
      }
      return "+" + group_digits(static_cast<uint64_t>(num));
    }
  }  // namespace detail
}  // namespace libsemigroups
