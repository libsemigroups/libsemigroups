//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2017 James D. Mitchell
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

#include <iostream>

#include "catch.hpp"

#include "../src/graph.h"

using namespace libsemigroups;

TEST_CASE("Graph 01: Default constructor with 1 default arg",
          "[quick][graph][02]") {
  for (size_t i = 0; i < 100; ++i) {
    Graph g = Graph(i);
    REQUIRE(g.nr_nodes() == 0);
    REQUIRE(g.nr_cols() == i);
    REQUIRE(g.nr_edges() == 0);
  }
}

TEST_CASE("Graph 03: Default constructor with 0 default args",
          "[quick][graph][03]") {
  for (size_t i = 0; i < 100; ++i) {
    for (size_t j = 0; j < 100; ++j) {
      Graph g = Graph(i, j);
      REQUIRE(g.nr_nodes() == j);
      REQUIRE(g.nr_cols() == i);
      REQUIRE(g.nr_edges() == 0);
    }
  }
}

TEST_CASE("Graph 04: add nodes",
          "[quick][graph][04]") {
  Graph g = Graph(10, 7);
  REQUIRE(g.nr_nodes() == 7);
  REQUIRE(g.nr_cols() == 10);
  REQUIRE(g.nr_edges() == 0);

  for (size_t i = 1; i < 100; ++i) {
    g.add_nodes(i);
    REQUIRE(g.nr_nodes() == 7 + i*(i + 1)/2);
  }

  for (size_t i = 0; i < 100; ++i) {
    for (size_t j = 0; j < 10; ++j) {
      REQUIRE(g.get(i, j) == Graph::UNDEFINED);
    }
  }
}

TEST_CASE("Graph 05: add edges",
          "[quick][graph][05]") {
  Graph g = Graph(30, 17);

  for (size_t i = 0; i < 17; ++i) {
    for (size_t j = 0; j < 30; ++j) {
      g.add_edge(i, (7 * i + 23 * j) % 17);
    }
  }

  REQUIRE(g.nr_cols() == 30);
  REQUIRE(g.nr_nodes() == 17);

  for (size_t i = 0; i < g.nr_nodes(); ++i) {
    for (auto it = g.begin_row(i); it < g.end_row(i); ++it) {
      size_t j = it - g.begin_row(i);
      REQUIRE(g.get(i, j) == (7 * i + 23 * j) % 17);
    }
  }
}

TEST_CASE("Graph 07: tidy",
          "[quick][graph][07]") {
  Graph g = Graph(100, 100);

  for (size_t i = 0; i < 100; ++i) {
    for (size_t j = 0; j < 100; ++j) {
      g.add_edge(i, (73 * i + 85 * j) % 100);
      if (g.get(i, j) % 13 == 0) {
        g.set(i, j, Graph::UNDEFINED);
      }
    }
  }

  g.tidy();

  for (size_t i = 0; i < 100; ++i) {
    for (auto it = g.begin_row(i); it < g.end_row(i) - 1; ++it) {
      REQUIRE(*(it + 1) >= *it);
    }
  }
}

TEST_CASE("Graph 08: Strongly connected components - cycles",
          "[quick][graph][08]") {
  for (size_t j = 2; j < 100; ++j) {
    Graph cycle = Graph(1, j + 1);
    for (size_t i = 0; i < j; ++i) {
      cycle.add_edge(i, i + 1);
    }
    cycle.add_edge(j, 0);
    cycle.gabow_scc();

    for (size_t i = 0; i < j; ++i) {
      REQUIRE(cycle.get_scc_id(i) == 0);
    }
  }
}

TEST_CASE("Graph 09: Strongly connected components - no edges",
          "[quick][graph][09]") {
  Graph graph = Graph(0);
  for (size_t j = 2; j < 100; ++j) {
    graph.add_nodes(j);
    graph.gabow_scc();

    for (size_t i = 0; i < j; ++i) {
      REQUIRE(graph.get_scc_id(i) == i);
    }
  }
}

TEST_CASE("Graph 10: Strongly connected components - disjoint cycles",
          "[quick][graph][10]") {
  for (size_t j = 2; j < 50; ++j) {
    Graph graph = Graph(1);

    for (size_t k = 0; k < 10; ++k) {
      graph.add_nodes(j);
      for (size_t i = k * j; i < (k + 1) * j - 1; ++i) {
        graph.add_edge(i, i + 1);
      }
      graph.add_edge((k + 1) * j - 1, k * j);

      graph.gabow_scc();
      for (size_t i = 0; i < j; ++i) {
        REQUIRE(graph.get_scc_id(i) == i/j);
      }
    }
  }
}

TEST_CASE("Graph 11: Strongly connected components - complete graphs",
          "[quick][graph][11]") {
  for (size_t k = 2; k < 50; ++k) {
    Graph graph = Graph(50);
    graph.add_nodes(k);

    for (size_t i = 0; i < k; ++i) {
      for (size_t j = 0; j < k; ++j) {
        // might as well leave the loops in
        graph.add_edge(i, j);
      }
    }

    graph.gabow_scc();
    for (size_t i = 0; i < k; ++i) {
      REQUIRE(graph.get_scc_id(i) == 0);
    }
  }
}

TEST_CASE("Graph 12: Strongly connected components - empty graph",
          "[quick][graph][12]") {
  Graph graph = Graph(0);
  graph.gabow_scc();
}
