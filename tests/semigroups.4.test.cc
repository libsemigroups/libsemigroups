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

#include "../src/bmat8.h"
#include "../src/semigroups.h"
#include "catch.hpp"
#include <iostream>

#define SEMIGROUPS_REPORT false

using namespace libsemigroups;

template <class TElementType>
void delete_gens(std::vector<TElementType>& gens) {
  for (auto& x : gens) {
    delete x;
  }
}

#if (!defined(LIBSEMIGROUPS_HAVE_DENSEHASHMAP)    \
     || !defined(LIBSEMIGROUPS_USE_DENSEHASHMAP)) \
    && LIBSEMIGROUPS_SIZEOF_VOID_P == 8

TEST_CASE("Semigroup 72: regular boolean mat monoid 4 using BMat8",
          "[quick][semigroup][finite][72]") {
  std::vector<BMat8> gens
      = {BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
         BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
         BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {1, 0, 0, 1}}),
         BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}})};

  Semigroup<BMat8> S(gens);
  S.set_report(SEMIGROUPS_REPORT);

  REQUIRE(S.current_max_word_length() == 1);
  REQUIRE(!S.is_done());
  REQUIRE(!S.is_begun());
  REQUIRE(S.current_position(S.gens(0) * S.gens(3))
          == Semigroup<BMat8>::UNDEFINED);
  REQUIRE(S.current_position(BMat8({{1, 0, 0, 1, 1},
                                    {0, 1, 0, 0, 1},
                                    {1, 0, 1, 0, 1},
                                    {0, 0, 1, 0, 1},
                                    {0, 0, 0, 0, 0}}))
          == Semigroup<BMat8>::UNDEFINED);
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
  REQUIRE(S.degree() == 0);
  REQUIRE(S.nrgens() == 4);
  REQUIRE(S.gens(0)
          == BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}));
  REQUIRE(S.gens(1)
          == BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}));
  REQUIRE(S.gens(2)
          == BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {1, 0, 0, 1}}));
  REQUIRE(S.gens(3)
          == BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}}));
  REQUIRE(S.is_done());
  REQUIRE(S.is_begun());
  REQUIRE(S.current_position(S.gens(0) * S.gens(3)) == 7);
  REQUIRE(S.current_position(BMat8({{1, 0, 0, 1, 1},
                                    {0, 1, 0, 0, 1},
                                    {1, 0, 1, 0, 1},
                                    {0, 0, 1, 0, 1},
                                    {0, 0, 0, 0, 0}}))
          == Semigroup<BMat8>::UNDEFINED);
  REQUIRE(S.current_nrrules() == 13716);
  REQUIRE(S.prefix(0) == Semigroup<BMat8>::UNDEFINED);
  REQUIRE(S.suffix(0) == Semigroup<BMat8>::UNDEFINED);
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
  REQUIRE(S.test_membership(S.gens(1)));
  REQUIRE(!S.test_membership(BMat8({{1, 0, 0, 1, 1},
                                    {0, 1, 0, 0, 1},
                                    {1, 0, 1, 0, 1},
                                    {0, 0, 1, 0, 1},
                                    {0, 0, 0, 0, 0}})));
  REQUIRE(S.position(S.gens(1)) == 1);
  REQUIRE(S.position(S.gens(0) * S.gens(3)) == 7);
  REQUIRE(S.position(BMat8({{1, 0, 0, 1, 1},
                            {0, 1, 0, 0, 1},
                            {1, 0, 1, 0, 1},
                            {0, 0, 1, 0, 1},
                            {0, 0, 0, 0, 0}}))
          == Semigroup<BMat8>::UNDEFINED);

  REQUIRE(S.sorted_position(BMat8({{1, 0, 0, 1, 1},
                                   {0, 1, 0, 0, 1},
                                   {1, 0, 1, 0, 1},
                                   {0, 0, 1, 0, 1},
                                   {0, 0, 0, 0, 0}}))
          == Semigroup<BMat8>::UNDEFINED);
  REQUIRE(S.sorted_position(S.gens(0)) == 18185);
  REQUIRE(S.sorted_position(S.gens(3)) == 33066);
  REQUIRE(S.sorted_position(S.gens(0) * S.gens(3)) == 18184);
  REQUIRE(S.position_to_sorted_position(0) == 18185);
  REQUIRE(S.position_to_sorted_position(3) == 33066);
  REQUIRE(S.position_to_sorted_position(7) == 18184);

  REQUIRE(S.at(7) == S.gens(0) * S.gens(3));
  REQUIRE(S[7] == S[0] * S[3]);

  REQUIRE(S.sorted_at(18185) == S.at(0));
  REQUIRE(S.sorted_at(33066) == S.at(3));
  REQUIRE(S.sorted_at(18184) == S.gens(0) * S.gens(3));

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

  word_t w;
  S.minimal_factorisation(w, 378);
  REQUIRE(w == word_t({0, 1, 2, 0, 1, 2}));
  REQUIRE(S.length_const(378) == 6);

  REQUIRE(S.minimal_factorisation(S.at(378)) == word_t({0, 1, 2, 0, 1, 2}));

  REQUIRE_THROWS_AS(S.minimal_factorisation(BMat8({{1, 0, 0, 1, 1},
                                                   {0, 1, 0, 0, 1},
                                                   {1, 0, 1, 0, 1},
                                                   {0, 0, 1, 0, 1},
                                                   {0, 0, 0, 0, 0}})),
                    LibsemigroupsException);

  REQUIRE_THROWS_AS(S.minimal_factorisation(1000000), LibsemigroupsException);

  w.clear();
  S.factorisation(w, 378);
  REQUIRE(w == word_t({0, 1, 2, 0, 1, 2}));
  REQUIRE(S.length_const(378) == 6);

  REQUIRE(S.factorisation(S.at(378)) == word_t({0, 1, 2, 0, 1, 2}));

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
  REQUIRE(T.degree() == 0);
  REQUIRE(T.nrgens() == 4);
  REQUIRE(T.gens(0)
          == BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}));
  REQUIRE(T.gens(1)
          == BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}));
  REQUIRE(T.gens(2)
          == BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {1, 0, 0, 1}}));
  REQUIRE(T.gens(3)
          == BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}}));
  REQUIRE(T.is_done());
  REQUIRE(T.is_begun());
}

