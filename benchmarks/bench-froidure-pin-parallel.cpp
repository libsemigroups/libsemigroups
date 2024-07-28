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

// TODO(later): iwyu

#include <functional>

#include "bench-main.hpp"  // for CATCH_CONFIG_ENABLE_BENCHMARKING
#include "catch.hpp"       // for REQUIRE

#include "libsemigroups/froidure-pin-parallel.hpp"
#include "libsemigroups/transf.hpp"
#include "libsemigroups/types.hpp"

#include "examples/generators.hpp"

namespace libsemigroups {
  // Return function to construct FroidurePinParallel with given number of
  // threads.
  template <typename T, size_t N>
  constexpr FroidurePinParallel<T>* before_bench(Generators<T> const& p) {
    auto fp = new FroidurePinParallel<T>(N);
    for (auto const& x : p.gens) {
      fp->add_generator(x);
    }
    return fp;
  }

  template <typename T>
  void bench_run(FroidurePinParallel<T>* fp, size_t) {
    fp->run();
    REQUIRE(fp->finished());
  }

  template <typename T>
  void after_bench(FroidurePinParallel<T>* fp) {
    delete fp;
  }

  // This is needed as otherwise the template arguments don't parse correctly
  // in macro expansion (for some reason)
  auto before_bench_func_2 = before_bench<LeastTransf<16>, 2>;
  auto before_bench_func_4 = before_bench<LeastTransf<16>, 4>;
  auto before_bench_func_6 = before_bench<LeastTransf<16>, 6>;
  auto before_bench_func_8 = before_bench<LeastTransf<16>, 8>;

  LIBSEMIGROUPS_BENCHMARK("FroidurePinParallel<LeastTransf<16>>-2 threads",
                          "[FroidurePinParallel][001]",
                          before_bench_func_2,
                          bench_run<LeastTransf<16>>,
                          after_bench<LeastTransf<16>>,
                          transf_examples());

  LIBSEMIGROUPS_BENCHMARK("FroidurePinParallel<LeastTransf<16>>-4 threads",
                          "[FroidurePinParallel][002]",
                          before_bench_func_4,
                          bench_run<LeastTransf<16>>,
                          after_bench<LeastTransf<16>>,
                          transf_examples());

  LIBSEMIGROUPS_BENCHMARK("FroidurePinParallel<LeastTransf<16>>-6 threads",
                          "[FroidurePinParallel][003]",
                          before_bench_func_6,
                          bench_run<LeastTransf<16>>,
                          after_bench<LeastTransf<16>>,
                          transf_examples());

  LIBSEMIGROUPS_BENCHMARK("FroidurePinParallel<LeastTransf<16>>-8 threads",
                          "[FroidurePinParallel][004]",
                          before_bench_func_8,
                          bench_run<LeastTransf<16>>,
                          after_bench<LeastTransf<16>>,
                          transf_examples());
}  // namespace libsemigroups
