//****************************************************************************//
//    Copyright (C) 2023-2024 James D. Mitchell <jdm3@st-andrews.ac.uk>       //
//    Copyright (C) 2023-2024 Florent Hivert <Florent.Hivert@lisn.fr>,        //
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

#ifndef HPCOMBI_TESTS_TEST_MAIN_HPP_
#define HPCOMBI_TESTS_TEST_MAIN_HPP_

#include <string>

#include "hpcombi/epu8.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_templated.hpp>

#define TEST_AGREES(fixture, ref, fun, vct, tags)                              \
    TEST_CASE_METHOD(fixture, #ref " == " #fun, tags) {                        \
        for (auto p : vct) {                                                   \
            CHECK(p.fun() == p.ref());                                         \
        }                                                                      \
    }

#define TEST_AGREES_FUN(fixture, ref, fun, vct, tags)                          \
    TEST_CASE_METHOD(fixture, #ref " == " #fun, tags) {                        \
        for (auto p : vct) {                                                   \
            CHECK(fun(p) == ref(p));                                           \
        }                                                                      \
    }

#define TEST_AGREES2(fixture, ref, fun, vct, tags)                             \
    TEST_CASE_METHOD(fixture, #ref " == " #fun, tags) {                        \
        for (auto p1 : vct) {                                                  \
            for (auto p2 : vct) {                                              \
                CHECK(p1.fun(p2) == p1.ref(p2));                               \
            }                                                                  \
        }                                                                      \
    }

#define TEST_AGREES2_FUN(fixture, ref, fun, vct, tags)                         \
    TEST_CASE_METHOD(fixture, #ref " == " #fun, tags) {                        \
        for (auto p1 : vct) {                                                  \
            for (auto p2 : vct) {                                              \
                CHECK(fun(p1, p2) == ref(p1, p2));                             \
            }                                                                  \
        }                                                                      \
    }

#define TEST_AGREES_EPU8(fixture, ref, fun, vct, tags)                         \
    TEST_CASE_METHOD(fixture, #ref " == " #fun, tags) {                        \
        for (auto p : vct) {                                                   \
            CHECK_THAT(p.fun(), Equals(p.ref()));                              \
        }                                                                      \
    }

#define TEST_AGREES_FUN_EPU8(fixture, ref, fun, vct, tags)                     \
    TEST_CASE_METHOD(fixture, #ref " == " #fun, tags) {                        \
        for (auto p : vct) {                                                   \
            CHECK_THAT(fun(p), Equals(ref(p)));                                \
        }                                                                      \
    }

#define TEST_AGREES2_EPU8(fixture, ref, fun, vct, tags)                        \
    TEST_CASE_METHOD(fixture, #ref " == " #fun, tags) {                        \
        for (auto p1 : vct) {                                                  \
            for (auto p2 : vct) {                                              \
                CHECK_THAT(p1.fun(p2), Equals(p1.ref(p2)));                    \
            }                                                                  \
        }                                                                      \
    }

#define TEST_AGREES2_FUN_EPU8(fixture, ref, fun, vct, tags)                    \
    TEST_CASE_METHOD(fixture, #ref " == " #fun, tags) {                        \
        for (auto p1 : vct) {                                                  \
            for (auto p2 : vct) {                                              \
                CHECK_THAT(fun(p1, p2), Equals(ref(p1, p2)));                  \
            }                                                                  \
        }                                                                      \
    }

struct Equals : Catch::Matchers::MatcherGenericBase {
    explicit Equals(HPCombi::epu8 v) : v(v) {}

    bool match(HPCombi::epu8 w) const { return HPCombi::equal(v, w); }

    std::string describe() const override {
        return "\n!=\n" + std::to_string(v);
    }

 private:
    const HPCombi::epu8 v;
};

#endif  // HPCOMBI_TESTS_TEST_MAIN_HPP_
