// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2021-5 Finn Smith
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
#include <cstdint>  // for int64_t
#include <vector>   // for vector

#include "Catch2-3.14.0/catch_amalgamated.hpp"  // for REQUIRE
#include "test-main.hpp"                        // FOR LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/adapters.hpp"  // for detail::BruidhinnTraits
#include "libsemigroups/froidure-pin.hpp"  // for FroidurePin, FroidurePin<>::eleme...
#include "libsemigroups/konieczny.hpp"           // for Konieczny
#include "libsemigroups/matrix.hpp"              // for TropicalMaxPlusMat
#include "libsemigroups/max-plus-trunc-mat.hpp"  // for Lambda, Rho, etc.

namespace libsemigroups {
  // Forward declaration
  struct LibsemigroupsException;

  template <typename Func,
            typename ResType,
            typename DomContainer,
            typename RangeContainer>
  void test_vals(DomContainer const& dom, RangeContainer& ran) {
    LIBSEMIGROUPS_ASSERT(dom.size() == ran.size());
    for (size_t i = 0; i < dom.size(); ++i) {
      ResType res;
      Func()(res, dom[i]);
      REQUIRE(res == ran[i]);
    }
  }

  LIBSEMIGROUPS_TEST_CASE(
      "Konieczny",
      "026",
      "Rank of small tropical max-plus mats (dim 2, threshold 9)",
      "[quick][konieczny][tropmaxplus]") {
    auto rg    = ReportGuard(false);
    using Mat  = MaxPlusTruncMat<9, 2>;
    using Rank = Rank<Mat>;

    REQUIRE(Rank()(Mat({{1, 3}, {2, 1}})) == 30);
    REQUIRE(Rank()(Mat({{2, 1}, {4, 0}})) == 28);

    FroidurePin S
        = make<FroidurePin>({Mat({{1, 3}, {2, 1}}), Mat({{2, 1}, {4, 0}})});
    S.run();
    for (auto it = S.cbegin(); it < S.cend(); ++it) {
      size_t rnk = Rank()(*it);
      for (auto it2 = it; it2 < S.cend(); ++it2) {
        size_t rnk_prod = Rank()((*it) * (*it2));
        REQUIRE(rnk_prod <= rnk);
        REQUIRE(rnk_prod <= Rank()(*it2));
      }
    }
  }

