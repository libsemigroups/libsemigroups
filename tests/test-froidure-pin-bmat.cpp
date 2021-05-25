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
#include <vector>   // for vector

#include "bmat-data.hpp"                   // for clark_gens
#include "catch.hpp"                       // for REQUIRE
#include "libsemigroups/froidure-pin.hpp"  // for FroidurePin<>::element_index_type
#include "libsemigroups/matrix.hpp"        // for BMat
#include "libsemigroups/report.hpp"        // for ReportGuard
#include "test-main.hpp"                   // for LIBSEMIGROUPS_TEST_CASE

namespace libsemigroups {
  // Forward declaration
  struct LibsemigroupsException;

  bool constexpr REPORT = false;

  namespace {
    ////////////////////////////////////////////////////////////////////////
    // Test functions
    ////////////////////////////////////////////////////////////////////////

    template <typename Mat>
    void test000() {
      auto             rg = ReportGuard(REPORT);
      FroidurePin<Mat> S;
      S.add_generator(
          Mat({{0, 1, 0, 1}, {1, 0, 0, 0}, {0, 1, 1, 1}, {0, 1, 1, 0}}));
      S.add_generator(
          Mat({{0, 1, 1, 1}, {1, 1, 0, 0}, {0, 0, 0, 0}, {1, 1, 1, 1}}));
      S.add_generator(
          Mat({{0, 1, 1, 0}, {0, 1, 1, 0}, {0, 1, 1, 1}, {1, 1, 1, 1}}));

      S.reserve(26);

      REQUIRE(S.size() == 26);
      REQUIRE(S.number_of_idempotents() == 4);
      size_t pos = 0;

      for (auto it = S.cbegin(); it < S.cend(); ++it) {
        REQUIRE(S.position(*it) == pos);
        pos++;
      }

      S.add_generators(
          {Mat({{1, 0, 0, 1}, {0, 1, 0, 1}, {0, 0, 1, 1}, {1, 1, 1, 0}})});
      REQUIRE(S.size() == 29);
      S.closure(
          {Mat({{1, 0, 0, 1}, {0, 1, 0, 1}, {0, 0, 1, 1}, {1, 1, 1, 0}})});
      REQUIRE(S.size() == 29);
      REQUIRE(
          S.minimal_factorisation(
              Mat({{1, 0, 0, 1}, {0, 1, 0, 1}, {0, 0, 1, 1}, {1, 1, 1, 0}})
              * Mat({{0, 1, 0, 1}, {1, 0, 0, 0}, {0, 1, 1, 1}, {0, 1, 1, 0}}))
          == word_type({3, 0}));
      REQUIRE(S.minimal_factorisation(28) == word_type({3, 0}));
      REQUIRE(
          S.at(28)
          == Mat({{1, 0, 0, 1}, {0, 1, 0, 1}, {0, 0, 1, 1}, {1, 1, 1, 0}})
                 * Mat(
                     {{0, 1, 0, 1}, {1, 0, 0, 0}, {0, 1, 1, 1}, {0, 1, 1, 0}}));
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

    // BMat
    template <typename Mat>
    void test001() {
      auto             rg = ReportGuard(REPORT);
      FroidurePin<Mat> S;
      S.add_generator(
          Mat({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}));
      S.add_generator(
          Mat({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}));
      S.add_generator(
          Mat({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {1, 0, 0, 1}}));
      S.add_generator(
          Mat({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}}));
      REQUIRE(S.size() == 63904);
      REQUIRE(S.number_of_idempotents() == 2360);
    }

