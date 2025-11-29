//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019-2025 James D. Mitchell
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

#include <array>   // for array
#include <set>     // for set
#include <string>  // for string
#include <vector>  // for vector

#include "Catch2-3.8.0/catch_amalgamated.hpp"  // for REQUIRE
#include "test-main.hpp"                       // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/detail/report.hpp"  // for ReportGuard
#include "libsemigroups/exception.hpp"      // for LibsemigroupsException
#include "libsemigroups/order.hpp"          // for wt_shortlex_compare*
#include "libsemigroups/types.hpp"          // for word_type
#include "libsemigroups/word-range.hpp"     // for operator ""_w

namespace libsemigroups {

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
    WtShortLexCompare   comp(weights, WtShortLexCompare::noCheck);

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
    WtShortLexCompare   comp(weights, WtShortLexCompare::noCheck);

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
    WtShortLexCompare   comp(weights, WtShortLexCompare::noCheck);

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
    WtShortLexCompare   comp(weights, WtShortLexCompare::doCheck);

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
    WtShortLexCompare   comp(weights, WtShortLexCompare::doCheck);

    word_type w1 = {0, 1};
    word_type w2 = {10};  // invalid: 10 >= weights.size()

    REQUIRE_THROWS_AS(comp(w1, w2), LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("WtShortLexCompare",
                          "030",
                          "call_checks always validates",
                          "[quick][order]") {
    auto                rg      = ReportGuard(false);
    std::vector<size_t> weights = {2, 1, 6, 3, 4};
    WtShortLexCompare   comp(weights, WtShortLexCompare::noCheck);

    word_type w1 = {0, 1};  // weight = 3
    word_type w2 = {2};     // weight = 6

    REQUIRE(comp.call_checks(w1, w2));
    REQUIRE(!comp.call_checks(w2, w1));
  }

  LIBSEMIGROUPS_TEST_CASE("WtShortLexCompare",
                          "034",
                          "call_checks throws on invalid letter",
                          "[quick][order]") {
    auto                rg      = ReportGuard(false);
    std::vector<size_t> weights = {2, 1, 6, 3, 4};  // alphabet size = 5
    WtShortLexCompare   comp(weights, WtShortLexCompare::noCheck);

    word_type w1 = {0, 1};
    word_type w2 = {10};  // invalid: 10 >= weights.size()

    // call_checks should throw even though checks are disabled in constructor
    REQUIRE_THROWS_AS(comp.call_checks(w1, w2), LibsemigroupsException);
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
    WtShortLexCompare   comp(weights, WtShortLexCompare::noCheck);

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

  LIBSEMIGROUPS_TEST_CASE("recursive_path_compare",
                          "034",
                          "empty word",
                          "[quick][order]") {
    word_type w1(12_w);
    word_type w2{};

    REQUIRE(!recursive_path_compare(w1, w1));
    REQUIRE(recursive_path_compare(w2, w1));
    REQUIRE(!recursive_path_compare(w1, w2));
  }

  LIBSEMIGROUPS_TEST_CASE("RecursivePathCompare",
                          "035",
                          "empty word",
                          "[quick][order]") {
    word_type w1(12_w);
    word_type w2{};

    REQUIRE(!RecursivePathCompare{}(w1, w1));
    REQUIRE(RecursivePathCompare{}(w2, w1));
    REQUIRE(!RecursivePathCompare{}(w1, w2));
  }
}  // namespace libsemigroups
