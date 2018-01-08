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

#include "../src/semigroups.h"
#include "catch.hpp"

#define SEMIGROUPS_REPORT false

using namespace libsemigroups;

static inline size_t evaluate_reduct(Semigroup<>& S, word_t const& word) {
  letter_t out = S.letter_to_pos(word[0]);
  for (auto it = word.cbegin() + 1; it < word.cend(); ++it) {
    out = S.right(out, *it);
  }
  return out;
}

static inline void test_idempotent(Semigroup<>& S, Element* x) {
  REQUIRE(S.is_idempotent(S.position(x)));
  Element* y = x->really_copy();
  y->redefine(x, x);
  REQUIRE(*x == *y);
  REQUIRE(S.fast_product(S.position(x), S.position(x)) == S.position(x));
  y->really_delete();
  delete y;
}
TEST_CASE("Semigroup 26: cbegin_idempotents/cend, is_idempotent [2 threads]",
          "[standard][semigroup][finite][multithread][26]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({1, 2, 3, 4, 5, 6, 0}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5, 6}),
         new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5, 0})};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(SEMIGROUPS_REPORT);
  S.set_max_threads(2);

  size_t nr = 0;

  for (auto it = S.cbegin_idempotents(); it < S.cend_idempotents(); it++) {
    test_idempotent(S, *it);
    nr++;
  }
  REQUIRE(nr == S.nridempotents());
  REQUIRE(nr == 6322);

  nr = 0;
  for (auto it = S.cbegin_idempotents(); it < S.cend_idempotents(); it++) {
    test_idempotent(S, *it);
    nr++;
  }
  REQUIRE(nr == S.nridempotents());
  REQUIRE(nr == 6322);

  really_delete_cont(gens);
}

TEST_CASE("Semigroup 27: is_done, is_begun", "[quick][semigroup][finite][27]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}),
         new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(SEMIGROUPS_REPORT);

  REQUIRE(!S.is_begun());
  REQUIRE(!S.is_done());

  S.set_batch_size(1024);
  S.enumerate(10);
  REQUIRE(S.is_begun());
  REQUIRE(!S.is_done());

  S.enumerate(8000);
  REQUIRE(S.is_begun());
  REQUIRE(S.is_done());

  really_delete_cont(gens);
}

TEST_CASE("Semigroup 28: current_position", "[quick][semigroup][finite][28]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}),
         new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(SEMIGROUPS_REPORT);
  S.set_report(SEMIGROUPS_REPORT);

  REQUIRE(S.current_position(gens[0]) == 0);
  REQUIRE(S.current_position(gens[1]) == 1);
  REQUIRE(S.current_position(gens[2]) == 2);
  REQUIRE(S.current_position(gens[3]) == 3);
  REQUIRE(S.current_position(gens[4]) == 4);

  S.set_batch_size(1024);
  S.enumerate(1024);

  REQUIRE(S.current_size() == 1029);
  REQUIRE(S.current_nrrules() == 74);
  REQUIRE(S.current_max_word_length() == 7);
  REQUIRE(S.current_position(S.at(1024)) == 1024);

  Element* x = new Transformation<u_int16_t>({5, 1, 5, 5, 2, 5});
  REQUIRE(S.current_position(x) == 1028);
  REQUIRE(S.current_size() == 1029);
  REQUIRE(S.current_nrrules() == 74);
  REQUIRE(S.current_max_word_length() == 7);
  x->really_delete();
  delete x;

  x = new Transformation<u_int16_t>({5, 1, 5, 5, 2, 5, 6});
  REQUIRE(S.current_position(x) == Semigroup<>::UNDEFINED);
  REQUIRE(S.current_size() == 1029);
  REQUIRE(S.current_nrrules() == 74);
  REQUIRE(S.current_max_word_length() == 7);
  x->really_delete();
  delete x;

  x = new Transformation<u_int16_t>({5, 4, 5, 1, 0, 5});
  REQUIRE(S.current_position(x) == Semigroup<>::UNDEFINED);
  REQUIRE(S.current_size() == 1029);
  REQUIRE(S.current_nrrules() == 74);
  REQUIRE(S.current_max_word_length() == 7);
  REQUIRE(S.position(x) == 1029);
  x->really_delete();
  delete x;

  really_delete_cont(gens);
}

