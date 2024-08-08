// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2020-2023 James D. Mitchell
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

// This file is the fifth of six that contains tests for the KnuthBendix
// classes. In a mostly vain attempt to speed up compilation the tests are
// split across 6 files as follows:
//
// 1: contains quick tests for KnuthBendix created from rules and all commented
//    out tests.
//
// 2: contains more quick tests for KnuthBendix created from rules
//
// 3: contains yet more quick tests for KnuthBendix created from rules
//
// 4: contains standard and extreme test for KnuthBendix created from rules
//
// 5: contains tests for KnuthBendix created from FroidurePin instances
//
// 6: contains tests for KnuthBendix.

#define CATCH_CONFIG_ENABLE_ALL_STRINGMAKERS

#include <iostream>

#include <algorithm>      // for copy, fill
#include <string>         // for basic_string
#include <unordered_map>  // for operator!=, operator==
#include <vector>         // for vector, operator==

#include "catch.hpp"      // for AssertionHandler, ope...
#include "test-main.hpp"  // for TEMPLATE_TEST_CASE

#include "libsemigroups/constants.hpp"        // for operator!=, operator==
#include "libsemigroups/exception.hpp"        // for LibsemigroupsException
#include "libsemigroups/froidure-pin.hpp"     // for FroidurePin
#include "libsemigroups/knuth-bendix.hpp"     // for KnuthBendix
#include "libsemigroups/presentation.hpp"     // for Presentation
#include "libsemigroups/to-froidure-pin.hpp"  // for to_froidure_pin
#include "libsemigroups/to-presentation.hpp"  // for to_presentation
#include "libsemigroups/transf.hpp"           // for Transf
#include "libsemigroups/types.hpp"            // for word_type, letter_type
#include "libsemigroups/words.hpp"            // for operator""_w

#include "libsemigroups/detail/kbe.hpp"     // for KBE
#include "libsemigroups/detail/report.hpp"  // for ReportGuard
#include "libsemigroups/detail/string.hpp"  // for operator""_w

namespace libsemigroups {
  congruence_kind constexpr twosided = congruence_kind::twosided;
  congruence_kind constexpr right    = congruence_kind::right;
  congruence_kind constexpr left     = congruence_kind::left;

  using literals::operator""_w;
  using namespace rx;

#define KNUTH_BENDIX_TYPES \
  KnuthBendix<detail::RewriteTrie>, KnuthBendix<detail::RewriteFromLeft>

  namespace {
    using rule_type = KnuthBendix<>::rule_type;

    struct weird_cmp {
      bool operator()(rule_type const& x, rule_type const& y) const noexcept {
        return shortlex_compare(x.first, y.first)
               || (x.first == y.first && shortlex_compare(x.second, y.second));
      }
    };
  }  // namespace

  TEMPLATE_TEST_CASE("transformation semigroup (size 4)",
                     "[120][quick][knuth-bendix]",
                     KNUTH_BENDIX_TYPES) {
    auto                  rg = ReportGuard(false);
    FroidurePin<Transf<>> S({Transf<>({1, 0}), Transf<>({0, 0})});
    REQUIRE(S.size() == 4);
    REQUIRE(S.number_of_rules() == 4);

    auto p = to_presentation<word_type>(S);

    TestType kb(twosided, p);
    // kb.process_pending_rules();
    REQUIRE(kb.confluent());
    REQUIRE(kb.presentation().rules.size() / 2 == 4);
    REQUIRE(kb.number_of_active_rules() == 4);
    REQUIRE(kb.number_of_classes() == 4);
  }

  TEMPLATE_TEST_CASE("transformation semigroup (size 9)",
                     "[121][quick][knuth-bendix]",
                     KNUTH_BENDIX_TYPES) {
    auto                  rg = ReportGuard(false);
    FroidurePin<Transf<>> S;
    S.add_generator(Transf<>({1, 3, 4, 2, 3}));
    S.add_generator(Transf<>({0, 0, 0, 0, 0}));

    REQUIRE(S.size() == 9);
    REQUIRE(S.degree() == 5);
    REQUIRE(S.number_of_rules() == 3);

    auto     p = to_presentation<word_type>(S);
    TestType kb(twosided, p);
    // kb.process_pending_rules();
    REQUIRE(kb.confluent());
    REQUIRE(kb.number_of_active_rules() == 3);
    REQUIRE(kb.number_of_classes() == 9);
  }

