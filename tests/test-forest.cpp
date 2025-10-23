// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019-2025 Finn Smith
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

#include <algorithm>  // for reverse
#include <cstddef>    // for size_t
#include <string>     // for basic_string, operator==
#include <tuple>      // for get
#include <utility>    // for move
#include <vector>     // for vector, operator==

#include "Catch2-3.8.0/catch_amalgamated.hpp"  // for SourceLineInfo, operat...
#include "test-main.hpp"                       // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/constants.hpp"           // for UNDEFINED, operator==
#include "libsemigroups/exception.hpp"           // for LibsemigroupsException
#include "libsemigroups/forest.hpp"              // for Forest, to_human_re...
#include "libsemigroups/ranges.hpp"              // for zip
#include "libsemigroups/types.hpp"               // for word_type
#include "libsemigroups/word-graph-helpers.hpp"  // for spanning_tree
#include "libsemigroups/word-graph.hpp"          // for WordGraph::target, ...
#include "libsemigroups/word-range.hpp"          // for operator""_w

#include "libsemigroups/detail/iterator.hpp"  // for operator+

#include "word-graph-test-common.hpp"  // for binary_tree

namespace libsemigroups {
  using literals::operator""_w;

  struct LibsemigroupsException;

  namespace {
    Forest test_forest1() {
      Forest f(100);
      REQUIRE(f.number_of_nodes() == 100);
      for (size_t i = 1; i < 100; ++i) {
        f.set_parent_and_label(i, i - 1, i * i % 7);
      }
      return f;
    }
  };  // namespace

  LIBSEMIGROUPS_TEST_CASE("Forest", "000", "test forest", "[quick]") {
    Forest f = test_forest1();
    REQUIRE_THROWS_AS(f.set_parent_and_label(0, -1, 0), LibsemigroupsException);

    REQUIRE(
        f.parents()
        == std::vector<Forest::node_type>(
            {UNDEFINED, 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13,
             14,        15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28,
             29,        30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43,
             44,        45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58,
             59,        60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73,
             74,        75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88,
             89,        90, 91, 92, 93, 94, 95, 96, 97, 98}));
    REQUIRE(f.labels()
            == std::vector<Forest::node_type>(
                {UNDEFINED, 1, 4, 2, 2, 4, 1, 0, 1, 4, 2, 2, 4, 1, 0, 1, 4,
                 2,         2, 4, 1, 0, 1, 4, 2, 2, 4, 1, 0, 1, 4, 2, 2, 4,
                 1,         0, 1, 4, 2, 2, 4, 1, 0, 1, 4, 2, 2, 4, 1, 0, 1,
                 4,         2, 2, 4, 1, 0, 1, 4, 2, 2, 4, 1, 0, 1, 4, 2, 2,
                 4,         1, 0, 1, 4, 2, 2, 4, 1, 0, 1, 4, 2, 2, 4, 1, 0,
                 1,         4, 2, 2, 4, 1, 0, 1, 4, 2, 2, 4, 1, 0, 1}));
    REQUIRE(to_human_readable_repr(f)
            == "<Forest with 100 nodes, 100 edges, and 1 root>");
    REQUIRE_NOTHROW(f.init());
    REQUIRE(f.number_of_nodes() == 0);
    REQUIRE_NOTHROW(f.add_nodes(10));
    REQUIRE(f.number_of_nodes() == 10);
  }

