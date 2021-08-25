// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019 James D. Mitchell
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

// This file is one of six that contains tests for the KnuthBendix classes. In
// a mostly vain attempt to speed up compilation the tests are split across 6
// files as follows:
//
// 1: contains quick tests for fpsemigroup::KnuthBendix created from rules and
//    all commented out tests.
//
// 2: contains more quick tests for fpsemigroup::KnuthBendix created from rules
//
// 3: contains yet more quick tests for fpsemigroup::KnuthBendix created from
//    rules
//
// 4: contains standard and extreme test for fpsemigroup::KnuthBendix created
//    from rules
//
// 5: contains tests for fpsemigroup::KnuthBendix created from FroidurePin
//    instances
//
// 6: contains tests for congruence::KnuthBendix.

// TODO(later)
// 1. The other examples from Sims' book (Chapters 5 and 6) which use
//    reduction orderings different from shortlex
// 2. Examples from MAF

// #define CATCH_CONFIG_ENABLE_PAIR_STRINGMAKER

#include <string>  // for string
#include <vector>  // for vector

#include "catch.hpp"      // for REQUIRE, REQUIRE_NOTHROW, REQUIRE_THROWS_AS
#include "test-main.hpp"  // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/config.hpp"        // for LIBSEMIGROUPS_DEBUG
#include "libsemigroups/constants.hpp"     // for POSITIVE_INFINITY
#include "libsemigroups/kbe.hpp"           // for KBE
#include "libsemigroups/knuth-bendix.hpp"  // for KnuthBendix, operator<<
#include "libsemigroups/report.hpp"        // for ReportGuard
#include "libsemigroups/types.hpp"         // for word_type

namespace libsemigroups {
  struct LibsemigroupsException;
  constexpr bool REPORT = false;