  LIBSEMIGROUPS_TEST_CASE(
      "Konieczny",
      "027",
      "Lambda of tropical max-plus mats (dim 5, threshold 6)",
      "[quick][konieczny][tropmaxplus]") {
    auto rg = ReportGuard(false);

    using Mat       = MaxPlusTruncMat<6, 5>;
    using LambdaVal = LambdaValue<Mat>::type;
    using Lambda    = Lambda<Mat, LambdaVal>;
    using Row       = Mat::Row;
    std::vector<Mat> mats
        = {Mat({{4, NEGATIVE_INFINITY, NEGATIVE_INFINITY, 4, NEGATIVE_INFINITY},
                {1, 0, 1, 2, 2},
                {1, 4, 1, 2, NEGATIVE_INFINITY},
                {NEGATIVE_INFINITY, 1, 1, NEGATIVE_INFINITY, 0},
                {NEGATIVE_INFINITY, 0, 1, 0, NEGATIVE_INFINITY}}),
           Mat({{2, 1, 0, NEGATIVE_INFINITY, 1},
                {3, 4, 0, NEGATIVE_INFINITY, 2},
                {1, 2, NEGATIVE_INFINITY, 0, NEGATIVE_INFINITY},
                {NEGATIVE_INFINITY, NEGATIVE_INFINITY, 2, 2, NEGATIVE_INFINITY},
                {3, 1, 4, 1, 0}}),
           Mat({{3, NEGATIVE_INFINITY, 3, 1, 3},
                {NEGATIVE_INFINITY, 2, 1, 0, 0},
                {3, 2, 1, 0, NEGATIVE_INFINITY},
                {5, 3, 3, 2, 1},
                {NEGATIVE_INFINITY, NEGATIVE_INFINITY, 5, 1, 4}}),
           Mat({{1, 1, 0, 2, 0},
                {1, 0, 2, 1, NEGATIVE_INFINITY},
                {3, 4, 1, 2, NEGATIVE_INFINITY},
                {0, 0, 1, 0, NEGATIVE_INFINITY},
                {2, 0, NEGATIVE_INFINITY, 1, 2}}),
           Mat({{NEGATIVE_INFINITY, 2, 0, 1, 2},
                {3, 2, 0, 1, 4},
                {4, 3, 1, 2, 5},
                {3, 2, NEGATIVE_INFINITY, 0, 4},
                {NEGATIVE_INFINITY, NEGATIVE_INFINITY, 5, 1, 4}}),
           Mat({{NEGATIVE_INFINITY, 2, 0, 1, 2},
                {2, 3, 2, 3, 6},
                {1, 2, 0, 1, 4},
                {1, NEGATIVE_INFINITY, NEGATIVE_INFINITY, 0, 2},
                {0, NEGATIVE_INFINITY, 0, 1, 4}})};

    std::vector<LambdaVal> vals = {
        {Row({NEGATIVE_INFINITY, 0, 1, 0, NEGATIVE_INFINITY}),
         Row({NEGATIVE_INFINITY, 1, 1, NEGATIVE_INFINITY, 0}),
         Row({1, 0, 1, 2, 2}),
         Row({1, 4, 1, 2, NEGATIVE_INFINITY}),
         Row({4, NEGATIVE_INFINITY, NEGATIVE_INFINITY, 4, NEGATIVE_INFINITY})},
        {Row({NEGATIVE_INFINITY, NEGATIVE_INFINITY, 2, 2, NEGATIVE_INFINITY}),
         Row({1, 2, NEGATIVE_INFINITY, 0, NEGATIVE_INFINITY}),
         Row({2, 1, 0, NEGATIVE_INFINITY, 1}),
         Row({3, 1, 4, 1, 0}),
         Row({3, 4, 0, NEGATIVE_INFINITY, 2})},
        {Row({NEGATIVE_INFINITY, NEGATIVE_INFINITY, 5, 1, 4}),
         Row({NEGATIVE_INFINITY, 2, 1, 0, 0}),
         Row({3, NEGATIVE_INFINITY, 3, 1, 3}),
         Row({3, 2, 1, 0, NEGATIVE_INFINITY}),
         Row({5, 3, 3, 2, 1})},
        {Row({0, 0, 1, 0, NEGATIVE_INFINITY}),
         Row({1, 0, 2, 1, NEGATIVE_INFINITY}),
         Row({1, 1, 0, 2, 0}),
         Row({2, 0, NEGATIVE_INFINITY, 1, 2}),
         Row({3, 4, 1, 2, NEGATIVE_INFINITY})},
        {Row({NEGATIVE_INFINITY, NEGATIVE_INFINITY, 5, 1, 4}),
         Row({NEGATIVE_INFINITY, 2, 0, 1, 2}),
         Row({3, 2, NEGATIVE_INFINITY, 0, 4})},
        {Row({NEGATIVE_INFINITY, 2, 0, 1, 2}),
         Row({0, NEGATIVE_INFINITY, 0, 1, 4}),
         Row({1, NEGATIVE_INFINITY, NEGATIVE_INFINITY, 0, 2})}};

    test_vals<Lambda, LambdaVal>(mats, vals);

    FroidurePin S = make<FroidurePin>(mats);
    S.run();
    for (auto it = S.cbegin(); it < S.cend(); ++it) {
      LambdaVal x;
      Lambda()(x, *it);
      size_t basis_size = x.size();
      for (auto it2 = S.cbegin(); it2 < S.cend(); ++it2) {
        x.clear();
        Lambda()(x, (*it) * (*it2));
        REQUIRE(x.size() <= basis_size);
      }
    }
  }

