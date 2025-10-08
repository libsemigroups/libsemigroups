//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2020-2025 James D. Mitchell
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

#include <chrono>
#include <initializer_list>
#define CATCH_CONFIG_ENABLE_BENCHMARKING

#include <cstddef>  // for size_t
#include <cstdint>  // for uint64_t
#include <string>   // for operator+, basic_string
#include <vector>   // for vector, operator==

#include "Catch2-3.8.0/catch_amalgamated.hpp"  // for REQUIRE, REQUIRE_NOTHROW, REQUIRE_THROWS_AS

#include "libsemigroups/presentation-examples.hpp"
#include "libsemigroups/presentation.hpp"
#include "libsemigroups/todd-coxeter.hpp"  // for ToddCoxeter, ToddCoxeter::...
#include "libsemigroups/types.hpp"         // for word_type, letter_type
#include "libsemigroups/word-range.hpp"    // for operator""_p

#include "libsemigroups/detail/report.hpp"  // for ReportGuard
//
namespace libsemigroups {
  using literals::            operator""_p;
  using std::string_literals::operator""s;
  using strategy         = detail::ToddCoxeterImpl::options::strategy;
  using lookahead_extent = detail::ToddCoxeterImpl::options::lookahead_extent;
  using def_policy       = detail::ToddCoxeterImpl::options::def_policy;
  using def_version      = detail::ToddCoxeterImpl::options::def_version;

  namespace {
    template <typename S, typename T>
    void open_xml_tag(S name, T val) {
      fmt::print("      <{} value=\"{}\">\n", name, val);
    }

    template <typename S>
    void close_xml_tag(S name) {
      fmt::print("      </{}>\n", name);
    }

    std::string xml_tag() {
      return "/>\n";
    }

    template <typename K, typename V, typename... Args>
    auto xml_tag(K key, V val, Args&&... args)
        -> std::enable_if_t<sizeof...(Args) % 2 == 0, std::string> {
      return fmt::format(" {}=\"{}\"", key, val)
             + xml_tag(std::forward<Args>(args)...);
    }

    template <typename... Args>
    auto xml_tag(char const* name, Args&&... args)
        -> std::enable_if_t<sizeof...(Args) % 2 == 0, std::string> {
      return fmt::format("      <{} ", name)
             + xml_tag(std::forward<Args>(args)...);
    }

    template <typename Word>
    void preprocess_presentation(Presentation<Word>& p) {
      // Removing redundant generators reverses things like
      // greedy_reduce_length, so we don't do it.
      // presentation::remove_redundant_generators(p);
      presentation::reduce_complements(p);
      presentation::remove_trivial_rules(p);
      presentation::remove_duplicate_rules(p);
      presentation::normalize_alphabet(p);
      presentation::sort_each_rule(p);
      presentation::sort_rules(p);
    }

    template <typename Word>
    void emit_xml_presentation_tags(Presentation<Word>& p,
                                    std::string const&  index,
                                    size_t              size) {
      fmt::print("{}", xml_tag("Index", "value", index));
      fmt::print("{}", xml_tag("Size", "value", size));
      fmt::print("{}",
                 xml_tag("PresentationNumGens", "value", p.alphabet().size()));
      fmt::print("{}",
                 xml_tag("PresentationNumRels", "value", p.rules.size() / 2));
      fmt::print(
          "{}",
          xml_tag("PresentationLength", "value", presentation::length(p)));
    }

    template <typename Word>
    void emit_xml_presentation_tags(Presentation<Word>& p,
                                    size_t              index,
                                    size_t              size) {
      emit_xml_presentation_tags(p, std::to_string(index), size);
    }

    template <typename Word>
    constexpr auto DoNothing = [](ToddCoxeter<Word>&) {};

    template <typename Func2>
    void benchmark_todd_coxeter_single(
        size_t                                 size,
        Presentation<word_type>&&              p,
        size_t                                 n,
        std::initializer_list<strategy> const& strategies,
        Func2&&                                init) {
      preprocess_presentation(p);
      emit_xml_presentation_tags(p, n, size);
      auto rg = ReportGuard(true);
      for (auto strategy : strategies) {
        auto title = fmt::format("{}", strategy);
        open_xml_tag("LatexColumnTitle", title);
        SECTION(title) {
          ToddCoxeter tc(congruence_kind::twosided, p);
          init(tc);
          tc.strategy(strategy);
          auto start = std::chrono::high_resolution_clock::now();
          REQUIRE(tc.number_of_classes() == size);
          auto now = std::chrono::high_resolution_clock::now();
          open_xml_tag("BenchmarkResults", title);
          fmt::print(
              "{}",
              xml_tag("mean",
                      "value",
                      std::chrono::duration_cast<std::chrono::nanoseconds>(
                          now - start)
                          .count()));
          fmt::print("{}", xml_tag("standardDeviation", "value", 0));
          close_xml_tag("BenchmarkResults");
        }
        close_xml_tag("LatexColumnTitle");
      }
    }

    void benchmark_todd_coxeter_single(
        size_t                                 size,
        Presentation<word_type>&&              p,
        size_t                                 n,
        std::initializer_list<strategy> const& strategies = {strategy::hlt}) {
      benchmark_todd_coxeter_single<decltype(DoNothing<word_type>) const&>(
          size, std::move(p), n, strategies, DoNothing<word_type>);
    }

    // TODO be good to recover the below
    // std::cout << tc.settings_string();
    // std::cout << tc.stats_string();

#define BENCHMARK_TODD_COXETER_RANGE(SIZES,                    \
                                     CAPTION,                  \
                                     LABEL,                    \
                                     SYMBOL,                   \
                                     FIRST,                    \
                                     LAST,                     \
                                     PRESENTATION,             \
                                     PRESENTATION_NAME,        \
                                     STRATEGIES,               \
                                     INIT)                     \
  TEST_CASE(PRESENTATION_NAME "(n), n = " #FIRST " .. " #LAST, \
            "[paper][" PRESENTATION_NAME "][000]") {           \
    benchmark_todd_coxeter_range(SIZES,                        \
                                 CAPTION,                      \
                                 LABEL,                        \
                                 SYMBOL,                       \
                                 FIRST,                        \
                                 LAST,                         \
                                 PRESENTATION,                 \
                                 STRATEGIES,                   \
                                 INIT);                        \
  }

    using sizes_type = std::initializer_list<uint64_t>;

    template <typename Func1, typename Func2>
    void benchmark_todd_coxeter_range(
        sizes_type const&                      ilist_sizes,
        std::string_view                       caption,
        std::string_view                       label,
        std::string_view                       symbol,
        size_t                                 first,
        size_t                                 last,
        Func1&&                                constructor,
        std::initializer_list<strategy> const& strategies,
        Func2&&                                init) {
      std::vector<uint64_t> sizes(ilist_sizes);
      auto                  rg = ReportGuard(false);
      fmt::print("{}", xml_tag("LatexCaption", "value", caption));
      fmt::print("{}", xml_tag("LatexLabel", "value", label));
      fmt::print("{}", xml_tag("LatexSymbol", "value", symbol));
      for (size_t n = first; n <= last; ++n) {
        auto p = constructor(n);
        preprocess_presentation(p);
        emit_xml_presentation_tags(p, std::to_string(n), sizes[n]);
        for (auto const& strategy : strategies) {
          auto title = fmt::format("{}", strategy);
          open_xml_tag("LatexColumnTitle", title);
          BENCHMARK(title.c_str()) {
            ToddCoxeter tc(congruence_kind::twosided, p);
            tc.strategy(strategy);
            init(tc);
            REQUIRE(tc.number_of_classes() == static_cast<uint64_t>(sizes[n]));
          };
          close_xml_tag("LatexColumnTitle");
        }
      }
    }

#define TEST_CASE_TODD_COXETER(PRESENTATION) \
  TEST_CASE(PRESENTATION, "[paper][" PRESENTATION "]")