  TEMPLATE_TEST_CASE("transformation semigroup (size 88)",
                     "[122][quick][knuth-bendix]",
                     KNUTH_BENDIX_TYPES) {
    auto                  rg = ReportGuard(false);
    FroidurePin<Transf<>> S;
    S.add_generator(Transf<>({1, 3, 4, 2, 3}));
    S.add_generator(Transf<>({3, 2, 1, 3, 3}));

    REQUIRE(S.size() == 88);
    REQUIRE(S.degree() == 5);
    REQUIRE(S.number_of_rules() == 18);

    auto     p = to_presentation<word_type>(S);
    TestType kb(twosided, p);
    // kb.process_pending_rules();
    REQUIRE(kb.confluent());
    REQUIRE(kb.number_of_active_rules() == 18);
    REQUIRE(kb.number_of_classes() == 88);
  }

  TEMPLATE_TEST_CASE("internal_string_to_word",
                     "[123][quick]",
                     KNUTH_BENDIX_TYPES) {
    auto                  rg = ReportGuard(false);
    FroidurePin<Transf<>> S;
    S.add_generator(Transf<>({1, 0}));
    S.add_generator(Transf<>({0, 0}));

    auto p = to_presentation<word_type>(S);

    TestType kb(twosided, p);
    REQUIRE(kb.confluent());
    auto t = to_froidure_pin(kb);
    REQUIRE(t.generator(0).word(kb) == 0_w);
  }

  TEMPLATE_TEST_CASE("internal_string_to_word x 2",
                     "[124][quick]",
                     KNUTH_BENDIX_TYPES) {
    auto                  rg = ReportGuard(false);
    FroidurePin<Transf<>> S(
        {Transf<>({1, 3, 4, 2, 3}), Transf<>({3, 2, 1, 3, 3})});

    REQUIRE(S.size() == 88);

    auto p = to_presentation<word_type>(S);

    TestType kb(twosided, p);
    kb.run();
    REQUIRE(kb.confluent());
    REQUIRE(kb.number_of_classes() == 88);
  }

  TEMPLATE_TEST_CASE("manual right congruence",
                     "[125][quick][knuth-bendix]",
                     KNUTH_BENDIX_TYPES) {
    using words::operator+;

    auto rg = ReportGuard(false);
    auto S  = FroidurePin<Transf<>>(
        {Transf<>({1, 3, 4, 2, 3}), Transf<>({3, 2, 1, 3, 3})});

    REQUIRE(S.size() == 88);
    REQUIRE(S.number_of_rules() == 18);

    auto p = to_presentation<word_type>(S);

    TestType kb1(twosided, p);
    REQUIRE(kb1.number_of_classes() == 88);

    presentation::add_rule_no_checks(
        p,
        2_w + S.factorisation(Transf<>({3, 4, 4, 4, 4})),
        2_w + S.factorisation(Transf<>({3, 1, 3, 3, 3})));

    p.alphabet(3);

    TestType    kb2(twosided, p);
    auto const& q = kb2.presentation();

    auto words = (S.normal_forms()
                  | rx::transform([](word_type const& w) { return 2_w + w; })
                  | ToStrings(q.alphabet()));
    REQUIRE((words | rx::count()) == 88);
    REQUIRE((words | rx::take(4) | rx::to_vector())
            == std::vector<std::string>({"ca", "cb", "caa", "cab"}));

    kb2.run();
    auto pp = partition(kb2, words);

    REQUIRE(pp.size() == 72);

    REQUIRE(kb2.gilman_graph().number_of_nodes() == 62);

    auto copy   = kb2.gilman_graph();
    auto source = copy.target(0, 2);
    copy.remove_label_no_checks(2);
    REQUIRE(copy.out_degree() == 2);
    REQUIRE(copy.number_of_nodes() == 62);
    REQUIRE(word_graph::is_acyclic(copy, source));

    Paths paths(copy);
    REQUIRE(paths.min(1).source(source).count() == 72);

    REQUIRE(!kb2.contains(2_w + S.factorisation(Transf<>({1, 3, 1, 3, 3})),
                          2_w + S.factorisation(Transf<>({4, 2, 4, 4, 2}))));

    REQUIRE(!kb2.contains(2_w + S.factorisation(Transf<>({1, 3, 3, 3, 3})),
                          2_w + S.factorisation(Transf<>({4, 2, 4, 4, 2}))));

    REQUIRE(kb2.contains(2_w + S.factorisation(Transf<>({2, 4, 2, 2, 2})),
                         2_w + S.factorisation(Transf<>({2, 3, 3, 3, 3}))));

    REQUIRE(!kb2.contains(2_w + S.factorisation(Transf<>({1, 3, 3, 3, 3})),
                          2_w + S.factorisation(Transf<>({2, 3, 3, 3, 3}))));

    auto ntc = (iterator_range(pp.begin(), pp.end())
                | filter([](auto const& val) { return val.size() > 1; })
                | transform([](auto& val) {
                    std::for_each(
                        val.begin(), val.end(), [](auto& w) -> auto& {
                          w.erase(w.begin());
                          return w;
                        });
                    return val;
                  }));

    REQUIRE((ntc | count()) == 4);
    REQUIRE(
        (ntc | to_vector())
        == std::vector<std::vector<std::string>>(
            {{"baaab",
              "baaabb",
              "aabaaab",
              "abaaaba",
              "abaaabab",
              "baaabaaa",
              "abaaabbaa"},
             {"abaaab", "baaabab", "aabaaabab"},
             {"baaaba", "abaaabb", "baaabba", "aabaaaba", "abaaabaa"},
             {"baaabaa", "abaaabba", "baaabbaa", "aabaaabaa", "abaaabaaa"}}));
  }

