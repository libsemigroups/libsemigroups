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

#include "bmat-data.hpp"                // for clark_gens
#include "libsemigroups/bmat.hpp"       // for BMat adapters
#include "libsemigroups/konieczny.hpp"  // for Konieczny
#include "libsemigroups/matrix.hpp"     // for BMat

namespace libsemigroups {

  constexpr bool REPORT = false;

  ////////////////////////////////////////////////////////////////////////
  // Test functions
  ////////////////////////////////////////////////////////////////////////

  namespace {

    template <typename Mat>
    void test000() {
      auto             rg = ReportGuard(REPORT);
      std::vector<Mat> gens
          = {Mat({{0, 1, 0, 1}, {1, 0, 0, 0}, {0, 1, 1, 1}, {0, 1, 1, 0}}),
             Mat({{0, 1, 1, 1}, {1, 1, 0, 0}, {0, 0, 0, 0}, {1, 1, 1, 1}}),
             Mat({{0, 1, 1, 0}, {0, 1, 1, 0}, {0, 1, 1, 1}, {1, 1, 1, 1}})};

      Konieczny<Mat> S(gens);
      REQUIRE(S.size() == 26);
    }

    template <typename Mat>
    void test001() {
      auto             rg = ReportGuard(REPORT);
      std::vector<Mat> gens
          = {Mat({{1, 0, 0, 0}, {0, 0, 1, 0}, {1, 0, 0, 1}, {0, 1, 0, 0}}),
             Mat({{1, 0, 0, 1}, {1, 0, 0, 1}, {1, 1, 1, 1}, {0, 1, 1, 0}}),
             Mat({{1, 0, 1, 0}, {1, 0, 1, 1}, {0, 0, 1, 1}, {0, 1, 0, 1}}),
             Mat({{0, 0, 0, 0}, {0, 1, 0, 1}, {1, 1, 1, 0}, {1, 0, 0, 1}}),
             Mat({{0, 0, 0, 1}, {0, 0, 1, 0}, {1, 0, 0, 1}, {1, 1, 0, 0}})};

      Konieczny<Mat> S(gens);
      REQUIRE(S.size() == 415);
    }

    template <typename Mat>
    void test002() {
      auto           rg = ReportGuard(true);
      Konieczny<Mat> S;
      for (auto const& v : konieczny_data::clark_gens) {
        S.add_generator(Mat(v));
      }
      REQUIRE(S.generator(0).number_of_rows() == 40);
      S.run();
      REQUIRE(S.size() == 248017);
    }

    template <typename Mat>
    void test003() {
      auto             rg   = ReportGuard(REPORT);
      std::vector<Mat> gens = {Mat({{0, 1, 1, 1, 0},
                                    {0, 0, 1, 0, 0},
                                    {1, 0, 0, 1, 0},
                                    {1, 1, 1, 0, 0},
                                    {0, 1, 1, 1, 1}}),
                               Mat({{0, 0, 0, 1, 0},
                                    {0, 0, 1, 0, 0},
                                    {1, 0, 0, 0, 0},
                                    {0, 0, 0, 0, 0},
                                    {0, 1, 0, 1, 1}}),
                               Mat({{0, 0, 0, 1, 0},
                                    {1, 1, 0, 0, 0},
                                    {0, 0, 1, 1, 1},
                                    {1, 1, 0, 0, 1},
                                    {0, 0, 1, 1, 0}}),
                               Mat({{0, 1, 0, 0, 1},
                                    {0, 0, 1, 0, 1},
                                    {1, 0, 1, 0, 0},
                                    {0, 1, 1, 1, 0},
                                    {1, 0, 0, 0, 1}})};

      Konieczny<Mat> S(gens);
      REQUIRE(S.size() == 513);
    }
  }  // namespace

  ////////////////////////////////////////////////////////////////////////
  // Test cases
  ////////////////////////////////////////////////////////////////////////

  LIBSEMIGROUPS_TEST_CASE("Konieczny",
                          "000",
                          "test000<BMat<>>",
                          "[quick][bmat]") {
    test000<BMat<>>();
  }

  LIBSEMIGROUPS_TEST_CASE("Konieczny",
                          "001",
                          "test000<BMat<4>>",
                          "[quick][bmat]") {
    test000<BMat<4>>();
  }

  LIBSEMIGROUPS_TEST_CASE("Konieczny",
                          "002",
                          "test001<BMat<>>",
                          "[quick][bmat][no-valgrind]") {
    test001<BMat<>>();
  }

  LIBSEMIGROUPS_TEST_CASE("Konieczny",
                          "003",
                          "test001<BMat<4>>",
                          "[quick][bmat][no-valgrind]") {
    test001<BMat<4>>();
  }

  LIBSEMIGROUPS_TEST_CASE("Konieczny",
                          "004",
                          "BMat<>: generators from Sean Clark",
                          "[extreme][bmat]") {
    test002<BMat<>>();
  }

  LIBSEMIGROUPS_TEST_CASE("Konieczny",
                          "005",
                          "BMat<40>: generators from Sean Clark",
                          "[extreme][bmat]") {
    test002<BMat<40>>();
  }

  LIBSEMIGROUPS_TEST_CASE("Konieczny", "006", "exceptions", "[quick][bmat]") {
    auto rg = ReportGuard(REPORT);
    REQUIRE_THROWS_AS(
        Konieczny<BMat<>>(
            {BMat<>({{1, 0, 0, 0}, {0, 0, 1, 0}, {1, 0, 0, 1}, {0, 1, 0, 0}}),
             BMat<>({{1, 0, 0}, {1, 0, 0}, {1, 1, 1}})}),
        LibsemigroupsException);
    // This doesn't throw when using BMat<4>, so there's no test for that
  }

  LIBSEMIGROUPS_TEST_CASE("Konieczny",
                          "007",
                          "code coverage",
                          "[quick][bmat][no-valgrind]") {
    test003<BMat<>>();
  }

  LIBSEMIGROUPS_TEST_CASE("Konieczny",
                          "008",
                          "code coverage",
                          "[quick][bmat][no-valgrind]") {
    test003<BMat<5>>();
  }
}  // namespace libsemigroups
