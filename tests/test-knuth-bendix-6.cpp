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

#include "catch.hpp"      // for REQUIRE, REQUIRE_NOTHROW, REQUIRE_THROWS_AS
#include "test-main.hpp"  // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/knuth-bendix.hpp"  // for KnuthBendix, operator<<
#include "libsemigroups/report.hpp"        // for ReportGuard
#include "libsemigroups/types.hpp"         // for word_type

namespace libsemigroups {
  struct LibsemigroupsException;
  constexpr bool REPORT = false;

  namespace congruence {

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "103",
                            "(cong) free semigroup congruence (5 classes)",
                            "[quick][congruence][knuth-bendix][cong]") {
      auto rg = ReportGuard(REPORT);

      KnuthBendix kb;
      kb.set_number_of_generators(2);
      kb.add_pair({0, 0, 0}, {0});
      kb.add_pair({0}, {1, 1});

      REQUIRE(!kb.finished());
      REQUIRE(kb.number_of_classes() == 5);
      REQUIRE(kb.finished());

      REQUIRE(kb.word_to_class_index({0, 0, 1}) == 4);
      REQUIRE(kb.word_to_class_index({0, 0, 0, 0, 1}) == 4);
      REQUIRE(kb.word_to_class_index({0, 1, 1, 0, 0, 1}) == 4);
      REQUIRE(kb.word_to_class_index({0, 0, 0}) == 0);
      REQUIRE(kb.word_to_class_index({1}) == 1);
      REQUIRE(kb.word_to_class_index({0, 0, 0, 0}) == 2);
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "104",
                            "free semigroup congruence (6 classes)",
                            "[quick][cong][congruence][knuth-bendix][cong]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_number_of_generators(5);
      kb.add_pair({0, 0}, {0});
      kb.add_pair({0, 1}, {1});
      kb.add_pair({1, 0}, {1});
      kb.add_pair({0, 2}, {2});
      kb.add_pair({2, 0}, {2});
      kb.add_pair({0, 3}, {3});
      kb.add_pair({3, 0}, {3});
      kb.add_pair({0, 4}, {4});
      kb.add_pair({4, 0}, {4});
      kb.add_pair({1, 2}, {0});
      kb.add_pair({2, 1}, {0});
      kb.add_pair({3, 4}, {0});
      kb.add_pair({4, 3}, {0});
      kb.add_pair({2, 2}, {0});
      kb.add_pair({1, 4, 2, 3, 3}, {0});
      kb.add_pair({4, 4, 4}, {0});

      REQUIRE(kb.number_of_classes() == 6);
      // Throws because there's no parent semigroup
      REQUIRE_THROWS_AS(kb.number_of_non_trivial_classes(),
                        LibsemigroupsException);
      REQUIRE(kb.word_to_class_index({1}) == kb.word_to_class_index({2}));
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "105",
                            "(cong) free semigroup congruence (16 classes)",
                            "[quick][congruence][knuth-bendix][cong]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_number_of_generators(4);
      kb.add_pair({3}, {2});
      kb.add_pair({0, 3}, {0, 2});
      kb.add_pair({1, 1}, {1});
      kb.add_pair({1, 3}, {1, 2});
      kb.add_pair({2, 1}, {2});
      kb.add_pair({2, 2}, {2});
      kb.add_pair({2, 3}, {2});
      kb.add_pair({0, 0, 0}, {0});
      kb.add_pair({0, 0, 1}, {1});
      kb.add_pair({0, 0, 2}, {2});
      kb.add_pair({0, 1, 2}, {1, 2});
      kb.add_pair({1, 0, 0}, {1});
      kb.add_pair({1, 0, 2}, {0, 2});
      kb.add_pair({2, 0, 0}, {2});
      kb.add_pair({0, 1, 0, 1}, {1, 0, 1});
      kb.add_pair({0, 2, 0, 2}, {2, 0, 2});
      kb.add_pair({1, 0, 1, 0}, {1, 0, 1});
      kb.add_pair({1, 2, 0, 1}, {1, 0, 1});
      kb.add_pair({1, 2, 0, 2}, {2, 0, 2});
      kb.add_pair({2, 0, 1, 0}, {2, 0, 1});
      kb.add_pair({2, 0, 2, 0}, {2, 0, 2});

      REQUIRE(kb.number_of_classes() == 16);
      // REQUIRE(kb.knuth_bendix().number_of_active_rules() == 16);
      REQUIRE(kb.word_to_class_index({2}) == kb.word_to_class_index({3}));
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "106",
                            "(cong) free semigroup congruence (6 classes)",
                            "[quick][congruence][knuth-bendix][cong]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_number_of_generators(11);
      kb.add_pair({2}, {1});
      kb.add_pair({4}, {3});
      kb.add_pair({5}, {0});
      kb.add_pair({6}, {3});
      kb.add_pair({7}, {1});
      kb.add_pair({8}, {3});
      kb.add_pair({9}, {3});
      kb.add_pair({10}, {0});
      kb.add_pair({0, 2}, {0, 1});
      kb.add_pair({0, 4}, {0, 3});
      kb.add_pair({0, 5}, {0, 0});
      kb.add_pair({0, 6}, {0, 3});
      kb.add_pair({0, 7}, {0, 1});
      kb.add_pair({0, 8}, {0, 3});
      kb.add_pair({0, 9}, {0, 3});
      kb.add_pair({0, 10}, {0, 0});
      kb.add_pair({1, 1}, {1});
      kb.add_pair({1, 2}, {1});
      kb.add_pair({1, 4}, {1, 3});
      kb.add_pair({1, 5}, {1, 0});
      kb.add_pair({1, 6}, {1, 3});
      kb.add_pair({1, 7}, {1});
      kb.add_pair({1, 8}, {1, 3});
      kb.add_pair({1, 9}, {1, 3});
      kb.add_pair({1, 10}, {1, 0});
      kb.add_pair({3, 1}, {3});
      kb.add_pair({3, 2}, {3});
      kb.add_pair({3, 3}, {3});
      kb.add_pair({3, 4}, {3});
      kb.add_pair({3, 5}, {3, 0});
      kb.add_pair({3, 6}, {3});
      kb.add_pair({3, 7}, {3});
      kb.add_pair({3, 8}, {3});
      kb.add_pair({3, 9}, {3});
      kb.add_pair({3, 10}, {3, 0});
      kb.add_pair({0, 0, 0}, {0});
      kb.add_pair({0, 0, 1}, {1});
      kb.add_pair({0, 0, 3}, {3});
      kb.add_pair({0, 1, 3}, {1, 3});
      kb.add_pair({1, 0, 0}, {1});
      kb.add_pair({1, 0, 3}, {0, 3});
      kb.add_pair({3, 0, 0}, {3});
      kb.add_pair({0, 1, 0, 1}, {1, 0, 1});
      kb.add_pair({0, 3, 0, 3}, {3, 0, 3});
      kb.add_pair({1, 0, 1, 0}, {1, 0, 1});
      kb.add_pair({1, 3, 0, 1}, {1, 0, 1});
      kb.add_pair({1, 3, 0, 3}, {3, 0, 3});
      kb.add_pair({3, 0, 1, 0}, {3, 0, 1});
      kb.add_pair({3, 0, 3, 0}, {3, 0, 3});

      REQUIRE(kb.number_of_classes() == 16);
      REQUIRE(kb.word_to_class_index({0}) == kb.word_to_class_index({5}));
      REQUIRE(kb.word_to_class_index({0}) == kb.word_to_class_index({10}));
      REQUIRE(kb.word_to_class_index({1}) == kb.word_to_class_index({2}));
      REQUIRE(kb.word_to_class_index({1}) == kb.word_to_class_index({7}));
      REQUIRE(kb.word_to_class_index({3}) == kb.word_to_class_index({4}));
      REQUIRE(kb.word_to_class_index({3}) == kb.word_to_class_index({6}));
      REQUIRE(kb.word_to_class_index({3}) == kb.word_to_class_index({8}));
      REQUIRE(kb.word_to_class_index({3}) == kb.word_to_class_index({9}));
    }

