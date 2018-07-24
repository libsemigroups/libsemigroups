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

#include "bmat8.h"
#include "catch.hpp"
#include "semigroup.h"

#define SEMIGROUPS_REPORT false

using namespace libsemigroups;

#if (!(defined(LIBSEMIGROUPS_DENSEHASHMAP)) && LIBSEMIGROUPS_SIZEOF_VOID_P == 8)

TEST_CASE("Semigroup of BMats 01: regular boolean mat monoid 4 using BMat8",
          "[quick][semigroup][bmat][finite][01]") {
  std::vector<BMat8> gens
      = {BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
         BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
         BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {1, 0, 0, 1}}),
         BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}})};

  Semigroup<BMat8> S(gens);
  REPORTER.set_report(SEMIGROUPS_REPORT);

  REQUIRE(S.current_max_word_length() == 1);
  REQUIRE(!S.is_done());
  REQUIRE(!S.is_begun());
  REQUIRE(S.current_position(S.generator(0) * S.generator(3)) == UNDEFINED);
  REQUIRE(S.current_position(BMat8({{1, 0, 0, 1, 1},
                                    {0, 1, 0, 0, 1},
                                    {1, 0, 1, 0, 1},
                                    {0, 0, 1, 0, 1},
                                    {0, 0, 0, 0, 0}}))
          == UNDEFINED);
  REQUIRE(S.current_size() == 4);
  REQUIRE(S.current_nrrules() == 0);
  REQUIRE(S.length_const(0) == 1);
  REQUIRE(S.length_non_const(5) == 2);

  REQUIRE(S.size() == 63904);
  REQUIRE(S.nridempotents() == 2360);
  REQUIRE(S.word_to_pos({0, 1, 2, 0, 1, 2}) == 378);
  REQUIRE(S.word_to_element({0, 1, 2, 0, 1, 2})
          == BMat8({{1, 0, 0, 1}, {0, 1, 0, 0}, {1, 0, 1, 0}, {0, 0, 1, 0}}));
  REQUIRE(S.current_max_word_length() == 21);
  REQUIRE(S.degree() == 8);
  REQUIRE(S.nrgens() == 4);
  REQUIRE(S.generator(0)
          == BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}));
  REQUIRE(S.generator(1)
          == BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}));
  REQUIRE(S.generator(2)
          == BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {1, 0, 0, 1}}));
  REQUIRE(S.generator(3)
          == BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}}));
  REQUIRE(S.is_done());
  REQUIRE(S.is_begun());
  REQUIRE(S.current_position(S.generator(0) * S.generator(3)) == 7);
  REQUIRE(S.current_position(BMat8({{1, 0, 0, 1, 1},
                                    {0, 1, 0, 0, 1},
                                    {1, 0, 1, 0, 1},
                                    {0, 0, 1, 0, 1},
                                    {0, 0, 0, 0, 0}}))
          == UNDEFINED);
  REQUIRE(S.current_nrrules() == 13716);
  REQUIRE(S.prefix(0) == UNDEFINED);
  REQUIRE(S.suffix(0) == UNDEFINED);
  REQUIRE(S.first_letter(0) == 0);
  REQUIRE(S.final_letter(0) == 0);
  REQUIRE(S.batch_size() == 8192);
  REQUIRE(S.length_const(0) == 1);
  REQUIRE(S.length_const(7) == 2);
  REQUIRE(S.length_const(63903) == 21);
  REQUIRE(S.length_non_const(7) == 2);
  REQUIRE(S.length_non_const(63903) == 21);
  REQUIRE(S.product_by_reduction(0, 3) == 7);
  REQUIRE(S.fast_product(0, 3) == 7);
  REQUIRE(S.letter_to_pos(0) == 0);
  REQUIRE(S.letter_to_pos(1) == 1);
  REQUIRE(S.letter_to_pos(2) == 2);
  REQUIRE(S.letter_to_pos(3) == 3);
  REQUIRE(!S.is_idempotent(0));
  REQUIRE(S.is_idempotent(3));
  REQUIRE(!S.is_idempotent(7));
  REQUIRE(S.nrrules() == 13716);
  REQUIRE(S.test_membership(S.generator(1)));
  REQUIRE(!S.test_membership(BMat8({{1, 0, 0, 1, 1},
                                    {0, 1, 0, 0, 1},
                                    {1, 0, 1, 0, 1},
                                    {0, 0, 1, 0, 1},
                                    {0, 0, 0, 0, 0}})));
  REQUIRE(S.position(S.generator(1)) == 1);
  REQUIRE(S.position(S.generator(0) * S.generator(3)) == 7);
  REQUIRE(S.position(BMat8({{1, 0, 0, 1, 1},
                            {0, 1, 0, 0, 1},
                            {1, 0, 1, 0, 1},
                            {0, 0, 1, 0, 1},
                            {0, 0, 0, 0, 0}}))
          == UNDEFINED);

  REQUIRE(S.sorted_position(BMat8({{1, 0, 0, 1, 1},
                                   {0, 1, 0, 0, 1},
                                   {1, 0, 1, 0, 1},
                                   {0, 0, 1, 0, 1},
                                   {0, 0, 0, 0, 0}}))
          == UNDEFINED);
  REQUIRE(S.sorted_position(S.generator(0)) == 18185);
  REQUIRE(S.sorted_position(S.generator(3)) == 33066);
  REQUIRE(S.sorted_position(S.generator(0) * S.generator(3)) == 18184);
  REQUIRE(S.position_to_sorted_position(0) == 18185);
  REQUIRE(S.position_to_sorted_position(3) == 33066);
  REQUIRE(S.position_to_sorted_position(7) == 18184);

  REQUIRE(S.at(7) == S.generator(0) * S.generator(3));
  REQUIRE(S[7] == S[0] * S[3]);

  REQUIRE(S.sorted_at(18185) == S.at(0));
  REQUIRE(S.sorted_at(33066) == S.at(3));
  REQUIRE(S.sorted_at(18184) == S.generator(0) * S.generator(3));

  REQUIRE(S.right(0, 3) == 7);
  RecVec<size_t>* right = S.right_cayley_graph_copy();

  for (size_t i = 0; i < 63904; ++i) {
    for (size_t j = 0; j < 4; ++j) {
      REQUIRE(right->get(i, j) == S.right(i, j));
    }
  }
  delete right;
  REQUIRE(S.right(0, 3) == 7);

  REQUIRE(S.left(0, 3) == 7);
  RecVec<size_t>* left = S.left_cayley_graph_copy();

  for (size_t i = 0; i < 63904; ++i) {
    for (size_t j = 0; j < 4; ++j) {
      REQUIRE(left->get(i, j) == S.left(i, j));
    }
  }
  delete left;
  REQUIRE(S.left(0, 3) == 7);

  word_type w;
  S.minimal_factorisation(w, 378);
  REQUIRE(w == word_type({0, 1, 2, 0, 1, 2}));
  REQUIRE(S.length_const(378) == 6);

  REQUIRE(S.minimal_factorisation(S.at(378)) == word_type({0, 1, 2, 0, 1, 2}));

  REQUIRE_THROWS_AS(S.minimal_factorisation(BMat8({{1, 0, 0, 1, 1},
                                                   {0, 1, 0, 0, 1},
                                                   {1, 0, 1, 0, 1},
                                                   {0, 0, 1, 0, 1},
                                                   {0, 0, 0, 0, 0}})),
                    LibsemigroupsException);

  REQUIRE_THROWS_AS(S.minimal_factorisation(1000000), LibsemigroupsException);

  w.clear();
  S.factorisation(w, 378);
  REQUIRE(w == word_type({0, 1, 2, 0, 1, 2}));
  REQUIRE(S.length_const(378) == 6);

  REQUIRE(S.factorisation(S.at(378)) == word_type({0, 1, 2, 0, 1, 2}));

  REQUIRE_THROWS_AS(S.factorisation(BMat8({{1, 0, 0, 1, 1},
                                           {0, 1, 0, 0, 1},
                                           {1, 0, 1, 0, 1},
                                           {0, 0, 1, 0, 1},
                                           {0, 0, 0, 0, 0}})),
                    LibsemigroupsException);

  REQUIRE_THROWS_AS(S.factorisation(1000000), LibsemigroupsException);

  S.next_relation(w);
  REQUIRE(w == std::vector<size_t>({2, 2, 2}));
  S.next_relation(w);
  REQUIRE(w == std::vector<size_t>({3, 0, 7}));
  S.next_relation(w);
  REQUIRE(w == std::vector<size_t>({3, 2, 3}));

  size_t pos = 0;
  for (auto it = S.cbegin(); it < S.cend(); ++it) {
    REQUIRE(S.position(*it) == pos);
    pos++;
  }
  REQUIRE(pos == S.size());

  // Copy - after enumerate
  Semigroup<BMat8> T(S);
  REQUIRE(T.size() == 63904);
  REQUIRE(T.nridempotents() == 2360);
  REQUIRE(T.word_to_pos({0, 1, 2, 0, 1, 2}) == 378);
  REQUIRE(T.word_to_element({0, 1, 2, 0, 1, 2})
          == BMat8({{1, 0, 0, 1}, {0, 1, 0, 0}, {1, 0, 1, 0}, {0, 0, 1, 0}}));
  REQUIRE(T.current_max_word_length() == 21);
  REQUIRE(T.degree() == 8);
  REQUIRE(T.nrgens() == 4);
  REQUIRE(T.generator(0)
          == BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}));
  REQUIRE(T.generator(1)
          == BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}));
  REQUIRE(T.generator(2)
          == BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {1, 0, 0, 1}}));
  REQUIRE(T.generator(3)
          == BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}}));
  REQUIRE(T.is_done());
  REQUIRE(T.is_begun());
}
#endif

