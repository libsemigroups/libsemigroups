//****************************************************************************//
//     Copyright (C) 2016-2024 Florent Hivert <Florent.Hivert@lisn.fr>,       //
//                                                                            //
//  This file is part of HP-Combi <https://github.com/libsemigroups/HPCombi>  //
//                                                                            //
//  HP-Combi is free software: you can redistribute it and/or modify it       //
//  under the terms of the GNU General Public License as published by the     //
//  Free Software Foundation, either version 3 of the License, or             //
//  (at your option) any later version.                                       //
//                                                                            //
//  HP-Combi is distributed in the hope that it will be useful, but WITHOUT   //
//  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or     //
//  FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License      //
//  for  more details.                                                        //
//                                                                            //
//  You should have received a copy of the GNU General Public License along   //
//  with HP-Combi. If not, see <https://www.gnu.org/licenses/>.               //
//****************************************************************************//

#include <cstddef>   // for size_t
#include <cstdint>   // for uint64_t
#include <iostream>  // for char_traits, ostream, ostrin...
#include <string>    // for operator==
#include <utility>   // for pair
#include <vector>    // for vector, allocator

#include "test_main.hpp"                 // for TEST_AGREES, TEST_AGREES2
#include <catch2/catch_test_macros.hpp>  // for operator""_catch_sr, operator==

#include "hpcombi/bmat8.hpp"   // for BMat8, operator<<
#include "hpcombi/perm16.hpp"  // for Perm16
#include "hpcombi/vect16.hpp"  // for Vect16