TEST_CASE("Semigroup 29: sorted_position, sorted_at",
          "[quick][semigroup][finite][29]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}),
         new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(SEMIGROUPS_REPORT);

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

  REQUIRE(S.is_done());

  REQUIRE(S.sorted_position(S.at(1024)) == 6810);
  REQUIRE(S.position_to_sorted_position(1024) == 6810);

  REQUIRE(*S.sorted_at(6810) == *S.at(1024));
  REQUIRE(S.sorted_at(6810) == S.at(1024));

  Element* x = new Transformation<u_int16_t>({5, 1, 5, 5, 2, 5});
  REQUIRE(S.sorted_position(x) == 6908);
  REQUIRE(S.position_to_sorted_position(S.position(x)) == 6908);
  REQUIRE(*S.sorted_at(6908) == *x);
  REQUIRE(S.sorted_at(6908) == S.at(S.position(x)));
  x->really_delete();
  delete x;

  x = new Transformation<u_int16_t>({5, 5, 5, 1, 5, 5, 6});
  REQUIRE(S.sorted_position(x) == Semigroup<>::UNDEFINED);
  x->really_delete();
  delete x;

  // REQUIRE(S.sorted_at(100000) == nullptr);
  // REQUIRE(S.at(100000) == nullptr);
  REQUIRE(S.position_to_sorted_position(100000) == Semigroup<>::UNDEFINED);

  really_delete_cont(gens);
}

TEST_CASE("Semigroup 30: right/left Cayley graph",
          "[quick][semigroup][finite][30]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}),
         new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(SEMIGROUPS_REPORT);

  REQUIRE(S.right(0, 0) == 0);
  REQUIRE(S.left(0, 0) == 0);

  Element* tmp = new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5});
  for (auto it = S.cbegin(); it < S.cend(); ++it) {
    for (size_t i = 0; i < 5; ++i) {
      tmp->redefine(*it, S.gens(i));
      REQUIRE(S.position(tmp) == S.right(S.position(*it), i));
      tmp->redefine(S.gens(i), *it);
      REQUIRE(S.position(tmp) == S.left(S.position(*it), i));
    }
  }
  tmp->really_delete();
  delete tmp;
  // TODO(JDM) more tests
  really_delete_cont(gens);
}

TEST_CASE("Semigroup 31: iterator", "[quick][semigroup][finite][31]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}),
         new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};
  Semigroup<> S = Semigroup<>(gens);
  really_delete_cont(gens);
  S.set_report(SEMIGROUPS_REPORT);

  REQUIRE(S.current_size() == 5);
  size_t size = S.current_size();
  for (auto it = S.cbegin(); it < S.cend(); it++) {
    size--;
    REQUIRE(S.test_membership(*it));
  }
  REQUIRE(size == 0);

  for (auto it = S.cbegin(); it < S.cend(); ++it) {
    size++;
    REQUIRE(S.test_membership(*it));
  }
  REQUIRE(size == S.current_size());
  REQUIRE(5 == S.current_size());

  S.set_batch_size(1024);
  S.enumerate(1000);
  REQUIRE(S.current_size() < 7776);

  size = S.current_size();
  for (auto it = S.cbegin(); it < S.cend(); it++) {
    size--;
    REQUIRE(S.test_membership(*it));
  }
  REQUIRE(size == 0);

  for (auto it = S.cbegin(); it < S.cend(); ++it) {
    size++;
    REQUIRE(S.test_membership(*it));
  }
  REQUIRE(size == S.current_size());
  REQUIRE(S.current_size() < 7776);

  REQUIRE(S.size() == 7776);
  size = S.size();
  for (auto it = S.cbegin(); it < S.cend(); it++) {
    size--;
    REQUIRE(S.test_membership(*it));
  }
  REQUIRE(size == 0);

  for (auto it = S.cbegin(); it < S.cend(); ++it) {
    size++;
    REQUIRE(S.test_membership(*it));
  }
  REQUIRE(size == S.size());
}