TEST_CASE("Semigroup 73: regular boolean mat monoid 4 using BooleanMat",
          "[quick][semigroup][finite][73]") {
  std::vector<Element*> gens = {
      new BooleanMat({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
      new BooleanMat({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
      new BooleanMat({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {1, 0, 0, 1}}),
      new BooleanMat({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}})};
  Semigroup<> S(gens);
  S.set_report(SEMIGROUPS_REPORT);
  REQUIRE(S.size() == 63904);
  REQUIRE(S.nridempotents() == 2360);
  delete_gens(gens);
}

TEST_CASE("Semigroup 74: regular boolean mat monoid 5 using BMat8",
          "[extreme][semigroup][finite][74]") {
  Semigroup<BMat8> S({BMat8({{0, 1, 0, 0, 0},
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
                             {0, 1, 0, 0, 0},
                             {0, 0, 1, 0, 0},
                             {0, 0, 0, 1, 0},
                             {1, 0, 0, 0, 1}}),
                      BMat8({{1, 0, 0, 0, 0},
                             {0, 1, 0, 0, 0},
                             {0, 0, 1, 0, 0},
                             {0, 0, 0, 1, 0},
                             {0, 0, 0, 0, 0}})});
  S.set_report(true);
  S.reserve(40000000);
  REQUIRE(S.size() == 32311832);
  REQUIRE(S.nridempotents() == 73023);
  S.set_report(false);
}

TEST_CASE("Semigroup 75: regular boolean mat monoid 5 using BooleanMat",
          "[extreme][semigroup][finite][75]") {
  std::vector<Element*> gens = {new BooleanMat({{0, 1, 0, 0, 0},
                                                {1, 0, 0, 0, 0},
                                                {0, 0, 1, 0, 0},
                                                {0, 0, 0, 1, 0},
                                                {0, 0, 0, 0, 1}}),
                                new BooleanMat({{0, 1, 0, 0, 0},
                                                {0, 0, 1, 0, 0},
                                                {0, 0, 0, 1, 0},
                                                {0, 0, 0, 0, 1},
                                                {1, 0, 0, 0, 0}}),
                                new BooleanMat({{1, 0, 0, 0, 0},
                                                {0, 1, 0, 0, 0},
                                                {0, 0, 1, 0, 0},
                                                {0, 0, 0, 1, 0},
                                                {1, 0, 0, 0, 1}}),
                                new BooleanMat({{1, 0, 0, 0, 0},
                                                {0, 1, 0, 0, 0},
                                                {0, 0, 1, 0, 0},
                                                {0, 0, 0, 1, 0},
                                                {0, 0, 0, 0, 0}})};
  Semigroup<>           S(gens);
  S.set_report(true);
  S.reserve(40000000);
  REQUIRE(S.size() == 32311832);
  REQUIRE(S.nridempotents() == 73023);
  S.set_report(false);
  delete_gens(gens);
}

TEST_CASE("Semigroup 76: add_generators BMat8",
          "[quick][semigroup][finite][76]") {
  std::vector<BMat8> gens
      = {BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
         BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
         BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {1, 0, 0, 1}}),
         BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}})};

  Semigroup<BMat8> S({gens[0]});
  REQUIRE(S.size() == 2);

  S.add_generators({gens[0]});
  REQUIRE(S.size() == 2);

  S.add_generators({});
  REQUIRE(S.size() == 2);

  S.add_generators({gens[1]});
  REQUIRE(S.current_size() == 6);
  REQUIRE(!S.is_done());
  S.enumerate(10);
  REQUIRE(S.current_size() == 24);
  REQUIRE(S.is_done());
  REQUIRE(S.size() == 24);

  S.add_generators({gens[2]});
  REQUIRE(S.current_size() == 330);
  REQUIRE(!S.is_done());

  S.add_generators({gens[3]});
  REQUIRE(S.current_size() == 864);
  S.enumerate(1000);
  REQUIRE(S.current_size() == 9056);
  REQUIRE(!S.is_done());
  REQUIRE(S.size() == 63904);
}

