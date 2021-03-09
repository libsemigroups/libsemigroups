//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2021 James D. Mitchell + Maria Tsalakou
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

#include <cstddef>   // for size_t
#include <iostream>  // for to_string
#include <string>    // for to_string
#include <unordered_set>

#include "bench-main.hpp"  // for CATCH_CONFIG_ENABLE_BENCHMARKING
#include "catch.hpp"       // for TEST_CASE, BENCHMARK, REQUIRE

#include "libsemigroups/kambites.hpp"      // for Kambites
#include "libsemigroups/knuth-bendix.hpp"  // for KnuthBendix
#include "libsemigroups/knuth-bendix.hpp"  // for random_string
#include "libsemigroups/siso.hpp"          // for cbegin_sislo

// TODO:
// - include number of recursive calls to wp-prefix.

namespace libsemigroups {
  using detail::MultiStringView;
  using detail::power_string;
  using detail::random_string;
  using detail::random_strings;
  using fpsemigroup::Kambites;

  namespace {
    std::string zip(std::vector<std::string> const& x,
                    std::vector<std::string> const& y) {
      LIBSEMIGROUPS_ASSERT(x.size() == y.size());
      std::string result = "";
      for (size_t i = 0; i < x.size(); ++i) {
        result += x[i];
        result += y[i];
      }
      return result;
    }

    // Returns {u_1, u_2, ..., u_{exp}} where u_i are chosen with uniform
    // distribution in {s, t}
    std::vector<std::string> random_sequence(std::string const& s,
                                             std::string const& t,
                                             size_t             exp) {
      static std::random_device              rd;
      static std::mt19937                    generator(rd());
      static std::uniform_int_distribution<> distribution(0, 1);
      std::vector<std::string>               result;
      while (exp > 0) {
        switch (distribution(generator)) {
          case 0: {
            result.push_back(s);
            break;
          }
          case 1: {
            result.push_back(t);
            break;
          }
        }
        exp--;
      }
      return result;
    }

  }  // namespace

  ////////////////////////////////////////////////////////////////////////
  // Benchmark checking C(4) or higher - Example A.1
  ////////////////////////////////////////////////////////////////////////

  std::pair<std::string, std::string> example1(size_t N) {
    std::string lhs, rhs;
    for (size_t b = 1; b <= N; ++b) {
      lhs += "a" + std::string(b, 'b');
    }
    for (size_t b = N + 1; b <= 2 * N; ++b) {
      rhs += "a" + std::string(b, 'b');
    }
    return std::make_pair(lhs, rhs);
  }

  template <typename T>
  void c4_ex_A1() {
    for (size_t N = 100; N <= 1000; N += 25) {
      size_t M;
      {
        std::string lhs, rhs;
        std::tie(lhs, rhs) = example1(N);
        M                  = lhs.size() + rhs.size();
      }
      BENCHMARK_ADVANCED("Length of relation words = " + std::to_string(M))
      (Catch::Benchmark::Chronometer meter) {
        std::string lhs, rhs;
        std::tie(lhs, rhs) = example1(N);
        Kambites<T> k;
        k.set_alphabet("ab");
        k.add_rule(lhs, rhs);
        meter.measure([&k]() { REQUIRE(k.small_overlap_class() >= 4); });
      };  // NOLINT(readability/braces)
    }
  }

  TEST_CASE("Example A1: determining C(4) (std::string)", "[quick][000]", ) {
    c4_ex_A1<std::string>();
  }

  TEST_CASE("Example A1: determining C(4) (MultiStringView)",
            "[quick][001]", ) {
    c4_ex_A1<MultiStringView>();
  }

  TEST_CASE("Example A1: KnuthBendix", "[quick][002]", ) {
    auto rg = ReportGuard(false);
    for (size_t N = 100; N <= 1000; N += 25) {
      size_t const M = N * (2 * N + 3);
      std::string  lhs, rhs;
      std::tie(lhs, rhs) = example1(N);
      BENCHMARK("Length of relation words = " + std::to_string(M)) {
        fpsemigroup::KnuthBendix k;
        k.set_alphabet("ab");
        k.add_rule(lhs, rhs);
        k.run();
        return k.confluent();
      };  // NOLINT(readability/braces)
    }
  }

  ////////////////////////////////////////////////////////////////////////
  // Benchmark checking C(4) or higher - Example A.2
  ////////////////////////////////////////////////////////////////////////

  // <a, b, c | a(bc) ^ k a = a (cb) ^ l a>

