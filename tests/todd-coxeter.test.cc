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

// The purpose of this file is to test the ToddCoxeter class.

#include <utility>

#include "catch.hpp"

#include "internal/report.h"

#include "bmat8.h"
#include "element-helper.h"
#include "element.h"
#include "rws.h"
#include "semigroup.h"
#include "tce.h"
#include "todd-coxeter.h"

namespace libsemigroups {
  namespace todd_coxeter {

    template <class TElementType>
    void delete_gens(std::vector<TElementType>& gens) {
      for (auto x : gens) {
        delete x;
      }
    }

    using congruence_type = CongIntf::congruence_type;
    using ToddCoxeter     = congruence::ToddCoxeter;

    constexpr bool REPORT = false;

    TEST_CASE("ToddCoxeter 01: small fp semigroup",
              "[quick][todd-coxeter][01]") {
      REPORTER.set_report(REPORT);

      std::vector<relation_type> rels;
      rels.push_back(relation_type({0, 0, 0}, {0}));  // (a^3, a)
      rels.push_back(relation_type({0}, {1, 1}));     // (a, b^2)

      ToddCoxeter tc(congruence_type::TWOSIDED, 2, rels);

      REQUIRE(!tc.finished());

      REQUIRE(tc.nr_classes() == 5);
      REQUIRE(tc.finished());

      REQUIRE(tc.word_to_class_index({0, 0, 1})
              == tc.word_to_class_index({0, 0, 0, 0, 1}));
      REQUIRE(tc.word_to_class_index({0, 1, 1, 0, 0, 1})
              == tc.word_to_class_index({0, 0, 0, 0, 1}));
      REQUIRE(tc.word_to_class_index({0, 0, 0}) != tc.word_to_class_index({1}));
    }

    TEST_CASE("ToddCoxeter 02: Example 6.6 in Sims (see also RWS 13)",
              "[standard][todd-coxeter][02]") {
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
      ToddCoxeter tc(congruence_type::TWOSIDED, 4, rels, {});
      tc.run_for(std::chrono::milliseconds(200));
      REQUIRE(tc.nr_classes() == 10752);

      SemigroupBase* S = tc.quotient_semigroup();
      REQUIRE(S->size() == 10752);
      REQUIRE(S->nridempotents() == 1);
      // RWS rws(S);
      // REQUIRE(rws.confluent());  // This is slow
    }

