//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2020 James D. Mitchell
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

#include "bench-main.hpp"  // for LIBSEMIGROUPS_BENCHMARK
#include "catch.hpp"       // for REQUIRE, REQUIRE_NOTHROW, REQUIRE_THROWS_AS

#include "libsemigroups/action.hpp"            // for LeftAction
#include "libsemigroups/bitset.hpp"            // for BitSet
#include "libsemigroups/element-adapters.hpp"  // for Lambda
#include "libsemigroups/element-helper.hpp"    // for PPermHelper
#include "libsemigroups/element.hpp"           // for PartialPerm
#include "libsemigroups/report.hpp"            // for ReportGuard

#define FOR_SET_BITS(__bit_int, __nr_bits, __variable) \
  uint_fast32_t block = __bit_int;                     \
  while (block != 0) {                                 \
    uint_fast32_t t          = block & -block;         \
    int           __variable = __builtin_ctzll(block); \
    if (__variable >= __nr_bits) {                     \
      break;                                           \
    }

#define END_FOR_SET_BITS \
  block ^= t;            \
  }

static constexpr uint_fast32_t MASK[32]
    = {0x1,        0x2,       0x4,       0x8,       0x10,       0x20,
       0x40,       0x80,      0x100,     0x200,     0x400,      0x800,
       0x1000,     0x2000,    0x4000,    0x8000,    0x10000,    0x20000,
       0x40000,    0x80000,   0x100000,  0x200000,  0x400000,   0x800000,
       0x1000000,  0x2000000, 0x4000000, 0x8000000, 0x10000000, 0x20000000,
       0x40000000, 0x80000000};

namespace libsemigroups {
  // Basic version of a function using BitSet in element.hpp, retained for the
  // purposes of comparison.
  template <typename TIntType>
  struct ImageRightAction<PartialPerm<TIntType>, uint_fast32_t> {
    void operator()(uint_fast32_t&               res,
                    uint_fast32_t const&         pt,
                    PartialPerm<TIntType> const& x) const {
      res = 0;
      FOR_SET_BITS(pt, static_cast<int>(x.degree()), i) {
        if (x[i] != UNDEFINED) {
          res |= MASK[x[i]];
        }
      }
      END_FOR_SET_BITS
    }
  };

  // Basic version of ImageRightAction for BooleanMat and BitSet, retained for
  // the purposes of comparison.
  template <size_t N>
  struct ImageRightAction2 {
    using result_type = BitSet<N>;
    void operator()(result_type&       res,
                    result_type const& pt,
                    BooleanMat const&  x) const {
      res.reset();
      pt.apply([&x, &res](size_t i) {
        for (size_t j = 0; j < x.degree(); ++j) {
          res.set(j, res[j] || x[i * x.degree() + j]);
        }
      });
    }
  };

