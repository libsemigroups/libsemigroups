// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2025 James Mitchell
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

#include "Catch2-3.8.0/catch_amalgamated.hpp"  // for REQUIRE, REQUIRE_NOTHROW, REQUIRE_THROWS_AS
#include "test-main.hpp"                       // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/to-word-graph.hpp"
#include "libsemigroups/word-graph-helpers.hpp"  // for spanning_tree
#include "word-graph-test-common.hpp"            // for binary_tree

namespace libsemigroups {

  LIBSEMIGROUPS_TEST_CASE("to<WordGraph>", "000", "from a Forest", "[quick]") {
    using node_type = Forest::node_type;

    WordGraph wg = binary_tree<node_type>(5);
    Forest    f  = v4::word_graph::spanning_tree(wg, 0);
    REQUIRE(to<WordGraph>(f) == wg);
  }
}  // namespace libsemigroups
