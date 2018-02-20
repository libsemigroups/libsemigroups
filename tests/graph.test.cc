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

#include <iostream>

#include "catch.hpp"

#include "../src/graph.h"

using namespace libsemigroups;

TEST_CASE("Digraph 01: Default constructor with 1 default arg",
          "[quick][graph][02]") {
  for (size_t i = 0; i < 100; ++i) {
    Digraph<size_t> g = Digraph<size_t>(i);
    REQUIRE(g.nr_nodes() == 0);
    REQUIRE(g.nr_edges() == 0);
  }
}

TEST_CASE("Digraph 02: Default constructor with 0 default args",
          "[quick][graph][02]") {
  for (size_t i = 0; i < 100; ++i) {
    for (size_t j = 0; j < 100; ++j) {
      Digraph<size_t> g = Digraph<size_t>(i, j);
      REQUIRE(g.nr_nodes() == j);
      REQUIRE(g.nr_edges() == 0);
    }
  }
}

TEST_CASE("Digraph 03: add nodes",
          "[quick][graph][03]") {
  Digraph<size_t> g = Digraph<size_t>(10, 7);
  REQUIRE(g.nr_nodes() == 7);
  REQUIRE(g.nr_edges() == 0);

  for (size_t i = 1; i < 100; ++i) {
    g.add_nodes(i);
    REQUIRE(g.nr_nodes() == 7 + i*(i + 1)/2);
  }

  for (size_t i = 0; i < 100; ++i) {
    for (size_t j = 0; j < 10; ++j) {
      REQUIRE(g.get(i, j) == Digraph<size_t>::UNDEFINED);
    }
  }
}

TEST_CASE("Digraph 04: add edges",
          "[quick][graph][04]") {
  Digraph<size_t> g = Digraph<size_t>(31, 17);

  for (size_t i = 0; i < 17; ++i) {
    for (size_t j = 0; j < 30; ++j) {
      g.add_edge(i, (7 * i + 23 * j) % 17);
    }
  }

  REQUIRE(g.degree_bound() == 31);
  REQUIRE(g.max_degree() == 30);
  REQUIRE(g.nr_edges() == 30 * 17);
  REQUIRE(g.nr_nodes() == 17);

  for (size_t i = 0; i < g.nr_nodes(); ++i) {
    for (size_t j = 0; j < g.degree_bound() - 1; ++j) {
      REQUIRE(g.get(i, j) == (7 * i + 23 * j) % 17);
    }
    REQUIRE(g.get(i, 30) == libsemigroups::Digraph<size_t>::UNDEFINED);
  }
}

TEST_CASE("Digraph 05: Strongly connected components - cycles",
          "[quick][graph][05]") {
  for (size_t j = 2; j < 100; ++j) {
    Digraph<size_t> cycle = Digraph<size_t>(1, j + 1);
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

TEST_CASE("Digraph 06: Strongly connected components - no edges",
          "[quick][graph][06]") {
  Digraph<size_t> graph = Digraph<size_t>(0);
  for (size_t j = 2; j < 100; ++j) {
    graph.add_nodes(j);
    graph.gabow_scc();

    for (size_t i = 0; i < j; ++i) {
      REQUIRE(graph.get_scc_id(i) == i);
    }
  }
}

TEST_CASE("Digraph 07: Strongly connected components - disjoint cycles",
          "[quick][graph][7]") {
  for (size_t j = 2; j < 50; ++j) {
    Digraph<size_t> graph = Digraph<size_t>(1);

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

TEST_CASE("Digraph 08: Strongly connected components - complete graphs",
          "[quick][graph][8]") {
  for (size_t k = 2; k < 50; ++k) {
    Digraph<size_t> graph = Digraph<size_t>(50);
    graph.add_nodes(k);

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

TEST_CASE("Digraph 09: Strongly connected components - empty graph",
          "[quick][graph][9]") {
  Digraph<size_t> graph = Digraph<size_t>(0);
  graph.gabow_scc();
}