  LIBSEMIGROUPS_TEST_CASE("Konieczny",
                          "028",
                          "Rho of tropical max-plus mats (dim 5, threshold 6)",
                          "[quick][konieczny][tropmaxplus]") {
    auto rg      = ReportGuard(false);
    using Mat    = MaxPlusTruncMat<6, 5>;
    using RhoVal = RhoValue<Mat>::type;
    using Rho    = Rho<Mat, RhoVal>;
    using Row    = Mat::Row;
    std::vector<Mat> mats
        = {Mat({{4, NEGATIVE_INFINITY, NEGATIVE_INFINITY, 4, NEGATIVE_INFINITY},
                {1, 0, 1, 2, 2},
                {1, 4, 1, 2, NEGATIVE_INFINITY},
                {NEGATIVE_INFINITY, 1, 1, NEGATIVE_INFINITY, 0},
                {NEGATIVE_INFINITY, 0, 1, 0, NEGATIVE_INFINITY}}),
           Mat({{2, 1, 0, NEGATIVE_INFINITY, 1},
                {3, 4, 0, NEGATIVE_INFINITY, 2},
                {1, 2, NEGATIVE_INFINITY, 0, NEGATIVE_INFINITY},
                {NEGATIVE_INFINITY, NEGATIVE_INFINITY, 2, 2, NEGATIVE_INFINITY},
                {3, 1, 4, 1, 0}}),
           Mat({{3, NEGATIVE_INFINITY, 3, 1, 3},
                {NEGATIVE_INFINITY, 2, 1, 0, 0},
                {3, 2, 1, 0, NEGATIVE_INFINITY},
                {5, 3, 3, 2, 1},
                {NEGATIVE_INFINITY, NEGATIVE_INFINITY, 5, 1, 4}}),
           Mat({{1, 1, 0, 2, 0},
                {1, 0, 2, 1, NEGATIVE_INFINITY},
                {3, 4, 1, 2, NEGATIVE_INFINITY},
                {0, 0, 1, 0, NEGATIVE_INFINITY},
                {2, 0, NEGATIVE_INFINITY, 1, 2}}),
           Mat({{NEGATIVE_INFINITY, 2, 0, 1, 2},
                {3, 2, 0, 1, 4},
                {4, 3, 1, 2, 5},
                {3, 2, NEGATIVE_INFINITY, 0, 4},
                {NEGATIVE_INFINITY, NEGATIVE_INFINITY, 5, 1, 4}}),
           Mat({{NEGATIVE_INFINITY, 2, 0, 1, 2},
                {2, 3, 2, 3, 6},
                {1, 2, 0, 1, 4},
                {1, NEGATIVE_INFINITY, NEGATIVE_INFINITY, 0, 2},
                {0, NEGATIVE_INFINITY, 0, 1, 4}})};

    for (auto& x : mats) {
      x.transpose();
    }

    std::vector<RhoVal> vals = {
        {Row({NEGATIVE_INFINITY, 0, 1, 0, NEGATIVE_INFINITY}),
         Row({NEGATIVE_INFINITY, 1, 1, NEGATIVE_INFINITY, 0}),
         Row({1, 0, 1, 2, 2}),
         Row({1, 4, 1, 2, NEGATIVE_INFINITY}),
         Row({4, NEGATIVE_INFINITY, NEGATIVE_INFINITY, 4, NEGATIVE_INFINITY})},
        {Row({NEGATIVE_INFINITY, NEGATIVE_INFINITY, 2, 2, NEGATIVE_INFINITY}),
         Row({1, 2, NEGATIVE_INFINITY, 0, NEGATIVE_INFINITY}),
         Row({2, 1, 0, NEGATIVE_INFINITY, 1}),
         Row({3, 1, 4, 1, 0}),
         Row({3, 4, 0, NEGATIVE_INFINITY, 2})},
        {Row({NEGATIVE_INFINITY, NEGATIVE_INFINITY, 5, 1, 4}),
         Row({NEGATIVE_INFINITY, 2, 1, 0, 0}),
         Row({3, NEGATIVE_INFINITY, 3, 1, 3}),
         Row({3, 2, 1, 0, NEGATIVE_INFINITY}),
         Row({5, 3, 3, 2, 1})},
        {Row({0, 0, 1, 0, NEGATIVE_INFINITY}),
         Row({1, 0, 2, 1, NEGATIVE_INFINITY}),
         Row({1, 1, 0, 2, 0}),
         Row({2, 0, NEGATIVE_INFINITY, 1, 2}),
         Row({3, 4, 1, 2, NEGATIVE_INFINITY})},
        {Row({NEGATIVE_INFINITY, NEGATIVE_INFINITY, 5, 1, 4}),
         Row({NEGATIVE_INFINITY, 2, 0, 1, 2}),
         Row({3, 2, NEGATIVE_INFINITY, 0, 4})},
        {Row({NEGATIVE_INFINITY, 2, 0, 1, 2}),
         Row({0, NEGATIVE_INFINITY, 0, 1, 4}),
         Row({1, NEGATIVE_INFINITY, NEGATIVE_INFINITY, 0, 2})}};

    test_vals<Rho, RhoVal>(mats, vals);

    FroidurePin S = make<FroidurePin>(mats);
    S.run();
    for (auto it = S.cbegin(); it < S.cend(); ++it) {
      RhoVal x;
      Rho()(x, *it);
      size_t basis_size = x.size();
      for (auto it2 = S.cbegin(); it2 < S.cend(); ++it2) {
        x.clear();
        Rho()(x, (*it2) * (*it));
        REQUIRE(x.size() <= basis_size);
      }
    }
  }

