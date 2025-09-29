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

#include <algorithm>  // for for_each
#include <chrono>     // for milliseconds
#include <cstddef>    // for size_t
#include <vector>     // for vector

#include "Catch2-3.8.0/catch_amalgamated.hpp"  // for REQUIRE
#include "test-main.hpp"                       // for LIBSEMIGROUPS_TEST...

#include "libsemigroups/bmat-fastest.hpp"  // for BMatFastest
#include "libsemigroups/bmat8.hpp"         // for BMat8
#include "libsemigroups/konieczny.hpp"     // for Konieczny, Koniecz...

#include "libsemigroups/detail/report.hpp"  // for ReportGuard

namespace libsemigroups {

  struct LibsemigroupsException;

  LIBSEMIGROUPS_TEST_CASE("Konieczny",
                          "012",
                          "full bmat monoid 5",
                          "[standard][bmat8]") {
    using BMat = BMatFastest<5>;
    auto rg    = ReportGuard(false);

    Konieczny T = make<Konieczny>({BMat({{1, 0, 0, 0, 0},
                                         {0, 1, 0, 0, 0},
                                         {0, 0, 1, 0, 0},
                                         {0, 0, 0, 1, 0},
                                         {0, 0, 0, 0, 1}}),
                                   BMat({{0, 1, 0, 0, 0},
                                         {0, 0, 1, 0, 0},
                                         {0, 0, 0, 1, 0},
                                         {0, 0, 0, 0, 1},
                                         {1, 0, 0, 0, 0}}),
                                   BMat({{0, 1, 0, 0, 0},
                                         {1, 0, 0, 0, 0},
                                         {0, 0, 1, 0, 0},
                                         {0, 0, 0, 1, 0},
                                         {0, 0, 0, 0, 1}}),
                                   BMat({{1, 0, 0, 0, 0},
                                         {0, 1, 0, 0, 0},
                                         {0, 0, 1, 0, 0},
                                         {0, 0, 0, 1, 0},
                                         {1, 0, 0, 0, 1}}),
                                   BMat({{1, 1, 0, 0, 0},
                                         {1, 0, 1, 0, 0},
                                         {0, 1, 0, 1, 0},
                                         {0, 0, 1, 1, 0},
                                         {0, 0, 0, 0, 1}}),
                                   BMat({{1, 1, 0, 0, 0},
                                         {1, 0, 1, 0, 0},
                                         {0, 1, 1, 0, 0},
                                         {0, 0, 0, 1, 0},
                                         {0, 0, 0, 0, 1}}),
                                   BMat({{1, 1, 1, 0, 0},
                                         {1, 0, 0, 1, 0},
                                         {0, 1, 0, 1, 0},
                                         {0, 0, 1, 1, 0},
                                         {0, 0, 0, 0, 1}}),
                                   BMat({{1, 1, 0, 0, 0},
                                         {1, 0, 1, 0, 0},
                                         {0, 1, 0, 1, 0},
                                         {0, 0, 1, 0, 1},
                                         {0, 0, 0, 1, 1}}),
                                   BMat({{1, 1, 1, 1, 0},
                                         {1, 0, 0, 0, 1},
                                         {0, 1, 0, 0, 1},
                                         {0, 0, 1, 0, 1},
                                         {0, 0, 0, 1, 1}}),
                                   BMat({{1, 0, 0, 0, 0},
                                         {0, 1, 0, 0, 0},
                                         {0, 0, 1, 0, 0},
                                         {0, 0, 0, 1, 0},
                                         {0, 0, 0, 0, 0}}),
                                   BMat({{1, 1, 1, 0, 0},
                                         {1, 0, 0, 1, 0},
                                         {0, 1, 0, 1, 0},
                                         {0, 0, 1, 0, 1},
                                         {0, 0, 0, 1, 1}}),
                                   BMat({{1, 1, 1, 0, 0},
                                         {1, 0, 0, 1, 0},
                                         {1, 0, 0, 0, 1},
                                         {0, 1, 0, 1, 0},
                                         {0, 0, 1, 0, 1}}),
                                   BMat({{1, 1, 1, 0, 0},
                                         {1, 0, 0, 1, 1},
                                         {0, 1, 0, 1, 0},
                                         {0, 1, 0, 0, 1},
                                         {0, 0, 1, 1, 0}}),
                                   BMat({{1, 1, 1, 0, 0},
                                         {1, 1, 0, 1, 0},
                                         {1, 0, 0, 0, 1},
                                         {0, 1, 0, 0, 1},
                                         {0, 0, 1, 1, 1}})});
    REQUIRE(T.size() == 33'554'432);
  }

