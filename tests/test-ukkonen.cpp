// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2021-2023 James D. Mitchell + Maria Tsalakou
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

#include <algorithm>  // for count_if, all_of
#include <cstddef>    // for size_t
#include <tuple>      // for tie
#include <utility>    // for pair
#include <vector>     // for vector

#include "catch_amalgamated.hpp"  // for REQUIRE, REQUIRE_THROWS_AS
#include "test-main.hpp"          // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/constants.hpp"   // for operator==, POSITIVE_INFINITY
#include "libsemigroups/exception.hpp"   // for LIBSEMIGROUPS_EXCEPTION
#include "libsemigroups/types.hpp"       // for word_type
#include "libsemigroups/ukkonen.hpp"     // for Ukkonen, Ukkonen::State
#include "libsemigroups/word-range.hpp"  // for literals

#include "libsemigroups/detail/int-range.hpp"  // for IntRange

namespace libsemigroups {

  using literals::operator""_w;

  namespace {
    // TODO Should really be just a use of
    // presentation::longest_subword_reducing_length
    auto best_subword(Ukkonen& u) {
      ukkonen::detail::GreedyReduceHelper helper(u);
      return ukkonen::dfs(u, helper);
    }
  }  // namespace

  LIBSEMIGROUPS_TEST_CASE("Ukkonen", "000", "basic tests", "[quick][ukkonen]") {
    Ukkonen t;
    // aaeaaa$
    // abcd$'
    // a -> 0
    // b -> 1
    // c -> 2
    // d -> 3
    // e -> 4
    ukkonen::add_word(t, 004000_w);
    REQUIRE(t.nodes().size() == 10);

    REQUIRE(ukkonen::is_subword(t, 004000_w));
    REQUIRE(ukkonen::is_subword(t, 04_w));
    REQUIRE(!ukkonen::is_subword(t, 44_w));
    REQUIRE(ukkonen::is_subword(t, ""_w));
    REQUIRE(ukkonen::is_subword(t, 0_w));
    REQUIRE(ukkonen::is_subword(t, 00_w));
    REQUIRE(ukkonen::is_subword(t, 000_w));
    REQUIRE(ukkonen::is_subword(t, 000_w));
    REQUIRE(!ukkonen::is_subword(t, 0000_w));
    REQUIRE(!ukkonen::is_subword(t, 1_w));
    REQUIRE(ukkonen::number_of_distinct_subwords(t) == 16);
    WordRange w;
    w.alphabet_size(6).min(0).max(8);
    REQUIRE((w | rx::filter([&t](word_type const& u) {
               return ukkonen::is_subword(t, u);
             })
             | rx::count())
            == 16);

    REQUIRE(ukkonen::is_subword(t, ""_w));  // 1
    REQUIRE(ukkonen::is_subword(t, 004000_w));
    REQUIRE(ukkonen::is_subword(t, 00400_w));
    REQUIRE(ukkonen::is_subword(t, 0040_w));
    REQUIRE(ukkonen::is_subword(t, 004_w));  // 5
    REQUIRE(ukkonen::is_subword(t, 00_w));
    REQUIRE(ukkonen::is_subword(t, 0_w));
    REQUIRE(ukkonen::is_subword(t, 04000_w));
    REQUIRE(ukkonen::is_subword(t, 0400_w));
    REQUIRE(ukkonen::is_subword(t, 040_w));  // 10
    REQUIRE(ukkonen::is_subword(t, 04_w));
    REQUIRE(ukkonen::is_subword(t, 4000_w));
    REQUIRE(ukkonen::is_subword(t, 400_w));
    REQUIRE(ukkonen::is_subword(t, 40_w));
    REQUIRE(ukkonen::is_subword(t, 4_w));    // 15
    REQUIRE(ukkonen::is_subword(t, 000_w));  // 16

    ukkonen::add_word(t, 0123_w);
    REQUIRE(t.nodes().size() == 15);

    REQUIRE(ukkonen::is_subword(t, ""_w));  // 1
    REQUIRE(ukkonen::is_subword(t, 004000_w));
    REQUIRE(ukkonen::is_subword(t, 00400_w));
    REQUIRE(ukkonen::is_subword(t, 0040_w));
    REQUIRE(ukkonen::is_subword(t, 004_w));  // 5
    REQUIRE(ukkonen::is_subword(t, 00_w));
    REQUIRE(ukkonen::is_subword(t, 0_w));
    REQUIRE(ukkonen::is_subword(t, 04000_w));
    REQUIRE(ukkonen::is_subword(t, 0400_w));
    REQUIRE(ukkonen::is_subword(t, 040_w));  // 10
    REQUIRE(ukkonen::is_subword(t, 04_w));
    REQUIRE(ukkonen::is_subword(t, 4000_w));
    REQUIRE(ukkonen::is_subword(t, 400_w));
    REQUIRE(ukkonen::is_subword(t, 40_w));
    REQUIRE(ukkonen::is_subword(t, 4_w));    // 15
    REQUIRE(ukkonen::is_subword(t, 000_w));  // 16

    REQUIRE(ukkonen::is_subword(t, 01_w));
    REQUIRE(ukkonen::is_subword(t, 012_w));
    REQUIRE(ukkonen::is_subword(t, 0123_w));
    REQUIRE(ukkonen::is_subword(t, 1_w));
    REQUIRE(ukkonen::is_subword(t, 12_w));
    REQUIRE(ukkonen::is_subword(t, 123_w));
    REQUIRE(ukkonen::is_subword(t, 2_w));
    REQUIRE(ukkonen::is_subword(t, 23_w));
    REQUIRE(ukkonen::is_subword(t, 3_w));

    REQUIRE(!ukkonen::is_subword(t, 33_w));
    REQUIRE(!ukkonen::is_subword(t, "ab"));
    REQUIRE(!ukkonen::is_subword(t, std::string("ab")));
    REQUIRE(!ukkonen::is_subword_no_checks(t, 33_w));
    REQUIRE(!ukkonen::is_subword_no_checks(t, "ab"));
    REQUIRE(!ukkonen::is_subword_no_checks(t, std::string("ab")));
    REQUIRE_THROWS_AS(ukkonen::is_subword(t, word_type({UNDEFINED})),
                      LibsemigroupsException);

    REQUIRE(ukkonen::number_of_distinct_subwords(t) == 25);

    REQUIRE(!ukkonen::is_suffix(t, 1235_w));
    REQUIRE(ukkonen::is_suffix(t, 123_w));

    REQUIRE(ukkonen::is_suffix(t, ""_w));
    REQUIRE(ukkonen::is_suffix(t, 004000_w));
    REQUIRE(ukkonen::is_suffix(t, 04000_w));
    REQUIRE(ukkonen::is_suffix(t, 4000_w));
    REQUIRE(ukkonen::is_suffix(t, 000_w));
    REQUIRE(ukkonen::is_suffix(t, 00_w));
    REQUIRE(ukkonen::is_suffix(t, 0_w));
    REQUIRE(ukkonen::is_suffix(t, 0123_w));
    REQUIRE(ukkonen::is_suffix(t, 123_w));
    REQUIRE(ukkonen::is_suffix(t, 23_w));
    REQUIRE(ukkonen::is_suffix(t, 3_w));
    REQUIRE(!ukkonen::is_suffix(t, 33_w));
    REQUIRE(!ukkonen::is_suffix(t, "ab"));
    REQUIRE_THROWS_AS(ukkonen::is_suffix(t, word_type({UNDEFINED})),
                      LibsemigroupsException);
    REQUIRE(!ukkonen::is_suffix(t, std::string("ab")));
    REQUIRE(!ukkonen::is_suffix_no_checks(t, 33_w));
    REQUIRE(!ukkonen::is_suffix_no_checks(t, "ab"));
    REQUIRE(!ukkonen::is_suffix_no_checks(t, std::string("ab")));

    w.alphabet_size(5).min(0).max(7);
    REQUIRE((w | rx::filter([&t](word_type const& u) {
               return ukkonen::is_suffix(t, u);
             })
             | rx::count())
            == 11);

    REQUIRE(ukkonen::length_maximal_piece_prefix(t, 004000_w) == 2);
    REQUIRE(ukkonen::length_maximal_piece_prefix(t, 0123_w) == 1);
    REQUIRE_THROWS_AS(
        ukkonen::length_maximal_piece_prefix(t, word_type({UNDEFINED})),
        LibsemigroupsException);

    REQUIRE(ukkonen::length_maximal_piece_prefix(t, "ab") == 0);
    REQUIRE(ukkonen::length_maximal_piece_prefix(t, std::string("ab")) == 0);
    REQUIRE(ukkonen::length_maximal_piece_prefix(t, 00043456_w) == 2);
    REQUIRE(ukkonen::length_maximal_piece_prefix(t, 0123456_w) == 1);
    REQUIRE(ukkonen::length_maximal_piece_prefix_no_checks(t, "ab") == 0);
    REQUIRE(ukkonen::length_maximal_piece_prefix_no_checks(t, std::string("ab"))
            == 0);
    REQUIRE(ukkonen::length_maximal_piece_prefix_no_checks(t, 00043456_w) == 2);
    REQUIRE(ukkonen::length_maximal_piece_prefix_no_checks(t, 0123456_w) == 1);

    REQUIRE(*ukkonen::maximal_piece_prefix(t, "ab") == 'a');
    REQUIRE(*ukkonen::maximal_piece_prefix(t, std::string("ab")) == 'a');
    REQUIRE(*ukkonen::maximal_piece_prefix(t, 00043456_w) == 0);
    REQUIRE(*ukkonen::maximal_piece_prefix(t, 0123456_w) == 1);
    REQUIRE(*ukkonen::maximal_piece_prefix_no_checks(t, "ab") == 'a');
    REQUIRE(*ukkonen::maximal_piece_prefix_no_checks(t, std::string("ab"))
            == 'a');
    REQUIRE(*ukkonen::maximal_piece_prefix_no_checks(t, 00043456_w) == 0);
    REQUIRE(*ukkonen::maximal_piece_prefix_no_checks(t, 0123456_w) == 1);
  }

