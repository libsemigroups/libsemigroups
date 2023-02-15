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

// The purpose of this file is to test the ToddCoxeter class.

// TODO
// * an option for ToddCoxeter that doesn't stack deductions in
// process_coincidences (but does elsewhere) this seems to be good when running
// Felsch

#define CATCH_CONFIG_ENABLE_PAIR_STRINGMAKER

#include <cstdlib>

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
#include "libsemigroups/word.hpp"               // for operator"" _w

namespace libsemigroups {

  using TCE         = v3::detail::TCE;
  using KnuthBendix = fpsemigroup::KnuthBendix;
  using options     = ToddCoxeter::options;

  using namespace literals;

  struct LibsemigroupsException;  // Forward declaration
  congruence_kind constexpr twosided = congruence_kind::twosided;
  congruence_kind constexpr left     = congruence_kind::left;
  congruence_kind constexpr right    = congruence_kind::right;

  namespace {
    void section_felsch(ToddCoxeter& tc) {
      SECTION("Felsch + no standardisation") {
        tc.strategy(options::strategy::felsch);
      }
    }

    void section_hlt(ToddCoxeter& tc) {
      SECTION("HLT") {
        tc.strategy(options::strategy::hlt);
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
    auto rg = ReportGuard(true);

    Presentation<word_type> p;
    p.alphabet(2);
    presentation::add_rule_and_check(p, 000_w, 0_w);
    presentation::add_rule_and_check(p, 1111_w, 1_w);
    presentation::add_rule_and_check(p, 0101_w, 00_w);
    ToddCoxeter tc(twosided, p);

    section_felsch(tc);
    section_hlt(tc);

    REQUIRE(tc.number_of_classes() == 27);

    check_complete_compatible(tc);
    check_standardize(tc);

    // section_random(tc);
    // section_rc_style(tc);
    // section_R_over_C_style(tc);
    // section_CR_style(tc);
    // section_Cr_style(tc);

    tc.standardize(order::shortlex);

    auto words = std::vector<word_type>(
        todd_coxeter::cbegin_class(tc, 1, 0, 10), todd_coxeter::cend_class(tc));
    REQUIRE(words == std::vector<word_type>({1_w, 1111_w, 1111111_w}));

    words
        = std::vector<word_type>(todd_coxeter::cbegin_class(tc, 1111_w, 0, 10),
                                 todd_coxeter::cend_class(tc));
    REQUIRE(words == std::vector<word_type>({1_w, 1111_w, 1111111_w}));

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
    section_hlt(tc);

    // section_random(tc);
    // section_rc_style(tc);
    // section_R_over_C_style(tc);
    // section_CR_style(tc);
    // section_Cr_style(tc);

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
  // FIXME this example is much slower in v3
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

    section_hlt(tc);
    section_felsch(tc);

    // section_random(tc);
    // section_rc_style(tc);
    // section_R_over_C_style(tc);
    // section_CR_style(tc);
    // section_Cr_style(tc);
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
    section_hlt(tc);
    //  section_random(tc);
    //  section_rc_style(tc);
    //  section_R_over_C_style(tc);
    //  section_CR_style(tc);
    //  section_Cr_style(tc);

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
    section_hlt(tc);

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
    section_hlt(tc);
    // section_random(tc);
    // section_rc_style(tc);
    // section_R_over_C_style(tc);
    // section_CR_style(tc);
    // section_Cr_style(tc);

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
    section_hlt(tc);
    // section_random(tc);
    // section_rc_style(tc);
    // section_R_over_C_style(tc);
    // section_CR_style(tc);
    // section_Cr_style(tc);

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
      section_hlt(tc);
      // section_random(tc);
      // section_rc_style(tc);
      // section_R_over_C_style(tc);
      // section_CR_style(tc);
      // section_Cr_style(tc);

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
    section_hlt(tc);
    // section_random(tc);
    // section_rc_style(tc);
    // section_R_over_C_style(tc);
    // section_CR_style(tc);
    // section_Cr_style(tc);

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
    section_hlt(tc);
    // section_random(tc);
    // section_rc_style(tc);
    // section_R_over_C_style(tc);
    // section_CR_style(tc);
    // section_Cr_style(tc);

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
    section_hlt(tc);
    // section_random(tc);
    // section_rc_style(tc);
    // section_R_over_C_style(tc);
    // section_CR_style(tc);
    // section_Cr_style(tc);

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
    section_hlt(tc);
    // section_random(tc);
    // section_rc_style(tc);
    // section_R_over_C_style(tc);
    // section_CR_style(tc);
    // section_Cr_style(tc);

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

    // section_hlt_no_save(tc);
    // section_hlt_save_throws(tc);
    // section_random(tc);

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

    section_hlt(tc);
    // section_random(tc);
    // section_rc_style(tc);
    // section_R_over_C_style(tc);
    // section_CR_style(tc);
    // section_Cr_style(tc);

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
    section_hlt(tc);
    // section_random(tc);
    // section_rc_style(tc);
    // section_R_over_C_style(tc);
    // section_CR_style(tc);
    // section_Cr_style(tc);

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
    section_hlt(tc);
    // section_random(tc);
    // section_rc_style(tc);
    // section_R_over_C_style(tc);
    // section_CR_style(tc);
    // section_Cr_style(tc);

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

  LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                          "018",
                          "test lookahead",
                          "[todd-coxeter][quick]") {
    auto rg = ReportGuard(false);
    for (auto knd : {left, right, twosided}) {
      Presentation<word_type> p;
      p.alphabet(2);
      presentation::add_rule_and_check(p, 000_w, 0_w);
      presentation::add_rule_and_check(p, 100_w, 10_w);
      presentation::add_rule_and_check(p, 10111_w, 10_w);
      presentation::add_rule_and_check(p, 11111_w, 11_w);
      presentation::add_rule_and_check(p, 110110_w, 101011_w);
      presentation::add_rule_and_check(p, 0010110_w, 010110_w);
      presentation::add_rule_and_check(p, 0011010_w, 011010_w);
      presentation::add_rule_and_check(p, 0101010_w, 101010_w);
      presentation::add_rule_and_check(p, 1010101_w, 101010_w);
      presentation::add_rule_and_check(p, 1010110_w, 101011_w);
      presentation::add_rule_and_check(p, 1011010_w, 101101_w);
      presentation::add_rule_and_check(p, 1101010_w, 101010_w);
      presentation::add_rule_and_check(p, 1111010_w, 1010_w);
      presentation::add_rule_and_check(p, 00111010_w, 111010_w);

      ToddCoxeter tc(knd, p);
      tc.lookahead_next(10);
      section_hlt(tc);
      REQUIRE(tc.number_of_classes() == 78);
      check_standardize(tc);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                          "020",
                          "2-sided cong. on free semigroup",
                          "[todd-coxeter][quick]") {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.alphabet(1);
    ToddCoxeter tc(twosided, p);

    section_hlt(tc);
    // section_random(tc);

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

    section_hlt(tc);
    // section_random(tc);

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

    section_hlt(tc);
    section_felsch(tc);
    // section_random(tc);
    // section_rc_style(tc);
    // section_R_over_C_style(tc);
    // section_CR_style(tc);
    // section_Cr_style(tc);

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

    section_hlt(tc);
    section_felsch(tc);
    // section_random(tc);
    // section_rc_style(tc);
    // section_R_over_C_style(tc);
    // section_CR_style(tc);
    // section_Cr_style(tc);

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
      section_hlt(tc2);
      // section_random(tc2);
      // section_rc_style(tc2);
      // section_R_over_C_style(tc2);
      // section_CR_style(tc2);
      // section_Cr_style(tc2);

      REQUIRE(tc2.number_of_classes() == 1);
      tc2.shrink_to_fit();

      presentation::add_rule_and_check(p, {0}, {1});
      ToddCoxeter tc3(knd, p);
      REQUIRE(tc3.number_of_classes() == 1);
      tc3.shrink_to_fit();
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

      section_hlt(tc);
      // section_random(tc);

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
      section_hlt(tc);
      section_felsch(tc);

      REQUIRE(tc.number_of_classes() == 5);
      REQUIRE(tc.class_index_to_word(0) == word_type({0}));
      // This next one should throw
      REQUIRE_THROWS_AS(make<FroidurePin<TCE>>(tc), LibsemigroupsException);
    }
    {
      ToddCoxeter tc(twosided, p);
      section_hlt(tc);
      section_felsch(tc);
      // section_random(tc);
      // section_rc_style(tc);
      // section_R_over_C_style(tc);
      // section_CR_style(tc);
      // section_Cr_style(tc);

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
    // section_hlt_no_save(tc2);
    // section_hlt_save_throws(tc2);
    section_felsch(tc2);
    // section_random(tc2);
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
    // section_random(*tc);
    // section_hlt(*tc);
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
    tc2.lookahead_next(1);
    tc2.report_every(1);
    section_hlt(tc2);
    section_felsch(tc2);
    // section_random(tc2);
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
    // section_hlt_no_save(tc);
    // section_hlt_save_throws(tc);
    // section_random(tc);
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
    section_hlt(tc1);
    section_felsch(tc1);
    // section_random(tc1);
    // section_rc_style(tc1);
    // section_R_over_C_style(tc1);
    // section_CR_style(tc1);
    // section_Cr_style(tc1);

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

    auto rg = ReportGuard(true);

    auto p = make<Presentation<word_type>>(stylic_monoid(9));
    presentation::remove_duplicate_rules(p);
    presentation::sort_rules(p);
    presentation::sort_each_rule(p);
    REQUIRE(presentation::length(p) == 1'467);

    // Makes this an extreme test
    // SECTION("preprocessing of the presentation . . .") {
    //   // presentation::greedy_reduce_length(p);
    //   // REQUIRE(presentation::length(p) == 1'095);
    //   // reducing the
    //   ToddCoxeter tc(twosided, p);
    //   tc.strategy(options::strategy::felsch);
    //   REQUIRE(tc.number_of_classes() == 115'974);
    // }
    ToddCoxeter tc(twosided, p);
    tc.strategy(options::strategy::hlt)
        .lookahead_extent(options::lookahead_extent::partial)
        .lookahead_style(options::lookahead_style::hlt);
    REQUIRE(tc.number_of_classes() == 115'974);

    // TODO uncomment
    // tc.strategy(options::strategy::random);
    // REQUIRE_THROWS_AS(tc.run_for(std::chrono::milliseconds(100)),
    //                   LibsemigroupsException);
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

    presentation::greedy_reduce_length(p);

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

    section_hlt(tc);
    section_felsch(tc);
    // section_random(tc);
    // section_R_over_C_style(tc);
    // section_rc_style(tc);
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
    section_hlt(tc);
    // section_random(tc);
    // section_R_over_C_style(tc);
    // section_rc_style(tc);
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
    tc.strategy(options::strategy::hlt)
        .lookahead_extent(options::lookahead_extent::partial);
    // tc.standardize(false)
    section_hlt(tc);
    section_felsch(tc);
    // section_random(tc);
    // section_R_over_C_style(tc);
    // section_rc_style(tc);

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
    section_hlt(tc);
    section_felsch(tc);
    // section_random(tc);
    // section_rc_style(tc);
    // section_R_over_C_style(tc);
    // section_CR_style(tc);
    // section_Cr_style(tc);

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
          ToddCoxeter tc(twosided, p);
      p.alphabet("ab");
      presentation::add_rule_and_check(p, "aaa", "a");
      presentation::add_rule_and_check(p, "a", "bb");
      section_hlt(tc);
      section_felsch(tc);
      // section_random(tc);
      // section_rc_style(tc);
      // section_R_over_C_style(tc);
      // section_CR_style(tc);
      // section_Cr_style(tc);
      SECTION("R/C + Felsch lookahead") {
        tc
            .strategy(options::strategy::R_over_C)
            .lookahead(options::lookahead::felsch |
  options::lookahead::full); tc.run();
      }
      SECTION("HLT + Felsch lookahead + save") {
        tc
            .strategy(options::strategy::hlt)
            .save(true)
            .lookahead(options::lookahead::felsch |
  options::lookahead::full) .lookahead_next(2); tc.run();
      }
      SECTION("Cr + small number of f_defs") {
        tc.strategy(options::strategy::Cr).f_defs(3);
        tc.run();
      }
      SECTION("Rc + small number of deductions") {
        tc.strategy(options::strategy::Rc).max_deductions(0);
        tc.run();
      }
      using digraph_type =
          typename ::libsemigroups::ToddCoxeter::digraph_type;
      SECTION("Felsch + v2 + no preferred defs") {
        tc
            .strategy(options::strategy::felsch)
            .deduction_version(digraph_type::process_defs::v2)
            .deduction_policy(DefinitionOptions::definitions::purge_all)
            .preferred_defs(options::preferred_defs::none);
      }
      SECTION("Felsch + v2 + immediate no stack") {
        tc
            .strategy(options::strategy::felsch)
            .deduction_version(digraph_type::process_defs::v2)
            .deduction_policy(DefinitionOptions::definitions::purge_from_top)
            .preferred_defs(options::preferred_defs::immediate_no_stack);
      }
      SECTION("Felsch + v1 + immediate no stack") {
        tc
            .strategy(options::strategy::felsch)
            .deduction_version(digraph_type::process_defs::v1)
            .deduction_policy(
                DefinitionOptions::definitions::discard_all_if_no_space)
            .preferred_defs(options::preferred_defs::immediate_no_stack);
      }
      SECTION("Felsch + v1 + immediate yes stack") {
        tc
            .strategy(options::strategy::felsch)
            .deduction_version(digraph_type::process_defs::v1)
            .deduction_policy(
                DefinitionOptions::definitions::no_stack_if_no_space)
            .preferred_defs(options::preferred_defs::immediate_yes_stack);
      }
      SECTION("large collapse") {
        tc.large_collapse(0);
      }

      REQUIRE(tc.number_of_classes() == 5);
    }
    {
          ToddCoxeter tc(twosided, p);
      Presentation<std::string> p;
  p.alphabet("ab");
      presentation::add_rule_and_check(p, "aaa", "a");
      presentation::add_rule_and_check(p, "a", "bb");
      tc.lookahead_next(1);
      section_hlt(tc);
      section_felsch(tc);
      // section_random(tc);
      // section_rc_style(tc);
      // section_R_over_C_style(tc);
      // section_CR_style(tc);
      // section_Cr_style(tc);

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
    section_hlt(tc);
    section_felsch(tc);
    // section_random(tc);
    // section_rc_style(tc);
    // section_R_over_C_style(tc);
    // section_CR_style(tc);
    // section_Cr_style(tc);
    //

    REQUIRE(tc.number_of_classes() == 24);
    REQUIRE(
        todd_coxeter::normal_form(tc, make<word_type>(p, "aaaaaaaaaaaaaaaaaaa"))
        == make<word_type>(p, "a"));
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
    presentation::greedy_reduce_length(p);
    REQUIRE(presentation::length(p) == 63);
    REQUIRE(p.alphabet() == "abcdefghijkl");

    presentation::remove_trivial_rules(p);
    presentation::remove_duplicate_rules(p);
    presentation::sort_rules(p);
    presentation::sort_each_rule(p);
    REQUIRE(p.rules
            == std::vector<std::string>(
                {"ad",    "",  "be",    "",  "cf",    "",  "da",    "",
                 "eb",    "",  "fc",    "",  "gjkii", "",  "hklgg", "",
                 "iljhh", "",  "ccefb", "g", "bbdea", "h", "aafdc", "i",
                 "dbaee", "l", "facdd", "j", "ecbff", "k"}));

    ToddCoxeter tc(twosided, p);

    tc.lookahead_extent(options::lookahead_extent::full)
        .lookahead_style(options::lookahead_style::felsch);

    REQUIRE(!is_obviously_infinite(tc));
    REQUIRE(tc.number_of_classes() == 1);
  }

  LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                          "067",
                          "Repeated construction from same FroidurePin",
                          "[todd-coxeter][quick]") {
    auto rg = ReportGuard(false);

    using Transf = LeastTransf<5>;

    FroidurePin<Transf> S({Transf({1, 3, 4, 2, 3}), Transf({3, 2, 1, 3, 3})});
    REQUIRE(S.size() == 88);
    REQUIRE(S.number_of_rules() == 18);

    word_type w1, w2, w3, w4;
    S.factorisation(w1, S.position(Transf({3, 4, 4, 4, 4})));
    S.factorisation(w2, S.position(Transf({3, 1, 3, 3, 3})));
    S.factorisation(w3, S.position(Transf({1, 3, 1, 3, 3})));
    S.factorisation(w4, S.position(Transf({4, 2, 4, 4, 2})));

    auto p = make<Presentation<word_type>>(S);

    ToddCoxeter tc1(twosided, p);
    tc1.add_pair(w1, w2);

    // section_hlt_no_save(tc1);
    // section_hlt_save_throws(tc1);
    section_felsch(tc1);
    // section_random(tc1);

    REQUIRE(tc1.number_of_classes() == 21);
    REQUIRE(tc1.number_of_classes() == make<FroidurePin<TCE>>(tc1).size());
    REQUIRE(tc1.contains(w3, w4));
    REQUIRE(todd_coxeter::normal_form(tc1, w3)
            == todd_coxeter::normal_form(tc1, w4));

    ToddCoxeter tc2(twosided, p);
    tc2.add_pair(w1, w2);

    // section_hlt_no_save(tc2);
    // section_hlt_save_throws(tc2);
    section_felsch(tc2);

    REQUIRE(tc2.number_of_classes() == 21);
    REQUIRE(tc2.number_of_classes() == make<FroidurePin<TCE>>(tc2).size());
    REQUIRE(tc2.contains(w3, w4));
    REQUIRE(todd_coxeter::normal_form(tc2, w3)
            == todd_coxeter::normal_form(tc2, w4));
  }

  LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                          "068",
                          "Sym(5) from Chapter 3, Proposition 1.1 in NR",
                          "[todd-coxeter][quick]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("ABabe");
    presentation::add_identity_rules(p, 'e');
    presentation::add_rule_and_check(p, "aa", "e");
    presentation::add_rule_and_check(p, "bbbbb", "e");
    presentation::add_rule_and_check(p, "babababa", "e");
    presentation::add_rule_and_check(p, "bB", "e");
    presentation::add_rule_and_check(p, "Bb", "e");
    presentation::add_rule_and_check(p, "BabBab", "e");
    presentation::add_rule_and_check(p, "aBBabbaBBabb", "e");
    presentation::add_rule_and_check(p, "aBBBabbbaBBBabbb", "e");
    presentation::add_rule_and_check(p, "aA", "e");
    presentation::add_rule_and_check(p, "Aa", "e");

    ToddCoxeter tc(twosided, p);
    // TODO uncomment
    // SECTION("definition policy == purge_from_top") {
    //   tc
    //       .max_definitions(2)
    //       .strategy(options::strategy::felsch)
    //       .max_preferred_defs(3);
    //   REQUIRE_THROWS_AS(tc.definition_policy(
    //                         DefinitionOptions::definitions::purge_from_top),
    //                     LibsemigroupsException);
    //   tc
    //       .definition_version(digraph_type::process_defs::v1)
    //       .definition_policy(DefinitionOptions::definitions::purge_from_top);
    // }
    // SECTION("definition policy == purge_all") {
    //   tc.max_definitions(2).strategy(options::strategy::felsch);
    //   tc
    //       .definition_version(digraph_type::process_defs::v1)
    //       .definition_policy(DefinitionOptions::definitions::purge_all);
    // }
    // SECTION("definition policy == discard_all_if_no_space") {
    //   tc.max_definitions(2).strategy(options::strategy::felsch);
    //   tc
    //       .definition_version(digraph_type::process_defs::v2)
    //       .definition_policy(
    //           DefinitionOptions::definitions::discard_all_if_no_space);
    // }
    section_hlt(tc);
    section_felsch(tc);
    // section_random(tc);
    // section_rc_style(tc);
    // section_R_over_C_style(tc);
    // section_CR_style(tc);
    // section_Cr_style(tc);

    REQUIRE(tc.number_of_classes() == 120);
  }

  LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                          "069",
                          "Chapter 7, Theorem 3.6 in NR (size 243)",
                          "[no-valgrind][todd-coxeter][quick]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("ab");
    presentation::add_rule_and_check(p, "aaa", "a");
    presentation::add_rule_and_check(p, "bbbb", "b");
    presentation::add_rule_and_check(p, "ababababab", "aa");

    ToddCoxeter tc(twosided, p);

    section_hlt(tc);
    section_felsch(tc);
    // section_random(tc);
    // section_rc_style(tc);
    // section_R_over_C_style(tc);
    // section_CR_style(tc);
    // section_Cr_style(tc);

    REQUIRE(tc.number_of_classes() == 243);
  }

  LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                          "070",
                          "finite semigroup (size 99)",
                          "[todd-coxeter][quick]") {
    auto                      rg = ReportGuard(true);
    Presentation<std::string> p;
    p.alphabet("ab");
    presentation::add_rule_and_check(p, "aaa", "a");
    presentation::add_rule_and_check(p, "bbbb", "b");
    presentation::add_rule_and_check(p, "abababab", "aa");

    ToddCoxeter tc(twosided, p);
    REQUIRE(!is_obviously_infinite(tc));

    section_hlt(tc);
    section_felsch(tc);
    // section_random(tc);
    // section_rc_style(tc);
    // section_R_over_C_style(tc);
    // section_CR_style(tc);
    // section_Cr_style(tc);

    REQUIRE(tc.number_of_classes() == 99);
    REQUIRE(tc.finished());
    // REQUIRE(tc.is_obviously_finite());
  }

