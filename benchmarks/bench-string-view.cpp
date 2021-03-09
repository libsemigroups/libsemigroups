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

#include "libsemigroups/string-view.hpp"  // for MultiStringView

namespace libsemigroups {

  TEST_CASE("MultiStringView iterators", "[quick]") {
    std::string w(0x4C4B40, 'a');
    REQUIRE(w.size() == 0x4C4B40);  // 5 million
    BENCHMARK("std::string") {
      for (auto it = w.cbegin(); it != w.cend(); ++it) {
        REQUIRE(*it == 'a');
      }
    };
    detail::MultiStringView x(w.cbegin(), w.cend());
    BENCHMARK("StringView 1 block") {
      for (auto it = x.cbegin(); it != x.cend(); ++it) {
        REQUIRE(*it == 'a');
      }
    };
    x.clear();
    x.append(w.cbegin(), w.begin() + 1000000);
    x.append(w.cbegin() + 19, w.begin() + 1000019);
    x.append(w.cbegin() + 119, w.begin() + 1000119);
    x.append(w.cbegin() + 1119, w.begin() + 1001119);
    x.append(w.cbegin() + 11119, w.begin() + 1011119);
    REQUIRE(x.size() == 0x4C4B40);
    BENCHMARK("MultiStringView 5 blocks") {
      for (auto it = x.cbegin(); it != x.cend(); ++it) {
        REQUIRE(*it == 'a');
      }
    };
    x.clear();
    for (size_t i = 0; i < 0x4C4B40 / 10; ++i) {
      x.append(w.cbegin(), w.cbegin() + 10);
    }
    BENCHMARK("MultiStringView 500000 blocks") {
      for (auto it = x.cbegin(); it != x.cend(); ++it) {
        REQUIRE(*it == 'a');
      }
    };
  }

  TEST_CASE("MultiStringView append", "[001][quick]") {
    std::string w(0x4C4B40, 'a');
    BENCHMARK("std::string::append 500k times 100 chars") {
      std::string x;
      for (size_t i = 0; i < 0x4C4B40 / 10; ++i) {
        x.append(w.cbegin(), w.cbegin() + 100);
      }
    };
    BENCHMARK("MultiStringView::append 500k times 100 chars") {
      detail::MultiStringView x;
      for (size_t i = 0; i < 0x4C4B40 / 10; ++i) {
        x.append(w.cbegin(), w.cbegin() + 100);
      }
    };
    BENCHMARK("std::string::append 5 million times 10 chars") {
      std::string x;
      for (size_t i = 0; i < 0x4C4B40; ++i) {
        x.append(w.cbegin(), w.cbegin() + 10);
      }
    };
    BENCHMARK("MultiStringView::append 5 million times 10 chars") {
      detail::MultiStringView x;
      for (size_t i = 0; i < 0x4C4B40; ++i) {
        x.append(w.cbegin(), w.cbegin() + 10);
      }
    };
  }

  TEST_CASE("MultiStringView constructor", "[002][quick]") {
    std::string w(1000, 'a');
    BENCHMARK("std::string::string 50k times 1000 chars") {
      for (size_t i = 0; i < 0x4C4B40 / 100; ++i) {
        std::string x(w.cbegin(), w.cbegin() + 1000);
        REQUIRE(x.size() == 1000);
      }
    };
    BENCHMARK("MultiStringView::MultiStringView 50k times 1000 chars") {
      for (size_t i = 0; i < 0x4C4B40 / 100; ++i) {
        detail::MultiStringView x(w.cbegin(), w.cbegin() + 1000);
        REQUIRE(x.size() == 1000);
      }
    };
    BENCHMARK("std::string::string 5 million times 10 chars") {
      for (size_t i = 0; i < 0x4C4B40; ++i) {
        std::string x(w.cbegin(), w.cbegin() + 10);
        REQUIRE(x.size() == 10);
      }
    };
    BENCHMARK("MultiStringView::MultiStringView 5 million times 10 chars") {
      for (size_t i = 0; i < 0x4C4B40; ++i) {
        detail::MultiStringView x(w.cbegin(), w.cbegin() + 10);
        REQUIRE(x.size() == 10);
      }
    };
  }

}  // namespace libsemigroups
