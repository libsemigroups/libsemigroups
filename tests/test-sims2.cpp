//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2022-23 James D. Mitchell
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

#include "catch.hpp"      // for REQUIRE, REQUIRE_THROWS_AS, REQUI...
#include "test-main.hpp"  // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/fpsemi-examples.hpp"
#include "libsemigroups/presentation.hpp"
#include "libsemigroups/sims.hpp"

namespace libsemigroups {
  using word_graph_type = typename Sims2::word_graph_type;
  using node_type       = typename word_graph_type::node_type;

  using namespace literals;

  LIBSEMIGROUPS_TEST_CASE("Sims2",
                          "092",
                          "temperley_lieb_monoid(4) from presentation",
                          "[quick][sims2][low-index]") {
    auto  rg = ReportGuard(false);
    Sims2 S;
    S.presentation(fpsemigroup::temperley_lieb_monoid(4));
    REQUIRE(S.number_of_congruences(14) == 9);
    // S.presentation(fpsemigroup::full_transformation_monoid(5));

    // REQUIRE(
    //     S.number_of_threads(6).idle_thread_restarts(128).number_of_congruences(
    //         3125)
    //     == 14);
  }

  // Takes approx. 13.5s in debug mode.
  LIBSEMIGROUPS_TEST_CASE("Sims2",
                          "001",
                          "2-sided T_4",
                          "[standard][sims2][no-valgrind][no-coverage]") {
    Sims2 S(fpsemigroup::full_transformation_monoid(4));

    REQUIRE(S.number_of_congruences(256) == 11);  // Verified with GAP
  }

  LIBSEMIGROUPS_TEST_CASE("Sims2",
                          "002",
                          "2-sided T_4 Iwahori presentation",
                          "[quick][sims2][low-index]") {
    Sims2 S(fpsemigroup::full_transformation_monoid(
        4, fpsemigroup::author::Iwahori));
    REQUIRE(S.number_of_congruences(256) == 11);
  }

  LIBSEMIGROUPS_TEST_CASE(
      "Sims2",
      "003",
      "2-sided T_4 Aizenstat presentation",
      "[extreme][sims2][low-index][no-valgrind][no-coverage]") {
    Sims2 S(fpsemigroup::full_transformation_monoid(
        4, fpsemigroup::author::Aizenstat));
    // The below test takes too long to terminate
    REQUIRE(S.number_of_congruences(256) == 11);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims2",
                          "004",
                          "2-sided S_6 Burnside+Miller presentation",
                          "[quick][sims2][low-index]") {
    Sims2 S(fpsemigroup::symmetric_group(
        7, fpsemigroup::author::Burnside + fpsemigroup::author::Miller));
    REQUIRE(S.number_of_congruences(720) == 3);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims2",
                          "005",
                          "2-sided CI_4 Fernandes presentation",
                          "[quick][sims2][low-index]") {
    Sims2 S(fpsemigroup::cyclic_inverse_monoid(
        4, fpsemigroup::author::Fernandes, 0));
    REQUIRE(S.number_of_congruences(61) == 14);
    S.presentation(fpsemigroup::cyclic_inverse_monoid(
        4, fpsemigroup::author::Fernandes, 1));
    REQUIRE(S.number_of_congruences(61) == 14);
  }

  /*
  LIBSEMIGROUPS_TEST_CASE("Sims2",
                          "006",
                          "2-sided CI_4 Froidure-Pin presentation",
                          "[quick][sims2][low-index]") {
    FroidurePinBase<PPerm<4>> T;
    T.add_generator(PPerm<4>::make({1, 2, 3, 0}));
    T.add_generator(PPerm<4>::make({1, 2, 3}, {1, 2, 3}, 4));
    T.add_generator(PPerm<4>::make({0, 2, 3}, {0, 2, 3}, 4));
    T.add_generator(PPerm<4>::make({0, 1, 3}, {0, 1, 3}, 4));
    T.add_generator(PPerm<4>::make({0, 1, 2}, {0, 1, 2}, 4));
    auto p = to_presentation<word_type>(T);

    Sims2 S(p);
    // FIXME: Segfaults
    REQUIRE(S.number_of_congruences(61) == 14);
  }*/

