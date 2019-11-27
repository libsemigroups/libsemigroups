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

#include "bmat8.hpp"            // for BMat8
#include "catch.hpp"            // for TEST_CASE
#include "cong-pair.hpp"        // for KnuthBendixCongruenceByPairs
#include "cong.hpp"             // for Congruence
#include "element-helper.hpp"   // for BMatHelper
#include "element.hpp"          // for Element
#include "fpsemi-examples.hpp"  // for RookMonoid
#include "fpsemi.hpp"           // for FpSemigroup
#include "froidure-pin.hpp"     // for FroidurePin
#include "report.hpp"           // for ReportGuard
#include "test-main.hpp"        // for LIBSEMIGROUPS_TEST_CASE
#include "types.hpp"            // for word_type

namespace libsemigroups {
  // Forward declarations
  struct LibsemigroupsException;

  constexpr bool REPORT = false;

  template <typename TElementType>
  void delete_gens(std::vector<TElementType>& gens) {
    for (auto& x : gens) {
      delete x;
    }
  }

  constexpr congruence_type twosided = congruence_type::twosided;
  constexpr congruence_type left     = congruence_type::left;
  constexpr congruence_type right    = congruence_type::right;

  LIBSEMIGROUPS_TEST_CASE("Congruence",
                          "000",
                          "left congruence on fp semigroup",
                          "[quick][cong]") {
    auto        rg = ReportGuard(REPORT);
    FpSemigroup S;
    S.set_alphabet(2);
    S.add_rule(word_type({0, 0, 0}), word_type({0}));
    S.add_rule(word_type({0}), word_type({1, 1}));

    Congruence cong(left, S);
  }

  LIBSEMIGROUPS_TEST_CASE("Congruence",
                          "001",
                          "2-sided congruence on fp semigroup",
                          "[quick][cong]") {
    auto rg = ReportGuard(REPORT);

    FpSemigroup S;
    S.set_alphabet(2);
    S.add_rule({0, 0, 0}, {0});
    S.add_rule({0}, {1, 1});
    REQUIRE(!S.is_obviously_infinite());

    Congruence cong(twosided, S);

    REQUIRE(cong.nr_generating_pairs() == 0);
    REQUIRE(cong.nr_classes() == 5);

    REQUIRE(cong.word_to_class_index({0, 0, 1})
            == cong.word_to_class_index({0, 0, 0, 0, 1}));
    REQUIRE(cong.contains({0, 0, 1}, {0, 0, 1}));
    REQUIRE(cong.contains({0, 0, 1}, {0, 0, 0, 0, 1}));
    REQUIRE(cong.word_to_class_index({0, 0, 0, 0, 1})
            == cong.word_to_class_index({0, 1, 1, 0, 0, 1}));
    REQUIRE(cong.contains({0, 0, 0, 0, 1}, {0, 1, 1, 0, 0, 1}));
    REQUIRE(cong.word_to_class_index({0, 0, 0})
            != cong.word_to_class_index({0, 0, 1}));
    REQUIRE(!cong.contains({0, 0, 0}, {0, 0, 1}));
    REQUIRE(cong.word_to_class_index({1})
            != cong.word_to_class_index({0, 0, 0}));
    REQUIRE(!cong.contains({1}, {0, 0, 0}));
  }

  LIBSEMIGROUPS_TEST_CASE("Congruence",
                          "002",
                          "left congruence on fp semigroup",
                          "[quick][cong]") {
    auto        rg = ReportGuard(REPORT);
    FpSemigroup S;
    S.set_alphabet(2);
    S.add_rule({0, 0, 0}, {0});  // (a^3, a)
    S.add_rule({0}, {1, 1});     // (a, b^2)

    Congruence cong(left, S);
    REQUIRE(cong.nr_classes() == 5);
  }

  LIBSEMIGROUPS_TEST_CASE("Congruence",
                          "003",
                          "word_to_class_index for cong. on fp semigroup",
                          "[quick][cong]") {
    auto rg = ReportGuard(REPORT);

    FpSemigroup S;
    S.set_alphabet(2);
    S.add_rule({0, 0, 0}, {0});  // (a^3, a)
    S.add_rule({0}, {1, 1});     // (a, b^2)

    Congruence cong(left, S);
    REQUIRE(cong.nr_classes() == 5);
    REQUIRE(cong.word_to_class_index({0, 1, 1, 0, 0, 1})
            == cong.word_to_class_index({0, 0, 1}));
    REQUIRE(cong.word_to_class_index({0, 0, 1})
            == cong.word_to_class_index({0, 0, 0, 0, 1}));
    REQUIRE(cong.contains({0, 1, 1, 0, 0, 1}, {0, 0, 1}));
    REQUIRE(cong.word_to_class_index({0, 0, 0})
            != cong.word_to_class_index({0, 0, 1}));
    REQUIRE(cong.word_to_class_index({1})
            != cong.word_to_class_index({0, 0, 0, 0}));
    REQUIRE(!cong.contains({0, 0, 0, 0}, {0, 0, 1}));
  }

  LIBSEMIGROUPS_TEST_CASE("Congruence",
                          "004",
                          "word_to_class_index for cong. on fp semigroup",
                          "[quick][cong]") {
    auto        rg = ReportGuard(REPORT);
    FpSemigroup S;
    S.set_alphabet(2);
    S.add_rule({0, 0, 0}, {0});  // (a^3, a)
    S.add_rule({0}, {1, 1});     // (a, b^2)

    Congruence cong1(twosided, S);

    REQUIRE(cong1.word_to_class_index({0, 0, 1})
            == cong1.word_to_class_index({0, 0, 0, 0, 1}));
    REQUIRE(cong1.word_to_class_index({0, 1, 1, 0, 0, 1})
            == cong1.word_to_class_index({0, 0, 0, 0, 1}));
    REQUIRE(cong1.word_to_class_index({0, 0, 0})
            == cong1.word_to_class_index({1, 1}));
    REQUIRE(cong1.word_to_class_index({1}) != cong1.word_to_class_index({0}));

    Congruence cong2(twosided, S);

    REQUIRE(cong2.word_to_class_index({0, 0, 0, 0})
            == cong2.word_to_class_index({0, 0}));
    REQUIRE(cong2.contains({0, 0, 0, 0}, {0, 1, 1, 0, 1, 1}));
  }

  LIBSEMIGROUPS_TEST_CASE("Congruence",
                          "005",
                          "trivial congruence on non-fp semigroup",
                          "[quick][cong]") {
    auto rg = ReportGuard(REPORT);

    using Transf = TransfHelper<5>::type;
    FroidurePin<Transf> S({Transf({1, 3, 4, 2, 3}), Transf({3, 2, 1, 3, 3})});
    REQUIRE(S.size() == 88);

    Congruence cong(twosided, S);
    REQUIRE(cong.is_quotient_obviously_finite());
    REQUIRE(cong.nr_classes() == 88);
  }

  LIBSEMIGROUPS_TEST_CASE("Congruence",
                          "006",
                          "2-sided congruence on non-fp semigroup",
                          "[quick][cong]") {
    auto rg = ReportGuard(REPORT);

    using Transf = TransfHelper<5>::type;
    FroidurePin<Transf> S({Transf({1, 3, 4, 2, 3}), Transf({3, 2, 1, 3, 3})});
    REQUIRE(S.size() == 88);

    Congruence cong(twosided, S);
    cong.add_pair(S.factorisation(Transf({3, 4, 4, 4, 4})),
                  S.factorisation(Transf({3, 1, 3, 3, 3})));
    REQUIRE(cong.nr_classes() == 21);
  }

  LIBSEMIGROUPS_TEST_CASE("Congruence",
                          "007",
                          "2-sided congruence on fp semigroup",
                          "[quick][cong]") {
    auto        rg = ReportGuard(REPORT);
    FpSemigroup S;
    S.set_alphabet(3);
    S.add_rule({0, 1}, {1, 0});
    S.add_rule({0, 2}, {2, 2});
    S.add_rule({0, 2}, {0});
    S.add_rule({0, 2}, {0});
    S.add_rule({2, 2}, {0});
    S.add_rule({1, 2}, {1, 2});
    S.add_rule({1, 2}, {2, 2});
    S.add_rule({1, 2, 2}, {1});
    S.add_rule({1, 2}, {1});
    S.add_rule({2, 2}, {1});
    S.add_rule({0}, {1});

    REQUIRE(S.size() == 2);
    REQUIRE(S.froidure_pin()->size() == 2);

    Congruence cong1(twosided, S.froidure_pin());
    cong1.add_pair({0}, {1});
    REQUIRE(cong1.nr_classes() == 2);

    Congruence cong2(twosided, S);
    cong2.add_pair({0}, {1});
    REQUIRE(cong2.nr_classes() == 2);
  }

