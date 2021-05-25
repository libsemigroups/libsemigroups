//
// libsemigroups - C/C++ library for semigroups and monoids
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

// The purpose of this file is to test the UF class which describes a partition
// of the set of integers {0, ..., n - 1 }

#include <cstddef>  // for size_t
#include <numeric>  // for iota
#include <vector>   // vector

#include "catch.hpp"             // for REQUIRE
#include "libsemigroups/uf.hpp"  // Duf + Suf
#include "test-main.hpp"         // for LIBSEMIGROUPS_TEST_CASE

namespace libsemigroups {
  namespace detail {
    LIBSEMIGROUPS_TEST_CASE("UF", "001", "constructor by size", "[quick]") {
      {
        Duf<> uf(7);
        REQUIRE(uf.size() == 7);
        std::vector<size_t> v(uf.size(), 0);
        std::iota(v.begin(), v.end(), 0);
        REQUIRE(v == std::vector<size_t>({0, 1, 2, 3, 4, 5, 6}));
        std::for_each(v.begin(), v.end(), [&uf](size_t& i) { i = uf.find(i); });
        REQUIRE(v == std::vector<size_t>({0, 1, 2, 3, 4, 5, 6}));
      }
      {
        Suf<7> uf;
        REQUIRE(uf.size() == 7);
        std::vector<size_t> v(uf.size(), 0);
        std::iota(v.begin(), v.end(), 0);
        REQUIRE(v == std::vector<size_t>({0, 1, 2, 3, 4, 5, 6}));
        std::for_each(v.begin(), v.end(), [&uf](size_t& i) { i = uf.find(i); });
        REQUIRE(v == std::vector<size_t>({0, 1, 2, 3, 4, 5, 6}));
      }
    }

    LIBSEMIGROUPS_TEST_CASE("UF", "002", "copy constructor", "[quick]") {
      {
        Duf<> uf(11);
        uf.unite(0, 10);
        uf.unite(2, 3);
        uf.unite(6, 3);
        uf.unite(6, 7);

        std::vector<size_t> v(uf.size(), 0);
        std::iota(v.begin(), v.end(), 0);
        REQUIRE(v == std::vector<size_t>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10}));
        std::for_each(v.begin(), v.end(), [&uf](size_t& i) { i = uf.find(i); });
        REQUIRE(v == std::vector<size_t>({10, 1, 3, 3, 4, 5, 3, 3, 8, 9, 10}));
        REQUIRE(std::vector<size_t>(uf.cbegin(), uf.cend())
                == std::vector<size_t>({1, 3, 4, 5, 8, 9, 10}));
        REQUIRE(std::vector<size_t>(uf.crbegin(), uf.crend())
                == std::vector<size_t>({10, 9, 8, 5, 4, 3, 1}));

        REQUIRE(uf.size() == 11);
        REQUIRE(uf.number_of_blocks() == 7);

