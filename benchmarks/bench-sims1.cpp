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

  namespace {
    void bench_length(Presentation<word_type>& p,
                      size_t                   max_classes = 16,
                      size_t                   expected    = 134) {
      auto rg = ReportGuard(false);
      xml_tag("XLabel", "Length");

      std::vector<std::pair<Presentation<word_type>, size_t>> ps;
      ps.emplace_back(p, presentation::length(p));

      auto it  = knuth_bendix::redundant_rule(p, std::chrono::milliseconds(10));
      size_t i = 0;

      while (it != p.rules.cend()) {
        i++;
        p.rules.erase(it, it + 2);
        it = knuth_bendix::redundant_rule(p, std::chrono::milliseconds(10));
        if (i % 5 == 0) {
          ps.emplace_back(p, presentation::length(p));
        }
      }

      for (auto& q : ps) {
        // FIXME something bad goes wrong here if long_rule_length is called
        // repeatedly, same for short_rules.
        Sims1_ C(congruence_kind::left);
        C.short_rules(q.first).number_of_threads(1);
        BENCHMARK(fmt::format("{}", q.second)) {
          REQUIRE(C.number_of_congruences(max_classes) == expected);
        };
      }
    }

  }  // namespace

  TEST_CASE("Presentation length Iwahori T_n",
            "[full_transf_monoid][length][000]") {
    auto p = full_transformation_monoid(4, fpsemigroup::author::Iwahori);
    bench_length(p);
  }

  // Doesn't run, or is so slow that it's useless
  TEST_CASE("Presentation length Aizenstat",
            "[full_transf_monoid][length][001]") {
    auto   p = full_transformation_monoid(4, fpsemigroup::author::Aizenstat);
    Sims1_ C(congruence_kind::left);
    C.short_rules(p).number_of_threads(1);
    REQUIRE(presentation::length(p) == 0);
    BENCHMARK(fmt::format("{}", presentation::length(p))) {
      REQUIRE(C.number_of_congruences(16) == 134);
    };
  }

  TEST_CASE("Presentation length machine",
            "[full_transf_monoid][length][002]") {
    auto                   rg = ReportGuard(false);
    FroidurePin<Transf<4>> S;
    S.add_generator(Transf<4>::make({1, 2, 3, 0}));
    S.add_generator(Transf<4>::make({1, 0, 2, 3}));
    S.add_generator(Transf<4>::make({0, 1, 2, 0}));
    REQUIRE(S.size() == 256);
    auto p = to_presentation<word_type>(S);
    bench_length(p);
  }

  TEST_CASE("Presentation length Burnside+Miller S_n",
            "[symmetric_group][length][003]") {
    auto p = symmetric_group(
        5, fpsemigroup::author::Burnside + fpsemigroup::author::Miller);
    bench_length(p, 120, 156);
  }

  TEST_CASE("Presentation length Fernandes cyclic inverse monoid 1st",
            "[cyclic_inverse][length][000]") {
    size_t n = 10;
    auto   p = cyclic_inverse_monoid(n, fpsemigroup::author::Fernandes, 0);
    bench_length(p, 4, 6);
  }

  TEST_CASE("Presentation length Fernandes cyclic inverse monoid 2nd",
            "[cyclic_inverse][length][001]") {
    size_t n = 10;
    auto   p = cyclic_inverse_monoid(n, fpsemigroup::author::Fernandes, 1);
    bench_length(p, 4, 6);
  }

  TEST_CASE("Presentation length machine cyclic inverse monoid 1st",
            "[cyclic_inverse][length][002]") {
    auto rg = ReportGuard(false);

    FroidurePin<PPerm<10>> S;
    S.add_generator(PPerm<10>::make({1, 2, 3, 4, 5, 6, 7, 8, 9, 0}));
    S.add_generator(PPerm<10>::make(
        {1, 2, 3, 4, 5, 6, 7, 8, 9}, {1, 2, 3, 4, 5, 6, 7, 8, 9}, 10));
    S.add_generator(PPerm<10>::make(
        {0, 2, 3, 4, 5, 6, 7, 8, 9}, {0, 2, 3, 4, 5, 6, 7, 8, 9}, 10));
    S.add_generator(PPerm<10>::make(
        {0, 1, 3, 4, 5, 6, 7, 8, 9}, {0, 1, 3, 4, 5, 6, 7, 8, 9}, 10));
    S.add_generator(PPerm<10>::make(
        {0, 1, 2, 4, 5, 6, 7, 8, 9}, {0, 1, 2, 4, 5, 6, 7, 8, 9}, 10));
    S.add_generator(PPerm<10>::make(
        {0, 1, 2, 3, 5, 6, 7, 8, 9}, {0, 1, 2, 3, 5, 6, 7, 8, 9}, 10));
    S.add_generator(PPerm<10>::make(
        {0, 1, 2, 3, 4, 6, 7, 8, 9}, {0, 1, 2, 3, 4, 6, 7, 8, 9}, 10));
    S.add_generator(PPerm<10>::make(
        {0, 1, 2, 3, 4, 5, 7, 8, 9}, {0, 1, 2, 3, 4, 5, 7, 8, 9}, 10));
    S.add_generator(PPerm<10>::make(
        {0, 1, 2, 3, 4, 5, 6, 8, 9}, {0, 1, 2, 3, 4, 5, 6, 8, 9}, 10));
    S.add_generator(PPerm<10>::make(
        {0, 1, 2, 3, 4, 5, 6, 7, 9}, {0, 1, 2, 3, 4, 5, 6, 7, 9}, 10));
    S.add_generator(PPerm<10>::make(
        {0, 1, 2, 3, 4, 5, 6, 7, 8}, {0, 1, 2, 3, 4, 5, 6, 7, 8}, 10));

    size_t n = 10;
    REQUIRE(S.size() == n * std::pow(2, n) - n + 1);
    auto p = to_presentation<word_type>(S);
    bench_length(p, 4, 6);
  }

  TEST_CASE("Presentation length machine cyclic inverse monoid 2nd",
            "[cyclic_inverse][length][003]") {
    auto rg = ReportGuard(false);

    FroidurePin<PPerm<10>> S;
    S.add_generator(PPerm<10>::make({1, 2, 3, 4, 5, 6, 7, 8, 9, 0}));
    S.add_generator(PPerm<10>::make(
        {1, 2, 3, 4, 5, 6, 7, 8, 9}, {1, 2, 3, 4, 5, 6, 7, 8, 9}, 10));

    size_t n = 10;
    REQUIRE(S.size() == n * std::pow(2, n) - n + 1);
    auto p = to_presentation<word_type>(S);
    bench_length(p, 4, 6);
  }

  // TEST_CASE("Presentation length S_5 (GAP presentation)",
  //           "[symmetric_group][length][004]") {
  //   using presentation::pow;
  //   Presentation<std::string> p;
  //   p.alphabet("abcd");
  //   p.contains_empty_word(true);
  //   presentation::add_rule(p, "aa", "");
  //   presentation::add_rule(p, "bb", "");
  //   presentation::add_rule(p, "cc", "");
  //   presentation::add_rule(p, "dd", "");
  //   presentation::add_rule(p, pow("ab", 3), "");
  //   presentation::add_rule(p, pow("ac", 2), "");
  //   presentation::add_rule(p, pow("ad", 2), "");
  //   presentation::add_rule(p, pow("bc", 3), "");
  //   presentation::add_rule(p, pow("bd", 2), "");
  //   presentation::add_rule(p, pow("cd", 3), "");
  //   auto pp = to_presentation<word_type>(p);
  //   bench_length(pp, 120, 156);
  // }

  // // Too slow
  // TEST_CASE("Presentation length S_6 (GAP presentation)",
  //           "[symmetric_group][length][005]") {
  //   auto rg = ReportGuard(true);

  //   using presentation::pow;
  //   Presentation<std::string> p;
  //   p.alphabet("abcde");
  //   p.contains_empty_word(true);
  //   presentation::add_rule(p, "aa", "");
  //   presentation::add_rule(p, "bb", "");
  //   presentation::add_rule(p, "cc", "");
  //   presentation::add_rule(p, "dd", "");
  //   presentation::add_rule(p, "ee", "");
  //   presentation::add_rule(p, pow("ab", 3), "");
  //   presentation::add_rule(p, pow("ac", 2), "");
  //   presentation::add_rule(p, pow("ad", 2), "");
  //   presentation::add_rule(p, pow("ae", 2), "");
  //   presentation::add_rule(p, pow("bc", 3), "");
  //   presentation::add_rule(p, pow("bd", 2), "");
  //   presentation::add_rule(p, pow("be", 2), "");
  //   presentation::add_rule(p, pow("cd", 3), "");
  //   presentation::add_rule(p, pow("ce", 2), "");
  //   presentation::add_rule(p, pow("de", 3), "");
  //   auto pp = to_presentation<word_type>(p);
  //   bench_length(pp, 720, 1'455);
  // }

  TEST_CASE("(2, 3, 7)-triangle group - index 50", "[triangle]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.contains_empty_word(true);
    p.alphabet("xy");
    presentation::add_rule_and_check(p, "xx", "");
    presentation::add_rule_and_check(p, "yyy", "");
    presentation::add_rule_and_check(p, "xyxyxyxyxyxyxy", "");
    Sims1_ S(congruence_kind::right);
    S.short_rules(p);
    BENCHMARK("1 thread") {
      REQUIRE(S.number_of_threads(1).number_of_congruences(50) == 75'971);
    };
    BENCHMARK("2 threads") {
      REQUIRE(S.number_of_threads(2).number_of_congruences(50) == 75'971);
    };
    BENCHMARK("4 threads") {
      REQUIRE(S.number_of_threads(4).number_of_congruences(50) == 75'971);
    };
  }

  TEST_CASE("Heineken group - index 10", "[heineken]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.contains_empty_word(true);
    p.alphabet("xXyY");
    presentation::add_inverse_rules(p, "XxYy");
    presentation::add_rule_and_check(p, "yXYYxyYYxyyXYYxyyXyXYYxy", "x");
    presentation::add_rule_and_check(
        p, "YxyyXXYYxyxYxyyXYXyXYYxxyyXYXyXYYxyx", "y");

    Sims1_ S(congruence_kind::right);
    S.short_rules(p);
    BENCHMARK("1 thread") {
      REQUIRE(S.number_of_threads(1).number_of_congruences(10) == 1);
    };
    BENCHMARK("2 threads") {
      REQUIRE(S.number_of_threads(2).number_of_congruences(10) == 1);
    };
    BENCHMARK("4 threads") {
      REQUIRE(S.number_of_threads(4).number_of_congruences(10) == 1);
    };
    REQUIRE(S.number_of_congruences(10) == 1);
  }

  TEST_CASE("Catalan monoid n = 1 - all", "[catalan][n=1]") {
    auto                   rg = ReportGuard(false);
    FroidurePin<Transf<1>> S;
    S.add_generator(Transf<1>::make({0}));
    REQUIRE(S.size() == 1);
    auto p = to_presentation<word_type>(S);

    Sims1_ C(congruence_kind::right);
    C.short_rules(p);
    BENCHMARK("1 thread") {
      REQUIRE(C.number_of_threads(1).number_of_congruences(1) == 1);
    };
  }

  TEST_CASE("Catalan monoid n = 2 - all", "[catalan][n=2]") {
    auto                   rg = ReportGuard(false);
    FroidurePin<Transf<2>> S;
    S.add_generator(Transf<2>::make({0, 1}));
    S.add_generator(Transf<2>::make({0, 0}));
    REQUIRE(S.size() == 2);
    auto p = to_presentation<word_type>(S);

    Sims1_ C(congruence_kind::right);
    C.short_rules(p);
    BENCHMARK("1 thread") {
      REQUIRE(C.number_of_threads(1).number_of_congruences(S.size()) == 2);
    };
  }

  TEST_CASE("Catalan monoid n = 3 - all", "[catalan][n=3]") {
    auto                   rg = ReportGuard(false);
    FroidurePin<Transf<3>> S;
    S.add_generator(Transf<3>::make({0, 1, 2}));
    S.add_generator(Transf<3>::make({0, 0, 2}));
    S.add_generator(Transf<3>::make({0, 1, 1}));
    REQUIRE(S.size() == 5);
    auto p = to_presentation<word_type>(S);

    Sims1_ C(congruence_kind::right);
    C.short_rules(p);
    BENCHMARK("1 thread") {
      REQUIRE(C.number_of_threads(1).number_of_congruences(S.size()) == 11);
    };
  }

  TEST_CASE("Catalan monoid n = 4 - all", "[catalan][n=4]") {
    auto                   rg = ReportGuard(false);
    FroidurePin<Transf<4>> S;
    S.add_generator(Transf<4>::make({0, 1, 2, 3}));
    S.add_generator(Transf<4>::make({0, 0, 2, 3}));
    S.add_generator(Transf<4>::make({0, 1, 1, 3}));
    S.add_generator(Transf<4>::make({0, 1, 2, 2}));
    REQUIRE(S.size() == 14);
    auto p = to_presentation<word_type>(S);

    Sims1_ C(congruence_kind::right);
    C.short_rules(p);
    BENCHMARK("1 thread") {
      REQUIRE(C.number_of_threads(1).number_of_congruences(S.size()) == 575);
    };
  }

  TEST_CASE("Catalan monoid n = 5 - all", "[catalan][n=5]") {
    auto                   rg = ReportGuard(false);
    FroidurePin<Transf<5>> S;
    S.add_generator(Transf<5>::make({0, 1, 2, 3, 4}));
    S.add_generator(Transf<5>::make({0, 0, 2, 3, 4}));
    S.add_generator(Transf<5>::make({0, 1, 1, 3, 4}));
    S.add_generator(Transf<5>::make({0, 1, 2, 2, 4}));
    S.add_generator(Transf<5>::make({0, 1, 2, 3, 3}));
    REQUIRE(S.size() == 42);
    auto p = to_presentation<word_type>(S);

    Sims1_ C(congruence_kind::right);
    C.short_rules(p);
    BENCHMARK("1 thread") {
      REQUIRE(C.number_of_threads(1).number_of_congruences(S.size())
              == 5'295'135);
    };
  }

  TEST_CASE("Heineken monoid", "[heineken][001]") {
    Presentation<std::string> p;
    p.contains_empty_word(true);
    p.alphabet("xyXY");
    presentation::add_rule_and_check(p, "yXYYxyYYxyyXYYxyyXyXYYxyX", "");
    presentation::add_rule_and_check(
        p, "YxyyXXYYxyxYxyyXYXyXYYxxyyXYXyXYYxyxY", "");
    Sims1_ S(congruence_kind::right);
    S.short_rules(p);
    REQUIRE(S.number_of_threads(4).number_of_congruences(2) == 4);
  }

  TEST_CASE("Order endomorphisms n = 2 - all", "[order_endos][n=2]") {
    auto                   rg = ReportGuard(false);
    FroidurePin<Transf<2>> S;
    S.add_generator(Transf<2>::make({0, 1}));
    S.add_generator(Transf<2>::make({0, 0}));
    S.add_generator(Transf<2>::make({1, 1}));
    REQUIRE(S.size() == 3);

    Sims1_ C(congruence_kind::right);
    C.short_rules(to_presentation<word_type>(S));
    BENCHMARK("1 thread") {
      REQUIRE(C.number_of_threads(1).number_of_congruences(3) == 5);
    };
  }

  TEST_CASE("Order endomorphisms n = 3 - all", "[order_endos][n=3]") {
    auto rg = ReportGuard(false);
    auto p  = fpsemigroup::order_preserving_monoid(3);

    Sims1_ C(congruence_kind::right);
    C.short_rules(p);
    BENCHMARK("1 thread") {
      REQUIRE(C.number_of_threads(1).number_of_congruences(10) == 25);
    };
  }

  TEST_CASE("Order endomorphisms n = 4 - all", "[order_endos][n=4]") {
    auto rg = ReportGuard(false);
    auto p  = fpsemigroup::order_preserving_monoid(4);

    Sims1_ C(congruence_kind::right);
    C.short_rules(p);
    BENCHMARK("1 thread") {
      REQUIRE(C.number_of_threads(1).number_of_congruences(35) == 385);
    };
  }

  TEST_CASE("Order endomorphisms n = 5 - all", "[order_endos][n=5]") {
    auto rg = ReportGuard(false);
    auto p  = fpsemigroup::order_preserving_monoid(5);
    presentation::sort_each_rule(p);
    presentation::sort_rules(p);
    presentation::remove_duplicate_rules(p);
    presentation::reduce_complements(p);
    presentation::remove_trivial_rules(p);

    Sims1_ C(congruence_kind::right);
    C.short_rules(p);
    BENCHMARK("1 thread") {
      REQUIRE(C.number_of_threads(1).number_of_congruences(126) == 37'951);
    };
  }

}  // namespace libsemigroups
