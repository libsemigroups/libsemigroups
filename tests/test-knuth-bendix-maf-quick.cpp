
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
  using std::literals::operator""s;

  using LenLexTrie = detail::RewritingSystemTrie<LenLexCmp>;
  using RPOTrie    = detail::RewritingSystemTrie<RPOCmp>;
  using RevRPOTrie = detail::RewritingSystemTrie<RevRPOCmp>;

  // (2,3,7;4) group
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "558",
                          "MAF: subgroups/2374",
                          "[quick][maf]") {
    Presentation<std::string> p;
    p.alphabet("abB"s).contains_empty_word(true);

    presentation::add_rule(p, "aa"s, ""s);
    presentation::add_rule(p, "bB"s, ""s);
    presentation::add_rule(p, "Bb"s, ""s);
    presentation::add_rule(p, "bb"s, "B"s);
    presentation::add_rule(p, "BaBaBaB"s, "abababa"s);
    presentation::add_rule(p, "abaBabaBabaBabaB"s, ""s);

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
    Presentation<std::string> p;
    p.alphabet("aA"s).contains_empty_word(true);

    presentation::add_rule(p, "aA"s, ""s);
    presentation::add_rule(p, "Aa"s, ""s);

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
    Presentation<std::string> p;
    p.alphabet("aAbB"s).contains_empty_word(true);

    presentation::add_rule(p, "aA"s, ""s);
    presentation::add_rule(p, "Aa"s, ""s);
    presentation::add_rule(p, "bB"s, ""s);
    presentation::add_rule(p, "Bb"s, ""s);
    presentation::add_rule(p, "ba"s, "ab"s);

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
    Presentation<std::string> p;
    p.alphabet("abB"s).contains_empty_word(true);

    presentation::add_rule(p, "aa"s, ""s);
    presentation::add_rule(p, "bB"s, ""s);
    presentation::add_rule(p, "Bb"s, ""s);
    presentation::add_rule(p, "bb"s, "B"s);
    presentation::add_rule(p, "aba"s, "B"s);

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
    Presentation<std::string> p;
    p.alphabet("yuUvV"s).contains_empty_word(true);

    presentation::add_rule(p, "yy"s, ""s);
    presentation::add_rule(p, "uU"s, ""s);
    presentation::add_rule(p, "Uu"s, ""s);
    presentation::add_rule(p, "vV"s, ""s);
    presentation::add_rule(p, "Vv"s, ""s);
    presentation::add_rule(p, "yuyu"s, ""s);
    presentation::add_rule(p, "uuu"s, ""s);
    presentation::add_rule(p, "yvyv"s, ""s);
    presentation::add_rule(p, "vvv"s, ""s);
    presentation::add_rule(p, "uvuvuvuvuvuv"s, ""s);
    presentation::add_rule(p, "uVuVuVuVuVuV"s, ""s);
    presentation::add_rule(p, "yuvyuvyuvyuvyuvyuv"s, ""s);
    presentation::add_rule(
        p, "yuvuvuvyuvuvuvyuvuvuvyuvuvuvyuvuvuvyuvuvuv"s, ""s);

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
    Presentation<std::string> p;
    p.alphabet("pqr"s).contains_empty_word(true);

    presentation::add_rule(p, "pp"s, ""s);
    presentation::add_rule(p, "qq"s, ""s);
    presentation::add_rule(p, "rr"s, ""s);

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
    Presentation<std::string> p;
    p.alphabet("wWuU"s).contains_empty_word(true);

    presentation::add_rule(p, "wW"s, ""s);
    presentation::add_rule(p, "Ww"s, ""s);
    presentation::add_rule(p, "uU"s, ""s);
    presentation::add_rule(p, "Uu"s, ""s);

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
    Presentation<std::string> p;
    p.alphabet("sxyrRcCdD"s).contains_empty_word(true);

    presentation::add_rule(p, "ss"s, ""s);
    presentation::add_rule(p, "xx"s, ""s);
    presentation::add_rule(p, "yy"s, ""s);
    presentation::add_rule(p, "rR"s, ""s);
    presentation::add_rule(p, "Rr"s, ""s);
    presentation::add_rule(p, "cC"s, ""s);
    presentation::add_rule(p, "Cc"s, ""s);
    presentation::add_rule(p, "dD"s, ""s);
    presentation::add_rule(p, "Dd"s, ""s);
    presentation::add_rule(p, "rr"s, "R"s);
    presentation::add_rule(p, "sxs"s, "y"s);
    presentation::add_rule(p, "srs"s, "Rc"s);
    presentation::add_rule(p, "scs"s, "d"s);
    presentation::add_rule(p, "RCr"s, "D"s);
    presentation::add_rule(p, "xy"s, "yx"s);
    presentation::add_rule(p, "c"s, ""s);
    presentation::add_rule(p, "d"s, ""s);
    presentation::add_rule(p, "y"s, "x"s);

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
  //   p.alphabet("aAtTuU"s).contains_empty_word(true);

  //   presentation::add_rule(p, "aA"s, ""s);
  //   presentation::add_rule(p, "Aa"s, ""s);
  //   presentation::add_rule(p, "tT"s, ""s);
  //   presentation::add_rule(p, "Tt"s, ""s);
  //   presentation::add_rule(p, "uU"s, ""s);
  //   presentation::add_rule(p, "Uu"s, ""s);
  //   presentation::add_rule(p, "AuAUau"s, "aUAuaU"s);
  //   presentation::add_rule(p, "tat"s, "aTA"s);
  //   presentation::add_rule(p, "tauAUau"s, "auAUauT"s);
  //   presentation::add_rule(p, "ut"s, "tu"s);
  //   presentation::add_rule(p, "aat"s, "taa"s);
  //   presentation::add_rule(p, "aau"s, "uaa"s);

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
    Presentation<std::string> p;
    p.alphabet("abcdefghi"s).contains_empty_word(true);

    presentation::add_rule(p, "aa"s, ""s);
    presentation::add_rule(p, "bb"s, ""s);
    presentation::add_rule(p, "cc"s, ""s);
    presentation::add_rule(p, "dd"s, ""s);
    presentation::add_rule(p, "ee"s, ""s);
    presentation::add_rule(p, "ff"s, ""s);
    presentation::add_rule(p, "gg"s, ""s);
    presentation::add_rule(p, "hh"s, ""s);
    presentation::add_rule(p, "ii"s, ""s);
    presentation::add_rule(p, "bab"s, "aba"s);
    presentation::add_rule(p, "ca"s, "ac"s);
    presentation::add_rule(p, "da"s, "ad"s);
    presentation::add_rule(p, "ea"s, "ae"s);
    presentation::add_rule(p, "fa"s, "af"s);
    presentation::add_rule(p, "ga"s, "ag"s);
    presentation::add_rule(p, "ha"s, "ah"s);
    presentation::add_rule(p, "ia"s, "ai"s);
    presentation::add_rule(p, "cbc"s, "bcb"s);
    presentation::add_rule(p, "db"s, "bd"s);
    presentation::add_rule(p, "eb"s, "be"s);
    presentation::add_rule(p, "fb"s, "bf"s);
    presentation::add_rule(p, "gb"s, "bg"s);
    presentation::add_rule(p, "hb"s, "bh"s);
    presentation::add_rule(p, "ib"s, "bi"s);
    presentation::add_rule(p, "dcd"s, "cdc"s);
    presentation::add_rule(p, "ec"s, "ce"s);
    presentation::add_rule(p, "fc"s, "cf"s);
    presentation::add_rule(p, "gc"s, "cg"s);
    presentation::add_rule(p, "hc"s, "ch"s);
    presentation::add_rule(p, "ic"s, "ci"s);
    presentation::add_rule(p, "ede"s, "ded"s);  // codespell:ignore
    presentation::add_rule(p, "fd"s, "df"s);
    presentation::add_rule(p, "gd"s, "dg"s);
    presentation::add_rule(p, "hd"s, "dh"s);
    presentation::add_rule(p, "id"s, "di"s);
    presentation::add_rule(p, "fef"s, "efe"s);
    presentation::add_rule(p, "ge"s, "eg"s);
    presentation::add_rule(p, "he"s, "eh"s);
    presentation::add_rule(p, "ie"s, "ei"s);
    presentation::add_rule(p, "gfg"s, "fgf"s);
    presentation::add_rule(p, "hf"s, "fh"s);
    presentation::add_rule(p, "if"s, "fi"s);
    presentation::add_rule(p, "hgh"s, "ghg"s);
    presentation::add_rule(p, "ig"s, "gi"s);
    presentation::add_rule(p, "ihi"s, "hih"s);

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
    Presentation<std::string> p;
    p.alphabet("abcdefghijklmno"s).contains_empty_word(true);

    presentation::add_rule(p, "aa"s, ""s);
    presentation::add_rule(p, "bb"s, ""s);
    presentation::add_rule(p, "cc"s, ""s);
    presentation::add_rule(p, "dd"s, ""s);
    presentation::add_rule(p, "ee"s, ""s);
    presentation::add_rule(p, "ff"s, ""s);
    presentation::add_rule(p, "gg"s, ""s);
    presentation::add_rule(p, "hh"s, ""s);
    presentation::add_rule(p, "ii"s, ""s);
    presentation::add_rule(p, "jj"s, ""s);
    presentation::add_rule(p, "kk"s, ""s);
    presentation::add_rule(p, "ll"s, ""s);
    presentation::add_rule(p, "mm"s, ""s);
    presentation::add_rule(p, "nn"s, ""s);
    presentation::add_rule(p, "oo"s, ""s);
    presentation::add_rule(p, "bab"s, "aba"s);
    presentation::add_rule(p, "ca"s, "ac"s);
    presentation::add_rule(p, "da"s, "ad"s);
    presentation::add_rule(p, "ea"s, "ae"s);
    presentation::add_rule(p, "fa"s, "af"s);
    presentation::add_rule(p, "ga"s, "ag"s);
    presentation::add_rule(p, "ha"s, "ah"s);
    presentation::add_rule(p, "ia"s, "ai"s);
    presentation::add_rule(p, "ja"s, "aj"s);
    presentation::add_rule(p, "ka"s, "ak"s);
    presentation::add_rule(p, "la"s, "al"s);
    presentation::add_rule(p, "ma"s, "am"s);
    presentation::add_rule(p, "na"s, "an"s);
    presentation::add_rule(p, "oa"s, "ao"s);
    presentation::add_rule(p, "cbc"s, "bcb"s);
    presentation::add_rule(p, "db"s, "bd"s);
    presentation::add_rule(p, "eb"s, "be"s);
    presentation::add_rule(p, "fb"s, "bf"s);
    presentation::add_rule(p, "gb"s, "bg"s);
    presentation::add_rule(p, "hb"s, "bh"s);
    presentation::add_rule(p, "ib"s, "bi"s);
    presentation::add_rule(p, "jb"s, "bj"s);
    presentation::add_rule(p, "kb"s, "bk"s);
    presentation::add_rule(p, "lb"s, "bl"s);
    presentation::add_rule(p, "mb"s, "bm"s);
    presentation::add_rule(p, "nb"s, "bn"s);
    presentation::add_rule(p, "ob"s, "bo"s);
    presentation::add_rule(p, "dcd"s, "cdc"s);
    presentation::add_rule(p, "ec"s, "ce"s);
    presentation::add_rule(p, "fc"s, "cf"s);
    presentation::add_rule(p, "gc"s, "cg"s);
    presentation::add_rule(p, "hc"s, "ch"s);
    presentation::add_rule(p, "ic"s, "ci"s);
    presentation::add_rule(p, "jc"s, "cj"s);
    presentation::add_rule(p, "kc"s, "ck"s);
    presentation::add_rule(p, "lc"s, "cl"s);
    presentation::add_rule(p, "mc"s, "cm"s);
    presentation::add_rule(p, "nc"s, "cn"s);
    presentation::add_rule(p, "oc"s, "co"s);
    presentation::add_rule(p, "ede"s, "ded"s);  // codespell:ignore
    presentation::add_rule(p, "fd"s, "df"s);
    presentation::add_rule(p, "gd"s, "dg"s);
    presentation::add_rule(p, "hd"s, "dh"s);
    presentation::add_rule(p, "id"s, "di"s);
    presentation::add_rule(p, "jd"s, "dj"s);
    presentation::add_rule(p, "kd"s, "dk"s);
    presentation::add_rule(p, "ld"s, "dl"s);
    presentation::add_rule(p, "md"s, "dm"s);
    presentation::add_rule(p, "nd"s, "dn"s);  // codespell:ignore
    presentation::add_rule(p, "od"s, "do"s);
    presentation::add_rule(p, "fef"s, "efe"s);
    presentation::add_rule(p, "ge"s, "eg"s);
    presentation::add_rule(p, "he"s, "eh"s);
    presentation::add_rule(p, "ie"s, "ei"s);
    presentation::add_rule(p, "je"s, "ej"s);
    presentation::add_rule(p, "ke"s, "ek"s);
    presentation::add_rule(p, "le"s, "el"s);
    presentation::add_rule(p, "me"s, "em"s);
    presentation::add_rule(p, "ne"s, "en"s);
    presentation::add_rule(p, "oe"s, "eo"s);
    presentation::add_rule(p, "gfg"s, "fgf"s);
    presentation::add_rule(p, "hf"s, "fh"s);
    presentation::add_rule(p, "if"s, "fi"s);
    presentation::add_rule(p, "jf"s, "fj"s);
    presentation::add_rule(p, "kf"s, "fk"s);
    presentation::add_rule(p, "lf"s, "fl"s);
    presentation::add_rule(p, "mf"s, "fm"s);
    presentation::add_rule(p, "nf"s, "fn"s);
    presentation::add_rule(p, "of"s, "fo"s);  // codespell:ignore
    presentation::add_rule(p, "hgh"s, "ghg"s);
    presentation::add_rule(p, "ig"s, "gi"s);
    presentation::add_rule(p, "jg"s, "gj"s);
    presentation::add_rule(p, "kg"s, "gk"s);
    presentation::add_rule(p, "lg"s, "gl"s);
    presentation::add_rule(p, "mg"s, "gm"s);
    presentation::add_rule(p, "ng"s, "gn"s);
    presentation::add_rule(p, "og"s, "go"s);
    presentation::add_rule(p, "ihi"s, "hih"s);
    presentation::add_rule(p, "jh"s, "hj"s);
    presentation::add_rule(p, "kh"s, "hk"s);
    presentation::add_rule(p, "lh"s, "hl"s);
    presentation::add_rule(p, "mh"s, "hm"s);
    presentation::add_rule(p, "nh"s, "hn"s);
    presentation::add_rule(p, "oh"s, "ho"s);
    presentation::add_rule(p, "jij"s, "iji"s);
    presentation::add_rule(p, "ki"s, "ik"s);
    presentation::add_rule(p, "li"s, "il"s);
    presentation::add_rule(p, "mi"s, "im"s);
    presentation::add_rule(p, "ni"s, "in"s);
    presentation::add_rule(p, "oi"s, "io"s);
    presentation::add_rule(p, "kjk"s, "jkj"s);
    presentation::add_rule(p, "lj"s, "jl"s);
    presentation::add_rule(p, "mj"s, "jm"s);
    presentation::add_rule(p, "nj"s, "jn"s);
    presentation::add_rule(p, "oj"s, "jo"s);
    presentation::add_rule(p, "lkl"s, "klk"s);
    presentation::add_rule(p, "mk"s, "km"s);
    presentation::add_rule(p, "nk"s, "kn"s);
    presentation::add_rule(p, "ok"s, "ko"s);
    presentation::add_rule(p, "mlm"s, "lml"s);
    presentation::add_rule(p, "nl"s, "ln"s);
    presentation::add_rule(p, "ol"s, "lo"s);
    presentation::add_rule(p, "nmn"s, "mnm"s);
    presentation::add_rule(p, "om"s, "mo"s);
    presentation::add_rule(p, "ono"s, "non"s);

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
    Presentation<std::string> p;
    p.alphabet("abB"s).contains_empty_word(true);

    presentation::add_rule(p, "aa"s, ""s);
    presentation::add_rule(p, "bB"s, ""s);
    presentation::add_rule(p, "Bb"s, ""s);
    presentation::add_rule(p, "bb"s, "B"s);
    presentation::add_rule(p, "BaBa"s, "abab"s);

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
    Presentation<std::string> p;
    p.alphabet("abcde"s).contains_empty_word(true);

    presentation::add_rule(p, "aa"s, ""s);
    presentation::add_rule(p, "bb"s, ""s);
    presentation::add_rule(p, "cc"s, ""s);
    presentation::add_rule(p, "dd"s, ""s);
    presentation::add_rule(p, "ee"s, ""s);
    presentation::add_rule(p, "bab"s, "aba"s);
    presentation::add_rule(p, "ca"s, "ac"s);
    presentation::add_rule(p, "da"s, "ad"s);
    presentation::add_rule(p, "ea"s, "ae"s);
    presentation::add_rule(p, "cbc"s, "bcb"s);
    presentation::add_rule(p, "db"s, "bd"s);
    presentation::add_rule(p, "eb"s, "be"s);
    presentation::add_rule(p, "dcd"s, "cdc"s);
    presentation::add_rule(p, "ec"s, "ce"s);
    presentation::add_rule(p, "ede"s, "ded"s);  // codespell:ignore

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
    Presentation<std::string> p;
    p.alphabet("abcdef"s).contains_empty_word(true);

    presentation::add_rule(p, "aa"s, ""s);
    presentation::add_rule(p, "bb"s, ""s);
    presentation::add_rule(p, "cc"s, ""s);
    presentation::add_rule(p, "dd"s, ""s);
    presentation::add_rule(p, "ee"s, ""s);
    presentation::add_rule(p, "ff"s, ""s);
    presentation::add_rule(p, "bab"s, "aba"s);
    presentation::add_rule(p, "ca"s, "ac"s);
    presentation::add_rule(p, "da"s, "ad"s);
    presentation::add_rule(p, "ea"s, "ae"s);
    presentation::add_rule(p, "fa"s, "af"s);
    presentation::add_rule(p, "cbc"s, "bcb"s);
    presentation::add_rule(p, "db"s, "bd"s);
    presentation::add_rule(p, "eb"s, "be"s);
    presentation::add_rule(p, "fb"s, "bf"s);
    presentation::add_rule(p, "dcd"s, "cdc"s);
    presentation::add_rule(p, "ec"s, "ce"s);
    presentation::add_rule(p, "fc"s, "cf"s);
    presentation::add_rule(p, "ede"s, "ded"s);  // codespell:ignore
    presentation::add_rule(p, "fd"s, "df"s);
    presentation::add_rule(p, "fef"s, "efe"s);

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
    Presentation<std::string> p;
    p.alphabet("abcdefg"s).contains_empty_word(true);

    presentation::add_rule(p, "aa"s, ""s);
    presentation::add_rule(p, "bb"s, ""s);
    presentation::add_rule(p, "cc"s, ""s);
    presentation::add_rule(p, "dd"s, ""s);
    presentation::add_rule(p, "ee"s, ""s);
    presentation::add_rule(p, "ff"s, ""s);
    presentation::add_rule(p, "gg"s, ""s);
    presentation::add_rule(p, "bab"s, "aba"s);
    presentation::add_rule(p, "ca"s, "ac"s);
    presentation::add_rule(p, "da"s, "ad"s);
    presentation::add_rule(p, "ea"s, "ae"s);
    presentation::add_rule(p, "fa"s, "af"s);
    presentation::add_rule(p, "ga"s, "ag"s);
    presentation::add_rule(p, "cbc"s, "bcb"s);
    presentation::add_rule(p, "db"s, "bd"s);
    presentation::add_rule(p, "eb"s, "be"s);
    presentation::add_rule(p, "fb"s, "bf"s);
    presentation::add_rule(p, "gb"s, "bg"s);
    presentation::add_rule(p, "dcd"s, "cdc"s);
    presentation::add_rule(p, "ec"s, "ce"s);
    presentation::add_rule(p, "fc"s, "cf"s);
    presentation::add_rule(p, "gc"s, "cg"s);
    presentation::add_rule(p, "ede"s, "ded"s);  // codespell:ignore
    presentation::add_rule(p, "fd"s, "df"s);
    presentation::add_rule(p, "gd"s, "dg"s);
    presentation::add_rule(p, "fef"s, "efe"s);
    presentation::add_rule(p, "ge"s, "eg"s);
    presentation::add_rule(p, "gfg"s, "fgf"s);

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
    Presentation<std::string> p;
    p.alphabet("abcdefgh"s).contains_empty_word(true);

    presentation::add_rule(p, "aa"s, ""s);
    presentation::add_rule(p, "bb"s, ""s);
    presentation::add_rule(p, "cc"s, ""s);
    presentation::add_rule(p, "dd"s, ""s);
    presentation::add_rule(p, "ee"s, ""s);
    presentation::add_rule(p, "ff"s, ""s);
    presentation::add_rule(p, "gg"s, ""s);
    presentation::add_rule(p, "hh"s, ""s);
    presentation::add_rule(p, "bab"s, "aba"s);
    presentation::add_rule(p, "ca"s, "ac"s);
    presentation::add_rule(p, "da"s, "ad"s);
    presentation::add_rule(p, "ea"s, "ae"s);
    presentation::add_rule(p, "fa"s, "af"s);
    presentation::add_rule(p, "ga"s, "ag"s);
    presentation::add_rule(p, "ha"s, "ah"s);
    presentation::add_rule(p, "cbc"s, "bcb"s);
    presentation::add_rule(p, "db"s, "bd"s);
    presentation::add_rule(p, "eb"s, "be"s);
    presentation::add_rule(p, "fb"s, "bf"s);
    presentation::add_rule(p, "gb"s, "bg"s);
    presentation::add_rule(p, "hb"s, "bh"s);
    presentation::add_rule(p, "dcd"s, "cdc"s);
    presentation::add_rule(p, "ec"s, "ce"s);
    presentation::add_rule(p, "fc"s, "cf"s);
    presentation::add_rule(p, "gc"s, "cg"s);
    presentation::add_rule(p, "hc"s, "ch"s);
    presentation::add_rule(p, "ede"s, "ded"s);  // codespell:ignore
    presentation::add_rule(p, "fd"s, "df"s);
    presentation::add_rule(p, "gd"s, "dg"s);
    presentation::add_rule(p, "hd"s, "dh"s);
    presentation::add_rule(p, "fef"s, "efe"s);
    presentation::add_rule(p, "ge"s, "eg"s);
    presentation::add_rule(p, "he"s, "eh"s);
    presentation::add_rule(p, "gfg"s, "fgf"s);
    presentation::add_rule(p, "hf"s, "fh"s);
    presentation::add_rule(p, "hgh"s, "ghg"s);

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
    Presentation<std::string> p;
    p.alphabet(""s).contains_empty_word(true);

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
    Presentation<std::string> p;
    p.alphabet("aA"s).contains_empty_word(true);

    presentation::add_rule(p, "aA"s, ""s);
    presentation::add_rule(p, "Aa"s, ""s);
    presentation::add_rule(p, "a"s, ""s);

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
    Presentation<std::string> p;
    p.alphabet("aAbB"s).contains_empty_word(true);

    presentation::add_rule(p, "aA"s, ""s);
    presentation::add_rule(p, "Aa"s, ""s);
    presentation::add_rule(p, "bB"s, ""s);
    presentation::add_rule(p, "Bb"s, ""s);
    presentation::add_rule(p, "ab"s, ""s);
    presentation::add_rule(p, "abb"s, ""s);

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
    Presentation<std::string> p;
    p.alphabet("aAbBcC"s).contains_empty_word(true);

    presentation::add_rule(p, "aA"s, ""s);
    presentation::add_rule(p, "Aa"s, ""s);
    presentation::add_rule(p, "bB"s, ""s);
    presentation::add_rule(p, "Bb"s, ""s);
    presentation::add_rule(p, "cC"s, ""s);
    presentation::add_rule(p, "Cc"s, ""s);
    presentation::add_rule(p, "Aba"s, "bb"s);
    presentation::add_rule(p, "Bcb"s, "cc"s);
    presentation::add_rule(p, "Cac"s, "aa"s);

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
    Presentation<std::string> p;
    p.alphabet("rRsStT"s).contains_empty_word(true);

    presentation::add_rule(p, "rR"s, ""s);
    presentation::add_rule(p, "Rr"s, ""s);
    presentation::add_rule(p, "sS"s, ""s);
    presentation::add_rule(p, "Ss"s, ""s);
    presentation::add_rule(p, "tT"s, ""s);
    presentation::add_rule(p, "Tt"s, ""s);
    presentation::add_rule(p, "Trt"s, "rr"s);
    presentation::add_rule(p, "Rsr"s, "ss"s);
    presentation::add_rule(p, "Sts"s, "tt"s);

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
    Presentation<std::string> p;
    p.alphabet("aAdDgGbBeEhHcCfFiI"s).contains_empty_word(true);

    presentation::add_rule(p, "aA"s, ""s);
    presentation::add_rule(p, "Aa"s, ""s);
    presentation::add_rule(p, "dD"s, ""s);
    presentation::add_rule(p, "Dd"s, ""s);
    presentation::add_rule(p, "gG"s, ""s);
    presentation::add_rule(p, "Gg"s, ""s);
    presentation::add_rule(p, "bB"s, ""s);
    presentation::add_rule(p, "BB"s, ""s);
    presentation::add_rule(p, "eE"s, ""s);
    presentation::add_rule(p, "Ee"s, ""s);
    presentation::add_rule(p, "hH"s, ""s);
    presentation::add_rule(p, "Hh"s, ""s);
    presentation::add_rule(p, "cC"s, ""s);
    presentation::add_rule(p, "Cc"s, ""s);
    presentation::add_rule(p, "fF"s, ""s);
    presentation::add_rule(p, "Ff"s, ""s);
    presentation::add_rule(p, "iI"s, ""s);
    presentation::add_rule(p, "II"s, ""s);
    presentation::add_rule(p, "ab"s, "d"s);
    presentation::add_rule(p, "bc"s, "e"s);
    presentation::add_rule(p, "cd"s, "f"s);
    presentation::add_rule(p, "de"s, "g"s);
    presentation::add_rule(p, "ef"s, "h"s);
    presentation::add_rule(p, "fg"s, "i"s);
    presentation::add_rule(p, "gh"s, "a"s);
    presentation::add_rule(p, "hi"s, "b"s);
    presentation::add_rule(p, "ia"s, "c"s);

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
    Presentation<std::string> p;
    p.alphabet("gGfFm"s).contains_empty_word(true);

    presentation::add_rule(p, "gG"s, ""s);
    presentation::add_rule(p, "Gg"s, ""s);
    presentation::add_rule(p, "fF"s, ""s);
    presentation::add_rule(p, "Ff"s, ""s);
    presentation::add_rule(p, "mm"s, ""s);
    presentation::add_rule(p, "mG"s, "fm"s);
    presentation::add_rule(p, "Fg"s, "Gf"s);
    presentation::add_rule(p, "FG"s, "gf"s);

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
    Presentation<std::string> p;
    p.alphabet("abc"s).contains_empty_word(true);

    presentation::add_rule(p, "aa"s, ""s);
    presentation::add_rule(p, "bb"s, ""s);
    presentation::add_rule(p, "cc"s, ""s);
    presentation::add_rule(p, "caca"s, ""s);
    presentation::add_rule(p, "babababa"s, ""s);
    presentation::add_rule(p, "cbcbcbcb"s, ""s);

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
  //   p.alphabet("abc"s).contains_empty_word(true);

  //   presentation::add_rule(p, "aa"s, ""s);
  //   presentation::add_rule(p, "bb"s, ""s);
  //   presentation::add_rule(p, "cc"s, ""s);
  //   presentation::add_rule(p, "caca"s, ""s);
  //   presentation::add_rule(p, "babababa"s, ""s);
  //   presentation::add_rule(p, "cbcbcbcb"s, ""s);

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
    Presentation<std::string> p;
    p.alphabet("abc"s).contains_empty_word(true);

    presentation::add_rule(p, "aa"s, ""s);
    presentation::add_rule(p, "bb"s, ""s);
    presentation::add_rule(p, "cc"s, ""s);
    presentation::add_rule(p, "bcbcbcbcbcbc"s, ""s);
    presentation::add_rule(p, "caca"s, ""s);
    presentation::add_rule(p, "ababab"s, ""s);

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
    Presentation<std::string> p;
    p.alphabet("jihgfedcba"s).contains_empty_word(true);

    presentation::add_rule(p, "jj"s, ""s);
    presentation::add_rule(p, "ii"s, ""s);
    presentation::add_rule(p, "hh"s, ""s);
    presentation::add_rule(p, "gg"s, ""s);
    presentation::add_rule(p, "ff"s, ""s);
    presentation::add_rule(p, "ee"s, ""s);
    presentation::add_rule(p, "dd"s, ""s);
    presentation::add_rule(p, "cc"s, ""s);
    presentation::add_rule(p, "bb"s, ""s);
    presentation::add_rule(p, "aa"s, ""s);
    presentation::add_rule(p, "abababab"s, ""s);
    presentation::add_rule(p, "acacacac"s, ""s);
    presentation::add_rule(p, "adadad"s, ""s);
    presentation::add_rule(p, "bcbcbc"s, ""s);
    presentation::add_rule(p, "bdbdbdbd"s, ""s);
    presentation::add_rule(p, "cdcdcdcd"s, ""s);
    presentation::add_rule(p, "abab"s, "e"s);
    presentation::add_rule(p, "acac"s, "f"s);
    presentation::add_rule(p, "ada"s, "g"s);
    presentation::add_rule(p, "bcb"s, "h"s);
    presentation::add_rule(p, "bdbd"s, "i"s);
    presentation::add_rule(p, "cdcd"s, "j"s);

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
    Presentation<std::string> p;
    p.alphabet("aAbBcdef"s).contains_empty_word(true);

    presentation::add_rule(p, "aA"s, ""s);
    presentation::add_rule(p, "Aa"s, ""s);
    presentation::add_rule(p, "bB"s, ""s);
    presentation::add_rule(p, "Bb"s, ""s);
    presentation::add_rule(p, "cc"s, ""s);
    presentation::add_rule(p, "dd"s, ""s);
    presentation::add_rule(p, "ee"s, ""s);
    presentation::add_rule(p, "ff"s, ""s);
    presentation::add_rule(p, "aaa"s, ""s);
    presentation::add_rule(p, "bbb"s, ""s);
    presentation::add_rule(p, "acacac"s, ""s);
    presentation::add_rule(p, "adadad"s, ""s);
    presentation::add_rule(p, "aeaeae"s, ""s);
    presentation::add_rule(p, "afafaf"s, ""s);
    presentation::add_rule(p, "bcbcbc"s, ""s);
    presentation::add_rule(p, "bdbdbd"s, ""s);
    presentation::add_rule(p, "bebebe"s, ""s);
    presentation::add_rule(p, "bfbfbf"s, ""s);
    presentation::add_rule(p, "abAcabAc"s, ""s);
    presentation::add_rule(p, "abAdabAd"s, ""s);
    presentation::add_rule(p, "AbaeAbae"s, ""s);
    presentation::add_rule(p, "AbafAbaf"s, ""s);
    presentation::add_rule(p, "baBcbaBc"s, ""s);
    presentation::add_rule(p, "BabdBabd"s, ""s);
    presentation::add_rule(p, "baBebaBe"s, ""s);
    presentation::add_rule(p, "BabfBabf"s, ""s);

    using RewritingSystem = detail::RewritingSystemTrie<WrCmp>;
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

    WrCmp cmp{p.alphabet_v4(), levels};
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
    Presentation<std::string> p;
    p.alphabet("abcdef"s).contains_empty_word(true);

    presentation::add_rule(p, "ab"s, ""s);
    presentation::add_rule(p, "ba"s, ""s);
    presentation::add_rule(p, "cd"s, ""s);
    presentation::add_rule(p, "dc"s, ""s);
    presentation::add_rule(p, "ef"s, ""s);
    presentation::add_rule(p, "fe"s, ""s);
    presentation::add_rule(p, "cafa"s, "bebd"s);
    presentation::add_rule(p, "cafb"s, "aebd"s);
    presentation::add_rule(p, "ceca"s, "bdfd"s);
    presentation::add_rule(p, "cecb"s, "adfd"s);
    presentation::add_rule(p, "ceda"s, "bcfd"s);
    presentation::add_rule(p, "cfca"s, "bded"s);
    presentation::add_rule(p, "eaca"s, "bdbf"s);
    presentation::add_rule(p, "eacb"s, "adbf"s);
    presentation::add_rule(p, "eada"s, "bcbf"s);
    presentation::add_rule(p, "eaec"s, "dfbf"s);
    presentation::add_rule(p, "eaed"s, "cfbf"s);
    presentation::add_rule(p, "eafc"s, "debf"s);
    presentation::add_rule(p, "cacaca"s, "bdbdbd"s);
    presentation::add_rule(p, "ceacea"s, "aecaec"s);
    presentation::add_rule(p, "ceadea"s, "aedaec"s);

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
    Presentation<std::string> p;
    p.alphabet("abcdef"s).contains_empty_word(true);

    presentation::add_rule(p, "ab"s, ""s);
    presentation::add_rule(p, "ba"s, ""s);
    presentation::add_rule(p, "cd"s, ""s);
    presentation::add_rule(p, "dc"s, ""s);
    presentation::add_rule(p, "ef"s, ""s);
    presentation::add_rule(p, "fe"s, ""s);
    presentation::add_rule(p, "aaa"s, "b"s);
    presentation::add_rule(p, "ccc"s, "d"s);
    presentation::add_rule(p, "eee"s, "f"s);
    presentation::add_rule(p, "aacca"s, "ccb"s);
    presentation::add_rule(p, "aaeea"s, "eeb"s);
    presentation::add_rule(p, "caea"s, "bfbd"s);
    presentation::add_rule(p, "ceca"s, "bdfd"s);
    presentation::add_rule(p, "deca"s, "bdfc"s);
    presentation::add_rule(p, "cfda"s, "bced"s);
    presentation::add_rule(p, "ebca"s, "bdaf"s);
    presentation::add_rule(p, "edea"s, "bfcf"s);
    presentation::add_rule(p, "fcea"s, "bfde"s);
    presentation::add_rule(p, "ecfb"s, "aedf"s);
    presentation::add_rule(p, "aaecaec"s, "fdbfd"s);
    presentation::add_rule(p, "cacaca"s, "bdbdbd"s);

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
    Presentation<std::string> p;
    p.alphabet("uUvV"s).contains_empty_word(true);

    presentation::add_rule(p, "uU"s, ""s);
    presentation::add_rule(p, "Uu"s, ""s);
    presentation::add_rule(p, "vV"s, ""s);
    presentation::add_rule(p, "Vv"s, ""s);
    presentation::add_rule(p, "vUvvUv"s, ""s);
    presentation::add_rule(p, "UvUvUv"s, ""s);
    presentation::add_rule(p, "uuuuuvuuvvuvvvvvuv"s, ""s);

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
    Presentation<std::string> p;
    p.alphabet("uUvV"s).contains_empty_word(true);

    presentation::add_rule(p, "uU"s, ""s);
    presentation::add_rule(p, "Uu"s, ""s);
    presentation::add_rule(p, "vV"s, ""s);
    presentation::add_rule(p, "Vv"s, ""s);
    presentation::add_rule(p, "vUvvUv"s, ""s);
    presentation::add_rule(p, "UvUvUv"s, ""s);
    presentation::add_rule(p, "uuuvuuvvuvvvuuvuvv"s, ""s);

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
    Presentation<std::string> p;
    p.alphabet("yYx"s).contains_empty_word(true);

    presentation::add_rule(p, "yY"s, ""s);
    presentation::add_rule(p, "Yy"s, ""s);
    presentation::add_rule(p, "xx"s, ""s);
    presentation::add_rule(p, "yy"s, "Y"s);
    presentation::add_rule(p, "xyxyxYxyxyxYxYxYxy"s, "YxyxyxyxYxyxyxYxYx"s);

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
    Presentation<std::string> p;
    p.alphabet("abc"s).contains_empty_word(true);

    presentation::add_rule(p, "aa"s, ""s);
    presentation::add_rule(p, "bb"s, ""s);
    presentation::add_rule(p, "cc"s, ""s);
    presentation::add_rule(p, "caca"s, ""s);
    presentation::add_rule(p, "babababa"s, ""s);
    presentation::add_rule(p, "cbcbcbcb"s, ""s);

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
    Presentation<std::string> p;
    p.alphabet("lrfLR"s).contains_empty_word(true);

    presentation::add_rule(p, "lL"s, ""s);
    presentation::add_rule(p, "rR"s, ""s);
    presentation::add_rule(p, "ff"s, ""s);
    presentation::add_rule(p, "Ll"s, ""s);
    presentation::add_rule(p, "Rr"s, ""s);
    presentation::add_rule(p, "lfl"s, "f"s);
    presentation::add_rule(p, "rfr"s, "f"s);
    presentation::add_rule(p, "frflfrf"s, "rflfr"s);
    presentation::add_rule(p, "lfrfl"s, "rflfr"s);

    using RewritingSystem = detail::RewritingSystemTrie<WrCmp>;
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
    Presentation<std::string> p;
    p.alphabet("aAgGdDbBhHeEcCiIfF"s).contains_empty_word(true);

    presentation::add_rule(p, "aA"s, ""s);
    presentation::add_rule(p, "Aa"s, ""s);
    presentation::add_rule(p, "gG"s, ""s);
    presentation::add_rule(p, "Gg"s, ""s);
    presentation::add_rule(p, "dD"s, ""s);
    presentation::add_rule(p, "Dd"s, ""s);
    presentation::add_rule(p, "bB"s, ""s);
    presentation::add_rule(p, "Bb"s, ""s);
    presentation::add_rule(p, "hH"s, ""s);
    presentation::add_rule(p, "Hh"s, ""s);
    presentation::add_rule(p, "eE"s, ""s);
    presentation::add_rule(p, "Ee"s, ""s);
    presentation::add_rule(p, "cC"s, ""s);
    presentation::add_rule(p, "Cc"s, ""s);
    presentation::add_rule(p, "iI"s, ""s);
    presentation::add_rule(p, "Ii"s, ""s);
    presentation::add_rule(p, "fF"s, ""s);
    presentation::add_rule(p, "Ff"s, ""s);
    presentation::add_rule(p, "ab"s, "e"s);
    presentation::add_rule(p, "bc"s, "f"s);
    presentation::add_rule(p, "cd"s, "g"s);
    presentation::add_rule(p, "de"s, "h"s);
    presentation::add_rule(p, "ef"s, "i"s);
    presentation::add_rule(p, "fg"s, "a"s);
    presentation::add_rule(p, "gh"s, "b"s);
    presentation::add_rule(p, "hi"s, "c"s);
    presentation::add_rule(p, "ia"s, "d"s);

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
    Presentation<std::string> p;
    p.alphabet("abB"s).contains_empty_word(true);

    presentation::add_rule(p, "bb"s, "B"s);
    presentation::add_rule(p, "BaB"s, "aba"s);

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
    Presentation<std::string> p;
    p.alphabet("cChHgGeEdDiIfF"s).contains_empty_word(true);

    presentation::add_rule(p, "cC"s, ""s);
    presentation::add_rule(p, "Cc"s, ""s);
    presentation::add_rule(p, "hH"s, ""s);
    presentation::add_rule(p, "Hh"s, ""s);
    presentation::add_rule(p, "gG"s, ""s);
    presentation::add_rule(p, "Gg"s, ""s);
    presentation::add_rule(p, "eE"s, ""s);
    presentation::add_rule(p, "Ee"s, ""s);
    presentation::add_rule(p, "dD"s, ""s);
    presentation::add_rule(p, "Dd"s, ""s);
    presentation::add_rule(p, "iI"s, ""s);
    presentation::add_rule(p, "Ii"s, ""s);
    presentation::add_rule(p, "fF"s, ""s);
    presentation::add_rule(p, "Ff"s, ""s);
    presentation::add_rule(p, "dD"s, ""s);
    presentation::add_rule(p, "Dd"s, ""s);
    presentation::add_rule(p, "eE"s, ""s);
    presentation::add_rule(p, "Ee"s, ""s);
    presentation::add_rule(p, "fF"s, ""s);
    presentation::add_rule(p, "Ff"s, ""s);
    presentation::add_rule(p, "gG"s, ""s);
    presentation::add_rule(p, "Gg"s, ""s);
    presentation::add_rule(p, "hH"s, ""s);
    presentation::add_rule(p, "Hh"s, ""s);
    presentation::add_rule(p, "iI"s, ""s);
    presentation::add_rule(p, "Ii"s, ""s);
    presentation::add_rule(p, "dC"s, "Cd"s);
    presentation::add_rule(p, "fD"s, "Df"s);
    presentation::add_rule(p, "gD"s, "Dg"s);
    presentation::add_rule(p, "fE"s, "Ef"s);
    presentation::add_rule(p, "iE"s, "Ei"s);
    presentation::add_rule(p, "gF"s, "Fg"s);
    presentation::add_rule(p, "iF"s, "Fi"s);
    presentation::add_rule(p, "iG"s, "Gi"s);
    presentation::add_rule(p, "iH"s, "Hi"s);
    presentation::add_rule(p, "cchC"s, "hc"s);
    presentation::add_rule(p, "Ege"s, "Cgc"s);  // codespell:ignore
    presentation::add_rule(p, "gIC"s, "CgI"s);
    presentation::add_rule(p, "GhC"s, "CGh"s);
    presentation::add_rule(p, "hCD"s, "CDh"s);
    presentation::add_rule(p, "Hgh"s, "Cgc"s);
    presentation::add_rule(p, "eDg"s, "Dge"s);
    presentation::add_rule(p, "GhD"s, "DhG"s);
    presentation::add_rule(p, "HiD"s, "DHi"s);
    presentation::add_rule(p, "GhF"s, "FhG"s);
    presentation::add_rule(p, "dGCi"s, "CGid"s);
    presentation::add_rule(p, "DeFC"s, "CeDF"s);
    presentation::add_rule(p, "fhIC"s, "CfhI"s);
    presentation::add_rule(p, "HFiC"s, "CHFi"s);
    presentation::add_rule(p, "iCDg"s, "CDgi"s);
    presentation::add_rule(p, "he"s, "eh"s);
    presentation::add_rule(p, "hfHI"s, "DfId"s);
    presentation::add_rule(p, "FHgE"s, "EHFg"s);
    presentation::add_rule(p, "EFGiD"s, "DEFGi"s);

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
    Presentation<std::string> p;
    p.alphabet("aAb"s).contains_empty_word(true);

    presentation::add_rule(p, "aA"s, ""s);
    presentation::add_rule(p, "Aa"s, ""s);
    presentation::add_rule(p, "bb"s, ""s);
    presentation::add_rule(p, "ababab"s, ""s);
    presentation::add_rule(p, "aaaaaaa"s, ""s);
    presentation::add_rule(p, "abAbabAbabAbabAbabAbabAbabAb"s, ""s);

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
    Presentation<std::string> p;
    p.alphabet("cCdDeEfFgGhHiI"s).contains_empty_word(true);

    presentation::add_rule(p, "cC"s, ""s);
    presentation::add_rule(p, "Cc"s, ""s);
    presentation::add_rule(p, "dD"s, ""s);
    presentation::add_rule(p, "Dd"s, ""s);
    presentation::add_rule(p, "eE"s, ""s);
    presentation::add_rule(p, "Ee"s, ""s);
    presentation::add_rule(p, "fF"s, ""s);
    presentation::add_rule(p, "Ff"s, ""s);
    presentation::add_rule(p, "gG"s, ""s);
    presentation::add_rule(p, "Gg"s, ""s);
    presentation::add_rule(p, "hH"s, ""s);
    presentation::add_rule(p, "Hh"s, ""s);
    presentation::add_rule(p, "iI"s, ""s);
    presentation::add_rule(p, "Ii"s, ""s);
    presentation::add_rule(p, "cC"s, ""s);
    presentation::add_rule(p, "cd"s, "dc"s);
    presentation::add_rule(p, "cD"s, "Dc"s);
    presentation::add_rule(p, "cf"s, "fc"s);
    presentation::add_rule(p, "cF"s, "Fc"s);
    presentation::add_rule(p, "cg"s, "gEce"s);
    presentation::add_rule(p, "cG"s, "GEce"s);
    presentation::add_rule(p, "ch"s, "hEce"s);
    presentation::add_rule(p, "cH"s, "HEce"s);
    presentation::add_rule(p, "ci"s, "iEce"s);
    presentation::add_rule(p, "cI"s, "IEce"s);
    presentation::add_rule(p, "Cc"s, ""s);
    presentation::add_rule(p, "Cd"s, "dC"s);
    presentation::add_rule(p, "CD"s, "DC"s);
    presentation::add_rule(p, "Ce"s, "ceCC"s);
    presentation::add_rule(p, "CE"s, "cECC"s);
    presentation::add_rule(p, "Cf"s, "fC"s);
    presentation::add_rule(p, "CF"s, "FC"s);
    presentation::add_rule(p, "Cg"s, "gEceCC"s);
    presentation::add_rule(p, "CG"s, "GEceCC"s);
    presentation::add_rule(p, "Ch"s, "hEceCC"s);
    presentation::add_rule(p, "CH"s, "HEceCC"s);
    presentation::add_rule(p, "Ci"s, "iEceCC"s);
    presentation::add_rule(p, "CI"s, "IEceCC"s);
    presentation::add_rule(p, "dD"s, ""s);
    presentation::add_rule(p, "de"s, "cedC"s);
    presentation::add_rule(p, "dE"s, "cEdC"s);
    presentation::add_rule(p, "df"s, "fd"s);
    presentation::add_rule(p, "dF"s, "Fd"s);
    presentation::add_rule(p, "dg"s, "gd"s);
    presentation::add_rule(p, "dG"s, "Gd"s);
    presentation::add_rule(p, "dh"s, "hEcedC"s);
    presentation::add_rule(p, "dH"s, "HEcedC"s);
    presentation::add_rule(p, "di"s, "iEcedC"s);
    presentation::add_rule(p, "dI"s, "IEcedC"s);
    presentation::add_rule(p, "Dd"s, ""s);
    presentation::add_rule(p, "De"s, "ceDC"s);
    presentation::add_rule(p, "DE"s, "cEDC"s);
    presentation::add_rule(p, "Df"s, "fD"s);
    presentation::add_rule(p, "DF"s, "FD"s);
    presentation::add_rule(p, "Dg"s, "gD"s);
    presentation::add_rule(p, "DG"s, "GD"s);
    presentation::add_rule(p, "Dh"s, "hEceDC"s);
    presentation::add_rule(p, "DH"s, "HEceDC"s);
    presentation::add_rule(p, "Di"s, "iEceDC"s);
    presentation::add_rule(p, "DI"s, "IEceDC"s);
    presentation::add_rule(p, "eE"s, ""s);
    presentation::add_rule(p, "ef"s, "fe"s);
    presentation::add_rule(p, "eF"s, "Fe"s);
    presentation::add_rule(p, "eg"s, "gceC"s);
    presentation::add_rule(p, "eG"s, "GceC"s);
    presentation::add_rule(p, "eh"s, "he"s);
    presentation::add_rule(p, "eH"s, "He"s);
    presentation::add_rule(p, "ei"s, "ie"s);
    presentation::add_rule(p, "eI"s, "Ie"s);
    presentation::add_rule(p, "Ee"s, ""s);
    presentation::add_rule(p, "Ef"s, "fE"s);
    presentation::add_rule(p, "EF"s, "FE"s);
    presentation::add_rule(p, "Eg"s, "gcEC"s);
    presentation::add_rule(p, "EG"s, "GcEC"s);
    presentation::add_rule(p, "Eh"s, "hE"s);
    presentation::add_rule(p, "EH"s, "HE"s);
    presentation::add_rule(p, "Ei"s, "iE"s);
    presentation::add_rule(p, "EI"s, "IE"s);
    presentation::add_rule(p, "fF"s, ""s);
    presentation::add_rule(p, "fg"s, "gf"s);
    presentation::add_rule(p, "fG"s, "Gf"s);
    presentation::add_rule(p, "fh"s, "hfEceC"s);
    presentation::add_rule(p, "fH"s, "HfEceC"s);
    presentation::add_rule(p, "fi"s, "if"s);
    presentation::add_rule(p, "fI"s, "If"s);
    presentation::add_rule(p, "Ff"s, ""s);
    presentation::add_rule(p, "Fg"s, "gF"s);
    presentation::add_rule(p, "FG"s, "GF"s);
    presentation::add_rule(p, "Fh"s, "hFEceC"s);
    presentation::add_rule(p, "FH"s, "HFEceC"s);
    presentation::add_rule(p, "Fi"s, "iF"s);
    presentation::add_rule(p, "FI"s, "IF"s);
    presentation::add_rule(p, "gG"s, ""s);
    presentation::add_rule(p, "gh"s, "hgEceC"s);
    presentation::add_rule(p, "gH"s, "HgEceC"s);
    presentation::add_rule(p, "gi"s, "ig"s);
    presentation::add_rule(p, "gI"s, "Ig"s);
    presentation::add_rule(p, "Gg"s, ""s);
    presentation::add_rule(p, "Gh"s, "hGEceC"s);
    presentation::add_rule(p, "GH"s, "HGEceC"s);
    presentation::add_rule(p, "Gi"s, "iG"s);
    presentation::add_rule(p, "GI"s, "IG"s);
    presentation::add_rule(p, "hH"s, ""s);
    presentation::add_rule(p, "hi"s, "ih"s);
    presentation::add_rule(p, "hI"s, "Ih"s);
    presentation::add_rule(p, "Hh"s, ""s);
    presentation::add_rule(p, "Hi"s, "iH"s);
    presentation::add_rule(p, "HI"s, "IH"s);
    presentation::add_rule(p, "iI"s, ""s);
    presentation::add_rule(p, "Ii"s, ""s);
    presentation::add_rule(p, "cce"s, "ecc"s);
    presentation::add_rule(p, "ccE"s, "Ecc"s);
    presentation::add_rule(p, "cee"s, "eec"s);
    presentation::add_rule(p, "cEE"s, "EEc"s);
    presentation::add_rule(p, "dce"s, "edc"s);
    presentation::add_rule(p, "dcE"s, "Edc"s);
    presentation::add_rule(p, "Dce"s, "eDc"s);
    presentation::add_rule(p, "DcE"s, "EDc"s);
    presentation::add_rule(p, "ecE"s, "Ece"s);
    presentation::add_rule(p, "cece"s, "ecec"s);
    presentation::add_rule(p, "cEce"s, "Ecec"s);
    presentation::add_rule(p, "cEcE"s, "EcEc"s);
    presentation::add_rule(p, "EEce"s, "cE"s);

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
    Presentation<std::string> p;
    p.alphabet("aAb"s).contains_empty_word(true);

    presentation::add_rule(p, "aA"s, ""s);
    presentation::add_rule(p, "Aa"s, ""s);
    presentation::add_rule(p, "bb"s, ""s);
    presentation::add_rule(p, "ababab"s, ""s);
    presentation::add_rule(p, "aaaaaaa"s, ""s);

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
    Presentation<std::string> p;
    p.alphabet("aAbcC"s).contains_empty_word(true);

    presentation::add_rule(p, "aA"s, ""s);
    presentation::add_rule(p, "Aa"s, ""s);
    presentation::add_rule(p, "bb"s, ""s);
    presentation::add_rule(p, "cC"s, ""s);
    presentation::add_rule(p, "Cc"s, ""s);
    presentation::add_rule(p, "abc"s, ""s);
    presentation::add_rule(p, "ccc"s, ""s);
    presentation::add_rule(p, "aaaaaaa"s, ""s);

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
    Presentation<std::string> p;
    p.alphabet("aAbBcC"s).contains_empty_word(true);

    presentation::add_rule(p, "aA"s, ""s);
    presentation::add_rule(p, "Aa"s, ""s);
    presentation::add_rule(p, "bB"s, ""s);
    presentation::add_rule(p, "Bb"s, ""s);
    presentation::add_rule(p, "cC"s, ""s);
    presentation::add_rule(p, "Cc"s, ""s);
    presentation::add_rule(p, "aaa"s, ""s);
    presentation::add_rule(p, "bbb"s, ""s);
    presentation::add_rule(p, "abababab"s, ""s);
    presentation::add_rule(p, "abc"s, ""s);

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
    Presentation<std::string> p;
    p.alphabet("aAbB"s).contains_empty_word(true);

    presentation::add_rule(p, "aA"s, ""s);
    presentation::add_rule(p, "Aa"s, ""s);
    presentation::add_rule(p, "bB"s, ""s);
    presentation::add_rule(p, "Bb"s, ""s);
    presentation::add_rule(p, "aaa"s, ""s);
    presentation::add_rule(p, "bbb"s, ""s);
    presentation::add_rule(p, "abababab"s, ""s);

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
    Presentation<std::string> p;
    p.alphabet("aAbB"s).contains_empty_word(true);

    presentation::add_rule(p, "aA"s, ""s);
    presentation::add_rule(p, "Aa"s, ""s);
    presentation::add_rule(p, "bB"s, ""s);
    presentation::add_rule(p, "Bb"s, ""s);
    presentation::add_rule(p, "aaa"s, ""s);
    presentation::add_rule(p, "bbb"s, ""s);
    presentation::add_rule(p, "ababababababab"s, ""s);

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
    Presentation<std::string> p;
    p.alphabet("aAbB"s).contains_empty_word(true);

    presentation::add_rule(p, "aA"s, ""s);
    presentation::add_rule(p, "Aa"s, ""s);
    presentation::add_rule(p, "bB"s, ""s);
    presentation::add_rule(p, "Bb"s, ""s);
    presentation::add_rule(p, "aaa"s, ""s);
    presentation::add_rule(p, "bbbbb"s, ""s);
    presentation::add_rule(p, "ababababababab"s, ""s);

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
    Presentation<std::string> p;
    p.alphabet("aAbBcC"s).contains_empty_word(true);

    presentation::add_rule(p, "aA"s, ""s);
    presentation::add_rule(p, "Aa"s, ""s);
    presentation::add_rule(p, "bB"s, ""s);
    presentation::add_rule(p, "Bb"s, ""s);
    presentation::add_rule(p, "cC"s, ""s);
    presentation::add_rule(p, "Cc"s, ""s);
    presentation::add_rule(p, "aaaa"s, ""s);
    presentation::add_rule(p, "bbb"s, ""s);
    presentation::add_rule(p, "ababababababab"s, ""s);
    presentation::add_rule(p, "abc"s, ""s);

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
    Presentation<std::string> p;
    p.alphabet("aAbB"s).contains_empty_word(true);

    presentation::add_rule(p, "aA"s, ""s);
    presentation::add_rule(p, "Aa"s, ""s);
    presentation::add_rule(p, "bB"s, ""s);
    presentation::add_rule(p, "Bb"s, ""s);
    presentation::add_rule(p, "aaaa"s, ""s);
    presentation::add_rule(p, "bbb"s, ""s);
    presentation::add_rule(p, "ababababababab"s, ""s);

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
    Presentation<std::string> p;
    p.alphabet("aAbB"s).contains_empty_word(true);

    presentation::add_rule(p, "aA"s, ""s);
    presentation::add_rule(p, "Aa"s, ""s);
    presentation::add_rule(p, "bB"s, ""s);
    presentation::add_rule(p, "Bb"s, ""s);
    presentation::add_rule(p, "aaaaa"s, ""s);
    presentation::add_rule(p, "bbbbb"s, ""s);
    presentation::add_rule(p, "ababababab"s, ""s);

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
    Presentation<std::string> p;
    p.alphabet("aAbcC"s).contains_empty_word(true);

    presentation::add_rule(p, "aA"s, ""s);
    presentation::add_rule(p, "Aa"s, ""s);
    presentation::add_rule(p, "bb"s, ""s);
    presentation::add_rule(p, "cC"s, ""s);
    presentation::add_rule(p, "Cc"s, ""s);
    presentation::add_rule(p, "aaaaaaa"s, ""s);
    presentation::add_rule(p, "ccc"s, ""s);
    presentation::add_rule(p, "abc"s, ""s);

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
    Presentation<std::string> p;
    p.alphabet("aAbB"s).contains_empty_word(true);

    presentation::add_rule(p, "aA"s, ""s);
    presentation::add_rule(p, "Aa"s, ""s);
    presentation::add_rule(p, "bB"s, ""s);
    presentation::add_rule(p, "Bb"s, ""s);
    presentation::add_rule(p, "aaaaaaa"s, ""s);
    presentation::add_rule(p, "bbbbbbb"s, ""s);
    presentation::add_rule(p, "ababababababab"s, ""s);

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
    Presentation<std::string> p;
    p.alphabet("aAbB"s).contains_empty_word(true);

    presentation::add_rule(p, "aA"s, ""s);
    presentation::add_rule(p, "Aa"s, ""s);
    presentation::add_rule(p, "bB"s, ""s);
    presentation::add_rule(p, "Bb"s, ""s);
    presentation::add_rule(p, "aaaaaaa"s, ""s);
    presentation::add_rule(p, "bbbbbbb"s, ""s);
    presentation::add_rule(p, "abababababababab"s, ""s);

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
    Presentation<std::string> p;
    p.alphabet("aAbB"s).contains_empty_word(true);

    presentation::add_rule(p, "aA"s, ""s);
    presentation::add_rule(p, "Aa"s, ""s);
    presentation::add_rule(p, "bB"s, ""s);
    presentation::add_rule(p, "Bb"s, ""s);
    presentation::add_rule(p, "aaaaaaa"s, ""s);
    presentation::add_rule(p, "bbbbbbbb"s, ""s);
    presentation::add_rule(p, "abababababababab"s, ""s);

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
    Presentation<std::string> p;
    p.alphabet("aAbB"s).contains_empty_word(true);

    presentation::add_rule(p, "aA"s, ""s);
    presentation::add_rule(p, "Aa"s, ""s);
    presentation::add_rule(p, "bB"s, ""s);
    presentation::add_rule(p, "Bb"s, ""s);
    presentation::add_rule(p, "aaaaaaaa"s, ""s);
    presentation::add_rule(p, "bbbbbbbb"s, ""s);
    presentation::add_rule(p, "abababababababab"s, ""s);

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
    Presentation<std::string> p;
    p.alphabet("abc"s).contains_empty_word(true);

    presentation::add_rule(p, "aa"s, ""s);
    presentation::add_rule(p, "bb"s, ""s);
    presentation::add_rule(p, "cc"s, ""s);
    presentation::add_rule(p, "bcbcbcbcbcbcbcbcbcbcbcbc"s, ""s);
    presentation::add_rule(p, "caca"s, ""s);
    presentation::add_rule(p, "ababab"s, ""s);

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
    Presentation<std::string> p;
    p.alphabet("abc"s).contains_empty_word(true);

    presentation::add_rule(p, "aa"s, ""s);
    presentation::add_rule(p, "bb"s, ""s);
    presentation::add_rule(p, "cc"s, ""s);
    presentation::add_rule(p, "bcbc"s, ""s);
    presentation::add_rule(p, "cacaca"s, ""s);
    presentation::add_rule(p, "ababababababab"s, ""s);

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
    Presentation<std::string> p;
    p.alphabet("abc"s).contains_empty_word(true);

    presentation::add_rule(p, "aa"s, ""s);
    presentation::add_rule(p, "bb"s, ""s);
    presentation::add_rule(p, "cc"s, ""s);
    presentation::add_rule(p, "bcbc"s, ""s);
    presentation::add_rule(p, "cacacaca"s, ""s);
    presentation::add_rule(p, "ababababab"s, ""s);

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
    Presentation<std::string> p;
    p.alphabet("abc"s).contains_empty_word(true);

    presentation::add_rule(p, "aa"s, ""s);
    presentation::add_rule(p, "bb"s, ""s);
    presentation::add_rule(p, "cc"s, ""s);
    presentation::add_rule(p, "bcbcbcbc"s, ""s);
    presentation::add_rule(p, "caca"s, ""s);
    presentation::add_rule(p, "ababababab"s, ""s);

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
    Presentation<std::string> p;
    p.alphabet("abc"s).contains_empty_word(true);

    presentation::add_rule(p, "aa"s, ""s);
    presentation::add_rule(p, "bb"s, ""s);
    presentation::add_rule(p, "cc"s, ""s);
    presentation::add_rule(p, "bcbcbcbc"s, ""s);
    presentation::add_rule(p, "cacaca"s, ""s);
    presentation::add_rule(p, "abababab"s, ""s);

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
    Presentation<std::string> p;
    p.alphabet("abc"s).contains_empty_word(true);

    presentation::add_rule(p, "aa"s, ""s);
    presentation::add_rule(p, "bb"s, ""s);
    presentation::add_rule(p, "cc"s, ""s);
    presentation::add_rule(p, "caca"s, ""s);
    presentation::add_rule(p, "babababa"s, ""s);
    presentation::add_rule(p, "cbcbcbcbcb"s, ""s);

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
    Presentation<std::string> p;
    p.alphabet("abc"s).contains_empty_word(true);

    presentation::add_rule(p, "aa"s, ""s);
    presentation::add_rule(p, "bb"s, ""s);
    presentation::add_rule(p, "cc"s, ""s);
    presentation::add_rule(p, "caca"s, ""s);
    presentation::add_rule(p, "bababababa"s, ""s);
    presentation::add_rule(p, "cbcbcbcbcb"s, ""s);

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
    Presentation<std::string> p;
    p.alphabet("abc"s).contains_empty_word(true);

    presentation::add_rule(p, "aa"s, ""s);
    presentation::add_rule(p, "bb"s, ""s);
    presentation::add_rule(p, "cc"s, ""s);
    presentation::add_rule(p, "caca"s, ""s);
    presentation::add_rule(p, "babababa"s, ""s);
    presentation::add_rule(p, "cbcbcbcbcbcb"s, ""s);

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
    Presentation<std::string> p;
    p.alphabet("abc"s).contains_empty_word(true);

    presentation::add_rule(p, "aa"s, ""s);
    presentation::add_rule(p, "bb"s, ""s);
    presentation::add_rule(p, "cc"s, ""s);
    presentation::add_rule(p, "caca"s, ""s);
    presentation::add_rule(p, "babababababa"s, ""s);
    presentation::add_rule(p, "cbcbcbcbcbcb"s, ""s);

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
    Presentation<std::string> p;
    p.alphabet("abc"s).contains_empty_word(true);

    presentation::add_rule(p, "aa"s, ""s);
    presentation::add_rule(p, "bb"s, ""s);
    presentation::add_rule(p, "cc"s, ""s);
    presentation::add_rule(p, "bcbcbcbcbcbc"s, ""s);
    presentation::add_rule(p, "cacaca"s, ""s);
    presentation::add_rule(p, "abababab"s, ""s);

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
    Presentation<std::string> p;
    p.alphabet("abc"s).contains_empty_word(true);

    presentation::add_rule(p, "aa"s, ""s);
    presentation::add_rule(p, "bb"s, ""s);
    presentation::add_rule(p, "cc"s, ""s);
    presentation::add_rule(p, "bcbcbcbcbcbc"s, ""s);
    presentation::add_rule(p, "cacacacaca"s, ""s);
    presentation::add_rule(p, "abababab"s, ""s);

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
    Presentation<std::string> p;
    p.alphabet("abc"s).contains_empty_word(true);

    presentation::add_rule(p, "aa"s, ""s);
    presentation::add_rule(p, "bb"s, ""s);
    presentation::add_rule(p, "cc"s, ""s);
    presentation::add_rule(p, "bcbcbcbcbcbcbc"s, ""s);
    presentation::add_rule(p, "caca"s, ""s);
    presentation::add_rule(p, "ababab"s, ""s);

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
    Presentation<std::string> p;
    p.alphabet("abc"s).contains_empty_word(true);

    presentation::add_rule(p, "aa"s, ""s);
    presentation::add_rule(p, "bb"s, ""s);
    presentation::add_rule(p, "cc"s, ""s);
    presentation::add_rule(p, "bcbcbcbcbcbcbcbc"s, ""s);
    presentation::add_rule(p, "caca"s, ""s);
    presentation::add_rule(p, "ababab"s, ""s);

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
    Presentation<std::string> p;
    p.alphabet("abc"s).contains_empty_word(true);

    presentation::add_rule(p, "aa"s, ""s);
    presentation::add_rule(p, "bb"s, ""s);
    presentation::add_rule(p, "cc"s, ""s);
    presentation::add_rule(p, "bcbcbcbcbcbcbcbc"s, ""s);
    presentation::add_rule(p, "caca"s, ""s);
    presentation::add_rule(p, "abababab"s, ""s);

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
    Presentation<std::string> p;
    p.alphabet("aAbB"s).contains_empty_word(true);

    presentation::add_rule(p, "aA"s, ""s);
    presentation::add_rule(p, "Aa"s, ""s);
    presentation::add_rule(p, "bB"s, ""s);
    presentation::add_rule(p, "Bb"s, ""s);

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
    Presentation<std::string> p;
    p.alphabet("aAbBcC"s).contains_empty_word(true);

    presentation::add_rule(p, "aA"s, ""s);
    presentation::add_rule(p, "Aa"s, ""s);
    presentation::add_rule(p, "bB"s, ""s);
    presentation::add_rule(p, "Bb"s, ""s);
    presentation::add_rule(p, "cC"s, ""s);
    presentation::add_rule(p, "Cc"s, ""s);

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
    Presentation<std::string> p;
    p.alphabet("xXyYzZ"s).contains_empty_word(true);

    presentation::add_rule(p, "xX"s, ""s);
    presentation::add_rule(p, "Xx"s, ""s);
    presentation::add_rule(p, "yY"s, ""s);
    presentation::add_rule(p, "Yy"s, ""s);
    presentation::add_rule(p, "zZ"s, ""s);
    presentation::add_rule(p, "Zz"s, ""s);

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
    Presentation<std::string> p;
    p.alphabet("aAbBcCdDeEfFgGhH"s).contains_empty_word(true);

    presentation::add_rule(p, "aA"s, ""s);
    presentation::add_rule(p, "Aa"s, ""s);
    presentation::add_rule(p, "bB"s, ""s);
    presentation::add_rule(p, "Bb"s, ""s);
    presentation::add_rule(p, "cC"s, ""s);
    presentation::add_rule(p, "Cc"s, ""s);
    presentation::add_rule(p, "dD"s, ""s);
    presentation::add_rule(p, "Dd"s, ""s);
    presentation::add_rule(p, "eE"s, ""s);
    presentation::add_rule(p, "Ee"s, ""s);
    presentation::add_rule(p, "fF"s, ""s);
    presentation::add_rule(p, "Ff"s, ""s);
    presentation::add_rule(p, "gG"s, ""s);
    presentation::add_rule(p, "Gg"s, ""s);
    presentation::add_rule(p, "hH"s, ""s);
    presentation::add_rule(p, "Hh"s, ""s);

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
    Presentation<std::string> p;
    p.alphabet("aAbBcCdDeE"s).contains_empty_word(true);

    presentation::add_rule(p, "aA"s, ""s);
    presentation::add_rule(p, "Aa"s, ""s);
    presentation::add_rule(p, "bB"s, ""s);
    presentation::add_rule(p, "Bb"s, ""s);
    presentation::add_rule(p, "cC"s, ""s);
    presentation::add_rule(p, "Cc"s, ""s);
    presentation::add_rule(p, "dD"s, ""s);
    presentation::add_rule(p, "Dd"s, ""s);
    presentation::add_rule(p, "eE"s, ""s);
    presentation::add_rule(p, "Ee"s, ""s);
    presentation::add_rule(p, "ab"s, "c"s);
    presentation::add_rule(p, "bc"s, "d"s);
    presentation::add_rule(p, "cd"s, "e"s);
    presentation::add_rule(p, "de"s, "a"s);
    presentation::add_rule(p, "ea"s, "b"s);

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
    Presentation<std::string> p;
    p.alphabet("aAbBcCdDeE"s).contains_empty_word(true);

    presentation::add_rule(p, "aA"s, ""s);
    presentation::add_rule(p, "Aa"s, ""s);
    presentation::add_rule(p, "bB"s, ""s);
    presentation::add_rule(p, "Bb"s, ""s);
    presentation::add_rule(p, "cC"s, ""s);
    presentation::add_rule(p, "Cc"s, ""s);
    presentation::add_rule(p, "dD"s, ""s);
    presentation::add_rule(p, "Dd"s, ""s);
    presentation::add_rule(p, "eE"s, ""s);
    presentation::add_rule(p, "Ee"s, ""s);
    presentation::add_rule(p, "abc"s, "d"s);
    presentation::add_rule(p, "bcd"s, "e"s);
    presentation::add_rule(p, "cde"s, "a"s);
    presentation::add_rule(p, "dea"s, "b"s);
    presentation::add_rule(p, "eab"s, "c"s);

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
    Presentation<std::string> p;
    p.alphabet("abB"s).contains_empty_word(true);

    presentation::add_rule(p, "aa"s, ""s);
    presentation::add_rule(p, "bB"s, ""s);
    presentation::add_rule(p, "Bb"s, ""s);
    presentation::add_rule(p, "bbb"s, ""s);
    presentation::add_rule(p, "ababababab"s, ""s);

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
    Presentation<std::string> p;
    p.alphabet("abc"s).contains_empty_word(true);

    presentation::add_rule(p, "aa"s, ""s);
    presentation::add_rule(p, "bb"s, ""s);
    presentation::add_rule(p, "cc"s, ""s);
    presentation::add_rule(p, "bcbcbcbc"s, ""s);
    presentation::add_rule(p, "caca"s, ""s);
    presentation::add_rule(p, "abab"s, ""s);

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
    Presentation<std::string> p;
    p.alphabet("abc"s).contains_empty_word(true);

    presentation::add_rule(p, "aa"s, ""s);
    presentation::add_rule(p, "bb"s, ""s);
    presentation::add_rule(p, "cc"s, ""s);
    presentation::add_rule(p, "bcbcbcbc"s, ""s);
    presentation::add_rule(p, "caca"s, ""s);
    presentation::add_rule(p, "ababab"s, ""s);
    presentation::add_rule(p, "abcbabcbc"s, ""s);

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
    Presentation<std::string> p;
    p.alphabet("abc"s).contains_empty_word(true);

    presentation::add_rule(p, "aa"s, ""s);
    presentation::add_rule(p, "bb"s, ""s);
    presentation::add_rule(p, "cc"s, ""s);
    presentation::add_rule(p, "bcbcbcbcbc"s, ""s);
    presentation::add_rule(p, "caca"s, ""s);
    presentation::add_rule(p, "ababab"s, ""s);

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
    Presentation<std::string> p;
    p.alphabet("abcd"s).contains_empty_word(true);

    presentation::add_rule(p, "aa"s, ""s);
    presentation::add_rule(p, "bb"s, ""s);
    presentation::add_rule(p, "cc"s, ""s);
    presentation::add_rule(p, "dd"s, ""s);
    presentation::add_rule(p, "aa"s, ""s);
    presentation::add_rule(p, "bb"s, ""s);
    presentation::add_rule(p, "cc"s, ""s);
    presentation::add_rule(p, "dd"s, ""s);
    presentation::add_rule(p, "bab"s, "aba"s);
    presentation::add_rule(p, "ca"s, "ac"s);
    presentation::add_rule(p, "dad"s, "ada"s);
    presentation::add_rule(p, "cbc"s, "bcb"s);
    presentation::add_rule(p, "db"s, "bd"s);
    presentation::add_rule(p, "dcd"s, "cdc"s);

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
    Presentation<std::string> p;
    p.alphabet("abcde"s).contains_empty_word(true);

    presentation::add_rule(p, "aa"s, ""s);
    presentation::add_rule(p, "bb"s, ""s);
    presentation::add_rule(p, "cc"s, ""s);
    presentation::add_rule(p, "dd"s, ""s);
    presentation::add_rule(p, "ee"s, ""s);
    presentation::add_rule(p, "bab"s, "aba"s);
    presentation::add_rule(p, "ca"s, "ac"s);
    presentation::add_rule(p, "da"s, "ad"s);
    presentation::add_rule(p, "eae"s, "aea"s);
    presentation::add_rule(p, "cbc"s, "bcb"s);
    presentation::add_rule(p, "db"s, "bd"s);
    presentation::add_rule(p, "eb"s, "be"s);
    presentation::add_rule(p, "dcd"s, "cdc"s);
    presentation::add_rule(p, "ec"s, "ce"s);
    presentation::add_rule(p, "ede"s, "ded"s);  // codespell:ignore

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
    Presentation<std::string> p;
    p.alphabet("dabc"s).contains_empty_word(true);

    presentation::add_rule(p, "dd"s, ""s);
    presentation::add_rule(p, "aa"s, ""s);
    presentation::add_rule(p, "bb"s, ""s);
    presentation::add_rule(p, "cc"s, ""s);
    presentation::add_rule(p, "bab"s, "aba"s);
    presentation::add_rule(p, "ca"s, "ac"s);
    presentation::add_rule(p, "adadadad"s, ""s);
    presentation::add_rule(p, "cbcbcbcb"s, ""s);
    presentation::add_rule(p, "db"s, "bd"s);
    presentation::add_rule(p, "dc"s, "cd"s);

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
    Presentation<std::string> p;
    p.alphabet("dabc"s).contains_empty_word(true);

    presentation::add_rule(p, "dd"s, ""s);
    presentation::add_rule(p, "aa"s, ""s);
    presentation::add_rule(p, "bb"s, ""s);
    presentation::add_rule(p, "cc"s, ""s);
    presentation::add_rule(p, "bab"s, "aba"s);
    presentation::add_rule(p, "ca"s, "ac"s);
    presentation::add_rule(p, "da"s, "ad"s);
    presentation::add_rule(p, "cbcbcbcb"s, ""s);
    presentation::add_rule(p, "bdbdbd"s, ""s);
    presentation::add_rule(p, "dc"s, "cd"s);

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
    Presentation<std::string> p;
    p.alphabet("abcde"s).contains_empty_word(true);

    presentation::add_rule(p, "aa"s, ""s);
    presentation::add_rule(p, "bb"s, ""s);
    presentation::add_rule(p, "cc"s, ""s);
    presentation::add_rule(p, "dd"s, ""s);
    presentation::add_rule(p, "ee"s, ""s);
    presentation::add_rule(p, "ababab"s, ""s);
    presentation::add_rule(p, "ca"s, "ac"s);
    presentation::add_rule(p, "da"s, "ad"s);
    presentation::add_rule(p, "ea"s, "ae"s);
    presentation::add_rule(p, "bcbcbc"s, ""s);
    presentation::add_rule(p, "bdbdbd"s, ""s);
    presentation::add_rule(p, "bebebe"s, ""s);
    presentation::add_rule(p, "dc"s, "cd"s);
    presentation::add_rule(p, "ec"s, "ce"s);
    presentation::add_rule(p, "ed"s, "de"s);

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
    Presentation<std::string> p;
    p.alphabet("abcdef"s).contains_empty_word(true);

    presentation::add_rule(p, "aa"s, ""s);
    presentation::add_rule(p, "bb"s, ""s);
    presentation::add_rule(p, "cc"s, ""s);
    presentation::add_rule(p, "dd"s, ""s);
    presentation::add_rule(p, "ee"s, ""s);
    presentation::add_rule(p, "ff"s, ""s);
    presentation::add_rule(p, "bab"s, "aba"s);
    presentation::add_rule(p, "ca"s, "ac"s);
    presentation::add_rule(p, "da"s, "ad"s);
    presentation::add_rule(p, "ea"s, "ae"s);
    presentation::add_rule(p, "fa"s, "af"s);
    presentation::add_rule(p, "cbc"s, "bcb"s);
    presentation::add_rule(p, "db"s, "bd"s);
    presentation::add_rule(p, "eb"s, "be"s);
    presentation::add_rule(p, "fbf"s, "bfb"s);
    presentation::add_rule(p, "dcd"s, "cdc"s);
    presentation::add_rule(p, "ece"s, "cec"s);
    presentation::add_rule(p, "fc"s, "cf"s);
    presentation::add_rule(p, "ed"s, "de"s);
    presentation::add_rule(p, "fd"s, "df"s);
    presentation::add_rule(p, "fe"s, "ef"s);

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
    Presentation<std::string> p;
    p.alphabet("abc"s).contains_empty_word(true);

    presentation::add_rule(p, "aa"s, ""s);
    presentation::add_rule(p, "bb"s, ""s);
    presentation::add_rule(p, "cc"s, ""s);
    presentation::add_rule(p, "bab"s, "aba"s);
    presentation::add_rule(p, "ca"s, "ac"s);
    presentation::add_rule(p, "bcbcbcbcbcbc"s, ""s);

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
    Presentation<std::string> p;
    p.alphabet("fedcba"s).contains_empty_word(true);

    presentation::add_rule(p, "ff"s, ""s);
    presentation::add_rule(p, "ee"s, ""s);
    presentation::add_rule(p, "dd"s, ""s);
    presentation::add_rule(p, "cc"s, ""s);
    presentation::add_rule(p, "bb"s, ""s);
    presentation::add_rule(p, "aa"s, ""s);
    presentation::add_rule(p, "ababababababab"s, ""s);
    presentation::add_rule(p, "acacacacacacac"s, ""s);
    presentation::add_rule(p, "bcbcbcbcbcbcbc"s, ""s);
    presentation::add_rule(p, "f"s, "abababa"s);
    presentation::add_rule(p, "e"s, "acacaca"s);
    presentation::add_rule(p, "d"s, "bcbcbcb"s);

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
    Presentation<std::string> p;
    p.alphabet("abcdefghijklmno"s).contains_empty_word(true);

    presentation::add_rule(p, "aa"s, ""s);
    presentation::add_rule(p, "bb"s, ""s);
    presentation::add_rule(p, "cc"s, ""s);
    presentation::add_rule(p, "dd"s, ""s);
    presentation::add_rule(p, "ee"s, ""s);
    presentation::add_rule(p, "ff"s, ""s);
    presentation::add_rule(p, "gg"s, ""s);
    presentation::add_rule(p, "hh"s, ""s);
    presentation::add_rule(p, "ii"s, ""s);
    presentation::add_rule(p, "jj"s, ""s);
    presentation::add_rule(p, "kk"s, ""s);
    presentation::add_rule(p, "ll"s, ""s);
    presentation::add_rule(p, "mm"s, ""s);
    presentation::add_rule(p, "nn"s, ""s);
    presentation::add_rule(p, "oo"s, ""s);
    presentation::add_rule(p, "bab"s, "aba"s);
    presentation::add_rule(p, "ca"s, "ac"s);
    presentation::add_rule(p, "da"s, "ad"s);
    presentation::add_rule(p, "ea"s, "ae"s);
    presentation::add_rule(p, "fa"s, "af"s);
    presentation::add_rule(p, "ga"s, "ag"s);
    presentation::add_rule(p, "ha"s, "ah"s);
    presentation::add_rule(p, "ia"s, "ai"s);
    presentation::add_rule(p, "ja"s, "aj"s);
    presentation::add_rule(p, "ka"s, "ak"s);
    presentation::add_rule(p, "la"s, "al"s);
    presentation::add_rule(p, "ma"s, "am"s);
    presentation::add_rule(p, "na"s, "an"s);
    presentation::add_rule(p, "oa"s, "ao"s);
    presentation::add_rule(p, "cbc"s, "bcb"s);
    presentation::add_rule(p, "db"s, "bd"s);
    presentation::add_rule(p, "eb"s, "be"s);
    presentation::add_rule(p, "fb"s, "bf"s);
    presentation::add_rule(p, "gb"s, "bg"s);
    presentation::add_rule(p, "hb"s, "bh"s);
    presentation::add_rule(p, "ib"s, "bi"s);
    presentation::add_rule(p, "jb"s, "bj"s);
    presentation::add_rule(p, "kb"s, "bk"s);
    presentation::add_rule(p, "lb"s, "bl"s);
    presentation::add_rule(p, "mb"s, "bm"s);
    presentation::add_rule(p, "nb"s, "bn"s);
    presentation::add_rule(p, "ob"s, "bo"s);
    presentation::add_rule(p, "dcd"s, "cdc"s);
    presentation::add_rule(p, "ec"s, "ce"s);
    presentation::add_rule(p, "fc"s, "cf"s);
    presentation::add_rule(p, "gc"s, "cg"s);
    presentation::add_rule(p, "hc"s, "ch"s);
    presentation::add_rule(p, "ic"s, "ci"s);
    presentation::add_rule(p, "jc"s, "cj"s);
    presentation::add_rule(p, "kc"s, "ck"s);
    presentation::add_rule(p, "lc"s, "cl"s);
    presentation::add_rule(p, "mc"s, "cm"s);
    presentation::add_rule(p, "nc"s, "cn"s);
    presentation::add_rule(p, "oc"s, "co"s);
    presentation::add_rule(p, "ede"s, "ded"s);  // codespell:ignore
    presentation::add_rule(p, "fd"s, "df"s);
    presentation::add_rule(p, "gd"s, "dg"s);
    presentation::add_rule(p, "hd"s, "dh"s);
    presentation::add_rule(p, "id"s, "di"s);
    presentation::add_rule(p, "jd"s, "dj"s);
    presentation::add_rule(p, "kd"s, "dk"s);
    presentation::add_rule(p, "ld"s, "dl"s);
    presentation::add_rule(p, "md"s, "dm"s);
    presentation::add_rule(p, "nd"s, "dn"s);  // codespell:ignore
    presentation::add_rule(p, "od"s, "do"s);  // codespell:ignore
    presentation::add_rule(p, "fef"s, "efe"s);
    presentation::add_rule(p, "ge"s, "eg"s);
    presentation::add_rule(p, "he"s, "eh"s);
    presentation::add_rule(p, "ie"s, "ei"s);
    presentation::add_rule(p, "je"s, "ej"s);
    presentation::add_rule(p, "ke"s, "ek"s);
    presentation::add_rule(p, "le"s, "el"s);
    presentation::add_rule(p, "me"s, "em"s);
    presentation::add_rule(p, "ne"s, "en"s);
    presentation::add_rule(p, "oe"s, "eo"s);
    presentation::add_rule(p, "gfg"s, "fgf"s);
    presentation::add_rule(p, "hf"s, "fh"s);
    presentation::add_rule(p, "if"s, "fi"s);
    presentation::add_rule(p, "jf"s, "fj"s);
    presentation::add_rule(p, "kf"s, "fk"s);
    presentation::add_rule(p, "lf"s, "fl"s);
    presentation::add_rule(p, "mf"s, "fm"s);
    presentation::add_rule(p, "nf"s, "fn"s);
    presentation::add_rule(p, "of"s, "fo"s);  // codespell:ignore
    presentation::add_rule(p, "hgh"s, "ghg"s);
    presentation::add_rule(p, "ig"s, "gi"s);
    presentation::add_rule(p, "jg"s, "gj"s);
    presentation::add_rule(p, "kg"s, "gk"s);
    presentation::add_rule(p, "lg"s, "gl"s);
    presentation::add_rule(p, "mg"s, "gm"s);
    presentation::add_rule(p, "ng"s, "gn"s);
    presentation::add_rule(p, "og"s, "go"s);
    presentation::add_rule(p, "ihi"s, "hih"s);
    presentation::add_rule(p, "jh"s, "hj"s);
    presentation::add_rule(p, "kh"s, "hk"s);
    presentation::add_rule(p, "lh"s, "hl"s);
    presentation::add_rule(p, "mh"s, "hm"s);
    presentation::add_rule(p, "nh"s, "hn"s);
    presentation::add_rule(p, "oh"s, "ho"s);
    presentation::add_rule(p, "jij"s, "iji"s);
    presentation::add_rule(p, "ki"s, "ik"s);
    presentation::add_rule(p, "li"s, "il"s);
    presentation::add_rule(p, "mi"s, "im"s);
    presentation::add_rule(p, "ni"s, "in"s);
    presentation::add_rule(p, "oi"s, "io"s);
    presentation::add_rule(p, "kjk"s, "jkj"s);
    presentation::add_rule(p, "lj"s, "jl"s);
    presentation::add_rule(p, "mj"s, "jm"s);
    presentation::add_rule(p, "nj"s, "jn"s);
    presentation::add_rule(p, "oj"s, "jo"s);
    presentation::add_rule(p, "lkl"s, "klk"s);
    presentation::add_rule(p, "mk"s, "km"s);
    presentation::add_rule(p, "nk"s, "kn"s);
    presentation::add_rule(p, "ok"s, "ko"s);
    presentation::add_rule(p, "mlm"s, "lml"s);
    presentation::add_rule(p, "nl"s, "ln"s);
    presentation::add_rule(p, "ol"s, "lo"s);
    presentation::add_rule(p, "nmn"s, "mnm"s);
    presentation::add_rule(p, "om"s, "mo"s);
    presentation::add_rule(p, "ono"s, "non"s);

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
    Presentation<std::string> p;
    p.alphabet("abc"s).contains_empty_word(true);

    presentation::add_rule(p, "aa"s, ""s);
    presentation::add_rule(p, "bb"s, ""s);
    presentation::add_rule(p, "cc"s, ""s);
    presentation::add_rule(p, "bab"s, "aba"s);
    presentation::add_rule(p, "ca"s, "ac"s);
    presentation::add_rule(p, "cbcbcbcb"s, ""s);

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
    Presentation<std::string> p;
    p.alphabet("abcd"s).contains_empty_word(true);

    presentation::add_rule(p, "aa"s, ""s);
    presentation::add_rule(p, "bb"s, ""s);
    presentation::add_rule(p, "cc"s, ""s);
    presentation::add_rule(p, "dd"s, ""s);
    presentation::add_rule(p, "bab"s, "aba"s);
    presentation::add_rule(p, "ca"s, "ac"s);
    presentation::add_rule(p, "da"s, "ad"s);
    presentation::add_rule(p, "cbc"s, "bcb"s);
    presentation::add_rule(p, "db"s, "bd"s);
    presentation::add_rule(p, "cdcdcdcd"s, ""s);

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
    Presentation<std::string> p;
    p.alphabet("abcd"s).contains_empty_word(true);

    presentation::add_rule(p, "aa"s, ""s);
    presentation::add_rule(p, "bb"s, ""s);
    presentation::add_rule(p, "cc"s, ""s);
    presentation::add_rule(p, "dd"s, ""s);
    presentation::add_rule(p, "aa"s, ""s);
    presentation::add_rule(p, "bb"s, ""s);
    presentation::add_rule(p, "cc"s, ""s);
    presentation::add_rule(p, "dd"s, ""s);
    presentation::add_rule(p, "bab"s, "aba"s);
    presentation::add_rule(p, "cac"s, "aca"s);
    presentation::add_rule(p, "dadada"s, "adadad"s);
    presentation::add_rule(p, "cbc"s, "bcb"s);
    presentation::add_rule(p, "db"s, "bd"s);
    presentation::add_rule(p, "dc"s, "cd"s);

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
    Presentation<std::string> p;
    p.alphabet("abcdef"s).contains_empty_word(true);

    presentation::add_rule(p, "aa"s, ""s);
    presentation::add_rule(p, "bb"s, ""s);
    presentation::add_rule(p, "cc"s, ""s);
    presentation::add_rule(p, "dd"s, ""s);
    presentation::add_rule(p, "ee"s, ""s);
    presentation::add_rule(p, "ff"s, ""s);
    presentation::add_rule(p, "bab"s, "aba"s);
    presentation::add_rule(p, "ca"s, "ac"s);
    presentation::add_rule(p, "da"s, "ad"s);
    presentation::add_rule(p, "ea"s, "ae"s);
    presentation::add_rule(p, "fa"s, "af"s);
    presentation::add_rule(p, "cbc"s, "bcb"s);
    presentation::add_rule(p, "db"s, "bd"s);
    presentation::add_rule(p, "eb"s, "be"s);
    presentation::add_rule(p, "fb"s, "bf"s);
    presentation::add_rule(p, "dcd"s, "cdc"s);
    presentation::add_rule(p, "ece"s, "cec"s);
    presentation::add_rule(p, "fc"s, "cf"s);
    presentation::add_rule(p, "ed"s, "de"s);
    presentation::add_rule(p, "fd"s, "df"s);
    presentation::add_rule(p, "fef"s, "efe"s);

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
    Presentation<std::string> p;
    p.alphabet("aAbBtTcCdD"s).contains_empty_word(true);

    presentation::add_rule(p, "aA"s, ""s);
    presentation::add_rule(p, "Aa"s, ""s);
    presentation::add_rule(p, "bB"s, ""s);
    presentation::add_rule(p, "Bb"s, ""s);
    presentation::add_rule(p, "tT"s, ""s);
    presentation::add_rule(p, "Tt"s, ""s);
    presentation::add_rule(p, "cC"s, ""s);
    presentation::add_rule(p, "Cc"s, ""s);
    presentation::add_rule(p, "dD"s, ""s);
    presentation::add_rule(p, "Dd"s, ""s);
    presentation::add_rule(p, "dcd"s, "cdc"s);
    presentation::add_rule(p, "t"s, "c"s);
    presentation::add_rule(p, "b"s, "dT"s);
    presentation::add_rule(p, "a"s, "tbT"s);

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
    Presentation<std::string> p;
    p.alphabet("abAB"s).contains_empty_word(true);

    presentation::add_rule(p, "aA"s, ""s);
    presentation::add_rule(p, "bB"s, ""s);
    presentation::add_rule(p, "Aa"s, ""s);
    presentation::add_rule(p, "Bb"s, ""s);
    presentation::add_rule(p, "aaa"s, ""s);
    presentation::add_rule(p, "bbb"s, ""s);
    presentation::add_rule(p, "abababab"s, ""s);
    presentation::add_rule(p, "aBaBaBaBaB"s, ""s);

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
    Presentation<std::string> p;
    p.alphabet("abc"s).contains_empty_word(true);

    presentation::add_rule(p, "aa"s, ""s);
    presentation::add_rule(p, "bc"s, ""s);
    presentation::add_rule(p, "cb"s, ""s);
    presentation::add_rule(p, "bb"s, "c"s);
    presentation::add_rule(p, "cac"s, "aba"s);

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
    Presentation<std::string> p;
    p.alphabet("a"s).contains_empty_word(true);

    presentation::add_rule(p, "aa"s, ""s);

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
    Presentation<std::string> p;
    p.alphabet("aA"s).contains_empty_word(true);

    presentation::add_rule(p, "aA"s, ""s);
    presentation::add_rule(p, "Aa"s, ""s);
    presentation::add_rule(p, "aaaaa"s, ""s);

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
    Presentation<std::string> p;
    p.alphabet("abB"s).contains_empty_word(true);

    presentation::add_rule(p, "aa"s, ""s);
    presentation::add_rule(p, "bB"s, ""s);
    presentation::add_rule(p, "Bb"s, ""s);
    presentation::add_rule(p, "bbb"s, ""s);
    presentation::add_rule(p, "ababababab"s, ""s);

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
    Presentation<std::string> p;
    p.alphabet("aAbBcCdDeEfF"s).contains_empty_word(true);

    presentation::add_rule(p, "aA"s, ""s);
    presentation::add_rule(p, "Aa"s, ""s);
    presentation::add_rule(p, "bB"s, ""s);
    presentation::add_rule(p, "Bb"s, ""s);
    presentation::add_rule(p, "cC"s, ""s);
    presentation::add_rule(p, "Cc"s, ""s);
    presentation::add_rule(p, "dD"s, ""s);
    presentation::add_rule(p, "Dd"s, ""s);
    presentation::add_rule(p, "eE"s, ""s);
    presentation::add_rule(p, "Ee"s, ""s);
    presentation::add_rule(p, "fF"s, ""s);
    presentation::add_rule(p, "Ff"s, ""s);
    presentation::add_rule(p, "aCAd"s, ""s);
    presentation::add_rule(p, "bfBE"s, ""s);
    presentation::add_rule(p, "ceCD"s, ""s);
    presentation::add_rule(p, "dFDa"s, ""s);
    presentation::add_rule(p, "ebEA"s, ""s);
    presentation::add_rule(p, "fCFB"s, ""s);

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
    Presentation<std::string> p;
    p.alphabet("abB"s).contains_empty_word(true);

    presentation::add_rule(p, "aa"s, ""s);
    presentation::add_rule(p, "bB"s, ""s);
    presentation::add_rule(p, "Bb"s, ""s);
    presentation::add_rule(p, "bbb"s, ""s);
    presentation::add_rule(p, "abab"s, ""s);

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
    Presentation<std::string> p;
    p.alphabet("aAbBcCdDeE"s).contains_empty_word(true);

    presentation::add_rule(p, "aA"s, ""s);
    presentation::add_rule(p, "Aa"s, ""s);
    presentation::add_rule(p, "bB"s, ""s);
    presentation::add_rule(p, "Bb"s, ""s);
    presentation::add_rule(p, "cC"s, ""s);
    presentation::add_rule(p, "Cc"s, ""s);
    presentation::add_rule(p, "dD"s, ""s);
    presentation::add_rule(p, "Dd"s, ""s);
    presentation::add_rule(p, "eE"s, ""s);
    presentation::add_rule(p, "Ee"s, ""s);
    presentation::add_rule(p, "add"s, ""s);
    presentation::add_rule(p, "bddd"s, ""s);
    presentation::add_rule(p, "cdddddeee"s, ""s);

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
    Presentation<std::string> p;
    p.alphabet("aAbB"s).contains_empty_word(true);

    presentation::add_rule(p, "aA"s, ""s);
    presentation::add_rule(p, "Aa"s, ""s);
    presentation::add_rule(p, "bB"s, ""s);
    presentation::add_rule(p, "Bb"s, ""s);
    presentation::add_rule(p, "aaaaa"s, ""s);
    presentation::add_rule(p, "bbbbb"s, ""s);
    presentation::add_rule(p, "baBBAA"s, ""s);

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
    Presentation<std::string> p;
    p.alphabet("hHvV"s).contains_empty_word(true);

    presentation::add_rule(p, "hH"s, ""s);
    presentation::add_rule(p, "Hh"s, ""s);
    presentation::add_rule(p, "vV"s, ""s);
    presentation::add_rule(p, "Vv"s, ""s);
    presentation::add_rule(p, "vh"s, "hv"s);

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
    Presentation<std::string> p;
    p.alphabet("hHvVrxy"s).contains_empty_word(true);

    presentation::add_rule(p, "hH"s, ""s);
    presentation::add_rule(p, "Hh"s, ""s);
    presentation::add_rule(p, "vV"s, ""s);
    presentation::add_rule(p, "Vv"s, ""s);
    presentation::add_rule(p, "rr"s, ""s);
    presentation::add_rule(p, "xx"s, ""s);
    presentation::add_rule(p, "yy"s, ""s);
    presentation::add_rule(p, "vh"s, "hv"s);
    presentation::add_rule(p, "yhy"s, "H"s);
    presentation::add_rule(p, "yv"s, "vy"s);
    presentation::add_rule(p, "xh"s, "hx"s);
    presentation::add_rule(p, "xvx"s, "V"s);
    presentation::add_rule(p, "yx"s, "xy"s);
    presentation::add_rule(p, "rhr"s, "H"s);
    presentation::add_rule(p, "xy"s, "r"s);

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
    Presentation<std::string> p;
    p.alphabet("bBaAt"s).contains_empty_word(true);

    presentation::add_rule(p, "bB"s, ""s);
    presentation::add_rule(p, "Bb"s, ""s);
    presentation::add_rule(p, "aA"s, ""s);
    presentation::add_rule(p, "Aa"s, ""s);
    presentation::add_rule(p, "tt"s, ""s);
    presentation::add_rule(p, "ba"s, "ab"s);
    presentation::add_rule(p, "tat"s, "b"s);

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
    Presentation<std::string> p;
    p.alphabet("aAbB"s).contains_empty_word(true);

    presentation::add_rule(p, "aA"s, ""s);
    presentation::add_rule(p, "Aa"s, ""s);
    presentation::add_rule(p, "bB"s, ""s);
    presentation::add_rule(p, "Bb"s, ""s);
    presentation::add_rule(p, "BAbaBab"s, "AA"s);
    presentation::add_rule(p, "ABabAba"s, "bbbb"s);

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
    Presentation<std::string> p;
    p.alphabet("aAbBcC"s).contains_empty_word(true);

    presentation::add_rule(p, "aA"s, ""s);
    presentation::add_rule(p, "Aa"s, ""s);
    presentation::add_rule(p, "bB"s, ""s);
    presentation::add_rule(p, "Bb"s, ""s);
    presentation::add_rule(p, "cC"s, ""s);
    presentation::add_rule(p, "Cc"s, ""s);
    presentation::add_rule(p, "baB"s, "A"s);
    presentation::add_rule(p, "cbC"s, "B"s);
    presentation::add_rule(p, "acA"s, "C"s);

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
    Presentation<std::string> p;
    p.alphabet("aAbBcC"s).contains_empty_word(true);

    presentation::add_rule(p, "aA"s, ""s);
    presentation::add_rule(p, "Aa"s, ""s);
    presentation::add_rule(p, "bB"s, ""s);
    presentation::add_rule(p, "Bb"s, ""s);
    presentation::add_rule(p, "cC"s, ""s);
    presentation::add_rule(p, "Cc"s, ""s);
    presentation::add_rule(p, "baB"s, "a"s);
    presentation::add_rule(p, "cbC"s, "B"s);
    presentation::add_rule(p, "acA"s, "C"s);

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
    Presentation<std::string> p;
    p.alphabet("aAbBcC"s).contains_empty_word(true);

    presentation::add_rule(p, "aA"s, ""s);
    presentation::add_rule(p, "Aa"s, ""s);
    presentation::add_rule(p, "bB"s, ""s);
    presentation::add_rule(p, "Bb"s, ""s);
    presentation::add_rule(p, "cC"s, ""s);
    presentation::add_rule(p, "Cc"s, ""s);
    presentation::add_rule(p, "baB"s, "a"s);
    presentation::add_rule(p, "cbC"s, "B"s);
    presentation::add_rule(p, "acA"s, "c"s);

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
    Presentation<std::string> p;
    p.alphabet("aAbBcC"s).contains_empty_word(true);

    presentation::add_rule(p, "aA"s, ""s);
    presentation::add_rule(p, "Aa"s, ""s);
    presentation::add_rule(p, "bB"s, ""s);
    presentation::add_rule(p, "Bb"s, ""s);
    presentation::add_rule(p, "cC"s, ""s);
    presentation::add_rule(p, "Cc"s, ""s);
    presentation::add_rule(p, "baB"s, "a"s);
    presentation::add_rule(p, "cbC"s, "b"s);
    presentation::add_rule(p, "acA"s, "C"s);

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
    Presentation<std::string> p;
    p.alphabet("aAbBcC"s).contains_empty_word(true);

    presentation::add_rule(p, "aA"s, ""s);
    presentation::add_rule(p, "Aa"s, ""s);
    presentation::add_rule(p, "bB"s, ""s);
    presentation::add_rule(p, "Bb"s, ""s);
    presentation::add_rule(p, "cC"s, ""s);
    presentation::add_rule(p, "Cc"s, ""s);
    presentation::add_rule(p, "baB"s, "a"s);
    presentation::add_rule(p, "cbC"s, "b"s);
    presentation::add_rule(p, "acA"s, "c"s);

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
    Presentation<std::string> p;
    p.alphabet("aAbBcC"s).contains_empty_word(true);

    presentation::add_rule(p, "aA"s, ""s);
    presentation::add_rule(p, "Aa"s, ""s);
    presentation::add_rule(p, "bB"s, ""s);
    presentation::add_rule(p, "Bb"s, ""s);
    presentation::add_rule(p, "cC"s, ""s);
    presentation::add_rule(p, "Cc"s, ""s);
    presentation::add_rule(p, "baB"s, "aa"s);
    presentation::add_rule(p, "cbC"s, "bb"s);
    presentation::add_rule(p, "acA"s, "CC"s);

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
    Presentation<std::string> p;
    p.alphabet("aAbBcC"s).contains_empty_word(true);

    presentation::add_rule(p, "aA"s, ""s);
    presentation::add_rule(p, "Aa"s, ""s);
    presentation::add_rule(p, "bB"s, ""s);
    presentation::add_rule(p, "Bb"s, ""s);
    presentation::add_rule(p, "cC"s, ""s);
    presentation::add_rule(p, "Cc"s, ""s);
    presentation::add_rule(p, "baB"s, "aa"s);
    presentation::add_rule(p, "cbC"s, "bb"s);
    presentation::add_rule(p, "acA"s, "cc"s);

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
    Presentation<std::string> p;
    p.alphabet("lLrRstj"s).contains_empty_word(true);

    presentation::add_rule(p, "lL"s, ""s);
    presentation::add_rule(p, "Ll"s, ""s);
    presentation::add_rule(p, "rR"s, ""s);
    presentation::add_rule(p, "Rr"s, ""s);
    presentation::add_rule(p, "ss"s, ""s);
    presentation::add_rule(p, "tt"s, ""s);
    presentation::add_rule(p, "jj"s, ""s);
    presentation::add_rule(p, "st"s, "j"s);
    presentation::add_rule(p, "slsl"s, ""s);
    presentation::add_rule(p, "srsr"s, ""s);
    presentation::add_rule(p, "tltl"s, ""s);
    presentation::add_rule(p, "trtr"s, ""s);
    presentation::add_rule(p, "srslsrslsrsl"s, ""s);
    presentation::add_rule(p, "j"s, ""s);

    using RewritingSystem = detail::RewritingSystemTrie<RevRPOCmp>;
    KnuthBendix<std::string, RewritingSystem> kb;
    kb.init(congruence_kind::twosided, p);
    kb.run();
    REQUIRE(kb.rewriting_system().number_of_rules() == 10);
    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "467",
                          "MAF: both/sfdo2",
                          "[quick][maf][both]") {
    Presentation<std::string> p;
    p.alphabet("aAbBcC"s).contains_empty_word(true);

    presentation::add_rule(p, "aA"s, ""s);
    presentation::add_rule(p, "Aa"s, ""s);
    presentation::add_rule(p, "bB"s, ""s);
    presentation::add_rule(p, "Bb"s, ""s);
    presentation::add_rule(p, "cC"s, ""s);
    presentation::add_rule(p, "Cc"s, ""s);
    presentation::add_rule(p, "Ba"s, "Ab"s);
    presentation::add_rule(p, "abC"s, "Ca"s);
    presentation::add_rule(p, "baB"s, "Aba"s);
    presentation::add_rule(p, "aaCB"s, "CCa"s);

    using RewritingSystem = detail::RewritingSystemTrie<RevWrCmp>;
    KnuthBendix<std::string, RewritingSystem> kb;
    kb.init(
        congruence_kind::twosided, p, std::vector<size_t>({1, 2, 1, 2, 3, 4}));
    kb.run();
    REQUIRE(kb.rewriting_system().number_of_rules() == 15);
    REQUIRE(kb.number_of_classes() == 60);
  }

}  // namespace libsemigroups
