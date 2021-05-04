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

#include <cstddef>  // for size_t

#include "bench-main.hpp"  // for CATCH_CONFIG_ENABLE_BENCHMARKING
#include "catch.hpp"       // for TEST_CASE, BENCHMARK, REQUIRE

#include "libsemigroups/bitset.hpp"        // for BitSet
#include "libsemigroups/bmat.hpp"          // for BMat adapters
#include "libsemigroups/froidure-pin.hpp"  // for FroidurePin
#include "libsemigroups/matrix.hpp"        // for BMat
#include "libsemigroups/report.hpp"        // for ReportGuard
#include "libsemigroups/transf.hpp"        // for Transformation

namespace libsemigroups {

  ////////////////////////////////////////////////////////////////////////
  // Alternative implementations of Lambda for BMat<>
  ////////////////////////////////////////////////////////////////////////

  template <typename T>
  struct LambdaBMatAlt1 {
    template <typename Mat>
    void operator()(T& res, Mat const& x) const {
      using S        = typename T::value_type;
      size_t const N = S().size();
      if (x.number_of_rows() > N) {
        LIBSEMIGROUPS_EXCEPTION(
            "expected matrix of dimension at most %llu, found %llu",
            N,
            x.number_of_rows());
      }
      T pt;
      for (size_t i = 0; i < x.number_of_rows(); ++i) {
        S cup;
        cup.reset();
        cup.set(i, true);
        pt.push_back(std::move(cup));
      }
      ImageRightAction<Mat, T>()(res, pt, x);
    }

    template <typename Mat>
    T operator()(Mat const& x) const {
      T     res;
      this->operator()(res, x);
      return res;
    }
  };

  template <typename T>
  struct LambdaBMatAlt2 {
    template <typename Mat>
    void operator()(T& res, Mat const& x) const {
      using S        = typename T::value_type;
      size_t const N = S().size();
      if (x.number_of_rows() > N) {
        LIBSEMIGROUPS_EXCEPTION(
            "expected matrix of dimension at most %llu, found %llu",
            N,
            x.number_of_rows());
      }
      static thread_local T pt;
      pt.clear();
      for (size_t i = 0; i < x.number_of_rows(); ++i) {
        S cup;
        cup.reset();
        cup.set(i, true);
        pt.push_back(std::move(cup));
      }
      ImageRightAction<Mat, T>()(res, pt, x);
    }

    template <typename Mat>
    T operator()(Mat const& x) const {
      T     res;
      this->operator()(res, x);
      return res;
    }
  };

  ////////////////////////////////////////////////////////////////////////
  // Alternative implementation of Rank for BMat<>
  ////////////////////////////////////////////////////////////////////////
  template <typename Mat>
  struct Rank<Mat, void> {
    size_t operator()(Mat const& x, size_t = 0) {
      return matrix_helpers::row_space_size(x);
    }
  };

  ////////////////////////////////////////////////////////////////////////
  // Examples providing candidate boolean matrices for the benchmarks
  ////////////////////////////////////////////////////////////////////////

  template <typename T>
  void BMat_example1(T& S) {
    using BMat = typename T::element_type;
    S.add_generator(BMat({{1, 0, 0, 0, 0, 0, 0, 0},
                          {0, 1, 0, 0, 0, 0, 0, 0},
                          {0, 0, 1, 0, 0, 0, 0, 0},
                          {0, 0, 0, 1, 0, 0, 0, 0},
                          {0, 0, 0, 0, 1, 0, 0, 0},
                          {0, 0, 0, 0, 0, 0, 1, 0},
                          {0, 0, 0, 0, 0, 0, 0, 1},
                          {0, 0, 0, 0, 0, 1, 0, 0}}));
    S.add_generator(BMat({{0, 1, 0, 1, 0, 1, 0, 0},
                          {0, 1, 1, 0, 1, 1, 0, 0},
                          {1, 0, 1, 1, 0, 0, 0, 0},
                          {0, 1, 0, 0, 1, 0, 0, 0},
                          {0, 0, 1, 0, 0, 1, 0, 0},
                          {0, 1, 0, 1, 1, 0, 0, 0},
                          {1, 0, 0, 0, 0, 0, 0, 0},
                          {0, 0, 0, 0, 0, 0, 0, 0}}));
    S.add_generator(BMat({{0, 1, 0, 1, 0, 1, 0, 0},
                          {1, 0, 1, 0, 0, 1, 0, 0},
                          {1, 0, 1, 1, 0, 1, 0, 0},
                          {0, 0, 1, 0, 1, 0, 0, 0},
                          {1, 1, 0, 1, 0, 1, 0, 0},
                          {0, 1, 0, 1, 0, 1, 0, 0},
                          {0, 0, 0, 0, 0, 0, 0, 0},
                          {0, 0, 0, 0, 0, 0, 0, 0}}));
  }

