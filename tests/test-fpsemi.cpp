//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019 James D. Mitchell
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

// The purpose of this file is to provide unit tests for the FpSemigroup class.

#include "catch.hpp"                 // for LIBSEMIGROUPS_TEST_CASE
#include "fpsemi-examples.hpp"       // for RennerTypeDMonoid, RennerTypeBMonoid
#include "libsemigroups/fpsemi.hpp"  // for FpSemigroup
#include "libsemigroups/froidure-pin-base.hpp"  // for FroidurePinBase
#include "libsemigroups/froidure-pin.hpp"  // for FroidurePin<>::element_index_type
#include "libsemigroups/knuth-bendix.hpp"  // for KnuthBendix
#include "libsemigroups/report.hpp"        // for ReportGuard
#include "libsemigroups/todd-coxeter.hpp"  // for ToddCoxeter
#include "libsemigroups/transf.hpp"        // for Transf<>
#include "libsemigroups/types.hpp"         // for relation_type
#include "test-main.hpp"

namespace libsemigroups {

  constexpr bool REPORT = false;

  constexpr congruence_kind twosided = congruence_kind::twosided;

  LIBSEMIGROUPS_TEST_CASE("FpSemigroup",
                          "001",
                          "Renner monoid type B2 (E. G. presentation), q = 1",
                          "[quick][fpsemi][hivert]") {
    auto        rg = ReportGuard(REPORT);
    FpSemigroup S;
    S.set_alphabet(6);

    for (relation_type const& rl : EGTypeBMonoid(2, 1)) {
      S.add_rule(rl);
    }
    REQUIRE(!S.is_obviously_infinite());
    REQUIRE(!S.is_obviously_finite());
    REQUIRE(!S.started());
    REQUIRE(!S.finished());
    REQUIRE(S.has_knuth_bendix());
    REQUIRE(S.has_todd_coxeter());
    REQUIRE(S.size() == 57);
    REQUIRE(S.started());
    REQUIRE(S.finished());
    REQUIRE(S.is_obviously_finite());
    if (!S.has_knuth_bendix()) {
      REQUIRE(S.has_todd_coxeter());
    } else {
      REQUIRE(S.has_knuth_bendix());
    }
  }

  LIBSEMIGROUPS_TEST_CASE("FpSemigroup",
                          "002",
                          "Renner monoid type B2 (E. G. presentation), q = 0",
                          "[quick][fpsemi][hivert]") {
    auto        rg = ReportGuard(REPORT);
    FpSemigroup S;
    S.set_alphabet(6);
    for (relation_type const& rl : EGTypeBMonoid(2, 0)) {
      S.add_rule(rl);
    }
    REQUIRE(!S.is_obviously_infinite());
    REQUIRE(!S.is_obviously_finite());
    REQUIRE(S.size() == 57);
    REQUIRE(S.is_obviously_finite());
  }

  // Loops for ever: Infinite monoid ???
  LIBSEMIGROUPS_TEST_CASE("FpSemigroup",
                          "003",
                          "Renner monoid type B3 (E. G. presentation), q = 1",
                          "[quick][fpsemi][hivert][no-valgrind]") {
    auto        rg = ReportGuard(REPORT);
    FpSemigroup S;
    S.set_alphabet(8);
    S.max_threads(2);
    for (relation_type const& rl : EGTypeBMonoid(3, 1)) {
      S.add_rule(rl);
    }
    REQUIRE(!S.is_obviously_infinite());
    REQUIRE(!S.is_obviously_finite());
    S.froidure_pin()->enumerate(8000);
    REQUIRE(S.froidure_pin()->current_size() == 8200);
    REQUIRE(S.started());
    // REQUIRE(S.size() == 757);
  }

  // Loops for ever: Infinite monoid ???
  LIBSEMIGROUPS_TEST_CASE("FpSemigroup",
                          "004",
                          "Renner monoid type B3 (E. G. presentation), q = 0",
                          "[quick][fpsemi][hivert][no-valgrind]") {
    auto        rg = ReportGuard(REPORT);
    FpSemigroup S;
    S.set_alphabet(8);
    S.max_threads(2);
    for (relation_type const& rl : EGTypeBMonoid(3, 0)) {
      S.add_rule(rl);
    }
    REQUIRE(!S.is_obviously_infinite());
    S.froidure_pin()->enumerate(8000);
    REQUIRE(S.froidure_pin()->current_size() == 8200);
    // REQUIRE(S.size() == 757);
  }

  LIBSEMIGROUPS_TEST_CASE(
      "FpSemigroup",
      "005",
      "Renner monoid type B2 (Gay-Hivert presentation), q = 1",
      "[quick][fpsemi][hivert]") {
    auto        rg = ReportGuard(REPORT);
    FpSemigroup S;
    S.set_alphabet(6);
    for (relation_type const& rl : RennerTypeBMonoid(2, 1)) {
      S.add_rule(rl);
    }
    REQUIRE(!S.is_obviously_infinite());
    S.run();
    REQUIRE(S.finished());
    REQUIRE(S.size() == 57);
  }

  LIBSEMIGROUPS_TEST_CASE(
      "FpSemigroup",
      "006",
      "Renner monoid type B2 (Gay-Hivert presentation), q = 0",
      "[quick][fpsemi][hivert]") {
    auto        rg = ReportGuard(REPORT);
    FpSemigroup S;
    S.set_alphabet(6);
    for (relation_type const& rl : RennerTypeBMonoid(2, 0)) {
      S.add_rule(rl);
    }
    REQUIRE(!S.is_obviously_infinite());
    REQUIRE(S.size() == 57);
  }

