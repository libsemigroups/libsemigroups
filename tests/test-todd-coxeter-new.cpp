//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019-2023 James D. Mitchell
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

// The purpose of this file is to test the ToddCoxeter classes.

#define CATCH_CONFIG_ENABLE_PAIR_STRINGMAKER

#include "catch.hpp"      // for TEST_CASE
#include "test-main.hpp"  // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/bmat8.hpp"
#include "libsemigroups/fpsemi-examples.hpp"  // for dual_symmetric_...
#include "libsemigroups/froidure-pin.hpp"
#include "libsemigroups/make-present.hpp"       // for Presentation
#include "libsemigroups/make-todd-coxeter.hpp"  // for cbegin_wislo
#include "libsemigroups/obvinf.hpp"             // for is_obviously_infinite
#include "libsemigroups/present.hpp"            // for Presentation
#include "libsemigroups/report.hpp"             // for ReportGuard
#include "libsemigroups/todd-coxeter-new.hpp"   // for ToddCoxeter
#include "libsemigroups/transf.hpp"             // for Transf
#include "libsemigroups/wilo.hpp"               // for cbegin_wilo
#include "libsemigroups/wislo.hpp"              // for cbegin_wislo

#include "libsemigroups/make-froidure-pin.hpp"  // for make
#include "libsemigroups/tce.hpp"                // for TCE

namespace libsemigroups {

  using TCE         = v3::detail::TCE;
  using KnuthBendix = fpsemigroup::KnuthBendix;

  struct LibsemigroupsException;  // Forward declaration
  congruence_kind constexpr twosided = congruence_kind::twosided;
  congruence_kind constexpr left     = congruence_kind::left;
  congruence_kind constexpr right    = congruence_kind::right;

  namespace {
    void section_felsch(ToddCoxeter& tc) {
      SECTION("Felsch + no standardisation") {
        tc.run();
        // var.strategy(options::strategy::felsch);
      }
    }

    void check_complete_compatible(ToddCoxeter& tc) {
      auto const& p = tc.presentation();
      tc.run();
      REQUIRE(todd_coxeter_digraph::complete(tc.word_graph()));
      REQUIRE(todd_coxeter_digraph::compatible(
          tc.word_graph(), p.rules.cbegin(), p.rules.cend()));
      tc.shrink_to_fit();
      REQUIRE(todd_coxeter_digraph::complete(tc.word_graph()));
      REQUIRE(todd_coxeter_digraph::compatible(
          tc.word_graph(), p.rules.cbegin(), p.rules.cend()));
    }

    void check_standardize(ToddCoxeter& tc) {
      using node_type = typename ToddCoxeter::node_type;
      order old_val   = tc.standardization_order();

      for (auto val : {order::shortlex, order::lex, order::recursive}) {
        tc.run();
        tc.standardize(val);
        REQUIRE(tc.is_standardized(val));
        REQUIRE(tc.is_standardized());
        REQUIRE(tc.standardization_order() == val);
      }
      {
        tc.standardize(order::shortlex);
        size_t const n = tc.presentation().alphabet().size();
        auto         first
            = cbegin_wislo(n, {0}, word_type(tc.number_of_classes() + 1, 0));
        auto last
            = cend_wislo(n, {0}, word_type(tc.number_of_classes() + 1, 0));

        std::unordered_map<node_type, word_type> map;
        for (auto it = first; it != last; ++it) {
          node_type n = action_digraph_helper::follow_path_nc(
              tc.word_graph(), 0, it->cbegin(), it->cend());
          REQUIRE(n != UNDEFINED);
          if (n != 0) {
            word_type w = *it;
            if (tc.kind() == congruence_kind::left) {
              std::reverse(w.begin(), w.end());
            }
            map.emplace(n - 1, std::move(w));
            if (map.size() == tc.number_of_classes()) {
              break;
            }
          }
        }

        std::vector<word_type> nf(todd_coxeter::cbegin_normal_forms(tc),
                                  todd_coxeter::cend_normal_forms(tc));

        for (auto const& p : map) {
          REQUIRE(nf[p.first] == p.second);
        }
      }
      {
        tc.standardize(order::lex);
        size_t const n     = tc.presentation().alphabet().size();
        auto         first = cbegin_wilo(n,
                                 tc.number_of_classes() + 1,
                                         {0},
                                 word_type(tc.number_of_classes() + 1, 0));
        auto         last  = cend_wilo(n,
                              tc.number_of_classes() + 1,
                                       {0},
                              word_type(tc.number_of_classes() + 1, 0));

        std::unordered_map<node_type, word_type> map;
        for (auto it = first; it != last; ++it) {
          node_type n = action_digraph_helper::follow_path_nc(
              tc.word_graph(), 0, it->cbegin(), it->cend());
          REQUIRE(n != UNDEFINED);
          if (n != 0) {
            word_type w = *it;
            if (tc.kind() == congruence_kind::left) {
              std::reverse(w.begin(), w.end());
            }
            map.emplace(n - 1, std::move(w));
            if (map.size() == tc.number_of_classes()) {
              break;
            }
          }
        }

        std::vector<word_type> nf(todd_coxeter::cbegin_normal_forms(tc),
                                  todd_coxeter::cend_normal_forms(tc));

        for (auto const& p : map) {
          REQUIRE(nf[p.first] == p.second);
        }
      }

      tc.standardize(old_val);
      // TODO use these for check_normal_forms
      //   REQUIRE(std::all_of(
      //       todd_coxeter::cbegin_normal_forms(tc),
      //       todd_coxeter::cend_normal_forms(tc),
      //       [&tc](word_type const& w) {
      //         return w == *todd_coxeter::cbegin_class(tc, w, 0, w.size() +
      //         1);
      //       }));
      // REQUIRE(std::is_sorted(todd_coxeter::cbegin_normal_forms(tc),
      //                        todd_coxeter::cend_normal_forms(tc),
      //                        ShortLexCompare{}));
      // tc.standardize(order::lex);
      // REQUIRE(std::is_sorted(todd_coxeter::cbegin_normal_forms(tc),
      //                        todd_coxeter::cend_normal_forms(tc),
      //                        LexicographicalCompare{}));
      // tc.standardize(order::recursive);
      // REQUIRE(std::is_sorted(todd_coxeter::cbegin_normal_forms(tc),
      //                        todd_coxeter::cend_normal_forms(tc),
      //                        RecursivePathCompare{}));
    }
  }  // namespace

  LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                          "000",
                          "small 2-sided congruence",
                          "[todd-coxeter][quick]") {
    auto rg = ReportGuard(false);

    Presentation<word_type> p;
    p.alphabet(2);
    presentation::add_rule_and_check(p, {0, 0, 0}, {0});
    presentation::add_rule_and_check(p, {1, 1, 1, 1}, {1});
    presentation::add_rule_and_check(p, {0, 1, 0, 1}, {0, 0});
    ToddCoxeter tc(twosided, p);

    section_felsch(tc);

    REQUIRE(tc.number_of_classes() == 27);

    check_complete_compatible(tc);
    check_standardize(tc);

    // check_hlt(tc);
    // check_random(tc);
    // check_Rc_style(tc);
    // check_R_over_C_style(tc);
    // check_CR_style(tc);
    // check_Cr_style(tc);

    auto words = std::vector<word_type>(
        todd_coxeter::cbegin_class(tc, 1, 0, 10), todd_coxeter::cend_class(tc));
    REQUIRE(words
            == std::vector<word_type>({word_type({1}),
                                       word_type({1, 1, 1, 1}),
                                       word_type({1, 1, 1, 1, 1, 1, 1})}));

    words = std::vector<word_type>(
        todd_coxeter::cbegin_class(tc, word_type({1, 1, 1, 1}), 0, 10),
        todd_coxeter::cend_class(tc));
    REQUIRE(words
            == std::vector<word_type>({word_type({1}),
                                       word_type({1, 1, 1, 1}),
                                       word_type({1, 1, 1, 1, 1, 1, 1})}));

    for (size_t i = 0; i < tc.number_of_classes(); ++i) {
      REQUIRE(todd_coxeter::number_of_words(tc, i) == POSITIVE_INFINITY);
    }
    REQUIRE(tc.word_to_class_index(words[0]) == 1);
    REQUIRE(
        std::all_of(words.cbegin(), words.cend(), [&tc](word_type const& w) {
          return tc.word_to_class_index(w) == 1;
        }));
  }

  LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                          "001",
                          "small 2-sided congruence",
                          "[no-valgrind][todd-coxeter][quick]") {
    auto rg = ReportGuard(false);

    Presentation<word_type> p;
    p.alphabet(2);
    presentation::add_rule_and_check(p, {0, 0, 0}, {0});
    presentation::add_rule_and_check(p, {0}, {1, 1});
    ToddCoxeter tc(twosided, p);

    section_felsch(tc);

    // check_hlt(tc);
    // check_random(tc);
    // check_Rc_style(tc);
    // check_R_over_C_style(tc);
    // check_CR_style(tc);
    // check_Cr_style(tc);

    REQUIRE(tc.number_of_classes() == 5);
    REQUIRE(tc.finished());
    REQUIRE(!tc.is_standardized(order::shortlex));

    REQUIRE(tc.word_to_class_index({0, 0, 1})
            == tc.word_to_class_index({0, 0, 0, 0, 1}));
    REQUIRE(tc.word_to_class_index({0, 1, 1, 0, 0, 1})
            == tc.word_to_class_index({0, 0, 0, 0, 1}));

    REQUIRE(tc.word_to_class_index({0, 0, 0}) != tc.word_to_class_index({1}));

    tc.standardize(order::shortlex);
    REQUIRE(tc.class_index_to_word(0) == word_type({0}));
    REQUIRE(tc.class_index_to_word(1) == word_type({1}));
    REQUIRE(tc.class_index_to_word(2) == word_type({0, 0}));
    tc.standardize(order::lex);
    REQUIRE(tc.is_standardized(order::lex));
    REQUIRE(tc.is_standardized());
    REQUIRE(!tc.is_standardized(order::shortlex));

    REQUIRE(tc.class_index_to_word(0) == word_type({0}));
    REQUIRE(tc.class_index_to_word(1) == word_type({0, 0}));
    REQUIRE(tc.class_index_to_word(2) == word_type({0, 0, 1}));
    REQUIRE(tc.class_index_to_word(3) == word_type({0, 0, 1, 0}));
    REQUIRE(tc.word_to_class_index(word_type({0, 0, 0, 1})) == 3);
    REQUIRE(tc.class_index_to_word(4) == word_type({1}));
    REQUIRE(tc.word_to_class_index(tc.class_index_to_word(0)) == 0);
    REQUIRE(tc.word_to_class_index(tc.class_index_to_word(1)) == 1);
    REQUIRE(tc.word_to_class_index(tc.class_index_to_word(2)) == 2);
    REQUIRE(tc.word_to_class_index(tc.class_index_to_word(3)) == 3);
    REQUIRE(tc.word_to_class_index(tc.class_index_to_word(4)) == 4);
    REQUIRE(tc.word_to_class_index({0, 1}) == 3);
    REQUIRE(LexicographicalCompare{}({0, 0, 1}, {0, 1}));

    REQUIRE(std::is_sorted(todd_coxeter::cbegin_normal_forms(tc),
                           todd_coxeter::cend_normal_forms(tc),
                           LexicographicalCompare{}));

    tc.standardize(order::shortlex);
    REQUIRE(tc.is_standardized(order::shortlex));
    REQUIRE(std::vector<word_type>(todd_coxeter::cbegin_normal_forms(tc),
                                   todd_coxeter::cend_normal_forms(tc))
            == std::vector<word_type>({{0}, {1}, {0, 0}, {0, 1}, {0, 0, 1}}));
    REQUIRE(tc.word_to_class_index(tc.class_index_to_word(0)) == 0);
    REQUIRE(tc.word_to_class_index(tc.class_index_to_word(1)) == 1);
    REQUIRE(tc.word_to_class_index(tc.class_index_to_word(2)) == 2);
    REQUIRE(tc.word_to_class_index(tc.class_index_to_word(3)) == 3);
    REQUIRE(tc.word_to_class_index(tc.class_index_to_word(4)) == 4);
    REQUIRE(std::is_sorted(todd_coxeter::cbegin_normal_forms(tc),
                           todd_coxeter::cend_normal_forms(tc),
                           ShortLexCompare{}));

    auto nf = std::vector<word_type>(todd_coxeter::cbegin_normal_forms(tc),
                                     todd_coxeter::cend_normal_forms(tc));
    REQUIRE(nf
            == std::vector<word_type>({{0}, {1}, {0, 0}, {0, 1}, {0, 0, 1}}));
    REQUIRE(std::all_of(nf.begin(), nf.end(), [&tc](word_type& w) {
      return w == *todd_coxeter::cbegin_class(tc, w, 0, w.size() + 1);
    }));

    // TODO implement cbegin/cend_wirpo (words in recursive path order
    // for (size_t i = 2; i < 6; ++i) {
    //   for (size_t j = 2; j < 10 - i; ++j) {
    //     auto v
    //         = std::vector<word_type>(cbegin_wislo(i, {0}, word_type(j + 1,
    //         0)),
    //                                  cend_wislo(i, {0}, word_type(j + 1,
    //                                  0)));
    //     std::sort(v.begin(), v.end(), RecursivePathCompare<word_type>{});
    //     REQUIRE(v == recursive_path_words(i, j));
    //   }
    // }

    tc.standardize(order::recursive);
    REQUIRE(tc.is_standardized());

    REQUIRE(tc.class_index_to_word(0) == word_type({0}));
    REQUIRE(tc.class_index_to_word(1) == word_type({0, 0}));
    REQUIRE(tc.class_index_to_word(2) == word_type({1}));
    REQUIRE(tc.class_index_to_word(3) == word_type({1, 0}));
    REQUIRE(tc.class_index_to_word(4) == word_type({1, 0, 0}));
    REQUIRE(std::is_sorted(todd_coxeter::cbegin_normal_forms(tc),
                           todd_coxeter::cend_normal_forms(tc),
                           RecursivePathCompare{}));
  }

  // Felsch is actually faster here!
  LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                          "002",
                          "Example 6.6 in Sims (see also KnuthBendix 013)",
                          "[todd-coxeter][standard]") {
    auto rg = ReportGuard(false);

    Presentation<word_type> p;
    p.alphabet(4);
    presentation::add_rule_and_check(p, {0, 0}, {0});
    presentation::add_rule_and_check(p, {1, 0}, {1});
    presentation::add_rule_and_check(p, {0, 1}, {1});
    presentation::add_rule_and_check(p, {2, 0}, {2});
    presentation::add_rule_and_check(p, {0, 2}, {2});
    presentation::add_rule_and_check(p, {3, 0}, {3});
    presentation::add_rule_and_check(p, {0, 3}, {3});
    presentation::add_rule_and_check(p, {1, 1}, {0});
    presentation::add_rule_and_check(p, {2, 3}, {0});
    presentation::add_rule_and_check(p, {2, 2, 2}, {0});
    presentation::add_rule_and_check(
        p, {1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2}, {0});
    presentation::add_rule_and_check(p,
                                     {1, 2, 1, 3, 1, 2, 1, 3, 1, 2, 1,
                                      3, 1, 2, 1, 3, 1, 2, 1, 3, 1, 2,
                                      1, 3, 1, 2, 1, 3, 1, 2, 1, 3},
                                     {0});

    ToddCoxeter tc(twosided, p);

    section_felsch(tc);

    // check_hlt(tc);
    //  section_felsch(tc);
    // check_random(tc);
    // check_Rc_style(tc);
    // check_R_over_C_style(tc);
    // check_CR_style(tc);
    // check_Cr_style(tc);
    REQUIRE(tc.number_of_classes() == 10'752);
    // check_complete_compatible(tc);

    REQUIRE(tc.finished());

    tc.standardize(order::recursive);
    REQUIRE(std::is_sorted(todd_coxeter::cbegin_normal_forms(tc),
                           todd_coxeter::cend_normal_forms(tc),
                           RecursivePathCompare{}));
    REQUIRE(std::vector<word_type>(todd_coxeter::cbegin_normal_forms(tc),
                                   todd_coxeter::cbegin_normal_forms(tc) + 10)
            == std::vector<word_type>({{{0},
                                        {1},
                                        {2},
                                        {2, 1},
                                        {1, 2},
                                        {1, 2, 1},
                                        {2, 2},
                                        {2, 2, 1},
                                        {2, 1, 2},
                                        {2, 1, 2, 1}}}));

    tc.standardize(order::lex);
    for (size_t c = 0; c < tc.number_of_classes(); ++c) {
      REQUIRE(tc.word_to_class_index(tc.class_index_to_word(c)) == c);
    }
    REQUIRE(std::is_sorted(todd_coxeter::cbegin_normal_forms(tc),
                           todd_coxeter::cend_normal_forms(tc),
                           LexicographicalCompare{}));
    REQUIRE(std::vector<word_type>(todd_coxeter::cbegin_normal_forms(tc),
                                   todd_coxeter::cbegin_normal_forms(tc) + 10)
            == std::vector<word_type>({{0},
                                       {0, 1},
                                       {0, 1, 2},
                                       {0, 1, 2, 1},
                                       {0, 1, 2, 1, 2},
                                       {0, 1, 2, 1, 2, 1},
                                       {0, 1, 2, 1, 2, 1, 2},
                                       {0, 1, 2, 1, 2, 1, 2, 1},
                                       {0, 1, 2, 1, 2, 1, 2, 1, 2},
                                       {0, 1, 2, 1, 2, 1, 2, 1, 2, 1}}));
    tc.standardize(order::shortlex);
    for (size_t c = 0; c < tc.number_of_classes(); ++c) {
      REQUIRE(tc.word_to_class_index(tc.class_index_to_word(c)) == c);
    }
    REQUIRE(std::is_sorted(todd_coxeter::cbegin_normal_forms(tc),
                           todd_coxeter::cend_normal_forms(tc),
                           ShortLexCompare{}));
    REQUIRE(std::vector<word_type>(todd_coxeter::cbegin_normal_forms(tc),
                                   todd_coxeter::cbegin_normal_forms(tc) + 10)
            == std::vector<word_type>({{0},
                                       {1},
                                       {2},
                                       {3},
                                       {1, 2},
                                       {1, 3},
                                       {2, 1},
                                       {3, 1},
                                       {1, 2, 1},
                                       {1, 3, 1}}));
  }

  LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                          "003",
                          "constructed from FroidurePin",
                          "[no-valgrind][todd-coxeter][quick][no-coverage]") {
    auto rg = ReportGuard(false);

    FroidurePin<BMat8> S(
        {BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
         BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
         BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {1, 0, 0, 1}}),
         BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}})});

    REQUIRE(S.size() == 63'904);
    auto p = make<Presentation<word_type>>(S);

    ToddCoxeter tc(twosided, std::move(p));
    tc.add_pair({0}, {1});

    section_felsch(tc);
    //  check_hlt(tc);
    //  check_random(tc);
    //  check_Rc_style(tc);
    //  check_R_over_C_style(tc);
    //  check_CR_style(tc);
    //  check_Cr_style(tc);

    // tc.random_interval(std::chrono::milliseconds(100));
    // tc.lower_bound(3);
    // tc.run();

    // check_complete_compatible(tc);
    // check_standardize(tc);

    REQUIRE(tc.number_of_classes() == 3);
    REQUIRE(tc.contains({0}, {1}));

    auto T = make<FroidurePin<TCE>>(tc);
    REQUIRE(T.size() == 3);
    REQUIRE(tc.class_index_to_word(0) == T.factorisation(0));
    REQUIRE(tc.class_index_to_word(1) == T.factorisation(1));
    REQUIRE(tc.class_index_to_word(2) == T.factorisation(2));

    REQUIRE(tc.class_index_to_word(0) == word_type({0}));
    REQUIRE(tc.class_index_to_word(1) == word_type({2}));
    REQUIRE(tc.class_index_to_word(2) == word_type({0, 0}));
    REQUIRE(tc.word_to_class_index(tc.class_index_to_word(0)) == 0);
    REQUIRE(tc.word_to_class_index(tc.class_index_to_word(1)) == 1);
    REQUIRE(tc.word_to_class_index(tc.class_index_to_word(2)) == 2);

    tc.standardize(order::lex);
    REQUIRE(tc.class_index_to_word(0) == word_type({0}));
    REQUIRE(tc.class_index_to_word(1) == word_type({0, 0}));
    REQUIRE(tc.class_index_to_word(2) == word_type({0, 0, 2}));
    REQUIRE(tc.word_to_class_index(tc.class_index_to_word(0)) == 0);
    REQUIRE(tc.word_to_class_index(tc.class_index_to_word(1)) == 1);
    REQUIRE(tc.word_to_class_index(tc.class_index_to_word(2)) == 2);

    tc.standardize(order::shortlex);
    REQUIRE(tc.class_index_to_word(0) == word_type({0}));
    REQUIRE(tc.class_index_to_word(1) == word_type({2}));
    REQUIRE(tc.class_index_to_word(2) == word_type({0, 0}));
  }

  LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                          "004",
                          "2-sided congruence from FroidurePin",
                          "[todd-coxeter][quick]") {
    auto rg = ReportGuard(false);

    using Transf = LeastTransf<5>;
    FroidurePin<Transf> S({Transf({1, 3, 4, 2, 3}), Transf({3, 2, 1, 3, 3})});

    REQUIRE(S.size() == 88);

    // Construct from Cayley graph of S
    auto tc = make<ToddCoxeter>(twosided, S);
    REQUIRE(tc.word_graph().number_of_nodes() == 89);

    tc.add_pair(S.factorisation(Transf({3, 4, 4, 4, 4})),
                S.factorisation(Transf({3, 1, 3, 3, 3})));
    REQUIRE(!tc.finished());
    tc.shrink_to_fit();  // does nothing
    REQUIRE(!tc.finished());
    tc.standardize(order::none);  // does nothing
    REQUIRE(!tc.finished());

    section_felsch(tc);

    REQUIRE(tc.number_of_classes() == 21);
    tc.shrink_to_fit();
    REQUIRE(tc.number_of_classes() == 21);
    tc.standardize(order::recursive);
    auto w = std::vector<word_type>(todd_coxeter::cbegin_normal_forms(tc),
                                    todd_coxeter::cend_normal_forms(tc));
    REQUIRE(w.size() == 21);
    REQUIRE(w
            == std::vector<word_type>({{0},
                                       {0, 0},
                                       {0, 0, 0},
                                       {0, 0, 0, 0},
                                       {1},
                                       {1, 0},
                                       {1, 0, 0},
                                       {1, 0, 0, 0},
                                       {0, 1},
                                       {0, 1, 0},
                                       {0, 1, 0, 0},
                                       {0, 1, 0, 0, 0},
                                       {0, 0, 1},
                                       {1, 1},
                                       {1, 1, 0},
                                       {1, 1, 0, 0},
                                       {1, 1, 0, 0, 0},
                                       {0, 1, 1},
                                       {0, 1, 1, 0},
                                       {0, 1, 1, 0, 0},
                                       {0, 1, 1, 0, 0, 0}}));
    REQUIRE(std::unique(w.begin(), w.end()) == w.end());
    REQUIRE(std::is_sorted(todd_coxeter::cbegin_normal_forms(tc),
                           todd_coxeter::cend_normal_forms(tc),
                           RecursivePathCompare{}));
    REQUIRE(std::all_of(
        todd_coxeter::cbegin_normal_forms(tc),
        todd_coxeter::cend_normal_forms(tc),
        [&tc](word_type const& ww) -> bool {
          return tc.class_index_to_word(tc.word_to_class_index(ww)) == ww;
        }));
  }

  LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                          "005",
                          "non-trivial two-sided from relations",
                          "[todd-coxeter][quick]") {
    auto rg = ReportGuard(false);

    Presentation<word_type> p;
    p.alphabet(3);
    presentation::add_rule_and_check(p, {0, 1}, {1, 0});
    presentation::add_rule_and_check(p, {0, 2}, {2, 2});
    presentation::add_rule_and_check(p, {0, 2}, {0});
    presentation::add_rule_and_check(p, {2, 2}, {0});
    presentation::add_rule_and_check(p, {1, 2}, {1, 2});
    presentation::add_rule_and_check(p, {1, 2}, {2, 2});
    presentation::add_rule_and_check(p, {1, 2, 2}, {1});
    presentation::add_rule_and_check(p, {1, 2}, {1});
    presentation::add_rule_and_check(p, {2, 2}, {1});
    presentation::add_rule_and_check(p, {0}, {1});

    ToddCoxeter tc(twosided, p);

    section_felsch(tc);
    // check_hlt(tc);
    // check_random(tc);
    // check_Rc_style(tc);
    // check_R_over_C_style(tc);
    // check_CR_style(tc);
    // check_Cr_style(tc);

    REQUIRE(tc.number_of_classes() == 2);
    check_standardize(tc);
  }

  LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                          "006",
                          "small right cong. on free semigroup",
                          "[todd-coxeter][quick]") {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.alphabet(2);
    presentation::add_rule_and_check(p, {0, 0, 0}, {0});
    presentation::add_rule_and_check(p, {0}, {1, 1});

    ToddCoxeter tc(right, p);

    section_felsch(tc);
    // check_hlt(tc);
    // check_random(tc);
    // check_Rc_style(tc);
    // check_R_over_C_style(tc);
    // check_CR_style(tc);
    // check_Cr_style(tc);

    REQUIRE(tc.number_of_classes() == 5);
    REQUIRE(tc.finished());
    check_standardize(tc);
  }

  LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                          "007",
                          "left cong. on free semigroup",
                          "[todd-coxeter][quick]") {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.alphabet(2);
    presentation::add_rule_and_check(p, {0, 0, 0}, {0});
    presentation::add_rule_and_check(p, {0}, {1, 1});
    {
      ToddCoxeter tc(left, p);
      // TODO uncomment tc.growth_factor(1.5);

      section_felsch(tc);
      // check_hlt(tc);
      // check_random(tc);
      // check_Rc_style(tc);
      // check_R_over_C_style(tc);
      // check_CR_style(tc);
      // check_Cr_style(tc);

      REQUIRE(!tc.is_standardized());
      REQUIRE(tc.word_to_class_index({0, 0, 1})
              == tc.word_to_class_index({0, 0, 0, 0, 1}));
      REQUIRE(tc.word_to_class_index({0, 1, 1, 0, 0, 1})
              == tc.word_to_class_index({0, 0, 0, 0, 1}));
      REQUIRE(tc.word_to_class_index({1})
              != tc.word_to_class_index({0, 0, 0, 0}));
      REQUIRE(tc.word_to_class_index({0, 0, 0})
              != tc.word_to_class_index({0, 0, 0, 0}));
      tc.standardize(order::shortlex);
      REQUIRE(tc.is_standardized());
      check_standardize(tc);
      check_complete_compatible(tc);
    }
    {
      ToddCoxeter tc(left, p);
      REQUIRE_NOTHROW(ToddCoxeter(left, tc));
    }
  }

  LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                          "008",
                          "for small fp semigroup",
                          "[todd-coxeter][quick]") {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.alphabet(2);
    presentation::add_rule_and_check(p, {0, 0, 0}, {0});
    presentation::add_rule_and_check(p, {0}, {1, 1});
    ToddCoxeter tc(twosided, p);

    section_felsch(tc);
    // check_hlt(tc);
    // check_random(tc);
    // check_Rc_style(tc);
    // check_R_over_C_style(tc);
    // check_CR_style(tc);
    // check_Cr_style(tc);

    REQUIRE(tc.word_to_class_index({0, 0, 1})
            == tc.word_to_class_index({0, 0, 0, 0, 1}));
    REQUIRE(tc.word_to_class_index({0, 1, 1, 0, 0, 1})
            == tc.word_to_class_index({0, 0, 0, 0, 1}));
    REQUIRE(tc.word_to_class_index({0, 0, 0}) != tc.word_to_class_index({1}));
    REQUIRE(tc.word_to_class_index({0, 0, 0, 0}) < tc.number_of_classes());
    tc.standardize(order::shortlex);
    check_standardize(tc);
    check_complete_compatible(tc);
  }

  // TODO move to test-make-todd-coxeter
  LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                          "009",
                          "2-sided cong. trans. semigroup",
                          "[todd-coxeter][quick]") {
    auto rg = ReportGuard(false);
    auto S  = FroidurePin<Transf<>>(
        {Transf<>({1, 3, 4, 2, 3}), Transf<>({3, 2, 1, 3, 3})});

    REQUIRE(S.size() == 88);
    REQUIRE(S.number_of_rules() == 18);

    auto p = make<Presentation<word_type>>(S);

    ToddCoxeter tc(twosided, p);
    REQUIRE(S.factorisation(Transf<>({3, 4, 4, 4, 4}))
            == word_type({0, 1, 0, 0, 0, 1, 1, 0, 0}));
    REQUIRE(S.factorisation(Transf<>({3, 1, 3, 3, 3}))
            == word_type({1, 0, 0, 0, 1}));
    tc.add_pair(S.factorisation(Transf<>({3, 4, 4, 4, 4})),
                S.factorisation(Transf<>({3, 1, 3, 3, 3})));

    section_felsch(tc);
    // check_hlt(tc);
    // check_random(tc);
    // check_Rc_style(tc);
    // check_R_over_C_style(tc);
    // check_CR_style(tc);
    // check_Cr_style(tc);

    check_complete_compatible(tc);
    check_standardize(tc);

    REQUIRE(*tc.cbegin_generating_pairs()
            == word_type({0, 1, 0, 0, 0, 1, 1, 0, 0}));
    todd_coxeter_digraph::compatible(tc.word_graph(),
                                     tc.cbegin_generating_pairs(),
                                     tc.cend_generating_pairs());
    REQUIRE(tc.number_of_classes() == 21);
    REQUIRE(tc.number_of_classes() == 21);

    REQUIRE(
        tc.word_to_class_index(S.factorisation(Transf<>({1, 3, 1, 3, 3})))
        == tc.word_to_class_index(S.factorisation(Transf<>({4, 2, 4, 4, 2}))));

    tc.standardize(order::shortlex);

    auto ntc = todd_coxeter::non_trivial_classes(
        tc, S.cbegin_normal_forms(), S.cend_normal_forms());

    REQUIRE(ntc.size() == 1);
    REQUIRE(ntc[0].size() == 68);
    REQUIRE(ntc
            == decltype(ntc)({{{0, 0, 1},
                               {1, 0, 1},
                               {0, 0, 0, 1},
                               {0, 0, 1, 0},
                               {0, 0, 1, 1},
                               {0, 1, 0, 1},
                               {1, 0, 0, 1},
                               {1, 0, 1, 0},
                               {1, 0, 1, 1},
                               {0, 0, 0, 0, 1},
                               {0, 0, 0, 1, 0},
                               {0, 0, 0, 1, 1},
                               {0, 0, 1, 0, 0},
                               {0, 0, 1, 0, 1},
                               {0, 0, 1, 1, 0},
                               {0, 1, 0, 1, 0},
                               {0, 1, 0, 1, 1},
                               {1, 0, 0, 0, 1},
                               {1, 0, 0, 1, 0},
                               {1, 0, 0, 1, 1},
                               {1, 0, 1, 0, 0},
                               {1, 0, 1, 0, 1},
                               {1, 0, 1, 1, 0},
                               {0, 0, 0, 0, 1, 0},
                               {0, 0, 0, 0, 1, 1},
                               {0, 0, 0, 1, 0, 0},
                               {0, 0, 0, 1, 0, 1},
                               {0, 0, 0, 1, 1, 0},
                               {0, 0, 1, 0, 0, 0},
                               {0, 0, 1, 1, 0, 0},
                               {0, 1, 0, 0, 0, 1},
                               {0, 1, 0, 1, 0, 0},
                               {0, 1, 0, 1, 0, 1},
                               {0, 1, 0, 1, 1, 0},
                               {1, 0, 0, 0, 1, 0},
                               {1, 0, 0, 0, 1, 1},
                               {1, 0, 0, 1, 0, 0},
                               {1, 0, 0, 1, 0, 1},
                               {1, 0, 0, 1, 1, 0},
                               {1, 0, 1, 0, 0, 0},
                               {1, 0, 1, 1, 0, 0},
                               {0, 0, 0, 0, 1, 0, 0},
                               {0, 0, 0, 0, 1, 0, 1},
                               {0, 0, 0, 0, 1, 1, 0},
                               {0, 0, 0, 1, 0, 0, 0},
                               {0, 0, 0, 1, 1, 0, 0},
                               {0, 0, 1, 0, 0, 0, 1},
                               {0, 1, 0, 0, 0, 1, 0},
                               {0, 1, 0, 0, 0, 1, 1},
                               {0, 1, 0, 1, 0, 0, 0},
                               {0, 1, 0, 1, 1, 0, 0},
                               {1, 0, 0, 0, 1, 0, 0},
                               {1, 0, 0, 0, 1, 0, 1},
                               {1, 0, 0, 0, 1, 1, 0},
                               {1, 0, 0, 1, 0, 0, 0},
                               {1, 0, 0, 1, 1, 0, 0},
                               {0, 0, 0, 0, 1, 0, 0, 0},
                               {0, 0, 0, 0, 1, 1, 0, 0},
                               {0, 0, 1, 0, 0, 0, 1, 0},
                               {0, 1, 0, 0, 0, 1, 0, 0},
                               {0, 1, 0, 0, 0, 1, 0, 1},
                               {0, 1, 0, 0, 0, 1, 1, 0},
                               {1, 0, 0, 0, 1, 0, 0, 0},
                               {1, 0, 0, 0, 1, 1, 0, 0},
                               {0, 0, 1, 0, 0, 0, 1, 0, 0},
                               {0, 0, 1, 0, 0, 0, 1, 0, 1},
                               {0, 1, 0, 0, 0, 1, 0, 0, 0},
                               {0, 1, 0, 0, 0, 1, 1, 0, 0}}}));
  }

  // TODO move to test-make-todd-coxeter.cpp
  LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                          "010",
                          "left congruence on transformation semigroup",
                          "[todd-coxeter][quick]") {
    auto rg = ReportGuard(false);
    auto S  = FroidurePin<Transf<>>(
        {Transf<>({1, 3, 4, 2, 3}), Transf<>({3, 2, 1, 3, 3})});

    REQUIRE(S.size() == 88);
    REQUIRE(S.number_of_rules() == 18);

    auto p = make<Presentation<word_type>>(S);

    ToddCoxeter tc(left, p);
    tc.add_pair(S.factorisation(Transf<>({3, 4, 4, 4, 4})),
                S.factorisation(Transf<>({3, 1, 3, 3, 3})));

    section_felsch(tc);
    // check_hlt(tc);
    // check_random(tc);
    // check_Rc_style(tc);
    // check_R_over_C_style(tc);
    // check_CR_style(tc);
    // check_Cr_style(tc);

    REQUIRE(
        tc.word_to_class_index(S.factorisation(Transf<>({1, 3, 1, 3, 3})))
        != tc.word_to_class_index(S.factorisation(Transf<>({4, 2, 4, 4, 2}))));

    REQUIRE(tc.number_of_classes() == 69);
    REQUIRE(tc.number_of_classes() == 69);
    auto ntc = todd_coxeter::non_trivial_classes(
        tc, S.cbegin_normal_forms(), S.cend_normal_forms());
    REQUIRE(ntc.size() == 1);
    REQUIRE(ntc[0].size() == 20);
    REQUIRE(ntc
            == decltype(ntc)({{{0, 0, 1},
                               {1, 0, 1},
                               {0, 0, 0, 1},
                               {0, 1, 0, 1},
                               {1, 0, 0, 1},
                               {0, 0, 0, 0, 1},
                               {0, 0, 1, 0, 1},
                               {1, 0, 0, 0, 1},
                               {1, 0, 1, 0, 1},
                               {0, 0, 0, 1, 0, 1},
                               {0, 1, 0, 0, 0, 1},
                               {0, 1, 0, 1, 0, 1},
                               {1, 0, 0, 1, 0, 1},
                               {0, 0, 0, 0, 1, 0, 1},
                               {0, 0, 1, 0, 0, 0, 1},
                               {1, 0, 0, 0, 1, 0, 1},
                               {0, 1, 0, 0, 0, 1, 0, 1},
                               {0, 0, 1, 0, 0, 0, 1, 0, 0},
                               {0, 0, 1, 0, 0, 0, 1, 0, 1},
                               {0, 1, 0, 0, 0, 1, 1, 0, 0}}}));
    REQUIRE(
        std::all_of(ntc[0].cbegin(), ntc[0].cend(), [&ntc, &tc](auto const& w) {
          return tc.contains(w, ntc[0][0]);
        }));
  }

  LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                          "011",
                          "right cong. trans. semigroup",
                          "[todd-coxeter][quick]") {
    auto rg = ReportGuard(false);
    auto S  = FroidurePin<Transf<>>(
        {Transf<>({1, 3, 4, 2, 3}), Transf<>({3, 2, 1, 3, 3})});

    REQUIRE(S.size() == 88);
    REQUIRE(S.number_of_rules() == 18);

    ToddCoxeter tc(right, make<Presentation<word_type>>(S));
    tc.add_pair(S.factorisation(Transf<>({3, 4, 4, 4, 4})),
                S.factorisation(Transf<>({3, 1, 3, 3, 3})));

    section_felsch(tc);
    // check_hlt(tc);
    // check_random(tc);
    // check_Rc_style(tc);
    // check_R_over_C_style(tc);
    // check_CR_style(tc);
    // check_Cr_style(tc);

    REQUIRE(tc.number_of_classes() == 72);
    REQUIRE(tc.number_of_classes() == 72);

    REQUIRE(
        tc.word_to_class_index(S.factorisation(Transf<>({1, 3, 1, 3, 3})))
        != tc.word_to_class_index(S.factorisation(Transf<>({4, 2, 4, 4, 2}))));

    REQUIRE(
        tc.word_to_class_index(S.factorisation(Transf<>({1, 3, 3, 3, 3})))
        != tc.word_to_class_index(S.factorisation(Transf<>({4, 2, 4, 4, 2}))));
    REQUIRE(
        tc.word_to_class_index(S.factorisation(Transf<>({2, 4, 2, 2, 2})))
        == tc.word_to_class_index(S.factorisation(Transf<>({2, 3, 3, 3, 3}))));
    REQUIRE(
        tc.word_to_class_index(S.factorisation(Transf<>({1, 3, 3, 3, 3})))
        != tc.word_to_class_index(S.factorisation(Transf<>({2, 3, 3, 3, 3}))));

    auto ntc = todd_coxeter::non_trivial_classes(
        tc, S.cbegin_normal_forms(), S.cend_normal_forms());
    REQUIRE(ntc.size() == 4);
    std::vector<size_t> sizes(ntc.size(), 0);
    std::transform(ntc.cbegin(),
                   ntc.cend(),
                   sizes.begin(),
                   std::mem_fn(&std::vector<word_type>::size));
    std::sort(sizes.begin(), sizes.end());
    REQUIRE(sizes == std::vector<size_t>({3, 5, 5, 7}));
    word_type w3, w4, w5, w6;
    S.factorisation(w3, S.position(Transf<>({1, 3, 3, 3, 3})));
    S.factorisation(w4, S.position(Transf<>({4, 2, 4, 4, 2})));
    S.factorisation(w5, S.position(Transf<>({2, 4, 2, 2, 2})));
    S.factorisation(w6, S.position(Transf<>({2, 3, 3, 3, 3})));
    REQUIRE(tc.word_to_class_index(w3) != tc.word_to_class_index(w4));
    REQUIRE(tc.word_to_class_index(w5) == tc.word_to_class_index(w6));
  }

  LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                          "012",
                          "trans. semigroup (size 88)",
                          "[todd-coxeter][quick]") {
    auto rg = ReportGuard(false);

    FroidurePin<Transf<>> S;
    S.add_generator(Transf<>({1, 3, 4, 2, 3}));
    S.add_generator(Transf<>({3, 2, 1, 3, 3}));

    REQUIRE(S.size() == 88);
    REQUIRE(S.number_of_rules() == 18);

    ToddCoxeter tc = make<ToddCoxeter>(twosided, S);  // use the Cayley graph

    word_type w1, w2;
    S.factorisation(w1, S.position(Transf<>({3, 4, 4, 4, 4})));
    S.factorisation(w2, S.position(Transf<>({3, 1, 3, 3, 3})));

    tc.add_pair(w1, w2);

    section_felsch(tc);

    // check_hlt_no_save(tc);
    // check_hlt_save_throws(tc);
    // check_random(tc);

    REQUIRE(tc.number_of_classes() == 21);
    REQUIRE(tc.number_of_classes() == 21);
    word_type w3, w4;
    S.factorisation(w3, S.position(Transf<>({1, 3, 1, 3, 3})));
    S.factorisation(w4, S.position(Transf<>({4, 2, 4, 4, 2})));
    REQUIRE(tc.word_to_class_index(w3) == tc.word_to_class_index(w4));
  }

  LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                          "015",
                          "finite fp-semigroup, dihedral group of order 6 ",
                          "[todd-coxeter][quick]") {
    auto rg = ReportGuard(false);

    Presentation<word_type> p;
    p.alphabet(5);
    presentation::add_identity_rules(p, 0);
    presentation::add_inverse_rules(p, {0, 2, 1, 4, 3}, 0);

    presentation::add_rule_and_check(p, {2, 2}, {0});
    presentation::add_rule_and_check(p, {1, 4, 2, 3, 3}, {0});
    presentation::add_rule_and_check(p, {4, 4, 4}, {0});

    ToddCoxeter tc(twosided, p);
    section_felsch(tc);

    // check_hlt(tc);
    // check_random(tc);
    // check_Rc_style(tc);
    // check_R_over_C_style(tc);
    // check_CR_style(tc);
    // check_Cr_style(tc);

    REQUIRE(tc.number_of_classes() == 6);
    REQUIRE(tc.word_to_class_index({1}) == tc.word_to_class_index({2}));
  }

  LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                          "016",
                          "finite fp-semigroup, size 16",
                          "[todd-coxeter][quick]") {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.alphabet(4);
    presentation::add_rule_and_check(p, {3}, {2});
    presentation::add_rule_and_check(p, {0, 3}, {0, 2});
    presentation::add_rule_and_check(p, {1, 1}, {1});
    presentation::add_rule_and_check(p, {1, 3}, {1, 2});
    presentation::add_rule_and_check(p, {2, 1}, {2});
    presentation::add_rule_and_check(p, {2, 2}, {2});
    presentation::add_rule_and_check(p, {2, 3}, {2});
    presentation::add_rule_and_check(p, {0, 0, 0}, {0});
    presentation::add_rule_and_check(p, {0, 0, 1}, {1});
    presentation::add_rule_and_check(p, {0, 0, 2}, {2});
    presentation::add_rule_and_check(p, {0, 1, 2}, {1, 2});
    presentation::add_rule_and_check(p, {1, 0, 0}, {1});
    presentation::add_rule_and_check(p, {1, 0, 2}, {0, 2});
    presentation::add_rule_and_check(p, {2, 0, 0}, {2});
    presentation::add_rule_and_check(p, {0, 1, 0, 1}, {1, 0, 1});
    presentation::add_rule_and_check(p, {0, 2, 0, 2}, {2, 0, 2});
    presentation::add_rule_and_check(p, {1, 0, 1, 0}, {1, 0, 1});
    presentation::add_rule_and_check(p, {1, 2, 0, 1}, {1, 0, 1});
    presentation::add_rule_and_check(p, {1, 2, 0, 2}, {2, 0, 2});
    presentation::add_rule_and_check(p, {2, 0, 1, 0}, {2, 0, 1});
    presentation::add_rule_and_check(p, {2, 0, 2, 0}, {2, 0, 2});

    ToddCoxeter tc(twosided, p);

    section_felsch(tc);
    // check_hlt(tc);
    // check_random(tc);
    // check_Rc_style(tc);
    // check_R_over_C_style(tc);
    // check_CR_style(tc);
    // check_Cr_style(tc);

    REQUIRE(tc.number_of_classes() == 16);
    REQUIRE(tc.word_to_class_index({2}) == tc.word_to_class_index({3}));
  }

  LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                          "017",
                          "finite fp-semigroup, size 16",
                          "[todd-coxeter][quick]") {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.alphabet(11);
    presentation::add_rule_and_check(p, {2}, {1});
    presentation::add_rule_and_check(p, {4}, {3});
    presentation::add_rule_and_check(p, {5}, {0});
    presentation::add_rule_and_check(p, {6}, {3});
    presentation::add_rule_and_check(p, {7}, {1});
    presentation::add_rule_and_check(p, {8}, {3});
    presentation::add_rule_and_check(p, {9}, {3});
    presentation::add_rule_and_check(p, {10}, {0});
    presentation::add_rule_and_check(p, {0, 2}, {0, 1});
    presentation::add_rule_and_check(p, {0, 4}, {0, 3});
    presentation::add_rule_and_check(p, {0, 5}, {0, 0});
    presentation::add_rule_and_check(p, {0, 6}, {0, 3});
    presentation::add_rule_and_check(p, {0, 7}, {0, 1});
    presentation::add_rule_and_check(p, {0, 8}, {0, 3});
    presentation::add_rule_and_check(p, {0, 9}, {0, 3});
    presentation::add_rule_and_check(p, {0, 10}, {0, 0});
    presentation::add_rule_and_check(p, {1, 1}, {1});
    presentation::add_rule_and_check(p, {1, 2}, {1});
    presentation::add_rule_and_check(p, {1, 4}, {1, 3});
    presentation::add_rule_and_check(p, {1, 5}, {1, 0});
    presentation::add_rule_and_check(p, {1, 6}, {1, 3});
    presentation::add_rule_and_check(p, {1, 7}, {1});
    presentation::add_rule_and_check(p, {1, 8}, {1, 3});
    presentation::add_rule_and_check(p, {1, 9}, {1, 3});
    presentation::add_rule_and_check(p, {1, 10}, {1, 0});
    presentation::add_rule_and_check(p, {3, 1}, {3});
    presentation::add_rule_and_check(p, {3, 2}, {3});
    presentation::add_rule_and_check(p, {3, 3}, {3});
    presentation::add_rule_and_check(p, {3, 4}, {3});
    presentation::add_rule_and_check(p, {3, 5}, {3, 0});
    presentation::add_rule_and_check(p, {3, 6}, {3});
    presentation::add_rule_and_check(p, {3, 7}, {3});
    presentation::add_rule_and_check(p, {3, 8}, {3});
    presentation::add_rule_and_check(p, {3, 9}, {3});
    presentation::add_rule_and_check(p, {3, 10}, {3, 0});
    presentation::add_rule_and_check(p, {0, 0, 0}, {0});
    presentation::add_rule_and_check(p, {0, 0, 1}, {1});
    presentation::add_rule_and_check(p, {0, 0, 3}, {3});
    presentation::add_rule_and_check(p, {0, 1, 3}, {1, 3});
    presentation::add_rule_and_check(p, {1, 0, 0}, {1});
    presentation::add_rule_and_check(p, {1, 0, 3}, {0, 3});
    presentation::add_rule_and_check(p, {3, 0, 0}, {3});
    presentation::add_rule_and_check(p, {0, 1, 0, 1}, {1, 0, 1});
    presentation::add_rule_and_check(p, {0, 3, 0, 3}, {3, 0, 3});
    presentation::add_rule_and_check(p, {1, 0, 1, 0}, {1, 0, 1});
    presentation::add_rule_and_check(p, {1, 3, 0, 1}, {1, 0, 1});
    presentation::add_rule_and_check(p, {1, 3, 0, 3}, {3, 0, 3});
    presentation::add_rule_and_check(p, {3, 0, 1, 0}, {3, 0, 1});
    presentation::add_rule_and_check(p, {3, 0, 3, 0}, {3, 0, 3});

    ToddCoxeter tc(twosided, p);

    section_felsch(tc);
    // check_hlt(tc);
    // check_random(tc);
    // check_Rc_style(tc);
    // check_R_over_C_style(tc);
    // check_CR_style(tc);
    // check_Cr_style(tc);

    REQUIRE(tc.number_of_classes() == 16);
    REQUIRE(tc.word_to_class_index({0}) == tc.word_to_class_index({5}));
    REQUIRE(tc.word_to_class_index({0}) == tc.word_to_class_index({10}));
    REQUIRE(tc.word_to_class_index({1}) == tc.word_to_class_index({2}));
    REQUIRE(tc.word_to_class_index({1}) == tc.word_to_class_index({7}));
    REQUIRE(tc.word_to_class_index({3}) == tc.word_to_class_index({4}));
    REQUIRE(tc.word_to_class_index({3}) == tc.word_to_class_index({6}));
    REQUIRE(tc.word_to_class_index({3}) == tc.word_to_class_index({8}));
    REQUIRE(tc.word_to_class_index({3}) == tc.word_to_class_index({9}));
  }

  /*
  LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                          "018",
                          "test lookahead",
                          "[todd-coxeter][quick]") {
    auto rg = ReportGuard(false);
    {
      ToddCoxeter tc(twosided);
      tc.set_number_of_generators(2);
      tc.next_lookahead(10);
      tc.add_pair({0, 0, 0}, {0});
      tc.add_pair({1, 0, 0}, {1, 0});
      tc.add_pair({1, 0, 1, 1, 1}, {1, 0});
      tc.add_pair({1, 1, 1, 1, 1}, {1, 1});
      tc.add_pair({1, 1, 0, 1, 1, 0}, {1, 0, 1, 0, 1, 1});
      tc.add_pair({0, 0, 1, 0, 1, 1, 0}, {0, 1, 0, 1, 1, 0});
      tc.add_pair({0, 0, 1, 1, 0, 1, 0}, {0, 1, 1, 0, 1, 0});
      tc.add_pair({0, 1, 0, 1, 0, 1, 0}, {1, 0, 1, 0, 1, 0});
      tc.add_pair({1, 0, 1, 0, 1, 0, 1}, {1, 0, 1, 0, 1, 0});
      tc.add_pair({1, 0, 1, 0, 1, 1, 0}, {1, 0, 1, 0, 1, 1});
      tc.add_pair({1, 0, 1, 1, 0, 1, 0}, {1, 0, 1, 1, 0, 1});
      tc.add_pair({1, 1, 0, 1, 0, 1, 0}, {1, 0, 1, 0, 1, 0});
      tc.add_pair({1, 1, 1, 1, 0, 1, 0}, {1, 0, 1, 0});
      tc.add_pair({0, 0, 1, 1, 1, 0, 1, 0}, {1, 1, 1, 0, 1, 0});

      //check_hlt(tc);
      REQUIRE(tc.number_of_classes() == 78);
      tc.standardize(order::shortlex);
    }
    {
      ToddCoxeter tc(left);
      tc.set_number_of_generators(2);
      tc.next_lookahead(10);
      tc.add_pair({0, 0, 0}, {0});
      tc.add_pair({1, 0, 0}, {1, 0});
      tc.add_pair({1, 0, 1, 1, 1}, {1, 0});
      tc.add_pair({1, 1, 1, 1, 1}, {1, 1});
      tc.add_pair({1, 1, 0, 1, 1, 0}, {1, 0, 1, 0, 1, 1});
      tc.add_pair({0, 0, 1, 0, 1, 1, 0}, {0, 1, 0, 1, 1, 0});
      tc.add_pair({0, 0, 1, 1, 0, 1, 0}, {0, 1, 1, 0, 1, 0});
      tc.add_pair({0, 1, 0, 1, 0, 1, 0}, {1, 0, 1, 0, 1, 0});
      tc.add_pair({1, 0, 1, 0, 1, 0, 1}, {1, 0, 1, 0, 1, 0});
      tc.add_pair({1, 0, 1, 0, 1, 1, 0}, {1, 0, 1, 0, 1, 1});
      tc.add_pair({1, 0, 1, 1, 0, 1, 0}, {1, 0, 1, 1, 0, 1});
      tc.add_pair({1, 1, 0, 1, 0, 1, 0}, {1, 0, 1, 0, 1, 0});
      tc.add_pair({1, 1, 1, 1, 0, 1, 0}, {1, 0, 1, 0});
      tc.add_pair({0, 0, 1, 1, 1, 0, 1, 0}, {1, 1, 1, 0, 1, 0});

      //check_hlt(tc);
      REQUIRE(tc.number_of_classes() == 78);
      tc.standardize(order::shortlex);
    }
  }*/

  LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                          "020",
                          "2-sided cong. on free semigroup",
                          "[todd-coxeter][quick]") {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.alphabet(1);
    ToddCoxeter tc(twosided, p);

    // check_hlt(tc);
    // check_random(tc);

    REQUIRE(tc.contains({0, 0}, {0, 0}));
    REQUIRE(!tc.contains({0, 0}, {0}));
  }

  LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                          "021",
                          "calling run when obviously infinite",
                          "[todd-coxeter][quick]") {
    Presentation<word_type> p;
    p.alphabet(5);
    ToddCoxeter tc(twosided, p);

    // check_hlt(tc);
    // check_random(tc);

    REQUIRE_THROWS_AS(tc.run(), LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                          "022",
                          "stellar_monoid S3",
                          "[todd-coxeter][quick][hivert]") {
    auto rg = ReportGuard(false);

    Presentation<word_type> p;
    p.alphabet(4);
    presentation::add_rule_and_check(p, {3, 3}, {3});
    presentation::add_rule_and_check(p, {0, 3}, {0});
    presentation::add_rule_and_check(p, {3, 0}, {0});
    presentation::add_rule_and_check(p, {1, 3}, {1});
    presentation::add_rule_and_check(p, {3, 1}, {1});
    presentation::add_rule_and_check(p, {2, 3}, {2});
    presentation::add_rule_and_check(p, {3, 2}, {2});
    presentation::add_rule_and_check(p, {0, 0}, {0});
    presentation::add_rule_and_check(p, {1, 1}, {1});
    presentation::add_rule_and_check(p, {2, 2}, {2});
    presentation::add_rule_and_check(p, {0, 2}, {2, 0});
    presentation::add_rule_and_check(p, {2, 0}, {0, 2});
    presentation::add_rule_and_check(p, {1, 2, 1}, {2, 1, 2});
    presentation::add_rule_and_check(p, {1, 0, 1, 0}, {0, 1, 0, 1});
    presentation::add_rule_and_check(p, {1, 0, 1, 0}, {0, 1, 0});

    ToddCoxeter tc(twosided, std::move(p));

    // check_hlt(tc);
    section_felsch(tc);
    // check_random(tc);
    // check_Rc_style(tc);
    // check_R_over_C_style(tc);
    // check_CR_style(tc);
    // check_Cr_style(tc);

    REQUIRE(tc.number_of_classes() == 34);

    auto S = make(tc);  // Make a FroidurePin object from tc
    REQUIRE(S.size() == 34);
    using v3::detail::TCE;

    std::vector<TCE> v(S.cbegin(), S.cend());
    std::sort(v.begin(), v.end());
    REQUIRE(v
            == std::vector<TCE>(
                {TCE(1),  TCE(2),  TCE(3),  TCE(4),  TCE(5),  TCE(6),  TCE(7),
                 TCE(8),  TCE(9),  TCE(10), TCE(11), TCE(12), TCE(13), TCE(14),
                 TCE(15), TCE(16), TCE(17), TCE(18), TCE(19), TCE(20), TCE(21),
                 TCE(22), TCE(23), TCE(24), TCE(25), TCE(26), TCE(27), TCE(28),
                 TCE(29), TCE(30), TCE(31), TCE(32), TCE(33), TCE(34)}));
    REQUIRE(std::vector<TCE>(S.cbegin_sorted(), S.cend_sorted())
            == std::vector<TCE>(
                {TCE(1),  TCE(2),  TCE(3),  TCE(4),  TCE(5),  TCE(6),  TCE(7),
                 TCE(8),  TCE(9),  TCE(10), TCE(11), TCE(12), TCE(13), TCE(14),
                 TCE(15), TCE(16), TCE(17), TCE(18), TCE(19), TCE(20), TCE(21),
                 TCE(22), TCE(23), TCE(24), TCE(25), TCE(26), TCE(27), TCE(28),
                 TCE(29), TCE(30), TCE(31), TCE(32), TCE(33), TCE(34)}));
    REQUIRE(detail::to_string(TCE(1)) == "1");
    REQUIRE_NOTHROW(IncreaseDegree<TCE>()(TCE(1), 10));

    std::ostringstream oss;
    oss << TCE(10);  // Does not do anything visible

    std::stringbuf buf;
    std::ostream   os(&buf);
    os << TCE(32);  // Does not do anything visible
  }

  LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                          "023",
                          "finite semigroup (size 5)",
                          "[todd-coxeter][quick]") {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    presentation::add_rule(p, {0, 0, 0}, {0});
    presentation::add_rule(p, {0}, {1, 1});
    p.alphabet_from_rules();

    ToddCoxeter tc(left, p);

    // check_hlt(tc);
    section_felsch(tc);
    // check_random(tc);
    // check_Rc_style(tc);
    // check_R_over_C_style(tc);
    // check_CR_style(tc);
    // check_Cr_style(tc);

    REQUIRE(tc.number_of_classes() == 5);
  }

  LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                          "024",
                          "exceptions",
                          "[todd-coxeter][quick]") {
    auto rg = ReportGuard(false);
    for (auto knd : {left, right}) {
      Presentation<word_type> p;
      p.alphabet(2);
      presentation::add_rule_and_check(p, {0, 0, 0}, {0});
      presentation::add_rule_and_check(p, {0}, {1, 1});

      ToddCoxeter tc1(knd, p);
      REQUIRE(tc1.number_of_classes() == 5);

      congruence_kind wrng_knd = (knd == left ? right : left);
      REQUIRE_THROWS_AS(ToddCoxeter(wrng_knd, tc1), LibsemigroupsException);
      REQUIRE_THROWS_AS(ToddCoxeter(twosided, tc1), LibsemigroupsException);

      ToddCoxeter tc2(knd, tc1);
      REQUIRE(!tc1.contains({0}, {1}));
      tc2.add_pair({0}, {1});

      section_felsch(tc2);

      // check_hlt(tc2);
      // check_random(tc2);
      // check_Rc_style(tc2);
      // check_R_over_C_style(tc2);
      // check_CR_style(tc2);
      // check_Cr_style(tc2);

      REQUIRE(tc2.number_of_classes() == 1);

      presentation::add_rule_and_check(p, {0}, {1});
      ToddCoxeter tc3(knd, p);
      REQUIRE(tc3.number_of_classes() == 1);
      REQUIRE(tc3.word_graph() == tc2.word_graph());
    }
  }

  LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                          "025",
                          "obviously infinite",
                          "[todd-coxeter][quick]") {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.alphabet(3);
    presentation::add_rule(p, {0, 0, 0}, {0});
    for (auto knd : {left, right, twosided}) {
      ToddCoxeter tc(knd, p);

      // check_hlt(tc);
      // check_random(tc);

      REQUIRE(tc.number_of_classes() == POSITIVE_INFINITY);
      REQUIRE(is_obviously_infinite(tc));
    }
  }

  LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                          "026",
                          "exceptions",
                          "[todd-coxeter][quick]") {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    presentation::add_rule(p, {0, 0, 0}, {0});
    presentation::add_rule(p, {0}, {1, 1});
    p.alphabet_from_rules();

    {
      ToddCoxeter tc(right, p);
      // check_hlt(tc);
      section_felsch(tc);

      REQUIRE(tc.number_of_classes() == 5);
      REQUIRE(tc.class_index_to_word(0) == word_type({0}));
      // This next one should throw
      REQUIRE_THROWS_AS(make<FroidurePin<TCE>>(tc), LibsemigroupsException);
    }
    {
      ToddCoxeter tc(twosided, p);
      // check_hlt(tc);
      section_felsch(tc);
      // check_random(tc);
      // check_Rc_style(tc);
      // check_R_over_C_style(tc);
      // check_CR_style(tc);
      // check_Cr_style(tc);

      REQUIRE(tc.number_of_classes() == 5);
      REQUIRE(tc.class_index_to_word(0) == word_type({0}));
      REQUIRE(tc.class_index_to_word(1) == word_type({1}));
      REQUIRE(tc.class_index_to_word(2) == word_type({0, 0}));
      REQUIRE(tc.class_index_to_word(3) == word_type({0, 1}));
      REQUIRE(tc.class_index_to_word(4) == word_type({0, 0, 1}));
      REQUIRE_THROWS_AS(tc.class_index_to_word(5), LibsemigroupsException);
      REQUIRE_THROWS_AS(tc.class_index_to_word(100), LibsemigroupsException);
    }
  }

  /*
    LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                            "027",
                            "empty",
                            "[todd-coxeter][quick]") {
      auto rg = ReportGuard(false);
      {
        ToddCoxeter tc(left);
        REQUIRE(tc.empty());
        tc.set_number_of_generators(3);
        REQUIRE(tc.empty());
        tc.add_pair({0}, {2});
        REQUIRE(tc.empty());
        tc.reserve(100);
        tc.reserve(200);
        REQUIRE(tc.empty());
      }
      {
        FroidurePin<BMat8> S(
            {BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}})});

        ToddCoxeter tc(twosided, S);
        REQUIRE(tc.empty());
        tc.add_pair({0}, {0, 0});
        REQUIRE(tc.empty());
      }
    }
*/
  LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                          "028",
                          "quotient ToddCoxeter",
                          "[todd-coxeter][quick]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("ab");
    presentation::add_rule_and_check(p, "aaa", "a");
    presentation::add_rule_and_check(p, "a", "bb");
    ToddCoxeter tc1(twosided, p);
    REQUIRE(tc1.number_of_classes() == 5);
    ToddCoxeter tc2(left, tc1);
    tc2.add_pair({0}, {1});
    REQUIRE_THROWS_AS(tc2.add_pair({0}, {2}), LibsemigroupsException);
    // check_hlt_no_save(tc2);
    // check_hlt_save_throws(tc2);
    section_felsch(tc2);
    // check_random(tc2);
    REQUIRE(tc2.number_of_classes() == 1);
  }

  // TODO move to make-todd-coxeter.hpp
  LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                          "029",
                          "from KnuthBendix",
                          "[todd-coxeter][quick]") {
    auto                     rg = ReportGuard(false);
    fpsemigroup::KnuthBendix kb;
    kb.set_alphabet("abB");

    kb.add_rule("bb", "B");
    kb.add_rule("BaB", "aba");
    SECTION("not started. . .") {
      REQUIRE(!kb.confluent());
      REQUIRE(!kb.started());
    }
    SECTION("finished . . .") {
      kb.run();
      REQUIRE(kb.confluent());
      REQUIRE(kb.number_of_active_rules() == 6);
      REQUIRE(kb.finished());
    }

    ToddCoxeter tc(make<ToddCoxeter>(twosided, kb));
    // check_random(*tc);
    // check_hlt(*tc);
    tc.add_pair({1}, {2});
    REQUIRE(is_obviously_infinite(tc));
    REQUIRE(tc.number_of_classes() == POSITIVE_INFINITY);
    REQUIRE(tc.presentation().rules
            == std::vector<word_type>({{1, 1}, {2}, {2, 0, 2}, {0, 1, 0}}));
    REQUIRE(std::vector<word_type>(tc.cbegin_generating_pairs(),
                                   tc.cend_generating_pairs())
            == std::vector<word_type>({{1}, {2}}));
    REQUIRE(!tc.finished());
    REQUIRE(!tc.started());
    tc.add_pair({1}, {0});
    REQUIRE(!is_obviously_infinite(tc));

    REQUIRE(tc.number_of_classes() == 1);
  }

  LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                          "031",
                          "KnuthBendix.finished()",
                          "[todd-coxeter][quick]") {
    auto                     rg = ReportGuard(false);
    fpsemigroup::KnuthBendix kb;
    kb.set_alphabet("abB");
    kb.add_rule("bb", "B");
    kb.add_rule("BaB", "aba");
    kb.add_rule("a", "b");
    kb.add_rule("b", "B");

    REQUIRE(kb.confluent());
    kb.run();
    REQUIRE(kb.confluent());
    REQUIRE(kb.number_of_active_rules() == 3);
    REQUIRE(kb.size() == 1);
    REQUIRE(kb.is_obviously_finite());
    REQUIRE(kb.finished());

    for (auto knd : {twosided, left, right}) {
      ToddCoxeter tc(make<ToddCoxeter>(knd, kb));
      tc.add_pair({1}, {2});
      REQUIRE(tc.number_of_classes() == 1);
      if (tc.kind() == twosided) {
        REQUIRE(make(tc).size() == 1);
      } else {
        REQUIRE_THROWS_AS(make(tc), LibsemigroupsException);
      }
    }
  }

  LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                          "032",
                          "from ActionDigraph",
                          "[todd-coxeter][quick]") {
    auto rg = ReportGuard(false);

    ActionDigraph<uint32_t> d(1, 2);
    REQUIRE(d.out_degree() == 2);
    REQUIRE(d.number_of_nodes() == 1);
    REQUIRE_NOTHROW(ToddCoxeter(twosided, d));
  }

  LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                          "033",
                          "congruence of ToddCoxeter",
                          "[todd-coxeter][quick]") {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.alphabet(2);
    presentation::add_rule_and_check(p, {0, 0, 0}, {0});
    presentation::add_rule_and_check(p, {0}, {1, 1});
    ToddCoxeter tc1(twosided, p);
    REQUIRE(tc1.number_of_classes() == 5);

    ToddCoxeter tc2(left, tc1);
    // tc2.next_lookahead(1);
    tc2.report_every(1);
    // check_hlt(tc2);
    // check_random(tc2);
    tc2.add_pair({0}, {0, 0});

    REQUIRE(tc2.number_of_classes() == 3);
  }

  LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                          "034",
                          "congruence of ToddCoxeter",
                          "[todd-coxeter][quick]") {
    auto rg      = ReportGuard(false);
    using Transf = LeastTransf<5>;
    FroidurePin<Transf> S({Transf({1, 3, 4, 2, 3}), Transf({3, 2, 1, 3, 3})});
    REQUIRE(S.size() == 88);
    REQUIRE(S.number_of_rules() == 18);
    ToddCoxeter tc = make<ToddCoxeter>(twosided, S);  // use Cayley graph
    // check_hlt_no_save(tc);
    // check_hlt_save_throws(tc);
    // check_random(tc);
    tc.add_pair({0}, {1, 1});
    section_felsch(tc);
    REQUIRE(tc.number_of_classes() == 1);
  }

  LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                          "035",
                          "congruence on FpSemigroup",
                          "[todd-coxeter][quick]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("abe");
    presentation::add_identity_rules(p, 'e');
    presentation::add_rule_and_check(p, "abb", "bb");
    presentation::add_rule_and_check(p, "bbb", "bb");
    presentation::add_rule_and_check(p, "aaaa", "a");
    presentation::add_rule_and_check(p, "baab", "bb");
    presentation::add_rule_and_check(p, "baaab", "b");
    presentation::add_rule_and_check(p, "babab", "b");
    presentation::add_rule_and_check(p, "bbaaa", "bb");
    presentation::add_rule_and_check(p, "bbaba", "bbaa");

    ToddCoxeter tc1(left, p);
    tc1.add_pair({0}, {1, 1, 1});
    // check_hlt(tc1);
    section_felsch(tc1);
    // check_random(tc1);
    // check_Rc_style(tc1);
    // check_R_over_C_style(tc1);
    // check_CR_style(tc1);
    // check_Cr_style(tc1);

    REQUIRE(tc1.number_of_classes() == 11);
    REQUIRE(std::vector<word_type>(todd_coxeter::cbegin_normal_forms(tc1),
                                   todd_coxeter::cend_normal_forms(tc1))
            == std::vector<word_type>({{0},
                                       {1},
                                       {2},
                                       {0, 1},
                                       {0, 0, 1},
                                       {1, 0, 1},
                                       {0, 0, 0, 1},
                                       {0, 1, 0, 1},
                                       {1, 1, 0, 1},
                                       {0, 0, 1, 0, 1},
                                       {0, 0, 0, 1, 0, 1}}));

    ToddCoxeter tc2(twosided, p);
    REQUIRE(tc2.number_of_classes() == 40);
    auto part = todd_coxeter::partition(tc1,
                                        todd_coxeter::cbegin_normal_forms(tc2),
                                        todd_coxeter::cend_normal_forms(tc2));
    REQUIRE(part
            == decltype(part)({{{0},
                                {0, 0},
                                {1, 0},
                                {1, 1},
                                {0, 0, 0},
                                {0, 1, 0},
                                {1, 0, 0},
                                {1, 1, 0},
                                {0, 0, 1, 0},
                                {0, 1, 0, 0},
                                {1, 0, 0, 0},
                                {1, 0, 1, 0},
                                {1, 1, 0, 0},
                                {0, 0, 0, 1, 0},
                                {0, 0, 1, 0, 0},
                                {0, 1, 0, 0, 0},
                                {0, 1, 0, 1, 0},
                                {1, 0, 1, 0, 0},
                                {0, 0, 0, 1, 0, 0},
                                {0, 0, 1, 0, 0, 0},
                                {0, 0, 1, 0, 1, 0},
                                {0, 1, 0, 1, 0, 0},
                                {1, 0, 1, 0, 0, 0},
                                {0, 0, 0, 1, 0, 0, 0},
                                {0, 0, 0, 1, 0, 1, 0},
                                {0, 0, 1, 0, 1, 0, 0},
                                {0, 1, 0, 1, 0, 0, 0},
                                {0, 0, 0, 1, 0, 1, 0, 0},
                                {0, 0, 1, 0, 1, 0, 0, 0},
                                {0, 0, 0, 1, 0, 1, 0, 0, 0}},
                               {{1}},
                               {{2}},
                               {{0, 1}},
                               {{0, 0, 1}},
                               {{1, 0, 1}},
                               {{0, 0, 0, 1}},
                               {{0, 1, 0, 1}},
                               {{1, 1, 0, 1}},
                               {{0, 0, 1, 0, 1}},
                               {{0, 0, 0, 1, 0, 1}}}));
  }

  LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                          "037",
                          "copy constructor",
                          "[todd-coxeter][quick]") {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.alphabet(2);
    presentation::add_rule_and_check(p, {0}, {1});
    presentation::add_rule_and_check(p, {0, 0}, {0});

    ToddCoxeter tc(twosided, p);
    // tc.strategy(options::strategy::felsch);
    // REQUIRE(tc.strategy() == options::strategy::felsch);
    REQUIRE(!todd_coxeter_digraph::complete(tc.word_graph()));
    REQUIRE(todd_coxeter_digraph::compatible(
        tc.word_graph(), p.rules.cbegin(), p.rules.cend()));
    REQUIRE(tc.number_of_classes() == 1);
    REQUIRE(std::vector<word_type>(todd_coxeter::cbegin_normal_forms(tc),
                                   todd_coxeter::cend_normal_forms(tc))
            == std::vector<word_type>(1, {0}));
    REQUIRE(todd_coxeter_digraph::complete(tc.word_graph()));
    REQUIRE(todd_coxeter_digraph::compatible(
        tc.word_graph(), p.rules.cbegin(), p.rules.cend()));

    ToddCoxeter copy(tc);
    REQUIRE(copy.presentation().rules == p.rules);
    REQUIRE(copy.presentation().alphabet() == p.alphabet());
    REQUIRE(copy.finished());
    REQUIRE(copy.number_of_classes() == 1);
    REQUIRE(todd_coxeter_digraph::complete(copy.word_graph()));
    REQUIRE(todd_coxeter_digraph::compatible(
        copy.word_graph(), p.rules.cbegin(), p.rules.cend()));
    REQUIRE(tc.word_graph() == copy.word_graph());
  }

  LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                          "039",
                          "stylic_monoid",
                          "[todd-coxeter][quick][no-coverage][no-valgrind]") {
    using fpsemigroup::make;
    using fpsemigroup::stylic_monoid;

    auto rg = ReportGuard(false);

    auto p = make<Presentation<word_type>>(stylic_monoid(9));
    presentation::remove_duplicate_rules(p);
    presentation::sort_rules(p);
    presentation::sort_each_rule(p);
    std::reverse(p.rules.begin(), p.rules.end());

    ToddCoxeter tc(twosided, p);
    // TODO uncomment
    // tc.strategy(options::strategy::random);
    // REQUIRE_THROWS_AS(tc.run_for(std::chrono::milliseconds(100)),
    //                   LibsemigroupsException);
    // tc.strategy(options::strategy::hlt)
    //     .lookahead(options::lookahead::partial | options::lookahead::hlt);
    REQUIRE(tc.number_of_classes() == 115'974);
  }

  LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                          "040",
                          "fibonacci_semigroup(4, 6)",
                          "[todd-coxeter][fail]") {
    using fpsemigroup::fibonacci_semigroup;
    using fpsemigroup::make;

    auto rg = ReportGuard();
    auto p  = make<Presentation<word_type>>(fibonacci_semigroup(4, 6));

    ToddCoxeter tc(twosided, p);
    // tc.strategy(options::strategy::felsch);
    REQUIRE(tc.number_of_classes() == 0);
  }

  /* LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                           "041",
                           "some finite classes",
                           "[todd-coxeter][quick]") {
     auto rg            = ReportGuard(false);
     using digraph_type = typename ToddCoxeter::digraph_type;
     ToddCoxeter tc(twosided);
     tc.set_number_of_generators(1);
     REQUIRE(tc.number_of_classes() == POSITIVE_INFINITY);

     tc.add_pair({0, 0, 0, 0, 0, 0}, {0, 0, 0, 0});
     tc.add_pair({0, 0, 0, 0, 0, 0}, {0, 0, 0, 0});
     tc.add_pair({0, 0, 0, 0, 0, 0}, {0, 0, 0, 0});
     tc.strategy(options::strategy::random)
         .remove_duplicate_generating_pairs()
         .standardize(true);
     REQUIRE(!tc.compatible());
     REQUIRE_THROWS_AS(tc.run_for(std::chrono::microseconds(1)),
                       LibsemigroupsException);
     tc.strategy(options::strategy::CR);
     size_t x = 0;
     REQUIRE_THROWS_AS(tc.run_until([&x] { return x > 4; }),
                       LibsemigroupsException);
     tc.lower_bound(100)
         .use_relations_in_extra(true)
         .deduction_policy(DefinitionOptions::definitions::unlimited)
         .deduction_version(digraph_type::process_defs::v1)
         .restandardize(true)
         .max_preferred_defs(0);
     REQUIRE_THROWS_AS(tc.hlt_defs(0), LibsemigroupsException);
     REQUIRE_THROWS_AS(tc.f_defs(0), LibsemigroupsException);
     tc.hlt_defs(10)
         .f_defs(10)
         .lookahead_growth_factor(3.0)
         .lookahead_growth_threshold(100'000)
         .large_collapse(1);
     REQUIRE_THROWS_AS(tc.lookahead_growth_factor(0.1),
   LibsemigroupsException);

     REQUIRE(tc.random_interval() == std::chrono::milliseconds(200));
     REQUIRE(tc.felsch_tree_height() == 6);
     REQUIRE(tc.number_of_classes() == 5);
     REQUIRE(tc.number_of_words(0) == 1);
     REQUIRE(tc.number_of_words(1) == 1);
     REQUIRE(tc.number_of_words(2) == 1);
     REQUIRE(tc.number_of_words(3) == POSITIVE_INFINITY);
     REQUIRE(tc.number_of_words(4) == POSITIVE_INFINITY);
     REQUIRE(tc.standardization_order() == order::none);
     REQUIRE(tc.felsch_tree_number_of_nodes() == 7);
     REQUIRE_THROWS_AS(tc.remove_duplicate_generating_pairs(),
                       LibsemigroupsException);
     ToddCoxeter tc2(left, tc);
     tc2.add_pair({0, 0}, {0});
     tc2.add_pair({0, 0}, {0});
     tc2.remove_duplicate_generating_pairs();
     // Uses CongruenceInterface's generating pairs
     REQUIRE(tc2.number_of_generating_pairs() == 2);
     ToddCoxeter tc3(twosided);
     tc3.set_number_of_generators(1);
     REQUIRE(tc3.is_non_trivial() == tril::TRUE);
     tc3.add_pair({0, 0}, {0});
     REQUIRE(tc3.is_non_trivial() == tril::unknown);
     REQUIRE(tc3.number_of_classes() == 1);
     REQUIRE(tc3.is_non_trivial() == tril::FALSE);
     REQUIRE(!tc.settings_string().empty());
     REQUIRE(!tc3.settings_string().empty());
     REQUIRE(!tc.stats_string().empty());
   }*/

  // Takes about 6m
  LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                          "042",
                          "symmetric_group(9, Moore)",
                          "[todd-coxeter][extreme]") {
    using fpsemigroup::author;
    using fpsemigroup::make;
    using fpsemigroup::symmetric_group;

    auto rg = ReportGuard(true);

    auto p = make<Presentation<word_type>>(symmetric_group(9, author::Moore));
    presentation::reduce_complements(p);
    presentation::remove_duplicate_rules(p);
    presentation::sort_rules(p);
    presentation::sort_each_rule(p);
    std::reverse(p.rules.begin(), p.rules.end());

    REQUIRE(presentation::length(p) == 570);

    auto w = presentation::longest_common_subword(p);
    while (!w.empty()) {
      presentation::replace_subword(p, w);
      w = presentation::longest_common_subword(p);
    }

    REQUIRE(presentation::length(p) == 81);
    REQUIRE(p.alphabet().size() == 11);

    ToddCoxeter tc(twosided, p);

    REQUIRE(tc.number_of_classes() == 362'880);
    // TODO uncomment std::cout << tc.stats_string();
  }

  LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                          "043",
                          "symmetric_group(7, Coxeter + Moser)",
                          "[todd-coxeter][quick][no-valgrind]") {
    using fpsemigroup::author;
    using fpsemigroup::make;
    using fpsemigroup::symmetric_group;

    auto rg = ReportGuard(false);

    size_t n = 7;
    auto   p = make<Presentation<word_type>>(
        symmetric_group(n, author::Coxeter + author::Moser));

    ToddCoxeter tc(twosided, p);

    tc.run_for(std::chrono::microseconds(1));
    // TODO uncomment REQUIRE(tc.is_non_trivial() == tril::TRUE);
    REQUIRE(!tc.finished());
    tc.standardize(order::shortlex);
    tc.standardize(order::none);

    REQUIRE(tc.number_of_classes() == 5'040);
  }

  LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                          "116",
                          "symmetric_group(7, Burnside + Miller)",
                          "[todd-coxeter][quick][no-valgrind]") {
    using fpsemigroup::author;
    using fpsemigroup::make;
    using fpsemigroup::symmetric_group;
    auto rg = ReportGuard(false);

    size_t n = 7;
    auto   p = make<Presentation<word_type>>(
        symmetric_group(n, author::Burnside + author::Miller));

    ToddCoxeter tc(twosided, p);

    REQUIRE(tc.number_of_classes() == 5'040);
  }
  /*
    LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                            "045",
                            "Options operator<<",
                            "[todd-coxeter][quick]") {
      auto rg = ReportGuard(false);
      {
        ToddCoxeter tc(twosided);
        tc.strategy(options::strategy::hlt);
        tc.settings_string();
        tc.strategy(options::strategy::felsch);
        tc.settings_string();
        tc.strategy(options::strategy::random);
        tc.settings_string();
        tc.strategy(options::strategy::CR);
        tc.settings_string();
        tc.strategy(options::strategy::R_over_C);
        tc.settings_string();
        tc.strategy(options::strategy::Cr);
        tc.settings_string();
        tc.strategy(options::strategy::Rc);
        tc.settings_string();
      }
      {
        ToddCoxeter tc(twosided);
        tc.lookahead(options::lookahead::full | options::lookahead::felsch);
        tc.settings_string();
        tc.lookahead(options::lookahead::full | options::lookahead::hlt);
        tc.settings_string();
        tc.lookahead(options::lookahead::partial | options::lookahead::felsch);
        tc.settings_string();
        tc.lookahead(options::lookahead::partial | options::lookahead::hlt);
        tc.settings_string();
      }
      {
        using digraph_type = typename ToddCoxeter::digraph_type;
        ToddCoxeter tc(twosided);
        tc.deduction_version(digraph_type::process_defs::v1)
            .deduction_policy(
                DefinitionOptions::definitions::no_stack_if_no_space);
        tc.settings_string();
        tc.deduction_version(digraph_type::process_defs::v1)
            .deduction_policy(DefinitionOptions::definitions::purge_all);
        tc.settings_string();
        tc.deduction_version(digraph_type::process_defs::v1)
            .deduction_policy(DefinitionOptions::definitions::purge_from_top);
        tc.settings_string();
        tc.deduction_version(digraph_type::process_defs::v1)
            .deduction_policy(
                DefinitionOptions::definitions::discard_all_if_no_space);
        tc.settings_string();
        tc.deduction_version(digraph_type::process_defs::v1)
            .deduction_policy(DefinitionOptions::definitions::unlimited);
        tc.settings_string();
        tc.deduction_version(digraph_type::process_defs::v2)
            .deduction_policy(
                DefinitionOptions::definitions::no_stack_if_no_space);
        tc.settings_string();
        tc.deduction_version(digraph_type::process_defs::v2)
            .deduction_policy(DefinitionOptions::definitions::purge_all);
        tc.settings_string();
        tc.deduction_version(digraph_type::process_defs::v2)
            .deduction_policy(DefinitionOptions::definitions::purge_from_top);
        tc.deduction_version(digraph_type::process_defs::v2)
            .deduction_policy(
                DefinitionOptions::definitions::discard_all_if_no_space);
        tc.settings_string();
        tc.deduction_version(digraph_type::process_defs::v2)
            .deduction_policy(DefinitionOptions::definitions::unlimited);
        tc.settings_string();
      }
      {
        ToddCoxeter tc(twosided);
        tc.froidure_pin_policy(options::froidure_pin::none);
        tc.settings_string();
        tc.froidure_pin_policy(options::froidure_pin::use_cayley_graph);
        tc.settings_string();
        tc.froidure_pin_policy(options::froidure_pin::use_relations);
        tc.settings_string();
      }
      {
        ToddCoxeter tc(twosided);
        tc.preferred_defs(options::preferred_defs::none);
        tc.settings_string();
        tc.preferred_defs(options::preferred_defs::immediate_no_stack);
        tc.settings_string();
        tc.preferred_defs(options::preferred_defs::immediate_yes_stack);
        tc.settings_string();
        tc.preferred_defs(options::preferred_defs::deferred);
        tc.settings_string();
      }
    }
    */

  LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                          "046",
                          "Easdown-East-FitzGerald DualSymInv(5)",
                          "[todd-coxeter][quick][no-valgrind][no-coverage]") {
    auto       rg = ReportGuard(true);
    auto const n  = 5;
    auto       p  = fpsemigroup::make<Presentation<word_type>>(
        fpsemigroup::dual_symmetric_inverse_monoid(n));
    ToddCoxeter tc(twosided, p);
    section_felsch(tc);
    REQUIRE(tc.number_of_classes() == 6'721);
    // check_complete_compatible(tc);
  }

  LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                          "047",
                          "uniform_block_bijection_monoid(3) (FitzGerald) ",
                          "[todd-coxeter][quick]") {
    using fpsemigroup::author;
    using fpsemigroup::make;
    using fpsemigroup::uniform_block_bijection_monoid;

    // 16, 131, 1496, 22482, 426833, 9934563, 9934563
    auto       rg = ReportGuard(false);
    auto const n  = 5;

    auto p = make<Presentation<word_type>>(
        uniform_block_bijection_monoid(n, author::FitzGerald));

    ToddCoxeter tc(twosided, p);

    // check_hlt(tc);
    section_felsch(tc);
    // check_random(tc);
    // check_R_over_C_style(tc);
    // check_Rc_style(tc);
    //
    REQUIRE(tc.number_of_classes() == 1496);
  }

  /*
  LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                          "048",
                          "stellar_monoid(7) (Gay-Hivert)",
                          "[todd-coxeter][quick][no-valgrind][no-coverage]") {
    auto         rg = ReportGuard(false);
    size_t const n  = 7;
    ToddCoxeter  tc1(congruence_kind::twosided);
    setup(tc1, n + 1, rook_monoid, n, 0);
    ToddCoxeter tc2(congruence_kind::twosided, tc1);
    setup(tc2, n + 1, stellar_monoid, n);
    tc2.strategy(options::strategy::felsch);
    REQUIRE(tc2.number_of_classes() == 13'700);
  }*/

  LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                          "049",
                          "partition_monoid(4) (East)",
                          "[todd-coxeter][quick][no-valgrind][no-coverage]") {
    auto rg = ReportGuard(true);
    using fpsemigroup::author;
    using fpsemigroup::make;
    using fpsemigroup::partition_monoid;

    auto const n = 4;
    auto p = make<Presentation<word_type>>(partition_monoid(n, author::East));
    REQUIRE(!p.contains_empty_word());
    ToddCoxeter tc(twosided, p);

    section_felsch(tc);
    // check_hlt(tc);
    // check_random(tc);
    // check_R_over_C_style(tc);
    // check_Rc_style(tc);
    check_complete_compatible(tc);
    REQUIRE(tc.number_of_classes() == 4'140);
  }

  LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                          "050",
                          "singular_brauer_monoid(6) (Maltcev + Mazorchuk)",
                          "[todd-coxeter][quick][no-valgrind][no-coverage]") {
    using fpsemigroup::make;
    using fpsemigroup::singular_brauer_monoid;

    auto         rg = ReportGuard(true);
    size_t const n  = 6;
    auto         p  = make<Presentation<word_type>>(singular_brauer_monoid(n));
    presentation::remove_duplicate_rules(p);
    presentation::sort_rules(p);
    presentation::sort_each_rule(p);
    REQUIRE(!p.contains_empty_word());
    ToddCoxeter tc(twosided, p);
    REQUIRE(tc.number_of_classes() == 9'675);
  }

  LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                          "051",
                          "orientation_preserving_monoid(6) (Ruskuc + Arthur)",
                          "[todd-coxeter][quick][no-valgrind][no-coverage]") {
    using fpsemigroup::make;
    using fpsemigroup::orientation_preserving_monoid;
    auto         rg = ReportGuard(false);
    size_t const n  = 4;
    auto p = make<Presentation<word_type>>(orientation_preserving_monoid(n));
    ToddCoxeter tc(congruence_kind::twosided, p);
    // TODO uncomment
    // tc.strategy(options::strategy::hlt);
    // tc.standardize(false).lookahead(options::lookahead::partial).save(true);
    // check_hlt(tc);
    section_felsch(tc);
    // check_random(tc);
    // check_R_over_C_style(tc);
    // check_Rc_style(tc);

    REQUIRE(tc.number_of_classes() == 128);
  }

  LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                          "052",
                          "orientation_reversing_monoid(5) (Ruskuc + Arthur)",
                          "[todd-coxeter][quick][no-valgrind][no-coverage]") {
    using fpsemigroup::make;
    using fpsemigroup::orientation_reversing_monoid;
    auto         rg = ReportGuard(false);
    size_t const n  = 5;
    auto p = make<Presentation<word_type>>(orientation_reversing_monoid(n));
    ToddCoxeter tc(congruence_kind::twosided, p);
    // check_hlt(tc);
    section_felsch(tc);
    // check_random(tc);
    // check_Rc_style(tc);
    // check_R_over_C_style(tc);
    // check_CR_style(tc);
    // check_Cr_style(tc);

    REQUIRE(tc.number_of_classes() == 1'015);
  }

  LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                          "053",
                          "temperley_lieb_monoid(10) (East)",
                          "[todd-coxeter][quick][no-valgrind][no-coverage]") {
    using fpsemigroup::make;
    using fpsemigroup::temperley_lieb_monoid;
    auto         rg = ReportGuard(false);
    size_t const n  = 10;
    auto         p  = make<Presentation<word_type>>(temperley_lieb_monoid(n));
    ToddCoxeter  tc(congruence_kind::twosided, p);
    section_felsch(tc);
    REQUIRE(tc.number_of_classes() == 16'795);
  }

  /*
  LIBSEMIGROUPS_TEST_CASE(
      "v3::ToddCoxeter",
      "054",
      "Generate GAP benchmarks for stellar_monoid(n) (Gay-Hivert)",
      "[todd-coxeter][fail]") {
    auto rg = ReportGuard(false);
    for (size_t n = 3; n <= 9; ++n) {
      ToddCoxeter tc1(congruence_kind::twosided);
      setup(tc1, n + 1, rook_monoid, n, 0);
      ToddCoxeter tc2(congruence_kind::twosided, tc1);
      setup(tc2, n + 1, stellar_monoid, n);
      output_gap_benchmark_file("stellar-" + std::to_string(n) + ".g", tc2);
    }
  }

    LIBSEMIGROUPS_TEST_CASE(
        "v3::ToddCoxeter",
        "055",
        "Generate GAP benchmarks for partition_monoid(n) (East)",
        "[todd-coxeter][fail]") {
      auto rg = ReportGuard(false);
      for (size_t n = 4; n <= 6; ++n) {
        ToddCoxeter tc(congruence_kind::twosided);
        setup(tc, 5, partition_monoid, n, author::East);
        tc.save(true);
        output_gap_benchmark_file("partition-" + std::to_string(n) + ".g",
tc);
      }
    }

    LIBSEMIGROUPS_TEST_CASE(
        "v3::ToddCoxeter",
        "056",
        "Generate GAP benchmarks for dual symmetric inverse monoid (East)",
        "[todd-coxeter][fail]") {
      auto rg = ReportGuard(false);
      for (size_t n = 3; n <= 6; ++n) {
        ToddCoxeter tc(congruence_kind::twosided);
        setup(tc,
              n + 1,
              dual_symmetric_inverse_monoid,
              n,
              author::Easdown + author::East + author::FitzGerald);
        output_gap_benchmark_file("dual-sym-inv-" + std::to_string(n) + ".g",
                                  tc);
      }
    }

    LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                            "057",
                            "Generate GAP benchmarks for "
                            "uniform_block_bijection_monoid (FitzGerald)",
                            "[todd-coxeter][fail]") {
      auto rg = ReportGuard(false);
      for (size_t n = 3; n <= 7; ++n) {
        ToddCoxeter tc(congruence_kind::twosided);
        setup(tc, n + 1, uniform_block_bijection_monoid, n,
author::FitzGerald); output_gap_benchmark_file( "uniform-block-bijection-" +
std::to_string(n) + ".g", tc);
      }
    }

    LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                            "058",
                            "Generate GAP benchmarks for stylic monoids",
                            "[todd-coxeter][fail]") {
      auto rg = ReportGuard(false);
      for (size_t n = 3; n <= 9; ++n) {
        ToddCoxeter tc(congruence_kind::twosided);
        setup(tc, n, stylic_monoid, n);
        output_gap_benchmark_file("stylic-" + std::to_string(n) + ".g", tc);
      }
    }

    LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                            "059",
                            "Generate GAP benchmarks for OP_n",
                            "[todd-coxeter][fail]") {
      auto rg = ReportGuard(false);
      for (size_t n = 3; n <= 9; ++n) {
        ToddCoxeter tc(congruence_kind::twosided);
        setup(tc, 3, orientation_preserving_monoid, n);
        output_gap_benchmark_file("orient-" + std::to_string(n) + ".g", tc);
      }
    }

    LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                            "060",
                            "Generate GAP benchmarks for OR_n",
                            "[todd-coxeter][fail]") {
      auto rg = ReportGuard(false);
      for (size_t n = 3; n <= 8; ++n) {
        ToddCoxeter tc(congruence_kind::twosided);
        setup(tc, 4, orientation_reversing_monoid, n);
        output_gap_benchmark_file("orient-reverse-" + std::to_string(n) +
".g", tc);
      }
    }

    LIBSEMIGROUPS_TEST_CASE(
        "v3::ToddCoxeter",
        "061",
        "Generate GAP benchmarks for temperley_lieb_monoid(n)",
        "[todd-coxeter][fail]") {
      auto rg = ReportGuard(false);
      for (size_t n = 3; n <= 13; ++n) {
        ToddCoxeter tc(congruence_kind::twosided);
        setup(tc, n - 1, temperley_lieb_monoid, n);
        output_gap_benchmark_file("temperley-lieb-" + std::to_string(n) +
".g", tc);
      }
    }

    LIBSEMIGROUPS_TEST_CASE(
        "v3::ToddCoxeter",
        "062",
        "Generate GAP benchmarks for singular_brauer_monoid(n)",
        "[todd-coxeter][fail]") {
      auto rg = ReportGuard(false);
      for (size_t n = 3; n <= 7; ++n) {
        ToddCoxeter tc(congruence_kind::twosided);
        setup(tc, n * n - n, singular_brauer_monoid, n);
        output_gap_benchmark_file("singular-brauer-" + std::to_string(n) +
".g", tc);
      }
    }
*/
  // TODO move the definition to fpsemi-examples.
  LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                          "111",
                          "partition_monoid(2)",
                          "[todd-coxeter][quick]") {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.alphabet(4);
    presentation::add_rule_and_check(p, {0, 1}, {1});
    presentation::add_rule_and_check(p, {1, 0}, {1});
    presentation::add_rule_and_check(p, {0, 2}, {2});
    presentation::add_rule_and_check(p, {2, 0}, {2});
    presentation::add_rule_and_check(p, {0, 3}, {3});
    presentation::add_rule_and_check(p, {3, 0}, {3});
    presentation::add_rule_and_check(p, {1, 1}, {0});
    presentation::add_rule_and_check(p, {1, 3}, {3});
    presentation::add_rule_and_check(p, {2, 2}, {2});
    presentation::add_rule_and_check(p, {3, 1}, {3});
    presentation::add_rule_and_check(p, {3, 3}, {3});
    presentation::add_rule_and_check(p, {2, 3, 2}, {2});
    presentation::add_rule_and_check(p, {3, 2, 3}, {3});
    presentation::add_rule_and_check(p, {1, 2, 1, 2}, {2, 1, 2});
    presentation::add_rule_and_check(p, {2, 1, 2, 1}, {2, 1, 2});

    ToddCoxeter tc(congruence_kind::twosided, p);
    section_felsch(tc);
    REQUIRE(tc.number_of_classes() == 15);
  }

  LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                          "112",
                          "brauer_monoid(4) (Kudryavtseva + Mazorchuk)",
                          "[todd-coxeter][quick][no-valgrind][no-coverage]") {
    using fpsemigroup::brauer_monoid;
    using fpsemigroup::make;

    auto         rg = ReportGuard(false);
    size_t const n  = 4;
    auto         p  = make<Presentation<word_type>>(brauer_monoid(n));
    presentation::sort_rules(p);
    presentation::remove_duplicate_rules(p);
    ToddCoxeter tc(congruence_kind::twosided, p);
    section_felsch(tc);
    REQUIRE(tc.number_of_classes() == 105);
  }

  LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                          "113",
                          "symmetric_inverse_monoid(5, Sutov)",
                          "[todd-coxeter][quick]") {
    using fpsemigroup::author;
    using fpsemigroup::make;
    using fpsemigroup::symmetric_inverse_monoid;

    auto rg = ReportGuard(false);
    auto p  = make<Presentation<word_type>>(
        symmetric_inverse_monoid(5, author::Sutov));

    ToddCoxeter tc(congruence_kind::twosided, p);
    section_felsch(tc);
    REQUIRE(tc.number_of_classes() == 1'546);
  }

  LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                          "114",
                          "partial_transformation_monoid(5, Sutov)",
                          "[todd-coxeter][standard]") {
    using fpsemigroup::author;
    using fpsemigroup::make;
    using fpsemigroup::partial_transformation_monoid;

    auto   rg = ReportGuard(true);
    size_t n  = 5;
    auto   p  = make<Presentation<word_type>>(
        partial_transformation_monoid(n, author::Sutov));

    ToddCoxeter tc(congruence_kind::twosided, p);
    section_felsch(tc);
    REQUIRE(tc.number_of_classes() == 7'776);
  }

  LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                          "115",
                          "full_transformation_monoid(7, Iwahori)",
                          "[todd-coxeter][extreme]") {
    using fpsemigroup::author;
    using fpsemigroup::full_transformation_monoid;
    using fpsemigroup::make;

    auto   rg = ReportGuard(true);
    size_t n  = 7;
    auto   p  = make<Presentation<word_type>>(
        full_transformation_monoid(n, author::Iwahori));
    ToddCoxeter tc(congruence_kind::twosided, p);
    section_felsch(tc);
    REQUIRE(tc.number_of_classes() == 823'543);
  }
  /*

  LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                          "063",
                          "add_rule",
                          "[todd-coxeter][quick]") {
    auto rg = ReportGuard(false);
    {
      ToddCoxeter tc;
      tc.set_alphabet("ab");
      tc.add_rule("aaa", "a");
      tc.add_rule("a", "bb");
      // check_hlt(tc);
      section_felsch(tc);
      // check_random(tc);
      // check_Rc_style(tc);
      // check_R_over_C_style(tc);
      // check_CR_style(tc);
      // check_Cr_style(tc);
      SECTION("R/C + Felsch lookahead") {
        tc.congruence()
            .strategy(options::strategy::R_over_C)
            .lookahead(options::lookahead::felsch |
options::lookahead::full); tc.congruence().run();
      }
      SECTION("HLT + Felsch lookahead + save") {
        tc.congruence()
            .strategy(options::strategy::hlt)
            .save(true)
            .lookahead(options::lookahead::felsch |
options::lookahead::full) .next_lookahead(2); tc.congruence().run();
      }
      SECTION("Cr + small number of f_defs") {
        tc.congruence().strategy(options::strategy::Cr).f_defs(3);
        tc.congruence().run();
      }
      SECTION("Rc + small number of deductions") {
        tc.congruence().strategy(options::strategy::Rc).max_deductions(0);
        tc.congruence().run();
      }
      using digraph_type =
          typename ::libsemigroups::ToddCoxeter::digraph_type;
      SECTION("Felsch + v2 + no preferred defs") {
        tc.congruence()
            .strategy(options::strategy::felsch)
            .deduction_version(digraph_type::process_defs::v2)
            .deduction_policy(DefinitionOptions::definitions::purge_all)
            .preferred_defs(options::preferred_defs::none);
      }
      SECTION("Felsch + v2 + immediate no stack") {
        tc.congruence()
            .strategy(options::strategy::felsch)
            .deduction_version(digraph_type::process_defs::v2)
            .deduction_policy(DefinitionOptions::definitions::purge_from_top)
            .preferred_defs(options::preferred_defs::immediate_no_stack);
      }
      SECTION("Felsch + v1 + immediate no stack") {
        tc.congruence()
            .strategy(options::strategy::felsch)
            .deduction_version(digraph_type::process_defs::v1)
            .deduction_policy(
                DefinitionOptions::definitions::discard_all_if_no_space)
            .preferred_defs(options::preferred_defs::immediate_no_stack);
      }
      SECTION("Felsch + v1 + immediate yes stack") {
        tc.congruence()
            .strategy(options::strategy::felsch)
            .deduction_version(digraph_type::process_defs::v1)
            .deduction_policy(
                DefinitionOptions::definitions::no_stack_if_no_space)
            .preferred_defs(options::preferred_defs::immediate_yes_stack);
      }
      SECTION("large collapse") {
        tc.congruence().large_collapse(0);
      }

      REQUIRE(tc.number_of_classes() == 5);
    }
    {
      ToddCoxeter tc;
      tc.set_alphabet("ab");
      tc.add_rule("aaa", "a");
      tc.add_rule("a", "bb");
      tc.congruence().next_lookahead(1);
      // check_hlt(tc);
      section_felsch(tc);
      // check_random(tc);
      // check_Rc_style(tc);
      // check_R_over_C_style(tc);
      // check_CR_style(tc);
      // check_Cr_style(tc);

      REQUIRE(tc.number_of_classes() == 5);
    }
  }
*/
  // KnuthBendix methods fail for this one
  LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                          "064",
                          "from kbmag/standalone/kb_data/s4",
                          "[todd-coxeter][quick][kbmag]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("abcd");
    presentation::add_rule_and_check(p, "bb", "c");
    presentation::add_rule_and_check(p, "caca", "abab");
    presentation::add_rule_and_check(p, "bc", "d");
    presentation::add_rule_and_check(p, "cb", "d");
    presentation::add_rule_and_check(p, "aa", "d");
    presentation::add_rule_and_check(p, "ad", "a");
    presentation::add_rule_and_check(p, "da", "a");
    presentation::add_rule_and_check(p, "bd", "b");
    presentation::add_rule_and_check(p, "db", "b");
    presentation::add_rule_and_check(p, "cd", "c");
    presentation::add_rule_and_check(p, "dc", "c");

    ToddCoxeter tc(twosided, p);
    // check_hlt(tc);
    section_felsch(tc);
    // check_random(tc);
    // check_Rc_style(tc);
    // check_R_over_C_style(tc);
    // check_CR_style(tc);
    // check_Cr_style(tc);
    //
    detail::StringToWord string_to_word(p.alphabet());

    REQUIRE(tc.number_of_classes() == 24);
    REQUIRE(todd_coxeter::normal_form(tc, string_to_word("aaaaaaaaaaaaaaaaaaa"))
            == string_to_word("a"));
    auto S = make<FroidurePin<TCE>>(tc);
    REQUIRE(KnuthBendix(S).confluent());
  }

  // Second of BHN's series of increasingly complicated presentations
  // of 1. Doesn't terminate
  LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                          "065",
                          "(from kbmag/standalone/kb_data/degen4b) "
                          "(KnuthBendix 065)",
                          "[fail][todd-coxeter][kbmag][shortlex]") {
    auto rg = ReportGuard(true);

    Presentation<std::string> p;
    p.alphabet("abcdef");
    p.contains_empty_word(true);
    presentation::add_inverse_rules(p, "defabc");
    presentation::add_rule_and_check(p, "bbdeaecbffdbaeeccefbccefb", "");
    presentation::add_rule_and_check(p, "ccefbfacddecbffaafdcaafdc", "");
    presentation::add_rule_and_check(p, "aafdcdbaeefacddbbdeabbdea", "");

    REQUIRE(presentation::length(p) == 87);
    auto w = presentation::longest_common_subword(p);
    REQUIRE(w == "ccefb");
    while (!w.empty()) {
      presentation::replace_subword(p, w);
      w = presentation::longest_common_subword(p);
    }
    REQUIRE(presentation::length(p) == 63);
    REQUIRE(p.alphabet() == "");

    presentation::remove_trivial_rules(p);
    presentation::remove_duplicate_rules(p);
    presentation::sort_rules(p);
    presentation::sort_each_rule(p);
    REQUIRE(p.rules == std::vector<std::string>());

    ToddCoxeter tc(twosided, p);

    // TODO uncomment
    // tc.lookahead(options::lookahead::full
    //                          | options::lookahead::felsch);
    REQUIRE(!is_obviously_infinite(tc));
    REQUIRE(tc.number_of_classes() == 1);
  }

  /*
    LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                            "066",
                            "test validate",
                            "[todd-coxeter][quick]") {
      auto rg = ReportGuard(false);

      ToddCoxeter tc;
      tc.set_alphabet("ab");
      tc.add_rule("a", "b");
      tc.add_rule("bb", "b");

      REQUIRE_THROWS_AS(tc.add_rule("b", "c"), LibsemigroupsException);
    }

    LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                            "067",
                            "add_rules after construct. from semigroup",
                            "[todd-coxeter][quick]") {
      auto rg = ReportGuard(false);

      using Transf = LeastTransf<5>;

      FroidurePin<Transf> S({Transf({1, 3, 4, 2, 3}), Transf({3, 2, 1, 3,
  3})}); REQUIRE(S.size() == 88); REQUIRE(S.number_of_rules() == 18);

      word_type w1, w2, w3, w4;
      S.factorisation(w1, S.position(Transf({3, 4, 4, 4, 4})));
      S.factorisation(w2, S.position(Transf({3, 1, 3, 3, 3})));
      S.factorisation(w3, S.position(Transf({1, 3, 1, 3, 3})));
      S.factorisation(w4, S.position(Transf({4, 2, 4, 4, 2})));

      ToddCoxeter tc1(S);
      tc1.add_rule(w1, w2);

      // check_hlt_no_save(tc1);
      // check_hlt_save_throws(tc1);
      section_felsch_throws(tc1);
      // check_random(tc1);

      REQUIRE(tc1.number_of_classes() == 21);
      REQUIRE(tc1.number_of_classes() == tc1.froidure_pin()->size());
      REQUIRE(tc1.equal_to(w3, w4));
      REQUIRE(tc1.normal_form(w3) == tc1.normal_form(w4));

      ToddCoxeter tc2(S);
      tc2.add_rule(w1, w2);

      // check_hlt_no_save(tc2);
      // check_hlt_save_throws(tc2);
      section_felsch_throws(tc2);

      REQUIRE(tc2.number_of_classes() == 21);
      REQUIRE(tc2.number_of_classes() == tc2.froidure_pin()->size());
      REQUIRE(tc2.equal_to(w3, w4));
      REQUIRE(tc2.normal_form(w3) == tc2.normal_form(w4));
    }

    LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                            "068",
                            "Sym(5) from Chapter 3, Proposition 1.1 in NR",
                            "[todd-coxeter][quick]") {
      auto rg = ReportGuard(false);

      ToddCoxeter tc;
      tc.set_alphabet("ABabe");
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

      using digraph_type = typename
  ::libsemigroups::ToddCoxeter::digraph_type;

      SECTION("Deduction policy == purge_from_top") {
        tc.congruence()
            .max_deductions(2)
            .strategy(options::strategy::felsch)
            .max_preferred_defs(3);
        REQUIRE_THROWS_AS(tc.congruence().deduction_policy(
                              DefinitionOptions::definitions::purge_from_top),
                          LibsemigroupsException);
        tc.congruence()
            .deduction_version(digraph_type::process_defs::v1)
            .deduction_policy(DefinitionOptions::definitions::purge_from_top);
      }
      SECTION("Deduction policy == purge_all") {
        tc.congruence().max_deductions(2).strategy(options::strategy::felsch);
        tc.congruence()
            .deduction_version(digraph_type::process_defs::v1)
            .deduction_policy(DefinitionOptions::definitions::purge_all);
      }
      SECTION("Deduction policy == discard_all_if_no_space") {
        tc.congruence().max_deductions(2).strategy(options::strategy::felsch);
        tc.congruence()
            .deduction_version(digraph_type::process_defs::v2)
            .deduction_policy(
                DefinitionOptions::definitions::discard_all_if_no_space);
      }
      // check_hlt(tc);
      section_felsch(tc);
      // check_random(tc);
      // check_Rc_style(tc);
      // check_R_over_C_style(tc);
      // check_CR_style(tc);
      // check_Cr_style(tc);

      REQUIRE(tc.number_of_classes() == 120);
    }

    LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                            "069",
                            "Chapter 7, Theorem 3.6 in NR (size 243)",
                            "[no-valgrind][todd-coxeter][quick]") {
      auto        rg = ReportGuard(false);
      ToddCoxeter tc;
      tc.set_alphabet("ab");
      tc.add_rule("aaa", "a");
      tc.add_rule("bbbb", "b");
      tc.add_rule("ababababab", "aa");

      // check_hlt(tc);
      section_felsch(tc);
      // check_random(tc);
      // check_Rc_style(tc);
      // check_R_over_C_style(tc);
      // check_CR_style(tc);
      // check_Cr_style(tc);

      REQUIRE(tc.number_of_classes() == 243);
    }

    LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                            "070",
                            "finite semigroup (size 99)",
                            "[todd-coxeter][quick]") {
      auto        rg = ReportGuard(false);
      ToddCoxeter tc;
      tc.set_alphabet("ab");
      tc.add_rule("aaa", "a");
      tc.add_rule("bbbb", "b");
      tc.add_rule("abababab", "aa");

      REQUIRE(!tc.is_obviously_finite());

      // check_hlt(tc);
      section_felsch(tc);
      // check_random(tc);
      // check_Rc_style(tc);
      // check_R_over_C_style(tc);
      // check_CR_style(tc);
      // check_Cr_style(tc);

      REQUIRE(tc.number_of_classes() == 99);
      REQUIRE(tc.finished());
      REQUIRE(tc.is_obviously_finite());
    }

    // The following 8 examples are from Trevor Walker's Thesis: Semigroup
    // enumeration - computer implementation and applications, p41.
    LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                            "071",
                            "Walker 1",
                            "[todd-coxeter][standard]") {
      auto        rg = ReportGuard(false);
      ToddCoxeter tc;
      tc.set_alphabet("abcABCDEFGHIXYZ");
      tc.add_rule("A", "aaaaaaaaaaaaaa");
      tc.add_rule("B", "bbbbbbbbbbbbbb");
      tc.add_rule("C", "cccccccccccccc");
      tc.add_rule("D", "aaaaba");
      tc.add_rule("E", "bbbbab");
      tc.add_rule("F", "aaaaca");
      tc.add_rule("G", "ccccac");
      tc.add_rule("H", "bbbbcb");
      tc.add_rule("I", "ccccbc");
      tc.add_rule("X", "aaa");
      tc.add_rule("Y", "bbb");
      tc.add_rule("Z", "ccc");

      tc.add_rule("A", "a");
      tc.add_rule("B", "b");
      tc.add_rule("C", "c");
      tc.add_rule("D", "Y");
      tc.add_rule("E", "X");
      tc.add_rule("F", "Z");
      tc.add_rule("G", "X");
      tc.add_rule("H", "Z");
      tc.add_rule("I", "Y");

      tc.congruence()
          .sort_generating_pairs(shortlex_compare)
          .next_lookahead(500'000)
          .run_until([&tc]() -> bool {
            return tc.congruence().coset_capacity() >= 10'000;
          });
      REQUIRE(!tc.finished());
      REQUIRE(!tc.is_obviously_finite());
      tc.congruence().standardize(order::shortlex);
      REQUIRE(!tc.finished());
      tc.congruence().standardize(order::lex);
      REQUIRE(!tc.finished());
      tc.congruence().standardize(order::recursive);
      REQUIRE(!tc.finished());

      // check_hlt(tc);
      section_felsch(tc);
      // check_random(tc);
      // check_Rc_style(tc);
      // check_R_over_C_style(tc);
      // check_CR_style(tc);
      // check_Cr_style(tc);

      // This takes approx 1 seconds with Felsch . . .
      REQUIRE(tc.number_of_classes() == 1);
      tc.congruence().standardize(order::shortlex);
      REQUIRE(std::is_sorted(tc.congruence().cbegin_normal_forms(),
                             tc.congruence().cend_normal_forms(),
                             ShortLexCompare<word_type>{}));
      tc.congruence().standardize(order::lex);
      REQUIRE(std::is_sorted(tc.congruence().cbegin_normal_forms(),
                             tc.congruence().cend_normal_forms(),
                             LexicographicalCompare<word_type>{}));
      tc.congruence().standardize(order::recursive);
      REQUIRE(std::is_sorted(tc.congruence().cbegin_normal_forms(),
                             tc.congruence().cend_normal_forms(),
                             RecursivePathCompare<word_type>{}));
    }

    // The following example is a good one for using the lookahead.
    LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                            "072",
                            "Walker 2",
                            "[todd-coxeter][extreme]") {
      auto        rg = ReportGuard(false);
      ToddCoxeter tc;
      tc.set_alphabet("ab");
      tc.add_rule("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", "a");
      tc.add_rule("bbb", "b");
      tc.add_rule("ababa", "b");
      tc.add_rule("aaaaaaaaaaaaaaaabaaaabaaaaaaaaaaaaaaaabaaaa", "b");

      REQUIRE(!tc.is_obviously_finite());
      using digraph_type = typename
  ::libsemigroups::ToddCoxeter::digraph_type;

      SECTION("custom HLT") {
        tc.congruence()
            .sort_generating_pairs()
            .next_lookahead(1'000'000)
            .max_deductions(2'000)
            .use_relations_in_extra(true)
            .strategy(options::strategy::hlt)
            .lookahead(options::lookahead::partial |
  options::lookahead::felsch) .deduction_version(digraph_type::process_defs::v2)
            .deduction_policy(
                DefinitionOptions::definitions::no_stack_if_no_space);
      }

      // check_hlt(tc);
      section_felsch(tc);
      // check_random(tc);
      // check_Rc_style(tc);
      // check_R_over_C_style(tc);
      // check_CR_style(tc);
      // check_Cr_style(tc);
      REQUIRE(tc.number_of_classes() == 14'911);
      tc.congruence().standardize(order::shortlex);
    }

    LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                            "073",
                            "Walker 3",
                            "[todd-coxeter][standard]") {
      auto        rg = ReportGuard(false);
      ToddCoxeter tc;
      tc.set_alphabet("ab");
      tc.add_rule("aaaaaaaaaaaaaaaa", "a");
      tc.add_rule("bbbbbbbbbbbbbbbb", "b");
      tc.add_rule("abb", "baa");
      tc.congruence().next_lookahead(2'000'000);
      tc.congruence().simplify();
      REQUIRE(!tc.is_obviously_finite());

      // check_hlt(tc);
      section_felsch(tc);
      // check_random(tc);
      // check_Rc_style(tc); // Rc_style + partial lookahead works very
      badly
          // 2m30s
          // check_R_over_C_style(tc);
          // check_Cr_style(tc);

          REQUIRE(tc.number_of_classes() == 20'490);
    }

    LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                            "074",
                            "Walker 4",
                            "[todd-coxeter][extreme]") {
      auto        rg = ReportGuard();
      ToddCoxeter tc;
      tc.set_alphabet("ab");
      tc.add_rule("aaa", "a");
      tc.add_rule("bbbbbb", "b");
      tc.add_rule("ababbbbababbbbababbbbababbbbababbbbababbbbababbbbabba",
                  "bb");

      tc.congruence().next_lookahead(3'000'000);

      REQUIRE(!tc.is_obviously_finite());

      // check_hlt(tc);
      //  Felsch very slow
      // check_random(tc);
      SECTION("custom R/C") {
        tc.congruence()
            .next_lookahead(3'000'000)
            .strategy(options::strategy::R_over_C)
            .max_deductions(100'000);
      }
      tc.congruence().run();
      REQUIRE(tc.number_of_classes() == 36'412);
    }

    LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                            "075",
                            "Walker 5",
                            "[todd-coxeter][extreme]") {
      auto        rg = ReportGuard(false);
      ToddCoxeter tc;
      tc.set_alphabet("ab");
      tc.add_rule("aaa", "a");
      tc.add_rule("bbbbbb", "b");
      tc.add_rule(
          "ababbbbababbbbababbbbababbbbababbbbababbbbababbbbabbabbbbbaa",
  "bb"); tc.congruence().next_lookahead(5'000'000);
      REQUIRE(!tc.is_obviously_finite());

      // This example is extremely slow with Felsch
      // check_hlt(tc);
      // check_random(tc);
      // check_Rc_style(tc);
      // check_R_over_C_style(tc);
      // check_CR_style(tc);
      // check_Cr_style(tc);

      tc.congruence().run();
      REQUIRE(tc.congruence().complete());
      REQUIRE(tc.congruence().compatible());

      REQUIRE(tc.number_of_classes() == 72'822);
      std::cout << tc.congruence().stats_string();
    }

    LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                            "076",
                            "not Walker 6",
                            "[todd-coxeter][extreme]") {
      auto        rg = ReportGuard();
      ToddCoxeter tc;
      tc.set_alphabet("ab");
      tc.add_rule("aaa", "a");
      tc.add_rule("bbbbbbbbb", "b");
      tc.add_rule(
          "ababbbbababbbbababbbbababbbbababbbbababbbbababbbbabbabbbbbbbb",
          "bb");
      tc.congruence().next_lookahead(5'000'000);
      REQUIRE(!tc.is_obviously_finite());

      // This example is extremely slow with Felsch, the random strategy
      // strategy is typically fastest
      // check_hlt(tc);
      // check_random(tc);
      // check_Rc_style(tc);
      // check_R_over_C_style(tc);
      // check_CR_style(tc);
      // check_Cr_style(tc);

      REQUIRE(tc.number_of_classes() == 8);
    }

    LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                            "077",
                            "Walker 6",
                            "[todd-coxeter][standard]") {
      auto        rg = ReportGuard(false);
      ToddCoxeter tc;
      tc.set_alphabet("ab");
      tc.add_rule("aaa", "a");
      tc.add_rule("bbbbbbbbb", "b");
      std::string lng("ababbbbbbb");
      lng += lng;
      lng += "abbabbbbbbbb";
      tc.add_rule(lng, "bb");
      REQUIRE(!tc.is_obviously_finite());

      // This example is extremely slow with Felsch
      // check_hlt(tc);
      // check_random(tc);
      // check_Rc_style(tc); // partial lookahead is too slow
      // check_Cr_style(tc); // very slow
      // check_R_over_C_style(tc);

      REQUIRE(tc.number_of_classes() == 78'722);
    }

    // Felsch is faster here too!
    LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                            "078",
                            "Walker 7",
                            "[todd-coxeter][standard]") {
      auto        rg = ReportGuard(false);
      ToddCoxeter tc;
      tc.set_alphabet("abcde");
      tc.add_rule("aaa", "a");
      tc.add_rule("bbb", "b");
      tc.add_rule("ccc", "c");
      tc.add_rule("ddd", "d");
      tc.add_rule("eee", "e");
      tc.add_rule("ababab", "aa");
      tc.add_rule("bcbcbc", "bb");
      tc.add_rule("cdcdcd", "cc");
      tc.add_rule("dedede", "dd");
      tc.add_rule("ac", "ca");
      tc.add_rule("ad", "da");
      tc.add_rule("ae", "ea");
      tc.add_rule("bd", "db");
      tc.add_rule("be", "eb");
      tc.add_rule("ce", "ec");
      REQUIRE(!tc.is_obviously_finite());

      // check_hlt(tc);
      section_felsch(tc);
      // check_random(tc);
      // check_Rc_style(tc); // partial lookahead very slow ~8s
      // check_R_over_C_style(tc);
      // check_Cr_style(tc);

      using digraph_type = typename
  ::libsemigroups::ToddCoxeter::digraph_type; tc.congruence()
          .deduction_version(digraph_type::process_defs::v1)
          .deduction_policy(
              DefinitionOptions::definitions::no_stack_if_no_space)
          .preferred_defs(options::preferred_defs::none);

      REQUIRE(tc.number_of_classes() == 153'500);
    }

    LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                            "079",
                            "Walker 8",
                            "[todd-coxeter][standard]") {
      auto        rg = ReportGuard(false);
      ToddCoxeter tc;
      tc.set_alphabet("ab");
      tc.add_rule("aaa", "a");
      tc.add_rule("bbbbbbbbbbbbbbbbbbbbbbb", "b");
      tc.add_rule("abbbbbbbbbbbabb", "bba");

      REQUIRE(tc.congruence().length_of_generating_pairs() == 46);
      REQUIRE(!tc.is_obviously_finite());

      tc.congruence().next_lookahead(500'000);
      // This example is extremely slow with Felsch
      // check_hlt(tc);
      // check_random(tc);
      // check_Rc_style(tc); + partial lookahead too slow
      // check_Cr_style(tc); // too slow
      // check_R_over_C_style(tc);

      REQUIRE(tc.congruence().number_of_classes() == 270'272);
    }

    LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                            "080",
                            "KnuthBendix 098",
                            "[todd-coxeter][quick][no-valgrind]") {
      auto        rg = ReportGuard(false);
      ToddCoxeter tc;
      tc.set_alphabet("aAbBcCdDyYfFgGe");
      tc.set_identity("e");
      tc.set_inverses("AaBbCcDdYyFfGge");

      tc.add_rule("ab", "c");
      tc.add_rule("bc", "d");
      tc.add_rule("cd", "y");
      tc.add_rule("dy", "f");
      tc.add_rule("yf", "g");
      tc.add_rule("fg", "a");
      tc.add_rule("ga", "b");

      // check_hlt(tc);
      //  section_felsch(tc);
      // check_random(tc);
      // check_Rc_style(tc);
      // check_R_over_C_style(tc);
      // check_CR_style(tc);
      // check_Cr_style(tc);

      REQUIRE(tc.number_of_classes() == 29);
    }

    LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                            "081",
                            "Holt 2 - SL(2, p)",
                            "[todd-coxeter][extreme]") {
      auto        rg = ReportGuard();
      ToddCoxeter tc;

      tc.set_alphabet("xXyYe");
      tc.set_identity("e");
      tc.set_inverses("XxYye");

      tc.add_rule("xxYXYXYX", "e");

      auto second = [](size_t p) -> std::string {
        std::string s = "xyyyyx";
        s += std::string((p + 1) / 2, 'y');
        s += s;
        s += std::string(p, 'y');
        s += std::string(2 * (p / 3), 'x');
        return s;
      };

      REQUIRE(second(3) == "xyyyyxyyxyyyyxyyyyyxx");
      SECTION("p = 3") {
        tc.add_rule(second(3), "e");

        // check_hlt(tc);
        section_felsch(tc);

        REQUIRE(tc.number_of_classes() == 24);
      }
      SECTION("p = 5") {
        tc.add_rule(second(5), "e");

        // check_hlt(tc);
        section_felsch(tc);

        REQUIRE(tc.number_of_classes() == 120);
      }
      SECTION("p = 7") {
        tc.add_rule(second(7), "e");

        // check_hlt(tc);
        section_felsch(tc);

        REQUIRE(tc.number_of_classes() == 336);
      }
      SECTION("p = 11") {
        tc.add_rule(second(11), "e");

        // check_hlt(tc);
        // check_random(tc);

        REQUIRE(tc.number_of_classes() == 1'320);
      }
    }

    LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                            "082",
                            "Holt 3",
                            "[todd-coxeter][standard]") {
      auto        rg = ReportGuard(false);
      ToddCoxeter tc;
      tc.set_alphabet("aAbBcCe");
      tc.set_identity("e");
      tc.set_inverses("AaBbCce");

      tc.add_rule("bbCbc", "e");
      tc.add_rule("aaBab", "e");
      tc.add_rule("cABcabc", "e");
      REQUIRE(tc.congruence().is_non_trivial() == tril::TRUE);

      // check_hlt(tc);
      section_felsch(tc);
      // check_random(tc);
      // check_Rc_style(tc);
      // check_R_over_C_style(tc);
      // check_CR_style(tc);
      // check_Cr_style(tc);

      REQUIRE(tc.number_of_classes() == 6'561);
    }

    LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                            "083",
                            "Holt 3",
                            "[todd-coxeter][fail]") {
      auto        rg = ReportGuard();
      ToddCoxeter tc;
      tc.set_alphabet("aAbBcCe");
      tc.set_identity("e");
      tc.set_inverses("AaBbCce");

      tc.add_rule("aaCac", "e");
      tc.add_rule("acbbACb", "e");
      tc.add_rule("ABabccc", "e");
      REQUIRE(!tc.is_obviously_infinite());
      REQUIRE(tc.congruence().number_of_generating_pairs() == 22);
      tc.congruence().strategy(options::strategy::R_over_C);
      tc.congruence()
          .sort_generating_pairs()
          .remove_duplicate_generating_pairs();
      REQUIRE(tc.congruence().number_of_generating_pairs() == 22);
      tc.congruence()
          .lookahead(options::lookahead::partial | options::lookahead::hlt)
          .lookahead_growth_factor(1.01)
          .lookahead_growth_threshold(10)
          .f_defs(250'000)
          .hlt_defs(20'000'000);
      // REQUIRE(tc.congruence().is_non_trivial() == tril::TRUE);
      tc.congruence().run();
      REQUIRE(tc.number_of_classes() == 6'561);
    }

    LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                            "084",
                            "Campbell-Reza 1",
                            "[todd-coxeter][quick]") {
      auto        rg = ReportGuard(false);
      ToddCoxeter tc;
      tc.set_alphabet("ab");
      tc.add_rule("aa", "bb");
      tc.add_rule("ba", "aaaaaab");

      // check_hlt(tc);
      section_felsch(tc);
      // check_random(tc);
      // check_Rc_style(tc);
      // check_R_over_C_style(tc);
      // check_CR_style(tc);
      // check_Cr_style(tc);

      REQUIRE(tc.number_of_classes() == 14);
      tc.congruence().standardize(order::shortlex);
      REQUIRE(std::vector<word_type>(tc.congruence().cbegin_normal_forms(),
                                     tc.congruence().cend_normal_forms())
              == std::vector<word_type>({{0},
                                         {1},
                                         {0, 0},
                                         {0, 1},
                                         {1, 0},
                                         {0, 0, 0},
                                         {0, 0, 1},
                                         {0, 0, 0, 0},
                                         {0, 0, 0, 1},
                                         {0, 0, 0, 0, 0},
                                         {0, 0, 0, 0, 1},
                                         {0, 0, 0, 0, 0, 0},
                                         {0, 0, 0, 0, 0, 1},
                                         {0, 0, 0, 0, 0, 0, 0}}));
      REQUIRE(tc.froidure_pin()->number_of_rules() == 6);
      REQUIRE(tc.normal_form("aaaaaaab") == "aab");
      REQUIRE(tc.normal_form("bab") == "aaa");
    }

    // The next example demonstrates why we require deferred standardization
    LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                            "085",
                            "Renner monoid type D4 (Gay-Hivert), q = 1",
                            "[no-valgrind][quick][todd-coxeter][no-coverage]")
  { auto        rg = ReportGuard(false); ToddCoxeter tc; tc.set_alphabet(11);
      for (relation_type const& rl : RennerTypeDMonoid(4, 1)) {
        tc.add_rule(rl);
      }
      REQUIRE(tc.number_of_rules() == 121);
      REQUIRE(!tc.is_obviously_infinite());

      REQUIRE(tc.number_of_classes() == 10'625);

      // check_hlt(tc);
      section_felsch(tc);
      // check_random(tc);
      // check_Rc_style(tc);
      // check_R_over_C_style(tc);
      // check_CR_style(tc);
      // check_Cr_style(tc);

      tc.congruence().standardize(order::shortlex);
      REQUIRE(std::is_sorted(tc.congruence().cbegin_normal_forms(),
                             tc.congruence().cend_normal_forms(),
                             ShortLexCompare<word_type>{}));
      tc.congruence().standardize(order::lex);
      REQUIRE(std::is_sorted(tc.congruence().cbegin_normal_forms(),
                             tc.congruence().cend_normal_forms(),
                             LexicographicalCompare<word_type>{}));
      // The next section is very slow
      // SECTION("standardizing with recursive order") {
      //  tc.congruence().standardize(order::recursive);
      //  REQUIRE(std::is_sorted(tc.congruence().cbegin_normal_forms(),
      //                         tc.congruence().cend_normal_forms(),
      //                         RecursivePathCompare<word_type>{}));
      // }
    }

    // Felsch very slow here
    LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                            "086",
                            "trivial semigroup",
                            "[no-valgrind][todd-coxeter][quick][no-coverage]")
  { auto rg = ReportGuard(true);

      for (size_t N = 2; N < 1000; N += 199) {
        ToddCoxeter tc;
        tc.set_alphabet("eab");
        tc.set_identity("e");
        std::string lhs = "a" + std::string(N, 'b');
        std::string rhs = "e";
        tc.add_rule(lhs, rhs);

        lhs = std::string(N, 'a');
        rhs = std::string(N + 1, 'b');
        tc.add_rule(lhs, rhs);

        lhs = "ba";
        rhs = std::string(N, 'b') + "a";
        tc.add_rule(lhs, rhs);
        tc.run();
        if (N % 3 == 1) {
          REQUIRE(tc.number_of_classes() == 3);
        } else {
          REQUIRE(tc.number_of_classes() == 1);
        }
      }
    }

    LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                            "087",
                            "ACE --- 2p17-2p14 - HLT",
                            "[todd-coxeter][standard][ace]") {
      auto        rg = ReportGuard(false);
      ToddCoxeter G;
      G.set_alphabet("abcABCe");
      G.set_identity("e");
      G.set_inverses("ABCabce");
      G.add_rule("aBCbac", "e");
      G.add_rule("bACbaacA", "e");
      G.add_rule("accAABab", "e");

      ToddCoxeter H(right, G.congruence());
      H.add_pair({1, 2}, {6});
      H.next_lookahead(1'000'000);

      REQUIRE(H.number_of_classes() == 16'384);
    }

    LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                            "088",
                            "ACE --- 2p17-2p3 - HLT",
                            "[todd-coxeter][standard][ace]") {
      auto        rg = ReportGuard(false);
      ToddCoxeter G;
      G.set_alphabet("abcABCe");
      G.set_identity("e");
      G.set_inverses("ABCabce");
      G.add_rule("aBCbac", "e");
      G.add_rule("bACbaacA", "e");
      G.add_rule("accAABab", "e");

      letter_type a = 0;
      letter_type b = 1;
      letter_type c = 2;
      letter_type A = 3;
      letter_type B = 4;
      letter_type C = 5;
      letter_type e = 6;
      ToddCoxeter H(right, G.congruence());
      H.add_pair({b, c}, {e});
      H.add_pair({b, c}, {A, B, A, A, b, c, a, b, C});

      H.strategy(options::strategy::hlt)
          .save(true)
          .lookahead(options::lookahead::partial);

      REQUIRE(H.number_of_classes() == 8);
    }

    LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                            "089",
                            "ACE --- 2p17-1a - HLT",
                            "[todd-coxeter][standard][ace]") {
      auto        rg = ReportGuard(false);
      ToddCoxeter G;
      G.set_alphabet("abcABCe");
      G.set_identity("e");
      G.set_inverses("ABCabce");
      G.add_rule("aBCbac", "e");
      G.add_rule("bACbaacA", "e");
      G.add_rule("accAABab", "e");

      letter_type a = 0;
      letter_type b = 1;
      letter_type c = 2;
      letter_type A = 3;
      letter_type B = 4;
      letter_type C = 5;
      letter_type e = 6;

      ToddCoxeter H(right, G.congruence());
      H.add_pair({b, c}, {e});
      H.add_pair({A, B, A, A, b, c, a, b, C}, {e});
      H.add_pair({A, c, c, c, a, c, B, c, A}, {e});
      H.large_collapse(10'000);

      H.strategy(options::strategy::hlt)
          .save(true)
          .lookahead(options::lookahead::partial);
      REQUIRE(H.number_of_classes() == 1);
    }

    LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                            "090",
                            "ACE --- F27",
                            "[todd-coxeter][standard][ace]") {
      auto        rg = ReportGuard(false);
      ToddCoxeter G;
      G.set_alphabet("abcdxyzABCDXYZe");
      G.set_identity("e");
      G.set_inverses("ABCDXYZabcdxyze");
      G.add_rule("abC", "e");
      G.add_rule("bcD", "e");
      G.add_rule("cdX", "e");
      G.add_rule("dxY", "e");
      G.add_rule("xyZ", "e");
      G.add_rule("yzA", "e");
      G.add_rule("zaB", "e");

      ToddCoxeter H(twosided, G);
      section_felsch(H);
      // check_hlt(H);
      // check_random(H);

      REQUIRE(H.number_of_classes() == 29);
    }

    LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                            "091",
                            "ACE --- SL219 - HLT",
                            "[todd-coxeter][standard][ace]") {
      auto        rg = ReportGuard(false);
      ToddCoxeter G;
      G.set_alphabet("abABe");
      G.set_identity("e");
      G.set_inverses("ABabe");
      G.add_rule("aBABAB", "e");
      G.add_rule("BAAbaa", "e");
      G.add_rule(
          "abbbbabbbbbbbbbbabbbbabbbbbbbbbbbbbbbbbbbbbbbbbbbbbaaaaaaaaaaaa",
          "e");

      letter_type b = 1;
      letter_type e = 4;

      ToddCoxeter H(right, G);
      H.add_pair({b}, {e});

      H.strategy(options::strategy::hlt)
          .save(false)
          .lookahead(options::lookahead::partial);
      REQUIRE(H.number_of_classes() == 180);
    }

    LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                            "092",
                            "ACE --- perf602p5 - HLT",
                            "[no-valgrind][todd-coxeter][quick][ace]") {
      auto        rg = ReportGuard(false);
      ToddCoxeter G;
      G.set_alphabet("abstuvdABSTUVDe");
      G.set_identity("e");
      G.set_inverses("ABSTUVDabstuvde");

      G.add_rule("aaD", "e");
      G.add_rule("bbb", "e");
      G.add_rule("ababababab", "e");
      G.add_rule("ss", "e");
      G.add_rule("tt", "e");
      G.add_rule("uu", "e");
      G.add_rule("vv", "e");
      G.add_rule("dd", "e");
      G.add_rule("STst", "e");
      G.add_rule("UVuv", "e");
      G.add_rule("SUsu", "e");
      G.add_rule("SVsv", "e");
      G.add_rule("TUtu", "e");
      G.add_rule("TVtv", "e");
      G.add_rule("AsaU", "e");
      G.add_rule("AtaV", "e");
      G.add_rule("AuaS", "e");
      G.add_rule("AvaT", "e");
      G.add_rule("BsbDVT", "e");
      G.add_rule("BtbVUTS", "e");
      G.add_rule("BubVU", "e");
      G.add_rule("BvbU", "e");
      G.add_rule("DAda", "e");
      G.add_rule("DBdb", "e");
      G.add_rule("DSds", "e");
      G.add_rule("DTdt", "e");
      G.add_rule("DUdu", "e");
      G.add_rule("DVdv", "e");

      ToddCoxeter H(right, G);

      letter_type a = 0;
      letter_type e = 14;

      H.add_pair({a}, {e});

      // check_hlt(H);
      // check_random(H);
      section_felsch(H);

      REQUIRE(H.number_of_classes() == 480);
    }

    LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                            "093",
                            "ACE --- M12",
                            "[todd-coxeter][standard][ace]") {
      auto        rg = ReportGuard(false);
      ToddCoxeter G;
      G.set_alphabet("abcABCe");
      G.set_identity("e");
      G.set_inverses("ABCabce");
      G.add_rule("aaaaaaaaaaa", "e");
      G.add_rule("bb", "e");
      G.add_rule("cc", "e");
      G.add_rule("ababab", "e");
      G.add_rule("acacac", "e");
      G.add_rule("bcbcbcbcbcbcbcbcbcbc", "e");
      G.add_rule("cbcbabcbcAAAAA", "e");

      ToddCoxeter H(twosided, G);

      SECTION("HLT + save + partial lookahead") {
        H.strategy(options::strategy::hlt)
            .save(true)
            .lookahead(options::lookahead::partial);
      }
      SECTION("random") {
        H.strategy(options::strategy::random)
            .random_interval(std::chrono::milliseconds(100));
      }
      section_felsch(H);

      REQUIRE(H.number_of_classes() == 95'040);
    }

    LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                            "094",
                            "ACE --- C5 - HLT",
                            "[todd-coxeter][quick][ace]") {
      auto        rg = ReportGuard(false);
      ToddCoxeter G;
      G.set_alphabet("abABe");
      G.set_identity("e");
      G.set_inverses("ABabe");
      G.add_rule("aaaaa", "e");
      G.add_rule("b", "e");

      ToddCoxeter H(twosided, G);

      // check_hlt(H);
      // check_random(H);
      section_felsch(H);

      REQUIRE(H.number_of_classes() == 5);
    }

    LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                            "095",
                            "ACE --- A5-C5",
                            "[todd-coxeter][quick][ace]") {
      auto        rg = ReportGuard(false);
      ToddCoxeter G;
      G.set_alphabet("abABe");
      G.set_identity("e");
      G.set_inverses("ABabe");
      G.add_rule("aa", "e");
      G.add_rule("bbb", "e");
      G.add_rule("ababababab", "e");

      ToddCoxeter H(right, G);

      letter_type const a = 0, b = 1, e = 4;

      H.add_pair({a, b}, {e});

      // check_hlt(H);
      // check_random(H);
      section_felsch(H);
      REQUIRE(H.number_of_classes() == 12);
    }

    LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                            "096",
                            "ACE --- A5",
                            "[todd-coxeter][quick][ace]") {
      auto        rg = ReportGuard(false);
      ToddCoxeter G;
      G.set_alphabet("abABe");
      G.set_identity("e");
      G.set_inverses("ABabe");
      G.add_rule("aa", "e");
      G.add_rule("bbb", "e");
      G.add_rule("ababababab", "e");

      ToddCoxeter H(twosided, G);

      // check_hlt(H);
      // check_random(H);
      section_felsch(H);
      H.random_shuffle_generating_pairs();

      REQUIRE(H.number_of_classes() == 60);
      REQUIRE_THROWS_AS(H.random_shuffle_generating_pairs(),
                        LibsemigroupsException);
    }

    // Felsch is much much better here
    LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                            "097",
                            "relation ordering",
                            "[todd-coxeter][extreme]") {
      auto        rg = ReportGuard();
      ToddCoxeter tc;
      tc.set_alphabet(13);
      for (relation_type const& rl : RennerTypeDMonoid(5, 1)) {
        tc.add_rule(rl);
      }
      REQUIRE(tc.number_of_rules() == 173);
      REQUIRE(!tc.is_obviously_infinite());
      tc.congruence()
          .sort_generating_pairs(&shortlex_compare)
          .sort_generating_pairs(recursive_path_compare)
          .remove_duplicate_generating_pairs();
      REQUIRE(tc.number_of_rules() == 173);

      tc.congruence().strategy(options::strategy::felsch).f_defs(100'000).run();
      REQUIRE(tc.number_of_classes() == 258'661);
    }

    LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                            "098",
                            "relation ordering",
                            "[todd-coxeter][quick]") {
      ToddCoxeter tc;
      tc.set_alphabet(10);
      tc.add_rule({0, 1}, {0});
      tc.add_rule({0, 2}, {0});
      tc.add_rule({0, 3}, {0});
      tc.add_rule({0, 4}, {0});
      tc.add_rule({0, 5}, {0});
      tc.add_rule({0, 6}, {0});
      tc.add_rule({0, 7}, {0});
      tc.add_rule({0, 8}, {0});
      tc.add_rule({0, 9}, {0});
      tc.add_rule({1, 0}, {1});
      tc.add_rule({1, 1}, {1});
      tc.add_rule({1, 2}, {1});
      tc.add_rule({1, 3}, {1});
      tc.add_rule({1, 4}, {1});
      tc.add_rule({1, 5}, {1});
      tc.add_rule({1, 6}, {1});
      tc.add_rule({1, 7}, {1});
      tc.add_rule({1, 8}, {1});
      tc.add_rule({1, 9}, {1});
      tc.add_rule({2, 0}, {2});
      tc.add_rule({2, 1}, {2});
      tc.add_rule({2, 2}, {2});
      tc.add_rule({2, 3}, {2});
      tc.add_rule({2, 4}, {2});
      tc.add_rule({2, 5}, {2});
      tc.add_rule({2, 6}, {2});
      tc.add_rule({2, 7}, {2});
      tc.add_rule({2, 8}, {2});
      tc.add_rule({2, 9}, {2});
      tc.add_rule({3, 0}, {3});
      tc.add_rule({3, 1}, {3});
      tc.add_rule({3, 2}, {3});
      tc.add_rule({3, 3}, {3});
      tc.add_rule({3, 4}, {3});
      tc.add_rule({3, 5}, {3});
      tc.add_rule({3, 6}, {3});
      tc.add_rule({3, 7}, {3});
      tc.add_rule({3, 8}, {3});
      tc.add_rule({3, 9}, {3});
      tc.add_rule({4, 0}, {4});
      tc.add_rule({4, 1}, {4});
      tc.add_rule({4, 2}, {4});
      tc.add_rule({4, 3}, {4});
      tc.add_rule({4, 4}, {4});
      tc.add_rule({4, 5}, {4});
      tc.add_rule({4, 6}, {4});
      tc.add_rule({4, 7}, {4});
      tc.add_rule({4, 8}, {4});
      tc.add_rule({4, 9}, {4});
      tc.add_rule({5, 0}, {5});
      tc.add_rule({5, 1}, {5});
      tc.add_rule({5, 2}, {5});
      tc.add_rule({5, 3}, {5});
      tc.add_rule({5, 4}, {5});
      tc.add_rule({5, 5}, {5});
      tc.add_rule({5, 6}, {5});
      tc.add_rule({5, 7}, {5});
      tc.add_rule({5, 8}, {5});
      tc.add_rule({5, 9}, {5});
      tc.add_rule({6, 0}, {6});
      tc.add_rule({6, 1}, {6});
      tc.add_rule({6, 2}, {6});
      tc.add_rule({6, 3}, {6});
      tc.add_rule({6, 4}, {6});
      tc.add_rule({6, 5}, {6});
      tc.add_rule({6, 6}, {6});
      tc.add_rule({6, 7}, {6});
      tc.add_rule({6, 8}, {6});
      tc.add_rule({6, 9}, {6});
      tc.add_rule({7, 0}, {7});
      tc.add_rule({7, 1}, {7});
      tc.add_rule({7}, {7, 2});
      tc.add_rule({7, 3}, {7});
      tc.add_rule({7, 4}, {7});
      tc.add_rule({7, 5}, {7});
      tc.add_rule({7, 6}, {7});
      tc.add_rule({7, 7}, {7});
      tc.add_rule({7, 8}, {7});
      tc.add_rule({7, 9}, {7});
      tc.add_rule({8, 0}, {8});
      tc.add_rule({8, 1}, {8});
      tc.add_rule({8, 2}, {8});
      tc.add_rule({8, 3}, {8});
      tc.add_rule({8, 4}, {8});
      tc.add_rule({8, 5}, {8});
      tc.add_rule({8, 6}, {8});
      tc.add_rule({8, 7}, {8});
      tc.add_rule({8, 8}, {8});
      tc.add_rule({8, 9}, {8});
      tc.add_rule({9, 0}, {9});
      tc.add_rule({9, 0, 1, 2, 3, 4, 5, 5, 1, 5, 6, 9, 8, 8, 8, 8, 8, 0},
  {9}); tc.congruence().sort_generating_pairs(recursive_path_compare);

      section_felsch(tc);
      // check_hlt(tc);
      // check_random(tc);

      REQUIRE(tc.number_of_classes() == 10);

      REQUIRE_THROWS_AS(tc.congruence().sort_generating_pairs(shortlex_compare),
                        LibsemigroupsException);
    }

    LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                            "099",
                            "short circuit size in obviously infinite",
                            "[todd-coxeter][quick]") {
      auto        rg = ReportGuard(false);
      ToddCoxeter tc;
      tc.set_alphabet("abc");
      tc.add_rule("aaaa", "a");
      REQUIRE(tc.number_of_classes() == POSITIVE_INFINITY);
    }

    LIBSEMIGROUPS_TEST_CASE(
        "v3::ToddCoxeter",
        "100",
        "http://brauer.maths.qmul.ac.uk/Atlas/misc/24A8/mag/24A8G1-P1.M",
        "[todd-coxeter][standard]") {
      ToddCoxeter tc;
      tc.set_alphabet("xyXYe");
      tc.set_identity("e");
      tc.set_inverses("XYxye");
      tc.add_rule("xx", "X");
      tc.add_rule("yyyyyy", "Y");
      tc.add_rule("YXyx", "XYxy");
      tc.add_rule("xYYYxYYYxYY", "yyXyyyXyyyX");
      tc.add_rule("xyxyyXyxYYxyyyx", "yyyXyyy");
      tc.congruence()
          .next_lookahead(2'000'000)
          .strategy(options::strategy::hlt)
          .sort_generating_pairs()
          .lookahead(options::lookahead::partial)
          .standardize(true);
      tc.congruence().run();

      REQUIRE(tc.number_of_classes() == 322'560);
    }

    LIBSEMIGROUPS_TEST_CASE(
        "v3::ToddCoxeter",
        "101",
        "http://brauer.maths.qmul.ac.uk/Atlas/spor/M11/mag/M11G1-P1.M",
        "[todd-coxeter][quick][no-coverage][no-valgrind]") {
      ToddCoxeter tc;
      tc.set_alphabet("xyXYe");
      tc.set_identity("e");
      tc.set_inverses("XYxye");
      tc.add_rule("xx", "e");
      tc.add_rule("yyyy", "e");
      tc.add_rule("xyxyxyxyxyxyxyxyxyxyxy", "e");
      tc.add_rule("xyyxyyxyyxyyxyyxyy", "e");
      tc.add_rule("xyxyxYxyxyyxYxyxYxY", "e");
      REQUIRE(tc.number_of_classes() == 7'920);
    }

    LIBSEMIGROUPS_TEST_CASE(
        "v3::ToddCoxeter",
        "102",
        "http://brauer.maths.qmul.ac.uk/Atlas/spor/M12/mag/M12G1-P1.M",
        "[todd-coxeter][standard]") {
      ToddCoxeter tc;
      tc.set_alphabet("xyXYe");
      tc.set_identity("e");
      tc.set_inverses("XYxye");
      tc.add_rule("xx", "e");
      tc.add_rule("yyy", "e");
      tc.add_rule("xyxyxyxyxyxyxyxyxyxyxy", "e");
      tc.add_rule("XYxyXYxyXYxyXYxyXYxyXYxy", "e");
      tc.add_rule("xyxyxYxyxyxYxyxyxYxyxyxYxyxyxYxyxyxY", "e");
      tc.add_rule("XYXYxyxyXYXYxyxyXYXYxyxyXYXYxyxyXYXYxyxy", "e");
      REQUIRE(tc.number_of_classes() == 95'040);
    }

    LIBSEMIGROUPS_TEST_CASE(
        "v3::ToddCoxeter",
        "103",
        "http://brauer.maths.qmul.ac.uk/Atlas/spor/M22/mag/M22G1-P1.M",
        "[todd-coxeter][extreme]") {
      ToddCoxeter tc;
      tc.set_alphabet("xyXYe");
      tc.set_identity("e");
      tc.set_inverses("XYxye");
      tc.add_rule("xx", "e");
      tc.add_rule("yyyy", "e");
      tc.add_rule("xyxyxyxyxyxyxyxyxyxyxy", "e");
      tc.add_rule("xyyxyyxyyxyyxyy", "e");
      tc.add_rule("XYxyXYxyXYxyXYxyXYxyXYxy", "e");
      tc.add_rule("XYXYxyxyXYXYxyxyXYXYxyxy", "e");
      tc.add_rule("xyxyxYxyxyxYxyxyxYxyxyxYxyxyxY", "e");
      REQUIRE(tc.number_of_classes() == 443'520);
    }

    // Takes about 4 minutes (2021 - MacBook Air M1 - 8GB RAM)
    // with Felsch (3.5mins or 2.5mins with lowerbound) or HLT (4.5mins)
    LIBSEMIGROUPS_TEST_CASE(
        "v3::ToddCoxeter",
        "104",
        "http://brauer.maths.qmul.ac.uk/Atlas/spor/M23/mag/M23G1-P1.M",
        "[todd-coxeter][extreme]") {
      ToddCoxeter tc;
      tc.set_alphabet("xyXYe");
      tc.set_identity("e");
      tc.set_inverses("XYxye");
      tc.add_rule("xx", "e");
      tc.add_rule("yyyy", "e");
      tc.add_rule("xyxyxyxyxyxyxyxyxyxyxyxyxyxyxyxyxyxyxyxyxyxyxy", "e");
      tc.add_rule("xyyxyyxyyxyyxyyxyy", "e");
      tc.add_rule("XYxyXYxyXYxyXYxyXYxyXYxy", "e");
      tc.add_rule("xyxYxyyxyxYxyyxyxYxyyxyxYxyy", "e");
      tc.add_rule("xyxyxyxYxyyxyxYxyxYxyxyxyxYxYxY", "e");
      tc.add_rule("xyxyyxyyxyxyyxyyxyxyyxyyxyxyyxyyxyxyyxyyxyxyyxyy", "e");
      tc.add_rule("xyxyyxyxyyxyxyyxyyxYxyyxYxyxyyxyxYxyy", "e");
      using digraph_type = typename
  ::libsemigroups::ToddCoxeter::digraph_type; tc.congruence()
          .sort_generating_pairs()
          .strategy(options::strategy::felsch)
          .use_relations_in_extra(true)
          .lower_bound(10'200'960)
          .deduction_version(digraph_type::process_defs::v2)
          .deduction_policy(
              DefinitionOptions::definitions::no_stack_if_no_space)
          .reserve(50'000'000);
      std::cout << tc.congruence().settings_string();
      tc.congruence().run();

      REQUIRE(tc.number_of_classes() == 10'200'960);
    }
    */
  // Takes about 3 minutes (doesn't currently run with v3)
  LIBSEMIGROUPS_TEST_CASE(
      "v3::ToddCoxeter",
      "105",
      "http://brauer.maths.qmul.ac.uk/Atlas/clas/S62/mag/S62G1-P1.M",
      "[todd-coxeter][extreme]") {
    Presentation<std::string> p;
    p.alphabet("xyXY");
    p.contains_empty_word(true);
    presentation::add_inverse_rules(p, "XYxy");
    presentation::add_rule_and_check(p, "xx", "");
    presentation::add_rule_and_check(p, "yyy", "");
    presentation::add_rule_and_check(
        p, "xyxyxyxyxyxyxyxyxyxyxyxyxyxyxyxyxyxyxyxyxyxyxy", "");
    presentation::add_rule_and_check(
        p, "XYxyXYxyXYxyXYxyXYxyXYxyXYxyXYxyXYxyXYxyXYxyXYxy", "");
    presentation::add_rule_and_check(
        p, "XYXYxyxyXYXYxyxyXYXYxyxyXYXYxyxyXYXYxyxy", "");
    presentation::add_rule_and_check(
        p, "xyxyxYxyxyxYxyxyxYxyxYxYxyxYxYxyxYxY", "");
    presentation::add_rule_and_check(
        p, "xyxyxYxyxYxyxYxyxyxYxyxYxyxYxyxyxYxyxYxyxYxyxyxYxyxYxyxY", "");

    detail::StringToWord string_to_word("xyXY");
    ToddCoxeter          tc(right, p);
    tc.add_pair(string_to_word("xy"), string_to_word(""));

    REQUIRE(tc.number_of_classes() == 10'644'480);
  }

  //  // Approx. 32 minutes (2021 - MacBook Air M1 - 8GB RAM)
  //  LIBSEMIGROUPS_TEST_CASE(
  //      "v3::ToddCoxeter",
  //      "106",
  //      "http://brauer.maths.qmul.ac.uk/Atlas/spor/HS/mag/HSG1-P1.M",
  //      "[todd-coxeter][extreme]") {
  //    auto        rg = ReportGuard();
  //    ToddCoxeter tc;
  //    tc.set_alphabet("xyXYe");
  //    tc.set_identity("e");
  //    tc.set_inverses("XYxye");
  //    tc.add_rule("xx", "e");
  //    tc.add_rule("yyyyy", "e");
  //    tc.add_rule("xyxyxyxyxyxyxyxyxyxyxy", "e");
  //    tc.add_rule("xyyxyyxyyxyyxyyxyyxyyxyyxyyxyy", "e");
  //    tc.add_rule("XYxyXYxyXYxyXYxyXYxy", "e");
  //    tc.add_rule("XYXYxyxyXYXYxyxyXYXYxyxy", "e");
  //    tc.add_rule("XYYxyyXYYxyyXYYxyyXYYxyyXYYxyyXYYxyy", "e");
  //    tc.add_rule("xyxyxyyxYxYYxYxyyxyxyxYYxYYxYYxYY", "e");
  //    tc.add_rule("xyxyyxYYxYYxyyxYYxYYxyyxyxyyxYxyyxYxyy", "e");
  //    tc.add_rule("xyxyxyyxyyxyxYxYxyxyyxyyxyxyxYYxYxYY", "e");
  //    tc.add_rule("xyxyxyyxYxYYxyxyxYxyxyxyyxYxYYxyxyxY", "e");
  //    tc.add_rule("xyxyxyyxyxyxyyxyxyxYxyxyxyyxyyxyyxyxyxY", "e");
  //    tc.add_rule("xyxyxyyxyxyyxyxyyxyxyxyyxYxyxYYxyxYxyy", "e");
  //    ToddCoxeter tc2(congruence_kind::right, tc);
  //    tc2.add_pair(tc.string_to_word("xy"), tc.string_to_word("e"));
  //    tc2.sort_generating_pairs()
  //        .use_relations_in_extra(true)
  //        .strategy(options::strategy::hlt)
  //        .lookahead(options::lookahead::felsch |
  // options::lookahead::partial); REQUIRE(tc2.number_of_classes() ==
  // 4'032'000);
  //  }

  LIBSEMIGROUPS_TEST_CASE(
      "v3::ToddCoxeter",
      "107",
      "http://brauer.maths.qmul.ac.uk/Atlas/spor/J1/mag/J1G1-P1.M",
      "[todd-coxeter][standard]") {
    Presentation<std::string> p;
    p.alphabet("xyXYe");
    presentation::add_identity_rules(p, 'e');
    presentation::add_inverse_rules(p, "XYxye", 'e');
    presentation::add_rule_and_check(p, "xx", "e");
    presentation::add_rule_and_check(p, "yyy", "e");
    presentation::add_rule_and_check(p, "xyxyxyxyxyxyxy", "e");
    presentation::add_rule_and_check(
        p,
        "xyxyxYxyxYxyxYxyxyxYxyxYxyxYxyxyxYxyxYxyxYxyxyxYxyxYxyxYxyxy"
        "xYxyxYxyxY",
        "e");
    presentation::add_rule_and_check(
        p,
        "xyxyxYxyxYxyxYxyxYxyxYxyxYxyxyxYxYxyxyxYxyxYxyxYxyxYxyxYxyxY"
        "xyxyxYxY",
        "e");
    ToddCoxeter tc(twosided, p);
    REQUIRE(tc.number_of_classes() == 175'560);
  }

  LIBSEMIGROUPS_TEST_CASE(
      "v3::ToddCoxeter",
      "108",
      "http://brauer.maths.qmul.ac.uk/Atlas/lin/L34/mag/L34G1-P1.M",
      "[todd-coxeter][quick][no-coverage][no-valgrind]") {
    Presentation<std::string> p;
    p.alphabet("xyXY");
    p.contains_empty_word(true);
    presentation::add_inverse_rules(p, "XYxy");
    presentation::add_rule_and_check(p, "xx", "");
    presentation::add_rule_and_check(p, "yyyy", "");
    presentation::add_rule_and_check(p, "xyxyxyxyxyxyxy", "");
    presentation::add_rule_and_check(p, "xyyxyyxyyxyyxyy", "");
    presentation::add_rule_and_check(p, "XYxyXYxyXYxyXYxyXYxy", "");
    presentation::add_rule_and_check(p, "xyxyxYxyxyxYxyxyxYxyxyxYxyxyxY", "");
    presentation::add_rule_and_check(
        p, "xyxyxyyxYxyxyxyyxYxyxyxyyxYxyxyxyyxYxyxyxyyxY", "");
    ToddCoxeter tc(twosided, p);
    REQUIRE(tc.number_of_classes() == 20'160);
  }

  // Takes about 10 seconds (2021 - MacBook Air M1 - 8GB RAM)
  LIBSEMIGROUPS_TEST_CASE(
      "v3::ToddCoxeter",
      "109",
      "http://brauer.maths.qmul.ac.uk/Atlas/clas/S62/mag/S62G1-P1.M",
      "[todd-coxeter][extreme]") {
    Presentation<std::string> p;
    p.alphabet("xyXYe");
    presentation::add_identity_rules(p, 'e');
    presentation::add_inverse_rules(p, "XYxye", 'e');
    presentation::add_rule_and_check(p, "xx", "e");
    presentation::add_rule_and_check(p, "yyyyyyy", "e");
    presentation::add_rule_and_check(p, "xyxyxyxyxyxyxyxyxy", "e");
    presentation::add_rule_and_check(
        p, "xyyxyyxyyxyyxyyxyyxyyxyyxyyxyyxyyxyy", "e");
    presentation::add_rule_and_check(p, "XYXYXYxyxyxyXYXYXYxyxyxy", "e");
    presentation::add_rule_and_check(p, "XYxyXYxyXYxy", "e");
    presentation::add_rule_and_check(p, "XYYxyyXYYxyy", "e");
    ToddCoxeter tc(twosided, p);
    REQUIRE(tc.number_of_classes() == 1'451'520);
    // TODO uncomment
    // std::cout << tc.congruence().stats_string();
  }
}  // namespace libsemigroups
