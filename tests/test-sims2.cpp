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

#include "catch.hpp"  // for REQUIRE, REQUIRE_THROWS_AS, REQUI...
#include "libsemigroups/presentation.hpp"
#include "test-main.hpp"  // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/fpsemi-examples.hpp"
#include "libsemigroups/sims2.hpp"

namespace libsemigroups {
  using word_graph_type = typename Sims2::word_graph_type;
  using node_type       = typename word_graph_type::node_type;

  using namespace literals;

  LIBSEMIGROUPS_TEST_CASE("Sims2",
                          "000",
                          "temperley_lieb_monoid(4) from presentation",
                          "[quick][sims2][low-index]") {
    auto  rg = ReportGuard(false);
    Sims2 S;
    S.presentation(fpsemigroup::temperley_lieb_monoid(4));
    REQUIRE(S.number_of_congruences(14) == 9);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims2",
                          "092",
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

  // Takes approx. 13.5s in debug mode.
  LIBSEMIGROUPS_TEST_CASE("Sims2",
                          "093",
                          "2-sided T_4",
                          "[standard][sims2][no-valgrind][no-coverage]") {
    Sims2 s(fpsemigroup::full_transformation_monoid(4));

    REQUIRE(s.number_of_congruences(256) == 11);  // Verified with GAP
  }

  LIBSEMIGROUPS_TEST_CASE("Sims2", "094", "2-sided example", "[quick][sims1]") {
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
                          "095",
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
                          "096",
                          "2-sided 2-generated free monoid",
                          "[extreme][sims2]") {
    Presentation<std::string> p;
    p.alphabet("ab");
    p.contains_empty_word(true);
    Sims2 s(p);
    s.number_of_threads(1);  // FIXME fails if number of threads is >1
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
    // REQUIRE(s.number_of_congruences(13) == 1'387'117);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims2",
                          "097",
                          "2-sided congruence-free monoid n=3",
                          "[quick][sims2]") {
    // Presentation taken from
    // Al-Kharousi, F., Cain, A.J., Maltcev, V. et al.
    // A countable family of finitely presented infinite congruence-free monoids
    // https://doi.org/10.14232/actasm-013-028-z
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
                          "098",
                          "2-sided congruence-free monoid n=8",
                          "[quick][sims2]") {
    // Presentation taken from
    // Al-Kharousi, F., Cain, A.J., Maltcev, V. et al.
    // A countable family of finitely presented infinite congruence-free monoids
    // https://doi.org/10.14232/actasm-013-028-z
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
                          "099",
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
                          "100",
                          "2-sided 2-generated free commutative monoid",
                          "[quick][sims2]") {
    Presentation<std::string> p;
    p.alphabet("ab");
    p.contains_empty_word(true);
    presentation::add_rule(p, "ab", "ba");
    Sims2 s(p);
    s.number_of_threads(1);
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
}  // namespace libsemigroups
