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

// #define CATCH_CONFIG_ENABLE_PAIR_STRINGMAKER

#include "catch.hpp"      // for REQUIRE, REQUIRE_NOTHROW, REQUIRE_THROWS_AS
#include "test-main.hpp"  // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/fpsemi-examples.hpp"   // for chinese_monoid
#include "libsemigroups/knuth-bendix-new.hpp"  // for KnuthBendix, operator<<
#include "libsemigroups/report.hpp"            // for ReportGuard
#include "libsemigroups/types.hpp"             // for word_type

namespace libsemigroups {
  using literals::operator""_w;

  struct LibsemigroupsException;

  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "103",
                          "Presentation<word_type>",
                          "[quick][knuth-bendix]") {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.alphabet(2);
    presentation::add_rule(p, 000_w, 0_w);
    presentation::add_rule(p, 0_w, 11_w);

    KnuthBendix kb(p);

    REQUIRE(!kb.finished());
    REQUIRE(kb.size() == 5);
    REQUIRE(kb.finished());

    REQUIRE(kb.normal_form(to_string(kb.presentation(), 001_w)) == "aab");
    REQUIRE(kb.normal_form(to_string(kb.presentation(), 00001_w)) == "aab");
    REQUIRE(kb.normal_form(to_string(kb.presentation(), 011001_w)) == "aab");
    REQUIRE(!kb.equal_to(to_string(kb.presentation(), 000_w),
                         to_string(kb.presentation(), 1_w)));
    REQUIRE(!kb.equal_to(to_string(kb.presentation(), 0000_w),
                         to_string(kb.presentation(), 000_w)));
  }

  //
  //    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
  //                            "104",
  //                            "free semigroup congruence (6 classes)",
  //                            "[quick][knuth-bendix]")
  //                            {
  //      auto        rg = ReportGuard(false);
  //      KnuthBendix kb;
  //      kb.set_number_of_generators(5);
  //      presentation::add_rule(p, {0, 0}, {0});
  //      presentation::add_rule(p, {0, 1}, {1});
  //      presentation::add_rule(p, {1, 0}, {1});
  //      presentation::add_rule(p, {0, 2}, {2});
  //      presentation::add_rule(p, {2, 0}, {2});
  //      presentation::add_rule(p, {0, 3}, {3});
  //      presentation::add_rule(p, {3, 0}, {3});
  //      presentation::add_rule(p, {0, 4}, {4});
  //      presentation::add_rule(p, {4, 0}, {4});
  //      presentation::add_rule(p, {1, 2}, {0});
  //      presentation::add_rule(p, {2, 1}, {0});
  //      presentation::add_rule(p, {3, 4}, {0});
  //      presentation::add_rule(p, {4, 3}, {0});
  //      presentation::add_rule(p, {2, 2}, {0});
  //      presentation::add_rule(p, {1, 4, 2, 3, 3}, {0});
  //      presentation::add_rule(p, {4, 4, 4}, {0});
  //
  //      REQUIRE(kb.number_of_classes() == 6);
  //      // Throws because there's no parent semigroup
  //      REQUIRE_THROWS_AS(kb.number_of_non_trivial_classes(),
  //                        LibsemigroupsException);
  //      REQUIRE(kb.word_to_class_index({1}) == kb.word_to_class_index({2}));
  //    }
  //
  //    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
  //                            "105",
  //                            "free semigroup congruence (16 classes)",
  //                            "[quick][knuth-bendix]") {
  //      auto        rg = ReportGuard(false);
  //      KnuthBendix kb;
  //      kb.set_number_of_generators(4);
  //      presentation::add_rule(p, {3}, {2});
  //      presentation::add_rule(p, {0, 3}, {0, 2});
  //      presentation::add_rule(p, {1, 1}, {1});
  //      presentation::add_rule(p, {1, 3}, {1, 2});
  //      presentation::add_rule(p, {2, 1}, {2});
  //      presentation::add_rule(p, {2, 2}, {2});
  //      presentation::add_rule(p, {2, 3}, {2});
  //      presentation::add_rule(p, {0, 0, 0}, {0});
  //      presentation::add_rule(p, {0, 0, 1}, {1});
  //      presentation::add_rule(p, {0, 0, 2}, {2});
  //      presentation::add_rule(p, {0, 1, 2}, {1, 2});
  //      presentation::add_rule(p, {1, 0, 0}, {1});
  //      presentation::add_rule(p, {1, 0, 2}, {0, 2});
  //      presentation::add_rule(p, {2, 0, 0}, {2});
  //      presentation::add_rule(p, {0, 1, 0, 1}, {1, 0, 1});
  //      presentation::add_rule(p, {0, 2, 0, 2}, {2, 0, 2});
  //      presentation::add_rule(p, {1, 0, 1, 0}, {1, 0, 1});
  //      presentation::add_rule(p, {1, 2, 0, 1}, {1, 0, 1});
  //      presentation::add_rule(p, {1, 2, 0, 2}, {2, 0, 2});
  //      presentation::add_rule(p, {2, 0, 1, 0}, {2, 0, 1});
  //      presentation::add_rule(p, {2, 0, 2, 0}, {2, 0, 2});
  //
  //      REQUIRE(kb.number_of_classes() == 16);
  //      // REQUIRE(kb.knuth_bendix().number_of_active_rules() == 16);
  //      REQUIRE(kb.word_to_class_index({2}) == kb.word_to_class_index({3}));
  //    }
  //
  //    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
  //                            "106",
  //                            "free semigroup congruence (6 classes)",
  //                            "[quick][knuth-bendix]") {
  //      auto        rg = ReportGuard(false);
  //      KnuthBendix kb;
  //      kb.set_number_of_generators(11);
  //      presentation::add_rule(p, {2}, {1});
  //      presentation::add_rule(p, {4}, {3});
  //      presentation::add_rule(p, {5}, {0});
  //      presentation::add_rule(p, {6}, {3});
  //      presentation::add_rule(p, {7}, {1});
  //      presentation::add_rule(p, {8}, {3});
  //      presentation::add_rule(p, {9}, {3});
  //      presentation::add_rule(p, {10}, {0});
  //      presentation::add_rule(p, {0, 2}, {0, 1});
  //      presentation::add_rule(p, {0, 4}, {0, 3});
  //      presentation::add_rule(p, {0, 5}, {0, 0});
  //      presentation::add_rule(p, {0, 6}, {0, 3});
  //      presentation::add_rule(p, {0, 7}, {0, 1});
  //      presentation::add_rule(p, {0, 8}, {0, 3});
  //      presentation::add_rule(p, {0, 9}, {0, 3});
  //      presentation::add_rule(p, {0, 10}, {0, 0});
  //      presentation::add_rule(p, {1, 1}, {1});
  //      presentation::add_rule(p, {1, 2}, {1});
  //      presentation::add_rule(p, {1, 4}, {1, 3});
  //      presentation::add_rule(p, {1, 5}, {1, 0});
  //      presentation::add_rule(p, {1, 6}, {1, 3});
  //      presentation::add_rule(p, {1, 7}, {1});
  //      presentation::add_rule(p, {1, 8}, {1, 3});
  //      presentation::add_rule(p, {1, 9}, {1, 3});
  //      presentation::add_rule(p, {1, 10}, {1, 0});
  //      presentation::add_rule(p, {3, 1}, {3});
  //      presentation::add_rule(p, {3, 2}, {3});
  //      presentation::add_rule(p, {3, 3}, {3});
  //      presentation::add_rule(p, {3, 4}, {3});
  //      presentation::add_rule(p, {3, 5}, {3, 0});
  //      presentation::add_rule(p, {3, 6}, {3});
  //      presentation::add_rule(p, {3, 7}, {3});
  //      presentation::add_rule(p, {3, 8}, {3});
  //      presentation::add_rule(p, {3, 9}, {3});
  //      presentation::add_rule(p, {3, 10}, {3, 0});
  //      presentation::add_rule(p, {0, 0, 0}, {0});
  //      presentation::add_rule(p, {0, 0, 1}, {1});
  //      presentation::add_rule(p, {0, 0, 3}, {3});
  //      presentation::add_rule(p, {0, 1, 3}, {1, 3});
  //      presentation::add_rule(p, {1, 0, 0}, {1});
  //      presentation::add_rule(p, {1, 0, 3}, {0, 3});
  //      presentation::add_rule(p, {3, 0, 0}, {3});
  //      presentation::add_rule(p, {0, 1, 0, 1}, {1, 0, 1});
  //      presentation::add_rule(p, {0, 3, 0, 3}, {3, 0, 3});
  //      presentation::add_rule(p, {1, 0, 1, 0}, {1, 0, 1});
  //      presentation::add_rule(p, {1, 3, 0, 1}, {1, 0, 1});
  //      presentation::add_rule(p, {1, 3, 0, 3}, {3, 0, 3});
  //      presentation::add_rule(p, {3, 0, 1, 0}, {3, 0, 1});
  //      presentation::add_rule(p, {3, 0, 3, 0}, {3, 0, 3});
  //
  //      REQUIRE(kb.number_of_classes() == 16);
  //      REQUIRE(kb.word_to_class_index({0}) == kb.word_to_class_index({5}));
  //      REQUIRE(kb.word_to_class_index({0}) == kb.word_to_class_index({10}));
  //      REQUIRE(kb.word_to_class_index({1}) == kb.word_to_class_index({2}));
  //      REQUIRE(kb.word_to_class_index({1}) == kb.word_to_class_index({7}));
  //      REQUIRE(kb.word_to_class_index({3}) == kb.word_to_class_index({4}));
  //      REQUIRE(kb.word_to_class_index({3}) == kb.word_to_class_index({6}));
  //      REQUIRE(kb.word_to_class_index({3}) == kb.word_to_class_index({8}));
  //      REQUIRE(kb.word_to_class_index({3}) == kb.word_to_class_index({9}));
  //    }
  //
  //    LIBSEMIGROUPS_TEST_CASE(
  //        "KnuthBendix",
  //        "107",
  //        "free semigroup congruence (240 classes)",
  //        "[no-valgrind][quick][knuth-bendix]") {
  //      auto        rg = ReportGuard(false);
  //      KnuthBendix kb;
  //      kb.set_number_of_generators(2);
  //      presentation::add_rule(p, {0, 0, 0}, {0});
  //      presentation::add_rule(p, {1, 1, 1, 1}, {1});
  //      presentation::add_rule(p, {0, 1, 1, 1, 0}, {0, 0});
  //      presentation::add_rule(p, {1, 0, 0, 1}, {1, 1});
  //      presentation::add_rule(p, {0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0}, {0,
  //      0});
  //
  //      REQUIRE(kb.number_of_classes() == 240);
  //    }
  //
  //    LIBSEMIGROUPS_TEST_CASE(
  //        "KnuthBendix",
  //        "108",
  //        "free semigroup congruence (240 classes)",
  //        "[no-valgrind][quick][knuth-bendix]") {
  //      auto        rg = ReportGuard(false);
  //      KnuthBendix kb;
  //      kb.set_number_of_generators(2);
  //      presentation::add_rule(p, {0, 0, 0}, {0});
  //      presentation::add_rule(p, {1, 1, 1, 1}, {1});
  //      presentation::add_rule(p, {0, 1, 1, 1, 0}, {0, 0});
  //      presentation::add_rule(p, {1, 0, 0, 1}, {1, 1});
  //      presentation::add_rule(p, {0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0}, {0,
  //      0});
  //
  //      REQUIRE_NOTHROW(kb.knuth_bendix().froidure_pin());
  //      REQUIRE_THROWS_AS(presentation::add_rule(p, {0}, {1}),
  //      LibsemigroupsException);
  //    }
  //
  //    LIBSEMIGROUPS_TEST_CASE(
  //        "KnuthBendix",
  //        "109",
  //        "less",
  //        "[no-valgrind][quick][knuth-bendix]") {
  //      auto rg = ReportGuard(false);
  //      {
  //        KnuthBendix kb;
  //        kb.set_number_of_generators(2);
  //        presentation::add_rule(p, {0, 0, 0}, {0});
  //        presentation::add_rule(p, {1, 1, 1, 1, 1}, {1});
  //        presentation::add_rule(p, {0, 1, 1, 1, 0, 1, 1}, {1, 1, 0});
  //        kb.run();
  //
  //        REQUIRE(!kb.less({0, 1, 1, 1, 0, 0, 1, 1, 1, 0}, {1, 1, 1, 1, 0,
  //        0})); REQUIRE(!kb.less({0, 1, 1, 0}, {0, 1, 1, 0}));
  //      }
  //      {
  //        KnuthBendix kb;
  //        kb.set_number_of_generators(2);
  //        presentation::add_rule(p, {0, 0, 0}, {0});
  //        presentation::add_rule(p, {1, 1, 1, 1, 1}, {1});
  //        presentation::add_rule(p, {0, 1, 1, 1, 0, 1, 1}, {1, 1, 0});
  //
  //        REQUIRE(!kb.less({0, 1, 1, 1, 0, 0, 1, 1, 1, 0}, {1, 1, 1, 1, 0,
  //        0})); REQUIRE(!kb.less({0, 1, 1, 0}, {0, 1, 1, 0}));
  //      }
  //    }
  //
  //    LIBSEMIGROUPS_TEST_CASE(
  //        "KnuthBendix",
  //        "110",
  //        "less",
  //        "[quick][knuth-bendix][no-valgrind]") {
  //      auto        rg = ReportGuard(false);
  //      KnuthBendix kb;
  //      kb.set_number_of_generators(2);
  //      presentation::add_rule(p, {0, 0, 0}, {0});
  //      presentation::add_rule(p, {1, 1, 1, 1, 1, 1, 1, 1, 1}, {1});
  //      presentation::add_rule(p, {0, 1, 1, 1, 1, 1, 0, 1, 1}, {1, 1, 0});
  //
  //      REQUIRE(kb.less({0, 0, 0}, {1, 1, 1, 1, 1, 1, 1, 1, 1}));
  //    }
  //
  //    LIBSEMIGROUPS_TEST_CASE(
  //        "KnuthBendix",
  //        "111",
  //        "constructors",
  //        "[quick][knuth-bendix][no-valgrind]") {
  //      auto        rg = ReportGuard(false);
  //      KnuthBendix kb;
  //      kb.set_number_of_generators(2);
  //      presentation::add_rule(p, {0, 0, 0}, {0});
  //      presentation::add_rule(p, {1, 1, 1, 1, 1, 1, 1, 1, 1}, {1});
  //      presentation::add_rule(p, {0, 1, 1, 1, 1, 1, 0, 1, 1}, {1, 1, 0});
  //      REQUIRE(kb.number_of_classes() == 746);
  //      auto copy(kb);
  //      REQUIRE(copy.number_of_classes() == 746);
  //      REQUIRE(copy.number_of_generators() == 2);
  //      // the copy uses the "active rules" of kb, of which there are 105
  //      since
  //      // knuth-bendix has already been run.
  //      REQUIRE(copy.number_of_generating_pairs() == 105);
  //    }
  //
  //    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
  //                            "112",
  //                            "const_contains/contains",
  //                            "[quick][knuth-bendix]") {
  //      auto        rg = ReportGuard(false);
  //      KnuthBendix kb;
  //      kb.set_number_of_generators(2);
  //      presentation::add_rule(p, {0, 0, 0}, {0});
  //      presentation::add_rule(p, {1, 1, 1, 1}, {1});
  //      presentation::add_rule(p, {0, 1, 1, 1, 1, 1, 0, 1, 1}, {1, 1, 0});
  //
  //      REQUIRE(kb.const_contains({0, 0, 0}, {1, 1, 1, 1, 1, 1, 1, 1, 1})
  //              == tril::unknown);
  //      REQUIRE(kb.const_contains({0, 0, 0}, {0, 0, 0}) == tril::TRUE);
  //      REQUIRE_THROWS_AS(kb.const_contains({0, 0, 2}, {0, 0, 0}),
  //                        LibsemigroupsException);
  //      REQUIRE(kb.const_contains({0, 0, 0}, {0}) == tril::TRUE);
  //      REQUIRE(kb.number_of_classes() == 12);
  //      REQUIRE(kb.const_contains({0, 0, 0}, {1, 1}) == tril::FALSE);
  //      REQUIRE(!kb.contains({0, 0, 0}, {1, 1}));
  //      REQUIRE(kb.contains({0, 0, 0}, {0, 0, 0}));
  //    }
  //
  //    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
  //                            "113",
  //                            "is_quotient_obviously_finite",
  //                            "[quick][knuth-bendix]") {
  //      auto        rg = ReportGuard(false);
  //      KnuthBendix kb;
  //      kb.set_number_of_generators(2);
  //      presentation::add_rule(p, {0, 0, 0}, {0});
  //      presentation::add_rule(p, {1, 1, 1, 1}, {1});
  //      presentation::add_rule(p, {0, 1, 1, 1, 1, 1, 0, 1, 1}, {1, 1, 0});
  //      REQUIRE(!kb.is_quotient_obviously_finite());
  //    }
  //
  //    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
  //                            "114",
  //                            "class_index_to_word",
  //                            "[quick][knuth-bendix]") {
  //      auto        rg = ReportGuard(false);
  //      KnuthBendix kb;
  //      kb.set_number_of_generators(2);
  //      presentation::add_rule(p, {0, 0, 0}, {0});
  //      presentation::add_rule(p, {1, 1, 1, 1}, {1});
  //      presentation::add_rule(p, {0, 1, 1, 1, 1, 1, 0, 1, 1}, {1, 1, 0});
  //      REQUIRE(kb.class_index_to_word(0) == word_type({0}));
  //      REQUIRE(kb.class_index_to_word(1) == word_type({1}));
  //      REQUIRE(kb.class_index_to_word(2) == word_type({0, 0}));
  //      REQUIRE(kb.class_index_to_word(3) == word_type({0, 1}));
  //      REQUIRE(kb.class_index_to_word(4) == word_type({1, 0}));
  //      REQUIRE(kb.class_index_to_word(5) == word_type({1, 1}));
  //      REQUIRE(kb.class_index_to_word(6) == word_type({0, 0, 1}));
  //      REQUIRE(kb.class_index_to_word(7) == word_type({0, 1, 1}));
  //      REQUIRE(kb.class_index_to_word(8) == word_type({1, 1, 1}));
  //      REQUIRE(kb.class_index_to_word(9) == word_type({0, 0, 1, 1}));
  //      REQUIRE(kb.class_index_to_word(10) == word_type({0, 1, 1, 1}));
  //      REQUIRE(kb.class_index_to_word(11) == word_type({0, 0, 1, 1, 1}));
  //    }
  //
  //    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
  //                            "115",
  //                            "quotient_froidure_pin",
  //                            "[quick][knuth-bendix]") {
  //      auto        rg = ReportGuard(false);
  //      KnuthBendix kb;
  //      kb.set_number_of_generators(2);
  //      presentation::add_rule(p, {0, 0, 0}, {0});
  //      presentation::add_rule(p, {1, 1, 1, 1}, {1});
  //      presentation::add_rule(p, {0, 1, 1, 1, 1, 1, 0, 1, 1}, {1, 1, 0});
  //      REQUIRE(kb.quotient_froidure_pin()->size() == 12);
  //    }
  //
  //    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
  //                            "116",
  //                            "set_number_of_generators",
  //                            "[quick][knuth-bendix]") {
  //      auto        rg = ReportGuard(false);
  //      KnuthBendix kb;
  //      REQUIRE_NOTHROW(kb.set_number_of_generators(2));
  //      REQUIRE_THROWS_AS(kb.set_number_of_generators(3),
  //      LibsemigroupsException);
  //      REQUIRE_NOTHROW(kb.set_number_of_generators(2));
  //    }
  //
  //    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
  //                            "117",
  //                            "number of classes when obv-inf",
  //                            "[quick][knuth-bendix]") {
  //      auto        rg = ReportGuard(false);
  //      KnuthBendix kb;
  //      kb.set_number_of_generators(3);
  //      presentation::add_rule(p, {0, 1}, {1, 0});
  //      presentation::add_rule(p, {0, 2}, {2, 0});
  //      presentation::add_rule(p, {0, 0}, {0});
  //      presentation::add_rule(p, {0, 2}, {0});
  //      presentation::add_rule(p, {2, 0}, {0});
  //      presentation::add_rule(p, {1, 1}, {1, 1});
  //      presentation::add_rule(p, {1, 2}, {2, 1});
  //      presentation::add_rule(p, {1, 1, 1}, {1});
  //      presentation::add_rule(p, {1, 2}, {1});
  //      presentation::add_rule(p, {2, 1}, {1});
  //      presentation::add_rule(p, {0}, {1});
  //      REQUIRE(kb.is_quotient_obviously_infinite());
  //      REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
  //    }
  //
  //    // LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
  //    //                         "020",
  //    //                         "Chinese monoid",
  //    //                         "[quick][knuth-bendix]") {
  //    //   auto        rg = ReportGuard(false);
  //    //   KnuthBendix kb;
  //    //   kb.set_number_of_generators(3);
  //    //   for (auto const& rel : chinese_monoid(3)) {
  //    //     presentation::add_rule(p, rel.first, rel.second);
  //    //   }
  //    //   REQUIRE(kb.is_quotient_obviously_infinite());
  //    //   REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
  //    //   REQUIRE(kb.number_of_generating_pairs() == 8);
  //    //   REQUIRE(std::vector<relation_type>(kb.cbegin_generating_pairs(),
  //    //                                      kb.cend_generating_pairs())
  //    //           == std::vector<relation_type>({{{1, 0, 0}, {0, 1, 0}},
  //    //                                          {{2, 0, 0}, {0, 2, 0}},
  //    //                                          {{1, 1, 0}, {1, 0, 1}},
  //    //                                          {{2, 1, 0}, {2, 0, 1}},
  //    //                                          {{2, 1, 0}, {1, 2, 0}},
  //    //                                          {{2, 2, 0}, {2, 0, 2}},
  //    //                                          {{2, 1, 1}, {1, 2, 1}},
  //    //                                          {{2, 2, 1}, {2, 1, 2}}}));
  //    //   REQUIRE(kb.knuth_bendix().number_of_normal_forms(0, 10) == 1175);
  //    // }
  //
  //    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
  //                            "083",
  //                            "partial_transformation_monoid4",
  //                            "[standard][knuth-bendix]") {
  //      auto rg = ReportGuard(false);
  //
  //      size_t n = 4;
  //      auto   s = partial_transformation_monoid(n, author::Sutov);
  //      auto   p = to_presentation<word_type>(s);
  //      p.alphabet(n + 2);
  //      presentation::replace_word(p, word_type({}), {n + 1});
  //      presentation::add_identity_rules(p, n + 1);
  //
  //      KnuthBendix kb;
  //      kb.set_number_of_generators(n + 2);
  //      for (size_t i = 0; i < p.rules.size() - 1; i += 2) {
  //        presentation::add_rule(p, p.rules[i], p.rules[i + 1]);
  //      }
  //      REQUIRE(!kb.is_quotient_obviously_infinite());
  //      REQUIRE(kb.number_of_classes() == 625);
  //    }
  //
  //    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
  //                            "118",
  //                            "partial_transformation_monoid5",
  //                            "[extreme][knuth-bendix]") {
  //      auto rg = ReportGuard(false);
  //
  //      size_t n = 5;
  //      auto   s = partial_transformation_monoid(n, author::Sutov);
  //      auto   p = to_presentation<word_type>(s);
  //      p.alphabet(n + 2);
  //      presentation::replace_word(p, word_type({}), {n + 1});
  //      presentation::add_identity_rules(p, n + 1);
  //
  //      KnuthBendix kb;
  //      kb.set_number_of_generators(n + 2);
  //      for (size_t i = 0; i < p.rules.size() - 1; i += 2) {
  //        presentation::add_rule(p, p.rules[i], p.rules[i + 1]);
  //      }
  //      REQUIRE(!kb.is_quotient_obviously_infinite());
  //      REQUIRE(kb.number_of_classes() == 7776);
  //    }
  //
  //    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
  //                            "119",
  //                            "full_transformation_monoid Iwahori",
  //                            "[extreme][knuth-bendix]") {
  //      auto rg = ReportGuard(false);
  //
  //      size_t n = 5;
  //      auto   s = full_transformation_monoid(n, author::Iwahori);
  //      auto   p = to_presentation<word_type>(s);
  //      p.alphabet(n + 1);
  //      presentation::replace_word(p, word_type({}), {n});
  //      presentation::add_identity_rules(p, n);
  //      KnuthBendix kb;
  //      kb.set_number_of_generators(n + 1);
  //      for (size_t i = 0; i < p.rules.size() - 1; i += 2) {
  //        presentation::add_rule(p, p.rules[i], p.rules[i + 1]);
  //      }
  //      REQUIRE(!kb.is_quotient_obviously_infinite());
  //      REQUIRE(kb.number_of_classes() == 3125);
  //    }
  //
  //  }  // namespace congruence

}  // namespace libsemigroups
