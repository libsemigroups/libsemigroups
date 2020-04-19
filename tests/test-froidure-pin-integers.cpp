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

#include "catch.hpp"                       // for LIBSEMIGROUPS_TEST_CASE
#include "libsemigroups/adapters.hpp"      // for complexity etc
#include "libsemigroups/froidure-pin.hpp"  // for FroidurePin<>::element_i...
#include "libsemigroups/libsemigroups-debug.hpp"  // for LIBSEMIGROUPS_ASSERT
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

  // #ifdef LIBSEMIGROUPS_DENSEHASHMAP
  //   template <typename TIntegralType>
  //   struct EmptyKey<
  //       TIntegralType,
  //       typename
  //       std::enable_if<std::is_integral<TIntegralType>::value>::type> {
  //     TIntegralType operator()(TIntegralType) const noexcept {
  //       return UNDEFINED;
  //     }
  //   }
  // #endif

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "102",
                          "(integers)",
                          "[quick][froidure-pin][integers]") {
    auto             rg = ReportGuard(REPORT);
    FroidurePin<int> S({2});
    REQUIRE(S.size() == 32);
    REQUIRE(S.nr_idempotents() == 1);
    FroidurePin<int>::const_iterator it = S.cbegin();
    REQUIRE(*it == 2);

    FroidurePin<uint8_t> T({2, 3});
    REQUIRE(T.size() == 130);
    REQUIRE(T.nr_idempotents() == 2);
    REQUIRE(*T.cbegin_idempotents() == 0);
    REQUIRE(*T.cbegin_idempotents() + 1 == 1);
  }
}  // namespace libsemigroups
