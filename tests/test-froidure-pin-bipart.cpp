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

#include "catch.hpp"      // for REQUIRE
#include "test-main.hpp"  // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/bipart.hpp"        // for Bipartition
#include "libsemigroups/froidure-pin.hpp"  // for FroidurePin

namespace libsemigroups {
  // Forward declaration
  struct LibsemigroupsException;

  constexpr bool REPORT = false;

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<Bipartition>",
                          "000",
                          "small example 1",
                          "[quick][froidure-pin][bipartition][bipart]") {
    auto                     rg = ReportGuard(REPORT);
    std::vector<Bipartition> gens
        = {Bipartition(
               {0, 1, 2, 1, 0, 2, 1, 0, 2, 2, 0, 0, 2, 0, 3, 4, 4, 1, 3, 0}),
           Bipartition(
               {0, 1, 1, 1, 1, 2, 3, 2, 4, 5, 5, 2, 4, 2, 1, 1, 1, 2, 3, 2}),
           Bipartition(
               {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0})};

    FroidurePin<Bipartition> S(gens);

    S.reserve(10);

    REQUIRE(S.size() == 10);
    REQUIRE(S.number_of_idempotents() == 6);

    size_t pos = 0;
    for (auto it = S.cbegin(); it < S.cend(); ++it) {
      REQUIRE(S.position(*it) == pos);
      pos++;
    }

    S.add_generator(Bipartition(
        {0, 1, 2, 1, 1, 3, 1, 4, 2, 3, 1, 0, 3, 2, 3, 5, 4, 1, 3, 0}));

    REQUIRE(S.size() == 21);
    S.closure({Bipartition(
        {0, 1, 2, 1, 1, 3, 1, 4, 2, 3, 1, 0, 3, 2, 3, 5, 4, 1, 3, 0})});
    REQUIRE(S.size() == 21);
    REQUIRE(
        S.minimal_factorisation(Bipartition({0, 1, 2, 1, 0, 2, 1, 0, 2, 2,
                                             0, 0, 2, 0, 3, 4, 4, 1, 3, 0})
                                * Bipartition({0, 1, 2, 1, 1, 3, 1, 4, 2, 3,
                                               1, 0, 3, 2, 3, 5, 4, 1, 3, 0})
                                * Bipartition({0, 1, 1, 1, 1, 2, 3, 2, 4, 5,
                                               5, 2, 4, 2, 1, 1, 1, 2, 3, 2}))
        == word_type({0, 3, 1}));
    REQUIRE(S.minimal_factorisation(11) == word_type({0, 3}));
    REQUIRE(S.at(11)
            == Bipartition(
                   {0, 1, 2, 1, 0, 2, 1, 0, 2, 2, 0, 0, 2, 0, 3, 4, 4, 1, 3, 0})
                   * Bipartition({0, 1, 2, 1, 1, 3, 1, 4, 2, 3,
                                  1, 0, 3, 2, 3, 5, 4, 1, 3, 0}));
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

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<Bipartition>",
                          "001",
                          "default constructed",
                          "[quick][froidure-pin][bipartition][bipart]") {
    auto rg = ReportGuard(REPORT);

    FroidurePin<Bipartition> S;
    S.add_generator(Bipartition(
        {0, 1, 2, 1, 0, 2, 1, 0, 2, 2, 0, 0, 2, 0, 3, 4, 4, 1, 3, 0}));
    S.add_generator(Bipartition(
        {0, 1, 1, 1, 1, 2, 3, 2, 4, 5, 5, 2, 4, 2, 1, 1, 1, 2, 3, 2}));
    S.add_generator(Bipartition(
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));

    S.reserve(10);

    REQUIRE(S.size() == 10);
    REQUIRE(S.number_of_idempotents() == 6);

    size_t pos = 0;
    for (auto it = S.cbegin(); it < S.cend(); ++it) {
      REQUIRE(S.position(*it) == pos);
      pos++;
    }

    S.add_generator(Bipartition(
        {0, 1, 2, 1, 1, 3, 1, 4, 2, 3, 1, 0, 3, 2, 3, 5, 4, 1, 3, 0}));

    REQUIRE(S.size() == 21);
    S.closure({Bipartition(
        {0, 1, 2, 1, 1, 3, 1, 4, 2, 3, 1, 0, 3, 2, 3, 5, 4, 1, 3, 0})});
    REQUIRE(S.size() == 21);
    REQUIRE(
        S.minimal_factorisation(Bipartition({0, 1, 2, 1, 0, 2, 1, 0, 2, 2,
                                             0, 0, 2, 0, 3, 4, 4, 1, 3, 0})
                                * Bipartition({0, 1, 2, 1, 1, 3, 1, 4, 2, 3,
                                               1, 0, 3, 2, 3, 5, 4, 1, 3, 0})
                                * Bipartition({0, 1, 1, 1, 1, 2, 3, 2, 4, 5,
                                               5, 2, 4, 2, 1, 1, 1, 2, 3, 2}))
        == word_type({0, 3, 1}));
    REQUIRE(S.minimal_factorisation(11) == word_type({0, 3}));
    REQUIRE(S.at(11)
            == Bipartition(
                   {0, 1, 2, 1, 0, 2, 1, 0, 2, 2, 0, 0, 2, 0, 3, 4, 4, 1, 3, 0})
                   * Bipartition({0, 1, 2, 1, 1, 3, 1, 4, 2, 3,
                                  1, 0, 3, 2, 3, 5, 4, 1, 3, 0}));
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

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<Bipartition>",
                          "002",
                          "small example 2",
                          "[quick][froidure-pin][element]") {
    auto                     rg = ReportGuard(REPORT);
    FroidurePin<Bipartition> S;
    S.add_generator(Bipartition(
        {0, 1, 2, 1, 0, 2, 1, 0, 2, 2, 0, 0, 2, 0, 3, 4, 4, 1, 3, 0}));
    S.add_generator(Bipartition(
        {0, 1, 1, 1, 1, 2, 3, 2, 4, 5, 5, 2, 4, 2, 1, 1, 1, 2, 3, 2}));
    S.add_generator(Bipartition(
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));

    REQUIRE(S.size() == 10);
    REQUIRE(S.degree() == 10);
    REQUIRE(S.number_of_idempotents() == 6);
    REQUIRE(S.number_of_generators() == 3);
    REQUIRE(S.number_of_rules() == 14);

    REQUIRE(S[0] == S.generator(0));
    REQUIRE(S[1] == S.generator(1));
    REQUIRE(S[2] == S.generator(2));

    REQUIRE(S.position(S.generator(0)) == 0);
    REQUIRE(S.contains(S.generator(0)));

    REQUIRE(S.position(S.generator(1)) == 1);
    REQUIRE(S.contains(S.generator(1)));

    REQUIRE(S.position(S.generator(2)) == 2);
    REQUIRE(S.contains(S.generator(2)));

    Bipartition y({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0});
    REQUIRE(S.position(y) == 2);
    REQUIRE(S.contains(y));
    y.product_inplace(S.generator(0), S.generator(1));
    REQUIRE(S.position(y) == 4);
    REQUIRE(S.contains(y));
    y.product_inplace(S.generator(1), S.generator(2));
    REQUIRE(S.position(y) == 7);
    REQUIRE(S.contains(y));
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<Bipartition>",
                          "003",
                          "number of idempotents",
                          "[extreme][froidure-pin][element]") {
    auto                     rg = ReportGuard();
    FroidurePin<Bipartition> S;
    S.add_generator(Bipartition({0, 1, 2, 3, 4, 5, 5, 0, 1, 2, 3, 4}));
    S.add_generator(Bipartition({0, 1, 2, 3, 4, 5, 1, 0, 2, 3, 4, 5}));
    S.add_generator(Bipartition({0, 1, 2, 3, 4, 5, 6, 1, 2, 3, 4, 5}));
    S.add_generator(Bipartition({0, 0, 1, 2, 3, 4, 0, 0, 1, 2, 3, 4}));
    S.reserve(4213597);
    REQUIRE(S.size() == 4213597);
    REQUIRE(S.number_of_idempotents() == 541254);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin<Bipartition>",
                          "004",
                          "exception: is_idempotent",
                          "[quick][froidure-pin][element]") {
    FroidurePin<Bipartition> S;
    S.add_generator(Bipartition(
        {0, 1, 2, 1, 0, 2, 1, 0, 2, 2, 0, 0, 2, 0, 3, 4, 4, 1, 3, 0}));
    S.add_generator(Bipartition(
        {0, 1, 1, 1, 1, 2, 3, 2, 4, 5, 5, 2, 4, 2, 1, 1, 1, 2, 3, 2}));
    S.add_generator(Bipartition(
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));

    // T has size 10
    for (size_t i = 0; i < 10; ++i) {
      REQUIRE_NOTHROW(S.is_idempotent(i));
    }
    for (size_t i = 0; i < 20; ++i) {
      REQUIRE_THROWS_AS(S.is_idempotent(10 + i), LibsemigroupsException);
    }
  }

}  // namespace libsemigroups
