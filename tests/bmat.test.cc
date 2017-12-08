// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2017 Finn Smith
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

#include "../src/bmat.h"

#define BMAT_REPORT false

using namespace libsemigroups;

TEST_CASE("BMat 01: transpose", "[quick][bmat][01]") {
  BMat8 bm1(0);
  REQUIRE(bm1.transpose() == bm1);

  BMat8 bm2({{1, 1}, {0, 1}});
  REQUIRE(bm2.transpose() == BMat8({{1, 0}, {1, 1}}));

  BMat8 bm3({{0, 0, 0, 1, 0, 0, 1, 1},
             {1, 1, 1, 1, 1, 1, 0, 1},
             {0, 1, 1, 1, 0, 1, 0, 1},
             {1, 1, 0, 1, 1, 1, 1, 1},
             {0, 0, 1, 0, 0, 1, 1, 1},
             {1, 1, 0, 0, 0, 0, 0, 1},
             {0, 1, 0, 0, 0, 0, 1, 1},
             {0, 1, 1, 1, 1, 0, 1, 0}});

  REQUIRE(bm3.transpose() == BMat8({{0, 1, 0, 1, 0, 1, 0, 0},
                                    {0, 1, 1, 1, 0, 1, 1, 1},
                                    {0, 1, 1, 0, 1, 0, 0, 1},
                                    {1, 1, 1, 1, 0, 0, 0, 1},
                                    {0, 1, 0, 1, 0, 0, 0, 1},
                                    {0, 1, 1, 1, 1, 0, 0, 0},
                                    {1, 0, 0, 1, 1, 0, 1, 1},
                                    {1, 1, 1, 1, 1, 1, 1, 0}}));
}

TEST_CASE("BMat 02: multiplication", "[quick][bmat][02]") {
  BMat8 bm({{0, 0, 0, 1, 0, 0, 1, 1},
            {1, 1, 1, 1, 1, 1, 0, 1},
            {0, 1, 1, 1, 0, 1, 0, 1},
            {1, 1, 0, 1, 1, 1, 1, 1},
            {0, 0, 1, 0, 0, 1, 1, 1},
            {1, 1, 0, 0, 0, 0, 0, 1},
            {0, 1, 0, 0, 0, 0, 1, 1},
            {0, 1, 1, 1, 1, 0, 1, 0}});

  BMat8 tmp;

  tmp.redefine(bm, bm.one());
  REQUIRE(tmp == bm);
  REQUIRE(tmp == bm * bm.one());

  tmp.redefine(bm.one(), bm);
  REQUIRE(tmp == bm);
  REQUIRE(tmp == bm.one() * bm);

  tmp.redefine(bm, BMat8(0));
  REQUIRE(tmp == BMat8(0));

  BMat8 bm2({{0, 0, 0, 1, 0, 0, 1, 1},
             {0, 0, 1, 0, 0, 1, 0, 1},
             {1, 1, 0, 0, 1, 1, 0, 1},
             {1, 1, 0, 0, 0, 0, 0, 1},
             {0, 1, 0, 0, 0, 0, 1, 1},
             {0, 1, 0, 1, 1, 1, 1, 1},
             {0, 1, 0, 1, 0, 1, 0, 1},
             {0, 1, 0, 0, 0, 0, 1, 0}});

  tmp.redefine(bm, bm2);

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
}

TEST_CASE("BMat 03: row space basis", "[quick][bmat][03]") {
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

  REQUIRE(bm.row_space_basis() == bm2);

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

  REQUIRE(bm3.row_space_basis() == bm4);
}

TEST_CASE("BMat 04: identity matrix", "[quick][bmat][04]") {
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

  REQUIRE(bm.one() == id);
}

TEST_CASE("BMat 05: random", "[quick][bmat][05]") {
  for (size_t d = 1; d < 8; ++d) {
    BMat8 bm = BMat8::random(d);
    for (size_t i = d + 1; i < 8; ++i) {
      for (size_t j = 0; j < 8; ++j) {
        REQUIRE(bm(i, j) == 0);
        REQUIRE(bm(j, i) == 0);
      }
    }
  }
}

TEST_CASE("BMat 06: call operator", "[quick][bmat][06]") {
  std::vector<std::vector<size_t>> mat = {{0, 0, 0, 1, 0, 0, 1},
                                          {0, 1, 1, 1, 0, 1, 0},
                                          {1, 1, 0, 1, 1, 1, 1},
                                          {0, 0, 1, 0, 0, 1, 1},
                                          {1, 1, 0, 0, 0, 0, 0},
                                          {0, 1, 0, 0, 0, 0, 1},
                                          {0, 1, 1, 1, 1, 0, 1}};
  BMat8 bm(mat);

  for (size_t i = 0; i < 7; ++i) {
    for (size_t j = 0; j < 7; ++j) {
      REQUIRE(static_cast<size_t>(bm(i, j)) == mat[i][j]);
    }
  }
}

TEST_CASE("BMat 07: swap rows", "[quick][bmat][07]") {
  std::vector<std::vector<size_t>> mat = {{0, 0, 0, 1, 0, 0, 1},
                                          {0, 1, 1, 1, 0, 1, 0},
                                          {1, 1, 0, 1, 1, 1, 1},
                                          {0, 0, 1, 0, 0, 1, 1},
                                          {1, 1, 0, 0, 0, 0, 0},
                                          {0, 1, 0, 0, 0, 0, 1},
                                          {0, 1, 1, 1, 1, 0, 1}};
  for (size_t i = 0; i < 7; ++i) {
    for (size_t j = i + 1; j < 7; ++j) {
      {
        BMat8 bm(mat);
        bm.swap_rows(i, j);
        for (size_t k = 0; k < 7; ++k) {
          REQUIRE(bm(i, k) == mat[j][k]);
          REQUIRE(bm(j, k) == mat[i][k]);
        }
        bm.swap_rows(j, i);
        for (size_t k = 0; k < 7; ++k) {
          REQUIRE(bm(i, k) == mat[i][k]);
          REQUIRE(bm(j, k) == mat[j][k]);
        }
      }
    }
  }
}

TEST_CASE("BMat8 08: operator<<", "[quick][bmat][08]") {
  std::ostringstream os;
  os << BMat8::random();  // Does not do anything visible
}
