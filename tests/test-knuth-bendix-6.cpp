// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2020-2023 James D. Mitchell
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
#include <string>   // for basic_string, char_traits
#include <utility>  // for move
#include <vector>   // for vector

#include "catch.hpp"      // for operator""_catch_sr
#include "test-main.hpp"  // for TEMPLATE_TEST_CASE

#include "libsemigroups/constants.hpp"        // for operator==, Max, POSIT...
#include "libsemigroups/exception.hpp"        // for LibsemigroupsException
#include "libsemigroups/fpsemi-examples.hpp"  // for partial_transformation...
#include "libsemigroups/froidure-pin.hpp"     // for FroidurePin
#include "libsemigroups/knuth-bendix.hpp"     // for KnuthBendix, normal_forms
#include "libsemigroups/obvinf.hpp"           // for is_obviously_infinite
#include "libsemigroups/paths.hpp"            // for Paths
#include "libsemigroups/presentation.hpp"     // for to_string, add_rule
#include "libsemigroups/to-froidure-pin.hpp"  // for to_froidure_pin
#include "libsemigroups/types.hpp"            // for word_type
#include "libsemigroups/words.hpp"            // for operator""_w

#include "libsemigroups/detail/report.hpp"  // for ReportGuard

namespace libsemigroups {
  congruence_kind constexpr twosided = congruence_kind::twosided;

  using literals::operator""_w;

#define KNUTH_BENDIX_TYPES \
  KnuthBendix<RewriteTrie>, KnuthBendix<RewriteFromLeft>

  TEMPLATE_TEST_CASE("Presentation<word_type>",
                     "[129][quick][knuth-bendix]",
                     KNUTH_BENDIX_TYPES) {
    auto rg = ReportGuard(false);

    Presentation<word_type> p;
    p.alphabet(2);
    presentation::add_rule(p, 000_w, 0_w);
    presentation::add_rule(p, 0_w, 11_w);

    TestType kb(twosided, p);

    REQUIRE(!kb.finished());
    REQUIRE(kb.number_of_classes() == 5);
    REQUIRE(kb.finished());

    REQUIRE(kb.normal_form(to_string(kb.presentation(), 001_w)) == "aab");
    REQUIRE(kb.normal_form(to_string(kb.presentation(), 00001_w)) == "aab");
    REQUIRE(kb.normal_form(to_string(kb.presentation(), 011001_w)) == "aab");
    REQUIRE(!kb.equal_to(to_string(kb.presentation(), 000_w),
                         to_string(kb.presentation(), 1_w)));
    REQUIRE(!kb.equal_to(to_string(kb.presentation(), 0000_w),
                         to_string(kb.presentation(), 000_w)));
  }

  TEMPLATE_TEST_CASE("free semigroup congruence (6 classes)",
                     "[130][quick][knuth-bendix]",
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

    TestType kb(twosided, p);

    REQUIRE(kb.number_of_classes() == 6);
    REQUIRE(kb.equal_to(to_string(kb.presentation(), 1_w),
                        to_string(kb.presentation(), 2_w)));
  }

