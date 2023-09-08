//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2022 James D. Mitchell
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

#include <cstddef>   // for size_t
#include <iostream>  // for cout

#include <algorithm>    // for for_each
#include <chrono>       // for duration, microsec...
#include <cstddef>      // for size_t
#include <cstdint>      // for size_t
#include <iostream>     // for string, char_traits
#include <iterator>     // for advance
#include <string>       // for swap, basic_string
#include <type_traits>  // for remove_reference<>...
#include <utility>      // for move
#include <vector>       // for vector, operator==

#include "catch.hpp"      // for REQUIRE, REQUIRE_THROWS_AS, REQUI...
#include "test-main.hpp"  // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/constants.hpp"        // for UNDEFINED
#include "libsemigroups/exception.hpp"        // for LibsemigroupsExcep...
#include "libsemigroups/fpsemi-examples.hpp"  // for make, fibonacci_se...
#include "libsemigroups/order.hpp"            // for LexicographicalCom...
#include "libsemigroups/presentation.hpp"     // for add_rule
#include "libsemigroups/stephen.hpp"          // for Stephen, Stephen::...
#include "libsemigroups/types.hpp"            // for word_type
#include "libsemigroups/word-graph.hpp"       // for WordGraph, ope...
#include "libsemigroups/words.hpp"            // for StringToWord, word...

#include "libsemigroups/detail/report.hpp"  // for ReportGuard

namespace libsemigroups {
  using namespace literals;
  namespace {
    void check_000(Stephen& s) {
      s.set_word({0}).run();
      REQUIRE(s.word_graph().number_of_nodes() == 2);
      REQUIRE(s.word_graph()
              == to_word_graph<uint32_t>(2, {{1, UNDEFINED}, {UNDEFINED, 1}}));
      REQUIRE(stephen::number_of_words_accepted(s) == POSITIVE_INFINITY);
      {
        auto first = stephen::cbegin_words_accepted(s);
        auto last  = stephen::cbegin_words_accepted(s);
        std::advance(last, 10);
        REQUIRE(std::vector<word_type>(first, last)
                == std::vector<word_type>({{0},
                                           {0, 1},
                                           {0, 1, 1},
                                           {0, 1, 1, 1},
                                           {0, 1, 1, 1, 1},
                                           {0, 1, 1, 1, 1, 1},
                                           {0, 1, 1, 1, 1, 1, 1},
                                           {0, 1, 1, 1, 1, 1, 1, 1},
                                           {0, 1, 1, 1, 1, 1, 1, 1, 1},
                                           {0, 1, 1, 1, 1, 1, 1, 1, 1, 1}}));
      }
      {
        auto first = stephen::cbegin_left_factors(s);
        auto last  = stephen::cbegin_left_factors(s);
        std::advance(last, 10);
        REQUIRE(std::vector<word_type>(first, last)
                == std::vector<word_type>({{},
                                           {0},
                                           {0, 1},
                                           {0, 1, 1},
                                           {0, 1, 1, 1},
                                           {0, 1, 1, 1, 1},
                                           {0, 1, 1, 1, 1, 1},
                                           {0, 1, 1, 1, 1, 1, 1},
                                           {0, 1, 1, 1, 1, 1, 1, 1},
                                           {0, 1, 1, 1, 1, 1, 1, 1, 1}}));
      }
    }

    void verify_c4_normal_form(Presentation<std::string> const& p,
                               std::string const&               word,
                               std::string const&               nf) {
      detail::StringToWord string_to_word(p.alphabet());
      Stephen              S(p);
      S.set_word(string_to_word(word)).run();

      auto words = std::vector<word_type>(stephen::cbegin_words_accepted(S),
                                          stephen::cend_words_accepted(S));
      std::vector<std::string> strings(words.size(), "");

      std::transform(
          words.cbegin(), words.cend(), strings.begin(), [&p](auto const& w) {
            return detail::word_to_string(p.alphabet(), w.cbegin(), w.cend());
          });

      std::sort(strings.begin(), strings.end(), LexicographicalCompare());
      REQUIRE(strings.front() == nf);

      REQUIRE(std::all_of(strings.cbegin(),
                          strings.cend(),
                          [&S, &string_to_word](auto const& w) {
                            return stephen::accepts(S, string_to_word(w));
                          }));
      REQUIRE(stephen::number_of_words_accepted(S) == strings.size());
    }

    void verify_c4_equal_to(Presentation<std::string> const& p,
                            std::string const&               word1,
                            std::string const&               word2) {
      detail::StringToWord string_to_word(p.alphabet());
      Stephen              S(p);
      S.set_word(string_to_word(word1)).run();
      REQUIRE(stephen::accepts(S, string_to_word(word2)));
      S.set_word(string_to_word(word2)).run();
      REQUIRE(stephen::accepts(S, string_to_word(word1)));
    }

    void verify_c4_not_equal_to(Presentation<std::string> const& p,
                                std::string const&               word1,
                                std::string const&               word2) {
      detail::StringToWord string_to_word(p.alphabet());
      Stephen              S(p);
      S.set_word(string_to_word(word1)).run();
      REQUIRE(!stephen::accepts(S, string_to_word(word2)));
      S.set_word(string_to_word(word2)).run();
      REQUIRE(!stephen::accepts(S, string_to_word(word1)));
    }

  }  // namespace

  LIBSEMIGROUPS_TEST_CASE("Stephen",
                          "000",
                          "basic test 1",
                          "[quick][stephen]") {
    auto                    rg = ReportGuard(true);
    Presentation<word_type> p;
    p.alphabet(2);
    presentation::add_rule(p, {0}, {0, 1});
    Stephen s;
    s.init(p);
    check_000(s);
    s.init(p);
    check_000(s);
  }

