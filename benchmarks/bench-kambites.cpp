//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2021-2022 James D. Mitchell + Maria Tsalakou
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

#define CATCH_CONFIG_ENABLE_TUPLE_STRINGMAKER

#include <cstddef>   // for size_t
#include <fstream>   // for ofstream
#include <iostream>  // for to_string
#include <string>    // for to_string

#include "bench-main.hpp"  // for CATCH_CONFIG_ENABLE_BENCHMARKING
#include "catch.hpp"       // for TEST_CASE, BENCHMARK, REQUIRE

#include "libsemigroups/kambites.hpp"      // for Kambites
#include "libsemigroups/knuth-bendix.hpp"  // for KnuthBendix
#include "libsemigroups/words.hpp"         // for Strings

namespace libsemigroups {
  using detail::MultiStringView;
  using detail::power_string;

  namespace {
    std::string zip(std::vector<std::string> const& x,
                    std::vector<std::string> const& y) {
      LIBSEMIGROUPS_ASSERT(x.size() == y.size());
      std::string result;
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

    template <typename S, typename T>
    void xml_tag(S name, T val) {
      std::cout << "<" << name << " value=\"" << val << "\"></" << name
                << ">\n";
    }
  }  // namespace

  ////////////////////////////////////////////////////////////////////////
  // Benchmark checking C(4) or higher - Example A.1
  ////////////////////////////////////////////////////////////////////////

  // <a, b | abab^2 ... >

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

  template <typename T, typename F>
  void c4_ex_A1(F&& foo) {
    T                         k;
    Presentation<std::string> p;
    p.alphabet("ab");
    for (size_t N = 100; N <= 1000; N += 25) {
      size_t const M = N * (2 * N + 3);
      BENCHMARK_ADVANCED(std::to_string(M))
      (Catch::Benchmark::Chronometer meter) {
        std::string lhs, rhs;
        std::tie(lhs, rhs) = example1(N);
        meter.measure([&]() {
          p.rules = {lhs, rhs};
          k.init(p);
          return foo(k);
        });
      };  // NOLINT(readability/braces)
    }
  }

  TEST_CASE("Example A.1 - C(4)-check - std::string", "[quick][000]", ) {
    xml_tag(
        "Title",
        "C(4)-check for $\\langle a, b \\mid abab^2\\cdots ab^n = ab^{n + 1} "
        "ab^{n+2} \\cdots ab^{2n}\\rangle$");
    xml_tag("XLabel", "Sums of lengths of relation words");
    xml_tag("Label", "std::string");
    c4_ex_A1<Kambites<std::string>>(
        [](auto& k) { return k.small_overlap_class(); });
  }

  TEST_CASE("Example A.1 - C(4)-check - MultiStringView", "[quick][001]", ) {
    xml_tag("Label", "libsemigroups::MultiStringView");
    c4_ex_A1<Kambites<MultiStringView>>(
        [](auto& k) { return k.small_overlap_class(); });
  }

  // TODO re add this case,
  // TEST_CASE("Example A.1 - KnuthBendix", "[quick][002]", ) {
  //   auto rg = ReportGuard(false);
  //   xml_tag("Label", "Knuth-Bendix");
  //   c4_ex_A1<KnuthBendix>([](auto& kb) {
  //     kb.run();
  //     return kb.confluent();
  //   });
  // }

  ////////////////////////////////////////////////////////////////////////
  // Benchmark checking C(4) or higher - Example A.2
  ////////////////////////////////////////////////////////////////////////

  // <a, b, c | a(bc) ^ k a = a (cb) ^ l a>

  template <typename T, typename F>
  void c4_ex_A2(F&& foo) {
    T                         k;
    Presentation<std::string> p;
    p.alphabet("ab");
    for (size_t m = 5000; m < 500000; m += 20000) {
      BENCHMARK_ADVANCED(std::to_string(4 * m + 4))
      (Catch::Benchmark::Chronometer meter) {
        std::string lhs = "a" + power_string("bc", m) + "a";
        std::string rhs = "a" + power_string("cb", m) + "a";
        meter.measure([&]() {
          p.rules = {lhs, rhs};
          k.init(p);
          foo(k);
        });
      };  // NOLINT(readability/braces)
    }
  }