    LIBSEMIGROUPS_TEST_CASE(
        "KnuthBendix",
        "107",
        "(cong) free semigroup congruence (240 classes)",
        "[no-valgrind][quick][congruence][knuth-bendix][cong]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_number_of_generators(2);
      kb.add_pair({0, 0, 0}, {0});
      kb.add_pair({1, 1, 1, 1}, {1});
      kb.add_pair({0, 1, 1, 1, 0}, {0, 0});
      kb.add_pair({1, 0, 0, 1}, {1, 1});
      kb.add_pair({0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0}, {0, 0});

      REQUIRE(kb.number_of_classes() == 240);
    }

    LIBSEMIGROUPS_TEST_CASE(
        "KnuthBendix",
        "108",
        "(cong) free semigroup congruence (240 classes)",
        "[no-valgrind][quick][congruence][knuth-bendix][cong]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_number_of_generators(2);
      kb.add_pair({0, 0, 0}, {0});
      kb.add_pair({1, 1, 1, 1}, {1});
      kb.add_pair({0, 1, 1, 1, 0}, {0, 0});
      kb.add_pair({1, 0, 0, 1}, {1, 1});
      kb.add_pair({0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0}, {0, 0});

      REQUIRE_NOTHROW(kb.knuth_bendix().froidure_pin());
      REQUIRE_THROWS_AS(kb.add_pair({0}, {1}), LibsemigroupsException);
    }

