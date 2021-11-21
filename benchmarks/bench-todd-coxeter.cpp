//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2020-21 James D. Mitchell
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

#include <array>     // for array
#include <chrono>    // for duration, seconds
#include <cmath>     // for pow
#include <cstddef>   // for size_t
#include <cstdint>   // for uint64_t
#include <iostream>  // for operator<<, cout, ostream
#include <memory>    // for allocator, shared_ptr, sha...
#include <string>    // for operator+, basic_string
#include <vector>    // for vector, operator==

#include "bench-main.hpp"  // for Benchmark, SourceLineInfo
#include "catch.hpp"       // for REQUIRE, REQUIRE_NOTHROW, REQUIRE_THROWS_AS

#include "libsemigroups/cong-intf.hpp"  // for congruence_kind, congruenc...
#include "libsemigroups/cong-wrap.hpp"  // for CongruenceWrapper
#include "libsemigroups/constants.hpp"  // for PositiveInfinity, POSITIVE...
#include "libsemigroups/froidure-pin-base.hpp"  // for FroidurePinBase
#include "libsemigroups/report.hpp"             // for ReportGuard
#include "libsemigroups/todd-coxeter.hpp"  // for ToddCoxeter, ToddCoxeter::...
#include "libsemigroups/types.hpp"         // for word_type, letter_type

#include "examples/cong-intf.hpp"
#include "examples/fpsemi-intf.hpp"
#include "tests/fpsemi-examples.hpp"

namespace libsemigroups {

  congruence_kind constexpr twosided = congruence_kind::twosided;
  congruence_kind constexpr right    = congruence_kind::right;
  using options                      = congruence::ToddCoxeter::options;

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

    using options = typename congruence::ToddCoxeter::options;

    void check_felsch(congruence::ToddCoxeter& var) {
      SECTION("Felsch (default)") {
        std::cout << "Running Felsch (default) . . ." << std::endl;
        var.strategy(options::strategy::felsch);
      }
    }

    void check_hlt(congruence::ToddCoxeter& var) {
      SECTION("HLT (default)") {
        std::cout << "Running HLT (default) . . ." << std::endl;
        var.strategy(options::strategy::hlt);
      }
    }

