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

#include <string>  // for string
#include <vector>  // for vector

#include "test-main.hpp"  // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/detail/report.hpp"  // for ReportGuard
#include "libsemigroups/presentation.hpp"   // for Presentation
#include "libsemigroups/ranges.hpp"         // for iterator_range, to_vector

namespace libsemigroups {

  LIBSEMIGROUPS_TEST_CASE("TietzeAddGeneratorsRange",
                          "000",
                          "strings",
                          "[quick][presentation][tietze]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("ab");
    presentation::add_rule(p, "abab", "ba");

    // std::vector<std::string> candidates = {"ab", "ba"};
    // auto range = rx::iterator_range(candidates)
    //              | presentation::TietzeAddGeneratorsRange(p);

    // REQUIRE(range.size_hint() == 2);
    // REQUIRE((range | rx::count()) == 2);

    // auto const presentations = range | rx::to_vector();
    // REQUIRE(p.alphabet() == "ab");
    // REQUIRE(p.rules == std::vector<std::string>({"abab", "ba"}));

    // REQUIRE(presentations.size() == 2);
    // REQUIRE(presentations[0].alphabet() == "abc");
    // REQUIRE(presentations[0].rules
    //         == std::vector<std::string>({"cc", "ba", "c", "ab"}));

    // REQUIRE(presentations[1].alphabet() == "abc");
    // REQUIRE(presentations[1].rules
    //         == std::vector<std::string>({"acb", "c", "c", "ba"}));
  }

}  // namespace libsemigroups