TEST_CASE("Semigroup 77: iterators BMat8", "[quick][semigroup][finite][77]") {
  std::vector<BMat8> gens
      = {BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
         BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
         BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {1, 0, 0, 1}}),
         BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}})};
  Semigroup<BMat8> S(gens);
  S.set_report(SEMIGROUPS_REPORT);

  // Calling cbegin/cend_sorted fully enumerates the semigroup
  { auto it = S.cbegin_sorted(); }
  REQUIRE(S.is_done());

  size_t pos = 0;
  for (auto it = S.cbegin_sorted(); it < S.cend_sorted(); it++) {
    REQUIRE(S.sorted_position(*it) == pos);
    REQUIRE(S.position_to_sorted_position(S.position(*it)) == pos);
    pos++;
  }
  REQUIRE(pos == S.size());

  pos = 0;
  for (auto it = S.cbegin_sorted(); it < S.cend_sorted(); ++it) {
    REQUIRE(S.sorted_position(*it) == pos);
    REQUIRE(S.position_to_sorted_position(S.position(*it)) == pos);
    pos++;
  }
  REQUIRE(pos == S.size());

  pos = S.size();
  for (auto it = S.crbegin_sorted(); it < S.crend_sorted(); it++) {
    pos--;
    REQUIRE(S.sorted_position(*it) == pos);
    REQUIRE(S.position_to_sorted_position(S.position(*it)) == pos);
  }
  REQUIRE(pos == 0);

  pos = S.size();
  for (auto it = S.crbegin_sorted(); it < S.crend_sorted(); ++it) {
    pos--;
    REQUIRE(S.sorted_position(*it) == pos);
    REQUIRE(S.position_to_sorted_position(S.position(*it)) == pos);
  }
  REQUIRE(pos == 0);
}
#endif

TEST_CASE("Semigroup 78: non-pointer, non-trivial element type",
          "[standard][semigroup][finite][78]") {
  std::vector<Transformation<uint_fast8_t>> gens
      = {Transformation<uint_fast8_t>({1, 7, 2, 6, 0, 4, 1, 5}),
         Transformation<uint_fast8_t>({2, 4, 6, 1, 4, 5, 2, 7}),
         Transformation<uint_fast8_t>({3, 0, 7, 2, 4, 6, 2, 4}),
         Transformation<uint_fast8_t>({3, 2, 3, 4, 5, 3, 0, 1}),
         Transformation<uint_fast8_t>({4, 3, 7, 7, 4, 5, 0, 4}),
         Transformation<uint_fast8_t>({5, 6, 3, 0, 3, 0, 5, 1}),
         Transformation<uint_fast8_t>({6, 0, 1, 1, 1, 6, 3, 4}),
         Transformation<uint_fast8_t>({7, 7, 4, 0, 6, 4, 1, 7})};

  Semigroup<Transformation<uint_fast8_t>> S(gens);
  S.reserve(597369);
  S.set_report(SEMIGROUPS_REPORT);

  REQUIRE(S.size() == 597369);
  REQUIRE(S.nridempotents() == 8194);
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
          == word_t({8, 1}));
  REQUIRE(S.minimal_factorisation(10) == word_t({0, 2}));
  REQUIRE(S.at(10) == Transformation<uint_fast8_t>({0, 4, 7, 2, 3, 4, 0, 6}));
  REQUIRE_THROWS_AS(S.minimal_factorisation(1000000000),
                    LibsemigroupsException);
  pos = 0;
  for (auto it = S.cbegin_idempotents(); it < S.cend_idempotents(); ++it) {
    REQUIRE(*it * *it == *it);
    pos++;
  }
  REQUIRE(pos == S.nridempotents());
  for (auto it = S.cbegin_sorted() + 1; it < S.cend_sorted(); ++it) {
    REQUIRE(*(it - 1) < *it);
  }
}

TEST_CASE("Semigroup 79: Exception: zero generators given",
          "[quick][finite][semigroup][79]") {
  std::vector<Transformation<uint_fast8_t>> gens1;
  std::vector<BMat8>                        gens2;
  std::vector<Element*>                     gens3;

  REQUIRE_THROWS_AS(Semigroup<Transformation<uint_fast8_t>>(gens1),
                    LibsemigroupsException);
  REQUIRE_THROWS_AS(Semigroup<BMat8>(gens2), LibsemigroupsException);

  REQUIRE_THROWS_AS(Semigroup<Element*>(gens3), LibsemigroupsException);
}

