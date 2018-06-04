//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2018 James D. Mitchell
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

#include "catch.hpp"

#include "../src/digraph.h"

using namespace libsemigroups;

TEST_CASE("CayleyDigraph 01: Default constructor with 1 default arg",
          "[quick][digraph][9]") {
  for (size_t i = 0; i < 100; ++i) {
    CayleyDigraph<size_t> g = CayleyDigraph<size_t>(i, 0);
    REQUIRE(g.nr_nodes() == 0);
    REQUIRE(g.nr_edges() == 0);
    REQUIRE(g.max_degree() == 0);
    REQUIRE(g.degree_bound() == i);
  }
}

TEST_CASE("CayleyDigraph 02: Default constructor with 0 default args",
          "[quick][digraph][10]") {
  for (size_t i = 0; i < 100; ++i) {
    for (size_t j = 0; i < 100; ++i) {
      CayleyDigraph<size_t> g = CayleyDigraph<size_t>(i, j);
      REQUIRE(g.nr_nodes() == j);
      REQUIRE(g.nr_edges() == 0);
      REQUIRE(g.max_degree() == 0);
      REQUIRE(g.degree_bound() == i);
    }
  }
}

TEST_CASE("CayleyDigraph 03: add nodes", "[quick][digraph][03]") {
  CayleyDigraph<size_t> g = CayleyDigraph<size_t>(0, 3);
  REQUIRE(g.nr_nodes() == 3);
  REQUIRE(g.nr_edges() == 0);

  for (size_t i = 1; i < 100; ++i) {
    g.add_nodes(i);
    REQUIRE(g.nr_nodes() == 3 + i * (i + 1) / 2);
  }
}

TEST_CASE("CayleyDigraph 04: add edges", "[quick][digraph][4]") {
  CayleyDigraph<size_t> g = CayleyDigraph<size_t>(31, 17);

  for (size_t i = 0; i < 17; ++i) {
    for (size_t j = 0; j < g.degree_bound(); ++j) {
      g.add_edge(i, (7 * i + 23 * j) % 17);
    }
  }

  REQUIRE(g.degree_bound() == 31);
  REQUIRE(g.max_degree() == 31);
  REQUIRE(g.nr_edges() == 31 * 17);
  REQUIRE(g.nr_nodes() == 17);

  for (size_t i = 0; i < g.nr_nodes(); ++i) {
    for (size_t j = 0; j < g.degree_bound() - 1; ++j) {
      REQUIRE(g.get(i, j) == (7 * i + 23 * j) % 17);
    }
  }

  g.increase_degree(10);
  REQUIRE(g.degree_bound() == 41);

  for (size_t i = 0; i < 17; ++i) {
    for (size_t j = 0; j < 10; ++j) {
      g.add_edge(i, (7 * i + 23 * j) % 17);
    }
  }

  REQUIRE(g.degree_bound() == 41);
  REQUIRE(g.max_degree() == 41);
  REQUIRE(g.nr_edges() == 41 * 17);
  REQUIRE(g.nr_nodes() == 17);
}

TEST_CASE("CayleyDigraph 05: Strongly connected components - cycles",
          "[quick][digraph][05]") {
  for (size_t j = 10; j < 100; ++j) {
    CayleyDigraph<size_t> cycle = CayleyDigraph<size_t>(1, j + 1);

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

TEST_CASE("CayleyDigraph 06: Strongly connected components - no edges",
          "[quick][digraph][06]") {
  CayleyDigraph<size_t> graph = CayleyDigraph<size_t>(0);
  for (size_t j = 1; j < 100; ++j) {
    graph.add_nodes(j);

    graph.gabow_scc();
    for (size_t i = 0; i < j * (j + 1) / 2; ++i) {
      REQUIRE(graph.get_scc_id(i) == i);
    }
  }
}

TEST_CASE("CayleyDigraph 07: Strongly connected components - disjoint cycles",
          "[quick][digraph][7]") {
  for (size_t j = 2; j < 50; ++j) {
    CayleyDigraph<size_t> graph = CayleyDigraph<size_t>(1);

    for (size_t k = 0; k < 10; ++k) {
      graph.add_nodes(j);
      for (size_t i = k * j; i < (k + 1) * j - 1; ++i) {
        graph.add_edge(i, i + 1);
      }
      graph.add_edge((k + 1) * j - 1, k * j);

      graph.gabow_scc();
      for (size_t i = 0; i < j; ++i) {
        REQUIRE(graph.get_scc_id(i) == i / j);
      }
    }
  }
}

TEST_CASE("CayleyDigraph 08: Strongly connected components - complete graphs",
          "[quick][digraph][8]") {
  for (size_t k = 2; k < 50; ++k) {
    CayleyDigraph<size_t> graph = CayleyDigraph<size_t>(k * k, k);

    for (size_t i = 0; i < k; ++i) {
      for (size_t j = 0; j < k; ++j) {
        // might as well leave the loops in
        graph.add_edge(i, j);
      }
    }

    for (size_t i = 0; i < k; ++i) {
      REQUIRE(graph.get_scc_id(i) == 0);
    }
  }
}

TEST_CASE("CayleyDigraph 09: Strongly connected components - empty graph",
          "[quick][digraph][9]") {
  CayleyDigraph<size_t> graph = CayleyDigraph<size_t>(0, 0);
  graph.gabow_scc();
}

TEST_CASE("CayleyDigraph 10: Exceptions",
          "[quick][digraph][10]") {
  CayleyDigraph<size_t> graph = CayleyDigraph<size_t>(5, 10);
  REQUIRE_THROWS_AS(graph.get(10, 0), LibsemigroupsException);
  REQUIRE_THROWS_AS(graph.get(0, 1), LibsemigroupsException);

  REQUIRE_THROWS_AS(graph.add_edge(0, 10), LibsemigroupsException);
  REQUIRE_THROWS_AS(graph.add_edge(10, 0), LibsemigroupsException);
  for (size_t i = 0; i < 5; ++i) {
    graph.add_edge(0, 1);
    graph.add_edge(2, 2);
  }
  REQUIRE_THROWS_AS(graph.add_edge(0, 1), LibsemigroupsException);
  REQUIRE_THROWS_AS(graph.add_edge(2, 2), LibsemigroupsException);

  graph.gabow_scc();
  REQUIRE_THROWS_AS(graph.get_scc_id(10), LibsemigroupsException);
}