TEST_CASE("Semigroup 66: reverse iterator", "[quick][semigroup][finite][66]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}),
         new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};
  Semigroup<> S = Semigroup<>(gens);
  really_delete_cont(gens);
  S.set_report(SEMIGROUPS_REPORT);

  REQUIRE(S.current_size() == 5);
  size_t size = S.current_size();
  for (auto it = S.crbegin(); it < S.crend(); it++) {
    size--;
    REQUIRE(S.test_membership(*it));
  }
  REQUIRE(size == 0);

  for (auto it = S.crbegin(); it < S.crend(); ++it) {
    size++;
    REQUIRE(S.test_membership(*it));
  }
  REQUIRE(size == S.current_size());
  REQUIRE(5 == S.current_size());

  S.set_batch_size(1024);
  S.enumerate(1000);
  REQUIRE(S.current_size() < 7776);

  size = S.current_size();
  for (auto it = S.crbegin(); it < S.crend(); it++) {
    size--;
    REQUIRE(S.test_membership(*it));
  }
  REQUIRE(size == 0);

  for (auto it = S.crbegin(); it < S.crend(); ++it) {
    size++;
    REQUIRE(S.test_membership(*it));
  }
  REQUIRE(size == S.current_size());
  REQUIRE(S.current_size() < 7776);

  REQUIRE(S.size() == 7776);
  size = S.size();
  for (auto it = S.crbegin(); it < S.crend(); it++) {
    size--;
    REQUIRE(S.test_membership(*it));
  }
  REQUIRE(size == 0);

  for (auto it = S.crbegin(); it < S.crend(); ++it) {
    size++;
    REQUIRE(S.test_membership(*it));
  }
  REQUIRE(size == S.size());
}

TEST_CASE("Semigroup 67: iterator arithmetic",
          "[quick][semigroup][finite][67]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}),
         new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};
  Semigroup<> S = Semigroup<>(gens);
  really_delete_cont(gens);
  S.set_report(SEMIGROUPS_REPORT);

  REQUIRE(S.size() == 7776);
  auto it = S.cbegin();
  // The next line should not compile (and does not).
  // (*it)->really_delete();

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
}

TEST_CASE("Semigroup 68: iterator sorted", "[quick][semigroup][finite][68]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}),
         new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};
  Semigroup<> S = Semigroup<>(gens);
  really_delete_cont(gens);
  S.set_report(SEMIGROUPS_REPORT);

  // Calling cbegin/cend_sorted fully enumerates the semigroup
  { auto it = S.cbegin_sorted(); }
  REQUIRE(S.is_done());

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

  pos = S.size();
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
}

TEST_CASE("Semigroup 69: iterator sorted arithmetic",
          "[quick][semigroup][finite][69]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}),
         new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};
  Semigroup<> S = Semigroup<>(gens);
  really_delete_cont(gens);
  S.set_report(SEMIGROUPS_REPORT);

  REQUIRE(S.size() == 7776);
  auto it = S.cbegin_sorted();
  // The next line should not compile (and does not).
  // (*it)->really_delete();

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
}

TEST_CASE("Semigroup 32: copy [not enumerated]",
          "[quick][semigroup][finite][32]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}),
         new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(SEMIGROUPS_REPORT);

  REQUIRE(!S.is_begun());
  REQUIRE(!S.is_done());
  REQUIRE(S.current_size() == 5);
  REQUIRE(S.current_nrrules() == 0);
  REQUIRE(S.current_max_word_length() == 1);
  REQUIRE(S.current_position(gens[1]) == 1);

  really_delete_cont(gens);

  Semigroup<> T = Semigroup<>(S);
  T.set_report(SEMIGROUPS_REPORT);

  REQUIRE(!T.is_begun());
  REQUIRE(!T.is_done());
  REQUIRE(T.nrgens() == 5);
  REQUIRE(T.degree() == 6);
  REQUIRE(T.current_size() == 5);
  REQUIRE(T.current_nrrules() == 0);
  REQUIRE(T.current_max_word_length() == 1);
  REQUIRE(T.current_position(S.gens(1)) == 1);

  REQUIRE(T.size() == 7776);
  REQUIRE(T.nridempotents() == 537);
  REQUIRE(T.nrrules() == 2459);
  REQUIRE(T.is_begun());
  REQUIRE(T.is_done());
}

