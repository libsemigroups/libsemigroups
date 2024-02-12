//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2020-24 James D. Mitchell
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

#include "catch.hpp"  // for REQUIRE, REQUIRE_NOTHROW, REQUIRE_THROWS_AS

#include "libsemigroups/fpsemi-examples.hpp"
#include "libsemigroups/presentation.hpp"
#include "libsemigroups/todd-coxeter.hpp"  // for ToddCoxeter, ToddCoxeter::...
#include "libsemigroups/types.hpp"         // for word_type, letter_type
#include "libsemigroups/words.hpp"         // for operator""_p

#include "libsemigroups/detail/report.hpp"  // for ReportGuard
//
namespace libsemigroups {
  using literals::operator""_p;
  using strategy         = ToddCoxeter::options::strategy;
  using lookahead_extent = ToddCoxeter::options::lookahead_extent;

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

    void preprocess_presentation(Presentation<word_type>& p) {
      presentation::remove_redundant_generators(p);
      presentation::reduce_complements(p);
      presentation::remove_trivial_rules(p);
      presentation::remove_duplicate_rules(p);
      presentation::normalize_alphabet(p);
      presentation::sort_each_rule(p);
      presentation::sort_rules(p);
    }

    void emit_xml_presentation_tags(Presentation<word_type>& p,
                                    size_t                   index,
                                    size_t                   size) {
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

    constexpr auto DoNothing = [](ToddCoxeter&) {};

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
        ToddCoxeter tc(congruence_kind::twosided, p);
        auto        title = fmt::format("{}", strategy);
        open_xml_tag("LatexColumnTitle", title);
        SECTION(title) {
          ToddCoxeter tc(congruence_kind::twosided, p);
          tc.strategy(strategy);
          init(tc);
          tc.strategy(strategy::hlt);
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
      benchmark_todd_coxeter_single<decltype(DoNothing) const&>(
          size, std::move(p), n, strategies, DoNothing);
    }

    // .lookahead_next(20'000'000)
    // .lookahead_min(10'000'000);
    // while (!tc.finished()) {  // TODO reporting doesn't work
    //   tc.run_for(std::chrono::seconds(10));
    //   tc.standardize(Order::shortlex);
    // }
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
        emit_xml_presentation_tags(p, n, sizes[n]);
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

  }  // namespace

