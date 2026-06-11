//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2025-2026 James W. Swent
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

#include <array>    // for array
#include <cstddef>  // for size_t
#include <set>      // for set
#include <string>   // for string
#include <vector>   // for vector

#include "test-main.hpp"  // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/exception.hpp"   // for LibsemigroupsException
#include "libsemigroups/order.hpp"       // for wt_shortlex_compare*
#include "libsemigroups/types.hpp"       // for word_type
#include "libsemigroups/word-range.hpp"  // for operator ""_w

#include "libsemigroups/detail/fmt.hpp"     // for format
#include "libsemigroups/detail/report.hpp"  // for ReportGuard

namespace libsemigroups {

  int rec_compare(char const* w1, char const* w2) {
    int         lastmoved = 0;
    char const *p1, *p2;
    p1 = w1 + std::strlen(w1) - 1;
    p2 = w2 + std::strlen(w2) - 1;
    while (1) {
      if (p1 < w1) {
        if (p2 < w2)
          return lastmoved;
        return 2;
      }
      if (p2 < w2)
        return 1;
      if (*p1 == *p2) {
        p1--;
        p2--;
      } else if (*p1 < *p2) {
        p1--;
        lastmoved = 1;
      } else if (*p2 < *p1) {
        p2--;
        lastmoved = 2;
      }
    }
  }

  using namespace literals;

  // =========================================================================
  // Basic functionality - wt_shortlex_compare_no_checks
  // =========================================================================

  LIBSEMIGROUPS_TEST_CASE("wt_shortlex_compare_no_checks",
                          "000",
                          "different weights",
                          "[quick][order]") {
    auto                rg      = ReportGuard(false);
    std::vector<size_t> weights = {2, 1, 6, 3, 4};
    word_type           w1      = {0, 1};  // weight = 2 + 1 = 3
    word_type           w2      = {2};     // weight = 6

    REQUIRE(wt_shortlex_compare_no_checks(w1, w2, weights));
    REQUIRE(!wt_shortlex_compare_no_checks(w2, w1, weights));
  }

  LIBSEMIGROUPS_TEST_CASE("wt_shortlex_compare_no_checks",
                          "001",
                          "same weight, different length",
                          "[quick][order]") {
    auto                rg      = ReportGuard(false);
    std::vector<size_t> weights = {2, 1, 6, 3, 4};
    word_type           w1      = {0, 0, 0};  // weight = 2 + 2 + 2 = 6
    word_type           w2      = {2};        // weight = 6

    REQUIRE(wt_shortlex_compare_no_checks(w2, w1, weights));
    REQUIRE(!wt_shortlex_compare_no_checks(w1, w2, weights));
  }

  LIBSEMIGROUPS_TEST_CASE("wt_shortlex_compare_no_checks",
                          "002",
                          "same weight, same length, lexicographic",
                          "[quick][order]") {
    auto                rg      = ReportGuard(false);
    std::vector<size_t> weights = {2, 1, 6, 3, 4};
    word_type           w1      = {1, 1, 3};  // weight = 1 + 1 + 3 = 5
    word_type           w2      = {0, 0, 1};  // weight = 2 + 2 + 1 = 5

    REQUIRE(wt_shortlex_compare_no_checks(w2, w1, weights));
    REQUIRE(!wt_shortlex_compare_no_checks(w1, w2, weights));
  }

  LIBSEMIGROUPS_TEST_CASE("wt_shortlex_compare_no_checks",
                          "003",
                          "equal words",
                          "[quick][order]") {
    auto                rg      = ReportGuard(false);
    std::vector<size_t> weights = {2, 1, 6, 3, 4};
    word_type           w1      = {0, 1, 2};
    word_type           w2      = {0, 1, 2};

    REQUIRE(!wt_shortlex_compare_no_checks(w1, w2, weights));
    REQUIRE(!wt_shortlex_compare_no_checks(w2, w1, weights));
  }

  LIBSEMIGROUPS_TEST_CASE("wt_shortlex_compare_no_checks",
                          "004",
                          "empty word vs non-empty",
                          "[quick][order]") {
    auto                rg      = ReportGuard(false);
    std::vector<size_t> weights = {2, 1, 6, 3, 4};
    word_type           w1      = {};   // weight = 0
    word_type           w2      = {1};  // weight = 1

    REQUIRE(wt_shortlex_compare_no_checks(w1, w2, weights));
    REQUIRE(!wt_shortlex_compare_no_checks(w2, w1, weights));
  }

  // =========================================================================
  // Interface variants - iterators, pointers, structs
  // =========================================================================

  LIBSEMIGROUPS_TEST_CASE("wt_shortlex_compare_no_checks",
                          "005",
                          "iterator version",
                          "[quick][order]") {
    auto                rg      = ReportGuard(false);
    std::vector<size_t> weights = {2, 1, 6, 3, 4};
    word_type           w1      = {0, 1};  // weight = 3
    word_type           w2      = {2};     // weight = 6

    REQUIRE(wt_shortlex_compare_no_checks(
        w1.cbegin(), w1.cend(), w2.cbegin(), w2.cend(), weights));
    REQUIRE(!wt_shortlex_compare_no_checks(
        w2.cbegin(), w2.cend(), w1.cbegin(), w1.cend(), weights));
  }