  // The following 8 examples are from Trevor Walker's Thesis: Semigroup
  // enumeration - computer implementation and applications, p41.
  LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                          "071",
                          "Walker 1",
                          "[todd-coxeter][standard]") {
    auto                      rg = ReportGuard(true);
    Presentation<std::string> p;
    p.alphabet("abcABCDEFGHIXYZ");
    presentation::add_rule_and_check(p, "A", "aaaaaaaaaaaaaa");
    presentation::add_rule_and_check(p, "B", "bbbbbbbbbbbbbb");
    presentation::add_rule_and_check(p, "C", "cccccccccccccc");
    presentation::add_rule_and_check(p, "D", "aaaaba");
    presentation::add_rule_and_check(p, "E", "bbbbab");
    presentation::add_rule_and_check(p, "F", "aaaaca");
    presentation::add_rule_and_check(p, "G", "ccccac");
    presentation::add_rule_and_check(p, "H", "bbbbcb");
    presentation::add_rule_and_check(p, "I", "ccccbc");
    presentation::add_rule_and_check(p, "X", "aaa");
    presentation::add_rule_and_check(p, "Y", "bbb");
    presentation::add_rule_and_check(p, "Z", "ccc");

    presentation::add_rule_and_check(p, "A", "a");
    presentation::add_rule_and_check(p, "B", "b");
    presentation::add_rule_and_check(p, "C", "c");
    presentation::add_rule_and_check(p, "D", "Y");
    presentation::add_rule_and_check(p, "E", "X");
    presentation::add_rule_and_check(p, "F", "Z");
    presentation::add_rule_and_check(p, "G", "X");
    presentation::add_rule_and_check(p, "H", "Z");
    presentation::add_rule_and_check(p, "I", "Y");

    REQUIRE(presentation::length(p) == 117);

    presentation::sort_rules(p);
    presentation::reduce_complements(p);
    presentation::remove_redundant_generators(p);
    presentation::remove_trivial_rules(p);
    REQUIRE(presentation::length(p) == 105);

    presentation::normalize_alphabet(p);
    REQUIRE(p.alphabet() == "abc");

    presentation::sort_rules(p);
    presentation::sort_each_rule(p);
    REQUIRE(p.rules
            == std::vector<std::string>({"bbbbab",
                                         "aaa",
                                         "aaaaba",
                                         "bbb",
                                         "bbbbcb",
                                         "ccc",
                                         "ccccac",
                                         "aaa",
                                         "bbbbcb",
                                         "aaaaca",
                                         "ccccbc",
                                         "aaaaba",
                                         "aaaaaaaaaaaaaa",
                                         "a",
                                         "bbbbbbbbbbbbbb",
                                         "b",
                                         "cccccccccccccc",
                                         "c"}));

    ToddCoxeter tc(twosided, p);
    tc.run_until([&tc]() -> bool {
      return tc.word_graph().number_of_nodes() >= 10'000;
    });
    tc.lookahead_next(100'000);
    REQUIRE(!tc.finished());
    REQUIRE(!is_obviously_infinite(tc));
    tc.standardize(order::shortlex);
    REQUIRE(!tc.finished());
    tc.standardize(order::lex);
    REQUIRE(!tc.finished());
    tc.standardize(order::recursive);
    REQUIRE(!tc.finished());

    section_hlt(tc);
    section_felsch(tc);
    // section_random(tc);
    // section_rc_style(tc);
    // section_R_over_C_style(tc);
    // section_CR_style(tc);
    // section_Cr_style(tc);

    REQUIRE(tc.number_of_classes() == 1);
    tc.standardize(order::shortlex);
    REQUIRE(std::is_sorted(todd_coxeter::cbegin_normal_forms(tc),
                           todd_coxeter::cend_normal_forms(tc),
                           ShortLexCompare()));
    tc.standardize(order::lex);
    REQUIRE(std::is_sorted(todd_coxeter::cbegin_normal_forms(tc),
                           todd_coxeter::cend_normal_forms(tc),
                           LexicographicalCompare()));
    tc.standardize(order::recursive);
    REQUIRE(std::is_sorted(todd_coxeter::cbegin_normal_forms(tc),
                           todd_coxeter::cend_normal_forms(tc),
                           RecursivePathCompare()));
  }

  // The following example is a good one for using the lookahead.
  // This is no longer extreme with the preprocessing
  LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                          "072",
                          "Walker 2",
                          "[todd-coxeter][quick]") {
    auto                      rg = ReportGuard(true);
    Presentation<std::string> p;
    p.alphabet("ab");
    presentation::add_rule_and_check(
        p, "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", "a");
    presentation::add_rule_and_check(p, "bbb", "b");
    presentation::add_rule_and_check(p, "ababa", "b");
    presentation::add_rule_and_check(
        p, "aaaaaaaaaaaaaaaabaaaabaaaaaaaaaaaaaaaabaaaa", "b");

    REQUIRE(presentation::length(p) == 87);
    presentation::greedy_reduce_length(p);
    REQUIRE(presentation::length(p) == 31);
    REQUIRE(p.alphabet() == "abcd");
    REQUIRE(p.rules
            == std::vector<std::string>({"dd",
                                         "a",
                                         "bbb",
                                         "b",
                                         "ababa",
                                         "b",
                                         "dbcbdbc",
                                         "b",
                                         "c",
                                         "aaaa",
                                         "d",
                                         "cccc"}));

    ToddCoxeter tc(twosided, p);

    REQUIRE(!is_obviously_infinite(tc));

    SECTION("custom HLT") {
      tc.lookahead_next(1'000'000)
          .max_definitions(2'000)
          // .use_relations_in_extra(true)
          .strategy(options::strategy::hlt)
          .lookahead_extent(options::lookahead_extent::partial)
          .lookahead_style(options::lookahead_style::felsch);
      // .deduction_version(digraph_type::process_defs::v2)
      // .deduction_policy(
      //     DefinitionOptions::definitions::no_stack_if_no_space);
    }

    section_hlt(tc);
    section_felsch(tc);
    // section_random(tc);
    // section_rc_style(tc);
    // section_R_over_C_style(tc);
    // section_CR_style(tc);
    // section_Cr_style(tc);
    REQUIRE(tc.number_of_classes() == 14'911);
    // check_standardize(tc);
  }

  // TODO This is much slower than in v2
  LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                          "073",
                          "Walker 3",
                          "[todd-coxeter][standard]") {
    auto                      rg = ReportGuard(true);
    Presentation<std::string> p;
    p.alphabet("abc");
    presentation::add_rule_and_check(p, "cc", "a");
    presentation::add_rule_and_check(p, "aaaaaaaa", "c");
    presentation::add_rule_and_check(p, "bbbbbbbbbbbbbbbb", "b");
    presentation::add_rule_and_check(p, "abb", "baa");

    REQUIRE(presentation::length(p) == 35);
    SECTION("preprocess the presentation") {
      presentation::greedy_reduce_length(p);
      REQUIRE(presentation::length(p) == 26);
      REQUIRE(p.alphabet() == "abcde");
    }
    ToddCoxeter tc(twosided, p);
    tc.lookahead_next(2'000'000);
    REQUIRE(!is_obviously_infinite(tc));

    section_hlt(tc);
    section_felsch(tc);
    // section_random(tc);
    // section_rc_style(tc); // Rc_style + partial lookahead works very badly
    // 2m30s
    // section_R_over_C_style(tc);
    // section_Cr_style(tc);

    REQUIRE(tc.number_of_classes() == 20'490);
  }

  LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                          "074",
                          "Walker 4",
                          "[todd-coxeter][extreme]") {
    auto                      rg = ReportGuard();
    Presentation<std::string> p;
    p.alphabet("ab");
    presentation::add_rule_and_check(p, "aaa", "a");
    presentation::add_rule_and_check(p, "bbbbbb", "b");
    presentation::add_rule_and_check(
        p, "ababbbbababbbbababbbbababbbbababbbbababbbbababbbbabba", "bb");

    REQUIRE(presentation::length(p) == 66);

    ToddCoxeter tc(twosided, p);
    tc.lookahead_next(3'000'000);

    REQUIRE(!is_obviously_infinite(tc));

    section_hlt(tc);

    // Felsch very slow without preprocessing
    SECTION("preprocessing + Felsch") {
      presentation::greedy_reduce_length(p);
      REQUIRE(presentation::length(p) == 29);
      REQUIRE(p.alphabet() == "abcd");
      p.rules = std::vector<std::string>({"aaa",
                                          "a",
                                          "dbb",
                                          "b",
                                          "abeceba",
                                          "bb",
                                          "c",
                                          "adab",
                                          "d",
                                          "bbbb",
                                          "ccc",
                                          "e"});
      p.alphabet_from_rules();
      tc.init(twosided, p);
      tc.strategy(options::strategy::felsch);
    }
    // TODO uncomment
    // section_random(tc);
    // SECTION("custom R/C") {
    //   tc.lookahead_next(3'000'000)
    //       .strategy(options::strategy::R_over_C)
    //       .max_deductions(100'000);
    // }
    REQUIRE(tc.number_of_classes() == 36'412);
  }

  LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                          "075",
                          "Walker 5",
                          "[todd-coxeter][extreme]") {
    auto                      rg = ReportGuard(true);
    Presentation<std::string> p;
    p.alphabet("ab");
    presentation::add_rule_and_check(p, "aaa", "a");
    presentation::add_rule_and_check(p, "bbbbbb", "b");
    presentation::add_rule_and_check(
        p,
        "ababbbbababbbbababbbbababbbbababbbbababbbbababbbbabbabbbbbaa",
        "bb");
    REQUIRE(presentation::length(p) == 73);
    presentation::greedy_reduce_length(p);
    REQUIRE(presentation::length(p) == 34);
    REQUIRE(p.alphabet() == "abcd");
    REQUIRE(p.rules
            == std::vector<std::string>({"aaa",
                                         "a",
                                         "ddd",
                                         "b",
                                         "abcccccccbaddbaa",
                                         "d",
                                         "c",
                                         "addab",
                                         "d",
                                         "bb"}));
    presentation::replace_subword(p, "ccc");
    REQUIRE(presentation::length(p) == 34);

    ToddCoxeter tc(twosided, p);
    tc.lookahead_next(5'000'000);
    REQUIRE(!is_obviously_infinite(tc));

    // This example is extremely slow with Felsch (even with the preprocessing)
    section_hlt(tc);
    // section_random(tc);
    // section_rc_style(tc);
    // section_R_over_C_style(tc);
    // section_CR_style(tc);
    // section_Cr_style(tc);

    REQUIRE(tc.number_of_classes() == 72'822);
    check_complete_compatible(tc);
    // TODO uncomment std::cout << tc.stats_string();
  }

  LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                          "076",
                          "not Walker 6",
                          "[todd-coxeter][extreme]") {
    auto                      rg = ReportGuard();
    Presentation<std::string> p;
    p.alphabet("ab");
    presentation::add_rule_and_check(p, "aaa", "a");
    presentation::add_rule_and_check(p, "bbbbbbbbb", "b");
    presentation::add_rule_and_check(
        p,
        "ababbbbababbbbababbbbababbbbababbbbababbbbababbbbabbabbbbbbbb",
        "bb");
    REQUIRE(presentation::length(p) == 77);

    ToddCoxeter tc(twosided, p);
    tc.lookahead_next(5'000'000);
    REQUIRE(!is_obviously_infinite(tc));

    // the random strategy
    // strategy is typically fastest
    section_hlt(tc);
    // Takes about 3.5s with preprocessing
    SECTION("preprocessing + Felsch") {
      presentation::greedy_reduce_length(p);
      REQUIRE(presentation::length(p) == 33);
      REQUIRE(p.alphabet() == "abcd");
      REQUIRE(p.rules
              == std::vector<std::string>({"aaa",
                                           "a",
                                           "ddb",
                                           "b",
                                           "abcccccccbadd",
                                           "bb",
                                           "c",
                                           "adab",
                                           "d",
                                           "bbbb"}));
      presentation::replace_subword(p, "ccc");
      REQUIRE(presentation::length(p) == 33);
      tc.init(twosided, p);
    }
    // section_random(tc);
    // section_rc_style(tc);
    // section_R_over_C_style(tc);
    // section_CR_style(tc);
    // section_Cr_style(tc);

    REQUIRE(tc.number_of_classes() == 8);
  }

  LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                          "077",
                          "Walker 6",
                          "[todd-coxeter][standard]") {
    auto                      rg = ReportGuard(true);
    Presentation<std::string> p;
    p.alphabet("ab");
    presentation::add_rule_and_check(p, "aaa", "a");
    presentation::add_rule_and_check(p, "bbbbbbbbb", "b");
    std::string lng("ababbbbbbb");
    lng += lng;
    lng += "abbabbbbbbbb";
    presentation::add_rule_and_check(p, lng, std::string("bb"));

    REQUIRE(presentation::length(p) == 48);
    presentation::greedy_reduce_length(p);
    REQUIRE(presentation::length(p) == 28);
    REQUIRE(p.alphabet() == "abcde");
    REQUIRE(p.rules
            == std::vector<std::string>({"aaa",
                                         "a",
                                         "cd",
                                         "b",
                                         "aeedacb",
                                         "d",
                                         "c",
                                         "dddb",
                                         "d",
                                         "bb",
                                         "e",
                                         "baca"}));

    presentation::replace_subword(p, "bbb");
    REQUIRE(presentation::length(p) == 32);

    ToddCoxeter tc(twosided, p);
    REQUIRE(!is_obviously_infinite(tc));

    // This example is extremely slow with Felsch, not anymore with the
    // preprocessing above
    section_hlt(tc);
    section_felsch(tc);
    // section_random(tc);
    // section_rc_style(tc); // partial lookahead is too slow
    // section_Cr_style(tc); // very slow
    // section_R_over_C_style(tc);

    REQUIRE(tc.number_of_classes() == 78'722);
  }

  // Felsch is faster here too!
  LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                          "078",
                          "Walker 7",
                          "[todd-coxeter][standard]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("abcde");
    presentation::add_rule_and_check(p, "aaa", "a");
    presentation::add_rule_and_check(p, "bbb", "b");
    presentation::add_rule_and_check(p, "ccc", "c");
    presentation::add_rule_and_check(p, "ddd", "d");
    presentation::add_rule_and_check(p, "eee", "e");
    presentation::add_rule_and_check(p, "ababab", "aa");
    presentation::add_rule_and_check(p, "bcbcbc", "bb");
    presentation::add_rule_and_check(p, "cdcdcd", "cc");
    presentation::add_rule_and_check(p, "dedede", "dd");
    presentation::add_rule_and_check(p, "ac", "ca");
    presentation::add_rule_and_check(p, "ad", "da");
    presentation::add_rule_and_check(p, "ae", "ea");
    presentation::add_rule_and_check(p, "bd", "db");
    presentation::add_rule_and_check(p, "be", "eb");
    presentation::add_rule_and_check(p, "ce", "ec");

    ToddCoxeter tc(twosided, p);
    REQUIRE(!is_obviously_infinite(tc));

    SECTION("def_version == one") {
      tc.definition_version(felsch_digraph::def_version::one);
    }
    SECTION("def_version == two") {
      tc.definition_version(felsch_digraph::def_version::two);
    }

    section_hlt(tc);
    section_felsch(tc);
    // section_random(tc);
    // section_rc_style(tc); // partial lookahead very slow ~8s
    // section_R_over_C_style(tc);
    // section_Cr_style(tc);

    //      .deduction_policy(
    //          DefinitionOptions::definitions::no_stack_if_no_space)
    //      .preferred_defs(options::preferred_defs::none);

    REQUIRE(tc.number_of_classes() == 153'500);
  }

  LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                          "079",
                          "Walker 8",
                          "[todd-coxeter][standard]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("ab");
    presentation::add_rule_and_check(p, "aaa", "a");
    presentation::add_rule_and_check(p, "bbbbbbbbbbbbbbbbbbbbbbb", "b");
    presentation::add_rule_and_check(p, "abbbbbbbbbbbabb", "bba");

    REQUIRE(presentation::length(p) == 46);
    presentation::greedy_reduce_length(p);
    REQUIRE(presentation::length(p) == 24);
    REQUIRE(p.alphabet() == "abcd");
    REQUIRE(
        p.rules
        == std::vector<std::string>(
            {"aaa", "a", "ccb", "b", "acad", "da", "c", "dddddb", "d", "bb"}));

    ToddCoxeter tc(twosided, p);
    tc.lookahead_next(500'000);

    REQUIRE(!is_obviously_infinite(tc));

    section_hlt(tc);
    section_felsch(tc);
    // section_random(tc);
    // section_rc_style(tc); + partial lookahead too slow
    // section_Cr_style(tc); // too slow
    // section_R_over_C_style(tc);

    check_complete_compatible(tc);

    REQUIRE(tc.number_of_classes() == 270'272);
  }

  LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                          "080",
                          "KnuthBendix 098",
                          "[todd-coxeter][quick][no-valgrind]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("aAbBcCdDyYfFgG");
    p.contains_empty_word(true);
    presentation::add_inverse_rules(p, "AaBbCcDdYyFfGg");
    presentation::add_rule_and_check(p, "ab", "c");
    presentation::add_rule_and_check(p, "bc", "d");
    presentation::add_rule_and_check(p, "cd", "y");
    presentation::add_rule_and_check(p, "dy", "f");
    presentation::add_rule_and_check(p, "yf", "g");
    presentation::add_rule_and_check(p, "fg", "a");
    presentation::add_rule_and_check(p, "ga", "b");
    ToddCoxeter tc(twosided, p);

    section_hlt(tc);
    section_felsch(tc);
    // section_random(tc);
    // section_rc_style(tc);
    // section_R_over_C_style(tc);
    // section_CR_style(tc);
    // section_Cr_style(tc);

    REQUIRE(tc.number_of_classes() == 29);
  }

  // TODO add to fpsemi-examples
  LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                          "081",
                          "Holt 2 - SL(2, p)",
                          "[todd-coxeter][extreme]") {
    auto rg = ReportGuard();

    Presentation<std::string> p;
    p.alphabet("xXyY");
    p.contains_empty_word(true);
    presentation::add_inverse_rules(p, "XxYy");
    presentation::add_rule_and_check(p, "xxYXYXYX", "");

    auto second = [](size_t p) -> std::string {
      std::string s = "xyyyyx";
      s += std::string((p + 1) / 2, 'y');
      s += s;
      s += std::string(p, 'y');
      s += std::string(2 * (p / 3), 'x');
      return s;
    };

    std::string id = "";
    REQUIRE(second(3) == "xyyyyxyyxyyyyxyyyyyxx");
    SECTION("p = 3") {
      presentation::add_rule_and_check(p, second(3), id);
      ToddCoxeter tc(twosided, p);

      section_hlt(tc);
      section_felsch(tc);

      REQUIRE(tc.number_of_classes() == 24);
    }
    SECTION("p = 5") {
      presentation::add_rule_and_check(p, second(5), id);

      ToddCoxeter tc(twosided, p);
      section_hlt(tc);
      section_felsch(tc);

      REQUIRE(tc.number_of_classes() == 120);
    }
    SECTION("p = 7") {
      presentation::add_rule_and_check(p, second(7), id);

      ToddCoxeter tc(twosided, p);
      section_hlt(tc);
      section_felsch(tc);

      REQUIRE(tc.number_of_classes() == 336);
    }
    SECTION("p = 11") {
      presentation::add_rule_and_check(p, second(11), id);
      // REQUIRE(presentation::length(p) == 57);
      // presentation::greedy_reduce_length(p);
      // REQUIRE(presentation::length(p) == 40);
      // REQUIRE(p.alphabet() == "xXyYab");
      // presentation::replace_subword(p, "xayyaabb");

      ToddCoxeter tc(twosided, p);
      section_hlt(tc);
      // section_random(tc);

      REQUIRE(tc.number_of_classes() == 1'320);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                          "082",
                          "Holt 3",
                          "[todd-coxeter][standard]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("aAbBcC");
    p.contains_empty_word(true);
    presentation::add_inverse_rules(p, "AaBbCc");

    presentation::add_rule_and_check(p, "bbCbc", "");
    presentation::add_rule_and_check(p, "aaBab", "");
    presentation::add_rule_and_check(p, "cABcabc", "");

    ToddCoxeter tc(twosided, p);
    // TODO implement
    // REQUIRE(tc.is_non_trivial() == tril::TRUE);

    section_hlt(tc);
    section_felsch(tc);
    // section_random(tc);
    // section_rc_style(tc);
    // section_R_over_C_style(tc);
    // section_CR_style(tc);
    // section_Cr_style(tc);

    REQUIRE(tc.number_of_classes() == 6'561);
  }

  LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                          "083",
                          "Holt 3",
                          "[todd-coxeter][fail]") {
    auto                      rg = ReportGuard();
    Presentation<std::string> p;
    p.alphabet("aAbBcC");
    p.contains_empty_word(true);
    presentation::add_inverse_rules(p, "AaBbCc");

    presentation::add_rule_and_check(p, "aaCac", "");
    presentation::add_rule_and_check(p, "acbbACb", "");
    presentation::add_rule_and_check(p, "ABabccc", "");
    presentation::remove_duplicate_rules(p);
    presentation::sort_rules(p);
    REQUIRE(p.rules.size() == 18);

    ToddCoxeter tc(twosided, p);
    REQUIRE(!is_obviously_infinite(tc));
    // TODO uncomment tc.strategy(options::strategy::R_over_C);

    // TODO uncomment
    // tc.lookahead(options::lookahead::partial | options::lookahead::hlt)
    //     .lookahead_growth_factor(1.01)
    //     .lookahead_growth_threshold(10)
    //     .f_defs(250'000)
    //     .hlt_defs(20'000'000);
    // TODO implement
    // REQUIRE(tc.is_non_trivial() == tril::TRUE);
    REQUIRE(tc.number_of_classes() == 6'561);
  }

  LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                          "084",
                          "Campbell-Reza 1",
                          "[todd-coxeter][quick]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("ab");
    presentation::add_rule_and_check(p, "aa", "bb");
    presentation::add_rule_and_check(p, "ba", "aaaaaab");

    ToddCoxeter tc(twosided, p);
    section_hlt(tc);
    section_felsch(tc);
    // section_random(tc);
    // section_rc_style(tc);
    // section_R_over_C_style(tc);
    // section_CR_style(tc);
    // section_Cr_style(tc);

    REQUIRE(tc.number_of_classes() == 14);
    tc.standardize(order::shortlex);
    REQUIRE(std::vector<word_type>(todd_coxeter::cbegin_normal_forms(tc),
                                   todd_coxeter::cend_normal_forms(tc))
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
    REQUIRE(make<FroidurePin<TCE>>(tc).number_of_rules() == 6);
    REQUIRE(todd_coxeter::normal_form(tc, make<word_type>(p, "aaaaaaab"))
            == make<word_type>(p, "aab"));
    REQUIRE(todd_coxeter::normal_form(tc, make<word_type>(p, "bab"))
            == make<word_type>(p, "aaa"));
  }

  // The next example demonstrates why we require deferred standardization
  // FIXME this is also very slow in v3 (661ms vs 45ms in v2)
  LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                          "085",
                          "Renner monoid type D4 (Gay-Hivert), q = 1",
                          "[no-valgrind][quick][todd-coxeter][no-coverage]") {
    using fpsemigroup::make;
    using fpsemigroup::RennerTypeDMonoid;

    auto rg = ReportGuard(false);
    auto p  = make<Presentation<word_type>>(RennerTypeDMonoid(4, 1));
    REQUIRE(p.rules.size() == 252);

    ToddCoxeter tc(twosided, p);
    REQUIRE(!is_obviously_infinite(tc));

    section_felsch(tc);

    REQUIRE(tc.number_of_classes() == 10'625);

    section_hlt(tc);
    // section_random(tc);
    // section_rc_style(tc);
    // section_R_over_C_style(tc);
    // section_CR_style(tc);
    // section_Cr_style(tc);

    tc.standardize(order::shortlex);
    REQUIRE(std::is_sorted(todd_coxeter::cbegin_normal_forms(tc),
                           todd_coxeter::cend_normal_forms(tc),
                           ShortLexCompare()));
    tc.standardize(order::lex);
    REQUIRE(std::is_sorted(todd_coxeter::cbegin_normal_forms(tc),
                           todd_coxeter::cend_normal_forms(tc),
                           LexicographicalCompare()));
    // The next section is very slow
    // tc.standardize(order::recursive);
    // REQUIRE(std::is_sorted(todd_coxeter::cbegin_normal_forms(tc),
    //                        todd_coxeter::cend_normal_forms(tc),
    //                        RecursivePathCompare()));
  }

  // Felsch very slow here
  LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                          "086",
                          "trivial semigroup",
                          "[no-valgrind][todd-coxeter][quick][no-coverage]") {
    auto rg = ReportGuard(true);

    for (size_t N = 2; N < 1000; N += 199) {
      Presentation<std::string> p;
      p.alphabet("ab");
      p.contains_empty_word(true);
      std::string lhs = "a" + std::string(N, 'b');
      presentation::add_rule_and_check(p, lhs, std::string(""));

      lhs = std::string(N, 'a');
      std::string rhs(N + 1, 'b');
      presentation::add_rule_and_check(p, lhs, rhs);

      lhs = "ba";
      rhs = std::string(N, 'b') + "a";
      presentation::add_rule_and_check(p, lhs, rhs);
      presentation::greedy_reduce_length(p);

      ToddCoxeter tc(twosided, p);
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
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("abcABC");
    p.contains_empty_word(true);
    presentation::add_inverse_rules(p, "ABCabc");
    presentation::add_rule_and_check(p, "aBCbac", "");
    presentation::add_rule_and_check(p, "bACbaacA", "");
    presentation::add_rule_and_check(p, "accAABab", "");

    ToddCoxeter H(right, p);
    H.add_pair({1, 2}, {});
    H.lookahead_next(1'000'000);

    REQUIRE(H.number_of_classes() == 16'384);
  }

  LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                          "088",
                          "ACE --- 2p17-2p3 - HLT",
                          "[todd-coxeter][standard][ace]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("abcABC");
    p.contains_empty_word(true);
    presentation::add_inverse_rules(p, "ABCabc");
    presentation::add_rule_and_check(p, "aBCbac", "");
    presentation::add_rule_and_check(p, "bACbaacA", "");
    presentation::add_rule_and_check(p, "accAABab", "");

    ToddCoxeter H(right, p);
    H.add_pair(make<word_type>(p, "bc"), make<word_type>(p, ""));
    H.add_pair(make<word_type>(p, "bc"), make<word_type>(p, "ABAAbcabC"));

    H.strategy(options::strategy::hlt)
        .lookahead_extent(options::lookahead_extent::partial);
    // TODO uncomment
    // .save(true);

    REQUIRE(H.number_of_classes() == 8);
  }

  LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                          "089",
                          "ACE --- 2p17-1a - HLT",
                          "[todd-coxeter][standard][ace]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("abcABC");
    p.contains_empty_word(true);

    presentation::add_inverse_rules(p, "ABCabc");
    presentation::add_rule_and_check(p, "aBCbac", "");
    presentation::add_rule_and_check(p, "bACbaacA", "");
    presentation::add_rule_and_check(p, "accAABab", "");

    ToddCoxeter H(right, p);
    H.add_pair(make<word_type>(p, "bc"), make<word_type>(p, ""));
    H.add_pair(make<word_type>(p, "ABAAbcabC"), make<word_type>(p, ""));
    H.add_pair(make<word_type>(p, "AcccacBcA"), make<word_type>(p, ""));
    // TODO uncomment
    // H.large_collapse(10'000);
    H.strategy(options::strategy::hlt)
        //    .save(true)
        .lookahead_extent(options::lookahead_extent::partial);
    REQUIRE(H.number_of_classes() == 1);
  }

  LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                          "090",
                          "ACE --- F27",
                          "[todd-coxeter][standard][ace]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("abcdxyzABCDXYZ");
    p.contains_empty_word(true);

    presentation::add_inverse_rules(p, "ABCDXYZabcdxyz");
    presentation::add_rule_and_check(p, "abC", "");
    presentation::add_rule_and_check(p, "bcD", "");
    presentation::add_rule_and_check(p, "cdX", "");
    presentation::add_rule_and_check(p, "dxY", "");
    presentation::add_rule_and_check(p, "xyZ", "");
    presentation::add_rule_and_check(p, "yzA", "");
    presentation::add_rule_and_check(p, "zaB", "");

    ToddCoxeter H(twosided, p);
    section_felsch(H);
    section_hlt(H);
    // section_random(H);

    REQUIRE(H.number_of_classes() == 29);
  }

  LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                          "091",
                          "ACE --- SL219 - HLT",
                          "[todd-coxeter][standard][ace]") {
    auto                      rg = ReportGuard(true);
    Presentation<std::string> p;
    p.alphabet("abAB");
    p.contains_empty_word(true);
    presentation::add_inverse_rules(p, "ABab");
    presentation::add_rule_and_check(p, "aBABAB", "");
    presentation::add_rule_and_check(p, "BAAbaa", "");
    presentation::add_rule_and_check(
        p,
        "abbbbabbbbbbbbbbabbbbabbbbbbbbbbbbbbbbbbbbbbbbbbbbbaaaaaaaaaaaa",
        "");

    ToddCoxeter H;

    SECTION("preprocessing") {
      presentation::greedy_reduce_length(p);
      REQUIRE(presentation::length(p) == 49);
      H.init(right, p);
      section_hlt(H);
      // section_felsch(H); TOO slow
    }
    SECTION("no preprocessing") {
      REQUIRE(presentation::length(p) == 83);
      H.init(right, p);
      section_hlt(H);
      // section_felsch(H);  TOO slow
    }
    H.add_pair(make<word_type>(p, "b"), make<word_type>(p, ""));

    // H.strategy(options::strategy::hlt)
    //     .lookahead_extent(options::lookahead_extent::partial);
    // TODO uncomment
    //     .save(false)
    REQUIRE(H.number_of_classes() == 180);
  }

  LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                          "092",
                          "ACE --- perf602p5",
                          "[no-valgrind][todd-coxeter][quick][ace]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("abstuvdABSTUVD");
    p.contains_empty_word(true);
    presentation::add_inverse_rules(p, "ABSTUVDabstuvd");
    presentation::add_rule_and_check(p, "aaD", "");
    presentation::add_rule_and_check(p, "bbb", "");
    presentation::add_rule_and_check(p, "ababababab", "");
    presentation::add_rule_and_check(p, "ss", "");
    presentation::add_rule_and_check(p, "tt", "");
    presentation::add_rule_and_check(p, "uu", "");
    presentation::add_rule_and_check(p, "vv", "");
    presentation::add_rule_and_check(p, "dd", "");
    presentation::add_rule_and_check(p, "STst", "");
    presentation::add_rule_and_check(p, "UVuv", "");
    presentation::add_rule_and_check(p, "SUsu", "");
    presentation::add_rule_and_check(p, "SVsv", "");
    presentation::add_rule_and_check(p, "TUtu", "");
    presentation::add_rule_and_check(p, "TVtv", "");
    presentation::add_rule_and_check(p, "AsaU", "");
    presentation::add_rule_and_check(p, "AtaV", "");
    presentation::add_rule_and_check(p, "AuaS", "");
    presentation::add_rule_and_check(p, "AvaT", "");
    presentation::add_rule_and_check(p, "BsbDVT", "");
    presentation::add_rule_and_check(p, "BtbVUTS", "");
    presentation::add_rule_and_check(p, "BubVU", "");
    presentation::add_rule_and_check(p, "BvbU", "");
    presentation::add_rule_and_check(p, "DAda", "");
    presentation::add_rule_and_check(p, "DBdb", "");
    presentation::add_rule_and_check(p, "DSds", "");
    presentation::add_rule_and_check(p, "DTdt", "");
    presentation::add_rule_and_check(p, "DUdu", "");
    presentation::add_rule_and_check(p, "DVdv", "");

