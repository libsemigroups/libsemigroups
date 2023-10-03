
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2023 Joe Edwards + James D. Mitchell
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

#include <fstream>

#include "libsemigroups/aho-corasick.hpp"  // for AhoCorasick

#include "catch.hpp"      // for REQUIRE, REQUIRE_THROWS_AS, REQUI...
#include "test-main.hpp"  // for LIBSEMIGROUPS_TEST_CASE

namespace libsemigroups {
  using namespace literals;

  LIBSEMIGROUPS_TEST_CASE("AhoCorasick",
                          "000",
                          "initial test",
                          "[quick][aho-corasick]") {
    AhoCorasick ac;
    ac.add_word_no_checks(00101_w);
    ac.add_word_no_checks(010_w);

    REQUIRE(ac.number_of_nodes() == 8);
    REQUIRE(ac.traverse(00101_w) == 5);
    REQUIRE(ac.traverse(010_w) == 7);

    std::ofstream file("aho.gv");
    file << dot(ac).to_string();
  }

}  // namespace libsemigroups
