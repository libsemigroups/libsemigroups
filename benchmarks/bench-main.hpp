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

#ifndef LIBSEMIGROUPS_BENCHMARKS_BENCH_MAIN_HPP_
#define LIBSEMIGROUPS_BENCHMARKS_BENCH_MAIN_HPP_

#include <vector>

#define CATCH_CONFIG_ENABLE_BENCHMARKING

#include "catch.hpp"

// TODO(later) can this be a function rather than a macro??

#define LIBSEMIGROUPS_BENCHMARK(                                              \
    test_case_name, tags, before_func, bench_func, after_func, sample)        \
  TEST_CASE(test_case_name, tags) {                                           \
    auto rg = ReportGuard(false);                                             \
    for (auto var : sample) {                                                 \
      BENCHMARK_ADVANCED(std::string(var.name) + " (" + to_hex_string(var.id) \
                         + ")")                                               \
      (Catch::Benchmark::Chronometer meter) {                                 \
        auto vec = initialised_sample<                                        \
            typename Function<decltype(before_func)>::return_type>(           \
            meter, var, before_func);                                         \
        meter.measure(                                                        \
            [&vec, &var](int i) { return bench_func(vec[i], var.id); });      \
        cleanup_sample(after_func, vec);                                      \
      }; /* NOLINT(readability/braces) */                                     \
    }                                                                         \
  }

namespace libsemigroups {
  std::string to_hex_string(size_t);

  template <typename TReturnType, typename... TArgs>
  struct FunctionBase {
    using return_type = TReturnType;
  };

  template <typename TSignature>
  struct Function {};

  // Free functions . . .
  template <typename TReturnType, typename... TArgs>
  struct Function<TReturnType(TArgs...)> : FunctionBase<TReturnType, TArgs...> {
  };

  // Function pointer
  template <typename TReturnType, typename... TArgs>
  struct Function<TReturnType (*)(TArgs...)>
      : FunctionBase<TReturnType, TArgs...> {};

  // Function pointer const
  template <typename TReturnType, typename... TArgs>
  struct Function<TReturnType (*const)(TArgs...)>
      : FunctionBase<TReturnType, TArgs...> {};

  template <typename T, typename S>
  std::vector<T> initialised_sample(Catch::Benchmark::Chronometer meter,
                                    S const& sample_constructor_params,
                                    T (*before_func)(S const&)) {
    std::vector<T> out;
    for (int i = 0; i < meter.runs(); ++i) {
      out.push_back(before_func(sample_constructor_params));
    }
    return out;
  }

  template <typename S, typename T>
  void cleanup_sample(S after_func, T& data) {
    for (auto& x : data) {
      after_func(x);
    }
  }

}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_BENCHMARKS_BENCH_MAIN_HPP_
