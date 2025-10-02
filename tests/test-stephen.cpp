//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2022-2025 James D. Mitchell
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

#include <algorithm>      // for for_each
#include <cctype>         // for isupper
#include <cstddef>        // for size_t
#include <cstdint>        // for uint32_t
#include <memory>         // for make_shared
#include <string>         // for basic_st...
#include <tuple>          // for tie
#include <unordered_map>  // for operator==
#include <utility>        // for make_pair
#include <vector>         // for vector

#include "Catch2-3.8.0/catch_amalgamated.hpp"  // for REQUIRE, REQUIRE_THROWS_AS, REQUI...
#include "test-main.hpp"                       // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/constants.hpp"              // for UNDEFINED
#include "libsemigroups/exception.hpp"              // for Libsemig...
#include "libsemigroups/order.hpp"                  // for Lexicogr...
#include "libsemigroups/paths.hpp"                  // for Paths
#include "libsemigroups/presentation-examples.hpp"  // for symmetric...
#include "libsemigroups/presentation.hpp"           // for InverseP...
#include "libsemigroups/ranges.hpp"                 // for operator|
#include "libsemigroups/stephen.hpp"                // for Stephen
#include "libsemigroups/todd-coxeter-class.hpp"     // for ToddCoxeter
#include "libsemigroups/todd-coxeter-helpers.hpp"   // for index_of
#include "libsemigroups/types.hpp"                  // for word_type
#include "libsemigroups/word-graph.hpp"             // for WordGraph
#include "libsemigroups/word-range.hpp"             // for ToWord

#include "libsemigroups/detail/fmt.hpp"       // for format
#include "libsemigroups/detail/iterator.hpp"  // for operator+
#include "libsemigroups/detail/report.hpp"    // for ReportGuard

namespace libsemigroups {
  using namespace literals;
  using rx::operator|;

  namespace {
    template <typename PresentationType>
    void check_000(Stephen<PresentationType>& s) {
      stephen::set_word(s, 0_w).run();
      REQUIRE(s.word_graph().number_of_nodes() == 2);
      REQUIRE(
          s.word_graph()
          == make<WordGraph<uint32_t>>(2, {{1, UNDEFINED}, {UNDEFINED, 1}}));
      REQUIRE(stephen::number_of_words_accepted(s) == POSITIVE_INFINITY);
      {
        REQUIRE((stephen::words_accepted(s) | rx::take(10) | rx::to_vector())
                == std::vector({0_w,
                                01_w,
                                011_w,
                                0111_w,
                                01111_w,
                                011111_w,
                                0111111_w,
                                01111111_w,
                                011111111_w,
                                0111111111_w}));
      }
      {
        REQUIRE((stephen::left_factors(s) | rx::take(10) | rx::to_vector())
                == std::vector({""_w,
                                0_w,
                                01_w,
                                011_w,
                                0111_w,
                                01111_w,
                                011111_w,
                                0111111_w,
                                01111111_w,
                                011111111_w}));
      }
    }

    // TODO(2): change this once we allow std::string presentations
    void verify_c4_normal_form(Presentation<word_type> const& p,
                               word_type const&               word,
                               word_type const&               nf) {
      Stephen S(p);
      stephen::set_word(S, word).run();

      REQUIRE((stephen::words_accepted(S) | rx::sort(LexicographicalCompare())
               | rx::take(1))
                  .get()
              == nf);

      REQUIRE((stephen::words_accepted(S) | rx::all_of([&S](auto const& w) {
                 return stephen::accepts(S, w);
               })));
      REQUIRE(stephen::number_of_words_accepted(S)
              == stephen::words_accepted(S).count());
    }

    // TODO(2): change this once we allow std::string presentations
    void verify_c4_equal_to(Presentation<word_type> const& p,
                            word_type const&               word1,
                            word_type const&               word2) {
      Stephen S(p);
      stephen::set_word(S, word1).run();
      REQUIRE(stephen::accepts(S, word2));
      stephen::set_word(S, word2).run();
      REQUIRE(stephen::accepts(S, word1));
    }

    // TODO(2): change this once we allow std::string presentations
    void verify_c4_not_equal_to(Presentation<word_type> const& p,
                                word_type const&               word1,
                                word_type const&               word2) {
      Stephen S(p);
      stephen::set_word(S, word1).run();
      REQUIRE(!stephen::accepts(S, word2));
      stephen::set_word(S, word2).run();
      REQUIRE(!stephen::accepts(S, word1));
    }

  }  // namespace

  LIBSEMIGROUPS_TEST_CASE("Stephen",
                          "000",
                          "basic test 1",
                          "[quick][stephen]") {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.alphabet(2);
    presentation::add_rule(p, 0_w, 01_w);
    Stephen s(p);
    check_000(s);
    s.init(p);
    check_000(s);
  }

  LIBSEMIGROUPS_TEST_CASE("Stephen",
                          "001",
                          "basic test 2",
                          "[quick][stephen]") {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.alphabet(2);
    presentation::add_rule(p, 000_w, 0_w);
    presentation::add_rule(p, 111_w, 1_w);
    presentation::add_rule(p, 0101_w, 00_w);
    Stephen s(p);
    stephen::set_word(s, 1101_w).run();
    REQUIRE(s.word_graph().number_of_nodes() == 7);
    REQUIRE(s.word_graph()
            == make<WordGraph<uint32_t>>(7,
                                         {{UNDEFINED, 1},
                                          {UNDEFINED, 2},
                                          {3, 1},
                                          {4, 5},
                                          {3, 6},
                                          {6, 3},
                                          {5, 4}}));
    REQUIRE(stephen::number_of_words_accepted(s) == POSITIVE_INFINITY);

    word_type w = 1101_w;

    REQUIRE(word_graph::last_node_on_path_no_checks(
                s.word_graph(), 0, w.begin(), w.end())
                .first
            == 5);
    w = 110010_w;
    REQUIRE(word_graph::last_node_on_path_no_checks(
                s.word_graph(), 0, w.begin(), w.end())
                .first
            == 5);

    REQUIRE(stephen::accepts(s, 110010_w));
    REQUIRE(stephen::accepts(s, 110010_w));
    REQUIRE(!stephen::accepts(s, ""_w));
    REQUIRE(!stephen::accepts(s, 0000000000_w));
    REQUIRE(!stephen::accepts(s, 111_w));
    {
      REQUIRE((stephen::words_accepted(s) | rx::take(10) | rx::to_vector())
              == std::vector({1101_w,
                              110001_w,
                              110010_w,
                              110100_w,
                              110111_w,
                              111101_w,
                              11000001_w,
                              11000010_w,
                              11000100_w,
                              11000111_w}));
    }
    {
      REQUIRE((stephen::left_factors(s) | rx::take(10) | rx::to_vector())
              == std::vector({""_w,
                              1_w,
                              11_w,
                              110_w,
                              111_w,
                              1100_w,
                              1101_w,
                              1111_w,
                              11000_w,
                              11001_w}));
      REQUIRE(stephen::number_of_left_factors(s) == POSITIVE_INFINITY);
      REQUIRE((stephen::left_factors(s) | rx::take(10)
               | rx::all_of([&s](auto const& ww) {
                   return stephen::is_left_factor(s, ww);
                 })));
    }

    stephen::set_word(s, 00_w).run();
    REQUIRE(s.word_graph().number_of_nodes() == 5);
    REQUIRE(s.word_graph()
            == make<WordGraph<uint32_t>>(
                5, {{1, UNDEFINED}, {2, 3}, {1, 4}, {4, 1}, {3, 2}}));

    p.rules.clear();
    presentation::add_rule(p, 000_w, 0_w);
    presentation::add_rule(p, 111_w, 1_w);
    stephen::set_word(s.init(p), 00_w).run();
    REQUIRE(s.word_graph().number_of_nodes() == 3);
    REQUIRE(s.word_graph()
            == make<WordGraph<uint32_t>>(
                3, {{1, UNDEFINED}, {2, UNDEFINED}, {1, UNDEFINED}}));
  }

