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

#include <cstddef>        // for size_t
#include <stdexcept>      // for runtime_error
#include <unordered_set>  // for unordered_set
#include <vector>         // for vector

#include "catch.hpp"      // for REQUIRE, REQUIRE_THROWS_AS, REQUI...
#include "test-main.hpp"  // for LIBSEMIGROUPS_TEST_CASE
#include "word-graph-test-common.hpp"  // for add_clique etc

#include "libsemigroups/forest.hpp"      // for Forest
#include "libsemigroups/word-graph.hpp"  // for WordGraph
#include "libsemigroups/words.hpp"       // for literals, Words

namespace libsemigroups {

  using namespace literals;

  struct LibsemigroupsException;  // forward decl

  namespace {
    void add_path(WordGraph<size_t>& digraph, size_t n) {
      size_t old_nodes = digraph.number_of_nodes();
      digraph.add_nodes(n);
      for (size_t i = old_nodes; i < digraph.number_of_nodes() - 1; ++i) {
        digraph.set_target(i, 0, i + 1);
      }
    }

    WordGraph<size_t> path(size_t n) {
      WordGraph<size_t> g(0, 1);
      add_path(g, n);
      return g;
    }
  }  // namespace

  LIBSEMIGROUPS_TEST_CASE("WordGraph",
                          "000",
                          "constructor with 1  default arg",
                          "[quick][digraph]") {
    WordGraph<size_t> g;
    REQUIRE(g.number_of_nodes() == 0);
    REQUIRE(g.number_of_edges() == 0);
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph",
                          "001",
                          "constructor with 0 default args",
                          "[quick][digraph]") {
    for (size_t j = 0; j < 100; ++j) {
      WordGraph<size_t> g(j);
      REQUIRE(g.number_of_nodes() == j);
      REQUIRE(g.number_of_edges() == 0);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph", "002", "add nodes", "[quick][digraph]") {
    WordGraph<size_t> g(3);
    REQUIRE(g.number_of_nodes() == 3);
    REQUIRE(g.number_of_edges() == 0);

    for (size_t i = 1; i < 100; ++i) {
      g.add_nodes(i);
      REQUIRE(g.number_of_nodes() == 3 + i * (i + 1) / 2);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph", "003", "add edges", "[quick][digraph]") {
    WordGraph<size_t> g(17, 31);

    for (size_t i = 0; i < 17; ++i) {
      for (size_t j = 0; j < 31; ++j) {
        g.set_target(i, j, (7 * i + 23 * j) % 17);
      }
    }

    REQUIRE(g.number_of_edges() == 31 * 17);
    REQUIRE(g.number_of_nodes() == 17);
    REQUIRE_THROWS_AS(g.set_target(0, 0, 32), LibsemigroupsException);

    for (size_t i = 0; i < 17; ++i) {
      for (size_t j = 0; j < 31; ++j) {
        REQUIRE(g.neighbor(i, j) == (7 * i + 23 * j) % 17);
      }
    }

    g.add_to_out_degree(10);
    REQUIRE(g.out_degree() == 41);
    REQUIRE(g.number_of_nodes() == 17);
    REQUIRE(!g.validate());

    for (size_t i = 0; i < 17; ++i) {
      for (size_t j = 0; j < 10; ++j) {
        g.set_target(i, 31 + j, (7 * i + 23 * j) % 17);
      }
    }

    REQUIRE(g.number_of_edges() == 41 * 17);
    REQUIRE(g.number_of_nodes() == 17);
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph",
                          "004",
                          "exceptions",
                          "[quick][digraph]") {
    WordGraph<size_t> graph(10, 5);
    REQUIRE_THROWS_AS(graph.neighbor(10, 0), LibsemigroupsException);
    REQUIRE(graph.neighbor(0, 1) == UNDEFINED);

    REQUIRE_THROWS_AS(graph.set_target(0, 10, 0), LibsemigroupsException);
    REQUIRE_THROWS_AS(graph.set_target(10, 0, 0), LibsemigroupsException);
    for (size_t i = 0; i < 5; ++i) {
      graph.set_target(0, i, 1);
      graph.set_target(2, i, 2);
    }
    REQUIRE_NOTHROW(graph.set_target(0, 0, 1));
    REQUIRE_NOTHROW(graph.set_target(2, 0, 2));
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph", "005", "random", "[quick][digraph]") {
    WordGraph<size_t> graph = WordGraph<size_t>::random(10, 10);
    REQUIRE(graph.number_of_nodes() == 10);
    REQUIRE(graph.number_of_edges() == 100);
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph", "006", "reserve", "[quick][digraph]") {
    WordGraph<size_t> graph;
    graph.reserve(10, 10);
    REQUIRE(graph.number_of_nodes() == 0);
    REQUIRE(graph.number_of_edges() == 0);
    graph.add_nodes(1);
    REQUIRE(graph.number_of_nodes() == 1);
    graph.add_nodes(9);
    REQUIRE(graph.number_of_nodes() == 10);
    REQUIRE(graph.number_of_edges() == 0);
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph",
                          "007",
                          "default constructors",
                          "[quick][digraph]") {
    auto g1 = WordGraph<size_t>();
    g1.add_to_out_degree(1);
    word_graph::add_cycle(g1, 10);

    // Copy constructor
    auto g2(g1);
    REQUIRE(g2.number_of_edges() == 10);
    REQUIRE(g2.number_of_nodes() == 10);

    // Move constructor
    auto g3(std::move(g2));
    REQUIRE(g3.number_of_edges() == 10);
    REQUIRE(g3.number_of_nodes() == 10);

    // Copy assignment
    g2 = g3;
    REQUIRE(g2.number_of_edges() == 10);
    REQUIRE(g2.number_of_nodes() == 10);
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph",
                          "008",
                          "iterator to edges",
                          "[quick][digraph]") {
    for (size_t n = 10; n < 512; n *= 4) {
      auto g = clique(n);
      REQUIRE(g.number_of_nodes() == n);
      REQUIRE(g.number_of_edges() == n * n);

      using node_type = decltype(g)::node_type;

      auto expected = std::vector<node_type>(n, 0);
      std::iota(expected.begin(), expected.end(), 0);

      for (auto it = g.cbegin_nodes(); it < g.cend_nodes(); ++it) {
        auto result
            = std::vector<node_type>(g.cbegin_edges(*it), g.cend_edges(*it));
        REQUIRE(result == expected);
      }
      REQUIRE_THROWS_AS(g.cbegin_edges(n), LibsemigroupsException);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph",
                          "009",
                          "reverse node iterator",
                          "[quick]") {
    using node_type = WordGraph<size_t>::node_type;
    WordGraph<size_t> ad;
    ad.add_nodes(10);
    REQUIRE(ad.number_of_nodes() == 10);
    REQUIRE(std::vector<node_type>(ad.cbegin_nodes(), ad.cend_nodes())
            == std::vector<node_type>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9}));

    auto it = ad.cbegin_nodes();
    REQUIRE(*it == 0);
    auto copy(it);
    REQUIRE(*copy == 0);
    it       = ad.cend_nodes();
    auto tmp = it;
    REQUIRE(*--tmp == 9);

    REQUIRE(std::vector<node_type>(ad.crbegin_nodes(), ad.crend_nodes())
            == std::vector<node_type>({9, 8, 7, 6, 5, 4, 3, 2, 1, 0}));
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph",
                          "010",
                          "random/random_acyclic exceptions",
                          "[quick]") {
    // Too few nodes
    REQUIRE_THROWS_AS(WordGraph<size_t>::random(0, 0, 0),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(WordGraph<size_t>::random_acyclic(0, 0, 0),
                      LibsemigroupsException);
    // Out degree too low
    REQUIRE_THROWS_AS(WordGraph<size_t>::random(2, 0, 0),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(WordGraph<size_t>::random_acyclic(2, 0, 0),
                      LibsemigroupsException);
    // Number of edges too high
    REQUIRE_THROWS_AS(WordGraph<size_t>::random(2, 2, 5),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(WordGraph<size_t>::random_acyclic(2, 2, 5),
                      LibsemigroupsException);
    // Number of edges = 0
    auto ad = WordGraph<size_t>::random(2, 2, 0);
    REQUIRE(ad.number_of_edges() == 0);
    ad = WordGraph<size_t>::random_acyclic(2, 2, 0);
    REQUIRE(ad.number_of_edges() == 0);
    ad = WordGraph<size_t>::random_acyclic(10, 10, 41);
    REQUIRE(ad.number_of_edges() == 41);
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph",
                          "011",
                          "unsafe (next) neighbour",
                          "[quick]") {
    auto ad = binary_tree(10);
    REQUIRE(ad.neighbor_no_checks(0, 1) == ad.neighbor(0, 1));
    REQUIRE(ad.next_neighbor_no_checks(0, 1) == ad.next_neighbor(0, 1));
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph",
                          "012",
                          "number_of_egdes incident to a node",
                          "[quick]") {
    auto ad = binary_tree(10);
    REQUIRE(ad.number_of_nodes() == 1023);
    REQUIRE(
        std::count_if(ad.cbegin_nodes(),
                      ad.cend_nodes(),
                      [&ad](size_t n) { return ad.number_of_edges(n) == 2; })
        == 511);
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph",
                          "013",
                          "induced_subgraph_no_checks",
                          "[quick]") {
    WordGraph<size_t> ad;
    ad.add_nodes(3);
    ad.add_to_out_degree(2);
    ad.set_target(0, 0, 1);
    ad.set_target(1, 0, 0);
    ad.set_target(2, 0, 0);

    ad.induced_subgraph_no_checks(0, 2);
    REQUIRE(ad == to_word_graph<size_t>(2, {{1, UNDEFINED}, {0}}));
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph",
                          "014",
                          "remove_target_no_checks",
                          "[quick]") {
    WordGraph<size_t> ad;
    ad.add_nodes(3);
    ad.add_to_out_degree(2);
    ad.set_target(0, 0, 1);
    ad.set_target(1, 0, 0);
    ad.set_target(2, 0, 0);

    ad.remove_target_no_checks(0, 0);  // remove edge from 0 labelled 0
    REQUIRE(ad == to_word_graph<size_t>(3, {{UNDEFINED, UNDEFINED}, {0}, {0}}));
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph",
                          "015",
                          "swap_edge_no_checks",
                          "[quick]") {
    WordGraph<size_t> ad;
    ad.add_nodes(3);
    ad.add_to_out_degree(2);
    ad.set_target(0, 0, 1);
    ad.set_target(1, 0, 0);
    ad.set_target(2, 0, 2);

    // swap edge from 0 labelled 0 with edge from 1 labelled 0
    ad.swap_targets_no_checks(0, 1, 0);
    REQUIRE(ad == to_word_graph<size_t>(3, {{0, UNDEFINED}, {1}, {2}}));
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph", "016", "operator<<", "[quick]") {
    WordGraph<size_t> ad;
    ad.add_nodes(3);
    ad.add_to_out_degree(2);
    ad.set_target(0, 0, 1);
    ad.set_target(1, 0, 0);
    ad.set_target(2, 0, 2);

    std::ostringstream oss;
    oss << ad;
    REQUIRE(oss.str() == "{{1, -}, {0, -}, {2, -}}");
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph",
                          "017",
                          "is_acyclic | 2-cycle",
                          "[quick]") {
    WordGraph<size_t> ad;
    ad.add_nodes(2);
    ad.add_to_out_degree(1);
    ad.set_target(0, 0, 1);
    ad.set_target(1, 0, 0);
    REQUIRE(!word_graph::is_acyclic(ad));
    REQUIRE(word_graph::topological_sort(ad).empty());
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph",
                          "018",
                          "is_acyclic | 1-cycle",
                          "[quick]") {
    WordGraph<size_t> ad;
    ad.add_nodes(1);
    ad.add_to_out_degree(1);
    ad.set_target(0, 0, 0);
    REQUIRE(!word_graph::is_acyclic(ad));
    REQUIRE(word_graph::topological_sort(ad).empty());
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph",
                          "019",
                          "is_acyclic | multi-digraph",
                          "[quick]") {
    using node_type = WordGraph<size_t>::node_type;
    WordGraph<size_t> ad;
    ad.add_nodes(2);
    ad.add_to_out_degree(2);
    ad.set_target(0, 0, 1);
    ad.set_target(0, 1, 1);
    REQUIRE(word_graph::is_acyclic(ad));
    REQUIRE(word_graph::topological_sort(ad) == std::vector<node_type>({1, 0}));
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph",
                          "020",
                          "is_acyclic | complete digraph 100",
                          "[quick]") {
    WordGraph<size_t> ad;
    size_t const      n = 100;
    ad.add_nodes(n);
    ad.add_to_out_degree(n);
    for (size_t i = 0; i < n; ++i) {
      for (size_t j = 0; j < n; ++j) {
        if (i != j) {
          ad.set_target(i, j, j);
        }
      }
    }
    REQUIRE(!word_graph::is_acyclic(ad));
    REQUIRE(word_graph::topological_sort(ad).empty());
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph",
                          "021",
                          "is_acyclic | acyclic digraph with 20000 nodes",
                          "[quick]") {
    WordGraph<size_t> ad;
    size_t const      n = 20000;
    ad.add_nodes(n);
    ad.add_to_out_degree(2);
    for (size_t i = 0; i < (n / 2 - 1); ++i) {
      ad.set_target(i, 0, i + 1);
    }
    ad.set_target(n / 2 - 1, 1, n - 1);
    ad.set_target(n / 2 + 1, 1, (3 * n) / 4 - 1);
    ad.set_target(n / 2, 1, 0);
    for (size_t i = n / 2; i < n - 1; ++i) {
      ad.set_target(i, 0, i + 1);
    }
    REQUIRE(word_graph::is_acyclic(ad));
    REQUIRE(word_graph::topological_sort(ad).size() == ad.number_of_nodes());
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph",
                          "022",
                          "is_acyclic | acyclic digraph with 10 million nodes",
                          "[standard]") {
    WordGraph<size_t> ad;
    size_t const      n = 10000000;
    ad.add_nodes(n);
    ad.add_to_out_degree(2);
    for (size_t i = 0; i < (n / 2 - 1); ++i) {
      ad.set_target(i, 0, i + 1);
    }
    ad.set_target(n / 2 - 1, 1, n - 1);
    ad.set_target(n / 2 + 1, 1, (3 * n) / 4 - 1);
    ad.set_target(n / 2, 1, 0);
    for (size_t i = n / 2; i < n - 1; ++i) {
      ad.set_target(i, 0, i + 1);
    }
    REQUIRE(word_graph::is_acyclic(ad));
    REQUIRE(word_graph::topological_sort(ad).size() == n);
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph",
                          "023",
                          "is_acyclic | for a node",
                          "[quick]") {
    using node_type = WordGraph<size_t>::node_type;
    WordGraph<size_t> ad;
    size_t const      n = 100;
    ad.add_nodes(n);
    ad.add_to_out_degree(2);
    for (size_t i = 0; i < n - 1; ++i) {
      ad.set_target(i, i % 2, i + 1);
    }
    word_graph::add_cycle(ad, 100);

    REQUIRE(std::all_of(
        ad.cbegin_nodes(), ad.cbegin_nodes() + 100, [&ad](node_type const& v) {
          return word_graph::is_acyclic(ad, v);
        }));

    REQUIRE(std::none_of(
        ad.crbegin_nodes(),
        ad.crbegin_nodes() + 100,
        [&ad](node_type const& v) { return word_graph::is_acyclic(ad, v); }));
    REQUIRE(!word_graph::is_acyclic(ad));
    REQUIRE(word_graph::is_acyclic(ad, 10, 20));
    REQUIRE(!word_graph::is_acyclic(ad, 100, 120));
    REQUIRE(word_graph::is_acyclic(ad, 10, 120));
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph",
                          "024",
                          "is_acyclic | for a node",
                          "[quick]") {
    WordGraph<size_t> ad;
    using node_type = decltype(ad)::node_type;
    ad.add_nodes(4);
    ad.add_to_out_degree(1);
    ad.set_target(0, 0, 1);
    ad.set_target(1, 0, 0);
    ad.set_target(2, 0, 3);
    REQUIRE(!word_graph::is_acyclic(ad));
    REQUIRE(word_graph::topological_sort(ad).empty());
    REQUIRE(!word_graph::is_acyclic(ad, 0));
    REQUIRE(word_graph::topological_sort(ad, 0).empty());
    REQUIRE(!word_graph::is_acyclic(ad, 1));
    REQUIRE(word_graph::topological_sort(ad, 1).empty());

    REQUIRE(word_graph::is_acyclic(ad, 2));
    REQUIRE(word_graph::topological_sort(ad, 2)
            == std::vector<node_type>({3, 2}));
    REQUIRE(word_graph::is_acyclic(ad, 3));
    REQUIRE(word_graph::topological_sort(ad, 3) == std::vector<node_type>({3}));
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph",
                          "025",
                          "is_reachable | acyclic 20 node digraph",
                          "[quick]") {
    WordGraph<size_t> ad;
    size_t const      n = 20;
    ad.add_nodes(n);
    ad.add_to_out_degree(2);
    for (size_t i = 0; i < (n / 2 - 1); ++i) {
      ad.set_target(i, 0, i + 1);
    }
    ad.set_target(n / 2 - 1, 1, n - 1);
    ad.set_target(n / 2 + 1, 1, (3 * n) / 4 - 1);
    ad.set_target(n / 2, 1, 0);
    for (size_t i = n / 2; i < n - 1; ++i) {
      ad.set_target(i, 0, i + 1);
    }
    REQUIRE(!word_graph::is_reachable(ad, 1, 10));
    REQUIRE(word_graph::is_reachable(ad, 10, 1));
    REQUIRE_THROWS_AS(word_graph::is_reachable(ad, 20, 1),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(word_graph::is_reachable(ad, 1, 20),
                      LibsemigroupsException);
    REQUIRE(word_graph::is_reachable(ad, 1, 1));
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph",
                          "026",
                          "is_reachable | 100 node path",
                          "[quick]") {
    WordGraph<size_t> ad;
    size_t const      n = 100;
    ad.add_nodes(n);
    ad.add_to_out_degree(2);
    for (size_t i = 0; i < n - 1; ++i) {
      ad.set_target(i, i % 2, i + 1);
    }
    for (auto it1 = ad.cbegin_nodes(); it1 < ad.cend_nodes(); ++it1) {
      for (auto it2 = it1 + 1; it2 < ad.cend_nodes(); ++it2) {
        REQUIRE(word_graph::is_reachable(ad, *it1, *it2));
        REQUIRE(!word_graph::is_reachable(ad, *it2, *it1));
      }
    }
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph",
                          "027",
                          "is_reachable | 100 node cycle",
                          "[quick]") {
    WordGraph<size_t> ad;
    ad.add_to_out_degree(1);
    word_graph::add_cycle(ad, 100);
    for (auto it1 = ad.cbegin_nodes(); it1 < ad.cend_nodes(); ++it1) {
      for (auto it2 = it1 + 1; it2 < ad.cend_nodes(); ++it2) {
        REQUIRE(word_graph::is_reachable(ad, *it1, *it2));
        REQUIRE(word_graph::is_reachable(ad, *it2, *it1));
      }
    }
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph",
                          "028",
                          "is_reachable | 20 node clique",
                          "[quick]") {
    WordGraph<size_t> ad = clique(20);
    for (auto it1 = ad.cbegin_nodes(); it1 < ad.cend_nodes(); ++it1) {
      for (auto it2 = it1 + 1; it2 < ad.cend_nodes(); ++it2) {
        REQUIRE(word_graph::is_reachable(ad, *it1, *it2));
        REQUIRE(word_graph::is_reachable(ad, *it2, *it1));
      }
    }
    REQUIRE(ad.validate());
    REQUIRE(word_graph::topological_sort(ad).empty());
    REQUIRE(word_graph::topological_sort(ad, 0).empty());
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph",
                          "029",
                          "follow_path | 20 node path",
                          "[quick]") {
    WordGraph<size_t> ad = path(20);
    for (auto it = cbegin_pilo(ad, 0); it != cend_pilo(ad); ++it) {
      REQUIRE(word_graph::follow_path(ad, 0, *it) == it.target());
      REQUIRE(word_graph::follow_path_no_checks(ad, 0, *it) == it.target());
    }
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph",
                          "030",
                          "validate_label | 20 node path",
                          "[quick]") {
    WordGraph<size_t> ad = path(20);
    REQUIRE_THROWS_AS(word_graph::validate_label(ad, 10),
                      LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph",
                          "031",
                          "last_node_on_path_no_checks | 20 node path",
                          "[quick]") {
    WordGraph<size_t> ad   = path(20);
    word_type         path = {};
    for (size_t i = 0; i < 19; ++i) {
      path.push_back(0);
      REQUIRE(word_graph::last_node_on_path_no_checks(
                  ad, 0, path.cbegin(), path.cend())
                  .first
              == i + 1);
    }
    path.push_back(0);
    auto p = word_graph::last_node_on_path_no_checks(
        ad, 0, path.cbegin(), path.cend());
    REQUIRE(p.first == 19);
    REQUIRE(p.second == path.cend() - 1);
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph", "032", "to_string", "[quick]") {
    WordGraph<size_t> ad = path(6);
    REQUIRE(detail::to_string(ad) == "{{1}, {2}, {3}, {4}, {5}, {-}}");
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph", "033", "to_word_graph", "[quick]") {
    auto ad = to_word_graph<uint8_t>(5, {{0, 0}, {1, 1}, {2}, {3, 3}});
    REQUIRE(detail::to_string(ad)
            == "{{0, 0}, {1, 1}, {2, -}, {3, 3}, {-, -}}");
    REQUIRE_THROWS_AS(
        to_word_graph<uint8_t>(5, {{0, 0}, {1, 1, 1}, {2}, {3, 3}}),
        LibsemigroupsException);
    ad = WordGraph<uint8_t>(5, 2);
    REQUIRE(detail::to_string(ad)
            == "{{-, -}, {-, -}, {-, -}, {-, -}, {-, -}}");
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph", "034", "is_connected", "[quick]") {
    auto ad = to_word_graph<size_t>(5, {{0, 0}, {1, 1}, {2}, {3, 3}});
    REQUIRE(!word_graph::is_connected(ad));
    ad = path(1'000);
    REQUIRE(word_graph::is_connected(ad));
    REQUIRE(ad.number_of_nodes() == 1'000);
    word_graph::add_cycle(ad, 100);
    REQUIRE(ad.number_of_nodes() == 1'100);

    REQUIRE(!word_graph::is_connected(ad));
    ad.add_to_out_degree(1);
    ad.set_target(0, 1, 1'000);
    REQUIRE(word_graph::is_connected(ad));
    ad = WordGraph<size_t>();
    REQUIRE(word_graph::is_connected(ad));
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph", "035", "is_strictly_cyclic", "[quick]") {
    auto ad = to_word_graph<size_t>(5, {{0, 0}, {1, 1}, {2}, {3, 3}});
    REQUIRE(!word_graph::is_strictly_cyclic(ad));
    ad = path(1'000);
    REQUIRE(word_graph::is_strictly_cyclic(ad));
    REQUIRE(ad.number_of_nodes() == 1'000);
    word_graph::add_cycle(ad, 100);
    REQUIRE(ad.number_of_nodes() == 1'100);

    REQUIRE(!word_graph::is_strictly_cyclic(ad));
    ad.add_to_out_degree(1);
    ad.set_target(0, 1, 1'000);
    REQUIRE(word_graph::is_strictly_cyclic(ad));
    ad = WordGraph<size_t>();
    REQUIRE(word_graph::is_strictly_cyclic(ad));
  }

}  // namespace libsemigroups
