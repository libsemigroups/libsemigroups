//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2025 Finn Smith
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

#include <cstddef>  // for size_t

#include "Catch2-3.8.0/catch_amalgamated.hpp"  // for TEST_CASE, BENCHMARK, REQUIRE

#include "libsemigroups/froidure-pin.hpp"        // for FroidurePin
#include "libsemigroups/konieczny.hpp"           // for Konieczny
#include "libsemigroups/max-plus-trunc-mat.hpp"  // for Lambda, Rho, etc.

namespace libsemigroups {

  template <size_t T>
  std::vector<MaxPlusTruncMat<T, 4>> example1() {
    using Mat = MaxPlusTruncMat<T, 4>;

    std::vector<Mat> gens
        = {Mat({{NEGATIVE_INFINITY, 0, NEGATIVE_INFINITY, NEGATIVE_INFINITY},
                {0, NEGATIVE_INFINITY, NEGATIVE_INFINITY, NEGATIVE_INFINITY},
                {NEGATIVE_INFINITY, NEGATIVE_INFINITY, 0, NEGATIVE_INFINITY},
                {NEGATIVE_INFINITY, NEGATIVE_INFINITY, NEGATIVE_INFINITY, 0}}),
           Mat({{NEGATIVE_INFINITY, 0, NEGATIVE_INFINITY, NEGATIVE_INFINITY},
                {NEGATIVE_INFINITY, NEGATIVE_INFINITY, 0, NEGATIVE_INFINITY},
                {NEGATIVE_INFINITY, NEGATIVE_INFINITY, NEGATIVE_INFINITY, 0},
                {0, NEGATIVE_INFINITY, NEGATIVE_INFINITY, NEGATIVE_INFINITY}}),
           Mat({{0, NEGATIVE_INFINITY, NEGATIVE_INFINITY, NEGATIVE_INFINITY},
                {0, 0, NEGATIVE_INFINITY, NEGATIVE_INFINITY},
                {NEGATIVE_INFINITY, NEGATIVE_INFINITY, 0, NEGATIVE_INFINITY},
                {NEGATIVE_INFINITY, NEGATIVE_INFINITY, NEGATIVE_INFINITY, 0}}),
           Mat({{NEGATIVE_INFINITY,
                 NEGATIVE_INFINITY,
                 NEGATIVE_INFINITY,
                 NEGATIVE_INFINITY},
                {NEGATIVE_INFINITY, 0, NEGATIVE_INFINITY, NEGATIVE_INFINITY},
                {NEGATIVE_INFINITY, NEGATIVE_INFINITY, 0, NEGATIVE_INFINITY},
                {NEGATIVE_INFINITY, NEGATIVE_INFINITY, NEGATIVE_INFINITY, 0}}),
           Mat({{1, 1, 1, 1},
                {NEGATIVE_INFINITY, 1, NEGATIVE_INFINITY, NEGATIVE_INFINITY},
                {NEGATIVE_INFINITY, NEGATIVE_INFINITY, 1, NEGATIVE_INFINITY},
                {NEGATIVE_INFINITY, NEGATIVE_INFINITY, NEGATIVE_INFINITY, 1}})};
    return gens;
  }

