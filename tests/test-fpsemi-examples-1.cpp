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

// This file is the first of three containing tests for the fpsemi-examples
// functions. The presentations here define finite semigroups, and we use
// ToddCoxeter in testing them. Exceptions and defaults are also checked in this
// file.

// #define CATCH_CONFIG_ENABLE_PAIR_STRINGMAKER

#include <cmath>                              // for pow
#include <cstddef>                            // for size_t
#include <unordered_map>                      // for operator!=
#include <vector>                             // for vector, allocator, oper...
                                              //
#include "catch_amalgamated.hpp"              // for StringRef, SourceLineInfo
#include "test-main.hpp"                      // for LIBSEMIGROUPS_TEST_CASE
                                              //
#include "libsemigroups/constants.hpp"        // for operator!=
#include "libsemigroups/exception.hpp"        // for LibsemigroupsException
#include "libsemigroups/fpsemi-examples.hpp"  // for author, symmetric_group
#include "libsemigroups/froidure-pin.hpp"     // for FroidurePin
#include "libsemigroups/obvinf.hpp"           // for is_obviously_infinite
#include "libsemigroups/presentation.hpp"     // for Presentation, operator==
#include "libsemigroups/ranges.hpp"           // for operator|, chain
#include "libsemigroups/to-froidure-pin.hpp"  // for to_froidure_pin
#include "libsemigroups/todd-coxeter.hpp"     // for ToddCoxeter
#include "libsemigroups/types.hpp"            // for congruence_kind, word_type
#include "libsemigroups/word-range.hpp"       // for operator""_w

#include "libsemigroups/detail/fmt.hpp"  // for format, print                                           //
#include "libsemigroups/detail/report.hpp"  // for ReportGuard

namespace libsemigroups {
  using literals::operator""_w;
  struct LibsemigroupsException;
  constexpr bool REPORT = false;

