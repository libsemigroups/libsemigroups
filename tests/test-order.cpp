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

#include <algorithm>  // for is_sorted
#include <array>      // for array
#include <cstddef>    // for size_t
#include <set>        // for set
#include <string>     // for string
#include <utility>    // for move
#include <vector>     // for vector

#include "libsemigroups/detail/rewriting-system.hpp"
#include "test-main.hpp"  // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/exception.hpp"   // for LibsemigroupsException
#include "libsemigroups/order.hpp"       // for wt_lenlex_cmp*
#include "libsemigroups/types.hpp"       // for word_type
#include "libsemigroups/word-range.hpp"  // for operator ""_w

#include "libsemigroups/detail/fmt.hpp"     // for format
#include "libsemigroups/detail/report.hpp"  // for ReportGuard

namespace libsemigroups {

  using namespace literals;

  // =========================================================================
  // Basic functionality - wt_lenlex_cmp_no_checks
  // =========================================================================

  LIBSEMIGROUPS_TEST_CASE("wt_lenlex_cmp_no_checks",
                          "000",
                          "different weights",
                          "[quick][order]") {
    auto                rg      = ReportGuard(false);
    std::vector<size_t> weights = {2, 1, 6, 3, 4};
    word_type           w1      = {0, 1};  // weight = 2 + 1 = 3
    word_type           w2      = {2};     // weight = 6

    REQUIRE(wt_lenlex_cmp_no_checks(weights, w1, w2));
    REQUIRE(!wt_lenlex_cmp_no_checks(weights, w2, w1));
  }

  LIBSEMIGROUPS_TEST_CASE("wt_lenlex_cmp_no_checks",
                          "001",
                          "same weight, different length",
                          "[quick][order]") {
    auto                rg      = ReportGuard(false);
    std::vector<size_t> weights = {2, 1, 6, 3, 4};
    word_type           w1      = {0, 0, 0};  // weight = 2 + 2 + 2 = 6
    word_type           w2      = {2};        // weight = 6

    REQUIRE(wt_lenlex_cmp_no_checks(weights, w2, w1));
    REQUIRE(!wt_lenlex_cmp_no_checks(weights, w1, w2));
  }

  LIBSEMIGROUPS_TEST_CASE("wt_lenlex_cmp_no_checks",
                          "002",
                          "same weight, same length, lexicographic",
                          "[quick][order]") {
    auto                rg      = ReportGuard(false);
    std::vector<size_t> weights = {2, 1, 6, 3, 4};
    word_type           w1      = {1, 1, 3};  // weight = 1 + 1 + 3 = 5
    word_type           w2      = {0, 0, 1};  // weight = 2 + 2 + 1 = 5

    REQUIRE(wt_lenlex_cmp_no_checks(weights, w2, w1));
    REQUIRE(!wt_lenlex_cmp_no_checks(weights, w1, w2));
  }

  LIBSEMIGROUPS_TEST_CASE("wt_lenlex_cmp_no_checks",
                          "003",
                          "equal words",
                          "[quick][order]") {
    auto                rg      = ReportGuard(false);
    std::vector<size_t> weights = {2, 1, 6, 3, 4};
    word_type           w1      = {0, 1, 2};
    word_type           w2      = {0, 1, 2};

    REQUIRE(!wt_lenlex_cmp_no_checks(weights, w1, w2));
    REQUIRE(!wt_lenlex_cmp_no_checks(weights, w2, w1));
  }

  LIBSEMIGROUPS_TEST_CASE("wt_lenlex_cmp_no_checks",
                          "004",
                          "empty word vs non-empty",
                          "[quick][order]") {
    auto                rg      = ReportGuard(false);
    std::vector<size_t> weights = {2, 1, 6, 3, 4};
    word_type           w1      = {};   // weight = 0
    word_type           w2      = {1};  // weight = 1

    REQUIRE(wt_lenlex_cmp_no_checks(weights, w1, w2));
    REQUIRE(!wt_lenlex_cmp_no_checks(weights, w2, w1));
  }

  // =========================================================================
  // Interface variants - iterators, structs
  // =========================================================================

  LIBSEMIGROUPS_TEST_CASE("wt_lenlex_cmp_no_checks",
                          "005",
                          "iterator version",
                          "[quick][order]") {
    auto                rg      = ReportGuard(false);
    std::vector<size_t> weights = {2, 1, 6, 3, 4};
    word_type           w1      = {0, 1};  // weight = 3
    word_type           w2      = {2};     // weight = 6

    REQUIRE(wt_lenlex_cmp_no_checks(
        weights, w1.cbegin(), w1.cend(), w2.cbegin(), w2.cend()));
    REQUIRE(!wt_lenlex_cmp_no_checks(
        weights, w2.cbegin(), w2.cend(), w1.cbegin(), w1.cend()));
  }

  LIBSEMIGROUPS_TEST_CASE("wt_lenlex_cmp_no_checks",
                          "006",
                          "with alphabet",
                          "[quick][order]") {
    using std::string_literals::operator""s;

    auto                rg = ReportGuard(false);
    Alphabet            alphabet("ba"s);
    std::vector<size_t> weights       = {10, 1};
    std::vector<size_t> equal_weights = {1, 1};
    auto                a             = "a"s;
    auto                b             = "b"s;

    REQUIRE(wt_lenlex_cmp_no_checks(alphabet, weights, a, b));
    REQUIRE(!wt_lenlex_cmp_no_checks(alphabet, weights, b, a));

    REQUIRE(wt_lenlex_cmp_no_checks(alphabet, equal_weights, b, a));
    REQUIRE(!wt_lenlex_cmp_no_checks(alphabet, equal_weights, a, b));
    REQUIRE(
        wt_lenlex_cmp_no_checks(
            alphabet, equal_weights, b.cbegin(), b.cend(), a.cbegin(), a.cend())
        == wt_lenlex_cmp_no_checks(alphabet, equal_weights, b, a));
  }

  LIBSEMIGROUPS_TEST_CASE("WtLenLexCmp",
                          "007",
                          "struct with alphabet",
                          "[quick][order]") {
    using std::string_literals::operator""s;

    auto                rg = ReportGuard(false);
    Alphabet            alphabet("ba"s);
    std::vector<size_t> weights       = {10, 1};
    std::vector<size_t> equal_weights = {1, 1};
    auto                a             = "a"s;
    auto                b             = "b"s;

    WtLenLexCmp comp(alphabet, weights);
    REQUIRE(comp(a, b));
    REQUIRE(!comp(b, a));

    comp.init(alphabet, equal_weights);
    REQUIRE(comp(b, a));
    REQUIRE(!comp(a, b));
    REQUIRE(comp(b.cbegin(), b.cend(), a.cbegin(), a.cend()));

    WtLenLexCmp moved(Alphabet("ba"s), std::vector<size_t>{1, 1});
    REQUIRE(moved(b, a));
  }

  LIBSEMIGROUPS_TEST_CASE("WtLenLexCmp",
                          "008",
                          "struct with operator()",
                          "[quick][order]") {
    auto                rg      = ReportGuard(false);
    std::vector<size_t> weights = {2, 1, 6, 3, 4};
    WtLenLexCmp         comp(weights);

    word_type w1 = {0, 1};  // weight = 3
    word_type w2 = {2};     // weight = 6

    REQUIRE(comp(w1, w2));
    REQUIRE(!comp(w2, w1));
  }

  LIBSEMIGROUPS_TEST_CASE("WtLenLexCmp",
                          "009",
                          "same weight fallback to shortlex",
                          "[quick][order]") {
    auto                rg      = ReportGuard(false);
    std::vector<size_t> weights = {1, 1, 1, 1, 1};
    WtLenLexCmp         comp(weights);

    word_type w1 = {0};  // weight = 1
    word_type w2 = {1};  // weight = 1

    REQUIRE(comp(w1, w2));
    REQUIRE(!comp(w2, w1));
  }

