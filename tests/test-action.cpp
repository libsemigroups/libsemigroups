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

// TODO(later):
// 1. add examples from Action

#include <algorithm>  // for sort
#include <cstdint>    // for uint8_t
#include <stdexcept>  // for out_of_range
#include <vector>     // for vector

#include "libsemigroups/action.hpp"  // for LeftAction, RightAction
#include "libsemigroups/bmat.hpp"    // for BMat adapters
#include "libsemigroups/bmat8.hpp"   // for BMat8 etc
#include "libsemigroups/matrix.hpp"  // for BMat
#include "libsemigroups/report.hpp"  // for ReportGuard
#include "libsemigroups/transf.hpp"  // for PPerm<>

#include "catch.hpp"      // for REQUIRE, REQUIRE_THROWS_AS, REQUI...
#include "test-main.hpp"  // for LIBSEMIGROUPS_TEST_CASE

namespace libsemigroups {
  struct LibsemigroupsException;  // forward decl
  constexpr bool REPORT = false;

  using row_action_type = ImageRightAction<BMat8, BMat8>;
  using col_action_type = ImageLeftAction<BMat8, BMat8>;
  using row_orb_type    = RightAction<BMat8, BMat8, row_action_type>;
  using col_orb_type    = LeftAction<BMat8, BMat8, col_action_type>;

  namespace {
    template <typename Mat>  // = BMat..
    void test000() {
      auto rg = ReportGuard(REPORT);
      using boolmat_row_action_type
          = ImageRightAction<Mat, detail::StaticVector1<BitSet<5>, 5>>;
      using boolmat_col_action_type
          = ImageLeftAction<Mat, detail::StaticVector1<BitSet<5>, 5>>;
      using boolmat_row_orb_type
          = RightAction<Mat,
                        detail::StaticVector1<BitSet<5>, 5>,
                        boolmat_row_action_type>;
      using boolmat_col_orb_type
          = LeftAction<Mat,
                       detail::StaticVector1<BitSet<5>, 5>,
                       boolmat_col_action_type>;
      const std::vector<Mat> reg_bmat5_gens = {Mat({{0, 1, 0, 0, 0},
                                                    {1, 0, 0, 0, 0},
                                                    {0, 0, 1, 0, 0},
                                                    {0, 0, 0, 1, 0},
                                                    {0, 0, 0, 0, 1}}),
                                               Mat({{0, 1, 0, 0, 0},
                                                    {0, 0, 1, 0, 0},
                                                    {0, 0, 0, 1, 0},
                                                    {0, 0, 0, 0, 1},
                                                    {1, 0, 0, 0, 0}}),
                                               Mat({{1, 0, 0, 0, 0},
                                                    {1, 1, 0, 0, 0},
                                                    {0, 0, 1, 0, 0},
                                                    {0, 0, 0, 1, 0},
                                                    {0, 0, 0, 0, 1}}),
                                               Mat({{0, 0, 0, 0, 0},
                                                    {0, 1, 0, 0, 0},
                                                    {0, 0, 1, 0, 0},
                                                    {0, 0, 0, 1, 0},
                                                    {0, 0, 0, 0, 1}})};
      boolmat_row_orb_type   row_orb;
      boolmat_col_orb_type   col_orb;

      row_orb.add_seed({BitSet<5>(0x10),
                        BitSet<5>(0x8),
                        BitSet<5>(0x4),
                        BitSet<5>(0x2),
                        BitSet<5>(0x1)});
      col_orb.add_seed({BitSet<5>(0x10),
                        BitSet<5>(0x8),
                        BitSet<5>(0x4),
                        BitSet<5>(0x2),
                        BitSet<5>(0x1)});
      for (Mat g : reg_bmat5_gens) {
        row_orb.add_generator(g);
        col_orb.add_generator(g);
      }
      row_orb.run_for(std::chrono::milliseconds(100));
      row_orb.run_for(std::chrono::milliseconds(100));
      row_orb.run_for(std::chrono::milliseconds(100));
      col_orb.run_for(std::chrono::milliseconds(100));
      col_orb.run_for(std::chrono::milliseconds(100));
      col_orb.run_for(std::chrono::milliseconds(100));

      REQUIRE(row_orb.size() == 110519);
      REQUIRE(col_orb.size() == 110519);
    }
  }  // namespace

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
    row_orb.cache_scc_multipliers(true);

