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

#include <cstddef>      // for size_t
#include <cstdint>      // for uint8_t
#include <type_traits>  // for enable_if, is_integral

#include "catch_amalgamated.hpp"           // for LIBSEMIGROUPS_TEST_CASE
#include "libsemigroups/adapters.hpp"      // for complexity etc
#include "libsemigroups/debug.hpp"         // for LIBSEMIGROUPS_ASSERT
#include "libsemigroups/froidure-pin.hpp"  // for FroidurePin<>::element_i...
#include "test-main.hpp"

namespace libsemigroups {

  constexpr bool REPORT = false;

  template <typename TIntegralType>
  struct Complexity<
      TIntegralType,
      typename std::enable_if<std::is_integral<TIntegralType>::value>::type> {
    constexpr size_t operator()(TIntegralType) const noexcept {
      return 0;
    }
  };

  template <typename TIntegralType>
  struct Degree<
      TIntegralType,
      typename std::enable_if<std::is_integral<TIntegralType>::value>::type> {
    constexpr size_t operator()(TIntegralType) const noexcept {
      return 0;
    }
  };

  template <typename TIntegralType>
  struct IncreaseDegree<
      TIntegralType,
      typename std::enable_if<std::is_integral<TIntegralType>::value>::type> {
    TIntegralType operator()(TIntegralType x) const noexcept {
      LIBSEMIGROUPS_ASSERT(false);
      return x;
    }
  };

  template <typename TIntegralType>
  struct One<
      TIntegralType,
      typename std::enable_if<std::is_integral<TIntegralType>::value>::type> {
    constexpr TIntegralType operator()(TIntegralType) const noexcept {
      return 1;
    }
  };

  template <typename TIntegralType>
  struct Product<
      TIntegralType,
      typename std::enable_if<std::is_integral<TIntegralType>::value>::type> {
    void operator()(TIntegralType& xy,
                    TIntegralType  x,
                    TIntegralType  y,
                    size_t = 0) const noexcept {
      xy = x * y;
    }
  };

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "031",
                          "uint32_t/uint8_t",
                          "[quick][froidure-pin][integers]") {
    auto                  rg = ReportGuard(REPORT);
    FroidurePin<uint32_t> S;
    S.add_generator(2);
    REQUIRE(S.size() == 32);
    REQUIRE(S.number_of_idempotents() == 1);
    REQUIRE(S[0] == 2);
    REQUIRE((froidure_pin::elements(S) | rx::to_vector())
            == std::vector<uint32_t>({2,
                                      4,
                                      8,
                                      16,
                                      32,
                                      64,
                                      128,
                                      256,
                                      512,
                                      1'024,
                                      2'048,
                                      4'096,
                                      8'192,
                                      16'384,
                                      32'768,
                                      65'536,
                                      131'072,
                                      262'144,
                                      524'288,
                                      1'048'576,
                                      2'097'152,
                                      4'194'304,
                                      8'388'608,
                                      16'777'216,
                                      33'554'432,
                                      67'108'864,
                                      134'217'728,
                                      268'435'456,
                                      536'870'912,
                                      1'073'741'824,
                                      2'147'483'648,
                                      0}));

    FroidurePin<uint8_t> T;
    T.add_generator(2);
    T.add_generator(3);
    REQUIRE(T.size() == 130);
    REQUIRE(T.number_of_idempotents() == 2);
    REQUIRE((froidure_pin::idempotents(T) | rx::to_vector())
            == std::vector<uint8_t>({0, 1}));
  }
}  // namespace libsemigroups