  namespace fpsemigroup {

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "000",
                            "(fpsemi) confluent fp semigroup 1 (infinite)",
                            "[quick][knuth-bendix][fpsemi][fpsemigroup]") {
      auto rg = ReportGuard(REPORT);

      KnuthBendix kb;
      kb.set_alphabet(3);
      kb.add_rule({0, 1}, {1, 0});
      kb.add_rule({0, 2}, {2, 0});
      kb.add_rule({0, 0}, {0});
      kb.add_rule({0, 2}, {0});
      kb.add_rule({2, 0}, {0});
      kb.add_rule({1, 1}, {1, 1});
      kb.add_rule({1, 2}, {2, 1});
      kb.add_rule({1, 1, 1}, {1});
      kb.add_rule({1, 2}, {1});
      kb.add_rule({2, 1}, {1});
      kb.add_rule({0}, {1});

      REQUIRE(kb.confluent());
#ifdef LIBSEMIGROUPS_DEBUG
      REQUIRE(kb.alphabet() == "abc");
#endif
      REQUIRE(kb.number_of_active_rules() == 4);
      REQUIRE(kb.normal_form({2, 0}) == word_type({0}));
      REQUIRE(kb.normal_form({0, 2}) == word_type({0}));
      REQUIRE(kb.equal_to(word_type({2, 0}), word_type({0})));
      REQUIRE(kb.equal_to(word_type({0, 2}), word_type({0})));
#ifdef LIBSEMIGROUPS_DEBUG
      REQUIRE(kb.normal_form("ac") == "a");
#endif
      REQUIRE(kb.is_obviously_infinite());

      REQUIRE(std::vector<std::string>(kb.cbegin_normal_forms("abc", 0, 5),
                                       kb.cend_normal_forms())
              == std::vector<std::string>({"a", "c", "cc", "ccc", "cccc"}));
      REQUIRE(kb.size() == POSITIVE_INFINITY);
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "001",
                            "(fpsemi) confluent fp semigroup 2 (infinite)",
                            "[quick][knuth-bendix][fpsemigroup][fpsemi]") {
      auto rg = ReportGuard(REPORT);

      KnuthBendix kb;
      REQUIRE_THROWS_AS(kb.add_rule({0, 1}, {1, 0}), LibsemigroupsException);

      kb.set_alphabet(3);
      kb.add_rule({0, 2}, {2, 0});
      kb.add_rule({0, 0}, {0});
      kb.add_rule({0, 2}, {0});
      kb.add_rule({2, 0}, {0});
      kb.add_rule({1, 1}, {1, 1});
      kb.add_rule({1, 2}, {2, 1});
      kb.add_rule({1, 1, 1}, {1});
      kb.add_rule({1, 2}, {1});
      kb.add_rule({2, 1}, {1});
      kb.add_rule({0}, {1});

      REQUIRE(kb.confluent());
      REQUIRE(kb.number_of_active_rules() == 4);
      REQUIRE(kb.is_obviously_infinite());
      REQUIRE(std::vector<std::string>(kb.cbegin_normal_forms("abc", 0, 5),
                                       kb.cend_normal_forms())
              == std::vector<std::string>({"a", "c", "cc", "ccc", "cccc"}));
      REQUIRE(kb.size() == POSITIVE_INFINITY);
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "002",
                            "(fpsemi) confluent fp semigroup 3 (infinite)",
                            "[quick][knuth-bendix][fpsemigroup][fpsemi]") {
      auto rg = ReportGuard(REPORT);

      KnuthBendix kb;
      kb.set_alphabet("012");

      REQUIRE(kb.alphabet() == "012");
      REQUIRE(kb.number_of_active_rules() == 0);

      kb.add_rule("01", "10");
      kb.add_rule("02", "20");
      kb.add_rule("00", "0");
      kb.add_rule("02", "0");
      kb.add_rule("20", "0");
      kb.add_rule("11", "11");
      kb.add_rule("12", "21");
      kb.add_rule("111", "1");
      kb.add_rule("12", "1");
      kb.add_rule("21", "1");
      kb.add_rule("0", "1");

      REQUIRE(kb.number_of_active_rules() == 4);
      REQUIRE(kb.confluent());
      REQUIRE(kb.number_of_active_rules() == 4);
      REQUIRE(kb.size() == POSITIVE_INFINITY);

      REQUIRE(std::vector<std::string>(kb.cbegin_normal_forms(1, 2),
                                       kb.cend_normal_forms())
              == std::vector<std::string>({"0", "2"}));

      REQUIRE(std::vector<std::string>(kb.cbegin_normal_forms(1, 12),
                                       kb.cend_normal_forms())
              == std::vector<std::string>({"0",
                                           "2",
                                           "22",
                                           "222",
                                           "2222",
                                           "22222",
                                           "222222",
                                           "2222222",
                                           "22222222",
                                           "222222222",
                                           "2222222222",
                                           "22222222222"}));
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "003",
                            "(fpsemi) non-confluent fp semigroup from "
                            "wikipedia (infinite)",
                            "[quick][knuth-bendix][fpsemigroup][fpsemi]") {
      auto rg = ReportGuard(REPORT);

      KnuthBendix kb;
      kb.set_alphabet("01");

      kb.add_rule("000", "");
      kb.add_rule("111", "");
      kb.add_rule("010101", "");

      REQUIRE(kb.alphabet() == "01");
      REQUIRE(!kb.confluent());
      kb.run();
      REQUIRE(kb.number_of_active_rules() == 4);
      REQUIRE(kb.confluent());
      REQUIRE(kb.size() == POSITIVE_INFINITY);

      REQUIRE(std::vector<std::string>(
                  {"",     "0",    "1",    "00",   "01",   "10",   "11",
                   "001",  "010",  "011",  "100",  "101",  "110",  "0010",
                   "0011", "0100", "0101", "0110", "1001", "1011", "1101"})
              == std::vector<std::string>(kb.cbegin_normal_forms(0, 5),
                                          kb.cend_normal_forms()));
      REQUIRE(std::all_of(
          kb.cbegin_normal_forms(0, 10),
          kb.cend_normal_forms(),
          [&kb](std::string const& w) { return kb.normal_form(w) == w; }));
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "004",
                            "(fpsemi) Example 5.1 in Sims (infinite)",
                            "[quick][knuth-bendix][fpsemigroup][fpsemi]") {
      auto rg = ReportGuard(REPORT);

      KnuthBendix kb;
      kb.set_alphabet("abcd");

      kb.add_rule("ab", "");
      kb.add_rule("ba", "");
      kb.add_rule("cd", "");
      kb.add_rule("dc", "");
      kb.add_rule("ca", "ac");

      REQUIRE(!kb.confluent());
      kb.run();
      REQUIRE(kb.number_of_active_rules() == 8);
      REQUIRE(kb.confluent());
      REQUIRE(kb.size() == POSITIVE_INFINITY);
      REQUIRE(std::vector<std::string>(
                  {"",     "a",    "b",    "c",    "d",    "aa",   "ac",
                   "ad",   "bb",   "bc",   "bd",   "cc",   "dd",   "aaa",
                   "aac",  "aad",  "acc",  "add",  "bbb",  "bbc",  "bbd",
                   "bcc",  "bdd",  "ccc",  "ddd",  "aaaa", "aaac", "aaad",
                   "aacc", "aadd", "accc", "addd", "bbbb", "bbbc", "bbbd",
                   "bbcc", "bbdd", "bccc", "bddd", "cccc", "dddd"})
              == std::vector<std::string>(kb.cbegin_normal_forms(0, 5),
                                          kb.cend_normal_forms()));
      REQUIRE(std::all_of(
          kb.cbegin_normal_forms(0, 6),
          kb.cend_normal_forms(),
          [&kb](std::string const& w) { return kb.normal_form(w) == w; }));
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "005",
                            "(fpsemi) Example 5.1 in Sims (infinite)",
                            "[quick][knuth-bendix][fpsemigroup][fpsemi]") {
      auto rg = ReportGuard(REPORT);

      KnuthBendix kb;
      kb.set_alphabet("aAbB");

      kb.add_rule("aA", "");
      kb.add_rule("Aa", "");
      kb.add_rule("bB", "");
      kb.add_rule("Bb", "");
      kb.add_rule("ba", "ab");

      REQUIRE(!kb.confluent());
      kb.run();
      REQUIRE(kb.number_of_active_rules() == 8);
      REQUIRE(kb.confluent());
      REQUIRE(kb.size() == POSITIVE_INFINITY);
      REQUIRE(std::vector<std::string>(
                  {"",     "a",    "b",    "c",    "d",    "aa",   "ac",
                   "ad",   "bb",   "bc",   "bd",   "cc",   "dd",   "aaa",
                   "aac",  "aad",  "acc",  "add",  "bbb",  "bbc",  "bbd",
                   "bcc",  "bdd",  "ccc",  "ddd",  "aaaa", "aaac", "aaad",
                   "aacc", "aadd", "accc", "addd", "bbbb", "bbbc", "bbbd",
                   "bbcc", "bbdd", "bccc", "bddd", "cccc", "dddd"})
              == std::vector<std::string>(kb.cbegin_normal_forms("abcd", 0, 5),
                                          kb.cend_normal_forms()));
      REQUIRE(std::all_of(
          kb.cbegin_normal_forms(0, 6),
          kb.cend_normal_forms(),
          [&kb](std::string const& w) { return kb.normal_form(w) == w; }));
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "006",
                            "(fpsemi) Example 5.3 in Sims",
                            "[quick][knuth-bendix][fpsemigroup][fpsemi]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_alphabet("ab");

      kb.add_rule("aa", "");
      kb.add_rule("bbb", "");
      kb.add_rule("ababab", "");

      REQUIRE(!kb.confluent());
      kb.run();
      REQUIRE(kb.number_of_active_rules() == 6);
      REQUIRE(kb.confluent());
      REQUIRE(kb.size() == 12);
      REQUIRE(std::distance(kb.cbegin_normal_forms(0, POSITIVE_INFINITY),
                            kb.cend_normal_forms())
              == 12);
      REQUIRE(std::vector<std::string>({"",
                                        "a",
                                        "b",
                                        "ab",
                                        "ba",
                                        "bb",
                                        "aba",
                                        "abb",
                                        "bab",
                                        "bba",
                                        "babb",
                                        "bbab"})
              == std::vector<std::string>(
                  kb.cbegin_normal_forms(0, POSITIVE_INFINITY),
                  kb.cend_normal_forms()));
      REQUIRE(std::all_of(
          kb.cbegin_normal_forms(0, 6),
          kb.cend_normal_forms(),
          [&kb](std::string const& w) { return kb.normal_form(w) == w; }));
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "007",
                            "(fpsemi) Example 5.4 in Sims",
                            "[quick][knuth-bendix][fpsemigroup][fpsemi]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_alphabet("Bab");

      kb.add_rule("aa", "");
      kb.add_rule("bB", "");
      kb.add_rule("bbb", "");
      kb.add_rule("ababab", "");

      REQUIRE(!kb.confluent());
      kb.run();
      REQUIRE(kb.number_of_active_rules() == 11);
      REQUIRE(kb.confluent());
      REQUIRE(kb.size() == 12);
      REQUIRE(std::vector<std::string>(kb.cbegin_normal_forms(1, 5),
                                       kb.cend_normal_forms())
              == std::vector<std::string>({"B",
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

    LIBSEMIGROUPS_TEST_CASE(
        "KnuthBendix",
        "008",
        "(fpsemi) Example 6.4 in Sims (size 168)",
        "[no-valgrind][quick][knuth-bendix][fpsemigroup][fpsemi]") {
      auto rg = ReportGuard(REPORT);

      KnuthBendix kb;
      kb.set_alphabet("abc");

      kb.add_rule("aa", "");
      kb.add_rule("bc", "");
      kb.add_rule("bbb", "");
      kb.add_rule("ababababababab", "");
      kb.add_rule("abacabacabacabac", "");

      REQUIRE(kb.alphabet() == "abc");
      REQUIRE(!kb.confluent());
      REQUIRE(!kb.is_obviously_infinite());
      REQUIRE(!kb.is_obviously_finite());
      kb.run();
      REQUIRE(kb.number_of_active_rules() == 40);
      REQUIRE(kb.confluent());
      REQUIRE(kb.normal_form("cc") == "b");
      REQUIRE(kb.normal_form("ccc") == "");
      REQUIRE(kb.size() == 168);

      REQUIRE(std::vector<std::string>(kb.cbegin_normal_forms(1, 5),
                                       kb.cend_normal_forms())
              == std::vector<std::string>(
                  {"a",    "b",    "c",    "ab",   "ac",   "ba",   "ca",
                   "aba",  "aca",  "bab",  "bac",  "cab",  "cac",  "abab",
                   "abac", "acab", "acac", "baba", "baca", "caba", "caca"}));
      using FroidurePinKBE = KnuthBendix::froidure_pin_type;
      auto& S              = static_cast<FroidurePinKBE&>(*kb.froidure_pin());
      REQUIRE(S.size() == 168);
      REQUIRE(S.generator(2).string(kb) == "c");
      // FIXME(later) the next line compiles but leaves T in an invalid state.
      // auto T = FroidurePinKBE({S.generator(2)});

      // Uncommenting the following adds ~3 seconds to the compile time of this
      // file.
      // auto T = FroidurePinKBE(kb);
      // T.add_generator(S.generator(2));
      // REQUIRE(T.size() == 3);
      // REQUIRE(get_strings(T) == std::vector<std::string>({"c", "b", ""}));
    }

    LIBSEMIGROUPS_TEST_CASE(
        "KnuthBendix",
        "009",
        "(fpsemi)",
        "[quick][knuth-bendix][fpsemigroup][fpsemi][no-valgrind]") {
      auto rg = ReportGuard(REPORT);

      KnuthBendix kb;
      kb.set_alphabet("012");

      kb.add_rule("000", "2");
      kb.add_rule("111", "2");
      kb.add_rule("010101", "2");
      kb.set_identity("2");

      REQUIRE(kb.alphabet() == "012");
      REQUIRE(!kb.confluent());
      kb.run();
      REQUIRE(kb.number_of_active_rules() == 9);
      REQUIRE(kb.confluent());

      auto& ad = kb.gilman_digraph();
      REQUIRE(ad.number_of_nodes() == 9);
      REQUIRE(ad.number_of_edges() == 13);
      REQUIRE(!action_digraph_helper::is_acyclic(ad));

      auto&                  fp = *kb.froidure_pin();
      std::vector<word_type> expected;
      fp.enumerate(100);
      for (size_t i = 0; i < fp.current_size(); ++i) {
        expected.push_back(fp.factorisation(i));
      }

      std::vector<word_type> result(
          ad.cbegin_pislo(0, 1, fp.current_max_word_length() + 1),
          ad.cend_pislo());
      for (size_t i = 0; i < expected.size(); ++i) {
        REQUIRE(expected[i] == result[i]);
      }
      REQUIRE(std::vector<std::string>(kb.cbegin_normal_forms(1, 5),
                                       kb.cend_normal_forms())
              == std::vector<std::string>(
                  {"0",    "1",    "2",    "00",   "01",   "10",   "11",
                   "001",  "010",  "011",  "100",  "101",  "110",  "0010",
                   "0011", "0100", "0101", "0110", "1001", "1011", "1101"}));
    }

    LIBSEMIGROUPS_TEST_CASE(
        "KnuthBendix",
        "010",
        "(fpsemi) SL(2, 7) from Chapter 3, Proposition 1.5 in NR "
        "(size 336)",
        "[no-valgrind][quick][knuth-bendix][fpsemigroup][fpsemi]") {
      using FroidurePinKBE = KnuthBendix::froidure_pin_type;
      auto rg              = ReportGuard(REPORT);

      KnuthBendix kb;
      kb.set_alphabet("abAB");

      kb.add_rule("aaaaaaa", "");
      kb.add_rule("bb", "ababab");
      kb.add_rule("bb", "aaaabaaaabaaaabaaaab");
      kb.add_rule("aA", "");
      kb.add_rule("Aa", "");
      kb.add_rule("bB", "");
      kb.add_rule("Bb", "");

      REQUIRE(!kb.confluent());

      kb.run();
      REQUIRE(kb.number_of_active_rules() == 152);
      REQUIRE(kb.confluent());
      REQUIRE(kb.size() == 336);

      // Test copy constructor
      auto           T = static_cast<FroidurePinKBE&>(*kb.froidure_pin());
      FroidurePinKBE S = T.copy_closure({T.generator(0)});

      REQUIRE(S.size() == 336);
      REQUIRE(S.number_of_generators() == 4);

      auto& ad = kb.gilman_digraph();
      REQUIRE(ad.number_of_nodes() == 232);
      REQUIRE(ad.number_of_edges() == 265);
      REQUIRE(action_digraph_helper::is_acyclic(ad));
      REQUIRE(ad.number_of_paths(0, 0, 13) == 336);
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "011",
                            "(fpsemi) F(2, 5) - Chapter 9, Section 1 in NR "
                            "(size 11)",
                            "[knuth-bendix][fpsemigroup][fpsemi][quick]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_alphabet("abcde");

      kb.add_rule("ab", "c");
      kb.add_rule("bc", "d");
      kb.add_rule("cd", "e");
      kb.add_rule("de", "a");
      kb.add_rule("ea", "b");

      REQUIRE(!kb.confluent());
      kb.run();
      REQUIRE(kb.number_of_active_rules() == 24);
      REQUIRE(kb.confluent());
      REQUIRE(kb.size() == 11);

      auto& ad = kb.gilman_digraph();
      REQUIRE(ad.number_of_nodes() == 8);
      REQUIRE(ad.number_of_edges() == 11);
      REQUIRE(action_digraph_helper::is_acyclic(ad));
      REQUIRE(ad.number_of_paths(0, 0, 5) == 12);
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "012",
                            "(fpsemi) Reinis example 1",
                            "[quick][knuth-bendix][fpsemigroup][fpsemi]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_alphabet("abc");

      kb.add_rule("a", "abb");
      kb.add_rule("b", "baa");

      REQUIRE(!kb.confluent());
      kb.run();
      REQUIRE(kb.number_of_active_rules() == 4);

      auto& ad = kb.gilman_digraph();
      REQUIRE(ad.number_of_nodes() == 7);
      REQUIRE(ad.number_of_edges() == 17);
      REQUIRE(!action_digraph_helper::is_acyclic(ad));
      REQUIRE(ad.number_of_paths(0, 0, 10) == 13044);
    }

    ////////////////////////////////////////////////////////////////////////
    // Commented out test cases
    ////////////////////////////////////////////////////////////////////////

    // This example verifies the nilpotence of the group using the Sims
    // algorithm. The original presentation was <a,b| [b,a,b], [b,a,a,a,a],
    // [b,a,a,a,b,a,a] >. (where [] mean left-normed commutators). The
    // presentation here was derived by first applying the NQA to find the
    // maximal nilpotent quotient, and then introducing new generators for the
    // PCP generators. It is essential for success that reasonably low values of
    // the maxstoredlen parameter are given.
    // LIBSEMIGROUPS_TEST_CASE(
    //     "KnuthBendix",
    //     "013",
    //     "(fpsemi) (from kbmag/standalone/kb_data/verifynilp)",
    //     "[quick][knuth-bendix][fpsemigroup][fpsemi][kbmag][recursive]") {}
    //   KnuthBendix kb(new RECURSIVE(), "hHgGfFyYdDcCbBaA");
    //   kb.add_rule("BAba", "c");
    //   kb.add_rule("CAca", "d");
    //   kb.add_rule("DAda", "y");
    //   kb.add_rule("YByb", "f");
    //   kb.add_rule("FAfa", "g");
    //   kb.add_rule("ga", "ag");
    //   kb.add_rule("GBgb", "h");
    //   kb.add_rule("cb", "bc");
    //   kb.add_rule("ya", "ay");
    //   auto rg = ReportGuard(REPORT);
    //
    //   REQUIRE(kb.confluent());
    //
    //   kb.run();
    //   REQUIRE(kb.confluent());
    //   REQUIRE(kb.number_of_active_rules() == 9);
    //
    //   REQUIRE(kb.equal_to("BAba", "c"));
    //   REQUIRE(kb.equal_to("CAca", "d"));
    //   REQUIRE(kb.equal_to("DAda", "y"));
    //   REQUIRE(kb.equal_to("YByb", "f"));
    //   REQUIRE(kb.equal_to("FAfa", "g"));
    //   REQUIRE(kb.equal_to("ga", "ag"));
    //   REQUIRE(kb.equal_to("GBgb", "h"));
    //   REQUIRE(kb.equal_to("cb", "bc"));
    //   REQUIRE(kb.equal_to("ya", "ay"));
    // REQUIRE(kb.active_rules() == std::vector<std::pair<std::string,
    // std::string>>({}));
    // }

    // TODO(later): temporarily commented out to because of change to
    // FpSemigroupInterface that forbids adding rules after started(), and
    // because the copy constructors for KnuthBendix et al. don't currently work
    // LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
    //                         "014",
    //                         "(cong) finite transformation semigroup "
    //                         "congruence (21 classes)",
    //                         "[quick][congruence][knuth-bendix][cong]") {
    //   auto rg      = ReportGuard(REPORT);
    //   using Transf = LeastTransf<5>;
    //   FroidurePin<Transf> S({Transf({1, 3, 4, 2, 3}), Transf({3, 2, 1, 3,
    //   3})});

    //   REQUIRE(S.size() == 88);
    //   REQUIRE(S.number_of_rules() == 18);

    //   KnuthBendix kb(S);
    //   auto&       P = kb.quotient_froidure_pin();
    //   REQUIRE(P.size() == 88);
    //   kb.add_pair(S.factorisation(Transf({3, 4, 4, 4, 4})),
    //               S.factorisation(Transf({3, 1, 3, 3, 3})));
    //   // P is now invalid, it's a reference to something that was deleted in
    //   // kb.

    //   REQUIRE(kb.number_of_classes() == 21);
    //   REQUIRE(kb.number_of_classes() == 21);
    //   auto& Q = kb.quotient_froidure_pin();  // quotient

    //   REQUIRE(Q.size() == 21);
    //   REQUIRE(Q.number_of_idempotents() == 3);

    //   std::vector<word_type>
    //   v(static_cast<FroidurePin<detail::KBE>&>(Q).cbegin(),
    //                            static_cast<FroidurePin<detail::KBE>&>(Q).cend());
    //   REQUIRE(v
    //           == std::vector<word_type>({{0},
    //                                      {1},
    //                                      {0, 0},
    //                                      {0, 1},
    //                                      {1, 0},
    //                                      {1, 1},
    //                                      {0, 0, 0},
    //                                      {0, 0, 1},
    //                                      {0, 1, 0},
    //                                      {0, 1, 1},
    //                                      {1, 0, 0},
    //                                      {1, 1, 0},
    //                                      {0, 0, 0, 0},
    //                                      {0, 1, 0, 0},
    //                                      {0, 1, 1, 0},
    //                                      {1, 0, 0, 0},
    //                                      {1, 1, 0, 0},
    //                                      {0, 1, 0, 0, 0},
    //                                      {0, 1, 1, 0, 0},
    //                                      {1, 1, 0, 0, 0},
    //                                      {0, 1, 1, 0, 0, 0}}));

    //   REQUIRE(
    //       kb.word_to_class_index(S.factorisation(Transf({1, 3, 1, 3, 3})))
    //       == kb.word_to_class_index(S.factorisation(Transf({4, 2, 4, 4,
    //       2}))));

    //   REQUIRE(kb.number_of_non_trivial_classes() == 1);
    //   REQUIRE(kb.number_of_generators() == 2);
    //   REQUIRE(kb.cbegin_ntc()->size() == 68);
    // }

    //  A nonhopfian group
    // LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
    //                         "015",
    //                         "(from kbmag/standalone/kb_data/nonhopf)",
    //                         "[quick][knuth-bendix][kbmag][recursive]") {
    //   KnuthBendix kb(new RECURSIVE(), "aAbB");
    //   kb.add_rule("Baab", "aaa");
    //   auto rg = ReportGuard(REPORT);

    //   REQUIRE(kb.confluent());

    //   kb.run();
    //   REQUIRE(kb.confluent());
    //   REQUIRE(kb.number_of_active_rules() == 1);

    //   REQUIRE(kb.equal_to("Baab", "aaa"));
    //   REQUIRE(kb.active_rules()
    //           == std::vector<rule_type>({}));
    // }

    // LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
    //                         "016",
    //                         "(from kbmag/standalone/kb_data/freenilpc3)",
    //                         "[quick][knuth-bendix][kbmag][recursive]") {
    //   KnuthBendix kb(new RECURSIVE(), "yYdDcCbBaA");
    //   kb.add_rule("BAba", "c");
    //   kb.add_rule("CAca", "d");
    //   kb.add_rule("CBcb", "y");
    //   kb.add_rule("da", "ad");
    //   kb.add_rule("ya", "ay");
    //   kb.add_rule("db", "bd");
    //   kb.add_rule("yb", "by");
    //   auto rg = ReportGuard(REPORT);

    //   REQUIRE(kb.confluent());

    //   kb.run();
    //   REQUIRE(kb.confluent());
    //   REQUIRE(kb.number_of_active_rules() == 7);

    //   REQUIRE(kb.equal_to("BAba", "c"));
    //   REQUIRE(kb.equal_to("CAca", "d"));
    //   REQUIRE(kb.equal_to("CBcb", "y"));
    //   REQUIRE(kb.equal_to("da", "ad"));
    //   REQUIRE(kb.equal_to("ya", "ay"));
    //   REQUIRE(kb.equal_to("db", "bd"));
    //   REQUIRE(kb.equal_to("yb", "by"));
    //   REQUIRE(kb.active_rules()
    //           == std::vector<rule_type>({}));
    // }

    // TODO(later): temporarily commented out to because of change to
    // FpSemigroupInterface that forbids adding rules after started(), and
    // because the copy constructors for KnuthBendix et al. don't currently work
    // LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
    //                         "017",
    //                         "add_rule after knuth_bendix",
    //                         "[quick][knuth-bendix][fpsemigroup]") {
    //   auto        rg = ReportGuard(REPORT);
    //   KnuthBendix kb;
    //   kb.set_alphabet("Bab");
    //   kb.add_rule("aa", "");
    //   kb.add_rule("bB", "");
    //   kb.add_rule("bbb", "");
    //   kb.add_rule("ababab", "");

    //   REQUIRE(!kb.confluent());
    //   kb.run_for(FOREVER);
    //   REQUIRE(kb.finished());
    //   // The next line tests what happens when run_for is called when
    //   finished. kb.run_for(FOREVER); REQUIRE(kb.number_of_active_rules() ==
    //   11); REQUIRE(kb.confluent()); REQUIRE(kb.size() == 12);

    //   REQUIRE(kb.equal_to("aa", ""));
    //   REQUIRE(!kb.equal_to("a", "b"));

    //   KnuthBendix kb2(&kb);
    //   REQUIRE(kb2.number_of_active_rules() == 11);
    //   kb2.add_rule("a", "b");
    //   REQUIRE(kb2.number_of_rules() == 5);
    //   // Adding a rule does not change the number of active rules until
    //   *after*
    //   // kb.run() is called again.
    //   REQUIRE(kb2.number_of_active_rules() == 11);

    //   using rules_type = std::vector<rule_type>;

    //   REQUIRE(rules_type(kb2.cbegin_rules(), kb2.cend_rules())
    //           == rules_type({{"aa", ""},
    //                          {"bB", ""},
    //                          {"bbb", ""},
    //                          {"ababab", ""},
    //                          {"a", "b"}}));

    //   REQUIRE(!kb2.confluent());
    //   REQUIRE(kb2.size() == 1);
    //   REQUIRE(kb2.confluent());
    //   REQUIRE(kb2.number_of_active_rules() == 3);
    //   REQUIRE(kb2.active_rules() == rules_type({{"B", ""}, {"a", ""}, {"b",
    //   "a"}}));
    // }

    // Free nilpotent group of rank 2 and class 2
    // LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
    //                         "018",
    //                         "(from kbmag/standalone/kb_data/nilp2)",
    //                         "[quick][knuth-bendix][kbmag][recursive]") {
    //   KnuthBendix kb(new RECURSIVE(), "cCbBaA");
    //   kb.add_rule("ba", "abc");
    //   kb.add_rule("ca", "ac");
    //   kb.add_rule("cb", "bc");
    //   auto rg = ReportGuard(REPORT);
    //
    //   REQUIRE(kb.confluent());
    //
    //   kb.run();
    //   REQUIRE(kb.confluent());
    //
    //   REQUIRE(kb.number_of_active_rules() == 3);
    // }

    // monoid presentation of F(2,7) - should produce a monoid of length 30
    // which is the same as the group, together with the empty word. This is a
    // very difficult calculation indeed, however.
    //
    // KBMAG does not terminate when SHORTLEX order is used.
    // LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
    //                         "019",
    //                         "(from kbmag/standalone/kb_data/f27monoid)",
    //                         "[fail][knuth-bendix][kbmag][recursive]") {
    //   KnuthBendix kb(new RECURSIVE(), "abcdefg");
    //   kb.add_rule("ab", "c");
    //   kb.add_rule("bc", "d");
    //   kb.add_rule("cd", "e");
    //   kb.add_rule("de", "f");
    //   kb.add_rule("ef", "g");
    //   kb.add_rule("fg", "a");
    //   kb.add_rule("ga", "b");
    //   auto rg = ReportGuard(REPORT);

    //   REQUIRE(!kb.confluent());

    //   kb.run();
    //   REQUIRE(kb.confluent());
    //   REQUIRE(kb.number_of_active_rules() == 32767);
    // }

    // This example verifies the nilpotence of the group using the Sims
    // algorithm. The original presentation was <a,b| [b,a,a,a], [b^-1,a,a,a],
    // [a,b,b,b], [a^-1,b,b,b], [a,a*b,a*b,a*b], [a^-1,a*b,a*b,a*b] >. (where []
    // mean left-normed commutators. The presentation here was derived by first
    // applying the NQA to find the maximal nilpotent quotient, and then
    // introducing new generators for the PCP generators.
    // LIBSEMIGROUPS_TEST_CASE(
    //     "KnuthBendix",
    //     "020",
    //     "(fpsemi) (from kbmag/standalone/kb_data/heinnilp)",
    //     "[fail][knuth-bendix][fpsemigroup][fpsemi][kbmag][recursive]") {
    //   // TODO(later) fails because internal_rewrite expect rules to be length
    //   reducing KnuthBendix kb(new RECURSIVE(), "fFyYdDcCbBaA");
    //   kb.add_rule("BAba", "c");
    //   kb.add_rule("CAca", "d");
    //   kb.add_rule("CBcb", "y");
    //   kb.add_rule("DBdb", "f");
    //   kb.add_rule("cBCb", "bcBC");
    //   kb.add_rule("babABaBA", "abABaBAb");
    //   kb.add_rule("cBACab", "abcBAC");
    //   kb.add_rule("BabABBAbab", "aabABBAb");
    //   auto rg = ReportGuard(REPORT);

    //   REQUIRE(!kb.confluent());

    //   kb.run();
    //   REQUIRE(kb.confluent());
    //   REQUIRE(kb.number_of_active_rules() == 32767);
    //  }

  }  // namespace fpsemigroup
}  // namespace libsemigroups
