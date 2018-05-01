//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2017 James D. Mitchell
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
// achieved by calling cong.force_kbp() before calculating anything about the
// congruence.

#include <utility>

#include "../src/cong.h"
#include "catch.hpp"

#define KBP_REPORT false

using namespace libsemigroups;

TEST_CASE("KBP 01: for an infinite fp semigroup",
          "[quick][congruence][kbp][fpsemigroup][01]") {
  std::vector<relation_t> rels  = {relation_t({0, 1}, {1, 0}),
                                  relation_t({0, 2}, {2, 0}),
                                  relation_t({0, 0}, {0}),
                                  relation_t({0, 2}, {0}),
                                  relation_t({2, 0}, {0}),
                                  relation_t({1, 2}, {2, 1}),
                                  relation_t({1, 1, 1}, {1}),
                                  relation_t({1, 2}, {1}),
                                  relation_t({2, 1}, {1})};
  std::vector<relation_t> extra = {{{0}, {1}}};
  Congruence              cong("twosided", 3, rels, extra);
  cong.set_report(KBP_REPORT);
  cong.force_kbp();

  REQUIRE(cong.word_to_class_index({0}) == cong.word_to_class_index({1}));
  REQUIRE(cong.word_to_class_index({0}) == cong.word_to_class_index({1, 0}));
  REQUIRE(cong.word_to_class_index({0}) == cong.word_to_class_index({1, 1}));
  REQUIRE(cong.word_to_class_index({0}) == cong.word_to_class_index({1, 0, 1}));

  Partition<word_t>* ntc = cong.nontrivial_classes();
  REQUIRE(ntc->size() == 1);
  REQUIRE(ntc->at(0)->size() == 5);
  REQUIRE(*(ntc->at(0, 0)) == word_t({0}));
  delete ntc;
}

TEST_CASE("KBP 02: for an infinite fp semigroup",
          "[quick][congruence][kbp][fpsemigroup][02]") {
  std::vector<relation_t> rels  = {relation_t({0, 1}, {1, 0}),
                                  relation_t({0, 2}, {2, 0}),
                                  relation_t({0, 0}, {0}),
                                  relation_t({0, 2}, {0}),
                                  relation_t({2, 0}, {0}),
                                  relation_t({1, 2}, {2, 1}),
                                  relation_t({1, 1, 1}, {1}),
                                  relation_t({1, 2}, {1}),
                                  relation_t({2, 1}, {1}),
                                  relation_t({0, 3}, {0}),
                                  relation_t({3, 0}, {0}),
                                  relation_t({1, 3}, {1}),
                                  relation_t({3, 1}, {1}),
                                  relation_t({2, 3}, {2}),
                                  relation_t({3, 2}, {2})};
  std::vector<relation_t> extra = {{{0}, {1}}};
  Congruence              cong("twosided", 4, rels, extra);
  cong.force_kbp();
  cong.set_report(KBP_REPORT);

  REQUIRE(cong.word_to_class_index({0}) == cong.word_to_class_index({1}));
  REQUIRE(cong.word_to_class_index({0}) == cong.word_to_class_index({1, 0}));
  REQUIRE(cong.word_to_class_index({0}) == cong.word_to_class_index({1, 1}));
  REQUIRE(cong.word_to_class_index({0}) == cong.word_to_class_index({1, 0, 1}));
}

TEST_CASE("KBP 03: for an infinite fp semigroup",
          "[quick][congruence][kbp][fpsemigroup][03]") {
  std::vector<relation_t> rels
      = {relation_t({0, 1}, {0}), relation_t({1, 0}, {0}),
         relation_t({0, 2}, {0}), relation_t({2, 0}, {0}),
         relation_t({0, 3}, {0}), relation_t({3, 0}, {0}),
         relation_t({0, 0}, {0}), relation_t({1, 1}, {0}),
         relation_t({2, 2}, {0}), relation_t({3, 3}, {0}),
         relation_t({1, 2}, {0}), relation_t({2, 1}, {0}),
         relation_t({1, 3}, {0}), relation_t({3, 1}, {0}),
         relation_t({2, 3}, {0}), relation_t({3, 2}, {0}),
         relation_t({4, 0}, {0}), relation_t({4, 1}, {1}),
         relation_t({4, 2}, {2}), relation_t({4, 3}, {3}),
         relation_t({0, 4}, {0}), relation_t({1, 4}, {1}),
         relation_t({2, 4}, {2}), relation_t({3, 4}, {3})};
  std::vector<relation_t> extra = {{{1}, {2}}};
  Congruence              cong("twosided", 5, rels, extra);
  cong.force_kbp();
  cong.set_report(KBP_REPORT);

  REQUIRE(cong.word_to_class_index({1}) == cong.word_to_class_index({2}));

  Partition<word_t>* ntc = cong.nontrivial_classes();
  REQUIRE(ntc->size() == 1);
  REQUIRE(ntc->at(0)->size() == 2);
  delete ntc;

  cong.force_kbp();  // clear data
  REQUIRE(cong.word_to_class_index({1}) == cong.word_to_class_index({2}));
}

