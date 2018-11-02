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

#include <iosfwd>         // for ostream, ostringstream, stringbuf
#include <set>            // for set
#include <unordered_set>  // for unordered_set
#include <vector>         // for vector

#include "bmat8.hpp"
#include "froidure-pin.hpp"
#include "test-main.hpp"
#include "timer.hpp"

namespace libsemigroups {

  LIBSEMIGROUPS_TEST_CASE("BMat8", "001", "transpose", "[quick]") {
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

    REQUIRE(bm3.transpose()
            == BMat8({{0, 1, 0, 1, 0, 1, 0, 0},
                      {0, 1, 1, 1, 0, 1, 1, 1},
                      {0, 1, 1, 0, 1, 0, 0, 1},
                      {1, 1, 1, 1, 0, 0, 0, 1},
                      {0, 1, 0, 1, 0, 0, 0, 1},
                      {0, 1, 1, 1, 1, 0, 0, 0},
                      {1, 0, 0, 1, 1, 0, 1, 1},
                      {1, 1, 1, 1, 1, 1, 1, 0}}));
  }

  LIBSEMIGROUPS_TEST_CASE("BMat8", "002", "multiplication", "[quick]") {
    BMat8 bm({{0, 0, 0, 1, 0, 0, 1, 1},
              {1, 1, 1, 1, 1, 1, 0, 1},
              {0, 1, 1, 1, 0, 1, 0, 1},
              {1, 1, 0, 1, 1, 1, 1, 1},
              {0, 0, 1, 0, 0, 1, 1, 1},
              {1, 1, 0, 0, 0, 0, 0, 1},
              {0, 1, 0, 0, 0, 0, 1, 1},
              {0, 1, 1, 1, 1, 0, 1, 0}});

    BMat8 tmp = bm * bm.one();
    REQUIRE(tmp == bm);
    REQUIRE(tmp == bm * bm.one());

    tmp = bm.one() * bm;
    REQUIRE(tmp == bm);
    REQUIRE(tmp == bm.one() * bm);

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
  }

