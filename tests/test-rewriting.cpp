// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019-2023 Joe Edwards
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

#include "catch.hpp"      // for AssertionHandler, ope...
#include "test-main.hpp"  // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/aho-corasick.hpp"  // for dot
#include "libsemigroups/knuth-bendix.hpp"  // for KnuthBendix

namespace libsemigroups {
  using namespace std::literals;
  LIBSEMIGROUPS_TEST_CASE("RewriteTrie", "000", "initial test", "[quick]") {
    KnuthBendix::RewriteTrie rt;
    REQUIRE(rt.number_of_active_rules() == 0);
    rt.add_rule("ba"s, "a"s);
    REQUIRE(rt.number_of_active_rules() == 1);
  }

  LIBSEMIGROUPS_TEST_CASE("RewriteTrie", "001", "simple test", "[quick]") {
    KnuthBendix::RewriteTrie rt = KnuthBendix::RewriteTrie();

    rt.add_rule("ac"s, "ca"s);
    rt.add_rule("aa"s, "a"s);
    rt.add_rule("ac"s, "a"s);
    rt.add_rule("ca"s, "a"s);
    rt.add_rule("bb"s, "bb"s);
    rt.add_rule("bc"s, "cb"s);
    rt.add_rule("bbb"s, "b"s);
    rt.add_rule("bc"s, "b"s);
    rt.add_rule("cb"s, "b"s);
    rt.add_rule("a"s, "b"s);

    // REQUIRE(rt.confluent());
  }
}  // namespace libsemigroups