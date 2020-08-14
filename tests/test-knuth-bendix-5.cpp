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
// 1: contains quick tests for fpsemigroup::KnuthBendix created from rules and
//    all commented out tests.
//
// 2: contains more quick tests for fpsemigroup::KnuthBendix created from rules
//
// 3: contains yet more quick tests for fpsemigroup::KnuthBendix created from
//    rules
//
// 4: contains standard and extreme test for fpsemigroup::KnuthBendix created
//    from rules
//
// 5: contains tests for fpsemigroup::KnuthBendix created from FroidurePin
//    instances
//
// 6: contains tests for congruence::KnuthBendix.

// #define CATCH_CONFIG_ENABLE_PAIR_STRINGMAKER

#include <vector>  // for vector

#include "catch.hpp"      // for REQUIRE, REQUIRE_NOTHROW, REQUIRE_THROWS_AS
#include "test-main.hpp"  // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/element-adapters.hpp"  // for Degree
#include "libsemigroups/element.hpp"           // for Element, Transf, Transf...
#include "libsemigroups/froidure-pin.hpp"      // for FroidurePin
#include "libsemigroups/kbe.hpp"               // for detail::KBE
#include "libsemigroups/knuth-bendix.hpp"      // for KnuthBendix, operator<<
#include "libsemigroups/report.hpp"            // for ReportGuard
#include "libsemigroups/types.hpp"             // for word_type

namespace libsemigroups {
  constexpr bool REPORT = false;

  template <typename TElementType>
  void delete_gens(std::vector<TElementType>& gens) {
    for (auto x : gens) {
      delete x;
    }
  }

  namespace fpsemigroup {
    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "095",
                            "(fpsemi) transformation semigroup (size 4)",
                            "[quick][knuth-bendix][fpsemigroup][fpsemi]") {
      auto rg      = ReportGuard(REPORT);
      using Transf = Transformation<uint16_t>;

      FroidurePin<Transf> S({Transf({1, 0}), Transf({0, 0})});
      REQUIRE(S.size() == 4);
      REQUIRE(S.nr_rules() == 4);

      KnuthBendix kb(S);
      REQUIRE(kb.confluent());
      REQUIRE(kb.nr_rules() == 4);
      REQUIRE(kb.nr_active_rules() == 4);
      REQUIRE(kb.size() == 4);
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "096",
                            "(fpsemi) transformation semigroup (size 9)",
                            "[quick][knuth-bendix][fpsemigroup][fpsemi]") {
      auto                  rg = ReportGuard(REPORT);
      std::vector<Element*> gens
          = {new Transformation<uint16_t>({1, 3, 4, 2, 3}),
             new Transformation<uint16_t>({0, 0, 0, 0, 0})};
      FroidurePin<Element const*> S = FroidurePin<Element const*>(gens);
      REQUIRE(S.size() == 9);
      REQUIRE(S.degree() == 5);
      REQUIRE(S.nr_rules() == 3);

      KnuthBendix kb(S);
      REQUIRE(kb.confluent());
      REQUIRE(kb.nr_active_rules() == 3);
      REQUIRE(kb.size() == 9);
      delete_gens(gens);
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "097",
                            "(fpsemi) transformation semigroup (size 88)",
                            "[quick][knuth-bendix][fpsemigroup][fpsemi]") {
      auto                  rg = ReportGuard(REPORT);
      std::vector<Element*> gens
          = {new Transformation<uint16_t>({1, 3, 4, 2, 3}),
             new Transformation<uint16_t>({3, 2, 1, 3, 3})};
      FroidurePin<Element const*> S = FroidurePin<Element const*>(gens);
      REQUIRE(S.size() == 88);
      REQUIRE(S.degree() == 5);
      REQUIRE(S.nr_rules() == 18);

      KnuthBendix kb(S);
      REQUIRE(kb.confluent());
      REQUIRE(kb.nr_active_rules() == 18);
      REQUIRE(kb.size() == 88);
      delete_gens(gens);
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "098",
                            "internal_string_to_word",
                            "[quick]") {
      std::vector<Element*>       gens = {new Transformation<uint16_t>({1, 0}),
                                    new Transformation<uint16_t>({0, 0})};
      FroidurePin<Element const*> S    = FroidurePin<Element const*>(gens);
      auto                        rg   = ReportGuard(REPORT);
      delete_gens(gens);

      KnuthBendix kb(S);
      REQUIRE(kb.confluent());

      auto x = detail::KBE(kb, 0);
      REQUIRE(x.word(kb) == word_type({0}));
    }

    LIBSEMIGROUPS_TEST_CASE(
        "KnuthBendix",
        "099",
        "(fpsemi) construct from shared_ptr<FroidurePin>",
        "[quick][knuth-bendix][fpsemigroup][fpsemi][shortlex]") {
      auto rg      = ReportGuard(REPORT);
      using Transf = Transformation<uint16_t>;
      auto ptr     = std::make_shared<FroidurePin<Transf>>(std::vector<Transf>(
          {Transf({1, 3, 4, 2, 3}), Transf({3, 2, 1, 3, 3})}));

      KnuthBendix kb(ptr);
      kb.run();
      REQUIRE(kb.confluent());
      REQUIRE(kb.size() == 88);
    }

    LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                            "100",
                            "internal_string_to_word",
                            "[quick]") {
      using Transf = Transformation<uint16_t>;
      FroidurePin<Transf> S({Transf({1, 3, 4, 2, 3}), Transf({3, 2, 1, 3, 3})});

      congruence::KnuthBendix kb(S);
      kb.run();
      REQUIRE(kb.knuth_bendix().confluent());
      REQUIRE(kb.nr_classes() == 88);
    }
  }  // namespace fpsemigroup
}  // namespace libsemigroups