TEST_CASE("Semigroup 33: copy_closure [not enumerated]",
          "[quick][semigroup][finite][33]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5})};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(SEMIGROUPS_REPORT);
  really_delete_cont(gens);

  REQUIRE(!S.is_begun());
  REQUIRE(!S.is_done());
  REQUIRE(S.nrgens() == 2);
  REQUIRE(S.degree() == 6);
  REQUIRE(S.current_size() == 2);
  REQUIRE(S.current_nrrules() == 0);
  REQUIRE(S.current_max_word_length() == 1);

  std::vector<Element*> coll
      = {new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}),
         new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};

  Semigroup<>* T = S.copy_closure(&coll);
  T->set_report(SEMIGROUPS_REPORT);
  really_delete_cont(coll);

  REQUIRE(T->is_begun());
  REQUIRE(!T->is_done());
  REQUIRE(T->nrgens() == 5);
  REQUIRE(T->degree() == 6);
  REQUIRE(T->current_size() == 7719);
  REQUIRE(T->current_nrrules() == 2418);
  REQUIRE(T->current_max_word_length() == 14);
  REQUIRE(T->current_position(S.gens(1)) == 1);

  REQUIRE(T->size() == 7776);
  REQUIRE(T->is_done());
  REQUIRE(T->nridempotents() == 537);
  REQUIRE(T->nrrules() == 2459);

  coll           = {new Transformation<u_int16_t>({6, 0, 1, 2, 3, 5, 6})};
  Semigroup<>* U = T->copy_closure(&coll);
  U->set_report(SEMIGROUPS_REPORT);
  really_delete_cont(coll);

  REQUIRE(U->is_begun());
  REQUIRE(U->is_done());
  REQUIRE(U->nrgens() == 6);
  REQUIRE(U->degree() == 7);
  REQUIRE(U->current_size() == 16807);
  REQUIRE(U->current_max_word_length() == 16);
  REQUIRE(U->nridempotents() == 1358);
  REQUIRE(U->nrrules() == 7901);

  coll           = std::vector<Element*>();
  Semigroup<>* V = U->copy_closure(&coll);
  V->set_report(SEMIGROUPS_REPORT);
  REQUIRE(V != U);
  REQUIRE(V->is_begun());
  REQUIRE(V->is_done());
  REQUIRE(V->nrgens() == 6);
  REQUIRE(V->degree() == 7);
  REQUIRE(V->current_size() == 16807);
  REQUIRE(V->current_max_word_length() == 16);
  REQUIRE(V->nridempotents() == 1358);
  REQUIRE(V->nrrules() == 7901);

  delete T;
  delete U;
  delete V;
}

