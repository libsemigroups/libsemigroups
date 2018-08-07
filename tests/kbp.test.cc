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

// The purpose of this file is to test the Congruence::KBP class, this is
// achieved by calling kbp.force_kbp() before calculating anything about the
// congruence.

#include <utility> // for std::transform

#include "catch.hpp"
#include "cong-p.h"

namespace libsemigroups {
  namespace congruence {
    namespace kbp {
      constexpr bool        REPORT   = false;
      congruence_type const TWOSIDED = congruence_type::TWOSIDED;
      congruence_type const LEFT     = congruence_type::LEFT;
      congruence_type const RIGHT    = congruence_type::RIGHT;

      using KnuthBendix = fpsemigroup::KnuthBendix;

      TEST_CASE("KBP 01: non-trivial congruence on an infinite fp semigroup",
                "[quick][kbp][01]") {
        REPORTER.set_report(REPORT);
        KnuthBendix kb;
        kb.set_alphabet(3);
        kb.add_rule({0, 1}, {1, 0});
        kb.add_rule({0, 2}, {2, 0});
        kb.add_rule({0, 0}, {0});
        kb.add_rule({0, 2}, {0});
        kb.add_rule({2, 0}, {0});
        kb.add_rule({1, 2}, {2, 1});
        kb.add_rule({1, 1, 1}, {1});
        kb.add_rule({1, 2}, {1});
        kb.add_rule({2, 1}, {1});

        KBP kbp(TWOSIDED, kb);
        kbp.add_pair({0}, {1});

        REQUIRE(kbp.word_to_class_index({0}) == kbp.word_to_class_index({1}));
        REQUIRE(kbp.word_to_class_index({0})
                == kbp.word_to_class_index({1, 0}));
        REQUIRE(kbp.word_to_class_index({0})
                == kbp.word_to_class_index({1, 1}));
        REQUIRE(kbp.word_to_class_index({0})
                == kbp.word_to_class_index({1, 0, 1}));

        REQUIRE(kbp.nr_non_trivial_classes() == 1);
        REQUIRE(kbp.cbegin_non_trivial_classes()->size() == 5);
        REQUIRE(
            std::vector<word_type>(kbp.cbegin_non_trivial_classes()->cbegin(),
                                   kbp.cbegin_non_trivial_classes()->cend())
            == std::vector<word_type>({{0}, {1}, {0, 1}, {1, 1}, {0, 1, 1}}));
      }

      TEST_CASE("KBP 02: non-trivial congruence on an infinite fp semigroup",
                "[quick][kbp][02]") {
        REPORTER.set_report(REPORT);
        KnuthBendix kb;
        kb.set_alphabet(4);
        kb.add_rule({0, 1}, {1, 0});
        kb.add_rule({0, 2}, {2, 0});
        kb.add_rule({0, 0}, {0});
        kb.add_rule({0, 2}, {0});
        kb.add_rule({2, 0}, {0});
        kb.add_rule({1, 2}, {2, 1});
        kb.add_rule({1, 1, 1}, {1});
        kb.add_rule({1, 2}, {1});
        kb.add_rule({2, 1}, {1});
        kb.add_rule({0, 3}, {0});
        kb.add_rule({3, 0}, {0});
        kb.add_rule({1, 3}, {1});
        kb.add_rule({3, 1}, {1});
        kb.add_rule({2, 3}, {2});
        kb.add_rule({3, 2}, {2});

        KBP kbp(TWOSIDED, kb);
        kbp.add_pair({0}, {1});

        REQUIRE(kbp.word_to_class_index({0}) == kbp.word_to_class_index({1}));
        REQUIRE(kbp.word_to_class_index({0})
                == kbp.word_to_class_index({1, 0}));
        REQUIRE(kbp.word_to_class_index({0})
                == kbp.word_to_class_index({1, 1}));
        REQUIRE(kbp.word_to_class_index({0})
                == kbp.word_to_class_index({1, 0, 1}));

        REQUIRE(kbp.nr_non_trivial_classes() == 1);
        REQUIRE(kbp.cbegin_non_trivial_classes()->size() == 5);
        REQUIRE(
            std::vector<word_type>(kbp.cbegin_non_trivial_classes()->cbegin(),
                                   kbp.cbegin_non_trivial_classes()->cend())
            == std::vector<word_type>({{0}, {1}, {0, 1}, {1, 1}, {0, 1, 1}}));
      }