  template <typename T>
  void c4_ex_A2() {
    for (size_t m = 5000; m < 500000; m += 20000) {
      BENCHMARK_ADVANCED("Length of relation words = "
                         + std::to_string(4 * m + 4))
      (Catch::Benchmark::Chronometer meter) {
        std::string lhs = "a" + power_string("bc", m) + "a";
        std::string rhs = "a" + power_string("cb", m) + "a";
        meter.measure([&lhs, &rhs]() {
          Kambites<T> k;
          k.set_alphabet("abc");
          k.add_rule(lhs, rhs);
          REQUIRE(k.small_overlap_class() >= 4);
        });
      };  // NOLINT(readability/braces)
    }
  }

  TEST_CASE("Example A2: determining C(4) (std::string)", "[quick][003]", ) {
    c4_ex_A2<std::string>();
  }

  TEST_CASE("Example A2: determining C(4) (MultiStringView)",
            "[quick][004]", ) {
    c4_ex_A2<MultiStringView>();
  }

  TEST_CASE("Example A2: KnuthBendix", "[quick][005]", ) {
    auto rg = ReportGuard(false);
    for (size_t m = 5000; m < 500000; m += 20000) {
      std::string lhs = "a" + power_string("bc", m) + "a";
      std::string rhs = "a" + power_string("cb", m) + "a";
      BENCHMARK("Length of relation words = " + std::to_string(4 * m + 4)) {
        fpsemigroup::KnuthBendix k;
        k.set_alphabet("abc");
        k.add_rule(lhs, rhs);
        k.run();
        return k.confluent();
      };
    }
  }

  ////////////////////////////////////////////////////////////////////////
  // Benchmark checking small overlap condition - Example A.3
  ////////////////////////////////////////////////////////////////////////

  template <typename T>
  void c4_ex_A3(size_t first, size_t last, size_t step) {
    auto   rg                                  = ReportGuard(false);
    size_t number_c4                           = 0;
    size_t number_c123                         = 0;
    size_t number_confluent                    = 0;
    size_t number_confluent_after_1_second     = 0;
    size_t number_not_confluent_after_1_second = 0;

    for (size_t m = first; m < last; m += step) {
      auto   rules  = random_strings(std::string("abcdefgh"), 8, 0, m);
      size_t length = std::accumulate(
          rules.cbegin(),
          rules.cend(),
          size_t(0),
          [](size_t val, std::string const& s) { return val + s.size(); });
      bool counted = false;  // each benchmark is invoked multiple times, but we
                             // only want to count once!
      BENCHMARK("(Kambites) length = " + std::to_string(length)) {
        Kambites<T> k;
        k.set_alphabet("abcdefgh");
        k.add_rule(rules[0], rules[1]);
        k.add_rule(rules[2], rules[3]);
        k.add_rule(rules[4], rules[5]);
        k.add_rule(rules[6], rules[7]);
        if (k.small_overlap_class() >= 4 && !counted) {
          number_c4++;
        } else if (!counted) {
          number_c123++;
        }
        counted = true;
        return k.small_overlap_class();
      };
      fpsemigroup::KnuthBendix k;
      k.set_alphabet("abcdefgh");
      k.add_rule(rules[0], rules[1]);
      k.add_rule(rules[2], rules[3]);
      k.add_rule(rules[4], rules[5]);
      k.add_rule(rules[6], rules[7]);
      if (k.confluent()) {
        std::cout << std::endl << "Presentation is confluent!" << std::endl;
        number_confluent++;
        BENCHMARK("(KnuthBendix) length = " + std::to_string(length)) {
          fpsemigroup::KnuthBendix k;
          k.set_alphabet("abcdefgh");
          k.add_rule(rules[0], rules[1]);
          k.add_rule(rules[2], rules[3]);
          k.add_rule(rules[4], rules[5]);
          k.add_rule(rules[6], rules[7]);
          return k.confluent();
        };
      } else {
        k.run_for(std::chrono::seconds(1));
        if (k.confluent()) {
          std::cout << std::endl
                    << "Presentation is confluent (after running KnuthBendix "
                       "for ~1 second)!"
                    << std::endl;
          number_confluent_after_1_second++;
          BENCHMARK("(KnuthBendix) length = " + std::to_string(length)) {
            fpsemigroup::KnuthBendix k;
            k.set_alphabet("abcdefgh");
            k.add_rule(rules[0], rules[1]);
            k.add_rule(rules[2], rules[3]);
            k.add_rule(rules[4], rules[5]);
            k.add_rule(rules[6], rules[7]);
            k.run_for(std::chrono::seconds(1));
            return k.confluent();
          };
        } else {
          std::cout
              << std::endl
              << "Presentation is not confluent (after running KnuthBendix "
                 "for ~1 second)!"
              << std::endl;
          number_not_confluent_after_1_second++;
        }
      }
    }
    std::cout << "\nNumber of C(4) presentations = " << number_c4 << std::endl;
    std::cout << "Number of C(1,2,3) presentations = " << number_c123
              << std::endl;
    std::cout << "Number of confluent presentations = " << number_confluent
              << std::endl;
    std::cout << "Number confluent after 1 second of KnuthBendix = "
              << number_confluent_after_1_second << std::endl;
    std::cout << "Number non-confluent after 1 second of KnuthBendix = "
              << number_not_confluent_after_1_second << std::endl;
  }

