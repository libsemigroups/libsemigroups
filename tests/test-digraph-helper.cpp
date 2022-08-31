// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2020 James D. Mitchell, Maria Tsalakou
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

#include <algorithm>  // for all_of
#include <cstddef>    // for size_t
#include <cstdint>    // for uint8_t
#include <iterator>   // for reverse_iterator, operat...
#include <stdexcept>  // for runtime_error
#include <string>     // for basic_string, operator==
#include <utility>    // for pair
#include <vector>     // for vector, operator==

#include "catch.hpp"      // for REQUIRE, REQUIRE_NOTHROW, REQUIRE_THROWS_AS
#include "test-main.hpp"  // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/digraph-helper.hpp"  // for is_acyclic, topological_...
#include "libsemigroups/digraph.hpp"         // for ActionDigraph, operator<<
#include "libsemigroups/string.hpp"          // for to_string
#include "libsemigroups/types.hpp"           // for word_type

namespace libsemigroups {
  namespace {
    void add_path(ActionDigraph<size_t>& digraph, size_t n) {
      size_t old_nodes = digraph.number_of_nodes();
      digraph.add_nodes(n);
      for (size_t i = old_nodes; i < digraph.number_of_nodes() - 1; ++i) {
        digraph.add_edge(i, i + 1, 0);
      }
    }

    ActionDigraph<size_t> path(size_t n) {
      ActionDigraph<size_t> g(0, 1);
      add_path(g, n);
      return g;
    }

    void add_clique(ActionDigraph<size_t>& digraph, size_t n) {
      if (n != digraph.out_degree()) {
        throw std::runtime_error("can't do it!");
      }
      size_t old_nodes = digraph.number_of_nodes();
      digraph.add_nodes(n);

      for (size_t i = old_nodes; i < digraph.number_of_nodes(); ++i) {
        for (size_t j = old_nodes; j < digraph.number_of_nodes(); ++j) {
          digraph.add_edge(i, j, j - old_nodes);
        }
      }
    }

    ActionDigraph<size_t> clique(size_t n) {
      ActionDigraph<size_t> g(0, n);
      add_clique(g, n);
      return g;
    }
  }  // namespace

  LIBSEMIGROUPS_TEST_CASE("is_acyclic", "000", "2-cycle", "[quick]") {
    ActionDigraph<size_t> ad;
    ad.add_nodes(2);
    ad.add_to_out_degree(1);
    ad.add_edge(0, 1, 0);
    ad.add_edge(1, 0, 0);
    REQUIRE(!action_digraph_helper::is_acyclic(ad));
    REQUIRE(action_digraph_helper::topological_sort(ad).empty());
  }

  LIBSEMIGROUPS_TEST_CASE("is_acyclic", "001", "1-cycle", "[quick]") {
    ActionDigraph<size_t> ad;
    ad.add_nodes(1);
    ad.add_to_out_degree(1);
    ad.add_edge(0, 0, 0);
    REQUIRE(!action_digraph_helper::is_acyclic(ad));
    REQUIRE(action_digraph_helper::topological_sort(ad).empty());
  }

  LIBSEMIGROUPS_TEST_CASE("is_acyclic", "002", "multi-digraph", "[quick]") {
    using node_type = ActionDigraph<size_t>::node_type;
    ActionDigraph<size_t> ad;
    ad.add_nodes(2);
    ad.add_to_out_degree(2);
    ad.add_edge(0, 1, 0);
    ad.add_edge(0, 1, 1);
    REQUIRE(action_digraph_helper::is_acyclic(ad));
    REQUIRE(action_digraph_helper::topological_sort(ad)
            == std::vector<node_type>({1, 0}));
  }

  LIBSEMIGROUPS_TEST_CASE("is_acyclic",
                          "003",
                          "complete digraph 100",
                          "[quick]") {
    ActionDigraph<size_t> ad;
    size_t const          n = 100;
    ad.add_nodes(n);
    ad.add_to_out_degree(n);
    for (size_t i = 0; i < n; ++i) {
      for (size_t j = 0; j < n; ++j) {
        if (i != j) {
          ad.add_edge(i, j, j);
        }
      }
    }
    REQUIRE(!action_digraph_helper::is_acyclic(ad));
    REQUIRE(action_digraph_helper::topological_sort(ad).empty());
  }