TEST_CASE("Semigroup 80: Exception: generators of different degrees",
          "[quick][finite][semigroup][80]") {
  std::vector<Element*> gens1
      = {new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5, 5})};
  std::vector<Transformation<uint_fast8_t>> gens2
      = {Transformation<uint_fast8_t>({1, 7, 2, 6, 0, 0, 1, 2}),
         Transformation<uint_fast8_t>({2, 4, 6, 1, 4, 5, 2, 7, 3})};
  std::vector<Element*> gens3
      = {new PartialPerm<u_int16_t>(
             {0, 1, 2, 3, 5, 6, 9}, {9, 7, 3, 5, 4, 2, 1}, 10),
         new PartialPerm<u_int16_t>({4, 5, 0}, {10, 0, 1}, 11)};

  REQUIRE_THROWS_AS(Semigroup<>(gens1), LibsemigroupsException);
  REQUIRE_THROWS_AS(Semigroup<Transformation<uint_fast8_t>>(gens2),
                    LibsemigroupsException);
  REQUIRE_THROWS_AS(Semigroup<>(gens3), LibsemigroupsException);

  delete_gens(gens1);
  delete_gens(gens3);
}

TEST_CASE("Semigroup 81: Exception: word_to_pos",
          "[quick][finite][semigroup][81]") {
  std::vector<BMat8> gens
      = {BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
         BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
         BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {1, 0, 0, 1}}),
         BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}})};
  Semigroup<BMat8> S(gens);

  REQUIRE_THROWS_AS(S.word_to_pos({}), LibsemigroupsException);
  REQUIRE_NOTHROW(S.word_to_pos({0}));
  REQUIRE_NOTHROW(S.word_to_pos({0, 3, 0, 3, 1}));
  REQUIRE_THROWS_AS(S.word_to_pos({0, 1, 0, 4}), LibsemigroupsException);

  Semiring<int64_t>*    sr = new Integers();
  std::vector<Element*> gens2
      = {new MatrixOverSemiring<int64_t>({{0, 0}, {0, 1}}, sr),
         new MatrixOverSemiring<int64_t>({{0, 1}, {-1, 0}}, sr)};
  Semigroup<> T(gens2);
  delete_gens(gens2);

  REQUIRE_THROWS_AS(T.word_to_pos({}), LibsemigroupsException);
  REQUIRE_NOTHROW(T.word_to_pos({0, 0, 1, 1}));
  REQUIRE_THROWS_AS(T.word_to_pos({0, 0, 1, 2}), LibsemigroupsException);

  std::vector<Element*> gens3
      = {new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}),
         new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};
  Semigroup<> U(gens3);
  delete_gens(gens3);

  REQUIRE_THROWS_AS(U.word_to_pos({}), LibsemigroupsException);
  REQUIRE_NOTHROW(U.word_to_pos({0, 0, 1, 2}));
  REQUIRE_THROWS_AS(U.word_to_pos({5}), LibsemigroupsException);
}

TEST_CASE("Semigroup 82: Exception: word_to_element",
          "[quick][finite][semigroup][82]") {
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

  Semiring<int64_t>*    sr = new Integers();
  std::vector<Element*> gens2
      = {new MatrixOverSemiring<int64_t>({{0, 0}, {0, 1}}, sr),
         new MatrixOverSemiring<int64_t>({{0, 1}, {-1, 0}}, sr)};
  Semigroup<> T(gens2);
  delete_gens(gens2);

  REQUIRE_THROWS_AS(T.word_to_element({}), LibsemigroupsException);
  REQUIRE_NOTHROW(T.word_to_element({0, 0, 1, 1}));
  REQUIRE_THROWS_AS(T.word_to_element({0, 0, 1, 2}), LibsemigroupsException);

  std::vector<Element*> gens3
      = {new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}),
         new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};
  Semigroup<> U(gens3);
  delete_gens(gens3);

  REQUIRE_THROWS_AS(U.word_to_element({}), LibsemigroupsException);
  REQUIRE_NOTHROW(U.word_to_element({0, 0, 1, 2}));
  REQUIRE_THROWS_AS(U.word_to_element({5}), LibsemigroupsException);
}

TEST_CASE("Semigroup 83: Exception: gens", "[quick][finite][semigroup][83]") {
  for (size_t i = 1; i < 20; ++i) {
    std::vector<Element*> gens;

    for (size_t j = 0; j < i; ++j) {
      std::vector<size_t> trans;
      for (size_t k = 0; k < i; ++k) {
        trans.push_back((k + j) % i);
      }
      gens.push_back(new Transformation<size_t>(trans));
    }
    Semigroup<> S(gens);
    delete_gens(gens);

    for (size_t j = 0; j < i; ++j) {
      REQUIRE_NOTHROW(S.gens(j));
    }
    REQUIRE_THROWS_AS(S.gens(i), LibsemigroupsException);
  }
}

