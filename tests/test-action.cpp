// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019 Finn Smith
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

// TODO(now):
// 1. iwyu
// 2. add examples from Action

#include "action.hpp"
#include "bmat8.hpp"
#include "element-helper.hpp"
#include "element.hpp"
#include "hpcombi.hpp"
#include "report.hpp"
#include "timer.hpp"

#include "catch.hpp"  // for REQUIRE, REQUIRE_THROWS_AS, REQUI...
#include "test-main.hpp"

namespace libsemigroups {

  constexpr bool REPORT = false;

  using row_action_type = ImageRightAction<BMat8, BMat8>;
  using col_action_type = ImageLeftAction<BMat8, BMat8>;
  using row_orb_type    = RightAction<BMat8, BMat8, row_action_type>;
  using col_orb_type    = LeftAction<BMat8, BMat8, col_action_type>;

  LIBSEMIGROUPS_TEST_CASE("Action",
                          "001",
                          "row and column basis orbits for BMat8",
                          "[quick]") {
    auto         rg = ReportGuard(REPORT);
    row_orb_type row_orb;
    row_orb.add_seed(BMat8({{1, 0, 0}, {0, 1, 0}, {0, 0, 0}}));
    row_orb.add_generator(BMat8({{0, 1, 0}, {1, 0, 0}, {0, 0, 1}}));

    REQUIRE(row_orb.size() == 1);

    col_orb_type col_orb;
    col_orb.add_seed(BMat8({{1, 0, 0}, {0, 1, 0}, {0, 0, 0}}));
    col_orb.add_generator(BMat8({{0, 1, 0}, {1, 0, 0}, {0, 0, 1}}));

    REQUIRE(col_orb.size() == 1);
  }