  LIBSEMIGROUPS_TEST_CASE("is_acyclic",
                          "004",
                          "acyclic digraph with 20000 nodes",
                          "[quick]") {
    ActionDigraph<size_t> ad;
    size_t const          n = 20000;
    ad.add_nodes(n);
    ad.add_to_out_degree(2);
    for (size_t i = 0; i < (n / 2 - 1); ++i) {
      ad.add_edge(i, i + 1, 0);
    }
    ad.add_edge(n / 2 - 1, n - 1, 1);
    ad.add_edge(n / 2 + 1, (3 * n) / 4 - 1, 1);
    ad.add_edge(n / 2, 0, 1);
    for (size_t i = n / 2; i < n - 1; ++i) {
      ad.add_edge(i, i + 1, 0);
    }
    REQUIRE(action_digraph_helper::is_acyclic(ad));
    REQUIRE(action_digraph_helper::topological_sort(ad).size()
            == ad.number_of_nodes());
  }

  LIBSEMIGROUPS_TEST_CASE("is_acyclic",
                          "005",
                          "acyclic digraph with 10 million nodes",
                          "[standard]") {
    ActionDigraph<size_t> ad;
    size_t const          n = 10000000;
    ad.add_nodes(n);
    ad.add_to_out_degree(2);
    for (size_t i = 0; i < (n / 2 - 1); ++i) {
      ad.add_edge(i, i + 1, 0);
    }
    ad.add_edge(n / 2 - 1, n - 1, 1);
    ad.add_edge(n / 2 + 1, (3 * n) / 4 - 1, 1);
    ad.add_edge(n / 2, 0, 1);
    for (size_t i = n / 2; i < n - 1; ++i) {
      ad.add_edge(i, i + 1, 0);
    }
    REQUIRE(action_digraph_helper::is_acyclic(ad));
    REQUIRE(action_digraph_helper::topological_sort(ad).size() == n);
  }

  LIBSEMIGROUPS_TEST_CASE("is_acyclic", "006", "for a node", "[quick]") {
    using node_type = ActionDigraph<size_t>::node_type;
    ActionDigraph<size_t> ad;
    size_t const          n = 100;
    ad.add_nodes(n);
    ad.add_to_out_degree(2);
    for (size_t i = 0; i < n - 1; ++i) {
      ad.add_edge(i, i + 1, i % 2);
    }
    action_digraph_helper::add_cycle(ad, 100);

    REQUIRE(std::all_of(
        ad.cbegin_nodes(), ad.cbegin_nodes() + 100, [&ad](node_type const& v) {
          return action_digraph_helper::is_acyclic(ad, v);
        }));

    REQUIRE(std::none_of(ad.crbegin_nodes(),
                         ad.crbegin_nodes() + 100,
                         [&ad](node_type const& v) {
                           return action_digraph_helper::is_acyclic(ad, v);
                         }));
    REQUIRE(!action_digraph_helper::is_acyclic(ad));
    REQUIRE(action_digraph_helper::is_acyclic(ad, 10, 20));
    REQUIRE(!action_digraph_helper::is_acyclic(ad, 100, 120));
    REQUIRE(action_digraph_helper::is_acyclic(ad, 10, 120));
  }

  LIBSEMIGROUPS_TEST_CASE("is_acyclic", "007", "for a node", "[quick]") {
    ActionDigraph<size_t> ad;
    using node_type = decltype(ad)::node_type;
    ad.add_nodes(4);
    ad.add_to_out_degree(1);
    ad.add_edge(0, 1, 0);
    ad.add_edge(1, 0, 0);
    ad.add_edge(2, 3, 0);
    REQUIRE(!action_digraph_helper::is_acyclic(ad));
    REQUIRE(action_digraph_helper::topological_sort(ad).empty());
    REQUIRE(!action_digraph_helper::is_acyclic(ad, 0));
    REQUIRE(action_digraph_helper::topological_sort(ad, 0).empty());
    REQUIRE(!action_digraph_helper::is_acyclic(ad, 1));
    REQUIRE(action_digraph_helper::topological_sort(ad, 1).empty());

    REQUIRE(action_digraph_helper::is_acyclic(ad, 2));
    REQUIRE(action_digraph_helper::topological_sort(ad, 2)
            == std::vector<node_type>({3, 2}));
    REQUIRE(action_digraph_helper::is_acyclic(ad, 3));
    REQUIRE(action_digraph_helper::topological_sort(ad, 3)
            == std::vector<node_type>({3}));
  }

