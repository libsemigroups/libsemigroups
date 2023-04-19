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

// This file is the second of three containing tests for the fpsemi-examples
// functions. The presentations here define not necessarily finite semigroups,
// and we use KnuthBendix in testing them.

// #define CATCH_CONFIG_ENABLE_PAIR_STRINGMAKER

#include "catch.hpp"      // for REQUIRE, REQUIRE_NOTHROW, REQUIRE_THROWS_AS
#include "test-main.hpp"  // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/fpsemi-examples.hpp"   // for the presentations
#include "libsemigroups/knuth-bendix-new.hpp"  // for KnuthBendix
#include "libsemigroups/obvinf.hpp"            // for is_obviously_infinite
#include "libsemigroups/report.hpp"            // for ReportGuard
#include "libsemigroups/types.hpp"             // for word_type
#include "libsemigroups/words.hpp"             // for literals

namespace libsemigroups {

  using literals::operator""_w;

  struct LibsemigroupsException;

  using fpsemigroup::chinese_monoid;
  using fpsemigroup::plactic_monoid;
  using fpsemigroup::stylic_monoid;

  namespace congruence {
    LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                            "067",
                            "chinese_monoid(3)",
                            "[fpsemi-examples][quick]") {
      auto rg = ReportGuard(false);

      KnuthBendix kb(congruence_kind::twosided, chinese_monoid(3));
      REQUIRE(is_obviously_infinite(kb));
      REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
      REQUIRE(kb.presentation().rules
              == std::vector<std::string>({"baa",
                                           "aba",
                                           "caa",
                                           "aca",
                                           "bba",
                                           "bab",
                                           "cba",
                                           "cab",
                                           "cba",
                                           "bca",
                                           "cca",
                                           "cac",
                                           "cbb",
                                           "bcb",
                                           "ccb",
                                           "cbc"}));
      REQUIRE(knuth_bendix::normal_forms(kb).min(0).max(10).count() == 1'175);
    }

    LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                            "068",
                            "plactic_monoid(3)",
                            "[fpsemi-examples][quick]") {
      auto        rg = ReportGuard(true);
      KnuthBendix kb(congruence_kind::twosided, plactic_monoid(3));
      REQUIRE(kb.presentation().rules
              == std::vector<std::string>({"abc",
                                           "acb",
                                           "bca",
                                           "cba",
                                           "abb",
                                           "bab",
                                           "aab",
                                           "aba",
                                           "cbb",
                                           "bcb",
                                           "ccb",
                                           "cbc",
                                           "caa",
                                           "aca",
                                           "cca",
                                           "cac"}));
      REQUIRE(kb.presentation().alphabet() == "abc");
      REQUIRE(is_obviously_infinite(kb));
      REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
      REQUIRE(knuth_bendix::normal_forms(kb).min(0).max(5).count() == 70);
    }

    LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                            "069",
                            "stylic_monoid(4)",
                            "[fpsemi-examples][quick]") {
      auto        rg = ReportGuard(false);
      KnuthBendix kb(congruence_kind::twosided, stylic_monoid(4));
      REQUIRE(kb.number_of_classes() == 51);
      REQUIRE(knuth_bendix::normal_forms(kb).min(0).max(6).count() == 49);
    }
  }  // namespace congruence
}  // namespace libsemigroups
