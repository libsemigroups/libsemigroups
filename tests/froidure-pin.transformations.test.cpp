//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2018 James D. Mitchell
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

#include "catch.hpp"
#include "element.hpp"
#include "froidure-pin.hpp"

#define SEMIGROUPS_REPORT false

using namespace libsemigroups;

TEST_CASE("FroidurePin of Transformations 01",
          "[standard][semigroup][transformation][finite][01]") {
  std::vector<Transformation<uint_fast8_t>> gens
      = {Transformation<uint_fast8_t>({1, 7, 2, 6, 0, 4, 1, 5}),
         Transformation<uint_fast8_t>({2, 4, 6, 1, 4, 5, 2, 7}),
         Transformation<uint_fast8_t>({3, 0, 7, 2, 4, 6, 2, 4}),
         Transformation<uint_fast8_t>({3, 2, 3, 4, 5, 3, 0, 1}),
         Transformation<uint_fast8_t>({4, 3, 7, 7, 4, 5, 0, 4}),
         Transformation<uint_fast8_t>({5, 6, 3, 0, 3, 0, 5, 1}),
         Transformation<uint_fast8_t>({6, 0, 1, 1, 1, 6, 3, 4}),
         Transformation<uint_fast8_t>({7, 7, 4, 0, 6, 4, 1, 7})};

  FroidurePin<Transformation<uint_fast8_t>> S(gens);
  S.reserve(597369);
  REPORTER.set_report(SEMIGROUPS_REPORT);

  REQUIRE(S.size() == 597369);
  REQUIRE(S.nr_idempotents() == 8194);
  size_t pos = 0;
  for (auto it = S.cbegin(); it < S.cend(); ++it) {
    REQUIRE(S.position(*it) == pos);
    pos++;
  }

  S.add_generators({Transformation<uint_fast8_t>({7, 1, 2, 6, 7, 4, 1, 5})});
  REQUIRE(S.size() == 826713);
  S.closure({Transformation<uint_fast8_t>({7, 1, 2, 6, 7, 4, 1, 5})});
  REQUIRE(S.size() == 826713);
  REQUIRE(S.minimal_factorisation(
              Transformation<uint_fast8_t>({7, 1, 2, 6, 7, 4, 1, 5})
              * Transformation<uint_fast8_t>({2, 4, 6, 1, 4, 5, 2, 7}))
          == word_type({8, 1}));
  REQUIRE(S.minimal_factorisation(10) == word_type({0, 2}));
  REQUIRE(S.at(10) == Transformation<uint_fast8_t>({0, 4, 7, 2, 3, 4, 0, 6}));
  REQUIRE_THROWS_AS(S.minimal_factorisation(1000000000),
                    LibsemigroupsException);
  pos = 0;
  for (auto it = S.cbegin_idempotents(); it < S.cend_idempotents(); ++it) {
    REQUIRE(*it * *it == *it);
    pos++;
  }
  REQUIRE(pos == S.nr_idempotents());
  for (auto it = S.cbegin_sorted() + 1; it < S.cend_sorted(); ++it) {
    REQUIRE(*(it - 1) < *it);
  }
}

TEST_CASE("FroidurePin of Transformations 02: Exception: zero generators given",
          "[quick][finite][semigroup][transformation][02]") {
  std::vector<Transformation<uint_fast8_t>> gens1;

  REQUIRE_THROWS_AS(FroidurePin<Transformation<uint_fast8_t>>(gens1),
                    LibsemigroupsException);
}

TEST_CASE("FroidurePin of Transformations 03: Exception: generators of "
          "different degrees",
          "[quick][finite][semigroup][transformation][03]") {
  std::vector<Transformation<uint_fast8_t>> gens
      = {Transformation<uint_fast8_t>({1, 7, 2, 6, 0, 0, 1, 2}),
         Transformation<uint_fast8_t>({2, 4, 6, 1, 4, 5, 2, 7, 3})};

  REQUIRE_THROWS_AS(FroidurePin<Transformation<uint_fast8_t>>(gens),
                    LibsemigroupsException);
}

TEST_CASE("FroidurePin of Transformations 04: Exception: word_to_pos",
          "[quick][finite][semigroup][transformation][04]") {
  std::vector<Transformation<u_int16_t>> gens
      = {Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}),
         Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
         Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};
  FroidurePin<Transformation<u_int16_t>> U(gens);

  REQUIRE_THROWS_AS(U.word_to_pos({}), LibsemigroupsException);
  REQUIRE_NOTHROW(U.word_to_pos({0, 0, 1, 2}));
  REQUIRE_THROWS_AS(U.word_to_pos({5}), LibsemigroupsException);
}

