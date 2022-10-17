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

// This file is the first of two containing tests for the fpsemi-examples
// functions. The presentations here define finite semigroups, and we use
// ToddCoxeter in testing them.

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

  namespace congruence {
    LIBSEMIGROUPS_TEST_CASE("fpsemi-examples",
                            "001",
                            "full_transformation_monoid(5)",
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
                            "partial_transformation_monoid(5)",
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
                            "007",
                            "symmetric_inverse_monoid(5)",
                            "[fpsemi-examples][quick]") {
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
                            "[fpsemi-examples][quick]") {
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
                            "symmetric_group(6) Moore",
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
                            "[fpsemi-examples][quick]") {
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
                            "[fpsemi-examples][quick]") {
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
                            "[fpsemi-examples][quick]") {
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
                            "[fpsemi-examples][quick]") {
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
      REQUIRE(p.rules != p.rules);
      auto        m = p.alphabet().size();
      ToddCoxeter tc(congruence_kind::twosided);
      tc.set_number_of_generators(m);
      for (size_t i = 0; i < p.rules.size() - 1; i += 2) {
        tc.add_pair(p.rules[i], p.rules[i + 1]);
      }
      REQUIRE(tc.number_of_classes() == 130922);
    }
  }  // namespace congruence
}  // namespace libsemigroups
