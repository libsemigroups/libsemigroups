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
#include <cstdint>  // for int64_t

#include "catch.hpp"      // for REQUIRE, REQUIRE_THROWS_AS
#include "test-main.hpp"  // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/constants.hpp"     // for UNDEFINED
#include "libsemigroups/froidure-pin.hpp"  // for FroidurePin
#include "libsemigroups/matrix.hpp"        // for IntMat

namespace libsemigroups {
  // Forward declaration
  struct LibsemigroupsException;

  constexpr bool REPORT = false;

  namespace {
    template <typename Mat>
    void test_IntMat000() {
      auto rg = ReportGuard(REPORT);

      FroidurePin<Mat> S;
      S.add_generator(Mat({{0, 1}, {0, -1}}));
      S.add_generator(Mat({{0, 1}, {2, 0}}));

      REQUIRE(Mat({{0, 1}, {0, -1}}) * Mat({{0, 1}, {2, 0}})
                  * Mat({{0, 1}, {2, 0}})
              == S.generator(0) * S.generator(1) * S.generator(0));
      REQUIRE(Mat::make({{64, 0}, {-64, 0}})
              == S.generator(0) * S.generator(1) * S.generator(0)
                     * S.generator(1) * S.generator(0) * S.generator(1)
                     * S.generator(0) * S.generator(1) * S.generator(0)
                     * S.generator(1) * S.generator(0) * S.generator(1));

      S.reserve(10000);

      S.enumerate(10000);
      REQUIRE(S.finished());
      REQUIRE(S.current_size() == 631);
      size_t pos = 0;

      for (auto it = S.cbegin(); it < S.cend(); ++it) {
        REQUIRE(S.position(*it) == pos);
        pos++;
      }
      S.enumerate(1000000);
      REQUIRE(S.current_size() == 631);
      REQUIRE(
          S.minimal_factorisation(Mat({{0, 1}, {0, -1}}) * Mat({{0, 1}, {2, 0}})
                                  * Mat({{0, 1}, {2, 0}}))
          == word_type({0, 1, 0}));
      REQUIRE(S.minimal_factorisation(52)
              == word_type({0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1}));
      REQUIRE(S.at(52) == Mat({{64, 0}, {-64, 0}}));
      REQUIRE_THROWS_AS(S.minimal_factorisation(1000000000),
                        LibsemigroupsException);
    }

    template <typename Mat>
    void test_IntMat001() {
      auto             rg = ReportGuard(REPORT);
      FroidurePin<Mat> S;
      S.add_generator(Mat({{0, 0}, {0, 1}}));
      S.add_generator(Mat({{0, 1}, {-1, 0}}));

      REQUIRE(S.size() == 13);
      REQUIRE(S.degree() == 2);
      REQUIRE(S.number_of_idempotents() == 4);
      REQUIRE(S.number_of_generators() == 2);
      REQUIRE(S.number_of_rules() == 6);
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
      REQUIRE(S.position(x) == 4);
      REQUIRE(S.contains(x));

      // x = new MatrixOverSemiring<int64_t>({{-2, 2, 0}, {-1, 0, 0}, {0, 0,
      // 0}},
      //                                    sr);
      // REQUIRE(S.position(x) == UNDEFINED);
      // REQUIRE(!S.contains(x));
    }

    // IntMat
    template <typename Mat>
    void test008() {
      FroidurePin<Mat> T;
      T.add_generator(Mat({{0, 0}, {0, 1}}));
      T.add_generator(Mat({{0, 1}, {-1, 0}}));
      REQUIRE_THROWS_AS(T.current_position({}), LibsemigroupsException);
      REQUIRE_NOTHROW(T.current_position({0, 0, 1, 1}));
      REQUIRE(T.current_position({0, 0, 1, 1}) == UNDEFINED);
      auto w = T.word_to_element({0, 0, 1, 1});
      REQUIRE(T.current_position(w) == UNDEFINED);
      REQUIRE_THROWS_AS(T.current_position({0, 0, 1, 2}),
                        LibsemigroupsException);

      REQUIRE(T.size() == 13);
      REQUIRE(T.current_position({0, 0, 1, 1}) == 6);
      w = T.word_to_element({0, 0, 1, 1});
      REQUIRE(T.current_position(w) == 6);
    }