  LIBSEMIGROUPS_TEST_CASE(
      "FpSemigroup",
      "007",
      "Renner monoid type B3 (Gay-Hivert presentation), q = 1",
      "[quick][fpsemi][hivert][no-valgrind]") {
    auto        rg = ReportGuard(REPORT);
    FpSemigroup S;
    S.set_alphabet(8);
    for (relation_type const& rl : RennerTypeBMonoid(3, 1)) {
      S.add_rule(rl);
    }
    REQUIRE(!S.is_obviously_infinite());
    REQUIRE(S.size() == 757);
  }

  LIBSEMIGROUPS_TEST_CASE(
      "FpSemigroup",
      "008",
      "Renner monoid type B3 (Gay-Hivert presentation), q = 0",
      "[quick][fpsemi][hivert][no-valgrind]") {
    auto        rg = ReportGuard(REPORT);
    FpSemigroup S;
    S.set_alphabet(8);
    for (relation_type const& rl : RennerTypeBMonoid(3, 0)) {
      S.add_rule(rl);
    }
    REQUIRE(!S.is_obviously_infinite());
    REQUIRE(S.size() == 757);
  }

  LIBSEMIGROUPS_TEST_CASE(
      "FpSemigroup",
      "009",
      "Renner monoid type B4 (Gay-Hivert presentation), q = 1",
      "[quick][fpsemi][hivert][no-valgrind]") {
    auto        rg = ReportGuard(REPORT);
    FpSemigroup S;
    S.set_alphabet(10);
    for (relation_type const& rl : RennerTypeBMonoid(4, 1)) {
      S.add_rule(rl);
    }
    REQUIRE(S.number_of_rules() == 110);
    REQUIRE(!S.is_obviously_infinite());
    REQUIRE(!S.knuth_bendix()->confluent());
    // S.knuth_bendix()->run(); // Works too but is slower :)
    REQUIRE(S.size() == 13889);
    REQUIRE(S.froidure_pin()->number_of_rules() == 356);
  }

  LIBSEMIGROUPS_TEST_CASE(
      "FpSemigroup",
      "010",
      "Renner monoid type B4 (Gay-Hivert presentation), q = 0",
      "[quick][fpsemi][hivert][no-valgrind]") {
    auto        rg = ReportGuard(REPORT);
    FpSemigroup S;
    S.set_alphabet(10);
    for (relation_type const& rl : RennerTypeBMonoid(4, 0)) {
      S.add_rule(rl);
    }
    REQUIRE(S.number_of_rules() == 110);
    REQUIRE(!S.is_obviously_infinite());
    REQUIRE(!S.knuth_bendix()->confluent());
    // S.knuth_bendix()->run(); // Works too :)
    REQUIRE(S.size() == 13889);
    REQUIRE(S.froidure_pin()->number_of_rules() == 356);
  }

  // This appears to be an example where KB + FP is faster than TC
  LIBSEMIGROUPS_TEST_CASE(
      "FpSemigroup",
      "011",
      "Renner monoid type B5 (Gay-Hivert presentation), q = 1",
      "[extreme][fpsemi][hivert]") {
    auto        rg = ReportGuard(true);
    FpSemigroup S;
    S.set_alphabet(12);
    for (relation_type const& rl : RennerTypeBMonoid(5, 1)) {
      S.add_rule(rl);
    }
    REQUIRE(S.number_of_rules() == 159);
    REQUIRE(!S.is_obviously_infinite());
    REQUIRE(!S.knuth_bendix()->confluent());
    // S.todd_coxeter()->run(); // Takes 2m30s or so to run
    REQUIRE(S.size() == 322021);
    REQUIRE(S.froidure_pin()->number_of_rules() == 1453);
    {
      congruence::ToddCoxeter tc(
          twosided,
          S.froidure_pin(),
          congruence::ToddCoxeter::options::froidure_pin::use_cayley_graph);
      REQUIRE(tc.number_of_classes() == 322021);  // Works!
    }
    {
      fpsemigroup::ToddCoxeter tc(S.froidure_pin());
      REQUIRE(tc.number_of_rules() == 1453);
      REQUIRE(tc.size() == 322021);
    }
  }

  LIBSEMIGROUPS_TEST_CASE(
      "FpSemigroup",
      "012",
      "Renner monoid type B5 (Gay-Hivert presentation), q = 0",
      "[extreme][fpsemi][hivert]") {
    auto        rg = ReportGuard(true);
    FpSemigroup S;
    S.set_alphabet(12);
    for (relation_type const& rl : RennerTypeBMonoid(5, 0)) {
      S.add_rule(rl);
    }
    REQUIRE(S.number_of_rules() == 159);
    REQUIRE(!S.is_obviously_infinite());
    REQUIRE(!S.knuth_bendix()->confluent());
    // Doesn't terminate, or show signs that it will, in 5 minutes or so
    // S.todd_coxeter()->run();
    REQUIRE(S.size() == 322021);
    REQUIRE(S.froidure_pin()->number_of_rules() == 1453);

    congruence::ToddCoxeter tc(
        twosided,
        S.froidure_pin(),
        congruence::ToddCoxeter::options::froidure_pin::use_cayley_graph);
    REQUIRE(tc.number_of_classes() == 322021);  // Works!
  }

