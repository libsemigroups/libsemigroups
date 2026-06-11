// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2020-2026 James D. Mitchell
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

#include <cstddef>      // for size_t
#include <cstdint>      // for uint64_t
#include <iostream>     // for string, ostringstream
#include <string>       // for allocator, basic_string
#include <type_traits>  // for is_same_v
#include <utility>      // for move
#include <vector>       // for vector, operator==

#define CATCH_CONFIG_ENABLE_ALL_STRINGMAKERS
#define CATCH_CONFIG_ENABLE_PAIR_STRINGMAKER

#include "Catch2-3.14.0/catch_amalgamated.hpp"  // for AssertionHandler, oper...
#include "test-main.hpp"  // for LIBSEMIGROUPS_TEMPLATE_TEST_CASE

#include "libsemigroups/constants.hpp"     // for operator==, operator!=
#include "libsemigroups/exception.hpp"     // for LibsemigroupsException
#include "libsemigroups/knuth-bendix.hpp"  // for KnuthBendix, normal_forms
#include "libsemigroups/order.hpp"         // for lenlex_cmp
#include "libsemigroups/paths.hpp"         // for Paths
#include "libsemigroups/presentation-examples.hpp"  // for chinese
#include "libsemigroups/presentation.hpp"     // for add_rule, Presentation
#include "libsemigroups/to-froidure-pin.hpp"  // for to<FroidurePin>
#include "libsemigroups/word-graph.hpp"       // for WordGraph

#include "libsemigroups/detail/report.hpp"  // for ReportGuard

#include "libsemigroups/ranges.hpp"  // for operator|, Inner, to_v...

namespace libsemigroups {

  congruence_kind constexpr twosided = congruence_kind::twosided;

  using namespace rx;

  struct LibsemigroupsException;

  using LenLexTrie = detail::RewritingSystemTrie<LenLexCmp>;
  using LenLexSet  = detail::RewritingSystemSet<LenLexCmp>;

  using RPOTrie = detail::RewritingSystemTrie<RevRPOCmp>;
  using RPOSet  = detail::RewritingSystemSet<RevRPOCmp>;

#define REWRITING_SYSTEM_TYPES LenLexTrie, LenLexSet, RPOTrie, RPOSet

