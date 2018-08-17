//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2018 James D. Mitchell
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

#include "cong.hpp"
#include "element.hpp"
#include "fpsemi-examples.hpp"
#include "fpsemi.hpp"
#include "froidure-pin.hpp"
#include "libsemigroups.tests.hpp"

namespace libsemigroups {

  constexpr bool REPORT = false;

  constexpr congruence_type TWOSIDED = congruence_type::TWOSIDED;
  constexpr congruence_type LEFT     = congruence_type::LEFT;
  constexpr congruence_type RIGHT    = congruence_type::RIGHT;

  LIBSEMIGROUPS_TEST_CASE("FpSemigroup",
                          "001",
                          "Renner monoid type B2 (E. G. presentation), q = 1",
                          "[quick][hivert]") {
    REPORTER.set_report(REPORT);
    FpSemigroup S;
    S.set_alphabet(6);

    for (relation_type const& rl : EGTypeBMonoid(2, 1)) {
      S.add_rule(rl);
    }
    REQUIRE(!S.is_obviously_infinite());
    REQUIRE(S.size() == 57);
  }

  LIBSEMIGROUPS_TEST_CASE("FpSemigroup",
                          "002",
                          "Renner monoid type B2 (E. G. presentation), q = 0",
                          "[quick][hivert]") {
    REPORTER.set_report(REPORT);
    FpSemigroup S;
    S.set_alphabet(6);
    for (relation_type const& rl : EGTypeBMonoid(2, 0)) {
      S.add_rule(rl);
    }
    REQUIRE(!S.is_obviously_infinite());
    REQUIRE(S.size() == 57);
  }

  // Loops for ever: Infinite monoid ???
  LIBSEMIGROUPS_TEST_CASE("FpSemigroup",
                          "003",
                          "Renner monoid type B3 (E. G. presentation), q = 1",
                          "[quick][hivert]") {
    REPORTER.set_report(REPORT);
    FpSemigroup S;
    S.set_alphabet(8);
    for (relation_type const& rl : EGTypeBMonoid(3, 1)) {
      S.add_rule(rl);
    }
    REQUIRE(!S.is_obviously_infinite());
    S.isomorphic_non_fp_semigroup()->enumerate(8000);
    REQUIRE(S.isomorphic_non_fp_semigroup()->current_size() == 8200);
    // REQUIRE(S.size() == 757);
  }

  // Loops for ever: Infinite monoid ???
  LIBSEMIGROUPS_TEST_CASE("FpSemigroup",
                          "004",
                          "Renner monoid type B3 (E. G. presentation), q = 0",
                          "[quick][hivert]") {
    REPORTER.set_report(REPORT);
    FpSemigroup S;
    S.set_alphabet(8);
    for (relation_type const& rl : EGTypeBMonoid(3, 0)) {
      S.add_rule(rl);
    }
    REQUIRE(!S.is_obviously_infinite());
    S.isomorphic_non_fp_semigroup()->enumerate(8000);
    REQUIRE(S.isomorphic_non_fp_semigroup()->current_size() == 8200);
    // REQUIRE(S.size() == 757);
  }

  LIBSEMIGROUPS_TEST_CASE(
      "FpSemigroup",
      "005",
      "Renner monoid type B2 (Gay-Hivert presentation), q = 1",
      "[quick][hivert]") {
    REPORTER.set_report(REPORT);
    FpSemigroup S;
    S.set_alphabet(6);
    for (relation_type const& rl : RennerTypeBMonoid(2, 1)) {
      S.add_rule(rl);
    }
    REQUIRE(!S.is_obviously_infinite());
    REQUIRE(S.size() == 57);
  }

  LIBSEMIGROUPS_TEST_CASE(
      "FpSemigroup",
      "006",
      "Renner monoid type B2 (Gay-Hivert presentation), q = 0",
      "[quick][hivert]") {
    REPORTER.set_report(REPORT);
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
      "[quick][hivert]") {
    REPORTER.set_report(REPORT);
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
      "[quick][hivert]") {
    REPORTER.set_report(REPORT);
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
      "[quick][hivert]") {
    REPORTER.set_report(REPORT);
    FpSemigroup S;
    S.set_alphabet(10);
    for (relation_type const& rl : RennerTypeBMonoid(4, 1)) {
      S.add_rule(rl);
    }
    REQUIRE(S.nr_rules() == 115);
    REQUIRE(!S.is_obviously_infinite());
    REQUIRE(!S.knuth_bendix()->confluent());
    // S.knuth_bendix()->run(); // Works too but is slower :)
    REQUIRE(S.size() == 13889);
    REQUIRE(S.isomorphic_non_fp_semigroup()->nr_rules() == 356);
  }