  TEST_CASE("Example A3: determining C(4) (std::string)", "[quick][006]", ) {
    c4_ex_A3<std::string>(3000, 150000, 3675);
  }

  TEST_CASE("Example A3: determining C(4) (MultiStringView)",
            "[quick][007]", ) {
    c4_ex_A3<std::string>(3000, 150000, 3675);
  }

  ////////////////////////////////////////////////////////////////////////
  // Benchmark checking small overlap condition - Example A.5
  ////////////////////////////////////////////////////////////////////////
  // <a, b, c, d | a(bc) ^ m a = a (cb) ^ m  a,  a(bc) ^ m a = b d ^ m b >

  template <typename T>
  void c4_ex_A5() {
    for (size_t m = 5000; m < 250000; m += 10000) {
      BENCHMARK_ADVANCED("Length of relation words = "
                         + std::to_string(7 * m + 8))
      (Catch::Benchmark::Chronometer meter) {
        std::string lhs1 = "a" + power_string("bc", m) + "a";
        std::string rhs1 = "a" + power_string("cb", m) + "a";
        std::string lhs2 = "b" + power_string("d", m) + "b";
        std::string rhs2 = "a" + power_string("bc", m) + "a";
        meter.measure([&lhs1, &rhs1, &lhs2, &rhs2]() {
          Kambites<T> k;
          k.set_alphabet("abcd");
          k.add_rule(lhs1, rhs1);
          k.add_rule(lhs2, rhs2);
          REQUIRE(k.small_overlap_class() >= 4);
        });
      };  // NOLINT(readability/braces)
    }
  }

  TEST_CASE("Example A5: determining C(4) (std::string)", "[quick][008]", ) {
    c4_ex_A5<std::string>();
  }

  TEST_CASE("Example A5: determining C(4) (MultiStringView)",
            "[quick][009]", ) {
    c4_ex_A5<MultiStringView>();
  }

  TEST_CASE("Example A5: determining C(4) (KnuthBendix)", "[quick][009]", ) {
    for (size_t m = 5000; m < 250000; m += 10000) {
      std::string lhs1 = "a" + power_string("bc", m) + "a";
      std::string rhs1 = "a" + power_string("cb", m) + "a";
      std::string lhs2 = "b" + power_string("d", m) + "b";
      std::string rhs2 = "a" + power_string("bc", m) + "a";
      BENCHMARK_ADVANCED("Length = " + std::to_string(7 * m + 8)) {
        fpsemigroup::KnuthBendix k;
        k.set_alphabet("abcd");
        k.add_rule(lhs1, rhs1);
        k.add_rule(lhs2, rhs2);
        REQUIRE(k.confluent());
        return k.confluent();
      };
    }
  }

  ////////////////////////////////////////////////////////////////////////
  // Benchmark wp-prefix - Example A.1
  ////////////////////////////////////////////////////////////////////////

  template <typename T>
  void equal_to_ex_A1(size_t m) {
    for (size_t N = 100; N <= 400; N += 8) {
      std::string lhs, rhs;
      std::tie(lhs, rhs) = example1(m);
      Kambites<T> k;
      k.set_alphabet("ab");
      k.add_rule(lhs, rhs);
      REQUIRE(k.small_overlap_class() >= 4);
      auto random = random_strings("ab", N, 0, 4 * N + 4);
      auto u      = zip(random_sequence(lhs, rhs, N), random);
      auto v      = zip(random_sequence(lhs, rhs, N), random);
      REQUIRE(k.small_overlap_class() >= 4);
      BENCHMARK("length = " + std::to_string(u.size() + v.size())) {
        REQUIRE(k.equal_to(u, v));
      };
    }
  }

  TEST_CASE("Example A1_10: equal_to (std::string)", "[quick][010]", ) {
    equal_to_ex_A1<std::string>(10);
  }

  TEST_CASE("Example A1_10: equal_to (MultiStringView)", "[quick][011]", ) {
    equal_to_ex_A1<MultiStringView>(10);
  }