    void start_table(char const* caption,
                     char const* label,
                     char const* symbol) {
      fmt::print("{}", xml_tag("LatexCaption", "value", caption));
      fmt::print("{}", xml_tag("LatexLabel", "value", label));
      fmt::print("{}", xml_tag("LatexSymbol", "value", symbol));
    }

  }  // namespace

  ////////////////////////////////////////////////////////////////////////
  // 1. orientation_preserving_monoid
  ////////////////////////////////////////////////////////////////////////

  namespace orientation_preserving {
    sizes_type sizes = {0,
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
                        67'603'744};

    auto strategies = {strategy::hlt, strategy::felsch};

    BENCHMARK_TODD_COXETER_RANGE(
        sizes,
        "The presentations for the monoid $OP_n$ of orientation "
        "preserving transformations of a chain from \\cite{Arthur2000aa}.",
        "table-orient",
        "OP_n",
        3,
        9,
        [](size_t n) {
          return presentation::examples::orientation_preserving_monoid_AR00(n);
        },
        "orientation_preserving_monoid_AR00",
        strategies,
        DoNothing<word_type>);
  }  // namespace orientation_preserving

  // Becomes impractical to do multiple runs when n >= 10, so we switch to
  // doing single runs.
  // Approx 27s (2021 - MacBook Air M1 - 8GB RAM)
  TEST_CASE("orientation_preserving_monoid(n) (Arthur-Ruskuc), n = 10",
            "[paper][orientation_preserving_monoid][n=10][hlt]") {
    benchmark_todd_coxeter_single(
        923'690,
        presentation::examples::orientation_preserving_monoid_AR00(10),
        10);
  }

  // 4m13s (2021 - MacBook Air M1 - 8GB RAM)
  TEST_CASE("orientation_preserving_monoid(n) (Arthur-Ruskuc), n = 11",
            "[paper][orientation_preserving_monoid][n=11][hlt]") {
    benchmark_todd_coxeter_single(
        3'879'766,
        presentation::examples::orientation_preserving_monoid_AR00(11),
        11);
  }

  // 54m35s (2021 - MacBook Air M1 - 8GB RAM)
  TEST_CASE("orientation_preserving_monoid(n) (Arthur-Ruskuc), n = 12",
            "[paper][orientation_preserving_monoid][n=12][hlt]") {
    benchmark_todd_coxeter_single(
        16'224'804,
        presentation::examples::orientation_preserving_monoid_AR00(12),
        12);
  }

  // 9h14m (2021 - MacBook Air M1 - 8GB RAM)
  TEST_CASE("orientation_preserving_monoid(n) (Arthur-Ruskuc), n = 13",
            "[paper][orientation_preserving_monoid][n=13][hlt]") {
    benchmark_todd_coxeter_single(
        67'603'744,
        presentation::examples::orientation_preserving_monoid_AR00(13),
        13);
  }

  ////////////////////////////////////////////////////////////////////////
  // 2. orientation_preserving_reversing_monoid
  ////////////////////////////////////////////////////////////////////////

  namespace orientation_reversing {
    sizes_type sizes      = {0,
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
    auto       strategies = {strategy::hlt, strategy::felsch};
    BENCHMARK_TODD_COXETER_RANGE(
        sizes,
        "The presentations for the monoid $OR_n$ of orientation preserving "
        "and reversing transformations of a chain from \\cite{Arthur2000aa}.",
        "table-orient-reverse",
        "OR_n",
        3,
        8,
        [](size_t n) {
          return presentation::examples::
              orientation_preserving_reversing_monoid_AR00(n);
        },
        "orientation_preserving_reversing_monoid_AR00",
        strategies,
        DoNothing<word_type>);
  }  // namespace orientation_reversing

  // Approx 9s (2021 - MacBook Air M1 - 8GB RAM)
  TEST_CASE("orientation_preserving_reversing_monoid(9) - hlt",
            "[paper][orientation_preserving_reversing_monoid][n=9][hlt]") {
    benchmark_todd_coxeter_single(
        434'835,
        presentation::examples::orientation_preserving_reversing_monoid_AR00(9),
        9);
  }

  // Approx 90s (2021 - MacBook Air M1 - 8GB RAM)
  TEST_CASE("orientation_preserving_reversing_monoid(10) - hlt",
            "[paper][orientation_preserving_reversing_monoid][n=10][hlt]") {
    benchmark_todd_coxeter_single(
        1'843'320,
        presentation::examples::orientation_preserving_reversing_monoid_AR00(
            10),
        10);
  }

  // ?? (2021 - MacBook Air M1 - 8GB RAM)
  TEST_CASE("orientation_preserving_reversing_monoid(11) - hlt",
            "[paper][orientation_preserving_reversing_monoid][n=11][hlt]") {
    benchmark_todd_coxeter_single(
        7'753'471,
        presentation::examples::orientation_preserving_reversing_monoid_AR00(
            11),
        11);
  }

  // ?? (2021 - MacBook Air M1 - 8GB RAM)
  TEST_CASE("orientation_preserving_reversing_monoid(12) - hlt",
            "[paper][orientation_preserving_reversing_monoid][n=12][hlt]") {
    benchmark_todd_coxeter_single(
        32'440'884,
        presentation::examples::orientation_preserving_reversing_monoid_AR00(
            12),
        12);
  }

  // ?? (2021 - MacBook Air M1 - 8GB RAM)
  // TEST_CASE("orientation_preserving_reversing_monoid(13) - hlt",
  //           "[paper][orientation_preserving_reversing_monoid][n=13][hlt]") {
  //   benchmark_todd_coxeter_single(
  //       135'195'307,
  //       presentation::examples::orientation_preserving_reversing_monoid_AR00(13),
  //       13);
  // }

  ////////////////////////////////////////////////////////////////////////
  // partition_monoid
  ////////////////////////////////////////////////////////////////////////

  namespace partition_monoid_namespace {
    sizes_type sizes = {0, 2, 15, 203, 4'140, 115'975, 4'213'597, 190'899'322};
    auto       strategies = {strategy::hlt, strategy::felsch};

    BENCHMARK_TODD_COXETER_RANGE(
        sizes,
        "The presentations for the partition monoids $P_n$ from "
        "\\cite[Theorem 41]{East2011aa}.",
        "table-partition",
        "P_n",
        4,
        6,
        [](size_t n) { return presentation::examples::partition_monoid(n); },
        "partition_monoid",
        strategies,
        DoNothing<word_type>);

    // Becomes impractical to do multiple runs for n >= 7, so we switch to
    // doing single runs.

    auto init_func = [](ToddCoxeter<word_type>& tc) {
      tc.use_relations_in_extra(true)
          .lookahead_next(200'000'000)
          .save(true)
          .lower_bound(190'899'322);
    };

    // Approx 49m35s
    TEST_CASE("partition_monoid(7) - hlt",
              "[paper][partition_monoid][n=7][hlt]") {
      auto p = presentation::examples::partition_monoid(7);
      benchmark_todd_coxeter_single(
          190'899'322, std::move(p), 7, {strategy::hlt}, init_func);
    }

  }  // namespace partition_monoid_namespace

  ////////////////////////////////////////////////////////////////////////
  // DualSymInv
  ////////////////////////////////////////////////////////////////////////

  namespace dual_symmetric_inverse {
    sizes_type sizes
        = {0, 0, 0, 25, 339, 6'721, 179'643, 6'166'105, 262'308'819};

    auto strategies = {strategy::hlt, strategy::felsch, strategy::Rc};

    BENCHMARK_TODD_COXETER_RANGE(
        sizes,
        "The presentations for the dual symmetric inverse "
        "monoids $I_n ^ *$ from \\cite{Easdown2008aa}.",
        "table-dual-sym-inv",
        "I_n^*",
        3,
        6,
        [](size_t n) {
          return presentation::examples::dual_symmetric_inverse_monoid_EEF07(n);
        },
        "dual_symmetric_inverse_monoid_EEF07",
        strategies,
        DoNothing<word_type>);

    // Becomes impractical to do multiple runs for n >= 7, so we switch to
    // doing single runs.

    template <typename Word>
    auto init = [](ToddCoxeter<Word>& tc) {
      tc.lookahead_min(10'000'000).save(true).def_policy(def_policy::unlimited);
    };

    TEST_CASE("dual_symmetric_inverse_monoid(7)",
              "[paper][dual_symmetric_inverse_monoid][n=7]") {
      auto p = presentation::examples::dual_symmetric_inverse_monoid_EEF07(7);
      benchmark_todd_coxeter_single(6'166'105,
                                    std::move(p),
                                    7,
                                    {strategy::hlt, strategy::felsch},
                                    init<word_type>);
    }
  }  // namespace dual_symmetric_inverse

  ////////////////////////////////////////////////////////////////////////
  // uniform_block_bijection_monoid
  ////////////////////////////////////////////////////////////////////////

  namespace uniform_block_bijection {
    sizes_type sizes      = {0, 0, 0, 16, 131, 1'496, 22'482, 426'833};
    auto       strategies = {strategy::hlt, strategy::felsch, strategy::Rc};

    BENCHMARK_TODD_COXETER_RANGE(
        sizes,
        "The presentations for the factorisable dual symmetric inverse "
        "monoids $FI_n ^ *$ from \\cite{fitzgerald_2003}. This monoid is "
        "sometimes called the \\textit{uniform block bijection monoid}.",
        "table-uniform",
        "FI_n^*",
        3,
        7,
        [](size_t n) {
          return presentation::examples::uniform_block_bijection_monoid_Fit03(
              n);
        },
        "uniform_block_bijection_monoid_Fit03",
        strategies,
        [](ToddCoxeter<word_type>& tc) {
          if (tc.strategy() == strategy::Rc) {
            tc.lookahead_extent(lookahead_extent::full);
          }
        });

    // Becomes impractical to do multiple runs for n >= 8, so we switch to
    // doing single runs.

    // Approx 4m39s (2021 - MacBook Air M1 - 8GB RAM)
    TEST_CASE("uniform_block_bijection_monoid(8)",
              "[paper][uniform_block_bijection_monoid][n=8]") {
      benchmark_todd_coxeter_single(
          9'934'563,
          presentation::examples::uniform_block_bijection_monoid_Fit03(8),
          8,
          {strategy::hlt, strategy::felsch, strategy::Rc});
    }
  }  // namespace uniform_block_bijection

  // |FI_9 ^ *| = 277'006'192 which would require too much memory at present.

  ////////////////////////////////////////////////////////////////////////
  // temperley_lieb_monoid
  ////////////////////////////////////////////////////////////////////////

  namespace temperley_lieb {
    sizes_type sizes      = {0,
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
    auto       strategies = {strategy::hlt, strategy::felsch};

    BENCHMARK_TODD_COXETER_RANGE(
        sizes,
        "The presentations for the Temperley-Lieb monoids $J_n$ from "
        "\\cite[Theorem 2.2]{East2021aa}; the Temperley-Lieb monoid is also "
        "sometimes referred to as the \\textit{Jones monoid} in the "
        "literature.",
        "table-temperley-lieb",
        "J_n",
        3,
        14,
        [](size_t n) {
          return presentation::examples::temperley_lieb_monoid_Eas21(n);
        },
        "temperley_lieb_monoid_Eas21",
        strategies,
        DoNothing<word_type>);

    // Becomes impractical to do multiple runs after n >= 15, so we switch to
    // doing single runs.

    // Approx. 18s (2021 - MacBook Air M1 - 8GB RAM)
    TEST_CASE("temperley_lieb_monoid(15) - hlt",
              "[paper][temperley_lieb_monoid][n=15][hlt]") {
      benchmark_todd_coxeter_single(
          9'694'845,
          presentation::examples::temperley_lieb_monoid_Eas21(15),
          15);
    }

    // Approx. 82s (2021 - MacBook Air M1 - 8GB RAM)
    TEST_CASE("temperley_lieb_monoid(16) - hlt",
              "[paper][temperley_lieb_monoid][n=16][hlt]") {
      benchmark_todd_coxeter_single(
          35'357'670,
          presentation::examples::temperley_lieb_monoid_Eas21(16),
          16);
    }

    // Approx. ? (2021 - MacBook Air M1 - 8GB RAM)
    // TEST_CASE("temperley_lieb_monoid(17) - hlt",
    //           "[paper][temperley_lieb_monoid][n=17][hlt]") {
    //   benchmark_todd_coxeter_single(129'644'790,
    //   presentation::examples::temperley_lieb_monoid_Eas21(17), 17);
    // }
  }  // namespace temperley_lieb

  ////////////////////////////////////////////////////////////////////////
  // singular_brauer_monoid
  ////////////////////////////////////////////////////////////////////////

  namespace singular_brauer {
    sizes_type sizes
        = {0, 0, 0, 9, 81, 825, 9'675, 130'095, 1'986'705, 34'096'545};

    auto strategies = {strategy::hlt, strategy::felsch};

    BENCHMARK_TODD_COXETER_RANGE(
        sizes,
        "The presentations for the singular Brauer monoids "
        "$B_n \\setminus S_n$ from \\cite{Maltcev2007aa}.",
        "table-singular-brauer",
        "B_n\\setminus S_n",
        3,
        7,
        [](size_t n) {
          return presentation::examples::singular_brauer_monoid_MM07(n);
        },
        "singular_brauer_monoid_MM07",
        strategies,
        DoNothing<word_type>);

    // Approx. 1 minute
    // TODO lower bound has no impact here, because HLT doesn't check for
    // this, maybe it should? When there are no nodes defined or killed in
    // some interval of time, then we should check if we're already complete
    // and compatible, and if the lower_bound() == number_of_active_nodes
    TEST_CASE("singular_brauer_monoid(8) (Maltcev-Mazorchuk)",
              "[paper][singular_brauer_monoid][n=8]") {
      uint64_t size      = 1'986'705;
      auto     init_func = [&size](ToddCoxeter<word_type>& tc) {
        tc.lookahead_next(size / 2).lookahead_min(size / 2).lower_bound(size);
      };
      benchmark_todd_coxeter_single(
          size,
          presentation::examples::singular_brauer_monoid_MM07(8),
          8,
          {strategy::hlt},
          init_func);
    }

    // Approx. ? Running this appears to use >27gb of memory, which JDM's
    // laptop does not have. The presentation is also huge, which makes it
    // unlikely that this will work
    // TEST_CASE("singular_brauer_monoid(9) (Maltcev-Mazorchuk)",
    //          "[paper][singular_brauer_monoid][n=9]") {
    //            uint64_t size      = 34'096'545;
    //            auto     init_func = [&size](ToddCoxeter& tc) {
    //              tc.lookahead_next(size / 2).lookahead_min(size / 2);
    //            };
    //            benchmark_todd_coxeter_single(size,
    //                                          presentation::examples::singular_brauer_monoid_MM07(9),
    //                                          9,
    //                                          {strategy::hlt},
    //                                          init_func);
    //          }

  }  // namespace singular_brauer

  ////////////////////////////////////////////////////////////////////////
  // stylic_monoid
  ////////////////////////////////////////////////////////////////////////

  namespace stylic {
    sizes_type sizes = {0,
                        2,
                        5,
                        15,
                        52,
                        203,
                        877,
                        4'140,
                        21'147,
                        115'975,
                        678'570,
                        4'213'597,
                        27'644'437};

    auto strategies = {strategy::hlt, strategy::felsch};

    BENCHMARK_TODD_COXETER_RANGE(
        sizes,
        "The presentations for the stylic monoids from \\cite{Abram2021aa}.",
        "table-stylic",
        "\\operatorname{Stylic}(n)",
        3,
        10,
        [](size_t n) { return presentation::examples::stylic_monoid_AR22(n); },
        "stylic_monoid_AR22",
        strategies,
        DoNothing<word_type>);

    // Becomes impractical to do multiple runs after n >= 11, so we switch to
    // doing single runs.

    // Approx 17s (2021 - MacBook Air M1 - 8GB RAM)
    TEST_CASE("stylic_monoid(11) - HLT (default)",
              "[paper][stylic_monoid][n=11][hlt]") {
      benchmark_todd_coxeter_single(
          4'213'597, presentation::examples::stylic_monoid_AR22(11), 11);
    }

    // Approx 153s (2021 - MacBook Air M1 - 8GB RAM)
    TEST_CASE("stylic_monoid(12) - HLT (default)",
              "[paper][stylic_monoid][n=12][hlt]") {
      benchmark_todd_coxeter_single(
          27'644'437,
          presentation::examples::stylic_monoid_AR22(12),
          12,
          {strategy::hlt});
    }

    // Approx ?? (2021 - MacBook Air M1 - 8GB RAM)
    // TODO try implementing lookahead_max, and ensure that lower_bound is
    // used by HLT this currently just spirals off into too many nodes.
    // TEST_CASE("stylic_monoid(13) - HLT (default)",
    //           "[paper][stylic_monoid][n=13][hlt]") {
    //   benchmark_todd_coxeter_single(190'899'322,
    //   presentation::examples::stylic_monoid_AR22(13), 13);
    // }
  }  // namespace stylic

  ////////////////////////////////////////////////////////////////////////
  // stellar_monoid
  ////////////////////////////////////////////////////////////////////////
  namespace stellar {
    sizes_type sizes      = {1,
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
    auto       strategies = {strategy::hlt, strategy::felsch};

    BENCHMARK_TODD_COXETER_RANGE(
        sizes,
        "The presentations for the stellar monoids from \\cite{Gay2019aa}.",
        "table-stellar",
        "\\operatorname{Stellar}(n)",
        3,
        9,
        [](size_t n) { return presentation::examples::stellar_monoid_GH19(n); },
        "stellar_monoid_GH19",
        strategies,
        DoNothing<word_type>);

    // Becomes impractical to do multiple runs after n >= 10, so we switch to
    // doing single runs.
    // Approx 90s (2021 - MacBook Air M1 - 8GB RAM)
    TEST_CASE("stellar_monoid(10) - Felsch (default)",
              "[paper][stellar_monoid][n=10][hlt]") {
      benchmark_todd_coxeter_single(
          9'864'101,
          presentation::examples::stellar_monoid_GH19(10),
          10,
          {strategy::felsch});
    }

    // Approx 22m52s (2021 - MacBook Air M1 - 8GB RAM)
    TEST_CASE("stellar_monoid(11) - Felsch (default)",
              "[paper][stellar_monoid][n=11][hlt]") {
      benchmark_todd_coxeter_single(
          108'505'112,
          presentation::examples::stellar_monoid_GH19(11),
          11,
          {strategy::felsch});
    }
  }  // namespace stellar

  ////////////////////////////////////////////////////////////////////////
  // walker
  ////////////////////////////////////////////////////////////////////////

  namespace walker {
    Presentation<word_type> walker(size_t index) {
      Presentation<std::string> p;
      if (index == 1) {
        p.alphabet("abc");
        presentation::add_rule(p, "a", "a^14"_p);
        presentation::add_rule(p, "b", "b^14"_p);
        presentation::add_rule(p, "c", "c^14"_p);
        presentation::add_rule(p, "bbb", "a^4ba"_p);
        presentation::add_rule(p, "aaa", "b^4ab"_p);
        presentation::add_rule(p, "ccc", "a^4ca"_p);
        presentation::add_rule(p, "aaa", "c^4ac"_p);
        presentation::add_rule(p, "ccc", "b^4cb"_p);
        presentation::add_rule(p, "bbb", "c^4bc"_p);
      } else if (index == 2) {
        p.alphabet("ab");
        presentation::add_rule(p, "a^32"_p, "a");
        presentation::add_rule(p, "bbb", "b");
        presentation::add_rule(p, "ababa", "b");
        presentation::add_rule(p, "a^16ba^4ba^16ba^4"_p, "b");
        presentation::greedy_reduce_length(p);
      } else if (index == 3) {
        p.alphabet("ab");
        presentation::add_rule(p, "aaaaaaaaaaaaaaaa", "a");
        presentation::add_rule(p, "bbbbbbbbbbbbbbbb", "b");
        presentation::add_rule(p, "abb", "baa");
      } else if (index == 4) {
        p.alphabet("ab");
        presentation::add_rule(p, "aaa", "a");
        presentation::add_rule(p, "b^6"_p, "b");
        presentation::add_rule(p, "((ab)^2b^3)^7ab^2a"_p, "bb");
        presentation::greedy_reduce_length(p);
        REQUIRE(presentation::length(p) == 29);
        // REQUIRE(p.alphabet() == "abcd");
        p.rules = std::vector<std::string>({"aaa",
                                            "a",
                                            "dbb",
                                            "b",
                                            "abeceba",
                                            "bb",
                                            "c",
                                            "adab",
                                            "d",
                                            "bbbb",
                                            "ccc",
                                            "e"});
        p.alphabet_from_rules();
      } else if (index == 5) {
        p.alphabet("ab");
        presentation::add_rule(p, "aaa", "a");
        presentation::add_rule(p, "b^6"_p, "b");
        presentation::add_rule(p, "((ab)^2b^3)^7(ab^2)^2b^3a^2"_p, "bb");
        REQUIRE(presentation::length(p) == 73);
        presentation::greedy_reduce_length(p);
        REQUIRE(presentation::length(p) == 34);
        REQUIRE(p.alphabet() == "abcd");
        REQUIRE(p.rules
                == std::vector<std::string>({"aaa",
                                             "a",
                                             "ddd",
                                             "b",
                                             "abc^7bad^2ba^2"_p,
                                             "d",
                                             "c",
                                             "addab",
                                             "d",
                                             "bb"}));
        presentation::replace_word_with_new_generator(p, "ccc");
      } else if (index == 6) {
        p.alphabet("ab");
        presentation::add_rule(p, "aaa", "a");
        presentation::add_rule(p, "b^9"_p, "b");
        presentation::add_rule(p, "((ab)^2b^6)^2(ab^2)^2b^6"_p, "bb");

        REQUIRE(presentation::length(p) == 48);
        presentation::greedy_reduce_length(p);
        REQUIRE(presentation::length(p) == 28);
        REQUIRE(p.alphabet() == "abcde");
        REQUIRE(p.rules
                == std::vector<std::string>({"aaa",
                                             "a",
                                             "cd",
                                             "b",
                                             "aeedacb",
                                             "d",
                                             "c",
                                             "dddb",
                                             "d",
                                             "bb",
                                             "e",
                                             "baca"}));

        presentation::replace_word_with_new_generator(p, "bbb");
        REQUIRE(presentation::length(p) == 32);
      } else if (index == 7) {
        p.alphabet("abcde");
        presentation::add_rule(p, "aaa", "a");
        presentation::add_rule(p, "bbb", "b");
        presentation::add_rule(p, "ccc", "c");
        presentation::add_rule(p, "ddd", "d");
        presentation::add_rule(p, "eee", "e");
        presentation::add_rule(p, "(ab) ^ 3"_p, "aa");
        presentation::add_rule(p, "(bc) ^ 3"_p, "bb");
        presentation::add_rule(p, "(cd) ^ 3"_p, "cc");
        presentation::add_rule(p, "(de) ^ 3"_p, "dd");
        presentation::add_rule(p, "ac", "ca");
        presentation::add_rule(p, "ad", "da");
        presentation::add_rule(p, "ae", "ea");
        presentation::add_rule(p, "bd", "db");
        presentation::add_rule(p, "be", "eb");
        presentation::add_rule(p, "ce", "ec");
      } else if (index == 8) {
        p.alphabet("ab");
        presentation::add_rule(p, "aaa", "a");
        presentation::add_rule(p, "b^23"_p, "b");
        presentation::add_rule(p, "ab^11ab^2"_p, "bba");
      }
      return v4::to<Presentation<word_type>>(p);
    }

    auto       rg = ReportGuard();
    sizes_type sizes
        = {0, 1, 14'911, 20'490, 36'412, 72'822, 78'722, 153'500, 270'272};

    TEST_CASE_TODD_COXETER("Walker") {
      start_table("Comparison of \\libsemigroups and GAP~\\cite{GAP4} "
                  "(semigroups Todd-Coxeter implementation) on examples from "
                  "Walker~\\cite{Walker1992aa}.",
                  "table-walker",
                  "S");

      {
        size_t index = 1;
        auto   p     = walker(index);
        preprocess_presentation(p);
        emit_xml_presentation_tags(p, std::to_string(index), 1);
        auto rg = ReportGuard(false);

        open_xml_tag("LatexColumnTitle", "HLT");
        BENCHMARK("HLT") {
          ToddCoxeter tc(congruence_kind::twosided, p);
          tc.strategy(strategy::hlt)
              .lookahead_next(500'000)
              .large_collapse(2'000);
          REQUIRE(tc.number_of_classes() == 1);
        };
        close_xml_tag("LatexColumnTitle");
        presentation::greedy_reduce_length(p);
        open_xml_tag("LatexColumnTitle", "Felsch");
        BENCHMARK("Felsch") {
          ToddCoxeter tc(congruence_kind::twosided, p);
          tc.strategy(strategy::felsch).use_relations_in_extra(true);
          REQUIRE(tc.number_of_classes() == 1);
        };
        close_xml_tag("LatexColumnTitle");
      }
      {
        size_t index = 2;
        auto   p     = walker(index);
        preprocess_presentation(p);
        emit_xml_presentation_tags(p, index, 14'911);
        auto rg = ReportGuard(false);

        open_xml_tag("LatexColumnTitle", "HLT");
        BENCHMARK("HLT") {
          ToddCoxeter tc(congruence_kind::twosided, p);
          tc.strategy(strategy::hlt)
              .use_relations_in_extra(true)
              .lookahead_next(2'000'000);
          REQUIRE(tc.number_of_classes() == 14'911);
        };
        close_xml_tag("LatexColumnTitle");
        open_xml_tag("LatexColumnTitle", "Felsch");
        BENCHMARK("Felsch") {
          ToddCoxeter tc(congruence_kind::twosided, p);
          tc.strategy(strategy::felsch);
          REQUIRE(tc.number_of_classes() == 14'911);
        };
        close_xml_tag("LatexColumnTitle");
      }
      {
        size_t index = 3;
        auto   p     = walker(index);
        preprocess_presentation(p);
        emit_xml_presentation_tags(p, index, 20'490);
        auto rg = ReportGuard(false);

        open_xml_tag("LatexColumnTitle", "HLT");
        BENCHMARK("HLT") {
          ToddCoxeter tc(congruence_kind::twosided, p);
          tc.strategy(strategy::hlt).lookahead_next(2'000'000);
          REQUIRE(tc.number_of_classes() == 20'490);
        };
        close_xml_tag("LatexColumnTitle");
        open_xml_tag("LatexColumnTitle", "Felsch");
        BENCHMARK("Felsch") {
          ToddCoxeter tc(congruence_kind::twosided, p);
          tc.strategy(strategy::felsch)
              .use_relations_in_extra(true)
              .def_max(100'000)
              .def_version(def_version::one)
              .def_policy(def_policy::no_stack_if_no_space);
          REQUIRE(tc.number_of_classes() == 20'490);
        };
        close_xml_tag("LatexColumnTitle");
      }
      {
        size_t index = 4;
        size_t N     = 36'412;
        auto   p     = walker(index);
        preprocess_presentation(p);
        emit_xml_presentation_tags(p, index, N);
        auto rg = ReportGuard(false);

        open_xml_tag("LatexColumnTitle", "HLT");
        BENCHMARK("HLT") {
          ToddCoxeter tc(congruence_kind::twosided, p);
          tc.strategy(strategy::hlt).lookahead_next(3'000'000);
          REQUIRE(tc.number_of_classes() == N);
        };
        close_xml_tag("LatexColumnTitle");
        open_xml_tag("LatexColumnTitle", "Felsch");
        BENCHMARK("Felsch") {
          ToddCoxeter tc(congruence_kind::twosided, p);
          tc.strategy(strategy::felsch)
              .use_relations_in_extra(true)
              .def_max(10'000)
              .large_collapse(3'000);
          REQUIRE(tc.number_of_classes() == N);
        };
        close_xml_tag("LatexColumnTitle");
      }
      {
        size_t index = 5;
        size_t N     = 72'822;
        auto   p     = walker(index);
        emit_xml_presentation_tags(p, index, N);
        auto rg = ReportGuard(false);

        open_xml_tag("LatexColumnTitle", "HLT");
        BENCHMARK("HLT") {
          ToddCoxeter tc(congruence_kind::twosided, p);
          tc.strategy(strategy::hlt).lookahead_next(5'000'000).save(true);
          REQUIRE(tc.number_of_classes() == N);
        };
        BENCHMARK("Felsch"){
            // This is intentionally empty so that all the columns have the same
            // values.
        };
        close_xml_tag("LatexColumnTitle");
      }
      {
        size_t index = 6;
        size_t N     = 78'722;
        auto   p     = walker(index);
        preprocess_presentation(p);
        emit_xml_presentation_tags(p, index, N);
        auto rg = ReportGuard(false);

        open_xml_tag("LatexColumnTitle", "HLT");
        BENCHMARK("HLT") {
          ToddCoxeter tc(congruence_kind::twosided, p);
          tc.strategy(strategy::hlt).lookahead_next(5'000'000).save(true);
          REQUIRE(tc.number_of_classes() == N);
        };
        close_xml_tag("LatexColumnTitle");
        open_xml_tag("LatexColumnTitle", "Felsch");
        BENCHMARK("Felsch") {
          ToddCoxeter tc(congruence_kind::twosided, p);
          tc.strategy(strategy::felsch).use_relations_in_extra(true);
          REQUIRE(tc.number_of_classes() == N);
        };
        close_xml_tag("LatexColumnTitle");
      }
      {
        size_t index = 7;
        size_t N     = 153'500;
        auto   p     = walker(index);
        preprocess_presentation(p);
        emit_xml_presentation_tags(p, index, N);
        auto rg = ReportGuard(false);

        open_xml_tag("LatexColumnTitle", "HLT");
        BENCHMARK("HLT") {
          ToddCoxeter tc(congruence_kind::twosided, p);
          tc.strategy(strategy::hlt);
          REQUIRE(tc.number_of_classes() == N);
        };
        close_xml_tag("LatexColumnTitle");
        presentation::greedy_reduce_length(p);
        open_xml_tag("LatexColumnTitle", "Felsch");
        BENCHMARK("Felsch") {
          ToddCoxeter tc(congruence_kind::twosided, p);
          tc.strategy(strategy::felsch)
              .def_version(def_version::one)
              .use_relations_in_extra(true);
          REQUIRE(tc.number_of_classes() == N);
        };
        close_xml_tag("LatexColumnTitle");
      }
      {
        size_t index = 8;
        size_t N     = 270'272;
        auto   p     = walker(index);
        preprocess_presentation(p);
        emit_xml_presentation_tags(p, index, N);
        auto rg = ReportGuard(false);

        open_xml_tag("LatexColumnTitle", "HLT");
        BENCHMARK("HLT") {
          ToddCoxeter tc(congruence_kind::twosided, p);
          tc.strategy(strategy::hlt).lookahead_next(500'000);
          REQUIRE(tc.number_of_classes() == N);
        };
        close_xml_tag("LatexColumnTitle");
        presentation::greedy_reduce_length_and_number_of_gens(p);
        open_xml_tag("LatexColumnTitle", "Felsch");
        BENCHMARK("Felsch") {
          ToddCoxeter tc(congruence_kind::twosided, p);
          tc.strategy(strategy::felsch).use_relations_in_extra(true);
          REQUIRE(tc.number_of_classes() == N);
        };
        close_xml_tag("LatexColumnTitle");
      }
    }
  }  // namespace walker

  namespace ace {

    TEST_CASE("ACE --- table header", "[paper][ace]") {
      start_table("Comparison of \\libsemigroups, ACE~\\cite{Havas1999aa}, and "
                  "GAP~\\cite{GAP4}.",
                  "table-ace",
                  "|G:H|");
    }

    TEST_CASE("ACE --- 2p17-2p14", "[paper][ace][2p17-2p14]") {
      Presentation<std::string> p;
      p.alphabet("abcABC");
      p.contains_empty_word(true);
      std::string inverses = "ABCabc";
      presentation::add_inverse_rules(p, inverses);
      presentation::add_rule(p, "aBCbac", "");
      presentation::add_rule(p, "bACbaacA", "");
      presentation::add_rule(p, "accAABab", "");

      emit_xml_presentation_tags(p, "2p17-2p14", 16'384);

      open_xml_tag("LatexColumnTitle", "HLT");
      BENCHMARK("HLT") {
        ToddCoxeter H(congruence_kind::onesided, p);
        todd_coxeter::add_generating_pair(H, "bc", "");
        H.lookahead_next(1'000'000).lookahead_extent(lookahead_extent::partial);
        REQUIRE(H.number_of_classes() == 16'384);
      };
      close_xml_tag("LatexColumnTitle");
      // About 2s
      // open_xml_tag("LatexColumnTitle", "Felsch");
      // BENCHMARK("Felsch") {
      //  ToddCoxeter H(congruence_kind::onesided, p);
      //  todd_coxeter::add_generating_pair(H, "bc", "");
      //  H.strategy(strategy::felsch).def_max(100'000);
      //  REQUIRE(H.number_of_classes() == 16'384);
      //};
    }

    TEST_CASE("ACE --- 2p17-2p3", "[paper][ace][2p17-2p3]") {
      auto                      rg = ReportGuard(false);
      Presentation<std::string> p;
      p.contains_empty_word(true);
      p.alphabet("abcABC");
      presentation::add_inverse_rules(p, "ABCabc");
      presentation::add_rule(p, "aBCbac", "");
      presentation::add_rule(p, "bACbaacA", "");
      presentation::add_rule(p, "accAABab", "");

      emit_xml_presentation_tags(p, "2p17-2p3", 8);

      open_xml_tag("LatexColumnTitle", "HLT");
      BENCHMARK("HLT") {
        ToddCoxeter H(congruence_kind::onesided, p);
        todd_coxeter::add_generating_pair(H, "bc", "");
        todd_coxeter::add_generating_pair(H, "ABAAb", "cBAC");

        H.strategy(strategy::hlt).save(true).def_max(100'000);

        REQUIRE(H.number_of_classes() == 8);
      };
      close_xml_tag("LatexColumnTitle");
      // About 2s
      // open_xml_tag("LatexColumnTitle", "Felsch");
      // BENCHMARK("Felsch") {
      //   ToddCoxeter H(congruence_kind::onesided, p);
      //   todd_coxeter::add_generating_pair(H, {b, c}, {});
      //   todd_coxeter::add_generating_pair(H, {A, B, A, A, b, c, a, b, C},
      //   {});

      //   H.strategy(strategy::felsch);

      //   REQUIRE(H.number_of_classes() == 8);
      // };
    }

    TEST_CASE("ACE --- 2p17-fel1", "[paper][ace][2p17-1]") {
      auto                      rg = ReportGuard(false);
      Presentation<std::string> p;
      p.alphabet("abcABC");
      p.contains_empty_word(true);
      presentation::add_inverse_rules(p, "ABCabc");
      presentation::add_rule(p, "aBCbac", "");
      presentation::add_rule(p, "bACbaacA", "");
      presentation::add_rule(p, "accAABab", "");

      presentation::remove_duplicate_rules(p);

      emit_xml_presentation_tags(p, "2p17-fel1", 131'072);

      open_xml_tag("LatexColumnTitle", "HLT");
      BENCHMARK("HLT") {
        ToddCoxeter H(congruence_kind::onesided, p);
        todd_coxeter::add_generating_pair(H, "", "aBCbac");
        todd_coxeter::add_generating_pair(H, "bACbaacA", "");
        todd_coxeter::add_generating_pair(H, "accAABab", "");

        H.save(true).def_max(20'000).large_collapse(10'000);
        REQUIRE(H.number_of_classes() == 131'072);
      };
      close_xml_tag("LatexColumnTitle");
    }

    TEST_CASE("ACE --- 2p17-fel1a", "[paper][ace][2p17-1a]") {
      auto                      rg = ReportGuard(false);
      Presentation<std::string> p;
      p.alphabet("abcABC");
      p.contains_empty_word(true);
      presentation::add_inverse_rules(p, "ABCabc");
      presentation::add_rule(p, "aBCbac", "");
      presentation::add_rule(p, "bACbaacA", "");
      presentation::add_rule(p, "accAABab", "");

      emit_xml_presentation_tags(p, "2p17-fel1a", 1);

      open_xml_tag("LatexColumnTitle", "HLT");
      BENCHMARK("HLT") {
        ToddCoxeter H(congruence_kind::onesided, p);
        todd_coxeter::add_generating_pair(H, "bc", "");
        todd_coxeter::add_generating_pair(H, "ABAAbcabC", "");
        todd_coxeter::add_generating_pair(H, "AcccacBcA", "");

        H.strategy(strategy::hlt)
            .save(true)
            .lookahead_extent(lookahead_extent::full)
            .def_max(10'000)
            .large_collapse(10'000);
        REQUIRE(H.number_of_classes() == 1);
      };
      close_xml_tag("LatexColumnTitle");
    }

    TEST_CASE("ACE --- 2p17-id-fel1", "[paper][ace][2p17-id]") {
      auto                      rg = ReportGuard(false);
      Presentation<std::string> p;
      p.alphabet("abcABC");
      p.contains_empty_word(true);
      presentation::add_inverse_rules(p, "ABCabc");
      presentation::add_rule(p, "aBCbac", "");
      presentation::add_rule(p, "bACbaacA", "");
      presentation::add_rule(p, "accAABab", "");

      emit_xml_presentation_tags(p, "2p17-id-fel1", 131'072);
      open_xml_tag("LatexColumnTitle", "HLT");
      BENCHMARK("HLT") {
        ToddCoxeter tc(congruence_kind::twosided, p);
        tc.strategy(strategy::hlt)
            .lookahead_extent(lookahead_extent::partial)
            .save(true)
            .def_max(POSITIVE_INFINITY);

        REQUIRE(tc.number_of_classes() == std::pow(2, 17));
      };
      close_xml_tag("LatexColumnTitle");
    }

    TEST_CASE("ACE --- 2p18-fe1", "[paper][ace][2p18]") {
      auto                      rg = ReportGuard(false);
      Presentation<std::string> p;
      p.alphabet("abcABCx");
      p.contains_empty_word(true);
      presentation::add_inverse_rules(p, "ABCabcx");
      presentation::add_rule(p, "aBCbac", "");
      presentation::add_rule(p, "bACbaacA", "");
      presentation::add_rule(p, "accAABab", "");
      presentation::add_rule(p, "xx", "");
      presentation::add_rule(p, "Axax", "");
      presentation::add_rule(p, "Bxbx", "");
      presentation::add_rule(p, "Cxcx", "");

      emit_xml_presentation_tags(p, "2p18-fe1", 262'144);

      open_xml_tag("LatexColumnTitle", "HLT");
      BENCHMARK("HLT") {
        ToddCoxeter H(congruence_kind::onesided, p);
        todd_coxeter::add_generating_pair(H, "aBCbac", "");
        todd_coxeter::add_generating_pair(H, "bACbaacA", "");
        todd_coxeter::add_generating_pair(H, "accAABab", "");

        H.strategy(strategy::hlt)
            .save(true)
            .large_collapse(10'000)
            .def_max(10'000)
            .lookahead_extent(lookahead_extent::partial)
            .lookahead_next(5'000'000);

        REQUIRE(H.number_of_classes() == 262'144);
      };
      close_xml_tag("LatexColumnTitle");
    }

    TEST_CASE("ACE --- F27", "[paper][ace][F27]") {
      auto                      rg = ReportGuard(false);
      Presentation<std::string> p;
      p.alphabet("abcdxyzABCDXYZ");
      p.contains_empty_word(true);
      presentation::add_inverse_rules(p, "ABCDXYZabcdxyz");
      presentation::add_rule(p, "ab", "c");
      presentation::add_rule(p, "bc", "d");
      presentation::add_rule(p, "cd", "x");
      presentation::add_rule(p, "dx", "y");
      presentation::add_rule(p, "xy", "z");
      presentation::add_rule(p, "yz", "a");
      presentation::add_rule(p, "za", "b");
      emit_xml_presentation_tags(p, "F27", 29);
      open_xml_tag("LatexColumnTitle", "HLT");
      BENCHMARK("HLT") {
        ToddCoxeter tc(congruence_kind::twosided, p);
        tc.strategy(strategy::hlt)
            .save(true)
            .lookahead_extent(lookahead_extent::partial);
        REQUIRE(tc.number_of_classes() == 29);
      };
      close_xml_tag("LatexColumnTitle");
    }

    TEST_CASE("ACE --- M12", "[paper][ace][M12]") {
      auto rg = ReportGuard(false);

      Presentation<std::string> p;
      p.alphabet("abcABC");
      p.contains_empty_word(true);
      presentation::add_inverse_rules(p, "ABCabc");
      presentation::add_rule(p, "bb", "");
      presentation::add_rule(p, "cc", "");
      presentation::add_rule(p, "ababab", "");
      presentation::add_rule(p, "acacac", "");
      presentation::add_rule(p, "aaaaaaaaaaa", "");
      presentation::add_rule(p, "cbcbabcbc", "aaaaa");
      presentation::add_rule(p, "bcbcbcbcbcbcbcbcbcbc", "");
      presentation::replace_word_with_new_generator(
          p, presentation::longest_subword_reducing_length(p));
      emit_xml_presentation_tags(p, "M12", 95'040);

      open_xml_tag("LatexColumnTitle", "HLT");
      BENCHMARK("HLT") {
        ToddCoxeter H(congruence_kind::twosided, p);

        H.strategy(strategy::hlt)
            .save(true)
            .lookahead_extent(lookahead_extent::partial);

        REQUIRE(H.number_of_classes() == 95'040);
      };
      close_xml_tag("LatexColumnTitle");
    }

    TEST_CASE("ACE --- SL(2, 19)", "[paper][ace][SL219]") {
      auto                      rg = ReportGuard(false);
      Presentation<std::string> p;
      p.alphabet("abAB");
      p.contains_empty_word(true);
      presentation::add_inverse_rules(p, "ABab");
      presentation::add_rule(p, "aBABAB", "");
      presentation::add_rule(p, "BAAbaa", "");
      presentation::add_rule(
          p,
          "abbbbabbbbbbbbbbabbbbabbbbbbbbbbbbbbbbbbbbbbbbbbbbbaaaaaaaaaaaa",
          "");
      presentation::balance_no_checks(p, "abAB"s, "ABab"s);
      presentation::sort_rules(p);

      emit_xml_presentation_tags(p, "SL219", 180);

      open_xml_tag("LatexColumnTitle", "HLT");
      BENCHMARK("HLT") {
        ToddCoxeter H(congruence_kind::onesided, p);
        todd_coxeter::add_generating_pair(H, "b", "");

        H.strategy(strategy::hlt)
            .save(false)
            .lookahead_extent(lookahead_extent::partial)
            .lookahead_next(500'000);
        REQUIRE(H.number_of_classes() == 180);
      };
      close_xml_tag("LatexColumnTitle");
    }

    TEST_CASE("ACE --- big-hard", "[paper][ace][big-hard]") {
      auto                      rg = ReportGuard(false);
      Presentation<std::string> p;
      p.alphabet("abcyABCYx");
      p.contains_empty_word(true);
      presentation::add_inverse_rules(p, "ABCYabcyx");
      presentation::add_rule(p, "aBCbac", "");
      presentation::add_rule(p, "bACbaacA", "");
      presentation::add_rule(p, "accAABab", "");
      presentation::add_rule(p, "xx", "");
      presentation::add_rule(p, "yyy", "");
      presentation::add_rule(p, "Axax", "");
      presentation::add_rule(p, "Bxbx", "");
      presentation::add_rule(p, "Cxcx", "");
      presentation::add_rule(p, "AYay", "");
      presentation::add_rule(p, "BYby", "");
      presentation::add_rule(p, "CYcy", "");
      presentation::add_rule(p, "xYxy", "");

      emit_xml_presentation_tags(p, "big-hard", 786'432);

      open_xml_tag("LatexColumnTitle", "HLT");
      BENCHMARK("HLT") {
        ToddCoxeter H(congruence_kind::onesided, p);
        todd_coxeter::add_generating_pair(H, "aBCbac", "");
        todd_coxeter::add_generating_pair(H, "bACbaacA", "");
        todd_coxeter::add_generating_pair(H, "accAABab", "");
        H.strategy(strategy::hlt)
            .save(true)
            .lookahead_extent(lookahead_extent::partial)
            .lookahead_next(1'000'000)
            .large_collapse(5'000)
            .def_max(1'000'000)
            .lower_bound(786'432);
        REQUIRE(H.number_of_classes() == 786'432);
      };
      close_xml_tag("LatexColumnTitle");
    }
  }  // namespace ace
  //
}  // namespace libsemigroups

// Unused!
//    TEST_CASE("ACE --- g25.a", "[paper][ace][g25.a]") {
//      auto        rg = ReportGuard(true);
//      Presentation<std::string> p;
//      p.alphabet("abcdeABCDx");
//      G.set_identity("x");
//      presentation::add_inverse_rules(p, "ABCDeabcdx");
//
//      presentation::add_rule(p, "ee", "x");
//      presentation::add_rule(p, "DaDa", "x");
//      presentation::add_rule(p, "dddd", "x");
//      presentation::add_rule(p, "BDbd", "x");
//      presentation::add_rule(p, "ccccc", "x");
//      presentation::add_rule(p, "bbbbb", "x");
//      presentation::add_rule(p, "AABaab", "x");
//      presentation::add_rule(p, "ddAAAA", "x");
//      presentation::add_rule(p, "AAcaacc", "x");
//      presentation::add_rule(p, "ececBBC", "x");
//      presentation::add_rule(p, "abababab", "x");
//      presentation::add_rule(p, "BBcbceceBCC", "x");
//      presentation::add_rule(p, "ebcBebCBBcB", "x");
//      presentation::add_rule(p, "ebebccBBcbC", "x");
//      presentation::add_rule(p, "ACabCBAcabcB", "x");
//      presentation::add_rule(p, "ABabABabABab", "x");
//      presentation::add_rule(p, "CACaCaCAccaCA", "x");
//      presentation::add_rule(p, "ABcbabCBCBccb", "x");
//      presentation::add_rule(p, "BCbcACaCBcbAca", "x");
//      presentation::add_rule(p, "eabbaBAeabbaBA", "x");
//      presentation::add_rule(p, "eBcbeabcBcACBB", "x");
//      presentation::add_rule(p, "BCbAcaBcbCACac", "x");
//      presentation::add_rule(p, "CACacaCAcACaCACa", "x");
//      presentation::add_rule(p, "CAcacbcBCACacbCB", "x");
//      presentation::add_rule(p, "CaCAcacAcaCACacA", "x");
//      presentation::add_rule(p, "cacbcBACCaCbCBAc", "x");
//      presentation::add_rule(p, "CBCbcBcbCCACACaca", "x");
//      presentation::add_rule(p, "BAcabbcBcbeCebACa", "x");
//      presentation::add_rule(p, "ACacAcaebcBCBcBCe", "x");
//      presentation::add_rule(p, "eDCDbABCDACaCAcabb", "x");
//      presentation::add_rule(p, "BCbbCBBcbbACacAcaB", "x");
//      presentation::add_rule(p, "eaaebcBACaCAcbABBA", "x");
//      presentation::add_rule(p, "BACaCAcacbCACacAca", "x");
//      presentation::add_rule(p, "AbcBabCBCbCBBcbAcaC", "x");
//      presentation::add_rule(p, "aabaBabaabaBabaabaBab", "x");
//      presentation::add_rule(p, "eAcaeACaeAcabCBaBcbaaa", "x");
//      presentation::add_rule(p, "deBAceAeACACacAcabcBcbaBBA", "x");
//      presentation::add_rule(p, "dCACacAcadACaCAcacdCACacAcA", "x");
//      presentation::add_rule(p,
//  "dCACacAcadCACacAcadCACacAcadCACacAcadCACacAcadCACacAca", "x");
//
//      p
//          .remove_duplicate_generating_pairs()
//          .sort_generating_pairs();
//      REQUIRE(tc.number_of_classes() == 1);
//      std::cout << p.stats_string();
//    }
