// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019 James D. Mitchell
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
#include <vector>   // for vector, vector<>::const_iterator, allocator

#include "catch.hpp"                   // for REQUIRE
#include "libsemigroups/iterator.hpp"  // for ConstIteratorStateless, ConstIteratorTraits
#include "test-main.hpp"               // for LIBSEMIGROUPS_TEST_CASE

namespace libsemigroups {

  struct TestEqualTo {
    bool operator()(std::vector<size_t>::const_iterator const& it1,
                    std::vector<size_t>::const_iterator        it2) {
      return it2 != it1;
    }
  };

  struct IteratorTraitsDefault
      : public detail::ConstIteratorTraits<std::vector<size_t>> {};

  struct IteratorTraitsCustomTypes1
      : public detail::ConstIteratorTraits<std::vector<size_t>> {
    using EqualTo = TestEqualTo;
  };

  LIBSEMIGROUPS_TEST_CASE("ConstIteratorStateless", "001", "?", "[quick]") {
    std::vector<size_t> vec(10, 0);

    auto it1
        = detail::ConstIteratorStateless<IteratorTraitsDefault>(vec.cbegin());
    auto it2
        = detail::ConstIteratorStateless<IteratorTraitsDefault>(vec.cbegin());
    REQUIRE(it1 == it2);
    REQUIRE(!(it1 != it2));
    auto it3 = detail::ConstIteratorStateless<IteratorTraitsDefault>(
        vec.cbegin() + 1);
    REQUIRE(!(it3 == it2));
    REQUIRE(it3 != it2);
  }

  LIBSEMIGROUPS_TEST_CASE("ConstIteratorStateless", "002", "?", "[quick]") {
    std::vector<size_t> vec(10, 0);

    auto it1 = detail::ConstIteratorStateless<IteratorTraitsCustomTypes1>(
        vec.cbegin());
    auto it2 = detail::ConstIteratorStateless<IteratorTraitsCustomTypes1>(
        vec.cbegin());
    REQUIRE(!(it1 == it2));
    auto it3 = detail::ConstIteratorStateless<IteratorTraitsCustomTypes1>(
        vec.cbegin() + 1);
    REQUIRE(it3 == it2);
  }
}  // namespace libsemigroups
