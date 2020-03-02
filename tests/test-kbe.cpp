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

#include <vector>  // for vector

#include "catch.hpp"                       // for LIBSEMIGROUPS_TEST_CASE
#include "libsemigroups/element.hpp"       // for Element
#include "libsemigroups/froidure-pin.hpp"  // for FroidurePin
#include "libsemigroups/kbe.hpp"           // for KBE
#include "libsemigroups/knuth-bendix.hpp"  // for KnuthBendix
#include "test-main.hpp"                   // for LIBSEMIGROUPS_TEST_CASE

namespace libsemigroups {
  namespace detail {
    template <typename TElementType>
    void delete_gens(std::vector<TElementType>& gens) {
      for (auto& x : gens) {
        delete x;
      }
    }

    using KnuthBendix = fpsemigroup::KnuthBendix;
    using FroidurePinKBE
        = FroidurePin<KBE, FroidurePinTraits<KBE, KnuthBendix>>;
    constexpr bool REPORT = false;

    LIBSEMIGROUPS_TEST_CASE("KBE", "000", "constructors", "[quick]") {
      std::vector<Element*>       gens = {new Transformation<uint16_t>({1, 0}),
                                    new Transformation<uint16_t>({0, 0})};
      FroidurePin<Element const*> S    = FroidurePin<Element const*>(gens);
      auto                        rg   = ReportGuard(REPORT);
      delete_gens(gens);

      KnuthBendix kb(S);
      KBE         ab(kb, word_type({0, 1}));
      KBE         b(ab);
      REQUIRE(b == ab);
      b = ab;
      REQUIRE(b == ab);
      KBE c(std::move(ab));
      REQUIRE(c == b);
      KBE d;
      d = std::move(c);
      REQUIRE(d == b);
    }

    LIBSEMIGROUPS_TEST_CASE("KBE", "001", "test", "[quick]") {
      auto                                  rg = ReportGuard(REPORT);
      FroidurePin<Transformation<uint16_t>> S(
          {Transformation<uint16_t>({1, 0}), Transformation<uint16_t>({0, 0})});

      REQUIRE(S.size() == 4);
      REQUIRE(S.degree() == 2);
      REQUIRE(S.nr_rules() == 4);

      KnuthBendix kb(S);
      REQUIRE(kb.confluent());

      FroidurePinKBE T(kb);
      T.add_generator(KBE(kb, 0));
      T.add_generator(KBE(kb, 1));

      REQUIRE(T.size() == 4);

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
      auto                                  rg = ReportGuard(REPORT);
      FroidurePin<Transformation<uint16_t>> S(
          {Transformation<uint16_t>({1, 0}), Transformation<uint16_t>({0, 0})});

      KnuthBendix kb(S);
      REQUIRE(kb.confluent());
      FroidurePinKBE T(kb);
      T.add_generators({KBE(kb, 0), KBE(kb, 1)});

      KBE ab(kb, word_type({0, 1}));
      REQUIRE(T.factorisation(ab) == word_type({1}));
      KBE aaa(kb, word_type({0, 0, 0}));
      REQUIRE(T.factorisation(aaa) == word_type({0}));
    }

    LIBSEMIGROUPS_TEST_CASE("KBE", "003", "swap", "[quick]") {
      auto                                  rg = ReportGuard(REPORT);
      FroidurePin<Transformation<uint16_t>> S(
          {Transformation<uint16_t>({1, 0}), Transformation<uint16_t>({0, 0})});

      KnuthBendix kb(S);
      REQUIRE(kb.confluent());

      auto x = KBE(kb, 0);
      auto y = KBE(kb, 1);

      REQUIRE(x == KBE(kb, 0));
      REQUIRE(y == KBE(kb, 1));

      x.swap(y);
      REQUIRE(x == KBE(kb, 1));
      REQUIRE(y == KBE(kb, 0));
    }

    LIBSEMIGROUPS_TEST_CASE("KBE", "004", "adapters", "[quick]") {
      auto                                  rg = ReportGuard(REPORT);
      FroidurePin<Transformation<uint16_t>> S(
          {Transformation<uint16_t>({1, 0}), Transformation<uint16_t>({0, 0})});

      KnuthBendix kb(S);
      REQUIRE(kb.confluent());

      auto x = KBE(kb, 0);
      REQUIRE(Complexity<KBE>()(x) == LIMIT_MAX);
      REQUIRE(EqualTo<KBE>()(x, x));
      REQUIRE(One<KBE>()(x) == KBE());
    }

    LIBSEMIGROUPS_TEST_CASE("KBE", "005", "conversions", "[quick]") {
      auto                                  rg = ReportGuard(REPORT);
      FroidurePin<Transformation<uint16_t>> S(
          {Transformation<uint16_t>({1, 0}), Transformation<uint16_t>({0, 0})});

      KnuthBendix kb(S);
      REQUIRE(kb.confluent());

      auto x = KBE(kb, 0);
      REQUIRE(x.word(kb) == word_type({0}));
      REQUIRE(x.string(kb) == std::string(1, kb.alphabet()[0]));
    }
  }  // namespace detail
}  // namespace libsemigroups
