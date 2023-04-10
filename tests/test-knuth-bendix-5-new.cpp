// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2020 James D. Mitchell
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

// This file is the fifth of six that contains tests for the KnuthBendix
// classes. In a mostly vain attempt to speed up compilation the tests are
// split across 6 files as follows:
//
// 1: contains quick tests for KnuthBendix created from rules and all commented
//    out tests.
//
// 2: contains more quick tests for KnuthBendix created from rules
//
// 3: contains yet more quick tests for KnuthBendix created from rules
//
// 4: contains standard and extreme test for KnuthBendix created from rules
//
// 5: contains tests for KnuthBendix created from FroidurePin instances
//
// 6: contains tests for congruence::KnuthBendix.

// #define CATCH_CONFIG_ENABLE_PAIR_STRINGMAKER

#include <vector>  // for vector

#include "catch.hpp"      // for REQUIRE, REQUIRE_NOTHROW, REQUIRE_THROWS_AS
#include "test-main.hpp"  // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/froidure-pin.hpp"      // for FroidurePin
#include "libsemigroups/kbe-new.hpp"           // for detail::KBE
#include "libsemigroups/knuth-bendix-new.hpp"  // for KnuthBendix, operator<<
#include "libsemigroups/make-froidure-pin.hpp"
#include "libsemigroups/report.hpp"  // for ReportGuard
#include "libsemigroups/transf.hpp"  // for Transf<>
#include "libsemigroups/types.hpp"   // for word_type

namespace libsemigroups {

  using literals::operator""_w;

  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "097",
                          "transformation semigroup (size 4)",
                          "[quick][knuth-bendix]") {
    auto                  rg = ReportGuard(false);
    FroidurePin<Transf<>> S({Transf<>({1, 0}), Transf<>({0, 0})});
    REQUIRE(S.size() == 4);
    REQUIRE(S.number_of_rules() == 4);

    auto p = to_presentation<word_type>(S);

    KnuthBendix kb(p);
    REQUIRE(kb.confluent());
    REQUIRE(kb.presentation().rules.size() / 2 == 4);
    REQUIRE(kb.number_of_active_rules() == 4);
    REQUIRE(kb.size() == 4);
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "098",
                          "transformation semigroup (size 9)",
                          "[quick][knuth-bendix]") {
    auto                  rg = ReportGuard(false);
    FroidurePin<Transf<>> S;
    S.add_generator(Transf<>({1, 3, 4, 2, 3}));
    S.add_generator(Transf<>({0, 0, 0, 0, 0}));

    REQUIRE(S.size() == 9);
    REQUIRE(S.degree() == 5);
    REQUIRE(S.number_of_rules() == 3);

    auto        p = to_presentation<word_type>(S);
    KnuthBendix kb(p);
    REQUIRE(kb.confluent());
    REQUIRE(kb.number_of_active_rules() == 3);
    REQUIRE(kb.size() == 9);
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "099",
                          "transformation semigroup (size 88)",
                          "[quick][knuth-bendix]") {
    auto                  rg = ReportGuard(false);
    FroidurePin<Transf<>> S;
    S.add_generator(Transf<>({1, 3, 4, 2, 3}));
    S.add_generator(Transf<>({3, 2, 1, 3, 3}));

    REQUIRE(S.size() == 88);
    REQUIRE(S.degree() == 5);
    REQUIRE(S.number_of_rules() == 18);

    auto        p = to_presentation<word_type>(S);
    KnuthBendix kb(p);
    REQUIRE(kb.confluent());
    REQUIRE(kb.number_of_active_rules() == 18);
    REQUIRE(kb.size() == 88);
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "100",
                          "internal_string_to_word",
                          "[quick]") {
    auto                  rg = ReportGuard(false);
    FroidurePin<Transf<>> S;
    S.add_generator(Transf<>({1, 0}));
    S.add_generator(Transf<>({0, 0}));

    auto p = to_presentation<word_type>(S);

    KnuthBendix kb(p);
    REQUIRE(kb.confluent());

    auto t = to_froidure_pin(kb);
    REQUIRE(t.generator(0).word(kb) == 0_w);
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "102",
                          "internal_string_to_word",
                          "[quick]") {
    auto                  rg = ReportGuard(false);
    FroidurePin<Transf<>> S(
        {Transf<>({1, 3, 4, 2, 3}), Transf<>({3, 2, 1, 3, 3})});

    REQUIRE(S.size() == 88);

    auto p = to_presentation<word_type>(S);

    KnuthBendix kb(p);
    kb.run();
    REQUIRE(kb.confluent());
    REQUIRE(kb.size() == 88);
  }
}  // namespace libsemigroups
