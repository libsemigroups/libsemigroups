// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2020 Finn Smith
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

#include <algorithm>  // for for_each
#include <chrono>     // for milliseconds
#include <cstddef>    // for size_t
#include <vector>     // for vector

#include "catch.hpp"      // for REQUIRE
#include "test-main.hpp"  // for LIBSEMIGROUPS_TEST...

#include "libsemigroups/bmat8.hpp"         // for BMat8
#include "libsemigroups/fastest-bmat.hpp"  // for FastestBMat
#include "libsemigroups/konieczny.hpp"     // for Konieczny, Koniecz...
#include "libsemigroups/report.hpp"        // for ReportGuard

namespace libsemigroups {

  struct LibsemigroupsException;
  constexpr bool REPORT = false;

  LIBSEMIGROUPS_TEST_CASE("Konieczny",
                          "016",
                          "full bmat monoid 5",
                          "[extreme][bmat8]") {
#ifdef LIBSEMIGROUPS_HPCOMBI_ENABLED
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#pragma GCC diagnostic ignored "-Winline"
#endif
    using BMat                         = FastestBMat<5>;
    auto                    rg         = ReportGuard(true);
    const std::vector<BMat> bmat5_gens = {BMat({{1, 0, 0, 0, 0},
                                                {0, 1, 0, 0, 0},
                                                {0, 0, 1, 0, 0},
                                                {0, 0, 0, 1, 0},
                                                {0, 0, 0, 0, 1}}),
                                          BMat({{0, 1, 0, 0, 0},
                                                {0, 0, 1, 0, 0},
                                                {0, 0, 0, 1, 0},
                                                {0, 0, 0, 0, 1},
                                                {1, 0, 0, 0, 0}}),
                                          BMat({{0, 1, 0, 0, 0},
                                                {1, 0, 0, 0, 0},
                                                {0, 0, 1, 0, 0},
                                                {0, 0, 0, 1, 0},
                                                {0, 0, 0, 0, 1}}),
                                          BMat({{1, 0, 0, 0, 0},
                                                {0, 1, 0, 0, 0},
                                                {0, 0, 1, 0, 0},
                                                {0, 0, 0, 1, 0},
                                                {1, 0, 0, 0, 1}}),
                                          BMat({{1, 1, 0, 0, 0},
                                                {1, 0, 1, 0, 0},
                                                {0, 1, 0, 1, 0},
                                                {0, 0, 1, 1, 0},
                                                {0, 0, 0, 0, 1}}),
                                          BMat({{1, 1, 0, 0, 0},
                                                {1, 0, 1, 0, 0},
                                                {0, 1, 1, 0, 0},
                                                {0, 0, 0, 1, 0},
                                                {0, 0, 0, 0, 1}}),
                                          BMat({{1, 1, 1, 0, 0},
                                                {1, 0, 0, 1, 0},
                                                {0, 1, 0, 1, 0},
                                                {0, 0, 1, 1, 0},
                                                {0, 0, 0, 0, 1}}),
                                          BMat({{1, 1, 0, 0, 0},
                                                {1, 0, 1, 0, 0},
                                                {0, 1, 0, 1, 0},
                                                {0, 0, 1, 0, 1},
                                                {0, 0, 0, 1, 1}}),
                                          BMat({{1, 1, 1, 1, 0},
                                                {1, 0, 0, 0, 1},
                                                {0, 1, 0, 0, 1},
                                                {0, 0, 1, 0, 1},
                                                {0, 0, 0, 1, 1}}),
                                          BMat({{1, 0, 0, 0, 0},
                                                {0, 1, 0, 0, 0},
                                                {0, 0, 1, 0, 0},
                                                {0, 0, 0, 1, 0},
                                                {0, 0, 0, 0, 0}}),
                                          BMat({{1, 1, 1, 0, 0},
                                                {1, 0, 0, 1, 0},
                                                {0, 1, 0, 1, 0},
                                                {0, 0, 1, 0, 1},
                                                {0, 0, 0, 1, 1}}),
                                          BMat({{1, 1, 1, 0, 0},
                                                {1, 0, 0, 1, 0},
                                                {1, 0, 0, 0, 1},
                                                {0, 1, 0, 1, 0},
                                                {0, 0, 1, 0, 1}}),
                                          BMat({{1, 1, 1, 0, 0},
                                                {1, 0, 0, 1, 1},
                                                {0, 1, 0, 1, 0},
                                                {0, 1, 0, 0, 1},
                                                {0, 0, 1, 1, 0}}),
                                          BMat({{1, 1, 1, 0, 0},
                                                {1, 1, 0, 1, 0},
                                                {1, 0, 0, 0, 1},
                                                {0, 1, 0, 0, 1},
                                                {0, 0, 1, 1, 1}})};

    Konieczny<BMat> T(bmat5_gens);
    REQUIRE(T.size() == 33554432);

#ifdef LIBSEMIGROUPS_HPCOMBI_ENABLED
#pragma GCC diagnostic pop
#endif
  }