  LIBSEMIGROUPS_TEST_CASE("wt_shortlex_compare_no_checks",
                          "006",
                          "pointer version",
                          "[quick][order]") {
    auto                rg      = ReportGuard(false);
    std::vector<size_t> weights = {2, 1, 6, 3, 4};
    word_type           w1      = {0, 1};  // weight = 3
    word_type           w2      = {2};     // weight = 6

    REQUIRE(wt_shortlex_compare_no_checks(&w1, &w2, weights));
    REQUIRE(!wt_shortlex_compare_no_checks(&w2, &w1, weights));
  }

  LIBSEMIGROUPS_TEST_CASE("WtShortLexCompare",
                          "007",
                          "struct with operator()",
                          "[quick][order]") {
    auto                rg      = ReportGuard(false);
    std::vector<size_t> weights = {2, 1, 6, 3, 4};
    WtShortLexCompare   comp(weights, WtShortLexCompare::no_checks);

    word_type w1 = {0, 1};  // weight = 3
    word_type w2 = {2};     // weight = 6

    REQUIRE(comp(w1, w2));
    REQUIRE(!comp(w2, w1));
  }

  LIBSEMIGROUPS_TEST_CASE("WtShortLexCompare",
                          "008",
                          "same weight fallback to shortlex",
                          "[quick][order]") {
    auto                rg      = ReportGuard(false);
    std::vector<size_t> weights = {1, 1, 1, 1, 1};
    WtShortLexCompare   comp(weights, WtShortLexCompare::no_checks);

    word_type w1 = {0};  // weight = 1
    word_type w2 = {1};  // weight = 1

    REQUIRE(comp(w1, w2));
    REQUIRE(!comp(w2, w1));
  }

  LIBSEMIGROUPS_TEST_CASE("WtShortLexCompare",
                          "009",
                          "complex example from documentation",
                          "[quick][order]") {
    auto                rg      = ReportGuard(false);
    std::vector<size_t> weights = {2, 1, 6, 3, 4};
    WtShortLexCompare   comp(weights, WtShortLexCompare::no_checks);

    word_type w1 = {1, 1, 1, 1};  // weight = 1 + 1 + 1 + 1 = 4
    word_type w2 = {3, 1};        // weight = 3 + 1 = 4
    word_type w3 = {0, 0};        // weight = 2 + 2 = 4
    word_type w4 = {4};           // weight = 4

    REQUIRE(comp(w4, w1));
    REQUIRE(comp(w4, w2));
    REQUIRE(comp(w4, w3));

    REQUIRE(comp(w3, w1));
    REQUIRE(comp(w3, w2));

    REQUIRE(comp(w2, w1));
  }

  // =========================================================================
  // Word format tests - std::string, std::array
  // =========================================================================

  LIBSEMIGROUPS_TEST_CASE("wt_shortlex_compare_no_checks",
                          "010",
                          "std::string format",
                          "[quick][order]") {
    auto                rg      = ReportGuard(false);
    std::vector<size_t> weights = {2, 1, 6, 3, 4};
    std::string         w1      = {0, 1};  // weight = 2 + 1 = 3
    std::string         w2      = {2};     // weight = 6

    REQUIRE(wt_shortlex_compare_no_checks(w1, w2, weights));
    REQUIRE(!wt_shortlex_compare_no_checks(w2, w1, weights));
  }

  LIBSEMIGROUPS_TEST_CASE("wt_shortlex_compare_no_checks",
                          "011",
                          "std::array format",
                          "[quick][order]") {
    auto                  rg      = ReportGuard(false);
    std::vector<size_t>   weights = {2, 1, 6, 3, 4};
    std::array<size_t, 2> w1      = {0, 1};  // weight = 2 + 1 = 3
    std::array<size_t, 1> w2      = {2};     // weight = 6

    // Use iterator version since arrays have different types
    REQUIRE(wt_shortlex_compare_no_checks(
        w1.cbegin(), w1.cend(), w2.cbegin(), w2.cend(), weights));
    REQUIRE(!wt_shortlex_compare_no_checks(
        w2.cbegin(), w2.cend(), w1.cbegin(), w1.cend(), weights));
  }

  LIBSEMIGROUPS_TEST_CASE("wt_shortlex_compare_no_checks",
                          "012",
                          "std::vector with different content",
                          "[quick][order]") {
    auto                rg      = ReportGuard(false);
    std::vector<size_t> weights = {2, 1, 6, 3, 4};
    std::vector<size_t> w1      = {0, 1, 1};  // weight = 2 + 1 + 1 = 4
    std::vector<size_t> w2      = {3, 1};     // weight = 3 + 1 = 4

    // Same weight, so falls back to shortlex (w2 < w1 because w2 is shorter)
    REQUIRE(!wt_shortlex_compare_no_checks(w1, w2, weights));
    REQUIRE(wt_shortlex_compare_no_checks(w2, w1, weights));
  }

  // =========================================================================
  // Edge cases
  // =========================================================================

