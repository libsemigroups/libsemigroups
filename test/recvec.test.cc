//
// Semigroups++ - C/C++ library for computing with semigroups and monoids
// Copyright (C) 2016 James D. Mitchell
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

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "../util/recvec.h"

using namespace libsemigroups;

TEST_CASE("RecVec: default constructor with 3 default args", "[constructor]") {
  RecVec<bool> rv = RecVec<bool>();
  REQUIRE(rv.size() == 0);
  REQUIRE(rv.nr_rows() == 0);
  REQUIRE(rv.nr_cols() == 0);
}

TEST_CASE("RecVec: default constructor with 2 default args", "[constructor]") {
  RecVec<size_t> rv = RecVec<size_t>(5);
  REQUIRE(rv.size() == 0);
  REQUIRE(rv.nr_cols() == 5);
  REQUIRE(rv.nr_rows() == 0);
}

TEST_CASE("RecVec: default constructor with 1 default args", "[constructor]") {
  RecVec<bool> rv = RecVec<bool>(5, 5);
  REQUIRE(rv.size() == 25);
  REQUIRE(rv.nr_cols() == 5);
  REQUIRE(rv.nr_rows() == 5);
  REQUIRE(all_of(rv.begin(), rv.end(), [](bool val) { return val == false; }));
}

TEST_CASE("RecVec: default constructor with 0 default args", "[constructor]") {
  RecVec<bool> rv = RecVec<bool>(2, 7, true);
  REQUIRE(rv.size() == 14);
  REQUIRE(rv.nr_cols() == 2);
  REQUIRE(rv.nr_rows() == 7);
  REQUIRE(all_of(rv.begin(), rv.end(), [](bool val) { return val == true; }));
}

TEST_CASE("RecVec: copy constructor with 1 default args", "[constructor]") {
  RecVec<size_t> rv   = RecVec<size_t>(3, 7, 666);
  RecVec<size_t> copy = RecVec<size_t>(rv);
  REQUIRE(copy.size() == 21);
  REQUIRE(copy.nr_cols() == 3);
  REQUIRE(copy.nr_rows() == 7);
  REQUIRE(
      all_of(copy.begin(), copy.end(), [](size_t val) { return val == 666; }));
}

TEST_CASE("RecVec: copy constructor with 0 default args", "[constructor]") {
  RecVec<size_t> rv   = RecVec<size_t>(3, 7, 666);
  RecVec<size_t> copy = RecVec<size_t>(rv, 2);
  REQUIRE(copy.size() == 35);
  REQUIRE(copy.nr_cols() == 5);
  REQUIRE(copy.nr_rows() == 7);
  REQUIRE(
      all_of(copy.begin(), copy.end(), [](size_t val) { return val == 666; }));
}

TEST_CASE("RecVec: method add_rows with 1 default args", "[method]") {
  RecVec<size_t> rv = RecVec<size_t>(3, 7, 666);
  rv.add_rows();
  REQUIRE(rv.size() == 24);
  REQUIRE(rv.nr_cols() == 3);
  REQUIRE(rv.nr_rows() == 8);
  REQUIRE(all_of(rv.begin(), rv.end(), [](size_t val) { return val == 666; }));
  rv.add_rows(2);
  REQUIRE(rv.size() == 30);
  REQUIRE(rv.nr_cols() == 3);
  REQUIRE(rv.nr_rows() == 10);
  REQUIRE(all_of(rv.begin(), rv.end(), [](size_t val) { return val == 666; }));
  rv.add_rows();
  REQUIRE(rv.size() == 33);
  REQUIRE(rv.nr_cols() == 3);
  REQUIRE(rv.nr_rows() == 11);
  REQUIRE(all_of(rv.begin(), rv.end(), [](size_t val) { return val == 666; }));
}

