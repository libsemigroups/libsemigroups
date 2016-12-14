//
// Semigroups++ - C/C++ library for computing with semigroups and monoids
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

#include "../cong.h"
#include "catch.hpp"

#define KBP_REPORT false

#include <utility>

using namespace libsemigroups;

template <typename T> static inline void really_delete_cont(T cont) {
  for (Element* x : cont) {
    x->really_delete();
    delete x;
  }
}

static inline void really_delete_partition(Congruence::partition_t part) {
  for (auto& cont : part) {
    for (Element const* x : cont) {
      const_cast<Element*>(x)->really_delete();
      delete x;
    }
  }
}

TEST_CASE("KBP 01: for an infinite fp semigroup",
          "[quick][congruence][kbp][fpsemigroup]") {
  std::vector<relation_t> rels = {relation_t({0, 1}, {1, 0}),
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

  Congruence::partition_t nontrivial_classes = cong.nontrivial_classes();
  REQUIRE(nontrivial_classes.size() == 1);
  REQUIRE(nontrivial_classes[0].size() == 5);
  // REQUIRE(nontrivial_classes[0][0] == word_t({0}));
  really_delete_partition(nontrivial_classes);
}

TEST_CASE("KBP 02: for an infinite fp semigroup",
          "[quick][congruence][kbp][fpsemigroup]") {
  std::vector<relation_t> rels = {relation_t({0, 1}, {1, 0}),
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
          "[quick][congruence][kbp][fpsemigroup]") {
  std::vector<relation_t> rels = {
      relation_t({0, 1}, {0}), relation_t({1, 0}, {0}),
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

  Congruence::partition_t nontrivial_classes = cong.nontrivial_classes();
  REQUIRE(nontrivial_classes.size() == 1);
  REQUIRE(nontrivial_classes[0].size() == 2);
  really_delete_partition(nontrivial_classes);
}

TEST_CASE("KBP 04: for an infinite fp semigroup",
          "[quick][congruence][kbp][fpsemigroup]") {
  std::vector<relation_t> rels = {
      relation_t({0, 1}, {0}), relation_t({1, 0}, {0}),
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

  Congruence::partition_t nontrivial_classes = cong.nontrivial_classes();
  REQUIRE(nontrivial_classes.size() == 1);
  REQUIRE(nontrivial_classes[0].size() == 3);
  really_delete_partition(nontrivial_classes);
}

TEST_CASE("KBP 05: trivial congruence on a finite fp semigroup",
          "[quick][congruence][kbp][fpsemigroup]") {
  std::vector<relation_t> rels = {relation_t({0, 0, 1}, {0, 0}),
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

  Congruence::partition_t nontrivial_classes = cong.nontrivial_classes();
  REQUIRE(nontrivial_classes.size() == 0);
  really_delete_partition(nontrivial_classes);

  REQUIRE(cong.word_to_class_index({0, 0, 0, 0}) == 1);
  REQUIRE(cong.word_to_class_index({0}) == 0);
  REQUIRE(cong.word_to_class_index({1, 0, 1}) == 2);
  REQUIRE(cong.word_to_class_index({0, 1, 1, 0}) == 1);

  nontrivial_classes = cong.nontrivial_classes();
  REQUIRE(nontrivial_classes.size() == 0);
  really_delete_partition(nontrivial_classes);
}

TEST_CASE("KBP 06: universal congruence on a finite fp semigroup",
          "[quick][congruence][kbp][fpsemigroup][universal]") {
  std::vector<relation_t> rels = {relation_t({0, 0, 1}, {0, 0}),
                                  relation_t({0, 0, 0, 0}, {0, 0}),
                                  relation_t({0, 1, 1, 0}, {0, 0}),
                                  relation_t({0, 1, 1, 1}, {0, 0, 0}),
                                  relation_t({1, 1, 1, 0}, {1, 1, 0}),
                                  relation_t({1, 1, 1, 1}, {1, 1, 1}),
                                  relation_t({0, 1, 0, 0, 0}, {0, 1, 0, 1}),
                                  relation_t({0, 1, 0, 1, 0}, {0, 1, 0, 0}),
                                  relation_t({0, 1, 0, 1, 1}, {0, 1, 0, 1})};
  std::vector<relation_t> extra = {relation_t({0}, {1}),
                                   relation_t({0, 0}, {0})};
  Congruence cong("twosided", 2, rels, extra);
  cong.force_kbp();
  cong.set_report(KBP_REPORT);

  REQUIRE(cong.nr_classes() == 1);
  REQUIRE(cong.word_to_class_index({0}) == 0);

  Congruence::partition_t nontrivial_classes = cong.nontrivial_classes();
  REQUIRE(nontrivial_classes.size() == 1);
  REQUIRE(nontrivial_classes[0].size() == 27);
  really_delete_partition(nontrivial_classes);

  REQUIRE(cong.word_to_class_index({0, 0, 0, 0}) == 0);
  REQUIRE(cong.word_to_class_index({0}) == 0);
  REQUIRE(cong.word_to_class_index({1, 0, 1}) == 0);
  REQUIRE(cong.word_to_class_index({0, 1, 1, 0}) == 0);

  nontrivial_classes = cong.nontrivial_classes();
  REQUIRE(nontrivial_classes.size() == 1);
  REQUIRE(nontrivial_classes[0].size() == 27);
  really_delete_partition(nontrivial_classes);
}

TEST_CASE("KBP 06: left congruence with even chunks on a finite fp semigroup",
          "[quick][left][congruence][kbp][fpsemigroup]") {
  std::vector<relation_t> rels = {relation_t({0, 0, 1}, {0, 0}),
                                  relation_t({0, 0, 0, 0}, {0, 0}),
                                  relation_t({0, 1, 1, 0}, {0, 0}),
                                  relation_t({0, 1, 1, 1}, {0, 0, 0}),
                                  relation_t({1, 1, 1, 0}, {1, 1, 0}),
                                  relation_t({1, 1, 1, 1}, {1, 1, 1}),
                                  relation_t({0, 1, 0, 0, 0}, {0, 1, 0, 1}),
                                  relation_t({0, 1, 0, 1, 0}, {0, 1, 0, 0}),
                                  relation_t({0, 1, 0, 1, 1}, {0, 1, 0, 1})};
  std::vector<relation_t> extra = {relation_t({0}, {1}),
                                   relation_t({0, 0}, {0})};
  Congruence cong("left", 2, rels, extra);
  cong.force_kbp();
  cong.set_report(KBP_REPORT);

  Congruence::partition_t nontrivial_classes = cong.nontrivial_classes();
  REQUIRE(nontrivial_classes.size() == 6);
  REQUIRE(nontrivial_classes[0].size() == 5);
  REQUIRE(nontrivial_classes[1].size() == 5);
  REQUIRE(nontrivial_classes[2].size() == 4);
  REQUIRE(nontrivial_classes[3].size() == 5);
  REQUIRE(nontrivial_classes[4].size() == 4);
  REQUIRE(nontrivial_classes[5].size() == 4);
  really_delete_partition(nontrivial_classes);

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
          "[quick][congruence][kbp][fpsemigroup]") {
  std::vector<relation_t> rels = {
      relation_t({0, 0}, {0}),
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
  Congruence cong("twosided", 3, rels, extra);
  cong.force_kbp();
  cong.set_report(KBP_REPORT);

  REQUIRE(cong.nr_classes() == 120);
}

TEST_CASE("KBP 08: infinite fp-semigroup, rght congruence with one non-trivial class",
          "[quick][congruence][kbp][fpsemigroup]") {
  std::vector<relation_t> rels = {
      relation_t({1, 1, 1, 1, 1, 1, 1}, {1}),
      relation_t({2, 2, 2, 2, 2}, {2}),
      relation_t({1, 2, 2, 1, 0}, {1, 2, 2, 1}),
      relation_t({1, 2, 2, 1, 2}, {1, 2, 2, 1}),
      relation_t({1, 1, 2, 1, 2, 0}, {1, 1, 2, 1, 2}),
      relation_t({1, 1, 2, 1, 2, 1}, {1, 1, 2, 1, 2}),
  };

  std::vector<relation_t> extra = {relation_t({1, 2, 2, 1}, {1, 1, 2, 1, 2})};
  Congruence cong("right", 3, rels, extra);
  cong.force_kbp();
  cong.set_report(KBP_REPORT);

  // Generating pair
  REQUIRE(cong.word_to_class_index({1, 2, 2, 1}) ==
          cong.word_to_class_index({1, 1, 2, 1, 2}));

  Congruence::partition_t nontrivial_classes = cong.nontrivial_classes();
  REQUIRE(nontrivial_classes.size() == 1);
  REQUIRE(nontrivial_classes[0].size() == 2);
  really_delete_partition(nontrivial_classes);
}
