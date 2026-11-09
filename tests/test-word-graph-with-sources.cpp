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

#include <cstdint>  // for uint32_t

#include "libsemigroups/constants.hpp"                       // for UNDEFINED
#include "libsemigroups/detail/word-graph-with-sources.hpp"  // for WordGraphWithSources

namespace libsemigroups {
  LIBSEMIGROUPS_TEST_CASE("WordGraphWithSources",
                          "000",
                          "induced_subgraph_no_checks",
                          "[word-graph-with-sources][quick]") {
    detail::WordGraphWithSources<uint32_t> graph(3, 1);
    graph.target_no_checks(1, 0, 2);
    graph.target_no_checks(0, 0, 2);

    graph.induced_subgraph_no_checks(1, 3);

    REQUIRE(graph.number_of_nodes() == 2);
    REQUIRE(graph.target_no_checks(0, 0) == 1);
    REQUIRE(graph.target_no_checks(1, 0) == UNDEFINED);
    REQUIRE(graph.first_source_no_checks(1, 0) == 0);
    REQUIRE(graph.next_source_no_checks(0, 0) == UNDEFINED);
  }
}  // namespace libsemigroups
