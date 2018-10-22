//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2018 Finn Smith
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

#include <iostream> // FIXME for debugging remove

#include "digraph.hpp"
#include "forest.hpp"
#include "libsemigroups.tests.hpp"

namespace libsemigroups {

  ActionDigraph<size_t> cycle(size_t n) {
    ActionDigraph<size_t> g(n);
    for (size_t i = 0; i < n - 1; ++i) {
      g.add_edge(i, 0, i + 1);
    }
    g.add_edge(n - 1, 0, 0);
    return g;
  }

  LIBSEMIGROUPS_TEST_CASE("ActionDigraph",
                          "001",
                          "constructor with 1  default arg",
                          "[quick][digraph]") {
    ActionDigraph<size_t> g;
    REQUIRE(g.nr_nodes() == 0);
    REQUIRE(g.nr_edges() == 0);
  }

  LIBSEMIGROUPS_TEST_CASE("ActionDigraph",
                          "002",
                          "constructor with 0 default args",
                          "[quick][digraph]") {
    for (size_t j = 0; j < 100; ++j) {
      ActionDigraph<size_t> g(j);
      REQUIRE(g.nr_nodes() == j);
      REQUIRE(g.nr_edges() == 0);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("ActionDigraph",
                          "003",
                          "add nodes",
                          "[quick][digraph]") {
    ActionDigraph<size_t> g(3);
    REQUIRE(g.nr_nodes() == 3);
    REQUIRE(g.nr_edges() == 0);

    for (size_t i = 1; i < 100; ++i) {
      g.add_nodes(i);
      REQUIRE(g.nr_nodes() == 3 + i * (i + 1) / 2);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("ActionDigraph",
                          "004",
                          "add edges",
                          "[quick][digraph]") {
    ActionDigraph<size_t> g(17);

    for (size_t i = 0; i < 17; ++i) {
      for (size_t j = 0; j < 31; ++j) {
        g.add_edge(i, j, (7 * i + 23 * j) % 17);
      }
    }

    REQUIRE(g.nr_edges() == 31 * 17);
    REQUIRE(g.nr_nodes() == 17);

    for (size_t i = 0; i < 17; ++i) {
      for (size_t j = 0; j < 31; ++j) {
        REQUIRE(g.get(i, j) == (7 * i + 23 * j) % 17);
      }
    }

    for (size_t i = 0; i < 17; ++i) {
      for (size_t j = 0; j < 10; ++j) {
        g.add_edge(i, 31 + j, (7 * i + 23 * j) % 17);
      }
    }

    REQUIRE(g.nr_edges() == 41 * 17);
    REQUIRE(g.nr_nodes() == 17);
  }

  LIBSEMIGROUPS_TEST_CASE("ActionDigraph",
                          "005",
                          "strongly connected components - cycles",
                          "[quick][digraph]") {
    for (size_t j = 10; j < 100; ++j) {
      auto g = cycle(j);
      for (size_t i = 0; i < j; ++i) {
        REQUIRE(g.scc_id(i) == 0);
      }
    }
  }

  LIBSEMIGROUPS_TEST_CASE("ActionDigraph",
                          "006",
                          "Strongly connected components - no edges",
                          "[quick][digraph]") {
    ActionDigraph<size_t> graph = ActionDigraph<size_t>(0);
    for (size_t j = 1; j < 100; ++j) {
      graph.add_nodes(j);

      for (size_t i = 0; i < j * (j + 1) / 2; ++i) {
        REQUIRE(graph.scc_id(i) == i);
      }
    }
  }

  LIBSEMIGROUPS_TEST_CASE("ActionDigraph",
                          "007",
                          "strongly connected components - disjoint cycles",
                          "[quick][digraph]") {
    for (size_t j = 2; j < 50; ++j) {
      ActionDigraph<size_t> graph;

      for (size_t k = 0; k < 10; ++k) {
        graph.add_nodes(j);
        for (size_t i = k * j; i < (k + 1) * j - 1; ++i) {
          graph.add_edge(i, 0, i + 1);
        }
        graph.add_edge((k + 1) * j - 1, 0, k * j);
      }
      for (size_t i = 0; i < 10 * j; ++i) {
        REQUIRE(graph.scc_id(i) == i / j);
      }
    }
  }

  LIBSEMIGROUPS_TEST_CASE("ActionDigraph",
                          "008",
                          "Strongly connected components - complete graphs",
                          "[quick][digraph]") {
    for (size_t k = 2; k < 50; ++k) {
      ActionDigraph<size_t> graph(k);

      for (size_t i = 0; i < k; ++i) {
        for (size_t j = 0; j < k; ++j) {
          // might as well leave the loops in
          graph.add_edge(i, j, j);
        }
      }
      for (size_t i = 0; i < k; ++i) {
        REQUIRE(graph.scc_id(i) == 0);
      }
    }
  }

  LIBSEMIGROUPS_TEST_CASE("ActionDigraph",
                          "009",
                          "exceptions",
                          "[quick][digraph]") {
    ActionDigraph<size_t> graph(10);
    REQUIRE_THROWS_AS(graph.get(10, 0), LibsemigroupsException);
    REQUIRE_THROWS_AS(graph.get(0, 1), LibsemigroupsException);

    REQUIRE_THROWS_AS(graph.add_edge(0, 0, 10), LibsemigroupsException);
    REQUIRE_THROWS_AS(graph.add_edge(10, 0, 0), LibsemigroupsException);
    for (size_t i = 0; i < 5; ++i) {
      graph.add_edge(0, i, 1);
      graph.add_edge(2, i, 2);
    }
    REQUIRE_NOTHROW(graph.add_edge(0, 0, 1));
    REQUIRE_NOTHROW(graph.add_edge(2, 0, 2));

    REQUIRE_THROWS_AS(graph.scc_id(10), LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("ActionDigraph",
                          "010",
                          "Spanning forest - complete graphs",
                          "[quick][digraph]") {
    for (size_t k = 2; k < 50; ++k) {
      ActionDigraph<size_t> graph(k);

      for (size_t i = 0; i < k; ++i) {
        for (size_t j = 0; j < k; ++j) {
          // might as well leave the loops in
          graph.add_edge(i, j, j);
        }
      }
      REQUIRE(graph.nr_scc() == 1);

      Forest forest = graph.spanning_forest();

      REQUIRE(forest.parent(1) == UNDEFINED);
      for (size_t i = 0; i < k - 1; ++i) {
        REQUIRE(forest.parent(i) == 1);
      }
    }
  }

  LIBSEMIGROUPS_TEST_CASE("ActionDigraph",
                          "011",
                          "Spanning forest - disjoint cycles",
                          "[quick][digraph]") {
    for (size_t j = 2; j < 50; ++j) {
      ActionDigraph<size_t> graph;

      for (size_t k = 0; k < 10; ++k) {
        graph.add_nodes(j);
        for (size_t i = k * j; i < (k + 1) * j - 1; ++i) {
          graph.add_edge(i, 0, i + 1);
        }
        graph.add_edge((k + 1) * j - 1, 0, k * j);
      }
      for (size_t i = 0; i < 10 * j; ++i) {
        REQUIRE(graph.scc_id(i) == i / j);
      }
      Forest forest = graph.spanning_forest();

      for (size_t i = 0; i < 10 * j; ++i) {
        if (i % j == 0) {
          REQUIRE(forest.parent(i) == UNDEFINED);
        } else {
          REQUIRE(forest.parent(i) == i - 1);
        }
      }
    }
  }

  LIBSEMIGROUPS_TEST_CASE("ActionDigraph",
                          "012",
                          "scc root paths - complete graphs",
                          "[quick][digraph]") {
    for (size_t k = 2; k < 50; ++k) {
      ActionDigraph<size_t> graph(k);

      for (size_t i = 0; i < k; ++i) {
        for (size_t j = 0; j < k; ++j) {
          graph.add_edge(i, j, j);
        }
      }

      for (size_t i = 0; i < k; ++i) {
        size_t              pos  = i;
        std::vector<size_t> path = graph.get_root_path(i);
        for (size_t x : path) {
          pos = graph.get(pos, x);
        }
        REQUIRE(pos == graph.cbegin_sccs()[graph.scc_id(i)][0]);
      }
    }
  }

  LIBSEMIGROUPS_TEST_CASE("ActionDigraph",
                          "013",
                          "scc root paths - disjoint cycles",
                          "[quick][digraph]") {
    for (size_t j = 2; j < 35; ++j) {
      ActionDigraph<size_t> graph;

      for (size_t k = 0; k < 6; ++k) {
        graph.add_nodes(j);
        for (size_t i = k * j; i < (k + 1) * j - 1; ++i) {
          graph.add_edge(i, 0, i + 1);
        }
        graph.add_edge((k + 1) * j - 1, 0, k * j);
      }

      for (size_t i = 0; i < graph.nr_nodes(); ++i) {
        size_t              pos  = i;
        std::vector<size_t> path = graph.get_root_path(i);
        for (size_t x : path) {
          pos = graph.get(pos, x);
        }
        REQUIRE(pos == graph.cbegin_sccs()[graph.scc_id(i)][0]);
      }
    }
  }
}  // namespace libsemigroups
