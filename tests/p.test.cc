//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2016 Michael Torpey
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

// The purpose of this file is to test the Congruence::TC class, this is
// achieved by calling cong->p() before calculating anything about the
// congruence.

#include <utility>

#include "../src/cong.h"
#include "catch.hpp"

#define P_REPORT false

using namespace libsemigroups;

TEST_CASE("P 01: congruence on finite semigroup",
          "[quick][congruence][p][finite][01]") {
  std::vector<Element*> gens = {new Transformation<u_int16_t>({1, 3, 4, 2, 3}),
                                new Transformation<u_int16_t>({3, 2, 1, 3, 3})};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(P_REPORT);
  really_delete_cont(gens);

  // The following lines are intentionally commented out so that we can check
  // that P does not enumerate the semigroup, they remain to remind us of the
  // size and number of rules of the semigroups.
  // REQUIRE(S.size(false) == 88);
  // REQUIRE(S.nrrules(false) == 18);

  std::vector<relation_t> extra(
      {relation_t({0, 1, 0, 0, 0, 1, 1, 0, 0}, {1, 0, 0, 0, 1})});
  Congruence cong("twosided", &S, extra);
  cong.set_report(P_REPORT);
  cong.force_p();

  REQUIRE(cong.word_to_class_index({0, 0, 0, 1})
          == cong.word_to_class_index({0, 0, 1, 0, 0}));
  REQUIRE(cong.is_done());
  REQUIRE(!S.is_begun());
  REQUIRE(!S.is_done());

  REQUIRE(cong.nr_classes() == 21);
  REQUIRE(cong.nr_classes() == 21);
  REQUIRE(S.is_done());  // nr_classes requires S.size();
}

TEST_CASE("P 02: left congruence on finite semigroup",
          "[quick][congruence][p][finite][02]") {
  std::vector<Element*> gens = {new Transformation<u_int16_t>({1, 3, 4, 2, 3}),
                                new Transformation<u_int16_t>({3, 2, 1, 3, 3})};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(P_REPORT);
  really_delete_cont(gens);

  // The following lines are intentionally commented out so that we can check
  // that P does not enumerate the semigroup, they remain to remind us of the
  // size and number of rules of the semigroups.
  // REQUIRE(S.size(false) == 88);
  // REQUIRE(S.nrrules(false) == 18);

  std::vector<relation_t> extra(
      {relation_t({0, 1, 0, 0, 0, 1, 1, 0, 0}, {1, 0, 0, 0, 1})});
  Congruence cong("left", &S, extra);
  cong.set_report(P_REPORT);
  cong.force_p();

  REQUIRE(cong.word_to_class_index({0, 0, 0, 1}) == 0);
  REQUIRE(cong.word_to_class_index({0, 0, 1, 0, 0}) == 1);
  REQUIRE(!S.is_begun());
  REQUIRE(!S.is_done());

  REQUIRE(cong.nr_classes() == 69);
  REQUIRE(cong.nr_classes() == 69);
  REQUIRE(S.is_done());  // nr_classes requires S.size();
}

TEST_CASE("P 03: right congruence on finite semigroup",
          "[quick][congruence][p][finite][03]") {
  std::vector<Element*> gens = {new Transformation<u_int16_t>({1, 3, 4, 2, 3}),
                                new Transformation<u_int16_t>({3, 2, 1, 3, 3})};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(P_REPORT);
  really_delete_cont(gens);

  // The following lines are intentionally commented out so that we can check
  // that P does not enumerate the semigroup, they remain to remind us of the
  // size and number of rules of the semigroups.
  // REQUIRE(S.size(false) == 88);
  // REQUIRE(S.nrrules(false) == 18);

  std::vector<relation_t> extra(
      {relation_t({0, 1, 0, 0, 0, 1, 1, 0, 0}, {1, 0, 0, 0, 1})});
  Congruence cong("right", &S, extra);
  cong.set_report(P_REPORT);
  cong.force_p();

  REQUIRE(cong.word_to_class_index({0, 0, 0, 1}) == 4);
  REQUIRE(cong.word_to_class_index({0, 0, 1, 0, 0}) == 5);
  REQUIRE(!S.is_begun());
  REQUIRE(!S.is_done());

  REQUIRE(cong.nr_classes() == 72);
  REQUIRE(cong.nr_classes() == 72);
  REQUIRE(S.is_done());  // nr_classes requires S.size();
}

