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

#include "catch_amalgamated.hpp"     // for REQUIRE
#include "libsemigroups/bitset.hpp"  // for BitSet
#include "test-main.hpp"             // for LIBSEMIGROUPS_TEMPLATE_TEST_CASE

namespace libsemigroups {

#define BITSET_32_TYPES \
  BitSet<7>, BitSet<8>, BitSet<10>, BitSet<16>, BitSet<20>, BitSet<32>

#if LIBSEMIGROUPS_SIZEOF_VOID_P == 8
#define BITSET_64_TYPES , BitSet<40>, BitSet<64>
#else
#define BITSET_64_TYPES
#endif

#define BITSET_TYPES BITSET_32_TYPES BITSET_64_TYPES

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("BitSet",
                                   "000",
                                   "size",
                                   "[bitset][quick]",
                                   BITSET_TYPES) {
    TestType bs;
    REQUIRE(bs.size() >= 7);
    REQUIRE(bs.size() <= 64);
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("BitSet",
                                   "001",
                                   "operator<",
                                   "[bitset][quick]",
                                   BITSET_TYPES) {
    TestType bs1;
    bs1.reset();
    TestType bs2;
    bs2.reset();
    bs2.set(0);

    REQUIRE(bs1 < bs2);
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("BitSet",
                                   "002",
                                   "operator==",
                                   "[bitset][quick]",
                                   BITSET_TYPES) {
    TestType bs1;
    bs1.reset();
    TestType bs2;
    bs2.reset();
    bs2.set(0);

    REQUIRE(!(bs1 == bs2));
    bs1.set(0);
    REQUIRE(bs1 == bs2);
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("BitSet",
                                   "003",
                                   "operator!=",
                                   "[bitset][quick]",
                                   BITSET_TYPES) {
    TestType bs1;
    bs1.reset();
    TestType bs2;
    bs2.reset();
    bs2.set(0);

    REQUIRE(bs1 != bs2);
    bs1.set(0);
    REQUIRE(bs1 == bs2);
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("BitSet",
                                   "004",
                                   "operator&=",
                                   "[bitset][quick]",
                                   BITSET_TYPES) {
    TestType bs1;
    bs1.reset();
    bs1.set(0);
    bs1.set(1);
    TestType bs2;
    bs2.reset();
    bs2.set(1);
    bs1 &= bs2;
    REQUIRE(bs1 == bs2);
    REQUIRE(bs1.count() == 1);
    REQUIRE(bs2.count() == 1);
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("BitSet",
                                   "005",
                                   "&",
                                   "[bitset][quick]",
                                   BITSET_TYPES) {
    TestType bs1;
    bs1.reset();
    bs1.set(0);
    bs1.set(1);
    TestType bs2;
    bs2.reset();
    bs2.set(1);
    TestType bs3 = bs1 & bs2;
    REQUIRE(bs3 == bs2);
    REQUIRE(&bs3 != &bs2);
    REQUIRE(bs1.count() == 2);
    REQUIRE(bs2.count() == 1);
    REQUIRE(bs3.count() == 1);
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("BitSet",
                                   "006",
                                   "operator|=",
                                   "[bitset][quick]",
                                   BITSET_TYPES) {
    TestType bs1;
    bs1.reset();
    bs1.set(0);
    TestType bs2;
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

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("BitSet",
                                   "007",
                                   "operator[]",
                                   "[bitset][quick]",
                                   BITSET_TYPES) {
    TestType bs;
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

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("BitSet",
                                   "008",
                                   "set(none)",
                                   "[bitset][quick]",
                                   BITSET_TYPES) {
    TestType bs;
    bs.set();
    REQUIRE(bs[0]);
    REQUIRE(bs[1]);
    REQUIRE(bs[2]);
    REQUIRE(bs[3]);
    REQUIRE(bs[4]);
    REQUIRE(bs[5]);
    REQUIRE(bs.count() == bs.size());
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("BitSet",
                                   "009",
                                   "set(pos, value)",
                                   "[bitset][quick]",
                                   BITSET_TYPES) {
    TestType bs;
    bs.set();
    bs.set(0, false);
    REQUIRE(!bs[0]);
    REQUIRE(bs[1]);
    REQUIRE(bs[2]);
    REQUIRE(bs[3]);
    REQUIRE(bs[4]);
    REQUIRE(bs[5]);
    REQUIRE(bs.count() == bs.size() - 1);
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("BitSet",
                                   "010",
                                   "set(first, last, value)",
                                   "[bitset][quick]",
                                   BITSET_TYPES) {
    TestType bs;
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

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("BitSet",
                                   "011",
                                   "reset(first, last)",
                                   "[bitset][quick]",
                                   BITSET_TYPES) {
    TestType bs;
    bs.set();
    REQUIRE(bs.count() == bs.size());
    bs.reset(2, 6);

    REQUIRE(bs.count() == bs.size() - 4);
    REQUIRE(bs[0]);
    REQUIRE(bs[1]);
    REQUIRE(!bs[2]);
    REQUIRE(!bs[3]);
    REQUIRE(!bs[4]);
    REQUIRE(bs[6]);
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("BitSet",
                                   "012",
                                   "reset(pos)",
                                   "[bitset][quick]",
                                   BITSET_TYPES) {
    TestType bs;
    bs.set();
    REQUIRE(bs.count() == bs.size());
    bs.reset(2);
    bs.reset(3);
    bs.reset(4);
    bs.reset(5);

    REQUIRE(bs.count() == bs.size() - 4);
    REQUIRE(bs[0]);
    REQUIRE(bs[1]);
    REQUIRE(!bs[2]);
    REQUIRE(!bs[3]);
    REQUIRE(!bs[4]);
    REQUIRE(bs[6]);
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("BitSet",
                                   "013",
                                   "apply (iterate through set bits)",
                                   "[bitset][quick]",
                                   BITSET_TYPES) {
    TestType bs;
    bs.set();
    bs.reset(2);
    bs.reset(3);
    bs.reset(4);
    bs.reset(5);

    std::vector<size_t> expected = {0, 1};
    for (typename TestType::block_type i = 6; i < bs.size(); i++) {
      expected.push_back(i);
    }
    std::vector<size_t> result;

    // hi bits are not nec. set to false
    bs.apply([&result](size_t i) { result.push_back(i); });

    REQUIRE(bs.count() == bs.size() - 4);
    // hi bits are set to false
    result.clear();
    bs.apply([&result](size_t i) { result.push_back(i); });
    REQUIRE(result == expected);
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("BitSet",
                                   "014",
                                   "std::hash",
                                   "[bitset][quick]",
                                   BITSET_TYPES) {
    TestType bs;
    bs.reset();
    std::hash<TestType>()(bs);
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("BitSet",
                                   "015",
                                   "constructors",
                                   "[bitset][quick]",
                                   BITSET_TYPES) {
    TestType bs;
    bs.set();
    bs.reset(2, 6);
    REQUIRE(bs.count() == bs.size() - 4);
    REQUIRE(bs[0]);
    REQUIRE(bs[1]);
    REQUIRE(!bs[2]);
    REQUIRE(!bs[3]);
    REQUIRE(!bs[4]);
    REQUIRE(bs[6]);

    {  // Copy constructor
      auto copy(bs);
      REQUIRE(copy == bs);
      REQUIRE(&copy != &bs);
    }
    REQUIRE(bs.count() == bs.size() - 4);
    REQUIRE(bs[0]);
    REQUIRE(bs[1]);
    REQUIRE(!bs[2]);
    REQUIRE(!bs[3]);
    REQUIRE(!bs[4]);
    REQUIRE(bs[6]);
    {  // Move constructor
      auto copy(std::move(bs));
      REQUIRE(copy.count() == copy.size() - 4);
      REQUIRE(copy[0]);
      REQUIRE(copy[1]);
      REQUIRE(!copy[2]);
      REQUIRE(!copy[3]);
      REQUIRE(!copy[4]);
      REQUIRE(copy[6]);
    }
    bs.set();
    bs.reset(2, 6);
    {  // Copy assignment
      auto copy = bs;
      REQUIRE(copy.count() == copy.size() - 4);
      REQUIRE(copy[0]);
      REQUIRE(copy[1]);
      REQUIRE(!copy[2]);
      REQUIRE(!copy[3]);
      REQUIRE(!copy[4]);
      REQUIRE(copy[6]);
    }
    REQUIRE(bs.count() == bs.size() - 4);
    REQUIRE(bs[0]);
    REQUIRE(bs[1]);
    REQUIRE(!bs[2]);
    REQUIRE(!bs[3]);
    REQUIRE(!bs[4]);
    REQUIRE(bs[6]);
    {  // Move assignment
      auto copy = std::move(bs);
      REQUIRE(copy.count() == copy.size() - 4);
      REQUIRE(copy[0]);
      REQUIRE(copy[1]);
      REQUIRE(!copy[2]);
      REQUIRE(!copy[3]);
      REQUIRE(!copy[4]);
      REQUIRE(copy[6]);
    }

    // block_type constructor
    // TODO(0) separate test case
    BitSet<30> bs2(0x15);
    REQUIRE(bs2.count() == 3);
    REQUIRE(bs2[0]);
    REQUIRE(!bs2[1]);
    REQUIRE(bs2[2]);
    REQUIRE(!bs2[3]);
    REQUIRE(bs2[4]);
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("BitSet",
                                   "016",
                                   "max_size",
                                   "[bitset][quick]",
                                   BITSET_TYPES) {
#if LIBSEMIGROUPS_SIZEOF_VOID_P == 8
    REQUIRE(BitSet<1>::max_size() == 64);
#else
    REQUIRE(BitSet<1>::max_size() == 32);
#endif
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("BitSet",
                                   "017",
                                   "insertion operators",
                                   "[bitset][quick]",
                                   BITSET_TYPES) {
    TestType bs;
    bs.set();
    bs.reset(2, 6);
    std::ostringstream oss;
    oss << bs;  // does nothing visible

    std::stringbuf buff;
    std::ostream   os(&buff);
    os << bs;  // does nothing visible
  }
}  // namespace libsemigroups