  LIBSEMIGROUPS_TEST_CASE("Action",
                          "002",
                          "row and column basis orbits for BMat8",
                          "[quick]") {
    auto         rg = ReportGuard(REPORT);
    row_orb_type row_orb;
    row_orb.add_seed(
        BMat8({{1, 1, 1, 0}, {1, 1, 0, 0}, {0, 1, 0, 1}, {0, 1, 0, 0}})
            .row_space_basis());

    row_orb.add_generator(
        BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}));
    row_orb.add_generator(
        BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}));
    row_orb.add_generator(
        BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}));
    row_orb.add_generator(
        BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {1, 0, 0, 1}}));
    row_orb.add_generator(
        BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}}));

    REQUIRE(row_orb.size() == 553);

    col_orb_type col_orb;
    col_orb.add_seed(
        BMat8({{1, 1, 1, 0}, {1, 1, 0, 0}, {0, 1, 0, 1}, {0, 1, 0, 0}})
            .col_space_basis());

    col_orb.add_generator(
        BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}));
    col_orb.add_generator(
        BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}));
    col_orb.add_generator(
        BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}));
    col_orb.add_generator(
        BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {1, 0, 0, 1}}));
    col_orb.add_generator(
        BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}}));

    REQUIRE(col_orb.size() == 553);
  }

  LIBSEMIGROUPS_TEST_CASE("Action",
                          "003",
                          "add generators after enumeration",
                          "[quick]") {
    auto         rg = ReportGuard(REPORT);
    row_orb_type row_orb;
    row_orb.add_seed(
        BMat8({{1, 1, 1, 0}, {1, 1, 0, 0}, {0, 1, 0, 1}, {0, 1, 0, 0}})
            .row_space_basis());

    row_orb.add_generator(
        BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}));
    row_orb.add_generator(
        BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}));
    row_orb.add_generator(
        BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}));
    row_orb.add_generator(
        BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {1, 0, 0, 1}}));

    REQUIRE(row_orb.size() == 177);

    row_orb.add_generator(
        BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}}));

    REQUIRE(row_orb.size() == 553);

    col_orb_type col_orb;
    col_orb.add_seed(
        BMat8({{1, 1, 1, 0}, {1, 1, 0, 0}, {0, 1, 0, 1}, {0, 1, 0, 0}})
            .col_space_basis());

    col_orb.add_generator(
        BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}));
    col_orb.add_generator(
        BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}));
    col_orb.add_generator(
        BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}));
    col_orb.add_generator(
        BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {1, 0, 0, 1}}));

    REQUIRE(col_orb.size() == 376);

    col_orb.add_generator(
        BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}}));

    REQUIRE(col_orb.size() == 553);
  }

  LIBSEMIGROUPS_TEST_CASE("Action",
                          "004",
                          "multipliers for BMat8 row and column orbits",
                          "[quick][no-valgrind]") {
    auto         rg = ReportGuard(REPORT);
    row_orb_type row_orb;
    row_orb.add_seed(
        BMat8({{1, 1, 1, 0}, {1, 1, 0, 0}, {0, 1, 0, 1}, {0, 1, 0, 0}})
            .row_space_basis());

    row_orb.add_generator(
        BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}));
    row_orb.add_generator(
        BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}));
    row_orb.add_generator(
        BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}));
    row_orb.add_generator(
        BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {1, 0, 0, 1}}));
    row_orb.add_generator(
        BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}}));

    row_orb.reserve(1000);

    REQUIRE(row_orb.size() == 553);
    REQUIRE(row_orb.digraph().nr_scc() == 14);
    REQUIRE(std::vector<size_t>(row_orb.digraph().cbegin_scc_roots(),
                                row_orb.digraph().cend_scc_roots())
            == std::vector<size_t>({277,
                                    317,
                                    160,
                                    119,
                                    267,
                                    116,
                                    411,
                                    497,
                                    183,
                                    272,
                                    154,
                                    443,
                                    65,
                                    101}));

    for (size_t i = 0; i < row_orb.size(); ++i) {
      REQUIRE(
          row_orb.position((row_orb.at(i) * row_orb.multiplier_to_scc_root(i))
                               .row_space_basis())
          == row_orb.position(row_orb.root_of_scc(i)));
      REQUIRE((row_orb.at(i) * row_orb.multiplier_to_scc_root(i)
               * row_orb.multiplier_from_scc_root(i))
                  .row_space_basis()
              == row_orb.at(i));
    }

    col_orb_type col_orb;
    col_orb.add_seed(
        BMat8({{1, 1, 1, 0}, {1, 1, 0, 0}, {0, 1, 0, 1}, {0, 1, 0, 0}})
            .col_space_basis());

    col_orb.add_generator(
        BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}));
    col_orb.add_generator(
        BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}));
    col_orb.add_generator(
        BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}));
    col_orb.add_generator(
        BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {1, 0, 0, 1}}));
    col_orb.add_generator(
        BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}}));

    REQUIRE(col_orb.size() == 553);

    for (size_t i = 0; i < col_orb.size(); ++i) {
      REQUIRE((col_orb.multiplier_from_scc_root(i)
               * col_orb.multiplier_to_scc_root(i) * col_orb.at(i))
                  .col_space_basis()
              == col_orb.at(i));
    }
  }

  LIBSEMIGROUPS_TEST_CASE("Action",
                          "005",
                          "orbits for regular boolean mat monoid 5",
                          "[quick][no-valgrind]") {
    auto                     rg             = ReportGuard(REPORT);
    const std::vector<BMat8> reg_bmat5_gens = {BMat8({{0, 1, 0, 0, 0},
                                                      {1, 0, 0, 0, 0},
                                                      {0, 0, 1, 0, 0},
                                                      {0, 0, 0, 1, 0},
                                                      {0, 0, 0, 0, 1}}),
                                               BMat8({{0, 1, 0, 0, 0},
                                                      {0, 0, 1, 0, 0},
                                                      {0, 0, 0, 1, 0},
                                                      {0, 0, 0, 0, 1},
                                                      {1, 0, 0, 0, 0}}),
                                               BMat8({{1, 0, 0, 0, 0},
                                                      {1, 1, 0, 0, 0},
                                                      {0, 0, 1, 0, 0},
                                                      {0, 0, 0, 1, 0},
                                                      {0, 0, 0, 0, 1}}),
                                               BMat8({{0, 0, 0, 0, 0},
                                                      {0, 1, 0, 0, 0},
                                                      {0, 0, 1, 0, 0},
                                                      {0, 0, 0, 1, 0},
                                                      {0, 0, 0, 0, 1}})};
    row_orb_type             row_orb;
    col_orb_type             col_orb;

    row_orb.add_seed(BMat8::one());
    col_orb.add_seed(BMat8::one());
    for (BMat8 g : reg_bmat5_gens) {
      row_orb.add_generator(g);
      col_orb.add_generator(g);
    }
    row_orb.run();
    col_orb.run();

    REQUIRE(row_orb.size() == 110519);
    REQUIRE(col_orb.size() == 110519);
  }

  LIBSEMIGROUPS_TEST_CASE("Action",
                          "006",
                          "orbits for regular boolean mat monoid 6",
                          "[standard]") {
    auto                     rg             = ReportGuard(REPORT);
    const std::vector<BMat8> reg_bmat6_gens = {BMat8({{0, 1, 0, 0, 0, 0},
                                                      {1, 0, 0, 0, 0, 0},
                                                      {0, 0, 1, 0, 0, 0},
                                                      {0, 0, 0, 1, 0, 0},
                                                      {0, 0, 0, 0, 1, 0},
                                                      {0, 0, 0, 0, 0, 1}}),
                                               BMat8({{0, 1, 0, 0, 0, 0},
                                                      {0, 0, 1, 0, 0, 0},
                                                      {0, 0, 0, 1, 0, 0},
                                                      {0, 0, 0, 0, 1, 0},
                                                      {0, 0, 0, 0, 0, 1},
                                                      {1, 0, 0, 0, 0, 0}}),
                                               BMat8({{1, 0, 0, 0, 0, 0},
                                                      {0, 1, 0, 0, 0, 0},
                                                      {0, 0, 1, 0, 0, 0},
                                                      {0, 0, 0, 1, 0, 0},
                                                      {0, 0, 0, 0, 1, 0},
                                                      {1, 0, 0, 0, 0, 1}}),
                                               BMat8({{1, 0, 0, 0, 0, 0},
                                                      {0, 1, 0, 0, 0, 0},
                                                      {0, 0, 1, 0, 0, 0},
                                                      {0, 0, 0, 1, 0, 0},
                                                      {0, 0, 0, 0, 1, 0},
                                                      {0, 0, 0, 0, 0, 0}})};
    row_orb_type             row_orb;

    row_orb.add_seed(BMat8::one());
    for (BMat8 g : reg_bmat6_gens) {
      row_orb.add_generator(g);
    }
    row_orb.run_for(std::chrono::milliseconds(500));

    // REQUIRE(row_orb.size() == 37977468);
  }

  LIBSEMIGROUPS_TEST_CASE("Action",
                          "007",
                          "partial perm image orbit",
                          "[quick]") {
    auto rg     = ReportGuard(REPORT);
    using PPerm = PartialPerm<uint_fast8_t>;
    RightAction<PPerm, PPerm, ImageRightAction<PPerm, PPerm>> o;
    o.add_seed(PPerm::identity(8));
    o.add_generator(
        PPerm({0, 1, 2, 3, 4, 5, 6, 7}, {1, 2, 3, 4, 5, 6, 7, 0}, 8));
    o.add_generator(
        PPerm({0, 1, 2, 3, 4, 5, 6, 7}, {1, 0, 2, 3, 4, 5, 6, 7}, 8));
    o.add_generator(PPerm({1, 2, 3, 4, 5, 6, 7}, {0, 1, 2, 3, 4, 5, 6}, 8));
    o.add_generator(PPerm({0, 1, 2, 3, 4, 5, 6}, {1, 2, 3, 4, 5, 6, 7}, 8));
    REQUIRE(o.size() == 256);
  }

  LIBSEMIGROUPS_TEST_CASE("Action",
                          "008",
                          "partial perm image orbit",
                          "[quick][no-valgrind]") {
    auto rg     = ReportGuard(REPORT);
    using PPerm = PartialPerm<uint_fast8_t>;
    RightAction<PPerm, PPerm, ImageRightAction<PPerm, PPerm>> o;
    o.add_seed(PPerm::identity(16));
    o.add_generator(
        PPerm({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
              {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0},
              16));
    o.add_generator(
        PPerm({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
              {1, 0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
              16));
    o.add_generator(PPerm({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
                          {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14},
                          16));
    o.add_generator(PPerm({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14},
                          {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
                          16));
    o.reserve(70000);
    REQUIRE(o.size() == 65536);
  }

  LIBSEMIGROUPS_TEST_CASE("Action",
                          "009",
                          "partial perm image orbit",
                          "[quick][no-valgrind]") {
    auto rg     = ReportGuard(REPORT);
    using PPerm = PPermHelper<16>::type;
    RightAction<PPerm, PPerm, ImageRightAction<PPerm, PPerm>> o;
    o.add_seed(One<PPerm>()(16));
    o.add_generator(
        PPerm({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
              {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0},
              16));
    o.add_generator(
        PPerm({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
              {1, 0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
              16));
    o.add_generator(PPerm({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
                          {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14},
                          16));
    o.add_generator(PPerm({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14},
                          {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
                          16));
    o.reserve(70000);
    REQUIRE(o.size() == 65536);
    REQUIRE(o.digraph().nr_scc() == 17);
  }

  LIBSEMIGROUPS_TEST_CASE("Action",
                          "010",
                          "partial perm image orbit",
                          "[quick][no-valgrind]") {
    auto rg     = ReportGuard(REPORT);
    using PPerm = PPermHelper<16>::type;
    LeftAction<PPerm, PPerm, ImageLeftAction<PPerm, PPerm>> o;
    o.add_seed(One<PPerm>()(16));
    o.add_generator(
        PPerm({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
              {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0},
              16));
    o.add_generator(
        PPerm({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
              {1, 0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
              16));
    o.add_generator(PPerm({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
                          {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14},
                          16));
    o.add_generator(PPerm({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14},
                          {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
                          16));
    REQUIRE(o.size() == 65536);
    REQUIRE(o.digraph().nr_scc() == 17);
  }

  LIBSEMIGROUPS_TEST_CASE("Action",
                          "011",
                          "permutation on integers",
                          "[quick]") {
    auto rg    = ReportGuard(REPORT);
    using Perm = PermHelper<8>::type;
    RightAction<Perm, u_int8_t, ImageRightAction<Perm, u_int8_t>> o;
    o.add_seed(0);
    o.add_generator(Perm({1, 0, 2, 3, 4, 5, 6, 7}));
    o.add_generator(Perm({1, 2, 3, 4, 5, 6, 7, 0}));

    REQUIRE(o.size() == 8);
    REQUIRE(o.digraph().nr_scc() == 1);
  }

  LIBSEMIGROUPS_TEST_CASE("Action",
                          "012",
                          "permutation on sets, arrays",
                          "[quick]") {
    auto rg    = ReportGuard(REPORT);
    using Perm = PermHelper<10>::type;

    RightAction<Perm,
                std::array<u_int8_t, 5>,
                OnSets<Perm, u_int8_t, std::array<u_int8_t, 5>>>
        o;
    o.add_seed({0, 1, 2, 3, 4});
    o.add_generator(Perm({1, 0, 2, 3, 4, 5, 6, 7, 8, 9}));
    o.add_generator(Perm({1, 2, 3, 4, 5, 6, 7, 8, 9, 0}));

    REQUIRE(o.size() == 252);
  }

  LIBSEMIGROUPS_TEST_CASE("Action",
                          "013",
                          "permutation on tuples, arrays",
                          "[quick][no-valgrind]") {
    auto rg    = ReportGuard(REPORT);
    using Perm = PermHelper<10>::type;

    RightAction<Perm,
                std::array<u_int8_t, 5>,
                OnTuples<Perm, u_int8_t, std::array<u_int8_t, 5>>>
        o;
    o.add_seed({0, 1, 2, 3, 4});
    o.add_generator(Perm({1, 0, 2, 3, 4, 5, 6, 7, 8, 9}));
    o.add_generator(Perm({1, 2, 3, 4, 5, 6, 7, 8, 9, 0}));

    REQUIRE(o.size() == 30240);
  }

  LIBSEMIGROUPS_TEST_CASE("Action",
                          "014",
                          "permutation on sets, vectors",
                          "[quick]") {
    auto rg    = ReportGuard(REPORT);
    using Perm = PermHelper<10>::type;

    RightAction<Perm, std::vector<u_int8_t>, OnSets<Perm, u_int8_t>> o;
    o.add_seed({0, 1, 2, 3, 4});
    o.add_generator(Perm({1, 0, 2, 3, 4, 5, 6, 7, 8, 9}));
    o.add_generator(Perm({1, 2, 3, 4, 5, 6, 7, 8, 9, 0}));
    REQUIRE(o.size() == 252);
  }

  LIBSEMIGROUPS_TEST_CASE("Action",
                          "015",
                          "permutation on tuples, vectors",
                          "[quick][no-valgrind]") {
    auto rg    = ReportGuard(REPORT);
    using Perm = PermHelper<10>::type;

    RightAction<Perm, std::vector<u_int8_t>, OnTuples<Perm, u_int8_t>> o;
    o.add_seed({0, 1, 2, 3, 4});
    o.add_generator(Perm({1, 0, 2, 3, 4, 5, 6, 7, 8, 9}));
    o.add_generator(Perm({1, 2, 3, 4, 5, 6, 7, 8, 9, 0}));

    REQUIRE(o.size() == 30240);
  }

  LIBSEMIGROUPS_TEST_CASE("Action", "016", "misc", "[quick]") {
    auto rg    = ReportGuard(REPORT);
    using Perm = PermHelper<8>::type;
    RightAction<Perm, u_int8_t, ImageRightAction<Perm, u_int8_t>> o;
    REQUIRE(o.current_size() == 0);
    REQUIRE(o.empty());
    REQUIRE_THROWS_AS(o.multiplier_to_scc_root(10), LibsemigroupsException);
    o.add_seed(0);
    REQUIRE(!o.empty());
    REQUIRE(std::vector<u_int8_t>(o.cbegin(), o.cend())
            == std::vector<u_int8_t>({0}));
    o.add_generator(Perm({1, 0, 2, 3, 4, 5, 6, 7}));
    o.add_generator(Perm({1, 2, 3, 4, 5, 6, 7, 0}));
    o.report_every(std::chrono::nanoseconds(10));

    REQUIRE(o.current_size() == 1);
    REQUIRE(o.size() == 8);
    REQUIRE(o.digraph().nr_scc() == 1);
    REQUIRE(o.position(10) == UNDEFINED);
    REQUIRE(o.current_size() == 8);
    REQUIRE_THROWS_AS(o.at(10), std::out_of_range);
    // REQUIRE_NOTHROW(o[10]);
    REQUIRE(o[0] == 0);
    REQUIRE(o[1] == 1);
    REQUIRE(o.at(0) == 0);
    REQUIRE(o.at(1) == 1);
    REQUIRE_THROWS_AS(o.multiplier_to_scc_root(10), LibsemigroupsException);
    REQUIRE_THROWS_AS(o.multiplier_from_scc_root(10), LibsemigroupsException);
    std::vector<u_int8_t> result(o.cbegin(), o.cend());
    std::sort(result.begin(), result.end());
    REQUIRE(result == std::vector<u_int8_t>({0, 1, 2, 3, 4, 5, 6, 7}));
  }

  LIBSEMIGROUPS_TEST_CASE("Action",
                          "017",
                          "partial perm image orbit",
                          "[quick]") {
    auto rg     = ReportGuard(REPORT);
    using PPerm = PPermHelper<3>::type;
    RightAction<PPerm, PPerm, ImageRightAction<PPerm, PPerm>> o;
    o.add_seed(PPerm({0, 1, 2}, {0, 1, 2}, 3));
    o.add_generator(PPerm({0, 1, 2}, {1, 2, 0}, 3));
    o.add_generator(PPerm({0, 1, 2}, {1, 0, 2}, 3));
    o.add_generator(PPerm({1, 2}, {0, 1}, 3));
    o.add_generator(PPerm({0, 1}, {1, 2}, 3));
    REQUIRE(o.size() == 8);
    REQUIRE(std::vector<PPerm>(o.cbegin(), o.cend())
            == std::vector<PPerm>({PPerm({0, 1, 2}, {0, 1, 2}, 3),
                                   PPerm({0, 1}, {0, 1}, 3),
                                   PPerm({1, 2}, {1, 2}, 3),
                                   PPerm({0}, {0}, 3),
                                   PPerm({0, 2}, {0, 2}, 3),
                                   PPerm({2}, {2}, 3),
                                   PPerm({1}, {1}, 3),
                                   PPerm({}, {}, 3)}));
    REQUIRE_THROWS_AS(o.digraph().cbegin_scc(10), LibsemigroupsException);
    REQUIRE(o.root_of_scc(PPerm({0, 2}, {0, 2}, 3))
            == PPerm({0, 2}, {0, 2}, 3));
    REQUIRE(o.root_of_scc(PPerm({0, 1}, {0, 1}, 3))
            == PPerm({0, 2}, {0, 2}, 3));
    REQUIRE_THROWS_AS(o.root_of_scc(PPerm({0, 3}, {0, 3}, 4)),
                      LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("Action",
                          "018",
                          "permutation on tuples, arrays (360360)",
                          "[quick][no-valgrind]") {
    auto rg    = ReportGuard(REPORT);
    using Perm = PermHelper<15>::type;

    RightAction<Perm,
                std::array<u_int8_t, 5>,
                OnTuples<Perm, u_int8_t, std::array<u_int8_t, 5>>>
        o;
    o.add_seed({0, 1, 2, 3, 4});
    o.add_generator(Perm({1, 0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14}));
    o.add_generator(Perm({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 0}));

    REQUIRE(o.size() == 360360);
  }
}  // namespace libsemigroups