  LIBSEMIGROUPS_TEST_CASE("Stephen",
                          "002",
                          "full transf monoid",
                          "[quick][stephen][no-valgrind]") {
    auto   rg = ReportGuard(false);
    size_t n  = 5;
    auto   p  = presentation::examples::full_transformation_monoid_II74(n);
    p.throw_if_bad_alphabet_or_rules();

    Stephen s(std::move(p));
    stephen::set_word(s, 010111020120_w).run();
    REQUIRE(s.word_graph().number_of_nodes() == 120);
    REQUIRE(
        s.word_graph()
        == make<WordGraph<uint32_t>>(
            120,
            {{1, 2, 3, 4, UNDEFINED},        {0, 5, 6, 7, UNDEFINED},
             {8, 0, 9, 10, UNDEFINED},       {11, 12, 0, 13, UNDEFINED},
             {14, 15, 16, 0, UNDEFINED},     {17, 1, 18, 19, UNDEFINED},
             {20, 21, 1, 22, UNDEFINED},     {23, 24, 25, 1, UNDEFINED},
             {2, 17, 26, 27, UNDEFINED},     {28, 29, 2, 30, UNDEFINED},
             {31, 32, 33, 2, UNDEFINED},     {3, 34, 20, 35, UNDEFINED},
             {36, 3, 29, 37, UNDEFINED},     {38, 39, 40, 3, UNDEFINED},
             {4, 41, 42, 23, UNDEFINED},     {43, 4, 44, 32, UNDEFINED},
             {45, 46, 4, 40, UNDEFINED},     {5, 8, 47, 48, UNDEFINED},
             {49, 50, 5, 51, UNDEFINED},     {52, 53, 54, 5, UNDEFINED},
             {6, 55, 11, 56, UNDEFINED},     {57, 6, 50, 58, UNDEFINED},
             {59, 60, 61, 6, UNDEFINED},     {7, 62, 63, 14, UNDEFINED},
             {64, 7, 65, 53, UNDEFINED},     {66, 67, 7, 61, UNDEFINED},
             {55, 57, 8, 68, UNDEFINED},     {62, 64, 69, 8, UNDEFINED},
             {9, 49, 55, 70, UNDEFINED},     {50, 9, 12, 71, UNDEFINED},
             {72, 73, 74, 9, UNDEFINED},     {10, 52, 75, 62, UNDEFINED},
             {53, 10, 76, 15, UNDEFINED},    {77, 78, 10, 74, UNDEFINED},
             {47, 11, 49, 79, UNDEFINED},    {63, 80, 66, 11, UNDEFINED},
             {12, 47, 57, 81, UNDEFINED},    {82, 76, 78, 12, UNDEFINED},
             {13, 83, 59, 63, UNDEFINED},    {84, 13, 73, 76, UNDEFINED},
             {61, 74, 13, 16, UNDEFINED},    {48, 14, 85, 52, UNDEFINED},
             {56, 86, 14, 59, UNDEFINED},    {15, 48, 87, 64, UNDEFINED},
             {88, 71, 15, 73, UNDEFINED},    {16, 89, 56, 66, UNDEFINED},
             {90, 16, 71, 78, UNDEFINED},    {34, 36, 17, 91, UNDEFINED},
             {41, 43, 92, 17, UNDEFINED},    {18, 28, 34, 93, UNDEFINED},
             {29, 18, 21, 94, UNDEFINED},    {95, 96, 97, 18, UNDEFINED},
             {19, 31, 98, 41, UNDEFINED},    {32, 19, 99, 24, UNDEFINED},
             {100, 101, 19, 97, UNDEFINED},  {26, 20, 28, 102, UNDEFINED},
             {42, 103, 45, 20, UNDEFINED},   {21, 26, 36, 104, UNDEFINED},
             {105, 99, 101, 21, UNDEFINED},  {22, 106, 38, 42, UNDEFINED},
             {107, 22, 96, 99, UNDEFINED},   {40, 97, 22, 25, UNDEFINED},
             {27, 23, 108, 31, UNDEFINED},   {35, 109, 23, 38, UNDEFINED},
             {24, 27, 110, 43, UNDEFINED},   {111, 94, 24, 96, UNDEFINED},
             {25, 112, 35, 45, UNDEFINED},   {113, 25, 94, 101, UNDEFINED},
             {106, 107, 114, 26, UNDEFINED}, {112, 113, 27, 114, UNDEFINED},
             {108, 111, 112, 28, UNDEFINED}, {115, 44, 46, 29, UNDEFINED},
             {30, 95, 106, 108, UNDEFINED},  {96, 30, 39, 44, UNDEFINED},
             {114, 40, 30, 33, UNDEFINED},   {102, 105, 31, 106, UNDEFINED},
             {116, 37, 32, 39, UNDEFINED},   {33, 100, 102, 112, UNDEFINED},
             {101, 33, 37, 46, UNDEFINED},   {98, 116, 100, 34, UNDEFINED},
             {110, 35, 111, 116, UNDEFINED}, {109, 110, 113, 36, UNDEFINED},
             {37, 98, 105, 109, UNDEFINED},  {91, 38, 95, 98, UNDEFINED},
             {39, 91, 107, 110, UNDEFINED},  {93, 115, 41, 95, UNDEFINED},
             {104, 42, 115, 105, UNDEFINED}, {103, 104, 43, 107, UNDEFINED},
             {44, 93, 103, 111, UNDEFINED},  {92, 45, 93, 100, UNDEFINED},
             {46, 92, 104, 113, UNDEFINED},  {83, 84, 117, 47, UNDEFINED},
             {89, 90, 48, 117, UNDEFINED},   {85, 88, 89, 49, UNDEFINED},
             {118, 65, 67, 50, UNDEFINED},   {51, 72, 83, 85, UNDEFINED},
             {73, 51, 60, 65, UNDEFINED},    {117, 61, 51, 54, UNDEFINED},
             {79, 82, 52, 83, UNDEFINED},    {119, 58, 53, 60, UNDEFINED},
             {54, 77, 79, 89, UNDEFINED},    {78, 54, 58, 67, UNDEFINED},
             {75, 119, 77, 55, UNDEFINED},   {87, 56, 88, 119, UNDEFINED},
             {86, 87, 90, 57, UNDEFINED},    {58, 75, 82, 86, UNDEFINED},
             {68, 59, 72, 75, UNDEFINED},    {60, 68, 84, 87, UNDEFINED},
             {70, 118, 62, 72, UNDEFINED},   {81, 63, 118, 82, UNDEFINED},
             {80, 81, 64, 84, UNDEFINED},    {65, 70, 80, 88, UNDEFINED},
             {69, 66, 70, 77, UNDEFINED},    {67, 69, 81, 90, UNDEFINED},
             {74, 117, 68, 69, UNDEFINED},   {71, 85, 86, 118, UNDEFINED},
             {76, 79, 119, 80, UNDEFINED},   {97, 114, 91, 92, UNDEFINED},
             {94, 108, 109, 115, UNDEFINED}, {99, 102, 116, 103, UNDEFINED}}));
  }

  // TODO(2): add a version of all test cases for std::string once this is
  // allowed by Stephen.
  LIBSEMIGROUPS_TEST_CASE("Stephen",
                          "003",
                          "from step_hen 002",
                          "[quick][stephen]") {
    ReportGuard             rg(false);
    v4::ToWord              to_word("ab");
    Presentation<word_type> p;
    p.alphabet(to_word("ab"));
    presentation::add_rule(p, to_word("aaa"), to_word("a"));
    presentation::add_rule(p, to_word("bbb"), to_word("b"));
    presentation::add_rule(p, to_word("abab"), to_word("aa"));

    Stephen S(p);
    stephen::set_word(S, to_word("bbab"));

    REQUIRE(stephen::accepts(S, to_word("bbaaba")));
    REQUIRE(!stephen::accepts(S, to_word("")));
    REQUIRE(!stephen::accepts(S, to_word("aaaaaaaaaa")));
    REQUIRE(!stephen::accepts(S, to_word("bbb")));

    stephen::set_word(S, to_word("bba"));
    REQUIRE(stephen::accepts(S, to_word("bbabb")));
    REQUIRE(stephen::accepts(S, to_word("bba")));
    REQUIRE(!stephen::accepts(S, to_word("bbb")));
    REQUIRE(!stephen::accepts(S, to_word("a")));
    REQUIRE(!stephen::accepts(S, to_word("ab")));

    stephen::set_word(S, to_word("bbaab"));
    REQUIRE(stephen::accepts(S, to_word("bbaba")));
  }

  LIBSEMIGROUPS_TEST_CASE("Stephen",
                          "004",
                          "from step_hen 003",
                          "[quick][stephen]") {
    ReportGuard             rg(false);
    v4::ToWord              to_word("abcdefg");
    Presentation<word_type> p;
    p.alphabet(to_word("abcdefg"));
    presentation::add_rule(p, to_word("aaaeaa"), to_word("abcd"));
    presentation::add_rule(p, to_word("ef"), to_word("dg"));

    Stephen S(p);

    stephen::set_word(S, to_word("abcef")).run();
    REQUIRE(to_word("abcef") == 01245_w);
    REQUIRE(S.word_graph()
            == make<WordGraph<uint32_t>>(
                11,
                {{1,
                  UNDEFINED,
                  UNDEFINED,
                  UNDEFINED,
                  UNDEFINED,
                  UNDEFINED,
                  UNDEFINED},
                 {2, 3},
                 {4},
                 {UNDEFINED, UNDEFINED, 5},
                 {UNDEFINED, UNDEFINED, UNDEFINED, UNDEFINED, 6},
                 {UNDEFINED, UNDEFINED, UNDEFINED, 7, 8},
                 {9},
                 {UNDEFINED,
                  UNDEFINED,
                  UNDEFINED,
                  UNDEFINED,
                  UNDEFINED,
                  UNDEFINED,
                  10},
                 {UNDEFINED, UNDEFINED, UNDEFINED, UNDEFINED, UNDEFINED, 10},
                 {7}}));
    auto rule = p.rules[0];
    auto m    = word_graph::last_node_on_path(
                 S.word_graph(), 0, rule.cbegin(), rule.cend())
                 .first;
    rule   = p.rules[1];
    auto n = word_graph::last_node_on_path(
                 S.word_graph(), 0, rule.cbegin(), rule.cend())
                 .first;
    REQUIRE(m != UNDEFINED);
    REQUIRE(n != UNDEFINED);
    REQUIRE(m == n);
    REQUIRE(S.word_graph().number_of_nodes() == 11);
    REQUIRE(stephen::accepts(S, to_word("aaaeaag")));
    REQUIRE(stephen::number_of_words_accepted(S) == 3);
    REQUIRE((stephen::words_accepted(S) | rx::to_vector())
            == std::vector({01236_w, 01245_w, 0004006_w}));

    stephen::set_word(S, to_word("aaaeaaaeaa")).run();

    REQUIRE(S.word_graph().number_of_nodes() == 15);
    REQUIRE(stephen::number_of_words_accepted(S) == 3);
    REQUIRE(stephen::accepts(S, to_word("aaaeabcd")));
    REQUIRE((stephen::words_accepted(S) | rx::to_vector())
            == std::vector({00040123_w, 01230400_w, 0004000400_w}));

    stephen::set_word(S, to_word("aaaeaag")).run();
    REQUIRE(S.word_graph().number_of_nodes() == 11);
    REQUIRE(stephen::number_of_words_accepted(S) == 3);
    REQUIRE((stephen::words_accepted(S) | rx::to_vector())
            == std::vector({01236_w, 01245_w, 0004006_w}));
  }