    REQUIRE(row_orb.size() == 553);
    REQUIRE(row_orb.digraph().number_of_scc() == 14);
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
                          "[extreme]") {
    // auto                     rg             = ReportGuard(REPORT);
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
    // row_orb.run_for(std::chrono::milliseconds(500));

    REQUIRE(row_orb.size() == 37977468);
  }

  LIBSEMIGROUPS_TEST_CASE("Action",
                          "007",
                          "partial perm image orbit",
                          "[quick]") {
    auto rg = ReportGuard(REPORT);
    RightAction<PPerm<8>, PPerm<8>, ImageRightAction<PPerm<8>, PPerm<8>>> o;
    o.add_seed(PPerm<8>::identity(8));
    o.add_generator(
        PPerm<8>({0, 1, 2, 3, 4, 5, 6, 7}, {1, 2, 3, 4, 5, 6, 7, 0}, 8));
    o.add_generator(
        PPerm<8>({0, 1, 2, 3, 4, 5, 6, 7}, {1, 0, 2, 3, 4, 5, 6, 7}, 8));
    o.add_generator(PPerm<8>({1, 2, 3, 4, 5, 6, 7}, {0, 1, 2, 3, 4, 5, 6}, 8));
    o.add_generator(PPerm<8>({0, 1, 2, 3, 4, 5, 6}, {1, 2, 3, 4, 5, 6, 7}, 8));
    REQUIRE(o.size() == 256);
  }

  LIBSEMIGROUPS_TEST_CASE("Action",
                          "008",
                          "partial perm image orbit",
                          "[quick][no-valgrind]") {
    auto rg = ReportGuard(REPORT);
    RightAction<PPerm<16>, PPerm<16>, ImageRightAction<PPerm<16>, PPerm<16>>> o;
    o.add_seed(PPerm<16>::identity(16));
    o.add_generator(
        PPerm<16>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
                  {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0},
                  16));
    o.add_generator(
        PPerm<16>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
                  {1, 0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
                  16));
    o.add_generator(
        PPerm<16>({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
                  {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14},
                  16));
    o.add_generator(
        PPerm<16>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14},
                  {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
                  16));
    o.reserve(70000);
    REQUIRE(o.size() == 65536);
  }

  LIBSEMIGROUPS_TEST_CASE("Action",
                          "009",
                          "partial perm image orbit",
                          "[quick][no-valgrind]") {
    auto rg = ReportGuard(REPORT);
    RightAction<PPerm<16>, PPerm<16>, ImageRightAction<PPerm<16>, PPerm<16>>> o;
    o.add_seed(One<PPerm<16>>()(16));
    o.add_generator(
        PPerm<16>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
                  {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0},
                  16));
    o.add_generator(
        PPerm<16>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
                  {1, 0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
                  16));
    o.add_generator(
        PPerm<16>({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
                  {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14},
                  16));
    o.add_generator(
        PPerm<16>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14},
                  {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
                  16));
    o.reserve(70000);
    REQUIRE(o.size() == 65536);
    REQUIRE(o.digraph().number_of_scc() == 17);
  }

  LIBSEMIGROUPS_TEST_CASE("Action",
                          "010",
                          "partial perm image orbit",
                          "[quick][no-valgrind]") {
    auto rg = ReportGuard(REPORT);
    LeftAction<PPerm<16>, PPerm<16>, ImageLeftAction<PPerm<16>, PPerm<16>>> o;
    o.add_seed(One<PPerm<16>>()(16));
    o.add_generator(
        PPerm<16>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
                  {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0},
                  16));
    o.add_generator(
        PPerm<16>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
                  {1, 0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
                  16));
    o.add_generator(
        PPerm<16>({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
                  {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14},
                  16));
    o.add_generator(
        PPerm<16>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14},
                  {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
                  16));
    REQUIRE(o.size() == 65536);
    REQUIRE(o.digraph().number_of_scc() == 17);
  }

  LIBSEMIGROUPS_TEST_CASE("Action",
                          "011",
                          "permutation on integers",
                          "[quick]") {
    auto rg    = ReportGuard(REPORT);
    using Perm = LeastPerm<8>;
    RightAction<Perm, uint8_t, ImageRightAction<Perm, uint8_t>> o;
    o.add_seed(0);
    o.add_generator(Perm({1, 0, 2, 3, 4, 5, 6, 7}));
    o.add_generator(Perm({1, 2, 3, 4, 5, 6, 7, 0}));

    REQUIRE(o.size() == 8);
    REQUIRE(o.digraph().number_of_scc() == 1);
  }

  LIBSEMIGROUPS_TEST_CASE("Action",
                          "012",
                          "permutation on sets, arrays",
                          "[quick]") {
    auto rg    = ReportGuard(REPORT);
    using Perm = LeastPerm<10>;

    RightAction<Perm,
                std::array<uint8_t, 5>,
                OnSets<Perm, uint8_t, std::array<uint8_t, 5>>>
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
    using Perm = LeastPerm<10>;

    RightAction<Perm,
                std::array<uint8_t, 5>,
                OnTuples<Perm, uint8_t, std::array<uint8_t, 5>>>
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
    using Perm = LeastPerm<10>;

    RightAction<Perm, std::vector<uint8_t>, OnSets<Perm, uint8_t>> o;
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
    using Perm = LeastPerm<10>;

    RightAction<Perm, std::vector<uint8_t>, OnTuples<Perm, uint8_t>> o;
    o.add_seed({0, 1, 2, 3, 4});
    o.add_generator(Perm({1, 0, 2, 3, 4, 5, 6, 7, 8, 9}));
    o.add_generator(Perm({1, 2, 3, 4, 5, 6, 7, 8, 9, 0}));

    REQUIRE(o.size() == 30240);
  }

  LIBSEMIGROUPS_TEST_CASE("Action", "016", "misc", "[quick]") {
    auto rg    = ReportGuard(REPORT);
    using Perm = LeastPerm<8>;
    RightAction<Perm, uint8_t, ImageRightAction<Perm, uint8_t>> o;
    REQUIRE(o.current_size() == 0);
    REQUIRE(o.empty());
    REQUIRE_THROWS_AS(o.multiplier_to_scc_root(10), LibsemigroupsException);
    o.add_seed(0);
    REQUIRE(!o.empty());
    REQUIRE(std::vector<uint8_t>(o.cbegin(), o.cend())
            == std::vector<uint8_t>({0}));
    o.add_generator(Perm({1, 0, 2, 3, 4, 5, 6, 7}));
    o.add_generator(Perm({1, 2, 3, 4, 5, 6, 7, 0}));
    o.report_every(std::chrono::nanoseconds(10));

    REQUIRE(o.current_size() == 1);
    REQUIRE(o.size() == 8);
    REQUIRE(o.digraph().number_of_scc() == 1);
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
    std::vector<uint8_t> result(o.cbegin(), o.cend());
    std::sort(result.begin(), result.end());
    REQUIRE(result == std::vector<uint8_t>({0, 1, 2, 3, 4, 5, 6, 7}));
  }

  LIBSEMIGROUPS_TEST_CASE("Action",
                          "017",
                          "partial perm image orbit",
                          "[quick]") {
    auto rg = ReportGuard(REPORT);
    RightAction<PPerm<3>, PPerm<3>, ImageRightAction<PPerm<3>, PPerm<3>>> o;
    o.add_seed(PPerm<3>({0, 1, 2}, {0, 1, 2}, 3));
    o.add_generator(PPerm<3>({0, 1, 2}, {1, 2, 0}, 3));
    o.add_generator(PPerm<3>({0, 1, 2}, {1, 0, 2}, 3));
    o.add_generator(PPerm<3>({1, 2}, {0, 1}, 3));
    o.add_generator(PPerm<3>({0, 1}, {1, 2}, 3));
    REQUIRE(o.size() == 8);
    REQUIRE(std::vector<PPerm<3>>(o.cbegin(), o.cend())
            == std::vector<PPerm<3>>({PPerm<3>({0, 1, 2}, {0, 1, 2}, 3),
                                      PPerm<3>({0, 1}, {0, 1}, 3),
                                      PPerm<3>({1, 2}, {1, 2}, 3),
                                      PPerm<3>({0}, {0}, 3),
                                      PPerm<3>({0, 2}, {0, 2}, 3),
                                      PPerm<3>({2}, {2}, 3),
                                      PPerm<3>({1}, {1}, 3),
                                      PPerm<3>({}, {}, 3)}));
    REQUIRE_THROWS_AS(o.digraph().cbegin_scc(10), LibsemigroupsException);
    REQUIRE(o.root_of_scc(PPerm<3>({0, 2}, {0, 2}, 3))
            == PPerm<3>({0, 2}, {0, 2}, 3));
    REQUIRE(o.root_of_scc(PPerm<3>({0, 1}, {0, 1}, 3))
            == PPerm<3>({0, 2}, {0, 2}, 3));
    REQUIRE_THROWS_AS(o.root_of_scc(PPerm<3>::make({0, 3}, {0, 3}, 4)),
                      LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("Action",
                          "018",
                          "permutation on tuples, arrays (360360)",
                          "[quick][no-valgrind]") {
    auto rg    = ReportGuard(REPORT);
    using Perm = LeastPerm<15>;

    RightAction<Perm,
                std::array<uint8_t, 5>,
                OnTuples<Perm, uint8_t, std::array<uint8_t, 5>>>
        o;
    o.add_seed({0, 1, 2, 3, 4});
    o.add_generator(Perm({1, 0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14}));
    o.add_generator(Perm({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 0}));

    REQUIRE(o.size() == 360360);
  }

  LIBSEMIGROUPS_TEST_CASE("Action",
                          "019",
                          "orbits for regular BMat8 monoid 5 with stop/start",
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

    row_orb_type row_orb;
    col_orb_type col_orb;

    row_orb.add_seed(BMat8::one());
    col_orb.add_seed(BMat8::one());
    for (BMat8 g : reg_bmat5_gens) {
      row_orb.add_generator(g);
      col_orb.add_generator(g);
    }
    row_orb.run_for(std::chrono::milliseconds(100));
    row_orb.run_for(std::chrono::milliseconds(100));
    row_orb.run_for(std::chrono::milliseconds(100));
    col_orb.run_for(std::chrono::milliseconds(100));
    col_orb.run_for(std::chrono::milliseconds(100));
    col_orb.run_for(std::chrono::milliseconds(100));

    REQUIRE(row_orb.size() == 110519);
    REQUIRE(col_orb.size() == 110519);
  }

  LIBSEMIGROUPS_TEST_CASE(
      "Action",
      "020",
      "orbits for regular boolean mat monoid 5 (BMat<>) with stop/start",
      "[standard][no-valgrind]") {
    test000<BMat<>>();
  }

  LIBSEMIGROUPS_TEST_CASE(
      "Action",
      "021",
      "orbits for regular boolean mat monoid 5 (BMat<5>) with stop/start",
      "[quick][no-valgrind]") {
    test000<BMat<5>>();
  }
}  // namespace libsemigroups
