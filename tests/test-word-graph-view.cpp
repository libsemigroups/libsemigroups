//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019-2023 Finn Smith
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

#include <cstddef>  // for size_t
#include <utility>  // for std::move

#include "catch_amalgamated.hpp"  // for REQUIRE, REQUIRE_THROWS_AS, REQUI...
#include "libsemigroups/word-graph.hpp"
#include "test-main.hpp"  // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/word-graph-view.hpp"

namespace libsemigroups {
  struct LibsemigroupsException;

  LIBSEMIGROUPS_TEST_CASE("WordGraphView",
                          "000",
                          "empty constructor",
                          "[quick]") {
    WordGraphView<size_t> g;
    REQUIRE(g.number_of_nodes() == 0);
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraphView",
                          "001",
                          "construct new graph",
                          "[quick]") {
    WordGraph<size_t>     g(10, 5);
    WordGraphView<size_t> v(g, 2, 5);
    REQUIRE(v.number_of_nodes() == 5 - 2);
    REQUIRE(*(v.cbegin_nodes()) == 0);
    REQUIRE(*(v.cend_nodes()) == 3);
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraphView",
                          "002",
                          "equality operator",
                          "[quick]") {
    WordGraph<size_t> g(10, 5);
    WordGraph<size_t> g1(10, 6);
    WordGraph<size_t> g2(10, 5);
    g2.target(2, 3, 5);
    WordGraphView<size_t> v(g, 2, 5);
    WordGraphView<size_t> v1(g, 2, 5);
    WordGraphView<size_t> v2(g, 2, 6);
    WordGraphView<size_t> v3(g1, 2, 5);
    WordGraphView<size_t> v4(g2, 2, 5);
    REQUIRE(v1 == v);
    REQUIRE(v2 != v);
    REQUIRE(v3 != v);
    REQUIRE(v4 != v);
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraphView", "003", "copy ctor", "[quick]") {
    WordGraph<size_t>     g(10, 5);
    WordGraphView<size_t> v(g, 2, 5);
    WordGraphView<size_t> v1 = v;
    REQUIRE(v1.number_of_nodes() == v.number_of_nodes());
    REQUIRE(v1 == v);
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraphView", "004", "move ctor", "[quick]") {
    WordGraph<size_t>     g(10, 5);
    WordGraphView<size_t> v(g, 2, 5);
    WordGraphView<size_t> v1(std::move(v));
    REQUIRE(v1.number_of_nodes() == v.number_of_nodes());
    REQUIRE(v1 == v);
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraphView",
                          "005",
                          "test offset correct",
                          "[quick]") {
    WordGraph<size_t> g(10, 5);
    g.target(2, 3, 5);
    g.target(3, 4, 5);
    WordGraphView<size_t> v(g, 2, 5);
    REQUIRE(v.target(0, 3) == 3);
    REQUIRE(v.target(1, 4) == 3);
  }
  LIBSEMIGROUPS_TEST_CASE("WordGraphView",
                          "006",
                          "test target(s, a) throws correctly",
                          "[quick]") {
    WordGraph<size_t> g(10, 5);
    g.target(2, 3, 5);
    g.target(3, 4, 5);
    WordGraphView<size_t> v(g, 2, 5);
    REQUIRE_THROWS_AS(v.target(5, 3), LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraphView",
                          "007",
                          "test label bounds checking throws",
                          "[quick]") {
    WordGraph<size_t>     g(10, 5);
    WordGraphView<size_t> v(g, 2, 5);
    REQUIRE_THROWS_AS(v.target(0, 7), LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraphView",
                          "008",
                          "test cbegin_targets",
                          "[quick]") {
    WordGraph<size_t> g(10, 5);
    g.target(2, 0, 5);
    WordGraphView<size_t> v(g, 2, 5);

    auto targets = v.cbegin_targets(0);
    REQUIRE(*targets == 3);
    REQUIRE_THROWS_AS(v.cbegin_targets(7), LibsemigroupsException);
    REQUIRE(*(v.cbegin_targets_no_checks(0)) == 3);
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraphView",
                          "009",
                          "test cend_targets",
                          "[quick]") {
    WordGraph<size_t> g(10, 5);
    g.target(2, 4, 5);
    WordGraphView<size_t> v(g, 2, 5);
    auto                  end   = v.cend_targets(0);
    auto                  begin = v.cbegin_targets(0);
    REQUIRE_THROWS_AS(v.cend_targets(7), LibsemigroupsException);
    auto   target = begin;
    size_t i      = 0;
    while (target != end) {
      if (i == 4) {
        REQUIRE(*target == 3);
      } else {
        REQUIRE(*target == UNDEFINED);
      }
      ++target;
      i++;
    }
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraphView",
                          "010",
                          "test nodes() range",
                          "[quick]") {
    WordGraph<size_t> g(10, 5);
    g.target(2, 4, 5);
    WordGraphView<size_t> v(g, 2, 5);

    auto   nodes = v.nodes();
    size_t i     = 0;
    for (const auto& elem : nodes) {
      REQUIRE(elem == i++);
    }
    REQUIRE(i == 3);
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraphView",
                          "011",
                          "test labels is same as graph labels",
                          "[quick]") {
    WordGraph<size_t>     g(10, 5);
    WordGraphView<size_t> v(g, 2, 5);
    auto                  v_labels = v.labels();

    size_t i = 0;
    for (const auto& elem : v_labels) {
      REQUIRE(elem == i++);
    }
    REQUIRE(i == 5);
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraphView",
                          "012",
                          "targets_no_checks",
                          "[quick]") {
    WordGraph<size_t> g(10, 5);
    g.target(2, 1, 5);
    g.target(2, 2, 6);
    g.target(2, 3, 4);
    WordGraphView<size_t> v(g, 2, 5);
    size_t                i = 0;
    for (auto target : v.targets_no_checks(0)) {
      if (i == 0) {
        REQUIRE(target == UNDEFINED);
      }
      if (i == 1) {
        REQUIRE(target == 3);
      }
      if (i == 2) {
        REQUIRE(target == 4);
      }
      if (i == 3) {
        REQUIRE(target == 2);
      }
      if (i == 4) {
        REQUIRE(target == UNDEFINED);
      }
      i++;
    }
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraphView",
                          "013",
                          "next_label_and_target",
                          "[quick]") {
    WordGraph<size_t> g(10, 5);
    g.target(2, 1, 5);
    g.target(2, 2, 6);
    g.target(2, 3, 4);
    WordGraphView<size_t> v(g, 2, 5);

    std::pair<size_t, size_t> compare_to(1, 3);
    std::pair<size_t, size_t> compare_to_1(2, 4);
    REQUIRE(compare_to == v.next_label_and_target(0, 1));
    REQUIRE(compare_to_1 == v.next_label_and_target(0, 2));
    REQUIRE_THROWS_AS(v.next_label_and_target(5, 1), LibsemigroupsException);
    REQUIRE_THROWS_AS(v.next_label_and_target(0, 6), LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraphView",
                          "014",
                          "graph from view",
                          "[quick]") {
    WordGraph<size_t> g(10, 5);
    g.target(2, 1, 5);
    g.target(2, 2, 6);
    g.target(2, 3, 4);
    WordGraphView<size_t> v(g, 2, 8);
    WordGraph<size_t>     g1(6, 5);
    g1.target(0, 1, 3);
    g1.target(0, 2, 4);
    g1.target(0, 3, 2);
    WordGraph<size_t>     from_view = word_graph_view::graph_from_view(v);
    WordGraphView<size_t> from_graph
        = WordGraphView<size_t>(from_view, 0, from_view.number_of_nodes());
    REQUIRE(from_graph == v);
    REQUIRE(from_view == g1);
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraphView",
                          "015",
                          "test to_graph for undefined",
                          "[quick]") {
    WordGraph<size_t>     g(10, 5);
    WordGraphView<size_t> v(g, 2, 5);
    REQUIRE(v.target_no_checks(0, 0) == UNDEFINED);
  }

}  // namespace libsemigroups