TEST_CASE("P 04: trivial congruence on finite semigroup",
          "[quick][congruence][p][finite][04]") {
  std::vector<Element*> gens
      = {new PartialPerm<u_int16_t>({0, 1, 3, 4}, {1, 4, 0, 3}, 5),
         new PartialPerm<u_int16_t>({0, 1, 2}, {0, 4, 3}, 5)};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(P_REPORT);
  really_delete_cont(gens);

  // The following lines are intentionally commented out so that we can check
  // that P does not enumerate the semigroup, they remain to remind us of the
  // size and number of rules of the semigroups.
  // REQUIRE(S.size(false) == 53);
  // REQUIRE(S.nrrules(false) == 20);

  std::vector<relation_t> extra({});
  Congruence              cong("twosided", &S, extra);
  cong.set_report(P_REPORT);
  cong.force_p();

  // Class indices are assigned starting at 0
  REQUIRE(cong.word_to_class_index({0, 0, 0, 1}) == 0);
  REQUIRE(cong.word_to_class_index({0, 0, 1, 0, 0}) == 1);
  REQUIRE(cong.word_to_class_index({0, 0, 1, 0, 1}) == 2);
  REQUIRE(cong.word_to_class_index({1, 1, 0, 1}) == 3);
  REQUIRE(cong.word_to_class_index({1, 1, 0, 0}) == 3);
  REQUIRE(cong.word_to_class_index({1, 0, 0, 1, 0, 0, 0}) == 4);
  REQUIRE(cong.word_to_class_index({0, 0, 0, 0, 0, 0, 0, 1}) == 0);
  REQUIRE(cong.word_to_class_index({0, 0})
          != cong.word_to_class_index({0, 0, 0}));
  REQUIRE(cong.word_to_class_index({1, 1})
          == cong.word_to_class_index({1, 1, 1}));
  REQUIRE(!S.is_begun());
  REQUIRE(!S.is_done());

  REQUIRE(cong.nr_classes() == 53);
  REQUIRE(cong.nr_classes() == 53);
  REQUIRE(S.is_done());  // nr_classes requires S.size();
}

TEST_CASE("P 05: trivial left congruence on finite semigroup",
          "[quick][congruence][p][finite][05]") {
  std::vector<Element*> gens
      = {new PartialPerm<u_int16_t>({0, 1, 3, 4}, {1, 4, 0, 3}, 5),
         new PartialPerm<u_int16_t>({0, 1, 2}, {0, 4, 3}, 5)};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(P_REPORT);
  really_delete_cont(gens);

  // The following lines are intentionally commented out so that we can check
  // that P does not enumerate the semigroup, they remain to remind us of the
  // size and number of rules of the semigroups.
  // REQUIRE(S.size(false) == 53);
  // REQUIRE(S.nrrules(false) == 20);

  std::vector<relation_t> extra({});
  Congruence              cong("left", &S, extra);
  cong.set_report(P_REPORT);
  cong.force_p();

  // Class indices are assigned starting at 0
  REQUIRE(cong.word_to_class_index({0, 0, 0, 1}) == 0);
  REQUIRE(cong.word_to_class_index({0, 0, 1, 0, 0}) == 1);
  REQUIRE(cong.word_to_class_index({0, 0, 1, 0, 1}) == 2);
  REQUIRE(cong.word_to_class_index({1, 1, 0, 1}) == 3);
  REQUIRE(cong.word_to_class_index({1, 1, 0, 0}) == 3);
  REQUIRE(cong.word_to_class_index({1, 0, 0, 1, 0, 0, 0}) == 4);
  REQUIRE(cong.word_to_class_index({0, 0, 0, 0, 0, 0, 0, 1}) == 0);
  REQUIRE(cong.word_to_class_index({0, 0})
          != cong.word_to_class_index({0, 0, 0}));
  REQUIRE(cong.word_to_class_index({1, 1})
          == cong.word_to_class_index({1, 1, 1}));
  REQUIRE(!S.is_begun());
  REQUIRE(!S.is_done());

  REQUIRE(cong.nr_classes() == 53);
  REQUIRE(cong.nr_classes() == 53);
  REQUIRE(S.is_done());  // nr_classes requires S.size();
}