TEST_CASE("RecVec: method add_rows with 0 default args", "[method]") {
  RecVec<size_t> rv = RecVec<size_t>(3, 7, 666);
  rv.add_rows(10);
  REQUIRE(rv.size() == 51);
  REQUIRE(rv.nr_cols() == 3);
  REQUIRE(rv.nr_rows() == 17);
  REQUIRE(all_of(rv.begin(), rv.end(), [](size_t val) { return val == 666; }));
  rv.add_rows(7);
  REQUIRE(rv.size() == 72);
  REQUIRE(rv.nr_cols() == 3);
  REQUIRE(rv.nr_rows() == 24);
  REQUIRE(all_of(rv.begin(), rv.end(), [](size_t val) { return val == 666; }));
}

TEST_CASE("RecVec: method add_cols", "[method]") {
  RecVec<size_t> rv = RecVec<size_t>(100, 2, 666);
  rv.add_cols(10);
  REQUIRE(rv.size() == 220);
  REQUIRE(rv.nr_cols() == 110);
  REQUIRE(rv.nr_rows() == 2);
  REQUIRE(all_of(rv.begin(), rv.end(), [](size_t val) { return val == 666; }));
  rv.add_cols(5);
  REQUIRE(rv.size() == 230);
  REQUIRE(rv.nr_cols() == 115);
  REQUIRE(rv.nr_rows() == 2);
  REQUIRE(all_of(rv.begin(), rv.end(), [](size_t val) { return val == 666; }));
}

TEST_CASE("RecVec: method set/get", "[method]") {
  RecVec<size_t> rv = RecVec<size_t>(100, 50, 666);
  rv.set(0, 98, 0);
  REQUIRE(rv.get(0, 98) == 0);
  rv.set(1, 45, 1);
  REQUIRE(rv.get(1, 45) == 1);
  rv.set(49, 99, 1);
  REQUIRE(rv.get(1, 45) == 1);
  size_t val = 0;
  for (size_t col = 0; col < 100; col++) {
    for (size_t row = 0; row < 50; row++) {
      rv.set(row, col, val++);
    }
  }
  val           = 0;
  auto check_it = [&val](RecVec<size_t>& rv) {
    for (size_t col = 0; col < 100; col++) {
      for (size_t row = 0; row < 50; row++) {
        if (rv.get(row, col) != val++) {
          return false;
        }
      }
    }
    return true;
  };
  REQUIRE(check_it(rv));
}

TEST_CASE("RecVec: method append 1/2", "[method]") {
  RecVec<size_t> rv1 = RecVec<size_t>(100, 50, 555);
  RecVec<size_t> rv2 = RecVec<size_t>(100, 50, 666);
  REQUIRE(rv1.size() == 5000);
  REQUIRE(rv1.nr_cols() == 100);
  REQUIRE(rv1.nr_rows() == 50);
  REQUIRE(rv2.size() == 5000);
  REQUIRE(rv2.nr_cols() == 100);
  REQUIRE(rv2.nr_rows() == 50);
  rv1.append(rv2);
  REQUIRE(rv1.size() == 10000);
  REQUIRE(rv1.nr_cols() == 100);
  REQUIRE(rv1.nr_rows() == 100);
  REQUIRE(rv2.size() == 5000);
  REQUIRE(rv2.nr_cols() == 100);
  REQUIRE(rv2.nr_rows() == 50);
  REQUIRE(
      all_of(rv2.begin(), rv2.end(), [](size_t val) { return val == 666; }));
  auto check_it = [&rv1](size_t begin, size_t end, size_t val) {
    for (size_t col = 0; col < 100; col++) {
      for (size_t row = begin; row < end; row++) {
        if (rv1.get(row, col) != val) {
          return false;
        }
      }
    }
    return true;
  };
  REQUIRE(check_it(0, 50, 555));
  REQUIRE(check_it(50, 100, 666));
}