  TEMPLATE_TEST_CASE("right congruence!!!",
                     "[126][quick][knuth-bendix]",
                     KNUTH_BENDIX_TYPES) {
    using words::operator+;

    auto rg = ReportGuard(false);
    auto S  = FroidurePin<Transf<>>(
        {Transf<>({1, 3, 4, 2, 3}), Transf<>({3, 2, 1, 3, 3})});

    REQUIRE(S.size() == 88);
    REQUIRE(S.number_of_rules() == 18);

    auto p = to_presentation<word_type>(S);

    TestType kb(right, p);
    REQUIRE(kb.presentation().rules
            == std::vector<std::string>(
                {"bbb",      "b",       "bbab",     "bab",      "aaaaa",
                 "aa",       "abaab",   "aaaab",    "baaaa",    "ba",
                 "bbaab",    "baaab",   "aababa",   "aabb",     "aababb",
                 "aaba",     "bababa",  "babb",     "bababb",   "baba",
                 "bbaaab",   "baab",    "aabbaaa",  "aabb",     "babaaab",
                 "aabaaab",  "babbaaa", "babb",     "aaabaaab", "aabaaab",
                 "aabaaabb", "aabaaab", "baabaaab", "aabaaab",  "aabaaabaaa",
                 "aabaaab"}));

    kb.add_pair(S.factorisation(Transf<>({3, 4, 4, 4, 4})),
                S.factorisation(Transf<>({3, 1, 3, 3, 3})));
    REQUIRE(knuth_bendix::normal_forms(kb).min(1).count() == 72);

    REQUIRE(kb.number_of_classes() == 72);

    REQUIRE(!kb.contains(S.factorisation(Transf<>({1, 3, 1, 3, 3})),
                         S.factorisation(Transf<>({4, 2, 4, 4, 2}))));

    REQUIRE(!kb.contains(S.factorisation(Transf<>({1, 3, 3, 3, 3})),
                         S.factorisation(Transf<>({4, 2, 4, 4, 2}))));

    REQUIRE(kb.contains(S.factorisation(Transf<>({2, 4, 2, 2, 2})),
                        S.factorisation(Transf<>({2, 3, 3, 3, 3}))));

    REQUIRE(!kb.contains(S.factorisation(Transf<>({1, 3, 3, 3, 3})),
                         S.factorisation(Transf<>({2, 3, 3, 3, 3}))));

    REQUIRE((knuth_bendix::normal_forms(kb).min(1) | count()) == 72);
    REQUIRE(!kb.presentation().contains_empty_word());
    REQUIRE((knuth_bendix::normal_forms(kb)
             | ToStrings(kb.presentation().alphabet()) | to_vector())
            == std::vector<std::string>(
                {"a",        "b",       "aa",      "ab",      "ba",
                 "bb",       "aaa",     "aab",     "aba",     "abb",
                 "baa",      "bab",     "bba",     "aaaa",    "aaab",
                 "aaba",     "aabb",    "abaa",    "abab",    "abba",
                 "baaa",     "baab",    "baba",    "babb",    "bbaa",
                 "aaaab",    "aaaba",   "aaabb",   "aabaa",   "aabab",
                 "aabba",    "abaaa",   "ababa",   "ababb",   "abbaa",
                 "baaab",    "baaba",   "baabb",   "babaa",   "babab",
                 "babba",    "bbaaa",   "aaaaba",  "aaaabb",  "aaabaa",
                 "aaabab",   "aaabba",  "aabaaa",  "aabbaa",  "abaaab",
                 "ababaa",   "ababab",  "ababba",  "abbaaa",  "baaaba",
                 "baabaa",   "baabab",  "baabba",  "babaaa",  "babbaa",
                 "aaaabaa",  "aaaabab", "aaaabba", "aaabaaa", "aaabbaa",
                 "ababaaa",  "ababbaa", "baaabaa", "baabaaa", "baabbaa",
                 "aaaabaaa", "aaaabbaa"}));

    REQUIRE(kb.rewrite("baaabb") == "baaab");

    auto nf = (S.normal_forms() | ToStrings(kb.presentation().alphabet()));
    REQUIRE((nf | count()) == 88);
    auto pp = partition(kb, nf);
    REQUIRE(pp.size() == 72);

    auto ntc = (iterator_range(pp)
                | filter([](auto const& val) { return val.size() > 1; }));

    REQUIRE((ntc | count()) == 4);
    REQUIRE(
        (ntc | to_vector())
        == std::vector<std::vector<std::string>>(
            {{"baaab",
              "baaabb",
              "aabaaab",
              "abaaaba",
              "abaaabab",
              "baaabaaa",
              "abaaabbaa"},
             {"abaaab", "baaabab", "aabaaabab"},
             {"baaaba", "abaaabb", "baaabba", "aabaaaba", "abaaabaa"},
             {"baaabaa", "abaaabba", "baaabbaa", "aabaaabaa", "abaaabaaa"}}));
  }

