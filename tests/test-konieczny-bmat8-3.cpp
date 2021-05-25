// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2020 Finn Smith
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

#include <cstddef>  // for size_t

#include "catch.hpp"      // for REQUIRE
#include "test-main.hpp"  // FOR LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/bmat8.hpp"         // for BMat8
#include "libsemigroups/fastest-bmat.hpp"  // for FastestBMat
#include "libsemigroups/konieczny.hpp"     // for Konieczny

namespace libsemigroups {

  constexpr bool REPORT = false;
  namespace {
    void test_it(Konieczny<BMat8>&           KS,
                 std::vector<BMat8>&         non_reg_reps,
                 size_t                      i,
                 std::array<size_t, 4> const v) {
      Konieczny<BMat8>::DClass& X = KS.D_class_of_element(non_reg_reps[i]);
      REQUIRE(X.size() == v[0]);
      REQUIRE(X.size_H_class() == v[1]);
      REQUIRE(X.number_of_L_classes() == v[2]);
      REQUIRE(X.number_of_R_classes() == v[3]);
    }
  }  // namespace

  LIBSEMIGROUPS_TEST_CASE("Konieczny",
                          "026",
                          "non-regular D-classes 02",
                          "[quick][bmat8]") {
    auto                     rg = ReportGuard(REPORT);
    std::vector<BMat8> const gens
        = {BMat8({{0, 1, 0, 0}, {1, 0, 0, 1}, {1, 0, 0, 1}, {0, 1, 1, 0}}),
           BMat8({{0, 1, 0, 1}, {0, 1, 1, 1}, {0, 0, 1, 0}, {1, 1, 1, 1}}),
           BMat8({{1, 1, 0, 1}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 1, 0, 1}}),
           BMat8({{0, 0, 1, 0}, {0, 0, 1, 1}, {0, 0, 0, 0}, {1, 0, 0, 0}}),
           BMat8({{1, 1, 0, 1}, {1, 1, 1, 1}, {1, 0, 1, 0}, {0, 1, 1, 0}}),
           BMat8({{0, 1, 0, 0}, {0, 1, 1, 0}, {1, 0, 1, 0}, {0, 1, 0, 1}}),
           BMat8({{0, 1, 0, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}, {0, 0, 1, 0}})};

    std::vector<BMat8> const idems
        = {BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
           BMat8({{1, 1, 1, 1}, {0, 1, 0, 0}, {0, 1, 1, 0}, {0, 1, 0, 1}}),
           BMat8({{1, 1, 0, 1}, {0, 1, 0, 1}, {0, 1, 1, 1}, {0, 0, 0, 0}}),
           BMat8({{1, 1, 1, 1}, {0, 1, 0, 1}, {0, 0, 1, 0}, {0, 0, 0, 0}}),
           BMat8({{1, 0, 0, 1}, {0, 1, 0, 0}, {0, 1, 1, 0}, {1, 0, 0, 1}}),
           BMat8({{1, 0, 0, 1}, {1, 1, 0, 1}, {1, 1, 1, 1}, {1, 0, 0, 1}}),
           BMat8({{1, 0, 0, 1}, {0, 1, 1, 0}, {0, 1, 1, 0}, {1, 0, 0, 1}}),
           BMat8({{1, 1, 1, 1}, {0, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}}),
           BMat8({{1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}}),
           BMat8({{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}})};

    Konieczny<BMat8> KS(gens);
    KS.run();

    size_t count = 0;
    for (BMat8 id : idems) {
      Konieczny<BMat8>::DClass& D = KS.D_class_of_element(id);
      count += D.size();
    }

    REQUIRE(size_t(KS.cend_regular_D_classes() - KS.cbegin_regular_D_classes())
            == idems.size());

    REQUIRE(count == 8712);

