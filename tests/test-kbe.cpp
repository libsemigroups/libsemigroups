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

#include <vector>  // for vector

#include "element.hpp"              // for Element
#include "froidure-pin.hpp"         // for FroidurePin<>::element_index_type
#include "kbe.hpp"                  // for KBE
#include "knuth-bendix.hpp"         // for KnuthBendix
#include "test-main.hpp"  // for LIBSEMIGROUPS_TEST_CAS

namespace libsemigroups {
  template <class TElementType>
  void delete_gens(std::vector<TElementType>& gens) {
    for (auto& x : gens) {
      delete x;
    }
  }
  using KnuthBendix     = fpsemigroup::KnuthBendix;
  constexpr bool REPORT = false;

  LIBSEMIGROUPS_TEST_CASE("KBE", "001", "test", "[quick]") {
    std::vector<Element*> gens = {new Transformation<uint16_t>({1, 0}),
                                  new Transformation<uint16_t>({0, 0})};
    FroidurePin<>         S    = FroidurePin<>(gens);
    REPORTER.set_report(REPORT);
    delete_gens(gens);

    REQUIRE(S.size() == 4);
    REQUIRE(S.degree() == 2);
    REQUIRE(S.nr_rules() == 4);

    KnuthBendix kb(S);
    REQUIRE(kb.confluent());

    gens            = {new KBE(kb, 0), new KBE(kb, 1)};
    FroidurePin<> T = FroidurePin<>(gens);
    REQUIRE(T.size() == 4);
    delete_gens(gens);

    KBE ab(kb, word_type({0, 1}));
    KBE b(kb, 1);
    REQUIRE(!(b < ab));
    REQUIRE(b == ab);
    REQUIRE(!(ab < b));
    REQUIRE(!(ab < b));

    KBE aba(kb, word_type({0, 1, 0}));
    REQUIRE(b < aba);
  }

  LIBSEMIGROUPS_TEST_CASE("KBE", "002", "factorisation", "[quick]") {
    std::vector<Element*> gens = {new Transformation<uint16_t>({1, 0}),
                                  new Transformation<uint16_t>({0, 0})};
    FroidurePin<>         S    = FroidurePin<>(gens);
    REPORTER.set_report(REPORT);
    delete_gens(gens);

    KnuthBendix kb(S);
    REQUIRE(kb.confluent());

    gens            = {new KBE(kb, 0), new KBE(kb, 1)};
    FroidurePin<> T = FroidurePin<>(gens);
    delete_gens(gens);

    KBE ab(kb, word_type({0, 1}));
    REQUIRE(T.factorisation(&ab) == word_type({1}));

    KBE aaa(kb, word_type({0, 0, 0}));
    REQUIRE(T.factorisation(&aaa) == word_type({0}));
  }

  LIBSEMIGROUPS_TEST_CASE("KBE", "003", "increase_degree_by", "[quick]") {
    std::vector<Element*> gens = {new Transformation<uint16_t>({1, 0}),
                                  new Transformation<uint16_t>({0, 0})};
    FroidurePin<>         S    = FroidurePin<>(gens);
    REPORTER.set_report(REPORT);
    delete_gens(gens);

    KnuthBendix kb(S);
    REQUIRE(kb.confluent());

    auto x = KBE(kb, 0);
    REQUIRE(x == KBE(kb, 0));
    x.increase_degree_by(1000);  // Does nothing
    REQUIRE(x == KBE(kb, 0));
  }
}  // namespace libsemigroups