  LIBSEMIGROUPS_TEST_CASE("Sims2",
                          "007",
                          "2-sided (2,3,7) tringle group",
                          "[quick][sims2][low-index]") {
    Presentation<std::string> p;
    p.contains_empty_word(true);
    p.alphabet("xy");
    presentation::add_rule(p, "xx", "");
    presentation::add_rule(p, "yyy", "");
    presentation::add_rule(p, "xyxyxyxyxyxyxy", "");
    Sims2 S(p);
    // Smallest non-trivial homomorphic image has size 168, see
    // https://mathoverflow.net/questions/180231/
    // for more details
    REQUIRE(S.number_of_congruences(168) == 2);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims2",
                          "008",
                          "2-sided Heineken group",
                          "[standard][sims2][low-index]") {
    Presentation<std::string> p;
    p.contains_empty_word(true);
    p.alphabet("xXyY");
    presentation::add_inverse_rules(p, "XxYy");
    presentation::add_rule(p, "yXYYxyYYxyyXYYxyyXyXYYxy", "x");
    presentation::add_rule(p, "YxyyXXYYxyxYxyyXYXyXYYxxyyXYXyXYYxyx", "y");

    Sims2 S(p);
    REQUIRE(S.number_of_threads(1).number_of_congruences(50) == 1);
    REQUIRE(S.number_of_threads(2).number_of_congruences(50) == 1);
    REQUIRE(S.number_of_threads(4).number_of_congruences(50) == 1);
    REQUIRE(S.number_of_threads(8).number_of_congruences(50) == 1);
    REQUIRE(S.number_of_threads(16).number_of_congruences(50) == 1);
    REQUIRE(S.number_of_threads(32).number_of_congruences(50) == 1);
  }

  /*
LIBSEMIGROUPS_TEST_CASE("Sims2",
                        "009",
                        "2-sided Catalan monoid n=4",
                        "[quick][sims2][low-index]") {
  FroidurePinBase<Transf<4>> S;
  S.add_generator(Transf<4>::make({0, 1, 2, 3}));
  S.add_generator(Transf<4>::make({0, 0, 2, 3}));
  S.add_generator(Transf<4>::make({0, 1, 1, 3}));
  S.add_generator(Transf<4>::make({0, 1, 2, 2}));
  REQUIRE(S.size() == 14);
  auto p = to_presentation<word_type>(S);

  // FIXME: Segfaults
  Sims2 C(p);
  REQUIRE(C.number_of_threads(1).number_of_congruences(S.size()) == 133);
  REQUIRE(C.number_of_threads(2).number_of_congruences(S.size()) == 133);
  REQUIRE(C.number_of_threads(4).number_of_congruences(S.size()) == 133);
  REQUIRE(C.number_of_threads(8).number_of_congruences(S.size()) == 133);
}*/

