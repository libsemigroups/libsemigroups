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

#include "catch_amalgamated.hpp"  // for REQUIRE, REQUIRE_THROWS_AS, REQUI...
#include "test-main.hpp"          // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/word-graph-view.hpp"

namespace libsemigroups {
  struct LibsemigroupsException;

  LIBSEMIGROUPS_TEST_CASE("WordGraphView",
                          "000",
                          "empty constructor",
                          "[quick]") {
    WordGraphView<size_t> g;
    REQUIRE(g.number_of_nodes() == 0);
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

  LIBSEMIGROUPS_TEST_CASE("WordGraphView", "002", "copy ctor", "[quick]") {
    WordGraph<size_t>     g(10, 5);
    WordGraphView<size_t> v(g, 2, 5);
    WordGraphView<size_t> v1 = v;
    REQUIRE(v1.number_of_nodes() == v.number_of_nodes());
    REQUIRE(v1 == v);
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraphView", "003", "move ctor", "[quick]") {
    WordGraph<size_t>     g(10, 5);
    WordGraphView<size_t> v(g, 2, 5);
    WordGraphView<size_t> v1(std::move(v));
    REQUIRE(v1.number_of_nodes() == v.number_of_nodes());
    REQUIRE(v1 == v);
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraphView",
                          "004",
                          "test offset correct",
                          "[quick]") {
                            WordGraph<size_t>     g(10, 5);
                            g.target(2, 3, 5);
                            g.target(3, 4, 5);
                            WordGraphView<size_t> v(g, 2, 5);
                            REQUIRE(v.target(0, 3) == 3);
                            REQUIRE(v.target(1, 4) == 3);
                            REQUIRE(v.target(2, 3) == UNDEFINED);
                          }
LIBSEMIGROUPS_TEST_CASE("WordGraphView", "005", "test target(s, a) throws", "[quick]") {
  WordGraph<size_t>     g(10, 5);
                            g.target(2, 3, 5);
                            g.target(3, 4, 5);
                            WordGraphView<size_t> v(g, 2, 5);
                            REQUIRE_THROWS_AS(v.target(5, 3), LibsemigroupsException);
}

}  // namespace libsemigroups