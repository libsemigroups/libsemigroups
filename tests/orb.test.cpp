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
#include "element.hpp"
#include "hpcombi.hpp"
#include "libsemigroups.tests.hpp"
#include "orb.hpp"
#include "report.hpp"
#include "timer.hpp"

namespace libsemigroups {

  using row_action_type = right_action<BMat8, BMat8>;
  using col_action_type = left_action<BMat8, BMat8>;
  using row_orb_type    = Orb<BMat8, BMat8, row_action_type, Side::RIGHT>;
  using col_orb_type    = Orb<BMat8, BMat8, col_action_type, Side::LEFT>;

  LIBSEMIGROUPS_TEST_CASE("Orb",
                          "001",
                          "row and column basis orbits for BMat8",
                          "[quick]") {
    row_orb_type row_orb;
    row_orb.add_seed(BMat8({{1, 0, 0}, {0, 1, 0}, {0, 0, 0}}));
    row_orb.add_generator(BMat8({{0, 1, 0}, {1, 0, 0}, {0, 0, 1}}));

    REQUIRE(row_orb.size() == 1);

    col_orb_type col_orb;
    col_orb.add_seed(BMat8({{1, 0, 0}, {0, 1, 0}, {0, 0, 0}}));
    col_orb.add_generator(BMat8({{0, 1, 0}, {1, 0, 0}, {0, 0, 1}}));

    REQUIRE(col_orb.size() == 1);
  }

  LIBSEMIGROUPS_TEST_CASE("Orb",
                          "002",
                          "row and column basis orbits for BMat8",
                          "[quick]") {
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

  LIBSEMIGROUPS_TEST_CASE("Orb",
                          "003",
                          "add generators after enumeration",
                          "[quick]") {
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

  LIBSEMIGROUPS_TEST_CASE("Orb",
                          "004",
                          "multipliers for BMat8 row and column orbits",
                          "[quick]") {
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
    REQUIRE(row_orb.nr_scc() == 14);
    REQUIRE(std::vector<size_t>(row_orb.cbegin_scc_roots(),
                                row_orb.cend_scc_roots())
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

  LIBSEMIGROUPS_TEST_CASE("Orb",
                          "005",
                          "orbits for regular boolean mat monoid 5",
                          "[standard]") {
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
    row_orb.enumerate();
    col_orb.enumerate();

    REQUIRE(row_orb.size() == 110519);
    REQUIRE(col_orb.size() == 110519);
  }

  LIBSEMIGROUPS_TEST_CASE("Orb",
                          "006",
                          "orbits for regular boolean mat monoid 6",
                          "[standard]") {
    auto                     rg             = ReportGuard();
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

  template <>
  struct right_action<PartialPerm<uint_fast8_t>, PartialPerm<uint_fast8_t>> {
    void operator()(PartialPerm<uint_fast8_t>&       res,
                    PartialPerm<uint_fast8_t> const& pt,
                    PartialPerm<uint_fast8_t> const& x) const noexcept {
      res.redefine(pt, x);
      res.swap(res.image());
    }
  };

  LIBSEMIGROUPS_TEST_CASE("Orb", "007", "partial perm image orbit", "[quick]") {
    // auto rg = ReportGuard();
    using PPerm = PartialPerm<uint_fast8_t>;
    Orb<PPerm, PPerm, right_action<PPerm, PPerm>> o;
    o.add_seed(PPerm::identity(8));
    o.add_generator(
        PPerm({0, 1, 2, 3, 4, 5, 6, 7}, {1, 2, 3, 4, 5, 6, 7, 0}, 8));
    o.add_generator(
        PPerm({0, 1, 2, 3, 4, 5, 6, 7}, {1, 0, 2, 3, 4, 5, 6, 7}, 8));
    o.add_generator(PPerm({1, 2, 3, 4, 5, 6, 7}, {0, 1, 2, 3, 4, 5, 6}, 8));
    o.add_generator(PPerm({0, 1, 2, 3, 4, 5, 6}, {1, 2, 3, 4, 5, 6, 7}, 8));
    REQUIRE(o.size() == 256);
  }

  LIBSEMIGROUPS_TEST_CASE("Orb",
                          "008",
                          "partial perm image orbit",
                          "[standard]") {
    // auto rg = ReportGuard();
    using PPerm = PartialPerm<uint_fast8_t>;
    Orb<PPerm, PPerm, right_action<PPerm, PPerm>> o;
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
    REQUIRE(o.size() == 65536);
  }

  template <>
  struct right_action<HPCombi::PPerm16, HPCombi::PPerm16> {
    void operator()(HPCombi::PPerm16&       res,
                    HPCombi::PPerm16 const& pt,
                    HPCombi::PPerm16 const& x) const noexcept {
      res = (x * pt).left_one();
    }
  };


  LIBSEMIGROUPS_TEST_CASE("Orb",
                          "009",
                          "partial perm image orbit",
                          "[standard]") {
    // auto rg = ReportGuard();
    using PPerm = HPCombi::PPerm16;
    Orb<PPerm, PPerm, right_action<PPerm, PPerm>> o;
    o.add_seed(PPerm::one());
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
    REQUIRE(o.action_digraph().nr_scc() == 17);
  }
  template <>
  struct left_action<HPCombi::PPerm16, HPCombi::PPerm16> {
    void operator()(HPCombi::PPerm16&       res,
                    HPCombi::PPerm16 const& pt,
                    HPCombi::PPerm16 const& x) const noexcept {
      res = (pt * x).right_one();
    }
  };
  LIBSEMIGROUPS_TEST_CASE("Orb",
                          "010",
                          "partial perm image orbit",
                          "[standard]") {
    // auto rg = ReportGuard();
    using PPerm = HPCombi::PPerm16;
    Orb<PPerm, PPerm, left_action<PPerm, PPerm>, Side::LEFT> o;
    o.add_seed(PPerm::one());
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
    REQUIRE(o.action_digraph().nr_scc() == 17);
  }
}  // namespace libsemigroups

