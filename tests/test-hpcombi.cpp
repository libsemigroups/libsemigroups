//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019-2026 James D. Mitchell
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

// Must include hpcombi.hpp so that LIBSEMIGROUPS_HPCOMBI_ENABLED is defined,
// if so specified at during configure.
#include "libsemigroups/config.hpp"  // for LIBSEMIGROUPS_HPCOMBI_ENABLED

#ifdef LIBSEMIGROUPS_HPCOMBI_ENABLED
#include <cstddef>  // for size_t

#include "Catch2-3.8.0/catch_amalgamated.hpp"  // for REQUIRE
#include "test-main.hpp"                       // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/action.hpp"
#include "libsemigroups/exception.hpp"
#include "libsemigroups/froidure-pin.hpp"  // for FroidurePin
#include "libsemigroups/hpcombi.hpp"       // for HPCombi::PTransf16, ...
#include "libsemigroups/transf.hpp"        // for Transf<>

#include "libsemigroups/detail/int-range.hpp"  // for detail::IntRange
#include "libsemigroups/detail/report.hpp"     // for ReportGuard

namespace {
  // Note that Renner0Element appears to require very little to make it a
  // template argument of FroidurePin, but in actual fact because it inherits
  // from HPCombi::PTransf16, most of the required specialisations of
  // One, etc, are in include/hpcombi.hpp.

  struct Renner0Element : public HPCombi::PTransf16 {
    using PTransf16::PTransf16;

