//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2022 James D. Mitchell
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

#include <array>     // for array
#include <chrono>    // for duration, seconds
#include <cmath>     // for pow
#include <cstddef>   // for size_t
#include <cstdint>   // for uint64_t
#include <iostream>  // for operator<<, cout, ostream
#include <memory>    // for allocator, shared_ptr, sha...
#include <string>    // for operator+, basic_string
#include <vector>    // for vector, operator==

#include "bench-main.hpp"  // for Benchmark, SourceLineInfo
#include "catch.hpp"       // for REQUIRE, REQUIRE_NOTHROW, REQUIRE_THROWS_AS

#include "libsemigroups/cong-intf.hpp"  // for congruence_kind, congruenc...
#include "libsemigroups/cong-wrap.hpp"  // for CongruenceWrapper
#include "libsemigroups/constants.hpp"  // for PositiveInfinity, POSITIVE...
#include "libsemigroups/fastest-bmat.hpp"
#include "libsemigroups/fpsemi-examples.hpp"  // for singular_brauer_monoid, ...
#include "libsemigroups/froidure-pin.hpp"     // for FroidurePinBase
#include "libsemigroups/make-present.hpp"     // for make
#include "libsemigroups/report.hpp"           // for ReportGuard
#include "libsemigroups/sims1.hpp"            // for ReportGuard
#include "libsemigroups/transf.hpp"           // for ReportGuard
#include "libsemigroups/types.hpp"            // for word_type, letter_type

#include "examples/cong-intf.hpp"
#include "examples/fpsemi-intf.hpp"

namespace libsemigroups {

  using Sims1_ = Sims1<uint32_t>;

  using fpsemigroup::make;

  using fpsemigroup::rook_monoid;
  using fpsemigroup::singular_brauer_monoid;

  TEST_CASE("POI(3) from FroidurePin", "[POI3][Sim1][quick][talk]") {
    auto                  rg = ReportGuard(false);
    FroidurePin<PPerm<3>> S;
    S.add_generator(PPerm<3>::make({0, 1, 2}, {0, 1, 2}, 3));
    S.add_generator(PPerm<3>::make({1, 2}, {0, 1}, 3));
    S.add_generator(PPerm<3>::make({0, 1}, {0, 2}, 3));
    S.add_generator(PPerm<3>::make({0, 2}, {1, 2}, 3));
    S.add_generator(PPerm<3>::make({0, 1}, {1, 2}, 3));
    S.add_generator(PPerm<3>::make({0, 2}, {0, 1}, 3));
    S.add_generator(PPerm<3>::make({1, 2}, {0, 2}, 3));
    REQUIRE(S.size() == 20);

    auto p = to_presentation<word_type>(S);

    BENCHMARK("Right congruences") {
      Sims1_ C(congruence_kind::right, p);
      REQUIRE(C.number_of_congruences(20) == 99);
    };
    BENCHMARK("Left congruences") {
      Sims1_ C(congruence_kind::left, p);
      REQUIRE(C.number_of_congruences(20) == 99);
    };
  }

