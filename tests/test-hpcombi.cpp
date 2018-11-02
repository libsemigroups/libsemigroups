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

// Must include hpcombi.hpp so that LIBSEMIGROUPS_HPCOMBI is defined, if so
// specified at during configure.
#include "hpcombi.hpp"

#ifdef LIBSEMIGROUPS_HPCOMBI

#include "froidure-pin.hpp"
#include "libsemigroups.tests.hpp"

using namespace HPCombi;

const uint8_t FE = 0xfe;
const uint8_t FF = 0xFF;

struct Renner0Element : public PTransf16 {
  using PTransf16::PTransf16;

  Renner0Element operator*(Renner0Element const& y) const {
    Renner0Element minab, maxab, mask, b = permuted(y);
    mask  = _mm_cmplt_epi8(y, Perm16::one());
    minab = _mm_min_epi8(v, b);
    maxab = _mm_max_epi8(v, b);
    return static_cast<epu8>(_mm_blendv_epi8(maxab, minab, mask))
           | (y.v == Epu8(0xFF));
  }
};

static_assert(std::is_trivial<Renner0Element>::value,
              "Renner0Element is not trivial");

namespace std {
  template <>
  struct hash<Renner0Element> {
    inline size_t operator()(const Renner0Element& ar) const {
      return hash<HPCombi::Vect16>{}(ar);
    }
  };
}  // namespace std

namespace libsemigroups {

  constexpr bool REPORT = false;

#ifdef LIBSEMIGROUPS_DENSEHASHMAP
  template <>
  struct empty_key<Renner0Element> {
    Renner0Element operator()(Renner0Element const&) const {
      return {FE, FE, FE, FE, FE, FE, FE, FE, FE, FE, FE, FE, FE, FE, FE, FE};
    }
  };
#endif

  LIBSEMIGROUPS_TEST_CASE("HPCombi", "001", "Transf16", "[quick][hpcombi]") {
    REPORTER.set_report(REPORT);
    FroidurePin<Transf16, std::hash<Transf16>, std::equal_to<Transf16>> S(
        {Transf16({1, 2, 0})});
    REQUIRE(S.size() == 3);
    REQUIRE(S.nr_idempotents() == 1);
    REQUIRE(std::vector<Transf16>(S.cbegin_sorted(), S.cend_sorted())
            == std::vector<Transf16>(
                   {Transf16({}), Transf16({1, 2, 0}), Transf16({2, 0, 1})}));
  }

  LIBSEMIGROUPS_TEST_CASE("HPCombi", "002", "Transf16", "[standard][hpcombi]") {
    REPORTER.set_report(REPORT);
    FroidurePin<Transf16, std::hash<Transf16>, std::equal_to<Transf16>> S(
        {Transf16({1, 7, 2, 6, 0, 4, 1, 5}),
         Transf16({2, 4, 6, 1, 4, 5, 2, 7}),
         Transf16({3, 0, 7, 2, 4, 6, 2, 4}),
         Transf16({3, 2, 3, 4, 5, 3, 0, 1}),
         Transf16({4, 3, 7, 7, 4, 5, 0, 4}),
         Transf16({5, 6, 3, 0, 3, 0, 5, 1}),
         Transf16({6, 0, 1, 1, 1, 6, 3, 4}),
         Transf16({7, 7, 4, 0, 6, 4, 1, 7})});
    S.reserve(600000);
    REQUIRE(S.size() == 597369);
  }

  LIBSEMIGROUPS_TEST_CASE("HPCombi", "003", "Renner0", "[extreme][hpcombi]") {
    REPORTER.set_report(true);
    FroidurePin<Renner0Element,
                std::hash<Renner0Element>,
                std::equal_to<Renner0Element>>
        S({Renner0Element(
               {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}),
           Renner0Element(
               {FF, FF, FF, FF, FF, FF, FF, FF, 8, 9, 10, 11, 12, 13, 14, 15}),
           Renner0Element(
               {0, 1, 2, 3, 4, 5, 6, 8, 7, 9, 10, 11, 12, 13, 14, 15}),
           Renner0Element(
               {0, 1, 2, 3, 4, 5, 7, 6, 9, 8, 10, 11, 12, 13, 14, 15}),
           Renner0Element(
               {0, 1, 2, 3, 4, 6, 5, 7, 8, 10, 9, 11, 12, 13, 14, 15}),
           Renner0Element(
               {0, 1, 2, 3, 5, 4, 6, 7, 8, 9, 11, 10, 12, 13, 14, 15}),
           Renner0Element(
               {0, 1, 2, 4, 3, 5, 6, 7, 8, 9, 10, 12, 11, 13, 14, 15}),
           Renner0Element(
               {0, 1, 3, 2, 4, 5, 6, 7, 8, 9, 10, 11, 13, 12, 14, 15})});
    REQUIRE(S.size() == 8962225);
    REQUIRE(S.nr_idempotents() == 128);
  }

  LIBSEMIGROUPS_TEST_CASE("HPCombi",
                          "004",
                          "full transformation monoid 8",
                          "[extreme][hpcombi]") {
    REPORTER.set_report(true);
    FroidurePin<Transf16, std::hash<Transf16>, std::equal_to<Transf16>> S(
        {Transf16({1, 2, 3, 4, 5, 6, 7, 0}),
         Transf16({1, 0, 2, 3, 4, 5, 6, 7}),
         Transf16({0, 1, 2, 3, 4, 5, 6, 0})});
    S.reserve(std::pow(8, 8));
    REQUIRE(S.size() == 16777216);
  }
}  // namespace libsemigroups
#endif