  TEST_CASE("Example A1_10: equal_to (KnuthBendix)", "[quick][012]", ) {
    for (size_t N = 100; N <= 400; N += 8) {
      std::string lhs, rhs;
      std::tie(lhs, rhs) = example1(10);
      fpsemigroup::KnuthBendix k;
      k.set_alphabet("ab");
      k.add_rule(lhs, rhs);
      REQUIRE(k.confluent());
      auto random = random_strings("ab", N, 0, 4 * N + 4);
      auto u      = zip(random_sequence(lhs, rhs, N), random);
      auto v      = zip(random_sequence(lhs, rhs, N), random);
      BENCHMARK("length = " + std::to_string(u.size() + v.size())) {
        REQUIRE(k.equal_to(u, v));
      };
    }
  }

  ////////////////////////////////////////////////////////////////////////
  // Benchmark wp-prefix - Example A.2
  ////////////////////////////////////////////////////////////////////////

  // w_0 lhs w_1 lhs w_2 ... w_n = w_0 rhs w_1 rhs w_2 ... w_n
  template <typename T>
  void equal_to_ex_A2(size_t m) {
    for (size_t N = 100; N <= 400; N += 8) {
      std::string lhs = "a" + power_string("bc", m) + "a";
      std::string rhs = "a" + power_string("cb", m) + "a";
      Kambites<T> k;
      k.set_alphabet("abc");
      k.add_rule(lhs, rhs);
      REQUIRE(k.small_overlap_class() >= 4);
      auto random = random_strings("abc", N, 0, 4 * N + 4);
      auto u      = zip(random_sequence(lhs, rhs, N), random);
      auto v      = zip(random_sequence(lhs, rhs, N), random);
      BENCHMARK("length = " + std::to_string(u.size() + v.size())) {
        REQUIRE(k.equal_to(u, v));
      };
    }
  }

  TEST_CASE("Example A2_13: equal_to (std::string)", "[quick][013]", ) {
    equal_to_ex_A2<std::string>(13);
  }

  TEST_CASE("Example A2_13: equal_to (MultiStringView)", "[quick][014]", ) {
    equal_to_ex_A2<MultiStringView>(13);
  }

  ////////////////////////////////////////////////////////////////////////
  // Benchmark wp-prefix - Example A.3
  ////////////////////////////////////////////////////////////////////////

  template <typename T>
  void equal_to_ex_A3(size_t m, size_t first, size_t last, size_t step) {
  start:
    auto        rules = random_strings(std::string("abcdefgh"), 8, 0, m);
    Kambites<T> k;
    k.set_alphabet("abcdefgh");
    k.add_rule(rules[0], rules[1]);
    k.add_rule(rules[2], rules[3]);
    k.add_rule(rules[4], rules[5]);
    k.add_rule(rules[6], rules[7]);
    if (k.small_overlap_class() < 4) {
      goto start;
    }

    for (size_t N = first; N <= last; N += step) {
      auto random = random_strings("abcdefgh", N, 0, 4 * N + 4);
      auto u      = zip(random_sequence(rules[4], rules[5], N), random);
      auto v      = zip(random_sequence(rules[4], rules[5], N), random);
      BENCHMARK("length = " + std::to_string(u.size() + v.size())) {
        REQUIRE(k.equal_to(u, v));
      };
    }
  }

  TEST_CASE("Example A3_300: equal_to (std::string)", "[quick][015]", ) {
    equal_to_ex_A3<std::string>(300, 100, 180, 2);
    // This runs superslow with the same params as the next test
  }

  TEST_CASE("Example A3_300: equal_to (MultiStringView)", "[quick][016]", ) {
    equal_to_ex_A3<MultiStringView>(300, 100, 400, 8);
  }

  ////////////////////////////////////////////////////////////////////////
  // Benchmark wp-prefix - Example A.4
  ////////////////////////////////////////////////////////////////////////

  template <typename T>
  void equal_to_ex_A4() {
    Kambites<T> k;
    k.set_alphabet("abcde");
    k.add_rule("bceac", "aeebbc");
    k.add_rule("aeebbc", "dabcd");
    k.small_overlap_class();
    std::string w1 = "bceac";
    std::string w2 = "dabcd";
    std::string w3 = "aeebbc";

    for (size_t N = 1000; N < 14000; N += 500) {
      auto random = random_strings("abcde", N, 0, 12);
      auto u      = zip(random_sequence(w1, w2, N), random);
      auto v      = zip(random_sequence(w3, w1, N), random);
      BENCHMARK("length = " + std::to_string(u.size() + v.size())) {
        REQUIRE(k.equal_to(u, v));
      };
    }
  }

