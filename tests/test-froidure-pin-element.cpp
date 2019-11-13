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

#include <cinttypes>  // for int64_t
#include <cstddef>    // for size_t
#include <iterator>   // for reverse_iterator, opera...
#include <vector>     // for vector

#include "bruidhinn-traits.hpp"      // for detail::BruidhinnTraits
#include "catch.hpp"                 // for LIBSEMIGROUPS_TEST_CASE
#include "element.hpp"               // for Element
#include "froidure-pin.hpp"          // for FroidurePin, FroidurePi...
#include "libsemigroups-config.hpp"  // for LIBSEMIGROUPS_SIZEOF_VO...
#include "report.hpp"                // for ReportGuard
#include "semiring.hpp"              // for Integers, Semiring, Max...
#include "test-main.hpp"
#include "types.hpp"  // for word_type, letter_type

namespace libsemigroups {
  struct LibsemigroupsException;
  constexpr bool REPORT = false;

  static inline size_t evaluate_reduct(FroidurePin<Element const*>& S,
                                       word_type const&             word) {
    letter_type out = S.letter_to_pos(word[0]);
    for (auto it = word.cbegin() + 1; it < word.cend(); ++it) {
      out = S.right(out, *it);
    }
    return out;
  }

  static inline size_t evaluate_reduct(FroidurePin<Element const*>* S,
                                       word_type const&             word) {
    return evaluate_reduct(*S, word);
  }

  static inline void test_idempotent(FroidurePin<Element const*>& S,
                                     Element const*               x) {
    REQUIRE(S.is_idempotent(S.position(x)));
    Element* y = x->heap_copy();
    y->redefine(x, x);
    REQUIRE(*x == *y);
    REQUIRE(S.fast_product(S.position(x), S.position(x)) == S.position(x));
    delete y;
  }