      TEST_CASE("KBP 03: non-trivial congruence on an infinite fp semigroup",
                "[quick][kbp][03]") {
        REPORTER.set_report(REPORT);
        KnuthBendix kb;
        kb.set_alphabet(5);
        kb.add_rule({0, 1}, {0});
        kb.add_rule({1, 0}, {0});
        kb.add_rule({0, 2}, {0});
        kb.add_rule({2, 0}, {0});
        kb.add_rule({0, 3}, {0});
        kb.add_rule({3, 0}, {0});
        kb.add_rule({0, 0}, {0});
        kb.add_rule({1, 1}, {0});
        kb.add_rule({2, 2}, {0});
        kb.add_rule({3, 3}, {0});
        kb.add_rule({1, 2}, {0});
        kb.add_rule({2, 1}, {0});
        kb.add_rule({1, 3}, {0});
        kb.add_rule({3, 1}, {0});
        kb.add_rule({2, 3}, {0});
        kb.add_rule({3, 2}, {0});
        kb.add_rule({4, 0}, {0});
        kb.add_rule({4, 1}, {1});
        kb.add_rule({4, 2}, {2});
        kb.add_rule({4, 3}, {3});
        kb.add_rule({0, 4}, {0});
        kb.add_rule({1, 4}, {1});
        kb.add_rule({2, 4}, {2});
        kb.add_rule({3, 4}, {3});

        KBP kbp(TWOSIDED, kb);
        kbp.add_pair({1}, {2});

        REQUIRE(kbp.word_to_class_index({1}) == kbp.word_to_class_index({2}));

        REQUIRE(kbp.nr_non_trivial_classes() == 1);
        REQUIRE(kbp.cbegin_non_trivial_classes()->size() == 2);
        REQUIRE(
            std::vector<word_type>(kbp.cbegin_non_trivial_classes()->cbegin(),
                                   kbp.cbegin_non_trivial_classes()->cend())
            == std::vector<word_type>({{1}, {2}}));

        REQUIRE(kbp.word_to_class_index({1}) == kbp.word_to_class_index({2}));
      }

      TEST_CASE("KBP 04: non-trivial congruence on an infinite fp semigroup",
                "[quick][kbp][04]") {
        REPORTER.set_report(REPORT);
        KnuthBendix kb;
        kb.set_alphabet(5);
        kb.add_rule({0, 1}, {0});
        kb.add_rule({1, 0}, {0});
        kb.add_rule({0, 2}, {0});
        kb.add_rule({2, 0}, {0});
        kb.add_rule({0, 3}, {0});
        kb.add_rule({3, 0}, {0});
        kb.add_rule({0, 0}, {0});
        kb.add_rule({1, 1}, {0});
        kb.add_rule({2, 2}, {0});
        kb.add_rule({3, 3}, {0});
        kb.add_rule({1, 2}, {0});
        kb.add_rule({2, 1}, {0});
        kb.add_rule({1, 3}, {0});
        kb.add_rule({3, 1}, {0});
        kb.add_rule({2, 3}, {0});
        kb.add_rule({3, 2}, {0});
        kb.add_rule({4, 0}, {0});
        kb.add_rule({4, 1}, {2});
        kb.add_rule({4, 2}, {3});
        kb.add_rule({4, 3}, {1});
        kb.add_rule({0, 4}, {0});
        kb.add_rule({1, 4}, {2});
        kb.add_rule({2, 4}, {3});
        kb.add_rule({3, 4}, {1});

        KBP kbp(TWOSIDED, kb);
        kbp.add_pair({2}, {3});

        REQUIRE(kbp.word_to_class_index({3}) == kbp.word_to_class_index({2}));

        REQUIRE(kbp.nr_non_trivial_classes() == 1);
        REQUIRE(kbp.cbegin_non_trivial_classes()->size() == 3);
        REQUIRE(
            std::vector<word_type>(kbp.cbegin_non_trivial_classes()->cbegin(),
                                   kbp.cbegin_non_trivial_classes()->cend())
            == std::vector<word_type>({{2}, {3}, {1}}));
      }