TEST_CASE("Semigroup 34: copy_add_generators [not enumerated]",
          "[quick][semigroup][finite][34]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5})};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(SEMIGROUPS_REPORT);
  really_delete_cont(gens);

  REQUIRE(!S.is_begun());
  REQUIRE(!S.is_done());
  REQUIRE(S.nrgens() == 2);
  REQUIRE(S.degree() == 6);
  REQUIRE(S.current_size() == 2);
  REQUIRE(S.current_nrrules() == 0);
  REQUIRE(S.current_max_word_length() == 1);

  std::vector<Element*> coll
      = {new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}),
         new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};

  Semigroup<>* T = S.copy_add_generators(&coll);
  T->set_report(SEMIGROUPS_REPORT);
  really_delete_cont(coll);

  REQUIRE(!T->is_begun());
  REQUIRE(!T->is_done());
  REQUIRE(T->nrgens() == 5);
  REQUIRE(T->degree() == 6);
  REQUIRE(T->current_size() == 5);
  REQUIRE(T->current_nrrules() == 0);
  REQUIRE(T->current_max_word_length() == 1);
  REQUIRE(T->current_position(S.gens(1)) == 1);

  REQUIRE(T->size() == 7776);
  REQUIRE(T->is_done());
  REQUIRE(T->nridempotents() == 537);
  REQUIRE(T->nrrules() == 2459);

  coll           = {new Transformation<u_int16_t>({6, 0, 1, 2, 3, 5, 6})};
  Semigroup<>* U = T->copy_add_generators(&coll);
  U->set_report(SEMIGROUPS_REPORT);
  really_delete_cont(coll);

  REQUIRE(U->is_begun());
  REQUIRE(U->is_done());
  REQUIRE(U->nrgens() == 6);
  REQUIRE(U->degree() == 7);
  REQUIRE(U->current_size() == 16807);
  REQUIRE(U->current_max_word_length() == 16);
  REQUIRE(U->nridempotents() == 1358);
  REQUIRE(U->nrrules() == 7901);

  coll           = std::vector<Element*>();
  Semigroup<>* V = U->copy_add_generators(&coll);
  V->set_report(SEMIGROUPS_REPORT);
  REQUIRE(V != U);
  REQUIRE(V->is_begun());
  REQUIRE(V->is_done());
  REQUIRE(V->nrgens() == 6);
  REQUIRE(V->degree() == 7);
  REQUIRE(V->current_size() == 16807);
  REQUIRE(V->current_max_word_length() == 16);
  REQUIRE(V->nridempotents() == 1358);
  REQUIRE(V->nrrules() == 7901);

  delete T;
  delete U;
  delete V;
}

TEST_CASE("Semigroup 35: copy [partly enumerated]",
          "[quick][semigroup][finite][35]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}),
         new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(SEMIGROUPS_REPORT);
  S.set_batch_size(1000);
  S.enumerate(1001);

  REQUIRE(S.is_begun());
  REQUIRE(!S.is_done());
  REQUIRE(S.current_size() == 1006);
  REQUIRE(S.current_nrrules() == 70);
  REQUIRE(S.current_max_word_length() == 7);
  REQUIRE(S.current_position(gens[1]) == 1);
  really_delete_cont(gens);

  Semigroup<> T = Semigroup<>(S);
  T.set_report(SEMIGROUPS_REPORT);

  REQUIRE(T.is_begun());
  REQUIRE(!T.is_done());
  REQUIRE(T.nrgens() == 5);
  REQUIRE(T.degree() == 6);
  REQUIRE(T.current_size() == 1006);
  REQUIRE(T.current_nrrules() == 70);
  REQUIRE(T.current_max_word_length() == 7);

  Element* x = new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5});
  REQUIRE(T.current_position(x) == 0);
  x->really_delete();
  delete x;

  x = new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5});
  REQUIRE(T.current_position(x) == 1);
  x->really_delete();
  delete x;

  REQUIRE(T.size() == 7776);
  REQUIRE(T.nridempotents() == 537);
  REQUIRE(T.nrrules() == 2459);
  REQUIRE(T.is_begun());
  REQUIRE(T.is_done());
}