  TEST_CASE("Example A4: equal_to (std::string)", "[quick][017]", ) {
    equal_to_ex_A4<std::string>();
  }

  TEST_CASE("Example A4: equal_to (MultiStringView)", "[quick][018]", ) {
    equal_to_ex_A4<MultiStringView>();
  }

  ////////////////////////////////////////////////////////////////////////
  // Benchmark wp-prefix - Example A.5
  ////////////////////////////////////////////////////////////////////////

  template <typename T>
  void equal_to_ex_A5(size_t m, size_t first, size_t last, size_t step) {
    std::string lhs1 = "a" + power_string("bc", m) + "a";
    std::string rhs1 = "a" + power_string("cb", m) + "a";
    std::string lhs2 = "b" + power_string("d", m) + "b";
    std::string rhs2 = "a" + power_string("bc", m) + "a";
    Kambites<T> k;
    k.set_alphabet("abcd");
    k.add_rule(lhs1, rhs1);
    k.add_rule(lhs2, rhs2);
    for (size_t N = first; N <= last; N += step) {
      auto random = random_strings("abcdefgh", N, 0, 4 * N + 4);
      auto u      = zip(random_sequence(rhs1, rhs2, N), random);
      auto v      = zip(random_sequence(lhs1, lhs2, N), random);
      BENCHMARK("length = " + std::to_string(u.size() + v.size())) {
        REQUIRE(k.equal_to(u, v));
      };
    }
  }

  TEST_CASE("Example A5_254: equal_to (std::string)", "[quick][019]", ) {
    equal_to_ex_A5<std::string>(254, 10, 160, 4);
  }

  TEST_CASE("Example A5_254: equal_to (MultiStringView)", "[quick][020]", ) {
    equal_to_ex_A5<MultiStringView>(254, 10, 310, 8);
  }

  ////////////////////////////////////////////////////////////////////////
  // Benchmark normal_form - Example A.1
  ////////////////////////////////////////////////////////////////////////

  template <typename T>
  void normal_form_ex_A1(size_t m, size_t first, size_t last, size_t step) {
    for (size_t N = first; N < last; N += step) {
      std::string lhs, rhs;
      std::tie(lhs, rhs) = example1(m);
      Kambites<T> k;
      k.set_alphabet("ab");
      k.add_rule(lhs, rhs);
      REQUIRE(k.small_overlap_class() >= 4);
      auto random = random_strings("ab", N, 0, 4 * N + 4);
      auto w      = zip(random_sequence(lhs, rhs, N), random);
      BENCHMARK("length = " + std::to_string(w.size())) {
        REQUIRE_NOTHROW(k.normal_form(w));
      };
    }
  }

  TEST_CASE("Example A1_159: normal_form (std::string)", "[quick][021]", ) {
    normal_form_ex_A1<std::string>(159, 1, 12, 1);
  }

  TEST_CASE("Example A1_159: normal_form checking (MultiStringView)",
            "[quick][022]", ) {
    normal_form_ex_A1<MultiStringView>(159, 1, 40, 1);
  }

  ////////////////////////////////////////////////////////////////////////
  // Benchmark normal_form - Example A.2
  ////////////////////////////////////////////////////////////////////////

  template <typename T>
  void normal_form_ex_A2(size_t m) {
    for (size_t N = 18; N <= 116; N += 2) {
      std::string lhs = "a" + power_string("bc", m) + "a";
      std::string rhs = "a" + power_string("cb", m) + "a";
      Kambites<T> k;
      k.set_alphabet("abc");
      k.add_rule(lhs, rhs);
      REQUIRE(k.small_overlap_class() >= 4);
      auto random = random_strings("abc", N, 0, 4 * N + 4);
      auto w      = zip(random_sequence(lhs, rhs, N), random);
      BENCHMARK("length = " + std::to_string(w.size())) {
        REQUIRE_NOTHROW(k.normal_form(w));
      };
    }
  }

  TEST_CASE("Example A2_104: normal_form checking (std::string)",
            "[quick][023]", ) {
    normal_form_ex_A2<std::string>(104);
  }

  TEST_CASE("Example A2_104: normal_form checking (MultiStringView)",
            "[quick][024]", ) {
    normal_form_ex_A2<MultiStringView>(104);
  }

  ////////////////////////////////////////////////////////////////////////
  // Benchmark normal_form - Example A.3
  ////////////////////////////////////////////////////////////////////////