  LIBSEMIGROUPS_TEST_CASE("Congruence",
                          "008",
                          "2-sided congruence on infinite fp semigroup",
                          "[quick][cong]") {
    auto        rg = ReportGuard(REPORT);
    FpSemigroup S;
    S.set_alphabet(3);
    S.add_rule({0, 1}, {1, 0});
    S.add_rule({0, 2}, {2, 2});
    S.add_rule({0, 2}, {0});
    S.add_rule({0, 2}, {0});
    S.add_rule({2, 2}, {0});
    S.add_rule({1, 2}, {1, 2});
    S.add_rule({1, 2}, {2, 2});
    S.add_rule({1, 2, 2}, {1});
    S.add_rule({1, 2}, {1});
    S.add_rule({2, 2}, {1});

    Congruence cong(twosided, S);
    cong.add_pair({0}, {1});

    REQUIRE(cong.word_to_class_index({0}) == cong.word_to_class_index({1}));
    REQUIRE(cong.word_to_class_index({0}) == cong.word_to_class_index({1, 0}));
    REQUIRE(cong.word_to_class_index({0}) == cong.word_to_class_index({1, 1}));
    REQUIRE(cong.word_to_class_index({0})
            == cong.word_to_class_index({1, 0, 1}));

    REQUIRE(cong.contains({1}, {1, 1}));
    REQUIRE(cong.contains({1, 0, 1}, {1, 0}));
    REQUIRE(cong.nr_classes() == 2);
  }

  LIBSEMIGROUPS_TEST_CASE("Congruence",
                          "009",
                          "2-sided congruence on infinite fp semigroup",
                          "[quick][cong]") {
    auto        rg = ReportGuard(REPORT);
    FpSemigroup S;
    S.set_alphabet(3);
    S.add_rule({0, 1}, {1, 0});
    S.add_rule({0, 2}, {2, 0});
    S.add_rule({0, 0}, {0});
    S.add_rule({0, 2}, {0});
    S.add_rule({2, 0}, {0});
    S.add_rule({1, 2}, {2, 1});
    S.add_rule({1, 1, 1}, {1});
    S.add_rule({1, 2}, {1});
    S.add_rule({2, 1}, {1});

    Congruence cong(twosided, S);
    cong.add_pair({0}, {1});

    // Requires KnuthBendixCongruenceByPairs to work
    REQUIRE(cong.word_to_class_index({0}) == cong.word_to_class_index({1}));
    REQUIRE(cong.word_to_class_index({0}) == cong.word_to_class_index({1, 0}));
    REQUIRE(cong.word_to_class_index({0}) == cong.word_to_class_index({1, 1}));
    REQUIRE(cong.word_to_class_index({0})
            == cong.word_to_class_index({1, 0, 1}));

    REQUIRE(cong.contains({1}, {1, 1}));
    REQUIRE(cong.contains({1, 0, 1}, {1, 0}));

    REQUIRE(!cong.less({1, 0, 1}, {1, 0}));
  }

  LIBSEMIGROUPS_TEST_CASE("Congruence",
                          "010",
                          "2-sided congruence on finite semigroup",
                          "[quick][cong][no-valgrind]") {
    auto rg      = ReportGuard(REPORT);
    using Transf = TransfHelper<8>::type;
    FroidurePin<Transf> S({Transf({7, 3, 5, 3, 4, 2, 7, 7}),
                           Transf({1, 2, 4, 4, 7, 3, 0, 7}),
                           Transf({0, 6, 4, 2, 2, 6, 6, 4}),
                           Transf({3, 6, 3, 4, 0, 6, 0, 7})});

    // The following lines are intentionally commented out.
    // REQUIRE(S.size() == 11804);
    // REQUIRE(S.nr_rules() == 2460);

    Congruence cong(twosided, S);
    cong.add_pair({0, 3, 2, 1, 3, 2, 2}, {3, 2, 2, 1, 3, 3});

    REQUIRE(cong.word_to_class_index({0, 0, 0, 1})
            == cong.word_to_class_index({0, 0, 1, 0, 0}));
    REQUIRE(cong.word_to_class_index({0, 0, 1, 0, 1})
            == cong.word_to_class_index({1, 1, 0, 1}));
    REQUIRE(cong.word_to_class_index({1, 1, 0, 0})
            != cong.word_to_class_index({0, 0, 0, 1}));
    REQUIRE(cong.word_to_class_index({0, 0, 3})
            != cong.word_to_class_index({0, 0, 0, 1}));
    REQUIRE(cong.word_to_class_index({1, 1, 0, 0})
            != cong.word_to_class_index({0, 0, 3}));
    REQUIRE(cong.word_to_class_index({1, 2, 1, 3, 3, 2, 1, 2})
            == cong.word_to_class_index({2, 1, 3, 3, 2, 1, 0}));
    REQUIRE(cong.word_to_class_index({0, 3, 1, 1, 1, 3, 2, 2, 1, 0})
            == cong.word_to_class_index({0, 3, 2, 2, 1}));
    REQUIRE(cong.word_to_class_index({0, 3, 2, 1, 3, 3, 3})
            != cong.word_to_class_index({0, 0, 3}));
    REQUIRE(cong.word_to_class_index({1, 1, 0})
            != cong.word_to_class_index({1, 3, 3, 2, 2, 1, 0}));

    REQUIRE(cong.contains({1, 2, 1, 3, 3, 2, 1, 2}, {2, 1, 3, 3, 2, 1, 0}));
    REQUIRE(!cong.contains({1, 1, 0}, {1, 3, 3, 2, 2, 1, 0}));

    REQUIRE(cong.less({1, 3, 3, 2, 2, 1, 0}, {1, 1, 0}));
    REQUIRE(!cong.less({1, 1, 0, 0}, {0, 0, 3}));

    REQUIRE(cong.nr_classes() == 525);
    REQUIRE(cong.nr_classes() == 525);
  }

  LIBSEMIGROUPS_TEST_CASE("Congruence",
                          "011",
                          "congruence on full PBR monoid on 2 points",
                          "[extreme][cong]") {
    auto             rg = ReportGuard(true);
    FroidurePin<PBR> S({PBR({{2}, {3}, {0}, {1}}),
                        PBR({{}, {2}, {1}, {0, 3}}),
                        PBR({{0, 3}, {2}, {1}, {}}),
                        PBR({{1, 2}, {3}, {0}, {1}}),
                        PBR({{2}, {3}, {0}, {1, 3}}),
                        PBR({{3}, {1}, {0}, {1}}),
                        PBR({{3}, {2}, {0}, {0, 1}}),
                        PBR({{3}, {2}, {0}, {1}}),
                        PBR({{3}, {2}, {0}, {3}}),
                        PBR({{3}, {2}, {1}, {0}}),
                        PBR({{3}, {2, 3}, {0}, {1}})});

    // REQUIRE(S.size() == 65536);
    // REQUIRE(S.nr_rules() == 45416);

    Congruence cong(twosided, S);
    cong.add_pair({7, 10, 9, 3, 6, 9, 4, 7, 9, 10}, {9, 3, 6, 6, 10, 9, 4, 7});
    cong.add_pair({8, 7, 5, 8, 9, 8}, {6, 3, 8, 6, 1, 2, 4});

    REQUIRE(cong.nr_classes() == 19009);
    REQUIRE(cong.nr_non_trivial_classes() == 577);
    REQUIRE(cong.cend_ntc() - cong.cbegin_ntc() == 577);

    std::vector<size_t> v(577, 0);
    std::transform(cong.cbegin_ntc(),
                   cong.cend_ntc(),
                   v.begin(),
                   std::mem_fn(&std::vector<word_type>::size));
    REQUIRE(std::count(v.cbegin(), v.cend(), 4) == 384);
    REQUIRE(std::count(v.cbegin(), v.cend(), 16) == 176);
    REQUIRE(std::count(v.cbegin(), v.cend(), 96) == 16);
    REQUIRE(std::count(v.cbegin(), v.cend(), 41216) == 1);
  }

  LIBSEMIGROUPS_TEST_CASE("Congruence",
                          "012",
                          "2-sided congruence on finite semigroup",
                          "[quick][cong][no-valgrind]") {
    auto rg = ReportGuard(REPORT);

    using PPerm = PPermHelper<6>::type;

    FroidurePin<PPerm> S({PPerm({0, 1, 2}, {4, 0, 1}, 6),
                          PPerm({0, 1, 2, 3, 5}, {2, 5, 3, 0, 4}, 6),
                          PPerm({0, 1, 2, 3}, {5, 0, 3, 1}, 6),
                          PPerm({0, 2, 5}, {3, 4, 1}, 6),
                          PPerm({0, 2, 5}, {0, 2, 5}, 6),
                          PPerm({0, 1, 4}, {1, 2, 0}, 6),
                          PPerm({0, 2, 3, 4, 5}, {3, 0, 2, 5, 1}, 6),
                          PPerm({0, 1, 3, 5}, {1, 3, 2, 0}, 6),
                          PPerm({1, 3, 4}, {5, 0, 2}, 6)});

    // REQUIRE(S.size() == 712);
    // REQUIRE(S.nr_rules() == 1121);

    Congruence cong(twosided, S);
    cong.add_pair({2, 7}, {1, 6, 6, 1});
    REQUIRE(cong.nr_classes() == 32);
  }

  LIBSEMIGROUPS_TEST_CASE("Congruence",
                          "013",
                          "trivial 2-sided congruence on bicyclic monoid",
                          "[quick][cong]") {
    auto        rg = ReportGuard(REPORT);
    FpSemigroup S;
    S.set_alphabet(3);
    S.set_identity(0);
    S.add_rule({1, 2}, {0});
    Congruence cong(twosided, S);
    REQUIRE(cong.word_to_class_index({0})
            == cong.word_to_class_index({1, 2, 1, 1, 2, 2}));
    REQUIRE(cong.word_to_class_index({0})
            == cong.word_to_class_index({1, 0, 2, 0, 1, 2}));
    REQUIRE(cong.word_to_class_index({2, 1})
            == cong.word_to_class_index({1, 2, 0, 2, 1, 1, 2}));
    REQUIRE(cong.contains({2, 1}, {1, 2, 0, 2, 1, 1, 2}));
  }

