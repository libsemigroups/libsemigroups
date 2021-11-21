// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2021 James D. Mitchell + Maria Tsalakou
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
#include <fstream>    // for ofstream
#include <string>     // for basic_string, operator==, operator!=, operator+
#include <vector>     // for vector

#include "catch.hpp"      // for REQUIRE, REQUIRE_THROWS_AS
#include "test-main.hpp"  // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/constants.hpp"    // for operator==, POSITIVE_INFINITY
#include "libsemigroups/suffix-tree.hpp"  // for SuffixTree, SuffixTree::State
#include "libsemigroups/types.hpp"        // for word_type
#include "libsemigroups/wislo.hpp"        // for const_wislo_iterator, cbegi...

namespace libsemigroups {
  struct LibsemigroupsException;  // forward decl

  using detail::SuffixTree;

  LIBSEMIGROUPS_TEST_CASE("SuffixTree",
                          "000",
                          "basic tests",
                          "[quick][suffix-tree]") {
    SuffixTree t;
    // aaeaaa$
    // abcd$'
    // a -> 0
    // b -> 1
    // c -> 2
    // d -> 3
    // e -> 4
    t.add_word({0, 0, 4, 0, 0, 0});
    REQUIRE(t.number_of_nodes() == 10);

    REQUIRE(t.is_subword({0, 0, 4, 0, 0, 0}));
    REQUIRE(t.is_subword({0, 4}));
    REQUIRE(!t.is_subword({4, 4}));
    REQUIRE(t.is_subword({}));
    REQUIRE(t.is_subword({0}));
    REQUIRE(t.is_subword({0, 0}));
    REQUIRE(t.is_subword({0, 0, 0}));
    REQUIRE(t.is_subword({0, 0, 0}));
    REQUIRE(!t.is_subword({0, 0, 0, 0}));
    REQUIRE(!t.is_subword({1}));
    REQUIRE(t.number_of_subwords() == 16);
    REQUIRE(cbegin_wislo(5, {}, {0, 0, 0, 0, 0, 0, 0})->empty());
    REQUIRE(std::count_if(cbegin_wislo(6, {}, {0, 0, 0, 0, 0, 0, 0, 0}),
                          cend_wislo(6, {}, {0, 0, 0, 0, 0, 0, 0, 0}),
                          [&t](word_type const& w) { return t.is_subword(w); })
            == 16);

    REQUIRE(t.is_subword({}));  // 1
    REQUIRE(t.is_subword({0, 0, 4, 0, 0, 0}));
    REQUIRE(t.is_subword({0, 0, 4, 0, 0}));
    REQUIRE(t.is_subword({0, 0, 4, 0}));
    REQUIRE(t.is_subword({0, 0, 4}));  // 5
    REQUIRE(t.is_subword({0, 0}));
    REQUIRE(t.is_subword({0}));
    REQUIRE(t.is_subword({0, 4, 0, 0, 0}));
    REQUIRE(t.is_subword({0, 4, 0, 0}));
    REQUIRE(t.is_subword({0, 4, 0}));  // 10
    REQUIRE(t.is_subword({0, 4}));
    REQUIRE(t.is_subword({4, 0, 0, 0}));
    REQUIRE(t.is_subword({4, 0, 0}));
    REQUIRE(t.is_subword({4, 0}));
    REQUIRE(t.is_subword({4}));        // 15
    REQUIRE(t.is_subword({0, 0, 0}));  // 16

    t.add_word({0, 1, 2, 3});
    REQUIRE(t.number_of_nodes() == 15);

    REQUIRE(t.is_subword({}));  // 1
    REQUIRE(t.is_subword({0, 0, 4, 0, 0, 0}));
    REQUIRE(t.is_subword({0, 0, 4, 0, 0}));
    REQUIRE(t.is_subword({0, 0, 4, 0}));
    REQUIRE(t.is_subword({0, 0, 4}));  // 5
    REQUIRE(t.is_subword({0, 0}));
    REQUIRE(t.is_subword({0}));
    REQUIRE(t.is_subword({0, 4, 0, 0, 0}));
    REQUIRE(t.is_subword({0, 4, 0, 0}));
    REQUIRE(t.is_subword({0, 4, 0}));  // 10
    REQUIRE(t.is_subword({0, 4}));
    REQUIRE(t.is_subword({4, 0, 0, 0}));
    REQUIRE(t.is_subword({4, 0, 0}));
    REQUIRE(t.is_subword({4, 0}));
    REQUIRE(t.is_subword({4}));        // 15
    REQUIRE(t.is_subword({0, 0, 0}));  // 16

    REQUIRE(t.is_subword({0, 1}));
    REQUIRE(t.is_subword({0, 1, 2}));
    REQUIRE(t.is_subword({0, 1, 2, 3}));
    REQUIRE(t.is_subword({1}));
    REQUIRE(t.is_subword({1, 2}));
    REQUIRE(t.is_subword({1, 2, 3}));
    REQUIRE(t.is_subword({2}));
    REQUIRE(t.is_subword({2, 3}));
    REQUIRE(t.is_subword({3}));

    REQUIRE(!t.is_subword({3, 3}));
    REQUIRE(t.number_of_subwords() == 25);

    REQUIRE(!t.is_suffix({1, 2, 3, 5}));
    REQUIRE(t.is_suffix({1, 2, 3}));

    REQUIRE(t.is_suffix({}));
    REQUIRE(t.is_suffix({0, 0, 4, 0, 0, 0}));
    REQUIRE(t.is_suffix({0, 4, 0, 0, 0}));
    REQUIRE(t.is_suffix({4, 0, 0, 0}));
    REQUIRE(t.is_suffix({0, 0, 0}));
    REQUIRE(t.is_suffix({0, 0}));
    REQUIRE(t.is_suffix({0}));
    REQUIRE(t.is_suffix({0, 1, 2, 3}));
    REQUIRE(t.is_suffix({1, 2, 3}));
    REQUIRE(t.is_suffix({2, 3}));
    REQUIRE(t.is_suffix({3}));

    REQUIRE(std::count_if(cbegin_wislo(5, {}, {0, 0, 0, 0, 0, 0, 0}),
                          cend_wislo(5, {}, {0, 0, 0, 0, 0, 0, 0}),
                          [&t](word_type const& w) { return t.is_suffix(w); })
            == 11);

    REQUIRE(t.maximal_piece_prefix({0, 0, 4, 0, 0, 0}) == 2);
    REQUIRE(t.maximal_piece_prefix({0, 1, 2, 3}) == 1);
  }

