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

#include "Catch2-3.7.1/catch_amalgamated.hpp"  // for REQUIRE
#include "test-main.hpp"                       // FOR LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/bmat-fastest.hpp"     // for BMatFastest
#include "libsemigroups/bmat8.hpp"            // for BMat8
#include "libsemigroups/froidure-pin.hpp"     // for FroidurePin
#include "libsemigroups/konieczny.hpp"        // for Konieczny
#include "libsemigroups/to-froidure-pin.hpp"  // for to<FroidurePin>

namespace libsemigroups {

  LIBSEMIGROUPS_TEST_CASE("Konieczny",
                          "005",
                          "regular elements and idempotents",
                          "[quick][no-valgrind][bmat8]") {
    using BMat = BMatFastest<4>;
    auto rg    = ReportGuard(false);

    Konieczny KS = make<Konieczny>(
        {BMat({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
         BMat({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
         BMat({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {1, 0, 0, 1}}),
         BMat({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}})});
    auto S = to<FroidurePin>(KS);
    S.run();

    REQUIRE(KS.size() == 63'904);
    REQUIRE(S.size() == 63'904);
    REQUIRE(KS.number_of_generators() == 4);
    REQUIRE(KS.number_of_D_classes() == 50);
    REQUIRE(Gabow(S.right_cayley_graph()).number_of_components() == 1'256);
    REQUIRE(Gabow(S.left_cayley_graph()).number_of_components() == 1'256);
    REQUIRE(KS.number_of_L_classes() == 1'256);
    REQUIRE(KS.number_of_R_classes() == 1'256);
    REQUIRE(KS.number_of_regular_D_classes() == 25);
    REQUIRE(KS.number_of_regular_L_classes() == 618);
    REQUIRE(KS.number_of_regular_R_classes() == 618);
    REQUIRE(KS.number_of_idempotents() == 2'360);
    REQUIRE(S.number_of_idempotents() == 2'360);
    REQUIRE(KS.number_of_H_classes() == 48'092);

    size_t count = 0;
    for (auto it = S.cbegin(); it < S.cend(); ++it) {
      if (KS.is_regular_element(*it)) {
        count++;
      }
    }
    REQUIRE(count == 40'408);
    REQUIRE(KS.number_of_regular_elements() == 40'408);
  }

  LIBSEMIGROUPS_TEST_CASE("Konieczny",
                          "006",
                          "regular D-class 01",
                          "[quick][bmat8]") {
    auto      rg = ReportGuard(false);
    Konieczny KS = make<Konieczny>({BMat8({{0, 1, 0}, {0, 0, 1}, {1, 0, 0}}),
                                    BMat8({{0, 1, 0}, {1, 0, 0}, {0, 0, 1}}),
                                    BMat8({{1, 0, 0}, {1, 1, 0}, {0, 0, 1}}),
                                    BMat8({{1, 1, 0}, {0, 1, 1}, {1, 0, 1}})});
    REQUIRE(KS.size() == 247);

    BMat8 x({{1, 0, 0}, {1, 1, 0}, {1, 0, 1}});
    auto& D = KS.D_class_of_element(x);
    REQUIRE(D.number_of_L_classes() == 3);
    REQUIRE(D.number_of_R_classes() == 3);
    REQUIRE(D.size() == 18);
    REQUIRE(D.is_regular_D_class());
  }

  LIBSEMIGROUPS_TEST_CASE("Konieczny",
                          "007",
                          "regular D-class 02",
                          "[quick][bmat8][no-valgrind]") {
    auto rg = ReportGuard(false);

    Konieczny KS = make<Konieczny>(
        {BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
         BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
         BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
         BMat8({{0, 1, 0, 1}, {1, 0, 1, 0}, {1, 0, 1, 0}, {0, 0, 1, 1}}),
         BMat8({{0, 1, 0, 1}, {1, 0, 1, 0}, {1, 0, 1, 0}, {0, 1, 0, 1}}),
         BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}})});
    KS.run();
    auto& D = KS.D_class_of_element(
        BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}));
    REQUIRE(D.size() == 24);
    REQUIRE(D.is_regular_D_class());
  }

  LIBSEMIGROUPS_TEST_CASE("Konieczny",
                          "008",
                          "regular D-class 04: contains",
                          "[quick][no-valgrind][bmat8]") {
    auto rg = ReportGuard(false);

    Konieczny KS = make<Konieczny>(
        {BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
         BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
         BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
         BMat8({{0, 1, 0, 1}, {1, 0, 1, 0}, {1, 0, 1, 0}, {0, 0, 1, 1}}),
         BMat8({{0, 1, 0, 1}, {1, 0, 1, 0}, {1, 0, 1, 0}, {0, 1, 0, 1}}),
         BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}})});
    auto S = to<FroidurePin>(KS);
    KS.run();
    S.run();
    auto& D = KS.D_class_of_element(
        BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}));

    // test that the top D-class contains only permutation matrices
    for (auto it = S.cbegin(); it < S.cend(); it++) {
      REQUIRE(D.contains(*it)
              == (((*it) * bmat8::transpose(*it)) == S.generator(0)));
    }
  }

  LIBSEMIGROUPS_TEST_CASE("Konieczny",
                          "009",
                          "non-regular D-classes 01",
                          "[quick][bmat8]") {
    auto rg = ReportGuard(false);

    auto KS = make<Konieczny>({BMat8({{0, 1, 0}, {0, 0, 1}, {1, 0, 0}}),
                               BMat8({{0, 1, 0}, {1, 0, 0}, {0, 0, 1}}),
                               BMat8({{1, 0, 0}, {1, 1, 0}, {0, 0, 1}}),
                               BMat8({{1, 1, 0}, {0, 1, 1}, {1, 0, 1}})});
    KS.run();

    REQUIRE(KS.number_of_regular_D_classes() == 9);

    size_t count = 0;
    for (BMat8 id : {BMat8({{1, 0, 0}, {0, 1, 0}, {0, 0, 1}}),
                     BMat8({{1, 0, 0}, {1, 1, 0}, {0, 0, 1}}),
                     BMat8({{1, 0, 0}, {1, 1, 1}, {0, 0, 1}}),
                     BMat8({{1, 0, 0}, {1, 1, 0}, {1, 0, 1}}),
                     BMat8({{1, 0, 0}, {1, 1, 0}, {1, 1, 1}}),
                     BMat8({{1, 1, 0}, {1, 1, 0}, {0, 0, 1}}),
                     BMat8({{1, 0, 0}, {1, 1, 1}, {1, 1, 1}}),
                     BMat8({{1, 1, 0}, {1, 1, 0}, {1, 1, 1}}),
                     BMat8({{1, 1, 1}, {1, 1, 1}, {1, 1, 1}})}) {
      auto& D = KS.D_class_of_element(id);
      count += D.size();
      REQUIRE(D.is_regular_D_class());
    }

    REQUIRE(count == 142);

    std::vector non_reg_reps = {BMat8({{0, 0, 1}, {1, 0, 1}, {1, 1, 0}}),
                                BMat8({{0, 0, 1}, {1, 1, 1}, {1, 1, 0}}),
                                BMat8({{0, 1, 1}, {1, 0, 1}, {1, 1, 1}}),
                                BMat8({{0, 1, 1}, {1, 1, 0}, {1, 0, 1}}),
                                BMat8({{1, 0, 1}, {1, 0, 1}, {1, 1, 0}}),
                                BMat8({{1, 1, 0}, {1, 1, 1}, {1, 1, 1}})};

    {
      auto& X = KS.D_class_of_element(non_reg_reps[0]);
      REQUIRE(X.size() == 36);
      REQUIRE(X.size_H_class() == 1);
      REQUIRE(X.number_of_L_classes() == 6);
      REQUIRE(X.number_of_R_classes() == 6);
    }

    {
      auto& X = KS.D_class_of_element(non_reg_reps[1]);
      REQUIRE(X.size() == 18);
      REQUIRE(X.size_H_class() == 1);
      REQUIRE(X.number_of_L_classes() == 3);
      REQUIRE(X.number_of_R_classes() == 6);
    }

    {
      auto& X = KS.D_class_of_element(non_reg_reps[2]);
      REQUIRE(X.size() == 18);
      REQUIRE(X.size_H_class() == 2);
      REQUIRE(X.number_of_L_classes() == 3);
      REQUIRE(X.number_of_R_classes() == 3);
    }

    {
      auto& X = KS.D_class_of_element(non_reg_reps[3]);
      REQUIRE(X.size() == 6);
      REQUIRE(X.size_H_class() == 6);
      REQUIRE(X.number_of_L_classes() == 1);
      REQUIRE(X.number_of_R_classes() == 1);
    }

    {
      auto& X = KS.D_class_of_element(non_reg_reps[4]);
      REQUIRE(X.size() == 18);
      REQUIRE(X.size_H_class() == 1);
      REQUIRE(X.number_of_L_classes() == 6);
      REQUIRE(X.number_of_R_classes() == 3);
    }

    {
      auto& X = KS.D_class_of_element(non_reg_reps[5]);
      REQUIRE(X.size() == 9);
      REQUIRE(X.size_H_class() == 1);
      REQUIRE(X.number_of_L_classes() == 3);
      REQUIRE(X.number_of_R_classes() == 3);
    }

    for (BMat8 x : non_reg_reps) {
      auto& N = KS.D_class_of_element(x);
      count += N.size();
      REQUIRE(!N.is_regular_D_class());
    }

    REQUIRE(count == 247);

    REQUIRE(KS.size() == 247);
  }

  LIBSEMIGROUPS_TEST_CASE("Konieczny",
                          "010",
                          "RegularDClass",
                          "[quick][bmat8]") {
    auto rg = ReportGuard(false);

    Konieczny KS = make<Konieczny>(
        {BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
         BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
         BMat8({{0, 1, 0, 1}, {1, 0, 1, 0}, {1, 0, 1, 0}, {0, 0, 1, 1}}),
         BMat8({{0, 1, 0, 1}, {1, 0, 1, 0}, {1, 0, 1, 0}, {0, 1, 0, 1}}),
         BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}})});
    KS.run();
    auto& D = KS.D_class_of_element(BMat8({{0, 1, 0}, {1, 0, 0}, {0, 0, 0}}));
    REQUIRE(D.size() == 90);
    REQUIRE(D.number_of_L_classes() == 5);
    REQUIRE(D.number_of_R_classes() == 9);
    REQUIRE(D.is_regular_D_class());
  }

  LIBSEMIGROUPS_TEST_CASE("Konieczny",
                          "011",
                          "full bmat monoid 4",
                          "[quick][no-valgrind][bmat8]") {
    auto rg = ReportGuard(false);

    Konieczny S = make<Konieczny>(
        {BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
         BMat8({{1, 1, 1, 0}, {1, 0, 0, 1}, {0, 1, 0, 1}, {0, 0, 1, 1}}),
         BMat8({{1, 1, 0, 0}, {1, 0, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 1}}),
         BMat8({{1, 1, 0, 0}, {1, 0, 1, 0}, {0, 1, 0, 1}, {0, 0, 1, 1}}),
         BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {1, 0, 0, 1}}),
         BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}}),
         BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
         BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}})});
    REQUIRE(S.size() == 65'536);
  }
}  // namespace libsemigroups
