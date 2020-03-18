//
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

#include "bench-main.hpp"  // for LIBSEMIGROUPS_BENCHMARK
#include "catch.hpp"       // for REQUIRE, REQUIRE_NOTHROW, REQUIRE_THROWS_AS
#include "libsemigroups/todd-coxeter.hpp"  // for ToddCoxeter

#include "examples/cong-intf.hpp"
#include "examples/fpsemi-intf.hpp"

namespace libsemigroups {
  namespace {
    using order = congruence::ToddCoxeter::order;
    fpsemigroup::ToddCoxeter* before_normal_forms2(FpSemiIntfArgs const& p) {
      auto tc = make<fpsemigroup::ToddCoxeter>(p);
      tc->run();
      tc->congruence().standardize(order::shortlex);
      return tc;
    }

    congruence::ToddCoxeter* before_normal_forms1(CongIntfArgs const& p) {
      auto tc = make<congruence::ToddCoxeter>(p);
      tc->run();
      tc->standardize(order::shortlex);
      return tc;
    }

    void bench_normal_forms(fpsemigroup::ToddCoxeter* tc, size_t) {
      auto ptr = tc->froidure_pin();
      ptr->run();
    }

    void bench_normal_forms(congruence::ToddCoxeter* tc, size_t) {
      auto ptr = tc->quotient_froidure_pin();
      ptr->run();
    }

    template <typename S>
    void after_normal_forms(S* tc) {
      delete tc;
    }
  }  // namespace

  //     std::vector<FpSemiIntfArgs> subset =
  //    {fpsemigroup::finite_examples(4)};

  LIBSEMIGROUPS_BENCHMARK("Shortlex normal forms ToddCoxeter 1",
                          "[ToddCoxeter][normal_forms_short_lex][quick][001]",
                          before_normal_forms1,
                          bench_normal_forms,
                          after_normal_forms<congruence::ToddCoxeter>,
                          congruence::finite_examples());

  LIBSEMIGROUPS_BENCHMARK("Shortlex normal forms ToddCoxeter 2",
                          "[ToddCoxeter][normal_forms_short_lex][quick][002]",
                          before_normal_forms2,
                          bench_normal_forms,
                          after_normal_forms<fpsemigroup::ToddCoxeter>,
                          fpsemigroup::finite_examples());

}  // namespace libsemigroups
