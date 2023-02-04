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

#define CATCH_CONFIG_ENABLE_PAIR_STRINGMAKER

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

#include "libsemigroups/constants.hpp"             // for UNDEFINED
#include "libsemigroups/digraph-helper.hpp"        // for make, last_node_on...
#include "libsemigroups/digraph.hpp"               // for ActionDigraph, ope...
#include "libsemigroups/exception.hpp"             // for LibsemigroupsExcep...
#include "libsemigroups/fpsemi-examples.hpp"       // for make, fibonacci_se...
#include "libsemigroups/int-range.hpp"             // for IntegralRange<>::v...
#include "libsemigroups/iterator.hpp"              // for ConstIteratorState...
#include "libsemigroups/order.hpp"                 // for LexicographicalCom...
#include "libsemigroups/present.hpp"               // for add_rule_and_check
#include "libsemigroups/report.hpp"                // for ReportGuard
#include "libsemigroups/stephen.hpp"               // for Stephen, Stephen::...
#include "libsemigroups/todd-coxeter-digraph.hpp"  // for ToddCoxeterDigraph
#include "libsemigroups/types.hpp"                 // for word_type

namespace libsemigroups {
  namespace {
    void check_000(Stephen& s) {
      s.set_word({0}).run();
      REQUIRE(s.word_graph().number_of_nodes() == 2);
      REQUIRE(s.word_graph()
              == action_digraph_helper::make<size_t>(
                  2, {{1, UNDEFINED}, {UNDEFINED, 1}}));
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
      Stephen S(p);
      S.set_word(presentation::make<word_type>(p, word)).run();

      auto words = std::vector<word_type>(stephen::cbegin_words_accepted(S),
                                          stephen::cend_words_accepted(S));
      std::vector<std::string> strings(words.size(), "");

      std::transform(
          words.cbegin(), words.cend(), strings.begin(), [&p](auto const& w) {
            return presentation::make<std::string>(p, w);
          });

      std::sort(strings.begin(),
                strings.end(),
                LexicographicalCompare<std::string>());
      REQUIRE(strings.front() == nf);

      REQUIRE(std::all_of(
          strings.cbegin(), strings.cend(), [&S, &p](auto const& w) {
            return stephen::accepts(S, presentation::make<word_type>(p, w));
          }));
      REQUIRE(stephen::number_of_words_accepted(S) == strings.size());
    }

    void verify_c4_equal_to(Presentation<std::string> const& p,
                            std::string const&               word1,
                            std::string const&               word2) {
      Stephen S(p);
      S.set_word(presentation::make<word_type>(p, word1)).run();
      REQUIRE(stephen::accepts(S, presentation::make<word_type>(p, word2)));
      S.set_word(presentation::make<word_type>(p, word2)).run();
      REQUIRE(stephen::accepts(S, presentation::make<word_type>(p, word1)));
    }

    void verify_c4_not_equal_to(Presentation<std::string> const& p,
                                std::string const&               word1,
                                std::string const&               word2) {
      Stephen S(p);
      S.set_word(presentation::make<word_type>(p, word1)).run();
      REQUIRE(!stephen::accepts(S, presentation::make<word_type>(p, word2)));
      S.set_word(presentation::make<word_type>(p, word2)).run();
      REQUIRE(!stephen::accepts(S, presentation::make<word_type>(p, word1)));
    }

  }  // namespace

