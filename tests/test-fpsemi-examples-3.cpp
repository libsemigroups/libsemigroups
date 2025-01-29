// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2022 Murray T. Whyte
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

// This file is the third of three containing tests for the fpsemi-examples
// functions. The tests in this file use Sims1.

// #define CATCH_CONFIG_ENABLE_PAIR_STRINGMAKER

#include <cstddef>  // for size_t

#include "catch_amalgamated.hpp"  // for StringRef, SourceLineInfo
#include "test-main.hpp"          // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/fpsemi-examples.hpp"  // for not_symmetric_group_GKK...
#include "libsemigroups/sims.hpp"             // for Sims1
#include "libsemigroups/types.hpp"            // for tril

#include "libsemigroups/detail/fmt.hpp"     // for format, print
#include "libsemigroups/detail/report.hpp"  // for ReportGuard

namespace libsemigroups {

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "097",
                          "not_symmetric_group(5) GKKL",
                          "[fpsemi-examples][quick]") {
    auto   rg = ReportGuard(false);
    size_t n  = 5;

    Sims1 C;
    C.presentation(presentation::examples::not_symmetric_group_GKKL08(n));

    Sims1 D;
    D.presentation(presentation::examples::symmetric_group_Car56(n));

    REQUIRE(C.number_of_congruences(3) == 41);
    REQUIRE(D.number_of_congruences(3) == 2);
  }
}  // namespace libsemigroups
