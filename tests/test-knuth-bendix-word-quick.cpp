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

#include <cstddef>  // for size_t
#include <utility>  // for move
#include <vector>   // for vector

#include "Catch2-3.14.0/catch_amalgamated.hpp"  // for operator""_catch_sr
#include "test-main.hpp"  // for LIBSEMIGROUPS_TEMPLATE_TEST_CASE

#include "libsemigroups/constants.hpp"     // for operator==, Max, POSIT...
#include "libsemigroups/knuth-bendix.hpp"  // for KnuthBendix, normal_forms
#include "libsemigroups/obvinf.hpp"        // for is_obviously_infinite
#include "libsemigroups/presentation-examples.hpp"  // for partial_transformation...
#include "libsemigroups/presentation.hpp"           // for add_rule
#include "libsemigroups/to-froidure-pin.hpp"        // for to<FroidurePin>
#include "libsemigroups/types.hpp"                  // for word_type
#include "libsemigroups/word-range.hpp"             // for operator""_w

#include "libsemigroups/detail/report.hpp"  // for ReportGuard

namespace libsemigroups {

  namespace {
    // Generate the 'normal forms' defined by an arbitrary WordGraph.
    // If `wg` corresponds to the Gilman graph of some KnuthBendix instance,
    // then the words returned are the normal forms of that KnuthBendix
    // instance. Since the node labels returned by gilman_graph() are
    // implementation dependent, the below function can be used to check that
    // `gilman_graph()` returns something that generates the correct normal
    // forms.
    template <typename RewritingSystem,
              typename ReductionOrder,
              typename WordType>
    [[nodiscard]] inline auto normal_forms_from_word_graph(
        KnuthBendix<RewritingSystem, ReductionOrder>& kb,
        WordGraph<WordType>&                          wg) {
      Paths paths(wg);
      paths.source(0);
      if (!kb.presentation().contains_empty_word()) {
        paths.next();
      }
      return paths;
    }
  }  // namespace
  congruence_kind constexpr twosided = congruence_kind::twosided;

  using literals::operator""_w;