  TEMPLATE_TEST_CASE("manual left congruence!!!",
                     "[127][quick][knuth-bendix]",
                     KNUTH_BENDIX_TYPES) {
    using words::operator+;

    auto rg = ReportGuard(false);
    auto S  = FroidurePin<Transf<>>(
        {Transf<>({1, 3, 4, 2, 3}), Transf<>({3, 2, 1, 3, 3})});

    REQUIRE(S.size() == 88);
    REQUIRE(S.number_of_rules() == 18);

    auto p = to_presentation<word_type>(S);
    REQUIRE(!p.contains_empty_word());

    presentation::reverse(p);
    REQUIRE(!p.contains_empty_word());
    p.alphabet(3);
    TestType kb(twosided, p);
    REQUIRE(
        to_string(kb.presentation(), S.factorisation(Transf<>({3, 4, 4, 4, 4})))
        == "abaaabbaa");
    REQUIRE(
        to_string(kb.presentation(), S.factorisation(Transf<>({3, 1, 3, 3, 3})))
        == "baaab");

    kb.add_pair(to_word(kb.presentation(), "caabbaaaba"),
                to_word(kb.presentation(), "cbaaab"));

    kb.run();
    REQUIRE(kb.number_of_active_rules() == 23);

    auto copy   = kb.gilman_graph();
    auto source = copy.target(0, 2);
    copy.remove_label_no_checks(2);
    REQUIRE(source == 34);
    REQUIRE(copy.out_degree() == 2);
    REQUIRE(copy.number_of_nodes() == 51);
    REQUIRE(word_graph::is_acyclic(copy, source));

    Paths paths1(copy);
    REQUIRE(paths1.min(1).source(source).count() == 69);

    auto nrset = word_graph::nodes_reachable_from(copy, source);
    auto nrvec = std::vector<size_t>(nrset.begin(), nrset.end());
    std::sort(nrvec.begin(), nrvec.end());
    source = std::distance(nrvec.begin(),
                           std::find(nrvec.begin(), nrvec.end(), source));
    REQUIRE(source == 28);

    copy.induced_subgraph_no_checks(nrvec.begin(), nrvec.end());
    REQUIRE(copy.out_degree() == 2);
    REQUIRE(copy.number_of_nodes() == 45);

    Paths paths(copy);
    REQUIRE(paths.min(1).source(source).count() == 69);

    REQUIRE(kb.gilman_graph().number_of_nodes() == 51);
  }

