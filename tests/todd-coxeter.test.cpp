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

// The purpose of this file is to test the ToddCoxeter classes.

#include <utility>

#include "libsemigroups.tests.hpp"

#include "internal/report.hpp"

#include "bmat8.hpp"
#include "element.hpp"
#include "froidure-pin.hpp"
#include "tce.hpp"

namespace libsemigroups {
  namespace congruence_todd_coxeter {

    template <class TElementType>
    void delete_gens(std::vector<TElementType>& gens) {
      for (auto x : gens) {
        delete x;
      }
    }

    using ToddCoxeter = congruence::ToddCoxeter;

    constexpr bool REPORT = false;

    congruence_type constexpr TWOSIDED = congruence_type::TWOSIDED;
    congruence_type constexpr LEFT     = congruence_type::LEFT;
    congruence_type constexpr RIGHT    = congruence_type::RIGHT;

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter 001",
                            "(cong) small fp semigroup",
                            "[quick]") {
      REPORTER.set_report(REPORT);

      std::vector<relation_type> rels;
      rels.push_back(relation_type({0, 0, 0}, {0}));  // (a^3, a)
      rels.push_back(relation_type({0}, {1, 1}));     // (a, b^2)

      ToddCoxeter tc(TWOSIDED, 2, rels);

      REQUIRE(!tc.finished());

      REQUIRE(tc.nr_classes() == 5);
      REQUIRE(tc.finished());

      REQUIRE(tc.word_to_class_index({0, 0, 1})
              == tc.word_to_class_index({0, 0, 0, 0, 1}));
      REQUIRE(tc.word_to_class_index({0, 1, 1, 0, 0, 1})
              == tc.word_to_class_index({0, 0, 0, 0, 1}));
      REQUIRE(tc.word_to_class_index({0, 0, 0}) != tc.word_to_class_index({1}));
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter 002",
                            "(cong) Example 6.6 in Sims (see also RWS 13)",
                            "[standard]") {
      // TODO double check if this is standard or not
      REPORTER.set_report(REPORT);

      std::vector<relation_type> rels
          = {relation_type({0, 0}, {0}),
             relation_type({1, 0}, {1}),
             relation_type({0, 1}, {1}),
             relation_type({2, 0}, {2}),
             relation_type({0, 2}, {2}),
             relation_type({3, 0}, {3}),
             relation_type({0, 3}, {3}),
             relation_type({1, 1}, {0}),
             relation_type({2, 3}, {0}),
             relation_type({2, 2, 2}, {0}),
             relation_type({1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2}, {0}),
             relation_type({1, 2, 1, 3, 1, 2, 1, 3, 1, 2, 1, 3, 1, 2, 1, 3,
                            1, 2, 1, 3, 1, 2, 1, 3, 1, 2, 1, 3, 1, 2, 1, 3},
                           {0})};
      ToddCoxeter tc(TWOSIDED, 4, rels, {});
      tc.run_for(std::chrono::milliseconds(200));
      REQUIRE(tc.nr_classes() == 10752);

      FroidurePinBase* S = tc.quotient_semigroup();
      REQUIRE(S->size() == 10752);
      REQUIRE(S->nr_idempotents() == 1);
      // RWS rws(S);
      // REQUIRE(rws.confluent());  // This is slow
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter 003",
                            "(cong) constructed from semigroup",
                            "[quick]") {
      REPORTER.set_report(REPORT);

      FroidurePin<BMat8> S(
          {BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
           BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
           BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {1, 0, 0, 1}}),
           BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}})});

      ToddCoxeter tc(TWOSIDED, S, ToddCoxeter::policy::use_relations);

      tc.add_pair({0}, {1});
      REQUIRE(tc.nr_classes() == 3);
    }

    LIBSEMIGROUPS_TEST_CASE(
        "ToddCoxeter 005",
        "(cong) non-trivial two-sided constructed with FroidurePin",
        "[quick]") {
      REPORTER.set_report(REPORT);

      using Transf = Transf<5>::type;
      FroidurePin<Transf> S({Transf({1, 3, 4, 2, 3}), Transf({3, 2, 1, 3, 3})});

      REQUIRE(S.size() == 88);

      ToddCoxeter tc(TWOSIDED, S, ToddCoxeter::policy::use_cayley_graph);
      tc.add_pair(S.factorisation(Transf({3, 4, 4, 4, 4})),
                  S.factorisation(Transf({3, 1, 3, 3, 3})));

      REQUIRE(tc.nr_classes() == 21);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter 006",
                            "(cong) non-trivial two-sided from relations",
                            "[quick]") {
      REPORTER.set_report(REPORT);

      std::vector<relation_type> relations = {relation_type({0, 1}, {1, 0}),
                                              relation_type({0, 2}, {2, 2}),
                                              relation_type({0, 2}, {0}),
                                              relation_type({0, 2}, {0}),
                                              relation_type({2, 2}, {0}),
                                              relation_type({1, 2}, {1, 2}),
                                              relation_type({1, 2}, {2, 2}),
                                              relation_type({1, 2, 2}, {1}),
                                              relation_type({1, 2}, {1}),
                                              relation_type({2, 2}, {1})};
      std::vector<relation_type> extra     = {relation_type({0}, {1})};

      ToddCoxeter tc1(TWOSIDED, 3, relations, extra);
      REQUIRE(tc1.nr_classes() == 2);

      ToddCoxeter tc2(TWOSIDED, 3, relations, {});
      REQUIRE(tc2.nr_classes() == 2);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter 009",
                            "(cong) small right cong. on free semigroup",
                            "[quick]") {
      REPORTER.set_report(REPORT);

      std::vector<relation_type> rels;
      rels.push_back(relation_type({0, 0, 0}, {0}));  // (a^3, a)
      rels.push_back(relation_type({0}, {1, 1}));     // (a, b^2)
      std::vector<relation_type> extra;

      ToddCoxeter tc(RIGHT, 2, rels, extra);
      REQUIRE(tc.nr_classes() == 5);
      REQUIRE(tc.finished());
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter 010",
                            "(cong) left cong. on free semigroup",
                            "[quick]") {
      REPORTER.set_report(REPORT);

      std::vector<relation_type> rels;
      rels.push_back(relation_type({0, 0, 0}, {0}));  // (a^3, a)
      rels.push_back(relation_type({0}, {1, 1}));     // (a, b^2)
      std::vector<relation_type> extra;

      ToddCoxeter tc(LEFT, 2, rels, extra);
      REQUIRE(tc.word_to_class_index({0, 0, 1})
              == tc.word_to_class_index({0, 0, 0, 0, 1}));
      REQUIRE(tc.word_to_class_index({0, 1, 1, 0, 0, 1})
              == tc.word_to_class_index({0, 0, 0, 0, 1}));
      REQUIRE(tc.word_to_class_index({1})
              != tc.word_to_class_index({0, 0, 0, 0}));
      REQUIRE(tc.word_to_class_index({0, 0, 0})
              != tc.word_to_class_index({0, 0, 0, 0}));
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter 011",
                            "(cong) for small fp semigroup",
                            "[quick]") {
      REPORTER.set_report(REPORT);
      std::vector<relation_type> rels;
      rels.push_back(relation_type({0, 0, 0}, {0}));  // (a^3, a)
      rels.push_back(relation_type({0}, {1, 1}));     // (a, b^2)
      std::vector<relation_type> extra;

      ToddCoxeter tc1(TWOSIDED, 2, rels, extra);
      REQUIRE(tc1.word_to_class_index({0, 0, 1})
              == tc1.word_to_class_index({0, 0, 0, 0, 1}));
      REQUIRE(tc1.word_to_class_index({0, 1, 1, 0, 0, 1})
              == tc1.word_to_class_index({0, 0, 0, 0, 1}));
      REQUIRE(tc1.word_to_class_index({0, 0, 0})
              != tc1.word_to_class_index({1}));

      ToddCoxeter tc2(TWOSIDED, 2, rels, extra);

      REQUIRE(tc2.word_to_class_index({0, 0, 0, 0}) < tc2.nr_classes());
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter 012",
                            "(cong) 2-sided cong. trans. semigroup",
                            "[quick]") {
      REPORTER.set_report(REPORT);
      auto S = FroidurePin<Transformation<u_int16_t>>(
          {Transformation<u_int16_t>({1, 3, 4, 2, 3}),
           Transformation<u_int16_t>({3, 2, 1, 3, 3})});

      REQUIRE(S.size() == 88);
      REQUIRE(S.nr_rules() == 18);

      ToddCoxeter tc(TWOSIDED, S, ToddCoxeter::policy::use_relations);
      tc.add_pair(S.factorisation(Transformation<u_int16_t>({3, 4, 4, 4, 4})),
                  S.factorisation(Transformation<u_int16_t>({3, 1, 3, 3, 3})));

      REQUIRE(tc.nr_classes() == 21);
      REQUIRE(tc.nr_classes() == 21);

      REQUIRE(tc.word_to_class_index(
                  S.factorisation(Transformation<u_int16_t>({1, 3, 1, 3, 3})))
              == tc.word_to_class_index(S.factorisation(
                     Transformation<u_int16_t>({4, 2, 4, 4, 2}))));

      REQUIRE(tc.nr_non_trivial_classes() == 1);
      REQUIRE(tc.cbegin_ntc()->size() == 68);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter 013",
                            "(cong) left cong. trans. semigroup",
                            "[quick]") {
      REPORTER.set_report(REPORT);
      auto S = FroidurePin<Transformation<u_int16_t>>(
          {Transformation<u_int16_t>({1, 3, 4, 2, 3}),
           Transformation<u_int16_t>({3, 2, 1, 3, 3})});

      REQUIRE(S.size() == 88);
      REQUIRE(S.nr_rules() == 18);

      ToddCoxeter tc(LEFT, S, ToddCoxeter::policy::use_relations);
      tc.add_pair(S.factorisation(Transformation<u_int16_t>({3, 4, 4, 4, 4})),
                  S.factorisation(Transformation<u_int16_t>({3, 1, 3, 3, 3})));

      REQUIRE(tc.nr_classes() == 69);
      REQUIRE(tc.nr_classes() == 69);

      REQUIRE(tc.word_to_class_index(
                  S.factorisation(Transformation<u_int16_t>({1, 3, 1, 3, 3})))
              != tc.word_to_class_index(S.factorisation(
                     Transformation<u_int16_t>({4, 2, 4, 4, 2}))));

      REQUIRE(tc.nr_non_trivial_classes() == 1);
      REQUIRE(tc.cbegin_ntc()->size() == 20);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter 014",
                            "(cong) right cong. trans. semigroup",
                            "[quick]") {
      REPORTER.set_report(REPORT);
      auto S = FroidurePin<Transformation<u_int16_t>>(
          {Transformation<u_int16_t>({1, 3, 4, 2, 3}),
           Transformation<u_int16_t>({3, 2, 1, 3, 3})});

      REQUIRE(S.size() == 88);
      REQUIRE(S.nr_rules() == 18);

      ToddCoxeter tc(RIGHT, S, ToddCoxeter::policy::use_relations);
      tc.add_pair(S.factorisation(Transformation<u_int16_t>({3, 4, 4, 4, 4})),
                  S.factorisation(Transformation<u_int16_t>({3, 1, 3, 3, 3})));

      REQUIRE(tc.nr_classes() == 72);
      REQUIRE(tc.nr_classes() == 72);

      REQUIRE(tc.word_to_class_index(
                  S.factorisation(Transformation<u_int16_t>({1, 3, 1, 3, 3})))
              != tc.word_to_class_index(S.factorisation(
                     Transformation<u_int16_t>({4, 2, 4, 4, 2}))));

      REQUIRE(tc.word_to_class_index(
                  S.factorisation(Transformation<u_int16_t>({1, 3, 3, 3, 3})))
              != tc.word_to_class_index(S.factorisation(
                     Transformation<u_int16_t>({4, 2, 4, 4, 2}))));
      REQUIRE(tc.word_to_class_index(
                  S.factorisation(Transformation<u_int16_t>({2, 4, 2, 2, 2})))
              == tc.word_to_class_index(S.factorisation(
                     Transformation<u_int16_t>({2, 3, 3, 3, 3}))));
      REQUIRE(tc.word_to_class_index(
                  S.factorisation(Transformation<u_int16_t>({1, 3, 3, 3, 3})))
              != tc.word_to_class_index(S.factorisation(
                     Transformation<u_int16_t>({2, 3, 3, 3, 3}))));

      REQUIRE(tc.nr_non_trivial_classes() == 4);

      std::vector<size_t> v(tc.nr_non_trivial_classes(), 0);
      std::transform(tc.cbegin_ntc(),
                     tc.cend_ntc(),
                     v.begin(),
                     std::mem_fn(&std::vector<word_type>::size));
      REQUIRE(std::count(v.cbegin(), v.cend(), 3) == 1);
      REQUIRE(std::count(v.cbegin(), v.cend(), 5) == 2);
      REQUIRE(std::count(v.cbegin(), v.cend(), 7) == 1);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter 015",
                            "(cong) trans. semigroup (size 88)",
                            "[quick]") {
      REPORTER.set_report(REPORT);

      std::vector<Element*> gens
          = {new Transformation<u_int16_t>({1, 3, 4, 2, 3}),
             new Transformation<u_int16_t>({3, 2, 1, 3, 3})};
      FroidurePin<> S = FroidurePin<>(gens);
      delete_gens(gens);

      REQUIRE(S.size() == 88);
      REQUIRE(S.nr_rules() == 18);
      REQUIRE(S.degree() == 5);

      ToddCoxeter tc(TWOSIDED, S, ToddCoxeter::policy::use_cayley_graph);

      Element*  t1 = new Transformation<u_int16_t>({3, 4, 4, 4, 4});
      Element*  t2 = new Transformation<u_int16_t>({3, 1, 3, 3, 3});
      word_type w1, w2;
      S.factorisation(w1, S.position(t1));
      S.factorisation(w2, S.position(t2));

      tc.add_pair(w1, w2);

      REQUIRE(tc.nr_classes() == 21);
      REQUIRE(tc.nr_classes() == 21);
      Element*  t3 = new Transformation<u_int16_t>({1, 3, 1, 3, 3});
      Element*  t4 = new Transformation<u_int16_t>({4, 2, 4, 4, 2});
      word_type w3, w4;
      S.factorisation(w3, S.position(t3));
      S.factorisation(w4, S.position(t4));
      REQUIRE(tc.word_to_class_index(w3) == tc.word_to_class_index(w4));

      delete t1;
      delete t2;
      delete t3;
      delete t4;
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter 016",
                            "(cong) left cong. on trans. semigroup (size 88)",
                            "[quick]") {
      std::vector<Element*> gens
          = {new Transformation<u_int16_t>({1, 3, 4, 2, 3}),
             new Transformation<u_int16_t>({3, 2, 1, 3, 3})};
      FroidurePin<> S = FroidurePin<>(gens);
      REPORTER.set_report(REPORT);
      delete_gens(gens);

      REQUIRE(S.size() == 88);
      REQUIRE(S.degree() == 5);
      Element*  t1 = new Transformation<u_int16_t>({3, 4, 4, 4, 4});
      Element*  t2 = new Transformation<u_int16_t>({3, 1, 3, 3, 3});
      word_type w1, w2;
      S.factorisation(w1, S.position(t1));
      S.factorisation(w2, S.position(t2));
      ToddCoxeter tc(LEFT, &S, ToddCoxeter::policy::use_relations);
      tc.add_pair(w1, w2);

      REPORTER.set_report(REPORT);

      REQUIRE(tc.nr_classes() == 69);
      REQUIRE(tc.nr_classes() == 69);
      Element*  t3 = new Transformation<u_int16_t>({1, 3, 1, 3, 3});
      Element*  t4 = new Transformation<u_int16_t>({4, 2, 4, 4, 2});
      word_type w3, w4;
      S.factorisation(w3, S.position(t3));
      S.factorisation(w4, S.position(t4));

      delete t1;
      delete t2;
      delete t3;
      delete t4;
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter 017",
                            "(cong) right cong. on trans. semigroup (size 88)",
                            "[quick]") {
      std::vector<Element*> gens
          = {new Transformation<u_int16_t>({1, 3, 4, 2, 3}),
             new Transformation<u_int16_t>({3, 2, 1, 3, 3})};
      FroidurePin<> S = FroidurePin<>(gens);
      REPORTER.set_report(REPORT);
      delete_gens(gens);

      REQUIRE(S.size() == 88);
      REQUIRE(S.nr_rules() == 18);
      REQUIRE(S.degree() == 5);
      Element*  t1 = new Transformation<u_int16_t>({3, 4, 4, 4, 4});
      Element*  t2 = new Transformation<u_int16_t>({3, 1, 3, 3, 3});
      word_type w1, w2;
      S.factorisation(w1, S.position(t1));
      S.factorisation(w2, S.position(t2));
      ToddCoxeter tc(RIGHT, S, ToddCoxeter::policy::use_relations);
      tc.add_pair(w1, w2);

      REPORTER.set_report(REPORT);

      REQUIRE(tc.nr_classes() == 72);
      REQUIRE(tc.nr_classes() == 72);
      Element*  t3 = new Transformation<u_int16_t>({1, 3, 3, 3, 3});
      Element*  t4 = new Transformation<u_int16_t>({4, 2, 4, 4, 2});
      Element*  t5 = new Transformation<u_int16_t>({2, 4, 2, 2, 2});
      Element*  t6 = new Transformation<u_int16_t>({2, 3, 3, 3, 3});
      word_type w3, w4, w5, w6;
      S.factorisation(w3, S.position(t3));
      S.factorisation(w4, S.position(t4));
      S.factorisation(w5, S.position(t5));
      S.factorisation(w6, S.position(t6));
      REQUIRE(tc.word_to_class_index(w3) != tc.word_to_class_index(w4));
      REQUIRE(tc.word_to_class_index(w5) == tc.word_to_class_index(w6));
      REQUIRE(tc.word_to_class_index(w3) != tc.word_to_class_index(w6));

      delete t1;
      delete t2;
      delete t3;
      delete t4;
      delete t5;
      delete t6;
    }

    LIBSEMIGROUPS_TEST_CASE(
        "ToddCoxeter 018",
        "(cong) finite fp-semigroup, dihedral group of order 6",
        "[quick]") {
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
                                         relation_type({1, 4, 2, 3, 3}, {0}),
                                         relation_type({4, 4, 4}, {0})};
      std::vector<relation_type> extra = {};

      ToddCoxeter tc(TWOSIDED, 5, rels, extra);
      REQUIRE(tc.nr_classes() == 6);
      REQUIRE(tc.word_to_class_index({1}) == tc.word_to_class_index({2}));
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter 019",
                            "(cong) finite fp-semigroup, size 016",
                            "[quick]") {
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
      ToddCoxeter                tc(TWOSIDED, 4, rels, extra);

      REQUIRE(tc.nr_classes() == 16);
      REQUIRE(tc.word_to_class_index({2}) == tc.word_to_class_index({3}));
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter 020",
                            "(cong) finite fp-semigroup, size 016",
                            "[quick]") {
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

      ToddCoxeter tc(TWOSIDED, 11, rels, extra);

      REQUIRE(tc.nr_classes() == 16);
      REQUIRE(tc.word_to_class_index({0}) == tc.word_to_class_index({5}));
      REQUIRE(tc.word_to_class_index({0}) == tc.word_to_class_index({10}));
      REQUIRE(tc.word_to_class_index({1}) == tc.word_to_class_index({2}));
      REQUIRE(tc.word_to_class_index({1}) == tc.word_to_class_index({7}));
      REQUIRE(tc.word_to_class_index({3}) == tc.word_to_class_index({4}));
      REQUIRE(tc.word_to_class_index({3}) == tc.word_to_class_index({6}));
      REQUIRE(tc.word_to_class_index({3}) == tc.word_to_class_index({8}));
      REQUIRE(tc.word_to_class_index({3}) == tc.word_to_class_index({9}));
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter 021",
                            "(cong) test prefilling of the table manually",
                            "[quick]") {
      REPORTER.set_report(REPORT);
      std::vector<Element*> gens
          = {new Transformation<u_int16_t>({7, 3, 5, 3, 4, 2, 7, 7}),
             new Transformation<u_int16_t>({3, 6, 3, 4, 0, 6, 0, 7})};
      FroidurePin<> S = FroidurePin<>(gens);
      delete_gens(gens);

      // Copy the right Cayley graph of S for prefilling
      FroidurePin<>::cayley_graph_type const* right = S.right_cayley_graph_copy();
      RecVec<size_t> table(S.nr_generators(), 1, UNDEFINED);
      table.append(*right);
      delete right;
      // TODO move this stuff into prefill
      size_t j = 1;
      std::for_each(table.begin(),
                    table.begin() + table.nr_cols(),
                    [&j](size_t& i) { i = j++; });
      std::for_each(
          table.begin() + table.nr_cols(), table.end(), [](size_t& i) { ++i; });

      ToddCoxeter tc(TWOSIDED, 2, {}, {});
      REQUIRE(tc.get_policy() == ToddCoxeter::policy::none);
      tc.prefill(table);
      REQUIRE(!tc.is_quotient_obviously_infinite());
      REQUIRE(tc.nr_classes() == S.size());
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter 022",
                            "(cong) test packing phase",
                            "[quick]") {
      REPORTER.set_report(REPORT);
      std::vector<relation_type> rels
          = {relation_type({0, 0, 0}, {0}),
             relation_type({1, 0, 0}, {1, 0}),
             relation_type({1, 0, 1, 1, 1}, {1, 0}),
             relation_type({1, 1, 1, 1, 1}, {1, 1}),
             relation_type({1, 1, 0, 1, 1, 0}, {1, 0, 1, 0, 1, 1}),
             relation_type({0, 0, 1, 0, 1, 1, 0}, {0, 1, 0, 1, 1, 0}),
             relation_type({0, 0, 1, 1, 0, 1, 0}, {0, 1, 1, 0, 1, 0}),
             relation_type({0, 1, 0, 1, 0, 1, 0}, {1, 0, 1, 0, 1, 0}),
             relation_type({1, 0, 1, 0, 1, 0, 1}, {1, 0, 1, 0, 1, 0}),
             relation_type({1, 0, 1, 0, 1, 1, 0}, {1, 0, 1, 0, 1, 1}),
             relation_type({1, 0, 1, 1, 0, 1, 0}, {1, 0, 1, 1, 0, 1}),
             relation_type({1, 1, 0, 1, 0, 1, 0}, {1, 0, 1, 0, 1, 0}),
             relation_type({1, 1, 1, 1, 0, 1, 0}, {1, 0, 1, 0}),
             relation_type({0, 0, 1, 1, 1, 0, 1, 0}, {1, 1, 1, 0, 1, 0})};

      ToddCoxeter tc1(TWOSIDED, 2, rels, std::vector<relation_type>());
      tc1.set_pack(10);
      REQUIRE(tc1.nr_classes() == 78);

      ToddCoxeter tc2(LEFT, 2, rels, std::vector<relation_type>());
      tc2.set_pack(10);
      REQUIRE(tc2.nr_classes() == 78);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter 024",
                            "(cong) non-trivial left cong. from semigroup",
                            "[quick]") {
      REPORTER.set_report(REPORT);

      std::vector<Element*> gens
          = {new Transformation<u_int16_t>({1, 3, 4, 2, 3}),
             new Transformation<u_int16_t>({3, 2, 1, 3, 3})};
      FroidurePin<> S = FroidurePin<>(gens);
      delete_gens(gens);

      REQUIRE(S.size() == 88);
      REQUIRE(S.degree() == 5);

      Element*  t1 = new Transformation<u_int16_t>({3, 4, 4, 4, 4});
      Element*  t2 = new Transformation<u_int16_t>({3, 1, 3, 3, 3});
      word_type w1, w2;
      S.factorisation(w1, S.position(t1));
      S.factorisation(w2, S.position(t2));
      delete t1;
      delete t2;

      ToddCoxeter tc(LEFT, S, ToddCoxeter::policy::use_cayley_graph);
      tc.add_pair(w1, w2);
      REQUIRE(tc.nr_classes() == 69);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter 025",
                            "(cong) 2-sided cong. on free semigroup",
                            "[quick]") {
      REPORTER.set_report(REPORT);
      ToddCoxeter tc(TWOSIDED, 1, {});
      REQUIRE(tc.contains({0, 0}, {0, 0}));
      REQUIRE(!tc.contains({0, 0}, {0}));
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter 027",
                            "(cong) calling run when obviously infinite",
                            "[quick]") {
      ToddCoxeter tc(TWOSIDED, 5, {});
      REQUIRE_THROWS_AS(tc.run(), LibsemigroupsException);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter 030",
                            "(cong) Stellar S3",
                            "[quick][hivert]") {
      REPORTER.set_report(REPORT);

      congruence::ToddCoxeter tc(TWOSIDED);
      tc.set_nr_generators(4);
      tc.add_pair({3, 3}, {3});
      tc.add_pair({0, 3}, {0});
      tc.add_pair({3, 0}, {0});
      tc.add_pair({1, 3}, {1});
      tc.add_pair({3, 1}, {1});
      tc.add_pair({2, 3}, {2});
      tc.add_pair({3, 2}, {2});
      tc.add_pair({0, 0}, {0});
      tc.add_pair({1, 1}, {1});
      tc.add_pair({2, 2}, {2});
      tc.add_pair({0, 2}, {2, 0});
      tc.add_pair({2, 0}, {0, 2});
      tc.add_pair({1, 2, 1}, {2, 1, 2});
      tc.add_pair({1, 0, 1, 0}, {0, 1, 0, 1});
      tc.add_pair({1, 0, 1, 0}, {0, 1, 0});

      REQUIRE(tc.nr_classes() == 34);
      REQUIRE(tc.quotient_semigroup()->size() == 34);

      auto S = static_cast<FroidurePin<TCE>*>(tc.quotient_semigroup());
      S->enumerate();
      std::vector<TCE> v(S->cbegin(), S->cend());
      std::sort(v.begin(), v.end());
      REQUIRE(v
              == std::vector<TCE>(
                     {TCE(tc, 1),  TCE(tc, 2),  TCE(tc, 3),  TCE(tc, 4),
                      TCE(tc, 5),  TCE(tc, 6),  TCE(tc, 7),  TCE(tc, 8),
                      TCE(tc, 9),  TCE(tc, 10), TCE(tc, 11), TCE(tc, 12),
                      TCE(tc, 13), TCE(tc, 14), TCE(tc, 15), TCE(tc, 16),
                      TCE(tc, 17), TCE(tc, 18), TCE(tc, 19), TCE(tc, 20),
                      TCE(tc, 21), TCE(tc, 22), TCE(tc, 23), TCE(tc, 24),
                      TCE(tc, 25), TCE(tc, 26), TCE(tc, 27), TCE(tc, 28),
                      TCE(tc, 29), TCE(tc, 30), TCE(tc, 31), TCE(tc, 32),
                      TCE(tc, 33), TCE(tc, 34)}));
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter 031",
                            "(cong) finite semigroup (size 5)",
                            "[quick]") {
      congruence::ToddCoxeter tc(LEFT);
      tc.set_nr_generators(2);
      tc.add_pair({0, 0, 0}, {0});  // (a^3, a)
      tc.add_pair({0}, {1, 1});     // (a, b^2)
      REQUIRE(tc.nr_classes() == 5);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter 033", "(cong) exceptions", "[quick]") {
      {
        congruence::ToddCoxeter tc1(LEFT);
        tc1.set_nr_generators(2);
        tc1.add_pair({0, 0, 0}, {0});
        tc1.add_pair({0}, {1, 1});
        REQUIRE(tc1.nr_classes() == 5);

        REQUIRE_THROWS_AS(ToddCoxeter(RIGHT, tc1), LibsemigroupsException);
        REQUIRE_THROWS_AS(ToddCoxeter(TWOSIDED, tc1), LibsemigroupsException);

        ToddCoxeter tc2(LEFT, tc1);
        REQUIRE(!tc1.contains({0}, {1}));
        tc2.add_pair({0}, {1});
        REQUIRE(tc2.nr_classes() == 1);

        ToddCoxeter tc3(LEFT);
        tc3.set_nr_generators(2);
        tc3.add_pair({0, 0, 0}, {0});
        tc3.add_pair({0}, {1, 1});
        tc3.add_pair({0}, {1});
        REQUIRE(tc3.nr_classes() == 1);
      }
      {
        congruence::ToddCoxeter tc1(RIGHT);
        tc1.set_nr_generators(2);
        tc1.add_pair({0, 0, 0}, {0});
        tc1.add_pair({0}, {1, 1});
        REQUIRE(tc1.nr_classes() == 5);

        REQUIRE_THROWS_AS(ToddCoxeter(LEFT, tc1), LibsemigroupsException);
        REQUIRE_THROWS_AS(ToddCoxeter(TWOSIDED, tc1), LibsemigroupsException);

        ToddCoxeter tc2(RIGHT, tc1);
        REQUIRE(!tc1.contains({0}, {1}));
        tc2.add_pair({0}, {1});
        REQUIRE(tc2.nr_classes() == 1);

        ToddCoxeter tc3(RIGHT);
        tc3.set_nr_generators(2);
        tc3.add_pair({0, 0, 0}, {0});
        tc3.add_pair({0}, {1, 1});
        tc3.add_pair({0}, {1});
        REQUIRE(tc3.nr_classes() == 1);
      }
    }
  }  // namespace congruence_todd_coxeter

  namespace fpsemigroup_todd_coxeter {
    using ToddCoxeter     = fpsemigroup::ToddCoxeter;
    constexpr bool REPORT = false;

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter 004", "(fpsemi) add_rule", "[quick]") {
      REPORTER.set_report(REPORT);
      ToddCoxeter tc;
      tc.set_alphabet("ab");
      tc.add_rule("aaa", "a");
      tc.add_rule("a", "bb");

      REQUIRE(tc.size() == 5);
    }

    // RWS methods fail for this one
    LIBSEMIGROUPS_TEST_CASE(
        "ToddCoxeter 007",
        "(fpsemi) (from kbmag/standalone/kb_data/s4) (RWS 49)",
        "[quick][kbmag]") {
      REPORTER.set_report(REPORT);

      ToddCoxeter tc;
      tc.set_alphabet("abcd");
      tc.add_rule("bb", "c");
      tc.add_rule("caca", "abab");
      tc.add_rule("bc", "d");
      tc.add_rule("cb", "d");
      tc.add_rule("aa", "d");
      tc.add_rule("ad", "a");
      tc.add_rule("da", "a");
      tc.add_rule("bd", "b");
      tc.add_rule("db", "b");
      tc.add_rule("cd", "c");
      tc.add_rule("dc", "c");
      REQUIRE(tc.size() == 24);
      REQUIRE(tc.isomorphic_non_fp_semigroup()->size() == 24);
      REQUIRE(tc.normal_form("aaaaaaaaaaaaaaaaaaa") == "a");
      // TODO uncomment the next line
      //  REQUIRE(RWS(tc.isomorphic_non_fp_semigroup()).confluent());
    }

    // Second of BHN's series of increasingly complicated presentations
    // of 1. Doesn't terminate
    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter 008",
                            "(fpsemi) (from kbmag/standalone/kb_data/degen4b) "
                            "(RWS 65)",
                            "[extreme][kbmag][shortlex]") {
      REPORTER.set_report(REPORT);

      ToddCoxeter tc;
      tc.set_alphabet("abcdefg");
      // Inverses . . .
      tc.add_rule("ad", "g");
      tc.add_rule("da", "g");
      tc.add_rule("be", "g");
      tc.add_rule("eb", "g");
      tc.add_rule("cf", "g");
      tc.add_rule("fc", "g");

      // Identity . . .
      tc.add_rule("ag", "a");
      tc.add_rule("bg", "b");
      tc.add_rule("cg", "c");
      tc.add_rule("dg", "d");
      tc.add_rule("eg", "e");
      tc.add_rule("fg", "f");
      tc.add_rule("ga", "a");
      tc.add_rule("gb", "b");
      tc.add_rule("gc", "c");
      tc.add_rule("gd", "d");
      tc.add_rule("ge", "e");
      tc.add_rule("gf", "f");
      tc.add_rule("gg", "g");

      tc.add_rule("bbdeaecbffdbaeeccefbccefb", "g");
      tc.add_rule("ccefbfacddecbffaafdcaafdc", "g");
      tc.add_rule("aafdcdbaeefacddbbdeabbdea", "g");

      REQUIRE(tc.size() == 1);
      REQUIRE(tc.isomorphic_non_fp_semigroup()->size() == 1);
      // TODO uncomment the next line
      // REQUIRE(RWS(tc.isomorphic_non_fp_semigroup()).confluent());
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter 023",
                            "(fpsemi) test validate",
                            "[quick]") {
      REPORTER.set_report(REPORT);

      ToddCoxeter tc;
      tc.set_alphabet("ab");
      tc.add_rule("a", "b");
      tc.add_rule("bb", "b");

      REQUIRE_THROWS_AS(tc.add_rule("b", "c"), LibsemigroupsException);
    }

    LIBSEMIGROUPS_TEST_CASE(
        "ToddCoxeter 026",
        "(fpsemi) add_rules after construct. from semigroup",
        "[quick]") {
      REPORTER.set_report(REPORT);

      using Transf = Transf<5>::type;

      FroidurePin<Transf> S({Transf({1, 3, 4, 2, 3}), Transf({3, 2, 1, 3, 3})});
      REQUIRE(S.size() == 88);
      REQUIRE(S.nr_rules() == 18);

      word_type w1, w2, w3, w4;
      S.factorisation(w1, S.position(Transf({3, 4, 4, 4, 4})));
      S.factorisation(w2, S.position(Transf({3, 1, 3, 3, 3})));
      S.factorisation(w3, S.position(Transf({1, 3, 1, 3, 3})));
      S.factorisation(w4, S.position(Transf({4, 2, 4, 4, 2})));

      ToddCoxeter tc1(S);
      tc1.add_rule(w1, w2);

      REQUIRE(tc1.size() == 21);
      REQUIRE(tc1.size() == tc1.isomorphic_non_fp_semigroup()->size());
      REQUIRE(tc1.equal_to(w3, w4));
      REQUIRE(tc1.normal_form(w3) == tc1.normal_form(w4));

      ToddCoxeter tc2(S);
      tc2.add_rule(w1, w2);

      REQUIRE(tc2.size() == 21);
      REQUIRE(tc2.size() == tc2.isomorphic_non_fp_semigroup()->size());
      REQUIRE(tc2.equal_to(w3, w4));
      REQUIRE(tc2.normal_form(w3) == tc2.normal_form(w4));
    }

    LIBSEMIGROUPS_TEST_CASE(
        "ToddCoxeter 028",
        "(fpsemi) Sym(5) from Chapter 3, Proposition 1.1 in NR",
        "[quick]") {
      REPORTER.set_report(REPORT);

      ToddCoxeter tc("ABabe");
      tc.set_identity("e");
      tc.add_rule("aa", "e");
      tc.add_rule("bbbbb", "e");
      tc.add_rule("babababa", "e");
      tc.add_rule("bB", "e");
      tc.add_rule("Bb", "e");
      tc.add_rule("BabBab", "e");
      tc.add_rule("aBBabbaBBabb", "e");
      tc.add_rule("aBBBabbbaBBBabbb", "e");
      tc.add_rule("aA", "e");
      tc.add_rule("Aa", "e");
      REQUIRE(tc.size() == 120);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter 029",
                            "(fpsemi) Chapter 7, Theorem 3.6 in NR (size 243)",
                            "[quick]") {
      REPORTER.set_report(REPORT);
      ToddCoxeter tc("ab");
      tc.add_rule("aaa", "a");
      tc.add_rule("bbbb", "b");
      tc.add_rule("ababababab", "aa");

      REQUIRE(tc.size() == 243);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter 032",
                            "(fpsemi) finite semigroup (size 99)",
                            "[quick]") {
      REPORTER.set_report(REPORT);
      ToddCoxeter tc("ab");
      tc.add_rule("aaa", "a");
      tc.add_rule("bbbb", "b");
      tc.add_rule("abababab", "aa");

      REQUIRE(tc.size() == 99);
    }

  }  // namespace fpsemigroup_todd_coxeter
}  // namespace libsemigroups
