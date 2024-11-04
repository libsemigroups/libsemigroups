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

#include "bmat-data.hpp"          // for clark_gens
#include "catch_amalgamated.hpp"  // for REQUIRE
#include "test-main.hpp"          // for LIBSEMIGROUPS_TEST_CASE_V3

#include "libsemigroups/froidure-pin.hpp"  // for FroidurePin<>::element_index_type
#include "libsemigroups/matrix.hpp"        // for BMat

#include "libsemigroups/detail/report.hpp"  // for ReportGuard

namespace libsemigroups {
  using namespace literals;  // for operator""_w

  // Forward declaration
  struct LibsemigroupsException;

  bool constexpr REPORT = false;

  ////////////////////////////////////////////////////////////////////////
  // Test cases - BMat
  ////////////////////////////////////////////////////////////////////////

  TEMPLATE_TEST_CASE("FroidurePin: small example 1",
                     "[005][quick][froidure-pin][bmat]",
                     BMat<4>,
                     BMat<>) {
    auto                  rg = ReportGuard(REPORT);
    FroidurePin<TestType> S;
    S.add_generator(
        TestType({{0, 1, 0, 1}, {1, 0, 0, 0}, {0, 1, 1, 1}, {0, 1, 1, 0}}));
    S.add_generator(
        TestType({{0, 1, 1, 1}, {1, 1, 0, 0}, {0, 0, 0, 0}, {1, 1, 1, 1}}));
    S.add_generator(
        TestType({{0, 1, 1, 0}, {0, 1, 1, 0}, {0, 1, 1, 1}, {1, 1, 1, 1}}));

    S.reserve(26);

    REQUIRE(S.size() == 26);
    REQUIRE(S.number_of_idempotents() == 4);

    size_t pos = 0;
    std::for_each(S.cbegin(), S.cend(), [&S, &pos](auto const& x) {
      REQUIRE(S.position(x) == pos++);
    });

    froidure_pin::add_generators(
        S,
        {TestType({{1, 0, 0, 1}, {0, 1, 0, 1}, {0, 0, 1, 1}, {1, 1, 1, 0}})});

    REQUIRE(S.size() == 29);
    froidure_pin::closure(
        S,
        {TestType({{1, 0, 0, 1}, {0, 1, 0, 1}, {0, 0, 1, 1}, {1, 1, 1, 0}})});
    REQUIRE(S.size() == 29);
    REQUIRE(
        froidure_pin::minimal_factorisation(S, S.generator(3) * S.generator(0))
        == 30_w);
    REQUIRE(froidure_pin::minimal_factorisation(S, 28) == 30_w);
    REQUIRE(S.at(28) == S.generator(3) * S.generator(0));
    REQUIRE_THROWS_AS(froidure_pin::minimal_factorisation(S, 1'000'000'000),
                      LibsemigroupsException);
    pos = 0;
    std::for_each(
        S.cbegin_idempotents(), S.cend_idempotents(), [&pos](auto const& x) {
          REQUIRE(x * x == x);
          ++pos;
        });
    REQUIRE(pos == S.number_of_idempotents());
    REQUIRE(std::is_sorted(S.cbegin_sorted(), S.cend_sorted()));
  }

  TEMPLATE_TEST_CASE("FroidurePin: regular bmat monoid 4",
                     "[007][quick][froidure-pin][bmat][no-valgrind]",
                     BMat<4>,
                     BMat<>) {
    auto                  rg = ReportGuard(REPORT);
    FroidurePin<TestType> S;
    S.add_generator(
        TestType({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}));
    S.add_generator(
        TestType({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}));
    S.add_generator(
        TestType({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {1, 0, 0, 1}}));
    S.add_generator(
        TestType({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}}));
    REQUIRE(S.size() == 63'904);
    REQUIRE(S.number_of_idempotents() == 2'360);
  }

  TEMPLATE_TEST_CASE("FroidurePin: small example 2",
                     "[009][quick][froidure-pin][bmat]",
                     BMat<3>,
                     BMat<>) {
    auto rg = ReportGuard(REPORT);

    FroidurePin<TestType> S;
    S.add_generator(TestType({{1, 0, 1}, {0, 1, 0}, {0, 1, 0}}));
    S.add_generator(TestType({{0, 0, 0}, {0, 0, 0}, {0, 0, 0}}));
    S.add_generator(TestType({{0, 0, 0}, {0, 0, 0}, {0, 0, 0}}));

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

    TestType y({{0, 0, 0}, {0, 0, 0}, {0, 0, 0}});
    y.product_inplace_no_checks(S.generator(0), S.generator(0));
    REQUIRE(S.position(y) == 2);
    REQUIRE(S.contains(y));
    REQUIRE(S.fast_product(1, 2) == 1);
  }

  TEMPLATE_TEST_CASE("FroidurePin: small example 3",
                     "[011][quick][froidure-pin][bmat]",
                     BMat<4>,
                     BMat<>) {
    auto S = to_froidure_pin(
        {TestType({{1, 0, 0, 0}, {0, 0, 1, 0}, {1, 0, 0, 1}, {0, 1, 0, 0}}),
         TestType({{1, 0, 0, 1}, {1, 0, 0, 1}, {1, 1, 1, 1}, {0, 1, 1, 0}}),
         TestType({{1, 0, 1, 0}, {1, 0, 1, 1}, {0, 0, 1, 1}, {0, 1, 0, 1}}),
         TestType({{0, 0, 0, 0}, {0, 1, 0, 1}, {1, 1, 1, 0}, {1, 0, 0, 1}}),
         TestType({{0, 0, 0, 1}, {0, 0, 1, 0}, {1, 0, 0, 1}, {1, 1, 0, 0}})});
    REQUIRE(S.size() == 415);
  }

  TEMPLATE_TEST_CASE("FroidurePin: Clark generators",
                     "[013][extreme][froidure-pin][bmat]",
                     BMat<40>,
                     BMat<>) {
    auto                  rg = ReportGuard(true);
    FroidurePin<TestType> S;
    for (auto const& x : konieczny_data::clark_gens) {
      S.add_generator(TestType(x));
    }
    REQUIRE(S.generator(0).number_of_rows() == 40);
    REQUIRE(S.size() == 248'017);
  }
}  // namespace libsemigroups
