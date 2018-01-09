//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2017 James D. Mitchell
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

#include "catch.hpp"

#include "../src/recvec.h"

using namespace libsemigroups;

TEST_CASE("RecVec 01: default constructor with 3 default args",
          "[quick][recvec][01]") {
  RecVec<bool> rv = RecVec<bool>();
  REQUIRE(rv.size() == 0);
  REQUIRE(rv.nr_rows() == 0);
  REQUIRE(rv.nr_cols() == 0);
}

TEST_CASE("RecVec 02: default constructor with 2 default args",
          "[quick][recvec][02]") {
  RecVec<size_t> rv = RecVec<size_t>(5);
  REQUIRE(rv.size() == 0);
  REQUIRE(rv.nr_cols() == 5);
  REQUIRE(rv.nr_rows() == 0);
}

TEST_CASE("RecVec 03: default constructor with 1 default args",
          "[quick][recvec][03]") {
  RecVec<bool> rv = RecVec<bool>(5, 5);
  REQUIRE(rv.size() == 25);
  REQUIRE(rv.nr_cols() == 5);
  REQUIRE(rv.nr_rows() == 5);
  REQUIRE(
      std::all_of(rv.begin(), rv.end(), [](bool val) { return val == false; }));
}

TEST_CASE("RecVec 04: default constructor with 0 default args",
          "[quick][recvec][04]") {
  RecVec<bool> rv = RecVec<bool>(2, 7, true);
  REQUIRE(rv.size() == 14);
  REQUIRE(rv.nr_cols() == 2);
  REQUIRE(rv.nr_rows() == 7);
  REQUIRE(
      std::all_of(rv.begin(), rv.end(), [](bool val) { return val == true; }));
}

TEST_CASE("RecVec 05: copy constructor with 1 default args",
          "[quick][recvec][05]") {
  RecVec<size_t> rv   = RecVec<size_t>(3, 7, 666);
  RecVec<size_t> copy = RecVec<size_t>(rv);
  REQUIRE(copy.size() == 21);
  REQUIRE(copy.nr_cols() == 3);
  REQUIRE(copy.nr_rows() == 7);
  REQUIRE(std::all_of(
      copy.begin(), copy.end(), [](size_t val) { return val == 666; }));
}

TEST_CASE("RecVec 06: copy constructor with 0 default args",
          "[quick][recvec][06]") {
  RecVec<size_t> rv   = RecVec<size_t>(3, 7, 666);
  RecVec<size_t> copy = RecVec<size_t>(rv, 2);
  REQUIRE(copy.size() == 35);
  REQUIRE(copy.nr_cols() == 5);
  REQUIRE(copy.nr_rows() == 7);
  REQUIRE(std::all_of(
      copy.begin(), copy.end(), [](size_t val) { return val == 666; }));

  // Check when there are available extra columns already in rv.
  rv.add_cols(10);
  RecVec<size_t> copy2(rv, 0);
  REQUIRE(copy2.size() == 91);
  REQUIRE(copy2.nr_cols() == 13);
  REQUIRE(copy2.nr_rows() == 7);
  REQUIRE(std::all_of(
      copy2.begin(), copy2.end(), [](size_t val) { return val == 666; }));
}

TEST_CASE("RecVec 07: add_rows", "[quick][recvec][07]") {
  RecVec<size_t> rv = RecVec<size_t>(3, 7, 666);
  rv.add_rows(1);
  REQUIRE(rv.size() == 24);
  REQUIRE(rv.nr_cols() == 3);
  REQUIRE(rv.nr_rows() == 8);
  REQUIRE(
      std::all_of(rv.begin(), rv.end(), [](size_t val) { return val == 666; }));
  rv.add_rows(2);
  REQUIRE(rv.size() == 30);
  REQUIRE(rv.nr_cols() == 3);
  REQUIRE(rv.nr_rows() == 10);
  REQUIRE(
      std::all_of(rv.begin(), rv.end(), [](size_t val) { return val == 666; }));
  rv.add_rows(1);
  REQUIRE(rv.size() == 33);
  REQUIRE(rv.nr_cols() == 3);
  REQUIRE(rv.nr_rows() == 11);
  REQUIRE(
      std::all_of(rv.begin(), rv.end(), [](size_t val) { return val == 666; }));
}

TEST_CASE("RecVec 08: add_rows", "[quick][recvec][08]") {
  RecVec<size_t> rv = RecVec<size_t>(3, 7, 666);
  rv.add_rows(10);
  REQUIRE(rv.size() == 51);
  REQUIRE(rv.nr_cols() == 3);
  REQUIRE(rv.nr_rows() == 17);
  REQUIRE(
      std::all_of(rv.begin(), rv.end(), [](size_t val) { return val == 666; }));
  rv.add_rows(7);
  REQUIRE(rv.size() == 72);
  REQUIRE(rv.nr_cols() == 3);
  REQUIRE(rv.nr_rows() == 24);
  REQUIRE(
      std::all_of(rv.begin(), rv.end(), [](size_t val) { return val == 666; }));
}

TEST_CASE("RecVec 09: add_cols", "[quick][recvec][09]") {
  RecVec<size_t> rv = RecVec<size_t>(100, 2, 666);
  rv.add_cols(10);
  REQUIRE(rv.size() == 220);
  REQUIRE(rv.nr_cols() == 110);
  REQUIRE(rv.nr_rows() == 2);
  REQUIRE(
      std::all_of(rv.begin(), rv.end(), [](size_t val) { return val == 666; }));
  rv.add_cols(5);
  REQUIRE(rv.size() == 230);
  REQUIRE(rv.nr_cols() == 115);
  REQUIRE(rv.nr_rows() == 2);
  REQUIRE(
      std::all_of(rv.begin(), rv.end(), [](size_t val) { return val == 666; }));
}