  template <typename T>
  void benchmark_example1(T& o) {
    using PPerm = PPermHelper<17>::type;
    o.add_generator(
        PPerm({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16},
              {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 0},
              17));
    o.add_generator(
        PPerm({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16},
              {1, 0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16},
              17));
    o.add_generator(
        PPerm({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16},
              {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
              17));
    o.add_generator(
        PPerm({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
              {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16},
              17));
  }

  template <typename T>
  void benchmark_example1_inverse(T& o) {
    using PPerm = PPermHelper<17>::type;
    o.add_generator(
        PPerm({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16},
              {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 0},
              17));
    o.add_generator(
        PPerm({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16},
              {1, 0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16},
              17));
    o.add_generator(
        PPerm({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16},
              {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
              17)
            .inverse());
    o.add_generator(
        PPerm({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
              {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16},
              17)
            .inverse());
  }

  template <typename T>
  void transf_example1(T& o) {
    using Transf = TransfHelper<17>::type;
    o.add_generator(
        Transf({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 0}));
    o.add_generator(
        Transf({1, 0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16}));
    o.add_generator(
        Transf({0, 0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16}));
  }

  template <typename T>
  void transf_example2(T& o) {
    using Transf = Transformation<uint_fast8_t>;
    o.add_generator(Transf({1, 2, 3, 4, 5, 6, 7, 8, 9, 0}));
    o.add_generator(Transf({1, 0, 2, 3, 4, 5, 6, 7, 8, 9}));
    o.add_generator(Transf({0, 0, 2, 3, 4, 5, 6, 7, 8, 9}));
  }

  template <typename T>
  void booleanmat_example1(T& o) {
    using BMat = typename T::element_type;
    /*o.add_generator(
        BooleanMat({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}));
    o.add_generator(
        BooleanMat({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}));
    o.add_generator(
        BooleanMat({{1, 0, 0, 0}, {1, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}));
    o.add_generator(
        BooleanMat({{0, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}));*/
    o.add_generator(BMat({{0, 1, 0, 0, 0},
                          {1, 0, 0, 0, 0},
                          {0, 0, 1, 0, 0},
                          {0, 0, 0, 1, 0},
                          {0, 0, 0, 0, 1}}));
    o.add_generator(BMat({{0, 1, 0, 0, 0},
                          {0, 0, 1, 0, 0},
                          {0, 0, 0, 1, 0},
                          {0, 0, 0, 0, 1},
                          {1, 0, 0, 0, 0}}));
    o.add_generator(BMat({{1, 0, 0, 0, 0},
                          {0, 1, 0, 0, 0},
                          {0, 0, 1, 0, 0},
                          {0, 0, 0, 1, 0},
                          {1, 0, 0, 0, 1}}));
    o.add_generator(BMat({{1, 0, 0, 0, 0},
                          {0, 1, 0, 0, 0},
                          {0, 0, 1, 0, 0},
                          {0, 0, 0, 1, 0},
                          {0, 0, 0, 0, 0}}));
  }

  template <typename T>
  void booleanmat_example2(T& o) {
    using BMat = typename T::element_type;
    /*o.add_generator(
        BooleanMat({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}));
    o.add_generator(
        BooleanMat({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}));
    o.add_generator(
        BooleanMat({{1, 0, 0, 0}, {1, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}));
    o.add_generator(
        BooleanMat({{0, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}));*/
    o.add_generator(BMat({{0, 1, 0, 0, 0, 0, 0, 0, 0},
                          {1, 0, 0, 0, 0, 0, 0, 0, 0},
                          {0, 0, 1, 0, 0, 0, 0, 0, 0},
                          {0, 0, 0, 1, 0, 0, 0, 0, 0},
                          {0, 0, 0, 0, 1, 0, 0, 0, 0},
                          {0, 0, 0, 0, 0, 1, 0, 0, 0},
                          {0, 0, 0, 0, 0, 0, 1, 0, 0},
                          {0, 0, 0, 0, 0, 0, 0, 1, 0},
                          {0, 0, 0, 0, 0, 0, 0, 0, 1}}));
    o.add_generator(BMat({{0, 1, 0, 0, 0, 0, 0, 0, 0},
                          {0, 0, 1, 0, 0, 0, 0, 0, 0},
                          {0, 0, 0, 1, 0, 0, 0, 0, 0},
                          {0, 0, 0, 0, 1, 0, 0, 0, 0},
                          {0, 0, 0, 0, 0, 1, 0, 0, 0},
                          {0, 0, 0, 0, 0, 0, 1, 0, 0},
                          {0, 0, 0, 0, 0, 0, 0, 1, 0},
                          {0, 0, 0, 0, 0, 0, 0, 0, 1},
                          {1, 0, 0, 0, 0, 0, 0, 0, 0}}));
    o.add_generator(BMat({{1, 0, 0, 0, 0, 0, 0, 0, 0},
                          {0, 1, 0, 0, 0, 0, 0, 0, 0},
                          {0, 0, 1, 0, 0, 0, 0, 0, 0},
                          {0, 0, 0, 1, 0, 0, 0, 0, 0},
                          {0, 0, 0, 0, 1, 0, 0, 0, 0},
                          {0, 0, 0, 0, 0, 1, 0, 0, 0},
                          {0, 0, 0, 0, 0, 0, 1, 0, 0},
                          {0, 0, 0, 0, 0, 0, 0, 1, 0},
                          {1, 0, 0, 0, 0, 0, 0, 0, 1}}));
    o.add_generator(BMat({{1, 0, 0, 0, 0, 0, 0, 0, 0},
                          {0, 1, 0, 0, 0, 0, 0, 0, 0},
                          {0, 0, 1, 0, 0, 0, 0, 0, 0},
                          {0, 0, 0, 1, 0, 0, 0, 0, 0},
                          {0, 0, 0, 0, 1, 0, 0, 0, 0},
                          {0, 0, 0, 0, 0, 1, 0, 0, 0},
                          {0, 0, 0, 0, 0, 0, 1, 0, 0},
                          {0, 0, 0, 0, 0, 0, 0, 1, 0},
                          {0, 0, 0, 0, 0, 0, 0, 0, 0}}));
  }

  TEST_CASE("Slowest PartialPerm", "[quick][001][pperm]") {
    auto rg     = ReportGuard(false);
    using PPerm = PPermHelper<17>::type;
    BENCHMARK("ImageRightAction = PartialPerm") {
      RightAction<PPerm, PPerm, ImageRightAction<PPerm, PPerm>> o;
      o.add_seed(One<PPerm>()(17));
      benchmark_example1(o);
      REQUIRE(o.size() == 131072);
    };

    BENCHMARK("ImageLeftAction = PartialPerm") {
      LeftAction<PPerm, PPerm, ImageLeftAction<PPerm, PPerm>> o;
      o.add_seed(One<PPerm>()(17));
      benchmark_example1(o);
      REQUIRE(o.size() == 131072);
    };
  }

  TEST_CASE("Second slowest PartialPerm", "[quick][002][pperm]") {
    auto rg        = ReportGuard(false);
    using PPerm    = PPermHelper<17>::type;
    using int_type = uint_fast8_t;
    BENCHMARK("ImageRightAction = vector") {
      RightAction<PPerm,
                  std::vector<int_type>,
                  ImageRightAction<PPerm, std::vector<int_type>>>
          o;
      o.add_seed({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16});
      benchmark_example1(o);
      REQUIRE(o.size() == 131072);
    };
    BENCHMARK("ImageLeftAction = vector") {
      LeftAction<PPerm,
                 std::vector<int_type>,
                 ImageLeftAction<PPerm, std::vector<int_type>>>
          o;
      o.add_seed({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16});
      benchmark_example1(o);
      REQUIRE(o.size() == 131072);
    };
  }

  TEST_CASE("Second fastest PartialPerm", "[quick][005][pperm]") {
    auto rg        = ReportGuard(false);
    using PPerm    = PPermHelper<17>::type;
    using int_type = uint_fast8_t;
    BENCHMARK("ImageRightAction = array/StaticVector1") {
      RightAction<PPerm,
                  detail::StaticVector1<int_type, 17>,
                  ImageRightAction<PPerm, detail::StaticVector1<int_type, 17>>>
          o;
      o.add_seed({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16});
      benchmark_example1(o);
      REQUIRE(o.size() == 131072);
    };
    BENCHMARK("ImageLeftAction = array/StaticVector1") {
      LeftAction<PPerm,
                 detail::StaticVector1<int_type, 17>,
                 ImageLeftAction<PPerm, detail::StaticVector1<int_type, 17>>>
          o;
      o.add_seed({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16});
      benchmark_example1(o);
      REQUIRE(o.size() == 131072);
    };
  }

  TEST_CASE("fastest PartialPerm", "[quick][004][pperm]") {
    auto rg     = ReportGuard(false);
    using PPerm = PPermHelper<17>::type;
    BENCHMARK("ImageRightAction = BitSets") {
      RightAction<PPerm, BitSet<17>, ImageRightAction<PPerm, BitSet<17>>> o;

      BitSet<17> sd;
      sd.set();
      // sd.set(0, 17, true);

      // sd.reset();
      // for (size_t i = 0; i < 17; ++i) {
      //  REQUIRE(sd.count() == i);
      //  sd.set(i);
      // }

      REQUIRE(sd.size() == 17);
      REQUIRE(sd.count() == 17);
      o.add_seed(sd);
      benchmark_example1(o);
      // REQUIRE(std::all_of(
      //    o.cbegin(), o.cend(), [](BitSet<17> i) { return i.count() == 17;
      //    }));
      REQUIRE(o.size() == 131072);
    };

    BENCHMARK("ImageLeftAction (via inverses) = BitSets") {
      RightAction<PPerm, BitSet<17>, ImageRightAction<PPerm, BitSet<17>>> o;

      BitSet<17> sd;
      sd.set();

      REQUIRE(sd.size() == 17);
      REQUIRE(sd.count() == 17);
      o.add_seed(sd);
      benchmark_example1_inverse(o);
      REQUIRE(o.size() == 131072);
    };

    BENCHMARK("ImageLeftAction = BitSets") {
      LeftAction<PPerm, BitSet<17>, ImageLeftAction<PPerm, BitSet<17>>> o;

      BitSet<17> sd;
      sd.set();
      REQUIRE(sd.size() == 17);
      REQUIRE(sd.count() == 17);
      o.add_seed(sd);
      benchmark_example1(o);
      // REQUIRE(std::all_of(
      //    o.cbegin(), o.cend(), [](BitSet<17> const& bs) { return bs.count()
      //    == 17; }));

      REQUIRE(o.size() == 131072);
    };
  }

  TEST_CASE("fastest PartialPerm comparison", "[quick][003][pperm]") {
    auto rg     = ReportGuard(false);
    using PPerm = PPermHelper<17>::type;
    BENCHMARK("ImageRightAction = naked bitset") {
      RightAction<PPerm, uint_fast32_t, ImageRightAction<PPerm, uint_fast32_t>>
          o;
      o.add_seed(static_cast<uint_fast32_t>(-1));
      benchmark_example1(o);
      REQUIRE(o.size() == 131073);
      // The above is off-by-one because the seed corresponds to a set of size
      // 32
    };
  }

  TEST_CASE("ImageRightAction for transformations", "[quick][006][transf]") {
    auto rg        = ReportGuard(false);
    using Transf   = TransfHelper<17>::type;
    using int_type = uint_fast8_t;
    BENCHMARK("vectors") {
      RightAction<Transf,
                  std::vector<int_type>,
                  ImageRightAction<Transf, std::vector<int_type>>>
          o;
      o.add_seed({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16});
      transf_example1(o);
      REQUIRE(o.size() == 131071);
    };
    BENCHMARK("array/StaticVector1") {
      RightAction<Transf,
                  detail::StaticVector1<int_type, 17>,
                  ImageRightAction<Transf, detail::StaticVector1<int_type, 17>>>
          o;
      o.add_seed({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16});
      transf_example1(o);
      REQUIRE(o.size() == 131071);
    };

    BENCHMARK("BitSets") {
      RightAction<Transf, BitSet<17>, ImageRightAction<Transf, BitSet<17>>> o;
      BitSet<17>                                                            sd;
      sd.set();
      REQUIRE(sd.size() == 17);
      REQUIRE(sd.count() == 17);
      o.add_seed(sd);
      transf_example1(o);
      REQUIRE(o.size() == 131071);
    };
  }

  TEST_CASE("ImageLeftAction for transformations", "[quick][007][transf]") {
    auto rg        = ReportGuard(false);
    using int_type = uint_fast8_t;
    using Transf   = Transformation<int_type>;
    BENCHMARK("vectors") {
      LeftAction<Transf,
                 std::vector<int_type>,
                 ImageLeftAction<Transf, std::vector<int_type>>>
          o;
      o.add_seed({0, 1, 2, 3, 4, 5, 6, 7, 8, 9});
      transf_example2(o);
      REQUIRE(o.size() == 115975);
    };
    BENCHMARK("array/StaticVector1") {
      LeftAction<Transf,
                 detail::StaticVector1<int_type, 10>,
                 ImageLeftAction<Transf, detail::StaticVector1<int_type, 10>>>
          o;
      o.add_seed({0, 1, 2, 3, 4, 5, 6, 7, 8, 9});
      transf_example2(o);
      REQUIRE(o.size() == 115975);
    };
  }

  TEST_CASE("ImageRightAction for boolean mats", "[quick][008][BooleanMat]") {
    auto rg = ReportGuard(false);
    BENCHMARK("StaticVector1 of BitSets") {
      RightAction<
          BooleanMat,
          detail::StaticVector1<BitSet<5>, 5>,
          ImageRightAction<BooleanMat, detail::StaticVector1<BitSet<5>, 5>>>
                                          o;
      detail::StaticVector1<BitSet<5>, 5> seed;
      for (size_t i = 0; i < 5; ++i) {
        BitSet<5> x;
        x.reset();
        x.set(i);
        seed.push_back(x);
      }
      o.add_seed(seed);
      booleanmat_example1(o);
      REQUIRE(o.size() == 110518);
    };

    BENCHMARK("vectors of BitSets") {
      RightAction<BooleanMat,
                  std::vector<BitSet<5>>,
                  ImageRightAction<BooleanMat, std::vector<BitSet<5>>>>
                             o;
      std::vector<BitSet<5>> seed;
      for (size_t i = 0; i < 5; ++i) {
        BitSet<5> x;
        x.reset();
        x.set(i);
        seed.push_back(x);
      }
      o.add_seed(seed);
      booleanmat_example1(o);
      REQUIRE(o.size() == 110518);
    };

    BENCHMARK("vectors of vectors") {
      RightAction<BooleanMat,
                  std::vector<std::vector<bool>>,
                  ImageRightAction<BooleanMat, std::vector<std::vector<bool>>>>
          o;
      o.add_seed(std::vector<std::vector<bool>>({{1, 0, 0, 0, 0},
                                                 {0, 1, 0, 0, 0},
                                                 {0, 0, 1, 0, 0},
                                                 {0, 0, 0, 1, 0},
                                                 {0, 0, 0, 0, 1}}));
      booleanmat_example1(o);
      REQUIRE(o.size() == 110519);
    };
  }

  TEST_CASE("ImageRightAction for BMat8", "[quick][009][bmat8]") {
    auto rg = ReportGuard(false);
    BENCHMARK("BMat8") {
      RightAction<BMat8, BMat8, ImageRightAction<BMat8, BMat8>> o;
      o.add_seed(BMat8({{1, 0, 0, 0, 0},
                        {0, 1, 0, 0, 0},
                        {0, 0, 1, 0, 0},
                        {0, 0, 0, 1, 0},
                        {0, 0, 0, 0, 1}}));
      booleanmat_example1(o);
      REQUIRE(o.size() == 110518);
    };
  }

  TEST_CASE("ImageRightAction for boolean mats and single bitsets",
            "[quick][010][BooleanMat]") {
    auto rg = ReportGuard(false);
    BENCHMARK("Basic BitSets action") {
      RightAction<BooleanMat, BitSet<9>, ImageRightAction2<9>> o;
      for (size_t i = 0; i < 9; ++i) {
        BitSet<9> x;
        x.reset();
        x.set(i);
        o.add_seed(x);
      }
      booleanmat_example2(o);
      REQUIRE(o.size() == 512);
    };

    BENCHMARK("Convert matrix action") {
      RightAction<BooleanMat,
                  BitSet<9>,
                  ImageRightAction<BooleanMat, BitSet<9>>>
          o;
      for (size_t i = 0; i < 9; ++i) {
        BitSet<9> x;
        x.reset();
        x.set(i);
        o.add_seed(x);
      }
      booleanmat_example2(o);
      REQUIRE(o.size() == 512);
    };
  }
}  // namespace libsemigroups
