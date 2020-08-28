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

#include "catch.hpp"                 // for REQUIRE
#include "libsemigroups/bitset.hpp"  // for BitSet
#include "test-main.hpp"             // for LIBSEMIGROUPS_TEST_CASE

namespace libsemigroups {

  template <size_t N>
  void test_bitset_000() {
    BitSet<N> bs;
    REQUIRE(bs.size() == N);
  }

  LIBSEMIGROUPS_TEST_CASE("BitSet", "000", "size", "[bitset][quick]") {
    test_bitset_000<5>();
    test_bitset_000<8>();
    test_bitset_000<10>();
    test_bitset_000<16>();
    test_bitset_000<20>();
    test_bitset_000<32>();
#if LIBSEMIGROUPS_SIZEOF_VOID_P == 8
    test_bitset_000<40>();
    test_bitset_000<64>();
#endif
  }

  template <size_t N>
  void test_bitset_001() {
    BitSet<N> bs1;
    bs1.reset();
    BitSet<N> bs2;
    bs2.reset();
    bs2.set(0);

    REQUIRE(bs1 < bs2);
  }

  LIBSEMIGROUPS_TEST_CASE("BitSet", "001", "operator<", "[bitset][quick]") {
    test_bitset_001<5>();
    test_bitset_001<8>();
    test_bitset_001<10>();
    test_bitset_001<16>();
    test_bitset_001<20>();
    test_bitset_001<32>();
#if LIBSEMIGROUPS_SIZEOF_VOID_P == 8
    test_bitset_001<40>();
    test_bitset_001<64>();
#endif
  }

  template <size_t N>
  void test_bitset_002() {
    BitSet<N> bs1;
    bs1.reset();
    BitSet<N> bs2;
    bs2.reset();
    bs2.set(0);

    REQUIRE(!(bs1 == bs2));
    bs1.set(0);
    REQUIRE(bs1 == bs2);
  }

  LIBSEMIGROUPS_TEST_CASE("BitSet", "002", "operator==", "[bitset][quick]") {
    test_bitset_002<5>();
    test_bitset_002<8>();
    test_bitset_002<10>();
    test_bitset_002<16>();
    test_bitset_002<20>();
    test_bitset_002<32>();
#if LIBSEMIGROUPS_SIZEOF_VOID_P == 8
    test_bitset_002<40>();
    test_bitset_002<64>();
#endif
  }

  template <size_t N>
  void test_bitset_003() {
    BitSet<N> bs1;
    bs1.reset();
    BitSet<N> bs2;
    bs2.reset();
    bs2.set(0);

    REQUIRE(bs1 != bs2);
    bs1.set(0);
    REQUIRE(bs1 == bs2);
  }

  LIBSEMIGROUPS_TEST_CASE("BitSet", "003", "operator!=", "[bitset][quick]") {
    test_bitset_003<5>();
    test_bitset_003<8>();
    test_bitset_003<10>();
    test_bitset_003<16>();
    test_bitset_003<20>();
    test_bitset_003<32>();
#if LIBSEMIGROUPS_SIZEOF_VOID_P == 8
    test_bitset_003<40>();
    test_bitset_003<64>();
#endif
  }

  template <size_t N>
  void test_bitset_004() {
    BitSet<N> bs1;
    bs1.reset();
    bs1.set(0);
    bs1.set(1);
    BitSet<N> bs2;
    bs2.reset();
    bs2.set(1);
    bs1 &= bs2;
    REQUIRE(bs1 == bs2);
    REQUIRE(bs1.count() == 1);
    REQUIRE(bs2.count() == 1);
  }

  LIBSEMIGROUPS_TEST_CASE("BitSet", "004", "operator&=", "[bitset][quick]") {
    test_bitset_004<5>();
    test_bitset_004<8>();
    test_bitset_004<10>();
    test_bitset_004<16>();
    test_bitset_004<20>();
    test_bitset_004<32>();
#if LIBSEMIGROUPS_SIZEOF_VOID_P == 8
    test_bitset_004<40>();
    test_bitset_004<64>();
#endif
  }

