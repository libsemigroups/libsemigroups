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

#include <cstddef>    // for size_t
#include <stdexcept>  // for runtime_error

#include "catch_amalgamated.hpp"  // for REQUIRE, REQUIRE_THROWS_AS, REQUI...
#include "test-main.hpp"          // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/word-graph-view.hpp"

namespace libsemigroups {
  struct LibsemigroupsException;

  LIBSEMIGROUPS_TEST_CASE("WordGraphView",
                          "000",
                          "empty constructor",
                          "[quick][digraph]") {
    WordGraphView<size_t> g;
    REQUIRE(g.number_of_nodes() == 0);
    REQUIRE(g.graph == nullptr);
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraphView",
                          "001",
                          "can access graph",
                          "[quick][digraph]") {
    WordGraph<size_t> g;
    g.add_nodes(17);
    WordGraphView<size_t> v(&g, 0, 17);
    REQUIRE(*v.graph == g);
    REQUIRE(g.number_of_nodes() == 17);
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraphView",
                          "002",
                          "number of nodes reported correctly",
                          "[quick][digraph]") {
    WordGraph<size_t> g;
    g.add_nodes(17);
    WordGraphView<size_t> v(&g, 3, 14);
    REQUIRE(v.number_of_nodes() == 11);
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraphView",
                          "003",
                          "exceptions thrown correctly",
                          "[quick][digraph]") {
    WordGraph<size_t> g;
    g.add_nodes(17);
    WordGraphView<size_t> v(&g, 3, 14);
    REQUIRE_THROWS_AS(v.cbegin_targets(16), LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraphView",
                          "004",
                          "node iterator range correct",
                          "[quick][digraph]") {
    WordGraph<size_t> g;
    g.add_nodes(17);
    WordGraphView<size_t> v(&g, 3, 14);
    auto                  begin = v.cbegin_nodes();
    auto                  end   = v.cend_nodes();
    REQUIRE(std::vector<size_t>(begin, end)
            == std::vector<size_t>({3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13}));
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraphView",
                          "005",
                          "oob target throws",
                          "[quick][digraph]") {
    WordGraph<size_t> g;
    g.add_nodes(17);
    WordGraphView<size_t> v(&g, 3, 14);
    REQUIRE_THROWS_AS(v.cbegin_targets(2), LibsemigroupsException);
    REQUIRE_THROWS_AS(v.cbegin_targets(15), LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraphView",
                          "006",
                          "get targets",
                          "[quick][digraph]") {
    WordGraph<size_t> g;
    g.add_nodes(17);
    g.add_to_out_degree(5);
    g.target(3, 0, 4);
    g.target(3, 1, 5);
    g.target(3, 2, 6);
    WordGraphView<size_t> v(&g, 3, 14);
    auto                  targets = v.targets(*v.cbegin_nodes());
    for (size_t i = 4; i < 6; i++) {
      REQUIRE(targets.get() == i);
      targets.advance_by(1);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraphView",
                          "007",
                          "get targets throws",
                          "[quick][digraph]") {
    WordGraph<size_t> g;
    g.add_nodes(17);
    g.add_to_out_degree(5);
    g.target(3, 0, 4);
    g.target(3, 1, 5);
    g.target(3, 2, 6);
    WordGraphView<size_t> v(&g, 3, 14);
    REQUIRE_THROWS_AS(v.targets(1), LibsemigroupsException);
  }

}  // namespace libsemigroups