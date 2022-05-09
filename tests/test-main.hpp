//
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

#ifndef LIBSEMIGROUPS_TESTS_TEST_MAIN_HPP_
#define LIBSEMIGROUPS_TESTS_TEST_MAIN_HPP_

#define STR2(X) #X
#define STR(X) STR2(X)

#define LIBSEMIGROUPS_TEST_CASE(classname, nr, msg, tags)             \
  TEST_CASE(classname " " nr ": " msg,                                \
            "[" classname " " nr "][" classname "][" nr "][" __FILE__ \
            "][" STR(__LINE__) "]" tags)

namespace libsemigroups {

  template <typename T>
  void verify_forward_iterator_requirements(T it) {
    using deref_type = decltype(*it);
    REQUIRE_NOTHROW(*it);
    REQUIRE(std::is_reference<deref_type>::value);
    REQUIRE(
        std::is_const<typename std::remove_reference<deref_type>::type>::value);
    T copy(it);
    REQUIRE(&copy != &it);
    it++;
    auto it_val   = *it;
    auto copy_val = *copy;
    std::swap(it, copy);
    REQUIRE(copy_val == *it);
    REQUIRE(it_val == *copy);

    it.swap(copy);
    REQUIRE(it_val == *it);
    REQUIRE(copy_val == *copy);

    ++copy;
    REQUIRE(*it == *copy);

    ++it;
    copy++;
    REQUIRE(*it == *copy);
  }
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_TESTS_TEST_MAIN_HPP_