  LIBSEMIGROUPS_TEST_CASE("WtLenLexCmp",
                          "010",
                          "complex example from documentation",
                          "[quick][order]") {
    auto                rg      = ReportGuard(false);
    std::vector<size_t> weights = {2, 1, 6, 3, 4};
    WtLenLexCmp         comp(weights);

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

  LIBSEMIGROUPS_TEST_CASE("wt_lenlex_cmp_no_checks",
                          "011",
                          "std::string format",
                          "[quick][order]") {
    auto                rg      = ReportGuard(false);
    std::vector<size_t> weights = {2, 1, 6, 3, 4};
    std::string         w1      = {0, 1};  // weight = 2 + 1 = 3
    std::string         w2      = {2};     // weight = 6

    REQUIRE(wt_lenlex_cmp_no_checks(weights, w1, w2));
    REQUIRE(!wt_lenlex_cmp_no_checks(weights, w2, w1));
  }

  LIBSEMIGROUPS_TEST_CASE("wt_lenlex_cmp_no_checks",
                          "012",
                          "std::array format",
                          "[quick][order]") {
    auto                  rg      = ReportGuard(false);
    std::vector<size_t>   weights = {2, 1, 6, 3, 4};
    std::array<size_t, 2> w1      = {0, 1};  // weight = 2 + 1 = 3
    std::array<size_t, 1> w2      = {2};     // weight = 6

    // Use iterator version since arrays have different types
    REQUIRE(wt_lenlex_cmp_no_checks(
        weights, w1.cbegin(), w1.cend(), w2.cbegin(), w2.cend()));
    REQUIRE(!wt_lenlex_cmp_no_checks(
        weights, w2.cbegin(), w2.cend(), w1.cbegin(), w1.cend()));
  }

  LIBSEMIGROUPS_TEST_CASE("wt_lenlex_cmp_no_checks",
                          "013",
                          "std::vector with different content",
                          "[quick][order]") {
    auto                rg      = ReportGuard(false);
    std::vector<size_t> weights = {2, 1, 6, 3, 4};
    std::vector<size_t> w1      = {0, 1, 1};  // weight = 2 + 1 + 1 = 4
    std::vector<size_t> w2      = {3, 1};     // weight = 3 + 1 = 4

    // Same weight, so falls back to shortlex (w2 < w1 because w2 is shorter)
    REQUIRE(!wt_lenlex_cmp_no_checks(weights, w1, w2));
    REQUIRE(wt_lenlex_cmp_no_checks(weights, w2, w1));
  }

  // =========================================================================
  // Edge cases
  // =========================================================================

  LIBSEMIGROUPS_TEST_CASE("wt_lenlex_cmp_no_checks",
                          "014",
                          "uniform weights (shortlex)",
                          "[quick][order]") {
    auto                rg      = ReportGuard(false);
    std::vector<size_t> weights = {1, 1, 1, 1, 1};
    word_type           w1      = {0, 1};  // weight = 2
    word_type           w2      = {2, 3};  // weight = 2

    // Same weight and length, so pure shortlex: {0,1} < {2,3}
    REQUIRE(wt_lenlex_cmp_no_checks(weights, w1, w2));
    REQUIRE(!wt_lenlex_cmp_no_checks(weights, w2, w1));
  }

  LIBSEMIGROUPS_TEST_CASE("wt_lenlex_cmp_no_checks",
                          "015",
                          "single letter alphabet",
                          "[quick][order]") {
    auto                rg      = ReportGuard(false);
    std::vector<size_t> weights = {5};
    word_type           w1      = {0};        // weight = 5
    word_type           w2      = {0, 0};     // weight = 10
    word_type           w3      = {0, 0, 0};  // weight = 15

    REQUIRE(wt_lenlex_cmp_no_checks(weights, w1, w2));
    REQUIRE(wt_lenlex_cmp_no_checks(weights, w2, w3));
    REQUIRE(wt_lenlex_cmp_no_checks(weights, w1, w3));  // transitivity
  }

  LIBSEMIGROUPS_TEST_CASE("wt_lenlex_cmp_no_checks",
                          "016",
                          "words with same prefix",
                          "[quick][order]") {
    auto                rg      = ReportGuard(false);
    std::vector<size_t> weights = {2, 1, 6, 3, 4};
    word_type           w1      = {0, 1, 2};     // weight = 2+1+6 = 9
    word_type           w2      = {0, 1, 2, 1};  // weight = 2+1+6+1 = 10

    REQUIRE(wt_lenlex_cmp_no_checks(weights, w1, w2));
  }

  LIBSEMIGROUPS_TEST_CASE("wt_lenlex_cmp_no_checks",
                          "017",
                          "repeated letters",
                          "[quick][order]") {
    auto                rg      = ReportGuard(false);
    std::vector<size_t> weights = {3, 2, 1};
    word_type           w1      = {2, 2, 2, 2};  // weight = 1+1+1+1 = 4
    word_type           w2      = {1, 1};        // weight = 2+2 = 4

    // Same weight (4), w2 is shorter so w2 < w1
    REQUIRE(!wt_lenlex_cmp_no_checks(weights, w1, w2));
    REQUIRE(wt_lenlex_cmp_no_checks(weights, w2, w1));
  }

  // =========================================================================
  // Mathematical properties
  // =========================================================================

  LIBSEMIGROUPS_TEST_CASE("wt_lenlex_cmp_no_checks",
                          "018",
                          "irreflexivity: !(a < a)",
                          "[quick][order]") {
    auto                rg      = ReportGuard(false);
    std::vector<size_t> weights = {2, 1, 6, 3, 4};
    word_type           w       = {0, 1, 2, 3};

    REQUIRE(!wt_lenlex_cmp_no_checks(weights, w, w));
  }

  LIBSEMIGROUPS_TEST_CASE("wt_lenlex_cmp_no_checks",
                          "019",
                          "transitivity: a<b && b<c => a<c",
                          "[quick][order]") {
    auto                rg      = ReportGuard(false);
    std::vector<size_t> weights = {2, 1, 6, 3, 4};
    word_type           w1      = {1};     // weight = 1
    word_type           w2      = {0, 1};  // weight = 3
    word_type           w3      = {2};     // weight = 6

    REQUIRE(wt_lenlex_cmp_no_checks(weights, w1, w2));
    REQUIRE(wt_lenlex_cmp_no_checks(weights, w2, w3));
    REQUIRE(wt_lenlex_cmp_no_checks(weights, w1, w3));  // transitivity
  }

  LIBSEMIGROUPS_TEST_CASE("wt_lenlex_cmp_no_checks",
                          "020",
                          "antisymmetry: a<b => !(b<a)",
                          "[quick][order]") {
    auto                rg      = ReportGuard(false);
    std::vector<size_t> weights = {2, 1, 6, 3, 4};
    word_type           w1      = {1};  // weight = 1
    word_type           w2      = {2};  // weight = 6

    REQUIRE(wt_lenlex_cmp_no_checks(weights, w1, w2));
    REQUIRE(!wt_lenlex_cmp_no_checks(weights, w2, w1));
  }

  // =========================================================================
  // Validation tests - wt_lenlex_cmp
  // =========================================================================

  LIBSEMIGROUPS_TEST_CASE("wt_lenlex_cmp",
                          "021",
                          "valid letters with word_type",
                          "[quick][order]") {
    auto                rg      = ReportGuard(false);
    std::vector<size_t> weights = {2, 1, 6, 3, 4};
    word_type           w1      = {0, 1};  // weight = 3
    word_type           w2      = {2};     // weight = 6

    REQUIRE(wt_lenlex_cmp(weights, w1, w2));
    REQUIRE(!wt_lenlex_cmp(weights, w2, w1));
  }

  LIBSEMIGROUPS_TEST_CASE("wt_lenlex_cmp",
                          "022",
                          "invalid letter throws exception",
                          "[quick][order]") {
    auto                rg      = ReportGuard(false);
    std::vector<size_t> weights = {2, 1, 6, 3, 4};  // alphabet size = 5
    word_type           w1      = {0, 1};
    word_type           w2      = {5};  // invalid: 5 >= weights.size()

    REQUIRE_EXCEPTION_MSG(std::ignore = wt_lenlex_cmp(weights, w1, w2),
                          "letter value not compatible with weights, expected "
                          "value in [0, 5), found 5 in position 0");
    REQUIRE_NOTHROW(wt_lenlex_cmp(weights, w1, w1));
  }

  LIBSEMIGROUPS_TEST_CASE("wt_lenlex_cmp",
                          "023",
                          "std::string format with validation",
                          "[quick][order]") {
    auto                rg      = ReportGuard(false);
    std::vector<size_t> weights = {2, 1, 6, 3, 4};
    std::string         w1      = {0, 1};  // weight = 3
    std::string         w2      = {2};     // weight = 6

    REQUIRE(wt_lenlex_cmp(weights, w1, w2));
    REQUIRE(!wt_lenlex_cmp(weights, w2, w1));
  }

  LIBSEMIGROUPS_TEST_CASE("wt_lenlex_cmp",
                          "024",
                          "iterator version with validation",
                          "[quick][order]") {
    auto                rg      = ReportGuard(false);
    std::vector<size_t> weights = {2, 1, 6, 3, 4};
    word_type           w1      = {0, 1};  // weight = 3
    word_type           w2      = {2};     // weight = 6

    REQUIRE(
        wt_lenlex_cmp(weights, w1.cbegin(), w1.cend(), w2.cbegin(), w2.cend()));
    REQUIRE(!wt_lenlex_cmp(
        weights, w2.cbegin(), w2.cend(), w1.cbegin(), w1.cend()));
  }

  LIBSEMIGROUPS_TEST_CASE("wt_lenlex_cmp",
                          "025",
                          "with alphabet",
                          "[quick][order]") {
    using std::string_literals::operator""s;

    auto                rg = ReportGuard(false);
    Alphabet            alphabet("ba"s);
    std::vector<size_t> weights       = {10, 1};
    std::vector<size_t> equal_weights = {1, 1};
    auto                a             = "a"s;
    auto                b             = "b"s;

    REQUIRE(wt_lenlex_cmp(alphabet, weights, a, b));
    REQUIRE(!wt_lenlex_cmp(alphabet, weights, b, a));

    REQUIRE(wt_lenlex_cmp(alphabet, equal_weights, b, a));
    REQUIRE(!wt_lenlex_cmp(alphabet, equal_weights, a, b));
    REQUIRE(wt_lenlex_cmp(
        alphabet, equal_weights, b.cbegin(), b.cend(), a.cbegin(), a.cend()));

    std::vector<size_t> short_weights = {1};
    REQUIRE_EXCEPTION_MSG(std::ignore
                          = wt_lenlex_cmp(alphabet, short_weights, a, b),
                          "letter value not compatible with weights, expected "
                          "value in [0, 1), found 1 in position 0");
  }

  LIBSEMIGROUPS_TEST_CASE("wt_lenlex_cmp",
                          "026",
                          "both words invalid",
                          "[quick][order]") {
    auto                rg      = ReportGuard(false);
    std::vector<size_t> weights = {2, 1, 6};  // alphabet size = 3
    word_type           w1      = {5};        // invalid
    word_type           w2      = {10};       // invalid

    REQUIRE_EXCEPTION_MSG(std::ignore = wt_lenlex_cmp(weights, w1, w2),
                          "letter value not compatible with weights, expected "
                          "value in [0, 3), found 5 in position 0");
  }

  LIBSEMIGROUPS_TEST_CASE("wt_lenlex_cmp",
                          "027",
                          "invalid letter in middle of word",
                          "[quick][order]") {
    auto                rg      = ReportGuard(false);
    std::vector<size_t> weights = {2, 1, 6};  // alphabet size = 3
    word_type           w1      = {0, 1, 2};  // valid
    word_type           w2      = {0, 5, 2};  // invalid in middle

    REQUIRE_EXCEPTION_MSG(std::ignore = wt_lenlex_cmp(weights, w1, w2),
                          "letter value not compatible with weights, expected "
                          "value in [0, 3), found 5 in position 1");
    REQUIRE_EXCEPTION_MSG(std::ignore = wt_lenlex_cmp(weights, w2, w1),
                          "letter value not compatible with weights, expected "
                          "value in [0, 3), found 5 in position 1");
  }

  LIBSEMIGROUPS_TEST_CASE("wt_lenlex_cmp",
                          "028",
                          "empty weights vector",
                          "[quick][order]") {
    auto                rg      = ReportGuard(false);
    std::vector<size_t> weights = {};   // empty alphabet
    word_type           w1      = {};   // empty word is valid
    word_type           w2      = {0};  // invalid: no letters in alphabet

    REQUIRE_NOTHROW(wt_lenlex_cmp(weights, w1, w1));
    REQUIRE_EXCEPTION_MSG(std::ignore = wt_lenlex_cmp(weights, w2, w1),
                          "letter value not compatible with weights, expected "
                          "value in [0, 0), found 0 in position 0");
  }

  LIBSEMIGROUPS_TEST_CASE("WtLenLexCmp",
                          "029",
                          "struct with validation enabled",
                          "[quick][order]") {
    auto                rg      = ReportGuard(false);
    std::vector<size_t> weights = {2, 1, 6, 3, 4};
    WtLenLexCmp         comp(weights);

    word_type w1 = {0, 1};  // weight = 3
    word_type w2 = {2};     // weight = 6

    REQUIRE(comp(w1, w2));
    REQUIRE(!comp(w2, w1));
  }

  LIBSEMIGROUPS_TEST_CASE("WtLenLexCmp",
                          "030",
                          "struct with alphabet and validation enabled",
                          "[quick][order]") {
    using std::string_literals::operator""s;

    auto                rg = ReportGuard(false);
    Alphabet            alphabet("ba"s);
    std::vector<size_t> weights = {10, 1};
    auto                a       = "a"s;
    auto                b       = "b"s;

    WtLenLexCmp comp(alphabet, weights);
    REQUIRE(comp(a, b));
    REQUIRE(!comp(b, a));

    REQUIRE_EXCEPTION_MSG(std::ignore = comp("c"s, "d"s),
                          "invalid letter 'c', valid letters are \"ba\"");
    REQUIRE(!comp(b, b));
  }

  LIBSEMIGROUPS_TEST_CASE("WtLenLexCmp",
                          "031",
                          "exceptions",
                          "[quick][order]") {
    auto                rg      = ReportGuard(false);
    std::vector<size_t> weights = {2, 1, 6, 3, 4};  // alphabet size = 5
    WtLenLexCmp         comp(weights);

    word_type w1 = {0, 1};
    word_type w2 = {10};  // invalid: 10 >= weights.size()

    REQUIRE_EXCEPTION_MSG(std::ignore = comp(w1, w2),
                          "letter value not compatible with weights, expected "
                          "value in [0, 5), found 10 in position 0");
  }

  // =========================================================================
  // STL integration
  // =========================================================================

  LIBSEMIGROUPS_TEST_CASE("WtLenLexCmp",
                          "034",
                          "use in std::set",
                          "[quick][order]") {
    auto                rg      = ReportGuard(false);
    std::vector<size_t> weights = {3, 2, 1};
    WtLenLexCmp         comp(weights);

    std::set<word_type, WtLenLexCmp<>> ordered_words(comp);
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

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("wt_lenlex_cmp_no_checks",
                                   "035",
                                   "multiple word types",
                                   "[quick][order]",
                                   word_type,
                                   std::string) {
    auto rg                     = ReportGuard(false);
    using W                     = TestType;
    std::vector<size_t> weights = {2, 1, 6, 3, 4};
    W                   w1      = {0, 1};  // weight = 3
    W                   w2      = {2};     // weight = 6

    REQUIRE(wt_lenlex_cmp_no_checks(weights, w1, w2));
    REQUIRE(!wt_lenlex_cmp_no_checks(weights, w2, w1));
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("wt_lenlex_cmp",
                                   "036",
                                   "validation with multiple word types",
                                   "[quick][order]",
                                   word_type,
                                   std::string) {
    auto rg                     = ReportGuard(false);
    using W                     = TestType;
    std::vector<size_t> weights = {2, 1, 6};
    W                   w1      = {0, 1};  // valid
    W                   w2      = {5};     // invalid

    REQUIRE_NOTHROW(wt_lenlex_cmp(weights, w1, w1));
    REQUIRE_EXCEPTION_MSG(std::ignore = wt_lenlex_cmp(weights, w1, w2),
                          "letter value not compatible with weights, expected "
                          "value in [0, 3), found 5 in position 0");
  }

  // =========================================================================
  // Recursive Path Compare
  // =========================================================================

  LIBSEMIGROUPS_TEST_CASE("rev_rpo_cmp",
                          "037",
                          "empty word",
                          "[quick][order]") {
    auto      rg = ReportGuard(false);
    word_type w1(12_w);
    word_type w2;

    REQUIRE(!rev_rpo_cmp(w1, w1));
    REQUIRE(rev_rpo_cmp(w2, w1));
    REQUIRE(!rev_rpo_cmp(w1, w2));
  }

  LIBSEMIGROUPS_TEST_CASE("RevRPOCmp", "038", "empty word", "[quick][order]") {
    auto      rg = ReportGuard(false);
    word_type w1(12_w);
    word_type w2;

    REQUIRE(!RevRPOCmp()(w1, w1));
    REQUIRE(RevRPOCmp()(w2, w1));
    REQUIRE(!RevRPOCmp()(w1, w2));
  }

  LIBSEMIGROUPS_TEST_CASE("rev_rpo_cmp",
                          "039",
                          "with alphabet",
                          "[quick][order]") {
    using std::string_literals::operator""s;

    Alphabet alphabet("ba"s);

    REQUIRE(rev_rpo_cmp("a"s, "b"s));
    REQUIRE(!rev_rpo_cmp(alphabet, "a"s, "b"s));
    REQUIRE(rev_rpo_cmp(alphabet, "b"s, "a"s));
    REQUIRE(!rev_rpo_cmp_no_checks(alphabet, "a"s, "b"s));
    REQUIRE(rev_rpo_cmp_no_checks(alphabet, "b"s, "a"s));

    auto u = "aa"s;
    auto v = "ab"s;
    REQUIRE(rev_rpo_cmp(u, v));
    REQUIRE(!rev_rpo_cmp(alphabet, u, v));
    REQUIRE(rev_rpo_cmp(alphabet, u.cbegin(), u.cend(), v.cbegin(), v.cend())
            == rev_rpo_cmp(alphabet, u, v));
    REQUIRE(rev_rpo_cmp_no_checks(
                alphabet, v.cbegin(), v.cend(), u.cbegin(), u.cend())
            == rev_rpo_cmp_no_checks(alphabet, v, u));

    REQUIRE(!RevRPOCmp(alphabet)(u, v));
    REQUIRE(RevRPOCmp(alphabet)(v, u));
    REQUIRE(!RevRPOCmpNoChecks(alphabet)(u, v));
    REQUIRE(RevRPOCmpNoChecks(alphabet)(v, u));

    alphabet.init("cd"s);

    REQUIRE_EXCEPTION_MSG(std::ignore = rev_rpo_cmp(alphabet, "b"s, "aa"s),
                          "invalid letter 'b', valid letters are \"cd\"");
    REQUIRE_EXCEPTION_MSG(std::ignore = RevRPOCmp(alphabet)("b"s, "aa"s),
                          "invalid letter 'b', valid letters are \"cd\"");
  }

  LIBSEMIGROUPS_TEST_CASE("rpo_cmp",
                          "040",
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

  LIBSEMIGROUPS_TEST_CASE("rpo_cmp", "041", "with alphabet", "[quick][order]") {
    using std::string_literals::operator""s;

    Alphabet alphabet("ba"s);

    REQUIRE(rpo_cmp("a"s, "b"s));
    REQUIRE(!rpo_cmp(alphabet, "a"s, "b"s));
    REQUIRE(rpo_cmp(alphabet, "b"s, "a"s));
    REQUIRE(!rpo_cmp_no_checks(alphabet, "a"s, "b"s));
    REQUIRE(rpo_cmp_no_checks(alphabet, "b"s, "a"s));

    auto u = "aa"s;
    auto v = "ba"s;
    REQUIRE(rpo_cmp(u, v));
    REQUIRE(!rpo_cmp(alphabet, u, v));
    REQUIRE(rpo_cmp(alphabet, u.cbegin(), u.cend(), v.cbegin(), v.cend())
            == rpo_cmp(alphabet, u, v));
    REQUIRE(
        rpo_cmp_no_checks(alphabet, v.cbegin(), v.cend(), u.cbegin(), u.cend())
        == rpo_cmp_no_checks(alphabet, v, u));

    REQUIRE(!RPOCmpNoChecks(alphabet)(u, v));
    REQUIRE(RPOCmpNoChecks(alphabet)(v, u));

    alphabet.init("cd"s);

    REQUIRE_EXCEPTION_MSG(static_cast<void>(rpo_cmp(alphabet, "b"s, "aa"s)),
                          "invalid letter 'b', valid letters are \"cd\"");
    REQUIRE_EXCEPTION_MSG(static_cast<void>(RPOCmp(alphabet)("b"s, "aa"s)),
                          "invalid letter 'b', valid letters are \"cd\"");
  }

  // =========================================================================
  // lex_cmp with alphabet
  // =========================================================================

  LIBSEMIGROUPS_TEST_CASE("lex_cmp", "042", "with alphabet", "[quick][order]") {
    using std::string_literals::operator""s;

    StringRange sr;
    sr.alphabet("ab").min(2).max(5);

    auto strings = (sr | rx::to_vector());

    std::sort(
        strings.begin(), strings.end(), [](auto const& lhop, auto const& rhop) {
          return lex_cmp(lhop, rhop);
        });

    REQUIRE(strings
            == std::vector(
                {"aa"s, "aaa"s, "aaaa"s, "aaab"s, "aab"s, "aaba"s, "aabb"s,
                 "ab"s, "aba"s, "abaa"s, "abab"s, "abb"s, "abba"s, "abbb"s,
                 "ba"s, "baa"s, "baaa"s, "baab"s, "bab"s, "baba"s, "babb"s,
                 "bb"s, "bba"s, "bbaa"s, "bbab"s, "bbb"s, "bbba"s, "bbbb"s}));

    Alphabet alphabet("ba"s);

    std::sort(strings.begin(),
              strings.end(),
              [&alphabet](auto const& lhop, auto const& rhop) {
                return lex_cmp(alphabet, lhop, rhop);
              });

    REQUIRE(strings
            == std::vector(
                {"bb"s, "bbb"s, "bbbb"s, "bbba"s, "bba"s, "bbab"s, "bbaa"s,
                 "ba"s, "bab"s, "babb"s, "baba"s, "baa"s, "baab"s, "baaa"s,
                 "ab"s, "abb"s, "abbb"s, "abba"s, "aba"s, "abab"s, "abaa"s,
                 "aa"s, "aab"s, "aabb"s, "aaba"s, "aaa"s, "aaab"s, "aaaa"s}));

    std::sort(strings.begin(), strings.end(), LexCmp());

    // TODO rm
    // template <typename Args...>
    // KnuthBendix<std::string, LenLex> kb(congruence_kind, p, Args &&..arg)
    //     : _order(std::forward<Args>(args...)) ()

    REQUIRE(strings
            == std::vector(
                {"aa"s, "aaa"s, "aaaa"s, "aaab"s, "aab"s, "aaba"s, "aabb"s,
                 "ab"s, "aba"s, "abaa"s, "abab"s, "abb"s, "abba"s, "abbb"s,
                 "ba"s, "baa"s, "baaa"s, "baab"s, "bab"s, "baba"s, "babb"s,
                 "bb"s, "bba"s, "bbaa"s, "bbab"s, "bbb"s, "bbba"s, "bbbb"s}));

    std::sort(strings.begin(), strings.end(), LexCmp(alphabet));
    REQUIRE(strings
            == std::vector(
                {"bb"s, "bbb"s, "bbbb"s, "bbba"s, "bba"s, "bbab"s, "bbaa"s,
                 "ba"s, "bab"s, "babb"s, "baba"s, "baa"s, "baab"s, "baaa"s,
                 "ab"s, "abb"s, "abbb"s, "abba"s, "aba"s, "abab"s, "abaa"s,
                 "aa"s, "aab"s, "aabb"s, "aaba"s, "aaa"s, "aaab"s, "aaaa"s}));

    alphabet.init("cd"s);

    REQUIRE_EXCEPTION_MSG(
        std::sort(strings.begin(), strings.end(), LexCmp(alphabet)),
        "invalid letter 'b', valid letters are \"cd\"");
  }

  // =========================================================================
  // lenlex_cmp with alphabet
  // =========================================================================

  LIBSEMIGROUPS_TEST_CASE("lenlex_cmp",
                          "043",
                          "with alphabet",
                          "[quick][order]") {
    using std::string_literals::operator""s;

    StringRange sr;
    sr.alphabet("ab").min(2).max(5);

    auto strings = (sr | rx::to_vector());

    std::sort(strings.begin(), strings.end(), lenlex_cmp<std::string, void>);

    REQUIRE(strings == std::vector({"aa"s,   "ab"s,   "ba"s,   "bb"s,   "aaa"s,
                                    "aab"s,  "aba"s,  "abb"s,  "baa"s,  "bab"s,
                                    "bba"s,  "bbb"s,  "aaaa"s, "aaab"s, "aaba"s,
                                    "aabb"s, "abaa"s, "abab"s, "abba"s, "abbb"s,
                                    "baaa"s, "baab"s, "baba"s, "babb"s, "bbaa"s,
                                    "bbab"s, "bbba"s, "bbbb"s}));

    Alphabet alphabet("ba"s);
    auto     a = "a"s;
    auto     b = "b"s;

    REQUIRE(lenlex_cmp_no_checks(alphabet, b, a));
    REQUIRE(lenlex_cmp_no_checks(
        alphabet, b.cbegin(), b.cend(), a.cbegin(), a.cend()));

    std::sort(strings.begin(),
              strings.end(),
              [&alphabet](auto const& lhop, auto const& rhop) {
                return lenlex_cmp(alphabet, lhop, rhop);
              });

    REQUIRE(strings == std::vector({"bb"s,   "ba"s,   "ab"s,   "aa"s,   "bbb"s,
                                    "bba"s,  "bab"s,  "baa"s,  "abb"s,  "aba"s,
                                    "aab"s,  "aaa"s,  "bbbb"s, "bbba"s, "bbab"s,
                                    "bbaa"s, "babb"s, "baba"s, "baab"s, "baaa"s,
                                    "abbb"s, "abba"s, "abab"s, "abaa"s, "aabb"s,
                                    "aaba"s, "aaab"s, "aaaa"s}));

    std::sort(strings.begin(), strings.end(), LenLexCmp());

    REQUIRE(strings == std::vector({"aa"s,   "ab"s,   "ba"s,   "bb"s,   "aaa"s,
                                    "aab"s,  "aba"s,  "abb"s,  "baa"s,  "bab"s,
                                    "bba"s,  "bbb"s,  "aaaa"s, "aaab"s, "aaba"s,
                                    "aabb"s, "abaa"s, "abab"s, "abba"s, "abbb"s,
                                    "baaa"s, "baab"s, "baba"s, "babb"s, "bbaa"s,
                                    "bbab"s, "bbba"s, "bbbb"s}));

    std::sort(strings.begin(), strings.end(), LenLexCmp(alphabet));
    REQUIRE(strings == std::vector({"bb"s,   "ba"s,   "ab"s,   "aa"s,   "bbb"s,
                                    "bba"s,  "bab"s,  "baa"s,  "abb"s,  "aba"s,
                                    "aab"s,  "aaa"s,  "bbbb"s, "bbba"s, "bbab"s,
                                    "bbaa"s, "babb"s, "baba"s, "baab"s, "baaa"s,
                                    "abbb"s, "abba"s, "abab"s, "abaa"s, "aabb"s,
                                    "aaba"s, "aaab"s, "aaaa"s}));

    REQUIRE(LenLexCmpNoChecks(alphabet)(b, a));
    REQUIRE(LenLexCmpNoChecks(alphabet)(
        b.cbegin(), b.cend(), a.cbegin(), a.cend()));

    alphabet.init("cd"s);

    REQUIRE_EXCEPTION_MSG(std::ignore = lenlex_cmp(alphabet, "b"s, "aa"s),
                          "invalid letter 'b', valid letters are \"cd\"");
    REQUIRE_EXCEPTION_MSG(std::ignore = LenLexCmp(alphabet)("b"s, "aa"s),
                          "invalid letter 'b', valid letters are \"cd\"");
  }

  // =========================================================================
  // rpo_cmp with alphabet
  // =========================================================================

  LIBSEMIGROUPS_TEST_CASE("rpo_cmp", "044", "with alphabet", "[quick][order]") {
    using std::string_literals::operator""s;

    StringRange sr;
    sr.alphabet("ab").min(2).max(5);

    auto strings = (sr | rx::to_vector());

    std::sort(
        strings.begin(), strings.end(), [](auto const& lhop, auto const& rhop) {
          return rpo_cmp(lhop, rhop);
        });

    REQUIRE(strings == std::vector({"aa"s,   "aaa"s,  "aaaa"s, "ab"s,   "aab"s,
                                    "aaab"s, "ba"s,   "aba"s,  "aaba"s, "baa"s,
                                    "abaa"s, "baaa"s, "bb"s,   "abb"s,  "aabb"s,
                                    "bab"s,  "abab"s, "baab"s, "bba"s,  "abba"s,
                                    "baba"s, "bbaa"s, "bbb"s,  "abbb"s, "babb"s,
                                    "bbab"s, "bbba"s, "bbbb"s}));

    Alphabet alphabet("ba"s);

    std::sort(strings.begin(),
              strings.end(),
              [&alphabet](auto const& lhop, auto const& rhop) {
                return rpo_cmp(alphabet, lhop, rhop);
              });

    REQUIRE(strings == std::vector({"bb"s,   "bbb"s,  "bbbb"s, "ba"s,   "bba"s,
                                    "bbba"s, "ab"s,   "bab"s,  "bbab"s, "abb"s,
                                    "babb"s, "abbb"s, "aa"s,   "baa"s,  "bbaa"s,
                                    "aba"s,  "baba"s, "abba"s, "aab"s,  "baab"s,
                                    "abab"s, "aabb"s, "aaa"s,  "baaa"s, "abaa"s,
                                    "aaba"s, "aaab"s, "aaaa"s}));

    std::sort(strings.begin(), strings.end(), RPOCmp());

    REQUIRE(strings == std::vector({"aa"s,   "aaa"s,  "aaaa"s, "ab"s,   "aab"s,
                                    "aaab"s, "ba"s,   "aba"s,  "aaba"s, "baa"s,
                                    "abaa"s, "baaa"s, "bb"s,   "abb"s,  "aabb"s,
                                    "bab"s,  "abab"s, "baab"s, "bba"s,  "abba"s,
                                    "baba"s, "bbaa"s, "bbb"s,  "abbb"s, "babb"s,
                                    "bbab"s, "bbba"s, "bbbb"s}));

    std::sort(strings.begin(), strings.end(), RPOCmp(alphabet));

    REQUIRE(strings == std::vector({"bb"s,   "bbb"s,  "bbbb"s, "ba"s,   "bba"s,
                                    "bbba"s, "ab"s,   "bab"s,  "bbab"s, "abb"s,
                                    "babb"s, "abbb"s, "aa"s,   "baa"s,  "bbaa"s,
                                    "aba"s,  "baba"s, "abba"s, "aab"s,  "baab"s,
                                    "abab"s, "aabb"s, "aaa"s,  "baaa"s, "abaa"s,
                                    "aaba"s, "aaab"s, "aaaa"s}));
  }

  // =========================================================================
  // rev_rpo_cmp with alphabet
  // =========================================================================

  LIBSEMIGROUPS_TEST_CASE("rev_rpo_cmp",
                          "045",
                          "with alphabet",
                          "[quick][order]") {
    using std::string_literals::operator""s;

    StringRange sr;
    sr.alphabet("ab").min(2).max(5);

    auto strings = (sr | rx::to_vector());

    std::sort(
        strings.begin(), strings.end(), [](auto const& lhop, auto const& rhop) {
          return rev_rpo_cmp(lhop, rhop);
        });

    REQUIRE(strings == std::vector({"aa"s,   "aaa"s,  "aaaa"s, "ba"s,   "baa"s,
                                    "baaa"s, "ab"s,   "aba"s,  "abaa"s, "aab"s,
                                    "aaba"s, "aaab"s, "bb"s,   "bba"s,  "bbaa"s,
                                    "bab"s,  "baba"s, "baab"s, "abb"s,  "abba"s,
                                    "abab"s, "aabb"s, "bbb"s,  "bbba"s, "bbab"s,
                                    "babb"s, "abbb"s, "bbbb"s}));

    Alphabet alphabet("ba"s);

    std::sort(strings.begin(),
              strings.end(),
              [&alphabet](auto const& lhop, auto const& rhop) {
                return rev_rpo_cmp(alphabet, lhop, rhop);
              });

    REQUIRE(strings == std::vector({"bb"s,   "bbb"s,  "bbbb"s, "ab"s,   "abb"s,
                                    "abbb"s, "ba"s,   "bab"s,  "babb"s, "bba"s,
                                    "bbab"s, "bbba"s, "aa"s,   "aab"s,  "aabb"s,
                                    "aba"s,  "abab"s, "abba"s, "baa"s,  "baab"s,
                                    "baba"s, "bbaa"s, "aaa"s,  "aaab"s, "aaba"s,
                                    "abaa"s, "baaa"s, "aaaa"s}));

    std::sort(strings.begin(), strings.end(), RevRPOCmp());

    REQUIRE(strings == std::vector({"aa"s,   "aaa"s,  "aaaa"s, "ba"s,   "baa"s,
                                    "baaa"s, "ab"s,   "aba"s,  "abaa"s, "aab"s,
                                    "aaba"s, "aaab"s, "bb"s,   "bba"s,  "bbaa"s,
                                    "bab"s,  "baba"s, "baab"s, "abb"s,  "abba"s,
                                    "abab"s, "aabb"s, "bbb"s,  "bbba"s, "bbab"s,
                                    "babb"s, "abbb"s, "bbbb"s}));

    alphabet.init("cd"s);
    REQUIRE_EXCEPTION_MSG(
        std::sort(strings.begin(), strings.end(), RevRPOCmp(alphabet)),
        "invalid letter 'a', valid letters are \"cd\"");
  }

  // =========================================================================
  // wt_lex_cmp with string weights
  // =========================================================================

  LIBSEMIGROUPS_TEST_CASE("wt_lenlex_cmp",
                          "046",
                          "with string weights",
                          "[quick][order]") {
    using std::string_literals::operator""s;

    StringRange sr;
    sr.alphabet("ab").min(2).max(5);

    auto strings = (sr | rx::to_vector());

    std::vector<size_t> weights(256, 100);
    weights['a'] = 1;
    weights['b'] = 2;

    std::sort(strings.begin(),
              strings.end(),
              [&weights](auto const& lhop, auto const& rhop) {
                return wt_lenlex_cmp(weights, lhop, rhop);
              });

    REQUIRE(strings == std::vector({"aa"s,   "ab"s,   "ba"s,   "aaa"s,  "bb"s,
                                    "aab"s,  "aba"s,  "baa"s,  "aaaa"s, "abb"s,
                                    "bab"s,  "bba"s,  "aaab"s, "aaba"s, "abaa"s,
                                    "baaa"s, "bbb"s,  "aabb"s, "abab"s, "abba"s,
                                    "baab"s, "baba"s, "bbaa"s, "abbb"s, "babb"s,
                                    "bbab"s, "bbba"s, "bbbb"s}));

    Alphabet alphabet("ba"s);

    REQUIRE_EXCEPTION_MSG(WtLenLexCmp(alphabet, weights),
                          "the alphabet and weights must have the same "
                          "size, but found 2 and 256");
    weights = {2, 1};
    std::sort(strings.begin(), strings.end(), WtLenLexCmp(alphabet, weights));

    REQUIRE(strings == std::vector({"aa"s,   "ba"s,   "ab"s,   "aaa"s,  "bb"s,
                                    "baa"s,  "aba"s,  "aab"s,  "aaaa"s, "bba"s,
                                    "bab"s,  "abb"s,  "baaa"s, "abaa"s, "aaba"s,
                                    "aaab"s, "bbb"s,  "bbaa"s, "baba"s, "baab"s,
                                    "abba"s, "abab"s, "aabb"s, "bbba"s, "bbab"s,
                                    "babb"s, "abbb"s, "bbbb"s}));
  }

  LIBSEMIGROUPS_TEST_CASE("wt_lex_cmp",
                          "047",
                          "with alphabet",
                          "[quick][order]") {
    using std::string_literals::operator""s;

    StringRange sr;
    sr.alphabet("ab").min(2).max(5);

    auto strings = (sr | rx::to_vector());

    std::vector<size_t> weights(256, 100);
    weights['a'] = 1;
    weights['b'] = 2;

    std::sort(strings.begin(), strings.end(), WtLexCmp(weights));

    REQUIRE(strings == std::vector({"aa"s,   "aaa"s,  "ab"s,   "ba"s,   "aaaa"s,
                                    "aab"s,  "aba"s,  "baa"s,  "bb"s,   "aaab"s,
                                    "aaba"s, "abaa"s, "abb"s,  "baaa"s, "bab"s,
                                    "bba"s,  "aabb"s, "abab"s, "abba"s, "baab"s,
                                    "baba"s, "bbaa"s, "bbb"s,  "abbb"s, "babb"s,
                                    "bbab"s, "bbba"s, "bbbb"s}));

    Alphabet            alphabet("ba"s);
    std::vector<size_t> alphabet_weights = {1, 2};
    auto                a                = "a"s;
    auto                b                = "b"s;

    REQUIRE(wt_lex_cmp(alphabet, alphabet_weights, b, a));
    REQUIRE(wt_lex_cmp_no_checks(alphabet, alphabet_weights, b, a));
    REQUIRE(wt_lex_cmp_no_checks(alphabet,
                                 alphabet_weights,
                                 b.cbegin(),
                                 b.cend(),
                                 a.cbegin(),
                                 a.cend()));

    std::sort(
        strings.begin(), strings.end(), WtLexCmp(alphabet, alphabet_weights));

    REQUIRE(strings == std::vector({"bb"s,   "bbb"s,  "ba"s,   "ab"s,   "bbbb"s,
                                    "bba"s,  "bab"s,  "abb"s,  "aa"s,   "bbba"s,
                                    "bbab"s, "babb"s, "baa"s,  "abbb"s, "aba"s,
                                    "aab"s,  "bbaa"s, "baba"s, "baab"s, "abba"s,
                                    "abab"s, "aabb"s, "aaa"s,  "baaa"s, "abaa"s,
                                    "aaba"s, "aaab"s, "aaaa"s}));

    REQUIRE(WtLexCmpNoChecks(alphabet, alphabet_weights)(b, a));

    std::vector<size_t> short_weights = {1};
    REQUIRE_EXCEPTION_MSG(
        static_cast<void>(wt_lex_cmp(alphabet, short_weights, a, b)),
        "letter value not compatible with weights, expected value in [0, 1), "
        "found 1 in position 0");

    alphabet.init("cd"s);
    REQUIRE_EXCEPTION_MSG(
        static_cast<void>(WtLexCmp(alphabet, alphabet_weights)("b"s, "aa"s)),
        "invalid letter 'b', valid letters are \"cd\"");
  }

  LIBSEMIGROUPS_TEST_CASE("order.hpp",
                          "048",
                          "coverage function instantiations",
                          "[quick][order]") {
    using std::string_literals::operator""s;

    auto rg = ReportGuard(false);

    // Function instantiations that are easy to miss in the coverage report.
    std::vector<size_t> v1 = {0, 1};
    std::vector<size_t> v2 = {0, 2};
    REQUIRE(lex_cmp(v1, v2));

    size_t const a1[] = {0, 1};
    size_t const a2[] = {0, 1, 0};
    REQUIRE(lenlex_cmp(a1, a1 + 2, a2, a2 + 3));

    auto c1 = chain(v1, v1);
    auto c2 = chain(v1, v2);
    REQUIRE(LenLexCmp()(c1, c2));

    Alphabet ba("ba"s);
    auto     a = "a"s;
    auto     b = "b"s;

    REQUIRE(lex_cmp_no_checks(ba, b, a));
    REQUIRE(lex_cmp_no_checks(ba, b.cbegin(), b.cend(), a.cbegin(), a.cend()));
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("order.hpp",
                                   "049",
                                   "default comparator API",
                                   "[quick][order]",
                                   LexCmp<>,
                                   LenLexCmp<>,
                                   RPOCmp<>,
                                   RevRPOCmp<>) {
    using std::string_literals::operator""s;

    auto rg = ReportGuard(false);
    auto a  = "a"s;
    auto b  = "b"s;

    TestType cmp;
    REQUIRE(cmp(a, b));
    REQUIRE(cmp(a.cbegin(), a.cend(), b.cbegin(), b.cend()));
    REQUIRE(TestType()(a.cbegin(), a.cend(), b.cbegin(), b.cend()));

    TestType cmp_copy(cmp);
    REQUIRE(cmp_copy(a, b));
    TestType cmp_move(std::move(cmp_copy));
    REQUIRE(cmp_move(a, b));

    TestType cmp_copy_assigned;
    cmp_copy_assigned = cmp;
    REQUIRE(cmp_copy_assigned(a, b));
    TestType cmp_move_assigned;
    cmp_move_assigned = std::move(cmp_move);
    REQUIRE(cmp_move_assigned(a, b));
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("order.hpp",
                                   "050",
                                   "alphabet comparator API",
                                   "[quick][order]",
                                   LexCmp<std::string>,
                                   LenLexCmp<std::string>,
                                   RPOCmp<std::string>,
                                   RevRPOCmp<std::string>) {
    using std::string_literals::operator""s;

    auto                  rg = ReportGuard(false);
    Alphabet<std::string> ba("ba"s);
    Alphabet<std::string> ab("ab"s);
    auto                  a = "a"s;
    auto                  b = "b"s;

    TestType cmp(ba);
    REQUIRE(cmp(b, a));
    REQUIRE(cmp(b.cbegin(), b.cend(), a.cbegin(), a.cend()));
    REQUIRE(cmp.alphabet().size() == 2);

    TestType from_rvalue(Alphabet<std::string>("ba"s));
    REQUIRE(from_rvalue(b, a));
    REQUIRE(from_rvalue(b.cbegin(), b.cend(), a.cbegin(), a.cend()));
    REQUIRE(from_rvalue.alphabet().size() == 2);

    TestType cmp_copy(cmp);
    REQUIRE(cmp_copy(b, a));
    TestType cmp_move(std::move(cmp_copy));
    REQUIRE(cmp_move(b, a));

    TestType cmp_copy_assigned(ab);
    cmp_copy_assigned = cmp;
    REQUIRE(cmp_copy_assigned(b, a));
    REQUIRE(cmp_copy_assigned.alphabet().size() == 2);
    TestType cmp_move_assigned(ab);
    cmp_move_assigned = std::move(cmp_move);
    REQUIRE(cmp_move_assigned(b, a));
    REQUIRE(cmp_move_assigned.alphabet().size() == 2);
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("order",
                                   "051",
                                   "alpha no-check API",
                                   "[quick][order]",
                                   LexCmpNoChecks<std::string>,
                                   LenLexCmpNoChecks<std::string>,
                                   RPOCmpNoChecks<std::string>,
                                   RevRPOCmpNoChecks<std::string>) {
    using std::string_literals::operator""s;

    auto                  rg = ReportGuard(false);
    Alphabet<std::string> ba("ba"s);
    Alphabet<std::string> ab("ab"s);
    auto                  a = "a"s;
    auto                  b = "b"s;

    TestType cmp(ba);
    REQUIRE(cmp(b, a));
    REQUIRE(cmp(b.cbegin(), b.cend(), a.cbegin(), a.cend()));
    REQUIRE(cmp.alphabet().size() == 2);

    TestType from_rvalue(Alphabet<std::string>("ba"s));
    REQUIRE(from_rvalue(b, a));
    REQUIRE(from_rvalue(b.cbegin(), b.cend(), a.cbegin(), a.cend()));
    REQUIRE(from_rvalue.alphabet().size() == 2);

    TestType cmp_copy(cmp);
    REQUIRE(cmp_copy(b, a));
    TestType cmp_move(std::move(cmp_copy));
    REQUIRE(cmp_move(b, a));

    TestType cmp_copy_assigned(ab);
    cmp_copy_assigned = cmp;
    REQUIRE(cmp_copy_assigned(b, a));
    REQUIRE(cmp_copy_assigned.alphabet().size() == 2);
    TestType cmp_move_assigned(ab);
    cmp_move_assigned = std::move(cmp_move);
    REQUIRE(cmp_move_assigned(b, a));
    REQUIRE(cmp_move_assigned.alphabet().size() == 2);
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("order",
                                   "052",
                                   "weighted alpha API",
                                   "[quick][order]",
                                   WtLenLexCmp<std::string>,
                                   WtLenLexCmpNoChecks<std::string>,
                                   WtLexCmp<std::string>,
                                   WtLexCmpNoChecks<std::string>) {
    using std::string_literals::operator""s;

    auto                  rg = ReportGuard(false);
    Alphabet<std::string> ba("ba"s);
    Alphabet<std::string> ab("ab"s);
    std::vector<size_t>   ba_weights = {10, 1};
    std::vector<size_t>   ab_weights = {1, 10};
    std::vector<size_t>   equal      = {1, 1};
    auto                  a          = "a"s;
    auto                  b          = "b"s;

    TestType cmp(ba, ba_weights);
    REQUIRE(cmp(a, b));
    REQUIRE(cmp(a.cbegin(), a.cend(), b.cbegin(), b.cend()));
    REQUIRE(cmp.alphabet().size() == 2);
    REQUIRE(cmp.weights() == ba_weights);

    cmp.init(ab, ab_weights);
    REQUIRE(cmp(a, b));
    cmp.init(Alphabet<std::string>("ba"s), std::vector<size_t>{10, 1});
    REQUIRE(cmp(a, b));

    TestType moved(Alphabet<std::string>("ab"s), std::vector<size_t>{1, 10});
    REQUIRE(moved(a, b));
    moved.init(ba, equal);
    REQUIRE(moved(b, a));
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("order.hpp",
                                   "053",
                                   "weights-only comparator API",
                                   "[quick][order]",
                                   WtLenLexCmp<>,
                                   WtLenLexCmpNoChecks<>,
                                   WtLexCmp<>,
                                   WtLexCmpNoChecks<>) {
    auto rg = ReportGuard(false);

    std::vector<size_t> ba_weights = {10, 1};
    std::vector<size_t> equal      = {1, 1};
    word_type           zero       = {0};
    word_type           one        = {1};

    TestType cmp(ba_weights);
    REQUIRE(cmp(one, zero));
    REQUIRE(cmp(one.cbegin(), one.cend(), zero.cbegin(), zero.cend()));
    REQUIRE(cmp.weights() == ba_weights);

    cmp.init(equal);
    REQUIRE(cmp(zero, one));
    cmp.init(std::vector<size_t>{10, 1});
    REQUIRE(cmp(one, zero));

    TestType moved(std::vector<size_t>{1, 1});
    REQUIRE(moved(zero, one));
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("order",
                                   "054",
                                   "wll alpha ctors",  // codespell:ignore
                                   "[quick][order]",
                                   WtLenLexCmp<std::string>,
                                   WtLenLexCmpNoChecks<std::string>) {
    using std::string_literals::operator""s;

    auto                  rg = ReportGuard(false);
    Alphabet<std::string> ba("ba"s);
    Alphabet<std::string> ab("ab"s);
    std::vector<size_t>   ba_weights = {10, 1};
    std::vector<size_t>   equal      = {1, 1};
    auto                  a          = "a"s;
    auto                  b          = "b"s;

    TestType from_lvalue(ba, ba_weights);
    REQUIRE(from_lvalue(a, b));
    REQUIRE(from_lvalue.alphabet().size() == 2);
    REQUIRE(from_lvalue.weights() == ba_weights);

    TestType from_rvalue(Alphabet<std::string>("ba"s),
                         std::vector<size_t>{10, 1});
    REQUIRE(from_rvalue(a, b));
    REQUIRE(from_rvalue.alphabet().size() == 2);
    REQUIRE(from_rvalue.weights() == ba_weights);

    TestType copied(from_lvalue);
    REQUIRE(copied(a, b));
    REQUIRE(copied.weights() == ba_weights);

    TestType moved(std::move(copied));
    REQUIRE(moved(a, b));
    REQUIRE(moved.weights() == ba_weights);

    TestType copy_assigned(ab, equal);
    copy_assigned = from_lvalue;
    REQUIRE(copy_assigned(a, b));
    REQUIRE(copy_assigned.weights() == ba_weights);

    TestType move_assigned(ab, equal);
    move_assigned = std::move(moved);
    REQUIRE(move_assigned(a, b));
    REQUIRE(move_assigned.weights() == ba_weights);

    std::vector<size_t> short_weights = {1};
    REQUIRE_EXCEPTION_MSG(
        static_cast<void>(TestType(ba, short_weights)),
        "the alphabet and weights must have the same size, but found 2 and 1");
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("order",
                                   "055",
                                   "wll ctors",  // codespell:ignore
                                   "[quick][order]",
                                   WtLenLexCmp<>,
                                   WtLenLexCmpNoChecks<>) {
    auto rg = ReportGuard(false);

    std::vector<size_t> ba_weights = {10, 1};
    std::vector<size_t> equal      = {1, 1};
    word_type           zero       = {0};
    word_type           one        = {1};

    TestType from_lvalue(ba_weights);
    REQUIRE(from_lvalue(one, zero));
    REQUIRE(from_lvalue.weights() == ba_weights);

    TestType from_rvalue(std::vector<size_t>{10, 1});
    REQUIRE(from_rvalue(one, zero));
    REQUIRE(from_rvalue.weights() == ba_weights);

    TestType copied(from_lvalue);
    REQUIRE(copied(one, zero));
    REQUIRE(copied.weights() == ba_weights);

    TestType moved(std::move(copied));
    REQUIRE(moved(one, zero));
    REQUIRE(moved.weights() == ba_weights);

    TestType copy_assigned(equal);
    copy_assigned = from_lvalue;
    REQUIRE(copy_assigned(one, zero));
    REQUIRE(copy_assigned.weights() == ba_weights);

    TestType move_assigned(equal);
    move_assigned = std::move(moved);
    REQUIRE(move_assigned(one, zero));
    REQUIRE(move_assigned.weights() == ba_weights);
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("order",
                                   "056",
                                   "wl alpha ctors",
                                   "[quick][order]",
                                   WtLexCmp<std::string>,
                                   WtLexCmpNoChecks<std::string>) {
    using std::string_literals::operator""s;

    auto                  rg = ReportGuard(false);
    Alphabet<std::string> ba("ba"s);
    Alphabet<std::string> ab("ab"s);
    std::vector<size_t>   ba_weights = {10, 1};
    std::vector<size_t>   equal      = {1, 1};
    auto                  a          = "a"s;
    auto                  b          = "b"s;

    TestType from_lvalue(ba, ba_weights);
    REQUIRE(from_lvalue(a, b));
    REQUIRE(from_lvalue.alphabet().size() == 2);
    REQUIRE(from_lvalue.weights() == ba_weights);

    TestType from_rvalue(Alphabet<std::string>("ba"s),
                         std::vector<size_t>{10, 1});
    REQUIRE(from_rvalue(a, b));
    REQUIRE(from_rvalue.alphabet().size() == 2);
    REQUIRE(from_rvalue.weights() == ba_weights);

    TestType copied(from_lvalue);
    REQUIRE(copied(a, b));
    REQUIRE(copied.weights() == ba_weights);

    TestType moved(std::move(copied));
    REQUIRE(moved(a, b));
    REQUIRE(moved.weights() == ba_weights);

    TestType copy_assigned(ab, equal);
    copy_assigned = from_lvalue;
    REQUIRE(copy_assigned(a, b));
    REQUIRE(copy_assigned.weights() == ba_weights);

    TestType move_assigned(ab, equal);
    move_assigned = std::move(moved);
    REQUIRE(move_assigned(a, b));
    REQUIRE(move_assigned.weights() == ba_weights);

    std::vector<size_t> short_weights = {1};
    REQUIRE_EXCEPTION_MSG(
        static_cast<void>(TestType(ba, short_weights)),
        "the alphabet and weights must have the same size, but found 2 and 1");
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("order",
                                   "057",
                                   "wl ctors",
                                   "[quick][order]",
                                   WtLexCmp<>,
                                   WtLexCmpNoChecks<>) {
    auto rg = ReportGuard(false);

    std::vector<size_t> ba_weights = {10, 1};
    std::vector<size_t> equal      = {1, 1};
    word_type           zero       = {0};
    word_type           one        = {1};

    TestType from_lvalue(ba_weights);
    REQUIRE(from_lvalue(one, zero));
    REQUIRE(from_lvalue.weights() == ba_weights);

    TestType from_rvalue(std::vector<size_t>{10, 1});
    REQUIRE(from_rvalue(one, zero));
    REQUIRE(from_rvalue.weights() == ba_weights);

    TestType copied(from_lvalue);
    REQUIRE(copied(one, zero));
    REQUIRE(copied.weights() == ba_weights);

    TestType moved(std::move(copied));
    REQUIRE(moved(one, zero));
    REQUIRE(moved.weights() == ba_weights);

    TestType copy_assigned(equal);
    copy_assigned = from_lvalue;
    REQUIRE(copy_assigned(one, zero));
    REQUIRE(copy_assigned.weights() == ba_weights);

    TestType move_assigned(equal);
    move_assigned = std::move(moved);
    REQUIRE(move_assigned(one, zero));
    REQUIRE(move_assigned.weights() == ba_weights);
  }

  LIBSEMIGROUPS_TEST_CASE("Order", "035", "is_stateful", "[quick][order]") {
    auto rg = ReportGuard(false);
    REQUIRE(!order::is_stateful_v<LenLexCmp<>>);
    REQUIRE(!order::is_stateful_v<LexCmp<>>);
    REQUIRE(!order::is_stateful_v<RevRPOCmp<>>);
    REQUIRE(!order::is_stateful_v<RPOCmp<>>);
    REQUIRE(order::is_stateful_v<WtLenLexCmp<>>);
    REQUIRE(order::is_stateful_v<WtLenLexCmp<>>);
  }

}  // namespace libsemigroups
