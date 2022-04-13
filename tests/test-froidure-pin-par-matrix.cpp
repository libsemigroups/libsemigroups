//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2022 Ewan Gilligan
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

#include "catch.hpp"      // REQUIRE
#include "test-main.hpp"  // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/constants.hpp"              // for UNDEFINED
#include "libsemigroups/froidure-pin-parallel.hpp"  // for FroidurePinParallel
#include "libsemigroups/froidure-pin.hpp"           // for FroidurePin
#include "libsemigroups/matrix.hpp"  // for NTPSemiring, MaxPlusTruncS...
#include "libsemigroups/report.hpp"  // for ReportGuard

namespace libsemigroups {
  struct LibsemigroupsException;
  constexpr bool REPORT = false;
  namespace {
    template <typename T>
    void test_same_result(std::vector<T> gens) {
      auto rg = ReportGuard(REPORT);

      FroidurePin<T>         S;
      FroidurePinParallel<T> S_par(4);
      S.add_generators(gens.begin(), gens.end());
      S_par.add_generators(gens.begin(), gens.end());

      REQUIRE(S_par.size() == S.size());

      // Check all elements are the same
      auto it     = S.cbegin_sorted();
      auto it_par = S_par.cbegin_sorted();
      while (it < S.cend_sorted() && it_par < S_par.cend_sorted()) {
        REQUIRE(EqualTo<T>()(*it, *it_par));
        it++;
        it_par++;
      }
    }

    template <typename Mat>
    void test000() {
      auto             rg = ReportGuard(REPORT);
      std::vector<Mat> gens
          = {Mat::make({{0, -4}, {-4, -1}}), Mat::make({{0, -3}, {-3, -1}})};
      test_same_result<Mat>(gens);
    }

    template <typename Mat>
    void test001(NTPSemiring<> const* sr = nullptr) {
      auto rg = ReportGuard();

      std::vector<Mat> gens = {Mat(sr, {{0, 0, 1}, {0, 1, 0}, {1, 1, 0}}),
                               Mat(sr, {{0, 0, 1}, {0, 1, 0}, {2, 0, 0}}),
                               Mat(sr, {{0, 0, 1}, {0, 1, 1}, {1, 0, 0}}),
                               Mat(sr, {{0, 0, 1}, {0, 1, 0}, {3, 0, 0}})};
      test_same_result<Mat>(gens);
    }

    // Min-plus
    template <typename Mat>
    void test004() {
      auto rg = ReportGuard(REPORT);

      std::vector<Mat> gens = {Mat::make({{1, 0}, {0, POSITIVE_INFINITY}})};
      test_same_result<Mat>(gens);
    }

    // MaxPlusTruncMat(33)
    template <typename Mat>
    void test005(MaxPlusTruncSemiring<> const* sr = nullptr) {
      auto             rg = ReportGuard(REPORT);
      FroidurePin<Mat> S;
      S.add_generator(Mat::make(sr, {{22, 21, 0}, {10, 0, 0}, {1, 32, 1}}));
      S.add_generator(Mat::make(sr, {{0, 0, 0}, {0, 1, 0}, {1, 1, 0}}));

      REQUIRE(S.size() == 119);
      REQUIRE(S.degree() == 3);
      REQUIRE(S.number_of_idempotents() == 1);
      REQUIRE(S.number_of_generators() == 2);
      REQUIRE(S.number_of_rules() == 18);

      REQUIRE(S[0] == S.generator(0));
      REQUIRE(S.position(S.generator(0)) == 0);
      REQUIRE(S.contains(S.generator(0)));

      // auto x = Mat::make(sr, {{2, 2}, {1, 0}});
      // REQUIRE(S.position(x) == UNDEFINED);
      // REQUIRE(!S.contains(x));
    }

    // MinPlusTruncMat(11)
    template <typename Mat>
    void test006(MinPlusTruncSemiring<> const* sr = nullptr) {
      auto             rg = ReportGuard(REPORT);
      FroidurePin<Mat> S;
      S.add_generator(Mat::make(sr, {{2, 1, 0}, {10, 0, 0}, {1, 2, 1}}));
      S.add_generator(Mat::make(sr, {{10, 0, 0}, {0, 1, 0}, {1, 1, 0}}));

      REQUIRE(S.size() == 1039);
      REQUIRE(S.degree() == 3);
      REQUIRE(S.number_of_idempotents() == 5);
      REQUIRE(S.number_of_generators() == 2);
      REQUIRE(S.number_of_rules() == 38);

      REQUIRE(S[0] == S.generator(0));
      REQUIRE(S.position(S.generator(0)) == 0);
      REQUIRE(S.contains(S.generator(0)));

      auto x = Mat::make(sr, {{2, 2, 0}, {1, 0, 0}, {0, 0, 0}});
      REQUIRE(S.position(x) == UNDEFINED);
      REQUIRE(!S.contains(x));
      x.product_inplace(S.generator(0), S.generator(0));
      REQUIRE(S.position(x) == 2);
      REQUIRE(S.contains(x));
    }

    // NTPSemiring(11, 3);
    template <typename Mat>
    void test007(NTPSemiring<> const* sr = nullptr) {
      auto             rg = ReportGuard(REPORT);
      FroidurePin<Mat> S;
      S.add_generator(Mat::make(sr, {{2, 1, 0}, {10, 0, 0}, {1, 2, 1}}));
      S.add_generator(Mat::make(sr, {{10, 0, 0}, {0, 1, 0}, {1, 1, 0}}));

      REQUIRE(S.size() == 86);
      REQUIRE(S.degree() == 3);
      REQUIRE(S.number_of_idempotents() == 10);
      REQUIRE(S.number_of_generators() == 2);
      REQUIRE(S.number_of_rules() == 16);

      REQUIRE(S[0] == S.generator(0));
      REQUIRE(S.position(S.generator(0)) == 0);
      REQUIRE(S.contains(S.generator(0)));

      auto x = Mat::make(sr, {{2, 2, 0}, {1, 0, 0}, {0, 0, 0}});
      REQUIRE(S.position(x) == UNDEFINED);
      REQUIRE(!S.contains(x));
      x.product_inplace(S.generator(1), S.generator(0));
      REQUIRE(S.position(x) == 4);
      REQUIRE(S.contains(x));
    }
  }  // namespace

  LIBSEMIGROUPS_TEST_CASE("FroidurePinParallel<MaxPlusMat<2>>",
                          "001",
                          "Example 000",
                          "[quick][froidure-pin-parallel][matrix]") {
    test000<MaxPlusMat<2>>();
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePinParallel<MaxPlusMat<>>",
                          "002",
                          "Example 000",
                          "[quick][froidure-pin-parallel][matrix]") {
    test000<MaxPlusMat<>>();
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePinParallel<NTPMat<0, 6, 3>>",
                          "003",
                          "Example 001",
                          "[extreme][froidure-pin-parallel][matrix]") {
    test001<NTPMat<0, 6, 3>>();
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePinParallel<NTPMat<0, 6>>",
                          "004",
                          "Example 001",
                          "[extreme][froidure-pin-parallel][matrix]") {
    test001<NTPMat<0, 6>>();
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePinParallel<MinPlusMat<2>>",
                          "005",
                          "Example 004",
                          "[quick][froidure-pin-parallel][matrix]") {
    test004<MinPlusMat<2>>();
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePinParallel<MinPlusMat<>>",
                          "006",
                          "Example 004",
                          "[quick][froidure-pin-parallel][matrix]") {
    test004<MinPlusMat<>>();
  }

}  // namespace libsemigroups
