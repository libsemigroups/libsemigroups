// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2022 Reinis Cirpons
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

// These tests are derived from:

#include <cstddef>  // for size_t
#include <cstdint>  // for uint64_t

#include "catch.hpp"                 // for LIBSEMIGROUPS_TEST_CASE
#include "libsemigroups/report.hpp"  // for ReportGuard
#include "libsemigroups/schreier-sims-helper.hpp"  // For schreier_sim_helper::intersection
#include "libsemigroups/schreier-sims.hpp"  // for SchreierSims, SchreierSims<>::ele...
#include "libsemigroups/transf.hpp"         // for Perm
#include "test-main.hpp"                    // for LIBSEMIGROUPS_TEST_CASE

namespace libsemigroups {
  struct LibsemigroupsException;

  constexpr bool REPORT = false;

  LIBSEMIGROUPS_TEST_CASE("SchreierSimsHelper",
                          "001",
                          "trivial perm. group intersection (degree 1)",
                          "[quick][schreier-sims-helper]") {
    auto            rg = ReportGuard(REPORT);
    SchreierSims<1> S, T, U;
    using Perm = decltype(S)::element_type;
    S.add_generator(Perm({0}));
    T.add_generator(Perm({0}));
    U = schreier_sims_helper::intersection(S, T);
    REQUIRE(U.size() == 1);
    REQUIRE(U.contains(Perm({0})));
  }

  LIBSEMIGROUPS_TEST_CASE("SchreierSimsHelper",
                          "002",
                          "trivial perm. group intersection (degree 2)",
                          "[quick][schreier-sims]") {
    auto            rg = ReportGuard(REPORT);
    SchreierSims<2> S, T, U;
    using Perm = SchreierSims<2>::element_type;
    S.add_generator(Perm({0, 1}));
    T.add_generator(Perm({1, 0}));
    U = schreier_sims_helper::intersection(S, T);
    REQUIRE(U.size() == 1);
    REQUIRE(U.sift(Perm({1, 0})) == Perm({1, 0}));
    REQUIRE(!U.contains(Perm({1, 0})));
    REQUIRE(U.contains(Perm({0, 1})));
  }
}  // namespace libsemigroups