TEST_CASE("KBP 04: for an infinite fp semigroup",
          "[quick][congruence][kbp][fpsemigroup][04]") {
  std::vector<relation_t> rels
      = {relation_t({0, 1}, {0}), relation_t({1, 0}, {0}),
         relation_t({0, 2}, {0}), relation_t({2, 0}, {0}),
         relation_t({0, 3}, {0}), relation_t({3, 0}, {0}),
         relation_t({0, 0}, {0}), relation_t({1, 1}, {0}),
         relation_t({2, 2}, {0}), relation_t({3, 3}, {0}),
         relation_t({1, 2}, {0}), relation_t({2, 1}, {0}),
         relation_t({1, 3}, {0}), relation_t({3, 1}, {0}),
         relation_t({2, 3}, {0}), relation_t({3, 2}, {0}),
         relation_t({4, 0}, {0}), relation_t({4, 1}, {2}),
         relation_t({4, 2}, {3}), relation_t({4, 3}, {1}),
         relation_t({0, 4}, {0}), relation_t({1, 4}, {2}),
         relation_t({2, 4}, {3}), relation_t({3, 4}, {1})};
  std::vector<relation_t> extra = {{{2}, {3}}};
  Congruence              cong("twosided", 5, rels, extra);
  cong.force_kbp();
  cong.set_report(KBP_REPORT);

  REQUIRE(cong.word_to_class_index({3}) == cong.word_to_class_index({2}));

  Partition<word_t>* ntc = cong.nontrivial_classes();
  REQUIRE(ntc->size() == 1);
  REQUIRE(ntc->at(0)->size() == 3);
  delete ntc;
}

TEST_CASE("KBP 05: trivial congruence on a finite fp semigroup",
          "[quick][congruence][kbp][fpsemigroup][05]") {
  std::vector<relation_t> rels  = {relation_t({0, 0, 1}, {0, 0}),
                                  relation_t({0, 0, 0, 0}, {0, 0}),
                                  relation_t({0, 1, 1, 0}, {0, 0}),
                                  relation_t({0, 1, 1, 1}, {0, 0, 0}),
                                  relation_t({1, 1, 1, 0}, {1, 1, 0}),
                                  relation_t({1, 1, 1, 1}, {1, 1, 1}),
                                  relation_t({0, 1, 0, 0, 0}, {0, 1, 0, 1}),
                                  relation_t({0, 1, 0, 1, 0}, {0, 1, 0, 0}),
                                  relation_t({0, 1, 0, 1, 1}, {0, 1, 0, 1})};
  std::vector<relation_t> extra = {};
  Congruence              cong("twosided", 2, rels, extra);
  cong.force_kbp();
  cong.set_report(KBP_REPORT);

  REQUIRE(cong.nr_classes() == 27);
  REQUIRE(cong.word_to_class_index({0}) == 0);

  Partition<word_t>* ntc = cong.nontrivial_classes();
  REQUIRE(ntc->size() == 0);
  delete ntc;

  REQUIRE(cong.word_to_class_index({0, 0, 0, 0}) == 1);
  REQUIRE(cong.word_to_class_index({0}) == 0);
  REQUIRE(cong.word_to_class_index({1, 0, 1}) == 2);
  REQUIRE(cong.word_to_class_index({0, 1, 1, 0}) == 1);

  ntc = cong.nontrivial_classes();
  REQUIRE(ntc->size() == 0);
  delete ntc;
}

