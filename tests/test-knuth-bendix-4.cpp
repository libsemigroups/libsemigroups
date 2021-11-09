// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2020 James D. Mitchell
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

// This file is the fourth of six that contains tests for the KnuthBendix
// classes. In a mostly vain attempt to speed up compilation the tests are
// split across 6 files as follows:
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

#define CATCH_CONFIG_ENABLE_ALL_STRINGMAKERS

#include <iostream>
#include <string>  // for string
#include <vector>  // for vector

#include "catch.hpp"      // for REQUIRE, REQUIRE_NOTHROW, REQUIRE_THROWS_AS
#include "test-main.hpp"  // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/constants.hpp"     // for POSITIVE_INFINITY
#include "libsemigroups/knuth-bendix.hpp"  // for KnuthBendix, operator<<
#include "libsemigroups/report.hpp"        // for ReportGuard
#include "libsemigroups/siso.hpp"          // for XXX
#include "libsemigroups/stl.hpp"           // for XXX
#include "libsemigroups/string.hpp"        // for random_string

namespace libsemigroups {
  constexpr bool REPORT = false;

  using rule_type = fpsemigroup::KnuthBendix::rule_type;

  namespace fpsemigroup {
    ////////////////////////////////////////////////////////////////////////
    // Standard tests
    ////////////////////////////////////////////////////////////////////////

    // Takes approx. 2s
    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "084",
                            "(fpsemi) Example 6.6 in Sims (with limited "
                            "overlap lengths)",
                            "[standard][knuth-bendix][fpsemigroup][fpsemi]") {
      auto rg = ReportGuard(REPORT);

      KnuthBendix kb;
      kb.set_alphabet("abc");

      kb.add_rule("aa", "");
      kb.add_rule("bc", "");
      kb.add_rule("bbb", "");
      kb.add_rule("ababababababab", "");
      kb.add_rule("abacabacabacabacabacabacabacabac", "");

      REQUIRE(!kb.confluent());

      // In Sims it says to use 44 here, but that doesn't seem to work.
      kb.max_overlap(45);
      // Avoid checking confluence since this is very slow, essentially takes
      // the same amount of time as running Knuth-Bendix (fpsemi) 13.
      kb.check_confluence_interval(LIMIT_MAX);

      kb.run();
      REQUIRE(kb.number_of_active_rules() == 1026);
      // REQUIRE(kb.confluent());
      // REQUIRE(kb.size() == 10752);
    }

    // Takes approx. 2s
    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "085",
                            "(from kbmag/standalone/kb_data/funny3)",
                            "[standard][knuth-bendix][kbmag][shortlex]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_alphabet("aAbBcC");
      kb.set_identity("");
      kb.set_inverses("AaBbCc");

      kb.add_rule("aaa", "");
      kb.add_rule("bbb", "");
      kb.add_rule("ccc", "");
      kb.add_rule("ABa", "BaB");
      kb.add_rule("bcB", "cBc");
      kb.add_rule("caC", "aCa");
      kb.add_rule("abcABCabcABCabcABC", "");
      kb.add_rule("BcabCABcabCABcabCA", "");
      kb.add_rule("cbACBacbACBacbACBa", "");

      REQUIRE(!kb.confluent());

