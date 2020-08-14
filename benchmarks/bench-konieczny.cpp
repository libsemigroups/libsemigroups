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

#include "libsemigroups/bitset.hpp"            // for BitSet
#include "libsemigroups/element-adapters.hpp"  // for Lambda
#include "libsemigroups/element-helper.hpp"    // for PPermHelper
#include "libsemigroups/element.hpp"           // for PartialPerm
#include "libsemigroups/froidure-pin.hpp"      // for FroidurePin
#include "libsemigroups/report.hpp"            // for ReportGuard

namespace libsemigroups {
  ////////////////////////////////////////////////////////////////////////
  // Alternative implementations of Lambda for BooleanMat
  ////////////////////////////////////////////////////////////////////////
  template <typename T>
  struct LambdaBooleanMatAlt1 {
    void operator()(T& res, BooleanMat const& x) const {
      using S        = typename T::value_type;
      size_t const N = S().size();
      if (x.degree() > N) {
        LIBSEMIGROUPS_EXCEPTION(
            "expected matrix of dimension at most %llu, found %llu",
            N,
            x.degree());
      }
      T pt;
      for (size_t i = 0; i < x.degree(); ++i) {
        S cup;
        cup.reset();
        cup.set(i, true);
        pt.push_back(std::move(cup));
      }
      ImageRightAction<BooleanMat, T>()(res, pt, x);
    }

    T operator()(BooleanMat const& x) const {
      T     res;
      this->operator()(res, x);
      return res;
    }
  };

  template <typename T>
  struct LambdaBooleanMatAlt2 {
    void operator()(T& res, BooleanMat const& x) const {
      using S        = typename T::value_type;
      size_t const N = S().size();
      if (x.degree() > N) {
        LIBSEMIGROUPS_EXCEPTION(
            "expected matrix of dimension at most %llu, found %llu",
            N,
            x.degree());
      }
      static thread_local T pt;
      pt.clear();
      for (size_t i = 0; i < x.degree(); ++i) {
        S cup;
        cup.reset();
        cup.set(i, true);
        pt.push_back(std::move(cup));
      }
      ImageRightAction<BooleanMat, T>()(res, pt, x);
    }

    T operator()(BooleanMat const& x) const {
      T     res;
      this->operator()(res, x);
      return res;
    }
  };

  ////////////////////////////////////////////////////////////////////////
  // Alternative implementation of Rank for BooleanMat
  ////////////////////////////////////////////////////////////////////////
  template <>
  struct Rank<BooleanMat, void> {
    size_t operator()(BooleanMat const& x, size_t = 0) {
      return x.row_space_size();
    }
  };

  ////////////////////////////////////////////////////////////////////////
  // Examples providing candidate boolean matrices for the benchmarks
  ////////////////////////////////////////////////////////////////////////