  LIBSEMIGROUPS_TEST_CASE("is_reachable",
                          "008",
                          "acyclic 20 node digraph",
                          "[quick]") {
    ActionDigraph<size_t> ad;
    size_t const          n = 20;
    ad.add_nodes(n);
    ad.add_to_out_degree(2);
    for (size_t i = 0; i < (n / 2 - 1); ++i) {
      ad.add_edge(i, i + 1, 0);
    }
    ad.add_edge(n / 2 - 1, n - 1, 1);
    ad.add_edge(n / 2 + 1, (3 * n) / 4 - 1, 1);
    ad.add_edge(n / 2, 0, 1);
    for (size_t i = n / 2; i < n - 1; ++i) {
      ad.add_edge(i, i + 1, 0);
    }
    REQUIRE(!action_digraph_helper::is_reachable(ad, 1, 10));
    REQUIRE(action_digraph_helper::is_reachable(ad, 10, 1));
    REQUIRE_THROWS_AS(action_digraph_helper::is_reachable(ad, 20, 1),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(action_digraph_helper::is_reachable(ad, 1, 20),
                      LibsemigroupsException);
    REQUIRE(action_digraph_helper::is_reachable(ad, 1, 1));
  }

  LIBSEMIGROUPS_TEST_CASE("is_reachable", "009", "100 node path", "[quick]") {
    ActionDigraph<size_t> ad;
    size_t const          n = 100;
    ad.add_nodes(n);
    ad.add_to_out_degree(2);
    for (size_t i = 0; i < n - 1; ++i) {
      ad.add_edge(i, i + 1, i % 2);
    }
    for (auto it1 = ad.cbegin_nodes(); it1 < ad.cend_nodes(); ++it1) {
      for (auto it2 = it1 + 1; it2 < ad.cend_nodes(); ++it2) {
        REQUIRE(action_digraph_helper::is_reachable(ad, *it1, *it2));
        REQUIRE(!action_digraph_helper::is_reachable(ad, *it2, *it1));
      }
    }
  }

  LIBSEMIGROUPS_TEST_CASE("is_reachable", "010", "100 node cycle", "[quick]") {
    ActionDigraph<size_t> ad;
    ad.add_to_out_degree(1);
    action_digraph_helper::add_cycle(ad, 100);
    for (auto it1 = ad.cbegin_nodes(); it1 < ad.cend_nodes(); ++it1) {
      for (auto it2 = it1 + 1; it2 < ad.cend_nodes(); ++it2) {
        REQUIRE(action_digraph_helper::is_reachable(ad, *it1, *it2));
        REQUIRE(action_digraph_helper::is_reachable(ad, *it2, *it1));
      }
    }
  }

  LIBSEMIGROUPS_TEST_CASE("is_reachable", "011", "20 node clique", "[quick]") {
    ActionDigraph<size_t> ad = clique(20);
    for (auto it1 = ad.cbegin_nodes(); it1 < ad.cend_nodes(); ++it1) {
      for (auto it2 = it1 + 1; it2 < ad.cend_nodes(); ++it2) {
        REQUIRE(action_digraph_helper::is_reachable(ad, *it1, *it2));
        REQUIRE(action_digraph_helper::is_reachable(ad, *it2, *it1));
      }
    }
    REQUIRE(ad.validate());
    REQUIRE(action_digraph_helper::topological_sort(ad).empty());
    REQUIRE(action_digraph_helper::topological_sort(ad, 0).empty());
  }

  LIBSEMIGROUPS_TEST_CASE("follow_path", "012", "20 node path", "[quick]") {
    ActionDigraph<size_t> ad = path(20);
    for (auto it = ad.cbegin_panilo(0); it != ad.cend_panilo(); ++it) {
      REQUIRE(action_digraph_helper::follow_path(ad, 0, it->first)
              == it->second);
      REQUIRE(action_digraph_helper::follow_path_nc(ad, 0, it->first)
              == it->second);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("validate_label", "013", "20 node path", "[quick]") {
    ActionDigraph<size_t> ad = path(20);
    REQUIRE_THROWS_AS(action_digraph_helper::validate_label(ad, 10),
                      LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("last_node_on_path_nc",
                          "014",
                          "20 node path",
                          "[quick]") {
    ActionDigraph<size_t> ad   = path(20);
    word_type             path = {};
    for (size_t i = 0; i < 19; ++i) {
      path.push_back(0);
      REQUIRE(action_digraph_helper::last_node_on_path_nc(
                  ad, 0, path.cbegin(), path.cend())
                  .first
              == i + 1);
    }
    path.push_back(0);
    auto p = action_digraph_helper::last_node_on_path_nc(
        ad, 0, path.cbegin(), path.cend());
    REQUIRE(p.first == 19);
    REQUIRE(p.second == path.cend() - 1);
  }

  LIBSEMIGROUPS_TEST_CASE("action_digraph_helper",
                          "015",
                          "detail::to_string",
                          "[quick]") {
    ActionDigraph<size_t> ad = path(6);
    REQUIRE(detail::to_string(ad) == "{{1}, {2}, {3}, {4}, {5}, {-}}");
    REQUIRE(action_digraph_helper::detail::to_string(ad)
            == R"V0G0N(ActionDigraph<size_t> ad;
ad.add_nodes(6);
ad.add_to_out_degree(1);
ad.add_edge(0, 1, 0);
ad.add_edge(1, 2, 0);
ad.add_edge(2, 3, 0);
ad.add_edge(3, 4, 0);
ad.add_edge(4, 5, 0);
)V0G0N");
  }

  LIBSEMIGROUPS_TEST_CASE("action_digraph_helper", "016", "make", "[quick]") {
    auto ad = action_digraph_helper::make<uint8_t>(
        5, {{0, 0}, {1, 1}, {2}, {3, 3}});
    REQUIRE(detail::to_string(ad)
            == "{{0, 0}, {1, 1}, {2, -}, {3, 3}, {-, -}}");
    REQUIRE_THROWS_AS(action_digraph_helper::make<uint8_t>(
                          5, {{0, 0}, {1, 1, 1}, {2}, {3, 3}}),
                      LibsemigroupsException);
    ad = ActionDigraph<uint8_t>(5, 2);
    REQUIRE(detail::to_string(ad)
            == "{{-, -}, {-, -}, {-, -}, {-, -}, {-, -}}");
  }

  LIBSEMIGROUPS_TEST_CASE("action_digraph_helper",
                          "017",
                          "is_connected",
                          "[quick]") {
    auto ad
        = action_digraph_helper::make<size_t>(5, {{0, 0}, {1, 1}, {2}, {3, 3}});
    REQUIRE(!action_digraph_helper::is_connected(ad));
    ad = path(1'000);
    REQUIRE(action_digraph_helper::is_connected(ad));
    REQUIRE(ad.number_of_nodes() == 1'000);
    action_digraph_helper::add_cycle(ad, 100);
    REQUIRE(ad.number_of_nodes() == 1'100);

    REQUIRE(!action_digraph_helper::is_connected(ad));
    ad.add_to_out_degree(1);
    ad.add_edge(0, 1'000, 1);
    REQUIRE(action_digraph_helper::is_connected(ad));
    ad = ActionDigraph<size_t>();
    REQUIRE(action_digraph_helper::is_connected(ad));
  }

  LIBSEMIGROUPS_TEST_CASE("action_digraph_helper",
                          "018",
                          "is_strictly_cyclic",
                          "[quick]") {
    auto ad
        = action_digraph_helper::make<size_t>(5, {{0, 0}, {1, 1}, {2}, {3, 3}});
    REQUIRE(!action_digraph_helper::is_strictly_cyclic(ad));
    ad = path(1'000);
    REQUIRE(action_digraph_helper::is_strictly_cyclic(ad));
    REQUIRE(ad.number_of_nodes() == 1'000);
    action_digraph_helper::add_cycle(ad, 100);
    REQUIRE(ad.number_of_nodes() == 1'100);

    REQUIRE(!action_digraph_helper::is_strictly_cyclic(ad));
    ad.add_to_out_degree(1);
    ad.add_edge(0, 1'000, 1);
    REQUIRE(action_digraph_helper::is_strictly_cyclic(ad));
    ad = ActionDigraph<size_t>();
    REQUIRE(action_digraph_helper::is_strictly_cyclic(ad));
  }
}  // namespace libsemigroups