  LIBSEMIGROUPS_TEST_CASE("Sims2",
                          "010",
                          "2-sided Heineken monoid",
                          "[extreme][sims2][low-index]") {
    Presentation<std::string> p;
    p.contains_empty_word(true);
    p.alphabet("xyXY");
    presentation::add_rule(p, "yXYYxyYYxyyXYYxyyXyXYYxyX", "");
    presentation::add_rule(p, "YxyyXXYYxyxYxyyXYXyXYYxxyyXYXyXYYxyxY", "");
    Sims2 S(p);
    // TODO: check correctness
    REQUIRE(S.number_of_threads(8).number_of_congruences(8) == 63);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims2",
                          "011",
                          "2-sided Fibonacci(2, 9)",
                          "[extreme][sims2][low-index]") {
    Presentation<std::string> p;
    p.alphabet("abAB");
    p.contains_empty_word(true);
    presentation::add_inverse_rules(p, "ABab");
    presentation::add_rule(p, "Abababbab", "aBaaBaB");
    presentation::add_rule(p, "babbabbAb", "ABaaBaa");
    presentation::add_rule(p, "abbabbAbA", "BABaaBa");
    presentation::add_rule(p, "bbabbAbAA", "ABABaaB");
    presentation::add_rule(p, "babbAbAAb", "BABABaa");
    presentation::add_rule(p, "abbAbAAbA", "BBABABa");
    presentation::add_rule(p, "bbAbAAbAA", "ABBABAB");
    presentation::add_rule(p, "bAbAAbAAb", "BABBABA");
    presentation::add_rule(p, "AbAAbAAba", "BBABBAB");
    presentation::add_rule(p, "bAAbAAbab", "aBBABBA");
    presentation::add_rule(p, "AAbAAbaba", "BaBBABB");

    presentation::add_rule(p, "AAbababb", "BaaBaBBA");
    presentation::add_rule(p, "Abababba", "aBaaBaBB");
    presentation::add_rule(p, "abbabaaBaaB", "bAbAAbA");
    presentation::add_rule(p, "babaaBaaBaB", "BAbAbAA");

    Sims2 S(p);
    // TODO: check correctness
    REQUIRE(S.number_of_threads(8).number_of_congruences(64) == 10);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims2",
                          "012",
                          "2-sided one-relation baaabaaa=aba",
                          "[quick][sims2][low-index]") {
    Presentation<std::string> p;
    p.alphabet("ab");
    p.contains_empty_word(true);
    presentation::add_rule(p, "baaabaaa", "aba");

    Sims2 S(p);
    // TODO: check correctness
    REQUIRE(S.number_of_threads(8).number_of_congruences(1) == 1);
    REQUIRE(S.number_of_threads(8).number_of_congruences(2) == 5);
    REQUIRE(S.number_of_threads(8).number_of_congruences(3) == 17);
    REQUIRE(S.number_of_threads(8).number_of_congruences(4) == 52);
    REQUIRE(S.number_of_threads(8).number_of_congruences(5) == 148);
    REQUIRE(S.number_of_threads(8).number_of_congruences(6) == 413);
    REQUIRE(S.number_of_threads(8).number_of_congruences(7) == 1101);
    REQUIRE(S.number_of_threads(8).number_of_congruences(8) == 2901);
    REQUIRE(S.number_of_threads(8).number_of_congruences(9) == 7569);
    REQUIRE(S.number_of_threads(8).number_of_congruences(10) == 19756);
    REQUIRE(S.number_of_threads(8).number_of_congruences(11) == 50729);
    REQUIRE(S.number_of_threads(8).number_of_congruences(12) == 129157);
    REQUIRE(S.number_of_threads(8).number_of_congruences(13) == 330328);
  }