TEST_CASE("P 06: trivial right congruence on finite semigroup",
          "[quick][congruence][p][finite][06]") {
  std::vector<Element*> gens
      = {new PartialPerm<u_int16_t>({0, 1, 3, 4}, {1, 4, 0, 3}, 5),
         new PartialPerm<u_int16_t>({0, 1, 2}, {0, 4, 3}, 5)};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(P_REPORT);
  really_delete_cont(gens);

  // The following lines are intentionally commented out so that we can check
  // that P does not enumerate the semigroup, they remain to remind us of the
  // size and number of rules of the semigroups.
  // REQUIRE(S.size(false) == 53);
  // REQUIRE(S.nrrules(false) == 20);

  std::vector<relation_t> extra({});
  Congruence              cong("right", &S, extra);
  cong.set_report(P_REPORT);
  cong.force_p();

  // Class indices are assigned starting at 0
  REQUIRE(cong.word_to_class_index({0, 0, 0, 1}) == 0);
  REQUIRE(cong.word_to_class_index({0, 0, 1, 0, 0}) == 1);
  REQUIRE(cong.word_to_class_index({0, 0, 1, 0, 1}) == 2);
  REQUIRE(cong.word_to_class_index({1, 1, 0, 1}) == 3);
  REQUIRE(cong.word_to_class_index({1, 1, 0, 0}) == 3);
  REQUIRE(cong.word_to_class_index({1, 0, 0, 1, 0, 0, 0}) == 4);
  REQUIRE(cong.word_to_class_index({0, 0, 0, 0, 0, 0, 0, 1}) == 0);
  REQUIRE(cong.word_to_class_index({0, 0})
          != cong.word_to_class_index({0, 0, 0}));
  REQUIRE(cong.word_to_class_index({1, 1})
          == cong.word_to_class_index({1, 1, 1}));
  REQUIRE(!S.is_begun());
  REQUIRE(!S.is_done());

  REQUIRE(cong.nr_classes() == 53);
  REQUIRE(cong.nr_classes() == 53);
  REQUIRE(S.is_done());  // nr_classes requires S.size();
}

TEST_CASE("P 07: universal congruence on finite semigroup",
          "[quick][congruence][p][finite][07]") {
  std::vector<Element*> gens
      = {new PartialPerm<u_int16_t>({0, 1, 3}, {4, 1, 0}, 5),
         new PartialPerm<u_int16_t>({0, 1, 2, 3, 4}, {0, 2, 4, 1, 3}, 5)};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(P_REPORT);
  really_delete_cont(gens);

  // The following lines are intentionally commented out so that we can check
  // that P does not enumerate the semigroup, they remain to remind us of the
  // size and number of rules of the semigroups.
  // REQUIRE(S.size(false) == 142);
  // REQUIRE(S.nrrules(false) == 32);

  std::vector<relation_t> extra({relation_t({1}, {0, 0, 0, 1, 0})});
  Congruence              cong("twosided", &S, extra);
  cong.set_report(P_REPORT);
  cong.force_p();

  // Class indices are assigned starting at 0
  REQUIRE(cong.word_to_class_index({0, 0, 0, 1}) == 0);
  REQUIRE(cong.word_to_class_index({0, 0, 1, 0, 0}) == 0);
  REQUIRE(cong.word_to_class_index({0, 0, 1, 0, 1}) == 0);
  REQUIRE(cong.word_to_class_index({1, 1, 0, 1}) == 0);
  REQUIRE(cong.word_to_class_index({1, 1, 0, 0}) == 0);
  REQUIRE(cong.word_to_class_index({1, 0, 0, 1, 0, 0, 0}) == 0);
  REQUIRE(cong.word_to_class_index({0, 0, 0, 0, 0, 0, 0, 1}) == 0);
  REQUIRE(cong.word_to_class_index({0, 0})
          == cong.word_to_class_index({0, 0, 0}));
  REQUIRE(cong.word_to_class_index({1, 1})
          == cong.word_to_class_index({1, 1, 1}));
  REQUIRE(!S.is_begun());
  REQUIRE(!S.is_done());

  REQUIRE(cong.nr_classes() == 1);
  REQUIRE(cong.nr_classes() == 1);
  REQUIRE(S.is_done());  // nr_classes requires S.size();
}

