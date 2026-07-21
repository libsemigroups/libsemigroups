
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019-2026 James D. Mitchell
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

#define CATCH_CONFIG_ENABLE_ALL_STRINGMAKERS

#include <algorithm>   // for any_of
#include <functional>  // for operator!=
#include <list>        // for operator!=
#include <string>      // for basic_string
#include <utility>     // for forward

#include "Catch2-3.14.0/catch_amalgamated.hpp"  // for SourceLineInfo
#include "test-main.hpp"                        // for LIBSEMIGROUPS_...

#include "libsemigroups/alphabet-class.hpp"        // for Alphabet::init
#include "libsemigroups/alphabet-helpers.hpp"      // for add_letter
#include "libsemigroups/cong-common-helpers.hpp"   // for add_generating...
#include "libsemigroups/constants.hpp"             // for operator==
#include "libsemigroups/knuth-bendix-class.hpp"    // for KnuthBendix
#include "libsemigroups/knuth-bendix-helpers.hpp"  // for add_generating...
#include "libsemigroups/obvinf.hpp"                // for is_obviously_i...
#include "libsemigroups/order.hpp"                 // for LenLexCmp, RPOCmp
#include "libsemigroups/presentation.hpp"          // for add_rule, Presentation
#include "libsemigroups/ranges.hpp"                // for LenLexCmp, RPOCmp
#include "libsemigroups/types.hpp"                 // for congruence_kind
#include "libsemigroups/word-graph-helpers.hpp"    // for topological_sort
#include "libsemigroups/word-graph.hpp"            // for WordGraph::target

#include "libsemigroups/detail/aho-corasick-impl.hpp"  // for begin_search_n...
#include "libsemigroups/detail/cong-common-class.hpp"  // for CongruenceComm...
#include "libsemigroups/detail/eigen.hpp"              // for DenseBase
#include "libsemigroups/detail/fmt.hpp"                // for format
#include "libsemigroups/detail/iterator.hpp"           // for operator+
#include "libsemigroups/detail/knuth-bendix-impl.hpp"  // for KnuthBendixImp...
#include "libsemigroups/detail/print.hpp"              // for to_printable
#include "libsemigroups/detail/report.hpp"             // for report_default
#include "libsemigroups/detail/rewriting-system.hpp"   // for RewritingSyste...
#include "libsemigroups/detail/string.hpp"             // for group_digits
#include "libsemigroups/detail/timer.hpp"              // for string_time
#include "libsemigroups/detail/value-guard.hpp"        // for ValueGuard::Va...

namespace libsemigroups {

  using LenLexTrie = detail::RewritingSystemTrie<LenLexCmp>;
  using RPOTrie    = detail::RewritingSystemTrie<RPOCmp>;
  using RevRPOTrie = detail::RewritingSystemTrie<RevRPOCmp>;

  // (2,3,7;4) group
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "558",
                          "MAF: subgroups/2374",
                          "[quick][maf]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("abB").contains_empty_word(true);

    presentation::add_rule(p, "aa", "");
    presentation::add_rule(p, "bB", "");
    presentation::add_rule(p, "Bb", "");
    presentation::add_rule(p, "bb", "B");
    presentation::add_rule(p, "BaBaBaB", "abababa");
    presentation::add_rule(p, "abaBabaBabaBabaB", "");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb(congruence_kind::onesided, p);

