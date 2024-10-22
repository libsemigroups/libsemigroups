// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019-24 Finn Smith
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

#include <algorithm>      // for find
#include <cmath>          // for pow
#include <cstddef>        // for size_t
#include <cstdint>        // for uint8_t, uint64_t
#include <set>            // for set
#include <sstream>        // for basic_ostream, basic_ostr...
#include <unordered_set>  // for unordered_set
#include <utility>        // for pair
#include <vector>         // for vector

#include "catch_amalgamated.hpp"  // for REQUIRE, REQUIRE_THROWS_AS, REQUIRE_NOTHROW
#include "test-main.hpp"          // for LIBSEMIGROUPS_TEST_CASE_V3

#include "libsemigroups/bmat8.hpp"         // for BMat8, operator<<, col_sp...
#include "libsemigroups/exception.hpp"     // for LibsemigroupsException
#include "libsemigroups/froidure-pin.hpp"  // for FroidurePin

#include "libsemigroups/detail/report.hpp"  // for ReportGuard
#include "libsemigroups/detail/timer.hpp"   // for Timer

namespace libsemigroups {

  // Forward decl
  struct LibsemigroupsException;

  LIBSEMIGROUPS_TEST_CASE_V3("BMat8",
                             "000",
                             "transpose",
                             "[quick][no-valgrind]") {
    auto  rg = ReportGuard(false);
    BMat8 bm1(0);
    REQUIRE(bmat8::transpose(bm1) == bm1);

    BMat8 bm2({{1, 1}, {0, 1}});
    REQUIRE(bmat8::transpose(bm2) == BMat8({{1, 0}, {1, 1}}));

    BMat8 bm3({{0, 0, 0, 1, 0, 0, 1, 1},
               {1, 1, 1, 1, 1, 1, 0, 1},
               {0, 1, 1, 1, 0, 1, 0, 1},
               {1, 1, 0, 1, 1, 1, 1, 1},
               {0, 0, 1, 0, 0, 1, 1, 1},
               {1, 1, 0, 0, 0, 0, 0, 1},
               {0, 1, 0, 0, 0, 0, 1, 1},
               {0, 1, 1, 1, 1, 0, 1, 0}});

    REQUIRE(bmat8::transpose(bm3)
            == BMat8({{0, 1, 0, 1, 0, 1, 0, 0},
                      {0, 1, 1, 1, 0, 1, 1, 1},
                      {0, 1, 1, 0, 1, 0, 0, 1},
                      {1, 1, 1, 1, 0, 0, 0, 1},
                      {0, 1, 0, 1, 0, 0, 0, 1},
                      {0, 1, 1, 1, 1, 0, 0, 0},
                      {1, 0, 0, 1, 1, 0, 1, 1},
                      {1, 1, 1, 1, 1, 1, 1, 0}}));
  }

  LIBSEMIGROUPS_TEST_CASE_V3("BMat8", "001", "arithmetic", "[quick]") {
    auto  rg = ReportGuard(false);
    BMat8 bm({{0, 0, 0, 1, 0, 0, 1, 1},
              {1, 1, 1, 1, 1, 1, 0, 1},
              {0, 1, 1, 1, 0, 1, 0, 1},
              {1, 1, 0, 1, 1, 1, 1, 1},
              {0, 0, 1, 0, 0, 1, 1, 1},
              {1, 1, 0, 0, 0, 0, 0, 1},
              {0, 1, 0, 0, 0, 0, 1, 1},
              {0, 1, 1, 1, 1, 0, 1, 0}});

    BMat8 tmp = bm * bmat8::one();
    REQUIRE(tmp == bm);
    REQUIRE(tmp == bm * bmat8::one());

    tmp = bmat8::one() * bm;
    REQUIRE(tmp == bm);
    REQUIRE(tmp == bmat8::one() * bm);

    tmp = bm * BMat8(0);
    REQUIRE(tmp == BMat8(0));

    BMat8 bm2({{0, 0, 0, 1, 0, 0, 1, 1},
               {0, 0, 1, 0, 0, 1, 0, 1},
               {1, 1, 0, 0, 1, 1, 0, 1},
               {1, 1, 0, 0, 0, 0, 0, 1},
               {0, 1, 0, 0, 0, 0, 1, 1},
               {0, 1, 0, 1, 1, 1, 1, 1},
               {0, 1, 0, 1, 0, 1, 0, 1},
               {0, 1, 0, 0, 0, 0, 1, 0}});

    tmp = bm * bm2;

    BMat8 bm3({{1, 1, 0, 1, 0, 1, 1, 1},
               {1, 1, 1, 1, 1, 1, 1, 1},
               {1, 1, 1, 1, 1, 1, 1, 1},
               {1, 1, 1, 1, 1, 1, 1, 1},
               {1, 1, 0, 1, 1, 1, 1, 1},
               {0, 1, 1, 1, 0, 1, 1, 1},
               {0, 1, 1, 1, 0, 1, 1, 1},
               {1, 1, 1, 1, 1, 1, 1, 1}});

    REQUIRE(tmp == bm3);
    REQUIRE(tmp == bm * bm2);
    bm *= bm2;
    REQUIRE(tmp == bm);

    // Scalar mult.
    REQUIRE(0 * tmp == BMat8(0));
    REQUIRE(tmp * 0 == 0 * tmp);
    REQUIRE(1 * tmp == tmp);
    REQUIRE(tmp * 1 == 1 * tmp);

    tmp *= 1;
    REQUIRE(tmp == bm);
    tmp *= 0;
    REQUIRE(tmp == BMat8(0));

    // Addition
    bm  = BMat8({{1, 0, 1}, {1, 1, 0}, {0, 0, 0}});
    bm2 = BMat8({{0, 0, 0}, {0, 1, 0}, {1, 0, 0}});
    REQUIRE(bm + bm2 == BMat8({{1, 0, 1}, {1, 1, 0}, {1, 0, 0}}));
    REQUIRE(bm + bm2 == bm2 + bm);
    bm += bm2;
    REQUIRE(bm == BMat8({{1, 0, 1}, {1, 1, 0}, {1, 0, 0}}));
  }