  LIBSEMIGROUPS_TEST_CASE("Ukkonen",
                          "001",
                          "maximal_piece_prefix 1",
                          "[quick][ukkonen]") {
    Ukkonen t;
    ukkonen::add_word(t, {0, 5, 7});
    ukkonen::add_word(t, {1, 6, 7});
    ukkonen::add_word(t, {7, 2});
    ukkonen::add_word(t, {3, 4});
    ukkonen::add_word(t, {4, 8});
    ukkonen::add_word(t, {9});
    ukkonen::add_word(t, {5, 7, 10});
    ukkonen::add_word(t, {6, 7, 11});

    REQUIRE(t.nodes().size() == 32);
    REQUIRE(ukkonen::length_maximal_piece_prefix(t, {0, 5, 7}) == 0);
    REQUIRE(ukkonen::length_maximal_piece_prefix(t, {1, 6, 7}) == 0);
    REQUIRE(ukkonen::length_maximal_piece_prefix(t, {7, 2}) == 1);
    REQUIRE(ukkonen::length_maximal_piece_prefix(t, {3, 4}) == 0);
    REQUIRE(ukkonen::length_maximal_piece_prefix(t, {4, 8}) == 1);
    REQUIRE(ukkonen::length_maximal_piece_prefix(t, {9}) == 0);
    REQUIRE(ukkonen::length_maximal_piece_prefix(t, {5, 7, 10}) == 2);
    REQUIRE(ukkonen::length_maximal_piece_prefix(t, {6, 7, 11}) == 2);
  }

