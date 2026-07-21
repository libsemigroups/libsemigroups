
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

  // Finite group of structure 2^3.L(3,2) (split extension).
  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "671",
                          "MAF: subgroups/l3231",
                          "[standard][maf]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("abBuvw").contains_empty_word(true);

    presentation::add_rule(p, "aa", "");
    presentation::add_rule(p, "bB", "");
    presentation::add_rule(p, "Bb", "");
    presentation::add_rule(p, "uu", "");
    presentation::add_rule(p, "vv", "");
    presentation::add_rule(p, "ww", "");
    presentation::add_rule(p, "aa", "");
    presentation::add_rule(p, "bbb", "");
    presentation::add_rule(p, "ababababababab", "");
    presentation::add_rule(p, "aBabaBabaBabaBab", "");
    presentation::add_rule(p, "uu", "");
    presentation::add_rule(p, "vv", "");
    presentation::add_rule(p, "ww", "");
    presentation::add_rule(p, "uvuv", "");
    presentation::add_rule(p, "uwuw", "");
    presentation::add_rule(p, "vwvw", "");
    presentation::add_rule(p, "auaw", "");
    presentation::add_rule(p, "avauvw", "");
    presentation::add_rule(p, "Bubv", "");
    presentation::add_rule(p, "Bvbuv", "");
    presentation::add_rule(p, "Bwbw", "");

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
    p.alphabet("xXyYzZ").contains_empty_word(true);

    presentation::add_rule(p, "xX", "");
    presentation::add_rule(p, "Xx", "");
    presentation::add_rule(p, "yY", "");
    presentation::add_rule(p, "Yy", "");
    presentation::add_rule(p, "zZ", "");
    presentation::add_rule(p, "Zz", "");
    presentation::add_rule(p, "yyXYxYzyZZXyxYYzzYZyzzYZy", "");
    presentation::add_rule(p, "zzYZyZxzXXYzyZZxxZXzxxZXz", "");
    presentation::add_rule(p, "xxZXzXyxYYZxzXXyyXYxyyXYx", "");

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
    presentation::add_rule(p, "ab", "d");
    presentation::add_rule(p, "bc", "e");
    presentation::add_rule(p, "cd", "f");
    presentation::add_rule(p, "de", "g");
    presentation::add_rule(p, "ef", "h");
    presentation::add_rule(p, "fg", "a");
    presentation::add_rule(p, "gh", "b");
    presentation::add_rule(p, "ha", "c");
    presentation::add_rule(p, "aaaaa", "");

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
    presentation::add_rule(p, "abc", "d");
    presentation::add_rule(p, "bcd", "e");
    presentation::add_rule(p, "cde", "f");
    presentation::add_rule(p, "def", "a");
    presentation::add_rule(p, "efa", "b");
    presentation::add_rule(p, "fab", "c");

    using RewritingSystem = detail::RewritingSystemTrie<LenLexCmp>;
    KnuthBendix<std::string, RewritingSystem> kb;
    kb.init(congruence_kind::twosided, p);
    kb.run();
    REQUIRE(kb.rewriting_system().number_of_rules() == 1'568);
    REQUIRE(kb.number_of_classes() == 1'512);
  }

}  // namespace libsemigroups