  LIBSEMIGROUPS_TEST_CASE("SuffixTree",
                          "001",
                          "maximal_piece_prefix 1",
                          "[quick][suffix-tree]") {
    SuffixTree t;
    t.add_word({0, 5, 7});
    t.add_word({1, 6, 7});
    t.add_word({7, 2});
    t.add_word({3, 4});
    t.add_word({4, 8});
    t.add_word({9});
    t.add_word({5, 7, 10});
    t.add_word({6, 7, 11});

    REQUIRE(t.number_of_nodes() == 32);
    REQUIRE(t.maximal_piece_prefix({0, 5, 7}) == 0);
    REQUIRE(t.maximal_piece_prefix({1, 6, 7}) == 0);
    REQUIRE(t.maximal_piece_prefix({7, 2}) == 1);
    REQUIRE(t.maximal_piece_prefix({3, 4}) == 0);
    REQUIRE(t.maximal_piece_prefix({4, 8}) == 1);
    REQUIRE(t.maximal_piece_prefix(word_type({9})) == 0);
    REQUIRE(t.maximal_piece_prefix({5, 7, 10}) == 2);
    REQUIRE(t.maximal_piece_prefix({6, 7, 11}) == 2);
  }

  LIBSEMIGROUPS_TEST_CASE("SuffixTree",
                          "002",
                          "maximal_piece_prefix 2",
                          "[quick][suffix-tree]") {
    SuffixTree t;
    t.add_word({0, 0, 4, 0, 0, 0});
    t.add_word({4, 5});

    REQUIRE(t.number_of_subwords() == 18);
    REQUIRE(t.maximal_piece_prefix({0, 0, 4, 0, 0, 0}) == 2);
    REQUIRE(t.maximal_piece_prefix({4, 5}) == 1);

    t.add_word({0, 1, 2, 3});
    REQUIRE(t.number_of_subwords() == 27);
    REQUIRE(t.maximal_piece_prefix({0, 0, 4, 0, 0, 0}) == 2);
    REQUIRE(t.maximal_piece_prefix({4, 5}) == 1);
    REQUIRE(t.maximal_piece_prefix({0, 1, 2, 3}) == 1);

    t.add_word({0, 0, 4});
    REQUIRE(t.number_of_subwords() == 27);
    REQUIRE(t.maximal_piece_prefix({0, 0, 4, 0, 0, 0}) == 3);
    REQUIRE(t.maximal_piece_prefix({4, 5}) == 1);
    REQUIRE(t.maximal_piece_prefix({0, 1, 2, 3}) == 1);
    REQUIRE(t.maximal_piece_prefix({0, 0, 4}) == 3);
  }

