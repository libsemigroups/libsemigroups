//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2020 James D. Mitchell
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

#include "libsemigroups/constants.hpp"     // for UNDEFINED
#include "libsemigroups/froidure-pin.hpp"  // for FroidurePin
#include "libsemigroups/matrix.hpp"        // for NTPSemiring, MaxPlusTruncS...
#include "libsemigroups/report.hpp"        // for ReportGuard

namespace libsemigroups {
  struct LibsemigroupsException;
  constexpr bool REPORT = false;
  namespace {
    template <typename Mat>
    void test000() {
      auto             rg = ReportGuard(REPORT);
      FroidurePin<Mat> S;
      S.add_generator(Mat::make({{0, -4}, {-4, -1}}));
      S.add_generator(Mat::make({{0, -3}, {-3, -1}}));

      REQUIRE(S.size() == 26);
      REQUIRE(S.degree() == 2);
      REQUIRE(S.number_of_idempotents() == 4);
      REQUIRE(S.number_of_generators() == 2);
      REQUIRE(S.number_of_rules() == 9);
      REQUIRE(S[0] == S.generator(0));
      REQUIRE(S[1] == S.generator(1));

      REQUIRE(S.position(S.generator(0)) == 0);
      REQUIRE(S.contains(S.generator(0)));

      REQUIRE(S.position(S.generator(1)) == 1);
      REQUIRE(S.contains(S.generator(1)));

      Mat x({{-2, 2}, {-1, 0}});
      REQUIRE(S.position(x) == UNDEFINED);
      REQUIRE(!S.contains(x));
      x.product_inplace(S.generator(1), S.generator(1));
      REQUIRE(S.position(x) == 5);
      REQUIRE(S.contains(x));

      // x = Mat({{-2, 2, 0}, {-1, 0, 0}, {0, 0, 0}});
      // REQUIRE(S.position(x) == UNDEFINED);
      // REQUIRE(!S.contains(x));
    }

    template <typename Mat>
    void test001(NTPSemiring<> const* sr = nullptr) {
      auto rg = ReportGuard();

      FroidurePin<Mat> S;
      S.add_generator(Mat(sr, {{0, 0, 1}, {0, 1, 0}, {1, 1, 0}}));
      S.add_generator(Mat(sr, {{0, 0, 1}, {0, 1, 0}, {2, 0, 0}}));
      S.add_generator(Mat(sr, {{0, 0, 1}, {0, 1, 1}, {1, 0, 0}}));
      S.add_generator(Mat(sr, {{0, 0, 1}, {0, 1, 0}, {3, 0, 0}}));
      S.reserve(10077696);
      REQUIRE(S.size() == 10077696);
      REQUIRE(S.number_of_idempotents() == 13688);
    }

    // Min-plus
    template <typename Mat>
    void test004() {
      auto             rg = ReportGuard(REPORT);
      FroidurePin<Mat> S;
      S.add_generator(Mat::make({{1, 0}, {0, POSITIVE_INFINITY}}));

      REQUIRE(S.size() == 3);
      REQUIRE(S.degree() == 2);
      REQUIRE(S.number_of_idempotents() == 1);
      REQUIRE(S.number_of_generators() == 1);
      REQUIRE(S.number_of_rules() == 1);

      REQUIRE(S[0] == S.generator(0));
      REQUIRE(S.position(S.generator(0)) == 0);
      REQUIRE(S.contains(S.generator(0)));

      auto x = Mat({{-2, 2}, {-1, 0}});
      REQUIRE(S.position(x) == UNDEFINED);
      REQUIRE(!S.contains(x));
      x.product_inplace(S.generator(0), S.generator(0));
      REQUIRE(S.position(x) == 1);
      REQUIRE(S.contains(x));
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

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<MaxPlusMat<2>>",
                          "040",
                          "Example 000",
                          "[quick][froidure-pin][matrix]") {
    test000<MaxPlusMat<2>>();
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<MaxPlusMat<>>",
                          "041",
                          "Example 000",
                          "[quick][froidure-pin][matrix]") {
    test000<MaxPlusMat<>>();
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<NTPMat<0, 6, 3>>",
                          "042",
                          "Example 001",
                          "[extreme][froidure-pin][matrix]") {
    test001<NTPMat<0, 6, 3>>();
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<NTPMat<0, 6>>",
                          "043",
                          "Example 001",
                          "[extreme][froidure-pin][matrix]") {
    test001<NTPMat<0, 6>>();
  }

  // TODO(later) Example 001 with a semiring

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<MinPlusMat<2>>",
                          "044",
                          "Example 004",
                          "[quick][froidure-pin][matrix]") {
    test004<MinPlusMat<2>>();
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<MinPlusMat<>>",
                          "045",
                          "Example 004",
                          "[quick][froidure-pin][matrix]") {
    test004<MinPlusMat<>>();
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<MaxPlusTruncMat<33, 3>>",
                          "046",
                          "Example 005",
                          "[quick][froidure-pin][matrix]") {
    test005<MaxPlusTruncMat<33, 3>>();
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<MaxPlusTruncMat<33>>",
                          "047",
                          "Example 005",
                          "[quick][froidure-pin][matrix]") {
    test005<MaxPlusTruncMat<33>>();
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<MaxPlusTruncMat<>>",
                          "048",
                          "Example 005",
                          "[quick][froidure-pin][matrix]") {
    MaxPlusTruncSemiring<> const* sr = new MaxPlusTruncSemiring<>(33);
    test005<MaxPlusTruncMat<>>(sr);
    delete sr;
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<MinPlusTruncMat<11, 3>>",
                          "049",
                          "Example 006",
                          "[quick][froidure-pin][matrix]") {
    test006<MinPlusTruncMat<11, 3>>();
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<MinPlusTruncMat<11>>",
                          "050",
                          "Example 006",
                          "[quick][froidure-pin][matrix]") {
    test006<MinPlusTruncMat<11>>();
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<MinPlusTruncMat<>>",
                          "051",
                          "Example 006",
                          "[quick][froidure-pin][matrix]") {
    MinPlusTruncSemiring<> const* sr = new MinPlusTruncSemiring<>(11);
    test006<MinPlusTruncMat<>>(sr);
    delete sr;
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<NTPMat<11, 3, 3>>",
                          "052",
                          "Example 007",
                          "[quick][froidure-pin][matrix]") {
    test007<NTPMat<11, 3, 3>>();
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<NTPMat<11, 3>>",
                          "053",
                          "Example 007",
                          "[quick][froidure-pin][matrix]") {
    test007<NTPMat<11, 3>>();
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<NTPMat<>>",
                          "054",
                          "Example 007",
                          "[quick][froidure-pin][matrix]") {
    NTPSemiring<> const* sr = new NTPSemiring<>(11, 3);
    test007<NTPMat<>>(sr);
    delete sr;
  }

}  // namespace libsemigroups