  template <typename T>
  void BMat_example2(T& S) {
    using BMat = typename T::element_type;
    S.add_generator(
        BMat({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}));
    S.add_generator(
        BMat({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}));
    S.add_generator(
        BMat({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {1, 0, 0, 1}}));
    S.add_generator(
        BMat({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}}));
  }

  template <typename T>
  void transf_example1(T& S) {
    using Transf = typename T::element_type;
    S.add_generator(Transf({1, 7, 2, 6, 0, 4, 1, 5}));
    S.add_generator(Transf({2, 4, 6, 1, 4, 5, 2, 7}));
    S.add_generator(Transf({3, 0, 7, 2, 4, 6, 2, 4}));
    S.add_generator(Transf({3, 2, 3, 4, 5, 3, 0, 1}));
    S.add_generator(Transf({4, 3, 7, 7, 4, 5, 0, 4}));
    S.add_generator(Transf({5, 6, 3, 0, 3, 0, 5, 1}));
    S.add_generator(Transf({6, 0, 1, 1, 1, 6, 3, 4}));
    S.add_generator(Transf({7, 7, 4, 0, 6, 4, 1, 7}));
  }

  ////////////////////////////////////////////////////////////////////////
  // Benchmark functions
  ////////////////////////////////////////////////////////////////////////

  template <typename Mat, typename T>
  void benchmark_BMat_lambda(FroidurePin<Mat> const& S, std::string title) {
    T result;
    BENCHMARK("using " + title) {
      for (auto it = S.cbegin(); it < S.cend(); ++it) {
        Lambda<Mat, T>()(result, *it);
      }
    };
  }

  template <typename Mat, typename T>
  void benchmark_BMat_lambda_alt1(FroidurePin<Mat> const& S,
                                  std::string             title) {
    T result;
    BENCHMARK("using " + title) {
      for (auto it = S.cbegin(); it < S.cend(); ++it) {
        LambdaBMatAlt1<T>()(result, *it);
      }
    };
  }

  template <typename Mat, typename T>
  void benchmark_BMat_lambda_alt2(FroidurePin<Mat> const& S,
                                  std::string             title) {
    T result;
    BENCHMARK("using " + title) {
      for (auto it = S.cbegin(); it < S.cend(); ++it) {
        LambdaBMatAlt2<T>()(result, *it);
      }
    };
  }

  template <typename Mat, typename T>
  void benchmark_BMat_rho(FroidurePin<Mat> const& S, std::string title) {
    T result;
    BENCHMARK("using " + title) {
      for (auto it = S.cbegin(); it < S.cend(); ++it) {
        Rho<Mat, T>()(result, *it);
      }
    };
  }

  template <typename T>
  void benchmark_transf_rho(FroidurePin<Transf<>> const& S, std::string title) {
    T result;
    BENCHMARK("using " + title) {
      for (auto it = S.cbegin(); it < S.cend(); ++it) {
        Rho<Transf<>, T>()(result, *it);
      }
    };
  }

  template <typename T>
  void benchmark_transf_lambda(FroidurePin<Transf<>> const& S,
                               std::string                  title) {
    T result;
    BENCHMARK("using " + title) {
      for (auto it = S.cbegin(); it < S.cend(); ++it) {
        Lambda<Transf<>, T>()(result, *it);
      }
    };
  }

  ////////////////////////////////////////////////////////////////////////
  // The actual benchmarks for BMats
  ////////////////////////////////////////////////////////////////////////

  TEST_CASE("Example 1: BMat<> + StaticVector1<BitSet<64>, 64>",
            "[quick][000][bmat]") {
    auto                rg = ReportGuard(false);
    FroidurePin<BMat<>> S;
    BMat_example1(S);
    S.run();
    REQUIRE(S.size() == 255);

    benchmark_BMat_lambda<BMat<>, detail::StaticVector1<BitSet<64>, 64>>(
        S, "Lambda<BMat<>> (duplicate code)");

    benchmark_BMat_lambda_alt1<BMat<>, detail::StaticVector1<BitSet<64>, 64>>(
        S, "LambdaBMatAlt1 (using BMat<>, ImageRightAction, non-static dummy)");

    benchmark_BMat_lambda_alt2<BMat<>, detail::StaticVector1<BitSet<64>, 64>>(
        S,
        "LambdaBMatAlt2 (using ImageRightAction, static thread_local "
        "dummy)");
  }

  TEST_CASE("Example 1: BMat<8> + StaticVector1<BitSet<64>, 64>",
            "[quick][016][bmat]") {
    auto                 rg = ReportGuard(false);
    FroidurePin<BMat<8>> S;
    BMat_example1(S);
    S.run();
    REQUIRE(S.size() == 255);

    benchmark_BMat_lambda<BMat<8>, detail::StaticVector1<BitSet<64>, 64>>(
        S, "Lambda<BMat<8>> (duplicate code)");

    benchmark_BMat_lambda_alt1<BMat<8>, detail::StaticVector1<BitSet<64>, 64>>(
        S,
        "LambdaBMatAlt1 (using BMat<8>, ImageRightAction, non-static dummy)");

    benchmark_BMat_lambda_alt2<BMat<8>, detail::StaticVector1<BitSet<64>, 64>>(
        S,
        "LambdaBMatAlt2 (using ImageRightAction, static thread_local "
        "dummy)");
  }

  TEST_CASE("Example 2: BMat<> + StaticVector1<BitSet<64>, 64>",
            "[quick][006][bmat]") {
    auto                rg = ReportGuard(false);
    FroidurePin<BMat<>> S;
    BMat_example2(S);
    S.run();
    REQUIRE(S.size() == 63904);

    benchmark_BMat_lambda<BMat<>, detail::StaticVector1<BitSet<64>, 64>>(
        S, "Lambda<BMat<>> (duplicate code)");

    benchmark_BMat_lambda_alt1<BMat<>, detail::StaticVector1<BitSet<64>, 64>>(
        S, "LambdaBMatAlt1 (using BMat<>, ImageRightAction, non-static dummy)");

    benchmark_BMat_lambda_alt2<BMat<>, detail::StaticVector1<BitSet<64>, 64>>(
        S,
        "LambdaBMatAlt2 (using ImageRightAction, static thread_local "
        "dummy)");
  }

  TEST_CASE("Example 2: BMat<4> + StaticVector1<BitSet<64>, 64>",
            "[quick][017][bmat]") {
    auto                 rg = ReportGuard(false);
    FroidurePin<BMat<4>> S;
    BMat_example2(S);
    S.run();
    REQUIRE(S.size() == 63904);

    benchmark_BMat_lambda<BMat<4>, detail::StaticVector1<BitSet<64>, 64>>(
        S, "Lambda<BMat<4>> (duplicate code)");

    benchmark_BMat_lambda_alt1<BMat<4>, detail::StaticVector1<BitSet<64>, 64>>(
        S,
        "LambdaBMatAlt1 (using BMat<4>, ImageRightAction, non-static dummy)");

    benchmark_BMat_lambda_alt2<BMat<4>, detail::StaticVector1<BitSet<64>, 64>>(
        S,
        "LambdaBMatAlt2 (using ImageRightAction, static thread_local "
        "dummy)");
  }

  TEST_CASE("Example 1: BMat<> + StaticVector1<BitSet<8>, 8>",
            "[quick][001][bmat]") {
    auto                rg = ReportGuard(false);
    FroidurePin<BMat<>> S;
    BMat_example1(S);
    S.run();

    benchmark_BMat_lambda<BMat<>, detail::StaticVector1<BitSet<8>, 8>>(
        S, "Lambda<BMat<>> (duplicate code)");

    benchmark_BMat_lambda_alt1<BMat<>, detail::StaticVector1<BitSet<8>, 8>>(
        S, "LambdaBMatAlt1 (using BMat<>,ImageRightAction, non-static dummy)");

    benchmark_BMat_lambda_alt2<BMat<>, detail::StaticVector1<BitSet<8>, 8>>(
        S,
        "LambdaBMatAlt2 (using ImageRightAction, static thread_local "
        "dummy)");
  }

  TEST_CASE("Example 1: BMat<8> + StaticVector1<BitSet<8>, 8>",
            "[quick][018][bmat]") {
    auto                 rg = ReportGuard(false);
    FroidurePin<BMat<8>> S;
    BMat_example1(S);
    S.run();

    benchmark_BMat_lambda<BMat<8>, detail::StaticVector1<BitSet<8>, 8>>(
        S, "Lambda<BMat<8>> (duplicate code)");

    benchmark_BMat_lambda_alt1<BMat<8>, detail::StaticVector1<BitSet<8>, 8>>(
        S, "LambdaBMatAlt1 (using BMat<8>,ImageRightAction, non-static dummy)");

    benchmark_BMat_lambda_alt2<BMat<8>, detail::StaticVector1<BitSet<8>, 8>>(
        S,
        "LambdaBMatAlt2 (using ImageRightAction, static thread_local "
        "dummy)");
  }

  TEST_CASE("Example 2: BMat<> + StaticVector1<BitSet<8>, 8>",
            "[quick][007][bmat]") {
    auto                rg = ReportGuard(false);
    FroidurePin<BMat<>> S;
    BMat_example2(S);
    S.run();

    benchmark_BMat_lambda<BMat<>, detail::StaticVector1<BitSet<8>, 8>>(
        S, "Lambda<BMat<>> (duplicate code)");

    benchmark_BMat_lambda_alt1<BMat<>, detail::StaticVector1<BitSet<8>, 8>>(
        S, "LambdaBMatAlt1 (using BMat<>, ImageRightAction, non-static dummy)");

    benchmark_BMat_lambda_alt2<BMat<>, detail::StaticVector1<BitSet<8>, 8>>(
        S,
        "LambdaBMatAlt2 (using ImageRightAction, static thread_local "
        "dummy)");
  }

  TEST_CASE("Example 2: BMat<4> + StaticVector1<BitSet<8>, 8>",
            "[quick][019][bmat]") {
    auto                 rg = ReportGuard(false);
    FroidurePin<BMat<4>> S;
    BMat_example2(S);
    S.run();

    benchmark_BMat_lambda<BMat<4>, detail::StaticVector1<BitSet<8>, 8>>(
        S, "Lambda<BMat<4>> (duplicate code)");

    benchmark_BMat_lambda_alt1<BMat<4>, detail::StaticVector1<BitSet<8>, 8>>(
        S,
        "LambdaBMatAlt1 (using BMat<4>, ImageRightAction, non-static dummy)");

    benchmark_BMat_lambda_alt2<BMat<4>, detail::StaticVector1<BitSet<8>, 8>>(
        S,
        "LambdaBMatAlt2 (using ImageRightAction, static thread_local "
        "dummy)");
  }

  TEST_CASE("Example 1: BMat<> + std::vector<BitSet<64>>",
            "[quick][002][bmat]") {
    auto                rg = ReportGuard(false);
    FroidurePin<BMat<>> S;
    BMat_example1(S);
    S.run();

    benchmark_BMat_lambda<BMat<>, std::vector<BitSet<64>>>(
        S, "Lambda<BMat<>> (duplicate code)");

    benchmark_BMat_lambda_alt1<BMat<>, std::vector<BitSet<64>>>(
        S, "LambdaBMatAlt1 (using BMat<>, ImageRightAction, non-static dummy)");

    benchmark_BMat_lambda_alt2<BMat<>, std::vector<BitSet<64>>>(
        S,
        "LambdaBMatAlt2 (using ImageRightAction, static thread_local "
        "dummy)");
  }

  TEST_CASE("Example 1: BMat<8> + std::vector<BitSet<64>>",
            "[quick][020][bmat]") {
    auto                 rg = ReportGuard(false);
    FroidurePin<BMat<8>> S;
    BMat_example1(S);
    S.run();

    benchmark_BMat_lambda<BMat<8>, std::vector<BitSet<64>>>(
        S, "Lambda<BMat<8>> (duplicate code)");

    benchmark_BMat_lambda_alt1<BMat<8>, std::vector<BitSet<64>>>(
        S,
        "LambdaBMatAlt1 (using BMat<8>, ImageRightAction, non-static dummy)");

    benchmark_BMat_lambda_alt2<BMat<8>, std::vector<BitSet<64>>>(
        S,
        "LambdaBMatAlt2 (using ImageRightAction, static thread_local "
        "dummy)");
  }

  TEST_CASE("Example 2: BMat<> + std::vector<BitSet<64>>",
            "[quick][008][bmat]") {
    auto                rg = ReportGuard(false);
    FroidurePin<BMat<>> S;
    BMat_example2(S);
    S.run();

    benchmark_BMat_lambda<BMat<>, std::vector<BitSet<64>>>(
        S, "Lambda<BMat<>> (duplicate code)");

    benchmark_BMat_lambda_alt1<BMat<>, std::vector<BitSet<64>>>(
        S, "LambdaBMatAlt1 (using BMat<>, ImageRightAction, non-static dummy)");

    benchmark_BMat_lambda_alt2<BMat<>, std::vector<BitSet<64>>>(
        S,
        "LambdaBMatAlt2 (using ImageRightAction, static thread_local "
        "dummy)");
  }

  TEST_CASE("Example 2: BMat<4> + std::vector<BitSet<64>>",
            "[quick][021][bmat]") {
    auto                 rg = ReportGuard(false);
    FroidurePin<BMat<4>> S;
    BMat_example2(S);
    S.run();

    benchmark_BMat_lambda<BMat<4>, std::vector<BitSet<64>>>(
        S, "Lambda<BMat<4>> (duplicate code)");

    benchmark_BMat_lambda_alt1<BMat<4>, std::vector<BitSet<64>>>(
        S,
        "LambdaBMatAlt1 (using BMat<4>, ImageRightAction, non-static dummy)");

    benchmark_BMat_lambda_alt2<BMat<4>, std::vector<BitSet<64>>>(
        S,
        "LambdaBMatAlt2 (using ImageRightAction, static thread_local "
        "dummy)");
  }

  TEST_CASE("Example 1: BMat<> + std::vector<BitSet<8>>",
            "[quick][003][bmat]") {
    auto                rg = ReportGuard(false);
    FroidurePin<BMat<>> S;
    BMat_example1(S);
    S.run();

    benchmark_BMat_lambda<BMat<>, std::vector<BitSet<8>>>(
        S, "Lambda<BMat<>> (duplicate code)");

    benchmark_BMat_lambda_alt1<BMat<>, std::vector<BitSet<8>>>(
        S, "LambdaBMatAlt1 (using BMat<>, ImageRightAction, non-static dummy)");

    benchmark_BMat_lambda_alt2<BMat<>, std::vector<BitSet<8>>>(
        S,
        "LambdaBMatAlt2 (using ImageRightAction, static thread_local "
        "dummy)");
  }

  TEST_CASE("Example 1: BMat<8> + std::vector<BitSet<8>>",
            "[quick][022][bmat]") {
    auto                 rg = ReportGuard(false);
    FroidurePin<BMat<8>> S;
    BMat_example1(S);
    S.run();

    benchmark_BMat_lambda<BMat<8>, std::vector<BitSet<8>>>(
        S, "Lambda<BMat<8>> (duplicate code)");

    benchmark_BMat_lambda_alt1<BMat<8>, std::vector<BitSet<8>>>(
        S,
        "LambdaBMatAlt1 (using BMat<8>, ImageRightAction, non-static dummy)");

    benchmark_BMat_lambda_alt2<BMat<8>, std::vector<BitSet<8>>>(
        S,
        "LambdaBMatAlt2 (using ImageRightAction, static thread_local "
        "dummy)");
  }

  TEST_CASE("Example 2: BMat<> + std::vector<BitSet<8>>",
            "[quick][009][bmat]") {
    auto                rg = ReportGuard(false);
    FroidurePin<BMat<>> S;
    BMat_example2(S);
    S.run();

    benchmark_BMat_lambda<BMat<>, std::vector<BitSet<8>>>(
        S, "Lambda<BMat<>> (duplicate code)");

    benchmark_BMat_lambda_alt1<BMat<>, std::vector<BitSet<8>>>(
        S, "LambdaBMatAlt1 (using BMat<>, ImageRightAction, non-static dummy)");

    benchmark_BMat_lambda_alt2<BMat<>, std::vector<BitSet<8>>>(
        S,
        "LambdaBMatAlt2 (using ImageRightAction, static thread_local "
        "dummy)");
  }

  TEST_CASE("Example 2: BMat<4> + std::vector<BitSet<8>>",
            "[quick][023][bmat]") {
    auto                 rg = ReportGuard(false);
    FroidurePin<BMat<4>> S;
    BMat_example2(S);
    S.run();

    benchmark_BMat_lambda<BMat<4>, std::vector<BitSet<8>>>(
        S, "Lambda<BMat<4>> (duplicate code)");

    benchmark_BMat_lambda_alt1<BMat<4>, std::vector<BitSet<8>>>(
        S,
        "LambdaBMatAlt1 (using BMat<4>, ImageRightAction, non-static dummy)");

    benchmark_BMat_lambda_alt2<BMat<4>, std::vector<BitSet<8>>>(
        S,
        "LambdaBMatAlt2 (using ImageRightAction, static thread_local "
        "dummy)");
  }

  TEST_CASE("Example 1: Lambda<BMat<>> std::bitset vs BitSet",
            "[quick][004][bmat]") {
    auto                rg = ReportGuard(false);
    FroidurePin<BMat<>> S;
    BMat_example1(S);
    S.run();
    benchmark_BMat_lambda<BMat<>, detail::StaticVector1<BitSet<64>, 64>>(
        S, "StaticVector1<BitSet<64>, 64>");

    benchmark_BMat_lambda<BMat<>, detail::StaticVector1<std::bitset<64>, 64>>(
        S, "StaticVector1<std::bitset<64>, 64>");

    benchmark_BMat_lambda<BMat<>, detail::StaticVector1<BitSet<8>, 8>>(
        S, "StaticVector1<BitSet<8>, 8>");

    benchmark_BMat_lambda<BMat<>, detail::StaticVector1<std::bitset<8>, 8>>(
        S, "StaticVector1<std::bitset<8>, 8>");

    benchmark_BMat_lambda<BMat<>, std::vector<BitSet<64>>>(
        S, "std::vector<BitSet<64>>");

    benchmark_BMat_lambda<BMat<>, std::vector<std::bitset<64>>>(
        S, "std::vector<std::bitset<64>>");

    benchmark_BMat_lambda<BMat<>, std::vector<BitSet<8>>>(
        S, "std::vector<BitSet<8>>");

    benchmark_BMat_lambda<BMat<>, std::vector<std::bitset<8>>>(
        S, "std::vector<std::bitset<8>>");
  }

  TEST_CASE("Example 1: Lambda<BMat<8>> std::bitset vs BitSet",
            "[quick][024][bmat]") {
    auto                 rg = ReportGuard(false);
    FroidurePin<BMat<8>> S;
    BMat_example1(S);
    S.run();
    benchmark_BMat_lambda<BMat<8>, detail::StaticVector1<BitSet<64>, 64>>(
        S, "StaticVector1<BitSet<64>, 64>");

    benchmark_BMat_lambda<BMat<8>, detail::StaticVector1<std::bitset<64>, 64>>(
        S, "StaticVector1<std::bitset<64>, 64>");

    benchmark_BMat_lambda<BMat<8>, detail::StaticVector1<BitSet<8>, 8>>(
        S, "StaticVector1<BitSet<8>, 8>");

    benchmark_BMat_lambda<BMat<8>, detail::StaticVector1<std::bitset<8>, 8>>(
        S, "StaticVector1<std::bitset<8>, 8>");

    benchmark_BMat_lambda<BMat<8>, std::vector<BitSet<64>>>(
        S, "std::vector<BitSet<64>>");

    benchmark_BMat_lambda<BMat<8>, std::vector<std::bitset<64>>>(
        S, "std::vector<std::bitset<64>>");

    benchmark_BMat_lambda<BMat<8>, std::vector<BitSet<8>>>(
        S, "std::vector<BitSet<8>>");

    benchmark_BMat_lambda<BMat<8>, std::vector<std::bitset<8>>>(
        S, "std::vector<std::bitset<8>>");
  }

  TEST_CASE("Example 2: Lambda<BMat<>> std::bitset vs BitSet",
            "[quick][010][bmat]") {
    auto                rg = ReportGuard(false);
    FroidurePin<BMat<>> S;
    BMat_example2(S);
    S.run();
    benchmark_BMat_lambda<BMat<>, detail::StaticVector1<BitSet<64>, 64>>(
        S, "StaticVector1<BitSet<64>, 64>");

    benchmark_BMat_lambda<BMat<>, detail::StaticVector1<std::bitset<64>, 64>>(
        S, "StaticVector1<std::bitset<64>, 64>");

    benchmark_BMat_lambda<BMat<>, detail::StaticVector1<BitSet<8>, 8>>(
        S, "StaticVector1<BitSet<8>, 8>");

    benchmark_BMat_lambda<BMat<>, detail::StaticVector1<std::bitset<8>, 8>>(
        S, "StaticVector1<std::bitset<8>, 8>");

    benchmark_BMat_lambda<BMat<>, std::vector<BitSet<64>>>(
        S, "std::vector<BitSet<64>>");

    benchmark_BMat_lambda<BMat<>, std::vector<std::bitset<64>>>(
        S, "std::vector<std::bitset<64>>");

    benchmark_BMat_lambda<BMat<>, std::vector<BitSet<8>>>(
        S, "std::vector<BitSet<8>>");

    benchmark_BMat_lambda<BMat<>, std::vector<std::bitset<8>>>(
        S, "std::vector<std::bitset<8>>");

    benchmark_BMat_lambda<BMat<>, std::vector<BitSet<4>>>(
        S, "std::vector<BitSet<4>>");

    benchmark_BMat_lambda<BMat<>, std::vector<std::bitset<4>>>(
        S, "std::vector<std::bitset<4>>");
  }

  TEST_CASE("Example 2: Lambda<BMat<4>> std::bitset vs BitSet",
            "[quick][025][bmat]") {
    auto                 rg = ReportGuard(false);
    FroidurePin<BMat<4>> S;
    BMat_example2(S);
    S.run();
    benchmark_BMat_lambda<BMat<4>, detail::StaticVector1<BitSet<64>, 64>>(
        S, "StaticVector1<BitSet<64>, 64>");

    benchmark_BMat_lambda<BMat<4>, detail::StaticVector1<std::bitset<64>, 64>>(
        S, "StaticVector1<std::bitset<64>, 64>");

    benchmark_BMat_lambda<BMat<4>, detail::StaticVector1<BitSet<8>, 8>>(
        S, "StaticVector1<BitSet<8>, 8>");

    benchmark_BMat_lambda<BMat<4>, detail::StaticVector1<std::bitset<8>, 8>>(
        S, "StaticVector1<std::bitset<8>, 8>");

    benchmark_BMat_lambda<BMat<4>, std::vector<BitSet<64>>>(
        S, "std::vector<BitSet<64>>");

    benchmark_BMat_lambda<BMat<4>, std::vector<std::bitset<64>>>(
        S, "std::vector<std::bitset<64>>");

    benchmark_BMat_lambda<BMat<4>, std::vector<BitSet<8>>>(
        S, "std::vector<BitSet<8>>");

    benchmark_BMat_lambda<BMat<4>, std::vector<std::bitset<8>>>(
        S, "std::vector<std::bitset<8>>");

    benchmark_BMat_lambda<BMat<4>, std::vector<BitSet<4>>>(
        S, "std::vector<BitSet<4>>");

    benchmark_BMat_lambda<BMat<4>, std::vector<std::bitset<4>>>(
        S, "std::vector<std::bitset<4>>");
  }

  TEST_CASE("Example 1: Lambda<BMat<>> std::bitset, BitSet too small",
            "[quick][005][bmat]") {
    auto                rg = ReportGuard(false);
    FroidurePin<BMat<>> S;
    BMat_example1(S);
    S.run();
    benchmark_BMat_lambda<BMat<>, detail::StaticVector1<std::bitset<128>, 8>>(
        S, "StaticVector1<std::bitset<128>, 8>");

    benchmark_BMat_lambda<BMat<>, std::vector<std::bitset<128>>>(
        S, "std::vector<std::bitset<128>>");
  }

  TEST_CASE("Example 1: Lambda<BMat<8>> std::bitset, BitSet too small",
            "[quick][026][bmat]") {
    auto                 rg = ReportGuard(false);
    FroidurePin<BMat<8>> S;
    BMat_example1(S);
    S.run();
    benchmark_BMat_lambda<BMat<8>, detail::StaticVector1<std::bitset<128>, 8>>(
        S, "StaticVector1<std::bitset<128>, 8>");

    benchmark_BMat_lambda<BMat<8>, std::vector<std::bitset<128>>>(
        S, "std::vector<std::bitset<128>>");
  }

  TEST_CASE("Example 1: Rho<BMat<>> vs Lambda<BMat<>>", "[quick][011][bmat]") {
    auto                rg = ReportGuard(false);
    FroidurePin<BMat<>> S;
    BMat_example1(S);
    S.run();

    benchmark_BMat_lambda<BMat<>, detail::StaticVector1<BitSet<64>, 64>>(
        S, "Lambda + StaticVector1<BitSet<64>, 64>");

    benchmark_BMat_rho<BMat<>, detail::StaticVector1<BitSet<64>, 64>>(
        S, "Rho + StaticVector1<BitSet<64>, 64>");

    benchmark_BMat_lambda<BMat<>, detail::StaticVector1<std::bitset<64>, 64>>(
        S, "Lambda + StaticVector1<std::bitset<64>, 64>");

    benchmark_BMat_rho<BMat<>, detail::StaticVector1<std::bitset<64>, 64>>(
        S, "Rho + StaticVector1<std::bitset<64>, 64>");

    benchmark_BMat_lambda<BMat<>, detail::StaticVector1<BitSet<8>, 8>>(
        S, "Lambda + StaticVector1<BitSet<8>, 8>");

    benchmark_BMat_rho<BMat<>, detail::StaticVector1<BitSet<8>, 8>>(
        S, "Rho + StaticVector1<BitSet<8>, 8>");

    benchmark_BMat_lambda<BMat<>, detail::StaticVector1<std::bitset<8>, 8>>(
        S, "Lambda + StaticVector1<std::bitset<8>, 8>");

    benchmark_BMat_rho<BMat<>, detail::StaticVector1<std::bitset<8>, 8>>(
        S, "Rho + StaticVector1<std::bitset<8>, 8>");

    benchmark_BMat_lambda<BMat<>, std::vector<BitSet<64>>>(
        S, "Lambda + std::vector<BitSet<64>>");

    benchmark_BMat_rho<BMat<>, std::vector<BitSet<64>>>(
        S, "Rho + std::vector<BitSet<64>>");

    benchmark_BMat_lambda<BMat<>, std::vector<std::bitset<64>>>(
        S, "Lambda + std::vector<std::bitset<64>>");

    benchmark_BMat_rho<BMat<>, std::vector<std::bitset<64>>>(
        S, "Rho + std::vector<std::bitset<64>>");

    benchmark_BMat_lambda<BMat<>, std::vector<BitSet<8>>>(
        S, "Lambda + std::vector<BitSet<8>>");

    benchmark_BMat_rho<BMat<>, std::vector<BitSet<8>>>(
        S, "Rho + std::vector<BitSet<8>>");

    benchmark_BMat_lambda<BMat<>, std::vector<std::bitset<8>>>(
        S, "Lambda + std::vector<std::bitset<8>>");

    benchmark_BMat_rho<BMat<>, std::vector<std::bitset<8>>>(
        S, "Rho + std::vector<std::bitset<8>>");
  }

  TEST_CASE("Example 1: Rho<BMat<8>> vs Lambda<BMat<8>>",
            "[quick][027][bmat]") {
    auto                 rg = ReportGuard(false);
    FroidurePin<BMat<8>> S;
    BMat_example1(S);
    S.run();

    benchmark_BMat_lambda<BMat<8>, detail::StaticVector1<BitSet<64>, 64>>(
        S, "Lambda + StaticVector1<BitSet<64>, 64>");

    benchmark_BMat_rho<BMat<8>, detail::StaticVector1<BitSet<64>, 64>>(
        S, "Rho + StaticVector1<BitSet<64>, 64>");

    benchmark_BMat_lambda<BMat<8>, detail::StaticVector1<std::bitset<64>, 64>>(
        S, "Lambda + StaticVector1<std::bitset<64>, 64>");

    benchmark_BMat_rho<BMat<8>, detail::StaticVector1<std::bitset<64>, 64>>(
        S, "Rho + StaticVector1<std::bitset<64>, 64>");

    benchmark_BMat_lambda<BMat<8>, detail::StaticVector1<BitSet<8>, 8>>(
        S, "Lambda + StaticVector1<BitSet<8>, 8>");

    benchmark_BMat_rho<BMat<8>, detail::StaticVector1<BitSet<8>, 8>>(
        S, "Rho + StaticVector1<BitSet<8>, 8>");

    benchmark_BMat_lambda<BMat<8>, detail::StaticVector1<std::bitset<8>, 8>>(
        S, "Lambda + StaticVector1<std::bitset<8>, 8>");

    benchmark_BMat_rho<BMat<8>, detail::StaticVector1<std::bitset<8>, 8>>(
        S, "Rho + StaticVector1<std::bitset<8>, 8>");

    benchmark_BMat_lambda<BMat<8>, std::vector<BitSet<64>>>(
        S, "Lambda + std::vector<BitSet<64>>");

    benchmark_BMat_rho<BMat<8>, std::vector<BitSet<64>>>(
        S, "Rho + std::vector<BitSet<64>>");

    benchmark_BMat_lambda<BMat<8>, std::vector<std::bitset<64>>>(
        S, "Lambda + std::vector<std::bitset<64>>");

    benchmark_BMat_rho<BMat<8>, std::vector<std::bitset<64>>>(
        S, "Rho + std::vector<std::bitset<64>>");

    benchmark_BMat_lambda<BMat<8>, std::vector<BitSet<8>>>(
        S, "Lambda + std::vector<BitSet<8>>");

    benchmark_BMat_rho<BMat<8>, std::vector<BitSet<8>>>(
        S, "Rho + std::vector<BitSet<8>>");

    benchmark_BMat_lambda<BMat<8>, std::vector<std::bitset<8>>>(
        S, "Lambda + std::vector<std::bitset<8>>");

    benchmark_BMat_rho<BMat<8>, std::vector<std::bitset<8>>>(
        S, "Rho + std::vector<std::bitset<8>>");
  }

  TEST_CASE("Example 2: Rho<BMat<>> vs Lambda<BMat<>>", "[quick][012][bmat]") {
    auto                rg = ReportGuard(false);
    FroidurePin<BMat<>> S;
    BMat_example2(S);
    S.run();

    benchmark_BMat_lambda<BMat<>, detail::StaticVector1<BitSet<64>, 64>>(
        S, "Lambda + StaticVector1<BitSet<64>, 64>");

    benchmark_BMat_rho<BMat<>, detail::StaticVector1<BitSet<64>, 64>>(
        S, "Rho + StaticVector1<BitSet<64>, 64>");

    benchmark_BMat_lambda<BMat<>, detail::StaticVector1<std::bitset<64>, 64>>(
        S, "Lambda + StaticVector1<std::bitset<64>, 64>");

    benchmark_BMat_rho<BMat<>, detail::StaticVector1<std::bitset<64>, 64>>(
        S, "Rho + StaticVector1<std::bitset<64>, 64>");

    benchmark_BMat_lambda<BMat<>, detail::StaticVector1<BitSet<8>, 8>>(
        S, "Lambda + StaticVector1<BitSet<8>, 8>");

    benchmark_BMat_rho<BMat<>, detail::StaticVector1<BitSet<8>, 8>>(
        S, "Rho + StaticVector1<BitSet<8>, 8>");

    benchmark_BMat_lambda<BMat<>, detail::StaticVector1<std::bitset<8>, 8>>(
        S, "Lambda + StaticVector1<std::bitset<8>, 8>");

    benchmark_BMat_rho<BMat<>, detail::StaticVector1<std::bitset<8>, 8>>(
        S, "Rho + StaticVector1<std::bitset<8>, 8>");

    benchmark_BMat_lambda<BMat<>, std::vector<BitSet<64>>>(
        S, "Lambda + std::vector<BitSet<64>>");

    benchmark_BMat_rho<BMat<>, std::vector<BitSet<64>>>(
        S, "Rho + std::vector<BitSet<64>>");

    benchmark_BMat_lambda<BMat<>, std::vector<std::bitset<64>>>(
        S, "Lambda + std::vector<std::bitset<64>>");

    benchmark_BMat_rho<BMat<>, std::vector<std::bitset<64>>>(
        S, "Rho + std::vector<std::bitset<64>>");

    benchmark_BMat_lambda<BMat<>, std::vector<BitSet<8>>>(
        S, "Lambda + std::vector<BitSet<8>>");

    benchmark_BMat_rho<BMat<>, std::vector<BitSet<8>>>(
        S, "Rho + std::vector<BitSet<8>>");

    benchmark_BMat_lambda<BMat<>, std::vector<std::bitset<8>>>(
        S, "Lambda + std::vector<std::bitset<8>>");

    benchmark_BMat_rho<BMat<>, std::vector<std::bitset<8>>>(
        S, "Rho + std::vector<std::bitset<8>>");

    benchmark_BMat_lambda<BMat<>, std::vector<BitSet<4>>>(
        S, "Lambda + std::vector<BitSet<4>>");

    benchmark_BMat_rho<BMat<>, std::vector<BitSet<4>>>(
        S, "Rho + std::vector<BitSet<4>>");

    benchmark_BMat_lambda<BMat<>, std::vector<std::bitset<4>>>(
        S, "Lambda + std::vector<std::bitset<4>>");

    benchmark_BMat_rho<BMat<>, std::vector<std::bitset<4>>>(
        S, "Rho + std::vector<std::bitset<4>>");
  }

  TEST_CASE("Example 2: Rho<BMat<4>> vs Lambda<BMat<4>>",
            "[quick][028][bmat]") {
    auto                 rg = ReportGuard(false);
    FroidurePin<BMat<4>> S;
    BMat_example2(S);
    S.run();

    benchmark_BMat_lambda<BMat<4>, detail::StaticVector1<BitSet<64>, 64>>(
        S, "Lambda + StaticVector1<BitSet<64>, 64>");

    benchmark_BMat_rho<BMat<4>, detail::StaticVector1<BitSet<64>, 64>>(
        S, "Rho + StaticVector1<BitSet<64>, 64>");

    benchmark_BMat_lambda<BMat<4>, detail::StaticVector1<std::bitset<64>, 64>>(
        S, "Lambda + StaticVector1<std::bitset<64>, 64>");

    benchmark_BMat_rho<BMat<4>, detail::StaticVector1<std::bitset<64>, 64>>(
        S, "Rho + StaticVector1<std::bitset<64>, 64>");

    benchmark_BMat_lambda<BMat<4>, detail::StaticVector1<BitSet<8>, 8>>(
        S, "Lambda + StaticVector1<BitSet<8>, 8>");

    benchmark_BMat_rho<BMat<4>, detail::StaticVector1<BitSet<8>, 8>>(
        S, "Rho + StaticVector1<BitSet<8>, 8>");

    benchmark_BMat_lambda<BMat<4>, detail::StaticVector1<std::bitset<8>, 8>>(
        S, "Lambda + StaticVector1<std::bitset<8>, 8>");

    benchmark_BMat_rho<BMat<4>, detail::StaticVector1<std::bitset<8>, 8>>(
        S, "Rho + StaticVector1<std::bitset<8>, 8>");

    benchmark_BMat_lambda<BMat<4>, std::vector<BitSet<64>>>(
        S, "Lambda + std::vector<BitSet<64>>");

    benchmark_BMat_rho<BMat<4>, std::vector<BitSet<64>>>(
        S, "Rho + std::vector<BitSet<64>>");

    benchmark_BMat_lambda<BMat<4>, std::vector<std::bitset<64>>>(
        S, "Lambda + std::vector<std::bitset<64>>");

    benchmark_BMat_rho<BMat<4>, std::vector<std::bitset<64>>>(
        S, "Rho + std::vector<std::bitset<64>>");

    benchmark_BMat_lambda<BMat<4>, std::vector<BitSet<8>>>(
        S, "Lambda + std::vector<BitSet<8>>");

    benchmark_BMat_rho<BMat<4>, std::vector<BitSet<8>>>(
        S, "Rho + std::vector<BitSet<8>>");

    benchmark_BMat_lambda<BMat<4>, std::vector<std::bitset<8>>>(
        S, "Lambda + std::vector<std::bitset<8>>");

    benchmark_BMat_rho<BMat<4>, std::vector<std::bitset<8>>>(
        S, "Rho + std::vector<std::bitset<8>>");

    benchmark_BMat_lambda<BMat<4>, std::vector<BitSet<4>>>(
        S, "Lambda + std::vector<BitSet<4>>");

    benchmark_BMat_rho<BMat<4>, std::vector<BitSet<4>>>(
        S, "Rho + std::vector<BitSet<4>>");

    benchmark_BMat_lambda<BMat<4>, std::vector<std::bitset<4>>>(
        S, "Lambda + std::vector<std::bitset<4>>");

    benchmark_BMat_rho<BMat<4>, std::vector<std::bitset<4>>>(
        S, "Rho + std::vector<std::bitset<4>>");
  }

  TEST_CASE("Example 4: BMat<> (dim = 8), rank", "[quick][015][bmat]") {
    auto                rg = ReportGuard(false);
    FroidurePin<BMat<>> S;
    BMat_example1(S);
    S.run();
    REQUIRE(S.size() == 255);
    std::vector<BMat<>> gens = {BMat<>({{1, 0, 0, 0, 0, 0, 0, 0},
                                        {0, 1, 0, 0, 0, 0, 0, 0},
                                        {0, 0, 1, 0, 0, 0, 0, 0},
                                        {0, 0, 0, 1, 0, 0, 0, 0},
                                        {0, 0, 0, 0, 1, 0, 0, 0},
                                        {0, 0, 0, 0, 0, 0, 1, 0},
                                        {0, 0, 0, 0, 0, 0, 0, 1},
                                        {0, 0, 0, 0, 0, 1, 0, 0}}),
                                BMat<>({{0, 1, 0, 1, 0, 1, 0, 0},
                                        {0, 1, 1, 0, 1, 1, 0, 0},
                                        {1, 0, 1, 1, 0, 0, 0, 0},
                                        {0, 1, 0, 0, 1, 0, 0, 0},
                                        {0, 0, 1, 0, 0, 1, 0, 0},
                                        {0, 1, 0, 1, 1, 0, 0, 0},
                                        {1, 0, 0, 0, 0, 0, 0, 0},
                                        {0, 0, 0, 0, 0, 0, 0, 0}}),
                                BMat<>({{0, 1, 0, 1, 0, 1, 0, 0},
                                        {1, 0, 1, 0, 0, 1, 0, 0},
                                        {1, 0, 1, 1, 0, 1, 0, 0},
                                        {0, 0, 1, 0, 1, 0, 0, 0},
                                        {1, 1, 0, 1, 0, 1, 0, 0},
                                        {0, 1, 0, 1, 0, 1, 0, 0},
                                        {0, 0, 0, 0, 0, 0, 0, 0},
                                        {0, 0, 0, 0, 0, 0, 0, 0}})};

    BENCHMARK("row space size") {
      for (auto it = S.cbegin(); it < S.cend(); ++it) {
        Rank<BMat<>, void>()(*it);
      }
    };

    BENCHMARK("transformation rank") {
      RankState<BMat<>> st(gens.cbegin(), gens.cend());
      for (auto it = S.cbegin(); it < S.cend(); ++it) {
        Rank<BMat<>>()(st, *it);
      }
    };
  }

  TEST_CASE("Example 4: BMat<8> (dim = 8), rank", "[quick][029][bmat]") {
    auto                 rg = ReportGuard(false);
    FroidurePin<BMat<8>> S;
    BMat_example1(S);
    S.run();
    REQUIRE(S.size() == 255);
    std::vector<BMat<8>> gens = {BMat<8>({{1, 0, 0, 0, 0, 0, 0, 0},
                                          {0, 1, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 1, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 1, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 1, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 1, 0},
                                          {0, 0, 0, 0, 0, 0, 0, 1},
                                          {0, 0, 0, 0, 0, 1, 0, 0}}),
                                 BMat<8>({{0, 1, 0, 1, 0, 1, 0, 0},
                                          {0, 1, 1, 0, 1, 1, 0, 0},
                                          {1, 0, 1, 1, 0, 0, 0, 0},
                                          {0, 1, 0, 0, 1, 0, 0, 0},
                                          {0, 0, 1, 0, 0, 1, 0, 0},
                                          {0, 1, 0, 1, 1, 0, 0, 0},
                                          {1, 0, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 0, 0}}),
                                 BMat<8>({{0, 1, 0, 1, 0, 1, 0, 0},
                                          {1, 0, 1, 0, 0, 1, 0, 0},
                                          {1, 0, 1, 1, 0, 1, 0, 0},
                                          {0, 0, 1, 0, 1, 0, 0, 0},
                                          {1, 1, 0, 1, 0, 1, 0, 0},
                                          {0, 1, 0, 1, 0, 1, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 0, 0}})};

    BENCHMARK("row space size") {
      for (auto it = S.cbegin(); it < S.cend(); ++it) {
        Rank<BMat<8>, void>()(*it);
      }
    };

    BENCHMARK("transformation rank") {
      RankState<BMat<8>> st(gens.cbegin(), gens.cend());
      for (auto it = S.cbegin(); it < S.cend(); ++it) {
        Rank<BMat<8>>()(st, *it);
      }
    };
  }

  ////////////////////////////////////////////////////////////////////////
  // The actual benchmarks for Transformations
  ////////////////////////////////////////////////////////////////////////

  TEST_CASE("Example 3: transformations, rho", "[quick][013][transf]") {
    auto                  rg = ReportGuard(false);
    FroidurePin<Transf<>> S;
    transf_example1(S);
    S.run();
    REQUIRE(S.size() == 597369);

    benchmark_transf_rho<detail::StaticVector1<size_t, 64>>(
        S, "Rho<Transf>, detail::StaticVector1<size_t, 64>");
    benchmark_transf_rho<detail::StaticVector1<size_t, 8>>(
        S, "Rho<Transf>, detail::StaticVector1<size_t, 8>");
    benchmark_transf_rho<std::vector<size_t>>(
        S, "Rho<Transf>, std::vector<size_t>");
  }

  TEST_CASE("Example 3: transformations, lambda", "[quick][014][transf]") {
    auto                  rg = ReportGuard(false);
    FroidurePin<Transf<>> S;
    transf_example1(S);
    S.run();
    REQUIRE(S.size() == 597369);
    benchmark_transf_lambda<detail::StaticVector1<size_t, 64>>(
        S, "Lambda<Transf>, detail::StaticVector1<size_t, 64>");
    benchmark_transf_lambda<detail::StaticVector1<size_t, 8>>(
        S, "Lambda<Transf>, detail::StaticVector1<size_t, 8>");
    benchmark_transf_lambda<std::vector<size_t>>(
        S, "Lambda<Transf>, std::vector<size_t>");
    benchmark_transf_lambda<BitSet<64>>(S, "Lambda<Transf>, BitSet<64>");
  }

}  // namespace libsemigroups