  LIBSEMIGROUPS_TEST_CASE("SuffixTree",
                          "003",
                          "maximal_piece_prefix 3",
                          "[quick][suffix-tree]") {
    SuffixTree t;
    REQUIRE(t.number_of_words() == 0);
    t.add_word({0, 1, 2});
    REQUIRE(t.number_of_words() == 1);
    t.add_word({1, 2, 4});
    REQUIRE(t.number_of_words() == 2);

    REQUIRE(t.number_of_nodes() == 11);
    REQUIRE(t.number_of_subwords() == 10);

    REQUIRE(t.is_subword({}));
    REQUIRE(t.is_subword({0}));
    REQUIRE(t.is_subword({1}));
    REQUIRE(t.is_subword({2}));
    REQUIRE(t.is_subword({4}));
    REQUIRE(t.is_subword({0, 1}));
    REQUIRE(t.is_subword({1, 2}));
    REQUIRE(t.is_subword({2, 4}));
    REQUIRE(t.is_subword({0, 1, 2}));
    REQUIRE(t.is_subword({1, 2, 4}));
    REQUIRE_THROWS_AS(t.is_subword({0, 1, 2, static_cast<size_t>(-1), 1}),
                      LibsemigroupsException);

    REQUIRE(!t.is_subword({1, 2, 3}));
    REQUIRE(!t.is_subword({1, 2, 3, 4}));
    REQUIRE(!t.is_subword({3}));
    REQUIRE(!t.is_subword({1, 3}));

    REQUIRE(std::count_if(cbegin_wislo(5, {}, {0, 0, 0, 0, 0}),
                          cend_wislo(5, {}, {0, 0, 0, 0, 0}),
                          [&t](word_type const& w) { return t.is_subword(w); })
            == 10);

    REQUIRE(t.is_suffix({}));
    REQUIRE(!t.is_suffix({0}));
    REQUIRE(!t.is_suffix({1}));
    REQUIRE(t.is_suffix({2}));
    REQUIRE(t.is_suffix({4}));
    REQUIRE(!t.is_suffix({0, 1}));
    REQUIRE(t.is_suffix({1, 2}));
    REQUIRE(t.is_suffix({2, 4}));
    REQUIRE(t.is_suffix({0, 1, 2}));
    REQUIRE(t.is_suffix({1, 2, 4}));
    REQUIRE_THROWS_AS(t.is_suffix({static_cast<size_t>(-1), 1, 2, 4}),
                      LibsemigroupsException);

    REQUIRE(t.maximal_piece_prefix({0, 1, 2}) == 0);
    REQUIRE(t.maximal_piece_prefix({1, 2, 4}) == 2);
    REQUIRE(t.maximal_piece_suffix({0, 1, 2}) == 2);
    REQUIRE(t.maximal_piece_suffix({1, 2, 4}) == 0);

    REQUIRE(t.number_of_pieces({0, 1, 2}) == POSITIVE_INFINITY);
    REQUIRE(t.number_of_pieces({1, 2, 4}) == POSITIVE_INFINITY);
  }

  LIBSEMIGROUPS_TEST_CASE("SuffixTree",
                          "004",
                          "number_of_pieces",
                          "[quick][suffix-tree]") {
    SuffixTree t;
    t.add_word({0, 1, 2});
    t.add_word({0});
    t.add_word({1});
    t.add_word({2});

    REQUIRE(t.number_of_pieces({0, 1, 2}) == 3);
    REQUIRE(t.number_of_pieces(word_type({0})) == 1);
    REQUIRE(t.number_of_pieces(word_type({1})) == 1);
    REQUIRE(t.number_of_pieces(word_type({2})) == 1);

    t.add_word({0, 1, 2, 8, 4, 5, 6, 7});
    t.add_word({0, 1, 2});  // does nothing
    t.add_word({8, 4, 5});
    t.add_word({5, 6});
    t.add_word({5, 6, 7});

    REQUIRE(t.number_of_words() == 8);

    REQUIRE(t.number_of_pieces({0, 1, 2}) == 1);
    REQUIRE(t.number_of_pieces(word_type({0})) == 1);
    REQUIRE(t.number_of_pieces(word_type({1})) == 1);
    REQUIRE(t.number_of_pieces(word_type({2})) == 1);

    REQUIRE(t.number_of_pieces({0, 1, 2, 8, 4, 5, 6, 7}) == 3);
    REQUIRE(t.number_of_pieces({8, 4, 5}) == 1);
    REQUIRE(t.number_of_pieces({5, 6}) == 1);
    REQUIRE(t.number_of_pieces({5, 6, 7}) == 1);
  }