  LIBSEMIGROUPS_TEST_CASE("Konieczny",
                          "017",
                          "regular generated bmat monoid 4 idempotents",
                          "[quick][no-valgrind][bmat8]") {
    auto                     rg = ReportGuard(REPORT);
    const std::vector<BMat8> reg_bmat4_gens
        = {BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
           BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
           BMat8({{1, 0, 0, 0}, {1, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
           BMat8({{0, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}})};

    Konieczny<BMat8> S(reg_bmat4_gens);

    REQUIRE(S.contains(reg_bmat4_gens[0]));
    REQUIRE(S.contains(reg_bmat4_gens[1]));
    REQUIRE(S.contains(reg_bmat4_gens[2]));
    REQUIRE(S.contains(reg_bmat4_gens[3]));
    REQUIRE(S.current_size() < 15000);
    REQUIRE(S.current_number_of_regular_elements() < 10000);
    REQUIRE(S.current_number_of_idempotents() < 500);
    REQUIRE(S.current_number_of_D_classes() < 100);
    REQUIRE(S.current_number_of_L_classes() < 300);
    REQUIRE(S.current_number_of_R_classes() < 300);
    REQUIRE(S.current_number_of_regular_D_classes() < 50);
    REQUIRE(S.current_number_of_regular_L_classes() < 150);
    REQUIRE(S.current_number_of_regular_R_classes() < 150);

    REQUIRE(S.size() == 63904);
    REQUIRE(S.number_of_regular_elements() == 40408);
    REQUIRE(S.number_of_idempotents() == 2360);

    REQUIRE(S.current_size() == S.size());
    REQUIRE(S.current_number_of_regular_elements()
            == S.number_of_regular_elements());
    REQUIRE(S.current_number_of_idempotents() == S.number_of_idempotents());
    REQUIRE(S.current_number_of_D_classes() == S.number_of_D_classes());
    REQUIRE(S.current_number_of_L_classes() == S.number_of_L_classes());
    REQUIRE(S.current_number_of_R_classes() == S.number_of_R_classes());

    REQUIRE(S.current_number_of_regular_D_classes()
            == S.number_of_regular_D_classes());
    REQUIRE(S.current_number_of_regular_L_classes()
            == S.number_of_regular_L_classes());
    REQUIRE(S.current_number_of_regular_R_classes()
            == S.number_of_regular_R_classes());
  }

  LIBSEMIGROUPS_TEST_CASE("Konieczny",
                          "018",
                          "regular generated bmat monoid 5",
                          "[extreme][bmat8]") {
#ifdef LIBSEMIGROUPS_HPCOMBI_ENABLED
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#pragma GCC diagnostic ignored "-Winline"
#endif
    using BMat                             = FastestBMat<5>;
    auto                    rg             = ReportGuard(true);
    const std::vector<BMat> reg_bmat5_gens = {BMat({{0, 1, 0, 0, 0},
                                                    {1, 0, 0, 0, 0},
                                                    {0, 0, 1, 0, 0},
                                                    {0, 0, 0, 1, 0},
                                                    {0, 0, 0, 0, 1}}),
                                              BMat({{0, 1, 0, 0, 0},
                                                    {0, 0, 1, 0, 0},
                                                    {0, 0, 0, 1, 0},
                                                    {0, 0, 0, 0, 1},
                                                    {1, 0, 0, 0, 0}}),
                                              BMat({{1, 0, 0, 0, 0},
                                                    {1, 1, 0, 0, 0},
                                                    {0, 0, 1, 0, 0},
                                                    {0, 0, 0, 1, 0},
                                                    {0, 0, 0, 0, 1}}),
                                              BMat({{0, 0, 0, 0, 0},
                                                    {0, 1, 0, 0, 0},
                                                    {0, 0, 1, 0, 0},
                                                    {0, 0, 0, 1, 0},
                                                    {0, 0, 0, 0, 1}})};

    using DClass = Konieczny<BMat>::DClass;

    Konieczny<BMat> T(reg_bmat5_gens);
    REQUIRE(T.size() == 32311832);

    size_t sum = 0;
    std::for_each(T.cbegin_rdc(), T.cend_rdc(), [&sum](DClass const& x) {
      sum += x.size();
    });
    REQUIRE(sum == 8683982);

    sum = 0;
    std::for_each(T.cbegin_rdc(), T.cend_rdc(), [&sum](DClass const& x) {
      sum += x.number_of_idempotents();
    });
    REQUIRE(sum == 73023);

    sum = 0;
    std::for_each(T.cbegin_rdc(), T.cend_rdc(), [&sum, &T](DClass const& x) {
      sum += T.D_class_of_element(x.rep()).number_of_idempotents();
    });
    REQUIRE(sum == 73023);
    REQUIRE(T.number_of_idempotents() == 73023);
#ifdef LIBSEMIGROUPS_HPCOMBI_ENABLED
#pragma GCC diagnostic pop
#endif
  }

  LIBSEMIGROUPS_TEST_CASE("Konieczny",
                          "019",
                          "my favourite example",
                          "[quick][finite][no-valgrind][bmat8]") {
    auto                     rg   = ReportGuard(REPORT);
    const std::vector<BMat8> gens = {BMat8({{0, 1, 0, 0, 0, 0, 0, 0},
                                            {0, 0, 0, 0, 0, 0, 0, 1},
                                            {0, 0, 1, 0, 0, 0, 0, 0},
                                            {0, 0, 0, 0, 0, 0, 1, 0},
                                            {1, 0, 0, 0, 0, 0, 0, 0},
                                            {0, 0, 0, 0, 1, 0, 0, 0},
                                            {0, 1, 0, 0, 0, 0, 0, 0},
                                            {0, 0, 0, 0, 0, 1, 0, 0}}),
                                     BMat8({{0, 0, 1, 0, 0, 0, 0, 0},
                                            {0, 0, 0, 0, 1, 0, 0, 0},
                                            {0, 0, 0, 0, 0, 0, 1, 0},
                                            {0, 1, 0, 0, 0, 0, 0, 0},
                                            {0, 0, 0, 0, 1, 0, 0, 0},
                                            {0, 0, 0, 0, 0, 1, 0, 0},
                                            {0, 0, 1, 0, 0, 0, 0, 0},
                                            {0, 0, 0, 0, 0, 0, 0, 1}}),
                                     BMat8({{0, 0, 0, 1, 0, 0, 0, 0},
                                            {1, 0, 0, 0, 0, 0, 0, 0},
                                            {0, 0, 0, 0, 0, 0, 0, 1},
                                            {0, 0, 1, 0, 0, 0, 0, 0},
                                            {0, 0, 0, 0, 1, 0, 0, 0},
                                            {0, 0, 0, 0, 0, 0, 1, 0},
                                            {0, 0, 1, 0, 0, 0, 0, 0},
                                            {0, 0, 0, 0, 1, 0, 0, 0}}),
                                     BMat8({{0, 0, 0, 1, 0, 0, 0, 0},
                                            {0, 0, 1, 0, 0, 0, 0, 0},
                                            {0, 0, 0, 1, 0, 0, 0, 0},
                                            {0, 0, 0, 0, 1, 0, 0, 0},
                                            {0, 0, 0, 0, 0, 1, 0, 0},
                                            {0, 0, 0, 1, 0, 0, 0, 0},
                                            {1, 0, 0, 0, 0, 0, 0, 0},
                                            {0, 1, 0, 0, 0, 0, 0, 0}}),
                                     BMat8({{0, 0, 0, 0, 1, 0, 0, 0},
                                            {0, 0, 0, 1, 0, 0, 0, 0},
                                            {0, 0, 0, 0, 0, 0, 0, 1},
                                            {0, 0, 0, 0, 0, 0, 0, 1},
                                            {0, 0, 0, 0, 1, 0, 0, 0},
                                            {0, 0, 0, 0, 0, 1, 0, 0},
                                            {1, 0, 0, 0, 0, 0, 0, 0},
                                            {0, 0, 0, 0, 1, 0, 0, 0}}),
                                     BMat8({{0, 0, 0, 0, 0, 1, 0, 0},
                                            {0, 0, 0, 0, 0, 0, 1, 0},
                                            {0, 0, 0, 1, 0, 0, 0, 0},
                                            {1, 0, 0, 0, 0, 0, 0, 0},
                                            {0, 0, 0, 1, 0, 0, 0, 0},
                                            {1, 0, 0, 0, 0, 0, 0, 0},
                                            {0, 0, 0, 0, 0, 1, 0, 0},
                                            {0, 1, 0, 0, 0, 0, 0, 0}}),
                                     BMat8({{0, 0, 0, 0, 0, 0, 1, 0},
                                            {1, 0, 0, 0, 0, 0, 0, 0},
                                            {0, 1, 0, 0, 0, 0, 0, 0},
                                            {0, 1, 0, 0, 0, 0, 0, 0},
                                            {0, 1, 0, 0, 0, 0, 0, 0},
                                            {0, 0, 0, 0, 0, 0, 1, 0},
                                            {0, 0, 0, 1, 0, 0, 0, 0},
                                            {0, 0, 0, 0, 1, 0, 0, 0}}),
                                     BMat8({{0, 0, 0, 0, 0, 0, 0, 1},
                                            {0, 0, 0, 0, 0, 0, 0, 1},
                                            {0, 0, 0, 0, 1, 0, 0, 0},
                                            {1, 0, 0, 0, 0, 0, 0, 0},
                                            {0, 0, 0, 0, 0, 0, 1, 0},
                                            {0, 0, 0, 0, 1, 0, 0, 0},
                                            {0, 1, 0, 0, 0, 0, 0, 0},
                                            {0, 0, 0, 0, 0, 0, 0, 1}})};

    Konieczny<BMat8> S(gens);
    REQUIRE(S.size() == 597369);
  }

  LIBSEMIGROUPS_TEST_CASE("Konieczny",
                          "020",
                          "another large example",
                          "[quick][no-valgrind][bmat8]") {
#ifdef LIBSEMIGROUPS_HPCOMBI_ENABLED
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#pragma GCC diagnostic ignored "-Winline"
#endif
    using BMat                   = FastestBMat<8>;
    auto                    rg   = ReportGuard(REPORT);
    const std::vector<BMat> gens = {BMat({{0, 1, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 1, 0, 0},
                                          {0, 0, 0, 0, 0, 1, 0, 0},
                                          {0, 0, 0, 0, 1, 0, 0, 0},
                                          {0, 0, 1, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 1, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 1, 0},
                                          {0, 0, 0, 0, 1, 0, 0, 0}}),
                                    BMat({{0, 0, 0, 1, 0, 0, 0, 0},
                                          {0, 1, 0, 0, 0, 0, 0, 0},
                                          {1, 0, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 1, 0, 0},
                                          {0, 1, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 0, 1},
                                          {0, 0, 1, 0, 0, 0, 0, 0},
                                          {1, 0, 0, 0, 0, 0, 0, 0}}),
                                    BMat({{0, 0, 0, 1, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 1, 0},
                                          {0, 0, 0, 1, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 1, 0},
                                          {0, 0, 0, 0, 1, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 1, 0},
                                          {0, 0, 0, 0, 0, 1, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 0, 1}}),
                                    BMat({{0, 0, 0, 0, 0, 0, 0, 1},
                                          {0, 0, 0, 0, 0, 0, 0, 1},
                                          {0, 0, 0, 0, 0, 0, 0, 1},
                                          {0, 0, 0, 0, 0, 1, 0, 0},
                                          {1, 0, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 1, 0},
                                          {0, 0, 0, 0, 1, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 0, 1}}),
                                    BMat({{0, 1, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 1, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 1, 0, 0},
                                          {0, 0, 0, 0, 0, 1, 0, 0},
                                          {0, 0, 1, 0, 0, 0, 0, 0},
                                          {0, 1, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 1, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 0, 1}}),
                                    BMat({{1, 0, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 1, 0},
                                          {1, 0, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 1, 0},
                                          {1, 0, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 0, 1},
                                          {0, 0, 0, 0, 0, 1, 0, 0},
                                          {0, 0, 0, 1, 0, 0, 0, 0}}),
                                    BMat({{0, 0, 0, 0, 0, 0, 1, 0},
                                          {1, 0, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 1, 0, 0, 0},
                                          {0, 1, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 1, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 1, 0, 0},
                                          {0, 0, 0, 0, 0, 1, 0, 0},
                                          {0, 0, 0, 0, 1, 0, 0, 0}}),
                                    BMat({{0, 0, 1, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 1, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 0, 1},
                                          {0, 0, 0, 0, 1, 0, 0, 0},
                                          {0, 0, 0, 1, 0, 0, 0, 0},
                                          {1, 0, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 1, 0, 0, 0},
                                          {0, 0, 1, 0, 0, 0, 0, 0}})};

    Konieczny<BMat> S(gens);
    REQUIRE(S.size() == 201750);
#ifdef LIBSEMIGROUPS_HPCOMBI_ENABLED
#pragma GCC diagnostic pop
#endif
  }

  LIBSEMIGROUPS_TEST_CASE("Konieczny",
                          "021",
                          "my favourite example transposed",
                          "[quick][no-valgrind][bmat8]") {
    auto                     rg   = ReportGuard(REPORT);
    const std::vector<BMat8> gens = {BMat8({{0, 0, 0, 0, 1, 0, 0, 0},
                                            {1, 0, 0, 0, 0, 0, 1, 0},
                                            {0, 0, 1, 0, 0, 0, 0, 0},
                                            {0, 0, 0, 0, 0, 0, 0, 0},
                                            {0, 0, 0, 0, 0, 1, 0, 0},
                                            {0, 0, 0, 0, 0, 0, 0, 1},
                                            {0, 0, 0, 1, 0, 0, 0, 0},
                                            {0, 1, 0, 0, 0, 0, 0, 0}}),
                                     BMat8({{0, 0, 0, 0, 0, 0, 0, 0},
                                            {0, 0, 0, 1, 0, 0, 0, 0},
                                            {1, 0, 0, 0, 0, 0, 1, 0},
                                            {0, 0, 0, 0, 0, 0, 0, 0},
                                            {0, 1, 0, 0, 1, 0, 0, 0},
                                            {0, 0, 0, 0, 0, 1, 0, 0},
                                            {0, 0, 1, 0, 0, 0, 0, 0},
                                            {0, 0, 0, 0, 0, 0, 0, 1}}),
                                     BMat8({{0, 1, 0, 0, 0, 0, 0, 0},
                                            {0, 0, 0, 0, 0, 0, 0, 0},
                                            {0, 0, 0, 1, 0, 0, 1, 0},
                                            {1, 0, 0, 0, 0, 0, 0, 0},
                                            {0, 0, 0, 0, 1, 0, 0, 1},
                                            {0, 0, 0, 0, 0, 0, 0, 0},
                                            {0, 0, 0, 0, 0, 1, 0, 0},
                                            {0, 0, 1, 0, 0, 0, 0, 0}}),
                                     BMat8({{0, 0, 0, 0, 0, 0, 1, 0},
                                            {0, 0, 0, 0, 0, 0, 0, 1},
                                            {0, 1, 0, 0, 0, 0, 0, 0},
                                            {1, 0, 1, 0, 0, 1, 0, 0},
                                            {0, 0, 0, 1, 0, 0, 0, 0},
                                            {0, 0, 0, 0, 1, 0, 0, 0},
                                            {0, 0, 0, 0, 0, 0, 0, 0},
                                            {0, 0, 0, 0, 0, 0, 0, 0}}),
                                     BMat8({{0, 0, 0, 0, 0, 0, 1, 0},
                                            {0, 0, 0, 0, 0, 0, 0, 0},
                                            {0, 0, 0, 0, 0, 0, 0, 0},
                                            {0, 1, 0, 0, 0, 0, 0, 0},
                                            {1, 0, 0, 0, 1, 0, 0, 1},
                                            {0, 0, 0, 0, 0, 1, 0, 0},
                                            {0, 0, 0, 0, 0, 0, 0, 0},
                                            {0, 0, 1, 1, 0, 0, 0, 0}}),
                                     BMat8({{0, 0, 0, 1, 0, 1, 0, 0},
                                            {0, 0, 0, 0, 0, 0, 0, 1},
                                            {0, 0, 0, 0, 0, 0, 0, 0},
                                            {0, 0, 1, 0, 1, 0, 0, 0},
                                            {0, 0, 0, 0, 0, 0, 0, 0},
                                            {1, 0, 0, 0, 0, 0, 1, 0},
                                            {0, 1, 0, 0, 0, 0, 0, 0},
                                            {0, 0, 0, 0, 0, 0, 0, 0}}),
                                     BMat8({{0, 1, 0, 0, 0, 0, 0, 0},
                                            {0, 0, 1, 1, 1, 0, 0, 0},
                                            {0, 0, 0, 0, 0, 0, 0, 0},
                                            {0, 0, 0, 0, 0, 0, 1, 0},
                                            {0, 0, 0, 0, 0, 0, 0, 1},
                                            {0, 0, 0, 0, 0, 0, 0, 0},
                                            {1, 0, 0, 0, 0, 1, 0, 0},
                                            {0, 0, 0, 0, 0, 0, 0, 0}}),
                                     BMat8({{0, 0, 0, 1, 0, 0, 0, 0},
                                            {0, 0, 0, 0, 0, 0, 1, 0},
                                            {0, 0, 0, 0, 0, 0, 0, 0},
                                            {0, 0, 0, 0, 0, 0, 0, 0},
                                            {0, 0, 1, 0, 0, 1, 0, 0},
                                            {0, 0, 0, 0, 0, 0, 0, 0},
                                            {0, 0, 0, 0, 1, 0, 0, 0},
                                            {1, 1, 0, 0, 0, 0, 0, 1}})};

    Konieczny<BMat8> S(gens);
    REQUIRE(S.size() == 597369);
  }

  LIBSEMIGROUPS_TEST_CASE("Konieczny",
                          "022",
                          "full bmat monoid 5 with stop in Action",
                          "[extreme][bmat8]") {
#ifdef LIBSEMIGROUPS_HPCOMBI_ENABLED
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#pragma GCC diagnostic ignored "-Winline"
#endif
    using BMat                         = FastestBMat<5>;
    auto                    rg         = ReportGuard(true);
    const std::vector<BMat> bmat5_gens = {BMat({{1, 0, 0, 0, 0},
                                                {0, 1, 0, 0, 0},
                                                {0, 0, 1, 0, 0},
                                                {0, 0, 0, 1, 0},
                                                {0, 0, 0, 0, 1}}),
                                          BMat({{0, 1, 0, 0, 0},
                                                {0, 0, 1, 0, 0},
                                                {0, 0, 0, 1, 0},
                                                {0, 0, 0, 0, 1},
                                                {1, 0, 0, 0, 0}}),
                                          BMat({{0, 1, 0, 0, 0},
                                                {1, 0, 0, 0, 0},
                                                {0, 0, 1, 0, 0},
                                                {0, 0, 0, 1, 0},
                                                {0, 0, 0, 0, 1}}),
                                          BMat({{1, 0, 0, 0, 0},
                                                {0, 1, 0, 0, 0},
                                                {0, 0, 1, 0, 0},
                                                {0, 0, 0, 1, 0},
                                                {1, 0, 0, 0, 1}}),
                                          BMat({{1, 1, 0, 0, 0},
                                                {1, 0, 1, 0, 0},
                                                {0, 1, 0, 1, 0},
                                                {0, 0, 1, 1, 0},
                                                {0, 0, 0, 0, 1}}),
                                          BMat({{1, 1, 0, 0, 0},
                                                {1, 0, 1, 0, 0},
                                                {0, 1, 1, 0, 0},
                                                {0, 0, 0, 1, 0},
                                                {0, 0, 0, 0, 1}}),
                                          BMat({{1, 1, 1, 0, 0},
                                                {1, 0, 0, 1, 0},
                                                {0, 1, 0, 1, 0},
                                                {0, 0, 1, 1, 0},
                                                {0, 0, 0, 0, 1}}),
                                          BMat({{1, 1, 0, 0, 0},
                                                {1, 0, 1, 0, 0},
                                                {0, 1, 0, 1, 0},
                                                {0, 0, 1, 0, 1},
                                                {0, 0, 0, 1, 1}}),
                                          BMat({{1, 1, 1, 1, 0},
                                                {1, 0, 0, 0, 1},
                                                {0, 1, 0, 0, 1},
                                                {0, 0, 1, 0, 1},
                                                {0, 0, 0, 1, 1}}),
                                          BMat({{1, 0, 0, 0, 0},
                                                {0, 1, 0, 0, 0},
                                                {0, 0, 1, 0, 0},
                                                {0, 0, 0, 1, 0},
                                                {0, 0, 0, 0, 0}}),
                                          BMat({{1, 1, 1, 0, 0},
                                                {1, 0, 0, 1, 0},
                                                {0, 1, 0, 1, 0},
                                                {0, 0, 1, 0, 1},
                                                {0, 0, 0, 1, 1}}),
                                          BMat({{1, 1, 1, 0, 0},
                                                {1, 0, 0, 1, 0},
                                                {1, 0, 0, 0, 1},
                                                {0, 1, 0, 1, 0},
                                                {0, 0, 1, 0, 1}}),
                                          BMat({{1, 1, 1, 0, 0},
                                                {1, 0, 0, 1, 1},
                                                {0, 1, 0, 1, 0},
                                                {0, 1, 0, 0, 1},
                                                {0, 0, 1, 1, 0}}),
                                          BMat({{1, 1, 1, 0, 0},
                                                {1, 1, 0, 1, 0},
                                                {1, 0, 0, 0, 1},
                                                {0, 1, 0, 0, 1},
                                                {0, 0, 1, 1, 1}})};

    Konieczny<BMat> T(bmat5_gens);
    T.run_for(std::chrono::milliseconds(100));
    T.run_for(std::chrono::milliseconds(100));
    T.run_for(std::chrono::milliseconds(100));
    T.run_for(std::chrono::milliseconds(100));
    T.run_for(std::chrono::milliseconds(100));
    T.run();
    T.run_for(std::chrono::milliseconds(100));
    T.run_for(std::chrono::milliseconds(100));
    REQUIRE(T.size() == 33554432);
#ifdef LIBSEMIGROUPS_HPCOMBI_ENABLED
#pragma GCC diagnostic pop
#endif
  }

  LIBSEMIGROUPS_TEST_CASE("Konieczny",
                          "023",
                          "regular generated bmat monoid 5 with stops",
                          "[extreme][bmat8]") {
    auto                     rg             = ReportGuard(true);
    const std::vector<BMat8> reg_bmat5_gens = {BMat8({{0, 1, 0, 0, 0},
                                                      {1, 0, 0, 0, 0},
                                                      {0, 0, 1, 0, 0},
                                                      {0, 0, 0, 1, 0},
                                                      {0, 0, 0, 0, 1}}),
                                               BMat8({{0, 1, 0, 0, 0},
                                                      {0, 0, 1, 0, 0},
                                                      {0, 0, 0, 1, 0},
                                                      {0, 0, 0, 0, 1},
                                                      {1, 0, 0, 0, 0}}),
                                               BMat8({{1, 0, 0, 0, 0},
                                                      {1, 1, 0, 0, 0},
                                                      {0, 0, 1, 0, 0},
                                                      {0, 0, 0, 1, 0},
                                                      {0, 0, 0, 0, 1}}),
                                               BMat8({{0, 0, 0, 0, 0},
                                                      {0, 1, 0, 0, 0},
                                                      {0, 0, 1, 0, 0},
                                                      {0, 0, 0, 1, 0},
                                                      {0, 0, 0, 0, 1}})};

    Konieczny<BMat8> T(reg_bmat5_gens);
    T.run_for(std::chrono::milliseconds(4000));
    size_t number_of_classes = T.cend_D_classes() - T.cbegin_D_classes();
    REQUIRE(number_of_classes > 0);
    T.run_for(std::chrono::milliseconds(2000));
    REQUIRE(size_t(T.cend_D_classes() - T.cbegin_D_classes())
            > number_of_classes);

    REQUIRE(T.size() == 32311832);
    REQUIRE(T.number_of_regular_elements() == 8683982);
    REQUIRE(T.number_of_idempotents() == 73023);
  }

  LIBSEMIGROUPS_TEST_CASE("Konieczny", "024", "exceptions", "[quick][bmat8]") {
    auto                     rg    = ReportGuard(REPORT);
    const std::vector<BMat8> gens  = {BMat8({{0, 1, 0}, {0, 0, 1}, {1, 0, 0}}),
                                     BMat8({{0, 1, 0}, {1, 0, 0}, {0, 0, 1}}),
                                     BMat8({{1, 0, 0}, {1, 1, 0}, {0, 0, 1}}),
                                     BMat8({{1, 1, 0}, {0, 1, 1}, {1, 0, 1}})};
    const std::vector<BMat8> idems = {BMat8({{1, 0, 0}, {0, 1, 0}, {0, 0, 1}}),
                                      BMat8({{1, 0, 0}, {1, 1, 0}, {0, 0, 1}}),
                                      BMat8({{1, 0, 0}, {1, 1, 1}, {0, 0, 1}}),
                                      BMat8({{1, 0, 0}, {1, 1, 0}, {1, 0, 1}}),
                                      BMat8({{1, 0, 0}, {1, 1, 0}, {1, 1, 1}}),
                                      BMat8({{1, 1, 0}, {1, 1, 0}, {0, 0, 1}}),
                                      BMat8({{1, 0, 0}, {1, 1, 1}, {1, 1, 1}}),
                                      BMat8({{1, 1, 0}, {1, 1, 0}, {1, 1, 1}}),
                                      BMat8({{1, 1, 1}, {1, 1, 1}, {1, 1, 1}})};
    const std::vector<BMat8> non_reg_reps
        = {BMat8({{0, 0, 1}, {1, 0, 1}, {1, 1, 0}}),
           BMat8({{0, 0, 1}, {1, 1, 1}, {1, 1, 0}}),
           BMat8({{0, 1, 1}, {1, 0, 1}, {1, 1, 1}}),
           BMat8({{0, 1, 1}, {1, 1, 0}, {1, 0, 1}}),
           BMat8({{1, 0, 1}, {1, 0, 1}, {1, 1, 0}}),
           BMat8({{1, 1, 0}, {1, 1, 1}, {1, 1, 1}})};

    const std::vector<BMat8> empty;

    REQUIRE_THROWS_AS(Konieczny<BMat8>(empty), LibsemigroupsException);

    Konieczny<BMat8> S;
    REQUIRE_THROWS_AS(S.run(), LibsemigroupsException);

    Konieczny<BMat8> KS(gens);
    KS.run();

    REQUIRE(size_t(KS.cend_regular_D_classes() - KS.cbegin_regular_D_classes())
            == idems.size());

    REQUIRE_THROWS_AS(KS.add_generators(gens), LibsemigroupsException);
    REQUIRE_THROWS_AS(KS.add_generator(gens[0]), LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("Konieczny",
                          "025",
                          "0-parameter constructor",
                          "[quick][no-valgrind][bmat8]") {
    auto                     rg   = ReportGuard(REPORT);
    const std::vector<BMat8> gens = {BMat8({{0, 1, 0, 0, 0, 0, 0, 0},
                                            {0, 0, 0, 0, 0, 1, 0, 0},
                                            {0, 0, 0, 0, 0, 1, 0, 0},
                                            {0, 0, 0, 0, 1, 0, 0, 0},
                                            {0, 0, 1, 0, 0, 0, 0, 0},
                                            {0, 0, 0, 1, 0, 0, 0, 0},
                                            {0, 0, 0, 0, 0, 0, 1, 0},
                                            {0, 0, 0, 0, 1, 0, 0, 0}}),
                                     BMat8({{0, 0, 0, 1, 0, 0, 0, 0},
                                            {0, 1, 0, 0, 0, 0, 0, 0},
                                            {1, 0, 0, 0, 0, 0, 0, 0},
                                            {0, 0, 0, 0, 0, 1, 0, 0},
                                            {0, 1, 0, 0, 0, 0, 0, 0},
                                            {0, 0, 0, 0, 0, 0, 0, 1},
                                            {0, 0, 1, 0, 0, 0, 0, 0},
                                            {1, 0, 0, 0, 0, 0, 0, 0}}),
                                     BMat8({{0, 0, 0, 1, 0, 0, 0, 0},
                                            {0, 0, 0, 0, 0, 0, 1, 0},
                                            {0, 0, 0, 1, 0, 0, 0, 0},
                                            {0, 0, 0, 0, 0, 0, 1, 0},
                                            {0, 0, 0, 0, 1, 0, 0, 0},
                                            {0, 0, 0, 0, 0, 0, 1, 0},
                                            {0, 0, 0, 0, 0, 1, 0, 0},
                                            {0, 0, 0, 0, 0, 0, 0, 1}}),
                                     BMat8({{0, 0, 0, 0, 0, 0, 0, 1},
                                            {0, 0, 0, 0, 0, 0, 0, 1},
                                            {0, 0, 0, 0, 0, 0, 0, 1},
                                            {0, 0, 0, 0, 0, 1, 0, 0},
                                            {1, 0, 0, 0, 0, 0, 0, 0},
                                            {0, 0, 0, 0, 0, 0, 1, 0},
                                            {0, 0, 0, 0, 1, 0, 0, 0},
                                            {0, 0, 0, 0, 0, 0, 0, 1}}),
                                     BMat8({{0, 1, 0, 0, 0, 0, 0, 0},
                                            {0, 0, 1, 0, 0, 0, 0, 0},
                                            {0, 0, 0, 0, 0, 1, 0, 0},
                                            {0, 0, 0, 0, 0, 1, 0, 0},
                                            {0, 0, 1, 0, 0, 0, 0, 0},
                                            {0, 1, 0, 0, 0, 0, 0, 0},
                                            {0, 0, 0, 1, 0, 0, 0, 0},
                                            {0, 0, 0, 0, 0, 0, 0, 1}}),
                                     BMat8({{1, 0, 0, 0, 0, 0, 0, 0},
                                            {0, 0, 0, 0, 0, 0, 1, 0},
                                            {1, 0, 0, 0, 0, 0, 0, 0},
                                            {0, 0, 0, 0, 0, 0, 1, 0},
                                            {1, 0, 0, 0, 0, 0, 0, 0},
                                            {0, 0, 0, 0, 0, 0, 0, 1},
                                            {0, 0, 0, 0, 0, 1, 0, 0},
                                            {0, 0, 0, 1, 0, 0, 0, 0}}),
                                     BMat8({{0, 0, 0, 0, 0, 0, 1, 0},
                                            {1, 0, 0, 0, 0, 0, 0, 0},
                                            {0, 0, 0, 0, 1, 0, 0, 0},
                                            {0, 1, 0, 0, 0, 0, 0, 0},
                                            {0, 0, 1, 0, 0, 0, 0, 0},
                                            {0, 0, 0, 0, 0, 1, 0, 0},
                                            {0, 0, 0, 0, 0, 1, 0, 0},
                                            {0, 0, 0, 0, 1, 0, 0, 0}}),
                                     BMat8({{0, 0, 1, 0, 0, 0, 0, 0},
                                            {0, 0, 0, 0, 1, 0, 0, 0},
                                            {0, 0, 0, 0, 0, 0, 0, 1},
                                            {0, 0, 0, 0, 1, 0, 0, 0},
                                            {0, 0, 0, 1, 0, 0, 0, 0},
                                            {1, 0, 0, 0, 0, 0, 0, 0},
                                            {0, 0, 0, 0, 1, 0, 0, 0},
                                            {0, 0, 1, 0, 0, 0, 0, 0}})};

    Konieczny<BMat8> S;
    for (auto x : gens) {
      S.add_generator(x);
    }
    S.add_generators(gens);

    REQUIRE(S.size() == 201750);
  }
}  // namespace libsemigroups