TEST_CASE("P 08: congruence on big finite semigroup",
          "[extreme][congruence][p][finite][08]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({7, 3, 5, 3, 4, 2, 7, 7}),
         new Transformation<u_int16_t>({1, 2, 4, 4, 7, 3, 0, 7}),
         new Transformation<u_int16_t>({0, 6, 4, 2, 2, 6, 6, 4}),
         new Transformation<u_int16_t>({3, 6, 3, 4, 0, 6, 0, 7})};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(P_REPORT);
  really_delete_cont(gens);

  // The following lines are intentionally commented out so that we can check
  // that P does not enumerate the semigroup, they remain to remind us of the
  // size and number of rules of the semigroups.
  // REQUIRE(S.size(false) == 11804);
  // REQUIRE(S.nrrules(false) == 2460);

  std::vector<relation_t> extra(
      {relation_t({0, 3, 2, 1, 3, 2, 2}, {3, 2, 2, 1, 3, 3})});
  Congruence cong("twosided", &S, extra);
  cong.set_report(P_REPORT);
  cong.force_p();

  REQUIRE(cong.word_to_class_index({0, 0, 0, 1}) == 0);
  REQUIRE(cong.word_to_class_index({0, 0, 1, 0, 0}) == 0);
  REQUIRE(cong.word_to_class_index({0, 0, 1, 0, 1}) == 0);
  REQUIRE(cong.word_to_class_index({1, 1, 0, 1}) == 0);
  REQUIRE(cong.word_to_class_index({1, 1, 0, 0}) == 1);
  REQUIRE(cong.word_to_class_index({0, 0, 3}) == 2);

  REQUIRE(cong.word_to_class_index({1, 2, 1, 3, 3, 2, 1, 2})
          == cong.word_to_class_index({2, 1, 3, 3, 2, 1, 0}));
  REQUIRE(cong.word_to_class_index({0, 3, 1, 1, 1, 3, 2, 2, 1, 0})
          == cong.word_to_class_index({0, 3, 2, 2, 1}));
  REQUIRE(cong.word_to_class_index({0, 3, 2, 1, 3, 3, 3})
          != cong.word_to_class_index({0, 0, 3}));
  REQUIRE(cong.word_to_class_index({1, 1, 0})
          != cong.word_to_class_index({1, 3, 3, 2, 2, 1, 0}));

  REQUIRE(!S.is_begun());
  REQUIRE(!S.is_done());

  REQUIRE(cong.nr_classes() == 525);
  REQUIRE(cong.nr_classes() == 525);
  REQUIRE(S.is_done());  // nr_classes requires S.size();
}