      TEST_CASE("KBP 05: trivial congruence on a finite fp semigroup",
                "[quick][kbp][05]") {
        REPORTER.set_report(REPORT);
        KnuthBendix kb;
        kb.set_alphabet(2);
        kb.add_rule({0, 0, 1}, {0, 0});
        kb.add_rule({0, 0, 0, 0}, {0, 0});
        kb.add_rule({0, 1, 1, 0}, {0, 0});
        kb.add_rule({0, 1, 1, 1}, {0, 0, 0});
        kb.add_rule({1, 1, 1, 0}, {1, 1, 0});
        kb.add_rule({1, 1, 1, 1}, {1, 1, 1});
        kb.add_rule({0, 1, 0, 0, 0}, {0, 1, 0, 1});
        kb.add_rule({0, 1, 0, 1, 0}, {0, 1, 0, 0});
        kb.add_rule({0, 1, 0, 1, 1}, {0, 1, 0, 1});

        KBP kbp(TWOSIDED, kb);

        REQUIRE(kbp.nr_classes() == 27);
        REQUIRE(kbp.word_to_class_index({0}) == 0);

        REQUIRE(kbp.word_to_class_index({0, 0, 0, 0}) == 1);
        REQUIRE(kbp.word_to_class_index({0}) == 0);
        REQUIRE(kbp.word_to_class_index({1, 0, 1}) == 2);
        REQUIRE(kbp.word_to_class_index({0, 1, 1, 0}) == 1);

        REQUIRE(kbp.nr_non_trivial_classes() == 0);
        REQUIRE(kbp.cbegin_non_trivial_classes()
                == kbp.cend_non_trivial_classes());
      }

      TEST_CASE("KBP 06: universal congruence on a finite fp semigroup",
                      "[quick][kbp][06]") {
        REPORTER.set_report(REPORT);

        KnuthBendix kb;
        kb.set_alphabet(2);
        kb.add_rule({0, 0, 1}, {0, 0});
        kb.add_rule({0, 0, 0, 0}, {0, 0});
        kb.add_rule({0, 1, 1, 0}, {0, 0});
        kb.add_rule({0, 1, 1, 1}, {0, 0, 0});
        kb.add_rule({1, 1, 1, 0}, {1, 1, 0});
        kb.add_rule({1, 1, 1, 1}, {1, 1, 1});
        kb.add_rule({0, 1, 0, 0, 0}, {0, 1, 0, 1});
        kb.add_rule({0, 1, 0, 1, 0}, {0, 1, 0, 0});
        kb.add_rule({0, 1, 0, 1, 1}, {0, 1, 0, 1});

        KBP kbp(TWOSIDED, kb);
        kbp.add_pair({0}, {1});
        kbp.add_pair({0, 0}, {0});

        REQUIRE(kbp.nr_classes() == 1);

        REQUIRE(kbp.cbegin_non_trivial_classes()->size() == 27);
        REQUIRE(kb.size() == 27);
        REQUIRE(
            std::vector<word_type>(kbp.cbegin_non_trivial_classes()->cbegin(),
                                   kbp.cbegin_non_trivial_classes()->cend())
            == std::vector<word_type>({{0},
                                       {1},
                                       {0, 0},
                                       {0, 1},
                                       {1, 0},
                                       {1, 1},
                                       {0, 0, 0},
                                       {1, 0, 0},
                                       {0, 1, 0},
                                       {1, 0, 1},
                                       {0, 1, 1},
                                       {1, 1, 0},
                                       {1, 1, 1},
                                       {1, 0, 0, 0},
                                       {0, 1, 0, 0},
                                       {1, 1, 0, 0},
                                       {1, 0, 1, 0},
                                       {0, 1, 0, 1},
                                       {1, 1, 0, 1},
                                       {1, 0, 1, 1},
                                       {1, 1, 0, 0, 0},
                                       {1, 0, 1, 0, 0},
                                       {1, 1, 0, 1, 0},
                                       {1, 0, 1, 0, 1},
                                       {1, 1, 0, 1, 1},
                                       {1, 1, 0, 1, 0, 0},
                                       {1, 1, 0, 1, 0, 1}}));

        REQUIRE(kbp.nr_non_trivial_classes() == 1);

        REQUIRE(kbp.word_to_class_index({0, 0, 0, 0}) == 0);
        REQUIRE(kbp.word_to_class_index({0}) == 0);
        REQUIRE(kbp.word_to_class_index({1, 0, 1}) == 0);
        REQUIRE(kbp.word_to_class_index({0, 1, 1, 0}) == 0);
      }