  LIBSEMIGROUPS_TEST_CASE(
      "Sims2",
      "013",
      "2-sided one-relation baabbaa=a",
      "[extreme][sims2][low-index][no-valgrind][no-coverage]") {
    Presentation<std::string> p;
    p.alphabet("ab");
    p.contains_empty_word(true);
    presentation::add_rule(p, "baabbaa", "a");

    Sims2 S(p);
    // TODO: check correctness
    // Takes a long time to run, seems like we get all the congruences quite
    // early on, but then spend very long checking that there are no more.
    // Perhaps if we had some sort of upper bound could speed things up?
    REQUIRE(S.number_of_threads(8).number_of_congruences(1) == 1);
    REQUIRE(S.number_of_threads(8).number_of_congruences(2) == 4);
    REQUIRE(S.number_of_threads(8).number_of_congruences(3) == 13);
    REQUIRE(S.number_of_threads(8).number_of_congruences(4) == 28);
    REQUIRE(S.number_of_threads(8).number_of_congruences(5) == 49);
    REQUIRE(S.number_of_threads(8).number_of_congruences(6) == 86);
    REQUIRE(S.number_of_threads(8).number_of_congruences(7) == 134);
    REQUIRE(S.number_of_threads(8).number_of_congruences(8) == 200);
    REQUIRE(S.number_of_threads(8).number_of_congruences(9) == 284);
    REQUIRE(S.number_of_threads(8).number_of_congruences(10) == 392);
    REQUIRE(S.number_of_threads(8).number_of_congruences(11) == 518);
    REQUIRE(S.number_of_threads(8).number_of_congruences(12) == 693);
    REQUIRE(S.number_of_threads(8).number_of_congruences(13) == 891);
    REQUIRE(S.number_of_threads(8).number_of_congruences(14) == 1127);
    REQUIRE(S.number_of_threads(8).number_of_congruences(15) == 1402);
    REQUIRE(S.number_of_threads(8).number_of_congruences(16) == 1733);
    REQUIRE(S.number_of_threads(8).number_of_congruences(17) == 2094);
    REQUIRE(S.number_of_threads(8).number_of_congruences(18) == 2531);
    REQUIRE(S.number_of_threads(8).number_of_congruences(19) == 3012);
    REQUIRE(S.number_of_threads(8).number_of_congruences(20) == 3574);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims2",
                          "093",
                          "2-sided full transformation monoid 2",
                          "[quick][sims2]") {
    Presentation<word_type> p;
    p.alphabet(2);
    p.contains_empty_word(true);
    presentation::add_rule(p, 00_w, {});
    presentation::add_rule(p, 01_w, 1_w);
    presentation::add_rule(p, 11_w, 1_w);
    Sims2 s(p);
    REQUIRE(s.number_of_congruences(4) == 4);  // Verified with GAP
    auto it = s.cbegin(4);
    REQUIRE(*(it++) == to_word_graph<node_type>(4, {{0, 0}}));          // ok
    REQUIRE(*(it++) == to_word_graph<node_type>(4, {{0, 1}, {1, 1}}));  // ok
    REQUIRE(*(it++)
            == to_word_graph<node_type>(4, {{1, 2}, {0, 2}, {2, 2}}));  // ok

    REQUIRE(*(it++)
            == to_word_graph<node_type>(
                4, {{1, 2}, {0, 2}, {3, 2}, {2, 2}}));  // ok
  }

  LIBSEMIGROUPS_TEST_CASE("Sims2", "095", "2-sided example", "[quick][sims1]") {
    Presentation<word_type> p;
    p.contains_empty_word(true);
    p.alphabet(01_w);
    presentation::add_rule(p, 000_w, 0_w);
    presentation::add_rule(p, 11_w, 1_w);
    presentation::add_rule(p, 0101_w, 0_w);
    Sims2 s(p);

    // REQUIRE(s.number_of_congruences(4) == 6);  // Verified with GAP
    auto it = s.cbegin(5);
    // Verified in 000
    REQUIRE(*(it++) == to_word_graph<node_type>(5, {{0, 0}}));
    REQUIRE(*(it++) == to_word_graph<node_type>(5, {{1, 0}, {1, 1}}));
    REQUIRE(*(it++) == to_word_graph<node_type>(5, {{1, 1}, {1, 1}}));
    REQUIRE(*(it++) == to_word_graph<node_type>(5, {{1, 2}, {1, 1}, {1, 2}}));
    REQUIRE(*(it++) == to_word_graph<node_type>(5, {{1, 2}, {1, 1}, {2, 2}}));
    REQUIRE(*(it++)
            == to_word_graph<node_type>(5, {{1, 2}, {1, 1}, {3, 2}, {3, 3}}));
  }

