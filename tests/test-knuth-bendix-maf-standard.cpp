
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

  // Finite group of structure 2^3.L(3,2) (split extension).
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "671",
                          "MAF: subgroups/l3231",
                          "[standard][maf]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("abBuvw"s).contains_empty_word(true);

    presentation::add_rule(p, "aa"s, "");
    presentation::add_rule(p, "bB"s, "");
    presentation::add_rule(p, "Bb"s, "");
    presentation::add_rule(p, "uu"s, "");
    presentation::add_rule(p, "vv"s, "");
    presentation::add_rule(p, "ww"s, "");
    presentation::add_rule(p, "aa"s, "");
    presentation::add_rule(p, "bbb"s, "");
    presentation::add_rule(p, "ababababababab"s, "");
    presentation::add_rule(p, "aBabaBabaBabaBab"s, "");
    presentation::add_rule(p, "uu"s, "");
    presentation::add_rule(p, "vv"s, "");
    presentation::add_rule(p, "ww"s, "");
    presentation::add_rule(p, "uvuv"s, "");
    presentation::add_rule(p, "uwuw"s, "");
    presentation::add_rule(p, "vwvw"s, "");
    presentation::add_rule(p, "auaw"s, "");
    presentation::add_rule(p, "avauvw"s, "");
    presentation::add_rule(p, "Bubv"s, "");
    presentation::add_rule(p, "Bvbuv"s, "");
    presentation::add_rule(p, "Bwbw"s, "");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb(congruence_kind::onesided, p);

    SECTION("l3231") {
      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 318);
      REQUIRE(kb.number_of_classes() == 1'344);
    }

    SECTION("l3231.sub") {
      knuth_bendix::add_generating_pair(kb, "u", "");
      knuth_bendix::add_generating_pair(kb, "v", "");
      knuth_bendix::add_generating_pair(kb, "w", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 822);
      REQUIRE(kb.number_of_classes() == 168);
    }

    SECTION("l3231.subx") {
      knuth_bendix::add_generating_pair(kb, "b", "");
      knuth_bendix::add_generating_pair(kb, "B", "");
      knuth_bendix::add_generating_pair(kb, "u", "");
      knuth_bendix::add_generating_pair(kb, "v", "");

      kb.run();
      REQUIRE(kb.rewriting_system().number_of_rules() == 477);
      REQUIRE(kb.number_of_classes() == 112);
    }
  }

  // Second of BHN's series of increasingly complicated presentations of 1.
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "672",
                          "MAF: trivial/degen4b",
                          "[standard][maf]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("xXyYzZ"s).contains_empty_word(true);

    presentation::add_rule(p, "xX"s, "");
    presentation::add_rule(p, "Xx"s, "");
    presentation::add_rule(p, "yY"s, "");
    presentation::add_rule(p, "Yy"s, "");
    presentation::add_rule(p, "zZ"s, "");
    presentation::add_rule(p, "Zz"s, "");
    presentation::add_rule(p, "yyXYxYzyZZXyxYYzzYZyzzYZy"s, "");
    presentation::add_rule(p, "zzYZyZxzXXYzyZZxxZXzxxZXz"s, "");
    presentation::add_rule(p, "xxZXzXyxYYZxzXXyyXYxyyXYx"s, "");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb(congruence_kind::twosided, p);
    kb.run();
    REQUIRE(kb.rewriting_system().number_of_rules() == 6);
    REQUIRE(kb.number_of_classes() == 1);
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "673",
                          "MAF: unknown/g8(1_3)",
                          "[standard][maf]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("aAbBcCdDeEfFgGhH"s).contains_empty_word(true);

    presentation::add_rule(p, "aA"s, "");
    presentation::add_rule(p, "Aa"s, "");
    presentation::add_rule(p, "bB"s, "");
    presentation::add_rule(p, "Bb"s, "");
    presentation::add_rule(p, "cC"s, "");
    presentation::add_rule(p, "Cc"s, "");
    presentation::add_rule(p, "dD"s, "");
    presentation::add_rule(p, "Dd"s, "");
    presentation::add_rule(p, "eE"s, "");
    presentation::add_rule(p, "Ee"s, "");
    presentation::add_rule(p, "fF"s, "");
    presentation::add_rule(p, "Ff"s, "");
    presentation::add_rule(p, "gG"s, "");
    presentation::add_rule(p, "Gg"s, "");
    presentation::add_rule(p, "hH"s, "");
    presentation::add_rule(p, "Hh"s, "");
    presentation::add_rule(p, "ab"s, "d");
    presentation::add_rule(p, "bc"s, "e");
    presentation::add_rule(p, "cd"s, "f");
    presentation::add_rule(p, "de"s, "g");
    presentation::add_rule(p, "ef"s, "h");
    presentation::add_rule(p, "fg"s, "a");
    presentation::add_rule(p, "gh"s, "b");
    presentation::add_rule(p, "ha"s, "c");
    presentation::add_rule(p, "aaaaa"s, "");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb(congruence_kind::twosided, p);
    kb.run();
    REQUIRE(kb.rewriting_system().number_of_rules() == 4058);
    REQUIRE(kb.number_of_classes() == 1215);
  }
  // Fibonacci group F(3,6).
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "674",
                          "MAF: Fibonacci/f3_6",
                          "[standard][maf][Fibonacci]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("aAbBcCdDeEfF"s).contains_empty_word(true);

    presentation::add_rule(p, "aA"s, "");
    presentation::add_rule(p, "Aa"s, "");
    presentation::add_rule(p, "bB"s, "");
    presentation::add_rule(p, "Bb"s, "");
    presentation::add_rule(p, "cC"s, "");
    presentation::add_rule(p, "Cc"s, "");
    presentation::add_rule(p, "dD"s, "");
    presentation::add_rule(p, "Dd"s, "");
    presentation::add_rule(p, "eE"s, "");
    presentation::add_rule(p, "Ee"s, "");
    presentation::add_rule(p, "fF"s, "");
    presentation::add_rule(p, "Ff"s, "");
    presentation::add_rule(p, "abc"s, "d");
    presentation::add_rule(p, "bcd"s, "e");
    presentation::add_rule(p, "cde"s, "f");
    presentation::add_rule(p, "def"s, "a");
    presentation::add_rule(p, "efa"s, "b");
    presentation::add_rule(p, "fab"s, "c");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb;
    kb.init(congruence_kind::twosided, p);
    kb.run();
    REQUIRE(kb.rewriting_system().number_of_rules() == 1'568);
    REQUIRE(kb.number_of_classes() == 1'512);
  }

}  // namespace libsemigroups