  LIBSEMIGROUPS_TEST_CASE("Ukkonen",
                          "002",
                          "maximal_piece_prefix 2",
                          "[quick][ukkonen]") {
    Ukkonen t;
    ukkonen::add_word(t, 004000_w);
    ukkonen::add_word(t, 45_w);

    REQUIRE(ukkonen::number_of_distinct_subwords(t) == 18);
    REQUIRE(ukkonen::length_maximal_piece_prefix(t, 004000_w) == 2);
    REQUIRE(ukkonen::length_maximal_piece_prefix(t, 45_w) == 1);

    ukkonen::add_word(t, 0123_w);
    REQUIRE(ukkonen::number_of_distinct_subwords(t) == 27);
    REQUIRE(ukkonen::length_maximal_piece_prefix(t, 004000_w) == 2);
    REQUIRE(ukkonen::length_maximal_piece_prefix(t, 45_w) == 1);
    REQUIRE(ukkonen::length_maximal_piece_prefix(t, 0123_w) == 1);

    ukkonen::add_word(t, 004_w);
    REQUIRE(ukkonen::number_of_distinct_subwords(t) == 27);
    REQUIRE(ukkonen::length_maximal_piece_prefix(t, 004000_w) == 3);
    REQUIRE(ukkonen::length_maximal_piece_prefix(t, "00456789"_w) == 3);
    REQUIRE(ukkonen::length_maximal_piece_prefix(t, 45_w) == 1);
    REQUIRE(ukkonen::length_maximal_piece_prefix(t, 0123_w) == 1);
    REQUIRE(ukkonen::length_maximal_piece_prefix(t, 004_w) == 3);
  }

