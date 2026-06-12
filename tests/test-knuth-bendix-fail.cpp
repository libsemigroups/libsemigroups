// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2026 James D. Mitchell
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

// This file contains tests that we couldn't get to complete, or that take too
// long to run under any testing regime.

#define CATCH_CONFIG_ENABLE_ALL_STRINGMAKERS

#include <algorithm>      // for next_permutation
#include <chrono>         // for milliseconds, seconds
#include <cmath>          // for pow
#include <cstddef>        // for size_t
#include <iostream>       // for string, operator<<, endl
#include <numeric>        // for iota
#include <string>         // for basic_string, char_traits
#include <unordered_set>  // for unordered_set
#include <utility>        // for move, operator==, pair
#include <vector>         // for vector, operator==

#include "Catch2-3.14.0/catch_amalgamated.hpp"  // for AssertionHandler, oper...
#include "test-main.hpp"  // for LIBSEMIGROUPS_TEMPLATE_TEST_CASE

#include "libsemigroups/constants.hpp"  // for operator==, operat...
#include "libsemigroups/detail/rules.hpp"
#include "libsemigroups/exception.hpp"              // for LibsemigroupsExcep...
#include "libsemigroups/knuth-bendix-helpers.hpp"   // for SubwordSolver
#include "libsemigroups/knuth-bendix.hpp"           // for KnuthBendix, norma...
#include "libsemigroups/order.hpp"                  // for shortlex_compare
#include "libsemigroups/paths.hpp"                  // for Paths
#include "libsemigroups/presentation-examples.hpp"  // for partition_mo
#include "libsemigroups/presentation.hpp"           // for add_rule, Presenta...
#include "libsemigroups/word-graph-helpers.hpp"     // for word_graph
#include "libsemigroups/word-graph.hpp"             // for WordGraph
#include "libsemigroups/word-range.hpp"             // for Inner, StringRange...

#include "libsemigroups/detail/report.hpp"  // for ReportGuard
#include "libsemigroups/detail/stl.hpp"     // for apply_permutation
#include "libsemigroups/detail/string.hpp"  // for random_string, operator<<

namespace libsemigroups {
  using literals::operator""_w;

  congruence_kind constexpr twosided = congruence_kind::twosided;

  using namespace rx;