TEST_CASE("RecVec: method append 2/2", "[method]") {
  RecVec<size_t> rv1 = RecVec<size_t>(10, 10, 555);
  REQUIRE(rv1.size() == 100);
  REQUIRE(rv1.nr_cols() == 10);
  REQUIRE(rv1.nr_rows() == 10);
  rv1.add_rows(40);
  for (size_t i = 0; i < 9; i++) {
    rv1.add_cols(10);
  }
  REQUIRE(rv1.size() == 5000);
  REQUIRE(rv1.nr_cols() == 100);
  REQUIRE(rv1.nr_rows() == 50);

  RecVec<size_t> rv2 = RecVec<size_t>(3, 4, 666);
  rv2.add_rows(46);
  rv2.add_cols(97);
  REQUIRE(rv1.size() == 5000);
  REQUIRE(rv1.nr_cols() == 100);
  REQUIRE(rv1.nr_rows() == 50);

  rv1.append(rv2);
  REQUIRE(rv1.size() == 10000);
  REQUIRE(rv1.nr_cols() == 100);
  REQUIRE(rv1.nr_rows() == 100);
  REQUIRE(rv2.size() == 5000);
  REQUIRE(rv2.nr_cols() == 100);
  REQUIRE(rv2.nr_rows() == 50);
  REQUIRE(
      all_of(rv2.begin(), rv2.end(), [](size_t val) { return val == 666; }));
  auto check_it = [&rv1](size_t begin, size_t end, size_t val) {
    for (size_t col = 0; col < 100; col++) {
      for (size_t row = begin; row < end; row++) {
        if (rv1.get(row, col) != val) {
          return false;
        }
      }
    }
    return true;
  };
  REQUIRE(check_it(0, 50, 555));
  REQUIRE(check_it(50, 100, 666));
}

TEST_CASE("RecVec: method count", "[method]") {
  RecVec<size_t> rv = RecVec<size_t>(10, 10);
  for (size_t i = 0; i < 9; i++) {
    rv.set(i, i, 1);
  }
  REQUIRE(rv.count(7, 0) == 9);
  REQUIRE(rv.count(7, 1) == 1);
  rv.set(7, 0, 1);
  REQUIRE(rv.count(7, 0) == 8);
  REQUIRE(rv.count(7, 1) == 2);
  REQUIRE(rv.count(7, 2) == 0);
  rv.add_cols(100);
  REQUIRE(rv.count(7, 0) == 108);
  REQUIRE(rv.count(7, 1) == 2);
  REQUIRE(rv.count(7, 2) == 0);
  for (size_t i = 10; i < 19; i++) {
    rv.set(7, i, 2);
  }
  REQUIRE(rv.count(7, 0) == 99);
  REQUIRE(rv.count(7, 1) == 2);
  REQUIRE(rv.count(7, 2) == 9);
}

TEST_CASE("RecVec: method clear", "[method]") {
  RecVec<size_t> rv = RecVec<size_t>(10, 10);
  REQUIRE(rv.size() == 100);
  REQUIRE(rv.nr_cols() == 10);
  REQUIRE(rv.nr_rows() == 10);
  rv.clear();
  REQUIRE(rv.size() == 0);
  REQUIRE(rv.nr_cols() == 0);
  REQUIRE(rv.nr_rows() == 0);
}

TEST_CASE("RecVec: method row_begin and row_end", "[method]") {
  RecVec<size_t> rv = RecVec<size_t>(100, 2);
  for (size_t i = 0; i < rv.nr_rows(); i++) {
    for (auto it = rv.row_begin(i); it < rv.row_end(i); it++) {
      REQUIRE(*it == 0);
      if (i == 0) {
        *it = 666;
        REQUIRE(*it == 666);
      }
    }
  }
  REQUIRE(rv.count(0, 666) == 100);
  REQUIRE(rv.count(1, 666) == 0);
}

TEST_CASE("RecVec: method row_cbegin and row_cend", "[method]") {
  RecVec<size_t> rv = RecVec<size_t>(10, 10, 66);
  for (size_t i = 0; i < rv.nr_rows(); i++) {
    for (auto it = rv.row_cbegin(i); it < rv.row_cend(i); it++) {
      REQUIRE(*it == 66);
    }
  }
}