  LIBSEMIGROUPS_TEST_CASE("FpSemigroup",
                          "013",
                          "Renner monoid type D2 (E. G. presentation), q = 1",
                          "[quick][fpsemi][hivert]") {
    auto        rg = ReportGuard(REPORT);
    FpSemigroup S;
    S.set_alphabet(7);
    for (relation_type const& rl : EGTypeDMonoid(2, 1)) {
      S.add_rule(rl);
    }
    REQUIRE(S.number_of_rules() == 44);
    REQUIRE(!S.is_obviously_infinite());
    REQUIRE(!S.knuth_bendix()->confluent());
    // S.knuth_bendix()->run(); // Works too :)
    REQUIRE(S.size() == 37);
    REQUIRE(S.froidure_pin()->number_of_rules() == 54);
  }

  LIBSEMIGROUPS_TEST_CASE("FpSemigroup",
                          "014",
                          "Renner monoid type D2 (E. G. presentation), q = 0",
                          "[quick][fpsemi][hivert]") {
    auto        rg = ReportGuard(REPORT);
    FpSemigroup S;
    S.set_alphabet(7);
    for (relation_type const& rl : EGTypeDMonoid(2, 0)) {
      S.add_rule(rl);
    }
    REQUIRE(S.number_of_rules() == 44);
    REQUIRE(!S.is_obviously_infinite());
    REQUIRE(!S.knuth_bendix()->confluent());
    // S.knuth_bendix()->run(); // Works too :)
    REQUIRE(S.size() == 37);
    REQUIRE(S.froidure_pin()->number_of_rules() == 54);
  }

  LIBSEMIGROUPS_TEST_CASE("FpSemigroup",
                          "015",
                          "Renner monoid type D3 (E. G. presentation), q = 1",
                          "[quick][fpsemi][hivert][no-valgrind]") {
    auto        rg = ReportGuard(REPORT);
    FpSemigroup S;
    S.set_alphabet(9);
    for (relation_type const& rl : EGTypeDMonoid(3, 1)) {
      S.add_rule(rl);
    }
    REQUIRE(S.number_of_rules() == 78);
    REQUIRE(!S.is_obviously_infinite());
    REQUIRE(!S.knuth_bendix()->confluent());
    // S.knuth_bendix()->run(); // Works too but is a bit slower :)
    REQUIRE(S.size() == 541);
    REQUIRE(S.froidure_pin()->number_of_rules() == 148);
  }

  LIBSEMIGROUPS_TEST_CASE("FpSemigroup",
                          "016",
                          "Renner monoid type D3 (E. G. presentation), q = 0",
                          "[quick][fpsemi][hivert][no-valgrind]") {
    auto        rg = ReportGuard(REPORT);
    FpSemigroup S;
    S.set_alphabet(9);
    for (relation_type const& rl : EGTypeDMonoid(3, 0)) {
      S.add_rule(rl);
    }
    REQUIRE(S.number_of_rules() == 78);
    REQUIRE(!S.is_obviously_infinite());
    REQUIRE(!S.knuth_bendix()->confluent());
    // S.knuth_bendix()->run(); // Works too but is a bit slower :)
    REQUIRE(S.size() == 541);
    REQUIRE(S.froidure_pin()->number_of_rules() == 148);
  }

  LIBSEMIGROUPS_TEST_CASE("FpSemigroup",
                          "017",
                          "Renner monoid type D4 (E. G. presentation), q = 1",
                          "[quick][fpsemi][hivert][no-valgrind]") {
    auto        rg = ReportGuard(REPORT);
    FpSemigroup S;
    S.set_alphabet(11);
    S.max_threads(2);
    for (relation_type const& rl : EGTypeDMonoid(4, 1)) {
      S.add_rule(rl);
    }
    REQUIRE(S.number_of_rules() == 119);
    REQUIRE(!S.is_obviously_infinite());
    REQUIRE(!S.knuth_bendix()->confluent());

    REQUIRE(S.size() == POSITIVE_INFINITY);

    S.froidure_pin()->enumerate(10626);
    REQUIRE(S.froidure_pin()->current_number_of_rules() == 417);
    REQUIRE(S.froidure_pin()->current_size() == 10626);
    // REQUIRE(S.size() == 10625); // Runs forever
  }

  LIBSEMIGROUPS_TEST_CASE("FpSemigroup",
                          "018",
                          "Renner monoid type D4 (E. G. presentation), q = 0",
                          "[quick][fpsemi][hivert][no-valgrind]") {
    auto        rg = ReportGuard(REPORT);
    FpSemigroup S;
    S.set_alphabet(11);
    S.max_threads(2);
    for (relation_type const& rl : EGTypeDMonoid(4, 0)) {
      S.add_rule(rl);
    }
    REQUIRE(S.number_of_rules() == 119);
    REQUIRE(!S.is_obviously_infinite());
    REQUIRE(!S.knuth_bendix()->confluent());

    S.froidure_pin()->enumerate(10626);
    REQUIRE(S.froidure_pin()->current_number_of_rules() == 417);
    REQUIRE(S.froidure_pin()->current_size() == 10626);
    // REQUIRE(S.size() == 10625); // Runs forever
  }

  LIBSEMIGROUPS_TEST_CASE(
      "FpSemigroup",
      "019",
      "Renner monoid type D2 (Gay-Hivert presentation), q = 1",
      "[quick][fpsemi][hivert]") {
    auto        rg = ReportGuard(REPORT);
    FpSemigroup S;
    S.set_alphabet(7);
    for (relation_type const& rl : RennerTypeDMonoid(2, 1)) {
      S.add_rule(rl);
    }
    REQUIRE(S.number_of_rules() == 44);
    REQUIRE(!S.is_obviously_infinite());
    REQUIRE(!S.knuth_bendix()->confluent());
    // S.knuth_bendix()->run(); // Works too :)
    REQUIRE(S.size() == 37);
    REQUIRE(S.froidure_pin()->number_of_rules() == 54);
  }