TEST_CASE("Semigroup of BMats 02: Exception: zero generators given",
          "[quick][finite][semigroup][bmat][02]") {
  std::vector<BMat8> gens;

  REQUIRE_THROWS_AS(Semigroup<BMat8>(gens), LibsemigroupsException);
}

TEST_CASE("Semigroup of BMats 03: Exception: word_to_element",
          "[quick][finite][semigroup][bmat][03]") {
  std::vector<BMat8> gens
      = {BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
         BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
         BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {1, 0, 0, 1}}),
         BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}})};
  Semigroup<BMat8> S(gens);

  REQUIRE_THROWS_AS(S.word_to_element({}), LibsemigroupsException);
  REQUIRE_NOTHROW(S.word_to_element({0}));
  REQUIRE_NOTHROW(S.word_to_element({0, 3, 0, 3, 1}));
  REQUIRE_THROWS_AS(S.word_to_element({0, 1, 0, 4}), LibsemigroupsException);
}

TEST_CASE("Semigroup of BMats 04: Exception: prefix",
          "[quick][finite][semigroup][bmat][04]") {
  std::vector<BMat8> gens
      = {BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
         BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
         BMat8({{1, 1, 0, 0}, {1, 0, 1, 0}, {0, 1, 1, 1}, {0, 1, 1, 1}})};
  Semigroup<BMat8> S(gens);

  for (size_t i = 0; i < S.size(); ++i) {
    REQUIRE_NOTHROW(S.prefix(i));
    REQUIRE_THROWS_AS(S.prefix(i + S.size()), LibsemigroupsException);
  }
}