  template <typename T>
  void booleanmat_example1(T& S) {
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
  void booleanmat_example2(T& S) {
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

  template <typename T>
  void benchmark_booleanmat_lambda(FroidurePin<BooleanMat> const& S,
                                   std::string                    title) {
    T result;
    BENCHMARK("using " + title) {
      for (auto it = S.cbegin(); it < S.cend(); ++it) {
        Lambda<BooleanMat, T>()(result, *it);
      }
    };
  }

  template <typename T>
  void benchmark_booleanmat_rho(FroidurePin<BooleanMat> const& S,
                                std::string                    title) {
    T result;
    BENCHMARK("using " + title) {
      for (auto it = S.cbegin(); it < S.cend(); ++it) {
        Rho<BooleanMat, T>()(result, *it);
      }
    };
  }

  template <typename T>
  void benchmark_booleanmat_lambda_alt1(FroidurePin<BooleanMat> const& S,
                                        std::string                    title) {
    T result;
    BENCHMARK("using " + title) {
      for (auto it = S.cbegin(); it < S.cend(); ++it) {
        LambdaBooleanMatAlt1<T>()(result, *it);
      }
    };
  }

  template <typename T>
  void benchmark_booleanmat_lambda_alt2(FroidurePin<BooleanMat> const& S,
                                        std::string                    title) {
    T result;
    BENCHMARK("using " + title) {
      for (auto it = S.cbegin(); it < S.cend(); ++it) {
        LambdaBooleanMatAlt2<T>()(result, *it);
      }
    };
  }

  template <typename T>
  void benchmark_transf_rho(FroidurePin<Transformation<size_t>> const& S,
                            std::string                                title) {
    T result;
    BENCHMARK("using " + title) {
      for (auto it = S.cbegin(); it < S.cend(); ++it) {
        Rho<Transformation<size_t>, T>()(result, *it);
      }
    };
  }

  template <typename T>
  void benchmark_transf_lambda(FroidurePin<Transformation<size_t>> const& S,
                               std::string title) {
    T result;
    BENCHMARK("using " + title) {
      for (auto it = S.cbegin(); it < S.cend(); ++it) {
        Lambda<Transformation<size_t>, T>()(result, *it);
      }
    };
  }

  ////////////////////////////////////////////////////////////////////////
  // The actual benchmarks
  ////////////////////////////////////////////////////////////////////////

  TEST_CASE("Example 1: StaticVector1<BitSet<64>, 64>", "[quick][000]") {
    auto                    rg = ReportGuard(false);
    FroidurePin<BooleanMat> S;
    booleanmat_example1(S);
    S.run();
    REQUIRE(S.size() == 255);

    benchmark_booleanmat_lambda<detail::StaticVector1<BitSet<64>, 64>>(
        S, "Lambda<BooleanMat> (duplicate code)");

    benchmark_booleanmat_lambda_alt1<detail::StaticVector1<BitSet<64>, 64>>(
        S, "LambdaBooleanMatAlt1 (using ImageRightAction, non-static dummy)");

    benchmark_booleanmat_lambda_alt2<detail::StaticVector1<BitSet<64>, 64>>(
        S,
        "LambdaBooleanMatAlt2 (using ImageRightAction, static thread_local "
        "dummy)");
  }

  TEST_CASE("Example 2: StaticVector1<BitSet<64>, 64>", "[quick][006]") {
    auto                    rg = ReportGuard(false);
    FroidurePin<BooleanMat> S;
    booleanmat_example2(S);
    S.run();
    REQUIRE(S.size() == 63904);

    benchmark_booleanmat_lambda<detail::StaticVector1<BitSet<64>, 64>>(
        S, "Lambda<BooleanMat> (duplicate code)");

    benchmark_booleanmat_lambda_alt1<detail::StaticVector1<BitSet<64>, 64>>(
        S, "LambdaBooleanMatAlt1 (using ImageRightAction, non-static dummy)");

    benchmark_booleanmat_lambda_alt2<detail::StaticVector1<BitSet<64>, 64>>(
        S,
        "LambdaBooleanMatAlt2 (using ImageRightAction, static thread_local "
        "dummy)");
  }

  TEST_CASE("Example 1: StaticVector1<BitSet<8>, 8>", "[quick][001]") {
    auto                    rg = ReportGuard(false);
    FroidurePin<BooleanMat> S;
    booleanmat_example1(S);
    S.run();

    benchmark_booleanmat_lambda<detail::StaticVector1<BitSet<8>, 8>>(
        S, "Lambda<BooleanMat> (duplicate code)");

    benchmark_booleanmat_lambda_alt1<detail::StaticVector1<BitSet<8>, 8>>(
        S, "LambdaBooleanMatAlt1 (using ImageRightAction, non-static dummy)");

    benchmark_booleanmat_lambda_alt2<detail::StaticVector1<BitSet<8>, 8>>(
        S,
        "LambdaBooleanMatAlt2 (using ImageRightAction, static thread_local "
        "dummy)");
  }

  TEST_CASE("Example 2: StaticVector1<BitSet<8>, 8>", "[quick][007]") {
    auto                    rg = ReportGuard(false);
    FroidurePin<BooleanMat> S;
    booleanmat_example2(S);
    S.run();

    benchmark_booleanmat_lambda<detail::StaticVector1<BitSet<8>, 8>>(
        S, "Lambda<BooleanMat> (duplicate code)");

    benchmark_booleanmat_lambda_alt1<detail::StaticVector1<BitSet<8>, 8>>(
        S, "LambdaBooleanMatAlt1 (using ImageRightAction, non-static dummy)");

    benchmark_booleanmat_lambda_alt2<detail::StaticVector1<BitSet<8>, 8>>(
        S,
        "LambdaBooleanMatAlt2 (using ImageRightAction, static thread_local "
        "dummy)");
  }

  TEST_CASE("Example 1: std::vector<BitSet<64>>", "[quick][002]") {
    auto                    rg = ReportGuard(false);
    FroidurePin<BooleanMat> S;
    booleanmat_example1(S);
    S.run();

    benchmark_booleanmat_lambda<std::vector<BitSet<64>>>(
        S, "Lambda<BooleanMat> (duplicate code)");

    benchmark_booleanmat_lambda_alt1<std::vector<BitSet<64>>>(
        S, "LambdaBooleanMatAlt1 (using ImageRightAction, non-static dummy)");

    benchmark_booleanmat_lambda_alt2<std::vector<BitSet<64>>>(
        S,
        "LambdaBooleanMatAlt2 (using ImageRightAction, static thread_local "
        "dummy)");
  }

  TEST_CASE("Example 2: std::vector<BitSet<64>>", "[quick][008]") {
    auto                    rg = ReportGuard(false);
    FroidurePin<BooleanMat> S;
    booleanmat_example2(S);
    S.run();

    benchmark_booleanmat_lambda<std::vector<BitSet<64>>>(
        S, "Lambda<BooleanMat> (duplicate code)");

    benchmark_booleanmat_lambda_alt1<std::vector<BitSet<64>>>(
        S, "LambdaBooleanMatAlt1 (using ImageRightAction, non-static dummy)");

    benchmark_booleanmat_lambda_alt2<std::vector<BitSet<64>>>(
        S,
        "LambdaBooleanMatAlt2 (using ImageRightAction, static thread_local "
        "dummy)");
  }

  TEST_CASE("Example 1: std::vector<BitSet<8>>", "[quick][003]") {
    auto                    rg = ReportGuard(false);
    FroidurePin<BooleanMat> S;
    booleanmat_example1(S);
    S.run();

    benchmark_booleanmat_lambda<std::vector<BitSet<8>>>(
        S, "Lambda<BooleanMat> (duplicate code)");

    benchmark_booleanmat_lambda_alt1<std::vector<BitSet<8>>>(
        S, "LambdaBooleanMatAlt1 (using ImageRightAction, non-static dummy)");

    benchmark_booleanmat_lambda_alt2<std::vector<BitSet<8>>>(
        S,
        "LambdaBooleanMatAlt2 (using ImageRightAction, static thread_local "
        "dummy)");
  }

  TEST_CASE("Example 2: std::vector<BitSet<8>>", "[quick][009]") {
    auto                    rg = ReportGuard(false);
    FroidurePin<BooleanMat> S;
    booleanmat_example2(S);
    S.run();

    benchmark_booleanmat_lambda<std::vector<BitSet<8>>>(
        S, "Lambda<BooleanMat> (duplicate code)");

    benchmark_booleanmat_lambda_alt1<std::vector<BitSet<8>>>(
        S, "LambdaBooleanMatAlt1 (using ImageRightAction, non-static dummy)");

    benchmark_booleanmat_lambda_alt2<std::vector<BitSet<8>>>(
        S,
        "LambdaBooleanMatAlt2 (using ImageRightAction, static thread_local "
        "dummy)");
  }

  TEST_CASE("Example 1: Lambda<BooleanMat> std::bitset vs BitSet",
            "[quick][004]") {
    auto                    rg = ReportGuard(false);
    FroidurePin<BooleanMat> S;
    booleanmat_example1(S);
    S.run();
    benchmark_booleanmat_lambda<detail::StaticVector1<BitSet<64>, 64>>(
        S, "StaticVector1<BitSet<64>, 64>");

    benchmark_booleanmat_lambda<detail::StaticVector1<std::bitset<64>, 64>>(
        S, "StaticVector1<std::bitset<64>, 64>");

    benchmark_booleanmat_lambda<detail::StaticVector1<BitSet<8>, 8>>(
        S, "StaticVector1<BitSet<8>, 8>");

    benchmark_booleanmat_lambda<detail::StaticVector1<std::bitset<8>, 8>>(
        S, "StaticVector1<std::bitset<8>, 8>");

    benchmark_booleanmat_lambda<std::vector<BitSet<64>>>(
        S, "std::vector<BitSet<64>>");

    benchmark_booleanmat_lambda<std::vector<std::bitset<64>>>(
        S, "std::vector<std::bitset<64>>");

    benchmark_booleanmat_lambda<std::vector<BitSet<8>>>(
        S, "std::vector<BitSet<8>>");

    benchmark_booleanmat_lambda<std::vector<std::bitset<8>>>(
        S, "std::vector<std::bitset<8>>");
  }

  TEST_CASE("Example 2: Lambda<BooleanMat> std::bitset vs BitSet",
            "[quick][010]") {
    auto                    rg = ReportGuard(false);
    FroidurePin<BooleanMat> S;
    booleanmat_example2(S);
    S.run();
    benchmark_booleanmat_lambda<detail::StaticVector1<BitSet<64>, 64>>(
        S, "StaticVector1<BitSet<64>, 64>");

    benchmark_booleanmat_lambda<detail::StaticVector1<std::bitset<64>, 64>>(
        S, "StaticVector1<std::bitset<64>, 64>");

    benchmark_booleanmat_lambda<detail::StaticVector1<BitSet<8>, 8>>(
        S, "StaticVector1<BitSet<8>, 8>");

    benchmark_booleanmat_lambda<detail::StaticVector1<std::bitset<8>, 8>>(
        S, "StaticVector1<std::bitset<8>, 8>");

    benchmark_booleanmat_lambda<std::vector<BitSet<64>>>(
        S, "std::vector<BitSet<64>>");

    benchmark_booleanmat_lambda<std::vector<std::bitset<64>>>(
        S, "std::vector<std::bitset<64>>");

    benchmark_booleanmat_lambda<std::vector<BitSet<8>>>(
        S, "std::vector<BitSet<8>>");

    benchmark_booleanmat_lambda<std::vector<std::bitset<8>>>(
        S, "std::vector<std::bitset<8>>");

    benchmark_booleanmat_lambda<std::vector<BitSet<4>>>(
        S, "std::vector<BitSet<4>>");

    benchmark_booleanmat_lambda<std::vector<std::bitset<4>>>(
        S, "std::vector<std::bitset<4>>");
  }

  TEST_CASE("Example 1: Lambda<BooleanMat> std::bitset, BitSet too small",
            "[quick][005]") {
    auto                    rg = ReportGuard(false);
    FroidurePin<BooleanMat> S;
    booleanmat_example1(S);
    S.run();
    benchmark_booleanmat_lambda<detail::StaticVector1<std::bitset<128>, 8>>(
        S, "StaticVector1<std::bitset<128>, 8>");

    benchmark_booleanmat_lambda<std::vector<std::bitset<128>>>(
        S, "std::vector<std::bitset<128>>");
  }

  TEST_CASE("Example 1: Rho<BooleanMat> vs Lambda<BooleanMat>",
            "[quick][011]") {
    auto                    rg = ReportGuard(false);
    FroidurePin<BooleanMat> S;
    booleanmat_example1(S);
    S.run();

    benchmark_booleanmat_lambda<detail::StaticVector1<BitSet<64>, 64>>(
        S, "Lambda + StaticVector1<BitSet<64>, 64>");

    benchmark_booleanmat_rho<detail::StaticVector1<BitSet<64>, 64>>(
        S, "Rho + StaticVector1<BitSet<64>, 64>");

    benchmark_booleanmat_lambda<detail::StaticVector1<std::bitset<64>, 64>>(
        S, "Lambda + StaticVector1<std::bitset<64>, 64>");

    benchmark_booleanmat_rho<detail::StaticVector1<std::bitset<64>, 64>>(
        S, "Rho + StaticVector1<std::bitset<64>, 64>");

    benchmark_booleanmat_lambda<detail::StaticVector1<BitSet<8>, 8>>(
        S, "Lambda + StaticVector1<BitSet<8>, 8>");

    benchmark_booleanmat_rho<detail::StaticVector1<BitSet<8>, 8>>(
        S, "Rho + StaticVector1<BitSet<8>, 8>");

    benchmark_booleanmat_lambda<detail::StaticVector1<std::bitset<8>, 8>>(
        S, "Lambda + StaticVector1<std::bitset<8>, 8>");

    benchmark_booleanmat_rho<detail::StaticVector1<std::bitset<8>, 8>>(
        S, "Rho + StaticVector1<std::bitset<8>, 8>");

    benchmark_booleanmat_lambda<std::vector<BitSet<64>>>(
        S, "Lambda + std::vector<BitSet<64>>");

    benchmark_booleanmat_rho<std::vector<BitSet<64>>>(
        S, "Rho + std::vector<BitSet<64>>");

    benchmark_booleanmat_lambda<std::vector<std::bitset<64>>>(
        S, "Lambda + std::vector<std::bitset<64>>");

    benchmark_booleanmat_rho<std::vector<std::bitset<64>>>(
        S, "Rho + std::vector<std::bitset<64>>");

    benchmark_booleanmat_lambda<std::vector<BitSet<8>>>(
        S, "Lambda + std::vector<BitSet<8>>");

    benchmark_booleanmat_rho<std::vector<BitSet<8>>>(
        S, "Rho + std::vector<BitSet<8>>");

    benchmark_booleanmat_lambda<std::vector<std::bitset<8>>>(
        S, "Lambda + std::vector<std::bitset<8>>");

    benchmark_booleanmat_rho<std::vector<std::bitset<8>>>(
        S, "Rho + std::vector<std::bitset<8>>");
  }

  TEST_CASE("Example 2: Rho<BooleanMat> vs Lambda<BooleanMat>",
            "[quick][012]") {
    auto                    rg = ReportGuard(false);
    FroidurePin<BooleanMat> S;
    booleanmat_example2(S);
    S.run();

    benchmark_booleanmat_lambda<detail::StaticVector1<BitSet<64>, 64>>(
        S, "Lambda + StaticVector1<BitSet<64>, 64>");

    benchmark_booleanmat_rho<detail::StaticVector1<BitSet<64>, 64>>(
        S, "Rho + StaticVector1<BitSet<64>, 64>");

    benchmark_booleanmat_lambda<detail::StaticVector1<std::bitset<64>, 64>>(
        S, "Lambda + StaticVector1<std::bitset<64>, 64>");

    benchmark_booleanmat_rho<detail::StaticVector1<std::bitset<64>, 64>>(
        S, "Rho + StaticVector1<std::bitset<64>, 64>");

    benchmark_booleanmat_lambda<detail::StaticVector1<BitSet<8>, 8>>(
        S, "Lambda + StaticVector1<BitSet<8>, 8>");

    benchmark_booleanmat_rho<detail::StaticVector1<BitSet<8>, 8>>(
        S, "Rho + StaticVector1<BitSet<8>, 8>");

    benchmark_booleanmat_lambda<detail::StaticVector1<std::bitset<8>, 8>>(
        S, "Lambda + StaticVector1<std::bitset<8>, 8>");

    benchmark_booleanmat_rho<detail::StaticVector1<std::bitset<8>, 8>>(
        S, "Rho + StaticVector1<std::bitset<8>, 8>");

    benchmark_booleanmat_lambda<std::vector<BitSet<64>>>(
        S, "Lambda + std::vector<BitSet<64>>");

    benchmark_booleanmat_rho<std::vector<BitSet<64>>>(
        S, "Rho + std::vector<BitSet<64>>");

    benchmark_booleanmat_lambda<std::vector<std::bitset<64>>>(
        S, "Lambda + std::vector<std::bitset<64>>");

    benchmark_booleanmat_rho<std::vector<std::bitset<64>>>(
        S, "Rho + std::vector<std::bitset<64>>");

    benchmark_booleanmat_lambda<std::vector<BitSet<8>>>(
        S, "Lambda + std::vector<BitSet<8>>");

    benchmark_booleanmat_rho<std::vector<BitSet<8>>>(
        S, "Rho + std::vector<BitSet<8>>");

    benchmark_booleanmat_lambda<std::vector<std::bitset<8>>>(
        S, "Lambda + std::vector<std::bitset<8>>");

    benchmark_booleanmat_rho<std::vector<std::bitset<8>>>(
        S, "Rho + std::vector<std::bitset<8>>");

    benchmark_booleanmat_lambda<std::vector<BitSet<4>>>(
        S, "Lambda + std::vector<BitSet<4>>");

    benchmark_booleanmat_rho<std::vector<BitSet<4>>>(
        S, "Rho + std::vector<BitSet<4>>");

    benchmark_booleanmat_lambda<std::vector<std::bitset<4>>>(
        S, "Lambda + std::vector<std::bitset<4>>");

    benchmark_booleanmat_rho<std::vector<std::bitset<4>>>(
        S, "Rho + std::vector<std::bitset<4>>");
  }

  TEST_CASE("Example 3: transformations, rho", "[quick][013][transf]") {
    auto                                rg = ReportGuard(false);
    FroidurePin<Transformation<size_t>> S;
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
    auto                                rg = ReportGuard(false);
    FroidurePin<Transformation<size_t>> S;
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

  TEST_CASE("Example 4: boolean matrices (dim = 8), rank",
            "[quick][015][boolmat]") {
    auto                    rg = ReportGuard(false);
    FroidurePin<BooleanMat> S;
    booleanmat_example1(S);
    S.run();
    REQUIRE(S.size() == 255);
    std::vector<BooleanMat> gens = {BooleanMat({{1, 0, 0, 0, 0, 0, 0, 0},
                                                {0, 1, 0, 0, 0, 0, 0, 0},
                                                {0, 0, 1, 0, 0, 0, 0, 0},
                                                {0, 0, 0, 1, 0, 0, 0, 0},
                                                {0, 0, 0, 0, 1, 0, 0, 0},
                                                {0, 0, 0, 0, 0, 0, 1, 0},
                                                {0, 0, 0, 0, 0, 0, 0, 1},
                                                {0, 0, 0, 0, 0, 1, 0, 0}}),
                                    BooleanMat({{0, 1, 0, 1, 0, 1, 0, 0},
                                                {0, 1, 1, 0, 1, 1, 0, 0},
                                                {1, 0, 1, 1, 0, 0, 0, 0},
                                                {0, 1, 0, 0, 1, 0, 0, 0},
                                                {0, 0, 1, 0, 0, 1, 0, 0},
                                                {0, 1, 0, 1, 1, 0, 0, 0},
                                                {1, 0, 0, 0, 0, 0, 0, 0},
                                                {0, 0, 0, 0, 0, 0, 0, 0}}),
                                    BooleanMat({{0, 1, 0, 1, 0, 1, 0, 0},
                                                {1, 0, 1, 0, 0, 1, 0, 0},
                                                {1, 0, 1, 1, 0, 1, 0, 0},
                                                {0, 0, 1, 0, 1, 0, 0, 0},
                                                {1, 1, 0, 1, 0, 1, 0, 0},
                                                {0, 1, 0, 1, 0, 1, 0, 0},
                                                {0, 0, 0, 0, 0, 0, 0, 0},
                                                {0, 0, 0, 0, 0, 0, 0, 0}})};

    BENCHMARK("row space size") {
      for (auto it = S.cbegin(); it < S.cend(); ++it) {
        Rank<BooleanMat, void>()(*it);
      }
    };

    BENCHMARK("transformation rank") {
      RankState<BooleanMat> st(gens.cbegin(), gens.cend());
      for (auto it = S.cbegin(); it < S.cend(); ++it) {
        Rank<BooleanMat>()(st, *it);
      }
    };
  }

}  // namespace libsemigroups