TEST_CASE("Semigroups 84: Exception: prefix",
          "[quick][finite][semigroup][84]") {
  std::vector<BMat8> gens
      = {BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
         BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
         BMat8({{1, 1, 0, 0}, {1, 0, 1, 0}, {0, 1, 1, 1}, {0, 1, 1, 1}})};
  Semigroup<BMat8> S(gens);

  for (size_t i = 0; i < S.size(); ++i) {
    REQUIRE_NOTHROW(S.prefix(i));
    REQUIRE_THROWS_AS(S.prefix(i + S.size()), LibsemigroupsException);
  }

  Semiring<int64_t>*    sr = new Integers();
  std::vector<Element*> gens2
      = {new MatrixOverSemiring<int64_t>({{0, 0}, {0, 1}}, sr),
         new MatrixOverSemiring<int64_t>({{0, 1}, {-1, 0}}, sr)};
  Semigroup<> T(gens2);
  delete_gens(gens2);

  for (size_t i = 0; i < T.size(); ++i) {
    REQUIRE_NOTHROW(T.prefix(i));
    REQUIRE_THROWS_AS(T.prefix(i + T.size()), LibsemigroupsException);
  }

  std::vector<Transformation<u_int16_t>> gens3
      = {Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}),
         Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};
  Semigroup<Transformation<u_int16_t>> U(gens3);

  for (size_t i = 0; i < U.size(); ++i) {
    REQUIRE_NOTHROW(U.prefix(i));
    REQUIRE_THROWS_AS(U.prefix(i + U.size()), LibsemigroupsException);
  }
  delete sr;
}

TEST_CASE("Semigroups 85: Exception: suffix",
          "[quick][finite][semigroup][85]") {
  std::vector<BMat8> gens
      = {BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
         BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
         BMat8({{1, 1, 0, 0}, {1, 0, 1, 0}, {0, 1, 1, 1}, {0, 1, 1, 1}})};
  Semigroup<BMat8> S(gens);

  for (size_t i = 0; i < S.size(); ++i) {
    REQUIRE_NOTHROW(S.suffix(i));
    REQUIRE_THROWS_AS(S.suffix(i + S.size()), LibsemigroupsException);
  }

  Semiring<int64_t>*    sr = new Integers();
  std::vector<Element*> gens2
      = {new MatrixOverSemiring<int64_t>({{0, 0}, {0, 1}}, sr),
         new MatrixOverSemiring<int64_t>({{0, 1}, {-1, 0}}, sr)};
  Semigroup<> T(gens2);
  delete_gens(gens2);

  for (size_t i = 0; i < T.size(); ++i) {
    REQUIRE_NOTHROW(T.suffix(i));
    REQUIRE_THROWS_AS(T.suffix(i + T.size()), LibsemigroupsException);
  }

  std::vector<Transformation<u_int16_t>> gens3
      = {Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}),
         Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
         Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};
  Semigroup<Transformation<u_int16_t>> U(gens3);

  for (size_t i = 0; i < U.size(); ++i) {
    REQUIRE_NOTHROW(U.suffix(i));
    REQUIRE_THROWS_AS(U.suffix(i + U.size()), LibsemigroupsException);
  }
}

TEST_CASE("Semigroups 86: Exception: first_letter",
          "[quick][finite][semigroup][86]") {
  std::vector<BMat8> gens
      = {BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
         BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
         BMat8({{1, 1, 0, 0}, {1, 0, 1, 0}, {0, 1, 1, 1}, {0, 1, 1, 1}})};
  Semigroup<BMat8> S(gens);

  for (size_t i = 0; i < S.size(); ++i) {
    REQUIRE_NOTHROW(S.first_letter(i));
    REQUIRE_THROWS_AS(S.first_letter(i + S.size()), LibsemigroupsException);
  }

  Semiring<int64_t>*    sr = new Integers();
  std::vector<Element*> gens2
      = {new MatrixOverSemiring<int64_t>({{0, 0}, {0, 1}}, sr),
         new MatrixOverSemiring<int64_t>({{0, 1}, {-1, 0}}, sr)};
  Semigroup<> T(gens2);
  delete_gens(gens2);

  for (size_t i = 0; i < T.size(); ++i) {
    REQUIRE_NOTHROW(T.first_letter(i));
    REQUIRE_THROWS_AS(T.first_letter(i + T.size()), LibsemigroupsException);
  }

  std::vector<Transformation<u_int16_t>> gens3
      = {Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
         Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};
  Semigroup<Transformation<u_int16_t>> U(gens3);

  for (size_t i = 0; i < U.size(); ++i) {
    REQUIRE_NOTHROW(U.first_letter(i));
    REQUIRE_THROWS_AS(U.first_letter(i + U.size()), LibsemigroupsException);
  }
}

TEST_CASE("Semigroups 87: Exception: final_letter",
          "[quick][finite][semigroup][87]") {
  std::vector<BMat8> gens
      = {BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
         BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
         BMat8({{1, 1, 0, 0}, {1, 0, 1, 0}, {0, 1, 1, 1}, {0, 1, 1, 1}})};
  Semigroup<BMat8> S(gens);

  for (size_t i = 0; i < S.size(); ++i) {
    REQUIRE_NOTHROW(S.final_letter(i));
    REQUIRE_THROWS_AS(S.final_letter(i + S.size()), LibsemigroupsException);
  }

  Semiring<int64_t>*    sr = new Integers();
  std::vector<Element*> gens2
      = {new MatrixOverSemiring<int64_t>({{0, 0}, {0, 1}}, sr),
         new MatrixOverSemiring<int64_t>({{0, 1}, {-1, 0}}, sr)};
  Semigroup<> T(gens2);
  delete_gens(gens2);

  for (size_t i = 0; i < T.size(); ++i) {
    REQUIRE_NOTHROW(T.final_letter(i));
    REQUIRE_THROWS_AS(T.final_letter(i + T.size()), LibsemigroupsException);
  }

  std::vector<Transformation<u_int16_t>> gens3
      = {Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
         Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};
  Semigroup<Transformation<u_int16_t>> U(gens3);

  for (size_t i = 0; i < U.size(); ++i) {
    REQUIRE_NOTHROW(U.final_letter(i));
    REQUIRE_THROWS_AS(U.final_letter(i + U.size()), LibsemigroupsException);
  }
}

