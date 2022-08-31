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

#define CATCH_CONFIG_ENABLE_PAIR_STRINGMAKER

#include <cstdint>  // for uint8_t
#include <vector>   // for vector

#include "catch.hpp"      // for REQUIRE, REQUIRE_THROWS_AS, REQUI...
#include "test-main.hpp"  // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/constants.hpp"          // for UNDEFINED
#include "libsemigroups/digraph-helper.hpp"     // for make
#include "libsemigroups/digraph.hpp"            // for ActionDigraph
#include "libsemigroups/froidure-pin.hpp"       // for FroidurePin, Froidure...
#include "libsemigroups/make-froidure-pin.hpp"  // for make
#include "libsemigroups/report.hpp"             // for ReportGuard
#include "libsemigroups/transf.hpp"             // for Transf

namespace libsemigroups {

  LIBSEMIGROUPS_TEST_CASE("make<FroidurePin<Transf>>",
                          "000",
                          "from ActionDigraph",
                          "[quick][make]") {
    auto rg = ReportGuard(false);
    auto ad = action_digraph_helper::make<uint8_t>(
        5,
        {{1, 3, 4, 1}, {0, 0, 1, 1}, {2, 1, 2, 2}, {3, 2, 3, 3}, {4, 4, 4, 4}});
    auto S = make<FroidurePin<Transf<5>>>(ad);
    REQUIRE(S.size() == 625);
    auto T = make<FroidurePin<Transf<6>>>(ad);
    REQUIRE(T.size() == 625);
    auto U = make<FroidurePin<Transf<0, uint8_t>>>(ad);
    REQUIRE(U.size() == 625);
    auto V = make<FroidurePin<Transf<0, uint8_t>>>(ad, 4, 5);
    REQUIRE(V.size() == 1);
    auto W = make<FroidurePin<Transf<0, uint8_t>>>(ad, 0, 0);
    REQUIRE(W.size() == 1);
  }

  LIBSEMIGROUPS_TEST_CASE("make<FroidurePin<Transf>>",
                          "001",
                          "from ActionDigraph (exceptions)",
                          "[quick][make]") {
    auto rg = ReportGuard(false);
    auto ad = action_digraph_helper::make<uint8_t>(
        5,
        {{1, 3, 4, 1}, {0, 0, 1, 1}, {2, 1, 2, 2}, {3, 2, 3, 3}, {4, 4, 4, 4}});
    // Couldn't use REQUIRE_THROWS_AS here for some reason
    REQUIRE_THROWS(make<FroidurePin<Transf<0, uint8_t>>>(ad, 10, 0));
    REQUIRE_THROWS(make<FroidurePin<Transf<0, uint8_t>>>(ad, 10, 11));
    REQUIRE_THROWS(make<FroidurePin<Transf<0, uint8_t>>>(ad, 0, 11));
  }

}  // namespace libsemigroups