  template <size_t N>
  void test_bitset_005() {
    BitSet<N> bs1;
    bs1.reset();
    bs1.set(0);
    bs1.set(1);
    BitSet<N> bs2;
    bs2.reset();
    bs2.set(1);
    BitSet<N> bs3 = bs1 & bs2;
    REQUIRE(bs3 == bs2);
    REQUIRE(&bs3 != &bs2);
    REQUIRE(bs1.count() == 2);
    REQUIRE(bs2.count() == 1);
    REQUIRE(bs3.count() == 1);
  }

  LIBSEMIGROUPS_TEST_CASE("BitSet", "005", "&", "[bitset][quick]") {
    test_bitset_005<5>();
    test_bitset_005<8>();
    test_bitset_005<10>();
    test_bitset_005<16>();
    test_bitset_005<20>();
    test_bitset_005<32>();
#if LIBSEMIGROUPS_SIZEOF_VOID_P == 8
    test_bitset_005<40>();
    test_bitset_005<64>();
#endif
  }

  template <size_t N>
  void test_bitset_006() {
    BitSet<N> bs1;
    bs1.reset();
    bs1.set(0);
    BitSet<N> bs2;
    bs2.reset();
    bs2.set(1);
    bs2 |= bs1;
    REQUIRE(bs1 != bs2);
    REQUIRE(bs2.count() == 2);
    REQUIRE(bs1.count() == 1);
    REQUIRE(bs2.test(0));
    REQUIRE(bs2.test(1));
    REQUIRE(!bs2.test(2));
  }

  LIBSEMIGROUPS_TEST_CASE("BitSet", "006", "operator|=", "[bitset][quick]") {
    test_bitset_006<5>();
    test_bitset_006<8>();
    test_bitset_006<10>();
    test_bitset_006<16>();
    test_bitset_006<20>();
    test_bitset_006<32>();
#if LIBSEMIGROUPS_SIZEOF_VOID_P == 8
    test_bitset_006<40>();
    test_bitset_006<64>();
#endif
  }

  template <size_t N>
  void test_bitset_007() {
    BitSet<N> bs;
    bs.reset();
    bs.set(0);
    bs.set(3);
    bs.set(5);
    REQUIRE(bs[0]);
    REQUIRE(!bs[1]);
    REQUIRE(!bs[2]);
    REQUIRE(bs[3]);
    REQUIRE(!bs[4]);
    REQUIRE(bs[5]);
  }

  LIBSEMIGROUPS_TEST_CASE("BitSet", "007", "operator[]", "[bitset][quick]") {
    test_bitset_007<6>();
    test_bitset_007<8>();
    test_bitset_007<10>();
    test_bitset_007<16>();
    test_bitset_007<20>();
    test_bitset_007<32>();
#if LIBSEMIGROUPS_SIZEOF_VOID_P == 8
    test_bitset_007<40>();
    test_bitset_007<64>();
#endif
  }

  template <size_t N>
  void test_bitset_008() {
    BitSet<N> bs;
    bs.set();
    REQUIRE(bs[0]);
    REQUIRE(bs[1]);
    REQUIRE(bs[2]);
    REQUIRE(bs[3]);
    REQUIRE(bs[4]);
    REQUIRE(bs[5]);
    REQUIRE(bs.count() == N);
  }

  LIBSEMIGROUPS_TEST_CASE("BitSet", "008", "set(none)", "[bitset][quick]") {
    test_bitset_008<6>();
    test_bitset_008<8>();
    test_bitset_008<10>();
    test_bitset_008<16>();
    test_bitset_008<20>();
    test_bitset_008<32>();
#if LIBSEMIGROUPS_SIZEOF_VOID_P == 8
    test_bitset_008<40>();
    test_bitset_008<64>();
#endif
  }

  template <size_t N>
  void test_bitset_009() {
    BitSet<N> bs;
    bs.set();
    bs.set(0, false);
    REQUIRE(!bs[0]);
    REQUIRE(bs[1]);
    REQUIRE(bs[2]);
    REQUIRE(bs[3]);
    REQUIRE(bs[4]);
    REQUIRE(bs[5]);
    REQUIRE(bs.count() == N - 1);
  }

  LIBSEMIGROUPS_TEST_CASE("BitSet",
                          "009",
                          "set(pos, value)",
                          "[bitset][quick]") {
    test_bitset_009<6>();
    test_bitset_009<8>();
    test_bitset_009<10>();
    test_bitset_009<16>();
    test_bitset_009<20>();
    test_bitset_009<32>();
#if LIBSEMIGROUPS_SIZEOF_VOID_P == 8
    test_bitset_009<40>();
    test_bitset_009<64>();
#endif
  }