  LIBSEMIGROUPS_TEST_CASE(
      "FpSemigroup",
      "010",
      "Renner monoid type B4 (Gay-Hivert presentation), q = 0",
      "[quick][hivert]") {
    REPORTER.set_report(REPORT);
    FpSemigroup S;
    S.set_alphabet(10);
    for (relation_type const& rl : RennerTypeBMonoid(4, 0)) {
      S.add_rule(rl);
    }
    REQUIRE(S.nr_rules() == 115);
    REQUIRE(!S.is_obviously_infinite());
    REQUIRE(!S.knuth_bendix()->confluent());
    // S.knuth_bendix()->run(); // Works too :)
    REQUIRE(S.size() == 13889);
    REQUIRE(S.isomorphic_non_fp_semigroup()->nr_rules() == 356);
  }

  // This appears to be an example where KB + FP is faster than TC
  LIBSEMIGROUPS_TEST_CASE(
      "FpSemigroup",
      "011",
      "Renner monoid type B5 (Gay-Hivert presentation), q = 1",
      "[extreme][hivert]") {
    REPORTER.set_report(true);
    FpSemigroup S;
    S.set_alphabet(12);
    for (relation_type const& rl : RennerTypeBMonoid(5, 1)) {
      S.add_rule(rl);
    }
    REQUIRE(S.nr_rules() == 165);
    REQUIRE(!S.is_obviously_infinite());
    REQUIRE(!S.knuth_bendix()->confluent());
    // S.todd_coxeter()->run(); // Takes 2m30s or so to run
    REQUIRE(S.size() == 322021);
    REQUIRE(S.isomorphic_non_fp_semigroup()->nr_rules() == 1453);

    congruence::ToddCoxeter tc(
        TWOSIDED,
        S.isomorphic_non_fp_semigroup(),
        congruence::ToddCoxeter::policy::use_cayley_graph);
    REQUIRE(tc.nr_classes() == 322021);  // Works!

    // fpsemigroup::ToddCoxeter tc(S.isomorphic_non_fp_semigroup());
    // REQUIRE(tc.nr_rules() == 1453); FIXME this gives an error, something
    // is wrong, every relation seems to be added twice
    // REQUIRE(tc.size() == 322021); // Runs forever
  }

  LIBSEMIGROUPS_TEST_CASE(
      "FpSemigroup",
      "012",
      "Renner monoid type B5 (Gay-Hivert presentation), q = 0",
      "[extreme][hivert]") {
    REPORTER.set_report(true);
    FpSemigroup S;
    S.set_alphabet(12);
    for (relation_type const& rl : RennerTypeBMonoid(5, 0)) {
      S.add_rule(rl);
    }
    REQUIRE(S.nr_rules() == 165);
    REQUIRE(!S.is_obviously_infinite());
    REQUIRE(!S.knuth_bendix()->confluent());
    // S.todd_coxeter()->run(); // TODO Check if it runs
    REQUIRE(S.size() == 322021);
    REQUIRE(S.isomorphic_non_fp_semigroup()->nr_rules() == 1453);

    congruence::ToddCoxeter tc(
        TWOSIDED,
        S.isomorphic_non_fp_semigroup(),
        congruence::ToddCoxeter::policy::use_cayley_graph);
    REQUIRE(tc.nr_classes() == 322021);  // Works!
  }

  LIBSEMIGROUPS_TEST_CASE("FpSemigroup",
                          "013",
                          "Renner monoid type D2 (E. G. presentation), q = 1",
                          "[quick][hivert]") {
    REPORTER.set_report(REPORT);
    FpSemigroup S;
    S.set_alphabet(7);
    for (relation_type const& rl : EGTypeDMonoid(2, 1)) {
      S.add_rule(rl);
    }
    REQUIRE(S.nr_rules() == 47);
    REQUIRE(!S.is_obviously_infinite());
    REQUIRE(!S.knuth_bendix()->confluent());
    // S.knuth_bendix()->run(); // Works too :)
    REQUIRE(S.size() == 37);
    REQUIRE(S.isomorphic_non_fp_semigroup()->nr_rules() == 54);
  }

  LIBSEMIGROUPS_TEST_CASE("FpSemigroup",
                          "014",
                          "Renner monoid type D2 (E. G. presentation), q = 0",
                          "[quick][hivert]") {
    REPORTER.set_report(REPORT);
    FpSemigroup S;
    S.set_alphabet(7);
    for (relation_type const& rl : EGTypeDMonoid(2, 0)) {
      S.add_rule(rl);
    }
    REQUIRE(S.nr_rules() == 47);
    REQUIRE(!S.is_obviously_infinite());
    REQUIRE(!S.knuth_bendix()->confluent());
    // S.knuth_bendix()->run(); // Works too :)
    REQUIRE(S.size() == 37);
    REQUIRE(S.isomorphic_non_fp_semigroup()->nr_rules() == 54);
  }