  LIBSEMIGROUPS_TEST_CASE("wt_shortlex_compare_no_checks",
                          "013",
                          "uniform weights (shortlex)",
                          "[quick][order]") {
    auto                rg      = ReportGuard(false);
    std::vector<size_t> weights = {1, 1, 1, 1, 1};
    word_type           w1      = {0, 1};  // weight = 2
    word_type           w2      = {2, 3};  // weight = 2

    // Same weight and length, so pure shortlex: {0,1} < {2,3}
    REQUIRE(wt_shortlex_compare_no_checks(w1, w2, weights));
    REQUIRE(!wt_shortlex_compare_no_checks(w2, w1, weights));
  }

  LIBSEMIGROUPS_TEST_CASE("wt_shortlex_compare_no_checks",
                          "014",
                          "single letter alphabet",
                          "[quick][order]") {
    auto                rg      = ReportGuard(false);
    std::vector<size_t> weights = {5};
    word_type           w1      = {0};        // weight = 5
    word_type           w2      = {0, 0};     // weight = 10
    word_type           w3      = {0, 0, 0};  // weight = 15

    REQUIRE(wt_shortlex_compare_no_checks(w1, w2, weights));
    REQUIRE(wt_shortlex_compare_no_checks(w2, w3, weights));
    REQUIRE(wt_shortlex_compare_no_checks(w1, w3, weights));  // transitivity
  }

  LIBSEMIGROUPS_TEST_CASE("wt_shortlex_compare_no_checks",
                          "015",
                          "words with same prefix",
                          "[quick][order]") {
    auto                rg      = ReportGuard(false);
    std::vector<size_t> weights = {2, 1, 6, 3, 4};
    word_type           w1      = {0, 1, 2};     // weight = 2+1+6 = 9
    word_type           w2      = {0, 1, 2, 1};  // weight = 2+1+6+1 = 10

    REQUIRE(wt_shortlex_compare_no_checks(w1, w2, weights));
  }

  LIBSEMIGROUPS_TEST_CASE("wt_shortlex_compare_no_checks",
                          "016",
                          "repeated letters",
                          "[quick][order]") {
    auto                rg      = ReportGuard(false);
    std::vector<size_t> weights = {3, 2, 1};
    word_type           w1      = {2, 2, 2, 2};  // weight = 1+1+1+1 = 4
    word_type           w2      = {1, 1};        // weight = 2+2 = 4

    // Same weight (4), w2 is shorter so w2 < w1
    REQUIRE(!wt_shortlex_compare_no_checks(w1, w2, weights));
    REQUIRE(wt_shortlex_compare_no_checks(w2, w1, weights));
  }

  // =========================================================================
  // Mathematical properties
  // =========================================================================

  LIBSEMIGROUPS_TEST_CASE("wt_shortlex_compare_no_checks",
                          "017",
                          "irreflexivity: !(a < a)",
                          "[quick][order]") {
    auto                rg      = ReportGuard(false);
    std::vector<size_t> weights = {2, 1, 6, 3, 4};
    word_type           w       = {0, 1, 2, 3};

    REQUIRE(!wt_shortlex_compare_no_checks(w, w, weights));
  }

  LIBSEMIGROUPS_TEST_CASE("wt_shortlex_compare_no_checks",
                          "018",
                          "transitivity: a<b && b<c => a<c",
                          "[quick][order]") {
    auto                rg      = ReportGuard(false);
    std::vector<size_t> weights = {2, 1, 6, 3, 4};
    word_type           w1      = {1};     // weight = 1
    word_type           w2      = {0, 1};  // weight = 3
    word_type           w3      = {2};     // weight = 6

    REQUIRE(wt_shortlex_compare_no_checks(w1, w2, weights));
    REQUIRE(wt_shortlex_compare_no_checks(w2, w3, weights));
    REQUIRE(wt_shortlex_compare_no_checks(w1, w3, weights));  // transitivity
  }

  LIBSEMIGROUPS_TEST_CASE("wt_shortlex_compare_no_checks",
                          "019",
                          "antisymmetry: a<b => !(b<a)",
                          "[quick][order]") {
    auto                rg      = ReportGuard(false);
    std::vector<size_t> weights = {2, 1, 6, 3, 4};
    word_type           w1      = {1};  // weight = 1
    word_type           w2      = {2};  // weight = 6

    REQUIRE(wt_shortlex_compare_no_checks(w1, w2, weights));
    REQUIRE(!wt_shortlex_compare_no_checks(w2, w1, weights));
  }

  // =========================================================================
  // Validation tests - wt_shortlex_compare
  // =========================================================================

  LIBSEMIGROUPS_TEST_CASE("wt_shortlex_compare",
                          "020",
                          "valid letters with word_type",
                          "[quick][order]") {
    auto                rg      = ReportGuard(false);
    std::vector<size_t> weights = {2, 1, 6, 3, 4};
    word_type           w1      = {0, 1};  // weight = 3
    word_type           w2      = {2};     // weight = 6

    REQUIRE(wt_shortlex_compare(w1, w2, weights));
    REQUIRE(!wt_shortlex_compare(w2, w1, weights));
  }

