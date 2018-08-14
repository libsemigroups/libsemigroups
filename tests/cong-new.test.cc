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

#include "libsemigroups.tests.hpp"

#include "include/internal/stl.h"

#include "bmat8.h"
#include "cong-new.h"
#include "cong-p.h"
#include "element-helper.h"
#include "fpsemi.h"
#include "semigroup.h"

namespace libsemigroups {
  namespace tmp {
    template <class TElementType>
    void delete_gens(std::vector<TElementType>& gens) {
      for (auto& x : gens) {
        delete x;
      }
    }

    constexpr bool REPORT = false;

    constexpr congruence_type TWOSIDED = congruence_type::TWOSIDED;
    constexpr congruence_type LEFT     = congruence_type::LEFT;
    constexpr congruence_type RIGHT    = congruence_type::RIGHT;

    LIBSEMIGROUPS_TEST_CASE("Congruence 000",
                            "left congruence on fp semigroup",
                            "[quick]") {
      REPORTER.set_report(REPORT);
      FpSemigroup S;
      S.set_alphabet(2);
      S.add_rule(word_type({0, 0, 0}), word_type({0}));
      S.add_rule(word_type({0}), word_type({1, 1}));

      Congruence cong(LEFT, S);
    }

    LIBSEMIGROUPS_TEST_CASE("Congruence 001",
                            "2-sided congruence on fp semigroup",
                            "[quick]") {
      REPORTER.set_report(REPORT);

      FpSemigroup S;
      S.set_alphabet(2);
      S.add_rule({0, 0, 0}, {0});
      S.add_rule({0}, {1, 1});

      Congruence cong(TWOSIDED, S);

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

    LIBSEMIGROUPS_TEST_CASE("Congruence 002",
                            "left congruence on fp semigroup",
                            "[quick]") {
      REPORTER.set_report(REPORT);
      FpSemigroup S;
      S.set_alphabet(2);
      S.add_rule({0, 0, 0}, {0});  // (a^3, a)
      S.add_rule({0}, {1, 1});     // (a, b^2)

      Congruence cong(LEFT, S);
      REQUIRE(cong.nr_classes() == 5);
    }

    LIBSEMIGROUPS_TEST_CASE("Congruence 003",
                            "word_to_class_index for cong. on fp semigroup",
                            "[quick]") {
      REPORTER.set_report(REPORT);

      FpSemigroup S;
      S.set_alphabet(2);
      S.add_rule({0, 0, 0}, {0});  // (a^3, a)
      S.add_rule({0}, {1, 1});     // (a, b^2)

      Congruence cong(LEFT, S);
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

    LIBSEMIGROUPS_TEST_CASE("Congruence 004",
                            "word_to_class_index for cong. on fp semigroup",
                            "[quick]") {
      REPORTER.set_report(REPORT);
      FpSemigroup S;
      S.set_alphabet(2);
      S.add_rule({0, 0, 0}, {0});  // (a^3, a)
      S.add_rule({0}, {1, 1});     // (a, b^2)

      Congruence cong1(TWOSIDED, S);

      REQUIRE(cong1.word_to_class_index({0, 0, 1})
              == cong1.word_to_class_index({0, 0, 0, 0, 1}));
      REQUIRE(cong1.word_to_class_index({0, 1, 1, 0, 0, 1})
              == cong1.word_to_class_index({0, 0, 0, 0, 1}));
      REQUIRE(cong1.word_to_class_index({0, 0, 0})
              == cong1.word_to_class_index({1, 1}));
      REQUIRE(cong1.word_to_class_index({1}) != cong1.word_to_class_index({0}));

      Congruence cong2(TWOSIDED, S);

      REQUIRE(cong2.word_to_class_index({0, 0, 0, 0})
              == cong2.word_to_class_index({0, 0}));
      REQUIRE(cong2.contains({0, 0, 0, 0}, {0, 1, 1, 0, 1, 1}));
    }

    LIBSEMIGROUPS_TEST_CASE("Congruence 005",
                            "trivial congruence on non-fp semigroup",
                            "[quick]") {
      REPORTER.set_report(REPORT);

      using Transf = Transf<5>::type;
      Semigroup<Transf> S({Transf({1, 3, 4, 2, 3}), Transf({3, 2, 1, 3, 3})});
      REQUIRE(S.size() == 88);

      Congruence cong(TWOSIDED, S);
      REQUIRE(cong.nr_classes() == 88);
    }

    LIBSEMIGROUPS_TEST_CASE("Congruence 006",
                            "2-sided congruence on non-fp semigroup",
                            "[quick]") {
      REPORTER.set_report(REPORT);

      using Transf = Transf<5>::type;
      Semigroup<Transf> S({Transf({1, 3, 4, 2, 3}), Transf({3, 2, 1, 3, 3})});
      REQUIRE(S.size() == 88);

      Congruence cong(TWOSIDED, S);
      cong.add_pair(S.factorisation(Transf({3, 4, 4, 4, 4})),
                    S.factorisation(Transf({3, 1, 3, 3, 3})));
      REQUIRE(cong.nr_classes() == 21);
    }

    LIBSEMIGROUPS_TEST_CASE("Congruence 007",
                            "2-sided congruence on fp semigroup",
                            "[quick]") {
      REPORTER.set_report(REPORT);
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
      REQUIRE(S.isomorphic_non_fp_semigroup()->size() == 2);

      Congruence cong1(TWOSIDED, S.isomorphic_non_fp_semigroup());
      cong1.add_pair({0}, {1});
      REQUIRE(cong1.nr_classes() == 2);

      Congruence cong2(TWOSIDED, S);
      cong2.add_pair({0}, {1});
      REQUIRE(cong2.nr_classes() == 2);
    }

    LIBSEMIGROUPS_TEST_CASE("Congruence 008",
                            "2-sided congruence on infinite fp semigroup",
                            "[quick]") {
      REPORTER.set_report(REPORT);
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

      Congruence cong(TWOSIDED, S);
      cong.add_pair({0}, {1});

      REQUIRE(cong.word_to_class_index({0}) == cong.word_to_class_index({1}));
      REQUIRE(cong.word_to_class_index({0})
              == cong.word_to_class_index({1, 0}));
      REQUIRE(cong.word_to_class_index({0})
              == cong.word_to_class_index({1, 1}));
      REQUIRE(cong.word_to_class_index({0})
              == cong.word_to_class_index({1, 0, 1}));

      REQUIRE(cong.contains({1}, {1, 1}));
      REQUIRE(cong.contains({1, 0, 1}, {1, 0}));
      REQUIRE(cong.nr_classes() == 2);
    }

    LIBSEMIGROUPS_TEST_CASE("Congruence 009",
                            "2-sided congruence on infinite fp semigroup",
                            "[quick]") {
      REPORTER.set_report(REPORT);
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

      Congruence cong(TWOSIDED, S);
      cong.add_pair({0}, {1});

      // Requires KBP to work
      REQUIRE(cong.word_to_class_index({0}) == cong.word_to_class_index({1}));
      REQUIRE(cong.word_to_class_index({0})
              == cong.word_to_class_index({1, 0}));
      REQUIRE(cong.word_to_class_index({0})
              == cong.word_to_class_index({1, 1}));
      REQUIRE(cong.word_to_class_index({0})
              == cong.word_to_class_index({1, 0, 1}));

      REQUIRE(cong.contains({1}, {1, 1}));
      REQUIRE(cong.contains({1, 0, 1}, {1, 0}));

      REQUIRE(!cong.less({1, 0, 1}, {1, 0}));
    }

    LIBSEMIGROUPS_TEST_CASE("Congruence 010",
                            "2-sided congruence on finite semigroup",
                            "[quick]") {
      REPORTER.set_report(REPORT);
      using Transf = Transf<8>::type;
      Semigroup<Transf> S({Transf({7, 3, 5, 3, 4, 2, 7, 7}),
                           Transf({1, 2, 4, 4, 7, 3, 0, 7}),
                           Transf({0, 6, 4, 2, 2, 6, 6, 4}),
                           Transf({3, 6, 3, 4, 0, 6, 0, 7})});

      // The following lines are intentionally commented out.
      // REQUIRE(S.size() == 11804);
      // REQUIRE(S.nrrules() == 2460);

      Congruence cong(TWOSIDED, S);
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

    LIBSEMIGROUPS_TEST_CASE("Congruence 011",
                            "congruence on full PBR monoid on 2 points",
                            "[extreme]") {
      REPORTER.set_report(true);
      Semigroup<PBR> S({PBR({{2}, {3}, {0}, {1}}),
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
      // REQUIRE(S.nrrules() == 45416);

      Congruence cong(TWOSIDED, S);
      cong.add_pair({7, 10, 9, 3, 6, 9, 4, 7, 9, 10},
                    {9, 3, 6, 6, 10, 9, 4, 7});
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

    LIBSEMIGROUPS_TEST_CASE("Congruence 012",
                            "2-sided congruence on finite semigroup",
                            "[quick]") {
      REPORTER.set_report(REPORT);

      using PPerm = PPerm<6>::type;

      Semigroup<PPerm> S({PPerm({0, 1, 2}, {4, 0, 1}, 6),
                          PPerm({0, 1, 2, 3, 5}, {2, 5, 3, 0, 4}, 6),
                          PPerm({0, 1, 2, 3}, {5, 0, 3, 1}, 6),
                          PPerm({0, 2, 5}, {3, 4, 1}, 6),
                          PPerm({0, 2, 5}, {0, 2, 5}, 6),
                          PPerm({0, 1, 4}, {1, 2, 0}, 6),
                          PPerm({0, 2, 3, 4, 5}, {3, 0, 2, 5, 1}, 6),
                          PPerm({0, 1, 3, 5}, {1, 3, 2, 0}, 6),
                          PPerm({1, 3, 4}, {5, 0, 2}, 6)});

      // REQUIRE(S.size() == 712);
      // REQUIRE(S.nrrules() == 1121);

      Congruence cong(TWOSIDED, S);
      cong.add_pair({2, 7}, {1, 6, 6, 1});
      REQUIRE(cong.nr_classes() == 32);
    }

    LIBSEMIGROUPS_TEST_CASE("Congruence 013",
                            "trivial 2-sided congruence on bicyclic monoid",
                            "[quick]") {
      REPORTER.set_report(REPORT);
      FpSemigroup S;
      S.set_alphabet(3);
      S.set_identity(0);
      S.add_rule({1, 2}, {0});
      Congruence cong(TWOSIDED, S);
      REQUIRE(cong.word_to_class_index({0})
              == cong.word_to_class_index({1, 2, 1, 1, 2, 2}));
      REQUIRE(cong.word_to_class_index({0})
              == cong.word_to_class_index({1, 0, 2, 0, 1, 2}));
      REQUIRE(cong.word_to_class_index({2, 1})
              == cong.word_to_class_index({1, 2, 0, 2, 1, 1, 2}));
      REQUIRE(cong.contains({2, 1}, {1, 2, 0, 2, 1, 1, 2}));
    }

    LIBSEMIGROUPS_TEST_CASE("Congruence 014",
                            "non-trivial 2-sided congruence on bicyclic monoid",
                            "[quick]") {
      REPORTER.set_report(REPORT);
      FpSemigroup S;
      S.set_alphabet(3);
      S.set_identity(0);
      S.add_rule({1, 2}, {0});
      REQUIRE(!S.is_obviously_infinite());

      Congruence cong(TWOSIDED, S);
      cong.add_pair({1, 1, 1}, {0});
      REQUIRE(cong.nr_classes() == 3);
      // The following currently throws since we cannot set the _parent
      // semigroup when it is an FpSemigroup.
      REQUIRE_THROWS_AS(cong.nr_non_trivial_classes() == 3,
                        LibsemigroupsException);
    }

    LIBSEMIGROUPS_TEST_CASE("Congruence 015",
                            "2-sided congruence on free abelian monoid",
                            "[quick]") {
      REPORTER.set_report(REPORT);
      FpSemigroup S;
      S.set_alphabet(3);
      S.add_rule({1, 2}, {2, 1});
      S.set_identity(0);

      Congruence cong(TWOSIDED, S);
      cong.add_pair({1, 1, 1, 1, 1}, {1});
      cong.add_pair({2, 2, 2}, {2});

      REQUIRE(cong.nr_classes() == 15);
    }

    // The previous Congruence 17 test was identical to Congruence 12

    LIBSEMIGROUPS_TEST_CASE("Congruence 016",
                            "example where TC works but KB doesn't",
                            "[quick]") {
      REPORTER.set_report(REPORT);
      FpSemigroup S;
      S.set_alphabet("abBe");
      S.set_identity("e");
      S.add_rule("aa", "e");
      // FIXME S.add_rule("aa", ""); causes seg fault, should be allowed or give
      // sensible exception
      S.add_rule("BB", "b");
      S.add_rule("BaBaBaB", "abababa");
      S.add_rule("aBabaBabaBabaBab", "BabaBabaBabaBaba");

      Congruence cong(TWOSIDED, S);
      cong.add_pair({0}, {1});

      REQUIRE(cong.nr_classes() == 4);
    }

    LIBSEMIGROUPS_TEST_CASE("Congruence 017",
                            "2-sided congruence on finite semigroup",
                            "[quick]") {
      REPORTER.set_report(REPORT);
      using Transf = Transf<5>::type;
      Semigroup<Transf> S({Transf({1, 3, 4, 2, 3}), Transf({3, 2, 1, 3, 3})});

      REQUIRE(S.size() == 88);
      REQUIRE(S.nrrules() == 18);

      word_type w1 = S.factorisation(Transf({3, 4, 4, 4, 4}));
      word_type w2 = S.factorisation(Transf({3, 4, 4, 4, 4}));

      Congruence cong(TWOSIDED, S);
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
    LIBSEMIGROUPS_TEST_CASE("Congruence 018",
                            "infinite fp semigroup from GAP library ",
                            "[quick]") {
      REPORTER.set_report(REPORT);
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

      REQUIRE(!S.is_obviously_infinite());
      // TODO it is obviously infinite, must update the method

      Congruence cong(TWOSIDED, S);
      cong.add_pair({0}, {1});
      REQUIRE(!cong.finished());
      REQUIRE(cong.nr_non_trivial_classes() == 1);
      REQUIRE(cong.cbegin_ntc()->size() == 5);
      REQUIRE(cong.finished());
      // REQUIRE(cong.nr_classes());  // Currently runs forever (and should)
    }

    LIBSEMIGROUPS_TEST_CASE(
        "Congruence 019",
        "2-sided cong. on fp semigroup with infinite classes",
        "[quick]") {
      REPORTER.set_report(REPORT);
      FpSemigroup S;
      S.set_alphabet(2);
      S.add_rule({0, 1}, {1, 0});
      S.add_rule({0, 0, 0}, {0, 0});

      Congruence cong(TWOSIDED, S);
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

    LIBSEMIGROUPS_TEST_CASE("Congruence 020",
                            "trivial cong. on an fp semigroup",
                            "[quick]") {
      FpSemigroup S;
      S.set_alphabet("ab");
      S.add_rule("ab", "ba");
      S.add_rule("a", "b");

      Congruence cong(LEFT, S);
      // TODO is the next line correct?
      REQUIRE(cong.nr_non_trivial_classes() == 0);
    }

    LIBSEMIGROUPS_TEST_CASE("Congruence 021",
                            "duplicate generators",
                            "[quick]") {
      REPORTER.set_report(REPORT);
      using Transf = Transf<8>::type;
      Semigroup<Transf> S({Transf({7, 3, 5, 3, 4, 2, 7, 7}),
                           Transf({7, 3, 5, 3, 4, 2, 7, 7}),
                           Transf({7, 3, 5, 3, 4, 2, 7, 7}),
                           Transf({3, 6, 3, 4, 0, 6, 0, 7})});
      Congruence        cong(TWOSIDED, S);
      REQUIRE(cong.nr_classes() == S.size());
    }

    LIBSEMIGROUPS_TEST_CASE("Congruence 022",
                            "non-trivial classes",
                            "[quick]") {
      REPORTER.set_report(false);

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

      // FIXME this test fails if we don't run the next line, since the
      // congruence below has no parent
      REQUIRE(S.size() == 78);

      Congruence cong(TWOSIDED, S);
      cong.add_pair({0}, {1});

      REQUIRE(cong.nr_non_trivial_classes() == 1);
      REQUIRE(cong.cbegin_ntc()->size() == 78);
    }

    // FIXME in this example non-winner ToddCoxeter takes a long time to stop
    // after it is killed
    LIBSEMIGROUPS_TEST_CASE("Congruence 023",
                            "right congruence on finite semigroup",
                            "[quick]") {
      REPORTER.set_report(REPORT);
      using Transf = Transf<8>::type;
      Semigroup<Transf> S({Transf({0, 1, 2, 3, 4, 5, 6, 7}),
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
      REQUIRE(std::all_of(
          elms.cbegin(), elms.cend(), [&S](Transf const& x) -> bool {
            return S.test_membership(x);
          }));

      Congruence cong(RIGHT, S);
      word_type  w1, w2;
      for (size_t i = 0; i < elms.size(); i += 2) {
        S.factorisation(w1, S.position(elms[i]));
        S.factorisation(w2, S.position(elms[i + 1]));
        cong.add_pair(w1, w2);
      }
      REQUIRE(cong.nr_classes() == 1);
    }

    LIBSEMIGROUPS_TEST_CASE("Congruence 024",
                            "redundant generating pairs",
                            "[quick]") {
      REPORTER.set_report(REPORT);
      FpSemigroup S;
      S.set_alphabet(1);

      Congruence cong(TWOSIDED, S);
      cong.add_pair({0, 0}, {0, 0});
      REQUIRE(cong.contains({0, 0}, {0, 0}));
    }

    LIBSEMIGROUPS_TEST_CASE("Congruence 025",
                            "2-sided cong. on free semigroup",
                            "[quick]") {
      REPORTER.set_report(REPORT);
      FpSemigroup S;
      S.set_alphabet("a");
      Congruence cong(TWOSIDED, S);
      REQUIRE(cong.contains({0, 0}, {0, 0}));
      REQUIRE(!cong.contains({0, 0}, {0}));
    }

    LIBSEMIGROUPS_TEST_CASE("Congruence 026",
                            "is_quotient_obviously_(in)finite",
                            "[quick]") {
      REPORTER.set_report(REPORT);
      {
        FpSemigroup S;
        S.set_alphabet(3);
        S.add_rule({0, 1}, {0});
        Congruence cong(TWOSIDED, S);
        cong.add_pair({2, 2}, {2});
        REQUIRE(cong.is_quotient_obviously_infinite());
        REQUIRE(!cong.is_quotient_obviously_finite());
      }
      {
        FpSemigroup S;
        S.set_alphabet(3);
        S.add_rule({0, 1}, {0});
        S.add_rule({0, 0}, {0});
        Congruence cong(TWOSIDED, S);
        cong.add_pair({1, 1}, {1});
        REQUIRE(cong.is_quotient_obviously_infinite());
        REQUIRE(!cong.is_quotient_obviously_finite());
      }
      {
        FpSemigroup S;
        S.set_alphabet(3);
        S.add_rule({0, 1}, {0});
        S.add_rule({0, 0}, {0});
        Congruence cong(TWOSIDED, S);
        cong.add_pair({1, 2}, {1});
        REQUIRE(!cong.is_quotient_obviously_infinite());
        REQUIRE(!cong.is_quotient_obviously_finite());
      }
      {
        FpSemigroup S;
        S.set_alphabet(3);
        S.add_rule({0, 1}, {0});
        Congruence cong(RIGHT, S);
        cong.add_pair({2, 2}, {2});
        REQUIRE(cong.is_quotient_obviously_infinite());
        REQUIRE(!cong.is_quotient_obviously_finite());
      }
      {
        FpSemigroup S;
        S.set_alphabet(3);
        S.add_rule({0, 1}, {0});
        S.add_rule({0, 0}, {0});
        Congruence cong(RIGHT, S);
        cong.add_pair({1, 1}, {1});
        REQUIRE(cong.is_quotient_obviously_infinite());
        REQUIRE(!cong.is_quotient_obviously_finite());
      }
      {
        FpSemigroup S;
        S.set_alphabet(3);
        S.add_rule({0, 1}, {0});
        S.add_rule({0, 0}, {0});
        Congruence cong(RIGHT, S);
        cong.add_pair({1, 2}, {1});
        REQUIRE(!cong.is_quotient_obviously_infinite());
        REQUIRE(!cong.is_quotient_obviously_finite());
      }
      {
        FpSemigroup S;
        S.set_alphabet(3);
        S.add_rule({0, 1}, {0});
        Congruence cong(LEFT, S);
        cong.add_pair({2, 2}, {2});
        REQUIRE(cong.is_quotient_obviously_infinite());
        REQUIRE(!cong.is_quotient_obviously_finite());
      }
      {
        FpSemigroup S;
        S.set_alphabet(3);
        S.add_rule({0, 1}, {0});
        S.add_rule({0, 0}, {0});
        Congruence cong(LEFT, S);
        cong.add_pair({1, 1}, {1});
        REQUIRE(cong.is_quotient_obviously_infinite());
        REQUIRE(!cong.is_quotient_obviously_finite());
      }
      {
        FpSemigroup S;
        S.set_alphabet(3);
        S.add_rule({0, 1}, {0});
        S.add_rule({0, 0}, {0});
        Congruence cong(LEFT, S);
        cong.add_pair({1, 2}, {1});
        REQUIRE(!cong.is_quotient_obviously_infinite());
        REQUIRE(!cong.is_quotient_obviously_finite());
      }

      using Transf = Transf<3>::type;
      Semigroup<Transf> S({Transf({0, 1, 0}), Transf({0, 1, 2})});
      REQUIRE(S.size() == 2);
      {
        Congruence cong(TWOSIDED, S);
        cong.add_pair({1}, {0});
        REQUIRE(!cong.is_quotient_obviously_infinite());
        REQUIRE(cong.is_quotient_obviously_finite());
        REQUIRE(cong.nr_classes() == 1);
      }
    }

    LIBSEMIGROUPS_TEST_CASE("Congruence 027", "less", "[quick]") {
      FpSemigroup S;
      S.set_alphabet(2);
      S.add_rule({0, 0}, {0});

      Congruence cong(TWOSIDED, S);
      REQUIRE(!cong.less({0, 0}, {0}));
    }

    LIBSEMIGROUPS_TEST_CASE("Congruence 028",
                            "2-sided congruences of BMat8 semigroup",
                            "[quick]") {
      REPORTER.set_report(REPORT);
      using BMat = BMat<4>::type;
      std::vector<BMat> gens
          = {BMat({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
             BMat({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
             BMat({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {1, 0, 0, 1}}),
             BMat({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}})};
      {
        Semigroup<BMat> S(gens);

        Congruence cong(TWOSIDED, S);
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
        Semigroup<BMat8> S({gens[0], gens[2], gens[3]});
        Congruence       cong(TWOSIDED, S);
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

    LIBSEMIGROUPS_TEST_CASE("Congruence 029",
                            "left congruence on finite semigroup",
                            "[quick]") {
      REPORTER.set_report(REPORT);
      std::vector<Element*> gens
          = {new Transformation<u_int16_t>({1, 3, 4, 2, 3}),
             new Transformation<u_int16_t>({3, 2, 1, 3, 3})};
      Semigroup<> S(gens);
      delete_gens(gens);

      // REQUIRE(S.size() == 88);
      // REQUIRE(S.degree() == 5);
      Congruence cong(LEFT, S);
      cong.add_pair({0, 1, 0, 0, 0, 1, 1, 0, 0}, {1, 0, 0, 0, 1});

      REQUIRE(cong.nr_classes() == 69);
      REQUIRE(cong.nr_classes() == 69);

      Element*  t3 = new Transformation<u_int16_t>({1, 3, 1, 3, 3});
      Element*  t4 = new Transformation<u_int16_t>({4, 2, 4, 4, 2});
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

    LIBSEMIGROUPS_TEST_CASE("Congruence 030",
                            "right congruence on finite semigroup",
                            "[quick]") {
      REPORTER.set_report(REPORT);
      std::vector<Element*> gens
          = {new Transformation<u_int16_t>({1, 3, 4, 2, 3}),
             new Transformation<u_int16_t>({3, 2, 1, 3, 3})};
      Semigroup<> S(gens);
      delete_gens(gens);

      // REQUIRE(S.size() == 88);
      // REQUIRE(S.degree() == 5);
      Congruence cong(RIGHT, S);
      cong.add_pair({0, 1, 0, 0, 0, 1, 1, 0, 0}, {1, 0, 0, 0, 1});

      REQUIRE(cong.nr_classes() == 72);
      REQUIRE(cong.nr_classes() == 72);

      Element*  t3 = new Transformation<u_int16_t>({1, 3, 1, 3, 3});
      Element*  t4 = new Transformation<u_int16_t>({4, 2, 4, 4, 2});
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

      REQUIRE(!cong.less({1, 0, 0, 0, 1, 0, 0, 0}, {1, 0, 0, 1}));
      REQUIRE(cong.less({1, 0, 0, 1}, {1, 0, 0, 0, 1, 0, 0, 0}));

      delete t3;
      delete t4;
    }

    // For some reason the following test case doesn't run...
    LIBSEMIGROUPS_TEST_CASE("Congruence 031",
                            "right congruence on finite semigroup",
                            "[quick]") {
      REPORTER.set_report(REPORT);
      std::vector<Element*> gens
          = {new Transformation<u_int16_t>({1, 3, 4, 2, 3}),
             new Transformation<u_int16_t>({3, 2, 1, 3, 3})};
      Semigroup<> S = Semigroup<>(gens);
      delete_gens(gens);

      REQUIRE(S.size() == 88);
      REQUIRE(S.nrrules() == 18);
      REQUIRE(S.degree() == 5);
      Element*  t1 = new Transformation<u_int16_t>({3, 4, 4, 4, 4});
      Element*  t2 = new Transformation<u_int16_t>({3, 1, 3, 3, 3});
      word_type w1, w2;
      S.factorisation(w1, S.position(t1));
      S.factorisation(w2, S.position(t2));
      Congruence cong(RIGHT, S);
      cong.add_pair(w1, w2);

      REQUIRE(cong.nr_classes() == 72);
      REQUIRE(cong.nr_classes() == 72);
      Element*  t3 = new Transformation<u_int16_t>({1, 3, 3, 3, 3});
      Element*  t4 = new Transformation<u_int16_t>({4, 2, 4, 4, 2});
      Element*  t5 = new Transformation<u_int16_t>({2, 3, 2, 2, 2});
      Element*  t6 = new Transformation<u_int16_t>({2, 3, 3, 3, 3});
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

    LIBSEMIGROUPS_TEST_CASE("Congruence 032", "contains", "[quick]") {
      REPORTER.set_report(REPORT);
      FpSemigroup S;
      S.set_alphabet(2);
      Congruence cong(TWOSIDED, S);
      cong.add_pair({0, 0}, {0});
      cong.add_pair({0, 1}, {0});
      cong.add_pair({1, 0}, {0});
      REQUIRE(cong.contains({0, 0}, {0}));
      REQUIRE(cong.contains({0, 1}, {0}));
      REQUIRE(cong.contains({1, 0}, {0}));
    }

    ////////////////////////////////////////////////////////////////////////
    // Functions for subsequent tests by Florent Hivert and Joel Gay.
    ////////////////////////////////////////////////////////////////////////

    std::vector<relation_type> RennerCommonTypeBMonoid(size_t l, int q) {
      // q is supposed to be 0 or 1
      std::vector<size_t> s;
      std::vector<size_t> e;
      for (size_t i = 0; i < l; ++i) {
        s.push_back(i);
      }
      for (size_t i = l; i < 2 * l + 1; ++i) {
        e.push_back(i);
      }
      size_t id = 2 * l + 1;

      std::vector<relation_type> rels = {relation_type({id, id}, {id})};
      // identity relations
      for (size_t i = 0; i < l; ++i) {
        rels.push_back({{s[i], id}, {s[i]}});
        rels.push_back({{id, s[i]}, {s[i]}});
        rels.push_back({{id, e[i]}, {e[i]}});
        rels.push_back({{e[i], id}, {e[i]}});
      }
      rels.push_back({{id, e[l]}, {e[l]}});
      rels.push_back({{e[l], id}, {e[l]}});

      switch (q) {
        case 0:
          for (size_t i = 0; i < l; ++i)
            rels.push_back({{s[i], s[i]}, {s[i]}});
          break;
        case 1:
          for (size_t i = 0; i < l; ++i)
            rels.push_back({{s[i], s[i]}, {id}});
          break;
          // default: assert(FALSE)
      }
      for (int i = 0; i < static_cast<int>(l); ++i) {
        for (int j = 0; j < static_cast<int>(l); ++j) {
          if (std::abs(i - j) >= 2) {
            rels.push_back({{s[i], s[j]}, {s[j], s[i]}});
          }
        }
      }

      for (size_t i = 1; i < l - 1; ++i) {
        rels.push_back({{s[i], s[i + 1], s[i]}, {s[i + 1], s[i], s[i + 1]}});
      }

      rels.push_back({{s[1], s[0], s[1], s[0]}, {s[0], s[1], s[0], s[1]}});

      for (size_t i = 1; i < l; ++i) {
        for (size_t j = 0; j < i; ++j) {
          rels.push_back({{s[i], e[j]}, {e[j], s[i]}});
        }
      }

      for (size_t i = 0; i < l; ++i) {
        for (size_t j = i + 1; j < l + 1; ++j) {
          rels.push_back({{s[i], e[j]}, {e[j], s[i]}});
          rels.push_back({{s[i], e[j]}, {e[j]}});
        }
      }

      for (size_t i = 0; i < l + 1; ++i) {
        for (size_t j = 0; j < l + 1; ++j) {
          rels.push_back({{e[i], e[j]}, {e[j], e[i]}});
          rels.push_back({{e[i], e[j]}, {e[std::max(i, j)]}});
        }
      }

      for (size_t i = 0; i < l; ++i) {
        rels.push_back({{e[i], s[i], e[i]}, {e[i + 1]}});
      }

      return rels;
    }

    std::vector<relation_type> EGTypeBMonoid(size_t l, int q) {
      std::vector<size_t> s;
      std::vector<size_t> e;
      for (size_t i = 0; i < l; ++i) {
        s.push_back(i);
      }
      for (size_t i = l; i < 2 * l + 1; ++i) {
        e.push_back(i);
      }
      size_t id = 2 * l + 1;

      std::vector<relation_type> rels = RennerCommonTypeBMonoid(l, q);

      if (l >= 2)
        rels.push_back({{e[0], s[0], s[1], s[0], e[0]}, {e[2]}});

      return rels;
    }

    std::vector<size_t> max_elt_B(size_t i) {
      std::vector<size_t> t(0);
      for (int end = i; end >= 0; end--) {
        for (int k = 0; k <= end; k++) {
          t.push_back(k);
        }
      }
      return t;
    }

    std::vector<relation_type> RennerTypeBMonoid(size_t l, int q) {
      std::vector<size_t> s;
      std::vector<size_t> e;
      for (size_t i = 0; i < l; ++i) {
        s.push_back(i);
      }
      for (size_t i = l; i < 2 * l + 1; ++i) {
        e.push_back(i);
      }
      size_t id = 2 * l + 1;

      std::vector<relation_type> rels = RennerCommonTypeBMonoid(l, q);

      for (size_t i = 1; i < l; i++) {
        std::vector<size_t> new_rel = max_elt_B(i);
        new_rel.push_back(e[0]);
        new_rel.insert(new_rel.begin(), e[0]);
        rels.push_back({new_rel, {e[i + 1]}});
      }

      return rels;
    }

    std::vector<relation_type> RennerCommonTypeDMonoid(size_t l, int q) {
      // q is supposed to be 0 or 1
      std::vector<size_t> s;
      std::vector<size_t> e;
      for (size_t i = 0; i < l; ++i) {
        s.push_back(i);  // 0 est \pi_1^f et 1 est \pi_1^e
      }
      for (size_t i = l; i < 2 * l + 1; ++i) {
        e.push_back(i);
      }
      size_t f  = 2 * l + 1;
      size_t id = 2 * l + 2;

      std::vector<relation_type> rels = {relation_type({id, id}, {id})};
      // identity relations
      for (size_t i = 0; i < l; ++i) {
        rels.push_back({{s[i], id}, {s[i]}});
        rels.push_back({{id, s[i]}, {s[i]}});
        rels.push_back({{id, e[i]}, {e[i]}});
        rels.push_back({{e[i], id}, {e[i]}});
      }
      rels.push_back({{id, e[l]}, {e[l]}});
      rels.push_back({{e[l], id}, {e[l]}});
      rels.push_back({{id, f}, {f}});
      rels.push_back({{f, id}, {f}});

      switch (q) {
        case 0:
          for (size_t i = 0; i < l; ++i)
            rels.push_back({{s[i], s[i]}, {s[i]}});
          break;
        case 1:
          for (size_t i = 0; i < l; ++i)
            rels.push_back({{s[i], s[i]}, {id}});
          break;
          // default: assert(FALSE)
      }
      for (int i = 1; i < static_cast<int>(l); ++i) {  // tout sauf \pi_1^f
        for (int j = 1; j < static_cast<int>(l); ++j) {
          if (std::abs(i - j) >= 2) {
            rels.push_back({{s[i], s[j]}, {s[j], s[i]}});
          }
        }
      }
      for (int i = 3; i < static_cast<int>(l);
           ++i) {  // \pi_1^f avec les \pi_i pour i>2
        rels.push_back({{s[0], s[i]}, {s[i], s[0]}});
      }
      rels.push_back({{s[0], s[1]}, {s[1], s[0]}});  // \pi_1^f avec \pi_1^e

      for (size_t i = 1; i < l - 1; ++i) {  // tresses
        rels.push_back({{s[i], s[i + 1], s[i]}, {s[i + 1], s[i], s[i + 1]}});
      }
      if (l > 2) {
        rels.push_back(
            {{s[0], s[2], s[0]}, {s[2], s[0], s[2]}});  // tresse de \pi_1^f
      }

      for (size_t i = 2; i < l; ++i) {  // commutation, attention début à 2
        for (size_t j = 0; j < i; ++j) {
          rels.push_back({{s[i], e[j]}, {e[j], s[i]}});
        }
        rels.push_back({{s[i], f}, {f, s[i]}});
      }
      rels.push_back({{s[0], f}, {f, s[0]}});        // commutation \pi_1^f et f
      rels.push_back({{s[1], e[0]}, {e[0], s[1]}});  // commutation \pi_1^e et e

      for (size_t i = 1; i < l; ++i) {  // absorption
        for (size_t j = i + 1; j < l + 1; ++j) {
          rels.push_back({{s[i], e[j]}, {e[j], s[i]}});
          rels.push_back({{s[i], e[j]}, {e[j]}});
          if (i == 1) {  // cas particulier \pi_1^f
            rels.push_back({{s[0], e[j]}, {e[j], s[0]}});
            rels.push_back({{s[0], e[j]}, {e[j]}});
          }
        }
      }

      for (size_t i = 0; i < l + 1; ++i) {  // produit e_i
        for (size_t j = 0; j < l + 1; ++j) {
          rels.push_back({{e[i], e[j]}, {e[j], e[i]}});
          rels.push_back({{e[i], e[j]}, {e[std::max(i, j)]}});
        }
        if (i > 1) {
          rels.push_back({{f, e[i]}, {e[i], f}});
          rels.push_back({{f, e[i]}, {e[i]}});
        }
      }
      rels.push_back({{f, f}, {f}});
      rels.push_back({{f, e[0]}, {e[1]}});
      rels.push_back({{e[0], f}, {e[1]}});

      for (size_t i = 2; i < l; ++i) {
        rels.push_back({{e[i], s[i], e[i]}, {e[i + 1]}});
      }
      rels.push_back({{e[0], s[0], e[0]}, {e[2]}});
      rels.push_back({{f, s[1], f}, {e[2]}});

      return rels;
    }

    std::vector<relation_type> EGTypeDMonoid(size_t l, int q) {
      std::vector<size_t> s;
      std::vector<size_t> e;
      for (size_t i = 0; i < l; ++i) {
        s.push_back(i);
      }
      for (size_t i = l; i < 2 * l + 1; ++i) {
        e.push_back(i);
      }
      size_t f  = 2 * l + 1;
      size_t id = 2 * l + 2;

      std::vector<relation_type> rels = RennerCommonTypeDMonoid(l, q);

      if (l >= 3) {
        rels.push_back({{e[0], s[0], s[2], s[1], f}, {e[3]}});
        rels.push_back({{f, s[1], s[2], s[0], e[0]}, {e[3]}});
      }
      return rels;
    }

    std::vector<size_t> max_elt_D(size_t i, int g) {
      // g est 0 ou 1 : 0 pour f et 1 pour e
      std::vector<size_t> t(0);
      int                 parity = g;
      for (int end = i; end > 0; end--) {
        t.push_back(parity);
        for (int k = 2; k <= end; k++) {
          t.push_back(k);
        }
        parity = (parity + 1) % 2;
      }
      return t;
    }

    std::vector<relation_type> RennerTypeDMonoid(size_t l, int q) {
      std::vector<size_t> s;
      std::vector<size_t> e;
      for (size_t i = 0; i < l; ++i) {
        s.push_back(i);
      }
      for (size_t i = l; i < 2 * l + 1; ++i) {
        e.push_back(i);
      }
      size_t f  = 2 * l + 1;
      size_t id = 2 * l + 2;

      std::vector<relation_type> rels = RennerCommonTypeDMonoid(l, q);

      for (size_t i = 2; i < l; i++) {
        std::vector<size_t> new_rel_f = max_elt_D(i, 0);
        std::vector<size_t> new_rel_e = max_elt_D(i, 1);

        if (i % 2 == 0) {
          new_rel_e.insert(new_rel_e.begin(), f);
          new_rel_e.push_back(e[0]);
          rels.push_back({new_rel_e, {e[i + 1]}});

          new_rel_f.insert(new_rel_f.begin(), e[0]);
          new_rel_f.push_back(f);
          rels.push_back({new_rel_f, {e[i + 1]}});
        } else {
          new_rel_e.insert(new_rel_e.begin(), f);
          new_rel_e.push_back(f);
          rels.push_back({new_rel_e, {e[i + 1]}});

          new_rel_f.insert(new_rel_f.begin(), e[0]);
          new_rel_f.push_back(e[0]);
          rels.push_back({new_rel_f, {e[i + 1]}});
        }
      }
      return rels;
    }

    LIBSEMIGROUPS_TEST_CASE("FpSemigroup 001",
                            "Renner monoid type B2 (E. G. presentation), q = 1",
                            "[quick][hivert]") {
      REPORTER.set_report(REPORT);
      FpSemigroup S;
      S.set_alphabet(6);

      for (relation_type const& rl : EGTypeBMonoid(2, 1)) {
        S.add_rule(rl);
      }
      REQUIRE(!S.is_obviously_infinite());
      REQUIRE(S.size() == 57);
    }

    LIBSEMIGROUPS_TEST_CASE("FpSemigroup 002",
                            "Renner monoid type B2 (E. G. presentation), q = 0",
                            "[quick][hivert]") {
      REPORTER.set_report(REPORT);
      FpSemigroup S;
      S.set_alphabet(6);
      for (relation_type const& rl : EGTypeBMonoid(2, 0)) {
        S.add_rule(rl);
      }
      REQUIRE(!S.is_obviously_infinite());
      REQUIRE(S.size() == 57);
    }

    // Loops for ever: Infinite monoid ???
    LIBSEMIGROUPS_TEST_CASE("FpSemigroup 003",
                            "Renner monoid type B3 (E. G. presentation), q = 1",
                            "[quick][hivert]") {
      REPORTER.set_report(REPORT);
      FpSemigroup S;
      S.set_alphabet(8);
      for (relation_type const& rl : EGTypeBMonoid(3, 1)) {
        S.add_rule(rl);
      }
      REQUIRE(!S.is_obviously_infinite());
      S.isomorphic_non_fp_semigroup()->enumerate(8000);
      REQUIRE(S.isomorphic_non_fp_semigroup()->current_size() == 8200);
      // REQUIRE(S.size() == 757);
    }

    // Loops for ever: Infinite monoid ???
    LIBSEMIGROUPS_TEST_CASE("FpSemigroup 004",
                            "Renner monoid type B3 (E. G. presentation), q = 0",
                            "[quick][hivert]") {
      REPORTER.set_report(REPORT);
      FpSemigroup S;
      S.set_alphabet(8);
      for (relation_type const& rl : EGTypeBMonoid(3, 0)) {
        S.add_rule(rl);
      }
      REQUIRE(!S.is_obviously_infinite());
      S.isomorphic_non_fp_semigroup()->enumerate(8000);
      REQUIRE(S.isomorphic_non_fp_semigroup()->current_size() == 8200);
      // REQUIRE(S.size() == 757);
    }

    LIBSEMIGROUPS_TEST_CASE(
        "FpSemigroup 005",
        "Renner monoid type B2 (Gay-Hivert presentation), q = 1",
        "[quick][hivert]") {
      REPORTER.set_report(REPORT);
      FpSemigroup S;
      S.set_alphabet(6);
      for (relation_type const& rl : RennerTypeBMonoid(2, 1)) {
        S.add_rule(rl);
      }
      REQUIRE(!S.is_obviously_infinite());
      REQUIRE(S.size() == 57);
    }

    LIBSEMIGROUPS_TEST_CASE(
        "FpSemigroup 006",
        "Renner monoid type B2 (Gay-Hivert presentation), q = 0",
        "[quick][hivert]") {
      REPORTER.set_report(REPORT);
      FpSemigroup S;
      S.set_alphabet(6);
      for (relation_type const& rl : RennerTypeBMonoid(2, 0)) {
        S.add_rule(rl);
      }
      REQUIRE(!S.is_obviously_infinite());
      REQUIRE(S.size() == 57);
    }

    LIBSEMIGROUPS_TEST_CASE(
        "FpSemigroup 007",
        "Renner monoid type B3 (Gay-Hivert presentation), q = 1",
        "[quick][hivert]") {
      REPORTER.set_report(REPORT);
      FpSemigroup S;
      S.set_alphabet(8);
      for (relation_type const& rl : RennerTypeBMonoid(3, 1)) {
        S.add_rule(rl);
      }
      REQUIRE(!S.is_obviously_infinite());
      REQUIRE(S.size() == 757);
    }

    LIBSEMIGROUPS_TEST_CASE(
        "FpSemigroup 008",
        "Renner monoid type B3 (Gay-Hivert presentation), q = 0",
        "[quick][hivert]") {
      REPORTER.set_report(REPORT);
      FpSemigroup S;
      S.set_alphabet(8);
      for (relation_type const& rl : RennerTypeBMonoid(3, 0)) {
        S.add_rule(rl);
      }
      REQUIRE(!S.is_obviously_infinite());
      REQUIRE(S.size() == 757);
    }

    LIBSEMIGROUPS_TEST_CASE(
        "FpSemigroup 009",
        "Renner monoid type B4 (Gay-Hivert presentation), q = 1",
        "[quick][hivert]") {
      REPORTER.set_report(REPORT);
      FpSemigroup S;
      S.set_alphabet(10);
      for (relation_type const& rl : RennerTypeBMonoid(4, 1)) {
        S.add_rule(rl);
      }
      REQUIRE(S.nr_rules() == 115);
      REQUIRE(!S.is_obviously_infinite());
      REQUIRE(!S.knuth_bendix()->confluent());
      // S.knuth_bendix()->run(); // Works too but is slower :)
      REQUIRE(S.size() == 13889);
      REQUIRE(S.isomorphic_non_fp_semigroup()->nrrules() == 356);
    }

    LIBSEMIGROUPS_TEST_CASE(
        "FpSemigroup 010",
        "Renner monoid type B4 (Gay-Hivert presentation), q = 0",
        "[quick][hivert]") {
      REPORTER.set_report(REPORT);
      FpSemigroup S;
      S.set_alphabet(10);
      for (relation_type const& rl : RennerTypeBMonoid(4, 0)) {
        S.add_rule(rl);
      }
      REQUIRE(S.nr_rules() == 115);
      REQUIRE(!S.is_obviously_infinite());
      REQUIRE(!S.knuth_bendix()->confluent());
      // S.knuth_bendix()->run(); // Works too :)
      REQUIRE(S.size() == 13889);
      REQUIRE(S.isomorphic_non_fp_semigroup()->nrrules() == 356);
    }

    // This appears to be an example where KB + FP is faster than TC
    LIBSEMIGROUPS_TEST_CASE(
        "FpSemigroup 011",
        "Renner monoid type B5 (Gay-Hivert presentation), q = 1",
        "[extreme][hivert]") {
      REPORTER.set_report(true);
      FpSemigroup S;
      S.set_alphabet(12);
      for (relation_type const& rl : RennerTypeBMonoid(5, 1)) {
        S.add_rule(rl);
      }
      REQUIRE(S.nr_rules() == 165);
      REQUIRE(!S.is_obviously_infinite());
      REQUIRE(!S.knuth_bendix()->confluent());
      // S.todd_coxeter()->run(); // Takes 2m30s or so to run
      REQUIRE(S.size() == 322021);
      REQUIRE(S.isomorphic_non_fp_semigroup()->nrrules() == 1453);

      congruence::ToddCoxeter tc(
          TWOSIDED,
          S.isomorphic_non_fp_semigroup(),
          congruence::ToddCoxeter::policy::use_cayley_graph);
      REQUIRE(tc.nr_classes() == 322021);  // Works!

      // fpsemigroup::ToddCoxeter tc(S.isomorphic_non_fp_semigroup());
      // REQUIRE(tc.nr_rules() == 1453); FIXME this gives an error, something
      // is wrong, every relation seems to be added twice
      // REQUIRE(tc.size() == 322021); // Runs forever
    }

    LIBSEMIGROUPS_TEST_CASE(
        "FpSemigroup 012",
        "Renner monoid type B5 (Gay-Hivert presentation), q = 0",
        "[extreme][hivert]") {
      REPORTER.set_report(true);
      FpSemigroup S;
      S.set_alphabet(12);
      for (relation_type const& rl : RennerTypeBMonoid(5, 0)) {
        S.add_rule(rl);
      }
      REQUIRE(S.nr_rules() == 165);
      REQUIRE(!S.is_obviously_infinite());
      REQUIRE(!S.knuth_bendix()->confluent());
      // S.todd_coxeter()->run(); // TODO Check if it runs
      REQUIRE(S.size() == 322021);
      REQUIRE(S.isomorphic_non_fp_semigroup()->nrrules() == 1453);

      congruence::ToddCoxeter tc(
          TWOSIDED,
          S.isomorphic_non_fp_semigroup(),
          congruence::ToddCoxeter::policy::use_cayley_graph);
      REQUIRE(tc.nr_classes() == 322021);  // Works!
    }

    LIBSEMIGROUPS_TEST_CASE("FpSemigroup 013",
                            "Renner monoid type D2 (E. G. presentation), q = 1",
                            "[quick][hivert]") {
      REPORTER.set_report(REPORT);
      FpSemigroup S;
      S.set_alphabet(7);
      for (relation_type const& rl : EGTypeDMonoid(2, 1)) {
        S.add_rule(rl);
      }
      REQUIRE(S.nr_rules() == 47);
      REQUIRE(!S.is_obviously_infinite());
      REQUIRE(!S.knuth_bendix()->confluent());
      // S.knuth_bendix()->run(); // Works too :)
      REQUIRE(S.size() == 37);
      REQUIRE(S.isomorphic_non_fp_semigroup()->nrrules() == 54);
    }

    LIBSEMIGROUPS_TEST_CASE("FpSemigroup 014",
                            "Renner monoid type D2 (E. G. presentation), q = 0",
                            "[quick][hivert]") {
      REPORTER.set_report(REPORT);
      FpSemigroup S;
      S.set_alphabet(7);
      for (relation_type const& rl : EGTypeDMonoid(2, 0)) {
        S.add_rule(rl);
      }
      REQUIRE(S.nr_rules() == 47);
      REQUIRE(!S.is_obviously_infinite());
      REQUIRE(!S.knuth_bendix()->confluent());
      // S.knuth_bendix()->run(); // Works too :)
      REQUIRE(S.size() == 37);
      REQUIRE(S.isomorphic_non_fp_semigroup()->nrrules() == 54);
    }

    LIBSEMIGROUPS_TEST_CASE("FpSemigroup 015",
                            "Renner monoid type D3 (E. G. presentation), q = 1",
                            "[quick][hivert]") {
      REPORTER.set_report(REPORT);
      FpSemigroup S;
      S.set_alphabet(9);
      for (relation_type const& rl : EGTypeDMonoid(3, 1)) {
        S.add_rule(rl);
      }
      REQUIRE(S.nr_rules() == 82);
      REQUIRE(!S.is_obviously_infinite());
      REQUIRE(!S.knuth_bendix()->confluent());
      // S.knuth_bendix()->run(); // Works too but is a bit slower :)
      REQUIRE(S.size() == 541);
      REQUIRE(S.isomorphic_non_fp_semigroup()->nrrules() == 148);
    }

    LIBSEMIGROUPS_TEST_CASE("FpSemigroup 016",
                            "Renner monoid type D3 (E. G. presentation), q = 0",
                            "[quick][hivert]") {
      REPORTER.set_report(REPORT);
      FpSemigroup S;
      S.set_alphabet(9);
      for (relation_type const& rl : EGTypeDMonoid(3, 0)) {
        S.add_rule(rl);
      }
      REQUIRE(S.nr_rules() == 82);
      REQUIRE(!S.is_obviously_infinite());
      REQUIRE(!S.knuth_bendix()->confluent());
      // S.knuth_bendix()->run(); // Works too but is a bit slower :)
      REQUIRE(S.size() == 541);
      REQUIRE(S.isomorphic_non_fp_semigroup()->nrrules() == 148);
    }

    LIBSEMIGROUPS_TEST_CASE("FpSemigroup 017",
                            "Renner monoid type D4 (E. G. presentation), q = 1",
                            "[quick][hivert]") {  // TODO check test category
      REPORTER.set_report(REPORT);
      FpSemigroup S;
      S.set_alphabet(11);
      for (relation_type const& rl : EGTypeDMonoid(4, 1)) {
        S.add_rule(rl);
      }
      REQUIRE(S.nr_rules() == 124);
      REQUIRE(!S.is_obviously_infinite());
      REQUIRE(!S.knuth_bendix()->confluent());

      S.isomorphic_non_fp_semigroup()->enumerate(10626);
      REQUIRE(S.isomorphic_non_fp_semigroup()->current_nrrules() == 417);
      REQUIRE(S.isomorphic_non_fp_semigroup()->current_size() == 10626);
      // REQUIRE(S.size() == 10625); // Runs forever
    }

    LIBSEMIGROUPS_TEST_CASE("FpSemigroup 018",
                            "Renner monoid type D4 (E. G. presentation), q = 0",
                            "[quick][hivert]") {  // TODO check test category
      REPORTER.set_report(REPORT);
      FpSemigroup S;
      S.set_alphabet(11);
      for (relation_type const& rl : EGTypeDMonoid(4, 0)) {
        S.add_rule(rl);
      }
      REQUIRE(S.nr_rules() == 124);
      REQUIRE(!S.is_obviously_infinite());
      REQUIRE(!S.knuth_bendix()->confluent());

      S.isomorphic_non_fp_semigroup()->enumerate(10626);
      REQUIRE(S.isomorphic_non_fp_semigroup()->current_nrrules() == 417);
      REQUIRE(S.isomorphic_non_fp_semigroup()->current_size() == 10626);
      // REQUIRE(S.size() == 10625); // Runs forever
    }

    LIBSEMIGROUPS_TEST_CASE(
        "FpSemigroup 019",
        "Renner monoid type D2 (Gay-Hivert presentation), q = 1",
        "[quick][hivert]") {
      REPORTER.set_report(REPORT);
      FpSemigroup S;
      S.set_alphabet(7);
      for (relation_type const& rl : RennerTypeDMonoid(2, 1)) {
        S.add_rule(rl);
      }
      REQUIRE(S.nr_rules() == 47);
      REQUIRE(!S.is_obviously_infinite());
      REQUIRE(!S.knuth_bendix()->confluent());
      // S.knuth_bendix()->run(); // Works too :)
      REQUIRE(S.size() == 37);
      REQUIRE(S.isomorphic_non_fp_semigroup()->nrrules() == 54);
    }

    LIBSEMIGROUPS_TEST_CASE(
        "FpSemigroup 020",
        "Renner monoid type D2 (Gay-Hivert presentation), q = 0",
        "[quick][hivert]") {
      REPORTER.set_report(REPORT);
      FpSemigroup S;
      S.set_alphabet(7);
      for (relation_type const& rl : RennerTypeDMonoid(2, 0)) {
        S.add_rule(rl);
      }
      REQUIRE(S.nr_rules() == 47);
      REQUIRE(!S.is_obviously_infinite());
      REQUIRE(!S.knuth_bendix()->confluent());
      // S.knuth_bendix()->run(); // Works too :)
      REQUIRE(S.size() == 37);
      REQUIRE(S.isomorphic_non_fp_semigroup()->nrrules() == 54);
    }

    LIBSEMIGROUPS_TEST_CASE(
        "FpSemigroup 021",
        "Renner monoid type D3 (Gay-Hivert presentation), q = 1",
        "[quick][hivert]") {
      REPORTER.set_report(REPORT);
      FpSemigroup S;
      S.set_alphabet(9);
      for (relation_type const& rl : RennerTypeDMonoid(3, 1)) {
        S.add_rule(rl);
      }
      REQUIRE(S.nr_rules() == 82);
      REQUIRE(!S.is_obviously_infinite());
      REQUIRE(!S.knuth_bendix()->confluent());
      // S.knuth_bendix()->run(); // Works too but is a bit slower :)
      REQUIRE(S.size() == 541);
      REQUIRE(S.isomorphic_non_fp_semigroup()->nrrules() == 148);
    }

    LIBSEMIGROUPS_TEST_CASE(
        "FpSemigroup 022",
        "Renner monoid type D3 (Gay-Hivert presentation), q = 0",
        "[quick][hivert]") {
      REPORTER.set_report(REPORT);
      FpSemigroup S;
      S.set_alphabet(9);
      for (relation_type const& rl : RennerTypeDMonoid(3, 0)) {
        S.add_rule(rl);
      }
      REQUIRE(S.nr_rules() == 82);
      REQUIRE(!S.is_obviously_infinite());
      REQUIRE(!S.knuth_bendix()->confluent());
      // S.knuth_bendix()->run(); // Works too but is a bit slower :)
      REQUIRE(S.size() == 541);
      REQUIRE(S.isomorphic_non_fp_semigroup()->nrrules() == 148);
    }

    LIBSEMIGROUPS_TEST_CASE(
        "FpSemigroup 023",
        "Renner monoid type D4 (Gay-Hivert presentation), q = 1",
        "[quick][hivert]") {
      REPORTER.set_report(REPORT);
      FpSemigroup S;
      S.set_alphabet(11);
      for (relation_type const& rl : RennerTypeDMonoid(4, 1)) {
        S.add_rule(rl);
      }
      REQUIRE(S.nr_rules() == 126);
      REQUIRE(!S.is_obviously_infinite());
      REQUIRE(!S.knuth_bendix()->confluent());

      REQUIRE(S.size() == 10625);
      REQUIRE(S.isomorphic_non_fp_semigroup()->nrrules() == 419);
    }

    LIBSEMIGROUPS_TEST_CASE(
        "FpSemigroup 024",
        "Renner monoid type D4 (Gay-Hivert presentation), q = 0",
        "[quick][hivert]") {
      REPORTER.set_report(REPORT);
      FpSemigroup S;
      S.set_alphabet(11);
      for (relation_type const& rl : RennerTypeDMonoid(4, 0)) {
        S.add_rule(rl);
      }
      REQUIRE(S.nr_rules() == 126);
      REQUIRE(!S.is_obviously_infinite());
      REQUIRE(!S.knuth_bendix()->confluent());
      REQUIRE(S.size() == 10625);
      REQUIRE(S.isomorphic_non_fp_semigroup()->nrrules() == 419);
    }

    LIBSEMIGROUPS_TEST_CASE(
        "FpSemigroup 025",
        "Renner monoid type D5 (Gay-Hivert presentation), q = 1",
        "[extreme][hivert]") {  // TODO check the category
      REPORTER.set_report(true);
      FpSemigroup S;
      S.set_alphabet(13);
      for (relation_type const& rl : RennerTypeDMonoid(5, 1)) {
        S.add_rule(rl);
      }
      REQUIRE(S.nr_rules() == 179);
      REQUIRE(!S.is_obviously_infinite());
      REQUIRE(!S.knuth_bendix()->confluent());

      REQUIRE(S.size() == 258661);
      REQUIRE(S.isomorphic_non_fp_semigroup()->nrrules() == 1279);
    }

    LIBSEMIGROUPS_TEST_CASE(
        "FpSemigroup 026",
        "Renner monoid type D5 (Gay-Hivert presentation), q = 0",
        "[extreme][hivert]") {  // TODO check the category
      REPORTER.set_report(true);
      FpSemigroup S;
      S.set_alphabet(13);
      for (relation_type const& rl : RennerTypeDMonoid(5, 0)) {
        S.add_rule(rl);
      }
      REQUIRE(S.nr_rules() == 179);
      REQUIRE(!S.is_obviously_infinite());
      REQUIRE(!S.knuth_bendix()->confluent());
      REQUIRE(S.size() == 258661);
      REQUIRE(S.isomorphic_non_fp_semigroup()->nrrules() == 1279);
    }

    // Takes about 4 minutes
    LIBSEMIGROUPS_TEST_CASE(
        "FpSemigroup 027",
        "Renner monoid type D6 (Gay-Hivert presentation), q = 1",
        "[extreme][hivert]") {
      REPORTER.set_report(true);
      FpSemigroup S;
      S.set_alphabet(15);
      for (relation_type const& rl : RennerTypeDMonoid(6, 1)) {
        S.add_rule(rl);
      }
      REQUIRE(S.nr_rules() == 241);
      REQUIRE(!S.is_obviously_infinite());
      REQUIRE(!S.knuth_bendix()->confluent());

      REQUIRE(S.size() == 7464625);
      REQUIRE(S.isomorphic_non_fp_semigroup()->nrrules() == 1279);
    }

    // Takes about 4 minutes
    LIBSEMIGROUPS_TEST_CASE(
        "FpSemigroup 028",
        "Renner monoid type D6 (Gay-Hivert presentation), q = 0",
        "[extreme][hivert]") {
      REPORTER.set_report(true);
      FpSemigroup S;
      S.set_alphabet(15);
      for (relation_type const& rl : RennerTypeDMonoid(6, 0)) {
        S.add_rule(rl);
      }
      REQUIRE(S.nr_rules() == 241);
      REQUIRE(!S.is_obviously_infinite());
      REQUIRE(!S.knuth_bendix()->confluent());
      S.knuth_bendix()->knuth_bendix_by_overlap_length();
      REQUIRE(S.size() == 7464625);
      REQUIRE(S.isomorphic_non_fp_semigroup()->nrrules() == 4570);
    }

    std::vector<relation_type> RookMonoid(size_t l, int q) {
      // q is supposed to be 0 or 1

      std::vector<size_t> s;
      for (size_t i = 0; i < l; ++i) {
        s.push_back(i);  // 0 est \pi_0
      }

      // identity relations
      size_t                     id   = l;
      std::vector<relation_type> rels = {relation_type({id, id}, {id})};
      for (size_t i = 0; i < l; ++i) {
        rels.push_back({{s[i], id}, {s[i]}});
        rels.push_back({{id, s[i]}, {s[i]}});
      }

      switch (q) {
        case 0:
          for (size_t i = 0; i < l; ++i)
            rels.push_back({{s[i], s[i]}, {s[i]}});
          break;
        case 1:
          rels.push_back({{s[0], s[0]}, {s[0]}});
          for (size_t i = 1; i < l; ++i)
            rels.push_back({{s[i], s[i]}, {id}});
          break;
          // default: assert(FALSE)
      }
      for (int i = 0; i < static_cast<int>(l); ++i) {
        for (int j = 0; j < static_cast<int>(l); ++j) {
          if (std::abs(i - j) >= 2) {
            rels.push_back({{s[i], s[j]}, {s[j], s[i]}});
          }
        }
      }

      for (size_t i = 1; i < l - 1; ++i) {
        rels.push_back({{s[i], s[i + 1], s[i]}, {s[i + 1], s[i], s[i + 1]}});
      }

      rels.push_back({{s[1], s[0], s[1], s[0]}, {s[0], s[1], s[0], s[1]}});
      rels.push_back({{s[1], s[0], s[1], s[0]}, {s[0], s[1], s[0]}});

      return rels;
    }

    LIBSEMIGROUPS_TEST_CASE("FpSemigroup 029",
                            "Rook monoid R5, q = 0",
                            "[quick]") {
      REPORTER.set_report(REPORT);
      FpSemigroup S;
      S.set_alphabet(6);
      for (relation_type const& rl : RookMonoid(5, 0)) {
        S.add_rule(rl);
      }
      REQUIRE(S.nr_rules() == 33);
      REQUIRE(!S.is_obviously_infinite());
      REQUIRE(!S.knuth_bendix()->confluent());
      REQUIRE(S.size() == 1546);
      REQUIRE(S.isomorphic_non_fp_semigroup()->nrrules() == 71);
    }

    LIBSEMIGROUPS_TEST_CASE("FpSemigroup 030",
                            "Rook monoid R5, q = 1",
                            "[quick]") {
      REPORTER.set_report(REPORT);
      FpSemigroup S;
      S.set_alphabet(6);
      for (relation_type const& rl : RookMonoid(5, 1)) {
        S.add_rule(rl);
      }
      REQUIRE(S.nr_rules() == 33);
      REQUIRE(!S.is_obviously_infinite());
      REQUIRE(!S.knuth_bendix()->confluent());
      REQUIRE(S.size() == 1546);
      REQUIRE(S.isomorphic_non_fp_semigroup()->nrrules() == 71);
    }

    LIBSEMIGROUPS_TEST_CASE("FpSemigroup 031",
                            "Rook monoid R6, q = 0",
                            "[quick]") {
      REPORTER.set_report(REPORT);
      FpSemigroup S;
      S.set_alphabet(7);
      for (relation_type const& rl : RookMonoid(6, 0)) {
        S.add_rule(rl);
      }
      REQUIRE(S.nr_rules() == 45);
      REQUIRE(!S.is_obviously_infinite());
      REQUIRE(!S.knuth_bendix()->confluent());
      REQUIRE(S.size() == 13327);
      REQUIRE(S.isomorphic_non_fp_semigroup()->nrrules() == 207);
    }

    LIBSEMIGROUPS_TEST_CASE("FpSemigroup 032",
                            "Rook monoid R6, q = 1",
                            "[quick]") {
      REPORTER.set_report(REPORT);
      FpSemigroup S;
      S.set_alphabet(7);
      for (relation_type const& rl : RookMonoid(6, 1)) {
        S.add_rule(rl);
      }
      REQUIRE(S.nr_rules() == 45);
      REQUIRE(!S.is_obviously_infinite());
      REQUIRE(!S.knuth_bendix()->confluent());
      REQUIRE(S.size() == 13327);
      REQUIRE(S.isomorphic_non_fp_semigroup()->nrrules() == 207);
    }


    template <typename T>
    std::vector<T> concat(std::vector<T> lhs, const std::vector<T>& rhs) {
      lhs.insert(lhs.end(), rhs.begin(), rhs.end());
      return lhs;
    }

    std::vector<relation_type> Stell(size_t l) {
      std::vector<size_t> pi;
      for (size_t i = 0; i < l; ++i) {
        pi.push_back(i);  // 0 est \pi_0
      }

      std::vector<relation_type> rels{};
      std::vector<size_t>        t{pi[0]};
      for (int i = 1; i < static_cast<int>(l); ++i) {
        t.insert(t.begin(), pi[i]);
        rels.push_back({concat(t, {pi[i]}), t});
      }
      return rels;
    }

    LIBSEMIGROUPS_TEST_CASE("Congruence 033", "Stellar S2", "[quick]") {
      REPORTER.set_report(REPORT);
      FpSemigroup S;
      S.set_alphabet(3);
      for (relation_type const& rl : RookMonoid(2, 0)) {
        S.add_rule(rl);
      }

      REQUIRE(S.nr_rules() == 9);
      REQUIRE(!S.is_obviously_infinite());
      REQUIRE(S.knuth_bendix()->confluent());
      REQUIRE(S.size() == 7);
      REQUIRE(S.isomorphic_non_fp_semigroup()->size() == 7);

      Congruence cong(TWOSIDED, S);
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

    LIBSEMIGROUPS_TEST_CASE("Congruence 034", "Stellar S3", "[quick]") {
      REPORTER.set_report(REPORT);
      FpSemigroup S;
      S.set_alphabet(4);
      for (relation_type const& rl : RookMonoid(3, 0)) {
        S.add_rule(rl);
      }

      REQUIRE(S.nr_rules() == 15);
      REQUIRE(!S.is_obviously_infinite());
      REQUIRE(!S.knuth_bendix()->confluent());
      REQUIRE(S.size() == 34);
      REQUIRE(S.isomorphic_non_fp_semigroup()->size() == 34);

      Congruence cong(TWOSIDED, S);
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

    LIBSEMIGROUPS_TEST_CASE("Congruence 035", "Stellar S4", "[quick]") {
      REPORTER.set_report(REPORT);
      FpSemigroup S;
      S.set_alphabet(5);
      for (relation_type const& rl : RookMonoid(4, 0)) {
        S.add_rule(rl);
      }

      REQUIRE(S.nr_rules() == 23);
      REQUIRE(!S.is_obviously_infinite());
      REQUIRE(!S.knuth_bendix()->confluent());
      REQUIRE(S.size() == 209);
      REQUIRE(S.isomorphic_non_fp_semigroup()->size() == 209);

      Congruence cong(TWOSIDED, S);
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

    LIBSEMIGROUPS_TEST_CASE("Congruence 036", "Stellar S5", "[quick]") {
      REPORTER.set_report(REPORT);
      FpSemigroup S;
      S.set_alphabet(6);
      for (relation_type const& rl : RookMonoid(5, 0)) {
        S.add_rule(rl);
      }

      REQUIRE(S.nr_rules() == 33);
      REQUIRE(!S.is_obviously_infinite());
      REQUIRE(!S.knuth_bendix()->confluent());
      REQUIRE(S.size() == 1546);
      REQUIRE(S.isomorphic_non_fp_semigroup()->size() == 1546);

      Congruence cong(TWOSIDED, S);
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

    LIBSEMIGROUPS_TEST_CASE("Congruence 037", "Stellar S6", "[quick]") {
      REPORTER.set_report(REPORT);
      FpSemigroup S;
      S.set_alphabet(7);
      for (relation_type const& rl : RookMonoid(6, 0)) {
        S.add_rule(rl);
      }

      REQUIRE(S.nr_rules() == 45);
      REQUIRE(!S.is_obviously_infinite());
      REQUIRE(!S.knuth_bendix()->confluent());
      REQUIRE(S.size() == 13327);

      Congruence cong(TWOSIDED, S);
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

    LIBSEMIGROUPS_TEST_CASE("Congruence 038", "Stellar S7", "[standard]") {
      // TODO check test category
      REPORTER.set_report(REPORT);
      FpSemigroup S;
      S.set_alphabet(8);
      for (relation_type const& rl : RookMonoid(7, 0)) {
        S.add_rule(rl);
      }

      REQUIRE(S.nr_rules() == 59);
      REQUIRE(!S.is_obviously_infinite());
      REQUIRE(!S.knuth_bendix()->confluent());
      REQUIRE(S.size() == 130922);

      Congruence cong(TWOSIDED, S);
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

    LIBSEMIGROUPS_TEST_CASE("Congruence 039",
                            "left cong. on an f.p. semigroup",
                            "[quick]") {
      REPORTER.set_report(REPORT);

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

      libsemigroups::congruence::KBP kbp(LEFT, S.knuth_bendix());
      // kbp.add_pair({0}, {1, 1, 1});
      kbp.add_pair({1, 1}, {0, 0, 0, 0, 0, 0, 0});

      REQUIRE(kbp.nr_classes() == 11);

      Congruence cong1(LEFT, S);
      cong1.add_pair({0}, {1, 1, 1});
      REQUIRE(cong1.nr_classes() == 11);

      Congruence cong2(LEFT, S);
      cong2.add_pair({1, 1}, {0, 0, 0, 0, 0, 0, 0});
      REQUIRE(cong1.nr_classes() == cong2.nr_classes());
    }

    LIBSEMIGROUPS_TEST_CASE("Congruence 040",
                            "2-sided cong. on infinite f.p. semigroup",
                            "[quick]") {
      REPORTER.set_report(REPORT);
      FpSemigroup S;
      S.set_alphabet(3);

      Congruence cong(TWOSIDED, S);
      cong.add_pair({1}, {2});
      cong.add_pair({0, 0}, {0});
      cong.add_pair({0, 1}, {1, 0});
      cong.add_pair({0, 1}, {1});
      cong.add_pair({0, 2}, {2, 0});
      cong.add_pair({0, 2}, {2});

      REQUIRE(!cong.contains({1}, {2, 2, 2, 2, 2, 2, 2, 2, 2, 2}));
    }
  }  // namespace tmp
}  // namespace libsemigroups