  LIBSEMIGROUPS_TEST_CASE("SuffixTree",
                          "005",
                          "traverse",
                          "[quick][suffix-tree]") {
    using State = SuffixTree::State;
    SuffixTree t;
    t.add_word({0, 0, 4, 0, 0, 0});

    auto s = t.traverse(State(0, 0), {});
    REQUIRE(s.v == 0);
    REQUIRE(s.pos == 0);

    s = t.traverse(State(0, 0), {4});
    REQUIRE(s.v == 4);
    REQUIRE(s.pos == 1);

    s = t.traverse(State(0, 0), {4, 0});
    REQUIRE(s.v == 4);
    REQUIRE(s.pos == 2);

    s = t.traverse(s, {0, 0});
    REQUIRE(s.v == 4);
    REQUIRE(s.pos == 4);

    s = t.traverse(State(0, 0), {0});
    REQUIRE(s.v == 2);
    REQUIRE(s.pos == 1);

    s = t.traverse(s, {4});
    REQUIRE(s.v == 3);
    REQUIRE(s.pos == 1);

    s = t.traverse(s, {0, 0, 0});
    REQUIRE(s.v == 3);
    REQUIRE(s.pos == 4);
    REQUIRE(s == State(3, 4));

    s = t.traverse(State(0, 0), {0, 0, 2});
    REQUIRE(!s.valid());
  }

  LIBSEMIGROUPS_TEST_CASE("SuffixTree", "006", "tikz", "[quick][suffix-tree]") {
    {
      SuffixTree t;
      t.add_word(std::string("aaeaaa"));
      t.add_word(std::string("abcd"));
      REQUIRE_NOTHROW(t.tikz());
      // std::cout << t.tikz() << std::endl;
    }
    {
      SuffixTree t;
      t.add_word(std::string("aaea"));
      t.add_word(std::string("abcd"));
      REQUIRE_NOTHROW(t.tikz());
      // std::cout << t.tikz() << std::endl;
    }
    {
      SuffixTree t;
      t.add_word(std::string("abcd"));
      t.add_word(std::string("aaaeaa"));
      t.add_word(std::string("ef"));
      t.add_word(std::string("dg"));
      // std::cout << t.tikz() << std::endl;
      REQUIRE_NOTHROW(t.tikz());
    }
    {
      SuffixTree t;
      t.add_word(std::string("anagnostopoulou"));
      // std::cout << t.tikz() << std::endl;
      REQUIRE_NOTHROW(t.tikz());
    }
  }

  LIBSEMIGROUPS_TEST_CASE("SuffixTree", "008", "dot", "[quick][suffix-tree]") {
    {
      SuffixTree t;
      t.add_word({0, 0});
      t.add_word({0, 0});
      t.add_word({0, 1, 0});
      t.add_word({0, 0});
      t.add_word({0, 1, 0, 1});
      t.add_word({0, 1, 0});
      REQUIRE_NOTHROW(t.dot());
      // std::ofstream file;
      // file.open("tmp.dot");
      // file << t.dot();
      // file.close();
    }
  }

