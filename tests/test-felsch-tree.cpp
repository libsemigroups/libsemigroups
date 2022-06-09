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

#include "catch.hpp"  // for REQUIRE, REQUIRE_THROWS_AS, REQUI...
#include "fpsemi-examples.hpp"
#include "test-main.hpp"  // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/felsch-tree.hpp"  // for FelschTree
#include "libsemigroups/present.hpp"      // for Presentation
#include "libsemigroups/report.hpp"       // for ReportGuard
#include "libsemigroups/types.hpp"        // for word_type

namespace libsemigroups {

  namespace detail {
    LIBSEMIGROUPS_TEST_CASE("FelschTree",
                            "000",
                            "previously failing example",
                            "[quick][low-index]") {
      auto                    rg = ReportGuard(false);
      Presentation<word_type> p;
      presentation::add_rule(p, {1, 2, 1}, {1, 1});
      presentation::add_rule(p, {3, 3}, {1, 1});
      presentation::add_rule(p, {1, 1, 1}, {1, 1});
      presentation::add_rule(p, {3, 2}, {2, 3});
      presentation::add_rule(p, {3, 1}, {1, 1});
      presentation::add_rule(p, {2, 2}, {2});
      presentation::add_rule(p, {1, 3}, {1, 1});
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