  TEST_CASE("POI(4) from FroidurePin", "[POI4][Sim1][standard]") {
    auto                  rg = ReportGuard(false);
    FroidurePin<PPerm<4>> S;
    S.add_generator(PPerm<4>::make({0, 1, 2, 3}, {0, 1, 2, 3}, 4));
    S.add_generator(PPerm<4>::make({1, 2, 3}, {0, 1, 2}, 4));
    S.add_generator(PPerm<4>::make({0, 1, 2}, {0, 1, 3}, 4));
    S.add_generator(PPerm<4>::make({0, 1, 3}, {0, 2, 3}, 4));
    S.add_generator(PPerm<4>::make({0, 2, 3}, {1, 2, 3}, 4));
    S.add_generator(PPerm<4>::make({0, 1, 2}, {1, 2, 3}, 4));
    S.add_generator(PPerm<4>::make({0, 1, 3}, {0, 1, 2}, 4));
    S.add_generator(PPerm<4>::make({0, 2, 3}, {0, 1, 3}, 4));
    S.add_generator(PPerm<4>::make({1, 2, 3}, {0, 2, 3}, 4));
    REQUIRE(S.size() == 70);

    auto p = to_presentation<word_type>(S);

    BENCHMARK("Right congruences") {
      Sims1_ C(congruence_kind::right, p);
      REQUIRE(C.number_of_congruences(70) == 8'146);
    };
  }

  TEST_CASE("ReflexiveBooleanMatMonoid(3) from FroidurePin",
            "[ReflexiveBooleanMatMonoid3][Sim1][fail]") {
    auto                        rg = ReportGuard(true);
    FroidurePin<FastestBMat<3>> S;
    S.add_generator(FastestBMat<3>::one(3));
    S.add_generator(FastestBMat<3>({{1, 1, 0}, {0, 1, 1}, {1, 0, 1}}));
    S.add_generator(FastestBMat<3>({{1, 1, 0}, {0, 1, 0}, {0, 0, 1}}));
    S.add_generator(FastestBMat<3>({{1, 0, 1}, {1, 1, 0}, {0, 1, 1}}));
    S.add_generator(FastestBMat<3>({{1, 0, 1}, {0, 1, 0}, {0, 0, 1}}));
    S.add_generator(FastestBMat<3>({{1, 0, 0}, {1, 1, 0}, {0, 0, 1}}));
    S.add_generator(FastestBMat<3>({{1, 0, 0}, {0, 1, 1}, {0, 0, 1}}));
    S.add_generator(FastestBMat<3>({{1, 0, 0}, {0, 1, 0}, {1, 0, 1}}));
    S.add_generator(FastestBMat<3>({{1, 0, 0}, {0, 1, 0}, {0, 1, 1}}));
    REQUIRE(S.size() == 64);

    auto p = to_presentation<word_type>(S);
    BENCHMARK("Right congruences") {
      Sims1_ C(congruence_kind::right, p);
      REQUIRE(C.number_of_congruences(S.size()) == 7);
    };
    BENCHMARK("Left congruences") {
      Sims1_ C(congruence_kind::left, p);
      REQUIRE(C.number_of_congruences(S.size()) == 7);
    };
  }

  TEST_CASE("singular_brauer_monoid(3) (Maltcev-Mazorchuk)",
            "[talk][singular_brauer_monoid3]") {
    auto rg = ReportGuard(false);
    auto p  = to_presentation<word_type>(singular_brauer_monoid(3));
    REQUIRE(p.rules.size() == 48);

    BENCHMARK("Right congruences") {
      Sims1_ C(congruence_kind::right, p);
      REQUIRE(C.number_of_congruences(9) == 205);
    };
    BENCHMARK("Left congruences") {
      Sims1_ C(congruence_kind::left, p);
      REQUIRE(C.number_of_congruences(9) == 205);
    };
  }

  TEST_CASE("singular_brauer_monoid(4) (Maltcev-Mazorchuk)",
            "[talk][singular_brauer_monoid4]") {
    auto rg = ReportGuard(true);
    auto p  = to_presentation<word_type>(singular_brauer_monoid(4));
    REQUIRE(presentation::length(p) == 660);
    presentation::remove_duplicate_rules(p);
    REQUIRE(presentation::length(p) == 600);
    presentation::sort_each_rule(p);
    presentation::sort_rules(p);
    REQUIRE(p.rules.size() == 252);
    // presentation::remove_redundant(p);

    BENCHMARK("Right congruences") {
      Sims1_ C(congruence_kind::right, p);
      C.split_at((252 - 64) / 2);
      REQUIRE(C.number_of_congruences(81) == 601'265);
    };
  }

  TEST_CASE("symmetric_inverse_monoid(2) (Hivert)",
            "[talk][symmetric_inverse_monoid2]") {
    auto rg = ReportGuard(true);
    auto p  = to_presentation<word_type>(rook_monoid(2, 1));
    presentation::remove_duplicate_rules(p);
    presentation::sort_each_rule(p);
    presentation::sort_rules(p);

    BENCHMARK("Right congruences") {
      Sims1_ C(congruence_kind::right, p);
      REQUIRE(C.number_of_congruences(7) == 10);
    };
  }

  TEST_CASE("symmetric_inverse_monoid(3) (Hivert)",
            "[talk][symmetric_inverse_monoid3]") {
    auto rg = ReportGuard(true);
    auto p  = to_presentation<word_type>(rook_monoid(3, 1));
    presentation::remove_duplicate_rules(p);
    presentation::sort_each_rule(p);
    presentation::sort_rules(p);

    BENCHMARK("Right congruences") {
      Sims1_ C(congruence_kind::right, p);
      REQUIRE(C.number_of_congruences(34) == 274);
    };
  }

  TEST_CASE("symmetric_inverse_monoid(4) (Hivert)",
            "[talk][symmetric_inverse_monoid4]") {
    auto rg = ReportGuard(false);
    auto p  = to_presentation<word_type>(rook_monoid(4, 1));
    presentation::remove_duplicate_rules(p);
    presentation::sort_each_rule(p);
    presentation::sort_rules(p);

    BENCHMARK("Right congruences") {
      Sims1_ C(congruence_kind::right, p);
      REQUIRE(C.number_of_threads(std::thread::hardware_concurrency())
                  .number_of_congruences(209)
              == 195'709);
    };
  }

}  // namespace libsemigroups