TEST_CASE("Semigroup of BMats 05: Exception: suffix",
          "[quick][finite][semigroup][bmat][085]") {
  std::vector<BMat8> gens
      = {BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
         BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
         BMat8({{1, 1, 0, 0}, {1, 0, 1, 0}, {0, 1, 1, 1}, {0, 1, 1, 1}})};
  Semigroup<BMat8> S(gens);
}

TEST_CASE("Semigroup of BMats 06: Exception: first_letter",
          "[quick][finite][semigroup][bmat][086]") {
  std::vector<BMat8> gens
      = {BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
         BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
         BMat8({{1, 1, 0, 0}, {1, 0, 1, 0}, {0, 1, 1, 1}, {0, 1, 1, 1}})};
  Semigroup<BMat8> S(gens);

  for (size_t i = 0; i < S.size(); ++i) {
    REQUIRE_NOTHROW(S.first_letter(i));
    REQUIRE_THROWS_AS(S.first_letter(i + S.size()), LibsemigroupsException);
  }
}

TEST_CASE("Semigroup of BMats 07: Exception: final_letter",
          "[quick][finite][semigroup][bmat][087]") {
  std::vector<BMat8> gens
      = {BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
         BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
         BMat8({{1, 1, 0, 0}, {1, 0, 1, 0}, {0, 1, 1, 1}, {0, 1, 1, 1}})};
  Semigroup<BMat8> S(gens);
}