  TEMPLATE_TEST_CASE("free semigroup congruence (16 classes)",
                     "[131][quick][knuth-bendix]",
                     KNUTH_BENDIX_TYPES) {
    auto rg = ReportGuard(false);

    Presentation<word_type> p;
    p.alphabet(4);
    presentation::add_rule(p, {3}, {2});
    presentation::add_rule(p, {0, 3}, {0, 2});
    presentation::add_rule(p, {1, 1}, {1});
    presentation::add_rule(p, {1, 3}, {1, 2});
    presentation::add_rule(p, {2, 1}, {2});
    presentation::add_rule(p, {2, 2}, {2});
    presentation::add_rule(p, {2, 3}, {2});
    presentation::add_rule(p, {0, 0, 0}, {0});
    presentation::add_rule(p, {0, 0, 1}, {1});
    presentation::add_rule(p, {0, 0, 2}, {2});
    presentation::add_rule(p, {0, 1, 2}, {1, 2});
    presentation::add_rule(p, {1, 0, 0}, {1});
    presentation::add_rule(p, {1, 0, 2}, {0, 2});
    presentation::add_rule(p, {2, 0, 0}, {2});
    presentation::add_rule(p, {0, 1, 0, 1}, {1, 0, 1});
    presentation::add_rule(p, {0, 2, 0, 2}, {2, 0, 2});
    presentation::add_rule(p, {1, 0, 1, 0}, {1, 0, 1});
    presentation::add_rule(p, {1, 2, 0, 1}, {1, 0, 1});
    presentation::add_rule(p, {1, 2, 0, 2}, {2, 0, 2});
    presentation::add_rule(p, {2, 0, 1, 0}, {2, 0, 1});
    presentation::add_rule(p, {2, 0, 2, 0}, {2, 0, 2});

    TestType kb(twosided, p);
    REQUIRE(kb.number_of_classes() == 16);
    REQUIRE(kb.number_of_active_rules() == 18);
    REQUIRE(kb.equal_to(to_string(kb.presentation(), 2_w),
                        to_string(kb.presentation(), 3_w)));
  }

  TEMPLATE_TEST_CASE("free semigroup congruence (6 classes) x 2",
                     "[132][quick][knuth-bendix]",
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

    TestType kb(twosided, p);
    REQUIRE(kb.number_of_classes() == 16);
    REQUIRE(kb.equal_to(to_string(kb.presentation(), {0}),
                        to_string(kb.presentation(), {5})));
    REQUIRE(kb.equal_to(to_string(kb.presentation(), {0}),
                        to_string(kb.presentation(), {5})));
    REQUIRE(kb.equal_to(to_string(kb.presentation(), {0}),
                        to_string(kb.presentation(), {10})));
    REQUIRE(kb.equal_to(to_string(kb.presentation(), {1}),
                        to_string(kb.presentation(), {2})));
    REQUIRE(kb.equal_to(to_string(kb.presentation(), {1}),
                        to_string(kb.presentation(), {7})));
    REQUIRE(kb.equal_to(to_string(kb.presentation(), {3}),
                        to_string(kb.presentation(), {4})));
    REQUIRE(kb.equal_to(to_string(kb.presentation(), {3}),
                        to_string(kb.presentation(), {6})));
    REQUIRE(kb.equal_to(to_string(kb.presentation(), {3}),
                        to_string(kb.presentation(), {8})));
    REQUIRE(kb.equal_to(to_string(kb.presentation(), {3}),
                        to_string(kb.presentation(), {9})));
  }

  TEMPLATE_TEST_CASE("free semigroup congruence (240 classes)",
                     "[133][no-valgrind][quick][knuth-bendix]",
                     KNUTH_BENDIX_TYPES) {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.alphabet(2);
    presentation::add_rule(p, 000_w, 0_w);
    presentation::add_rule(p, 1111_w, 1_w);
    presentation::add_rule(p, 01110_w, 00_w);
    presentation::add_rule(p, 1001_w, 11_w);
    presentation::add_rule(p, 001010101010_w, 00_w);

    TestType kb(twosided, p);
    REQUIRE(kb.number_of_classes() == 240);
  }

  TEMPLATE_TEST_CASE("free semigroup congruence (240 classes) x 2",
                     "[134][no-valgrind][quick][knuth-bendix]",
                     KNUTH_BENDIX_TYPES) {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.alphabet(2);
    presentation::add_rule(p, 000_w, 0_w);
    presentation::add_rule(p, 1111_w, 1_w);
    presentation::add_rule(p, 01110_w, 00_w);
    presentation::add_rule(p, 1001_w, 11_w);
    presentation::add_rule(p, 001010101010_w, 00_w);

    TestType kb(twosided, p);
    REQUIRE_NOTHROW(to_froidure_pin(kb));
  }