TEST_CASE("Semigroup 36: copy_closure [partly enumerated]",
          "[quick][semigroup][finite][36]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5})};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(SEMIGROUPS_REPORT);
  really_delete_cont(gens);
  S.set_batch_size(60);
  S.enumerate(60);

  REQUIRE(S.is_begun());
  REQUIRE(!S.is_done());
  REQUIRE(S.nrgens() == 3);
  REQUIRE(S.degree() == 6);
  REQUIRE(S.current_size() == 63);
  REQUIRE(S.current_nrrules() == 11);
  REQUIRE(S.current_max_word_length() == 7);

  std::vector<Element*> coll
      = {new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};

  Semigroup<>* T = S.copy_closure(&coll);
  T->set_report(SEMIGROUPS_REPORT);
  REQUIRE(*coll[0] == *(T->gens(3)));
  REQUIRE(*coll[1] == *(T->gens(4)));
  really_delete_cont(coll);

  REQUIRE(T->is_begun());
  REQUIRE(!T->is_done());
  REQUIRE(T->nrgens() == 5);
  REQUIRE(T->degree() == 6);
  REQUIRE(T->current_size() == 7719);
  REQUIRE(T->current_nrrules() == 2418);
  REQUIRE(T->current_max_word_length() == 14);

  REQUIRE(T->size() == 7776);
  REQUIRE(T->is_done());
  REQUIRE(T->nridempotents() == 537);
  REQUIRE(T->nrrules() == 2459);
  delete T;
}

TEST_CASE("Semigroup 37: copy_add_generators [partly enumerated]",
          "[quick][semigroup][finite][37]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5})};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(SEMIGROUPS_REPORT);
  really_delete_cont(gens);
  S.set_batch_size(60);
  S.enumerate(60);

  REQUIRE(S.is_begun());
  REQUIRE(!S.is_done());
  REQUIRE(S.nrgens() == 3);
  REQUIRE(S.degree() == 6);
  REQUIRE(S.current_size() == 63);
  REQUIRE(S.current_nrrules() == 11);
  REQUIRE(S.current_max_word_length() == 7);

  std::vector<Element*> coll
      = {new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};

  Semigroup<>* T = S.copy_add_generators(&coll);
  T->set_report(SEMIGROUPS_REPORT);
  REQUIRE(*coll[0] == *(T->gens(3)));
  REQUIRE(*coll[1] == *(T->gens(4)));
  really_delete_cont(coll);

  REQUIRE(T->is_begun());
  REQUIRE(!T->is_done());
  REQUIRE(T->nrgens() == 5);
  REQUIRE(T->degree() == 6);
  REQUIRE(T->current_size() == 818);
  REQUIRE(T->current_nrrules() == 55);
  REQUIRE(T->current_max_word_length() == 7);

  REQUIRE(T->size() == 7776);
  REQUIRE(T->is_done());
  REQUIRE(T->nridempotents() == 537);
  REQUIRE(T->nrrules() == 2459);
  delete T;
}

TEST_CASE("Semigroup 38: copy [fully enumerated]",
          "[quick][semigroup][finite][38]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}),
         new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(SEMIGROUPS_REPORT);
  really_delete_cont(gens);

  S.enumerate(8000);

  REQUIRE(S.is_begun());
  REQUIRE(S.is_done());
  REQUIRE(S.size() == 7776);
  REQUIRE(S.nridempotents() == 537);
  REQUIRE(S.nrrules() == 2459);

  Semigroup<> T = Semigroup<>(S);
  T.set_report(SEMIGROUPS_REPORT);

  REQUIRE(T.is_begun());
  REQUIRE(T.is_done());
  REQUIRE(T.nrgens() == 5);
  REQUIRE(T.degree() == 6);
  REQUIRE(T.size() == 7776);
  REQUIRE(T.nridempotents() == 537);
  REQUIRE(T.nrrules() == 2459);
}