TEST_CASE("Semigroup of BMats 08: Exception: length_const",
          "[quick][finite][semigroup][bmat][088]") {
  std::vector<BMat8> gens
      = {BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
         BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
         BMat8({{1, 1, 0, 0}, {1, 0, 1, 0}, {0, 1, 1, 1}, {0, 1, 1, 1}})};
  Semigroup<BMat8> S(gens);

  for (size_t i = 0; i < S.size(); ++i) {
    REQUIRE_NOTHROW(S.length_const(i));
    REQUIRE_THROWS_AS(S.length_const(i + S.size()), LibsemigroupsException);
  }
}

TEST_CASE("Semigroup of BMats 09: Exception: product_by_reduction",
          "[quick][finite][semigroup][bmat][089]") {
  std::vector<BMat8> gens
      = {BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
         BMat8({{1, 1, 0, 0}, {1, 0, 1, 0}, {0, 1, 1, 1}, {0, 1, 1, 1}})};
  Semigroup<BMat8> S(gens);

  for (size_t i = 1; i < S.size(); ++i) {
    for (size_t j = 1; j < S.size(); ++j) {
      REQUIRE_NOTHROW(S.product_by_reduction(i, j));
      REQUIRE_THROWS_AS(S.product_by_reduction(i + S.size(), j),
                        LibsemigroupsException);
      REQUIRE_THROWS_AS(S.product_by_reduction(i, j + S.size()),
                        LibsemigroupsException);
      REQUIRE_THROWS_AS(S.product_by_reduction(i + S.size(), j + S.size()),
                        LibsemigroupsException);
    }
  }
}

TEST_CASE("Semigroup of BMats 10: Exception: fast_product",
          "[quick][finite][semigroup][bmat][10]") {
  std::vector<BMat8> gens
      = {BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
         BMat8({{1, 1, 0, 0}, {1, 0, 1, 0}, {0, 1, 1, 1}, {0, 1, 1, 1}})};
  Semigroup<BMat8> S(gens);

  for (size_t i = 1; i < S.size(); ++i) {
    for (size_t j = 1; j < S.size(); ++j) {
      REQUIRE_NOTHROW(S.fast_product(i, j));
      REQUIRE_THROWS_AS(S.fast_product(i + S.size(), j),
                        LibsemigroupsException);
      REQUIRE_THROWS_AS(S.fast_product(i, j + S.size()),
                        LibsemigroupsException);
      REQUIRE_THROWS_AS(S.fast_product(i + S.size(), j + S.size()),
                        LibsemigroupsException);
    }
  }
}

TEST_CASE("Semigroup of BMats 11: Exception: is_idempotent",
          "[quick][finite][semigroup][bmat][11]") {
  std::vector<BMat8> gens
      = {BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
         BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
         BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {1, 0, 0, 1}}),
         BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}})};
  Semigroup<BMat8> S(gens);

  // S has size 63904
  for (size_t i = 0; i < 63904; ++i) {
    REQUIRE_NOTHROW(S.is_idempotent(i));
  }
  for (size_t i = 0; i < 20; ++i) {
    REQUIRE_THROWS_AS(S.is_idempotent(63904 + i), LibsemigroupsException);
  }
}
