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

#include "Catch2-3.8.0/catch_amalgamated.hpp"  // for REQUIRE, REQUIRE_THROWS_AS, REQUI...
#include "test-main.hpp"                       // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/word-graph-helpers.hpp"
#include "libsemigroups/word-graph-view.hpp"
#include "libsemigroups/word-graph.hpp"

namespace libsemigroups {
  struct LibsemigroupsException;

  LIBSEMIGROUPS_TEST_CASE("WordGraphView",
                          "000",
                          "default constructor and entire graph",
                          "[quick]") {
    WordGraphView<size_t> v;
    REQUIRE(v.number_of_nodes() == 0);
    WordGraph<size_t>     g1(10, 5);
    WordGraphView<size_t> v1(g1);
    REQUIRE(g1.number_of_nodes() == v1.number_of_nodes());
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
    WordGraph<size_t>     g1(10, 5);
    WordGraphView<size_t> v1(g1, 2, 5);

    // Same graph over same range.
    REQUIRE(WordGraphView<size_t>(g1, 2, 5) == v1);

    // Not equal since the views have different numbers of nodes
    REQUIRE(WordGraphView<size_t>(g1, 2, 6) != v1);

    // Equal because the views are isomorphic
    REQUIRE(WordGraphView<size_t>(g1, 3, 6) == v1);

    WordGraph<size_t> g2(10, 6);

    // Not equal because the views have different out-degree
    REQUIRE(WordGraphView<size_t>(g2, 2, 5) != v1);

    WordGraph<size_t> g3(10, 5);
    g3.target(2, 0, 4);

    // Not equal because the edges are not the same
    REQUIRE(WordGraphView<size_t>(g3, 2, 5) != v1);

    WordGraph<size_t> g4(10, 5);
    g4.target(7, 0, 8);

    // Equal because the views are isomorphic, despite the underlying graphs
    // being non-isomorphic.
    REQUIRE(WordGraphView<size_t>(g4, 2, 5) == v1);
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraphView",
                          "003",
                          "copy constructor + assignment",
                          "[quick]") {
    WordGraph<size_t>     g(10, 5);
    WordGraphView<size_t> v(g, 2, 5);
    WordGraphView<size_t> v1(v);
    WordGraphView<size_t> v2;
    v2 = v;
    REQUIRE(v1.number_of_nodes() == v.number_of_nodes());
    REQUIRE(v1 == v);
    REQUIRE(v2.number_of_nodes() == v.number_of_nodes());
    REQUIRE(v2 == v);
    REQUIRE(v1.number_of_nodes() == v2.number_of_nodes());
    REQUIRE(v1 == v2);
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraphView",
                          "004",
                          "move constructor + assignment",
                          "[quick]") {
    WordGraph<size_t>     g(10, 5);
    WordGraphView<size_t> v(g, 2, 5);
    WordGraphView<size_t> v1(std::move(v));
    WordGraphView<size_t> v2;
    v2 = std::move(v);
    REQUIRE(v1.number_of_nodes() == v.number_of_nodes());
    REQUIRE(v1 == v);
    REQUIRE(v2.number_of_nodes() == v.number_of_nodes());
    REQUIRE(v2 == v);
    REQUIRE(v1.number_of_nodes() == v2.number_of_nodes());
    REQUIRE(v1 == v2);
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
                          "test label and node bounds checking throws",
                          "[quick]") {
    WordGraph<size_t>     g(10, 5);
    WordGraphView<size_t> v(g, 2, 5);
    REQUIRE_THROWS_AS(v.target(0, 7), LibsemigroupsException);
    REQUIRE_THROWS_AS(v.throw_if_label_out_of_bounds(15),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(v.throw_if_node_out_of_bounds(15),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(
        v.throw_if_node_out_of_bounds(g.cbegin_nodes(), g.cbegin_nodes() + 7),
        LibsemigroupsException);
    v.throw_if_node_out_of_bounds(g.cbegin_nodes(), g.cbegin_nodes() + 2);
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
    WordGraph<size_t>     from_view = word_graph::graph_from_view(v);
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

  LIBSEMIGROUPS_TEST_CASE("WordGraphView", "016", "reshape", "[quick]") {
    WordGraph<size_t> g(10, 5);
    g.target(3, 0, 4);
    WordGraphView<size_t> v(g, 2, 5);
    REQUIRE(v.number_of_nodes() == 3);
    REQUIRE(v.start_node() == 2);
    REQUIRE(v.end_node() == 5);
    REQUIRE(v.target(1, 0) == 2);
    v.reshape(3, 7);
    REQUIRE(v.number_of_nodes() == 4);
    REQUIRE(v.start_node() == 3);
    REQUIRE(v.end_node() == 7);
    REQUIRE(v.target(0, 0) == 1);
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraphView", "017", "init", "[quick]") {
    WordGraph<size_t> g(10, 5);
    g.target(3, 0, 4);
    g.target(4, 0, 3);
    WordGraphView<size_t> v1;
    WordGraphView<size_t> v2(g);
    v1.init(g);
    REQUIRE(v1 == v2);

    WordGraphView<size_t> v3(g, 2, 5);
    v1.init(g, 2, 5);
    REQUIRE(v1 == v3);

    WordGraphView<size_t> v4;
    v1.init();
    REQUIRE(v1 == v4);
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraphView",
                          "018",
                          "number_of_edges",
                          "[quick]") {
    WordGraph<size_t> g(10, 5);
    v4::word_graph::add_cycle_no_checks(
        g, g.cbegin_nodes(), g.cbegin_nodes() + 5);
    v4::word_graph::add_cycle_no_checks(
        g, g.cbegin_nodes() + 5, g.cend_nodes());
    REQUIRE(g.number_of_edges() == 10);

    WordGraphView<size_t> v(g);

    for (size_t i = 0; i < 4; ++i) {
      v.reshape(i, i + 1);
      REQUIRE(v.number_of_edges() == 1);
      v.reshape(i + 5, i + 6);
      REQUIRE(v.number_of_edges() == 1);
    }

    v.reshape(4, 6);
    REQUIRE_THROWS(v.throw_if_any_target_out_of_bounds());
    // We might expect this to be 0, since there is no edge between 4 and 5.
    // However, since there is an edge from 4 to 0, and from 5 to 6, we get a
    // count of 2.
    REQUIRE(v.number_of_edges() == 2);

    v.reshape(0, 5);
    REQUIRE(v.number_of_edges() == 5);
    v.reshape(5, 10);
    REQUIRE(v.number_of_edges() == 5);
    v.reshape(0, 10);
    REQUIRE(v.number_of_edges() == 10);
  }

}  // namespace libsemigroups