  LIBSEMIGROUPS_TEST_CASE("Sims2",
                          "096",
                          "2-sided full transf. monoid 3",
                          "[quick][sims2]") {
    Presentation<std::string> p;
    p.alphabet("abc");
    p.contains_empty_word(true);
    presentation::add_rule(p, "b^2"_p, ""_p);
    presentation::add_rule(p, "bc"_p, "ac"_p);
    presentation::add_rule(p, "c^2"_p, "c"_p);
    presentation::add_rule(p, "a^3"_p, ""_p);
    presentation::add_rule(p, "a^2b"_p, "ba"_p);
    presentation::add_rule(p, "aba"_p, "b"_p);
    presentation::add_rule(p, "baa"_p, "ab"_p);
    presentation::add_rule(p, "bab"_p, "aa"_p);
    presentation::add_rule(p, "bac"_p, "c"_p);
    presentation::add_rule(p, "cac"_p, "cb"_p);
    presentation::add_rule(p, "aca^2c"_p, "ca^2c"_p);
    presentation::add_rule(p, "ca^2cb"_p, "ca^2ca"_p);
    presentation::add_rule(p, "ca^2cab"_p, "ca^2c"_p);
    Sims2 s(p);
    REQUIRE(s.number_of_congruences(27) == 7);  // Verified with GAP

    auto it = s.cbegin(27);

    REQUIRE(*(it++) == to_word_graph<node_type>(27, {{0, 0, 0}}));  // ok
    REQUIRE(*(it++)
            == to_word_graph<node_type>(27, {{0, 0, 1}, {1, 1, 1}}));  // ok
    REQUIRE(*(it++)
            == to_word_graph<node_type>(
                27, {{0, 1, 2}, {1, 0, 2}, {2, 2, 2}}));  // ok
    REQUIRE(*(it++)
            == to_word_graph<node_type>(27,
                                        {{1, 2, 3},
                                         {4, 5, 3},
                                         {6, 0, 3},
                                         {3, 3, 3},
                                         {0, 6, 3},
                                         {2, 1, 3},
                                         {5, 4, 3}}));  // ok
    REQUIRE(*(it++)
            == to_word_graph<node_type>(27,
                                        {{1, 2, 3},
                                         {4, 5, 6},
                                         {7, 0, 6},
                                         {8, 3, 3},
                                         {0, 7, 9},
                                         {2, 1, 9},
                                         {10, 6, 6},
                                         {5, 4, 3},
                                         {11, 11, 3},
                                         {12, 9, 9},
                                         {13, 13, 6},
                                         {3, 8, 14},
                                         {15, 15, 9},
                                         {6, 10, 14},
                                         {14, 14, 14},
                                         {9, 12, 14}}));  // ok
    REQUIRE(*(it++)
            == to_word_graph<node_type>(
                27, {{1, 2, 3},    {4, 5, 6},    {7, 0, 6},    {8, 9, 3},
                     {0, 7, 10},   {2, 1, 10},   {11, 12, 6},  {5, 4, 3},
                     {13, 14, 9},  {15, 3, 9},   {16, 17, 10}, {18, 19, 12},
                     {20, 6, 12},  {3, 15, 21},  {9, 8, 21},   {14, 13, 3},
                     {22, 23, 17}, {24, 10, 17}, {6, 20, 21},  {12, 11, 21},
                     {19, 18, 6},  {21, 21, 21}, {10, 24, 21}, {17, 16, 21},
                     {23, 22, 10}}));  // ok
    REQUIRE(*(it++)
            == to_word_graph<node_type>(
                27, {{1, 2, 3},    {4, 5, 6},    {7, 0, 6},    {8, 9, 3},
                     {0, 7, 10},   {2, 1, 10},   {11, 12, 6},  {5, 4, 3},
                     {13, 14, 9},  {15, 3, 9},   {16, 17, 10}, {18, 19, 12},
                     {20, 6, 12},  {3, 15, 21},  {9, 8, 21},   {14, 13, 3},
                     {22, 23, 17}, {24, 10, 17}, {6, 20, 21},  {12, 11, 21},
                     {19, 18, 6},  {25, 25, 21}, {10, 24, 21}, {17, 16, 21},
                     {23, 22, 10}, {26, 21, 25}, {21, 26, 21}}));  // ok
  }

