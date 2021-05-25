//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019 James D. Mitchell
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
#include "test-main.hpp"  // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/froidure-pin.hpp"  // for FroidurePin
#include "libsemigroups/matrix.hpp"        // for ProjMaxPlusMat

namespace libsemigroups {
  // Forward declaration
  struct LibsemigroupsException;

  constexpr bool REPORT = false;

  namespace {
    template <typename Mat>
    void test000() {
      auto             rg = ReportGuard(REPORT);
      FroidurePin<Mat> S;
      S.add_generator(Mat({{0, 1, 2}, {3, 4, 1}, {2, 1, 1}}));
      S.add_generator(Mat({{0, 1, 1}, {1, 1, 1}, {0, 0, 0}}));
      S.add_generator(Mat({{0, 1, 1}, {0, 0, 1}, {1, 0, 0}}));

      S.reserve(142);

      REQUIRE(S.size() == 142);
      REQUIRE(S.number_of_idempotents() == 90);
      size_t pos = 0;

      for (auto it = S.cbegin(); it < S.cend(); ++it) {
        REQUIRE(S.position(*it) == pos);
        pos++;
      }

      S.add_generators({Mat({{1, 0, 0}, {1, 0, 1}, {0, 1, 0}})});
      REQUIRE(S.size() == 223);
      S.closure({Mat({{1, 0, 0}, {1, 0, 1}, {0, 1, 0}})});
      REQUIRE(S.size() == 223);
      REQUIRE(S.minimal_factorisation(Mat({{1, 0, 0}, {1, 0, 1}, {0, 1, 0}})
                                      * Mat({{0, 1, 2}, {3, 4, 1}, {2, 1, 1}}))
              == word_type({3, 0}));
      REQUIRE_THROWS_AS(S.minimal_factorisation(1000000000),
                        LibsemigroupsException);
      pos = 0;
      for (auto it = S.cbegin_idempotents(); it < S.cend_idempotents(); ++it) {
        REQUIRE(*it * *it == *it);
        pos++;
      }
      REQUIRE(pos == S.number_of_idempotents());
      for (auto it = S.cbegin_sorted() + 1; it < S.cend_sorted(); ++it) {
        REQUIRE(*(it - 1) < *it);
      }
    }

    template <typename Mat>
    void test001() {
      auto             rg = ReportGuard(REPORT);
      auto             id = Mat::identity(3);
      FroidurePin<Mat> S;
      S.add_generator(id);

      REQUIRE(S.size() == 1);
      REQUIRE(S.degree() == 3);
      REQUIRE(S.number_of_idempotents() == 1);
      REQUIRE(S.number_of_generators() == 1);
      REQUIRE(S.number_of_rules() == 1);
      REQUIRE(S[0] == id);

      REQUIRE(S.position(id) == 0);
      REQUIRE(S.contains(id));

      auto x = Mat({{-2, 2, 0}, {-1, 0, 0}, {1, -3, 1}});
      REQUIRE(S.position(x) == UNDEFINED);
      REQUIRE(!S.contains(x));
    }
  }  // namespace

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "061",
                          "projective max plus matrix",
                          "[quick][froidure-pin][projmaxplus]") {
    test000<ProjMaxPlusMat<3>>();
    test000<ProjMaxPlusMat<>>();
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "062",
                          "projective max plus matrix",
                          "[quick][froidure-pin][element]") {
    test001<ProjMaxPlusMat<3>>();
    test001<ProjMaxPlusMat<>>();
  }
}  // namespace libsemigroups