namespace HPCombi {
namespace {
struct BMat8Fixture {
    const BMat8 zero, one1, one2, ones, bm, bm1, bmm1, bm2, bm2t, bm3, bm3t;
    const std::vector<BMat8> BMlist;
    BMat8Fixture()
        : zero(0), one1(1), one2(0x201), ones(0xffffffffffffffff),
          bm({{0, 0, 0, 1, 0, 0, 1, 1},
              {1, 1, 1, 1, 1, 1, 0, 1},
              {0, 1, 1, 1, 0, 1, 0, 1},
              {1, 1, 0, 1, 1, 1, 1, 1},
              {0, 0, 1, 0, 0, 1, 1, 1},
              {1, 1, 0, 0, 0, 0, 0, 1},
              {0, 1, 0, 0, 0, 0, 1, 1},
              {0, 1, 1, 1, 1, 0, 1, 0}}),
          bm1({{0, 0, 0, 1, 0, 0, 1, 1},
               {0, 0, 1, 0, 0, 1, 0, 1},
               {1, 1, 0, 0, 1, 1, 0, 1},
               {1, 1, 0, 0, 0, 0, 0, 1},
               {0, 1, 0, 0, 0, 0, 1, 1},
               {0, 1, 0, 1, 1, 1, 1, 1},
               {0, 1, 0, 1, 0, 1, 0, 1},
               {0, 1, 0, 0, 0, 0, 1, 0}}),
          bmm1({{1, 1, 0, 1, 0, 1, 1, 1},
                {1, 1, 1, 1, 1, 1, 1, 1},
                {1, 1, 1, 1, 1, 1, 1, 1},
                {1, 1, 1, 1, 1, 1, 1, 1},
                {1, 1, 0, 1, 1, 1, 1, 1},
                {0, 1, 1, 1, 0, 1, 1, 1},
                {0, 1, 1, 1, 0, 1, 1, 1},
                {1, 1, 1, 1, 1, 1, 1, 1}}),
          bm2({{1, 1}, {0, 1}}), bm2t({{1, 0}, {1, 1}}),
          bm3({{0, 0, 0, 1, 0, 0, 1, 1},
               {1, 1, 1, 1, 1, 1, 0, 1},
               {0, 1, 1, 1, 1, 1, 0, 1},
               {1, 1, 0, 1, 1, 1, 1, 1},
               {0, 0, 1, 0, 0, 1, 1, 1},
               {1, 1, 0, 0, 0, 0, 0, 1},
               {0, 1, 0, 0, 0, 0, 1, 1},
               {0, 1, 1, 1, 1, 0, 1, 0}}),
          bm3t({{0, 1, 0, 1, 0, 1, 0, 0},
                {0, 1, 1, 1, 0, 1, 1, 1},
                {0, 1, 1, 0, 1, 0, 0, 1},
                {1, 1, 1, 1, 0, 0, 0, 1},
                {0, 1, 1, 1, 0, 0, 0, 1},
                {0, 1, 1, 1, 1, 0, 0, 0},
                {1, 0, 0, 1, 1, 0, 1, 1},
                {1, 1, 1, 1, 1, 1, 1, 0}}),
          BMlist(
              {zero, one1, one2, ones, bm, bm1, bmm1, bm2, bm2t, bm3, bm3t}) {}
};
}  // namespace

//****************************************************************************//
//****************************************************************************//

TEST_CASE_METHOD(BMat8Fixture, "BMat8::one", "[BMat8][000]") {
    CHECK(BMat8::one(0) == zero);
    CHECK(BMat8::one(2) == BMat8({{1, 0, 0, 0, 0, 0, 0, 0},
                                  {0, 1, 0, 0, 0, 0, 0, 0},
                                  {0, 0, 0, 0, 0, 0, 0, 0},
                                  {0, 0, 0, 0, 0, 0, 0, 0},
                                  {0, 0, 0, 0, 0, 0, 0, 0},
                                  {0, 0, 0, 0, 0, 0, 0, 0},
                                  {0, 0, 0, 0, 0, 0, 0, 0},
                                  {0, 0, 0, 0, 0, 0, 0, 0}}));
    CHECK(BMat8::one(5) == BMat8({{1, 0, 0, 0, 0, 0, 0, 0},
                                  {0, 1, 0, 0, 0, 0, 0, 0},
                                  {0, 0, 1, 0, 0, 0, 0, 0},
                                  {0, 0, 0, 1, 0, 0, 0, 0},
                                  {0, 0, 0, 0, 1, 0, 0, 0},
                                  {0, 0, 0, 0, 0, 0, 0, 0},
                                  {0, 0, 0, 0, 0, 0, 0, 0},
                                  {0, 0, 0, 0, 0, 0, 0, 0}}));
    CHECK(BMat8::one(8) == BMat8::one());
}

TEST_CASE_METHOD(BMat8Fixture, "BMat8::transpose", "[BMat8][001]") {
    CHECK(zero.transpose() == zero);
    CHECK(bm2.transpose() == bm2t);
    CHECK(bm3.transpose() == bm3t);

    for (auto m : BMlist) {
        CHECK(m.transpose().transpose() == m);
    }
}

TEST_AGREES(BMat8Fixture, transpose, transpose_mask, BMlist, "[BMat8][002]");
TEST_AGREES(BMat8Fixture, transpose, transpose_maskd, BMlist, "[BMat8][003]");
TEST_AGREES(BMat8Fixture, transpose, transpose_naive, BMlist, "[BMat8][004]");

TEST_CASE_METHOD(BMat8Fixture, "BMat8::transpose2", "[BMat8][005]") {
    for (auto a : BMlist) {
        for (auto b : BMlist) {
            BMat8 at = a, bt = b;
            BMat8::transpose2(at, bt);
            CHECK(at == a.transpose());
            CHECK(bt == b.transpose());
        }
    }
}

TEST_CASE_METHOD(BMat8Fixture, "BMat8::operator*", "[BMat8][006]") {
    BMat8 tmp = bm * bm1;
    CHECK(tmp == bmm1);
    CHECK(tmp == bm * bm1);

    for (auto b : BMlist) {
        CHECK(zero * b == zero);
        CHECK(b * zero == zero);
        CHECK(b * b.one() == b);
        CHECK(b.one() * b == b);
        CHECK((b * b) * (b * b) == b * b * b * b);
    }

    for (auto b1 : BMlist) {
        for (auto b2 : BMlist) {
            for (auto b3 : BMlist) {
                CHECK((b1 * b2) * b3 == b1 * (b2 * b3));
            }
        }
    }
}

TEST_AGREES2(BMat8Fixture, BMat8::operator*, mult_naive, BMlist, "[BMat8][007]");
TEST_AGREES2(BMat8Fixture, BMat8::operator*, mult_naive_array, BMlist, "[BMat8][008]");

TEST_CASE("BMat8::random", "[BMat8][009]") {
    for (size_t d = 1; d < 8; ++d) {
        BMat8 bm = BMat8::random(d);
        for (size_t i = d + 1; i < 8; ++i) {
            for (size_t j = 0; j < 8; ++j) {
                CHECK(bm(i, j) == 0);
                CHECK(bm(j, i) == 0);
            }
        }
    }
}

TEST_CASE("BMat8::operator()", "[BMat8][010]") {
    std::vector<std::vector<bool>> mat = {
        {0, 0, 0, 1, 0, 0, 1}, {0, 1, 1, 1, 0, 1, 0}, {1, 1, 0, 1, 1, 1, 1},
        {0, 0, 1, 0, 0, 1, 1}, {1, 1, 0, 0, 0, 0, 0}, {0, 1, 0, 0, 0, 0, 1},
        {0, 1, 1, 1, 1, 0, 1}};
    BMat8 bm(mat);
    for (size_t i = 0; i < 7; ++i) {
        for (size_t j = 0; j < 7; ++j) {
            CHECK(static_cast<size_t>(bm(i, j)) == mat[i][j]);
        }
    }
}

TEST_CASE_METHOD(BMat8Fixture, "BMat8::operator<<", "[BMat8][011]") {
    std::ostringstream oss;
    oss << bm3;
    CHECK(oss.str() == "00010011\n"
                       "11111101\n"
                       "01111101\n"
                       "11011111\n"
                       "00100111\n"
                       "11000001\n"
                       "01000011\n"
                       "01111010\n");

    std::stringbuf buff;
    std::ostream os(&buff);
    os << BMat8::random();  // Also does not do anything visible
}

TEST_CASE_METHOD(BMat8Fixture, "BMat8::set", "[BMat8][012]") {
    BMat8 bs;
    bs = bm;
    bs.set(0, 0, 1);
    CHECK(bs != bm);
    bs = bm;
    bs.set(0, 0, 0);
    CHECK(bs == bm);
    bs = bm;
    bs.set(2, 4, 1);
    CHECK(bs != bm);
    CHECK(bs == bm3);

    for (size_t i = 0; i < 8; ++i)
        for (size_t j = 0; j < 8; ++j)
            bs.set(i, j, true);
    CHECK(bs == ones);

    for (size_t i = 0; i < 8; ++i)
        for (size_t j = 0; j < 8; ++j)
            bs.set(i, j, false);
    CHECK(bs == zero);
}

TEST_CASE("BMat8::row_space_basis", "[BMat8][013]") {
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

    CHECK(bm.row_space_basis() == bm2.row_space_basis());

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

    CHECK(bm3.row_space_basis() == bm4);
    CHECK(bm4.row_space_basis() == bm4);

    BMat8 bm5(0xff00000000000000);

    uint64_t data = 0xffffffffffffffff;

    for (size_t i = 0; i < 7; ++i) {
        CHECK(BMat8(data).row_space_basis() == bm5);
        data = data >> 8;
    }

    for (size_t i = 0; i < 1000; ++i) {
        bm = BMat8::random();
        CHECK(bm.row_space_basis().row_space_basis() == bm.row_space_basis());
    }
}

TEST_CASE("BMat8::col_space_basis", "[BMat8][014]") {
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

    CHECK(bm.col_space_basis() == bm2);

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

    CHECK(bm3.col_space_basis() == bm4);

    uint64_t col = 0x8080808080808080;
    BMat8 bm5(col);

    uint64_t data = 0xffffffffffffffff;

    for (size_t i = 0; i < 7; ++i) {
        CHECK(BMat8(data).col_space_basis() == bm5);
        data &= ~(col >> i);
    }

    for (size_t i = 0; i < 1000; ++i) {
        bm = BMat8::random();
        CHECK(bm.col_space_basis().col_space_basis() == bm.col_space_basis());
    }
}

TEST_CASE_METHOD(BMat8Fixture, "BMat8::row_space_size", "[BMat8][015]") {
    CHECK(zero.row_space_size() == 1);
    CHECK(one1.row_space_size() == 2);
    CHECK(one2.row_space_size() == 4);
    CHECK(BMat8::one().row_space_size() == 256);
    CHECK(bm.row_space_size() == 22);
    CHECK(bm1.row_space_size() == 31);
    CHECK(bm2.row_space_size() == 3);
    CHECK(bm2t.row_space_size() == 3);
    CHECK(bm3.row_space_size() == 21);
    CHECK(bm3t.row_space_size() == 21);
    CHECK(bmm1.row_space_size() == 6);
}

TEST_AGREES(BMat8Fixture, row_space_size_ref, row_space_size, BMlist,
            "[BMat8][016]");
TEST_AGREES(BMat8Fixture, row_space_size_ref, row_space_size_incl, BMlist,
            "[BMat8][017]");
TEST_AGREES(BMat8Fixture, row_space_size_ref, row_space_size_incl1, BMlist,
            "[BMat8][018]");
TEST_AGREES(BMat8Fixture, row_space_size_ref, row_space_size_bitset, BMlist,
            "[BMat8][019]");

TEST_CASE_METHOD(BMat8Fixture, "BMat8::row_space_included", "[BMat8][020]") {
    CHECK(zero.row_space_included(one1));
    CHECK_FALSE(one1.row_space_included(zero));

    BMat8 m1({{1, 1, 0}, {1, 0, 1}, {0, 0, 0}});
    BMat8 m2({{0, 0, 0}, {1, 0, 1}, {1, 1, 0}});
    CHECK(m1.row_space_included(m2));
    CHECK(m2.row_space_included(m1));

    BMat8 m3({{0, 0, 1}, {1, 0, 1}, {1, 1, 0}});
    CHECK(m1.row_space_included(m3));
    CHECK(m2.row_space_included(m3));
    CHECK_FALSE(m3.row_space_included(m1));
    CHECK_FALSE(m3.row_space_included(m1));

    CHECK(m1.row_space_included(BMat8::one()));
    CHECK(m2.row_space_included(BMat8::one()));
    CHECK(m3.row_space_included(BMat8::one()));
}

TEST_AGREES2(BMat8Fixture, row_space_included, row_space_included_ref, BMlist,
             "[BMat8][021]");
TEST_AGREES2(BMat8Fixture, row_space_included, row_space_included_bitset,
             BMlist, "[BMat8][022]");

TEST_CASE_METHOD(BMat8Fixture, "BMat8::row_space_included2", "[BMat8][023]") {
    BMat8 a0 = BMat8::one();
    BMat8 b0 = BMat8(0);
    BMat8 a1 = BMat8(0);
    BMat8 b1 = BMat8::one();

    auto res = BMat8::row_space_included2(a0, b0, a1, b1);
    CHECK(res.first == a0.row_space_included(b0));
    CHECK(res.second == a1.row_space_included(b1));

    for (auto a0 : BMlist) {
        for (auto b0 : BMlist) {
            for (auto a1 : BMlist) {
                for (auto b1 : BMlist) {
                    auto res = BMat8::row_space_included2(a0, b0, a1, b1);
                    CHECK(res.first == a0.row_space_included(b0));
                    CHECK(res.second == a1.row_space_included(b1));
                }
            }
        }
    }
}

TEST_CASE_METHOD(BMat8Fixture, "BMat8::row_permuted", "[BMat8][024]") {
    CHECK(bm2.row_permuted(Perm16({1, 0})) == BMat8({{0, 1}, {1, 1}}));
    CHECK(bm2.row_permuted(Perm16({2, 1, 0})) ==
          BMat8({{0, 0, 0}, {0, 1, 0}, {1, 1, 0}}));
    CHECK(bm.row_permuted(Perm16({5, 3, 1, 4, 2, 0})) ==
          BMat8({{1, 1, 0, 0, 0, 0, 0, 1},
                 {1, 1, 0, 1, 1, 1, 1, 1},
                 {1, 1, 1, 1, 1, 1, 0, 1},
                 {0, 0, 1, 0, 0, 1, 1, 1},
                 {0, 1, 1, 1, 0, 1, 0, 1},
                 {0, 0, 0, 1, 0, 0, 1, 1},
                 {0, 1, 0, 0, 0, 0, 1, 1},
                 {0, 1, 1, 1, 1, 0, 1, 0}}));
    CHECK(BMat8::one().row_permuted(Perm16({5, 3, 1, 4, 2, 0})) ==
          BMat8({{0, 0, 0, 0, 0, 1, 0, 0},
                 {0, 0, 0, 1, 0, 0, 0, 0},
                 {0, 1, 0, 0, 0, 0, 0, 0},
                 {0, 0, 0, 0, 1, 0, 0, 0},
                 {0, 0, 1, 0, 0, 0, 0, 0},
                 {1, 0, 0, 0, 0, 0, 0, 0},
                 {0, 0, 0, 0, 0, 0, 1, 0},
                 {0, 0, 0, 0, 0, 0, 0, 1}}));
}

TEST_CASE_METHOD(BMat8Fixture, "BMat8::col_permuted", "[BMat8][025]") {
    CHECK(bm2.col_permuted(Perm16({1, 0})) == BMat8({{1, 1}, {1, 0}}));
    CHECK(bm2.col_permuted(Perm16({2, 1, 0})) ==
          BMat8({{0, 1, 1}, {0, 1, 0}, {0, 0, 0}}));
    CHECK(bm.col_permuted(Perm16({5, 3, 1, 4, 2, 0})) ==
          BMat8({{0, 1, 0, 0, 0, 0, 1, 1},
                 {1, 1, 1, 1, 1, 1, 0, 1},
                 {1, 1, 1, 0, 1, 0, 0, 1},
                 {1, 1, 1, 1, 0, 1, 1, 1},
                 {1, 0, 0, 0, 1, 0, 1, 1},
                 {0, 0, 1, 0, 0, 1, 0, 1},
                 {0, 0, 1, 0, 0, 0, 1, 1},
                 {0, 1, 1, 1, 1, 0, 1, 0}}));
    CHECK(BMat8::one().col_permuted(Perm16({4, 1, 3, 0, 2, 6, 5})) ==
          BMat8({{0, 0, 0, 1, 0, 0, 0, 0},
                 {0, 1, 0, 0, 0, 0, 0, 0},
                 {0, 0, 0, 0, 1, 0, 0, 0},
                 {0, 0, 1, 0, 0, 0, 0, 0},
                 {1, 0, 0, 0, 0, 0, 0, 0},
                 {0, 0, 0, 0, 0, 0, 1, 0},
                 {0, 0, 0, 0, 0, 1, 0, 0},
                 {0, 0, 0, 0, 0, 0, 0, 1}}));
}

TEST_CASE("BMat8::row_permutation_matrix", "[BMat8][026]") {
    CHECK(BMat8::row_permutation_matrix(Perm16({1, 0})) ==
          BMat8({{0, 1, 0, 0, 0, 0, 0, 0},
                 {1, 0, 0, 0, 0, 0, 0, 0},
                 {0, 0, 1, 0, 0, 0, 0, 0},
                 {0, 0, 0, 1, 0, 0, 0, 0},
                 {0, 0, 0, 0, 1, 0, 0, 0},
                 {0, 0, 0, 0, 0, 1, 0, 0},
                 {0, 0, 0, 0, 0, 0, 1, 0},
                 {0, 0, 0, 0, 0, 0, 0, 1}}));
    CHECK(BMat8::row_permutation_matrix(Perm16({1, 3, 4, 0, 2})) ==
          BMat8({{0, 1, 0, 0, 0, 0, 0, 0},
                 {0, 0, 0, 1, 0, 0, 0, 0},
                 {0, 0, 0, 0, 1, 0, 0, 0},
                 {1, 0, 0, 0, 0, 0, 0, 0},
                 {0, 0, 1, 0, 0, 0, 0, 0},
                 {0, 0, 0, 0, 0, 1, 0, 0},
                 {0, 0, 0, 0, 0, 0, 1, 0},
                 {0, 0, 0, 0, 0, 0, 0, 1}}));
    CHECK(BMat8::row_permutation_matrix(Perm16({5, 3, 1, 4, 2, 0})) ==
          BMat8({{0, 0, 0, 0, 0, 1, 0, 0},
                 {0, 0, 0, 1, 0, 0, 0, 0},
                 {0, 1, 0, 0, 0, 0, 0, 0},
                 {0, 0, 0, 0, 1, 0, 0, 0},
                 {0, 0, 1, 0, 0, 0, 0, 0},
                 {1, 0, 0, 0, 0, 0, 0, 0},
                 {0, 0, 0, 0, 0, 0, 1, 0},
                 {0, 0, 0, 0, 0, 0, 0, 1}}));
}

TEST_CASE("BMat8::col_permutation_matrix", "[BMat8][027]") {
    CHECK(BMat8::col_permutation_matrix(Perm16({1, 0})) ==
          BMat8({{0, 1, 0, 0, 0, 0, 0, 0},
                 {1, 0, 0, 0, 0, 0, 0, 0},
                 {0, 0, 1, 0, 0, 0, 0, 0},
                 {0, 0, 0, 1, 0, 0, 0, 0},
                 {0, 0, 0, 0, 1, 0, 0, 0},
                 {0, 0, 0, 0, 0, 1, 0, 0},
                 {0, 0, 0, 0, 0, 0, 1, 0},
                 {0, 0, 0, 0, 0, 0, 0, 1}}));
    CHECK(BMat8::col_permutation_matrix(Perm16({1, 3, 4, 0, 2})) ==
          BMat8({{0, 0, 0, 1, 0, 0, 0, 0},
                 {1, 0, 0, 0, 0, 0, 0, 0},
                 {0, 0, 0, 0, 1, 0, 0, 0},
                 {0, 1, 0, 0, 0, 0, 0, 0},
                 {0, 0, 1, 0, 0, 0, 0, 0},
                 {0, 0, 0, 0, 0, 1, 0, 0},
                 {0, 0, 0, 0, 0, 0, 1, 0},
                 {0, 0, 0, 0, 0, 0, 0, 1}}));
    CHECK(BMat8::col_permutation_matrix(Perm16({5, 3, 1, 4, 2, 0})) ==
          BMat8({{0, 0, 0, 0, 0, 1, 0, 0},
                 {0, 0, 1, 0, 0, 0, 0, 0},
                 {0, 0, 0, 0, 1, 0, 0, 0},
                 {0, 1, 0, 0, 0, 0, 0, 0},
                 {0, 0, 0, 1, 0, 0, 0, 0},
                 {1, 0, 0, 0, 0, 0, 0, 0},
                 {0, 0, 0, 0, 0, 0, 1, 0},
                 {0, 0, 0, 0, 0, 0, 0, 1}}));
}

TEST_CASE_METHOD(BMat8Fixture, "BMat8::nr_rows", "[BMat8][028]") {
    CHECK(zero.nr_rows() == 0);
    CHECK(one1.nr_rows() == 1);
    CHECK(one2.nr_rows() == 2);
    CHECK(bm.nr_rows() == 8);
    CHECK(BMat8({{1, 0, 1}, {1, 1, 0}, {0, 0, 0}}).nr_rows() == 2);
}

TEST_CASE("BMat8::right_perm_action_on_basis_ref", "[BMat8][029]") {
    BMat8 m1({{1, 1, 0}, {1, 0, 1}, {0, 0, 0}});
    BMat8 m2({{0, 0, 0}, {1, 0, 1}, {1, 1, 0}});
    CHECK(m1.right_perm_action_on_basis_ref(m2) == Perm16({1, 0}));
    CHECK(m1.right_perm_action_on_basis(m2) == Perm16({1, 0}));

    m1 = BMat8({{1, 1, 0, 1}, {1, 0, 1, 0}, {0, 0, 0, 1}, {0, 0, 0, 0}});
    m2 = BMat8({{1, 0, 0, 0}, {0, 1, 0, 1}, {1, 0, 1, 0}, {0, 0, 0, 1}});
    CHECK(m1.right_perm_action_on_basis_ref(m2) == Perm16::one());
    CHECK(m1.right_perm_action_on_basis(m2) == Perm16::one());

    m1 = BMat8({{1, 1, 0, 1}, {1, 0, 1, 0}, {0, 0, 0, 1}, {0, 0, 0, 0}});
    m2 = BMat8({{0, 0, 0, 0}, {1, 1, 0, 1}, {1, 0, 1, 0}, {0, 0, 0, 1}});
    CHECK(m1.right_perm_action_on_basis_ref(m2) == Perm16::one());
    CHECK(m1.right_perm_action_on_basis(m2) == Perm16::one());

    m1 = BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {1, 0, 0, 1}, {0, 0, 0, 0}});
    m2 = BMat8({{1, 0, 0, 1}, {0, 0, 1, 0}, {0, 1, 0, 0}, {0, 0, 0, 1}});
    CHECK(m1.right_perm_action_on_basis_ref(m2) == Perm16({1, 0}));
    CHECK(m1.right_perm_action_on_basis(m2) == Perm16({1, 0}));

    m1 = BMat8({{0, 0, 0, 1}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 1, 0, 0}});
    m2 = BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {1, 0, 0, 0}, {0, 0, 0, 1}});
    CHECK(m1.right_perm_action_on_basis_ref(m2) == Perm16({0, 2, 3, 1}));
    CHECK(m1.right_perm_action_on_basis(m2) == Perm16({0, 2, 3, 1}));

    m1 = BMat8({{0, 0, 0, 1}, {0, 0, 1, 0}, {0, 1, 0, 0}, {1, 0, 0, 0}});
    m2 = BMat8({{0, 1, 0, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}, {0, 0, 1, 0}});
    CHECK(m1.right_perm_action_on_basis_ref(m2) == Perm16({2, 0, 3, 1}));
    CHECK(m1.right_perm_action_on_basis(m2) == Perm16({2, 0, 3, 1}));
}

}  // namespace HPCombi