  LIBSEMIGROUPS_TEST_CASE("Sims2",
                          "097",
                          "2-sided 2-generated free monoid",
                          "[extreme][sims2]") {
    Presentation<std::string> p;
    p.alphabet("ab");
    p.contains_empty_word(true);
    Sims2 s(p);
    s.number_of_threads(4);
    REQUIRE(s.number_of_congruences(1) == 1);
    REQUIRE(s.number_of_congruences(2) == 7);    // verified with GAP
    REQUIRE(s.number_of_congruences(3) == 27);   // verified with GAP
    REQUIRE(s.number_of_congruences(4) == 94);   // verified with GAP
    REQUIRE(s.number_of_congruences(5) == 275);  // verified with GAP
    REQUIRE(s.number_of_congruences(6) == 833);
    REQUIRE(s.number_of_congruences(7) == 2'307);
    REQUIRE(s.number_of_congruences(8) == 6'488);
    REQUIRE(s.number_of_congruences(9) == 18'207);
    REQUIRE(s.number_of_congruences(10) == 52'960);
    REQUIRE(s.number_of_congruences(11) == 156'100);
    REQUIRE(s.number_of_congruences(12) == 462'271);
    REQUIRE(s.number_of_congruences(13) == 1'387'117);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims2", "098", "todo", "[extreme][sims2]") {
    auto p = symmetric_inverse_monoid(5, fpsemigroup::author::Gay);
    presentation::remove_duplicate_rules(p);
    presentation::sort_each_rule(p);
    presentation::sort_rules(p);

    Sims2 C(p);
    REQUIRE(C.number_of_threads(1).number_of_congruences(1546) == 14);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims2",
                          "099",
                          "2-sided congruence-free monoid n=3",
                          "[quick][sims2]") {
    // Presentation taken from
    // Al-Kharousi, F., Cain, A.J., Maltcev, V. et al.
    // A countable family of finitely presented infinite congruence-free
    // monoids https://doi.org/10.14232/actasm-013-028-z
    Presentation<std::string> p;
    p.alphabet("abcdz");
    p.contains_empty_word(true);
    presentation::add_zero_rules(p, 'z');
    presentation::add_rule(p, "ac", "");
    presentation::add_rule(p, "db", "");
    presentation::add_rule(p, "dc", "");
    // n = 3
    presentation::add_rule(p, "dab", "");
    presentation::add_rule(p, "da^2b"_p, "");
    presentation::add_rule(p, "a^3b"_p, "z");

    Sims2 s(p);
    s.number_of_threads(1);
    REQUIRE(s.number_of_congruences(1) == 1);
    REQUIRE(s.number_of_congruences(2) == 1);
    REQUIRE(s.number_of_congruences(3) == 1);
    REQUIRE(s.number_of_congruences(4) == 1);
    REQUIRE(s.number_of_congruences(5) == 1);
    REQUIRE(s.number_of_congruences(10) == 1);
    REQUIRE(s.number_of_congruences(20) == 1);
    REQUIRE(s.number_of_congruences(30) == 1);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims2",
                          "100",
                          "2-sided congruence-free monoid n=8",
                          "[quick][sims2]") {
    // Presentation taken from
    // Al-Kharousi, F., Cain, A.J., Maltcev, V. et al.
    // A countable family of finitely presented infinite congruence-free
    // monoids https://doi.org/10.14232/actasm-013-028-z
    Presentation<std::string> p;
    p.alphabet("abcdz");
    p.contains_empty_word(true);
    presentation::add_zero_rules(p, 'z');
    presentation::add_rule(p, "ac", "");
    presentation::add_rule(p, "db", "");
    presentation::add_rule(p, "dc", "");
    // n = 8
    presentation::add_rule(p, "dab", "");
    presentation::add_rule(p, "da^2b"_p, "");
    presentation::add_rule(p, "da^3b"_p, "");
    presentation::add_rule(p, "da^4b"_p, "");
    presentation::add_rule(p, "da^5b"_p, "");
    presentation::add_rule(p, "da^6b"_p, "");
    presentation::add_rule(p, "da^7b"_p, "");
    presentation::add_rule(p, "a^8b"_p, "z");

    Sims2 s(p);
    s.number_of_threads(1);
    REQUIRE(s.number_of_congruences(1) == 1);
    REQUIRE(s.number_of_congruences(2) == 1);
    REQUIRE(s.number_of_congruences(3) == 1);
    REQUIRE(s.number_of_congruences(4) == 1);
    REQUIRE(s.number_of_congruences(5) == 1);
    REQUIRE(s.number_of_congruences(10) == 1);
    REQUIRE(s.number_of_congruences(20) == 1);
    REQUIRE(s.number_of_congruences(30) == 1);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims2",
                          "101",
                          "2-sided bicyclic monoid",
                          "[quick][sims2]") {
    Presentation<std::string> p;
    p.alphabet("ab");
    p.contains_empty_word(true);
    presentation::add_rule(p, "ab"_p, ""_p);
    Sims2 s(p);
    s.number_of_threads(1);
    for (size_t i = 1; i < 50; ++i) {
      REQUIRE(s.number_of_congruences(i) == i);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("Sims2",
                          "102",
                          "2-sided 2-generated free commutative monoid",
                          "[quick][sims2]") {
    Presentation<std::string> p;
    p.alphabet("ab");
    p.contains_empty_word(true);
    presentation::add_rule(p, "ab", "ba");

    Sims2 s(p);
    s.number_of_threads(4);
    REQUIRE(s.number_of_congruences(1) == 1);
    REQUIRE(s.number_of_congruences(2) == 7);  // verified by hand
    REQUIRE(s.number_of_congruences(3) == 25);
    REQUIRE(s.number_of_congruences(4) == 76);
    REQUIRE(s.number_of_congruences(5) == 184);
    REQUIRE(s.number_of_congruences(6) == 432);
    REQUIRE(s.number_of_congruences(7) == 892);
    REQUIRE(s.number_of_congruences(8) == 1'800);
    REQUIRE(s.number_of_congruences(9) == 3'402);
    REQUIRE(s.number_of_congruences(10) == 6'280);
    REQUIRE(s.number_of_congruences(11) == 11'051);
    REQUIRE(s.number_of_congruences(12) == 19'245);
    REQUIRE(s.number_of_congruences(13) == 32'299);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims2",
                          "103",
                          "free semilattice n = 8",
                          "[extreme][sims1]") {
    // https://oeis.org/A102894
    constexpr std::array<size_t, 6> results = {0, 1, 4, 45, 2'271, 1'373'701};
    for (auto A : {"a", "ab", "abc", "abcd", "abcde"}) {
      Presentation<std::string> p;
      p.alphabet(A);
      presentation::add_commutes_rules_no_checks(p, p.alphabet());
      presentation::add_idempotent_rules_no_checks(p, p.alphabet());
      Sims2 s(p);

      size_t const n = p.alphabet().size();
      REQUIRE(s.number_of_threads(4).number_of_congruences(std::pow(2, n))
              == results[n]);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("Sims2",
                          "104",
                          "2-sided 2-generated free semigroup",
                          "[extreme][sims2]") {
    Presentation<std::string> p;
    p.alphabet("ab");
    p.contains_empty_word(false);  // FIXME: Method fails for non-monoids
    Sims2 s(p);
    s.number_of_threads(4);
    // Number of congruences with up to 7 classes given in:
    // A. Bailey, M. Finn-Sell and R. Snocken
    // "SUBSEMIGROUP, IDEAL AND CONGRUENCE GROWTH OF FREE SEMIGROUPS"
    REQUIRE(s.number_of_congruences(1) == 1);
    REQUIRE(s.number_of_congruences(2) == 11);    // From Bailey et al
    REQUIRE(s.number_of_congruences(3) == 51);    // From Bailey et al
    REQUIRE(s.number_of_congruences(4) == 200);   // From Bailey et al
    REQUIRE(s.number_of_congruences(5) == 657);   // From Bailey et al
    REQUIRE(s.number_of_congruences(6) == 2037);  // From Bailey et al
    REQUIRE(s.number_of_congruences(7) == 5977);  // From Bailey et al
  }
}  // namespace libsemigroups