  LIBSEMIGROUPS_TEST_CASE("FpSemigroup",
                          "015",
                          "Renner monoid type D3 (E. G. presentation), q = 1",
                          "[quick][hivert]") {
    REPORTER.set_report(REPORT);
    FpSemigroup S;
    S.set_alphabet(9);
    for (relation_type const& rl : EGTypeDMonoid(3, 1)) {
      S.add_rule(rl);
    }
    REQUIRE(S.nr_rules() == 82);
    REQUIRE(!S.is_obviously_infinite());
    REQUIRE(!S.knuth_bendix()->confluent());
    // S.knuth_bendix()->run(); // Works too but is a bit slower :)
    REQUIRE(S.size() == 541);
    REQUIRE(S.isomorphic_non_fp_semigroup()->nr_rules() == 148);
  }

  LIBSEMIGROUPS_TEST_CASE("FpSemigroup",
                          "016",
                          "Renner monoid type D3 (E. G. presentation), q = 0",
                          "[quick][hivert]") {
    REPORTER.set_report(REPORT);
    FpSemigroup S;
    S.set_alphabet(9);
    for (relation_type const& rl : EGTypeDMonoid(3, 0)) {
      S.add_rule(rl);
    }
    REQUIRE(S.nr_rules() == 82);
    REQUIRE(!S.is_obviously_infinite());
    REQUIRE(!S.knuth_bendix()->confluent());
    // S.knuth_bendix()->run(); // Works too but is a bit slower :)
    REQUIRE(S.size() == 541);
    REQUIRE(S.isomorphic_non_fp_semigroup()->nr_rules() == 148);
  }

  LIBSEMIGROUPS_TEST_CASE("FpSemigroup",
                          "017",
                          "Renner monoid type D4 (E. G. presentation), q = 1",
                          "[quick][hivert]") {  // TODO check test category
    REPORTER.set_report(REPORT);
    FpSemigroup S;
    S.set_alphabet(11);
    for (relation_type const& rl : EGTypeDMonoid(4, 1)) {
      S.add_rule(rl);
    }
    REQUIRE(S.nr_rules() == 124);
    REQUIRE(!S.is_obviously_infinite());
    REQUIRE(!S.knuth_bendix()->confluent());

    S.isomorphic_non_fp_semigroup()->enumerate(10626);
    REQUIRE(S.isomorphic_non_fp_semigroup()->current_nr_rules() == 417);
    REQUIRE(S.isomorphic_non_fp_semigroup()->current_size() == 10626);
    // REQUIRE(S.size() == 10625); // Runs forever
  }

  LIBSEMIGROUPS_TEST_CASE("FpSemigroup",
                          "018",
                          "Renner monoid type D4 (E. G. presentation), q = 0",
                          "[quick][hivert]") {  // TODO check test category
    REPORTER.set_report(REPORT);
    FpSemigroup S;
    S.set_alphabet(11);
    for (relation_type const& rl : EGTypeDMonoid(4, 0)) {
      S.add_rule(rl);
    }
    REQUIRE(S.nr_rules() == 124);
    REQUIRE(!S.is_obviously_infinite());
    REQUIRE(!S.knuth_bendix()->confluent());

    S.isomorphic_non_fp_semigroup()->enumerate(10626);
    REQUIRE(S.isomorphic_non_fp_semigroup()->current_nr_rules() == 417);
    REQUIRE(S.isomorphic_non_fp_semigroup()->current_size() == 10626);
    // REQUIRE(S.size() == 10625); // Runs forever
  }

  LIBSEMIGROUPS_TEST_CASE(
      "FpSemigroup",
      "019",
      "Renner monoid type D2 (Gay-Hivert presentation), q = 1",
      "[quick][hivert]") {
    REPORTER.set_report(REPORT);
    FpSemigroup S;
    S.set_alphabet(7);
    for (relation_type const& rl : RennerTypeDMonoid(2, 1)) {
      S.add_rule(rl);
    }
    REQUIRE(S.nr_rules() == 47);
    REQUIRE(!S.is_obviously_infinite());
    REQUIRE(!S.knuth_bendix()->confluent());
    // S.knuth_bendix()->run(); // Works too :)
    REQUIRE(S.size() == 37);
    REQUIRE(S.isomorphic_non_fp_semigroup()->nr_rules() == 54);
  }

