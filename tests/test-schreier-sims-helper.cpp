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
#include <iostream>

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
    schreier_sims_helper::intersection(U, S, T);
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
    schreier_sims_helper::intersection(U, S, T);
    REQUIRE(U.size() == 1);
    REQUIRE(U.sift(Perm({1, 0})) == Perm({1, 0}));
    REQUIRE(!U.contains(Perm({1, 0})));
    REQUIRE(U.contains(Perm({0, 1})));
  }

  LIBSEMIGROUPS_TEST_CASE("SchreierSimsHelper",
                          "003",
                          "cyclic group intersection (degree 13)",
                          "[quick][schreier-sims]") {
    auto             rg = ReportGuard(REPORT);
    SchreierSims<13> S, T, U;
    using Perm = SchreierSims<13>::element_type;
    // Adapted from:
    // https://math.stackexchange.com/q/4093199/152276
    // (0, 1, 2, 3, 4)(5, 9)(6, 10)(7, 11)(8, 12)
    S.add_generator(Perm({1, 2, 3, 4, 0, 9, 10, 11, 12, 5, 6, 7, 8}));
    // (1, 4)(2, 3)(5, 6, 7, 8, 9, 10, 11, 12)
    T.add_generator(Perm({0, 4, 3, 2, 1, 6, 7, 8, 9, 10, 11, 12, 5}));
    schreier_sims_helper::intersection(U, S, T);
    REQUIRE(U.size() == 2);
    REQUIRE(!U.contains(Perm({1, 0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12})));
    REQUIRE(U.contains(Perm({0, 1, 2, 3, 4, 9, 10, 11, 12, 5, 6, 7, 8})));
  }

  LIBSEMIGROUPS_TEST_CASE("SchreierSimsHelper",
                          "004",
                          "D10 and Z5 intersection",
                          "[quick][schreier-sims]") {
    auto             rg = ReportGuard(REPORT);
    SchreierSims<12> S, T, U;
    using Perm = SchreierSims<12>::element_type;
    // (0, 2, 4, 6, 8)(1, 3, 5, 7, 9)(10, 11)
    S.add_generator(Perm({2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 11, 10}));
    // (0, 2, 4, 6, 8)(1, 3, 5, 7, 9)
    // (0, 1)(2, 9)(3, 8)(4, 7)(5, 6)
    T.add_generator(Perm({2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 10, 11}));
    T.add_generator(Perm({1, 0, 9, 8, 7, 6, 5, 4, 3, 2, 10, 11}));
    schreier_sims_helper::intersection(U, S, T);
    REQUIRE(U.size() == 5);
    // (0, 6, 2, 8, 4)(1, 7, 3, 9, 5)
    REQUIRE(U.contains(Perm({6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 10, 11})));
  }

  LIBSEMIGROUPS_TEST_CASE("SchreierSimsHelper",
                          "005",
                          "D8 and Q8 intersection",
                          "[quick][schreier-sims]") {
    auto            rg = ReportGuard(REPORT);
    SchreierSims<8> S, T, U;
    using Perm = SchreierSims<8>::element_type;
    S.add_generator(Perm({1, 3, 7, 5, 2, 0, 4, 6}));
    S.add_generator(Perm({2, 4, 3, 6, 5, 7, 0, 1}));
    S.add_generator(Perm({3, 5, 6, 0, 7, 1, 2, 4}));
    T.add_generator(Perm({1, 0, 7, 5, 6, 3, 4, 2}));
    T.add_generator(Perm({2, 4, 3, 6, 5, 7, 0, 1}));
    T.add_generator(Perm({3, 5, 6, 0, 7, 1, 2, 4}));
    schreier_sims_helper::intersection(U, S, T);
    REQUIRE(U.size() == 4);
    REQUIRE(U.contains(Perm({2, 4, 3, 6, 5, 7, 0, 1})));
  }

  LIBSEMIGROUPS_TEST_CASE("SchreierSimsHelper",
                          "006",
                          "primitive on 8 points intersection",
                          "[quick][schreier-sims]") {
    auto            rg = ReportGuard(REPORT);
    SchreierSims<8> S, T, U;
    using Perm = SchreierSims<8>::element_type;
    S.add_generator(Perm({0, 2, 3, 4, 1, 5, 6, 7}));
    S.add_generator(Perm({1, 2, 4, 0, 3, 5, 6, 7}));
    T.add_generator(Perm({1, 2, 3, 4, 5, 6, 0, 7}));
    T.add_generator(Perm({0, 1, 2, 3, 4, 6, 7, 5}));
    schreier_sims_helper::intersection(U, S, T);
    REQUIRE(U.size() == 10);
    REQUIRE(U.contains(Perm({0, 3, 4, 1, 2, 5, 6, 7})));
    REQUIRE(U.contains(Perm({1, 2, 4, 0, 3, 5, 6, 7})));
  }

  LIBSEMIGROUPS_TEST_CASE("SchreierSimsHelper",
                          "007",
                          "primitive on 8 points intersection (swap order)",
                          "[quick][schreier-sims]") {
    auto            rg = ReportGuard(REPORT);
    SchreierSims<8> S, T, U;
    using Perm = SchreierSims<8>::element_type;
    S.add_generator(Perm({0, 2, 3, 4, 1, 5, 6, 7}));
    S.add_generator(Perm({1, 2, 4, 0, 3, 5, 6, 7}));
    T.add_generator(Perm({1, 2, 3, 4, 5, 6, 0, 7}));
    T.add_generator(Perm({0, 1, 2, 3, 4, 6, 7, 5}));
    schreier_sims_helper::intersection(U, T, S);
    REQUIRE(U.size() == 10);
    REQUIRE(U.contains(Perm({0, 3, 4, 1, 2, 5, 6, 7})));
    REQUIRE(U.contains(Perm({1, 2, 4, 0, 3, 5, 6, 7})));
  }

  LIBSEMIGROUPS_TEST_CASE("SchreierSimsHelper",
                          "008",
                          "A13 and PGL(2, 11) intersection",
                          "[quick][schreier-sims]") {
    auto             rg = ReportGuard(REPORT);
    SchreierSims<13> S, T, U;
    using Perm = SchreierSims<13>::element_type;
    S.add_generator(Perm({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 0}));
    S.add_generator(Perm({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 11, 12, 10}));
    T.add_generator(Perm({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 0, 11, 12}));
    T.add_generator(Perm({0, 2, 4, 6, 8, 10, 1, 3, 5, 7, 9, 11, 12}));
    T.add_generator(Perm({11, 10, 5, 7, 8, 2, 9, 3, 4, 6, 1, 0, 12}));
    schreier_sims_helper::intersection(U, S, T);
    REQUIRE(U.size() == 660);
    REQUIRE(U.contains(Perm({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 0, 11, 12})));
    REQUIRE(U.contains(Perm({0, 4, 8, 1, 5, 9, 2, 6, 10, 3, 7, 11, 12})));
    REQUIRE(U.contains(Perm({11, 10, 5, 7, 8, 2, 9, 3, 4, 6, 1, 0, 12})));
  }

  LIBSEMIGROUPS_TEST_CASE("SchreierSimsHelper",
                          "009",
                          "A13 and PGL(2, 11) intersection (swap order)",
                          "[quick][schreier-sims]") {
    auto             rg = ReportGuard(REPORT);
    SchreierSims<13> S, T, U;
    using Perm = SchreierSims<13>::element_type;
    S.add_generator(Perm({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 0}));
    S.add_generator(Perm({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 11, 12, 10}));
    T.add_generator(Perm({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 0, 11, 12}));
    T.add_generator(Perm({0, 2, 4, 6, 8, 10, 1, 3, 5, 7, 9, 11, 12}));
    T.add_generator(Perm({11, 10, 5, 7, 8, 2, 9, 3, 4, 6, 1, 0, 12}));
    schreier_sims_helper::intersection(U, T, S);
    REQUIRE(U.size() == 660);
    REQUIRE(U.contains(Perm({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 0, 11, 12})));
    REQUIRE(U.contains(Perm({0, 4, 8, 1, 5, 9, 2, 6, 10, 3, 7, 11, 12})));
    REQUIRE(U.contains(Perm({11, 10, 5, 7, 8, 2, 9, 3, 4, 6, 1, 0, 12})));
  }

}  // namespace libsemigroups