  LIBSEMIGROUPS_TEST_CASE("Ukkonen",
                          "003",
                          "maximal_piece_prefix 3",
                          "[quick][ukkonen]") {
    Ukkonen t;
    REQUIRE(t.number_of_distinct_words() == 0);
    ukkonen::add_word(t, 012_w);
    REQUIRE(t.number_of_distinct_words() == 1);
    ukkonen::add_word(t, 124_w);
    REQUIRE(t.number_of_distinct_words() == 2);

    REQUIRE(t.nodes().size() == 11);
    REQUIRE(ukkonen::number_of_distinct_subwords(t) == 10);

    REQUIRE(ukkonen::is_subword(t, ""_w));
    REQUIRE(ukkonen::is_subword(t, 0_w));
    REQUIRE(ukkonen::is_subword(t, 1_w));
    REQUIRE(ukkonen::is_subword(t, 2_w));
    REQUIRE(ukkonen::is_subword(t, 4_w));
    REQUIRE(ukkonen::is_subword(t, 01_w));
    REQUIRE(ukkonen::is_subword(t, 12_w));
    REQUIRE(ukkonen::is_subword(t, 24_w));
    REQUIRE(ukkonen::is_subword(t, 012_w));
    REQUIRE(ukkonen::is_subword(t, 124_w));
    REQUIRE_THROWS_AS(
        ukkonen::is_subword(
            t, std::vector<size_t>({static_cast<size_t>(-1), 124})),
        LibsemigroupsException);

    REQUIRE(!ukkonen::is_subword(t, 123_w));
    REQUIRE(!ukkonen::is_subword(t, 1234_w));
    REQUIRE(!ukkonen::is_subword(t, 3_w));
    REQUIRE(!ukkonen::is_subword(t, 13_w));

    WordRange words;
    words.alphabet_size(5).min(0).max(5);
    REQUIRE((words | rx::filter([&t](word_type const& w) {
               return ukkonen::is_subword(t, w);
             })
             | rx::count())
            == 10);

    REQUIRE(ukkonen::is_suffix(t, ""_w));
    REQUIRE(!ukkonen::is_suffix(t, 0_w));
    REQUIRE(!ukkonen::is_suffix(t, 1_w));
    REQUIRE(ukkonen::is_suffix(t, 2_w));
    REQUIRE(ukkonen::is_suffix(t, 4_w));
    REQUIRE(!ukkonen::is_suffix(t, 01_w));
    REQUIRE(ukkonen::is_suffix(t, 12_w));
    REQUIRE(ukkonen::is_suffix(t, 24_w));
    REQUIRE(ukkonen::is_suffix(t, 012_w));
    REQUIRE(ukkonen::is_suffix(t, 124_w));
    REQUIRE_THROWS_AS(ukkonen::is_suffix(t, {static_cast<size_t>(-1), 124}),
                      LibsemigroupsException);

    REQUIRE(ukkonen::length_maximal_piece_prefix(t, 012_w) == 0);
    REQUIRE(ukkonen::length_maximal_piece_suffix(t, 012_w) == 2);
    REQUIRE(ukkonen::length_maximal_piece_suffix_no_checks(t, 012_w) == 2);
    REQUIRE(ukkonen::length_maximal_piece_suffix(t, 124_w) == 0);
    REQUIRE(ukkonen::length_maximal_piece_suffix_no_checks(t, 124_w) == 0);

    REQUIRE(*ukkonen::maximal_piece_suffix(t, 012_w) == 1);
    auto w = 124_w;
    REQUIRE(ukkonen::maximal_piece_suffix(t, w) == w.cend());

    REQUIRE(*ukkonen::maximal_piece_suffix_no_checks(t, 012_w) == 1);
    REQUIRE(ukkonen::maximal_piece_suffix_no_checks(t, w) == w.cend());

    REQUIRE(ukkonen::number_of_pieces(t, 012_w) == POSITIVE_INFINITY);
    REQUIRE(ukkonen::number_of_pieces_no_checks(t, 012_w) == POSITIVE_INFINITY);
    REQUIRE(ukkonen::number_of_pieces_no_checks(t, "abc") == POSITIVE_INFINITY);
    REQUIRE(ukkonen::number_of_pieces_no_checks(t, std::string("abc"))
            == POSITIVE_INFINITY);
    REQUIRE(ukkonen::number_of_pieces(t, 012_w) == POSITIVE_INFINITY);
    REQUIRE(ukkonen::number_of_pieces(t, "abc") == POSITIVE_INFINITY);
    REQUIRE(ukkonen::number_of_pieces(t, std::string("abc"))
            == POSITIVE_INFINITY);
    REQUIRE(ukkonen::pieces(t, 012_w) == std::vector<word_type>());
    REQUIRE(ukkonen::length_maximal_piece_prefix(t, 124_w) == 2);
    REQUIRE(ukkonen::number_of_pieces(t, 124_w) == POSITIVE_INFINITY);
    REQUIRE(ukkonen::pieces(t, 124_w) == std::vector<word_type>());
  }

  LIBSEMIGROUPS_TEST_CASE("Ukkonen",
                          "004",
                          "number_of_pieces",
                          "[quick][ukkonen]") {
    Ukkonen t;
    ukkonen::add_word(t, 012_w);
    ukkonen::add_word(t, 0_w);
    ukkonen::add_word(t, 1_w);
    ukkonen::add_word(t, 2_w);

    REQUIRE(ukkonen::number_of_pieces(t, 012_w) == 3);
    REQUIRE(ukkonen::pieces(t, 012_w)
            == std::vector<word_type>({0_w, 1_w, 2_w}));
    REQUIRE(ukkonen::number_of_pieces(t, 0_w) == 1);
    REQUIRE(ukkonen::pieces(t, 0_w) == std::vector<word_type>({0_w}));
    REQUIRE(ukkonen::number_of_pieces(t, 1_w) == 1);
    REQUIRE(ukkonen::pieces(t, 1_w) == std::vector<word_type>({1_w}));
    REQUIRE(ukkonen::number_of_pieces(t, 2_w) == 1);
    REQUIRE(ukkonen::pieces(t, 2_w) == std::vector<word_type>({2_w}));

    ukkonen::add_word(t, "01284567"_w);
    ukkonen::add_word(t, 012_w);  // does nothing
    ukkonen::add_word(t, 845_w);
    ukkonen::add_word(t, 56_w);
    ukkonen::add_word(t, 567_w);

    REQUIRE(t.number_of_distinct_words() == 8);
    REQUIRE(t.number_of_words() == 9);

    REQUIRE(ukkonen::number_of_pieces(t, 012_w) == 1);
    REQUIRE(ukkonen::pieces(t, 012_w) == std::vector<word_type>({012_w}));
    REQUIRE(ukkonen::number_of_pieces(t, 0_w) == 1);
    REQUIRE(ukkonen::number_of_pieces(t, 1_w) == 1);
    REQUIRE(ukkonen::number_of_pieces(t, 2_w) == 1);

    REQUIRE(ukkonen::number_of_pieces(t, "01284567"_w) == 3);
    REQUIRE(ukkonen::pieces(t, "01284567"_w)
            == std::vector<word_type>({012_w, 845_w, 67_w}));
    REQUIRE(ukkonen::is_piece(t, 012_w));
    REQUIRE(ukkonen::is_piece(t, 845_w));
    REQUIRE(ukkonen::is_piece(t, 67_w));
    REQUIRE(ukkonen::is_piece_no_checks(t, 012_w));
    REQUIRE(ukkonen::is_piece_no_checks(t, 845_w));
    REQUIRE(ukkonen::is_piece_no_checks(t, 67_w));
    REQUIRE(ukkonen::number_of_pieces(t, 845_w) == 1);
    REQUIRE(ukkonen::pieces(t, 845_w) == std::vector<word_type>({845_w}));
    REQUIRE(ukkonen::number_of_pieces(t, 56_w) == 1);
    REQUIRE(ukkonen::pieces(t, 56_w) == std::vector<word_type>({56_w}));
    REQUIRE(ukkonen::number_of_pieces(t, 567_w) == 1);

    REQUIRE(ukkonen::pieces(t, 567_w) == std::vector<word_type>({567_w}));
    REQUIRE(ukkonen::number_of_pieces(t, 12845_w) == 2);
    REQUIRE(ukkonen::pieces(t, 12845_w)
            == std::vector<word_type>({12_w, 845_w}));

    auto w = "0128456701284567"_w;
    REQUIRE(ukkonen::pieces(t, w.cbegin(), w.cend()).size() == 7);
    REQUIRE(ukkonen::pieces(t, w)
            == std::vector<word_type>(
                {{0, 1, 2}, {8, 4, 5}, {6, 7}, {0, 1, 2}, {8, 4, 5}, {6, 7}}));
  }