TEST_CASE("FroidurePin of Transformations 05: Exception: word_to_element",
          "[quick][finite][semigroup][transformation][05]") {
  std::vector<Transformation<u_int16_t>> gens
      = {Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}),
         Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
         Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};
  FroidurePin<Transformation<u_int16_t>> U(gens);

  REQUIRE_THROWS_AS(U.word_to_element({}), LibsemigroupsException);
  REQUIRE_THROWS_AS(U.word_to_element({5}), LibsemigroupsException);

  Transformation<u_int16_t> u = U.word_to_element({0, 0, 1, 2});
  REQUIRE(u
          == Transformation<u_int16_t>({0, 1, 2, 3, 4, 5})
                 * Transformation<u_int16_t>({0, 1, 2, 3, 4, 5})
                 * Transformation<u_int16_t>({1, 0, 2, 3, 4, 5})
                 * Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}));
}

TEST_CASE("FroidurePin of Transformations 16: Exception: gens",
          "[quick][finite][semigroup][transformation][05]") {
  for (size_t i = 1; i < 20; ++i) {
    std::vector<Transformation<size_t>> gens;

    for (size_t j = 0; j < i; ++j) {
      std::vector<size_t> trans;
      for (size_t k = 0; k < i; ++k) {
        trans.push_back((k + j) % i);
      }
      gens.push_back(Transformation<size_t>(trans));
    }
    FroidurePin<Transformation<size_t>> S(gens);

    for (size_t j = 0; j < i; ++j) {
      REQUIRE_NOTHROW(S.generator(j));
    }
    REQUIRE_THROWS_AS(S.generator(i), LibsemigroupsException);
  }
}

TEST_CASE("FroidurePin of Transformations 06: Exception: prefix",
          "[quick][finite][semigroup][transformation][06]") {
  std::vector<Transformation<u_int16_t>> gens
      = {Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}),
         Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};
  FroidurePin<Transformation<u_int16_t>> U(gens);

  for (size_t i = 0; i < U.size(); ++i) {
    REQUIRE_NOTHROW(U.prefix(i));
    REQUIRE_THROWS_AS(U.prefix(i + U.size()), LibsemigroupsException);
  }
}

TEST_CASE("FroidurePin of Transformations 07: Exception: suffix",
          "[quick][finite][semigroup][transformation][07]") {
  std::vector<Transformation<u_int16_t>> gens
      = {Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}),
         Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
         Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};
  FroidurePin<Transformation<u_int16_t>> U(gens);

  for (size_t i = 0; i < U.size(); ++i) {
    REQUIRE_NOTHROW(U.suffix(i));
    REQUIRE_THROWS_AS(U.suffix(i + U.size()), LibsemigroupsException);
  }
}

TEST_CASE("FroidurePin of Transformations 08: Exception: first_letter",
          "[quick][finite][semigroup][transformation][08]") {
  std::vector<Transformation<u_int16_t>> gens
      = {Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
         Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};
  FroidurePin<Transformation<u_int16_t>> U(gens);

  for (size_t i = 0; i < U.size(); ++i) {
    REQUIRE_NOTHROW(U.first_letter(i));
    REQUIRE_THROWS_AS(U.first_letter(i + U.size()), LibsemigroupsException);
  }
}

TEST_CASE("FroidurePin of Transformations 09: Exception: final_letter",
          "[quick][finite][semigroup][transformation][09]") {
  std::vector<Transformation<u_int16_t>> gens
      = {Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
         Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};
  FroidurePin<Transformation<u_int16_t>> U(gens);

  for (size_t i = 0; i < U.size(); ++i) {
    REQUIRE_NOTHROW(U.final_letter(i));
    REQUIRE_THROWS_AS(U.final_letter(i + U.size()), LibsemigroupsException);
  }
}

TEST_CASE("FroidurePin of Transformations 10: Exception: length_const",
          "[quick][finite][semigroup][transformation][10]") {
  std::vector<Transformation<u_int16_t>> gens
      = {Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
         Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};
  FroidurePin<Transformation<u_int16_t>> U(gens);

  for (size_t i = 0; i < U.size(); ++i) {
    REQUIRE_NOTHROW(U.length_const(i));
    REQUIRE_THROWS_AS(U.length_const(i + U.size()), LibsemigroupsException);
  }
}

