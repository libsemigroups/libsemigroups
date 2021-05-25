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
#include "libsemigroups/froidure-pin.hpp"  // for FroidurePin
#include "libsemigroups/konieczny.hpp"     // for Konieczny

namespace libsemigroups {

  constexpr bool REPORT = false;

  LIBSEMIGROUPS_TEST_CASE("Konieczny",
                          "009",
                          "regular elements and idempotents",
                          "[quick][no-valgrind][bmat8]") {
#ifdef LIBSEMIGROUPS_HPCOMBI_ENABLED
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#pragma GCC diagnostic ignored "-Winline"
#endif
    using BMat = FastestBMat<4>;
    auto rg    = ReportGuard(REPORT);

    const std::vector<BMat> gens
        = {BMat({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
           BMat({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
           BMat({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {1, 0, 0, 1}}),
           BMat({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}})};
    Konieczny<BMat>   KS(gens);
    FroidurePin<BMat> S(gens);
    S.run();

    REQUIRE(KS.size() == 63904);
    REQUIRE(S.size() == 63904);
    REQUIRE(KS.number_of_generators() == 4);
    REQUIRE(KS.number_of_D_classes() == 50);
    REQUIRE(KS.number_of_L_classes() == 1256);
    REQUIRE(KS.number_of_R_classes() == 1256);
    REQUIRE(KS.number_of_regular_D_classes() == 25);
    REQUIRE(KS.number_of_regular_L_classes() == 618);
    REQUIRE(KS.number_of_regular_R_classes() == 618);
    REQUIRE(KS.number_of_idempotents() == 2360);
    REQUIRE(KS.number_of_H_classes() == 48092);
    REQUIRE(std::vector<BMat>(KS.cbegin_generators(), KS.cend_generators())
            == gens);

    size_t count = 0;
    for (auto it = S.cbegin(); it < S.cend(); ++it) {
      if (KS.is_regular_element(*it)) {
        count++;
      }
    }
    REQUIRE(count == 40408);
    REQUIRE(KS.number_of_regular_elements() == 40408);
#ifdef LIBSEMIGROUPS_HPCOMBI_ENABLED
#pragma GCC diagnostic pop
#endif
  }

  LIBSEMIGROUPS_TEST_CASE("Konieczny",
                          "010",
                          "regular D-class 01",
                          "[quick][bmat8]") {
    auto                     rg   = ReportGuard(REPORT);
    const std::vector<BMat8> gens = {BMat8({{0, 1, 0}, {0, 0, 1}, {1, 0, 0}}),
                                     BMat8({{0, 1, 0}, {1, 0, 0}, {0, 0, 1}}),
                                     BMat8({{1, 0, 0}, {1, 1, 0}, {0, 0, 1}}),
                                     BMat8({{1, 1, 0}, {0, 1, 1}, {1, 0, 1}})};
    Konieczny<BMat8>         KS(gens);
    REQUIRE(KS.size() == 247);

    BMat8                     x({{1, 0, 0}, {1, 1, 0}, {1, 0, 1}});
    Konieczny<BMat8>::DClass& D = KS.D_class_of_element(x);
    REQUIRE(D.number_of_L_classes() == 3);
    REQUIRE(D.number_of_R_classes() == 3);
    REQUIRE(D.size() == 18);
    REQUIRE(D.is_regular_D_class());
  }

  LIBSEMIGROUPS_TEST_CASE("Konieczny",
                          "011",
                          "regular D-class 02",
                          "[quick][bmat8]") {
    auto                     rg = ReportGuard(REPORT);
    const std::vector<BMat8> gens
        = {BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
           BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
           BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
           BMat8({{0, 1, 0, 1}, {1, 0, 1, 0}, {1, 0, 1, 0}, {0, 0, 1, 1}}),
           BMat8({{0, 1, 0, 1}, {1, 0, 1, 0}, {1, 0, 1, 0}, {0, 1, 0, 1}}),
           BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}})};

    Konieczny<BMat8> KS(gens);
    KS.run();
    BMat8 idem(BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}));

    Konieczny<BMat8>::DClass& D = KS.D_class_of_element(idem);
    REQUIRE(D.size() == 24);
    REQUIRE(D.is_regular_D_class());
  }

  LIBSEMIGROUPS_TEST_CASE("Konieczny",
                          "012",
                          "regular D-class 04: contains",
                          "[quick][no-valgrind][bmat8]") {
    auto                     rg = ReportGuard(REPORT);
    const std::vector<BMat8> gens
        = {BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
           BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
           BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
           BMat8({{0, 1, 0, 1}, {1, 0, 1, 0}, {1, 0, 1, 0}, {0, 0, 1, 1}}),
           BMat8({{0, 1, 0, 1}, {1, 0, 1, 0}, {1, 0, 1, 0}, {0, 1, 0, 1}}),
           BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}})};

    Konieczny<BMat8>   KS(gens);
    FroidurePin<BMat8> S(gens);
    KS.run();
    S.run();
    BMat8 idem(BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}));
    Konieczny<BMat8>::DClass& D = KS.D_class_of_element(idem);

    // test that the top D-class contains only permutation matrices
    for (auto it = S.cbegin(); it < S.cend(); it++) {
      REQUIRE(D.contains(*it) == (((*it) * (*it).transpose()) == gens[0]));
    }
  }

  LIBSEMIGROUPS_TEST_CASE("Konieczny",
                          "013",
                          "non-regular D-classes 01",
                          "[quick][bmat8]") {
    auto                     rg    = ReportGuard(REPORT);
    const std::vector<BMat8> gens  = {BMat8({{0, 1, 0}, {0, 0, 1}, {1, 0, 0}}),
                                     BMat8({{0, 1, 0}, {1, 0, 0}, {0, 0, 1}}),
                                     BMat8({{1, 0, 0}, {1, 1, 0}, {0, 0, 1}}),
                                     BMat8({{1, 1, 0}, {0, 1, 1}, {1, 0, 1}})};
    const std::vector<BMat8> idems = {BMat8({{1, 0, 0}, {0, 1, 0}, {0, 0, 1}}),
                                      BMat8({{1, 0, 0}, {1, 1, 0}, {0, 0, 1}}),
                                      BMat8({{1, 0, 0}, {1, 1, 1}, {0, 0, 1}}),
                                      BMat8({{1, 0, 0}, {1, 1, 0}, {1, 0, 1}}),
                                      BMat8({{1, 0, 0}, {1, 1, 0}, {1, 1, 1}}),
                                      BMat8({{1, 1, 0}, {1, 1, 0}, {0, 0, 1}}),
                                      BMat8({{1, 0, 0}, {1, 1, 1}, {1, 1, 1}}),
                                      BMat8({{1, 1, 0}, {1, 1, 0}, {1, 1, 1}}),
                                      BMat8({{1, 1, 1}, {1, 1, 1}, {1, 1, 1}})};

    Konieczny<BMat8> KS(gens);
    KS.run();

    REQUIRE(size_t(KS.cend_regular_D_classes() - KS.cbegin_regular_D_classes())
            == idems.size());

    size_t count = 0;
    for (BMat8 id : idems) {
      Konieczny<BMat8>::DClass& D = KS.D_class_of_element(id);
      count += D.size();
      REQUIRE(D.is_regular_D_class());
    }

    REQUIRE(count == 142);

    std::vector<BMat8> non_reg_reps
        = {BMat8({{0, 0, 1}, {1, 0, 1}, {1, 1, 0}}),
           BMat8({{0, 0, 1}, {1, 1, 1}, {1, 1, 0}}),
           BMat8({{0, 1, 1}, {1, 0, 1}, {1, 1, 1}}),
           BMat8({{0, 1, 1}, {1, 1, 0}, {1, 0, 1}}),
           BMat8({{1, 0, 1}, {1, 0, 1}, {1, 1, 0}}),
           BMat8({{1, 1, 0}, {1, 1, 1}, {1, 1, 1}})};

    {
      Konieczny<BMat8>::DClass& X = KS.D_class_of_element(non_reg_reps[0]);
      REQUIRE(X.size() == 36);
      REQUIRE(X.size_H_class() == 1);
      REQUIRE(X.number_of_L_classes() == 6);
      REQUIRE(X.number_of_R_classes() == 6);
    }

    {
      Konieczny<BMat8>::DClass& X = KS.D_class_of_element(non_reg_reps[1]);
      REQUIRE(X.size() == 18);
      REQUIRE(X.size_H_class() == 1);
      REQUIRE(X.number_of_L_classes() == 3);
      REQUIRE(X.number_of_R_classes() == 6);
    }

    {
      Konieczny<BMat8>::DClass& X = KS.D_class_of_element(non_reg_reps[2]);
      REQUIRE(X.size() == 18);
      REQUIRE(X.size_H_class() == 2);
      REQUIRE(X.number_of_L_classes() == 3);
      REQUIRE(X.number_of_R_classes() == 3);
    }

    {
      Konieczny<BMat8>::DClass& X = KS.D_class_of_element(non_reg_reps[3]);
      REQUIRE(X.size() == 6);
      REQUIRE(X.size_H_class() == 6);
      REQUIRE(X.number_of_L_classes() == 1);
      REQUIRE(X.number_of_R_classes() == 1);
    }

    {
      Konieczny<BMat8>::DClass& X = KS.D_class_of_element(non_reg_reps[4]);
      REQUIRE(X.size() == 18);
      REQUIRE(X.size_H_class() == 1);
      REQUIRE(X.number_of_L_classes() == 6);
      REQUIRE(X.number_of_R_classes() == 3);
    }

    {
      Konieczny<BMat8>::DClass& X = KS.D_class_of_element(non_reg_reps[5]);
      REQUIRE(X.size() == 9);
      REQUIRE(X.size_H_class() == 1);
      REQUIRE(X.number_of_L_classes() == 3);
      REQUIRE(X.number_of_R_classes() == 3);
    }

    for (BMat8 x : non_reg_reps) {
      Konieczny<BMat8>::DClass& N = KS.D_class_of_element(x);
      count += N.size();
      REQUIRE(!N.is_regular_D_class());
    }

    REQUIRE(count == 247);

    REQUIRE(KS.size() == 247);
  }

  LIBSEMIGROUPS_TEST_CASE("Konieczny",
                          "014",
                          "RegularDClass",
                          "[quick][bmat8]") {
    auto                     rg = ReportGuard(REPORT);
    const std::vector<BMat8> gens
        = {BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
           BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
           BMat8({{0, 1, 0, 1}, {1, 0, 1, 0}, {1, 0, 1, 0}, {0, 0, 1, 1}}),
           BMat8({{0, 1, 0, 1}, {1, 0, 1, 0}, {1, 0, 1, 0}, {0, 1, 0, 1}}),
           BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}})};

    Konieczny<BMat8> KS(gens);
    KS.run();
    BMat8                     x = BMat8({{0, 1, 0}, {1, 0, 0}, {0, 0, 0}});
    Konieczny<BMat8>::DClass& D = KS.D_class_of_element(x);
    REQUIRE(D.size() == 90);
    REQUIRE(D.number_of_L_classes() == 5);
    REQUIRE(D.number_of_R_classes() == 9);
    REQUIRE(D.is_regular_D_class());
  }

  LIBSEMIGROUPS_TEST_CASE("Konieczny",
                          "015",
                          "full bmat monoid 4",
                          "[quick][no-valgrind][bmat8]") {
    auto                     rg = ReportGuard(REPORT);
    const std::vector<BMat8> bmat4_gens
        = {BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
           BMat8({{1, 1, 1, 0}, {1, 0, 0, 1}, {0, 1, 0, 1}, {0, 0, 1, 1}}),
           BMat8({{1, 1, 0, 0}, {1, 0, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 1}}),
           BMat8({{1, 1, 0, 0}, {1, 0, 1, 0}, {0, 1, 0, 1}, {0, 0, 1, 1}}),
           BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {1, 0, 0, 1}}),
           BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}}),
           BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
           BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}})};

    Konieczny<BMat8> S(bmat4_gens);
    REQUIRE(S.size() == 65536);
  }
}  // namespace libsemigroups