  LIBSEMIGROUPS_TEST_CASE(
      "FpSemigroup",
      "020",
      "Renner monoid type D2 (Gay-Hivert presentation), q = 0",
      "[quick][fpsemi][hivert]") {
    auto        rg = ReportGuard(REPORT);
    FpSemigroup S;
    S.set_alphabet(7);
    for (relation_type const& rl : RennerTypeDMonoid(2, 0)) {
      S.add_rule(rl);
    }
    REQUIRE(S.number_of_rules() == 44);
    REQUIRE(!S.is_obviously_infinite());
    REQUIRE(!S.knuth_bendix()->confluent());
    // S.knuth_bendix()->run(); // Works too :)
    REQUIRE(S.size() == 37);
    REQUIRE(S.froidure_pin()->number_of_rules() == 54);
  }

  LIBSEMIGROUPS_TEST_CASE(
      "FpSemigroup",
      "021",
      "Renner monoid type D3 (Gay-Hivert presentation), q = 1",
      "[quick][fpsemi][hivert][no-valgrind]") {
    auto        rg = ReportGuard(REPORT);
    FpSemigroup S;
    S.set_alphabet(9);
    for (relation_type const& rl : RennerTypeDMonoid(3, 1)) {
      S.add_rule(rl);
    }
    REQUIRE(S.number_of_rules() == 78);
    REQUIRE(!S.is_obviously_infinite());
    REQUIRE(!S.knuth_bendix()->confluent());
    // S.knuth_bendix()->run(); // Works too but is a bit slower :)
    REQUIRE(S.size() == 541);
    REQUIRE(S.froidure_pin()->number_of_rules() == 148);
  }

  LIBSEMIGROUPS_TEST_CASE(
      "FpSemigroup",
      "022",
      "Renner monoid type D3 (Gay-Hivert presentation), q = 0",
      "[quick][fpsemi][hivert][no-valgrind]") {
    auto        rg = ReportGuard(REPORT);
    FpSemigroup S;
    S.set_alphabet(9);
    for (relation_type const& rl : RennerTypeDMonoid(3, 0)) {
      S.add_rule(rl);
    }
    REQUIRE(S.number_of_rules() == 78);
    REQUIRE(!S.is_obviously_infinite());
    REQUIRE(!S.knuth_bendix()->confluent());
    // S.knuth_bendix()->run(); // Works too but is a bit slower :)
    REQUIRE(S.size() == 541);
    REQUIRE(S.froidure_pin()->number_of_rules() == 148);
  }

  LIBSEMIGROUPS_TEST_CASE(
      "FpSemigroup",
      "023",
      "Renner monoid type D4 (Gay-Hivert presentation), q = 1",
      "[quick][fpsemi][hivert][no-valgrind]") {
    auto        rg = ReportGuard(REPORT);
    FpSemigroup S;
    S.set_alphabet(11);
    for (relation_type const& rl : RennerTypeDMonoid(4, 1)) {
      S.add_rule(rl);
    }
    REQUIRE(S.number_of_rules() == 121);
    REQUIRE(!S.is_obviously_infinite());
    REQUIRE(!S.knuth_bendix()->confluent());

    REQUIRE(S.size() == 10625);
    REQUIRE(S.froidure_pin()->number_of_rules() == 419);
  }

  LIBSEMIGROUPS_TEST_CASE(
      "FpSemigroup",
      "024",
      "Renner monoid type D4 (Gay-Hivert presentation), q = 0",
      "[quick][fpsemi][hivert][no-valgrind]") {
    auto        rg = ReportGuard(false);
    FpSemigroup S;
    S.set_alphabet(11);
    for (relation_type const& rl : RennerTypeDMonoid(4, 0)) {
      S.add_rule(rl);
    }
    REQUIRE(S.number_of_rules() == 121);
    REQUIRE(!S.is_obviously_infinite());
    REQUIRE(!S.knuth_bendix()->confluent());
    REQUIRE(S.size() == 10625);
    REQUIRE(S.froidure_pin()->number_of_rules() == 419);
  }

  LIBSEMIGROUPS_TEST_CASE(
      "FpSemigroup",
      "025",
      "Renner monoid type D5 (Gay-Hivert presentation), q = 1",
      "[extreme][fpsemi][hivert]") {
    auto        rg = ReportGuard(true);
    FpSemigroup S;
    S.set_alphabet(13);
    for (relation_type const& rl : RennerTypeDMonoid(5, 1)) {
      S.add_rule(rl);
    }
    REQUIRE(S.number_of_rules() == 173);
    REQUIRE(!S.is_obviously_infinite());
    REQUIRE(!S.knuth_bendix()->confluent());

    REQUIRE(S.size() == 258661);
    REQUIRE(S.froidure_pin()->number_of_rules() == 1279);
  }

  LIBSEMIGROUPS_TEST_CASE(
      "FpSemigroup",
      "026",
      "Renner monoid type D5 (Gay-Hivert presentation), q = 0",
      "[extreme][fpsemi][hivert]") {
    auto        rg = ReportGuard(true);
    FpSemigroup S;
    S.set_alphabet(13);
    for (relation_type const& rl : RennerTypeDMonoid(5, 0)) {
      S.add_rule(rl);
    }
    REQUIRE(S.number_of_rules() == 173);
    REQUIRE(!S.is_obviously_infinite());
    REQUIRE(!S.knuth_bendix()->confluent());
    REQUIRE(S.size() == 258661);
    REQUIRE(S.froidure_pin()->number_of_rules() == 1279);
  }