  template <typename T>
  void normal_form_ex_A3(size_t m) {
  start:
    auto        rules = random_strings(std::string("abcdefgh"), 8, 0, m);
    Kambites<T> k;
    k.set_alphabet("abcdefgh");
    k.add_rule(rules[0], rules[1]);
    k.add_rule(rules[2], rules[3]);
    k.add_rule(rules[4], rules[5]);
    k.add_rule(rules[6], rules[7]);
    if (k.small_overlap_class() < 4) {
      goto start;
    }

    for (size_t N = 20; N < 220; N += 5) {
      auto random = random_strings("abcdefgh", N, 0, 4 * N + 4);
      auto w      = zip(random_sequence(rules[0], rules[7], N), random);
      BENCHMARK("length = " + std::to_string(w.size())) {
        REQUIRE_NOTHROW(k.normal_form(w));
      };
    }
  }

  TEST_CASE("Example A3_14: normal_form checking (std::string)",
            "[quick][025]", ) {
    normal_form_ex_A3<std::string>(14);
  }

  TEST_CASE("Example A3_14: normal_form checking (MultiStringView)",
            "[quick][026]", ) {
    normal_form_ex_A3<MultiStringView>(14);
  }

  ////////////////////////////////////////////////////////////////////////
  // Benchmark normal_form - Example A.4
  ////////////////////////////////////////////////////////////////////////

  template <typename T>
  void normal_form_ex_A4() {
    Kambites<T> k;
    k.set_alphabet("abcde");
    k.add_rule("bceac", "aeebbc");
    k.add_rule("aeebbc", "dabcd");
    k.small_overlap_class();
    std::string w1 = "bceac";
    std::string w2 = "dabcd";
    std::string w3 = "aeebbc";

    for (size_t N = 50; N < 750; N += 18) {
      auto random = random_strings("abcde", N, 0, 12);
      auto w      = zip(random_sequence(w3, w1, N), random);
      BENCHMARK("length = " + std::to_string(w.size())) {
        REQUIRE_NOTHROW(k.normal_form(w));
      };
    }
  }

  TEST_CASE("Example A4: normal_form (std::string)", "[quick][027]", ) {
    normal_form_ex_A4<std::string>();
  }

  TEST_CASE("Example A4: normal_form (MultiStringView)", "[quick][028]", ) {
    normal_form_ex_A4<MultiStringView>();
  }

  ////////////////////////////////////////////////////////////////////////
  // Benchmark normal_form - Example A.5
  ////////////////////////////////////////////////////////////////////////

  template <typename T>
  void normal_form_ex_A5(size_t m) {
    std::string lhs1 = "a" + power_string("bc", m) + "a";
    std::string rhs1 = "a" + power_string("cb", m) + "a";
    std::string lhs2 = "b" + power_string("d", m) + "b";
    std::string rhs2 = "a" + power_string("bc", m) + "a";
    Kambites<T> k;
    k.set_alphabet("abcd");
    k.add_rule(lhs1, rhs1);
    k.add_rule(lhs2, rhs2);
    for (size_t N = 18; N <= 310; N += 8) {
      auto random = random_strings("abcdefgh", N, 0, 4 * N + 4);
      auto w      = zip(random_sequence(rhs1, rhs2, N), random);
      BENCHMARK("length = " + std::to_string(w.size())) {
        REQUIRE_NOTHROW(k.normal_form(w));
      };
    }
  }

  TEST_CASE("Example A5_42: normal_form (std::string)", "[quick][029]", ) {
    normal_form_ex_A5<std::string>(42);
  }

  TEST_CASE("Example A5_42: normal_form (MultiStringView)", "[quick][030]", ) {
    normal_form_ex_A5<MultiStringView>(42);
  }

  ////////////////////////////////////////////////////////////////////////
  // Benchmark number_of_normal_forms - Example A.1
  ////////////////////////////////////////////////////////////////////////

  template <typename T>
  void number_of_normal_forms_ex_A1(size_t m) {
    auto        rg = ReportGuard(false);
    std::string lhs, rhs;
    std::tie(lhs, rhs) = example1(m);
    BENCHMARK("Enumerate all normal forms length 0 to 18") {
      Kambites<T> k;
      k.set_alphabet("ab");
      k.add_rule(lhs, rhs);
      REQUIRE(k.number_of_normal_forms(0, 18) == 262142);
    };
  }

  TEST_CASE("Example A1_4: number_of_normal_forms (std::string)",
            "[quick][031]", ) {
    number_of_normal_forms_ex_A1<std::string>(4);
  }

  TEST_CASE("Example A1_4: number_of_normal_forms (MultiStringView)",
            "[quick][032]", ) {
    number_of_normal_forms_ex_A1<MultiStringView>(4);
  }

  ////////////////////////////////////////////////////////////////////////
  // Benchmark number_of_normal_forms - Example A.2
  ////////////////////////////////////////////////////////////////////////