  LIBSEMIGROUPS_TEST_CASE_V3("BMat8", "002", "identity matrix", "[quick]") {
    auto  rg = ReportGuard(false);
    BMat8 bm({{0, 1, 1, 1, 0, 1, 0, 1},
              {0, 0, 0, 0, 0, 0, 0, 1},
              {1, 1, 1, 1, 1, 1, 0, 1},
              {1, 1, 0, 1, 1, 1, 1, 1},
              {0, 0, 1, 0, 0, 1, 1, 1},
              {1, 1, 0, 0, 0, 0, 0, 1},
              {0, 1, 0, 0, 0, 0, 1, 1},
              {0, 1, 1, 1, 1, 0, 1, 0}});

    BMat8 id({{1, 0, 0, 0, 0, 0, 0, 0},
              {0, 1, 0, 0, 0, 0, 0, 0},
              {0, 0, 1, 0, 0, 0, 0, 0},
              {0, 0, 0, 1, 0, 0, 0, 0},
              {0, 0, 0, 0, 1, 0, 0, 0},
              {0, 0, 0, 0, 0, 1, 0, 0},
              {0, 0, 0, 0, 0, 0, 1, 0},
              {0, 0, 0, 0, 0, 0, 0, 1}});

    REQUIRE(bmat8::one() == id);
  }