  LIBSEMIGROUPS_TEST_CASE("wt_shortlex_compare",
                          "021",
                          "invalid letter throws exception",
                          "[quick][order]") {
    auto                rg      = ReportGuard(false);
    std::vector<size_t> weights = {2, 1, 6, 3, 4};  // alphabet size = 5
    word_type           w1      = {0, 1};
    word_type           w2      = {5};  // invalid: 5 >= weights.size()

    REQUIRE_THROWS_AS(wt_shortlex_compare(w1, w2, weights),
                      LibsemigroupsException);
    REQUIRE_NOTHROW(wt_shortlex_compare(w1, w1, weights));
  }

  LIBSEMIGROUPS_TEST_CASE("wt_shortlex_compare",
                          "022",
                          "std::string format with validation",
                          "[quick][order]") {
    auto                rg      = ReportGuard(false);
    std::vector<size_t> weights = {2, 1, 6, 3, 4};
    std::string         w1      = {0, 1};  // weight = 3
    std::string         w2      = {2};     // weight = 6

    REQUIRE(wt_shortlex_compare(w1, w2, weights));
    REQUIRE(!wt_shortlex_compare(w2, w1, weights));
  }

  LIBSEMIGROUPS_TEST_CASE("wt_shortlex_compare",
                          "023",
                          "iterator version with validation",
                          "[quick][order]") {
    auto                rg      = ReportGuard(false);
    std::vector<size_t> weights = {2, 1, 6, 3, 4};
    word_type           w1      = {0, 1};  // weight = 3
    word_type           w2      = {2};     // weight = 6

    REQUIRE(wt_shortlex_compare(
        w1.cbegin(), w1.cend(), w2.cbegin(), w2.cend(), weights));
    REQUIRE(!wt_shortlex_compare(
        w2.cbegin(), w2.cend(), w1.cbegin(), w1.cend(), weights));
  }

  LIBSEMIGROUPS_TEST_CASE("wt_shortlex_compare",
                          "024",
                          "pointer version with validation",
                          "[quick][order]") {
    auto                rg      = ReportGuard(false);
    std::vector<size_t> weights = {2, 1, 6, 3, 4};
    word_type           w1      = {0, 1};  // weight = 3
    word_type           w2      = {2};     // weight = 6

    REQUIRE(wt_shortlex_compare(&w1, &w2, weights));
    REQUIRE(!wt_shortlex_compare(&w2, &w1, weights));
  }