  LIBSEMIGROUPS_TEST_CASE(
      "Konieczny",
      "029",
      "Lambda/ImageRightAction, tropical max-plus mats (d=5, t=6)",
      "[quick][konieczny][tropmaxplus]") {
    auto rg         = ReportGuard(false);
    using Mat       = MaxPlusTruncMat<6, 5>;
    using LambdaVal = LambdaValue<Mat>::type;
    using Lambda    = Lambda<Mat, LambdaVal>;
    using Action    = ImageRightAction<Mat, LambdaVal>;
    std::vector<Mat> mats
        = {Mat({{4, NEGATIVE_INFINITY, NEGATIVE_INFINITY, 4, NEGATIVE_INFINITY},
                {1, 0, 1, 2, 2},
                {1, 4, 1, 2, NEGATIVE_INFINITY},
                {NEGATIVE_INFINITY, 1, 1, NEGATIVE_INFINITY, 0},
                {NEGATIVE_INFINITY, 0, 1, 0, NEGATIVE_INFINITY}}),
           Mat({{2, 1, 0, NEGATIVE_INFINITY, 1},
                {3, 4, 0, NEGATIVE_INFINITY, 2},
                {1, 2, NEGATIVE_INFINITY, 0, NEGATIVE_INFINITY},
                {NEGATIVE_INFINITY, NEGATIVE_INFINITY, 2, 2, NEGATIVE_INFINITY},
                {3, 1, 4, 1, 0}}),
           Mat({{3, NEGATIVE_INFINITY, 3, 1, 3},
                {NEGATIVE_INFINITY, 2, 1, 0, 0},
                {3, 2, 1, 0, NEGATIVE_INFINITY},
                {5, 3, 3, 2, 1},
                {NEGATIVE_INFINITY, NEGATIVE_INFINITY, 5, 1, 4}}),
           Mat({{1, 1, 0, 2, 0},
                {1, 0, 2, 1, NEGATIVE_INFINITY},
                {3, 4, 1, 2, NEGATIVE_INFINITY},
                {0, 0, 1, 0, NEGATIVE_INFINITY},
                {2, 0, NEGATIVE_INFINITY, 1, 2}}),
           Mat({{NEGATIVE_INFINITY, 2, 0, 1, 2},
                {3, 2, 0, 1, 4},
                {4, 3, 1, 2, 5},
                {3, 2, NEGATIVE_INFINITY, 0, 4},
                {NEGATIVE_INFINITY, NEGATIVE_INFINITY, 5, 1, 4}}),
           Mat({{NEGATIVE_INFINITY, 2, 0, 1, 2},
                {2, 3, 2, 3, 6},
                {1, 2, 0, 1, 4},
                {1, NEGATIVE_INFINITY, NEGATIVE_INFINITY, 0, 2},
                {0, NEGATIVE_INFINITY, 0, 1, 4}})};

    FroidurePin S = make<FroidurePin>(mats);
    S.run();
    for (auto it = mats.cbegin(); it < mats.cend(); ++it) {
      LambdaVal x;
      Lambda()(x, *it);
      for (auto it2 = S.cbegin(); it2 < S.cend(); ++it2) {
        LambdaVal y;
        Lambda()(y, (*it) * (*it2));
        LambdaVal z;
        Action()(z, x, *it2);
        REQUIRE(y == z);
      }
    }
  }

  LIBSEMIGROUPS_TEST_CASE(
      "Konieczny",
      "030",
      "Rho/ImageLeftAction for tropical max-plus mats (d=5, t=6)",
      "[quick][konieczny][tropmaxplus]") {
    auto rg      = ReportGuard(false);
    using Mat    = MaxPlusTruncMat<6, 5>;
    using RhoVal = RhoValue<Mat>::type;
    using Rho    = Rho<Mat, RhoVal>;
    using Action = ImageLeftAction<Mat, RhoVal>;
    std::vector<Mat> mats
        = {Mat({{4, NEGATIVE_INFINITY, NEGATIVE_INFINITY, 4, NEGATIVE_INFINITY},
                {1, 0, 1, 2, 2},
                {1, 4, 1, 2, NEGATIVE_INFINITY},
                {NEGATIVE_INFINITY, 1, 1, NEGATIVE_INFINITY, 0},
                {NEGATIVE_INFINITY, 0, 1, 0, NEGATIVE_INFINITY}}),
           Mat({{2, 1, 0, NEGATIVE_INFINITY, 1},
                {3, 4, 0, NEGATIVE_INFINITY, 2},
                {1, 2, NEGATIVE_INFINITY, 0, NEGATIVE_INFINITY},
                {NEGATIVE_INFINITY, NEGATIVE_INFINITY, 2, 2, NEGATIVE_INFINITY},
                {3, 1, 4, 1, 0}}),
           Mat({{3, NEGATIVE_INFINITY, 3, 1, 3},
                {NEGATIVE_INFINITY, 2, 1, 0, 0},
                {3, 2, 1, 0, NEGATIVE_INFINITY},
                {5, 3, 3, 2, 1},
                {NEGATIVE_INFINITY, NEGATIVE_INFINITY, 5, 1, 4}}),
           Mat({{1, 1, 0, 2, 0},
                {1, 0, 2, 1, NEGATIVE_INFINITY},
                {3, 4, 1, 2, NEGATIVE_INFINITY},
                {0, 0, 1, 0, NEGATIVE_INFINITY},
                {2, 0, NEGATIVE_INFINITY, 1, 2}}),
           Mat({{NEGATIVE_INFINITY, 2, 0, 1, 2},
                {3, 2, 0, 1, 4},
                {4, 3, 1, 2, 5},
                {3, 2, NEGATIVE_INFINITY, 0, 4},
                {NEGATIVE_INFINITY, NEGATIVE_INFINITY, 5, 1, 4}}),
           Mat({{NEGATIVE_INFINITY, 2, 0, 1, 2},
                {2, 3, 2, 3, 6},
                {1, 2, 0, 1, 4},
                {1, NEGATIVE_INFINITY, NEGATIVE_INFINITY, 0, 2},
                {0, NEGATIVE_INFINITY, 0, 1, 4}})};

    FroidurePin S = make<FroidurePin>(mats);
    S.run();
    for (auto it = mats.cbegin(); it < mats.cend(); ++it) {
      RhoVal x;
      Rho()(x, *it);
      for (auto it2 = S.cbegin(); it2 < S.cend(); ++it2) {
        RhoVal y;
        Rho()(y, (*it2) * (*it));
        RhoVal z;
        Action()(z, x, *it2);
        REQUIRE(y == z);
      }
    }
  }