  TEMPLATE_TEST_CASE("automatic left congruence!!!",
                     "[128][quick][knuth-bendix]",
                     KNUTH_BENDIX_TYPES) {
    using words::operator+;

    auto rg = ReportGuard(false);
    auto S  = FroidurePin<Transf<>>(
        {Transf<>({1, 3, 4, 2, 3}), Transf<>({3, 2, 1, 3, 3})});

    REQUIRE(S.size() == 88);
    REQUIRE(S.number_of_rules() == 18);

    auto p = to_presentation<word_type>(S);
    REQUIRE(!p.contains_empty_word());

    TestType kb(left, p);

    kb.add_pair(S.factorisation(Transf<>({3, 4, 4, 4, 4})),
                S.factorisation(Transf<>({3, 1, 3, 3, 3})));

    kb.run();

    auto copy = kb.gilman_graph();
    REQUIRE(copy.out_degree() == 2);
    REQUIRE(copy.number_of_nodes() == 45);
    REQUIRE(word_graph::is_acyclic(copy, 0));

    Paths paths1(copy);
    REQUIRE(paths1.min(1).source(0).count() == 69);
    REQUIRE(knuth_bendix::normal_forms(kb).min(1).count() == 69);
    REQUIRE(!kb.equal_to("aaaabb", "abaaabaa"));

    REQUIRE(kb.number_of_classes() == 69);

    auto nf1 = (knuth_bendix::normal_forms(kb).min(1)
                | ToStrings(kb.presentation().alphabet()) | to_vector());

    REQUIRE(nf1
            == std::vector<std::string>(
                {"a",        "b",        "aa",       "ba",       "ab",
                 "bb",       "aaa",      "baa",      "aba",      "bba",
                 "aab",      "abb",      "aaaa",     "baaa",     "abaa",
                 "bbaa",     "aaba",     "baba",     "abba",     "aabb",
                 "babb",     "abaaa",    "bbaaa",    "aabaa",    "babaa",
                 "abbaa",    "aaaba",    "baaba",    "ababa",    "aabba",
                 "babba",    "aaabb",    "baabb",    "ababb",    "aabaaa",
                 "babaaa",   "abbaaa",   "aaabaa",   "baabaa",   "ababaa",
                 "aabbaa",   "babbaa",   "aaaaba",   "baaaba",   "aaabba",
                 "baabba",   "ababba",   "aaaabb",   "baaabb",   "aaabaaa",
                 "baabaaa",  "ababaaa",  "aaaabaa",  "baaabaa",  "aaabbaa",
                 "baabbaa",  "ababbaa",  "abaaaba",  "aaaabba",  "baaabba",
                 "abaaabb",  "aaaabaaa", "baaabaaa", "abaaabaa", "aaaabbaa",
                 "baaabbaa", "aabaaaba", "abaaabba", "abaaabaaa"}));
    REQUIRE(std::all_of(nf1.begin(), nf1.end(), [&kb](auto& w) {
      return kb.normal_form(w) == w;
    }));

    auto nf = S.normal_forms() | ToStrings(kb.presentation().alphabet())
              | take(S.size());
    auto result = kb.gilman_graph();
    // auto expected = to_word_graph<size_t>(
    //     45, {{1, 2},   {31, 9}, {43, 11}, {4, 5},  {},       {20, 21}, {7,
    //     8},
    //          {4, 18},  {},      {10, 11}, {6, 12}, {14},     {13},     {},
    //          {15, 16}, {24, 8}, {17},     {},      {19},     {23},     {27,
    //          12}, {22},     {},      {},       {4, 25}, {26},     {}, {28,
    //          8}, {4, 29},  {30},    {},       {3, 32}, {33, 34}, {39, 12},
    //          {35}, {36, 16}, {37, 8}, {4, 38},  {},      {40, 8},  {4, 41},
    //          {42},
    //          {},       {44},    {}});
    // REQUIRE(result == expected);

    REQUIRE(kb.normal_form("abaaaa") == "aba");

    REQUIRE((nf | count()) == 88);
    auto pp = partition(kb, nf);
    REQUIRE(pp.size() == 69);

    auto ntc = (iterator_range(pp)
                | filter([](auto const& val) { return val.size() > 1; })
                | to_vector());

    REQUIRE(ntc.size() == 1);
    REQUIRE(
        ntc
        == std::vector<std::vector<std::string>>(
            {{"aab",     "bab",      "aaab",      "abab",      "baab",
              "aaaab",   "aabab",    "baaab",     "babab",     "aaabab",
              "abaaab",  "ababab",   "baabab",    "aaaabab",   "aabaaab",
              "baaabab", "abaaabab", "aabaaabaa", "aabaaabab", "abaaabbaa"}}));

    REQUIRE(std::all_of(ntc[0].begin(), ntc[0].end(), [&kb, &ntc](auto& w) {
      return kb.normal_form(w) == ntc[0][0];
    }));

    REQUIRE((kb.active_rules() | count()) == 23);

    REQUIRE((kb.active_rules() | sort(weird_cmp()) | to_vector())
            == std::vector<std::pair<std::string, std::string>>(
                {{"bbb", "b"},
                 {"babc", "aabc"},
                 {"bbab", "bab"},
                 {"aaaaa", "aa"},
                 {"aaabc", "aabc"},
                 {"abaab", "aaaab"},
                 {"baaaa", "ba"},
                 {"baabc", "aabc"},
                 {"bbaab", "baaab"},
                 {"aababa", "aabb"},
                 {"aababb", "aaba"},
                 {"bababa", "babb"},
                 {"bababb", "baba"},
                 {"bbaaab", "baab"},
                 {"aabbaaa", "aabb"},
                 {"babaaab", "aabaaab"},
                 {"babbaaa", "babb"},
                 {"aaabaaab", "aabaaab"},
                 {"aabaaabb", "aabaaab"},
                 {"baabaaab", "aabaaab"},
                 {"aabaaabaaa", "aabaaab"},
                 {"aabaaabaac", "aabc"},
                 {"abaaabbaac", "aabc"}}));
    REQUIRE(knuth_bendix::is_reduced(kb));

    kb.init(right, p);
    kb.add_pair(S.factorisation(Transf<>({3, 4, 4, 4, 4})),
                S.factorisation(Transf<>({3, 1, 3, 3, 3})));
    REQUIRE(kb.number_of_classes() == 72);
    REQUIRE(kb.contains({1, 1, 1}, {1}));
    REQUIRE_THROWS_AS(kb.presentation(p), LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE(
      "KnuthBendix",
      "028",
      "left congruence on finite semigroup (RewriteFromLeft)",
      "[quick]") {
    auto                  rg = ReportGuard(false);
    FroidurePin<Transf<>> S;
    S.add_generator(Transf<>({1, 3, 4, 2, 3}));
    S.add_generator(Transf<>({3, 2, 1, 3, 3}));
    REQUIRE(S.size() == 88);

    auto l = 010001100_w;
    auto r = 10001_w;

    KnuthBendix kb(left, to_presentation<word_type>(S));
    kb.add_pair(l, r);
    REQUIRE(kb.number_of_classes() == 69);
    REQUIRE(kb.rewrite("baabab") == "aab");
    REQUIRE(kb.rewrite("aabaaab") == "aab");
    REQUIRE(kb.equal_to("baabab", "aabaaab"));
  }
}  // namespace libsemigroups
