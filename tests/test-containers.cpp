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

#include <algorithm>         // for all_of, count, fill
#include <cstddef>           // for size_t
#include <cstdint>           // for int64_t
#include <initializer_list>  // for initializer_list
#include <numeric>           // for iota
#include <stdexcept>         // for out_of_range
#include <utility>           // for swap
#include <vector>            // for vector

#include "catch.hpp"      // for REQUIRE
#include "test-main.hpp"  // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/containers.hpp"  // for DynamicArray2, DynamicArray...

namespace libsemigroups {
  namespace detail {
    LIBSEMIGROUPS_TEST_CASE("DynamicArray2",
                            "001",
                            "default constructor with 3 default args",
                            "[containers][quick]") {
      DynamicArray2<bool> rv = DynamicArray2<bool>();
      REQUIRE(rv.size() == 0);
      REQUIRE(rv.number_of_rows() == 0);
      REQUIRE(rv.number_of_cols() == 0);
    }

    LIBSEMIGROUPS_TEST_CASE("DynamicArray2",
                            "002",
                            "default constructor with 2 default args",
                            "[containers][quick]") {
      DynamicArray2<size_t> rv = DynamicArray2<size_t>(5);
      REQUIRE(rv.size() == 0);
      REQUIRE(rv.number_of_cols() == 5);
      REQUIRE(rv.number_of_rows() == 0);
    }

    LIBSEMIGROUPS_TEST_CASE("DynamicArray2",
                            "003",
                            "default constructor with 1 default args",
                            "[containers][quick]") {
      DynamicArray2<bool> rv = DynamicArray2<bool>(5, 5);
      REQUIRE(rv.size() == 25);
      REQUIRE(rv.number_of_cols() == 5);
      REQUIRE(rv.number_of_rows() == 5);
      REQUIRE(std::all_of(
          rv.begin(), rv.end(), [](bool val) { return val == false; }));
    }

    LIBSEMIGROUPS_TEST_CASE("DynamicArray2",
                            "004",
                            "default constructor with 0 default args",
                            "[containers][quick]") {
      DynamicArray2<bool> rv = DynamicArray2<bool>(2, 7, true);
      REQUIRE(rv.size() == 14);
      REQUIRE(rv.number_of_cols() == 2);
      REQUIRE(rv.number_of_rows() == 7);
      REQUIRE(std::all_of(
          rv.begin(), rv.end(), [](bool val) { return val == true; }));
    }

    LIBSEMIGROUPS_TEST_CASE("DynamicArray2",
                            "005",
                            "copy constructor with 1 default args",
                            "[containers][quick]") {
      DynamicArray2<size_t> rv   = DynamicArray2<size_t>(3, 7, 666);
      DynamicArray2<size_t> copy = DynamicArray2<size_t>(rv);
      REQUIRE(copy.size() == 21);
      REQUIRE(copy.number_of_cols() == 3);
      REQUIRE(copy.number_of_rows() == 7);
      REQUIRE(std::all_of(
          copy.begin(), copy.end(), [](size_t val) { return val == 666; }));
    }

    LIBSEMIGROUPS_TEST_CASE("DynamicArray2",
                            "006",
                            "copy constructor with 0 default args",
                            "[containers][quick]") {
      DynamicArray2<size_t> rv   = DynamicArray2<size_t>(3, 7, 666);
      DynamicArray2<size_t> copy = DynamicArray2<size_t>(rv, 2);
      REQUIRE(copy.size() == 35);
      REQUIRE(copy.number_of_cols() == 5);
      REQUIRE(copy.number_of_rows() == 7);
      REQUIRE(std::all_of(
          copy.begin(), copy.end(), [](size_t val) { return val == 666; }));

      // Check when there are available extra columns already in rv.
      rv.add_cols(10);
      DynamicArray2<size_t> copy2(rv, 0);
      REQUIRE(copy2.size() == 91);
      REQUIRE(copy2.number_of_cols() == 13);
      REQUIRE(copy2.number_of_rows() == 7);
      REQUIRE(std::all_of(
          copy2.begin(), copy2.end(), [](size_t val) { return val == 666; }));
    }

    LIBSEMIGROUPS_TEST_CASE("DynamicArray2",
                            "007",
                            "add_rows",
                            "[containers][quick]") {
      DynamicArray2<size_t> rv = DynamicArray2<size_t>(3, 7, 666);
      rv.add_rows(1);
      REQUIRE(rv.size() == 24);
      REQUIRE(rv.number_of_cols() == 3);
      REQUIRE(rv.number_of_rows() == 8);
      REQUIRE(std::all_of(
          rv.begin(), rv.end(), [](size_t val) { return val == 666; }));
      rv.add_rows(2);
      REQUIRE(rv.size() == 30);
      REQUIRE(rv.number_of_cols() == 3);
      REQUIRE(rv.number_of_rows() == 10);
      REQUIRE(std::all_of(
          rv.begin(), rv.end(), [](size_t val) { return val == 666; }));
      rv.add_rows(1);
      REQUIRE(rv.size() == 33);
      REQUIRE(rv.number_of_cols() == 3);
      REQUIRE(rv.number_of_rows() == 11);
      REQUIRE(std::all_of(
          rv.begin(), rv.end(), [](size_t val) { return val == 666; }));
    }

