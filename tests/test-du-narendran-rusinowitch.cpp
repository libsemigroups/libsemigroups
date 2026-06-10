//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2020-2026 James D. Mitchell
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

#include "test-main.hpp"  // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/du-narendran-rusinowitch.hpp"  // for du_...

#include "libsemigroups/detail/report.hpp"  // for ReportGuard

namespace libsemigroups {

  LIBSEMIGROUPS_TEST_CASE("du_narendran_rusinowitch",
                          "000",
                          "TODO",
                          "[quick]") {
    Presentation<std::string> p;
    p.alphabet("abcde");
    p.rules = {"dbcbace", "cbbaec", "bcbad", "badbc"};

    REQUIRE(du_narendran_rusinowitch(p) == "bacde");
  }
}  // namespace libsemigroups