      kb.knuth_bendix_by_overlap_length();
      // kb.run() // also works, but is slower
      REQUIRE(kb.confluent());
      REQUIRE(kb.number_of_active_rules() == 8);
      REQUIRE(kb.size() == 3);
      REQUIRE(
          std::vector<std::string>(kb.cbegin_normal_forms(0, POSITIVE_INFINITY),
                                   kb.cend_normal_forms())
          == std::vector<std::string>({"", "a", "A"}));
    }

    ////////////////////////////////////////////////////////////////////////
    // Extreme tests
    ////////////////////////////////////////////////////////////////////////

    // Fibonacci group F(2,7) - order 29 - works better with largish tidyint
    // Takes approx. 50s
    LIBSEMIGROUPS_TEST_CASE(
        "KnuthBendix",
        "086",
        "(fpsemi) (from kbmag/standalone/kb_data/f27) (finite) (2 / 2)",
        "[extreme][knuth-bendix][fpsemigroup][fpsemi][kbmag][shortlex]") {
      auto        rg = ReportGuard(true);
      KnuthBendix kb;
      kb.set_alphabet("aAbBcCdDyYfFgG");
      kb.set_identity("");
      kb.set_inverses("AaBbCcDdYyFfGg");

      kb.add_rule("ab", "c");
      kb.add_rule("bc", "d");
      kb.add_rule("cd", "y");
      kb.add_rule("dy", "f");
      kb.add_rule("yf", "g");
      kb.add_rule("fg", "a");
      kb.add_rule("ga", "b");

      REQUIRE(!kb.confluent());

      kb.knuth_bendix_by_overlap_length();
      REQUIRE(kb.confluent());
      REQUIRE(kb.number_of_active_rules() == 194);
      REQUIRE(kb.size() == 29);
    }

    // Mathieu group M_11
    // Takes approx. 2m9s (majority in checking confluence)
    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "087",
                            "(from kbmag/standalone/kb_data/m11)",
                            "[extreme][knuth-bendix][kbmag][shortlex]") {
      auto        rg = ReportGuard(true);
      KnuthBendix kb;
      kb.set_alphabet("abB");
      kb.set_identity("");
      kb.set_inverses("aBb");

      kb.add_rule("BB", "bb");
      kb.add_rule("BaBaBaBaBaB", "abababababa");
      kb.add_rule("bbabbabba", "abbabbabb");
      kb.add_rule("aBaBababaBabaBBaBab", "");

      REQUIRE(!kb.confluent());

      kb.knuth_bendix_by_overlap_length();
      REQUIRE(kb.confluent());
      REQUIRE(kb.number_of_active_rules() == 1731);
      REQUIRE(kb.size() == 7920);
    }

    // Weyl group E8 (all gens involutory).
    // Takes approx. 8s for KnuthBendix
    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "088",
                            "(from kbmag/standalone/kb_data/e8)",
                            "[extreme][knuth-bendix][kbmag][shortlex]") {
      auto        rg = ReportGuard(true);
      KnuthBendix kb;
      kb.set_alphabet("abcdefgh");
      kb.set_identity("");
      kb.set_inverses("abcdefgh");
      kb.add_rule("bab", "aba");
      kb.add_rule("ca", "ac");
      kb.add_rule("da", "ad");
      kb.add_rule("ea", "ae");
      kb.add_rule("fa", "af");
      kb.add_rule("ga", "ag");
      kb.add_rule("ha", "ah");
      kb.add_rule("cbc", "bcb");
      kb.add_rule("db", "bd");
      kb.add_rule("eb", "be");
      kb.add_rule("fb", "bf");
      kb.add_rule("gb", "bg");
      kb.add_rule("hb", "bh");
      kb.add_rule("dcd", "cdc");
      kb.add_rule("ece", "cec");
      kb.add_rule("fc", "cf");
      kb.add_rule("gc", "cg");
      kb.add_rule("hc", "ch");
      kb.add_rule("ed", "de");
      kb.add_rule("fd", "df");
      kb.add_rule("gd", "dg");
      kb.add_rule("hd", "dh");
      kb.add_rule("fef", "efe");
      kb.add_rule("ge", "eg");
      kb.add_rule("he", "eh");
      kb.add_rule("gfg", "fgf");
      kb.add_rule("hf", "fh");
      kb.add_rule("hgh", "ghg");

      REQUIRE(!kb.confluent());
      kb.knuth_bendix_by_overlap_length();
      REQUIRE(kb.confluent());
      REQUIRE(kb.number_of_active_rules() == 192);
      REQUIRE(kb.gilman_digraph().number_of_nodes() == 332);
      REQUIRE(kb.gilman_digraph().number_of_edges() == 533);
      REQUIRE(kb.size() == 696729600);
    }

    // Second of BHN's series of increasingly complicated presentations of 1.
    // Works quickest with large value of tidyint
    // Takes > 1m (knuth_bendix), didn't run to the end
    // Takes approx. 9s (knuth_bendix_by_overlap_length)
    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "089",
                            "(from kbmag/standalone/kb_data/degen4b)",
                            "[extreme][knuth-bendix][kbmag][shortlex]") {
      auto        rg = ReportGuard(true);
      KnuthBendix kb;
      kb.set_alphabet("aAbBcC");
      kb.set_identity("");
      kb.set_inverses("AaBbCc");

      kb.add_rule("bbABaBcbCCAbaBBccBCbccBCb", "");
      kb.add_rule("ccBCbCacAABcbCCaaCAcaaCAc", "");
      kb.add_rule("aaCAcAbaBBCacAAbbABabbABa", "");

      REQUIRE(!kb.confluent());
      // kb.run();
      kb.knuth_bendix_by_overlap_length();
      REQUIRE(kb.confluent());
      REQUIRE(kb.number_of_active_rules() == 6);
      REQUIRE(kb.size() == 1);
    }
    // Two generator presentation of Fibonacci group F(2,7) - order 29. Large
    // value of tidyint works better.
    // Takes approx. 12s (knuth_bendix_by_overlap_length)
    // Takes > 19s (knuth_bendix), didn't run to the end
    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "090",
                            "(from kbmag/standalone/kb_data/f27_2gen)",
                            "[extreme][knuth-bendix][kbmag][shortlex]") {
      auto        rg = ReportGuard(true);
      KnuthBendix kb;
      kb.set_alphabet("aAbB");
      kb.set_identity("");
      kb.set_inverses("AaBb");

      kb.add_rule("bababbababbabbababbab", "a");
      kb.add_rule("abbabbababbaba", "b");

      REQUIRE(!kb.confluent());

      kb.knuth_bendix_by_overlap_length();
      REQUIRE(kb.confluent());
      REQUIRE(kb.number_of_active_rules() == 19);
      REQUIRE(kb.size() == 29);
    }

    // Takes approx. 1m8s
    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "091",
                            "(fpsemi) Example 6.6 in Sims",
                            "[extreme][knuth-bendix][fpsemigroup][fpsemi]") {
      auto        rg = ReportGuard(true);
      KnuthBendix kb;
      kb.set_alphabet("abc");

      kb.add_rule("aa", "");
      kb.add_rule("bc", "");
      kb.add_rule("bbb", "");
      kb.add_rule("ababababababab", "");
      kb.add_rule("abacabacabacabacabacabacabacabac", "");

      REQUIRE(!kb.confluent());
      kb.knuth_bendix_by_overlap_length();
      REQUIRE(kb.number_of_active_rules() == 1026);
      REQUIRE(kb.confluent());
      REQUIRE(kb.size() == 10752);
    }

    // Fibonacci group F(2,7) - without inverses
    // Takes approx. 13s
    LIBSEMIGROUPS_TEST_CASE(
        "KnuthBendix",
        "092",
        "(fpsemi) (from kbmag/standalone/kb_data/f27) (infinite) (1 / 2)",
        "[extreme][knuth-bendix][fpsemigroup][fpsemi][kbmag][shortlex]") {
      auto        rg = ReportGuard(true);
      KnuthBendix kb;
      kb.set_alphabet("aAbBcCdDyYfFgG");

      kb.add_rule("ab", "c");
      kb.add_rule("bc", "d");
      kb.add_rule("cd", "y");
      kb.add_rule("dy", "f");
      kb.add_rule("yf", "g");
      kb.add_rule("fg", "a");
      kb.add_rule("ga", "b");

      REQUIRE(!kb.confluent());

      kb.knuth_bendix_by_overlap_length();
      // Fails to terminate, or is very slow, with knuth_bendix
      REQUIRE(kb.confluent());
      REQUIRE(kb.number_of_active_rules() == 47);
      REQUIRE(kb.size() == POSITIVE_INFINITY);
    }

    // An extension of 2^6 be L32
    // Takes approx. 1m7s
    LIBSEMIGROUPS_TEST_CASE(
        "KnuthBendix",
        "093",
        "(fpsemi) (from kbmag/standalone/kb_data/l32ext)",
        "[extreme][knuth-bendix][fpsemigroup][fpsemi][kbmag][shortlex]") {
      auto        rg = ReportGuard(true);
      KnuthBendix kb;
      kb.set_alphabet("abB");
      kb.set_identity("");
      kb.set_inverses("aBb");

      kb.add_rule("aa", "");
      kb.add_rule("BB", "b");
      kb.add_rule("BaBaBaB", "abababa");
      kb.add_rule("aBabaBabaBabaBab", "BabaBabaBabaBaba");

      REQUIRE(!kb.confluent());

      kb.knuth_bendix_by_overlap_length();
      REQUIRE(kb.confluent());
      REQUIRE(kb.number_of_active_rules() == 1026);
      REQUIRE(kb.size() == 10752);

      auto& ad = kb.gilman_digraph();
      REQUIRE(ad.number_of_nodes() == 6021);
      REQUIRE(ad.number_of_edges() == 7435);
      REQUIRE(action_digraph_helper::is_acyclic(ad));
      REQUIRE(ad.number_of_paths(0, 0, 100) == 10752);
    }

    ////////////////////////////////////////////////////////////////////////
    // Tests that fail
    ////////////////////////////////////////////////////////////////////////

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "094",
                            "Ceitin's undecidable word problem example",
                            "[fail][knuth-bendix]") {
      auto        rg = ReportGuard(true);
      KnuthBendix kb;
      kb.set_alphabet("abcde");
      kb.add_rule("ac", "ca");
      kb.add_rule("ad", "da");
      kb.add_rule("bc", "cb");
      kb.add_rule("bd", "db");
      kb.add_rule("eca", "ce");
      kb.add_rule("edb", "de");
      kb.add_rule("cca", "ccae");
      kb.run();  // I guess this shouldn't work, and indeed it doesn't!
    }

    // kbmag/standalone/kb_data/verifynilp
    LIBSEMIGROUPS_TEST_CASE(
        "KnuthBendix",
        "095",
        "(KnuthBendix 050 again) (from kbmag/standalone/kb_data/verifynilp)",
        "[fail][knuth-bendix][kbmag][shortlex]") {
      auto                rg    = ReportGuard();
      std::string         lphbt = "hHgGfFyYdDcCbBaA";
      std::string         invrs = "HhGgFfYyDdCcBbAa";
      std::vector<size_t> perm(lphbt.size(), 0);
      std::iota(perm.begin(), perm.end(), 0);

      do {
        detail::apply_permutation(lphbt, invrs, perm);
        // std::cout << "Alphabet is " << lphbt << std::endl;
        // std::cout << std::string(72, '#') << std::endl;

        KnuthBendix kb;
        kb.set_alphabet(lphbt);
        kb.set_identity("");
        kb.set_inverses(invrs);

        kb.add_rule("BAba", "c");
        kb.add_rule("CAca", "d");
        kb.add_rule("DAda", "y");
        kb.add_rule("YByb", "f");
        kb.add_rule("FAfa", "g");
        kb.add_rule("ga", "ag");
        kb.add_rule("GBgb", "h");
        kb.add_rule("cb", "bc");
        kb.add_rule("ya", "ay");

        REQUIRE(!kb.confluent());

        kb.knuth_bendix_by_overlap_length();
        if (kb.confluent()) {
          REQUIRE(kb.number_of_active_rules() == 0);
          REQUIRE(kb.size() == 0);
          break;
        }
      } while (std::next_permutation(perm.begin(), perm.end()));
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "096",
                            "Sorouhesh",
                            "[quick][knuth-bendix][kbmag][shortlex]") {
      auto         rg = ReportGuard(REPORT);
      size_t const n  = 2;
      size_t const p  = 11;

      KnuthBendix kb;
      kb.set_alphabet("ab");
      kb.add_rule(std::string(size_t(std::pow(5, n)), 'a'), "a");
      kb.add_rule("aba", "b");
      kb.add_rule("ab", std::string(p, 'b') + "a");

      REQUIRE(!kb.confluent());

      kb.run();
      REQUIRE(kb.number_of_active_rules() == 7);
      REQUIRE(kb.size() == size_t(std::pow(5, n)) + 4 * p - 5);
      REQUIRE(kb.normal_form("aabb") == "aabb");
      REQUIRE(kb.normal_form("aabbaabb") == "bbbb");
      REQUIRE(kb.normal_form("aabbaabbaabb") == "aabbbbbb");
      REQUIRE(kb.normal_form("aabbaabbaabbaabb") == "bbbbbbbb");
      REQUIRE(kb.normal_form("aabbaabbaabbaabbaabb") == "aabbbbbbbbbb");
      REQUIRE(
          std::vector<std::string>(kb.cbegin_normal_forms(0, POSITIVE_INFINITY),
                                   kb.cend_normal_forms())
          == std::vector<std::string>({"a",
                                       "b",
                                       "aa",
                                       "ab",
                                       "ba",
                                       "bb",
                                       "aaa",
                                       "aab",
                                       "abb",
                                       "bab",
                                       "bbb",
                                       "aaaa",
                                       "aabb",
                                       "abbb",
                                       "babb",
                                       "bbbb",
                                       "aaaaa",
                                       "aabbb",
                                       "abbbb",
                                       "babbb",
                                       "bbbbb",
                                       "aaaaaa",
                                       "aabbbb",
                                       "abbbbb",
                                       "babbbb",
                                       "bbbbbb",
                                       "aaaaaaa",
                                       "aabbbbb",
                                       "abbbbbb",
                                       "babbbbb",
                                       "bbbbbbb",
                                       "aaaaaaaa",
                                       "aabbbbbb",
                                       "abbbbbbb",
                                       "babbbbbb",
                                       "bbbbbbbb",
                                       "aaaaaaaaa",
                                       "aabbbbbbb",
                                       "abbbbbbbb",
                                       "babbbbbbb",
                                       "bbbbbbbbb",
                                       "aaaaaaaaaa",
                                       "aabbbbbbbb",
                                       "abbbbbbbbb",
                                       "babbbbbbbb",
                                       "bbbbbbbbbb",
                                       "aaaaaaaaaaa",
                                       "aabbbbbbbbb",
                                       "abbbbbbbbbb",
                                       "babbbbbbbbb",
                                       "aaaaaaaaaaaa",
                                       "aabbbbbbbbbb",
                                       "aaaaaaaaaaaaa",
                                       "aaaaaaaaaaaaaa",
                                       "aaaaaaaaaaaaaaa",
                                       "aaaaaaaaaaaaaaaa",
                                       "aaaaaaaaaaaaaaaaa",
                                       "aaaaaaaaaaaaaaaaaa",
                                       "aaaaaaaaaaaaaaaaaaa",
                                       "aaaaaaaaaaaaaaaaaaaa",
                                       "aaaaaaaaaaaaaaaaaaaaa",
                                       "aaaaaaaaaaaaaaaaaaaaaa",
                                       "aaaaaaaaaaaaaaaaaaaaaaa",
                                       "aaaaaaaaaaaaaaaaaaaaaaaa"}));
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
        auto tmp = *it1 + "#" + *it2;
        auto u   = swap_a_and_b(*it1);
        auto v   = swap_a_and_b(*it2);
        if (shortlex_compare(u, v)) {
          get_set().insert(u + "#" + v);
        } else {
          get_set().insert(v + "#" + u);
        }
        std::cout << *it1 << " = " << *it2 << std::endl;
        nr++;
      }
    }  // namespace

    LIBSEMIGROUPS_TEST_CASE(
        "KnuthBendix",
        "015",
        "(fpsemi) all 2-generated 1-relation semigroups 1 to 10",
        "[extreme][kambites][fpsemigroup][fpsemi][xxx]") {
      auto rg = ReportGuard(false);
      auto first
          = cbegin_sislo("ab", std::string(1, 'a'), std::string(11, 'a'));
      auto last = cbegin_sislo("ab", std::string(1, 'a'), std::string(11, 'a'));
      size_t N  = std::distance(
          first, cend_sislo("ab", std::string(1, 'a'), std::string(11, 'a')));
      REQUIRE(N == 2046);
      std::advance(last, N - 1);

      size_t total_c4 = 0;
      size_t total    = 0;
      auto   llast    = last;
      ++llast;

      for (auto it1 = first; it1 != last; ++it1) {
        auto it2 = it1;
        ++it2;
        for (; it2 != llast; ++it2) {
          auto tmp = *it1 + "#" + *it2;
          if (get_set().insert(tmp).second) {
            bool try_again = false;
            {
              KnuthBendix k;
              k.set_alphabet("ab");
              k.add_rule(*it1, *it2);
              k.run_for(std::chrono::milliseconds(10));
              if (k.confluent()) {
                register_relation(it1, it2, total_c4);
              } else {
                try_again = true;
              }
            }
            if (try_again) {
              KnuthBendix k;
              k.set_alphabet("ba");
              k.add_rule(*it1, *it2);
              k.run_for(std::chrono::milliseconds(10));
              if (k.confluent()) {
                register_relation(it1, it2, total_c4);
              }
            }
          }
        }
      }
      REQUIRE(total_c4 == 471479);
      REQUIRE(total == 2092035);
    }

    LIBSEMIGROUPS_TEST_CASE(
        "KnuthBendix",
        "016",
        "(fpsemi) hard 2-generated 1-relation semigroups 1 to 10",
        "[extreme][kambites][fpsemigroup][fpsemi][xxx2]") {
      KnuthBendix k;
      k.set_alphabet("bac");
      k.add_rule("a", "cc");
      k.add_rule("c", "bab");
      k.knuth_bendix_by_overlap_length();
      std::cout << k.active_rules() << std::endl;
      REQUIRE(k.active_rules()[0] == rule_type({"", ""}));
    }

    LIBSEMIGROUPS_TEST_CASE(
        "KnuthBendix",
        "017",
        "(fpsemi) Konovalov",
        "[extreme][knuthbendix][fpsemigroup][fpsemi][xxx3]") {
      KnuthBendix k;
      k.set_alphabet("abAB");
      k.add_rule("Abba", "BB");
      k.add_rule("Baab", "AA");
      k.run();
      REQUIRE(k.size() == POSITIVE_INFINITY);
    }

    LIBSEMIGROUPS_TEST_CASE(
        "KnuthBendix",
        "018",
        "(fpsemi) https://math.stackexchange.com/questions/2649807",
        "[knuthbendix][fpsemigroup][fpsemi][fail]") {
      do {
        std::string lphbt = "abcABC";
        std::string invrs = "ABCabc";
        std::cout << std::string(72, '#') << std::endl;

        KnuthBendix k;
        k.set_alphabet(lphbt);
        k.set_identity("");
        k.set_inverses(invrs);

        k.add_rule("aa", "");
        k.add_rule("bbbbbbbbbbb", "");
        k.add_rule("cc", "");
        k.add_rule("abababab", "");
        k.add_rule("abbabbabbabbabbabb", "");
        k.add_rule("abbabaBabaBBabbaB", "");
        k.add_rule("acacac", "");
        k.add_rule("bcbc", "");

        for (size_t i = 1; i < 3; ++i) {
          auto lhs = detail::random_string(lphbt, 100);
          auto rhs = detail::random_string(lphbt, 100);
          k.add_rule(lhs, rhs);
          std::cout << "trying rule " << lhs << " -> " << rhs << std::endl;
        }
        k.run_for(std::chrono::seconds(1));
        if (k.confluent()) {
          size_t const N = k.size();
          std::cout << "k.size() == " << N << std::endl;
          if (k.size() == POSITIVE_INFINITY) {
            break;
          }
        }
      } while (true);
    }
  }  // namespace fpsemigroup
}  // namespace libsemigroups