  template <size_t T>
  std::vector<MaxPlusTruncMat<T, 5>> example2() {
    using Mat = MaxPlusTruncMat<T, 5>;
    std::vector<Mat> gens
        = {Mat({{NEGATIVE_INFINITY,
                 0,
                 NEGATIVE_INFINITY,
                 NEGATIVE_INFINITY,
                 NEGATIVE_INFINITY},
                {0,
                 NEGATIVE_INFINITY,
                 NEGATIVE_INFINITY,
                 NEGATIVE_INFINITY,
                 NEGATIVE_INFINITY},
                {NEGATIVE_INFINITY,
                 NEGATIVE_INFINITY,
                 0,
                 NEGATIVE_INFINITY,
                 NEGATIVE_INFINITY},
                {NEGATIVE_INFINITY,
                 NEGATIVE_INFINITY,
                 NEGATIVE_INFINITY,
                 0,
                 NEGATIVE_INFINITY},
                {NEGATIVE_INFINITY,
                 NEGATIVE_INFINITY,
                 NEGATIVE_INFINITY,
                 NEGATIVE_INFINITY,
                 0}}),
           Mat({{NEGATIVE_INFINITY,
                 0,
                 NEGATIVE_INFINITY,
                 NEGATIVE_INFINITY,
                 NEGATIVE_INFINITY},
                {NEGATIVE_INFINITY,
                 NEGATIVE_INFINITY,
                 0,
                 NEGATIVE_INFINITY,
                 NEGATIVE_INFINITY},
                {NEGATIVE_INFINITY,
                 NEGATIVE_INFINITY,
                 NEGATIVE_INFINITY,
                 0,
                 NEGATIVE_INFINITY},
                {NEGATIVE_INFINITY,
                 NEGATIVE_INFINITY,
                 NEGATIVE_INFINITY,
                 NEGATIVE_INFINITY,
                 0},
                {0,
                 NEGATIVE_INFINITY,
                 NEGATIVE_INFINITY,
                 NEGATIVE_INFINITY,
                 NEGATIVE_INFINITY}}),
           Mat({{0,
                 NEGATIVE_INFINITY,
                 NEGATIVE_INFINITY,
                 NEGATIVE_INFINITY,
                 NEGATIVE_INFINITY},
                {0, 0, NEGATIVE_INFINITY, NEGATIVE_INFINITY, NEGATIVE_INFINITY},
                {NEGATIVE_INFINITY,
                 NEGATIVE_INFINITY,
                 0,
                 NEGATIVE_INFINITY,
                 NEGATIVE_INFINITY},
                {NEGATIVE_INFINITY,
                 NEGATIVE_INFINITY,
                 NEGATIVE_INFINITY,
                 0,
                 NEGATIVE_INFINITY},
                {NEGATIVE_INFINITY,
                 NEGATIVE_INFINITY,
                 NEGATIVE_INFINITY,
                 NEGATIVE_INFINITY,
                 0}}),
           Mat({{NEGATIVE_INFINITY,
                 NEGATIVE_INFINITY,
                 NEGATIVE_INFINITY,
                 NEGATIVE_INFINITY,
                 NEGATIVE_INFINITY},
                {NEGATIVE_INFINITY,
                 0,
                 NEGATIVE_INFINITY,
                 NEGATIVE_INFINITY,
                 NEGATIVE_INFINITY},
                {NEGATIVE_INFINITY,
                 NEGATIVE_INFINITY,
                 0,
                 NEGATIVE_INFINITY,
                 NEGATIVE_INFINITY},
                {NEGATIVE_INFINITY,
                 NEGATIVE_INFINITY,
                 NEGATIVE_INFINITY,
                 0,
                 NEGATIVE_INFINITY},
                {NEGATIVE_INFINITY,
                 NEGATIVE_INFINITY,
                 NEGATIVE_INFINITY,
                 NEGATIVE_INFINITY,
                 0}}),
           Mat({{1, 1, 1, 1, 1},
                {NEGATIVE_INFINITY,
                 1,
                 NEGATIVE_INFINITY,
                 NEGATIVE_INFINITY,
                 NEGATIVE_INFINITY},
                {NEGATIVE_INFINITY,
                 NEGATIVE_INFINITY,
                 1,
                 NEGATIVE_INFINITY,
                 NEGATIVE_INFINITY},
                {NEGATIVE_INFINITY,
                 NEGATIVE_INFINITY,
                 NEGATIVE_INFINITY,
                 1,
                 NEGATIVE_INFINITY},
                {NEGATIVE_INFINITY,
                 NEGATIVE_INFINITY,
                 NEGATIVE_INFINITY,
                 NEGATIVE_INFINITY,
                 1}})};
    return gens;
  }

  ////////////////////////////////////////////////////////////////////////
  // The actual benchmarks for BMats
  ////////////////////////////////////////////////////////////////////////
  TEST_CASE("Example 1: regular-generated bmat gens - tropical max-plus (dim "
            "4, t 1-10)",
            "[000][standard]") {
    auto rg = ReportGuard(false);

    std::vector<MaxPlusTruncMat<1, 4>>  gens1  = example1<1>();
    std::vector<MaxPlusTruncMat<4, 4>>  gens4  = example1<4>();
    std::vector<MaxPlusTruncMat<7, 4>>  gens7  = example1<7>();
    std::vector<MaxPlusTruncMat<10, 4>> gens10 = example1<10>();

    BENCHMARK("Konieczny: threshold 1") {
      Konieczny S = make<Konieczny>(gens1);
      S.run();
    };
    BENCHMARK("Konieczny: threshold 4") {
      Konieczny S = make<Konieczny>(gens4);
      S.run();
    };
    BENCHMARK("Konieczny: threshold 7") {
      Konieczny S = make<Konieczny>(gens7);
      S.run();
    };
    BENCHMARK("Konieczny: threshold 10") {
      Konieczny S = make<Konieczny>(gens10);
      S.run();
    };

    BENCHMARK("FroidurePin: threshold 1") {
      FroidurePin F = make<FroidurePin>(gens1);
      F.run();
    };
    BENCHMARK("FroidurePin: threshold 4") {
      FroidurePin F = make<FroidurePin>(gens4);
      F.run();
    };
    BENCHMARK("FroidurePin: threshold 7") {
      FroidurePin F = make<FroidurePin>(gens7);
      F.run();
    };
    BENCHMARK("FroidurePin: threshold 10") {
      FroidurePin F = make<FroidurePin>(gens10);
      F.run();
    };
  }

  TEST_CASE("Example 2: regular-generated bmat gens - tropical max-plus (dim "
            "5, t 1-5)",
            "[001][extreme]") {
    // auto rg = ReportGuard(false);

    std::vector<MaxPlusTruncMat<1, 5>> gens1 = example2<1>();
    std::vector<MaxPlusTruncMat<2, 5>> gens2 = example2<2>();
    std::vector<MaxPlusTruncMat<3, 5>> gens3 = example2<3>();
    std::vector<MaxPlusTruncMat<4, 5>> gens4 = example2<4>();

    BENCHMARK("Konieczny: threshold 1") {
      Konieczny S = make<Konieczny>(gens1);
      S.run();
    };

    BENCHMARK("Konieczny: threshold 2") {
      Konieczny S = make<Konieczny>(gens2);
      S.run();
    };

    BENCHMARK("Konieczny: threshold 3") {
      Konieczny S = make<Konieczny>(gens3);
      S.run();
    };

    BENCHMARK("Konieczny: threshold 4") {
      Konieczny S = make<Konieczny>(gens4);
      S.run();
    };

    BENCHMARK("FroidurePin: threshold 1") {
      FroidurePin F = make<FroidurePin>(gens1);
      F.run();
    };
  }
}  // namespace libsemigroups
