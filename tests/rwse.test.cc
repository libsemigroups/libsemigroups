//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2017 James D. Mitchell
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

#include <utility>

#include "../src/cong.h"
#include "../src/rwse.h"
#include "catch.hpp"

#define RWSE_REPORT false

using namespace libsemigroups;

TEST_CASE("RWSE 01:", "[quick][rwse][01]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({1, 0}),
         new Transformation<u_int16_t>(std::vector<u_int16_t>({0, 0}))};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(RWSE_REPORT);
  really_delete_cont(gens);

  REQUIRE(S.size() == 4);
  REQUIRE(S.degree() == 2);
  REQUIRE(S.nrrules() == 4);
  std::vector<relation_t> extra;
  Congruence              cong("twosided", &S, extra);

  RWS rws(cong);
  REQUIRE(rws.confluent());

  gens          = {new RWSE(rws, 0), new RWSE(rws, 1)};
  Semigroup<> T = Semigroup<>(gens);
  really_delete_cont(gens);
  T.set_report(RWSE_REPORT);
  REQUIRE(T.size() == 4);

  RWSE ab(rws, word_t({0, 1}));
  RWSE b(rws, 1);
  REQUIRE(!(b < ab));
  REQUIRE(b == ab);
  REQUIRE(!(ab < b));
  REQUIRE(!(ab < b));
  ab.really_delete();

  RWSE aba(rws, word_t({0, 1, 0}));
  REQUIRE(b < aba);
  aba.really_delete();
  b.really_delete();
}

TEST_CASE("RWSE 02: factorisation", "[quick][rwse][02]") {
  std::vector<Element*> gens
      = {new Transformation<u_int16_t>({1, 0}),
         new Transformation<u_int16_t>(std::vector<u_int16_t>({0, 0}))};
  Semigroup<> S = Semigroup<>(gens);
  S.set_report(RWSE_REPORT);
  really_delete_cont(gens);

  std::vector<relation_t> extra;
  Congruence              cong("twosided", &S, extra);
  RWS                     rws(cong);
  REQUIRE(rws.confluent());

  gens          = {new RWSE(rws, 0), new RWSE(rws, 1)};
  Semigroup<> T = Semigroup<>(gens);
  really_delete_cont(gens);
  T.set_report(RWSE_REPORT);

  RWSE    ab(rws, word_t({0, 1}));
  word_t* w = T.factorisation(&ab);
  REQUIRE(*w == word_t({1}));
  delete w;

  RWSE aaa(rws, word_t({0, 0, 0}));
  w = T.factorisation(&aaa);
  REQUIRE(*w == word_t({0}));
  delete w;

  aaa.copy(&ab);
  REQUIRE(aaa == ab);

  aaa.really_delete();
  ab.really_delete();
}