  LIBSEMIGROUPS_TEST_CASE("SuffixTree",
                          "007",
                          "strings",
                          "[quick][suffix-tree]") {
    SuffixTree t;
    t.add_word(std::string("aaaeaa"));
    t.add_word(std::string("abcd"));
    REQUIRE(t.number_of_words() == 2);
    t.add_word(std::string(""));
    REQUIRE(t.number_of_words() == 2);

    REQUIRE(t.number_of_nodes() == 15);
    REQUIRE(t.number_of_pieces(std::string("aaaeaa")) == POSITIVE_INFINITY);
    REQUIRE(t.maximal_piece_suffix(std::string("aaaeaa")) == 2);
    REQUIRE(t.maximal_piece_suffix(std::string("abcd")) == 0);
    REQUIRE(t.maximal_piece_prefix(std::string("aaaeaa")) == 2);
    REQUIRE(!t.is_suffix(std::string("aaaeaaaaaaaaaaaaaaaa")));
    REQUIRE(t.is_suffix(std::string("")));
    REQUIRE_THROWS_AS(t.maximal_piece_prefix(std::string("")),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(t.maximal_piece_suffix(std::string("")),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(t.number_of_pieces(std::string("")),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(t.maximal_piece_prefix(std::string("xxx")),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(t.maximal_piece_suffix(std::string("xxx")),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(t.number_of_pieces(std::string("xxx")),
                      LibsemigroupsException);
  }

  namespace {
    auto best_subword(SuffixTree& t) {
      detail::DFSHelper helper(t);
      return t.dfs(helper);
    }
  }  // namespace

  LIBSEMIGROUPS_TEST_CASE("SuffixTree",
                          "009",
                          "dfs #01",
                          "[quick][suffix-tree]") {
    using const_iterator = typename SuffixTree::const_iterator;
    SuffixTree t;
    t.add_word({1, 2, 1, 2});
    t.add_word({0});
    t.add_word({1, 2, 1, 3, 1, 2, 1, 3, 1, 2, 1, 3, 1, 2, 1, 3});
    t.add_word({0});
    const_iterator first, last;
    std::tie(first, last) = best_subword(t);
    REQUIRE(word_type(first, last) == word_type({1, 2, 1, 3}));
  }

  LIBSEMIGROUPS_TEST_CASE("SuffixTree",
                          "010",
                          "dfs #02",
                          "[quick][suffix-tree]") {
    using const_iterator = typename SuffixTree::const_iterator;

    SuffixTree t;
    t.add_word(std::string("aaaaa"));
    t.add_word(std::string("bbb"));
    t.add_word(std::string("ababa"));
    t.add_word(std::string("aaabaabaaabaa"));
    const_iterator first, last;
    std::tie(first, last) = best_subword(t);
    REQUIRE(word_type(first, last) == word_type({97, 98, 97}));
  }

  LIBSEMIGROUPS_TEST_CASE("SuffixTree",
                          "011",
                          "dfs #03",
                          "[quick][suffix-tree]") {
    using const_iterator = typename SuffixTree::const_iterator;
    SuffixTree t;
    t.add_word(std::string("aaaaa"));
    t.add_word(std::string("bbb"));
    t.add_word(std::string("cba"));
    t.add_word(std::string("aaccaca"));
    t.add_word(std::string("aba"));
    const_iterator first, last;
    std::tie(first, last) = best_subword(t);
    REQUIRE(word_type(first, last) == word_type());
  }

  LIBSEMIGROUPS_TEST_CASE("SuffixTree",
                          "012",
                          "dfs #04",
                          "[quick][suffix-tree]") {
    using const_iterator = typename SuffixTree::const_iterator;
    SuffixTree t;
    t.add_word({0, 0});
    t.add_word({1, 0});
    t.add_word({0, 1});
    t.add_word({2, 0});
    t.add_word({0, 2});
    t.add_word({3, 0});
    t.add_word({0, 3});
    t.add_word({1, 1});
    t.add_word({2, 3});
    t.add_word({2, 2, 2});
    t.add_word({1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2});
    t.add_word({1, 2, 1, 3, 1, 2, 1, 3, 1, 2, 1, 3, 1, 2, 1, 3,
                1, 2, 1, 3, 1, 2, 1, 3, 1, 2, 1, 3, 1, 2, 1, 3});
    const_iterator first, last;
    std::tie(first, last) = best_subword(t);
    REQUIRE(word_type(first, last) == word_type({1, 2, 1, 3}));
  }

  LIBSEMIGROUPS_TEST_CASE("SuffixTree",
                          "013",
                          "dfs #05",
                          "[quick][suffix-tree]") {
    using const_iterator = typename SuffixTree::const_iterator;
    SuffixTree t;
    t.add_word({0, 0});
    t.add_word({1, 0});
    t.add_word({0, 1});
    t.add_word({2, 0});
    t.add_word({0, 2});
    t.add_word({3, 0});
    t.add_word({0, 3});
    t.add_word({1, 1});
    t.add_word({2, 3});
    t.add_word({2, 2, 2});
    t.add_word({1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2});
    t.add_word({4, 4, 4, 4, 4, 4, 4, 4});
    t.add_word({1, 2, 1, 3});
    const_iterator first, last;
    std::tie(first, last) = best_subword(t);
    REQUIRE(word_type(first, last) == word_type({1, 2}));
  }

  LIBSEMIGROUPS_TEST_CASE("SuffixTree",
                          "014",
                          "dfs #06",
                          "[quick][suffix-tree]") {
    using const_iterator = typename SuffixTree::const_iterator;
    SuffixTree t;
    t.add_word({0, 0});
    t.add_word({1, 0});
    t.add_word({0, 1});
    t.add_word({2, 0});
    t.add_word({0, 2});
    t.add_word({3, 0});
    t.add_word({0, 3});
    t.add_word({1, 1});
    t.add_word({2, 3});
    t.add_word({2, 2, 2});
    t.add_word({5, 5, 5, 5, 5, 5, 5});
    t.add_word({4, 4, 4, 4, 4, 4, 4, 4});
    t.add_word({5, 1, 3});
    t.add_word({1, 2});
    const_iterator first, last;
    std::tie(first, last) = best_subword(t);
    REQUIRE(word_type(first, last) == word_type());
  }

  LIBSEMIGROUPS_TEST_CASE("SuffixTree",
                          "015",
                          "dfs #07",
                          "[quick][suffix-tree]") {
    using const_iterator = typename SuffixTree::const_iterator;
    SuffixTree t;
    t.add_word(std::string("aaaaaaaaaaaaaa"));
    t.add_word(std::string("bbbbbbbbbbbbbb"));
    t.add_word(std::string("cccccccccccccc"));
    t.add_word(std::string("aaaaba"));
    t.add_word(std::string("bbb"));
    t.add_word(std::string("bbbbab"));
    t.add_word(std::string("aaa"));
    t.add_word(std::string("aaaaca"));
    t.add_word(std::string("ccc"));
    t.add_word(std::string("ccccac"));
    t.add_word(std::string("aaa"));
    t.add_word(std::string("bbbbcb"));
    t.add_word(std::string("ccc"));
    t.add_word(std::string("ccccbc"));
    t.add_word(std::string("bbb"));
    const_iterator first, last;
    std::tie(first, last) = best_subword(t);
    REQUIRE(word_type(first, last) == word_type({99, 99, 99}));
  }

  LIBSEMIGROUPS_TEST_CASE("SuffixTree",
                          "016",
                          "dfs #08",
                          "[quick][suffix-tree]") {
    using const_iterator = typename SuffixTree::const_iterator;
    SuffixTree t;
    t.add_word(std::string("aaaaaaaaaaaaaa"));
    t.add_word(std::string("bbbbbbbbbbbbbb"));
    t.add_word(std::string("ddddcc"));
    t.add_word(std::string("aaaaba"));
    t.add_word(std::string("bbb"));
    t.add_word(std::string("bbbbab"));
    t.add_word(std::string("aaa"));
    t.add_word(std::string("aaaaca"));
    t.add_word(std::string("dcac"));
    t.add_word(std::string("aaa"));
    t.add_word(std::string("bbbbcb"));
    t.add_word(std::string("dcbc"));
    t.add_word(std::string("bbb"));
    t.add_word(std::string("ccc"));
    const_iterator first, last;
    std::tie(first, last) = best_subword(t);
    REQUIRE(word_type(first, last) == word_type({98, 98, 98}));
  }

  LIBSEMIGROUPS_TEST_CASE("SuffixTree",
                          "017",
                          "dfs #09",
                          "[quick][suffix-tree]") {
    using const_iterator = typename SuffixTree::const_iterator;
    SuffixTree t;
    t.add_word(std::string("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
    t.add_word(std::string("bbb"));
    t.add_word(std::string("ababa"));
    t.add_word(std::string("aaaaaaaaaaaaaaaabaaaabaaaaaaaaaaaaaaaabaaaa"));
    const_iterator first, last;
    std::tie(first, last) = best_subword(t);
    REQUIRE(
        word_type(first, last)
        == word_type(
            {97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97}));
  }

  LIBSEMIGROUPS_TEST_CASE("SuffixTree",
                          "018",
                          "dfs #10",
                          "[quick][suffix-tree]") {
    using const_iterator = typename SuffixTree::const_iterator;
    SuffixTree t;
    t.add_word(std::string("aBCbac"));
    t.add_word(std::string("bACbaacA"));
    t.add_word(std::string("accAABab"));
    const_iterator first, last;
    std::tie(first, last) = best_subword(t);
    REQUIRE(word_type(first, last) == word_type());
  }
}  // namespace libsemigroups