TEST_CASE("Semigroup 39: copy_closure [fully enumerated]",
          "[quick][semigroup][finite][39]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5})};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(SEMIGROUPS_REPORT);
  really_delete_cont(gens);
  S.enumerate(121);

  REQUIRE(S.is_begun());
  REQUIRE(S.is_done());
  REQUIRE(S.nrgens() == 3);
  REQUIRE(S.degree() == 6);
  REQUIRE(S.current_size() == 120);
  REQUIRE(S.current_nrrules() == 25);
  REQUIRE(S.current_max_word_length() == 11);

  std::vector<Element*> coll
      = {new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};

  Semigroup<>* T = S.copy_closure(&coll);
  T->set_report(SEMIGROUPS_REPORT);
  REQUIRE(*coll[0] == *(T->gens(3)));
  REQUIRE(*coll[1] == *(T->gens(4)));
  really_delete_cont(coll);

  REQUIRE(T->is_begun());
  REQUIRE(!T->is_done());
  REQUIRE(T->nrgens() == 5);
  REQUIRE(T->degree() == 6);
  REQUIRE(T->current_size() == 7719);
  REQUIRE(T->current_nrrules() == 2418);
  REQUIRE(T->current_max_word_length() == 14);

  REQUIRE(T->size() == 7776);
  REQUIRE(T->is_done());
  REQUIRE(T->nridempotents() == 537);
  REQUIRE(T->nrrules() == 2459);
  delete T;
}

TEST_CASE("Semigroup 40: copy_add_generators [fully enumerated]",
          "[quick][semigroup][finite][40]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5})};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(SEMIGROUPS_REPORT);
  really_delete_cont(gens);
  S.enumerate(121);

  REQUIRE(S.is_begun());
  REQUIRE(S.is_done());
  REQUIRE(S.nrgens() == 3);
  REQUIRE(S.degree() == 6);
  REQUIRE(S.current_size() == 120);
  REQUIRE(S.current_nrrules() == 25);
  REQUIRE(S.current_max_word_length() == 11);

  std::vector<Element*> coll
      = {new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};

  Semigroup<>* T = S.copy_add_generators(&coll);
  T->set_report(SEMIGROUPS_REPORT);
  REQUIRE(*coll[0] == *(T->gens(3)));
  REQUIRE(*coll[1] == *(T->gens(4)));
  really_delete_cont(coll);

  REQUIRE(T->is_begun());
  REQUIRE(!T->is_done());
  REQUIRE(T->nrgens() == 5);
  REQUIRE(T->degree() == 6);
  REQUIRE(T->current_size() == 6842);
  REQUIRE(T->current_nrrules() == 1970);
  REQUIRE(T->current_max_word_length() == 12);

  REQUIRE(T->size() == 7776);
  REQUIRE(T->is_done());
  REQUIRE(T->nridempotents() == 537);
  REQUIRE(T->nrrules() == 2459);
  delete T;
}

TEST_CASE("Semigroup 41: relations [duplicate gens]",
          "[quick][semigroup][finite][41]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5})};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(SEMIGROUPS_REPORT);
  really_delete_cont(gens);

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
  REQUIRE(S.nrrules() == nr);

  S.next_relation(result);
  REQUIRE(result.empty());
}

TEST_CASE("Semigroup 42: relations", "[quick][semigroup][finite][42]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({0, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 0, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({4, 0, 1, 2, 3, 5}),
         new Transformation<u_int16_t>({5, 1, 2, 3, 4, 5}),
         new Transformation<u_int16_t>({1, 1, 2, 3, 4, 5})};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(SEMIGROUPS_REPORT);
  really_delete_cont(gens);

  std::vector<size_t> result;
  S.next_relation(result);
  size_t nr = 0;
  while (!result.empty()) {
    word_t lhs, rhs;
    S.factorisation(lhs, result[0]);
    lhs.push_back(result[1]);
    S.factorisation(rhs, result[2]);

    REQUIRE(evaluate_reduct(S, lhs) == evaluate_reduct(S, rhs));
    S.next_relation(result);
    nr++;
  }
  REQUIRE(S.nrrules() == nr);

  S.reset_next_relation();
  S.next_relation(result);
  nr = 0;

  while (!result.empty()) {
    word_t lhs, rhs;
    S.factorisation(lhs, result[0]);
    lhs.push_back(result[1]);
    S.factorisation(rhs, result[2]);

    REQUIRE(evaluate_reduct(S, lhs) == evaluate_reduct(S, rhs));
    REQUIRE(evaluate_reduct(S, lhs) == evaluate_reduct(S, rhs));
    S.next_relation(result);
    nr++;
  }

  REQUIRE(S.nrrules() == nr);
}