  // Fibonacci group F(2,5) - monoid presentation - has order 12 (group
  // elements + empty word)
  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "027",
                                   "kbmag/standalone/kb_data/f25monoid",
                                   "[quick][knuth-bendix][kbmag]",
                                   REWRITING_SYSTEM_TYPES) {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("abcde");

    presentation::add_rule(p, "ab", "c");
    presentation::add_rule(p, "bc", "d");
    presentation::add_rule(p, "cd", "e");
    presentation::add_rule(p, "de", "a");
    presentation::add_rule(p, "ea", "b");

    KnuthBendix<std::string, TestType> kb(twosided, p);

    REQUIRE(!kb.rewriting_system().confluent());

    kb.run();
    REQUIRE(kb.rewriting_system().confluent());
    using order = typename TestType::reduction_order;
    if constexpr (std::is_same_v<order, LenLexCmp>) {
      REQUIRE(kb.rewriting_system().number_of_rules() == 24);
      REQUIRE(knuth_bendix::reduce_no_run(kb, "ca") == "ac");
      using rule_type = typename decltype(kb)::rule_type;
      REQUIRE((kb.active_rules() | sort(weird_cmp()) | to_vector())
              == std::vector<rule_type>(
                  {{"ab", "c"},  {"ae", "b"},   {"ba", "c"},  {"bc", "d"},
                   {"bd", "aa"}, {"ca", "ac"},  {"cb", "d"},  {"cc", "ad"},
                   {"cd", "e"},  {"ce", "bb"},  {"da", "ad"}, {"db", "aa"},
                   {"dc", "e"},  {"dd", "be"},  {"de", "a"},  {"ea", "b"},
                   {"eb", "be"}, {"ec", "bb"},  {"ed", "a"},  {"ee", "ac"},
                   {"aaa", "e"}, {"aac", "be"}, {"bbb", "a"}, {"bbe", "aad"}}));

      auto nf = knuth_bendix::normal_forms(kb);
      REQUIRE(
          (nf.min(1).max(5) | to_vector())
          == std::vector<std::string>(
              {"a", "b", "c", "d", "e", "aa", "ac", "ad", "bb", "be", "aad"}));
      REQUIRE(kb.number_of_classes() == 11);
      REQUIRE(nf.min(1).max(POSITIVE_INFINITY).count() == 11);
    } else if (std::is_same_v<order, RevRPOCmp>) {
      REQUIRE(kb.rewriting_system().number_of_rules() == 5);
    }

    REQUIRE(knuth_bendix::contains(kb, "ab", "c"));
    REQUIRE(knuth_bendix::contains(kb, "bc", "d"));
    REQUIRE(knuth_bendix::contains(kb, "cd", "e"));
    REQUIRE(knuth_bendix::contains(kb, "de", "a"));
    REQUIRE(knuth_bendix::contains(kb, "ea", "b"));
    REQUIRE(knuth_bendix::contains(kb, "cc", "ad"));
    REQUIRE(knuth_bendix::contains(kb, "dd", "be"));
    REQUIRE(knuth_bendix::contains(kb, "ee", "ca"));
    REQUIRE(knuth_bendix::contains(kb, "ec", "bb"));
    REQUIRE(knuth_bendix::contains(kb, "db", "aa"));
    REQUIRE(knuth_bendix::contains(kb, "aac", "be"));
    REQUIRE(knuth_bendix::contains(kb, "bd", "aa"));
    REQUIRE(knuth_bendix::contains(kb, "bbe", "aad"));
    REQUIRE(knuth_bendix::contains(kb, "aaa", "e"));
    REQUIRE(knuth_bendix::contains(kb, "eb", "be"));
    REQUIRE(knuth_bendix::contains(kb, "ba", "c"));
    REQUIRE(knuth_bendix::contains(kb, "da", "ad"));
    REQUIRE(knuth_bendix::contains(kb, "ca", "ac"));
    REQUIRE(knuth_bendix::contains(kb, "ce", "bb"));
    REQUIRE(knuth_bendix::contains(kb, "cb", "d"));
    REQUIRE(knuth_bendix::contains(kb, "ed", "a"));
    REQUIRE(knuth_bendix::contains(kb, "dc", "e"));
    REQUIRE(knuth_bendix::contains(kb, "ae", "b"));
    REQUIRE(knuth_bendix::contains(kb, "bbb", "a"));
  }

  // trivial group - BHN presentation
  // RPOTrie is very slow here
  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "028",
                                   "kbmag/standalone/kb_data/degen4a",
                                   "[quick][knuth-bendix][kbmag][no-valgrind]",
                                   LenLexSet,
                                   LenLexTrie,
                                   RPOSet) {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("aAbBcC");
    p.contains_empty_word(true);
    presentation::add_inverse_rules(p, "AaBbCc");

    presentation::add_rule(p, "Aba", "bb");
    presentation::add_rule(p, "Bcb", "cc");
    presentation::add_rule(p, "Cac", "aa");

    KnuthBendix<std::string, TestType> kb(twosided, p);

    REQUIRE(!kb.rewriting_system().confluent());

    kb.run();
    REQUIRE(kb.rewriting_system().confluent());
    REQUIRE(kb.rewriting_system().number_of_rules() == 6);

    REQUIRE(knuth_bendix::contains(kb, "Aba", "bb"));
    REQUIRE(knuth_bendix::contains(kb, "Bcb", "cc"));
    REQUIRE(knuth_bendix::contains(kb, "Cac", "aa"));
    using rule_type = typename decltype(kb)::rule_type;
    REQUIRE((kb.active_rules() | sort(weird_cmp()) | to_vector())
            == std::vector<rule_type>({{"A", ""},
                                       {"B", ""},
                                       {"C", ""},
                                       {"a", ""},
                                       {"b", ""},
                                       {"c", ""}}));
    REQUIRE(kb.number_of_classes() == 1);
    auto nf = knuth_bendix::normal_forms(kb);
    REQUIRE(nf.count() == 1);
  }

  // Torus group
  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "029",
                                   "kbmag/standalone/kb_data/torus",
                                   "[quick][knuth-bendix][kbmag]",
                                   REWRITING_SYSTEM_TYPES) {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("aAcCbBdD");
    p.contains_empty_word(true);
    presentation::add_inverse_rules(p, "AaCcBbDd");
    presentation::add_rule(p, "ABab", "DCdc");

    KnuthBendix<std::string, TestType> kb(twosided, p);
    REQUIRE(!kb.rewriting_system().confluent());
    kb.run();
    REQUIRE(kb.rewriting_system().confluent());
    auto nf = knuth_bendix::normal_forms(kb).min(0).max(6);

    using order     = typename TestType::reduction_order;
    using rule_type = typename decltype(kb)::rule_type;
    if constexpr (std::is_same_v<order, LenLexCmp>) {
      REQUIRE(kb.rewriting_system().number_of_rules() == 16);
      REQUIRE((kb.active_rules() | sort(weird_cmp()) | to_vector())
              == std::vector<rule_type>({{"Aa", ""},
                                         {"Bb", ""},
                                         {"Cc", ""},
                                         {"Dd", ""},
                                         {"aA", ""},
                                         {"bB", ""},
                                         {"cC", ""},
                                         {"dD", ""},
                                         {"BAba", "CDcd"},
                                         {"BabC", "aDCd"},
                                         {"DCdc", "ABab"},
                                         {"DcdA", "cBAb"},
                                         {"bCDc", "AbaD"},
                                         {"baDC", "abCD"},
                                         {"dABa", "CdcB"},
                                         {"dcBA", "cdAB"}}));
      REQUIRE(nf.count() == 155'577);
      REQUIRE(
          (nf.min(0).max(2) | to_vector())
          == std::vector<std::string>(
              {"",   "a",  "A",  "c",  "C",  "b",  "B",  "d",  "D",  "aa", "ac",
               "aC", "ab", "aB", "ad", "aD", "AA", "Ac", "AC", "Ab", "AB", "Ad",
               "AD", "ca", "cA", "cc", "cb", "cB", "cd", "cD", "Ca", "CA", "CC",
               "Cb", "CB", "Cd", "CD", "ba", "bA", "bc", "bC", "bb", "bd", "bD",
               "Ba", "BA", "Bc", "BC", "BB", "Bd", "BD", "da", "dA", "dc", "dC",
               "db", "dB", "dd", "Da", "DA", "Dc", "DC", "Db", "DB", "DD"}));
    } else {
      REQUIRE(kb.rewriting_system().number_of_rules() == 12);
      REQUIRE((kb.active_rules() | sort(weird_cmp()) | to_vector())
              == std::vector<rule_type>({{"Aa", ""},
                                         {"Bb", ""},
                                         {"Cc", ""},
                                         {"Cd", "dABabC"},
                                         {"Dd", ""},
                                         {"aA", ""},
                                         {"bB", ""},
                                         {"cC", ""},
                                         {"cd", "dcBAba"},
                                         {"dD", ""},
                                         {"AbaD", "bCDc"},
                                         {"abCD", "baDC"}}));
      REQUIRE(nf.count() == 130'643);
      REQUIRE(
          (nf.min(0).max(2) | to_vector())
          == std::vector<std::string>(
              {"",   "a",  "A",  "c",  "C",  "b",  "B",  "d",  "D",  "aa", "ac",
               "aC", "ab", "aB", "ad", "aD", "AA", "Ac", "AC", "Ab", "AB", "Ad",
               "AD", "ca", "cA", "cc", "cb", "cB", "cD", "Ca", "CA", "CC", "Cb",
               "CB", "CD", "ba", "bA", "bc", "bC", "bb", "bd", "bD", "Ba", "BA",
               "Bc", "BC", "BB", "Bd", "BD", "da", "dA", "dc", "dC", "db", "dB",
               "dd", "Da", "DA", "Dc", "DC", "Db", "DB", "DD"}));
    }

    REQUIRE(knuth_bendix::contains(kb, "DCdc", "ABab"));
    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
  }

  //  3-fold cover of A_6
  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "030",
                                   "kbmag/standalone/kb_data/3a6",
                                   "[quick][knuth-bendix][kbmag][no-valgrind]",
                                   REWRITING_SYSTEM_TYPES) {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.contains_empty_word(true);
    p.alphabet("abAB");

    presentation::add_inverse_rules(p, "ABab");

    presentation::add_rule(p, "aaa", "");
    presentation::add_rule(p, "bbb", "");
    presentation::add_rule(p, "abababab", "");
    presentation::add_rule(p, "aBaBaBaBaB", "");

    KnuthBendix<std::string, TestType> kb(twosided, p);
    REQUIRE(!kb.rewriting_system().confluent());

    kb.run();
    REQUIRE(kb.rewriting_system().confluent());
    using order = typename TestType::reduction_order;
    if constexpr (std::is_same_v<order, LenLexCmp>) {
      REQUIRE(kb.rewriting_system().number_of_rules() == 183);
      auto nf = knuth_bendix::normal_forms(kb);

      REQUIRE(nf.count() == 1080);
      REQUIRE((nf.min(0).max(2) | to_vector())
              == std::vector<std::string>({"",
                                           "a",
                                           "b",
                                           "A",
                                           "B",
                                           "ab",
                                           "aB",
                                           "ba",
                                           "bA",
                                           "Ab",
                                           "AB",
                                           "Ba",
                                           "BA"}));
    } else if (std::is_same_v<order, RevRPOCmp>) {
      REQUIRE(kb.rewriting_system().number_of_rules() == 72);
    }

    REQUIRE(knuth_bendix::contains(kb, "aaa", ""));
    REQUIRE(knuth_bendix::contains(kb, "bbb", ""));
    REQUIRE(knuth_bendix::contains(kb, "BaBaBaBaB", "aa"));
    REQUIRE(knuth_bendix::contains(kb, "bababa", "aabb"));
    REQUIRE(knuth_bendix::contains(kb, "ababab", "bbaa"));
    REQUIRE(knuth_bendix::contains(kb, "aabbaa", "babab"));
    REQUIRE(knuth_bendix::contains(kb, "bbaabb", "ababa"));
    REQUIRE(knuth_bendix::contains(kb, "bababbabab", "aabbabbaa"));
    REQUIRE(knuth_bendix::contains(kb, "ababaababa", "bbaabaabb"));
    REQUIRE(knuth_bendix::contains(kb, "bababbabaababa", "aabbabbaabaabb"));
    REQUIRE(knuth_bendix::contains(kb, "bbaabaabbabbaa", "ababaababbabab"));

    REQUIRE(kb.number_of_classes() == 1080);
  }

  //  Free group on 2 generators
  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "031",
                                   "kbmag/standalone/kb_data/f2",
                                   "[quick][knuth-bendix][kbmag]",
                                   REWRITING_SYSTEM_TYPES) {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("aAbB");
    p.contains_empty_word(true);
    presentation::add_inverse_rules(p, "AaBb");

    KnuthBendix<std::string, TestType> kb(twosided, p);
    // kb.process_pending_rules();
    REQUIRE(kb.rewriting_system().confluent());
    kb.run();
    REQUIRE(kb.rewriting_system().confluent());
    REQUIRE(kb.rewriting_system().number_of_rules() == 4);
    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);

    auto nf = knuth_bendix::normal_forms(kb).min(0).max(2);

    REQUIRE((nf | to_vector())
            == std::vector<std::string>({"",
                                         "a",
                                         "A",
                                         "b",
                                         "B",
                                         "aa",
                                         "ab",
                                         "aB",
                                         "AA",
                                         "Ab",
                                         "AB",
                                         "ba",
                                         "bA",
                                         "bb",
                                         "Ba",
                                         "BA",
                                         "BB"}));
    REQUIRE(nf.min(0).max(4).count() == 161);
  }

  // Symmetric group S_16
  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "032",
                                   "kbmag/standalone/kb_data/s16",
                                   "[quick][knuth-bendix][kbmag][no-valgrind]",
                                   REWRITING_SYSTEM_TYPES) {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("abcdefghijklmno");
    p.contains_empty_word(true);

    presentation::add_inverse_rules(p, "abcdefghijklmno");

    presentation::add_rule(p, "bab", "aba");
    presentation::add_rule(p, "ca", "ac");
    presentation::add_rule(p, "da", "ad");
    presentation::add_rule(p, "ea", "ae");
    presentation::add_rule(p, "fa", "af");
    presentation::add_rule(p, "ga", "ag");
    presentation::add_rule(p, "ha", "ah");
    presentation::add_rule(p, "ia", "ai");
    presentation::add_rule(p, "ja", "aj");
    presentation::add_rule(p, "ka", "ak");
    presentation::add_rule(p, "la", "al");
    presentation::add_rule(p, "ma", "am");
    presentation::add_rule(p, "na", "an");
    presentation::add_rule(p, "oa", "ao");
    presentation::add_rule(p, "cbc", "bcb");
    presentation::add_rule(p, "db", "bd");
    presentation::add_rule(p, "eb", "be");
    presentation::add_rule(p, "fb", "bf");
    presentation::add_rule(p, "gb", "bg");
    presentation::add_rule(p, "hb", "bh");
    presentation::add_rule(p, "ib", "bi");
    presentation::add_rule(p, "jb", "bj");
    presentation::add_rule(p, "kb", "bk");
    presentation::add_rule(p, "lb", "bl");
    presentation::add_rule(p, "mb", "bm");
    presentation::add_rule(p, "nb", "bn");
    presentation::add_rule(p, "ob", "bo");
    presentation::add_rule(p, "dcd", "cdc");
    presentation::add_rule(p, "ec", "ce");
    presentation::add_rule(p, "fc", "cf");
    presentation::add_rule(p, "gc", "cg");
    presentation::add_rule(p, "hc", "ch");
    presentation::add_rule(p, "ic", "ci");
    presentation::add_rule(p, "jc", "cj");
    presentation::add_rule(p, "kc", "ck");
    presentation::add_rule(p, "lc", "cl");
    presentation::add_rule(p, "mc", "cm");
    presentation::add_rule(p, "nc", "cn");
    presentation::add_rule(p, "oc", "co");
    presentation::add_rule(p, "ede", "ded");
    presentation::add_rule(p, "fd", "df");
    presentation::add_rule(p, "gd", "dg");
    presentation::add_rule(p, "hd", "dh");
    presentation::add_rule(p, "id", "di");
    presentation::add_rule(p, "jd", "dj");
    presentation::add_rule(p, "kd", "dk");
    presentation::add_rule(p, "ld", "dl");
    presentation::add_rule(p, "md", "dm");
    presentation::add_rule(p, "nd", "dn");
    presentation::add_rule(p, "od", "do");
    presentation::add_rule(p, "fef", "efe");
    presentation::add_rule(p, "ge", "eg");
    presentation::add_rule(p, "he", "eh");
    presentation::add_rule(p, "ie", "ei");
    presentation::add_rule(p, "je", "ej");
    presentation::add_rule(p, "ke", "ek");
    presentation::add_rule(p, "le", "el");
    presentation::add_rule(p, "me", "em");
    presentation::add_rule(p, "ne", "en");
    presentation::add_rule(p, "oe", "eo");
    presentation::add_rule(p, "gfg", "fgf");
    presentation::add_rule(p, "hf", "fh");
    presentation::add_rule(p, "if", "fi");
    presentation::add_rule(p, "jf", "fj");
    presentation::add_rule(p, "kf", "fk");
    presentation::add_rule(p, "lf", "fl");
    presentation::add_rule(p, "mf", "fm");
    presentation::add_rule(p, "nf", "fn");
    presentation::add_rule(p, "of", "fo");
    presentation::add_rule(p, "hgh", "ghg");
    presentation::add_rule(p, "ig", "gi");
    presentation::add_rule(p, "jg", "gj");
    presentation::add_rule(p, "kg", "gk");
    presentation::add_rule(p, "lg", "gl");
    presentation::add_rule(p, "mg", "gm");
    presentation::add_rule(p, "ng", "gn");
    presentation::add_rule(p, "og", "go");
    presentation::add_rule(p, "ihi", "hih");
    presentation::add_rule(p, "jh", "hj");
    presentation::add_rule(p, "kh", "hk");
    presentation::add_rule(p, "lh", "hl");
    presentation::add_rule(p, "mh", "hm");
    presentation::add_rule(p, "nh", "hn");
    presentation::add_rule(p, "oh", "ho");
    presentation::add_rule(p, "jij", "iji");
    presentation::add_rule(p, "ki", "ik");
    presentation::add_rule(p, "li", "il");
    presentation::add_rule(p, "mi", "im");
    presentation::add_rule(p, "ni", "in");
    presentation::add_rule(p, "oi", "io");
    presentation::add_rule(p, "kjk", "jkj");
    presentation::add_rule(p, "lj", "jl");
    presentation::add_rule(p, "mj", "jm");
    presentation::add_rule(p, "nj", "jn");
    presentation::add_rule(p, "oj", "jo");
    presentation::add_rule(p, "lkl", "klk");
    presentation::add_rule(p, "mk", "km");
    presentation::add_rule(p, "nk", "kn");
    presentation::add_rule(p, "ok", "ko");
    presentation::add_rule(p, "mlm", "lml");
    presentation::add_rule(p, "nl", "ln");
    presentation::add_rule(p, "ol", "lo");
    presentation::add_rule(p, "nmn", "mnm");
    presentation::add_rule(p, "om", "mo");
    presentation::add_rule(p, "ono", "non");
    KnuthBendix<std::string, TestType> kb(twosided, p);

    REQUIRE(!kb.rewriting_system().confluent());

    kb.run();  // faster
    REQUIRE(kb.rewriting_system().confluent());
    REQUIRE(kb.rewriting_system().number_of_rules()
            == 211);  // verified with KBMAG

    using order = typename TestType::reduction_order;
    if constexpr (std::is_same_v<order, LenLexCmp>) {
      REQUIRE(kb.gilman_graph().number_of_nodes() == 121);
      auto g = kb.gilman_graph_node_labels();
      std::sort(g.begin(), g.end(), [](std::string x, std::string y) {
        return lenlex_cmp(x, y);
      });
      REQUIRE(g
              == std::vector<std::string>({"",
                                           "a",
                                           "b",
                                           "c",
                                           "d",
                                           "e",
                                           "f",
                                           "g",
                                           "h",
                                           "i",
                                           "j",
                                           "k",
                                           "l",
                                           "m",
                                           "n",
                                           "o",
                                           "ba",
                                           "cb",
                                           "dc",
                                           "ed",
                                           "fe",
                                           "gf",
                                           "hg",
                                           "ih",
                                           "ji",
                                           "kj",
                                           "lk",
                                           "ml",
                                           "nm",
                                           "on",
                                           "cba",
                                           "dcb",
                                           "edc",
                                           "fed",
                                           "gfe",
                                           "hgf",
                                           "ihg",
                                           "jih",
                                           "kji",
                                           "lkj",
                                           "mlk",
                                           "nml",
                                           "onm",
                                           "dcba",
                                           "edcb",
                                           "fedc",
                                           "gfed",
                                           "hgfe",
                                           "ihgf",
                                           "jihg",
                                           "kjih",
                                           "lkji",
                                           "mlkj",
                                           "nmlk",
                                           "onml",
                                           "edcba",
                                           "fedcb",
                                           "gfedc",
                                           "hgfed",
                                           "ihgfe",
                                           "jihgf",
                                           "kjihg",
                                           "lkjih",
                                           "mlkji",
                                           "nmlkj",
                                           "onmlk",
                                           "fedcba",
                                           "gfedcb",
                                           "hgfedc",
                                           "ihgfed",
                                           "jihgfe",
                                           "kjihgf",
                                           "lkjihg",
                                           "mlkjih",
                                           "nmlkji",
                                           "onmlkj",
                                           "gfedcba",
                                           "hgfedcb",
                                           "ihgfedc",
                                           "jihgfed",
                                           "kjihgfe",
                                           "lkjihgf",
                                           "mlkjihg",
                                           "nmlkjih",
                                           "onmlkji",
                                           "hgfedcba",
                                           "ihgfedcb",
                                           "jihgfedc",
                                           "kjihgfed",
                                           "lkjihgfe",
                                           "mlkjihgf",
                                           "nmlkjihg",
                                           "onmlkjih",
                                           "ihgfedcba",
                                           "jihgfedcb",
                                           "kjihgfedc",
                                           "lkjihgfed",
                                           "mlkjihgfe",
                                           "nmlkjihgf",
                                           "onmlkjihg",
                                           "jihgfedcba",
                                           "kjihgfedcb",
                                           "lkjihgfedc",
                                           "mlkjihgfed",
                                           "nmlkjihgfe",
                                           "onmlkjihgf",
                                           "kjihgfedcba",
                                           "lkjihgfedcb",
                                           "mlkjihgfedc",
                                           "nmlkjihgfed",
                                           "onmlkjihgfe",
                                           "lkjihgfedcba",
                                           "mlkjihgfedcb",
                                           "nmlkjihgfedc",
                                           "onmlkjihgfed",
                                           "mlkjihgfedcba",
                                           "nmlkjihgfedcb",
                                           "onmlkjihgfedc",
                                           "nmlkjihgfedcba",
                                           "onmlkjihgfedcb",
                                           "onmlkjihgfedcba"}));
      REQUIRE(kb.gilman_graph().number_of_edges() == 680);

      // verified with KBMAG
      auto nf = knuth_bendix::normal_forms(kb).min(0).max(6);
      REQUIRE(nf.count() == 49'436);

      // verified with KBMAG
      REQUIRE(nf.min(0).max(10).count() == 2'554'607);
      REQUIRE(std::is_same_v<decltype(nf.size_hint()), uint64_t>);
      REQUIRE(nf.max(POSITIVE_INFINITY).size_hint() == 20'922'789'888'000);
    }
    REQUIRE(kb.number_of_classes() == 20'922'789'888'000);
  }

  // Presentation of group A_4 regarded as monoid presentation - gives
  // infinite monoid.
  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "033",
                                   "kbmag/standalone/kb_data/a4monoid",
                                   "[quick][knuth-bendix][kbmag]",
                                   REWRITING_SYSTEM_TYPES) {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("abB");

    presentation::add_rule(p, "bb", "B");
    presentation::add_rule(p, "BaB", "aba");

    KnuthBendix<std::string, TestType> kb(twosided, p);
    REQUIRE(!kb.rewriting_system().confluent());

    kb.run();
    REQUIRE(kb.rewriting_system().confluent());

    REQUIRE(knuth_bendix::contains(kb, "bb", "B"));
    REQUIRE(knuth_bendix::contains(kb, "BaB", "aba"));
    REQUIRE(knuth_bendix::contains(kb, "Bb", "bB"));
    REQUIRE(knuth_bendix::contains(kb, "Baaba", "abaaB"));
    REQUIRE(knuth_bendix::contains(kb, "BabB", "abab"));
    REQUIRE(knuth_bendix::contains(kb, "Bababa", "ababaB"));

    using order     = typename TestType::reduction_order;
    using rule_type = typename decltype(kb)::rule_type;

    if constexpr (std::is_same_v<order, LenLexCmp>) {
      REQUIRE(kb.rewriting_system().number_of_rules() == 6);
      REQUIRE((kb.active_rules() | sort(weird_cmp()) | to_vector())
              == std::vector<rule_type>({{{"Bb", "bB"},
                                          {"bb", "B"},
                                          {"BaB", "aba"},
                                          {"BabB", "abab"},
                                          {"Baaba", "abaaB"},
                                          {"Bababa", "ababaB"}}}));
    } else if (std::is_same_v<order, RevRPOCmp>) {
      REQUIRE(kb.rewriting_system().number_of_rules() == 4);
      REQUIRE((kb.active_rules() | sort(weird_cmp()) | to_vector())
              == std::vector<rule_type>({{"B", "bb"},
                                         {"bbabb", "aba"},
                                         {"abaabb", "bbaaba"},
                                         {"abababb", "bbababa"}}));
    }
    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
  }

  // fairly clearly the trivial group
  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "034",
                                   "kbmag/standalone/kb_data/degen3",
                                   "[quick][knuth-bendix][kbmag][no-valgrind]",
                                   REWRITING_SYSTEM_TYPES) {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("aAbB");
    p.contains_empty_word(true);
    presentation::add_rule(p, "ab", "");
    presentation::add_rule(p, "abb", "");

    KnuthBendix<std::string, TestType> kb(twosided, p);
    // kb.process_pending_rules();
    using rule_type = typename decltype(kb)::rule_type;
    REQUIRE((kb.active_rules() | sort(weird_cmp()) | to_vector())
            == std::vector<rule_type>({{"a", ""}, {"b", ""}}));
    REQUIRE(kb.rewriting_system().number_of_rules() == 2);
    REQUIRE(kb.rewriting_system().confluent());

    kb.run();
    REQUIRE(kb.rewriting_system().confluent());
    REQUIRE(kb.rewriting_system().number_of_rules() == 2);

    REQUIRE(knuth_bendix::contains(kb, "b", ""));
    REQUIRE(knuth_bendix::contains(kb, "a", ""));
    using rule_type = typename decltype(kb)::rule_type;
    REQUIRE((kb.active_rules() | sort(weird_cmp()) | to_vector())
            == std::vector<rule_type>({{"a", ""}, {"b", ""}}));
  }

  // infinite cyclic group
  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "035",
                                   "kbmag/standalone/kb_data/ab1",
                                   "[quick][knuth-bendix][kbmag]",
                                   REWRITING_SYSTEM_TYPES) {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("aA");
    p.contains_empty_word(true);
    presentation::add_inverse_rules(p, "Aa");

    KnuthBendix<std::string, TestType> kb(twosided, p);
    // kb.process_pending_rules();
    REQUIRE(kb.rewriting_system().confluent());

    kb.run();
    REQUIRE(kb.rewriting_system().confluent());
    REQUIRE(kb.rewriting_system().number_of_rules() == 2);
    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
  }

  // A generator, but trivial.
  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "036",
                                   "kbmag/standalone/kb_data/degen2",
                                   "[quick][knuth-bendix][kbmag]",
                                   REWRITING_SYSTEM_TYPES) {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("aA");
    p.contains_empty_word(true);
    presentation::add_rule(p, "a", "");

    KnuthBendix<std::string, TestType> kb(twosided, p);
    // kb.process_pending_rules();
    REQUIRE(kb.rewriting_system().confluent());

    kb.run();
    REQUIRE(kb.rewriting_system().confluent());
    REQUIRE(kb.rewriting_system().number_of_rules() == 1);

    REQUIRE(knuth_bendix::contains(kb, "a", ""));
    using rule_type = typename decltype(kb)::rule_type;
    REQUIRE((kb.active_rules() | sort(weird_cmp()) | to_vector())
            == std::vector<rule_type>({{"a", ""}}));
  }

  // Fibonacci group F(2,5)
  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "037",
                                   "kbmag/standalone/kb_data/f25",
                                   "[quick][knuth-bendix][kbmag]",
                                   REWRITING_SYSTEM_TYPES) {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("aAbBcCdDyY");

    presentation::add_rule(p, "ab", "c");
    presentation::add_rule(p, "bc", "d");
    presentation::add_rule(p, "cd", "y");
    presentation::add_rule(p, "dy", "a");
    presentation::add_rule(p, "ya", "b");

    KnuthBendix<std::string, TestType> kb(twosided, p);
    REQUIRE(!kb.rewriting_system().confluent());

    kb.run();
    REQUIRE(kb.rewriting_system().confluent());

    REQUIRE(knuth_bendix::contains(kb, "ab", "c"));
    REQUIRE(knuth_bendix::contains(kb, "bc", "d"));
    REQUIRE(knuth_bendix::contains(kb, "cd", "y"));
    REQUIRE(knuth_bendix::contains(kb, "dy", "a"));
    REQUIRE(knuth_bendix::contains(kb, "ya", "b"));
    REQUIRE(knuth_bendix::contains(kb, "cc", "ad"));
    REQUIRE(knuth_bendix::contains(kb, "dd", "by"));
    REQUIRE(knuth_bendix::contains(kb, "yy", "ac"));
    REQUIRE(knuth_bendix::contains(kb, "yc", "bb"));
    REQUIRE(knuth_bendix::contains(kb, "db", "aa"));
    REQUIRE(knuth_bendix::contains(kb, "aac", "by"));
    REQUIRE(knuth_bendix::contains(kb, "bd", "aa"));
    REQUIRE(knuth_bendix::contains(kb, "bby", "aad"));
    REQUIRE(knuth_bendix::contains(kb, "aaa", "y"));
    REQUIRE(knuth_bendix::contains(kb, "yb", "by"));
    REQUIRE(knuth_bendix::contains(kb, "ba", "c"));
    REQUIRE(knuth_bendix::contains(kb, "da", "ad"));
    REQUIRE(knuth_bendix::contains(kb, "ca", "ac"));
    REQUIRE(knuth_bendix::contains(kb, "cy", "bb"));
    REQUIRE(knuth_bendix::contains(kb, "cb", "d"));
    REQUIRE(knuth_bendix::contains(kb, "yd", "a"));
    REQUIRE(knuth_bendix::contains(kb, "dc", "y"));
    REQUIRE(knuth_bendix::contains(kb, "ay", "b"));
    REQUIRE(knuth_bendix::contains(kb, "bbb", "a"));

    using rule_type = typename decltype(kb)::rule_type;

    using order = typename TestType::reduction_order;
    if constexpr (std::is_same_v<order, LenLexCmp>) {
      REQUIRE(kb.rewriting_system().number_of_rules() == 24);

      REQUIRE((kb.active_rules() | sort(weird_cmp()) | to_vector())
              == std::vector<rule_type>(
                  {{"ab", "c"},  {"ay", "b"},   {"ba", "c"},  {"bc", "d"},
                   {"bd", "aa"}, {"ca", "ac"},  {"cb", "d"},  {"cc", "ad"},
                   {"cd", "y"},  {"cy", "bb"},  {"da", "ad"}, {"db", "aa"},
                   {"dc", "y"},  {"dd", "by"},  {"dy", "a"},  {"ya", "b"},
                   {"yb", "by"}, {"yc", "bb"},  {"yd", "a"},  {"yy", "ac"},
                   {"aaa", "y"}, {"aac", "by"}, {"bbb", "a"}, {"bby", "aad"}}));
    } else if (std::is_same_v<order, RevRPOCmp>) {
      REQUIRE(kb.rewriting_system().number_of_rules() == 5);
      REQUIRE((kb.active_rules() | sort(weird_cmp()) | to_vector())
              == std::vector<rule_type>({{"b", "aaaa"},
                                         {"c", "aaaaa"},
                                         {"d", "aaaaaaaaa"},
                                         {"y", "aaa"},
                                         {"aaaaaaaaaaaa", "a"}}));
    }
  }

  // Von Dyck (2,3,7) group - infinite hyperbolic
  // both RPOTrie + RPOSet very slow here
  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "038",
                                   "kbmag/standalone/kb_data/237",
                                   "[quick][knuth-bendix][kbmag]",
                                   LenLexSet,
                                   LenLexTrie) {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("aAbBc");
    p.contains_empty_word(true);

    presentation::add_inverse_rules(p, "AaBbc");

    presentation::add_rule(p, "aaaa", "AAA");
    presentation::add_rule(p, "bb", "B");
    presentation::add_rule(p, "BA", "c");

    KnuthBendix<std::string, TestType> kb(twosided, p);
    REQUIRE(!kb.rewriting_system().confluent());

    kb.run();
    REQUIRE(kb.rewriting_system().confluent());
    REQUIRE(kb.rewriting_system().number_of_rules() == 32);
    using rule_type = typename decltype(kb)::rule_type;
    REQUIRE((kb.active_rules() | sort(weird_cmp()) | to_vector())
            == std::vector<rule_type>({{"Aa", ""},
                                       {"Ac", "b"},
                                       {"BA", "c"},
                                       {"BB", "b"},
                                       {"Bb", ""},
                                       {"Bc", "bA"},
                                       {"aA", ""},
                                       {"ab", "c"},
                                       {"bB", ""},
                                       {"ba", "AB"},
                                       {"bb", "B"},
                                       {"bc", "A"},
                                       {"cB", "a"},
                                       {"ca", "B"},
                                       {"cb", "aB"},
                                       {"cc", ""},
                                       {"BaB", "bAb"},
                                       {"bAB", "Ba"},
                                       {"cAB", "aBa"},
                                       {"AAAA", "aaa"},
                                       {"AAAb", "aaac"},
                                       {"aaaa", "AAA"},
                                       {"bAbA", "Bac"},
                                       {"cAAA", "Baaa"},
                                       {"cAbA", "aBac"},
                                       {"ABaaa", "bAAA"},
                                       {"Baaac", "cAAb"},
                                       {"bAABaac", "BacAAb"},
                                       {"cAABaac", "aBacAAb"},
                                       {"BaaaBaaa", "cAAbAAA"},
                                       {"bAABaaBaaa", "BacAAbAAA"},
                                       {"cAABaaBaaa", "aBacAAbAAA"}}));
    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
  }

  // Cyclic group of order 2.
  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "039",
                                   "kbmag/standalone/kb_data/c2",
                                   "[quick][knuth-bendix][kbmag]",
                                   REWRITING_SYSTEM_TYPES) {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("a");
    p.contains_empty_word(true);
    presentation::add_rule(p, "aa", "");

    KnuthBendix<std::string, TestType> kb(twosided, p);
    // kb.process_pending_rules();
    REQUIRE(kb.rewriting_system().confluent());

    kb.run();
    REQUIRE(kb.rewriting_system().confluent());
    REQUIRE(kb.rewriting_system().number_of_rules() == 1);

    using rule_type = typename decltype(kb)::rule_type;
    REQUIRE((kb.active_rules() | sort(weird_cmp()) | to_vector())
            == std::vector<rule_type>({{"aa", ""}}));
  }

  // The group is S_4, and the subgroup H of order 4. There are 30 reduced
  // words - 24 for the group elements, and 6 for the 6 cosets Hg.
  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "040",
                                   "kbmag/standalone/kb_data/cosets",
                                   "[quick][knuth-bendix][kbmag]",
                                   REWRITING_SYSTEM_TYPES) {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.contains_empty_word(true);
    p.alphabet("HaAbB");

    presentation::add_rule(p, "aaa", "");
    presentation::add_rule(p, "bbbb", "");
    presentation::add_rule(p, "abab", "");
    presentation::add_rule(p, "Hb", "H");
    presentation::add_rule(p, "HH", "H");
    presentation::add_rule(p, "aH", "H");
    presentation::add_rule(p, "bH", "H");

    KnuthBendix<std::string, TestType> kb(twosided, p);
    REQUIRE(!kb.rewriting_system().confluent());

    kb.run();
    REQUIRE(kb.rewriting_system().confluent());

    REQUIRE(knuth_bendix::contains(kb, "aaa", ""));
    REQUIRE(knuth_bendix::contains(kb, "Hb", "H"));
    REQUIRE(knuth_bendix::contains(kb, "HH", "H"));
    REQUIRE(knuth_bendix::contains(kb, "aH", "H"));
    REQUIRE(knuth_bendix::contains(kb, "bH", "H"));
    REQUIRE(knuth_bendix::contains(kb, "bab", "aa"));
    REQUIRE(knuth_bendix::contains(kb, "bbb", "aba"));
    REQUIRE(knuth_bendix::contains(kb, "Hab", "Haa"));
    REQUIRE(knuth_bendix::contains(kb, "abaab", "bbaa"));
    REQUIRE(knuth_bendix::contains(kb, "baaba", "aabb"));
    REQUIRE(knuth_bendix::contains(kb, "Haabb", "Haaba"));
    REQUIRE(knuth_bendix::contains(kb, "bbaabb", "abba"));
    REQUIRE(knuth_bendix::contains(kb, "aabbaa", "baab"));
    REQUIRE(knuth_bendix::contains(kb, "baabba", "abbaab"));

    using rule_type = typename decltype(kb)::rule_type;

    using order = typename TestType::reduction_order;
    if constexpr (std::is_same_v<order, LenLexCmp>) {
      REQUIRE(kb.rewriting_system().number_of_rules() == 14);
      REQUIRE((kb.active_rules() | sort(weird_cmp()) | to_vector())
              == std::vector<rule_type>({{{"HH", "H"},
                                          {"Hb", "H"},
                                          {"aH", "H"},
                                          {"bH", "H"},
                                          {"Hab", "Haa"},
                                          {"aaa", ""},
                                          {"bab", "aa"},
                                          {"bbb", "aba"},
                                          {"Haabb", "Haaba"},
                                          {"abaab", "bbaa"},
                                          {"baaba", "aabb"},
                                          {"aabbaa", "baab"},
                                          {"baabba", "abbaab"},
                                          {"bbaabb", "abba"}}}));
    } else if (std::is_same_v<order, RevRPOCmp>) {
      REQUIRE(kb.rewriting_system().number_of_rules() == 11);
      REQUIRE((kb.active_rules() | sort(weird_cmp()) | to_vector())
              == std::vector<rule_type>({{"HH", "H"},
                                         {"Hb", "H"},
                                         {"aH", "H"},
                                         {"bH", "H"},
                                         {"Hab", "Haa"},
                                         {"aaa", ""},
                                         {"bab", "aa"},
                                         {"bbb", "aba"},
                                         {"aabb", "baaba"},
                                         {"abaab", "bbaa"},
                                         {"abbaab", "bbaabaa"}}));
    }
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "041",
                                   "Ex. 5.1 in Sims (KnuthBendix 09 again)",
                                   "[quick][knuth-bendix]",
                                   REWRITING_SYSTEM_TYPES) {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("aAbB");
    p.contains_empty_word(true);

    presentation::add_rule(p, "aA", "");
    presentation::add_rule(p, "Aa", "");
    presentation::add_rule(p, "bB", "");
    presentation::add_rule(p, "Bb", "");
    presentation::add_rule(p, "ba", "ab");

    KnuthBendix<std::string, TestType> kb(twosided, p);
    REQUIRE(!kb.rewriting_system().confluent());

    kb.run();
    REQUIRE(kb.rewriting_system().number_of_rules() == 8);
    REQUIRE(kb.rewriting_system().confluent());
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "042",
                                   "kbmag/standalone/kb_data/nilp2",
                                   "[quick][knuth-bendix][kbmag]",
                                   REWRITING_SYSTEM_TYPES) {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("cCbBaA");
    p.contains_empty_word(true);

    presentation::add_inverse_rules(p, "CcBbAa");

    presentation::add_rule(p, "ba", "abc");
    presentation::add_rule(p, "ca", "ac");
    presentation::add_rule(p, "cb", "bc");

    KnuthBendix<std::string, TestType> kb(twosided, p);
    REQUIRE(!kb.rewriting_system().confluent());
  }

  // This test checks some specific things related to lenlex so don't do RPO
  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "043",
                                   "Ex. 6.4 in Sims",
                                   "[quick][knuth-bendix][no-valgrind]",
                                   LenLexSet,
                                   LenLexTrie) {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.contains_empty_word(true);
    p.alphabet("abc");
    presentation::add_rule(p, "aa", "");
    presentation::add_rule(p, "bc", "");
    presentation::add_rule(p, "bbb", "");
    presentation::add_rule(p, "ababababababab", "");
    presentation::add_rule(p, "abacabacabacabac", "");

    KnuthBendix<std::string, TestType> kb(twosided, p);
    REQUIRE(kb.rewriting_system().number_of_rules() == 5);
    REQUIRE(!kb.rewriting_system().confluent());
    kb.run();
    REQUIRE(kb.rewriting_system().confluent());
    REQUIRE(kb.rewriting_system().number_of_rules() == 40);

    kb.init(twosided, p);
    kb.max_rules(10);
    kb.run();
    REQUIRE(kb.rewriting_system().number_of_rules() > 10);
    REQUIRE(!kb.rewriting_system().confluent());

    kb.run();
    REQUIRE(kb.rewriting_system().number_of_rules() > 10);
    REQUIRE(!kb.rewriting_system().confluent());

    kb.max_rules(20);
    kb.run();
    REQUIRE(kb.rewriting_system().number_of_rules() > 20);
    REQUIRE(!kb.rewriting_system().confluent());

    kb.max_rules(LIMIT_MAX);
    kb.run();
    REQUIRE(kb.rewriting_system().confluent());

    REQUIRE(kb.rewriting_system().number_of_rules() == 40);
  }

  // Von Dyck (2,3,7) group - infinite hyperbolic
  // at least RPOSet very slow here
  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "044",
                                   "KnuthBendix 071 again",
                                   "[no-valgrind][quick][knuth-bendix]",
                                   LenLexSet,
                                   LenLexTrie) {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("aAbBc");
    p.contains_empty_word(true);

    presentation::add_inverse_rules(p, "AaBbc");

    presentation::add_rule(p, "BA", "c");
    presentation::add_rule(p, "Bb", "bB");
    presentation::add_rule(p, "bb", "B");
    presentation::add_rule(p, "AAAa", "aAAA");
    presentation::add_rule(p, "aaaa", "AAA");
    presentation::add_rule(p, "BaAAA", "cAAa");
    presentation::add_rule(p, "BaaAAA", "cAAaa");
    presentation::add_rule(p, "BaAaAAA", "cAAaAa");
    presentation::add_rule(p, "BaaaAAA", "cAAaaa");
    presentation::add_rule(p, "BaAAaAAA", "cAAaAAa");
    presentation::add_rule(p, "BaAaaAAA", "cAAaAaa");
    presentation::add_rule(p, "BaaAaAAA", "cAAaaAa");
    presentation::add_rule(p, "BaAAaaAAA", "cAAaAAaa");
    presentation::add_rule(p, "BaAaAaAAA", "cAAaAaAa");
    presentation::add_rule(p, "BaAaaaAAA", "cAAaAaaa");
    presentation::add_rule(p, "BaaAAaAAA", "cAAaaAAa");
    presentation::add_rule(p, "BaaAaaAAA", "cAAaaAaa");
    presentation::add_rule(p, "BaAAaAaAAA", "cAAaAAaAa");
    presentation::add_rule(p, "BaAAaaaAAA", "cAAaAAaaa");
    presentation::add_rule(p, "BaAaAAaAAA", "cAAaAaAAa");
    presentation::add_rule(p, "BaAaAaaAAA", "cAAaAaAaa");
    presentation::add_rule(p, "BaAaaAaAAA", "cAAaAaaAa");
    presentation::add_rule(p, "BaaAAaaAAA", "cAAaaAAaa");
    presentation::add_rule(p, "BaaAaAaAAA", "cAAaaAaAa");
    presentation::add_rule(p, "BaAAaAAaAAA", "cAAaAAaAAa");
    presentation::add_rule(p, "BaAAaAaaAAA", "cAAaAAaAaa");
    presentation::add_rule(p, "BaAAaaAaAAA", "cAAaAAaaAa");
    presentation::add_rule(p, "BaAaAAaaAAA", "cAAaAaAAaa");
    presentation::add_rule(p, "BaAaAaAaAAA", "cAAaAaAaAa");
    presentation::add_rule(p, "BaAaaAAaAAA", "cAAaAaaAAa");
    presentation::add_rule(p, "BaaAAaAaAAA", "cAAaaAAaAa");
    presentation::add_rule(p, "BaaAaAAaAAA", "cAAaaAaAAa");
    presentation::add_rule(p, "BaAAaAAaaAAA", "cAAaAAaAAaa");
    presentation::add_rule(p, "BaAAaAaAaAAA", "cAAaAAaAaAa");
    presentation::add_rule(p, "BaAAaaAAaAAA", "cAAaAAaaAAa");
    presentation::add_rule(p, "BaAaAAaAaAAA", "cAAaAaAAaAa");
    presentation::add_rule(p, "BaAaAaAAaAAA", "cAAaAaAaAAa");
    presentation::add_rule(p, "BaaAAaAAaAAA", "cAAaaAAaAAa");
    presentation::add_rule(p, "BaAAaAAaAaAAA", "cAAaAAaAAaAa");
    presentation::add_rule(p, "BaAAaAaAAaAAA", "cAAaAAaAaAAa");
    presentation::add_rule(p, "BaAaAAaAAaAAA", "cAAaAaAAaAAa");
    presentation::add_rule(p, "BaAAaAAaAAaAAA", "cAAaAAaAAaAAa");

    KnuthBendix<std::string, TestType> kb(twosided, p);

    REQUIRE(kb.rewriting_system().number_of_rules() == 47);
    REQUIRE(!kb.rewriting_system().confluent());
    kb.run();
    REQUIRE(kb.rewriting_system().confluent());
    REQUIRE(kb.rewriting_system().number_of_rules() == 32);
    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
    auto nf = knuth_bendix::normal_forms(kb);

    REQUIRE(nf.min(4).max(4).count() == 24);
    nf.min(4);
    REQUIRE((nf | to_vector())
            == std::vector<std::string>({"aaaB", "aaac", "aaBa", "aacA", "aBaa",
                                         "aBac", "acAA", "acAb", "AAAB", "AAbA",
                                         "AABa", "AbAA", "AbAb", "ABaa", "ABac",
                                         "bAAA", "bAAb", "bAAB", "Baaa", "BaaB",
                                         "Baac", "BacA", "cAAb", "cAAB"}));
  }

  // No rpo specific to LenLex
  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "045",
                                   "Sims Ex. 5.4 - alt. overlap policy",
                                   "[quick][knuth-bendix]",
                                   LenLexTrie,
                                   LenLexSet) {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("Bab");
    p.contains_empty_word(true);
    presentation::add_rule(p, "aa", "");
    presentation::add_rule(p, "bB", "");
    presentation::add_rule(p, "bbb", "");
    presentation::add_rule(p, "ababab", "");

    KnuthBendix<std::string, TestType> kb(twosided, p);
    kb.overlap_policy(
        KnuthBendix<std::string, TestType>::options::overlap::AB_BC);

    REQUIRE(!kb.rewriting_system().confluent());

    knuth_bendix::by_overlap_length(kb);
    REQUIRE(kb.rewriting_system().confluent());
    REQUIRE(kb.rewriting_system().number_of_rules() == 11);
    REQUIRE(kb.rewriting_system().confluent());
    REQUIRE(kb.number_of_classes() == 12);
    auto nf1 = knuth_bendix::normal_forms(kb).min(4).max(4);
    REQUIRE(nf1.count() == 0);

    REQUIRE((knuth_bendix::normal_forms(kb) | to_vector())
            == std::vector<std::string>({"",
                                         "B",
                                         "a",
                                         "b",
                                         "Ba",
                                         "aB",
                                         "ab",
                                         "ba",
                                         "BaB",
                                         "Bab",
                                         "aBa",
                                         "baB"}));
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "046",
                                   "Sims - Ex. 5.4 - alt. overlap policy",
                                   "[quick][knuth-bendix]",
                                   REWRITING_SYSTEM_TYPES) {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.contains_empty_word(true);
    p.alphabet("Bab");
    presentation::add_rule(p, "aa", "");
    presentation::add_rule(p, "bB", "");
    presentation::add_rule(p, "bbb", "");
    presentation::add_rule(p, "ababab", "");

    KnuthBendix<std::string, TestType> kb(twosided, p);
    kb.overlap_policy(
        KnuthBendix<std::string, TestType>::options::overlap::MAX_AB_BC);
    // The next line tests that we don't delete
    // the old OverlapMeasure.
    kb.overlap_policy(
        KnuthBendix<std::string, TestType>::options::overlap::MAX_AB_BC);

    REQUIRE(!kb.rewriting_system().confluent());

    knuth_bendix::by_overlap_length(kb);
    using order = typename TestType::reduction_order;
    if constexpr (std::is_same_v<order, LenLexCmp>) {
      REQUIRE(kb.rewriting_system().number_of_rules() == 11);
    } else if (std::is_same_v<order, RevRPOCmp>) {
      REQUIRE(kb.rewriting_system().number_of_rules() == 5);
    }
    REQUIRE(kb.rewriting_system().confluent());
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "047",
                                   "operator<<",
                                   "[quick][knuth-bendix]",
                                   REWRITING_SYSTEM_TYPES) {
    std::ostringstream os;

    Presentation<std::string> p;
    p.alphabet("Bab");
    p.contains_empty_word(true);
    presentation::add_rule(p, "aa", "");
    presentation::add_rule(p, "bB", "");
    presentation::add_rule(p, "bbb", "");
    presentation::add_rule(p, "ababab", "");

    KnuthBendix<std::string, TestType> kb1(twosided, p);
    os << kb1;  // Does not do anything visible
    p.alphabet("cbaB");
    presentation::add_rule(p, "aa", "");
    presentation::add_rule(p, "bB", "");
    presentation::add_rule(p, "bbb", "");
    presentation::add_rule(p, "ababab", "");
    KnuthBendix<std::string, TestType> kb2(twosided, p);
    os << kb2;  // Does not do anything visible
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "049",
                                   "max_overlap",
                                   "[quick][knuth-bendix]",
                                   REWRITING_SYSTEM_TYPES) {
    Presentation<std::string> p;
    p.contains_empty_word(true);
    p.alphabet("Bab");

    presentation::add_rule(p, "aa", "");
    presentation::add_rule(p, "bB", "");
    presentation::add_rule(p, "bbb", "");
    presentation::add_rule(p, "ababab", "");

    KnuthBendix<std::string, TestType> kb(twosided, p);
    kb.max_overlap(10);
    kb.max_overlap(-11);
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "050",
                                   "kbmag/standalone/kb_data/d22",
                                   "[quick][knuth-bendix][fpsemi][kbmag]",
                                   REWRITING_SYSTEM_TYPES) {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("ABCDYFabcdyf");
    p.contains_empty_word(true);

    presentation::add_inverse_rules(p, "abcdyfABCDYF");

    presentation::add_rule(p, "aCAd", "");
    presentation::add_rule(p, "bfBY", "");
    presentation::add_rule(p, "cyCD", "");
    presentation::add_rule(p, "dFDa", "");
    presentation::add_rule(p, "ybYA", "");
    presentation::add_rule(p, "fCFB", "");

    KnuthBendix<std::string, TestType> kb(twosided, p);
    REQUIRE(!kb.rewriting_system().confluent());

    knuth_bendix::by_overlap_length(kb);
    REQUIRE(kb.rewriting_system().confluent());
    REQUIRE(kb.presentation().rules.size() / 2 == 18);
    REQUIRE(kb.number_of_classes() == 22);

    using order = typename TestType::reduction_order;
    if constexpr (std::is_same_v<order, LenLexCmp>) {
      REQUIRE((knuth_bendix::normal_forms(kb) | to_vector())
              == std::vector<std::string>(
                  {"",   "A",   "B",   "C",   "D",   "Y",  "F",  "AB",
                   "AC", "AD",  "AY",  "AF",  "BA",  "BD", "BY", "CY",
                   "DB", "ABA", "ABD", "ABY", "ACY", "ADB"}));
    } else if (std::is_same_v<order, RevRPOCmp>) {
      REQUIRE((knuth_bendix::normal_forms(kb) | to_vector())
              == std::vector<std::string>(
                  {"",           "A",          "B",         "AB",
                   "BA",         "ABA",        "BAB",       "ABAB",
                   "BABA",       "ABABA",      "BABAB",     "ABABAB",
                   "BABABA",     "ABABABA",    "BABABAB",   "ABABABAB",
                   "BABABABA",   "ABABABABA",  "BABABABAB", "ABABABABAB",
                   "BABABABABA", "ABABABABABA"}));
    }
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "051",
                                   "kbmag/standalone/kb_data/d22",
                                   "[quick][knuth-bendix][fpsemi][kbmag]",
                                   REWRITING_SYSTEM_TYPES) {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("aAbBcCdDyYfF");
    p.contains_empty_word(true);

    presentation::add_inverse_rules(p, "AaBbCcDdYyFf");

    presentation::add_rule(p, "aCAd", "");
    presentation::add_rule(p, "bfBY", "");
    presentation::add_rule(p, "cyCD", "");
    presentation::add_rule(p, "dFDa", "");
    presentation::add_rule(p, "ybYA", "");
    presentation::add_rule(p, "fCFB", "");
    KnuthBendix<std::string, TestType> kb(twosided, p);
    REQUIRE(!kb.rewriting_system().confluent());

    knuth_bendix::by_overlap_length(kb);
    REQUIRE(kb.rewriting_system().confluent());
    REQUIRE(kb.presentation().rules.size() / 2 == 18);
    REQUIRE(kb.number_of_classes() == 22);
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "052",
                                   "small example",
                                   "[quick][knuth-bendix]",
                                   REWRITING_SYSTEM_TYPES) {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("ab");
    presentation::add_rule(p, "aaa", "a");
    presentation::add_rule(p, "bbbb", "b");
    presentation::add_rule(p, "ababababab", "aa");
    KnuthBendix<std::string, TestType> kb(twosided, p);
    kb.run();
    REQUIRE(kb.rewriting_system().confluent());
    REQUIRE(kb.number_of_classes() == 243);
    auto nf = knuth_bendix::normal_forms(kb).min(1).max(2);
    REQUIRE((nf | to_vector())
            == std::vector<std::string>({"a", "b", "aa", "ab", "ba", "bb"}));
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "053",
                                   "code coverage",
                                   "[quick]",
                                   REWRITING_SYSTEM_TYPES) {
    KnuthBendix<std::string, TestType> kb1;
    KnuthBendix<std::string, TestType> kb2(kb1);
    REQUIRE(kb1.number_of_classes() == 0);

    Presentation<std::string> p;
    p.alphabet("ab");
    presentation::add_rule(p, "aaa", "a");
    KnuthBendix<std::string, TestType> kb3(twosided, p);
    REQUIRE(kb3.presentation().rules.size() / 2 == 1);
  }

  // RPO very slow here
  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "054",
                                   "small overlap 1",
                                   "[quick]",
                                   LenLexSet,
                                   LenLexTrie) {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("BCA");
    presentation::add_rule(p, "AABC", "ACBA");

    KnuthBendix<std::string, TestType> kb(twosided, p);
    REQUIRE(knuth_bendix::contains(
        kb, "CBAABCABCAABCAABCABC", "CBACBAABCAABCACBACBA"));
    REQUIRE(knuth_bendix::contains(
        kb, "CBAABCABCAABCAABCABC", "CBACBAABCAABCACBACBA"));
    REQUIRE(knuth_bendix::contains(kb,
                                   "AABCAABCCACAACBBCBCCACBBAABCBA",
                                   "ACBAACBACACAACBBCBCCACBBACBABA"));
    REQUIRE(knuth_bendix::contains(kb,
                                   "CACCBABACCBABACCAAAABCAABCBCAA",
                                   "CACCBABACCBABACCAAACBAACBABCAA"));
    REQUIRE(knuth_bendix::contains(kb,
                                   "CAAACAABCCBABCCBCCBCACABACBBAC",
                                   "CAAACACBACBABCCBCCBCACABACBBAC"));
    REQUIRE(knuth_bendix::contains(kb,
                                   "BABCACBACBCCCCCAACCAAABAABCBCC",
                                   "BABCACBACBCCCCCAACCAAABACBABCC"));
    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
  }

  // Symmetric group S_9
  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "055",
                                   "kbmag/standalone/kb_data/s9",
                                   "[quick][knuth-bendix][kbmag]",
                                   REWRITING_SYSTEM_TYPES) {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("abcdefgh");
    p.contains_empty_word(true);

    presentation::add_inverse_rules(p, "abcdefgh");

    presentation::add_rule(p, "bab", "aba");
    presentation::add_rule(p, "ca", "ac");
    presentation::add_rule(p, "da", "ad");
    presentation::add_rule(p, "ea", "ae");
    presentation::add_rule(p, "fa", "af");
    presentation::add_rule(p, "ga", "ag");
    presentation::add_rule(p, "ha", "ah");
    presentation::add_rule(p, "cbc", "bcb");
    presentation::add_rule(p, "db", "bd");
    presentation::add_rule(p, "eb", "be");
    presentation::add_rule(p, "fb", "bf");
    presentation::add_rule(p, "gb", "bg");
    presentation::add_rule(p, "hb", "bh");
    presentation::add_rule(p, "dcd", "cdc");
    presentation::add_rule(p, "ec", "ce");
    presentation::add_rule(p, "fc", "cf");
    presentation::add_rule(p, "gc", "cg");
    presentation::add_rule(p, "hc", "ch");
    presentation::add_rule(p, "ede", "ded");
    presentation::add_rule(p, "fd", "df");
    presentation::add_rule(p, "gd", "dg");
    presentation::add_rule(p, "hd", "dh");
    presentation::add_rule(p, "fef", "efe");
    presentation::add_rule(p, "ge", "eg");
    presentation::add_rule(p, "he", "eh");
    presentation::add_rule(p, "gfg", "fgf");
    presentation::add_rule(p, "hf", "fh");
    presentation::add_rule(p, "hgh", "ghg");

    KnuthBendix<std::string, TestType> kb(twosided, p);
    REQUIRE(!kb.rewriting_system().confluent());
    kb.run();
    REQUIRE(kb.rewriting_system().confluent());
    REQUIRE(kb.rewriting_system().number_of_rules() == 57);
    REQUIRE(kb.number_of_classes() == 362'880);
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "056",
                                   "C(4) monoid",
                                   "[quick][knuth-bendix]",
                                   REWRITING_SYSTEM_TYPES) {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("abcde");
    presentation::add_rule(p, "bceac", "aeebbc");
    presentation::add_rule(p, "aeebbc", "dabcd");

    KnuthBendix<std::string, TestType> kb(twosided, p);
    kb.run();
    REQUIRE(kb.rewriting_system().confluent());
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "058",
                                   "1-relation hard case x 2",
                                   "[quick][knuth-bendix][no-valgrind]",
                                   REWRITING_SYSTEM_TYPES) {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.contains_empty_word(true);
    p.alphabet("abcd");
    presentation::add_rule(p, "aa", "a");
    presentation::add_rule(p, "ad", "d");
    presentation::add_rule(p, "bb", "b");
    presentation::add_rule(p, "ca", "ac");
    presentation::add_rule(p, "cc", "c");
    presentation::add_rule(p, "da", "d");
    presentation::add_rule(p, "dc", "cd");
    presentation::add_rule(p, "dd", "d");
    presentation::add_rule(p, "aba", "a");
    presentation::add_rule(p, "abd", "d");
    presentation::add_rule(p, "acd", "cd");
    presentation::add_rule(p, "bab", "b");
    presentation::add_rule(p, "bcb", "b");
    presentation::add_rule(p, "bcd", "cd");
    presentation::add_rule(p, "cbc", "c");
    presentation::add_rule(p, "cdb", "cd");
    presentation::add_rule(p, "dba", "d");
    presentation::add_rule(p, "dbd", "d");
    presentation::add_rule(p, "acba", "ac");
    presentation::add_rule(p, "acbd", "cd");
    presentation::add_rule(p, "cbac", "ac");
    auto it = knuth_bendix::redundant_rule(p, std::chrono::milliseconds(100));
    while (it != p.rules.end()) {
      // std::cout << std::endl
      //           << "REMOVING " << *it << " = " << *(it + 1) << std::endl;
      p.rules.erase(it, it + 2);
      it = knuth_bendix::redundant_rule(p, std::chrono::milliseconds(100));
    }
    REQUIRE(p.rules
            == std::vector<std::string>(
                {"aa",  "a", "ad",  "d",  "bb",  "b", "ca",  "ac", "cc",  "c",
                 "da",  "d", "dc",  "cd", "dd",  "d", "aba", "a",  "bab", "b",
                 "bcb", "b", "bcd", "cd", "cbc", "c", "cdb", "cd"}));
    KnuthBendix<std::string, TestType> kb(congruence_kind::twosided, p);
    REQUIRE(kb.number_of_classes() == 24);

    using order = typename TestType::reduction_order;
    if constexpr (std::is_same_v<order, LenLexCmp>) {
      REQUIRE(knuth_bendix::reduce(kb, "dcb") == "cd");
      REQUIRE(knuth_bendix::reduce(kb, "dca") == "cd");
      REQUIRE(knuth_bendix::reduce(kb, "da") == "d");
      REQUIRE(knuth_bendix::reduce(kb, "cda") == "cd");
      REQUIRE(knuth_bendix::reduce(kb, "cdb") == "cd");
      REQUIRE(knuth_bendix::reduce(kb, "cdc") == "cd");
      REQUIRE(knuth_bendix::reduce(kb, "cdd") == "cd");
      REQUIRE(knuth_bendix::reduce(kb, "dad") == "d");
      REQUIRE(!knuth_bendix::contains(kb, "bd", "db"));
      REQUIRE(knuth_bendix::reduce(kb, "bd") == "bd");
      REQUIRE(knuth_bendix::reduce(kb, "db") == "db");
      REQUIRE(knuth_bendix::reduce(kb, "cbdcbd") == "cd");
      REQUIRE(
          (knuth_bendix::normal_forms(kb) | to_vector())
          == std::vector<std::string>(
              {"",    "a",   "b",   "c",   "d",    "ab",   "ac",   "ba",
               "bc",  "bd",  "cb",  "cd",  "db",   "abc",  "acb",  "bac",
               "bdb", "cba", "cbd", "dbc", "bacb", "bdbc", "cbdb", "cbdbc"}));
    }
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "059",
                                   "search for a monoid that might not exist",
                                   "[quick][knuth-bendix]",
                                   REWRITING_SYSTEM_TYPES) {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.contains_empty_word(true);
    p.alphabet("abcde");
    presentation::add_rule(p, "aa", "a");
    presentation::add_rule(p, "ad", "d");
    presentation::add_rule(p, "bb", "b");
    presentation::add_rule(p, "ca", "ac");
    presentation::add_rule(p, "cc", "c");
    presentation::add_rule(p, "da", "d");
    presentation::add_rule(p, "dc", "cd");
    presentation::add_rule(p, "dd", "d");
    presentation::add_rule(p, "aba", "a");
    presentation::add_rule(p, "bab", "b");
    presentation::add_rule(p, "bcb", "b");
    presentation::add_rule(p, "bcd", "cd");
    presentation::add_rule(p, "cbc", "c");
    presentation::add_rule(p, "cdb", "cd");
    presentation::change_alphabet(p, "cbade");

    presentation::add_rule(p, "ea", "ae");
    presentation::add_rule(p, "be", "eb");
    presentation::add_rule(p, "ee", "e");
    presentation::add_rule(p, "cec", "c");
    presentation::add_rule(p, "ece", "e");

    presentation::add_rule(p, "ead", "ad");
    presentation::add_rule(p, "ade", "ad");
    // presentation::add_rule(p, "de", "ed");
    KnuthBendix<std::string, TestType> kb(congruence_kind::twosided, p);
    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
  }

  // Lenlex specific test
  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "062",
                                   "Chinese id monoid",
                                   "[knuth-bendix][quick]",
                                   LenLexSet,
                                   LenLexTrie) {
    auto rg = ReportGuard(false);
    auto n  = 4;
    auto p  = presentation::examples::chinese_monoid(n);
    p.contains_empty_word(true);
    presentation::add_idempotent_rules_no_checks(p, p.alphabet());

    KnuthBendix<std::string, TestType> kb(twosided,
                                          v4::to<Presentation<std::string>>(p));
    kb.run();

    REQUIRE(knuth_bendix::reduce(kb, "cbda") == "bcda");
    REQUIRE(knuth_bendix::reduce(kb, "badc") == "badc");
    REQUIRE(knuth_bendix::reduce(kb, "cadb") == "cadb");
  }

  // TODO(1): Remove no-cygwin and throw correct exception instead
  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "145",
                                   "alphabet limit",
                                   "[knuth-bendix][quick][no-cygwin]",
                                   REWRITING_SYSTEM_TYPES) {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet(256);
    for (auto a : p.alphabet()) {
      for (auto b : p.alphabet()) {
        presentation::add_rule_no_checks(
            p, std::string(1, a) + std::string(1, b), std::string(1, a));
      }
    }

    if (std::is_signed_v<char>) {
      REQUIRE_EXCEPTION_MSG(
          (KnuthBendix<std::string, TestType>(congruence_kind::onesided, p)),
          "expected the 2nd argument (presentation) to "
          "have alphabet of size at most 128, but found 256");
    } else {
      REQUIRE_NOTHROW(
          KnuthBendix<std::string, TestType>(congruence_kind::onesided, p));
    }
  }

}  // namespace libsemigroups