  LIBSEMIGROUPS_TEST_CASE("Konieczny",
                          "031",
                          "small tropical max-plus (dim 3, threshold 9)",
                          "[quick][konieczny][tropmaxplus]") {
    using Mat    = MaxPlusTruncMat<9, 2>;
    auto      rg = ReportGuard(false);
    Konieczny S
        = make<Konieczny>({Mat({{1, 3}, {2, 1}}), Mat({{2, 1}, {4, 0}})});

    REQUIRE(S.size() == 20);
  }

  LIBSEMIGROUPS_TEST_CASE("Konieczny",
                          "032",
                          "small tropical max-plus (dim 3, threshold 9)",
                          "[quick][konieczny][tropmaxplus]") {
    using Mat    = MaxPlusTruncMat<9, 3>;
    auto      rg = ReportGuard(false);
    Konieczny S  = make<Konieczny>(
        {Mat({{2, 2, 0}, {1, 0, 0}, {1, 3, 1}}),
          Mat({{NEGATIVE_INFINITY, 0, 0}, {0, 1, 0}, {1, 1, 0}}),
          Mat({{1, NEGATIVE_INFINITY, 0}, {2, 1, 0}, {2, 2, 0}})});

    REQUIRE(S.size() == 423);
  }

  LIBSEMIGROUPS_TEST_CASE(
      "Konieczny",
      "033",
      "large tropical max-plus example (dim 5, threshold 6)",
      "[extreme][konieczny][tropmaxplus]") {
    using Mat = MaxPlusTruncMat<6, 5>;
    auto rg   = ReportGuard(true);
    auto S    = make<Konieczny>(
        {Mat({{NEGATIVE_INFINITY,
                  NEGATIVE_INFINITY,
                  NEGATIVE_INFINITY,
                  0,
                  NEGATIVE_INFINITY},
                 {NEGATIVE_INFINITY,
                  NEGATIVE_INFINITY,
                  NEGATIVE_INFINITY,
                  NEGATIVE_INFINITY,
                  0},
                 {0,
                  NEGATIVE_INFINITY,
                  NEGATIVE_INFINITY,
                  NEGATIVE_INFINITY,
                  NEGATIVE_INFINITY},
                 {NEGATIVE_INFINITY,
                  0,
                  NEGATIVE_INFINITY,
                  NEGATIVE_INFINITY,
                  NEGATIVE_INFINITY},
                 {0,
                  NEGATIVE_INFINITY,
                  NEGATIVE_INFINITY,
                  NEGATIVE_INFINITY,
                  NEGATIVE_INFINITY}}),
            Mat({{NEGATIVE_INFINITY,
                  NEGATIVE_INFINITY,
                  NEGATIVE_INFINITY,
                  NEGATIVE_INFINITY,
                  0},
                 {NEGATIVE_INFINITY,
                  NEGATIVE_INFINITY,
                  0,
                  NEGATIVE_INFINITY,
                  NEGATIVE_INFINITY},
                 {NEGATIVE_INFINITY,
                  NEGATIVE_INFINITY,
                  0,
                  NEGATIVE_INFINITY,
                  NEGATIVE_INFINITY},
                 {NEGATIVE_INFINITY,
                  NEGATIVE_INFINITY,
                  0,
                  NEGATIVE_INFINITY,
                  NEGATIVE_INFINITY},
                 {0,
                  NEGATIVE_INFINITY,
                  NEGATIVE_INFINITY,
                  NEGATIVE_INFINITY,
                  NEGATIVE_INFINITY}}),
            // NOLINTNEXTLINE(whitespace/line_length)
            Mat({{4, NEGATIVE_INFINITY, NEGATIVE_INFINITY, 4, NEGATIVE_INFINITY},
                 {1, 0, 1, 2, 2},
                 {1, 4, 1, 2, NEGATIVE_INFINITY},
                 {NEGATIVE_INFINITY, 1, 1, NEGATIVE_INFINITY, 0},
                 {NEGATIVE_INFINITY, 0, 1, 0, NEGATIVE_INFINITY}}),
            Mat({{2, 1, 0, NEGATIVE_INFINITY, 1},
                 {3, 4, 0, NEGATIVE_INFINITY, 2},
                 {1, 2, NEGATIVE_INFINITY, 0, NEGATIVE_INFINITY},
                 // NOLINTNEXTLINE(whitespace/line_length)
                 {NEGATIVE_INFINITY, NEGATIVE_INFINITY, 2, 2, NEGATIVE_INFINITY},
                 {3, 1, 4, 1, 0}})});

    REQUIRE(S.size() == 743'448);
  }

  LIBSEMIGROUPS_TEST_CASE("Konieczny",
                          "034",
                          "full tropical max-plus monoid (dim 2, threshold 8)",
                          "[standard][konieczny][tropmaxplus]") {
    using Mat = MaxPlusTruncMat<8, 2>;

    auto rg = ReportGuard(false);

    Konieczny S = make<Konieczny>(
        {Mat({{NEGATIVE_INFINITY, 0}, {NEGATIVE_INFINITY, NEGATIVE_INFINITY}}),
         Mat({{NEGATIVE_INFINITY, 0}, {0, NEGATIVE_INFINITY}}),
         Mat({{NEGATIVE_INFINITY, 0}, {0, 0}}),
         Mat({{NEGATIVE_INFINITY, 1}, {0, NEGATIVE_INFINITY}}),
         Mat({{NEGATIVE_INFINITY, 0}, {0, 1}}),
         Mat({{0, 1}, {1, 0}}),
         Mat({{NEGATIVE_INFINITY, 0}, {0, 2}}),
         Mat({{0, 1}, {2, 0}}),
         Mat({{0, 2}, {2, 0}}),
         Mat({{NEGATIVE_INFINITY, 0}, {0, 3}}),
         Mat({{0, 1}, {3, 0}}),
         Mat({{0, 2}, {3, 0}}),
         Mat({{0, 3}, {3, 0}}),
         Mat({{NEGATIVE_INFINITY, 0}, {0, 4}}),
         Mat({{0, 1}, {4, 0}}),
         Mat({{0, 2}, {4, 0}}),
         Mat({{0, 3}, {4, 0}}),
         Mat({{0, 4}, {4, 0}}),
         Mat({{NEGATIVE_INFINITY, 0}, {0, 5}}),
         Mat({{0, 1}, {5, 0}}),
         Mat({{0, 2}, {5, 0}}),
         Mat({{0, 3}, {5, 0}}),
         Mat({{0, 4}, {5, 0}}),
         Mat({{0, 5}, {5, 0}}),
         Mat({{NEGATIVE_INFINITY, 0}, {0, 6}}),
         Mat({{0, 1}, {6, 0}}),
         Mat({{0, 2}, {6, 0}}),
         Mat({{0, 3}, {6, 0}}),
         Mat({{0, 4}, {6, 0}}),
         Mat({{0, 5}, {6, 0}}),
         Mat({{0, 6}, {6, 0}}),
         Mat({{NEGATIVE_INFINITY, 0}, {0, 7}}),
         Mat({{0, 1}, {7, 0}}),
         Mat({{0, 2}, {7, 0}}),
         Mat({{0, 3}, {7, 0}}),
         Mat({{0, 4}, {7, 0}}),
         Mat({{0, 5}, {7, 0}}),
         Mat({{0, 6}, {7, 0}}),
         Mat({{0, 7}, {7, 0}}),
         Mat({{NEGATIVE_INFINITY, 0}, {0, 8}}),
         Mat({{0, 1}, {8, 0}}),
         Mat({{0, 2}, {8, 0}}),
         Mat({{0, 3}, {8, 0}}),
         Mat({{0, 4}, {8, 0}}),
         Mat({{0, 5}, {8, 0}}),
         Mat({{0, 6}, {8, 0}}),
         Mat({{0, 7}, {8, 0}}),
         Mat({{0, 8}, {8, 0}})});

    REQUIRE(S.number_of_D_classes() == 2'200);
    REQUIRE(S.size() == 10'000);
  }

  LIBSEMIGROUPS_TEST_CASE(
      "Konieczny",
      "035",
      "reg-gen bmat gens + arrow - tropical max-plus (dim 4, t 1)",
      "[extreme][konieczny][tropmaxplus]") {
    using Mat = MaxPlusTruncMat<10, 4>;
    auto rg   = ReportGuard(true);

    Konieczny S = make<Konieczny>(
        {Mat({{NEGATIVE_INFINITY, 0, NEGATIVE_INFINITY, NEGATIVE_INFINITY},
              {0, NEGATIVE_INFINITY, NEGATIVE_INFINITY, NEGATIVE_INFINITY},
              {NEGATIVE_INFINITY, NEGATIVE_INFINITY, 0, NEGATIVE_INFINITY},
              {NEGATIVE_INFINITY, NEGATIVE_INFINITY, NEGATIVE_INFINITY, 0}}),
         Mat({{NEGATIVE_INFINITY, 0, NEGATIVE_INFINITY, NEGATIVE_INFINITY},
              {NEGATIVE_INFINITY, NEGATIVE_INFINITY, 0, NEGATIVE_INFINITY},
              {NEGATIVE_INFINITY, NEGATIVE_INFINITY, NEGATIVE_INFINITY, 0},
              {0, NEGATIVE_INFINITY, NEGATIVE_INFINITY, NEGATIVE_INFINITY}}),
         Mat({{0, NEGATIVE_INFINITY, NEGATIVE_INFINITY, NEGATIVE_INFINITY},
              {0, 0, NEGATIVE_INFINITY, NEGATIVE_INFINITY},
              {NEGATIVE_INFINITY, NEGATIVE_INFINITY, 0, NEGATIVE_INFINITY},
              {NEGATIVE_INFINITY, NEGATIVE_INFINITY, NEGATIVE_INFINITY, 0}}),
         Mat({{NEGATIVE_INFINITY,
               NEGATIVE_INFINITY,
               NEGATIVE_INFINITY,
               NEGATIVE_INFINITY},
              {NEGATIVE_INFINITY, 0, NEGATIVE_INFINITY, NEGATIVE_INFINITY},
              {NEGATIVE_INFINITY, NEGATIVE_INFINITY, 0, NEGATIVE_INFINITY},
              {NEGATIVE_INFINITY, NEGATIVE_INFINITY, NEGATIVE_INFINITY, 0}}),
         Mat({{1, 1, 1, 1},
              {NEGATIVE_INFINITY, 1, NEGATIVE_INFINITY, NEGATIVE_INFINITY},
              {NEGATIVE_INFINITY, NEGATIVE_INFINITY, 1, NEGATIVE_INFINITY},
              {NEGATIVE_INFINITY, NEGATIVE_INFINITY, NEGATIVE_INFINITY, 1}})});
    REQUIRE(S.size() == 582'214);
  }

  LIBSEMIGROUPS_TEST_CASE(
      "Konieczny",
      "036",
      "regular-generated bmat gens - tropical max-plus (dim 5, t 2)",
      "[extreme][konieczny][tropmaxplus]") {
    using Mat = MaxPlusTruncMat<2, 5>;
    auto rg   = ReportGuard(true);

    Konieczny S = make<Konieczny>(
        {Mat({{NEGATIVE_INFINITY,
               0,
               NEGATIVE_INFINITY,
               NEGATIVE_INFINITY,
               NEGATIVE_INFINITY},
              {0,
               NEGATIVE_INFINITY,
               NEGATIVE_INFINITY,
               NEGATIVE_INFINITY,
               NEGATIVE_INFINITY},
              {NEGATIVE_INFINITY,
               NEGATIVE_INFINITY,
               0,
               NEGATIVE_INFINITY,
               NEGATIVE_INFINITY},
              {NEGATIVE_INFINITY,
               NEGATIVE_INFINITY,
               NEGATIVE_INFINITY,
               0,
               NEGATIVE_INFINITY},
              {NEGATIVE_INFINITY,
               NEGATIVE_INFINITY,
               NEGATIVE_INFINITY,
               NEGATIVE_INFINITY,
               0}}),
         Mat({{NEGATIVE_INFINITY,
               0,
               NEGATIVE_INFINITY,
               NEGATIVE_INFINITY,
               NEGATIVE_INFINITY},
              {NEGATIVE_INFINITY,
               NEGATIVE_INFINITY,
               0,
               NEGATIVE_INFINITY,
               NEGATIVE_INFINITY},
              {NEGATIVE_INFINITY,
               NEGATIVE_INFINITY,
               NEGATIVE_INFINITY,
               0,
               NEGATIVE_INFINITY},
              {NEGATIVE_INFINITY,
               NEGATIVE_INFINITY,
               NEGATIVE_INFINITY,
               NEGATIVE_INFINITY,
               0},
              {0,
               NEGATIVE_INFINITY,
               NEGATIVE_INFINITY,
               NEGATIVE_INFINITY,
               NEGATIVE_INFINITY}}),
         Mat({{0,
               NEGATIVE_INFINITY,
               NEGATIVE_INFINITY,
               NEGATIVE_INFINITY,
               NEGATIVE_INFINITY},
              {0, 0, NEGATIVE_INFINITY, NEGATIVE_INFINITY, NEGATIVE_INFINITY},
              {NEGATIVE_INFINITY,
               NEGATIVE_INFINITY,
               0,
               NEGATIVE_INFINITY,
               NEGATIVE_INFINITY},
              {NEGATIVE_INFINITY,
               NEGATIVE_INFINITY,
               NEGATIVE_INFINITY,
               0,
               NEGATIVE_INFINITY},
              {NEGATIVE_INFINITY,
               NEGATIVE_INFINITY,
               NEGATIVE_INFINITY,
               NEGATIVE_INFINITY,
               0}}),
         Mat({{NEGATIVE_INFINITY,
               NEGATIVE_INFINITY,
               NEGATIVE_INFINITY,
               NEGATIVE_INFINITY,
               NEGATIVE_INFINITY},
              {NEGATIVE_INFINITY,
               0,
               NEGATIVE_INFINITY,
               NEGATIVE_INFINITY,
               NEGATIVE_INFINITY},
              {NEGATIVE_INFINITY,
               NEGATIVE_INFINITY,
               0,
               NEGATIVE_INFINITY,
               NEGATIVE_INFINITY},
              {NEGATIVE_INFINITY,
               NEGATIVE_INFINITY,
               NEGATIVE_INFINITY,
               NEGATIVE_INFINITY,
               0},
              {NEGATIVE_INFINITY,
               NEGATIVE_INFINITY,
               NEGATIVE_INFINITY,
               NEGATIVE_INFINITY,
               0}}),
         Mat({{1, 1, 1, 1, 1},
              {NEGATIVE_INFINITY,
               1,
               NEGATIVE_INFINITY,
               NEGATIVE_INFINITY,
               NEGATIVE_INFINITY},
              {NEGATIVE_INFINITY,
               NEGATIVE_INFINITY,
               1,
               NEGATIVE_INFINITY,
               NEGATIVE_INFINITY},
              {NEGATIVE_INFINITY,
               NEGATIVE_INFINITY,
               NEGATIVE_INFINITY,
               1,
               NEGATIVE_INFINITY},
              {NEGATIVE_INFINITY,
               NEGATIVE_INFINITY,
               NEGATIVE_INFINITY,
               NEGATIVE_INFINITY,
               1}})});
    REQUIRE(S.size() == 43'059'454);
  }

  LIBSEMIGROUPS_TEST_CASE("Konieczny",
                          "037",
                          "dynamic tropical max-plus mats (dim 2, threshold 9)",
                          "[quick][konieczny][tropmaxplus]") {
    using Mat    = MaxPlusTruncMat<9>;
    auto      rg = ReportGuard(false);
    Konieczny S
        = make<Konieczny>({Mat({{1, 3}, {2, 1}}), Mat({{2, 1}, {4, 0}})});
    REQUIRE(S.size() == 20);
  }

  LIBSEMIGROUPS_TEST_CASE("Konieczny",
                          "038",
                          "dynamic tropical max-plus mats (dim 2, threshold 5)",
                          "[quick][konieczny][tropmaxplus]") {
    using Mat    = MaxPlusTruncMat<5>;
    auto      rg = ReportGuard(false);
    Konieczny S  = make<Konieczny>(
        {Mat({{1, 3}, {2, 1}}),
          Mat({{2, 1}, {4, 0}}),
          Mat({{0, NEGATIVE_INFINITY}, {NEGATIVE_INFINITY, 0}}),
          Mat({{0, NEGATIVE_INFINITY}, {NEGATIVE_INFINITY, NEGATIVE_INFINITY}}),
          Mat({{NEGATIVE_INFINITY, 0}, {1, NEGATIVE_INFINITY}})});
    REQUIRE(S.size() == 173);
  }

  LIBSEMIGROUPS_TEST_CASE(
      "Konieczny",
      "039",
      "dynamic sr tropical max-plus mats (dim 2, threshold 5)",
      "[quick][konieczny][tropmaxplus]") {
    using Mat = MaxPlusTruncMat<>;

    auto rg = ReportGuard(false);
    auto sr = std::make_unique<MaxPlusTruncSemiring<>>(5);

    Konieczny S = make<Konieczny>(
        {make<Mat>(sr.get(), {{1, 3}, {2, 1}}),
         make<Mat>(sr.get(), {{2, 1}, {4, 0}}),
         make<Mat>(sr.get(), {{0, NEGATIVE_INFINITY}, {NEGATIVE_INFINITY, 0}}),
         make<Mat>(
             sr.get(),
             {{0, NEGATIVE_INFINITY}, {NEGATIVE_INFINITY, NEGATIVE_INFINITY}}),
         make<Mat>(sr.get(),
                   {{NEGATIVE_INFINITY, 0}, {1, NEGATIVE_INFINITY}})});

    auto bad = std::make_unique<MaxPlusTruncSemiring<>>(13);
    REQUIRE_EXCEPTION_MSG(
        S.add_generator(make<Mat>(bad.get(), {{1, 3}, {2, 1}})),
        "the matrix has threshold 13 but should have threshold 5");

    REQUIRE(S.size() == 173);
  }
}  // namespace libsemigroups