  LIBSEMIGROUPS_TEST_CASE("Congruence",
                          "014",
                          "non-trivial 2-sided congruence on bicyclic monoid",
                          "[quick][cong]") {
    auto        rg = ReportGuard(REPORT);
    FpSemigroup S;
    S.set_alphabet(3);
    S.set_identity(0);
    S.add_rule({1, 2}, {0});
    REQUIRE(!S.is_obviously_infinite());

    Congruence cong(twosided, S);
    cong.add_pair({1, 1, 1}, {0});
    REQUIRE(cong.nr_classes() == 3);
    // The following runs forever because !S.is_obviously_infinite(), but it is
    // infinite
    // REQUIRE_THROWS_AS(cong.nr_non_trivial_classes() == 3,
    //                 LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("Congruence",
                          "015",
                          "2-sided congruence on free abelian monoid",
                          "[quick][cong]") {
    auto        rg = ReportGuard(REPORT);
    FpSemigroup S;
    S.set_alphabet(3);
    S.add_rule({1, 2}, {2, 1});
    S.set_identity(0);

    Congruence cong(twosided, S);
    cong.add_pair({1, 1, 1, 1, 1}, {1});
    cong.add_pair({2, 2, 2}, {2});

    REQUIRE(cong.nr_classes() == 15);
  }

  // The previous Congruence 17 test was identical to Congruence 12

  LIBSEMIGROUPS_TEST_CASE("Congruence",
                          "016",
                          "example where TC works but KB doesn't",
                          "[quick][cong]") {
    auto        rg = ReportGuard(REPORT);
    FpSemigroup S;
    S.set_alphabet("abBe");
    REQUIRE_THROWS_AS(S.add_rule("aa", ""), LibsemigroupsException);
    S.set_identity("e");
    S.add_rule("aa", "e");
    S.add_rule("BB", "b");
    S.add_rule("BaBaBaB", "abababa");
    S.add_rule("aBabaBabaBabaBab", "BabaBabaBabaBaba");

    Congruence cong(twosided, S);
    cong.add_pair({0}, {1});

    REQUIRE(cong.nr_classes() == 4);
    REQUIRE(!cong.quotient_froidure_pin()->is_monoid());
  }

  LIBSEMIGROUPS_TEST_CASE("Congruence",
                          "017",
                          "2-sided congruence on finite semigroup",
                          "[quick][cong]") {
    auto rg      = ReportGuard(REPORT);
    using Transf = TransfHelper<5>::type;
    FroidurePin<Transf> S({Transf({1, 3, 4, 2, 3}), Transf({3, 2, 1, 3, 3})});

    REQUIRE(S.size() == 88);
    REQUIRE(S.nr_rules() == 18);

    word_type w1 = S.factorisation(Transf({3, 4, 4, 4, 4}));
    word_type w2 = S.factorisation(Transf({3, 4, 4, 4, 4}));

    Congruence cong(twosided, S);
    cong.add_pair(S.factorisation(Transf({3, 4, 4, 4, 4})),
                  S.factorisation(Transf({3, 1, 3, 3, 3})));
    REQUIRE(cong.nr_classes() == 21);

    word_type u = S.factorisation(Transf({1, 3, 1, 3, 3}));
    word_type v = S.factorisation(Transf({4, 2, 4, 4, 2}));
    REQUIRE(cong.word_to_class_index(u) == cong.word_to_class_index(v));
    REQUIRE(cong.contains(u, v));
  }

  // The next test behaves as expected but runs forever, since the nr_classes
  // method requires to know the size of the semigroup S, and we cannot
  // currently work that out.
  LIBSEMIGROUPS_TEST_CASE("Congruence",
                          "018",
                          "infinite fp semigroup from GAP library ",
                          "[quick][cong]") {
    auto        rg = ReportGuard(REPORT);
    FpSemigroup S;
    S.set_alphabet(3);
    S.add_rule({0, 0}, {0, 0});
    S.add_rule({0, 1}, {1, 0});
    S.add_rule({0, 2}, {2, 0});
    S.add_rule({0, 0}, {0});
    S.add_rule({0, 2}, {0});
    S.add_rule({2, 0}, {0});
    S.add_rule({1, 0}, {0, 1});
    S.add_rule({1, 1}, {1, 1});
    S.add_rule({1, 2}, {2, 1});
    S.add_rule({1, 1, 1}, {1});
    S.add_rule({1, 2}, {1});
    S.add_rule({2, 1}, {1});

    REQUIRE(S.is_obviously_infinite());

    Congruence cong(twosided, S);
    cong.add_pair({0}, {1});
    REQUIRE(!cong.finished());
    REQUIRE(cong.nr_non_trivial_classes() == 1);
    REQUIRE(cong.cbegin_ntc()->size() == 5);
    REQUIRE(cong.finished());
    // REQUIRE(cong.nr_classes());  // Currently runs forever (and should)
  }