  LIBSEMIGROUPS_TEST_CASE(
      "FpSemigroup",
      "020",
      "Renner monoid type D2 (Gay-Hivert presentation), q = 0",
      "[quick][hivert]") {
    REPORTER.set_report(REPORT);
    FpSemigroup S;
    S.set_alphabet(7);
    for (relation_type const& rl : RennerTypeDMonoid(2, 0)) {
      S.add_rule(rl);
    }
    REQUIRE(S.nr_rules() == 47);
    REQUIRE(!S.is_obviously_infinite());
    REQUIRE(!S.knuth_bendix()->confluent());
    // S.knuth_bendix()->run(); // Works too :)
    REQUIRE(S.size() == 37);
    REQUIRE(S.isomorphic_non_fp_semigroup()->nr_rules() == 54);
  }

  LIBSEMIGROUPS_TEST_CASE(
      "FpSemigroup",
      "021",
      "Renner monoid type D3 (Gay-Hivert presentation), q = 1",
      "[quick][hivert]") {
    REPORTER.set_report(REPORT);
    FpSemigroup S;
    S.set_alphabet(9);
    for (relation_type const& rl : RennerTypeDMonoid(3, 1)) {
      S.add_rule(rl);
    }
    REQUIRE(S.nr_rules() == 82);
    REQUIRE(!S.is_obviously_infinite());
    REQUIRE(!S.knuth_bendix()->confluent());
    // S.knuth_bendix()->run(); // Works too but is a bit slower :)
    REQUIRE(S.size() == 541);
    REQUIRE(S.isomorphic_non_fp_semigroup()->nr_rules() == 148);
  }

  LIBSEMIGROUPS_TEST_CASE(
      "FpSemigroup",
      "022",
      "Renner monoid type D3 (Gay-Hivert presentation), q = 0",
      "[quick][hivert]") {
    REPORTER.set_report(REPORT);
    FpSemigroup S;
    S.set_alphabet(9);
    for (relation_type const& rl : RennerTypeDMonoid(3, 0)) {
      S.add_rule(rl);
    }
    REQUIRE(S.nr_rules() == 82);
    REQUIRE(!S.is_obviously_infinite());
    REQUIRE(!S.knuth_bendix()->confluent());
    // S.knuth_bendix()->run(); // Works too but is a bit slower :)
    REQUIRE(S.size() == 541);
    REQUIRE(S.isomorphic_non_fp_semigroup()->nr_rules() == 148);
  }

  LIBSEMIGROUPS_TEST_CASE(
      "FpSemigroup",
      "023",
      "Renner monoid type D4 (Gay-Hivert presentation), q = 1",
      "[quick][hivert]") {
    REPORTER.set_report(REPORT);
    FpSemigroup S;
    S.set_alphabet(11);
    for (relation_type const& rl : RennerTypeDMonoid(4, 1)) {
      S.add_rule(rl);
    }
    REQUIRE(S.nr_rules() == 126);
    REQUIRE(!S.is_obviously_infinite());
    REQUIRE(!S.knuth_bendix()->confluent());

    REQUIRE(S.size() == 10625);
    REQUIRE(S.isomorphic_non_fp_semigroup()->nr_rules() == 419);
  }

  LIBSEMIGROUPS_TEST_CASE(
      "FpSemigroup",
      "024",
      "Renner monoid type D4 (Gay-Hivert presentation), q = 0",
      "[quick][hivert]") {
    REPORTER.set_report(REPORT);
    FpSemigroup S;
    S.set_alphabet(11);
    for (relation_type const& rl : RennerTypeDMonoid(4, 0)) {
      S.add_rule(rl);
    }
    REQUIRE(S.nr_rules() == 126);
    REQUIRE(!S.is_obviously_infinite());
    REQUIRE(!S.knuth_bendix()->confluent());
    REQUIRE(S.size() == 10625);
    REQUIRE(S.isomorphic_non_fp_semigroup()->nr_rules() == 419);
  }

  LIBSEMIGROUPS_TEST_CASE(
      "FpSemigroup",
      "025",
      "Renner monoid type D5 (Gay-Hivert presentation), q = 1",
      "[extreme][hivert]") {  // TODO check the category
    REPORTER.set_report(true);
    FpSemigroup S;
    S.set_alphabet(13);
    for (relation_type const& rl : RennerTypeDMonoid(5, 1)) {
      S.add_rule(rl);
    }
    REQUIRE(S.nr_rules() == 179);
    REQUIRE(!S.is_obviously_infinite());
    REQUIRE(!S.knuth_bendix()->confluent());

    REQUIRE(S.size() == 258661);
    REQUIRE(S.isomorphic_non_fp_semigroup()->nr_rules() == 1279);
  }

