//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2023 James D. Mitchell
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

#define CATCH_CONFIG_ENABLE_PAIR_STRINGMAKER

#include "catch.hpp"      // for operator""_catch_sr
#include "test-main.hpp"  // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/ranges.hpp"  // for ChainRange, get_range...

#include "libsemigroups/ranges.hpp"  // for chain, shortlex_compare

namespace libsemigroups {
  namespace {
    template <typename Range>
    std::string to_string(Range r) {
      std::string result;
      while (!r.at_end()) {
        result += r.get();
        r.next();
      }
      return result;
    }
  }  // namespace

  struct LibsemigroupsException;  // forward decl
                                  //
  LIBSEMIGROUPS_TEST_CASE("Ranges", "000", "chain", "[quick][presentation]") {
    std::string prefix1 = "dabd", suffix1 = "cbb", prefix2 = "abbaba",
                suffix2 = "c";

    REQUIRE(to_string(chain(prefix1, suffix1)) == prefix1 + suffix1);
    REQUIRE(to_string(chain(prefix2, suffix2)) == prefix2 + suffix2);
  }

  LIBSEMIGROUPS_TEST_CASE("Ranges", "001", "skip_n", "[quick]") {
    auto   sequence      = (rx::seq() | rx::take(100));
    size_t no_skip_count = (sequence | rx::count());
    size_t skip_count    = (sequence | rx::skip_n(10) | rx::count());
    REQUIRE(no_skip_count == skip_count + 10);
  }

  LIBSEMIGROUPS_TEST_CASE("Ranges", "002", "next", "[quick]") {
    auto sequence = (rx::seq() | rx::take(100));

    size_t no_next_count = (sequence | rx::count());
    for (size_t i = 0; i < 10; ++i) {
      sequence.next();
    }

    size_t next_count = (sequence | rx::count());
    REQUIRE(no_next_count == next_count + 10);
  }
}  // namespace libsemigroups