    LIBSEMIGROUPS_TEST_CASE(
        "KnuthBendix",
        "109",
        "(cong) less",
        "[no-valgrind][quick][congruence][knuth-bendix][cong]") {
      auto rg = ReportGuard(REPORT);
      {
        KnuthBendix kb;
        kb.set_number_of_generators(2);
        kb.add_pair({0, 0, 0}, {0});
        kb.add_pair({1, 1, 1, 1, 1}, {1});
        kb.add_pair({0, 1, 1, 1, 0, 1, 1}, {1, 1, 0});
        kb.run();

        REQUIRE(!kb.less({0, 1, 1, 1, 0, 0, 1, 1, 1, 0}, {1, 1, 1, 1, 0, 0}));
        REQUIRE(!kb.less({0, 1, 1, 0}, {0, 1, 1, 0}));
      }
      {
        KnuthBendix kb;
        kb.set_number_of_generators(2);
        kb.add_pair({0, 0, 0}, {0});
        kb.add_pair({1, 1, 1, 1, 1}, {1});
        kb.add_pair({0, 1, 1, 1, 0, 1, 1}, {1, 1, 0});

        REQUIRE(!kb.less({0, 1, 1, 1, 0, 0, 1, 1, 1, 0}, {1, 1, 1, 1, 0, 0}));
        REQUIRE(!kb.less({0, 1, 1, 0}, {0, 1, 1, 0}));
      }
    }

    LIBSEMIGROUPS_TEST_CASE(
        "KnuthBendix",
        "110",
        "(cong) less",
        "[quick][congruence][knuth-bendix][cong][no-valgrind]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_number_of_generators(2);
      kb.add_pair({0, 0, 0}, {0});
      kb.add_pair({1, 1, 1, 1, 1, 1, 1, 1, 1}, {1});
      kb.add_pair({0, 1, 1, 1, 1, 1, 0, 1, 1}, {1, 1, 0});

      REQUIRE(kb.less({0, 0, 0}, {1, 1, 1, 1, 1, 1, 1, 1, 1}));
    }