      TEST_CASE(
          "KBP 06: left congruence with even chunks on a finite fp semigroup",
          "[quick][left][kbp][06]") {
        REPORTER.set_report(REPORT);
        KnuthBendix kb;
        kb.set_alphabet(2);
        kb.add_rule({0, 0, 1}, {0, 0});
        kb.add_rule({0, 0, 0, 0}, {0, 0});
        kb.add_rule({0, 1, 1, 0}, {0, 0});
        kb.add_rule({0, 1, 1, 1}, {0, 0, 0});
        kb.add_rule({1, 1, 1, 0}, {1, 1, 0});
        kb.add_rule({1, 1, 1, 1}, {1, 1, 1});
        kb.add_rule({0, 1, 0, 0, 0}, {0, 1, 0, 1});
        kb.add_rule({0, 1, 0, 1, 0}, {0, 1, 0, 0});
        kb.add_rule({0, 1, 0, 1, 1}, {0, 1, 0, 1});

        KBP kbp(LEFT, kb);
        kbp.add_pair({0}, {1});
        kbp.add_pair({0, 0}, {0});

        REQUIRE(kbp.nr_non_trivial_classes() == 6);

        std::vector<size_t> v(kbp.nr_non_trivial_classes(), 0);
        std::transform(kbp.cbegin_non_trivial_classes(),
                       kbp.cend_non_trivial_classes(),
                       v.begin(),
                       std::mem_fn(&std::vector<word_type>::size));
        std::sort(v.begin(), v.end());
        REQUIRE(v == std::vector<size_t>({4, 4, 4, 5, 5, 5}));

        REQUIRE(kbp.word_to_class_index({0})
                == kbp.word_to_class_index({0, 0, 0}));
        REQUIRE(kbp.word_to_class_index({1, 0, 1, 1})
                == kbp.word_to_class_index({1, 0, 1, 0, 1}));
        REQUIRE(kbp.word_to_class_index({1, 1, 0, 0})
                != kbp.word_to_class_index({0, 1}));
        REQUIRE(kbp.word_to_class_index({1, 0, 1, 0})
                != kbp.word_to_class_index({1, 1, 0, 1, 0, 1}));

        REQUIRE(kbp.word_to_class_index({1, 0, 1}) == 1);
        REQUIRE(kbp.word_to_class_index({0}) == 0);
        REQUIRE(kbp.word_to_class_index({0, 1, 1, 0}) == 0);

        REQUIRE(kbp.nr_classes() == 6);
      }