TEST_CASE("Semigroups 88: Exception: length_const",
          "[quick][finite][semigroup][88]") {
  std::vector<BMat8> gens
      = {BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
         BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
         BMat8({{1, 1, 0, 0}, {1, 0, 1, 0}, {0, 1, 1, 1}, {0, 1, 1, 1}})};
  Semigroup<BMat8> S(gens);

  for (size_t i = 0; i < S.size(); ++i) {
    REQUIRE_NOTHROW(S.length_const(i));
    REQUIRE_THROWS_AS(S.length_const(i + S.size()), LibsemigroupsException);
  }

  Semiring<int64_t>*    sr = new Integers();
  std::vector<Element*> gens2
      = {new MatrixOverSemiring<int64_t>({{0, 0}, {0, 1}}, sr),
         new MatrixOverSemiring<int64_t>({{0, 1}, {-1, 0}}, sr)};
  Semigroup<> T(gens2);
  delete_gens(gens2);

  for (size_t i = 0; i < T.size(); ++i) {
    REQUIRE_NOTHROW(T.length_const(i));
    REQUIRE_THROWS_AS(T.length_const(i + T.size()), LibsemigroupsException);
  }

  std::vector<Transformation<u_int16_t>> gens3
      = {Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
         Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};
  Semigroup<Transformation<u_int16_t>> U(gens3);

  for (size_t i = 0; i < U.size(); ++i) {
    REQUIRE_NOTHROW(U.length_const(i));
    REQUIRE_THROWS_AS(U.length_const(i + U.size()), LibsemigroupsException);
  }
}