  LIBSEMIGROUPS_TEST_CASE("Stephen",
                          "000",
                          "basic test 1",
                          "[quick][stephen]") {
    auto                    rg = ReportGuard(true);
    Presentation<word_type> p;
    p.alphabet(2);
    presentation::add_rule_and_check(p, {0}, {0, 1});
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
    presentation::add_rule_and_check(p, {0, 0, 0}, {0});
    presentation::add_rule_and_check(p, {1, 1, 1}, {1});
    presentation::add_rule_and_check(p, {0, 1, 0, 1}, {0, 0});
    Stephen s(p);
    s.set_word({1, 1, 0, 1}).run();
    REQUIRE(s.word_graph().number_of_nodes() == 7);
    REQUIRE(s.word_graph()
            == action_digraph_helper::make<size_t>(7,
                                                   {{UNDEFINED, 1},
                                                    {UNDEFINED, 2},
                                                    {3, 1},
                                                    {4, 5},
                                                    {3, 6},
                                                    {6, 3},
                                                    {5, 4}}));
    REQUIRE(stephen::number_of_words_accepted(s) == POSITIVE_INFINITY);

    word_type w = {1, 1, 0, 1};

    REQUIRE(action_digraph_helper::last_node_on_path_nc(
                s.word_graph(), 0, w.begin(), w.end())
                .first
            == 5);
    w = {1, 1, 0, 0, 1, 0};
    REQUIRE(action_digraph_helper::last_node_on_path_nc(
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
            == action_digraph_helper::make<size_t>(
                5, {{1, UNDEFINED}, {2, 3}, {1, 4}, {4, 1}, {3, 2}}));

    p.rules.clear();
    presentation::add_rule_and_check(p, {0, 0, 0}, {0});
    presentation::add_rule_and_check(p, {1, 1, 1}, {1});
    s.init(p).set_word({0, 0}).run();
    REQUIRE(s.word_graph().number_of_nodes() == 3);
    REQUIRE(s.word_graph()
            == action_digraph_helper::make<size_t>(
                3, {{1, UNDEFINED}, {2, UNDEFINED}, {1, UNDEFINED}}));
  }

  LIBSEMIGROUPS_TEST_CASE("Stephen",
                          "002",
                          "full transf monoid",
                          "[quick][stephen]") {
    using namespace fpsemigroup;
    auto   rg = ReportGuard(true);
    size_t n  = 5;
    auto   p  = make<Presentation<word_type>>(
        full_transformation_monoid(n, author::Iwahori));
    presentation::replace_word(p, {}, {n});
    p.alphabet(n + 1);
    presentation::add_identity_rules(p, n);
    p.validate();

    Stephen s;
    s.init(std::move(p)).set_word({0, 1, 0, 1, 1, 1, 0, 2, 0, 1, 2, 0}).run();
    REQUIRE(s.word_graph().number_of_nodes() == 121);
    REQUIRE(s.word_graph()
            == action_digraph_helper::make<size_t>(
                121, {{1, 2, 3, 4, UNDEFINED, 5},
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
    presentation::add_rule_and_check(p, "aaa", "a");
    presentation::add_rule_and_check(p, "bbb", "b");
    presentation::add_rule_and_check(p, "abab", "aa");

    Stephen S(p);
    S.set_word(presentation::make<word_type>(p, "bbab"));

    REQUIRE(stephen::accepts(S, presentation::make<word_type>(p, "bbaaba")));
    REQUIRE(!stephen::accepts(S, presentation::make<word_type>(p, "")));
    REQUIRE(
        !stephen::accepts(S, presentation::make<word_type>(p, "aaaaaaaaaa")));
    REQUIRE(!stephen::accepts(S, presentation::make<word_type>(p, "bbb")));

    S.set_word(presentation::make<word_type>(p, "bba"));
    REQUIRE(stephen::accepts(S, presentation::make<word_type>(p, "bbabb")));
    REQUIRE(stephen::accepts(S, presentation::make<word_type>(p, "bba")));
    REQUIRE(!stephen::accepts(S, presentation::make<word_type>(p, "bbb")));
    REQUIRE(!stephen::accepts(S, presentation::make<word_type>(p, "a")));
    REQUIRE(!stephen::accepts(S, presentation::make<word_type>(p, "ab")));

    S.set_word(presentation::make<word_type>(p, "bbaab"));
    REQUIRE(stephen::accepts(S, presentation::make<word_type>(p, "bbaba")));
  }

  LIBSEMIGROUPS_TEST_CASE("Stephen",
                          "004",
                          "from step_hen 003",
                          "[quick][stephen]") {
    Presentation<std::string> p;
    p.alphabet("abcdefg");
    presentation::add_rule_and_check(p, "aaaeaa", "abcd");
    presentation::add_rule_and_check(p, "ef", "dg");

    Stephen S(p);

    S.set_word(presentation::make<word_type>(p, "abcef")).run();
    REQUIRE(presentation::make<word_type>(p, "abcef")
            == word_type({0, 1, 2, 4, 5}));
    REQUIRE(S.word_graph()
            == action_digraph_helper::make<size_t>(
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
    auto rule = presentation::make<word_type>(p, p.rules[0]);
    auto m    = action_digraph_helper::last_node_on_path(
                 S.word_graph(), 0, rule.cbegin(), rule.cend())
                 .first;
    rule   = presentation::make<word_type>(p, p.rules[1]);
    auto n = action_digraph_helper::last_node_on_path(
                 S.word_graph(), 0, rule.cbegin(), rule.cend())
                 .first;
    REQUIRE(m != UNDEFINED);
    REQUIRE(n != UNDEFINED);
    REQUIRE(m == n);
    REQUIRE(S.word_graph().number_of_nodes() == 11);
    REQUIRE(stephen::accepts(S, presentation::make<word_type>(p, "aaaeaag")));
    REQUIRE(stephen::number_of_words_accepted(S) == 3);
    REQUIRE(std::vector<word_type>(stephen::cbegin_words_accepted(S),
                                   stephen::cend_words_accepted(S))
            == std::vector<word_type>(
                {{0, 1, 2, 3, 6}, {0, 1, 2, 4, 5}, {0, 0, 0, 4, 0, 0, 6}}));

    S.set_word(presentation::make<word_type>(p, "aaaeaaaeaa")).run();

    REQUIRE(S.word_graph().number_of_nodes() == 15);
    REQUIRE(stephen::number_of_words_accepted(S) == 3);
    REQUIRE(stephen::accepts(S, presentation::make<word_type>(p, "aaaeabcd")));
    REQUIRE(std::vector<word_type>(stephen::cbegin_words_accepted(S),
                                   stephen::cend_words_accepted(S))
            == std::vector<word_type>({{0, 0, 0, 4, 0, 1, 2, 3},
                                       {0, 1, 2, 3, 0, 4, 0, 0},
                                       {0, 0, 0, 4, 0, 0, 0, 4, 0, 0}}));

    S.set_word(presentation::make<word_type>(p, "aaaeaag")).run();
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
    presentation::add_rule_and_check(p, "ab", "ba");
    presentation::add_rule_and_check(p, "ac", "cc");
    presentation::add_rule_and_check(p, "ac", "a");
    presentation::add_rule_and_check(p, "cc", "a");
    presentation::add_rule_and_check(p, "bc", "cc");
    presentation::add_rule_and_check(p, "bcc", "b");
    presentation::add_rule_and_check(p, "bc", "b");
    presentation::add_rule_and_check(p, "cc", "b");
    presentation::add_rule_and_check(p, "a", "b");

    Stephen S(p);
    S.set_word(presentation::make<word_type>(p, "abcc")).run();
    REQUIRE(stephen::accepts(S, presentation::make<word_type>(p, "baac")));
    REQUIRE(S.word_graph().number_of_nodes() == 3);
    REQUIRE(stephen::number_of_words_accepted(S) == POSITIVE_INFINITY);
  }

  LIBSEMIGROUPS_TEST_CASE("Stephen",
                          "006",
                          "from step_hen 005",
                          "[quick][stephen]") {
    Presentation<std::string> p;
    p.alphabet("abcd");
    presentation::add_rule_and_check(p, "bb", "c");
    presentation::add_rule_and_check(p, "caca", "abab");
    presentation::add_rule_and_check(p, "bc", "d");
    presentation::add_rule_and_check(p, "cb", "d");
    presentation::add_rule_and_check(p, "aa", "d");
    presentation::add_rule_and_check(p, "ad", "a");
    presentation::add_rule_and_check(p, "da", "a");
    presentation::add_rule_and_check(p, "bd", "b");
    presentation::add_rule_and_check(p, "db", "b");
    presentation::add_rule_and_check(p, "cd", "c");
    presentation::add_rule_and_check(p, "dc", "c");

    Stephen S(p);
    S.set_word(presentation::make<word_type>(p, "dabdaaadabab")).run();
    REQUIRE(stephen::accepts(S, presentation::make<word_type>(p, "abdadcaca")));
    REQUIRE(S.word_graph().number_of_nodes() == 25);
    REQUIRE(stephen::number_of_words_accepted(S) == POSITIVE_INFINITY);
  }

  LIBSEMIGROUPS_TEST_CASE("Stephen",
                          "007",
                          "Fibonacci(4, 6)",
                          "[stephen][extreme]") {
    using namespace fpsemigroup;
    auto    rg = ReportGuard(true);
    auto    p  = make<Presentation<word_type>>(fibonacci_semigroup(4, 6));
    Stephen S(p);
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
    presentation::add_rule_and_check(p, "abcd", "ce");
    presentation::add_rule_and_check(p, "df", "dg");

    Stephen S(p);
    S.set_word(presentation::make<word_type>(p, "dfabcdf")).run();

    REQUIRE(S.word_graph().number_of_nodes() == 9);
    REQUIRE(stephen::number_of_words_accepted(S) == 8);

    auto words = std::vector<word_type>(stephen::cbegin_words_accepted(S),
                                        stephen::cend_words_accepted(S));
    std::vector<std::string> strings(words.size(), "");

    std::transform(
        words.cbegin(), words.cend(), strings.begin(), [&p](auto const& w) {
          return presentation::make<std::string>(p, w);
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
    std::sort(
        strings.begin(), strings.end(), LexicographicalCompare<std::string>());
    REQUIRE(strings.front() == "dfabcdf");

    REQUIRE(
        std::all_of(strings.cbegin(), strings.cend(), [&S, &p](auto const& w) {
          return stephen::accepts(S, presentation::make<word_type>(p, w));
        }));
    REQUIRE(stephen::number_of_words_accepted(S) == strings.size());

    S.set_word(presentation::make<word_type>(p, "abcdfceg")).run();
    REQUIRE(stephen::number_of_words_accepted(S) == 16);
    words = std::vector<word_type>(stephen::cbegin_words_accepted(S),
                                   stephen::cend_words_accepted(S));
    strings.resize(stephen::number_of_words_accepted(S));

    std::transform(
        words.cbegin(), words.cend(), strings.begin(), [&p](auto const& w) {
          return presentation::make<std::string>(p, w);
        });

    std::sort(
        strings.begin(), strings.end(), LexicographicalCompare<std::string>());
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
    REQUIRE(
        stephen::accepts(S, presentation::make<word_type>(p, "abcdfabcdf")));
  }

  LIBSEMIGROUPS_TEST_CASE(
      "Stephen",
      "009",
      "C(4) monoid normal form (test_case_gap_smalloverlap_85)",
      "[stephen][quick]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("cab");
    presentation::add_rule_and_check(p, "aabc", "acba");

    Stephen S(p);
    S.set_word(presentation::make<word_type>(p, "a")).run();
    REQUIRE(!stephen::accepts(S, presentation::make<word_type>(p, "b")));

    S.set_word(presentation::make<word_type>(p, "aabcabc")).run();
    REQUIRE(stephen::accepts(S, presentation::make<word_type>(p, "aabccba")));

    S.set_word(presentation::make<word_type>(p, "aabccba")).run();
    REQUIRE(stephen::accepts(S, presentation::make<word_type>(p, "aabcabc")));

    S.set_word(presentation::make<word_type>(p, "acba")).run();
    auto words = std::vector<word_type>(stephen::cbegin_words_accepted(S),
                                        stephen::cend_words_accepted(S));
    std::vector<std::string> strings(words.size(), "");

    std::transform(
        words.cbegin(), words.cend(), strings.begin(), [&p](auto const& w) {
          return presentation::make<std::string>(p, w);
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

    Stephen S(p);
    auto    w = presentation::make<word_type>(p, "abbbddbcbcbc");
    S.set_word(w);
    S.run();
    REQUIRE(S.finished());
    S.run();
    S.set_word(presentation::make<word_type>(p, "abbbddbcbcbc"));  // resets
    S.report_every(std::chrono::microseconds(10));

    S.run();

    Stephen T(S);
    REQUIRE(stephen::accepts(T, w));
    REQUIRE(!stephen::accepts(T, presentation::make<word_type>(p, "abbbd")));
    REQUIRE(stephen::number_of_words_accepted(T) == 1);
    REQUIRE(stephen::number_of_left_factors(T) == w.size() + 1);
    REQUIRE(stephen::accepts(S, w));
    REQUIRE(!stephen::accepts(S, presentation::make<word_type>(p, "abbbd")));
    REQUIRE(stephen::number_of_words_accepted(S) == 1);
    REQUIRE(stephen::number_of_left_factors(S) == w.size() + 1);

    Stephen U(std::move(S));
    REQUIRE(stephen::accepts(U, w));
    REQUIRE(!stephen::accepts(U, presentation::make<word_type>(p, "abbbd")));
    REQUIRE(stephen::number_of_words_accepted(U) == 1);
    REQUIRE(stephen::number_of_left_factors(U) == w.size() + 1);

    S = T;
    REQUIRE(stephen::accepts(T, w));
    REQUIRE(!stephen::accepts(T, presentation::make<word_type>(p, "abbbd")));
    REQUIRE(stephen::number_of_words_accepted(T) == 1);
    REQUIRE(stephen::number_of_left_factors(T) == w.size() + 1);
    REQUIRE(stephen::accepts(S, w));
    REQUIRE(!stephen::accepts(S, presentation::make<word_type>(p, "abbbd")));
    REQUIRE(stephen::number_of_words_accepted(S) == 1);
    REQUIRE(stephen::number_of_left_factors(S) == w.size() + 1);

    Stephen V;
    V = std::move(S);
    REQUIRE(stephen::accepts(V, w));
    REQUIRE(!stephen::accepts(V, presentation::make<word_type>(p, "abbbd")));
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

    presentation::add_rule_and_check(p, "abcd", "ce");
    presentation::add_rule_and_check(p, "df", "hd");
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

    presentation::add_rule_and_check(p, "afh", "bgh");
    presentation::add_rule_and_check(p, "hc", "d");
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

    presentation::add_rule_and_check(p, "afh", "bgh");
    presentation::add_rule_and_check(p, "hc", "de");
    presentation::add_rule_and_check(p, "ei", "j");

    verify_c4_equal_to(p, "afdj", "bgdj");
    verify_c4_not_equal_to(p, "jjjjjjjjjjj", "b");
  }

  LIBSEMIGROUPS_TEST_CASE("Stephen",
                          "014",
                          "C(4) monoid normal form (test_case_ex_3_13_14)",
                          "[stephen][quick]") {
    Presentation<std::string> p;
    p.alphabet("abcd");
    presentation::add_rule_and_check(p, "abbba", "cdc");
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
    presentation::add_rule_and_check(p, "aabc", "acba");
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
    presentation::add_rule_and_check(p, "abcd", "acca");
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
    presentation::add_rule_and_check(p, "abcd", "accca");

    verify_c4_normal_form(p, "bbcabcdaccaccabcddd", "bbcabcdaccaccabcddd");
    verify_c4_equal_to(p, "bbcabcdaccaccabcddd", "bbcabcdaccaccabcddd");
  }

  LIBSEMIGROUPS_TEST_CASE("Stephen",
                          "018",
                          "C(4) monoid normal form (test_case_mt_5)",
                          "[stephen][quick]") {
    Presentation<std::string> p;
    p.alphabet("abc");
    presentation::add_rule_and_check(p, "ac", "cbbbbc");

    verify_c4_normal_form(p, "acbbbbc", "aac");
    verify_c4_equal_to(p, "acbbbbc", "aac");
  }

  LIBSEMIGROUPS_TEST_CASE("Stephen",
                          "019",
                          "C(4) monoid normal form (test_case_mt_6)",
                          "[stephen][quick]") {
    Presentation<std::string> p;
    p.alphabet("abc");
    presentation::add_rule_and_check(p, "ccab", "cbac");

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
    presentation::add_rule_and_check(p, "afh", "bgh");
    presentation::add_rule_and_check(p, "hc", "de");
    presentation::add_rule_and_check(p, "ei", "j");

    verify_c4_normal_form(p, "bgdj", "afdei");
    verify_c4_equal_to(p, "bgdj", "afdei");
  }

  LIBSEMIGROUPS_TEST_CASE("Stephen",
                          "021",
                          "C(4) monoid normal form (test_case_mt_13)",
                          "[stephen][quick]") {
    Presentation<std::string> p;
    p.alphabet("abcd");
    presentation::add_rule_and_check(p, "abcd", "dcba");

    verify_c4_normal_form(p, "dcbdcba", "abcdbcd");
    verify_c4_equal_to(p, "dcbdcba", "abcdbcd");
  }

  LIBSEMIGROUPS_TEST_CASE("Stephen",
                          "022",
                          "C(4) monoid normal form (test_case_mt_14)",
                          "[stephen][quick]") {
    Presentation<std::string> p;
    p.alphabet("abcd");
    presentation::add_rule_and_check(p, "abca", "dcbd");

    verify_c4_normal_form(p, "dcbabca", "abcacbd");
    verify_c4_equal_to(p, "dcbabca", "abcacbd");
  }

  LIBSEMIGROUPS_TEST_CASE("Stephen",
                          "023",
                          "C(4) monoid normal form (test_case_mt_15)",
                          "[stephen][quick]") {
    Presentation<std::string> p;
    p.alphabet("abcd");
    presentation::add_rule_and_check(p, "abcd", "dcba");
    presentation::add_rule_and_check(p, "adda", "dbbd");

    verify_c4_normal_form(p, "dbbabcd", "addacba");
    verify_c4_equal_to(p, "dbbabcd", "addacba");
  }

  LIBSEMIGROUPS_TEST_CASE("Stephen",
                          "024",
                          "C(4) monoid normal form (test_case_mt_16)",
                          "[stephen][quick]") {
    Presentation<std::string> p;
    p.alphabet("abcdefg");
    presentation::add_rule_and_check(p, "abcd", "acca");
    presentation::add_rule_and_check(p, "gf", "ge");

    verify_c4_normal_form(p, "accabcdgf", "abcdbcdge");
    verify_c4_equal_to(p, "accabcdgf", "abcdbcdge");
  }

  LIBSEMIGROUPS_TEST_CASE("Stephen",
                          "025",
                          "C(4) monoid normal form (test_case_mt_17)",
                          "[stephen][quick]") {
    Presentation<std::string> p;
    p.alphabet("abcd");
    presentation::add_rule_and_check(
        p, "ababbabbbabbbb", "abbbbbabbbbbbabbbbbbbabbbbbbbb");
    presentation::add_rule_and_check(
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
    presentation::add_rule_and_check(p, "acba", "aabc");
    presentation::add_rule_and_check(p, "acba", "dbbbd");

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
    presentation::add_rule_and_check(p, "acba", "aabc");
    presentation::add_rule_and_check(p, "acba", "adbd");
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
    presentation::add_rule_and_check(p, "bceac", "aeebbc");
    presentation::add_rule_and_check(p, "aeebbc", "dabcd");
    verify_c4_normal_form(p, "bceacdabcd", "aeebbcaeebbc");
    verify_c4_normal_form(p, "aeebbcaeebbc", "aeebbcaeebbc");
  }

  LIBSEMIGROUPS_TEST_CASE("Stephen",
                          "029",
                          "C(4) monoid normal form (test_case_weak_4)",
                          "[stephen][quick]") {
    Presentation<std::string> p;
    p.alphabet("abcd");
    presentation::add_rule_and_check(p, "acba", "aabc");
    presentation::add_rule_and_check(p, "acba", "dbbd");
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
    presentation::add_rule_and_check(p, "acba", "aabc");
    presentation::add_rule_and_check(p, "acba", "adbd");
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
}  // namespace libsemigroups
