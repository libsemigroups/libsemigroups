//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2022 James D. Mitchell
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

#include "libsemigroups/bmat8.hpp"
#include "libsemigroups/cong-intf.hpp"      // for congruence_kind, congruenc...
#include "libsemigroups/constants.hpp"      // for PositiveInfinity, POSITIVE...
#include "libsemigroups/detail/report.hpp"  // for ReportGuard
#include "libsemigroups/fastest-bmat.hpp"
#include "libsemigroups/fpsemi-examples.hpp"  // for singular_brauer_monoid, ...
#include "libsemigroups/froidure-pin.hpp"     // for FroidurePinBase
#include "libsemigroups/sims1.hpp"            // for ReportGuard
#include "libsemigroups/to-presentation.hpp"  // for to_presentation
#include "libsemigroups/transf.hpp"           // for ReportGuard
#include "libsemigroups/types.hpp"            // for word_type, letter_type

namespace libsemigroups {
  namespace {
    template <typename S, typename T>
    void xml_tag(S name, T val) {
      std::cout << fmt::format("<{0} value=\"{1}\"></{0}>", name, val);
    }
  }  // namespace

  using Sims1_ = Sims1<uint32_t>;

  using fpsemigroup::rook_monoid;
  using fpsemigroup::singular_brauer_monoid;

  TEST_CASE("POI(3) from FroidurePin", "[POI3][Sim1][quick][talk]") {
    auto                  rg = ReportGuard(false);
    FroidurePin<PPerm<3>> S;
    S.add_generator(PPerm<3>::make({0, 1, 2}, {0, 1, 2}, 3));
    S.add_generator(PPerm<3>::make({1, 2}, {0, 1}, 3));
    S.add_generator(PPerm<3>::make({0, 1}, {0, 2}, 3));
    S.add_generator(PPerm<3>::make({0, 2}, {1, 2}, 3));
    S.add_generator(PPerm<3>::make({0, 1}, {1, 2}, 3));
    S.add_generator(PPerm<3>::make({0, 2}, {0, 1}, 3));
    S.add_generator(PPerm<3>::make({1, 2}, {0, 2}, 3));
    REQUIRE(S.size() == 20);

    auto p = to_presentation<word_type>(S);

    BENCHMARK("Right congruences") {
      Sims1_ C(congruence_kind::right);
      C.short_rules(p);
      REQUIRE(C.number_of_congruences(20) == 99);
    };
    BENCHMARK("Left congruences") {
      Sims1_ C(congruence_kind::left);
      C.short_rules(p);
      REQUIRE(C.number_of_congruences(20) == 99);
    };
  }