  using fpsemigroup::dual_symmetric_inverse_monoid;
  using fpsemigroup::orientation_preserving_monoid;
  using fpsemigroup::orientation_reversing_monoid;
  using fpsemigroup::partition_monoid;
  using fpsemigroup::singular_brauer_monoid;
  using fpsemigroup::stellar_monoid;
  using fpsemigroup::stylic_monoid;
  using fpsemigroup::temperley_lieb_monoid;
  using fpsemigroup::uniform_block_bijection_monoid;

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
        orientation_preserving_monoid,
        "orientation_preserving_monoid",
        strategies,
        DoNothing);
  }  // namespace orientation_preserving

  // Becomes impractical to do multiple runs when n >= 10, so we switch to
  // doing single runs.
  // Approx 27s (2021 - MacBook Air M1 - 8GB RAM)
  TEST_CASE("orientation_preserving_monoid(n) (Arthur-Ruskuc), n = 10",
            "[paper][orientation_preserving_monoid][n=10][hlt]") {
    benchmark_todd_coxeter_single(
        923'690, orientation_preserving_monoid(10), 10);
  }

  // 4m13s (2021 - MacBook Air M1 - 8GB RAM)
  TEST_CASE("orientation_preserving_monoid(n) (Arthur-Ruskuc), n = 11",
            "[paper][orientation_preserving_monoid][n=11][hlt]") {
    benchmark_todd_coxeter_single(
        3'879'766, orientation_preserving_monoid(11), 11);
  }

  // 54m35s (2021 - MacBook Air M1 - 8GB RAM)
  TEST_CASE("orientation_preserving_monoid(n) (Arthur-Ruskuc), n = 12",
            "[paper][orientation_preserving_monoid][n=12][hlt]") {
    benchmark_todd_coxeter_single(
        16'224'804, orientation_preserving_monoid(12), 12);
  }

  // 9h14m (2021 - MacBook Air M1 - 8GB RAM)
  TEST_CASE("orientation_preserving_monoid(n) (Arthur-Ruskuc), n = 13",
            "[paper][orientation_preserving_monoid][n=13][hlt]") {
    benchmark_todd_coxeter_single(
        67'603'744, orientation_preserving_monoid(13), 13);
  }

  ////////////////////////////////////////////////////////////////////////
  // 2. orientation_reversing_monoid
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
        orientation_reversing_monoid,
        "orientation_reversing_monoid",
        strategies,
        DoNothing);
  }  // namespace orientation_reversing

  // Approx 9s (2021 - MacBook Air M1 - 8GB RAM)
  TEST_CASE("orientation_reversing_monoid(9) - hlt",
            "[paper][orientation_reversing_monoid][n=9][hlt]") {
    benchmark_todd_coxeter_single(434'835, orientation_reversing_monoid(9), 9);
  }

  // Approx 90s (2021 - MacBook Air M1 - 8GB RAM)
  TEST_CASE("orientation_reversing_monoid(10) - hlt",
            "[paper][orientation_reversing_monoid][n=10][hlt]") {
    benchmark_todd_coxeter_single(
        1'843'320, orientation_reversing_monoid(10), 10);
  }

  // ?? (2021 - MacBook Air M1 - 8GB RAM)
  TEST_CASE("orientation_reversing_monoid(11) - hlt",
            "[paper][orientation_reversing_monoid][n=11][hlt]") {
    benchmark_todd_coxeter_single(
        7'753'471, orientation_reversing_monoid(11), 11);
  }

  // ?? (2021 - MacBook Air M1 - 8GB RAM)
  TEST_CASE("orientation_reversing_monoid(12) - hlt",
            "[paper][orientation_reversing_monoid][n=12][hlt]") {
    benchmark_todd_coxeter_single(
        32'440'884, orientation_reversing_monoid(12), 12);
  }

  // ?? (2021 - MacBook Air M1 - 8GB RAM)
  TEST_CASE("orientation_reversing_monoid(13) - hlt",
            "[paper][orientation_reversing_monoid][n=13][hlt]") {
    benchmark_todd_coxeter_single(
        135'195'307, orientation_reversing_monoid(13), 13);
  }

  ////////////////////////////////////////////////////////////////////////
  // partition_monoid
  ////////////////////////////////////////////////////////////////////////

  namespace partition {
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
        [](size_t n) { return partition_monoid(n); },
        "partition_monoid",
        strategies,
        DoNothing);

    // Becomes impractical to do multiple runs for n >= 6, so we switch to
    // doing single runs.

    auto init_func = [](ToddCoxeter& tc) {
      tc.use_relations_in_extra(true)
          .lookahead_next(200'000)
          .lookahead_growth_factor(2.5);
      // .reserve(15'000'000);
    };

    // Approx 31s (2021 - MacBook Air M1 - 8GB RAM)
    TEST_CASE("partition_monoid(6) - hlt",
              "[paper][partition_monoid][n=6][hlt]") {
      benchmark_todd_coxeter_single(
          4'213'597, partition_monoid(6), 6, {strategy::hlt}, init_func);
    }

    // Approx 49m35s ??
    TEST_CASE("partition_monoid(7) - hlt",
              "[paper][partition_monoid][n=7][hlt]") {
      benchmark_todd_coxeter_single(
          190'899'322, partition_monoid(7), 7, {strategy::hlt}, init_func);
    }

  }  // namespace partition

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
        [](size_t n) { return dual_symmetric_inverse_monoid(n); },
        "dual_symmetric_inverse_monoid",
        strategies,
        DoNothing);

    // Becomes impractical to do multiple runs for n >= 7, so we switch to
    // doing single runs.

    TEST_CASE("dual_symmetric_inverse_monoid(7)",
              "[paper][dual_symmetric_inverse_monoid][n=7][Felsch]") {
      benchmark_todd_coxeter_single(
          6'166'105,
          dual_symmetric_inverse_monoid(7),
          7,
          {strategy::hlt, strategy::felsch, strategy::Rc});
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
        "sometimes called the \textit{uniform block bijection monoid}.",
        "table-uniform",
        "FI_n^*",
        3,
        7,
        [](size_t n) { return uniform_block_bijection_monoid(n); },
        "uniform_block_bijection_monoid",
        strategies,
        [](ToddCoxeter& tc) {
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
          uniform_block_bijection_monoid(8),
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
        temperley_lieb_monoid,
        "temperley_lieb_monoid",
        strategies,
        DoNothing);

    // Becomes impractical to do multiple runs after n >= 15, so we switch to
    // doing single runs.

    // Approx. 18s (2021 - MacBook Air M1 - 8GB RAM)
    TEST_CASE("temperley_lieb_monoid(15) - hlt",
              "[paper][temperley_lieb_monoid][n=15][hlt]") {
      benchmark_todd_coxeter_single(9'694'845, temperley_lieb_monoid(15), 15);
    }

    // Approx. 82s (2021 - MacBook Air M1 - 8GB RAM)
    TEST_CASE("temperley_lieb_monoid(16) - hlt",
              "[paper][temperley_lieb_monoid][n=16][hlt]") {
      benchmark_todd_coxeter_single(35'357'670, temperley_lieb_monoid(16), 16);
    }

    // Approx. ? (2021 - MacBook Air M1 - 8GB RAM)
    // TEST_CASE("temperley_lieb_monoid(17) - hlt",
    //           "[paper][temperley_lieb_monoid][n=17][hlt]") {
    //   benchmark_todd_coxeter_single(129'644'790, temperley_lieb_monoid(17),
    //   17);
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
        singular_brauer_monoid,
        "singular_brauer_monoid",
        strategies,
        DoNothing);

    // Approx. 1 minute
    // TODO lower bound has no impact here, because HLT doesn't check for this,
    // maybe it should? When there are no nodes defined or killed in some
    // interval of time, then we should check if we're already complete and
    // compatible, and if the lower_bound() == number_of_active_nodes
    TEST_CASE("singular_brauer_monoid(8) (Maltcev-Mazorchuk)",
              "[paper][singular_brauer_monoid][n=8]") {
      uint64_t size      = 1'986'705;
      auto     init_func = [&size](ToddCoxeter& tc) {
        tc.lookahead_next(size / 2).lookahead_min(size / 2).lower_bound(size);
      };
      benchmark_todd_coxeter_single(
          size, singular_brauer_monoid(8), 8, {strategy::hlt}, init_func);
    }

    // Approx. ? Running this appears to use >27gb of memory, which JDM's
    // laptop does not have.
    // TEST_CASE("singular_brauer_monoid(9) (Maltcev-Mazorchuk)",
    //          "[paper][singular_brauer_monoid][n=9]") {
    //            uint64_t size      = 34'096'545;
    //            auto     init_func = [&size](ToddCoxeter& tc) {
    //              tc.lookahead_next(size / 2).lookahead_min(size / 2);
    //            };
    //            benchmark_todd_coxeter_single(size,
    //                                          singular_brauer_monoid(9),
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
        stylic_monoid,
        "stylic_monoid",
        strategies,
        DoNothing);

  }  // namespace stylic

  // Becomes impractical to do multiple runs after n >= 11, so we switch to
  // doing single runs.

  // Approx 17s (2021 - MacBook Air M1 - 8GB RAM)
  TEST_CASE("stylic_monoid(11) - HLT (default)",
            "[paper][stylic_monoid][n=11][hlt]") {
    auto p = stylic_monoid(11);
    benchmark_todd_coxeter_single(4'213'597, stylic_monoid(11), 11);
  }

  // Approx 153s (2021 - MacBook Air M1 - 8GB RAM)
  TEST_CASE("stylic_monoid(12) - HLT (default)",
            "[paper][stylic_monoid][n=12][hlt]") {
    benchmark_todd_coxeter_single(27'644'437, stylic_monoid(12), 12);
  }

  // Approx ?? (2021 - MacBook Air M1 - 8GB RAM)
  // TODO try implementing lookahead_max, and ensure that lower_bound is used by
  // HLT this currently just spirals off into too many nodes.
  // TEST_CASE("stylic_monoid(13) - HLT (default)",
  //           "[paper][stylic_monoid][n=13][hlt]") {
  //   benchmark_todd_coxeter_single(190'899'322, stylic_monoid(13), 13);
  // }

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
        stellar_monoid,
        "stellar_monoid",
        strategies,
        DoNothing);

    // Becomes impractical to do multiple runs after n >= 10, so we switch to
    // doing single runs.
    // Approx 90s (2021 - MacBook Air M1 - 8GB RAM)
    TEST_CASE("stellar_monoid(10) - Felsch (default)",
              "[paper][stellar_monoid][n=10][hlt]") {
      benchmark_todd_coxeter_single(
          9'864'101, stellar_monoid(10), 10, {strategy::felsch});
    }

    // Approx 22m52s (2021 - MacBook Air M1 - 8GB RAM)
    TEST_CASE("stellar_monoid(11) - Felsch (default)",
              "[paper][stellar_monoid][n=11][hlt]") {
      benchmark_todd_coxeter_single(
          108'505'112, stylic_monoid(11), 11, {strategy::felsch});
    }
  }  // namespace stellar

  ////////////////////////////////////////////////////////////////////////
  // walker
  ////////////////////////////////////////////////////////////////////////
  namespace walker {
    Presentation<word_type> walker(size_t index) {
      Presentation<std::string> p;
      if (index == 1) {
        p.alphabet("abcABCDEFGHIXYZ");
        presentation::add_rule(p, "A", "a^14"_p);
        presentation::add_rule(p, "B", "b^14"_p);
        presentation::add_rule(p, "C", "c^14"_p);
        presentation::add_rule(p, "D", "a^4ba"_p);
        presentation::add_rule(p, "E", "b^4ab"_p);
        presentation::add_rule(p, "F", "a^4ca"_p);
        presentation::add_rule(p, "G", "c^4ac"_p);
        presentation::add_rule(p, "H", "b^4cb"_p);
        presentation::add_rule(p, "I", "c^4bc"_p);
        presentation::add_rule(p, "X", "aaa");
        presentation::add_rule(p, "Y", "bbb");
        presentation::add_rule(p, "Z", "ccc");

        presentation::add_rule(p, "A", "a");
        presentation::add_rule(p, "B", "b");
        presentation::add_rule(p, "C", "c");
        presentation::add_rule(p, "D", "Y");
        presentation::add_rule(p, "E", "X");
        presentation::add_rule(p, "F", "Z");
        presentation::add_rule(p, "G", "X");
        presentation::add_rule(p, "H", "Z");
        presentation::add_rule(p, "I", "Y");
      }
      return to_presentation<word_type>(p);
    }

    auto init = [](ToddCoxeter& tc) {
      if (tc.strategy() == strategy::hlt) {
        tc.lookahead_next(500'000).large_collapse(2'000);
      } else {
        tc.lookahead_next(100'000);
      }
    };

    sizes_type sizes      = {0, 1};
    auto       strategies = {strategy::hlt, strategy::felsch};
    BENCHMARK_TODD_COXETER_RANGE(sizes,
                                 "TODO",
                                 "TODO",
                                 "TODO",
                                 1,
                                 1,
                                 walker,
                                 "walker",
                                 strategies,
                                 init);
  }  // namespace walker
}  // namespace libsemigroups

