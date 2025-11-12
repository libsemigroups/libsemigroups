// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2025 James D. Mitchell
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

#include <initializer_list>  // for initializer_list
#include <string>            // for string

#include "Catch2-3.8.0/catch_amalgamated.hpp"  // for REQUIRE
#include "test-main.hpp"                       // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/exception.hpp"  // for LibsemigroupsException
#include "libsemigroups/is-transf.hpp"  // for is_transf etc

namespace libsemigroups {

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("find_duplicates",
                                   "000",
                                   "exceptions",
                                   "[quick]",
                                   std::vector<uint32_t>,
                                   std::string,
                                   (std::array<uint8_t, 16>),
                                   std::initializer_list<size_t>) {
    TestType vec = {0, 1, 12, 1, 13, 1, 3, 3, 13, 13, 1, 41, 4, 41, 14, 4};

    auto [it1, pos1] = detail::find_duplicates(vec.begin(), vec.end());
    REQUIRE(it1 == vec.begin() + 3);
    REQUIRE(pos1 == 1);
    REQUIRE(detail::has_duplicates(vec.begin(), vec.end()));

    auto [it2, pos2] = detail::find_duplicates(vec.begin(), vec.begin() + 3);
    REQUIRE(it2 == vec.begin() + 3);
    REQUIRE(pos2 == 3);
    REQUIRE(!detail::has_duplicates(vec.begin(), vec.begin() + 3));
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("throw_if_duplicates",
                                   "001",
                                   "exceptions",
                                   "[quick]",
                                   std::vector<uint32_t>,
                                   (std::array<uint8_t, 16>),
                                   std::initializer_list<size_t>) {
    TestType vec = {0, 1, 12, 1, 13, 1, 3, 3, 13, 13, 1, 41, 4, 41, 14, 4};

    REQUIRE_EXCEPTION_MSG(
        detail::throw_if_duplicates(vec.begin(), vec.end(), "vector"),
        "duplicate vector value, found 1 in position "
        "3, first occurrence in position 1");
    REQUIRE_NOTHROW(
        detail::throw_if_duplicates(vec.begin(), vec.begin() + 3, "vector"));
  }

  LIBSEMIGROUPS_TEST_CASE("throw_if_duplicates",
                          "002",
                          "exceptions - std::string",
                          "[quick]") {
    std::string vec = {0, 1, 12, 1, 13, 1, 3, 3, 13, 13, 1, 41, 4, 41, 14, 4};

    REQUIRE_EXCEPTION_MSG(
        detail::throw_if_duplicates(vec.begin(), vec.end(), "vector"),
        "duplicate vector value, found (char with value) 1 in position "
        "3, first occurrence in position 1");
    REQUIRE_NOTHROW(
        detail::throw_if_duplicates(vec.begin(), vec.begin() + 3, "vector"));
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("throw_if_not_ptransf",
                                   "003",
                                   "for image",
                                   "[quick]",
                                   std::vector<uint32_t>,
                                   (std::array<uint8_t, 16>),
                                   std::initializer_list<size_t>) {
    // No std::string here because we static_assert that the integer values in
    // the container are unsigned.
    TestType vec = {0, 1, 12, 1, 13, 1, 3, 3, 13, 13, 1, 41, 4, 41, 14, 4};
    REQUIRE(vec.size() == 16);
    REQUIRE_NOTHROW(detail::throw_if_not_ptransf(vec.begin(), vec.end(), 42));
    REQUIRE_EXCEPTION_MSG(detail::throw_if_not_ptransf(vec.begin(), vec.end()),
                          "image value out of bounds, expected value in "
                          "[0, 16), found 41 in position 11");
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("throw_if_not_ptransf",
                                   "004",
                                   "for dom. and img.",
                                   "[quick]",
                                   std::vector<uint32_t>,
                                   (std::array<uint8_t, 16>),
                                   std::initializer_list<size_t>) {
    TestType vec
        = {0, 1, 12, 1, 13, 1, 3, 3, UNDEFINED, 13, 1, 41, 4, 41, 14, 4};
    REQUIRE_NOTHROW(detail::throw_if_not_ptransf(
        vec.begin(), vec.begin() + 3, vec.begin() + 3, vec.begin() + 6, 16));
    REQUIRE_EXCEPTION_MSG(
        detail::throw_if_not_ptransf(
            vec.begin(), vec.begin() + 3, vec.begin() + 3, vec.begin() + 4, 16),
        "domain and image size mismatch, domain has "
        "size 3 but image has size 1");
    REQUIRE_EXCEPTION_MSG(
        detail::throw_if_not_ptransf(vec.begin() + 7,
                                     vec.begin() + 9,
                                     vec.begin() + 3,
                                     vec.begin() + 5,
                                     16),
        fmt::format("the 1st argument (domain) must not contain "
                    "UNDEFINED, but found UNDEFINED (= {}) in position 1",
                    static_cast<typename TestType::value_type>(UNDEFINED)));
    REQUIRE_EXCEPTION_MSG(
        detail::throw_if_not_ptransf(vec.begin() + 3,
                                     vec.begin() + 5,
                                     vec.begin() + 7,
                                     vec.begin() + 9,
                                     16),
        fmt::format("the 2nd argument (image) must not contain "
                    "UNDEFINED, but found UNDEFINED (= {}) in position 1",
                    static_cast<typename TestType::value_type>(UNDEFINED)));
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("throw_if_not_transf",
                                   "005",
                                   "for image",
                                   "[quick]",
                                   std::vector<uint32_t>,
                                   (std::array<uint8_t, 16>),
                                   std::initializer_list<size_t>) {
    TestType vec = {0, 1, 12, 1, 13, 1, 3, 3, 13, 13, 1, 41, 4, 41, 14, 4};
    REQUIRE(vec.size() == 16);
    REQUIRE_NOTHROW(detail::throw_if_not_transf(vec.begin(), vec.end(), 42));
    REQUIRE_EXCEPTION_MSG(detail::throw_if_not_transf(vec.begin(), vec.end()),
                          "image value out of bounds, expected value in "
                          "[0, 16), found 41 in position 11");
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("throw_if_not_perm",
                                   "006",
                                   "for image",
                                   "[quick]",
                                   std::vector<uint32_t>,
                                   (std::array<uint8_t, 16>),
                                   std::initializer_list<size_t>) {
    TestType vec = {0, 1, 12, 1, 13, 1, 3, 3, 13, 13, 1, 41, 4, 41, 14, 4};
    REQUIRE(vec.size() == 16);
    REQUIRE_NOTHROW(detail::throw_if_not_perm(vec.begin(), vec.begin() + 2, 2));
    REQUIRE_NOTHROW(detail::throw_if_not_perm(vec.begin(), vec.begin() + 2));
    REQUIRE_EXCEPTION_MSG(detail::throw_if_not_perm(vec.begin(), vec.end(), 42),
                          "duplicate image value, found 1 in position 3, "
                          "first occurrence in position 1");
    REQUIRE_EXCEPTION_MSG(detail::throw_if_not_perm(vec.begin(), vec.end()),
                          "image value out of bounds, expected value in "
                          "[0, 16), found 41 in position 11");
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("throw_if_not_pperm",
                                   "007",
                                   "for image",
                                   "[quick]",
                                   std::vector<uint32_t>,
                                   (std::array<uint8_t, 16>),
                                   std::initializer_list<size_t>) {
    TestType vec = {0, 1, 12, 1, 13, 1, 3, 3, 13, 13, 1, 41, 4, 41, 14, 4};
    REQUIRE(vec.size() == 16);
    REQUIRE_EXCEPTION_MSG(
        detail::throw_if_not_pperm(vec.begin(), vec.end(), 42),
        "duplicate image value, found 1 in position 3, "
        "first occurrence in position 1");
    REQUIRE_EXCEPTION_MSG(detail::throw_if_not_pperm(vec.begin(), vec.end()),
                          "image value out of bounds, expected value in "
                          "[0, 16), found 41 in position 11");
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("throw_if_not_pperm",
                                   "008",
                                   "for dom. and img.",
                                   "[quick]",
                                   std::vector<uint32_t>,
                                   (std::array<uint8_t, 16>),
                                   std::initializer_list<size_t>) {
    TestType vec
        = {0, 1, 12, 1, 13, 1, 3, 3, UNDEFINED, 13, 1, 41, 4, 41, 14, 4};
    REQUIRE_EXCEPTION_MSG(
        detail::throw_if_not_pperm(
            vec.begin(), vec.begin() + 3, vec.begin() + 3, vec.begin() + 6, 16),
        "duplicate image value, found 1 in position 2, first occurrence in "
        "position 0");
    REQUIRE_EXCEPTION_MSG(
        detail::throw_if_not_pperm(
            vec.begin(), vec.begin() + 3, vec.begin() + 3, vec.begin() + 4, 16),
        "domain and image size mismatch, domain has size 3 but image has size "
        "1");
    REQUIRE_EXCEPTION_MSG(
        detail::throw_if_not_ptransf(vec.begin() + 7,
                                     vec.begin() + 9,
                                     vec.begin() + 3,
                                     vec.begin() + 5,
                                     16),
        fmt::format("the 1st argument (domain) must not contain "
                    "UNDEFINED, but found UNDEFINED (= {}) in position 1",
                    static_cast<typename TestType::value_type>(UNDEFINED)));
    REQUIRE_EXCEPTION_MSG(
        detail::throw_if_not_ptransf(vec.begin() + 3,
                                     vec.begin() + 5,
                                     vec.begin() + 7,
                                     vec.begin() + 9,
                                     16),
        fmt::format("the 2nd argument (image) must not contain "
                    "UNDEFINED, but found UNDEFINED (= {}) in position 1",
                    static_cast<typename TestType::value_type>(UNDEFINED)));
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("is_ptransf",
                                   "009",
                                   "for image",
                                   "[quick]",
                                   std::vector<uint32_t>,
                                   (std::array<uint8_t, 16>),
                                   std::initializer_list<size_t>) {
    // No std::string here because we static_assert that the integer values in
    // the container are unsigned.
    TestType vec
        = {0, 1, 12, 1, 13, 1, 3, 3, 13, UNDEFINED, 1, 41, 4, 41, 14, 4};
    REQUIRE(vec.size() == 16);
    REQUIRE(is_ptransf(vec.begin(), vec.end(), 42));
    REQUIRE(!is_ptransf(vec.begin(), vec.end()));
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("is_transf",
                                   "009",
                                   "for image",
                                   "[quick]",
                                   std::vector<uint32_t>,
                                   (std::array<uint8_t, 16>),
                                   std::initializer_list<size_t>) {
    // No std::string here because we static_assert that the integer values in
    // the container are unsigned.
    TestType vec
        = {0, 1, 12, 1, 13, 1, 3, 3, 13, UNDEFINED, 1, 41, 4, 41, 14, 4};
    REQUIRE(vec.size() == 16);
    REQUIRE(!is_transf(vec.begin(), vec.begin() + 10, 14));
    REQUIRE(is_transf(vec.begin(), vec.begin() + 9, 14));
    REQUIRE(!is_transf(vec.begin(), vec.end()));
    REQUIRE(!is_transf(vec.begin(), vec.end(), 42));
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("is_pperm",
                                   "010",
                                   "for image",
                                   "[quick]",
                                   std::vector<uint32_t>,
                                   (std::array<uint8_t, 16>),
                                   std::initializer_list<size_t>) {
    TestType vec
        = {0, UNDEFINED, 12, 1, 13, 1, 3, 3, 13, 13, 1, 41, 4, 41, 14, 4};
    REQUIRE(vec.size() == 16);
    REQUIRE(is_pperm(vec.begin(), vec.begin() + 2, 2));
    REQUIRE(is_pperm(vec.begin(), vec.begin() + 2));
    REQUIRE(!is_pperm(vec.begin(), vec.end(), 42));
    REQUIRE(!is_pperm(vec.begin(), vec.end()));
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("is_perm",
                                   "010",
                                   "for image",
                                   "[quick]",
                                   std::vector<uint32_t>,
                                   (std::array<uint8_t, 16>),
                                   std::initializer_list<size_t>) {
    TestType vec = {0, 1, 12, 1, 13, 1, 3, 3, 13, 13, 1, 41, 4, 41, 14, 4};
    REQUIRE(vec.size() == 16);
    REQUIRE(is_perm(vec.begin(), vec.begin() + 2, 2));
    REQUIRE(is_perm(vec.begin(), vec.begin() + 2));
    REQUIRE(!is_perm(vec.begin(), vec.end(), 42));
    REQUIRE(!is_perm(vec.begin(), vec.end()));
  }

}  // namespace libsemigroups