    LIBSEMIGROUPS_TEST_CASE("DynamicArray2",
                            "008",
                            "add_rows",
                            "[containers][quick]") {
      DynamicArray2<size_t> rv = DynamicArray2<size_t>(3, 7, 666);
      rv.add_rows(10);
      REQUIRE(rv.size() == 51);
      REQUIRE(rv.number_of_cols() == 3);
      REQUIRE(rv.number_of_rows() == 17);
      REQUIRE(std::all_of(
          rv.begin(), rv.end(), [](size_t val) { return val == 666; }));
      rv.add_rows(7);
      REQUIRE(rv.size() == 72);
      REQUIRE(rv.number_of_cols() == 3);
      REQUIRE(rv.number_of_rows() == 24);
      REQUIRE(std::all_of(
          rv.begin(), rv.end(), [](size_t val) { return val == 666; }));
    }

    LIBSEMIGROUPS_TEST_CASE("DynamicArray2",
                            "009",
                            "add_cols",
                            "[containers][quick]") {
      DynamicArray2<size_t> rv = DynamicArray2<size_t>(100, 2, 666);
      rv.add_cols(10);
      REQUIRE(rv.size() == 220);
      REQUIRE(rv.number_of_cols() == 110);
      REQUIRE(rv.number_of_rows() == 2);
      REQUIRE(std::all_of(
          rv.begin(), rv.end(), [](size_t val) { return val == 666; }));
      rv.add_cols(5);
      REQUIRE(rv.size() == 230);
      REQUIRE(rv.number_of_cols() == 115);
      REQUIRE(rv.number_of_rows() == 2);
      REQUIRE(std::all_of(
          rv.begin(), rv.end(), [](size_t val) { return val == 666; }));
    }

