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

#include "bmat8.h"
#include "cong-new.h"
#include "element-helper.h"
#include "fpsemi.h"
#include "semigroup.h"
#include "tce.h"

namespace libsemigroups {
  namespace tmp {
    template <class TElementType>
    void delete_gens(std::vector<TElementType>& gens) {
      for (auto& x : gens) {
        delete x;
      }
    }

    constexpr bool REPORT = false;

    congruence_type const TWOSIDED = congruence_type::TWOSIDED;
    congruence_type const LEFT     = congruence_type::LEFT;
    congruence_type const RIGHT    = congruence_type::RIGHT;

    LIBSEMIGROUPS_TEST_CASE("000",
                            "left congruence on fp semigroup",
                            "[quick]") {
      REPORTER.set_report(REPORT);
      FpSemigroup S;
      S.set_alphabet(2);
      S.add_rule(word_type({0, 0, 0}), word_type({0}));
      S.add_rule(word_type({0}), word_type({1, 1}));

      Congruence cong(LEFT, S);
    }

    LIBSEMIGROUPS_TEST_CASE("001",
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

    LIBSEMIGROUPS_TEST_CASE("002",
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

    LIBSEMIGROUPS_TEST_CASE("003",
                            "word_to_class_index for cong. on fp semigroup",
                            "[quick]") {
      REPORTER.set_report(REPORT);

      FpSemigroup S;
      S.set_alphabet(2);
      S.add_rule({0, 0, 0}, {0});  // (a^3, a)
      S.add_rule({0}, {1, 1});     // (a, b^2)

      Congruence cong(LEFT, S);

      REQUIRE(cong.word_to_class_index({0, 0, 1})
              == cong.word_to_class_index({0, 0, 0, 0, 1}));
      REQUIRE(cong.contains({0, 1, 1, 0, 0, 1}, {0, 0, 1}));
      REQUIRE(cong.word_to_class_index({0, 0, 0})
              != cong.word_to_class_index({0, 0, 1}));
      REQUIRE(cong.word_to_class_index({1})
              != cong.word_to_class_index({0, 0, 0, 0}));
      REQUIRE(!cong.contains({0, 0, 0, 0}, {0, 0, 1}));
    }

    LIBSEMIGROUPS_TEST_CASE("004",
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

    LIBSEMIGROUPS_TEST_CASE("005",
                            "trivial congruence on non-fp semigroup",
                            "[quick]") {
      REPORTER.set_report(REPORT);

      using Transf = Transf<5>::type;
      Semigroup<Transf> S({Transf({1, 3, 4, 2, 3}), Transf({3, 2, 1, 3, 3})});
      REQUIRE(S.size() == 88);

      Congruence cong(TWOSIDED, S);
      REQUIRE(cong.nr_classes() == 88);
    }

    LIBSEMIGROUPS_TEST_CASE("006",
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

    LIBSEMIGROUPS_TEST_CASE("007",
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

    LIBSEMIGROUPS_TEST_CASE("008",
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

    LIBSEMIGROUPS_TEST_CASE("009",
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

    LIBSEMIGROUPS_TEST_CASE("010",
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

    LIBSEMIGROUPS_TEST_CASE("011",
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

    LIBSEMIGROUPS_TEST_CASE("012",
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

    LIBSEMIGROUPS_TEST_CASE("013",
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

    LIBSEMIGROUPS_TEST_CASE("014",
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

    LIBSEMIGROUPS_TEST_CASE("015",
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

    LIBSEMIGROUPS_TEST_CASE("016",
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

    LIBSEMIGROUPS_TEST_CASE("017",
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
    LIBSEMIGROUPS_TEST_CASE("018",
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
      // REQUIRE(cong.nr_classes());  // Currently runs forever
    }

    LIBSEMIGROUPS_TEST_CASE(
        "019",
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
      REQUIRE(cong.contains(x, y));
      REQUIRE(!cong.less({0, 0, 0}, {1}));
      REQUIRE(cong.less({1}, {0, 0, 0}));
      REQUIRE(!cong.less(x, y));
      REQUIRE(!cong.less(y, x));
      REQUIRE(cong.contains(x, y));
    }

    // The next test is deprecated.
    LIBSEMIGROUPS_TEST_CASE("020",
                            "trivial cong. on an fp semigroup",
                            "[quick]") {
      FpSemigroup S;
      S.set_alphabet("ab");
      S.add_rule("ab", "ba");
      S.add_rule("a", "b");

      Congruence cong(LEFT, S);
      REQUIRE(cong.nr_non_trivial_classes() == 0);
    }

    LIBSEMIGROUPS_TEST_CASE("021", "duplicate generators", "[quick]") {
      REPORTER.set_report(REPORT);
      using Transf = Transf<8>::type;
      Semigroup<Transf> S({Transf({7, 3, 5, 3, 4, 2, 7, 7}),
                           Transf({7, 3, 5, 3, 4, 2, 7, 7}),
                           Transf({7, 3, 5, 3, 4, 2, 7, 7}),
                           Transf({3, 6, 3, 4, 0, 6, 0, 7})});
      Congruence        cong(TWOSIDED, S);
      REQUIRE(cong.nr_classes() == S.size());
    }

    LIBSEMIGROUPS_TEST_CASE("022", "non-trivial classes", "[quick]") {
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
    LIBSEMIGROUPS_TEST_CASE("023",
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

    LIBSEMIGROUPS_TEST_CASE("024", "redundant generating pairs", "[quick]") {
      REPORTER.set_report(REPORT);
      FpSemigroup S;
      S.set_alphabet(1);

      Congruence cong(TWOSIDED, S);
      cong.add_pair({0, 0}, {0, 0});
      REQUIRE(cong.contains({0, 0}, {0, 0}));
    }

    LIBSEMIGROUPS_TEST_CASE("025",
                            "2-sided cong. on free semigroup",
                            "[quick]") {
      REPORTER.set_report(REPORT);
      FpSemigroup S;
      S.set_alphabet("a");
      Congruence cong(TWOSIDED, S);
      REQUIRE(cong.contains({0, 0}, {0, 0}));
      REQUIRE(!cong.contains({0, 0}, {0}));
    }

    LIBSEMIGROUPS_TEST_CASE("026",
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

    LIBSEMIGROUPS_TEST_CASE("027", "less", "[quick]") {
      FpSemigroup S;
      S.set_alphabet(2);
      S.add_rule({0, 0}, {0});

      Congruence cong(TWOSIDED, S);
      REQUIRE(!cong.less({0, 0}, {0}));
    }

    LIBSEMIGROUPS_TEST_CASE("028",
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
        Semigroup<BMat8>  S({gens[0], gens[2], gens[3]});
        Congruence cong(TWOSIDED, S);
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

    LIBSEMIGROUPS_TEST_CASE("029",
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

      Element* t3 = new Transformation<u_int16_t>({1, 3, 1, 3, 3});
      Element* t4 = new Transformation<u_int16_t>({4, 2, 4, 4, 2});
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

    LIBSEMIGROUPS_TEST_CASE("030",
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

      Element* t3 = new Transformation<u_int16_t>({1, 3, 1, 3, 3});
      Element* t4 = new Transformation<u_int16_t>({4, 2, 4, 4, 2});
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
      LIBSEMIGROUPS_TEST_CASE("031",
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
        Element* t1 = new Transformation<u_int16_t>({3, 4, 4, 4, 4});
        Element* t2 = new Transformation<u_int16_t>({3, 1, 3, 3, 3});
        word_type   w1, w2;
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

      LIBSEMIGROUPS_TEST_CASE("032", "contains", "[broken]") {
        REPORTER.set_report(true);
        FpSemigroup S;
        S.set_alphabet(2);
        Congruence cong(TWOSIDED, S);
        cong.add_pair({0, 0}, {0});
        cong.add_pair({0, 1}, {0});
        cong.add_pair({1, 0}, {0});
        // FIXME this runs forever but shouldn't!
        REQUIRE(cong.contains({0, 0}, {0}));
        REQUIRE(cong.contains({0, 1}, {0}));
        REQUIRE(cong.contains({1, 0}, {0}));
      }
  }  // namespace tmp
}  // namespace libsemigroups
        /*fpsemigroup::KnuthBendix S;
        S.set_alphabet(2);
        S.add_rule({0, 0}, {0});
        S.add_rule({0, 1}, {0});
        S.add_rule({1, 0}, {0});

        REQUIRE(S.equal_to(word_type({0, 0}), word_type({0})));*/