TEST_CASE("FroidurePin of Transformations 11: Exception: product_by_reduction",
          "[quick][finite][semigroup][transformation][11]") {
  std::vector<Transformation<u_int16_t>> gens
      = {Transformation<u_int16_t>({0, 1, 2, 3}),
         Transformation<u_int16_t>({3, 1, 1, 2})};
  FroidurePin<Transformation<u_int16_t>> U(gens);

  for (size_t i = 0; i < U.size(); ++i) {
    for (size_t j = 0; j < U.size(); ++j) {
      REQUIRE_NOTHROW(U.product_by_reduction(i, j));
      REQUIRE_THROWS_AS(U.product_by_reduction(i + U.size(), j),
                        LibsemigroupsException);
      REQUIRE_THROWS_AS(U.product_by_reduction(i, j + U.size()),
                        LibsemigroupsException);
      REQUIRE_THROWS_AS(U.product_by_reduction(i + U.size(), j + U.size()),
                        LibsemigroupsException);
    }
  }
}

TEST_CASE("FroidurePin of Transformations 12: Exception: fast_product",
          "[quick][finite][semigroup][transformation][12]") {
  std::vector<Transformation<u_int16_t>> gens
      = {Transformation<u_int16_t>({0, 1, 2, 3}),
         Transformation<u_int16_t>({3, 1, 1, 2})};
  FroidurePin<Transformation<u_int16_t>> U(gens);

  for (size_t i = 0; i < U.size(); ++i) {
    for (size_t j = 0; j < U.size(); ++j) {
      REQUIRE_NOTHROW(U.fast_product(i, j));
      REQUIRE_THROWS_AS(U.fast_product(i + U.size(), j),
                        LibsemigroupsException);
      REQUIRE_THROWS_AS(U.fast_product(i, j + U.size()),
                        LibsemigroupsException);
      REQUIRE_THROWS_AS(U.fast_product(i + U.size(), j + U.size()),
                        LibsemigroupsException);
    }
  }
}

TEST_CASE("FroidurePin of Transformations 13: Exception: letter_to_pos",
          "[quick][finite][semigroup][transformation][13]") {
  for (size_t i = 1; i < 20; ++i) {
    std::vector<Transformation<size_t>> gens;

    for (size_t j = 0; j < i; ++j) {
      std::vector<size_t> trans;
      for (size_t k = 0; k < i; ++k) {
        trans.push_back((k + j) % i);
      }
      gens.push_back(Transformation<size_t>(trans));
    }
    FroidurePin<Transformation<size_t>> S(gens);

    for (size_t j = 0; j < i; ++j) {
      REQUIRE_NOTHROW(S.letter_to_pos(j));
    }
    REQUIRE_THROWS_AS(S.letter_to_pos(i), LibsemigroupsException);
  }
}

TEST_CASE("FroidurePin of Transformations 14: Exception: is_idempotent",
          "[quick][finite][semigroup][transformation][14]") {
  std::vector<Transformation<u_int16_t>> gens
      = {Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         Transformation<u_int16_t>({5, 1, 3, 3, 2, 5}),
         Transformation<u_int16_t>({2, 1, 2, 3, 4, 4}),
         Transformation<u_int16_t>({5, 5, 2, 1, 1, 2})};
  FroidurePin<Transformation<u_int16_t>> S(gens);

  // S has size 441
  for (size_t i = 0; i < 441; ++i) {
    REQUIRE_NOTHROW(S.is_idempotent(i));
  }
  for (size_t i = 0; i < 20; ++i) {
    REQUIRE_THROWS_AS(S.is_idempotent(441 + i), LibsemigroupsException);
  }
}

TEST_CASE("FroidurePin of Transformations 15: Exception: add_generators",
          "[quick][finite][semigroup][transformation][15]") {
  std::vector<Transformation<uint_fast8_t>> gens
      = {Transformation<uint_fast8_t>({1, 7, 2, 6, 0, 0, 1, 2}),
         Transformation<uint_fast8_t>({2, 4, 6, 1, 4, 5, 2, 7})};
  FroidurePin<Transformation<uint_fast8_t>> T(gens);

  std::vector<Transformation<uint_fast8_t>> additional_gens_1
      = {Transformation<uint_fast8_t>({1, 2, 2, 2, 1, 1, 3, 4}),
         Transformation<uint_fast8_t>({1, 2, 1, 3, 1, 4, 1, 5})};
  std::vector<Transformation<uint_fast8_t>> additional_gens_2
      = {Transformation<uint_fast8_t>({1, 2, 2, 2, 1, 1, 3, 4}),
         Transformation<uint_fast8_t>({1, 2, 1, 3, 1, 4, 1, 5, 1})};

  REQUIRE_NOTHROW(T.add_generators(additional_gens_1));
  REQUIRE_THROWS_AS(T.add_generators(additional_gens_2),
                    LibsemigroupsException);
}