    Renner0Element operator*(Renner0Element const& y) const {
      Renner0Element minab, maxab, mask, b = permuted(y);
      mask  = simde_mm_cmplt_epi8(y, HPCombi::Perm16::one());
      minab = simde_mm_min_epi8(v, b);
      maxab = simde_mm_max_epi8(v, b);
      return static_cast<HPCombi::epu8>(
                 simde_mm_blendv_epi8(maxab, minab, mask))
             | (y.v == HPCombi::Epu8(0xFF));
    }
  };
}  // namespace

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

  LIBSEMIGROUPS_TEST_CASE("HPCombi",
                          "000",
                          "make<HPCombi::Transf16>",
                          "[quick][hpcombi]") {
    auto rg = ReportGuard(false);
    auto S  = make<FroidurePin>({make<HPCombi::Transf16>({1, 2, 0})});
    REQUIRE(S.size() == 3);
    REQUIRE(S.number_of_idempotents() == 1);
    REQUIRE(std::vector(S.cbegin_sorted(), S.cend_sorted())
            == std::vector({make<HPCombi::Transf16>({0, 1, 2}),
                            make<HPCombi::Transf16>({1, 2, 0}),
                            make<HPCombi::Transf16>({2, 0, 1})}));
  }

  LIBSEMIGROUPS_TEST_CASE("HPCombi",
                          "001",
                          "One specialisation",
                          "[quick][hpcombi]") {
    auto id = One<HPCombi::Transf16>()(10);
    auto x  = make<HPCombi::Transf16>({3, 2, 3, 4, 5, 3, 0, 1});
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
    auto x = make<HPCombi::Transf16>({0, 0, 0, 0, 0, 0, 0, 0});
    auto y = make<HPCombi::Transf16>({1, 1, 1, 1, 1, 1, 1, 1});
    Swap<HPCombi::Transf16>()(x, y);
    REQUIRE(x == make<HPCombi::Transf16>({1, 1, 1, 1, 1, 1, 1, 1}));
    REQUIRE(y == make<HPCombi::Transf16>({0, 0, 0, 0, 0, 0, 0, 0}));
  }

  LIBSEMIGROUPS_TEST_CASE("HPCombi",
                          "004",
                          "Swap specialisation",
                          "[quick][hpcombi]") {
    auto x = Renner0Element({0xFF,
                             0xFF,
                             0xFF,
                             0xFF,
                             0xFF,
                             0xFF,
                             0xFF,
                             0xFF,
                             8,
                             9,
                             10,
                             11,
                             12,
                             13,
                             14,
                             15});
    auto y = Renner0Element(
        {0, 1, 2, 4, 3, 5, 6, 7, 8, 9, 10, 12, 11, 13, 14, 15});
    Swap<Renner0Element>()(x, y);
    REQUIRE(x
            == Renner0Element(
                {0, 1, 2, 4, 3, 5, 6, 7, 8, 9, 10, 12, 11, 13, 14, 15}));
    REQUIRE(y
            == Renner0Element({0xFF,
                               0xFF,
                               0xFF,
                               0xFF,
                               0xFF,
                               0xFF,
                               0xFF,
                               0xFF,
                               8,
                               9,
                               10,
                               11,
                               12,
                               13,
                               14,
                               15}));
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("HPCombi",
                                   "015",
                                   "IncreaseDegree",
                                   "[quick][hpcombi]",
                                   HPCombi::Perm16,
                                   HPCombi::PPerm16,
                                   HPCombi::Transf16,
                                   HPCombi::PTransf16) {
    auto x = TestType({0, 2, 1, 4, 5, 3});
    REQUIRE(x.size() == 16);
    IncreaseDegree<TestType>{}(x, 11'212);
    REQUIRE(x.size() == 16);
  }

  LIBSEMIGROUPS_TEST_CASE("HPCombi",
                          "005",
                          "Inverse specialisation",
                          "[quick][hpcombi]") {
    auto id = One<HPCombi::Perm16>()(10);
    auto x  = HPCombi::Perm16({0, 2, 1, 4, 5, 3});
    auto y  = Inverse<HPCombi::Perm16>()(x);
    REQUIRE(x * y == id);
    REQUIRE(y * x == id);
  }

  LIBSEMIGROUPS_TEST_CASE("HPCombi",
                          "006",
                          "ImageRightAction<HPCombi::Perm16, int>",
                          "[quick][hpcombi]") {
    auto x = HPCombi::Perm16({0, 2, 1, 4, 5, 3});
    REQUIRE(ImageRightAction<HPCombi::Perm16, int>()(0, x) == 0);
    REQUIRE(ImageRightAction<HPCombi::Perm16, int>()(1, x) == 2);
    REQUIRE(ImageRightAction<HPCombi::Perm16, int>()(2, x) == 1);
    REQUIRE(ImageRightAction<HPCombi::Perm16, int>()(3, x) == 4);
    REQUIRE(ImageRightAction<HPCombi::Perm16, int>()(4, x) == 5);
    REQUIRE(ImageRightAction<HPCombi::Perm16, int>()(5, x) == 3);

    int pt;
    ImageRightAction<HPCombi::Perm16, int>()(pt, 0, x);
    REQUIRE(pt == 0);
    ImageRightAction<HPCombi::Perm16, int>()(pt, 1, x);
    REQUIRE(pt == 2);
    ImageRightAction<HPCombi::Perm16, int>()(pt, 2, x);
    REQUIRE(pt == 1);
    ImageRightAction<HPCombi::Perm16, int>()(pt, 3, x);
    REQUIRE(pt == 4);
    ImageRightAction<HPCombi::Perm16, int>()(pt, 4, x);
    REQUIRE(pt == 5);
    ImageRightAction<HPCombi::Perm16, int>()(pt, 5, x);
    REQUIRE(pt == 3);

    auto id = One<HPCombi::Perm16>()(10);
    auto r  = detail::IntRange<int>(0, 10);
    REQUIRE(std::all_of(r.cbegin(), r.cend(), [&id](int y) {
      return ImageRightAction<HPCombi::Perm16, int>()(y, id) == y;
    }));
    REQUIRE(std::all_of(r.cbegin(), r.cend(), [&id](int y) {
      int qt;
      ImageRightAction<HPCombi::Perm16, int>()(qt, y, id);
      return qt == y;
    }));
  }

  LIBSEMIGROUPS_TEST_CASE(
      "HPCombi",
      "007",
      "ImageLeft/RightAction<HPCombi::PPerm16, HPCombi::PPerm16>",
      "[quick][hpcombi]") {
    auto             id = One<HPCombi::PPerm16>()(5);
    auto             x  = HPCombi::PPerm16({10}, {0});
    auto             y  = HPCombi::PPerm16({1}, {2});
    HPCombi::PPerm16 res;
    ImageRightAction<HPCombi::PPerm16, HPCombi::PPerm16>()(res, id, x);
    REQUIRE(res == HPCombi::PPerm16({0}, {0}));
    ImageRightAction<HPCombi::PPerm16, HPCombi::PPerm16>()(res, x, id);
    REQUIRE(res == HPCombi::PPerm16({0}, {0}));
    ImageRightAction<HPCombi::PPerm16, HPCombi::PPerm16>()(res, x, y);
    REQUIRE(res == HPCombi::PPerm16({}, {}));
    ImageRightAction<HPCombi::PPerm16, HPCombi::PPerm16>()(res, y, x);
    REQUIRE(res == HPCombi::PPerm16({}, {}));
    ImageRightAction<HPCombi::PPerm16, HPCombi::PPerm16>()(res, y, id);
    REQUIRE(res == HPCombi::PPerm16({2}, {2}));
    ImageRightAction<HPCombi::PPerm16, HPCombi::PPerm16>()(res, id, y);
    REQUIRE(res == HPCombi::PPerm16({2}, {2}));

    ImageLeftAction<HPCombi::PPerm16, HPCombi::PPerm16>()(res, id, x);
    REQUIRE(res == HPCombi::PPerm16({10}, {10}));
    ImageLeftAction<HPCombi::PPerm16, HPCombi::PPerm16>()(res, x, id);
    REQUIRE(res == HPCombi::PPerm16({10}, {10}));
    ImageLeftAction<HPCombi::PPerm16, HPCombi::PPerm16>()(res, x, y);
    REQUIRE(res == HPCombi::PPerm16({}, {}));
    ImageLeftAction<HPCombi::PPerm16, HPCombi::PPerm16>()(res, y, x);
    REQUIRE(res == HPCombi::PPerm16({}, {}));
    ImageLeftAction<HPCombi::PPerm16, HPCombi::PPerm16>()(res, y, id);
    REQUIRE(res == HPCombi::PPerm16({1}, {1}));
    ImageLeftAction<HPCombi::PPerm16, HPCombi::PPerm16>()(res, id, y);
    REQUIRE(res == HPCombi::PPerm16({1}, {1}));
  }

  LIBSEMIGROUPS_TEST_CASE("HPCombi",
                          "008",
                          "Product<Renner0Element>",
                          "[quick][hpcombi]") {
    auto id = One<Renner0Element>()(5);
    auto x  = Renner0Element(
        {0, 1, 2, 3, 4, 5, 6, 8, 7, 9, 10, 11, 12, 13, 14, 15});
    auto           y = Renner0Element({0xFF,
                                       0xFF,
                                       0xFF,
                                       0xFF,
                                       0xFF,
                                       0xFF,
                                       0xFF,
                                       0xFF,
                                       8,
                                       9,
                                       10,
                                       11,
                                       12,
                                       13,
                                       14,
                                       15});
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

  LIBSEMIGROUPS_TEST_CASE("HPCombi",
                          "010",
                          "make<HPCombi::Transf16>",
                          "[standard][hpcombi]") {
    auto rg = ReportGuard(false);
    auto S  = make<FroidurePin>(
        {make<HPCombi::Transf16>({1, 7, 2, 6, 0, 4, 1, 5}),
          make<HPCombi::Transf16>({2, 4, 6, 1, 4, 5, 2, 7}),
          make<HPCombi::Transf16>({3, 0, 7, 2, 4, 6, 2, 4}),
          make<HPCombi::Transf16>({3, 2, 3, 4, 5, 3, 0, 1}),
          make<HPCombi::Transf16>({4, 3, 7, 7, 4, 5, 0, 4}),
          make<HPCombi::Transf16>({5, 6, 3, 0, 3, 0, 5, 1}),
          make<HPCombi::Transf16>({6, 0, 1, 1, 1, 6, 3, 4}),
          make<HPCombi::Transf16>({7, 7, 4, 0, 6, 4, 1, 7})});
    S.reserve(600000);
    REQUIRE(S.size() == 597369);
  }

  LIBSEMIGROUPS_TEST_CASE("HPCombi",
                          "011",
                          "make<HPCombi::Transf16>",
                          "[standard][hpcombi]") {
    auto rg = ReportGuard(false);

    using Transf = libsemigroups::Transf<>;
    auto S       = make<FroidurePin>({Transf({1, 7, 2, 6, 0, 4, 1, 5}),
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

  LIBSEMIGROUPS_TEST_CASE("HPCombi", "012", "Renner0", "[standard][hpcombi]") {
    auto rg = ReportGuard(false);
    auto S  = make<FroidurePin>(
        {Renner0Element({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}),
          Renner0Element({0xFF,
                          0xFF,
                          0xFF,
                          0xFF,
                          0xFF,
                          0xFF,
                          0xFF,
                          0xFF,
                          8,
                          9,
                          10,
                          11,
                          12,
                          13,
                          14,
                          15}),
          // NOLINTBEGIN(whitespace/line_length)
          Renner0Element({0, 1, 2, 3, 4, 5, 6, 8, 7, 9, 10, 11, 12, 13, 14, 15}),
          Renner0Element({0, 1, 2, 3, 4, 5, 7, 6, 9, 8, 10, 11, 12, 13, 14, 15}),
          Renner0Element({0, 1, 2, 3, 4, 6, 5, 7, 8, 10, 9, 11, 12, 13, 14, 15}),
          Renner0Element({0, 1, 2, 3, 5, 4, 6, 7, 8, 9, 11, 10, 12, 13, 14, 15}),
          Renner0Element({0, 1, 2, 4, 3, 5, 6, 7, 8, 9, 10, 12, 11, 13, 14, 15}),
          // NOLINTEND
          Renner0Element(
             {0, 1, 3, 2, 4, 5, 6, 7, 8, 9, 10, 11, 13, 12, 14, 15})});
    // Note that the number in this test file was 8962225 since (at least) this
    // file was renamed from .cc to .cpp, but the current value below ?? was
    // the returned value since at least commit 4d39875 (the first commit after
    // the file was renamed where the tests worked).
    REQUIRE(S.size() == 1793622);
    // Same problem as above, the value below was 128.
    REQUIRE(S.number_of_idempotents() == 158716);
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("HPCombi",
                                   "013",
                                   "full transformation monoid 8",
                                   "[extreme][hpcombi]",
                                   HPCombi::Transf16,
                                   Transf<8>,
                                   Transf<>) {
    auto rg = ReportGuard(true);
    auto S  = make<FroidurePin>({make<TestType>({1, 2, 3, 4, 5, 6, 7, 0}),
                                 make<TestType>({1, 0, 2, 3, 4, 5, 6, 7}),
                                 make<TestType>({0, 1, 2, 3, 4, 5, 6, 0})});
    // 1. including the next line makes this test run extremely slowly
    // (20/09/2019) under clang.
    // 2. Without the next line this is no faster than the next test.
    // 3. Does not appear to be an issue under gcc (FLS) - takes ~7s
    // It seems that if we put 8 ^ 8 in here for reserve, then since it is
    // divisible by 16 something horrible goes wrong in the hashing when
    // compiled with clang. Choosing a prime, makes this fast again.
    S.reserve(2 * 16777259);
    REQUIRE(S.size() == 16777216);
  }

  LIBSEMIGROUPS_TEST_CASE("HPCombi",
                          "014",
                          "HPCombi::Transf16 exceptions",
                          "[quick][hpcombi]") {
    REQUIRE_THROWS_AS(make<HPCombi::Transf16>(
                          {1, 2, 3, 4, 5, 6, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(make<HPCombi::Transf16>(
                          {17, 2, 3, 4, 5, 6, 7, 0, 0, 0, 0, 0, 0, 0, 0, 254}),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(make<HPCombi::Transf16>(
                          {17, 2, 3, 4, 5, 6, 7, 0, 0, 0, 0, 0, 0, 0, 0, 17}),
                      LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("HPCombi",
                          "016",
                          "Complexity<HPCombi::BMat8>",
                          "[quick][hpcombi]") {
    REQUIRE(Complexity<HPCombi::BMat8>{}(HPCombi::BMat8()) == 0);
  }

  LIBSEMIGROUPS_TEST_CASE("HPCombi",
                          "018",
                          "Degree<HPCombi::BMat8>",
                          "[quick][hpcombi]") {
    REQUIRE(Degree<HPCombi::BMat8>{}(HPCombi::BMat8()) == 8);
  }

  LIBSEMIGROUPS_TEST_CASE("HPCombi",
                          "019",
                          "IncreaseDegree<HPCombi::BMat8>",
                          "[quick][hpcombi]") {
    HPCombi::BMat8 x;
    REQUIRE(Degree<HPCombi::BMat8>{}(x) == 8);
    IncreaseDegree<HPCombi::BMat8>{}(x, 11'212);
    REQUIRE(Degree<HPCombi::BMat8>{}(x) == 8);
  }

  LIBSEMIGROUPS_TEST_CASE("HPCombi",
                          "020",
                          "One<HPCombi::BMat8>",
                          "[quick][hpcombi]") {
    HPCombi::BMat8 x;
    HPCombi::BMat8 id({{1, 0, 0, 0, 0, 0, 0, 0},
                       {0, 1, 0, 0, 0, 0, 0, 0},
                       {0, 0, 1, 0, 0, 0, 0, 0},
                       {0, 0, 0, 1, 0, 0, 0, 0},
                       {0, 0, 0, 0, 1, 0, 0, 0},
                       {0, 0, 0, 0, 0, 1, 0, 0},
                       {0, 0, 0, 0, 0, 0, 1, 0},
                       {0, 0, 0, 0, 0, 0, 0, 1}});

    REQUIRE(One<HPCombi::BMat8>{}(x) == id);
    REQUIRE(One<HPCombi::BMat8>{}(4) == id);
    REQUIRE(One<HPCombi::BMat8>{}(8) == id);
    REQUIRE(One<HPCombi::BMat8>{}(16) == id);
  }

  LIBSEMIGROUPS_TEST_CASE("HPCombi",
                          "021",
                          "Product<HPCombi::BMat8>",
                          "[quick][hpcombi]") {
    HPCombi::BMat8 x(3230294132);
    HPCombi::BMat8 y(2195952830);
    HPCombi::BMat8 xy;
    Product<HPCombi::BMat8>{}(xy, x, y);
    REQUIRE(xy == x * y);
  }

  LIBSEMIGROUPS_TEST_CASE("HPCombi",
                          "022",
                          "ImageRightAction<HPCombi::BMat8>",
                          "[quick][hpcombi]") {
    HPCombi::BMat8 pt(3230294132);
    HPCombi::BMat8 x(2195952830);
    HPCombi::BMat8 res;
    ImageRightAction<HPCombi::BMat8, HPCombi::BMat8>{}(res, pt, x);
    REQUIRE(res == (pt * x).row_space_basis());
  }

  LIBSEMIGROUPS_TEST_CASE("HPCombi",
                          "023",
                          "ImageLeftAction<HPCombi::BMat8>",
                          "[quick][hpcombi]") {
    HPCombi::BMat8 pt(3230294132);
    HPCombi::BMat8 x(2195952830);
    HPCombi::BMat8 res;
    ImageLeftAction<HPCombi::BMat8, HPCombi::BMat8>{}(res, pt, x);
    REQUIRE(res == (x * pt).col_space_basis());
  }

  LIBSEMIGROUPS_TEST_CASE("HPCombi",
                          "024",
                          "Inverse<HPCombi::BMat8>",
                          "[quick][hpcombi]") {
    HPCombi::BMat8 x({{0, 1, 0}, {0, 0, 1}, {1, 0, 0}});
    REQUIRE(Inverse<HPCombi::BMat8>{}(x) == x.transpose());
    REQUIRE(x * Inverse<HPCombi::BMat8>{}(x)
            == HPCombi::BMat8({{1, 0, 0}, {0, 1, 0}, {0, 0, 1}}));
  }

  LIBSEMIGROUPS_TEST_CASE("HPCombi",
                          "025",
                          "RightAction<HPCombi::PTransf16, HPCombi::PTransf16>",
                          "[quick][hpcombi]") {
    auto rg = ReportGuard(false);
    RightAction<HPCombi::PTransf16,
                HPCombi::PTransf16,
                ImageRightAction<HPCombi::PTransf16, HPCombi::PTransf16>>
        o;
    o.add_seed(HPCombi::PTransf16::one());
    o.add_generator(make<HPCombi::PTransf16>({1, 0, 2, 3, 4}));
    o.add_generator(make<HPCombi::PTransf16>({1, 2, 3, 4, 0}));
    o.add_generator(make<HPCombi::PTransf16>({0, 0, 2, 3, 4}));

    REQUIRE(o.size() == 31);
  }

  LIBSEMIGROUPS_TEST_CASE("HPCombi",
                          "026",
                          "LeftAction<HPCombi::PTransf16, HPCombi::PTransf16>",
                          "[quick][hpcombi]") {
    auto rg = ReportGuard(false);
    LeftAction<HPCombi::PTransf16,
               HPCombi::PTransf16,
               ImageLeftAction<HPCombi::PTransf16, HPCombi::Vect16>>
        o;
    o.add_seed(HPCombi::PTransf16::one());
    o.add_generator(make<HPCombi::PTransf16>({1, 0, 2, 3, 4}));
    o.add_generator(make<HPCombi::PTransf16>({1, 2, 3, 4, 0}));
    o.add_generator(make<HPCombi::PTransf16>({0, 0, 2, 3, 4}));

    REQUIRE(o.size() == 52);
  }

  // TODO more

}  // namespace libsemigroups
#endif