  // Takes about 4 minutes
  LIBSEMIGROUPS_TEST_CASE(
      "FpSemigroup",
      "027",
      "Renner monoid type D6 (Gay-Hivert presentation), q = 1",
      "[extreme][fpsemi][hivert]") {
    auto        rg = ReportGuard(true);
    FpSemigroup S;
    S.set_alphabet(15);
    for (relation_type const& rl : RennerTypeDMonoid(6, 1)) {
      S.add_rule(rl);
    }
    REQUIRE(S.number_of_rules() == 234);
    REQUIRE(!S.is_obviously_infinite());
    REQUIRE(!S.knuth_bendix()->confluent());

    REQUIRE(S.size() == 7464625);
    REQUIRE(S.froidure_pin()->number_of_rules() == 4570);
  }

  // Takes about 4 minutes
  LIBSEMIGROUPS_TEST_CASE(
      "FpSemigroup",
      "028",
      "Renner monoid type D6 (Gay-Hivert presentation), q = 0",
      "[extreme][fpsemi][hivert]") {
    auto        rg = ReportGuard(true);
    FpSemigroup S;
    S.set_alphabet(15);
    for (relation_type const& rl : RennerTypeDMonoid(6, 0)) {
      S.add_rule(rl);
    }
    REQUIRE(S.number_of_rules() == 234);
    REQUIRE(!S.is_obviously_infinite());
    REQUIRE(!S.knuth_bendix()->confluent());
    S.knuth_bendix()->knuth_bendix_by_overlap_length();
    REQUIRE(S.size() == 7464625);
    REQUIRE(S.froidure_pin()->number_of_rules() == 4570);
  }

  LIBSEMIGROUPS_TEST_CASE("FpSemigroup",
                          "029",
                          "Rook monoid R5, q = 0",
                          "[quick][fpsemi][no-valgrind]") {
    auto        rg = ReportGuard(REPORT);
    FpSemigroup S;
    S.set_alphabet(6);
    for (relation_type const& rl : RookMonoid(5, 0)) {
      S.add_rule(rl);
    }
    REQUIRE(S.number_of_rules() == 33);
    REQUIRE(!S.is_obviously_infinite());
    REQUIRE(!S.knuth_bendix()->confluent());
    REQUIRE(S.size() == 1546);
    REQUIRE(S.froidure_pin()->number_of_rules() == 71);
  }

  LIBSEMIGROUPS_TEST_CASE("FpSemigroup",
                          "030",
                          "Rook monoid R5, q = 1",
                          "[quick][fpsemi][no-valgrind]") {
    auto        rg = ReportGuard(REPORT);
    FpSemigroup S;
    S.set_alphabet(6);
    for (relation_type const& rl : RookMonoid(5, 1)) {
      S.add_rule(rl);
    }
    REQUIRE(S.number_of_rules() == 33);
    REQUIRE(!S.is_obviously_infinite());
    REQUIRE(!S.knuth_bendix()->confluent());
    REQUIRE(S.size() == 1546);
    REQUIRE(S.froidure_pin()->number_of_rules() == 71);
  }

  LIBSEMIGROUPS_TEST_CASE("FpSemigroup",
                          "031",
                          "Rook monoid R6, q = 0",
                          "[quick][fpsemi][no-valgrind]") {
    auto        rg = ReportGuard(REPORT);
    FpSemigroup S;
    S.set_alphabet(7);
    for (relation_type const& rl : RookMonoid(6, 0)) {
      S.add_rule(rl);
    }
    REQUIRE(S.number_of_rules() == 45);
    REQUIRE(!S.is_obviously_infinite());
    REQUIRE(!S.knuth_bendix()->confluent());
    REQUIRE(S.size() == 13327);
    REQUIRE(S.froidure_pin()->number_of_rules() == 207);
  }

  LIBSEMIGROUPS_TEST_CASE("FpSemigroup",
                          "032",
                          "Rook monoid R6, q = 1",
                          "[quick][fpsemi][no-valgrind]") {
    auto        rg = ReportGuard(REPORT);
    FpSemigroup S;
    S.set_alphabet(7);
    for (relation_type const& rl : RookMonoid(6, 1)) {
      S.add_rule(rl);
    }
    REQUIRE(S.number_of_rules() == 45);
    REQUIRE(!S.is_obviously_infinite());
    REQUIRE(!S.knuth_bendix()->confluent());
    REQUIRE(S.size() == 13327);
    REQUIRE(S.froidure_pin()->number_of_rules() == 207);
  }

  LIBSEMIGROUPS_TEST_CASE("FpSemigroup",
                          "033",
                          "normal_form",
                          "[quick][fpsemi]") {
    auto rg = ReportGuard(REPORT);

    FpSemigroup S;
    S.set_alphabet(2);
    S.add_rule(relation_type({0, 0, 0}, {0}));
    S.add_rule(relation_type({0}, {1, 1}));

    REQUIRE(S.size() == 5);

    REQUIRE(S.normal_form({0, 0, 1}) == word_type({0, 0, 1}));
    REQUIRE(S.normal_form({0, 0, 0, 0, 1}) == word_type({0, 0, 1}));
    REQUIRE(S.normal_form({0, 1, 1, 0, 0, 1}) == word_type({0, 0, 1}));
    REQUIRE(S.normal_form({0, 0, 0}) == word_type({0}));
    REQUIRE(S.normal_form({1}) == word_type({1}));
  }