  LIBSEMIGROUPS_TEST_CASE("Forest", "001", "path_to_root", "[quick]") {
    using literals::operator""_w;

    Forest f = test_forest1();

    REQUIRE(forest::path_to_root_no_checks(f, 50)
            == 10142241014224101422410142241014224101422410142241_w);
    REQUIRE(forest::path_to_root(f, 50)
            == 10142241014224101422410142241014224101422410142241_w);
    REQUIRE_THROWS_AS(forest::path_to_root(f, 1'000), LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("Forest", "002", "path_from_root", "[quick]") {
    using literals::operator""_w;

    Forest f = test_forest1();

    REQUIRE(forest::path_from_root_no_checks(f, 50)
            == 14224101422410142241014224101422410142241014224101_w);
    REQUIRE(forest::path_from_root(f, 50)
            == 14224101422410142241014224101422410142241014224101_w);
    for (size_t n = 0; n < f.number_of_nodes(); ++n) {
      auto p = forest::path_from_root(f, n);
      std::reverse(p.begin(), p.end());
      REQUIRE(p == forest::path_to_root(f, n));
    }
    REQUIRE_THROWS_AS(forest::path_from_root(f, 1'000), LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("Forest", "003", "depth", "[quick]") {
    Forest f = test_forest1();

    REQUIRE(forest::depth_no_checks(f, 50) == 50);
    REQUIRE_THROWS_AS(forest::depth(f, f.number_of_nodes()),
                      LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("Forest", "004", "cbegin_path_to_root", "[quick]") {
    using literals::operator""_w;

    Forest f = test_forest1();

    REQUIRE(word_type(f.cbegin_path_to_root_no_checks(50),
                      f.cend_path_to_root_no_checks(50))
            == 10142241014224101422410142241014224101422410142241_w);
    REQUIRE(f.parent(50) == 49);
    REQUIRE(f.label(50) == 1);

    REQUIRE_THROWS_AS(f.parent(101), LibsemigroupsException);
    REQUIRE_THROWS_AS(f.label(101), LibsemigroupsException);

    auto first = f.cbegin_path_to_root(10), last = f.cend_path_to_root(10);
    REQUIRE(first != last);

    Forest copy(f);
    last = copy.cend_path_to_root(10);
    REQUIRE(first != last);

    last = f.cbegin_path_to_root(10);
    ++last;
    ++first;
    REQUIRE(first == last);
  }

  LIBSEMIGROUPS_TEST_CASE("Forest", "005", "constructors", "[quick]") {
    Forest f = test_forest1();
    REQUIRE(!f.empty());

    Forest copy(f);
    REQUIRE(copy == f);
    REQUIRE(copy.parents() == f.parents());
    REQUIRE(copy.labels() == f.labels());
    REQUIRE(to_human_readable_repr(copy) == to_human_readable_repr(f));

    Forest move(std::move(copy));
    REQUIRE(move == f);
    REQUIRE(move.parents() == f.parents());
    REQUIRE(move.labels() == f.labels());
    REQUIRE(to_human_readable_repr(move) == to_human_readable_repr(f));

    Forest copy_ass;
    REQUIRE(copy_ass != f);
    copy_ass = f;
    REQUIRE(copy_ass == f);
    REQUIRE(copy_ass.parents() == f.parents());
    REQUIRE(copy_ass.labels() == f.labels());
    REQUIRE(to_human_readable_repr(copy_ass) == to_human_readable_repr(f));

    Forest move_ass;
    REQUIRE(move_ass != f);
    move_ass = std::move(copy_ass);
    REQUIRE(move_ass == f);
    REQUIRE(move_ass.parents() == f.parents());
    REQUIRE(move_ass.labels() == f.labels());
    REQUIRE(to_human_readable_repr(move_ass) == to_human_readable_repr(f));
  }

  LIBSEMIGROUPS_TEST_CASE("Forest", "006", "is_root", "[quick]") {
    Forest f     = test_forest1();
    size_t count = 0;
    for (size_t n = 0; n < f.number_of_nodes(); ++n) {
      count += forest::is_root(f, n);
    }
    REQUIRE(count == 1);
    REQUIRE_THROWS_AS(forest::is_root(f, f.number_of_nodes()),
                      LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("Forest", "007", "fmt", "[quick]") {
    Forest f = test_forest1();
    REQUIRE(
        fmt::format("{}", f)
        == "{[4294967295, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, "
           "13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, "
           "29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, "
           "45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, "
           "61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, "
           "77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, "
           "93, 94, 95, 96, 97, 98], [4294967295, 1, 4, 2, 2, 4, 1, "
           "0, 1, 4, 2, 2, 4, 1, 0, 1, 4, 2, 2, 4, 1, 0, 1, 4, 2, 2, 4, 1, 0, "
           "1, 4, 2, 2, 4, 1, 0, 1, 4, 2, 2, 4, 1, 0, 1, 4, 2, 2, 4, 1, 0, 1, "
           "4, 2, 2, 4, 1, 0, 1, 4, 2, 2, 4, 1, 0, 1, 4, 2, 2, 4, 1, 0, 1, 4, "
           "2, 2, 4, 1, 0, 1, 4, 2, 2, 4, 1, 0, 1, 4, 2, 2, 4, 1, 0, 1, 4, 2, "
           "2, 4, 1, 0, 1]}");
  }

  LIBSEMIGROUPS_TEST_CASE("Forest", "008", "make", "[quick]") {
    Forest f  = test_forest1();
    auto   ff = make<Forest>(
        {UNDEFINED, 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13,
           14,        15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28,
           29,        30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43,
           44,        45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58,
           59,        60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73,
           74,        75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88,
           89,        90, 91, 92, 93, 94, 95, 96, 97, 98},
        {UNDEFINED, 1, 4, 2, 2, 4, 1, 0, 1, 4, 2, 2, 4, 1, 0, 1, 4, 2, 2, 4,
           1,         0, 1, 4, 2, 2, 4, 1, 0, 1, 4, 2, 2, 4, 1, 0, 1, 4, 2, 2,
           4,         1, 0, 1, 4, 2, 2, 4, 1, 0, 1, 4, 2, 2, 4, 1, 0, 1, 4, 2,
           2,         4, 1, 0, 1, 4, 2, 2, 4, 1, 0, 1, 4, 2, 2, 4, 1, 0, 1, 4,
           2,         2, 4, 1, 0, 1, 4, 2, 2, 4, 1, 0, 1, 4, 2, 2, 4, 1, 0, 1});

    REQUIRE(f == ff);
    // Unequal parents and labels sizes
    REQUIRE_THROWS_AS(make<Forest>({UNDEFINED, 0, 1}, {UNDEFINED, 0}),
                      LibsemigroupsException);
    // UNDEFINED not in same positions
    REQUIRE_THROWS_AS(make<Forest>({UNDEFINED, 0, 1}, {1, UNDEFINED, 0}),
                      LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("Forest", "009", "max_label helper", "[quick]") {
    Forest f = test_forest1();
    REQUIRE(forest::max_label(f) == 4);

    f.set_parent_and_label(10, 9, 666);
    REQUIRE(forest::max_label(f) == 666);

    f.init();
    REQUIRE(forest::max_label(f) == UNDEFINED);
    f.add_nodes(1);

    REQUIRE(forest::max_label(f) == UNDEFINED);
    f.add_nodes(10);
    REQUIRE(forest::max_label(f) == UNDEFINED);

    f.init();
    f.add_nodes(2);
    f.set_parent_and_label(0, 1, 4);
    REQUIRE(forest::max_label(f) == 4);
    f.set_parent_and_label(0, 1, 0);
    REQUIRE(forest::max_label(f) == 0);

    f.init();
    f.add_nodes(12);
    f.set_parent_and_label(4, 5, 12);
    REQUIRE(forest::max_label(f) == 12);
  }

  LIBSEMIGROUPS_TEST_CASE("Forest", "010", "is_forest", "[quick]") {
    Forest f = make<Forest>(
        {UNDEFINED, 4, 0, 0, UNDEFINED, 3, 8, 1, 1, 12, 12, 8, 3},
        {UNDEFINED, 0, 0, 1, UNDEFINED, 0, 1, 1, 0, 0, 1, 0, 1});

    REQUIRE(forest::is_forest(f));

    REQUIRE_EXCEPTION_MSG(f.set_parent_and_label(0, 10, 1),
                          "defining the parent of node 0 to be 10 creates a "
                          "cycle [0, 10, ..., 3] (length 4)");

    REQUIRE_THROWS_AS(make<Forest>({1, 0}, {0, 0}), LibsemigroupsException);

    f.init();
    f.add_nodes(2);
    f.set_parent_and_label_no_checks(0, 1, 0);
    f.set_parent_and_label_no_checks(1, 0, 0);

    REQUIRE(!forest::is_forest(f));

    f = v4::word_graph::spanning_tree(binary_tree(16), 0);
    Forest copy(f);
    REQUIRE(forest::is_forest(f));

    REQUIRE_EXCEPTION_MSG(f.set_parent_and_label(0, 2000, 1),
                          "defining the parent of node 0 to be 2000 creates a "
                          "cycle [0, 2000, ..., 2] (length 11)");
    REQUIRE(f == copy);  // Check exception guarantee

    REQUIRE_EXCEPTION_MSG(f.set_parent_and_label(0, 0, 0),
                          "a node cannot be its own parent, attempted to set 0 "
                          "as the parent of 0");
    REQUIRE(f == copy);  // Check exception guarantee
    f.set_parent_and_label_no_checks(0, 0, 0);

    REQUIRE_EXCEPTION_MSG(
        f.throw_if_not_acyclic(),
        "the Forest object contains the loop [0] and is invalid");
  }

  LIBSEMIGROUPS_TEST_CASE("Forest", "011", "dot", "[quick]") {
    Forest f = make<Forest>(
        {UNDEFINED, 4, 0, 0, UNDEFINED, 3, 8, 1, 1, 12, 12, 8, 3},
        {UNDEFINED, 0, 0, 1, UNDEFINED, 0, 1, 1, 0, 0, 1, 0, 1});

    REQUIRE(forest::dot(f).to_string() == R"vogon(digraph Forest {
  rankdir="BT"
  0  [label="0: ε", shape="box"]
  1  [label="1: 0", shape="box"]
  10  [label="10: 111", shape="box"]
  11  [label="11: 000", shape="box"]
  12  [label="12: 11", shape="box"]
  2  [label="2: 0", shape="box"]
  3  [label="3: 1", shape="box"]
  4  [label="4: ε", shape="box"]
  5  [label="5: 01", shape="box"]
  6  [label="6: 100", shape="box"]
  7  [label="7: 10", shape="box"]
  8  [label="8: 00", shape="box"]
  9  [label="9: 011", shape="box"]
  1 -> 4  [color="#00ff00"]
  2 -> 0  [color="#00ff00"]
  3 -> 0  [color="#ff00ff"]
  5 -> 3  [color="#00ff00"]
  6 -> 8  [color="#ff00ff"]
  7 -> 1  [color="#ff00ff"]
  8 -> 1  [color="#00ff00"]
  9 -> 12  [color="#00ff00"]
  10 -> 12  [color="#ff00ff"]
  11 -> 8  [color="#00ff00"]
  12 -> 3  [color="#ff00ff"]
})vogon");

    REQUIRE(forest::dot(f, {"a", "b"}).to_string() == R"vogon(digraph Forest {
  rankdir="BT"
  0  [label="0: ε", shape="box"]
  1  [label="1: a", shape="box"]
  10  [label="10: bbb", shape="box"]
  11  [label="11: aaa", shape="box"]
  12  [label="12: bb", shape="box"]
  2  [label="2: a", shape="box"]
  3  [label="3: b", shape="box"]
  4  [label="4: ε", shape="box"]
  5  [label="5: ab", shape="box"]
  6  [label="6: baa", shape="box"]
  7  [label="7: ba", shape="box"]
  8  [label="8: aa", shape="box"]
  9  [label="9: abb", shape="box"]
  1 -> 4  [color="#00ff00"]
  2 -> 0  [color="#00ff00"]
  3 -> 0  [color="#ff00ff"]
  5 -> 3  [color="#00ff00"]
  6 -> 8  [color="#ff00ff"]
  7 -> 1  [color="#ff00ff"]
  8 -> 1  [color="#00ff00"]
  9 -> 12  [color="#00ff00"]
  10 -> 12  [color="#ff00ff"]
  11 -> 8  [color="#00ff00"]
  12 -> 3  [color="#ff00ff"]
})vogon");

    REQUIRE_THROWS_AS(forest::dot(f, {"a", "b", "c"}), LibsemigroupsException);

    REQUIRE(forest::dot(f, {}).to_string() == R"vogon(digraph Forest {
  rankdir="BT"
  0  [shape="box"]
  1  [shape="box"]
  10  [shape="box"]
  11  [shape="box"]
  12  [shape="box"]
  2  [shape="box"]
  3  [shape="box"]
  4  [shape="box"]
  5  [shape="box"]
  6  [shape="box"]
  7  [shape="box"]
  8  [shape="box"]
  9  [shape="box"]
  1 -> 4  [color="#00ff00"]
  2 -> 0  [color="#00ff00"]
  3 -> 0  [color="#ff00ff"]
  5 -> 3  [color="#00ff00"]
  6 -> 8  [color="#ff00ff"]
  7 -> 1  [color="#ff00ff"]
  8 -> 1  [color="#00ff00"]
  9 -> 12  [color="#00ff00"]
  10 -> 12  [color="#ff00ff"]
  11 -> 8  [color="#00ff00"]
  12 -> 3  [color="#ff00ff"]
})vogon");
  }
}  // namespace libsemigroups
