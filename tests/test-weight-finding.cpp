// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2026 Joseph Edwards
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

#include "libsemigroups/config.hpp"  // for LIBSEMIGROUPS_ALGLIB_ENABLED
#ifdef LIBSEMIGROUPS_ALGLIB_ENABLED

#include <cstddef>  // for size_t

#include "test-main.hpp"  // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/exception.hpp"  // for LibsemigroupsException

#include "libsemigroups/detail/containers.hpp"
#include "libsemigroups/detail/weight-finding.hpp"

namespace libsemigroups::detail {

  LIBSEMIGROUPS_TEST_CASE("get_weights", "000", "simple example", "[quick]") {
    DynamicArray2<int> coefs(2, 2);
    std::vector<bool>  is_strict(2);

    // x > 0
    coefs.set(0, 0, 1);
    is_strict[0] = true;

    // y > 0
    coefs.set(1, 1, 1);
    is_strict[1] = true;

    auto res = get_weights(coefs, is_strict);
    REQUIRE(res.has_value());
    REQUIRE(res.value() == std::vector<size_t>{1, 1});
  }

  LIBSEMIGROUPS_TEST_CASE("get_weights",
                          "001",
                          "simple example x2",
                          "[quick]") {
    DynamicArray2<int> coefs(2, 3);
    std::vector<bool>  is_strict(3);

    // x - y > 0
    coefs.set(0, 0, 1);
    coefs.set(0, 1, -1);
    is_strict[0] = true;

    // -x + 200y > 0
    coefs.set(1, 0, -1);
    coefs.set(1, 1, 200);
    is_strict[1] = true;

    // x - 199y > 0
    coefs.set(2, 0, 1);
    coefs.set(2, 1, -199);
    is_strict[2] = true;

    auto res = get_weights(coefs, is_strict);
    REQUIRE(res.has_value());
    REQUIRE(res.value() == std::vector<size_t>{399, 2});
  }

  LIBSEMIGROUPS_TEST_CASE("get_weights", "002", "no solution", "[quick]") {
    // -x1 - x2 - x3 - x4 > 0
    DynamicArray2<int> coefs(4, 1, -1);
    std::vector<bool>  is_strict{true};

    auto res = get_weights(coefs, is_strict);
    REQUIRE(!res.has_value());
  }

  LIBSEMIGROUPS_TEST_CASE("get_weights", "003", "no solution x2", "[quick]") {
    DynamicArray2<int> coefs(2, 2);
    std::vector<bool>  is_strict(2);

    // x - y > 0
    coefs.set(0, 0, 1);
    coefs.set(0, 1, -1);
    is_strict[0] = true;

    // -x + y > 0
    coefs.set(1, 0, -1);
    coefs.set(1, 1, 1);
    is_strict[1] = true;

    auto res = get_weights(coefs, is_strict);
    REQUIRE(!res.has_value());
  }

  LIBSEMIGROUPS_TEST_CASE("get_weights", "004", "non-strict", "[quick]") {
    DynamicArray2<int> coefs(2, 2);
    std::vector<bool>  is_strict(2);

    // x - y >= 0
    coefs.set(0, 0, 1);
    coefs.set(0, 1, -1);
    is_strict[0] = false;

    // -x + y >= 0
    coefs.set(1, 0, -1);
    coefs.set(1, 1, 1);
    is_strict[1] = false;

    auto res = get_weights(coefs, is_strict);
    REQUIRE(res.has_value());
    REQUIRE(res.value() == std::vector<size_t>{1, 1});
  }

  LIBSEMIGROUPS_TEST_CASE("get_weights", "005", "exception", "[quick]") {
    DynamicArray2<int> coefs(2, 2);
    std::vector<bool>  is_strict(3);
    REQUIRE_EXCEPTION_MSG(
        get_weights(coefs, is_strict),
        "the number of rows of the first argument must be the same as the size "
        "of the second argument, found 2 and 3 respectively");
  }

  LIBSEMIGROUPS_TEST_CASE("get_weights",
                          "006",
                          "duplicated constraints (high dim)",
                          "[standard]") {
    // x1 + ... + x10 > 0
    DynamicArray2<int> coefs(10, 1000, 1);
    std::vector<bool>  is_strict(1000, true);

    auto res = get_weights(coefs, is_strict);
    REQUIRE(res.has_value());

    // The 883 is here because this is apparently the first feasible solution
    // the solver finds; without specifying a function for the solver to
    // minimise, we get no say on which solution is found.
    REQUIRE(res.value() == std::vector<size_t>{1, 1, 1, 1, 1, 1, 1, 1, 1, 883});
  }

  LIBSEMIGROUPS_TEST_CASE("get_weights",
                          "007",
                          "duplicated constraints (low dim)",
                          "[quick]") {
    // x1 + x2 > 0
    DynamicArray2<int> coefs(2, 1000, 1);
    std::vector<bool>  is_strict(1000, true);

    auto res = get_weights(coefs, is_strict);
    REQUIRE(res.has_value());
    REQUIRE(res.value() == std::vector<size_t>{1, 1});
  }

  LIBSEMIGROUPS_TEST_CASE("get_weights", "008", "no constraints", "[quick]") {
    DynamicArray2<int> coefs(2, 0);
    std::vector<bool>  is_strict(0);

    auto res = get_weights(coefs, is_strict);
    REQUIRE(res.has_value());
    REQUIRE(res.value() == std::vector<size_t>{1, 1});
  }
}  // namespace libsemigroups::detail

#endif  // LIBSEMIGROUPS_ALGLIB_ENABLED