  LIBSEMIGROUPS_TEST_CASE("FpSemigroup",
                          "034",
                          "for a finite semigroup",
                          "[quick][fpsemi]") {
    auto rg = ReportGuard(REPORT);

    FroidurePin<LeastTransf<5>> S(
        {LeastTransf<5>({1, 3, 4, 2, 3}), LeastTransf<5>({3, 2, 1, 3, 3})});

    REQUIRE(S.size() == 88);
    REQUIRE(S.number_of_rules() == 18);

    FpSemigroup T(S);
    REQUIRE(T.number_of_rules() == 18);
    T.add_rule(S.factorisation(LeastTransf<5>({3, 4, 4, 4, 4})),
               S.factorisation(LeastTransf<5>({3, 1, 3, 3, 3})));
    REQUIRE(T.number_of_rules() == 19);

    REQUIRE(T.size() == 21);
    REQUIRE(T.equal_to(S.factorisation(LeastTransf<5>({1, 3, 1, 3, 3})),
                       S.factorisation(LeastTransf<5>({4, 2, 4, 4, 2}))));
    REQUIRE(T.normal_form(S.factorisation(LeastTransf<5>({1, 3, 1, 3, 3})))
            == T.normal_form(S.factorisation(LeastTransf<5>({4, 2, 4, 4, 2}))));
  }

  LIBSEMIGROUPS_TEST_CASE("FpSemigroup",
                          "035",
                          "finite fp semigroup, dihedral group of order 6",
                          "[quick][fpsemi]") {
    auto        rg = ReportGuard(REPORT);
    FpSemigroup S;
    S.set_alphabet("abcde");
    S.add_rule("aa", "a");
    S.add_rule("ab", "b");
    S.add_rule("ba", "b");
    S.add_rule("ac", "c");
    S.add_rule("ca", "c");
    S.add_rule("ad", "d");
    S.add_rule("da", "d");
    S.add_rule("ae", "e");
    S.add_rule("ea", "e");
    S.add_rule("bc", "a");
    S.add_rule("cb", "a");
    S.add_rule("de", "a");
    S.add_rule("ed", "a");
    S.add_rule("cc", "a");
    S.add_rule("becdd", "a");
    S.add_rule("eee", "a");

    REQUIRE(S.size() == 6);
    REQUIRE(S.equal_to("b", "c"));
  }

  LIBSEMIGROUPS_TEST_CASE("FpSemigroup",
                          "036",
                          "finite fp semigroup, size 16",
                          "[quick][fpsemi][kbfp]") {
    auto        rg = ReportGuard(REPORT);
    FpSemigroup S;
    S.set_alphabet("0123");

    S.add_rule("3", "2");
    S.add_rule("03", "02");
    S.add_rule("11", "1");
    S.add_rule("13", "12");
    S.add_rule("21", "2");
    S.add_rule("22", "2");
    S.add_rule("23", "2");
    S.add_rule("000", "0");
    S.add_rule("001", "1");
    S.add_rule("002", "2");
    S.add_rule("012", "12");
    S.add_rule("100", "1");
    S.add_rule("102", "02");
    S.add_rule("200", "2");
    S.add_rule("0101", "101");
    S.add_rule("0202", "202");
    S.add_rule("1010", "101");
    S.add_rule("1201", "101");
    S.add_rule("1202", "202");
    S.add_rule("2010", "201");
    S.add_rule("2020", "202");

    REQUIRE(S.size() == 16);
    REQUIRE(S.equal_to("2", "3"));
  }

