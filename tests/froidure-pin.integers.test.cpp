//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2018 James D. Mitchell
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

#include "froidure-pin.hpp"
#include "libsemigroups.tests.hpp"

namespace libsemigroups {

  constexpr bool REPORT = false;

  template <typename TIntegralType>
  struct complexity<
      TIntegralType,
      typename std::enable_if<std::is_integral<TIntegralType>::value>::type> {
    constexpr size_t operator()(TIntegralType) const noexcept {
      return 0;
    }
  };

  template <typename TIntegralType>
  struct degree<
      TIntegralType,
      typename std::enable_if<std::is_integral<TIntegralType>::value>::type> {
    constexpr size_t operator()(TIntegralType) const noexcept {
      return 0;
    }
  };

  template <typename TIntegralType>
  struct increase_degree_by<
      TIntegralType,
      typename std::enable_if<std::is_integral<TIntegralType>::value>::type> {
    TIntegralType operator()(TIntegralType x) const noexcept {
      LIBSEMIGROUPS_ASSERT(false);
      return x;
    }
  };

  template <typename TIntegralType>
  struct less<
      TIntegralType,
      typename std::enable_if<std::is_integral<TIntegralType>::value>::type> {
    bool operator()(TIntegralType x, TIntegralType y) const noexcept {
      return x < y;
    }
  };

  template <typename TIntegralType>
  struct one<
      TIntegralType,
      typename std::enable_if<std::is_integral<TIntegralType>::value>::type> {
    constexpr TIntegralType operator()(TIntegralType) const noexcept {
      return 1;
    }
  };

  template <typename TIntegralType>
  struct product<
      TIntegralType,
      typename std::enable_if<std::is_integral<TIntegralType>::value>::type> {
    void operator()(TIntegralType& xy,
                    TIntegralType  x,
                    TIntegralType  y,
                    size_t = 0) const noexcept {
      xy = x * y;
    }
  };

#ifdef LIBSEMIGROUPS_DENSEHASHMAP
  template <typename TIntegralType>
  struct empty_key<
      TIntegralType,
      typename std::enable_if<std::is_integral<TIntegralType>::value>::type> {
    TIntegralType operator()(TIntegralType) const noexcept {
      return UNDEFINED;
    }
  }
#endif

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "107",
                          "(integers)",
                          "[quick][froidure-pin][integers]") {
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