  LIBSEMIGROUPS_TEST_CASE("Ukkonen", "005", "traverse", "[quick][ukkonen]") {
    using State = Ukkonen::State;
    Ukkonen t;
    ukkonen::add_word(t, 004000_w);

    auto s = ukkonen::traverse(t, word_type({}));
    REQUIRE(s.first.v == 0);
    REQUIRE(s.first.pos == 0);

    s = ukkonen::traverse(t, 4_w);
    REQUIRE(s.first.v == 4);
    REQUIRE(s.first.pos == 1);

    s = ukkonen::traverse(t, 40_w);
    REQUIRE(s.first.v == 4);
    REQUIRE(s.first.pos == 2);

    s = ukkonen::traverse(t, 4000_w);
    REQUIRE(s.first.v == 4);
    REQUIRE(s.first.pos == 4);

    s = ukkonen::traverse(t, 0_w);
    REQUIRE(s.first.v == 2);
    REQUIRE(s.first.pos == 1);

    s = ukkonen::traverse(t, 04_w);
    REQUIRE(s.first.v == 3);
    REQUIRE(s.first.pos == 1);

    s = ukkonen::traverse(t, 04000_w);
    REQUIRE(s.first.v == 3);
    REQUIRE(s.first.pos == 4);
    REQUIRE(s.first == State(3, 4));

    s = ukkonen::traverse(t, 002_w);
    REQUIRE(t.distance_from_root(t.nodes()[s.first.v]));
  }