TEST_CASE("Semigroups 89: Exception: product_by_reduction",
          "[quick][finite][semigroup][89]") {
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

  Semiring<int64_t>*    sr = new Integers();
  std::vector<Element*> gens2
      = {new MatrixOverSemiring<int64_t>({{0, 0}, {0, 1}}, sr),
         new MatrixOverSemiring<int64_t>({{0, 1}, {-1, 0}}, sr)};
  Semigroup<> T(gens2);
  delete_gens(gens2);

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

  std::vector<Transformation<u_int16_t>> gens3
      = {Transformation<u_int16_t>({0, 1, 2, 3}),
         Transformation<u_int16_t>({3, 1, 1, 2})};
  Semigroup<Transformation<u_int16_t>> U(gens3);

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

TEST_CASE("Semigroups 90: Exception: fast_product",
          "[quick][finite][semigroup][90]") {
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

  Semiring<int64_t>*    sr = new Integers();
  std::vector<Element*> gens2
      = {new MatrixOverSemiring<int64_t>({{0, 0}, {0, 1}}, sr),
         new MatrixOverSemiring<int64_t>({{0, 1}, {-1, 0}}, sr)};
  Semigroup<> T(gens2);
  delete_gens(gens2);

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

  std::vector<Transformation<u_int16_t>> gens3
      = {Transformation<u_int16_t>({0, 1, 2, 3}),
         Transformation<u_int16_t>({3, 1, 1, 2})};
  Semigroup<Transformation<u_int16_t>> U(gens3);

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

TEST_CASE("Semigroup 90: Exception: letter_to_pos",
          "[quick][finite][semigroup][90]") {
  for (size_t i = 1; i < 20; ++i) {
    std::vector<Element*> gens;

    for (size_t j = 0; j < i; ++j) {
      std::vector<size_t> trans;
      for (size_t k = 0; k < i; ++k) {
        trans.push_back((k + j) % i);
      }
      gens.push_back(new Transformation<size_t>(trans));
    }
    Semigroup<> S(gens);
    delete_gens(gens);

    for (size_t j = 0; j < i; ++j) {
      REQUIRE_NOTHROW(S.letter_to_pos(j));
    }
    REQUIRE_THROWS_AS(S.letter_to_pos(i), LibsemigroupsException);
  }
}

TEST_CASE("Semigroup 91: Exception: is_idempotent",
          "[quick][finite][semigroup][91]") {
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

  std::vector<Element*> gens2
      = {new Bipartition(
             {0, 1, 2, 1, 0, 2, 1, 0, 2, 2, 0, 0, 2, 0, 3, 4, 4, 1, 3, 0}),
         new Bipartition(
             {0, 1, 1, 1, 1, 2, 3, 2, 4, 5, 5, 2, 4, 2, 1, 1, 1, 2, 3, 2}),
         new Bipartition(
             {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0})};
  Semigroup<> T = Semigroup<>(gens2);
  delete_gens(gens2);

  // T has size 10
  for (size_t i = 0; i < 10; ++i) {
    REQUIRE_NOTHROW(T.is_idempotent(i));
  }
  for (size_t i = 0; i < 20; ++i) {
    REQUIRE_THROWS_AS(T.is_idempotent(10 + i), LibsemigroupsException);
  }
}

TEST_CASE("Semigroup 92: Exception: add_generators",
          "[quick][finite][semigroup][92]") {
  std::vector<Element*> gens1
      = {new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 2, 3, 2, 2, 3})};
  std::vector<Transformation<uint_fast8_t>> gens2
      = {Transformation<uint_fast8_t>({1, 7, 2, 6, 0, 0, 1, 2}),
         Transformation<uint_fast8_t>({2, 4, 6, 1, 4, 5, 2, 7})};
  std::vector<Element*> gens3
      = {new PartialPerm<u_int16_t>(
             {0, 1, 2, 3, 5, 6, 9}, {9, 7, 3, 5, 4, 2, 1}, 11),
         new PartialPerm<u_int16_t>({4, 5, 0}, {10, 0, 1}, 11)};

  Semigroup<>                             S(gens1);
  Semigroup<Transformation<uint_fast8_t>> T(gens2);
  Semigroup<>                             U(gens3);

  std::vector<Element*> additional_gens_1_1
      = {new Transformation<u_int16_t>({0, 1, 2, 3, 3, 3})};
  std::vector<Element*> additional_gens_1_2
      = {new Transformation<u_int16_t>({0, 1, 2, 3, 3, 3}),
         new Transformation<u_int16_t>({0, 1, 2, 3, 3, 3, 3})};
  std::vector<Transformation<uint_fast8_t>> additional_gens_2_1
      = {Transformation<uint_fast8_t>({1, 2, 2, 2, 1, 1, 3, 4}),
         Transformation<uint_fast8_t>({1, 2, 1, 3, 1, 4, 1, 5})};
  std::vector<Transformation<uint_fast8_t>> additional_gens_2_2
      = {Transformation<uint_fast8_t>({1, 2, 2, 2, 1, 1, 3, 4}),
         Transformation<uint_fast8_t>({1, 2, 1, 3, 1, 4, 1, 5, 1})};
  std::vector<Element*> additional_gens_3_1
      = {new PartialPerm<u_int16_t>(
             {0, 1, 2, 3, 5, 6, 9}, {2, 7, 5, 1, 4, 3, 9}, 11),
         new PartialPerm<u_int16_t>({2, 5, 1}, {6, 0, 3}, 11)};
  std::vector<Element*> additional_gens_3_2
      = {new PartialPerm<u_int16_t>(
             {0, 1, 2, 3, 5, 6, 9}, {2, 7, 5, 1, 4, 3, 9}, 11),
         new PartialPerm<u_int16_t>({2, 5, 1}, {6, 0, 3}, 12)};

  REQUIRE_NOTHROW(S.add_generators(additional_gens_1_1));
  REQUIRE_THROWS_AS(S.add_generators(additional_gens_1_2),
                    LibsemigroupsException);

  REQUIRE_NOTHROW(T.add_generators(additional_gens_2_1));
  REQUIRE_THROWS_AS(T.add_generators(additional_gens_2_2),
                    LibsemigroupsException);

  REQUIRE_NOTHROW(U.add_generators(additional_gens_3_1));
  REQUIRE_THROWS_AS(U.add_generators(additional_gens_3_2),
                    LibsemigroupsException);

  delete_gens(gens1);
  delete_gens(gens3);
  delete_gens(additional_gens_1_1);
  delete_gens(additional_gens_1_2);
  delete_gens(additional_gens_3_1);
  delete_gens(additional_gens_3_2);
}

TEST_CASE("Semigroup 93: non-pointer Bipartitions",
          "[quick][semigroup][finite][93]") {
  std::vector<Bipartition> gens = {
      Bipartition({0, 1, 2, 1, 0, 2, 1, 0, 2, 2, 0, 0, 2, 0, 3, 4, 4, 1, 3, 0}),
      Bipartition({0, 1, 1, 1, 1, 2, 3, 2, 4, 5, 5, 2, 4, 2, 1, 1, 1, 2, 3, 2}),
      Bipartition(
          {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0})};

  Semigroup<Bipartition> S(gens);

  S.reserve(10);
  S.set_report(SEMIGROUPS_REPORT);

  REQUIRE(S.size() == 10);
  REQUIRE(S.nridempotents() == 6);

  size_t pos = 0;
  for (auto it = S.cbegin(); it < S.cend(); ++it) {
    REQUIRE(S.position(*it) == pos);
    pos++;
  }

  S.add_generators({Bipartition(
      {0, 1, 2, 1, 1, 3, 1, 4, 2, 3, 1, 0, 3, 2, 3, 5, 4, 1, 3, 0})});

  REQUIRE(S.size() == 21);
  S.closure({Bipartition(
      {0, 1, 2, 1, 1, 3, 1, 4, 2, 3, 1, 0, 3, 2, 3, 5, 4, 1, 3, 0})});
  REQUIRE(S.size() == 21);
  REQUIRE(S.minimal_factorisation(Bipartition({0, 1, 2, 1, 0, 2, 1, 0, 2, 2,
                                               0, 0, 2, 0, 3, 4, 4, 1, 3, 0})
                                  * Bipartition({0, 1, 2, 1, 1, 3, 1, 4, 2, 3,
                                                 1, 0, 3, 2, 3, 5, 4, 1, 3, 0})
                                  * Bipartition({0, 1, 1, 1, 1, 2, 3, 2, 4, 5,
                                                 5, 2, 4, 2, 1, 1, 1, 2, 3, 2}))
          == word_t({0, 3, 1}));
  REQUIRE(S.minimal_factorisation(11) == word_t({0, 3}));
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
  REQUIRE(pos == S.nridempotents());
  for (auto it = S.cbegin_sorted() + 1; it < S.cend_sorted(); ++it) {
    REQUIRE(*(it - 1) < *it);
  }
}