TEST_CASE("KBP 06: universal congruence on a finite fp semigroup",
          "[quick][congruence][kbp][fpsemigroup][universal][06]") {
  std::vector<relation_t> rels = {relation_t({0, 0, 1}, {0, 0}),
                                  relation_t({0, 0, 0, 0}, {0, 0}),
                                  relation_t({0, 1, 1, 0}, {0, 0}),
                                  relation_t({0, 1, 1, 1}, {0, 0, 0}),
                                  relation_t({1, 1, 1, 0}, {1, 1, 0}),
                                  relation_t({1, 1, 1, 1}, {1, 1, 1}),
                                  relation_t({0, 1, 0, 0, 0}, {0, 1, 0, 1}),
                                  relation_t({0, 1, 0, 1, 0}, {0, 1, 0, 0}),
                                  relation_t({0, 1, 0, 1, 1}, {0, 1, 0, 1})};
  std::vector<relation_t> extra
      = {relation_t({0}, {1}), relation_t({0, 0}, {0})};
  Congruence cong("twosided", 2, rels, extra);
  cong.force_kbp();
  cong.set_report(KBP_REPORT);

  REQUIRE(cong.nr_classes() == 1);
  REQUIRE(cong.word_to_class_index({0}) == 0);

  Partition<word_t>* ntc = cong.nontrivial_classes();
  REQUIRE(ntc->size() == 1);
  REQUIRE(ntc->at(0)->size() == 27);
  delete ntc;

  REQUIRE(cong.word_to_class_index({0, 0, 0, 0}) == 0);
  REQUIRE(cong.word_to_class_index({0}) == 0);
  REQUIRE(cong.word_to_class_index({1, 0, 1}) == 0);
  REQUIRE(cong.word_to_class_index({0, 1, 1, 0}) == 0);

  ntc = cong.nontrivial_classes();
  REQUIRE(ntc->size() == 1);
  REQUIRE(ntc->at(0)->size() == 27);
  delete ntc;
}

TEST_CASE("KBP 06: left congruence with even chunks on a finite fp semigroup",
          "[quick][left][congruence][kbp][fpsemigroup][06]") {
  std::vector<relation_t> rels = {relation_t({0, 0, 1}, {0, 0}),
                                  relation_t({0, 0, 0, 0}, {0, 0}),
                                  relation_t({0, 1, 1, 0}, {0, 0}),
                                  relation_t({0, 1, 1, 1}, {0, 0, 0}),
                                  relation_t({1, 1, 1, 0}, {1, 1, 0}),
                                  relation_t({1, 1, 1, 1}, {1, 1, 1}),
                                  relation_t({0, 1, 0, 0, 0}, {0, 1, 0, 1}),
                                  relation_t({0, 1, 0, 1, 0}, {0, 1, 0, 0}),
                                  relation_t({0, 1, 0, 1, 1}, {0, 1, 0, 1})};
  std::vector<relation_t> extra
      = {relation_t({0}, {1}), relation_t({0, 0}, {0})};
  Congruence cong("left", 2, rels, extra);
  cong.force_kbp();
  cong.set_report(KBP_REPORT);

  Partition<word_t>* ntc = cong.nontrivial_classes();
  REQUIRE(ntc->size() == 6);
  REQUIRE(ntc->at(0)->size() == 5);
  REQUIRE(ntc->at(1)->size() == 5);
  REQUIRE(ntc->at(2)->size() == 4);
  REQUIRE(ntc->at(3)->size() == 5);
  REQUIRE(ntc->at(4)->size() == 4);
  REQUIRE(ntc->at(5)->size() == 4);
  delete ntc;

  REQUIRE(cong.word_to_class_index({0}) == cong.word_to_class_index({0, 0, 0}));
  REQUIRE(cong.word_to_class_index({1, 0, 1, 1})
          == cong.word_to_class_index({1, 0, 1, 0, 1}));
  REQUIRE(cong.word_to_class_index({1, 1, 0, 0})
          != cong.word_to_class_index({0, 1}));
  REQUIRE(cong.word_to_class_index({1, 0, 1, 0})
          != cong.word_to_class_index({1, 1, 0, 1, 0, 1}));

  REQUIRE(cong.word_to_class_index({1, 0, 1}) == 1);
  REQUIRE(cong.word_to_class_index({0}) == 0);
  REQUIRE(cong.word_to_class_index({0, 1, 1, 0}) == 0);

  REQUIRE(cong.nr_classes() == 6);
}

TEST_CASE("KBP 07: finite group, Chapter 11, Theorem 1.9, H, q = 4 in NR",
          "[quick][congruence][kbp][fpsemigroup][07]") {
  std::vector<relation_t> rels
      = {relation_t({0, 0}, {0}),
         relation_t({0, 1}, {1}),
         relation_t({1, 0}, {1}),
         relation_t({0, 2}, {2}),
         relation_t({2, 0}, {2}),
         relation_t({0, 3}, {3}),
         relation_t({3, 0}, {3}),
         relation_t({2, 3}, {0}),
         relation_t({3, 2}, {0}),
         relation_t({1, 1}, {0}),
         relation_t({2, 2, 2, 2}, {0}),
         relation_t({1, 2, 1, 3, 1, 3, 1, 2, 1, 3, 1, 2}, {0})};

  std::vector<relation_t> extra = {};
  Congruence              cong("twosided", 3, rels, extra);
  cong.force_kbp();
  cong.set_report(KBP_REPORT);

  REQUIRE(cong.nr_classes() == 120);
}