  using namespace fpsemigroup;

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "000",
                          "test semigroup/monoid status",
                          "[fpsemi-examples][quick]") {
    auto rg = ReportGuard(REPORT);
    REQUIRE(stellar_monoid_GH19(5).contains_empty_word());
    REQUIRE(dual_symmetric_inverse_monoid(5).contains_empty_word());
    REQUIRE(uniform_block_bijection_monoid(5).contains_empty_word());
    REQUIRE(partition_monoid_Eas11(5).contains_empty_word());
    REQUIRE(partition_monoid_HR05(5).contains_empty_word());
    REQUIRE(!singular_brauer_monoid(5).contains_empty_word());
    REQUIRE(orientation_preserving_monoid(5).contains_empty_word());
    REQUIRE(orientation_preserving_reversing_monoid(5).contains_empty_word());
    REQUIRE(temperley_lieb_monoid(5).contains_empty_word());
    REQUIRE(brauer_monoid(5).contains_empty_word());
    REQUIRE(partial_brauer_monoid(5).contains_empty_word());
    REQUIRE(motzkin_monoid(5).contains_empty_word());
    REQUIRE(!fibonacci_semigroup(5, 2).contains_empty_word());
    REQUIRE(plactic_monoid(5).contains_empty_word());
    REQUIRE(stylic_monoid(5).contains_empty_word());
    REQUIRE(symmetric_group_Bur12(5).contains_empty_word());
    REQUIRE(symmetric_group_Car56(5).contains_empty_word());
    REQUIRE(symmetric_group_Moo97_a(5).contains_empty_word());
    REQUIRE(symmetric_group_Moo97_b(5).contains_empty_word());
    REQUIRE(alternating_group(5).contains_empty_word());
    REQUIRE(!rectangular_band(5, 5).contains_empty_word());
    REQUIRE(full_transformation_monoid_II74(5).contains_empty_word());
    REQUIRE(full_transformation_monoid_Aiz58(5).contains_empty_word());
    REQUIRE(full_transformation_monoid_MW24_a(5).contains_empty_word());
    REQUIRE(full_transformation_monoid_MW24_b(5).contains_empty_word());
    REQUIRE(partial_transformation_monoid_Shu60(5).contains_empty_word());
    REQUIRE(partial_transformation_monoid_MW24(5).contains_empty_word());
    REQUIRE(symmetric_inverse_monoid(5).contains_empty_word());
    REQUIRE(chinese_monoid_CEKNH01(5).contains_empty_word());
    REQUIRE(monogenic_semigroup(0, 5).contains_empty_word());
    REQUIRE(!monogenic_semigroup(2, 6).contains_empty_word());
    REQUIRE(order_preserving_monoid(5).contains_empty_word());
    REQUIRE(cyclic_inverse_monoid_Fer22_a(5).contains_empty_word());
    REQUIRE(cyclic_inverse_monoid_Fer22_b(5).contains_empty_word());
    REQUIRE(
        order_preserving_cyclic_inverse_monoid_Fer22(5).contains_empty_word());
    REQUIRE(
        partial_isometries_cycle_graph_monoid_FP22(5).contains_empty_word());
    REQUIRE(not_symmetric_group_GKKL08(5).contains_empty_word());
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "001",
                          "full_transformation_monoid degree except",
                          "[fpsemi-examples][quick]") {
    auto rg = ReportGuard(REPORT);
    REQUIRE_THROWS_AS(full_transformation_monoid_Aiz58(1),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(full_transformation_monoid_II74(3),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(full_transformation_monoid_MW24_a(1),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(full_transformation_monoid_MW24_b(1),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(full_transformation_monoid_MW24_b(2),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(full_transformation_monoid_MW24_b(6),
                      LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "002",
                          "partial_transformation_monoid degree except",
                          "[fpsemi-examples][quick]") {
    auto rg = ReportGuard(REPORT);
    REQUIRE_THROWS_AS(partial_transformation_monoid_Shu60(3),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(partial_transformation_monoid_MW24(1),
                      LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "003",
                          "symmetric_group degree except",
                          "[fpsemi-examples][quick]") {
    auto rg = ReportGuard(REPORT);
    REQUIRE_THROWS_AS(symmetric_group_Bur12(1), LibsemigroupsException);
    REQUIRE_THROWS_AS(symmetric_group_Car56(1), LibsemigroupsException);
    REQUIRE_THROWS_AS(symmetric_group_Moo97_a(1), LibsemigroupsException);
    REQUIRE_THROWS_AS(symmetric_group_Moo97_b(1), LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "004",
                          "dual_symmetric_inverse_monoid degree except",
                          "[fpsemi-examples][quick]") {
    auto rg = ReportGuard(REPORT);
    REQUIRE_THROWS_AS(dual_symmetric_inverse_monoid(2), LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "005",
                          "uniform_block_bijection_monoid degree except",
                          "[fpsemi-examples][quick]") {
    auto rg = ReportGuard(REPORT);
    REQUIRE_THROWS_AS(uniform_block_bijection_monoid(2),
                      LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "006",
                          "partition_monoid degree except",
                          "[fpsemi-examples][quick]") {
    auto rg = ReportGuard(REPORT);
    REQUIRE_THROWS_AS(partition_monoid_Eas11(3), LibsemigroupsException);
    REQUIRE_THROWS_AS(partition_monoid_HR05(0), LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "007",
                          "alternating_group degree except",
                          "[fpsemi-examples][quick]") {
    auto rg = ReportGuard(REPORT);
    REQUIRE_THROWS_AS(alternating_group(3), LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "008",
                          "chinese_monoid degree except",
                          "[fpsemi-examples][quick]") {
    auto rg = ReportGuard(REPORT);
    REQUIRE_THROWS_AS(chinese_monoid(1), LibsemigroupsException);
    REQUIRE_THROWS_AS(chinese_monoid(0), LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "009",
                          "monogenic_semigroup degree except",
                          "[fpsemi-examples][quick]") {
    auto rg = ReportGuard(REPORT);
    REQUIRE_THROWS_AS(monogenic_semigroup(0, 0), LibsemigroupsException);
    REQUIRE_THROWS_AS(monogenic_semigroup(5, 0), LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "010",
                          "rectangular_band degree except",
                          "[fpsemi-examples][quick]") {
    auto rg = ReportGuard(REPORT);
    REQUIRE_THROWS_AS(rectangular_band(0, 0), LibsemigroupsException);
    REQUIRE_THROWS_AS(rectangular_band(1, 0), LibsemigroupsException);
    REQUIRE_THROWS_AS(rectangular_band(0, 1), LibsemigroupsException);
    REQUIRE_THROWS_AS(rectangular_band(0, 100), LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "011",
                          "stellar_monoid degree except",
                          "[fpsemi-examples][quick]") {
    auto rg = ReportGuard(REPORT);
    REQUIRE_THROWS_AS(stellar_monoid(0), LibsemigroupsException);
    REQUIRE_THROWS_AS(stellar_monoid(1), LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "012",
                          "plactic_monoid degree except",
                          "[fpsemi-examples][quick]") {
    auto rg = ReportGuard(REPORT);
    REQUIRE_THROWS_AS(plactic_monoid(0), LibsemigroupsException);
    REQUIRE_NOTHROW(plactic_monoid(1));
    REQUIRE_NOTHROW(plactic_monoid(2));
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "013",
                          "stylic_monoid degree except",
                          "[fpsemi-examples][quick]") {
    auto rg = ReportGuard(REPORT);
    REQUIRE_THROWS_AS(stylic_monoid(0), LibsemigroupsException);
    REQUIRE_THROWS_AS(stylic_monoid(1), LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "014",
                          "temperley_lieb_monoid degree except",
                          "[fpsemi-examples][quick]") {
    auto rg = ReportGuard(REPORT);
    REQUIRE_THROWS_AS(temperley_lieb_monoid(0), LibsemigroupsException);
    REQUIRE_THROWS_AS(temperley_lieb_monoid(1), LibsemigroupsException);
    REQUIRE_THROWS_AS(temperley_lieb_monoid(2), LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "015",
                          "singular_brauer_monoid degree except",
                          "[fpsemi-examples][quick]") {
    auto rg = ReportGuard(REPORT);
    REQUIRE_THROWS_AS(singular_brauer_monoid(0), LibsemigroupsException);
    REQUIRE_THROWS_AS(singular_brauer_monoid(1), LibsemigroupsException);
    REQUIRE_THROWS_AS(singular_brauer_monoid(2), LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "016",
                          "orientation_preserving_monoid degree except",
                          "[fpsemi-examples][quick]") {
    auto rg = ReportGuard(REPORT);
    REQUIRE_THROWS_AS(orientation_preserving_monoid(0), LibsemigroupsException);
    REQUIRE_THROWS_AS(orientation_preserving_monoid(1), LibsemigroupsException);
    REQUIRE_THROWS_AS(orientation_preserving_monoid(2), LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE(
      "fpsemi-examples",
      "017",
      "orientation_preserving_reversing_monoid degree except",
      "[fpsemi-examples][quick]") {
    auto rg = ReportGuard(REPORT);
    REQUIRE_THROWS_AS(orientation_preserving_reversing_monoid(0),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(orientation_preserving_reversing_monoid(1),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(orientation_preserving_reversing_monoid(2),
                      LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "018",
                          "order_preserving_monoid degree except",
                          "[fpsemi-examples][quick]") {
    auto rg = ReportGuard(REPORT);
    REQUIRE_THROWS_AS(order_preserving_monoid(0), LibsemigroupsException);
    REQUIRE_THROWS_AS(order_preserving_monoid(1), LibsemigroupsException);
    REQUIRE_THROWS_AS(order_preserving_monoid(2), LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "019",
                          "cyclic_inverse_monoid degree except",
                          "[fpsemi-examples][quick]") {
    auto rg = ReportGuard(REPORT);
    REQUIRE_THROWS_AS(cyclic_inverse_monoid_Fer22_a(0), LibsemigroupsException);
    REQUIRE_THROWS_AS(cyclic_inverse_monoid_Fer22_a(1), LibsemigroupsException);
    REQUIRE_THROWS_AS(cyclic_inverse_monoid_Fer22_a(2), LibsemigroupsException);
    REQUIRE_THROWS_AS(cyclic_inverse_monoid_Fer22_b(0), LibsemigroupsException);
    REQUIRE_THROWS_AS(cyclic_inverse_monoid_Fer22_b(1), LibsemigroupsException);
    REQUIRE_THROWS_AS(cyclic_inverse_monoid_Fer22_b(2), LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE(
      "fpsemi-examples",
      "020",
      "order_preserving_cyclic_inverse_monoid degree except",
      "[fpsemi-examples][quick]") {
    auto rg = ReportGuard(REPORT);
    REQUIRE_THROWS_AS(order_preserving_cyclic_inverse_monoid(0),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(order_preserving_cyclic_inverse_monoid(1),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(order_preserving_cyclic_inverse_monoid(2),
                      LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "021",
                          "partial_isometries_cycle_graph_monoid degree except",
                          "[fpsemi-examples][quick]") {
    auto rg = ReportGuard(REPORT);
    REQUIRE_THROWS_AS(partial_isometries_cycle_graph_monoid(0),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(partial_isometries_cycle_graph_monoid(1),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(partial_isometries_cycle_graph_monoid(2),
                      LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "022",
                          "order_preserving_monoid(5)",
                          "[fpsemi-examples][quick]") {
    auto   rg = ReportGuard(REPORT);
    size_t n  = 5;
    auto   p  = order_preserving_monoid(n);
    p.validate();

    ToddCoxeter tc(congruence_kind::twosided, p);
    REQUIRE(tc.number_of_classes() == 126);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "023",
                          "order_preserving_monoid nr generators and relations",
                          "[fpsemi-examples][quick]") {
    auto                    rg    = ReportGuard(REPORT);
    size_t                  min_n = 3;
    size_t                  max_n = 10;
    Presentation<word_type> p;
    for (size_t n = min_n; n <= max_n; ++n) {
      p = order_preserving_monoid_AR00(n);
      REQUIRE(p.alphabet().size() == 2 * n - 2);
      REQUIRE(p.rules.size() == 2 * n * n);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "024",
                          "monogenic_semigroup(4, 9)",
                          "[fpsemi-examples][quick]") {
    auto rg = ReportGuard(REPORT);
    auto p  = monogenic_semigroup(4, 9);
    p.validate();
    ToddCoxeter tc(congruence_kind::twosided, p);
    REQUIRE(tc.number_of_classes() == 12);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "025",
                          "dual_symmetric_inverse_monoid(5)",
                          "[fpsemi-examples][quick][no-valgrind]") {
    auto   rg = ReportGuard(REPORT);
    size_t n  = 5;
    auto   p  = dual_symmetric_inverse_monoid(n);
    p.validate();
    ToddCoxeter tc(congruence_kind::twosided, p);
    REQUIRE(tc.number_of_classes() == 6'721);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "026",
                          "uniform_block_bijection_monoid(5)",
                          "[fpsemi-examples][quick][no-valgrind]") {
    auto   rg = ReportGuard(REPORT);
    size_t n  = 5;
    auto   p  = uniform_block_bijection_monoid(n);
    p.validate();
    ToddCoxeter tc(congruence_kind::twosided, p);
    REQUIRE(tc.number_of_classes() == 1'496);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "027",
                          "partition_monoid(5)",
                          "[fpsemi-examples][standard]") {
    auto        rg = ReportGuard(REPORT);
    ToddCoxeter tc(congruence_kind::twosided, partition_monoid_Eas11(5));
    REQUIRE(tc.number_of_classes() == 115'975);
    tc.init(congruence_kind::twosided, partition_monoid_HR05(5));
    REQUIRE(tc.number_of_classes() == 115'975);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "028",
                          "singular_brauer_monoid(6)",
                          "[fpsemi-examples][quick][no-valgrind]") {
    auto        rg = ReportGuard(REPORT);
    size_t      n  = 6;
    ToddCoxeter tc(congruence_kind::twosided, singular_brauer_monoid(n));
    REQUIRE(tc.number_of_classes() == 9'675);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "029",
                          "orientation_preserving_monoid(6)",
                          "[fpsemi-examples][quick][no-valgrind]") {
    auto        rg = ReportGuard(REPORT);
    size_t      n  = 6;
    ToddCoxeter tc(congruence_kind::twosided, orientation_preserving_monoid(n));
    REQUIRE(tc.number_of_classes() == 2'742);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "030",
                          "orientation_preserving_reversing_monoid(5)",
                          "[fpsemi-examples][quick][no-valgrind]") {
    auto        rg = ReportGuard(REPORT);
    size_t      n  = 5;
    ToddCoxeter tc(congruence_kind::twosided,
                   orientation_preserving_reversing_monoid(n));
    REQUIRE(tc.number_of_classes() == 1'015);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "031",
                          "partition_monoid(n), 1 <= n <= 3",
                          "[fpsemi-examples][quick]") {
    auto        rg = ReportGuard(REPORT);
    ToddCoxeter tc(congruence_kind::twosided, partition_monoid_HR05(3));
    REQUIRE(tc.number_of_classes() == 203);
    tc.init(congruence_kind::twosided, partition_monoid_HR05(2));
    REQUIRE(tc.number_of_classes() == 15);
    tc.init(congruence_kind::twosided, partition_monoid_HR05(1));
    REQUIRE(tc.number_of_classes() == 2);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "032",
                          "temperley_lieb_monoid(10)",
                          "[fpsemi-examples][quick][no-valgrind]") {
    auto        rg = ReportGuard(REPORT);
    size_t      n  = 10;
    ToddCoxeter tc(congruence_kind::twosided, temperley_lieb_monoid(n));
    REQUIRE(tc.number_of_classes() == 16'796);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "033",
                          "brauer_monoid(5)",
                          "[fpsemi-examples][quick][no-valgrind]") {
    auto        rg = ReportGuard(REPORT);
    size_t      n  = 5;
    ToddCoxeter tc(congruence_kind::twosided, brauer_monoid(n));
    REQUIRE(tc.number_of_classes() == 945);

    tc.init(congruence_kind::twosided, brauer_monoid(1));
    REQUIRE(tc.number_of_classes() == 1);

    tc.init(congruence_kind::twosided, brauer_monoid(2));
    REQUIRE(tc.number_of_classes() == 3);

    tc.init(congruence_kind::twosided, brauer_monoid(3));
    REQUIRE(tc.number_of_classes() == 15);

    tc.init(congruence_kind::twosided, brauer_monoid(4));
    REQUIRE(tc.number_of_classes() == 105);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "034",
                          "rectangular_band(5, 9)",
                          "[fpsemi-examples][quick]") {
    auto        rg = ReportGuard(REPORT);
    ToddCoxeter tc(congruence_kind::twosided, rectangular_band(5, 9));
    REQUIRE(tc.number_of_classes() == 45);
  }

  LIBSEMIGROUPS_TEST_CASE(
      "fpsemi-examples",
      "035",
      "symmetric_group nr generators, relations and classes",
      "[fpsemi-examples][quick][no-valgrind]") {
    auto                               rg = ReportGuard(REPORT);
    Presentation<word_type>            p;
    ToddCoxeter                        tc;
    size_t                             min_n = 2;
    size_t                             max_n = 8;
    std::unordered_map<size_t, size_t> factorial;
    size_t                             current_factorial = 1;
    for (size_t n = 1; n < max_n; ++n) {
      current_factorial *= n;
      if (n >= min_n) {
        factorial[n] = current_factorial;
      }
    }
    for (size_t n = min_n; n < max_n; ++n) {
      p = symmetric_group_Bur12(n);
      REQUIRE(p.alphabet().size() == n - 1);
      REQUIRE(p.rules.size() == 2 * (n * n * n - 5 * n * n + 9 * n - 5));
      tc = ToddCoxeter(congruence_kind::twosided, p);
      REQUIRE(tc.number_of_classes() == factorial[n]);
    }
    for (size_t n = min_n; n < max_n; ++n) {
      p = symmetric_group_Car56(n);
      REQUIRE(p.alphabet().size() == n - 1);
      REQUIRE(p.rules.size() == 2 * (n - 1) * (n - 1));
      tc = ToddCoxeter(congruence_kind::twosided, p);
      REQUIRE(tc.number_of_classes() == factorial[n]);
    }
    for (size_t n = min_n; n < max_n; ++n) {
      p = symmetric_group_Moo97_a(n);
      REQUIRE(p.alphabet().size() == n - 1);
      REQUIRE(p.rules.size() == n * (n - 1));
      tc = ToddCoxeter(congruence_kind::twosided, p);
      REQUIRE(tc.number_of_classes() == factorial[n]);
    }
    for (size_t n = min_n; n < max_n; ++n) {
      p = symmetric_group_Moo97_b(n);
      REQUIRE(p.alphabet().size() == 2);
      if (n < 4) {
        REQUIRE(p.rules.size() == 2 * 4);
      } else {
        REQUIRE(p.rules.size() == 2 * (n + 1));
      }
      tc = ToddCoxeter(congruence_kind::twosided, p);
      REQUIRE(tc.number_of_classes() == factorial[n]);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "036",
                          "symmetric inverse monoid Gay presentation",
                          "[fpsemi-examples][quick]") {
    auto rg = ReportGuard(REPORT);
    auto p  = fpsemigroup::symmetric_inverse_monoid_Gay18(4);

    presentation::sort_each_rule(p);
    presentation::sort_rules(p);

    REQUIRE(p.rules
            == std::vector<word_type>(
                {00_w,  {},    11_w,   {},    22_w,   {},    33_w,  3_w,
                 20_w,  02_w,  31_w,   13_w,  32_w,   23_w,  101_w, 010_w,
                 212_w, 121_w, 0303_w, 303_w, 3030_w, 0303_w}));

    ToddCoxeter tc(congruence_kind::twosided, p);
    REQUIRE(tc.number_of_classes() == 209);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "037",
                          "0-Hecke rook monoid",
                          "[fpsemi-examples][quick]") {
    auto rg = ReportGuard(REPORT);
    auto p  = zero_rook_monoid(4);

    presentation::sort_each_rule(p);
    presentation::sort_rules(p);

    ToddCoxeter tc(congruence_kind::twosided, p);
    REQUIRE(tc.number_of_classes() == 209);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "038",
                          "full_transformation_monoid(5) Iwahori",
                          "[fpsemi-examples][quick][no-valgrind]") {
    auto   rg = ReportGuard(REPORT);
    size_t n  = 5;

    ToddCoxeter tc(congruence_kind::twosided,
                   full_transformation_monoid_II74(n));
    REQUIRE(tc.number_of_classes() == 3'125);
  }

  LIBSEMIGROUPS_TEST_CASE(
      "fpsemi-examples",
      "039",
      "full_transformation_monoid(n = 2, 3) Mitchell + Whyte",
      "[fpsemi-examples][quick]") {
    auto rg = ReportGuard(REPORT);

    ToddCoxeter tc2(congruence_kind::twosided,
                    full_transformation_monoid_MW24_a(2));
    REQUIRE(tc2.number_of_classes() == 4);

    ToddCoxeter tc3(congruence_kind::twosided,
                    full_transformation_monoid_MW24_a(3));
    REQUIRE(tc3.number_of_classes() == 27);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "040",
                          "full_transformation_monoid(5) Mitchell + Whyte",
                          "[fpsemi-examples][quick]") {
    auto   rg = ReportGuard(REPORT);
    size_t n  = 5;

    ToddCoxeter tc(congruence_kind::twosided,
                   full_transformation_monoid_MW24_a(n));
    REQUIRE(tc.number_of_classes() == 3'125);
  }

  LIBSEMIGROUPS_TEST_CASE(
      "fpsemi-examples",
      "041",
      "full_transformation_monoid(5) Mitchell + Whyte index 1",
      "[fpsemi-examples]") {
    auto   rg = ReportGuard(REPORT);
    size_t n  = 5;

    ToddCoxeter tc(congruence_kind::twosided,
                   full_transformation_monoid_MW24_b(n));
    REQUIRE(tc.number_of_classes() == 3'125);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "042",
                          "full_transformation_monoid(6) Mitchell + Whyte",
                          "[fpsemi-examples]") {
    auto   rg = ReportGuard(REPORT);
    size_t n  = 6;

    ToddCoxeter tc(congruence_kind::twosided,
                   full_transformation_monoid_MW24_a(n));
    REQUIRE(tc.number_of_classes() == 46'656);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "043",
                          "partial_transformation_monoid(5) Shutov",
                          "[fpsemi-examples][quick][no-valgrind]") {
    auto        rg = ReportGuard(REPORT);
    size_t      n  = 5;
    ToddCoxeter tc(congruence_kind::twosided,
                   partial_transformation_monoid_Shu60(n));
    REQUIRE(tc.number_of_classes() == 7'776);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "044",
                          "partial_transformation_monoid(5) Mitchell + Whyte",
                          "[fpsemi-examples][quick]") {
    auto        rg = ReportGuard(REPORT);
    size_t      n  = 5;
    ToddCoxeter tc(congruence_kind::twosided,
                   partial_transformation_monoid_MW24(n));
    REQUIRE(tc.number_of_classes() == 7'776);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "045",
                          "symmetric_inverse_monoid(5)",
                          "[fpsemi-examples][quick][no-valgrind]") {
    auto   rg = ReportGuard(REPORT);
    size_t n  = 5;

    ToddCoxeter tc(congruence_kind::twosided,
                   fpsemigroup::symmetric_inverse_monoid_Shu60(n));
    REQUIRE(tc.number_of_classes() == 1'546);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "046",
                          "symmetric_inverse_monoid(4) Mitchell + Whyte",
                          "[fpsemi-examples][quick][no-valgrind]") {
    auto   rg = ReportGuard(REPORT);
    size_t n  = 4;

    ToddCoxeter tc(congruence_kind::twosided,
                   fpsemigroup::symmetric_inverse_monoid_MW24(n));
    REQUIRE(tc.number_of_classes() == 209);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "047",
                          "symmetric_inverse_monoid(5) Mitchell + Whyte",
                          "[fpsemi-examples][quick][no-valgrind]") {
    auto   rg = ReportGuard(REPORT);
    size_t n  = 5;

    ToddCoxeter tc(congruence_kind::twosided,
                   fpsemigroup::symmetric_inverse_monoid_MW24(n));
    REQUIRE(tc.number_of_classes() == 1'546);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "048",
                          "fibonacci_semigroup(2, 5)",
                          "[fpsemi-examples][quick]") {
    auto        rg = ReportGuard(REPORT);
    ToddCoxeter tc(congruence_kind::twosided, fibonacci_semigroup(2, 5));
    REQUIRE(tc.number_of_classes() == 11);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "049",
                          "alternating_group(7) Moore",
                          "[fpsemi-examples][quick][no-valgrind]") {
    auto        rg = ReportGuard(REPORT);
    size_t      n  = 7;
    ToddCoxeter tc(congruence_kind::twosided, alternating_group_Moo97(n));
    REQUIRE(tc.number_of_classes() == 2'520);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "050",
                          "full_transformation_monoid(4) Aizenstat",
                          "[fpsemi-examples][quick]") {
    auto   rg = ReportGuard(REPORT);
    size_t n  = 4;

    ToddCoxeter tc(congruence_kind::twosided,
                   full_transformation_monoid_Aiz58(n));
    REQUIRE(tc.number_of_classes() == 256);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "051",
                          "order_preserving_monoid(10)",
                          "[fpsemi-examples][standard]") {
    auto        rg = ReportGuard(REPORT);
    size_t      n  = 10;
    ToddCoxeter tc(congruence_kind::twosided, order_preserving_monoid(n));
    REQUIRE(tc.number_of_classes() == 92'378);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "052",
                          "cyclic_inverse_monoid(4) Fernandes b",
                          "[fpsemi-examples][quick]") {
    auto   rg = ReportGuard(REPORT);
    size_t n  = 4;

    ToddCoxeter tc(congruence_kind::twosided, cyclic_inverse_monoid_Fer22_b(n));
    REQUIRE(tc.number_of_classes() == 61);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "053",
                          "cyclic_inverse_monoid(8) Fernandes b",
                          "[fpsemi-examples][quick][no-valgrind]") {
    auto        rg = ReportGuard(REPORT);
    size_t      n  = 8;
    ToddCoxeter tc(congruence_kind::twosided, cyclic_inverse_monoid_Fer22_b(n));
    REQUIRE(tc.number_of_classes() == 2'041);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "054",
                          "cyclic_inverse_monoid Fernandes a",
                          "[fpsemi-examples][quick][no-valgrind]") {
    auto rg = ReportGuard(REPORT);
    for (size_t n = 3; n < 10; ++n) {
      auto p = fpsemigroup::cyclic_inverse_monoid_Fer22_a(n);
      REQUIRE(p.alphabet().size() == (n + 1));
      REQUIRE(p.rules.size() == (n * n + 3 * n + 4));
      if (n == 3) {
        presentation::sort_each_rule(p);
        presentation::sort_rules(p);

        REQUIRE(p.rules
                == std::vector<word_type>(
                    {000_w, {},   11_w, 1_w,  22_w,   2_w,  33_w, 3_w,
                     10_w,  02_w, 20_w, 03_w, 21_w,   12_w, 30_w, 01_w,
                     31_w,  13_w, 32_w, 23_w, 0123_w, 123_w}));
      }

      ToddCoxeter tc(congruence_kind::twosided, p);
      REQUIRE(tc.number_of_classes() == n * std::pow(2, n) - n + 1);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "055",
                          "cyclic_inverse_monoid Fernandes b",
                          "[fpsemi-examples][quick][no-valgrind]") {
    auto rg = ReportGuard(REPORT);
    for (size_t n = 3; n < 10; ++n) {
      auto p = fpsemigroup::cyclic_inverse_monoid_Fer22_b(n);
      REQUIRE(p.alphabet().size() == 2);
      REQUIRE(p.rules.size() == (n * n - n + 6));
      ToddCoxeter tc(congruence_kind::twosided, p);
      REQUIRE(tc.number_of_classes() == n * std::pow(2, n) - n + 1);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "056",
                          "order_preserving_cyclic_inverse_monoid(4)",
                          "[fpsemi-examples][quick]") {
    auto        rg = ReportGuard(REPORT);
    size_t      n  = 4;
    ToddCoxeter tc(congruence_kind::twosided,
                   order_preserving_cyclic_inverse_monoid(n));
    REQUIRE(tc.number_of_classes() == 38);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "057",
                          "order_preserving_cyclic_inverse_monoid(11)",
                          "[fpsemi-examples][quick][no-valgrind]") {
    auto        rg = ReportGuard(REPORT);
    size_t      n  = 11;
    ToddCoxeter tc(congruence_kind::twosided,
                   order_preserving_cyclic_inverse_monoid(n));
    REQUIRE(tc.number_of_classes() == 6'120);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "058",
                          "order_preserving_cyclic_inverse_monoid(11)",
                          "[fpsemi-examples][quick][no-valgrind]") {
    auto        rg = ReportGuard(REPORT);
    size_t      n  = 11;
    ToddCoxeter tc(congruence_kind::twosided,
                   order_preserving_cyclic_inverse_monoid(n));
    REQUIRE(tc.number_of_classes() == 6'120);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "059",
                          "partial_isometries_cycle_graph_monoid(5)",
                          "[fpsemi-examples][quick]") {
    auto        rg = ReportGuard(REPORT);
    size_t      n  = 5;
    ToddCoxeter tc(congruence_kind::twosided,
                   partial_isometries_cycle_graph_monoid(n));
    REQUIRE(tc.number_of_classes() == 286);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "060",
                          "partial_isometries_cycle_graph_monoid(4)",
                          "[fpsemi-examples][quick]") {
    auto        rg = ReportGuard(REPORT);
    size_t      n  = 4;
    ToddCoxeter tc(congruence_kind::twosided,
                   partial_isometries_cycle_graph_monoid(n));
    REQUIRE(tc.number_of_classes() == 97);
  }

  LIBSEMIGROUPS_TEST_CASE(
      "fpsemi-examples",
      "061",
      "partial_isometries_cycle_graph_monoid(10)",
      "[fpsemi-examples][quick][no-valgrind][no-coverage]") {
    auto        rg = ReportGuard(REPORT);
    size_t      n  = 10;
    ToddCoxeter tc(congruence_kind::twosided,
                   partial_isometries_cycle_graph_monoid(n));
    REQUIRE(tc.number_of_classes() == 20'311);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "062",
                          "not_symmetric_group(4)",
                          "[fpsemi-examples][quick]") {
    auto   rg = ReportGuard(REPORT);
    size_t n  = 4;

    ToddCoxeter tc(congruence_kind::twosided, not_symmetric_group(n));
    REQUIRE(tc.number_of_classes() == 72);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "063",
                          "partial_brauer_monoid(4)",
                          "[fpsemi-examples][quick][no-valgrind]") {
    auto   rg = ReportGuard(REPORT);
    size_t n  = 4;

    ToddCoxeter tc(congruence_kind::twosided,
                   fpsemigroup::partial_brauer_monoid(n));
    REQUIRE(tc.number_of_classes() == 764);
    REQUIRE_THROWS_AS(fpsemigroup::partial_brauer_monoid(0),
                      LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "064",
                          "motzkin_monoid(5)",
                          "[fpsemi-examples][quick]") {
    auto   rg = ReportGuard(REPORT);
    size_t n  = 5;

    ToddCoxeter tc(congruence_kind::twosided, fpsemigroup::motzkin_monoid(n));
    REQUIRE(tc.number_of_classes() == 2188);
    REQUIRE_THROWS_AS(fpsemigroup::motzkin_monoid(0), LibsemigroupsException);

    tc.init(congruence_kind::twosided, motzkin_monoid(1));
    REQUIRE(tc.number_of_classes() == 2);

    tc.init(congruence_kind::twosided, motzkin_monoid(2));
    REQUIRE(tc.number_of_classes() == 9);

    tc.init(congruence_kind::twosided, motzkin_monoid(3));
    REQUIRE(tc.number_of_classes() == 51);

    tc.init(congruence_kind::twosided, motzkin_monoid(4));
    REQUIRE(tc.number_of_classes() == 323);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "065",
                          "not_renner_type_B_monoid_God09(2)",
                          "[quick][fpsemi-examples][hivert]") {
    auto        rg = ReportGuard(REPORT);
    ToddCoxeter tc(congruence_kind::twosided,
                   fpsemigroup::not_renner_type_B_monoid_God09(2));
    REQUIRE(!is_obviously_infinite(tc));
    REQUIRE(!tc.started());
    REQUIRE(!tc.finished());
    REQUIRE(tc.number_of_classes() == 57);
    REQUIRE(tc.started());
    REQUIRE(tc.finished());
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "066",
                          "not_renner_type_B_monoid_Gay18(2)",
                          "[quick][fpsemi-examples][hivert]") {
    auto        rg = ReportGuard(REPORT);
    ToddCoxeter tc(congruence_kind::twosided,
                   fpsemigroup::not_renner_type_B_monoid_Gay18(2));
    REQUIRE(!is_obviously_infinite(tc));
    REQUIRE(tc.number_of_classes() == 57);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "067",
                          "renner_type_B_monoid_Gay18_b(2)",
                          "[quick][fpsemi-examples][hivert]") {
    auto        rg = ReportGuard(REPORT);
    ToddCoxeter tc(congruence_kind::twosided,
                   fpsemigroup::renner_type_B_monoid_Gay18_b(2));
    REQUIRE(!is_obviously_infinite(tc));
    REQUIRE(tc.number_of_classes() == 57);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "068",
                          "renner_type_B_monoid_Gay18_a(2)",
                          "[quick][fpsemi-examples][hivert]") {
    auto        rg = ReportGuard(REPORT);
    ToddCoxeter tc(congruence_kind::twosided,
                   fpsemigroup::renner_type_B_monoid_Gay18_a(2));
    REQUIRE(!is_obviously_infinite(tc));
    REQUIRE(tc.number_of_classes() == 57);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "069",
                          "renner_type_B_monoid_Gay18_b(3)",
                          "[quick][fpsemi-examples][hivert][no-valgrind]") {
    auto        rg = ReportGuard(REPORT);
    ToddCoxeter tc(congruence_kind::twosided,
                   fpsemigroup::renner_type_B_monoid_Gay18_b(3));
    REQUIRE(!is_obviously_infinite(tc));
    REQUIRE(tc.number_of_classes() == 757);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "070",
                          "renner_type_B_monoid_Gay18_a(3)",
                          "[quick][fpsemi-examples][hivert][no-valgrind]") {
    auto        rg = ReportGuard(REPORT);
    ToddCoxeter tc(congruence_kind::twosided,
                   fpsemigroup::renner_type_B_monoid_Gay18_a(3));
    REQUIRE(!is_obviously_infinite(tc));
    REQUIRE(tc.number_of_classes() == 757);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "071",
                          "renner_type_B_monoid_Gay18_b(4)",
                          "[quick][fpsemi-examples][hivert][no-valgrind]") {
    auto        rg = ReportGuard(REPORT);
    ToddCoxeter tc(congruence_kind::twosided,
                   fpsemigroup::renner_type_B_monoid_Gay18_b(4));
    REQUIRE(tc.presentation().rules.size() == 186);
    REQUIRE(!is_obviously_infinite(tc));
    REQUIRE(tc.number_of_classes() == 13'889);
    // TODO REQUIRE(to_froidure_pin(tc).run().number_of_rules() == 356);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "072",
                          "renner_type_B_monoid_Gay18_a(4)",
                          "[quick][fpsemi-examples][hivert][no-valgrind]") {
    auto        rg = ReportGuard(REPORT);
    ToddCoxeter tc(congruence_kind::twosided,
                   fpsemigroup::renner_type_B_monoid_Gay18_a(4));
    REQUIRE(tc.presentation().rules.size() == 186);
    REQUIRE(!is_obviously_infinite(tc));
    REQUIRE(tc.number_of_classes() == 13'889);
    // REQUIRE(S.froidure_pin()->number_of_rules() == 356);
  }

  // This appears to be an example where KB + FP is faster than TC
  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "073",
                          "renner_type_B_monoid_Gay18_b(5)",
                          "[extreme][fpsemi-examples][hivert]") {
    auto        rg = ReportGuard(true);
    ToddCoxeter tc(congruence_kind::twosided,
                   fpsemigroup::renner_type_B_monoid_Gay18_b(5));
    REQUIRE(tc.presentation().rules.size() == 272);
    REQUIRE(!is_obviously_infinite(tc));
    tc.strategy(ToddCoxeter::options::strategy::felsch);
    REQUIRE(tc.number_of_classes() == 322'021);
    // REQUIRE(S.froidure_pin()->number_of_rules() == 1453);
    // {
    //   congruence::ToddCoxeter tc(
    //       twosided,
    //       S.froidure_pin(),
    //       congruence::ToddCoxeter::options::froidure_pin::use_cayley_graph);
    //   REQUIRE(tc.number_of_classes() == 322021);  // Works!
    // }
    // {
    //   fpsemigroup::ToddCoxeter tc(S.froidure_pin());
    //   REQUIRE(tc.number_of_rules() == 1453);
    //   REQUIRE(tc.size() == 322021);
    // }
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "074",
                          "renner_type_B_monoid_Gay18_a(5)",
                          "[extreme][fpsemi-examples][hivert]") {
    auto        rg = ReportGuard(true);
    ToddCoxeter tc(congruence_kind::twosided,
                   fpsemigroup::renner_type_B_monoid_Gay18_a(5));
    tc.strategy(ToddCoxeter::options::strategy::felsch);

    REQUIRE(tc.presentation().rules.size() == 272);
    REQUIRE(!is_obviously_infinite(tc));
    REQUIRE(tc.number_of_classes() == 322'021);

    auto S = to_froidure_pin(tc);
    S.run();
    REQUIRE(S.number_of_rules() == 1430);

    //  FIXME the following doesn't seem to work
    // tc.init(congruence_kind::twosided, S.right_cayley_graph());
    // REQUIRE(tc.number_of_classes() == 322'021);  // Works!
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "075",
                          "renner_type_D_monoid_Gay18_b(2)",
                          "[quick][fpsemi-examples][hivert]") {
    auto        rg = ReportGuard(REPORT);
    ToddCoxeter tc(congruence_kind::twosided,
                   fpsemigroup::renner_type_D_monoid_Gay18_b(2));
    REQUIRE(tc.presentation().rules.size() == 68);
    REQUIRE(!is_obviously_infinite(tc));
    REQUIRE(tc.number_of_classes() == 37);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "076",
                          "renner_type_D_monoid_Gay18_a(2)",
                          "[quick][fpsemi-examples][hivert]") {
    auto        rg = ReportGuard(REPORT);
    ToddCoxeter tc(congruence_kind::twosided,
                   fpsemigroup::renner_type_D_monoid_Gay18_a(2));
    REQUIRE(tc.presentation().rules.size() == 68);
    REQUIRE(!is_obviously_infinite(tc));
    REQUIRE(tc.number_of_classes() == 37);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "077",
                          "renner_type_D_monoid_Gay18_b(3)",
                          "[quick][fpsemi-examples][hivert][no-valgrind]") {
    auto        rg = ReportGuard(REPORT);
    ToddCoxeter tc(congruence_kind::twosided,
                   fpsemigroup::renner_type_D_monoid_Gay18_b(3));
    REQUIRE(tc.presentation().rules.size() == 130);
    REQUIRE(!is_obviously_infinite(tc));
    REQUIRE(tc.number_of_classes() == 541);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "078",
                          "renner_type_D_monoid_Gay18_a(3)",
                          "[quick][fpsemi-examples][hivert][no-valgrind]") {
    auto        rg = ReportGuard(REPORT);
    ToddCoxeter tc(congruence_kind::twosided,
                   fpsemigroup::renner_type_D_monoid_Gay18_a(3));
    REQUIRE(tc.presentation().rules.size() == 130);
    REQUIRE(!is_obviously_infinite(tc));
    REQUIRE(tc.number_of_classes() == 541);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "079",
                          "renner_type_D_monoid_Gay18_b(4)",
                          "[quick][fpsemi-examples][hivert][no-valgrind]") {
    auto        rg = ReportGuard(REPORT);
    ToddCoxeter tc(congruence_kind::twosided,
                   fpsemigroup::renner_type_D_monoid_Gay18_b(4));
    REQUIRE(tc.presentation().rules.size() == 208);
    REQUIRE(!is_obviously_infinite(tc));

    REQUIRE(tc.number_of_classes() == 10'625);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "080",
                          "renner_type_D_monoid_Gay18_a(4)",
                          "[quick][fpsemi-examples][hivert][no-valgrind]") {
    auto        rg = ReportGuard(false);
    ToddCoxeter tc(congruence_kind::twosided,
                   fpsemigroup::renner_type_D_monoid_Gay18_a(4));
    REQUIRE(tc.presentation().rules.size() == 208);
    REQUIRE(!is_obviously_infinite(tc));
    REQUIRE(tc.number_of_classes() == 10'625);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "081",
                          "renner_type_D_monoid_Gay18_b(5)",
                          "[extreme][fpsemi-examples][hivert]") {
    auto        rg = ReportGuard(true);
    ToddCoxeter tc(congruence_kind::twosided,
                   fpsemigroup::renner_type_D_monoid_Gay18_b(5));
    tc.strategy(ToddCoxeter::options::strategy::felsch);

    REQUIRE(tc.presentation().rules.size() == 302);
    REQUIRE(!is_obviously_infinite(tc));

    REQUIRE(tc.number_of_classes() == 258'661);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "082",
                          "renner_type_D_monoid_Gay18_a(5)",
                          "[extreme][fpsemi-examples][hivert]") {
    auto        rg = ReportGuard(true);
    ToddCoxeter tc(congruence_kind::twosided,
                   fpsemigroup::renner_type_D_monoid_Gay18_a(5));
    tc.strategy(ToddCoxeter::options::strategy::felsch);

    REQUIRE(tc.presentation().rules.size() == 302);
    REQUIRE(!is_obviously_infinite(tc));
    REQUIRE(tc.number_of_classes() == 258'661);
  }

  // Takes about 1m46s
  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "083",
                          "renner_type_D_monoid_Gay18_b(6)",
                          "[extreme][fpsemi-examples][hivert]") {
    auto        rg = ReportGuard(true);
    ToddCoxeter tc(congruence_kind::twosided,
                   fpsemigroup::renner_type_D_monoid_Gay18_b(6));
    tc.strategy(ToddCoxeter::options::strategy::felsch);

    REQUIRE(tc.presentation().rules.size() == 412);
    REQUIRE(!is_obviously_infinite(tc));
    REQUIRE(tc.number_of_classes() == 7'464'625);
  }

  // Takes about 2m42s
  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "084",
                          "renner_type_D_monoid_Gay18_a(6)",
                          "[extreme][fpsemi-examples][hivert]") {
    auto        rg = ReportGuard(true);
    ToddCoxeter tc(congruence_kind::twosided,
                   fpsemigroup::renner_type_D_monoid_Gay18_a(6));
    tc.strategy(ToddCoxeter::options::strategy::felsch);

    REQUIRE(tc.presentation().rules.size() == 412);
    REQUIRE(!is_obviously_infinite(tc));
    REQUIRE(tc.number_of_classes() == 7'464'625);
  }
}  // namespace libsemigroups