  LIBSEMIGROUPS_TEST_CASE("FpSemigroup",
                          "037",
                          "finite fp semigroup, size 16",
                          "[quick][fpsemi]") {
    auto        rg = ReportGuard(REPORT);
    FpSemigroup S;
    S.set_alphabet(11);

    S.add_rule(relation_type({2}, {1}));
    S.add_rule(relation_type({4}, {3}));
    S.add_rule(relation_type({5}, {0}));
    S.add_rule(relation_type({6}, {3}));
    S.add_rule(relation_type({7}, {1}));
    S.add_rule(relation_type({8}, {3}));
    S.add_rule(relation_type({9}, {3}));
    S.add_rule(relation_type({10}, {0}));
    S.add_rule(relation_type({0, 2}, {0, 1}));
    S.add_rule(relation_type({0, 4}, {0, 3}));
    S.add_rule(relation_type({0, 5}, {0, 0}));
    S.add_rule(relation_type({0, 6}, {0, 3}));
    S.add_rule(relation_type({0, 7}, {0, 1}));
    S.add_rule(relation_type({0, 8}, {0, 3}));
    S.add_rule(relation_type({0, 9}, {0, 3}));
    S.add_rule(relation_type({0, 10}, {0, 0}));
    S.add_rule(relation_type({1, 1}, {1}));
    S.add_rule(relation_type({1, 2}, {1}));
    S.add_rule(relation_type({1, 4}, {1, 3}));
    S.add_rule(relation_type({1, 5}, {1, 0}));
    S.add_rule(relation_type({1, 6}, {1, 3}));
    S.add_rule(relation_type({1, 7}, {1}));
    S.add_rule(relation_type({1, 8}, {1, 3}));
    S.add_rule(relation_type({1, 9}, {1, 3}));
    S.add_rule(relation_type({1, 10}, {1, 0}));
    S.add_rule(relation_type({3, 1}, {3}));
    S.add_rule(relation_type({3, 2}, {3}));
    S.add_rule(relation_type({3, 3}, {3}));
    S.add_rule(relation_type({3, 4}, {3}));
    S.add_rule(relation_type({3, 5}, {3, 0}));
    S.add_rule(relation_type({3, 6}, {3}));
    S.add_rule(relation_type({3, 7}, {3}));
    S.add_rule(relation_type({3, 8}, {3}));
    S.add_rule(relation_type({3, 9}, {3}));
    S.add_rule(relation_type({3, 10}, {3, 0}));
    S.add_rule(relation_type({0, 0, 0}, {0}));
    S.add_rule(relation_type({0, 0, 1}, {1}));
    S.add_rule(relation_type({0, 0, 3}, {3}));
    S.add_rule(relation_type({0, 1, 3}, {1, 3}));
    S.add_rule(relation_type({1, 0, 0}, {1}));
    S.add_rule(relation_type({1, 0, 3}, {0, 3}));
    S.add_rule(relation_type({3, 0, 0}, {3}));
    S.add_rule(relation_type({0, 1, 0, 1}, {1, 0, 1}));
    S.add_rule(relation_type({0, 3, 0, 3}, {3, 0, 3}));
    S.add_rule(relation_type({1, 0, 1, 0}, {1, 0, 1}));
    S.add_rule(relation_type({1, 3, 0, 1}, {1, 0, 1}));
    S.add_rule(relation_type({1, 3, 0, 3}, {3, 0, 3}));
    S.add_rule(relation_type({3, 0, 1, 0}, {3, 0, 1}));
    S.add_rule(relation_type({3, 0, 3, 0}, {3, 0, 3}));

    REQUIRE(S.size() == 16);
    REQUIRE(S.equal_to({0}, {5}));
    REQUIRE(S.equal_to({0}, {10}));
    REQUIRE(S.equal_to({1}, {2}));
    REQUIRE(S.equal_to({1}, {7}));
    REQUIRE(S.equal_to({3}, {4}));
    REQUIRE(S.equal_to({3}, {6}));
    REQUIRE(S.equal_to({3}, {8}));
    REQUIRE(S.equal_to({3}, {9}));
  }

  LIBSEMIGROUPS_TEST_CASE("FpSemigroup",
                          "038",
                          "fp semigroup, size 240",
                          "[quick][fpsemi]") {
    auto        rg = ReportGuard(REPORT);
    FpSemigroup S;
    S.set_alphabet("01");

    S.add_rule("000", "0");
    S.add_rule("1111", "1");
    S.add_rule("01110", "00");
    S.add_rule("1001", "11");
    S.add_rule("001010101010", "00");

    REQUIRE(S.size() == 240);
    REQUIRE(S.froidure_pin()->size() == 240);
  }

  LIBSEMIGROUPS_TEST_CASE("FpSemigroup", "039", "add_rule", "[quick][fpsemi]") {
    auto        rg = ReportGuard(REPORT);
    FpSemigroup S;
    S.set_alphabet("ab");
    REQUIRE(S.is_obviously_infinite());
    REQUIRE(S.size() == POSITIVE_INFINITY);
    S.add_rule("aaa", "a");
    S.add_rule("a", "bb");
    REQUIRE(!S.is_obviously_infinite());
    REQUIRE(S.size() == 5);

    auto T = S.froidure_pin();
    REQUIRE(T->size() == 5);
    REQUIRE(T->number_of_idempotents() == 1);
  }

  LIBSEMIGROUPS_TEST_CASE("FpSemigroup", "040", "add_rule", "[quick][fpsemi]") {
    auto        rg = ReportGuard(REPORT);
    FpSemigroup S;
    S.set_alphabet("ab");
    REQUIRE(S.is_obviously_infinite());
    S.add_rule("aaa", "a");
    S.add_rule("a", "bb");
    REQUIRE(!S.is_obviously_infinite());
    REQUIRE(S.knuth_bendix()->froidure_pin()->size() == 5);
    REQUIRE(S.size() == 5);

    auto T = S.froidure_pin();
    REQUIRE(T->size() == 5);
    REQUIRE(T->number_of_idempotents() == 1);
  }

  LIBSEMIGROUPS_TEST_CASE("FpSemigroup", "041", "equal_to", "[quick][fpsemi]") {
    auto rg = ReportGuard(REPORT);

    FpSemigroup S;
    S.set_alphabet("ab");
    S.add_rule("aa", "a");
    S.add_rule("ab", "a");
    S.add_rule("ba", "a");
    S.max_threads(2);

    REQUIRE(S.is_obviously_infinite());
    REQUIRE(S.equal_to("ab", "a"));
    REQUIRE(S.equal_to("ba", "a"));
    REQUIRE(S.equal_to("aa", "a"));
  }

  LIBSEMIGROUPS_TEST_CASE("FpSemigroup",
                          "042",
                          "cbegin/cend_rules",
                          "[quick][fpsemi]") {
    auto rg = ReportGuard(REPORT);

    FpSemigroup S;
    S.set_alphabet("ab");
    S.add_rule("aa", "a");
    S.add_rule("ab", "a");
    S.add_rule("ba", "a");

    using rules_type = std::vector<std::pair<std::string, std::string>>;
    REQUIRE(rules_type(S.cbegin_rules(), S.cend_rules())
            == rules_type({{"aa", "a"}, {"ab", "a"}, {"ba", "a"}}));
  }