    void check_Rc_full_style(congruence::ToddCoxeter& tc) {
      SECTION("Rc style + full lookahead") {
        std::cout << "Running Rc style + full lookahead . . ." << std::endl;
        tc.strategy(options::strategy::Rc).lookahead(options::lookahead::full);
      }
    }
    void check_random(congruence::ToddCoxeter& tc) {
      SECTION("random") {
        std::cout << "Running random strategy . . ." << std::endl;
        tc.strategy(options::strategy::random);
      }
    }
  }  // namespace

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

  namespace congruence {

    ////////////////////////////////////////////////////////////////////////
    // DualSymInv
    ////////////////////////////////////////////////////////////////////////

    TEST_CASE("DualSymInv(n) (Easdown-East-FitzGerald), n = 3 .. 6",
              "[paper][DualSymInv][EEF]") {
      std::array<uint64_t, 9> sizes
          = {0, 0, 0, 25, 339, 6'721, 179'643, 6'166'105, 262'308'819};
      auto rg = ReportGuard(false);
      for (size_t n = 3; n <= 7; ++n) {
        std::string title
            = std::string("DualSymInv(") + std::to_string(n) + ") ";
        BENCHMARK((title + "+ Rc (full HLT lookahead)").c_str()) {
          ToddCoxeter tc(congruence_kind::twosided);
          setup(tc, n + 1, DualSymmetricInverseMonoidEEF, n);
          tc.strategy(ToddCoxeter::options::strategy::Rc)
              .lookahead(ToddCoxeter::options::lookahead::full);
          REQUIRE(tc.number_of_classes() == sizes[n]);
        };
        BENCHMARK((title + "+ HLT (default)").c_str()) {
          ToddCoxeter tc(congruence_kind::twosided);
          setup(tc, n + 1, DualSymmetricInverseMonoidEEF, n);
          tc.strategy(ToddCoxeter::options::strategy::hlt);
          REQUIRE(tc.number_of_classes() == sizes[n]);
        };
        BENCHMARK((title + "+ Felsch (default)").c_str()) {
          ToddCoxeter tc(congruence_kind::twosided);
          setup(tc, n + 1, DualSymmetricInverseMonoidEEF, n);
          tc.strategy(ToddCoxeter::options::strategy::felsch);
          REQUIRE(tc.number_of_classes() == sizes[n]);
        };
        BENCHMARK((title + "+ random").c_str()) {
          ToddCoxeter tc(congruence_kind::twosided);
          setup(tc, n + 1, DualSymmetricInverseMonoidEEF, n);
          tc.strategy(ToddCoxeter::options::strategy::random);
          REQUIRE(tc.number_of_classes() == sizes[n]);
        };
      }
    }

    // Becomes impractical to do multiple runs after n = 6, so we switch to
    // doing single runs.
    namespace {
      template <typename Func>
      void bench_sym_inv(size_t n, size_t size, Func&& foo) {
        auto        rg = ReportGuard(true);
        ToddCoxeter tc(congruence_kind::twosided);
        setup(tc, n + 1, DualSymmetricInverseMonoidEEF, n);
        foo(tc);
        tc.random_interval(std::chrono::seconds(15));
        std::cout << tc.settings_string();
        REQUIRE(tc.number_of_classes() == size);
        std::cout << tc.stats_string();
      }
    }  // namespace

    TEST_CASE("SymInv(7) - Felsch (default)", "[paper][SymInv][n=7][Felsch]") {
      bench_sym_inv(7, 6'166'105, check_felsch);
    }

    TEST_CASE("SymInv(7) - HLT (default)", "[paper][SymInv][n=7][HLT]") {
      bench_sym_inv(7, 6'166'105, check_hlt);
    }

    TEST_CASE("SymInv(7) - Rc (+ full HLT lookahead)",
              "[paper][SymInv][n=7][Rc]") {
      bench_sym_inv(7, 6'166'105, check_Rc_full_style);
    }

    TEST_CASE("SymInv(7) - random strategy", "[paper][SymInv][n=7][random]") {
      bench_sym_inv(7, 6'166'105, check_random);
    }

    ////////////////////////////////////////////////////////////////////////
    // UniformBlockBijection
    ////////////////////////////////////////////////////////////////////////

    TEST_CASE("UniformBlockBijection(n) (FitzGerald), n = 3 .. 7",
              "[paper][UniformBlockBijection][EEF]") {
      std::array<uint64_t, 8> sizes
          = {0, 0, 0, 16, 131, 1'496, 22'482, 426'833};
      auto rg = ReportGuard(false);
      for (size_t n = 3; n <= 7; ++n) {
        std::string title
            = std::string("UniformBlockBijection(") + std::to_string(n) + ") ";
        BENCHMARK((title + "+ Rc (full HLT lookahead)").c_str()) {
          ToddCoxeter tc(congruence_kind::twosided);
          setup(tc, n + 1, UniformBlockBijectionMonoidF, n);
          tc.strategy(ToddCoxeter::options::strategy::Rc)
              .lookahead(ToddCoxeter::options::lookahead::full);
          REQUIRE(tc.number_of_classes() == sizes[n]);
        };
        BENCHMARK((title + "+ HLT (default)").c_str()) {
          ToddCoxeter tc(congruence_kind::twosided);
          setup(tc, n + 1, UniformBlockBijectionMonoidF, n);
          tc.strategy(ToddCoxeter::options::strategy::hlt);
          REQUIRE(tc.number_of_classes() == sizes[n]);
        };
        BENCHMARK((title + "+ Felsch (default)").c_str()) {
          ToddCoxeter tc(congruence_kind::twosided);
          setup(tc, n + 1, UniformBlockBijectionMonoidF, n);
          tc.strategy(ToddCoxeter::options::strategy::felsch);
          REQUIRE(tc.number_of_classes() == sizes[n]);
        };
        BENCHMARK((title + "+ random").c_str()) {
          ToddCoxeter tc(congruence_kind::twosided);
          setup(tc, n + 1, UniformBlockBijectionMonoidF, n);
          tc.strategy(ToddCoxeter::options::strategy::random);
          REQUIRE(tc.number_of_classes() == sizes[n]);
        };
      }
    }

    // Becomes impractical to do multiple runs after n = 7, so we switch to
    // doing single runs.
    namespace {
      template <typename Func>
      void bench_uniform_block_bijection(size_t n, size_t size, Func&& foo) {
        auto        rg = ReportGuard(true);
        ToddCoxeter tc(congruence_kind::twosided);
        setup(tc, n + 1, UniformBlockBijectionMonoidF, n);
        foo(tc);
        std::cout << tc.settings_string();
        REQUIRE(tc.number_of_classes() == size);
        std::cout << tc.stats_string();
      }
    }  // namespace

    // Approx 1m15s (2021 - MacBook Air M1 - 8GB RAM)
    TEST_CASE("UniformBlockBijection(8) - Felsch (default)",
              "[paper][UniformBlockBijection][n=8][Felsch]") {
      bench_uniform_block_bijection(8, 9'934'563, check_felsch);
    }

    // Approx 1m39s (2021 - MacBook Air M1 - 8GB RAM)
    TEST_CASE("UniformBlockBijection(8) - HLT (default)",
              "[paper][UniformBlockBijection][n=8][hlt]") {
      bench_uniform_block_bijection(8, 9'934'563, check_hlt);
    }

    // Approx 1m46s (2021 - MacBook Air M1 - 8GB RAM)
    TEST_CASE("UniformBlockBijection(8) - Rc + full lookahead",
              "[paper][UniformBlockBijection][n=8][Rc]") {
      bench_uniform_block_bijection(8, 9'934'563, check_Rc_full_style);
    }

    // Approx 2m (2021 - MacBook Air M1 - 8GB RAM)
    TEST_CASE("UniformBlockBijection(8) - random",
              "[paper][UniformBlockBijection][n=8][random]") {
      bench_uniform_block_bijection(8, 9'934'563, check_random);
    }

    // |FI_9 ^ *| = 277'006'192 which would require too much memory at present.

    ////////////////////////////////////////////////////////////////////////
    // Stylic
    ////////////////////////////////////////////////////////////////////////

    TEST_CASE("Stylic(n) (Abram-Reutenauer), n = 3 .. 10", "[paper][Stylic]") {
      std::array<uint64_t, 13> sizes = {0,
                                        1,
                                        4,
                                        14,
                                        51,
                                        202,
                                        876,
                                        4'139,
                                        21'146,
                                        115'974,
                                        678'569,
                                        4'213'596,
                                        27'644'436};
      auto                     rg    = ReportGuard(false);
      for (size_t n = 3; n <= 10; ++n) {
        std::string title = std::string("Stylic(") + std::to_string(n) + ") ";
        BENCHMARK((title + "+ HLT (default)").c_str()) {
          ToddCoxeter tc(congruence_kind::twosided);
          setup(tc, n, Stylic, n);
          tc.strategy(ToddCoxeter::options::strategy::hlt);
          REQUIRE(tc.number_of_classes() == sizes[n]);
        };
        BENCHMARK((title + "+ Felsch (default)").c_str()) {
          ToddCoxeter tc(congruence_kind::twosided);
          setup(tc, n, Stylic, n);
          tc.strategy(ToddCoxeter::options::strategy::felsch);
          REQUIRE(tc.number_of_classes() == sizes[n]);
        };
      }
    }

    // Becomes impractical to do multiple runs after n = 10, so we switch to
    // doing single runs.
    namespace {
      template <typename Func>
      void bench_stylic(size_t n, size_t size, Func&& foo) {
        auto        rg = ReportGuard(true);
        ToddCoxeter tc(congruence_kind::twosided);
        setup(tc, n, Stylic, n);
        foo(tc);
        std::cout << tc.settings_string();
        REQUIRE(tc.number_of_classes() == size);
        std::cout << tc.stats_string();
      }
    }  // namespace

    // Approx 17s (2021 - MacBook Air M1 - 8GB RAM)
    TEST_CASE("Stylic(11) - HLT (default)", "[paper][Stylic][n=11][hlt]") {
      bench_stylic(11, 4'213'596, check_hlt);
    }

    // Approx 153s (2021 - MacBook Air M1 - 8GB RAM)
    TEST_CASE("Stylic(12) - HLT (default)", "[paper][Stylic][n=12][hlt]") {
      bench_stylic(12, 27'644'436, check_hlt);
    }

    // Approx ?? (2021 - MacBook Air M1 - 8GB RAM)
    TEST_CASE("Stylic(13) - HLT (default)", "[paper][Stylic][n=13][hlt]") {
      bench_stylic(13, 27'644'436, check_hlt);
    }

    ////////////////////////////////////////////////////////////////////////
    // Stellar
    ////////////////////////////////////////////////////////////////////////

    TEST_CASE("Stellar(n) (Gay-Hivert), n = 3 .. 9", "[paper][Stellar]") {
      std::array<uint64_t, 13> sizes = {1,
                                        2,
                                        5,
                                        16,
                                        65,
                                        326,
                                        1'957,
                                        13'700,
                                        109'601,
                                        986'410,
                                        9'864'101,
                                        108'505'112};
      auto                     rg    = ReportGuard(false);
      for (size_t n = 3; n <= 9; ++n) {
        std::string title = std::string("Stellar(") + std::to_string(n) + ") ";
        BENCHMARK((title + "+ HLT (default)").c_str()) {
          ToddCoxeter tc1(congruence_kind::twosided);
          setup(tc1, n + 1, RookMonoid, n, 0);
          ToddCoxeter tc2(congruence_kind::twosided, tc1);
          setup(tc2, n + 1, Stell, n);
          tc2.strategy(ToddCoxeter::options::strategy::hlt);
          REQUIRE(tc2.number_of_classes() == sizes[n]);
        };
        BENCHMARK((title + "+ Felsch (default)").c_str()) {
          ToddCoxeter tc1(congruence_kind::twosided);
          setup(tc1, n + 1, RookMonoid, n, 0);
          ToddCoxeter tc2(congruence_kind::twosided, tc1);
          setup(tc2, n + 1, Stell, n);
          tc2.strategy(ToddCoxeter::options::strategy::felsch);
          REQUIRE(tc2.number_of_classes() == sizes[n]);
        };
      }
    }

    // Becomes impractical to do multiple runs after n = 9, so we switch to
    // doing single runs.
    namespace {
      void bench_stellar(size_t n, size_t size) {
        auto        rg = ReportGuard(true);
        ToddCoxeter tc1(congruence_kind::twosided);
        setup(tc1, n + 1, RookMonoid, n, 0);
        ToddCoxeter tc2(congruence_kind::twosided, tc1);
        setup(tc2, n + 1, Stell, n);
        std::cout << tc2.settings_string();
        REQUIRE(tc2.number_of_classes() == size);
        std::cout << tc2.stats_string();
      }

      // Approx 90s (2021 - MacBook Air M1 - 8GB RAM)
      TEST_CASE("Stellar(10) - HLT (default)", "[paper][Stellar][n=10][hlt]") {
        bench_stellar(10, 9'864'101);
      }

      // Approx 22m52s (2021 - MacBook Air M1 - 8GB RAM)
      TEST_CASE("Stellar(11) - HLT (default)", "[paper][Stellar][n=11][hlt]") {
        bench_stellar(11, 108'505'112);
      }
    }  // namespace

    ////////////////////////////////////////////////////////////////////////
    // PartitionMonoid
    ////////////////////////////////////////////////////////////////////////

    TEST_CASE("PartitionMonoid(n) (East 41), n = 4 .. 6",
              "[paper][PartitionMonoid]") {
      std::array<uint64_t, 13> sizes
          = {0, 2, 15, 203, 4'140, 115'975, 4'213'597, 190'899'322};

      auto rg = ReportGuard(false);
      for (size_t n = 4; n <= 6; ++n) {
        std::string title
            = std::string("PartitionMonoid(") + std::to_string(n) + ") ";
        BENCHMARK((title + "+ HLT (default) + save").c_str()) {
          ToddCoxeter tc(congruence_kind::twosided);
          setup(tc, 5, PartitionMonoidEast41, n);
          tc.strategy(ToddCoxeter::options::strategy::hlt).save(true);
          REQUIRE(tc.number_of_classes() == sizes[n]);
        };
        BENCHMARK((title + "+ Felsch (default)").c_str()) {
          ToddCoxeter tc(congruence_kind::twosided);
          setup(tc, 5, PartitionMonoidEast41, n);
          tc.strategy(ToddCoxeter::options::strategy::felsch);
          REQUIRE(tc.number_of_classes() == sizes[n]);
        };
      }
    }

    // Becomes impractical to do multiple runs after n = 7, so we switch to
    // doing single runs.
    namespace {
      void bench_partition(size_t n, size_t size) {
        auto        rg = ReportGuard(true);
        ToddCoxeter tc(congruence_kind::twosided);
        setup(tc, 5, PartitionMonoidEast41, n);
        tc.lookahead(options::lookahead::hlt | options::lookahead::partial)
            .use_relations_in_extra(true)
            .sort_generating_pairs()
            .remove_duplicate_generating_pairs()
            .next_lookahead(200'000)
            .lookahead_growth_factor(2.5)
            .reserve(15'000'000);
        check_hlt(tc);
        std::cout << tc.settings_string();
        REQUIRE(tc.number_of_classes() == size);
        std::cout << tc.stats_string();
      }
    }  // namespace

    // Approx 31s (2021 - MacBook Air M1 - 8GB RAM)
    TEST_CASE("PartitionMonoid(6) - hlt",
              "[paper][PartitionMonoid][n=6][hlt]") {
      bench_partition(6, 4'213'597);
    }

    // Approx 49m35s ??
    TEST_CASE("PartitionMonoid(7) - hlt",
              "[paper][PartitionMonoid][n=7][hlt]") {
      bench_partition(7, 190'899'322);
    }

    ////////////////////////////////////////////////////////////////////////
    // SingularBrauer
    ////////////////////////////////////////////////////////////////////////

    TEST_CASE("SingularBrauer(n) (Maltcev-Mazorchuk), n = 4 .. 7",
              "[paper][SingularBrauer]") {
      std::array<uint64_t, 13> sizes
          = {0, 0, 0, 9, 81, 825, 9'675, 130'095, 1'986'705, 34'096'545};

      auto rg = ReportGuard(false);
      for (size_t n = 3; n <= 7; ++n) {
        std::string title
            = std::string("SingularBrauer(") + std::to_string(n) + ") ";
        BENCHMARK((title + "+ HLT (default)").c_str()) {
          ToddCoxeter tc(congruence_kind::twosided);
          setup(tc, n * n - n, SingularBrauer, n);
          tc.strategy(ToddCoxeter::options::strategy::hlt);
          REQUIRE(tc.number_of_classes() == sizes[n]);
        };
        BENCHMARK((title + "+ Felsch (default)").c_str()) {
          ToddCoxeter tc(congruence_kind::twosided);
          setup(tc, n * n - n, SingularBrauer, n);
          tc.strategy(ToddCoxeter::options::strategy::felsch);
          REQUIRE(tc.number_of_classes() == sizes[n]);
        };
      }
    }

    namespace {
      void bench_singular_brauer(size_t n, size_t size) {
        auto        rg = ReportGuard(true);
        ToddCoxeter tc(congruence_kind::twosided);
        setup(tc, n * n - n, SingularBrauer, n);
        tc.sort_generating_pairs().remove_duplicate_generating_pairs();
        tc.strategy(options::strategy::hlt)
            .next_lookahead(size / 2)
            .min_lookahead(size / 2);
        check_hlt(tc);
        std::cout << tc.settings_string();
        REQUIRE(tc.number_of_classes() == size);
        std::cout << tc.stats_string();
      }
    }  // namespace

    TEST_CASE("SingularBrauer(8) (Maltcev-Mazorchuk)",
              "[paper][SingularBrauer][n=8]") {
      bench_singular_brauer(8, 1'986'705);
    }

    TEST_CASE("SingularBrauer(9) (Maltcev-Mazorchuk)",
              "[paper][SingularBrauer][n=9]") {
      bench_singular_brauer(9, 34'096'545);
    }

    ////////////////////////////////////////////////////////////////////////
    // OrientationPreserving
    ////////////////////////////////////////////////////////////////////////

    TEST_CASE("OrientationPreserving(n) (Arthur-Ruskuc), n = 3 .. 9",
              "[paper][OP][OrientationPreserving]") {
      std::array<uint64_t, 15> sizes = {0,
                                        0,
                                        0,
                                        24,
                                        128,
                                        610,
                                        2'742,
                                        11'970,
                                        51'424,
                                        218'718,
                                        923'690,
                                        3'879'766,
                                        16'224'804,
                                        67'603'744,
                                        280'816'018};

      auto rg = ReportGuard(false);
      for (size_t n = 3; n <= 9; ++n) {
        std::string title
            = std::string("OrientationPreserving(") + std::to_string(n) + ") ";
        BENCHMARK((title + "+ HLT (default)").c_str()) {
          ToddCoxeter tc(congruence_kind::twosided);
          setup(tc, 3, OrientationPreserving, n);
          tc.strategy(ToddCoxeter::options::strategy::hlt);
          REQUIRE(tc.number_of_classes() == sizes[n]);
        };
        BENCHMARK((title + "+ Felsch (default)").c_str()) {
          ToddCoxeter tc(congruence_kind::twosided);
          setup(tc, 3, OrientationPreserving, n);
          tc.strategy(ToddCoxeter::options::strategy::felsch);
          REQUIRE(tc.number_of_classes() == sizes[n]);
        };
      }
    }

    // Becomes impractical to do multiple runs after n = 7, so we switch to
    // doing single runs.
    namespace {
      void bench_orientation(size_t n, size_t size) {
        auto        rg = ReportGuard(true);
        ToddCoxeter tc(congruence_kind::twosided);
        setup(tc, 3, OrientationPreserving, n);
        check_hlt(tc);
        std::cout << tc.settings_string();
        REQUIRE(tc.number_of_classes() == size);
        std::cout << tc.stats_string();
      }
    }  // namespace

    // Approx 27s (2021 - MacBook Air M1 - 8GB RAM)
    TEST_CASE("OrientationPreserving(10) - hlt",
              "[paper][OrientationPreserving][n=10][hlt]") {
      bench_orientation(10, 923'690);
    }

    // 4m13s (2021 - MacBook Air M1 - 8GB RAM)
    TEST_CASE("OrientationPreserving(11) - hlt",
              "[paper][OrientationPreserving][n=11][hlt]") {
      bench_orientation(11, 3'879'766);
    }

    // 54m35s (2021 - MacBook Air M1 - 8GB RAM)
    TEST_CASE("OrientationPreserving(12) - hlt",
              "[paper][OrientationPreserving][n=12][hlt]") {
      bench_orientation(12, 16'224'804);
    }

    // 9h14m (2021 - MacBook Air M1 - 8GB RAM)
    TEST_CASE("OrientationPreserving(13) - hlt",
              "[paper][OrientationPreserving][n=13][hlt]") {
      bench_orientation(13, 67'603'744);
    }

    ////////////////////////////////////////////////////////////////////////
    // TemperleyLieb
    ////////////////////////////////////////////////////////////////////////

    TEST_CASE("TemperleyLieb(n) (East), n = 3 .. 14",
              "[paper][TemperleyLieb]") {
      std::array<uint64_t, 17> sizes = {0,
                                        0,
                                        0,
                                        5,
                                        14,
                                        42,
                                        132,
                                        429,
                                        1'430,
                                        4'862,
                                        16'796,
                                        58'786,
                                        208'012,
                                        742'900,
                                        2'674'440,
                                        9'694'845,
                                        35'357'670};

      auto rg = ReportGuard(false);
      for (size_t n = 3; n <= 14; ++n) {
        std::string title
            = std::string("TemperleyLieb(") + std::to_string(n) + ") ";
        BENCHMARK((title + "+ HLT (default)").c_str()) {
          ToddCoxeter tc(congruence_kind::twosided);
          setup(tc, n - 1, TemperleyLieb, n);
          tc.strategy(ToddCoxeter::options::strategy::hlt);
          REQUIRE(tc.number_of_classes() == sizes[n] - 1);
        };
        BENCHMARK((title + "+ Felsch (default)").c_str()) {
          ToddCoxeter tc(congruence_kind::twosided);
          setup(tc, n - 1, TemperleyLieb, n);
          tc.strategy(ToddCoxeter::options::strategy::felsch);
          REQUIRE(tc.number_of_classes() == sizes[n] - 1);
        };
      }
    }

    // Becomes impractical to do multiple runs after n = 14, so we switch to
    // doing single runs.
    namespace {
      void bench_temperley_lieb(size_t n, size_t size) {
        auto        rg = ReportGuard(true);
        ToddCoxeter tc(congruence_kind::twosided);
        setup(tc, n - 1, TemperleyLieb, n);
        check_hlt(tc);
        std::cout << tc.settings_string();
        REQUIRE(tc.number_of_classes() == size);
        std::cout << tc.stats_string();
      }
    }  // namespace

    // Approx. ? (2021 - MacBook Air M1 - 8GB RAM)
    TEST_CASE("TemperleyLieb(15) - hlt", "[paper][TemperleyLieb][n=15][hlt]") {
      bench_temperley_lieb(15, 9'694'845);
    }

    // Approx. ? (2021 - MacBook Air M1 - 8GB RAM)
    TEST_CASE("TemperleyLieb(16) - hlt", "[paper][TemperleyLieb][n=16][hlt]") {
      bench_temperley_lieb(15, 35'357'670);
    }

    // Approx. ? (2021 - MacBook Air M1 - 8GB RAM)
    TEST_CASE("TemperleyLieb(17) - hlt", "[paper][TemperleyLieb][n=17][hlt]") {
      bench_temperley_lieb(15, 129'644'790);
    }

    ////////////////////////////////////////////////////////////////////////
    // OrientationReversing
    ////////////////////////////////////////////////////////////////////////

    TEST_CASE("OrientationReversing(n) (Arthur-Ruskuc), n = 3 .. 8",
              "[paper][OrientationReversing][OR]") {
      std::array<uint64_t, 17> sizes = {0,
                                        0,
                                        0,
                                        27,
                                        180,
                                        1'015,
                                        5'028,
                                        23'051,
                                        101'272,
                                        434'835,
                                        1'843'320,
                                        7'753'471,
                                        32'440'884,
                                        135'195'307,
                                        561'615'460,
                                        2'326'740'315};

      auto rg = ReportGuard(false);
      for (size_t n = 3; n <= 8; ++n) {
        std::string title
            = std::string("OrientationReversing(") + std::to_string(n) + ") ";
        BENCHMARK((title + "+ HLT (default)").c_str()) {
          ToddCoxeter tc(congruence_kind::twosided);
          setup(tc, 4, OrientationReversing, n);
          tc.strategy(ToddCoxeter::options::strategy::hlt);
          tc.next_lookahead(4 * sizes[n]).sort_generating_pairs();
          REQUIRE(tc.number_of_classes() == sizes[n]);
        };
        BENCHMARK((title + "+ Felsch (default)").c_str()) {
          ToddCoxeter tc(congruence_kind::twosided);
          setup(tc, 4, OrientationReversing, n);
          tc.strategy(ToddCoxeter::options::strategy::felsch);
          REQUIRE(tc.number_of_classes() == sizes[n]);
        };
      }
    }

    namespace {
      void bench_orient_reverse(size_t n, size_t size) {
        auto        rg = ReportGuard(true);
        ToddCoxeter tc(congruence_kind::twosided);
        setup(tc, 4, OrientationReversing, n);
        check_hlt(tc);
        tc.next_lookahead(5'000'000).sort_generating_pairs();
        std::cout << tc.settings_string();
        REQUIRE(tc.number_of_classes() == size);
        std::cout << tc.stats_string();
      }
    }  // namespace

    // Approx 9s (2021 - MacBook Air M1 - 8GB RAM)
    TEST_CASE("OrientationReversing(9) - hlt",
              "[paper][OrientationReversing][n=9][hlt]") {
      bench_orient_reverse(9, 434'835);
    }

    // Approx 90s (2021 - MacBook Air M1 - 8GB RAM)
    TEST_CASE("OrientationReversing(10) - hlt",
              "[paper][OrientationReversing][n=10][hlt]") {
      bench_orient_reverse(10, 1'843'320);
    }

    // ?? (2021 - MacBook Air M1 - 8GB RAM)
    TEST_CASE("OrientationReversing(11) - hlt",
              "[paper][OrientationReversing][n=11][hlt]") {
      bench_orient_reverse(11, 7'753'471);
    }

    // ?? (2021 - MacBook Air M1 - 8GB RAM)
    TEST_CASE("OrientationReversing(12) - hlt",
              "[paper][OrientationReversing][n=12][hlt]") {
      bench_orient_reverse(12, 32'440'884);
    }

    // ?? (2021 - MacBook Air M1 - 8GB RAM)
    TEST_CASE("OrientationReversing(13) - hlt",
              "[paper][OrientationReversing][n=13][hlt]") {
      bench_orient_reverse(13, 135'195'307);
    }
  }  // namespace congruence

  namespace fpsemigroup {
    TEST_CASE("ACE --- 2p17-2p14", "[paper][ace][2p17-2p14]") {
      auto        rg = ReportGuard(false);
      ToddCoxeter G;
      G.set_alphabet("abcABCe");
      G.set_identity("e");
      G.set_inverses("ABCabce");
      G.add_rule("aBCbac", "e");
      G.add_rule("bACbaacA", "e");
      G.add_rule("accAABab", "e");

      BENCHMARK("HLT") {
        congruence::ToddCoxeter H(right, G.congruence());
        H.add_pair({1, 2}, {6});
        H.simplify();
        H.next_lookahead(1'000'000).lookahead(options::lookahead::partial);
        REQUIRE(H.number_of_classes() == 16'384);
      };
    }

    TEST_CASE("ACE --- 2p17-2p3", "[paper][ace][2p17-2p3]") {
      auto        rg = ReportGuard(false);
      ToddCoxeter G;
      G.set_alphabet("abcABCe");
      G.set_identity("e");
      G.set_inverses("ABCabce");
      G.add_rule("aBCbac", "e");
      G.add_rule("bACbaacA", "e");
      G.add_rule("accAABab", "e");

      letter_type a = 0;
      letter_type b = 1;
      letter_type c = 2;
      letter_type A = 3;
      letter_type B = 4;
      letter_type C = 5;
      letter_type e = 6;
      BENCHMARK("HLT") {
        congruence::ToddCoxeter H(right, G.congruence());
        H.add_pair({b, c}, {e});
        H.add_pair({A, B, A, A, b, c, a, b, C}, {e});

        H.strategy(options::strategy::hlt)
            .save(true)
            .lookahead(options::lookahead::partial);

        REQUIRE(H.number_of_classes() == 8);
      };
    }

    TEST_CASE("ACE --- 2p17-fel1", "[paper][ace][2p17-1]") {
      auto        rg = ReportGuard(false);
      ToddCoxeter G;
      G.set_alphabet("abcABCe");
      G.set_identity("e");
      G.set_inverses("ABCabce");
      G.add_rule("aBCbac", "e");
      G.add_rule("bACbaacA", "e");
      G.add_rule("accAABab", "e");

      letter_type a = 0;
      letter_type b = 1;
      letter_type c = 2;
      letter_type A = 3;
      letter_type B = 4;
      letter_type C = 5;
      letter_type e = 6;

      BENCHMARK("HLT") {
        congruence::ToddCoxeter H(right, G.congruence());
        H.add_pair({e}, {a, B, C, b, a, c});
        H.add_pair({b, A, C, b, a, a, c, A}, {e});
        H.add_pair({a, c, c, A, A, B, a, b}, {e});

        H.save(true)
            .lookahead(options::lookahead::partial)
            .max_deductions(20'000)
            .large_collapse(10'000)
            .remove_duplicate_generating_pairs();
        REQUIRE(H.number_of_classes() == 131'072);
      };
    }

    TEST_CASE("ACE --- 2p17-fel1a", "[paper][ace][2p17-1a]") {
      auto        rg = ReportGuard(false);
      ToddCoxeter G;
      G.set_alphabet("abcABCe");
      G.set_identity("e");
      G.set_inverses("ABCabce");
      G.add_rule("aBCbac", "e");
      G.add_rule("bACbaacA", "e");
      G.add_rule("accAABab", "e");

      letter_type a = 0;
      letter_type b = 1;
      letter_type c = 2;
      letter_type A = 3;
      letter_type B = 4;
      letter_type C = 5;
      letter_type e = 6;

      BENCHMARK("HLT") {
        congruence::ToddCoxeter H(right, G.congruence());
        H.add_pair({b, c}, {e});
        H.add_pair({A, B, A, A, b, c, a, b, C}, {e});
        H.add_pair({A, c, c, c, a, c, B, c, A}, {e});

        H.strategy(options::strategy::hlt)
            .save(true)
            .lookahead(options::lookahead::full)
            .max_deductions(10'000)
            .large_collapse(10'000);
        REQUIRE(H.number_of_classes() == 1);
      };
    }

    TEST_CASE("ACE --- 2p17-id-fel1", "[paper][ace][2p17-id]") {
      auto rg = ReportGuard(false);
      BENCHMARK("HLT") {
        ToddCoxeter G;
        G.set_alphabet("abcABCe");
        G.set_identity("e");
        G.set_inverses("ABCabce");
        G.add_rule("aBCbac", "e");
        G.add_rule("bACbaacA", "e");
        G.add_rule("accAABab", "e");

        G.congruence().reserve(5'000'000);
        G.congruence()
            .strategy(options::strategy::hlt)
            .lookahead(options::lookahead::partial)
            .save(true)
            .max_deductions(POSITIVE_INFINITY);

        REQUIRE(G.size() == std::pow(2, 17));
      };
    }

    TEST_CASE("ACE --- 2p18-fe1", "[paper][ace][2p18]") {
      auto        rg = ReportGuard(false);
      ToddCoxeter G;
      G.set_alphabet("abcABCex");
      G.set_identity("e");
      G.set_inverses("ABCabcex");
      G.add_rule("aBCbac", "e");
      G.add_rule("bACbaacA", "e");
      G.add_rule("accAABab", "e");
      G.add_rule("xx", "e");
      G.add_rule("Axax", "e");
      G.add_rule("Bxbx", "e");
      G.add_rule("Cxcx", "e");

      letter_type constexpr a = 0, b = 1, c = 2, A = 3, B = 4, C = 5, e = 6;

      BENCHMARK("HLT") {
        congruence::ToddCoxeter H(right, G.congruence());
        H.add_pair({a, B, C, b, a, c}, {e});
        H.add_pair({b, A, C, b, a, a, c, A}, {e});
        H.add_pair({a, c, c, A, A, B, a, b}, {e});

        H.strategy(options::strategy::hlt)
            .save(true)
            .lookahead(options::lookahead::partial)
            .sort_generating_pairs()
            .remove_duplicate_generating_pairs()
            .large_collapse(10'000)
            .max_deductions(10'000)
            .next_lookahead(5'000'000);

        REQUIRE(H.number_of_classes() == 262'144);
      };
    }

    TEST_CASE("ACE --- F27", "[paper][ace][F27]") {
      auto rg = ReportGuard(false);
      BENCHMARK("HLT") {
        ToddCoxeter G;
        G.set_alphabet("abcdxyzABCDXYZe");
        G.set_identity("e");
        G.set_inverses("ABCDXYZabcdxyze");
        G.add_rule("abC", "e");
        G.add_rule("bcD", "e");
        G.add_rule("cdX", "e");
        G.add_rule("dxY", "e");
        G.add_rule("xyZ", "e");
        G.add_rule("yzA", "e");
        G.add_rule("zaB", "e");

        G.congruence()
            .strategy(options::strategy::hlt)
            .save(true)
            .lookahead(options::lookahead::partial);
        REQUIRE(G.size() == 29);
      };
    }

    TEST_CASE("ACE --- M12", "[paper][ace][M12]") {
      auto rg = ReportGuard(false);
      BENCHMARK("HLT") {
        ToddCoxeter G;
        G.set_alphabet("abcABCe");
        G.set_identity("e");
        G.set_inverses("ABCabce");
        G.add_rule("aaaaaaaaaaa", "e");
        G.add_rule("bb", "e");
        G.add_rule("cc", "e");
        G.add_rule("ababab", "e");
        G.add_rule("acacac", "e");
        G.add_rule("bcbcbcbcbcbcbcbcbcbc", "e");
        G.add_rule("cbcbabcbcAAAAA", "e");

        congruence::ToddCoxeter H(twosided, G);

        H.strategy(options::strategy::hlt)
            .save(true)
            .lookahead(options::lookahead::partial);

        REQUIRE(H.number_of_classes() == 95'040);
      };
    }

    TEST_CASE("ACE --- SL(2, 19)", "[paper][ace][SL219]") {
      auto rg = ReportGuard(false);
      BENCHMARK("HLT") {
        ToddCoxeter G;
        G.set_alphabet("abABe");
        G.set_identity("e");
        G.set_inverses("ABabe");
        G.add_rule("aBABAB", "e");
        G.add_rule("BAAbaa", "e");
        G.add_rule(
            "abbbbabbbbbbbbbbabbbbabbbbbbbbbbbbbbbbbbbbbbbbbbbbbaaaaaaaaaaaa",
            "e");

        letter_type b = 1;
        letter_type e = 4;

        congruence::ToddCoxeter H(right, G);
        H.add_pair({b}, {e});

        H.strategy(options::strategy::hlt)
            .save(false)
            .lookahead(options::lookahead::partial)
            .next_lookahead(500'000);
        REQUIRE(H.number_of_classes() == 180);
      };
    }

    TEST_CASE("ACE --- big-hard", "[paper][ace][big-hard]") {
      auto        rg = ReportGuard(false);
      ToddCoxeter G;
      G.set_alphabet("abcyABCYex");
      G.set_identity("e");
      G.set_inverses("ABCYabcyex");
      G.add_rule("aBCbac", "e");
      G.add_rule("bACbaacA", "e");
      G.add_rule("accAABab", "e");
      G.add_rule("xx", "e");
      G.add_rule("yyy", "e");
      G.add_rule("Axax", "e");
      G.add_rule("Bxbx", "e");
      G.add_rule("Cxcx", "e");
      G.add_rule("AYay", "e");
      G.add_rule("BYby", "e");
      G.add_rule("CYcy", "e");
      G.add_rule("xYxy", "e");

      letter_type constexpr a = 0, b = 1, c = 2, A = 4, B = 5, C = 6, e = 8;

      BENCHMARK("HLT") {
        congruence::ToddCoxeter H(right, G.congruence());
        H.add_pair({a, B, C, b, a, c}, {e});
        H.add_pair({b, A, C, b, a, a, c, A}, {e});
        H.add_pair({a, c, c, A, A, B, a, b}, {e});
        H.strategy(options::strategy::hlt)
            .save(true)
            .lookahead(options::lookahead::partial)
            .next_lookahead(1'000'000)
            .large_collapse(5'000)
            .max_deductions(1'000'000)
            .lower_bound(786'432);
        REQUIRE(H.number_of_classes() == 786'432);
      };
    }

    TEST_CASE("ACE --- g25.a", "[paper][ace][g25.a]") {
      auto        rg = ReportGuard(true);
      ToddCoxeter G;
      G.set_alphabet("abcdeABCDx");
      G.set_identity("x");
      G.set_inverses("ABCDeabcdx");

      G.add_rule("ee", "x");
      G.add_rule("DaDa", "x");
      G.add_rule("dddd", "x");
      G.add_rule("BDbd", "x");
      G.add_rule("ccccc", "x");
      G.add_rule("bbbbb", "x");
      G.add_rule("AABaab", "x");
      G.add_rule("ddAAAA", "x");
      G.add_rule("AAcaacc", "x");
      G.add_rule("ececBBC", "x");
      G.add_rule("abababab", "x");
      G.add_rule("BBcbceceBCC", "x");
      G.add_rule("ebcBebCBBcB", "x");
      G.add_rule("ebebccBBcbC", "x");
      G.add_rule("ACabCBAcabcB", "x");
      G.add_rule("ABabABabABab", "x");
      G.add_rule("CACaCaCAccaCA", "x");
      G.add_rule("ABcbabCBCBccb", "x");
      G.add_rule("BCbcACaCBcbAca", "x");
      G.add_rule("eabbaBAeabbaBA", "x");
      G.add_rule("eBcbeabcBcACBB", "x");
      G.add_rule("BCbAcaBcbCACac", "x");
      G.add_rule("CACacaCAcACaCACa", "x");
      G.add_rule("CAcacbcBCACacbCB", "x");
      G.add_rule("CaCAcacAcaCACacA", "x");
      G.add_rule("cacbcBACCaCbCBAc", "x");
      G.add_rule("CBCbcBcbCCACACaca", "x");
      G.add_rule("BAcabbcBcbeCebACa", "x");
      G.add_rule("ACacAcaebcBCBcBCe", "x");
      G.add_rule("eDCDbABCDACaCAcabb", "x");
      G.add_rule("BCbbCBBcbbACacAcaB", "x");
      G.add_rule("eaaebcBACaCAcbABBA", "x");
      G.add_rule("BACaCAcacbCACacAca", "x");
      G.add_rule("AbcBabCBCbCBBcbAcaC", "x");
      G.add_rule("aabaBabaabaBabaabaBab", "x");
      G.add_rule("eAcaeACaeAcabCBaBcbaaa", "x");
      G.add_rule("deBAceAeACACacAcabcBcbaBBA", "x");
      G.add_rule("dCACacAcadACaCAcacdCACacAcA", "x");
      G.add_rule("dCACacAcadCACacAcadCACacAcadCACacAcadCACacAcadCACacAca", "x");

      G.congruence()
          .remove_duplicate_generating_pairs()
          .sort_generating_pairs();
      REQUIRE(G.size() == 1);
      std::cout << G.congruence().stats_string();
    }

    namespace {
      void walker1(ToddCoxeter& tc) {
        tc.set_alphabet("abc");
        tc.add_rule("aaaaaaaaaaaaaa", "a");
        tc.add_rule("bbbbbbbbbbbbbb", "b");
        tc.add_rule("cccccccccccccc", "c");
        tc.add_rule("aaaaba", "bbb");
        tc.add_rule("bbbbab", "aaa");
        tc.add_rule("aaaaca", "ccc");
        tc.add_rule("ccccac", "aaa");
        tc.add_rule("bbbbcb", "ccc");
        tc.add_rule("ccccbc", "bbb");
      }

      void walker2(ToddCoxeter& tc) {
        tc.set_alphabet("ab");
        tc.add_rule("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", "a");
        tc.add_rule("bbb", "b");
        tc.add_rule("ababa", "b");
        tc.add_rule("aaaaaaaaaaaaaaaabaaaabaaaaaaaaaaaaaaaabaaaa", "b");
      }

      void walker3(ToddCoxeter& tc) {
        tc.set_alphabet("ab");
        tc.add_rule("aaaaaaaaaaaaaaaa", "a");
        tc.add_rule("bbbbbbbbbbbbbbbb", "b");
        tc.add_rule("abb", "baa");
      }

      void walker4(ToddCoxeter& tc) {
        tc.set_alphabet("ab");
        tc.add_rule("aaa", "a");
        tc.add_rule("bbbbbb", "b");
        tc.add_rule("ababbbbababbbbababbbbababbbbababbbbababbbbababbbbabba",
                    "bb");
      }

      void walker5(ToddCoxeter& tc) {
        tc.set_alphabet("ab");
        tc.add_rule("aaa", "a");
        tc.add_rule("bbbbbb", "b");
        tc.add_rule(
            "ababbbbababbbbababbbbababbbbababbbbababbbbababbbbabbabbbbbaa",
            "bb");
      }

      void walker6(ToddCoxeter& tc) {
        tc.set_alphabet("ab");
        tc.add_rule("aaa", "a");
        tc.add_rule("bbbbbbbbb", "b");
        std::string lng("ababbbbbbb");
        lng += lng;
        lng += "abbabbbbbbbb";
        tc.add_rule(lng, "bb");
      }

      void walker7(ToddCoxeter& tc) {
        tc.set_alphabet("abcde");
        tc.add_rule("aaa", "a");
        tc.add_rule("bbb", "b");
        tc.add_rule("ccc", "c");
        tc.add_rule("ddd", "d");
        tc.add_rule("eee", "e");
        tc.add_rule("ababab", "aa");
        tc.add_rule("bcbcbc", "bb");
        tc.add_rule("cdcdcd", "cc");
        tc.add_rule("dedede", "dd");
        tc.add_rule("ac", "ca");
        tc.add_rule("ad", "da");
        tc.add_rule("ae", "ea");
        tc.add_rule("bd", "db");
        tc.add_rule("be", "eb");
        tc.add_rule("ce", "ec");
      }

      void walker8(ToddCoxeter& tc) {
        tc.set_alphabet("ab");
        tc.add_rule("aaa", "a");
        tc.add_rule("bbbbbbbbbbbbbbbbbbbbbbb", "b");
        tc.add_rule("abbbbbbbbbbbabb", "bba");
      }
    }  // namespace

    TEST_CASE("Walker 1", "[quick][Walker1][paper]") {
      using options = libsemigroups::congruence::ToddCoxeter::options;
      auto rg       = ReportGuard(false);
      BENCHMARK("HLT") {
        ToddCoxeter tc;
        walker1(tc);
        tc.congruence()
            .sort_generating_pairs()
            .strategy(options::strategy::hlt)
            .next_lookahead(500'000)
            .large_collapse(2'000);

        REQUIRE(tc.size() == 1);
      };
      BENCHMARK("Felsch") {
        ToddCoxeter tc;
        walker1(tc);
        tc.congruence().simplify();
        tc.congruence()
            .sort_generating_pairs()
            .strategy(options::strategy::felsch)
            .max_deductions(2'000)
            .preferred_defs(options::preferred_defs::immediate_no_stack)
            .large_collapse(10'000);
        REQUIRE(tc.size() == 1);
      };
    }

    TEST_CASE("Walker 2", "[quick][Walker2][paper]") {
      using options = libsemigroups::congruence::ToddCoxeter::options;
      auto rg       = ReportGuard(false);
      BENCHMARK("HLT") {
        ToddCoxeter tc;
        walker2(tc);
        tc.congruence().simplify();
        tc.congruence()
            .strategy(options::strategy::hlt)
            .next_lookahead(2'000'000);
        REQUIRE(tc.size() == 14'911);
      };

      BENCHMARK("Felsch") {
        ToddCoxeter tc;
        walker2(tc);
        tc.congruence().simplify();
        tc.congruence()
            .strategy(options::strategy::felsch)
            .use_relations_in_extra(true)
            .max_deductions(100'000);
        REQUIRE(tc.size() == 14'911);
      };

      BENCHMARK("HLT tweaked") {
        ToddCoxeter tc;
        walker2(tc);
        tc.congruence().simplify();
        tc.congruence()
            .sort_generating_pairs()
            .next_lookahead(1'000'000)
            .max_deductions(2'000)
            .use_relations_in_extra(true)
            .strategy(options::strategy::hlt)
            .lookahead(options::lookahead::partial | options::lookahead::felsch)
            .deduction_policy(options::deductions::v2
                              | options::deductions::no_stack_if_no_space);
        REQUIRE(tc.size() == 14'911);
      };
    }

    TEST_CASE("Walker 3", "[quick][Walker3][paper]") {
      using options = libsemigroups::congruence::ToddCoxeter::options;
      auto rg       = ReportGuard(false);
      BENCHMARK("HLT") {
        ToddCoxeter tc;
        walker3(tc);
        tc.congruence().next_lookahead(2'000'000).strategy(
            options::strategy::hlt);
        REQUIRE(tc.size() == 20'490);
      };

      BENCHMARK("Felsch") {
        ToddCoxeter tc;
        walker3(tc);
        tc.congruence()
            .strategy(options::strategy::felsch)
            .use_relations_in_extra(true)
            .max_deductions(100'000)
            .deduction_policy(options::deductions::v1
                              | options::deductions::no_stack_if_no_space)
            .preferred_defs(options::preferred_defs::none);
        REQUIRE(tc.size() == 20'490);
      };
    }

    TEST_CASE("Walker 4", "[quick][Walker4][paper]") {
      using options = libsemigroups::congruence::ToddCoxeter::options;
      auto rg       = ReportGuard(false);
      BENCHMARK("HLT") {
        ToddCoxeter tc;
        walker4(tc);
        tc.congruence().next_lookahead(3'000'000).strategy(
            options::strategy::hlt);
        REQUIRE(tc.size() == 36'412);
      };
    }

    TEST_CASE("Walker 4 - Felsch only", "[paper][Walker4][felsch]") {
      auto        rg = ReportGuard(true);
      ToddCoxeter tc;
      walker4(tc);
      tc.congruence().simplify(3);
      REQUIRE(std::vector<word_type>(tc.congruence().cbegin_relations(),
                                     tc.congruence().cend_relations())
              == std::vector<word_type>({{0},
                                         {0, 0, 0},
                                         {1, 1},
                                         {0, 1, 2, 2, 2, 2, 2, 2, 2, 1, 0},
                                         {1},
                                         {3, 1, 1},
                                         {2},
                                         {0, 3, 0, 1},
                                         {3},
                                         {1, 1, 1, 1}}));

      tc.congruence()
          .strategy(options::strategy::felsch)
          .preferred_defs(options::preferred_defs::deferred)
          .max_deductions(10'000)
          .large_collapse(3'000);
      std::cout << tc.congruence().settings_string();
      REQUIRE(tc.size() == 36'412);
      std::cout << tc.congruence().stats_string();
    }

    TEST_CASE("Walker 5", "[quick][Walker5][paper]") {
      using options = libsemigroups::congruence::ToddCoxeter::options;
      auto rg       = ReportGuard(false);
      BENCHMARK("HLT") {
        ToddCoxeter tc;
        walker5(tc);
        tc.congruence().next_lookahead(5'000'000).strategy(
            options::strategy::hlt);
        // REQUIRE(tc.congruence().number_of_generators() == 2);
        // REQUIRE(tc.congruence().number_of_generating_pairs() == 3);
        // REQUIRE(tc.congruence().length_of_generating_pairs() == 73);
        REQUIRE(tc.size() == 72'822);
      };
    }

    TEST_CASE("Walker 5 - Felsch only", "[paper][Walker5][felsch]") {
      auto        rg = ReportGuard(true);
      ToddCoxeter tc;
      walker5(tc);
      tc.congruence().simplify(3);
      tc.congruence()
          .strategy(options::strategy::felsch)
          .max_deductions(POSITIVE_INFINITY)
          .preferred_defs(options::preferred_defs::none);
      REQUIRE(std::vector<word_type>(tc.congruence().cbegin_relations(),
                                     tc.congruence().cend_relations())
              == std::vector<word_type>(
                  {{0, 0, 0},
                   {0},
                   {3, 1, 1},
                   {1},
                   {0, 1, 2, 2, 2, 2, 2, 2, 2, 1, 0, 3, 1, 0, 0},
                   {1, 1},
                   {2},
                   {0, 3, 0, 1},
                   {3},
                   {1, 1, 1, 1}}));
      std::cout << tc.congruence().settings_string();
      REQUIRE(tc.size() == 72'822);
      std::cout << tc.congruence().stats_string();
    }

    TEST_CASE("Walker 6", "[quick][Walker6][paper]") {
      using options = libsemigroups::congruence::ToddCoxeter::options;
      auto rg       = ReportGuard(false);
      BENCHMARK("HLT") {
        ToddCoxeter tc;
        walker6(tc);
        tc.congruence().simplify(1);
        tc.congruence()
            .sort_generating_pairs()
            .remove_duplicate_generating_pairs();
        tc.congruence().strategy(options::strategy::hlt);
        REQUIRE(tc.size() == 78'722);
      };
      BENCHMARK("Felsch") {
        ToddCoxeter tc;
        walker6(tc);
        tc.congruence().simplify(10);
        tc.congruence()
            .sort_generating_pairs()
            .remove_duplicate_generating_pairs()
            .use_relations_in_extra(true);
        tc.congruence().strategy(options::strategy::felsch);
        REQUIRE(tc.size() == 78'722);
      };
    }

    TEST_CASE("Walker 7", "[quick][Walker7][paper]") {
      using options = libsemigroups::congruence::ToddCoxeter::options;
      auto rg       = ReportGuard(false);
      BENCHMARK("HLT") {
        ToddCoxeter tc;
        walker7(tc);
        tc.congruence().strategy(options::strategy::hlt);
        REQUIRE(tc.size() == 153'500);
      };
      BENCHMARK("Felsch") {
        ToddCoxeter tc;
        walker7(tc);
        tc.congruence().simplify(10);
        tc.congruence()
            .strategy(options::strategy::felsch)
            .deduction_policy(options::deductions::no_stack_if_no_space
                              | options::deductions::v1)
            .preferred_defs(options::preferred_defs::none);
        REQUIRE(tc.size() == 153'500);
      };
    }

    TEST_CASE("Walker 8", "[quick][Walker8][paper]") {
      using options = libsemigroups::congruence::ToddCoxeter::options;
      auto rg       = ReportGuard(false);
      BENCHMARK("HLT") {
        ToddCoxeter tc;
        walker8(tc);
        tc.congruence().next_lookahead(500'000);
        tc.congruence().strategy(options::strategy::hlt);
        REQUIRE(tc.size() == 270'272);
      };
    }

    TEST_CASE("Walker 8 - Felsch only", "[paper][Walker8][felsch]") {
      auto        rg = ReportGuard(true);
      ToddCoxeter tc;
      walker5(tc);
      tc.congruence().simplify(10);
      tc.congruence().strategy(options::strategy::felsch);
      std::cout << tc.congruence().settings_string();
      REQUIRE(tc.size() == 270'272);
      std::cout << tc.congruence().stats_string();
    }
  }  // namespace fpsemigroup
}  // namespace libsemigroups