TEST_CASE("RecVec 10: set/get", "[quick][recvec][10]") {
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
  auto check_it = [&val, &rv]() {
    for (size_t col = 0; col < 100; col++) {
      for (size_t row = 0; row < 50; row++) {
        if (rv.get(row, col) != val++) {
          return false;
        }
      }
    }
    return true;
  };
  REQUIRE(check_it());
}

TEST_CASE("RecVec 11: append 1/2", "[quick][recvec][11]") {
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
  REQUIRE(std::all_of(
      rv2.begin(), rv2.end(), [](size_t val) { return val == 666; }));
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

TEST_CASE("RecVec 12: append 2/2", "[quick][recvec][12]") {
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
  REQUIRE(std::all_of(
      rv2.begin(), rv2.end(), [](size_t val) { return val == 666; }));
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

TEST_CASE("RecVec 13: count", "[quick][recvec][13]") {
  RecVec<size_t> rv = RecVec<size_t>(10, 10);
  for (size_t i = 0; i < 9; i++) {
    rv.set(i, i, 1);
  }
  REQUIRE(std::count(rv.begin_row(7), rv.end_row(7), 0) == 9);
  REQUIRE(std::count(rv.begin_row(7), rv.end_row(7), 1) == 1);
  rv.set(7, 0, 1);
  REQUIRE(std::count(rv.begin_row(7), rv.end_row(7), 0) == 8);
  REQUIRE(std::count(rv.begin_row(7), rv.end_row(7), 1) == 2);
  REQUIRE(std::count(rv.begin_row(7), rv.end_row(7), 2) == 0);
  rv.add_cols(100);
  REQUIRE(std::count(rv.begin_row(7), rv.end_row(7), 0) == 108);
  REQUIRE(std::count(rv.begin_row(7), rv.end_row(7), 1) == 2);
  REQUIRE(std::count(rv.begin_row(7), rv.end_row(7), 2) == 0);
  for (size_t i = 10; i < 19; i++) {
    rv.set(7, i, 2);
  }
  REQUIRE(std::count(rv.begin_row(7), rv.end_row(7), 0) == 99);
  REQUIRE(std::count(rv.begin_row(7), rv.end_row(7), 1) == 2);
  REQUIRE(std::count(rv.begin_row(7), rv.end_row(7), 2) == 9);
}

TEST_CASE("RecVec 14: clear", "[quick][recvec][14]") {
  RecVec<size_t> rv = RecVec<size_t>(10, 10);
  REQUIRE(rv.size() == 100);
  REQUIRE(rv.nr_cols() == 10);
  REQUIRE(rv.nr_rows() == 10);
  rv.clear();
  REQUIRE(rv.size() == 0);
  REQUIRE(rv.nr_cols() == 0);
  REQUIRE(rv.nr_rows() == 0);
}

TEST_CASE("RecVec 15: begin_row and end_row", "[quick][recvec][15]") {
  RecVec<size_t> rv = RecVec<size_t>(100, 2);
  for (size_t i = 0; i < rv.nr_rows(); i++) {
    for (auto it = rv.begin_row(i); it < rv.end_row(i); it++) {
      REQUIRE(*it == 0);
      if (i == 0) {
        *it = 666;
        REQUIRE(*it == 666);
      }
    }
  }
  REQUIRE(std::count(rv.begin_row(0), rv.end_row(0), 666) == 100);
  REQUIRE(std::count(rv.begin_row(1), rv.end_row(1), 666) == 0);
}

TEST_CASE("RecVec 16: cbegin_row and cend_row", "[quick][recvec][16]") {
  RecVec<size_t> rv = RecVec<size_t>(10, 10, 66);
  for (size_t i = 0; i < rv.nr_rows(); i++) {
    for (auto it = rv.cbegin_row(i); it < rv.cend_row(i); it++) {
      REQUIRE(*it == 66);
    }
  }
}

TEST_CASE("RecVec 17: iterator operator++ (postfix)", "[quick][recvec][17]") {
  RecVec<size_t> rv1 = RecVec<size_t>(100, 2);  // cols, rows
  rv1.add_cols(10);  // rv1 has 129 = 5 * 100 / 4 + 4 cols in total

  size_t val = 0;
  for (auto it = rv1.begin(); it < rv1.end(); it++) {
    REQUIRE(*it == 0);
    *it = val++;
  }
  REQUIRE(val == rv1.nr_cols() * rv1.nr_rows());
  REQUIRE(val == (100 + 10) * 2);

  val = 0;
  for (auto it = rv1.begin(); it < rv1.end(); it++) {
    REQUIRE(*it == val++);
  }
  REQUIRE(val == rv1.nr_cols() * rv1.nr_rows());
  REQUIRE(val == (100 + 10) * 2);

  for (auto it = rv1.rbegin(); it < rv1.rend(); it++) {
    REQUIRE(*it == --val);
  }
  REQUIRE(val == 0);

  RecVec<bool> rv2 = RecVec<bool>(100, 2);  // cols, rows
  rv2.add_cols(10);  // rv2 has 129 = 5 * 100 / 4 + 4 cols in total

  val = 0;
  for (auto it = rv2.begin(); it < rv2.end(); it++) {
    REQUIRE(*it == false);
    if (((it - rv2.begin()) % 2) == 0) {
      *it = true;
    }
    val++;
  }
  REQUIRE(val == rv2.nr_cols() * rv2.nr_rows());
  REQUIRE(val == (100 + 10) * 2);

  for (auto it = rv2.rbegin(); it < rv2.rend(); it++) {
    if (((it - rv2.rend() + 1) % 2) == 0) {
      REQUIRE(*it == true);
    } else {
      REQUIRE(*it == false);
      *it = true;
    }
    --val;
  }
  REQUIRE(val == 0);

  val = 0;
  for (auto it = rv2.begin(); it < rv2.end(); it++) {
    REQUIRE(*it == true);
    val++;
  }
  REQUIRE(val == rv2.nr_cols() * rv2.nr_rows());
  REQUIRE(val == (100 + 10) * 2);
}

TEST_CASE("RecVec 18: iterator operator++ (prefix)", "[quick][recvec][18]") {
  RecVec<size_t> rv1 = RecVec<size_t>(100, 2);  // cols, rows
  for (auto it = rv1.begin(); it < rv1.end(); ++it) {
    auto tmp(it);
    REQUIRE((++tmp - it) == 1);
    REQUIRE((it - tmp) == -1);
    REQUIRE((--tmp - it) == 0);
    REQUIRE(tmp == it);
    REQUIRE(++tmp == it + 1);
    REQUIRE((--tmp) == it);
    REQUIRE(tmp == it);

    REQUIRE((--tmp - it) == -1);
    REQUIRE((++tmp - it) == 0);

    REQUIRE(tmp == it);
    REQUIRE((--tmp) == it - 1);
    REQUIRE((++tmp) == it);
    REQUIRE(tmp == it);
  }

  rv1.add_cols(10);  // rv1 has 129 = 5 * 100 / 4 + 4 cols in total

  size_t val = 0;
  for (auto it = rv1.begin(); it < rv1.end(); ++it) {
    REQUIRE(*it == 0);
    *it = val++;
  }

  val = 0;
  for (auto it = rv1.begin(); it < rv1.end(); it++) {
    REQUIRE(*it == val++);
  }
  REQUIRE(val == rv1.nr_cols() * rv1.nr_rows());
  REQUIRE(val == (100 + 10) * 2);

  val = 0;
  for (auto it = rv1.begin(); it < rv1.end(); ++it) {
    REQUIRE(*it == val++);
  }
  REQUIRE(val == rv1.nr_cols() * rv1.nr_rows());
  REQUIRE(val == (100 + 10) * 2);

  for (auto it = rv1.begin(); it < rv1.end(); ++it) {
    auto tmp(it);
    REQUIRE((++tmp - it) == 1);
    REQUIRE((it - tmp) == -1);
    REQUIRE((--tmp - it) == 0);
    REQUIRE(tmp == it);
    REQUIRE(++tmp == it + 1);
    REQUIRE((--tmp) == it);
    REQUIRE(tmp == it);

    REQUIRE((--tmp - it) == -1);
    REQUIRE((++tmp - it) == 0);

    REQUIRE(tmp == it);
    REQUIRE((--tmp) == it - 1);
    REQUIRE((++tmp) == it);
    REQUIRE(tmp == it);
  }

  RecVec<bool> rv2 = RecVec<bool>(100, 2);  // cols, rows
  rv2.add_cols(10);  // rv2 has 129 = 5 * 100 / 4 + 4 cols in total

  val = 0;
  for (auto it = rv2.begin(); it < rv2.end(); ++it) {
    REQUIRE(*it == false);
    if ((it - rv2.begin()) % 6 == 4) {
      *it = true;
      REQUIRE(*it == true);
    }
    val++;
  }
  REQUIRE(val == rv2.nr_cols() * rv2.nr_rows());
  REQUIRE(val == (100 + 10) * 2);

  for (auto it = rv2.begin(); it < rv2.end(); ++it) {
    if ((it - rv2.begin()) % 6 == 4) {
      REQUIRE(*it == true);
    } else {
      REQUIRE(*it == false);
    }
    val--;
  }
  REQUIRE(val == 0);

  val = 0;
  for (auto it = rv2.begin(); it < rv2.end(); ++it) {
    auto tmp(it);

    REQUIRE((++tmp - it) == 1);
    REQUIRE((it - tmp) == -1);
    REQUIRE((--tmp - it) == 0);
    REQUIRE((++tmp) == it + 1);
    REQUIRE((--tmp) == it);
    REQUIRE(tmp == it);

    REQUIRE((--tmp - it) == -1);
    REQUIRE((++tmp - it) == 0);

    REQUIRE(tmp == it);
    REQUIRE((--tmp) == it - 1);
    REQUIRE((++tmp) == it);
    REQUIRE(tmp == it);
  }
}

TEST_CASE("RecVec 19: iterator operator-- (postfix)", "[quick][recvec][19]") {
  RecVec<size_t> rv = RecVec<size_t>(100, 2);  // cols, rows
  rv.add_cols(10);  // rv has 129 = 5 * 100 / 4 + 4 cols in total

  size_t val = 0;
  for (auto it = rv.end() - 1; it >= rv.begin(); it--) {
    REQUIRE(*it == 0);
    *it = val++;
  }

  val = 0;
  for (auto it = rv.end() - 1; it >= rv.begin(); it--) {
    REQUIRE(*it == val++);
  }
  REQUIRE(val == rv.nr_cols() * rv.nr_rows());
  REQUIRE(val == (100 + 10) * 2);

  for (auto it = rv.end() - 1; it >= rv.begin(); --it) {
    auto tmp(it);
    REQUIRE(--(++tmp) == it);
    REQUIRE(++(--tmp) == it);
  }

  for (auto it = rv.rbegin(); it < rv.rend(); it++) {
    auto tmp(it);
    REQUIRE(--(++tmp) == it);
    REQUIRE(++(--tmp) == it);
  }

  for (auto it = rv.rbegin(); it < rv.rend(); ++it) {
    auto tmp(it);
    REQUIRE(--(++tmp) == it);
    REQUIRE(++(--tmp) == it);
  }
}

TEST_CASE("RecVec 20: iterator operator-- (prefix)", "[quick][recvec][20]") {
  RecVec<size_t> rv = RecVec<size_t>(100, 2);  // cols, rows
  rv.add_cols(10);  // rv has 129 = 5 * 100 / 4 + 4 cols in total

  size_t val = 0;
  for (auto it = rv.end() - 1; it >= rv.begin(); --it) {
    REQUIRE(*it == 0);
    *it = val++;
  }

  val = 0;
  for (auto it = rv.end() - 1; it >= rv.begin(); --it) {
    REQUIRE(*it == val++);
  }
  REQUIRE(val == rv.nr_cols() * rv.nr_rows());
  REQUIRE(val == (100 + 10) * 2);

  for (auto it = rv.end() - 1; it >= rv.begin(); --it) {
    auto tmp(it);
    REQUIRE(--(++tmp) == it);
    REQUIRE(++(--tmp) == it);
  }

  for (auto it = rv.rbegin(); it < rv.rend(); it++) {
    auto tmp(it);
    REQUIRE(--(++tmp) == it);
    REQUIRE(++(--tmp) == it);
  }

  for (auto it = rv.rbegin(); it < rv.rend(); ++it) {
    auto tmp(it);
    REQUIRE(--(++tmp) == it);
    REQUIRE(++(--tmp) == it);
  }
}

TEST_CASE("RecVec 21: operator=", "[quick][recvec][21]") {
  RecVec<size_t> rv1 = RecVec<size_t>(10, 10, 3);
  RecVec<size_t> rv2 = RecVec<size_t>(9, 9, 2);
  rv1.operator       =(rv2);
  REQUIRE(rv1.nr_cols() == 9);
  REQUIRE(rv1.nr_rows() == 9);
  REQUIRE(
      std::all_of(rv1.begin(), rv1.end(), [](size_t val) { return val == 2; }));
  REQUIRE(rv2.nr_cols() == 9);
  REQUIRE(rv2.nr_rows() == 9);
  REQUIRE(
      std::all_of(rv2.begin(), rv2.end(), [](size_t val) { return val == 2; }));

  RecVec<bool> rv3 = RecVec<bool>(10, 10, false);
  RecVec<bool> rv4 = RecVec<bool>(9, 9, true);
  rv3.operator     =(rv4);
  REQUIRE(rv3.nr_cols() == 9);
  REQUIRE(rv3.nr_rows() == 9);
  REQUIRE(std::all_of(
      rv3.begin(), rv3.end(), [](bool val) { return val == true; }));
  REQUIRE(rv4.nr_cols() == 9);
  REQUIRE(rv4.nr_rows() == 9);
  REQUIRE(std::all_of(
      rv4.begin(), rv4.end(), [](bool val) { return val == true; }));
}

TEST_CASE("RecVec 22: operator== and operator!=", "[quick][recvec][22]") {
  RecVec<size_t> rv1 = RecVec<size_t>(10, 10, 3);
  RecVec<size_t> rv2 = RecVec<size_t>(10, 10, 2);

  REQUIRE(rv1 != rv2);  // wrong values

  rv1.add_cols(2);      // rv has 16 = 5 * 10 / 4 + 4 cols in total
  REQUIRE(rv1 != rv2);  // wrong dimensions

  rv2.add_cols(2);      // rv has 16 = 5 * 10 / 4 + 4 cols in total
  REQUIRE(rv1 != rv2);  // wrong values

  rv1.add_rows(1);
  REQUIRE(rv1 != rv2);  // wrong dimensions

  rv2.add_rows(1);
  REQUIRE(rv1 != rv2);  // wrong values

  REQUIRE(rv1.size() == 12 * 11);
  REQUIRE(rv2.size() == 12 * 11);

  std::fill(rv1.begin(), rv1.end(), 2);

  for (auto it1 = rv1.begin(), it2 = rv2.begin(); it1 < rv1.end();
       it1++, it2++) {
    REQUIRE(*it1 == *it2);
  }
  for (auto it1 = rv1.cbegin(), it2 = rv2.cbegin(); it1 < rv1.cend();
       it1++, it2++) {
    REQUIRE(*it1 == *it2);
  }
  for (auto it1 = rv1.rbegin(), it2 = rv2.rbegin(); it1 < rv1.rend();
       it1++, it2++) {
    REQUIRE(*it1 == *it2);
  }
  for (auto it1 = rv1.crbegin(), it2 = rv2.crbegin(); it1 < rv1.crend();
       it1++, it2++) {
    REQUIRE(*it1 == *it2);
  }
  REQUIRE(rv1 == rv2);

  RecVec<bool> rv3 = RecVec<bool>(10, 10, true);
  RecVec<bool> rv4 = RecVec<bool>(10, 10, false);

  REQUIRE(rv3 != rv4);  // wrong values

  rv3.add_cols(2);      // rv has 16 = 5 * 10 / 4 + 4 cols in total
  REQUIRE(rv3 != rv4);  // wrong dimensions

  rv4.add_cols(2);      // rv has 16 = 5 * 10 / 4 + 4 cols in total
  REQUIRE(rv3 != rv4);  // wrong values

  rv3.add_rows(1);
  REQUIRE(rv3 != rv4);  // wrong dimensions

  rv4.add_rows(1);
  REQUIRE(rv3 != rv4);  // wrong values

  REQUIRE(rv3.size() == 12 * 11);
  REQUIRE(rv4.size() == 12 * 11);

  std::fill(rv3.begin(), rv3.end(), false);

  for (auto it1 = rv3.begin(), it2 = rv4.begin(); it1 < rv3.end();
       it1++, it2++) {
    REQUIRE(*it1 == *it2);
  }
  for (auto it1 = rv3.cbegin(), it2 = rv4.cbegin(); it1 < rv3.cend();
       it1++, it2++) {
    REQUIRE(*it1 == *it2);
  }
  for (auto it1 = rv3.rbegin(), it2 = rv4.rbegin(); it1 < rv3.rend();
       it1++, it2++) {
    REQUIRE(*it1 == *it2);
  }
  for (auto it1 = rv3.crbegin(), it2 = rv4.crbegin(); it1 < rv3.crend();
       it1++, it2++) {
    REQUIRE(*it1 == *it2);
  }
  REQUIRE(rv3 == rv4);
}

TEST_CASE("RecVec 23: empty and clear", "[quick][recvec][23]") {
  RecVec<size_t> rv1 = RecVec<size_t>(10, 10);
  REQUIRE(!rv1.empty());
  rv1.clear();
  REQUIRE(rv1.empty());
  REQUIRE(rv1.size() == 0);
  REQUIRE(rv1.nr_rows() == 0);
  REQUIRE(rv1.nr_cols() == 0);

  RecVec<size_t> rv2 = RecVec<size_t>(10);
  REQUIRE(rv2.empty());
  REQUIRE(rv2.size() == 0);
  REQUIRE(rv2.nr_rows() == 0);
  REQUIRE(rv2.nr_cols() != 0);

  RecVec<bool> rv3 = RecVec<bool>(10, 10);
  REQUIRE(!rv3.empty());
  rv3.clear();
  REQUIRE(rv3.empty());
  REQUIRE(rv3.size() == 0);
  REQUIRE(rv3.nr_rows() == 0);
  REQUIRE(rv3.nr_cols() == 0);

  RecVec<bool> rv4 = RecVec<bool>(10);
  REQUIRE(rv4.empty());
  REQUIRE(rv4.size() == 0);
  REQUIRE(rv4.nr_rows() == 0);
  REQUIRE(rv4.nr_cols() != 0);
}

TEST_CASE("RecVec 24: max_size", "[quick][recvec][24]") {
  RecVec<size_t> rv1 = RecVec<size_t>(10, 10);
  REQUIRE(rv1.max_size() != 0);

  RecVec<bool> rv2 = RecVec<bool>(10);
  REQUIRE(rv2.max_size() != 0);
}

TEST_CASE("RecVec 25: swap", "[quick][recvec][25]") {
  RecVec<size_t> rv1 = RecVec<size_t>(10, 10, 3);
  RecVec<size_t> rv2 = RecVec<size_t>(9, 9, 2);

  rv1.add_cols(2);  // rv1 has 16 = 5 * 10 / 4 + 4 cols in total
  rv2.add_cols(1);  // rv2 has 15 = 5 * 9 / 4 + 4 cols in total

  REQUIRE(rv1.nr_cols() == 12);
  REQUIRE(rv1.nr_rows() == 10);
  REQUIRE(
      std::all_of(rv1.begin(), rv1.end(), [](size_t val) { return val == 3; }));
  REQUIRE(rv2.nr_cols() == 10);
  REQUIRE(rv2.nr_rows() == 9);
  REQUIRE(
      std::all_of(rv2.begin(), rv2.end(), [](size_t val) { return val == 2; }));

  rv1.swap(rv2);
  REQUIRE(rv1.nr_cols() == 10);
  REQUIRE(rv1.nr_rows() == 9);
  REQUIRE(
      std::all_of(rv1.begin(), rv1.end(), [](size_t val) { return val == 2; }));
  REQUIRE(rv2.nr_cols() == 12);
  REQUIRE(rv2.nr_rows() == 10);
  REQUIRE(
      std::all_of(rv2.begin(), rv2.end(), [](size_t val) { return val == 3; }));

  std::swap(rv1, rv2);
  REQUIRE(rv1.nr_cols() == 12);
  REQUIRE(rv1.nr_rows() == 10);
  REQUIRE(
      std::all_of(rv1.begin(), rv1.end(), [](size_t val) { return val == 3; }));
  REQUIRE(rv2.nr_cols() == 10);
  REQUIRE(rv2.nr_rows() == 9);
  REQUIRE(
      std::all_of(rv2.begin(), rv2.end(), [](size_t val) { return val == 2; }));

  RecVec<bool> rv3 = RecVec<bool>(10, 10, false);
  RecVec<bool> rv4 = RecVec<bool>(9, 9, true);

  rv3.add_cols(2);  // rv3 has 16 = 5 * 10 / 4 + 4 cols in total

  REQUIRE(rv3.nr_cols() == 12);
  REQUIRE(rv3.nr_rows() == 10);
  REQUIRE(std::all_of(
      rv3.begin(), rv3.end(), [](bool val) { return val == false; }));
  REQUIRE(rv4.nr_cols() == 9);
  REQUIRE(rv4.nr_rows() == 9);
  REQUIRE(std::all_of(
      rv4.begin(), rv4.end(), [](bool val) { return val == true; }));

  rv3.swap(rv4);
  REQUIRE(rv3.nr_cols() == 9);
  REQUIRE(rv3.nr_rows() == 9);
  REQUIRE(std::all_of(
      rv3.begin(), rv3.end(), [](bool val) { return val == true; }));
  REQUIRE(rv4.nr_cols() == 12);
  REQUIRE(rv4.nr_rows() == 10);
  REQUIRE(std::all_of(
      rv4.begin(), rv4.end(), [](bool val) { return val == false; }));

  std::swap(rv3, rv4);
  REQUIRE(rv3.nr_cols() == 12);
  REQUIRE(rv3.nr_rows() == 10);
  REQUIRE(std::all_of(
      rv3.begin(), rv3.end(), [](bool val) { return val == false; }));
  REQUIRE(rv4.nr_cols() == 9);
  REQUIRE(rv4.nr_rows() == 9);
  REQUIRE(std::all_of(
      rv4.begin(), rv4.end(), [](bool val) { return val == true; }));
}

TEST_CASE("RecVec 26: iterator arithmetic", "[quick][recvec][26]") {
  {
    RecVec<size_t> rv  = RecVec<size_t>(10, 10, 1000);
    size_t         val = 0;
    for (auto it = rv.begin(); it < rv.end(); it++) {
      *it = val++;
    }
    auto it = rv.begin();
    REQUIRE(*it == 0);
    for (int64_t i = 0; i < 100; i++) {
      REQUIRE(*(it + i) == i);
      it += i;
      REQUIRE(*it == i);
      it -= i;
      REQUIRE(*it == 0);
      REQUIRE(it == rv.begin());
      auto tmp(it);
      REQUIRE((tmp + i) - i == tmp);
      REQUIRE((i + tmp) - i == tmp);
      tmp += i;
      REQUIRE(tmp - it == i);
      REQUIRE(it - tmp == -i);
      tmp -= i;
      REQUIRE(tmp - it == 0);
      tmp -= i;
      REQUIRE(tmp - it == -i);
      REQUIRE(it - tmp == i);
    }
    for (int64_t i = 100; i < 200; i++) {
      it += i;
      it -= i;
      REQUIRE(*it == 0);
      REQUIRE(it == rv.begin());
      auto tmp(it);
      REQUIRE((tmp + i) - i == tmp);
      REQUIRE((i + tmp) - i == tmp);
      tmp += i;
      REQUIRE(tmp - it == i);
      REQUIRE(it - tmp == -i);
      tmp -= i;
      REQUIRE(tmp - it == 0);
      tmp -= i;
      REQUIRE(tmp - it == -i);
      REQUIRE(it - tmp == i);
    }
  }
  {
    RecVec<bool> rv = RecVec<bool>(10, 10, false);
    auto         it = rv.begin();
    REQUIRE(*it == false);
    for (int64_t i = 1; i < 100; i++) {
      *(rv.begin() + i) = true;
      REQUIRE(*(it + i) == true);
      it += i;
      REQUIRE(*it == true);
      it -= i;
      REQUIRE(*it == false);
      REQUIRE(it == rv.begin());
      auto tmp(it);
      REQUIRE((tmp + i) - i == tmp);
      REQUIRE((i + tmp) - i == tmp);
      tmp += i;
      REQUIRE(tmp - it == i);
      REQUIRE(it - tmp == -i);
      tmp -= i;
      REQUIRE(tmp - it == 0);
      tmp -= i;
      REQUIRE(tmp - it == -i);
      REQUIRE(it - tmp == i);
    }
    for (int64_t i = 100; i < 200; i++) {
      it += i;
      it -= i;
      REQUIRE(*it == 0);
      REQUIRE(it == rv.begin());
      auto tmp(it);
      REQUIRE((tmp + i) - i == tmp);
      REQUIRE((i + tmp) - i == tmp);
      tmp += i;
      REQUIRE(tmp - it == i);
      REQUIRE(it - tmp == -i);
      tmp -= i;
      REQUIRE(tmp - it == 0);
      tmp -= i;
      REQUIRE(tmp - it == -i);
      REQUIRE(it - tmp == i);
    }
  }
  {
    RecVec<size_t> rv = RecVec<size_t>(10, 10, 1000);
    rv.add_cols(2);
    size_t val = 0;
    for (auto it = rv.begin(); it < rv.end(); it++) {
      *it = val++;
    }

    auto it = rv.cbegin();
    REQUIRE(*it == 0);
    for (int64_t i = 0; i < 100; i++) {
      REQUIRE(*(it + i) == i);
      it += i;
      REQUIRE(*it == i);
      it -= i;
      REQUIRE(*it == 0);
      REQUIRE(it == rv.cbegin());
      auto tmp(it);
      REQUIRE((tmp + i) - i == tmp);
      REQUIRE((i + tmp) - i == tmp);
      tmp += i;
      REQUIRE(tmp - it == i);
      REQUIRE(tmp - it == i);
      REQUIRE(it - tmp == -i);
      tmp -= i;
      REQUIRE(tmp - it == 0);
      tmp -= i;
      REQUIRE(tmp - it == -i);
      REQUIRE(it - tmp == i);
    }
    for (int64_t i = 100; i < 200; i++) {
      it += i;
      it -= i;
      REQUIRE(*it == 0);
      REQUIRE(it == rv.cbegin());
      auto tmp(it);
      REQUIRE((tmp + i) - i == tmp);
      REQUIRE((i + tmp) - i == tmp);
      tmp += i;
      REQUIRE(tmp - it == i);
      REQUIRE(it - tmp == -i);
      tmp -= i;
      REQUIRE(tmp - it == 0);
      tmp -= i;
      REQUIRE(tmp - it == -i);
      REQUIRE(it - tmp == i);
    }
  }
  {
    RecVec<bool> rv = RecVec<bool>(10, 10, false);
    auto         it = rv.cbegin();
    REQUIRE(*it == false);
    for (int64_t i = 1; i < 100; i++) {
      *(rv.begin() + i) = true;
      REQUIRE(*(it + i) == true);
      it += i;
      REQUIRE(*it == true);
      it -= i;
      REQUIRE(*it == false);
      REQUIRE(it == rv.cbegin());
      auto tmp(it);
      REQUIRE((tmp + i) - i == tmp);
      REQUIRE((i + tmp) - i == tmp);
      tmp += i;
      REQUIRE(tmp - it == i);
      REQUIRE(it - tmp == -i);
      tmp -= i;
      REQUIRE(tmp - it == 0);
      tmp -= i;
      REQUIRE(tmp - it == -i);
      REQUIRE(it - tmp == i);
    }
    for (int64_t i = 100; i < 200; i++) {
      it += i;
      it -= i;
      REQUIRE(*it == 0);
      REQUIRE(it == rv.cbegin());
      auto tmp(it);
      REQUIRE((tmp + i) - i == tmp);
      REQUIRE((i + tmp) - i == tmp);
      tmp += i;
      REQUIRE(tmp - it == i);
      REQUIRE(it - tmp == -i);
      tmp -= i;
      REQUIRE(tmp - it == 0);
      tmp -= i;
      REQUIRE(tmp - it == -i);
      REQUIRE(it - tmp == i);
    }
  }
}

TEST_CASE("RecVec 27: iterator comparison", "[quick][recvec][27]") {
  {
    RecVec<size_t> rv = RecVec<size_t>(10, 10, 1000);
    REQUIRE(rv.begin() < rv.end());
    REQUIRE(!(rv.begin() > rv.end()));
    REQUIRE(rv.begin() <= rv.end());
    REQUIRE(!(rv.begin() >= rv.end()));

    REQUIRE(rv.begin() >= rv.begin());
    REQUIRE(rv.begin() <= rv.begin());
    REQUIRE(rv.end() >= rv.end());
    REQUIRE(rv.end() <= rv.end());
  }
  {
    RecVec<size_t> rv = RecVec<size_t>(10, 10, 1000);
    REQUIRE(rv.cbegin() < rv.cend());
    REQUIRE(!(rv.cbegin() > rv.cend()));
    REQUIRE(rv.cbegin() <= rv.cend());
    REQUIRE(!(rv.cbegin() >= rv.cend()));

    REQUIRE(rv.cbegin() >= rv.cbegin());
    REQUIRE(rv.cbegin() <= rv.cbegin());
    REQUIRE(rv.cend() >= rv.cend());
    REQUIRE(rv.cend() <= rv.cend());
  }
}

TEST_CASE("RecVec 28: iterator operator=", "[quick][recvec][28]") {
  RecVec<size_t> rv = RecVec<size_t>(10, 10, 1000);
  {
    auto it  = rv.begin();
    auto it2 = it;
    REQUIRE(it2 == it);
    REQUIRE(*it2 == 1000);
    it2 += 34;
    REQUIRE(it2 - it == 34);
  }
  {
    auto it  = rv.cbegin();
    auto it2 = it;
    REQUIRE(it2 == it);
    REQUIRE(*it2 == 1000);
    it2 += 34;
    REQUIRE(it2 - it == 34);
  }
}

TEST_CASE("RecVec 29: iterator operator[]", "[quick][recvec][29]") {
  {
    RecVec<size_t> rv = RecVec<size_t>(10, 10, 1000);
    {
      size_t val = 0;
      for (auto it = rv.begin(); it < rv.end(); it++) {
        *it = val++;
      }
      auto it = rv.begin();
      while (val > 0) {
        --val;
        REQUIRE(it[val] == val);
      }
    }
    {
      size_t val = 0;
      for (auto it = rv.begin(); it < rv.end(); it++) {
        *it = val++;
      }
      auto it = rv.cbegin();
      while (val > 0) {
        --val;
        REQUIRE(it[val] == val);
      }
    }
  }
  {
    RecVec<bool> rv = RecVec<bool>(10, 10, false);
    {
      size_t val = rv.size();
      auto   it  = rv.begin();
      auto   it2 = rv.rbegin();
      while (val > 0) {
        --val;
        *it2 = true;
        REQUIRE(it[val] == true);
        it2++;
      }
    }
    {
      size_t val = rv.size();
      auto   it  = rv.cbegin();
      auto   it2 = rv.rbegin();
      while (val > 0) {
        --val;
        *it2 = true;
        REQUIRE(it[val] == true);
        it2++;
      }
    }
  }
}

TEST_CASE("RecVec 30: iterator operator->", "[quick][recvec][30") {
  RecVec<RecVec<bool>> rv = RecVec<RecVec<bool>>(13, 13, RecVec<bool>());
  {
    auto it = rv.begin();
    REQUIRE(it->empty());
  }
  {
    auto it = rv.cbegin();
    REQUIRE(it->empty());
  }
}

TEST_CASE("RecVec 31: const_iterator operator++/--", "[quick][recvec][31]") {
  RecVec<size_t> rv1 = RecVec<size_t>(100, 2);  // cols, rows
  rv1.add_cols(10);  // rv1 has 129 = 5 * 100 / 4 + 4 cols in total

  size_t val = 0;
  for (auto it = rv1.begin(); it < rv1.end(); it++) {
    REQUIRE(*it == 0);
    *it = val++;
  }
  REQUIRE(val == rv1.nr_cols() * rv1.nr_rows());
  REQUIRE(val == (100 + 10) * 2);

  val = 0;
  for (auto it = rv1.cbegin(); it < rv1.cend(); it++) {
    REQUIRE(*it == val++);
  }
  REQUIRE(val == rv1.nr_cols() * rv1.nr_rows());
  REQUIRE(val == (100 + 10) * 2);

  for (auto it = rv1.crbegin(); it < rv1.crend(); it++) {
    REQUIRE(*it == --val);
  }
  REQUIRE(val == 0);

  RecVec<bool> rv2 = RecVec<bool>(100, 2);  // cols, rows
  rv2.add_cols(10);  // rv2 has 129 = 5 * 100 / 4 + 4 cols in total

  val = 0;
  for (auto it = rv2.begin(); it < rv2.end(); it++) {
    if (((it - rv2.begin()) % 2) == 0) {
      *it = true;
    }
    val++;
  }
  REQUIRE(val == rv2.nr_cols() * rv2.nr_rows());
  REQUIRE(val == (100 + 10) * 2);

  for (auto it = rv2.crbegin(); it < rv2.crend(); it++) {
    if (((it - rv2.crend() + 1) % 2) == 0) {
      REQUIRE(*it == true);
    } else {
      REQUIRE(*it == false);
    }
    --val;
  }
  REQUIRE(val == 0);

  val = 0;
  for (auto it = rv2.cbegin(); it < rv2.cend(); it++) {
    if (((it - rv2.cend() + 1) % 2) == 0) {
      REQUIRE(*it == false);
    } else {
      REQUIRE(*it == true);
    }
    val++;
  }
  REQUIRE(val == rv2.nr_cols() * rv2.nr_rows());
  REQUIRE(val == (100 + 10) * 2);
}

TEST_CASE("RecVec 32: const_iterator operator++/--", "[quick][recvec][32]") {
  RecVec<size_t> rv = RecVec<size_t>(1, 1, 6);  // cols, rows

  auto const it_b = rv.begin();
  REQUIRE(*it_b == 6);

  auto const it_e = rv.end();
  REQUIRE(*(it_e - 1) == 6);
}

TEST_CASE("RecVec 33: column iterators", "[quick][recvec][33]") {
  RecVec<size_t> rv = RecVec<size_t>(3, 3);
  for (size_t i = 0; i < rv.nr_cols(); i++) {
    std::fill(rv.begin_column(i), rv.end_column(i), i);
  }

  for (size_t i = 0; i < rv.nr_rows(); i++) {
    size_t j = 0;
    for (auto it = rv.begin_row(i); it < rv.end_row(i); it++, j++) {
      REQUIRE(*it == j);
    }
  }

  for (size_t i = 0; i < rv.nr_cols(); i++) {
    for (auto it = rv.cbegin_column(i); it < rv.cend_column(i); it++) {
      REQUIRE(*it == i);
    }
  }

  for (size_t i = 0; i < rv.nr_cols(); i++) {
    for (auto it = rv.cend_column(i) - 1; it >= rv.cbegin_column(i); it--) {
      REQUIRE(*it == i);
    }
  }

  for (size_t i = 0; i < rv.nr_cols(); i++) {
    for (auto it = rv.cbegin_column(i); it < rv.cend_column(i); ++it) {
      REQUIRE(*it == i);
    }
  }

  for (size_t i = 0; i < rv.nr_cols(); i++) {
    for (auto it = rv.cend_column(i) - 1; it >= rv.cbegin_column(i); --it) {
      REQUIRE(*it == i);
    }
  }

  for (size_t i = 0; i < rv.nr_cols(); i++) {
    for (auto it = rv.begin_column(i); it < rv.end_column(i); it++) {
      REQUIRE(*it == i);
    }
  }

  for (size_t i = 0; i < rv.nr_cols(); i++) {
    for (auto it = rv.end_column(i) - 1; it >= rv.begin_column(i); it--) {
      REQUIRE(*it == i);
    }
  }

  for (size_t i = 0; i < rv.nr_cols(); i++) {
    for (auto it = rv.begin_column(i); it < rv.end_column(i); ++it) {
      REQUIRE(*it == i);
    }
  }

  for (size_t i = 0; i < rv.nr_cols(); i++) {
    for (auto it = rv.end_column(i) - 1; it >= rv.begin_column(i); --it) {
      REQUIRE(*it == i);
    }
  }
}

TEST_CASE("RecVec 34: column iterator arithmetic", "[quick][recvec][34]") {
  {
    RecVec<size_t> rv  = RecVec<size_t>(10, 10, 1000);
    size_t         val = 0;
    for (size_t i = 0; i < rv.nr_cols(); i++) {
      std::fill(rv.begin_column(i), rv.end_column(i), i);
    }
    for (size_t j = 0; j < 10; j++) {
      auto it = rv.begin_column(j);
      REQUIRE(*it == j);
      for (int64_t i = 0; i < 10; i++) {
        REQUIRE(*(it + i) == j);
        it += i;
        REQUIRE(*it == j);
        it -= i;
        REQUIRE(*it == j);
        REQUIRE(it == rv.begin_column(j));
        auto tmp(it);
        REQUIRE((tmp + i) - i == tmp);
        REQUIRE((i + tmp) - i == tmp);
        tmp += i;
        REQUIRE(tmp - it == i);
        REQUIRE(it - tmp == -i);
        tmp -= i;
        REQUIRE(tmp - it == 0);
        tmp -= i;
        REQUIRE(tmp - it == -i);
        REQUIRE(it - tmp == i);
      }
      for (int64_t i = 10; i < 200; i++) {
        it += i;
        it -= i;
        REQUIRE(*it == j);
        REQUIRE(it == rv.begin_column(j));
        auto tmp(it);
        REQUIRE((tmp + i) - i == tmp);
        REQUIRE((i + tmp) - i == tmp);
        tmp += i;
        REQUIRE(tmp - it == i);
        REQUIRE(it - tmp == -i);
        tmp -= i;
        REQUIRE(tmp - it == 0);
        tmp -= i;
        REQUIRE(tmp - it == -i);
        REQUIRE(it - tmp == i);
      }
    }
  }
  {
    RecVec<bool> rv = RecVec<bool>(10, 10, false);
    for (size_t i = 0; i < rv.nr_cols(); i++) {
      std::fill(rv.begin_column(i), rv.end_column(i), i % 2);
    }
    for (size_t j = 0; j < 10; j++) {
      auto it = rv.begin_column(j);
      REQUIRE(*it == j % 2);
      for (int64_t i = 0; i < 10; i++) {
        *(rv.begin_column(j) + i) = *rv.begin_column(j);
        it += i;
        it -= i;
        REQUIRE(it == rv.begin_column(j));
        auto tmp(it);
        REQUIRE((tmp + i) - i == tmp);
        REQUIRE((i + tmp) - i == tmp);
        tmp += i;
        REQUIRE(tmp - it == i);
        REQUIRE(it - tmp == -i);
        tmp -= i;
        REQUIRE(tmp - it == 0);
        tmp -= i;
        REQUIRE(tmp - it == -i);
        REQUIRE(it - tmp == i);
      }
      for (int64_t i = 10; i < 200; i++) {
        it += i;
        it -= i;
        REQUIRE(it == rv.begin_column(j));
        auto tmp(it);
        REQUIRE((tmp + i) - i == tmp);
        REQUIRE((i + tmp) - i == tmp);
        tmp += i;
        REQUIRE(tmp - it == i);
        REQUIRE(it - tmp == -i);
        tmp -= i;
        REQUIRE(tmp - it == 0);
        tmp -= i;
        REQUIRE(tmp - it == -i);
        REQUIRE(it - tmp == i);
      }
    }
  }
}

TEST_CASE("RecVec 35: iterator assignment constructor", "[quick][recvec][35]") {
  RecVec<size_t> rv = RecVec<size_t>(100, 100);

  for (size_t i = 0; i < 100; i++) {
    for (size_t j = 0; j < 100; j++) {
      rv.set(i, j, (i + j) % 31);
    }
  }

  for (size_t i = 0; i < 99; i++) {
    auto it  = rv.begin_row(i);
    auto it2 = rv.begin_row(i + 1);

    it++;

    // the thing we really want to test
    it2 = it;

    for (; it2 != rv.end_row(i); ++it2) {
      REQUIRE(*it2 == *it);
      ++it;
    }
  }
}