        Duf<> uf2(uf);
        REQUIRE(uf2.size() == 11);
        REQUIRE(uf2.number_of_blocks() == 7);
      }
      {
        Suf<11> uf;
        uf.unite(0, 10);
        uf.unite(2, 3);
        uf.unite(6, 3);
        uf.unite(6, 7);

        REQUIRE(uf.size() == 11);
        REQUIRE(uf.number_of_blocks() == 7);

        std::vector<size_t> v(uf.size(), 0);
        std::iota(v.begin(), v.end(), 0);
        REQUIRE(v == std::vector<size_t>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10}));
        std::for_each(v.begin(), v.end(), [&uf](size_t& i) { i = uf.find(i); });
        REQUIRE(v == std::vector<size_t>({10, 1, 3, 3, 4, 5, 3, 3, 8, 9, 10}));
        REQUIRE(std::vector<size_t>(uf.cbegin(), uf.cend())
                == std::vector<size_t>({1, 3, 4, 5, 8, 9, 10}));
        REQUIRE(std::vector<size_t>(uf.crbegin(), uf.crend())
                == std::vector<size_t>({10, 9, 8, 5, 4, 3, 1}));

        REQUIRE(uf.size() == 11);
        REQUIRE(uf.number_of_blocks() == 7);

        auto uf2(uf);
        REQUIRE(uf2.size() == 11);
        REQUIRE(uf2.number_of_blocks() == 7);
      }
    }

    LIBSEMIGROUPS_TEST_CASE("UF", "003", "find", "[quick]") {
      {
        Duf<> uf(11);
        uf.unite(0, 10);
        uf.unite(2, 3);
        uf.unite(4, 3);
        uf.unite(4, 5);
        uf.unite(6, 2);
        uf.unite(6, 7);
        REQUIRE(uf.number_of_blocks() == 5);
        REQUIRE(uf.find(0) == 10);
        REQUIRE(uf.find(1) == 1);
        REQUIRE(uf.find(2) == 3);
        REQUIRE(uf.find(3) == 3);
        REQUIRE(uf.find(4) == 3);
        REQUIRE(uf.find(5) == 3);
        REQUIRE(uf.find(6) == 3);
        REQUIRE(uf.find(7) == 3);
        REQUIRE(uf.find(8) == 8);
        REQUIRE(uf.find(9) == 9);
        REQUIRE(uf.find(10) == 10);
      }
      {
        Suf<11> uf;
        uf.unite(0, 10);
        uf.unite(2, 3);
        uf.unite(4, 3);
        uf.unite(4, 5);
        uf.unite(6, 2);
        uf.unite(6, 7);
        REQUIRE(uf.number_of_blocks() == 5);
        REQUIRE(uf.find(0) == 10);
        REQUIRE(uf.find(1) == 1);
        REQUIRE(uf.find(2) == 3);
        REQUIRE(uf.find(3) == 3);
        REQUIRE(uf.find(4) == 3);
        REQUIRE(uf.find(5) == 3);
        REQUIRE(uf.find(6) == 3);
        REQUIRE(uf.find(7) == 3);
        REQUIRE(uf.find(8) == 8);
        REQUIRE(uf.find(9) == 9);
        REQUIRE(uf.find(10) == 10);
      }
    }

    LIBSEMIGROUPS_TEST_CASE("Duf", "004", "unite", "[quick]") {
      Duf<> uf(12);
      uf.unite(0, 1);
      uf.unite(4, 2);
      uf.unite(3, 1);
      uf.unite(4, 10);
      uf.unite(4, 10);
      uf.unite(11, 9);
      uf.unite(8, 9);

      REQUIRE(uf.number_of_blocks() == 6);
      REQUIRE(std::vector<size_t>(uf.cbegin(), uf.cend())
              == std::vector<size_t>({1, 2, 5, 6, 7, 9}));
      REQUIRE(std::all_of(
          uf.cbegin(), uf.cend(), [&uf](size_t i) { return uf.find(i) == i; }));

      REQUIRE(uf.find(0) == 1);
      REQUIRE(uf.find(8) == 9);
      REQUIRE(uf.find(11) == 9);

      REQUIRE(uf.number_of_blocks() == 6);
      REQUIRE(std::vector<size_t>(uf.cbegin(), uf.cend())
              == std::vector<size_t>({1, 2, 5, 6, 7, 9}));
      REQUIRE(std::all_of(
          uf.cbegin(), uf.cend(), [&uf](size_t i) { return uf.find(i) == i; }));

      uf.unite(8, 8);
      REQUIRE(uf.find(0) == 1);
      REQUIRE(uf.find(8) == 9);
      REQUIRE(uf.find(11) == 9);
      REQUIRE(uf.number_of_blocks() == 6);
      REQUIRE(std::vector<size_t>(uf.cbegin(), uf.cend())
              == std::vector<size_t>({1, 2, 5, 6, 7, 9}));
      REQUIRE(std::all_of(
          uf.cbegin(), uf.cend(), [&uf](size_t i) { return uf.find(i) == i; }));

      uf.unite(11, 8);
      REQUIRE(uf.find(0) == 1);
      REQUIRE(uf.find(8) == 9);
      REQUIRE(uf.find(11) == 9);
      REQUIRE(uf.number_of_blocks() == 6);
      REQUIRE(std::vector<size_t>(uf.cbegin(), uf.cend())
              == std::vector<size_t>({1, 2, 5, 6, 7, 9}));
      REQUIRE(std::all_of(
          uf.cbegin(), uf.cend(), [&uf](size_t i) { return uf.find(i) == i; }));

      uf.unite(11, 0);
      REQUIRE(uf.find(0) == 1);
      REQUIRE(uf.find(8) == 1);
      REQUIRE(uf.find(11) == 1);
      REQUIRE(uf.number_of_blocks() == 5);
      REQUIRE(std::vector<size_t>(uf.cbegin(), uf.cend())
              == std::vector<size_t>({1, 2, 5, 6, 7}));
      REQUIRE(std::all_of(
          uf.cbegin(), uf.cend(), [&uf](size_t i) { return uf.find(i) == i; }));
    }

    LIBSEMIGROUPS_TEST_CASE("Suf", "005", "unite", "[quick]") {
      Suf<12> uf;
      uf.unite(0, 1);
      uf.unite(4, 2);
      uf.unite(3, 1);
      uf.unite(4, 10);
      uf.unite(4, 10);
      uf.unite(11, 9);
      uf.unite(8, 9);

      REQUIRE(uf.number_of_blocks() == 6);
      REQUIRE(std::vector<size_t>(uf.cbegin(), uf.cend())
              == std::vector<size_t>({1, 2, 5, 6, 7, 9}));
      REQUIRE(std::all_of(
          uf.cbegin(), uf.cend(), [&uf](size_t i) { return uf.find(i) == i; }));

      REQUIRE(uf.find(0) == 1);
      REQUIRE(uf.find(8) == 9);
      REQUIRE(uf.find(11) == 9);

      REQUIRE(uf.number_of_blocks() == 6);
      REQUIRE(std::vector<size_t>(uf.cbegin(), uf.cend())
              == std::vector<size_t>({1, 2, 5, 6, 7, 9}));
      REQUIRE(std::all_of(
          uf.cbegin(), uf.cend(), [&uf](size_t i) { return uf.find(i) == i; }));

      uf.unite(8, 8);
      REQUIRE(uf.find(0) == 1);
      REQUIRE(uf.find(8) == 9);
      REQUIRE(uf.find(11) == 9);
      REQUIRE(uf.number_of_blocks() == 6);
      REQUIRE(std::vector<size_t>(uf.cbegin(), uf.cend())
              == std::vector<size_t>({1, 2, 5, 6, 7, 9}));
      REQUIRE(std::all_of(
          uf.cbegin(), uf.cend(), [&uf](size_t i) { return uf.find(i) == i; }));

      uf.unite(11, 8);
      REQUIRE(uf.find(0) == 1);
      REQUIRE(uf.find(8) == 9);
      REQUIRE(uf.find(11) == 9);
      REQUIRE(uf.number_of_blocks() == 6);
      REQUIRE(std::vector<size_t>(uf.cbegin(), uf.cend())
              == std::vector<size_t>({1, 2, 5, 6, 7, 9}));
      REQUIRE(std::all_of(
          uf.cbegin(), uf.cend(), [&uf](size_t i) { return uf.find(i) == i; }));

      uf.unite(11, 0);
      REQUIRE(uf.find(0) == 1);
      REQUIRE(uf.find(8) == 1);
      REQUIRE(uf.find(11) == 1);
      REQUIRE(uf.number_of_blocks() == 5);
      REQUIRE(std::vector<size_t>(uf.cbegin(), uf.cend())
              == std::vector<size_t>({1, 2, 5, 6, 7}));
      REQUIRE(std::all_of(
          uf.cbegin(), uf.cend(), [&uf](size_t i) { return uf.find(i) == i; }));
    }

    LIBSEMIGROUPS_TEST_CASE("Duf", "006", "compress", "[quick]") {
      {
        Duf<> uf(12);
        uf.unite(0, 1);
        uf.unite(4, 2);
        uf.unite(3, 1);
        uf.unite(4, 10);
        uf.unite(4, 10);
        uf.unite(11, 9);
        uf.unite(8, 9);

        REQUIRE(std::vector<size_t>(uf.cbegin_data(), uf.cend_data())
                == std::vector<size_t>({1, 1, 2, 1, 2, 5, 6, 7, 9, 9, 2, 9}));
        uf.compress();
        REQUIRE(std::vector<size_t>(uf.cbegin_data(), uf.cend_data())
                == std::vector<size_t>({1, 1, 2, 1, 2, 5, 6, 7, 9, 9, 2, 9}));
        uf.normalize();
        REQUIRE(std::vector<size_t>(uf.cbegin_data(), uf.cend_data())
                == std::vector<size_t>({0, 0, 2, 0, 2, 5, 6, 7, 8, 8, 2, 8}));
      }
      {
        Duf<> uf({0, 1, 2, 2, 3, 4, 2, 2, 6, 5, 0});
        REQUIRE(std::vector<size_t>(uf.cbegin_data(), uf.cend_data())
                == std::vector<size_t>({0, 1, 2, 2, 3, 4, 2, 2, 6, 5, 0}));
        uf.compress();
        REQUIRE(std::vector<size_t>(uf.cbegin_data(), uf.cend_data())
                == std::vector<size_t>({0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 0}));
      }
    }

    LIBSEMIGROUPS_TEST_CASE("Suf", "007", "compress", "[quick]") {
      {
        Suf<12> uf;
        uf.unite(0, 1);
        uf.unite(4, 2);
        uf.unite(3, 1);
        uf.unite(4, 10);
        uf.unite(4, 10);
        uf.unite(11, 9);
        uf.unite(8, 9);

        REQUIRE(std::vector<size_t>(uf.cbegin_data(), uf.cend_data())
                == std::vector<size_t>({1, 1, 2, 1, 2, 5, 6, 7, 9, 9, 2, 9}));
        uf.compress();
        REQUIRE(std::vector<size_t>(uf.cbegin_data(), uf.cend_data())
                == std::vector<size_t>({1, 1, 2, 1, 2, 5, 6, 7, 9, 9, 2, 9}));
      }
      {
        Suf<11> uf({0, 1, 2, 2, 3, 4, 2, 2, 6, 5, 0});
        REQUIRE(std::vector<size_t>(uf.cbegin_data(), uf.cend_data())
                == std::vector<size_t>({0, 1, 2, 2, 3, 4, 2, 2, 6, 5, 0}));
        uf.compress();
        REQUIRE(std::vector<size_t>(uf.cbegin_data(), uf.cend_data())
                == std::vector<size_t>({0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 0}));
      }
    }

    LIBSEMIGROUPS_TEST_CASE("Duf", "008", "resize", "[quick]") {
      Duf<> uf(0);
      for (size_t i = 0; i < 10; ++i) {
        uf.resize(i);
      }
      REQUIRE(std::vector<size_t>(uf.cbegin_data(), uf.cend_data())
              == std::vector<size_t>({0, 1, 2, 3, 4, 5, 6, 7, 8}));
      REQUIRE(std::vector<size_t>(uf.cbegin_rank(), uf.cend_rank())
              == std::vector<size_t>({0, 0, 0, 0, 0, 0, 0, 0, 0}));
      uf.compress();
      REQUIRE(std::vector<size_t>(uf.cbegin_data(), uf.cend_data())
              == std::vector<size_t>({0, 1, 2, 3, 4, 5, 6, 7, 8}));
      REQUIRE(std::vector<size_t>(uf.cbegin_rank(), uf.cend_rank())
              == std::vector<size_t>({0, 0, 0, 0, 0, 0, 0, 0, 0}));
      uf.normalize();
      REQUIRE(std::vector<size_t>(uf.cbegin_data(), uf.cend_data())
              == std::vector<size_t>({0, 1, 2, 3, 4, 5, 6, 7, 8}));
      REQUIRE(std::vector<size_t>(uf.cbegin_rank(), uf.cend_rank())
              == std::vector<size_t>({0, 0, 0, 0, 0, 0, 0, 0, 0}));
      uf.unite(0, 8);
      uf.unite(0, 0);
      uf.unite(1, 0);
      REQUIRE(std::vector<size_t>(uf.cbegin_data(), uf.cend_data())
              == std::vector<size_t>({8, 8, 2, 3, 4, 5, 6, 7, 8}));
      REQUIRE(std::vector<size_t>(uf.cbegin_rank(), uf.cend_rank())
              == std::vector<size_t>({0, 0, 0, 0, 0, 0, 0, 0, 1}));
      uf.resize(25);
      REQUIRE(std::vector<size_t>(uf.cbegin_data(), uf.cend_data())
              == std::vector<size_t>({8,  8,  2,  3,  4,  5,  6,  7,  8,
                                      9,  10, 11, 12, 13, 14, 15, 16, 17,
                                      18, 19, 20, 21, 22, 23, 24}));
      REQUIRE(std::vector<size_t>(uf.cbegin_rank(), uf.cend_rank())
              == std::vector<size_t>({0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
                                      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
      REQUIRE(uf.number_of_blocks() == 23);
    }

    LIBSEMIGROUPS_TEST_CASE("Duf", "009", "resize", "[quick]") {
      Duf<> uf({0, 0, 2, 3, 3, 5});
      REQUIRE(uf.size() == 6);
      uf.resize(7);
      REQUIRE(uf.size() == 7);
      uf.resize(8);
      REQUIRE(uf.size() == 8);
      REQUIRE(uf.find(6) == 6);
      REQUIRE(uf.find(7) == 7);
      uf.unite(1, 7);
      REQUIRE(uf.find(7) == 7);
      REQUIRE(uf.number_of_blocks() == 5);
      REQUIRE(std::vector<size_t>(uf.cbegin(), uf.cend())
              == std::vector<size_t>({2, 3, 5, 6, 7}));
    }

    LIBSEMIGROUPS_TEST_CASE("Duf", "010", "big chain", "[no-valgrind][quick]") {
      std::vector<size_t> tab;
      tab.push_back(0);
      for (size_t i = 0; i < 100000; i++) {
        tab.push_back(i);
      }
      Duf<> uf(tab);
      REQUIRE(uf.number_of_blocks() == 1);
      REQUIRE(uf.size() == 100001);
      REQUIRE(uf.find(12345) == 0);
      REQUIRE(uf.find(100000) == 0);
      uf.compress();
      uf.normalize();
      for (size_t i = 0; i < 100001; i++) {
        REQUIRE(uf.find(i) == 0);
      }
    }

    LIBSEMIGROUPS_TEST_CASE("Suf", "011", "big chain", "[no-valgrind][quick]") {
      std::array<uint32_t, 100001> tab;
      std::iota(tab.begin() + 1, tab.end(), 0);
      Suf<100001> uf(std::move(tab));
      REQUIRE(uf.number_of_blocks() == 1);
      REQUIRE(uf.size() == 100001);
      REQUIRE(uf.find(12345) == 0);
      REQUIRE(uf.find(100000) == 0);
      uf.compress();
      uf.normalize();
      for (size_t i = 0; i < 100001; i++) {
        REQUIRE(uf.find(i) == 0);
      }
    }

    LIBSEMIGROUPS_TEST_CASE("Duf", "012", "empty table", "[quick]") {
      Duf<> uf(0);
      REQUIRE(uf.number_of_blocks() == 0);
      uf.resize(1);
      REQUIRE(uf.size() == 1);
      REQUIRE(uf.number_of_blocks() == 1);
    }

    LIBSEMIGROUPS_TEST_CASE("Suf", "013", "empty table", "[quick]") {
      Suf<0> uf;
      REQUIRE(uf.number_of_blocks() == 0);
    }

    LIBSEMIGROUPS_TEST_CASE("Duf", "014", "join", "[quick]") {
      Duf<> uf1(10);
      uf1.unite(2, 4);
      uf1.unite(4, 9);
      uf1.unite(1, 7);

      REQUIRE(uf1.number_of_blocks() == 7);

      uf1.join(uf1);
      REQUIRE(uf1.number_of_blocks() == 7);

      Duf<> uf2(10);
      uf2.unite(1, 4);
      uf2.unite(3, 9);
      uf2.unite(0, 7);
      REQUIRE(uf2.number_of_blocks() == 7);

      uf1.join(uf2);
      REQUIRE(uf2.number_of_blocks() == 7);
      REQUIRE(uf1.number_of_blocks() == 4);

      REQUIRE(std::vector<size_t>(uf1.cbegin(), uf1.cend())
              == std::vector<size_t>({4, 5, 6, 8}));
    }

    LIBSEMIGROUPS_TEST_CASE("Suf", "015", "join", "[quick]") {
      Suf<10> uf1;
      uf1.unite(2, 4);
      uf1.unite(4, 9);
      uf1.unite(1, 7);

      REQUIRE(uf1.number_of_blocks() == 7);

      uf1.join(uf1);
      REQUIRE(uf1.number_of_blocks() == 7);

      Suf<10> uf2;
      uf2.unite(1, 4);
      uf2.unite(3, 9);
      uf2.unite(0, 7);
      REQUIRE(uf2.number_of_blocks() == 7);

      uf1.join(uf2);
      REQUIRE(uf2.number_of_blocks() == 7);
      REQUIRE(uf1.number_of_blocks() == 4);

      REQUIRE(std::vector<size_t>(uf1.cbegin(), uf1.cend())
              == std::vector<size_t>({4, 5, 6, 8}));
    }

    LIBSEMIGROUPS_TEST_CASE("Duf", "016", "contains", "[quick]") {
      Duf<> uf1(10);
      uf1.unite(2, 4);
      uf1.unite(4, 9);
      uf1.unite(1, 7);

      Duf<> uf2(10);
      REQUIRE(uf1.contains(uf2));
      REQUIRE(!uf2.contains(uf1));

      uf2.unite(9, 2);
      REQUIRE(uf1.contains(uf2));
      REQUIRE(!uf2.contains(uf1));

      uf2.unite(1, 7);
      REQUIRE(uf1.contains(uf2));
      REQUIRE(!uf2.contains(uf1));

      uf2.unite(4, 9);
      REQUIRE(uf1.contains(uf2));
      REQUIRE(uf2.contains(uf1));
      REQUIRE(uf1 == uf2);

      uf2.unite(1, 9);
      REQUIRE(uf2.contains(uf1));
      REQUIRE(!uf1.contains(uf2));

      uf1.unite(0, 3);
      uf2.unite(0, 1);
      REQUIRE(uf1.find(0) == uf1.find(3));
      REQUIRE(uf2.find(0) != uf2.find(3));

      REQUIRE(uf2.find(0) == uf2.find(1));
      REQUIRE(uf1.find(0) != uf1.find(1));

      REQUIRE(!uf2.contains(uf1));
      REQUIRE(!uf1.contains(uf2));
      REQUIRE(uf1 != uf2);
    }

    LIBSEMIGROUPS_TEST_CASE("Suf", "017", "contains", "[quick]") {
      Suf<10> uf1;
      uf1.unite(2, 4);
      uf1.unite(4, 9);
      uf1.unite(1, 7);

      Suf<10> uf2;
      REQUIRE(uf1.contains(uf2));
      REQUIRE(!uf2.contains(uf1));

      uf2.unite(9, 2);
      REQUIRE(uf1.contains(uf2));
      REQUIRE(!uf2.contains(uf1));

      uf2.unite(1, 7);
      REQUIRE(uf1.contains(uf2));
      REQUIRE(!uf2.contains(uf1));

      uf2.unite(4, 9);
      REQUIRE(uf1.contains(uf2));
      REQUIRE(uf2.contains(uf1));
      REQUIRE(uf1 == uf2);

      uf2.unite(1, 9);
      REQUIRE(uf2.contains(uf1));
      REQUIRE(!uf1.contains(uf2));

      uf1.unite(0, 3);
      uf2.unite(0, 1);
      REQUIRE(uf1.find(0) == uf1.find(3));
      REQUIRE(uf2.find(0) != uf2.find(3));

      REQUIRE(uf2.find(0) == uf2.find(1));
      REQUIRE(uf1.find(0) != uf1.find(1));

      REQUIRE(!uf2.contains(uf1));
      REQUIRE(!uf1.contains(uf2));
      REQUIRE(uf1 != uf2);
    }

    LIBSEMIGROUPS_TEST_CASE("Duf", "018", "swap", "[quick]") {
      Duf<> uf1(10);
      uf1.unite(2, 4);
      uf1.unite(4, 9);
      uf1.unite(1, 7);

      Duf<> uf2(10);
      REQUIRE(uf1.contains(uf2));
      REQUIRE(!uf2.contains(uf1));

      Duf<> uf3(uf1);
      Duf<> uf4(uf2);

      std::swap(uf1, uf2);
      REQUIRE(uf1 == uf4);
      REQUIRE(uf2 == uf3);

      uf1.swap(uf2);
      REQUIRE(uf1 == uf3);
      REQUIRE(uf2 == uf4);

      swap(uf1, uf2);
      REQUIRE(uf1 == uf4);
      REQUIRE(uf2 == uf3);

      REQUIRE(uf2 != uf1);
      // operator=
      uf1 = uf3;
      REQUIRE(uf2 == uf1);
    }

    LIBSEMIGROUPS_TEST_CASE("Suf", "019", "swap", "[quick]") {
      Suf<10> uf1;
      uf1.unite(2, 4);
      uf1.unite(4, 9);
      uf1.unite(1, 7);

      Suf<10> uf2;
      REQUIRE(uf1.contains(uf2));
      REQUIRE(!uf2.contains(uf1));

      Suf<10> uf3(uf1);
      Suf<10> uf4(uf2);

      std::swap(uf1, uf2);
      REQUIRE(uf1 == uf4);
      REQUIRE(uf2 == uf3);

      uf1.swap(uf2);
      REQUIRE(uf1 == uf3);
      REQUIRE(uf2 == uf4);

      swap(uf1, uf2);
      REQUIRE(uf1 == uf4);
      REQUIRE(uf2 == uf3);

      REQUIRE(uf2 != uf1);
      // operator=
      uf1 = uf3;
      REQUIRE(uf2 == uf1);
    }
  }  // namespace detail
}  // namespace libsemigroups
