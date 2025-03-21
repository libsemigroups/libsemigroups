// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2020-2025 Finn Smith
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

#include "Catch2-3.8.0/catch_amalgamated.hpp"  // for REQUIRE
#include "test-main.hpp"                       // FOR LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/bmat-fastest.hpp"  // for BMatFastest
#include "libsemigroups/bmat8.hpp"         // for BMat8
#include "libsemigroups/konieczny.hpp"     // for Konieczny

namespace libsemigroups {

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
                          "022",
                          "non-regular D-classes 02",
                          "[quick][bmat8][no-valgrind]") {
    auto              rg = ReportGuard(false);
    std::vector const gens
        = {BMat8({{0, 1, 0, 0}, {1, 0, 0, 1}, {1, 0, 0, 1}, {0, 1, 1, 0}}),
           BMat8({{0, 1, 0, 1}, {0, 1, 1, 1}, {0, 0, 1, 0}, {1, 1, 1, 1}}),
           BMat8({{1, 1, 0, 1}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 1, 0, 1}}),
           BMat8({{0, 0, 1, 0}, {0, 0, 1, 1}, {0, 0, 0, 0}, {1, 0, 0, 0}}),
           BMat8({{1, 1, 0, 1}, {1, 1, 1, 1}, {1, 0, 1, 0}, {0, 1, 1, 0}}),
           BMat8({{0, 1, 0, 0}, {0, 1, 1, 0}, {1, 0, 1, 0}, {0, 1, 0, 1}}),
           BMat8({{0, 1, 0, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}, {0, 0, 1, 0}})};

    std::vector const idems
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

    Konieczny KS = make<Konieczny>(gens);
    KS.run();

    REQUIRE(KS.size() == 10'160);
    REQUIRE(KS.number_of_D_classes() == 66);

    size_t count = 0;
    for (BMat8 id : idems) {
      count += KS.D_class_of_element(id).size();
    }
    REQUIRE(count == 8'712);

    REQUIRE(size_t(KS.cend_current_regular_D_classes()
                   - KS.cbegin_current_regular_D_classes())
            == idems.size());

    std::vector non_reg_reps
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
                          "023",
                          "Hall monoid 5",
                          "[extreme][bmat8]") {
    auto rg    = ReportGuard();
    using BMat = BMatFastest<5>;
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
    REQUIRE(K.size() == 23'191'071);
  }

  LIBSEMIGROUPS_TEST_CASE("Konieczny",
                          "024",
                          "copy constructors",
                          "[quick][bmat8]") {
    auto              rg = ReportGuard(false);
    std::vector const gens
        = {BMat8({{0, 1, 0, 0}, {1, 0, 0, 1}, {1, 0, 0, 1}, {0, 1, 1, 0}}),
           BMat8({{0, 1, 0, 1}, {0, 1, 1, 1}, {0, 0, 1, 0}, {1, 1, 1, 1}}),
           BMat8({{1, 1, 0, 1}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 1, 0, 1}}),
           BMat8({{0, 0, 1, 0}, {0, 0, 1, 1}, {0, 0, 0, 0}, {1, 0, 0, 0}}),
           BMat8({{1, 1, 0, 1}, {1, 1, 1, 1}, {1, 0, 1, 0}, {0, 1, 1, 0}}),
           BMat8({{0, 1, 0, 0}, {0, 1, 1, 0}, {1, 0, 1, 0}, {0, 1, 0, 1}}),
           BMat8({{0, 1, 0, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}, {0, 0, 1, 0}})};

    Konieczny KS = make<Konieczny>(gens);
    Konieczny KT = Konieczny(KS);
    KS.run();

    REQUIRE(KT.current_size() == 0);
    REQUIRE(KT.size() == 10'160);

    Konieczny KU = KT;

    REQUIRE(KU.size() == 10'160);
    REQUIRE(KU.number_of_D_classes() == 66);

    Konieczny KV = make<Konieczny>(gens);
    KV.run_until(
        [&KV]() -> bool { return KV.current_number_of_D_classes() > 20; });
    size_t found_classes = KV.current_number_of_D_classes();

    Konieczny<BMat8> KW;
    KW = KV;
    REQUIRE(KW.size() == 10'160);
    REQUIRE(KW.number_of_D_classes() == 66);
    REQUIRE(KV.current_number_of_D_classes() == found_classes);

    KV.run();
    REQUIRE(KV.size() == 10'160);
  }

  LIBSEMIGROUPS_TEST_CASE("Konieczny",
                          "025",
                          "move constructors",
                          "[quick][bmat8]") {
    auto              rg = ReportGuard(false);
    std::vector const gens
        = {BMat8({{0, 1, 0, 0}, {1, 0, 0, 1}, {1, 0, 0, 1}, {0, 1, 1, 0}}),
           BMat8({{0, 1, 0, 1}, {0, 1, 1, 1}, {0, 0, 1, 0}, {1, 1, 1, 1}}),
           BMat8({{1, 1, 0, 1}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 1, 0, 1}}),
           BMat8({{0, 0, 1, 0}, {0, 0, 1, 1}, {0, 0, 0, 0}, {1, 0, 0, 0}}),
           BMat8({{1, 1, 0, 1}, {1, 1, 1, 1}, {1, 0, 1, 0}, {0, 1, 1, 0}}),
           BMat8({{0, 1, 0, 0}, {0, 1, 1, 0}, {1, 0, 1, 0}, {0, 1, 0, 1}}),
           BMat8({{0, 1, 0, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}, {0, 0, 1, 0}})};

    Konieczny KS = make<Konieczny>(gens);
    Konieczny KT = std::move(KS);

    REQUIRE(KT.current_size() == 0);
    KT.run();
    REQUIRE(KT.current_size() == 10'160);

    Konieczny KU = std::move(KT);

    REQUIRE(KU.size() == 10'160);
    REQUIRE(KU.number_of_D_classes() == 66);

    Konieczny<BMat8> KW;
    size_t           found_classes = 0;
    {
      Konieczny KV = make<Konieczny>(gens);
      KV.run_until(
          [&KV]() -> bool { return KV.current_number_of_D_classes() > 20; });
      found_classes = KV.current_number_of_D_classes();

      KW = std::move(KV);
    }

    REQUIRE(KW.current_number_of_D_classes() == found_classes);
    KW.run();
    LIBSEMIGROUPS_ASSERT(KW.number_of_D_classes() == 66);
    LIBSEMIGROUPS_ASSERT(KW.size() == 10'160);
    KW.run();
    REQUIRE(KW.size() == 10'160);
  }
}  // namespace libsemigroups
