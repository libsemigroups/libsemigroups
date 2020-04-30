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

#include "catch.hpp"  // for REQUIRE, REQUIRE_NOTHROW, REQUIRE_THROWS_AS

#include "libsemigroups/digraph-helper.hpp"  // for is_acyclic
#include "libsemigroups/digraph.hpp"         // for ActionDigraph

#include "test-main.hpp"  // for LIBSEMIGROUPS_TEST_CASE

namespace libsemigroups {

  LIBSEMIGROUPS_TEST_CASE("action_digraph_helper::is_acyclic",
                          "000",
                          "2-cycle",
                          "[quick]") {
    ActionDigraph<size_t> ad;
    ad.add_nodes(2);
    ad.add_to_out_degree(1);
    ad.add_edge(0, 1, 0);
    ad.add_edge(1, 0, 0);
    REQUIRE(!action_digraph_helper::is_acyclic(ad));
  }

  LIBSEMIGROUPS_TEST_CASE("action_digraph_helper::is_acyclic",
                          "001",
                          "1-cycle",
                          "[quick]") {
    ActionDigraph<size_t> ad;
    ad.add_nodes(1);
    ad.add_to_out_degree(1);
    ad.add_edge(0, 0, 0);
    REQUIRE(!action_digraph_helper::is_acyclic(ad));
  }

  LIBSEMIGROUPS_TEST_CASE("action_digraph_helper::is_acyclic",
                          "002",
                          "multi-digraph",
                          "[quick]") {
    ActionDigraph<size_t> ad;
    ad.add_nodes(2);
    ad.add_to_out_degree(2);
    ad.add_edge(0, 1, 0);
    ad.add_edge(0, 1, 1);
    REQUIRE(action_digraph_helper::is_acyclic(ad));
  }

  LIBSEMIGROUPS_TEST_CASE("action_digraph_helper::is_acyclic",
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
  }

  LIBSEMIGROUPS_TEST_CASE("action_digraph_helper::is_acyclic",
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
  }

  LIBSEMIGROUPS_TEST_CASE("action_digraph_helper::is_acyclic",
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
  }

  LIBSEMIGROUPS_TEST_CASE("follow_path", "011", "20 node clique", "[quick]") {
    ActionDigraph<size_t> ad = path(20);
    for (auto it = ad.cbegin_panilo(0); it != ad.cend_panilo(); ++it) {
      REQUIRE(action_digraph_helper::follow_path(ad, 0, it->first) == it->second);
    }
  }
}  // namespace libsemigroups
