// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2022-2025 Murray T. Whyte
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

// This file is the second of three containing tests for the
// presentation-examples functions. The presentations here define not
// necessarily finite semigroups, and we use KnuthBendix in testing them.

#define CATCH_CONFIG_ENABLE_PAIR_STRINGMAKER

#include <algorithm>      // for max, next_permutation
#include <chrono>         // for operator-
#include <cstddef>        // for size_t
#include <list>           // for operator!=
#include <string>         // for basic_string, operator+
#include <unordered_map>  // for unordered_map, operator==
#include <utility>        // for pair
#include <vector>         // for vector, operator==

#include "Catch2-3.8.0/catch_amalgamated.hpp"  // for StringRef, SourceLineInfo
#include "test-main.hpp"                       // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/config.hpp"  // LIBSEMIGROUPS_EIGEN_ENABLED

#include "libsemigroups/constants.hpp"     // for operator==, operator!=
#include "libsemigroups/knuth-bendix.hpp"  // for KnuthBendix, to_present...
#include "libsemigroups/obvinf.hpp"        // for is_obviously_infinite
#include "libsemigroups/paths.hpp"         // for Paths
#include "libsemigroups/presentation-examples.hpp"  // for presentation::examples
#include "libsemigroups/presentation.hpp"           // for longest_rule_length
#include "libsemigroups/ranges.hpp"                 // for operator|, to_vector
#include "libsemigroups/to-presentation.hpp"        // for to<Presentation>
#include "libsemigroups/types.hpp"       // for congruence_kind, word_type
#include "libsemigroups/word-graph.hpp"  // for is_complete
#include "libsemigroups/word-range.hpp"  // for operator""_w, to_string

#include "libsemigroups/detail/eigen.hpp"  // // for DenseBase::row, DenseBa...
#include "libsemigroups/detail/fmt.hpp"    // for format, print
#include "libsemigroups/detail/iterator.hpp"   // for operator+
#include "libsemigroups/detail/report.hpp"     // for ReportGuard
#include "libsemigroups/detail/rewriters.hpp"  // for RewriteTrie

namespace libsemigroups {

  using literals::operator""_w;
  using namespace rx;

  struct LibsemigroupsException;

  using namespace presentation::examples;

