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

// Best compiled with: CXXFLAGS="-march=native -mtune=haswell -mssse3
// -ftree-vectorize"

#include <cstddef>  // for size_T

#include "bench-main.hpp"  // for CATCH_CONFIG_ENABLE_BENCHMARKING
#include "catch.hpp"       // for REQUIRE, REQUIRE_NOTHROW, REQUIRE_THROWS_AS

#include "libsemigroups/matrix.hpp"  // for BMat

#include "../tests/bmat-data.hpp"

namespace libsemigroups {
  namespace {
    int myproduct(int x, int y) {
      return x & y;
    }

    int mysum(int x, int y) {
      return x | y;
    }

    void mymatmult(std::vector<int>& res,
                   std::vector<int>& A,
                   std::vector<int>& B) {
      size_t                  m = 40;
      static std::vector<int> colPtr;
      colPtr.resize(40);
      for (size_t c = 0; c < m; c++) {
        for (size_t i = 0; i < m; i++) {
          colPtr[i] = B[i * m + c];
        }
        for (size_t r = 0; r < m; r++) {
          res[r * m + c] = std::inner_product(A.begin() + r * m,
                                              A.begin() + (r + 1) * m,
                                              colPtr.begin(),
                                              0,
                                              mysum,
                                              myproduct);
        }
      }
    }

    std::vector<int> to_vec_1d(std::vector<std::vector<int>> const& x) {
      std::vector<int> vec;
      for (size_t i = 0; i < x.size(); ++i) {
        for (size_t j = 0; j < x[i].size(); ++j) {
          vec.push_back(x[i][j]);
        }
      }
      return vec;
    }
  }  // namespace

  // TEST_CASE("BMat0", "[quick][000]") {
  //   BENCHMARK("redefine") {
  //     BooleanMat result1(size_t(40));
  //     BooleanMat result2 = konieczny_data::clark_gens.back();
  //     REQUIRE(konieczny_data::clark_gens.size() == 6);
  //     for (size_t i = 0; i < 500; ++i) {
  //       for (auto const& y : konieczny_data::clark_gens) {
  //         result1.redefine(result2, y);
  //         std::swap(result1, result2);
  //       }
  //     }
  //   };
  // }

  TEST_CASE("BMat1", "[quick][001]") {
    std::vector<int>              result1(40 * 40, false);
    std::vector<std::vector<int>> clark;
    for (auto const& x : konieczny_data::clark_gens) {
      clark.push_back(to_vec_1d(x));
    }
    std::vector<int> result2 = clark.back();
    REQUIRE(clark.size() == 6);
    REQUIRE(result2.size() == 1600);

    BENCHMARK("inner product direct") {
      for (size_t i = 0; i < 500; ++i) {
        for (auto& y : clark) {
          mymatmult(result1, result2, y);
          std::swap(result1, result2);
        }
      }
    };
  }

  TEST_CASE("BMat2", "[quick][002]") {
    BMat<40>              result1;
    std::vector<BMat<40>> clark;
    for (auto const& x : konieczny_data::clark_gens) {
      clark.emplace_back(x);
    }
    auto result2 = clark.back();
    REQUIRE(clark.size() == 6);

    BENCHMARK("product_inplace static matrix") {
      for (size_t i = 0; i < 500; ++i) {
        for (auto& y : clark) {
          result1.product_inplace(result2, y);
          std::swap(result1, result2);
        }
      }
    };
  }

  TEST_CASE("BMat3", "[quick][003]") {
    BMat<>              result1(40, 40);
    std::vector<BMat<>> clark;
    for (auto const& x : konieczny_data::clark_gens) {
      clark.emplace_back(x);
    }
    auto result2 = clark.back();
    REQUIRE(clark.size() == 6);

    BENCHMARK("product_inplace dynamic matrix") {
      for (size_t i = 0; i < 500; ++i) {
        for (auto& y : clark) {
          result1.product_inplace(result2, y);
          std::swap(result1, result2);
        }
      }
    };
  }

  TEST_CASE("BMat4", "[quick][004]") {
    auto m = BMat<>::identity(16);
    BENCHMARK("rowspace size") {
      REQUIRE(matrix_helpers::row_space_size(m) == 65535);
    };
  }
}  // namespace libsemigroups
