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

// This file is the second of two containing tests for the fpsemi-examples
// functions. The presentations here define not necessarily finite semigroups,
// and we use KnuthBendix in testing them.

// #define CATCH_CONFIG_ENABLE_PAIR_STRINGMAKER

#include "catch.hpp"      // for REQUIRE, REQUIRE_NOTHROW, REQUIRE_THROWS_AS
#include "test-main.hpp"  // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/fpsemi-examples.hpp"  // for the presentations
#include "libsemigroups/knuth-bendix.hpp"     // for KnuthBendix
#include "libsemigroups/report.hpp"           // for ReportGuard
#include "libsemigroups/types.hpp"            // for word_type

namespace libsemigroups {
  struct LibsemigroupsException;
  constexpr bool REPORT = false;

  using fpsemigroup::chinese_monoid;
  using fpsemigroup::plactic_monoid;
  using fpsemigroup::stylic_monoid;

  namespace congruence {
    LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                            "028",
                            "chinese_monoid(3)",
                            "[fpsemi-examples][quick]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_number_of_generators(3);
      for (auto const& rel : chinese_monoid(3)) {
        kb.add_pair(rel.first, rel.second);
      }
      REQUIRE(kb.is_quotient_obviously_infinite());
      REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
      REQUIRE(std::vector<relation_type>(kb.cbegin_generating_pairs(),
                                         kb.cend_generating_pairs())
              == std::vector<relation_type>({{{1, 0, 0}, {0, 1, 0}},
                                             {{2, 0, 0}, {0, 2, 0}},
                                             {{1, 1, 0}, {1, 0, 1}},
                                             {{2, 1, 0}, {2, 0, 1}},
                                             {{2, 1, 0}, {1, 2, 0}},
                                             {{2, 2, 0}, {2, 0, 2}},
                                             {{2, 1, 1}, {1, 2, 1}},
                                             {{2, 2, 1}, {2, 1, 2}}}));
      REQUIRE(kb.knuth_bendix().number_of_normal_forms(0, 10) == 1175);
    }

    LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                            "029",
                            "plactic_monoid(3)",
                            "[fpsemi-examples][quick]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_number_of_generators(3);
      for (auto const& rel : plactic_monoid(3)) {
        kb.add_pair(rel.first, rel.second);
      }
      REQUIRE(kb.is_quotient_obviously_infinite());
      REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
      REQUIRE(kb.knuth_bendix().number_of_normal_forms(0, 5) == 70);
    }

    LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                            "030",
                            "stylic_monoid(3)",
                            "[fpsemi-examples][quick]") {
      auto        rg = ReportGuard(REPORT);
      KnuthBendix kb;
      kb.set_number_of_generators(4);
      for (auto const& rel : stylic_monoid(4)) {
        kb.add_pair(rel.first, rel.second);
      }
      REQUIRE(kb.knuth_bendix().number_of_normal_forms(0, 6) == 49);
    }
  }  // namespace congruence
}  // namespace libsemigroups