  LIBSEMIGROUPS_TEST_CASE(
      "FpSemigroup",
      "026",
      "Renner monoid type D5 (Gay-Hivert presentation), q = 0",
      "[extreme][hivert]") {  // TODO check the category
    REPORTER.set_report(true);
    FpSemigroup S;
    S.set_alphabet(13);
    for (relation_type const& rl : RennerTypeDMonoid(5, 0)) {
      S.add_rule(rl);
    }
    REQUIRE(S.nr_rules() == 179);
    REQUIRE(!S.is_obviously_infinite());
    REQUIRE(!S.knuth_bendix()->confluent());
    REQUIRE(S.size() == 258661);
    REQUIRE(S.isomorphic_non_fp_semigroup()->nr_rules() == 1279);
  }

  // Takes about 4 minutes
  LIBSEMIGROUPS_TEST_CASE(
      "FpSemigroup",
      "027",
      "Renner monoid type D6 (Gay-Hivert presentation), q = 1",
      "[extreme][hivert]") {
    REPORTER.set_report(true);
    FpSemigroup S;
    S.set_alphabet(15);
    for (relation_type const& rl : RennerTypeDMonoid(6, 1)) {
      S.add_rule(rl);
    }
    REQUIRE(S.nr_rules() == 241);
    REQUIRE(!S.is_obviously_infinite());
    REQUIRE(!S.knuth_bendix()->confluent());

    REQUIRE(S.size() == 7464625);
    REQUIRE(S.isomorphic_non_fp_semigroup()->nr_rules() == 1279);
  }

  // Takes about 4 minutes
  LIBSEMIGROUPS_TEST_CASE(
      "FpSemigroup",
      "028",
      "Renner monoid type D6 (Gay-Hivert presentation), q = 0",
      "[extreme][hivert]") {
    REPORTER.set_report(true);
    FpSemigroup S;
    S.set_alphabet(15);
    for (relation_type const& rl : RennerTypeDMonoid(6, 0)) {
      S.add_rule(rl);
    }
    REQUIRE(S.nr_rules() == 241);
    REQUIRE(!S.is_obviously_infinite());
    REQUIRE(!S.knuth_bendix()->confluent());
    S.knuth_bendix()->knuth_bendix_by_overlap_length();
    REQUIRE(S.size() == 7464625);
    REQUIRE(S.isomorphic_non_fp_semigroup()->nr_rules() == 4570);
  }

  LIBSEMIGROUPS_TEST_CASE("FpSemigroup",
                          "029",
                          "Rook monoid R5, q = 0",
                          "[quick]") {
    REPORTER.set_report(REPORT);
    FpSemigroup S;
    S.set_alphabet(6);
    for (relation_type const& rl : RookMonoid(5, 0)) {
      S.add_rule(rl);
    }
    REQUIRE(S.nr_rules() == 33);
    REQUIRE(!S.is_obviously_infinite());
    REQUIRE(!S.knuth_bendix()->confluent());
    REQUIRE(S.size() == 1546);
    REQUIRE(S.isomorphic_non_fp_semigroup()->nr_rules() == 71);
  }

  LIBSEMIGROUPS_TEST_CASE("FpSemigroup",
                          "030",
                          "Rook monoid R5, q = 1",
                          "[quick]") {
    REPORTER.set_report(REPORT);
    FpSemigroup S;
    S.set_alphabet(6);
    for (relation_type const& rl : RookMonoid(5, 1)) {
      S.add_rule(rl);
    }
    REQUIRE(S.nr_rules() == 33);
    REQUIRE(!S.is_obviously_infinite());
    REQUIRE(!S.knuth_bendix()->confluent());
    REQUIRE(S.size() == 1546);
    REQUIRE(S.isomorphic_non_fp_semigroup()->nr_rules() == 71);
  }

  LIBSEMIGROUPS_TEST_CASE("FpSemigroup",
                          "031",
                          "Rook monoid R6, q = 0",
                          "[quick]") {
    REPORTER.set_report(REPORT);
    FpSemigroup S;
    S.set_alphabet(7);
    for (relation_type const& rl : RookMonoid(6, 0)) {
      S.add_rule(rl);
    }
    REQUIRE(S.nr_rules() == 45);
    REQUIRE(!S.is_obviously_infinite());
    REQUIRE(!S.knuth_bendix()->confluent());
    REQUIRE(S.size() == 13327);
    REQUIRE(S.isomorphic_non_fp_semigroup()->nr_rules() == 207);
  }

  LIBSEMIGROUPS_TEST_CASE("FpSemigroup",
                          "032",
                          "Rook monoid R6, q = 1",
                          "[quick]") {
    REPORTER.set_report(REPORT);
    FpSemigroup S;
    S.set_alphabet(7);
    for (relation_type const& rl : RookMonoid(6, 1)) {
      S.add_rule(rl);
    }
    REQUIRE(S.nr_rules() == 45);
    REQUIRE(!S.is_obviously_infinite());
    REQUIRE(!S.knuth_bendix()->confluent());
    REQUIRE(S.size() == 13327);
    REQUIRE(S.isomorphic_non_fp_semigroup()->nr_rules() == 207);
  }