    LIBSEMIGROUPS_TEST_CASE(
        "KnuthBendix",
        "111",
        "(cong) constructors",
        "[quick][congruence][knuth-bendix][cong][no-valgrind]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_number_of_generators(2);
      kb.add_pair({0, 0, 0}, {0});
      kb.add_pair({1, 1, 1, 1, 1, 1, 1, 1, 1}, {1});
      kb.add_pair({0, 1, 1, 1, 1, 1, 0, 1, 1}, {1, 1, 0});
      REQUIRE(kb.number_of_classes() == 746);
      auto copy(kb);
      REQUIRE(copy.number_of_classes() == 746);
      REQUIRE(copy.number_of_generators() == 2);
      // the copy uses the "active rules" of kb, of which there are 105 since
      // knuth-bendix has already been run.
      REQUIRE(copy.number_of_generating_pairs() == 105);
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "112",
                            "(cong) const_contains/contains",
                            "[quick][congruence][knuth-bendix][cong]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_number_of_generators(2);
      kb.add_pair({0, 0, 0}, {0});
      kb.add_pair({1, 1, 1, 1}, {1});
      kb.add_pair({0, 1, 1, 1, 1, 1, 0, 1, 1}, {1, 1, 0});

      REQUIRE(kb.const_contains({0, 0, 0}, {1, 1, 1, 1, 1, 1, 1, 1, 1})
              == tril::unknown);
      REQUIRE(kb.const_contains({0, 0, 0}, {0, 0, 0}) == tril::TRUE);
      REQUIRE_THROWS_AS(kb.const_contains({0, 0, 2}, {0, 0, 0}),
                        LibsemigroupsException);
      REQUIRE(kb.const_contains({0, 0, 0}, {0}) == tril::TRUE);
      REQUIRE(kb.number_of_classes() == 12);
      REQUIRE(kb.const_contains({0, 0, 0}, {1, 1}) == tril::FALSE);
      REQUIRE(!kb.contains({0, 0, 0}, {1, 1}));
      REQUIRE(kb.contains({0, 0, 0}, {0, 0, 0}));
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "113",
                            "(cong) is_quotient_obviously_finite",
                            "[quick][congruence][knuth-bendix][cong]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_number_of_generators(2);
      kb.add_pair({0, 0, 0}, {0});
      kb.add_pair({1, 1, 1, 1}, {1});
      kb.add_pair({0, 1, 1, 1, 1, 1, 0, 1, 1}, {1, 1, 0});
      REQUIRE(!kb.is_quotient_obviously_finite());
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "114",
                            "(cong) class_index_to_word",
                            "[quick][congruence][knuth-bendix][cong]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_number_of_generators(2);
      kb.add_pair({0, 0, 0}, {0});
      kb.add_pair({1, 1, 1, 1}, {1});
      kb.add_pair({0, 1, 1, 1, 1, 1, 0, 1, 1}, {1, 1, 0});
      REQUIRE(kb.class_index_to_word(0) == word_type({0}));
      REQUIRE(kb.class_index_to_word(1) == word_type({1}));
      REQUIRE(kb.class_index_to_word(2) == word_type({0, 0}));
      REQUIRE(kb.class_index_to_word(3) == word_type({0, 1}));
      REQUIRE(kb.class_index_to_word(4) == word_type({1, 0}));
      REQUIRE(kb.class_index_to_word(5) == word_type({1, 1}));
      REQUIRE(kb.class_index_to_word(6) == word_type({0, 0, 1}));
      REQUIRE(kb.class_index_to_word(7) == word_type({0, 1, 1}));
      REQUIRE(kb.class_index_to_word(8) == word_type({1, 1, 1}));
      REQUIRE(kb.class_index_to_word(9) == word_type({0, 0, 1, 1}));
      REQUIRE(kb.class_index_to_word(10) == word_type({0, 1, 1, 1}));
      REQUIRE(kb.class_index_to_word(11) == word_type({0, 0, 1, 1, 1}));
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "115",
                            "(cong) quotient_froidure_pin",
                            "[quick][congruence][knuth-bendix][cong]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_number_of_generators(2);
      kb.add_pair({0, 0, 0}, {0});
      kb.add_pair({1, 1, 1, 1}, {1});
      kb.add_pair({0, 1, 1, 1, 1, 1, 0, 1, 1}, {1, 1, 0});
      REQUIRE(kb.quotient_froidure_pin()->size() == 12);
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "116",
                            "(cong) set_number_of_generators",
                            "[quick][congruence][knuth-bendix][cong]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      REQUIRE_NOTHROW(kb.set_number_of_generators(2));
      REQUIRE_THROWS_AS(kb.set_number_of_generators(3), LibsemigroupsException);
      REQUIRE_NOTHROW(kb.set_number_of_generators(2));
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "117",
                            "(cong) number of classes when obv-inf",
                            "[quick][congruence][knuth-bendix][cong]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_number_of_generators(3);
      kb.add_pair({0, 1}, {1, 0});
      kb.add_pair({0, 2}, {2, 0});
      kb.add_pair({0, 0}, {0});
      kb.add_pair({0, 2}, {0});
      kb.add_pair({2, 0}, {0});
      kb.add_pair({1, 1}, {1, 1});
      kb.add_pair({1, 2}, {2, 1});
      kb.add_pair({1, 1, 1}, {1});
      kb.add_pair({1, 2}, {1});
      kb.add_pair({2, 1}, {1});
      kb.add_pair({0}, {1});
      REQUIRE(kb.is_quotient_obviously_infinite());
      REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
    }
  }  // namespace congruence
}  // namespace libsemigroups