    // BMat
    template <typename Mat>
    void test002() {
      auto             rg = ReportGuard(REPORT);
      FroidurePin<Mat> S;
      S.add_generator(Mat({{1, 0, 1}, {0, 1, 0}, {0, 1, 0}}));
      S.add_generator(Mat({{0, 0, 0}, {0, 0, 0}, {0, 0, 0}}));
      S.add_generator(Mat({{0, 0, 0}, {0, 0, 0}, {0, 0, 0}}));

      REQUIRE(S.size() == 3);
      REQUIRE(S.degree() == 3);
      REQUIRE(S.number_of_idempotents() == 2);
      REQUIRE(S.number_of_generators() == 3);
      REQUIRE(S.number_of_rules() == 7);
      REQUIRE(S[0] == S.generator(0));
      REQUIRE(S[1] == S.generator(1));
      REQUIRE(S[1] == S.generator(2));

      REQUIRE(S.position(S.generator(0)) == 0);
      REQUIRE(S.contains(S.generator(0)));

      REQUIRE(S.position(S.generator(1)) == 1);
      REQUIRE(S.contains(S.generator(1)));

      REQUIRE(S.position(S.generator(2)) == 1);
      REQUIRE(S.contains(S.generator(1)));

      Mat y({{0, 0, 0}, {0, 0, 0}, {0, 0, 0}});
      y.product_inplace(S.generator(0), S.generator(0));
      REQUIRE(S.position(y) == 2);
      REQUIRE(S.contains(y));
      REQUIRE(S.fast_product(1, 2) == 1);
    }

    template <typename Mat>
    void test003() {
      std::vector<Mat> gens
          = {Mat({{1, 0, 0, 0}, {0, 0, 1, 0}, {1, 0, 0, 1}, {0, 1, 0, 0}}),
             Mat({{1, 0, 0, 1}, {1, 0, 0, 1}, {1, 1, 1, 1}, {0, 1, 1, 0}}),
             Mat({{1, 0, 1, 0}, {1, 0, 1, 1}, {0, 0, 1, 1}, {0, 1, 0, 1}}),
             Mat({{0, 0, 0, 0}, {0, 1, 0, 1}, {1, 1, 1, 0}, {1, 0, 0, 1}}),
             Mat({{0, 0, 0, 1}, {0, 0, 1, 0}, {1, 0, 0, 1}, {1, 1, 0, 0}})};

      FroidurePin<Mat> S(gens);
      REQUIRE(S.size() == 415);
    }

    template <typename Mat>
    void test004() {
      auto             rg = ReportGuard(true);
      FroidurePin<Mat> S;
      for (auto const& v : konieczny_data::clark_gens) {
        S.add_generator(Mat(v));
      }
      REQUIRE(S.generator(0).number_of_rows() == 40);
      S.run();
      REQUIRE(S.size() == 248017);
    }

  }  // namespace

  ////////////////////////////////////////////////////////////////////////
  // Test cases - BMat
  ////////////////////////////////////////////////////////////////////////

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<BMat<4>>",
                          "005",
                          "small example 1",
                          "[quick][froidure-pin][bmat]") {
    test000<BMat<4>>();
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<BMat<>>",
                          "006",
                          "small example 1",
                          "[quick][froidure-pin][bmat]") {
    test000<BMat<>>();
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<BMat<4>>",
                          "007",
                          "regular bmat monoid 4",
                          "[quick][froidure-pin][bmat][no-valgrind]") {
    test001<BMat<4>>();
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<BMat<>>",
                          "008",
                          "regular bmat monoid 4",
                          "[quick][froidure-pin][bmat][no-valgrind]") {
    test001<BMat<>>();
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<BMat<3>>",
                          "009",
                          "small example 2",
                          "[quick][froidure-pin][bmat]") {
    test002<BMat<3>>();
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<BMat<>>",
                          "010",
                          "small example 2",
                          "[quick][froidure-pin][bmat]") {
    test002<BMat<>>();
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<BMat<4>>",
                          "011",
                          "small example 3",
                          "[quick][froidure-pin][bmat]") {
    test003<BMat<4>>();
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<BMat<>>",
                          "012",
                          "small example 3",
                          "[quick][froidure-pin][bmat]") {
    test003<BMat<>>();
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<BMat<>>",
                          "013",
                          "Clark generators",
                          "[extreme][froidure-pin][bmat]") {
    test004<BMat<>>();
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<BMat<40>>",
                          "014",
                          "Clark generators",
                          "[extreme][froidure-pin][bmat]") {
    test004<BMat<40>>();
  }

}  // namespace libsemigroups