    template <typename Mat>
    void test009() {
      FroidurePin<Mat> T;
      T.add_generator(Mat({{0, 0}, {0, 1}}));
      T.add_generator(Mat({{0, 1}, {-1, 0}}));

      REQUIRE_THROWS_AS(T.word_to_element({}), LibsemigroupsException);
      REQUIRE_THROWS_AS(T.word_to_element({0, 0, 1, 2}),
                        LibsemigroupsException);

      auto t = T.word_to_element({0, 0, 1, 1});
      REQUIRE(t
              == T.generator(0) * T.generator(0) * T.generator(1)
                     * T.generator(1));
    }
    template <typename Mat>
    void test010() {
      FroidurePin<Mat> T;
      T.add_generator(Mat({{0, 0}, {0, 1}}));
      T.add_generator(Mat({{0, 1}, {-1, 0}}));

      for (size_t i = 0; i < T.size(); ++i) {
        REQUIRE_NOTHROW(T.prefix(i));
        REQUIRE_THROWS_AS(T.prefix(i + T.size()), LibsemigroupsException);
      }
      for (size_t i = 0; i < T.size(); ++i) {
        REQUIRE_NOTHROW(T.suffix(i));
        REQUIRE_THROWS_AS(T.suffix(i + T.size()), LibsemigroupsException);
      }
      for (size_t i = 0; i < T.size(); ++i) {
        REQUIRE_NOTHROW(T.first_letter(i));
        REQUIRE_THROWS_AS(T.first_letter(i + T.size()), LibsemigroupsException);
      }
      for (size_t i = 0; i < T.size(); ++i) {
        REQUIRE_NOTHROW(T.final_letter(i));
        REQUIRE_THROWS_AS(T.final_letter(i + T.size()), LibsemigroupsException);
      }
      for (size_t i = 0; i < T.size(); ++i) {
        REQUIRE_NOTHROW(T.current_length(i));
        REQUIRE_THROWS_AS(T.current_length(i + T.size()),
                          LibsemigroupsException);
      }
      for (size_t i = 0; i < T.size(); ++i) {
        for (size_t j = 0; j < T.size(); ++j) {
          REQUIRE_NOTHROW(T.product_by_reduction(i, j));
          REQUIRE_THROWS_AS(T.product_by_reduction(i + T.size(), j),
                            LibsemigroupsException);
          REQUIRE_THROWS_AS(T.product_by_reduction(i, j + T.size()),
                            LibsemigroupsException);
          REQUIRE_THROWS_AS(T.product_by_reduction(i + T.size(), j + T.size()),
                            LibsemigroupsException);
        }
      }
      for (size_t i = 0; i < T.size(); ++i) {
        for (size_t j = 0; j < T.size(); ++j) {
          REQUIRE_NOTHROW(T.fast_product(i, j));
          REQUIRE_THROWS_AS(T.fast_product(i + T.size(), j),
                            LibsemigroupsException);
          REQUIRE_THROWS_AS(T.fast_product(i, j + T.size()),
                            LibsemigroupsException);
          REQUIRE_THROWS_AS(T.fast_product(i + T.size(), j + T.size()),
                            LibsemigroupsException);
        }
      }
    }
  }  // namespace

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<IntMat<2, int64_t>>",
                          "032",
                          "Example 000",
                          "[quick][froidure-pin][intmat]") {
    test_IntMat000<IntMat<2, 2, int64_t>>();  // Static
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<IntMat<0, int64_t>>",
                          "033",
                          "Example 000",
                          "[quick][froidure-pin][intmat]") {
    test_IntMat000<IntMat<0, 0, int64_t>>();
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<IntMat<2, int64_t>>",
                          "034",
                          "Example 001",
                          "[quick][froidure-pin][intmat]") {
    test_IntMat001<IntMat<2, 2, int64_t>>();  // Static
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<IntMat<0, int64_t>>",
                          "035",
                          "Example 001",
                          "[quick][froidure-pin][intmat]") {
    test_IntMat001<IntMat<0, 0, int64_t>>();  // Static
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<IntMat>",
                          "036",
                          "exception: current_position",
                          "[quick][froidure-pin][element]") {
    test008<IntMat<2>>();
    test008<IntMat<>>();
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<IntMat>",
                          "037",
                          "exception: word_to_element",
                          "[quick][froidure-pin][element]") {
    test009<IntMat<2>>();
    test009<IntMat<>>();
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<IntMat>",
                          "038",
                          "exception: prefix, suffix, first_letter",
                          "[quick][froidure-pin][element]") {
    test010<IntMat<2>>();
    test010<IntMat<>>();
  }
}  // namespace libsemigroups