  using LenLexTrie = detail::RewritingSystemTrie<ShortLexCompare>;
  using LenLexSet  = detail::RewritingSystemSet<ShortLexCompare>;
  using RPOTrie    = detail::RewritingSystemTrie<RecursivePathCompare>;
  using RPOSet     = detail::RewritingSystemSet<RecursivePathCompare>;

// TODO update to use RPO also
#define REWRITING_SYSTEM_TYPES LenLexTrie, LenLexSet

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "014",
                                   "redundant_rule (word_type)",
                                   "[quick][knuth-bendix]",
                                   REWRITING_SYSTEM_TYPES) {
    using literals::operator""_w;

    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.alphabet(3);
    presentation::add_rule(p, 0_w, 011_w);
    presentation::add_rule(p, 1_w, 100_w);
    presentation::add_rule(p, 2_w, 011010101000101010101_w);

    auto it = knuth_bendix::redundant_rule(p, std::chrono::milliseconds(10));
    REQUIRE(it == p.rules.cend());

    presentation::add_rule(p, 1_w, 100_w);
    it = knuth_bendix::redundant_rule(p, std::chrono::milliseconds(10));
    REQUIRE(it != p.rules.cend());
    REQUIRE(*it == 1_w);
    REQUIRE(*(it + 1) == 100_w);
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "020",
                                   "non-trivial classes x 4",
                                   "[quick][knuth-bendix]",
                                   REWRITING_SYSTEM_TYPES,
                                   RPOTrie) {
    using order = typename TestType::reduction_order;

    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.alphabet(4);
    presentation::add_rule_no_checks(p, 01_w, 10_w);
    presentation::add_rule_no_checks(p, 02_w, 20_w);
    presentation::add_rule_no_checks(p, 00_w, 0_w);
    presentation::add_rule_no_checks(p, 02_w, 0_w);
    presentation::add_rule_no_checks(p, 20_w, 0_w);
    presentation::add_rule_no_checks(p, 12_w, 21_w);
    presentation::add_rule_no_checks(p, 111_w, 1_w);
    presentation::add_rule_no_checks(p, 12_w, 1_w);
    presentation::add_rule_no_checks(p, 21_w, 1_w);
    presentation::add_rule_no_checks(p, 03_w, 0_w);
    presentation::add_rule_no_checks(p, 30_w, 0_w);
    presentation::add_rule_no_checks(p, 13_w, 1_w);
    presentation::add_rule_no_checks(p, 31_w, 1_w);
    presentation::add_rule_no_checks(p, 23_w, 2_w);
    presentation::add_rule_no_checks(p, 32_w, 2_w);

    KnuthBendix<word_type, TestType> kb1(twosided, p);

    presentation::add_rule_no_checks(p, 0_w, 1_w);

    KnuthBendix<word_type, TestType> kb2(twosided, p);
    if constexpr (std::is_same_v<order, ShortLexCompare>) {
      REQUIRE(knuth_bendix::non_trivial_classes(kb1, kb2)
              == std::vector<std::vector<word_type>>(
                  {{1_w, 01_w, 11_w, 011_w, 0_w}}));
    } else {
      REQUIRE(knuth_bendix::non_trivial_classes(kb1, kb2)
              == std::vector<std::vector<word_type>>(
                  {{1_w, 10_w, 11_w, 110_w, 0_w}}));
    }
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "021",
                                   "non-triv. cong. on infinite fp semigp",
                                   "[quick][knuth-bendix][no-valgrind]",
                                   REWRITING_SYSTEM_TYPES) {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.alphabet(5);
    presentation::add_rule_no_checks(p, 01_w, 0_w);
    presentation::add_rule_no_checks(p, 10_w, 0_w);
    presentation::add_rule_no_checks(p, 02_w, 0_w);
    presentation::add_rule_no_checks(p, 20_w, 0_w);
    presentation::add_rule_no_checks(p, 03_w, 0_w);
    presentation::add_rule_no_checks(p, 30_w, 0_w);
    presentation::add_rule_no_checks(p, 00_w, 0_w);
    presentation::add_rule_no_checks(p, 11_w, 0_w);
    presentation::add_rule_no_checks(p, 22_w, 0_w);
    presentation::add_rule_no_checks(p, 33_w, 0_w);
    presentation::add_rule_no_checks(p, 12_w, 0_w);
    presentation::add_rule_no_checks(p, 21_w, 0_w);
    presentation::add_rule_no_checks(p, 13_w, 0_w);
    presentation::add_rule_no_checks(p, 31_w, 0_w);
    presentation::add_rule_no_checks(p, 23_w, 0_w);
    presentation::add_rule_no_checks(p, 32_w, 0_w);
    presentation::add_rule_no_checks(p, 40_w, 0_w);
    presentation::add_rule_no_checks(p, 41_w, 1_w);
    presentation::add_rule_no_checks(p, 42_w, 2_w);
    presentation::add_rule_no_checks(p, 43_w, 3_w);
    presentation::add_rule_no_checks(p, 04_w, 0_w);
    presentation::add_rule_no_checks(p, 14_w, 1_w);
    presentation::add_rule_no_checks(p, 24_w, 2_w);
    presentation::add_rule_no_checks(p, 34_w, 3_w);

    KnuthBendix<word_type, TestType> kb1(twosided, p);

    WordGraph test_wg1 = v4::make<WordGraph<size_t>>(
        6,
        {{1, 2, 3, 4, 5},
         {},
         {},
         {},
         {},
         {UNDEFINED, UNDEFINED, UNDEFINED, UNDEFINED, 5}});
    REQUIRE(kb1.number_of_classes() == POSITIVE_INFINITY);

    REQUIRE(
        equal((knuth_bendix::normal_forms(kb1) | rx::take(1000)),
              (normal_forms_from_word_graph(kb1, test_wg1) | rx::take(1000))));

    presentation::add_rule_no_checks(p, 1_w, 2_w);
    KnuthBendix<word_type, TestType> kb2(twosided, p);

    WordGraph test_wg2 = v4::make<WordGraph<size_t>>(
        5,
        {{1, 2, UNDEFINED, 3, 4},
         {},
         {},
         {},
         {UNDEFINED, UNDEFINED, UNDEFINED, UNDEFINED, 4}});

    REQUIRE(kb1.number_of_classes() == POSITIVE_INFINITY);
    REQUIRE(
        equal((knuth_bendix::normal_forms(kb2) | rx::take(1000)),
              (normal_forms_from_word_graph(kb2, test_wg2) | rx::take(1000))));

    REQUIRE(knuth_bendix::contains(kb2, 1_w, 2_w));

    auto ntc = knuth_bendix::non_trivial_classes(kb1, kb2);
    REQUIRE(ntc.size() == 1);
    REQUIRE(ntc[0].size() == 2);
    REQUIRE(ntc == decltype(ntc)({{{2}, {1}}}));
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "022",
                                   "non-triv. cong. on infinite fp semigroup",
                                   "[quick][kbp]",
                                   REWRITING_SYSTEM_TYPES) {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.alphabet(5);
    presentation::add_rule_no_checks(p, 01_w, 0_w);
    presentation::add_rule_no_checks(p, 10_w, 0_w);
    presentation::add_rule_no_checks(p, 02_w, 0_w);
    presentation::add_rule_no_checks(p, 20_w, 0_w);
    presentation::add_rule_no_checks(p, 03_w, 0_w);
    presentation::add_rule_no_checks(p, 30_w, 0_w);
    presentation::add_rule_no_checks(p, 00_w, 0_w);
    presentation::add_rule_no_checks(p, 11_w, 0_w);
    presentation::add_rule_no_checks(p, 22_w, 0_w);
    presentation::add_rule_no_checks(p, 33_w, 0_w);
    presentation::add_rule_no_checks(p, 12_w, 0_w);
    presentation::add_rule_no_checks(p, 21_w, 0_w);
    presentation::add_rule_no_checks(p, 13_w, 0_w);
    presentation::add_rule_no_checks(p, 31_w, 0_w);
    presentation::add_rule_no_checks(p, 23_w, 0_w);
    presentation::add_rule_no_checks(p, 32_w, 0_w);
    presentation::add_rule_no_checks(p, 40_w, 0_w);
    presentation::add_rule_no_checks(p, 41_w, 2_w);
    presentation::add_rule_no_checks(p, 42_w, 3_w);
    presentation::add_rule_no_checks(p, 43_w, 1_w);
    presentation::add_rule_no_checks(p, 04_w, 0_w);
    presentation::add_rule_no_checks(p, 14_w, 2_w);
    presentation::add_rule_no_checks(p, 24_w, 3_w);
    presentation::add_rule_no_checks(p, 34_w, 1_w);

    KnuthBendix<word_type, TestType> kb1(twosided, p);

    presentation::add_rule_no_checks(p, 2_w, 3_w);

    KnuthBendix<word_type, TestType> kb2(twosided, p);
    auto ntc = knuth_bendix::non_trivial_classes(kb1, kb2);
    REQUIRE(ntc.size() == 1);
    REQUIRE(ntc[0].size() == 3);
    REQUIRE(ntc == decltype(ntc)({{{2}, {3}, {1}}}));
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "023",
                                   "triv. cong. on finite fp semigp",
                                   "[quick][kbp]",
                                   REWRITING_SYSTEM_TYPES) {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.alphabet(2);
    presentation::add_rule_no_checks(p, 001_w, 00_w);
    presentation::add_rule_no_checks(p, 0000_w, 00_w);
    presentation::add_rule_no_checks(p, 0110_w, 00_w);
    presentation::add_rule_no_checks(p, 0111_w, 000_w);
    presentation::add_rule_no_checks(p, 1110_w, 110_w);
    presentation::add_rule_no_checks(p, 1111_w, 111_w);
    presentation::add_rule_no_checks(p, 01000_w, 0101_w);
    presentation::add_rule_no_checks(p, 01010_w, 0100_w);
    presentation::add_rule_no_checks(p, 01011_w, 0101_w);

    KnuthBendix<word_type, TestType> kb1(twosided, p);
    KnuthBendix<word_type, TestType> kb2(twosided, p);

    REQUIRE(!p.contains_empty_word());
    REQUIRE(kb1.number_of_classes() == 27);
    REQUIRE(kb2.number_of_classes() == 27);
    auto ntc = knuth_bendix::non_trivial_classes(kb1, kb2);
    REQUIRE(ntc.empty());
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "024",
                                   "universal cong. on finite fp semigroup",
                                   "[quick][kbp]",
                                   REWRITING_SYSTEM_TYPES) {
    using order = typename TestType::reduction_order;
    auto rg     = ReportGuard(false);

    Presentation<word_type> p;
    p.alphabet(2);
    presentation::add_rule_no_checks(p, 001_w, 00_w);
    presentation::add_rule_no_checks(p, 0000_w, 00_w);
    presentation::add_rule_no_checks(p, 0110_w, 00_w);
    presentation::add_rule_no_checks(p, 0111_w, 000_w);
    presentation::add_rule_no_checks(p, 1110_w, 110_w);
    presentation::add_rule_no_checks(p, 1111_w, 111_w);
    presentation::add_rule_no_checks(p, 01000_w, 0101_w);
    presentation::add_rule_no_checks(p, 01010_w, 0100_w);
    presentation::add_rule_no_checks(p, 01011_w, 0101_w);

    KnuthBendix<word_type, TestType> kb1(twosided, p);

    presentation::add_rule_no_checks(p, 0_w, 1_w);
    presentation::add_rule_no_checks(p, 00_w, 0_w);

    KnuthBendix<word_type, TestType> kb2(twosided, p);

    REQUIRE(kb2.number_of_classes() == 1);

    auto ntc = knuth_bendix::non_trivial_classes(kb1, kb2);

    REQUIRE(ntc.size() == 1);
    REQUIRE(ntc[0].size() == 27);
    std::vector<word_type> expected;
    if constexpr (std::is_same_v<order, ShortLexCompare>) {
      expected
          = {0_w,     1_w,     00_w,    01_w,    10_w,     11_w,    000_w,
             100_w,   010_w,   101_w,   011_w,   110_w,    111_w,   1000_w,
             0100_w,  1100_w,  1010_w,  0101_w,  1101_w,   1011_w,  11000_w,
             10100_w, 11010_w, 10101_w, 11011_w, 110100_w, 110101_w};
    } else {
      expected
          = {0_w,     00_w,     000_w,    01_w,      010_w,   0100_w, 01000_w,
             011_w,   1_w,      10_w,     100_w,     1000_w,  101_w,  1010_w,
             10100_w, 101000_w, 1011_w,   11_w,      110_w,   1100_w, 11000_w,
             1101_w,  11010_w,  110100_w, 1101000_w, 11011_w, 111_w};
    }

    std::sort(expected.begin(), expected.end());
    std::sort(ntc[0].begin(), ntc[0].end());
    REQUIRE(ntc[0] == expected);
    REQUIRE(ntc[0].size() == 27);
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "025",
                                   "finite fp semigroup, size 16",
                                   "[quick][kbp]",
                                   REWRITING_SYSTEM_TYPES) {
    auto rg = ReportGuard(false);

    Presentation<word_type> p;
    p.alphabet(11);
    presentation::add_rule_no_checks(p, {2}, {1});
    presentation::add_rule_no_checks(p, {4}, {3});
    presentation::add_rule_no_checks(p, {5}, {0});
    presentation::add_rule_no_checks(p, {6}, {3});
    presentation::add_rule_no_checks(p, {7}, {1});
    presentation::add_rule_no_checks(p, {8}, {3});
    presentation::add_rule_no_checks(p, {9}, {3});
    presentation::add_rule_no_checks(p, {10}, {0});
    presentation::add_rule_no_checks(p, {0, 2}, {0, 1});
    presentation::add_rule_no_checks(p, {0, 4}, {0, 3});
    presentation::add_rule_no_checks(p, {0, 5}, {0, 0});
    presentation::add_rule_no_checks(p, {0, 6}, {0, 3});
    presentation::add_rule_no_checks(p, {0, 7}, {0, 1});
    presentation::add_rule_no_checks(p, {0, 8}, {0, 3});
    presentation::add_rule_no_checks(p, {0, 9}, {0, 3});

    presentation::add_rule_no_checks(p, {0, 10}, {0, 0});
    presentation::add_rule_no_checks(p, {1, 1}, {1});
    presentation::add_rule_no_checks(p, {1, 2}, {1});
    presentation::add_rule_no_checks(p, {1, 4}, {1, 3});
    presentation::add_rule_no_checks(p, {1, 5}, {1, 0});
    presentation::add_rule_no_checks(p, {1, 6}, {1, 3});
    presentation::add_rule_no_checks(p, {1, 7}, {1});
    presentation::add_rule_no_checks(p, {1, 8}, {1, 3});
    presentation::add_rule_no_checks(p, {1, 9}, {1, 3});
    presentation::add_rule_no_checks(p, {1, 10}, {1, 0});
    presentation::add_rule_no_checks(p, {3, 1}, {3});
    presentation::add_rule_no_checks(p, {3, 2}, {3});
    presentation::add_rule_no_checks(p, {3, 3}, {3});
    presentation::add_rule_no_checks(p, {3, 4}, {3});
    presentation::add_rule_no_checks(p, {3, 5}, {3, 0});
    presentation::add_rule_no_checks(p, {3, 6}, {3});
    presentation::add_rule_no_checks(p, {3, 7}, {3});
    presentation::add_rule_no_checks(p, {3, 8}, {3});
    presentation::add_rule_no_checks(p, {3, 9}, {3});
    presentation::add_rule_no_checks(p, {3, 10}, {3, 0});
    presentation::add_rule_no_checks(p, {0, 0, 0}, {0});
    presentation::add_rule_no_checks(p, {0, 0, 1}, {1});
    presentation::add_rule_no_checks(p, {0, 0, 3}, {3});
    presentation::add_rule_no_checks(p, {0, 1, 3}, {1, 3});
    presentation::add_rule_no_checks(p, {1, 0, 0}, {1});
    presentation::add_rule_no_checks(p, {1, 0, 3}, {0, 3});
    presentation::add_rule_no_checks(p, {3, 0, 0}, {3});
    presentation::add_rule_no_checks(p, {0, 1, 0, 1}, {1, 0, 1});
    presentation::add_rule_no_checks(p, {0, 3, 0, 3}, {3, 0, 3});
    presentation::add_rule_no_checks(p, {1, 0, 1, 0}, {1, 0, 1});
    presentation::add_rule_no_checks(p, {1, 3, 0, 1}, {1, 0, 1});
    presentation::add_rule_no_checks(p, {1, 3, 0, 3}, {3, 0, 3});
    presentation::add_rule_no_checks(p, {3, 0, 1, 0}, {3, 0, 1});
    presentation::add_rule_no_checks(p, {3, 0, 3, 0}, {3, 0, 3});

    KnuthBendix<word_type, TestType> kb1(twosided, p);
    REQUIRE(kb1.gilman_graph().number_of_nodes() == 16);

    WordGraph test_wg1
        = v4::make<WordGraph<size_t>>(16,
                                      {{3,
                                        1,
                                        UNDEFINED,
                                        2,
                                        UNDEFINED,
                                        UNDEFINED,
                                        UNDEFINED,
                                        UNDEFINED,
                                        UNDEFINED,
                                        UNDEFINED,
                                        UNDEFINED},
                                       {6, UNDEFINED, UNDEFINED, 12},
                                       {7, UNDEFINED},
                                       {4, 5, UNDEFINED, 9},
                                       {},
                                       {8},
                                       {UNDEFINED, 11},
                                       {UNDEFINED, 14, UNDEFINED, 15},
                                       {},
                                       {10},
                                       {UNDEFINED, 14},
                                       {},
                                       {13},
                                       {UNDEFINED}});
    REQUIRE(equal(knuth_bendix::normal_forms(kb1),
                  normal_forms_from_word_graph(kb1, test_wg1)));

    presentation::add_rule_no_checks(p, {1}, {3});
    KnuthBendix<word_type, TestType> kb2(twosided, p);

    WordGraph test_wg2 = v4::make<WordGraph<size_t>>(4,
                                                     {{2,
                                                       1,
                                                       UNDEFINED,
                                                       UNDEFINED,
                                                       UNDEFINED,
                                                       UNDEFINED,
                                                       UNDEFINED,
                                                       UNDEFINED,
                                                       UNDEFINED,
                                                       UNDEFINED,
                                                       UNDEFINED},
                                                      {},
                                                      {3}});

    REQUIRE(equal(knuth_bendix::normal_forms(kb2),
                  normal_forms_from_word_graph(kb2, test_wg2)));

    auto ntc = knuth_bendix::non_trivial_classes(kb1, kb2);

    std::vector expected = {1_w,
                            3_w,
                            01_w,
                            03_w,
                            10_w,
                            30_w,
                            13_w,
                            010_w,
                            030_w,
                            101_w,
                            301_w,
                            303_w,
                            130_w,
                            0301_w};
    std::sort(expected.begin(), expected.end());
    std::sort(ntc[0].begin(), ntc[0].end());
    REQUIRE(ntc[0] == expected);
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "026",
                                   "non_trivial_classes exceptions",
                                   "[quick][kbp]",
                                   REWRITING_SYSTEM_TYPES) {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.alphabet(1);
    KnuthBendix<word_type, TestType> kbp(twosided, p);

    {
      Presentation<word_type> q;
      q.alphabet(2);
      KnuthBendix<word_type, TestType> kbq(twosided, q);
      REQUIRE_THROWS_AS(knuth_bendix::non_trivial_classes(kbp, kbq),
                        LibsemigroupsException);
      //      REQUIRE(kbq.number_of_inactive_rules() == 0);
    }
    {
      presentation::add_rule_no_checks(p, 0000_w, 00_w);
      kbp.init(twosided, p);

      Presentation<word_type> q;
      q.alphabet(1);
      presentation::add_rule_no_checks(q, 00_w, 0_w);

      // auto kbq = knuth_bendix::make<TestType>(twosided, q);

      KnuthBendix<word_type, TestType> kbq(twosided, q);
      REQUIRE_THROWS_AS(knuth_bendix::non_trivial_classes(kbq, kbp),
                        LibsemigroupsException);
    }
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "060",
                                   "Chinese monoid",
                                   "[knuth-bendix][quick][no-valgrind]",
                                   REWRITING_SYSTEM_TYPES) {
    auto rg = ReportGuard(false);
    // fmt::print(bg(fmt::color::white) | fg(fmt::color::black),
    //            "062",": Chinese monoid STARTING . . .\n");

    std::array<uint64_t, 11> const num
        = {0, 0, 22, 71, 181, 391, 750, 1'317, 2'161, 3'361, 5'006};

    KnuthBendix<word_type, TestType> kb;
    for (size_t n = 2; n < 11; ++n) {
      auto p = presentation::examples::chinese_monoid(n);
      p.contains_empty_word(true);
      kb.init(twosided, p);
      kb.run();
      REQUIRE(knuth_bendix::normal_forms(kb).min(0).max(4).count() == num[n]);
    }
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "063",
                                   "sigma sylvester monoid",
                                   "[knuth-bendix][quick][no-valgrind]",
                                   REWRITING_SYSTEM_TYPES) {
    using namespace literals;
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.alphabet(4);
    p.contains_empty_word(true);
    presentation::add_rule(p, 00_w, 0_w);
    presentation::add_rule(p, 11_w, 1_w);
    presentation::add_rule(p, 22_w, 2_w);
    presentation::add_rule(p, 33_w, 3_w);
    presentation::add_rule(p, 010_w, 01_w);
    presentation::add_rule(p, 020_w, 02_w);
    presentation::add_rule(p, 030_w, 03_w);
    presentation::add_rule(p, 121_w, 12_w);
    presentation::add_rule(p, 131_w, 13_w);
    presentation::add_rule(p, 232_w, 23_w);
    presentation::add_rule(p, 0120_w, 012_w);
    presentation::add_rule(p, 0130_w, 013_w);
    presentation::add_rule(p, 0210_w, 021_w);
    presentation::add_rule(p, 0230_w, 023_w);
    presentation::add_rule(p, 0310_w, 031_w);
    presentation::add_rule(p, 0320_w, 032_w);
    presentation::add_rule(p, 1202_w, 120_w);
    presentation::add_rule(p, 1231_w, 123_w);
    presentation::add_rule(p, 1303_w, 130_w);
    presentation::add_rule(p, 1321_w, 132_w);
    presentation::add_rule(p, 2303_w, 230_w);
    presentation::add_rule(p, 2313_w, 231_w);
    presentation::add_rule(p, 01230_w, 0123_w);
    presentation::add_rule(p, 01320_w, 0132_w);
    presentation::add_rule(p, 02120_w, 0212_w);
    presentation::add_rule(p, 02130_w, 0213_w);
    presentation::add_rule(p, 02310_w, 0231_w);
    presentation::add_rule(p, 03120_w, 0312_w);
    presentation::add_rule(p, 03130_w, 0313_w);
    presentation::add_rule(p, 03210_w, 0321_w);
    presentation::add_rule(p, 03230_w, 0323_w);
    presentation::add_rule(p, 10212_w, 1021_w);
    presentation::add_rule(p, 10313_w, 1031_w);
    presentation::add_rule(p, 12012_w, 1201_w);
    presentation::add_rule(p, 12032_w, 1203_w);
    presentation::add_rule(p, 12302_w, 1230_w);
    presentation::add_rule(p, 13013_w, 1301_w);
    presentation::add_rule(p, 13202_w, 1320_w);
    presentation::add_rule(p, 13231_w, 1323_w);
    presentation::add_rule(p, 20313_w, 2031_w);
    presentation::add_rule(p, 20323_w, 2032_w);
    presentation::add_rule(p, 21323_w, 2132_w);
    presentation::add_rule(p, 23013_w, 2301_w);
    presentation::add_rule(p, 23023_w, 2302_w);
    presentation::add_rule(p, 23103_w, 2310_w);
    presentation::add_rule(p, 23123_w, 2312_w);
    presentation::add_rule(p, 013230_w, 01323_w);
    presentation::add_rule(p, 021230_w, 02123_w);
    presentation::add_rule(p, 021320_w, 02132_w);
    presentation::add_rule(p, 023120_w, 02312_w);
    presentation::add_rule(p, 031230_w, 03123_w);
    presentation::add_rule(p, 031320_w, 03132_w);
    presentation::add_rule(p, 032120_w, 03212_w);
    presentation::add_rule(p, 032130_w, 03213_w);
    presentation::add_rule(p, 032310_w, 03231_w);
    presentation::add_rule(p, 102132_w, 10213_w);
    presentation::add_rule(p, 102312_w, 10231_w);
    presentation::add_rule(p, 103212_w, 10321_w);
    presentation::add_rule(p, 120132_w, 12013_w);
    presentation::add_rule(p, 120312_w, 12031_w);
    presentation::add_rule(p, 123012_w, 12301_w);
    presentation::add_rule(p, 130212_w, 13021_w);
    presentation::add_rule(p, 132012_w, 13201_w);
    presentation::add_rule(p, 132032_w, 13203_w);
    presentation::add_rule(p, 132302_w, 13230_w);
    presentation::add_rule(p, 201323_w, 20132_w);
    presentation::add_rule(p, 203123_w, 20312_w);
    presentation::add_rule(p, 203213_w, 20321_w);
    presentation::add_rule(p, 210323_w, 21032_w);
    presentation::add_rule(p, 213023_w, 21302_w);
    presentation::add_rule(p, 213203_w, 21320_w);
    presentation::add_rule(p, 230123_w, 23012_w);
    presentation::add_rule(p, 230213_w, 23021_w);
    presentation::add_rule(p, 231013_w, 23101_w);
    presentation::add_rule(p, 231023_w, 23102_w);
    presentation::add_rule(p, 231203_w, 23120_w);
    presentation::add_rule(p, 0313230_w, 031323_w);
    presentation::add_rule(p, 0321230_w, 032123_w);
    presentation::add_rule(p, 0321320_w, 032132_w);
    presentation::add_rule(p, 0323120_w, 032312_w);
    presentation::add_rule(p, 1032132_w, 103213_w);
    presentation::add_rule(p, 1032312_w, 103231_w);
    presentation::add_rule(p, 1302132_w, 130213_w);
    presentation::add_rule(p, 1302312_w, 130231_w);
    presentation::add_rule(p, 1320132_w, 132013_w);
    presentation::add_rule(p, 1320312_w, 132031_w);
    presentation::add_rule(p, 1323012_w, 132301_w);
    presentation::add_rule(p, 2032123_w, 203212_w);
    presentation::add_rule(p, 2101323_w, 210132_w);
    presentation::add_rule(p, 2103123_w, 210312_w);
    presentation::add_rule(p, 2103213_w, 210321_w);
    presentation::add_rule(p, 2130123_w, 213012_w);
    presentation::add_rule(p, 2130213_w, 213021_w);
    presentation::add_rule(p, 2132013_w, 213201_w);
    presentation::add_rule(p, 2302123_w, 230212_w);
    presentation::add_rule(p, 2310123_w, 231012_w);
    presentation::add_rule(p, 2310213_w, 231021_w);
    presentation::add_rule(p, 2312013_w, 231201_w);
    REQUIRE(p.rules.size() == 196);
    // {
    //   ReportGuard rg(false);
    //   auto it = knuth_bendix::redundant_rule(p,
    //   std::chrono::milliseconds(100)); while (it != p.rules.cend()) {
    //     p.rules.erase(it, it + 2);
    //     it = knuth_bendix::redundant_rule(p, std::chrono::milliseconds(100));
    //   }

    //   REQUIRE(p.rules.size() == 58);
    //   REQUIRE(
    //       p.rules
    //       == std::vector(
    //           {00_w,      0_w,      11_w,      1_w,      22_w,      2_w,
    //            33_w,      3_w,      010_w,     01_w,     020_w,     02_w,
    //            030_w,     03_w,     121_w,     12_w,     131_w,     13_w,
    //            232_w,     23_w,     1202_w,    120_w,    1303_w,    130_w,
    //            2303_w,    230_w,    2313_w,    231_w,    10212_w,   1021_w,
    //            10313_w,   1031_w,   20313_w,   2031_w,   20323_w,   2032_w,
    //            21323_w,   2132_w,   102312_w,  10231_w,  103212_w,  10321_w,
    //            201323_w,  20132_w,  203123_w,  20312_w,  210323_w,  21032_w,
    //            213023_w,  21302_w,  1032312_w, 103231_w, 2101323_w, 210132_w,
    //            2103123_w, 210312_w, 2130123_w, 213012_w}));
    // }
    KnuthBendix<word_type, TestType> kb(twosided, p);
    kb.run();
    REQUIRE(kb.number_of_classes() == 312);
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "064",
                                   "Reinis MFE",
                                   "[knuth-bendix][quick]",
                                   REWRITING_SYSTEM_TYPES) {
    auto            rg = ReportGuard(false);
    using literals::operator""_w;

    Presentation<word_type> p;
    p.alphabet(2);
    presentation::add_rule(p, 000_w, 11_w);
    presentation::add_rule(p, 001_w, 10_w);
    KnuthBendix<word_type, TestType> kb(congruence_kind::twosided, p);

    REQUIRE(knuth_bendix::contains(kb, 000_w, 11_w));
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "061",
                                   "hypostylic",
                                   "[knuth-bendix][quick]",
                                   REWRITING_SYSTEM_TYPES) {
    using namespace literals;
    using words::operator+;

    auto   rg = ReportGuard(false);
    size_t n  = 2;
    auto   p  = presentation::examples::hypo_plactic_monoid(n);
    p.contains_empty_word(true);
    presentation::add_idempotent_rules_no_checks(
        p, (rx::seq<size_t>() | rx::take(n) | rx::to_vector()));
    KnuthBendix<word_type, TestType> kb(congruence_kind::twosided, p);
    kb.run();
    REQUIRE(kb.presentation().alphabet() == word_type({0, 1}));
    REQUIRE((knuth_bendix::normal_forms(kb) | rx::filter([&kb](auto const& w) {
               auto ww = w;
               ww.insert(ww.begin(), w.begin(), w.end());
               return knuth_bendix::reduce(kb, ww) == w;
             })
             | rx::to_vector())
            == std::vector({{}, 0_w, 1_w, 10_w}));
    REQUIRE((kb.active_rules() | rx::sort(weird_cmp()) | rx::to_vector())
            == std::vector<std::pair<word_type, word_type>>(
                {{00_w, 0_w}, {11_w, 1_w}, {010_w, 10_w}, {101_w, 10_w}}));
    auto expected = std::vector({{}, 0_w, 01_w, 1_w, 10_w});
    auto found    = kb.gilman_graph_node_labels();
    std::sort(found.begin(), found.end());

    REQUIRE(found == expected);
    // The gilman_graph generated is isomorphic to the word_graph given, but not
    // identical. Since the normal forms are correct (see above) the below check
    // is omitted.
    // REQUIRE(kb.gilman_graph()
    //         == make<WordGraph<size_t>>(5, {{1, 3}, {UNDEFINED, 2}, {},
    //         {4}}));
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "129",
                                   "Presentation<word_type>",
                                   "[quick][knuth-bendix]",
                                   REWRITING_SYSTEM_TYPES) {
    auto rg = ReportGuard(false);

    Presentation<word_type> p;
    p.alphabet(2);
    presentation::add_rule(p, 000_w, 0_w);
    presentation::add_rule(p, 0_w, 11_w);

    KnuthBendix<word_type, TestType> kb(twosided, p);

    REQUIRE(!kb.finished());
    REQUIRE(kb.number_of_classes() == 5);
    REQUIRE(kb.finished());

    REQUIRE(knuth_bendix::reduce(kb, 001_w) == 001_w);
    REQUIRE(knuth_bendix::reduce(kb, 00001_w) == 001_w);
    REQUIRE(knuth_bendix::reduce(kb, 011001_w) == 001_w);
    REQUIRE(!knuth_bendix::contains(kb, 000_w, 1_w));
    REQUIRE(!knuth_bendix::contains(kb, 0000_w, 000_w));
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "130",
                                   "free semigroup congruence (6 classes)",
                                   "[quick][knuth-bendix]",
                                   REWRITING_SYSTEM_TYPES) {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.alphabet(5);
    presentation::add_rule(p, 00_w, 0_w);
    presentation::add_rule(p, 01_w, 1_w);
    presentation::add_rule(p, 10_w, 1_w);
    presentation::add_rule(p, 02_w, 2_w);
    presentation::add_rule(p, 20_w, 2_w);
    presentation::add_rule(p, 03_w, 3_w);
    presentation::add_rule(p, 30_w, 3_w);
    presentation::add_rule(p, 04_w, 4_w);
    presentation::add_rule(p, 40_w, 4_w);
    presentation::add_rule(p, 12_w, 0_w);
    presentation::add_rule(p, 21_w, 0_w);
    presentation::add_rule(p, 34_w, 0_w);
    presentation::add_rule(p, 43_w, 0_w);
    presentation::add_rule(p, 22_w, 0_w);
    presentation::add_rule(p, 14233_w, 0_w);
    presentation::add_rule(p, 444_w, 0_w);

    KnuthBendix<word_type, TestType> kb(twosided, p);

    REQUIRE(kb.number_of_classes() == 6);
    REQUIRE(knuth_bendix::contains(kb, 1_w, 2_w));
    // TODO(0) try constructing a KnuthBendix object from a presentation with
    // more than 255 generators
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "131",
                                   "free semigroup congruence (16 classes)",
                                   "[quick][knuth-bendix]",
                                   REWRITING_SYSTEM_TYPES) {
    auto rg = ReportGuard(false);

    Presentation<word_type> p;
    p.alphabet(4);
    presentation::add_rule(p, 3_w, 2_w);
    presentation::add_rule(p, 03_w, 02_w);
    presentation::add_rule(p, 11_w, 1_w);
    presentation::add_rule(p, 13_w, 12_w);
    presentation::add_rule(p, 21_w, 2_w);
    presentation::add_rule(p, 22_w, 2_w);
    presentation::add_rule(p, 23_w, 2_w);
    presentation::add_rule(p, 000_w, 0_w);
    presentation::add_rule(p, 001_w, 1_w);
    presentation::add_rule(p, 002_w, 2_w);
    presentation::add_rule(p, 012_w, 12_w);
    presentation::add_rule(p, 100_w, 1_w);
    presentation::add_rule(p, 102_w, 02_w);
    presentation::add_rule(p, 200_w, 2_w);
    presentation::add_rule(p, 0101_w, 101_w);
    presentation::add_rule(p, 0202_w, 202_w);
    presentation::add_rule(p, 1010_w, 101_w);
    presentation::add_rule(p, 1201_w, 101_w);
    presentation::add_rule(p, 1202_w, 202_w);
    presentation::add_rule(p, 2010_w, 201_w);
    presentation::add_rule(p, 2020_w, 202_w);

    KnuthBendix<word_type, TestType> kb(twosided, p);

    REQUIRE(kb.number_of_classes() == 16);
    REQUIRE(kb.rewriting_system().number_of_rules() == 18);
    REQUIRE(knuth_bendix::contains(kb, 2_w, 3_w));
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "132",
                                   "free semigroup congruence x 2",
                                   "[quick][knuth-bendix]",
                                   REWRITING_SYSTEM_TYPES) {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.alphabet(11);
    p.rules
        = {{2},          {1},          {4},          {3},          {5},
           {0},          {6},          {3},          {7},          {1},
           {8},          {3},          {9},          {3},          {10},
           {0},          {0, 2},       {0, 1},       {0, 4},       {0, 3},
           {0, 5},       {0, 0},       {0, 6},       {0, 3},       {0, 7},
           {0, 1},       {0, 8},       {0, 3},       {0, 9},       {0, 3},
           {0, 10},      {0, 0},       {1, 1},       {1},          {1, 2},
           {1},          {1, 4},       {1, 3},       {1, 5},       {1, 0},
           {1, 6},       {1, 3},       {1, 7},       {1},          {1, 8},
           {1, 3},       {1, 9},       {1, 3},       {1, 10},      {1, 0},
           {3, 1},       {3},          {3, 2},       {3},          {3, 3},
           {3},          {3, 4},       {3},          {3, 5},       {3, 0},
           {3, 6},       {3},          {3, 7},       {3},          {3, 8},
           {3},          {3, 9},       {3},          {3, 10},      {3, 0},
           {0, 0, 0},    {0},          {0, 0, 1},    {1},          {0, 0, 3},
           {3},          {0, 1, 3},    {1, 3},       {1, 0, 0},    {1},
           {1, 0, 3},    {0, 3},       {3, 0, 0},    {3},          {0, 1, 0, 1},
           {1, 0, 1},    {0, 3, 0, 3}, {3, 0, 3},    {1, 0, 1, 0}, {1, 0, 1},
           {1, 3, 0, 1}, {1, 0, 1},    {1, 3, 0, 3}, {3, 0, 3},    {3, 0, 1, 0},
           {3, 0, 1},    {3, 0, 3, 0}, {3, 0, 3}};

    KnuthBendix<word_type, TestType> kb(twosided, p);
    REQUIRE(kb.number_of_classes() == 16);
    REQUIRE(knuth_bendix::contains(kb, {0}, {5}));
    REQUIRE(knuth_bendix::contains(kb, {0}, {5}));
    REQUIRE(knuth_bendix::contains(kb, {0}, {10}));
    REQUIRE(knuth_bendix::contains(kb, {1}, {2}));
    REQUIRE(knuth_bendix::contains(kb, {1}, {7}));
    REQUIRE(knuth_bendix::contains(kb, {3}, {4}));
    REQUIRE(knuth_bendix::contains(kb, {3}, {6}));
    REQUIRE(knuth_bendix::contains(kb, {3}, {8}));
    REQUIRE(knuth_bendix::contains(kb, {3}, {9}));
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "133",
                                   "free semigroup congruence (240 classes)",
                                   "[no-valgrind][quick][knuth-bendix]",
                                   REWRITING_SYSTEM_TYPES) {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.alphabet(2);
    presentation::add_rule(p, 000_w, 0_w);
    presentation::add_rule(p, 1111_w, 1_w);
    presentation::add_rule(p, 01110_w, 00_w);
    presentation::add_rule(p, 1001_w, 11_w);
    presentation::add_rule(p, 001010101010_w, 00_w);

    KnuthBendix<word_type, TestType> kb(twosided, p);
    REQUIRE(kb.number_of_classes() == 240);
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "134",
                                   "free semigroup congruence x 2",
                                   "[no-valgrind][quick][knuth-bendix]",
                                   REWRITING_SYSTEM_TYPES) {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.alphabet(2);
    presentation::add_rule(p, 000_w, 0_w);
    presentation::add_rule(p, 1111_w, 1_w);
    presentation::add_rule(p, 01110_w, 00_w);
    presentation::add_rule(p, 1001_w, 11_w);
    presentation::add_rule(p, 001010101010_w, 00_w);

    KnuthBendix<word_type, TestType> kb(twosided, p);
    REQUIRE_NOTHROW(to<FroidurePin>(kb));
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "135",
                                   "constructors",
                                   "[quick][knuth-bendix][no-valgrind]",
                                   REWRITING_SYSTEM_TYPES) {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.alphabet(2);
    presentation::add_rule(p, 000_w, 0_w);
    presentation::add_rule(p, 111111111_w, 1_w);
    presentation::add_rule(p, 011111011_w, 110_w);

    KnuthBendix<word_type, TestType> kb(twosided, p);
    REQUIRE(kb.number_of_classes() == 746);

    auto copy(kb);
    REQUIRE(copy.number_of_classes() == 746);
    REQUIRE(copy.presentation().alphabet().size() == 2);
    // the copy uses the "active rules" of kb, of which there are 105 since
    // knuth-bendix has already been run.
    REQUIRE(copy.rewriting_system().number_of_rules() == 105);
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "136",
                                   "number of classes when obv-inf",
                                   "[quick][knuth-bendix]",
                                   REWRITING_SYSTEM_TYPES) {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.alphabet(3);
    presentation::add_rule(p, 01_w, 10_w);
    presentation::add_rule(p, 02_w, 20_w);
    presentation::add_rule(p, 00_w, 0_w);
    presentation::add_rule(p, 02_w, 0_w);
    presentation::add_rule(p, 20_w, 0_w);
    presentation::add_rule(p, 11_w, 11_w);
    presentation::add_rule(p, 12_w, 21_w);
    presentation::add_rule(p, 111_w, 1_w);
    presentation::add_rule(p, 12_w, 1_w);
    presentation::add_rule(p, 21_w, 1_w);
    presentation::add_rule(p, 0_w, 1_w);

    KnuthBendix<word_type, TestType> kb(twosided, p);
    REQUIRE(is_obviously_infinite(kb));
    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "137",
                                   "Chinese monoid x 2",
                                   "[quick][knuth-bendix]",
                                   REWRITING_SYSTEM_TYPES) {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p  = presentation::examples::chinese_monoid(3);

    KnuthBendix<word_type, TestType> kb(twosided, p);
    REQUIRE(is_obviously_infinite(kb));
    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
    REQUIRE(kb.presentation().rules.size() / 2 == 8);
    auto nf = knuth_bendix::normal_forms(kb).min(1).max(9);
    REQUIRE(nf.count() == 1'175);
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "138",
                                   "partial_transformation_monoid(4)",
                                   "[quick][knuth-bendix][no-valgrind]",
                                   REWRITING_SYSTEM_TYPES) {
    auto rg = ReportGuard(false);

    size_t n = 4;
    auto   p = presentation::examples::partial_transformation_monoid_Shu60(n);

    KnuthBendix<word_type, TestType> kb(twosided, p);
    REQUIRE(!is_obviously_infinite(kb));
    REQUIRE(kb.number_of_classes() == 625);
    REQUIRE(to_human_readable_repr(kb)
            == "<confluent 2-sided KnuthBendix over <monoid presentation with "
               "5 letters, 260 rules, and length 4,131> with 0 gen. pairs, 362 "
               "rules>");
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "141",
                                   "constructors/init for finished x 2",
                                   "[quick][knuth-bendix]",
                                   REWRITING_SYSTEM_TYPES) {
    using literals::operator""_w;
    auto            rg = ReportGuard(false);

    Presentation<word_type> p1;
    p1.contains_empty_word(true);
    p1.alphabet(4);
    presentation::add_rule(p1, 01_w, {});
    presentation::add_rule(p1, 10_w, {});
    presentation::add_rule(p1, 23_w, {});
    presentation::add_rule(p1, 32_w, {});
    presentation::add_rule(p1, 20_w, 02_w);

    Presentation<word_type> p2;
    p2.contains_empty_word(true);
    p2.alphabet(2);
    presentation::add_rule(p2, 000_w, {});
    presentation::add_rule(p2, 111_w, {});
    presentation::add_rule(p2, 010101_w, {});

    KnuthBendix<word_type, TestType> kb1(twosided, p1);
    REQUIRE(!kb1.rewriting_system().confluent());
    REQUIRE(!kb1.finished());
    kb1.run();
    REQUIRE(kb1.rewriting_system().confluent());
    REQUIRE(kb1.rewriting_system().number_of_rules() == 8);

    kb1.init(twosided, p2);
    REQUIRE(!kb1.rewriting_system().confluent());
    REQUIRE(!kb1.finished());
    kb1.run();
    REQUIRE(kb1.finished());
    REQUIRE(kb1.rewriting_system().confluent());
    REQUIRE(kb1.rewriting_system().confluent_known());
    REQUIRE(kb1.rewriting_system().number_of_rules() == 4);

    kb1.init(twosided, p1);
    REQUIRE(!kb1.rewriting_system().confluent());
    REQUIRE(!kb1.finished());
    kb1.run();
    REQUIRE(kb1.finished());
    REQUIRE(kb1.rewriting_system().confluent());
    REQUIRE(kb1.rewriting_system().confluent_known());
    REQUIRE(kb1.rewriting_system().number_of_rules() == 8);

    KnuthBendix kb2(std::move(kb1));
    REQUIRE(kb2.rewriting_system().confluent());
    REQUIRE(kb2.rewriting_system().confluent_known());
    REQUIRE(kb2.finished());
    REQUIRE(kb2.rewriting_system().number_of_rules() == 8);

    kb1 = std::move(kb2);
    REQUIRE(kb1.rewriting_system().confluent());
    REQUIRE(kb1.rewriting_system().confluent_known());
    REQUIRE(kb1.finished());
    REQUIRE(kb1.rewriting_system().number_of_rules() == 8);

    kb1.init(twosided, std::move(p1));
    REQUIRE(!kb1.rewriting_system().confluent());
    REQUIRE(!kb1.finished());
    kb1.run();
    REQUIRE(kb1.finished());
    REQUIRE(kb1.rewriting_system().confluent());
    REQUIRE(kb1.rewriting_system().confluent_known());
    REQUIRE(kb1.rewriting_system().number_of_rules() == 8);

    KnuthBendix<word_type, TestType> kb3(twosided, std::move(p2));
    REQUIRE(!kb3.rewriting_system().confluent());
    REQUIRE(!kb3.finished());
    kb3.run();
    REQUIRE(kb3.finished());
    REQUIRE(kb3.rewriting_system().confluent());
    REQUIRE(kb3.rewriting_system().confluent_known());
    REQUIRE(kb3.rewriting_system().number_of_rules() == 4);
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "142",
                                   "close to or greater than 255 letters",
                                   "[quick][knuth-bendix]",
                                   REWRITING_SYSTEM_TYPES) {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.alphabet(257);

    REQUIRE_THROWS_AS(KnuthBendix(twosided, p), LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "143",
                                   "process pending rules x2",
                                   "[quick][knuth-bendix][no-valgrind]",
                                   REWRITING_SYSTEM_TYPES) {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.alphabet(2);
    p.contains_empty_word(true);

    WordRange wr;
    wr.alphabet_size(2).min(0).max(19);
    for (auto const& word : wr) {
      presentation::add_rule_no_checks(p, word, ""_w);
    }

    KnuthBendix<word_type, TestType> kb(twosided, p);
    REQUIRE(kb.rewriting_system().number_of_rules() == 524'286);
    kb.rewriting_system().reduce();
    REQUIRE(kb.rewriting_system().number_of_rules() == 2);
  }

}  // namespace libsemigroups