    ToddCoxeter H(right, p);
    H.add_pair(make<word_type>(p, "a"), make<word_type>(p, ""));

    section_hlt(H);
    // section_random(H);
    section_felsch(H);

    REQUIRE(H.number_of_classes() == 480);
  }

  LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                          "093",
                          "ACE --- M12",
                          "[todd-coxeter][standard][ace]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("abcABC");
    p.contains_empty_word(true);
    presentation::add_inverse_rules(p, "ABCabc");
    presentation::add_rule_and_check(p, "aaaaaaaaaaa", "");
    presentation::add_rule_and_check(p, "bb", "");
    presentation::add_rule_and_check(p, "cc", "");
    presentation::add_rule_and_check(p, "ababab", "");
    presentation::add_rule_and_check(p, "acacac", "");
    presentation::add_rule_and_check(p, "bcbcbcbcbcbcbcbcbcbc", "");
    presentation::add_rule_and_check(p, "cbcbabcbcAAAAA", "");

    ToddCoxeter H(twosided, p);

    SECTION("HLT + save + partial lookahead") {
      H.strategy(options::strategy::hlt)
          //       .save(true)
          .lookahead_extent(options::lookahead_extent::partial);
    }
    // TODO uncomment
    // SECTION("random") {
    //   H.strategy(options::strategy::random)
    //       .random_interval(std::chrono::milliseconds(100));
    // }
    section_felsch(H);

    REQUIRE(H.number_of_classes() == 95'040);
  }

  LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                          "094",
                          "ACE --- C5 - HLT",
                          "[todd-coxeter][quick][ace]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("abAB");
    p.contains_empty_word(true);

    presentation::add_inverse_rules(p, "ABab");
    presentation::add_rule_and_check(p, "aaaaa", "");
    presentation::add_rule_and_check(p, "b", "");

    ToddCoxeter H(twosided, p);

    section_hlt(H);
    // section_random(H);
    section_felsch(H);

    REQUIRE(H.number_of_classes() == 5);
  }

  LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                          "095",
                          "ACE --- A5-C5",
                          "[todd-coxeter][quick][ace]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("abAB");
    p.contains_empty_word(true);

    presentation::add_inverse_rules(p, "ABab");
    presentation::add_rule_and_check(p, "aa", "");
    presentation::add_rule_and_check(p, "bbb", "");
    presentation::add_rule_and_check(p, "ababababab", "");

    ToddCoxeter H(right, p);

    H.add_pair(make<word_type>(p, "ab"), make<word_type>(p, ""));

    section_hlt(H);
    // section_random(H);
    section_felsch(H);
    REQUIRE(H.number_of_classes() == 12);
  }

  LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                          "096",
                          "ACE --- A5",
                          "[todd-coxeter][quick][ace]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("abAB");
    p.contains_empty_word(true);

    presentation::add_inverse_rules(p, "ABab");
    presentation::add_rule_and_check(p, "aa", "");
    presentation::add_rule_and_check(p, "bbb", "");
    presentation::add_rule_and_check(p, "ababababab", "");

    ToddCoxeter H(twosided, p);

    section_hlt(H);
    // section_random(H);
    section_felsch(H);

    REQUIRE(H.number_of_classes() == 60);
  }

  // Felsch is much much better here (TODO slightly slower in v3)
  LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                          "097",
                          "relation ordering",
                          "[todd-coxeter][extreme]") {
    using fpsemigroup::make;
    using fpsemigroup::RennerTypeDMonoid;
    auto rg = ReportGuard();
    auto p  = make<Presentation<word_type>>(RennerTypeDMonoid(5, 1));
    REQUIRE(p.rules.size() == 358);
    presentation::sort_rules(p);
    presentation::remove_duplicate_rules(p);
    REQUIRE(p.rules.size() == 322);

    ToddCoxeter tc(twosided, p);
    REQUIRE(!is_obviously_infinite(tc));

    // TODO uncomment
    tc.strategy(options::strategy::felsch);  // .f_defs(100'000).run();
    REQUIRE(tc.number_of_classes() == 258'661);
  }

  LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                          "098",
                          "relation ordering",
                          "[todd-coxeter][quick]") {
    Presentation<word_type> p;
    p.alphabet(10);
    presentation::add_rule_and_check(p, {0, 1}, {0});
    presentation::add_rule_and_check(p, {0, 2}, {0});
    presentation::add_rule_and_check(p, {0, 3}, {0});
    presentation::add_rule_and_check(p, {0, 4}, {0});
    presentation::add_rule_and_check(p, {0, 5}, {0});
    presentation::add_rule_and_check(p, {0, 6}, {0});
    presentation::add_rule_and_check(p, {0, 7}, {0});
    presentation::add_rule_and_check(p, {0, 8}, {0});
    presentation::add_rule_and_check(p, {0, 9}, {0});
    presentation::add_rule_and_check(p, {1, 0}, {1});
    presentation::add_rule_and_check(p, {1, 1}, {1});
    presentation::add_rule_and_check(p, {1, 2}, {1});
    presentation::add_rule_and_check(p, {1, 3}, {1});
    presentation::add_rule_and_check(p, {1, 4}, {1});
    presentation::add_rule_and_check(p, {1, 5}, {1});
    presentation::add_rule_and_check(p, {1, 6}, {1});
    presentation::add_rule_and_check(p, {1, 7}, {1});
    presentation::add_rule_and_check(p, {1, 8}, {1});
    presentation::add_rule_and_check(p, {1, 9}, {1});
    presentation::add_rule_and_check(p, {2, 0}, {2});
    presentation::add_rule_and_check(p, {2, 1}, {2});
    presentation::add_rule_and_check(p, {2, 2}, {2});
    presentation::add_rule_and_check(p, {2, 3}, {2});
    presentation::add_rule_and_check(p, {2, 4}, {2});
    presentation::add_rule_and_check(p, {2, 5}, {2});
    presentation::add_rule_and_check(p, {2, 6}, {2});
    presentation::add_rule_and_check(p, {2, 7}, {2});
    presentation::add_rule_and_check(p, {2, 8}, {2});
    presentation::add_rule_and_check(p, {2, 9}, {2});
    presentation::add_rule_and_check(p, {3, 0}, {3});
    presentation::add_rule_and_check(p, {3, 1}, {3});
    presentation::add_rule_and_check(p, {3, 2}, {3});
    presentation::add_rule_and_check(p, {3, 3}, {3});
    presentation::add_rule_and_check(p, {3, 4}, {3});
    presentation::add_rule_and_check(p, {3, 5}, {3});
    presentation::add_rule_and_check(p, {3, 6}, {3});
    presentation::add_rule_and_check(p, {3, 7}, {3});
    presentation::add_rule_and_check(p, {3, 8}, {3});
    presentation::add_rule_and_check(p, {3, 9}, {3});
    presentation::add_rule_and_check(p, {4, 0}, {4});
    presentation::add_rule_and_check(p, {4, 1}, {4});
    presentation::add_rule_and_check(p, {4, 2}, {4});
    presentation::add_rule_and_check(p, {4, 3}, {4});
    presentation::add_rule_and_check(p, {4, 4}, {4});
    presentation::add_rule_and_check(p, {4, 5}, {4});
    presentation::add_rule_and_check(p, {4, 6}, {4});
    presentation::add_rule_and_check(p, {4, 7}, {4});
    presentation::add_rule_and_check(p, {4, 8}, {4});
    presentation::add_rule_and_check(p, {4, 9}, {4});
    presentation::add_rule_and_check(p, {5, 0}, {5});
    presentation::add_rule_and_check(p, {5, 1}, {5});
    presentation::add_rule_and_check(p, {5, 2}, {5});
    presentation::add_rule_and_check(p, {5, 3}, {5});
    presentation::add_rule_and_check(p, {5, 4}, {5});
    presentation::add_rule_and_check(p, {5, 5}, {5});
    presentation::add_rule_and_check(p, {5, 6}, {5});
    presentation::add_rule_and_check(p, {5, 7}, {5});
    presentation::add_rule_and_check(p, {5, 8}, {5});
    presentation::add_rule_and_check(p, {5, 9}, {5});
    presentation::add_rule_and_check(p, {6, 0}, {6});
    presentation::add_rule_and_check(p, {6, 1}, {6});
    presentation::add_rule_and_check(p, {6, 2}, {6});
    presentation::add_rule_and_check(p, {6, 3}, {6});
    presentation::add_rule_and_check(p, {6, 4}, {6});
    presentation::add_rule_and_check(p, {6, 5}, {6});
    presentation::add_rule_and_check(p, {6, 6}, {6});
    presentation::add_rule_and_check(p, {6, 7}, {6});
    presentation::add_rule_and_check(p, {6, 8}, {6});
    presentation::add_rule_and_check(p, {6, 9}, {6});
    presentation::add_rule_and_check(p, {7, 0}, {7});
    presentation::add_rule_and_check(p, {7, 1}, {7});
    presentation::add_rule_and_check(p, {7}, {7, 2});
    presentation::add_rule_and_check(p, {7, 3}, {7});
    presentation::add_rule_and_check(p, {7, 4}, {7});
    presentation::add_rule_and_check(p, {7, 5}, {7});
    presentation::add_rule_and_check(p, {7, 6}, {7});
    presentation::add_rule_and_check(p, {7, 7}, {7});
    presentation::add_rule_and_check(p, {7, 8}, {7});
    presentation::add_rule_and_check(p, {7, 9}, {7});
    presentation::add_rule_and_check(p, {8, 0}, {8});
    presentation::add_rule_and_check(p, {8, 1}, {8});
    presentation::add_rule_and_check(p, {8, 2}, {8});
    presentation::add_rule_and_check(p, {8, 3}, {8});
    presentation::add_rule_and_check(p, {8, 4}, {8});
    presentation::add_rule_and_check(p, {8, 5}, {8});
    presentation::add_rule_and_check(p, {8, 6}, {8});
    presentation::add_rule_and_check(p, {8, 7}, {8});
    presentation::add_rule_and_check(p, {8, 8}, {8});
    presentation::add_rule_and_check(p, {8, 9}, {8});
    presentation::add_rule_and_check(p, {9, 0}, {9});
    presentation::add_rule_and_check(
        p, {9, 0, 1, 2, 3, 4, 5, 5, 1, 5, 6, 9, 8, 8, 8, 8, 8, 0}, {9});

    // TODO uncomment tc.sort_generating_pairs(recursive_path_compare);

    ToddCoxeter tc(twosided, p);
    section_felsch(tc);
    section_hlt(tc);
    // section_random(tc);

    section_felsch(tc);
    section_hlt(tc);
    // section_random(tc);

    REQUIRE(tc.number_of_classes() == 10);
  }

  LIBSEMIGROUPS_TEST_CASE("v3::ToddCoxeter",
                          "099",
                          "short circuit size in obviously infinite",
                          "[todd-coxeter][quick]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("abc");
    presentation::add_rule_and_check(p, "aaaa", "a");
    ToddCoxeter tc(twosided, p);
    REQUIRE(tc.number_of_classes() == POSITIVE_INFINITY);
  }

  LIBSEMIGROUPS_TEST_CASE(
      "v3::ToddCoxeter",
      "100",
      "http://brauer.maths.qmul.ac.uk/Atlas/misc/24A8/mag/24A8G1-P1.M",
      "[todd-coxeter][standard]") {
    Presentation<std::string> p;
    p.alphabet("xyXY");
    p.contains_empty_word(true);

    presentation::add_inverse_rules(p, "XYxy");
    presentation::add_rule_and_check(p, "xx", "X");
    presentation::add_rule_and_check(p, "yyyyyy", "Y");
    presentation::add_rule_and_check(p, "YXyx", "XYxy");
    presentation::add_rule_and_check(p, "xYYYxYYYxYY", "yyXyyyXyyyX");
    presentation::add_rule_and_check(p, "xyxyyXyxYYxyyyx", "yyyXyyy");
    presentation::sort_rules(p);

    ToddCoxeter tc(twosided, p);

    tc.lookahead_next(2'000'000)
        .lookahead_extent(options::lookahead_extent::partial)
        .strategy(options::strategy::hlt);
    //     .standardize(true);
    section_hlt(tc);
    section_felsch(tc);  // about 6s with Felsch
    REQUIRE(tc.number_of_classes() == 322'560);
  }

  LIBSEMIGROUPS_TEST_CASE(
      "v3::ToddCoxeter",
      "101",
      "http://brauer.maths.qmul.ac.uk/Atlas/spor/M11/mag/M11G1-P1.M",
      "[todd-coxeter][quick][no-coverage][no-valgrind]") {
    Presentation<std::string> p;
    p.alphabet("xyXY");
    p.contains_empty_word(true);

    presentation::add_inverse_rules(p, "XYxy");
    presentation::add_rule_and_check(p, "xx", "");
    presentation::add_rule_and_check(p, "yyyy", "");
    presentation::add_rule_and_check(p, "xyxyxyxyxyxyxyxyxyxyxy", "");
    presentation::add_rule_and_check(p, "xyyxyyxyyxyyxyyxyy", "");
    presentation::add_rule_and_check(p, "xyxyxYxyxyyxYxyxYxY", "");

    ToddCoxeter tc(twosided, p);

    section_felsch(tc);
    section_hlt(tc);
    REQUIRE(tc.number_of_classes() == 7'920);
  }

  LIBSEMIGROUPS_TEST_CASE(
      "v3::ToddCoxeter",
      "102",
      "http://brauer.maths.qmul.ac.uk/Atlas/spor/M12/mag/M12G1-P1.M",
      "[todd-coxeter][standard]") {
    Presentation<std::string> p;
    p.alphabet("xyXY");
    p.contains_empty_word(true);

    presentation::add_inverse_rules(p, "XYxy");
    presentation::add_rule_and_check(p, "xx", "");
    presentation::add_rule_and_check(p, "yyy", "");
    presentation::add_rule_and_check(p, "xyxyxyxyxyxyxyxyxyxyxy", "");
    presentation::add_rule_and_check(p, "XYxyXYxyXYxyXYxyXYxyXYxy", "");
    presentation::add_rule_and_check(
        p, "xyxyxYxyxyxYxyxyxYxyxyxYxyxyxYxyxyxY", "");
    presentation::add_rule_and_check(
        p, "XYXYxyxyXYXYxyxyXYXYxyxyXYXYxyxyXYXYxyxy", "");

    ToddCoxeter tc(twosided, p);
    section_felsch(tc);
    section_hlt(tc);
    REQUIRE(tc.number_of_classes() == 95'040);
  }

  LIBSEMIGROUPS_TEST_CASE(
      "v3::ToddCoxeter",
      "103",
      "http://brauer.maths.qmul.ac.uk/Atlas/spor/M22/mag/M22G1-P1.M",
      "[todd-coxeter][extreme]") {
    ReportGuard               rg(true);
    Presentation<std::string> p;
    p.alphabet("xyXY");
    p.contains_empty_word(true);

    presentation::add_inverse_rules(p, "XYxy");
    presentation::add_rule_and_check(p, "xx", "");
    presentation::add_rule_and_check(p, "yyyy", "");
    presentation::add_rule_and_check(p, "xyxyxyxyxyxyxyxyxyxyxy", "");
    presentation::add_rule_and_check(p, "xyyxyyxyyxyyxyy", "");
    presentation::add_rule_and_check(p, "XYxyXYxyXYxyXYxyXYxyXYxy", "");
    presentation::add_rule_and_check(p, "XYXYxyxyXYXYxyxyXYXYxyxy", "");
    presentation::add_rule_and_check(p, "xyxyxYxyxyxYxyxyxYxyxyxYxyxyxY", "");

    ToddCoxeter tc(twosided, p);
    section_hlt(tc);
    section_felsch(tc);  // 2s with Felsch
    REQUIRE(tc.number_of_classes() == 443'520);
  }

  // Takes about 4 minutes (2021 - MacBook Air M1 - 8GB RAM)
  // with Felsch (3.5mins or 2.5mins with lowerbound) or HLT (4.5mins)
  LIBSEMIGROUPS_TEST_CASE(
      "v3::ToddCoxeter",
      "104",
      "http://brauer.maths.qmul.ac.uk/Atlas/spor/M23/mag/M23G1-P1.M",
      "[todd-coxeter][extreme]") {
    Presentation<std::string> p;
    p.alphabet("xyXY");
    p.contains_empty_word(true);

    presentation::add_inverse_rules(p, "XYxy");
    presentation::add_rule_and_check(p, "xx", "");
    presentation::add_rule_and_check(p, "yyyy", "");
    presentation::add_rule_and_check(
        p, "xyxyxyxyxyxyxyxyxyxyxyxyxyxyxyxyxyxyxyxyxyxyxy", "");
    presentation::add_rule_and_check(p, "xyyxyyxyyxyyxyyxyy", "");
    presentation::add_rule_and_check(p, "XYxyXYxyXYxyXYxyXYxyXYxy", "");
    presentation::add_rule_and_check(p, "xyxYxyyxyxYxyyxyxYxyyxyxYxyy", "");
    presentation::add_rule_and_check(p, "xyxyxyxYxyyxyxYxyxYxyxyxyxYxYxY", "");
    presentation::add_rule_and_check(
        p, "xyxyyxyyxyxyyxyyxyxyyxyyxyxyyxyyxyxyyxyyxyxyyxyy", "");
    presentation::add_rule_and_check(
        p, "xyxyyxyxyyxyxyyxyyxYxyyxYxyxyyxyxYxyy", "");

    REQUIRE(presentation::length(p) == 246);
    presentation::greedy_reduce_length(p);
    REQUIRE(presentation::length(p) == 102);

    ToddCoxeter tc(twosided, p);

    // TODO uncomment
    // using digraph_type = typename
    // ::libsemigroups::ToddCoxeter::digraph_type; tc.sort_generating_pairs()
    //     .strategy(options::strategy::felsch)
    //     .use_relations_in_extra(true)
    //     .lower_bound(10'200'960)
    //     .deduction_version(digraph_type::process_defs::v2)
    //     .deduction_policy(DefinitionOptions::definitions::no_stack_if_no_space)
    //     .reserve(50'000'000);
    // std::cout << tc.settings_string();
    // TODO this is currently slow, probably because we stack too many
    // definitions

    section_felsch(tc);

    REQUIRE(tc.number_of_classes() == 10'200'960);
  }

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

    REQUIRE(presentation::length(p) == 239);
    presentation::greedy_reduce_length(p);
    REQUIRE(presentation::length(p) == 92);

    ToddCoxeter tc(right, p);
    tc.add_pair(make<word_type>(p, "xy"), make<word_type>(p, ""));
    section_felsch(tc);

    REQUIRE(tc.number_of_classes() == 10'644'480);
  }

  // Approx. 32 minutes (2021 - MacBook Air M1 - 8GB RAM)
  // TODO Doesn't run in v3 at present
  LIBSEMIGROUPS_TEST_CASE(
      "v3::ToddCoxeter",
      "106",
      "http://brauer.maths.qmul.ac.uk/Atlas/spor/HS/mag/HSG1-P1.M",
      "[todd-coxeter][extreme]") {
    auto rg = ReportGuard();

    Presentation<std::string> p;
    p.alphabet("xyXY");
    p.contains_empty_word(true);

    presentation::add_inverse_rules(p, "XYxy");
    presentation::add_rule_and_check(p, "xx", "");
    presentation::add_rule_and_check(p, "yyyyy", "");
    presentation::add_rule_and_check(p, "xyxyxyxyxyxyxyxyxyxyxy", "");
    presentation::add_rule_and_check(p, "xyyxyyxyyxyyxyyxyyxyyxyyxyyxyy", "");
    presentation::add_rule_and_check(p, "XYxyXYxyXYxyXYxyXYxy", "");
    presentation::add_rule_and_check(p, "XYXYxyxyXYXYxyxyXYXYxyxy", "");
    presentation::add_rule_and_check(
        p, "XYYxyyXYYxyyXYYxyyXYYxyyXYYxyyXYYxyy", "");
    presentation::add_rule_and_check(
        p, "xyxyxyyxYxYYxYxyyxyxyxYYxYYxYYxYY", "");
    presentation::add_rule_and_check(
        p, "xyxyyxYYxYYxyyxYYxYYxyyxyxyyxYxyyxYxyy", "");
    presentation::add_rule_and_check(
        p, "xyxyxyyxyyxyxYxYxyxyyxyyxyxyxYYxYxYY", "");
    presentation::add_rule_and_check(
        p, "xyxyxyyxYxYYxyxyxYxyxyxyyxYxYYxyxyxY", "");
    presentation::add_rule_and_check(
        p, "xyxyxyyxyxyxyyxyxyxYxyxyxyyxyyxyyxyxyxY", "");
    presentation::add_rule_and_check(
        p, "xyxyxyyxyxyyxyxyyxyxyxyyxYxyxYYxyxYxyy", "");
    presentation::sort_rules(p);

    REQUIRE(presentation::length(p) == 367);
    presentation::greedy_reduce_length(p);
    REQUIRE(presentation::length(p) == 135);

    ToddCoxeter tc(right, p);
    tc.add_pair(make<word_type>(p, "xy"), make<word_type>(p, ""));
    // section_felsch(tc);
    SECTION("custom HLT") {
      tc.lookahead_style(options::lookahead_style::hlt)
          .lookahead_extent(options::lookahead_extent::full)
          .strategy(options::strategy::hlt);
      // TODO uncomment
      //     .use_relations_in_extra(true)
    }
    REQUIRE(tc.number_of_classes() == 4'032'000);
  }

  LIBSEMIGROUPS_TEST_CASE(
      "v3::ToddCoxeter",
      "107",
      "http://brauer.maths.qmul.ac.uk/Atlas/spor/J1/mag/J1G1-P1.M",
      "[todd-coxeter][standard]") {
    Presentation<std::string> p;
    p.alphabet("xyXY");
    p.contains_empty_word(true);
    presentation::add_inverse_rules(p, "XYxy");
    presentation::add_rule_and_check(p, "xx", "");
    presentation::add_rule_and_check(p, "yyy", "");
    presentation::add_rule_and_check(p, "xyxyxyxyxyxyxy", "");
    presentation::add_rule_and_check(
        p,
        "xyxyxYxyxYxyxYxyxyxYxyxYxyxYxyxyxYxyxYxyxYxyxyxYxyxYxyxYxyxy"
        "xYxyxYxyxY",
        "");
    presentation::add_rule_and_check(
        p,
        "xyxyxYxyxYxyxYxyxYxyxYxyxYxyxyxYxYxyxyxYxyxYxyxYxyxYxyxYxyxY"
        "xyxyxYxY",
        "");

    // Greedy reducing the presentation here makes this slower
    ToddCoxeter tc(twosided, p);
    section_felsch(tc);
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

    section_felsch(tc);
    section_hlt(tc);
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
    // std::cout << tc.stats_string();
  }
}  // namespace libsemigroups
