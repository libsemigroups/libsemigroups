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

// #define CATCH_CONFIG_ENABLE_PAIR_STRINGMAKER

#include <string>  // for string
#include <vector>  // for vector

#include "catch.hpp"      // for REQUIRE, REQUIRE_NOTHROW, REQUIRE_THROWS_AS
#include "test-main.hpp"  // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/constants.hpp"     // for POSITIVE_INFINITY
#include "libsemigroups/knuth-bendix.hpp"  // for KnuthBendix, operator<<
#include "libsemigroups/report.hpp"        // for ReportGuard

namespace libsemigroups {
  constexpr bool REPORT = false;

  using rule_type = fpsemigroup::KnuthBendix::rule_type;

  template <typename TElementType>
  void delete_gens(std::vector<TElementType>& gens) {
    for (auto x : gens) {
      delete x;
    }
  }
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
      auto rg = ReportGuard();

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
      REQUIRE(kb.nr_active_rules() == 1026);
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
      REQUIRE(kb.nr_active_rules() == 8);
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
      REQUIRE(kb.nr_active_rules() == 194);
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
      REQUIRE(kb.nr_active_rules() == 1731);
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
      REQUIRE(kb.nr_active_rules() == 192);
      REQUIRE(kb.gilman_digraph().nr_nodes() == 332);
      REQUIRE(kb.gilman_digraph().nr_edges() == 533);
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
      REQUIRE(kb.nr_active_rules() == 6);
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
      REQUIRE(kb.nr_active_rules() == 19);
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
      REQUIRE(kb.nr_active_rules() == 1026);
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
      REQUIRE(kb.nr_active_rules() == 47);
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
      REQUIRE(kb.nr_active_rules() == 1026);
      REQUIRE(kb.size() == 10752);

      auto& ad = kb.gilman_digraph();
      REQUIRE(ad.nr_nodes() == 6021);
      REQUIRE(ad.nr_edges() == 7435);
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
        "083",
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
          REQUIRE(kb.nr_active_rules() == 0);
          REQUIRE(kb.size() == 0);
          break;
        }
      } while (std::next_permutation(perm.begin(), perm.end()));
    }
  }  // namespace fpsemigroup
}  // namespace libsemigroups