  LIBSEMIGROUPS_TEST_CASE("Stephen",
                          "005",
                          "from step_hen 004",
                          "[quick][stephen]") {
    ReportGuard             rg(false);
    v4::ToWord              to_word("abc");
    Presentation<word_type> p;
    p.alphabet(to_word("abc"));
    presentation::add_rule(p, to_word("ab"), to_word("ba"));
    presentation::add_rule(p, to_word("ac"), to_word("cc"));
    presentation::add_rule(p, to_word("ac"), to_word("a"));
    presentation::add_rule(p, to_word("cc"), to_word("a"));
    presentation::add_rule(p, to_word("bc"), to_word("cc"));
    presentation::add_rule(p, to_word("bcc"), to_word("b"));
    presentation::add_rule(p, to_word("bc"), to_word("b"));
    presentation::add_rule(p, to_word("cc"), to_word("b"));
    presentation::add_rule(p, to_word("a"), to_word("b"));

    Stephen S(p);
    stephen::set_word(S, to_word("abcc")).run();
    REQUIRE(stephen::accepts(S, to_word("baac")));
    REQUIRE(S.word_graph().number_of_nodes() == 3);
    REQUIRE(stephen::number_of_words_accepted(S) == POSITIVE_INFINITY);
    REQUIRE((stephen::words_accepted(S) | rx::take(10) | rx::to_vector())
            == std::vector({to_word("a"),
                            to_word("b"),
                            to_word("aa"),
                            to_word("ab"),
                            to_word("ac"),
                            to_word("ba"),
                            to_word("bb"),
                            to_word("bc"),
                            to_word("ca"),
                            to_word("cb")}));
  }

  LIBSEMIGROUPS_TEST_CASE("Stephen",
                          "006",
                          "from step_hen 005",
                          "[quick][stephen]") {
    ReportGuard             rg(false);
    v4::ToWord              to_word("abcd");
    Presentation<word_type> p;
    p.alphabet(to_word("abcd"));
    presentation::add_rule(p, to_word("bb"), to_word("c"));
    presentation::add_rule(p, to_word("caca"), to_word("abab"));
    presentation::add_rule(p, to_word("bc"), to_word("d"));
    presentation::add_rule(p, to_word("cb"), to_word("d"));
    presentation::add_rule(p, to_word("aa"), to_word("d"));
    presentation::add_rule(p, to_word("ad"), to_word("a"));
    presentation::add_rule(p, to_word("da"), to_word("a"));
    presentation::add_rule(p, to_word("bd"), to_word("b"));
    presentation::add_rule(p, to_word("db"), to_word("b"));
    presentation::add_rule(p, to_word("cd"), to_word("c"));
    presentation::add_rule(p, to_word("dc"), to_word("c"));

    Stephen S(p);
    stephen::set_word(S, to_word("dabdaaadabab")).run();
    REQUIRE(stephen::accepts(S, to_word("abdadcaca")));
    REQUIRE(S.word_graph().number_of_nodes() == 25);
    REQUIRE(stephen::number_of_words_accepted(S) == POSITIVE_INFINITY);
  }

  LIBSEMIGROUPS_TEST_CASE("Stephen",
                          "007",
                          "Fibonacci(4, 6)",
                          "[stephen][extreme]") {
    auto    rg = ReportGuard(true);
    Stephen S(presentation::examples::fibonacci_semigroup(4, 6));
    stephen::set_word(S, 0123_w).run_for(std::chrono::seconds(10));
    REQUIRE(!S.finished());
  }

  LIBSEMIGROUPS_TEST_CASE(
      "Stephen",
      "008",
      "C(4) monoid normal form (test_case_knuth_bendix_055)",
      "[stephen][quick]") {
    auto                    rg = ReportGuard(false);
    v4::ToWord              to_word("abcdefg");
    Presentation<word_type> p;
    p.alphabet(to_word("abcdefg"));
    presentation::add_rule(p, to_word("abcd"), to_word("ce"));
    presentation::add_rule(p, to_word("df"), to_word("dg"));

    Stephen S(p);
    stephen::set_word(S, to_word("dfabcdf")).run();

    REQUIRE(S.word_graph().number_of_nodes() == 9);
    REQUIRE(stephen::number_of_words_accepted(S) == 8);

    REQUIRE((stephen::words_accepted(S) | rx::to_vector())
            == std::vector({to_word("dfcef"),
                            to_word("dfceg"),
                            to_word("dgcef"),
                            to_word("dgceg"),
                            to_word("dfabcdf"),
                            to_word("dfabcdg"),
                            to_word("dgabcdf"),
                            to_word("dgabcdg")}));

    REQUIRE((stephen::words_accepted(S) | rx::sort(LexicographicalCompare())
             | rx::take(1))
                .get()
            == to_word("dfabcdf"));

    REQUIRE((stephen::words_accepted(S) | rx::all_of([&S](auto const& w) {
               return stephen::accepts(S, w);
             })));
    REQUIRE(stephen::number_of_words_accepted(S)
            == stephen::words_accepted(S).count());

    stephen::set_word(S, to_word("abcdfceg")).run();
    REQUIRE(stephen::number_of_words_accepted(S) == 16);

    REQUIRE((stephen::words_accepted(S) | rx::sort(LexicographicalCompare())
             | rx::to_vector())
            == std::vector({to_word("abcdfabcdf"),
                            to_word("abcdfabcdg"),
                            to_word("abcdfcef"),
                            to_word("abcdfceg"),
                            to_word("abcdgabcdf"),
                            to_word("abcdgabcdg"),
                            to_word("abcdgcef"),
                            to_word("abcdgceg"),
                            to_word("cefabcdf"),
                            to_word("cefabcdg"),
                            to_word("cefcef"),
                            to_word("cefceg"),
                            to_word("cegabcdf"),
                            to_word("cegabcdg"),
                            to_word("cegcef"),
                            to_word("cegceg")}));

    REQUIRE((stephen::words_accepted(S) | rx::sort(LexicographicalCompare())
             | rx::take(1))
                .get()
            == to_word("abcdfabcdf"));
    REQUIRE(stephen::accepts(S, to_word("abcdfabcdf")));
  }

  LIBSEMIGROUPS_TEST_CASE(
      "Stephen",
      "009",
      "C(4) monoid normal form (test_case_gap_smalloverlap_85)",
      "[stephen][quick]") {
    auto                    rg = ReportGuard(false);
    v4::ToWord              to_word("abc");
    Presentation<word_type> p;
    p.alphabet(to_word("cab"));
    presentation::add_rule(p, to_word("aabc"), to_word("acba"));

    Stephen S(p);
    stephen::set_word(S, to_word("a")).run();
    REQUIRE(!stephen::accepts(S, to_word("b")));

    stephen::set_word(S, to_word("aabcabc")).run();
    REQUIRE(stephen::accepts(S, to_word("aabccba")));

    stephen::set_word(S, to_word("aabccba")).run();
    REQUIRE(stephen::accepts(S, to_word("aabcabc")));

    stephen::set_word(S, to_word("acba")).run();

    REQUIRE((stephen::words_accepted(S) | rx::to_vector())
            == std::vector({to_word("aabc"), to_word("acba")}));
    verify_c4_normal_form(p, to_word("acba"), to_word("aabc"));
  }

  LIBSEMIGROUPS_TEST_CASE("Stephen",
                          "010",
                          "code coverage",
                          "[stephen][quick]") {
    auto                    rg = ReportGuard(false);
    v4::ToWord              to_word("abcdefg");
    Presentation<word_type> p;
    // TODO(2): Once we have make<Stephen> check that this error is thrown
    // REQUIRE_THROWS_AS(Stephen(p), LibsemigroupsException);
    p.alphabet(to_word("abcdefg"));

    Stephen S(p);
    auto    w = to_word("abbbddbcbcbc");
    stephen::set_word(S, w);
    S.run();
    REQUIRE(S.finished());
    S.run();
    stephen::set_word(S, to_word("abbbddbcbcbc"));  // resets

    S.run();

    Stephen T(S);
    REQUIRE(stephen::accepts(T, w));
    REQUIRE(!stephen::accepts(T, to_word("abbbd")));
    REQUIRE(stephen::number_of_words_accepted(T) == 1);
    REQUIRE(stephen::number_of_left_factors(T) == w.size() + 1);
    REQUIRE(stephen::accepts(S, w));
    REQUIRE(!stephen::accepts(S, to_word("abbbd")));
    REQUIRE(stephen::number_of_words_accepted(S) == 1);
    REQUIRE(stephen::number_of_left_factors(S) == w.size() + 1);

    Stephen U(std::move(S));
    REQUIRE(stephen::accepts(U, w));
    REQUIRE(!stephen::accepts(U, to_word("abbbd")));
    REQUIRE(stephen::number_of_words_accepted(U) == 1);
    REQUIRE(stephen::number_of_left_factors(U) == w.size() + 1);

    S = T;
    REQUIRE(stephen::accepts(T, w));
    REQUIRE(!stephen::accepts(T, to_word("abbbd")));
    REQUIRE(stephen::number_of_words_accepted(T) == 1);
    REQUIRE(stephen::number_of_left_factors(T) == w.size() + 1);
    REQUIRE(stephen::accepts(S, w));
    REQUIRE(!stephen::accepts(S, to_word("abbbd")));
    REQUIRE(stephen::number_of_words_accepted(S) == 1);
    REQUIRE(stephen::number_of_left_factors(S) == w.size() + 1);

    decltype(S) V;
    V = std::move(S);
    REQUIRE(stephen::accepts(V, w));
    REQUIRE(!stephen::accepts(V, to_word("abbbd")));
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
    ReportGuard             rg(false);
    v4::ToWord              to_word("abcdefgh");
    Presentation<word_type> p;
    p.alphabet(to_word("abcdefgh"));

    presentation::add_rule(p, to_word("abcd"), to_word("ce"));
    presentation::add_rule(p, to_word("df"), to_word("hd"));
    verify_c4_equal_to(p, to_word("abchd"), to_word("abcdf"));
    verify_c4_equal_to(p, to_word("abchd"), to_word("abchd"));
    verify_c4_equal_to(p, to_word("abchdf"), to_word("abchhd"));
    verify_c4_equal_to(p, to_word("abchd"), to_word("cef"));
    verify_c4_equal_to(p, to_word("cef"), to_word("abchd"));
    verify_c4_not_equal_to(p, to_word("abchf"), to_word("abcdf"));

    verify_c4_equal_to(p, to_word("hdfabce"), to_word("dffababcd"));

    verify_c4_normal_form(p, to_word("hdfabce"), to_word("dffababcd"));
  }

