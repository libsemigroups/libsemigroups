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
#include "cong-new.h"
#include "element-helper.h"
#include "fpsemi.h"
#include "semigroup.h"
#include "tce.h"

namespace libsemigroups {
  namespace tmp {
    constexpr bool REPORT = false;

    congruence_type const TWOSIDED = congruence_type::TWOSIDED;
    congruence_type const LEFT     = congruence_type::LEFT;
    congruence_type const RIGHT    = congruence_type::RIGHT;

    TEST_CASE("Congruence 00: construct from fp semigroup",
              "[quick][congruence-new][00]") {
      REPORTER.set_report(REPORT);
      FpSemigroup S;
      S.set_alphabet(2);
      S.add_rule(word_type({0, 0, 0}), word_type({0}));
      S.add_rule(word_type({0}), word_type({1, 1}));

      Congruence cong(LEFT, S);
    }

    TEST_CASE("Congruence 01: 2-sided congruence on fp semigroup",
              "[quick][congruence-new][01]") {
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

    TEST_CASE("Congruence 02: left congruence on fp semigroup",
              "[quick][congruence-new][02]") {
      REPORTER.set_report(REPORT);
      FpSemigroup S;
      S.set_alphabet(2);
      S.add_rule({0, 0, 0}, {0});  // (a^3, a)
      S.add_rule({0}, {1, 1});     // (a, b^2)

      Congruence cong(LEFT, S);
      REQUIRE(cong.nr_classes() == 5);
    }

    TEST_CASE("Congruence 04: word_to_class_index for left congruence on fp "
              "semigroup ",
              "[quick][congruence-new][04]") {
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

    TEST_CASE("Congruence 05: word_to_class_index for 2-sided congruence on fp "
              "semigroup ",
              "[quick][congruence-new][05]") {
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

    TEST_CASE("Congruence 06: trivial congruence on non-fp semigroup",
              "[quick][congruence-new][06]") {
      REPORTER.set_report(REPORT);

      using Transf = Transf<5>::type;
      Semigroup<Transf> S({Transf({1, 3, 4, 2, 3}), Transf({3, 2, 1, 3, 3})});
      REQUIRE(S.size() == 88);

      Congruence cong(TWOSIDED, S);
      REQUIRE(cong.nr_classes() == 88);
    }

    TEST_CASE("Congruence 07: 2-sided congruence on non-fp semigroup",
              "[quick][congruence-new][07]") {
      REPORTER.set_report(REPORT);

      using Transf = Transf<5>::type;
      Semigroup<Transf> S({Transf({1, 3, 4, 2, 3}), Transf({3, 2, 1, 3, 3})});
      REQUIRE(S.size() == 88);

      Congruence cong(TWOSIDED, S);
      cong.add_pair(S.factorisation(Transf({3, 4, 4, 4, 4})),
                    S.factorisation(Transf({3, 1, 3, 3, 3})));
      REQUIRE(cong.nr_classes() == 21);
    }

    TEST_CASE("Congruence 08: 2-sided congruence on fp semigroup",
              "[quick][congruence-new][08]") {
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

    TEST_CASE("Congruence 09: 2-sided congruence on infinite fp semigroup",
              "[quick][congruence-new][09]") {
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

    TEST_CASE("Congruence 10: 2-sided congruence on infinite fp semigroup",
                "[quick][congruence-new][10]") {
        REPORTER.set_report(true);
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

    /*
      TEST_CASE("Congruence 11: 2-sided congruence on finite semigroup",
                "[quick][congruence-new][finite][11]") {
        REPORTER.set_report(REPORT);
        std::vector<Element*> gens
            = {new Transformation<u_int16_t>({7, 3, 5, 3, 4, 2, 7, 7}),
               new Transformation<u_int16_t>({1, 2, 4, 4, 7, 3, 0, 7}),
               new Transformation<u_int16_t>({0, 6, 4, 2, 2, 6, 6, 4}),
               new Transformation<u_int16_t>({3, 6, 3, 4, 0, 6, 0, 7})};
        Semigroup<> S = Semigroup<>(gens);
        really_delete_cont(gens);

        // The following lines are intentionally commented out so that we can
      check
        // that P does not enumerate the semigroup, they remain to remind us of
      the
        // size and number of rules of the semigroups.
        // REQUIRE(S.size(false) == 11804);
        // REQUIRE(S.nrrules(false) == 2460);

        Congruence cong(
  S.add_rule({0, 3, 2, 1, 3, 2, 2}, {3, 2, 2, 1, 3,
      3})});

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

      TEST_CASE("Congruence 12: 2-sided congruence on finite semigroup",
                "[extreme][congruence-new][12]") {
        REPORTER.set_report(true);
        std::vector<Element*> gens = {
            new PBR(new std::vector<std::vector<u_int32_t>>({{2}, {3}, {0},
      {1}})), new PBR( new std::vector<std::vector<u_int32_t>>({{}, {2}, {1},
      {0, 3}})), new PBR( new std::vector<std::vector<u_int32_t>>({{0, 3}, {2},
      {1}, {}})), new PBR( new std::vector<std::vector<u_int32_t>>({{1, 2}, {3},
      {0}, {1}})), new PBR( new std::vector<std::vector<u_int32_t>>({{2}, {3},
      {0}, {1, 3}})), new PBR(new std::vector<std::vector<u_int32_t>>({{3}, {1},
      {0}, {1}})), new PBR( new std::vector<std::vector<u_int32_t>>({{3}, {2},
      {0}, {0, 1}})), new PBR(new std::vector<std::vector<u_int32_t>>({{3}, {2},
      {0}, {1}})), new PBR(new std::vector<std::vector<u_int32_t>>({{3}, {2},
      {0}, {3}})), new PBR(new std::vector<std::vector<u_int32_t>>({{3}, {2},
      {1}, {0}})), new PBR( new std::vector<std::vector<u_int32_t>>({{3}, {2,
      3}, {0}, {1}}))}; REQUIRE(gens[0]->degree() == 2);

        Semigroup<> S = Semigroup<>(gens);
        really_delete_cont(gens);

        // REQUIRE(S.size() == 65536);
        // REQUIRE(S.nrrules() == 45416);

  S.add_rule> extra(
  S.add_rule({7, 10, 9, 3, 6, 9, 4, 7, 9, 10},
                        {9, 3, 6, 6, 10, 9, 4, 7}),
  S.add_rule({8, 7, 5, 8, 9, 8}, {6, 3, 8, 6, 1, 2, 4})});
        Congruence cong(TWOSIDED, &S, extra);

        REQUIRE(cong.nr_classes() == 19009);
        size_t                             nr = 0;
        std::unordered_map<size_t, size_t> result;
        for (auto it = cong.cbegin_ntc();
             it < cong.cend_ntc();
             ++it) {
          auto result_it = result.find((*it).size());
          if (result_it == result.end()) {
            result.emplace((*it).size(), 1);
          } else {
            ++(*result_it).second;
          }
          nr++;
        }
        REQUIRE(nr == 577);

        std::unordered_map<size_t, size_t> expected;
        expected.emplace(4, 384);
        expected.emplace(16, 176);
        expected.emplace(96, 16);
        expected.emplace(41216, 1);

        REQUIRE(result == expected);
      }

      TEST_CASE("Congruence 13: 2-sided congruence on finite semigroup",
                "[quick][congruence-new][13]") {
        std::vector<Element*> gens
            = {new PartialPerm<u_int16_t>({0, 1, 2}, {4, 0, 1}, 6),
               new PartialPerm<u_int16_t>({0, 1, 2, 3, 5}, {2, 5, 3, 0, 4}, 6),
               new PartialPerm<u_int16_t>({0, 1, 2, 3}, {5, 0, 3, 1}, 6),
               new PartialPerm<u_int16_t>({0, 2, 5}, {3, 4, 1}, 6),
               new PartialPerm<u_int16_t>({0, 2, 5}, {0, 2, 5}, 6),
               new PartialPerm<u_int16_t>({0, 1, 4}, {1, 2, 0}, 6),
               new PartialPerm<u_int16_t>({0, 2, 3, 4, 5}, {3, 0, 2, 5, 1}, 6),
               new PartialPerm<u_int16_t>({0, 1, 3, 5}, {1, 3, 2, 0}, 6),
               new PartialPerm<u_int16_t>({1, 3, 4}, {5, 0, 2}, 6)};

        Semigroup<> S = Semigroup<>(gens);
        S.set_report(REPORT);
        really_delete_cont(gens);

        // REQUIRE(S.size() == 712);
        // REQUIRE(S.nrrules() == 1121);

  S.add_rule> extra
  S.add_rule({2, 7}, {1, 6, 6, 1})};
        Congruence cong(TWOSIDED, &S, extra);
        REPORTER.set_report(REPORT);

        REQUIRE(cong.nr_classes() == 32);
      }

      TEST_CASE("Congruence 14: trivial 2-sided congruence on bicyclic monoid",
                "[quick][congruence-new][14]") {
        REPORTER.set_report(REPORT);
  S.add_rule({0, 1}, {1}),
  S.add_rule({1, 0}, {1}),
  S.add_rule({0, 0}, {0}),
  S.add_rule({0, 2}, {2}),
  S.add_rule({2, 0}, {2}),
  S.add_rule({1, 2}, {0})};
        FpSemigroup             S(3, rels);
        Congruence              cong(TWOSIDED, &S, {});
        REQUIRE(cong.word_to_class_index({0})
                == cong.word_to_class_index({1, 2, 1, 1, 2, 2}));
        REQUIRE(cong.word_to_class_index({0})
                == cong.word_to_class_index({1, 0, 2, 0, 1, 2}));
        REQUIRE(cong.word_to_class_index({2, 1})
                == cong.word_to_class_index({1, 2, 0, 2, 1, 1, 2}));
        REQUIRE(cong.contains({2, 1}, {1, 2, 0, 2, 1, 1, 2}));
      }

      TEST_CASE("Congruence 15: non-trivial 2-sided congruence on bicyclic
      monoid",
                "[quick][congruence-new][15]") {
        REPORTER.set_report(REPORT);
  S.add_rule({0, 1}, {1}),
  S.add_rule({1, 0}, {1}),
  S.add_rule({0, 0}, {0}),
  S.add_rule({0, 2}, {2}),
  S.add_rule({2, 0}, {2}),
  S.add_rule({1, 2}, {0})};
        FpSemigroup             S(3, rels);
  S.add_rule({1, 1, 1}, {0})});
        Congruence              cong(TWOSIDED, &S, extra);
        REQUIRE(cong.nr_classes() == 3);
      }

      TEST_CASE("Congruence 16: 2-sided congruence on free abelian monoid",
                "[quick][congruence-new][16]") {
        REPORTER.set_report(REPORT);
  S.add_rule({0, 1}, {1}),
  S.add_rule({1, 0}, {1}),
  S.add_rule({0, 0}, {0}),
  S.add_rule({0, 2}, {2}),
  S.add_rule({2, 0}, {2}),
  S.add_rule({1, 2}, {2, 1})};
        FpSemigroup             S(3, rels);
  S.add_rule> extra(
  S.add_rule({2, 2, 2}, {2})});
        Congruence cong(TWOSIDED, S, extra);

        REQUIRE(cong.nr_classes() == 15);
      }

      TEST_CASE("Congruence 17: 2-sided congruence on finite semigroup",
                "[extreme][congruence-new][17]") {
        REPORTER.set_report(true);
        std::vector<Element*> gens = {
            new PBR(new std::vector<std::vector<u_int32_t>>({{2}, {3}, {0},
      {1}})), new PBR( new std::vector<std::vector<u_int32_t>>({{}, {2}, {1},
      {0, 3}})), new PBR( new std::vector<std::vector<u_int32_t>>({{0, 3}, {2},
      {1}, {}})), new PBR( new std::vector<std::vector<u_int32_t>>({{1, 2}, {3},
      {0}, {1}})), new PBR( new std::vector<std::vector<u_int32_t>>({{2}, {3},
      {0}, {1, 3}})), new PBR(new std::vector<std::vector<u_int32_t>>({{3}, {1},
      {0}, {1}})), new PBR( new std::vector<std::vector<u_int32_t>>({{3}, {2},
      {0}, {0, 1}})), new PBR(new std::vector<std::vector<u_int32_t>>({{3}, {2},
      {0}, {1}})), new PBR(new std::vector<std::vector<u_int32_t>>({{3}, {2},
      {0}, {3}})), new PBR(new std::vector<std::vector<u_int32_t>>({{3}, {2},
      {1}, {0}})), new PBR( new std::vector<std::vector<u_int32_t>>({{3}, {2,
      3}, {0}, {1}}))}; REQUIRE(gens[0]->degree() == 2);

        Semigroup<> S = Semigroup<>(gens);
        really_delete_cont(gens);

        // REQUIRE(S.size() == 65536);
        // REQUIRE(S.nrrules() == 45416);

  S.add_rule> extra(
  S.add_rule({7, 10, 9, 3, 6, 9, 4, 7, 9, 10},
                        {9, 3, 6, 6, 10, 9, 4, 7}),
  S.add_rule({8, 7, 5, 8, 9, 8}, {6, 3, 8, 6, 1, 2, 4})});
        Congruence cong(TWOSIDED, &S, extra);

        REQUIRE(cong.nr_classes() == 19009);

        // Partition<word_t>* ntc = cong.nontrivial_classes();
        // REQUIRE(ntc->size() == 577);
        // std::vector<size_t> sizes({0, 0, 0, 0});
        // for (size_t i = 0; i < ntc->size(); i++) {
        //   switch (ntc->at(i)->size()) {
        //     case 4:
        //       sizes[0]++;
        //       break;
        //     case 16:
        //       sizes[1]++;
        //       break;
        //     case 96:
        //       sizes[2]++;
        //       break;
        //     case 41216:
        //       sizes[3]++;
        //       break;
        //     default:
        //       REQUIRE(false);
        //   }
        // }
        // REQUIRE(sizes == std::vector<size_t>({384, 176, 16, 1}));
        // delete ntc;
      }

      TEST_CASE("Congruence 18: 2-sided congruence on finite semigroup",
                "[quick][congruence-new][18]") {
        std::vector<Element*> gens
            = {new Transformation<u_int16_t>({1, 3, 4, 2, 3}),
               new Transformation<u_int16_t>({3, 2, 1, 3, 3})};
        Semigroup<> S = Semigroup<>(gens);
        S.set_report(REPORT);
        really_delete_cont(gens);
        REQUIRE(S.size() == 88);
        REQUIRE(S.nrrules() == 18);
        REQUIRE(S.degree() == 5);
        Element* t1 = new Transformation<u_int16_t>({3, 4, 4, 4, 4});
        Element* t2 = new Transformation<u_int16_t>({3, 1, 3, 3, 3});
        word_t   w1, w2;
        S.factorisation(w1, S.position(t1));
        S.factorisation(w2, S.position(t2));
  S.add_rule> extra({std::make_pair(w1, w2)});
        Congruence              cong(TWOSIDED, &S, extra);
        REPORTER.set_report(REPORT);

        REQUIRE(cong.nr_classes() == 21);
        REQUIRE(cong.nr_classes() == 21);
        Element* t3 = new Transformation<u_int16_t>({1, 3, 1, 3, 3});
        Element* t4 = new Transformation<u_int16_t>({4, 2, 4, 4, 2});
        word_t   w3, w4;
        S.factorisation(w3, S.position(t3));
        S.factorisation(w4, S.position(t4));
        REQUIRE(cong.word_to_class_index(w3) == cong.word_to_class_index(w4));
        REQUIRE(cong.contains(w3, w4));

        t1->really_delete();
        t2->really_delete();
        t3->really_delete();
        t4->really_delete();
        delete t1;
        delete t2;
        delete t3;
        delete t4;
      }

      // The next test behaves as expected but runs forever, since the
      nr_classes
      // method requires to know the size of the semigroup S, and we cannot
      // currently work that out.

      // TEST_CASE("Congruence 19: infinite fp semigroup from GAP library",
      //           "[eternal][congruence-new][19]") {
      //   REPORTER.set_report(true);
  S.add_rule({0, 0}, {0, 0}),
  S.add_rule({0, 1}, {1, 0}),
  S.add_rule({0, 2}, {2, 0}),
  S.add_rule({0, 0}, {0}),
  S.add_rule({0, 2}, {0}),
  S.add_rule({2, 0}, {0}),
  S.add_rule({1, 0}, {0, 1}),
  S.add_rule({1, 1}, {1, 1}),
  S.add_rule({1, 2}, {2, 1}),
  S.add_rule({1, 1, 1}, {1}),
  S.add_rule({1, 2}, {1}),
  S.add_rule({2, 1}, {1})};
  S.add_rule({0}, {1})};
      //
      //   FpSemigroup S(3, rels);
      //   Congruence  cong(TWOSIDED, S, extra);
      //   REQUIRE(cong.nr_classes());  // Runs forever
      //
      //   REQUIRE(!cong.finished());
      //
      //   Partition<word_t>* ntc = cong.nontrivial_classes();
      //   REQUIRE(ntc->size() == 1);
      //   REQUIRE((*ntc)[0]->size() == 5);
      //   delete ntc;
      //
      //   REQUIRE(cong.finished());
      // }

      // The next test case no longer works because partial data is no longer.
      // TEST_CASE(
      //     "Congruence 20: 2-sided congruence on fp semigroup with infinite
      //     classes",
      //     "[deprecated][fails][congruence-new][20]") {
      //   REPORTER.set_report(true);
  S.add_rule> rels
  S.add_rule({0, 1}, {1, 0})};
      //   FpSemigroup S(2, rels);
  S.add_rule({0}, {0, 0})};
      //   Congruence              cong(TWOSIDED, S, extra);
      //   word_t x = {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
      1,
      //   1};
      //   word_t y
      //       = {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
      1};
      //   REQUIRE(cong.contains(x, y));
      //   REQUIRE(cong.less({0, 0, 0}, {1}));
      //   REQUIRE(!cong.less({1}, {0, 0, 0}));
      //   REQUIRE(!cong.less(x, y));
      //   REQUIRE(!cong.less(y, x));
      //   REQUIRE(cong.contains(x, y));
      // }

      // The next test is deprecated.
      // TEST_CASE("Congruence 21: trivial cong on an fp semigroup",
      //           "[quick][congruence-new][21]") {
  S.add_rule> rels;
  S.add_rule({0, 0, 0}, {0}));  // (a^3, a)
  S.add_rule({0}, {1}));        // (a, b)
  S.add_rule> extra;
      //   Congruence cong(LEFT, 2, rels, extra);
      //   Partition<word_t>* ntc = cong.nontrivial_classes();
      //   REQUIRE(ntc->size() == 0);
      //   delete ntc;
      // }

      TEST_CASE("Congruence 22: 2-sided congruence on finite semigroup with "
                "duplicate generators",
                "[quick][congruence-new][finite][22]") {
        REPORTER.set_report(REPORT);
        std::vector<Element*> gens
            = {new Transformation<u_int16_t>({7, 3, 5, 3, 4, 2, 7, 7}),
               new Transformation<u_int16_t>({7, 3, 5, 3, 4, 2, 7, 7}),
               new Transformation<u_int16_t>({7, 3, 5, 3, 4, 2, 7, 7}),
               new Transformation<u_int16_t>({3, 6, 3, 4, 0, 6, 0, 7})};
        Semigroup<> S = Semigroup<>(gens);
        really_delete_cont(gens);
  S.add_rule>());

        REQUIRE(cong.nr_classes() == S.size());
      }

      // The next test is deprecated.
      // TEST_CASE("Congruence 23: test nontrivial_classes for a fp semigroup
      cong",
      //           "[quick][congruence-new][finite][23]") {
  S.add_rule> rels
  S.add_rule({0, 0, 0}, {0}),
  S.add_rule({1, 0, 0}, {1, 0}),
  S.add_rule({1, 0, 1, 1, 1}, {1, 0}),
  S.add_rule({1, 1, 1, 1, 1}, {1, 1}),
  S.add_rule({1, 1, 0, 1, 1, 0}, {1, 0, 1, 0, 1, 1}),
  S.add_rule({0, 0, 1, 0, 1, 1, 0}, {0, 1, 0, 1, 1, 0}),
  S.add_rule({0, 0, 1, 1, 0, 1, 0}, {0, 1, 1, 0, 1, 0}),
  S.add_rule({0, 1, 0, 1, 0, 1, 0}, {1, 0, 1, 0, 1, 0}),
  S.add_rule({1, 0, 1, 0, 1, 0, 1}, {1, 0, 1, 0, 1, 0}),
  S.add_rule({1, 0, 1, 0, 1, 1, 0}, {1, 0, 1, 0, 1, 1}),
  S.add_rule({1, 0, 1, 1, 0, 1, 0}, {1, 0, 1, 1, 0, 1}),
  S.add_rule({1, 1, 0, 1, 0, 1, 0}, {1, 0, 1, 0, 1, 0}),
  S.add_rule({1, 1, 1, 1, 0, 1, 0}, {1, 0, 1, 0}),
  S.add_rule({0, 0, 1, 1, 1, 0, 1, 0}, {1, 1, 1, 0, 1, 0})};
      //   Congruence cong(
  S.add_rule({0},
      {1})}));
      //   REPORTER.set_report(REPORT);
      //   Partition<word_t>* ntc = cong.nontrivial_classes();
      //   REQUIRE(ntc->size() == 1);
      //   delete ntc;
      // }

      TEST_CASE("Congruence 24: right congruence on finite semigroup",
                "[quick][congruence-new][24]") {
        REPORTER.set_report(REPORT);
        std::vector<Element*> gens
            = {new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5, 6, 7}),
               new Transformation<u_int16_t>({1, 2, 3, 4, 5, 0, 6, 7}),
               new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5, 6, 7}),
               new Transformation<u_int16_t>({0, 1, 2, 3, 4, 0, 6, 7}),
               new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5, 7, 6})};
        Semigroup<> S = Semigroup<>(gens);
        really_delete_cont(gens);

        std::vector<Element*> elms
            = {new Transformation<u_int16_t>({0, 0, 0, 0, 0, 0, 7, 6}),
               new Transformation<u_int16_t>({0, 0, 0, 0, 0, 0, 6, 7}),
               new Transformation<u_int16_t>({0, 0, 0, 0, 0, 0, 6, 7}),
               new Transformation<u_int16_t>({1, 1, 1, 1, 1, 1, 6, 7}),
               new Transformation<u_int16_t>({0, 0, 0, 0, 0, 0, 6, 7}),
               new Transformation<u_int16_t>({2, 2, 2, 2, 2, 2, 6, 7}),
               new Transformation<u_int16_t>({0, 0, 0, 0, 0, 0, 6, 7}),
               new Transformation<u_int16_t>({3, 3, 3, 3, 3, 3, 6, 7}),
               new Transformation<u_int16_t>({0, 0, 0, 0, 0, 0, 6, 7}),
               new Transformation<u_int16_t>({4, 4, 4, 4, 4, 4, 6, 7}),
               new Transformation<u_int16_t>({0, 0, 0, 0, 0, 0, 6, 7}),
               new Transformation<u_int16_t>({5, 5, 5, 5, 5, 5, 6, 7}),
               new Transformation<u_int16_t>({0, 0, 0, 0, 0, 0, 7, 6}),
               new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5, 7, 6})};

  S.add_rule> extra;
        word_t                  w1, w2;
        for (size_t i = 0; i < elms.size(); i += 2) {
          S.factorisation(w1, S.position(elms[i]));
          S.factorisation(w2, S.position(elms[i + 1]));
          extra.push_back(std::make_pair(w1, w2));
          elms[i]->really_delete();
          elms[i + 1]->really_delete();
          delete elms[i];
          delete elms[i + 1];
        }

        Congruence cong(RIGHT, &S, extra);

        REQUIRE(cong.nr_classes() == 1);
      }

      TEST_CASE("Congruence 25: 2-sided congruence on free semigroup with "
                "redundant generating pairs",
                "[quick][congruence-new][25]") {
        REPORTER.set_report(REPORT);
        FpSemigroup S(1, {});
  S.add_rule({0, 0}, {0, 0})});
        REQUIRE(cong.contains({0, 0}, {0, 0}));
      }

      TEST_CASE("Congruence 26: 2-sided congruence on free semigroup",
                "[quick][congruence-new][26]") {
        REPORTER.set_report(REPORT);
        FpSemigroup S(1, {});
        Congruence  cong(TWOSIDED, S, {});
        REQUIRE(cong.contains({0, 0}, {0, 0}));
        REQUIRE(!cong.contains({0, 0}, {0}));
      }

      TEST_CASE("Congruence 27: is_quotient_obviously_infinite",
                "[quick][congruence-new][27]") {
  S.add_rule({0, 1}, {0})});
  S.add_rule({2, 2}, {2})});
        REQUIRE(cong1.is_quotient_obviously_infinite());

  S.add_rule({0, 0}, {0})});
  S.add_rule({1, 1}, {1})});
        REQUIRE(cong2.is_quotient_obviously_infinite());

  S.add_rule({1, 2}, {1})});
        REQUIRE(!cong3.is_quotient_obviously_infinite());

        std::vector<Element*> gens = {new Transformation<u_int16_t>({0, 1, 0}),
                                      new Transformation<u_int16_t>({0, 1, 2})};

        Semigroup<> S = Semigroup<>(gens);
        S.set_report(REPORT);
        really_delete_cont(gens);

  S.add_rule({1}, {0})});
        REQUIRE(!cong4.is_quotient_obviously_infinite());
      }

      TEST_CASE("Congruence 28: less for a 2-sided congruence over fp
      semigroup",
                "[quick][congruence-new][28]") {
  S.add_rule({0, 0}, {0})});
        Congruence  cong(TWOSIDED, S, {});
        REQUIRE(!cong.less({0, 0}, {0}));
      }

      TEST_CASE("Congruence 29: 2-sided congruences of BMat8 semigroup",
                "[quick][congruence-new][29]") {
        REPORTER.set_report(REPORT);
        std::vector<BMat8> gens
            = {BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
               BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
               BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {1, 0, 0, 1}}),
               BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}})};

        Semigroup<BMat8> S(gens);

  S.add_rule({1}, {0})});
        REQUIRE(cong1.nr_classes() == 3);
        REQUIRE(cong1.word_to_class_index({1}) == 0);

  S.add_rule({1}, {0})});
        REQUIRE(cong2.nr_classes() == 3);
        REQUIRE(cong2.word_to_class_index({1}) == 0);

        Semigroup<BMat8> T({gens[0], gens[2], gens[3]});
  S.add_rule({1}, {0})});
        REQUIRE(cong3.nr_classes() == 2);
        REQUIRE(cong3.word_to_class_index({1}) == 0);

  S.add_rule({1}, {0})});
        REQUIRE(cong4.nr_classes() == 3);
        REQUIRE(cong4.word_to_class_index({1}) == 0);
      }

      TEST_CASE("Congruence 30: left congruence on finite semigroup",
                "[quick][congruence-new][30]") {
        REPORTER.set_report(REPORT);
        std::vector<Element*> gens
            = {new Transformation<u_int16_t>({1, 3, 4, 2, 3}),
               new Transformation<u_int16_t>({3, 2, 1, 3, 3})};
        Semigroup<> S = Semigroup<>(gens);
        S.set_report(REPORT);
        really_delete_cont(gens);

        // REQUIRE(S.size() == 88);
        // REQUIRE(S.degree() == 5);
  S.add_rule> extra(
  S.add_rule({0, 1, 0, 0, 0, 1, 1, 0, 0}, {1, 0, 0, 0, 1})});
        Congruence cong(LEFT, &S, extra);

        REQUIRE(cong.nr_classes() == 69);
        REQUIRE(cong.nr_classes() == 69);

        Element* t3 = new Transformation<u_int16_t>({1, 3, 1, 3, 3});
        Element* t4 = new Transformation<u_int16_t>({4, 2, 4, 4, 2});
        word_t   w3, w4;
        S.factorisation(w3, S.position(t3));
        S.factorisation(w4, S.position(t4));
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

        t3->really_delete();
        t4->really_delete();
        delete t3;
        delete t4;
      }

      // For some reason the following test case doesn't run...
      TEST_CASE("Congruence 31: right congruence on finite semigroup"
                "[quick][congruence-new][31]") {
        REPORTER.set_report(REPORT);
        std::vector<Element*> gens
            = {new Transformation<u_int16_t>({1, 3, 4, 2, 3}),
               new Transformation<u_int16_t>({3, 2, 1, 3, 3})};
        Semigroup<> S = Semigroup<>(gens);
        really_delete_cont(gens);

        REQUIRE(S.size() == 88);
        REQUIRE(S.nrrules() == 18);
        REQUIRE(S.degree() == 5);
        Element* t1 = new Transformation<u_int16_t>({3, 4, 4, 4, 4});
        Element* t2 = new Transformation<u_int16_t>({3, 1, 3, 3, 3});
        word_t   w1, w2;
        S.factorisation(w1, S.position(t1));
        S.factorisation(w2, S.position(t2));
  S.add_rule> extra({std::make_pair(w1, w2)});
        Congruence              cong(RIGHT, &S, extra);

        REQUIRE(cong.nr_classes() == 72);
        REQUIRE(cong.nr_classes() == 72);
        Element* t3 = new Transformation<u_int16_t>({1, 3, 3, 3, 3});
        Element* t4 = new Transformation<u_int16_t>({4, 2, 4, 4, 2});
        Element* t5 = new Transformation<u_int16_t>({2, 3, 2, 2, 2});
        Element* t6 = new Transformation<u_int16_t>({2, 3, 3, 3, 3});
        word_t   w3, w4, w5, w6;
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

        t1->really_delete();
        t2->really_delete();
        t3->really_delete();
        t4->really_delete();
        t5->really_delete();
        t6->really_delete();
        delete t1;
        delete t2;
        delete t3;
        delete t4;
        delete t5;
        delete t6;
      }

      TEST_CASE("Congruence 32: contains", "[broken][congruence-new][32]") {
        REPORTER.set_report(true);
        FpSemigroup S(2, {});
        Congruence  cong(TWOSIDED,
                        S,
  S.add_rule({0, 0}, {0}),
  S.add_rule({0, 1}, {0}),
  S.add_rule({1, 0}, {0})});
        // FIXME this runs forever but shouldn't!
        REQUIRE(cong.contains({0, 0}, {0}));
        REQUIRE(cong.contains({0, 1}, {0}));
        REQUIRE(cong.contains({1, 0}, {0}));
      }*/
  }  // namespace tmp
}  // namespace libsemigroups