  using LenLexTrie = detail::RewritingSystemTrie<ShortLexCompare>;
  using LenLexSet  = detail::RewritingSystemSet<ShortLexCompare>;
  using RPOTrie    = detail::RewritingSystemTrie<RecursivePathCompare>;
  using RPOSet     = detail::RewritingSystemSet<RecursivePathCompare>;

#define REWRITING_SYSTEM_TYPES LenLexTrie, RPOTrie

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "110",
                                   "Ceitin's undecidable word problem example",
                                   "[fail][knuth-bendix]",
                                   REWRITING_SYSTEM_TYPES) {
    auto                      rg = ReportGuard(true);
    Presentation<std::string> p;
    p.contains_empty_word(true);
    p.alphabet("abcde");
    presentation::add_rule(p, "ac", "ca");
    presentation::add_rule(p, "ad", "da");
    presentation::add_rule(p, "bc", "cb");
    presentation::add_rule(p, "bd", "db");
    presentation::add_rule(p, "eca", "ce");
    presentation::add_rule(p, "edb", "de");
    presentation::add_rule(p, "cca", "ccae");

    KnuthBendix<std::string, TestType> kb(twosided, p);
    kb.run();  // I guess this shouldn't work, and indeed it doesn't!
  }

  // kbmag/standalone/kb_data/verifynilp
  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "111",
                                   "kbmag/standalone/kb_data/verifynilp",
                                   "[fail][knuth-bendix][kbmag]",
                                   RPOTrie) {
    auto        rg    = ReportGuard();
    std::string lphbt = "hHgGfFyYdDcCbBaA";
    std::string invrs = "HhGgFfYyDdCcBbAa";

    Presentation<std::string> p;
    p.contains_empty_word(true);
    p.alphabet(lphbt);

    presentation::add_inverse_rules(p, invrs);

    presentation::add_rule(p, "BAba", "c");
    presentation::add_rule(p, "CAca", "d");
    presentation::add_rule(p, "DAda", "y");
    presentation::add_rule(p, "YByb", "f");
    presentation::add_rule(p, "FAfa", "g");
    presentation::add_rule(p, "ga", "ag");
    presentation::add_rule(p, "GBgb", "h");
    presentation::add_rule(p, "cb", "bc");
    presentation::add_rule(p, "ya", "ay");

    KnuthBendix<std::string, TestType> kb(twosided, p);

    REQUIRE(!kb.rewriting_system().confluent());
    kb.rewriting_system().sort_pending_rules_by(detail::rpo_cmp);

    knuth_bendix::by_overlap_length(kb);
    REQUIRE(kb.rewriting_system().number_of_rules() == 0);
    REQUIRE(kb.number_of_classes() == 0);
  }

  // TODO move to quick
  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "114",
                                   "hard 2-generated 1-relation monoid",
                                   "[quick][knuth-bendix][tietze-explorer]",
                                   RPOTrie) {
    fmt::print("\n");
    Presentation<std::string> p;
    p.contains_empty_word(true);
    p.alphabet("abc");
    presentation::add_rule(p, "a", "cc");
    presentation::add_rule(p, "c", "bab");

    KnuthBendix<std::string, TestType> kb(twosided, p);

    knuth_bendix::TietzeExplorer solver(kb);
    kb = solver.run();

    using rule_type = typename decltype(kb)::rule_type;
    REQUIRE(kb.presentation().alphabet() == "cab");
    REQUIRE(kb.presentation().rules
            == std::vector<std::string>({"a", "cc", "c", "bab"}));
    REQUIRE((kb.active_rules() | rx::to_vector())
            == std::vector<rule_type>(
                {{"a", "cc"}, {"bccb", "c"}, {"cccb", "bccc"}}));

    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE(
      "KnuthBendix",
      "116",
      "https://math.stackexchange.com/questions/2649807",
      "[knuth-bendix][extreme][tietze-explorer]",
      LenLexTrie) {
    std::string lphbt = "abcB";
    std::string invrs = "aBcb";

    Presentation<std::string> p;
    p.contains_empty_word(true);
    p.alphabet(lphbt);

    presentation::add_inverse_rules(p, invrs);
    presentation::add_rule(p, "aa", "");
    presentation::add_rule(p, "bbbbbbbbbbb", "");
    presentation::add_rule(p, "cc", "");
    presentation::add_rule(p, "abababab", "");
    presentation::add_rule(p, "abbabbabbabbabbabb", "");
    presentation::add_rule(p, "abbabaBabaBBabbaB", "");
    presentation::add_rule(p, "acacac", "");
    presentation::add_rule(p, "bcbc", "");

    KnuthBendix<std::string, TestType> kb(twosided, p);
    knuth_bendix::TietzeExplorer       solver(kb);
    solver.depth_max(1);

    auto result     = solver.run();
    using rule_type = typename decltype(result)::rule_type;
    REQUIRE((result.active_rules() | rx::to_vector())
            == std::vector<rule_type>({}));
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "057",
                                   "1-relation hard case",
                                   "[extreme][knuth-bendix][tietze-explorer]",
                                   RPOTrie) {
    auto                      rg = ReportGuard(true);
    Presentation<std::string> p;
    p.alphabet("ba");
    p.contains_empty_word(true);
    presentation::add_rule(p, "baaababaaa", "aaba");

    KnuthBendix<std::string, TestType> kb(twosided, p);
    knuth_bendix::TietzeExplorer       solver(kb);
    solver.depth_max(2).run_each_for(std::chrono::milliseconds(1));

    auto result = solver.run();
    REQUIRE(result.rewriting_system().active_rules().size() == 0);
  }

  // TODO move to quick
  LIBSEMIGROUPS_TEMPLATE_TEST_CASE(
      "KnuthBendix",
      "099",
      "Giles Gardam (https://arxiv.org/abs/2102.11818)",
      "[quick][tietze-explorer]",
      RPOTrie) {
    fmt::print("\n");
    Presentation<std::string> p;
    p.alphabet("bABa");
    p.contains_empty_word(true);
    presentation::add_inverse_rules(p, "BabA");
    presentation::add_rule(p, "Abba", "BB");
    presentation::add_rule(p, "Baab", "AA");

    KnuthBendix<std::string, TestType> kb(twosided, p);
    knuth_bendix::TietzeExplorer       solver(kb);
    auto                               result = solver.depth_max(2).run();

    using rule_type = typename decltype(kb)::rule_type;
    using rule_type = typename decltype(kb)::rule_type;
    REQUIRE(result.finished());
    REQUIRE(result.presentation().alphabet() == "bBcAa");
    REQUIRE(result.presentation().rules
            == std::vector<std::string>({"bB",
                                         "",
                                         "Aa",
                                         "",
                                         "Bb",
                                         "",
                                         "aA",
                                         "",
                                         "Abba",
                                         "BB",
                                         "cb",
                                         "AA",
                                         "c",
                                         "Baa"}));

    REQUIRE((result.active_rules() | rx::to_vector())
            == std::vector<rule_type>({{"AA", "cb"},
                                       {"Bb", ""},
                                       {"bB", ""},
                                       {"cbA", "Acb"},
                                       {"a", "Abc"},
                                       {"BA", "bAbb"},
                                       {"cc", "BB"},
                                       {"bbc", "cbb"},
                                       {"cA", "bAbcbb"},
                                       {"bbA", "ABB"},
                                       {"Bc", "bcBB"}}));
    REQUIRE(result.rewriting_system().confluent());
    // REQUIRE(kb.presentation().alphabet() == "BbcaA");
    // REQUIRE((kb.active_rules() | rx::to_vector())
    //         == std::vector<rule_type>({{"bB", ""},
    //                                    {"Bb", ""},
    //                                    {"A", "acb"},
    //                                    {"ba", "BaBB"},
    //                                    {"cc", "BB"},
    //                                    {"BBa", "abb"},
    //                                    {"cBa", "acbbb"},
    //                                    {"bc", "Bcbb"},
    //                                    {"BBc", "cBB"},
    //                                    {"ca", "BaBcbb"},
    //                                    {"aa", "Bcbb"}}));
    REQUIRE(result.number_of_classes() == POSITIVE_INFINITY);
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "147",
                                   "aaa=a, abba=bb",
                                   "[extreme][tietze-explorer]",
                                   LenLexTrie) {
    fmt::print("\n");
    Presentation<std::string> p;
    p.alphabet("ab");
    p.contains_empty_word(true);
    presentation::add_rule(p, "aaa", "a");
    presentation::add_rule(p, "abba", "bb");

    KnuthBendix<std::string, TestType> kb(twosided, p);
    knuth_bendix::TietzeExplorer       solver(kb);
    auto                               result = solver.run();

    using rule_type = typename decltype(kb)::rule_type;
    REQUIRE((result.active_rules() | rx::to_vector())
            == std::vector<rule_type>({}));
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "149",
                                   "abaab=aabba",
                                   "[quick][tietze-explorer]",
                                   LenLexTrie) {
    fmt::print("\n");
    Presentation<std::string> p;
    p.alphabet("ab");
    p.contains_empty_word(true);
    presentation::add_rule(p, "abbab", "baabb");

    KnuthBendix<std::string, TestType> kb(twosided, p);
    knuth_bendix::TietzeExplorer       solver(kb);
    auto                               result = solver.run();

    using rule_type = typename decltype(kb)::rule_type;
    REQUIRE((result.active_rules() | rx::to_vector())
            == std::vector<rule_type>({{"baab", "c"},
                                       {"abba", "d"},
                                       {"db", "cb"},
                                       {"baac", "caab"},
                                       {"bad", "cba"},
                                       {"abbd", "cbba"},
                                       {"abc", "dab"},
                                       {"dc", "cc"},
                                       {"bacb", "cbab"},
                                       {"bacc", "cbac"},
                                       {"abbcb", "cbbab"},
                                       {"abbcc", "cbbac"}}));
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "150",
                                   "baabbbaba=a",
                                   "[standard][tietze-explorer]",
                                   RPOTrie) {
    fmt::print("\n");
    Presentation<std::string> p;
    p.alphabet("ab");
    p.contains_empty_word(true);
    presentation::add_rule(p, "baabbbaba", "a");

    KnuthBendix<std::string, TestType> kb(twosided, p);
    knuth_bendix::TietzeExplorer       solver(kb);
    auto                               result = solver.depth_max(2).run();

    using rule_type = typename decltype(kb)::rule_type;
    REQUIRE((result.active_rules() | rx::to_vector())
            == std::vector<rule_type>({{"ba", "dada"},
                                       {"bdada", "daddada"},
                                       {"bdaddada", "d"},
                                       {"dadaada", "aaddada"},
                                       {"c", "dadaadb"},
                                       {"dadaaddada", "a"},
                                       {"bdaa", "daddaa"},
                                       {"bdaddaa", "ddaaddada"},
                                       {"dadaaa", "aaddaa"},
                                       {"dadaaddaa", "adaaddada"}}));
  }

  // Fails with depth_max(3) + RPOTrie too in ~10 minutes
  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "151",
                                   "baaabaaa = aba",
                                   "[fail][tietze-explorer]",
                                   RPOTrie,
                                   LenLexTrie) {
    fmt::print("\n");
    Presentation<std::string> p;
    p.alphabet("ab");
    p.contains_empty_word(true);
    presentation::add_rule(p, "baaabaaa", "aba");

    KnuthBendix<std::string, TestType> kb(twosided, p);
    knuth_bendix::TietzeExplorer       solver(kb);
    auto                               result = solver.depth_max(2).run();

    using rule_type = typename decltype(kb)::rule_type;
    REQUIRE((result.active_rules() | rx::to_vector())
            == std::vector<rule_type>({}));
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "152",
                                   "baa(ba)^N=a",
                                   "[extreme][tietze-explorer]",
                                   RPOTrie) {
    fmt::print("\n");

    Presentation<std::string> p;
    p.alphabet("ab");
    p.contains_empty_word(true);

    KnuthBendix<std::string, TestType> kb;

    using rule_type = typename decltype(kb)::rule_type;

    std::vector<std::vector<rule_type>> expected
        = {{{"c", "dadb"},
            {"ba", "d"},
            {"dadd", "a"},
            {"dada", "aadd"},
            {"daa", "aadddd"}},
           {{"c", "baadb"},
            {"aba", "baadd"},
            {"dba", "bad"},
            {"bbaada", "a"},
            {"bbaadd", "d"},
            {"daddd", "a"},
            {"bbaaa", "addd"},
            {"dadda", "aaddd"},
            {"dada", "aadddddd"},
            {"daa", "aaddddddddd"}},
           {{"ca", "a"},
            {"cd", "d"},
            {"dab", "abd"},
            {"abab", "badd"},
            {"dbadac", "baddac"},
            {"bbadad", "d"},
            {"bbadac", "c"},
            {"bbadaa", "a"},
            {"abbadd", "d"},
            {"cbadd", "badd"},
            {"dbadd", "baddd"},
            {"cbadac", "badac"},
            {"dbadaa", "baddaa"},
            {"dbadad", "baddad"},
            {"babaddd", "ab"},
            {"babaddac", "abadddd"},
            {"dadddd", "ac"},
            {"cbadaa", "badaa"},
            {"cbadad", "badad"},
            {"bbaac", "dddd"},
            {"babaddaa", "abadddda"},
            {"babaddad", "abaddddd"},
            {"aab", "baddaddd"},
            {"dbaac", "badac"},
            {"dadddac", "aadddd"},
            {"bbaaa", "dddda"},
            {"bbaad", "ddddd"},
            {"babadac", "abadddddddd"},
            {"dbaaa", "badaa"},
            {"dbaad", "badad"},
            {"dadddaa", "aadddda"},
            {"dadddad", "aaddddd"},
            {"cbaac", "baac"},
            {"babadaa", "abadddddddda"},
            {"babadad", "abaddddddddd"},
            {"dadac", "aadddddddddddd"},
            {"babaac", "abadddddddddddd"},
            {"dadaa", "aadddddddddddda"},
            {"daddac", "aadddddddd"},
            {"cbaaa", "baaa"},
            {"cbaad", "baad"},
            {"dadad", "aaddddddddddddd"},
            {"babaaa", "abadddddddddddda"},
            {"babaad", "abaddddddddddddd"},
            {"daac", "aadddddddddddddddd"},
            {"daddaa", "aadddddddda"},
            {"daddad", "aaddddddddd"},
            {"daaa", "aadddddddddddddddda"},
            {"daad", "aaddddddddddddddddd"}},
           {{"dbadd", "baddd"},
            {"dab", "abd"},
            {"dbadaa", "baddaa"},
            {"bbadaa", "a"},
            {"bbabaddaa", "abda"},
            {"abbadd", "d"},
            {"dbabaddaa", "babadddaa"},
            {"dbabaddad", "babadddad"},
            {"dbadad", "baddad"},
            {"bbadad", "d"},
            {"bbabaddad", "abdd"},
            {"bbabadddadddd", "ab"},
            {"daddddda", "aa"},
            {"dadddddd", "ad"},
            {"bbaaa", "ddddda"},
            {"bbaad", "dddddd"},
            {"abab", "babaddd"},
            {"aab", "baddadddd"},
            {"c", "bbaddaddddd"},
            {"bbabaaa", "abddddddddddda"},
            {"bbabaad", "abdddddddddddd"},
            {"bbabadaa", "abdddddda"},
            {"dbabadaa", "babaddaa"},
            {"dbaaa", "badaa"},
            {"daddddaa", "aaddddda"},
            {"bbabadad", "abddddddd"},
            {"dbabaaa", "babadaa"},
            {"dbabaad", "babadad"},
            {"dbabadad", "babaddad"},
            {"dbaad", "badad"},
            {"daddddad", "aadddddd"},
            {"abbabaddd", "badd"},
            {"dbabaddd", "babadddd"},
            {"dadddaa", "aadddddddddda"},
            {"dadddad", "aaddddddddddd"},
            {"daddaa", "aaddddddddddddddda"},
            {"daddad", "aadddddddddddddddd"},
            {"dadad", "aaddddddddddddddddddddd"},
            {"dadaa", "aadddddddddddddddddddda"},
            {"daaa", "aaddddddddddddddddddddddddda"},
            {"daad", "aadddddddddddddddddddddddddd"}},
           {{"dbadd", "baddd"},
            {"dab", "abd"},
            {"dbadaa", "baddaa"},
            {"bbadaa", "a"},
            {"bbabaddaa", "abda"},
            {"abbadd", "d"},
            {"dbabaddaa", "babadddaa"},
            {"dbabaddad", "babadddad"},
            {"dbadad", "baddad"},
            {"bbadad", "d"},
            {"bbabaddad", "abdd"},
            {"bababadddd", "abab"},
            {"dadddddda", "aa"},
            {"daddddddd", "ad"},
            {"ababab", "babaddd"},
            {"bababadddaa", "ababadddddda"},
            {"bababadddad", "ababaddddddd"},
            {"bbaaa", "dddddda"},
            {"bbaad", "ddddddd"},
            {"bbabadaa", "abddddddda"},
            {"bbabadad", "abdddddddd"},
            {"dbabadaa", "babaddaa"},
            {"dbaaa", "badaa"},
            {"dbaad", "badad"},
            {"aab", "baddaddddd"},
            {"dadddddaa", "aadddddda"},
            {"dadddddad", "aaddddddd"},
            {"c", "bbaddadddddd"},
            {"bababaddaa", "ababadddddddddddda"},
            {"bababaddad", "ababaddddddddddddd"},
            {"abbabaddd", "badd"},
            {"dbabaddd", "babadddd"},
            {"dbabadad", "babaddad"},
            {"bbabaaa", "abddddddddddddda"},
            {"bbabaad", "abdddddddddddddd"},
            {"bbabadddaddddd", "ab"},
            {"daddddaa", "aadddddddddddda"},
            {"daddddad", "aaddddddddddddd"},
            {"bababadaa", "ababadddddddddddddddddda"},
            {"bababadad", "ababaddddddddddddddddddd"},
            {"daddaa", "aadddddddddddddddddddddddda"},
            {"daddad", "aaddddddddddddddddddddddddd"},
            {"dbabaaa", "babadaa"},
            {"dbabaad", "babadad"},
            {"dadaa", "aadddddddddddddddddddddddddddddda"},
            {"dadddaa", "aadddddddddddddddddda"},
            {"dadddad", "aaddddddddddddddddddd"},
            {"bababaaa", "ababadddddddddddddddddddddddda"},
            {"bababaad", "ababaddddddddddddddddddddddddd"},
            {"dadad", "aaddddddddddddddddddddddddddddddd"},
            {"daaa", "aadddddddddddddddddddddddddddddddddddda"},
            {"daad", "aaddddddddddddddddddddddddddddddddddddd"}}};

    for (auto N = 2; N < 7; ++N) {
      p.rules = {parse(fmt::format("baa(ba)^{}", N)), "a"};
      kb.init(twosided, p);
      knuth_bendix::TietzeExplorer solver(kb);
      auto                         result = solver.depth_max(2).run();

      REQUIRE((result.active_rules() | rx::to_vector()) == expected[N - 2]);
    }
  }

  namespace {
    std::string swap_a_and_b(std::string const& w) {
      std::string result;
      for (auto l : w) {
        if (l == 'a') {
          result += "b";
        } else {
          result += "a";
        }
      }
      return result;
    }

    std::unordered_set<std::string>& get_set() {
      static std::unordered_set<std::string> set;
      return set;
    }

    template <typename T>
    void register_relation(T const& it1, T const& it2, size_t& nr) {
      auto tmp = it1 + "#" + it2;
      auto u   = swap_a_and_b(it1);
      auto v   = swap_a_and_b(it2);
      if (shortlex_compare(u, v)) {
        get_set().insert(u + "#" + v);
      } else {
        get_set().insert(v + "#" + u);
      }
      std::cout << it1 << " = " << it2 << std::endl;
      nr++;
    }
  }  // namespace

  // This test case doesn't fail it's just extremely time consuming
  LIBSEMIGROUPS_TEMPLATE_TEST_CASE(
      "KnuthBendix",
      "113",
      "all 2-generated 1-relation semigroups 1 to 10",
      "[fail][knuth-bendix][xxx]",
      REWRITING_SYSTEM_TYPES) {
    auto rg = ReportGuard(false);

    StringRange lhss;
    lhss.alphabet("ab").min(1).max(11);
    REQUIRE((lhss | count()) == 2'046);

    StringRange rhss;
    rhss.alphabet("ab").max(11);

    size_t total_c4 = 0;
    size_t total    = 0;

    for (auto const& lhs : lhss) {
      rhss.first(lhs);
      for (auto const& rhs : rhss | skip_n(1)) {
        auto tmp = lhs + "#" + rhs;
        if (get_set().insert(tmp).second) {
          bool try_again = false;
          {
            Presentation<std::string> p;
            p.contains_empty_word(true);
            p.alphabet("ab");
            presentation::add_rule(p, lhs, rhs);
            KnuthBendix<std::string, TestType> k(twosided, p);
            k.run_for(std::chrono::milliseconds(10));
            if (k.rewriting_system().confluent()) {
              register_relation(lhs, rhs, total_c4);
            } else {
              try_again = true;
            }
          }
          if (try_again) {
            Presentation<std::string> p;
            p.contains_empty_word(true);
            p.alphabet("ba");
            presentation::add_rule(p, lhs, rhs);
            KnuthBendix<std::string, TestType> k(twosided, p);
            k.run_for(std::chrono::milliseconds(10));
            if (k.rewriting_system().confluent()) {
              register_relation(lhs, rhs, total_c4);
            }
          }
        }
      }
    }
    REQUIRE(total_c4 == 471'479);
    REQUIRE(total == 2'092'035);
  }

}  // namespace libsemigroups