  LIBSEMIGROUPS_TEST_CASE(
      "Stephen",
      "012",
      "C(4) monoid normal form (test_case_gap_smalloverlap_63)",
      "[stephen][quick]") {
    ReportGuard             rg(false);
    v4::ToWord              to_word("abcdefgh");
    Presentation<word_type> p;
    p.alphabet(to_word("abcdefgh"));

    presentation::add_rule(p, to_word("afh"), to_word("bgh"));
    presentation::add_rule(p, to_word("hc"), to_word("d"));
    verify_c4_equal_to(p, to_word("afd"), to_word("bgd"));
    verify_c4_equal_to(p, to_word("bghcafhbgd"), to_word("afdafhafd"));
    verify_c4_normal_form(p, to_word("bghcafhbgd"), to_word("afdafhafd"));
  }

  LIBSEMIGROUPS_TEST_CASE(
      "Stephen",
      "013",
      "C(4) monoid equal to (test_case_gap_smalloverlap_70)",
      "[stephen][quick]") {
    ReportGuard             rg(false);
    v4::ToWord              to_word("abcdefghij");
    Presentation<word_type> p;
    p.alphabet(to_word("abcdefghij"));

    presentation::add_rule(p, to_word("afh"), to_word("bgh"));
    presentation::add_rule(p, to_word("hc"), to_word("de"));
    presentation::add_rule(p, to_word("ei"), to_word("j"));

    verify_c4_equal_to(p, to_word("afdj"), to_word("bgdj"));
    verify_c4_not_equal_to(p, to_word("jjjjjjjjjjjjjjjjjjjjjjj"), to_word("b"));
  }

  LIBSEMIGROUPS_TEST_CASE("Stephen",
                          "014",
                          "C(4) monoid normal form (test_case_ex_3_13_14)",
                          "[stephen][quick]") {
    ReportGuard             rg(false);
    v4::ToWord              to_word("abcd");
    Presentation<word_type> p;
    p.alphabet(to_word("abcd"));
    presentation::add_rule(p, to_word("abbba"), to_word("cdc"));
    verify_c4_normal_form(
        p, to_word("cdcdcabbbabbbabbcd"), to_word("abbbadcabbbabbbabbcd"));
    verify_c4_equal_to(
        p, to_word("cdcdcabbbabbbabbcd"), to_word("abbbadcabbbabbbabbcd"));

    verify_c4_equal_to(p, to_word("abbbadcbbba"), to_word("cdabbbcdc"));
    verify_c4_equal_to(p, to_word("cdabbbcdc"), to_word("cdabbbcdc"));
    verify_c4_normal_form(p, to_word("cdabbbcdc"), to_word("abbbadcbbba"));
  }

  LIBSEMIGROUPS_TEST_CASE("Stephen",
                          "015",
                          "C(4) monoid normal form (test_case_ex_3_15)",
                          "[stephen][quick]") {
    ReportGuard             rg(false);
    v4::ToWord              to_word("abcd");
    Presentation<word_type> p;
    p.alphabet(to_word("abcd"));
    presentation::add_rule(p, to_word("aabc"), to_word("acba"));
    word_type original = to_word("cbacbaabcaabcacbacba");
    word_type expected = to_word("cbaabcabcaabcaabcabc");

    verify_c4_equal_to(p, to_word("cbaabcabcaabcaabccba"), original);
    verify_c4_equal_to(p, original, expected);
    verify_c4_equal_to(p, expected, original);
    verify_c4_equal_to(p, to_word("cbaabcabcaabcaabccba"), expected);

    verify_c4_equal_to(p, original, to_word("cbaabcabcaabcaabccba"));

    verify_c4_equal_to(p, expected, to_word("cbaabcabcaabcaabccba"));
    verify_c4_normal_form(p, original, expected);
  }

  LIBSEMIGROUPS_TEST_CASE("Stephen",
                          "016",
                          "C(4) monoid normal form (test_case_ex_3_16)",
                          "[stephen][quick]") {
    ReportGuard             rg(false);
    v4::ToWord              to_word("abcd");
    Presentation<word_type> p;
    p.alphabet(to_word("abcd"));
    presentation::add_rule(p, to_word("abcd"), to_word("acca"));
    word_type original = to_word("bbcabcdaccaccabcddd");
    word_type expected = to_word("bbcabcdabcdbcdbcddd");

    verify_c4_equal_to(p, original, expected);
    verify_c4_equal_to(p, expected, original);

    verify_c4_normal_form(p, original, expected);
    verify_c4_normal_form(p, expected, expected);
  }

  LIBSEMIGROUPS_TEST_CASE("Stephen",
                          "017",
                          "C(4) monoid normal form (test_case_mt_3)",
                          "[stephen][quick]") {
    ReportGuard             rg(false);
    v4::ToWord              to_word("abcd");
    Presentation<word_type> p;
    p.alphabet(to_word("abcd"));
    presentation::add_rule(p, to_word("abcd"), to_word("accca"));

    verify_c4_normal_form(
        p, to_word("bbcabcdaccaccabcddd"), to_word("bbcabcdaccaccabcddd"));
    verify_c4_equal_to(
        p, to_word("bbcabcdaccaccabcddd"), to_word("bbcabcdaccaccabcddd"));
  }

  LIBSEMIGROUPS_TEST_CASE("Stephen",
                          "018",
                          "C(4) monoid normal form (test_case_mt_5)",
                          "[stephen][quick]") {
    ReportGuard             rg(false);
    v4::ToWord              to_word("abc");
    Presentation<word_type> p;
    p.alphabet(to_word("abc"));
    presentation::add_rule(p, to_word("ac"), to_word("cbbbbc"));

    verify_c4_normal_form(p, to_word("acbbbbc"), to_word("aac"));
    verify_c4_equal_to(p, to_word("acbbbbc"), to_word("aac"));
  }

  LIBSEMIGROUPS_TEST_CASE("Stephen",
                          "019",
                          "C(4) monoid normal form (test_case_mt_6)",
                          "[stephen][quick]") {
    ReportGuard             rg(false);
    v4::ToWord              to_word("abc");
    Presentation<word_type> p;
    p.alphabet(to_word("abc"));
    presentation::add_rule(p, to_word("ccab"), to_word("cbac"));

    verify_c4_normal_form(
        p, to_word("bacbaccabccabcbacbac"), to_word("bacbacbaccbaccbacbac"));
    verify_c4_equal_to(
        p, to_word("bacbaccabccabcbacbac"), to_word("bacbacbaccbaccbacbac"));
    verify_c4_normal_form(p, to_word("ccabcbaccab"), to_word("cbaccbacbac"));
    verify_c4_equal_to(p, to_word("ccabcbaccab"), to_word("cbaccbacbac"));
  }

  LIBSEMIGROUPS_TEST_CASE("Stephen",
                          "020",
                          "C(4) monoid normal form (test_case_mt_10)",
                          "[stephen][quick]") {
    ReportGuard             rg(false);
    v4::ToWord              to_word("abcdefghij");
    Presentation<word_type> p;
    p.alphabet(to_word("abcdefghij"));
    presentation::add_rule(p, to_word("afh"), to_word("bgh"));
    presentation::add_rule(p, to_word("hc"), to_word("de"));
    presentation::add_rule(p, to_word("ei"), to_word("j"));

    verify_c4_normal_form(p, to_word("bgdj"), to_word("afdei"));
    verify_c4_equal_to(p, to_word("bgdj"), to_word("afdei"));
  }

  LIBSEMIGROUPS_TEST_CASE("Stephen",
                          "021",
                          "C(4) monoid normal form (test_case_mt_13)",
                          "[stephen][quick]") {
    ReportGuard             rg(false);
    v4::ToWord              to_word("abcd");
    Presentation<word_type> p;
    p.alphabet(to_word("abcd"));
    presentation::add_rule(p, to_word("abcd"), to_word("dcba"));

    verify_c4_normal_form(p, to_word("dcbdcba"), to_word("abcdbcd"));
    verify_c4_equal_to(p, to_word("dcbdcba"), to_word("abcdbcd"));
  }