  template <typename T>
  void number_of_normal_forms_ex_A2(size_t m) {
    std::array<size_t, 5> values = {0, 29381, 29516, 29523, 29523};
    auto                  rg     = ReportGuard(false);
    std::string           lhs    = "a" + power_string("bc", m) + "a";
    std::string           rhs    = "a" + power_string("cb", m) + "a";
    BENCHMARK("Enumerate normal forms length 0 to 10, m = "
              + std::to_string(m)) {
      Kambites<T> k;
      k.set_alphabet("abc");
      k.add_rule(lhs, rhs);
      REQUIRE(k.number_of_normal_forms(0, 10) == values[m]);
    };
  }  // NOLINT(readability/braces)

  TEST_CASE("Example A2_1_2_3_4: number_of_normal_forms (std::string)",
            "[quick][033]", ) {
    for (size_t i = 1; i <= 4; ++i) {
      number_of_normal_forms_ex_A2<std::string>(i);
    }
  }

  TEST_CASE("Example A2_1_2_3_4: number_of_normal_forms (MultiStringView)",
            "[quick][034]", ) {
    for (size_t i = 1; i <= 4; ++i) {
      number_of_normal_forms_ex_A2<MultiStringView>(i);
    }
  }

  std::string swap_a_and_b(std::string const& w) {
    std::string result;
    for (auto l : w) {
      if (l == 'a') {
        result += "b";
      } else {
        result += "a";
      }
    }
    return result;
  }

  void normal_form_2_letter_1_relation_c4_monoids(size_t min,
                                                  size_t max,
                                                  size_t nr_words,
                                                  size_t length_words) {
    auto first
        = cbegin_sislo("ab", std::string(min, 'a'), std::string(max, 'a'));
    auto last
        = cbegin_sislo("ab", std::string(min, 'a'), std::string(max, 'a'));
    size_t N = std::distance(
        first, cend_sislo("ab", std::string(min, 'a'), std::string(max, 'a')));
    std::advance(last, N - 1);

    auto llast = last;
    ++llast;
    std::unordered_set<std::string>        set;
    std::vector<Kambites<MultiStringView>> kk;

    for (auto it1 = first; it1 != last; ++it1) {
      auto it2 = it1;
      ++it2;
      for (; it2 != llast; ++it2) {
        Kambites<MultiStringView> k;
        k.set_alphabet("ab");
        k.add_rule(*it1, *it2);
        auto tmp = *it1 + "#" + *it2;
        if (set.insert(tmp).second) {
          if (k.small_overlap_class() >= 4) {
            auto u = swap_a_and_b(*it1);
            auto v = swap_a_and_b(*it2);
            if (shortlex_compare(u, v)) {
              set.insert(u + "#" + v);
            } else {
              set.insert(v + "#" + u);
            }
            std::cout << *it1 << " = " << *it2 << std::endl;
            kk.push_back(k);
          }
        }
      }
    }
    auto   w = random_strings("ab", nr_words, length_words, length_words + 1);
    size_t n = 0;
    for (auto& k : kk) {
      n += 1;
      BENCHMARK("n = " + std::to_string(n)) {
        std::string u;
        for (size_t i = 0; i < nr_words; ++i) {
          u += k.normal_form(w[i]);
        }
        return u;
      };
    }
  }

  TEST_CASE("All 2-letter 1-relation C4 monoids 10 random words of length 10)",
            "[035]", ) {
    normal_form_2_letter_1_relation_c4_monoids(7, 11, 10, 10);
  }

  TEST_CASE(
      "All 2-letter 1-relation C4 monoids 100 random words of length 100)",
      "[036]", ) {
    normal_form_2_letter_1_relation_c4_monoids(7, 11, 100, 100);
  }

