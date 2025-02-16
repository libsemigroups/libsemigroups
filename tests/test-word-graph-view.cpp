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

#include "catch_amalgamated.hpp"  // for REQUIRE, REQUIRE_THROWS_AS, REQUI...
#include "test-main.hpp"          // for LIBSEMIGROUPS_TEST_CASE
#include "word-graph-test-common.hpp"  // for add_clique etc

#include "libsemigroups/forest.hpp"  // for Forest
#include "libsemigroups/paths.hpp"   // for cbegin_pilo
#include "libsemigroups/word-graph-view.hpp"
#include "libsemigroups/word-graph.hpp"  // for WordGraph
#include "libsemigroups/word-range.hpp"  // for literalsR, WordRange

#include "libsemigroups/detail/string.hpp"  // for detail::to_string

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
    WordGraph<size_t>     g(17, 31);
    WordGraphView<size_t> v(g, 0, 31);
    REQUIRE(*v.graph == g);
    REQUIRE(g.number_of_nodes() == 17);
  }

}  // namespace libsemigroups