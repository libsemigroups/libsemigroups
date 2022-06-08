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

  LIBSEMIGROUPS_TEST_CASE("SchreierSimsHelper",
                          "010",
                          "S17 and A39 intersection",
                          "[quick][schreier-sims]") {
    auto             rg = ReportGuard(REPORT);
    SchreierSims<50> S, T, U;
    using Perm = SchreierSims<50>::element_type;
    S.add_generator(Perm({1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13,
                          14, 15, 16, 0,  17, 18, 19, 20, 21, 22, 23, 24, 25,
                          26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38,
                          39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49}));
    S.add_generator(Perm({1,  0,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12,
                          13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
                          26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38,
                          39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49}));
    T.add_generator(Perm({1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13,
                          14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26,
                          27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 0,
                          39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49}));
    T.add_generator(Perm({0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12,
                          13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
                          26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 37, 38, 36,
                          39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49}));
    schreier_sims_helper::intersection(U, S, T);
    REQUIRE(U.size() == 177843714048000);
    REQUIRE(U.contains(Perm({1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13,
                             14, 15, 16, 0,  17, 18, 19, 20, 21, 22, 23, 24, 25,
                             26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38,
                             39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49})));
    REQUIRE(U.contains(Perm({0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12,
                             13, 15, 16, 14, 17, 18, 19, 20, 21, 22, 23, 24, 25,
                             26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38,
                             39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49})));
  }

  LIBSEMIGROUPS_TEST_CASE("SchreierSimsHelper",
                          "011",
                          "A50 and PGL(2, 49) intersection",
                          "[quick][schreier-sims]") {
    auto             rg = ReportGuard(REPORT);
    SchreierSims<50> S, T, U;
    using Perm = SchreierSims<50>::element_type;
    S.add_generator(Perm({1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13,
                          14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26,
                          27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
                          40, 41, 42, 43, 44, 45, 46, 47, 48, 0,  49}));
    S.add_generator(Perm({0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12,
                          13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
                          26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38,
                          39, 40, 41, 42, 43, 44, 45, 46, 48, 49, 47}));
    T.add_generator(Perm({1,  2,  3,  4,  5,  6,  0,  8,  9,  10, 11, 12, 13,
                          7,  15, 16, 17, 18, 19, 20, 14, 22, 23, 24, 25, 26,
                          27, 21, 29, 30, 31, 32, 33, 34, 28, 36, 37, 38, 39,
                          40, 41, 35, 43, 44, 45, 46, 47, 48, 42, 49}));
    T.add_generator(Perm({0,  2,  4,  6,  1,  3,  5,  14, 16, 18, 20, 15, 17,
                          19, 28, 30, 32, 34, 29, 31, 33, 42, 44, 46, 48, 43,
                          45, 47, 7,  9,  11, 13, 8,  10, 12, 21, 23, 25, 27,
                          22, 24, 26, 35, 37, 39, 41, 36, 38, 40, 49}));
    T.add_generator(Perm({0,  17, 34, 44, 12, 22, 39, 26, 36, 4,  14, 31, 48,
                          9,  45, 13, 23, 40, 1,  18, 28, 15, 32, 42, 10, 27,
                          37, 5,  41, 2,  19, 29, 46, 7,  24, 11, 21, 38, 6,
                          16, 33, 43, 30, 47, 8,  25, 35, 3,  20, 49}));
    T.add_generator(Perm({49, 6,  3,  2,  5,  4,  1,  7,  31, 22, 41, 36, 27,
                          32, 28, 46, 19, 38, 39, 16, 45, 35, 9,  30, 43, 48,
                          33, 12, 14, 44, 23, 8,  13, 26, 47, 21, 11, 40, 17,
                          18, 37, 10, 42, 24, 29, 20, 15, 34, 25, 0}));
    schreier_sims_helper::intersection(U, S, T);
    REQUIRE(U.size() == 58800);
    REQUIRE(U.contains(Perm({1,  2,  3,  4,  5,  6,  0,  8,  9,  10, 11, 12, 13,
                             7,  15, 16, 17, 18, 19, 20, 14, 22, 23, 24, 25, 26,
                             27, 21, 29, 30, 31, 32, 33, 34, 28, 36, 37, 38, 39,
                             40, 41, 35, 43, 44, 45, 46, 47, 48, 42, 49})));
    REQUIRE(U.contains(Perm({0,  2,  4,  6,  1,  3,  5,  14, 16, 18, 20, 15, 17,
                             19, 28, 30, 32, 34, 29, 31, 33, 42, 44, 46, 48, 43,
                             45, 47, 7,  9,  11, 13, 8,  10, 12, 21, 23, 25, 27,
                             22, 24, 26, 35, 37, 39, 41, 36, 38, 40, 49})));
    REQUIRE(U.contains(Perm({0,  40, 24, 8,  48, 32, 16, 37, 21, 12, 45, 29, 20,
                             4,  25, 9,  42, 33, 17, 1,  41, 13, 46, 30, 14, 5,
                             38, 22, 43, 34, 18, 2,  35, 26, 10, 31, 15, 6,  39,
                             23, 7,  47, 19, 3,  36, 27, 11, 44, 28, 49})));
    REQUIRE(U.contains(Perm({49, 6,  3,  2,  5,  4,  1,  7,  31, 22, 41, 36, 27,
                             32, 28, 46, 19, 38, 39, 16, 45, 35, 9,  30, 43, 48,
                             33, 12, 14, 44, 23, 8,  13, 26, 47, 21, 11, 40, 17,
                             18, 37, 10, 42, 24, 29, 20, 15, 34, 25, 0})));
  }

  LIBSEMIGROUPS_TEST_CASE("SchreierSimsHelper",
                          "012",
                          "3^3:13 and 3^3.2.A(4) intersection",
                          "[quick][schreier-sims]") {
    auto             rg = ReportGuard(REPORT);
    SchreierSims<27> S, T, U;
    using Perm = SchreierSims<27>::element_type;
    S.add_generator(Perm({0,  17, 22, 1,  15, 23, 2,  16, 21, 3,  11, 25, 4, 9,
                          26, 5,  10, 24, 6,  14, 19, 7,  12, 20, 8,  13, 18}));
    S.add_generator(
        Perm({1,  2,  0,  4,  5,  3,  7,  8,  6,  10, 11, 9,  13, 14,
              12, 16, 17, 15, 19, 20, 18, 22, 23, 21, 25, 26, 24}));
    T.add_generator(Perm({0,  9, 18, 1,  10, 19, 2,  11, 20, 3,  12, 21, 4, 13,
                          22, 5, 14, 23, 6,  15, 24, 7,  16, 25, 8,  17, 26}));
    T.add_generator(
        Perm({0,  2,  1,  6,  8,  7,  3,  5,  4,  9,  11, 10, 15, 17,
              16, 12, 14, 13, 18, 20, 19, 24, 26, 25, 21, 23, 22}));
    T.add_generator(
        Perm({0,  1,  2,  6,  7,  8,  3,  4,  5,  9,  10, 11, 15, 16,
              17, 12, 13, 14, 18, 19, 20, 24, 25, 26, 21, 22, 23}));
    T.add_generator(
        Perm({1,  2,  0,  4,  5,  3,  7,  8,  6,  10, 11, 9,  13, 14,
              12, 16, 17, 15, 19, 20, 18, 22, 23, 21, 25, 26, 24}));
    schreier_sims_helper::intersection(U, S, T);
    REQUIRE(U.size() == 27);
    REQUIRE(
        U.contains(Perm({1,  2,  0,  4,  5,  3,  7,  8,  6,  10, 11, 9,  13, 14,
                         12, 16, 17, 15, 19, 20, 18, 22, 23, 21, 25, 26, 24})));
    REQUIRE(
        U.contains(Perm({3,  4, 5,  6,  7,  8,  0,  1,  2,  12, 13, 14, 15, 16,
                         17, 9, 10, 11, 21, 22, 23, 24, 25, 26, 18, 19, 20})));
    REQUIRE(
        U.contains(Perm({9,  10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22,
                         23, 24, 25, 26, 0,  1,  2,  3,  4,  5,  6,  7,  8})));
  }

  LIBSEMIGROUPS_TEST_CASE(
      "SchreierSimsHelper",
      "013",
      "PGamma(2, 9) wreath Sym(2) and Alt(6)^2.D_8 intersection",
      "[quick][schreier-sims]") {
    auto              rg = ReportGuard(REPORT);
    SchreierSims<100> S, T, U;
    using Perm = SchreierSims<100>::element_type;
    S.add_generator(Perm(
        {13, 17, 16, 19, 18, 12, 14, 10, 15, 11, 83, 87, 86, 89, 88, 82, 84,
         80, 85, 81, 63, 67, 66, 69, 68, 62, 64, 60, 65, 61, 53, 57, 56, 59,
         58, 52, 54, 50, 55, 51, 33, 37, 36, 39, 38, 32, 34, 30, 35, 31, 3,
         7,  6,  9,  8,  2,  4,  0,  5,  1,  43, 47, 46, 49, 48, 42, 44, 40,
         45, 41, 93, 97, 96, 99, 98, 92, 94, 90, 95, 91, 23, 27, 26, 29, 28,
         22, 24, 20, 25, 21, 73, 77, 76, 79, 78, 72, 74, 70, 75, 71}));
    S.add_generator(Perm(
        {65, 25, 5,  55, 35, 95, 15, 85, 75, 45, 63, 23, 3,  53, 33, 93, 13,
         83, 73, 43, 64, 24, 4,  54, 34, 94, 14, 84, 74, 44, 68, 28, 8,  58,
         38, 98, 18, 88, 78, 48, 61, 21, 1,  51, 31, 91, 11, 81, 71, 41, 60,
         20, 0,  50, 30, 90, 10, 80, 70, 40, 69, 29, 9,  59, 39, 99, 19, 89,
         79, 49, 62, 22, 2,  52, 32, 92, 12, 82, 72, 42, 66, 26, 6,  56, 36,
         96, 16, 86, 76, 46, 67, 27, 7,  57, 37, 97, 17, 87, 77, 47}));
    S.add_generator(Perm(
        {18, 68, 58, 98, 38, 28, 78, 88, 48, 8,  19, 69, 59, 99, 39, 29, 79,
         89, 49, 9,  16, 66, 56, 96, 36, 26, 76, 86, 46, 6,  17, 67, 57, 97,
         37, 27, 77, 87, 47, 7,  13, 63, 53, 93, 33, 23, 73, 83, 43, 3,  11,
         61, 51, 91, 31, 21, 71, 81, 41, 1,  15, 65, 55, 95, 35, 25, 75, 85,
         45, 5,  10, 60, 50, 90, 30, 20, 70, 80, 40, 0,  14, 64, 54, 94, 34,
         24, 74, 84, 44, 4,  12, 62, 52, 92, 32, 22, 72, 82, 42, 2}));
    T.add_generator(Perm(
        {78, 76, 77, 79, 75, 74, 71, 72, 70, 73, 38, 36, 37, 39, 35, 34, 31,
         32, 30, 33, 28, 26, 27, 29, 25, 24, 21, 22, 20, 23, 88, 86, 87, 89,
         85, 84, 81, 82, 80, 83, 8,  6,  7,  9,  5,  4,  1,  2,  0,  3,  48,
         46, 47, 49, 45, 44, 41, 42, 40, 43, 68, 66, 67, 69, 65, 64, 61, 62,
         60, 63, 58, 56, 57, 59, 55, 54, 51, 52, 50, 53, 98, 96, 97, 99, 95,
         94, 91, 92, 90, 93, 18, 16, 17, 19, 15, 14, 11, 12, 10, 13}));
    T.add_generator(Perm(
        {24, 74, 44, 64, 94, 4,  84, 34, 14, 54, 23, 73, 43, 63, 93, 3,  83,
         33, 13, 53, 28, 78, 48, 68, 98, 8,  88, 38, 18, 58, 29, 79, 49, 69,
         99, 9,  89, 39, 19, 59, 25, 75, 45, 65, 95, 5,  85, 35, 15, 55, 22,
         72, 42, 62, 92, 2,  82, 32, 12, 52, 20, 70, 40, 60, 90, 0,  80, 30,
         10, 50, 27, 77, 47, 67, 97, 7,  87, 37, 17, 57, 26, 76, 46, 66, 96,
         6,  86, 36, 16, 56, 21, 71, 41, 61, 91, 1,  81, 31, 11, 51}));
    schreier_sims_helper::intersection(U, S, T);
    REQUIRE(U.size() == 2);
    REQUIRE(U.contains(Perm(
        {11, 1,  91, 81, 71, 61, 51, 41, 31, 21, 10, 0,  90, 80, 70, 60, 50,
         40, 30, 20, 19, 9,  99, 89, 79, 69, 59, 49, 39, 29, 18, 8,  98, 88,
         78, 68, 58, 48, 38, 28, 17, 7,  97, 87, 77, 67, 57, 47, 37, 27, 16,
         6,  96, 86, 76, 66, 56, 46, 36, 26, 15, 5,  95, 85, 75, 65, 55, 45,
         35, 25, 14, 4,  94, 84, 74, 64, 54, 44, 34, 24, 13, 3,  93, 83, 73,
         63, 53, 43, 33, 23, 12, 2,  92, 82, 72, 62, 52, 42, 32, 22})));
  }

  LIBSEMIGROUPS_TEST_CASE("SchreierSimsHelper",
                          "014",
                          "Alt(6)^2.2^2:4 and Alt(6)^2.4 intersection",
                          "[quick][schreier-sims]") {
    auto              rg = ReportGuard(REPORT);
    SchreierSims<100> S, T, U;
    using Perm = SchreierSims<100>::element_type;
    S.add_generator(Perm(
        {68, 62, 69, 67, 66, 64, 61, 65, 60, 63, 98, 92, 99, 97, 96, 94, 91,
         95, 90, 93, 38, 32, 39, 37, 36, 34, 31, 35, 30, 33, 78, 72, 79, 77,
         76, 74, 71, 75, 70, 73, 8,  2,  9,  7,  6,  4,  1,  5,  0,  3,  28,
         22, 29, 27, 26, 24, 21, 25, 20, 23, 18, 12, 19, 17, 16, 14, 11, 15,
         10, 13, 88, 82, 89, 87, 86, 84, 81, 85, 80, 83, 58, 52, 59, 57, 56,
         54, 51, 55, 50, 53, 48, 42, 49, 47, 46, 44, 41, 45, 40, 43}));
    S.add_generator(Perm(
        {72, 92, 12, 82, 32, 62, 42, 22, 52, 2,  70, 90, 10, 80, 30, 60, 40,
         20, 50, 0,  75, 95, 15, 85, 35, 65, 45, 25, 55, 5,  76, 96, 16, 86,
         36, 66, 46, 26, 56, 6,  71, 91, 11, 81, 31, 61, 41, 21, 51, 1,  79,
         99, 19, 89, 39, 69, 49, 29, 59, 9,  78, 98, 18, 88, 38, 68, 48, 28,
         58, 8,  74, 94, 14, 84, 34, 64, 44, 24, 54, 4,  77, 97, 17, 87, 37,
         67, 47, 27, 57, 7,  73, 93, 13, 83, 33, 63, 43, 23, 53, 3}));
    T.add_generator(Perm(
        {20, 26, 23, 28, 24, 29, 25, 22, 27, 21, 40, 46, 43, 48, 44, 49, 45,
         42, 47, 41, 60, 66, 63, 68, 64, 69, 65, 62, 67, 61, 70, 76, 73, 78,
         74, 79, 75, 72, 77, 71, 50, 56, 53, 58, 54, 59, 55, 52, 57, 51, 30,
         36, 33, 38, 34, 39, 35, 32, 37, 31, 90, 96, 93, 98, 94, 99, 95, 92,
         97, 91, 10, 16, 13, 18, 14, 19, 15, 12, 17, 11, 0,  6,  3,  8,  4,
         9,  5,  2,  7,  1,  80, 86, 83, 88, 84, 89, 85, 82, 87, 81}));
    T.add_generator(Perm(
        {21, 11, 71, 51, 41, 91, 1,  31, 61, 81, 24, 14, 74, 54, 44, 94, 4,
         34, 64, 84, 27, 17, 77, 57, 47, 97, 7,  37, 67, 87, 28, 18, 78, 58,
         48, 98, 8,  38, 68, 88, 25, 15, 75, 55, 45, 95, 5,  35, 65, 85, 20,
         10, 70, 50, 40, 90, 0,  30, 60, 80, 26, 16, 76, 56, 46, 96, 6,  36,
         66, 86, 23, 13, 73, 53, 43, 93, 3,  33, 63, 83, 22, 12, 72, 52, 42,
         92, 2,  32, 62, 82, 29, 19, 79, 59, 49, 99, 9,  39, 69, 89}));
    schreier_sims_helper::intersection(U, S, T);
    REQUIRE(U.size() == 4);
    REQUIRE(U.contains(Perm(
        {1,  11, 21, 31, 41, 51, 61, 71, 81, 91, 0,  10, 20, 30, 40, 50, 60,
         70, 80, 90, 9,  19, 29, 39, 49, 59, 69, 79, 89, 99, 8,  18, 28, 38,
         48, 58, 68, 78, 88, 98, 7,  17, 27, 37, 47, 57, 67, 77, 87, 97, 6,
         16, 26, 36, 46, 56, 66, 76, 86, 96, 5,  15, 25, 35, 45, 55, 65, 75,
         85, 95, 4,  14, 24, 34, 44, 54, 64, 74, 84, 94, 3,  13, 23, 33, 43,
         53, 63, 73, 83, 93, 2,  12, 22, 32, 42, 52, 62, 72, 82, 92})));
  }

  LIBSEMIGROUPS_TEST_CASE("SchreierSimsHelper",
                          "015",
                          "3^3(S(4) x 2) and ASL(3, 3) intersection",
                          "[quick][schreier-sims]") {
    auto             rg = ReportGuard(REPORT);
    SchreierSims<27> S, T, U;
    using Perm = SchreierSims<27>::element_type;
    S.add_generator(Perm({0, 6,  3,  25, 22, 19, 14, 11, 17, 9,  15, 12, 7, 4,
                          1, 23, 20, 26, 18, 24, 21, 16, 13, 10, 5,  2,  8}));
    S.add_generator(Perm({0, 18, 9, 25, 16, 7,  14, 5,  23, 3, 21, 12, 19, 10,
                          1, 17, 8, 26, 6,  24, 15, 22, 13, 4, 11, 2,  20}));
    S.add_generator(Perm({0, 9,  18, 14, 23, 5,  25, 7,  16, 1, 10, 19, 12, 21,
                          3, 26, 8,  17, 2,  11, 20, 13, 22, 4, 24, 6,  15}));
    S.add_generator(
        Perm({1,  2,  0,  4,  5,  3,  7,  8,  6,  10, 11, 9,  13, 14,
              12, 16, 17, 15, 19, 20, 18, 22, 23, 21, 25, 26, 24}));
    T.add_generator(Perm({0,  26, 13, 1,  24, 14, 2, 25, 12, 9, 8,  22, 10, 6,
                          23, 11, 7,  21, 18, 17, 4, 19, 15, 5, 20, 16, 3}));
    T.add_generator(Perm({0,  17, 22, 1,  15, 23, 2,  16, 21, 3,  11, 25, 4, 9,
                          26, 5,  10, 24, 6,  14, 19, 7,  12, 20, 8,  13, 18}));
    T.add_generator(
        Perm({1,  2,  0,  4,  5,  3,  7,  8,  6,  10, 11, 9,  13, 14,
              12, 16, 17, 15, 19, 20, 18, 22, 23, 21, 25, 26, 24}));
    schreier_sims_helper::intersection(U, S, T);
    REQUIRE(U.size() == 648);
    REQUIRE(
        U.contains(Perm({0,  9, 18, 6,  15, 24, 3,  12, 21, 1,  10, 19, 7, 16,
                         25, 4, 13, 22, 2,  11, 20, 8,  17, 26, 5,  14, 23})));
    REQUIRE(
        U.contains(Perm({0,  1,  2,  25, 26, 24, 14, 12, 13, 3,  4,  5, 19, 20,
                         18, 17, 15, 16, 6,  7,  8,  22, 23, 21, 11, 9, 10})));
    REQUIRE(
        U.contains(Perm({1,  2,  0,  26, 24, 25, 12, 13, 14, 4,  5, 3,  20, 18,
                         19, 15, 16, 17, 7,  8,  6,  23, 21, 22, 9, 10, 11})));
  }

  LIBSEMIGROUPS_TEST_CASE("SchreierSimsHelper",
                          "016",
                          "7^2:3 x Q(8) and 7^2:D(2*6) intersection",
                          "[quick][schreier-sims]") {
    auto             rg = ReportGuard(REPORT);
    SchreierSims<49> S, T, U;
    using Perm = SchreierSims<49>::element_type;
    S.add_generator(Perm({0,  28, 35, 42, 7,  14, 21, 1,  29, 36, 43, 8,  15,
                          22, 2,  30, 37, 44, 9,  16, 23, 3,  31, 38, 45, 10,
                          17, 24, 4,  32, 39, 46, 11, 18, 25, 5,  33, 40, 47,
                          12, 19, 26, 6,  34, 41, 48, 13, 20, 27}));
    S.add_generator(
        Perm({0, 44, 10, 18, 26, 34, 36, 41, 22, 42, 5,  31, 16, 11, 43, 19, 30,
              7, 6,  39, 24, 9,  32, 27, 38, 14, 1,  47, 17, 13, 40, 29, 46, 21,
              2, 25, 3,  15, 48, 37, 12, 28, 33, 35, 4,  23, 8,  45, 20}));
    S.add_generator(Perm({0,  3,  4,  5,  6,  1,  2,  21, 24, 25, 26, 27, 22,
                          23, 28, 31, 32, 33, 34, 29, 30, 35, 38, 39, 40, 41,
                          36, 37, 42, 45, 46, 47, 48, 43, 44, 7,  10, 11, 12,
                          13, 8,  9,  14, 17, 18, 19, 20, 15, 16}));
    S.add_generator(Perm({1,  3,  5,  2,  0,  6,  4,  8,  10, 12, 9,  7,  13,
                          11, 15, 17, 19, 16, 14, 20, 18, 22, 24, 26, 23, 21,
                          27, 25, 29, 31, 33, 30, 28, 34, 32, 36, 38, 40, 37,
                          35, 41, 39, 43, 45, 47, 44, 42, 48, 46}));
    T.add_generator(Perm({0,  32, 40, 48, 8,  16, 24, 7,  4,  47, 34, 22, 37,
                          17, 14, 25, 5,  13, 36, 30, 45, 21, 11, 33, 6,  15,
                          44, 38, 28, 46, 19, 41, 1,  23, 10, 35, 18, 12, 27,
                          43, 2,  31, 42, 39, 26, 20, 29, 9,  3}));
    T.add_generator(Perm({0,  32, 40, 48, 8,  16, 24, 1,  28, 41, 46, 10, 19,
                          23, 2,  31, 35, 43, 12, 18, 27, 3,  29, 39, 42, 9,
                          20, 26, 4,  34, 37, 47, 7,  17, 22, 5,  30, 36, 45,
                          13, 14, 25, 6,  33, 38, 44, 11, 15, 21}));
    T.add_generator(Perm({0,  4,  5,  6,  1,  2,  3,  28, 32, 33, 34, 29, 30,
                          31, 35, 39, 40, 41, 36, 37, 38, 42, 46, 47, 48, 43,
                          44, 45, 7,  11, 12, 13, 8,  9,  10, 14, 18, 19, 20,
                          15, 16, 17, 21, 25, 26, 27, 22, 23, 24}));
    T.add_generator(Perm({1,  3,  5,  2,  0,  6,  4,  8,  10, 12, 9,  7,  13,
                          11, 15, 17, 19, 16, 14, 20, 18, 22, 24, 26, 23, 21,
                          27, 25, 29, 31, 33, 30, 28, 34, 32, 36, 38, 40, 37,
                          35, 41, 39, 43, 45, 47, 44, 42, 48, 46}));
    schreier_sims_helper::intersection(U, S, T);
    REQUIRE(U.size() == 98);
    REQUIRE(U.contains(Perm({0,  4,  5,  6,  1,  2,  3,  28, 32, 33, 34, 29, 30,
                             31, 35, 39, 40, 41, 36, 37, 38, 42, 46, 47, 48, 43,
                             44, 45, 7,  11, 12, 13, 8,  9,  10, 14, 18, 19, 20,
                             15, 16, 17, 21, 25, 26, 27, 22, 23, 24})));
    REQUIRE(U.contains(Perm({1,  0,  6,  4,  3,  5,  2,  29, 28, 34, 32, 31, 33,
                             30, 36, 35, 41, 39, 38, 40, 37, 43, 42, 48, 46, 45,
                             47, 44, 8,  7,  13, 11, 10, 12, 9,  15, 14, 20, 18,
                             17, 19, 16, 22, 21, 27, 25, 24, 26, 23})));
    REQUIRE(U.contains(Perm({7,  11, 12, 13, 8,  9,  10, 0,  4,  5,  6,  1,  2,
                             3,  42, 46, 47, 48, 43, 44, 45, 28, 32, 33, 34, 29,
                             30, 31, 21, 25, 26, 27, 22, 23, 24, 35, 39, 40, 41,
                             36, 37, 38, 14, 18, 19, 20, 15, 16, 17})));
  }

  LIBSEMIGROUPS_TEST_CASE("SchreierSimsHelper",
                          "018",
                          "D(2*53) and 53:13 intersection",
                          "[quick][schreier-sims]") {
    auto             rg = ReportGuard(REPORT);
    SchreierSims<53> S, T, U;
    using Perm = SchreierSims<53>::element_type;
    S.add_generator(Perm(
        {0,  27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43,
         44, 45, 46, 47, 48, 49, 50, 51, 52, 1,  2,  3,  4,  5,  6,  7,  8,  9,
         10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26}));
    S.add_generator(Perm(
        {1,  2,  18, 48, 35, 11, 24, 20, 40, 30, 37, 36, 10, 5,  34, 4,  13, 47,
         3,  15, 25, 43, 29, 9,  12, 16, 52, 0,  27, 44, 41, 39, 8,  23, 6,  49,
         38, 31, 50, 42, 21, 45, 51, 26, 28, 22, 33, 14, 19, 7,  32, 46, 17}));
    T.add_generator(Perm(
        {0,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21,
         22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
         40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 1,  2,  3,  4}));
    T.add_generator(Perm(
        {1,  2,  18, 48, 35, 11, 24, 20, 40, 30, 37, 36, 10, 5,  34, 4,  13, 47,
         3,  15, 25, 43, 29, 9,  12, 16, 52, 0,  27, 44, 41, 39, 8,  23, 6,  49,
         38, 31, 50, 42, 21, 45, 51, 26, 28, 22, 33, 14, 19, 7,  32, 46, 17}));
    schreier_sims_helper::intersection(U, S, T);
    REQUIRE(U.size() == 53);
    REQUIRE(U.contains(Perm(
        {1,  2,  18, 48, 35, 11, 24, 20, 40, 30, 37, 36, 10, 5,  34, 4,  13, 47,
         3,  15, 25, 43, 29, 9,  12, 16, 52, 0,  27, 44, 41, 39, 8,  23, 6,  49,
         38, 31, 50, 42, 21, 45, 51, 26, 28, 22, 33, 14, 19, 7,  32, 46, 17})));
  }

  LIBSEMIGROUPS_TEST_CASE("SchreierSimsHelper",
                          "019",
                          "2^6:(7 x D_14) and PSL(2, 6)^2.4 intersection",
                          "[quick][schreier-sims]") {
    auto             rg = ReportGuard(REPORT);
    SchreierSims<64> S, T, U;
    using Perm = SchreierSims<64>::element_type;
    S.add_generator(
        Perm({0,  32, 5,  37, 2,  34, 7,  39, 20, 52, 17, 49, 22, 54, 19, 51,
              8,  40, 13, 45, 10, 42, 15, 47, 28, 60, 25, 57, 30, 62, 27, 59,
              16, 48, 21, 53, 18, 50, 23, 55, 4,  36, 1,  33, 6,  38, 3,  35,
              24, 56, 29, 61, 26, 58, 31, 63, 12, 44, 9,  41, 14, 46, 11, 43}));
    S.add_generator(
        Perm({0,  17, 2,  19, 1,  16, 3,  18, 8,  25, 10, 27, 9,  24, 11, 26,
              4,  21, 6,  23, 5,  20, 7,  22, 12, 29, 14, 31, 13, 28, 15, 30,
              32, 49, 34, 51, 33, 48, 35, 50, 40, 57, 42, 59, 41, 56, 43, 58,
              36, 53, 38, 55, 37, 52, 39, 54, 44, 61, 46, 63, 45, 60, 47, 62}));
    S.add_generator(
        Perm({1,  0,  3,  2,  5,  4,  7,  6,  9,  8,  11, 10, 13, 12, 15, 14,
              17, 16, 19, 18, 21, 20, 23, 22, 25, 24, 27, 26, 29, 28, 31, 30,
              33, 32, 35, 34, 37, 36, 39, 38, 41, 40, 43, 42, 45, 44, 47, 46,
              49, 48, 51, 50, 53, 52, 55, 54, 57, 56, 59, 58, 61, 60, 63, 62}));
    T.add_generator(
        Perm({10, 34, 58, 42, 50, 2, 18, 26, 9,  33, 57, 41, 49, 1, 17, 25,
              15, 39, 63, 47, 55, 7, 23, 31, 13, 37, 61, 45, 53, 5, 21, 29,
              12, 36, 60, 44, 52, 4, 20, 28, 14, 38, 62, 46, 54, 6, 22, 30,
              11, 35, 59, 43, 51, 3, 19, 27, 8,  32, 56, 40, 48, 0, 16, 24}));
    T.add_generator(
        Perm({39, 23, 63, 55, 31, 15, 47, 7, 37, 21, 61, 53, 29, 13, 45, 5,
              32, 16, 56, 48, 24, 8,  40, 0, 35, 19, 59, 51, 27, 11, 43, 3,
              33, 17, 57, 49, 25, 9,  41, 1, 34, 18, 58, 50, 26, 10, 42, 2,
              36, 20, 60, 52, 28, 12, 44, 4, 38, 22, 62, 54, 30, 14, 46, 6}));
    schreier_sims_helper::intersection(U, S, T);
    REQUIRE(U.contains(Perm({3,  2,  1,  0,  7,  6,  5,  4,  11, 10, 9,  8,  15,
                             14, 13, 12, 19, 18, 17, 16, 23, 22, 21, 20, 27, 26,
                             25, 24, 31, 30, 29, 28, 35, 34, 33, 32, 39, 38, 37,
                             36, 43, 42, 41, 40, 47, 46, 45, 44, 51, 50, 49, 48,
                             55, 54, 53, 52, 59, 58, 57, 56, 63, 62, 61, 60})));
    REQUIRE(U.contains(Perm({4,  5,  6,  7,  0,  1,  2,  3,  12, 13, 14, 15, 8,
                             9,  10, 11, 20, 21, 22, 23, 16, 17, 18, 19, 28, 29,
                             30, 31, 24, 25, 26, 27, 36, 37, 38, 39, 32, 33, 34,
                             35, 44, 45, 46, 47, 40, 41, 42, 43, 52, 53, 54, 55,
                             48, 49, 50, 51, 60, 61, 62, 63, 56, 57, 58, 59})));
    REQUIRE(U.contains(Perm({24, 25, 26, 27, 28, 29, 30, 31, 16, 17, 18, 19, 20,
                             21, 22, 23, 8,  9,  10, 11, 12, 13, 14, 15, 0,  1,
                             2,  3,  4,  5,  6,  7,  56, 57, 58, 59, 60, 61, 62,
                             63, 48, 49, 50, 51, 52, 53, 54, 55, 40, 41, 42, 43,
                             44, 45, 46, 47, 32, 33, 34, 35, 36, 37, 38, 39})));
    REQUIRE(U.contains(Perm({32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44,
                             45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57,
                             58, 59, 60, 61, 62, 63, 0,  1,  2,  3,  4,  5,  6,
                             7,  8,  9,  10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
                             20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31})));
    REQUIRE(U.size() == 16);
  }

}  // namespace libsemigroups
