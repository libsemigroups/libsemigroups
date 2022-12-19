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

#include "catch.hpp"      // for REQUIRE, REQUIRE_NOTHROW, REQUIRE_THROWS_AS
#include "test-main.hpp"  // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/fpsemi-examples.hpp"  // for the presentations
#include "libsemigroups/report.hpp"           // for ReportGuard
#include "libsemigroups/todd-coxeter.hpp"     // for ToddCoxeter
#include "libsemigroups/types.hpp"            // for word_type

namespace libsemigroups {
  struct LibsemigroupsException;
  constexpr bool REPORT = false;

  using fpsemigroup::author;
  using fpsemigroup::make;

  using fpsemigroup::alternating_group;
  using fpsemigroup::chinese_monoid;
  using fpsemigroup::cyclic_inverse_monoid;
  using fpsemigroup::dual_symmetric_inverse_monoid;
  using fpsemigroup::fibonacci_semigroup;
  using fpsemigroup::full_transformation_monoid;
  using fpsemigroup::monogenic_semigroup;
  using fpsemigroup::order_preserving_cyclic_inverse_monoid;
  using fpsemigroup::order_preserving_monoid;
  using fpsemigroup::orientation_preserving_monoid;
  using fpsemigroup::orientation_reversing_monoid;
  using fpsemigroup::partial_isometries_cycle_graph_monoid;
  using fpsemigroup::partial_transformation_monoid;
  using fpsemigroup::partition_monoid;
  using fpsemigroup::plactic_monoid;
  using fpsemigroup::rectangular_band;
  using fpsemigroup::rook_monoid;
  using fpsemigroup::singular_brauer_monoid;
  using fpsemigroup::stellar_monoid;
  using fpsemigroup::stylic_monoid;
  using fpsemigroup::symmetric_group;
  using fpsemigroup::symmetric_inverse_monoid;
  using fpsemigroup::temperley_lieb_monoid;
  using fpsemigroup::uniform_block_bijection_monoid;

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "049",
                          "test default values",
                          "[fpsemi-examples][quick]") {
    // author defaults
    REQUIRE(symmetric_group(4) == symmetric_group(4, author::Carmichael));
    REQUIRE(alternating_group(4) == alternating_group(4, author::Moore));
    REQUIRE(full_transformation_monoid(4)
            == full_transformation_monoid(4, author::Iwahori));
    REQUIRE(partial_transformation_monoid(4)
            == partial_transformation_monoid(4, author::Sutov));
    REQUIRE(symmetric_inverse_monoid(4)
            == symmetric_inverse_monoid(4, author::Sutov));
    REQUIRE(dual_symmetric_inverse_monoid(4)
            == dual_symmetric_inverse_monoid(
                4, author::Easdown + author::East + author::FitzGerald));
    REQUIRE(uniform_block_bijection_monoid(4)
            == uniform_block_bijection_monoid(4, author::FitzGerald));
    REQUIRE(partition_monoid(4) == partition_monoid(4, author::East));
    REQUIRE(cyclic_inverse_monoid(4)
            == cyclic_inverse_monoid(4, author::Fernandes));

    // index defaults
    REQUIRE(symmetric_group(4, author::Moore)
            == symmetric_group(4, author::Moore, 0));
    REQUIRE(cyclic_inverse_monoid(4, author::Fernandes)
            == cyclic_inverse_monoid(4, author::Fernandes, 1));
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "002",
                          "full_transformation_monoid author except",
                          "[fpsemi-examples][quick]") {
    auto rg = ReportGuard(REPORT);
    REQUIRE_THROWS_AS(full_transformation_monoid(5, author::Burnside),
                      LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "003",
                          "full_transformation_monoid degree except",
                          "[fpsemi-examples][quick]") {
    auto rg = ReportGuard(REPORT);
    REQUIRE_THROWS_AS(full_transformation_monoid(3, author::Iwahori),
                      LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "005",
                          "partial_transformation_monoid author except",
                          "[fpsemi-examples][quick]") {
    auto rg = ReportGuard(REPORT);
    REQUIRE_THROWS_AS(partial_transformation_monoid(4, author::Burnside),
                      LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "006",
                          "partial_transformation_monoid degree except",
                          "[fpsemi-examples][quick]") {
    auto rg = ReportGuard(REPORT);
    REQUIRE_THROWS_AS(partial_transformation_monoid(3, author::Sutov),
                      LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "008",
                          "symmetric_group auth except",
                          "[fpsemi-examples][quick]") {
    auto rg = ReportGuard(REPORT);
    REQUIRE_THROWS_AS(symmetric_group(9, author::Sutov),
                      LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "009",
                          "symmetric_group degree except",
                          "[fpsemi-examples][quick]") {
    auto rg = ReportGuard(REPORT);
    REQUIRE_THROWS_AS(symmetric_group(3, author::Carmichael),
                      LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "064",
                          "symmetric_group index except",
                          "[fpsemi-examples][quick]") {
    auto rg = ReportGuard(REPORT);
    REQUIRE_THROWS_AS(symmetric_group(5, author::Moore, 2),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(symmetric_group(5, author::Carmichael, 1),
                      LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "012",
                          "dual_symmetric_inverse_monoid auth except",
                          "[fpsemi-examples][quick]") {
    auto rg = ReportGuard(REPORT);
    REQUIRE_THROWS_AS(dual_symmetric_inverse_monoid(9, author::East),
                      LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "013",
                          "dual_symmetric_inverse_monoid degree except",
                          "[fpsemi-examples][quick]") {
    auto rg = ReportGuard(REPORT);
    REQUIRE_THROWS_AS(
        dual_symmetric_inverse_monoid(
            2, author::Easdown + author::East + author::FitzGerald),
        LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "015",
                          "uniform_block_bijection_monoid auth except",
                          "[fpsemi-examples][quick]") {
    auto rg = ReportGuard(REPORT);
    REQUIRE_THROWS_AS(uniform_block_bijection_monoid(9, author::Burnside),
                      LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "016",
                          "uniform_block_bijection_monoid degree except",
                          "[fpsemi-examples][quick]") {
    auto rg = ReportGuard(REPORT);
    REQUIRE_THROWS_AS(uniform_block_bijection_monoid(2, author::FitzGerald),
                      LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "019",
                          "partition_monoid auth except",
                          "[fpsemi-examples][quick]") {
    auto rg = ReportGuard(REPORT);
    REQUIRE_THROWS_AS(partition_monoid(3, author::Burnside),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(partition_monoid(5, author::Machine),
                      LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "020",
                          "partition_monoid degree except",
                          "[fpsemi-examples][quick]") {
    auto rg = ReportGuard(REPORT);
    REQUIRE_THROWS_AS(partition_monoid(4, author::Machine),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(partition_monoid(3, author::East),
                      LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "036",
                          "alternating_group auth except",
                          "[fpsemi-examples][quick]") {
    auto rg = ReportGuard(REPORT);
    REQUIRE_THROWS_AS(alternating_group(5, author::Sutov),
                      LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "037",
                          "alternating_group degree except",
                          "[fpsemi-examples][quick]") {
    auto rg = ReportGuard(REPORT);
    REQUIRE_THROWS_AS(alternating_group(3, author::Moore),
                      LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "039",
                          "chinese_monoid degree except",
                          "[fpsemi-examples][quick]") {
    auto rg = ReportGuard(REPORT);
    REQUIRE_THROWS_AS(chinese_monoid(1), LibsemigroupsException);
    REQUIRE_THROWS_AS(chinese_monoid(0), LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "040",
                          "monogenic_semigroup degree except",
                          "[fpsemi-examples][quick]") {
    auto rg = ReportGuard(REPORT);
    REQUIRE_THROWS_AS(monogenic_semigroup(0, 0), LibsemigroupsException);
    REQUIRE_THROWS_AS(monogenic_semigroup(5, 0), LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "041",
                          "rectangular_band degree except",
                          "[fpsemi-examples][quick]") {
    auto rg = ReportGuard(REPORT);
    REQUIRE_THROWS_AS(rectangular_band(0, 0), LibsemigroupsException);
    REQUIRE_THROWS_AS(rectangular_band(1, 0), LibsemigroupsException);
    REQUIRE_THROWS_AS(rectangular_band(0, 1), LibsemigroupsException);
    REQUIRE_THROWS_AS(rectangular_band(0, 100), LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "042",
                          "stellar_monoid degree except",
                          "[fpsemi-examples][quick]") {
    auto rg = ReportGuard(REPORT);
    REQUIRE_THROWS_AS(stellar_monoid(0), LibsemigroupsException);
    REQUIRE_THROWS_AS(stellar_monoid(1), LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "043",
                          "plactic_monoid degree except",
                          "[fpsemi-examples][quick]") {
    auto rg = ReportGuard(REPORT);
    REQUIRE_THROWS_AS(plactic_monoid(0), LibsemigroupsException);
    REQUIRE_THROWS_AS(plactic_monoid(1), LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "044",
                          "stylic_monoid degree except",
                          "[fpsemi-examples][quick]") {
    auto rg = ReportGuard(REPORT);
    REQUIRE_THROWS_AS(stylic_monoid(0), LibsemigroupsException);
    REQUIRE_THROWS_AS(stylic_monoid(1), LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "045",
                          "temperley_lieb_monoid degree except",
                          "[fpsemi-examples][quick]") {
    auto rg = ReportGuard(REPORT);
    REQUIRE_THROWS_AS(temperley_lieb_monoid(0), LibsemigroupsException);
    REQUIRE_THROWS_AS(temperley_lieb_monoid(1), LibsemigroupsException);
    REQUIRE_THROWS_AS(temperley_lieb_monoid(2), LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "046",
                          "singular_brauer_monoid degree except",
                          "[fpsemi-examples][quick]") {
    auto rg = ReportGuard(REPORT);
    REQUIRE_THROWS_AS(singular_brauer_monoid(0), LibsemigroupsException);
    REQUIRE_THROWS_AS(singular_brauer_monoid(1), LibsemigroupsException);
    REQUIRE_THROWS_AS(singular_brauer_monoid(2), LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "047",
                          "orientation_preserving_monoid degree except",
                          "[fpsemi-examples][quick]") {
    auto rg = ReportGuard(REPORT);
    REQUIRE_THROWS_AS(orientation_preserving_monoid(0), LibsemigroupsException);
    REQUIRE_THROWS_AS(orientation_preserving_monoid(1), LibsemigroupsException);
    REQUIRE_THROWS_AS(orientation_preserving_monoid(2), LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "048",
                          "orientation_reversing_monoid degree except",
                          "[fpsemi-examples][quick]") {
    auto rg = ReportGuard(REPORT);
    REQUIRE_THROWS_AS(orientation_reversing_monoid(0), LibsemigroupsException);
    REQUIRE_THROWS_AS(orientation_reversing_monoid(1), LibsemigroupsException);
    REQUIRE_THROWS_AS(orientation_reversing_monoid(2), LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "055",
                          "order_preserving_monoid degree except",
                          "[fpsemi-examples][quick]") {
    auto rg = ReportGuard(REPORT);
    REQUIRE_THROWS_AS(order_preserving_monoid(0), LibsemigroupsException);
    REQUIRE_THROWS_AS(order_preserving_monoid(1), LibsemigroupsException);
    REQUIRE_THROWS_AS(order_preserving_monoid(2), LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "061",
                          "cyclic_inverse_monoid degree except",
                          "[fpsemi-examples][quick]") {
    auto rg = ReportGuard(REPORT);
    REQUIRE_THROWS_AS(cyclic_inverse_monoid(0, author::Fernandes, 0),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(cyclic_inverse_monoid(1, author::Fernandes, 0),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(cyclic_inverse_monoid(2, author::Fernandes, 0),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(cyclic_inverse_monoid(0, author::Fernandes, 1),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(cyclic_inverse_monoid(1, author::Fernandes, 1),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(cyclic_inverse_monoid(2, author::Fernandes, 1),
                      LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "062",
                          "cyclic_inverse_monoid author except",
                          "[fpsemi-examples][quick]") {
    auto rg = ReportGuard(REPORT);
    REQUIRE_THROWS_AS(cyclic_inverse_monoid(5, author::Burnside, 0),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(cyclic_inverse_monoid(5, author::Fernandes, 3),
                      LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "063",
                          "order_preserving_monoid degree except",
                          "[fpsemi-examples][quick]") {
    auto rg = ReportGuard(REPORT);
    REQUIRE_THROWS_AS(order_preserving_monoid(0), LibsemigroupsException);
    REQUIRE_THROWS_AS(order_preserving_monoid(1), LibsemigroupsException);
    REQUIRE_THROWS_AS(order_preserving_monoid(2), LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                          "069",
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

  namespace congruence {
    LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                            "001",
                            "full_transformation_monoid(5) Iwahori",
                            "[fpsemi-examples][standard]") {
      auto   rg = ReportGuard(REPORT);
      size_t n  = 5;
      auto   s  = full_transformation_monoid(n, author::Iwahori);
      auto   p  = make<Presentation<word_type>>(s);
      p.alphabet(n + 1);
      presentation::replace_word(p, word_type({}), {n});
      presentation::add_identity_rules(p, n);
      p.validate();

      ToddCoxeter tc(congruence_kind::twosided);
      tc.set_number_of_generators(n + 1);
      for (size_t i = 0; i < p.rules.size() - 1; i += 2) {
        tc.add_pair(p.rules[i], p.rules[i + 1]);
      }
      REQUIRE(tc.number_of_classes() == 3125);
    }

    LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                            "004",
                            "partial_transformation_monoid(5) Sutov",
                            "[fpsemi-examples][standard]") {
      auto   rg = ReportGuard(REPORT);
      size_t n  = 5;
      auto   s  = partial_transformation_monoid(n, author::Sutov);
      auto   p  = make<Presentation<word_type>>(s);
      p.alphabet(n + 2);
      presentation::replace_word(p, word_type({}), {n + 1});
      presentation::add_identity_rules(p, n + 1);
      p.validate();

      ToddCoxeter tc(congruence_kind::twosided);
      tc.set_number_of_generators(n + 2);
      for (size_t i = 0; i < p.rules.size() - 1; i += 2) {
        tc.add_pair(p.rules[i], p.rules[i + 1]);
      }
      REQUIRE(tc.number_of_classes() == 7776);
    }

    LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                            "038",
                            "partial_transformation_monoid(3) Machine",
                            "[fpsemi-examples][quick]") {
      auto rg = ReportGuard(REPORT);
      auto s  = partial_transformation_monoid(3, author::Machine);
      auto p  = make<Presentation<word_type>>(s);
      p.alphabet(5);
      presentation::replace_word(p, word_type({}), {4});
      presentation::add_identity_rules(p, 4);
      p.validate();

      ToddCoxeter tc(congruence_kind::twosided);
      tc.set_number_of_generators(5);
      for (size_t i = 0; i < p.rules.size() - 1; i += 2) {
        tc.add_pair(p.rules[i], p.rules[i + 1]);
      }
      REQUIRE(tc.number_of_classes() == 64);
    }

    LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                            "007",
                            "symmetric_inverse_monoid(5)",
                            "[fpsemi-examples][quick][no-valgrind]") {
      auto   rg = ReportGuard(REPORT);
      size_t n  = 5;
      auto   s  = symmetric_inverse_monoid(n, author::Sutov);
      auto   p  = make<Presentation<word_type>>(s);
      p.alphabet(n + 1);
      presentation::replace_word(p, word_type({}), {n});
      presentation::add_identity_rules(p, n);
      p.validate();

      ToddCoxeter tc(congruence_kind::twosided);
      tc.set_number_of_generators(n + 1);
      for (size_t i = 0; i < p.rules.size() - 1; i += 2) {
        tc.add_pair(p.rules[i], p.rules[i + 1]);
      }
      REQUIRE(tc.number_of_classes() == 1546);
    }

    LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                            "010",
                            "symmetric_group(6) Burnside + Miller",
                            "[fpsemi-examples][quick][no-valgrind]") {
      auto   rg = ReportGuard(REPORT);
      size_t n  = 6;
      auto   s  = symmetric_group(n, author::Burnside + author::Miller);
      auto   p  = make<Presentation<word_type>>(s);
      p.alphabet(n);
      presentation::replace_word(p, word_type({}), {n - 1});
      presentation::add_identity_rules(p, n - 1);
      p.validate();

      ToddCoxeter tc(congruence_kind::twosided);
      tc.set_number_of_generators(n);
      for (size_t i = 0; i < p.rules.size() - 1; i += 2) {
        tc.add_pair(p.rules[i], p.rules[i + 1]);
      }
      REQUIRE(tc.number_of_classes() == 720);
    }

    LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                            "031",
                            "symmetric_group(6) Carmichael",
                            "[fpsemi-examples][quick]") {
      auto   rg = ReportGuard(REPORT);
      size_t n  = 6;
      auto   s  = symmetric_group(n, author::Carmichael);
      auto   p  = make<Presentation<word_type>>(s);
      p.alphabet(n);
      presentation::replace_word(p, word_type({}), {n - 1});
      presentation::add_identity_rules(p, n - 1);
      p.validate();

      ToddCoxeter tc(congruence_kind::twosided);
      tc.set_number_of_generators(n);
      for (size_t i = 0; i < p.rules.size() - 1; i += 2) {
        tc.add_pair(p.rules[i], p.rules[i + 1]);
      }
      REQUIRE(tc.number_of_classes() == 720);
    }

    LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                            "032",
                            "symmetric_group(6) Moore index 0",
                            "[fpsemi-examples][quick]") {
      auto   rg = ReportGuard(REPORT);
      size_t n  = 6;
      auto   s  = symmetric_group(n, author::Moore);
      auto   p  = make<Presentation<word_type>>(s);
      p.alphabet(3);
      presentation::replace_word(p, word_type({}), {2});
      presentation::add_identity_rules(p, 2);
      p.validate();

      ToddCoxeter tc(congruence_kind::twosided);
      tc.set_number_of_generators(3);
      for (size_t i = 0; i < p.rules.size() - 1; i += 2) {
        tc.add_pair(p.rules[i], p.rules[i + 1]);
      }
      REQUIRE(tc.number_of_classes() == 720);
    }

    LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                            "065",
                            "symmetric_group(7) Moore index 1",
                            "[fpsemi-examples][quick]") {
      auto   rg = ReportGuard(REPORT);
      size_t n  = 7;
      auto   s  = symmetric_group(n, author::Moore, 1);
      auto   p  = make<Presentation<word_type>>(s);
      p.alphabet(n);
      presentation::replace_word(p, word_type({}), {n - 1});
      presentation::add_identity_rules(p, n - 1);
      p.validate();

      ToddCoxeter tc(congruence_kind::twosided);
      tc.set_number_of_generators(n);
      for (size_t i = 0; i < p.rules.size() - 1; i += 2) {
        tc.add_pair(p.rules[i], p.rules[i + 1]);
      }
      REQUIRE(tc.number_of_classes() == 5040);
    }

    LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                            "033",
                            "symmetric_group(6) Coxeter + Moser",
                            "[fpsemi-examples][quick]") {
      auto   rg = ReportGuard(REPORT);
      size_t n  = 6;
      auto   s  = symmetric_group(n, author::Coxeter + author::Moser);
      auto   p  = make<Presentation<word_type>>(s);
      p.alphabet(n);
      presentation::replace_word(p, word_type({}), {n - 1});
      presentation::add_identity_rules(p, n - 1);
      p.validate();

      ToddCoxeter tc(congruence_kind::twosided);
      tc.set_number_of_generators(n);
      for (size_t i = 0; i < p.rules.size() - 1; i += 2) {
        tc.add_pair(p.rules[i], p.rules[i + 1]);
      }
      REQUIRE(tc.number_of_classes() == 720);
    }

    LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                            "011",
                            "dual_symmetric_inverse_monoid(5)",
                            "[fpsemi-examples][quick][no-valgrind]") {
      auto   rg = ReportGuard(REPORT);
      size_t n  = 5;
      auto   s  = dual_symmetric_inverse_monoid(
          n, author::Easdown + author::East + author::FitzGerald);
      auto p = make<Presentation<word_type>>(s);
      p.validate();
      auto        m = p.alphabet().size();
      ToddCoxeter tc(congruence_kind::twosided);
      tc.set_number_of_generators(m);
      for (size_t i = 0; i < p.rules.size() - 1; i += 2) {
        tc.add_pair(p.rules[i], p.rules[i + 1]);
      }
      REQUIRE(tc.number_of_classes() == 6721);
    }

    LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                            "014",
                            "uniform_block_bijection_monoid(5)",
                            "[fpsemi-examples][quick]") {
      auto   rg = ReportGuard(REPORT);
      size_t n  = 5;
      auto   s  = uniform_block_bijection_monoid(n, author::FitzGerald);
      auto   p  = make<Presentation<word_type>>(s);
      p.validate();
      auto        m = p.alphabet().size();
      ToddCoxeter tc(congruence_kind::twosided);
      tc.set_number_of_generators(m);
      for (size_t i = 0; i < p.rules.size() - 1; i += 2) {
        tc.add_pair(p.rules[i], p.rules[i + 1]);
      }
      REQUIRE(tc.number_of_classes() == 1496);
    }

    LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                            "017",
                            "partition_monoid(5)",
                            "[fpsemi-examples][standard]") {
      auto   rg = ReportGuard(REPORT);
      size_t n  = 5;
      auto   s  = partition_monoid(n, author::East);
      auto   p  = make<Presentation<word_type>>(s);
      p.validate();
      auto        m = p.alphabet().size();
      ToddCoxeter tc(congruence_kind::twosided);
      tc.set_number_of_generators(m);
      for (size_t i = 0; i < p.rules.size() - 1; i += 2) {
        tc.add_pair(p.rules[i], p.rules[i + 1]);
      }
      REQUIRE(tc.number_of_classes() == 115975);
    }

    LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                            "018",
                            "partition_monoid(3)",
                            "[fpsemi-examples][standard]") {
      auto rg = ReportGuard(REPORT);
      auto s  = partition_monoid(3, author::Machine);
      auto p  = make<Presentation<word_type>>(s);
      p.validate();
      ToddCoxeter tc(congruence_kind::twosided);
      auto        m = p.alphabet().size();
      tc.set_number_of_generators(m);
      for (size_t i = 0; i < p.rules.size() - 1; i += 2) {
        tc.add_pair(p.rules[i], p.rules[i + 1]);
      }
      REQUIRE(tc.number_of_classes() == 203);
    }

    LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                            "021",
                            "rectangular_band(5, 9)",
                            "[fpsemi-examples][standard]") {
      auto rg = ReportGuard(REPORT);
      auto s  = rectangular_band(5, 9);
      auto p  = make<Presentation<word_type>>(s);
      p.validate();
      auto m = p.alphabet().size();
      p.validate();
      ToddCoxeter tc(congruence_kind::twosided);
      tc.set_number_of_generators(m);
      for (size_t i = 0; i < p.rules.size() - 1; i += 2) {
        tc.add_pair(p.rules[i], p.rules[i + 1]);
      }
      REQUIRE(tc.number_of_classes() == 45);
    }

    LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                            "022",
                            "monogenic_semigroup(4, 9)",
                            "[fpsemi-examples][quick]") {
      auto rg = ReportGuard(REPORT);
      auto s  = monogenic_semigroup(4, 9);
      auto p  = make<Presentation<word_type>>(s);
      p.validate();
      auto m = p.alphabet().size();
      p.validate();
      ToddCoxeter tc(congruence_kind::twosided);
      tc.set_number_of_generators(m);
      for (size_t i = 0; i < p.rules.size() - 1; i += 2) {
        tc.add_pair(p.rules[i], p.rules[i + 1]);
      }
      REQUIRE(tc.number_of_classes() == 12);
    }

    LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                            "023",
                            "fibonacci_semigroup(2, 5)",
                            "[fpsemi-examples][quick]") {
      auto rg = ReportGuard(REPORT);
      auto s  = fibonacci_semigroup(2, 5);
      auto p  = make<Presentation<word_type>>(s);
      p.validate();
      auto m = p.alphabet().size();
      p.validate();
      ToddCoxeter tc(congruence_kind::twosided);
      tc.set_number_of_generators(m);
      for (size_t i = 0; i < p.rules.size() - 1; i += 2) {
        tc.add_pair(p.rules[i], p.rules[i + 1]);
      }
      REQUIRE(tc.number_of_classes() == 11);
    }

    LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                            "024",
                            "temperley_lieb_monoid(10)",
                            "[fpsemi-examples][quick][no-valgrind]") {
      auto   rg = ReportGuard(REPORT);
      size_t n  = 10;
      auto   s  = temperley_lieb_monoid(n);
      auto   p  = make<Presentation<word_type>>(s);
      p.alphabet(n);
      presentation::add_identity_rules(p, n - 1);
      p.validate();
      ToddCoxeter tc(congruence_kind::twosided);
      tc.set_number_of_generators(n);
      for (size_t i = 0; i < p.rules.size() - 1; i += 2) {
        tc.add_pair(p.rules[i], p.rules[i + 1]);
      }
      REQUIRE(tc.number_of_classes() == 16796);
    }

    LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                            "025",
                            "singular_brauer_monoid(6)",
                            "[fpsemi-examples][quick][no-valgrind]") {
      auto   rg = ReportGuard(REPORT);
      size_t n  = 6;
      auto   s  = singular_brauer_monoid(n);
      auto   p  = make<Presentation<word_type>>(s);
      p.validate();
      auto        m = p.alphabet().size();
      ToddCoxeter tc(congruence_kind::twosided);
      tc.set_number_of_generators(m);
      for (size_t i = 0; i < p.rules.size() - 1; i += 2) {
        tc.add_pair(p.rules[i], p.rules[i + 1]);
      }
      REQUIRE(tc.number_of_classes() == 9675);
    }

    LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                            "026",
                            "orientation_preserving_monoid(6)",
                            "[fpsemi-examples][quick][no-valgrind]") {
      auto   rg = ReportGuard(REPORT);
      size_t n  = 6;
      auto   s  = orientation_preserving_monoid(n);
      auto   p  = make<Presentation<word_type>>(s);
      p.validate();
      auto        m = p.alphabet().size();
      ToddCoxeter tc(congruence_kind::twosided);
      tc.set_number_of_generators(m);
      for (size_t i = 0; i < p.rules.size() - 1; i += 2) {
        tc.add_pair(p.rules[i], p.rules[i + 1]);
      }
      REQUIRE(tc.number_of_classes() == 2742);
    }

    LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                            "027",
                            "orientation_reversing_monoid(5)",
                            "[fpsemi-examples][quick]") {
      auto   rg = ReportGuard(REPORT);
      size_t n  = 5;
      auto   s  = orientation_reversing_monoid(n);
      auto   p  = make<Presentation<word_type>>(s);
      p.validate();
      auto        m = p.alphabet().size();
      ToddCoxeter tc(congruence_kind::twosided);
      tc.set_number_of_generators(m);
      for (size_t i = 0; i < p.rules.size() - 1; i += 2) {
        tc.add_pair(p.rules[i], p.rules[i + 1]);
      }
      REQUIRE(tc.number_of_classes() == 1015);
    }

    LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                            "034",
                            "test for the rook monoid",
                            "[fpsemi-examples][quick]") {
      auto rg = ReportGuard(REPORT);
      auto s  = rook_monoid(4, 1);
      auto p  = make<Presentation<word_type>>(s);
      p.validate();
      auto        m = p.alphabet().size();
      ToddCoxeter tc(congruence_kind::twosided);
      tc.set_number_of_generators(m);
      for (size_t i = 0; i < p.rules.size() - 1; i += 2) {
        tc.add_pair(p.rules[i], p.rules[i + 1]);
      }
      REQUIRE(tc.number_of_classes() == 209);
    }

    LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                            "035",
                            "alternating_group(7) Moore",
                            "[fpsemi-examples][quick][no-valgrind]") {
      auto   rg = ReportGuard(REPORT);
      size_t n  = 7;
      auto   s  = alternating_group(n, author::Moore);
      auto   p  = make<Presentation<word_type>>(s);
      p.alphabet(n - 1);
      presentation::replace_word(p, word_type({}), {n - 2});
      presentation::add_identity_rules(p, n - 2);
      p.validate();

      ToddCoxeter tc(congruence_kind::twosided);
      tc.set_number_of_generators(n - 1);
      for (size_t i = 0; i < p.rules.size() - 1; i += 2) {
        tc.add_pair(p.rules[i], p.rules[i + 1]);
      }
      REQUIRE(tc.number_of_classes() == 2520);
    }

    LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                            "050",
                            "full_transformation_monoid(4) Aizenstat",
                            "[fpsemi-examples][standard]") {
      auto   rg = ReportGuard(REPORT);
      size_t n  = 4;
      auto   s  = full_transformation_monoid(n, author::Aizenstat);
      auto   p  = make<Presentation<word_type>>(s);
      p.alphabet(4);
      presentation::replace_word(p, word_type({}), {3});
      presentation::add_identity_rules(p, 3);
      p.validate();

      ToddCoxeter tc(congruence_kind::twosided);
      tc.set_number_of_generators(4);
      for (size_t i = 0; i < p.rules.size() - 1; i += 2) {
        tc.add_pair(p.rules[i], p.rules[i + 1]);
      }
      REQUIRE(tc.number_of_classes() == 256);
    }

    LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                            "053",
                            "order_preserving_monoid(5)",
                            "[fpsemi-examples][quick]") {
      auto   rg = ReportGuard(REPORT);
      size_t n  = 5;
      auto   s  = order_preserving_monoid(n);
      auto   p  = make<Presentation<word_type>>(s);
      p.alphabet(2 * n - 1);
      presentation::add_identity_rules(p, 2 * n - 2);
      p.validate();

      ToddCoxeter tc(congruence_kind::twosided);
      tc.set_number_of_generators(2 * n - 1);
      for (size_t i = 0; i < p.rules.size() - 1; i += 2) {
        tc.add_pair(p.rules[i], p.rules[i + 1]);
      }
      REQUIRE(tc.number_of_classes() == 126);
    }

    LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                            "054",
                            "order_preserving_monoid(10)",
                            "[fpsemi-examples][standard]") {
      auto   rg = ReportGuard(REPORT);
      size_t n  = 10;
      auto   s  = order_preserving_monoid(n);
      auto   p  = make<Presentation<word_type>>(s);
      p.alphabet(2 * n - 1);
      presentation::add_identity_rules(p, 2 * n - 2);
      p.validate();

      ToddCoxeter tc(congruence_kind::twosided);
      tc.set_number_of_generators(2 * n - 1);
      for (size_t i = 0; i < p.rules.size() - 1; i += 2) {
        tc.add_pair(p.rules[i], p.rules[i + 1]);
      }
      REQUIRE(tc.number_of_classes() == 92'378);
    }

    LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                            "056",
                            "cyclic_inverse_monoid(4) Fernandes 1",
                            "[fpsemi-examples][quick]") {
      auto   rg = ReportGuard(REPORT);
      size_t n  = 4;
      auto   s  = cyclic_inverse_monoid(n, author::Fernandes, 1);
      auto   p  = make<Presentation<word_type>>(s);
      p.alphabet(3);
      presentation::replace_word(p, word_type({}), {2});
      presentation::add_identity_rules(p, 2);
      p.validate();

      ToddCoxeter tc(congruence_kind::twosided);
      tc.set_number_of_generators(3);
      for (size_t i = 0; i < p.rules.size() - 1; i += 2) {
        tc.add_pair(p.rules[i], p.rules[i + 1]);
      }
      REQUIRE(tc.number_of_classes() == 61);
    }

    LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                            "057",
                            "cyclic_inverse_monoid(8) Fernandes index 1",
                            "[fpsemi-examples][quick]") {
      auto   rg = ReportGuard(REPORT);
      size_t n  = 8;
      auto   s  = cyclic_inverse_monoid(n, author::Fernandes, 1);
      auto   p  = make<Presentation<word_type>>(s);
      p.alphabet(3);
      presentation::replace_word(p, word_type({}), {2});
      presentation::add_identity_rules(p, 2);
      p.validate();

      ToddCoxeter tc(congruence_kind::twosided);
      tc.set_number_of_generators(3);
      for (size_t i = 0; i < p.rules.size() - 1; i += 2) {
        tc.add_pair(p.rules[i], p.rules[i + 1]);
      }
      REQUIRE(tc.number_of_classes() == 2041);
    }

    LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                            "058",
                            "cyclic_inverse_monoid Fernandes index 0",
                            "[fpsemi-examples][quick]") {
      auto rg = ReportGuard(REPORT);
      for (size_t n = 3; n < 10; ++n) {
        auto p = make<Presentation<word_type>>(
            fpsemigroup::cyclic_inverse_monoid(n, author::Fernandes, 0));
        REQUIRE(p.rules.size() == (n * n + 3 * n + 4));
        p.alphabet(n + 2);
        presentation::replace_word(p, word_type({}), {n + 1});
        presentation::add_identity_rules(p, n + 1);
        p.alphabet_from_rules();
        p.validate();

        ToddCoxeter tc(congruence_kind::twosided);
        tc.set_number_of_generators(n + 2);
        for (size_t i = 0; i < p.rules.size() - 1; i += 2) {
          tc.add_pair(p.rules[i], p.rules[i + 1]);
        }
        REQUIRE(tc.number_of_classes() == n * std::pow(2, n) - n + 1);
      }
    }

    LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                            "059",
                            "order_preserving_cyclic_inverse_monoid(4)",
                            "[fpsemi-examples][quick]") {
      auto   rg = ReportGuard(REPORT);
      size_t n  = 4;
      auto   s  = order_preserving_cyclic_inverse_monoid(n);
      auto   p  = make<Presentation<word_type>>(s);
      p.alphabet(n + 1);
      presentation::replace_word(p, word_type({}), {n});
      presentation::add_identity_rules(p, n);
      p.validate();

      ToddCoxeter tc(congruence_kind::twosided);
      tc.set_number_of_generators(n + 1);
      for (size_t i = 0; i < p.rules.size() - 1; i += 2) {
        tc.add_pair(p.rules[i], p.rules[i + 1]);
      }
      REQUIRE(tc.number_of_classes() == 38);
    }

    LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                            "060",
                            "order_preserving_cyclic_inverse_monoid(10)",
                            "[fpsemi-examples][quick]") {
      auto   rg = ReportGuard(REPORT);
      size_t n  = 11;
      auto   s  = order_preserving_cyclic_inverse_monoid(n);
      auto   p  = make<Presentation<word_type>>(s);
      p.alphabet(n + 1);
      presentation::replace_word(p, word_type({}), {n});
      presentation::add_identity_rules(p, n);
      p.validate();

      ToddCoxeter tc(congruence_kind::twosided);
      tc.set_number_of_generators(n + 1);
      for (size_t i = 0; i < p.rules.size() - 1; i += 2) {
        tc.add_pair(p.rules[i], p.rules[i + 1]);
      }
      REQUIRE(tc.number_of_classes() == 6120);
    }

    LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                            "066",
                            "order_preserving_cyclic_inverse_monoid(10)",
                            "[fpsemi-examples][quick]") {
      auto   rg = ReportGuard(REPORT);
      size_t n  = 11;
      auto   s  = order_preserving_cyclic_inverse_monoid(n);
      auto   p  = make<Presentation<word_type>>(s);
      p.alphabet(n + 1);
      presentation::replace_word(p, word_type({}), {n});
      presentation::add_identity_rules(p, n);
      p.validate();

      ToddCoxeter tc(congruence_kind::twosided);
      tc.set_number_of_generators(n + 1);
      for (size_t i = 0; i < p.rules.size() - 1; i += 2) {
        tc.add_pair(p.rules[i], p.rules[i + 1]);
      }
      REQUIRE(tc.number_of_classes() == 6120);
    }

    LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                            "067",
                            "partial_isometries_cycle_graph_monoid(5)",
                            "[fpsemi-examples][quick]") {
      auto   rg = ReportGuard(REPORT);
      size_t n  = 5;
      auto   s  = partial_isometries_cycle_graph_monoid(n);
      REQUIRE(s.size() == 16);
      auto p = make<Presentation<word_type>>(s);
      p.alphabet(4);
      presentation::replace_word(p, word_type({}), {3});
      presentation::add_identity_rules(p, 3);
      p.validate();

      ToddCoxeter tc(congruence_kind::twosided);
      tc.set_number_of_generators(4);
      for (size_t i = 0; i < p.rules.size() - 1; i += 2) {
        tc.add_pair(p.rules[i], p.rules[i + 1]);
      }
      REQUIRE(tc.number_of_classes() == 286);
    }

    LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                            "070",
                            "partial_isometries_cycle_graph_monoid(4)",
                            "[fpsemi-examples][quick]") {
      auto   rg = ReportGuard(REPORT);
      size_t n  = 4;
      auto   s  = partial_isometries_cycle_graph_monoid(n);
      REQUIRE(s.size() == 13);
      auto p = make<Presentation<word_type>>(s);
      p.alphabet(4);
      presentation::replace_word(p, word_type({}), {3});
      presentation::add_identity_rules(p, 3);
      p.validate();

      ToddCoxeter tc(congruence_kind::twosided);
      tc.set_number_of_generators(4);
      for (size_t i = 0; i < p.rules.size() - 1; i += 2) {
        tc.add_pair(p.rules[i], p.rules[i + 1]);
      }
      REQUIRE(tc.number_of_classes() == 97);
    }

    LIBSEMIGROUPS_TEST_CASE(
        "fpsemi-examples",
        "068",
        "partial_isometries_cycle_graph_monoid(10)",
        "[fpsemi-examples][quick][no-valgrind][no-coverage]") {
      auto   rg = ReportGuard(REPORT);
      size_t n  = 10;
      auto   s  = partial_isometries_cycle_graph_monoid(n);
      REQUIRE(s.size() == 52);
      auto p = make<Presentation<word_type>>(s);
      p.alphabet(4);
      presentation::replace_word(p, word_type({}), {3});
      presentation::add_identity_rules(p, 3);
      p.validate();

      ToddCoxeter tc(congruence_kind::twosided);
      tc.set_number_of_generators(4);
      for (size_t i = 0; i < p.rules.size() - 1; i += 2) {
        tc.add_pair(p.rules[i], p.rules[i + 1]);
      }
      REQUIRE(tc.number_of_classes() == 20'311);
    }

    LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                            "051",
                            "not_symmetric_group(4)",
                            "[fpsemi-examples][standard]") {
      auto   rg = ReportGuard(REPORT);
      size_t n  = 4;
      auto   s  = not_symmetric_group(n,
                                   author::Guralnick + author::Kantor
                                       + author::Kassabov + author::Lubotzky);
      auto   p  = make<Presentation<word_type>>(s);
      p.alphabet(n);
      presentation::replace_word(p, word_type({}), {n - 1});
      presentation::add_identity_rules(p, n - 1);
      p.validate();

      ToddCoxeter tc(congruence_kind::twosided);
      tc.set_number_of_generators(n);
      for (size_t i = 0; i < p.rules.size() - 1; i += 2) {
        tc.add_pair(p.rules[i], p.rules[i + 1]);
      }
      REQUIRE(tc.number_of_classes() == 72);
    }

  }  // namespace congruence
}  // namespace libsemigroups
