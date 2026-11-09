// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2026 James D. Mitchell
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

#include "test-main.hpp"  // for LIBSEMIGROUPS_TEST_CASE

#include <cstddef>  // for size_t
#include <cstdint>  // for uint16_t, uint32_t

#include "libsemigroups/order.hpp"       // for LenLexCmp
#include "libsemigroups/word-graph.hpp"  // for WordGraph, standardize_no_checks

#include "libsemigroups/detail/felsch-graph.hpp"        // for IgnoreDefinitions
#include "libsemigroups/detail/node-managed-graph.hpp"  // for NodeManagedGraph
#include "libsemigroups/detail/report.hpp"              // for ReportGuard

namespace libsemigroups {
  LIBSEMIGROUPS_TEST_CASE("NodeManagedGraph",
                          "000",
                          "graph copies preserve traversal cursors",
                          "[node-managed-graph][quick]") {
    using Graph = detail::NodeManagedGraph<uint32_t>;
    Graph original;
    original.init();
    auto const first            = original.new_node();
    auto const second           = original.new_node();
    original.cursor()           = first;
    original.lookahead_cursor() = second;
    SECTION("copy constructor") {
      Graph copy(original);
      REQUIRE(copy.cursor() == first);
      REQUIRE(copy.lookahead_cursor() == second);
    }
    SECTION("copy assignment") {
      Graph copy;
      copy.init();
      copy = original;
      REQUIRE(copy.cursor() == first);
      REQUIRE(copy.lookahead_cursor() == second);
    }
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("NodeManagedGraph",
                                   "001",
                                   "graph imports preserve active edge counts",
                                   "[node-managed-graph][quick]",
                                   uint16_t,
                                   uint32_t) {
    using Graph = detail::NodeManagedGraph<uint32_t>;
    WordGraph<TestType> initial(3, 2);
    initial.target(0, 0, 1);
    initial.target(0, 1, 2);
    initial.target(1, 0, 1);
    initial.target(2, 1, 2);

    auto check = [&initial](Graph const& graph) {
      REQUIRE(graph.number_of_nodes_active() == initial.number_of_nodes());
      REQUIRE(graph.number_of_edges_active() == 4);
      REQUIRE(graph.number_of_edges_active()
              == graph.count_number_of_edges_active());
      REQUIRE(WordGraph(graph) == WordGraph<uint32_t>(initial));
    };

    Graph graph;
    graph.init(initial);
    check(graph);

    graph.init(WordGraph<TestType>(3, 2));
    REQUIRE(graph.number_of_edges_active() == 0);
    REQUIRE(graph.count_number_of_edges_active() == 0);
    graph.init(initial);
    check(graph);
  }

  LIBSEMIGROUPS_TEST_CASE("NodeManagedGraph",
                          "002",
                          "redundant coincidences preserve active edge counts",
                          "[node-managed-graph][quick]") {
    using detail::felsch_graph::IgnoreDefinitions;

    auto                rg = ReportGuard(false);
    WordGraph<uint32_t> initial(3, 2);
    initial.target(0, 0, 1);
    initial.target(0, 1, 2);
    initial.target(1, 0, 1);
    initial.target(2, 1, 2);
    detail::NodeManagedGraph<uint32_t> graph;
    graph.init(initial);
    graph.large_collapse(
        LIBSEMIGROUPS_GENERATE(test::prefix("large_collapse = "),
                               size_t(1),
                               size_t(2),
                               size_t(100'000)));

    SECTION("equal nodes") {
      graph.merge_nodes_no_checks(1, 1);
      graph.merge_nodes_no_checks(2, 2);
      graph.merge_nodes_no_checks(1, 1);
    }
    SECTION("nodes with equal representatives") {
      graph.merge_nodes_no_checks(1, 2);
      REQUIRE(graph.process_coincidences(IgnoreDefinitions{}));
      REQUIRE(graph.number_of_edges_active()
              == graph.count_number_of_edges_active());
      graph.merge_nodes_no_checks(1, 2);
      graph.merge_nodes_no_checks(2, 1);
      graph.merge_nodes_no_checks(1, 2);
    }

    auto const                edges = graph.count_number_of_edges_active();
    auto const                nodes = graph.number_of_nodes_active();
    WordGraph<uint32_t> const before(graph);
    REQUIRE(edges > 0);
    REQUIRE(graph.number_of_edges_active() == edges);
    size_t definitions = 0;
    REQUIRE_FALSE(graph.process_coincidences(
        [&definitions](auto, auto) { ++definitions; }));
    REQUIRE(definitions == 0);
    REQUIRE(graph.number_of_edges_active() == edges);
    REQUIRE(graph.number_of_edges_active()
            == graph.count_number_of_edges_active());
    REQUIRE(graph.number_of_nodes_active() == nodes);
    REQUIRE(WordGraph(graph) == before);
  }

  LIBSEMIGROUPS_TEST_CASE("NodeManagedGraph",
                          "003",
                          "default construction creates an active root",
                          "[node-managed-graph][quick]") {
    detail::NodeManagedGraph<uint32_t> graph;
    REQUIRE(graph.number_of_nodes() == 1);
    REQUIRE(graph.is_active_node(0));

    auto const node = graph.new_node();
    REQUIRE(node == 1);
    REQUIRE(node < graph.number_of_nodes());
    REQUIRE(graph.is_active_node(node));
  }

  LIBSEMIGROUPS_TEST_CASE("NodeManagedGraph",
                          "004",
                          "sized initialization resets graph state",
                          "[node-managed-graph][quick]") {
    detail::NodeManagedGraph<uint32_t> graph;
    graph.init(2, 1);
    graph.target_no_checks(0, 0, 1);
    graph.merge_nodes_no_checks(0, 1);

    graph.init(3, 2);
    REQUIRE(graph.number_of_nodes() == 3);
    REQUIRE(graph.node_capacity() == 3);
    REQUIRE(graph.number_of_nodes_active() == 3);
    REQUIRE(graph.out_degree() == 2);
    REQUIRE(graph.number_of_edges() == 0);
    REQUIRE(graph.number_of_edges_active() == 0);
    REQUIRE(graph.number_of_coincidences() == 0);
  }

  LIBSEMIGROUPS_TEST_CASE("NodeManagedGraph",
                          "005",
                          "batch activation clears recycled rows",
                          "[node-managed-graph][quick]") {
    detail::NodeManagedGraph<uint32_t> graph;
    graph.init(3, 1);
    graph.target_no_checks(0, 0, 2);
    graph.target_no_checks(2, 0, 1);
    graph.merge_nodes_no_checks(1, 2);
    REQUIRE(
        graph.process_coincidences(detail::felsch_graph::IgnoreDefinitions{}));
    REQUIRE_FALSE(graph.is_active_node(2));

    graph.add_active_nodes(2);
    REQUIRE(graph.number_of_nodes() == 4);
    REQUIRE(graph.number_of_nodes() == graph.node_capacity());
    REQUIRE(graph.number_of_nodes_active() == 4);
    REQUIRE(graph.target_no_checks(2, 0) == UNDEFINED);
    REQUIRE(graph.first_source_no_checks(2, 0) == UNDEFINED);
    REQUIRE(graph.count_number_of_edges_active()
            == graph.number_of_edges_active());
  }

  LIBSEMIGROUPS_TEST_CASE("NodeManagedGraph",
                          "006",
                          "erasing free nodes trims graph rows and capacity",
                          "[node-managed-graph][quick]") {
    detail::NodeManagedGraph<uint32_t> graph;
    graph.init(2, 1);
    graph.target_no_checks(0, 0, 1);
    graph.add_free_nodes(5);
    graph.erase_free_nodes();

    REQUIRE(graph.number_of_nodes() == 2);
    REQUIRE(graph.node_capacity() == 2);
    REQUIRE(graph.target_no_checks(0, 0) == 1);
    REQUIRE(graph.number_of_edges_active() == 1);

    auto const node = graph.new_node();
    REQUIRE(node == 2);
    REQUIRE(node < graph.number_of_nodes());
    REQUIRE(graph.number_of_nodes() == graph.node_capacity());
  }

  LIBSEMIGROUPS_TEST_CASE("NodeManagedGraph",
                          "007",
                          "standardization preserves the lookahead cursor",
                          "[node-managed-graph][quick]") {
    detail::NodeManagedGraph<uint32_t> graph;
    graph.init(4, 1);
    graph.target_no_checks(0, 0, 3);
    graph.target_no_checks(3, 0, 2);
    graph.merge_nodes_no_checks(0, 1);
    REQUIRE(
        graph.process_coincidences(detail::felsch_graph::IgnoreDefinitions{}));

    SECTION("cursor points to an active node") {
      graph.lookahead_cursor() = 3;
      REQUIRE(word_graph::standardize_no_checks(graph, LenLexCmp()).first);
      REQUIRE(graph.lookahead_cursor() == 1);
    }
    SECTION("cursor points past the active nodes") {
      graph.lookahead_cursor() = graph.first_free_node();
      REQUIRE(word_graph::standardize_no_checks(graph, LenLexCmp()).first);
      REQUIRE(graph.lookahead_cursor() == graph.first_free_node());
    }
  }
}  // namespace libsemigroups