TEST_CASE("P 09: congruence on big finite semigroup",
          "[standard][congruence][p][finite][09]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({7, 3, 5, 3, 4, 2, 7, 7}),
         new Transformation<u_int16_t>({1, 2, 4, 4, 7, 3, 0, 7}),
         new Transformation<u_int16_t>({0, 6, 4, 2, 2, 6, 6, 4}),
         new Transformation<u_int16_t>({3, 6, 3, 4, 0, 6, 0, 7})};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(P_REPORT);
  really_delete_cont(gens);

  // The following lines are intentionally commented out so that we can check
  // that P does not enumerate the semigroup, they remain to remind us of the
  // size and number of rules of the semigroups.
  // REQUIRE(S.size(false) == 11804);
  // REQUIRE(S.nrrules(false) == 2460);

  std::vector<relation_t> extra(
      {relation_t({1, 3, 0, 1, 2, 2, 0, 2}, {1, 0, 0, 1, 3, 1})});
  Congruence cong("twosided", &S, extra);
  cong.set_report(P_REPORT);
  cong.force_p();

  REQUIRE(cong.word_to_class_index({0, 0, 0, 1}) == 1);
  REQUIRE(cong.word_to_class_index({0, 0, 3}) == 2);
  REQUIRE(cong.word_to_class_index({0, 1, 1, 2, 3}) == 0);

  REQUIRE(cong.word_to_class_index({0, 1, 1, 2, 3})
          == cong.word_to_class_index({1, 0, 3, 3, 3, 2, 0}));
  REQUIRE(cong.word_to_class_index({3, 0, 2, 0, 2, 0, 2})
          == cong.word_to_class_index({1, 2, 3, 1, 2}));
  REQUIRE(cong.word_to_class_index({0, 3, 2, 1, 3, 3, 3})
          != cong.word_to_class_index({0, 0, 3}));
  REQUIRE(cong.word_to_class_index({1, 1, 0})
          != cong.word_to_class_index({1, 3, 3, 2, 2, 1, 0}));

  REQUIRE(!S.is_begun());
  REQUIRE(!S.is_done());

  REQUIRE(cong.nr_classes() == 9597);
  REQUIRE(cong.nr_classes() == 9597);
  REQUIRE(S.is_done());  // nr_classes requires S.size();
}

TEST_CASE("P 10: left congruence on big finite semigroup",
          "[quick][congruence][p][finite][10]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({7, 3, 5, 3, 4, 2, 7, 7}),
         new Transformation<u_int16_t>({1, 2, 4, 4, 7, 3, 0, 7}),
         new Transformation<u_int16_t>({0, 6, 4, 2, 2, 6, 6, 4}),
         new Transformation<u_int16_t>({3, 6, 3, 4, 0, 6, 0, 7})};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(P_REPORT);
  really_delete_cont(gens);

  // The following lines are intentionally commented out so that we can check
  // that P does not enumerate the semigroup, they remain to remind us of the
  // size and number of rules of the semigroups.
  // REQUIRE(S.size(false) == 11804);
  // REQUIRE(S.nrrules(false) == 2460);
  std::vector<relation_t> extra(
      {relation_t({0, 3, 2, 1, 3, 2, 2}, {3, 2, 2, 1, 3, 3})});
  Congruence cong("left", &S, extra);
  cong.set_report(P_REPORT);
  cong.force_p();

  REQUIRE(cong.word_to_class_index({1, 1, 0, 3}) == 1);
  REQUIRE(cong.word_to_class_index({0, 0, 3}) == 2);
  REQUIRE(cong.word_to_class_index({2, 2, 0, 1}) == 0);

  REQUIRE(cong.word_to_class_index({1, 1, 3, 2, 2, 1, 3, 1, 3, 3})
          == cong.word_to_class_index({2, 2, 0, 1}));
  REQUIRE(cong.word_to_class_index({2, 1, 3, 1, 2, 2, 1, 3, 3})
          == cong.word_to_class_index({1, 2, 1, 2, 1, 3, 2, 2, 1, 3, 3}));
  REQUIRE(cong.word_to_class_index({1, 1, 0, 3})
          != cong.word_to_class_index({1, 0, 3, 2, 0, 2, 0, 3, 2, 2, 1}));
  REQUIRE(cong.word_to_class_index({1, 3, 2, 1, 3, 1, 3, 2, 2, 1, 3, 3, 3})
          != cong.word_to_class_index({3, 1, 0, 2, 0, 3, 1}));

  REQUIRE(!S.is_begun());
  REQUIRE(!S.is_done());

  REQUIRE(cong.nr_classes() == 7449);
  REQUIRE(cong.nr_classes() == 7449);
  REQUIRE(S.is_done());  // nr_classes requires S.size();
}