    LIBSEMIGROUPS_TEST_CASE("DynamicArray2",
                            "010",
                            "set/get",
                            "[containers][quick]") {
      DynamicArray2<size_t> rv = DynamicArray2<size_t>(100, 50, 666);
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

    LIBSEMIGROUPS_TEST_CASE("DynamicArray2",
                            "011",
                            "append 1/2",
                            "[containers][quick]") {
      DynamicArray2<size_t> rv1 = DynamicArray2<size_t>(100, 50, 555);
      DynamicArray2<size_t> rv2 = DynamicArray2<size_t>(100, 50, 666);
      REQUIRE(rv1.size() == 5000);
      REQUIRE(rv1.number_of_cols() == 100);
      REQUIRE(rv1.number_of_rows() == 50);
      REQUIRE(rv2.size() == 5000);
      REQUIRE(rv2.number_of_cols() == 100);
      REQUIRE(rv2.number_of_rows() == 50);
      rv1.append(rv2);
      REQUIRE(rv1.size() == 10000);
      REQUIRE(rv1.number_of_cols() == 100);
      REQUIRE(rv1.number_of_rows() == 100);
      REQUIRE(rv2.size() == 5000);
      REQUIRE(rv2.number_of_cols() == 100);
      REQUIRE(rv2.number_of_rows() == 50);
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

    LIBSEMIGROUPS_TEST_CASE("DynamicArray2",
                            "012",
                            "append 2/2",
                            "[containers][quick]") {
      DynamicArray2<size_t> rv1 = DynamicArray2<size_t>(10, 10, 555);
      REQUIRE(rv1.size() == 100);
      REQUIRE(rv1.number_of_cols() == 10);
      REQUIRE(rv1.number_of_rows() == 10);
      rv1.add_rows(40);
      for (size_t i = 0; i < 9; i++) {
        rv1.add_cols(10);
      }
      REQUIRE(rv1.size() == 5000);
      REQUIRE(rv1.number_of_cols() == 100);
      REQUIRE(rv1.number_of_rows() == 50);

      DynamicArray2<size_t> rv2 = DynamicArray2<size_t>(3, 4, 666);
      rv2.add_rows(46);
      rv2.add_cols(97);
      REQUIRE(rv1.size() == 5000);
      REQUIRE(rv1.number_of_cols() == 100);
      REQUIRE(rv1.number_of_rows() == 50);

      rv1.append(rv2);
      REQUIRE(rv1.size() == 10000);
      REQUIRE(rv1.number_of_cols() == 100);
      REQUIRE(rv1.number_of_rows() == 100);
      REQUIRE(rv2.size() == 5000);
      REQUIRE(rv2.number_of_cols() == 100);
      REQUIRE(rv2.number_of_rows() == 50);
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

    LIBSEMIGROUPS_TEST_CASE("DynamicArray2",
                            "013",
                            "count",
                            "[containers][quick]") {
      DynamicArray2<size_t> rv = DynamicArray2<size_t>(10, 10);
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

    LIBSEMIGROUPS_TEST_CASE("DynamicArray2",
                            "014",
                            "clear",
                            "[containers][quick]") {
      DynamicArray2<size_t> rv = DynamicArray2<size_t>(10, 10);
      REQUIRE(rv.size() == 100);
      REQUIRE(rv.number_of_cols() == 10);
      REQUIRE(rv.number_of_rows() == 10);
      rv.clear();
      REQUIRE(rv.size() == 0);
      REQUIRE(rv.number_of_cols() == 0);
      REQUIRE(rv.number_of_rows() == 0);
    }

    LIBSEMIGROUPS_TEST_CASE("DynamicArray2",
                            "015",
                            "begin_row and end_row",
                            "[containers][quick]") {
      DynamicArray2<size_t> rv = DynamicArray2<size_t>(100, 2);
      for (size_t i = 0; i < rv.number_of_rows(); i++) {
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

    LIBSEMIGROUPS_TEST_CASE("DynamicArray2",
                            "016",
                            "cbegin_row and cend_row",
                            "[containers][quick]") {
      DynamicArray2<size_t> rv = DynamicArray2<size_t>(10, 10, 66);
      for (size_t i = 0; i < rv.number_of_rows(); i++) {
        for (auto it = rv.cbegin_row(i); it < rv.cend_row(i); it++) {
          REQUIRE(*it == 66);
        }
      }
    }

    LIBSEMIGROUPS_TEST_CASE("DynamicArray2",
                            "017",
                            "iterator operator++ (postfix)",
                            "[containers][quick]") {
      DynamicArray2<size_t> rv1 = DynamicArray2<size_t>(100, 2);  // cols, rows
      rv1.add_cols(10);  // rv1 has 129 = 5 * 100 / 4 + 4 cols in total

      size_t val = 0;
      for (auto it = rv1.begin(); it < rv1.end(); it++) {
        REQUIRE(*it == 0);
        *it = val++;
      }
      REQUIRE(val == rv1.number_of_cols() * rv1.number_of_rows());
      REQUIRE(val == (100 + 10) * 2);

      val = 0;
      for (auto it = rv1.begin(); it < rv1.end(); it++) {
        REQUIRE(*it == val++);
      }
      REQUIRE(val == rv1.number_of_cols() * rv1.number_of_rows());
      REQUIRE(val == (100 + 10) * 2);

      for (auto it = rv1.rbegin(); it < rv1.rend(); it++) {
        REQUIRE(*it == --val);
      }
      REQUIRE(val == 0);

      DynamicArray2<bool> rv2 = DynamicArray2<bool>(100, 2);  // cols, rows
      rv2.add_cols(10);  // rv2 has 129 = 5 * 100 / 4 + 4 cols in total

      val = 0;
      for (auto it = rv2.begin(); it < rv2.end(); it++) {
        REQUIRE(*it == false);
        if (((it - rv2.begin()) % 2) == 0) {
          *it = true;
        }
        val++;
      }
      REQUIRE(val == rv2.number_of_cols() * rv2.number_of_rows());
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
      REQUIRE(val == rv2.number_of_cols() * rv2.number_of_rows());
      REQUIRE(val == (100 + 10) * 2);
    }

    LIBSEMIGROUPS_TEST_CASE("DynamicArray2",
                            "018",
                            "iterator operator++ (prefix)",
                            "[containers][quick]") {
      DynamicArray2<size_t> rv1 = DynamicArray2<size_t>(100, 2);  // cols, rows
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
      REQUIRE(val == rv1.number_of_cols() * rv1.number_of_rows());
      REQUIRE(val == (100 + 10) * 2);

      val = 0;
      for (auto it = rv1.begin(); it < rv1.end(); ++it) {
        REQUIRE(*it == val++);
      }
      REQUIRE(val == rv1.number_of_cols() * rv1.number_of_rows());
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

      DynamicArray2<bool> rv2 = DynamicArray2<bool>(100, 2);  // cols, rows
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
      REQUIRE(val == rv2.number_of_cols() * rv2.number_of_rows());
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

    LIBSEMIGROUPS_TEST_CASE("DynamicArray2",
                            "019",
                            "iterator operator-- (postfix)",
                            "[containers][quick]") {
      DynamicArray2<size_t> rv = DynamicArray2<size_t>(100, 2);  // cols, rows
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
      REQUIRE(val == rv.number_of_cols() * rv.number_of_rows());
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

    LIBSEMIGROUPS_TEST_CASE("DynamicArray2",
                            "020",
                            "iterator operator-- (prefix)",
                            "[containers][quick]") {
      DynamicArray2<size_t> rv = DynamicArray2<size_t>(100, 2);  // cols, rows
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
      REQUIRE(val == rv.number_of_cols() * rv.number_of_rows());
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

    LIBSEMIGROUPS_TEST_CASE("DynamicArray2",
                            "021",
                            "operator=",
                            "[containers][quick]") {
      DynamicArray2<size_t> rv1     = DynamicArray2<size_t>(10, 10, 3);
      DynamicArray2<size_t> rv2     = DynamicArray2<size_t>(9, 9, 2);
      rv1.                  operator=(rv2);
      REQUIRE(rv1.number_of_cols() == 9);
      REQUIRE(rv1.number_of_rows() == 9);
      REQUIRE(std::all_of(
          rv1.begin(), rv1.end(), [](size_t val) { return val == 2; }));
      REQUIRE(rv2.number_of_cols() == 9);
      REQUIRE(rv2.number_of_rows() == 9);
      REQUIRE(std::all_of(
          rv2.begin(), rv2.end(), [](size_t val) { return val == 2; }));

      DynamicArray2<bool> rv3     = DynamicArray2<bool>(10, 10, false);
      DynamicArray2<bool> rv4     = DynamicArray2<bool>(9, 9, true);
      rv3.                operator=(rv4);
      REQUIRE(rv3.number_of_cols() == 9);
      REQUIRE(rv3.number_of_rows() == 9);
      REQUIRE(std::all_of(
          rv3.begin(), rv3.end(), [](bool val) { return val == true; }));
      REQUIRE(rv4.number_of_cols() == 9);
      REQUIRE(rv4.number_of_rows() == 9);
      REQUIRE(std::all_of(
          rv4.begin(), rv4.end(), [](bool val) { return val == true; }));
    }

    LIBSEMIGROUPS_TEST_CASE("DynamicArray2",
                            "022",
                            "operator== and operator!=",
                            "[containers][quick]") {
      DynamicArray2<size_t> rv1 = DynamicArray2<size_t>(10, 10, 3);
      DynamicArray2<size_t> rv2 = DynamicArray2<size_t>(10, 10, 2);

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

      DynamicArray2<bool> rv3(10, 10, true);
      DynamicArray2<bool> rv4(10, 10, false);

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

    LIBSEMIGROUPS_TEST_CASE("DynamicArray2",
                            "023",
                            "empty and clear",
                            "[containers][quick]") {
      DynamicArray2<size_t> rv1 = DynamicArray2<size_t>(10, 10);
      REQUIRE(!rv1.empty());
      rv1.clear();
      REQUIRE(rv1.empty());
      REQUIRE(rv1.size() == 0);
      REQUIRE(rv1.number_of_rows() == 0);
      REQUIRE(rv1.number_of_cols() == 0);

      DynamicArray2<size_t> rv2 = DynamicArray2<size_t>(10);
      REQUIRE(rv2.empty());
      REQUIRE(rv2.size() == 0);
      REQUIRE(rv2.number_of_rows() == 0);
      REQUIRE(rv2.number_of_cols() != 0);

      DynamicArray2<bool> rv3 = DynamicArray2<bool>(10, 10);
      REQUIRE(!rv3.empty());
      rv3.clear();
      REQUIRE(rv3.empty());
      REQUIRE(rv3.size() == 0);
      REQUIRE(rv3.number_of_rows() == 0);
      REQUIRE(rv3.number_of_cols() == 0);

      DynamicArray2<bool> rv4 = DynamicArray2<bool>(10);
      REQUIRE(rv4.empty());
      REQUIRE(rv4.size() == 0);
      REQUIRE(rv4.number_of_rows() == 0);
      REQUIRE(rv4.number_of_cols() != 0);
    }

    LIBSEMIGROUPS_TEST_CASE("DynamicArray2",
                            "024",
                            "max_size",
                            "[containers][quick]") {
      DynamicArray2<size_t> rv1 = DynamicArray2<size_t>(10, 10);
      REQUIRE(rv1.max_size() != 0);

      DynamicArray2<bool> rv2 = DynamicArray2<bool>(10);
      REQUIRE(rv2.max_size() != 0);
    }

    LIBSEMIGROUPS_TEST_CASE("DynamicArray2",
                            "025",
                            "swap",
                            "[containers][quick]") {
      DynamicArray2<size_t> rv1 = DynamicArray2<size_t>(10, 10, 3);
      DynamicArray2<size_t> rv2 = DynamicArray2<size_t>(9, 9, 2);

      rv1.add_cols(2);  // rv1 has 16 = 5 * 10 / 4 + 4 cols in total
      rv2.add_cols(1);  // rv2 has 15 = 5 * 9 / 4 + 4 cols in total

      REQUIRE(rv1.number_of_cols() == 12);
      REQUIRE(rv1.number_of_rows() == 10);
      REQUIRE(std::all_of(
          rv1.begin(), rv1.end(), [](size_t val) { return val == 3; }));
      REQUIRE(rv2.number_of_cols() == 10);
      REQUIRE(rv2.number_of_rows() == 9);
      REQUIRE(std::all_of(
          rv2.begin(), rv2.end(), [](size_t val) { return val == 2; }));

      rv1.swap(rv2);
      REQUIRE(rv1.number_of_cols() == 10);
      REQUIRE(rv1.number_of_rows() == 9);
      REQUIRE(std::all_of(
          rv1.begin(), rv1.end(), [](size_t val) { return val == 2; }));
      REQUIRE(rv2.number_of_cols() == 12);
      REQUIRE(rv2.number_of_rows() == 10);
      REQUIRE(std::all_of(
          rv2.begin(), rv2.end(), [](size_t val) { return val == 3; }));

      std::swap(rv1, rv2);
      REQUIRE(rv1.number_of_cols() == 12);
      REQUIRE(rv1.number_of_rows() == 10);
      REQUIRE(std::all_of(
          rv1.begin(), rv1.end(), [](size_t val) { return val == 3; }));
      REQUIRE(rv2.number_of_cols() == 10);
      REQUIRE(rv2.number_of_rows() == 9);
      REQUIRE(std::all_of(
          rv2.begin(), rv2.end(), [](size_t val) { return val == 2; }));

      DynamicArray2<bool> rv3 = DynamicArray2<bool>(10, 10, false);
      DynamicArray2<bool> rv4 = DynamicArray2<bool>(9, 9, true);

      rv3.add_cols(2);  // rv3 has 16 = 5 * 10 / 4 + 4 cols in total

      REQUIRE(rv3.number_of_cols() == 12);
      REQUIRE(rv3.number_of_rows() == 10);
      REQUIRE(std::all_of(
          rv3.begin(), rv3.end(), [](bool val) { return val == false; }));
      REQUIRE(rv4.number_of_cols() == 9);
      REQUIRE(rv4.number_of_rows() == 9);
      REQUIRE(std::all_of(
          rv4.begin(), rv4.end(), [](bool val) { return val == true; }));

      rv3.swap(rv4);
      REQUIRE(rv3.number_of_cols() == 9);
      REQUIRE(rv3.number_of_rows() == 9);
      REQUIRE(std::all_of(
          rv3.begin(), rv3.end(), [](bool val) { return val == true; }));
      REQUIRE(rv4.number_of_cols() == 12);
      REQUIRE(rv4.number_of_rows() == 10);
      REQUIRE(std::all_of(
          rv4.begin(), rv4.end(), [](bool val) { return val == false; }));

      std::swap(rv3, rv4);
      REQUIRE(rv3.number_of_cols() == 12);
      REQUIRE(rv3.number_of_rows() == 10);
      REQUIRE(std::all_of(
          rv3.begin(), rv3.end(), [](bool val) { return val == false; }));
      REQUIRE(rv4.number_of_cols() == 9);
      REQUIRE(rv4.number_of_rows() == 9);
      REQUIRE(std::all_of(
          rv4.begin(), rv4.end(), [](bool val) { return val == true; }));
    }

    LIBSEMIGROUPS_TEST_CASE("DynamicArray2",
                            "026",
                            "iterator arithmetic",
                            "[containers][quick]") {
      {
        DynamicArray2<size_t> rv  = DynamicArray2<size_t>(10, 10, 1000);
        size_t                val = 0;
        for (auto it = rv.begin(); it < rv.end(); it++) {
          *it = val++;
        }
        auto it = rv.begin();
        REQUIRE(*it == 0);
        for (int64_t i = 0; i < 100; i++) {
          REQUIRE(*(it + i) == size_t(i));
          it += i;
          REQUIRE(*it == size_t(i));
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
        DynamicArray2<bool> rv = DynamicArray2<bool>(10, 10, false);
        auto                it = rv.begin();
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
        DynamicArray2<size_t> rv = DynamicArray2<size_t>(10, 10, 1000);
        rv.add_cols(2);
        size_t val = 0;
        for (auto it = rv.begin(); it < rv.end(); it++) {
          *it = val++;
        }

        auto it = rv.cbegin();
        REQUIRE(*it == 0);
        for (int64_t i = 0; i < 100; i++) {
          REQUIRE(*(it + i) == size_t(i));
          it += i;
          REQUIRE(*it == size_t(i));
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
        DynamicArray2<bool> rv = DynamicArray2<bool>(10, 10, false);
        auto                it = rv.cbegin();
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

    LIBSEMIGROUPS_TEST_CASE("DynamicArray2",
                            "027",
                            "iterator comparison",
                            "[containers][quick]") {
      {
        DynamicArray2<size_t> rv = DynamicArray2<size_t>(10, 10, 1000);
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
        DynamicArray2<size_t> rv = DynamicArray2<size_t>(10, 10, 1000);
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

    LIBSEMIGROUPS_TEST_CASE("DynamicArray2",
                            "028",
                            "iterator operator=",
                            "[containers][quick]") {
      DynamicArray2<size_t> rv = DynamicArray2<size_t>(10, 10, 1000);
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

    LIBSEMIGROUPS_TEST_CASE("DynamicArray2",
                            "029",
                            "iterator operator[]",
                            "[containers][quick]") {
      {
        DynamicArray2<size_t> rv = DynamicArray2<size_t>(10, 10, 1000);
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
        DynamicArray2<bool> rv = DynamicArray2<bool>(10, 10, false);
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

    LIBSEMIGROUPS_TEST_CASE("DynamicArray2",
                            "030",
                            "iterator operator->",
                            "[containers][quick][30") {
      DynamicArray2<DynamicArray2<bool>> rv
          = DynamicArray2<DynamicArray2<bool>>(13, 13, DynamicArray2<bool>());
      {
        auto it = rv.begin();
        REQUIRE(it->empty());
      }
      {
        auto it = rv.cbegin();
        REQUIRE(it->empty());
      }
    }

    LIBSEMIGROUPS_TEST_CASE("DynamicArray2",
                            "031",
                            "const_iterator operator++/--",
                            "[containers][quick]") {
      DynamicArray2<size_t> rv1 = DynamicArray2<size_t>(100, 2);  // cols, rows
      rv1.add_cols(10);  // rv1 has 129 = 5 * 100 / 4 + 4 cols in total

      size_t val = 0;
      for (auto it = rv1.begin(); it < rv1.end(); it++) {
        REQUIRE(*it == 0);
        *it = val++;
      }
      REQUIRE(val == rv1.number_of_cols() * rv1.number_of_rows());
      REQUIRE(val == (100 + 10) * 2);

      val = 0;
      for (auto it = rv1.cbegin(); it < rv1.cend(); it++) {
        REQUIRE(*it == val++);
      }
      REQUIRE(val == rv1.number_of_cols() * rv1.number_of_rows());
      REQUIRE(val == (100 + 10) * 2);

      for (auto it = rv1.crbegin(); it < rv1.crend(); it++) {
        REQUIRE(*it == --val);
      }
      REQUIRE(val == 0);

      DynamicArray2<bool> rv2 = DynamicArray2<bool>(100, 2);  // cols, rows
      rv2.add_cols(10);  // rv2 has 129 = 5 * 100 / 4 + 4 cols in total

      val = 0;
      for (auto it = rv2.begin(); it < rv2.end(); it++) {
        if (((it - rv2.begin()) % 2) == 0) {
          *it = true;
        }
        val++;
      }
      REQUIRE(val == rv2.number_of_cols() * rv2.number_of_rows());
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
      REQUIRE(val == rv2.number_of_cols() * rv2.number_of_rows());
      REQUIRE(val == (100 + 10) * 2);
    }

    LIBSEMIGROUPS_TEST_CASE("DynamicArray2",
                            "032",
                            "const_iterator operator++/--",
                            "[containers][quick]") {
      DynamicArray2<size_t> rv = DynamicArray2<size_t>(1, 1, 6);  // cols, rows

      auto const it_b = rv.begin();
      REQUIRE(*it_b == 6);

      auto const it_e = rv.end();
      REQUIRE(*(it_e - 1) == 6);
    }

    LIBSEMIGROUPS_TEST_CASE("DynamicArray2",
                            "033",
                            "column iterators",
                            "[containers][quick]") {
      DynamicArray2<size_t> rv = DynamicArray2<size_t>(3, 3);
      for (size_t i = 0; i < rv.number_of_cols(); i++) {
        std::fill(rv.begin_column(i), rv.end_column(i), i);
      }

      for (size_t i = 0; i < rv.number_of_rows(); i++) {
        size_t j = 0;
        for (auto it = rv.begin_row(i); it < rv.end_row(i); it++, j++) {
          REQUIRE(*it == j);
        }
      }

      for (size_t i = 0; i < rv.number_of_cols(); i++) {
        for (auto it = rv.cbegin_column(i); it < rv.cend_column(i); it++) {
          REQUIRE(*it == i);
        }
      }

      for (size_t i = 0; i < rv.number_of_cols(); i++) {
        for (auto it = rv.cend_column(i) - 1; it >= rv.cbegin_column(i); it--) {
          REQUIRE(*it == i);
        }
      }

      for (size_t i = 0; i < rv.number_of_cols(); i++) {
        for (auto it = rv.cbegin_column(i); it < rv.cend_column(i); ++it) {
          REQUIRE(*it == i);
        }
      }

      for (size_t i = 0; i < rv.number_of_cols(); i++) {
        for (auto it = rv.cend_column(i) - 1; it >= rv.cbegin_column(i); --it) {
          REQUIRE(*it == i);
        }
      }

      for (size_t i = 0; i < rv.number_of_cols(); i++) {
        for (auto it = rv.begin_column(i); it < rv.end_column(i); it++) {
          REQUIRE(*it == i);
        }
      }

      for (size_t i = 0; i < rv.number_of_cols(); i++) {
        for (auto it = rv.end_column(i) - 1; it >= rv.begin_column(i); it--) {
          REQUIRE(*it == i);
        }
      }

      for (size_t i = 0; i < rv.number_of_cols(); i++) {
        for (auto it = rv.begin_column(i); it < rv.end_column(i); ++it) {
          REQUIRE(*it == i);
        }
      }

      for (size_t i = 0; i < rv.number_of_cols(); i++) {
        for (auto it = rv.end_column(i) - 1; it >= rv.begin_column(i); --it) {
          REQUIRE(*it == i);
        }
      }
    }

    LIBSEMIGROUPS_TEST_CASE("DynamicArray2",
                            "034",
                            "column iterator arithmetic",
                            "[containers][quick][no-valgrind]") {
      {
        DynamicArray2<size_t> rv = DynamicArray2<size_t>(10, 10, 1000);
        for (size_t i = 0; i < rv.number_of_cols(); i++) {
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
        DynamicArray2<bool> rv = DynamicArray2<bool>(10, 10, false);
        for (size_t i = 0; i < rv.number_of_cols(); i++) {
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

    LIBSEMIGROUPS_TEST_CASE("DynamicArray2",
                            "035",
                            "iterator assignment constructor",
                            "[containers][quick]") {
      DynamicArray2<size_t> rv = DynamicArray2<size_t>(100, 100);

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

    LIBSEMIGROUPS_TEST_CASE("DynamicArray2",
                            "036",
                            "reserve method",
                            "[containers][quick]") {
      DynamicArray2<size_t> rv = DynamicArray2<size_t>(100, 100);
      rv.reserve(1000);
      REQUIRE(rv.number_of_cols() == 100);
      REQUIRE(rv.number_of_rows() == 100);
    }

    LIBSEMIGROUPS_TEST_CASE("DynamicArray2",
                            "037",
                            "erase column",
                            "[containers][quick]") {
      DynamicArray2<size_t> rv = DynamicArray2<size_t>(10, 10);
      for (size_t i = 0; i < 10; i++) {
        std::iota(rv.begin_row(i), rv.end_row(i), 0);
      }

      rv.erase_column(2);
      REQUIRE(rv.number_of_cols() == 9);
      REQUIRE(rv.number_of_rows() == 10);
      for (size_t i = 0; i < 10; ++i) {
        REQUIRE(rv.get(i, 1) == 1);
        REQUIRE(rv.get(i, 2) == 3);
        REQUIRE(rv.get(i, 3) == 4);
      }
    }

    LIBSEMIGROUPS_TEST_CASE("DynamicArray2",
                            "038",
                            "swap_rows",
                            "[containers][quick]") {
      DynamicArray2<size_t> rv = DynamicArray2<size_t>(3, 10);
      for (size_t i = 0; i < 10; i++) {
        std::fill(rv.begin_row(i), rv.end_row(i), i);
      }
      rv.swap_rows(4, 8);
      REQUIRE(std::all_of(
          rv.begin_row(4), rv.end_row(4), [](size_t x) { return x == 8; }));
      REQUIRE(std::all_of(
          rv.begin_row(8), rv.end_row(8), [](size_t x) { return x == 4; }));
      for (size_t i = 0; i < 10; i++) {
        if (i != 4 && i != 8) {
          REQUIRE(std::all_of(rv.begin_row(i), rv.end_row(i), [i](size_t x) {
            return x == i;
          }));
        }
      }
    }

    LIBSEMIGROUPS_TEST_CASE("DynamicArray2",
                            "039",
                            "apply_row_permutation",
                            "[containers][quick]") {
      DynamicArray2<size_t> rv = DynamicArray2<size_t>(3, 10);
      for (size_t i = 0; i < 10; i++) {
        std::fill(rv.begin_row(i), rv.end_row(i), i);
      }
      std::vector<size_t> p = {1, 2, 3, 4, 5, 6, 7, 0, 9, 8};
      // copy p, since the apply_row_permutation method modifies p.
      std::vector<size_t> q = p;
      rv.apply_row_permutation(p);

      for (size_t i = 0; i < 10; i++) {
        REQUIRE(std::all_of(rv.begin_row(i), rv.end_row(i), [&q, &i](size_t x) {
          return x == q[i];
        }));
      }
    }

    LIBSEMIGROUPS_TEST_CASE("DynamicArray2",
                            "040",
                            "swap",
                            "[containers][quick]") {
      DynamicArray2<size_t> da = DynamicArray2<size_t>({{0, 1}, {2, 3}});
      REQUIRE(da.get(0, 0) == 0);
      REQUIRE(da.get(0, 1) == 1);
      REQUIRE(da.get(1, 0) == 2);
      REQUIRE(da.get(1, 1) == 3);
      da.swap(0, 0, 1, 1);
      REQUIRE(da.get(0, 0) == 3);
      REQUIRE(da.get(0, 1) == 1);
      REQUIRE(da.get(1, 0) == 2);
      REQUIRE(da.get(1, 1) == 0);
    }

    LIBSEMIGROUPS_TEST_CASE("DynamicArray2",
                            "041",
                            "shrink_rows_to",
                            "[containers][quick]") {
      DynamicArray2<size_t> da = DynamicArray2<size_t>({{0, 1}, {2, 3}});
      REQUIRE(da.number_of_rows() == 2);
      REQUIRE(da.number_of_cols() == 2);
      da.shrink_rows_to(3);
      REQUIRE(da.number_of_rows() == 2);
      REQUIRE(da.number_of_cols() == 2);
      REQUIRE(da == DynamicArray2<size_t>({{0, 1}, {2, 3}}));
      da.shrink_rows_to(1);
      REQUIRE(da.number_of_rows() == 1);
      REQUIRE(da.number_of_cols() == 2);
      REQUIRE(da == DynamicArray2<size_t>({{0, 1}}));

      da.add_rows(3);
      da.add_cols(2);
      da.set_default_value(0);
      REQUIRE(da.number_of_rows() == 4);
      REQUIRE(da.number_of_cols() == 4);
      REQUIRE(da
              == DynamicArray2<size_t>(
                  {{0, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}));
      da.shrink_rows_to(5);
      REQUIRE(da
              == DynamicArray2<size_t>(
                  {{0, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}));
      da.shrink_rows_to(2);
      REQUIRE(da == DynamicArray2<size_t>({{0, 1, 0, 0}, {0, 0, 0, 0}}));
    }

    LIBSEMIGROUPS_TEST_CASE("DynamicArray2",
                            "042",
                            "shrink_rows_to - for range",
                            "[containers][quick]") {
      DynamicArray2<size_t> da = DynamicArray2<size_t>({{0, 1}, {2, 3}});
      REQUIRE(da.number_of_rows() == 2);
      REQUIRE(da.number_of_cols() == 2);
      da.shrink_rows_to(3);
      REQUIRE(da.number_of_rows() == 2);
      REQUIRE(da.number_of_cols() == 2);
      REQUIRE(da == DynamicArray2<size_t>({{0, 1}, {2, 3}}));
      da.shrink_rows_to(1, 2);
      REQUIRE(da.number_of_rows() == 1);
      REQUIRE(da.number_of_cols() == 2);
      REQUIRE(da == DynamicArray2<size_t>({{2, 3}}));

      da.add_rows(3);
      da.add_cols(2);
      da.set_default_value(0);
      REQUIRE(da.number_of_rows() == 4);
      REQUIRE(da.number_of_cols() == 4);
      REQUIRE(da
              == DynamicArray2<size_t>(
                  {{2, 3, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}));
      da.shrink_rows_to(1, 4);
      REQUIRE(
          da
          == DynamicArray2<size_t>({{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}));
      da.set(2, 1, 3);
      REQUIRE(
          da
          == DynamicArray2<size_t>({{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 3, 0, 0}}));
      da.shrink_rows_to(1, 3);
      REQUIRE(da == DynamicArray2<size_t>({{0, 0, 0, 0}, {0, 3, 0, 0}}));
    }

    LIBSEMIGROUPS_TEST_CASE("StaticVector2",
                            "043",
                            "all",
                            "[containers][quick]") {
      StaticVector2<size_t, 3> sv;
      REQUIRE(sv.size(0) == 0);
      REQUIRE(sv.size(1) == 0);
      REQUIRE(sv.size(2) == 0);
      sv.push_back(0, 0);
      sv.push_back(0, 1);
      sv.push_back(0, 2);
      sv.push_back(1, 3);
      sv.push_back(1, 4);
      sv.push_back(2, 5);
      REQUIRE(sv.size(0) == 3);
      REQUIRE(sv.size(1) == 2);
      REQUIRE(sv.size(2) == 1);
      sv.clear();
      REQUIRE(sv.size(0) == 0);
      REQUIRE(sv.size(1) == 0);
      REQUIRE(sv.size(2) == 0);
      sv.push_back(0, 0);
      sv.push_back(0, 1);
      sv.push_back(0, 2);
      sv.push_back(1, 3);
      sv.push_back(1, 4);
      sv.push_back(2, 5);
      REQUIRE(sv.back(0) == 2);
      REQUIRE(sv.back(1) == 4);
      REQUIRE(sv.back(2) == 5);
      REQUIRE(sv.at(2, 0) == 5);
      REQUIRE(sv.at(1, 0) == 3);
      REQUIRE(sv.at(0, 0) == 0);
      REQUIRE(std::vector<size_t>(sv.cbegin(0), sv.cend(0))
              == std::vector<size_t>({0, 1, 2}));
      REQUIRE(std::vector<size_t>(sv.cbegin(1), sv.cend(1))
              == std::vector<size_t>({3, 4}));
      REQUIRE(std::vector<size_t>(sv.cbegin(2), sv.cend(2))
              == std::vector<size_t>({5}));
      REQUIRE(std::vector<size_t>(sv.begin(0), sv.end(0))
              == std::vector<size_t>({0, 1, 2}));
      REQUIRE(std::vector<size_t>(sv.begin(1), sv.end(1))
              == std::vector<size_t>({3, 4}));
      REQUIRE(std::vector<size_t>(sv.begin(2), sv.end(2))
              == std::vector<size_t>({5}));
    }

    LIBSEMIGROUPS_TEST_CASE("Array2", "044", "all", "[containers][quick]") {
      Array2<size_t, 3> rry;
      rry.fill(10);
      REQUIRE(std::vector<size_t>(rry.cbegin(0), rry.cend(0))
              == std::vector<size_t>({10, 10, 10}));
      REQUIRE(std::vector<size_t>(rry.cbegin(1), rry.cend(1))
              == std::vector<size_t>({10, 10, 10}));
      REQUIRE(std::vector<size_t>(rry.cbegin(2), rry.cend(2))
              == std::vector<size_t>({10, 10, 10}));
      rry[0] = {0, 1, 2};
      REQUIRE(std::vector<size_t>(rry.cbegin(0), rry.cend(0))
              == std::vector<size_t>({0, 1, 2}));
      REQUIRE(std::vector<size_t>(rry.cbegin(1), rry.cend(1))
              == std::vector<size_t>({10, 10, 10}));
      REQUIRE(std::vector<size_t>(rry.cbegin(2), rry.cend(2))
              == std::vector<size_t>({10, 10, 10}));
      REQUIRE(rry[0][0] == 0);
      REQUIRE(rry[1][1] == 10);
      REQUIRE(rry[2][2] == 10);
      REQUIRE(rry.at(0, 0) == 0);
      REQUIRE(rry.at(1, 1) == 10);
      REQUIRE(rry.at(2, 2) == 10);
      REQUIRE_THROWS_AS(rry.at(10, 0), std::out_of_range);
      REQUIRE_THROWS_AS(rry.at(0, 10), std::out_of_range);
      rry.fill(11);
      rry[1] = {3, 4, 5};
      REQUIRE(std::vector<size_t>(rry.begin(0), rry.end(0))
              == std::vector<size_t>({11, 11, 11}));
      REQUIRE(std::vector<size_t>(rry.begin(1), rry.end(1))
              == std::vector<size_t>({3, 4, 5}));
      REQUIRE(std::vector<size_t>(rry.begin(2), rry.end(2))
              == std::vector<size_t>({11, 11, 11}));
    }

    LIBSEMIGROUPS_TEST_CASE("StaticTriVector2",
                            "045",
                            "all",
                            "[containers][quick]") {
      StaticTriVector2<size_t, 3> stv;
      REQUIRE(stv.size(0) == 0);
      REQUIRE(stv.size(1) == 0);
      REQUIRE(stv.size(2) == 0);
      stv.push_back(0, 0);
      stv.push_back(0, 1);
      stv.push_back(0, 2);
      stv.push_back(1, 3);
      stv.push_back(1, 4);
      stv.push_back(2, 5);
      REQUIRE(stv.size(0) == 3);
      REQUIRE(stv.size(1) == 2);
      REQUIRE(stv.size(2) == 1);
      stv.clear();
      REQUIRE(stv.size(0) == 0);
      REQUIRE(stv.size(1) == 0);
      REQUIRE(stv.size(2) == 0);
      stv.push_back(0, 0);
      stv.push_back(0, 1);
      stv.push_back(0, 2);
      stv.push_back(1, 3);
      stv.push_back(1, 4);
      stv.push_back(2, 5);
      REQUIRE(stv.back(0) == 2);
      REQUIRE(stv.back(1) == 4);
      REQUIRE(stv.back(2) == 5);
      REQUIRE(stv.at(2, 0) == 5);
      REQUIRE(stv.at(1, 0) == 3);
      REQUIRE(stv.at(0, 0) == 0);
      REQUIRE(std::vector<size_t>(stv.cbegin(0), stv.cend(0))
              == std::vector<size_t>({0, 1, 2}));
      REQUIRE(std::vector<size_t>(stv.cbegin(1), stv.cend(1))
              == std::vector<size_t>({3, 4}));
      REQUIRE(std::vector<size_t>(stv.cbegin(2), stv.cend(2))
              == std::vector<size_t>({5}));
      REQUIRE(std::vector<size_t>(stv.begin(0), stv.end(0))
              == std::vector<size_t>({0, 1, 2}));
      REQUIRE(std::vector<size_t>(stv.begin(1), stv.end(1))
              == std::vector<size_t>({3, 4}));
      REQUIRE(std::vector<size_t>(stv.begin(2), stv.end(2))
              == std::vector<size_t>({5}));
    }
  }  // namespace detail
}  // namespace libsemigroups