  LIBSEMIGROUPS_TEST_CASE("Congruence",
                          "019",
                          "2-sided cong. on fp semigroup with infinite classes",
                          "[quick][cong]") {
    auto        rg = ReportGuard(REPORT);
    FpSemigroup S;
    S.set_alphabet(2);
    S.add_rule({0, 1}, {1, 0});
    S.add_rule({0, 0, 0}, {0, 0});

    Congruence cong(twosided, S);
    cong.add_pair({0}, {1});

    word_type x
        = {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    word_type y
        = {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    REQUIRE(std::count(x.cbegin(), x.cend(), 1) == 20);
    REQUIRE(std::count(y.cbegin(), y.cend(), 1) == 20);
    REQUIRE(cong.contains(x, y));
    REQUIRE(!cong.less({0, 0, 0}, {1}));
    REQUIRE(cong.less({1}, {0, 0, 0}));
    REQUIRE(!cong.less(x, y));
    REQUIRE(!cong.less(y, x));
    REQUIRE(cong.contains(x, y));
  }

  LIBSEMIGROUPS_TEST_CASE("Congruence",
                          "020",
                          "trivial cong. on an fp semigroup",
                          "[quick][cong][no-valgrind]") {
    auto        rg = ReportGuard(REPORT);
    FpSemigroup S;
    S.set_alphabet("ab");
    S.add_rule("ab", "ba");
    S.add_rule("a", "b");

    Congruence cong(left, S);
    // No generating pairs for the congruence (not the fp semigroup) means no
    // non-trivial classes.
    REQUIRE(cong.nr_non_trivial_classes() == 0);
  }

  LIBSEMIGROUPS_TEST_CASE("Congruence",
                          "021",
                          "duplicate generators",
                          "[quick][cong]") {
    auto rg      = ReportGuard(REPORT);
    using Transf = TransfHelper<8>::type;
    FroidurePin<Transf> S({Transf({7, 3, 5, 3, 4, 2, 7, 7}),
                           Transf({7, 3, 5, 3, 4, 2, 7, 7}),
                           Transf({7, 3, 5, 3, 4, 2, 7, 7}),
                           Transf({3, 6, 3, 4, 0, 6, 0, 7})});
    Congruence          cong(twosided, S);
    REQUIRE(cong.nr_classes() == S.size());
  }

  LIBSEMIGROUPS_TEST_CASE("Congruence",
                          "022",
                          "non-trivial classes",
                          "[quick][cong]") {
    auto rg = ReportGuard(false);

    FpSemigroup S;
    S.set_alphabet(2);
    S.add_rule({0, 0, 0}, {0});
    S.add_rule({1, 0, 0}, {1, 0});
    S.add_rule({1, 0, 1, 1, 1}, {1, 0});
    S.add_rule({1, 1, 1, 1, 1}, {1, 1});
    S.add_rule({1, 1, 0, 1, 1, 0}, {1, 0, 1, 0, 1, 1});
    S.add_rule({0, 0, 1, 0, 1, 1, 0}, {0, 1, 0, 1, 1, 0});
    S.add_rule({0, 0, 1, 1, 0, 1, 0}, {0, 1, 1, 0, 1, 0});
    S.add_rule({0, 1, 0, 1, 0, 1, 0}, {1, 0, 1, 0, 1, 0});
    S.add_rule({1, 0, 1, 0, 1, 0, 1}, {1, 0, 1, 0, 1, 0});
    S.add_rule({1, 0, 1, 0, 1, 1, 0}, {1, 0, 1, 0, 1, 1});
    S.add_rule({1, 0, 1, 1, 0, 1, 0}, {1, 0, 1, 1, 0, 1});
    S.add_rule({1, 1, 0, 1, 0, 1, 0}, {1, 0, 1, 0, 1, 0});
    S.add_rule({1, 1, 1, 1, 0, 1, 0}, {1, 0, 1, 0});
    S.add_rule({0, 0, 1, 1, 1, 0, 1, 0}, {1, 1, 1, 0, 1, 0});

    // TODO(later) this test fails if we don't run the next line, since the
    // congruence below has no parent
    REQUIRE(S.size() == 78);

    Congruence cong(twosided, S);
    cong.add_pair({0}, {1});

    REQUIRE(cong.nr_non_trivial_classes() == 1);
    REQUIRE(cong.cbegin_ntc()->size() == 78);
  }

  LIBSEMIGROUPS_TEST_CASE("Congruence",
                          "023",
                          "right congruence on finite semigroup",
                          "[quick][cong][no-valgrind]") {
    auto rg      = ReportGuard(REPORT);
    using Transf = TransfHelper<8>::type;
    FroidurePin<Transf> S({Transf({0, 1, 2, 3, 4, 5, 6, 7}),
                           Transf({1, 2, 3, 4, 5, 0, 6, 7}),
                           Transf({1, 0, 2, 3, 4, 5, 6, 7}),
                           Transf({0, 1, 2, 3, 4, 0, 6, 7}),
                           Transf({0, 1, 2, 3, 4, 5, 7, 6})});
    REQUIRE(S.size() == 93312);
    std::vector<Transf> elms = {Transf({0, 0, 0, 0, 0, 0, 7, 6}),
                                Transf({0, 0, 0, 0, 0, 0, 6, 7}),
                                Transf({0, 0, 0, 0, 0, 0, 6, 7}),
                                Transf({1, 1, 1, 1, 1, 1, 6, 7}),
                                Transf({0, 0, 0, 0, 0, 0, 6, 7}),
                                Transf({2, 2, 2, 2, 2, 2, 6, 7}),
                                Transf({0, 0, 0, 0, 0, 0, 6, 7}),
                                Transf({3, 3, 3, 3, 3, 3, 6, 7}),
                                Transf({0, 0, 0, 0, 0, 0, 6, 7}),
                                Transf({4, 4, 4, 4, 4, 4, 6, 7}),
                                Transf({0, 0, 0, 0, 0, 0, 6, 7}),
                                Transf({5, 5, 5, 5, 5, 5, 6, 7}),
                                Transf({0, 0, 0, 0, 0, 0, 7, 6}),
                                Transf({0, 1, 2, 3, 4, 5, 7, 6})};
    REQUIRE(
        std::all_of(elms.cbegin(), elms.cend(), [&S](Transf const& x) -> bool {
          return S.contains(x);
        }));

    Congruence cong(right, S);
    word_type  w1, w2;
    for (size_t i = 0; i < elms.size(); i += 2) {
      S.factorisation(w1, S.position(elms[i]));
      S.factorisation(w2, S.position(elms[i + 1]));
      cong.add_pair(w1, w2);
    }
    REQUIRE(cong.nr_classes() == 1);
  }

  LIBSEMIGROUPS_TEST_CASE("Congruence",
                          "024",
                          "redundant generating pairs",
                          "[quick][cong]") {
    auto        rg = ReportGuard(REPORT);
    FpSemigroup S;
    S.set_alphabet(1);

    Congruence cong(twosided, S);
    cong.add_pair({0, 0}, {0, 0});
    REQUIRE(cong.contains({0, 0}, {0, 0}));
  }

  LIBSEMIGROUPS_TEST_CASE("Congruence",
                          "025",
                          "2-sided cong. on free semigroup",
                          "[quick][cong]") {
    auto        rg = ReportGuard(REPORT);
    FpSemigroup S;
    S.set_alphabet("a");
    Congruence cong(twosided, S);
    REQUIRE(cong.contains({0, 0}, {0, 0}));
    REQUIRE(!cong.contains({0, 0}, {0}));
  }

  LIBSEMIGROUPS_TEST_CASE("Congruence",
                          "026",
                          "is_quotient_obviously_(in)finite",
                          "[quick][cong]") {
    auto rg = ReportGuard(REPORT);
    {
      FpSemigroup S;
      S.set_alphabet(3);
      S.add_rule({0, 1}, {0});
      Congruence cong(twosided, S);
      cong.add_pair({2, 2}, {2});
      REQUIRE(cong.is_quotient_obviously_infinite());
      REQUIRE(cong.nr_classes() == POSITIVE_INFINITY);
      // REQUIRE(cong.class_index_to_word(0) == word_type({}));
      // TODO(later) the above doesn't currently work, but it should, and there
      // is no reason it can't
      REQUIRE(!cong.is_quotient_obviously_finite());
    }
    {
      FpSemigroup S;
      S.set_alphabet(3);
      S.add_rule({0, 1}, {0});
      S.add_rule({0, 0}, {0});
      Congruence cong(twosided, S);
      cong.add_pair({1, 1}, {1});
      REQUIRE(cong.is_quotient_obviously_infinite());
      REQUIRE(!cong.is_quotient_obviously_finite());
    }
    {
      FpSemigroup S;
      S.set_alphabet(3);
      S.add_rule({0, 1}, {0});
      S.add_rule({0, 0}, {0});
      Congruence cong(twosided, S);
      cong.add_pair({1, 2}, {1});
      REQUIRE(cong.is_quotient_obviously_infinite());
      REQUIRE(!cong.is_quotient_obviously_finite());
    }
    {
      FpSemigroup S;
      S.set_alphabet(3);
      S.add_rule({0, 1}, {0});
      Congruence cong(right, S);
      cong.add_pair({2, 2}, {2});
      REQUIRE(cong.is_quotient_obviously_infinite());
      REQUIRE(!cong.is_quotient_obviously_finite());
    }
    {
      FpSemigroup S;
      S.set_alphabet(3);
      S.add_rule({0, 1}, {0});
      S.add_rule({0, 0}, {0});
      Congruence cong(right, S);
      cong.add_pair({1, 1}, {1});
      REQUIRE(cong.is_quotient_obviously_infinite());
      REQUIRE(!cong.is_quotient_obviously_finite());
    }
    {
      FpSemigroup S;
      S.set_alphabet(3);
      S.add_rule({0, 1}, {0});
      S.add_rule({0, 0}, {0});
      Congruence cong(right, S);
      cong.add_pair({1, 2}, {1});
      REQUIRE(cong.is_quotient_obviously_infinite());
      REQUIRE(!cong.is_quotient_obviously_finite());
    }
    {
      FpSemigroup S;
      S.set_alphabet(3);
      S.add_rule({0, 1}, {0});
      Congruence cong(left, S);
      cong.add_pair({2, 2}, {2});
      REQUIRE(cong.is_quotient_obviously_infinite());
      REQUIRE(!cong.is_quotient_obviously_finite());
    }
    {
      FpSemigroup S;
      S.set_alphabet(3);
      S.add_rule({0, 1}, {0});
      S.add_rule({0, 0}, {0});
      Congruence cong(left, S);
      cong.add_pair({1, 1}, {1});
      REQUIRE(cong.is_quotient_obviously_infinite());
      REQUIRE(!cong.is_quotient_obviously_finite());
    }
    {
      FpSemigroup S;
      S.set_alphabet(3);
      S.add_rule({0, 1}, {0});
      S.add_rule({0, 0}, {0});
      Congruence cong(left, S);
      cong.add_pair({1, 2}, {1});
      REQUIRE(cong.is_quotient_obviously_infinite());
      REQUIRE(!cong.is_quotient_obviously_finite());
    }

    using Transf = TransfHelper<3>::type;
    FroidurePin<Transf> S({Transf({0, 1, 0}), Transf({0, 1, 2})});
    REQUIRE(S.size() == 2);
    {
      Congruence cong(twosided, S);
      cong.add_pair({1}, {0});
      REQUIRE(!cong.is_quotient_obviously_infinite());
      REQUIRE(cong.is_quotient_obviously_finite());
      REQUIRE(cong.nr_classes() == 1);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("Congruence", "027", "less", "[quick][cong]") {
    FpSemigroup S;
    S.set_alphabet(2);
    S.add_rule({0, 0}, {0});

    Congruence cong(twosided, S);
    REQUIRE(!cong.less({0, 0}, {0}));
  }

  LIBSEMIGROUPS_TEST_CASE("Congruence",
                          "028",
                          "2-sided congruences of BMat8 semigroup",
                          "[quick][cong][no-valgrind]") {
    auto rg    = ReportGuard(REPORT);
    using BMat = BMatHelper<4>::type;
    std::vector<BMat> gens
        = {BMat({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
           BMat({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
           BMat({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {1, 0, 0, 1}}),
           BMat({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}})};
    {
      FroidurePin<BMat> S(gens);

      Congruence cong(twosided, S);
      cong.add_pair({1}, {0});

      REQUIRE(cong.nr_classes() == 3);
      REQUIRE(cong.word_to_class_index({1}) == 0);
      REQUIRE(cong.nr_non_trivial_classes() == 3);

      std::vector<size_t> v(cong.nr_non_trivial_classes(), 0);
      std::transform(cong.cbegin_ntc(),
                     cong.cend_ntc(),
                     v.begin(),
                     std::mem_fn(&std::vector<word_type>::size));
      std::sort(v.begin(), v.end());
      REQUIRE(v == std::vector<size_t>({12, 12, 63880}));
      REQUIRE(cong.cbegin_ntc()->size() == 12);
      REQUIRE(std::vector<word_type>(cong.cbegin_ntc()->cbegin(),
                                     cong.cbegin_ntc()->cend())
              == std::vector<word_type>({{0},
                                         {1},
                                         {0, 1, 0},
                                         {0, 1, 1},
                                         {1, 0, 1},
                                         {1, 1, 0},
                                         {1, 1, 1},
                                         {0, 1, 0, 1, 1},
                                         {0, 1, 1, 0, 1},
                                         {1, 0, 1, 1, 0},
                                         {1, 0, 1, 1, 1},
                                         {1, 1, 0, 1, 1}}));
    }
    {
      FroidurePin<BMat> S({gens[0], gens[2], gens[3]});
      Congruence        cong(twosided, S);
      cong.add_pair({1}, {0});

      REQUIRE(cong.nr_classes() == 2);
      REQUIRE(cong.word_to_class_index({1}) == 0);
      REQUIRE(cong.nr_non_trivial_classes() == 2);

      std::vector<size_t> v(cong.nr_non_trivial_classes(), 0);
      std::transform(cong.cbegin_ntc(),
                     cong.cend_ntc(),
                     v.begin(),
                     std::mem_fn(&std::vector<word_type>::size));
      std::sort(v.begin(), v.end());
      REQUIRE(v == std::vector<size_t>({8, 8}));
      REQUIRE(cong.cbegin_ntc()->size() == 8);
      REQUIRE(std::vector<word_type>(cong.cbegin_ntc()->cbegin(),
                                     cong.cbegin_ntc()->cend())
              == std::vector<word_type>({{0},
                                         {1},
                                         {0, 0},
                                         {0, 1},
                                         {1, 0},
                                         {0, 1, 0},
                                         {1, 0, 1},
                                         {0, 1, 0, 1}}));
    }
  }

  LIBSEMIGROUPS_TEST_CASE("Congruence",
                          "029",
                          "left congruence on finite semigroup",
                          "[quick][cong]") {
    auto                  rg = ReportGuard(REPORT);
    std::vector<Element*> gens
        = {new Transformation<uint16_t>({1, 3, 4, 2, 3}),
           new Transformation<uint16_t>({3, 2, 1, 3, 3})};
    FroidurePin<Element const*> S(gens);
    delete_gens(gens);

    // REQUIRE(S.size() == 88);
    // REQUIRE(S.degree() == 5);
    Congruence cong(left, S);
    cong.add_pair({0, 1, 0, 0, 0, 1, 1, 0, 0}, {1, 0, 0, 0, 1});

    REQUIRE(cong.nr_classes() == 69);
    REQUIRE(cong.nr_classes() == 69);

    Element*  t3 = new Transformation<uint16_t>({1, 3, 1, 3, 3});
    Element*  t4 = new Transformation<uint16_t>({4, 2, 4, 4, 2});
    word_type w3 = S.factorisation(t3);
    word_type w4 = S.factorisation(t4);
    REQUIRE(cong.word_to_class_index(w3) != cong.word_to_class_index(w4));
    REQUIRE(cong.word_to_class_index(w3)
            == cong.word_to_class_index({0, 0, 1, 0, 1}));
    REQUIRE(cong.word_to_class_index({1, 0, 0, 1, 0, 1})
            == cong.word_to_class_index({0, 0, 1, 0, 0, 0, 1}));
    REQUIRE(cong.word_to_class_index({0, 1, 1, 0, 0, 0})
            != cong.word_to_class_index({1, 1}));
    REQUIRE(cong.word_to_class_index({1, 0, 0, 0, 1, 0, 0, 0})
            != cong.word_to_class_index({1, 0, 0, 1}));

    REQUIRE(cong.contains({1, 0, 0, 1, 0, 1}, {0, 0, 1, 0, 0, 0, 1}));
    REQUIRE(!cong.contains({1, 0, 0, 0, 1, 0, 0, 0}, {1, 0, 0, 1}));

    REQUIRE(!cong.less({1, 0, 0, 0, 1, 0, 0, 0}, {1, 0, 0, 1}));
    REQUIRE(cong.less({1, 0, 0, 1}, {1, 0, 0, 0, 1, 0, 0, 0}));

    delete t3;
    delete t4;
  }

  LIBSEMIGROUPS_TEST_CASE("Congruence",
                          "030",
                          "right congruence on finite semigroup",
                          "[quick][cong]") {
    auto                  rg = ReportGuard(REPORT);
    std::vector<Element*> gens
        = {new Transformation<uint16_t>({1, 3, 4, 2, 3}),
           new Transformation<uint16_t>({3, 2, 1, 3, 3})};
    FroidurePin<Element const*> S(gens);
    delete_gens(gens);

    // REQUIRE(S.size() == 88);
    // REQUIRE(S.degree() == 5);
    Congruence cong(right, S);
    cong.add_pair({0, 1, 0, 0, 0, 1, 1, 0, 0}, {1, 0, 0, 0, 1});

    REQUIRE(cong.nr_classes() == 72);
    REQUIRE(cong.nr_classes() == 72);

    Element*  t3 = new Transformation<uint16_t>({1, 3, 1, 3, 3});
    Element*  t4 = new Transformation<uint16_t>({4, 2, 4, 4, 2});
    word_type w3 = S.factorisation(t3);
    word_type w4 = S.factorisation(t4);
    REQUIRE(cong.word_to_class_index(w3) != cong.word_to_class_index(w4));
    REQUIRE(cong.word_to_class_index(w3)
            != cong.word_to_class_index({0, 0, 1, 0, 1}));
    REQUIRE(cong.word_to_class_index({1, 0, 0, 1, 0, 1})
            != cong.word_to_class_index({0, 0, 1, 0, 0, 0, 1}));
    REQUIRE(cong.word_to_class_index({0, 1, 1, 0, 0, 0})
            != cong.word_to_class_index({1, 1}));
    REQUIRE(cong.word_to_class_index({1, 0, 0, 0, 1, 0, 0, 0})
            != cong.word_to_class_index({1, 0, 0, 1}));

    REQUIRE(!cong.contains({1, 0, 0, 1, 0, 1}, {0, 0, 1, 0, 0, 0, 1}));
    REQUIRE(!cong.contains({1, 0, 0, 0, 1, 0, 0, 0}, {1, 0, 0, 1}));

    if (!cong.less({1, 0, 0, 0, 1, 0, 0, 0}, {1, 0, 0, 1})) {
      // This depends on which method for cong wins!
      REQUIRE(!cong.less({1, 0, 0, 0, 1, 0, 0, 0}, {1, 0, 0, 1}));
      REQUIRE(cong.less({1, 0, 0, 1}, {1, 0, 0, 0, 1, 0, 0, 0}));
    } else {
      REQUIRE(cong.less({1, 0, 0, 0, 1, 0, 0, 0}, {1, 0, 0, 1}));
      REQUIRE(!cong.less({1, 0, 0, 1}, {1, 0, 0, 0, 1, 0, 0, 0}));
    }

    delete t3;
    delete t4;
  }

  // For some reason the following test case doesn't run..., i.e. it is not run
  // when we run all the tests.
  LIBSEMIGROUPS_TEST_CASE("Congruence",
                          "031",
                          "right congruence on finite semigroup",
                          "[quick][cong]") {
    auto                  rg = ReportGuard(REPORT);
    std::vector<Element*> gens
        = {new Transformation<uint16_t>({1, 3, 4, 2, 3}),
           new Transformation<uint16_t>({3, 2, 1, 3, 3})};
    FroidurePin<Element const*> S = FroidurePin<Element const*>(gens);
    delete_gens(gens);

    REQUIRE(S.size() == 88);
    REQUIRE(S.nr_rules() == 18);
    REQUIRE(S.degree() == 5);
    Element*  t1 = new Transformation<uint16_t>({3, 4, 4, 4, 4});
    Element*  t2 = new Transformation<uint16_t>({3, 1, 3, 3, 3});
    word_type w1, w2;
    S.factorisation(w1, S.position(t1));
    S.factorisation(w2, S.position(t2));
    Congruence cong(right, S);
    cong.add_pair(w1, w2);

    REQUIRE(cong.nr_classes() == 72);
    REQUIRE(cong.nr_classes() == 72);
    Element*  t3 = new Transformation<uint16_t>({1, 3, 3, 3, 3});
    Element*  t4 = new Transformation<uint16_t>({4, 2, 4, 4, 2});
    Element*  t5 = new Transformation<uint16_t>({2, 3, 2, 2, 2});
    Element*  t6 = new Transformation<uint16_t>({2, 3, 3, 3, 3});
    word_type w3, w4, w5, w6;
    S.factorisation(w3, S.position(t3));
    S.factorisation(w4, S.position(t4));
    S.factorisation(w5, S.position(t5));
    S.factorisation(w6, S.position(t6));
    REQUIRE(cong.word_to_class_index(w3) != cong.word_to_class_index(w4));
    REQUIRE(cong.word_to_class_index(w5) == cong.word_to_class_index(w6));
    REQUIRE(cong.word_to_class_index(w3) != cong.word_to_class_index(w6));

    REQUIRE(cong.contains(w1, w2));
    REQUIRE(cong.contains(w5, w6));
    REQUIRE(!cong.contains(w3, w5));

    delete t1;
    delete t2;
    delete t3;
    delete t4;
    delete t5;
    delete t6;
  }

  LIBSEMIGROUPS_TEST_CASE("Congruence", "032", "contains", "[quick][cong]") {
    auto        rg = ReportGuard(REPORT);
    FpSemigroup S;
    S.set_alphabet(2);
    Congruence cong(twosided, S);
    cong.add_pair({0, 0}, {0});
    cong.add_pair({0, 1}, {0});
    cong.add_pair({1, 0}, {0});
    REQUIRE(cong.contains({0, 0}, {0}));
    REQUIRE(cong.contains({0, 1}, {0}));
    REQUIRE(cong.contains({1, 0}, {0}));
    REQUIRE(cong.contains({1, 0}, {0, 1, 0, 1}));
    REQUIRE(!cong.contains({1, 1}, {1}));
  }

  LIBSEMIGROUPS_TEST_CASE("Congruence", "033", "Stellar S2", "[quick][cong]") {
    auto        rg = ReportGuard(REPORT);
    FpSemigroup S;
    S.set_alphabet(3);
    for (relation_type const& rl : RookMonoid(2, 0)) {
      S.add_rule(rl);
    }

    REQUIRE(S.nr_rules() == 9);
    REQUIRE(!S.is_obviously_infinite());
    REQUIRE(S.knuth_bendix()->confluent());
    REQUIRE(S.size() == 7);
    REQUIRE(S.froidure_pin()->size() == 7);

    Congruence cong(twosided, S);
    for (relation_type const& rl : Stell(2)) {
      cong.add_pair(rl.first, rl.second);
    }
    REQUIRE(!cong.is_quotient_obviously_infinite());
    REQUIRE(cong.nr_classes() == 5);
    REQUIRE(cong.nr_non_trivial_classes() == 1);

    std::vector<word_type> v(cong.cbegin_ntc()->cbegin(),
                             cong.cbegin_ntc()->cend());
    std::sort(v.begin(), v.end());
    REQUIRE(v == std::vector<word_type>({{0, 1, 0}, {1, 0}, {1, 0, 1}}));
  }

  LIBSEMIGROUPS_TEST_CASE("Congruence", "034", "Stellar S3", "[quick][cong]") {
    auto        rg = ReportGuard(REPORT);
    FpSemigroup S;
    S.set_alphabet(4);
    for (relation_type const& rl : RookMonoid(3, 0)) {
      S.add_rule(rl);
    }

    REQUIRE(S.nr_rules() == 15);
    REQUIRE(!S.is_obviously_infinite());
    REQUIRE(!S.knuth_bendix()->confluent());
    REQUIRE(S.size() == 34);
    REQUIRE(S.froidure_pin()->size() == 34);

    Congruence cong(twosided, S);
    for (relation_type const& rl : Stell(3)) {
      cong.add_pair(rl.first, rl.second);
    }
    REQUIRE(!cong.is_quotient_obviously_infinite());
    REQUIRE(cong.nr_classes() == 16);
    REQUIRE(cong.nr_non_trivial_classes() == 4);

    using non_trivial_classes_type = Congruence::non_trivial_classes_type;

    non_trivial_classes_type v;
    v.reserve(cong.nr_non_trivial_classes());
    for (auto it = cong.cbegin_ntc(); it < cong.cend_ntc(); ++it) {
      v.push_back(std::vector<word_type>(it->cbegin(), it->cend()));
      std::sort(v.back().begin(), v.back().end());
    }
    std::sort(v.begin(), v.end());

    REQUIRE(v
            == non_trivial_classes_type(
                {{{0, 1, 0}, {1, 0}, {1, 0, 1}},
                 {{0, 1, 0, 2}, {1, 0, 1, 2}, {1, 0, 2}},
                 {{0, 1, 0, 2, 1}, {1, 0, 1, 2, 1}, {1, 0, 2, 1}},
                 {{0, 1, 0, 2, 1, 0},
                  {0, 1, 2, 1, 0},
                  {0, 1, 2, 1, 0, 1},
                  {0, 2, 1, 0},
                  {1, 0, 1, 2, 1, 0},
                  {1, 0, 1, 2, 1, 0, 1},
                  {1, 0, 2, 1, 0},
                  {1, 2, 1, 0},
                  {1, 2, 1, 0, 1},
                  {1, 2, 1, 0, 1, 2},
                  {2, 1, 0},
                  {2, 1, 0, 1},
                  {2, 1, 0, 1, 2}}}));
  }

  LIBSEMIGROUPS_TEST_CASE("Congruence", "035", "Stellar S4", "[quick][cong]") {
    auto        rg = ReportGuard(REPORT);
    FpSemigroup S;
    S.set_alphabet(5);
    for (relation_type const& rl : RookMonoid(4, 0)) {
      S.add_rule(rl);
    }

    REQUIRE(S.nr_rules() == 23);
    REQUIRE(!S.is_obviously_infinite());
    REQUIRE(!S.knuth_bendix()->confluent());
    REQUIRE(S.size() == 209);
    REQUIRE(S.froidure_pin()->size() == 209);

    Congruence cong(twosided, S);
    for (relation_type const& rl : Stell(4)) {
      cong.add_pair(rl.first, rl.second);
    }
    REQUIRE(!cong.is_quotient_obviously_infinite());
    REQUIRE(cong.nr_classes() == 65);
    REQUIRE(cong.nr_non_trivial_classes() == 17);

    std::vector<size_t> v(cong.nr_non_trivial_classes(), 0);
    std::transform(cong.cbegin_ntc(),
                   cong.cend_ntc(),
                   v.begin(),
                   std::mem_fn(&std::vector<word_type>::size));
    std::sort(v.begin(), v.end());
    REQUIRE(v
            == std::vector<size_t>(
                {3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 13, 13, 13, 13, 73}));
    REQUIRE(std::accumulate(v.cbegin(), v.cend(), 0)
                + (cong.nr_classes() - cong.nr_non_trivial_classes())
            == 209);
  }

  LIBSEMIGROUPS_TEST_CASE("Congruence",
                          "036",
                          "Stellar S5",
                          "[quick][cong][no-valgrind]") {
    auto        rg = ReportGuard(REPORT);
    FpSemigroup S;
    S.set_alphabet(6);
    for (relation_type const& rl : RookMonoid(5, 0)) {
      S.add_rule(rl);
    }

    REQUIRE(S.nr_rules() == 33);
    REQUIRE(!S.is_obviously_infinite());
    REQUIRE(!S.knuth_bendix()->confluent());
    REQUIRE(S.size() == 1546);
    REQUIRE(S.froidure_pin()->size() == 1546);

    Congruence cong(twosided, S);
    for (relation_type const& rl : Stell(5)) {
      cong.add_pair(rl.first, rl.second);
    }
    REQUIRE(!cong.is_quotient_obviously_infinite());
    REQUIRE(cong.nr_classes() == 326);
    REQUIRE(cong.nr_non_trivial_classes() == 86);

    std::vector<size_t> v(cong.nr_non_trivial_classes(), 0);
    std::transform(cong.cbegin_ntc(),
                   cong.cend_ntc(),
                   v.begin(),
                   std::mem_fn(&std::vector<word_type>::size));
    REQUIRE(std::count(v.cbegin(), v.cend(), 3) == 60);
    REQUIRE(std::count(v.cbegin(), v.cend(), 13) == 20);
    REQUIRE(std::count(v.cbegin(), v.cend(), 73) == 5);
    REQUIRE(std::count(v.cbegin(), v.cend(), 501) == 1);
    REQUIRE(std::accumulate(v.cbegin(), v.cend(), 0)
                + (cong.nr_classes() - cong.nr_non_trivial_classes())
            == S.size());
  }

  LIBSEMIGROUPS_TEST_CASE("Congruence",
                          "037",
                          "Stellar S6",
                          "[quick][cong][no-valgrind]") {
    auto        rg = ReportGuard(REPORT);
    FpSemigroup S;
    S.set_alphabet(7);
    for (relation_type const& rl : RookMonoid(6, 0)) {
      S.add_rule(rl);
    }

    REQUIRE(S.nr_rules() == 45);
    REQUIRE(!S.is_obviously_infinite());
    REQUIRE(!S.knuth_bendix()->confluent());
    REQUIRE(S.size() == 13327);

    Congruence cong(twosided, S);
    for (relation_type const& rl : Stell(6)) {
      cong.add_pair(rl.first, rl.second);
    }
    REQUIRE(!cong.is_quotient_obviously_infinite());
    REQUIRE(cong.nr_classes() == 1957);
    REQUIRE(cong.nr_non_trivial_classes() == 517);

    std::vector<size_t> v(cong.nr_non_trivial_classes(), 0);
    std::transform(cong.cbegin_ntc(),
                   cong.cend_ntc(),
                   v.begin(),
                   std::mem_fn(&std::vector<word_type>::size));
    REQUIRE(std::accumulate(v.cbegin(), v.cend(), 0)
                + (cong.nr_classes() - cong.nr_non_trivial_classes())
            == S.size());
  }

  LIBSEMIGROUPS_TEST_CASE("Congruence",
                          "038",
                          "Stellar S7",
                          "[standard][cong]") {
    auto        rg = ReportGuard(REPORT);
    FpSemigroup S;
    S.set_alphabet(8);
    for (relation_type const& rl : RookMonoid(7, 0)) {
      S.add_rule(rl);
    }

    REQUIRE(S.nr_rules() == 59);
    REQUIRE(!S.is_obviously_infinite());
    REQUIRE(!S.knuth_bendix()->confluent());
    REQUIRE(S.size() == 130922);

    Congruence cong(twosided, S);
    for (relation_type const& rl : Stell(7)) {
      cong.add_pair(rl.first, rl.second);
    }
    REQUIRE(!cong.is_quotient_obviously_infinite());
    REQUIRE(cong.nr_classes() == 13700);
    REQUIRE(cong.nr_non_trivial_classes() == 3620);

    std::vector<size_t> v(cong.nr_non_trivial_classes(), 0);
    std::transform(cong.cbegin_ntc(),
                   cong.cend_ntc(),
                   v.begin(),
                   std::mem_fn(&std::vector<word_type>::size));
    REQUIRE(std::accumulate(v.cbegin(), v.cend(), 0)
                + (cong.nr_classes() - cong.nr_non_trivial_classes())
            == S.size());
  }

  LIBSEMIGROUPS_TEST_CASE("Congruence",
                          "039",
                          "left cong. on an f.p. semigroup",
                          "[quick][cong]") {
    auto rg = ReportGuard(REPORT);

    FpSemigroup S;
    S.set_alphabet("abe");
    S.set_identity("e");
    S.add_rule("abb", "bb");
    S.add_rule("bbb", "bb");
    S.add_rule("aaaa", "a");
    S.add_rule("baab", "bb");
    S.add_rule("baaab", "b");
    S.add_rule("babab", "b");
    S.add_rule("bbaaa", "bb");
    S.add_rule("bbaba", "bbaa");

    REQUIRE(S.knuth_bendix()->confluent());
    REQUIRE(S.knuth_bendix()->nr_rules() == 13);

    KnuthBendixCongruenceByPairs kbp(left, S.knuth_bendix());
    // kbp.add_pair({0}, {1, 1, 1});
    kbp.add_pair({1, 1}, {0, 0, 0, 0, 0, 0, 0});

    REQUIRE(kbp.nr_classes() == 11);

    Congruence cong1(left, S);
    cong1.add_pair({0}, {1, 1, 1});
    REQUIRE(cong1.nr_classes() == 11);

    Congruence cong2(left, S);
    cong2.add_pair({1, 1}, {0, 0, 0, 0, 0, 0, 0});
    REQUIRE(cong1.nr_classes() == cong2.nr_classes());
  }

  LIBSEMIGROUPS_TEST_CASE("Congruence",
                          "040",
                          "2-sided cong. on infinite f.p. semigroup",
                          "[quick][cong]") {
    auto        rg = ReportGuard(REPORT);
    FpSemigroup S;
    S.set_alphabet(3);

    Congruence cong(twosided, S);
    cong.add_pair({1}, {2});
    cong.add_pair({0, 0}, {0});
    cong.add_pair({0, 1}, {1, 0});
    cong.add_pair({0, 1}, {1});
    cong.add_pair({0, 2}, {2, 0});
    cong.add_pair({0, 2}, {2});

    REQUIRE(!cong.contains({1}, {2, 2, 2, 2, 2, 2, 2, 2, 2, 2}));
  }
  LIBSEMIGROUPS_TEST_CASE("Congruence",
                          "041",
                          "2-sided congruence constructed from type only",
                          "[quick][cong]") {
    auto       rg = ReportGuard(REPORT);
    Congruence cong(twosided);
    REQUIRE(cong.nr_generators() == UNDEFINED);
    REQUIRE_THROWS_AS(cong.set_nr_generators(0), LibsemigroupsException);
    REQUIRE_THROWS_AS(cong.contains({1}, {2, 2, 2, 2, 2, 2, 2, 2, 2, 2}),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(cong.const_contains({1}, {2, 2, 2, 2, 2, 2, 2, 2, 2, 2}),
                      LibsemigroupsException);
    REQUIRE(cong.nr_classes() == UNDEFINED);
    REQUIRE_THROWS_AS(cong.word_to_class_index({2, 2, 2, 2}),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(cong.class_index_to_word(0), LibsemigroupsException);
    REQUIRE_THROWS_AS(cong.class_index_to_word(1), LibsemigroupsException);
    REQUIRE_THROWS_AS(cong.class_index_to_word(2), LibsemigroupsException);
    REQUIRE_THROWS_AS(cong.run(), LibsemigroupsException);
    REQUIRE(!cong.started());
    REQUIRE(!cong.finished());

    REQUIRE_NOTHROW(cong.set_nr_generators(2));

    REQUIRE_NOTHROW(cong.add_pair({0, 0, 0}, {0}));
    REQUIRE_NOTHROW(cong.add_pair({0}, {1, 1}));
    REQUIRE_THROWS_AS(!cong.contains({1}, {2, 2, 2, 2, 2, 2, 2, 2, 2, 2}),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(cong.const_contains({1}, {2, 2, 2, 2, 2, 2, 2, 2, 2, 2}),
                      LibsemigroupsException);
    REQUIRE(cong.const_contains({0}, {1, 1}) == tril::TRUE);
    REQUIRE_NOTHROW(cong.run());
    REQUIRE(!cong.contains({1}, {0}));
    REQUIRE(cong.contains({0}, {1, 1}));
    REQUIRE(cong.nr_classes() == 5);
    REQUIRE_THROWS_AS(cong.word_to_class_index({2, 2, 2, 2}),
                      LibsemigroupsException);
    REQUIRE(cong.word_to_class_index({1, 1, 1, 1}) == 2);
    REQUIRE(cong.class_index_to_word(0) == word_type({0}));
    REQUIRE(cong.class_index_to_word(1) == word_type({1}));
    REQUIRE(cong.class_index_to_word(2) == word_type({0, 0}));
    REQUIRE(cong.started());
    REQUIRE(cong.finished());
  }

  LIBSEMIGROUPS_TEST_CASE("Congruence",
                          "043",
                          "const_contains",
                          "[quick][cong]") {
    auto       rg = ReportGuard(REPORT);
    Congruence cong(twosided);
    cong.set_nr_generators(2);
    cong.add_pair({0, 0, 0}, {0});
    cong.add_pair({1, 1, 1, 1}, {1});
    cong.add_pair({0, 1, 1, 1, 0}, {0, 0});
    cong.add_pair({1, 0, 0, 1}, {1, 1});
    cong.add_pair({0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0}, {0, 0});

    REQUIRE(cong.const_contains({1, 1, 1, 1}, {1}) == tril::TRUE);
    REQUIRE(cong.const_contains({1, 1, 1, 1}, {1, 1}) == tril::unknown);
    REQUIRE(!cong.contains({1, 1, 1, 1}, {1, 1}));
    REQUIRE(cong.finished());
    if (cong.has_todd_coxeter()) {
      REQUIRE_NOTHROW(cong.todd_coxeter());
    }
    if (cong.has_knuth_bendix()) {
      REQUIRE_NOTHROW(cong.knuth_bendix());
    }
  }

  LIBSEMIGROUPS_TEST_CASE("Congruence", "042", "no winner", "[quick][cong]") {
    Congruence cong(twosided);
    cong.set_nr_generators(4);
    REQUIRE_THROWS_AS(cong.word_to_class_index({2, 2, 2, 2}),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(cong.class_index_to_word(2), LibsemigroupsException);
    REQUIRE_THROWS_AS(cong.quotient_froidure_pin(), LibsemigroupsException);
    REQUIRE_THROWS_AS(cong.cbegin_ntc(), LibsemigroupsException);
  }

  // The next 3 test cases are commented out because they test features we
  // decided not to include in v1.0.0.

  // LIBSEMIGROUPS_TEST_CASE("Congruence",
  //                         "043",
  //                         "Policy::none edge cases",
  //                         "[quick][cong]") {
  //   auto       rg = ReportGuard(REPORT);
  //   std::unique_ptr<Congruence> cong;
  //   SECTION("Congruence(congruence_type)") {
  //     cong = detail::make_unique<Congruence>(twosided);
  //   }
  //   SECTION("Congruence(congruence_type, FpSemigroup const&)") {
  //     FpSemigroup S;
  //     S.set_alphabet(2);
  //     cong = detail::make_unique<Congruence>(twosided, S);
  //   }
  //   REQUIRE(!cong->empty());
  //   REQUIRE_THROWS_AS(cong->set_nr_generators(0), LibsemigroupsException);
  //   REQUIRE_THROWS_AS(!cong->contains({1}, {2, 2, 2, 2, 2, 2, 2, 2, 2, 2}),
  //                     LibsemigroupsException);
  //   REQUIRE(cong->const_contains({1}, {2, 2, 2, 2, 2, 2, 2, 2, 2, 2})
  //           == tril::unknown);
  //   REQUIRE_THROWS_AS(cong->nr_classes(), LibsemigroupsException);
  //   REQUIRE_THROWS_AS(cong->word_to_class_index({2, 2, 2, 2}),
  //                     LibsemigroupsException);
  //   REQUIRE_THROWS_AS(cong->class_index_to_word(0), LibsemigroupsException);
  //   REQUIRE_THROWS_AS(cong->class_index_to_word(1), LibsemigroupsException);
  //   REQUIRE_THROWS_AS(cong->class_index_to_word(2), LibsemigroupsException);
  //   REQUIRE_THROWS_AS(cong->run(), LibsemigroupsException);
  // }

  // LIBSEMIGROUPS_TEST_CASE("Congruence",
  //                         "044",
  //                         "Policy::none",
  //                         "[quick][cong]") {
  //   auto rg      = ReportGuard(REPORT);

  //   using Transf = TransfHelper<5>::type;
  //   FroidurePin<Transf> S({Transf({1, 3, 4, 2, 3}), Transf({3, 2, 1, 3,
  //   3})}); REQUIRE(S.size() == 88);

  //   Congruence cong(twosided, S, Congruence::Policy::none);
  //   REQUIRE(!cong.is_quotient_obviously_infinite());

  //   SECTION("ToddCoxeter") {
  //     congruence::ToddCoxeter tc(twosided, S);
  //     REQUIRE(tc.is_quotient_obviously_finite());
  //     REQUIRE(!tc.is_quotient_obviously_infinite());

  //     SECTION("add pair before copy") {
  //       tc.add_pair({0}, {0, 0});
  //       cong.add_runner(tc);
  //       REQUIRE(cong.nr_classes() == 6);
  //     }
  //     SECTION("add pair after copy") {
  //       cong.add_runner(tc);
  //       tc.add_pair({0}, {0, 0});
  //       cong.add_pair({0}, {0, 0});
  //       REQUIRE(cong.nr_classes() == 6);
  //     }
  //     SECTION("no added pairs") {
  //       cong.add_runner(tc);
  //       tc.add_pair({0}, {0, 0});
  //       REQUIRE(cong.nr_classes() == 88);
  //     }
  //     REQUIRE(tc.nr_classes() == 6);
  //   }
  //   SECTION("KnuthBendix") {
  //     congruence::KnuthBendix kb(S);
  //     REQUIRE(kb.is_quotient_obviously_finite());
  //     REQUIRE(!kb.is_quotient_obviously_infinite());

  //     SECTION("add pair before copy") {
  //       kb.add_pair({0}, {0, 0});
  //       cong.add_runner(kb);
  //       REQUIRE(cong.nr_classes() == 6);
  //     }
  //     SECTION("add pair after copy") {
  //       cong.add_runner(kb);
  //       kb.add_pair({0}, {0, 0});
  //       cong.add_pair({0}, {0, 0});
  //       REQUIRE(cong.nr_classes() == 6);
  //     }
  //     SECTION("no added pairs") {
  //       cong.add_runner(kb);
  //       kb.add_pair({0}, {0, 0});
  //       REQUIRE(cong.nr_classes() == 88);
  //     }
  //     REQUIRE(kb.nr_classes() == 6);
  //   }
  //   SECTION("ToddCoxeter + KnuthBendix") {
  //     congruence::ToddCoxeter tc(twosided, S);
  //     congruence::KnuthBendix kb(S);

  //     SECTION("add pair before copy") {
  //       tc.add_pair({0}, {0, 0});
  //       kb.add_pair({0}, {0, 0});
  //       cong.add_runner(tc);
  //       cong.add_runner(kb);
  //       REQUIRE(cong.number_runners() == 2);
  //       REQUIRE(cong.nr_classes() == 6);
  //     }
  //     SECTION("add pair after copy") {
  //       cong.add_runner(tc);
  //       cong.add_runner(kb);
  //       REQUIRE(cong.number_runners() == 2);
  //       tc.add_pair({0}, {0, 0});
  //       kb.add_pair({0}, {0, 0});
  //       cong.add_pair({0}, {0, 0});
  //       REQUIRE(cong.nr_classes() == 6);
  //     }
  //     SECTION("no added pairs") {
  //       cong.add_runner(tc);
  //       cong.add_runner(kb);
  //       REQUIRE(cong.number_runners() == 2);
  //       tc.add_pair({0}, {0, 0});
  //       kb.add_pair({0}, {0, 0});
  //       REQUIRE(cong.nr_classes() == 88);
  //     }
  //     REQUIRE(tc.nr_classes() == 6);
  //     REQUIRE(kb.nr_classes() == 6);
  //   }
  //   REQUIRE(!cong.is_quotient_obviously_infinite());
  //   REQUIRE(cong.is_quotient_obviously_finite());
  // }

  // LIBSEMIGROUPS_TEST_CASE("Congruence",
  //                         "045",
  //                         "add_runner exceptions",
  //                         "[quick][cong]") {
  //   auto rg      = ReportGuard(REPORT);
  //   using Transf = TransfHelper<5>::type;
  //   FroidurePin<Transf> S({Transf({1, 3, 4, 2, 3}), Transf({3, 2, 1, 3,
  //   3})}); REQUIRE(S.size() == 88);

  //   SECTION("add runner after finished") {
  //     Congruence cong(twosided, S);
  //     REQUIRE(cong.nr_classes() == 88);
  //     congruence::ToddCoxeter tc(twosided, S);
  //     REQUIRE_THROWS_AS(cong.add_runner(tc), LibsemigroupsException);
  //   }
  //   SECTION("add before nr_generators is set") {
  //     Congruence cong(twosided);
  //     congruence::ToddCoxeter tc(twosided, S);
  //     REQUIRE_THROWS_AS(cong.add_runner(tc), LibsemigroupsException);
  //   }
  //   SECTION("add_runner when initialized with generators") {
  //     Congruence cong(twosided);
  //     cong.set_nr_generators(2);
  //     REQUIRE(!cong.is_quotient_obviously_finite());
  //     REQUIRE(cong.is_quotient_obviously_infinite());

  //     congruence::ToddCoxeter tc(twosided, S);
  //     REQUIRE_THROWS_AS(cong.add_runner(tc), LibsemigroupsException);
  //     REQUIRE(tc.nr_classes() == 88);
  //     REQUIRE(cong.nr_classes() == POSITIVE_INFINITY);
  //   }
  //   SECTION("add_runner when initialized with generators + pairs") {
  //     Congruence cong(twosided);
  //     cong.set_nr_generators(2);
  //     cong.add_pair({0, 0}, {0});
  //     REQUIRE(!cong.is_quotient_obviously_finite());
  //     REQUIRE(cong.is_quotient_obviously_infinite());

  //     congruence::ToddCoxeter tc(twosided, S);
  //     REQUIRE_THROWS_AS(cong.add_runner(tc), LibsemigroupsException);
  //     REQUIRE(tc.nr_classes() == 88);
  //     REQUIRE(cong.nr_classes() == POSITIVE_INFINITY);
  //   }
  //   SECTION("add_runner when not initialized + Policy::none") {
  //     Congruence cong(twosided, Congruence::Policy::none);

  //     size_t     old_nr_runners = cong.number_runners();
  //     REQUIRE(cong.number_runners() == 0);
  //     REQUIRE(!cong.is_quotient_obviously_finite());
  //     REQUIRE(!cong.is_quotient_obviously_infinite());
  //     REQUIRE(cong.nr_generators() == UNDEFINED);
  //     {
  //       congruence::ToddCoxeter tc(twosided, S);
  //       REQUIRE_THROWS_AS(cong.add_runner(tc), LibsemigroupsException);
  //       REQUIRE(tc.nr_classes() == 88);
  //     }
  //     {
  //       congruence::ToddCoxeter tc(twosided);
  //       REQUIRE_NOTHROW(cong.add_runner(tc));
  //       REQUIRE(cong.number_runners() == old_nr_runners + 1);
  //       congruence::KnuthBendix kb;
  //       REQUIRE_NOTHROW(cong.add_runner(kb));
  //       REQUIRE(cong.number_runners() == old_nr_runners + 2);
  //     }
  //     REQUIRE(!cong.is_quotient_obviously_finite());
  //     REQUIRE(!cong.is_quotient_obviously_infinite());
  //     REQUIRE(cong.nr_generators() == S.nr_generators());
  //     cong.add_pair({0, 0}, {0});
  //     REQUIRE(cong.nr_classes() == 6);
  //   }
  //   SECTION("add_runner when not initialized + Policy::standard") {
  //     Congruence cong(twosided, Congruence::Policy::standard);
  //     size_t     old_nr_runners = cong.number_runners();
  //     REQUIRE(cong.number_runners() > 0);
  //     REQUIRE(!cong.is_quotient_obviously_finite());
  //     REQUIRE(!cong.is_quotient_obviously_infinite());
  //     REQUIRE(cong.nr_generators() == UNDEFINED);
  //     {
  //       congruence::ToddCoxeter tc(twosided, S);
  //       REQUIRE_THROWS_AS(cong.add_runner(tc), LibsemigroupsException);
  //       REQUIRE(tc.nr_classes() == 88);
  //     }
  //     {
  //       congruence::ToddCoxeter tc(twosided);
  //       REQUIRE_THROWS_AS(cong.add_runner(tc), LibsemigroupsException);
  //       REQUIRE(cong.number_runners() == old_nr_runners);
  //     }
  //     REQUIRE(!cong.is_quotient_obviously_finite());
  //     REQUIRE(!cong.is_quotient_obviously_infinite());
  //     REQUIRE(cong.nr_generators() == S.nr_generators());
  //     cong.add_pair({0, 0}, {0});
  //     REQUIRE(cong.nr_classes() == 6);
  //   }
  // }

}  // namespace libsemigroups