  LIBSEMIGROUPS_TEST_CASE("BMat8", "003", "identity matrix", "[quick]") {
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

  LIBSEMIGROUPS_TEST_CASE("BMat8", "004", "random", "[quick]") {
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

  LIBSEMIGROUPS_TEST_CASE("BMat8", "005", "call operator", "[quick]") {
    std::vector<std::vector<bool>> mat = {{0, 0, 0, 1, 0, 0, 1},
                                          {0, 1, 1, 1, 0, 1, 0},
                                          {1, 1, 0, 1, 1, 1, 1},
                                          {0, 0, 1, 0, 0, 1, 1},
                                          {1, 1, 0, 0, 0, 0, 0},
                                          {0, 1, 0, 0, 0, 0, 1},
                                          {0, 1, 1, 1, 1, 0, 1}};
    BMat8                          bm(mat);

    for (size_t i = 0; i < 7; ++i) {
      for (size_t j = 0; j < 7; ++j) {
        REQUIRE(static_cast<size_t>(bm(i, j)) == mat[i][j]);
      }
    }
  }

  LIBSEMIGROUPS_TEST_CASE("BMat8", "006", "operator<<", "[quick]") {
    std::ostringstream oss;
    oss << BMat8::random();  // Does not do anything visible

    std::stringbuf buff;
    std::ostream   os(&buff);
    os << BMat8::random();  // Also does not do anything visible
  }

  LIBSEMIGROUPS_TEST_CASE("BMat8", "007", "set", "[quick]") {
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

    bm.set(0, 0, 1);
    REQUIRE(bm == bm2);

    bm.set(0, 1, false);
    REQUIRE(bm == bm3);

    bm.set(5, 6, true);
    REQUIRE(bm == bm4);

    bm.set(7, 7, true);
    REQUIRE(bm == bm5);

    for (size_t i = 0; i < 8; ++i) {
      for (size_t j = 0; j < 8; ++j) {
        bm.set(i, j, true);
      }
    }

    BMat8 ones(0xffffffffffffffff);
    REQUIRE(bm == ones);

    for (size_t i = 0; i < 8; ++i) {
      for (size_t j = 0; j < 8; ++j) {
        bm.set(i, j, false);
      }
    }

    BMat8 zeros(0);
    REQUIRE(bm == zeros);
  }
  LIBSEMIGROUPS_TEST_CASE("BMat8", "008", "row space basis", "[quick]") {
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

    REQUIRE(bm.row_space_basis() == bm2.row_space_basis());

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
    REQUIRE(bm4.row_space_basis() == bm4);

    BMat8 bm5(0xff00000000000000);

    uint64_t data = 0xffffffffffffffff;

    for (size_t i = 0; i < 7; ++i) {
      REQUIRE(BMat8(data).row_space_basis() == bm5);
      data = data >> 8;
    }

    for (size_t i = 0; i < 1000; ++i) {
      bm = BMat8::random();
      REQUIRE(bm.row_space_basis().row_space_basis() == bm.row_space_basis());
    }
  }

  LIBSEMIGROUPS_TEST_CASE("BMat8", "009", "col space basis", "[quick]") {
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

    REQUIRE(bm.col_space_basis() == bm2);

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

    REQUIRE(bm3.col_space_basis() == bm4);

    uint64_t col = 0x8080808080808080;
    BMat8    bm5(col);

    uint64_t data = 0xffffffffffffffff;

    for (size_t i = 0; i < 7; ++i) {
      REQUIRE(BMat8(data).col_space_basis() == bm5);
      data &= ~(col >> i);
    }

    for (size_t i = 0; i < 1000; ++i) {
      bm = BMat8::random();
      REQUIRE(bm.col_space_basis().col_space_basis() == bm.col_space_basis());
    }
  }

  LIBSEMIGROUPS_TEST_CASE("BMat8", "010", "row space basis", "[quick]") {
    internal::Timer          t;
    const std::vector<BMat8> gens
        = {BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
           BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
           BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
           BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {1, 0, 0, 1}}),
           BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}})};

    int lg = 0;
    using std::unordered_set;
    unordered_set<BMat8> res;
    res.insert(BMat8::one().row_space_basis());

    std::vector<BMat8> todo, newtodo;
    todo.push_back(BMat8::one().row_space_basis());
    while (todo.size()) {
      newtodo.clear();
      lg++;
      for (auto v : todo) {
        for (auto g : gens) {
          auto el = (v * g).row_space_basis();
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

  LIBSEMIGROUPS_TEST_CASE("BMat8", "011", "is_group_index", "[quick]") {
    BMat8 idem = BMat8::one();
    BMat8 one  = BMat8::one();
    BMat8 zero = BMat8(0);

    REQUIRE(BMat8::is_group_index(one, one));
    for (size_t i = 0; i < 7; ++i) {
      idem.set(7 - i, 7 - i, false);
      REQUIRE(BMat8::is_group_index(idem, idem));

      REQUIRE(!BMat8::is_group_index(idem, one));
      REQUIRE(!BMat8::is_group_index(idem, zero));
    }
    REQUIRE(BMat8::is_group_index(zero, zero));
    REQUIRE(!BMat8::is_group_index(one, zero));

    const std::vector<BMat8> gens
        = {BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
           BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
           BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
           BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}})};

    FroidurePin<BMat8> S(gens);
    REQUIRE(S.size() == 209);
    REQUIRE(S.nr_idempotents() == 16);

    std::vector<std::vector<BMat8>> group_indices;
    for (auto it = S.begin(); it < S.end(); it++) {
      for (auto it2 = S.begin(); it2 < S.end(); it2++) {
        std::vector<BMat8> vec = std::vector<BMat8>(
            {(*it).col_space_basis(), (*it2).row_space_basis()});
        if (vec[0].col_space_basis() == vec[0]
            && vec[1].row_space_basis() == vec[1]
            && BMat8::is_group_index(vec[0], vec[1])) {
          if (std::find(group_indices.begin(), group_indices.end(), vec)
              == group_indices.end()) {
            group_indices.push_back(vec);
          }
        }
      }
    }
    REQUIRE(group_indices.size() == 16);
  }

  LIBSEMIGROUPS_TEST_CASE("BMat8", "012", "nr_rows, nr_cols", "[quick]") {
    BMat8 idem1 = BMat8::one();
    BMat8 idem2 = BMat8::one();
    BMat8 one   = BMat8::one();
    BMat8 zero  = BMat8(0);

    REQUIRE(one.nr_rows() == 8);
    REQUIRE(one.nr_cols() == 8);
    for (size_t i = 0; i < 7; ++i) {
      idem1.set(i, i, false);
      idem2.set(7 - i, 7 - i, false);

      REQUIRE(idem1.nr_rows() == 7 - i);
      REQUIRE(idem1.nr_cols() == 7 - i);
      REQUIRE(idem2.nr_rows() == 7 - i);
      REQUIRE(idem2.nr_cols() == 7 - i);
    }

    const std::vector<BMat8> gens
        = {BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
           BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
           BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
           BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {1, 0, 0, 1}}),
           BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}})};

    FroidurePin<BMat8> S(gens);

    for (auto it = S.begin(); it < S.end(); it++) {
      REQUIRE((*it).nr_rows() <= 8);
      REQUIRE((*it).nr_rows() <= 8);

      REQUIRE((*it).row_space_basis().nr_rows() <= (*it).nr_rows());
      REQUIRE((*it).col_space_basis().nr_cols() <= (*it).nr_cols());
    }
  }

  LIBSEMIGROUPS_TEST_CASE("BMat8", "013", "row_space, col_space", "[quick]") {
    BMat8 idem1 = BMat8::one();
    BMat8 idem2 = BMat8::one();
    BMat8 one   = BMat8::one();
    BMat8 zero  = BMat8(0);

    REQUIRE(one.row_space_size() == 256);
    REQUIRE(one.col_space_size() == 256);
    for (size_t i = 0; i < 8; ++i) {
      idem1.set(7 - i, 7 - i, false);
      idem2.set(i, i, false);

      REQUIRE(idem1.row_space_size() == pow(2, 7 - i));
      REQUIRE(idem1.col_space_size() == pow(2, 7 - i));
      REQUIRE(idem2.row_space_size() == pow(2, 7 - i));
      REQUIRE(idem2.col_space_size() == pow(2, 7 - i));
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
      BMat8 rows = x.row_space_basis();
      BMat8 cols = x.col_space_basis();
      REQUIRE(x.row_space_size() <= 16);
      REQUIRE(x.col_space_size() <= 16);

      REQUIRE(rows.row_space_size() <= pow(2, rows.nr_rows()));
      REQUIRE(cols.col_space_size() <= pow(2, cols.nr_cols()));

      for (auto it2 = S.begin(); it2 < S.end(); it2++) {
        BMat8 y = *it2;
        REQUIRE((x * y).row_space_size() <= x.row_space_size());
        REQUIRE((x * y).col_space_size() <= x.col_space_size());
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

    REQUIRE(22 == bm.row_space_size());
    REQUIRE(31 == bm1.row_space_size());
    REQUIRE(6 == bmm1.row_space_size());
    REQUIRE(3 == bm2.row_space_size());
    REQUIRE(3 == bm2t.row_space_size());
    REQUIRE(21 == bm3.row_space_size());
    REQUIRE(21 == bm3t.row_space_size());
  }

  LIBSEMIGROUPS_TEST_CASE("BMat8", "014", "rows", "[quick]") {
    BMat8 idem = BMat8::one();
    BMat8 one  = BMat8::one();
    BMat8 zero = BMat8(0);

    std::vector<uint8_t> rows = one.rows();
    REQUIRE(std::set<uint8_t>(rows.begin(), rows.end()).size() == 8);
    for (size_t i = 0; i < 8; ++i) {
      idem.set(7 - i, 7 - i, false);
      rows = idem.rows();
      REQUIRE(std::set<uint8_t>(rows.begin(), rows.end()).size() == 8 - i);
    }

    const std::vector<BMat8> gens
        = {BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
           BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
           BMat8({{0, 1, 0, 1}, {1, 0, 1, 0}, {1, 0, 1, 0}, {0, 0, 0, 1}}),
           BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
           BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}})};

    FroidurePin<BMat8> S(gens);

    for (auto it = S.begin(); it < S.end(); it++) {
      BMat8                x          = *it;
      rows       = x.rows();
      std::vector<uint8_t> basis_rows = x.row_space_basis().rows();
      for (uint8_t row : basis_rows) {
        REQUIRE((row == 0
                 || std::find(rows.begin(), rows.end(), row) != rows.end()));
      }
    }
  }

  // FIXME this test currently fails
  // LIBSEMIGROUPS_TEST_CASE("BMat8", "015", "one", "[quick]") {
  //  BMat8 one = BMat8::one();
  //  for (size_t i = 1; i < 8; ++i) {
  //    BMat8 x = BMat8::one(i);
  //    REQUIRE(x * one == x);
  //    REQUIRE(one * x == x);
  //    REQUIRE(x * x == x);
  //    REQUIRE(x.min_possible_dim() == i);
  //  }
  //}

  /*
  LIBSEMIGROUPS_TEST_CASE("BMat8", "015", "count row space sizes", "[extreme]") {
    const std::vector<BMat8> bmat4_gens
        = {BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
           BMat8({{1, 1, 1, 0}, {1, 0, 0, 1}, {0, 1, 0, 1}, {0, 0, 1, 1}}),
           BMat8({{1, 1, 0, 0}, {1, 0, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 1}}),
           BMat8({{1, 1, 0, 0}, {1, 0, 1, 0}, {0, 1, 0, 1}, {0, 0, 1, 1}}),
           BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {1, 0, 0, 1}}),
           BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}}),
           BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
           BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}})};

    const std::vector<BMat8> bmat5_gens = {BMat8({{1, 0, 0, 0, 0},
                                                  {0, 1, 0, 0, 0},
                                                  {0, 0, 1, 0, 0},
                                                  {0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 1}}),
                                           BMat8({{0, 1, 0, 0, 0},
                                                  {0, 0, 1, 0, 0},
                                                  {0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 1},
                                                  {1, 0, 0, 0, 0}}),
                                           BMat8({{0, 1, 0, 0, 0},
                                                  {1, 0, 0, 0, 0},
                                                  {0, 0, 1, 0, 0},
                                                  {0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 1}}),
                                           BMat8({{1, 0, 0, 0, 0},
                                                  {0, 1, 0, 0, 0},
                                                  {0, 0, 1, 0, 0},
                                                  {0, 0, 0, 1, 0},
                                                  {1, 0, 0, 0, 1}}),
                                           BMat8({{1, 1, 0, 0, 0},
                                                  {1, 0, 1, 0, 0},
                                                  {0, 1, 0, 1, 0},
                                                  {0, 0, 1, 1, 0},
                                                  {0, 0, 0, 0, 1}}),
                                           BMat8({{1, 1, 0, 0, 0},
                                                  {1, 0, 1, 0, 0},
                                                  {0, 1, 1, 0, 0},
                                                  {0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 1}}),
                                           BMat8({{1, 1, 1, 0, 0},
                                                  {1, 0, 0, 1, 0},
                                                  {0, 1, 0, 1, 0},
                                                  {0, 0, 1, 1, 0},
                                                  {0, 0, 0, 0, 1}}),
                                           BMat8({{1, 1, 0, 0, 0},
                                                  {1, 0, 1, 0, 0},
                                                  {0, 1, 0, 1, 0},
                                                  {0, 0, 1, 0, 1},
                                                  {0, 0, 0, 1, 1}}),
                                           BMat8({{1, 1, 1, 1, 0},
                                                  {1, 0, 0, 0, 1},
                                                  {0, 1, 0, 0, 1},
                                                  {0, 0, 1, 0, 1},
                                                  {0, 0, 0, 1, 1}}),
                                           BMat8({{1, 0, 0, 0, 0},
                                                  {0, 1, 0, 0, 0},
                                                  {0, 0, 1, 0, 0},
                                                  {0, 0, 0, 1, 0},
                                                  {0, 0, 0, 0, 0}}),
                                           BMat8({{1, 1, 1, 0, 0},
                                                  {1, 0, 0, 1, 0},
                                                  {0, 1, 0, 1, 0},
                                                  {0, 0, 1, 0, 1},
                                                  {0, 0, 0, 1, 1}}),
                                           BMat8({{1, 1, 1, 0, 0},
                                                  {1, 0, 0, 1, 0},
                                                  {1, 0, 0, 0, 1},
                                                  {0, 1, 0, 1, 0},
                                                  {0, 0, 1, 0, 1}}),
                                           BMat8({{1, 1, 1, 0, 0},
                                                  {1, 0, 0, 1, 1},
                                                  {0, 1, 0, 1, 0},
                                                  {0, 1, 0, 0, 1},
                                                  {0, 0, 1, 1, 0}}),
                                           BMat8({{1, 1, 1, 0, 0},
                                                  {1, 1, 0, 1, 0},
                                                  {1, 0, 0, 0, 1},
                                                  {0, 1, 0, 0, 1},
                                                  {0, 0, 1, 1, 1}})};

    FroidurePin<BMat8> S(bmat4_gens);
    REQUIRE(S.size() == 65536);

    //FroidurePin<BMat8> T(bmat5_gens);
    //REQUIRE(T.size() == 33554432);

    std::vector<size_t> nr_with_rank_dim4(17, 0);
    for (auto it = S.cbegin(); it < S.cend(); it++) {
      nr_with_rank_dim4[(*it).row_space_size()]++;
    }

    std::vector<size_t> nr_with_rank_dim5(32, 0);
    for (auto it = T.cbegin(); it < T.cend(); it++) {
      nr_with_rank_dim5[(*it).row_space_size()]++;
    }

    std::cout << "full boolean mat monoid 4: row rank counts: " << std::endl;
    for (size_t i = 0; i < nr_with_rank_dim4.size(); ++i) {
      std::cout << internal::to_string(i) << ": "
                << internal::to_string(nr_with_rank_dim4[i]) << std::endl;
    }
    std::cout << "full boolean mat monoid 5: row rank counts: " << std::endl;
    for (size_t i = 0; i < nr_with_rank_dim5.size(); ++i) {
      std::cout << internal::to_string(i) << ": "
                << internal::to_string(nr_with_rank_dim5[i]) << std::endl;
    }
  }
  */
}  // namespace libsemigroups