TEST_CASE(
    "KBP 08: infinite fp-semigroup, rght congruence with one non-trivial class",
    "[quick][congruence][kbp][fpsemigroup][08]") {
  std::vector<relation_t> rels = {
      relation_t({1, 1, 1, 1, 1, 1, 1}, {1}),
      relation_t({2, 2, 2, 2, 2}, {2}),
      relation_t({1, 2, 2, 1, 0}, {1, 2, 2, 1}),
      relation_t({1, 2, 2, 1, 2}, {1, 2, 2, 1}),
      relation_t({1, 1, 2, 1, 2, 0}, {1, 1, 2, 1, 2}),
      relation_t({1, 1, 2, 1, 2, 1}, {1, 1, 2, 1, 2}),
  };

  std::vector<relation_t> extra = {relation_t({1, 2, 2, 1}, {1, 1, 2, 1, 2})};
  Congruence              cong("right", 3, rels, extra);
  cong.force_kbp();
  cong.set_report(KBP_REPORT);

  // Generating pair
  REQUIRE(cong.word_to_class_index({1, 2, 2, 1})
          == cong.word_to_class_index({1, 1, 2, 1, 2}));

  Partition<word_t>* ntc = cong.nontrivial_classes();
  REQUIRE(ntc->size() == 1);
  REQUIRE(ntc->at(0)->size() == 2);
  delete ntc;
}

TEST_CASE("KBP 09: finite fp-semigroup, dihedral group of order 6",
          "[quick][fpsemigroup][kbp][finite][09]") {
  std::vector<relation_t> rels  = {relation_t({0, 0}, {0}),
                                  relation_t({0, 1}, {1}),
                                  relation_t({1, 0}, {1}),
                                  relation_t({0, 2}, {2}),
                                  relation_t({2, 0}, {2}),
                                  relation_t({0, 3}, {3}),
                                  relation_t({3, 0}, {3}),
                                  relation_t({0, 4}, {4}),
                                  relation_t({4, 0}, {4}),
                                  relation_t({1, 2}, {0}),
                                  relation_t({2, 1}, {0}),
                                  relation_t({3, 4}, {0}),
                                  relation_t({4, 3}, {0}),
                                  relation_t({2, 2}, {0}),
                                  relation_t({1, 4, 2, 3, 3}, {0}),
                                  relation_t({4, 4, 4}, {0})};
  std::vector<relation_t> extra = {};

  Congruence cong("twosided", 5, rels, extra);
  cong.force_kbp();
  cong.set_report(KBP_REPORT);

  REQUIRE(cong.nr_classes() == 6);
  REQUIRE(cong.word_to_class_index({1}) == cong.word_to_class_index({2}));
}

TEST_CASE("KBP 10: finite fp-semigroup, size 16",
          "[quick][fpsemigroup][kbp][finite][10]") {
  std::vector<relation_t> rels  = {relation_t({3}, {2}),
                                  relation_t({0, 3}, {0, 2}),
                                  relation_t({1, 1}, {1}),
                                  relation_t({1, 3}, {1, 2}),
                                  relation_t({2, 1}, {2}),
                                  relation_t({2, 2}, {2}),
                                  relation_t({2, 3}, {2}),
                                  relation_t({0, 0, 0}, {0}),
                                  relation_t({0, 0, 1}, {1}),
                                  relation_t({0, 0, 2}, {2}),
                                  relation_t({0, 1, 2}, {1, 2}),
                                  relation_t({1, 0, 0}, {1}),
                                  relation_t({1, 0, 2}, {0, 2}),
                                  relation_t({2, 0, 0}, {2}),
                                  relation_t({0, 1, 0, 1}, {1, 0, 1}),
                                  relation_t({0, 2, 0, 2}, {2, 0, 2}),
                                  relation_t({1, 0, 1, 0}, {1, 0, 1}),
                                  relation_t({1, 2, 0, 1}, {1, 0, 1}),
                                  relation_t({1, 2, 0, 2}, {2, 0, 2}),
                                  relation_t({2, 0, 1, 0}, {2, 0, 1}),
                                  relation_t({2, 0, 2, 0}, {2, 0, 2})};
  std::vector<relation_t> extra = {};

  Congruence cong("twosided", 4, rels, extra);
  cong.force_kbp();
  cong.set_report(KBP_REPORT);

  REQUIRE(cong.nr_classes() == 16);
  REQUIRE(cong.word_to_class_index({2}) == cong.word_to_class_index({3}));
}