  LIBSEMIGROUPS_TEST_CASE("wt_shortlex_compare",
                          "025",
                          "both words invalid",
                          "[quick][order]") {
    auto                rg      = ReportGuard(false);
    std::vector<size_t> weights = {2, 1, 6};  // alphabet size = 3
    word_type           w1      = {5};        // invalid
    word_type           w2      = {10};       // invalid

    REQUIRE_THROWS_AS(wt_shortlex_compare(w1, w2, weights),
                      LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("wt_shortlex_compare",
                          "026",
                          "invalid letter in middle of word",
                          "[quick][order]") {
    auto                rg      = ReportGuard(false);
    std::vector<size_t> weights = {2, 1, 6};  // alphabet size = 3
    word_type           w1      = {0, 1, 2};  // valid
    word_type           w2      = {0, 5, 2};  // invalid in middle

    REQUIRE_THROWS_AS(wt_shortlex_compare(w1, w2, weights),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(wt_shortlex_compare(w2, w1, weights),
                      LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("wt_shortlex_compare",
                          "027",
                          "empty weights vector",
                          "[quick][order]") {
    auto                rg      = ReportGuard(false);
    std::vector<size_t> weights = {};   // empty alphabet
    word_type           w1      = {};   // empty word is valid
    word_type           w2      = {0};  // invalid: no letters in alphabet

    REQUIRE_NOTHROW(wt_shortlex_compare(w1, w1, weights));
    REQUIRE_THROWS_AS(wt_shortlex_compare(w2, w1, weights),
                      LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("WtShortLexCompare",
                          "028",
                          "struct with validation enabled",
                          "[quick][order]") {
    auto                rg      = ReportGuard(false);
    std::vector<size_t> weights = {2, 1, 6, 3, 4};
    WtShortLexCompare   comp(weights, WtShortLexCompare::checks);

    word_type w1 = {0, 1};  // weight = 3
    word_type w2 = {2};     // weight = 6

    REQUIRE(comp(w1, w2));
    REQUIRE(!comp(w2, w1));
  }

  LIBSEMIGROUPS_TEST_CASE("WtShortLexCompare",
                          "029",
                          "struct throws on invalid letter with checks enabled",
                          "[quick][order]") {
    auto                rg      = ReportGuard(false);
    std::vector<size_t> weights = {2, 1, 6, 3, 4};  // alphabet size = 5
    WtShortLexCompare   comp(weights, WtShortLexCompare::checks);

    word_type w1 = {0, 1};
    word_type w2 = {10};  // invalid: 10 >= weights.size()

    REQUIRE_THROWS_AS(comp(w1, w2), LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("WtShortLexCompare",
                          "030",
                          "call_no_checks always validates",
                          "[quick][order]") {
    auto                rg      = ReportGuard(false);
    std::vector<size_t> weights = {2, 1, 6, 3, 4};
    WtShortLexCompare   comp(weights, WtShortLexCompare::no_checks);

    word_type w1 = {0, 1};  // weight = 3
    word_type w2 = {2};     // weight = 6

    REQUIRE(comp.call_no_checks(w1, w2));
    REQUIRE(!comp.call_no_checks(w2, w1));
  }

  LIBSEMIGROUPS_TEST_CASE("WtShortLexCompare",
                          "034",
                          "call_no_checks throws on invalid letter",
                          "[quick][order]") {
    auto                rg      = ReportGuard(false);
    std::vector<size_t> weights = {2, 1, 6, 3, 4};  // alphabet size = 5
    WtShortLexCompare   comp(weights, WtShortLexCompare::checks);

    word_type w1 = {0, 1};
    word_type w2 = {10};  // invalid: 10 >= weights.size()

    // constructor
    REQUIRE_THROWS_AS(comp(w1, w2), LibsemigroupsException);
  }

  // =========================================================================
  // STL integration
  // =========================================================================

  LIBSEMIGROUPS_TEST_CASE("WtShortLexCompare",
                          "031",
                          "use in std::set",
                          "[quick][order]") {
    auto                rg      = ReportGuard(false);
    std::vector<size_t> weights = {3, 2, 1};
    WtShortLexCompare   comp(weights, WtShortLexCompare::no_checks);

    std::set<word_type, WtShortLexCompare> ordered_words(comp);
    ordered_words.insert({2});     // weight = 1
    ordered_words.insert({1, 1});  // weight = 4
    ordered_words.insert({0, 2});  // weight = 4
    ordered_words.insert({0});     // weight = 3

    // Should be ordered: {2}, {0}, {0,2}, {1,1}
    auto it = ordered_words.begin();
    REQUIRE(*it++ == word_type{2});
    REQUIRE(*it++ == word_type{0});
    REQUIRE(*it++ == word_type{0, 2});
    REQUIRE(*it++ == word_type{1, 1});
  }

  // =========================================================================
  // Template tests for multiple word types
  // =========================================================================

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("wt_shortlex_compare_no_checks",
                                   "032",
                                   "multiple word types",
                                   "[quick][order]",
                                   word_type,
                                   std::string) {
    auto rg                     = ReportGuard(false);
    using W                     = TestType;
    std::vector<size_t> weights = {2, 1, 6, 3, 4};
    W                   w1      = {0, 1};  // weight = 3
    W                   w2      = {2};     // weight = 6

    REQUIRE(wt_shortlex_compare_no_checks(w1, w2, weights));
    REQUIRE(!wt_shortlex_compare_no_checks(w2, w1, weights));
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("wt_shortlex_compare",
                                   "033",
                                   "validation with multiple word types",
                                   "[quick][order]",
                                   word_type,
                                   std::string) {
    auto rg                     = ReportGuard(false);
    using W                     = TestType;
    std::vector<size_t> weights = {2, 1, 6};
    W                   w1      = {0, 1};  // valid
    W                   w2      = {5};     // invalid

    REQUIRE_NOTHROW(wt_shortlex_compare(w1, w1, weights));
    REQUIRE_THROWS_AS(wt_shortlex_compare(w1, w2, weights),
                      LibsemigroupsException);
  }

  // =========================================================================
  // Recursive Path Compare
  // =========================================================================

  LIBSEMIGROUPS_TEST_CASE("rev_rpo_cmp",
                          "034",
                          "empty word",
                          "[quick][order]") {
    auto      rg = ReportGuard(false);
    word_type w1(12_w);
    word_type w2{};

    REQUIRE(!rev_rpo_cmp(w1, w1));
    REQUIRE(rev_rpo_cmp(w2, w1));
    REQUIRE(!rev_rpo_cmp(w1, w2));
  }

  LIBSEMIGROUPS_TEST_CASE("RevRPOCmp", "035", "empty word", "[quick][order]") {
    auto      rg = ReportGuard(false);
    word_type w1(12_w);
    word_type w2{};

    REQUIRE(!RevRPOCmp()(w1, w1));
    REQUIRE(RevRPOCmp()(w2, w1));
    REQUIRE(!RevRPOCmp()(w1, w2));
  }

  LIBSEMIGROUPS_TEST_CASE("rpo_cmp",
                          "036",
                          "random examples",
                          "[quick][order]") {
    using std::literals::string_literals::operator""s;
    REQUIRE(!rpo_cmp(""s, ""s));
    REQUIRE(!rpo_cmp("a"s, ""s));
    REQUIRE(!rpo_cmp("b"s, ""s));
    REQUIRE(!rpo_cmp("c"s, ""s));
    REQUIRE(!rpo_cmp("c"s, "c"s));
    REQUIRE(!rpo_cmp("ab"s, ""s));
    REQUIRE(!rpo_cmp("caa"s, ""s));
    REQUIRE(!rpo_cmp("cba"s, ""s));
    REQUIRE(!rpo_cmp("cc"s, "ab"s));
    REQUIRE(!rpo_cmp("cc"s, "ba"s));
    REQUIRE(!rpo_cmp("cccb"s, ""s));
    REQUIRE(!rpo_cmp("ca"s, "aab"s));
    REQUIRE(!rpo_cmp("caa"s, "bb"s));
    REQUIRE(!rpo_cmp("cca"s, "bb"s));
    REQUIRE(!rpo_cmp("aaca"s, "c"s));
    REQUIRE(!rpo_cmp("acbb"s, "b"s));
    REQUIRE(!rpo_cmp("abcbb"s, ""s));
    REQUIRE(!rpo_cmp("cbcca"s, ""s));
    REQUIRE(!rpo_cmp("bcc"s, "aca"s));
    REQUIRE(!rpo_cmp("cbc"s, "bca"s));
    REQUIRE(!rpo_cmp("ccb"s, "aab"s));
    REQUIRE(!rpo_cmp("aacaa"s, "b"s));
    REQUIRE(!rpo_cmp("cabbb"s, "a"s));
    REQUIRE(!rpo_cmp("cbbab"s, "a"s));
    REQUIRE(!rpo_cmp("acabac"s, ""s));
    REQUIRE(!rpo_cmp("bbcbac"s, ""s));
    REQUIRE(!rpo_cmp("bcabcb"s, ""s));
    REQUIRE(!rpo_cmp("cabcaa"s, ""s));
    REQUIRE(!rpo_cmp("cbbbac"s, ""s));
    REQUIRE(!rpo_cmp("bcab"s, "bbc"s));
    REQUIRE(!rpo_cmp("aaccc"s, "ab"s));
    REQUIRE(!rpo_cmp("aabcbbc"s, ""s));
    REQUIRE(!rpo_cmp("caba"s, "aaac"s));
    REQUIRE(!rpo_cmp("accac"s, "ccb"s));
    REQUIRE(!rpo_cmp("ccaaab"s, "ac"s));
    REQUIRE(!rpo_cmp("bcaabac"s, "c"s));
    REQUIRE(!rpo_cmp("aabaacbb"s, ""s));
    REQUIRE(!rpo_cmp("bcccbabb"s, ""s));
    REQUIRE(!rpo_cmp("bbcc"s, "aaaab"s));
    REQUIRE(!rpo_cmp("acccc"s, "bbca"s));
    REQUIRE(!rpo_cmp("bcbcc"s, "caaa"s));
    REQUIRE(!rpo_cmp("cccaab"s, "aac"s));
    REQUIRE(!rpo_cmp("acabbba"s, "ac"s));
    REQUIRE(!rpo_cmp("bcabbbc"s, "bc"s));
    REQUIRE(!rpo_cmp("caaccbcb"s, "c"s));
    REQUIRE(!rpo_cmp("cc"s, "bbacabbb"s));
    REQUIRE(!rpo_cmp("acc"s, "aabcbba"s));
    REQUIRE(!rpo_cmp("acaa"s, "aababc"s));
    REQUIRE(!rpo_cmp("acaa"s, "abbabb"s));
    REQUIRE(!rpo_cmp("cbcaacca"s, "ac"s));
    REQUIRE(!rpo_cmp("caaccccaa"s, "a"s));
    REQUIRE(!rpo_cmp("aabaabcabc"s, ""s));
    REQUIRE(!rpo_cmp("abbbacacac"s, ""s));
    REQUIRE(!rpo_cmp("cbacbbaaca"s, ""s));
    REQUIRE(!rpo_cmp("bbbc"s, "abbaaba"s));
    REQUIRE(!rpo_cmp("bccbaa"s, "cabbc"s));
    REQUIRE(!rpo_cmp("abaacaaabb"s, "b"s));
    REQUIRE(!rpo_cmp("cbaacbbbbc"s, "b"s));
    REQUIRE(!rpo_cmp("ccaabc"s, "abcaaa"s));
    REQUIRE(!rpo_cmp("ccacca"s, "baaccc"s));
    REQUIRE(!rpo_cmp("aacbccc"s, "bbacc"s));
    REQUIRE(!rpo_cmp("bcacbbc"s, "bacab"s));
    REQUIRE(!rpo_cmp("cacbbac"s, "bcbac"s));
    REQUIRE(!rpo_cmp("cccacbb"s, "cccac"s));
    REQUIRE(!rpo_cmp("cbabaabb"s, "bbbb"s));
    REQUIRE(!rpo_cmp("cbacbcba"s, "baab"s));
    REQUIRE(!rpo_cmp("abcacacbb"s, "bab"s));
    REQUIRE(!rpo_cmp("abacbbcccc"s, "ac"s));
    REQUIRE(!rpo_cmp("bcacbbbccb"s, "ca"s));
    REQUIRE(!rpo_cmp("caccbbacbc"s, "ab"s));
    REQUIRE(!rpo_cmp("accaa"s, "aabaacac"s));
    REQUIRE(!rpo_cmp("acacbaa"s, "bcbabc"s));
    REQUIRE(!rpo_cmp("acbcbaca"s, "aabcb"s));
    REQUIRE(!rpo_cmp("abcbaabaa"s, "abcb"s));
    REQUIRE(!rpo_cmp("bbcacabca"s, "caaa"s));
    REQUIRE(!rpo_cmp("bcaababaa"s, "babb"s));
    REQUIRE(!rpo_cmp("cbbcbcbacc"s, "bcb"s));
    REQUIRE(!rpo_cmp("cccbbccccb"s, "aaa"s));
    REQUIRE(!rpo_cmp("acccaa"s, "cacbbaca"s));
    REQUIRE(!rpo_cmp("baccbcba"s, "ccabab"s));
    REQUIRE(!rpo_cmp("caaccbab"s, "babbcb"s));
    REQUIRE(!rpo_cmp("abcacaaab"s, "cabac"s));
    REQUIRE(!rpo_cmp("baabccccb"s, "caacb"s));
    REQUIRE(!rpo_cmp("aaabccaabc"s, "bbcc"s));
    REQUIRE(!rpo_cmp("acccba"s, "bbcabccaa"s));
    REQUIRE(!rpo_cmp("bcacac"s, "bcbaaacba"s));
    REQUIRE(!rpo_cmp("accbcacc"s, "abbbcba"s));
    REQUIRE(!rpo_cmp("ccaccacc"s, "bcbabcb"s));
    REQUIRE(!rpo_cmp("ccacac"s, "acbaacbaba"s));
    REQUIRE(!rpo_cmp("aaaccbcb"s, "acabacca"s));
    REQUIRE(!rpo_cmp("cbbbbcab"s, "babbabca"s));
    REQUIRE(!rpo_cmp("ccabaaacab"s, "aabaaa"s));
    REQUIRE(!rpo_cmp("cbccbcb"s, "aaccbbaaab"s));
    REQUIRE(!rpo_cmp("bccaccbca"s, "cacccbba"s));
    REQUIRE(!rpo_cmp("aabcacbacc"s, "bccbbca"s));
    REQUIRE(!rpo_cmp("cbcacacbab"s, "caccbaa"s));
    REQUIRE(!rpo_cmp("acacbbacab"s, "abbbccba"s));
    REQUIRE(!rpo_cmp("bcbabcacac"s, "ccababcc"s));
    REQUIRE(!rpo_cmp("abacccaaab"s, "aaaccacab"s));
    REQUIRE(!rpo_cmp("bccccbbbbc"s, "bbabccbcbb"s));

    REQUIRE(rpo_cmp(""s, "a"s));
    REQUIRE(rpo_cmp(""s, "b"s));
    REQUIRE(rpo_cmp(""s, "c"s));
    REQUIRE(rpo_cmp(""s, "ba"s));
    REQUIRE(rpo_cmp(""s, "bca"s));
    REQUIRE(rpo_cmp("a"s, "bc"s));
    REQUIRE(rpo_cmp("a"s, "cc"s));
    REQUIRE(rpo_cmp("bb"s, "bc"s));
    REQUIRE(rpo_cmp(""s, "bbbcc"s));
    REQUIRE(rpo_cmp("a"s, "aabc"s));
    REQUIRE(rpo_cmp("c"s, "ccaa"s));
    REQUIRE(rpo_cmp("ab"s, "cca"s));
    REQUIRE(rpo_cmp("baa"s, "bc"s));
    REQUIRE(rpo_cmp("abab"s, "c"s));
    REQUIRE(rpo_cmp("baaa"s, "c"s));
    REQUIRE(rpo_cmp(""s, "accabc"s));
    REQUIRE(rpo_cmp(""s, "bccbcc"s));
    REQUIRE(rpo_cmp(""s, "cbabcc"s));
    REQUIRE(rpo_cmp("a"s, "acbba"s));
    REQUIRE(rpo_cmp("c"s, "bbbac"s));
    REQUIRE(rpo_cmp("c"s, "ccbbc"s));
    REQUIRE(rpo_cmp("ab"s, "acaa"s));
    REQUIRE(rpo_cmp("ab"s, "baab"s));
    REQUIRE(rpo_cmp("bb"s, "bcba"s));
    REQUIRE(rpo_cmp("aac"s, "acb"s));
    REQUIRE(rpo_cmp("abb"s, "bba"s));
    REQUIRE(rpo_cmp(""s, "cabaacc"s));
    REQUIRE(rpo_cmp("b"s, "ccaacb"s));
    REQUIRE(rpo_cmp("bb"s, "acaac"s));
    REQUIRE(rpo_cmp("bcb"s, "ccca"s));
    REQUIRE(rpo_cmp(""s, "aacaacba"s));
    REQUIRE(rpo_cmp(""s, "bacbaaba"s));
    REQUIRE(rpo_cmp(""s, "bbbaacca"s));
    REQUIRE(rpo_cmp(""s, "cbacbbab"s));
    REQUIRE(rpo_cmp(""s, "cbccccba"s));
    REQUIRE(rpo_cmp("a"s, "bbababb"s));
    REQUIRE(rpo_cmp("ab"s, "ccaabc"s));
    REQUIRE(rpo_cmp("bb"s, "bacaaa"s));
    REQUIRE(rpo_cmp("bc"s, "cbcaac"s));
    REQUIRE(rpo_cmp("bbca"s, "acbb"s));
    REQUIRE(rpo_cmp(""s, "abcacbbcb"s));
    REQUIRE(rpo_cmp(""s, "cacabaaca"s));
    REQUIRE(rpo_cmp("b"s, "bcabbcaa"s));
    REQUIRE(rpo_cmp("b"s, "cbcbaacb"s));
    REQUIRE(rpo_cmp("abc"s, "cbabac"s));
    REQUIRE(rpo_cmp("cac"s, "cabcaa"s));
    REQUIRE(rpo_cmp("cbc"s, "cabcba"s));
    REQUIRE(rpo_cmp("aacb"s, "caaca"s));
    REQUIRE(rpo_cmp("baab"s, "aaacc"s));
    REQUIRE(rpo_cmp("caab"s, "bccaa"s));
    REQUIRE(rpo_cmp("abbbaa"s, "cca"s));
    REQUIRE(rpo_cmp(""s, "babbcbabaa"s));
    REQUIRE(rpo_cmp(""s, "cabacacabb"s));
    REQUIRE(rpo_cmp("c"s, "babaabaca"s));
    REQUIRE(rpo_cmp("bc"s, "acbbccaa"s));
    REQUIRE(rpo_cmp("aba"s, "aacaacc"s));
    REQUIRE(rpo_cmp("abc"s, "bbabbcb"s));
    REQUIRE(rpo_cmp("cbb"s, "ccbaabc"s));
    REQUIRE(rpo_cmp("bbca"s, "babacc"s));
    REQUIRE(rpo_cmp("bbabc"s, "bcabb"s));
    REQUIRE(rpo_cmp("bcabba"s, "cbbb"s));
    REQUIRE(rpo_cmp("cabaac"s, "accb"s));
    REQUIRE(rpo_cmp("c"s, "cbccbcabba"s));
    REQUIRE(rpo_cmp("ab"s, "cbacabbcc"s));
    REQUIRE(rpo_cmp("aac"s, "aaccbbac"s));
    REQUIRE(rpo_cmp("aca"s, "aabccccc"s));
    REQUIRE(rpo_cmp("babaa"s, "ccacbb"s));
    REQUIRE(rpo_cmp("bacbaca"s, "ccac"s));
    REQUIRE(rpo_cmp("aa"s, "cbbbbbcbca"s));
    REQUIRE(rpo_cmp("bc"s, "bccbcaaaca"s));
    REQUIRE(rpo_cmp("cbcbb"s, "cbbcacc"s));
    REQUIRE(rpo_cmp("babbbabc"s, "ccaa"s));
    REQUIRE(rpo_cmp("aaa"s, "cccbccaabc"s));
    REQUIRE(rpo_cmp("cbcc"s, "cccbaacca"s));
    REQUIRE(rpo_cmp("bbcbaa"s, "bcbcbcc"s));
    REQUIRE(rpo_cmp("cbcbbb"s, "bccacca"s));
    REQUIRE(rpo_cmp("bcbc"s, "aabbcccccb"s));
    REQUIRE(rpo_cmp("caaa"s, "acbabcbbab"s));
    REQUIRE(rpo_cmp("ccac"s, "abccaabcaa"s));
    REQUIRE(rpo_cmp("aabbb"s, "baccbcccc"s));
    REQUIRE(rpo_cmp("abbcab"s, "cabccaca"s));
    REQUIRE(rpo_cmp("aabbbbacb"s, "aaccc"s));
    REQUIRE(rpo_cmp("bbbcabbbc"s, "ccbab"s));
    REQUIRE(rpo_cmp("cbabbabaac"s, "bcca"s));
    REQUIRE(rpo_cmp("aacca"s, "cacccccbbb"s));
    REQUIRE(rpo_cmp("baaca"s, "abaaaabcca"s));
    REQUIRE(rpo_cmp("bbaab"s, "bbccaaacba"s));
    REQUIRE(rpo_cmp("cabbcb"s, "aaacbcbaa"s));
    REQUIRE(rpo_cmp("cabaccc"s, "acbcbcbc"s));
    REQUIRE(rpo_cmp("acaabc"s, "bcaccccbbc"s));
    REQUIRE(rpo_cmp("acccca"s, "cbacacacca"s));
    REQUIRE(rpo_cmp("aaacabcc"s, "cbcbbbbc"s));
    REQUIRE(rpo_cmp("aacacacc"s, "baccacbc"s));
    REQUIRE(rpo_cmp("ccbaabba"s, "ccbabcca"s));
    REQUIRE(rpo_cmp("cabacbcbb"s, "bcbcccc"s));
    REQUIRE(rpo_cmp("acbacacb"s, "cbaacbccb"s));
    REQUIRE(rpo_cmp("bcbbbbbb"s, "bacababcb"s));
    REQUIRE(rpo_cmp("ccaababc"s, "aacbccbab"s));
    REQUIRE(rpo_cmp("aaaacbacba"s, "ccbbacb"s));
    REQUIRE(rpo_cmp("bcbabcbbc"s, "acbbabcacb"s));
  }

}  // namespace libsemigroups