    SECTION("2374") {
      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 40);
      REQUIRE(kb.number_of_classes() == 168);
    }

    SECTION("2374.sub") {
      knuth_bendix::add_generating_pair(kb, "a", "");
      knuth_bendix::add_generating_pair(kb, "abaBa", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 48);
      REQUIRE(kb.number_of_classes() == 21);
    }
  }

  // Infinite cyclic
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "559",
                          "MAF: subgroups/ab1",
                          "[quick][maf]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("aA").contains_empty_word(true);

    presentation::add_rule(p, "aA", "");
    presentation::add_rule(p, "Aa", "");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb(congruence_kind::onesided, p);

    SECTION("ab1") {
      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 2);
      REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
    }

    SECTION("ab1.subind6") {
      knuth_bendix::add_generating_pair(kb, "aaaaaa", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 4);
      REQUIRE(kb.number_of_classes() == 6);
    }
  }

  // Free abelian group of rank 2 - this ordering gives a finite confluent set
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "560",
                          "MAF: subgroups/ab2",
                          "[quick][maf]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("aAbB").contains_empty_word(true);

    presentation::add_rule(p, "aA", "");
    presentation::add_rule(p, "Aa", "");
    presentation::add_rule(p, "bB", "");
    presentation::add_rule(p, "Bb", "");
    presentation::add_rule(p, "ba", "ab");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb(congruence_kind::onesided, p);

    SECTION("ab2") {
      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 8);
      REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
    }

    SECTION("ab2.sub") {
      knuth_bendix::add_generating_pair(kb, "aabbb", "");
      knuth_bendix::add_generating_pair(kb, "aaaabbbbbbbbb", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 14);
      REQUIRE(kb.number_of_classes() == 6);
    }

    SECTION("ab2.sub1") {
      knuth_bendix::add_generating_pair(kb, "aabbb", "");
      knuth_bendix::add_generating_pair(kb, "BBBAA", "");
      knuth_bendix::add_generating_pair(kb, "aaaabbbbbbbbb", "");
      knuth_bendix::add_generating_pair(kb, "BBBBBBBBBAAAA", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 14);
      REQUIRE(kb.number_of_classes() == 6);
    }

    SECTION("ab2.subi") {
      knuth_bendix::add_generating_pair(kb, "aa", "");
      knuth_bendix::add_generating_pair(kb, "AA", "");
      knuth_bendix::add_generating_pair(kb, "bbbbb", "");
      knuth_bendix::add_generating_pair(kb, "BBBBB", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 14);
      REQUIRE(kb.number_of_classes() == 10);
    }
  }

  // Dihedral group of order 6
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "561",
                          "MAF: subgroups/d6",
                          "[quick][maf]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("abB").contains_empty_word(true);

    presentation::add_rule(p, "aa", "");
    presentation::add_rule(p, "bB", "");
    presentation::add_rule(p, "Bb", "");
    presentation::add_rule(p, "bb", "B");
    presentation::add_rule(p, "aba", "B");

    using RewritingSystem = detail::RewritingSystemTrie<RevRPOCmp>;
    KnuthBendix<std::string, RewritingSystem> kb(congruence_kind::onesided, p);

    SECTION("d6") {
      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 4);
      REQUIRE(kb.number_of_classes() == 6);
    }

    SECTION("d6.sub") {
      knuth_bendix::add_generating_pair(kb, "b", "");
      knuth_bendix::add_generating_pair(kb, "B", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 6);
      REQUIRE(kb.number_of_classes() == 2);
    }

    SECTION("d6.sub1") {
      knuth_bendix::add_generating_pair(kb, "a", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 5);
      REQUIRE(kb.number_of_classes() == 3);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "562",
                          "MAF: subgroups/edeson",
                          "[quick][maf]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("yuUvV").contains_empty_word(true);

    presentation::add_rule(p, "yy", "");
    presentation::add_rule(p, "uU", "");
    presentation::add_rule(p, "Uu", "");
    presentation::add_rule(p, "vV", "");
    presentation::add_rule(p, "Vv", "");
    presentation::add_rule(p, "yuyu", "");
    presentation::add_rule(p, "uuu", "");
    presentation::add_rule(p, "yvyv", "");
    presentation::add_rule(p, "vvv", "");
    presentation::add_rule(p, "uvuvuvuvuvuv", "");
    presentation::add_rule(p, "uVuVuVuVuVuV", "");
    presentation::add_rule(p, "yuvyuvyuvyuvyuvyuv", "");
    presentation::add_rule(p, "yuvuvuvyuvuvuvyuvuvuvyuvuvuvyuvuvuvyuvuvuv", "");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb(congruence_kind::onesided, p);

    SECTION("edeson") {
      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 31);
      REQUIRE(kb.number_of_classes() == 54);
    }

    SECTION("edeson.sub") {
      knuth_bendix::add_generating_pair(kb, "yu", "");
      knuth_bendix::add_generating_pair(kb, "v", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 40);
      REQUIRE(kb.number_of_classes() == 3);
    }
  }

  // Involution group of rank 3.
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "563",
                          "MAF: subgroups/inv(3)",
                          "[quick][maf]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("pqr").contains_empty_word(true);

    presentation::add_rule(p, "pp", "");
    presentation::add_rule(p, "qq", "");
    presentation::add_rule(p, "rr", "");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb(congruence_kind::onesided, p);

    SECTION("inv(3)") {
      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 3);
      REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
    }

    SECTION("inv(3).sub") {
      knuth_bendix::add_generating_pair(kb, "qr", "");
      knuth_bendix::add_generating_pair(kb, "rq", "");
      knuth_bendix::add_generating_pair(kb, "rp", "");
      knuth_bendix::add_generating_pair(kb, "pr", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 7);
      REQUIRE(kb.number_of_classes() == 2);
    }

    SECTION("inv(3).sub1") {
      knuth_bendix::add_generating_pair(kb, "qr", "");
      knuth_bendix::add_generating_pair(kb, "rq", "");
      knuth_bendix::add_generating_pair(kb, "rp", "");
      knuth_bendix::add_generating_pair(kb, "pr", "");
      knuth_bendix::add_generating_pair(kb, "qp", "");
      knuth_bendix::add_generating_pair(kb, "pq", "");
      REQUIRE(kb.rewriting_system().number_of_rules() == 3);
      REQUIRE(kb.number_of_classes() == 2);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "564",
                          "MAF: subgroups/klein",
                          "[quick][maf]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("wWuU").contains_empty_word(true);

    presentation::add_rule(p, "wW", "");
    presentation::add_rule(p, "Ww", "");
    presentation::add_rule(p, "uU", "");
    presentation::add_rule(p, "Uu", "");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb(congruence_kind::onesided, p);

    SECTION("klein") {
      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 4);
      REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
    }

    SECTION("klein.sub1") {
      knuth_bendix::add_generating_pair(kb, "UwUwUwUwUwUwUwUwUwUwUwW", "");
      knuth_bendix::add_generating_pair(kb, "UwUwUwUwUwUwUwUwUwUwUwUwWWu", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 8);
      REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
    }

    SECTION("klein.sub2") {
      knuth_bendix::add_generating_pair(
          kb, "uWuuWuWuuWuuWuWuuWuWuuWuuWuWuuWuuWuWuuWuWuuWuuWuWuuWuuW", "");
      knuth_bendix::add_generating_pair(
          kb,
          "uWuuWuWuuWuuWuWuuWuWuuWuuWuWuuWuuWuWuuWuWuuWuuWuWuuWuuWuWuuWuWuuWuuW"
          "uWuuWuuWUwUwUUwUwUUwUUwUwUUwU",
          "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 8);
      REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
    }
  }

  // Group of generator changes for SL(2,C) free groups of rank 2.
  // s = interchange a and b
  // x = interchange a and A
  // y = interchange b and B
  // r = a->b b->AB (meaning matrix that was labelled b is labelled a, and
  // matrix that was labelled AB is labelled b) c = a->a b->abA d = a->baB b->b
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "565",
                          "MAF: subgroups/mcgq",
                          "[quick][maf]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("sxyrRcCdD").contains_empty_word(true);

    presentation::add_rule(p, "ss", "");
    presentation::add_rule(p, "xx", "");
    presentation::add_rule(p, "yy", "");
    presentation::add_rule(p, "rR", "");
    presentation::add_rule(p, "Rr", "");
    presentation::add_rule(p, "cC", "");
    presentation::add_rule(p, "Cc", "");
    presentation::add_rule(p, "dD", "");
    presentation::add_rule(p, "Dd", "");
    presentation::add_rule(p, "rr", "R");
    presentation::add_rule(p, "sxs", "y");
    presentation::add_rule(p, "srs", "Rc");
    presentation::add_rule(p, "scs", "d");
    presentation::add_rule(p, "RCr", "D");
    presentation::add_rule(p, "xy", "yx");
    presentation::add_rule(p, "c", "");
    presentation::add_rule(p, "d", "");
    presentation::add_rule(p, "y", "x");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb(congruence_kind::twosided, p);
    kb.run();
    REQUIRE(kb.rewriting_system().number_of_rules() == 14);
    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
  }

  // This is another presentation of SL(2,Z[i]). See picard for details
  // Generators m,M and j have been removed, since they are not needed.
  // TODO currently RevLenLexCmp isn't a thing, so skipping this until it is.
  // LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
  //                         "566",
  //                         "MAF: subgroups/picard3",
  //                         "[quick][maf]") {
  //   auto rg = ReportGuard(false);

  //   Presentation<std::string> p;
  //   p.alphabet("aAtTuU").contains_empty_word(true);

  //   presentation::add_rule(p, "aA", "");
  //   presentation::add_rule(p, "Aa", "");
  //   presentation::add_rule(p, "tT", "");
  //   presentation::add_rule(p, "Tt", "");
  //   presentation::add_rule(p, "uU", "");
  //   presentation::add_rule(p, "Uu", "");
  //   presentation::add_rule(p, "AuAUau", "aUAuaU");
  //   presentation::add_rule(p, "tat", "aTA");
  //   presentation::add_rule(p, "tauAUau", "auAUauT");
  //   presentation::add_rule(p, "ut", "tu");
  //   presentation::add_rule(p, "aat", "taa");
  //   presentation::add_rule(p, "aau", "uaa");

  //   using RewritingSystem = detail::RewritingSystemTrie<RevLenLexCmp>;
  //   KnuthBendix<std::string, RewritingSystem> kb(congruence_kind::twosided,
  //   p); kb.run(); REQUIRE(kb.rewriting_system().number_of_rules() == 0);
  //   REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
  // }

  // Sym(n) is generated using n-1 generators.
  // a=(12),b=(23),... Each generator multiplied by the next is a 3 cycle
  // but commutes with all the remaining generators.
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "567",
                          "MAF: symmetric/Sym(10)",
                          "[quick][maf]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("abcdefghi").contains_empty_word(true);

    presentation::add_rule(p, "aa", "");
    presentation::add_rule(p, "bb", "");
    presentation::add_rule(p, "cc", "");
    presentation::add_rule(p, "dd", "");
    presentation::add_rule(p, "ee", "");
    presentation::add_rule(p, "ff", "");
    presentation::add_rule(p, "gg", "");
    presentation::add_rule(p, "hh", "");
    presentation::add_rule(p, "ii", "");
    presentation::add_rule(p, "bab", "aba");
    presentation::add_rule(p, "ca", "ac");
    presentation::add_rule(p, "da", "ad");
    presentation::add_rule(p, "ea", "ae");
    presentation::add_rule(p, "fa", "af");
    presentation::add_rule(p, "ga", "ag");
    presentation::add_rule(p, "ha", "ah");
    presentation::add_rule(p, "ia", "ai");
    presentation::add_rule(p, "cbc", "bcb");
    presentation::add_rule(p, "db", "bd");
    presentation::add_rule(p, "eb", "be");
    presentation::add_rule(p, "fb", "bf");
    presentation::add_rule(p, "gb", "bg");
    presentation::add_rule(p, "hb", "bh");
    presentation::add_rule(p, "ib", "bi");
    presentation::add_rule(p, "dcd", "cdc");
    presentation::add_rule(p, "ec", "ce");
    presentation::add_rule(p, "fc", "cf");
    presentation::add_rule(p, "gc", "cg");
    presentation::add_rule(p, "hc", "ch");
    presentation::add_rule(p, "ic", "ci");
    presentation::add_rule(p, "ede", "ded");  // codespell:ignore
    presentation::add_rule(p, "fd", "df");
    presentation::add_rule(p, "gd", "dg");
    presentation::add_rule(p, "hd", "dh");
    presentation::add_rule(p, "id", "di");
    presentation::add_rule(p, "fef", "efe");
    presentation::add_rule(p, "ge", "eg");
    presentation::add_rule(p, "he", "eh");
    presentation::add_rule(p, "ie", "ei");
    presentation::add_rule(p, "gfg", "fgf");
    presentation::add_rule(p, "hf", "fh");
    presentation::add_rule(p, "if", "fi");
    presentation::add_rule(p, "hgh", "ghg");
    presentation::add_rule(p, "ig", "gi");
    presentation::add_rule(p, "ihi", "hih");

    using RewritingSystem = detail::RewritingSystemTrie<RevRPOCmp>;
    KnuthBendix<std::string, RewritingSystem> kb(congruence_kind::onesided, p);

    SECTION("Sym(10)") {
      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 73);
      REQUIRE(kb.number_of_classes() == 3'628'800);
    }

    SECTION("Sym(10).sub") {
      knuth_bendix::add_generating_pair(kb, "acdfghi", "");
      knuth_bendix::add_generating_pair(kb, "ihgfdca", "");
      knuth_bendix::add_generating_pair(kb, "abdefghi", "");
      knuth_bendix::add_generating_pair(kb, "ihgfedba", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 82);
      REQUIRE(kb.number_of_classes() == 1);
    }

    SECTION("Sym(10).sub1") {
      knuth_bendix::add_generating_pair(kb, "a", "");
      knuth_bendix::add_generating_pair(kb, "cd", "");
      knuth_bendix::add_generating_pair(kb, "fghi", "");
      knuth_bendix::add_generating_pair(kb, "ab", "");
      knuth_bendix::add_generating_pair(kb, "defghi", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 82);
      REQUIRE(kb.number_of_classes() == 1);
    }
  }

  // Coxeter Group A15 - Symmetric group Sym(16)
  // Sym(n) is generated using n-1 generators.
  // a=(12),b=(23),... Each generator multiplied by the next is a 3 cycle
  // but commutes with all the remaining generators.
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "568",
                          "MAF: symmetric/Sym(16)",
                          "[quick][maf]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("abcdefghijklmno").contains_empty_word(true);

    presentation::add_rule(p, "aa", "");
    presentation::add_rule(p, "bb", "");
    presentation::add_rule(p, "cc", "");
    presentation::add_rule(p, "dd", "");
    presentation::add_rule(p, "ee", "");
    presentation::add_rule(p, "ff", "");
    presentation::add_rule(p, "gg", "");
    presentation::add_rule(p, "hh", "");
    presentation::add_rule(p, "ii", "");
    presentation::add_rule(p, "jj", "");
    presentation::add_rule(p, "kk", "");
    presentation::add_rule(p, "ll", "");
    presentation::add_rule(p, "mm", "");
    presentation::add_rule(p, "nn", "");
    presentation::add_rule(p, "oo", "");
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
    presentation::add_rule(p, "ede", "ded");  // codespell:ignore
    presentation::add_rule(p, "fd", "df");
    presentation::add_rule(p, "gd", "dg");
    presentation::add_rule(p, "hd", "dh");
    presentation::add_rule(p, "id", "di");
    presentation::add_rule(p, "jd", "dj");
    presentation::add_rule(p, "kd", "dk");
    presentation::add_rule(p, "ld", "dl");
    presentation::add_rule(p, "md", "dm");
    presentation::add_rule(p, "nd", "dn");  // codespell:ignore
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
    presentation::add_rule(p, "of", "fo");  // codespell:ignore
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

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb(congruence_kind::twosided, p);
    kb.run();
    REQUIRE(kb.rewriting_system().number_of_rules() == 211);
    REQUIRE(kb.number_of_classes() == 20'922'789'888'000);
  }

  // Symmetric group Sym(4)
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "569",
                          "MAF: symmetric/Sym(4)",
                          "[quick][maf]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("abB").contains_empty_word(true);

    presentation::add_rule(p, "aa", "");
    presentation::add_rule(p, "bB", "");
    presentation::add_rule(p, "Bb", "");
    presentation::add_rule(p, "bb", "B");
    presentation::add_rule(p, "BaBa", "abab");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb(congruence_kind::onesided, p);

    SECTION("Sym(4)") {
      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 11);
      REQUIRE(kb.number_of_classes() == 24);
    }

    SECTION("Sym(4).sub") {
      knuth_bendix::add_generating_pair(kb, "b", "");
      knuth_bendix::add_generating_pair(kb, "aBaba", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 15);
      REQUIRE(kb.number_of_classes() == 4);
    }
  }

  // Symmetric group Sym(8) as Coxeter group
  // Sym(n) is generated using n-1 generators.
  // a=(12),b=(23),... Each generator multiplied by the next is a 3 cycle
  // but commutes with all the remaining generators.
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "570",
                          "MAF: symmetric/sym(6)",
                          "[quick][maf]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("abcde").contains_empty_word(true);

    presentation::add_rule(p, "aa", "");
    presentation::add_rule(p, "bb", "");
    presentation::add_rule(p, "cc", "");
    presentation::add_rule(p, "dd", "");
    presentation::add_rule(p, "ee", "");
    presentation::add_rule(p, "bab", "aba");
    presentation::add_rule(p, "ca", "ac");
    presentation::add_rule(p, "da", "ad");
    presentation::add_rule(p, "ea", "ae");
    presentation::add_rule(p, "cbc", "bcb");
    presentation::add_rule(p, "db", "bd");
    presentation::add_rule(p, "eb", "be");
    presentation::add_rule(p, "dcd", "cdc");
    presentation::add_rule(p, "ec", "ce");
    presentation::add_rule(p, "ede", "ded");  // codespell:ignore

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb(congruence_kind::twosided, p);
    kb.run();
    REQUIRE(kb.rewriting_system().number_of_rules() == 21);
    REQUIRE(kb.number_of_classes() == 720);
  }

  // Symmetric group Sym(7) as Coxeter group
  // Sym(n) is generated using n-1 generators.
  // a=(12),b=(23),... Each generator multiplied by the next is a 3 cycle
  // but commutes with all the remaining generators.
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "571",
                          "MAF: symmetric/Sym(7)",
                          "[quick][maf]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("abcdef").contains_empty_word(true);

    presentation::add_rule(p, "aa", "");
    presentation::add_rule(p, "bb", "");
    presentation::add_rule(p, "cc", "");
    presentation::add_rule(p, "dd", "");
    presentation::add_rule(p, "ee", "");
    presentation::add_rule(p, "ff", "");
    presentation::add_rule(p, "bab", "aba");
    presentation::add_rule(p, "ca", "ac");
    presentation::add_rule(p, "da", "ad");
    presentation::add_rule(p, "ea", "ae");
    presentation::add_rule(p, "fa", "af");
    presentation::add_rule(p, "cbc", "bcb");
    presentation::add_rule(p, "db", "bd");
    presentation::add_rule(p, "eb", "be");
    presentation::add_rule(p, "fb", "bf");
    presentation::add_rule(p, "dcd", "cdc");
    presentation::add_rule(p, "ec", "ce");
    presentation::add_rule(p, "fc", "cf");
    presentation::add_rule(p, "ede", "ded");  // codespell:ignore
    presentation::add_rule(p, "fd", "df");
    presentation::add_rule(p, "fef", "efe");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb(congruence_kind::onesided, p);

    SECTION("Sym(7)") {
      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 31);
      REQUIRE(kb.number_of_classes() == 5040);
    }

    SECTION("Sym(7).sub1") {
      knuth_bendix::add_generating_pair(kb, "a", "");
      knuth_bendix::add_generating_pair(kb, "abcdef", "");
      knuth_bendix::add_generating_pair(kb, "fedcba", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 37);
      REQUIRE(kb.number_of_classes() == 1);
    }
  }

  // Symmetric group Sym(8) as Coxeter group
  // Sym(n) is generated using n-1 generators.
  // a=(12),b=(23),... Each generator multiplied by the next is a 3 cycle
  // but commutes with all the remaining generators.
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "572",
                          "MAF: symmetric/sym(8)",
                          "[quick][maf]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("abcdefg").contains_empty_word(true);

    presentation::add_rule(p, "aa", "");
    presentation::add_rule(p, "bb", "");
    presentation::add_rule(p, "cc", "");
    presentation::add_rule(p, "dd", "");
    presentation::add_rule(p, "ee", "");
    presentation::add_rule(p, "ff", "");
    presentation::add_rule(p, "gg", "");
    presentation::add_rule(p, "bab", "aba");
    presentation::add_rule(p, "ca", "ac");
    presentation::add_rule(p, "da", "ad");
    presentation::add_rule(p, "ea", "ae");
    presentation::add_rule(p, "fa", "af");
    presentation::add_rule(p, "ga", "ag");
    presentation::add_rule(p, "cbc", "bcb");
    presentation::add_rule(p, "db", "bd");
    presentation::add_rule(p, "eb", "be");
    presentation::add_rule(p, "fb", "bf");
    presentation::add_rule(p, "gb", "bg");
    presentation::add_rule(p, "dcd", "cdc");
    presentation::add_rule(p, "ec", "ce");
    presentation::add_rule(p, "fc", "cf");
    presentation::add_rule(p, "gc", "cg");
    presentation::add_rule(p, "ede", "ded");  // codespell:ignore
    presentation::add_rule(p, "fd", "df");
    presentation::add_rule(p, "gd", "dg");
    presentation::add_rule(p, "fef", "efe");
    presentation::add_rule(p, "ge", "eg");
    presentation::add_rule(p, "gfg", "fgf");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb(congruence_kind::twosided, p);
    kb.run();
    REQUIRE(kb.rewriting_system().number_of_rules() == 43);
    REQUIRE(kb.number_of_classes() == 40'320);
  }

  // Symmetric group Sym(9) as Coxeter group
  // Sym(n) is generated using n-1 generators.
  // a=(12),b=(23),... Each generator multiplied by the next is a 3 cycle
  // but commutes with all the remaining generators.
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "573",
                          "MAF: symmetric/Sym(9)",
                          "[quick][maf]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("abcdefgh").contains_empty_word(true);

    presentation::add_rule(p, "aa", "");
    presentation::add_rule(p, "bb", "");
    presentation::add_rule(p, "cc", "");
    presentation::add_rule(p, "dd", "");
    presentation::add_rule(p, "ee", "");
    presentation::add_rule(p, "ff", "");
    presentation::add_rule(p, "gg", "");
    presentation::add_rule(p, "hh", "");
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
    presentation::add_rule(p, "ede", "ded");  // codespell:ignore
    presentation::add_rule(p, "fd", "df");
    presentation::add_rule(p, "gd", "dg");
    presentation::add_rule(p, "hd", "dh");
    presentation::add_rule(p, "fef", "efe");
    presentation::add_rule(p, "ge", "eg");
    presentation::add_rule(p, "he", "eh");
    presentation::add_rule(p, "gfg", "fgf");
    presentation::add_rule(p, "hf", "fh");
    presentation::add_rule(p, "hgh", "ghg");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb(congruence_kind::onesided, p);

    SECTION("Sym(9)") {
      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 57);
      REQUIRE(kb.number_of_classes() == 362'880);
    }

    SECTION("Sym(9).sub2") {
      knuth_bendix::add_generating_pair(kb, "ab", "");
      knuth_bendix::add_generating_pair(kb, "ba", "");
      knuth_bendix::add_generating_pair(kb, "abcdefgh", "");
      knuth_bendix::add_generating_pair(kb, "hgfedcba", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 71);
      REQUIRE(kb.number_of_classes() == 2);
    }

    SECTION("Sym(9).sub3") {
      knuth_bendix::add_generating_pair(kb, "ab", "");
      knuth_bendix::add_generating_pair(kb, "ba", "");
      knuth_bendix::add_generating_pair(kb, "abcdef", "");
      knuth_bendix::add_generating_pair(kb, "fedcba", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 67);
      REQUIRE(kb.number_of_classes() == 144);
    }
  }
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "574",
                          "MAF: trivial/degen1",
                          "[quick][maf]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("").contains_empty_word(true);

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb(congruence_kind::twosided, p);
    kb.run();
    REQUIRE(kb.rewriting_system().number_of_rules() == 0);
    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "575",
                          "MAF: trivial/degen2",
                          "[quick][maf]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("aA").contains_empty_word(true);

    presentation::add_rule(p, "aA", "");
    presentation::add_rule(p, "Aa", "");
    presentation::add_rule(p, "a", "");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb(congruence_kind::twosided, p);
    kb.run();
    REQUIRE(kb.rewriting_system().number_of_rules() == 2);
    REQUIRE(kb.number_of_classes() == 1);
  }

  // trivial group
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "576",
                          "MAF: trivial/degen3",
                          "[quick][maf]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("aAbB").contains_empty_word(true);

    presentation::add_rule(p, "aA", "");
    presentation::add_rule(p, "Aa", "");
    presentation::add_rule(p, "bB", "");
    presentation::add_rule(p, "Bb", "");
    presentation::add_rule(p, "ab", "");
    presentation::add_rule(p, "abb", "");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb(congruence_kind::twosided, p);
    kb.run();
    REQUIRE(kb.rewriting_system().number_of_rules() == 4);
    REQUIRE(kb.number_of_classes() == 1);
  }

  // trivial group - BHN presentation
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "577",
                          "MAF: trivial/degen4",
                          "[quick][maf]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("aAbBcC").contains_empty_word(true);

    presentation::add_rule(p, "aA", "");
    presentation::add_rule(p, "Aa", "");
    presentation::add_rule(p, "bB", "");
    presentation::add_rule(p, "Bb", "");
    presentation::add_rule(p, "cC", "");
    presentation::add_rule(p, "Cc", "");
    presentation::add_rule(p, "Aba", "bb");
    presentation::add_rule(p, "Bcb", "cc");
    presentation::add_rule(p, "Cac", "aa");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb(congruence_kind::twosided, p);
    kb.run();
    REQUIRE(kb.rewriting_system().number_of_rules() == 6);
    REQUIRE(kb.number_of_classes() == 1);
  }

  // trivial group - BHN presentation
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "578",
                          "MAF: trivial/degen4a",
                          "[quick][maf]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("rRsStT").contains_empty_word(true);

    presentation::add_rule(p, "rR", "");
    presentation::add_rule(p, "Rr", "");
    presentation::add_rule(p, "sS", "");
    presentation::add_rule(p, "Ss", "");
    presentation::add_rule(p, "tT", "");
    presentation::add_rule(p, "Tt", "");
    presentation::add_rule(p, "Trt", "rr");
    presentation::add_rule(p, "Rsr", "ss");
    presentation::add_rule(p, "Sts", "tt");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb(congruence_kind::twosided, p);
    kb.run();
    REQUIRE(kb.rewriting_system().number_of_rules() == 6);
    REQUIRE(kb.number_of_classes() == 1);
  }

  // Generated by MAF
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "579",
                          "MAF: unknown/g9(1_3)",
                          "[quick][maf]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("aAdDgGbBeEhHcCfFiI").contains_empty_word(true);

    presentation::add_rule(p, "aA", "");
    presentation::add_rule(p, "Aa", "");
    presentation::add_rule(p, "dD", "");
    presentation::add_rule(p, "Dd", "");
    presentation::add_rule(p, "gG", "");
    presentation::add_rule(p, "Gg", "");
    presentation::add_rule(p, "bB", "");
    presentation::add_rule(p, "BB", "");
    presentation::add_rule(p, "eE", "");
    presentation::add_rule(p, "Ee", "");
    presentation::add_rule(p, "hH", "");
    presentation::add_rule(p, "Hh", "");
    presentation::add_rule(p, "cC", "");
    presentation::add_rule(p, "Cc", "");
    presentation::add_rule(p, "fF", "");
    presentation::add_rule(p, "Ff", "");
    presentation::add_rule(p, "iI", "");
    presentation::add_rule(p, "II", "");
    presentation::add_rule(p, "ab", "d");
    presentation::add_rule(p, "bc", "e");
    presentation::add_rule(p, "cd", "f");
    presentation::add_rule(p, "de", "g");
    presentation::add_rule(p, "ef", "h");
    presentation::add_rule(p, "fg", "i");
    presentation::add_rule(p, "gh", "a");
    presentation::add_rule(p, "hi", "b");
    presentation::add_rule(p, "ia", "c");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb(congruence_kind::twosided, p);
    kb.run();
    REQUIRE(kb.rewriting_system().number_of_rules() == 18);
    REQUIRE(kb.number_of_classes() == 1);
  }

  // A confluent and automatic presentation of P4GM found by MAF.
  // g is supposed to be vertical glide reflection up about line x=0.25
  // f is supposed to be horizontal glide reflection to the right about line
  // y=0.25
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "580",
                          "MAF: wallpaper/P4GM",
                          "[quick][maf]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("gGfFm").contains_empty_word(true);

    presentation::add_rule(p, "gG", "");
    presentation::add_rule(p, "Gg", "");
    presentation::add_rule(p, "fF", "");
    presentation::add_rule(p, "Ff", "");
    presentation::add_rule(p, "mm", "");
    presentation::add_rule(p, "mG", "fm");
    presentation::add_rule(p, "Fg", "Gf");
    presentation::add_rule(p, "FG", "gf");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb(congruence_kind::twosided, p);
    kb.run();
    REQUIRE(kb.rewriting_system().number_of_rules() == 13);
    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
  }

  // P4MM triangle group presentation
  // b +
  // / |a
  // +---+
  // c
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "581",
                          "MAF: wallpaper/t(4_2_4)",
                          "[quick][maf]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("abc").contains_empty_word(true);

    presentation::add_rule(p, "aa", "");
    presentation::add_rule(p, "bb", "");
    presentation::add_rule(p, "cc", "");
    presentation::add_rule(p, "caca", "");
    presentation::add_rule(p, "babababa", "");
    presentation::add_rule(p, "cbcbcbcb", "");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb(congruence_kind::onesided, p);

    SECTION("t(4_2_4)") {
      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 7);
      REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
      using rule_type = typename decltype(kb)::rule_type;
      REQUIRE((kb.active_rules() | rx::to_vector())
              == std::vector<rule_type>({{"cc", ""},
                                         {"bb", ""},
                                         {"aa", ""},
                                         {"ca", "ac"},
                                         {"cbcb", "bcbc"},
                                         {"cbacbab", "bcbacba"},
                                         {"baba", "abab"}}));
    }

    SECTION("t(4_2_4).sub_C1M") {
      knuth_bendix::add_generating_pair(kb, "abcb", "");
      knuth_bendix::add_generating_pair(kb, "bcba", "");
      knuth_bendix::add_generating_pair(kb, "babc", "");
      knuth_bendix::add_generating_pair(kb, "cbab", "");
      knuth_bendix::add_generating_pair(kb, "b", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 11);
      REQUIRE(kb.number_of_classes() == 4);
    }

    SECTION("t(4_2_4).sub_C2MM") {
      knuth_bendix::add_generating_pair(kb, "abcb", "");
      knuth_bendix::add_generating_pair(kb, "bcba", "");
      knuth_bendix::add_generating_pair(kb, "babc", "");
      knuth_bendix::add_generating_pair(kb, "cbab", "");
      knuth_bendix::add_generating_pair(kb, "bcbc", "");
      knuth_bendix::add_generating_pair(kb, "b", "");
      knuth_bendix::add_generating_pair(kb, "cbc", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 11);
      REQUIRE(kb.number_of_classes() == 2);
    }

    SECTION("t(4_2_4).sub_P") {
      knuth_bendix::add_generating_pair(kb, "abcb", "");
      knuth_bendix::add_generating_pair(kb, "bcba", "");
      knuth_bendix::add_generating_pair(kb, "babc", "");
      knuth_bendix::add_generating_pair(kb, "cbab", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 15);
      REQUIRE(kb.number_of_classes() == 8);
    }

    SECTION("t(4_2_4).sub_P1G") {
      knuth_bendix::add_generating_pair(kb, "cbacba", "");
      knuth_bendix::add_generating_pair(kb, "abcabc", "");
      knuth_bendix::add_generating_pair(kb, "bac", "");
      knuth_bendix::add_generating_pair(kb, "cab", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 15);
      REQUIRE(kb.number_of_classes() == 8);
    }

    SECTION("t(4_2_4).sub_P1M") {
      knuth_bendix::add_generating_pair(kb, "abcb", "");
      knuth_bendix::add_generating_pair(kb, "bcba", "");
      knuth_bendix::add_generating_pair(kb, "babc", "");
      knuth_bendix::add_generating_pair(kb, "cbab", "");
      knuth_bendix::add_generating_pair(kb, "bcb", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 12);
      REQUIRE(kb.number_of_classes() == 4);
    }

    SECTION("t(4_2_4).sub_P2") {
      knuth_bendix::add_generating_pair(kb, "abcb", "");
      knuth_bendix::add_generating_pair(kb, "bcba", "");
      knuth_bendix::add_generating_pair(kb, "babc", "");
      knuth_bendix::add_generating_pair(kb, "cbab", "");
      knuth_bendix::add_generating_pair(kb, "bcbc", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 13);
      REQUIRE(kb.number_of_classes() == 4);
    }

    SECTION("t(4_2_4).sub_P2GG") {
      knuth_bendix::add_generating_pair(kb, "bac", "");
      knuth_bendix::add_generating_pair(kb, "cab", "");
      knuth_bendix::add_generating_pair(kb, "cba", "");
      knuth_bendix::add_generating_pair(kb, "abc", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 12);
      REQUIRE(kb.number_of_classes() == 4);
    }

    SECTION("t(4_2_4).sub_P2MG") {
      knuth_bendix::add_generating_pair(kb, "cabcab", "");
      knuth_bendix::add_generating_pair(kb, "bacbac", "");
      knuth_bendix::add_generating_pair(kb, "cba", "");
      knuth_bendix::add_generating_pair(kb, "abc", "");
      knuth_bendix::add_generating_pair(kb, "aba", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 13);
      REQUIRE(kb.number_of_classes() == 4);
    }

    SECTION("t(4_2_4).sub_P2MG_2") {
      knuth_bendix::add_generating_pair(kb, "ac", "");
      knuth_bendix::add_generating_pair(kb, "bacb", "");
      knuth_bendix::add_generating_pair(kb, "aba", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 13);
      REQUIRE(kb.number_of_classes() == 4);
    }

    SECTION("t(4_2_4).sub_P2MM") {
      knuth_bendix::add_generating_pair(kb, "abcb", "");
      knuth_bendix::add_generating_pair(kb, "bcba", "");
      knuth_bendix::add_generating_pair(kb, "babc", "");
      knuth_bendix::add_generating_pair(kb, "cbab", "");
      knuth_bendix::add_generating_pair(kb, "c", "");
      knuth_bendix::add_generating_pair(kb, "bcb", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 11);
      REQUIRE(kb.number_of_classes() == 2);
    }

    SECTION("t(4_2_4).sub_P2MM_2") {
      knuth_bendix::add_generating_pair(kb, "bcb", "");
      knuth_bendix::add_generating_pair(kb, "c", "");
      knuth_bendix::add_generating_pair(kb, "a", "");
      knuth_bendix::add_generating_pair(kb, "bab", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 11);
      REQUIRE(kb.number_of_classes() == 2);
    }

    SECTION("t(4_2_4).sub_P4") {
      knuth_bendix::add_generating_pair(kb, "abcb", "");
      knuth_bendix::add_generating_pair(kb, "bcba", "");
      knuth_bendix::add_generating_pair(kb, "babc", "");
      knuth_bendix::add_generating_pair(kb, "cbab", "");
      knuth_bendix::add_generating_pair(kb, "bc", "");
      knuth_bendix::add_generating_pair(kb, "cb", "");
      knuth_bendix::add_generating_pair(kb, "bcbc", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 11);
      REQUIRE(kb.number_of_classes() == 2);
    }

    SECTION("t(4_2_4).sub_P4GM_2") {
      knuth_bendix::add_generating_pair(kb, "cbacba", "");
      knuth_bendix::add_generating_pair(kb, "abcabc", "");
      knuth_bendix::add_generating_pair(kb, "cabcab", "");
      knuth_bendix::add_generating_pair(kb, "bacbac", "");
      knuth_bendix::add_generating_pair(kb, "bac", "");
      knuth_bendix::add_generating_pair(kb, "cab", "");
      knuth_bendix::add_generating_pair(kb, "cba", "");
      knuth_bendix::add_generating_pair(kb, "abc", "");
      knuth_bendix::add_generating_pair(kb, "ab", "");
      knuth_bendix::add_generating_pair(kb, "ba", "");
      knuth_bendix::add_generating_pair(kb, "abab", "");
      knuth_bendix::add_generating_pair(kb, "c", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 11);
      REQUIRE(kb.number_of_classes() == 2);
    }

    SECTION("t(4_2_4).sub_P4GM_fgm") {
      knuth_bendix::add_generating_pair(kb, "bac", "");
      knuth_bendix::add_generating_pair(kb, "cab", "");
      knuth_bendix::add_generating_pair(kb, "cba", "");
      knuth_bendix::add_generating_pair(kb, "abc", "");
      knuth_bendix::add_generating_pair(kb, "c", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 11);
      REQUIRE(kb.number_of_classes() == 2);
    }

    SECTION("t(4_2_4).sub_P4GM_fgq") {
      knuth_bendix::add_generating_pair(kb, "bac", "");
      knuth_bendix::add_generating_pair(kb, "cab", "");
      knuth_bendix::add_generating_pair(kb, "cba", "");
      knuth_bendix::add_generating_pair(kb, "abc", "");
      knuth_bendix::add_generating_pair(kb, "ab", "");
      knuth_bendix::add_generating_pair(kb, "ba", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 11);
      REQUIRE(kb.number_of_classes() == 2);
    }

    SECTION("t(4_2_4).sub_P4GM_mq") {
      knuth_bendix::add_generating_pair(kb, "ab", "");
      knuth_bendix::add_generating_pair(kb, "ba", "");
      knuth_bendix::add_generating_pair(kb, "c", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 11);
      REQUIRE(kb.number_of_classes() == 2);
    }

    SECTION("t(4_2_4).sub_P4MM") {
      knuth_bendix::add_generating_pair(kb, "abcb", "");
      knuth_bendix::add_generating_pair(kb, "bcba", "");
      knuth_bendix::add_generating_pair(kb, "babc", "");
      knuth_bendix::add_generating_pair(kb, "cbab", "");
      knuth_bendix::add_generating_pair(kb, "bc", "");
      knuth_bendix::add_generating_pair(kb, "cb", "");
      knuth_bendix::add_generating_pair(kb, "bcbc", "");
      knuth_bendix::add_generating_pair(kb, "b", "");
      knuth_bendix::add_generating_pair(kb, "cbc", "");
      knuth_bendix::add_generating_pair(kb, "c", "");
      knuth_bendix::add_generating_pair(kb, "bcb", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 10);
      REQUIRE(kb.number_of_classes() == 1);
    }

    SECTION("t(4_2_4).sub_P4MM_1") {
      knuth_bendix::add_generating_pair(kb, "abcb", "");
      knuth_bendix::add_generating_pair(kb, "bcba", "");
      knuth_bendix::add_generating_pair(kb, "babc", "");
      knuth_bendix::add_generating_pair(kb, "cbab", "");
      knuth_bendix::add_generating_pair(kb, "bc", "");
      knuth_bendix::add_generating_pair(kb, "cb", "");
      knuth_bendix::add_generating_pair(kb, "bcb", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 10);
      REQUIRE(kb.number_of_classes() == 1);
    }

    // NOTE: The next two contain generators not in the alphabet, so are
    // omitted.
    //
    // SECTION("t(4_2_4).sub_P4MM_1.rws.sub") {
    //   knuth_bendix::add_generating_pair(kb, "h", "");
    //   knuth_bendix::add_generating_pair(kb, "H", "");
    //   knuth_bendix::add_generating_pair(kb, "v", "");
    //   knuth_bendix::add_generating_pair(kb, "V", "");
    //   knuth_bendix::add_generating_pair(kb, "y", "");

    //   kb.run();
    //   REQUIRE(kb.rewriting_system().number_of_rules() == 10);
    //   REQUIRE(kb.number_of_classes() == 1);
    // }

    // SECTION("t(4_2_4).sub_P4MM_1.rws.sub1") {
    //   knuth_bendix::add_generating_pair(kb, "h", "");
    //   knuth_bendix::add_generating_pair(kb, "H", "");
    //   knuth_bendix::add_generating_pair(kb, "v", "");
    //   knuth_bendix::add_generating_pair(kb, "V", "");

    //   kb.run();
    //   REQUIRE(kb.rewriting_system().number_of_rules() == 10);
    //   REQUIRE(kb.number_of_classes() == 1);
    // }

    SECTION("t(4_2_4).sub_P4MM_2") {
      knuth_bendix::add_generating_pair(kb, "cbacba", "");
      knuth_bendix::add_generating_pair(kb, "abcabc", "");
      knuth_bendix::add_generating_pair(kb, "cabcab", "");
      knuth_bendix::add_generating_pair(kb, "bacbac", "");
      knuth_bendix::add_generating_pair(kb, "ab", "");
      knuth_bendix::add_generating_pair(kb, "ba", "");
      knuth_bendix::add_generating_pair(kb, "abab", "");
      knuth_bendix::add_generating_pair(kb, "a", "");
      knuth_bendix::add_generating_pair(kb, "bab", "");
      knuth_bendix::add_generating_pair(kb, "b", "");
      knuth_bendix::add_generating_pair(kb, "aba", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 10);
      REQUIRE(kb.number_of_classes() == 2);
    }

    SECTION("t(4_2_4).sub_p1m_2") {
      knuth_bendix::add_generating_pair(kb, "babc", "");
      knuth_bendix::add_generating_pair(kb, "cbab", "");
      knuth_bendix::add_generating_pair(kb, "bcb", "");
      knuth_bendix::add_generating_pair(kb, "a", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 12);
      REQUIRE(kb.number_of_classes() == 4);
    }

    SECTION("t(4_2_4).sub_power") {
      knuth_bendix::add_generating_pair(kb, "abcabcabcabc", "");
      knuth_bendix::add_generating_pair(kb, "acbacbacbacb", "");
      knuth_bendix::add_generating_pair(kb, "abcbabcbabcbabcb", "");
      knuth_bendix::add_generating_pair(kb, "babcbabcbabcbabc", "");
      knuth_bendix::add_generating_pair(kb, "babcbbabcbbabcbbabcb", "");
      knuth_bendix::add_generating_pair(kb, "bcbabbcbabbcbabbcbab", "");
      knuth_bendix::add_generating_pair(kb, "bcbacbcbacbcbacbcbac", "");
      knuth_bendix::add_generating_pair(kb, "babcbcbabcbcbabcbcbabcbc", "");
      knuth_bendix::add_generating_pair(kb, "abcbacbabcbacbabcbacbabcbacb", "");
      knuth_bendix::add_generating_pair(
          kb, "ababcbabcbacbcababcbabcbacbcababcbabcbacbcababcbabcbacbc", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 31);
      REQUIRE(kb.number_of_classes() == 64);
    }
  }

  // NOTE the next test case is a duplicate of the previous

  // P4MM triangle group presentation
  // b +
  // / |a
  // +---+
  // c
  // LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
  //                         "582",
  //                         "MAF: wallpaper/t(4_2_4)mon",
  //                         "[quick][maf]") {
  //   auto rg = ReportGuard(true);

  //   Presentation<std::string> p;
  //   p.alphabet("abc").contains_empty_word(true);

  //   presentation::add_rule(p, "aa", "");
  //   presentation::add_rule(p, "bb", "");
  //   presentation::add_rule(p, "cc", "");
  //   presentation::add_rule(p, "caca", "");
  //   presentation::add_rule(p, "babababa", "");
  //   presentation::add_rule(p, "cbcbcbcb", "");

  //   using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
  //   KnuthBendix<std::string, RewritingSystem> kb(congruence_kind::twosided,
  //   p); kb.run(); REQUIRE(kb.rewriting_system().number_of_rules() == );
  //   REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
  // }

  // P6MM triangle group presentation
  // b +
  // / |a
  // +---+
  // c
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "583",
                          "MAF: wallpaper/t(6_2_3)",
                          "[maf][quick]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("abc").contains_empty_word(true);

    presentation::add_rule(p, "aa", "");
    presentation::add_rule(p, "bb", "");
    presentation::add_rule(p, "cc", "");
    presentation::add_rule(p, "bcbcbcbcbcbc", "");
    presentation::add_rule(p, "caca", "");
    presentation::add_rule(p, "ababab", "");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb(congruence_kind::onesided, p);

    SECTION("t(6_2_3)") {
      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 7);
      REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
      using rule_type = typename decltype(kb)::rule_type;
      REQUIRE((kb.active_rules() | rx::to_vector())
              == std::vector<rule_type>({{"cc", ""},
                                         {"bb", ""},
                                         {"aa", ""},
                                         {"ca", "ac"},
                                         {"bab", "aba"},
                                         {"cbcbcb", "bcbcbc"},
                                         {"cbcbacba", "bcbcbacb"}}));
    }

    SECTION("t(6_2_3).sub_P3MM") {
      knuth_bendix::add_generating_pair(kb, "a", "");
      knuth_bendix::add_generating_pair(kb, "b", "");
      knuth_bendix::add_generating_pair(kb, "cbc", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 10);
      REQUIRE(kb.number_of_classes() == 2);
    }

    SECTION("t(6_2_3).sub_P3_1") {
      knuth_bendix::add_generating_pair(kb, "cbcbab", "");
      knuth_bendix::add_generating_pair(kb, "babcbc", "");
      knuth_bendix::add_generating_pair(kb, "cbabcb", "");
      knuth_bendix::add_generating_pair(kb, "bcbabc", "");
      knuth_bendix::add_generating_pair(kb, "bcbc", "");
      knuth_bendix::add_generating_pair(kb, "cbcb", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 11);
      REQUIRE(kb.number_of_classes() == 4);
    }

    SECTION("t(6_2_3).sub_P3_2") {
      knuth_bendix::add_generating_pair(kb, "cbcbab", "");
      knuth_bendix::add_generating_pair(kb, "babcbc", "");
      knuth_bendix::add_generating_pair(kb, "cbabcb", "");
      knuth_bendix::add_generating_pair(kb, "bcbabc", "");
      knuth_bendix::add_generating_pair(kb, "ab", "");
      knuth_bendix::add_generating_pair(kb, "ba", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 11);
      REQUIRE(kb.number_of_classes() == 4);
    }

    SECTION("t(6_2_3).sub_P6") {
      knuth_bendix::add_generating_pair(kb, "bc", "");
      knuth_bendix::add_generating_pair(kb, "cb", "");
      knuth_bendix::add_generating_pair(kb, "ca", "");
      knuth_bendix::add_generating_pair(kb, "ac", "");
      knuth_bendix::add_generating_pair(kb, "ab", "");
      knuth_bendix::add_generating_pair(kb, "ba", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 11);
      REQUIRE(kb.number_of_classes() == 2);
    }

    SECTION("t(6_2_3).sub_P6MM") {
      knuth_bendix::add_generating_pair(kb, "abcbcb", "");
      knuth_bendix::add_generating_pair(kb, "bcbcba", "");
      knuth_bendix::add_generating_pair(kb, "babcbc", "");
      knuth_bendix::add_generating_pair(kb, "cbcbab", "");
      knuth_bendix::add_generating_pair(kb, "bc", "");
      knuth_bendix::add_generating_pair(kb, "cb", "");
      knuth_bendix::add_generating_pair(kb, "c", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 10);
      REQUIRE(kb.number_of_classes() == 1);
    }

    SECTION("t(6_2_3).sub_P6MM_1") {
      knuth_bendix::add_generating_pair(kb, "cbcbab", "");
      knuth_bendix::add_generating_pair(kb, "babcbc", "");
      knuth_bendix::add_generating_pair(kb, "cbabcb", "");
      knuth_bendix::add_generating_pair(kb, "bcbabc", "");
      knuth_bendix::add_generating_pair(kb, "bc", "");
      knuth_bendix::add_generating_pair(kb, "cb", "");
      knuth_bendix::add_generating_pair(kb, "c", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 10);
      REQUIRE(kb.number_of_classes() == 1);
    }

    SECTION("t(6_2_3).sub_P6_1") {
      knuth_bendix::add_generating_pair(kb, "cbcbab", "");
      knuth_bendix::add_generating_pair(kb, "babcbc", "");
      knuth_bendix::add_generating_pair(kb, "cbabcb", "");
      knuth_bendix::add_generating_pair(kb, "bcbabc", "");
      knuth_bendix::add_generating_pair(kb, "bc", "");
      knuth_bendix::add_generating_pair(kb, "cb", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 11);
      REQUIRE(kb.number_of_classes() == 2);
    }

    SECTION("t(6_2_3).sub_p3") {
      knuth_bendix::add_generating_pair(kb, "bcbc", "");
      knuth_bendix::add_generating_pair(kb, "cbcb", "");
      knuth_bendix::add_generating_pair(kb, "ab", "");
      knuth_bendix::add_generating_pair(kb, "ba", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 11);
      REQUIRE(kb.number_of_classes() == 4);
    }

    SECTION("t(6_2_3).sub_power") {
      knuth_bendix::add_generating_pair(kb, "abcabcabcabcabcabc", "");
      knuth_bendix::add_generating_pair(kb, "acbacbacbacbacbacb", "");
      knuth_bendix::add_generating_pair(
          kb, "abacbabacbabacbabacbabacbabacb", "");
      knuth_bendix::add_generating_pair(
          kb, "abcbcabcbcabcbcabcbcabcbcabcbc", "");
      knuth_bendix::add_generating_pair(
          kb, "acbcbacbcbacbcbacbcbacbcbacbcb", "");
      knuth_bendix::add_generating_pair(
          kb,
          "acbacbcbacbacbacbcbacbacbacbcbacbacbacbcbacbacbacbcbacbacbacbcbacb",
          "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 29);
      REQUIRE(kb.number_of_classes() == 108);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "584",
                          "MAF: coxeter/hermiller2",
                          "[quick][maf]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("jihgfedcba").contains_empty_word(true);

    presentation::add_rule(p, "jj", "");
    presentation::add_rule(p, "ii", "");
    presentation::add_rule(p, "hh", "");
    presentation::add_rule(p, "gg", "");
    presentation::add_rule(p, "ff", "");
    presentation::add_rule(p, "ee", "");
    presentation::add_rule(p, "dd", "");
    presentation::add_rule(p, "cc", "");
    presentation::add_rule(p, "bb", "");
    presentation::add_rule(p, "aa", "");
    presentation::add_rule(p, "abababab", "");
    presentation::add_rule(p, "acacacac", "");
    presentation::add_rule(p, "adadad", "");
    presentation::add_rule(p, "bcbcbc", "");
    presentation::add_rule(p, "bdbdbdbd", "");
    presentation::add_rule(p, "cdcdcdcd", "");
    presentation::add_rule(p, "abab", "e");
    presentation::add_rule(p, "acac", "f");
    presentation::add_rule(p, "ada", "g");
    presentation::add_rule(p, "bcb", "h");
    presentation::add_rule(p, "bdbd", "i");
    presentation::add_rule(p, "cdcd", "j");

    using RewritingSystem       = detail::RewritingSystemTrie<WtLexCmp>;
    std::vector<size_t> weights = {4, 4, 3, 3, 4, 4, 1, 1, 1, 1};
    KnuthBendix<std::string, RewritingSystem> kb(
        congruence_kind::twosided,
        p,
        std::vector<size_t>({4, 4, 3, 3, 4, 4, 1, 1, 1, 1}));
    REQUIRE(kb.rewriting_system().number_of_rules() == 22);
    REQUIRE(!kb.rewriting_system().confluent());
    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
    using rule_type = typename decltype(kb)::rule_type;
    // codespell:begin-ignore
    REQUIRE((kb.active_rules() | rx::to_vector())
            == std::vector<rule_type>(
                {{"aa", ""},       {"abab", "e"},    {"acac", "f"},
                 {"ada", "g"},     {"bb", ""},       {"bcb", "h"},
                 {"bdbd", "i"},    {"cc", ""},       {"cdcd", "j"},
                 {"dd", ""},       {"ee", ""},       {"ff", ""},
                 {"gg", ""},       {"hh", ""},       {"ii", ""},
                 {"jj", ""},       {"abah", "ecb"},  {"abai", "edbd"},
                 {"acaj", "fdcd"}, {"ade", "gbab"},  {"adf", "gcac"},
                 {"ae", "bab"},    {"af", "cac"},    {"ag", "da"},
                 {"bci", "hdbd"},  {"bh", "cb"},     {"bi", "dbd"},
                 {"cbc", "h"},     {"cj", "dcd"},    {"dad", "g"},
                 {"eb", "aba"},    {"fc", "aca"},    {"ga", "ad"},
                 {"hb", "bc"},     {"id", "bdb"},    {"jd", "cdc"},
                 {"acah", "fbc"},  {"gd", "da"},     {"bdbg", "iad"},
                 {"hc", "cb"},     {"cbj", "hdcd"},  {"cdcg", "jad"},
                 {"ch", "bc"},     {"dg", "ad"},     {"eh", "abacb"},
                 {"ei", "abadbd"}, {"fh", "acabc"},  {"fj", "acadcd"},
                 {"ge", "adbab"},  {"gf", "adcac"},  {"hi", "bcdbd"},
                 {"hj", "cbdcd"},  {"ea", "bab"},    {"fa", "cac"},
                 {"ib", "dbd"},    {"ig", "bdbad"},  {"jg", "cdcad"},
                 {"baba", "e"},    {"caca", "f"},    {"babf", "ecac"},
                 {"babg", "eda"},  {"dbdb", "i"},    {"cace", "fbab"},
                 {"cacg", "fda"},  {"jc", "dcd"},    {"dbdh", "icb"},
                 {"ef", "babcac"}, {"eg", "babda"},  {"fe", "cacbab"},
                 {"fg", "cacda"},  {"ih", "dbdcb"},  {"di", "bdb"},
                 {"bce", "haba"},  {"be", "aba"},    {"cbf", "haca"},
                 {"cf", "aca"},    {"dai", "gbdb"},  {"gi", "dabdb"},
                 {"he", "bcaba"},  {"hf", "cbaca"},  {"ie", "dbdaba"},
                 {"dcdc", "j"},    {"cdci", "jbdb"}, {"dbde", "iaba"},
                 {"dcdh", "jbc"},  {"jf", "dcdaca"}, {"jh", "dcdbc"},
                 {"ji", "cdcbdb"}, {"dcdf", "jaca"}, {"dj", "cdc"},
                 {"bdbj", "icdc"}, {"daj", "gcdc"},  {"gj", "dacdc"},
                 {"ij", "bdbcdc"}}));
    // codespell:end-ignore

    WtLexCmp cmp{p.alphabet_v4(), weights};
    for (auto const& rule : kb.active_rules()) {
      REQUIRE(cmp(rule.second, rule.first));
    }
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "585",
                          "MAF: recursive/conder",
                          "[maf][quick]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("aAbBcdef").contains_empty_word(true);

    presentation::add_rule(p, "aA", "");
    presentation::add_rule(p, "Aa", "");
    presentation::add_rule(p, "bB", "");
    presentation::add_rule(p, "Bb", "");
    presentation::add_rule(p, "cc", "");
    presentation::add_rule(p, "dd", "");
    presentation::add_rule(p, "ee", "");
    presentation::add_rule(p, "ff", "");
    presentation::add_rule(p, "aaa", "");
    presentation::add_rule(p, "bbb", "");
    presentation::add_rule(p, "acacac", "");
    presentation::add_rule(p, "adadad", "");
    presentation::add_rule(p, "aeaeae", "");
    presentation::add_rule(p, "afafaf", "");
    presentation::add_rule(p, "bcbcbc", "");
    presentation::add_rule(p, "bdbdbd", "");
    presentation::add_rule(p, "bebebe", "");
    presentation::add_rule(p, "bfbfbf", "");
    presentation::add_rule(p, "abAcabAc", "");
    presentation::add_rule(p, "abAdabAd", "");
    presentation::add_rule(p, "AbaeAbae", "");
    presentation::add_rule(p, "AbafAbaf", "");
    presentation::add_rule(p, "baBcbaBc", "");
    presentation::add_rule(p, "BabdBabd", "");
    presentation::add_rule(p, "baBebaBe", "");
    presentation::add_rule(p, "BabfBabf", "");

    using RewritingSystem = detail::RewritingSystemTrie<WreathCmp>;
    std::vector<size_t>                       levels = {1, 1, 2, 2, 3, 3, 3, 3};
    KnuthBendix<std::string, RewritingSystem> kb(
        congruence_kind::twosided,
        p,
        std::vector<size_t>({1, 1, 2, 2, 3, 3, 3, 3}));
    kb.run();
    REQUIRE(kb.rewriting_system().number_of_rules() == 76);
    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
    using rule_type = typename decltype(kb)::rule_type;
    // codespell:begin-ignore
    REQUIRE((kb.active_rules() | rx::to_vector())
            == std::vector<rule_type>({{"ff", ""},
                                       {"ee", ""},
                                       {"dd", ""},
                                       {"cc", ""},
                                       {"Bb", ""},
                                       {"bB", ""},
                                       {"Aa", ""},
                                       {"aA", ""},
                                       {"bb", "B"},
                                       {"aa", "A"},
                                       {"BB", "b"},
                                       {"ebe", "BeB"},
                                       {"cbc", "BcB"},
                                       {"AA", "a"},
                                       {"dad", "AdA"},
                                       {"cac", "AcA"},
                                       {"eBe", "beb"},
                                       {"BAd", "AdabA"},
                                       {"BAc", "AcabA"},
                                       {"Baf", "afAba"},
                                       {"Bae", "aeAba"},
                                       {"fAf", "afa"},
                                       {"eAe", "aea"},
                                       {"bae", "aeABa"},
                                       {"baf", "afABa"},
                                       {"bAc", "AcaBA"},
                                       {"bAd", "AdaBA"},
                                       {"eae", "AeA"},
                                       {"ABc", "BcbaB"},
                                       {"faf", "AfA"},
                                       {"ABe", "BebaB"},
                                       {"Abf", "bfBab"},
                                       {"Abd", "bdBab"},
                                       {"abd", "bdBAb"},
                                       {"abf", "bfBAb"},
                                       {"aBe", "BebAB"},
                                       {"cBc", "bcb"},
                                       {"aBc", "BcbAB"},
                                       {"dAd", "ada"},
                                       {"cAc", "aca"},
                                       {"cbac", "abcBaBa"},
                                       {"dbad", "abadAbaBa"},
                                       {"fbAf", "AbAfabABA"},
                                       {"Bac", "abcBaba"},
                                       {"Abc", "bacAbab"},
                                       {"abe", "bAeabAb"},
                                       {"BAf", "AbAfabAbA"},
                                       {"cabc", "bacAbAb"},
                                       {"Bf", "abAfabaB"},
                                       {"fbf", "abAfabab"},
                                       {"Bad", "abadAbaba"},
                                       {"babc", "acABAb"},
                                       {"fabAf", "bfBABA"},
                                       {"eabAe", "bAeabABA"},
                                       {"Abe", "abAeabAb"},
                                       {"BAe", "abAeababA"},
                                       {"ebAe", "abAeabaBA"},
                                       {"fAbAf", "bAfabaBA"},
                                       {"AbAe", "beBaBA"},
                                       {"BAbAf", "bAfaBaBA"},
                                       {"dAbad", "bdBaBa"},
                                       {"babAf", "fbABA"},
                                       {"Bd", "AbadAbAB"},
                                       {"dabad", "badAbABa"},
                                       {"dbd", "AbadAbAb"},
                                       {"Babc", "bacABAb"},
                                       {"BabAf", "bfbABA"},
                                       {"Babad", "badABABa"},
                                       {"babAe", "AeaBABA"},
                                       {"abac", "bcBABa"},
                                       {"Abac", "abcBABa"},
                                       {"BAbad", "bdbaBa"},
                                       {"bAbAf", "AfaBaBA"},
                                       {"babad", "adABABa"},
                                       {"bAbad", "dbaBa"},
                                       {"BabAe", "bAeaBABA"}}));

    WreathCmp cmp{p.alphabet_v4(), levels};
    for (auto const& rule : kb.active_rules()) {
      REQUIRE(cmp(rule.second, rule.first));
    }
    // codespell:end-ignore
  }

  // A presentation of the antislice group.
  // This is a set of independent axioms
  // It is easy to show this for all but the three indented axioms by commenting
  // out axioms one at a time - the system will still be confluent quickly but
  // gives a larger group. For the other three axioms, we can find finite
  // index subgroups which are infinite
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "586",
                          "MAF: rubik/antislice",
                          "[quick][maf][rubik]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("abcdef").contains_empty_word(true);

    presentation::add_rule(p, "ab", "");
    presentation::add_rule(p, "ba", "");
    presentation::add_rule(p, "cd", "");
    presentation::add_rule(p, "dc", "");
    presentation::add_rule(p, "ef", "");
    presentation::add_rule(p, "fe", "");
    presentation::add_rule(p, "cafa", "bebd");
    presentation::add_rule(p, "cafb", "aebd");
    presentation::add_rule(p, "ceca", "bdfd");
    presentation::add_rule(p, "cecb", "adfd");
    presentation::add_rule(p, "ceda", "bcfd");
    presentation::add_rule(p, "cfca", "bded");
    presentation::add_rule(p, "eaca", "bdbf");
    presentation::add_rule(p, "eacb", "adbf");
    presentation::add_rule(p, "eada", "bcbf");
    presentation::add_rule(p, "eaec", "dfbf");
    presentation::add_rule(p, "eaed", "cfbf");
    presentation::add_rule(p, "eafc", "debf");
    presentation::add_rule(p, "cacaca", "bdbdbd");
    presentation::add_rule(p, "ceacea", "aecaec");
    presentation::add_rule(p, "ceadea", "aedaec");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb;

    // SECTION("antislice") {  // fails 30s
    //   kb.init(congruence_kind::twosided, p);
    //   kb.run();
    //   REQUIRE(kb.rewriting_system().number_of_rules() == 0);
    //   REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
    // }

    // SECTION("antislice.sub") {  // fails 10s
    //   kb.init(congruence_kind::onesided, p);
    //   knuth_bendix::add_generating_pair(kb, "ceda", "");
    //   knuth_bendix::add_generating_pair(kb, "eafc", "");

    //   kb.run();
    //   REQUIRE(kb.rewriting_system().number_of_rules() == 0);
    //   REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
    // }

    SECTION("antislice.sub1") {
      kb.init(congruence_kind::twosided, p);
      knuth_bendix::add_generating_pair(kb, "eaed", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 408);
      REQUIRE(kb.number_of_classes() == 768);
    }

    SECTION("antislice.sub2") {
      kb.init(congruence_kind::twosided, p);
      knuth_bendix::add_generating_pair(kb, "eafc", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 104);
      REQUIRE(kb.number_of_classes() == 192);
    }

    // SECTION("antislice.sub3") {  // fails 30s
    //   kb.init(congruence_kind::onesided, p);
    //   knuth_bendix::add_generating_pair(kb, "aa", "");
    //   knuth_bendix::add_generating_pair(kb, "cc", "");
    //   knuth_bendix::add_generating_pair(kb, "ee", "");

    //   kb.run();
    //   REQUIRE(kb.rewriting_system().number_of_rules() == 0);
    //   REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
    // }

    // SECTION("antislice.sub4") {  // fails 10s
    //   kb.init(congruence_kind::onesided, p);
    //   knuth_bendix::add_generating_pair(kb, "a", "");
    //   knuth_bendix::add_generating_pair(kb, "b", "");
    //   knuth_bendix::add_generating_pair(kb, "c", "");
    //   knuth_bendix::add_generating_pair(kb, "d", "");

    //   kb.run();
    //   REQUIRE(kb.rewriting_system().number_of_rules() == 0);
    //   REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
    // }
  }

  // This is another set of independent axioms for the antislice group
  // For all but the indented axiom the system is confluent to a larger
  // finite group when that axiom is commented out. Coset enumeration
  // over antislice_alt.sub5 will show the independence of the indented
  // axiom when that is commented out.
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "587",
                          "MAF: rubik/antislice_alt",
                          "[quick][maf][rubik]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("abcdef").contains_empty_word(true);

    presentation::add_rule(p, "ab", "");
    presentation::add_rule(p, "ba", "");
    presentation::add_rule(p, "cd", "");
    presentation::add_rule(p, "dc", "");
    presentation::add_rule(p, "ef", "");
    presentation::add_rule(p, "fe", "");
    presentation::add_rule(p, "aaa", "b");
    presentation::add_rule(p, "ccc", "d");
    presentation::add_rule(p, "eee", "f");
    presentation::add_rule(p, "aacca", "ccb");
    presentation::add_rule(p, "aaeea", "eeb");
    presentation::add_rule(p, "caea", "bfbd");
    presentation::add_rule(p, "ceca", "bdfd");
    presentation::add_rule(p, "deca", "bdfc");
    presentation::add_rule(p, "cfda", "bced");
    presentation::add_rule(p, "ebca", "bdaf");
    presentation::add_rule(p, "edea", "bfcf");
    presentation::add_rule(p, "fcea", "bfde");
    presentation::add_rule(p, "ecfb", "aedf");
    presentation::add_rule(p, "aaecaec", "fdbfd");
    presentation::add_rule(p, "cacaca", "bdbdbd");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb;

    // SECTION("antislice_alt") {  // fails 30s
    //   kb.init(congruence_kind::twosided, p);
    //   kb.run();
    //   REQUIRE(kb.rewriting_system().number_of_rules() == 0);
    //   REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
    // }

    // SECTION("antislice_alt.sub") {  // fails 10s
    //   kb.init(congruence_kind::onesided, p);
    //   knuth_bendix::add_generating_pair(kb, "ceda", "");
    //   knuth_bendix::add_generating_pair(kb, "eafc", "");

    //   kb.run();
    //   REQUIRE(kb.rewriting_system().number_of_rules() == 0);
    //   REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
    // }

    SECTION("antislice_alt.sub1") {
      kb.init(congruence_kind::twosided, p);
      knuth_bendix::add_generating_pair(kb, "eaed", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 408);
      REQUIRE(kb.number_of_classes() == 768);
    }

    SECTION("antislice_alt.sub2") {
      kb.init(congruence_kind::twosided, p);
      knuth_bendix::add_generating_pair(kb, "eafc", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 104);
      REQUIRE(kb.number_of_classes() == 192);
    }

    // SECTION("antislice_alt.sub3") {  // fails 10s
    //   kb.init(congruence_kind::onesided, p);
    //   knuth_bendix::add_generating_pair(kb, "aa", "");
    //   knuth_bendix::add_generating_pair(kb, "cc", "");
    //   knuth_bendix::add_generating_pair(kb, "ee", "");

    //  kb.run();
    //  REQUIRE(kb.rewriting_system().number_of_rules() == 0);
    //  REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
    //}

    // SECTION("antislice_alt.sub4") {  // fails 10s
    //   kb.init(congruence_kind::onesided, p);
    //   knuth_bendix::add_generating_pair(kb, "a", "");
    //   knuth_bendix::add_generating_pair(kb, "b", "");
    //   knuth_bendix::add_generating_pair(kb, "c", "");
    //   knuth_bendix::add_generating_pair(kb, "d", "");

    //  kb.run();
    //  REQUIRE(kb.rewriting_system().number_of_rules() == 0);
    //  REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
    //}

    // SECTION("antislice_alt.sub5") {  // fails 10s
    //   kb.init(congruence_kind::onesided, p);
    //   knuth_bendix::add_generating_pair(kb, "ecae", "");
    //   knuth_bendix::add_generating_pair(kb, "caec", "");

    //  kb.run();
    //  REQUIRE(kb.rewriting_system().number_of_rules() == 0);
    //  REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
    //}
  }

  // Generated by MAF
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "588",
                          "MAF: onerelq/q18",
                          "[quick][maf][onerelq]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("uUvV").contains_empty_word(true);

    presentation::add_rule(p, "uU", "");
    presentation::add_rule(p, "Uu", "");
    presentation::add_rule(p, "vV", "");
    presentation::add_rule(p, "Vv", "");
    presentation::add_rule(p, "vUvvUv", "");
    presentation::add_rule(p, "UvUvUv", "");
    presentation::add_rule(p, "uuuuuvuuvvuvvvvvuv", "");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb;

    // SECTION("q18") {  // fails 30s
    //   kb.init(congruence_kind::twosided, p);
    //   kb.run();
    //   REQUIRE(kb.rewriting_system().number_of_rules() == 0);
    //   REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
    // }

    SECTION("q18.sub") {
      kb.init(congruence_kind::twosided, p);
      knuth_bendix::add_generating_pair(kb, "uvUVuVUvvuVUVuvU", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 24);
      REQUIRE(kb.number_of_classes() == 18);
    }

    // SECTION("q18.sub1") {  // fails 10s
    //   kb.init(congruence_kind::onesided, p);
    //   knuth_bendix::add_generating_pair(kb, "uuvv", "");
    //   knuth_bendix::add_generating_pair(kb, "VVUU", "");
    //   knuth_bendix::add_generating_pair(kb, "vuuv", "");
    //   knuth_bendix::add_generating_pair(kb, "VUUV", "");

    //   kb.run();
    //   REQUIRE(kb.rewriting_system().number_of_rules() == 0);
    //   REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
    // }
  }

  // Generated by MAF
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "589",
                          "MAF: onerelq/q40_uv",
                          "[quick][maf][onerelq]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("uUvV").contains_empty_word(true);

    presentation::add_rule(p, "uU", "");
    presentation::add_rule(p, "Uu", "");
    presentation::add_rule(p, "vV", "");
    presentation::add_rule(p, "Vv", "");
    presentation::add_rule(p, "vUvvUv", "");
    presentation::add_rule(p, "UvUvUv", "");
    presentation::add_rule(p, "uuuvuuvvuvvvuuvuvv", "");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb;

    // SECTION("q40_uv") {  // fails 30s
    //   kb.init(congruence_kind::twosided, p);
    //   kb.run();
    //   REQUIRE(kb.rewriting_system().number_of_rules() == 0);
    //   REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
    // }

    SECTION("q40_uv.sub") {
      kb.init(congruence_kind::twosided, p);
      knuth_bendix::add_generating_pair(kb, "uuuuuu", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 70);
      REQUIRE(kb.number_of_classes() == 168);
    }

    // SECTION("q40_uv.sub1") {  // fails after 10s
    //   kb.init(congruence_kind::twosided, p);
    //   knuth_bendix::add_generating_pair(kb, "vvvuvvuuvuuuvvuvuu", "");

    //   kb.run();
    //   REQUIRE(kb.rewriting_system().number_of_rules() == 0);
    //   REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
    // }

    SECTION("q40_uv.sub2") {
      kb.init(congruence_kind::twosided, p);
      knuth_bendix::add_generating_pair(kb, "uuuuuuuuuuuu", "");
      knuth_bendix::add_generating_pair(kb, "uuuuuuvUUUUUUV", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 174);
      REQUIRE(kb.number_of_classes() == 336);
    }

    // SECTION("q40_uv.sub3") {  // fails after 10s
    //   kb.init(congruence_kind::twosided, p);
    //   knuth_bendix::add_generating_pair(kb, "uuuuvuuuuvuvvuvuvuuvvu", "");
    //   knuth_bendix::add_generating_pair(kb, "uuuvuuuuvuuVVUVVVVUVVU", "");

    //   kb.run();
    //   REQUIRE(kb.rewriting_system().number_of_rules() == 0);
    //   REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
    // }
  }

  // Generated by MAF
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "590",
                          "MAF: onerelq/q41_xy",
                          "[quick][maf][onerelq]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("yYx").contains_empty_word(true);

    presentation::add_rule(p, "yY", "");
    presentation::add_rule(p, "Yy", "");
    presentation::add_rule(p, "xx", "");
    presentation::add_rule(p, "yy", "Y");
    presentation::add_rule(p, "xyxyxYxyxyxYxYxYxy", "YxyxyxyxYxyxyxYxYx");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb;

    // SECTION("q41_xy") {  // fails 30s
    //   kb.init(congruence_kind::twosided, p);
    //   kb.run();
    //   REQUIRE(kb.rewriting_system().number_of_rules() == 0);
    //   REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
    // }

    // SECTION("q41_xy.sub") {  // fails 10s
    //   kb.init(congruence_kind::twosided, p);
    //   // NOTE: X was present here, replaced with x
    //   knuth_bendix::add_generating_pair(
    //       kb, "xYxYxyxYxYxyxyxyxYxYxYxyxyxYxyxyxyxY", "");

    //   kb.run();
    //   REQUIRE(kb.rewriting_system().number_of_rules() == 0);
    //   REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
    // }

    SECTION("q41_xy.sub1") {
      kb.init(congruence_kind::twosided, p);
      knuth_bendix::add_generating_pair(kb, "xyxyxyxyxyxy", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 26);
      REQUIRE(kb.number_of_classes() == 186);
    }
  }

  // P4MM triangle group presentation
  // b +
  // / |a
  // +---+
  // c
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "591",
                          "MAF: subgroups/t(4_2_4)",
                          "[quick][maf][subgroups]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("abc").contains_empty_word(true);

    presentation::add_rule(p, "aa", "");
    presentation::add_rule(p, "bb", "");
    presentation::add_rule(p, "cc", "");
    presentation::add_rule(p, "caca", "");
    presentation::add_rule(p, "babababa", "");
    presentation::add_rule(p, "cbcbcbcb", "");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb;

    // SECTION("t(4_2_4)") {  // fails in 30s
    //   kb.init(congruence_kind::twosided, p);
    //   kb.run();
    //   REQUIRE(kb.rewriting_system().number_of_rules() == 0);
    //   REQUIRE(kb.number_of_classes() == 2);
    // }

    SECTION("t(4_2_4).sub") {
      kb.init(congruence_kind::onesided, p);
      knuth_bendix::add_generating_pair(kb, "c", "");
      knuth_bendix::add_generating_pair(kb, "a", "");
      knuth_bendix::add_generating_pair(kb, "bab", "");
      knuth_bendix::add_generating_pair(kb, "bcb", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 11);
      REQUIRE(kb.number_of_classes() == 2);
    }

    SECTION("t(4_2_4).sub1") {
      kb.init(congruence_kind::onesided, p);
      knuth_bendix::add_generating_pair(kb, "cbc", "");
      knuth_bendix::add_generating_pair(kb, "a", "");
      knuth_bendix::add_generating_pair(kb, "b", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 10);
      REQUIRE(kb.number_of_classes() == 2);
    }

    SECTION("t(4_2_4).subgen") {
      kb.init(congruence_kind::onesided, p);
      knuth_bendix::add_generating_pair(kb, "c", "");
      knuth_bendix::add_generating_pair(kb, "a", "");
      knuth_bendix::add_generating_pair(kb, "bab", "");
      knuth_bendix::add_generating_pair(kb, "bcb", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 11);
      REQUIRE(kb.number_of_classes() == 2);
    }

    SECTION("t(4_2_4).subnor") {
      kb.init(congruence_kind::twosided, p);
      knuth_bendix::add_generating_pair(kb, "cbcb", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 6);
      REQUIRE(kb.number_of_classes() == 16);
    }
  }

  // Generated by MAF
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "592",
                          "MAF: subgroups/trace",
                          "[quick][maf][subgroups]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("lrfLR").contains_empty_word(true);

    presentation::add_rule(p, "lL", "");
    presentation::add_rule(p, "rR", "");
    presentation::add_rule(p, "ff", "");
    presentation::add_rule(p, "Ll", "");
    presentation::add_rule(p, "Rr", "");
    presentation::add_rule(p, "lfl", "f");
    presentation::add_rule(p, "rfr", "f");
    presentation::add_rule(p, "frflfrf", "rflfr");
    presentation::add_rule(p, "lfrfl", "rflfr");

    using RewritingSystem = detail::RewritingSystemTrie<WreathCmp>;
    KnuthBendix<std::string, RewritingSystem> kb(
        congruence_kind::onesided, p, std::vector<size_t>({1, 1, 2, 3, 3}));

    SECTION("trace") {
      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 13);
      REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
    }

    SECTION("trace.sub") {
      knuth_bendix::add_generating_pair(kb, "rf", "");
      knuth_bendix::add_generating_pair(kb, "lf", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 16);
      REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
    }
  }

  // Ran for approx. 30s didn't terminate
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "593",
                          "MAF: unknown/g9(1_4)",
                          "[quick][unknown][maf]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("aAgGdDbBhHeEcCiIfF").contains_empty_word(true);

    presentation::add_rule(p, "aA", "");
    presentation::add_rule(p, "Aa", "");
    presentation::add_rule(p, "gG", "");
    presentation::add_rule(p, "Gg", "");
    presentation::add_rule(p, "dD", "");
    presentation::add_rule(p, "Dd", "");
    presentation::add_rule(p, "bB", "");
    presentation::add_rule(p, "Bb", "");
    presentation::add_rule(p, "hH", "");
    presentation::add_rule(p, "Hh", "");
    presentation::add_rule(p, "eE", "");
    presentation::add_rule(p, "Ee", "");
    presentation::add_rule(p, "cC", "");
    presentation::add_rule(p, "Cc", "");
    presentation::add_rule(p, "iI", "");
    presentation::add_rule(p, "Ii", "");
    presentation::add_rule(p, "fF", "");
    presentation::add_rule(p, "Ff", "");
    presentation::add_rule(p, "ab", "e");
    presentation::add_rule(p, "bc", "f");
    presentation::add_rule(p, "cd", "g");
    presentation::add_rule(p, "de", "h");
    presentation::add_rule(p, "ef", "i");
    presentation::add_rule(p, "fg", "a");
    presentation::add_rule(p, "gh", "b");
    presentation::add_rule(p, "hi", "c");
    presentation::add_rule(p, "ia", "d");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb;

    // SECTION("g9(1_4)") {  // fails 30s
    //   kb.init(congruence_kind::twosided, p);
    //   kb.run();
    //   REQUIRE(kb.rewriting_system().number_of_rules() == 0);
    //   REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
    // }

    SECTION("g9(1_4).sub") {
      kb.init(congruence_kind::twosided, p);
      knuth_bendix::add_generating_pair(kb, "abAB", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 315);
      REQUIRE(kb.number_of_classes() == 37);
    }
  }

  // Presentation of group A_4 regarded as monoid presentation
  // - gives infinite monoid.
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "594",
                          "MAF: monoids/a4monoid",
                          "[quick][maf]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("abB").contains_empty_word(true);

    presentation::add_rule(p, "bb", "B");
    presentation::add_rule(p, "BaB", "aba");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb(congruence_kind::twosided, p);
    kb.run();
    REQUIRE(kb.rewriting_system().number_of_rules() == 6);
    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
  }

  // Generated by MAF
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "595",
                          "MAF: lesson5/2379_i504",
                          "[quick][maf][lesson5]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("cChHgGeEdDiIfF").contains_empty_word(true);

    presentation::add_rule(p, "cC", "");
    presentation::add_rule(p, "Cc", "");
    presentation::add_rule(p, "hH", "");
    presentation::add_rule(p, "Hh", "");
    presentation::add_rule(p, "gG", "");
    presentation::add_rule(p, "Gg", "");
    presentation::add_rule(p, "eE", "");
    presentation::add_rule(p, "Ee", "");
    presentation::add_rule(p, "dD", "");
    presentation::add_rule(p, "Dd", "");
    presentation::add_rule(p, "iI", "");
    presentation::add_rule(p, "Ii", "");
    presentation::add_rule(p, "fF", "");
    presentation::add_rule(p, "Ff", "");
    presentation::add_rule(p, "dD", "");
    presentation::add_rule(p, "Dd", "");
    presentation::add_rule(p, "eE", "");
    presentation::add_rule(p, "Ee", "");
    presentation::add_rule(p, "fF", "");
    presentation::add_rule(p, "Ff", "");
    presentation::add_rule(p, "gG", "");
    presentation::add_rule(p, "Gg", "");
    presentation::add_rule(p, "hH", "");
    presentation::add_rule(p, "Hh", "");
    presentation::add_rule(p, "iI", "");
    presentation::add_rule(p, "Ii", "");
    presentation::add_rule(p, "dC", "Cd");
    presentation::add_rule(p, "fD", "Df");
    presentation::add_rule(p, "gD", "Dg");
    presentation::add_rule(p, "fE", "Ef");
    presentation::add_rule(p, "iE", "Ei");
    presentation::add_rule(p, "gF", "Fg");
    presentation::add_rule(p, "iF", "Fi");
    presentation::add_rule(p, "iG", "Gi");
    presentation::add_rule(p, "iH", "Hi");
    presentation::add_rule(p, "cchC", "hc");
    presentation::add_rule(p, "Ege", "Cgc");  // codespell:ignore
    presentation::add_rule(p, "gIC", "CgI");
    presentation::add_rule(p, "GhC", "CGh");
    presentation::add_rule(p, "hCD", "CDh");
    presentation::add_rule(p, "Hgh", "Cgc");
    presentation::add_rule(p, "eDg", "Dge");
    presentation::add_rule(p, "GhD", "DhG");
    presentation::add_rule(p, "HiD", "DHi");
    presentation::add_rule(p, "GhF", "FhG");
    presentation::add_rule(p, "dGCi", "CGid");
    presentation::add_rule(p, "DeFC", "CeDF");
    presentation::add_rule(p, "fhIC", "CfhI");
    presentation::add_rule(p, "HFiC", "CHFi");
    presentation::add_rule(p, "iCDg", "CDgi");
    presentation::add_rule(p, "he", "eh");
    presentation::add_rule(p, "hfHI", "DfId");
    presentation::add_rule(p, "FHgE", "EHFg");
    presentation::add_rule(p, "EFGiD", "DEFGi");

    using RewritingSystem = detail::RewritingSystemTrie<RevRPOCmp>;
    KnuthBendix<std::string, RewritingSystem> kb;

    SECTION("2379_i504") {
      kb.init(congruence_kind::twosided, p);
      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 105);
      REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
    }

    SECTION("2379_i504.sub") {
      kb.init(congruence_kind::twosided, p);
      knuth_bendix::add_generating_pair(kb, "ceCE", "");
      knuth_bendix::add_generating_pair(kb, "cgCG", "");
      knuth_bendix::add_generating_pair(kb, "chCH", "");
      knuth_bendix::add_generating_pair(kb, "ciCI", "");
      knuth_bendix::add_generating_pair(kb, "deDE", "");
      knuth_bendix::add_generating_pair(kb, "dhDH", "");
      knuth_bendix::add_generating_pair(kb, "diDI", "");  // codespell:ignore
      knuth_bendix::add_generating_pair(kb, "egEG", "");
      knuth_bendix::add_generating_pair(kb, "fhFH", "");
      knuth_bendix::add_generating_pair(kb, "ghGH", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 98);
      REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
    }
  }

  // A finite quotient of the Von Dyck (7,2,3) group
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "596",
                          "MAF: lesson5/7237",
                          "[quick][maf][lesson5]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("aAb").contains_empty_word(true);

    presentation::add_rule(p, "aA", "");
    presentation::add_rule(p, "Aa", "");
    presentation::add_rule(p, "bb", "");
    presentation::add_rule(p, "ababab", "");
    presentation::add_rule(p, "aaaaaaa", "");
    presentation::add_rule(p, "abAbabAbabAbabAbabAbabAbabAb", "");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb;
    kb.init(congruence_kind::twosided, p);
    kb.run();
    REQUIRE(kb.rewriting_system().number_of_rules() == 203);
    REQUIRE(kb.number_of_classes() == 1092);
  }

  // Generated by MAF
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "597",
                          "MAF: lesson5/fred2",
                          "[quick][maf][lesson5]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("cCdDeEfFgGhHiI").contains_empty_word(true);

    presentation::add_rule(p, "cC", "");
    presentation::add_rule(p, "Cc", "");
    presentation::add_rule(p, "dD", "");
    presentation::add_rule(p, "Dd", "");
    presentation::add_rule(p, "eE", "");
    presentation::add_rule(p, "Ee", "");
    presentation::add_rule(p, "fF", "");
    presentation::add_rule(p, "Ff", "");
    presentation::add_rule(p, "gG", "");
    presentation::add_rule(p, "Gg", "");
    presentation::add_rule(p, "hH", "");
    presentation::add_rule(p, "Hh", "");
    presentation::add_rule(p, "iI", "");
    presentation::add_rule(p, "Ii", "");
    presentation::add_rule(p, "cC", "");
    presentation::add_rule(p, "cd", "dc");
    presentation::add_rule(p, "cD", "Dc");
    presentation::add_rule(p, "cf", "fc");
    presentation::add_rule(p, "cF", "Fc");
    presentation::add_rule(p, "cg", "gEce");
    presentation::add_rule(p, "cG", "GEce");
    presentation::add_rule(p, "ch", "hEce");
    presentation::add_rule(p, "cH", "HEce");
    presentation::add_rule(p, "ci", "iEce");
    presentation::add_rule(p, "cI", "IEce");
    presentation::add_rule(p, "Cc", "");
    presentation::add_rule(p, "Cd", "dC");
    presentation::add_rule(p, "CD", "DC");
    presentation::add_rule(p, "Ce", "ceCC");
    presentation::add_rule(p, "CE", "cECC");
    presentation::add_rule(p, "Cf", "fC");
    presentation::add_rule(p, "CF", "FC");
    presentation::add_rule(p, "Cg", "gEceCC");
    presentation::add_rule(p, "CG", "GEceCC");
    presentation::add_rule(p, "Ch", "hEceCC");
    presentation::add_rule(p, "CH", "HEceCC");
    presentation::add_rule(p, "Ci", "iEceCC");
    presentation::add_rule(p, "CI", "IEceCC");
    presentation::add_rule(p, "dD", "");
    presentation::add_rule(p, "de", "cedC");
    presentation::add_rule(p, "dE", "cEdC");
    presentation::add_rule(p, "df", "fd");
    presentation::add_rule(p, "dF", "Fd");
    presentation::add_rule(p, "dg", "gd");
    presentation::add_rule(p, "dG", "Gd");
    presentation::add_rule(p, "dh", "hEcedC");
    presentation::add_rule(p, "dH", "HEcedC");
    presentation::add_rule(p, "di", "iEcedC");
    presentation::add_rule(p, "dI", "IEcedC");
    presentation::add_rule(p, "Dd", "");
    presentation::add_rule(p, "De", "ceDC");
    presentation::add_rule(p, "DE", "cEDC");
    presentation::add_rule(p, "Df", "fD");
    presentation::add_rule(p, "DF", "FD");
    presentation::add_rule(p, "Dg", "gD");
    presentation::add_rule(p, "DG", "GD");
    presentation::add_rule(p, "Dh", "hEceDC");
    presentation::add_rule(p, "DH", "HEceDC");
    presentation::add_rule(p, "Di", "iEceDC");
    presentation::add_rule(p, "DI", "IEceDC");
    presentation::add_rule(p, "eE", "");
    presentation::add_rule(p, "ef", "fe");
    presentation::add_rule(p, "eF", "Fe");
    presentation::add_rule(p, "eg", "gceC");
    presentation::add_rule(p, "eG", "GceC");
    presentation::add_rule(p, "eh", "he");
    presentation::add_rule(p, "eH", "He");
    presentation::add_rule(p, "ei", "ie");
    presentation::add_rule(p, "eI", "Ie");
    presentation::add_rule(p, "Ee", "");
    presentation::add_rule(p, "Ef", "fE");
    presentation::add_rule(p, "EF", "FE");
    presentation::add_rule(p, "Eg", "gcEC");
    presentation::add_rule(p, "EG", "GcEC");
    presentation::add_rule(p, "Eh", "hE");
    presentation::add_rule(p, "EH", "HE");
    presentation::add_rule(p, "Ei", "iE");
    presentation::add_rule(p, "EI", "IE");
    presentation::add_rule(p, "fF", "");
    presentation::add_rule(p, "fg", "gf");
    presentation::add_rule(p, "fG", "Gf");
    presentation::add_rule(p, "fh", "hfEceC");
    presentation::add_rule(p, "fH", "HfEceC");
    presentation::add_rule(p, "fi", "if");
    presentation::add_rule(p, "fI", "If");
    presentation::add_rule(p, "Ff", "");
    presentation::add_rule(p, "Fg", "gF");
    presentation::add_rule(p, "FG", "GF");
    presentation::add_rule(p, "Fh", "hFEceC");
    presentation::add_rule(p, "FH", "HFEceC");
    presentation::add_rule(p, "Fi", "iF");
    presentation::add_rule(p, "FI", "IF");
    presentation::add_rule(p, "gG", "");
    presentation::add_rule(p, "gh", "hgEceC");
    presentation::add_rule(p, "gH", "HgEceC");
    presentation::add_rule(p, "gi", "ig");
    presentation::add_rule(p, "gI", "Ig");
    presentation::add_rule(p, "Gg", "");
    presentation::add_rule(p, "Gh", "hGEceC");
    presentation::add_rule(p, "GH", "HGEceC");
    presentation::add_rule(p, "Gi", "iG");
    presentation::add_rule(p, "GI", "IG");
    presentation::add_rule(p, "hH", "");
    presentation::add_rule(p, "hi", "ih");
    presentation::add_rule(p, "hI", "Ih");
    presentation::add_rule(p, "Hh", "");
    presentation::add_rule(p, "Hi", "iH");
    presentation::add_rule(p, "HI", "IH");
    presentation::add_rule(p, "iI", "");
    presentation::add_rule(p, "Ii", "");
    presentation::add_rule(p, "cce", "ecc");
    presentation::add_rule(p, "ccE", "Ecc");
    presentation::add_rule(p, "cee", "eec");
    presentation::add_rule(p, "cEE", "EEc");
    presentation::add_rule(p, "dce", "edc");
    presentation::add_rule(p, "dcE", "Edc");
    presentation::add_rule(p, "Dce", "eDc");
    presentation::add_rule(p, "DcE", "EDc");
    presentation::add_rule(p, "ecE", "Ece");
    presentation::add_rule(p, "cece", "ecec");
    presentation::add_rule(p, "cEce", "Ecec");
    presentation::add_rule(p, "cEcE", "EcEc");
    presentation::add_rule(p, "EEce", "cE");

    using RewritingSystem = detail::RewritingSystemTrie<RevRPOCmp>;
    KnuthBendix<std::string, RewritingSystem> kb;
    kb.init(congruence_kind::twosided, p);
    kb.run();
    REQUIRE(kb.rewriting_system().number_of_rules() == 109);
    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
  }

  // Von Dyck (7,2,3) group - infinite hyperbolic
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "598",
                          "MAF: lesson2/723",
                          "[quick][maf][lesson2]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("aAb").contains_empty_word(true);

    presentation::add_rule(p, "aA", "");
    presentation::add_rule(p, "Aa", "");
    presentation::add_rule(p, "bb", "");
    presentation::add_rule(p, "ababab", "");
    presentation::add_rule(p, "aaaaaaa", "");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb;
    kb.init(congruence_kind::twosided, p);
    kb.run();
    REQUIRE(kb.rewriting_system().number_of_rules() == 13);
    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
  }

  // Von Dyck (7,2,3) group - infinite hyperbolic
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "599",
                          "MAF: lesson2/723_abc",
                          "[quick][maf][lesson2]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("aAbcC").contains_empty_word(true);

    presentation::add_rule(p, "aA", "");
    presentation::add_rule(p, "Aa", "");
    presentation::add_rule(p, "bb", "");
    presentation::add_rule(p, "cC", "");
    presentation::add_rule(p, "Cc", "");
    presentation::add_rule(p, "abc", "");
    presentation::add_rule(p, "ccc", "");
    presentation::add_rule(p, "aaaaaaa", "");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb;

    SECTION("723_abc") {
      kb.init(congruence_kind::twosided, p);
      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 34);
      REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
    }

    SECTION("723_abc.sub") {
      kb.init(congruence_kind::twosided, p);
      knuth_bendix::add_generating_pair(kb, "bcbCbcbCbcbCbcbC", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 84);
      REQUIRE(kb.number_of_classes() == 168);
    }

    SECTION("723_abc.sub1") {
      kb.init(congruence_kind::twosided, p);
      knuth_bendix::add_generating_pair(kb, "bcbCbcbCbcbCbcbCbcbCbcbC", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 244);
      REQUIRE(kb.number_of_classes() == 1'092);
    }
  }

  // Von Dyck (3,3,4) group - infinite hyperbolic
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "600",
                          "MAF: hyperbolic/d(3_3_4)_abc",
                          "[quick][maf][hyperbolic]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("aAbBcC").contains_empty_word(true);

    presentation::add_rule(p, "aA", "");
    presentation::add_rule(p, "Aa", "");
    presentation::add_rule(p, "bB", "");
    presentation::add_rule(p, "Bb", "");
    presentation::add_rule(p, "cC", "");
    presentation::add_rule(p, "Cc", "");
    presentation::add_rule(p, "aaa", "");
    presentation::add_rule(p, "bbb", "");
    presentation::add_rule(p, "abababab", "");
    presentation::add_rule(p, "abc", "");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb;
    kb.init(congruence_kind::twosided, p);
    kb.run();
    REQUIRE(kb.rewriting_system().number_of_rules() == 44);
    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
  }

  // Von Dyck (3,3,4) group - infinite hyperbolic
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "601",
                          "MAF: hyperbolic/d(3_3_4)_recursive",
                          "[quick][maf][hyperbolic]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("aAbB").contains_empty_word(true);

    presentation::add_rule(p, "aA", "");
    presentation::add_rule(p, "Aa", "");
    presentation::add_rule(p, "bB", "");
    presentation::add_rule(p, "Bb", "");
    presentation::add_rule(p, "aaa", "");
    presentation::add_rule(p, "bbb", "");
    presentation::add_rule(p, "abababab", "");

    using RewritingSystem = detail::RewritingSystemTrie<RevRPOCmp>;
    KnuthBendix<std::string, RewritingSystem> kb;
    kb.init(congruence_kind::twosided, p);
    kb.run();
    REQUIRE(kb.rewriting_system().number_of_rules() == 9);
    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
  }

  // Von Dyck (3,3,7) group - infinite hyperbolic
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "602",
                          "MAF: hyperbolic/d(3_3_7)_recursive",
                          "[quick][maf][hyperbolic]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("aAbB").contains_empty_word(true);

    presentation::add_rule(p, "aA", "");
    presentation::add_rule(p, "Aa", "");
    presentation::add_rule(p, "bB", "");
    presentation::add_rule(p, "Bb", "");
    presentation::add_rule(p, "aaa", "");
    presentation::add_rule(p, "bbb", "");
    presentation::add_rule(p, "ababababababab", "");

    using RewritingSystem = detail::RewritingSystemTrie<RevRPOCmp>;
    KnuthBendix<std::string, RewritingSystem> kb;
    kb.init(congruence_kind::twosided, p);
    kb.run();
    REQUIRE(kb.rewriting_system().number_of_rules() == 9);
    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
  }

  // Von Dyck (3,5,7) group - infinite hyperbolic
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "603",
                          "MAF: hyperbolic/d(3_5_7)_recursive",
                          "[quick][maf][hyperbolic]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("aAbB").contains_empty_word(true);

    presentation::add_rule(p, "aA", "");
    presentation::add_rule(p, "Aa", "");
    presentation::add_rule(p, "bB", "");
    presentation::add_rule(p, "Bb", "");
    presentation::add_rule(p, "aaa", "");
    presentation::add_rule(p, "bbbbb", "");
    presentation::add_rule(p, "ababababababab", "");

    using RewritingSystem = detail::RewritingSystemTrie<RevRPOCmp>;
    KnuthBendix<std::string, RewritingSystem> kb;
    kb.init(congruence_kind::twosided, p);
    kb.run();
    REQUIRE(kb.rewriting_system().number_of_rules() == 10);
    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
  }

  // Von Dyck (4,3,7) group - infinite hyperbolic
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "604",
                          "MAF: hyperbolic/d(4_3_7)_abc",
                          "[quick][maf][hyperbolic]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("aAbBcC").contains_empty_word(true);

    presentation::add_rule(p, "aA", "");
    presentation::add_rule(p, "Aa", "");
    presentation::add_rule(p, "bB", "");
    presentation::add_rule(p, "Bb", "");
    presentation::add_rule(p, "cC", "");
    presentation::add_rule(p, "Cc", "");
    presentation::add_rule(p, "aaaa", "");
    presentation::add_rule(p, "bbb", "");
    presentation::add_rule(p, "ababababababab", "");
    presentation::add_rule(p, "abc", "");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb;
    kb.init(congruence_kind::twosided, p);
    kb.run();
    REQUIRE(kb.rewriting_system().number_of_rules() == 46);
    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
  }

  // Von Dyck (4,3,7) group - infinite hyperbolic
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "605",
                          "MAF: hyperbolic/d(4_3_7)_recursive",
                          "[quick][maf][hyperbolic]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("aAbB").contains_empty_word(true);

    presentation::add_rule(p, "aA", "");
    presentation::add_rule(p, "Aa", "");
    presentation::add_rule(p, "bB", "");
    presentation::add_rule(p, "Bb", "");
    presentation::add_rule(p, "aaaa", "");
    presentation::add_rule(p, "bbb", "");
    presentation::add_rule(p, "ababababababab", "");

    using RewritingSystem = detail::RewritingSystemTrie<RevRPOCmp>;
    KnuthBendix<std::string, RewritingSystem> kb;
    kb.init(congruence_kind::twosided, p);
    kb.run();
    REQUIRE(kb.rewriting_system().number_of_rules() == 9);
    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
  }

  // Von Dyck (5,5,5) group - infinite hyperbolic
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "606",
                          "MAF: hyperbolic/d(5_5_5)_recursive",
                          "[quick][maf][hyperbolic]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("aAbB").contains_empty_word(true);

    presentation::add_rule(p, "aA", "");
    presentation::add_rule(p, "Aa", "");
    presentation::add_rule(p, "bB", "");
    presentation::add_rule(p, "Bb", "");
    presentation::add_rule(p, "aaaaa", "");
    presentation::add_rule(p, "bbbbb", "");
    presentation::add_rule(p, "ababababab", "");

    using RewritingSystem = detail::RewritingSystemTrie<RevRPOCmp>;
    KnuthBendix<std::string, RewritingSystem> kb;
    kb.init(congruence_kind::twosided, p);
    kb.run();
    REQUIRE(kb.rewriting_system().number_of_rules() == 6);
    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
  }

  // (7,2,3) Von Dyck group - infinite hyperbolic - confluent presentation
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "607",
                          "MAF: hyperbolic/d(7_2_3)",
                          "[quick][maf][hyperbolic]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("aAbcC").contains_empty_word(true);

    presentation::add_rule(p, "aA", "");
    presentation::add_rule(p, "Aa", "");
    presentation::add_rule(p, "bb", "");
    presentation::add_rule(p, "cC", "");
    presentation::add_rule(p, "Cc", "");
    presentation::add_rule(p, "aaaaaaa", "");
    presentation::add_rule(p, "ccc", "");
    presentation::add_rule(p, "abc", "");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb;

    SECTION("d(7_2_3)") {
      kb.init(congruence_kind::twosided, p);
      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 34);
      REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
    }

    SECTION("d(7_2_3).sub") {
      kb.init(congruence_kind::onesided, p);
      knuth_bendix::add_generating_pair(kb, "a", "");
      knuth_bendix::add_generating_pair(kb, "acbCaaabaa", "");
      knuth_bendix::add_generating_pair(kb, "aCaaacbCaa", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 39);
      REQUIRE(kb.number_of_classes() == 1);
    }

    SECTION("d(7_2_3).sub1") {
      kb.init(congruence_kind::onesided, p);
      knuth_bendix::add_generating_pair(kb, "a", "");
      knuth_bendix::add_generating_pair(kb, "A", "");
      knuth_bendix::add_generating_pair(kb, "cbCAAAbabaaacbC", "");
      knuth_bendix::add_generating_pair(kb, "cbCAAAbAbaaacbC", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 50);
      REQUIRE(kb.number_of_classes() == 24);
    }

    SECTION("d(7_2_3).sub168") {
      // NOTE: JDM B -> b
      kb.init(congruence_kind::onesided, p);
      knuth_bendix::add_generating_pair(kb, "CabC", "");
      knuth_bendix::add_generating_pair(kb, "CACA", "");
      knuth_bendix::add_generating_pair(kb, "AcbAb", "");
      knuth_bendix::add_generating_pair(kb, "bacaCa", "");
      knuth_bendix::add_generating_pair(kb, "AAAbcAAA", "");
      knuth_bendix::add_generating_pair(kb, "AcAccAb", "");
      knuth_bendix::add_generating_pair(kb, "cAAAAAAb", "");
      knuth_bendix::add_generating_pair(kb, "baaaCbaaC", "");
      knuth_bendix::add_generating_pair(kb, "cbaaaaaCA", "");
      knuth_bendix::add_generating_pair(kb, "cbaaCbaaCA", "");
      knuth_bendix::add_generating_pair(kb, "AcAAcAbaCaCA", "");
      knuth_bendix::add_generating_pair(kb, "AcAbaCaaCaCA", "");
      knuth_bendix::add_generating_pair(kb, "baaacbAbaaC", "");
      knuth_bendix::add_generating_pair(kb, "cAAcAAcAbCAA", "");
      knuth_bendix::add_generating_pair(kb, "cAAcAbaCaCAA", "");
      knuth_bendix::add_generating_pair(kb, "cAcAAcAbaCaa", "");
      knuth_bendix::add_generating_pair(kb, "cAcAbaCaaCaa", "");
      knuth_bendix::add_generating_pair(kb, "cbaacbAbaaCA", "");
      knuth_bendix::add_generating_pair(kb, "aacAcAAcAbaC", "");
      knuth_bendix::add_generating_pair(kb, "aacAcAbaCaaC", "");
      knuth_bendix::add_generating_pair(kb, "acAcAAbAAbaCa", "");
      knuth_bendix::add_generating_pair(kb, "AAAcAAcAbCAAb", "");
      knuth_bendix::add_generating_pair(kb, "AAAcAbaCaCAAb", "");
      knuth_bendix::add_generating_pair(kb, "AAbaacAcAbaCAA", "");
      knuth_bendix::add_generating_pair(kb, "AAbaacbaCaaCAA", "");
      knuth_bendix::add_generating_pair(kb, "AAcAbaabaaCaC", "");
      knuth_bendix::add_generating_pair(kb, "AbaacAcAbaCAAA", "");
      knuth_bendix::add_generating_pair(kb, "AbaacbaCaaCAAA", "");
      knuth_bendix::add_generating_pair(kb, "AcAAbaaaCAAbCA", "");
      knuth_bendix::add_generating_pair(kb, "AcAbaaaaaCAAb", "");
      knuth_bendix::add_generating_pair(kb, "AcAbaacAAAbaCA", "");
      knuth_bendix::add_generating_pair(kb, "baaacAccAbaaC", "");
      knuth_bendix::add_generating_pair(kb, "baacAAbcAAbaCa", "");
      knuth_bendix::add_generating_pair(kb, "baacAcAbaCaaa", "");
      knuth_bendix::add_generating_pair(kb, "baacbaabaaCaaa", "");
      knuth_bendix::add_generating_pair(kb, "baacbaCaaCaaa", "");
      knuth_bendix::add_generating_pair(kb, "cAAbaaaCAAbCAA", "");
      knuth_bendix::add_generating_pair(kb, "cAbaacAAAbaCAA", "");
      knuth_bendix::add_generating_pair(kb, "cAcAAbaaaCAAba", "");
      knuth_bendix::add_generating_pair(kb, "cbaacAAAbaaCaa", "");
      knuth_bendix::add_generating_pair(kb, "cbaacAccAbaaCA", "");
      knuth_bendix::add_generating_pair(kb, "aaacAAbacAAbaaC", "");
      knuth_bendix::add_generating_pair(kb, "aaacAAcAbCAAba", "");
      knuth_bendix::add_generating_pair(kb, "aaacAbaabaCAAba", "");
      knuth_bendix::add_generating_pair(kb, "aaacAbaCaCAAba", "");
      knuth_bendix::add_generating_pair(kb, "aacAbaaaCAAbaC", "");
      knuth_bendix::add_generating_pair(kb, "aacAbaacAbaCAAb", "");
      knuth_bendix::add_generating_pair(kb, "aacAbaaCbCAAbaC", "");
      knuth_bendix::add_generating_pair(kb, "aacbaacAAAbaaC", "");
      knuth_bendix::add_generating_pair(kb, "acAAbaacbaCAAAb", "");
      knuth_bendix::add_generating_pair(kb, "acAAbaaCaaCAAAb", "");
      knuth_bendix::add_generating_pair(kb, "acAbaaaccAAbaCa", "");
      knuth_bendix::add_generating_pair(kb, "acbaacAbcAbaaCa", "");
      knuth_bendix::add_generating_pair(kb, "AAAcAAbaaaCAAAb", "");
      knuth_bendix::add_generating_pair(kb, "AAbaaacAAAbaaCAA", "");
      knuth_bendix::add_generating_pair(kb, "AAcAAbaCbaCAAbC", "");
      knuth_bendix::add_generating_pair(kb, "AbaaacAAcAAbaaC", "");
      knuth_bendix::add_generating_pair(kb, "AbaaacAbCAAbaaC", "");
      knuth_bendix::add_generating_pair(kb, "AbaacAAcbCAAbaCA", "");
      knuth_bendix::add_generating_pair(kb, "AbaacAbaCAAbaCA", "");
      knuth_bendix::add_generating_pair(kb, "baaacAAbCAAbCAAA", "");
      knuth_bendix::add_generating_pair(kb, "baaacAAcAAbaaCA", "");
      knuth_bendix::add_generating_pair(kb, "baaacAbacAbaaCA", "");
      knuth_bendix::add_generating_pair(kb, "baacAAcAbCAAbaC", "");
      knuth_bendix::add_generating_pair(kb, "baacAbaacAbaCAA", "");
      knuth_bendix::add_generating_pair(kb, "baacAbaCAAbaCAA", "");
      knuth_bendix::add_generating_pair(kb, "cAbaaacAbCAAbaCA", "");
      knuth_bendix::add_generating_pair(kb, "cbaacAbacAbaaCAA", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 108);
      REQUIRE(kb.number_of_classes() == 168);
    }

    SECTION("d(7_2_3).sub2") {
      kb.init(congruence_kind::onesided, p);
      knuth_bendix::add_generating_pair(kb, "a", "");
      knuth_bendix::add_generating_pair(kb, "A", "");
      knuth_bendix::add_generating_pair(kb, "bAAAcaaab", "");
      knuth_bendix::add_generating_pair(kb, "bAAACaaab", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 45);
      REQUIRE(kb.number_of_classes() == 8);
    }

    SECTION("d(7_2_3).sub3") {
      kb.init(congruence_kind::onesided, p);
      knuth_bendix::add_generating_pair(kb, "a", "");
      knuth_bendix::add_generating_pair(kb, "A", "");
      knuth_bendix::add_generating_pair(kb, "baaacbabCAAAb", "");
      knuth_bendix::add_generating_pair(kb, "baaacbAbCAAAb", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 46);
      REQUIRE(kb.number_of_classes() == 9);
    }

    SECTION("d(7_2_3).sub4") {
      kb.init(congruence_kind::onesided, p);
      knuth_bendix::add_generating_pair(kb, "a", "");
      knuth_bendix::add_generating_pair(kb, "A", "");
      knuth_bendix::add_generating_pair(kb, "cAbCAcacbaC", "");
      knuth_bendix::add_generating_pair(kb, "cAbCACacbaC", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 49);
      REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
    }

    SECTION("d(7_2_3).sub5") {
      kb.init(congruence_kind::onesided, p);
      knuth_bendix::add_generating_pair(kb, "cbCbcbCb", "");
      knuth_bendix::add_generating_pair(kb, "bcbCbcbC", "");
      knuth_bendix::add_generating_pair(kb, "abAb", "");
      knuth_bendix::add_generating_pair(kb, "babA", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 48);
      REQUIRE(kb.number_of_classes() == 7);
    }

    SECTION("d(7_2_3).sub6") {
      kb.init(congruence_kind::onesided, p);
      knuth_bendix::add_generating_pair(kb, "baaa", "");
      knuth_bendix::add_generating_pair(kb, "AAAb", "");
      knuth_bendix::add_generating_pair(kb, "bCAAAbaaac", "");
      knuth_bendix::add_generating_pair(kb, "CAAAbaaacb", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 62);
      REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
    }

    SECTION("d(7_2_3).sub7") {
      kb.init(congruence_kind::twosided, p);
      knuth_bendix::add_generating_pair(kb, "abAbabAbabAbabAb", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 84);
      REQUIRE(kb.number_of_classes() == 168);
    }

    SECTION("d(7_2_3).subcomm") {
      kb.init(congruence_kind::onesided, p);
      knuth_bendix::add_generating_pair(kb, "abAb", "");
      knuth_bendix::add_generating_pair(kb, "Abab", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 39);
      REQUIRE(kb.number_of_classes() == 1);
    }
  }

  // Von Dyck (7,7,7) group - infinite hyperbolic
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "608",
                          "MAF: hyperbolic/d(7_7_7)_recursive",
                          "[quick][maf][hyperbolic]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("aAbB").contains_empty_word(true);

    presentation::add_rule(p, "aA", "");
    presentation::add_rule(p, "Aa", "");
    presentation::add_rule(p, "bB", "");
    presentation::add_rule(p, "Bb", "");
    presentation::add_rule(p, "aaaaaaa", "");
    presentation::add_rule(p, "bbbbbbb", "");
    presentation::add_rule(p, "ababababababab", "");

    using RewritingSystem = detail::RewritingSystemTrie<RevRPOCmp>;
    KnuthBendix<std::string, RewritingSystem> kb;
    kb.init(congruence_kind::twosided, p);
    kb.run();
    REQUIRE(kb.rewriting_system().number_of_rules() == 6);
    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
  }

  // Von Dyck (7,7,8) group - infinite hyperbolic
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "609",
                          "MAF: hyperbolic/d(7_7_8)_recursive",
                          "[quick][maf][hyperbolic]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("aAbB").contains_empty_word(true);

    presentation::add_rule(p, "aA", "");
    presentation::add_rule(p, "Aa", "");
    presentation::add_rule(p, "bB", "");
    presentation::add_rule(p, "Bb", "");
    presentation::add_rule(p, "aaaaaaa", "");
    presentation::add_rule(p, "bbbbbbb", "");
    presentation::add_rule(p, "abababababababab", "");

    using RewritingSystem = detail::RewritingSystemTrie<RevRPOCmp>;
    KnuthBendix<std::string, RewritingSystem> kb;
    kb.init(congruence_kind::twosided, p);
    kb.run();
    REQUIRE(kb.rewriting_system().number_of_rules() == 13);
    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
  }

  // Von Dyck (7,8,8) group - infinite hyperbolic
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "610",
                          "MAF: hyperbolic/d(7_8_8)_recursive",
                          "[quick][maf][hyperbolic]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("aAbB").contains_empty_word(true);

    presentation::add_rule(p, "aA", "");
    presentation::add_rule(p, "Aa", "");
    presentation::add_rule(p, "bB", "");
    presentation::add_rule(p, "Bb", "");
    presentation::add_rule(p, "aaaaaaa", "");
    presentation::add_rule(p, "bbbbbbbb", "");
    presentation::add_rule(p, "abababababababab", "");

    using RewritingSystem = detail::RewritingSystemTrie<RevRPOCmp>;
    KnuthBendix<std::string, RewritingSystem> kb;
    kb.init(congruence_kind::twosided, p);
    kb.run();
    REQUIRE(kb.rewriting_system().number_of_rules() == 6);
    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
  }

  // Von Dyck (8,8,8) group - infinite hyperbolic
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "611",
                          "MAF: hyperbolic/d(8_8_8)_recursive",
                          "[quick][maf][hyperbolic]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("aAbB").contains_empty_word(true);

    presentation::add_rule(p, "aA", "");
    presentation::add_rule(p, "Aa", "");
    presentation::add_rule(p, "bB", "");
    presentation::add_rule(p, "Bb", "");
    presentation::add_rule(p, "aaaaaaaa", "");
    presentation::add_rule(p, "bbbbbbbb", "");
    presentation::add_rule(p, "abababababababab", "");

    using RewritingSystem = detail::RewritingSystemTrie<RevRPOCmp>;
    KnuthBendix<std::string, RewritingSystem> kb;
    kb.init(congruence_kind::twosided, p);
    kb.run();
    REQUIRE(kb.rewriting_system().number_of_rules() == 6);
    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
  }

  // (12,2,3) triangle group - infinite hyperbolic
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "612",
                          "MAF: hyperbolic/t(12_2_3)",
                          "[quick][maf][hyperbolic]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("abc").contains_empty_word(true);

    presentation::add_rule(p, "aa", "");
    presentation::add_rule(p, "bb", "");
    presentation::add_rule(p, "cc", "");
    presentation::add_rule(p, "bcbcbcbcbcbcbcbcbcbcbcbc", "");
    presentation::add_rule(p, "caca", "");
    presentation::add_rule(p, "ababab", "");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb;

    SECTION("t(12_2_3)") {
      kb.init(congruence_kind::twosided, p);
      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 7);
      REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
    }

    SECTION("t(12_2_3).sub") {
      kb.init(congruence_kind::onesided, p);
      knuth_bendix::add_generating_pair(kb, "c", "");
      knuth_bendix::add_generating_pair(kb, "ab", "");
      knuth_bendix::add_generating_pair(kb, "ba", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 11);
      REQUIRE(kb.number_of_classes() == 2);
    }
  }

  // 2-3-7 triangle group presentation
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "613",
                          "MAF: hyperbolic/t(2_3_7)",
                          "[quick][maf][hyperbolic]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("abc").contains_empty_word(true);

    presentation::add_rule(p, "aa", "");
    presentation::add_rule(p, "bb", "");
    presentation::add_rule(p, "cc", "");
    presentation::add_rule(p, "bcbc", "");
    presentation::add_rule(p, "cacaca", "");
    presentation::add_rule(p, "ababababababab", "");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb;

    SECTION("t(2_3_7)") {
      kb.init(congruence_kind::twosided, p);
      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 10);
      REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
    }

    SECTION("t(2_3_7).sub_237") {
      kb.init(congruence_kind::onesided, p);
      knuth_bendix::add_generating_pair(kb, "bc", "");
      knuth_bendix::add_generating_pair(kb, "cb", "");
      knuth_bendix::add_generating_pair(kb, "ca", "");
      knuth_bendix::add_generating_pair(kb, "ac", "");
      knuth_bendix::add_generating_pair(kb, "ab", "");
      knuth_bendix::add_generating_pair(kb, "ba", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 14);
      REQUIRE(kb.number_of_classes() == 2);
    }
  }

  // (2,4,5) triangle group - infinite hyperbolic
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "614",
                          "MAF: hyperbolic/t(2_4_5)",
                          "[quick][maf][hyperbolic]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("abc").contains_empty_word(true);

    presentation::add_rule(p, "aa", "");
    presentation::add_rule(p, "bb", "");
    presentation::add_rule(p, "cc", "");
    presentation::add_rule(p, "bcbc", "");
    presentation::add_rule(p, "cacacaca", "");
    presentation::add_rule(p, "ababababab", "");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb;

    SECTION("t(2_4_5)") {
      kb.init(congruence_kind::twosided, p);
      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 8);
      REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
    }

    SECTION("t(2_4_5).sub_GM") {
      kb.init(congruence_kind::onesided, p);
      knuth_bendix::add_generating_pair(kb, "c", "");
      knuth_bendix::add_generating_pair(kb, "ab", "");
      knuth_bendix::add_generating_pair(kb, "ba", "");
      knuth_bendix::add_generating_pair(kb, "bcb", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 12);
      REQUIRE(kb.number_of_classes() == 2);
    }
  }

  // (4,2,5) triangle group - infinite hyperbolic
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "615",
                          "MAF: hyperbolic/t(4_2_5)",
                          "[quick][maf][hyperbolic]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("abc").contains_empty_word(true);

    presentation::add_rule(p, "aa", "");
    presentation::add_rule(p, "bb", "");
    presentation::add_rule(p, "cc", "");
    presentation::add_rule(p, "bcbcbcbc", "");
    presentation::add_rule(p, "caca", "");
    presentation::add_rule(p, "ababababab", "");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb;

    SECTION("t(4_2_5)") {
      kb.init(congruence_kind::twosided, p);
      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 7);
      REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
    }

    SECTION("t(4_2_5).sub_GM") {
      kb.init(congruence_kind::onesided, p);
      knuth_bendix::add_generating_pair(kb, "c", "");
      knuth_bendix::add_generating_pair(kb, "ab", "");
      knuth_bendix::add_generating_pair(kb, "ba", "");
      knuth_bendix::add_generating_pair(kb, "bcb", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 11);
      REQUIRE(kb.number_of_classes() == 2);
    }

    SECTION("t(4_2_5).sub_X") {
      kb.init(congruence_kind::onesided, p);
      knuth_bendix::add_generating_pair(kb, "ac", "");
      knuth_bendix::add_generating_pair(kb, "ca", "");
      knuth_bendix::add_generating_pair(kb, "bacb", "");
      knuth_bendix::add_generating_pair(kb, "bcab", "");
      knuth_bendix::add_generating_pair(kb, "aba", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 10);
      REQUIRE(kb.number_of_classes() == 1);
    }
  }

  // (4,3,4) triangle group - infinite hyperbolic
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "616",
                          "MAF: hyperbolic/t(4_3_4)",
                          "[quick][maf][hyperbolic]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("abc").contains_empty_word(true);

    presentation::add_rule(p, "aa", "");
    presentation::add_rule(p, "bb", "");
    presentation::add_rule(p, "cc", "");
    presentation::add_rule(p, "bcbcbcbc", "");
    presentation::add_rule(p, "cacaca", "");
    presentation::add_rule(p, "abababab", "");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb;

    SECTION("t(4_3_4)") {
      kb.init(congruence_kind::twosided, p);
      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 9);
      REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
    }

    SECTION("t(4_3_4).sub") {
      kb.init(congruence_kind::onesided, p);
      knuth_bendix::add_generating_pair(kb, "bcb", "");
      knuth_bendix::add_generating_pair(kb, "c", "");
      knuth_bendix::add_generating_pair(kb, "a", "");
      knuth_bendix::add_generating_pair(kb, "bab", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 13);
      REQUIRE(kb.number_of_classes() == 2);
    }

    SECTION("t(4_3_4).sub_x") {
      kb.init(congruence_kind::onesided, p);
      knuth_bendix::add_generating_pair(kb, "ac", "");
      knuth_bendix::add_generating_pair(kb, "ca", "");
      knuth_bendix::add_generating_pair(kb, "bacb", "");
      knuth_bendix::add_generating_pair(kb, "bcab", "");
      knuth_bendix::add_generating_pair(kb, "aba", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 15);
      REQUIRE(kb.number_of_classes() == 4);
    }
  }

  // 5,2,4 triangle group presentation
  // b +
  // / |a
  // +---+
  // c
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "617",
                          "MAF: hyperbolic/t(5_2_4)",
                          "[quick][maf][hyperbolic]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("abc").contains_empty_word(true);

    presentation::add_rule(p, "aa", "");
    presentation::add_rule(p, "bb", "");
    presentation::add_rule(p, "cc", "");
    presentation::add_rule(p, "caca", "");
    presentation::add_rule(p, "babababa", "");
    presentation::add_rule(p, "cbcbcbcbcb", "");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb;

    SECTION("t(5_2_4)") {
      kb.init(congruence_kind::twosided, p);
      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 9);
      REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
    }

    SECTION("t(5_2_4).sub") {
      kb.init(congruence_kind::onesided, p);
      knuth_bendix::add_generating_pair(kb, "c", "");
      knuth_bendix::add_generating_pair(kb, "a", "");
      knuth_bendix::add_generating_pair(kb, "bab", "");
      knuth_bendix::add_generating_pair(kb, "bcb", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 12);
      REQUIRE(kb.number_of_classes() == 1);
    }

    SECTION("t(5_2_4).sub_x") {
      kb.init(congruence_kind::onesided, p);
      knuth_bendix::add_generating_pair(kb, "ac", "");
      knuth_bendix::add_generating_pair(kb, "ca", "");
      knuth_bendix::add_generating_pair(kb, "bacb", "");
      knuth_bendix::add_generating_pair(kb, "bcab", "");
      knuth_bendix::add_generating_pair(kb, "aba", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 12);
      REQUIRE(kb.number_of_classes() == 1);
    }
  }

  // 5,2,5 triangle group presentation
  // b +
  // / |a
  // +---+
  // c
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "618",
                          "MAF: hyperbolic/t(5_2_5)",
                          "[quick][maf][hyperbolic]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("abc").contains_empty_word(true);

    presentation::add_rule(p, "aa", "");
    presentation::add_rule(p, "bb", "");
    presentation::add_rule(p, "cc", "");
    presentation::add_rule(p, "caca", "");
    presentation::add_rule(p, "bababababa", "");
    presentation::add_rule(p, "cbcbcbcbcb", "");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb;

    SECTION("t(5_2_5)") {
      kb.init(congruence_kind::twosided, p);
      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 9);
      REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
    }

    SECTION("t(5_2_5).sub") {
      kb.init(congruence_kind::onesided, p);
      knuth_bendix::add_generating_pair(kb, "c", "");
      knuth_bendix::add_generating_pair(kb, "a", "");
      knuth_bendix::add_generating_pair(kb, "bab", "");
      knuth_bendix::add_generating_pair(kb, "bcb", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 12);
      REQUIRE(kb.number_of_classes() == 1);
    }
  }

  // 6,2,4 triangle group presentation
  // b +
  // / |a
  // +---+
  // c
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "619",
                          "MAF: hyperbolic/t(6_2_4)",
                          "[quick][maf][hyperbolic]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("abc").contains_empty_word(true);

    presentation::add_rule(p, "aa", "");
    presentation::add_rule(p, "bb", "");
    presentation::add_rule(p, "cc", "");
    presentation::add_rule(p, "caca", "");
    presentation::add_rule(p, "babababa", "");
    presentation::add_rule(p, "cbcbcbcbcbcb", "");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb;

    SECTION("t(6_2_4)") {
      kb.init(congruence_kind::twosided, p);
      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 7);
      REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
    }

    SECTION("t(6_2_4).sub") {
      kb.init(congruence_kind::onesided, p);
      knuth_bendix::add_generating_pair(kb, "c", "");
      knuth_bendix::add_generating_pair(kb, "a", "");
      knuth_bendix::add_generating_pair(kb, "bab", "");
      knuth_bendix::add_generating_pair(kb, "bcb", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 11);
      REQUIRE(kb.number_of_classes() == 2);
    }

    SECTION("t(6_2_4).sub5") {
      kb.init(congruence_kind::onesided, p);
      knuth_bendix::add_generating_pair(kb, "ac", "");
      knuth_bendix::add_generating_pair(kb, "bacb", "");
      knuth_bendix::add_generating_pair(kb, "aba", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 11);
      REQUIRE(kb.number_of_classes() == 2);
    }

    SECTION("t(6_2_4).sub_K") {
      kb.init(congruence_kind::onesided, p);
      knuth_bendix::add_generating_pair(kb, "a", "");
      knuth_bendix::add_generating_pair(kb, "bab", "");
      knuth_bendix::add_generating_pair(kb, "bcb", "");
      knuth_bendix::add_generating_pair(kb, "c", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 11);
      REQUIRE(kb.number_of_classes() == 2);
    }

    SECTION("t(6_2_4).sub_x") {
      kb.init(congruence_kind::onesided, p);
      knuth_bendix::add_generating_pair(kb, "ac", "");
      knuth_bendix::add_generating_pair(kb, "ca", "");
      knuth_bendix::add_generating_pair(kb, "bacb", "");
      knuth_bendix::add_generating_pair(kb, "bcab", "");
      knuth_bendix::add_generating_pair(kb, "aba", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 11);
      REQUIRE(kb.number_of_classes() == 2);
    }
  }

  // 6,2,6 triangle group presentation
  // b +
  // / |a
  // +---+
  // c
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "620",
                          "MAF: hyperbolic/t(6_2_6)",
                          "[quick][maf][hyperbolic]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("abc").contains_empty_word(true);

    presentation::add_rule(p, "aa", "");
    presentation::add_rule(p, "bb", "");
    presentation::add_rule(p, "cc", "");
    presentation::add_rule(p, "caca", "");
    presentation::add_rule(p, "babababababa", "");
    presentation::add_rule(p, "cbcbcbcbcbcb", "");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb;

    SECTION("t(6_2_6)") {
      kb.init(congruence_kind::twosided, p);
      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 7);
      REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
    }

    SECTION("t(6_2_6).sub") {
      kb.init(congruence_kind::onesided, p);
      knuth_bendix::add_generating_pair(kb, "c", "");
      knuth_bendix::add_generating_pair(kb, "a", "");
      knuth_bendix::add_generating_pair(kb, "bab", "");
      knuth_bendix::add_generating_pair(kb, "bcb", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 11);
      REQUIRE(kb.number_of_classes() == 2);
    }
  }

  // (6,2,4) triangle group - infinite hyperbolic
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "621",
                          "MAF: hyperbolic/t(6_3_4)",
                          "[quick][maf][hyperbolic]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("abc").contains_empty_word(true);

    presentation::add_rule(p, "aa", "");
    presentation::add_rule(p, "bb", "");
    presentation::add_rule(p, "cc", "");
    presentation::add_rule(p, "bcbcbcbcbcbc", "");
    presentation::add_rule(p, "cacaca", "");
    presentation::add_rule(p, "abababab", "");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb;

    SECTION("t(6_3_4)") {
      kb.init(congruence_kind::twosided, p);
      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 9);
      REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
    }

    SECTION("t(6_3_4).sub_K") {
      kb.init(congruence_kind::onesided, p);
      knuth_bendix::add_generating_pair(kb, "a", "");
      knuth_bendix::add_generating_pair(kb, "bab", "");
      knuth_bendix::add_generating_pair(kb, "bcb", "");
      knuth_bendix::add_generating_pair(kb, "c", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 13);
      REQUIRE(kb.number_of_classes() == 2);
    }
  }

  // (6,5,4) triangle group - infinite hyperbolic
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "622",
                          "MAF: hyperbolic/t(6_5_4)",
                          "[quick][maf][hyperbolic]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("abc").contains_empty_word(true);

    presentation::add_rule(p, "aa", "");
    presentation::add_rule(p, "bb", "");
    presentation::add_rule(p, "cc", "");
    presentation::add_rule(p, "bcbcbcbcbcbc", "");
    presentation::add_rule(p, "cacacacaca", "");
    presentation::add_rule(p, "abababab", "");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb;

    SECTION("t(6_5_4)") {
      kb.init(congruence_kind::twosided, p);
      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 9);
      REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
    }

    SECTION("t(6_5_4).sub_K") {
      kb.init(congruence_kind::onesided, p);
      knuth_bendix::add_generating_pair(kb, "a", "");
      knuth_bendix::add_generating_pair(kb, "bab", "");
      knuth_bendix::add_generating_pair(kb, "bcb", "");
      knuth_bendix::add_generating_pair(kb, "c", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 13);
      REQUIRE(kb.number_of_classes() == 2);
    }
  }

  // (7,2,3) triangle group - infinite hyperbolic
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "623",
                          "MAF: hyperbolic/t(7_2_3)",
                          "[quick][maf][hyperbolic]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("abc").contains_empty_word(true);

    presentation::add_rule(p, "aa", "");
    presentation::add_rule(p, "bb", "");
    presentation::add_rule(p, "cc", "");
    presentation::add_rule(p, "bcbcbcbcbcbcbc", "");
    presentation::add_rule(p, "caca", "");
    presentation::add_rule(p, "ababab", "");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb;

    SECTION("t(7_2_3)") {
      kb.init(congruence_kind::twosided, p);
      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 9);
      REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
    }

    SECTION("t(7_2_3).sub") {
      kb.init(congruence_kind::onesided, p);
      knuth_bendix::add_generating_pair(kb, "a", "");
      knuth_bendix::add_generating_pair(kb, "bcacb", "");
      knuth_bendix::add_generating_pair(kb, "cbabc", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 12);
      REQUIRE(kb.number_of_classes() == 1);
    }
  }

  // (8,2,3) triangle group - infinite hyperbolic
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "624",
                          "MAF: hyperbolic/t(8_2_3)",
                          "[quick][maf][hyperbolic]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("abc").contains_empty_word(true);

    presentation::add_rule(p, "aa", "");
    presentation::add_rule(p, "bb", "");
    presentation::add_rule(p, "cc", "");
    presentation::add_rule(p, "bcbcbcbcbcbcbcbc", "");
    presentation::add_rule(p, "caca", "");
    presentation::add_rule(p, "ababab", "");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb;

    SECTION("t(8_2_3)") {
      kb.init(congruence_kind::twosided, p);
      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 7);
      REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
    }

    SECTION("t(8_2_3).sub") {
      kb.init(congruence_kind::onesided, p);
      knuth_bendix::add_generating_pair(kb, "b", "");
      knuth_bendix::add_generating_pair(kb, "cbc", "");
      knuth_bendix::add_generating_pair(kb, "cac", "");
      knuth_bendix::add_generating_pair(kb, "a", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 10);
      REQUIRE(kb.number_of_classes() == 2);
    }

    SECTION("t(8_2_3).sub1") {
      kb.init(congruence_kind::onesided, p);
      knuth_bendix::add_generating_pair(kb, "a", "");
      knuth_bendix::add_generating_pair(kb, "bcacb", "");
      knuth_bendix::add_generating_pair(kb, "cbabc", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 10);
      REQUIRE(kb.number_of_classes() == 2);
    }
  }

  // (8,2,4) triangle group - infinite hyperbolic
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "625",
                          "MAF: hyperbolic/t(8_2_4)",
                          "[quick][maf][hyperbolic]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("abc").contains_empty_word(true);

    presentation::add_rule(p, "aa", "");
    presentation::add_rule(p, "bb", "");
    presentation::add_rule(p, "cc", "");
    presentation::add_rule(p, "bcbcbcbcbcbcbcbc", "");
    presentation::add_rule(p, "caca", "");
    presentation::add_rule(p, "abababab", "");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb;

    SECTION("t(8_2_4)") {
      kb.init(congruence_kind::twosided, p);
      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 7);
      REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
    }

    SECTION("t(8_2_4).sub_x") {
      kb.init(congruence_kind::onesided, p);
      knuth_bendix::add_generating_pair(kb, "ac", "");
      knuth_bendix::add_generating_pair(kb, "ca", "");
      knuth_bendix::add_generating_pair(kb, "bacb", "");
      knuth_bendix::add_generating_pair(kb, "bcab", "");
      knuth_bendix::add_generating_pair(kb, "aba", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 13);
      REQUIRE(kb.number_of_classes() == 4);
    }
  }

  // Free group of rank 2.
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "626",
                          "MAF: free/Free(2)",
                          "[quick][maf][free]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("aAbB").contains_empty_word(true);

    presentation::add_rule(p, "aA", "");
    presentation::add_rule(p, "Aa", "");
    presentation::add_rule(p, "bB", "");
    presentation::add_rule(p, "Bb", "");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb;

    SECTION("Free(2)") {
      kb.init(congruence_kind::twosided, p);
      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 4);
      REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
    }

    SECTION("Free(2).sub") {
      kb.init(congruence_kind::onesided, p);
      knuth_bendix::add_generating_pair(kb, "aba", "");
      knuth_bendix::add_generating_pair(kb, "AAb", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 8);
      REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
    }

    SECTION("Free(2).sub2") {
      kb.init(congruence_kind::onesided, p);
      knuth_bendix::add_generating_pair(kb, "aa", "");
      knuth_bendix::add_generating_pair(kb, "bb", "");
      knuth_bendix::add_generating_pair(kb, "abbA", "");
      knuth_bendix::add_generating_pair(kb, "baaB", "");
      knuth_bendix::add_generating_pair(kb, "abab", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 14);
      REQUIRE(kb.number_of_classes() == 4);
    }

    SECTION("Free(2).sub3") {
      kb.init(congruence_kind::onesided, p);
      knuth_bendix::add_generating_pair(kb, "aaa", "");
      knuth_bendix::add_generating_pair(kb, "bbb", "");
      knuth_bendix::add_generating_pair(kb, "abbbA", "");
      knuth_bendix::add_generating_pair(kb, "Abbba", "");
      knuth_bendix::add_generating_pair(kb, "baaaB", "");
      knuth_bendix::add_generating_pair(kb, "Baaab", "");
      knuth_bendix::add_generating_pair(kb, "ababab", "");
      knuth_bendix::add_generating_pair(kb, "aBaBaB", "");
      knuth_bendix::add_generating_pair(kb, "AbAbAb", "");
      knuth_bendix::add_generating_pair(kb, "ABABAB", "");
      knuth_bendix::add_generating_pair(kb, "abaaaBA", "");
      knuth_bendix::add_generating_pair(kb, "aBaaabA", "");
      knuth_bendix::add_generating_pair(kb, "AbaaaBa", "");
      knuth_bendix::add_generating_pair(kb, "ABaaaba", "");
      knuth_bendix::add_generating_pair(kb, "babbbAB", "");
      knuth_bendix::add_generating_pair(kb, "bAbbbaB", "");
      knuth_bendix::add_generating_pair(kb, "BabbbAb", "");
      knuth_bendix::add_generating_pair(kb, "BAbbbab", "");
      knuth_bendix::add_generating_pair(kb, "aabababA", "");
      knuth_bendix::add_generating_pair(kb, "aaBaBaBA", "");
      knuth_bendix::add_generating_pair(kb, "bbababaB", "");
      knuth_bendix::add_generating_pair(kb, "BBaBaBab", "");
      knuth_bendix::add_generating_pair(kb, "abaBaBaBBA", "");
      knuth_bendix::add_generating_pair(kb, "abABABABBA", "");
      knuth_bendix::add_generating_pair(kb, "AbABABABBa", "");
      knuth_bendix::add_generating_pair(kb, "ABAbAbAbba", "");
      knuth_bendix::add_generating_pair(kb, "baaBaBaBAB", "");
      knuth_bendix::add_generating_pair(kb, "bAABABABaB", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 60);
      REQUIRE(kb.number_of_classes() == 27);
    }
  }

  // Free group of rank 3.
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "627",
                          "MAF: free/free(3)",
                          "[quick][maf][free]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("aAbBcC").contains_empty_word(true);

    presentation::add_rule(p, "aA", "");
    presentation::add_rule(p, "Aa", "");
    presentation::add_rule(p, "bB", "");
    presentation::add_rule(p, "Bb", "");
    presentation::add_rule(p, "cC", "");
    presentation::add_rule(p, "Cc", "");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb;

    SECTION("free(3)") {
      kb.init(congruence_kind::twosided, p);
      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 6);
      REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
    }

    SECTION("free(3).sub2") {
      kb.init(congruence_kind::onesided, p);
      knuth_bendix::add_generating_pair(kb, "aa", "");
      knuth_bendix::add_generating_pair(kb, "bb", "");
      knuth_bendix::add_generating_pair(kb, "cc", "");
      knuth_bendix::add_generating_pair(kb, "abbA", "");
      knuth_bendix::add_generating_pair(kb, "bccB", "");
      knuth_bendix::add_generating_pair(kb, "caaC", "");
      knuth_bendix::add_generating_pair(kb, "accA", "");
      knuth_bendix::add_generating_pair(kb, "baaB", "");
      knuth_bendix::add_generating_pair(kb, "cbbC", "");
      knuth_bendix::add_generating_pair(kb, "abab", "");
      knuth_bendix::add_generating_pair(kb, "bcbc", "");
      knuth_bendix::add_generating_pair(kb, "caca", "");
      knuth_bendix::add_generating_pair(kb, "abcabc", "");
      knuth_bendix::add_generating_pair(kb, "acbacb", "");
      knuth_bendix::add_generating_pair(kb, "abccBA", "");
      knuth_bendix::add_generating_pair(kb, "bcaaCB", "");
      knuth_bendix::add_generating_pair(kb, "cabbAC", "");
      knuth_bendix::add_generating_pair(kb, "bcabca", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 40);
      REQUIRE(kb.number_of_classes() == 8);
    }
  }

  // free group of rank 3
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "628",
                          "MAF: free/free(3)_xyz",
                          "[quick][maf][free]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("xXyYzZ").contains_empty_word(true);

    presentation::add_rule(p, "xX", "");
    presentation::add_rule(p, "Xx", "");
    presentation::add_rule(p, "yY", "");
    presentation::add_rule(p, "Yy", "");
    presentation::add_rule(p, "zZ", "");
    presentation::add_rule(p, "Zz", "");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb;

    SECTION("free(3)_xyz") {
      kb.init(congruence_kind::twosided, p);
      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 6);
      REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
    }

    SECTION("free(3)_xyz.sub1") {
      kb.init(congruence_kind::onesided, p);
      knuth_bendix::add_generating_pair(kb, "xx", "");
      knuth_bendix::add_generating_pair(kb, "xyX", "");
      knuth_bendix::add_generating_pair(kb, "yXy", "");
      knuth_bendix::add_generating_pair(kb, "yyy", "");
      knuth_bendix::add_generating_pair(kb, "yxxy", "");
      knuth_bendix::add_generating_pair(kb, "yxZY", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 18);
      REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
    }

    SECTION("free(3)_xyz.sub2") {
      kb.init(congruence_kind::onesided, p);
      knuth_bendix::add_generating_pair(kb, "YXxxxy", "");
      knuth_bendix::add_generating_pair(kb, "YXxyXxy", "");
      knuth_bendix::add_generating_pair(kb, "YXyXyxy", "");
      knuth_bendix::add_generating_pair(kb, "YXyyyxy", "");
      knuth_bendix::add_generating_pair(kb, "YXyxxyxy", "");
      knuth_bendix::add_generating_pair(kb, "YXyxZYxy", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 18);
      REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
    }

    SECTION("free(3)_xyz.sub3") {
      kb.init(congruence_kind::onesided, p);
      knuth_bendix::add_generating_pair(kb, "xx", "");
      knuth_bendix::add_generating_pair(kb, "yyy", "");
      knuth_bendix::add_generating_pair(kb, "yxxy", "");
      knuth_bendix::add_generating_pair(kb, "xyXxyXxyX", "");
      knuth_bendix::add_generating_pair(kb, "xyxxyX", "");
      knuth_bendix::add_generating_pair(kb, "xyXyXy", "");
      knuth_bendix::add_generating_pair(kb, "xYxYxY", "");
      knuth_bendix::add_generating_pair(kb, "yXXXXy", "");
      knuth_bendix::add_generating_pair(kb, "xyXYYXy", "");
      knuth_bendix::add_generating_pair(kb, "xYxyyxY", "");
      knuth_bendix::add_generating_pair(kb, "xyXXXXyX", "");
      knuth_bendix::add_generating_pair(kb, "yXYxyxYyXYxyxYyXYxyxY", "");
      knuth_bendix::add_generating_pair(kb, "YxyxYXyYxyxYXyYxyxYXy", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 32);
      REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
    }
  }

  // Free group of rank 8.
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "629",
                          "MAF: free/free(8)",
                          "[quick][maf][free]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("aAbBcCdDeEfFgGhH").contains_empty_word(true);

    presentation::add_rule(p, "aA", "");
    presentation::add_rule(p, "Aa", "");
    presentation::add_rule(p, "bB", "");
    presentation::add_rule(p, "Bb", "");
    presentation::add_rule(p, "cC", "");
    presentation::add_rule(p, "Cc", "");
    presentation::add_rule(p, "dD", "");
    presentation::add_rule(p, "Dd", "");
    presentation::add_rule(p, "eE", "");
    presentation::add_rule(p, "Ee", "");
    presentation::add_rule(p, "fF", "");
    presentation::add_rule(p, "Ff", "");
    presentation::add_rule(p, "gG", "");
    presentation::add_rule(p, "Gg", "");
    presentation::add_rule(p, "hH", "");
    presentation::add_rule(p, "Hh", "");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb;
    kb.init(congruence_kind::twosided, p);
    kb.run();
    REQUIRE(kb.rewriting_system().number_of_rules() == 16);
    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
  }

  // Fibonacci group F(2,5)
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "630",
                          "MAF: Fibonacci/f2_5",
                          "[quick][maf][Fibonacci]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("aAbBcCdDeE").contains_empty_word(true);

    presentation::add_rule(p, "aA", "");
    presentation::add_rule(p, "Aa", "");
    presentation::add_rule(p, "bB", "");
    presentation::add_rule(p, "Bb", "");
    presentation::add_rule(p, "cC", "");
    presentation::add_rule(p, "Cc", "");
    presentation::add_rule(p, "dD", "");
    presentation::add_rule(p, "Dd", "");
    presentation::add_rule(p, "eE", "");
    presentation::add_rule(p, "Ee", "");
    presentation::add_rule(p, "ab", "c");
    presentation::add_rule(p, "bc", "d");
    presentation::add_rule(p, "cd", "e");
    presentation::add_rule(p, "de", "a");
    presentation::add_rule(p, "ea", "b");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb;
    kb.init(congruence_kind::twosided, p);
    kb.run();
    REQUIRE(kb.rewriting_system().number_of_rules() == 100);
    REQUIRE(kb.number_of_classes() == 11);
  }

  // Fibonacci group F(3,5).
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "631",
                          "MAF: Fibonacci/f3_5",
                          "[quick][maf][Fibonacci]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("aAbBcCdDeE").contains_empty_word(true);

    presentation::add_rule(p, "aA", "");
    presentation::add_rule(p, "Aa", "");
    presentation::add_rule(p, "bB", "");
    presentation::add_rule(p, "Bb", "");
    presentation::add_rule(p, "cC", "");
    presentation::add_rule(p, "Cc", "");
    presentation::add_rule(p, "dD", "");
    presentation::add_rule(p, "Dd", "");
    presentation::add_rule(p, "eE", "");
    presentation::add_rule(p, "Ee", "");
    presentation::add_rule(p, "abc", "d");
    presentation::add_rule(p, "bcd", "e");
    presentation::add_rule(p, "cde", "a");
    presentation::add_rule(p, "dea", "b");
    presentation::add_rule(p, "eab", "c");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb;

    SECTION("f3_5") {
      kb.init(congruence_kind::twosided, p);
      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 98);
      REQUIRE(kb.number_of_classes() == 22);
    }

    SECTION("f3_5.sub") {
      kb.init(congruence_kind::onesided, p);
      knuth_bendix::add_generating_pair(kb, "c", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 108);
      REQUIRE(kb.number_of_classes() == 1);
    }
  }

  // Von Dyck (2,3,5) group - isomorphic to A_5
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "632",
                          "MAF: elliptic/D(2_3_5)",
                          "[quick][maf][elliptic]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("abB").contains_empty_word(true);

    presentation::add_rule(p, "aa", "");
    presentation::add_rule(p, "bB", "");
    presentation::add_rule(p, "Bb", "");
    presentation::add_rule(p, "bbb", "");
    presentation::add_rule(p, "ababababab", "");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb;

    SECTION("D(2_3_5)") {
      kb.init(congruence_kind::twosided, p);
      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 17);
      REQUIRE(kb.number_of_classes() == 60);
    }

    SECTION("D(2_3_5).sub") {
      kb.init(congruence_kind::onesided, p);
      knuth_bendix::add_generating_pair(kb, "aB", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 23);
      REQUIRE(kb.number_of_classes() == 12);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "633",
                          "MAF: elliptic/t(4_2_2)",
                          "[quick][maf][elliptic]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("abc").contains_empty_word(true);

    presentation::add_rule(p, "aa", "");
    presentation::add_rule(p, "bb", "");
    presentation::add_rule(p, "cc", "");
    presentation::add_rule(p, "bcbcbcbc", "");
    presentation::add_rule(p, "caca", "");
    presentation::add_rule(p, "abab", "");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb;
    kb.init(congruence_kind::twosided, p);
    kb.run();
    REQUIRE(kb.rewriting_system().number_of_rules() == 6);
    REQUIRE(kb.number_of_classes() == 16);
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "634",
                          "MAF: elliptic/t(4_2_3)",
                          "[quick][maf][elliptic]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("abc").contains_empty_word(true);

    presentation::add_rule(p, "aa", "");
    presentation::add_rule(p, "bb", "");
    presentation::add_rule(p, "cc", "");
    presentation::add_rule(p, "bcbcbcbc", "");
    presentation::add_rule(p, "caca", "");
    presentation::add_rule(p, "ababab", "");
    presentation::add_rule(p, "abcbabcbc", "");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb;
    kb.init(congruence_kind::twosided, p);
    kb.run();
    REQUIRE(kb.rewriting_system().number_of_rules() == 14);
    REQUIRE(kb.number_of_classes() == 24);
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "635",
                          "MAF: elliptic/t(5_2_3)",
                          "[quick][maf][elliptic]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("abc").contains_empty_word(true);

    presentation::add_rule(p, "aa", "");
    presentation::add_rule(p, "bb", "");
    presentation::add_rule(p, "cc", "");
    presentation::add_rule(p, "bcbcbcbcbc", "");
    presentation::add_rule(p, "caca", "");
    presentation::add_rule(p, "ababab", "");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb;
    kb.init(congruence_kind::twosided, p);
    kb.run();
    REQUIRE(kb.rewriting_system().number_of_rules() == 9);
    REQUIRE(kb.number_of_classes() == 120);
  }

  // Coxeter graph
  // a-b-c
  // \ /
  // d
  // Coxeter Matrix (above upper diagonal)
  // 1 3 2 3
  // 1 3 2
  // 1 3
  // 1
  //
  // Coxeter group ~A3
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "636",
                          "MAF: coxeter/~a3",
                          "[quick][maf][coxeter]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("abcd").contains_empty_word(true);

    presentation::add_rule(p, "aa", "");
    presentation::add_rule(p, "bb", "");
    presentation::add_rule(p, "cc", "");
    presentation::add_rule(p, "dd", "");
    presentation::add_rule(p, "aa", "");
    presentation::add_rule(p, "bb", "");
    presentation::add_rule(p, "cc", "");
    presentation::add_rule(p, "dd", "");
    presentation::add_rule(p, "bab", "aba");
    presentation::add_rule(p, "ca", "ac");
    presentation::add_rule(p, "dad", "ada");
    presentation::add_rule(p, "cbc", "bcb");
    presentation::add_rule(p, "db", "bd");
    presentation::add_rule(p, "dcd", "cdc");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb;
    kb.init(congruence_kind::twosided, p);
    kb.run();
    REQUIRE(kb.rewriting_system().number_of_rules() == 27);
    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
  }

  // Coxeter graph
  // a-b-c-d
  // \ /
  // e
  // Coxeter Matrix (above upper diagonal)
  // 1 3 2 2 3
  // 1 3 2 2
  // 1 3 2
  // 1 3
  // 1
  // Coxeter group ~A4
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "637",
                          "MAF: coxeter/~a4",
                          "[quick][maf][coxeter]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("abcde").contains_empty_word(true);

    presentation::add_rule(p, "aa", "");
    presentation::add_rule(p, "bb", "");
    presentation::add_rule(p, "cc", "");
    presentation::add_rule(p, "dd", "");
    presentation::add_rule(p, "ee", "");
    presentation::add_rule(p, "bab", "aba");
    presentation::add_rule(p, "ca", "ac");
    presentation::add_rule(p, "da", "ad");
    presentation::add_rule(p, "eae", "aea");
    presentation::add_rule(p, "cbc", "bcb");
    presentation::add_rule(p, "db", "bd");
    presentation::add_rule(p, "eb", "be");
    presentation::add_rule(p, "dcd", "cdc");
    presentation::add_rule(p, "ec", "ce");
    presentation::add_rule(p, "ede", "ded");  // codespell:ignore

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb;
    kb.init(congruence_kind::twosided, p);
    kb.run();
    REQUIRE(kb.rewriting_system().number_of_rules() == 58);
    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
  }

  // Coxeter graph
  //
  // 4 4
  // d-a-b-c
  //
  // Coxeter Matrix (above upper diagonal)
  // 1 3 2 4
  // 1 4 2
  // 1 2
  // 1
  // Weyl group ~B3 (all gens involutory).
  // Symmetry group of cubic tessellation
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "638",
                          "MAF: coxeter/~b3",
                          "[quick][maf][coxeter]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("dabc").contains_empty_word(true);

    presentation::add_rule(p, "dd", "");
    presentation::add_rule(p, "aa", "");
    presentation::add_rule(p, "bb", "");
    presentation::add_rule(p, "cc", "");
    presentation::add_rule(p, "bab", "aba");
    presentation::add_rule(p, "ca", "ac");
    presentation::add_rule(p, "adadadad", "");
    presentation::add_rule(p, "cbcbcbcb", "");
    presentation::add_rule(p, "db", "bd");
    presentation::add_rule(p, "dc", "cd");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb;
    kb.init(congruence_kind::twosided, p);
    kb.run();
    REQUIRE(kb.rewriting_system().number_of_rules() == 15);
    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
  }

  // Coxeter graph
  // d
  // |
  // a-b-c
  // 4
  //
  // Coxeter Matrix (above upper diagonal)
  // 1 3 2 2
  // 1 4 3
  // 1 2
  // 1
  // Weyl group ~C3 (all gens involutory).
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "639",
                          "MAF: coxeter/~c3",
                          "[quick][maf][coxeter]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("dabc").contains_empty_word(true);

    presentation::add_rule(p, "dd", "");
    presentation::add_rule(p, "aa", "");
    presentation::add_rule(p, "bb", "");
    presentation::add_rule(p, "cc", "");
    presentation::add_rule(p, "bab", "aba");
    presentation::add_rule(p, "ca", "ac");
    presentation::add_rule(p, "da", "ad");
    presentation::add_rule(p, "cbcbcbcb", "");
    presentation::add_rule(p, "bdbdbd", "");
    presentation::add_rule(p, "dc", "cd");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb;
    kb.init(congruence_kind::twosided, p);
    kb.run();
    REQUIRE(kb.rewriting_system().number_of_rules() == 16);
    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
  }

  // Coxeter graph
  // d
  // |
  // a-b-c
  // |
  // e
  // Coxeter Matrix (above upper diagonal)
  // 1 3 2 2 2
  // 1 3 3 3
  // 1 2 2
  // 1 2
  // 1
  // Weyl group ~D4.
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "640",
                          "MAF: coxeter/~d4",
                          "[quick][maf][coxeter]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("abcde").contains_empty_word(true);

    presentation::add_rule(p, "aa", "");
    presentation::add_rule(p, "bb", "");
    presentation::add_rule(p, "cc", "");
    presentation::add_rule(p, "dd", "");
    presentation::add_rule(p, "ee", "");
    presentation::add_rule(p, "ababab", "");
    presentation::add_rule(p, "ca", "ac");
    presentation::add_rule(p, "da", "ad");
    presentation::add_rule(p, "ea", "ae");
    presentation::add_rule(p, "bcbcbc", "");
    presentation::add_rule(p, "bdbdbd", "");
    presentation::add_rule(p, "bebebe", "");
    presentation::add_rule(p, "dc", "cd");
    presentation::add_rule(p, "ec", "ce");
    presentation::add_rule(p, "ed", "de");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb;
    kb.init(congruence_kind::twosided, p);
    kb.run();
    REQUIRE(kb.rewriting_system().number_of_rules() == 67);
    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
  }

  // Coxeter graph
  // d
  // |
  // a-b-c-e
  // |
  // f
  // Coxeter Matrix (above upper diagonal)
  // 1 3 2 2 2 2
  // 1 3 2 2 3
  // 1 3 3 2
  // 1 2 2
  // 1 2
  // 1
  //
  //
  // Weyl group ~D5
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "641",
                          "MAF: coxeter/~d5",
                          "[quick][maf][coxeter]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("abcdef").contains_empty_word(true);

    presentation::add_rule(p, "aa", "");
    presentation::add_rule(p, "bb", "");
    presentation::add_rule(p, "cc", "");
    presentation::add_rule(p, "dd", "");
    presentation::add_rule(p, "ee", "");
    presentation::add_rule(p, "ff", "");
    presentation::add_rule(p, "bab", "aba");
    presentation::add_rule(p, "ca", "ac");
    presentation::add_rule(p, "da", "ad");
    presentation::add_rule(p, "ea", "ae");
    presentation::add_rule(p, "fa", "af");
    presentation::add_rule(p, "cbc", "bcb");
    presentation::add_rule(p, "db", "bd");
    presentation::add_rule(p, "eb", "be");
    presentation::add_rule(p, "fbf", "bfb");
    presentation::add_rule(p, "dcd", "cdc");
    presentation::add_rule(p, "ece", "cec");
    presentation::add_rule(p, "fc", "cf");
    presentation::add_rule(p, "ed", "de");
    presentation::add_rule(p, "fd", "df");
    presentation::add_rule(p, "fe", "ef");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb;
    kb.init(congruence_kind::twosided, p);
    kb.run();
    REQUIRE(kb.rewriting_system().number_of_rules() == 139);
    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
  }

  // Coxeter graph
  //
  // 6
  // a-b-c
  //
  // Coxeter Matrix (above upper diagonal)
  // 1 3 2
  // 1 6
  // 1
  //
  // Weyl group ~H2 (all gens involutory).
  // Symmetry group of tiling of hexagons
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "642",
                          "MAF: coxeter/~h2",
                          "[quick][maf][coxeter]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("abc").contains_empty_word(true);

    presentation::add_rule(p, "aa", "");
    presentation::add_rule(p, "bb", "");
    presentation::add_rule(p, "cc", "");
    presentation::add_rule(p, "bab", "aba");
    presentation::add_rule(p, "ca", "ac");
    presentation::add_rule(p, "bcbcbcbcbcbc", "");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb;
    kb.init(congruence_kind::twosided, p);
    kb.run();
    REQUIRE(kb.rewriting_system().number_of_rules() == 7);
    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
  }

  // (7,7,7) Triangle group
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "643",
                          "MAF: coxeter/777",
                          "[quick][maf][coxeter]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("fedcba").contains_empty_word(true);

    presentation::add_rule(p, "ff", "");
    presentation::add_rule(p, "ee", "");
    presentation::add_rule(p, "dd", "");
    presentation::add_rule(p, "cc", "");
    presentation::add_rule(p, "bb", "");
    presentation::add_rule(p, "aa", "");
    presentation::add_rule(p, "ababababababab", "");
    presentation::add_rule(p, "acacacacacacac", "");
    presentation::add_rule(p, "bcbcbcbcbcbcbc", "");
    presentation::add_rule(p, "f", "abababa");
    presentation::add_rule(p, "e", "acacaca");
    presentation::add_rule(p, "d", "bcbcbcb");

    using RewritingSystem = detail::RewritingSystemTrie<WtLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb;
    kb.init(
        congruence_kind::twosided, p, std::vector<size_t>({7, 7, 7, 1, 1, 1}));
    kb.run();
    REQUIRE(kb.rewriting_system().number_of_rules() == 36);
    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
  }

  // Coxeter Group A15 - Symmetric group Sym(16)
  // Sym(n) is generated using n-1 generators.
  // a=(12),b=(23),... Each generator multiplied by the next is a 3 cycle
  // but commutes with all the remaining generators.
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "644",
                          "MAF: coxeter/a15",
                          "[quick][maf][coxeter]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("abcdefghijklmno").contains_empty_word(true);

    presentation::add_rule(p, "aa", "");
    presentation::add_rule(p, "bb", "");
    presentation::add_rule(p, "cc", "");
    presentation::add_rule(p, "dd", "");
    presentation::add_rule(p, "ee", "");
    presentation::add_rule(p, "ff", "");
    presentation::add_rule(p, "gg", "");
    presentation::add_rule(p, "hh", "");
    presentation::add_rule(p, "ii", "");
    presentation::add_rule(p, "jj", "");
    presentation::add_rule(p, "kk", "");
    presentation::add_rule(p, "ll", "");
    presentation::add_rule(p, "mm", "");
    presentation::add_rule(p, "nn", "");
    presentation::add_rule(p, "oo", "");
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
    presentation::add_rule(p, "ede", "ded");  // codespell:ignore
    presentation::add_rule(p, "fd", "df");
    presentation::add_rule(p, "gd", "dg");
    presentation::add_rule(p, "hd", "dh");
    presentation::add_rule(p, "id", "di");
    presentation::add_rule(p, "jd", "dj");
    presentation::add_rule(p, "kd", "dk");
    presentation::add_rule(p, "ld", "dl");
    presentation::add_rule(p, "md", "dm");
    presentation::add_rule(p, "nd", "dn");  // codespell:ignore
    presentation::add_rule(p, "od", "do");  // codespell:ignore
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
    presentation::add_rule(p, "of", "fo");  // codespell:ignore
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

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb;
    kb.init(congruence_kind::twosided, p);
    kb.run();
    REQUIRE(kb.rewriting_system().number_of_rules() == 211);
    REQUIRE(kb.number_of_classes() == 20922789888000);
  }

  // Coxeter graph
  //
  // 4
  // a-b-c
  //
  // Coxeter Matrix (above upper diagonal)
  // 1 3 2
  // 1 4
  // 1
  //
  // Weyl group B3/C3 (all gens involutory).
  // Symmetry group of cube/octahedron
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "645",
                          "MAF: coxeter/b3",
                          "[quick][maf][coxeter]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("abc").contains_empty_word(true);

    presentation::add_rule(p, "aa", "");
    presentation::add_rule(p, "bb", "");
    presentation::add_rule(p, "cc", "");
    presentation::add_rule(p, "bab", "aba");
    presentation::add_rule(p, "ca", "ac");
    presentation::add_rule(p, "cbcbcbcb", "");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb;
    kb.init(congruence_kind::twosided, p);
    kb.run();
    REQUIRE(kb.rewriting_system().number_of_rules() == 7);
    REQUIRE(kb.number_of_classes() == 48);
  }

  // Coxeter graph
  //
  // 4
  // a-b-c-d
  //
  // Coxeter Matrix (above upper diagonal)
  // 1 3 2 2
  // 1 3 2
  // 1 4
  // 1
  //
  // Weyl group B4 (all gens involutory).
  // Symmetry group of hypercube
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "646",
                          "MAF: coxeter/b4",
                          "[quick][maf][coxeter]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("abcd").contains_empty_word(true);

    presentation::add_rule(p, "aa", "");
    presentation::add_rule(p, "bb", "");
    presentation::add_rule(p, "cc", "");
    presentation::add_rule(p, "dd", "");
    presentation::add_rule(p, "bab", "aba");
    presentation::add_rule(p, "ca", "ac");
    presentation::add_rule(p, "da", "ad");
    presentation::add_rule(p, "cbc", "bcb");
    presentation::add_rule(p, "db", "bd");
    presentation::add_rule(p, "cdcdcdcd", "");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb;
    kb.init(congruence_kind::twosided, p);
    kb.run();
    REQUIRE(kb.rewriting_system().number_of_rules() == 13);
    REQUIRE(kb.number_of_classes() == 384);
  }

  // Coxeter group
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "647",
                          "MAF: coxeter/cox3363",
                          "[quick][maf][coxeter]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("abcd").contains_empty_word(true);

    presentation::add_rule(p, "aa", "");
    presentation::add_rule(p, "bb", "");
    presentation::add_rule(p, "cc", "");
    presentation::add_rule(p, "dd", "");
    presentation::add_rule(p, "aa", "");
    presentation::add_rule(p, "bb", "");
    presentation::add_rule(p, "cc", "");
    presentation::add_rule(p, "dd", "");
    presentation::add_rule(p, "bab", "aba");
    presentation::add_rule(p, "cac", "aca");
    presentation::add_rule(p, "dadada", "adadad");
    presentation::add_rule(p, "cbc", "bcb");
    presentation::add_rule(p, "db", "bd");
    presentation::add_rule(p, "dc", "cd");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb;

    SECTION("cox3363") {
      kb.init(congruence_kind::twosided, p);
      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 19);
      REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
    }

    SECTION("cox3363.sub") {
      kb.init(congruence_kind::onesided, p);
      knuth_bendix::add_generating_pair(kb, "a", "");
      knuth_bendix::add_generating_pair(kb, "b", "");
      knuth_bendix::add_generating_pair(kb, "c", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 22);
      REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
    }
  }

  // Coxeter graph
  // d
  // |
  // a-b-c-e-f
  //
  // Coxeter Matrix (above upper diagonal)
  // 1 3 2 2 2 2
  // 1 3 2 2 2
  // 1 3 3 2
  // 1 2 2
  // 1 3
  // 1
  //
  //
  // Weyl group E6 (all gens involutory).
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "648",
                          "MAF: coxeter/e6",
                          "[quick][maf][coxeter]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("abcdef").contains_empty_word(true);

    presentation::add_rule(p, "aa", "");
    presentation::add_rule(p, "bb", "");
    presentation::add_rule(p, "cc", "");
    presentation::add_rule(p, "dd", "");
    presentation::add_rule(p, "ee", "");
    presentation::add_rule(p, "ff", "");
    presentation::add_rule(p, "bab", "aba");
    presentation::add_rule(p, "ca", "ac");
    presentation::add_rule(p, "da", "ad");
    presentation::add_rule(p, "ea", "ae");
    presentation::add_rule(p, "fa", "af");
    presentation::add_rule(p, "cbc", "bcb");
    presentation::add_rule(p, "db", "bd");
    presentation::add_rule(p, "eb", "be");
    presentation::add_rule(p, "fb", "bf");
    presentation::add_rule(p, "dcd", "cdc");
    presentation::add_rule(p, "ece", "cec");
    presentation::add_rule(p, "fc", "cf");
    presentation::add_rule(p, "ed", "de");
    presentation::add_rule(p, "fd", "df");
    presentation::add_rule(p, "fef", "efe");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb;
    kb.init(congruence_kind::twosided, p);
    kb.run();
    REQUIRE(kb.rewriting_system().number_of_rules() == 51);
    REQUIRE(kb.number_of_classes() == 51840);
  }

  // Braid group b3
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "649",
                          "MAF: braid/b3new",
                          "[quick][maf][braid]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("aAbBtTcCdD").contains_empty_word(true);

    presentation::add_rule(p, "aA", "");
    presentation::add_rule(p, "Aa", "");
    presentation::add_rule(p, "bB", "");
    presentation::add_rule(p, "Bb", "");
    presentation::add_rule(p, "tT", "");
    presentation::add_rule(p, "Tt", "");
    presentation::add_rule(p, "cC", "");
    presentation::add_rule(p, "Cc", "");
    presentation::add_rule(p, "dD", "");
    presentation::add_rule(p, "Dd", "");
    presentation::add_rule(p, "dcd", "cdc");
    presentation::add_rule(p, "t", "c");
    presentation::add_rule(p, "b", "dT");
    presentation::add_rule(p, "a", "tbT");

    using RewritingSystem = detail::RewritingSystemTrie<RevRPOCmp>;
    KnuthBendix<std::string, RewritingSystem> kb;
    kb.init(congruence_kind::twosided, p);
    kb.run();
    REQUIRE(kb.rewriting_system().number_of_rules() == 18);
    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
  }

  // 3-fold cover of A_6
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "650",
                          "MAF: both/3a6",
                          "[quick][maf][both]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("abAB").contains_empty_word(true);

    presentation::add_rule(p, "aA", "");
    presentation::add_rule(p, "bB", "");
    presentation::add_rule(p, "Aa", "");
    presentation::add_rule(p, "Bb", "");
    presentation::add_rule(p, "aaa", "");
    presentation::add_rule(p, "bbb", "");
    presentation::add_rule(p, "abababab", "");
    presentation::add_rule(p, "aBaBaBaBaB", "");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb;
    kb.init(congruence_kind::twosided, p);
    kb.run();
    REQUIRE(kb.rewriting_system().number_of_rules() == 183);
    REQUIRE(kb.number_of_classes() == 1080);
  }

  // Group is A_4. Generators named by the enumerated prefix method.
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "651",
                          "MAF: both/a4",
                          "[quick][maf][both]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("abc").contains_empty_word(true);

    presentation::add_rule(p, "aa", "");
    presentation::add_rule(p, "bc", "");
    presentation::add_rule(p, "cb", "");
    presentation::add_rule(p, "bb", "c");
    presentation::add_rule(p, "cac", "aba");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb;
    kb.init(congruence_kind::twosided, p);
    kb.run();
    REQUIRE(kb.rewriting_system().number_of_rules() == 11);
    REQUIRE(kb.number_of_classes() == 12);
  }

  // Cyclic group of order 2
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "652",
                          "MAF: both/c2",
                          "[quick][maf][both]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("a").contains_empty_word(true);

    presentation::add_rule(p, "aa", "");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb;
    kb.init(congruence_kind::twosided, p);
    kb.run();
    REQUIRE(kb.rewriting_system().number_of_rules() == 1);
    REQUIRE(kb.number_of_classes() == 2);
  }

  // Cyclic group of order 5
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "653",
                          "MAF: both/c5",
                          "[quick][maf][both]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("aA").contains_empty_word(true);

    presentation::add_rule(p, "aA", "");
    presentation::add_rule(p, "Aa", "");
    presentation::add_rule(p, "aaaaa", "");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb;
    kb.init(congruence_kind::twosided, p);
    kb.run();
    REQUIRE(kb.rewriting_system().number_of_rules() == 4);
    REQUIRE(kb.number_of_classes() == 5);
  }

  // Von Dyck (2,3,5) group - isomorphic to A_5
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "654",
                          "MAF: both/d(2_3_5)",
                          "[quick][maf][both]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("abB").contains_empty_word(true);

    presentation::add_rule(p, "aa", "");
    presentation::add_rule(p, "bB", "");
    presentation::add_rule(p, "Bb", "");
    presentation::add_rule(p, "bbb", "");
    presentation::add_rule(p, "ababababab", "");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb;
    kb.init(congruence_kind::twosided, p);
    kb.run();
    REQUIRE(kb.rewriting_system().number_of_rules() == 17);
    REQUIRE(kb.number_of_classes() == 60);
  }

  // This group is actually D_22 (although it wasn't meant to be).
  // All generators are unexpectedly involutory.
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "655",
                          "MAF: both/d22",
                          "[quick][maf][both]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("aAbBcCdDeEfF").contains_empty_word(true);

    presentation::add_rule(p, "aA", "");
    presentation::add_rule(p, "Aa", "");
    presentation::add_rule(p, "bB", "");
    presentation::add_rule(p, "Bb", "");
    presentation::add_rule(p, "cC", "");
    presentation::add_rule(p, "Cc", "");
    presentation::add_rule(p, "dD", "");
    presentation::add_rule(p, "Dd", "");
    presentation::add_rule(p, "eE", "");
    presentation::add_rule(p, "Ee", "");
    presentation::add_rule(p, "fF", "");
    presentation::add_rule(p, "Ff", "");
    presentation::add_rule(p, "aCAd", "");
    presentation::add_rule(p, "bfBE", "");
    presentation::add_rule(p, "ceCD", "");
    presentation::add_rule(p, "dFDa", "");
    presentation::add_rule(p, "ebEA", "");
    presentation::add_rule(p, "fCFB", "");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb;

    SECTION("d22") {
      kb.init(congruence_kind::twosided, p);
      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 41);
      REQUIRE(kb.number_of_classes() == 22);
    }

    SECTION("d22.sub") {
      kb.init(congruence_kind::onesided, p);
      knuth_bendix::add_generating_pair(kb, "a", "");
      knuth_bendix::add_generating_pair(kb, "A", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 42);
      REQUIRE(kb.number_of_classes() == 11);
    }
  }

  // This group is actually D_22 (although it wasn't meant to be).
  // All generators are unexpectedly involutory.
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "656",
                          "MAF: both/d6",
                          "[quick][maf][both]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("abB").contains_empty_word(true);

    presentation::add_rule(p, "aa", "");
    presentation::add_rule(p, "bB", "");
    presentation::add_rule(p, "Bb", "");
    presentation::add_rule(p, "bbb", "");
    presentation::add_rule(p, "abab", "");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb;
    kb.init(congruence_kind::twosided, p);
    kb.run();
    REQUIRE(kb.rewriting_system().number_of_rules() == 7);
    REQUIRE(kb.number_of_classes() == 6);
  }

  // free group of rank2 with lots of redundant generators
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "657",
                          "MAF: both/f2_unusual",
                          "[quick][maf][both]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("aAbBcCdDeE").contains_empty_word(true);

    presentation::add_rule(p, "aA", "");
    presentation::add_rule(p, "Aa", "");
    presentation::add_rule(p, "bB", "");
    presentation::add_rule(p, "Bb", "");
    presentation::add_rule(p, "cC", "");
    presentation::add_rule(p, "Cc", "");
    presentation::add_rule(p, "dD", "");
    presentation::add_rule(p, "Dd", "");
    presentation::add_rule(p, "eE", "");
    presentation::add_rule(p, "Ee", "");
    presentation::add_rule(p, "add", "");
    presentation::add_rule(p, "bddd", "");
    presentation::add_rule(p, "cdddddeee", "");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb;
    kb.init(congruence_kind::twosided, p);
    kb.run();
    REQUIRE(kb.rewriting_system().number_of_rules() == 177);
    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
  }

  // group of order 55
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "658",
                          "MAF: both/gp55",
                          "[quick][maf][both]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("aAbB").contains_empty_word(true);

    presentation::add_rule(p, "aA", "");
    presentation::add_rule(p, "Aa", "");
    presentation::add_rule(p, "bB", "");
    presentation::add_rule(p, "Bb", "");
    presentation::add_rule(p, "aaaaa", "");
    presentation::add_rule(p, "bbbbb", "");
    presentation::add_rule(p, "baBBAA", "");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb;
    kb.init(congruence_kind::twosided, p);
    kb.run();
    REQUIRE(kb.rewriting_system().number_of_rules() == 63);
    REQUIRE(kb.number_of_classes() == 55);
  }

  // P. Actually the Free abeliean group of rank 2.
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "659",
                          "MAF: both/P",
                          "[quick][maf][both]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("hHvV").contains_empty_word(true);

    presentation::add_rule(p, "hH", "");
    presentation::add_rule(p, "Hh", "");
    presentation::add_rule(p, "vV", "");
    presentation::add_rule(p, "Vv", "");
    presentation::add_rule(p, "vh", "hv");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb;
    kb.init(congruence_kind::twosided, p);
    kb.run();
    REQUIRE(kb.rewriting_system().number_of_rules() == 8);
    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "660",
                          "MAF: both/p2mm",
                          "[quick][maf][both]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("hHvVrxy").contains_empty_word(true);

    presentation::add_rule(p, "hH", "");
    presentation::add_rule(p, "Hh", "");
    presentation::add_rule(p, "vV", "");
    presentation::add_rule(p, "Vv", "");
    presentation::add_rule(p, "rr", "");
    presentation::add_rule(p, "xx", "");
    presentation::add_rule(p, "yy", "");
    presentation::add_rule(p, "vh", "hv");
    presentation::add_rule(p, "yhy", "H");
    presentation::add_rule(p, "yv", "vy");
    presentation::add_rule(p, "xh", "hx");
    presentation::add_rule(p, "xvx", "V");
    presentation::add_rule(p, "yx", "xy");
    presentation::add_rule(p, "rhr", "H");
    presentation::add_rule(p, "xy", "r");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb;
    kb.init(congruence_kind::twosided, p);
    kb.run();
    REQUIRE(kb.rewriting_system().number_of_rules() == 29);
    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
  }

  // Z wr C2 with - only works with correct gen order.
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "661",
                          "MAF: both/zw2",
                          "[quick][maf][both]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("bBaAt").contains_empty_word(true);

    presentation::add_rule(p, "bB", "");
    presentation::add_rule(p, "Bb", "");
    presentation::add_rule(p, "aA", "");
    presentation::add_rule(p, "Aa", "");
    presentation::add_rule(p, "tt", "");
    presentation::add_rule(p, "ba", "ab");
    presentation::add_rule(p, "tat", "b");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb;
    kb.init(congruence_kind::twosided, p);
    kb.run();
    REQUIRE(kb.rewriting_system().number_of_rules() == 13);
    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "662",
                          "MAF: balanced/G(-3_-3)",
                          "[quick][maf][balanced]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("aAbB").contains_empty_word(true);

    presentation::add_rule(p, "aA", "");
    presentation::add_rule(p, "Aa", "");
    presentation::add_rule(p, "bB", "");
    presentation::add_rule(p, "Bb", "");
    presentation::add_rule(p, "BAbaBab", "AA");
    presentation::add_rule(p, "ABabAba", "bbbb");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb;
    kb.init(congruence_kind::twosided, p);
    kb.run();
    REQUIRE(kb.rewriting_system().number_of_rules() == 219);
    REQUIRE(kb.number_of_classes() == 243);
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "663",
                          "MAF: balanced/Mennicke(-1_-1_-1)",
                          "[quick][maf][balanced]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("aAbBcC").contains_empty_word(true);

    presentation::add_rule(p, "aA", "");
    presentation::add_rule(p, "Aa", "");
    presentation::add_rule(p, "bB", "");
    presentation::add_rule(p, "Bb", "");
    presentation::add_rule(p, "cC", "");
    presentation::add_rule(p, "Cc", "");
    presentation::add_rule(p, "baB", "A");
    presentation::add_rule(p, "cbC", "B");
    presentation::add_rule(p, "acA", "C");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb;
    kb.init(congruence_kind::twosided, p);
    kb.run();
    REQUIRE(kb.rewriting_system().number_of_rules() == 18);
    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "664",
                          "MAF: balanced/Mennicke(1_-1_-1)",
                          "[quick][maf][balanced]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("aAbBcC").contains_empty_word(true);

    presentation::add_rule(p, "aA", "");
    presentation::add_rule(p, "Aa", "");
    presentation::add_rule(p, "bB", "");
    presentation::add_rule(p, "Bb", "");
    presentation::add_rule(p, "cC", "");
    presentation::add_rule(p, "Cc", "");
    presentation::add_rule(p, "baB", "a");
    presentation::add_rule(p, "cbC", "B");
    presentation::add_rule(p, "acA", "C");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb;
    kb.init(congruence_kind::twosided, p);
    kb.run();
    REQUIRE(kb.rewriting_system().number_of_rules() == 18);
    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "665",
                          "MAF: balanced/Mennicke(1_-1_1)",
                          "[quick][maf][balanced]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("aAbBcC").contains_empty_word(true);

    presentation::add_rule(p, "aA", "");
    presentation::add_rule(p, "Aa", "");
    presentation::add_rule(p, "bB", "");
    presentation::add_rule(p, "Bb", "");
    presentation::add_rule(p, "cC", "");
    presentation::add_rule(p, "Cc", "");
    presentation::add_rule(p, "baB", "a");
    presentation::add_rule(p, "cbC", "B");
    presentation::add_rule(p, "acA", "c");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb;
    kb.init(congruence_kind::twosided, p);
    kb.run();
    REQUIRE(kb.rewriting_system().number_of_rules() == 18);
    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "666",
                          "MAF: balanced/Mennicke(1_1_-1)",
                          "[quick][maf][balanced]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("aAbBcC").contains_empty_word(true);

    presentation::add_rule(p, "aA", "");
    presentation::add_rule(p, "Aa", "");
    presentation::add_rule(p, "bB", "");
    presentation::add_rule(p, "Bb", "");
    presentation::add_rule(p, "cC", "");
    presentation::add_rule(p, "Cc", "");
    presentation::add_rule(p, "baB", "a");
    presentation::add_rule(p, "cbC", "b");
    presentation::add_rule(p, "acA", "C");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb;
    kb.init(congruence_kind::twosided, p);
    kb.run();
    REQUIRE(kb.rewriting_system().number_of_rules() == 18);
    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "667",
                          "MAF: balanced/Mennicke(1_1_1)",
                          "[quick][maf][balanced]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("aAbBcC").contains_empty_word(true);

    presentation::add_rule(p, "aA", "");
    presentation::add_rule(p, "Aa", "");
    presentation::add_rule(p, "bB", "");
    presentation::add_rule(p, "Bb", "");
    presentation::add_rule(p, "cC", "");
    presentation::add_rule(p, "Cc", "");
    presentation::add_rule(p, "baB", "a");
    presentation::add_rule(p, "cbC", "b");
    presentation::add_rule(p, "acA", "c");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb;
    kb.init(congruence_kind::twosided, p);
    kb.run();
    REQUIRE(kb.rewriting_system().number_of_rules() == 18);
    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "668",
                          "MAF: balanced/Mennicke(2_2_-2)",
                          "[quick][maf][balanced]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("aAbBcC").contains_empty_word(true);

    presentation::add_rule(p, "aA", "");
    presentation::add_rule(p, "Aa", "");
    presentation::add_rule(p, "bB", "");
    presentation::add_rule(p, "Bb", "");
    presentation::add_rule(p, "cC", "");
    presentation::add_rule(p, "Cc", "");
    presentation::add_rule(p, "baB", "aa");
    presentation::add_rule(p, "cbC", "bb");
    presentation::add_rule(p, "acA", "CC");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb;
    kb.init(congruence_kind::twosided, p);
    kb.run();
    REQUIRE(kb.rewriting_system().number_of_rules() == 32);
    REQUIRE(kb.number_of_classes() == 21);
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "669",
                          "MAF: balanced/Mennicke(2_2_2)",
                          "[quick][maf][balanced]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("aAbBcC").contains_empty_word(true);

    presentation::add_rule(p, "aA", "");
    presentation::add_rule(p, "Aa", "");
    presentation::add_rule(p, "bB", "");
    presentation::add_rule(p, "Bb", "");
    presentation::add_rule(p, "cC", "");
    presentation::add_rule(p, "Cc", "");
    presentation::add_rule(p, "baB", "aa");
    presentation::add_rule(p, "cbC", "bb");
    presentation::add_rule(p, "acA", "cc");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb;
    kb.init(congruence_kind::twosided, p);
    kb.run();
    REQUIRE(kb.rewriting_system().number_of_rules() == 6);
    REQUIRE(kb.number_of_classes() == 1);
  }

  // Generator change group - should be isomorphic to a subgroup of UL(2,Z)
  // l = (a,b,AB)->(a,ba,AAB)
  // r = (a,b,AB)->(ab,b,BAB)
  // s = (a,b,AB)->(a,B,Ab)
  // t = (a,b,AB)->(A,b,aB)
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "670",
                          "MAF: automatic/ul(2_Z)",
                          "[quick][maf][automatic]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("lLrRstj").contains_empty_word(true);

    presentation::add_rule(p, "lL", "");
    presentation::add_rule(p, "Ll", "");
    presentation::add_rule(p, "rR", "");
    presentation::add_rule(p, "Rr", "");
    presentation::add_rule(p, "ss", "");
    presentation::add_rule(p, "tt", "");
    presentation::add_rule(p, "jj", "");
    presentation::add_rule(p, "st", "j");
    presentation::add_rule(p, "slsl", "");
    presentation::add_rule(p, "srsr", "");
    presentation::add_rule(p, "tltl", "");
    presentation::add_rule(p, "trtr", "");
    presentation::add_rule(p, "srslsrslsrsl", "");
    presentation::add_rule(p, "j", "");

    using RewritingSystem = detail::RewritingSystemTrie<RPOCmp>;
    KnuthBendix<std::string, RewritingSystem> kb;
    kb.init(congruence_kind::twosided, p);
    kb.run();
    REQUIRE(kb.rewriting_system().number_of_rules() == 10);
    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
  }

}  // namespace libsemigroups
