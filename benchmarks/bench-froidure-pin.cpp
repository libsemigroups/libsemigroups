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

#include "libsemigroups/froidure-pin.hpp"

#include "examples/generators.hpp"

namespace libsemigroups {
  template <typename T>
  FroidurePin<T>* before_bench(Generators<T> const& p) {
    auto fp = make<FroidurePin<T>>(p);
    return fp;
  }

  template <typename T>
  void bench_run(FroidurePin<T>* fp, size_t) {
    fp->run();
    REQUIRE(fp->finished());
  }

  template <typename T>
  void after_bench(FroidurePin<T>* fp) {
    delete fp;
  }

  using Transf = typename TransfHelper<16>::type;

  LIBSEMIGROUPS_BENCHMARK("FroidurePin<Transf>",
                          "[FroidurePin][001]",
                          before_bench<Transf>,
                          bench_run<Transf>,
                          after_bench<Transf>,
                          transf_examples());

}  // namespace libsemigroups
