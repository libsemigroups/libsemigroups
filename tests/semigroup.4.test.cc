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
#include "src/semigroup.h"

#define SEMIGROUPS_REPORT false

using namespace libsemigroups;

template <class TElementType>
void delete_gens(std::vector<TElementType>& gens) {
  for (auto& x : gens) {
    delete x;
  }
}

#if (!(defined(LIBSEMIGROUPS_DENSEHASHMAP)) && LIBSEMIGROUPS_SIZEOF_VOID_P == 8)

TEST_CASE("Semigroup 073: regular boolean mat monoid 4 using BooleanMat",
          "[quick][semigroup][finite][073]") {
  std::vector<Element*> gens = {
      new BooleanMat({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
      new BooleanMat({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
      new BooleanMat({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {1, 0, 0, 1}}),
      new BooleanMat({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}})};
  Semigroup<> S(gens);
  REPORTER.set_report(SEMIGROUPS_REPORT);
  REQUIRE(S.size() == 63904);
  REQUIRE(S.nridempotents() == 2360);
  delete_gens(gens);
}
#endif

TEST_CASE("Semigroup 079: Exception: zero generators given",
          "[quick][finite][semigroup][079]") {
  std::vector<Element*> gens;

  REQUIRE_THROWS_AS(Semigroup<>(gens), LibsemigroupsException);
}

TEST_CASE("Semigroup 080: Exception: generators of different degrees",
          "[quick][finite][semigroup][080]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5, 5})};
  std::vector<Element*> gens2
      = {new PartialPerm<u_int16_t>(
             {0, 1, 2, 3, 5, 6, 9}, {9, 7, 3, 5, 4, 2, 1}, 10),
         new PartialPerm<u_int16_t>({4, 5, 0}, {10, 0, 1}, 11)};

  REQUIRE_THROWS_AS(Semigroup<>(gens), LibsemigroupsException);
  REQUIRE_THROWS_AS(Semigroup<>(gens2), LibsemigroupsException);

  delete_gens(gens);
  delete_gens(gens2);
}

TEST_CASE("Semigroup 081: Exception: word_to_pos",
          "[quick][finite][semigroup][081]") {
  Semiring<int64_t>*    sr = new Integers();
  std::vector<Element*> gens
      = {new MatrixOverSemiring<int64_t>({{0, 0}, {0, 1}}, sr),
         new MatrixOverSemiring<int64_t>({{0, 1}, {-1, 0}}, sr)};
  Semigroup<> T(gens);
  auto& UNDEFINED = Semigroup<>::UNDEFINED;

  REQUIRE_THROWS_AS(T.word_to_pos({}), LibsemigroupsException);
  REQUIRE_NOTHROW(T.word_to_pos({0, 0, 1, 1}));
  REQUIRE(T.word_to_pos({0, 0, 1, 1}) == UNDEFINED);
  REQUIRE(T.current_position(T.word_to_element({0, 0, 1, 1})) == UNDEFINED);
  REQUIRE_THROWS_AS(T.word_to_pos({0, 0, 1, 2}), LibsemigroupsException);

  REQUIRE(T.size() == 13);
  REQUIRE(T.word_to_pos({0, 0, 1, 1}) == 6);
  REQUIRE(T.current_position(T.word_to_element({0, 0, 1, 1})) == 6);

  std::vector<Element*> gens2
      = {new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}),
         new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};
  Semigroup<> U(gens2);

  REQUIRE_THROWS_AS(U.word_to_pos({}), LibsemigroupsException);
  REQUIRE_NOTHROW(U.word_to_pos({0, 0, 1, 2}));
  REQUIRE_THROWS_AS(U.word_to_pos({5}), LibsemigroupsException);

  delete_gens(gens);
  delete_gens(gens2);
  delete sr;
}

TEST_CASE("Semigroup 082: Exception: word_to_element",
          "[quick][finite][semigroup][082]") {
  Semiring<int64_t>*    sr = new Integers();
  std::vector<Element*> gens
      = {new MatrixOverSemiring<int64_t>({{0, 0}, {0, 1}}, sr),
         new MatrixOverSemiring<int64_t>({{0, 1}, {-1, 0}}, sr)};
  Semigroup<> T(gens);

  REQUIRE_THROWS_AS(T.word_to_element({}), LibsemigroupsException);
  REQUIRE_THROWS_AS(T.word_to_element({0, 0, 1, 2}), LibsemigroupsException);

  Element* t = T.word_to_element({0, 0, 1, 1});
  REQUIRE(*t
          == MatrixOverSemiring<int64_t>({{0, 0}, {0, 1}}, sr)
                 * MatrixOverSemiring<int64_t>({{0, 0}, {0, 1}}, sr)
                 * MatrixOverSemiring<int64_t>({{0, 1}, {-1, 0}}, sr)
                 * MatrixOverSemiring<int64_t>({{0, 1}, {-1, 0}}, sr));

  std::vector<Element*> gens2
      = {new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}),
         new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};
  Semigroup<> U(gens2);

  REQUIRE_THROWS_AS(U.word_to_element({}), LibsemigroupsException);
  REQUIRE_THROWS_AS(U.word_to_element({5}), LibsemigroupsException);

  Element* u = U.word_to_element({0, 0, 1, 2});
  REQUIRE(*u
          == Transformation<u_int16_t>({0, 1, 2, 3, 4, 5})
                 * Transformation<u_int16_t>({0, 1, 2, 3, 4, 5})
                 * Transformation<u_int16_t>({1, 0, 2, 3, 4, 5})
                 * Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}));
  delete t;
  delete u;
  delete_gens(gens);
  delete_gens(gens2);
  delete sr;
}

