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

#define SEMIGROUPS_REPORT false

using namespace libsemigroups;

#if !defined(LIBSEMIGROUPS_HAVE_DENSEHASHMAP) \
    || !defined(LIBSEMIGROUPS_USE_DENSEHASHMAP)

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

  word_t* ww = S.minimal_factorisation(S.at(378));
  REQUIRE(*ww == word_t({0, 1, 2, 0, 1, 2}));
  delete ww;

  ww = S.minimal_factorisation(BMat8({{1, 0, 0, 1, 1},
                                      {0, 1, 0, 0, 1},
                                      {1, 0, 1, 0, 1},
                                      {0, 0, 1, 0, 1},
                                      {0, 0, 0, 0, 0}}));
  REQUIRE(ww == nullptr);
  delete ww;

  ww = S.minimal_factorisation(1000000);
  REQUIRE(ww == nullptr);

  w.clear();
  S.factorisation(w, 378);
  REQUIRE(w == word_t({0, 1, 2, 0, 1, 2}));
  REQUIRE(S.length_const(378) == 6);

  ww = S.factorisation(S.at(378));
  REQUIRE(*ww == word_t({0, 1, 2, 0, 1, 2}));
  delete ww;

  ww = S.factorisation(BMat8({{1, 0, 0, 1, 1},
                              {0, 1, 0, 0, 1},
                              {1, 0, 1, 0, 1},
                              {0, 0, 1, 0, 1},
                              {0, 0, 0, 0, 0}}));
  REQUIRE(ww == nullptr);
  delete ww;

  ww = S.factorisation(1000000);
  REQUIRE(ww == nullptr);

  S.next_relation(w);
  REQUIRE(w == std::vector<size_t>({2, 2, 2}));
  S.next_relation(w);
  REQUIRE(w == std::vector<size_t>({3, 0, 7}));
  S.next_relation(w);
  REQUIRE(w == std::vector<size_t>({3, 2, 3}));

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
  really_delete_cont(gens);
  S.set_report(SEMIGROUPS_REPORT);
  REQUIRE(S.size() == 63904);
  REQUIRE(S.nridempotents() == 2360);
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
  S.reserve(2 * 32311832);
  REQUIRE(S.size() == 32311832);
  REQUIRE(S.nridempotents() == 73023);
  // FIXME this demonstrates a very bad split with almost all the
  // elements being put in the final thread by init_idempotents
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
  Semigroup<> S(gens);
  really_delete_cont(gens);
  S.set_report(true);
  REQUIRE(S.size() == 32311832);
  REQUIRE(S.nridempotents() == 73023);
  // FIXME this demonstrates a very bad split with almost all the
  // elements being put in the final thread by init_idempotents
  S.set_report(false);
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