  TEST_CASE("Example A.2 - C(4)-check - std::string", "[quick][003]", ) {
    xml_tag(
        "Title",
        "C(4)-check for $\\langle a, b, c \\mid a(bc)^ka = a (cb)^la\\rangle$");
    xml_tag("XLabel", "Sums of lengths of relation words");
    xml_tag("Label", "std::string");
    c4_ex_A2<Kambites<std::string>>(
        [](auto& k) { return k.small_overlap_class(); });
  }

  TEST_CASE("Example A.2 - C(4)-check - MultiStringView", "[quick][004]", ) {
    xml_tag("Label", "libsemigroups::MultiStringView");
    c4_ex_A2<Kambites<MultiStringView>>(
        [](auto& k) { return k.small_overlap_class(); });
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
      auto random = random_strings("ab", N, 0, 4 * N + 4);
      auto u      = zip(random_sequence(lhs, rhs, N), random);
      auto v      = zip(random_sequence(lhs, rhs, N), random);
      REQUIRE(k.small_overlap_class() >= 4);
      BENCHMARK(std::to_string(u.size() + v.size())) {
        std::ignore = k.contains(u, v);
      };
    }
  }

  TEST_CASE("Example A.1 - n = 10 - contains - MultiStringView",
            "[A1][contains][n=10]") {
    size_t const n = 10;
    xml_tag("Title",
            "WpPrefix for $\\langle a, b \\mid ab^1ab^2\\cdots ab^n = "
            "ab^{n + 1} ab^{n+2} \\cdots ab^{2n}\\rangle$");
    xml_tag("XLabel", "The sum of the lengths of the 2 words compared");
    xml_tag("Label", std::string("$n = ") + std::to_string(n) + "$");
    equal_to_ex_A1<MultiStringView>(n);
  }

  namespace {
    template <typename T>
    auto c4_check_2_gen_1_rel_all(size_t len) {
      Strings lhs;
      lhs.letters("ab").min(len).max(len + 1);

      rx::advance_by(lhs, lhs.count() - 1);
      auto last = lhs.get();
      lhs.init().letters("ab").min(len).max(len + 1);

      Strings rhs;
      rhs.letters("ab").min(1).max(len);

      uint64_t total_c4     = 0;
      uint64_t total        = 0;
      uint64_t total_length = 0;

      Kambites<T>               k;
      Presentation<std::string> p;

      for (auto const& [i, l] : rx::enumerate(lhs)) {
        for (auto const& r : rhs) {
          total++;
          total_length += l.size() + r.size();
          p.rules = {l, r};
          k.init(p);
          if (k.small_overlap_class() >= 4) {
            total_c4++;
          }
        }
        auto alt_rhs = lhs;
        for (auto const& r : alt_rhs | rx::skip_n(i)) {
          total_length += l.size() + r.size();
          total++;
          p.rules = {l, r};
          k.init(p);
          if (k.small_overlap_class() >= 4) {
            total_c4++;
          }
        }
      }
      return std::make_tuple(total_c4, total, total_length);
    }
  }  // namespace