    TEST_CASE("ToddCoxeter 03: ToddCoxeter constructed with Semigroup",
              "[quick][todd-coxeter][03]") {
      REPORTER.set_report(REPORT);
      std::vector<BMat8> gens
          = {BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
             BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
             BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {1, 0, 0, 1}}),
             BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}})};

      Semigroup<BMat8> S(gens);

      ToddCoxeter tc(congruence_type::TWOSIDED,
                     &S,
                     {{{0}, {1}}},
                     ToddCoxeter::policy::use_relations);
      REQUIRE(tc.nr_classes() == 3);
    }

    /*TEST_CASE("ToddCoxeter 04: add_relation ", "[quick][todd-coxeter][04]") {
      REPORTER.set_report(REPORT);
      ToddCoxeter tc(congruence_type::LEFT);
      tc.set_alphabet("ab");
      tc.add_relation("aaa", "a");
      tc.add_relation("a", "bb");

      REQUIRE(tc.nr_classes() == 5);
    }*/

    TEST_CASE(
        "ToddCoxeter 05: non-trivial two-sided constructed with Semigroup",
        "[quick][todd-coxeter][05]") {
      REPORTER.set_report(REPORT);

      using Transf = Transf<5>::type;
      Semigroup<Transf> S({Transf({1, 3, 4, 2, 3}), Transf({3, 2, 1, 3, 3})});

      REQUIRE(S.size() == 88);

      word_type w1, w2;
      S.factorisation(w1, S.position(Transf({3, 4, 4, 4, 4})));
      S.factorisation(w2, S.position(Transf({3, 1, 3, 3, 3})));

      std::vector<relation_type> extra({relation_type(w1, w2)});
      ToddCoxeter                tc(congruence_type::TWOSIDED,
                     &S,
                     extra,
                     ToddCoxeter::policy::use_cayley_graph);
      REQUIRE(tc.nr_classes() == 21);
    }

    TEST_CASE("ToddCoxeter 06: non-trivial two-sided constructed by relations",
              "[quick][todd-coxeter][06]") {
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

      ToddCoxeter tc1(congruence_type::TWOSIDED, 3, relations, extra);
      REQUIRE(tc1.nr_classes() == 2);

      ToddCoxeter tc2(congruence_type::TWOSIDED, 3, relations, {});
      REQUIRE(tc2.nr_classes() == 2);
    }

    // RWS methods fail for this one
    /*TEST_CASE("ToddCoxeter 07: (from kbmag/standalone/kb_data/s4) (RWS 49)",
              "[quick][todd-coxeter][07]") {
      REPORTER.set_report(REPORT);

      ToddCoxeter tc(congruence_type::TWOSIDED);
      tc.set_alphabet("abcd");
      tc.add_relation("bb", "c");
      tc.add_relation("caca", "abab");
      tc.add_relation("bc", "d");
      tc.add_relation("cb", "d");
      tc.add_relation("aa", "d");
      tc.add_relation("ad", "a");
      tc.add_relation("da", "a");
      tc.add_relation("bd", "b");
      tc.add_relation("db", "b");
      tc.add_relation("cd", "c");
      tc.add_relation("dc", "c");
      REQUIRE(tc.nr_classes() == 24);
      REQUIRE(tc.isomorphic_non_fp_semigroup()->size() == 24);
      REQUIRE(RWS(tc.isomorphic_non_fp_semigroup()).confluent());
    }*/

    // Second of BHN's series of increasingly complicated presentations of 1.
    // Doesn't terminate
    /*    TEST_CASE(
            "ToddCoxeter 08: (from kbmag/standalone/kb_data/degen4b) (RWS 65)",
            "[extreme][todd-coxeter][kbmag][shortlex][08]") {
          REPORTER.set_report(REPORT);

          ToddCoxeter tc(congruence_type::TWOSIDED);
          tc.set_alphabet("abcdefg");
          // Inverses . . .
          tc.add_relation("ad", "g");
          tc.add_relation("da", "g");
          tc.add_relation("be", "g");
          tc.add_relation("eb", "g");
          tc.add_relation("cf", "g");
          tc.add_relation("fc", "g");

          // Identity . . .
          tc.add_relation("ag", "a");
          tc.add_relation("bg", "b");
          tc.add_relation("cg", "c");
          tc.add_relation("dg", "d");
          tc.add_relation("eg", "e");
          tc.add_relation("fg", "f");
          tc.add_relation("ga", "a");
          tc.add_relation("gb", "b");
          tc.add_relation("gc", "c");
          tc.add_relation("gd", "d");
          tc.add_relation("ge", "e");
          tc.add_relation("gf", "f");
          tc.add_relation("gg", "g");

          tc.add_relation("bbdeaecbffdbaeeccefbccefb", "g");
          tc.add_relation("ccefbfacddecbffaafdcaafdc", "g");
          tc.add_relation("aafdcdbaeefacddbbdeabbdea", "g");

          REQUIRE(tc.nr_classes() == 1);
          REQUIRE(tc.isomorphic_non_fp_semigroup()->size() == 1);
          REQUIRE(RWS(tc.isomorphic_non_fp_semigroup()).confluent());
        }*/

    TEST_CASE("ToddCoxeter 09: Small right congruence on free semigroup",
              "[quick][todd-coxeter][09]") {
      REPORTER.set_report(REPORT);

      std::vector<relation_type> rels;
      rels.push_back(relation_type({0, 0, 0}, {0}));  // (a^3, a)
      rels.push_back(relation_type({0}, {1, 1}));     // (a, b^2)
      std::vector<relation_type> extra;

      ToddCoxeter tc(congruence_type::RIGHT, 2, rels, extra);
      REQUIRE(tc.nr_classes() == 5);
      REQUIRE(tc.finished());
    }

    TEST_CASE("ToddCoxeter 10: word_to_class_index for left "
              "congruence on free semigroup",
              "[quick][todd-coxeter][10]") {
      REPORTER.set_report(REPORT);

      std::vector<relation_type> rels;
      rels.push_back(relation_type({0, 0, 0}, {0}));  // (a^3, a)
      rels.push_back(relation_type({0}, {1, 1}));     // (a, b^2)
      std::vector<relation_type> extra;

      ToddCoxeter tc(congruence_type::LEFT, 2, rels, extra);
      REQUIRE(tc.word_to_class_index({0, 0, 1})
              == tc.word_to_class_index({0, 0, 0, 0, 1}));
      REQUIRE(tc.word_to_class_index({0, 1, 1, 0, 0, 1})
              == tc.word_to_class_index({0, 0, 0, 0, 1}));
      REQUIRE(tc.word_to_class_index({1})
              != tc.word_to_class_index({0, 0, 0, 0}));
      REQUIRE(tc.word_to_class_index({0, 0, 0})
              != tc.word_to_class_index({0, 0, 0, 0}));
    }

    TEST_CASE("ToddCoxeter 11: word_to_class_index for small fp semigroup",
              "[quick][todd-coxeter][11]") {
      REPORTER.set_report(REPORT);
      std::vector<relation_type> rels;
      rels.push_back(relation_type({0, 0, 0}, {0}));  // (a^3, a)
      rels.push_back(relation_type({0}, {1, 1}));     // (a, b^2)
      std::vector<relation_type> extra;

      ToddCoxeter tc1(congruence_type::TWOSIDED, 2, rels, extra);
      REQUIRE(tc1.word_to_class_index({0, 0, 1})
              == tc1.word_to_class_index({0, 0, 0, 0, 1}));
      REQUIRE(tc1.word_to_class_index({0, 1, 1, 0, 0, 1})
              == tc1.word_to_class_index({0, 0, 0, 0, 1}));
      REQUIRE(tc1.word_to_class_index({0, 0, 0})
              != tc1.word_to_class_index({1}));

      ToddCoxeter tc2(congruence_type::TWOSIDED, 2, rels, extra);

      REQUIRE(tc2.word_to_class_index({0, 0, 0, 0}) < tc2.nr_classes());
    }

    TEST_CASE("ToddCoxeter 12: transformation semigroup size 88",
              "[quick][todd-coxeter][12]") {
      REPORTER.set_report(REPORT);
      std::vector<Element*> vec
          = {new Transformation<u_int16_t>({1, 3, 4, 2, 3}),
             new Transformation<u_int16_t>({3, 2, 1, 3, 3})};
      Semigroup<> S = Semigroup<>(vec);

      REQUIRE(S.size() == 88);
      REQUIRE(S.nrrules() == 18);
      REQUIRE(S.degree() == 5);

      vec.push_back(new Transformation<u_int16_t>({3, 4, 4, 4, 4}));
      word_type w1;
      S.factorisation(w1, S.position(vec.back()));

      vec.push_back(new Transformation<u_int16_t>({3, 1, 3, 3, 3}));
      word_type w2;
      S.factorisation(w2, S.position(vec.back()));

      std::vector<relation_type> extra({relation_type(w1, w2)});
      ToddCoxeter                tc(congruence_type::TWOSIDED,
                     &S,
                     extra,
                     ToddCoxeter::policy::use_relations);

      REQUIRE(tc.nr_classes() == 21);
      REQUIRE(tc.nr_classes() == 21);

      vec.push_back(new Transformation<u_int16_t>({1, 3, 1, 3, 3}));
      S.factorisation(w1, S.position(vec.back()));

      vec.push_back(new Transformation<u_int16_t>({4, 2, 4, 4, 2}));
      S.factorisation(w2, S.position(vec.back()));

      REQUIRE(tc.word_to_class_index(w1) == tc.word_to_class_index(w2));

      // Partition<word_type>* ntc = tc.nontrivial_classes();
      // REQUIRE(ntc->size() == 1);
      // REQUIRE(ntc->at(0)->size() == 68);
      // delete ntc;

      delete_gens(vec);
    }

    TEST_CASE(
        "ToddCoxeter 13: left congruence on transformation semigroup size 88",
        "[quick][todd-coxeter][13]") {
      REPORTER.set_report(REPORT);
      std::vector<Element*> vec
          = {new Transformation<u_int16_t>({1, 3, 4, 2, 3}),
             new Transformation<u_int16_t>({3, 2, 1, 3, 3})};
      Semigroup<> S = Semigroup<>(vec);

      REQUIRE(S.size() == 88);
      REQUIRE(S.degree() == 5);

      vec.push_back(new Transformation<u_int16_t>({3, 4, 4, 4, 4}));
      word_type w1;
      S.factorisation(w1, S.position(vec.back()));
      vec.push_back(new Transformation<u_int16_t>({3, 1, 3, 3, 3}));
      word_type w2;
      S.factorisation(w2, S.position(vec.at(3)));
      std::vector<relation_type> extra({relation_type(w1, w2)});
      ToddCoxeter                tc(
          congruence_type::LEFT, &S, extra, ToddCoxeter::policy::use_relations);

      REQUIRE(tc.nr_classes() == 69);
      REQUIRE(tc.nr_classes() == 69);

      // Partition<word_type>* ntc = tc.nontrivial_classes();
      // REQUIRE(ntc->size() == 1);
      // REQUIRE(ntc->at(0)->size() == 20);
      // delete ntc;

      delete_gens(vec);
    }

    TEST_CASE(
        "ToddCoxeter 14: right congruence on transformation semigroup size 88",
        "[quick][todd-coxeter][14]") {
      REPORTER.set_report(REPORT);
      std::vector<Element*> vec
          = {new Transformation<u_int16_t>({1, 3, 4, 2, 3}),
             new Transformation<u_int16_t>({3, 2, 1, 3, 3})};
      Semigroup<> S = Semigroup<>(vec);

      REQUIRE(S.size() == 88);
      REQUIRE(S.nrrules() == 18);
      REQUIRE(S.degree() == 5);

      word_type w1, w2;
      vec.push_back(new Transformation<u_int16_t>({3, 4, 4, 4, 4}));
      S.factorisation(w1, S.position(vec.back()));
      vec.push_back(new Transformation<u_int16_t>({3, 1, 3, 3, 3}));
      S.factorisation(w2, S.position(vec.back()));

      ToddCoxeter tc(congruence_type::RIGHT,
                     &S,
                     {relation_type(w1, w2)},
                     ToddCoxeter::policy::use_relations);

      REQUIRE(tc.nr_classes() == 72);
      REQUIRE(tc.nr_classes() == 72);

      word_type w3, w4, w5, w6;
      vec.push_back(new Transformation<u_int16_t>({1, 3, 3, 3, 3}));
      S.factorisation(w3, S.position(vec.back()));
      vec.push_back(new Transformation<u_int16_t>({4, 2, 4, 4, 2}));
      S.factorisation(w4, S.position(vec.back()));
      vec.push_back(new Transformation<u_int16_t>({2, 4, 2, 2, 2}));
      S.factorisation(w5, S.position(vec.back()));
      vec.push_back(new Transformation<u_int16_t>({2, 3, 3, 3, 3}));
      S.factorisation(w6, S.position(vec.back()));

      REQUIRE(tc.word_to_class_index(w3) != tc.word_to_class_index(w4));
      REQUIRE(tc.word_to_class_index(w5) == tc.word_to_class_index(w6));
      REQUIRE(tc.word_to_class_index(w3) != tc.word_to_class_index(w6));

      // Partition<word_type>* ntc = tc.nontrivial_classes();
      // REQUIRE(ntc->size() == 4);
      // std::vector<size_t> sizes({0, 0, 0});
      // for (size_t i = 0; i < ntc->size(); i++) {
      //   switch (ntc->at(i)->size()) {
      //     case 3:
      //       sizes[0]++;
      //       break;
      //     case 5:
      //       sizes[1]++;
      //       break;
      //     case 7:
      //       sizes[2]++;
      //       break;
      //     default:
      //       REQUIRE(false);
      //   }
      // }
      // REQUIRE(sizes == std::vector<size_t>({1, 2, 1}));
      // delete ntc;
      delete_gens(vec);
    }

    TEST_CASE("ToddCoxeter 15: transformation semigroup size 88",
              "[quick][todd-coxeter][15]") {
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
      std::vector<relation_type> extra({relation_type(w1, w2)});
      ToddCoxeter                tc(congruence_type::TWOSIDED,
                     &S,
                     extra,
                     ToddCoxeter::policy::use_cayley_graph);

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

    TEST_CASE(
        "ToddCoxeter 16: left congruence on transformation semigroup size 88 ",
        "[quick][todd-coxeter][16]") {
      std::vector<Element*> gens
          = {new Transformation<u_int16_t>({1, 3, 4, 2, 3}),
             new Transformation<u_int16_t>({3, 2, 1, 3, 3})};
      Semigroup<> S = Semigroup<>(gens);
      REPORTER.set_report(REPORT);
      delete_gens(gens);

      REQUIRE(S.size() == 88);
      REQUIRE(S.degree() == 5);
      Element*  t1 = new Transformation<u_int16_t>({3, 4, 4, 4, 4});
      Element*  t2 = new Transformation<u_int16_t>({3, 1, 3, 3, 3});
      word_type w1, w2;
      S.factorisation(w1, S.position(t1));
      S.factorisation(w2, S.position(t2));
      std::vector<relation_type> extra({relation_type(w1, w2)});
      ToddCoxeter                tc(
          congruence_type::LEFT, &S, extra, ToddCoxeter::policy::use_relations);

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

    TEST_CASE(
        "ToddCoxeter 17: right congruence on transformation semigroup size 88 ",
        "[quick][todd-coxeter][17]") {
      std::vector<Element*> gens
          = {new Transformation<u_int16_t>({1, 3, 4, 2, 3}),
             new Transformation<u_int16_t>({3, 2, 1, 3, 3})};
      Semigroup<> S = Semigroup<>(gens);
      REPORTER.set_report(REPORT);
      delete_gens(gens);

      REQUIRE(S.size() == 88);
      REQUIRE(S.nrrules() == 18);
      REQUIRE(S.degree() == 5);
      Element*  t1 = new Transformation<u_int16_t>({3, 4, 4, 4, 4});
      Element*  t2 = new Transformation<u_int16_t>({3, 1, 3, 3, 3});
      word_type w1, w2;
      S.factorisation(w1, S.position(t1));
      S.factorisation(w2, S.position(t2));
      std::vector<relation_type> extra({relation_type(w1, w2)});
      ToddCoxeter                tc(congruence_type::RIGHT,
                     &S,
                     extra,
                     ToddCoxeter::policy::use_relations);

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

    TEST_CASE("ToddCoxeter 18: finite fp-semigroup, dihedral group of order 6",
              "[quick][todd-coxeter][18]") {
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

      ToddCoxeter tc(congruence_type::TWOSIDED, 5, rels, extra);
      REQUIRE(tc.nr_classes() == 6);
      REQUIRE(tc.word_to_class_index({1}) == tc.word_to_class_index({2}));
    }

    TEST_CASE("ToddCoxeter 19: finite fp-semigroup, size 16",
              "[quick][todd-coxeter][19]") {
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
      ToddCoxeter                tc(congruence_type::TWOSIDED, 4, rels, extra);

      REQUIRE(tc.nr_classes() == 16);
      REQUIRE(tc.word_to_class_index({2}) == tc.word_to_class_index({3}));
    }

    TEST_CASE("ToddCoxeter 20: finite fp-semigroup, size 16",
              "[quick][todd-coxeter][20]") {
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

      ToddCoxeter tc(congruence_type::TWOSIDED, 11, rels, extra);

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

    TEST_CASE("ToddCoxeter 21: test prefilling of the table manually",
              "[quick][todd-coxeter][21]") {
      REPORTER.set_report(REPORT);
      std::vector<Element*> gens
          = {new Transformation<u_int16_t>({7, 3, 5, 3, 4, 2, 7, 7}),
             new Transformation<u_int16_t>({3, 6, 3, 4, 0, 6, 0, 7})};
      Semigroup<> S = Semigroup<>(gens);
      delete_gens(gens);

      // Copy the right Cayley graph of S for prefilling
      Semigroup<>::cayley_graph_type const* right = S.right_cayley_graph_copy();
      RecVec<size_t>                        table(S.nrgens(), 1, UNDEFINED);
      table.append(*right);
      delete right;
      // TODO move this stuff into prefill
      size_t j = 1;
      std::for_each(table.begin(),
                    table.begin() + table.nr_cols(),
                    [&j](size_t& i) { i = j++; });
      std::for_each(
          table.begin() + table.nr_cols(), table.end(), [](size_t& i) { ++i; });

      ToddCoxeter tc(congruence_type::TWOSIDED, 2, {}, {});
      REQUIRE(tc.get_policy() == ToddCoxeter::policy::none);
      tc.prefill(table);
      REQUIRE(!tc.is_quotient_obviously_infinite());
      REQUIRE(tc.nr_classes() == S.size());
    }

    TEST_CASE("ToddCoxeter 22: test packing phase",
              "[quick][todd-coxeter][22]") {
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

      ToddCoxeter tc1(
          congruence_type::TWOSIDED, 2, rels, std::vector<relation_type>());
      tc1.set_pack(10);
      REQUIRE(tc1.nr_classes() == 78);

      ToddCoxeter tc2(
          congruence_type::LEFT, 2, rels, std::vector<relation_type>());
      tc2.set_pack(10);
      REQUIRE(tc2.nr_classes() == 78);
    }

    /*    TEST_CASE("ToddCoxeter 23: test validate",
       "[quick][todd-coxeter][23]") { REPORTER.set_report(REPORT);

          ToddCoxeter tc(congruence_type::TWOSIDED);
          tc.set_nr_generators(2);
          tc.add_relation({0}, {1});
          REQUIRE(tc.validate_relations());

          tc.add_pair({1, 1}, {1});
          REQUIRE(tc.validate_relations());

          // FIXME The following fail in an assertion, but should fail in an
          // exception.
          //
          // tc.add_pair({1}, {2});
          // REQUIRE(!tc.validate_relations());

          // tc.add_relation({1}, {2});
          // REQUIRE(!tc.validate_relations());
        }*/

    TEST_CASE("ToddCoxeter 24: non-trivial left congruence constructed with "
              "semigroup",
              "[quick][todd-coxeter][24]") {
      REPORTER.set_report(REPORT);

      std::vector<Element*> gens
          = {new Transformation<u_int16_t>({1, 3, 4, 2, 3}),
             new Transformation<u_int16_t>({3, 2, 1, 3, 3})};
      Semigroup<> S = Semigroup<>(gens);
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

      std::vector<relation_type> extra({relation_type(w1, w2)});
      ToddCoxeter                tc(congruence_type::LEFT,
                     &S,
                     extra,
                     ToddCoxeter::policy::use_cayley_graph);
      REQUIRE(tc.nr_classes() == 69);
    }

    TEST_CASE("ToddCoxeter 25: 2-sided congruence on free semigroup",
              "[quick][todd-coxeter][25]") {
      REPORTER.set_report(true);
      ToddCoxeter tc(congruence_type::TWOSIDED, 1, {});
      REQUIRE(tc.contains({0, 0}, {0, 0}));
      REQUIRE_THROWS_AS(!tc.contains({0, 0}, {0}), LibsemigroupsException);
    }

   /* TEST_CASE("ToddCoxeter 26: add_relations after constructed from semigroup",
              "[quick][todd-coxeter][26]") {
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
      Element*  t3 = new Transformation<u_int16_t>({1, 3, 1, 3, 3});
      Element*  t4 = new Transformation<u_int16_t>({4, 2, 4, 4, 2});
      word_type w1, w2, w3, w4;
      S.factorisation(w1, S.position(t1));
      S.factorisation(w2, S.position(t2));
      S.factorisation(w3, S.position(t3));
      S.factorisation(w4, S.position(t4));

      delete t1;
      delete t2;
      delete t3;
      delete t4;

      ToddCoxeter tc1(congruence_type::TWOSIDED,
                      &S,
                      {},
                      ToddCoxeter::policy::use_cayley_graph);
      tc1.add_relation(w1, w2);

      REQUIRE(tc1.size() == 21);
      REQUIRE(tc1.size() == tc1.isomorphic_non_fp_semigroup()->size());
      REQUIRE(tc1.equal_to(w3, w4));
      REQUIRE(tc1.normal_form(w3) == tc1.normal_form(w4));

      ToddCoxeter tc2(congruence_type::TWOSIDED,
                      &S,
                      {},
                      ToddCoxeter::policy::use_relations);
      tc2.add_relation(w1, w2);

      REQUIRE(tc2.size() == 21);
      REQUIRE(tc2.size() == tc2.isomorphic_non_fp_semigroup()->size());
      REQUIRE(tc2.equal_to(w3, w4));
      REQUIRE(tc2.normal_form(w3) == tc2.normal_form(w4));
    }*/

    TEST_CASE("ToddCoxeter 27: calling run when obviously infinite",
              "[quick][todd-coxeter][27]") {
      ToddCoxeter tc(congruence_type::TWOSIDED, 5, {});
      REQUIRE_THROWS_AS(tc.run(), std::runtime_error);
    }
  }  // namespace todd_coxeter
}  // namespace libsemigroups