  template <size_t N>
  void test_bitset_010() {
    BitSet<N> bs;
    bs.reset();
    REQUIRE(bs.count() == 0);
    bs.set(2, 6, true);
    REQUIRE(bs.count() == 4);
    REQUIRE(!bs[0]);
    REQUIRE(!bs[1]);
    REQUIRE(bs[2]);
    REQUIRE(bs[3]);
    REQUIRE(bs[4]);
    REQUIRE(bs[5]);
    REQUIRE(!bs[6]);
  }

  LIBSEMIGROUPS_TEST_CASE("BitSet",
                          "010",
                          "set(first, last, value)",
                          "[bitset][quick]") {
    test_bitset_010<7>();
    test_bitset_010<8>();
    test_bitset_010<10>();
    test_bitset_010<16>();
    test_bitset_010<20>();
    test_bitset_010<32>();
#if LIBSEMIGROUPS_SIZEOF_VOID_P == 8
    test_bitset_010<40>();
    test_bitset_010<64>();
#endif
  }

  template <size_t N>
  void test_bitset_011() {
    BitSet<N> bs;
    bs.set();
    REQUIRE(bs.count() == N);
    bs.reset(2, 6);

    REQUIRE(bs.count() == N - 4);
    REQUIRE(bs[0]);
    REQUIRE(bs[1]);
    REQUIRE(!bs[2]);
    REQUIRE(!bs[3]);
    REQUIRE(!bs[4]);
    REQUIRE(bs[6]);
  }

  LIBSEMIGROUPS_TEST_CASE("BitSet",
                          "011",
                          "reset(first, last)",
                          "[bitset][quick]") {
    test_bitset_011<7>();
    test_bitset_011<8>();
    test_bitset_011<10>();
    test_bitset_011<16>();
    test_bitset_011<20>();
    test_bitset_011<32>();
#if LIBSEMIGROUPS_SIZEOF_VOID_P == 8
    test_bitset_011<40>();
    test_bitset_011<64>();
#endif
  }

  template <size_t N>
  void test_bitset_012() {
    BitSet<N> bs;
    bs.set();
    REQUIRE(bs.count() == N);
    bs.reset(2);
    bs.reset(3);
    bs.reset(4);
    bs.reset(5);

    REQUIRE(bs.count() == N - 4);
    REQUIRE(bs[0]);
    REQUIRE(bs[1]);
    REQUIRE(!bs[2]);
    REQUIRE(!bs[3]);
    REQUIRE(!bs[4]);
    REQUIRE(bs[6]);
  }

  LIBSEMIGROUPS_TEST_CASE("BitSet", "012", "reset(pos)", "[bitset][quick]") {
    test_bitset_012<7>();
    test_bitset_012<8>();
    test_bitset_012<10>();
    test_bitset_012<16>();
    test_bitset_012<20>();
    test_bitset_012<32>();
#if LIBSEMIGROUPS_SIZEOF_VOID_P == 8
    test_bitset_012<40>();
    test_bitset_012<64>();
#endif
  }

  template <size_t N>
  void test_bitset_013() {
    BitSet<N> bs;
    bs.set();
    bs.reset(2);
    bs.reset(3);
    bs.reset(4);
    bs.reset(5);

    std::vector<size_t> expected = {0, 1};
    for (typename BitSet<N>::block_type i = 6; i < N; i++) {
      expected.push_back(i);
    }
    std::vector<size_t> result;

    // hi bits are not nec. set to false
    bs.apply([&result](size_t i) { result.push_back(i); });

    REQUIRE(bs.count() == N - 4);
    // hi bits are set to false
    result.clear();
    bs.apply([&result](size_t i) { result.push_back(i); });
    REQUIRE(result == expected);
  }

  LIBSEMIGROUPS_TEST_CASE("BitSet",
                          "013",
                          "apply (iterate through set bits)",
                          "[bitset][quick]") {
    test_bitset_013<7>();
    test_bitset_013<8>();
    test_bitset_013<10>();
    test_bitset_013<16>();
    test_bitset_013<20>();
    test_bitset_013<32>();
#if LIBSEMIGROUPS_SIZEOF_VOID_P == 8
    test_bitset_013<40>();
    test_bitset_013<64>();
#endif
  }

