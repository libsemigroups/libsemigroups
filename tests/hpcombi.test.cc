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

#include "../src/libsemigroups-config.h"

#ifdef LIBSEMIGROUPS_USE_HPCOMBI

#include "../extern/HPCombi/include/perm16.hpp"
#include "../src/semigroups.h"
#include "catch.hpp"

using namespace libsemigroups;
using namespace HPCombi;

struct Renner0Element : public PTransf16 {
  using PTransf16::PTransf16;

  Renner0Element operator*(Renner0Element y) {
    Renner0Element minab, maxab, mask, b = permuted(y);
    mask  = _mm_cmplt_epi8(y, Perm16::one());
    minab = _mm_min_epi8(v, b);
    maxab = _mm_max_epi8(v, b);
    return static_cast<epu8>(_mm_blendv_epi8(maxab, minab, mask))
           | (y.v == cst_epu8_0xFF);
  }

  size_t complexity() {
    return -1;
  }
};

namespace std {

  template <> struct hash<Renner0Element> {
    inline size_t operator()(const Renner0Element& ar) const {
      return hash<HPCombi::Vect16>()(ar);
    }
  };
}

namespace libsemigroups {
  template <>
  size_t libsemigroups::ElementContainer<Renner0Element>::complexity(
      Renner0Element) const {
    return -1;
  }
}

TEST_CASE("HPCombi 01: Transf16", "[quick][hpcombi][finite][01]") {
  Semigroup<Transf16, std::hash<Transf16>, std::equal_to<Transf16>> S(
      {Transf16({1, 2, 0})});
  S.set_report(true);
  REQUIRE(S.size() == 3);
  REQUIRE(S.nridempotents() == 1);
}

TEST_CASE("HPCombi 02: Transf16", "[quick][hpcombi][finite][02]") {
  Semigroup<Transf16, std::hash<Transf16>, std::equal_to<Transf16>> S(
      {Transf16({1, 7, 2, 6, 0, 4, 1, 5}),
       Transf16({2, 4, 6, 1, 4, 5, 2, 7}),
       Transf16({3, 0, 7, 2, 4, 6, 2, 4}),
       Transf16({3, 2, 3, 4, 5, 3, 0, 1}),
       Transf16({4, 3, 7, 7, 4, 5, 0, 4}),
       Transf16({5, 6, 3, 0, 3, 0, 5, 1}),
       Transf16({6, 0, 1, 1, 1, 6, 3, 4}),
       Transf16({7, 7, 4, 0, 6, 4, 1, 7})});
  S.reserve(597369 * 2);
  S.set_report(false);
  REQUIRE(S.size() == 597369);
}

const uint8_t FF = 0xFF;

TEST_CASE("HPCombi 03: Renner0", "[quick][hpcombi][finite][03]") {
  Semigroup<Renner0Element,
            std::hash<Renner0Element>,
            std::equal_to<Renner0Element>>
      S({Renner0Element({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}),
         Renner0Element(
             {FF, FF, FF, FF, FF, FF, FF, FF, 8, 9, 10, 11, 12, 13, 14, 15}),
         Renner0Element({0, 1, 2, 3, 4, 5, 6, 8, 7, 9, 10, 11, 12, 13, 14, 15}),
         Renner0Element({0, 1, 2, 3, 4, 5, 7, 6, 9, 8, 10, 11, 12, 13, 14, 15}),
         Renner0Element({0, 1, 2, 3, 4, 6, 5, 7, 8, 10, 9, 11, 12, 13, 14, 15}),
         Renner0Element({0, 1, 2, 3, 5, 4, 6, 7, 8, 9, 11, 10, 12, 13, 14, 15}),
         Renner0Element({0, 1, 2, 4, 3, 5, 6, 7, 8, 9, 10, 12, 11, 13, 14, 15}),
         Renner0Element(
             {0, 1, 3, 2, 4, 5, 6, 7, 8, 9, 10, 11, 13, 12, 14, 15})});
  S.set_report(true);
  REQUIRE(S.size() == 8962225);
  REQUIRE(S.nridempotents() == 128);
}

TEST_CASE("HPCombi 03: full transformation monoid 8",
          "[extreme][hpcombi][finite][03]") {
  Semigroup<Transf16, std::hash<Transf16>, std::equal_to<Transf16>> S(
      {Transf16({1, 2, 3, 4, 5, 6, 7, 0}),
       Transf16({1, 0, 2, 3, 4, 5, 6, 7}),
       Transf16({0, 1, 2, 3, 4, 5, 6, 0})});
  S.reserve(std::pow(8, 8));
  S.set_report(true);
  REQUIRE(S.size() == 16777216);
  S.set_report(false);
}

#endif
