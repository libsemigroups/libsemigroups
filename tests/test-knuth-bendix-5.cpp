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

#include "catch_amalgamated.hpp"  // for AssertionHandler, ope...
#include "test-main.hpp"          // for TEMPLATE_TEST_CASE

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
  congruence_kind constexpr onesided = congruence_kind::onesided;

  using literals::operator""_w;
  using namespace rx;

  using knuth_bendix::add_generating_pair;
  using knuth_bendix::contains;
  using knuth_bendix::non_trivial_classes;
  using knuth_bendix::normal_forms;
  using knuth_bendix::partition;
  using knuth_bendix::reduce;
  using knuth_bendix::reduce_no_run;
  using knuth_bendix::redundant_rule;

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
    auto rg = ReportGuard(false);
    auto S  = to_froidure_pin({Transf<>({1, 0}), Transf<>({0, 0})});
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
    auto rg = ReportGuard(false);
    auto S  = to_froidure_pin(
        {Transf<>({1, 3, 4, 2, 3}), Transf<>({3, 2, 1, 3, 3})});

    REQUIRE(S.size() == 88);

    auto p = to_presentation<word_type>(S);

    TestType kb(twosided, p);
    kb.run();
    REQUIRE(kb.confluent());
    REQUIRE(kb.number_of_classes() == 88);
  }

  TEMPLATE_TEST_CASE("manual onesided congruence",
                     "[125][quick][knuth-bendix]",
                     KNUTH_BENDIX_TYPES) {
    using words::operator+;

    auto rg = ReportGuard(false);
    auto S  = to_froidure_pin(
        {Transf<>({1, 3, 4, 2, 3}), Transf<>({3, 2, 1, 3, 3})});

    REQUIRE(S.size() == 88);
    REQUIRE(S.number_of_rules() == 18);

    auto p = to_presentation<word_type>(S);

    TestType kb1(twosided, p);
    REQUIRE(kb1.number_of_classes() == 88);

    presentation::add_rule_no_checks(
        p,
        2_w + froidure_pin::factorisation(S, Transf<>({3, 4, 4, 4, 4})),
        2_w + froidure_pin::factorisation(S, Transf<>({3, 1, 3, 3, 3})));

    p.alphabet(3);

    TestType kb2(twosided, p);

    auto words = (froidure_pin::normal_forms(S)
                  | rx::transform([](word_type const& w) { return 2_w + w; }));
    REQUIRE((words | rx::count()) == 88);
    REQUIRE((words | ToString("abc") | rx::take(4) | rx::to_vector())
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

    REQUIRE(!contains(
        kb2,
        2_w + froidure_pin::factorisation(S, Transf<>({1, 3, 1, 3, 3})),
        2_w + froidure_pin::factorisation(S, Transf<>({4, 2, 4, 4, 2}))));

    REQUIRE(!contains(
        kb2,
        2_w + froidure_pin::factorisation(S, Transf<>({1, 3, 3, 3, 3})),
        2_w + froidure_pin::factorisation(S, Transf<>({4, 2, 4, 4, 2}))));

    REQUIRE(contains(
        kb2,
        2_w + froidure_pin::factorisation(S, Transf<>({2, 4, 2, 2, 2})),
        2_w + froidure_pin::factorisation(S, Transf<>({2, 3, 3, 3, 3}))));

    REQUIRE(!contains(
        kb2,
        2_w + froidure_pin::factorisation(S, Transf<>({1, 3, 3, 3, 3})),
        2_w + froidure_pin::factorisation(S, Transf<>({2, 3, 3, 3, 3}))));

    auto ntc = (iterator_range(pp.begin(), pp.end())
                | filter([](auto const& val) { return val.size() > 1; })
                | transform([](auto& val) {
                    std::for_each(val.begin(), val.end(), [](auto& w) -> auto& {
                      w.erase(w.begin());
                      return w;
                    });
                    return val;
                  }));

    REQUIRE((ntc | count()) == 4);
    REQUIRE(
        (ntc | to_vector())
        == std::vector<std::vector<word_type>>(
            {{10001_w,
              100011_w,
              0010001_w,
              0100010_w,
              01000101_w,
              10001000_w,
              010001100_w},
             {010001_w, 1000101_w, 001000101_w},
             {100010_w, 0100011_w, 1000110_w, 00100010_w, 01000100_w},
             {1000100_w, 01000110_w, 10001100_w, 001000100_w, 010001000_w}}));
  }

  TEMPLATE_TEST_CASE("onesided congruence!!!",
                     "[126][quick][knuth-bendix]",
                     KNUTH_BENDIX_TYPES) {
    using words::operator+;

    auto rg = ReportGuard(false);
    auto S  = to_froidure_pin(
        {Transf<>({1, 3, 4, 2, 3}), Transf<>({3, 2, 1, 3, 3})});

    REQUIRE(S.size() == 88);
    REQUIRE(S.number_of_rules() == 18);

    auto p = to_presentation<word_type>(S);

    TestType     kb(onesided, p);
    Presentation q(kb.presentation());
    presentation::change_alphabet(q, "ab");

    REQUIRE(q.rules
            == std::vector<std::string>(
                {"bbb",      "b",       "bbab",     "bab",      "aaaaa",
                 "aa",       "abaab",   "aaaab",    "baaaa",    "ba",
                 "bbaab",    "baaab",   "aababa",   "aabb",     "aababb",
                 "aaba",     "bababa",  "babb",     "bababb",   "baba",
                 "bbaaab",   "baab",    "aabbaaa",  "aabb",     "babaaab",
                 "aabaaab",  "babbaaa", "babb",     "aaabaaab", "aabaaab",
                 "aabaaabb", "aabaaab", "baabaaab", "aabaaab",  "aabaaabaaa",
                 "aabaaab"}));

    add_generating_pair(
        kb,
        froidure_pin::factorisation(S, Transf<>({3, 4, 4, 4, 4})),
        froidure_pin::factorisation(S, Transf<>({3, 1, 3, 3, 3})));

    REQUIRE(kb.generating_pairs()
            == std::vector<word_type>({010001100_w, 10001_w}));
    REQUIRE(normal_forms(kb).min(1).count() == 72);

    REQUIRE(kb.number_of_classes() == 72);

    REQUIRE(
        !contains(kb,
                  froidure_pin::factorisation(S, Transf<>({1, 3, 1, 3, 3})),
                  froidure_pin::factorisation(S, Transf<>({4, 2, 4, 4, 2}))));

    REQUIRE(
        !contains(kb,
                  froidure_pin::factorisation(S, Transf<>({1, 3, 3, 3, 3})),
                  froidure_pin::factorisation(S, Transf<>({4, 2, 4, 4, 2}))));

    REQUIRE(
        contains(kb,
                 froidure_pin::factorisation(S, Transf<>({2, 4, 2, 2, 2})),
                 froidure_pin::factorisation(S, Transf<>({2, 3, 3, 3, 3}))));

    REQUIRE(
        !contains(kb,
                  froidure_pin::factorisation(S, Transf<>({1, 3, 3, 3, 3})),
                  froidure_pin::factorisation(S, Transf<>({2, 3, 3, 3, 3}))));

    REQUIRE((normal_forms(kb).min(1) | count()) == 72);
    REQUIRE(!kb.presentation().contains_empty_word());
    REQUIRE((normal_forms<word_type>(kb) | to_vector())
            == std::vector<word_type>(
                {0_w,        1_w,       00_w,      01_w,      10_w,
                 11_w,       000_w,     001_w,     010_w,     011_w,
                 100_w,      101_w,     110_w,     0000_w,    0001_w,
                 0010_w,     0011_w,    0100_w,    0101_w,    0110_w,
                 1000_w,     1001_w,    1010_w,    1011_w,    1100_w,
                 00001_w,    00010_w,   00011_w,   00100_w,   00101_w,
                 00110_w,    01000_w,   01010_w,   01011_w,   01100_w,
                 10001_w,    10010_w,   10011_w,   10100_w,   10101_w,
                 10110_w,    11000_w,   000010_w,  000011_w,  000100_w,
                 000101_w,   000110_w,  001000_w,  001100_w,  010001_w,
                 010100_w,   010101_w,  010110_w,  011000_w,  100010_w,
                 100100_w,   100101_w,  100110_w,  101000_w,  101100_w,
                 0000100_w,  0000101_w, 0000110_w, 0001000_w, 0001100_w,
                 0101000_w,  0101100_w, 1000100_w, 1001000_w, 1001100_w,
                 00001000_w, 00001100_w}));

    REQUIRE(reduce_no_run(kb, 100011_w) == 10001_w);

    auto nf = froidure_pin::normal_forms(S);
    REQUIRE((nf | count()) == 88);
    auto pp = partition(kb, nf);
    REQUIRE(pp.size() == 72);

    auto ntc = (iterator_range(pp)
                | filter([](auto const& val) { return val.size() > 1; }));

    REQUIRE((ntc | count()) == 4);
    REQUIRE(
        (ntc | to_vector())
        == std::vector<std::vector<word_type>>(
            {{10001_w,
              100011_w,
              0010001_w,
              0100010_w,
              01000101_w,
              10001000_w,
              010001100_w},
             {010001_w, 1000101_w, 001000101_w},
             {100010_w, 0100011_w, 1000110_w, 00100010_w, 01000100_w},
             {1000100_w, 01000110_w, 10001100_w, 001000100_w, 010001000_w}}));
  }

  TEMPLATE_TEST_CASE("manual left congruence!!!",
                     "[127][quick][knuth-bendix]",
                     KNUTH_BENDIX_TYPES) {
    using words::operator+;

    auto rg = ReportGuard(false);
    auto S  = to_froidure_pin(
        {Transf<>({1, 3, 4, 2, 3}), Transf<>({3, 2, 1, 3, 3})});

    REQUIRE(S.size() == 88);
    REQUIRE(S.number_of_rules() == 18);

    auto p = to_presentation<std::string>(S);
    REQUIRE(!p.contains_empty_word());
    REQUIRE(p.alphabet() == "ab");

    presentation::reverse(p);
    REQUIRE(!p.contains_empty_word());
    p.alphabet("abc");
    TestType kb(twosided, p);
    ToString to_string;
    ToWord   to_word;

    REQUIRE(to_string(froidure_pin::factorisation(S, Transf<>({3, 4, 4, 4, 4})))
            == "abaaabbaa");
    REQUIRE(to_string(froidure_pin::factorisation(S, Transf<>({3, 1, 3, 3, 3})))
            == "baaab");

    REQUIRE(kb.number_of_generating_pairs() == 0);
    add_generating_pair(kb, "caabbaaaba", "cbaaab");

    REQUIRE(kb.number_of_generating_pairs() == 1);
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
                     "[128][quick][knuth-bendix][no-valgrind]",
                     KNUTH_BENDIX_TYPES) {
    using words::operator+;

    auto rg = ReportGuard(false);
    auto S  = to_froidure_pin(
        {Transf<>({1, 3, 4, 2, 3}), Transf<>({3, 2, 1, 3, 3})});

    REQUIRE(S.size() == 88);
    REQUIRE(S.number_of_rules() == 18);

    auto p = to_presentation<std::string>(S);
    REQUIRE(!p.contains_empty_word());
    presentation::reverse(p);

    TestType kb(onesided, p);

    add_generating_pair(kb, "aabbaaaba", "baaab");

    kb.run();

    auto copy = kb.gilman_graph();
    REQUIRE(copy.out_degree() == 2);
    REQUIRE(copy.number_of_nodes() == 45);
    REQUIRE(word_graph::is_acyclic(copy, 0));

    Paths paths1(copy);
    REQUIRE(paths1.min(1).source(0).count() == 69);
    REQUIRE(normal_forms(kb).min(1).count() == 69);
    REQUIRE(!contains(kb, "bbaaaa", "aabaaaba"));

    REQUIRE(kb.number_of_classes() == 69);

    auto nf1 = (normal_forms(kb).min(1) | to_vector());

    REQUIRE(nf1
            == std::vector<std::string>(
                {"a",        "b",        "aa",       "ab",       "ba",
                 "bb",       "aaa",      "aab",      "aba",      "abb",
                 "baa",      "bba",      "aaaa",     "aaab",     "aaba",
                 "aabb",     "abaa",     "abab",     "abba",     "bbaa",
                 "bbab",     "aaaba",    "aaabb",    "aabaa",    "aabab",
                 "aabba",    "abaaa",    "abaab",    "ababa",    "abbaa",
                 "abbab",    "bbaaa",    "bbaab",    "bbaba",    "aaabaa",
                 "aaabab",   "aaabba",   "aabaaa",   "aabaab",   "aababa",
                 "aabbaa",   "aabbab",   "abaaaa",   "abaaab",   "abbaaa",
                 "abbaab",   "abbaba",   "bbaaaa",   "bbaaab",   "aaabaaa",
                 "aaabaab",  "aaababa",  "aabaaaa",  "aabaaab",  "aabbaaa",
                 "aabbaab",  "aabbaba",  "abaaaba",  "abbaaaa",  "abbaaab",
                 "bbaaaba",  "aaabaaaa", "aaabaaab", "aabaaaba", "aabbaaaa",
                 "aabbaaab", "abaaabaa", "abbaaaba", "aaabaaaba"}));
    REQUIRE(std::all_of(
        nf1.begin(), nf1.end(), [&kb](auto& w) { return reduce(kb, w) == w; }));

    auto to_string = ToString(kb.presentation().alphabet());
    auto nf        = froidure_pin::normal_forms(S) | to_string | take(S.size());

    REQUIRE(reduce(kb, "aaaaba") == "aba");

    REQUIRE((nf | count()) == 88);
    auto vnf = (nf | to_vector());
    for (auto& w : vnf) {
      reverse(w);
    }
    auto pp = partition(kb, iterator_range(vnf.begin(), vnf.end()));
    REQUIRE(pp.size() == 69);

    auto ntc = (iterator_range(pp)
                | filter([](auto const& val) { return val.size() > 1; })
                | to_vector());

    REQUIRE(ntc.size() == 1);
    REQUIRE(
        ntc
        == std::vector<std::vector<std::string>>(
            {{"baa",     "bab",      "baaa",      "baba",      "baab",
              "baaaa",   "babaa",    "baaab",     "babab",     "babaaa",
              "baaaba",  "bababa",   "babaab",    "babaaaa",   "baaabaa",
              "babaaab", "babaaaba", "aabaaabaa", "babaaabaa", "aabbaaaba"}}));

    REQUIRE(std::all_of(ntc[0].begin(), ntc[0].end(), [&kb, &ntc](auto& w) {
      return reduce(kb, w) == ntc[0][0];
    }));

    REQUIRE((kb.active_rules() | count()) == 23);

    REQUIRE((kb.active_rules() | sort(weird_cmp()) | to_vector())
            == std::vector<std::pair<std::string, std::string>>(
                {{"bbb", "b"},
                 {"babb", "bab"},
                 {"cbab", "cbaa"},
                 {"aaaaa", "aa"},
                 {"aaaab", "ab"},
                 {"baaba", "baaaa"},
                 {"baabb", "baaab"},
                 {"cbaaa", "cbaa"},
                 {"cbaab", "cbaa"},
                 {"ababaa", "bbaa"},
                 {"ababab", "bbab"},
                 {"baaabb", "baab"},
                 {"bbabaa", "abaa"},
                 {"bbabab", "abab"},
                 {"aaabbaa", "bbaa"},
                 {"aaabbab", "bbab"},
                 {"baaabab", "baaabaa"},
                 {"baaabaaa", "baaabaa"},
                 {"baaabaab", "baaabaa"},
                 {"bbaaabaa", "baaabaa"},
                 {"aaabaaabaa", "baaabaa"},
                 {"caabaaabaa", "cbaa"},
                 {"caabbaaaba", "cbaa"}}));
    REQUIRE(knuth_bendix::is_reduced(kb));

    presentation::reverse(p);
    kb.init(onesided, p);
    add_generating_pair(
        kb,
        to_string(froidure_pin::factorisation(S, Transf<>({3, 4, 4, 4, 4}))),
        to_string(froidure_pin::factorisation(S, Transf<>({3, 1, 3, 3, 3}))));
    REQUIRE(kb.number_of_classes() == 72);
    REQUIRE(contains(kb, "bbb", "b"));
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "028",
                          "left congruence on finite semigroup",
                          "[quick]") {
    auto                  rg = ReportGuard(false);
    FroidurePin<Transf<>> S;
    S.add_generator(Transf<>({1, 3, 4, 2, 3}));
    S.add_generator(Transf<>({3, 2, 1, 3, 3}));
    REQUIRE(S.size() == 88);

    auto l = 010001100_w;
    auto r = 10001_w;

    KnuthBendix kb(onesided,
                   presentation::reverse(to_presentation<word_type>(S)));
    ToString    to_string(kb.presentation().alphabet());
    add_generating_pair(kb, reverse(l), reverse(r));
    REQUIRE(kb.number_of_classes() == 69);
    REQUIRE(reduce_no_run(kb, 101001_w) == 100_w);
    REQUIRE(reduce_no_run(kb, 1000100_w) == 100_w);
    REQUIRE(contains(kb, 101001_w, 1000100_w));
  }
}  // namespace libsemigroups