TEST_CASE("KBP 11: finite fp-semigroup, size 16",
          "[quick][fpsemigroup][kbp][finite][11]") {
  std::vector<relation_t> rels  = {relation_t({2}, {1}),
                                  relation_t({4}, {3}),
                                  relation_t({5}, {0}),
                                  relation_t({6}, {3}),
                                  relation_t({7}, {1}),
                                  relation_t({8}, {3}),
                                  relation_t({9}, {3}),
                                  relation_t({10}, {0}),
                                  relation_t({0, 2}, {0, 1}),
                                  relation_t({0, 4}, {0, 3}),
                                  relation_t({0, 5}, {0, 0}),
                                  relation_t({0, 6}, {0, 3}),
                                  relation_t({0, 7}, {0, 1}),
                                  relation_t({0, 8}, {0, 3}),
                                  relation_t({0, 9}, {0, 3}),
                                  relation_t({0, 10}, {0, 0}),
                                  relation_t({1, 1}, {1}),
                                  relation_t({1, 2}, {1}),
                                  relation_t({1, 4}, {1, 3}),
                                  relation_t({1, 5}, {1, 0}),
                                  relation_t({1, 6}, {1, 3}),
                                  relation_t({1, 7}, {1}),
                                  relation_t({1, 8}, {1, 3}),
                                  relation_t({1, 9}, {1, 3}),
                                  relation_t({1, 10}, {1, 0}),
                                  relation_t({3, 1}, {3}),
                                  relation_t({3, 2}, {3}),
                                  relation_t({3, 3}, {3}),
                                  relation_t({3, 4}, {3}),
                                  relation_t({3, 5}, {3, 0}),
                                  relation_t({3, 6}, {3}),
                                  relation_t({3, 7}, {3}),
                                  relation_t({3, 8}, {3}),
                                  relation_t({3, 9}, {3}),
                                  relation_t({3, 10}, {3, 0}),
                                  relation_t({0, 0, 0}, {0}),
                                  relation_t({0, 0, 1}, {1}),
                                  relation_t({0, 0, 3}, {3}),
                                  relation_t({0, 1, 3}, {1, 3}),
                                  relation_t({1, 0, 0}, {1}),
                                  relation_t({1, 0, 3}, {0, 3}),
                                  relation_t({3, 0, 0}, {3}),
                                  relation_t({0, 1, 0, 1}, {1, 0, 1}),
                                  relation_t({0, 3, 0, 3}, {3, 0, 3}),
                                  relation_t({1, 0, 1, 0}, {1, 0, 1}),
                                  relation_t({1, 3, 0, 1}, {1, 0, 1}),
                                  relation_t({1, 3, 0, 3}, {3, 0, 3}),
                                  relation_t({3, 0, 1, 0}, {3, 0, 1}),
                                  relation_t({3, 0, 3, 0}, {3, 0, 3})};
  std::vector<relation_t> extra = {};

  Congruence cong("twosided", 11, rels, extra);
  cong.force_kbp();
  cong.set_report(KBP_REPORT);

  REQUIRE(cong.nr_classes() == 16);
  REQUIRE(cong.word_to_class_index({0}) == cong.word_to_class_index({5}));
  REQUIRE(cong.word_to_class_index({0}) == cong.word_to_class_index({10}));
  REQUIRE(cong.word_to_class_index({1}) == cong.word_to_class_index({2}));
  REQUIRE(cong.word_to_class_index({1}) == cong.word_to_class_index({7}));
  REQUIRE(cong.word_to_class_index({3}) == cong.word_to_class_index({4}));
  REQUIRE(cong.word_to_class_index({3}) == cong.word_to_class_index({6}));
  REQUIRE(cong.word_to_class_index({3}) == cong.word_to_class_index({8}));
  REQUIRE(cong.word_to_class_index({3}) == cong.word_to_class_index({9}));
}

TEST_CASE("KBP 12: Infinite fp semigroup with infinite classes",
          "[quick][fpsemigroup][kbp][12]") {
  std::vector<relation_t> rels
      = {relation_t({0, 0, 0}, {0}), relation_t({0, 1}, {1, 0})};
  std::vector<relation_t> extra = {relation_t({0}, {0, 0})};
  Congruence              cong("twosided", 2, rels, extra);
  cong.force_kbp();
  cong.set_report(KBP_REPORT);

  word_t x = {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
  word_t y = {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

  REQUIRE(cong.test_equals(x, y));

  REQUIRE(!cong.is_done());
}