  LIBSEMIGROUPS_TEST_CASE_V3("BMat8", "003", "random", "[quick]") {
    auto rg = ReportGuard(false);
    for (size_t d = 1; d < 9; ++d) {
      BMat8 const bm = bmat8::random(d);
      for (size_t i = d; i < 8; ++i) {
        for (size_t j = 0; j < 8; ++j) {
          REQUIRE(bm(i, j) == 0);
          REQUIRE(bm(j, i) == 0);
          REQUIRE(bm.at(i, j) == 0);
          REQUIRE(bm.at(j, i) == 0);
        }
      }
    }
    REQUIRE_THROWS_AS(bmat8::random(9), LibsemigroupsException);
    REQUIRE_THROWS_AS(bmat8::random(9), LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE_V3("BMat8", "004", "call operator", "[quick]") {
    auto                           rg  = ReportGuard(false);
    std::vector<std::vector<bool>> mat = {{0, 0, 0, 1, 0, 0, 1},
                                          {0, 1, 1, 1, 0, 1, 0},
                                          {1, 1, 0, 1, 1, 1, 1},
                                          {0, 0, 1, 0, 0, 1, 1},
                                          {1, 1, 0, 0, 0, 0, 0},
                                          {0, 1, 0, 0, 0, 0, 1},
                                          {0, 1, 1, 1, 1, 0, 1}};
    BMat8 const                    bm(mat);

    for (size_t i = 0; i < 7; ++i) {
      for (size_t j = 0; j < 7; ++j) {
        REQUIRE(bm(i, j) == mat[i][j]);
      }
    }
  }

  LIBSEMIGROUPS_TEST_CASE_V3("BMat8", "005", "operator<<", "[quick]") {
    auto               rg = ReportGuard(false);
    std::ostringstream oss;
    oss << bmat8::random();  // Does not do anything visible

    std::stringbuf buff;
    std::ostream   os(&buff);
    os << bmat8::random();  // Also does not do anything visible
  }

  LIBSEMIGROUPS_TEST_CASE_V3("BMat8", "006", "set", "[quick]") {
    auto  rg = ReportGuard(false);
    BMat8 bm({{0, 1, 1, 1, 0, 1, 0, 1},
              {0, 0, 0, 0, 0, 0, 0, 1},
              {1, 1, 1, 1, 1, 1, 0, 1},
              {1, 1, 0, 1, 1, 1, 1, 1},
              {0, 0, 1, 0, 0, 1, 1, 1},
              {1, 1, 0, 0, 0, 0, 0, 1},
              {0, 1, 0, 0, 0, 0, 1, 1},
              {0, 1, 1, 1, 1, 0, 1, 0}});

    BMat8 bm2({{1, 1, 1, 1, 0, 1, 0, 1},
               {0, 0, 0, 0, 0, 0, 0, 1},
               {1, 1, 1, 1, 1, 1, 0, 1},
               {1, 1, 0, 1, 1, 1, 1, 1},
               {0, 0, 1, 0, 0, 1, 1, 1},
               {1, 1, 0, 0, 0, 0, 0, 1},
               {0, 1, 0, 0, 0, 0, 1, 1},
               {0, 1, 1, 1, 1, 0, 1, 0}});

    BMat8 bm3({{1, 0, 1, 1, 0, 1, 0, 1},
               {0, 0, 0, 0, 0, 0, 0, 1},
               {1, 1, 1, 1, 1, 1, 0, 1},
               {1, 1, 0, 1, 1, 1, 1, 1},
               {0, 0, 1, 0, 0, 1, 1, 1},
               {1, 1, 0, 0, 0, 0, 0, 1},
               {0, 1, 0, 0, 0, 0, 1, 1},
               {0, 1, 1, 1, 1, 0, 1, 0}});

    BMat8 bm4({{1, 0, 1, 1, 0, 1, 0, 1},
               {0, 0, 0, 0, 0, 0, 0, 1},
               {1, 1, 1, 1, 1, 1, 0, 1},
               {1, 1, 0, 1, 1, 1, 1, 1},
               {0, 0, 1, 0, 0, 1, 1, 1},
               {1, 1, 0, 0, 0, 0, 1, 1},
               {0, 1, 0, 0, 0, 0, 1, 1},
               {0, 1, 1, 1, 1, 0, 1, 0}});

    BMat8 bm5({{1, 0, 1, 1, 0, 1, 0, 1},
               {0, 0, 0, 0, 0, 0, 0, 1},
               {1, 1, 1, 1, 1, 1, 0, 1},
               {1, 1, 0, 1, 1, 1, 1, 1},
               {0, 0, 1, 0, 0, 1, 1, 1},
               {1, 1, 0, 0, 0, 0, 1, 1},
               {0, 1, 0, 0, 0, 0, 1, 1},
               {0, 1, 1, 1, 1, 0, 1, 1}});

    bm(0, 0) = true;
    REQUIRE(bm == bm2);

    bm(0, 1) = false;
    REQUIRE(bm == bm3);

    bm(5, 6) = true;
    REQUIRE(bm == bm4);

    bm(7, 7) = true;
    REQUIRE(bm == bm5);

    for (size_t i = 0; i < 8; ++i) {
      for (size_t j = 0; j < 8; ++j) {
        bm(i, j) = true;
      }
    }

    BMat8 ones(0xffffffffffffffff);
    REQUIRE(bm == ones);

    for (size_t i = 0; i < 8; ++i) {
      for (size_t j = 0; j < 8; ++j) {
        bm(i, j) = false;
      }
    }

    BMat8 zeros(0);
    REQUIRE(bm == zeros);

    REQUIRE_THROWS_AS(zeros.at(0, 8) = true, LibsemigroupsException);
    REQUIRE_THROWS_AS(zeros.at(8, 0) = true, LibsemigroupsException);
    REQUIRE_THROWS_AS(zeros.at(8, 8) = true, LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE_V3("BMat8", "007", "row space basis", "[quick]") {
    auto  rg = ReportGuard(false);
    BMat8 bm({{0, 1, 1, 1, 0, 1, 0, 1},
              {0, 0, 0, 0, 0, 0, 0, 1},
              {1, 1, 1, 1, 1, 1, 0, 1},
              {1, 1, 0, 1, 1, 1, 1, 1},
              {0, 0, 1, 0, 0, 1, 1, 1},
              {1, 1, 0, 0, 0, 0, 0, 1},
              {0, 1, 0, 0, 0, 0, 1, 1},
              {0, 1, 1, 1, 1, 0, 1, 0}});

    BMat8 bm2({{1, 1, 1, 1, 1, 1, 0, 1},
               {1, 1, 0, 1, 1, 1, 1, 1},
               {1, 1, 0, 0, 0, 0, 0, 1},
               {0, 1, 1, 1, 1, 0, 1, 0},
               {0, 1, 1, 1, 0, 1, 0, 1},
               {0, 1, 0, 0, 0, 0, 1, 1},
               {0, 0, 1, 0, 0, 1, 1, 1},
               {0, 0, 0, 0, 0, 0, 0, 1}});

    REQUIRE(bmat8::row_space_basis(bm) == bmat8::row_space_basis(bm2));

    BMat8 bm3({{1, 1, 1, 1, 0, 1, 0, 1},
               {0, 1, 1, 1, 1, 1, 0, 1},
               {1, 1, 1, 1, 1, 1, 0, 1},
               {1, 1, 1, 1, 1, 1, 0, 1},
               {1, 1, 1, 0, 0, 1, 0, 1},
               {1, 1, 0, 0, 0, 1, 1, 1},
               {0, 1, 0, 0, 0, 0, 1, 1},
               {1, 0, 0, 0, 0, 1, 0, 0}});

    BMat8 bm4({{1, 1, 1, 1, 0, 1, 0, 1},
               {1, 1, 1, 0, 0, 1, 0, 1},
               {1, 0, 0, 0, 0, 1, 0, 0},
               {0, 1, 1, 1, 1, 1, 0, 1},
               {0, 1, 0, 0, 0, 0, 1, 1},
               {0, 0, 0, 0, 0, 0, 0, 0},
               {0, 0, 0, 0, 0, 0, 0, 0},
               {0, 0, 0, 0, 0, 0, 0, 0}});

    REQUIRE(bmat8::row_space_basis(bm3) == bm4);
    REQUIRE(bmat8::row_space_basis(bm4) == bm4);

    BMat8 bm5(0xff00000000000000);

    uint64_t data = 0xffffffffffffffff;

    for (size_t i = 0; i < 7; ++i) {
      REQUIRE(bmat8::row_space_basis(BMat8(data)) == bm5);
      data = data >> 8;
    }

    for (size_t i = 0; i < 1000; ++i) {
      bm = bmat8::random();
      REQUIRE(bmat8::row_space_basis(bmat8::row_space_basis(bm))
              == bmat8::row_space_basis(bm));
    }
  }

  LIBSEMIGROUPS_TEST_CASE_V3("BMat8", "008", "col space basis", "[quick]") {
    auto  rg = ReportGuard(false);
    BMat8 bm({{0, 1, 1, 1, 0, 1, 0, 1},
              {0, 0, 0, 0, 0, 0, 0, 1},
              {1, 1, 1, 1, 1, 1, 0, 1},
              {1, 1, 0, 1, 1, 1, 1, 1},
              {0, 0, 1, 0, 0, 1, 1, 1},
              {1, 1, 0, 0, 0, 0, 0, 1},
              {0, 1, 0, 0, 0, 0, 1, 1},
              {0, 1, 1, 1, 1, 0, 1, 0}});

    BMat8 bm2({{1, 1, 1, 1, 1, 0, 0, 0},
               {1, 0, 0, 0, 0, 0, 0, 0},
               {1, 1, 1, 1, 1, 1, 1, 0},
               {1, 1, 1, 1, 0, 1, 1, 1},
               {1, 1, 0, 0, 1, 0, 0, 1},
               {1, 0, 1, 0, 0, 1, 0, 0},
               {1, 0, 1, 0, 0, 0, 0, 1},
               {0, 0, 1, 1, 1, 0, 1, 1}});

    REQUIRE(bmat8::col_space_basis(bm) == bm2);

    BMat8 bm3({{1, 1, 1, 1, 0, 1, 0, 1},
               {0, 1, 1, 1, 1, 1, 0, 1},
               {1, 1, 1, 1, 1, 1, 0, 1},
               {1, 1, 1, 1, 1, 1, 0, 1},
               {1, 1, 1, 0, 0, 1, 0, 1},
               {1, 1, 0, 0, 0, 1, 1, 1},
               {0, 1, 0, 0, 0, 0, 1, 1},
               {1, 0, 0, 0, 0, 1, 0, 0}});

    BMat8 bm4({{1, 1, 1, 0, 0, 0, 0, 0},
               {1, 1, 0, 1, 0, 0, 0, 0},
               {1, 1, 1, 1, 0, 0, 0, 0},
               {1, 1, 1, 1, 0, 0, 0, 0},
               {1, 0, 1, 0, 0, 0, 0, 0},
               {0, 0, 1, 0, 1, 0, 0, 0},
               {0, 0, 0, 0, 1, 0, 0, 0},
               {0, 0, 1, 0, 0, 0, 0, 0}});

    REQUIRE(bmat8::col_space_basis(bm3) == bm4);

    uint64_t col = 0x8080808080808080;
    BMat8    bm5(col);

    uint64_t data = 0xffffffffffffffff;

    for (size_t i = 0; i < 7; ++i) {
      REQUIRE(bmat8::col_space_basis(BMat8(data)) == bm5);
      data &= ~(col >> i);
    }

    for (size_t i = 0; i < 1000; ++i) {
      bm = bmat8::random();
      REQUIRE(bmat8::col_space_basis(bmat8::col_space_basis(bm))
              == bmat8::col_space_basis(bm));
    }
  }

  LIBSEMIGROUPS_TEST_CASE_V3("BMat8", "009", "row space basis x 2", "[quick]") {
    auto                     rg = ReportGuard(false);
    detail::Timer            t;
    const std::vector<BMat8> gens
        = {BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
           BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
           BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
           BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {1, 0, 0, 1}}),
           BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}})};

