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

#include <array>             // for array
#include <cstdint>           // for uint8_t, uint32_t
#include <initializer_list>  // for initializer_list
#include <string>            // for string
#include <unordered_map>     // for unordered_map
#include <vector>            // for vector

#include "test-main.hpp"  // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/detail/report.hpp"  // for ReportGuard
#include "libsemigroups/exception.hpp"      // for find_duplicates

namespace libsemigroups {
  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("find_duplicates",
                                   "000",
                                   "exceptions",
                                   "[quick][exception]",
                                   std::vector<uint32_t>,
                                   std::string,
                                   (std::array<uint8_t, 16>),
                                   std::initializer_list<size_t>) {
    auto     rg  = ReportGuard(false);
    TestType vec = {0, 1, 12, 1, 13, 1, 3, 3, 13, 13, 1, 41, 4, 41, 14, 4};

    auto [it1, pos1] = detail::find_duplicates(vec.begin(), vec.end());
    REQUIRE(it1 == vec.begin() + 3);
    REQUIRE(pos1 == 1);
    REQUIRE(detail::has_duplicates(vec.begin(), vec.end()));

    auto [it2, pos2] = detail::find_duplicates(vec.begin(), vec.begin() + 3);
    REQUIRE(it2 == vec.begin() + 3);
    REQUIRE(pos2 == 3);
    REQUIRE(!detail::has_duplicates(vec.begin(), vec.begin() + 3));

    std::unordered_map<typename TestType::value_type, size_t> seen;
    auto [it3, pos3] = detail::find_duplicates(
        vec.begin(), vec.end(), seen, [](auto val) { return val == 1; });
    REQUIRE(it3 == vec.begin() + 7);
    REQUIRE(pos3 == 6);
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("throw_if_duplicates",
                                   "001",
                                   "exceptions",
                                   "[quick][exception]",
                                   std::vector<uint32_t>,
                                   (std::array<uint8_t, 16>),
                                   std::initializer_list<size_t>) {
    auto     rg  = ReportGuard(false);
    TestType vec = {0, 1, 12, 1, 13, 1, 3, 3, 13, 13, 1, 41, 4, 41, 14, 4};

    REQUIRE_EXCEPTION_MSG(
        detail::throw_if_duplicates(vec.begin(), vec.end(), "vector"),
        "duplicate vector value, found 1 in position "
        "3, first occurrence in position 1");
    REQUIRE_NOTHROW(
        detail::throw_if_duplicates(vec.begin(), vec.begin() + 3, "vector"));
  }

  LIBSEMIGROUPS_TEST_CASE("throw_if_duplicates",
                          "002",
                          "exceptions - std::string",
                          "[quick][exception]") {
    auto        rg  = ReportGuard(false);
    std::string vec = {0, 1, 12, 1, 13, 1, 3, 3, 13, 13, 1, 41, 4, 41, 14, 4};

    REQUIRE_EXCEPTION_MSG(
        detail::throw_if_duplicates(vec.begin(), vec.end(), "vector"),
        "duplicate vector value, found (char with value) 1 in position "
        "3, first occurrence in position 1");
    REQUIRE_NOTHROW(
        detail::throw_if_duplicates(vec.begin(), vec.begin() + 3, "vector"));
  }
}  // namespace libsemigroups