  LIBSEMIGROUPS_TEST_CASE("Stephen",
                          "001",
                          "basic test 2",
                          "[quick][stephen]") {
    auto                    rg = ReportGuard(true);
    Presentation<word_type> p;
    p.alphabet(2);
    presentation::add_rule(p, {0, 0, 0}, {0});
    presentation::add_rule(p, {1, 1, 1}, {1});
    presentation::add_rule(p, {0, 1, 0, 1}, {0, 0});
    Stephen s(p);
    s.set_word({1, 1, 0, 1}).run();
    REQUIRE(s.word_graph().number_of_nodes() == 7);
    REQUIRE(s.word_graph()
            == to_word_graph<size_t>(7,
                                     {{UNDEFINED, 1},
                                      {UNDEFINED, 2},
                                      {3, 1},
                                      {4, 5},
                                      {3, 6},
                                      {6, 3},
                                      {5, 4}}));
    REQUIRE(stephen::number_of_words_accepted(s) == POSITIVE_INFINITY);

    word_type w = {1, 1, 0, 1};

    REQUIRE(word_graph::last_node_on_path_no_checks(
                s.word_graph(), 0, w.begin(), w.end())
                .first
            == 5);
    w = {1, 1, 0, 0, 1, 0};
    REQUIRE(word_graph::last_node_on_path_no_checks(
                s.word_graph(), 0, w.begin(), w.end())
                .first
            == 5);

    REQUIRE(stephen::accepts(s, {1, 1, 0, 0, 1, 0}));
    REQUIRE(stephen::accepts(s, {1, 1, 0, 0, 1, 0}));
    REQUIRE(!stephen::accepts(s, {}));
    REQUIRE(!stephen::accepts(s, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
    REQUIRE(!stephen::accepts(s, {1, 1, 1}));
    {
      auto first = stephen::cbegin_words_accepted(s);
      auto last  = stephen::cbegin_words_accepted(s);
      std::advance(last, 10);
      REQUIRE(std::vector<word_type>(first, last)
              == std::vector<word_type>({{1, 1, 0, 1},
                                         {1, 1, 0, 0, 0, 1},
                                         {1, 1, 0, 0, 1, 0},
                                         {1, 1, 0, 1, 0, 0},
                                         {1, 1, 0, 1, 1, 1},
                                         {1, 1, 1, 1, 0, 1},
                                         {1, 1, 0, 0, 0, 0, 0, 1},
                                         {1, 1, 0, 0, 0, 0, 1, 0},
                                         {1, 1, 0, 0, 0, 1, 0, 0},
                                         {1, 1, 0, 0, 0, 1, 1, 1}}));
    }
    {
      auto first = stephen::cbegin_left_factors(s);
      auto last  = stephen::cbegin_left_factors(s);
      std::advance(last, 10);
      REQUIRE(std::vector<word_type>(first, last)
              == std::vector<word_type>({{},
                                         {1},
                                         {1, 1},
                                         {1, 1, 0},
                                         {1, 1, 1},
                                         {1, 1, 0, 0},
                                         {1, 1, 0, 1},
                                         {1, 1, 1, 1},
                                         {1, 1, 0, 0, 0},
                                         {1, 1, 0, 0, 1}}));
      REQUIRE(stephen::number_of_left_factors(s) == POSITIVE_INFINITY);
      std::for_each(first, last, [&s](auto const& w) {
        return stephen::is_left_factor(s, w);
      });
    }

    s.set_word({0, 0}).run();
    REQUIRE(s.word_graph().number_of_nodes() == 5);
    REQUIRE(s.word_graph()
            == to_word_graph<size_t>(
                5, {{1, UNDEFINED}, {2, 3}, {1, 4}, {4, 1}, {3, 2}}));

    p.rules.clear();
    presentation::add_rule(p, {0, 0, 0}, {0});
    presentation::add_rule(p, {1, 1, 1}, {1});
    s.init(p).set_word({0, 0}).run();
    REQUIRE(s.word_graph().number_of_nodes() == 3);
    REQUIRE(s.word_graph()
            == to_word_graph<size_t>(
                3, {{1, UNDEFINED}, {2, UNDEFINED}, {1, UNDEFINED}}));
  }

  LIBSEMIGROUPS_TEST_CASE("Stephen",
                          "002",
                          "full transf monoid",
                          "[quick][stephen]") {
    using namespace fpsemigroup;
    auto   rg = ReportGuard(true);
    size_t n  = 5;
    auto   p  = full_transformation_monoid(n, author::Iwahori);
    presentation::replace_word(p, {}, {n});
    p.alphabet(n + 1);
    presentation::add_identity_rules(p, n);
    p.validate();

    Stephen s;
    s.init(std::move(p)).set_word({0, 1, 0, 1, 1, 1, 0, 2, 0, 1, 2, 0}).run();
    REQUIRE(s.word_graph().number_of_nodes() == 121);
    REQUIRE(
        s.word_graph()
        == to_word_graph<size_t>(121, {{1, 2, 3, 4, UNDEFINED, 5},
                                       {5, 6, 7, 8, UNDEFINED, 1},
                                       {9, 5, 10, 11, UNDEFINED, 2},
                                       {12, 13, 5, 14, UNDEFINED, 3},
                                       {15, 16, 17, 5, UNDEFINED, 4},
                                       {1, 2, 3, 4, UNDEFINED, 5},
                                       {18, 1, 19, 20, UNDEFINED, 6},
                                       {21, 22, 1, 23, UNDEFINED, 7},
                                       {24, 25, 26, 1, UNDEFINED, 8},
                                       {2, 18, 27, 28, UNDEFINED, 9},
                                       {29, 30, 2, 31, UNDEFINED, 10},
                                       {32, 33, 34, 2, UNDEFINED, 11},
                                       {3, 35, 21, 36, UNDEFINED, 12},
                                       {37, 3, 30, 38, UNDEFINED, 13},
                                       {39, 40, 41, 3, UNDEFINED, 14},
                                       {4, 42, 43, 24, UNDEFINED, 15},
                                       {44, 4, 45, 33, UNDEFINED, 16},
                                       {46, 47, 4, 41, UNDEFINED, 17},
                                       {6, 9, 48, 49, UNDEFINED, 18},
                                       {50, 51, 6, 52, UNDEFINED, 19},
                                       {53, 54, 55, 6, UNDEFINED, 20},
                                       {7, 56, 12, 57, UNDEFINED, 21},
                                       {58, 7, 51, 59, UNDEFINED, 22},
                                       {60, 61, 62, 7, UNDEFINED, 23},
                                       {8, 63, 64, 15, UNDEFINED, 24},
                                       {65, 8, 66, 54, UNDEFINED, 25},
                                       {67, 68, 8, 62, UNDEFINED, 26},
                                       {56, 58, 9, 69, UNDEFINED, 27},
                                       {63, 65, 70, 9, UNDEFINED, 28},
                                       {10, 50, 56, 71, UNDEFINED, 29},
                                       {51, 10, 13, 72, UNDEFINED, 30},
                                       {73, 74, 75, 10, UNDEFINED, 31},
                                       {11, 53, 76, 63, UNDEFINED, 32},
                                       {54, 11, 77, 16, UNDEFINED, 33},
                                       {78, 79, 11, 75, UNDEFINED, 34},
                                       {48, 12, 50, 80, UNDEFINED, 35},
                                       {64, 81, 67, 12, UNDEFINED, 36},
                                       {13, 48, 58, 82, UNDEFINED, 37},
                                       {83, 77, 79, 13, UNDEFINED, 38},
                                       {14, 84, 60, 64, UNDEFINED, 39},
                                       {85, 14, 74, 77, UNDEFINED, 40},
                                       {62, 75, 14, 17, UNDEFINED, 41},
                                       {49, 15, 86, 53, UNDEFINED, 42},
                                       {57, 87, 15, 60, UNDEFINED, 43},
                                       {16, 49, 88, 65, UNDEFINED, 44},
                                       {89, 72, 16, 74, UNDEFINED, 45},
                                       {17, 90, 57, 67, UNDEFINED, 46},
                                       {91, 17, 72, 79, UNDEFINED, 47},
                                       {35, 37, 18, 92, UNDEFINED, 48},
                                       {42, 44, 93, 18, UNDEFINED, 49},
                                       {19, 29, 35, 94, UNDEFINED, 50},
                                       {30, 19, 22, 95, UNDEFINED, 51},
                                       {96, 97, 98, 19, UNDEFINED, 52},
                                       {20, 32, 99, 42, UNDEFINED, 53},
                                       {33, 20, 100, 25, UNDEFINED, 54},
                                       {101, 102, 20, 98, UNDEFINED, 55},
                                       {27, 21, 29, 103, UNDEFINED, 56},
                                       {43, 104, 46, 21, UNDEFINED, 57},
                                       {22, 27, 37, 105, UNDEFINED, 58},
                                       {106, 100, 102, 22, UNDEFINED, 59},
                                       {23, 107, 39, 43, UNDEFINED, 60},
                                       {108, 23, 97, 100, UNDEFINED, 61},
                                       {41, 98, 23, 26, UNDEFINED, 62},
                                       {28, 24, 109, 32, UNDEFINED, 63},
                                       {36, 110, 24, 39, UNDEFINED, 64},
                                       {25, 28, 111, 44, UNDEFINED, 65},
                                       {112, 95, 25, 97, UNDEFINED, 66},
                                       {26, 113, 36, 46, UNDEFINED, 67},
                                       {114, 26, 95, 102, UNDEFINED, 68},
                                       {107, 108, 115, 27, UNDEFINED, 69},
                                       {113, 114, 28, 115, UNDEFINED, 70},
                                       {109, 112, 113, 29, UNDEFINED, 71},
                                       {116, 45, 47, 30, UNDEFINED, 72},
                                       {31, 96, 107, 109, UNDEFINED, 73},
                                       {97, 31, 40, 45, UNDEFINED, 74},
                                       {115, 41, 31, 34, UNDEFINED, 75},
                                       {103, 106, 32, 107, UNDEFINED, 76},
                                       {117, 38, 33, 40, UNDEFINED, 77},
                                       {34, 101, 103, 113, UNDEFINED, 78},
                                       {102, 34, 38, 47, UNDEFINED, 79},
                                       {99, 117, 101, 35, UNDEFINED, 80},
                                       {111, 36, 112, 117, UNDEFINED, 81},
                                       {110, 111, 114, 37, UNDEFINED, 82},
                                       {38, 99, 106, 110, UNDEFINED, 83},
                                       {92, 39, 96, 99, UNDEFINED, 84},
                                       {40, 92, 108, 111, UNDEFINED, 85},
                                       {94, 116, 42, 96, UNDEFINED, 86},
                                       {105, 43, 116, 106, UNDEFINED, 87},
                                       {104, 105, 44, 108, UNDEFINED, 88},
                                       {45, 94, 104, 112, UNDEFINED, 89},
                                       {93, 46, 94, 101, UNDEFINED, 90},
                                       {47, 93, 105, 114, UNDEFINED, 91},
                                       {84, 85, 118, 48, UNDEFINED, 92},
                                       {90, 91, 49, 118, UNDEFINED, 93},
                                       {86, 89, 90, 50, UNDEFINED, 94},
                                       {119, 66, 68, 51, UNDEFINED, 95},
                                       {52, 73, 84, 86, UNDEFINED, 96},
                                       {74, 52, 61, 66, UNDEFINED, 97},
                                       {118, 62, 52, 55, UNDEFINED, 98},
                                       {80, 83, 53, 84, UNDEFINED, 99},
                                       {120, 59, 54, 61, UNDEFINED, 100},
                                       {55, 78, 80, 90, UNDEFINED, 101},
                                       {79, 55, 59, 68, UNDEFINED, 102},
                                       {76, 120, 78, 56, UNDEFINED, 103},
                                       {88, 57, 89, 120, UNDEFINED, 104},
                                       {87, 88, 91, 58, UNDEFINED, 105},
                                       {59, 76, 83, 87, UNDEFINED, 106},
                                       {69, 60, 73, 76, UNDEFINED, 107},
                                       {61, 69, 85, 88, UNDEFINED, 108},
                                       {71, 119, 63, 73, UNDEFINED, 109},
                                       {82, 64, 119, 83, UNDEFINED, 110},
                                       {81, 82, 65, 85, UNDEFINED, 111},
                                       {66, 71, 81, 89, UNDEFINED, 112},
                                       {70, 67, 71, 78, UNDEFINED, 113},
                                       {68, 70, 82, 91, UNDEFINED, 114},
                                       {75, 118, 69, 70, UNDEFINED, 115},
                                       {72, 86, 87, 119, UNDEFINED, 116},
                                       {77, 80, 120, 81, UNDEFINED, 117},
                                       {98, 115, 92, 93, UNDEFINED, 118},
                                       {95, 109, 110, 116, UNDEFINED, 119},
                                       {100, 103, 117, 104, UNDEFINED, 120}}));
  }

  LIBSEMIGROUPS_TEST_CASE("Stephen",
                          "003",
                          "from step_hen 002",
                          "[quick][stephen]") {
    Presentation<std::string> p;
    p.alphabet("ab");
    presentation::add_rule(p, "aaa", "a");
    presentation::add_rule(p, "bbb", "b");
    presentation::add_rule(p, "abab", "aa");

    Stephen              S(p);
    detail::StringToWord string_to_word("ab");
    S.set_word(string_to_word("bbab"));

    REQUIRE(stephen::accepts(S, string_to_word("bbaaba")));
    REQUIRE(!stephen::accepts(S, string_to_word("")));
    REQUIRE(!stephen::accepts(S, string_to_word("aaaaaaaaaa")));
    REQUIRE(!stephen::accepts(S, string_to_word("bbb")));

    S.set_word(string_to_word("bba"));
    REQUIRE(stephen::accepts(S, string_to_word("bbabb")));
    REQUIRE(stephen::accepts(S, string_to_word("bba")));
    REQUIRE(!stephen::accepts(S, string_to_word("bbb")));
    REQUIRE(!stephen::accepts(S, string_to_word("a")));
    REQUIRE(!stephen::accepts(S, string_to_word("ab")));

    S.set_word(string_to_word("bbaab"));
    REQUIRE(stephen::accepts(S, string_to_word("bbaba")));
  }

  LIBSEMIGROUPS_TEST_CASE("Stephen",
                          "004",
                          "from step_hen 003",
                          "[quick][stephen]") {
    Presentation<std::string> p;
    p.alphabet("abcdefg");
    presentation::add_rule(p, "aaaeaa", "abcd");
    presentation::add_rule(p, "ef", "dg");

    Stephen              S(p);
    detail::StringToWord string_to_word("abcdefg");

    S.set_word(string_to_word("abcef")).run();
    REQUIRE(string_to_word("abcef") == word_type({0, 1, 2, 4, 5}));
    REQUIRE(S.word_graph()
            == to_word_graph<size_t>(
                11,
                {{1,
                  UNDEFINED,
                  UNDEFINED,
                  UNDEFINED,
                  UNDEFINED,
                  UNDEFINED,
                  UNDEFINED},
                 {2, 3, UNDEFINED, UNDEFINED, UNDEFINED, UNDEFINED, UNDEFINED},
                 {4,
                  UNDEFINED,
                  UNDEFINED,
                  UNDEFINED,
                  UNDEFINED,
                  UNDEFINED,
                  UNDEFINED},
                 {UNDEFINED,
                  UNDEFINED,
                  5,
                  UNDEFINED,
                  UNDEFINED,
                  UNDEFINED,
                  UNDEFINED},
                 {UNDEFINED,
                  UNDEFINED,
                  UNDEFINED,
                  UNDEFINED,
                  6,
                  UNDEFINED,
                  UNDEFINED},
                 {UNDEFINED, UNDEFINED, UNDEFINED, 7, 8, UNDEFINED, UNDEFINED},
                 {9,
                  UNDEFINED,
                  UNDEFINED,
                  UNDEFINED,
                  UNDEFINED,
                  UNDEFINED,
                  UNDEFINED},
                 {UNDEFINED,
                  UNDEFINED,
                  UNDEFINED,
                  UNDEFINED,
                  UNDEFINED,
                  UNDEFINED,
                  10},
                 {UNDEFINED,
                  UNDEFINED,
                  UNDEFINED,
                  UNDEFINED,
                  UNDEFINED,
                  10,
                  UNDEFINED},
                 {7,
                  UNDEFINED,
                  UNDEFINED,
                  UNDEFINED,
                  UNDEFINED,
                  UNDEFINED,
                  UNDEFINED},
                 {UNDEFINED,
                  UNDEFINED,
                  UNDEFINED,
                  UNDEFINED,
                  UNDEFINED,
                  UNDEFINED,
                  UNDEFINED}}));
    auto rule = string_to_word(p.rules[0]);
    auto m    = word_graph::last_node_on_path(
                 S.word_graph(), 0, rule.cbegin(), rule.cend())
                 .first;
    rule   = string_to_word(p.rules[1]);
    auto n = word_graph::last_node_on_path(
                 S.word_graph(), 0, rule.cbegin(), rule.cend())
                 .first;
    REQUIRE(m != UNDEFINED);
    REQUIRE(n != UNDEFINED);
    REQUIRE(m == n);
    REQUIRE(S.word_graph().number_of_nodes() == 11);
    REQUIRE(stephen::accepts(S, string_to_word("aaaeaag")));
    REQUIRE(stephen::number_of_words_accepted(S) == 3);
    REQUIRE(std::vector<word_type>(stephen::cbegin_words_accepted(S),
                                   stephen::cend_words_accepted(S))
            == std::vector<word_type>(
                {{0, 1, 2, 3, 6}, {0, 1, 2, 4, 5}, {0, 0, 0, 4, 0, 0, 6}}));

    S.set_word(string_to_word("aaaeaaaeaa")).run();

    REQUIRE(S.word_graph().number_of_nodes() == 15);
    REQUIRE(stephen::number_of_words_accepted(S) == 3);
    REQUIRE(stephen::accepts(S, string_to_word("aaaeabcd")));
    REQUIRE(std::vector<word_type>(stephen::cbegin_words_accepted(S),
                                   stephen::cend_words_accepted(S))
            == std::vector<word_type>({{0, 0, 0, 4, 0, 1, 2, 3},
                                       {0, 1, 2, 3, 0, 4, 0, 0},
                                       {0, 0, 0, 4, 0, 0, 0, 4, 0, 0}}));

    S.set_word(string_to_word("aaaeaag")).run();
    REQUIRE(S.word_graph().number_of_nodes() == 11);
    REQUIRE(stephen::number_of_words_accepted(S) == 3);
    REQUIRE(std::vector<word_type>(stephen::cbegin_words_accepted(S),
                                   stephen::cend_words_accepted(S))
            == std::vector<word_type>(
                {{0, 1, 2, 3, 6}, {0, 1, 2, 4, 5}, {0, 0, 0, 4, 0, 0, 6}}));
  }

  LIBSEMIGROUPS_TEST_CASE("Stephen",
                          "005",
                          "from step_hen 004",
                          "[quick][stephen]") {
    Presentation<std::string> p;
    p.alphabet("abc");
    presentation::add_rule(p, "ab", "ba");
    presentation::add_rule(p, "ac", "cc");
    presentation::add_rule(p, "ac", "a");
    presentation::add_rule(p, "cc", "a");
    presentation::add_rule(p, "bc", "cc");
    presentation::add_rule(p, "bcc", "b");
    presentation::add_rule(p, "bc", "b");
    presentation::add_rule(p, "cc", "b");
    presentation::add_rule(p, "a", "b");

    Stephen              S(p);
    detail::StringToWord string_to_word("abc");
    S.set_word(string_to_word("abcc")).run();
    REQUIRE(stephen::accepts(S, string_to_word("baac")));
    REQUIRE(S.word_graph().number_of_nodes() == 3);
    REQUIRE(stephen::number_of_words_accepted(S) == POSITIVE_INFINITY);
  }

  LIBSEMIGROUPS_TEST_CASE("Stephen",
                          "006",
                          "from step_hen 005",
                          "[quick][stephen]") {
    Presentation<std::string> p;
    p.alphabet("abcd");
    presentation::add_rule(p, "bb", "c");
    presentation::add_rule(p, "caca", "abab");
    presentation::add_rule(p, "bc", "d");
    presentation::add_rule(p, "cb", "d");
    presentation::add_rule(p, "aa", "d");
    presentation::add_rule(p, "ad", "a");
    presentation::add_rule(p, "da", "a");
    presentation::add_rule(p, "bd", "b");
    presentation::add_rule(p, "db", "b");
    presentation::add_rule(p, "cd", "c");
    presentation::add_rule(p, "dc", "c");

    Stephen              S(p);
    detail::StringToWord string_to_word("abcd");
    S.set_word(string_to_word("dabdaaadabab")).run();
    REQUIRE(stephen::accepts(S, string_to_word("abdadcaca")));
    REQUIRE(S.word_graph().number_of_nodes() == 25);
    REQUIRE(stephen::number_of_words_accepted(S) == POSITIVE_INFINITY);
  }

  LIBSEMIGROUPS_TEST_CASE("Stephen",
                          "007",
                          "Fibonacci(4, 6)",
                          "[stephen][extreme]") {
    using namespace fpsemigroup;
    auto    rg = ReportGuard(true);
    Stephen S(fibonacci_semigroup(4, 6));
    S.set_word({0, 1, 2, 3}).run_for(std::chrono::seconds(10));
    REQUIRE(!S.finished());
  }

  LIBSEMIGROUPS_TEST_CASE(
      "Stephen",
      "008",
      "C(4) monoid normal form (test_case_knuth_bendix_055)",
      "[stephen][quick]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("abcdefg");
    presentation::add_rule(p, "abcd", "ce");
    presentation::add_rule(p, "df", "dg");

    detail::StringToWord string_to_word("abcdefg");
    Stephen              S(p);
    S.set_word(string_to_word("dfabcdf")).run();

    REQUIRE(S.word_graph().number_of_nodes() == 9);
    REQUIRE(stephen::number_of_words_accepted(S) == 8);

    auto words = std::vector<word_type>(stephen::cbegin_words_accepted(S),
                                        stephen::cend_words_accepted(S));
    std::vector<std::string> strings(words.size(), "");

    std::transform(
        words.cbegin(), words.cend(), strings.begin(), [&p](auto const& w) {
          return detail::word_to_string(p.alphabet(), w.cbegin(), w.cend());
        });

    REQUIRE(strings
            == std::vector<std::string>({"dfcef",
                                         "dfceg",
                                         "dgcef",
                                         "dgceg",
                                         "dfabcdf",
                                         "dfabcdg",
                                         "dgabcdf",
                                         "dgabcdg"}));
    std::sort(strings.begin(), strings.end(), LexicographicalCompare());
    REQUIRE(strings.front() == "dfabcdf");

    REQUIRE(std::all_of(
        strings.cbegin(), strings.cend(), [&S, &string_to_word](auto const& w) {
          return stephen::accepts(S, string_to_word(w));
        }));
    REQUIRE(stephen::number_of_words_accepted(S) == strings.size());

    S.set_word(string_to_word("abcdfceg")).run();
    REQUIRE(stephen::number_of_words_accepted(S) == 16);
    words = std::vector<word_type>(stephen::cbegin_words_accepted(S),
                                   stephen::cend_words_accepted(S));
    strings.resize(stephen::number_of_words_accepted(S));

    std::transform(
        words.cbegin(), words.cend(), strings.begin(), [&p](auto const& w) {
          return detail::word_to_string(p.alphabet(), w.cbegin(), w.cend());
        });

    std::sort(strings.begin(), strings.end(), LexicographicalCompare());
    REQUIRE(strings
            == std::vector<std::string>({"abcdfabcdf",
                                         "abcdfabcdg",
                                         "abcdfcef",
                                         "abcdfceg",
                                         "abcdgabcdf",
                                         "abcdgabcdg",
                                         "abcdgcef",
                                         "abcdgceg",
                                         "cefabcdf",
                                         "cefabcdg",
                                         "cefcef",
                                         "cefceg",
                                         "cegabcdf",
                                         "cegabcdg",
                                         "cegcef",
                                         "cegceg"}));

    REQUIRE(strings.front() == "abcdfabcdf");
    REQUIRE(stephen::accepts(S, string_to_word("abcdfabcdf")));
  }

  LIBSEMIGROUPS_TEST_CASE(
      "Stephen",
      "009",
      "C(4) monoid normal form (test_case_gap_smalloverlap_85)",
      "[stephen][quick]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("cab");
    presentation::add_rule(p, "aabc", "acba");

    detail::StringToWord string_to_word("cab");
    Stephen              S(p);
    S.set_word(string_to_word("a")).run();
    REQUIRE(!stephen::accepts(S, string_to_word("b")));

    S.set_word(string_to_word("aabcabc")).run();
    REQUIRE(stephen::accepts(S, string_to_word("aabccba")));

    S.set_word(string_to_word("aabccba")).run();
    REQUIRE(stephen::accepts(S, string_to_word("aabcabc")));

    S.set_word(string_to_word("acba")).run();
    auto words = std::vector<word_type>(stephen::cbegin_words_accepted(S),
                                        stephen::cend_words_accepted(S));
    std::vector<std::string> strings(words.size(), "");

    std::transform(
        words.cbegin(), words.cend(), strings.begin(), [&p](auto const& w) {
          return detail::word_to_string(p.alphabet(), w.cbegin(), w.cend());
        });

    REQUIRE(strings == std::vector<std::string>({"acba", "aabc"}));
    verify_c4_normal_form(p, "acba", "aabc");
  }

  LIBSEMIGROUPS_TEST_CASE("Stephen",
                          "010",
                          "code coverage",
                          "[stephen][quick]") {
    auto                      rg = ReportGuard(true);
    Presentation<std::string> p;
    REQUIRE_THROWS_AS(Stephen(p), LibsemigroupsException);
    p.alphabet("abcdefg");

    detail::StringToWord string_to_word("abcdefg");
    Stephen              S(p);
    auto                 w = string_to_word("abbbddbcbcbc");
    S.set_word(w);
    S.run();
    REQUIRE(S.finished());
    S.run();
    S.set_word(string_to_word("abbbddbcbcbc"));  // resets
    S.report_every(std::chrono::microseconds(10));

    S.run();

    Stephen T(S);
    REQUIRE(stephen::accepts(T, w));
    REQUIRE(!stephen::accepts(T, string_to_word("abbbd")));
    REQUIRE(stephen::number_of_words_accepted(T) == 1);
    REQUIRE(stephen::number_of_left_factors(T) == w.size() + 1);
    REQUIRE(stephen::accepts(S, w));
    REQUIRE(!stephen::accepts(S, string_to_word("abbbd")));
    REQUIRE(stephen::number_of_words_accepted(S) == 1);
    REQUIRE(stephen::number_of_left_factors(S) == w.size() + 1);

    Stephen U(std::move(S));
    REQUIRE(stephen::accepts(U, w));
    REQUIRE(!stephen::accepts(U, string_to_word("abbbd")));
    REQUIRE(stephen::number_of_words_accepted(U) == 1);
    REQUIRE(stephen::number_of_left_factors(U) == w.size() + 1);

    S = T;
    REQUIRE(stephen::accepts(T, w));
    REQUIRE(!stephen::accepts(T, string_to_word("abbbd")));
    REQUIRE(stephen::number_of_words_accepted(T) == 1);
    REQUIRE(stephen::number_of_left_factors(T) == w.size() + 1);
    REQUIRE(stephen::accepts(S, w));
    REQUIRE(!stephen::accepts(S, string_to_word("abbbd")));
    REQUIRE(stephen::number_of_words_accepted(S) == 1);
    REQUIRE(stephen::number_of_left_factors(S) == w.size() + 1);

    Stephen V;
    V = std::move(S);
    REQUIRE(stephen::accepts(V, w));
    REQUIRE(!stephen::accepts(V, string_to_word("abbbd")));
    REQUIRE(stephen::number_of_words_accepted(V) == 1);
    REQUIRE(stephen::number_of_left_factors(V) == w.size() + 1);
    REQUIRE(V.word() == w);
    REQUIRE(V.accept_state() == 12);
  }

  LIBSEMIGROUPS_TEST_CASE(
      "Stephen",
      "011",
      "C(4) monoid normal form (test_case_gap_smalloverlap_49)",
      "[stephen][quick]") {
    Presentation<std::string> p;
    p.alphabet("abcdefgh");

    presentation::add_rule(p, "abcd", "ce");
    presentation::add_rule(p, "df", "hd");
    verify_c4_equal_to(p, "abchd", "abcdf");
    verify_c4_equal_to(p, "abchd", "abchd");
    verify_c4_equal_to(p, "abchdf", "abchhd");
    verify_c4_equal_to(p, "abchd", "cef");
    verify_c4_equal_to(p, "cef", "abchd");
    verify_c4_not_equal_to(p, "abchf", "abcdf");

    verify_c4_equal_to(p, "hdfabce", "dffababcd");

    verify_c4_normal_form(p, "hdfabce", "dffababcd");
  }

  LIBSEMIGROUPS_TEST_CASE(
      "Stephen",
      "012",
      "C(4) monoid normal form (test_case_gap_smalloverlap_63)",
      "[stephen][quick]") {
    Presentation<std::string> p;
    p.alphabet("abcdefgh");

    presentation::add_rule(p, "afh", "bgh");
    presentation::add_rule(p, "hc", "d");
    verify_c4_equal_to(p, "afd", "bgd");
    verify_c4_equal_to(p, "bghcafhbgd", "afdafhafd");
    verify_c4_normal_form(p, "bghcafhbgd", "afdafhafd");
  }

  LIBSEMIGROUPS_TEST_CASE(
      "Stephen",
      "013",
      "C(4) monoid equal to (test_case_gap_smalloverlap_70)",
      "[stephen][quick]") {
    Presentation<std::string> p;
    p.alphabet("abcdefghij");

    presentation::add_rule(p, "afh", "bgh");
    presentation::add_rule(p, "hc", "de");
    presentation::add_rule(p, "ei", "j");

    verify_c4_equal_to(p, "afdj", "bgdj");
    verify_c4_not_equal_to(p, "xxxxxxxxxxxxxxxxxxxxxxx", "b");
  }

  LIBSEMIGROUPS_TEST_CASE("Stephen",
                          "014",
                          "C(4) monoid normal form (test_case_ex_3_13_14)",
                          "[stephen][quick]") {
    Presentation<std::string> p;
    p.alphabet("abcd");
    presentation::add_rule(p, "abbba", "cdc");
    verify_c4_normal_form(p, "cdcdcabbbabbbabbcd", "abbbadcabbbabbbabbcd");
    verify_c4_equal_to(p, "cdcdcabbbabbbabbcd", "abbbadcabbbabbbabbcd");

    verify_c4_equal_to(p, "abbbadcbbba", "cdabbbcdc");
    verify_c4_equal_to(p, "cdabbbcdc", "cdabbbcdc");
    verify_c4_normal_form(p, "cdabbbcdc", "abbbadcbbba");
  }

  LIBSEMIGROUPS_TEST_CASE("Stephen",
                          "015",
                          "C(4) monoid normal form (test_case_ex_3_15)",
                          "[stephen][quick]") {
    Presentation<std::string> p;
    p.alphabet("abcd");
    presentation::add_rule(p, "aabc", "acba");
    std::string original = "cbacbaabcaabcacbacba";
    std::string expected = "cbaabcabcaabcaabcabc";

    verify_c4_equal_to(p, "cbaabcabcaabcaabccba", original);
    verify_c4_equal_to(p, original, expected);
    verify_c4_equal_to(p, expected, original);
    verify_c4_equal_to(p, "cbaabcabcaabcaabccba", expected);

    verify_c4_equal_to(p, original, "cbaabcabcaabcaabccba");

    verify_c4_equal_to(p, expected, "cbaabcabcaabcaabccba");
    verify_c4_normal_form(p, original, expected);
  }

  LIBSEMIGROUPS_TEST_CASE("Stephen",
                          "016",
                          "C(4) monoid normal form (test_case_ex_3_16)",
                          "[stephen][quick]") {
    Presentation<std::string> p;
    p.alphabet("abcd");
    presentation::add_rule(p, "abcd", "acca");
    std::string original = "bbcabcdaccaccabcddd";
    std::string expected = "bbcabcdabcdbcdbcddd";

    verify_c4_equal_to(p, original, expected);
    verify_c4_equal_to(p, expected, original);

    verify_c4_normal_form(p, original, expected);
    verify_c4_normal_form(p, expected, expected);
  }

  LIBSEMIGROUPS_TEST_CASE("Stephen",
                          "017",
                          "C(4) monoid normal form (test_case_mt_3)",
                          "[stephen][quick]") {
    Presentation<std::string> p;
    p.alphabet("abcd");
    presentation::add_rule(p, "abcd", "accca");

    verify_c4_normal_form(p, "bbcabcdaccaccabcddd", "bbcabcdaccaccabcddd");
    verify_c4_equal_to(p, "bbcabcdaccaccabcddd", "bbcabcdaccaccabcddd");
  }

  LIBSEMIGROUPS_TEST_CASE("Stephen",
                          "018",
                          "C(4) monoid normal form (test_case_mt_5)",
                          "[stephen][quick]") {
    Presentation<std::string> p;
    p.alphabet("abc");
    presentation::add_rule(p, "ac", "cbbbbc");

    verify_c4_normal_form(p, "acbbbbc", "aac");
    verify_c4_equal_to(p, "acbbbbc", "aac");
  }

  LIBSEMIGROUPS_TEST_CASE("Stephen",
                          "019",
                          "C(4) monoid normal form (test_case_mt_6)",
                          "[stephen][quick]") {
    Presentation<std::string> p;
    p.alphabet("abc");
    presentation::add_rule(p, "ccab", "cbac");

    verify_c4_normal_form(p, "bacbaccabccabcbacbac", "bacbacbaccbaccbacbac");
    verify_c4_equal_to(p, "bacbaccabccabcbacbac", "bacbacbaccbaccbacbac");
    verify_c4_normal_form(p, "ccabcbaccab", "cbaccbacbac");
    verify_c4_equal_to(p, "ccabcbaccab", "cbaccbacbac");
  }

  LIBSEMIGROUPS_TEST_CASE("Stephen",
                          "020",
                          "C(4) monoid normal form (test_case_mt_10)",
                          "[stephen][quick]") {
    Presentation<std::string> p;
    p.alphabet("abcdefghij");
    presentation::add_rule(p, "afh", "bgh");
    presentation::add_rule(p, "hc", "de");
    presentation::add_rule(p, "ei", "j");

    verify_c4_normal_form(p, "bgdj", "afdei");
    verify_c4_equal_to(p, "bgdj", "afdei");
  }

  LIBSEMIGROUPS_TEST_CASE("Stephen",
                          "021",
                          "C(4) monoid normal form (test_case_mt_13)",
                          "[stephen][quick]") {
    Presentation<std::string> p;
    p.alphabet("abcd");
    presentation::add_rule(p, "abcd", "dcba");

    verify_c4_normal_form(p, "dcbdcba", "abcdbcd");
    verify_c4_equal_to(p, "dcbdcba", "abcdbcd");
  }

  LIBSEMIGROUPS_TEST_CASE("Stephen",
                          "022",
                          "C(4) monoid normal form (test_case_mt_14)",
                          "[stephen][quick]") {
    Presentation<std::string> p;
    p.alphabet("abcd");
    presentation::add_rule(p, "abca", "dcbd");

    verify_c4_normal_form(p, "dcbabca", "abcacbd");
    verify_c4_equal_to(p, "dcbabca", "abcacbd");
  }

  LIBSEMIGROUPS_TEST_CASE("Stephen",
                          "023",
                          "C(4) monoid normal form (test_case_mt_15)",
                          "[stephen][quick]") {
    Presentation<std::string> p;
    p.alphabet("abcd");
    presentation::add_rule(p, "abcd", "dcba");
    presentation::add_rule(p, "adda", "dbbd");

    verify_c4_normal_form(p, "dbbabcd", "addacba");
    verify_c4_equal_to(p, "dbbabcd", "addacba");
  }

  LIBSEMIGROUPS_TEST_CASE("Stephen",
                          "024",
                          "C(4) monoid normal form (test_case_mt_16)",
                          "[stephen][quick]") {
    Presentation<std::string> p;
    p.alphabet("abcdefg");
    presentation::add_rule(p, "abcd", "acca");
    presentation::add_rule(p, "gf", "ge");

    verify_c4_normal_form(p, "accabcdgf", "abcdbcdge");
    verify_c4_equal_to(p, "accabcdgf", "abcdbcdge");
  }

  LIBSEMIGROUPS_TEST_CASE("Stephen",
                          "025",
                          "C(4) monoid normal form (test_case_mt_17)",
                          "[stephen][quick]") {
    Presentation<std::string> p;
    p.alphabet("abcd");
    presentation::add_rule(
        p, "ababbabbbabbbb", "abbbbbabbbbbbabbbbbbbabbbbbbbb");
    presentation::add_rule(
        p, "cdcddcdddcdddd", "cdddddcddddddcdddddddcdddddddd");

    verify_c4_normal_form(
        p, "abbbacdddddcddddddcdddddddcdddddddd", "abbbacdcddcdddcdddd");
    verify_c4_equal_to(
        p, "abbbacdddddcddddddcdddddddcdddddddd", "abbbacdcddcdddcdddd");
  }

  LIBSEMIGROUPS_TEST_CASE("Stephen",
                          "026",
                          "C(4) monoid normal form (test_case_weak_1)",
                          "[stephen][quick]") {
    Presentation<std::string> p;
    p.alphabet("abcd");
    presentation::add_rule(p, "acba", "aabc");
    presentation::add_rule(p, "acba", "dbbbd");

    verify_c4_equal_to(p, "aaabc", "adbbbd");
    verify_c4_equal_to(p, "adbbbd", "aaabc");

    verify_c4_equal_to(p, "aaabcadbbbd", "adbbbdadbbbd");
    verify_c4_equal_to(p, "aaabcaaabc", "adbbbdadbbbd");
    verify_c4_equal_to(p, "acba", "dbbbd");
    verify_c4_equal_to(p, "acbabbbd", "aabcbbbd");
    verify_c4_equal_to(p, "aabcbbbd", "acbabbbd");
  }

  LIBSEMIGROUPS_TEST_CASE("Stephen",
                          "027",
                          "C(4) monoid normal form (test_case_weak_2)",
                          "[stephen][quick]") {
    Presentation<std::string> p;
    p.alphabet("abcd");
    presentation::add_rule(p, "acba", "aabc");
    presentation::add_rule(p, "acba", "adbd");
    verify_c4_equal_to(p, "acbacba", "aabcabc");
    verify_c4_normal_form(p, "acbacba", "aabcabc");
    verify_c4_equal_to(p, "aabcabc", "acbacba");
    verify_c4_normal_form(p, "aabcabc", "aabcabc");
  }

  LIBSEMIGROUPS_TEST_CASE("Stephen",
                          "028",
                          "C(4) monoid normal form (test_case_weak_3)",
                          "[stephen][quick]") {
    Presentation<std::string> p;
    p.alphabet("abcde");
    presentation::add_rule(p, "bceac", "aeebbc");
    presentation::add_rule(p, "aeebbc", "dabcd");
    verify_c4_normal_form(p, "bceacdabcd", "aeebbcaeebbc");
    verify_c4_normal_form(p, "aeebbcaeebbc", "aeebbcaeebbc");
  }

  LIBSEMIGROUPS_TEST_CASE("Stephen",
                          "029",
                          "C(4) monoid normal form (test_case_weak_4)",
                          "[stephen][quick]") {
    Presentation<std::string> p;
    p.alphabet("abcd");
    presentation::add_rule(p, "acba", "aabc");
    presentation::add_rule(p, "acba", "dbbd");
    verify_c4_normal_form(p, "bbacbcaaabcbbd", "bbacbcaaabcbbd");
    verify_c4_normal_form(p, "acbacba", "aabcabc");
    verify_c4_normal_form(p, "aabcabc", "aabcabc");
  }

  LIBSEMIGROUPS_TEST_CASE("Stephen",
                          "030",
                          "C(4) monoid normal form (test_case_weak_5)",
                          "[stephen][quick]") {
    Presentation<std::string> p;
    p.alphabet("abcd");
    presentation::add_rule(p, "acba", "aabc");
    presentation::add_rule(p, "acba", "adbd");
    verify_c4_normal_form(p, "acbacba", "aabcabc");
    verify_c4_normal_form(p, "aabcabc", "aabcabc");
  }

  LIBSEMIGROUPS_TEST_CASE("Stephen",
                          "031",
                          "Test behaviour when uninitialised",
                          "[stephen][quick]") {
    Stephen S;

    REQUIRE_THROWS_AS(S.accept_state(), LibsemigroupsException);
    REQUIRE_THROWS_AS(stephen::cbegin_words_accepted(S),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(stephen::cend_words_accepted(S), LibsemigroupsException);
    REQUIRE_THROWS_AS(stephen::cbegin_left_factors(S), LibsemigroupsException);
    REQUIRE_THROWS_AS(stephen::cend_left_factors(S), LibsemigroupsException);

    REQUIRE_THROWS_AS(stephen::is_left_factor(S, {0, 0, 0}),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(stephen::accepts(S, {0, 0, 0}), LibsemigroupsException);
    REQUIRE_THROWS_AS(stephen::number_of_left_factors(S),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(stephen::number_of_words_accepted(S),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(S.run(), LibsemigroupsException);
  }

  /* LIBSEMIGROUPS_TEST_CASE("Stephen",
                           "032",
                           "(inverse presentation) "
                           "step_hen/tests/test_schutzenbergergraph.py:test_001",
                           "[stephen][quick]") {
     detail::StringToWord string_to_word("abcABC");

     InversePresentation<word_type> p;
     p.alphabet(string_to_word("abcABC"));
     p.inverses(string_to_word("ABCabc"));

     auto S = v3::Stephen<InversePresentation<word_type>>(p);

     S.set_word(string_to_word("aBcAbC")).run();

     REQUIRE(S.finished());
     REQUIRE(S.word_graph().number_of_nodes() == 7);
     REQUIRE(!v3::stephen::accepts(S, string_to_word("BaAbaBcAbC")));
     REQUIRE(v3::stephen::accepts(S, string_to_word("aBcCbBcAbC")));

     S.set_word(string_to_word("aBcCbBcAbC"));
     REQUIRE(v3::stephen::accepts(S, string_to_word("aBcAbC")));

     S.set_word(string_to_word("BaAbaBcAbC"));
     REQUIRE(v3::stephen::accepts(S, string_to_word("aBcAbC")));
   }

   LIBSEMIGROUPS_TEST_CASE("Stephen",
                           "033",
                           "(inverse presentation) "
                           "step_hen/tests/test_schutzenbergergraph.py:test_001",
                           "[stephen][quick]") {
     InversePresentation<std::string> p;
     p.alphabet("abcABC");
     p.inverses("ABCabc");

     auto S = v3::Stephen<InversePresentation<word_type>>(p);

     detail::StringToWord string_to_word(p.alphabet());

     S.set_word(string_to_word("aBcAbC")).run();

     REQUIRE(S.finished());
     REQUIRE(S.word_graph().number_of_nodes() == 7);
     REQUIRE(!v3::stephen::accepts(S, string_to_word("BaAbaBcAbC")));
     REQUIRE(v3::stephen::accepts(S, string_to_word("aBcCbBcAbC")));

     S.set_word(string_to_word("aBcCbBcAbC"));
     REQUIRE(v3::stephen::accepts(S, string_to_word("aBcAbC")));

     S.set_word(string_to_word("BaAbaBcAbC"));
     REQUIRE(v3::stephen::accepts(S, string_to_word("aBcAbC")));
   }

   LIBSEMIGROUPS_TEST_CASE("Stephen",
                           "034",
                           "(inverse presentation) "
                           "step_hen/tests/test_schutzenbergergraph.py:test_002",
                           "[stephen][quick]") {
     detail::StringToWord           string_to_word("abcABC");
     InversePresentation<word_type> p;
     p.alphabet(string_to_word("abcABC"));
     p.inverses(string_to_word("ABCabc"));

     auto S = v3::Stephen<InversePresentation<word_type>>(p);
     S.set_word(string_to_word("aBbcABAabCc")).run();

     REQUIRE(S.accept_state() == 4);
     REQUIRE(action_digraph_helper::follow_path(S.word_graph(), 0, S.word())
             == 4);
     REQUIRE(v3::stephen::number_of_words_accepted(S) == POSITIVE_INFINITY);
   }

   LIBSEMIGROUPS_TEST_CASE("Stephen",
                           "035",
                           "(inverse presentation) "
                           "step_hen/tests/test_schutzenbergergraph.py:test_003",
                           "[stephen][quick]") {
     detail::StringToWord           string_to_word("xyXY");
     InversePresentation<word_type> p;
     p.alphabet(string_to_word("xyXY"));
     p.inverses(string_to_word("XYxy"));

     auto S = v3::Stephen<InversePresentation<word_type>>(p);
     S.set_word(string_to_word("xxxyyy")).run();

     REQUIRE(v3::stephen::accepts(S, string_to_word("xxxyyyYYYXXXxxxyyy")));
     S.set_word(string_to_word("xxxyyyYYYXXXxxxyyy"));
     REQUIRE(v3::stephen::accepts(S, string_to_word("xxxyyy")));
     REQUIRE(!v3::stephen::accepts(S, string_to_word("xxx")));
   }

   LIBSEMIGROUPS_TEST_CASE("Stephen",
                           "036",
                           "(inverse presentation) "
                           "step_hen/tests/test_schutzenbergergraph.py:test_004",
                           "[stephen][quick]") {
     detail::StringToWord           string_to_word("xyXY");
     InversePresentation<word_type> p;
     p.alphabet(string_to_word("xyXY"));
     p.inverses(string_to_word("XYxy"));
     presentation::add_rule_and_check(
         p, string_to_word("xyXxyX"), string_to_word("xyX"));

     auto S = v3::Stephen<InversePresentation<word_type>>(p);
     // FIXME seems like every word is accepted when we don't set the word
     S.set_word(string_to_word("xyXyy"));
     std::string ys = "";
     for (size_t i = 0; i < 10; ++i) {
       REQUIRE(v3::stephen::accepts(
           S, string_to_word(std::string("x") + ys + "Xyy")));
       ys += "y";
     }

     REQUIRE(!v3::stephen::accepts(S, string_to_word("xXyx")));
     REQUIRE(!v3::stephen::accepts(S, string_to_word("xXxx")));
     REQUIRE(!v3::stephen::accepts(S, string_to_word("xXxy")));
     REQUIRE(!v3::stephen::accepts(S, string_to_word("xXxX")));
     REQUIRE(!v3::stephen::accepts(S, string_to_word("xXyY")));
     REQUIRE(v3::stephen::accepts(S, string_to_word("xyXyy")));
     REQUIRE(v3::stephen::number_of_words_accepted(S) == POSITIVE_INFINITY);
     REQUIRE(S.word_graph().number_of_nodes() == 4);
     REQUIRE(S.word_graph().number_of_edges() == 8);

     REQUIRE(S.word_graph()
             == action_digraph_helper::make<typename Stephen::node_type>(
                 4,
                 {{1, 2, UNDEFINED, UNDEFINED},
                  {UNDEFINED, 1, 0, 1},
                  {UNDEFINED, 3, UNDEFINED, 0},
                  {UNDEFINED, UNDEFINED, UNDEFINED, 2}}));
   }

   LIBSEMIGROUPS_TEST_CASE("Stephen",
                           "037",
                           "(inverse presentation) "
                           "step_hen/tests/test_schutzenbergergraph.py:test_005",
                           "[stephen][quick]") {
     detail::StringToWord           string_to_word("xyXY");
     InversePresentation<word_type> p;
     p.alphabet(string_to_word("xyXY"));
     p.inverses(string_to_word("XYxy"));
     presentation::add_rule_and_check(
         p, string_to_word("xyXxyX"), string_to_word("xyX"));
     presentation::add_rule_and_check(
         p, string_to_word("xyxy"), string_to_word("xy"));

     auto S = v3::Stephen<InversePresentation<word_type>>(p);
     S.set_word(string_to_word("xyXyy"));
     REQUIRE(v3::stephen::accepts(S, string_to_word("y")));
     REQUIRE(v3::stephen::accepts(S, string_to_word("xxxxxxxxxxxxx")));
     REQUIRE(v3::stephen::accepts(S, string_to_word("xyXxyxyxyxyxyXyy")));
     REQUIRE(S.word_graph().number_of_nodes() == 1);
   }

   LIBSEMIGROUPS_TEST_CASE("Stephen",
                           "038",
                           "(inverse presentation) "
                           "step_hen/tests/test_schutzenbergergraph.py:test_006",
                           "[stephen][quick]") {
     detail::StringToWord           string_to_word("abcABC");
     InversePresentation<word_type> p;
     p.alphabet(string_to_word("abcABC"));
     p.inverses(string_to_word("ABCabc"));
     presentation::add_rule_and_check(
         p, string_to_word("ac"), string_to_word("ca"));
     presentation::add_rule_and_check(
         p, string_to_word("ab"), string_to_word("ba"));
     presentation::add_rule_and_check(
         p, string_to_word("bc"), string_to_word("cb"));

     auto S = v3::Stephen<InversePresentation<word_type>>(p);
     S.set_word(string_to_word("BaAbaBcAbC"));
     S.run();
     REQUIRE(S.word_graph().number_of_nodes() == 7);
     REQUIRE(S.word_graph()
             == action_digraph_helper::make<typename Stephen::node_type>(
                 7,
                 {{1, UNDEFINED, 2, UNDEFINED, 3, UNDEFINED},
                  {UNDEFINED, UNDEFINED, UNDEFINED, 0, 4, UNDEFINED},
                  {UNDEFINED, UNDEFINED, UNDEFINED, UNDEFINED, 5, 0},
                  {4, 0, 5, UNDEFINED, UNDEFINED, UNDEFINED},
                  {UNDEFINED, 1, 6, 3, UNDEFINED, UNDEFINED},
                  {6, 2, UNDEFINED, UNDEFINED, UNDEFINED, 3},
                  {UNDEFINED, UNDEFINED, UNDEFINED, 5, UNDEFINED, 4}}));
   }

   LIBSEMIGROUPS_TEST_CASE("Stephen", "039", "corner case", "[stephen][quick]")
   { detail::StringToWord string_to_word("x");

     Presentation<word_type> p;
     p.contains_empty_word(true);

     p.alphabet(string_to_word("x"));
     presentation::add_rule_and_check(
         p, string_to_word("xxxx"), string_to_word("xx"));

     Stephen S(p);
     S.set_word(string_to_word(""));
     S.run();
     REQUIRE(S.accept_state() == 0);
     REQUIRE(S.word_graph().number_of_nodes() == 1);
     REQUIRE(!stephen::accepts(S, string_to_word("x")));
   }

   LIBSEMIGROUPS_TEST_CASE("Stephen", "040", "empty word", "[stephen][quick]") {
     auto p = fpsemigroup::make<Presentation<word_type>>(
         fpsemigroup::symmetric_inverse_monoid(4));
     REQUIRE(p.contains_empty_word());
     REQUIRE(p.alphabet().size() == 4);

     auto s = Stephen(p);
     s.set_word({}).run();
     REQUIRE(s.word_graph().number_of_nodes() == 24);
     s.set_word({0}).run();
     REQUIRE(s.word_graph().number_of_nodes() == 24);
     s.set_word({1}).run();
     REQUIRE(s.word_graph().number_of_nodes() == 24);
     s.set_word({2}).run();
     REQUIRE(s.word_graph().number_of_nodes() == 24);
     s.set_word({3}).run();
     REQUIRE(s.word_graph().number_of_nodes() == 48);
   }

   LIBSEMIGROUPS_TEST_CASE("Stephen", "041", "shared_ptr", "[stephen][quick]") {
     detail::StringToWord           string_to_word("abcABC");
     InversePresentation<word_type> p;
     p.alphabet(string_to_word("abcABC"));
     p.inverses(string_to_word("ABCabc"));
     presentation::add_rule_and_check(
         p, string_to_word("ac"), string_to_word("ca"));
     presentation::add_rule_and_check(
         p, string_to_word("ab"), string_to_word("ba"));
     presentation::add_rule_and_check(
         p, string_to_word("bc"), string_to_word("cb"));

     auto ptr = std::make_shared<decltype(p)>(p);
     auto S = v3::Stephen<std::shared_ptr<InversePresentation<word_type>>>(ptr);
     S.set_word(string_to_word("BaAbaBcAbC"));
     S.run();
     REQUIRE(S.word_graph().number_of_nodes() == 7);
     REQUIRE(S.word_graph()
             == action_digraph_helper::make<typename Stephen::node_type>(
                 7,
                 {{1, UNDEFINED, 2, UNDEFINED, 3, UNDEFINED},
                  {UNDEFINED, UNDEFINED, UNDEFINED, 0, 4, UNDEFINED},
                  {UNDEFINED, UNDEFINED, UNDEFINED, UNDEFINED, 5, 0},
                  {4, 0, 5, UNDEFINED, UNDEFINED, UNDEFINED},
                  {UNDEFINED, 1, 6, 3, UNDEFINED, UNDEFINED},
                  {6, 2, UNDEFINED, UNDEFINED, UNDEFINED, 3},
                  {UNDEFINED, UNDEFINED, UNDEFINED, 5, UNDEFINED, 4}}));
   }

   LIBSEMIGROUPS_TEST_CASE("Stephen",
                           "042",
                           "inverse presentation -- operator==",
                           "[stephen][quick]") {
     congruence::ToddCoxeter tc(congruence_kind::twosided);
     {
       auto p = fpsemigroup::make<Presentation<word_type>>(
           fpsemigroup::symmetric_inverse_monoid(4));
       REQUIRE(p.contains_empty_word());
       REQUIRE(p.alphabet().size() == 4);
       p.alphabet(5);
       presentation::replace_word(p, {}, {4});
       presentation::add_identity_rules(p, 4);
       p.validate();

       tc.set_number_of_generators(5);
       for (auto it = p.rules.cbegin(); it != p.rules.cend(); it += 2) {
         tc.add_pair(*it, *(it + 1));
       }
     }

     {
       InversePresentation<word_type> p
           = fpsemigroup::make<InversePresentation<word_type>>(
               fpsemigroup::symmetric_inverse_monoid(4));
       p.alphabet(4);
       p.inverses({0, 1, 2, 3});
       p.validate();

       auto      S = v3::Stephen<InversePresentation<word_type>>(p);
       word_type w = {0, 1, 2, 0, 1, 1, 2, 0, 1, 0, 2, 2, 2, 2, 0, 1};
       S.set_word(w);

       REQUIRE(v3::stephen::number_of_words_accepted(S) == POSITIVE_INFINITY);

       {
         auto const index = tc.word_to_class_index(w);
         auto       first = v3::stephen::cbegin_words_accepted(S);
         auto       last  = first;
         std::advance(last, 1024);
         auto T = v3::Stephen<InversePresentation<word_type>>(p);

         for (auto it = first; it != last; ++it) {
           REQUIRE(tc.word_to_class_index(*it) == index);
           REQUIRE(v3::stephen::accepts(T.set_word(*it), w));
         }
       }
     }
   }

   LIBSEMIGROUPS_TEST_CASE("Stephen",
                           "044",
                           "inverse presentation",
                           "[stephen][quick]") {
     detail::StringToWord           string_to_word("abcABC");
     InversePresentation<word_type> p;
     p.alphabet(string_to_word("abcABC"));
     p.inverses(string_to_word("ABCabc"));
     presentation::add_rule_and_check(
         p, string_to_word("ac"), string_to_word("ca"));
     presentation::add_rule_and_check(
         p, string_to_word("ab"), string_to_word("ba"));
     presentation::add_rule_and_check(
         p, string_to_word("bc"), string_to_word("cb"));
     auto S = v3::Stephen<InversePresentation<word_type>>(p);
     auto T = v3::Stephen<InversePresentation<word_type>>(p);

     Sislo sislo;
     sislo.alphabet("abcABC").first("aaaaa").last("aaaaaaaaa");
     auto sislo_end = sislo.cend();

     for (auto w = sislo.cbegin(); w != sislo_end; ++w) {
       S.set_word(string_to_word(*w));
       auto first = ++v3::stephen::cbegin_words_accepted(S);
       auto last  = first;
       std::advance(last, 1024);
       for (auto it = first; it != last; ++it) {
         REQUIRE(v3::stephen::accepts(S, *it));
         if (!v3::stephen::accepts(T.set_word(*it), S.word())) {
           std::cout << "S.word() = " << S.word() << ", T.word() = " << T.word()
                     << std::endl;
         }
         REQUIRE(S == T);
       }
     }
   }

   LIBSEMIGROUPS_TEST_CASE("Stephen",
                           "043",
                           "non-inverse presentation -- operator==",
                           "[stephen][quick]") {
     auto p = fpsemigroup::make<Presentation<word_type>>(
         fpsemigroup::symmetric_inverse_monoid(4));
     REQUIRE(p.contains_empty_word());
     REQUIRE(p.alphabet().size() == 4);
     p.alphabet(5);
     presentation::replace_word(p, {}, {4});
     presentation::add_identity_rules(p, 4);
     p.validate();

     congruence::ToddCoxeter tc(congruence_kind::twosided);
     tc.set_number_of_generators(5);
     for (auto it = p.rules.cbegin(); it != p.rules.cend(); it += 2) {
       tc.add_pair(*it, *(it + 1));
     }

     auto      S = v3::Stephen<Presentation<word_type>>(p);
     word_type w = {0, 1, 2, 0, 1, 1, 2, 0, 1, 0, 2, 2, 2, 2, 0, 1};
     S.set_word(w);
     REQUIRE(v3::stephen::number_of_words_accepted(S) == POSITIVE_INFINITY);

     {
       auto const index = tc.word_to_class_index(w);
       auto       first = v3::stephen::cbegin_words_accepted(S);
       auto       last  = first;
       std::advance(last, 1024);

       for (auto it = first; it != last; ++it) {
         REQUIRE(tc.word_to_class_index(*it) == index);
       }
     }
     {
       auto first = tc.cbegin_class(w);
       auto last  = first;
       std::advance(last, 1024);

       for (auto it = first; it != last; ++it) {
         REQUIRE(stephen::accepts(S, *it));
       }
     }
   }*/

  LIBSEMIGROUPS_TEST_CASE("Stephen",
                          "032",
                          "Plactic monoid",
                          "[stephen][quick]") {
    auto p = fpsemigroup::plactic_monoid(4);
    p.contains_empty_word(true);
    Stephen s(p);
    s.set_word(0013122_w).run();
    REQUIRE(!stephen::accepts(s, 0013212_w));
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "033",
                          "Whyte's 4-relation full transf monoid 8",
                          "[todd-coxeter][extreme]") {
    auto                    rg = ReportGuard(true);
    Presentation<word_type> p;
    p.rules = {00_w,       {},         11_w,         {},           22_w,
               {},         33_w,       {},           44_w,         {},
               55_w,       {},         66_w,         {},           010101_w,
               {},         121212_w,   {},           232323_w,     {},
               343434_w,   {},         454545_w,     {},           565656_w,
               {},         606060_w,   {},           01020102_w,   {},
               01030103_w, {},         01040104_w,   {},           01050105_w,
               {},         01060106_w, {},           12101210_w,   {},
               12131213_w, {},         12141214_w,   {},           12151215_w,
               {},         12161216_w, {},           23202320_w,   {},
               23212321_w, {},         23242324_w,   {},           23252325_w,
               {},         23262326_w, {},           34303430_w,   {},
               34313431_w, {},         34323432_w,   {},           34353435_w,
               {},         34363436_w, {},           45404540_w,   {},
               45414541_w, {},         45424542_w,   {},           45434543_w,
               {},         45464546_w, {},           56505650_w,   {},
               56515651_w, {},         56525652_w,   {},           56535653_w,
               {},         56545654_w, {},           60616061_w,   {},
               60626062_w, {},         60636063_w,   {},           60646064_w,
               {},         60656065_w, {},           071654321_w,  16543217_w,
               217121_w,   17171_w,    0102720107_w, 7010270102_w, 107017_w,
               70107010_w, 1217_w,     7121_w};
    p.alphabet_from_rules();
    // REQUIRE(presentation::length(p) == 398);
    // REQUIRE(presentation::longest_subword_reducing_length(p) == 010_w);
    // presentation::replace_word_with_new_generator(p, 010_w);
    // REQUIRE(presentation::length(p) == 368);
    // presentation::replace_word_with_new_generator(p, 010_w);

    Stephen s;
    s.init(p).set_word(1217_w);
    // TODO doing run_for and then checking and running some more doesn't seem
    // to work, seems to be going in a circle.
    // TODO the next is excessively slow takes about 2 minutes to return.
    s.run_until([&s]() {
      return word_graph::last_node_on_path(s.word_graph(), 0, 1217_w)
             == word_graph::last_node_on_path(s.word_graph(), 0, 7121_w);
    });
  }
}  // namespace libsemigroups
