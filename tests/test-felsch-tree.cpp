//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2022 James D. Mitchell
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

#include "Catch2-3.7.1/catch_amalgamated.hpp"  // for REQUIRE, REQUIRE_THROWS_AS, REQUI...
#include "test-main.hpp"                       // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/presentation.hpp"  // for Presentation
#include "libsemigroups/types.hpp"         // for word_type

#include "libsemigroups/detail/felsch-tree.hpp"  // for FelschTree
#include "libsemigroups/detail/report.hpp"       // for ReportGuard

namespace libsemigroups {
  using literals::operator""_w;

  namespace detail {
    LIBSEMIGROUPS_TEST_CASE("FelschTree",
                            "000",
                            "previously failing example",
                            "[quick][low-index]") {
      auto                    rg = ReportGuard(false);
      Presentation<word_type> p;
      presentation::add_rule_no_checks(p, 121_w, 11_w);
      presentation::add_rule_no_checks(p, 33_w, 11_w);
      presentation::add_rule_no_checks(p, 111_w, 11_w);
      presentation::add_rule_no_checks(p, 32_w, 23_w);
      presentation::add_rule_no_checks(p, 31_w, 11_w);
      presentation::add_rule_no_checks(p, 22_w, 2_w);
      presentation::add_rule_no_checks(p, 13_w, 11_w);
      p.alphabet_from_rules();
      p.validate();

      FelschTree ft(4);
      ft.add_relations(p.rules.cbegin(), p.rules.cend());

      REQUIRE(ft.number_of_nodes() == 15);
      REQUIRE(ft.height() == 3);
      ft.push_back(0);
      REQUIRE(std::vector<word_type>(ft.cbegin(), ft.cend())
              == std::vector<word_type>());
    }
  }  // namespace detail
}  // namespace libsemigroups