  TEST_CASE("C(4)-check for all 2-generated 1-relation monoids (max. word "
            "length = 4..12)",
            "[038]") {
    std::array<std::tuple<uint64_t, uint64_t, uint64_t>, 14> const expected
        = {std::make_tuple(0, 0, 0),
           {0, 1, 0},
           {0, 14, 0},
           {0, 76, 392},
           {0, 344, 2'400},
           {0, 1'456, 12'896},
           {0, 5'984, 64'512},
           {2, 24'256, 308'864},
           {26, 97'664, 1'436'160},
           {760, 391'936, 6'540'800},
           {17'382, 1'570'304, 29'331'456},
           {217'458, 6'286'336, 129'959'936},
           {1'994'874, 25'155'584, 570'286'080},
           {14'633'098, 100'642'816, 2'482'724'864}};

    // xml_tag("Title", "C(4)-check for all 2-generated 1-relation monoids");
    xml_tag("XLabel", "Maximum length of a relation word");
    xml_tag("YLabel", "Mean time in microseconds");
    std::vector<uint64_t> results;
    for (size_t n = 4; n < 13; ++n) {
      std::tuple<uint64_t, uint64_t, uint64_t> x;
      BENCHMARK(std::to_string(n)) {
        x = c4_check_2_gen_1_rel_all<MultiStringView>(n);
      };
      results.push_back(std::get<1>(x));
      REQUIRE(x == expected[n]);
    }
    xml_tag("Data", detail::to_string(results));
  }

  /*  namespace {
      auto write_2_gen_1_rel_C4_monoids(std::string const& fname, size_t len) {
        Sislo lhs;
        lhs.alphabet("ab");
        lhs.first(len);
        lhs.last(len + 1);

        Sislo rhs;
        rhs.alphabet("ab");
        rhs.first(1);
        rhs.last(len);

        auto last = lhs.cbegin();
        std::advance(last, std::distance(lhs.cbegin(), lhs.cend()) - 1);

        auto lhs_end = lhs.cend();
        auto rhs_end = rhs.cend();

        Kambites<MultiStringView> k;
        k.set_alphabet("ab");
        std::cout << "Writing file " << fname << " . . ." << std::endl;
        std::ofstream file;
        file.open(fname);

        for (auto l = lhs.cbegin(); l != lhs_end; ++l) {
          for (auto r = rhs.cbegin(); r != rhs_end; ++r) {
            k.clear();
            k.add_rule_nc(*l, *r);
            if (k.small_overlap_class() >= 4) {
              file << *l << "\n";
              file << *r << "\n";
            }
          }
          if (l != last) {
            for (auto r = l + 1; r != lhs_end; ++r) {
              k.clear();
              k.add_rule_nc(*l, *r);
              if (k.small_overlap_class() >= 4) {
                file << *l << "\n";
                file << *r << "\n";
              }
            }
          }
        }
        file.close();
      }
      // TODO(write equality check benchmark)
    }  // namespace

    TEST_CASE("Write 2-generated 1-relation C(4) monoids to file",
              "[equality][wpprefix][write-files]") {
      for (size_t n = 7; n < 14; ++n) {
        write_2_gen_1_rel_C4_monoids(
            "2_gen_1_rel_C4_monoids_" + std::to_string(n) + ".txt", n);
      }
    } */

  namespace {
    template <typename T>
    auto
    c4_check_2_gen_1_rel_from_sample(std::vector<std::string> const& sample) {
      uint64_t total_c4     = 0;
      uint64_t total        = 0;
      uint64_t total_length = 0;

      Kambites<T>               k;
      Presentation<std::string> p;
      p.alphabet("ab");

      for (auto l = sample.cbegin(); l != sample.cend(); l += 2) {
        for (auto r = l + 1; r != sample.cend() - 1; r += 2) {
          total_length += l->size() + r->size();
          total++;
          p.rules = {*l, *r};
          k.init(p);
          if (k.small_overlap_class() >= 4) {
            total_c4++;
          }
        }
      }
      return std::make_tuple(total_c4, total, total_length);
    }
  }  // namespace

  TEST_CASE("C(4)-check for random 2-generated 1-relation monoids "
            "(max. word length 10,12..100 )",
            "[039]") {
    xml_tag("Title", "C(4)-check for random 2-generated 1-relation monoids");
    xml_tag("XLabel", "Maximum length of a relation word");
    xml_tag("YLabel", "Mean time in nanoseconds");

    size_t const          sample_size = 1'000;
    std::vector<uint64_t> results;

    for (size_t n = 10; n < 100; n += 2) {
      std::vector<std::string> sample;
      for (size_t i = 0; i < sample_size; ++i) {
        sample.push_back(random_string("ab", n));
        sample.push_back(random_string("ab", 1, n));
      }

      std::tuple<uint64_t, uint64_t, uint64_t> x;
      BENCHMARK(std::to_string(n)) {
        x = c4_check_2_gen_1_rel_from_sample<MultiStringView>(sample);
      };
      results.push_back(std::get<1>(x));
    }
    xml_tag("Data", detail::to_string(results));
  }

  // This test case is only to compute an approx. of the ratio of C4 to
  // total 2-generator 1-relation monoids
  TEST_CASE("C(4)-check for random 2-generated 1-relation monoids"
            "(max. word length 10..50)",
            "[041]") {
    size_t const          sample_size = 1'000;
    std::vector<uint64_t> results;

    for (size_t n = 10; n < 51; n += 1) {
      std::vector<std::string> sample;
      for (size_t i = 0; i < sample_size; ++i) {
        sample.push_back(random_string("ab", n));
        sample.push_back(random_string("ab", 1, n));
      }

      std::tuple<uint64_t, uint64_t, uint64_t> x
          = c4_check_2_gen_1_rel_from_sample<MultiStringView>(sample);
      std::cout << "n = " << n << std::endl;
      std::cout << "ratio = "
                << static_cast<long double>(std::get<0>(x)) / std::get<1>(x)
                << std::endl;
      results.push_back(std::get<1>(x));
    }
  }

  TEST_CASE("C(4)-check for random 2-generated 1-relation monoids "
            "(max. word length 1000,3000..100000)",
            "[040]") {
    xml_tag("Title", "C(4)-check for random 2-generated 1-relation monoids");
    xml_tag("XLabel", "Maximum length of a relation word");
    xml_tag("YLabel", "Mean time in nanoseconds");

    size_t const          sample_size = 100;
    std::vector<uint64_t> results;

    for (size_t n = 1000; n < 100000; n += 2000) {
      std::vector<std::string> sample;
      for (size_t i = 0; i < 2 * sample_size; ++i) {
        sample.push_back(random_string("ab", n));
      }

      std::tuple<uint64_t, uint64_t, uint64_t> x;
      BENCHMARK(std::to_string(n)) {
        x = c4_check_2_gen_1_rel_from_sample<MultiStringView>(sample);
      };
      results.push_back(std::get<1>(x));
    }
    xml_tag("Data", detail::to_string(results));
  }

  ////////////////////////////////////////////////////////////////////////
  // Equality checking benchmarks
  ////////////////////////////////////////////////////////////////////////

  namespace {
    std::vector<std::string> all_2_gen_1_rel_C4_monoids(size_t n) {
      std::array<uint64_t, 14> const expected = {0,
                                                 0,
                                                 0,
                                                 0,
                                                 0,
                                                 0,
                                                 0,
                                                 2,
                                                 26,
                                                 760,
                                                 17'382,
                                                 217'458,
                                                 1'994'874,
                                                 14'633'098};
      std::ifstream                  file;
      file.open("2_gen_1_rel_C4_monoids_" + std::to_string(n) + ".txt");

      std::vector<std::string> relations;
      std::string              line;

      while (std::getline(file, line)) {
        relations.push_back(line);
      }

      REQUIRE(relations.size() == 2 * expected[n]);
      return relations;
    }

    std::vector<std::string> pseudo_random_sample_words(size_t             num,
                                                        std::string const& lhs,
                                                        std::string const& rhs,
                                                        size_t             N) {
      std::vector<std::string> results;
      for (size_t i = 0; i < num; ++i) {
        auto random = random_strings("ab", N, 0, 4 * N + 4);
        results.push_back(zip(random_sequence(lhs, rhs, N), random));
        results.push_back(zip(random_sequence(lhs, rhs, N), random));
      }
      return results;
    }

    std::vector<std::string> random_sample_words(size_t num,
                                                 std::string const&,
                                                 std::string const&,
                                                 size_t N) {
      // Weird looking range here so that the length of the words is approx.
      // the same as from the pseudo_random_sample_words
      return random_strings("ab", 2 * num, 0, 4 * N * N + 7 * N + 4);
    }

    template <typename WordSampler>
    void bench_contains(std::vector<std::string> relations,
                        WordSampler&&            wu,
                        size_t                   min   = 10,
                        size_t                   max   = 90,
                        size_t                   step  = 4,
                        std::string              label = "") {
      auto data = relations.size() / 2;

      Kambites<MultiStringView> k;
      Presentation<std::string> p;
      p.alphabet("ab");

      size_t const sample_size = 10;

      for (size_t N = min; N < max; N += step) {
        BENCHMARK_ADVANCED(std::to_string(N))
        (Catch::Benchmark::Chronometer meter) {
          for (size_t i = 0; i < relations.size() - 1; i += 2) {
            p.rules = {relations[i], relations[i + 1]};
            k.init(p);
            REQUIRE(k.small_overlap_class() >= 4);

            auto words = wu(sample_size, relations[i], relations[i + 1], N);

            meter.measure([&]() {
              bool result = true;
              for (auto wit = words.cbegin(); wit < words.cend() - 1;
                   wit += 2) {
                result &= k.contains(*wit, *(wit + 1));
              }
              return result;
            });
          }
        };  // NOLINT(readability/braces)
      }

      // xml_tag("Title",
      //         "Word problem for all 2-generated 1-relation monoids with
      //         max. " "relation word length " + std::to_string(n));
      xml_tag("XLabel", "$N$");
      xml_tag("YLabel", "Mean time in ");
      if (!label.empty()) {
        xml_tag("Label", label);
      }
      xml_tag("Data", detail::to_string(data));
    }
  }  // namespace

  TEST_CASE("Word problem for all 2-generated 1-relation monoids (max. word "
            "length = 7) pseudo-random",
            "[contains][n=7][pseudo]") {
    bench_contains(all_2_gen_1_rel_C4_monoids(7),
                   pseudo_random_sample_words,
                   10,
                   90,
                   4,
                   "pseudo-random words");
  }

  TEST_CASE("Word problem for all 2-generated 1-relation monoids (max. word "
            "length = 7) uniform random",
            "[contains][n=7][uniform]") {
    bench_contains(all_2_gen_1_rel_C4_monoids(7),
                   random_sample_words,
                   10,
                   90,
                   4,
                   "uniform random words");
  }

  TEST_CASE("Word problem for all 2-generated 1-relation monoids (max. word "
            "length = 8) pseudo-random",
            "[contains][n=8][pseudo]") {
    bench_contains(all_2_gen_1_rel_C4_monoids(8),
                   pseudo_random_sample_words,
                   10,
                   90,
                   4,
                   "pseudo-random words");
  }

  TEST_CASE("Word problem for all 2-generated 1-relation monoids (max. word "
            "length = 8) uniform random",
            "[contains][n=8][uniform]") {
    bench_contains(all_2_gen_1_rel_C4_monoids(8),
                   random_sample_words,
                   10,
                   90,
                   4,
                   "uniform random words");
  }

  // For n = 9 and higher the benchmarks are super slow, probably because of
  // the large(ish) number of C(4) presentations.

  TEST_CASE("Word problem for random 2-generated 2-relation presentation "
            "(maximum word length = 100) with pseudo-random words N = 10, "
            "14, .., 86",
            "[contains][100][N=10][pseudo]") {
    // Note that although N < max. length of a relation, the actual
    // pseudo-random words used are longer than max (probably)!
    std::vector<std::string> relations;
    relations.push_back(random_string("ab", 100));
    relations.push_back(random_string("ab", 1, 100));
    relations.push_back(random_string("ab", 1, 100));
    relations.push_back(random_string("ab", 1, 100));

    bench_contains(relations,
                   pseudo_random_sample_words,
                   10,
                   90,
                   4,
                   "pseudo-random words");
  }

  TEST_CASE("Word problem for random 2-generated 2-relation presentation "
            "(maximum word length = 100) with uniform random words N = 10, "
            "14, .., 86",
            "[contains][100][N=10][uniform]") {
    // Note that although N < max. length of a relation, the actual
    // random words used are longer than max (probably)!
    std::vector<std::string> relations;
    relations.push_back(random_string("ab", 100));
    relations.push_back(random_string("ab", 1, 100));
    relations.push_back(random_string("ab", 1, 100));
    relations.push_back(random_string("ab", 1, 100));

    bench_contains(
        relations, random_sample_words, 10, 90, 4, "uniform random words");
  }

  TEST_CASE("Word problem for random 2-generated 2-relation presentation "
            "(maximum word length = 100) with pseudo-random words N = 100, "
            "140, .., 860",
            "[contains][100][N=100][pseudo]") {
    std::vector<std::string> relations;
    relations.push_back(random_string("ab", 100));
    relations.push_back(random_string("ab", 1, 100));
    relations.push_back(random_string("ab", 1, 100));
    relations.push_back(random_string("ab", 1, 100));

    bench_contains(relations,
                   pseudo_random_sample_words,
                   100,
                   900,
                   40,
                   "pseudo-random words");
  }

  TEST_CASE("Word problem for random 2-generated 2-relation presentation "
            "(maximum word length = 100) with uniform random words N = 100, "
            "140, .., 860",
            "[contains][100][N=100][uniform]") {
    std::vector<std::string> relations;
    relations.push_back(random_string("ab", 100));
    relations.push_back(random_string("ab", 1, 100));
    relations.push_back(random_string("ab", 1, 100));
    relations.push_back(random_string("ab", 1, 100));

    bench_contains(
        relations, random_sample_words, 100, 900, 40, "uniform random words");
  }

  ////////////////////////////////////////////////////////////////////////
  // Normal form benchmarks
  ////////////////////////////////////////////////////////////////////////

  namespace {
    template <typename WordSampler>
    void bench_normal_form(std::vector<std::string> relations,
                           WordSampler&&            wu,
                           size_t                   min   = 10,
                           size_t                   max   = 90,
                           size_t                   step  = 4,
                           std::string              label = "") {
      auto data = relations.size() / 2;

      Kambites<MultiStringView> k;
      Presentation<std::string> p;
      p.alphabet("ab");

      size_t const sample_size = 10;

      for (size_t N = min; N < max; N += step) {
        BENCHMARK_ADVANCED(std::to_string(N))
        (Catch::Benchmark::Chronometer meter) {
          for (size_t i = 0; i < relations.size() - 1; i += 2) {
            p.rules = {relations[i], relations[i + 1]};
            k.init(p);
            REQUIRE(k.small_overlap_class() >= 4);

            auto words = wu(sample_size, relations[i], relations[i + 1], N);

            bool result = true;
            meter.measure([&]() {
              for (auto wit = words.cbegin(); wit < words.cend(); ++wit) {
                result &= k.contains(k.normal_form(*wit), *wit);
              }
            });
            if (!result) {
              std::cout << "lhs = " << relations[i] << std::endl;
              std::cout << "rhs = " << relations[i + 1] << std::endl;
              std::cout << words << std::endl;
            }
            REQUIRE(result);
          }
        };  // NOLINT(readability/braces)
      }

      // xml_tag("Title",
      //         "Word problem for all 2-generated 1-relation monoids with
      //         max. " "relation word length " + std::to_string(n));
      xml_tag("XLabel", "$N$");
      xml_tag("YLabel", "Mean time in ");
      if (!label.empty()) {
        xml_tag("Label", label);
      }
      xml_tag("Data", detail::to_string(data));
    }
  }  // namespace

  TEST_CASE("Normal form for all 2-generated 1-relation monoids (max. word "
            "length = 7) pseudo-random",
            "[normal_form][n=7][pseudo]") {
    bench_normal_form(all_2_gen_1_rel_C4_monoids(7),
                      pseudo_random_sample_words,
                      10,
                      90,
                      4,
                      "pseudo-random words");
  }

  TEST_CASE("Normal form for all 2-generated 1-relation monoids (max. word "
            "length = 7) uniform random",
            "[normal_form][n=7][uniform]") {
    bench_normal_form(all_2_gen_1_rel_C4_monoids(7),
                      random_sample_words,
                      10,
                      90,
                      4,
                      "uniform random words");
  }

  TEST_CASE("Normal form for all 2-generated 1-relation monoids (max. word "
            "length = 8) pseudo-random",
            "[normal_form][n=8][pseudo]") {
    bench_normal_form(all_2_gen_1_rel_C4_monoids(8),
                      pseudo_random_sample_words,
                      10,
                      90,
                      4,
                      "pseudo-random words");
  }

  TEST_CASE("Normal form for all 2-generated 1-relation monoids (max. word "
            "length = 8) uniform random",
            "[normal_form][n=8][uniform]") {
    bench_normal_form(all_2_gen_1_rel_C4_monoids(8),
                      random_sample_words,
                      10,
                      90,
                      4,
                      "uniform random words");
  }

  TEST_CASE("Normal form for random 2-generated 2-relation presentation "
            "(maximum word length = 100) with pseudo-random words N = 10, "
            "14, .., 86",
            "[normal_form][100][N=10][pseudo]") {
    // Note that although N < max. length of a relation, the actual
    // pseudo-random words used are longer than max (probably)!
    std::vector<std::string> relations;
    relations.push_back(random_string("ab", 100));
    relations.push_back(random_string("ab", 1, 100));
    relations.push_back(random_string("ab", 1, 100));
    relations.push_back(random_string("ab", 1, 100));

    bench_normal_form(relations,
                      pseudo_random_sample_words,
                      10,
                      90,
                      4,
                      "pseudo-random words");
  }

  TEST_CASE("Normal form for random 2-generated 2-relation presentation "
            "(maximum word length = 100) with uniform random words N = 10, "
            "14, .., 86",
            "[normal_form][100][N=10][uniform]") {
    // Note that although N < max. length of a relation, the actual
    // random words used are longer than max (probably)!
    std::vector<std::string> relations;
    relations.push_back(random_string("ab", 100));
    relations.push_back(random_string("ab", 1, 100));
    relations.push_back(random_string("ab", 1, 100));
    relations.push_back(random_string("ab", 1, 100));

    bench_normal_form(
        relations, random_sample_words, 10, 90, 4, "uniform random words");
  }

  TEST_CASE("Normal form for random 2-generated 2-relation presentation "
            "(maximum word length = 100) with pseudo-random words N = 100, "
            "140, .., 860",
            "[normal_form][100][N=100][pseudo]") {
    std::vector<std::string> relations;
    relations.push_back(random_string("ab", 100));
    relations.push_back(random_string("ab", 1, 100));
    relations.push_back(random_string("ab", 1, 100));
    relations.push_back(random_string("ab", 1, 100));

    bench_normal_form(relations,
                      pseudo_random_sample_words,
                      100,
                      900,
                      40,
                      "pseudo-random words");
  }

  TEST_CASE("Normal form for random 2-generated 2-relation presentation "
            "(maximum word length = 100) with uniform random words N = 100, "
            "140, .., 860",
            "[normal_form][100][N=100][uniform]") {
    std::vector<std::string> relations;
    relations.push_back(random_string("ab", 100));
    relations.push_back(random_string("ab", 1, 100));
    relations.push_back(random_string("ab", 1, 100));
    relations.push_back(random_string("ab", 1, 100));

    bench_normal_form(
        relations, random_sample_words, 100, 900, 40, "uniform random words");
  }

}  // namespace libsemigroups