      /*






            TEST_CASE("KBP 07: finite group, Chapter 11, Theorem 1.9, H, q = 4
      in NR",
                      "[quick][kbp][07]") {
      REPORTER.set_report(REPORT);
              std::vector<relation_type> rels
                  = {relation_type({0, 0}, {0}),
                     relation_type({0, 1}, {1}),
                     relation_type({1, 0}, {1}),
                     relation_type({0, 2}, {2}),
                     relation_type({2, 0}, {2}),
                     relation_type({0, 3}, {3}),
                     relation_type({3, 0}, {3}),
                     relation_type({2, 3}, {0}),
                     relation_type({3, 2}, {0}),
                     relation_type({1, 1}, {0}),
                     relation_type({2, 2, 2, 2}, {0}),
                     relation_type({1, 2, 1, 3, 1, 3, 1, 2, 1, 3, 1, 2}, {0})};

              std::vector<relation_type> extra = {};
              KBP kbp(TWOSIDED, kb);



              REQUIRE(kbp.nr_classes() == 120);
            }

            TEST_CASE("KBP 08: infinite fp-semigroup, rght congruence with one "
                      "non-trivial class",
                      "[quick][kbp][08]") {
      REPORTER.set_report(REPORT);
              std::vector<relation_type> rels = {
                  relation_type({1, 1, 1, 1, 1, 1, 1}, {1}),
                  relation_type({2, 2, 2, 2, 2}, {2}),
                  relation_type({1, 2, 2, 1, 0}, {1, 2, 2, 1}),
                  relation_type({1, 2, 2, 1, 2}, {1, 2, 2, 1}),
                  relation_type({1, 1, 2, 1, 2, 0}, {1, 1, 2, 1, 2}),
                  relation_type({1, 1, 2, 1, 2, 1}, {1, 1, 2, 1, 2}),
              };

              std::vector<relation_type> extra
                  = {relation_type({1, 2, 2, 1}, {1, 1, 2, 1, 2})};
              KBP kbp("right", 3, rels, extra);


              // Generating pair
              REQUIRE(kbp.word_to_class_index({1, 2, 2, 1})
                      == kbp.word_to_class_index({1, 1, 2, 1, 2}));

              Partition<word_type>* ntc = kbp.nontrivial_classes();
              REQUIRE(ntc->size() == 1);
              REQUIRE(ntc->at(0)->size() == 2);
              delete ntc;
            }

            TEST_CASE("KBP 09: finite fp-semigroup, dihedral group of order 6",
                      "[quick][kbp][finite][09]") {
      REPORTER.set_report(REPORT);
              std::vector<relation_type> rels  = {relation_type({0, 0}, {0}),
                                                 relation_type({0, 1}, {1}),
                                                 relation_type({1, 0}, {1}),
                                                 relation_type({0, 2}, {2}),
                                                 relation_type({2, 0}, {2}),
                                                 relation_type({0, 3}, {3}),
                                                 relation_type({3, 0}, {3}),
                                                 relation_type({0, 4}, {4}),
                                                 relation_type({4, 0}, {4}),
                                                 relation_type({1, 2}, {0}),
                                                 relation_type({2, 1}, {0}),
                                                 relation_type({3, 4}, {0}),
                                                 relation_type({4, 3}, {0}),
                                                 relation_type({2, 2}, {0}),
                                                 relation_type({1, 4, 2, 3, 3},
      {0}), relation_type({4, 4, 4}, {0})}; std::vector<relation_type> extra =
      {};

              KBP kbp(TWOSIDED, kb);



              REQUIRE(kbp.nr_classes() == 6);
              REQUIRE(kbp.word_to_class_index({1}) ==
      kbp.word_to_class_index({2}));
            }

            TEST_CASE("KBP 10: finite fp-semigroup, size 16",
                      "[quick][kbp][finite][10]") {
      REPORTER.set_report(REPORT);
              std::vector<relation_type> rels
                  = {relation_type({3}, {2}),
                     relation_type({0, 3}, {0, 2}),
                     relation_type({1, 1}, {1}),
                     relation_type({1, 3}, {1, 2}),
                     relation_type({2, 1}, {2}),
                     relation_type({2, 2}, {2}),
                     relation_type({2, 3}, {2}),
                     relation_type({0, 0, 0}, {0}),
                     relation_type({0, 0, 1}, {1}),
                     relation_type({0, 0, 2}, {2}),
                     relation_type({0, 1, 2}, {1, 2}),
                     relation_type({1, 0, 0}, {1}),
                     relation_type({1, 0, 2}, {0, 2}),
                     relation_type({2, 0, 0}, {2}),
                     relation_type({0, 1, 0, 1}, {1, 0, 1}),
                     relation_type({0, 2, 0, 2}, {2, 0, 2}),
                     relation_type({1, 0, 1, 0}, {1, 0, 1}),
                     relation_type({1, 2, 0, 1}, {1, 0, 1}),
                     relation_type({1, 2, 0, 2}, {2, 0, 2}),
                     relation_type({2, 0, 1, 0}, {2, 0, 1}),
                     relation_type({2, 0, 2, 0}, {2, 0, 2})};
              std::vector<relation_type> extra = {};

              KBP kbp(TWOSIDED, kb);



              REQUIRE(kbp.nr_classes() == 16);
              REQUIRE(kbp.word_to_class_index({2}) ==
      kbp.word_to_class_index({3}));
            }

            TEST_CASE("KBP 11: finite fp-semigroup, size 16",
                      "[quick][kbp][finite][11]") {
      REPORTER.set_report(REPORT);
              std::vector<relation_type> rels
                  = {relation_type({2}, {1}),
                     relation_type({4}, {3}),
                     relation_type({5}, {0}),
                     relation_type({6}, {3}),
                     relation_type({7}, {1}),
                     relation_type({8}, {3}),
                     relation_type({9}, {3}),
                     relation_type({10}, {0}),
                     relation_type({0, 2}, {0, 1}),
                     relation_type({0, 4}, {0, 3}),
                     relation_type({0, 5}, {0, 0}),
                     relation_type({0, 6}, {0, 3}),
                     relation_type({0, 7}, {0, 1}),
                     relation_type({0, 8}, {0, 3}),
                     relation_type({0, 9}, {0, 3}),
                     relation_type({0, 10}, {0, 0}),
                     relation_type({1, 1}, {1}),
                     relation_type({1, 2}, {1}),
                     relation_type({1, 4}, {1, 3}),
                     relation_type({1, 5}, {1, 0}),
                     relation_type({1, 6}, {1, 3}),
                     relation_type({1, 7}, {1}),
                     relation_type({1, 8}, {1, 3}),
                     relation_type({1, 9}, {1, 3}),
                     relation_type({1, 10}, {1, 0}),
                     relation_type({3, 1}, {3}),
                     relation_type({3, 2}, {3}),
                     relation_type({3, 3}, {3}),
                     relation_type({3, 4}, {3}),
                     relation_type({3, 5}, {3, 0}),
                     relation_type({3, 6}, {3}),
                     relation_type({3, 7}, {3}),
                     relation_type({3, 8}, {3}),
                     relation_type({3, 9}, {3}),
                     relation_type({3, 10}, {3, 0}),
                     relation_type({0, 0, 0}, {0}),
                     relation_type({0, 0, 1}, {1}),
                     relation_type({0, 0, 3}, {3}),
                     relation_type({0, 1, 3}, {1, 3}),
                     relation_type({1, 0, 0}, {1}),
                     relation_type({1, 0, 3}, {0, 3}),
                     relation_type({3, 0, 0}, {3}),
                     relation_type({0, 1, 0, 1}, {1, 0, 1}),
                     relation_type({0, 3, 0, 3}, {3, 0, 3}),
                     relation_type({1, 0, 1, 0}, {1, 0, 1}),
                     relation_type({1, 3, 0, 1}, {1, 0, 1}),
                     relation_type({1, 3, 0, 3}, {3, 0, 3}),
                     relation_type({3, 0, 1, 0}, {3, 0, 1}),
                     relation_type({3, 0, 3, 0}, {3, 0, 3})};
              std::vector<relation_type> extra = {};

              KBP kbp(TWOSIDED, kb);



              REQUIRE(kbp.nr_classes() == 16);
              REQUIRE(kbp.word_to_class_index({0}) ==
      kbp.word_to_class_index({5})); REQUIRE(kbp.word_to_class_index({0})
                      == kbp.word_to_class_index({10}));
              REQUIRE(kbp.word_to_class_index({1}) ==
      kbp.word_to_class_index({2})); REQUIRE(kbp.word_to_class_index({1}) ==
      kbp.word_to_class_index({7})); REQUIRE(kbp.word_to_class_index({3}) ==
      kbp.word_to_class_index({4})); REQUIRE(kbp.word_to_class_index({3}) ==
      kbp.word_to_class_index({6})); REQUIRE(kbp.word_to_class_index({3}) ==
      kbp.word_to_class_index({8})); REQUIRE(kbp.word_to_class_index({3}) ==
      kbp.word_to_class_index({9}));
            }

            TEST_CASE("KBP 12: Infinite fp semigroup with infinite classes",
                      "[quick][kbp][12]") {
      REPORTER.set_report(REPORT);
              std::vector<relation_type> rels
                  = {relation_type({0, 0, 0}, {0}),
      relation_type({0, 1}, {1, 0})};
              std::vector<relation_type> extra = {relation_type({0}, {0, 0})};
              KBP kbp(TWOSIDED, kb);



              word_type x
                  = {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
      1}; word_type y = {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
      1, 1, 1, 1};

              REQUIRE(kbp.test_equals(x, y));

              REQUIRE(!kbp.is_done());
            }*/
    }  // namespace kbp
  }    // namespace congruence
}  // namespace libsemigroups
