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

TEST_CASE("Semigroup 098: non-pointer BooleanMats",
          "[quick][semigroup][finite][098]") {
  std::vector<BooleanMat> gens
      = {BooleanMat({0, 1, 0, 1, 1, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 0}),
         BooleanMat({0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1}),
         BooleanMat({0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1})};

  Semigroup<BooleanMat> S(gens);

  S.reserve(26);
  REPORTER.set_report(SEMIGROUPS_REPORT);

  REQUIRE(S.size() == 26);
  REQUIRE(S.nridempotents() == 4);
  size_t pos = 0;

  for (auto it = S.cbegin(); it < S.cend(); ++it) {
    REQUIRE(S.position(*it) == pos);
    pos++;
  }

  S.add_generators(
      {BooleanMat({1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 1, 1, 0})});
  REQUIRE(S.size() == 29);
  S.closure({BooleanMat({1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 1, 1, 0})});
  REQUIRE(S.size() == 29);
  REQUIRE(S.minimal_factorisation(
              BooleanMat({1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 1, 1, 0})
              * BooleanMat({0, 1, 0, 1, 1, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 0}))
          == word_t({3, 0}));
  REQUIRE(S.minimal_factorisation(28) == word_t({3, 0}));
  REQUIRE(
      S.at(28)
      == BooleanMat({1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 1, 1, 0})
             * BooleanMat({0, 1, 0, 1, 1, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 0}));
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

TEST_CASE("Semigroup 099: non-pointer ProjectiveMaxPlusMatrix",
          "[quick][semigroup][finite][099]") {
  Semiring<int64_t>*                   sr = new MaxPlusSemiring();
  std::vector<ProjectiveMaxPlusMatrix> gens
      = {ProjectiveMaxPlusMatrix({0, 1, 2, 3, 4, 1, 2, 1, 1}, sr),
         ProjectiveMaxPlusMatrix({0, 1, 1, 1, 1, 1, 0, 0, 0}, sr),
         ProjectiveMaxPlusMatrix({0, 1, 1, 0, 0, 1, 1, 0, 0}, sr)};
  Semigroup<ProjectiveMaxPlusMatrix> S(gens);

  S.reserve(142);
  REPORTER.set_report(SEMIGROUPS_REPORT);

  REQUIRE(S.size() == 142);
  REQUIRE(S.nridempotents() == 90);
  size_t pos = 0;

  for (auto it = S.cbegin(); it < S.cend(); ++it) {
    REQUIRE(S.position(*it) == pos);
    pos++;
  }

  S.add_generators({ProjectiveMaxPlusMatrix({1, 0, 0, 1, 0, 1, 0, 1, 0}, sr)});
  REQUIRE(S.size() == 223);
  S.closure({ProjectiveMaxPlusMatrix({1, 0, 0, 1, 0, 1, 0, 1, 0}, sr)});
  REQUIRE(S.size() == 223);
  REQUIRE(S.minimal_factorisation(
              ProjectiveMaxPlusMatrix({1, 0, 0, 1, 0, 1, 0, 1, 0}, sr)
              * ProjectiveMaxPlusMatrix({0, 1, 2, 3, 4, 1, 2, 1, 1}, sr))
          == word_t({3, 0}));
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
  delete sr;
}

TEST_CASE("Semigroup 100: non-pointer PBR", "[quick][semigroup][finite][100]") {
  std::vector<PBR> gens = {PBR({{5, 3},
                                {5, 4, 3, 0, 1, 2},
                                {5, 4, 3, 0, 2},
                                {5, 3, 0, 1, 2},
                                {5, 0, 2},
                                {5, 4, 3, 1, 2}}),
                           PBR({{5, 4, 3, 0},
                                {5, 4, 2},
                                {5, 1, 2},
                                {5, 4, 3, 2},
                                {5, 4, 3, 2},
                                {4, 1, 2}}),
                           PBR({{5, 4, 3, 0},
                                {5, 4, 2},
                                {5, 1, 2},
                                {5, 4, 3, 2},
                                {5, 4, 3, 2},
                                {4, 1, 2}})};
  Semigroup<PBR>   S(gens);

  S.reserve(4);
  REPORTER.set_report(SEMIGROUPS_REPORT);

  REQUIRE(S.size() == 4);
  REQUIRE(S.nridempotents() == 2);
  size_t pos = 0;

  for (auto it = S.cbegin(); it < S.cend(); ++it) {
    REQUIRE(S.position(*it) == pos);
    pos++;
  }
  S.add_generators({PBR(
      {{5, 4, 3}, {5, 4, 2}, {4, 2, 1}, {5, 3, 0}, {5, 3, 2, 1}, {3, 1, 2}})});
  REQUIRE(S.size() == 6);
  S.closure({PBR(
      {{5, 4, 3}, {5, 4, 2}, {4, 2, 1}, {5, 3, 0}, {5, 3, 2, 1}, {3, 1, 2}})});
  REQUIRE(S.size() == 6);
  REQUIRE(S.minimal_factorisation(PBR({{5, 3},
                                       {5, 4, 3, 0, 1, 2},
                                       {5, 4, 3, 0, 2},
                                       {5, 3, 0, 1, 2},
                                       {5, 0, 2},
                                       {5, 4, 3, 1, 2}})
                                  * PBR({{5, 4, 3},
                                         {5, 4, 2},
                                         {4, 2, 1},
                                         {5, 3, 0},
                                         {5, 3, 2, 1},
                                         {3, 1, 2}}))
          == word_t({0, 0}));
  REQUIRE(S.minimal_factorisation(5) == word_t({3, 3}));
  REQUIRE(S.at(5)
          == PBR({{5, 4, 3},
                  {5, 4, 2},
                  {4, 2, 1},
                  {5, 3, 0},
                  {5, 3, 2, 1},
                  {3, 1, 2}})
                 * PBR({{5, 4, 3},
                        {5, 4, 2},
                        {4, 2, 1},
                        {5, 3, 0},
                        {5, 3, 2, 1},
                        {3, 1, 2}}));
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

TEST_CASE("Semigroup 101: non-pointer matrices over TropicalMaxPlusSemiring",
          "[quick][semigroup][finite][101]") {
  Semiring<int64_t>*                       sr = new TropicalMaxPlusSemiring(9);
  std::vector<MatrixOverSemiring<int64_t>> gens
      = {MatrixOverSemiring<int64_t>({{1, 3}, {2, 1}}, sr),
         MatrixOverSemiring<int64_t>({{2, 1}, {4, 0}}, sr)};
  Semigroup<MatrixOverSemiring<int64_t>> S
      = Semigroup<MatrixOverSemiring<int64_t>>(gens);

  S.reserve(4);
  REPORTER.set_report(SEMIGROUPS_REPORT);

  REQUIRE(S.size() == 20);
  REQUIRE(S.nridempotents() == 1);
  size_t pos = 0;

  for (auto it = S.cbegin(); it < S.cend(); ++it) {
    REQUIRE(S.position(*it) == pos);
    pos++;
  }
  S.add_generators({MatrixOverSemiring<int64_t>({{1, 1}, {0, 2}}, sr)});
  REQUIRE(S.size() == 73);
  S.closure({MatrixOverSemiring<int64_t>({{1, 1}, {0, 2}}, sr)});
  REQUIRE(S.size() == 73);
  REQUIRE(S.minimal_factorisation(
              MatrixOverSemiring<int64_t>({{1, 1}, {0, 2}}, sr)
              * MatrixOverSemiring<int64_t>({{2, 1}, {4, 0}}, sr))
          == word_t({2, 1}));
  REQUIRE(S.minimal_factorisation(52) == word_t({0, 2, 2, 1}));
  REQUIRE(S.at(52) == MatrixOverSemiring<int64_t>({{9, 7}, {9, 5}}, sr));
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

  delete sr;
}

TEST_CASE("Semigroup 102: non-pointer matrices over Integers",
          "[quick][semigroup][finite][102]") {
  Semiring<int64_t>*                       sr = new Integers();
  std::vector<MatrixOverSemiring<int64_t>> gens
      = {MatrixOverSemiring<int64_t>({{0, 1}, {0, -1}}, sr),
         MatrixOverSemiring<int64_t>({{0, 1}, {2, 0}}, sr)};
  Semigroup<MatrixOverSemiring<int64_t>> S
      = Semigroup<MatrixOverSemiring<int64_t>>(gens);

  S.reserve(10000);
  REPORTER.set_report(SEMIGROUPS_REPORT);

  S.enumerate(10000);
  REQUIRE(S.current_size() == 631);
  size_t pos = 0;

  for (auto it = S.cbegin(); it < S.cend(); ++it) {
    REQUIRE(S.position(*it) == pos);
    pos++;
  }
  S.enumerate(1000000);
  REQUIRE(S.current_size() == 631);
  REQUIRE(S.minimal_factorisation(
              MatrixOverSemiring<int64_t>({{0, 1}, {0, -1}}, sr)
              * MatrixOverSemiring<int64_t>({{0, 1}, {2, 0}}, sr)
              * MatrixOverSemiring<int64_t>({{0, 1}, {2, 0}}, sr))
          == word_t({0, 1, 0}));
  REQUIRE(S.minimal_factorisation(52)
          == word_t({0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1}));
  REQUIRE(S.at(52) == MatrixOverSemiring<int64_t>({{64, 0}, {-64, 0}}, sr));
  REQUIRE_THROWS_AS(S.minimal_factorisation(1000000000),
                    LibsemigroupsException);
  delete sr;
}