    std::vector<BMat8> non_reg_reps
        = {BMat8({{1, 1, 1, 1}, {1, 1, 1, 1}, {0, 1, 1, 1}, {1, 1, 1, 0}}),
           BMat8({{0, 0, 1, 0}, {0, 0, 1, 1}, {0, 0, 0, 0}, {1, 0, 0, 0}}),
           BMat8({{1, 0, 0, 1}, {1, 1, 1, 1}, {0, 0, 0, 0}, {0, 1, 0, 0}}),
           BMat8({{1, 0, 0, 0}, {1, 0, 1, 0}, {0, 0, 0, 0}, {0, 0, 1, 1}}),
           BMat8({{1, 0, 1, 0}, {0, 0, 1, 1}, {0, 0, 0, 0}, {0, 0, 1, 0}}),
           BMat8({{0, 1, 1, 0}, {1, 1, 1, 1}, {0, 0, 0, 0}, {0, 1, 0, 1}}),
           BMat8({{0, 0, 0, 0}, {0, 1, 0, 1}, {0, 1, 1, 1}, {1, 1, 1, 0}}),
           BMat8({{0, 1, 0, 1}, {0, 1, 1, 1}, {0, 0, 0, 0}, {1, 0, 1, 0}}),
           BMat8({{0, 1, 0, 1}, {0, 1, 1, 1}, {0, 0, 0, 0}, {1, 1, 1, 0}}),
           BMat8({{1, 0, 1, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {1, 1, 0, 1}}),
           BMat8({{0, 0, 0, 0}, {0, 1, 1, 0}, {0, 1, 1, 1}, {1, 1, 0, 1}}),
           BMat8({{1, 0, 0, 1}, {1, 1, 0, 1}, {0, 0, 0, 0}, {0, 1, 1, 0}}),
           BMat8({{0, 1, 1, 0}, {0, 1, 1, 1}, {0, 0, 0, 0}, {1, 1, 0, 1}}),
           BMat8({{1, 1, 0, 1}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 1, 0, 1}}),
           BMat8({{0, 1, 1, 1}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 1, 0, 1}}),
           BMat8({{1, 1, 1, 0}, {0, 1, 0, 1}, {0, 0, 0, 0}, {0, 1, 1, 0}}),
           BMat8({{0, 1, 1, 1}, {1, 1, 0, 1}, {0, 0, 0, 0}, {0, 1, 1, 0}}),
           BMat8({{0, 1, 1, 1}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 1, 1, 0}}),
           BMat8({{1, 1, 1, 1}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 1, 0, 1}}),
           BMat8({{0, 0, 1, 0}, {1, 0, 1, 0}, {1, 0, 0, 0}, {0, 0, 1, 1}}),
           BMat8({{0, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 0, 1}}),
           BMat8({{0, 0, 1, 0}, {1, 0, 1, 0}, {1, 0, 1, 0}, {0, 0, 1, 1}}),
           BMat8({{0, 0, 1, 0}, {1, 0, 1, 0}, {1, 0, 1, 1}, {0, 0, 1, 1}}),
           BMat8({{0, 1, 0, 0}, {0, 1, 0, 1}, {1, 1, 1, 1}, {1, 1, 1, 0}}),
           BMat8({{0, 0, 1, 0}, {0, 0, 1, 1}, {0, 0, 1, 1}, {1, 0, 1, 0}}),
           BMat8({{1, 0, 0, 0}, {1, 0, 1, 0}, {0, 0, 1, 1}, {1, 0, 0, 0}}),
           BMat8({{0, 1, 0, 0}, {0, 1, 1, 0}, {1, 1, 1, 1}, {1, 1, 0, 1}}),
           BMat8({{0, 1, 0, 0}, {0, 1, 1, 0}, {1, 0, 1, 0}, {0, 1, 0, 1}}),
           BMat8({{0, 0, 1, 1}, {1, 0, 1, 1}, {1, 0, 0, 0}, {1, 0, 1, 0}}),
           BMat8({{1, 0, 0, 0}, {0, 0, 1, 1}, {0, 0, 1, 1}, {1, 0, 1, 0}}),
           BMat8({{1, 0, 0, 0}, {1, 0, 1, 1}, {0, 0, 1, 1}, {1, 0, 1, 0}}),
           BMat8({{0, 1, 0, 0}, {1, 1, 1, 0}, {1, 1, 1, 0}, {0, 1, 0, 1}}),
           BMat8({{0, 1, 0, 1}, {0, 1, 0, 1}, {0, 1, 1, 0}, {1, 1, 1, 1}}),
           BMat8({{0, 1, 0, 1}, {0, 1, 0, 1}, {1, 1, 1, 0}, {0, 1, 1, 1}}),
           BMat8({{0, 1, 0, 1}, {1, 1, 0, 1}, {1, 1, 0, 1}, {0, 1, 1, 1}}),
           BMat8({{0, 1, 0, 1}, {1, 1, 0, 1}, {1, 1, 1, 1}, {0, 1, 1, 1}}),
           BMat8({{0, 1, 0, 1}, {0, 1, 1, 0}, {0, 1, 1, 0}, {1, 1, 0, 1}}),
           BMat8({{0, 1, 0, 1}, {0, 1, 1, 0}, {0, 1, 1, 0}, {1, 1, 1, 1}}),
           BMat8({{0, 1, 1, 0}, {0, 1, 0, 1}, {0, 1, 0, 1}, {1, 1, 1, 0}}),
           BMat8({{1, 1, 1, 1}, {1, 1, 1, 1}, {0, 1, 1, 0}, {0, 1, 0, 1}}),
           BMat8({{1, 0, 1, 0}, {1, 1, 1, 0}, {0, 1, 1, 1}, {1, 1, 1, 0}}),
           BMat8({{1, 1, 1, 0}, {1, 1, 1, 1}, {0, 1, 1, 1}, {0, 1, 0, 1}}),
           BMat8({{1, 0, 1, 0}, {0, 1, 0, 1}, {0, 1, 0, 1}, {1, 1, 1, 0}}),
           BMat8({{0, 1, 0, 1}, {1, 1, 1, 1}, {1, 0, 1, 0}, {0, 1, 1, 1}}),
           BMat8({{0, 1, 0, 1}, {1, 1, 1, 0}, {1, 1, 1, 0}, {0, 1, 1, 1}}),
           BMat8({{1, 1, 0, 1}, {1, 1, 1, 1}, {1, 0, 1, 0}, {0, 1, 1, 0}}),
           BMat8({{0, 1, 0, 1}, {1, 1, 1, 1}, {1, 1, 1, 0}, {0, 1, 1, 1}}),
           BMat8({{1, 0, 1, 0}, {1, 1, 1, 1}, {1, 1, 0, 1}, {1, 1, 1, 0}}),
           BMat8({{0, 1, 1, 0}, {0, 1, 1, 0}, {1, 1, 0, 1}, {0, 1, 1, 1}}),
           BMat8({{0, 1, 1, 0}, {0, 1, 1, 1}, {0, 1, 1, 1}, {1, 1, 1, 0}}),
           BMat8({{1, 1, 0, 1}, {1, 1, 1, 1}, {0, 1, 1, 1}, {0, 1, 1, 0}}),
           BMat8({{0, 1, 1, 0}, {1, 1, 1, 0}, {1, 1, 1, 1}, {0, 1, 1, 1}}),
           BMat8({{1, 0, 0, 1}, {1, 1, 1, 1}, {0, 1, 1, 0}, {1, 1, 0, 1}}),
           BMat8({{0, 1, 1, 0}, {1, 1, 0, 1}, {1, 1, 0, 1}, {0, 1, 1, 1}}),
           BMat8({{0, 1, 1, 0}, {1, 1, 1, 1}, {1, 1, 0, 1}, {0, 1, 1, 1}}),
           BMat8({{0, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 0}})};

    std::vector<std::array<size_t, 4>> const expected
        = {{180, 2, 2, 45}, {16, 1, 4, 4}, {64, 1, 4, 16}, {16, 1, 4, 4},
           {32, 1, 4, 8},   {64, 2, 4, 8}, {16, 1, 4, 4},  {16, 1, 4, 4},
           {16, 1, 4, 4},   {16, 1, 4, 4}, {16, 1, 4, 4},  {16, 1, 4, 4},
           {16, 1, 4, 4},   {16, 1, 4, 4}, {16, 1, 4, 4},  {16, 1, 4, 4},
           {16, 1, 4, 4},   {16, 2, 2, 4}, {32, 1, 8, 4},  {16, 1, 4, 4},
           {156, 2, 39, 2}, {16, 1, 4, 4}, {32, 1, 4, 8},  {32, 1, 4, 8},
           {16, 1, 4, 4},   {16, 1, 4, 4}, {32, 1, 4, 8},  {16, 1, 4, 4},
           {16, 1, 4, 4},   {16, 1, 4, 4}, {16, 1, 4, 4},  {16, 1, 4, 4},
           {32, 1, 8, 4},   {16, 1, 4, 4}, {32, 1, 8, 4},  {32, 2, 4, 4},
           {16, 1, 4, 4},   {32, 1, 8, 4}, {16, 1, 4, 4},  {32, 1, 8, 4},
           {16, 1, 4, 4},   {16, 1, 4, 4}, {16, 1, 4, 4},  {16, 1, 4, 4},
           {16, 1, 4, 4},   {16, 1, 4, 4}, {16, 1, 4, 4},  {16, 1, 4, 4},
           {16, 1, 4, 4},   {16, 1, 4, 4}, {16, 1, 4, 4},  {16, 2, 2, 4},
           {16, 1, 4, 4},   {16, 1, 4, 4}, {16, 1, 4, 4},  {8, 2, 2, 2}};

    for (size_t i = 0; i < non_reg_reps.size(); ++i) {
      test_it(KS, non_reg_reps, i, expected[i]);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("Konieczny",
                          "027",
                          "Hall monoid 5",
                          "[extreme][bmat8]") {
#ifdef LIBSEMIGROUPS_HPCOMBI_ENABLED
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#pragma GCC diagnostic ignored "-Winline"
#endif
    auto rg    = ReportGuard();
    using BMat = FastestBMat<5>;
    Konieczny<BMat> K;
    K.add_generator(BMat({{0, 1, 0, 0, 0},
                          {0, 0, 1, 0, 0},
                          {0, 0, 0, 1, 0},
                          {0, 0, 0, 0, 1},
                          {1, 0, 0, 0, 0}}));
    K.add_generator(BMat({{0, 1, 0, 0, 0},
                          {1, 0, 0, 0, 0},
                          {0, 0, 1, 0, 0},
                          {0, 0, 0, 1, 0},
                          {0, 0, 0, 0, 1}}));
    K.add_generator(BMat({{0, 0, 0, 0, 1},
                          {0, 0, 0, 1, 0},
                          {0, 0, 1, 0, 0},
                          {0, 1, 0, 0, 0},
                          {1, 0, 0, 0, 1}}));
    K.add_generator(BMat({{0, 0, 0, 0, 1},
                          {0, 0, 0, 1, 0},
                          {0, 1, 1, 0, 0},
                          {1, 0, 1, 0, 0},
                          {1, 1, 0, 0, 0}}));
    K.add_generator(BMat({{0, 0, 0, 0, 1},
                          {0, 0, 1, 1, 0},
                          {0, 1, 0, 1, 0},
                          {1, 0, 0, 1, 0},
                          {1, 1, 1, 0, 0}}));
    K.add_generator(BMat({{0, 0, 0, 0, 1},
                          {0, 0, 1, 1, 0},
                          {0, 1, 0, 1, 0},
                          {1, 0, 1, 0, 0},
                          {1, 1, 0, 0, 0}}));
    K.add_generator(BMat({{0, 0, 0, 1, 1},
                          {0, 0, 1, 0, 1},
                          {0, 1, 0, 0, 1},
                          {1, 0, 0, 0, 1},
                          {1, 1, 1, 1, 0}}));
    K.add_generator(BMat({{0, 0, 0, 1, 1},
                          {0, 0, 1, 0, 1},
                          {0, 1, 0, 0, 1},
                          {1, 0, 0, 1, 0},
                          {1, 1, 1, 0, 0}}));
    K.add_generator(BMat({{0, 0, 0, 1, 1},
                          {0, 0, 1, 0, 1},
                          {0, 1, 0, 1, 0},
                          {1, 0, 1, 0, 0},
                          {1, 1, 0, 0, 1}}));
    K.add_generator(BMat({{0, 0, 0, 1, 1},
                          {0, 0, 1, 0, 1},
                          {0, 1, 0, 1, 0},
                          {1, 0, 1, 1, 0},
                          {1, 1, 0, 0, 1}}));
    K.add_generator(BMat({{0, 0, 0, 1, 1},
                          {0, 0, 1, 0, 1},
                          {0, 1, 1, 1, 0},
                          {1, 0, 1, 1, 0},
                          {1, 1, 0, 0, 1}}));
    K.add_generator(BMat({{0, 0, 0, 1, 1},
                          {0, 0, 1, 0, 1},
                          {0, 1, 0, 1, 0},
                          {1, 0, 1, 0, 0},
                          {1, 1, 0, 0, 0}}));
    REQUIRE(K.size() == 23191071);
#ifdef LIBSEMIGROUPS_HPCOMBI_ENABLED
#pragma GCC diagnostic pop
#endif
  }
}  // namespace libsemigroups