  TEST_CASE("POI(4) from FroidurePin", "[POI4][Sim1][standard]") {
    auto                  rg = ReportGuard(false);
    FroidurePin<PPerm<4>> S;
    S.add_generator(PPerm<4>::make({0, 1, 2, 3}, {0, 1, 2, 3}, 4));
    S.add_generator(PPerm<4>::make({1, 2, 3}, {0, 1, 2}, 4));
    S.add_generator(PPerm<4>::make({0, 1, 2}, {0, 1, 3}, 4));
    S.add_generator(PPerm<4>::make({0, 1, 3}, {0, 2, 3}, 4));
    S.add_generator(PPerm<4>::make({0, 2, 3}, {1, 2, 3}, 4));
    S.add_generator(PPerm<4>::make({0, 1, 2}, {1, 2, 3}, 4));
    S.add_generator(PPerm<4>::make({0, 1, 3}, {0, 1, 2}, 4));
    S.add_generator(PPerm<4>::make({0, 2, 3}, {0, 1, 3}, 4));
    S.add_generator(PPerm<4>::make({1, 2, 3}, {0, 2, 3}, 4));
    REQUIRE(S.size() == 70);

    auto p = to_presentation<word_type>(S);

    BENCHMARK("Right congruences") {
      Sims1_ C(congruence_kind::right);
      C.short_rules(p);
      REQUIRE(C.number_of_congruences(70) == 8'146);
    };
  }

  TEST_CASE("ReflexiveBooleanMatMonoid(3) from FroidurePin",
            "[ReflexiveBooleanMatMonoid3][Sim1][fail]") {
    auto                        rg = ReportGuard(true);
    FroidurePin<FastestBMat<3>> S;
    S.add_generator(FastestBMat<3>::one(3));
    S.add_generator(FastestBMat<3>({{1, 1, 0}, {0, 1, 1}, {1, 0, 1}}));
    S.add_generator(FastestBMat<3>({{1, 1, 0}, {0, 1, 0}, {0, 0, 1}}));
    S.add_generator(FastestBMat<3>({{1, 0, 1}, {1, 1, 0}, {0, 1, 1}}));
    S.add_generator(FastestBMat<3>({{1, 0, 1}, {0, 1, 0}, {0, 0, 1}}));
    S.add_generator(FastestBMat<3>({{1, 0, 0}, {1, 1, 0}, {0, 0, 1}}));
    S.add_generator(FastestBMat<3>({{1, 0, 0}, {0, 1, 1}, {0, 0, 1}}));
    S.add_generator(FastestBMat<3>({{1, 0, 0}, {0, 1, 0}, {1, 0, 1}}));
    S.add_generator(FastestBMat<3>({{1, 0, 0}, {0, 1, 0}, {0, 1, 1}}));
    REQUIRE(S.size() == 64);

    auto p = to_presentation<word_type>(S);
    BENCHMARK("Right congruences") {
      Sims1_ C(congruence_kind::right);
      C.short_rules(p);
      REQUIRE(C.number_of_congruences(S.size()) == 7);
    };
    BENCHMARK("Left congruences") {
      Sims1_ C(congruence_kind::left);
      C.short_rules(p);
      REQUIRE(C.number_of_congruences(S.size()) == 7);
    };
  }

  TEST_CASE("singular_brauer_monoid(3) (Maltcev-Mazorchuk)",
            "[talk][singular_brauer_monoid3]") {
    auto rg = ReportGuard(false);
    auto p  = to_presentation<word_type>(singular_brauer_monoid(3));
    REQUIRE(p.rules.size() == 48);

    BENCHMARK("Right congruences") {
      Sims1_ C(congruence_kind::right);
      C.short_rules(p);
      REQUIRE(C.number_of_congruences(9) == 205);
    };
    BENCHMARK("Left congruences") {
      Sims1_ C(congruence_kind::left);
      C.short_rules(p);
      REQUIRE(C.number_of_congruences(9) == 205);
    };
  }

  TEST_CASE("singular_brauer_monoid(4) (Maltcev-Mazorchuk)",
            "[talk][singular_brauer_monoid4]") {
    auto rg = ReportGuard(true);
    auto p  = to_presentation<word_type>(singular_brauer_monoid(4));
    REQUIRE(presentation::length(p) == 660);
    presentation::remove_duplicate_rules(p);
    REQUIRE(presentation::length(p) == 600);
    presentation::sort_each_rule(p);
    presentation::sort_rules(p);
    REQUIRE(p.rules.size() == 252);
    // presentation::remove_redundant(p);

    BENCHMARK("Right congruences") {
      Sims1_ C(congruence_kind::right);
      C.short_rules(p);
      C.split_at((252 - 64) / 2);
      REQUIRE(C.number_of_congruences(81) == 601'265);
    };
  }

  TEST_CASE("symmetric_inverse_monoid(2) (Hivert)",
            "[talk][symmetric_inverse_monoid2]") {
    auto rg = ReportGuard(true);
    auto p  = to_presentation<word_type>(rook_monoid(2, 1));
    presentation::remove_duplicate_rules(p);
    presentation::sort_each_rule(p);
    presentation::sort_rules(p);

    BENCHMARK("Right congruences") {
      Sims1_ C(congruence_kind::right);
      C.short_rules(p);
      REQUIRE(C.number_of_congruences(7) == 10);
    };
  }

  TEST_CASE("symmetric_inverse_monoid(3) (Hivert)",
            "[talk][symmetric_inverse_monoid3]") {
    auto rg = ReportGuard(true);
    auto p  = to_presentation<word_type>(rook_monoid(3, 1));
    presentation::remove_duplicate_rules(p);
    presentation::sort_each_rule(p);
    presentation::sort_rules(p);

    BENCHMARK("Right congruences") {
      Sims1_ C(congruence_kind::right);
      C.short_rules(p);
      REQUIRE(C.number_of_congruences(34) == 274);
    };
  }

  TEST_CASE("symmetric_inverse_monoid(4) (Hivert)",
            "[talk][symmetric_inverse_monoid4]") {
    auto rg = ReportGuard(false);
    auto p  = to_presentation<word_type>(rook_monoid(4, 1));
    presentation::remove_duplicate_rules(p);
    presentation::sort_each_rule(p);
    presentation::sort_rules(p);

    BENCHMARK("Right congruences") {
      Sims1_ C(congruence_kind::right);
      C.short_rules(p);
      REQUIRE(C.number_of_threads(std::thread::hardware_concurrency())
                  .number_of_congruences(209)
              == 195'709);
    };
  }

  namespace {

    std::vector<std::pair<Presentation<word_type>, size_t>>
    generate_random_sample(size_t sample_size,
                           size_t num_letters,
                           size_t word_len) {
      std::vector<std::pair<Presentation<word_type>, size_t>> sample;
      sample.reserve(sample_size);

      Presentation<word_type> p;
      p.alphabet(num_letters);

      for (size_t i = 0; i < sample_size; ++i) {
        p.rules = {random_word(word_len, num_letters),
                   random_word(word_len, num_letters)};
        sample.emplace_back(p, i);
      }

      std::sort(sample.begin(), sample.end(), [](auto const& x, auto const& y) {
        return x.second < y.second;
      });
      return sample;
    }

    std::vector<std::pair<Presentation<word_type>, size_t>>
    generate_sample(size_t num_letters, size_t word_len, size_t num_classes) {
      std::vector<std::pair<Presentation<word_type>, size_t>> sample;

      Presentation<word_type> p;
      p.alphabet(num_letters);
      Sims1_ C(congruence_kind::right);

      Words lhs;
      lhs.letters(num_letters).min(1).max(word_len);
      Words rhs;
      rhs.letters(num_letters);

      for (auto const& l : lhs) {
        rhs.first(l).max(word_len);
        for (auto const& r : rhs | rx::skip_n(1)) {
          p.rules = {l, r};
          C.short_rules(p);
          size_t const m
              = C.number_of_threads(1).number_of_congruences(num_classes);
          sample.emplace_back(p, m);
        }
      }

      std::sort(sample.begin(), sample.end(), [](auto const& x, auto const& y) {
        return x.second < y.second;
      });
      return sample;
    }

    void bench_parallel(
        std::vector<std::pair<Presentation<word_type>, size_t>> const& sample,
        size_t num_threads,
        size_t num_classes) {
      xml_tag(
          "Title",
          fmt::format("Algorithm 4 for {} randomly chosen presentations "
                      "with $|A| = {}$, "
                      "$|R| = {}$, $|\\langle A\\mid R \\rangle| = {}$, and "
                      "number of classes = {}",
                      sample.size(),
                      sample[0].first.alphabet().size(),
                      sample[0].first.rules.size() / 2,
                      presentation::length(sample[0].first),
                      num_classes));
      xml_tag("XLabel", "Test case");

      Sims1_   C(congruence_kind::right);
      uint64_t num_congs = 0;
      for (auto const& s : sample) {
        C.short_rules(s.first);
        num_congs += C.number_of_threads(num_threads)
                         .number_of_congruences(num_classes);
        BENCHMARK(fmt::format("{}", s.second)) {
          C.short_rules(s.first);
          C.number_of_threads(num_threads).number_of_congruences(num_classes);
        };
      }
      std::cout << "Mean number of congruences per monoid "
                << num_congs / sample.size() << std::endl;
    }

  }  // namespace

  TEST_CASE("Parallel version", "[parallel]") {
    auto rg = ReportGuard(false);

    size_t const sample_size = 128;
    size_t const num_letters = 2;
    size_t const word_len    = 10;
    size_t const num_classes = 5;

    auto sample = generate_random_sample(sample_size, num_letters, word_len);

    for (size_t i = 1; i <= std::thread::hardware_concurrency(); i *= 2) {
      bench_parallel(sample, i, num_classes);
    }
  }

  TEST_CASE("Presentation length", "[parallel]") {
    auto rg = ReportGuard(false);

    size_t const sample_size = 128;
    size_t const num_letters = 2;
    size_t const word_len    = 10;
    size_t const num_classes = 5;

    auto sample = generate_random_sample(sample_size, num_letters, word_len);

    for (size_t i = 1; i <= std::thread::hardware_concurrency(); i *= 2) {
      bench_parallel(sample, i, num_classes);
    }
  }

}  // namespace libsemigroups