  TEMPLATE_TEST_CASE("constructors",
                     "[135][quick][knuth-bendix][no-valgrind]",
                     KNUTH_BENDIX_TYPES) {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.alphabet(2);
    presentation::add_rule(p, 000_w, 0_w);
    presentation::add_rule(p, 111111111_w, 1_w);
    presentation::add_rule(p, 011111011_w, 110_w);

    TestType kb(twosided, p);
    REQUIRE(kb.number_of_classes() == 746);

    auto copy(kb);
    REQUIRE(copy.number_of_classes() == 746);
    REQUIRE(copy.presentation().alphabet().size() == 2);
    // the copy uses the "active rules" of kb, of which there are 105 since
    // knuth-bendix has already been run.
    REQUIRE(copy.number_of_active_rules() == 105);
  }

  TEMPLATE_TEST_CASE("to_froidure_pin",
                     "[136][quick][knuth-bendix]",
                     KNUTH_BENDIX_TYPES) {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.alphabet(2);
    presentation::add_rule(p, 000_w, 0_w);
    presentation::add_rule(p, 1111_w, 1_w);
    presentation::add_rule(p, 011111011_w, 110_w);

    TestType kb(twosided, p);
    REQUIRE(to_froidure_pin(kb).size() == 12);
  }

  TEMPLATE_TEST_CASE("number of classes when obv-inf",
                     "[137][quick][knuth-bendix]",
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

    TestType kb(twosided, p);
    REQUIRE(is_obviously_infinite(kb));
    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
  }

  TEMPLATE_TEST_CASE("Chinese monoid x 2",
                     "[138][quick][knuth-bendix]",
                     KNUTH_BENDIX_TYPES) {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p  = fpsemigroup::chinese_monoid(3);

    TestType kb(twosided, p);
    REQUIRE(is_obviously_infinite(kb));
    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
    REQUIRE(kb.presentation().rules.size() / 2 == 8);
    auto nf = knuth_bendix::normal_forms(kb).min(1).max(10);
    REQUIRE(nf.count() == 1'175);
  }

  TEMPLATE_TEST_CASE("partial_transformation_monoid(4)",
                     "[139][standard][knuth-bendix]",
                     KNUTH_BENDIX_TYPES) {
    auto rg = ReportGuard(false);

    size_t n = 4;
    auto   p = partial_transformation_monoid(n, fpsemigroup::author::Sutov);

    TestType kb(twosided, p);
    REQUIRE(!is_obviously_infinite(kb));
    REQUIRE(kb.number_of_classes() == 625);
  }

  // Takes about 1 minute
  TEMPLATE_TEST_CASE("partial_transformation_monoid5",
                     "[140][extreme][knuth-bendix]",
                     KNUTH_BENDIX_TYPES) {
    auto rg = ReportGuard(true);

    size_t n = 5;
    auto   p = partial_transformation_monoid(n, fpsemigroup::author::Sutov);

    TestType kb(twosided, p);
    REQUIRE(!is_obviously_infinite(kb));
    REQUIRE(kb.number_of_classes() == 7'776);
  }

  // Takes about 5 seconds
  TEMPLATE_TEST_CASE("full_transformation_monoid Iwahori",
                     "[141][extreme][knuth-bendix]",
                     KNUTH_BENDIX_TYPES) {
    auto rg = ReportGuard(true);

    size_t   n = 5;
    auto     p = full_transformation_monoid(n, fpsemigroup::author::Iwahori);
    TestType kb(twosided, p);
    REQUIRE(!is_obviously_infinite(kb));
    kb.run();
    REQUIRE(kb.number_of_active_rules() == 1'162);
    REQUIRE(kb.number_of_classes() == 3'125);
  }

  TEMPLATE_TEST_CASE("constructors/init for finished x 2",
                     "[142][quick][knuth-bendix]",
                     KNUTH_BENDIX_TYPES) {
    using literals::operator""_w;
    auto rg = ReportGuard(false);

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

    TestType kb1(twosided, p1);
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

    TestType kb2(std::move(kb1));
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

    TestType kb3(twosided, std::move(p2));
    REQUIRE(!kb3.confluent());
    REQUIRE(!kb3.finished());
    kb3.run();
    REQUIRE(kb3.finished());
    REQUIRE(kb3.confluent());
    REQUIRE(kb3.confluent_known());
    REQUIRE(kb3.number_of_active_rules() == 4);
  }

}  // namespace libsemigroups