  /*std::array<std::string, 5> swap_a_b_c(std::string const& w) {
    static std::array<std::string, 5> perms
        = {"bac", "acb", "cba", "bca", "cab"};
    std::array<std::string, 5> result;
    size_t                     count = 0;
    for (auto const& p : perms) {
      std::string ww;
      for (auto l : w) {
        if (l == 'a') {
          ww += p[0];
        } else if (l == 'b') {
          ww += p[1];
        } else {
          ww += p[2];
        }
      }
      result[count] = ww;
      count++;
    }
    return result;
  }

  TEST_CASE("Kambites",
                          "073",
                          "(fpsemi) 3-generated 1-relation C(4)-semigroups",
                          "[extreme][kambites][fpsemigroup][fpsemi]") {
    auto   first = cbegin_sislo("abc", "a", std::string(8, 'a'));
    auto   last  = cbegin_sislo("abc", "a", std::string(8, 'a'));
    size_t N
        = std::distance(first, cend_sislo("abc", "a", std::string(8, 'a')));
    REQUIRE(N == 3279);
    std::advance(last, N - 1);

    size_t total_c4 = 0;
    size_t total    = 0;
    auto   llast    = last;
    ++llast;
    std::unordered_set<std::string>        set;
    std::vector<Kambites<MultiStringView>> kk;

    for (auto it1 = first; it1 != last; ++it1) {
      auto it2 = it1;
      ++it2;
      for (; it2 != llast; ++it2) {
        total++;
        Kambites<std::string> k;
        k.set_alphabet("abc");
        k.add_rule(*it1, *it2);
        if (k.small_overlap_class() >= 4) {
          auto tmp = *it1 + "#" + *it2;
          if (set.insert(tmp).second) {
            auto u = swap_a_b_c(*it1);
            auto v = swap_a_b_c(*it2);
            for (size_t i = 0; i < 5; ++i) {
              if (shortlex_compare(u[i], v[i])) {
                set.insert(u[i] + "#" + v[i]);
              } else {
                set.insert(v[i] + "#" + u[i]);
              }
            }
            std::cout << *it1 << " = " << *it2 << std::endl;
            total_c4++;
          }
        }
      }
    }
    REQUIRE(total_c4 == 307511);
    REQUIRE(total == 5374281);
    auto   w = random_strings("abc", 10, 10, 11);
    size_t n = 0;
    for (auto& k : kk) {
      n += 1;
      BENCHMARK("n = " + std::to_string(n)) {
        std::string u;
        for (size_t i = 0; i < 10; ++i) {
          u += k.normal_form(w[i]);
        }
        return u;
      };
    }
  }*/

  void random_benchmarks(std::string const& alphabet,
                         size_t             number,
                         size_t             min,
                         size_t             max,
                         size_t             sample_size) {
    std::cout << "Alphabet            = " << alphabet << std::endl;
    std::cout << "Number of relations = " << number << std::endl;
    std::cout << "Minimum length      = " << min << std::endl;
    std::cout << "Maximum length      = " << max << std::endl;
    std::cout << "Sample size         = " << sample_size << std::endl;

    std::vector<std::vector<std::string>> sample;
    for (size_t i = 0; i < sample_size; ++i) {
      sample.push_back(random_strings(alphabet, 2 * number, min, max));
    }

    size_t number_confluent = 0;
    size_t number_c4        = 0;
    size_t number_both      = 0;

    for (size_t i = 0; i < sample_size; ++i) {
      bool confluent = false;
      bool c4        = false;
      BENCHMARK_ADVANCED("(KnuthBendix) n = " + std::to_string(i))
      (Catch::Benchmark::Chronometer meter) {
        fpsemigroup::KnuthBendix k;
        k.set_alphabet(alphabet);
        for (size_t j = 0; j < 2 * number; j += 2) {
          k.add_rule(sample[i][j], sample[i][j + 1]);
        }
        meter.measure([&k, &confluent]() {
          bool result = k.confluent();
          if (result) {
            confluent = true;
          }
          return result;
        });
      };  // NOLINT(readability/braces)
      BENCHMARK_ADVANCED("(Kambites) n = " + std::to_string(i))
      (Catch::Benchmark::Chronometer meter) {
        Kambites<std::string> k;
        k.set_alphabet(alphabet);
        for (size_t j = 0; j < 2 * number; j += 2) {
          k.add_rule(sample[i][j], sample[i][j + 1]);
        }
        meter.measure([&k, &c4]() {
          size_t result = k.small_overlap_class();
          if (result >= 4) {
            c4 = true;
          }
          return result;
        });
      };  // NOLINT(readability/braces)
      number_c4 += c4;
      number_confluent += confluent;
      number_both += (c4 && confluent);
    }
    std::cout << "\nNumber confluent = " << number_confluent << std::endl;
    std::cout << "Number C(4)      = " << number_c4 << std::endl;
    std::cout << "Number both      = " << number_both << std::endl;
  }

  TEST_CASE("Random benchmarks 2-generator", "[037]") {
    size_t sample_size = 100;
    for (size_t nr_rels = 1; nr_rels < 200; nr_rels += 4) {
      for (size_t min = 50; min < 13000; min *= 2) {
        std::cout << std::string(72, '#') << std::endl;
        random_benchmarks("ab", nr_rels, min, 2 * min, sample_size);
        std::cout << std::string(72, '#') << std::endl;
      }
    }
  }
}  // namespace libsemigroups