    // int lg = 0;
    using std::unordered_set;
    unordered_set<BMat8> res;
    res.insert(bmat8::row_space_basis(bmat8::one()));

    std::vector<BMat8> todo, newtodo;
    todo.push_back(bmat8::row_space_basis(bmat8::one()));
    while (todo.size()) {
      newtodo.clear();
      // lg++;
      for (auto v : todo) {
        for (auto g : gens) {
          auto el = bmat8::row_space_basis(v * g);
          if (res.insert(el).second)
            newtodo.push_back(el);
        }
      }
      std::swap(todo, newtodo);
      // std::cout << lg << ", todo = " << todo.size() << ", res = " <<
      // res.size()
      //     << ", #Bucks = " << res.bucket_count() << std::endl;
    }
    // std::cout << "res =  " << res.size() << std::endl;
    // std::cout << t;
  }

  LIBSEMIGROUPS_TEST_CASE_V3("BMat8",
                             "010",
                             "number_of_rows, number_of_cols",
                             "[quick]") {
    auto  rg    = ReportGuard(false);
    BMat8 idem1 = bmat8::one();
    BMat8 idem2 = bmat8::one();
    BMat8 one   = bmat8::one();

    REQUIRE(bmat8::number_of_rows(one) == 8);
    REQUIRE(bmat8::number_of_cols(one) == 8);
    for (size_t i = 0; i < 7; ++i) {
      idem1(i, i)         = false;
      idem2(7 - i, 7 - i) = false;

      REQUIRE(bmat8::number_of_rows(idem1) == 7 - i);
      REQUIRE(bmat8::number_of_cols(idem1) == 7 - i);
      REQUIRE(bmat8::number_of_rows(idem2) == 7 - i);
      REQUIRE(bmat8::number_of_cols(idem2) == 7 - i);
    }

    const std::vector<BMat8> gens
        = {BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
           BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
           BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
           BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {1, 0, 0, 1}}),
           BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}})};

    FroidurePin<BMat8> S(gens);

    for (auto it = S.begin(); it < S.end(); it++) {
      REQUIRE(bmat8::number_of_rows(*it) <= 8);
      REQUIRE(bmat8::number_of_rows(*it) <= 8);

      REQUIRE(bmat8::number_of_rows(bmat8::row_space_basis(*it))
              <= bmat8::number_of_rows(*it));
      REQUIRE(bmat8::number_of_cols(bmat8::col_space_basis(*it))
              <= bmat8::number_of_cols(*it));
    }
  }

  LIBSEMIGROUPS_TEST_CASE_V3("BMat8",
                             "011",
                             "row_space, col_space",
                             "[quick]") {
    auto  rg    = ReportGuard(false);
    BMat8 idem1 = bmat8::one();
    BMat8 idem2 = bmat8::one();
    BMat8 one   = bmat8::one();

    REQUIRE(bmat8::row_space_size(one) == 256);
    REQUIRE(bmat8::col_space_size(one) == 256);
    for (size_t i = 0; i < 8; ++i) {
      idem1(7 - i, 7 - i) = false;
      idem2(i, i)         = false;

      REQUIRE(bmat8::row_space_size(idem1) == pow(2, 7 - i));
      REQUIRE(bmat8::col_space_size(idem1) == pow(2, 7 - i));
      REQUIRE(bmat8::row_space_size(idem2) == pow(2, 7 - i));
      REQUIRE(bmat8::col_space_size(idem2) == pow(2, 7 - i));
    }

    const std::vector<BMat8> gens
        = {BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
           BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
           BMat8({{0, 1, 0, 1}, {1, 0, 1, 0}, {1, 0, 1, 0}, {0, 0, 0, 1}}),
           BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
           BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}})};

    FroidurePin<BMat8> S(gens);

    for (auto it = S.begin(); it < S.end(); it++) {
      BMat8 x    = *it;
      BMat8 rows = bmat8::row_space_basis(x);
      BMat8 cols = bmat8::col_space_basis(x);
      REQUIRE(bmat8::row_space_size(x) <= 16);
      REQUIRE(bmat8::col_space_size(x) <= 16);

      REQUIRE(bmat8::row_space_size(rows)
              <= pow(2, bmat8::number_of_rows(rows)));
      REQUIRE(bmat8::col_space_size(cols)
              <= pow(2, bmat8::number_of_cols(cols)));

      for (auto it2 = S.begin(); it2 < S.end(); it2++) {
        BMat8 y = *it2;
        REQUIRE(bmat8::row_space_size(x * y) <= bmat8::row_space_size(x));
        REQUIRE(bmat8::col_space_size(x * y) <= bmat8::col_space_size(x));
      }
    }

    BMat8 bm1({{0, 0, 0, 1, 0, 0, 1, 1},
               {0, 0, 1, 0, 0, 1, 0, 1},
               {1, 1, 0, 0, 1, 1, 0, 1},
               {1, 1, 0, 0, 0, 0, 0, 1},
               {0, 1, 0, 0, 0, 0, 1, 1},
               {0, 1, 0, 1, 1, 1, 1, 1},
               {0, 1, 0, 1, 0, 1, 0, 1},
               {0, 1, 0, 0, 0, 0, 1, 0}});
    BMat8 bmm1({{1, 1, 0, 1, 0, 1, 1, 1},
                {1, 1, 1, 1, 1, 1, 1, 1},
                {1, 1, 1, 1, 1, 1, 1, 1},
                {1, 1, 1, 1, 1, 1, 1, 1},
                {1, 1, 0, 1, 1, 1, 1, 1},
                {0, 1, 1, 1, 0, 1, 1, 1},
                {0, 1, 1, 1, 0, 1, 1, 1},
                {1, 1, 1, 1, 1, 1, 1, 1}});
    BMat8 bm2({{1, 1}, {0, 1}});
    BMat8 bm2t({{1, 0}, {1, 1}});
    BMat8 bm3({{0, 0, 0, 1, 0, 0, 1, 1},
               {1, 1, 1, 1, 1, 1, 0, 1},
               {0, 1, 1, 1, 1, 1, 0, 1},
               {1, 1, 0, 1, 1, 1, 1, 1},
               {0, 0, 1, 0, 0, 1, 1, 1},
               {1, 1, 0, 0, 0, 0, 0, 1},
               {0, 1, 0, 0, 0, 0, 1, 1},
               {0, 1, 1, 1, 1, 0, 1, 0}});
    BMat8 bm3t({{0, 1, 0, 1, 0, 1, 0, 0},
                {0, 1, 1, 1, 0, 1, 1, 1},
                {0, 1, 1, 0, 1, 0, 0, 1},
                {1, 1, 1, 1, 0, 0, 0, 1},
                {0, 1, 1, 1, 0, 0, 0, 1},
                {0, 1, 1, 1, 1, 0, 0, 0},
                {1, 0, 0, 1, 1, 0, 1, 1},
                {1, 1, 1, 1, 1, 1, 1, 0}});
    BMat8 bm({{0, 0, 0, 1, 0, 0, 1, 1},
              {1, 1, 1, 1, 1, 1, 0, 1},
              {0, 1, 1, 1, 0, 1, 0, 1},
              {1, 1, 0, 1, 1, 1, 1, 1},
              {0, 0, 1, 0, 0, 1, 1, 1},
              {1, 1, 0, 0, 0, 0, 0, 1},
              {0, 1, 0, 0, 0, 0, 1, 1},
              {0, 1, 1, 1, 1, 0, 1, 0}});

    REQUIRE(22 == bmat8::row_space_size(bm));
    REQUIRE(31 == bmat8::row_space_size(bm1));
    REQUIRE(6 == bmat8::row_space_size(bmm1));
    REQUIRE(3 == bmat8::row_space_size(bm2));
    REQUIRE(3 == bmat8::row_space_size(bm2t));
    REQUIRE(21 == bmat8::row_space_size(bm3));
    REQUIRE(21 == bmat8::row_space_size(bm3t));
  }

  LIBSEMIGROUPS_TEST_CASE_V3("BMat8", "012", "rows", "[quick]") {
    auto  rg = ReportGuard(false);
    BMat8 x({{0, 1}, {1, 0}});
    REQUIRE(x.to_int() == 4647714815446351872);
    REQUIRE(bmat8::to_vector(x(0))
            == std::vector<bool>({0, 1, 0, 0, 0, 0, 0, 0}));
    REQUIRE(bmat8::to_vector(x(1))
            == std::vector<bool>({1, 0, 0, 0, 0, 0, 0, 0}));

    BMat8 idem = bmat8::one();
    BMat8 one  = bmat8::one();

    std::vector<uint8_t> rows = bmat8::rows(one);
    REQUIRE(rows[0] == one(0));
    REQUIRE(bmat8::to_vector(rows[0])
            == std::vector<bool>({1, 0, 0, 0, 0, 0, 0, 0}));
    REQUIRE(bmat8::to_vector(rows[1])
            == std::vector<bool>({0, 1, 0, 0, 0, 0, 0, 0}));
    REQUIRE(bmat8::to_vector(rows[2])
            == std::vector<bool>({0, 0, 1, 0, 0, 0, 0, 0}));
    REQUIRE(bmat8::to_vector(rows[3])
            == std::vector<bool>({0, 0, 0, 1, 0, 0, 0, 0}));
    REQUIRE(bmat8::to_vector(rows[4])
            == std::vector<bool>({0, 0, 0, 0, 1, 0, 0, 0}));
    REQUIRE(bmat8::to_vector(rows[5])
            == std::vector<bool>({0, 0, 0, 0, 0, 1, 0, 0}));
    REQUIRE(bmat8::to_vector(rows[6])
            == std::vector<bool>({0, 0, 0, 0, 0, 0, 1, 0}));
    REQUIRE(bmat8::to_vector(rows[7])
            == std::vector<bool>({0, 0, 0, 0, 0, 0, 0, 1}));
    REQUIRE(one(0) == 128);
    REQUIRE(one(1) == 64);
    REQUIRE(one(2) == 32);
    REQUIRE(one(3) == 16);
    REQUIRE(one(4) == 8);
    REQUIRE(one(5) == 4);
    REQUIRE(one(6) == 2);
    REQUIRE(one(7) == 1);
    REQUIRE(one.at(0) == 128);
    REQUIRE(one.at(1) == 64);
    REQUIRE(one.at(2) == 32);
    REQUIRE(one.at(3) == 16);
    REQUIRE(one.at(4) == 8);
    REQUIRE(one.at(5) == 4);
    REQUIRE(one.at(6) == 2);
    REQUIRE(one.at(7) == 1);
    REQUIRE_THROWS_AS(one.at(8), LibsemigroupsException);
    REQUIRE(std::set<uint8_t>(rows.begin(), rows.end()).size() == 8);
    for (size_t i = 0; i < 8; ++i) {
      idem(7 - i, 7 - i) = false;
      rows.clear();
      bmat8::push_back_rows(rows, idem);
      REQUIRE(std::set<uint8_t>(rows.begin(), rows.end()).size() == 8 - i);
    }

    const std::vector<BMat8> gens
        = {BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
           BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
           BMat8({{0, 1, 0, 1}, {1, 0, 1, 0}, {1, 0, 1, 0}, {0, 0, 0, 1}}),
           BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
           BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}})};

    FroidurePin<BMat8>   S(gens);
    std::vector<uint8_t> basis_rows;

    for (auto it = S.begin(); it < S.end(); it++) {
      BMat8 x = *it;
      rows.clear();
      bmat8::push_back_rows(rows, x);
      basis_rows.clear();
      bmat8::push_back_rows(basis_rows, bmat8::row_space_basis(x));
      for (uint8_t row : basis_rows) {
        REQUIRE((row == 0
                 || std::find(rows.begin(), rows.end(), row) != rows.end()));
      }
    }
  }

  LIBSEMIGROUPS_TEST_CASE_V3("BMat8", "013", "one", "[quick]") {
    auto rg = ReportGuard(false);
    for (size_t i = 1; i <= 8; ++i) {
      BMat8 x = bmat8::one<BMat8>(i);
      REQUIRE(x * x == x);
      REQUIRE(bmat8::minimum_dim(x) == i);
    }
    REQUIRE(bmat8::minimum_dim(BMat8(0)) == 0);
  }

  LIBSEMIGROUPS_TEST_CASE_V3("BMat8", "014", "vector constructor", "[quick]") {
    auto  rg = ReportGuard(false);
    BMat8 zero(0);
    REQUIRE(BMat8({{0, 0}, {0, 0}}) == zero);
    REQUIRE(BMat8({{0, 0}, {0, 1}}) != zero);
    REQUIRE(BMat8({{0, 0}, {0, 1}}) == BMat8(static_cast<uint64_t>(1) << 54));

    REQUIRE_THROWS_AS(BMat8({{0, 0}}), LibsemigroupsException);
    REQUIRE_THROWS_AS(BMat8({{0, 1}}), LibsemigroupsException);
    REQUIRE_THROWS_AS(BMat8({{0}, {0}}), LibsemigroupsException);
    REQUIRE_THROWS_AS(BMat8({{1, 0}, {0}}), LibsemigroupsException);
    REQUIRE_THROWS_AS(BMat8({{0, 0, 0, 1, 0, 0, 1, 1, 0},
                             {1, 1, 1, 1, 1, 1, 0, 1, 0},
                             {0, 1, 1, 1, 0, 1, 0, 1, 0},
                             {1, 1, 0, 1, 1, 1, 1, 1, 0},
                             {0, 0, 1, 0, 0, 1, 1, 1, 0},
                             {1, 1, 0, 0, 0, 0, 0, 1, 0},
                             {1, 1, 0, 0, 0, 0, 0, 1, 0},
                             {0, 1, 0, 0, 0, 0, 1, 1, 0},
                             {0, 1, 1, 1, 1, 0, 1, 0, 0}}),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(BMat8({{0, 0, 0, 1, 0, 0, 1, 1, 0},
                             {1, 1, 1, 1, 1, 1, 0, 1, 0},
                             {0, 1, 1, 1, 0, 1, 0, 1, 0},
                             {1, 1, 0, 1, 1, 1, 1, 1, 0},
                             {0, 0, 1, 0, 0, 1, 1, 1, 0},
                             {1, 1, 0, 0, 0, 0, 0, 1, 0},
                             {0, 1, 0, 0, 0, 0, 1, 1, 0},
                             {0, 1, 1, 1, 1, 0, 1, 0, 0}}),
                      LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE_V3("BMat8",
                             "015",
                             "comparison operators",
                             "[quick]") {
    auto  rg = ReportGuard(false);
    BMat8 bm1(0);
    BMat8 bm2({{0, 0, 0, 1, 0, 0, 1, 1},
               {1, 1, 1, 1, 1, 1, 0, 1},
               {0, 1, 1, 1, 0, 1, 0, 1},
               {1, 1, 0, 1, 1, 1, 1, 1},
               {0, 0, 1, 0, 0, 1, 1, 1},
               {1, 1, 0, 0, 0, 0, 0, 1},
               {0, 1, 0, 0, 0, 0, 1, 1},
               {0, 1, 1, 1, 1, 0, 1, 0}});
    BMat8 bm3({{1, 1}, {0, 1}});
    REQUIRE(!(bm1 < bm1));
    REQUIRE(!(bm2 < bm2));
    REQUIRE(!(bm3 < bm3));
    REQUIRE(bm2 > bm1);
    REQUIRE(bm3 > bm2);
    REQUIRE(bm3 > bm1);
    REQUIRE(bm1 < bm2);
    REQUIRE(bm2 < bm3);
    REQUIRE(bm1 < bm3);
    REQUIRE(!(bm2 < bm1));
    REQUIRE(!(bm3 < bm2));
    REQUIRE(!(bm3 < bm1));
    REQUIRE(bm1 <= bm2);
    REQUIRE(bm1 <= bm1);
    REQUIRE(bm2 >= bm1);
    REQUIRE(bm1 >= bm1);
    REQUIRE(bm1 == bm1);
    REQUIRE(bm1 != bm2);
  }

  LIBSEMIGROUPS_TEST_CASE_V3("BMat8", "016", "adapters", "[quick]") {
    auto  rg = ReportGuard(false);
    BMat8 bm1(0);
    REQUIRE(Complexity<BMat8>()(bm1) == 0);
    REQUIRE(Degree<BMat8>()(bm1) == 8);
    REQUIRE_NOTHROW(IncreaseDegree<BMat8>()(bm1, 0));
    REQUIRE(One<BMat8>()(bm1) == bmat8::one());
    REQUIRE(One<BMat8>()(4) == bmat8::one<BMat8>(4));

    BMat8 bm2 = bmat8::random();
    BMat8 bm3, bm4;
    Product<BMat8>()(bm3, bm2, bmat8::one());
    REQUIRE(bm3 == bm2);
    Product<BMat8>()(bm3, bmat8::one(), bm2);
    REQUIRE(bm3 == bm2);
    Product<BMat8>()(bm3, bm2, bm2);
    REQUIRE(bm3 == bm2 * bm2);

    ImageRightAction<BMat8, BMat8>()(bm3, bm2, bmat8::one());
    REQUIRE(bm3 == bmat8::row_space_basis(bm2));

    ImageRightAction<BMat8, BMat8>()(bm3, bmat8::one(), bm2);
    REQUIRE(bm3 == bmat8::row_space_basis(bm2));

    ImageLeftAction<BMat8, BMat8>()(bm3, bm2, bmat8::one());
    REQUIRE(bm3 == bmat8::col_space_basis(bm2));

    ImageLeftAction<BMat8, BMat8>()(bm3, bmat8::one(), bm2);
    REQUIRE(bm3 == bmat8::col_space_basis(bm2));

    ImageRightAction<BMat8, BMat8>()(bm3, bm2, bm2);
    ImageLeftAction<BMat8, BMat8>()(
        bm4, bmat8::transpose(bm2), bmat8::transpose(bm2));
    REQUIRE(bm3 == bmat8::transpose(bm4));

    REQUIRE(Inverse<BMat8>()(bmat8::one()) == bmat8::one());
    BMat8 bm5({{0, 1, 0, 0, 0, 0, 0, 0},
               {1, 0, 0, 0, 0, 0, 0, 0},
               {0, 0, 0, 1, 0, 0, 0, 0},
               {0, 0, 1, 0, 0, 0, 0, 0},
               {0, 0, 0, 0, 1, 0, 0, 0},
               {0, 0, 0, 0, 0, 1, 0, 0},
               {0, 0, 0, 0, 0, 0, 1, 0},
               {0, 0, 0, 0, 0, 0, 0, 1}});
    REQUIRE(Inverse<BMat8>()(bm5) == bm5);
    BMat8 bm6({{0, 1, 0, 0, 0, 0, 0, 0},
               {0, 0, 1, 0, 0, 0, 0, 0},
               {1, 0, 0, 0, 0, 0, 0, 0},
               {0, 0, 0, 1, 0, 0, 0, 0},
               {0, 0, 0, 0, 1, 0, 0, 0},
               {0, 0, 0, 0, 0, 1, 0, 0},
               {0, 0, 0, 0, 0, 0, 1, 0},
               {0, 0, 0, 0, 0, 0, 0, 1}});
    REQUIRE(Inverse<BMat8>()(bm6) == bm6 * bm6);
  }

  LIBSEMIGROUPS_TEST_CASE_V3("BMat8", "017", "one x 2", "[quick]") {
    BMat8 bm5({{1, 0, 0, 0, 0, 0, 0, 0},
               {0, 1, 0, 0, 0, 0, 0, 0},
               {0, 0, 1, 0, 0, 0, 0, 0},
               {0, 0, 0, 1, 0, 0, 0, 0},
               {0, 0, 0, 0, 1, 0, 0, 0},
               {0, 0, 0, 0, 0, 0, 0, 0},
               {0, 0, 0, 0, 0, 0, 0, 0},
               {0, 0, 0, 0, 0, 0, 0, 0}});
    REQUIRE(bmat8::one(5) == bm5);
    REQUIRE(bmat8::one(0) == BMat8(0));
    REQUIRE(bmat8::one(8) == bmat8::one());
  }

  LIBSEMIGROUPS_TEST_CASE_V3("BMat8", "018", "is_regular_element", "[quick]") {
    REQUIRE((rx::seq<uint64_t>() | rx::take(100'000) | rx::filter([](auto val) {
               return bmat8::is_regular_element(BMat8(val));
             })
             | rx::count())
            == 97'996);
  }

  LIBSEMIGROUPS_TEST_CASE_V3("BMat8", "019", "at", "[quick]") {
    auto x = bmat8::random();
    REQUIRE_THROWS_AS(x.at(0, 8), LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE_V3("BMat8", "020", "to_string", "[quick]") {
    REQUIRE(to_string(bmat8::one(5)) == R"V0G0N(BMat8({{1, 0, 0, 0, 0},
       {0, 1, 0, 0, 0},
       {0, 0, 1, 0, 0},
       {0, 0, 0, 1, 0},
       {0, 0, 0, 0, 1}}))V0G0N");
    REQUIRE(to_string(BMat8(0)) == "BMat8(0)");
    REQUIRE(to_string(bmat8::one(5), "[]") == R"V0G0N(BMat8([[1, 0, 0, 0, 0],
       [0, 1, 0, 0, 0],
       [0, 0, 1, 0, 0],
       [0, 0, 0, 1, 0],
       [0, 0, 0, 0, 1]]))V0G0N");
    REQUIRE(to_string(BMat8(0), "[]") == "BMat8(0)");
    REQUIRE_THROWS_AS(to_string(BMat8(0), ""), LibsemigroupsException);
  }

}  // namespace libsemigroups