  template <size_t N>
  void test_bitset_014() {
    BitSet<N> bs;
    bs.reset();
    std::hash<BitSet<N>>()(bs);
  }

  LIBSEMIGROUPS_TEST_CASE("BitSet", "014", "std::hash", "[bitset][quick]") {
    test_bitset_014<7>();
    test_bitset_014<8>();
    test_bitset_014<10>();
    test_bitset_014<16>();
    test_bitset_014<20>();
    test_bitset_014<32>();
#if LIBSEMIGROUPS_SIZEOF_VOID_P == 8
    test_bitset_014<40>();
    test_bitset_014<64>();
#endif
  }

  LIBSEMIGROUPS_TEST_CASE("BitSet", "015", "constructors", "[bitset][quick]") {
    BitSet<10> bs;
    bs.set();
    bs.reset(2, 6);
    REQUIRE(bs.count() == 6);
    REQUIRE(bs[0]);
    REQUIRE(bs[1]);
    REQUIRE(!bs[2]);
    REQUIRE(!bs[3]);
    REQUIRE(!bs[4]);
    REQUIRE(bs[6]);
    REQUIRE(bs[7]);

    {  // Copy constructor
      auto copy(bs);
      REQUIRE(copy == bs);
      REQUIRE(&copy != &bs);
    }
    REQUIRE(bs.count() == 6);
    REQUIRE(bs[0]);
    REQUIRE(bs[1]);
    REQUIRE(!bs[2]);
    REQUIRE(!bs[3]);
    REQUIRE(!bs[4]);
    REQUIRE(bs[6]);
    REQUIRE(bs[7]);
    {  // Move constructor
      auto copy(std::move(bs));
      REQUIRE(copy.count() == 6);
      REQUIRE(copy[0]);
      REQUIRE(copy[1]);
      REQUIRE(!copy[2]);
      REQUIRE(!copy[3]);
      REQUIRE(!copy[4]);
      REQUIRE(copy[6]);
      REQUIRE(copy[7]);
    }
    bs.set();
    bs.reset(2, 6);
    {  // Copy assignment
      auto copy = bs;
      REQUIRE(copy.count() == 6);
      REQUIRE(copy[0]);
      REQUIRE(copy[1]);
      REQUIRE(!copy[2]);
      REQUIRE(!copy[3]);
      REQUIRE(!copy[4]);
      REQUIRE(copy[6]);
      REQUIRE(copy[7]);
    }
    REQUIRE(bs.count() == 6);
    REQUIRE(bs[0]);
    REQUIRE(bs[1]);
    REQUIRE(!bs[2]);
    REQUIRE(!bs[3]);
    REQUIRE(!bs[4]);
    REQUIRE(bs[6]);
    REQUIRE(bs[7]);
    {  // Move assignment
      auto copy = std::move(bs);
      REQUIRE(copy.count() == 6);
      REQUIRE(copy[0]);
      REQUIRE(copy[1]);
      REQUIRE(!copy[2]);
      REQUIRE(!copy[3]);
      REQUIRE(!copy[4]);
      REQUIRE(copy[6]);
      REQUIRE(copy[7]);
    }

    // block_type constructor
    BitSet<30> bs2(0x15);
    REQUIRE(bs2.count() == 3);
    REQUIRE(bs2[0]);
    REQUIRE(!bs2[1]);
    REQUIRE(bs2[2]);
    REQUIRE(!bs2[3]);
    REQUIRE(bs2[4]);
  }

  LIBSEMIGROUPS_TEST_CASE("BitSet", "016", "max_size", "[bitset][quick]") {
#if LIBSEMIGROUPS_SIZEOF_VOID_P == 8
    REQUIRE(BitSet<1>::max_size() == 64);
#else
    REQUIRE(BitSet<1>::max_size() == 32);
#endif
  }

  LIBSEMIGROUPS_TEST_CASE("BitSet",
                          "017",
                          "insertion operators",
                          "[bitset][quick]") {
    BitSet<10> bs;
    bs.set();
    bs.reset(2, 6);
    std::ostringstream oss;
    oss << bs;  // does nothing visible

    std::stringbuf buff;
    std::ostream   os(&buff);
    os << bs;  // does nothing visible
  }
}  // namespace libsemigroups