  LIBSEMIGROUPS_TEST_CASE("Konieczny",
                          "013",
                          "regular generated bmat monoid 4 idempotents",
                          "[quick][no-valgrind][bmat8]") {
    auto rg = ReportGuard(false);

    Konieczny S = make<Konieczny>(
        {BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
         BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
         BMat8({{1, 0, 0, 0}, {1, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
         BMat8({{0, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}})});

    REQUIRE(S.contains(S.generator(0)));
    REQUIRE(S.contains(S.generator(1)));
    REQUIRE(S.contains(S.generator(2)));
    REQUIRE(S.contains(S.generator(3)));
    REQUIRE(S.current_size() < 15'000);
    REQUIRE(S.current_number_of_regular_elements() < 10'000);
    REQUIRE(S.current_number_of_idempotents() < 500);
    REQUIRE(S.current_number_of_D_classes() < 100);
    REQUIRE(S.current_number_of_L_classes() < 300);
    REQUIRE(S.current_number_of_R_classes() < 300);
    REQUIRE(S.current_number_of_regular_D_classes() < 50);
    REQUIRE(S.current_number_of_regular_L_classes() < 150);
    REQUIRE(S.current_number_of_regular_R_classes() < 150);

    REQUIRE(S.size() == 63'904);
    REQUIRE(S.number_of_regular_elements() == 40'408);
    REQUIRE(S.number_of_idempotents() == 2'360);

    REQUIRE(S.current_size() == S.size());
    REQUIRE(S.current_number_of_regular_elements()
            == S.number_of_regular_elements());
    REQUIRE(S.current_number_of_idempotents() == S.number_of_idempotents());
    REQUIRE(S.current_number_of_D_classes() == S.number_of_D_classes());
    REQUIRE(S.current_number_of_L_classes() == S.number_of_L_classes());
    REQUIRE(S.current_number_of_R_classes() == S.number_of_R_classes());

    REQUIRE(S.current_number_of_regular_D_classes()
            == S.number_of_regular_D_classes());
    REQUIRE(S.current_number_of_regular_L_classes()
            == S.number_of_regular_L_classes());
    REQUIRE(S.current_number_of_regular_R_classes()
            == S.number_of_regular_R_classes());
  }

  LIBSEMIGROUPS_TEST_CASE("Konieczny",
                          "014",
                          "regular generated bmat monoid 5",
                          "[standard][bmat8]") {
    using BMat = BMatFastest<5>;
    auto rg    = ReportGuard(false);

    Konieczny T = make<Konieczny>({BMat({{0, 1, 0, 0, 0},
                                         {1, 0, 0, 0, 0},
                                         {0, 0, 1, 0, 0},
                                         {0, 0, 0, 1, 0},
                                         {0, 0, 0, 0, 1}}),
                                   BMat({{0, 1, 0, 0, 0},
                                         {0, 0, 1, 0, 0},
                                         {0, 0, 0, 1, 0},
                                         {0, 0, 0, 0, 1},
                                         {1, 0, 0, 0, 0}}),
                                   BMat({{1, 0, 0, 0, 0},
                                         {1, 1, 0, 0, 0},
                                         {0, 0, 1, 0, 0},
                                         {0, 0, 0, 1, 0},
                                         {0, 0, 0, 0, 1}}),
                                   BMat({{0, 0, 0, 0, 0},
                                         {0, 1, 0, 0, 0},
                                         {0, 0, 1, 0, 0},
                                         {0, 0, 0, 1, 0},
                                         {0, 0, 0, 0, 1}})});
    REQUIRE(T.size() == 32'311'832);

    size_t sum = 0;
    std::for_each(T.cbegin_regular_D_classes(),
                  T.cend_regular_D_classes(),
                  [&sum](auto const& x) { sum += x.size(); });
    REQUIRE(sum == 8'683'982);

    sum = 0;
    std::for_each(T.cbegin_regular_D_classes(),
                  T.cend_regular_D_classes(),
                  [&sum](auto const& x) { sum += x.number_of_idempotents(); });
    REQUIRE(sum == 73'023);

    sum = 0;
    std::for_each(
        T.cbegin_regular_D_classes(),
        T.cend_regular_D_classes(),
        [&sum, &T](auto const& x) {
          sum += T.D_class_of_element(x.rep()).number_of_idempotents();
        });
    REQUIRE(sum == 73'023);
    REQUIRE(T.number_of_idempotents() == 73'023);
  }

  LIBSEMIGROUPS_TEST_CASE("Konieczny",
                          "015",
                          "my favourite example",
                          "[quick][finite][no-valgrind][bmat8]") {
    auto rg = ReportGuard(false);

    Konieczny S = make<Konieczny>({BMat8({{0, 1, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 0, 1},
                                          {0, 0, 1, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 1, 0},
                                          {1, 0, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 1, 0, 0, 0},
                                          {0, 1, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 1, 0, 0}}),
                                   BMat8({{0, 0, 1, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 1, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 1, 0},
                                          {0, 1, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 1, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 1, 0, 0},
                                          {0, 0, 1, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 0, 1}}),
                                   BMat8({{0, 0, 0, 1, 0, 0, 0, 0},
                                          {1, 0, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 0, 1},
                                          {0, 0, 1, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 1, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 1, 0},
                                          {0, 0, 1, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 1, 0, 0, 0}}),
                                   BMat8({{0, 0, 0, 1, 0, 0, 0, 0},
                                          {0, 0, 1, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 1, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 1, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 1, 0, 0},
                                          {0, 0, 0, 1, 0, 0, 0, 0},
                                          {1, 0, 0, 0, 0, 0, 0, 0},
                                          {0, 1, 0, 0, 0, 0, 0, 0}}),
                                   BMat8({{0, 0, 0, 0, 1, 0, 0, 0},
                                          {0, 0, 0, 1, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 0, 1},
                                          {0, 0, 0, 0, 0, 0, 0, 1},
                                          {0, 0, 0, 0, 1, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 1, 0, 0},
                                          {1, 0, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 1, 0, 0, 0}}),
                                   BMat8({{0, 0, 0, 0, 0, 1, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 1, 0},
                                          {0, 0, 0, 1, 0, 0, 0, 0},
                                          {1, 0, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 1, 0, 0, 0, 0},
                                          {1, 0, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 1, 0, 0},
                                          {0, 1, 0, 0, 0, 0, 0, 0}}),
                                   BMat8({{0, 0, 0, 0, 0, 0, 1, 0},
                                          {1, 0, 0, 0, 0, 0, 0, 0},
                                          {0, 1, 0, 0, 0, 0, 0, 0},
                                          {0, 1, 0, 0, 0, 0, 0, 0},
                                          {0, 1, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 1, 0},
                                          {0, 0, 0, 1, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 1, 0, 0, 0}}),
                                   BMat8({{0, 0, 0, 0, 0, 0, 0, 1},
                                          {0, 0, 0, 0, 0, 0, 0, 1},
                                          {0, 0, 0, 0, 1, 0, 0, 0},
                                          {1, 0, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 1, 0},
                                          {0, 0, 0, 0, 1, 0, 0, 0},
                                          {0, 1, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 0, 1}})});
    REQUIRE(S.size() == 597'369);
  }

  LIBSEMIGROUPS_TEST_CASE("Konieczny",
                          "016",
                          "another large example",
                          "[quick][no-valgrind][bmat8]") {
    using BMat = BMatFastest<8>;
    auto rg    = ReportGuard(false);

    Konieczny S = make<Konieczny>({BMat({{0, 1, 0, 0, 0, 0, 0, 0},
                                         {0, 0, 0, 0, 0, 1, 0, 0},
                                         {0, 0, 0, 0, 0, 1, 0, 0},
                                         {0, 0, 0, 0, 1, 0, 0, 0},
                                         {0, 0, 1, 0, 0, 0, 0, 0},
                                         {0, 0, 0, 1, 0, 0, 0, 0},
                                         {0, 0, 0, 0, 0, 0, 1, 0},
                                         {0, 0, 0, 0, 1, 0, 0, 0}}),
                                   BMat({{0, 0, 0, 1, 0, 0, 0, 0},
                                         {0, 1, 0, 0, 0, 0, 0, 0},
                                         {1, 0, 0, 0, 0, 0, 0, 0},
                                         {0, 0, 0, 0, 0, 1, 0, 0},
                                         {0, 1, 0, 0, 0, 0, 0, 0},
                                         {0, 0, 0, 0, 0, 0, 0, 1},
                                         {0, 0, 1, 0, 0, 0, 0, 0},
                                         {1, 0, 0, 0, 0, 0, 0, 0}}),
                                   BMat({{0, 0, 0, 1, 0, 0, 0, 0},
                                         {0, 0, 0, 0, 0, 0, 1, 0},
                                         {0, 0, 0, 1, 0, 0, 0, 0},
                                         {0, 0, 0, 0, 0, 0, 1, 0},
                                         {0, 0, 0, 0, 1, 0, 0, 0},
                                         {0, 0, 0, 0, 0, 0, 1, 0},
                                         {0, 0, 0, 0, 0, 1, 0, 0},
                                         {0, 0, 0, 0, 0, 0, 0, 1}}),
                                   BMat({{0, 0, 0, 0, 0, 0, 0, 1},
                                         {0, 0, 0, 0, 0, 0, 0, 1},
                                         {0, 0, 0, 0, 0, 0, 0, 1},
                                         {0, 0, 0, 0, 0, 1, 0, 0},
                                         {1, 0, 0, 0, 0, 0, 0, 0},
                                         {0, 0, 0, 0, 0, 0, 1, 0},
                                         {0, 0, 0, 0, 1, 0, 0, 0},
                                         {0, 0, 0, 0, 0, 0, 0, 1}}),
                                   BMat({{0, 1, 0, 0, 0, 0, 0, 0},
                                         {0, 0, 1, 0, 0, 0, 0, 0},
                                         {0, 0, 0, 0, 0, 1, 0, 0},
                                         {0, 0, 0, 0, 0, 1, 0, 0},
                                         {0, 0, 1, 0, 0, 0, 0, 0},
                                         {0, 1, 0, 0, 0, 0, 0, 0},
                                         {0, 0, 0, 1, 0, 0, 0, 0},
                                         {0, 0, 0, 0, 0, 0, 0, 1}}),
                                   BMat({{1, 0, 0, 0, 0, 0, 0, 0},
                                         {0, 0, 0, 0, 0, 0, 1, 0},
                                         {1, 0, 0, 0, 0, 0, 0, 0},
                                         {0, 0, 0, 0, 0, 0, 1, 0},
                                         {1, 0, 0, 0, 0, 0, 0, 0},
                                         {0, 0, 0, 0, 0, 0, 0, 1},
                                         {0, 0, 0, 0, 0, 1, 0, 0},
                                         {0, 0, 0, 1, 0, 0, 0, 0}}),
                                   BMat({{0, 0, 0, 0, 0, 0, 1, 0},
                                         {1, 0, 0, 0, 0, 0, 0, 0},
                                         {0, 0, 0, 0, 1, 0, 0, 0},
                                         {0, 1, 0, 0, 0, 0, 0, 0},
                                         {0, 0, 1, 0, 0, 0, 0, 0},
                                         {0, 0, 0, 0, 0, 1, 0, 0},
                                         {0, 0, 0, 0, 0, 1, 0, 0},
                                         {0, 0, 0, 0, 1, 0, 0, 0}}),
                                   BMat({{0, 0, 1, 0, 0, 0, 0, 0},
                                         {0, 0, 0, 0, 1, 0, 0, 0},
                                         {0, 0, 0, 0, 0, 0, 0, 1},
                                         {0, 0, 0, 0, 1, 0, 0, 0},
                                         {0, 0, 0, 1, 0, 0, 0, 0},
                                         {1, 0, 0, 0, 0, 0, 0, 0},
                                         {0, 0, 0, 0, 1, 0, 0, 0},
                                         {0, 0, 1, 0, 0, 0, 0, 0}})});
    REQUIRE(S.size() == 201'750);
  }

  LIBSEMIGROUPS_TEST_CASE("Konieczny",
                          "017",
                          "my favourite example transposed",
                          "[quick][no-valgrind][bmat8]") {
    auto rg = ReportGuard(false);

    Konieczny S = make<Konieczny>({BMat8({{0, 0, 0, 0, 1, 0, 0, 0},
                                          {1, 0, 0, 0, 0, 0, 1, 0},
                                          {0, 0, 1, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 1, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 0, 1},
                                          {0, 0, 0, 1, 0, 0, 0, 0},
                                          {0, 1, 0, 0, 0, 0, 0, 0}}),
                                   BMat8({{0, 0, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 1, 0, 0, 0, 0},
                                          {1, 0, 0, 0, 0, 0, 1, 0},
                                          {0, 0, 0, 0, 0, 0, 0, 0},
                                          {0, 1, 0, 0, 1, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 1, 0, 0},
                                          {0, 0, 1, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 0, 1}}),
                                   BMat8({{0, 1, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 1, 0, 0, 1, 0},
                                          {1, 0, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 1, 0, 0, 1},
                                          {0, 0, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 1, 0, 0},
                                          {0, 0, 1, 0, 0, 0, 0, 0}}),
                                   BMat8({{0, 0, 0, 0, 0, 0, 1, 0},
                                          {0, 0, 0, 0, 0, 0, 0, 1},
                                          {0, 1, 0, 0, 0, 0, 0, 0},
                                          {1, 0, 1, 0, 0, 1, 0, 0},
                                          {0, 0, 0, 1, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 1, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 0, 0}}),
                                   BMat8({{0, 0, 0, 0, 0, 0, 1, 0},
                                          {0, 0, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 0, 0},
                                          {0, 1, 0, 0, 0, 0, 0, 0},
                                          {1, 0, 0, 0, 1, 0, 0, 1},
                                          {0, 0, 0, 0, 0, 1, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 1, 1, 0, 0, 0, 0}}),
                                   BMat8({{0, 0, 0, 1, 0, 1, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 0, 1},
                                          {0, 0, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 1, 0, 1, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 0, 0},
                                          {1, 0, 0, 0, 0, 0, 1, 0},
                                          {0, 1, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 0, 0}}),
                                   BMat8({{0, 1, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 1, 1, 1, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 1, 0},
                                          {0, 0, 0, 0, 0, 0, 0, 1},
                                          {0, 0, 0, 0, 0, 0, 0, 0},
                                          {1, 0, 0, 0, 0, 1, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 0, 0}}),
                                   BMat8({{0, 0, 0, 1, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 1, 0},
                                          {0, 0, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 1, 0, 0, 1, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 1, 0, 0, 0},
                                          {1, 1, 0, 0, 0, 0, 0, 1}})});
    REQUIRE(S.size() == 597'369);
  }

  LIBSEMIGROUPS_TEST_CASE("Konieczny",
                          "018",
                          "full bmat monoid 5 with stop in Action",
                          "[standard][bmat8]") {
    using BMat = BMatFastest<5>;
    auto rg    = ReportGuard(false);

    Konieczny T = make<Konieczny>({BMat({{1, 0, 0, 0, 0},
                                         {0, 1, 0, 0, 0},
                                         {0, 0, 1, 0, 0},
                                         {0, 0, 0, 1, 0},
                                         {0, 0, 0, 0, 1}}),
                                   BMat({{0, 1, 0, 0, 0},
                                         {0, 0, 1, 0, 0},
                                         {0, 0, 0, 1, 0},
                                         {0, 0, 0, 0, 1},
                                         {1, 0, 0, 0, 0}}),
                                   BMat({{0, 1, 0, 0, 0},
                                         {1, 0, 0, 0, 0},
                                         {0, 0, 1, 0, 0},
                                         {0, 0, 0, 1, 0},
                                         {0, 0, 0, 0, 1}}),
                                   BMat({{1, 0, 0, 0, 0},
                                         {0, 1, 0, 0, 0},
                                         {0, 0, 1, 0, 0},
                                         {0, 0, 0, 1, 0},
                                         {1, 0, 0, 0, 1}}),
                                   BMat({{1, 1, 0, 0, 0},
                                         {1, 0, 1, 0, 0},
                                         {0, 1, 0, 1, 0},
                                         {0, 0, 1, 1, 0},
                                         {0, 0, 0, 0, 1}}),
                                   BMat({{1, 1, 0, 0, 0},
                                         {1, 0, 1, 0, 0},
                                         {0, 1, 1, 0, 0},
                                         {0, 0, 0, 1, 0},
                                         {0, 0, 0, 0, 1}}),
                                   BMat({{1, 1, 1, 0, 0},
                                         {1, 0, 0, 1, 0},
                                         {0, 1, 0, 1, 0},
                                         {0, 0, 1, 1, 0},
                                         {0, 0, 0, 0, 1}}),
                                   BMat({{1, 1, 0, 0, 0},
                                         {1, 0, 1, 0, 0},
                                         {0, 1, 0, 1, 0},
                                         {0, 0, 1, 0, 1},
                                         {0, 0, 0, 1, 1}}),
                                   BMat({{1, 1, 1, 1, 0},
                                         {1, 0, 0, 0, 1},
                                         {0, 1, 0, 0, 1},
                                         {0, 0, 1, 0, 1},
                                         {0, 0, 0, 1, 1}}),
                                   BMat({{1, 0, 0, 0, 0},
                                         {0, 1, 0, 0, 0},
                                         {0, 0, 1, 0, 0},
                                         {0, 0, 0, 1, 0},
                                         {0, 0, 0, 0, 0}}),
                                   BMat({{1, 1, 1, 0, 0},
                                         {1, 0, 0, 1, 0},
                                         {0, 1, 0, 1, 0},
                                         {0, 0, 1, 0, 1},
                                         {0, 0, 0, 1, 1}}),
                                   BMat({{1, 1, 1, 0, 0},
                                         {1, 0, 0, 1, 0},
                                         {1, 0, 0, 0, 1},
                                         {0, 1, 0, 1, 0},
                                         {0, 0, 1, 0, 1}}),
                                   BMat({{1, 1, 1, 0, 0},
                                         {1, 0, 0, 1, 1},
                                         {0, 1, 0, 1, 0},
                                         {0, 1, 0, 0, 1},
                                         {0, 0, 1, 1, 0}}),
                                   BMat({{1, 1, 1, 0, 0},
                                         {1, 1, 0, 1, 0},
                                         {1, 0, 0, 0, 1},
                                         {0, 1, 0, 0, 1},
                                         {0, 0, 1, 1, 1}})});
    T.run_for(std::chrono::milliseconds(100));
    T.run_for(std::chrono::milliseconds(100));
    T.run_for(std::chrono::milliseconds(100));
    T.run_for(std::chrono::milliseconds(100));
    T.run_for(std::chrono::milliseconds(100));
    T.run();
    T.run_for(std::chrono::milliseconds(100));
    T.run_for(std::chrono::milliseconds(100));
    REQUIRE(T.size() == 33'554'432);
  }

  LIBSEMIGROUPS_TEST_CASE("Konieczny",
                          "019",
                          "regular generated bmat monoid 5 with stops",
                          "[standard][bmat8]") {
    auto rg = ReportGuard(false);

    Konieczny T = make<Konieczny>({BMat8({{0, 1, 0, 0, 0},
                                          {1, 0, 0, 0, 0},
                                          {0, 0, 1, 0, 0},
                                          {0, 0, 0, 1, 0},
                                          {0, 0, 0, 0, 1}}),
                                   BMat8({{0, 1, 0, 0, 0},
                                          {0, 0, 1, 0, 0},
                                          {0, 0, 0, 1, 0},
                                          {0, 0, 0, 0, 1},
                                          {1, 0, 0, 0, 0}}),
                                   BMat8({{1, 0, 0, 0, 0},
                                          {1, 1, 0, 0, 0},
                                          {0, 0, 1, 0, 0},
                                          {0, 0, 0, 1, 0},
                                          {0, 0, 0, 0, 1}}),
                                   BMat8({{0, 0, 0, 0, 0},
                                          {0, 1, 0, 0, 0},
                                          {0, 0, 1, 0, 0},
                                          {0, 0, 0, 1, 0},
                                          {0, 0, 0, 0, 1}})});
    T.run_until([&T]() { return T.current_number_of_D_classes() > 0; });
    REQUIRE(T.current_number_of_D_classes() > 0);

    T.run();
    REQUIRE(T.current_number_of_D_classes() == 704);
    REQUIRE(T.size() == 32'311'832);
    REQUIRE(T.number_of_regular_elements() == 8'683'982);
    REQUIRE(T.number_of_idempotents() == 73'023);
  }

  LIBSEMIGROUPS_TEST_CASE("Konieczny", "020", "exceptions", "[quick][bmat8]") {
    auto rg = ReportGuard(false);
    REQUIRE_THROWS_AS(make<Konieczny>(std::vector<BMat8>()),
                      LibsemigroupsException);

    Konieczny<BMat8> S;
    REQUIRE_THROWS_AS(S.run(), LibsemigroupsException);

    Konieczny KS = make<Konieczny>({BMat8({{0, 1, 0}, {0, 0, 1}, {1, 0, 0}}),
                                    BMat8({{0, 1, 0}, {1, 0, 0}, {0, 0, 1}}),
                                    BMat8({{1, 0, 0}, {1, 1, 0}, {0, 0, 1}}),
                                    BMat8({{1, 1, 0}, {0, 1, 1}, {1, 0, 1}})});
    KS.run();

    REQUIRE(KS.number_of_regular_D_classes() == 9);

    REQUIRE_THROWS_AS(
        KS.add_generators(KS.cbegin_generators(), KS.cend_generators()),
        LibsemigroupsException);
    REQUIRE_THROWS_AS(KS.add_generator(KS.generator(0)),
                      LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("Konieczny",
                          "021",
                          "0-parameter constructor",
                          "[quick][no-valgrind][bmat8]") {
    auto              rg   = ReportGuard(false);
    std::vector const gens = {BMat8({{0, 1, 0, 0, 0, 0, 0, 0},
                                     {0, 0, 0, 0, 0, 1, 0, 0},
                                     {0, 0, 0, 0, 0, 1, 0, 0},
                                     {0, 0, 0, 0, 1, 0, 0, 0},
                                     {0, 0, 1, 0, 0, 0, 0, 0},
                                     {0, 0, 0, 1, 0, 0, 0, 0},
                                     {0, 0, 0, 0, 0, 0, 1, 0},
                                     {0, 0, 0, 0, 1, 0, 0, 0}}),
                              BMat8({{0, 0, 0, 1, 0, 0, 0, 0},
                                     {0, 1, 0, 0, 0, 0, 0, 0},
                                     {1, 0, 0, 0, 0, 0, 0, 0},
                                     {0, 0, 0, 0, 0, 1, 0, 0},
                                     {0, 1, 0, 0, 0, 0, 0, 0},
                                     {0, 0, 0, 0, 0, 0, 0, 1},
                                     {0, 0, 1, 0, 0, 0, 0, 0},
                                     {1, 0, 0, 0, 0, 0, 0, 0}}),
                              BMat8({{0, 0, 0, 1, 0, 0, 0, 0},
                                     {0, 0, 0, 0, 0, 0, 1, 0},
                                     {0, 0, 0, 1, 0, 0, 0, 0},
                                     {0, 0, 0, 0, 0, 0, 1, 0},
                                     {0, 0, 0, 0, 1, 0, 0, 0},
                                     {0, 0, 0, 0, 0, 0, 1, 0},
                                     {0, 0, 0, 0, 0, 1, 0, 0},
                                     {0, 0, 0, 0, 0, 0, 0, 1}}),
                              BMat8({{0, 0, 0, 0, 0, 0, 0, 1},
                                     {0, 0, 0, 0, 0, 0, 0, 1},
                                     {0, 0, 0, 0, 0, 0, 0, 1},
                                     {0, 0, 0, 0, 0, 1, 0, 0},
                                     {1, 0, 0, 0, 0, 0, 0, 0},
                                     {0, 0, 0, 0, 0, 0, 1, 0},
                                     {0, 0, 0, 0, 1, 0, 0, 0},
                                     {0, 0, 0, 0, 0, 0, 0, 1}}),
                              BMat8({{0, 1, 0, 0, 0, 0, 0, 0},
                                     {0, 0, 1, 0, 0, 0, 0, 0},
                                     {0, 0, 0, 0, 0, 1, 0, 0},
                                     {0, 0, 0, 0, 0, 1, 0, 0},
                                     {0, 0, 1, 0, 0, 0, 0, 0},
                                     {0, 1, 0, 0, 0, 0, 0, 0},
                                     {0, 0, 0, 1, 0, 0, 0, 0},
                                     {0, 0, 0, 0, 0, 0, 0, 1}}),
                              BMat8({{1, 0, 0, 0, 0, 0, 0, 0},
                                     {0, 0, 0, 0, 0, 0, 1, 0},
                                     {1, 0, 0, 0, 0, 0, 0, 0},
                                     {0, 0, 0, 0, 0, 0, 1, 0},
                                     {1, 0, 0, 0, 0, 0, 0, 0},
                                     {0, 0, 0, 0, 0, 0, 0, 1},
                                     {0, 0, 0, 0, 0, 1, 0, 0},
                                     {0, 0, 0, 1, 0, 0, 0, 0}}),
                              BMat8({{0, 0, 0, 0, 0, 0, 1, 0},
                                     {1, 0, 0, 0, 0, 0, 0, 0},
                                     {0, 0, 0, 0, 1, 0, 0, 0},
                                     {0, 1, 0, 0, 0, 0, 0, 0},
                                     {0, 0, 1, 0, 0, 0, 0, 0},
                                     {0, 0, 0, 0, 0, 1, 0, 0},
                                     {0, 0, 0, 0, 0, 1, 0, 0},
                                     {0, 0, 0, 0, 1, 0, 0, 0}}),
                              BMat8({{0, 0, 1, 0, 0, 0, 0, 0},
                                     {0, 0, 0, 0, 1, 0, 0, 0},
                                     {0, 0, 0, 0, 0, 0, 0, 1},
                                     {0, 0, 0, 0, 1, 0, 0, 0},
                                     {0, 0, 0, 1, 0, 0, 0, 0},
                                     {1, 0, 0, 0, 0, 0, 0, 0},
                                     {0, 0, 0, 0, 1, 0, 0, 0},
                                     {0, 0, 1, 0, 0, 0, 0, 0}})};

    Konieczny<BMat8> S;
    for (auto x : gens) {
      S.add_generator(x);
    }
    konieczny::add_generators(S, gens);

    REQUIRE(S.size() == 201'750);
  }
}  // namespace libsemigroups
