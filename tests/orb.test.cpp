// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2018 Finn Smith
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

#include "bmat8.hpp"
#include "libsemigroups.tests.hpp"
#include "orb.hpp"
#include "report.hpp"
#include "timer.hpp"

namespace libsemigroups {

  TEST_CASE("Minimal row_orb 01", "[quick][orb-min][01]") {
    RowOrb row_orb;
    row_orb.add_seed(BMat8({{1, 0, 0}, {0, 1, 0}, {0, 0, 0}}));
    row_orb.add_generator(BMat8({{0, 1, 0}, {1, 0, 0}, {0, 0, 1}}));

    REQUIRE(row_orb.size() == 1);

    ColOrb col_orb;
    col_orb.add_seed(BMat8({{1, 0, 0}, {0, 1, 0}, {0, 0, 0}}));
    col_orb.add_generator(BMat8({{0, 1, 0}, {1, 0, 0}, {0, 0, 1}}));

    REQUIRE(col_orb.size() == 1);
  }

  TEST_CASE("Minimal row_orb 02", "[quick][orb-min][02]") {
    RowOrb row_orb;
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

    ColOrb col_orb;
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

  TEST_CASE("Minimal row_orb 03", "[quick][orb-min][03]") {
    RowOrb row_orb;
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

    ColOrb col_orb;
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

  TEST_CASE("Minimal row_orb 04: multipliers", "[quick][orb-min][04]") {
    RowOrb row_orb;
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

    for (size_t i = 0; i < row_orb.size(); ++i) {
      REQUIRE((row_orb.at(i) * row_orb.multiplier_to_scc_root(i)
               * row_orb.multiplier_from_scc_root(i))
                  .row_space_basis()
              == row_orb.at(i));
    }

    ColOrb col_orb;
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

  TEST_CASE("Minimal row_orb 05", "[orb-min][05][standard]") {
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
    RowOrb                   row_orb;
    ColOrb                   col_orb;

    row_orb.add_seed(BMat8::one());
    col_orb.add_seed(BMat8::one());
    for (BMat8 g : reg_bmat5_gens) {
      row_orb.add_generator(g);
      col_orb.add_generator(g);
    }
    row_orb.enumerate();
    col_orb.enumerate();

    REQUIRE(row_orb.size() == 110519);
    REQUIRE(col_orb.size() == 110519);
  }

  TEST_CASE("Minimal row_orb 06", "[orb-min][06][extreme]") {
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
    RowOrb                   row_orb;

    row_orb.add_seed(BMat8::one());
    for (BMat8 g : reg_bmat6_gens) {
      row_orb.add_generator(g);
    }
    row_orb.enumerate();

    REQUIRE(row_orb.size() == 37977468);
  }
}  // namespace libsemigroups