  /*LIBSEMIGROUPS_TEST_CASE("FpSemigroup", "001", "normal_form", "[quick]") {
    REPORTER.set_report(REPORT);
    std::vector<relation_type> rels;
    rels.push_back(relation_type({0, 0, 0}, {0}));  // (a^3, a)
    rels.push_back(relation_type({0}, {1, 1}));     // (a, b^2)

    FpSemigroup S(2, rels);

    REQUIRE(S.size() == 5);

    REQUIRE(S.normal_form({0, 0, 1}) == word_type({0, 0, 1}));
    REQUIRE(S.normal_form({0, 0, 0, 0, 1}) == word_type({0, 0, 1}));
    REQUIRE(S.normal_form({0, 1, 1, 0, 0, 1}) == word_type({0, 0, 1}));
    REQUIRE(S.normal_form({0, 0, 0}) == word_type({0}));
    REQUIRE(S.normal_form({1}) == word_type({1}));
  }

  // Not sure I see the point of the next test
  // LIBSEMIGROUPS_TEST_CASE("FpSemigroup",
  //                         "002",
  //                         "word_to_class_index for fp semigroup",
  //                         "[quick]") {
  //   std::vector<relation_type> rels;
  //   rels.push_back(relation_type({0, 0, 0}, {0}));  // (a^3, a)
  //   rels.push_back(relation_type({0}, {1, 1}));     // (a, b^2)
  //   std::vector<relation_type> extra;

  //   Congruence<> cong1("twosided", 2, rels, extra);

  //   REQUIRE(cong1.word_to_class_index({0, 0, 1}) == 4);
  //   REQUIRE(cong1.word_to_class_index({0, 0, 0, 0, 1}) == 4);
  //   REQUIRE(cong1.word_to_class_index({0, 1, 1, 0, 0, 1}) == 4);
  //   REQUIRE(cong1.word_to_class_index({0, 0, 0}) == 0);
  //   REQUIRE(cong1.word_to_class_index({1}) == 1);

  //   Congruence<> cong2("twosided", 2, rels, extra);

  //   REQUIRE(cong2.word_to_class_index({0, 0, 0, 0}) == 2);
  // }

  LIBSEMIGROUPS_TEST_CASE("FpSemigroup",
                          "003",
                          "for a finite semigroup",
                          "[quick]") {
    REPORTER.set_report(REPORT);

    std::vector<Element*> gens
        = {new Transformation<uint16_t>({1, 3, 4, 2, 3}),
           new Transformation<uint16_t>({3, 2, 1, 3, 3})};
    FroidurePin<> S = FroidurePin<>(gens);
    really_delete_cont(gens);

    REQUIRE(S.size() == 88);
    REQUIRE(S.nr_rules() == 18);
    REQUIRE(S.degree() == 5);

    Element*  t1 = new Transformation<uint16_t>({3, 4, 4, 4, 4});
    Element*  t2 = new Transformation<uint16_t>({3, 1, 3, 3, 3});
    Element*  t3 = new Transformation<uint16_t>({1, 3, 1, 3, 3});
    Element*  t4 = new Transformation<uint16_t>({4, 2, 4, 4, 2});
    word_type w1, w2, w3, w4;
    S.factorisation(w1, S.position(t1));
    S.factorisation(w2, S.position(t2));
    S.factorisation(w3, S.position(t3));
    S.factorisation(w4, S.position(t4));
    t1->really_delete();
    t2->really_delete();
    t3->really_delete();
    t4->really_delete();
    delete t1;
    delete t2;
    delete t3;
    delete t4;

    FpSemigroup T(&S);
    T.add_relation(w1, w2);

    REQUIRE(T.size() == 21);

    REQUIRE(T.equal_to(w3, w4));
    REQUIRE(T.normal_form(w3) == T.normal_form(w4));
  }

  // LIBSEMIGROUPS_TEST_CASE("FpSemigroup",
  //                         "004",
  //                         "finite fp-semigroup, dihedral group of order
  //                         6 ",
  //                           "[quick][kbfp]") {
  //   std::vector<relation_type> rels = {relation_type({0, 0}, {0}),
  //                                   relation_type({0, 1}, {1}),
  //                                   relation_type({1, 0}, {1}),
  //                                   relation_type({0, 2}, {2}),
  //                                   relation_type({2, 0}, {2}),
  //                                   relation_type({0, 3}, {3}),
  //                                   relation_type({3, 0}, {3}),
  //                                   relation_type({0, 4}, {4}),
  //                                   relation_type({4, 0}, {4}),
  //                                   relation_type({1, 2}, {0}),
  //                                   relation_type({2, 1}, {0}),
  //                                   relation_type({3, 4}, {0}),
  //                                   relation_type({4, 3}, {0}),
  //                                   relation_type({2, 2}, {0}),
  //                                   relation_type({1, 4, 2, 3, 3}, {0}),
  //                                   relation_type({4, 4, 4}, {0})};

  //   std::vector<relation_type> extra = {};

  //   Congruence<> cong("twosided", 5, rels, extra);
  //   REPORTER.set_report(REPORT);

  //   REQUIRE(S.size() == 6);
  //   REQUIRE(cong.word_to_class_index({1}) == cong.word_to_class_index({2}));
  // }

  // LIBSEMIGROUPS_TEST_CASE("FpSemigroup", "005", "finite fp-semigroup, size
  // 16",
  //           "[quick][kbfp]") {
  //   std::vector<relation_type> rels = {relation_type({3}, {2}),
  //                                   relation_type({0, 3}, {0, 2}),
  //                                   relation_type({1, 1}, {1}),
  //                                   relation_type({1, 3}, {1, 2}),
  //                                   relation_type({2, 1}, {2}),
  //                                   relation_type({2, 2}, {2}),
  //                                   relation_type({2, 3}, {2}),
  //                                   relation_type({0, 0, 0}, {0}),
  //                                   relation_type({0, 0, 1}, {1}),
  //                                   relation_type({0, 0, 2}, {2}),
  //                                   relation_type({0, 1, 2}, {1, 2}),
  //                                   relation_type({1, 0, 0}, {1}),
  //                                   relation_type({1, 0, 2}, {0, 2}),
  //                                   relation_type({2, 0, 0}, {2}),
  //                                   relation_type({0, 1, 0, 1}, {1, 0, 1}),
  //                                   relation_type({0, 2, 0, 2}, {2, 0, 2}),
  //                                   relation_type({1, 0, 1, 0}, {1, 0, 1}),
  //                                   relation_type({1, 2, 0, 1}, {1, 0, 1}),
  //                                   relation_type({1, 2, 0, 2}, {2, 0, 2}),
  //                                   relation_type({2, 0, 1, 0}, {2, 0, 1}),
  //                                   relation_type({2, 0, 2, 0}, {2, 0, 2})};
  //   std::vector<relation_type> extra = {};

  //   Congruence<> cong("twosided", 4, rels, extra);
  //   REPORTER.set_report(REPORT);

  //   REQUIRE(S.size() == 16);
  //   REQUIRE(cong.word_to_class_index({2}) == cong.word_to_class_index({3}));
  // }

  // LIBSEMIGROUPS_TEST_CASE("FpSemigroup", "006", "finite fp-semigroup, size
  // 16",
  //           "[quick][kbfp]") {
  //   std::vector<relation_type> rels = {relation_type({2}, {1}),
  //                                   relation_type({4}, {3}),
  //                                   relation_type({5}, {0}),
  //                                   relation_type({6}, {3}),
  //                                   relation_type({7}, {1}),
  //                                   relation_type({8}, {3}),
  //                                   relation_type({9}, {3}),
  //                                   relation_type({10}, {0}),
  //                                   relation_type({0, 2}, {0, 1}),
  //                                   relation_type({0, 4}, {0, 3}),
  //                                   relation_type({0, 5}, {0, 0}),
  //                                   relation_type({0, 6}, {0, 3}),
  //                                   relation_type({0, 7}, {0, 1}),
  //                                   relation_type({0, 8}, {0, 3}),
  //                                   relation_type({0, 9}, {0, 3}),
  //                                   relation_type({0, 10}, {0, 0}),
  //                                   relation_type({1, 1}, {1}),
  //                                   relation_type({1, 2}, {1}),
  //                                   relation_type({1, 4}, {1, 3}),
  //                                   relation_type({1, 5}, {1, 0}),
  //                                   relation_type({1, 6}, {1, 3}),
  //                                   relation_type({1, 7}, {1}),
  //                                   relation_type({1, 8}, {1, 3}),
  //                                   relation_type({1, 9}, {1, 3}),
  //                                   relation_type({1, 10}, {1, 0}),
  //                                   relation_type({3, 1}, {3}),
  //                                   relation_type({3, 2}, {3}),
  //                                   relation_type({3, 3}, {3}),
  //                                   relation_type({3, 4}, {3}),
  //                                   relation_type({3, 5}, {3, 0}),
  //                                   relation_type({3, 6}, {3}),
  //                                   relation_type({3, 7}, {3}),
  //                                   relation_type({3, 8}, {3}),
  //                                   relation_type({3, 9}, {3}),
  //                                   relation_type({3, 10}, {3, 0}),
  //                                   relation_type({0, 0, 0}, {0}),
  //                                   relation_type({0, 0, 1}, {1}),
  //                                   relation_type({0, 0, 3}, {3}),
  //                                   relation_type({0, 1, 3}, {1, 3}),
  //                                   relation_type({1, 0, 0}, {1}),
  //                                   relation_type({1, 0, 3}, {0, 3}),
  //                                   relation_type({3, 0, 0}, {3}),
  //                                   relation_type({0, 1, 0, 1}, {1, 0, 1}),
  //                                   relation_type({0, 3, 0, 3}, {3, 0, 3}),
  //                                   relation_type({1, 0, 1, 0}, {1, 0, 1}),
  //                                   relation_type({1, 3, 0, 1}, {1, 0, 1}),
  //                                   relation_type({1, 3, 0, 3}, {3, 0, 3}),
  //                                   relation_type({3, 0, 1, 0}, {3, 0, 1}),
  //                                   relation_type({3, 0, 3, 0}, {3, 0, 3})};
  //   std::vector<relation_type> extra = {};

  //   Congruence<> cong("twosided", 11, rels, extra);
  //   REPORTER.set_report(REPORT);

  //   REQUIRE(S.size() == 16);
  //   REQUIRE(cong.word_to_class_index({0}) == cong.word_to_class_index({5}));
  //   REQUIRE(cong.word_to_class_index({0}) == cong.word_to_class_index({10}));
  //   REQUIRE(cong.word_to_class_index({1}) == cong.word_to_class_index({2}));
  //   REQUIRE(cong.word_to_class_index({1}) == cong.word_to_class_index({7}));
  //   REQUIRE(cong.word_to_class_index({3}) == cong.word_to_class_index({4}));
  //   REQUIRE(cong.word_to_class_index({3}) == cong.word_to_class_index({6}));
  //   REQUIRE(cong.word_to_class_index({3}) == cong.word_to_class_index({8}));
  //   REQUIRE(cong.word_to_class_index({3}) == cong.word_to_class_index({9}));
  // }

  // LIBSEMIGROUPS_TEST_CASE("FpSemigroup", "007", "fp semigroup, size 240",
  //           "[quick]") {
  //   std::vector<relation_type> extra
  //       = {relation_type({0, 0, 0}, {0}),
  //          relation_type({1, 1, 1, 1}, {1}),
  //          relation_type({0, 1, 1, 1, 0}, {0, 0}),
  //          relation_type({1, 0, 0, 1}, {1, 1}),
  //          relation_type({0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0}, {0, 0})};

  //   Congruence<> cong("twosided", 2, std::vector<relation_type>(), extra);
  //   REPORTER.set_report(REPORT);

  //   REQUIRE(S.size() == 240);
  // }

  LIBSEMIGROUPS_TEST_CASE("FpSemigroup", "008", "add_relation", "[quick]") {
    REPORTER.set_report(REPORT);
    FpSemigroup S;
    S.set_alphabet("ab");
    REQUIRE(S.is_obviously_infinite());
    S.add_relation("aaa", "a");
    S.add_relation("a", "bb");
    REQUIRE(!S.is_obviously_infinite());
    REQUIRE(S.size() == 5);

    FroidurePinBase* T = S.isomorphic_non_fp_semigroup();
    REQUIRE(T->size() == 5);
    REQUIRE(T->nr_idempotents() == 1);
  }

  LIBSEMIGROUPS_TEST_CASE("FpSemigroup", "009", "add_relation", "[quick]") {
    REPORTER.set_report(REPORT);
    FpSemigroup S;
    S.set_alphabet("ab");
    REQUIRE(S.is_obviously_infinite());
    S.add_relation("aaa", "a");
    S.add_relation("a", "bb");
    REQUIRE(!S.is_obviously_infinite());
    REQUIRE(S.rws()->isomorphic_non_fp_semigroup()->size() == 5);
    REQUIRE(S.size() == 5);

    FroidurePinBase* T = S.isomorphic_non_fp_semigroup();
    REQUIRE(T->size() == 5);
    REQUIRE(T->nr_idempotents() == 1);
  }

  LIBSEMIGROUPS_TEST_CASE("FpSemigroup", "010", "equal_to", "[quick]") {
    REPORTER.set_report(REPORT);

    FpSemigroup S;
    S.set_alphabet("ab");
    S.add_relation("aa", "a");
    S.add_relation("ab", "a");
    S.add_relation("ba", "a");

    REQUIRE(!S.is_obviously_infinite());
    REQUIRE(S.equal_to("ab", "a"));
    REQUIRE(S.equal_to("ba", "a"));
    REQUIRE(S.equal_to("aa", "a"));
  }*/
}  // namespace libsemigroups

