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

// Must include hpcombi.hpp so that LIBSEMIGROUPS_HPCOMBI is defined, if so
// specified at during configure.
#include "libsemigroups/libsemigroups-config.hpp"  // for LIBSEMIGROUPS_HPCOMBI

#ifdef LIBSEMIGROUPS_HPCOMBI

#include <cstddef>  // for size_t

#include "catch.hpp"                       // for REQUIRE
#include "libsemigroups/element.hpp"       // for Transformation
#include "libsemigroups/froidure-pin.hpp"  // for FroidurePin
#include "libsemigroups/hpcombi.hpp"       // for PTransf16, ...
#include "libsemigroups/int-range.hpp"     // for IntegralRange
#include "libsemigroups/report.hpp"        // for ReportGuard
#include "test-main.hpp"                   // for LIBSEMIGROUPS_TEST_CASE

using namespace HPCombi;

const uint8_t FF = 0xFF;

// Note that Renner0Element appears to require very little to make it a
// template argument of FroidurePin, but in actual fact because it inherits
// from PTransf16, most of the required specialisations of One, etc, are in
// include/hpcombi.hpp.

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

  // #ifdef LIBSEMIGROUPS_DENSEHASHMAP
  //   const uint8_t FE = 0xfe;
  //   template <>
  //   struct EmptyKey<Renner0Element> {
  //     Renner0Element operator()(Renner0Element const&) const {
  //       return {FE, FE, FE, FE, FE, FE, FE, FE, FE, FE, FE, FE, FE, FE, FE,
  //       FE};
  //     }
  //   };
  // #endif

  LIBSEMIGROUPS_TEST_CASE("HPCombi", "000", "Transf16", "[quick][hpcombi]") {
    auto                  rg = ReportGuard(REPORT);
    FroidurePin<Transf16> S({Transf16({1, 2, 0})});
    REQUIRE(S.size() == 3);
    REQUIRE(S.nr_idempotents() == 1);
    REQUIRE(std::vector<Transf16>(S.cbegin_sorted(), S.cend_sorted())
            == std::vector<Transf16>(
                {Transf16({}), Transf16({1, 2, 0}), Transf16({2, 0, 1})}));
  }

  LIBSEMIGROUPS_TEST_CASE("HPCombi",
                          "001",
                          "One specialisation",
                          "[quick][hpcombi]") {
    auto id = One<Transf16>()(10);
    auto x  = Transf16({3, 2, 3, 4, 5, 3, 0, 1});
    REQUIRE(x * id == x);
    REQUIRE(id * x == x);
    REQUIRE(id * id == id);
    REQUIRE(id != x);
  }

  LIBSEMIGROUPS_TEST_CASE("HPCombi",
                          "002",
                          "One specialisation",
                          "[quick][hpcombi]") {
    auto id = One<Renner0Element>()(10);
    auto x  = Renner0Element(
        {0, 1, 2, 4, 3, 5, 6, 7, 8, 9, 10, 12, 11, 13, 14, 15});
    REQUIRE(x * id == x);
    REQUIRE(id * x == x);
    REQUIRE(id * id == id);
    REQUIRE(id != x);
    id = One<Renner0Element>()(x);
    REQUIRE(x * id == x);
    REQUIRE(id * x == x);
    REQUIRE(id * id == id);
    REQUIRE(id != x);
  }

  LIBSEMIGROUPS_TEST_CASE("HPCombi",
                          "003",
                          "Swap specialisation",
                          "[quick][hpcombi]") {
    auto x = Transf16({0, 0, 0, 0, 0, 0, 0, 0});
    auto y = Transf16({1, 1, 1, 1, 1, 1, 1, 1});
    Swap<Transf16>()(x, y);
    REQUIRE(x == Transf16({1, 1, 1, 1, 1, 1, 1, 1}));
    REQUIRE(y == Transf16({0, 0, 0, 0, 0, 0, 0, 0}));
  }

  LIBSEMIGROUPS_TEST_CASE("HPCombi",
                          "004",
                          "Swap specialisation",
                          "[quick][hpcombi]") {
    auto x = Renner0Element(
        {FF, FF, FF, FF, FF, FF, FF, FF, 8, 9, 10, 11, 12, 13, 14, 15});
    auto y = Renner0Element(
        {0, 1, 2, 4, 3, 5, 6, 7, 8, 9, 10, 12, 11, 13, 14, 15});
    Swap<Renner0Element>()(x, y);
    REQUIRE(x
            == Renner0Element(
                {0, 1, 2, 4, 3, 5, 6, 7, 8, 9, 10, 12, 11, 13, 14, 15}));
    REQUIRE(
        y
        == Renner0Element(
            {FF, FF, FF, FF, FF, FF, FF, FF, 8, 9, 10, 11, 12, 13, 14, 15}));
  }

  LIBSEMIGROUPS_TEST_CASE("HPCombi",
                          "005",
                          "Inverse specialisation",
                          "[quick][hpcombi]") {
    auto id = One<Perm16>()(10);
    auto x  = Perm16({0, 2, 1, 4, 5, 3});
    auto y  = Inverse<Perm16>()(x);
    REQUIRE(x * y == id);
    REQUIRE(y * x == id);
  }

  LIBSEMIGROUPS_TEST_CASE("HPCombi",
                          "006",
                          "ImageRightAction<Perm16, int>",
                          "[quick][hpcombi]") {
    auto x = Perm16({0, 2, 1, 4, 5, 3});
    REQUIRE(ImageRightAction<Perm16, int>()(0, x) == 0);
    REQUIRE(ImageRightAction<Perm16, int>()(1, x) == 2);
    REQUIRE(ImageRightAction<Perm16, int>()(2, x) == 1);
    REQUIRE(ImageRightAction<Perm16, int>()(3, x) == 4);
    REQUIRE(ImageRightAction<Perm16, int>()(4, x) == 5);
    REQUIRE(ImageRightAction<Perm16, int>()(5, x) == 3);

    int pt;
    ImageRightAction<Perm16, int>()(pt, 0, x);
    REQUIRE(pt == 0);
    ImageRightAction<Perm16, int>()(pt, 1, x);
    REQUIRE(pt == 2);
    ImageRightAction<Perm16, int>()(pt, 2, x);
    REQUIRE(pt == 1);
    ImageRightAction<Perm16, int>()(pt, 3, x);
    REQUIRE(pt == 4);
    ImageRightAction<Perm16, int>()(pt, 4, x);
    REQUIRE(pt == 5);
    ImageRightAction<Perm16, int>()(pt, 5, x);
    REQUIRE(pt == 3);

    auto id = One<Perm16>()(10);
    auto r  = IntegralRange<int, 0, 10>();
    REQUIRE(std::all_of(r.cbegin(), r.cend(), [&id](int pt) {
      return ImageRightAction<Perm16, int>()(pt, id) == pt;
    }));
    REQUIRE(std::all_of(r.cbegin(), r.cend(), [&id](int pt) {
      int qt;
      ImageRightAction<Perm16, int>()(qt, pt, id);
      return qt == pt;
    }));
  }

  LIBSEMIGROUPS_TEST_CASE("HPCombi",
                          "007",
                          "ImageLeft/RightAction<PPerm16, PPerm16>",
                          "[quick][hpcombi]") {
    auto    id = One<PPerm16>()(5);
    auto    x  = PPerm16({10}, {0});
    auto    y  = PPerm16({1}, {2});
    PPerm16 res;
    ImageRightAction<PPerm16, PPerm16>()(res, id, x);
    REQUIRE(res == PPerm16({0}, {0}));
    ImageRightAction<PPerm16, PPerm16>()(res, x, id);
    REQUIRE(res == PPerm16({0}, {0}));
    ImageRightAction<PPerm16, PPerm16>()(res, x, y);
    REQUIRE(res == PPerm16({}, {}));
    ImageRightAction<PPerm16, PPerm16>()(res, y, x);
    REQUIRE(res == PPerm16({}, {}));
    ImageRightAction<PPerm16, PPerm16>()(res, y, id);
    REQUIRE(res == PPerm16({2}, {2}));
    ImageRightAction<PPerm16, PPerm16>()(res, id, y);
    REQUIRE(res == PPerm16({2}, {2}));

    ImageLeftAction<PPerm16, PPerm16>()(res, id, x);
    REQUIRE(res == PPerm16({10}, {10}));
    ImageLeftAction<PPerm16, PPerm16>()(res, x, id);
    REQUIRE(res == PPerm16({10}, {10}));
    ImageLeftAction<PPerm16, PPerm16>()(res, x, y);
    REQUIRE(res == PPerm16({}, {}));
    ImageLeftAction<PPerm16, PPerm16>()(res, y, x);
    REQUIRE(res == PPerm16({}, {}));
    ImageLeftAction<PPerm16, PPerm16>()(res, y, id);
    REQUIRE(res == PPerm16({1}, {1}));
    ImageLeftAction<PPerm16, PPerm16>()(res, id, y);
    REQUIRE(res == PPerm16({1}, {1}));
  }

  LIBSEMIGROUPS_TEST_CASE("HPCombi",
                          "008",
                          "Product<Renner0Element>",
                          "[quick][hpcombi]") {
    auto id = One<Renner0Element>()(5);
    auto x  = Renner0Element(
        {0, 1, 2, 3, 4, 5, 6, 8, 7, 9, 10, 11, 12, 13, 14, 15});
    auto y = Renner0Element(
        {FF, FF, FF, FF, FF, FF, FF, FF, 8, 9, 10, 11, 12, 13, 14, 15});
    Renner0Element xy;

    Product<Renner0Element>()(xy, x, y);
    REQUIRE(xy == y * x);
    Product<Renner0Element>()(xy, y, x);
    REQUIRE(xy == x * y);
    Product<Renner0Element>()(xy, y, id);
    REQUIRE(xy == y);
    Product<Renner0Element>()(xy, x, id);
    REQUIRE(xy == x);
    Product<Renner0Element>()(xy, id, x);
    REQUIRE(xy == x);
    Product<Renner0Element>()(xy, id, y);
    REQUIRE(xy == y);
  }

  LIBSEMIGROUPS_TEST_CASE("HPCombi",
                          "009",
                          "Degree/Complexity<Renner0Element>",
                          "[quick][hpcombi]") {
    auto id = One<Renner0Element>()(5);
    REQUIRE(Degree<Renner0Element>()(id) == 16);
    REQUIRE(Complexity<Renner0Element>()(id) == 0);
  }

  LIBSEMIGROUPS_TEST_CASE("HPCombi", "010", "Transf16", "[standard][hpcombi]") {
    auto                  rg = ReportGuard(REPORT);
    FroidurePin<Transf16> S({Transf16({1, 7, 2, 6, 0, 4, 1, 5}),
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

  LIBSEMIGROUPS_TEST_CASE("HPCombi", "011", "Transf16", "[standard][hpcombi]") {
    auto rg = ReportGuard(REPORT);

    using Transf = libsemigroups::Transformation<size_t>;
    FroidurePin<Transf> S({Transf({1, 7, 2, 6, 0, 4, 1, 5}),
                           Transf({2, 4, 6, 1, 4, 5, 2, 7}),
                           Transf({3, 0, 7, 2, 4, 6, 2, 4}),
                           Transf({3, 2, 3, 4, 5, 3, 0, 1}),
                           Transf({4, 3, 7, 7, 4, 5, 0, 4}),
                           Transf({5, 6, 3, 0, 3, 0, 5, 1}),
                           Transf({6, 0, 1, 1, 1, 6, 3, 4}),
                           Transf({7, 7, 4, 0, 6, 4, 1, 7})});
    S.reserve(600000);
    REQUIRE(S.size() == 597369);
  }

  LIBSEMIGROUPS_TEST_CASE("HPCombi", "012", "Renner0", "[extreme][hpcombi]") {
    auto                        rg = ReportGuard(true);
    FroidurePin<Renner0Element> S(
        {Renner0Element({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}),
         Renner0Element(
             {FF, FF, FF, FF, FF, FF, FF, FF, 8, 9, 10, 11, 12, 13, 14, 15}),
         Renner0Element({0, 1, 2, 3, 4, 5, 6, 8, 7, 9, 10, 11, 12, 13, 14, 15}),
         Renner0Element({0, 1, 2, 3, 4, 5, 7, 6, 9, 8, 10, 11, 12, 13, 14, 15}),
         Renner0Element({0, 1, 2, 3, 4, 6, 5, 7, 8, 10, 9, 11, 12, 13, 14, 15}),
         Renner0Element({0, 1, 2, 3, 5, 4, 6, 7, 8, 9, 11, 10, 12, 13, 14, 15}),
         Renner0Element({0, 1, 2, 4, 3, 5, 6, 7, 8, 9, 10, 12, 11, 13, 14, 15}),
         Renner0Element(
             {0, 1, 3, 2, 4, 5, 6, 7, 8, 9, 10, 11, 13, 12, 14, 15})});
    // Note that the number in this test file was 8962225 since (at least) this
    // file was renamed from .cc to .cpp, but the current value below ?? was
    // the returned value since at least commit 4d39875 (the first commit after
    // the file was renamed where the tests worked).
    REQUIRE(S.size() == 1793622);
    // Same problem as above, the value below was 128.
    REQUIRE(S.nr_idempotents() == 158716);
  }

  LIBSEMIGROUPS_TEST_CASE("HPCombi",
                          "013",
                          "full transformation monoid 8",
                          "[extreme][hpcombi]") {
    auto                  rg = ReportGuard(true);
    FroidurePin<Transf16> S({Transf16({1, 2, 3, 4, 5, 6, 7, 0}),
                             Transf16({1, 0, 2, 3, 4, 5, 6, 7}),
                             Transf16({0, 1, 2, 3, 4, 5, 6, 0})});
    // FIXME
    // 1. including the next line makes this test run extremely slowly
    // (20/09/2019) under clang.
    // 2. Without the next line this is no faster than the next test.
    // 3. Does not appear to be an issue under gcc (FLS) - takes ~7s
    // S.reserve(2 * std::pow(8, 8));
    REQUIRE(S.size() == 16777216);
  }

  LIBSEMIGROUPS_TEST_CASE("HPCombi",
                          "014",
                          "full transformation monoid 8",
                          "[extreme][hpcombi]") {
    using Transf           = libsemigroups::Transformation<uint_fast8_t>;
    auto                rg = ReportGuard(true);
    FroidurePin<Transf> S({Transf({1, 2, 3, 4, 5, 6, 7, 0}),
                           Transf({1, 0, 2, 3, 4, 5, 6, 7}),
                           Transf({0, 1, 2, 3, 4, 5, 6, 0})});
    S.reserve(std::pow(8, 8));
    REQUIRE(S.size() == 16777216);
  }
}  // namespace libsemigroups
#endif
