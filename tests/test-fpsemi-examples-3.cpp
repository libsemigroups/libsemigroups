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

// This file is the third of three containing tests for the fpsemi-examples
// functions. The tests in this file use Sims1.

// #define CATCH_CONFIG_ENABLE_PAIR_STRINGMAKER

#include "catch.hpp"      // for REQUIRE, REQUIRE_NOTHROW, REQUIRE_THROWS_AS
#include "test-main.hpp"  // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/fpsemi-examples.hpp"  // for the presentations
#include "libsemigroups/report.hpp"           // for ReportGuard
#include "libsemigroups/sims1.hpp"            // for Sims1
#include "libsemigroups/types.hpp"            // for word_type

namespace libsemigroups {

  using Sims1_ = Sims1<uint32_t>;

  using fpsemigroup::author;
  using fpsemigroup::make;

  using fpsemigroup::not_symmetric_group;

  LIBSEMIGROUPS_TEST_CASE(
      "fpsemi-examples",
      "052",
      "not_symmetric_group(5) Guralnick + Kantor + Kassabov + Lubotzky",
      "[fpsemi-examples][quick]") {
    auto   rg = ReportGuard(false);
    size_t n  = 5;

    auto p = make<Presentation<word_type>>(
        not_symmetric_group(n,
                            author::Guralnick + author::Kantor
                                + author::Kassabov + author::Lubotzky));
    p.alphabet(n);
    presentation::replace_word(p, word_type({}), {n - 1});
    presentation::add_identity_rules(p, n - 1);
    p.validate();
    Sims1_ C(congruence_kind::right);
    C.short_rules(p);

    auto q
        = make<Presentation<word_type>>(symmetric_group(n, author::Carmichael));
    q.alphabet(n);
    presentation::replace_word(q, word_type({}), {n - 1});
    presentation::add_identity_rules(q, n - 1);
    q.validate();
    Sims1_ D(congruence_kind::right);
    D.short_rules(q);

    REQUIRE(C.number_of_congruences(3) == 43);
    REQUIRE(D.number_of_congruences(3) == 4);
  }
}  // namespace libsemigroups
