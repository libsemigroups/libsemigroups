// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2020-2025 James D. Mitchell
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

// This file is the sixth of six that contains tests for the KnuthBendix
// classes. In a mostly vain attempt to speed up compilation the tests are
// split across 6 files as follows:
//
// 1: contains quick tests for KnuthBendix created from rules and all commented
//    out tests.
//
// 2: contains more quick tests for KnuthBendix created from rules
//
// 3: contains yet more quick tests for KnuthBendix created from rules
//
// 4: contains standard and extreme test for KnuthBendix created from rules
//
// 5: contains tests for KnuthBendix created from FroidurePin instances
//
// 6: contains tests for KnuthBendix.

#include <cstddef>  // for size_t
#include <utility>  // for move
#include <vector>   // for vector

#include "Catch2-3.8.0/catch_amalgamated.hpp"  // for operator""_catch_sr
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
  congruence_kind constexpr twosided = congruence_kind::twosided;

  using literals::operator""_w;

  using RewriteTrie     = detail::RewriteTrie;
  using RewriteFromLeft = detail::RewriteFromLeft;

#define KNUTH_BENDIX_TYPES RewriteTrie, RewriteFromLeft

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "129",
                                   "Presentation<word_type>",
                                   "[quick][knuth-bendix]",
                                   KNUTH_BENDIX_TYPES) {
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
                                   KNUTH_BENDIX_TYPES) {
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
                                   KNUTH_BENDIX_TYPES) {
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
    REQUIRE(kb.number_of_active_rules() == 18);
    REQUIRE(knuth_bendix::contains(kb, 2_w, 3_w));
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "132",
                                   "free semigroup congruence x 2",
                                   "[quick][knuth-bendix]",
                                   KNUTH_BENDIX_TYPES) {
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
                                   KNUTH_BENDIX_TYPES) {
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
                                   KNUTH_BENDIX_TYPES) {
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
                                   KNUTH_BENDIX_TYPES) {
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
    REQUIRE(copy.number_of_active_rules() == 105);
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "136",
                                   "number of classes when obv-inf",
                                   "[quick][knuth-bendix]",
                                   KNUTH_BENDIX_TYPES) {
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
                                   KNUTH_BENDIX_TYPES) {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p  = presentation::examples::chinese_monoid(3);

    KnuthBendix<word_type, TestType> kb(twosided, p);
    REQUIRE(is_obviously_infinite(kb));
    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
    REQUIRE(kb.presentation().rules.size() / 2 == 8);
    auto nf = knuth_bendix::normal_forms(kb).min(1).max(10);
    REQUIRE(nf.count() == 1'175);
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "138",
                                   "partial_transformation_monoid(4)",
                                   "[quick][knuth-bendix][no-valgrind]",
                                   KNUTH_BENDIX_TYPES) {
    auto rg = ReportGuard(false);

    size_t n = 4;
    auto   p = presentation::examples::partial_transformation_monoid_Shu60(n);

    KnuthBendix<word_type, TestType> kb(twosided, p);
    REQUIRE(!is_obviously_infinite(kb));
    REQUIRE(kb.number_of_classes() == 625);
    REQUIRE(to_human_readable_repr(kb)
            == "<confluent 2-sided KnuthBendix over <monoid presentation with "
               "5 letters, 260 rules, and length 4,131> with 0 gen. pairs, 362 "
               "active rules, 0 pending rules>");
  }

  // Takes about 1 minute
  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "139",
                                   "partial_transformation_monoid5",
                                   "[extreme][knuth-bendix]",
                                   RewriteTrie  // KNUTH_BENDIX_TYPES
  ) {
    auto rg = ReportGuard(true);

    size_t n = 5;
    auto   p = presentation::examples::partial_transformation_monoid_Shu60(n);

    KnuthBendix<word_type, TestType> kb(twosided, p);
    kb.max_pending_rules(100'000);
    REQUIRE(!is_obviously_infinite(kb));
    REQUIRE(kb.number_of_classes() == 7'776);
  }

  // Takes about 5 seconds
  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "140",
                                   "full_transformation_monoid Iwahori",
                                   "[extreme][knuth-bendix]",
                                   KNUTH_BENDIX_TYPES) {
    auto rg = ReportGuard(true);

    size_t n = 5;
    auto   p = presentation::examples::full_transformation_monoid_II74(n);
    KnuthBendix<word_type, TestType> kb(twosided, p);
    REQUIRE(!is_obviously_infinite(kb));
    kb.run();
    REQUIRE(kb.number_of_active_rules() == 1'162);
    REQUIRE(kb.number_of_classes() == 3'125);
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "141",
                                   "constructors/init for finished x 2",
                                   "[quick][knuth-bendix]",
                                   KNUTH_BENDIX_TYPES) {
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
    REQUIRE(!kb1.confluent());
    REQUIRE(!kb1.finished());
    kb1.run();
    REQUIRE(kb1.confluent());
    REQUIRE(kb1.number_of_active_rules() == 8);

    kb1.init(twosided, p2);
    REQUIRE(!kb1.confluent());
    REQUIRE(!kb1.finished());
    kb1.run();
    REQUIRE(kb1.finished());
    REQUIRE(kb1.confluent());
    REQUIRE(kb1.confluent_known());
    REQUIRE(kb1.number_of_active_rules() == 4);

    kb1.init(twosided, p1);
    REQUIRE(!kb1.confluent());
    REQUIRE(!kb1.finished());
    kb1.run();
    REQUIRE(kb1.finished());
    REQUIRE(kb1.confluent());
    REQUIRE(kb1.confluent_known());
    REQUIRE(kb1.number_of_active_rules() == 8);

    KnuthBendix kb2(std::move(kb1));
    REQUIRE(kb2.confluent());
    REQUIRE(kb2.confluent_known());
    REQUIRE(kb2.finished());
    REQUIRE(kb2.number_of_active_rules() == 8);

    kb1 = std::move(kb2);
    REQUIRE(kb1.confluent());
    REQUIRE(kb1.confluent_known());
    REQUIRE(kb1.finished());
    REQUIRE(kb1.number_of_active_rules() == 8);

    kb1.init(twosided, std::move(p1));
    REQUIRE(!kb1.confluent());
    REQUIRE(!kb1.finished());
    kb1.run();
    REQUIRE(kb1.finished());
    REQUIRE(kb1.confluent());
    REQUIRE(kb1.confluent_known());
    REQUIRE(kb1.number_of_active_rules() == 8);

    KnuthBendix<word_type, TestType> kb3(twosided, std::move(p2));
    REQUIRE(!kb3.confluent());
    REQUIRE(!kb3.finished());
    kb3.run();
    REQUIRE(kb3.finished());
    REQUIRE(kb3.confluent());
    REQUIRE(kb3.confluent_known());
    REQUIRE(kb3.number_of_active_rules() == 4);
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "142",
                                   "close to or greater than 255 letters",
                                   "[quick][knuth-bendix]",
                                   KNUTH_BENDIX_TYPES) {
    Presentation<word_type> p;
    p.alphabet(257);

    REQUIRE_THROWS_AS(KnuthBendix(twosided, p), LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "118",
                                   "process pending rules x1",
                                   "[extreme][knuth-bendix]",
                                   KNUTH_BENDIX_TYPES) {
    Presentation<word_type> p;
    p.alphabet(2);
    p.contains_empty_word(true);

    WordRange wr;
    wr.alphabet_size(2).min(16).max(17);
    for (auto const& word : wr) {
      presentation::add_rule(p, word, ""_w);
    }

    KnuthBendix<word_type, TestType> kb(twosided, p);
    REQUIRE(kb.number_of_pending_rules() == wr.count());
    kb.process_pending_rules();
    REQUIRE(kb.number_of_active_rules() == wr.count());
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("KnuthBendix",
                                   "143",
                                   "process pending rules x2",
                                   "[quick][knuth-bendix]",
                                   KNUTH_BENDIX_TYPES) {
    Presentation<word_type> p;
    p.alphabet(2);
    p.contains_empty_word(true);

    WordRange wr;
    wr.alphabet_size(2).min(0).max(19);
    for (auto const& word : wr) {
      presentation::add_rule(p, word, ""_w);
    }

    KnuthBendix<word_type, TestType> kb(twosided, p);
    kb.process_pending_rules();
    REQUIRE(kb.number_of_active_rules() == 2);
  }

}  // namespace libsemigroups