  LIBSEMIGROUPS_TEST_CASE("Stephen",
                          "022",
                          "C(4) monoid normal form (test_case_mt_14)",
                          "[stephen][quick]") {
    ReportGuard             rg(false);
    v4::ToWord              to_word("abcd");
    Presentation<word_type> p;
    p.alphabet(to_word("abcd"));
    presentation::add_rule(p, to_word("abca"), to_word("dcbd"));

    verify_c4_normal_form(p, to_word("dcbabca"), to_word("abcacbd"));
    verify_c4_equal_to(p, to_word("dcbabca"), to_word("abcacbd"));
  }

  LIBSEMIGROUPS_TEST_CASE("Stephen",
                          "023",
                          "C(4) monoid normal form (test_case_mt_15)",
                          "[stephen][quick]") {
    ReportGuard             rg(false);
    v4::ToWord              to_word("abcd");
    Presentation<word_type> p;
    p.alphabet(to_word("abcd"));
    presentation::add_rule(p, to_word("abcd"), to_word("dcba"));
    presentation::add_rule(p, to_word("adda"), to_word("dbbd"));

    verify_c4_normal_form(p, to_word("dbbabcd"), to_word("addacba"));
    verify_c4_equal_to(p, to_word("dbbabcd"), to_word("addacba"));
  }

  LIBSEMIGROUPS_TEST_CASE("Stephen",
                          "024",
                          "C(4) monoid normal form (test_case_mt_16)",
                          "[stephen][quick]") {
    ReportGuard             rg(false);
    v4::ToWord              to_word("abcdefg");
    Presentation<word_type> p;
    p.alphabet(to_word("abcdefg"));
    presentation::add_rule(p, to_word("abcd"), to_word("acca"));
    presentation::add_rule(p, to_word("gf"), to_word("ge"));

    verify_c4_normal_form(p, to_word("accabcdgf"), to_word("abcdbcdge"));
    verify_c4_equal_to(p, to_word("accabcdgf"), to_word("abcdbcdge"));
  }

  LIBSEMIGROUPS_TEST_CASE("Stephen",
                          "025",
                          "C(4) monoid normal form (test_case_mt_17)",
                          "[stephen][quick]") {
    ReportGuard             rg(false);
    v4::ToWord              to_word("abcd");
    Presentation<word_type> p;
    p.alphabet(to_word("abcd"));
    presentation::add_rule(p,
                           to_word("ababbabbbabbbb"),
                           to_word("abbbbbabbbbbbabbbbbbbabbbbbbbb"));
    presentation::add_rule(p,
                           to_word("cdcddcdddcdddd"),
                           to_word("cdddddcddddddcdddddddcdddddddd"));

    verify_c4_normal_form(p,
                          to_word("abbbacdddddcddddddcdddddddcdddddddd"),
                          to_word("abbbacdcddcdddcdddd"));
    verify_c4_equal_to(p,
                       to_word("abbbacdddddcddddddcdddddddcdddddddd"),
                       to_word("abbbacdcddcdddcdddd"));
  }

  LIBSEMIGROUPS_TEST_CASE("Stephen",
                          "026",
                          "C(4) monoid normal form (test_case_weak_1)",
                          "[stephen][quick]") {
    ReportGuard             rg(false);
    v4::ToWord              to_word("abcd");
    Presentation<word_type> p;
    p.alphabet(to_word("abcd"));
    presentation::add_rule(p, to_word("acba"), to_word("aabc"));
    presentation::add_rule(p, to_word("acba"), to_word("dbbbd"));

    verify_c4_equal_to(p, to_word("aaabc"), to_word("adbbbd"));
    verify_c4_equal_to(p, to_word("adbbbd"), to_word("aaabc"));

    verify_c4_equal_to(p, to_word("aaabcadbbbd"), to_word("adbbbdadbbbd"));
    verify_c4_equal_to(p, to_word("aaabcaaabc"), to_word("adbbbdadbbbd"));
    verify_c4_equal_to(p, to_word("acba"), to_word("dbbbd"));
    verify_c4_equal_to(p, to_word("acbabbbd"), to_word("aabcbbbd"));
    verify_c4_equal_to(p, to_word("aabcbbbd"), to_word("acbabbbd"));
  }

  LIBSEMIGROUPS_TEST_CASE("Stephen",
                          "027",
                          "C(4) monoid normal form (test_case_weak_2)",
                          "[stephen][quick]") {
    ReportGuard             rg(false);
    v4::ToWord              to_word("abcd");
    Presentation<word_type> p;
    p.alphabet(to_word("abcd"));
    presentation::add_rule(p, to_word("acba"), to_word("aabc"));
    presentation::add_rule(p, to_word("acba"), to_word("adbd"));
    verify_c4_equal_to(p, to_word("acbacba"), to_word("aabcabc"));
    verify_c4_normal_form(p, to_word("acbacba"), to_word("aabcabc"));
    verify_c4_equal_to(p, to_word("aabcabc"), to_word("acbacba"));
    verify_c4_normal_form(p, to_word("aabcabc"), to_word("aabcabc"));
  }

  LIBSEMIGROUPS_TEST_CASE("Stephen",
                          "028",
                          "C(4) monoid normal form (test_case_weak_3)",
                          "[stephen][quick]") {
    ReportGuard             rg(false);
    v4::ToWord              to_word("abcde");
    Presentation<word_type> p;
    p.alphabet(to_word("abcde"));
    presentation::add_rule(p, to_word("bceac"), to_word("aeebbc"));
    presentation::add_rule(p, to_word("aeebbc"), to_word("dabcd"));
    verify_c4_normal_form(p, to_word("bceacdabcd"), to_word("aeebbcaeebbc"));
    verify_c4_normal_form(p, to_word("aeebbcaeebbc"), to_word("aeebbcaeebbc"));
  }

  LIBSEMIGROUPS_TEST_CASE("Stephen",
                          "029",
                          "C(4) monoid normal form (test_case_weak_4)",
                          "[stephen][quick]") {
    ReportGuard             rg(false);
    v4::ToWord              to_word("abcd");
    Presentation<word_type> p;
    p.alphabet(to_word("abcd"));
    presentation::add_rule(p, to_word("acba"), to_word("aabc"));
    presentation::add_rule(p, to_word("acba"), to_word("dbbd"));
    verify_c4_normal_form(
        p, to_word("bbacbcaaabcbbd"), to_word("bbacbcaaabcbbd"));
    verify_c4_normal_form(p, to_word("acbacba"), to_word("aabcabc"));
    verify_c4_normal_form(p, to_word("aabcabc"), to_word("aabcabc"));
  }

  LIBSEMIGROUPS_TEST_CASE("Stephen",
                          "030",
                          "C(4) monoid normal form (test_case_weak_5)",
                          "[stephen][quick]") {
    ReportGuard             rg(false);
    v4::ToWord              to_word("abcd");
    Presentation<word_type> p;
    p.alphabet(to_word("abcd"));
    presentation::add_rule(p, to_word("acba"), to_word("aabc"));
    presentation::add_rule(p, to_word("acba"), to_word("adbd"));
    verify_c4_normal_form(p, to_word("acbacba"), to_word("aabcabc"));
    verify_c4_normal_form(p, to_word("aabcabc"), to_word("aabcabc"));
  }