TEST_CASE("Semigroup 083: Exception: gens", "[quick][finite][semigroup][083]") {
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

TEST_CASE("Semigroup 084: Exception: prefix",
          "[quick][finite][semigroup][084]") {
  Semiring<int64_t>*    sr = new Integers();
  std::vector<Element*> gens
      = {new MatrixOverSemiring<int64_t>({{0, 0}, {0, 1}}, sr),
         new MatrixOverSemiring<int64_t>({{0, 1}, {-1, 0}}, sr)};
  Semigroup<> T(gens);
  delete_gens(gens);

  for (size_t i = 0; i < T.size(); ++i) {
    REQUIRE_NOTHROW(T.prefix(i));
    REQUIRE_THROWS_AS(T.prefix(i + T.size()), LibsemigroupsException);
  }
  delete sr;
}

TEST_CASE("Semigroup 085: Exception: suffix",
          "[quick][finite][semigroup][085]") {
  Semiring<int64_t>*    sr = new Integers();
  std::vector<Element*> gens
      = {new MatrixOverSemiring<int64_t>({{0, 0}, {0, 1}}, sr),
         new MatrixOverSemiring<int64_t>({{0, 1}, {-1, 0}}, sr)};
  Semigroup<> T(gens);

  for (size_t i = 0; i < T.size(); ++i) {
    REQUIRE_NOTHROW(T.suffix(i));
    REQUIRE_THROWS_AS(T.suffix(i + T.size()), LibsemigroupsException);
  }
  delete_gens(gens);
  delete sr;
}

TEST_CASE("Semigroup 086: Exception: first_letter",
          "[quick][finite][semigroup][086]") {
  Semiring<int64_t>*    sr = new Integers();
  std::vector<Element*> gens
      = {new MatrixOverSemiring<int64_t>({{0, 0}, {0, 1}}, sr),
         new MatrixOverSemiring<int64_t>({{0, 1}, {-1, 0}}, sr)};
  Semigroup<> T(gens);

  for (size_t i = 0; i < T.size(); ++i) {
    REQUIRE_NOTHROW(T.first_letter(i));
    REQUIRE_THROWS_AS(T.first_letter(i + T.size()), LibsemigroupsException);
  }
  delete_gens(gens);
  delete sr;
}

TEST_CASE("Semigroup 087: Exception: final_letter",
          "[quick][finite][semigroup][087]") {
  Semiring<int64_t>*    sr = new Integers();
  std::vector<Element*> gens
      = {new MatrixOverSemiring<int64_t>({{0, 0}, {0, 1}}, sr),
         new MatrixOverSemiring<int64_t>({{0, 1}, {-1, 0}}, sr)};
  Semigroup<> T(gens);

  for (size_t i = 0; i < T.size(); ++i) {
    REQUIRE_NOTHROW(T.final_letter(i));
    REQUIRE_THROWS_AS(T.final_letter(i + T.size()), LibsemigroupsException);
  }
  delete_gens(gens);
  delete sr;
}

TEST_CASE("Semigroup 088: Exception: length_const",
          "[quick][finite][semigroup][088]") {
  Semiring<int64_t>*    sr = new Integers();
  std::vector<Element*> gens
      = {new MatrixOverSemiring<int64_t>({{0, 0}, {0, 1}}, sr),
         new MatrixOverSemiring<int64_t>({{0, 1}, {-1, 0}}, sr)};
  Semigroup<> T(gens);

  for (size_t i = 0; i < T.size(); ++i) {
    REQUIRE_NOTHROW(T.length_const(i));
    REQUIRE_THROWS_AS(T.length_const(i + T.size()), LibsemigroupsException);
  }
  delete_gens(gens);
  delete sr;
}

TEST_CASE("Semigroup 089: Exception: product_by_reduction",
          "[quick][finite][semigroup][089]") {
  Semiring<int64_t>*    sr = new Integers();
  std::vector<Element*> gens
      = {new MatrixOverSemiring<int64_t>({{0, 0}, {0, 1}}, sr),
         new MatrixOverSemiring<int64_t>({{0, 1}, {-1, 0}}, sr)};
  Semigroup<> T(gens);

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

  delete_gens(gens);
  delete sr;
}

TEST_CASE("Semigroup 090: Exception: fast_product",
          "[quick][finite][semigroup][090]") {
  Semiring<int64_t>*    sr = new Integers();
  std::vector<Element*> gens
      = {new MatrixOverSemiring<int64_t>({{0, 0}, {0, 1}}, sr),
         new MatrixOverSemiring<int64_t>({{0, 1}, {-1, 0}}, sr)};
  Semigroup<> T(gens);

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

  delete_gens(gens);
  delete sr;
}

TEST_CASE("Semigroup 091: Exception: letter_to_pos",
          "[quick][finite][semigroup][091]") {
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

TEST_CASE("Semigroup 092: Exception: is_idempotent",
          "[quick][finite][semigroup][092]") {
  std::vector<Element*> gens
      = {new Bipartition(
             {0, 1, 2, 1, 0, 2, 1, 0, 2, 2, 0, 0, 2, 0, 3, 4, 4, 1, 3, 0}),
         new Bipartition(
             {0, 1, 1, 1, 1, 2, 3, 2, 4, 5, 5, 2, 4, 2, 1, 1, 1, 2, 3, 2}),
         new Bipartition(
             {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0})};
  Semigroup<> T = Semigroup<>(gens);
  delete_gens(gens);

  // T has size 10
  for (size_t i = 0; i < 10; ++i) {
    REQUIRE_NOTHROW(T.is_idempotent(i));
  }
  for (size_t i = 0; i < 20; ++i) {
    REQUIRE_THROWS_AS(T.is_idempotent(10 + i), LibsemigroupsException);
  }
}

TEST_CASE("Semigroup 093: Exception: add_generators",
          "[quick][finite][semigroup][093]") {
  std::vector<Element*> gens1
      = {new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 2, 3, 2, 2, 3})};
  std::vector<Element*> gens2
      = {new PartialPerm<u_int16_t>(
             {0, 1, 2, 3, 5, 6, 9}, {9, 7, 3, 5, 4, 2, 1}, 11),
         new PartialPerm<u_int16_t>({4, 5, 0}, {10, 0, 1}, 11)};

  Semigroup<> S(gens1);
  Semigroup<> U(gens2);

  std::vector<Element*> additional_gens_1_1
      = {new Transformation<u_int16_t>({0, 1, 2, 3, 3, 3})};
  std::vector<Element*> additional_gens_1_2
      = {new Transformation<u_int16_t>({0, 1, 2, 3, 3, 3}),
         new Transformation<u_int16_t>({0, 1, 2, 3, 3, 3, 3})};
  std::vector<Element*> additional_gens_2_1
      = {new PartialPerm<u_int16_t>(
             {0, 1, 2, 3, 5, 6, 9}, {2, 7, 5, 1, 4, 3, 9}, 11),
         new PartialPerm<u_int16_t>({2, 5, 1}, {6, 0, 3}, 11)};
  std::vector<Element*> additional_gens_2_2
      = {new PartialPerm<u_int16_t>(
             {0, 1, 2, 3, 5, 6, 9}, {2, 7, 5, 1, 4, 3, 9}, 11),
         new PartialPerm<u_int16_t>({2, 5, 1}, {6, 0, 3}, 12)};

  REQUIRE_NOTHROW(S.add_generators(additional_gens_1_1));
  REQUIRE_THROWS_AS(S.add_generators(additional_gens_1_2),
                    LibsemigroupsException);

  REQUIRE_NOTHROW(U.add_generators(additional_gens_2_1));
  REQUIRE_THROWS_AS(U.add_generators(additional_gens_2_2),
                    LibsemigroupsException);

  delete_gens(gens1);
  delete_gens(gens2);
  delete_gens(additional_gens_1_1);
  delete_gens(additional_gens_1_2);
  delete_gens(additional_gens_2_1);
  delete_gens(additional_gens_2_2);
}