  LIBSEMIGROUPS_TEST_CASE("FpSemigroup",
                          "043",
                          "semigroup of size 3",
                          "[todd-coxeter][quick]") {
    auto        rg = ReportGuard(REPORT);
    FpSemigroup S;
    S.set_alphabet("eab");
    S.set_identity("e");

    size_t const N   = 10;
    std::string  lhs = "a" + std::string(N, 'b');
    std::string  rhs = "e";
    S.add_rule(lhs, rhs);

    lhs = std::string(N, 'a');
    rhs = std::string(N + 1, 'b');
    S.add_rule(lhs, rhs);

    lhs = "ba";
    rhs = std::string(N, 'b') + "a";
    S.add_rule(lhs, rhs);

    REQUIRE(S.size() == 3);
  }

  LIBSEMIGROUPS_TEST_CASE("FpSemigroup",
                          "044",
                          "run_for/until",
                          "[fpsemigroup][quick]") {
    auto        rg = ReportGuard(REPORT);
    FpSemigroup S;
    S.set_alphabet("abce");
    S.set_identity("e");
    S.add_rule("aa", "e");
    S.add_rule("bc", "e");
    S.add_rule("bbb", "e");
    S.add_rule("ababababababab", "e");
    S.add_rule("abacabacabacabacabacabacabacabac", "e");
    S.run_for(std::chrono::nanoseconds(1));
    REQUIRE(!S.finished());
    size_t number_of_calls = 0;
    S.run_until([&number_of_calls]() {
      ++number_of_calls;
      return number_of_calls == 3;
    });
    // REQUIRE(!S.finished());
    REQUIRE(number_of_calls == 3);
  }

  LIBSEMIGROUPS_TEST_CASE("FpSemigroup",
                          "045",
                          "constructors",
                          "[fpsemigroup][quick]") {
    using Transf8           = LeastTransf<3>;
    auto                 rg = ReportGuard(REPORT);
    FroidurePin<Transf8> S({Transf8({1, 0, 1}), Transf8({0, 0, 0})});

    FpSemigroup T(S);

    REQUIRE(!T.has_froidure_pin());
    REQUIRE(T.size() == S.size());
  }

  LIBSEMIGROUPS_TEST_CASE("FpSemigroup",
                          "046",
                          "set_inverses",
                          "[fpsemigroup][quick]") {
    auto        rg = ReportGuard(REPORT);
    FpSemigroup S;
    S.set_alphabet("abAe");
    S.set_identity("e");
    REQUIRE_THROWS_AS(S.set_inverses("bAae"), LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("FpSemigroup",
                          "047",
                          "smalloverlap",
                          "[fpsemigroup][quick]") {
    auto        rg = ReportGuard(REPORT);
    FpSemigroup k;
    k.set_alphabet("abcdefg");
    k.add_rule("abcd", "aaaeaa");
    k.add_rule("ef", "dg");
    REQUIRE(k.size() == POSITIVE_INFINITY);
    REQUIRE(k.equal_to("abcd", "aaaeaa"));
    REQUIRE(k.equal_to("ef", "dg"));
    REQUIRE(k.equal_to("aaaaaef", "aaaaadg"));
    REQUIRE(k.equal_to("efababa", "dgababa"));
    k.froidure_pin()->enumerate(100);
    REQUIRE(k.froidure_pin()->current_size() == 8205);
  }

  LIBSEMIGROUPS_TEST_CASE("FpSemigroup",
                          "048",
                          "quaternion group Q8",
                          "[fpsemigroup][quick]") {
    auto        rg = ReportGuard(REPORT);
    FpSemigroup S;
    S.set_alphabet("xyXYe");
    S.set_identity("e");
    S.set_inverses("XYxye");
    S.add_rule("xxxx", "e");
    S.add_rule("xyXy", "e");
    S.add_rule("xxYY", "e");

    REQUIRE(S.size() == 8);
  }

  LIBSEMIGROUPS_TEST_CASE("FpSemigroup",
                          "049",
                          "symmetric group Coxeter presentation",
                          "[fpsemigroup][quick]") {
    size_t const N = 10;
    FpSemigroup  S;
    S.set_alphabet(N + 1);
    S.set_identity(N);
    S.set_inverses(S.alphabet());

    for (size_t i = 0; i < N; ++i) {
      S.add_rule({i, i}, {N});
    }
    for (size_t i = 0; i < N - 1; ++i) {
      S.add_rule({i, i + 1, i, i + 1, i, i + 1}, {N});
    }
    for (size_t i = 0; i < N; ++i) {
      for (size_t j = 0; j < N; ++j) {
        if (std::abs(static_cast<int>(i - j)) > 1) {
          S.add_rule({i, j, i, j}, {N});
        }
      }
    }

    REQUIRE(S.size() == 39916800);  // 11!
  }

  LIBSEMIGROUPS_TEST_CASE("FpSemigroup",
                          "050",
                          "https://math.stackexchange.com/questions/2649807",
                          "[fpsemigroup][fail]") {
    fpsemigroup::ToddCoxeter S;
    S.set_alphabet("abcABCe");
    S.set_identity("e");
    S.set_inverses("ABCabce");
    S.add_rule("aa", "e");
    S.add_rule("bbbbbbbbbbb", "e");
    S.add_rule("cc", "e");
    S.add_rule("abababab", "e");
    S.add_rule("abbabbabbabbabbabb", "e");
    S.add_rule("abbabaBabaBBabbaB", "e");
    S.add_rule("acacac", "e");
    S.add_rule("bcbc", "e");
    S.congruence().strategy(congruence::ToddCoxeter::options::strategy::random);

    REQUIRE(S.size() == 0);
  }
}  // namespace libsemigroups