  LIBSEMIGROUPS_TEST_CASE("Stephen",
                          "031",
                          "Test behaviour when uninitialised",
                          "[stephen][quick]") {
    ReportGuard                      rg(false);
    Stephen<Presentation<word_type>> S;

    REQUIRE_THROWS_AS(S.accept_state(), LibsemigroupsException);
    REQUIRE_THROWS_AS(S.word(), LibsemigroupsException);
    REQUIRE_THROWS_AS(S.word_graph(), LibsemigroupsException);

    REQUIRE_THROWS_AS(stephen::is_left_factor(S, {0, 0, 0}),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(stephen::accepts(S, {0, 0, 0}), LibsemigroupsException);
    REQUIRE_THROWS_AS(stephen::number_of_left_factors(S),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(stephen::number_of_words_accepted(S),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(S.run(), LibsemigroupsException);

    REQUIRE_THROWS_AS(stephen::words_accepted(S), LibsemigroupsException);
    REQUIRE_THROWS_AS(stephen::left_factors(S), LibsemigroupsException);

    Presentation<word_type> p;
    p.alphabet(01_w);
    presentation::add_rule(p, 01_w, 10_w);
    S.init(p);

    REQUIRE_THROWS_AS(S.accept_state(), LibsemigroupsException);
    REQUIRE_THROWS_AS(S.word(), LibsemigroupsException);
    REQUIRE_THROWS_AS(S.word_graph(), LibsemigroupsException);

    REQUIRE_THROWS_AS(stephen::is_left_factor(S, {0, 0, 0}),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(stephen::accepts(S, {0, 0, 0}), LibsemigroupsException);
    REQUIRE_THROWS_AS(stephen::number_of_left_factors(S),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(stephen::number_of_words_accepted(S),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(S.run(), LibsemigroupsException);

    REQUIRE_THROWS_AS(stephen::words_accepted(S), LibsemigroupsException);
    REQUIRE_THROWS_AS(stephen::left_factors(S), LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("Stephen",
                          "032",
                          "(inverse) "
                          "step_hen test_schutzenbergergraph 001 "
                          "(string)",
                          "[stephen][quick]") {
    ReportGuard rg(false);
    v4::ToWord  to_word("abcABC");

    InversePresentation<word_type> p;
    p.alphabet(to_word("abcABC"));
    p.inverses_no_checks(to_word("ABCabc"));

    auto S = Stephen(std::move(p));

    stephen::set_word(S, to_word("aBcAbC")).run();

    REQUIRE(S.finished());
    REQUIRE(S.word_graph().number_of_nodes() == 7);
    REQUIRE(!stephen::accepts(S, to_word("BaAbaBcAbC")));
    REQUIRE(stephen::accepts(S, to_word("aBcCbBcAbC")));

    stephen::set_word(S, to_word("aBcCbBcAbC"));
    REQUIRE(stephen::accepts(S, to_word("aBcAbC")));

    stephen::set_word(S, to_word("BaAbaBcAbC"));
    REQUIRE(stephen::accepts(S, to_word("aBcAbC")));
  }

  LIBSEMIGROUPS_TEST_CASE("Stephen",
                          "033",
                          "(inverse) "
                          "step_hen test_schutzenbergergraph 001",
                          "[stephen][quick]") {
    ReportGuard                    rg(false);
    v4::ToWord                     to_word("abcABC");
    InversePresentation<word_type> p;
    p.alphabet(to_word("abcABC"));
    p.inverses_no_checks(to_word("ABCabc"));

    Stephen S(p);
    REQUIRE(to_word("aBcAbC") == 042315_w);

    stephen::set_word(S, to_word("aBcAbC")).run();

    REQUIRE(S.finished());
    REQUIRE(S.word_graph().number_of_nodes() == 7);
    REQUIRE(!stephen::accepts(S, to_word("BaAbaBcAbC")));
    REQUIRE(stephen::accepts(S, to_word("aBcCbBcAbC")));

    stephen::set_word(S, to_word("aBcCbBcAbC"));
    REQUIRE(stephen::accepts(S, to_word("aBcAbC")));

    stephen::set_word(S, to_word("BaAbaBcAbC"));
    REQUIRE(stephen::accepts(S, to_word("aBcAbC")));
  }

  LIBSEMIGROUPS_TEST_CASE("Stephen",
                          "034",
                          "(inverse) "
                          "step_hen test_schutzenbergergraph 002",
                          "[stephen][quick]") {
    ReportGuard                    rg(false);
    v4::ToWord                     to_word("abcABC");
    InversePresentation<word_type> p;
    p.alphabet(to_word("abcABC"));
    p.inverses_no_checks(to_word("ABCabc"));

    auto S = Stephen(p);
    stephen::set_word(S, to_word("aBbcABAabCc")).run();

    REQUIRE(S.accept_state() == 4);
    REQUIRE(word_graph::follow_path(S.word_graph(), 0, S.word()) == 4);
    REQUIRE(stephen::number_of_words_accepted(S) == POSITIVE_INFINITY);
  }

  LIBSEMIGROUPS_TEST_CASE("Stephen",
                          "035",
                          "(inverse) "
                          "step_hen test_schutzenbergergraph 003",
                          "[stephen][quick]") {
    ReportGuard                    rg(false);
    v4::ToWord                     to_word("xyXY");
    InversePresentation<word_type> p;
    p.alphabet(to_word("xyXY"));
    p.inverses_no_checks(to_word("XYxy"));

    auto S = Stephen(p);
    stephen::set_word(S, to_word("xxxyyy")).run();

    REQUIRE(stephen::accepts(S, to_word("xxxyyyYYYXXXxxxyyy")));
    stephen::set_word(S, to_word("xxxyyyYYYXXXxxxyyy"));
    REQUIRE(stephen::accepts(S, to_word("xxxyyy")));
    REQUIRE(!stephen::accepts(S, to_word("xxx")));
  }

  LIBSEMIGROUPS_TEST_CASE("Stephen",
                          "036",
                          "(inverse) "
                          "step_hen test_schutzenbergergraph 004",
                          "[stephen][quick]") {
    ReportGuard                    rg(false);
    v4::ToWord                     to_word("xyXY");
    InversePresentation<word_type> p;
    p.alphabet(to_word("xyXY"));
    p.inverses_no_checks(to_word("XYxy"));
    presentation::add_rule(p, to_word("xyXxyX"), to_word("xyX"));

    auto S = Stephen(p);
    stephen::set_word(S, to_word("xyXyy"));
    std::string ys = "";
    for (size_t i = 0; i < 10; ++i) {
      REQUIRE(stephen::accepts(S, to_word(std::string("x") + ys + "Xyy")));
      ys += "y";
    }

    REQUIRE(!stephen::accepts(S, to_word("xXyx")));
    REQUIRE(!stephen::accepts(S, to_word("xXxx")));
    REQUIRE(!stephen::accepts(S, to_word("xXxy")));
    REQUIRE(!stephen::accepts(S, to_word("xXxX")));
    REQUIRE(!stephen::accepts(S, to_word("xXyY")));
    REQUIRE(stephen::accepts(S, to_word("xyXyy")));
    REQUIRE(stephen::number_of_words_accepted(S) == POSITIVE_INFINITY);
    REQUIRE(S.word_graph().number_of_nodes() == 4);
    REQUIRE(S.word_graph().number_of_edges() == 8);

    REQUIRE(
        S.word_graph()
        == make<WordGraph<uint32_t>>(4,
                                     {{1, 2, UNDEFINED, UNDEFINED},
                                      {UNDEFINED, 1, 0, 1},
                                      {UNDEFINED, 3, UNDEFINED, 0},
                                      {UNDEFINED, UNDEFINED, UNDEFINED, 2}}));
  }

  LIBSEMIGROUPS_TEST_CASE("Stephen",
                          "037",
                          "(inverse) "
                          "step_hen test_schutzenbergergraph 005",
                          "[stephen][quick]") {
    ReportGuard                    rg(false);
    v4::ToWord                     to_word("xyXY");
    InversePresentation<word_type> p;
    p.alphabet(to_word("xyXY"));
    p.inverses_no_checks(to_word("XYxy"));
    presentation::add_rule(p, to_word("xyXxyX"), to_word("xyX"));
    presentation::add_rule(p, to_word("xyxy"), to_word("xy"));

    auto S = Stephen(p);
    stephen::set_word(S, to_word("xyXyy"));
    REQUIRE(stephen::accepts(S, to_word("y")));
    REQUIRE(stephen::accepts(S, to_word("xxxxxxxxxxxxx")));
    REQUIRE(stephen::accepts(S, to_word("xyXxyxyxyxyxyXyy")));
    REQUIRE(S.word_graph().number_of_nodes() == 1);
  }

  LIBSEMIGROUPS_TEST_CASE("Stephen",
                          "038",
                          "(inverse) "
                          "step_hen test_schutzenbergergraph 006",
                          "[stephen][quick]") {
    ReportGuard                    rg(false);
    v4::ToWord                     to_word("abcABC");
    InversePresentation<word_type> p;
    p.alphabet(to_word("abcABC"));
    p.inverses_no_checks(to_word("ABCabc"));
    presentation::add_rule(p, to_word("ac"), to_word("ca"));
    presentation::add_rule(p, to_word("ab"), to_word("ba"));
    presentation::add_rule(p, to_word("bc"), to_word("cb"));

    auto S = Stephen(p);
    stephen::set_word(S, to_word("BaAbaBcAbC"));
    S.run();
    REQUIRE(S.word_graph().number_of_nodes() == 7);
    REQUIRE(S.word_graph()
            == make<WordGraph<uint32_t>>(
                7,
                {{1, UNDEFINED, 2, UNDEFINED, 3, UNDEFINED},
                 {UNDEFINED, UNDEFINED, UNDEFINED, 0, 4, UNDEFINED},
                 {UNDEFINED, UNDEFINED, UNDEFINED, UNDEFINED, 5, 0},
                 {4, 0, 5, UNDEFINED, UNDEFINED, UNDEFINED},
                 {UNDEFINED, 1, 6, 3, UNDEFINED, UNDEFINED},
                 {6, 2, UNDEFINED, UNDEFINED, UNDEFINED, 3},
                 {UNDEFINED, UNDEFINED, UNDEFINED, 5, UNDEFINED, 4}}));
  }

  LIBSEMIGROUPS_TEST_CASE("Stephen", "039", "corner case", "[stephen][quick]") {
    ReportGuard rg(false);
    v4::ToWord  to_word("x");

    Presentation<word_type> p;
    p.contains_empty_word(true);

    p.alphabet(to_word("x"));
    presentation::add_rule(p, to_word("xxxx"), to_word("xx"));

    Stephen S(p);
    stephen::set_word(S, ""_w);
    S.run();
    REQUIRE(S.accept_state() == 0);
    REQUIRE(S.word_graph().number_of_nodes() == 1);
    REQUIRE(!stephen::accepts(S, to_word("x")));
  }

  LIBSEMIGROUPS_TEST_CASE("Stephen", "040", "empty word", "[stephen][quick]") {
    ReportGuard rg(false);
    auto        p = presentation::examples::symmetric_inverse_monoid(4);
    REQUIRE(p.contains_empty_word());
    REQUIRE(p.alphabet().size() == 4);

    auto s = Stephen(p);
    stephen::set_word(s, ""_w).run();
    REQUIRE(s.word_graph().number_of_nodes() == 24);
    stephen::set_word(s, 0_w).run();
    REQUIRE(s.word_graph().number_of_nodes() == 24);
    stephen::set_word(s, 1_w).run();
    REQUIRE(s.word_graph().number_of_nodes() == 24);
    stephen::set_word(s, 2_w).run();
    REQUIRE(s.word_graph().number_of_nodes() == 24);
    stephen::set_word(s, 3_w).run();
    REQUIRE(s.word_graph().number_of_nodes() == 48);
  }

  LIBSEMIGROUPS_TEST_CASE("Stephen", "041", "shared_ptr", "[stephen][quick]") {
    ReportGuard                    rg(false);
    v4::ToWord                     to_word("abcABC");
    InversePresentation<word_type> p;
    p.alphabet(to_word("abcABC"));
    p.inverses_no_checks(to_word("ABCabc"));
    presentation::add_rule(p, to_word("ac"), to_word("ca"));
    presentation::add_rule(p, to_word("ab"), to_word("ba"));
    presentation::add_rule(p, to_word("bc"), to_word("cb"));

    auto ptr = std::make_shared<decltype(p)>(p);
    auto S   = Stephen<InversePresentation<word_type>>(ptr);
    stephen::set_word(S, to_word("BaAbaBcAbC"));
    S.run();
    REQUIRE(S.word_graph().number_of_nodes() == 7);
    REQUIRE(S.word_graph()
            == make<WordGraph<uint32_t>>(
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
                          "[stephen][quick][no-valgrind]") {
    ReportGuard            rg(false);
    ToddCoxeter<word_type> tc;
    using presentation::examples::symmetric_inverse_monoid;
    {
      auto p = symmetric_inverse_monoid(4);
      REQUIRE(p.contains_empty_word());
      REQUIRE(p.alphabet().size() == 4);
      p.throw_if_bad_alphabet_or_rules();
      tc.init(congruence_kind::twosided, p);
    }

    {
      InversePresentation<word_type> p(symmetric_inverse_monoid(4));
      p.inverses_no_checks(0123_w);
      p.throw_if_bad_alphabet_rules_or_inverses();

      Stephen S(p);
      auto    w = 0120112010222201_w;
      stephen::set_word(S, w);

      REQUIRE(stephen::number_of_words_accepted(S) == POSITIVE_INFINITY);

      {
        auto const index = todd_coxeter::index_of(tc, w);
        auto       T     = Stephen(p);

        for (auto const& ww : (stephen::words_accepted(S) | rx::take(1024))) {
          REQUIRE(todd_coxeter::index_of(tc, ww) == index);
          REQUIRE(stephen::accepts(stephen::set_word(T, ww), ww));
        }
      }
    }
  }

  namespace {
    std::string invert(std::string const& g) {
      auto invert = [](char c) {
        if (std::isupper(c)) {
          return std::tolower(c);
        } else {
          return std::toupper(c);
        }
      };
      auto G = g;
      std::reverse(G.begin(), G.end());
      std::transform(G.begin(), G.end(), G.begin(), invert);
      return G;
    }
  }  // namespace

  LIBSEMIGROUPS_TEST_CASE("Stephen",
                          "043",
                          "inverse presentation",
                          "[stephen][extreme]") {
    using words::                  operator+;
    ReportGuard                    rg(false);
    v4::ToWord                     to_word("abcABC");
    InversePresentation<word_type> p;
    p.alphabet(to_word("abcABC"));
    p.inverses_no_checks(to_word("ABCabc"));
    presentation::add_rule(p, to_word("ac"), to_word("ca"));
    presentation::add_rule(p, to_word("ab"), to_word("ba"));
    presentation::add_rule(p, to_word("bc"), to_word("cb"));
    auto S = Stephen(p);
    auto T = Stephen(p);

    StringRange strings;
    strings.alphabet("abcABC").first("aaa").last("aaaaa");

    for (auto const& w : strings) {
      stephen::set_word(S, to_word(w));
      for (auto x : stephen::words_accepted(S) | rx::take(1024)) {
        REQUIRE(stephen::accepts(S, x));
        // Need to use ww^{-1}x because we are working in an inverse
        // presentation
        stephen::set_word(T, to_word(w) + to_word(invert(w)) + x);
        REQUIRE(stephen::accepts(T, S.word()));
        REQUIRE(S == T);
      }
    }
  }

  LIBSEMIGROUPS_TEST_CASE("Stephen",
                          "044",
                          "non-inverse presentation -- operator==",
                          "[stephen][quick][no-valgrind]") {
    ReportGuard rg(false);
    auto        p = presentation::examples::symmetric_inverse_monoid(4);

    ToddCoxeter tc(congruence_kind::twosided, p);

    auto      S = Stephen(p);
    word_type w = 0120112010222201_w;
    stephen::set_word(S, w);
    REQUIRE(stephen::number_of_words_accepted(S) == POSITIVE_INFINITY);

    {
      auto const index = todd_coxeter::index_of(tc, w);

      for (auto const& u : (stephen::words_accepted(S) | rx::take(1024))) {
        REQUIRE(todd_coxeter::index_of(tc, u) == index);
      }
    }
    {
      for (auto const& u : todd_coxeter::class_of(tc, w) | rx::take(1024)) {
        REQUIRE(stephen::accepts(S, u));
      }
    }
  }

  LIBSEMIGROUPS_TEST_CASE("Stephen",
                          "045",
                          "Plactic monoid",
                          "[stephen][quick]") {
    auto rg = ReportGuard(false);
    auto p  = presentation::examples::plactic_monoid(4);
    p.contains_empty_word(true);
    Stephen s(p);
    stephen::set_word(s, 0013122_w).run();
    REQUIRE(!stephen::accepts(s, 0013212_w));
  }

  LIBSEMIGROUPS_TEST_CASE("Stephen",
                          "046",
                          "Whyte's 4-relation full transf monoid 8",
                          "[stephen][extreme]") {
    auto                    rg = ReportGuard(true);
    Presentation<word_type> p;
    p.rules = {00_w,       ""_w,       11_w,         ""_w,         22_w,
               ""_w,       33_w,       ""_w,         44_w,         ""_w,
               55_w,       ""_w,       66_w,         ""_w,         010101_w,
               ""_w,       121212_w,   ""_w,         232323_w,     ""_w,
               343434_w,   ""_w,       454545_w,     ""_w,         565656_w,
               ""_w,       606060_w,   ""_w,         01020102_w,   ""_w,
               01030103_w, ""_w,       01040104_w,   ""_w,         01050105_w,
               ""_w,       01060106_w, ""_w,         12101210_w,   ""_w,
               12131213_w, ""_w,       12141214_w,   ""_w,         12151215_w,
               ""_w,       12161216_w, ""_w,         23202320_w,   ""_w,
               23212321_w, ""_w,       23242324_w,   ""_w,         23252325_w,
               ""_w,       23262326_w, ""_w,         34303430_w,   ""_w,
               34313431_w, ""_w,       34323432_w,   ""_w,         34353435_w,
               ""_w,       34363436_w, ""_w,         45404540_w,   ""_w,
               45414541_w, ""_w,       45424542_w,   ""_w,         45434543_w,
               ""_w,       45464546_w, ""_w,         56505650_w,   ""_w,
               56515651_w, ""_w,       56525652_w,   ""_w,         56535653_w,
               ""_w,       56545654_w, ""_w,         60616061_w,   ""_w,
               60626062_w, ""_w,       60636063_w,   ""_w,         60646064_w,
               ""_w,       60656065_w, ""_w,         071654321_w,  16543217_w,
               217121_w,   17171_w,    0102720107_w, 7010270102_w, 107017_w,
               70107010_w, 1217_w,     7121_w};
    p.alphabet_from_rules();
    presentation::balance_no_checks(p, 0123456_w, 0123456_w);

    Stephen s(p);
    stephen::set_word(s, 1217_w);
    s.run();

    REQUIRE(word_graph::last_node_on_path(s.word_graph(), 0, 1217_w).first
            == word_graph::last_node_on_path(s.word_graph(), 0, 7121_w).first);
    REQUIRE(stephen::accepts(s, 7121_w));
  }

  LIBSEMIGROUPS_TEST_CASE("Stephen",
                          "047",
                          "Munn tree products",
                          "[stephen][quick]") {
    ReportGuard rg(false);
    using words::pow;
    v4::ToWord to_word("abcABC");

    InversePresentation<word_type> p;
    p.alphabet(to_word("abcABC"));
    p.inverses_no_checks(to_word("ABCabc"));

    auto S = Stephen(p);
    auto T = Stephen(p);

    stephen::set_word(S, to_word("aBbcaABAabCc"));
    stephen::set_word(T, to_word("aBbcaABAabCc"));
    S.run();
    REQUIRE(S.word_graph().number_of_nodes() == 7);

    T.run();
    REQUIRE(T.word_graph().number_of_nodes() == 7);
    S *= T;
    REQUIRE(S.word_graph().number_of_nodes() == 14);
    // fmt::print("{}", word_graph::dot(S.word_graph()));
    REQUIRE(!S.finished());
    S.run();
    REQUIRE(S.finished());
    REQUIRE(S.word_graph().number_of_nodes() == 12);
    REQUIRE(stephen::accepts(S, pow(T.word(), 2)));
    stephen::set_word(T, pow(T.word(), 2));
    T.run();
    REQUIRE(S == T);
    REQUIRE(stephen::words_accepted(S).get() == 0202_w);
    REQUIRE(stephen::number_of_left_factors(S) == POSITIVE_INFINITY);
    stephen::set_word(S, to_word("aBbcaABAabCc"));
    // std::ofstream f("wordgraph.gv");
    // f << stephen::dot(S).to_string();
    stephen::set_word(T, to_word("CcBAabaACBbA"));
    S *= T;

    InversePresentation<word_type> q;
    q.alphabet(to_word("abcABC"));
    q.inverses_no_checks(to_word("ABCabc"));
    presentation::add_rule(q, to_word("ab"), to_word("ba"));
    T.init(q);
    stephen::set_word(T, to_word("CcBAabaACBbA"));
    T.run();
    // Different presentation should throw
    REQUIRE_THROWS_AS(S *= T, LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("Stephen",
                          "048",
                          "bicyclic monoid",
                          "[stephen][fail]") {
    ReportGuard                    rg(true);
    v4::ToWord                     to_word("aA");
    InversePresentation<word_type> p;
    p.alphabet(to_word("aA"));
    p.inverses_no_checks(to_word("Aa"));
    p.contains_empty_word(true);
    presentation::add_rule(p, to_word("aA"), to_word(""));

    Stephen S(p);
    stephen::set_word(S, ""_w).run();
    // NOTE: Must fail since a^nA^n = 1 for all n, but a^n are each in a
    // distinct R-class, so the Stephen graph will have infinitely many
    // vertices.
    REQUIRE(S.word_graph().number_of_nodes() == POSITIVE_INFINITY);
  }

  LIBSEMIGROUPS_TEST_CASE("Stephen",
                          "049",
                          "chinese monoid",
                          "[stephen][quick]") {
    ReportGuard rg(false);
    auto        p = presentation::examples::chinese_monoid(3);

    Stephen S(p);
    stephen::set_word(S, 0110_w).run();
    REQUIRE(S.word_graph().number_of_nodes() == 8);
    REQUIRE((stephen::words_accepted(S) | rx::to_vector())
            == std::vector({0101_w, 0110_w, 1001_w}));
  }

  LIBSEMIGROUPS_TEST_CASE("Stephen",
                          "050",
                          "to_human_readable_repr",
                          "[stephen][quick]") {
    ReportGuard             rg(false);
    Presentation<word_type> p;
    p.alphabet(01_w);
    p.contains_empty_word(true);
    presentation::add_rule(p, 000_w, 11_w);
    presentation::add_rule(p, 001_w, 10_w);

    Stephen<Presentation<word_type>> stephen;
    REQUIRE(to_human_readable_repr(stephen)
            == fmt::format("<Stephen object over {} with no word set>",
                           to_human_readable_repr(stephen.presentation())));
    stephen.init(p);
    REQUIRE(to_human_readable_repr(stephen)
            == fmt::format("<Stephen object over {} with no word set>",
                           to_human_readable_repr(stephen.presentation())));
    stephen::set_word(stephen, 0110_w);
    REQUIRE(to_human_readable_repr(stephen)
            == fmt::format("<Stephen object over {} for word [0, 1, 1, 0] "
                           "with 5 nodes and 4 edges>",
                           to_human_readable_repr(stephen.presentation())));
    stephen.run();
    REQUIRE(to_human_readable_repr(stephen)
            == fmt::format("<Stephen object over {} for word [0, 1, 1, 0] "
                           "with 13 nodes and 26 edges>",
                           to_human_readable_repr(stephen.presentation())));
    stephen::set_word(stephen, 011001100_w);
    REQUIRE(to_human_readable_repr(stephen)
            == fmt::format(
                "<Stephen object over {} for word [0, 1, 1, 0, 0, 1, 1, 0, 0] "
                "with 10 nodes and 9 edges>",
                to_human_readable_repr(stephen.presentation())));
    stephen.run();
    REQUIRE(to_human_readable_repr(stephen)
            == fmt::format(
                "<Stephen object over {} for word [0, 1, 1, 0, 0, 1, 1, 0, 0] "
                "with 13 nodes and 26 edges>",
                to_human_readable_repr(stephen.presentation())));
    stephen::set_word(stephen, 0110011001_w);
    stephen.run();
    REQUIRE(to_human_readable_repr(stephen)
            == fmt::format("<Stephen object over {} for 10 letter word "
                           "with 13 nodes and 26 edges>",
                           to_human_readable_repr(stephen.presentation())));
    stephen::set_word(stephen, 01100110010_w);
    stephen.run();
    REQUIRE(to_human_readable_repr(stephen)
            == fmt::format("<Stephen object over {} for 11 letter word "
                           "with 13 nodes and 26 edges>",
                           to_human_readable_repr(stephen.presentation())));
    stephen.init(p);
    REQUIRE(to_human_readable_repr(stephen)
            == fmt::format("<Stephen object over {} with no word set>",
                           to_human_readable_repr(stephen.presentation())));

    v4::ToWord                     to_word("abcABC");
    InversePresentation<word_type> pi;
    pi.alphabet(to_word("abcABC"));
    pi.inverses_no_checks(to_word("ABCabc"));
    presentation::add_rule(pi, to_word("ac"), to_word("ca"));
    presentation::add_rule(pi, to_word("ab"), to_word("ba"));
    presentation::add_rule(pi, to_word("bc"), to_word("cb"));

    Stephen<InversePresentation<word_type>> inverse_stephen;
    REQUIRE(
        to_human_readable_repr(inverse_stephen)
        == fmt::format("<Stephen object over {} with no word set>",
                       to_human_readable_repr(inverse_stephen.presentation())));
    inverse_stephen.init(pi);
    REQUIRE(
        to_human_readable_repr(inverse_stephen)
        == fmt::format("<Stephen object over {} with no word set>",
                       to_human_readable_repr(inverse_stephen.presentation())));
    stephen::set_word(inverse_stephen, to_word("BaAbaBcAb"));
    REQUIRE(to_human_readable_repr(inverse_stephen)
            == fmt::format(
                "<Stephen object over {} for word [4, 0, 3, 1, 0, 4, 2, 3, 1] "
                "with 8 nodes and 14 edges>",
                to_human_readable_repr(inverse_stephen.presentation())));
    inverse_stephen.run();
    REQUIRE(to_human_readable_repr(inverse_stephen)
            == fmt::format(
                "<Stephen object over {} for word [4, 0, 3, 1, 0, 4, 2, 3, 1] "
                "with 7 nodes and 18 edges>",
                to_human_readable_repr(inverse_stephen.presentation())));
  }

  // The following uses up about 7GB memory to run
  LIBSEMIGROUPS_TEST_CASE("Stephen",
                          "051",
                          "shared_ptr memory check",
                          "[stephen][standard]") {
    ReportGuard                    rg(false);
    v4::ToWord                     to_word("abcABC");
    InversePresentation<word_type> p;
    p.alphabet(to_word("abcABC"));
    p.inverses_no_checks(to_word("ABCabc"));
    presentation::add_rule(p, to_word("a"), to_word("b"));
    for (size_t i = 0; i < 2 << 14; ++i) {
      presentation::add_rule(p, random_word(2 << 9, 6), random_word(2 << 9, 6));
    }  // About 500 Mb in size
    auto ptr = std::make_shared<InversePresentation<word_type>>(
        p);  // Adds another 500 Mb due to copy

    size_t                                               n = 12;
    std::vector<Stephen<InversePresentation<word_type>>> stephens(n);
    // Uses no extra memory with shared pointer
    for (size_t i = 0; i < n; ++i) {
      stephens[i].init(ptr);
      if (i % 2 == 0) {
        stephen::set_word(stephens[i], to_word("a"));
      } else {
        stephen::set_word(stephens[i], to_word("b"));
      }
      stephens[i].run();
      REQUIRE(stephens[i].word_graph().number_of_nodes() == 2);
      REQUIRE(stephens[i].word_graph()
              == make<WordGraph<uint32_t>>(
                  2,
                  {{1, 1, UNDEFINED, UNDEFINED, UNDEFINED, UNDEFINED},
                   {UNDEFINED, UNDEFINED, UNDEFINED, 0, 0, UNDEFINED}}));
    }

    std::vector<Stephen<InversePresentation<word_type>>> bad_stephens(n);
    // Uses 6GB extra memory without shared pointer
    for (size_t i = 0; i < n; ++i) {
      bad_stephens[i].init(p);
      if (i % 2 == 0) {
        stephen::set_word(bad_stephens[i], to_word("a"));
      } else {
        stephen::set_word(bad_stephens[i], to_word("b"));
      }
      bad_stephens[i].run();
      REQUIRE(bad_stephens[i].word_graph().number_of_nodes() == 2);
      REQUIRE(bad_stephens[i].word_graph()
              == make<WordGraph<uint32_t>>(
                  2,
                  {{1, 1, UNDEFINED, UNDEFINED, UNDEFINED, UNDEFINED},
                   {UNDEFINED, UNDEFINED, UNDEFINED, 0, 0, UNDEFINED}}));
    }
  }

  LIBSEMIGROUPS_TEST_CASE("Stephen",
                          "052",
                          "Incomplete Munn tree products",
                          "[stephen][quick]") {
    ReportGuard rg(false);
    using words::pow;
    v4::ToWord to_word("abcABC");

    InversePresentation<word_type> p;
    p.alphabet(to_word("abcABC"));
    p.inverses_no_checks(to_word("ABCabc"));

    auto S  = Stephen(p);
    auto Si = Stephen(p);
    auto T  = Stephen(p);
    auto Ti = Stephen(p);

    stephen::set_word(S, to_word("aBbcaABAabCc")).run();
    stephen::set_word(T, to_word("aBbcaABAabCc")).run();
    S *= T;
    S.run();

    stephen::set_word(Si, to_word("aBbcaABAabCc"));
    Si *= T;
    Si.run();
    REQUIRE(Si == S);

    stephen::set_word(Si, to_word("aBbcaABAabCc"));
    stephen::set_word(Ti, to_word("aBbcaABAabCc"));
    Si.run();
    Si *= Ti;
    Si.run();
    REQUIRE(Si == S);
  }

  // TODO(2): the examples from Stephen's paper/thesis?
}  // namespace libsemigroups