  LIBSEMIGROUPS_TEST_CASE("Example",
                          "085",
                          "chinese_monoid(3)",
                          "[Example][quick]") {
    auto rg = ReportGuard(false);

    KnuthBendix kb(congruence_kind::twosided,
                   v4::to<Presentation<std::string>>(chinese_monoid(3)));
    REQUIRE(is_obviously_infinite(kb));
    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
    REQUIRE(kb.presentation().rules
            == std::vector<std::string>({"baa",
                                         "aba",
                                         "caa",
                                         "aca",
                                         "bba",
                                         "bab",
                                         "cba",
                                         "cab",
                                         "cba",
                                         "bca",
                                         "cca",
                                         "cac",
                                         "cbb",
                                         "bcb",
                                         "ccb",
                                         "cbc"}));
    REQUIRE(knuth_bendix::normal_forms(kb).min(0).max(10).count() == 1'176);
  }

  // Note that the alphabet order matters here, if the lhs of the first
  // relation is abc instead of bac (or rather the alphabet is re-ordered so
  // that the first rule is abc), then this runs forever.
  LIBSEMIGROUPS_TEST_CASE("Example",
                          "086",
                          "plactic_monoid(3)",
                          "[Example][quick]") {
    auto rg = ReportGuard(false);
    REQUIRE(plactic_monoid(3).rules
            == std::vector({102_w,
                            120_w,
                            021_w,
                            201_w,
                            100_w,
                            010_w,
                            110_w,
                            101_w,
                            200_w,
                            020_w,
                            220_w,
                            202_w,
                            211_w,
                            121_w,
                            221_w,
                            212_w}));
    auto p = v4::to<Presentation<std::string>>(plactic_monoid(3));
    REQUIRE(p.rules
            == std::vector<std::string>({"bac",
                                         "bca",
                                         "acb",
                                         "cab",
                                         "baa",
                                         "aba",
                                         "bba",
                                         "bab",
                                         "caa",
                                         "aca",
                                         "cca",
                                         "cac",
                                         "cbb",
                                         "bcb",
                                         "ccb",
                                         "cbc"}));
    KnuthBendix kb(congruence_kind::twosided, p);
    REQUIRE(kb.presentation().rules
            == std::vector<std::string>({"bac",
                                         "bca",
                                         "acb",
                                         "cab",
                                         "baa",
                                         "aba",
                                         "bba",
                                         "bab",
                                         "caa",
                                         "aca",
                                         "cca",
                                         "cac",
                                         "cbb",
                                         "bcb",
                                         "ccb",
                                         "cbc"}));
    REQUIRE(kb.presentation().alphabet() == "abc");
    REQUIRE(is_obviously_infinite(kb));
    kb.run();
    // using rule_type = KnuthBendix<>::rule_type;
    // REQUIRE((kb.active_rules() | to_vector())
    //         == std::vector<rule_type>({{"bca", "bac"},
    //                                    {"cab", "acb"},
    //                                    {"baa", "aba"},
    //                                    {"bba", "bab"},
    //                                    {"caa", "aca"},
    //                                    {"cca", "cac"},
    //                                    {"cbb", "bcb"},
    //                                    {"ccb", "cbc"},
    //                                    {"cbab", "bcba"},
    //                                    {"cbcba", "cbacb"},
    //                                    {"cbaca", "cacba"}}));
    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
    REQUIRE(knuth_bendix::normal_forms(kb).min(1).max(5).count() == 70);
  }

  LIBSEMIGROUPS_TEST_CASE("Example",
                          "087",
                          "stylic_monoid(4)",
                          "[Example][quick]") {
    auto        rg = ReportGuard(false);
    KnuthBendix kb(congruence_kind::twosided, stylic_monoid(4));
    // REQUIRE(kb.number_of_classes() == 51);
    REQUIRE(knuth_bendix::normal_forms(kb).min(0).max(6).count() == 50);
  }

  LIBSEMIGROUPS_TEST_CASE("Example",
                          "088",
                          "hypo_plactic_monoid(3)",
                          "[Example][quick]") {
    auto rg = ReportGuard(false);

    KnuthBendix kb(congruence_kind::twosided, hypo_plactic_monoid(3));
    kb.run();

    word_type                             letters = 012_w;
    std::unordered_map<word_type, size_t> map;
    size_t                                next = 0;

    do {
      next += map.emplace(knuth_bendix::reduce(kb, letters), next).second;
    } while (std::next_permutation(letters.begin(), letters.end()));
    REQUIRE(map.size() == 4);

    REQUIRE(knuth_bendix::normal_forms(kb).min(3).max(4).count() == 19);  // 19
    REQUIRE(
        (knuth_bendix::normal_forms(kb).min(3).max(4) | rx::to_vector()).size()
        == 19);
  }

  LIBSEMIGROUPS_TEST_CASE(
      "Example",
      "089",
      "not_renner_type_B_monoid(3, 1) (Godelle presentation)",
      "[quick][Example][hivert][no-valgrind]") {
    auto        rg = ReportGuard(false);
    KnuthBendix kb(congruence_kind::twosided, not_renner_type_B_monoid(3, 1));
    REQUIRE(!is_obviously_infinite(kb));
    kb.run();
    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
  }

  LIBSEMIGROUPS_TEST_CASE(
      "Example",
      "090",
      "not_renner_type_B_monoid(3, 0) (Godelle presentation)",
      "[quick][Example][hivert][no-valgrind]") {
    auto        rg = ReportGuard(false);
    KnuthBendix kb(congruence_kind::twosided, not_renner_type_B_monoid(3, 0));
    REQUIRE(!is_obviously_infinite(kb));
    kb.run();
    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
  }

  LIBSEMIGROUPS_TEST_CASE(
      "Example",
      "091",
      "not_renner_type_D_monoid(2, 1) (Godelle presentation)",
      "[quick][Example][hivert]") {
    auto        rg = ReportGuard(false);
    KnuthBendix kb(congruence_kind::twosided, not_renner_type_D_monoid(2, 1));
    REQUIRE(kb.presentation().rules.size() == 68);
    REQUIRE(!is_obviously_infinite(kb));
    kb.run();
    REQUIRE(kb.number_of_classes() == 37);
  }

  LIBSEMIGROUPS_TEST_CASE(
      "Example",
      "092",
      "not_renner_type_D_monoid(2, 0) (Godelle presentation)",
      "[quick][Example][hivert]") {
    auto        rg = ReportGuard(false);
    KnuthBendix kb(congruence_kind::twosided, not_renner_type_D_monoid(2, 0));

    REQUIRE(kb.presentation().rules.size() == 68);
    REQUIRE(!is_obviously_infinite(kb));
    REQUIRE(kb.number_of_classes() == 37);
  }

  LIBSEMIGROUPS_TEST_CASE(
      "Example",
      "093",
      "not_renner_type_D_monoid(3, 1) (Godelle presentation)",
      "[quick][Example][hivert][no-valgrind]") {
    auto        rg = ReportGuard(false);
    KnuthBendix kb(congruence_kind::twosided, not_renner_type_D_monoid(3, 1));
    REQUIRE(kb.presentation().rules.size() == 130);
    REQUIRE(!is_obviously_infinite(kb));
    REQUIRE(kb.number_of_classes() == 541);
  }

  LIBSEMIGROUPS_TEST_CASE(
      "Example",
      "094",
      "not_renner_type_D_monoid(3, 0) (Godelle presentation)",
      "[quick][Example][hivert][no-valgrind]") {
    auto        rg = ReportGuard(false);
    KnuthBendix kb(congruence_kind::twosided, not_renner_type_D_monoid(3, 0));
    REQUIRE(kb.presentation().rules.size() == 130);
    REQUIRE(!is_obviously_infinite(kb));
    REQUIRE(kb.number_of_classes() == 541);
  }

  LIBSEMIGROUPS_TEST_CASE(
      "Example",
      "095",
      "not_renner_type_D_monoid(4, 1) (Godelle presentation)",
      "[quick][Example][hivert][no-valgrind]") {
    auto        rg = ReportGuard(false);
    KnuthBendix kb(congruence_kind::twosided, not_renner_type_D_monoid(4, 1));
    REQUIRE(kb.presentation().rules.size() == 204);
    REQUIRE(!is_obviously_infinite(kb));
    kb.run();
    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
  }

  LIBSEMIGROUPS_TEST_CASE(
      "Example",
      "096",
      "not_renner_type_D_monoid(4, 0) (Godelle presentation)",
      "[quick][Example][hivert][no-valgrind]") {
    auto        rg = ReportGuard(false);
    KnuthBendix kb(congruence_kind::twosided, not_renner_type_D_monoid(4, 0));
    REQUIRE(kb.presentation().rules.size() == 204);
    REQUIRE(!is_obviously_infinite(kb));
    kb.run();
    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
  }

  LIBSEMIGROUPS_TEST_CASE("Example",
                          "099",
                          "Braid group 4 generators",
                          "[quick]") {
    auto        rg = ReportGuard(false);
    KnuthBendix kb(congruence_kind::twosided, braid_group(5));
    REQUIRE(kb.presentation().contains_empty_word());
    REQUIRE(kb.presentation().alphabet().size() == 8);
    REQUIRE(kb.presentation().rules.size() == 40);

#ifdef LIBSEMIGROUPS_EIGEN_ENABLED
    REQUIRE(is_obviously_infinite(kb));
    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
#endif

    REQUIRE_THROWS_AS(braid_group(0), LibsemigroupsException);
    REQUIRE_THROWS_AS(braid_group(1), LibsemigroupsException);
    REQUIRE_THROWS_AS(braid_group(2), LibsemigroupsException);
  }
}  // namespace libsemigroups