  LIBSEMIGROUPS_TEST_CASE("Ukkonen", "006", "dot", "[quick][ukkonen]") {
    {
      Ukkonen t;
      ukkonen::add_word(t, 00_w);
      ukkonen::add_word(t, 00_w);
      ukkonen::add_word(t, 010_w);
      ukkonen::add_word(t, 00_w);
      ukkonen::add_word(t, 0101_w);
      ukkonen::add_word(t, 010_w);
      REQUIRE_NOTHROW(ukkonen::dot(t));
    }
    {
      Ukkonen u;
      // No words
      REQUIRE_THROWS_AS(ukkonen::dot(u), LibsemigroupsException);
      WordRange words;
      words.alphabet_size(2).min(0).max(5);
      ukkonen::add_words(u, rx::begin(words), rx::end(words));
      REQUIRE(u.number_of_distinct_words() == 30);
      // Too many words
      REQUIRE_THROWS_AS(ukkonen::dot(u), LibsemigroupsException);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("Ukkonen", "007", "strings", "[quick][ukkonen]") {
    Ukkonen t;
    ukkonen::add_word(t, "aaaeaa");
    ukkonen::add_word(t, "abcd");
    REQUIRE(t.number_of_distinct_words() == 2);
    ukkonen::add_word(t, "");
    REQUIRE(t.number_of_distinct_words() == 2);

    REQUIRE(t.nodes().size() == 15);
    REQUIRE(ukkonen::number_of_pieces(t, "aaaeaa") == POSITIVE_INFINITY);
    REQUIRE(ukkonen::length_maximal_piece_suffix(t, "aaaeaa") == 2);

    auto w = "aaaeaa";
    REQUIRE(std::string(ukkonen::maximal_piece_suffix(t, w), w + 6) == "aa");
    REQUIRE(std::string(ukkonen::maximal_piece_suffix_no_checks(t, w), w + 6)
            == "aa");
    REQUIRE(ukkonen::length_maximal_piece_suffix(t, w) == 2);
    REQUIRE(ukkonen::length_maximal_piece_suffix_no_checks(t, w) == 2);
    REQUIRE(std::string(w, ukkonen::maximal_piece_prefix_no_checks(t, w))
            == "aa");

    std::string ww = "aaaeaa";
    REQUIRE(std::string(ukkonen::maximal_piece_suffix(t, ww), ww.cend())
            == "aa");
    REQUIRE(
        std::string(ukkonen::maximal_piece_suffix_no_checks(t, ww), ww.cend())
        == "aa");

    REQUIRE(ukkonen::pieces(t, ww) == std::vector<std::string>());
    REQUIRE(ukkonen::pieces(t, "aaaaaa")
            == std::vector<std::string>({"aa", "aa", "aa"}));
    REQUIRE_THROWS_AS(ukkonen::pieces(t, word_type({UNDEFINED})),
                      LibsemigroupsException);
    REQUIRE(
        std::string(ww.cbegin(), ukkonen::maximal_piece_prefix_no_checks(t, ww))
        == "aa");
    REQUIRE(ukkonen::length_maximal_piece_suffix(t, ww) == 2);
    REQUIRE(ukkonen::length_maximal_piece_suffix_no_checks(t, ww) == 2);

    REQUIRE(ukkonen::length_maximal_piece_suffix(t, "abcd") == 0);
    REQUIRE(ukkonen::length_maximal_piece_prefix(t, "aaaeaa") == 2);
    REQUIRE(!ukkonen::is_suffix(t, "aaaeaaaaaaaaaaaaaaaa"));
    REQUIRE(ukkonen::is_suffix(t, ""));
    REQUIRE(ukkonen::length_maximal_piece_prefix(t, "") == 0);
    REQUIRE(ukkonen::length_maximal_piece_suffix(t, "") == 0);
    REQUIRE(ukkonen::number_of_pieces(t, "") == 0);
    REQUIRE(ukkonen::length_maximal_piece_prefix(t, "xxx") == 0);
    REQUIRE(ukkonen::length_maximal_piece_suffix(t, "xxx") == 0);
    REQUIRE(ukkonen::number_of_pieces(t, "xxx") == POSITIVE_INFINITY);
  }

  LIBSEMIGROUPS_TEST_CASE("Ukkonen", "008", "dfs #01", "[quick][ukkonen]") {
    using const_iterator = typename Ukkonen::const_iterator;
    Ukkonen t;
    ukkonen::add_word(t, 1212_w);
    ukkonen::add_word(t, 0_w);
    ukkonen::add_word(t, 1213121312131213_w);
    ukkonen::add_word(t, 0_w);
    const_iterator first, last;
    std::tie(first, last) = best_subword(t);
    REQUIRE(word_type(first, last) == word_type(1213_w));
  }

  LIBSEMIGROUPS_TEST_CASE("Ukkonen", "009", "dfs #02", "[quick][ukkonen]") {
    using const_iterator = typename Ukkonen::const_iterator;

    Ukkonen t;
    ukkonen::add_word(t, "aaaaa");
    ukkonen::add_word(t, "bbb");
    ukkonen::add_word(t, "ababa");
    ukkonen::add_word(t, "aaabaabaaabaa");
    const_iterator first, last;
    std::tie(first, last) = best_subword(t);
    REQUIRE(word_type(first, last) == word_type({97, 98, 97}));
  }

  LIBSEMIGROUPS_TEST_CASE("Ukkonen", "010", "dfs #03", "[quick][ukkonen]") {
    using const_iterator = typename Ukkonen::const_iterator;
    Ukkonen t;
    ukkonen::add_word(t, "aaaaa");
    ukkonen::add_word(t, "bbb");
    ukkonen::add_word(t, "cba");
    ukkonen::add_word(t, "aaccaca");
    ukkonen::add_word(t, "aba");
    const_iterator first, last;
    std::tie(first, last) = best_subword(t);
    REQUIRE(word_type(first, last) == word_type());
  }

  LIBSEMIGROUPS_TEST_CASE("Ukkonen", "011", "dfs #04", "[quick][ukkonen]") {
    using const_iterator = typename Ukkonen::const_iterator;
    Ukkonen t;
    ukkonen::add_word(t, 00_w);
    ukkonen::add_word(t, 10_w);
    ukkonen::add_word(t, 01_w);
    ukkonen::add_word(t, 20_w);
    ukkonen::add_word(t, 02_w);
    ukkonen::add_word(t, 30_w);
    ukkonen::add_word(t, 03_w);
    ukkonen::add_word(t, 11_w);
    ukkonen::add_word(t, 23_w);
    ukkonen::add_word(t, 222_w);
    ukkonen::add_word(t, 12121212121212_w);
    ukkonen::add_word(t, 12131213121312131213121312131213_w);
    const_iterator first, last;
    std::tie(first, last) = best_subword(t);
    REQUIRE(word_type(first, last) == 12131213_w);
  }

  LIBSEMIGROUPS_TEST_CASE("Ukkonen", "012", "dfs #05", "[quick][ukkonen]") {
    using const_iterator = typename Ukkonen::const_iterator;
    Ukkonen t;
    ukkonen::add_word(t, 00_w);
    ukkonen::add_word(t, 10_w);
    ukkonen::add_word(t, 01_w);
    ukkonen::add_word(t, 20_w);
    ukkonen::add_word(t, 02_w);
    ukkonen::add_word(t, 30_w);
    ukkonen::add_word(t, 03_w);
    ukkonen::add_word(t, 11_w);
    ukkonen::add_word(t, 23_w);
    ukkonen::add_word(t, 222_w);
    ukkonen::add_word(t, 12121212121212_w);
    ukkonen::add_word(t, 44444444_w);
    ukkonen::add_word(t, 1213_w);
    const_iterator first, last;
    std::tie(first, last) = best_subword(t);
    REQUIRE(word_type(first, last) == 12_w);
  }

  LIBSEMIGROUPS_TEST_CASE("Ukkonen", "013", "dfs #06", "[quick][ukkonen]") {
    using const_iterator = typename Ukkonen::const_iterator;
    Ukkonen t;
    ukkonen::add_word(t, 00_w);
    ukkonen::add_word(t, 10_w);
    ukkonen::add_word(t, 01_w);
    ukkonen::add_word(t, 20_w);
    ukkonen::add_word(t, 02_w);
    ukkonen::add_word(t, 30_w);
    ukkonen::add_word(t, 03_w);
    ukkonen::add_word(t, 11_w);
    ukkonen::add_word(t, 23_w);
    ukkonen::add_word(t, 222_w);
    ukkonen::add_word(t, 5555555_w);
    ukkonen::add_word(t, 44444444_w);
    ukkonen::add_word(t, 513_w);
    ukkonen::add_word(t, 12_w);
    const_iterator first, last;
    std::tie(first, last) = best_subword(t);
    REQUIRE(word_type(first, last) == 4444_w);
  }

  LIBSEMIGROUPS_TEST_CASE("Ukkonen", "014", "dfs #07", "[quick][ukkonen]") {
    using const_iterator = typename Ukkonen::const_iterator;
    Ukkonen t;
    ukkonen::add_word(t, "aaaaaaaaaaaaaa");
    ukkonen::add_word(t, "bbbbbbbbbbbbbb");
    ukkonen::add_word(t, "cccccccccccccc");
    ukkonen::add_word(t, "aaaaba");
    ukkonen::add_word(t, "bbb");
    ukkonen::add_word(t, "bbbbab");
    ukkonen::add_word(t, "aaa");
    ukkonen::add_word(t, "aaaaca");
    ukkonen::add_word(t, "ccc");
    ukkonen::add_word(t, "ccccac");
    ukkonen::add_word(t, "aaa");
    ukkonen::add_word(t, "bbbbcb");
    ukkonen::add_word(t, "ccc");
    ukkonen::add_word(t, "ccccbc");
    ukkonen::add_word(t, "bbb");
    const_iterator first, last;
    std::tie(first, last) = best_subword(t);
    REQUIRE(word_type(first, last) == word_type({99, 99, 99}));
  }

  LIBSEMIGROUPS_TEST_CASE("Ukkonen", "015", "dfs #08", "[quick][ukkonen]") {
    using const_iterator = typename Ukkonen::const_iterator;
    Ukkonen t;
    ukkonen::add_word(t, "aaaaaaaaaaaaaa");
    ukkonen::add_word(t, "bbbbbbbbbbbbbb");
    ukkonen::add_word(t, "ddddcc");
    ukkonen::add_word(t, "aaaaba");
    ukkonen::add_word(t, "bbb");
    ukkonen::add_word(t, "bbbbab");
    ukkonen::add_word(t, "aaa");
    ukkonen::add_word(t, "aaaaca");
    ukkonen::add_word(t, "dcac");
    ukkonen::add_word(t, "aaa");
    ukkonen::add_word(t, "bbbbcb");
    ukkonen::add_word(t, "dcbc");
    ukkonen::add_word(t, "bbb");
    ukkonen::add_word(t, "ccc");
    const_iterator first, last;
    std::tie(first, last) = best_subword(t);
    REQUIRE(word_type(first, last) == word_type({98, 98, 98}));
  }

  LIBSEMIGROUPS_TEST_CASE("Ukkonen", "016", "dfs #09", "[quick][ukkonen]") {
    using const_iterator = typename Ukkonen::const_iterator;
    Ukkonen t;
    ukkonen::add_word(t, "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
    ukkonen::add_word(t, "bbb");
    ukkonen::add_word(t, "ababa");
    ukkonen::add_word(t, "aaaaaaaaaaaaaaaabaaaabaaaaaaaaaaaaaaaabaaaa");
    const_iterator first, last;
    std::tie(first, last) = best_subword(t);
    REQUIRE(word_type(first, last)
            == word_type({
                97,
                97,
                97,
                97,
            }));
  }

  LIBSEMIGROUPS_TEST_CASE("Ukkonen", "017", "dfs #10", "[quick][ukkonen]") {
    using const_iterator = typename Ukkonen::const_iterator;
    Ukkonen t;
    ukkonen::add_word(t, "aBCbac");
    ukkonen::add_word(t, "bACbaacA");
    ukkonen::add_word(t, "accAABab");
    const_iterator first, last;
    std::tie(first, last) = best_subword(t);
    REQUIRE(word_type(first, last) == word_type());
  }

  LIBSEMIGROUPS_TEST_CASE("Ukkonen", "018", "pieces", "[quick][ukkonen]") {
    Ukkonen t;
    ToWord  string_to_word("ab");
    ukkonen::add_word(t, string_to_word("baabbaaaa"));
    ukkonen::add_word(t, string_to_word("abababbbaa"));

    REQUIRE(ukkonen::number_of_pieces(t, string_to_word("baabbaaaa")) == 3);
    REQUIRE(ukkonen::pieces(t, string_to_word("baabbaaaa"))
            == std::vector<word_type>({100_w, 1100_w, 00_w}));
    REQUIRE(ukkonen::is_piece(t, string_to_word("baa")));
    REQUIRE(ukkonen::is_piece(t, string_to_word("bbaa")));
    REQUIRE(ukkonen::is_piece(t, string_to_word("aa")));
    REQUIRE(!ukkonen::is_piece(t, "aa"));
    REQUIRE(!ukkonen::is_piece(t, std::string("aa")));
    REQUIRE(!ukkonen::is_piece_no_checks(t, "aa"));
    REQUIRE(!ukkonen::is_piece_no_checks(t, std::string("aa")));
    REQUIRE(ukkonen::is_piece_no_checks(t, string_to_word("baa")));
    REQUIRE(ukkonen::is_piece_no_checks(t, string_to_word("bbaa")));
    REQUIRE(ukkonen::is_piece_no_checks(t, string_to_word("aa")));

    REQUIRE(ukkonen::number_of_pieces(t, string_to_word("abababbbaa")) == 3);
    REQUIRE(ukkonen::pieces(t, string_to_word("abababbbaa"))
            == std::vector<word_type>({0101_w, 011_w, 100_w}));
  }

  LIBSEMIGROUPS_TEST_CASE("Ukkonen",
                          "019",
                          "code coverage",
                          "[quick][ukkonen]") {
    Ukkonen u;
    ukkonen::add_word_no_checks(u, 0001000_w);
    auto w = "abcdefabababab";
    ukkonen::add_word_no_checks(u, w, w + std::strlen(w));
    ukkonen::add_word_no_checks(u, w);
    ukkonen::add_word_no_checks(u, {1, 2, 3, 4, 0, 0, 1, 1, 0, 0, 1});
    std::string ww("abdbadbabdbabdabdj");
    ukkonen::add_word_no_checks(u, ww);
    ukkonen::add_word(u, ww);

    auto www = 0001000_w;
    ukkonen::add_word_no_checks(u, www.cbegin(), www.cend());

    REQUIRE(u.nodes().size() == 78);
    REQUIRE(u.length_of_distinct_words() == 50);
    REQUIRE(u.length_of_words() == 89);
    REQUIRE(std::equal(u.cbegin(), u.cend(), u.begin(), u.end()));
    REQUIRE(word_type(u.begin(), u.end())
            == word_type({0,
                          0,
                          0,
                          1,
                          0,
                          0,
                          0,
                          u.unique_letter(0),
                          97,
                          98,
                          99,
                          100,
                          101,
                          102,
                          97,
                          98,
                          97,
                          98,
                          97,
                          98,
                          97,
                          98,
                          u.unique_letter(1),
                          1,
                          2,
                          3,
                          4,
                          0,
                          0,
                          1,
                          1,
                          0,
                          0,
                          1,
                          u.unique_letter(2),
                          97,
                          98,
                          100,
                          98,
                          97,
                          100,
                          98,
                          97,
                          98,
                          100,
                          98,
                          97,
                          98,
                          100,
                          97,
                          98,
                          100,
                          106,
                          u.unique_letter(3)}));
    std::vector<size_t> distances(u.nodes().size(), 0);
    std::transform(u.nodes().cbegin(),
                   u.nodes().cend(),
                   distances.begin(),
                   [&u](auto const& n) { return u.distance_from_root(n); });
    REQUIRE(distances
            == std::vector<size_t>(
                {0,  8,  2,  7,  1,  6, 5,  3, 4,  3, 2,  1,  15, 14, 13, 12,
                 11, 10, 2,  9,  1,  8, 6,  7, 5,  6, 4,  5,  3,  4,  3,  2,
                 1,  1,  12, 11, 10, 9, 3,  8, 2,  7, 6,  3,  5,  4,  3,  2,
                 1,  19, 18, 1,  17, 2, 16, 1, 15, 3, 14, 13, 5,  12, 4,  11,
                 5,  10, 4,  9,  3,  8, 2,  7, 6,  5, 4,  3,  2,  1}));

    distances.resize(u.number_of_distinct_words(), 0);
    auto range = detail::IntRange<size_t>(0, u.number_of_distinct_words());
    std::transform(range.cbegin(),
                   range.cend(),
                   distances.begin(),
                   [&u](auto i) { return u.multiplicity(i); });
    REQUIRE(distances == std::vector<size_t>({2, 2, 1, 2}));

    std::vector<word_type> v = {www, www};
    ukkonen::add_words_no_checks(u, v);
    ukkonen::add_words(u, v);
    ukkonen::add_words_no_checks(u, v.cbegin(), v.cend());
    ukkonen::add_words(u, v.cbegin(), v.cend());
    REQUIRE(u.nodes().size() == 78);
  }

  LIBSEMIGROUPS_TEST_CASE("Ukkonen",
                          "020",
                          "code coverage",
                          "[quick][ukkonen]") {
    Ukkonen u;
    REQUIRE(u.is_suffix(Ukkonen::State()) == UNDEFINED);
    REQUIRE(ukkonen::is_suffix(u, ""_w) == true);
  }
}  // namespace libsemigroups