// REQUIRE(tc.size() == 1);
//}
/*
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


    TEST_CASE("Walker 2", "[quick][Walker2][paper]") {
      using options = libsemigroups::congruence::ToddCoxeter::options;
      auto rg       = ReportGuard(false);
      BENCHMARK("HLT") {
        ToddCoxeter tc;
        walker2(tc);
        tc.congruence().simplify();
        tc.congruence().strategy(strategy::hlt).next_lookahead(2'000'000);
        REQUIRE(tc.size() == 14'911);
      };

      BENCHMARK("Felsch") {
        ToddCoxeter tc;
        walker2(tc);
        tc.congruence().simplify();
        tc.congruence()
            .strategy(strategy::felsch)
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
            .strategy(strategy::hlt)
            .lookahead(lookahead::partial | lookahead::felsch)
            .deduction_policy(deductions::v2
                              | deductions::no_stack_if_no_space);
        REQUIRE(tc.size() == 14'911);
      };
    }

    TEST_CASE("Walker 3", "[quick][Walker3][paper]") {
      using options = libsemigroups::congruence::ToddCoxeter::options;
      auto rg       = ReportGuard(false);
      BENCHMARK("HLT") {
        ToddCoxeter tc;
        walker3(tc);
        tc.congruence().next_lookahead(2'000'000).strategy(strategy::hlt);
        REQUIRE(tc.size() == 20'490);
      };

      BENCHMARK("Felsch") {
        ToddCoxeter tc;
        walker3(tc);
        tc.congruence()
            .strategy(strategy::felsch)
            .use_relations_in_extra(true)
            .max_deductions(100'000)
            .deduction_policy(deductions::v1 |
  deductions::no_stack_if_no_space) .preferred_defs(preferred_defs::none);
        REQUIRE(tc.size() == 20'490);
      };
    }

    TEST_CASE("Walker 4", "[quick][Walker4][paper]") {
      using options = libsemigroups::congruence::ToddCoxeter::options;
      auto rg       = ReportGuard(false);
      BENCHMARK("HLT") {
        ToddCoxeter tc;
        walker4(tc);
        tc.congruence().next_lookahead(3'000'000).strategy(strategy::hlt);
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
          .strategy(strategy::felsch)
          .preferred_defs(preferred_defs::deferred)
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
        tc.congruence().next_lookahead(5'000'000).strategy(strategy::hlt);
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
          .strategy(strategy::felsch)
          .max_deductions(POSITIVE_INFINITY)
          .preferred_defs(preferred_defs::none);
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
        tc.congruence().strategy(strategy::hlt);
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
        tc.congruence().strategy(strategy::felsch);
        REQUIRE(tc.size() == 78'722);
      };
    }

    TEST_CASE("Walker 7", "[quick][Walker7][paper]") {
      using options = libsemigroups::congruence::ToddCoxeter::options;
      auto rg       = ReportGuard(false);
      BENCHMARK("HLT") {
        ToddCoxeter tc;
        walker7(tc);
        tc.congruence().strategy(strategy::hlt);
        REQUIRE(tc.size() == 153'500);
      };
      BENCHMARK("Felsch") {
        ToddCoxeter tc;
        walker7(tc);
        tc.congruence().simplify(10);
        tc.congruence()
            .strategy(strategy::felsch)
            .deduction_policy(deductions::no_stack_if_no_space |
  deductions::v1) .preferred_defs(preferred_defs::none); REQUIRE(tc.size()
  == 153'500);
      };
    }

    TEST_CASE("Walker 8", "[quick][Walker8][paper]") {
      using options = libsemigroups::congruence::ToddCoxeter::options;
      auto rg       = ReportGuard(false);
      BENCHMARK("HLT") {
        ToddCoxeter tc;
        walker8(tc);
        tc.congruence().next_lookahead(500'000);
        tc.congruence().strategy(strategy::hlt);
        REQUIRE(tc.size() == 270'272);
      };
    }

    TEST_CASE("Walker 8 - Felsch only", "[paper][Walker8][felsch]") {
      auto        rg = ReportGuard(true);
      ToddCoxeter tc;
      walker5(tc);
      tc.congruence().simplify(10);
      tc.congruence().strategy(strategy::felsch);
      std::cout << tc.congruence().settings_string();
      REQUIRE(tc.size() == 270'272);
      std::cout << tc.congruence().stats_string();
    }
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
        H.next_lookahead(1'000'000).lookahead(lookahead::partial);
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

        H.strategy(strategy::hlt).save(true).lookahead(lookahead::partial);

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
            .lookahead(lookahead::partial)
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

        H.strategy(strategy::hlt)
            .save(true)
            .lookahead(lookahead::full)
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
            .strategy(strategy::hlt)
            .lookahead(lookahead::partial)
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

        H.strategy(strategy::hlt)
            .save(true)
            .lookahead(lookahead::partial)
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
            .strategy(strategy::hlt)
            .save(true)
            .lookahead(lookahead::partial);
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

        H.strategy(strategy::hlt).save(true).lookahead(lookahead::partial);

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

        H.strategy(strategy::hlt)
            .save(false)
            .lookahead(lookahead::partial)
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
        H.strategy(strategy::hlt)
            .save(true)
            .lookahead(lookahead::partial)
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
      G.add_rule("dCACacAcadCACacAcadCACacAcadCACacAcadCACacAcadCACacAca",
  "x");

      G.congruence()
          .remove_duplicate_generating_pairs()
          .sort_generating_pairs();
      REQUIRE(G.size() == 1);
      std::cout << G.congruence().stats_string();
    }
    */
