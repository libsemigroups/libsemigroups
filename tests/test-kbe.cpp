//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019-2025 James D. Mitchell
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

#include <memory>
#include <vector>  // for vector

#include "Catch2-3.8.0/catch_amalgamated.hpp"  // for LIBSEMIGROUPS_TEST_CASE
#include "test-main.hpp"                       // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/froidure-pin.hpp"     // for FroidurePin
#include "libsemigroups/knuth-bendix.hpp"     // for KnuthBendix
#include "libsemigroups/to-froidure-pin.hpp"  // for to<FroidurePin>
#include "libsemigroups/to-presentation.hpp"  // for to<Presentation>
#include "libsemigroups/transf.hpp"           // for Transf<>
#include "libsemigroups/word-range.hpp"       // for literals

#include "libsemigroups/detail/kbe.hpp"  // for KBE

namespace libsemigroups {
  using literals::operator""_w;

  namespace detail {

    LIBSEMIGROUPS_TEST_CASE("KBE", "000", "constructors", "[quick]") {
      auto                  rg = ReportGuard(false);
      FroidurePin<Transf<>> S;
      S.add_generator(make<Transf<>>({1, 0}));
      S.add_generator(make<Transf<>>({0, 0}));

      KnuthBendix kb(congruence_kind::twosided, to<Presentation<word_type>>(S));
      // NOTE: don't do "using KBE = KBE<decltype(kb)>;" because this isn't the
      // same as the next line.
      using KBE = typename decltype(to<FroidurePin>(kb))::element_type;

      KBE ab(kb, 01_w);
      KBE b(ab);
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
      auto        rg = ReportGuard(false);
      FroidurePin S
          = make<FroidurePin>({make<Transf<>>({1, 0}), make<Transf<>>({0, 0})});

      REQUIRE(S.size() == 4);
      REQUIRE(S.degree() == 2);
      REQUIRE(S.number_of_rules() == 4);

      KnuthBendix kb(congruence_kind::twosided, to<Presentation<word_type>>(S));
      using KBE = typename decltype(to<FroidurePin>(kb))::element_type;

      REQUIRE(kb.confluent());

      auto T = to<FroidurePin>(kb);
      REQUIRE(T.size() == 4);

      KBE ab(kb, 01_w);
      KBE b(kb, 1);

      REQUIRE(!(b < ab));
      REQUIRE(b == ab);
      REQUIRE(!(ab < b));
      REQUIRE(!(ab < b));

      KBE aba(kb, 010_w);
      REQUIRE(b < aba);
    }

    LIBSEMIGROUPS_TEST_CASE("KBE", "002", "factorisation", "[quick]") {
      auto                  rg = ReportGuard(false);
      FroidurePin<Transf<>> S
          = make<FroidurePin>({make<Transf<>>({1, 0}), make<Transf<>>({0, 0})});

      KnuthBendix kb(congruence_kind::twosided, to<Presentation<word_type>>(S));
      REQUIRE(kb.confluent());

      auto T = to<FroidurePin>(kb);
      REQUIRE(T.size() == 4);
      REQUIRE(froidure_pin::factorisation(T, 3) == 10_w);
    }

    LIBSEMIGROUPS_TEST_CASE("KBE", "003", "swap", "[quick]") {
      auto                  rg = ReportGuard(false);
      FroidurePin<Transf<>> S
          = make<FroidurePin>({make<Transf<>>({1, 0}), make<Transf<>>({0, 0})});

      KnuthBendix kb(congruence_kind::twosided, to<Presentation<word_type>>(S));
      REQUIRE(kb.confluent());
      using KBE = typename decltype(to<FroidurePin>(kb))::element_type;

      auto x = KBE(kb, 0);
      auto y = KBE(kb, 1);

      REQUIRE(x == KBE(kb, 0));
      REQUIRE(y == KBE(kb, 1));

      x.swap(y);
      REQUIRE(x == KBE(kb, 1));
      REQUIRE(y == KBE(kb, 0));
    }

    LIBSEMIGROUPS_TEST_CASE("KBE", "004", "adapters", "[quick]") {
      auto                  rg = ReportGuard(false);
      FroidurePin<Transf<>> S
          = make<FroidurePin>({make<Transf<>>({1, 0}), make<Transf<>>({0, 0})});

      KnuthBendix kb(congruence_kind::twosided, to<Presentation<word_type>>(S));
      REQUIRE(kb.confluent());

      using KBE = typename decltype(to<FroidurePin>(kb))::element_type;
      auto x    = KBE(kb, 0);
      REQUIRE(Complexity<KBE>()(x) == LIMIT_MAX);
      REQUIRE(EqualTo<KBE>()(x, x));
      REQUIRE(One<KBE>()(x) == KBE());
      auto y(x);
      IncreaseDegree<KBE>()(y, 10);
      REQUIRE(x == y);
    }

    LIBSEMIGROUPS_TEST_CASE("KBE", "005", "conversions", "[quick]") {
      auto                  rg = ReportGuard(false);
      FroidurePin<Transf<>> S
          = make<FroidurePin>({make<Transf<>>({1, 0}), make<Transf<>>({0, 0})});

      KnuthBendix kb(congruence_kind::twosided, to<Presentation<word_type>>(S));
      using KBE = typename decltype(to<FroidurePin>(kb))::element_type;
      REQUIRE(kb.confluent());

      auto x = KBE(kb, 0);
      REQUIRE(x.word() == 0_w);
    }
  }  // namespace detail
}  // namespace libsemigroups