TEST_CASE("Semigroup 94: non-pointer PartialPerms",
          "[quick][semigroup][finite][94]") {
  std::vector<PartialPerm<u_int16_t>> gens
      = {PartialPerm<u_int16_t>({0, 3, 4, 5}, {1, 0, 3, 2}, 6),
         PartialPerm<u_int16_t>({1, 2, 3}, {0, 5, 2}, 6),
         PartialPerm<u_int16_t>({0, 2, 3, 4, 5}, {5, 2, 3, 0, 1}, 6)};

  Semigroup<PartialPerm<u_int16_t>> S(gens);

  S.reserve(102);
  S.set_report(SEMIGROUPS_REPORT);

  REQUIRE(S.size() == 102);
  REQUIRE(S.nridempotents() == 8);
  size_t pos = 0;

  for (auto it = S.cbegin(); it < S.cend(); ++it) {
    REQUIRE(S.position(*it) == pos);
    pos++;
  }

  S.add_generators({PartialPerm<u_int16_t>({0, 1, 2}, {3, 4, 5}, 6)});
  REQUIRE(S.size() == 396);
  S.closure({PartialPerm<u_int16_t>({0, 1, 2}, {3, 4, 5}, 6)});
  REQUIRE(S.size() == 396);
  REQUIRE(S.minimal_factorisation(
              PartialPerm<u_int16_t>({0, 1, 2}, {3, 4, 5}, 6)
              * PartialPerm<u_int16_t>({0, 2, 3, 4, 5}, {5, 2, 3, 0, 1}, 6))
          == word_t({3, 2}));
  REQUIRE(S.minimal_factorisation(10) == word_t({2, 1}));
  REQUIRE(S.at(10) == PartialPerm<u_int16_t>({2, 3, 5}, {5, 2, 0}, 6));
  REQUIRE_THROWS_AS(S.minimal_factorisation(1000000000),
                    LibsemigroupsException);
  pos = 0;
  for (auto it = S.cbegin_idempotents(); it < S.cend_idempotents(); ++it) {
    REQUIRE(*it * *it == *it);
    pos++;
  }
  REQUIRE(pos == S.nridempotents());
  for (auto it = S.cbegin_sorted() + 1; it < S.cend_sorted(); ++it) {
    REQUIRE(*(it - 1) < *it);
  }
}

namespace libsemigroups {
  template <>
  int one(int) {
    return 1;
  }
  template <>
  uint8_t one(uint8_t) {
    return 1;
  }
}  // namespace libsemigroups

TEST_CASE("Semigroup 95: integers", "[quick][semigroup][finite][95]") {
  Semigroup<int> S({2});
  REQUIRE(S.size() == 32);
  REQUIRE(S.nridempotents() == 1);

  Semigroup<uint8_t> T({2, 3});
  REQUIRE(T.size() == 130);
  REQUIRE(T.nridempotents() == 2);
  REQUIRE(*T.cbegin_idempotents() == 0);
  REQUIRE(*T.cbegin_idempotents() + 1 == 1);
}

class NonTrivial {
 public:
  NonTrivial() : _re(0), _im(0) {}
  NonTrivial(int re, int im) : _re(re), _im(im) {}

  NonTrivial operator*(NonTrivial const& that) const {
    return NonTrivial(_re * that._re, _im * that._im);
  }

  bool operator==(NonTrivial const& that) const {
    return _re == that._re && _im == that._im;
  }

  bool operator<(NonTrivial const& that) const {
    return _re < that._re || (_re == that._re && _im < that._im);
  }

  NonTrivial one() const {
    return NonTrivial(1, 1);
  }

  size_t hash() const {
    return _re * 17 + _im;
  }

 private:
  int _re;
  int _im;
};

namespace std {
  template <> struct hash<NonTrivial> {
    size_t operator()(NonTrivial const& x) const {
      return x.hash();
    }
  };
}

static_assert(!std::is_trivial<NonTrivial>::value,
              "NonTrivial is not non-trivial");

TEST_CASE("Semigroup 96: non-trivial user type",
          "[quick][semigroup][finite][96]") {
  Semigroup<NonTrivial> S({NonTrivial(1, 1)});
  REQUIRE(S.size() == 1);
  REQUIRE(S.nridempotents() == 1);
}