  template <typename TElementType>
  void delete_gens(std::vector<TElementType>& gens) {
    for (auto x : gens) {
      delete x;
    }
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "017",
                          "small transformation semigroup",
                          "[quick][froidure-pin][element]") {
    std::vector<Element*> gens = {new Transformation<uint16_t>({0, 1, 0}),
                                  new Transformation<uint16_t>({0, 1, 2})};
    static_assert(
        std::is_same<typename FroidurePin<Element const*>::element_type,
                     Element*>::value
            == true,
        "WTF");
    FroidurePin<Element const*> S(gens);
    auto                        rg = ReportGuard(REPORT);
    delete_gens(gens);

    REQUIRE(S.size() == 2);
    REQUIRE(S.degree() == 3);
    REQUIRE(S.nr_idempotents() == 2);
    REQUIRE(S.nr_generators() == 2);
    REQUIRE(S.nr_rules() == 4);

    Element* expected = new Transformation<uint16_t>({0, 1, 0});
    REQUIRE(*S[0] == *expected);
    delete expected;

    expected = new Transformation<uint16_t>({0, 1, 2});
    REQUIRE(*S[1] == *expected);
    delete expected;

    Element* x = new Transformation<uint16_t>({0, 1, 0});
    REQUIRE(S.position(x) == 0);
    REQUIRE(S.contains(x));
    delete x;

    x = new Transformation<uint16_t>({0, 1, 2});
    REQUIRE(S.position(x) == 1);
    REQUIRE(S.contains(x));
    delete x;

    x = new Transformation<uint16_t>({0, 0, 0});
    REQUIRE(S.position(x) == UNDEFINED);
    REQUIRE(!S.contains(x));
    delete x;
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "018",
                          "small partial perm semigroup",
                          "[quick][froidure-pin][element]") {
    std::vector<Element*> gens
        = {new PartialPerm<uint16_t>(
               {0, 1, 2, 3, 5, 6, 9}, {9, 7, 3, 5, 4, 2, 1}, 11),
           new PartialPerm<uint16_t>({4, 5, 0}, {10, 0, 1}, 11)};
    FroidurePin<Element const*> S  = FroidurePin<Element const*>(gens);
    auto                        rg = ReportGuard(REPORT);
    delete_gens(gens);

    REQUIRE(S.size() == 22);
    REQUIRE(S.degree() == 11);
    REQUIRE(S.nr_idempotents() == 1);
    REQUIRE(S.nr_generators() == 2);
    REQUIRE(S.nr_rules() == 9);

    Element* expected = new PartialPerm<uint16_t>(
        {0, 1, 2, 3, 5, 6, 9}, {9, 7, 3, 5, 4, 2, 1}, 11);
    REQUIRE(*S[0] == *expected);
    delete expected;

    expected = new PartialPerm<uint16_t>({4, 5, 0}, {10, 0, 1}, 11);
    REQUIRE(*S[1] == *expected);
    delete expected;

    Element* x = new Transformation<uint16_t>({0, 1, 0});
    REQUIRE(S.position(x) == UNDEFINED);
    REQUIRE(!S.contains(x));
    delete x;

    x = new PartialPerm<uint16_t>({}, {}, 11);
    REQUIRE(S.position(x) == 10);
    REQUIRE(S.contains(x));
    delete x;

    x = new PartialPerm<uint16_t>({}, {}, 9);
    REQUIRE(S.position(x) == UNDEFINED);
    REQUIRE(!S.contains(x));

    delete x;

    x = new PartialPerm<uint16_t>(
        {0, 1, 2, 3, 5, 6, 9}, {9, 7, 3, 5, 4, 2, 1}, 11);
    REQUIRE(S.position(x) == 0);
    REQUIRE(S.contains(x));

    Element* y = new PartialPerm<uint16_t>({4, 5, 0}, {10, 0, 1}, 11);
    REQUIRE(S.position(y) == 1);
    REQUIRE(S.contains(y));

    y->redefine(x, x);
    REQUIRE(S.position(y) == 2);
    REQUIRE(S.contains(y));

    REQUIRE(*y == *S[2]);
    delete x;
    delete y;
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "019",
                          "small bipartition semigroup",
                          "[quick][froidure-pin][element]") {
    std::vector<Element*> gens
        = {new Bipartition(
               {0, 1, 2, 1, 0, 2, 1, 0, 2, 2, 0, 0, 2, 0, 3, 4, 4, 1, 3, 0}),
           new Bipartition(
               {0, 1, 1, 1, 1, 2, 3, 2, 4, 5, 5, 2, 4, 2, 1, 1, 1, 2, 3, 2}),
           new Bipartition(
               {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0})};
    FroidurePin<Element const*> S  = FroidurePin<Element const*>(gens);
    auto                        rg = ReportGuard(REPORT);

    REQUIRE(S.size() == 10);
    REQUIRE(S.degree() == 10);
    REQUIRE(S.nr_idempotents() == 6);
    REQUIRE(S.nr_generators() == 3);
    REQUIRE(S.nr_rules() == 14);

    REQUIRE(*S[0] == *gens[0]);
    REQUIRE(*S[1] == *gens[1]);
    REQUIRE(*S[2] == *gens[2]);

    REQUIRE(S.position(gens[0]) == 0);
    REQUIRE(S.contains(gens[0]));

    REQUIRE(S.position(gens[1]) == 1);
    REQUIRE(S.contains(gens[1]));

    REQUIRE(S.position(gens[2]) == 2);
    REQUIRE(S.contains(gens[2]));

    Element* y = new Bipartition(
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0});
    REQUIRE(S.position(y) == 2);
    REQUIRE(S.contains(y));
    y->redefine(gens[0], gens[1]);
    REQUIRE(S.position(y) == 4);
    REQUIRE(S.contains(y));
    y->redefine(gens[1], gens[2]);
    REQUIRE(S.position(y) == 7);
    REQUIRE(S.contains(y));
    delete y;
    delete_gens(gens);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "020",
                          "small Boolean matrix semigroup",
                          "[quick][froidure-pin][element]") {
    std::vector<Element*> gens
        = {new BooleanMat({{1, 0, 1}, {0, 1, 0}, {0, 1, 0}}),
           new BooleanMat({{0, 0, 0}, {0, 0, 0}, {0, 0, 0}}),
           new BooleanMat({{0, 0, 0}, {0, 0, 0}, {0, 0, 0}})};
    FroidurePin<Element const*> S  = FroidurePin<Element const*>(gens);
    auto                        rg = ReportGuard(REPORT);

    REQUIRE(S.size() == 3);
    REQUIRE(S.degree() == 3);
    REQUIRE(S.nr_idempotents() == 2);
    REQUIRE(S.nr_generators() == 3);
    REQUIRE(S.nr_rules() == 7);
    REQUIRE(*S[0] == *gens[0]);
    REQUIRE(*S[1] == *gens[1]);
    REQUIRE(*S[1] == *gens[2]);

    REQUIRE(S.position(gens[0]) == 0);
    REQUIRE(S.contains(gens[0]));

    REQUIRE(S.position(gens[1]) == 1);
    REQUIRE(S.contains(gens[1]));

    REQUIRE(S.position(gens[2]) == 1);
    REQUIRE(S.contains(gens[1]));

    Element* y = new BooleanMat({{0, 0, 0}, {0, 0, 0}, {0, 0, 0}});
    y->redefine(gens[0], gens[0]);
    REQUIRE(S.position(y) == 2);
    REQUIRE(S.contains(y));
    delete y;
    delete_gens(gens);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "021",
                          "small projective max plus matrix semigroup",
                          "[quick][froidure-pin][element]") {
    Semiring<int64_t>* sr = new MaxPlusSemiring();
    auto x = new ProjectiveMaxPlusMatrix({{0, 0, 0}, {0, 0, 0}, {0, 0, 0}}, sr);
    auto id = x->identity();
    delete x;
    FroidurePin<Element const*> S({&id});
    auto                        rg = ReportGuard(REPORT);

    REQUIRE(S.size() == 1);
    REQUIRE(S.degree() == 3);
    REQUIRE(S.nr_idempotents() == 1);
    REQUIRE(S.nr_generators() == 1);
    REQUIRE(S.nr_rules() == 1);
    REQUIRE(*S[0] == id);

    REQUIRE(S.position(&id) == 0);
    REQUIRE(S.contains(&id));

    x = new ProjectiveMaxPlusMatrix({{-2, 2, 0}, {-1, 0, 0}, {1, -3, 1}}, sr);
    REQUIRE(S.position(x) == UNDEFINED);
    REQUIRE(!S.contains(x));
    delete x;
    delete sr;
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "022",
                          "small matrix semigroup [Integers]",
                          "[quick][froidure-pin][element]") {
    Semiring<int64_t>*    sr = new Integers();
    std::vector<Element*> gens
        = {new MatrixOverSemiring<int64_t>({{0, 0}, {0, 1}}, sr),
           new MatrixOverSemiring<int64_t>({{0, 1}, {-1, 0}}, sr)};
    FroidurePin<Element const*> S  = FroidurePin<Element const*>(gens);
    auto                        rg = ReportGuard(REPORT);

    REQUIRE(S.size() == 13);
    REQUIRE(S.degree() == 2);
    REQUIRE(S.nr_idempotents() == 4);
    REQUIRE(S.nr_generators() == 2);
    REQUIRE(S.nr_rules() == 6);
    REQUIRE(*S[0] == *(gens)[0]);
    REQUIRE(*S[1] == *(gens)[1]);

    REQUIRE(S.position(gens[0]) == 0);
    REQUIRE(S.contains(gens[0]));

    REQUIRE(S.position(gens[1]) == 1);
    REQUIRE(S.contains(gens[1]));

    Element* x = new MatrixOverSemiring<int64_t>({{-2, 2}, {-1, 0}}, sr);
    REQUIRE(S.position(x) == UNDEFINED);
    REQUIRE(!S.contains(x));

    x->redefine(gens[1], gens[1]);
    REQUIRE(S.position(x) == 4);
    REQUIRE(S.contains(x));
    delete x;

    x = new MatrixOverSemiring<int64_t>({{-2, 2, 0}, {-1, 0, 0}, {0, 0, 0}},
                                        sr);
    REQUIRE(S.position(x) == UNDEFINED);
    REQUIRE(!S.contains(x));
    delete x;

    delete sr;
    delete_gens(gens);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "023",
                          "small matrix semigroup [MaxPlusSemiring]",
                          "[quick][froidure-pin][element]") {
    Semiring<int64_t>*    sr = new MaxPlusSemiring();
    std::vector<Element*> gens
        = {new MatrixOverSemiring<int64_t>({{0, -4}, {-4, -1}}, sr),
           new MatrixOverSemiring<int64_t>({{0, -3}, {-3, -1}}, sr)};
    FroidurePin<Element const*> S  = FroidurePin<Element const*>(gens);
    auto                        rg = ReportGuard(REPORT);

    REQUIRE(S.size() == 26);
    REQUIRE(S.degree() == 2);
    REQUIRE(S.nr_idempotents() == 4);
    REQUIRE(S.nr_generators() == 2);
    REQUIRE(S.nr_rules() == 9);
    REQUIRE(*S[0] == *gens[0]);
    REQUIRE(*S[1] == *gens[1]);

    REQUIRE(S.position(gens[0]) == 0);
    REQUIRE(S.contains(gens[0]));

    REQUIRE(S.position(gens[1]) == 1);
    REQUIRE(S.contains(gens[1]));

    Element* x = new MatrixOverSemiring<int64_t>({{-2, 2}, {-1, 0}}, sr);
    REQUIRE(S.position(x) == UNDEFINED);
    REQUIRE(!S.contains(x));
    x->redefine(gens[1], gens[1]);
    REQUIRE(S.position(x) == 5);
    REQUIRE(S.contains(x));
    delete x;

    x = new MatrixOverSemiring<int64_t>({{-2, 2, 0}, {-1, 0, 0}, {0, 0, 0}},
                                        sr);
    REQUIRE(S.position(x) == UNDEFINED);
    REQUIRE(!S.contains(x));
    delete x;

    delete sr;
    delete_gens(gens);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "024",
                          "small matrix semigroup [MinPlusSemiring]",
                          "[quick][froidure-pin][element]") {
    Semiring<int64_t>*    sr   = new MinPlusSemiring();
    std::vector<Element*> gens = {
        new MatrixOverSemiring<int64_t>({{1, 0}, {0, POSITIVE_INFINITY}}, sr)};
    FroidurePin<Element const*> S  = FroidurePin<Element const*>(gens);
    auto                        rg = ReportGuard(REPORT);

    REQUIRE(S.size() == 3);
    REQUIRE(S.degree() == 2);
    REQUIRE(S.nr_idempotents() == 1);
    REQUIRE(S.nr_generators() == 1);
    REQUIRE(S.nr_rules() == 1);

    REQUIRE(*S[0] == *gens[0]);
    REQUIRE(S.position(gens[0]) == 0);
    REQUIRE(S.contains(gens[0]));

    Element* x = new MatrixOverSemiring<int64_t>({{-2, 2}, {-1, 0}}, sr);
    REQUIRE(S.position(x) == UNDEFINED);
    REQUIRE(!S.contains(x));
    x->redefine(gens[0], gens[0]);
    REQUIRE(S.position(x) == 1);
    REQUIRE(S.contains(x));
    delete x;

    x = new MatrixOverSemiring<int64_t>({{-2, 2, 0}, {-1, 0, 0}, {0, 0, 0}},
                                        sr);
    REQUIRE(S.position(x) == UNDEFINED);
    REQUIRE(!S.contains(x));
    delete x;

    delete sr;
    delete_gens(gens);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "025",
                          "small matrix semigroup [TropicalMaxPlusSemiring]",
                          "[quick][froidure-pin][element]") {
    Semiring<int64_t>*    sr   = new TropicalMaxPlusSemiring(33);
    std::vector<Element*> gens = {
        new MatrixOverSemiring<int64_t>({{22, 21, 0}, {10, 0, 0}, {1, 32, 1}},
                                        sr),
        new MatrixOverSemiring<int64_t>({{0, 0, 0}, {0, 1, 0}, {1, 1, 0}}, sr)};
    FroidurePin<Element const*> S  = FroidurePin<Element const*>(gens);
    auto                        rg = ReportGuard(REPORT);

    REQUIRE(S.size() == 119);
    REQUIRE(S.degree() == 3);
    REQUIRE(S.nr_idempotents() == 1);
    REQUIRE(S.nr_generators() == 2);
    REQUIRE(S.nr_rules() == 18);

    REQUIRE(*S[0] == *gens[0]);
    REQUIRE(S.position(gens[0]) == 0);
    REQUIRE(S.contains(gens[0]));

    Element* x = new MatrixOverSemiring<int64_t>({{2, 2}, {1, 0}}, sr);
    REQUIRE(S.position(x) == UNDEFINED);
    REQUIRE(!S.contains(x));
    delete x;

    x = new MatrixOverSemiring<int64_t>({{2, 2, 0}, {1, 0, 0}, {0, 0, 0}}, sr);
    REQUIRE(S.position(x) == UNDEFINED);
    REQUIRE(!S.contains(x));
    x->redefine(gens[0], gens[0]);
    REQUIRE(S.position(x) == 2);
    REQUIRE(S.contains(x));
    delete x;

    delete sr;
    delete_gens(gens);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "026",
                          "small matrix semigroup [TropicalMinPlusSemiring]",
                          "[quick][froidure-pin][element]") {
    Semiring<int64_t>*    sr   = new TropicalMinPlusSemiring(11);
    std::vector<Element*> gens = {
        new MatrixOverSemiring<int64_t>({{2, 1, 0}, {10, 0, 0}, {1, 2, 1}}, sr),
        new MatrixOverSemiring<int64_t>({{10, 0, 0}, {0, 1, 0}, {1, 1, 0}},
                                        sr)};
    FroidurePin<Element const*> S  = FroidurePin<Element const*>(gens);
    auto                        rg = ReportGuard(REPORT);

    REQUIRE(S.size() == 1039);
    REQUIRE(S.degree() == 3);
    REQUIRE(S.nr_idempotents() == 5);
    REQUIRE(S.nr_generators() == 2);
    REQUIRE(S.nr_rules() == 38);

    REQUIRE(*S[0] == *gens[0]);
    REQUIRE(S.position(gens[0]) == 0);
    REQUIRE(S.contains(gens[0]));

    Element* x = new MatrixOverSemiring<int64_t>({{2, 2}, {1, 0}}, sr);
    REQUIRE(S.position(x) == UNDEFINED);
    REQUIRE(!S.contains(x));
    delete x;

    x = new MatrixOverSemiring<int64_t>({{2, 2, 0}, {1, 0, 0}, {0, 0, 0}}, sr);
    REQUIRE(S.position(x) == UNDEFINED);
    REQUIRE(!S.contains(x));
    x->redefine(gens[0], gens[0]);
    REQUIRE(S.position(x) == 2);
    REQUIRE(S.contains(x));
    delete x;

    delete sr;
    delete_gens(gens);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "027",
                          "small matrix semigroup [NaturalSemiring]",
                          "[quick][froidure-pin][element]") {
    Semiring<int64_t>*    sr   = new NaturalSemiring(11, 3);
    std::vector<Element*> gens = {
        new MatrixOverSemiring<int64_t>({{2, 1, 0}, {10, 0, 0}, {1, 2, 1}}, sr),
        new MatrixOverSemiring<int64_t>({{10, 0, 0}, {0, 1, 0}, {1, 1, 0}},
                                        sr)};
    FroidurePin<Element const*> S  = FroidurePin<Element const*>(gens);
    auto                        rg = ReportGuard(REPORT);

    REQUIRE(S.size() == 86);
    REQUIRE(S.degree() == 3);
    REQUIRE(S.nr_idempotents() == 10);
    REQUIRE(S.nr_generators() == 2);
    REQUIRE(S.nr_rules() == 16);

    REQUIRE(*S[0] == *gens[0]);
    REQUIRE(S.position(gens[0]) == 0);
    REQUIRE(S.contains(gens[0]));

    Element* x = new MatrixOverSemiring<int64_t>({{2, 2}, {1, 0}}, sr);
    REQUIRE(S.position(x) == UNDEFINED);
    REQUIRE(!S.contains(x));
    delete x;

    x = new MatrixOverSemiring<int64_t>({{2, 2, 0}, {1, 0, 0}, {0, 0, 0}}, sr);
    REQUIRE(S.position(x) == UNDEFINED);
    REQUIRE(!S.contains(x));
    x->redefine(gens[1], gens[0]);
    REQUIRE(S.position(x) == 4);
    REQUIRE(S.contains(x));
    delete x;

    delete sr;
    delete_gens(gens);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "028",
                          "small pbr semigroup",
                          "[quick][froidure-pin][element]") {
    std::vector<Element*> gens
        = {new PBR({{1}, {4}, {3}, {1}, {0, 2}, {0, 3, 4, 5}}),
           new PBR({{1, 2}, {0, 1}, {0, 2, 3}, {0, 1, 2}, {3}, {0, 3, 4, 5}})};
    FroidurePin<Element const*> S  = FroidurePin<Element const*>(gens);
    auto                        rg = ReportGuard(REPORT);

    REQUIRE(S.size() == 30);
    REQUIRE(S.degree() == 3);
    REQUIRE(S.nr_idempotents() == 22);
    REQUIRE(S.nr_generators() == 2);
    REQUIRE(S.nr_rules() == 11);

    REQUIRE(*S[0] == *gens[0]);
    REQUIRE(S.position(gens[0]) == 0);
    REQUIRE(S.contains(gens[0]));
    REQUIRE(*S[1] == *gens[1]);
    REQUIRE(S.position(gens[1]) == 1);
    REQUIRE(S.contains(gens[1]));

    Element* x = new PBR({{}, {}, {}, {}, {}, {}});
    REQUIRE(S.position(x) == UNDEFINED);
    REQUIRE(!S.contains(x));
    x->redefine(gens[1], gens[1]);
    REQUIRE(S.position(x) == 5);
    REQUIRE(S.contains(x));
    delete x;
    delete_gens(gens);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "029",
                          "large transformation semigroup",
                          "[quick][froidure-pin][element][no-valgrind]") {
    std::vector<Element*> gens
        = {new Transformation<uint16_t>({0, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 0, 2, 3, 4, 5}),
           new Transformation<uint16_t>({4, 0, 1, 2, 3, 5}),
           new Transformation<uint16_t>({5, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 1, 2, 3, 4, 5})};
    FroidurePin<Element const*> S  = FroidurePin<Element const*>(gens);
    auto                        rg = ReportGuard(REPORT);
    delete_gens(gens);

    REQUIRE(S.size() == 7776);
    REQUIRE(S.degree() == 6);
    REQUIRE(S.nr_idempotents() == 537);
    REQUIRE(S.nr_generators() == 5);
    REQUIRE(S.nr_rules() == 2459);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "030",
                          "at, position, current_*",
                          "[quick][froidure-pin][element][no-valgrind]") {
    auto                  rg = ReportGuard(REPORT);
    std::vector<Element*> gens
        = {new Transformation<uint16_t>({0, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 0, 2, 3, 4, 5}),
           new Transformation<uint16_t>({4, 0, 1, 2, 3, 5}),
           new Transformation<uint16_t>({5, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 1, 2, 3, 4, 5})};
    FroidurePin<Element const*> S = FroidurePin<Element const*>(gens);
    delete_gens(gens);

    S.batch_size(1024);

    REQUIRE(*S.at(100) == Transformation<uint16_t>({5, 3, 4, 1, 2, 5}));
    REQUIRE(S.current_size() == 1029);
    REQUIRE(S.current_nr_rules() == 74);
    REQUIRE(S.current_max_word_length() == 7);

    Element* x = new Transformation<uint16_t>({5, 3, 4, 1, 2, 5});
    REQUIRE(S.position(x) == 100);
    delete x;

    REQUIRE(*S.at(1023) == Transformation<uint16_t>({5, 4, 3, 4, 1, 5}));
    REQUIRE(S.current_size() == 1029);
    REQUIRE(S.current_nr_rules() == 74);
    REQUIRE(S.current_max_word_length() == 7);

    x = new Transformation<uint16_t>({5, 4, 3, 4, 1, 5});
    REQUIRE(S.position(x) == 1023);
    delete x;

    REQUIRE(*S.at(3000) == Transformation<uint16_t>({5, 3, 5, 3, 4, 5}));
    REQUIRE(S.current_size() == 3001);
    REQUIRE(S.current_nr_rules() == 526);
    REQUIRE(S.current_max_word_length() == 9);

    x = new Transformation<uint16_t>({5, 3, 5, 3, 4, 5});
    REQUIRE(S.position(x) == 3000);
    delete x;

    REQUIRE(S.size() == 7776);
    REQUIRE(S.degree() == 6);
    REQUIRE(S.nr_idempotents() == 537);
    REQUIRE(S.nr_generators() == 5);
    REQUIRE(S.nr_rules() == 2459);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "031",
                          "run",
                          "[quick][froidure-pin][element][no-valgrind]") {
    std::vector<Element*> gens
        = {new Transformation<uint16_t>({0, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 0, 2, 3, 4, 5}),
           new Transformation<uint16_t>({4, 0, 1, 2, 3, 5}),
           new Transformation<uint16_t>({5, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 1, 2, 3, 4, 5})};
    FroidurePin<Element const*> S  = FroidurePin<Element const*>(gens);
    auto                        rg = ReportGuard(REPORT);
    delete_gens(gens);

    S.batch_size(1024);

    S.enumerate(3000);
    REQUIRE(S.current_size() == 3000);
    REQUIRE(S.current_nr_rules() == 526);
    REQUIRE(S.current_max_word_length() == 9);

    S.enumerate(3001);
    REQUIRE(S.current_size() == 4024);
    REQUIRE(S.current_nr_rules() == 999);
    REQUIRE(S.current_max_word_length() == 10);

    S.enumerate(7000);
    REQUIRE(S.current_size() == 7000);
    REQUIRE(S.current_nr_rules() == 2044);
    REQUIRE(S.current_max_word_length() == 12);

    REQUIRE(S.size() == 7776);
    REQUIRE(S.degree() == 6);
    REQUIRE(S.nr_idempotents() == 537);
    REQUIRE(S.nr_generators() == 5);
    REQUIRE(S.nr_rules() == 2459);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "032",
                          "run [many stops and starts]",
                          "[quick][froidure-pin][element][no-valgrind]") {
    std::vector<Element*> gens
        = {new Transformation<uint16_t>({0, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 0, 2, 3, 4, 5}),
           new Transformation<uint16_t>({4, 0, 1, 2, 3, 5}),
           new Transformation<uint16_t>({5, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 1, 2, 3, 4, 5})};
    FroidurePin<Element const*> S  = FroidurePin<Element const*>(gens);
    auto                        rg = ReportGuard(REPORT);
    delete_gens(gens);

    S.batch_size(128);

    for (size_t i = 1; !S.finished(); i++) {
      S.enumerate(i * 128);
    }

    REQUIRE(S.size() == 7776);
    REQUIRE(S.degree() == 6);
    REQUIRE(S.nr_idempotents() == 537);
    REQUIRE(S.nr_generators() == 5);
    REQUIRE(S.nr_rules() == 2459);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "033",
                          "factorisation, length [1 element]",
                          "[quick][froidure-pin][element][no-valgrind]") {
    std::vector<Element*> gens
        = {new Transformation<uint16_t>({0, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 0, 2, 3, 4, 5}),
           new Transformation<uint16_t>({4, 0, 1, 2, 3, 5}),
           new Transformation<uint16_t>({5, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 1, 2, 3, 4, 5})};
    FroidurePin<Element const*> S  = FroidurePin<Element const*>(gens);
    auto                        rg = ReportGuard(REPORT);
    delete_gens(gens);

    S.batch_size(1024);

    word_type result;
    S.factorisation(result, 5537);
    word_type expected = {1, 2, 2, 2, 3, 2, 4, 1, 2, 2, 3};
    REQUIRE(result == expected);
    REQUIRE(S.length_const(5537) == 11);
    REQUIRE(S.length_non_const(5537) == 11);
    REQUIRE(S.current_max_word_length() == 11);

    REQUIRE(S.current_size() == 5539);
    REQUIRE(S.current_nr_rules() == 1484);
    REQUIRE(S.current_max_word_length() == 11);

    REQUIRE(S.length_non_const(7775) == 16);
    REQUIRE(S.current_max_word_length() == 16);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "034",
                          "factorisation, products [all elements]",
                          "[quick][froidure-pin][element][no-valgrind]") {
    std::vector<Element*> gens
        = {new Transformation<uint16_t>({0, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 0, 2, 3, 4, 5}),
           new Transformation<uint16_t>({4, 0, 1, 2, 3, 5}),
           new Transformation<uint16_t>({5, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 1, 2, 3, 4, 5})};
    FroidurePin<Element const*> S  = FroidurePin<Element const*>(gens);
    auto                        rg = ReportGuard(REPORT);
    delete_gens(gens);

    S.batch_size(1024);

    word_type result;
    for (size_t i = 0; i < S.size(); i++) {
      S.factorisation(result, i);
      REQUIRE(evaluate_reduct(S, result) == i);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "035",
                          "first/final letter, prefix, suffix, products",
                          "[quick][froidure-pin][element][no-valgrind]") {
    std::vector<Element*> gens
        = {new Transformation<uint16_t>({0, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 0, 2, 3, 4, 5}),
           new Transformation<uint16_t>({4, 0, 1, 2, 3, 5}),
           new Transformation<uint16_t>({5, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 1, 2, 3, 4, 5})};
    FroidurePin<Element const*> S  = FroidurePin<Element const*>(gens);
    auto                        rg = ReportGuard(REPORT);
    delete_gens(gens);

    S.enumerate(1000);  // fully enumerates

    REQUIRE(S.first_letter(6377) == 2);
    REQUIRE(S.prefix(6377) == 5049);
    REQUIRE(S.final_letter(6377) == 2);
    REQUIRE(S.suffix(6377) == 5149);
    REQUIRE(S.fast_product(S.prefix(6377), S.final_letter(6377)) == 6377);
    REQUIRE(S.fast_product(S.first_letter(6377), S.suffix(6377)) == 6377);
    REQUIRE(S.product_by_reduction(S.prefix(6377), S.final_letter(6377))
            == 6377);
    REQUIRE(S.product_by_reduction(S.first_letter(6377), S.suffix(6377))
            == 6377);

    REQUIRE(S.first_letter(2103) == 3);
    REQUIRE(S.prefix(2103) == 1050);
    REQUIRE(S.final_letter(2103) == 1);
    REQUIRE(S.suffix(2103) == 860);
    REQUIRE(S.fast_product(S.prefix(2103), S.final_letter(2103)) == 2103);
    REQUIRE(S.fast_product(S.first_letter(2103), S.suffix(2103)) == 2103);
    REQUIRE(S.product_by_reduction(S.prefix(2103), S.final_letter(2103))
            == 2103);
    REQUIRE(S.product_by_reduction(S.first_letter(2103), S.suffix(2103))
            == 2103);

    REQUIRE(S.first_letter(3407) == 2);
    REQUIRE(S.prefix(3407) == 1923);
    REQUIRE(S.final_letter(3407) == 3);
    REQUIRE(S.suffix(3407) == 2115);
    REQUIRE(S.fast_product(S.prefix(3407), S.final_letter(3407)) == 3407);
    REQUIRE(S.fast_product(S.first_letter(3407), S.suffix(3407)) == 3407);
    REQUIRE(S.product_by_reduction(S.prefix(3407), S.final_letter(3407))
            == 3407);
    REQUIRE(S.product_by_reduction(S.first_letter(3407), S.suffix(3407))
            == 3407);

    REQUIRE(S.first_letter(4245) == 2);
    REQUIRE(S.prefix(4245) == 2767);
    REQUIRE(S.final_letter(4245) == 3);
    REQUIRE(S.suffix(4245) == 2319);
    REQUIRE(S.fast_product(S.prefix(4225), S.final_letter(4225)) == 4225);
    REQUIRE(S.fast_product(S.first_letter(4225), S.suffix(4225)) == 4225);
    REQUIRE(S.product_by_reduction(S.prefix(4225), S.final_letter(4225))
            == 4225);
    REQUIRE(S.product_by_reduction(S.first_letter(4225), S.suffix(4225))
            == 4225);

    REQUIRE(S.first_letter(3683) == 4);
    REQUIRE(S.prefix(3683) == 2246);
    REQUIRE(S.final_letter(3683) == 2);
    REQUIRE(S.suffix(3683) == 1685);
    REQUIRE(S.fast_product(S.prefix(3683), S.final_letter(3683)) == 3683);
    REQUIRE(S.fast_product(S.first_letter(3683), S.suffix(3683)) == 3683);
    REQUIRE(S.product_by_reduction(S.prefix(3683), S.final_letter(3683))
            == 3683);
    REQUIRE(S.product_by_reduction(S.first_letter(3683), S.suffix(3683))
            == 3683);

    REQUIRE(S.first_letter(0) == 0);
    REQUIRE(S.prefix(0) == UNDEFINED);
    REQUIRE(S.final_letter(0) == 0);
    REQUIRE(S.suffix(0) == UNDEFINED);

    REQUIRE(S.first_letter(7775) == 1);
    REQUIRE(S.prefix(7775) == 7760);
    REQUIRE(S.final_letter(7775) == 2);
    REQUIRE(S.suffix(7775) == 7768);
    REQUIRE(S.fast_product(S.prefix(7775), S.final_letter(7775)) == 7775);
    REQUIRE(S.fast_product(S.first_letter(7775), S.suffix(7775)) == 7775);
    REQUIRE(S.product_by_reduction(S.prefix(7775), S.final_letter(7775))
            == 7775);
    REQUIRE(S.product_by_reduction(S.first_letter(7775), S.suffix(7775))
            == 7775);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "036",
                          "letter_to_pos [standard]",
                          "[quick][froidure-pin][element]") {
    std::vector<Element*> gens
        = {new Transformation<uint16_t>({0, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 0, 2, 3, 4, 5}),
           new Transformation<uint16_t>({4, 0, 1, 2, 3, 5}),
           new Transformation<uint16_t>({5, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 1, 2, 3, 4, 5})};
    FroidurePin<Element const*> S  = FroidurePin<Element const*>(gens);
    auto                        rg = ReportGuard(REPORT);
    delete_gens(gens);

    REQUIRE(S.letter_to_pos(0) == 0);
    REQUIRE(S.letter_to_pos(1) == 1);
    REQUIRE(S.letter_to_pos(2) == 2);
    REQUIRE(S.letter_to_pos(3) == 3);
    REQUIRE(S.letter_to_pos(4) == 4);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "037",
                          "letter_to_pos [duplicate gens]",
                          "[quick][froidure-pin][element][no-valgrind]") {
    std::vector<Element*> gens
        = {new Transformation<uint16_t>({0, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 0, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 0, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 0, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 0, 2, 3, 4, 5}),
           new Transformation<uint16_t>({4, 0, 1, 2, 3, 5}),
           new Transformation<uint16_t>({5, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 0, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 0, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 0, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 0, 2, 3, 4, 5}),
           new Transformation<uint16_t>({4, 0, 1, 2, 3, 5}),
           new Transformation<uint16_t>({5, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 0, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 0, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 0, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 0, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 0, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 0, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 0, 2, 3, 4, 5}),
           new Transformation<uint16_t>({4, 0, 1, 2, 3, 5}),
           new Transformation<uint16_t>({5, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 0, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 0, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 0, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 0, 2, 3, 4, 5}),
           new Transformation<uint16_t>({4, 0, 1, 2, 3, 5}),
           new Transformation<uint16_t>({5, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 0, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 0, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 0, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 1, 2, 3, 4, 5})};
    FroidurePin<Element const*> S  = FroidurePin<Element const*>(gens);
    auto                        rg = ReportGuard(REPORT);
    delete_gens(gens);

    REQUIRE(S.letter_to_pos(0) == 0);
    REQUIRE(S.letter_to_pos(1) == 1);
    REQUIRE(S.letter_to_pos(2) == 1);
    REQUIRE(S.letter_to_pos(3) == 1);
    REQUIRE(S.letter_to_pos(4) == 1);
    REQUIRE(S.letter_to_pos(10) == 1);
    REQUIRE(S.letter_to_pos(12) == 3);

    REQUIRE(S.size() == 7776);
    REQUIRE(S.degree() == 6);
    REQUIRE(S.nr_idempotents() == 537);
    REQUIRE(S.nr_generators() == 32);
    REQUIRE(S.nr_rules() == 2621);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "038",
                          "letter_to_pos [after add_generators]",
                          "[quick][froidure-pin][element][no-valgrind]") {
    std::vector<Element*> gens
        = {new Transformation<uint16_t>({0, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 0, 2, 3, 4, 5}),
           new Transformation<uint16_t>({4, 0, 1, 2, 3, 5}),
           new Transformation<uint16_t>({5, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 1, 2, 3, 4, 5})};
    FroidurePin<Element const*> S  = FroidurePin<Element const*>({gens[0]});
    auto                        rg = ReportGuard(REPORT);

    REQUIRE(S.size() == 1);
    REQUIRE(S.degree() == 6);
    REQUIRE(S.nr_idempotents() == 1);
    REQUIRE(S.nr_generators() == 1);
    REQUIRE(S.nr_rules() == 1);

    S.add_generators({gens[1]});
    REQUIRE(S.size() == 2);
    REQUIRE(S.degree() == 6);
    REQUIRE(S.nr_idempotents() == 1);
    REQUIRE(S.nr_generators() == 2);
    REQUIRE(S.nr_rules() == 4);

    S.add_generators({gens[2]});
    REQUIRE(S.size() == 120);
    REQUIRE(S.degree() == 6);
    REQUIRE(S.nr_idempotents() == 1);
    REQUIRE(S.nr_generators() == 3);
    REQUIRE(S.nr_rules() == 25);

    S.add_generators({gens[3]});
    REQUIRE(S.size() == 1546);
    REQUIRE(S.degree() == 6);
    REQUIRE(S.nr_idempotents() == 32);
    REQUIRE(S.nr_generators() == 4);
    REQUIRE(S.nr_rules() == 495);

    S.add_generators({gens[4]});
    REQUIRE(S.size() == 7776);
    REQUIRE(S.degree() == 6);
    REQUIRE(S.nr_idempotents() == 537);
    REQUIRE(S.nr_generators() == 5);
    REQUIRE(S.nr_rules() == 2459);

    REQUIRE(S.letter_to_pos(0) == 0);
    REQUIRE(S.letter_to_pos(1) == 1);
    REQUIRE(S.letter_to_pos(2) == 2);
    REQUIRE(S.letter_to_pos(3) == 120);
    REQUIRE(S.letter_to_pos(4) == 1546);
    delete_gens(gens);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "039",
                          "cbegin_idempotents/cend [1 thread]",
                          "[quick][froidure-pin][element][no-valgrind]") {
    std::vector<Element*> gens
        = {new Transformation<uint16_t>({0, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 0, 2, 3, 4, 5}),
           new Transformation<uint16_t>({4, 0, 1, 2, 3, 5}),
           new Transformation<uint16_t>({5, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 1, 2, 3, 4, 5})};
    FroidurePin<Element const*> S  = FroidurePin<Element const*>(gens);
    auto                        rg = ReportGuard(REPORT);
    delete_gens(gens);

    size_t nr = 0;
    for (auto it = S.cbegin_idempotents(); it < S.cend_idempotents(); it++) {
      test_idempotent(S, *it);
      nr++;
    }
    REQUIRE(nr == S.nr_idempotents());
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "040",
                          "idempotent_cend/cbegin [1 thread]",
                          "[quick][froidure-pin][element][no-valgrind]") {
    std::vector<Element*> gens
        = {new Transformation<uint16_t>({0, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 0, 2, 3, 4, 5}),
           new Transformation<uint16_t>({4, 0, 1, 2, 3, 5}),
           new Transformation<uint16_t>({5, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 1, 2, 3, 4, 5})};
    FroidurePin<Element const*> S  = FroidurePin<Element const*>(gens);
    auto                        rg = ReportGuard(REPORT);
    delete_gens(gens);

    size_t nr  = 0;
    auto   end = S.cend_idempotents();
    for (auto it = S.cbegin_idempotents(); it < end; it++) {
      test_idempotent(S, *it);
      nr++;
    }
    REQUIRE(nr == S.nr_idempotents());
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "041",
                          "is_idempotent [1 thread]",
                          "[quick][froidure-pin][element][no-valgrind]") {
    std::vector<Element*> gens
        = {new Transformation<uint16_t>({0, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 0, 2, 3, 4, 5}),
           new Transformation<uint16_t>({4, 0, 1, 2, 3, 5}),
           new Transformation<uint16_t>({5, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 1, 2, 3, 4, 5})};
    FroidurePin<Element const*> S  = FroidurePin<Element const*>(gens);
    auto                        rg = ReportGuard(REPORT);
    delete_gens(gens);

    S.max_threads(1000);

    size_t nr = 0;
    for (size_t i = 0; i < S.size(); i++) {
      if (S.is_idempotent(i)) {
        nr++;
      }
    }
    REQUIRE(nr == S.nr_idempotents());
  }
// FIXME(later) the following example is extremely slow when using densehashmap.
#if !defined(LIBSEMIGROUPS_DENSEHASHMAP)

  LIBSEMIGROUPS_TEST_CASE(
      "FroidurePin",
      "042",
      "cbegin_idempotents/cend, is_idempotent [2 threads]",
      "[standard][froidure-pin][element][multithread][no-valgrind]") {
    std::vector<Element*> gens
        = {new Transformation<uint16_t>({1, 2, 3, 4, 5, 6, 0}),
           new Transformation<uint16_t>({1, 0, 2, 3, 4, 5, 6}),
           new Transformation<uint16_t>({0, 1, 2, 3, 4, 5, 0})};
    FroidurePin<Element const*> S  = FroidurePin<Element const*>(gens);
    auto                        rg = ReportGuard(REPORT);
    S.max_threads(2);

    size_t nr = 0;

    for (auto it = S.cbegin_idempotents(); it < S.cend_idempotents(); it++) {
      test_idempotent(S, *it);
      nr++;
    }
    REQUIRE(nr == S.nr_idempotents());
    REQUIRE(nr == 6322);

    nr = 0;
    for (auto it = S.cbegin_idempotents(); it < S.cend_idempotents(); it++) {
      test_idempotent(S, *it);
      nr++;
    }
    REQUIRE(nr == S.nr_idempotents());
    REQUIRE(nr == 6322);
    delete_gens(gens);
  }

#endif

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "043",
                          "finished, started",
                          "[quick][froidure-pin][element]") {
    std::vector<Element*> gens
        = {new Transformation<uint16_t>({0, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 0, 2, 3, 4, 5}),
           new Transformation<uint16_t>({4, 0, 1, 2, 3, 5}),
           new Transformation<uint16_t>({5, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 1, 2, 3, 4, 5})};
    FroidurePin<Element const*> S  = FroidurePin<Element const*>(gens);
    auto                        rg = ReportGuard(REPORT);

    REQUIRE(!S.started());
    REQUIRE(!S.finished());

    S.batch_size(1024);
    S.enumerate(10);
    REQUIRE(S.started());
    REQUIRE(!S.finished());

    S.enumerate(8000);
    REQUIRE(S.started());
    REQUIRE(S.finished());
    delete_gens(gens);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "044",
                          "current_position",
                          "[quick][froidure-pin][element]") {
    std::vector<Element*> gens
        = {new Transformation<uint16_t>({0, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 0, 2, 3, 4, 5}),
           new Transformation<uint16_t>({4, 0, 1, 2, 3, 5}),
           new Transformation<uint16_t>({5, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 1, 2, 3, 4, 5})};
    FroidurePin<Element const*> S  = FroidurePin<Element const*>(gens);
    auto                        rg = ReportGuard(REPORT);

    REQUIRE(S.current_position(gens[0]) == 0);
    REQUIRE(S.current_position(gens[1]) == 1);
    REQUIRE(S.current_position(gens[2]) == 2);
    REQUIRE(S.current_position(gens[3]) == 3);
    REQUIRE(S.current_position(gens[4]) == 4);

    S.batch_size(1024);
    S.enumerate(1024);

    REQUIRE(S.current_size() == 1029);
    REQUIRE(S.current_nr_rules() == 74);
    REQUIRE(S.current_max_word_length() == 7);
    REQUIRE(S.current_position(S.at(1024)) == 1024);

    Element* x = new Transformation<uint16_t>({5, 1, 5, 5, 2, 5});
    REQUIRE(S.current_position(x) == 1028);
    REQUIRE(S.current_size() == 1029);
    REQUIRE(S.current_nr_rules() == 74);
    REQUIRE(S.current_max_word_length() == 7);
    delete x;

    x = new Transformation<uint16_t>({5, 1, 5, 5, 2, 5, 6});
    REQUIRE(S.current_position(x) == UNDEFINED);
    REQUIRE(S.current_size() == 1029);
    REQUIRE(S.current_nr_rules() == 74);
    REQUIRE(S.current_max_word_length() == 7);
    delete x;

    x = new Transformation<uint16_t>({5, 4, 5, 1, 0, 5});
    REQUIRE(S.current_position(x) == UNDEFINED);
    REQUIRE(S.current_size() == 1029);
    REQUIRE(S.current_nr_rules() == 74);
    REQUIRE(S.current_max_word_length() == 7);
    REQUIRE(S.position(x) == 1029);
    delete x;
    delete_gens(gens);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "045",
                          "sorted_position, sorted_at",
                          "[quick][froidure-pin][element]") {
    std::vector<Element*> gens
        = {new Transformation<uint16_t>({0, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 0, 2, 3, 4, 5}),
           new Transformation<uint16_t>({4, 0, 1, 2, 3, 5}),
           new Transformation<uint16_t>({5, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 1, 2, 3, 4, 5})};
    FroidurePin<Element const*> S  = FroidurePin<Element const*>(gens);
    auto                        rg = ReportGuard(REPORT);

    REQUIRE(S.sorted_position(gens[0]) == 310);
    REQUIRE(*S.sorted_at(310) == *gens[0]);
    REQUIRE(S.sorted_at(310) == S.at(0));

    REQUIRE(S.sorted_position(gens[1]) == 1390);
    REQUIRE(*S.sorted_at(1390) == *gens[1]);
    REQUIRE(S.sorted_at(1390) == S.at(1));

    REQUIRE(S.sorted_position(gens[2]) == 5235);
    REQUIRE(*S.sorted_at(5235) == *gens[2]);
    REQUIRE(S.sorted_at(5235) == S.at(2));

    REQUIRE(S.sorted_position(gens[3]) == 6790);
    REQUIRE(*S.sorted_at(6790) == *gens[3]);
    REQUIRE(S.sorted_at(6790) == S.at(3));

    REQUIRE(S.sorted_position(gens[4]) == 1606);
    REQUIRE(*S.sorted_at(1606) == *gens[4]);
    REQUIRE(S.sorted_at(1606) == S.at(4));

    REQUIRE(S.finished());

    REQUIRE(S.sorted_position(S.at(1024)) == 6810);
    REQUIRE(S.position_to_sorted_position(1024) == 6810);

    REQUIRE(*S.sorted_at(6810) == *S.at(1024));
    REQUIRE(S.sorted_at(6810) == S.at(1024));

    Element* x = new Transformation<uint16_t>({5, 1, 5, 5, 2, 5});
    REQUIRE(S.sorted_position(x) == 6908);
    REQUIRE(S.position_to_sorted_position(S.position(x)) == 6908);
    REQUIRE(*S.sorted_at(6908) == *x);
    REQUIRE(S.sorted_at(6908) == S.at(S.position(x)));
    delete x;

    x = new Transformation<uint16_t>({5, 5, 5, 1, 5, 5, 6});
    REQUIRE(S.sorted_position(x) == UNDEFINED);
    delete x;

    // REQUIRE(S.sorted_at(100000) == nullptr);
    // REQUIRE(S.at(100000) == nullptr);
    REQUIRE(S.position_to_sorted_position(100000) == UNDEFINED);
    delete_gens(gens);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "046",
                          "right/left Cayley graph",
                          "[quick][froidure-pin][element][no-valgrind]") {
    std::vector<Element*> gens
        = {new Transformation<uint16_t>({0, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 0, 2, 3, 4, 5}),
           new Transformation<uint16_t>({4, 0, 1, 2, 3, 5}),
           new Transformation<uint16_t>({5, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 1, 2, 3, 4, 5})};
    FroidurePin<Element const*> S  = FroidurePin<Element const*>(gens);
    auto                        rg = ReportGuard(REPORT);

    REQUIRE(S.right(0, 0) == 0);
    REQUIRE(S.left(0, 0) == 0);

    Element* tmp = new Transformation<uint16_t>({0, 1, 2, 3, 4, 5});
    for (auto it = S.cbegin(); it < S.cend(); ++it) {
      for (size_t i = 0; i < 5; ++i) {
        tmp->redefine(*it, S.generator(i));
        REQUIRE(S.position(tmp) == S.right(S.position(*it), i));
        tmp->redefine(S.generator(i), *it);
        REQUIRE(S.position(tmp) == S.left(S.position(*it), i));
      }
    }
    delete tmp;
    delete_gens(gens);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "047",
                          "iterator",
                          "[quick][froidure-pin][element][no-valgrind]") {
    std::vector<Element*> gens
        = {new Transformation<uint16_t>({0, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 0, 2, 3, 4, 5}),
           new Transformation<uint16_t>({4, 0, 1, 2, 3, 5}),
           new Transformation<uint16_t>({5, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 1, 2, 3, 4, 5})};
    FroidurePin<Element const*> S  = FroidurePin<Element const*>(gens);
    auto                        rg = ReportGuard(REPORT);

    REQUIRE(S.current_size() == 5);
    size_t size = S.current_size();
    for (auto it = S.cbegin(); it < S.cend(); it++) {
      size--;
      REQUIRE(S.contains(*it));
    }
    REQUIRE(size == 0);

    for (auto it = S.cbegin(); it < S.cend(); ++it) {
      size++;
      REQUIRE(S.contains(*it));
    }
    REQUIRE(size == S.current_size());
    REQUIRE(5 == S.current_size());

    S.batch_size(1024);
    S.enumerate(1000);
    REQUIRE(S.current_size() < 7776);

    size = S.current_size();
    for (auto it = S.cbegin(); it < S.cend(); it++) {
      size--;
      REQUIRE(S.contains(*it));
    }
    REQUIRE(size == 0);

    for (auto it = S.cbegin(); it < S.cend(); ++it) {
      size++;
      REQUIRE(S.contains(*it));
    }
    REQUIRE(size == S.current_size());
    REQUIRE(S.current_size() < 7776);

    REQUIRE(S.size() == 7776);
    size = S.size();
    for (auto it = S.cbegin(); it < S.cend(); it++) {
      size--;
      REQUIRE(S.contains(*it));
    }
    REQUIRE(size == 0);

    for (auto it = S.cbegin(); it < S.cend(); ++it) {
      size++;
      REQUIRE(S.contains(*it));
    }
    REQUIRE(size == S.size());
    delete_gens(gens);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "048",
                          "reverse iterator",
                          "[quick][froidure-pin][element][no-valgrind]") {
    std::vector<Element*> gens
        = {new Transformation<uint16_t>({0, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 0, 2, 3, 4, 5}),
           new Transformation<uint16_t>({4, 0, 1, 2, 3, 5}),
           new Transformation<uint16_t>({5, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 1, 2, 3, 4, 5})};
    FroidurePin<Element const*> S  = FroidurePin<Element const*>(gens);
    auto                        rg = ReportGuard(REPORT);

    REQUIRE(S.current_size() == 5);
    size_t size = S.current_size();
    for (auto it = S.crbegin(); it < S.crend(); it++) {
      size--;
      REQUIRE(S.contains(*it));
    }
    REQUIRE(size == 0);

    for (auto it = S.crbegin(); it < S.crend(); ++it) {
      size++;
      REQUIRE(S.contains(*it));
    }
    REQUIRE(size == S.current_size());
    REQUIRE(5 == S.current_size());

    S.batch_size(1024);
    S.enumerate(1000);
    REQUIRE(S.current_size() < 7776);

    size = S.current_size();
    for (auto it = S.crbegin(); it < S.crend(); it++) {
      size--;
      REQUIRE(S.contains(*it));
    }
    REQUIRE(size == 0);

    for (auto it = S.crbegin(); it < S.crend(); ++it) {
      size++;
      REQUIRE(S.contains(*it));
    }
    REQUIRE(size == S.current_size());
    REQUIRE(S.current_size() < 7776);

    REQUIRE(S.size() == 7776);
    size = S.size();
    for (auto it = S.crbegin(); it < S.crend(); it++) {
      size--;
      REQUIRE(S.contains(*it));
    }
    REQUIRE(size == 0);

    for (auto it = S.crbegin(); it < S.crend(); ++it) {
      size++;
      REQUIRE(S.contains(*it));
    }
    REQUIRE(size == S.size());
    delete_gens(gens);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "049",
                          "iterator arithmetic",
                          "[quick][froidure-pin][element][no-valgrind]") {
    std::vector<Element*> gens
        = {new Transformation<uint16_t>({0, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 0, 2, 3, 4, 5}),
           new Transformation<uint16_t>({4, 0, 1, 2, 3, 5}),
           new Transformation<uint16_t>({5, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 1, 2, 3, 4, 5})};
    FroidurePin<Element const*> S  = FroidurePin<Element const*>(gens);
    auto                        rg = ReportGuard(REPORT);

    REQUIRE(S.size() == 7776);
    auto it = S.cbegin();
    // The next line should not compile (and does not).

    for (int64_t i = 0; i < static_cast<int64_t>(S.size()); i++) {
      // *it = reinterpret_cast<Element*>(0);
      // This does not and should not compile
      REQUIRE(*(it + i) == S.at(i));
      it += i;
      REQUIRE(*it == S.at(i));
      it -= i;
      REQUIRE(*it == S.at(0));
      REQUIRE(it == S.cbegin());
      auto tmp(it);
      REQUIRE((tmp + i) - i == tmp);
      REQUIRE((i + tmp) - i == tmp);
      tmp += i;
      REQUIRE(tmp - it == i);
      REQUIRE(it - tmp == -i);
      tmp -= i;
      REQUIRE(tmp - it == 0);
      tmp -= i;
      REQUIRE(tmp - it == -i);
      REQUIRE(it - tmp == i);
    }
    for (int64_t i = S.size(); i < static_cast<int64_t>(2 * S.size()); i++) {
      it += i;
      it -= i;
      REQUIRE(*it == S.at(0));
      REQUIRE(it == S.cbegin());
      auto tmp(it);
      REQUIRE((tmp + i) - i == tmp);
      REQUIRE((i + tmp) - i == tmp);
      tmp += i;
      REQUIRE(tmp - it == i);
      REQUIRE(it - tmp == -i);
      tmp -= i;
      REQUIRE(tmp - it == 0);
      tmp -= i;
      REQUIRE(tmp - it == -i);
      REQUIRE(it - tmp == i);
    }
    delete_gens(gens);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "050",
                          "iterator sorted",
                          "[quick][froidure-pin][element][no-valgrind]") {
    std::vector<Element*> gens
        = {new Transformation<uint16_t>({0, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 0, 2, 3, 4, 5}),
           new Transformation<uint16_t>({4, 0, 1, 2, 3, 5}),
           new Transformation<uint16_t>({5, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 1, 2, 3, 4, 5})};
    FroidurePin<Element const*> S  = FroidurePin<Element const*>(gens);
    auto                        rg = ReportGuard(REPORT);

    // Calling cbegin/cend_sorted fully enumerates the semigroup
    { auto it = S.cbegin_sorted(); }
    REQUIRE(S.finished());

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
    delete_gens(gens);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "051",
                          "iterator sorted arithmetic",
                          "[quick][froidure-pin][element][no-valgrind]") {
    std::vector<Element*> gens
        = {new Transformation<uint16_t>({0, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 0, 2, 3, 4, 5}),
           new Transformation<uint16_t>({4, 0, 1, 2, 3, 5}),
           new Transformation<uint16_t>({5, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 1, 2, 3, 4, 5})};
    FroidurePin<Element const*> S  = FroidurePin<Element const*>(gens);
    auto                        rg = ReportGuard(REPORT);

    REQUIRE(S.size() == 7776);
    auto it = S.cbegin_sorted();
    // The next line should not compile (and does not).

    for (int64_t i = 0; i < static_cast<int64_t>(S.size()); i++) {
      // The next line does not and should not compile
      // *it = reinterpret_cast<Element*>(0);
      REQUIRE(*(it + i) == S.sorted_at(i));
      it += i;
      REQUIRE(*it == S.sorted_at(i));
      it -= i;
      REQUIRE(*it == S.sorted_at(0));
      REQUIRE(it == S.cbegin_sorted());
      auto tmp(it);
      REQUIRE((tmp + i) - i == tmp);
      REQUIRE((i + tmp) - i == tmp);
      tmp += i;
      REQUIRE(tmp - it == i);
      REQUIRE(it - tmp == -i);
      tmp -= i;
      REQUIRE(tmp - it == 0);
      tmp -= i;
      REQUIRE(tmp - it == -i);
      REQUIRE(it - tmp == i);
    }
    for (int64_t i = S.size(); i < static_cast<int64_t>(2 * S.size()); i++) {
      it += i;
      it -= i;
      REQUIRE(*it == S.sorted_at(0));
      REQUIRE(it == S.cbegin_sorted());
      auto tmp(it);
      REQUIRE((tmp + i) - i == tmp);
      REQUIRE((i + tmp) - i == tmp);
      tmp += i;
      REQUIRE(tmp - it == i);
      REQUIRE(it - tmp == -i);
      tmp -= i;
      REQUIRE(tmp - it == 0);
      tmp -= i;
      REQUIRE(tmp - it == -i);
      REQUIRE(it - tmp == i);
    }
    delete_gens(gens);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "052",
                          "copy [not enumerated]",
                          "[quick][froidure-pin][element][no-valgrind]") {
    std::vector<Element*> gens
        = {new Transformation<uint16_t>({0, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 0, 2, 3, 4, 5}),
           new Transformation<uint16_t>({4, 0, 1, 2, 3, 5}),
           new Transformation<uint16_t>({5, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 1, 2, 3, 4, 5})};
    FroidurePin<Element const*> S  = FroidurePin<Element const*>(gens);
    auto                        rg = ReportGuard(REPORT);

    REQUIRE(!S.started());
    REQUIRE(!S.finished());
    REQUIRE(S.current_size() == 5);
    REQUIRE(S.current_nr_rules() == 0);
    REQUIRE(S.current_max_word_length() == 1);
    REQUIRE(S.current_position(gens[1]) == 1);

    FroidurePin<Element const*> T = FroidurePin<Element const*>(S);

    REQUIRE(!T.started());
    REQUIRE(!T.finished());
    REQUIRE(T.nr_generators() == 5);
    REQUIRE(T.degree() == 6);
    REQUIRE(T.current_size() == 5);
    REQUIRE(T.current_nr_rules() == 0);
    REQUIRE(T.current_max_word_length() == 1);
    REQUIRE(T.current_position(S.generator(1)) == 1);

    REQUIRE(T.size() == 7776);
    REQUIRE(T.nr_idempotents() == 537);
    REQUIRE(T.nr_rules() == 2459);
    REQUIRE(T.started());
    REQUIRE(T.finished());
    delete_gens(gens);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "053",
                          "copy_closure [not enumerated]",
                          "[quick][froidure-pin][element][no-valgrind]") {
    std::vector<Element*> gens
        = {new Transformation<uint16_t>({0, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 0, 2, 3, 4, 5})};
    FroidurePin<Element const*> S  = FroidurePin<Element const*>(gens);
    auto                        rg = ReportGuard(REPORT);

    REQUIRE(!S.started());
    REQUIRE(!S.finished());
    REQUIRE(S.nr_generators() == 2);
    REQUIRE(S.degree() == 6);
    REQUIRE(S.current_size() == 2);
    REQUIRE(S.current_nr_rules() == 0);
    REQUIRE(S.current_max_word_length() == 1);

    std::vector<Element*> coll
        = {new Transformation<uint16_t>({4, 0, 1, 2, 3, 5}),
           new Transformation<uint16_t>({5, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 1, 2, 3, 4, 5})};

    FroidurePin<Element const*>* T = S.copy_closure(coll);
    delete_gens(coll);

    REQUIRE(T->started());
    REQUIRE(!T->finished());
    REQUIRE(T->nr_generators() == 5);
    REQUIRE(T->degree() == 6);
    REQUIRE(T->current_size() == 7719);
    REQUIRE(T->current_nr_rules() == 2418);
    REQUIRE(T->current_max_word_length() == 14);
    REQUIRE(T->current_position(S.generator(1)) == 1);

    REQUIRE(T->size() == 7776);
    REQUIRE(T->finished());
    REQUIRE(T->nr_idempotents() == 537);
    REQUIRE(T->nr_rules() == 2459);

    coll = {new Transformation<uint16_t>({6, 0, 1, 2, 3, 5, 6})};
    FroidurePin<Element const*>* U = T->copy_closure(coll);
    delete_gens(coll);

    REQUIRE(U->started());
    REQUIRE(U->finished());
    REQUIRE(U->nr_generators() == 6);
    REQUIRE(U->degree() == 7);
    REQUIRE(U->current_size() == 16807);
    REQUIRE(U->current_max_word_length() == 16);
    REQUIRE(U->nr_idempotents() == 1358);
    REQUIRE(U->nr_rules() == 7901);

    coll                           = std::vector<Element*>();
    FroidurePin<Element const*>* V = U->copy_closure(coll);
    delete_gens(coll);

    REQUIRE(V != U);
    REQUIRE(V->started());
    REQUIRE(V->finished());
    REQUIRE(V->nr_generators() == 6);
    REQUIRE(V->degree() == 7);
    REQUIRE(V->current_size() == 16807);
    REQUIRE(V->current_max_word_length() == 16);
    REQUIRE(V->nr_idempotents() == 1358);
    REQUIRE(V->nr_rules() == 7901);

    delete T;
    delete U;
    delete V;
    delete_gens(gens);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "054",
                          "copy_add_generators [not enumerated]",
                          "[quick][froidure-pin][element][no-valgrind]") {
    auto                  rg = ReportGuard(REPORT);
    std::vector<Element*> gens
        = {new Transformation<uint16_t>({0, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 0, 2, 3, 4, 5})};
    FroidurePin<Element const*> S = FroidurePin<Element const*>(gens);

    REQUIRE(!S.started());
    REQUIRE(!S.finished());
    REQUIRE(S.nr_generators() == 2);
    REQUIRE(S.degree() == 6);
    REQUIRE(S.current_size() == 2);
    REQUIRE(S.current_nr_rules() == 0);
    REQUIRE(S.current_max_word_length() == 1);

    std::vector<Element*> coll
        = {new Transformation<uint16_t>({4, 0, 1, 2, 3, 5}),
           new Transformation<uint16_t>({5, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 1, 2, 3, 4, 5})};

    FroidurePin<Element const*>* T = S.copy_add_generators(coll);
    delete_gens(coll);

    REQUIRE(!T->started());
    REQUIRE(!T->finished());
    REQUIRE(T->nr_generators() == 5);
    REQUIRE(T->degree() == 6);
    REQUIRE(T->current_size() == 5);
    REQUIRE(T->current_nr_rules() == 0);
    REQUIRE(T->current_max_word_length() == 1);
    REQUIRE(T->current_position(S.generator(1)) == 1);

    REQUIRE(T->size() == 7776);
    REQUIRE(T->finished());
    REQUIRE(T->nr_idempotents() == 537);
    REQUIRE(T->nr_rules() == 2459);

    coll = {new Transformation<uint16_t>({6, 0, 1, 2, 3, 5, 6})};
    FroidurePin<Element const*>* U = T->copy_add_generators(coll);
    delete_gens(coll);

    // REQUIRE(U->started());
    // REQUIRE(U->finished());
    REQUIRE(U->nr_generators() == 6);
    REQUIRE(U->degree() == 7);
    REQUIRE(U->current_size() == 16807);
    REQUIRE(U->current_max_word_length() == 16);
    REQUIRE(U->nr_idempotents() == 1358);
    REQUIRE(U->nr_rules() == 7901);
    REQUIRE(U->finished());

    coll                           = std::vector<Element*>();
    FroidurePin<Element const*>* V = U->copy_add_generators(coll);
    delete_gens(coll);

    REQUIRE(V != U);
    REQUIRE(V->started());
    REQUIRE(V->finished());
    REQUIRE(V->nr_generators() == 6);
    REQUIRE(V->degree() == 7);
    REQUIRE(V->current_size() == 16807);
    REQUIRE(V->current_max_word_length() == 16);
    REQUIRE(V->nr_idempotents() == 1358);
    REQUIRE(V->nr_rules() == 7901);

    delete T;
    delete U;
    delete V;
    delete_gens(gens);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "055",
                          "copy [partly enumerated]",
                          "[quick][froidure-pin][element]") {
    std::vector<Element*> gens
        = {new Transformation<uint16_t>({0, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 0, 2, 3, 4, 5}),
           new Transformation<uint16_t>({4, 0, 1, 2, 3, 5}),
           new Transformation<uint16_t>({5, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 1, 2, 3, 4, 5})};
    FroidurePin<Element const*> S  = FroidurePin<Element const*>(gens);
    auto                        rg = ReportGuard(REPORT);
    S.batch_size(1000);
    S.enumerate(1001);

    REQUIRE(S.started());
    REQUIRE(!S.finished());
    REQUIRE(S.current_size() == 1006);
    REQUIRE(S.current_nr_rules() == 70);
    REQUIRE(S.current_max_word_length() == 7);
    REQUIRE(S.current_position(gens[1]) == 1);

    FroidurePin<Element const*> T = FroidurePin<Element const*>(S);

    REQUIRE(T.started());
    REQUIRE(!T.finished());
    REQUIRE(T.nr_generators() == 5);
    REQUIRE(T.degree() == 6);
    REQUIRE(T.current_size() == 1006);
    REQUIRE(T.current_nr_rules() == 70);
    REQUIRE(T.current_max_word_length() == 7);

    Element* x = new Transformation<uint16_t>({0, 1, 2, 3, 4, 5});
    REQUIRE(T.current_position(x) == 0);
    delete x;

    x = new Transformation<uint16_t>({1, 0, 2, 3, 4, 5});
    REQUIRE(T.current_position(x) == 1);
    delete x;

    REQUIRE(T.size() == 7776);
    REQUIRE(T.nr_idempotents() == 537);
    REQUIRE(T.nr_rules() == 2459);
    REQUIRE(T.started());
    REQUIRE(T.finished());
    delete_gens(gens);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "056",
                          "copy_closure [partly enumerated]",
                          "[quick][froidure-pin][element]") {
    std::vector<Element*> gens
        = {new Transformation<uint16_t>({0, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 0, 2, 3, 4, 5}),
           new Transformation<uint16_t>({4, 0, 1, 2, 3, 5})};
    FroidurePin<Element const*> S  = FroidurePin<Element const*>(gens);
    auto                        rg = ReportGuard(REPORT);
    S.batch_size(60);
    S.enumerate(60);

    REQUIRE(S.started());
    REQUIRE(!S.finished());
    REQUIRE(S.nr_generators() == 3);
    REQUIRE(S.degree() == 6);
    REQUIRE(S.current_size() == 63);
    REQUIRE(S.current_nr_rules() == 11);
    REQUIRE(S.current_max_word_length() == 7);

    std::vector<Element*> coll
        = {new Transformation<uint16_t>({5, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 1, 2, 3, 4, 5})};

    FroidurePin<Element const*>* T = S.copy_closure(coll);
    REQUIRE(*coll[0] == *(T->generator(3)));
    REQUIRE(*coll[1] == *(T->generator(4)));
    delete_gens(coll);

    REQUIRE(T->started());
    REQUIRE(!T->finished());
    REQUIRE(T->nr_generators() == 5);
    REQUIRE(T->degree() == 6);
    REQUIRE(T->current_size() == 7719);
    REQUIRE(T->current_nr_rules() == 2418);
    REQUIRE(T->current_max_word_length() == 14);

    REQUIRE(T->size() == 7776);
    REQUIRE(T->finished());
    REQUIRE(T->nr_idempotents() == 537);
    REQUIRE(T->nr_rules() == 2459);
    delete T;
    delete_gens(gens);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "057",
                          "copy_add_generators [partly enumerated]",
                          "[quick][froidure-pin][element]") {
    std::vector<Element*> gens
        = {new Transformation<uint16_t>({0, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 0, 2, 3, 4, 5}),
           new Transformation<uint16_t>({4, 0, 1, 2, 3, 5})};
    FroidurePin<Element const*> S  = FroidurePin<Element const*>(gens);
    auto                        rg = ReportGuard(REPORT);
    S.batch_size(60);
    S.enumerate(60);

    REQUIRE(S.started());
    REQUIRE(!S.finished());
    REQUIRE(S.nr_generators() == 3);
    REQUIRE(S.degree() == 6);
    REQUIRE(S.current_size() == 63);
    REQUIRE(S.current_nr_rules() == 11);
    REQUIRE(S.current_max_word_length() == 7);

    std::vector<Element*> coll
        = {new Transformation<uint16_t>({5, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 1, 2, 3, 4, 5})};

    FroidurePin<Element const*>* T = S.copy_add_generators(coll);
    REQUIRE(*coll[0] == *(T->generator(3)));
    REQUIRE(*coll[1] == *(T->generator(4)));

    REQUIRE(!T->finished());
    REQUIRE(T->nr_generators() == 5);
    REQUIRE(T->degree() == 6);
    REQUIRE(T->current_size() == 818);
    REQUIRE(T->current_nr_rules() == 55);
    REQUIRE(T->current_max_word_length() == 7);

    REQUIRE(T->size() == 7776);
    REQUIRE(T->finished());
    REQUIRE(T->nr_idempotents() == 537);
    REQUIRE(T->nr_rules() == 2459);
    delete T;
    delete_gens(gens);
    delete_gens(coll);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "058",
                          "copy [fully enumerated]",
                          "[quick][froidure-pin][element]") {
    std::vector<Element*> gens
        = {new Transformation<uint16_t>({0, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 0, 2, 3, 4, 5}),
           new Transformation<uint16_t>({4, 0, 1, 2, 3, 5}),
           new Transformation<uint16_t>({5, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 1, 2, 3, 4, 5})};
    FroidurePin<Element const*> S  = FroidurePin<Element const*>(gens);
    auto                        rg = ReportGuard(REPORT);

    S.enumerate(8000);

    REQUIRE(S.started());
    REQUIRE(S.finished());
    REQUIRE(S.size() == 7776);
    REQUIRE(S.nr_idempotents() == 537);
    REQUIRE(S.nr_rules() == 2459);

    FroidurePin<Element const*> T = FroidurePin<Element const*>(S);

    REQUIRE(T.started());
    REQUIRE(T.finished());
    REQUIRE(T.nr_generators() == 5);
    REQUIRE(T.degree() == 6);
    REQUIRE(T.size() == 7776);
    REQUIRE(T.nr_idempotents() == 537);
    REQUIRE(T.nr_rules() == 2459);
    delete_gens(gens);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "059",
                          "copy_closure [fully enumerated]",
                          "[quick][froidure-pin][element]") {
    std::vector<Element*> gens
        = {new Transformation<uint16_t>({0, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 0, 2, 3, 4, 5}),
           new Transformation<uint16_t>({4, 0, 1, 2, 3, 5})};
    FroidurePin<Element const*> S  = FroidurePin<Element const*>(gens);
    auto                        rg = ReportGuard(REPORT);
    S.enumerate(121);

    REQUIRE(S.started());
    REQUIRE(S.finished());
    REQUIRE(S.nr_generators() == 3);
    REQUIRE(S.degree() == 6);
    REQUIRE(S.current_size() == 120);
    REQUIRE(S.current_nr_rules() == 25);
    REQUIRE(S.current_max_word_length() == 11);

    std::vector<Element*> coll
        = {new Transformation<uint16_t>({5, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 1, 2, 3, 4, 5})};

    FroidurePin<Element const*>* T = S.copy_closure(coll);
    REQUIRE(*coll[0] == *(T->generator(3)));
    REQUIRE(*coll[1] == *(T->generator(4)));

    REQUIRE(T->started());
    REQUIRE(!T->finished());
    REQUIRE(T->nr_generators() == 5);
    REQUIRE(T->degree() == 6);
    REQUIRE(T->current_size() == 7719);
    REQUIRE(T->current_nr_rules() == 2418);
    REQUIRE(T->current_max_word_length() == 14);

    REQUIRE(T->size() == 7776);
    REQUIRE(T->finished());
    REQUIRE(T->nr_idempotents() == 537);
    REQUIRE(T->nr_rules() == 2459);
    delete T;
    delete_gens(gens);
    delete_gens(coll);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "060",
                          "copy_add_generators [fully enumerated]",
                          "[quick][froidure-pin][element]") {
    std::vector<Element*> gens
        = {new Transformation<uint16_t>({0, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 0, 2, 3, 4, 5}),
           new Transformation<uint16_t>({4, 0, 1, 2, 3, 5})};
    FroidurePin<Element const*> S  = FroidurePin<Element const*>(gens);
    auto                        rg = ReportGuard(REPORT);
    S.enumerate(121);

    REQUIRE(S.started());
    REQUIRE(S.finished());
    REQUIRE(S.nr_generators() == 3);
    REQUIRE(S.degree() == 6);
    REQUIRE(S.current_size() == 120);
    REQUIRE(S.current_nr_rules() == 25);
    REQUIRE(S.current_max_word_length() == 11);

    std::vector<Element*> coll
        = {new Transformation<uint16_t>({5, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 1, 2, 3, 4, 5})};

    FroidurePin<Element const*>* T = S.copy_add_generators(coll);
    REQUIRE(*coll[0] == *(T->generator(3)));
    REQUIRE(*coll[1] == *(T->generator(4)));

    REQUIRE(!T->finished());
    REQUIRE(T->nr_generators() == 5);
    REQUIRE(T->degree() == 6);
    REQUIRE(T->current_size() == 6842);
    REQUIRE(T->current_nr_rules() == 1970);
    REQUIRE(T->current_max_word_length() == 12);

    REQUIRE(T->size() == 7776);
    REQUIRE(T->finished());
    REQUIRE(T->nr_idempotents() == 537);
    REQUIRE(T->nr_rules() == 2459);
    delete T;
    delete_gens(gens);
    delete_gens(coll);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "061",
                          "relations [duplicate gens]",
                          "[quick][froidure-pin][element]") {
    std::vector<Element*> gens
        = {new Transformation<uint16_t>({0, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({0, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 0, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 0, 2, 3, 4, 5}),
           new Transformation<uint16_t>({4, 0, 1, 2, 3, 5})};
    FroidurePin<Element const*> S  = FroidurePin<Element const*>(gens);
    auto                        rg = ReportGuard(REPORT);

    std::vector<size_t> result;
    S.next_relation(result);
    REQUIRE(result.size() == 2);
    REQUIRE(result[0] == 1);
    REQUIRE(result[1] == 0);

    S.next_relation(result);
    REQUIRE(result.size() == 2);
    REQUIRE(result[0] == 3);
    REQUIRE(result[1] == 2);

    S.next_relation(result);
    size_t nr = 2;
    while (!result.empty()) {
      S.next_relation(result);
      nr++;
    }
    REQUIRE(S.nr_rules() == nr);

    S.next_relation(result);
    REQUIRE(result.empty());
    delete_gens(gens);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "062",
                          "relations",
                          "[quick][froidure-pin][element]") {
    std::vector<Element*> gens
        = {new Transformation<uint16_t>({0, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 0, 2, 3, 4, 5}),
           new Transformation<uint16_t>({4, 0, 1, 2, 3, 5}),
           new Transformation<uint16_t>({5, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 1, 2, 3, 4, 5})};
    FroidurePin<Element const*> S  = FroidurePin<Element const*>(gens);
    auto                        rg = ReportGuard(REPORT);

    std::vector<size_t> result;
    S.next_relation(result);
    size_t nr = 0;
    while (!result.empty()) {
      word_type lhs, rhs;
      S.factorisation(lhs, result[0]);
      lhs.push_back(result[1]);
      S.factorisation(rhs, result[2]);

      REQUIRE(evaluate_reduct(S, lhs) == evaluate_reduct(S, rhs));
      S.next_relation(result);
      nr++;
    }
    REQUIRE(S.nr_rules() == nr);

    S.reset_next_relation();
    S.next_relation(result);
    nr = 0;

    while (!result.empty()) {
      word_type lhs, rhs;
      S.factorisation(lhs, result[0]);
      lhs.push_back(result[1]);
      S.factorisation(rhs, result[2]);

      REQUIRE(evaluate_reduct(S, lhs) == evaluate_reduct(S, rhs));
      REQUIRE(evaluate_reduct(S, lhs) == evaluate_reduct(S, rhs));
      S.next_relation(result);
      nr++;
    }

    REQUIRE(S.nr_rules() == nr);
    delete_gens(gens);
  }
  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "063",
                          "relations [copy_closure, duplicate gens]",
                          "[quick][froidure-pin][element]") {
    std::vector<Element*> gens
        = {new Transformation<uint16_t>({0, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({0, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 0, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 0, 2, 3, 4, 5}),
           new Transformation<uint16_t>({4, 0, 1, 2, 3, 5})};
    FroidurePin<Element const*> S  = FroidurePin<Element const*>(gens);
    auto                        rg = ReportGuard(REPORT);

    S.run();
    REQUIRE(S.started());
    REQUIRE(S.finished());
    REQUIRE(S.nr_generators() == 5);
    REQUIRE(S.degree() == 6);
    REQUIRE(S.current_size() == 120);
    REQUIRE(S.size() == 120);
    REQUIRE(S.current_nr_rules() == 33);
    REQUIRE(S.nr_rules() == 33);
    REQUIRE(S.current_max_word_length() == 11);

    std::vector<Element*> coll
        = {new Transformation<uint16_t>({5, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({0, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 0, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 1, 2, 3, 4, 5})};

    FroidurePin<Element const*>* T = S.copy_closure(coll);

    REQUIRE(T->size() == 7776);
    REQUIRE(T->finished());
    REQUIRE(T->nr_idempotents() == 537);
    delete T;
    delete_gens(gens);
    delete_gens(coll);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "064",
                          "relations [copy_add_generators, duplicate gens]",
                          "[quick][froidure-pin][element]") {
    std::vector<Element*> gens
        = {new Transformation<uint16_t>({0, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({0, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 0, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 0, 2, 3, 4, 5}),
           new Transformation<uint16_t>({4, 0, 1, 2, 3, 5})};
    FroidurePin<Element const*> S  = FroidurePin<Element const*>(gens);
    auto                        rg = ReportGuard(REPORT);

    S.run();
    REQUIRE(S.started());
    REQUIRE(S.finished());
    REQUIRE(S.nr_generators() == 5);
    REQUIRE(S.degree() == 6);
    REQUIRE(S.current_size() == 120);
    REQUIRE(S.size() == 120);
    REQUIRE(S.current_nr_rules() == 33);
    REQUIRE(S.nr_rules() == 33);
    REQUIRE(S.current_max_word_length() == 11);

    std::vector<Element*> coll
        = {new Transformation<uint16_t>({5, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({0, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 0, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 1, 2, 3, 4, 5})};

    FroidurePin<Element const*>* T = S.copy_add_generators(coll);

    REQUIRE(T->size() == 7776);
    REQUIRE(T->finished());
    REQUIRE(T->nr_idempotents() == 537);
    delete T;
    delete_gens(gens);
    delete_gens(coll);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "065",
                          "relations [from copy, not enumerated]",
                          "[quick][froidure-pin][element]") {
    std::vector<Element*> gens
        = {new Transformation<uint16_t>({0, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 0, 2, 3, 4, 5}),
           new Transformation<uint16_t>({4, 0, 1, 2, 3, 5}),
           new Transformation<uint16_t>({5, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 1, 2, 3, 4, 5})};
    FroidurePin<Element const*> S  = FroidurePin<Element const*>(gens);
    auto                        rg = ReportGuard(REPORT);

    FroidurePin<Element const*> T = FroidurePin<Element const*>(S);
    REQUIRE(T.nr_rules() == S.nr_rules());

    std::vector<size_t> result;
    T.next_relation(result);
    size_t nr = 0;
    while (!result.empty()) {
      word_type lhs, rhs;
      T.factorisation(lhs, result[0]);
      lhs.push_back(result[1]);
      T.factorisation(rhs, result[2]);

      REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
      REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
      T.next_relation(result);
      nr++;
    }
    REQUIRE(T.nr_rules() == nr);

    T.reset_next_relation();
    T.next_relation(result);
    nr = 0;

    while (!result.empty()) {
      word_type lhs, rhs;
      T.factorisation(lhs, result[0]);
      lhs.push_back(result[1]);
      T.factorisation(rhs, result[2]);

      REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
      REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
      T.next_relation(result);
      nr++;
    }
    REQUIRE(T.nr_rules() == nr);
    delete_gens(gens);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "066",
                          "relations [from copy, partly enumerated]",
                          "[quick][froidure-pin][element]") {
    std::vector<Element*> gens
        = {new Transformation<uint16_t>({0, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 0, 2, 3, 4, 5}),
           new Transformation<uint16_t>({4, 0, 1, 2, 3, 5}),
           new Transformation<uint16_t>({5, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 1, 2, 3, 4, 5})};
    FroidurePin<Element const*> S  = FroidurePin<Element const*>(gens);
    auto                        rg = ReportGuard(REPORT);

    S.batch_size(1023);
    S.enumerate(1000);

    FroidurePin<Element const*> T = FroidurePin<Element const*>(S);
    REQUIRE(T.nr_rules() == S.nr_rules());

    std::vector<size_t> result;
    T.next_relation(result);
    size_t nr = 0;
    while (!result.empty()) {
      word_type lhs, rhs;
      T.factorisation(lhs, result[0]);
      lhs.push_back(result[1]);
      T.factorisation(rhs, result[2]);

      REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
      REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
      T.next_relation(result);
      nr++;
    }
    REQUIRE(T.nr_rules() == nr);

    T.reset_next_relation();
    T.next_relation(result);
    nr = 0;

    while (!result.empty()) {
      word_type lhs, rhs;
      T.factorisation(lhs, result[0]);
      lhs.push_back(result[1]);
      T.factorisation(rhs, result[2]);

      REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
      REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
      T.next_relation(result);
      nr++;
    }
    REQUIRE(T.nr_rules() == nr);
    delete_gens(gens);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "067",
                          "relations [from copy, fully enumerated]",
                          "[quick][froidure-pin][element]") {
    std::vector<Element*> gens
        = {new Transformation<uint16_t>({0, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 0, 2, 3, 4, 5}),
           new Transformation<uint16_t>({4, 0, 1, 2, 3, 5}),
           new Transformation<uint16_t>({5, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 1, 2, 3, 4, 5})};
    FroidurePin<Element const*> S  = FroidurePin<Element const*>(gens);
    auto                        rg = ReportGuard(REPORT);

    S.enumerate(8000);

    FroidurePin<Element const*> T = FroidurePin<Element const*>(S);
    REQUIRE(T.nr_rules() == S.nr_rules());

    std::vector<size_t> result;
    T.next_relation(result);
    size_t nr = 0;
    while (!result.empty()) {
      word_type lhs, rhs;
      T.factorisation(lhs, result[0]);
      lhs.push_back(result[1]);
      T.factorisation(rhs, result[2]);

      REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
      REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
      T.next_relation(result);
      nr++;
    }
    REQUIRE(T.nr_rules() == nr);

    T.reset_next_relation();
    T.next_relation(result);
    nr = 0;

    while (!result.empty()) {
      word_type lhs, rhs;
      T.factorisation(lhs, result[0]);
      lhs.push_back(result[1]);
      T.factorisation(rhs, result[2]);

      REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
      REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
      T.next_relation(result);
      nr++;
    }
    REQUIRE(T.nr_rules() == nr);
    delete_gens(gens);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "068",
                          "relations [from copy_closure, not enumerated]",
                          "[quick][froidure-pin][element]") {
    std::vector<Element*> gens
        = {new Transformation<uint16_t>({0, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 0, 2, 3, 4, 5}),
           new Transformation<uint16_t>({4, 0, 1, 2, 3, 5})};
    FroidurePin<Element const*> S  = FroidurePin<Element const*>(gens);
    auto                        rg = ReportGuard(REPORT);

    REQUIRE(!S.started());
    REQUIRE(!S.finished());

    std::vector<Element*> coll
        = {new Transformation<uint16_t>({5, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 1, 2, 3, 4, 5})};

    FroidurePin<Element const*>* T = S.copy_closure(coll);
    REQUIRE(*coll[0] == *(T->generator(3)));
    REQUIRE(*coll[1] == *(T->generator(4)));
    delete_gens(coll);

    std::vector<size_t> result;
    T->next_relation(result);
    size_t nr = 0;
    while (!result.empty()) {
      word_type lhs, rhs;
      T->factorisation(lhs, result[0]);
      lhs.push_back(result[1]);
      T->factorisation(rhs, result[2]);

      REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
      REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
      T->next_relation(result);
      nr++;
    }
    REQUIRE(T->nr_rules() == nr);
    REQUIRE(2459 == nr);

    T->reset_next_relation();
    T->next_relation(result);
    nr = 0;

    while (!result.empty()) {
      word_type lhs, rhs;
      T->factorisation(lhs, result[0]);
      lhs.push_back(result[1]);
      T->factorisation(rhs, result[2]);

      REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
      REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
      T->next_relation(result);
      nr++;
    }
    REQUIRE(T->nr_rules() == nr);
    REQUIRE(2459 == nr);
    delete T;
    delete_gens(gens);
  }

  LIBSEMIGROUPS_TEST_CASE(
      "FroidurePin",
      "069",
      "relations [from copy_add_generators, not enumerated]",
      "[quick][froidure-pin][element]") {
    std::vector<Element*> gens
        = {new Transformation<uint16_t>({0, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 0, 2, 3, 4, 5}),
           new Transformation<uint16_t>({4, 0, 1, 2, 3, 5})};
    FroidurePin<Element const*> S  = FroidurePin<Element const*>(gens);
    auto                        rg = ReportGuard(REPORT);

    REQUIRE(!S.started());
    REQUIRE(!S.finished());

    std::vector<Element*> coll
        = {new Transformation<uint16_t>({5, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 1, 2, 3, 4, 5})};

    FroidurePin<Element const*>* T = S.copy_add_generators(coll);
    REQUIRE(*coll[0] == *(T->generator(3)));
    REQUIRE(*coll[1] == *(T->generator(4)));
    delete_gens(coll);

    std::vector<size_t> result;
    T->next_relation(result);
    size_t nr = 0;
    while (!result.empty()) {
      word_type lhs, rhs;
      T->factorisation(lhs, result[0]);
      lhs.push_back(result[1]);
      T->factorisation(rhs, result[2]);

      REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
      REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
      T->next_relation(result);
      nr++;
    }
    REQUIRE(T->nr_rules() == nr);
    REQUIRE(2459 == nr);

    T->reset_next_relation();
    T->next_relation(result);
    nr = 0;

    while (!result.empty()) {
      word_type lhs, rhs;
      T->factorisation(lhs, result[0]);
      lhs.push_back(result[1]);
      T->factorisation(rhs, result[2]);

      REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
      REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
      T->next_relation(result);
      nr++;
    }
    REQUIRE(T->nr_rules() == nr);
    REQUIRE(2459 == nr);
    delete T;
    delete_gens(gens);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "070",
                          "relations [from copy_closure, partly enumerated]",
                          "[quick][froidure-pin][element]") {
    std::vector<Element*> gens
        = {new Transformation<uint16_t>({0, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 0, 2, 3, 4, 5}),
           new Transformation<uint16_t>({4, 0, 1, 2, 3, 5})};
    FroidurePin<Element const*> S  = FroidurePin<Element const*>(gens);
    auto                        rg = ReportGuard(REPORT);
    S.batch_size(100);

    S.enumerate(10);

    REQUIRE(S.started());
    REQUIRE(!S.finished());

    std::vector<Element*> coll
        = {new Transformation<uint16_t>({5, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 1, 2, 3, 4, 5})};

    FroidurePin<Element const*>* T = S.copy_closure(coll);
    delete_gens(coll);

    std::vector<size_t> result;
    T->next_relation(result);
    size_t nr = 0;
    while (!result.empty()) {
      word_type lhs, rhs;
      T->factorisation(lhs, result[0]);
      lhs.push_back(result[1]);
      T->factorisation(rhs, result[2]);

      REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
      REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
      T->next_relation(result);
      nr++;
    }
    REQUIRE(T->nr_rules() == nr);
    REQUIRE(2459 == nr);

    T->reset_next_relation();
    T->next_relation(result);
    nr = 0;

    while (!result.empty()) {
      word_type lhs, rhs;
      T->factorisation(lhs, result[0]);
      lhs.push_back(result[1]);
      T->factorisation(rhs, result[2]);

      REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
      REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
      T->next_relation(result);
      nr++;
    }
    REQUIRE(T->nr_rules() == nr);
    REQUIRE(2459 == nr);
    delete T;
    delete_gens(gens);
  }

  LIBSEMIGROUPS_TEST_CASE(
      "FroidurePin",
      "071",
      "relations [from copy_add_generators, partly enumerated]",
      "[quick][froidure-pin][element]") {
    std::vector<Element*> gens
        = {new Transformation<uint16_t>({0, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 0, 2, 3, 4, 5}),
           new Transformation<uint16_t>({4, 0, 1, 2, 3, 5})};
    FroidurePin<Element const*> S  = FroidurePin<Element const*>(gens);
    auto                        rg = ReportGuard(REPORT);
    S.batch_size(100);

    S.enumerate(10);

    REQUIRE(S.started());
    REQUIRE(!S.finished());

    std::vector<Element*> coll
        = {new Transformation<uint16_t>({5, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 1, 2, 3, 4, 5})};

    FroidurePin<Element const*>* T = S.copy_add_generators(coll);
    delete_gens(coll);

    std::vector<size_t> result;
    T->next_relation(result);
    size_t nr = 0;
    while (!result.empty()) {
      word_type lhs, rhs;
      T->factorisation(lhs, result[0]);
      lhs.push_back(result[1]);
      T->factorisation(rhs, result[2]);

      REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
      REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
      T->next_relation(result);
      nr++;
    }
    REQUIRE(T->nr_rules() == nr);
    REQUIRE(2459 == nr);

    T->reset_next_relation();
    T->next_relation(result);
    nr = 0;

    while (!result.empty()) {
      word_type lhs, rhs;
      T->factorisation(lhs, result[0]);
      lhs.push_back(result[1]);
      T->factorisation(rhs, result[2]);

      REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
      REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
      T->next_relation(result);
      nr++;
    }
    REQUIRE(T->nr_rules() == nr);
    REQUIRE(2459 == nr);
    delete T;
    delete_gens(gens);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "072",
                          "relations [from copy_closure, fully enumerated]",
                          "[quick][froidure-pin][element]") {
    std::vector<Element*> gens
        = {new Transformation<uint16_t>({0, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 0, 2, 3, 4, 5}),
           new Transformation<uint16_t>({4, 0, 1, 2, 3, 5})};
    FroidurePin<Element const*> S  = FroidurePin<Element const*>(gens);
    auto                        rg = ReportGuard(REPORT);

    S.enumerate(8000);

    REQUIRE(S.started());
    REQUIRE(S.finished());

    std::vector<Element*> coll
        = {new Transformation<uint16_t>({5, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 1, 2, 3, 4, 5})};

    FroidurePin<Element const*>* T = S.copy_closure(coll);
    delete_gens(coll);

    std::vector<size_t> result;
    T->next_relation(result);
    size_t nr = 0;
    while (!result.empty()) {
      REQUIRE(result.size() == 3);  // there are no duplicate gens
      word_type lhs, rhs;
      T->factorisation(lhs, result[0]);
      lhs.push_back(result[1]);
      T->factorisation(rhs, result[2]);

      REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
      REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
      T->next_relation(result);
      nr++;
    }
    REQUIRE(T->nr_rules() == nr);
    REQUIRE(2459 == nr);

    T->reset_next_relation();
    T->next_relation(result);
    nr = 0;

    while (!result.empty()) {
      REQUIRE(result.size() == 3);
      word_type lhs, rhs;
      T->factorisation(lhs, result[0]);
      lhs.push_back(result[1]);
      T->factorisation(rhs, result[2]);

      REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
      REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
      T->next_relation(result);
      nr++;
    }
    REQUIRE(T->nr_rules() == nr);
    REQUIRE(2459 == nr);
    delete T;
    delete_gens(gens);
  }

  LIBSEMIGROUPS_TEST_CASE(
      "FroidurePin",
      "073",
      "relations [from copy_add_generators, fully enumerated]",
      "[quick][froidure-pin][element]") {
    std::vector<Element*> gens
        = {new Transformation<uint16_t>({0, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 0, 2, 3, 4, 5}),
           new Transformation<uint16_t>({4, 0, 1, 2, 3, 5})};
    FroidurePin<Element const*> S  = FroidurePin<Element const*>(gens);
    auto                        rg = ReportGuard(REPORT);

    S.enumerate(8000);

    REQUIRE(S.started());
    REQUIRE(S.finished());

    std::vector<Element*> coll
        = {new Transformation<uint16_t>({5, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 1, 2, 3, 4, 5})};

    FroidurePin<Element const*>* T = S.copy_add_generators(coll);
    delete_gens(coll);

    std::vector<size_t> result;
    T->next_relation(result);
    size_t nr = 0;
    while (!result.empty()) {
      REQUIRE(result.size() == 3);  // there are no duplicate gens
      word_type lhs, rhs;
      T->factorisation(lhs, result[0]);
      lhs.push_back(result[1]);
      T->factorisation(rhs, result[2]);

      REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
      REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
      T->next_relation(result);
      nr++;
    }
    REQUIRE(T->nr_rules() == nr);
    REQUIRE(2459 == nr);

    T->reset_next_relation();
    T->next_relation(result);
    nr = 0;

    while (!result.empty()) {
      REQUIRE(result.size() == 3);
      word_type lhs, rhs;
      T->factorisation(lhs, result[0]);
      lhs.push_back(result[1]);
      T->factorisation(rhs, result[2]);

      REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
      REQUIRE(evaluate_reduct(T, lhs) == evaluate_reduct(T, rhs));
      T->next_relation(result);
      nr++;
    }
    REQUIRE(T->nr_rules() == nr);
    REQUIRE(2459 == nr);
    delete T;
    delete_gens(gens);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "074",
                          "add_generators [duplicate generators]",
                          "[quick][froidure-pin][element]") {
    std::vector<Element*> gens
        = {new Transformation<uint16_t>({0, 1, 0, 3, 4, 5}),
           new Transformation<uint16_t>({0, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({0, 1, 3, 5, 5, 4}),
           new Transformation<uint16_t>({1, 0, 2, 4, 4, 5}),
           new Transformation<uint16_t>({4, 3, 3, 1, 0, 5}),
           new Transformation<uint16_t>({4, 3, 5, 1, 0, 5}),
           new Transformation<uint16_t>({5, 5, 2, 3, 4, 0})};

    FroidurePin<Element const*> S
        = FroidurePin<Element const*>({gens[0], gens[0]});
    auto rg = ReportGuard(REPORT);

    REQUIRE(S.size() == 1);
    REQUIRE(S.nr_generators() == 2);

    S.add_generators(std::vector<Element*>({}));
    REQUIRE(S.size() == 1);
    REQUIRE(S.nr_generators() == 2);

    S.add_generators({gens[0]});
    REQUIRE(S.size() == 1);
    REQUIRE(S.nr_generators() == 3);

    S.add_generators({gens[1]});
    REQUIRE(S.size() == 2);
    REQUIRE(S.nr_generators() == 4);

    S.add_generators({gens[2]});
    REQUIRE(S.size() == 7);
    REQUIRE(S.nr_generators() == 5);

    S.add_generators({gens[3]});
    REQUIRE(S.size() == 18);
    REQUIRE(S.nr_generators() == 6);

    S.add_generators({gens[4]});
    REQUIRE(S.size() == 87);
    REQUIRE(S.nr_generators() == 7);

    S.add_generators({gens[5]});
    REQUIRE(S.size() == 97);
    REQUIRE(S.nr_generators() == 8);

    S.add_generators({gens[6]});
    REQUIRE(S.size() == 119);
    REQUIRE(S.nr_generators() == 9);
    REQUIRE(S.nr_rules() == 213);

    gens[0]->redefine(gens[3], gens[4]);
    S.add_generators({gens[0]});
    REQUIRE(S.size() == 119);
    REQUIRE(S.nr_generators() == 10);
    REQUIRE(S.nr_rules() == 267);

    REQUIRE(S.letter_to_pos(0) == 0);
    REQUIRE(S.letter_to_pos(1) == 0);
    REQUIRE(S.letter_to_pos(2) == 0);
    REQUIRE(S.letter_to_pos(3) == 1);
    REQUIRE(S.letter_to_pos(4) == 2);
    REQUIRE(S.letter_to_pos(5) == 7);
    REQUIRE(S.letter_to_pos(6) == 18);
    REQUIRE(S.letter_to_pos(7) == 87);
    REQUIRE(S.letter_to_pos(8) == 97);
    REQUIRE(S.letter_to_pos(9) == 21);
    delete_gens(gens);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "075",
                          "add_generators [incremental 1]",
                          "[quick][froidure-pin][element]") {
    std::vector<Element*> gens
        = {new Transformation<uint16_t>({0, 1, 0, 3, 4, 5}),
           new Transformation<uint16_t>({0, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({0, 1, 3, 5, 5, 4}),
           new Transformation<uint16_t>({1, 0, 2, 4, 4, 5}),
           new Transformation<uint16_t>({4, 3, 3, 1, 0, 5}),
           new Transformation<uint16_t>({4, 3, 5, 1, 0, 5}),
           new Transformation<uint16_t>({5, 5, 2, 3, 4, 0})};

    FroidurePin<Element const*> S
        = FroidurePin<Element const*>({gens[0], gens[0]});
    auto rg = ReportGuard(REPORT);
    S.add_generators(std::vector<Element*>({}));
    S.add_generators({gens[0]});
    S.add_generators({gens[1]});
    S.add_generators({gens[2]});
    S.add_generators({gens[3]});
    REQUIRE(S.size() == 18);
    REQUIRE(S.nr_generators() == 6);

    S.add_generators({gens[4]});
    S.add_generators({gens[5]});
    REQUIRE(S.size() == 97);
    REQUIRE(S.nr_generators() == 8);
    REQUIRE(S.nr_rules() == 126);

    S.add_generators({gens[4], gens[5]});
    S.add_generators({gens[5]});
    S.add_generators({gens[6]});
    S.add_generators({gens[0], gens[0]});
    REQUIRE(S.size() == 119);
    REQUIRE(S.nr_generators() == 14);
    REQUIRE(S.nr_rules() == 253);
    delete_gens(gens);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "076",
                          "add_generators [incremental 2]",
                          "[quick][froidure-pin][element]") {
    std::vector<Element*> gens
        = {new Transformation<uint16_t>({0, 1, 0, 3, 4, 5}),
           new Transformation<uint16_t>({0, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({0, 1, 3, 5, 5, 4}),
           new Transformation<uint16_t>({1, 0, 2, 4, 4, 5}),
           new Transformation<uint16_t>({4, 3, 3, 1, 0, 5}),
           new Transformation<uint16_t>({4, 3, 5, 1, 0, 5}),
           new Transformation<uint16_t>({5, 5, 2, 3, 4, 0})};

    FroidurePin<Element const*> T  = FroidurePin<Element const*>(gens);
    auto                        rg = ReportGuard(REPORT);
    REQUIRE(T.size() == 119);

    FroidurePin<Element const*> S
        = FroidurePin<Element const*>({gens[0], gens[0]});
    S.add_generators(std::vector<Element*>({}));
    S.add_generators({gens[0]});
    S.run();
    S.add_generators({gens[1]});
    S.run();
    S.add_generators({gens[2]});
    S.run();
    REQUIRE(S.current_size() == 7);
    S.add_generators({gens[3], gens[4], gens[5]});
    REQUIRE(S.nr_generators() == 8);
    REQUIRE(S.letter_to_pos(5) == 7);
    REQUIRE(S.letter_to_pos(6) == 8);
    REQUIRE(S.letter_to_pos(7) == 9);
    REQUIRE(S.current_size() == 55);

    S.add_generators({S.at(44)});
    REQUIRE(S.nr_generators() == 9);
    REQUIRE(S.current_size() == 73);
    REQUIRE(S.size() == 97);

    S.add_generators({S.at(75)});
    REQUIRE(S.nr_generators() == 10);
    REQUIRE(S.current_size() == 97);
    REQUIRE(S.size() == 97);

    S.add_generators({gens[6]});
    REQUIRE(S.nr_generators() == 11);
    REQUIRE(S.size() == 119);
    delete_gens(gens);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "077",
                          "closure [duplicate generators]",
                          "[quick][froidure-pin][element]") {
    std::vector<Element*> gens
        = {new Transformation<uint16_t>({0, 1, 0, 3, 4, 5}),
           new Transformation<uint16_t>({0, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({0, 1, 3, 5, 5, 4}),
           new Transformation<uint16_t>({1, 0, 2, 4, 4, 5}),
           new Transformation<uint16_t>({4, 3, 3, 1, 0, 5}),
           new Transformation<uint16_t>({4, 3, 5, 1, 0, 5}),
           new Transformation<uint16_t>({5, 5, 2, 3, 4, 0})};

    FroidurePin<Element const*> S
        = FroidurePin<Element const*>({gens[0], gens[0]});
    auto rg = ReportGuard(REPORT);

    REQUIRE(S.size() == 1);
    REQUIRE(S.nr_generators() == 2);

    S.closure(std::vector<Element*>({}));
    REQUIRE(S.size() == 1);
    REQUIRE(S.nr_generators() == 2);

    S.closure({gens[0]});
    REQUIRE(S.size() == 1);
    REQUIRE(S.nr_generators() == 2);

    S.closure({gens[1]});
    REQUIRE(S.size() == 2);
    REQUIRE(S.nr_generators() == 3);

    S.closure({gens[2]});
    REQUIRE(S.size() == 7);
    REQUIRE(S.nr_generators() == 4);

    S.closure({gens[3]});
    REQUIRE(S.size() == 18);
    REQUIRE(S.nr_generators() == 5);

    S.closure({gens[4]});
    REQUIRE(S.size() == 87);
    REQUIRE(S.nr_generators() == 6);

    S.closure({gens[5]});
    REQUIRE(S.size() == 97);
    REQUIRE(S.nr_generators() == 7);

    S.closure({gens[6]});
    REQUIRE(S.size() == 119);
    REQUIRE(S.nr_generators() == 8);
    delete_gens(gens);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "078",
                          "closure ",
                          "[quick][froidure-pin][element]") {
    std::vector<Element*> gens = {new Transformation<uint16_t>({0, 0, 0}),
                                  new Transformation<uint16_t>({0, 0, 1}),
                                  new Transformation<uint16_t>({0, 0, 2}),
                                  new Transformation<uint16_t>({0, 1, 0}),
                                  new Transformation<uint16_t>({0, 1, 1}),
                                  new Transformation<uint16_t>({0, 1, 2}),
                                  new Transformation<uint16_t>({0, 2, 0}),
                                  new Transformation<uint16_t>({0, 2, 1}),
                                  new Transformation<uint16_t>({0, 2, 2}),
                                  new Transformation<uint16_t>({1, 0, 0}),
                                  new Transformation<uint16_t>({1, 0, 1}),
                                  new Transformation<uint16_t>({1, 0, 2}),
                                  new Transformation<uint16_t>({1, 1, 0}),
                                  new Transformation<uint16_t>({1, 1, 1}),
                                  new Transformation<uint16_t>({1, 1, 2}),
                                  new Transformation<uint16_t>({1, 2, 0}),
                                  new Transformation<uint16_t>({1, 2, 1}),
                                  new Transformation<uint16_t>({1, 2, 2}),
                                  new Transformation<uint16_t>({2, 0, 0}),
                                  new Transformation<uint16_t>({2, 0, 1}),
                                  new Transformation<uint16_t>({2, 0, 2}),
                                  new Transformation<uint16_t>({2, 1, 0}),
                                  new Transformation<uint16_t>({2, 1, 1}),
                                  new Transformation<uint16_t>({2, 1, 2}),
                                  new Transformation<uint16_t>({2, 2, 0}),
                                  new Transformation<uint16_t>({2, 2, 1}),
                                  new Transformation<uint16_t>({2, 2, 2})};

    FroidurePin<Element const*> S  = FroidurePin<Element const*>({gens[0]});
    auto                        rg = ReportGuard(REPORT);

    S.closure(gens);
    REQUIRE(S.size() == 27);
    REQUIRE(S.nr_generators() == 10);
    delete_gens(gens);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "079",
                          "factorisation ",
                          "[quick][froidure-pin][element]") {
    std::vector<Element*> gens
        = {new Transformation<uint16_t>({1, 1, 4, 5, 4, 5}),
           new Transformation<uint16_t>({2, 3, 2, 3, 5, 5})};

    FroidurePin<Element const*> S  = FroidurePin<Element const*>(gens);
    auto                        rg = ReportGuard(REPORT);

    REQUIRE(S.factorisation(2) == word_type({0, 1}));
    delete_gens(gens);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "080",
                          "my favourite example with reserve",
                          "[standard][froidure-pin][element]") {
    std::vector<Element*> gens
        = {new Transformation<uint_fast8_t>({1, 7, 2, 6, 0, 4, 1, 5}),
           new Transformation<uint_fast8_t>({2, 4, 6, 1, 4, 5, 2, 7}),
           new Transformation<uint_fast8_t>({3, 0, 7, 2, 4, 6, 2, 4}),
           new Transformation<uint_fast8_t>({3, 2, 3, 4, 5, 3, 0, 1}),
           new Transformation<uint_fast8_t>({4, 3, 7, 7, 4, 5, 0, 4}),
           new Transformation<uint_fast8_t>({5, 6, 3, 0, 3, 0, 5, 1}),
           new Transformation<uint_fast8_t>({6, 0, 1, 1, 1, 6, 3, 4}),
           new Transformation<uint_fast8_t>({7, 7, 4, 0, 6, 4, 1, 7})};
    FroidurePin<Element const*> S(gens);
    S.reserve(597369);
    auto rg = ReportGuard(REPORT);

    REQUIRE(S.size() == 597369);
    delete_gens(gens);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "081",
                          "minimal_factorisation ",
                          "[quick][froidure-pin][element]") {
    std::vector<Element*> gens
        = {new Transformation<uint16_t>({1, 1, 4, 5, 4, 5}),
           new Transformation<uint16_t>({2, 3, 2, 3, 5, 5})};

    FroidurePin<Element const*> S  = FroidurePin<Element const*>(gens);
    auto                        rg = ReportGuard(REPORT);

    REQUIRE(S.minimal_factorisation(gens[0]) == word_type({0}));

    REQUIRE(S.factorisation(gens[0]) == word_type({0}));

    Element* x = new Transformation<uint16_t>({4, 1, 4, 1, 4, 5});
    REQUIRE_THROWS_AS(S.minimal_factorisation(x), LibsemigroupsException);
    delete x;

    REQUIRE_THROWS_AS(S.minimal_factorisation(10000000),
                      LibsemigroupsException);
    delete_gens(gens);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "082",
                          "batch_size (for an extremely large value)",
                          "[quick][froidure-pin][element]") {
    std::vector<Element*> gens
        = {new Transformation<uint16_t>({1, 1, 4, 5, 4, 5}),
           new Transformation<uint16_t>({2, 3, 2, 3, 5, 5})};
    FroidurePin<Element const*> S = FroidurePin<Element const*>(gens);

    auto rg = ReportGuard(REPORT);
    S.batch_size(LIMIT_MAX);
    S.run();

    REQUIRE(S.size() == 5);
    delete_gens(gens);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "083",
                          "my favourite example without reserve",
                          "[standard][froidure-pin][element]") {
    std::vector<Element*> gens
        = {new Transformation<uint_fast8_t>({1, 7, 2, 6, 0, 4, 1, 5}),
           new Transformation<uint_fast8_t>({2, 4, 6, 1, 4, 5, 2, 7}),
           new Transformation<uint_fast8_t>({3, 0, 7, 2, 4, 6, 2, 4}),
           new Transformation<uint_fast8_t>({3, 2, 3, 4, 5, 3, 0, 1}),
           new Transformation<uint_fast8_t>({4, 3, 7, 7, 4, 5, 0, 4}),
           new Transformation<uint_fast8_t>({5, 6, 3, 0, 3, 0, 5, 1}),
           new Transformation<uint_fast8_t>({6, 0, 1, 1, 1, 6, 3, 4}),
           new Transformation<uint_fast8_t>({7, 7, 4, 0, 6, 4, 1, 7})};

    FroidurePin<Element const*> S = FroidurePin<Element const*>(gens);

    auto rg = ReportGuard(REPORT);
    REQUIRE(S.size() == 597369);
    delete_gens(gens);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "084",
                          "number of idempotents",
                          "[extreme][froidure-pin][element]") {
    auto               rg = ReportGuard();
    Semiring<int64_t>* sr = new NaturalSemiring(0, 6);

    std::vector<Element*> gens = {
        new MatrixOverSemiring<int64_t>({{0, 0, 1}, {0, 1, 0}, {1, 1, 0}}, sr),
        new MatrixOverSemiring<int64_t>({{0, 0, 1}, {0, 1, 0}, {2, 0, 0}}, sr),
        new MatrixOverSemiring<int64_t>({{0, 0, 1}, {0, 1, 1}, {1, 0, 0}}, sr),
        new MatrixOverSemiring<int64_t>({{0, 0, 1}, {0, 1, 0}, {3, 0, 0}}, sr)};
    FroidurePin<Element const*> S(gens);
    S.reserve(10077696);
    REQUIRE(S.size() == 10077696);
    REQUIRE(S.nr_idempotents() == 13688);
    delete sr;
    delete_gens(gens);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "085",
                          "number of idempotents",
                          "[extreme][froidure-pin][element]") {
    auto                  rg = ReportGuard();
    std::vector<Element*> gens
        = {new Bipartition({0, 1, 2, 3, 4, 5, 5, 0, 1, 2, 3, 4}),
           new Bipartition({0, 1, 2, 3, 4, 5, 1, 0, 2, 3, 4, 5}),
           new Bipartition({0, 1, 2, 3, 4, 5, 6, 1, 2, 3, 4, 5}),
           new Bipartition({0, 0, 1, 2, 3, 4, 0, 0, 1, 2, 3, 4})};
    FroidurePin<Element const*> S(gens);
    S.reserve(4213597);
    REQUIRE(S.size() == 4213597);
    REQUIRE(S.nr_idempotents() == 541254);
    delete_gens(gens);
  }

#if (!(defined(LIBSEMIGROUPS_DENSEHASHMAP)) && LIBSEMIGROUPS_SIZEOF_VOID_P == 8)

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "086",
                          "regular boolean mat monoid 4 using BooleanMat",
                          "[quick][froidure-pin][element][no-valgrind]") {
    std::vector<Element*> gens
        = {new BooleanMat(
               {{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
           new BooleanMat(
               {{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
           new BooleanMat(
               {{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {1, 0, 0, 1}}),
           new BooleanMat(
               {{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}})};
    FroidurePin<Element const*> S(gens);
    auto                        rg = ReportGuard(REPORT);
    REQUIRE(S.size() == 63904);
    REQUIRE(S.nr_idempotents() == 2360);
    delete_gens(gens);
  }
#endif

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "087",
                          "exception: zero generators given",
                          "[quick][froidure-pin][element]") {
    std::vector<Element*> gens;

    REQUIRE_THROWS_AS(FroidurePin<Element const*>(gens),
                      LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "088",
                          "exception: generators of different degrees",
                          "[quick][froidure-pin][element]") {
    std::vector<Element*> gens
        = {new Transformation<uint16_t>({0, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({0, 1, 2, 3, 4, 5, 5})};
    std::vector<Element*> gens2
        = {new PartialPerm<uint16_t>(
               {0, 1, 2, 3, 5, 6, 9}, {9, 7, 3, 5, 4, 2, 1}, 10),
           new PartialPerm<uint16_t>({4, 5, 0}, {10, 0, 1}, 11)};

    REQUIRE_THROWS_AS(FroidurePin<Element const*>(gens),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(FroidurePin<Element const*>(gens2),
                      LibsemigroupsException);

    delete_gens(gens);
    delete_gens(gens2);
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "089",
                          "exception: word_to_pos",
                          "[quick][froidure-pin][element]") {
    Semiring<int64_t>*    sr = new Integers();
    std::vector<Element*> gens
        = {new MatrixOverSemiring<int64_t>({{0, 0}, {0, 1}}, sr),
           new MatrixOverSemiring<int64_t>({{0, 1}, {-1, 0}}, sr)};
    FroidurePin<Element const*> T(gens);

    REQUIRE_THROWS_AS(T.word_to_pos({}), LibsemigroupsException);
    REQUIRE_NOTHROW(T.word_to_pos({0, 0, 1, 1}));
    REQUIRE(T.word_to_pos({0, 0, 1, 1}) == UNDEFINED);
    auto* w = T.word_to_element({0, 0, 1, 1});
    REQUIRE(T.current_position(w) == UNDEFINED);
    REQUIRE_THROWS_AS(T.word_to_pos({0, 0, 1, 2}), LibsemigroupsException);
    delete w;

    REQUIRE(T.size() == 13);
    REQUIRE(T.word_to_pos({0, 0, 1, 1}) == 6);
    w = T.word_to_element({0, 0, 1, 1});
    REQUIRE(T.current_position(w) == 6);
    delete w;

    std::vector<Element*> gens2
        = {new Transformation<uint16_t>({0, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 0, 2, 3, 4, 5}),
           new Transformation<uint16_t>({4, 0, 1, 2, 3, 5}),
           new Transformation<uint16_t>({5, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 1, 2, 3, 4, 5})};
    FroidurePin<Element const*> U(gens2);

    REQUIRE_THROWS_AS(U.word_to_pos({}), LibsemigroupsException);
    REQUIRE_NOTHROW(U.word_to_pos({0, 0, 1, 2}));
    REQUIRE_THROWS_AS(U.word_to_pos({5}), LibsemigroupsException);

    delete_gens(gens);
    delete_gens(gens2);
    delete sr;
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "090",
                          "exception: word_to_element",
                          "[quick][froidure-pin][element]") {
    Semiring<int64_t>*    sr = new Integers();
    std::vector<Element*> gens
        = {new MatrixOverSemiring<int64_t>({{0, 0}, {0, 1}}, sr),
           new MatrixOverSemiring<int64_t>({{0, 1}, {-1, 0}}, sr)};
    FroidurePin<Element const*> T(gens);

    REQUIRE_THROWS_AS(T.word_to_element({}), LibsemigroupsException);
    REQUIRE_THROWS_AS(T.word_to_element({0, 0, 1, 2}), LibsemigroupsException);

    Element* t = T.word_to_element({0, 0, 1, 1});
    REQUIRE(*t
            == MatrixOverSemiring<int64_t>({{0, 0}, {0, 1}}, sr)
                   * MatrixOverSemiring<int64_t>({{0, 0}, {0, 1}}, sr)
                   * MatrixOverSemiring<int64_t>({{0, 1}, {-1, 0}}, sr)
                   * MatrixOverSemiring<int64_t>({{0, 1}, {-1, 0}}, sr));

    std::vector<Element*> gens2
        = {new Transformation<uint16_t>({0, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 0, 2, 3, 4, 5}),
           new Transformation<uint16_t>({4, 0, 1, 2, 3, 5}),
           new Transformation<uint16_t>({5, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 1, 2, 3, 4, 5})};
    FroidurePin<Element const*> U(gens2);

    REQUIRE_THROWS_AS(U.word_to_element({}), LibsemigroupsException);
    REQUIRE_THROWS_AS(U.word_to_element({5}), LibsemigroupsException);

    Element* u = U.word_to_element({0, 0, 1, 2});
    REQUIRE(*u
            == Transformation<uint16_t>({0, 1, 2, 3, 4, 5})
                   * Transformation<uint16_t>({0, 1, 2, 3, 4, 5})
                   * Transformation<uint16_t>({1, 0, 2, 3, 4, 5})
                   * Transformation<uint16_t>({4, 0, 1, 2, 3, 5}));
    delete t;
    delete u;
    delete_gens(gens);
    delete_gens(gens2);
    delete sr;
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "091",
                          "exception: gens",
                          "[quick][froidure-pin][element]") {
    for (size_t i = 1; i < 20; ++i) {
      std::vector<Element*> gens;

      for (size_t j = 0; j < i; ++j) {
        std::vector<size_t> trans;
        for (size_t k = 0; k < i; ++k) {
          trans.push_back((k + j) % i);
        }
        gens.push_back(new Transformation<size_t>(trans));
      }
      FroidurePin<Element*> S(gens);
      delete_gens(gens);

      for (size_t j = 0; j < i; ++j) {
        REQUIRE_NOTHROW(S.generator(j));
      }
      REQUIRE_THROWS_AS(S.generator(i), LibsemigroupsException);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "092",
                          "exception: prefix",
                          "[quick][froidure-pin][element]") {
    Semiring<int64_t>*    sr = new Integers();
    std::vector<Element*> gens
        = {new MatrixOverSemiring<int64_t>({{0, 0}, {0, 1}}, sr),
           new MatrixOverSemiring<int64_t>({{0, 1}, {-1, 0}}, sr)};
    FroidurePin<Element const*> T(gens);
    delete_gens(gens);

    for (size_t i = 0; i < T.size(); ++i) {
      REQUIRE_NOTHROW(T.prefix(i));
      REQUIRE_THROWS_AS(T.prefix(i + T.size()), LibsemigroupsException);
    }
    delete sr;
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "093",
                          "exception: suffix",
                          "[quick][froidure-pin][element]") {
    Semiring<int64_t>*    sr = new Integers();
    std::vector<Element*> gens
        = {new MatrixOverSemiring<int64_t>({{0, 0}, {0, 1}}, sr),
           new MatrixOverSemiring<int64_t>({{0, 1}, {-1, 0}}, sr)};
    FroidurePin<Element const*> T(gens);

    for (size_t i = 0; i < T.size(); ++i) {
      REQUIRE_NOTHROW(T.suffix(i));
      REQUIRE_THROWS_AS(T.suffix(i + T.size()), LibsemigroupsException);
    }
    delete_gens(gens);
    delete sr;
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "094",
                          "exception: first_letter",
                          "[quick][froidure-pin][element]") {
    Semiring<int64_t>*    sr = new Integers();
    std::vector<Element*> gens
        = {new MatrixOverSemiring<int64_t>({{0, 0}, {0, 1}}, sr),
           new MatrixOverSemiring<int64_t>({{0, 1}, {-1, 0}}, sr)};
    FroidurePin<Element const*> T(gens);

    for (size_t i = 0; i < T.size(); ++i) {
      REQUIRE_NOTHROW(T.first_letter(i));
      REQUIRE_THROWS_AS(T.first_letter(i + T.size()), LibsemigroupsException);
    }
    delete_gens(gens);
    delete sr;
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "095",
                          "exception: final_letter",
                          "[quick][froidure-pin][element]") {
    Semiring<int64_t>*    sr = new Integers();
    std::vector<Element*> gens
        = {new MatrixOverSemiring<int64_t>({{0, 0}, {0, 1}}, sr),
           new MatrixOverSemiring<int64_t>({{0, 1}, {-1, 0}}, sr)};
    FroidurePin<Element const*> T(gens);

    for (size_t i = 0; i < T.size(); ++i) {
      REQUIRE_NOTHROW(T.final_letter(i));
      REQUIRE_THROWS_AS(T.final_letter(i + T.size()), LibsemigroupsException);
    }
    delete_gens(gens);
    delete sr;
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "096",
                          "exception: length_const",
                          "[quick][froidure-pin][element]") {
    Semiring<int64_t>*    sr = new Integers();
    std::vector<Element*> gens
        = {new MatrixOverSemiring<int64_t>({{0, 0}, {0, 1}}, sr),
           new MatrixOverSemiring<int64_t>({{0, 1}, {-1, 0}}, sr)};
    FroidurePin<Element const*> T(gens);

    for (size_t i = 0; i < T.size(); ++i) {
      REQUIRE_NOTHROW(T.length_const(i));
      REQUIRE_THROWS_AS(T.length_const(i + T.size()), LibsemigroupsException);
    }
    delete_gens(gens);
    delete sr;
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "097",
                          "exception: product_by_reduction",
                          "[quick][froidure-pin][element]") {
    Semiring<int64_t>*    sr = new Integers();
    std::vector<Element*> gens
        = {new MatrixOverSemiring<int64_t>({{0, 0}, {0, 1}}, sr),
           new MatrixOverSemiring<int64_t>({{0, 1}, {-1, 0}}, sr)};
    FroidurePin<Element const*> T(gens);

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

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "098",
                          "exception: fast_product",
                          "[quick][froidure-pin][element]") {
    Semiring<int64_t>*    sr = new Integers();
    std::vector<Element*> gens
        = {new MatrixOverSemiring<int64_t>({{0, 0}, {0, 1}}, sr),
           new MatrixOverSemiring<int64_t>({{0, 1}, {-1, 0}}, sr)};
    FroidurePin<Element const*> T(gens);

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

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "099",
                          "exception: letter_to_pos",
                          "[quick][froidure-pin][element]") {
    for (size_t i = 1; i < 20; ++i) {
      std::vector<Element*> gens;

      for (size_t j = 0; j < i; ++j) {
        std::vector<size_t> trans;
        for (size_t k = 0; k < i; ++k) {
          trans.push_back((k + j) % i);
        }
        gens.push_back(new Transformation<size_t>(trans));
      }
      FroidurePin<Element const*> S(gens);
      delete_gens(gens);

      for (size_t j = 0; j < i; ++j) {
        REQUIRE_NOTHROW(S.letter_to_pos(j));
      }
      REQUIRE_THROWS_AS(S.letter_to_pos(i), LibsemigroupsException);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "100",
                          "exception: is_idempotent",
                          "[quick][froidure-pin][element]") {
    std::vector<Element*> gens
        = {new Bipartition(
               {0, 1, 2, 1, 0, 2, 1, 0, 2, 2, 0, 0, 2, 0, 3, 4, 4, 1, 3, 0}),
           new Bipartition(
               {0, 1, 1, 1, 1, 2, 3, 2, 4, 5, 5, 2, 4, 2, 1, 1, 1, 2, 3, 2}),
           new Bipartition(
               {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0})};
    FroidurePin<Element const*> T = FroidurePin<Element const*>(gens);
    delete_gens(gens);

    // T has size 10
    for (size_t i = 0; i < 10; ++i) {
      REQUIRE_NOTHROW(T.is_idempotent(i));
    }
    for (size_t i = 0; i < 20; ++i) {
      REQUIRE_THROWS_AS(T.is_idempotent(10 + i), LibsemigroupsException);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "101",
                          "exception: add_generators",
                          "[quick][froidure-pin][element]") {
    std::vector<Element*> gens1
        = {new Transformation<uint16_t>({0, 1, 2, 3, 4, 5}),
           new Transformation<uint16_t>({1, 2, 3, 2, 2, 3})};
    std::vector<Element*> gens2
        = {new PartialPerm<uint16_t>(
               {0, 1, 2, 3, 5, 6, 9}, {9, 7, 3, 5, 4, 2, 1}, 11),
           new PartialPerm<uint16_t>({4, 5, 0}, {10, 0, 1}, 11)};

    FroidurePin<Element const*> S(gens1);
    FroidurePin<Element const*> U(gens2);

    std::vector<Element*> additional_gens_1_1
        = {new Transformation<uint16_t>({0, 1, 2, 3, 3, 3})};
    std::vector<Element*> additional_gens_1_2
        = {new Transformation<uint16_t>({0, 1, 2, 3, 3, 3}),
           new Transformation<uint16_t>({0, 1, 2, 3, 3, 3, 3})};
    std::vector<Element*> additional_gens_2_1
        = {new PartialPerm<uint16_t>(
               {0, 1, 2, 3, 5, 6, 9}, {2, 7, 5, 1, 4, 3, 9}, 11),
           new PartialPerm<uint16_t>({2, 5, 1}, {6, 0, 3}, 11)};
    std::vector<Element*> additional_gens_2_2
        = {new PartialPerm<uint16_t>(
               {0, 1, 2, 3, 5, 6, 9}, {2, 7, 5, 1, 4, 3, 9}, 11),
           new PartialPerm<uint16_t>({2, 5, 1}, {6, 0, 3}, 12)};

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
}  // namespace libsemigroups
