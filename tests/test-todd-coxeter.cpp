// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019-2025 James D. Mitchell
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

// The purpose of this file is to test the ToddCoxeter class.

#include <chrono>    // for chrono::seconds
#include <cstdlib>   // for ?
#include <fstream>   // for ofstream
#include <iostream>  // for cout

#include "test-main.hpp"  // for LIBSEMIGROUPS_TEST_CASE

#include "Catch2-3.8.0/catch_amalgamated.hpp"  // for TEST_CASE

#include "libsemigroups/bmat8.hpp"                  // for BMat8
#include "libsemigroups/constants.hpp"              // for UNDEFINED
#include "libsemigroups/froidure-pin.hpp"           // for FroidurePin
#include "libsemigroups/gabow.hpp"                  // for Gabow
#include "libsemigroups/knuth-bendix.hpp"           // for KnuthBendix
#include "libsemigroups/obvinf.hpp"                 // for is_obviously_infinite
#include "libsemigroups/presentation-examples.hpp"  // for dual_symmetric_...
#include "libsemigroups/presentation.hpp"           // for Presentation
#include "libsemigroups/ranges.hpp"                 // for is_sorted
#include "libsemigroups/to-froidure-pin.hpp"        // for make
#include "libsemigroups/to-knuth-bendix.hpp"        // for to_knuth_bendix
#include "libsemigroups/to-presentation.hpp"        // for Presentation
#include "libsemigroups/to-todd-coxeter.hpp"        // for ??
#include "libsemigroups/todd-coxeter.hpp"           // for ToddCoxeter
#include "libsemigroups/transf.hpp"                 // for Transf
#include "libsemigroups/word-graph.hpp"             // for WordGraph
#include "libsemigroups/word-range.hpp"             // for operator"" _w

#include "libsemigroups/detail/report.hpp"  // for ReportGuard
#include "libsemigroups/detail/tce.hpp"     // for TCE

namespace libsemigroups {

  using todd_coxeter::class_by_index;
  using todd_coxeter::class_of;
  using todd_coxeter::contains;
  using todd_coxeter::index_of;
  using todd_coxeter::is_non_trivial;
  using todd_coxeter::non_trivial_classes;
  using todd_coxeter::normal_forms;
  using todd_coxeter::partition;
  using todd_coxeter::reduce;
  using todd_coxeter::redundant_rule;
  using todd_coxeter::word_of;

  namespace {
    template <typename Word>
    void output_gap_benchmark_file(std::string const& fname,
                                   ToddCoxeter<Word>& tc) {
      std::ofstream file;
      file.open(fname);
      file << "local free, rules, RR, SS, TT, a, b, c, d, e, f, g, h, i, j, k, "
              "l, m, n, o, p, q, r, s, t, u, v, w, x, y, z, A, B, C, D, E, F, "
              "G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z;\n";
      file << presentation::to_gap_string(tc.presentation(), "SS");
      file << "RR := RightMagmaCongruenceByGeneratingPairs(SS, []);\n";
      file << "TT := CosetTableOfFpSemigroup(RR);;\n";
      file << "Assert(0, Length(TT) = Size(GeneratorsOfSemigroup(SS)));\n";
      file << "Assert(0, Length(TT[1]) - 1 = "
           << std::to_string(tc.number_of_classes()) << ");\n";
      file.close();
    }
  }  // namespace

  using TCE     = detail::TCE;
  using options = detail::ToddCoxeterImpl::options;
  using std::string_literals::operator""s;

  using namespace literals;
  using namespace rx;

  struct LibsemigroupsException;  // Forward declaration
  congruence_kind constexpr twosided = congruence_kind::twosided;
  congruence_kind constexpr onesided = congruence_kind::onesided;

  namespace {
    template <typename Word>
    void section_felsch(ToddCoxeter<Word>& tc) {
      SECTION("Felsch") {
        tc.strategy(options::strategy::felsch);
      }
    }

    template <typename Word>
    void section_hlt(ToddCoxeter<Word>& tc) {
      SECTION("HLT + full hlt lookahead + no save") {
        tc.strategy(options::strategy::hlt)
            .lookahead_extent(options::lookahead_extent::full)
            .lookahead_style(options::lookahead_style::hlt)
            .save(false);
      }
      SECTION("HLT + partial hlt lookahead + no save") {
        tc.strategy(options::strategy::hlt)
            .lookahead_extent(options::lookahead_extent::partial)
            .lookahead_style(options::lookahead_style::hlt)
            .save(false);
      }
      SECTION("HLT + full lookahead + save") {
        tc.strategy(options::strategy::hlt)
            .lookahead_extent(options::lookahead_extent::partial)
            .lookahead_style(options::lookahead_style::hlt)
            .save(true);
      }
      SECTION("HLT + partial hlt lookahead + save") {
        tc.strategy(options::strategy::hlt)
            .lookahead_extent(options::lookahead_extent::partial)
            .lookahead_style(options::lookahead_style::hlt)
            .save(true);
      }
    }

    template <typename Word>
    void section_CR_style(ToddCoxeter<Word>& tc) {
      SECTION("CR style") {
        tc.strategy(options::strategy::CR);
      }
    }

    template <typename Word>
    void section_R_over_C_style(ToddCoxeter<Word>& tc) {
      SECTION("R/C style") {
        tc.strategy(options::strategy::R_over_C);
      }
    }

    template <typename Word>
    void section_Cr_style(ToddCoxeter<Word>& tc) {
      SECTION("Cr style") {
        tc.strategy(options::strategy::Cr);
      }
    }

    template <typename Word>
    void section_Rc_style(ToddCoxeter<Word>& tc) {
      SECTION("Rc style") {
        tc.strategy(options::strategy::Rc);
      }
    }

    template <typename Word>
    void check_complete_compatible(ToddCoxeter<Word>& tc) {
      auto const& p  = tc.internal_presentation();
      auto const& wg = tc.word_graph();
      REQUIRE(word_graph::is_complete(
          wg, wg.cbegin_active_nodes(), wg.cend_active_nodes()));
      REQUIRE(word_graph::is_compatible_no_checks(wg,
                                                  wg.cbegin_active_nodes(),
                                                  wg.cend_active_nodes(),
                                                  p.rules.cbegin(),
                                                  p.rules.cend()));
      tc.shrink_to_fit();
      REQUIRE(word_graph::is_complete(
          wg, wg.cbegin_active_nodes(), wg.cend_active_nodes()));
      REQUIRE(word_graph::is_compatible_no_checks(wg,
                                                  wg.cbegin_active_nodes(),
                                                  wg.cend_active_nodes(),
                                                  p.rules.cbegin(),
                                                  p.rules.cend()));
    }

    template <typename Word>
    void check_standardize(ToddCoxeter<Word>& tc) {
      using namespace rx;
      using word_graph::follow_path_no_checks;

      using node_type = typename ToddCoxeter<Word>::node_type;
      Order old_val   = tc.standardization_order();

      tc.run();
      for (auto val : {Order::shortlex, Order::lex, Order::recursive}) {
        tc.standardize(val);
        REQUIRE(tc.is_standardized(val));
        REQUIRE(tc.is_standardized());
        REQUIRE(tc.standardization_order() == val);
      }
      {
        tc.standardize(Order::shortlex);
        size_t const m = tc.number_of_classes();
        size_t const n = tc.presentation().alphabet().size();

        WordRange words;
        words.alphabet_size(n).min(1).max(m + 1);

        std::unordered_map<node_type, word_type> map;
        for (auto const& w : words) {
          node_type t = follow_path_no_checks(tc.current_word_graph(), 0, w);
          REQUIRE(t != UNDEFINED);
          if (t != 0) {
            map.emplace(t - 1, w);
            if (map.size() == tc.number_of_classes()) {
              break;
            }
          }
        }

        auto nf = normal_forms(tc) | to_vector();

        for (auto const& p : map) {
          REQUIRE(nf[p.first] == p.second);
        }
      }
      {
        tc.standardize(Order::lex);

        size_t const m = tc.number_of_classes();
        size_t const n = tc.presentation().alphabet().size();

        WordRange words;
        words.order(Order::lex)
            .alphabet_size(n)
            .upper_bound(m + 1)
            .min(1)
            .max(m + 1);

        std::unordered_map<node_type, word_type> map;

        for (auto const& w : words) {
          node_type t = word_graph::follow_path_no_checks(
              tc.current_word_graph(), 0, w);
          if (t != 0) {
            auto ww = w;
            map.emplace(t - 1, std::move(ww));
            if (map.size() == tc.number_of_classes()) {
              break;
            }
          }
        }
        auto nf = normal_forms(tc) | to_vector();

        for (auto const& p : map) {
          REQUIRE(nf[p.first] == p.second);
        }
      }
      tc.standardize(old_val);
    }

    template <typename Word>
    void check_contains(ToddCoxeter<Word>& tc) {
      auto const& p = tc.presentation();
      for (auto it = p.rules.cbegin(); it < p.rules.cend(); it += 2) {
        REQUIRE(contains(tc, *it, *(it + 1)));
      }
    }

    template <typename Word>
    void check_word_to_index_of(ToddCoxeter<Word>& tc) {
      for (size_t i = 0; i < tc.number_of_classes(); ++i) {
        REQUIRE(index_of(tc, word_of(tc, i)) == i);
      }
    }

    // TODO(2) this seems a bit slow, it could be faster if we didn't repeatedly
    // re-traverse the forest, and we didn't repeatedly copy the word returned
    // by "index_of_to_word"
    template <typename Word>
    void check_normal_forms(ToddCoxeter<Word>& tc, size_t i) {
      tc.standardize(Order::shortlex);
      REQUIRE((normal_forms(tc) | take(i) | all_of([&tc](Word const& w) {
                 return w == class_of(tc, w).get();
               })));

      // the following fails if the alphabet isn't sorted
      if (std::is_sorted(tc.presentation().alphabet().begin(),
                         tc.presentation().alphabet().end())) {
        // TODO(1) allow specifying the alphabet order to ShortLexCompare
        // and the other orders
        REQUIRE(is_sorted(normal_forms(tc), ShortLexCompare{}));
        tc.standardize(Order::lex);
        REQUIRE(is_sorted(normal_forms(tc), LexicographicalCompare{}));
      }
    }
  }  // namespace

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "000",
                          "small 2-sided congruence",
                          "[todd-coxeter][quick]") {
    using namespace rx;
    auto rg = ReportGuard(false);

    Presentation<word_type> p;
    p.alphabet(2);
    presentation::add_rule(p, 000_w, 0_w);
    presentation::add_rule(p, 1111_w, 1_w);
    presentation::add_rule(p, 0101_w, 00_w);
    ToddCoxeter tc(twosided, p);

    section_felsch(tc);
    section_hlt(tc);
    section_CR_style(tc);
    section_R_over_C_style(tc);
    section_Rc_style(tc);
    section_Cr_style(tc);

    REQUIRE(tc.number_of_classes() == 27);

    check_complete_compatible(tc);
    check_standardize(tc);

    tc.standardize(Order::shortlex);

    auto c = class_by_index(tc, 1);
    // TODO(1) this no longer works
    // REQUIRE(c.count() == POSITIVE_INFINITY);

    // REQUIRE((c.min(0).max(8) | to_vector())
    //         == std::vector({1_w, 1111_w, 1111111_w}));
    // REQUIRE((c.min(0).max(10) | to_vector())
    //         == std::vector({1_w, 1111_w, 1111111_w}));

    // TODO(1) this no longer works
    // REQUIRE((seq() | take(tc.number_of_classes()) | all_of([&tc](size_t i) {
    //            return class_by_index(tc, i).count() == POSITIVE_INFINITY;
    //          })));

    REQUIRE((c | rx::take(3)
             | all_of([&tc](auto const& w) { return index_of(tc, w) == 1; })));
    check_normal_forms(tc, tc.number_of_classes());
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "001",
                          "small 2-sided congruence x 2",
                          "[no-valgrind][todd-coxeter][quick]") {
    auto rg = ReportGuard(false);

    Presentation<word_type> p;
    p.alphabet(2);
    presentation::add_rule(p, 000_w, 0_w);
    presentation::add_rule(p, 0_w, 11_w);
    ToddCoxeter tc(twosided, p);

    section_felsch(tc);
    section_hlt(tc);
    section_Rc_style(tc);
    section_R_over_C_style(tc);
    section_CR_style(tc);
    section_Cr_style(tc);

    tc.run();
    REQUIRE(tc.number_of_classes() == 5);
    REQUIRE(tc.finished());

    REQUIRE(index_of(tc, 001_w) == index_of(tc, 00001_w));
    REQUIRE(index_of(tc, 011001_w) == index_of(tc, 00001_w));

    REQUIRE(index_of(tc, 000_w) != index_of(tc, 1_w));

    tc.standardize(Order::shortlex);
    REQUIRE(word_of(tc, 0) == 0_w);
    REQUIRE(word_of(tc, 1) == 1_w);
    REQUIRE(word_of(tc, 2) == 00_w);
    tc.standardize(Order::lex);
    REQUIRE(tc.is_standardized(Order::lex));
    REQUIRE(tc.is_standardized());
    REQUIRE(!tc.is_standardized(Order::shortlex));

    REQUIRE(word_of(tc, 0) == 0_w);
    REQUIRE(word_of(tc, 1) == 00_w);
    REQUIRE(word_of(tc, 2) == 001_w);
    REQUIRE(word_of(tc, 3) == 0010_w);
    REQUIRE(index_of(tc, 0001_w) == 3);
    REQUIRE(word_of(tc, 4) == 1_w);
    REQUIRE(index_of(tc, word_of(tc, 0)) == 0);
    REQUIRE(index_of(tc, word_of(tc, 1)) == 1);
    REQUIRE(index_of(tc, word_of(tc, 2)) == 2);
    REQUIRE(index_of(tc, word_of(tc, 3)) == 3);
    REQUIRE(index_of(tc, word_of(tc, 4)) == 4);
    REQUIRE(index_of(tc, 01_w) == 3);
    REQUIRE(LexicographicalCompare()(001_w, 01_w));

    REQUIRE(is_sorted(normal_forms(tc), LexicographicalCompare{}));

    tc.standardize(Order::shortlex);
    REQUIRE(tc.is_standardized(Order::shortlex));
    REQUIRE((normal_forms(tc) | to_vector())
            == std::vector({0_w, 1_w, 00_w, 01_w, 001_w}));
    REQUIRE(index_of(tc, word_of(tc, 0)) == 0);
    REQUIRE(index_of(tc, word_of(tc, 1)) == 1);
    REQUIRE(index_of(tc, word_of(tc, 2)) == 2);
    REQUIRE(index_of(tc, word_of(tc, 3)) == 3);
    REQUIRE(index_of(tc, word_of(tc, 4)) == 4);
    REQUIRE(is_sorted(normal_forms(tc), ShortLexCompare()));

    auto nf = normal_forms(tc) | to_vector();

    REQUIRE(nf == std::vector({0_w, 1_w, 00_w, 01_w, 001_w}));
    REQUIRE(std::all_of(nf.begin(), nf.end(), [&tc](word_type& w) {
      return w == class_of(tc, w).get();
    }));

    // TODO(2) implement cbegin/cend_wirpo (words in recursive path order
    // for (size_t i = 2; i < 6; ++i) {
    //   for (size_t j = 2; j < 10 - i; ++j) {
    //     auto v
    //         = std::vector(cbegin_wislo(i, {0}, word_type(j + 1,
    //         0)),
    //                                  cend_wislo(i, {0}, word_type(j + 1,
    //                                  0)));
    //     std::sort(v.begin(), v.end(), RecursivePathCompare<word_type>{});
    //     REQUIRE(v == recursive_path_words(i, j));
    //   }
    // }

    tc.standardize(Order::recursive);
    REQUIRE(tc.is_standardized());

    REQUIRE(word_of(tc, 0) == 0_w);
    REQUIRE(word_of(tc, 1) == 00_w);
    REQUIRE(word_of(tc, 2) == 1_w);
    REQUIRE(word_of(tc, 3) == 10_w);
    REQUIRE(word_of(tc, 4) == 100_w);
    REQUIRE(is_sorted(normal_forms(tc), RecursivePathCompare{}));

    check_normal_forms(tc, tc.number_of_classes());
  }

  // Felsch is actually faster here!
  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "002",
                          "Example 6.6 in Sims (see also KnuthBendix 013)",
                          "[todd-coxeter][extreme]") {
    auto rg = ReportGuard(true);

    Presentation<word_type> p;
    p.alphabet(4);
    presentation::add_rule(p, 00_w, 0_w);
    presentation::add_rule(p, 10_w, 1_w);
    presentation::add_rule(p, 01_w, 1_w);
    presentation::add_rule(p, 20_w, 2_w);
    presentation::add_rule(p, 02_w, 2_w);
    presentation::add_rule(p, 30_w, 3_w);
    presentation::add_rule(p, 03_w, 3_w);
    presentation::add_rule(p, 11_w, 0_w);
    presentation::add_rule(p, 23_w, 0_w);
    presentation::add_rule(p, 222_w, 0_w);
    presentation::add_rule(p, 12121212121212_w, 0_w);
    presentation::add_rule(p, 12131213121312131213121312131213_w, 0_w);

    ToddCoxeter tc(twosided, p);

    section_hlt(tc);
    section_felsch(tc);
    section_CR_style(tc);
    section_Rc_style(tc);
    section_R_over_C_style(tc);
    section_Cr_style(tc);

    REQUIRE(tc.number_of_classes() == 10'752);
    check_complete_compatible(tc);

    REQUIRE(tc.finished());

    tc.standardize(Order::recursive);
    REQUIRE(is_sorted(normal_forms(tc), RecursivePathCompare{}));
    REQUIRE(
        (normal_forms(tc) | take(10) | to_vector())
        == std::vector(
            {0_w, 1_w, 2_w, 21_w, 12_w, 121_w, 22_w, 221_w, 212_w, 2121_w}));

    tc.standardize(Order::lex);
    REQUIRE(normal_forms(tc).size_hint() == 10'752);
    REQUIRE(tc.is_standardized());
    REQUIRE(tc.is_standardized(Order::lex));
    for (size_t c = 0; c < tc.number_of_classes(); ++c) {
      REQUIRE(index_of(tc, word_of(tc, c)) == c);
    }
    REQUIRE(is_sorted(normal_forms(tc), LexicographicalCompare{}));
    REQUIRE((normal_forms(tc) | take(10) | to_vector())
            == std::vector({0_w,
                            01_w,
                            012_w,
                            0121_w,
                            01212_w,
                            012121_w,
                            0121212_w,
                            01212121_w,
                            012121212_w,
                            0121212121_w}));
    tc.standardize(Order::shortlex);
    for (size_t c = 0; c < tc.number_of_classes(); ++c) {
      REQUIRE(index_of(tc, word_of(tc, c)) == c);
    }
    REQUIRE(is_sorted(normal_forms(tc), ShortLexCompare{}));
    REQUIRE((normal_forms(tc) | take(10) | to_vector())
            == std::vector(
                {0_w, 1_w, 2_w, 3_w, 12_w, 13_w, 21_w, 31_w, 121_w, 131_w}));
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "003",
                          "constructed from FroidurePin",
                          "[todd-coxeter][quick][no-coverage][no-valgrind]") {
    auto rg = ReportGuard(false);

    FroidurePin S = make<FroidurePin>(
        {BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
         BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
         BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {1, 0, 0, 1}}),
         BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}})});

    REQUIRE(S.size() == 63'904);

    ToddCoxeter tc(twosided, to<Presentation<word_type>>(S));

    todd_coxeter::add_generating_pair_no_checks(tc, 0_w, 1_w);

    section_felsch(tc);
    section_hlt(tc);
    section_CR_style(tc);
    section_Rc_style(tc);
    section_R_over_C_style(tc);
    section_Cr_style(tc);

    tc.lower_bound(3);
    tc.run();

    check_complete_compatible(tc);
    check_standardize(tc);

    REQUIRE(tc.number_of_classes() == 3);
    REQUIRE(contains(tc, 0_w, 1_w));

    auto T = to<FroidurePin>(tc);
    REQUIRE(T.size() == 3);
    REQUIRE(word_of(tc, 0) == froidure_pin::factorisation(T, 0));
    REQUIRE(word_of(tc, 1) == froidure_pin::factorisation(T, 1));
    REQUIRE(word_of(tc, 2) == froidure_pin::factorisation(T, 2));

    REQUIRE(word_of(tc, 0) == 0_w);
    REQUIRE(word_of(tc, 1) == 2_w);
    REQUIRE(word_of(tc, 2) == 00_w);
    REQUIRE(index_of(tc, word_of(tc, 0)) == 0);
    REQUIRE(index_of(tc, word_of(tc, 1)) == 1);
    REQUIRE(index_of(tc, word_of(tc, 2)) == 2);

    tc.standardize(Order::lex);
    REQUIRE(word_of(tc, 0) == 0_w);
    REQUIRE(word_of(tc, 1) == 00_w);
    REQUIRE(word_of(tc, 2) == 002_w);
    REQUIRE(index_of(tc, word_of(tc, 0)) == 0);
    REQUIRE(index_of(tc, word_of(tc, 1)) == 1);
    REQUIRE(index_of(tc, word_of(tc, 2)) == 2);

    tc.standardize(Order::shortlex);
    REQUIRE(word_of(tc, 0) == 0_w);
    REQUIRE(word_of(tc, 1) == 2_w);
    REQUIRE(word_of(tc, 2) == 00_w);

    check_normal_forms(tc, tc.number_of_classes());

    tc.init(twosided, to<Presentation<word_type>>(S));
    REQUIRE(tc.number_of_generating_pairs() == 0);
    REQUIRE(tc.generating_pairs().empty());
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "004",
                          "non-trivial two-sided from relations",
                          "[todd-coxeter][quick]") {
    auto rg = ReportGuard(false);

    Presentation<word_type> p;
    p.alphabet(3);
    presentation::add_rule(p, 01_w, 10_w);
    presentation::add_rule(p, 02_w, 22_w);
    presentation::add_rule(p, 02_w, 0_w);
    presentation::add_rule(p, 22_w, 0_w);
    presentation::add_rule(p, 12_w, 12_w);
    presentation::add_rule(p, 12_w, 22_w);
    presentation::add_rule(p, 122_w, 1_w);
    presentation::add_rule(p, 12_w, 1_w);
    presentation::add_rule(p, 22_w, 1_w);
    presentation::add_rule(p, 0_w, 1_w);

    ToddCoxeter tc(twosided, p);

    section_felsch(tc);
    section_hlt(tc);
    section_CR_style(tc);
    section_Rc_style(tc);
    section_R_over_C_style(tc);
    section_Cr_style(tc);

    REQUIRE(tc.number_of_classes() == 2);
    check_standardize(tc);
    check_normal_forms(tc, tc.number_of_classes());
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "005",
                          "small onesided cong. on free semigroup",
                          "[todd-coxeter][quick]") {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.alphabet(2);
    presentation::add_rule(p, 000_w, 0_w);
    presentation::add_rule(p, 0_w, 11_w);

    ToddCoxeter tc(onesided, p);

    section_felsch(tc);
    section_hlt(tc);
    section_Rc_style(tc);
    section_R_over_C_style(tc);
    section_CR_style(tc);
    section_Cr_style(tc);

    REQUIRE(tc.number_of_classes() == 5);
    REQUIRE(tc.finished());
    check_standardize(tc);
    check_normal_forms(tc, tc.number_of_classes());
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "006",
                          "left cong. on free semigroup",
                          "[todd-coxeter][quick]") {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.alphabet(2);
    presentation::add_rule(p, 000_w, 0_w);
    presentation::add_rule(p, 0_w, 11_w);
    presentation::reverse(p);
    {
      ToddCoxeter tc(onesided, p);
      tc.lookahead_growth_factor(1.5);

      section_felsch(tc);
      section_hlt(tc);
      section_Rc_style(tc);
      section_R_over_C_style(tc);
      section_CR_style(tc);
      section_Cr_style(tc);

      REQUIRE(!tc.is_standardized());
      REQUIRE(index_of(tc, 100_w) == index_of(tc, 10000_w));
      REQUIRE(index_of(tc, 100110_w) == index_of(tc, 10000_w));
      REQUIRE(index_of(tc, 1_w) != index_of(tc, 0000_w));
      REQUIRE(index_of(tc, 000_w) != index_of(tc, 0000_w));
      tc.standardize(Order::shortlex);
      REQUIRE(tc.is_standardized());
      check_standardize(tc);
      check_complete_compatible(tc);
    }
    {
      ToddCoxeter tc(onesided, p);
      REQUIRE_NOTHROW(ToddCoxeter(onesided, tc));
    }
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "007",
                          "for small fp semigroup",
                          "[todd-coxeter][quick]") {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.alphabet(2);
    presentation::add_rule(p, 000_w, 0_w);
    presentation::add_rule(p, 0_w, 11_w);
    ToddCoxeter tc(twosided, p);

    section_felsch(tc);
    section_hlt(tc);
    section_Rc_style(tc);
    section_R_over_C_style(tc);
    section_CR_style(tc);
    section_Cr_style(tc);

    REQUIRE(index_of(tc, 001_w) == index_of(tc, 00001_w));
    REQUIRE(index_of(tc, 011001_w) == index_of(tc, 00001_w));
    REQUIRE(index_of(tc, 000_w) != index_of(tc, 1_w));
    REQUIRE(index_of(tc, 0000_w) < tc.number_of_classes());
    tc.standardize(Order::shortlex);
    check_standardize(tc);
    check_complete_compatible(tc);
  }

  namespace {
    template <typename Word>
    void test_case_010(ToddCoxeter<Word>& tc, FroidurePin<Transf<>>& S) {
      section_felsch(tc);
      section_hlt(tc);
      section_Rc_style(tc);
      section_R_over_C_style(tc);
      section_CR_style(tc);
      section_Cr_style(tc);

      REQUIRE(index_of(tc,
                       froidure_pin::factorisation(
                           S, make<Transf<>>({1, 3, 1, 3, 3})))
              != index_of(tc,
                          froidure_pin::factorisation(
                              S, make<Transf<>>({4, 2, 4, 4, 2}))));

      REQUIRE(tc.number_of_classes() == 69);
      REQUIRE(tc.number_of_classes() == 69);
      std::vector nf(S.cbegin_normal_forms(), S.cend_normal_forms());
      for (auto& w : nf) {
        reverse(w);
      }
      auto ntc = non_trivial_classes(tc, nf.begin(), nf.end());

      REQUIRE(ntc.size() == 1);
      REQUIRE(ntc[0].size() == 20);
      REQUIRE(ntc
              == decltype(ntc)(
                  {{100_w,      101_w,       1000_w,      1010_w,
                    1001_w,     10000_w,     10100_w,     10001_w,
                    10101_w,    101000_w,    100010_w,    101010_w,
                    101001_w,   1010000_w,   1000100_w,   1010001_w,
                    10100010_w, 001000100_w, 101000100_w, 001100010_w}}));
      REQUIRE(std::all_of(
          ntc[0].cbegin(), ntc[0].cend(), [&ntc, &tc](auto const& w) {
            return contains(tc, w, ntc[0][0]);
          }));
    }
  }  // namespace

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "008",
                          "left congruence on transformation semigroup",
                          "[todd-coxeter][quick]") {
    auto rg = ReportGuard(false);
    auto S  = make<FroidurePin>(
        {make<Transf<>>({1, 3, 4, 2, 3}), make<Transf<>>({3, 2, 1, 3, 3})});

    REQUIRE(S.size() == 88);
    REQUIRE(S.number_of_rules() == 18);

    SECTION("construction from presentation") {
      ToddCoxeter tc(onesided,
                     presentation::reverse(to<Presentation<word_type>>(S)));
      todd_coxeter::add_generating_pair(
          tc,
          reverse(
              froidure_pin::factorisation(S, make<Transf<>>({3, 4, 4, 4, 4}))),
          reverse(
              froidure_pin::factorisation(S, make<Transf<>>({3, 1, 3, 3, 3}))));
      test_case_010(tc, S);
    }
    SECTION("construction from Cayley graph") {
      auto tc = to<ToddCoxeter<word_type>>(onesided, S, S.left_cayley_graph());
      todd_coxeter::add_generating_pair(
          tc,
          reverse(
              froidure_pin::factorisation(S, make<Transf<>>({3, 4, 4, 4, 4}))),
          reverse(
              froidure_pin::factorisation(S, make<Transf<>>({3, 1, 3, 3, 3}))));
      test_case_010(tc, S);
    }
    SECTION("default construction + move from Cayley graph") {
      ToddCoxeter<word_type> tc;
      tc = to<ToddCoxeter<word_type>>(onesided, S, S.left_cayley_graph());
      todd_coxeter::add_generating_pair(
          tc,
          reverse(
              froidure_pin::factorisation(S, make<Transf<>>({3, 4, 4, 4, 4}))),
          reverse(
              froidure_pin::factorisation(S, make<Transf<>>({3, 1, 3, 3, 3}))));
      test_case_010(tc, S);
    }
    SECTION("default construction + move from presentation") {
      ToddCoxeter<word_type> tc;
      tc = ToddCoxeter(onesided,
                       presentation::reverse(to<Presentation<word_type>>(S)));
      todd_coxeter::add_generating_pair(
          tc,
          reverse(
              froidure_pin::factorisation(S, make<Transf<>>({3, 4, 4, 4, 4}))),
          reverse(
              froidure_pin::factorisation(S, make<Transf<>>({3, 1, 3, 3, 3}))));
      test_case_010(tc, S);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "009",
                          "onesided cong. trans. semigroup",
                          "[todd-coxeter][quick]") {
    auto rg = ReportGuard(false);
    auto S  = make<FroidurePin>(
        {make<Transf<>>({1, 3, 4, 2, 3}), make<Transf<>>({3, 2, 1, 3, 3})});

    REQUIRE(S.size() == 88);
    REQUIRE(S.number_of_rules() == 18);

    auto tc = to<ToddCoxeter<word_type>>(onesided, S, S.right_cayley_graph());

    todd_coxeter::add_generating_pair(
        tc,
        froidure_pin::factorisation(S, make<Transf<>>({3, 4, 4, 4, 4})),
        froidure_pin::factorisation(S, make<Transf<>>({3, 1, 3, 3, 3})));

    section_felsch(tc);
    section_hlt(tc);
    section_Rc_style(tc);
    section_R_over_C_style(tc);
    section_CR_style(tc);
    section_Cr_style(tc);

    REQUIRE(tc.number_of_classes() == 72);
    REQUIRE(tc.number_of_classes() == 72);

    REQUIRE(
        index_of(
            tc, froidure_pin::factorisation(S, make<Transf<>>({1, 3, 1, 3, 3})))
        != index_of(
            tc,
            froidure_pin::factorisation(S, make<Transf<>>({4, 2, 4, 4, 2}))));

    REQUIRE(
        index_of(
            tc, froidure_pin::factorisation(S, make<Transf<>>({1, 3, 3, 3, 3})))
        != index_of(
            tc,
            froidure_pin::factorisation(S, make<Transf<>>({4, 2, 4, 4, 2}))));
    REQUIRE(
        index_of(
            tc, froidure_pin::factorisation(S, make<Transf<>>({2, 4, 2, 2, 2})))
        == index_of(
            tc,
            froidure_pin::factorisation(S, make<Transf<>>({2, 3, 3, 3, 3}))));
    REQUIRE(
        index_of(
            tc, froidure_pin::factorisation(S, make<Transf<>>({1, 3, 3, 3, 3})))
        != index_of(
            tc,
            froidure_pin::factorisation(S, make<Transf<>>({2, 3, 3, 3, 3}))));

    auto ntc = non_trivial_classes(
        tc, S.cbegin_normal_forms(), S.cend_normal_forms());
    REQUIRE(ntc.size() == 4);
    std::vector<size_t> sizes(ntc.size(), 0);
    std::transform(ntc.cbegin(),
                   ntc.cend(),
                   sizes.begin(),
                   std::mem_fn(&std::vector<word_type>::size));
    std::sort(sizes.begin(), sizes.end());
    REQUIRE(sizes == std::vector<size_t>({3, 5, 5, 7}));
    word_type w3, w4, w5, w6;
    froidure_pin::factorisation(
        S, w3, S.position(make<Transf<>>({1, 3, 3, 3, 3})));
    froidure_pin::factorisation(
        S, w4, S.position(make<Transf<>>({4, 2, 4, 4, 2})));
    froidure_pin::factorisation(
        S, w5, S.position(make<Transf<>>({2, 4, 2, 2, 2})));
    froidure_pin::factorisation(
        S, w6, S.position(make<Transf<>>({2, 3, 3, 3, 3})));
    REQUIRE(index_of(tc, w3) != index_of(tc, w4));
    REQUIRE(index_of(tc, w5) == index_of(tc, w6));
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "010",
                          "trans. semigroup (size 88)",
                          "[todd-coxeter][quick]") {
    auto rg = ReportGuard(false);

    FroidurePin<Transf<>> S;
    S.add_generator(make<Transf<>>({1, 3, 4, 2, 3}));
    S.add_generator(make<Transf<>>({3, 2, 1, 3, 3}));

    REQUIRE(S.size() == 88);
    REQUIRE(S.number_of_rules() == 18);

    ToddCoxeter tc
        = to<ToddCoxeter<word_type>>(twosided, S, S.right_cayley_graph());

    word_type w1 = froidure_pin::factorisation(
        S, S.position(make<Transf<>>({3, 4, 4, 4, 4})));
    word_type w2 = froidure_pin::factorisation(
        S, S.position(make<Transf<>>({3, 1, 3, 3, 3})));

    todd_coxeter::add_generating_pair(tc, w1, w2);

    section_hlt(tc);
    section_felsch(tc);
    section_CR_style(tc);
    section_Cr_style(tc);
    section_R_over_C_style(tc);
    section_Rc_style(tc);

    REQUIRE(tc.number_of_classes() == 21);
    REQUIRE(tc.number_of_classes() == 21);
    word_type w3 = froidure_pin::factorisation(
        S, S.position(make<Transf<>>({1, 3, 1, 3, 3})));
    word_type w4 = froidure_pin::factorisation(
        S, S.position(make<Transf<>>({4, 2, 4, 4, 2})));
    REQUIRE(index_of(tc, w3) == index_of(tc, w4));
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "011",
                          "finite fp-semigroup, dihedral group of order 6 ",
                          "[todd-coxeter][quick]") {
    auto rg = ReportGuard(false);

    Presentation<word_type> p;
    p.alphabet(5);
    presentation::add_identity_rules(p, 0);
    presentation::add_inverse_rules(p, 02143_w, 0);

    presentation::add_rule(p, 22_w, 0_w);
    presentation::add_rule(p, 14233_w, 0_w);
    presentation::add_rule(p, 444_w, 0_w);

    ToddCoxeter tc(twosided, p);
    section_felsch(tc);

    section_hlt(tc);
    section_Rc_style(tc);
    section_R_over_C_style(tc);
    section_CR_style(tc);
    section_Cr_style(tc);

    REQUIRE(tc.number_of_classes() == 6);
    REQUIRE(index_of(tc, {1}) == index_of(tc, {2}));
    REQUIRE(tc.word_graph().number_of_nodes() == 7);
    REQUIRE(tc.word_graph().target(0, 0) == 1);
    auto        pred = word_graph::ancestors_of_no_checks(tc.word_graph(), 1);
    std::vector result(pred.begin(), pred.end());
    std::sort(result.begin(), result.end());
    REQUIRE(result == std::vector<uint32_t>({0, 1, 2, 3, 4, 5, 6}));
    auto desc = word_graph::nodes_reachable_from(tc.word_graph(), 1);
    result.assign(desc.begin(), desc.end());
    std::sort(result.begin(), result.end());
    REQUIRE(result == std::vector<uint32_t>({1, 2, 3, 4, 5, 6}));
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "012",
                          "finite fp-semigroup, size 16",
                          "[todd-coxeter][quick]") {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.alphabet(4);
    presentation::add_rule(p, 3_w, 2_w);
    presentation::add_rule(p, 03_w, 02_w);
    presentation::add_rule(p, 11_w, 1_w);
    presentation::add_rule(p, 13_w, 12_w);
    presentation::add_rule(p, 21_w, 2_w);
    presentation::add_rule(p, 22_w, 2_w);
    presentation::add_rule(p, 23_w, 2_w);
    presentation::add_rule(p, 000_w, 0_w);
    presentation::add_rule(p, 001_w, 1_w);
    presentation::add_rule(p, 002_w, 2_w);
    presentation::add_rule(p, 012_w, 12_w);
    presentation::add_rule(p, 100_w, 1_w);
    presentation::add_rule(p, 102_w, 02_w);
    presentation::add_rule(p, 200_w, 2_w);
    presentation::add_rule(p, 0101_w, 101_w);
    presentation::add_rule(p, 0202_w, 202_w);
    presentation::add_rule(p, 1010_w, 101_w);
    presentation::add_rule(p, 1201_w, 101_w);
    presentation::add_rule(p, 1202_w, 202_w);
    presentation::add_rule(p, 2010_w, 201_w);
    presentation::add_rule(p, 2020_w, 202_w);

    ToddCoxeter tc(twosided, p);

    section_felsch(tc);
    section_hlt(tc);
    section_Rc_style(tc);
    section_R_over_C_style(tc);
    section_CR_style(tc);
    section_Cr_style(tc);

    REQUIRE(tc.number_of_classes() == 16);
    REQUIRE(index_of(tc, {2}) == index_of(tc, {3}));
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "013",
                          "finite fp-semigroup, size 16 x 2",
                          "[todd-coxeter][quick]") {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.alphabet(11);
    presentation::add_rule(p, {2}, {1});
    presentation::add_rule(p, {4}, {3});
    presentation::add_rule(p, {5}, {0});
    presentation::add_rule(p, {6}, {3});
    presentation::add_rule(p, {7}, {1});
    presentation::add_rule(p, {8}, {3});
    presentation::add_rule(p, {9}, {3});
    presentation::add_rule(p, {10}, {0});
    presentation::add_rule(p, {0, 2}, {0, 1});
    presentation::add_rule(p, {0, 4}, {0, 3});
    presentation::add_rule(p, {0, 5}, {0, 0});
    presentation::add_rule(p, {0, 6}, {0, 3});
    presentation::add_rule(p, {0, 7}, {0, 1});
    presentation::add_rule(p, {0, 8}, {0, 3});
    presentation::add_rule(p, {0, 9}, {0, 3});
    presentation::add_rule(p, {0, 10}, {0, 0});
    presentation::add_rule(p, {1, 1}, {1});
    presentation::add_rule(p, {1, 2}, {1});
    presentation::add_rule(p, {1, 4}, {1, 3});
    presentation::add_rule(p, {1, 5}, {1, 0});
    presentation::add_rule(p, {1, 6}, {1, 3});
    presentation::add_rule(p, {1, 7}, {1});
    presentation::add_rule(p, {1, 8}, {1, 3});
    presentation::add_rule(p, {1, 9}, {1, 3});
    presentation::add_rule(p, {1, 10}, {1, 0});
    presentation::add_rule(p, {3, 1}, {3});
    presentation::add_rule(p, {3, 2}, {3});
    presentation::add_rule(p, {3, 3}, {3});
    presentation::add_rule(p, {3, 4}, {3});
    presentation::add_rule(p, {3, 5}, {3, 0});
    presentation::add_rule(p, {3, 6}, {3});
    presentation::add_rule(p, {3, 7}, {3});
    presentation::add_rule(p, {3, 8}, {3});
    presentation::add_rule(p, {3, 9}, {3});
    presentation::add_rule(p, {3, 10}, {3, 0});
    presentation::add_rule(p, {0, 0, 0}, {0});
    presentation::add_rule(p, {0, 0, 1}, {1});
    presentation::add_rule(p, {0, 0, 3}, {3});
    presentation::add_rule(p, {0, 1, 3}, {1, 3});
    presentation::add_rule(p, {1, 0, 0}, {1});
    presentation::add_rule(p, {1, 0, 3}, {0, 3});
    presentation::add_rule(p, {3, 0, 0}, {3});
    presentation::add_rule(p, {0, 1, 0, 1}, {1, 0, 1});
    presentation::add_rule(p, {0, 3, 0, 3}, {3, 0, 3});
    presentation::add_rule(p, {1, 0, 1, 0}, {1, 0, 1});
    presentation::add_rule(p, {1, 3, 0, 1}, {1, 0, 1});
    presentation::add_rule(p, {1, 3, 0, 3}, {3, 0, 3});
    presentation::add_rule(p, {3, 0, 1, 0}, {3, 0, 1});
    presentation::add_rule(p, {3, 0, 3, 0}, {3, 0, 3});

    ToddCoxeter tc(twosided, p);

    section_felsch(tc);
    section_hlt(tc);
    section_Rc_style(tc);
    section_R_over_C_style(tc);
    section_CR_style(tc);
    section_Cr_style(tc);

    REQUIRE(tc.number_of_classes() == 16);
    REQUIRE(index_of(tc, {0}) == index_of(tc, {5}));
    REQUIRE(index_of(tc, {0}) == index_of(tc, {10}));
    REQUIRE(index_of(tc, {1}) == index_of(tc, {2}));
    REQUIRE(index_of(tc, {1}) == index_of(tc, {7}));
    REQUIRE(index_of(tc, {3}) == index_of(tc, {4}));
    REQUIRE(index_of(tc, {3}) == index_of(tc, {6}));
    REQUIRE(index_of(tc, {3}) == index_of(tc, {8}));
    REQUIRE(index_of(tc, {3}) == index_of(tc, {9}));
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "014",
                          "test lookahead",
                          "[todd-coxeter][quick]") {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.alphabet(2);
    presentation::add_rule(p, 000_w, 0_w);
    presentation::add_rule(p, 100_w, 10_w);
    presentation::add_rule(p, 10111_w, 10_w);
    presentation::add_rule(p, 11111_w, 11_w);
    presentation::add_rule(p, 110110_w, 101011_w);
    presentation::add_rule(p, 0010110_w, 010110_w);
    presentation::add_rule(p, 0011010_w, 011010_w);
    presentation::add_rule(p, 0101010_w, 101010_w);
    presentation::add_rule(p, 1010101_w, 101010_w);
    presentation::add_rule(p, 1010110_w, 101011_w);
    presentation::add_rule(p, 1011010_w, 101101_w);
    presentation::add_rule(p, 1101010_w, 101010_w);
    presentation::add_rule(p, 1111010_w, 1010_w);
    presentation::add_rule(p, 00111010_w, 111010_w);

    for (auto knd : {onesided, twosided}) {
      ToddCoxeter tc(knd, p);
      tc.lookahead_next(10);
      section_hlt(tc);
      REQUIRE(tc.number_of_classes() == 78);
      tc.standardize(Order::shortlex);
      REQUIRE(tc.spanning_tree().number_of_nodes() == 79);
      check_standardize(tc);
    }
    {
      presentation::reverse(p);
      ToddCoxeter tc(onesided, p);
      tc.lookahead_next(10);
      section_hlt(tc);
      REQUIRE(tc.number_of_classes() == 78);
      check_standardize(tc);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "015",
                          "2-sided cong. on free semigroup",
                          "[todd-coxeter][quick]") {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.alphabet(1);
    ToddCoxeter tc(twosided, p);

    section_hlt(tc);
    section_felsch(tc);
    section_CR_style(tc);
    section_Cr_style(tc);
    section_Rc_style(tc);
    section_R_over_C_style(tc);

    REQUIRE(contains(tc, 00_w, 00_w));
    REQUIRE(!contains(tc, 00_w, 0_w));
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "016",
                          "calling run when obviously infinite",
                          "[todd-coxeter][quick]") {
    Presentation<word_type> p;
    p.alphabet(5);
    ToddCoxeter tc(twosided, p);

    section_hlt(tc);
    section_felsch(tc);
    section_CR_style(tc);
    section_Cr_style(tc);
    section_Rc_style(tc);
    section_R_over_C_style(tc);

    REQUIRE_THROWS_AS(tc.run(), LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "017",
                          "stellar_monoid S3",
                          "[todd-coxeter][quick][hivert]") {
    auto rg = ReportGuard(false);

    Presentation<word_type> p;
    p.alphabet(4);
    presentation::add_rule(p, 33_w, 3_w);
    presentation::add_rule(p, 03_w, 0_w);
    presentation::add_rule(p, 30_w, 0_w);
    presentation::add_rule(p, 13_w, 1_w);
    presentation::add_rule(p, 31_w, 1_w);
    presentation::add_rule(p, 23_w, 2_w);
    presentation::add_rule(p, 32_w, 2_w);
    presentation::add_rule(p, 00_w, 0_w);
    presentation::add_rule(p, 11_w, 1_w);
    presentation::add_rule(p, 22_w, 2_w);
    presentation::add_rule(p, 02_w, 20_w);
    presentation::add_rule(p, 20_w, 02_w);
    presentation::add_rule(p, 121_w, 212_w);
    presentation::add_rule(p, 1010_w, 0101_w);
    presentation::add_rule(p, 1010_w, 010_w);

    ToddCoxeter tc(twosided, std::move(p));

    section_hlt(tc);
    section_felsch(tc);
    section_Rc_style(tc);
    section_R_over_C_style(tc);
    section_CR_style(tc);
    section_Cr_style(tc);

    REQUIRE(tc.number_of_classes() == 34);

    auto S = to<FroidurePin>(tc);
    REQUIRE(S.size() == 34);
    using detail::TCE;

    std::vector v(S.cbegin(), S.cend());
    std::sort(v.begin(), v.end());
    REQUIRE(
        v == std::vector({TCE(1),  TCE(2),  TCE(3),  TCE(4),  TCE(5),  TCE(6),
                          TCE(7),  TCE(8),  TCE(9),  TCE(10), TCE(11), TCE(12),
                          TCE(13), TCE(14), TCE(15), TCE(16), TCE(17), TCE(18),
                          TCE(19), TCE(20), TCE(21), TCE(22), TCE(23), TCE(24),
                          TCE(25), TCE(26), TCE(27), TCE(28), TCE(29), TCE(30),
                          TCE(31), TCE(32), TCE(33), TCE(34)}));
    REQUIRE(std::vector(S.cbegin_sorted(), S.cend_sorted())
            == std::vector<TCE>(
                {TCE(1),  TCE(2),  TCE(3),  TCE(4),  TCE(5),  TCE(6),  TCE(7),
                 TCE(8),  TCE(9),  TCE(10), TCE(11), TCE(12), TCE(13), TCE(14),
                 TCE(15), TCE(16), TCE(17), TCE(18), TCE(19), TCE(20), TCE(21),
                 TCE(22), TCE(23), TCE(24), TCE(25), TCE(26), TCE(27), TCE(28),
                 TCE(29), TCE(30), TCE(31), TCE(32), TCE(33), TCE(34)}));
    REQUIRE(detail::to_string(TCE(1)) == "1");
    REQUIRE_NOTHROW(IncreaseDegree<TCE>()(TCE(1), 10));

    std::ostringstream oss;
    oss << TCE(10);  // Does not do anything visible

    std::stringbuf buf;
    std::ostream   os(&buf);
    os << TCE(32);  // Does not do anything visible
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "018",
                          "finite semigroup (size 5)",
                          "[todd-coxeter][quick]") {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    presentation::add_rule_no_checks(p, 000_w, 0_w);
    presentation::add_rule_no_checks(p, 0_w, 11_w);
    p.alphabet_from_rules();
    presentation::reverse(p);

    ToddCoxeter tc(onesided, p);

    section_hlt(tc);
    section_felsch(tc);
    section_Rc_style(tc);
    section_R_over_C_style(tc);
    section_CR_style(tc);
    section_Cr_style(tc);

    REQUIRE(tc.number_of_classes() == 5);
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "019",
                          "exceptions",
                          "[todd-coxeter][quick]") {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.alphabet(2);
    presentation::add_rule(p, 000_w, 0_w);
    presentation::add_rule(p, 0_w, 11_w);

    ToddCoxeter tc1(onesided, p);
    REQUIRE(tc1.number_of_classes() == 5);

    REQUIRE_THROWS_AS(ToddCoxeter(twosided, tc1), LibsemigroupsException);

    ToddCoxeter tc2(onesided, tc1);
    REQUIRE(!contains(tc1, 0_w, 1_w));
    todd_coxeter::add_generating_pair(tc2, 0_w, 1_w);

    section_felsch(tc2);
    section_hlt(tc2);
    section_Rc_style(tc2);
    section_R_over_C_style(tc2);
    section_CR_style(tc2);
    section_Cr_style(tc2);

    REQUIRE(tc2.number_of_classes() == 1);
    tc2.shrink_to_fit();

    presentation::add_rule(p, 0_w, 1_w);
    ToddCoxeter tc3(onesided, p);
    REQUIRE(tc3.number_of_classes() == 1);
    tc3.shrink_to_fit();
    REQUIRE(tc3.current_word_graph() == tc2.current_word_graph());

    p.rules.clear();
    tc3.init(twosided, p);
    REQUIRE(tc3.presentation().rules.empty());
    REQUIRE(tc3.number_of_generating_pairs() == 0);

    REQUIRE_THROWS_AS(tc3.run_for(std::chrono::nanoseconds(1)),
                      LibsemigroupsException);
    REQUIRE(tc3.current_word_graph().number_of_nodes_active() == 1);
    REQUIRE(tc3.current_word_graph().number_of_edges() == 0);
    REQUIRE(!tc3.finished());
    // This shouldn't ever return true, since the presentation defines the free
    // semigroup with 2 generators
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "020",
                          "obviously infinite",
                          "[todd-coxeter][quick]") {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.alphabet(3);
    presentation::add_rule_no_checks(p, 000_w, 0_w);
    for (auto knd : {onesided, twosided}) {
      ToddCoxeter tc(knd, p);

      section_hlt(tc);
      section_felsch(tc);
      section_CR_style(tc);
      section_Cr_style(tc);
      section_R_over_C_style(tc);
      section_Rc_style(tc);

      REQUIRE(tc.number_of_classes() == POSITIVE_INFINITY);
      REQUIRE(is_obviously_infinite(tc));
    }
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "021",
                          "exceptions x 2",
                          "[todd-coxeter][quick]") {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    presentation::add_rule_no_checks(p, 000_w, 0_w);
    presentation::add_rule_no_checks(p, 0_w, 11_w);
    p.alphabet_from_rules();

    {
      ToddCoxeter tc(onesided, p);
      section_hlt(tc);
      section_felsch(tc);
      section_Rc_style(tc);
      section_R_over_C_style(tc);
      section_CR_style(tc);
      section_Cr_style(tc);

      REQUIRE(tc.number_of_classes() == 5);
      REQUIRE(word_of(tc, 0) == 0_w);
      // This next one should throw
      REQUIRE_THROWS_AS(to<FroidurePin>(tc), LibsemigroupsException);
    }
    {
      ToddCoxeter tc(twosided, p);
      section_hlt(tc);
      section_felsch(tc);
      section_Rc_style(tc);
      section_R_over_C_style(tc);
      section_CR_style(tc);
      section_Cr_style(tc);

      REQUIRE(tc.number_of_classes() == 5);
      REQUIRE(word_of(tc, 0) == 0_w);
      REQUIRE(word_of(tc, 1) == 1_w);
      REQUIRE(word_of(tc, 2) == 00_w);
      REQUIRE(word_of(tc, 3) == 01_w);
      REQUIRE(word_of(tc, 4) == 001_w);
      REQUIRE_THROWS_AS(word_of(tc, 5), LibsemigroupsException);
      REQUIRE_THROWS_AS(word_of(tc, 100), LibsemigroupsException);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "022",
                          "quotient ToddCoxeter",
                          "[todd-coxeter][quick]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("ab");
    presentation::add_rule(p, "aaa", "a");
    presentation::add_rule(p, "a", "bb");
    presentation::reverse(p);
    ToddCoxeter tc1(twosided, p);
    REQUIRE(tc1.number_of_classes() == 5);
    ToddCoxeter tc2(onesided, tc1);
    todd_coxeter::add_generating_pair(tc2, "a", "b");
    REQUIRE_THROWS_AS(todd_coxeter::add_generating_pair(tc2, "a", "c"),
                      LibsemigroupsException);

    section_hlt(tc2);
    section_felsch(tc2);
    section_CR_style(tc2);
    section_R_over_C_style(tc2);
    section_Cr_style(tc2);
    section_Rc_style(tc2);

    REQUIRE(tc2.number_of_classes() == 1);
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "023",
                          "from KnuthBendix",
                          "[todd-coxeter][quick]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("abB");
    presentation::add_rule_no_checks(p, "bb", "B");
    presentation::add_rule_no_checks(p, "BaB", "aba");

    KnuthBendix kb(twosided, p);
    SECTION("not started. . .") {
      REQUIRE(!kb.confluent());
      REQUIRE(!kb.started());
    }
    SECTION("finished . . .") {
      kb.run();
      REQUIRE(kb.confluent());
      REQUIRE(kb.number_of_active_rules() == 6);
      REQUIRE(kb.finished());
    }

    ToddCoxeter tc(twosided, kb.presentation());

    section_hlt(tc);
    section_felsch(tc);
    section_CR_style(tc);
    section_R_over_C_style(tc);
    section_Cr_style(tc);
    section_Rc_style(tc);

    todd_coxeter::add_generating_pair(tc, "b", "B");
    REQUIRE(is_obviously_infinite(tc));
    REQUIRE(tc.number_of_classes() == POSITIVE_INFINITY);
    REQUIRE(tc.presentation().rules
            == std::vector<std::string>(
                {{98, 98}, {66}, {66, 97, 66}, {97, 98, 97}}));
    REQUIRE(tc.generating_pairs() == std::vector<std::string>({{98}, {66}}));
    REQUIRE(!tc.finished());
    REQUIRE(!tc.started());
    todd_coxeter::add_generating_pair(tc, "b", "a");
    REQUIRE(!is_obviously_infinite(tc));

    REQUIRE(tc.number_of_classes() == 1);

    REQUIRE(todd_coxeter::word_of(tc, 0) == "a");
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "024",
                          "from WordGraph",
                          "[todd-coxeter][quick]") {
    auto rg = ReportGuard(false);

    WordGraph<uint32_t> wg(1, 2);
    REQUIRE(wg.out_degree() == 2);
    REQUIRE(wg.number_of_nodes() == 1);
    REQUIRE_NOTHROW(ToddCoxeter<word_type>(twosided, wg));
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "025",
                          "congruence of ToddCoxeter",
                          "[todd-coxeter][quick]") {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.alphabet(2);
    presentation::add_rule(p, 000_w, 0_w);
    presentation::add_rule(p, 0_w, 11_w);
    presentation::reverse(p);
    ToddCoxeter tc1(twosided, p);
    REQUIRE(tc1.number_of_classes() == 5);

    ToddCoxeter tc2(onesided, tc1);
    tc2.lookahead_next(1);
    todd_coxeter::add_generating_pair(tc2, 0_w, 00_w);

    section_hlt(tc2);
    section_felsch(tc2);

    REQUIRE(tc2.number_of_classes() == 3);
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "026",
                          "congruence of ToddCoxeter x 2",
                          "[todd-coxeter][quick]") {
    auto rg               = ReportGuard(false);
    using Transf          = LeastTransf<5>;
    FroidurePin<Transf> S = make<FroidurePin>(
        {make<Transf>({1, 3, 4, 2, 3}), make<Transf>({3, 2, 1, 3, 3})});
    REQUIRE(S.size() == 88);
    REQUIRE(S.number_of_rules() == 18);
    auto tc = to<ToddCoxeter<word_type>>(
        twosided, S, S.right_cayley_graph());  // use Cayley graph
    todd_coxeter::add_generating_pair(tc, {0}, {1, 1});

    section_felsch(tc);
    section_hlt(tc);
    section_CR_style(tc);
    section_Cr_style(tc);
    section_R_over_C_style(tc);
    section_Rc_style(tc);

    REQUIRE(tc.number_of_classes() == 1);
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "027",
                          "congruence over fp semigroup",
                          "[todd-coxeter][quick]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("abe");
    presentation::add_identity_rules(p, 'e');
    presentation::add_rule(p, "abb", "bb");
    presentation::add_rule(p, "bbb", "bb");
    presentation::add_rule(p, "aaaa", "a");
    presentation::add_rule(p, "baab", "bb");
    presentation::add_rule(p, "baaab", "b");
    presentation::add_rule(p, "babab", "b");
    presentation::add_rule(p, "bbaaa", "bb");
    presentation::add_rule(p, "bbaba", "bbaa");
    presentation::reverse(p);

    ToddCoxeter tc1(onesided, p);
    todd_coxeter::add_generating_pair(tc1, "a", "bbb");

    section_hlt(tc1);
    // section_felsch(tc1);
    // section_Rc_style(tc1);
    // section_R_over_C_style(tc1);
    // section_CR_style(tc1);
    // section_Cr_style(tc1);

    REQUIRE(tc1.number_of_classes() == 11);

    REQUIRE((normal_forms(tc1) | to_vector())
            == std::vector<std::string>({"a",
                                         "b",
                                         "e",
                                         "ba",
                                         "baa",
                                         "bab",
                                         "baaa",
                                         "baba",
                                         "babb",
                                         "babaa",
                                         "babaaa"}));

    ToddCoxeter tc2(twosided, p);
    REQUIRE(tc2.number_of_classes() == 40);
    auto part = partition(tc1, normal_forms(tc2));
    REQUIRE(part
            == decltype(part)(
                {{"a",       "aa",      "ab",       "bb",       "aaa",
                  "aab",     "aba",     "abb",      "aaab",     "aaba",
                  "aabb",    "abaa",    "abab",     "aaaba",    "aabaa",
                  "aabab",   "abaaa",   "ababa",    "aaabaa",   "aaabab",
                  "aabaaa",  "aababa",  "ababaa",   "aaabaaa",  "aaababa",
                  "aababaa", "ababaaa", "aaababaa", "aababaaa", "aaababaaa"},
                 {"b"},
                 {"e"},
                 {"ba"},
                 {"baa"},
                 {"bab"},
                 {"baaa"},
                 {"baba"},
                 {"babb"},
                 {"babaa"},
                 {"babaaa"}}));
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "028",
                          "copy constructor",
                          "[todd-coxeter][quick]") {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.alphabet(2);
    presentation::add_rule(p, 0_w, 1_w);
    presentation::add_rule(p, 00_w, 0_w);

    ToddCoxeter tc(twosided, p);
    tc.strategy(options::strategy::felsch);
    REQUIRE(tc.strategy() == options::strategy::felsch);
    auto const& d = tc.current_word_graph();
    REQUIRE(!word_graph::is_complete(
        d, d.cbegin_active_nodes(), d.cend_active_nodes()));
    REQUIRE(word_graph::is_compatible_no_checks(d,
                                                d.cbegin_active_nodes(),
                                                d.cend_active_nodes(),
                                                p.rules.cbegin(),
                                                p.rules.cend()));
    REQUIRE(tc.number_of_classes() == 1);
    REQUIRE((normal_forms(tc) | to_vector()) == std::vector<word_type>({0_w}));
    REQUIRE(word_graph::is_complete(
        d, d.cbegin_active_nodes(), d.cend_active_nodes()));
    REQUIRE(word_graph::is_compatible_no_checks(d,
                                                d.cbegin_active_nodes(),
                                                d.cend_active_nodes(),
                                                p.rules.cbegin(),
                                                p.rules.cend()));

    ToddCoxeter copy(tc);
    REQUIRE(copy.presentation().rules == p.rules);
    REQUIRE(copy.presentation().alphabet() == p.alphabet());
    REQUIRE(copy.finished());
    REQUIRE(copy.number_of_classes() == 1);

    auto const& dd = copy.current_word_graph();
    REQUIRE(word_graph::is_complete(
        dd, dd.cbegin_active_nodes(), dd.cend_active_nodes()));
    REQUIRE(word_graph::is_compatible_no_checks(dd,
                                                dd.cbegin_active_nodes(),
                                                dd.cend_active_nodes(),
                                                p.rules.cbegin(),
                                                p.rules.cend()));
    REQUIRE(tc.current_word_graph() == copy.current_word_graph());
  }

  LIBSEMIGROUPS_TEST_CASE(
      "ToddCoxeter",
      "029",
      "stylic_monoid",
      "[todd-coxeter][standard][no-coverage][no-valgrind]") {
    auto rg = ReportGuard(false);

    auto p = presentation::examples::stylic_monoid(9);
    REQUIRE(presentation::length(p) == 1'467);
    presentation::remove_trivial_rules(p);
    presentation::remove_duplicate_rules(p);
    presentation::sort_rules(p);
    presentation::sort_each_rule(p);
    REQUIRE(presentation::length(p) == 1'467);

    ToddCoxeter tc(twosided, p);
    // SECTION("preprocessing + Felsch . . .") {
    //   tc.init(twosided, p);
    //   tc.strategy(options::strategy::felsch);  // about 2.5s
    // }
    SECTION("default HLT") {
      tc.strategy(options::strategy::hlt);
    }
    SECTION("custom CR style") {
      tc.strategy(options::strategy::CR)
          .f_defs(2'000)
          .hlt_defs(100'000'000'000);
    }
    // section_Cr_style(tc);        // about 2.5s
    // section_Rc_style(tc);        // about 2.5s
    section_R_over_C_style(tc);

    REQUIRE(tc.number_of_classes() == 115'975);
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "030",
                          "fibonacci_semigroup(4, 6)",
                          "[todd-coxeter][fail]") {
    auto        rg = ReportGuard();
    ToddCoxeter tc(twosided, presentation::examples::fibonacci_semigroup(4, 6));
    // tc.strategy(ptions::strategy::felsch);
    REQUIRE(tc.number_of_classes() == 0);
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "031",
                          "some finite classes",
                          "[todd-coxeter][quick]") {
    auto rg = ReportGuard(false);

    Presentation<word_type> p;
    p.alphabet(1);

    ToddCoxeter tc(twosided, p);
    REQUIRE(tc.number_of_classes() == POSITIVE_INFINITY);

    presentation::add_rule(p, 000000_w, 0000_w);
    presentation::add_rule(p, 000000_w, 0000_w);
    presentation::add_rule(p, 000000_w, 0000_w);
    presentation::remove_duplicate_rules(p);

    tc.init(twosided, p);

    auto const& d = tc.current_word_graph();
    REQUIRE(word_graph::is_compatible_no_checks(d,
                                                d.cbegin_active_nodes(),
                                                d.cend_active_nodes(),
                                                p.rules.cbegin(),
                                                p.rules.cend()));
    tc.strategy(options::strategy::CR);
    size_t x = 0;
    REQUIRE_THROWS_AS(tc.run_until([&x] { return x > 4; }),
                      LibsemigroupsException);
    tc.lower_bound(100)
        .use_relations_in_extra(true)
        .def_policy(options::def_policy::unlimited)
        .def_version(options::def_version::one);
    REQUIRE_THROWS_AS(tc.hlt_defs(0), LibsemigroupsException);
    REQUIRE_THROWS_AS(tc.f_defs(0), LibsemigroupsException);
    tc.hlt_defs(10)
        .f_defs(10)
        .lookahead_growth_factor(3.0)
        .lookahead_growth_threshold(100'000)
        .large_collapse(1);
    REQUIRE_THROWS_AS(tc.lookahead_growth_factor(0.1), LibsemigroupsException);

    REQUIRE(tc.current_word_graph().felsch_tree().height() == 6);
    REQUIRE(tc.number_of_classes() == 5);

    auto c = class_by_index(tc, 0);
    REQUIRE(c.get() == 0_w);
    REQUIRE(!c.at_end());
    REQUIRE(Paths(tc.current_word_graph()).source(0).target(1).count() == 1);
    auto pp = Paths(tc.current_word_graph()).source(0).target(1);
    REQUIRE(pp.order() == Order::shortlex);
    REQUIRE(pp.get() == 0_w);
    REQUIRE(!pp.at_end());
    pp.next();
    c.next();
    REQUIRE(c.at_end());
    // TODO(1) the following no longer work
    // REQUIRE((class_by_index(tc, 0) | rx::count()) == 1);
    // REQUIRE((class_by_index(tc, 1) | rx::count()) == 1);
    // REQUIRE((class_by_index(tc, 2) | rx::count()) == 1);
    // REQUIRE(class_by_index(tc, 3).count() == POSITIVE_INFINITY);
    // REQUIRE(class_by_index(tc, 4).count() == POSITIVE_INFINITY);

    REQUIRE(!p.contains_empty_word());
    REQUIRE_THROWS_AS(class_of(tc, ""_w), LibsemigroupsException);
    REQUIRE_THROWS_AS(class_of(tc, {}), LibsemigroupsException);

    REQUIRE(!tc.is_standardized());
    REQUIRE(tc.current_word_graph().felsch_tree().number_of_nodes() == 7);

    ToddCoxeter tc2(onesided, tc);
    todd_coxeter::add_generating_pair(tc2, 00_w, 0_w);
    todd_coxeter::add_generating_pair(tc2, 00_w, 0_w);

    // Uses detail::CongruenceCommon's generating pairs
    REQUIRE(tc2.number_of_generating_pairs() == 2);

    p.init();
    p.alphabet(1);

    ToddCoxeter tc3(twosided, p);
    REQUIRE(is_non_trivial(tc3) == tril::TRUE);
    todd_coxeter::add_generating_pair(tc3, 00_w, 0_w);

    REQUIRE(is_non_trivial(tc3) == tril::unknown);
    REQUIRE(tc3.number_of_classes() == 1);
    REQUIRE(is_non_trivial(tc3) == tril::FALSE);
  }

  // Takes about 1.8s
  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "032",
                          "symmetric_group(9) Moore_b",
                          "[todd-coxeter][standard]") {
    auto rg = ReportGuard(false);

    auto p = presentation::examples::symmetric_group_Moo97_b(9);
    presentation::reduce_complements(p);
    presentation::remove_duplicate_rules(p);
    presentation::sort_rules(p);
    presentation::sort_each_rule(p);
    std::reverse(p.rules.begin(), p.rules.end());

    REQUIRE(presentation::length(p) == 570);
    ToddCoxeter tc(twosided, p);

    SECTION("preprocess + Felsch") {
      presentation::greedy_reduce_length(p);
      REQUIRE(presentation::length(p) == 79);
      REQUIRE(p.alphabet().size() == 10);
      tc.init(twosided, p);
      tc.strategy(options::strategy::felsch);
    }
    // section_hlt(tc); // slow
    // section_CR_style(tc); // slow
    // section_Cr_style(tc); // slow
    // section_Rc_style(tc); // about 5s
    REQUIRE(tc.number_of_classes() == 362'880);
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "033",
                          "symmetric_group(7) Moore_a",
                          "[todd-coxeter][quick][no-valgrind]") {
    auto rg = ReportGuard(false);

    size_t n = 7;
    auto   p = presentation::examples::symmetric_group_Moo97_a(n);

    ToddCoxeter tc(twosided, p);

    section_hlt(tc);
    section_felsch(tc);
    section_CR_style(tc);
    section_Cr_style(tc);
    section_R_over_C_style(tc);
    section_Rc_style(tc);

    tc.run_for(std::chrono::microseconds(1));
    REQUIRE(is_non_trivial(tc) == tril::TRUE);
    // REQUIRE(!tc.finished());
    tc.standardize(Order::shortlex);
    tc.standardize(Order::none);

    REQUIRE(tc.number_of_classes() == 5'040);
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "034",
                          "symmetric_group(7) Burnside",
                          "[todd-coxeter][quick][no-valgrind]") {
    auto rg = ReportGuard(false);

    size_t n = 7;
    auto   p = presentation::examples::symmetric_group_Bur12(n);

    ToddCoxeter tc(twosided, p);
    section_hlt(tc);
    section_felsch(tc);
    section_CR_style(tc);
    section_Cr_style(tc);
    section_R_over_C_style(tc);
    section_Rc_style(tc);

    REQUIRE(tc.number_of_classes() == 5'040);
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "035",
                          "Easdown-East-FitzGerald DualSymInv(5)",
                          "[todd-coxeter][quick][no-coverage][no-valgrind]") {
    auto       rg = ReportGuard(false);
    auto const n  = 5;
    auto p = presentation::examples::dual_symmetric_inverse_monoid_EEF07(n);
    ToddCoxeter tc(twosided, p);

    section_felsch(tc);
    section_hlt(tc);
    section_CR_style(tc);
    section_Cr_style(tc);
    section_R_over_C_style(tc);
    section_Rc_style(tc);

    REQUIRE(tc.number_of_classes() == 6'721);
    check_complete_compatible(tc);
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "036",
                          "uniform_block_bijection_monoid(3) (FitzGerald) ",
                          "[todd-coxeter][quick][no-valgrind]") {
    // 16, 131, 1496, 22482, 426833, 9934563, 9934563
    auto       rg = ReportGuard(false);
    auto const n  = 5;

    auto p = presentation::examples::uniform_block_bijection_monoid_Fit03(n);

    ToddCoxeter tc(twosided, p);

    section_hlt(tc);
    section_felsch(tc);
    section_CR_style(tc);
    section_Cr_style(tc);
    section_R_over_C_style(tc);
    section_Rc_style(tc);
    REQUIRE(tc.number_of_classes() == 1496);
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "037",
                          "stellar_monoid(7) (Gay-Hivert)",
                          "[todd-coxeter][quick][no-coverage][no-valgrind]") {
    auto         rg = ReportGuard(false);
    size_t const n  = 7;
    auto         p  = presentation::examples::stellar_monoid_GH19(n);

    ToddCoxeter tc(congruence_kind::twosided, p);

    section_hlt(tc);
    section_felsch(tc);
    section_CR_style(tc);
    section_Cr_style(tc);
    section_R_over_C_style(tc);
    section_Rc_style(tc);

    REQUIRE(tc.number_of_classes() == 13'700);
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "038",
                          "partition_monoid(4) (East)",
                          "[todd-coxeter][quick][no-valgrind][no-coverage]") {
    auto rg = ReportGuard(false);

    auto const  n = 4;
    auto        p = presentation::examples::partition_monoid_Eas11(n);
    ToddCoxeter tc(twosided, p);

    section_felsch(tc);
    section_hlt(tc);
    section_CR_style(tc);
    section_R_over_C_style(tc);
    section_Cr_style(tc);
    section_Rc_style(tc);

    check_complete_compatible(tc);
    REQUIRE(tc.number_of_classes() == 4'140);
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "039",
                          "singular_brauer_monoid(6) (Maltcev + Mazorchuk)",
                          "[todd-coxeter][quick][no-valgrind][no-coverage]") {
    auto         rg = ReportGuard(false);
    size_t const n  = 3;
    auto         p  = presentation::examples::singular_brauer_monoid_MM07(n);
    presentation::remove_redundant_generators(p);

    // presentation::remove_duplicate_rules(p);
    // presentation::sort_rules(p);
    // presentation::sort_each_rule(p);
    // REQUIRE(!p.contains_empty_word());
    REQUIRE(p.alphabet() == "013"_w);
    REQUIRE(!is_obviously_infinite(p));
    p.throw_if_bad_alphabet_or_rules();

    ToddCoxeter tc(twosided, p);
    REQUIRE(tc.number_of_classes() == 10);
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "040",
                          "orientation_preserving_monoid(6) (Ruskuc + Arthur)",
                          "[todd-coxeter][quick][no-valgrind][no-coverage]") {
    auto         rg = ReportGuard(false);
    size_t const n  = 4;
    auto p = presentation::examples::orientation_preserving_monoid_AR00(n);
    ToddCoxeter tc(congruence_kind::twosided, p);
    tc.strategy(options::strategy::hlt)
        .lookahead_extent(options::lookahead_extent::partial);

    section_hlt(tc);
    section_felsch(tc);
    section_CR_style(tc);
    section_Cr_style(tc);
    section_R_over_C_style(tc);
    section_Rc_style(tc);

    REQUIRE(tc.number_of_classes() == 128);
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "041",
                          "POPR(5) (Ruskuc + Arthur)",
                          "[todd-coxeter][quick][no-valgrind][no-coverage]") {
    auto         rg = ReportGuard(false);
    size_t const n  = 5;
    auto         p
        = presentation::examples::orientation_preserving_reversing_monoid_AR00(
            n);
    ToddCoxeter tc(congruence_kind::twosided, p);

    section_hlt(tc);
    section_felsch(tc);
    section_Rc_style(tc);
    section_R_over_C_style(tc);
    section_CR_style(tc);
    section_Cr_style(tc);

    REQUIRE(tc.number_of_classes() == 1'015);
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "042",
                          "temperley_lieb_monoid(10) (East)",
                          "[todd-coxeter][quick][no-coverage][no-valgrind]") {
    auto         rg = ReportGuard(false);
    size_t const n  = 10;
    auto         p  = presentation::examples::temperley_lieb_monoid_Eas21(n);

    REQUIRE(presentation::length(p) == 315);
    presentation::reduce_complements(p);
    presentation::remove_trivial_rules(p);
    presentation::remove_duplicate_rules(p);
    REQUIRE(presentation::length(p) == 203);

    ToddCoxeter tc(congruence_kind::twosided, p);

    section_felsch(tc);
    section_hlt(tc);
    section_CR_style(tc);
    section_Cr_style(tc);
    section_R_over_C_style(tc);
    section_Rc_style(tc);

    REQUIRE(tc.number_of_classes() == 16'796);
  }

  LIBSEMIGROUPS_TEST_CASE(
      "ToddCoxeter",
      "043",
      "Generate GAP benchmarks for stellar_monoid(n) (Gay-Hivert)",
      "[todd-coxeter][fail]") {
    auto rg = ReportGuard(true);
    for (size_t n = 3; n <= 9; ++n) {
      auto        p = presentation::examples::stellar_monoid_GH19(n);
      ToddCoxeter tc(congruence_kind::twosided, p);
      output_gap_benchmark_file("stellar-" + std::to_string(n) + ".g", tc);
    }
  }

  LIBSEMIGROUPS_TEST_CASE(
      "ToddCoxeter",
      "044",
      "Generate GAP benchmarks for partition_monoid(n) (East)",
      "[todd-coxeter][fail]") {
    auto rg = ReportGuard(true);

    for (size_t n = 4; n <= 6; ++n) {
      auto        p = presentation::examples::partition_monoid_Eas11(n);
      ToddCoxeter tc(congruence_kind::twosided, p);
      tc.save(true);
      output_gap_benchmark_file("partition-" + std::to_string(n) + ".g", tc);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "045",
                          "Generate GAP benchmarks for dual symmetric inverse "
                          "monoid (Easdown + East + FitzGerald)",
                          "[todd-coxeter][fail]") {
    auto rg = ReportGuard(true);

    for (size_t n = 3; n <= 6; ++n) {
      auto p = presentation::examples::dual_symmetric_inverse_monoid_EEF07(n);
      ToddCoxeter tc(congruence_kind::twosided, p);
      output_gap_benchmark_file("dual-sym-inv-" + std::to_string(n) + ".g", tc);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "046",
                          "Generate GAP benchmarks for "
                          "uniform_block_bijection_monoid (FitzGerald)",
                          "[todd-coxeter][fail]") {
    auto rg = ReportGuard(true);

    for (size_t n = 3; n <= 7; ++n) {
      auto p = presentation::examples::uniform_block_bijection_monoid_Fit03(n);
      ToddCoxeter tc(congruence_kind::twosided, p);
      output_gap_benchmark_file(
          "uniform-block-bijection-" + std::to_string(n) + ".g", tc);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "047",
                          "Generate GAP benchmarks for stylic monoids",
                          "[todd-coxeter][fail]") {
    auto rg = ReportGuard(true);
    for (size_t n = 3; n <= 9; ++n) {
      auto        p = presentation::examples::stylic_monoid(n);
      ToddCoxeter tc(congruence_kind::twosided, p);
      output_gap_benchmark_file("stylic-" + std::to_string(n) + ".g", tc);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "048",
                          "Generate GAP benchmarks for OP_n",
                          "[todd-coxeter][fail]") {
    auto rg = ReportGuard(true);
    for (size_t n = 3; n <= 9; ++n) {
      auto        p = presentation::examples::orientation_preserving_monoid(n);
      ToddCoxeter tc(congruence_kind::twosided, p);
      output_gap_benchmark_file("orient-" + std::to_string(n) + ".g", tc);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "049",
                          "Generate GAP benchmarks for OR_n",
                          "[todd-coxeter][fail]") {
    auto rg = ReportGuard(true);
    for (size_t n = 3; n <= 8; ++n) {
      auto p
          = presentation::examples::orientation_preserving_reversing_monoid(n);
      ToddCoxeter tc(congruence_kind::twosided, p);
      output_gap_benchmark_file("orient-reverse-" + std::to_string(n) + ".g",
                                tc);
    }
  }

  LIBSEMIGROUPS_TEST_CASE(
      "ToddCoxeter",
      "050",
      "Generate GAP benchmarks for temperley_lieb_monoid(n)",
      "[todd-coxeter][fail]") {
    auto rg = ReportGuard(true);
    for (size_t n = 3; n <= 13; ++n) {
      auto        p = presentation::examples::temperley_lieb_monoid(n);
      ToddCoxeter tc(congruence_kind::twosided, p);
      output_gap_benchmark_file("temperley-lieb-" + std::to_string(n) + ".g",
                                tc);
    }
  }

  LIBSEMIGROUPS_TEST_CASE(
      "ToddCoxeter",
      "051",
      "Generate GAP benchmarks for singular_brauer_monoid(n)",
      "[todd-coxeter][fail]") {
    auto rg = ReportGuard(true);
    for (size_t n = 3; n <= 7; ++n) {
      auto        p = presentation::examples::singular_brauer_monoid(n);
      ToddCoxeter tc(congruence_kind::twosided, p);
      output_gap_benchmark_file("singular-brauer-" + std::to_string(n) + ".g",
                                tc);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "052",
                          "partition_monoid(2)",
                          "[todd-coxeter][quick]") {
    auto rg = ReportGuard(false);
    auto p  = presentation::examples::partition_monoid_HR05(2);

    ToddCoxeter tc(congruence_kind::twosided, p);
    section_felsch(tc);
    section_hlt(tc);
    section_CR_style(tc);
    section_Cr_style(tc);
    section_R_over_C_style(tc);
    section_Rc_style(tc);
    REQUIRE(tc.number_of_classes() == 15);
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "053",
                          "brauer_monoid(4) (Kudryavtseva + Mazorchuk)",
                          "[todd-coxeter][quick][no-valgrind][no-coverage]") {
    auto         rg = ReportGuard(false);
    size_t const n  = 4;
    auto         p  = presentation::examples::brauer_monoid_KM07(n);
    presentation::sort_rules(p);
    presentation::remove_duplicate_rules(p);
    ToddCoxeter tc(congruence_kind::twosided, p);
    section_felsch(tc);
    section_hlt(tc);
    section_CR_style(tc);
    section_Cr_style(tc);
    section_R_over_C_style(tc);
    section_Rc_style(tc);
    REQUIRE(tc.number_of_classes() == 105);
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "054",
                          "symmetric_inverse_monoid(5) Shutov",
                          "[todd-coxeter][quick][no-valgrind]") {
    auto rg = ReportGuard(false);
    auto p  = presentation::examples::symmetric_inverse_monoid_Shu60(5);

    ToddCoxeter tc(congruence_kind::twosided, p);
    section_felsch(tc);
    section_hlt(tc);
    section_CR_style(tc);
    section_Cr_style(tc);
    section_R_over_C_style(tc);
    section_Rc_style(tc);
    REQUIRE(tc.number_of_classes() == 1'546);
  }

  LIBSEMIGROUPS_TEST_CASE(
      "ToddCoxeter",
      "055",
      "partial_transformation_monoid(5) Shutov",
      "[todd-coxeter][standard][no-coverage][no-valgrind]") {
    auto   rg = ReportGuard(false);
    size_t n  = 5;
    auto   p  = presentation::examples::partial_transformation_monoid_Shu60(n);

    ToddCoxeter tc(congruence_kind::twosided, p);
    section_felsch(tc);
    section_hlt(tc);
    section_CR_style(tc);
    section_Cr_style(tc);
    section_R_over_C_style(tc);
    section_Rc_style(tc);
    REQUIRE(tc.number_of_classes() == 7'776);
  }

  // stop_early in lookahead really helps in this example
  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "056",
                          "full_transformation_monoid(7) Iwahori",
                          "[todd-coxeter][extreme]") {
    auto   rg = ReportGuard(true);
    size_t n  = 7;
    auto   p  = presentation::examples::full_transformation_monoid_II74(n);

    REQUIRE(presentation::length(p) == 69'656);
    presentation::remove_redundant_generators(p);
    REQUIRE(presentation::length(p) == 69'020);
    presentation::reduce_complements(p);
    REQUIRE(presentation::length(p) == 45'380);
    presentation::remove_trivial_rules(p);
    REQUIRE(presentation::length(p) == 45'380);
    presentation::remove_duplicate_rules(p);
    REQUIRE(presentation::length(p) == 45'380);
    // presentation::greedy_reduce_length(p);
    // REQUIRE(presentation::length(p) == 8'515);

    ToddCoxeter tc(congruence_kind::twosided, p);

    // TODO(1) should be some interplay between lookahead_min and
    // lookahead_next, i.e.  lookahead_min shouldn't be allowed to be greater
    // than lookahead_next, maybe?
    tc.lookahead_min(2'500'000)
        .lookahead_growth_factor(1.2)
        .lookahead_stop_early_ratio(0.1);
    REQUIRE(tc.number_of_classes() == 823'543);
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "057",
                          "add_rule",
                          "[todd-coxeter][quick]") {
    auto rg = ReportGuard(false);
    {
      Presentation<std::string> p;
      p.alphabet("ab");
      presentation::add_rule(p, "aaa", "a");
      presentation::add_rule(p, "a", "bb");

      ToddCoxeter tc(twosided, p);
      section_hlt(tc);
      section_felsch(tc);
      section_Rc_style(tc);
      section_R_over_C_style(tc);
      section_CR_style(tc);
      section_Cr_style(tc);

      SECTION("R/C + Felsch lookahead") {
        tc.strategy(options::strategy::R_over_C)
            .lookahead_style(options::lookahead_style::felsch)
            .lookahead_extent(options::lookahead_extent::full);
        tc.run();
      }
      SECTION("HLT + Felsch lookahead + save") {
        tc.strategy(options::strategy::hlt)
            .save(true)
            .lookahead_style(options::lookahead_style::felsch)
            .lookahead_extent(options::lookahead_extent::full)
            .lookahead_next(2);
        tc.run();
      }
      SECTION("Cr + small number of f_defs") {
        tc.strategy(options::strategy::Cr).f_defs(3);
        tc.run();
      }
      SECTION("Rc + small number of deductions") {
        tc.strategy(options::strategy::Rc).def_max(0);
        tc.run();
      }
      SECTION("Felsch + v2 + purge_from_top") {
        tc.strategy(options::strategy::felsch)
            .def_version(options::def_version::two)
            .def_policy(options::def_policy::purge_from_top);
      }
      SECTION("Felsch + v2 + discard_all_if_no_space") {
        tc.strategy(options::strategy::felsch)
            .def_version(options::def_version::two)
            .def_policy(options::def_policy::discard_all_if_no_space);
      }
      SECTION("Felsch + v2 + no_stack_if_no_space") {
        tc.strategy(options::strategy::felsch)
            .def_version(options::def_version::two)
            .def_policy(options::def_policy::no_stack_if_no_space);
      }
      SECTION("Felsch + v1 + purge_from_top") {
        tc.strategy(options::strategy::felsch)
            .def_version(options::def_version::one)
            .def_policy(options::def_policy::purge_from_top);
      }
      SECTION("Felsch + v1 + discard_all_if_no_space") {
        tc.strategy(options::strategy::felsch)
            .def_version(options::def_version::one)
            .def_policy(options::def_policy::discard_all_if_no_space);
      }
      SECTION("Felsch + v1 + no_stack_if_no_space") {
        tc.strategy(options::strategy::felsch)
            .def_version(options::def_version::one)
            .def_policy(options::def_policy::no_stack_if_no_space);
      }
      SECTION("large collapse") {
        tc.large_collapse(0);
        REQUIRE(tc.number_of_classes() == 5);
      }
      {
        Presentation<std::string> q;
        q.alphabet("ab");
        presentation::add_rule(q, "aaa", "a");
        presentation::add_rule(q, "a", "bb");

        ToddCoxeter tcq(twosided, q);
        tcq.lookahead_next(1);

        section_hlt(tcq);
        section_felsch(tcq);
        section_Rc_style(tcq);
        section_R_over_C_style(tcq);
        section_CR_style(tcq);
        section_Cr_style(tcq);

        REQUIRE(tcq.number_of_classes() == 5);
      }
    }
  }

  // KnuthBendix methods fail for this one
  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "058",
                          "from kbmag/standalone/kb_data/s4",
                          "[todd-coxeter][quick][kbmag]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("abcd");
    presentation::add_rule(p, "bb", "c");
    presentation::add_rule(p, "caca", "abab");
    presentation::add_rule(p, "bc", "d");
    presentation::add_rule(p, "cb", "d");
    presentation::add_rule(p, "aa", "d");
    presentation::add_rule(p, "ad", "a");
    presentation::add_rule(p, "da", "a");
    presentation::add_rule(p, "bd", "b");
    presentation::add_rule(p, "db", "b");
    presentation::add_rule(p, "cd", "c");
    presentation::add_rule(p, "dc", "c");

    ToddCoxeter tc(twosided, p);
    section_hlt(tc);
    section_felsch(tc);
    section_Rc_style(tc);
    section_R_over_C_style(tc);
    section_CR_style(tc);
    section_Cr_style(tc);

    REQUIRE(tc.number_of_classes() == 24);
    REQUIRE(words::human_readable_letter(0) == 'a');
    REQUIRE(words::human_readable_index('a') == 0);
    REQUIRE(reduce(tc, "aaaaaaaaaaaaaaaaaaa") == "a");
    auto S = to<FroidurePin>(tc);
    REQUIRE(to<KnuthBendix<word_type>>(twosided, S).confluent());
  }

  // Second of BHN's series of increasingly complicated presentations
  // of 1. Used to not terminate with lookbehind it does!!
  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "059",
                          "(from kbmag/standalone/kb_data/degen4b) "
                          "(KnuthBendix 065)",
                          "[extreme][todd-coxeter][kbmag][shortlex]") {
    auto rg = ReportGuard(true);

    Presentation<std::string> p;
    p.alphabet("abcdef");
    p.contains_empty_word(true);
    presentation::add_inverse_rules(p, "defabc");
    presentation::add_rule(p, "bbdeaecbffdbaeeccefbccefb", "");
    presentation::add_rule(p, "ccefbfacddecbffaafdcaafdc", "");
    presentation::add_rule(p, "aafdcdbaeefacddbbdeabbdea", "");

    ToddCoxeter tc(twosided, p);

    tc.lookahead_extent(options::lookahead_extent::full)
        .lookahead_style(options::lookahead_style::hlt)
        .large_collapse(1'000'000'000);

    REQUIRE(!is_obviously_infinite(tc));
    tc.run_until([&tc]() -> bool {
      return tc.current_word_graph().number_of_nodes_active() >= 12'000'000;
    });
    todd_coxeter::perform_lookbehind(tc);

    REQUIRE(tc.number_of_classes() == 1);
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "060",
                          "Repeated construction from same FroidurePin",
                          "[todd-coxeter][quick]") {
    auto rg = ReportGuard(false);

    using Transf = LeastTransf<5>;

    FroidurePin S = make<FroidurePin>(
        {make<Transf>({1, 3, 4, 2, 3}), make<Transf>({3, 2, 1, 3, 3})});
    REQUIRE(S.size() == 88);
    REQUIRE(S.number_of_rules() == 18);

    word_type w1
        = froidure_pin::factorisation(S, make<Transf>({3, 4, 4, 4, 4}));
    word_type w2
        = froidure_pin::factorisation(S, make<Transf>({3, 1, 3, 3, 3}));
    word_type w3
        = froidure_pin::factorisation(S, make<Transf>({1, 3, 1, 3, 3}));
    word_type w4
        = froidure_pin::factorisation(S, make<Transf>({4, 2, 4, 4, 2}));

    auto p = to<Presentation<word_type>>(S);
    {
      ToddCoxeter tc1(twosided, p);
      todd_coxeter::add_generating_pair(tc1, w1, w2);

      section_felsch(tc1);
      section_hlt(tc1);
      section_Rc_style(tc1);
      section_R_over_C_style(tc1);
      section_CR_style(tc1);
      section_Cr_style(tc1);

      REQUIRE(tc1.number_of_classes() == 21);
      REQUIRE(tc1.number_of_classes() == to<FroidurePin>(tc1).size());
      REQUIRE(contains(tc1, w3, w4));
      REQUIRE(reduce(tc1, w3) == reduce(tc1, w4));
    }
    {
      ToddCoxeter tc2(twosided, p);
      todd_coxeter::add_generating_pair(tc2, w1, w2);

      section_hlt(tc2);
      section_felsch(tc2);
      section_Rc_style(tc2);
      section_R_over_C_style(tc2);
      section_CR_style(tc2);
      section_Cr_style(tc2);

      REQUIRE(tc2.number_of_classes() == 21);
      REQUIRE(tc2.number_of_classes() == to<FroidurePin>(tc2).size());
      REQUIRE(contains(tc2, w3, w4));
      REQUIRE(reduce(tc2, w3) == reduce(tc2, w4));
    }
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "061",
                          "Sym(5) from Chapter 3, Proposition 1.1 in NR",
                          "[todd-coxeter][quick]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("ABabe");
    presentation::add_identity_rules(p, 'e');
    presentation::add_rule(p, "aa", "e");
    presentation::add_rule(p, "bbbbb", "e");
    presentation::add_rule(p, "babababa", "e");
    presentation::add_rule(p, "bB", "e");
    presentation::add_rule(p, "Bb", "e");
    presentation::add_rule(p, "BabBab", "e");
    presentation::add_rule(p, "aBBabbaBBabb", "e");
    presentation::add_rule(p, "aBBBabbbaBBBabbb", "e");
    presentation::add_rule(p, "aA", "e");
    presentation::add_rule(p, "Aa", "e");

    ToddCoxeter tc(twosided, p);
    SECTION("definition policy == purge_from_top") {
      tc.def_max(2)
          .strategy(options::strategy::felsch)
          .def_version(options::def_version::one)
          .def_policy(options::def_policy::purge_from_top);
    }
    SECTION("definition policy == purge_all") {
      tc.def_max(2)
          .strategy(options::strategy::felsch)
          .def_version(options::def_version::one)
          .def_policy(options::def_policy::purge_all);
    }
    SECTION("definition policy == discard_all_if_no_space") {
      tc.def_max(2)
          .strategy(options::strategy::felsch)
          .def_version(options::def_version::two)
          .def_policy(options::def_policy::discard_all_if_no_space);
    }
    section_hlt(tc);
    section_felsch(tc);
    section_Rc_style(tc);
    section_R_over_C_style(tc);
    section_CR_style(tc);
    section_Cr_style(tc);

    REQUIRE(tc.number_of_classes() == 120);
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "062",
                          "Chapter 7, Theorem 3.6 in NR (size 243)",
                          "[no-valgrind][todd-coxeter][quick]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("ab");
    presentation::add_rule(p, "a^3"_p, "a");
    presentation::add_rule(p, "b^4"_p, "b");
    presentation::add_rule(p, "(ab)^5"_p, "aa");

    ToddCoxeter tc(twosided, p);

    section_hlt(tc);
    section_felsch(tc);
    section_Rc_style(tc);
    section_R_over_C_style(tc);
    section_CR_style(tc);
    section_Cr_style(tc);

    REQUIRE(tc.number_of_classes() == 243);
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "063",
                          "finite semigroup (size 99)",
                          "[todd-coxeter][quick]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("ab");
    presentation::add_rule(p, "aaa", "a");
    presentation::add_rule(p, "b^4"_p, "b");
    presentation::add_rule(p, "(ab)^4"_p, "aa");

    ToddCoxeter tc(twosided, p);
    REQUIRE(!is_obviously_infinite(tc));

    section_hlt(tc);
    section_felsch(tc);
    section_Rc_style(tc);
    section_R_over_C_style(tc);
    section_CR_style(tc);
    section_Cr_style(tc);

    REQUIRE(tc.number_of_classes() == 99);
    REQUIRE(tc.finished());
  }

  // The following 8 examples are from Trevor Walker's Thesis: Semigroup
  // enumeration - computer implementation and applications, p41.
  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "064",
                          "Walker 1",
                          "[todd-coxeter][standard]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
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

    REQUIRE(presentation::length(p) == 117);

    presentation::sort_each_rule(p);
    presentation::sort_rules(p);

    // reduce_complements seems to be non-deterministic, and as such we
    // shouldn't check the result of the reductions below.
    presentation::reduce_complements(p);
    presentation::remove_redundant_generators(p);
    presentation::remove_trivial_rules(p);

    presentation::normalize_alphabet(p);
    REQUIRE(p.alphabet() == "abc");

    presentation::sort_each_rule(p);
    presentation::sort_rules(p);

    ToddCoxeter tc(twosided, p);
    // tc.run_until([&tc]() -> bool {
    //   return tc.current_word_graph().number_of_nodes() >= 10'000;
    // });
    // tc.lookahead_next(100'000);
    REQUIRE(!tc.finished());
    REQUIRE(!is_obviously_infinite(tc));
    tc.standardize(Order::shortlex);
    REQUIRE(!tc.finished());
    tc.standardize(Order::lex);
    REQUIRE(!tc.finished());
    tc.standardize(Order::recursive);
    REQUIRE(!tc.finished());

    section_hlt(tc);
    section_felsch(tc);
    section_CR_style(tc);
    section_Rc_style(tc);
    section_R_over_C_style(tc);
    section_Cr_style(tc);

    REQUIRE(tc.number_of_classes() == 1);
    tc.standardize(Order::shortlex);
    REQUIRE(is_sorted(normal_forms(tc), ShortLexCompare{}));
    tc.standardize(Order::lex);
    REQUIRE(is_sorted(normal_forms(tc), LexicographicalCompare()));
    tc.standardize(Order::recursive);
    REQUIRE(is_sorted(normal_forms(tc), RecursivePathCompare()));
  }

  // The following example is a good one for using the lookahead.
  // This is no longer extreme with the preprocessing
  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "065",
                          "Walker 2",
                          "[todd-coxeter][quick][no-coverage][no-valgrind]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("ab");
    presentation::add_rule(p, "a^32"_p, "a");
    presentation::add_rule(p, "bbb", "b");
    presentation::add_rule(p, "ababa", "b");
    presentation::add_rule(p, "a^16ba^4ba^16ba^4"_p, "b");

    REQUIRE(presentation::length(p) == 87);
    presentation::greedy_reduce_length(p);
    REQUIRE(presentation::length(p) == 31);
    REQUIRE(p.alphabet() == "abcd");
    REQUIRE(p.rules
            == std::vector<std::string>({"dd",
                                         "a",
                                         "bbb",
                                         "b",
                                         "ababa",
                                         "b",
                                         "dbcbdbc",
                                         "b",
                                         "c",
                                         "aaaa",
                                         "d",
                                         "cccc"}));

    ToddCoxeter tc(twosided, p);

    REQUIRE(!is_obviously_infinite(tc));

    SECTION("custom HLT") {
      tc.lookahead_next(1'000'000)
          .def_max(2'000)
          .use_relations_in_extra(true)
          .strategy(options::strategy::hlt)
          .lookahead_extent(options::lookahead_extent::partial)
          .lookahead_style(options::lookahead_style::felsch)
          .def_version(options::def_version::two)
          .def_policy(options::def_policy::no_stack_if_no_space);
    }

    section_hlt(tc);
    section_felsch(tc);
    section_CR_style(tc);
    section_Rc_style(tc);
    section_R_over_C_style(tc);
    section_Cr_style(tc);

    REQUIRE(tc.number_of_classes() == 14'911);
    REQUIRE(tc.finished());

    v4::ToWord to_word("abcd");
    REQUIRE(todd_coxeter::currently_contains_no_checks(tc, "dd", "a")
            == tril::TRUE);
    REQUIRE(todd_coxeter::contains_no_checks(tc, "dd", "a"));
    REQUIRE(contains(tc, "dd", "a"));
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "066",
                          "Walker 3",
                          "[todd-coxeter][standard]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("ab");
    presentation::add_rule(p, "a^16"_p, "a");
    presentation::add_rule(p, "b^16"_p, "b");
    presentation::add_rule(p, "abb", "baa");

    ToddCoxeter tc(twosided, p);

    REQUIRE(!is_obviously_infinite(tc));
    section_hlt(tc);
    section_felsch(tc);
    SECTION("custom Felsch") {
      tc.strategy(options::strategy::felsch)
          .def_version(options::def_version::one)
          .def_policy(options::def_policy::unlimited);
    }
    section_CR_style(tc);
    section_Rc_style(tc);
    section_R_over_C_style(tc);
    section_Cr_style(tc);

    REQUIRE(tc.number_of_classes() == 20'490);
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "067",
                          "Walker 4",
                          "[todd-coxeter][standard]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("ab");
    presentation::add_rule(p, "aaa", "a");
    presentation::add_rule(p, "b^6"_p, "b");
    presentation::add_rule(p, "((ab)^2b^3)^7ab^2a"_p, "bb");

    REQUIRE(presentation::length(p) == 66);

    ToddCoxeter tc(twosided, p);
    tc.lookahead_next(3'000'000);

    REQUIRE(!is_obviously_infinite(tc));

    // section_hlt(tc);

    // Felsch very slow with + without preprocessing
    SECTION("preprocessing + Felsch") {
      presentation::greedy_reduce_length(p);
      REQUIRE(presentation::length(p) == 29);
      REQUIRE(p.alphabet() == "abcd");
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
      tc.init(twosided, p);
      tc.def_max(10'000).large_collapse(3'000);
      tc.strategy(options::strategy::felsch);
    }
    // SECTION("custom R/C") {
    //   tc.lookahead_next(3'000'000)
    //       .strategy(options::strategy::R_over_C)
    //       .def_max(100'000);
    // }
    REQUIRE(tc.number_of_classes() == 36'412);
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "068",
                          "Walker 5",
                          "[todd-coxeter][extreme]") {
    auto                      rg = ReportGuard(true);
    Presentation<std::string> p;
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
    REQUIRE(presentation::length(p) == 34);

    ToddCoxeter tc(twosided, p);
    tc.lookahead_next(5'000'000);
    REQUIRE(!is_obviously_infinite(tc));

    // This example is extremely slow with Felsch (even with the
    // preprocessing)
    section_hlt(tc);
    section_Rc_style(tc);
    section_R_over_C_style(tc);
    section_CR_style(tc);
    // This no longer runs (or maybe never did), but it also doesn't
    // run in "main" so it seems ok to comment it out for now.
    // section_Cr_style(tc);

    REQUIRE(tc.number_of_classes() == 72'822);
    check_complete_compatible(tc);
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "069",
                          "not Walker 6",
                          "[todd-coxeter][extreme]") {
    auto                      rg = ReportGuard();
    Presentation<std::string> p;
    p.alphabet("ab");
    presentation::add_rule(p, "aaa", "a");
    presentation::add_rule(p, "b^9"_p, "b");
    presentation::add_rule(p, "((ab)^2b^3)^7(ab^2)^2b^6"_p, "bb");
    REQUIRE(presentation::length(p) == 77);

    ToddCoxeter tc(twosided, p);
    tc.lookahead_next(5'000'000);
    REQUIRE(!is_obviously_infinite(tc));

    // the random strategy is typically fastest
    section_hlt(tc);
    // Takes about 3.5s with preprocessing
    SECTION("preprocessing + Felsch") {
      presentation::greedy_reduce_length(p);
      REQUIRE(presentation::length(p) == 33);
      REQUIRE(p.alphabet() == "abcd");
      REQUIRE(p.rules
              == std::vector<std::string>({"aaa",
                                           "a",
                                           "ddb",
                                           "b",
                                           "abcccccccbadd",
                                           "bb",
                                           "c",
                                           "adab",
                                           "d",
                                           "bbbb"}));
      presentation::replace_word_with_new_generator(p, "ccc");
      REQUIRE(presentation::length(p) == 33);
      tc.init(twosided, p);
    }
    section_Rc_style(tc);
    section_R_over_C_style(tc);
    section_CR_style(tc);
    // This no longer runs (or maybe never did), but it also doesn't
    // run in "main" so it seems ok to comment it out for now.
    // section_Cr_style(tc);

    REQUIRE(tc.number_of_classes() == 8);
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "070",
                          "Walker 6",
                          "[todd-coxeter][quick][no-valgrind][no-coverage]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
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

    ToddCoxeter tc(twosided, p);
    REQUIRE(!is_obviously_infinite(tc));

    // This example is extremely slow with Felsch, not anymore with the
    // preprocessing above
    section_hlt(tc);
    // section_CR_style(tc); // about 0.9s

    // section_felsch(tc); // takes about 3.5 seconds
    section_Rc_style(tc);
    // section_Cr_style(tc);  // very slow
    section_R_over_C_style(tc);

    REQUIRE(tc.number_of_classes() == 78'722);
  }

  // Felsch is faster here too!
  LIBSEMIGROUPS_TEST_CASE(
      "ToddCoxeter",
      "071",
      "Walker 7",
      "[todd-coxeter][standard][no-valgrind][no-coverage]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
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

    ToddCoxeter tc(twosided, p);
    REQUIRE(!is_obviously_infinite(tc));

    SECTION("def_version == one") {
      tc.def_version(options::def_version::one);
    }
    SECTION("def_version == two") {
      tc.def_version(options::def_version::two);
    }

    section_hlt(tc);
    section_felsch(tc);
    section_CR_style(tc);
    section_Rc_style(tc);
    section_R_over_C_style(tc);
    section_Cr_style(tc);

    REQUIRE(tc.number_of_classes() == 153'500);
  }

  LIBSEMIGROUPS_TEST_CASE(
      "ToddCoxeter",
      "072",
      "Walker 8",
      "[todd-coxeter][standard][no-coverage][no-valgrind]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("ab");
    presentation::add_rule(p, "aaa", "a");
    presentation::add_rule(p, "b^23"_p, "b");
    presentation::add_rule(p, "ab^11ab^2"_p, "bba");

    REQUIRE(presentation::length(p) == 46);
    // presentation::greedy_reduce_length(p);
    // REQUIRE(presentation::length(p) == 24);
    // REQUIRE(p.alphabet() == "abcd");
    // REQUIRE(
    //     p.rules
    //     == std::vector<std::string>(
    //         {"aaa", "a", "ccb", "b", "acad", "da", "c", "dddddb", "d",
    //         "bb"}));

    ToddCoxeter tc(twosided, p);
    tc.lookahead_next(500'000);

    REQUIRE(!is_obviously_infinite(tc));

    section_hlt(tc);
    // section_CR_style(tc); // Too slow
    // section_felsch(tc);
    // // extremely slow without preprocessing (when it is
    // // still slower than HLT)
    // section_Rc_style(tc);  // about 2s
    // section_Cr_style(tc);  // too slow
    section_R_over_C_style(tc);

    check_complete_compatible(tc);

    REQUIRE(tc.number_of_classes() == 270'272);
  }

  // This is a good example of why large collapse is required
  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "073",
                          "KnuthBendix 098",
                          "[todd-coxeter][quick][no-coverage][no-valgrind]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("aAbBcCdDyYfFgGe");
    p.contains_empty_word(false);
    presentation::add_identity_rules(p, 'e');
    presentation::add_inverse_rules(p, "AaBbCcDdYyFfGge", 'e');
    presentation::add_rule(p, "ab", "c");
    presentation::add_rule(p, "bc", "d");
    presentation::add_rule(p, "cd", "y");
    presentation::add_rule(p, "dy", "f");
    presentation::add_rule(p, "yf", "g");
    presentation::add_rule(p, "fg", "a");
    presentation::add_rule(p, "ga", "b");
    ToddCoxeter tc(twosided, p);

    section_hlt(tc);
    section_felsch(tc);
    section_Rc_style(tc);
    section_R_over_C_style(tc);
    section_CR_style(tc);
    section_Cr_style(tc);

    REQUIRE(tc.number_of_classes() == 29);
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "074",
                          "Holt 2 - SL(2, p)",
                          "[todd-coxeter][extreme]") {
    auto                        rg     = ReportGuard(true);
    std::array<size_t, 4> const sizes  = {24, 120, 336, 1'320};
    std::array<size_t, 4> const primes = {3, 5, 7, 11};

    for (auto [i, p] : enumerate(primes)) {
      SECTION("p = " + std::to_string(p)) {
        ToddCoxeter tc(twosided,
                       presentation::examples::special_linear_group_2(p));

        section_hlt(tc);
        section_felsch(tc);

        REQUIRE(tc.number_of_classes() == sizes[i]);
      }
    }
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "075",
                          "Holt 3",
                          "[todd-coxeter][standard]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("aAbBcC");
    p.contains_empty_word(true);
    presentation::add_inverse_rules(p, "AaBbCc");

    presentation::add_rule(p, "bbCbc", "");
    presentation::add_rule(p, "aaBab", "");
    presentation::add_rule(p, "cABcabc", "");

    ToddCoxeter tc(twosided, p);
    REQUIRE(is_non_trivial(tc) != tril::FALSE);

    section_hlt(tc);
    section_felsch(tc);
    section_Rc_style(tc);
    section_R_over_C_style(tc);
    section_CR_style(tc);
    section_Cr_style(tc);

    REQUIRE(tc.number_of_classes() == 6'561);
    check_normal_forms(tc, 0);
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "076",
                          "Holt 3 x 2",
                          "[todd-coxeter][fail]") {
    auto                      rg = ReportGuard();
    Presentation<std::string> p;
    p.alphabet("aAbBcC");
    p.contains_empty_word(true);
    presentation::add_inverse_rules(p, "AaBbCc");

    presentation::add_rule(p, "aaCac", "");
    presentation::add_rule(p, "acbbACb", "");
    presentation::add_rule(p, "ABabccc", "");
    presentation::remove_duplicate_rules(p);
    presentation::sort_rules(p);
    REQUIRE(p.rules.size() == 18);

    ToddCoxeter tc(twosided, p);
    REQUIRE(!is_obviously_infinite(tc));

    tc.lookahead_extent(options::lookahead_extent::partial)
        .lookahead_style(options::lookahead_style::hlt)
        .lookahead_growth_factor(1.01)
        .lookahead_growth_threshold(10);
    REQUIRE(is_non_trivial(tc) == tril::TRUE);
    REQUIRE(tc.number_of_classes() == 6'561);
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "077",
                          "Campbell-Reza 1",
                          "[todd-coxeter][quick]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("ab");
    presentation::add_rule(p, "aa", "bb");
    presentation::add_rule(p, "ba", "aaaaaab");

    ToddCoxeter tc(twosided, p);
    section_hlt(tc);
    section_felsch(tc);
    section_Rc_style(tc);
    section_R_over_C_style(tc);
    section_CR_style(tc);
    section_Cr_style(tc);

    REQUIRE(tc.number_of_classes() == 14);
    tc.standardize(Order::shortlex);
    REQUIRE((normal_forms(tc) | to_vector())
            == std::vector<std::string>({"a",
                                         "b",
                                         "aa",
                                         "ab",
                                         "ba",
                                         "aaa",
                                         "aab",
                                         "aaaa",
                                         "aaab",
                                         "aaaaa",
                                         "aaaab",
                                         "aaaaaa",
                                         "aaaaab",
                                         "aaaaaaa"}));
    REQUIRE(to<FroidurePin>(tc).number_of_rules() == 6);

    using namespace std::string_literals;
    REQUIRE(reduce(tc, "aaaaaaab") == "aab");
    REQUIRE(reduce(tc, "bab") == "aaa");
  }

  // The next example demonstrates why we require deferred standardization
  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "078",
                          "Renner monoid type D4 (Gay-Hivert), q = 1",
                          "[quick][todd-coxeter][no-coverage][no-valgrind]") {
    auto rg = ReportGuard(false);
    auto p  = presentation::examples::renner_type_D_monoid(4, 1);
    presentation::normalize_alphabet(p);
    ToddCoxeter tc(twosided, p);
    tc.strategy(options::strategy::hlt)
        .lookahead_extent(options::lookahead_extent::partial);

    REQUIRE(tc.presentation().alphabet() == "0123456789"_w);
    REQUIRE(!is_obviously_infinite(tc));

    section_felsch(tc);
    section_hlt(tc);
    section_CR_style(tc);
    section_R_over_C_style(tc);
    section_Rc_style(tc);
    section_Cr_style(tc);

    REQUIRE(tc.number_of_classes() == 10'625);

    tc.standardize(Order::shortlex);
    REQUIRE((normal_forms(tc) | skip_n(10) | take(10) | to_vector())
            == std::vector(
                {9_w, 01_w, 02_w, 03_w, 04_w, 05_w, 06_w, 12_w, 13_w, 14_w}));
    REQUIRE(is_sorted(normal_forms(tc), ShortLexCompare()));
    tc.standardize(Order::lex);
    REQUIRE(is_sorted(normal_forms(tc), LexicographicalCompare()));
  }

  // Felsch very slow here
  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "079",
                          "trivial semigroup",
                          "[no-valgrind][todd-coxeter][quick][no-coverage]") {
    auto rg = ReportGuard(false);

    for (size_t N = 2; N < 1000; N += 199) {
      Presentation<std::string> p;
      p.alphabet("eab");
      presentation::add_identity_rules(p, 'e');

      std::string lhs = "a" + std::string(N, 'b');
      std::string rhs = "e";
      presentation::add_rule_no_checks(p, lhs, rhs);

      lhs = std::string(N, 'a') + std::string(N + 1, 'b');
      rhs = "e";
      presentation::add_rule_no_checks(p, lhs, rhs);

      lhs = "ab";
      rhs = std::string(N, 'b') + "a";
      presentation::add_rule_no_checks(p, lhs, rhs);
      ToddCoxeter tc(twosided, std::move(p));
      REQUIRE(tc.number_of_classes() == 1);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "080",
                          "ACE --- 2p17-2p14 - HLT",
                          "[todd-coxeter][standard][ace]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("abcABC");
    p.contains_empty_word(true);
    presentation::add_inverse_rules(p, "ABCabc");
    presentation::add_rule(p, "aBCbac", "");
    presentation::add_rule(p, "bACbaacA", "");
    presentation::add_rule(p, "accAABab", "");

    ToddCoxeter H(onesided, p);
    REQUIRE(H.presentation().alphabet()
            == std::string({97, 98, 99, 65, 66, 67}));

    todd_coxeter::add_generating_pair(H, "bc", "");
    H.lookahead_next(1'000'000);

    REQUIRE(H.number_of_classes() == 16'384);

    REQUIRE(word_graph::is_reachable(H.word_graph(), 0, 0));
    REQUIRE(word_graph::ancestors_of_no_checks(H.word_graph(), 0).size()
            == 16'384);
    REQUIRE(!word_graph::is_acyclic(H.word_graph(), 0, 0));

    // The following no longer works
    // REQUIRE(class_of(H, "").size_hint() == POSITIVE_INFINITY);
    REQUIRE((class_of(H, "") | rx::take(50) | rx::to_vector())
            == std::vector<std::string>(
                {"aA",   "bc",   "bB",   "cC",   "Aa",   "Bb",   "Cc",   "CB",
                 "aaAA", "abBA", "acCA", "aAaA", "aAbc", "aAbB", "aAcC", "aAAa",
                 "aABb", "aACc", "aACB", "aBbA", "aCcA", "baAc", "baAB", "bbBc",
                 "bbBB", "bcaA", "bcbc", "bcbB", "bccC", "bcAa", "bcBb", "bcCc",
                 "bcCB", "bAac", "bAaB", "bBaA", "bBbc", "bBbB", "bBcC", "bBAa",
                 "bBBb", "bBCc", "bBCB", "bCcc", "bCcB", "caAC", "cbBC", "ccCC",
                 "cAaC", "cBbC"}));
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "081",
                          "ACE --- 2p17-2p3 - HLT",
                          "[todd-coxeter][standard][ace]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("abcABC");
    p.contains_empty_word(true);
    presentation::add_inverse_rules(p, "ABCabc");
    presentation::add_rule(p, "aBCbac", "");
    presentation::add_rule(p, "bACbaacA", "");
    presentation::add_rule(p, "accAABab", "");

    ToddCoxeter H(onesided, p);
    todd_coxeter::add_generating_pair(H, "bc", "");
    todd_coxeter::add_generating_pair(H, "bc", "ABAAbcabC");

    H.strategy(options::strategy::hlt)
        .lookahead_extent(options::lookahead_extent::partial);
    SECTION("HLT + save") {
      H.save(true);
    }
    SECTION("HLT + no save") {
      H.save(false);
    }

    REQUIRE(H.number_of_classes() == 8);
  }

  // In this example large_collapse makes a huge difference to the run time,
  // 73ms versus 1173ms for the save case.
  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "082",
                          "ACE --- 2p17-1a - HLT",
                          "[todd-coxeter][standard][ace]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("abcABC");
    p.contains_empty_word(true);

    presentation::add_inverse_rules(p, "ABCabc");
    presentation::add_rule(p, "aBCbac", "");
    presentation::add_rule(p, "bACbaacA", "");
    presentation::add_rule(p, "accAABab", "");

    ToddCoxeter H(onesided, p);
    todd_coxeter::add_generating_pair(H, "bc", "");
    todd_coxeter::add_generating_pair(H, "ABAAbcabC", "");
    todd_coxeter::add_generating_pair(H, "AcccacBcA", "");
    H.large_collapse(10'000)
        .strategy(options::strategy::hlt)
        .lookahead_extent(options::lookahead_extent::partial);
    SECTION("HLT + save") {
      H.save(true);
    }
    SECTION("HLT + no save") {
      H.save(false);
    }
    REQUIRE(H.number_of_classes() == 1);
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "083",
                          "ACE --- F27",
                          "[todd-coxeter][standard][ace]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("abcdxyzABCDXYZ");
    p.contains_empty_word(true);

    presentation::add_inverse_rules(p, "ABCDXYZabcdxyz");
    presentation::add_rule(p, "abC", "");
    presentation::add_rule(p, "bcD", "");
    presentation::add_rule(p, "cdX", "");
    presentation::add_rule(p, "dxY", "");
    presentation::add_rule(p, "xyZ", "");
    presentation::add_rule(p, "yzA", "");
    presentation::add_rule(p, "zaB", "");

    ToddCoxeter H(twosided, p);
    section_felsch(H);
    section_hlt(H);
    section_CR_style(H);
    section_Cr_style(H);
    section_R_over_C_style(H);
    section_Rc_style(H);

    REQUIRE(H.number_of_classes() == 29);
  }

  LIBSEMIGROUPS_TEST_CASE(
      "ToddCoxeter",
      "084",
      "ACE --- SL219 - HLT",
      "[todd-coxeter][standard][ace][no-valgrind][no-coverage]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("abAB");
    p.contains_empty_word(true);
    presentation::add_inverse_rules(p, "ABab");
    presentation::add_rule(p, "aBABAB", "");
    presentation::add_rule(p, "BAAbaa", "");
    presentation::add_rule(p, "ab^4ab^10ab^4ab^29a^12"_p, "");

    ToddCoxeter H(onesided, p);

    SECTION("HLT + preprocessing + save") {
      REQUIRE(p.alphabet() == "abAB");
      presentation::greedy_reduce_length(p);
      REQUIRE(p.alphabet() == "abABcde");
      REQUIRE(presentation::length(p) == 49);
      REQUIRE(H.internal_presentation().alphabet() == 0123_w);
      REQUIRE(H.presentation().alphabet() == "abAB");

      H.init(onesided, p);
      REQUIRE(H.internal_presentation().alphabet() == 0123456_w);
      REQUIRE(H.presentation().alphabet() == "abABcde");
      H.strategy(options::strategy::hlt)
          .lookahead_extent(options::lookahead_extent::partial)
          .save(true);
    }
    SECTION("HLT + no preprocessing + no save") {
      REQUIRE(presentation::length(p) == 83);
      H.strategy(options::strategy::hlt)
          .lookahead_extent(options::lookahead_extent::partial)
          .save(false);
      REQUIRE(H.internal_presentation().alphabet() == 0123_w);
      REQUIRE(H.presentation().alphabet() == "abAB");
    }
    // section_CR_style(H); // too slow
    section_R_over_C_style(H);
    // section_Cr_style(H); // too slow
    // section_Rc_style(H); // about 1.7s

    todd_coxeter::add_generating_pair(H, "b", "");

    REQUIRE(H.number_of_classes() == 180);
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "085",
                          "ACE --- perf602p5",
                          "[no-valgrind][todd-coxeter][quick][ace]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("abstuvdABSTUVD");
    p.contains_empty_word(true);
    presentation::add_inverse_rules(p, "ABSTUVDabstuvd");
    presentation::add_rule(p, "aaD", "");
    presentation::add_rule(p, "bbb", "");
    presentation::add_rule(p, "(ab)^5"_p, "");
    presentation::add_rule(p, "ss", "");
    presentation::add_rule(p, "tt", "");
    presentation::add_rule(p, "uu", "");
    presentation::add_rule(p, "vv", "");
    presentation::add_rule(p, "dd", "");
    presentation::add_rule(p, "STst", "");
    presentation::add_rule(p, "UVuv", "");
    presentation::add_rule(p, "SUsu", "");
    presentation::add_rule(p, "SVsv", "");
    presentation::add_rule(p, "TUtu", "");
    presentation::add_rule(p, "TVtv", "");
    presentation::add_rule(p, "AsaU", "");
    presentation::add_rule(p, "AtaV", "");
    presentation::add_rule(p, "AuaS", "");
    presentation::add_rule(p, "AvaT", "");
    presentation::add_rule(p, "BsbDVT", "");
    presentation::add_rule(p, "BtbVUTS", "");
    presentation::add_rule(p, "BubVU", "");
    presentation::add_rule(p, "BvbU", "");
    presentation::add_rule(p, "DAda", "");
    presentation::add_rule(p, "DBdb", "");
    presentation::add_rule(p, "DSds", "");
    presentation::add_rule(p, "DTdt", "");
    presentation::add_rule(p, "DUdu", "");
    presentation::add_rule(p, "DVdv", "");

    ToddCoxeter H(onesided, p);
    todd_coxeter::add_generating_pair(H, "a", "");

    section_hlt(H);
    section_CR_style(H);
    section_Cr_style(H);
    section_R_over_C_style(H);
    section_felsch(H);
    section_Rc_style(H);

    REQUIRE(H.number_of_classes() == 480);
  }

  LIBSEMIGROUPS_TEST_CASE(
      "ToddCoxeter",
      "086",
      "ACE --- M12",
      "[todd-coxeter][standard][ace][no-coverage][no-valgrind]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("abcABC");
    p.contains_empty_word(true);
    presentation::add_inverse_rules(p, "ABCabc");
    presentation::add_rule(p, "a^11"_p, "");
    presentation::add_rule(p, "bb", "");
    presentation::add_rule(p, "cc", "");
    presentation::add_rule(p, "(ab)^3"_p, "");
    presentation::add_rule(p, "(ac)^3"_p, "");
    presentation::add_rule(p, "(bc)^10"_p, "");
    presentation::add_rule(p, "(cb)^2a(bc)^2A^5"_p, "");

    ToddCoxeter H(twosided, p);

    SECTION("HLT + save + partial lookahead") {
      H.strategy(options::strategy::hlt)
          .save(true)
          .lookahead_extent(options::lookahead_extent::partial);
    }
    section_hlt(H);
    section_felsch(H);
    section_CR_style(H);
    section_Cr_style(H);
    section_R_over_C_style(H);
    section_Rc_style(H);

    REQUIRE(H.number_of_classes() == 95'040);
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "087",
                          "ACE --- C5",
                          "[todd-coxeter][quick][ace]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("abAB");
    p.contains_empty_word(true);

    presentation::add_inverse_rules(p, "ABab");
    presentation::add_rule(p, "aaaaa", "");
    presentation::add_rule(p, "b", "");

    ToddCoxeter H(twosided, p);

    section_hlt(H);
    section_felsch(H);
    section_CR_style(H);
    section_Cr_style(H);
    section_R_over_C_style(H);
    section_Rc_style(H);

    REQUIRE(H.number_of_classes() == 5);
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "088",
                          "ACE --- A5-C5",
                          "[todd-coxeter][quick][ace]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("abAB");
    p.contains_empty_word(true);

    presentation::add_inverse_rules(p, "ABab");
    presentation::add_rule(p, "aa", "");
    presentation::add_rule(p, "bbb", "");
    presentation::add_rule(p, "(ab)^5"_p, "");

    ToddCoxeter H(onesided, p);

    todd_coxeter::add_generating_pair(H, "ab", "");

    section_hlt(H);
    section_felsch(H);
    section_CR_style(H);
    section_Cr_style(H);
    section_R_over_C_style(H);
    section_Rc_style(H);

    REQUIRE(H.number_of_classes() == 12);
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "089",
                          "ACE --- A5",
                          "[todd-coxeter][quick][ace]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("abAB");
    p.contains_empty_word(true);

    presentation::add_inverse_rules(p, "ABab");
    presentation::add_rule(p, "aa", "");
    presentation::add_rule(p, "bbb", "");
    presentation::add_rule(p, "(ab)^5"_p, "");

    ToddCoxeter H(twosided, p);

    section_hlt(H);
    section_felsch(H);
    section_CR_style(H);
    section_Cr_style(H);
    section_R_over_C_style(H);
    section_Rc_style(H);

    REQUIRE(H.number_of_classes() == 60);
  }

  // Felsch is much much better here, slightly slower in v3, nothing obvious
  // stands out as being the cause of this, in v2 this takes about 1.4s in v3
  // 1.5s or so.
  // Seems to have gotten worse still since the comment about up to 1.8s
  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "090",
                          "relation ordering",
                          "[todd-coxeter][standard]") {
    auto rg = ReportGuard(false);
    // Sorting the rules makes this twice as slow...
    auto p = presentation::examples::renner_type_D_monoid(5, 1);
    presentation::sort_each_rule(p);
    presentation::sort_rules(p);
    REQUIRE(p.rules.size() == 302);
    presentation::remove_duplicate_rules(p);
    presentation::reduce_complements(p);
    REQUIRE(p.rules.size() == 230);

    ToddCoxeter tc(twosided, p);
    REQUIRE(!is_obviously_infinite(tc));
    tc.strategy(options::strategy::felsch);
    REQUIRE(tc.number_of_classes() == 258'661);
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "091",
                          "relation ordering x 2",
                          "[todd-coxeter][quick]") {
    Presentation<word_type> p;
    p.alphabet(10);
    presentation::add_rule(p, 01_w, 0_w);
    presentation::add_rule(p, 02_w, 0_w);
    presentation::add_rule(p, 03_w, 0_w);
    presentation::add_rule(p, 04_w, 0_w);
    presentation::add_rule(p, 05_w, 0_w);
    presentation::add_rule(p, 06_w, 0_w);
    presentation::add_rule(p, 07_w, 0_w);
    presentation::add_rule(p, "08"_w, 0_w);
    presentation::add_rule(p, "09"_w, 0_w);
    presentation::add_rule(p, 10_w, 1_w);
    presentation::add_rule(p, 11_w, 1_w);
    presentation::add_rule(p, 12_w, 1_w);
    presentation::add_rule(p, 13_w, 1_w);
    presentation::add_rule(p, 14_w, 1_w);
    presentation::add_rule(p, 15_w, 1_w);
    presentation::add_rule(p, 16_w, 1_w);
    presentation::add_rule(p, 17_w, 1_w);
    presentation::add_rule(p, 18_w, 1_w);
    presentation::add_rule(p, 19_w, 1_w);
    presentation::add_rule(p, 20_w, 2_w);
    presentation::add_rule(p, 21_w, 2_w);
    presentation::add_rule(p, 22_w, 2_w);
    presentation::add_rule(p, 23_w, 2_w);
    presentation::add_rule(p, 24_w, 2_w);
    presentation::add_rule(p, 25_w, 2_w);
    presentation::add_rule(p, 26_w, 2_w);
    presentation::add_rule(p, 27_w, 2_w);
    presentation::add_rule(p, 28_w, 2_w);
    presentation::add_rule(p, 29_w, 2_w);
    presentation::add_rule(p, 30_w, 3_w);
    presentation::add_rule(p, 31_w, 3_w);
    presentation::add_rule(p, 32_w, 3_w);
    presentation::add_rule(p, 33_w, 3_w);
    presentation::add_rule(p, 34_w, 3_w);
    presentation::add_rule(p, 35_w, 3_w);
    presentation::add_rule(p, 36_w, 3_w);
    presentation::add_rule(p, 37_w, 3_w);
    presentation::add_rule(p, 38_w, 3_w);
    presentation::add_rule(p, 39_w, 3_w);
    presentation::add_rule(p, 40_w, 4_w);
    presentation::add_rule(p, 41_w, 4_w);
    presentation::add_rule(p, 42_w, 4_w);
    presentation::add_rule(p, 43_w, 4_w);
    presentation::add_rule(p, 44_w, 4_w);
    presentation::add_rule(p, 45_w, 4_w);
    presentation::add_rule(p, 46_w, 4_w);
    presentation::add_rule(p, 47_w, 4_w);
    presentation::add_rule(p, 48_w, 4_w);
    presentation::add_rule(p, 49_w, 4_w);
    presentation::add_rule(p, 50_w, 5_w);
    presentation::add_rule(p, 51_w, 5_w);
    presentation::add_rule(p, 52_w, 5_w);
    presentation::add_rule(p, 53_w, 5_w);
    presentation::add_rule(p, 54_w, 5_w);
    presentation::add_rule(p, 55_w, 5_w);
    presentation::add_rule(p, 56_w, 5_w);
    presentation::add_rule(p, 57_w, 5_w);
    presentation::add_rule(p, 58_w, 5_w);
    presentation::add_rule(p, 59_w, 5_w);
    presentation::add_rule(p, 60_w, 6_w);
    presentation::add_rule(p, 61_w, 6_w);
    presentation::add_rule(p, 62_w, 6_w);
    presentation::add_rule(p, 63_w, 6_w);
    presentation::add_rule(p, 64_w, 6_w);
    presentation::add_rule(p, 65_w, 6_w);
    presentation::add_rule(p, 66_w, 6_w);
    presentation::add_rule(p, 67_w, 6_w);
    presentation::add_rule(p, 68_w, 6_w);
    presentation::add_rule(p, 69_w, 6_w);
    presentation::add_rule(p, 70_w, 7_w);
    presentation::add_rule(p, 71_w, 7_w);
    presentation::add_rule(p, 7_w, 72_w);
    presentation::add_rule(p, 73_w, 7_w);
    presentation::add_rule(p, 74_w, 7_w);
    presentation::add_rule(p, 75_w, 7_w);
    presentation::add_rule(p, 76_w, 7_w);
    presentation::add_rule(p, 77_w, 7_w);
    presentation::add_rule(p, 78_w, 7_w);
    presentation::add_rule(p, 79_w, 7_w);
    presentation::add_rule(p, 80_w, 8_w);
    presentation::add_rule(p, 81_w, 8_w);
    presentation::add_rule(p, 82_w, 8_w);
    presentation::add_rule(p, 83_w, 8_w);
    presentation::add_rule(p, 84_w, 8_w);
    presentation::add_rule(p, 85_w, 8_w);
    presentation::add_rule(p, 86_w, 8_w);
    presentation::add_rule(p, 87_w, 8_w);
    presentation::add_rule(p, 88_w, 8_w);
    presentation::add_rule(p, 89_w, 8_w);
    presentation::add_rule(p, 90_w, 9_w);
    presentation::add_rule(p, 901234551569888880_w, 9_w);

    ToddCoxeter tc(twosided, p);
    section_felsch(tc);
    section_hlt(tc);
    section_CR_style(tc);
    section_Cr_style(tc);
    section_R_over_C_style(tc);
    section_Rc_style(tc);

    REQUIRE(tc.number_of_classes() == 10);
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "092",
                          "short circuit size in obviously infinite",
                          "[todd-coxeter][quick]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("abc");
    presentation::add_rule(p, "a^4"_p, "a");
    ToddCoxeter tc(twosided, p);
    REQUIRE(tc.number_of_classes() == POSITIVE_INFINITY);
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "093",
                          "http://brauer.maths.qmul.ac.uk/Atlas/misc/24A8",
                          "[todd-coxeter][standard]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("xyXY");
    p.contains_empty_word(true);

    presentation::add_inverse_rules(p, "XYxy");
    presentation::add_rule(p, "xx", "X");
    presentation::add_rule(p, "y^6"_p, "Y");
    presentation::add_rule(p, "YXyx", "XYxy");
    presentation::add_rule(p, "(xY^3)^2xY^2"_p, "(y^2Xy)^2y^2X"_p);
    presentation::add_rule(p, "(xy)^2yXyxY^2xy^3x"_p, "yyyXyyy");
    presentation::sort_rules(p);

    ToddCoxeter tc(twosided, p);

    SECTION("custom HLT") {
      tc.lookahead_next(2'000'000)
          .lookahead_extent(options::lookahead_extent::partial)
          .strategy(options::strategy::hlt);
    }
    // section_felsch(tc);  // about 3s with Felsch
    // section_CR_style(tc); // about 2.7s
    section_R_over_C_style(tc);
    // section_Cr_style(tc);  // about 3.1s
    section_Rc_style(tc);
    REQUIRE(tc.number_of_classes() == 322'560);
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "094",
                          "http://brauer.maths.qmul.ac.uk/Atlas/spor/M11/",
                          "[todd-coxeter][quick][no-coverage][no-valgrind]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("xyXY");
    p.contains_empty_word(true);

    presentation::add_inverse_rules(p, "XYxy");
    presentation::add_rule(p, "xx", "");
    presentation::add_rule(p, "y^4"_p, "");
    presentation::add_rule(p, "(xy)^11"_p, "");
    presentation::add_rule(p, "(xy^2)^6"_p, "");
    presentation::add_rule(p, "(xy)^2xY(xy)^2(yxYx)^2Y"_p, "");

    ToddCoxeter tc(twosided, p);

    section_felsch(tc);
    section_hlt(tc);
    section_CR_style(tc);
    section_Cr_style(tc);
    section_R_over_C_style(tc);
    section_Rc_style(tc);

    REQUIRE(tc.number_of_classes() == 7'920);

    REQUIRE(contains(tc, "xx", ""));
    REQUIRE(!contains(tc, "yy", ""));
    REQUIRE(!contains(tc, "yyy", ""));
    REQUIRE(contains(tc, "yyyy", ""));

    REQUIRE(word_of(tc, 0) == "");

    check_contains(tc);
    check_word_to_index_of(tc);

    REQUIRE(reduce(tc, "") == "");
    REQUIRE(normal_forms(tc).size_hint() == tc.number_of_classes());
    REQUIRE(normal_forms(tc).get() == "");
  }

  LIBSEMIGROUPS_TEST_CASE(
      "ToddCoxeter",
      "095",
      "http://brauer.maths.qmul.ac.uk/Atlas/spor/M12/",
      "[todd-coxeter][standard][no-valgrind][no-coverage]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("xyXY");
    p.contains_empty_word(true);

    presentation::add_inverse_rules(p, "XYxy");
    presentation::add_rule(p, "xx", "");
    presentation::add_rule(p, "yyy", "");
    presentation::add_rule(p, "(xy)^11"_p, "");
    presentation::add_rule(p, "(XYxy)^6"_p, "");
    presentation::add_rule(p, "((xy)^2xY)^6"_p, "");
    presentation::add_rule(p, "((XY)^2(xy)^2)^5"_p, "");

    ToddCoxeter tc(twosided, p);
    section_felsch(tc);
    section_hlt(tc);
    section_CR_style(tc);
    section_Cr_style(tc);
    section_R_over_C_style(tc);
    section_Rc_style(tc);
    REQUIRE(tc.number_of_classes() == 95'040);
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "096",
                          "http://brauer.maths.qmul.ac.uk/Atlas/spor/M22",
                          "[todd-coxeter][extreme]") {
    ReportGuard               rg(true);
    Presentation<std::string> p;
    p.alphabet("xyXY");
    p.contains_empty_word(true);

    presentation::add_inverse_rules(p, "XYxy");
    presentation::add_rule(p, "xx", "");
    presentation::add_rule(p, "y^4"_p, "");
    presentation::add_rule(p, "(xy)^11"_p, "");
    presentation::add_rule(p, "(xy^2)^5"_p, "");
    presentation::add_rule(p, "(XYxy)^6"_p, "");
    presentation::add_rule(p, "((XY)^2(xy)^2)^3"_p, "");
    presentation::add_rule(p, "((xy)^2xY)^5"_p, "");

    ToddCoxeter tc(twosided, p);
    section_hlt(tc);
    section_felsch(tc);  // 2s with Felsch
    REQUIRE(tc.number_of_classes() == 443'520);
  }

  // Takes about 4 minutes (2021 - MacBook Air M1 - 8GB RAM)
  // with Felsch (3.5mins or 2.5mins with lowerbound) or HLT (4.5mins)
  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "097",
                          "http://brauer.maths.qmul.ac.uk/Atlas/spor/M23",
                          "[todd-coxeter][extreme]") {
    auto                      rg = ReportGuard(true);
    Presentation<std::string> p;
    p.alphabet("xyXY");
    p.contains_empty_word(true);

    presentation::add_inverse_rules(p, "XYxy");
    presentation::add_rule(p, "xx", "");
    presentation::add_rule(p, "y^4"_p, "");
    presentation::add_rule(p, "(xy)^23"_p, "");
    presentation::add_rule(p, "(xy^2)^6"_p, "");
    presentation::add_rule(p, "(XYxy)^6"_p, "");
    presentation::add_rule(p, "(xyxYxy^2)^4"_p, "");
    presentation::add_rule(p, "xyx((yx)^2Yxy)^2xY(xy)^3(xY)^3"_p, "");
    presentation::add_rule(p, "(x(yxy)^2y)^6"_p, "");
    presentation::add_rule(p, "((xy)^2y)^3(xy^2xY)^2x(yxy)^2xYxy^2"_p, "");

    REQUIRE(presentation::length(p) == 246);

    presentation::sort_each_rule(p);
    presentation::sort_rules(p);
    REQUIRE(presentation::longest_subword_reducing_length(p) == "xy");
    presentation::replace_word_with_new_generator(p, "xy");

    ToddCoxeter tc(twosided, p);

    tc.strategy(options::strategy::felsch)
        .def_version(options::def_version::two)
        .def_policy(options::def_policy::discard_all_if_no_space)
        .use_relations_in_extra(true)
        .def_max(10'000)
        .lower_bound(10'200'960);
    // TODO(1) uncomment
    //     .reserve(50'000'000);

    tc.run_for(std::chrono::seconds(3));

    std::vector<std::string> words
        = {"xxyyyYYayyaaaYxYaaXxxaaayXYyYXxXyXXyyyYYxxyyyYYayyaaaYxYaaXxxaaayXY"
           "yYXxXyXXyyyYYxxyyyYYayyaaaYxYaaXxxaaayXYyYXxXyXXyyyYYxxyyyYYayyaaaY"
           "xYaaXxxaaayXYyYXxXyXXyyyYxxyyyYYayyaaaYxYaaXxxaaayXYyYXxXyXXyyyYxxy"
           "yyYYayyaaaYxYaaXxxaaayXYyYXxXyXXyyyYxxyyyYYayyaaaYxYaaXxxaaayXYyYXx"
           "XyXXyyyYxxyyyYYayyaaaYxYaaXxxaaayXYyYXxXyXXyyyYxxyyyYYayyaaaYxYaaXx"
           "xaaayXYyYXxXyXXyyyYYYYYYY",
           "YXxxXaxaYyYXxxYYyYYYxaYaXaayaXaXxxyaXYaY",
           "aYXYaXXxxxyXyyyXXaXYyyYaXaYxxyaayXyYXyaa",
           "YyYXYxaaaYaYaYyxxaYaxxyyYaxyXyxxxXxXaaay",
           "YxxxXYXXyayyYYaayYXYXaXXXXYxyYXXXXXXXYyY"};
    std::vector<std::string> expected(words);
    std::for_each(expected.begin(), expected.end(), [&](std::string& word) {
      word = todd_coxeter::reduce_no_run(tc, word);
    });
    // Standardising (triggered by reduce_no_run) after 3 seconds makes this run
    // much faster

    for (size_t i = 0; i != expected.size(); ++i) {
      REQUIRE(todd_coxeter::currently_contains(tc, words[i], expected[i])
              == tril::TRUE);
    }
    tc.run_until([&tc]() {
      return tc.current_word_graph().number_of_nodes_active()
             > 1.5 * 10'200'960;
    });
    // todd_coxeter::perform_lookbehind(tc);
    tc.large_collapse(POSITIVE_INFINITY);

    REQUIRE(tc.number_of_classes() == 10'200'960);
    for (size_t i = 0; i != expected.size(); ++i) {
      REQUIRE(todd_coxeter::contains(tc, words[i], expected[i]));
    }
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "098",
                          "http://brauer.maths.qmul.ac.uk/Atlas/clas/S62",
                          "[todd-coxeter][extreme]") {
    auto                      rg = ReportGuard(true);
    Presentation<std::string> p;
    p.alphabet("xyXY");
    p.contains_empty_word(true);
    presentation::add_inverse_rules(p, "XYxy");
    presentation::add_rule(p, "xx", "");
    presentation::add_rule(p, "y^3"_p, "");
    presentation::add_rule(p, "(xy)^23"_p, "");
    presentation::add_rule(p, "(XYxy)^12"_p, "");
    presentation::add_rule(p, "((XY)^2(xy)^2)^5"_p, "");
    presentation::add_rule(p, "((xy)^2xY)^3(xy(xY)^2)^3"_p, "");
    presentation::add_rule(p, "(xy(xyxY)^3)^4"_p, "");

    REQUIRE(presentation::length(p) == 239);

    SECTION("custom HLT") {
      ToddCoxeter tc(onesided, p);
      todd_coxeter::add_generating_pair(tc, "xy", "");
      tc.strategy(options::strategy::hlt)
          .lookahead_extent(options::lookahead_extent::partial)
          .lookahead_style(options::lookahead_style::hlt)
          .lookahead_stop_early_ratio(0.0)
          .lookahead_next(16'000'000)
          .lookahead_min(16'000'000)
          .save(false);
      REQUIRE(tc.number_of_classes() == 10'644'480);
    }

    SECTION("preprocess + Felsch") {
      REQUIRE(presentation::longest_subword_reducing_length(p) == "xy");
      presentation::replace_word_with_new_generator(p, "xy");
      REQUIRE(presentation::longest_subword_reducing_length(p) == "axY");
      presentation::replace_word_with_new_generator(p, "axY");
      REQUIRE(presentation::length(p) == 140);
      ToddCoxeter tc(onesided, p);
      todd_coxeter::add_generating_pair(tc, "xy", "");
      tc.strategy(options::strategy::felsch);  // .lower_bound(10'644'480);
      REQUIRE(tc.number_of_classes() == 10'644'480);
    }
  }

  // Approx. 32 minutes (2021 - MacBook Air M1 - 8GB RAM)
  // Approx. 17.5 minutes (2025 - intel i7-13700H - 64GB RAM)
  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "099",
                          "http://brauer.maths.qmul.ac.uk/Atlas/spor/HS",
                          "[todd-coxeter][extreme]") {
    auto rg = ReportGuard(true);

    Presentation<std::string> p;
    p.alphabet("xyXY");
    p.contains_empty_word(true);

    presentation::add_inverse_rules(p, "XYxy");
    presentation::add_rule(p, "xx", "");
    presentation::add_rule(p, "y^5"_p, "");
    presentation::add_rule(p, "(xy)^11"_p, "");
    presentation::add_rule(p, "(xyy)^10"_p, "");
    presentation::add_rule(p, "(XYxy)^5"_p, "");
    presentation::add_rule(p, "((XY)^2(xy)^2)^3"_p, "");
    presentation::add_rule(p, "(XY^2xy^2)^6"_p, "");
    presentation::add_rule(p, "(xy)^3yx(YxY)^2xy(yx)^2y(xY^2)^4"_p, "");
    presentation::add_rule(p, "xy(xy^2(xY^2)^2)^2xy^2xy(xy^2xY)^2xy^2"_p, "");
    presentation::add_rule(p, "xyx(yxy)^3(xY)^2x(yxy)^3xyxY(Yx)^2Y^2"_p, "");
    presentation::add_rule(p, "((xy)^3y(xY)^2Y(xy)^2xY)^2"_p, "");
    presentation::add_rule(p, "((xy)^3y)^2(xy)^2xYxyx(yxy)^4xyxY"_p, "");
    presentation::add_rule(p, "x((yx)^2y)^4xy^2x(YxyxY)^2xy^2"_p, "");
    presentation::sort_each_rule(p);
    presentation::sort_rules(p);

    REQUIRE(presentation::length(p) == 367);
    presentation::balance(p, "xyXY"s, "XYxy"s);

    ToddCoxeter tc(onesided, p);
    todd_coxeter::add_generating_pair(tc, "xy", "");
    tc.lookahead_style(options::lookahead_style::felsch)
        .lookahead_extent(options::lookahead_extent::partial)
        .strategy(options::strategy::hlt)
        .use_relations_in_extra(true)
        .lookahead_next(40'000'000)
        .lookahead_stop_early_ratio(0.0);
    tc.run_until([&tc]() { return tc.number_of_large_collapses() > 0; });
    tc.strategy(options::strategy::felsch)
        .def_policy(options::def_policy::unlimited);

    REQUIRE(tc.number_of_classes() == 4'032'000);
  }

  LIBSEMIGROUPS_TEST_CASE(
      "ToddCoxeter",
      "100",
      "http://brauer.maths.qmul.ac.uk/Atlas/spor/J1",
      "[todd-coxeter][standard][no-valgrind][no-coverage]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("xyXY");
    p.contains_empty_word(true);
    presentation::add_inverse_rules(p, "XYxy");
    presentation::add_rule(p, "xx", "");
    presentation::add_rule(p, "yyy", "");
    presentation::add_rule(p, "(xy)^7"_p, "");
    presentation::add_rule(p, "(xy(xyxY)^3)^5"_p, "");
    presentation::add_rule(p, "(xy(xyxY)^6(xy)^2(xY)^2)^2"_p, "");

    // Greedy reducing the presentation here makes this slower
    ToddCoxeter tc(twosided, p);
    // section_felsch(tc);
    // section_hlt(tc);
    REQUIRE(tc.number_of_classes() == 175'560);
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "101",
                          "http://brauer.maths.qmul.ac.uk/Atlas/lin/L34/",
                          "[todd-coxeter][quick][no-coverage][no-valgrind]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("xyXY");
    p.contains_empty_word(true);
    presentation::add_inverse_rules(p, "XYxy");
    presentation::add_rule(p, "xx", "");
    presentation::add_rule(p, "y^4"_p, "");
    presentation::add_rule(p, "(xy)^7"_p, "");
    presentation::add_rule(p, "(xyy)^5"_p, "");
    presentation::add_rule(p, "(XYxy)^5"_p, "");
    presentation::add_rule(p, "((xy)^2xY)^5"_p, "");
    presentation::add_rule(p, "((xy)^3yxY)^5"_p, "");
    ToddCoxeter tc(twosided, p);

    section_felsch(tc);
    section_hlt(tc);
    section_CR_style(tc);
    section_Cr_style(tc);
    section_R_over_C_style(tc);
    section_Rc_style(tc);

    REQUIRE(tc.number_of_classes() == 20'160);
    check_complete_compatible(tc);
  }

  // Takes about 10 seconds (2021 - MacBook Air M1 - 8GB RAM)
  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "102",
                          "http://brauer.maths.qmul.ac.uk/Atlas/clas/S62 x 2",
                          "[todd-coxeter][extreme]") {
    auto                      rg = ReportGuard(true);
    Presentation<std::string> p;
    p.alphabet("xyXY");
    p.contains_empty_word(true);
    presentation::add_inverse_rules(p, "XYxy");
    presentation::add_rule(p, "xx", "");
    presentation::add_rule(p, "y^7"_p, "");
    presentation::add_rule(p, "(xy)^9"_p, "");
    presentation::add_rule(p, "(xyy)^12"_p, "");
    presentation::add_rule(p, "((XY)^3(xy)^3)^2"_p, "");
    presentation::add_rule(p, "(XYxy)^3"_p, "");
    presentation::add_rule(p, "(XY^2xy^2)^2"_p, "");
    // presentation::greedy_reduce_length(p); makes this slower for both hlt
    // and Felsch

    ToddCoxeter tc(twosided, p);
    section_felsch(tc);
    section_hlt(tc);

    REQUIRE(tc.number_of_classes() == 1'451'520);
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "103",
                          "redundant rule x1",
                          "[todd-coxeter][quick]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.contains_empty_word(true);
    p.alphabet("abcd");
    presentation::add_rule(p, "aa", "a");
    presentation::add_rule(p, "ad", "d");
    presentation::add_rule(p, "bb", "b");
    presentation::add_rule(p, "ca", "ac");
    presentation::add_rule(p, "cc", "c");
    presentation::add_rule(p, "da", "d");
    presentation::add_rule(p, "dc", "cd");
    presentation::add_rule(p, "dd", "d");
    presentation::add_rule(p, "aba", "a");
    presentation::add_rule(p, "bab", "b");
    presentation::add_rule(p, "bcb", "b");
    presentation::add_rule(p, "bcd", "cd");
    presentation::add_rule(p, "cbc", "c");
    presentation::add_rule(p, "cdb", "cd");
    ToddCoxeter tc(twosided, p);
    REQUIRE(tc.number_of_classes() == 24);
    auto it = redundant_rule(p, std::chrono::milliseconds(10));
    REQUIRE(it == p.rules.end());
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "104",
                          "redundant rule x2",
                          "[todd-coxeter][standard]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("abc");
    presentation::add_rule(p, "a", "abb");
    presentation::add_rule(p, "b", "baa");
    presentation::add_rule(p, "c", "abbabababaaababababab");

    auto it = todd_coxeter::redundant_rule(p, std::chrono::milliseconds(100));
    REQUIRE(it == p.rules.cend());

    presentation::add_rule(p, "b", "abb");
    it = todd_coxeter::redundant_rule(p, std::chrono::milliseconds(100));
    REQUIRE(it != p.rules.cend());
    REQUIRE(*it == "b");
    REQUIRE(*(it + 1) == "baa");
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "105",
                          "hypo plactic id monoid",
                          "[todd-coxeter][standard]") {
    std::array<uint64_t, 11> const num
        = {0, 0, 4, 13, 40, 121, 364, 1'093, 3'280, 9'841, 29'524};
    // A003462
    auto rg = ReportGuard(false);
    for (size_t n = 4; n < 11; ++n) {
      auto p = presentation::examples::hypo_plactic_monoid(n);
      p.contains_empty_word(true);
      presentation::add_idempotent_rules_no_checks(
          p, (seq<size_t>() | take(n) | to_vector()));
      ToddCoxeter tc(twosided, p);
      REQUIRE(tc.number_of_classes() == num[n] + 1);
      auto  fp = to<FroidurePin>(tc);
      Gabow scc(fp.right_cayley_graph());
      REQUIRE(scc.number_of_components() == num[n]);
      REQUIRE(fp.number_of_idempotents() == std::pow(2, n) - 1);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "106",
                          "Chinese id monoid",
                          "[todd-coxeter][standard]") {
    std::array<uint64_t, 11> const num = {
        0, 0, 4, 14, 50, 187, 730, 2'949, 12'234, 51'821, 223'190};  // A007317
    std::vector<std::vector<uint64_t>> tri
        = {{1},
           {1, 1},
           {1, 2, 2},
           {1, 3, 6, 4, 1},
           {1, 4, 12, 16, 13, 4, 1},
           {1, 5, 20, 40, 55, 41, 20, 5, 1},
           {1, 6, 30, 80, 155, 186, 156, 80, 30, 6, 1}};
    auto rg = ReportGuard(false);
    for (size_t n = 2; n < 11; ++n) {
      auto p = presentation::examples::chinese_monoid(n);
      p.contains_empty_word(true);
      presentation::add_idempotent_rules_no_checks(
          p, (seq<size_t>() | take(n) | to_vector()));
      ToddCoxeter tc(twosided, p);
      REQUIRE(tc.number_of_classes() == num[n] + 1);
      auto  fp = to<FroidurePin>(tc);
      Gabow scc(fp.right_cayley_graph());
      REQUIRE(scc.number_of_components() == num[n]);
      REQUIRE(fp.number_of_idempotents() == std::pow(2, n) - 1);

      if (n <= 6) {
        std::vector<uint64_t> length = {};
        for (auto&& nf : normal_forms(tc)) {
          while (nf.size() >= length.size()) {
            length.push_back(0);
          }
          length[nf.size()]++;
        }
        REQUIRE(length == tri[n]);
      }
    }
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "107",
                          "Chinese id monoid x 2",
                          "[todd-coxeter][fail]") {
    auto n = 5;
    auto p = presentation::examples::chinese_monoid(n);
    p.contains_empty_word(true);
    presentation::add_idempotent_rules_no_checks(
        p, (seq<size_t>() | take(n) | to_vector()));
    ToddCoxeter tc(twosided, p);
    tc.run();

    // for (auto const& w : normal_forms(tc)) {
    //   if (reduce(tc, w + w) == w) {
    //     std::cout << w << std::endl;
    //   }
    // }

    REQUIRE(reduce(tc, 21323143_w) == 314637_w);
    // REQUIRE(reduce(tc, 314637_w) == 314637_w);
    // REQUIRE(reduce(tc, 314636_w) == 314636_w);
    // REQUIRE(reduce(tc, 314635_w) == 31463_w);
    // REQUIRE(reduce(tc, 314634_w) == ""_w);
    // REQUIRE(reduce(tc, 314633_w) == ""_w);
    // REQUIRE(reduce(tc, 314632_w) == ""_w);
    // REQUIRE(reduce(tc, 314631_w) == 3461_w);
    // REQUIRE(reduce(tc, 314630_w) == 13460_w);

    REQUIRE(reduce(tc, 13042564870_w) == 123456780_w);
    REQUIRE(reduce(tc, 13042564871_w) == 230456781_w);
    REQUIRE(reduce(tc, 13042564872_w) == 130456782_w);
    REQUIRE(reduce(tc, 13042564873_w) == 1304256783_w);
    REQUIRE(reduce(tc, 13042564874_w) == 1304256784_w);
    REQUIRE(reduce(tc, 13042564875_w) == 1304264785_w);
    REQUIRE(reduce(tc, 13042564876_w) == 13042564786_w);
    REQUIRE(reduce(tc, 13042564877_w) == 1304256487_w);
    REQUIRE(reduce(tc, 13042564878_w) == 1304256487_w);
    // std::vector ws = {1230_w, 2031_w, 10231_w, 12032_w,
    // 102132_w}; std::vector ws = {012_w, 021_w, 102_w, 120_w,
    // 1021_w}; std::vector ws = {1203_w, 0123_w, 0213_w, 1023_w,
    // 10213_w}; std::vector ws
    //     = {123043_w, 1203243_w, 203143_w, 1023143_w, 10213243_w};

    // auto involution = [&tc](word_type& w) {
    //   std::reverse(w.begin(), w.end());
    //   for (auto& a : w) {
    //     a = 4 - a;
    //   }
    //   w = reduce(tc, w);
    // };

    // std::for_each(ws.begin(), ws.end(), involution);
    // REQUIRE((iterator_range(ws.begin(), ws.end()) | ToString("abcde")
    //          | to_vector())
    //         == std::vector<std::string>());

    // REQUIRE(reduce(tc, 0123012_w) == 1230_w);
    // REQUIRE(reduce(tc, 3021_w) == ""_w);
    //  REQUIRE(reduce(tc, 01_w) == ""_w);
    //  std::vector<std::string> word =  //{"bcda",
    //                                  // "bdac",
    //                                  // "cadb",
    //                                  // "cdab",
    //                                  // "bacdb",
    //                                  // "badbc",
    //                                  // "bcadc",
    //                                  // "bcdad",
    //                                  // "cadbc",
    //                                  // "cadbd",
    //                                  // "bacbdc",
    //                                  // "bacdbd",
    //                                  // "bcadcd",
    //                                  // "bacbdcd"};
    //     {"abcd",
    //      "abdc",
    //      "acbd",
    //      "acdb",
    //      "bacd",
    //      "badc",
    //      "bcad",
    //      "bcda",
    //      "cadb",
    //      "acbdc",
    //      "bacbd",
    //      "bacdb",
    //      "bcadc",
    //      "bacbdc"};
    //  auto q       = to<Presentation<std::string>>(p);
    //  auto convert = [&](auto const& w) {
    //    return to_string(q, reduce(tc, to_word(q, w)));
    //  };

    // REQUIRE((iterator_range(word.begin(), word.end())
    //          | transform([](auto const& w) { return "d" + w; })
    //          | transform(convert) | to_vector())
    //         == std::vector<std::string>());

    REQUIRE((normal_forms(tc) | ToString("abcd") | to_vector())
            == std::vector<std::string>());
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "108",
                          "plactic (n, 1)-id monoid",
                          "[todd-coxeter][quick][no-valgrind]") {
    // auto                          r = 3, s = 2;
    // std::array<uint64_t, 7> const size = {1, 3, 14, 95, 885, 10'858,
    // 170'209};

    // auto                          r = 2, s = 1;
    // std::array<uint64_t, 9> const size
    //     = {1, 2, 5, 15, 52, 203, 27'509, 248'127, 2'434'129};
    // auto                          r = 3, s = 1;
    // std::array<uint64_t, 9> const size
    //     = {0, 3, 13, 71, 457, 3'355, 27'509, 248'127, 2'434'129};
    // https://oeis.org/A126390

    auto                          r = 4, s = 1;
    std::array<uint64_t, 8> const size
        = {0, 0, 0, 199, 1'876, 20'257, 245'017, 3'266'914};
    // https://oeis.org/A284859

    // auto                           r = 5, s = 1;
    // std::array<uint64_t, 7> const size
    //     = {0, 0, 0, 429, 5329, 75989, 1'215'481};
    // std::array<uint64_t, 11> const num_idem
    //    = {1, 2, 4, 8, 16, 32, 64, 128, 256};
    // https://oeis.org/A126390
    using words::pow;
    for (size_t n = 5; n < 6; ++n) {  // size.size(); ++n) {
      auto p = presentation::examples::plactic_monoid(n);
      p.contains_empty_word(true);

      for (size_t a = 0; a < n; ++a) {
        presentation::add_rule(p, pow({a}, r), pow({a}, s));
      }
      // REQUIRE(p.rules == std::vector<word_type>());
      ToddCoxeter tc(twosided, p);
      REQUIRE(tc.number_of_classes() == size[n]);
      //  REQUIRE(todd_coxeter::number_of_idempotents(tc) == num_idem[n]);
      //  std::vector possible1
      //      = {012_w,     0122_w,       0112_w,      01122_w,
      //         0012_w,    00122_w,      00112_w,     001122_w,
      //         2012_w,    220122_w,     20112_w,     2201122_w,
      //         20012_w,   2200122_w,    200112_w,    22001122_w,
      //         1012_w,    10122_w,      110112_w,    1101122_w,
      //         10012_w,   100122_w,     1100112_w,   11001122_w,
      //         12012_w,   1220122_w,    1120112_w,   112201122_w,
      //         120012_w,  12200122_w,   11200112_w,  1122001122_w,
      //         212012_w,  221220122_w,  21120112_w,  22112201122_w,
      //         2120012_w, 2212200122_w, 211200112_w, 221122001122_w};
      //  auto pair = todd_coxeter::first_equivalent_pair(
      //      tc, possible1.cbegin(), possible1.cend());
      //  REQUIRE(!contains(tc, 2012_w, 20122_w));
      //  REQUIRE(!contains(tc, 2012_w, 22012_w));
      //  REQUIRE(*pair.first == 2012_w);
      //  REQUIRE(*pair.second == 220122_w);

      //  REQUIRE(!todd_coxeter::is_traversal(
      //      tc, possible1.cbegin(), possible1.cend()));
      //  // REQUIRE(partition(tc, possible1.cbegin(), possible1.cend())
      //  //       == std::vector<std::vector<word_type>>());

      //  std::vector possible2 = {
      //      012_w,     0122_w,     0112_w,     01122_w,    0012_w, 00122_w,
      //      00112_w,   001122_w,   2012_w,     22012_w,    20112_w,
      //      220112_w, 20012_w,   220012_w,   200112_w,   2200112_w,  1012_w,
      //      10122_w, 11012_w,   110122_w,   10012_w,    100122_w, 110012_w,
      //      1100122_w, 12012_w,   122012_w,   112012_w,   1122012_w,
      //      120012_w, 1220012_w, 1120012_w, 11220012_w, 212012_w, 2212012_w,
      //      2112012_w, 22112012_w, 2120012_w, 22120012_w, 21120012_w,
      //      221120012_w};
      //  REQUIRE(
      //      todd_coxeter::is_traversal(tc, possible2.cbegin(),
      //      possible2.cend()));
      //  std::vector possible3 = {
      //      0123_w,         01233_w,         01223_w,         012233_w,
      //      01123_w,        011233_w,        011223_w,        0112233_w,
      //      00123_w,        001233_w,        001223_w,        0012233_w,
      //      001123_w,       0011233_w,       0011223_w,       00112233_w,
      //      30123_w,        330123_w,        301223_w,        3301223_w,
      //      301123_w,       3301123_w,       3011223_w,       33011223_w,
      //      300123_w,       3300123_w,       3001223_w,       33001223_w,
      //      3001123_w,      33001123_w,      30011223_w,      330011223_w,
      //      20123_w,        201233_w,        220123_w,        2201233_w,
      //      201123_w,       2011233_w,       2201123_w,       22011233_w,
      //      200123_w,       2001233_w,       2200123_w,       22001233_w,
      //      2001123_w,      20011233_w,      22001123_w,      220011233_w,
      //      230123_w,       2330123_w,       2230123_w,       22330123_w,
      //      2301123_w,      23301123_w,      22301123_w,      223301123_w,
      //      2300123_w,      23300123_w,      22300123_w,      223300123_w,
      //      23001123_w,     233001123_w,     223001123_w,     2233001123_w,
      //      3230123_w,      33230123_w,      32230123_w,      332230123_w,
      //      32301123_w,     332301123_w,     322301123_w,     3322301123_w,
      //      32300123_w,     332300123_w,     322300123_w,     3322300123_w,
      //      323001123_w,    3323001123_w,    3223001123_w,    33223001123_w,
      //      10123_w,        101233_w,        101223_w,        1012233_w,
      //      110123_w,       1101233_w,       1101223_w,       11012233_w,
      //      100123_w,       1001233_w,       1001223_w,       10012233_w,
      //      1100123_w,      11001233_w,      11001223_w,      110012233_w,
      //      130123_w,       1330123_w,       1301223_w,       13301223_w,
      //      1130123_w,      11330123_w,      11301223_w,      113301223_w,
      //      1300123_w,      13300123_w,      13001223_w,      133001223_w,
      //      11300123_w,     113300123_w,     113001223_w,     1133001223_w,
      //      3130123_w,      33130123_w,      31301223_w,      331301223_w,
      //      31130123_w,     331130123_w,     311301223_w,     3311301223_w,
      //      31300123_w,     331300123_w,     313001223_w,     3313001223_w,
      //      311300123_w,    3311300123_w,    3113001223_w,    33113001223_w,
      //      120123_w,       1201233_w,       1220123_w,       12201233_w,
      //      1120123_w,      11201233_w,      11220123_w,      112201233_w,
      //      1200123_w,      12001233_w,      12200123_w,      122001233_w,
      //      11200123_w,     112001233_w,     112200123_w,     1122001233_w,
      //      1230123_w,      12330123_w,      12230123_w,      122330123_w,
      //      11230123_w,     112330123_w,     112230123_w,     1122330123_w,
      //      12300123_w,     123300123_w,     122300123_w,     1223300123_w,
      //      112300123_w,    1123300123_w,    1122300123_w,    11223300123_w,
      //      31230123_w,     331230123_w,     312230123_w,     3312230123_w,
      //      311230123_w,    3311230123_w,    3112230123_w,    33112230123_w,
      //      312300123_w,    3312300123_w,    3122300123_w,    33122300123_w,
      //      3112300123_w,   33112300123_w,   31122300123_w, 331122300123_w,
      //      2120123_w,      21201233_w,      22120123_w,      221201233_w,
      //      21120123_w,     211201233_w,     221120123_w,     2211201233_w,
      //      21200123_w,     212001233_w,     221200123_w,     2212001233_w,
      //      211200123_w,    2112001233_w,    2211200123_w,    22112001233_w,
      //      21230123_w,     212330123_w,     221230123_w,     2212330123_w,
      //      211230123_w,    2112330123_w,    2211230123_w,    22112330123_w,
      //      212300123_w,    2123300123_w,    2212300123_w,    22123300123_w,
      //      2112300123_w,   21123300123_w,   22112300123_w, 221123300123_w,
      //      231230123_w,    2331230123_w,    2231230123_w,    22331230123_w,
      //      2311230123_w,   23311230123_w,   22311230123_w, 223311230123_w,
      //      2312300123_w,   23312300123_w,   22312300123_w, 223312300123_w,
      //      23112300123_w,  233112300123_w,  223112300123_w,
      //      2233112300123_w, 3231230123_w,   33231230123_w,   32231230123_w,
      //      332231230123_w, 32311230123_w,  332311230123_w,  322311230123_w,
      //      3322311230123_w, 32312300123_w,  332312300123_w, 322312300123_w,
      //      3322312300123_w, 323112300123_w, 3323112300123_w,
      //      3223112300123_w, 33223112300123_w};
      //  REQUIRE(
      //      todd_coxeter::is_traversal(tc, possible3.cbegin(),
      //      possible3.cend()));
      //
      // #include "bla.txt"
      //      REQUIRE(
      //          todd_coxeter::is_traversal(tc, possible4.cbegin(),
      //          possible4.cend()));
      //
      //      NormalForms nf(tc, possible4.begin(), possible4.end());
      //      REQUIRE(std::all_of(possible4.begin(),
      //                          possible4.end(),
      //                          [&nf](auto const& w) { return nf(w) == w;
      //                          }));
      //
      // REQUIRE(partition(tc, possible3.cbegin(), possible3.cend())
      //         == std::vector<std::vector<word_type>>());
      // REQUIRE(reduce(tc, 3413401234_w) == ""_w);
      // REQUIRE((normal_forms(tc) | ToString("abcd")
      //          | to_vector())
      //         == std::vector<std::string>());
    }
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "109",
                          "plactic (n, 1)-id monoid x 2",
                          "[todd-coxeter][fail]") {
    using words::pow;
    using words::operator+;
    // #include "Plact4-1_3_last.txt"
    size_t n = 4, r = 4, s = 1;

    auto p = presentation::examples::plactic_monoid(n);
    p.contains_empty_word(true);

    for (size_t a = 0; a < n; ++a) {
      presentation::add_rule(p, pow({a}, r), pow({a}, s));
    }
    // REQUIRE(p.rules == std::vector<word_type>());
    ToddCoxeter tc(twosided, p);
    REQUIRE(contains(tc, 30011222_w, 32200112_w));

    // NormalForms nf(tc, words.begin(), words.end());
    // REQUIRE(std::all_of(words.begin(), words.end(), [&nf](auto const& w) {
    //   return nf(w) == w;
    // }));

    // auto                   w = 212_w;
    // std::vector<word_type> result;
    // for (auto i = 0; i < 4; ++i) {
    //   for (auto j = 0; j < 4; ++j) {
    //     for (auto k = 0; k < 4; ++k) {
    //       result.push_back(nf(w + pow(0_w, i) + pow(1_w, j) + pow(2_w,
    //       k)));
    //     }
    //   }
    // }
    // REQUIRE(result == std::vector<word_type>());
    //  for (auto it = w.begin() + 1; it != w.end(); ++it) {
    //    word_type ww(w.begin(), it);
    //    std::cout << "prefix = " << ww << ", normal form = " << nf(ww)
    //              << std::endl;
    //  }
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "110",
                          "sigma-plactic monoid",
                          "[todd-coxeter][quick]") {
    auto p = presentation::examples::sigma_plactic_monoid({2, 2, 2});
    p.contains_empty_word(true);
    ToddCoxeter tc(twosided, p);

    REQUIRE(tc.number_of_classes() == 15);

    REQUIRE((normal_forms(tc) | to_vector())
            == std::vector<word_type>({""_w,
                                       0_w,
                                       1_w,
                                       2_w,
                                       01_w,
                                       02_w,
                                       10_w,
                                       12_w,
                                       20_w,
                                       21_w,
                                       012_w,
                                       021_w,
                                       102_w,
                                       210_w,
                                       1021_w}));
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "111",
                          "2-sylvester monoid",
                          "[todd-coxeter][extreme]") {
    auto rg = ReportGuard(true);
    using words::pow;
    size_t                  n = 4;
    Presentation<word_type> p;
    p.alphabet(n);
    p.contains_empty_word(true);
    for (size_t a = 0; a < n; ++a) {
      presentation::add_rule(p, pow({a}, 3), {a});
    }
    using words::operator+;
    WordRange    words;
    words.alphabet_size(n).min(0).max(8);

    for (size_t a = 0; a < n - 1; ++a) {
      for (size_t b = a; b < n - 1; ++b) {
        for (size_t c = b + 1; c < n; ++c) {
          for (auto& v : words) {
            presentation::add_rule(p, a + (c + v) + b, c + (a + v) + b);
          }
        }
      }
    }

    ToddCoxeter tc(twosided, p);
    REQUIRE(tc.number_of_classes() == 3'945);
  }

  // Takes nearly 13 hours to complete
  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "112",
                          "Whyte's 8-generator 4-relation full transf monoid 8",
                          "[todd-coxeter][fail]") {
    auto                    rg = ReportGuard(true);
    Presentation<word_type> p;
    p.rules = {
        00_w,         {},           11_w,        {},         22_w,     {},
        33_w,         {},           44_w,        {},         55_w,     {},
        66_w,         {},           010_w,       101_w,      121_w,    212_w,
        232_w,        323_w,        343_w,       434_w,      454_w,    545_w,
        565_w,        656_w,        606_w,       060_w,      0102_w,   2010_w,
        0103_w,       3010_w,       0104_w,      4010_w,     0105_w,   5010_w,
        0106_w,       6010_w,       1210_w,      0121_w,     1213_w,   3121_w,
        1214_w,       4121_w,       1215_w,      5121_w,     1216_w,   6121_w,
        2320_w,       0232_w,       2321_w,      1232_w,     2324_w,   4232_w,
        2325_w,       5232_w,       2326_w,      6232_w,     3430_w,   0343_w,
        3431_w,       1343_w,       3432_w,      2343_w,     3435_w,   5343_w,
        3436_w,       6343_w,       4540_w,      0454_w,     4541_w,   1454_w,
        4542_w,       2454_w,       4543_w,      3454_w,     4546_w,   6454_w,
        5650_w,       0565_w,       5651_w,      1565_w,     5652_w,   2565_w,
        5653_w,       3565_w,       5654_w,      4565_w,     6061_w,   1606_w,
        6062_w,       2606_w,       6063_w,      3606_w,     6064_w,   4606_w,
        6065_w,       5606_w,       071654321_w, 16543217_w, 217121_w, 17171_w,
        7010270102_w, 0102720107_w, 7010701_w,   1070170_w,  1217_w,   7121_w};

    p.alphabet_from_rules();

    presentation::balance_no_checks(p, 0123456_w, 0123456_w);
    REQUIRE(p.rules
            == std::vector(
                {00_w,      {},      11_w,         {},           22_w,
                 {},        33_w,    {},           44_w,         {},
                 55_w,      {},      66_w,         {},           101_w,
                 010_w,     212_w,   121_w,        323_w,        232_w,
                 434_w,     343_w,   545_w,        454_w,        656_w,
                 565_w,     606_w,   060_w,        2010_w,       0102_w,
                 3010_w,    0103_w,  4010_w,       0104_w,       5010_w,
                 0105_w,    6010_w,  0106_w,       1210_w,       0121_w,
                 3121_w,    1213_w,  4121_w,       1214_w,       5121_w,
                 1215_w,    6121_w,  1216_w,       2320_w,       0232_w,
                 2321_w,    1232_w,  4232_w,       2324_w,       5232_w,
                 2325_w,    6232_w,  2326_w,       3430_w,       0343_w,
                 3431_w,    1343_w,  3432_w,       2343_w,       5343_w,
                 3435_w,    6343_w,  3436_w,       4540_w,       0454_w,
                 4541_w,    1454_w,  4542_w,       2454_w,       4543_w,
                 3454_w,    6454_w,  4546_w,       5650_w,       0565_w,
                 5651_w,    1565_w,  5652_w,       2565_w,       5653_w,
                 3565_w,    5654_w,  4565_w,       6061_w,       1606_w,
                 6062_w,    2606_w,  6063_w,       3606_w,       6064_w,
                 4606_w,    6065_w,  5606_w,       071654321_w,  16543217_w,
                 217121_w,  17171_w, 7010270102_w, 0102720107_w, 7010701_w,
                 1070170_w, 7121_w,  1217_w}));
    // REQUIRE(presentation::length(p) == 398);
    // REQUIRE(presentation::longest_subword_reducing_length(p) == 010_w);
    // presentation::replace_word_with_new_generator(p, 010_w);
    // REQUIRE(presentation::length(p) == 370);
    // presentation::greedy_reduce_length(p);
    // presentation::reduce_complements(p);
    // REQUIRE(presentation::length(p) == 253);

    {
      ToddCoxeter tc(twosided, p);
      tc.strategy(options::strategy::hlt)
          .lookahead_extent(options::lookahead_extent::full)
          .lookahead_growth_factor(1.01)
          .lookahead_next(32'000'000)
          .lookahead_min(24'000'000)
          .lower_bound(16'777'216);
      REQUIRE(tc.number_of_classes() == 0);
    }

    presentation::add_rule(p, 07_w, 7_w);
    presentation::add_rule(p, 70_w, 7_w);
    {
      ToddCoxeter tc(twosided, p);
      REQUIRE(tc.number_of_classes() == 40'321);
    }
    // TODO(1) lookahead_max
    // .lookahead_style(options::lookahead_style::felsch)
    // .lower_bound(16'777'216);

    WordGraph<uint32_t> wg;
    {
      using Transf_ = LeastTransf<8>;
      FroidurePin<Transf_> S;
      S.add_generator(make<Transf_>({1, 2, 3, 4, 5, 6, 7, 0}));
      S.add_generator(make<Transf_>({1, 0, 2, 3, 4, 5, 6, 7}));
      S.add_generator(make<Transf_>({0, 0, 2, 3, 4, 5, 6, 7}));

      REQUIRE(S.size() == 16'777'216);
      wg = S.left_cayley_graph();
    }

    {
      ToddCoxeter<word_type> tc(twosided, wg);
      todd_coxeter::add_generating_pair(tc, 12_w, 2_w);
      todd_coxeter::add_generating_pair(tc, 21_w, 2_w);
      REQUIRE(tc.number_of_classes() == 40'321);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "113",
                          "Whyte's 2-generator 4-relation full transf monoid 8",
                          "[todd-coxeter][fail]") {
    auto                    rg = ReportGuard(true);
    Presentation<word_type> p;
    p.rules = {00_w,
               {},
               11111111_w,
               {},
               01010101010101_w,
               {},
               011111110101111111010111111101_w,
               {},
               01111110110111111011_w,
               {},
               01111101110111110111_w,
               {},
               01111011110111101111_w,
               {},
               01110111110111011111_w,
               {},
               01101111110110111111_w,
               {},
               010111111101211111110101111111_w,
               201111111010201111111010_w,
               1111110112111111011_w,
               201111111010201111111010_w,
               0111111010101111110101201111110101011111101012_w,
               2011111101010111111010120111111010101111110101_w,
               0111111101211111110102_w,
               21111111012111111101_w};
    p.alphabet_from_rules();
    REQUIRE(presentation::length(p) == 385);
    REQUIRE(presentation::longest_subword_reducing_length(p) == 11111101_w);
    presentation::replace_word_with_new_generator(p, 11111101_w);
    REQUIRE(presentation::length(p) == 212);
    // REQUIRE(presentation::longest_subword_reducing_length(p) == 01_w);
    // presentation::replace_word_with_new_generator(p, 01_w);
    // REQUIRE(presentation::length(p) == 174);

    ToddCoxeter tc(twosided, p);

    tc.strategy(options::strategy::felsch)
        .lookahead_extent(options::lookahead_extent::partial)
        .lookahead_growth_factor(1.03)
        .lookahead_next(16'000'000);
    REQUIRE(tc.number_of_classes() == 0);
  }

  LIBSEMIGROUPS_TEST_CASE(
      "ToddCoxeter",
      "114",
      "minimal E-disjunctive idempotent pure onesided congruence",
      "[todd-coxeter][quick]") {
    auto rg     = ReportGuard(false);
    using PPerm = LeastPPerm<5>;
    FroidurePin<PPerm> S;
    S.add_generator(make<PPerm>({1, 3, 4}, {0, 4, 3}, 5));
    S.add_generator(make<PPerm>({1, 3, 4}, {2, 4, 3}, 5));
    S.add_generator(make<PPerm>({0, 3, 4}, {1, 4, 3}, 5));
    S.add_generator(make<PPerm>({2, 3, 4}, {1, 4, 3}, 5));

    REQUIRE(S.size() == 11);
    auto p = to<Presentation<std::string>>(S);
    REQUIRE(p.alphabet() == "abcd");
    presentation::change_alphabet(p, "xyXY");
    REQUIRE(p.alphabet() == "xyXY");
    auto it = knuth_bendix::redundant_rule(p, std::chrono::milliseconds(100));
    while (it != p.rules.end()) {
      // std::cout << std::endl
      //           << "REMOVING " << *it << " = " << *(it + 1) << std::endl;
      p.rules.erase(it, it + 2);
      it = knuth_bendix::redundant_rule(p, std::chrono::milliseconds(100));
    }
    REQUIRE(p.rules
            == std::vector<std::string>({"xy",  "xx", "xY",  "xx", "yx",  "xx",
                                         "yX",  "xx", "yY",  "xX", "XY",  "xx",
                                         "YX",  "xx", "xXx", "x",  "xXy", "y",
                                         "XxX", "X",  "YxX", "Y"}));
  }

  // Takes about 2 minutes
  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "115",
                          "https://brauer.maths.qmul.ac.uk/Atlas/exc/TF42",
                          "[todd-coxeter][extreme]") {
    auto                      rg = ReportGuard(true);
    Presentation<std::string> p;
    p.contains_empty_word(true).alphabet("xyXY");
    presentation::add_inverse_rules(p, "XYxy");

    presentation::add_rule(p, "x^2"_p, ""_p);
    presentation::add_rule(p, "y^3"_p, ""_p);
    presentation::add_rule(p, "(xy)^13"_p, ""_p);
    presentation::add_rule(p, "(XYxy)^5"_p, ""_p);
    presentation::add_rule(p, "(XYXYxyxy)^4"_p, ""_p);
    presentation::add_rule(p, "(xyxyxyxyxY)^6"_p, ""_p);
    REQUIRE(presentation::length(p) == 151);
    presentation::replace_word_with_new_generator(
        p, presentation::longest_subword_reducing_length(p));
    REQUIRE(presentation::length(p) == 90);
    // Can't balance don't know the inverse of the new generator,
    // presentation::balance(p, "xyXY", "XYxy");

    ToddCoxeter tc(twosided, p);
    tc.lower_bound(17'971'200).strategy(options::strategy::felsch);
    REQUIRE(tc.number_of_classes() == 17'971'200);
    tc.standardize(Order::shortlex);
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "116",
                          "cyclic groups",
                          "[todd-coxeter][quick]") {
    Presentation<std::string> p;
    p.contains_empty_word(true).alphabet("xyz");

    presentation::add_rule(p, "x^2"_p, "y"_p);
    presentation::add_rule(p, "y^2"_p, "z"_p);
    presentation::add_rule(p, "xz"_p, ""_p);

    ToddCoxeter tc(twosided, p);
    REQUIRE(tc.number_of_classes() == 5);
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "117",
                          "Rudvalis group",
                          "[todd-coxeter][extreme]") {
    auto                      rg = ReportGuard(true);
    Presentation<std::string> p;
    p.alphabet("abctABCT").contains_empty_word(true);
    presentation::add_inverse_rules(p, "ABCTabct");
    presentation::add_rule(p, "a^7"_p, "");
    presentation::add_rule(p, "b^3"_p, "");
    presentation::add_rule(p, "c^2"_p, "");
    presentation::add_rule(p, "t^2"_p, "");
    presentation::add_rule(p, "BCbc"_p, "");
    presentation::add_rule(p, "TCtc"_p, "");
    presentation::add_rule(p, "(ac)^6"_p, "");
    presentation::add_rule(p, "(ACac)^4"_p, "");
    presentation::add_rule(p, "(bt)^3"_p, "");
    presentation::add_rule(p, "BabA^2"_p, "");
    presentation::add_rule(p, "(abc)^7"_p, "");
    presentation::add_rule(p, "(ab^2t)^3"_p, "");
    presentation::add_rule(p, "TACABacatACAbaca"_p, "");
    presentation::add_rule(p, "TB(AC)^2acabtBACA(ca)^2b"_p, "");
    presentation::add_rule(p, "tA^3ta^3BtAtabACA^2(A^3C)^2a^3ca^5ca"_p, "");
    presentation::balance_no_checks(p, p.alphabet(), std::string("ABCTabct"));

    REQUIRE(presentation::length(p) == 183);
    presentation::reverse(p);

    ToddCoxeter tc(onesided, p);
    todd_coxeter::add_generating_pair(tc, "a", "");
    todd_coxeter::add_generating_pair(tc, "b", "");
    todd_coxeter::add_generating_pair(tc, "c", "");
    tc.strategy(options::strategy::felsch).use_relations_in_extra(true);

    REQUIRE(tc.number_of_classes() == 7'238'400);
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "118",
                          "alternating group 8",
                          "[todd-coxeter][standard]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("abcABC").contains_empty_word(true);
    presentation::add_inverse_rules(p, "ABCabc");
    presentation::add_rule(p, "a^7"_p, "");
    presentation::add_rule(p, "b^3"_p, "");
    presentation::add_rule(p, "c^2"_p, "");
    presentation::add_rule(p, "BCbc"_p, "");
    presentation::add_rule(p, "(ac)^6"_p, "");
    presentation::add_rule(p, "(ACac)^4"_p, "");
    presentation::add_rule(p, "BabAA"_p, "");
    presentation::add_rule(p, "(abc)^7"_p, "");
    presentation::balance_no_checks(p, p.alphabet(), "ABCabc"s);

    ToddCoxeter tc(twosided, p);
    tc.strategy(options::strategy::felsch).use_relations_in_extra(true);

    REQUIRE(tc.number_of_classes() == 20'160);
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "119",
                          "full transf. monoid 6 (maybe)",
                          "[todd-coxeter][extreme]") {
    ReportGuard             rg(true);
    Presentation<word_type> p;
    p.contains_empty_word(true);

    p.rules = {00_w,
               {},
               11_w,
               {},
               22_w,
               {},
               33_w,
               {},
               44_w,
               {},
               010101_w,
               {},
               121212_w,
               {},
               232323_w,
               {},
               343434_w,
               {},
               404040_w,
               {},
               01020102_w,
               {},
               01030103_w,
               {},
               01040104_w,
               {},
               12101210_w,
               {},
               12131213_w,
               {},
               12141214_w,
               {},
               23202320_w,
               {},
               23212321_w,
               {},
               23242324_w,
               {},
               34303430_w,
               {},
               34313431_w,
               {},
               34323432_w,
               {},
               40414041_w,
               {},
               40424042_w,
               {},
               40434043_w,
               {},
               5401054010_w,
               4010540105_w,
               010501050105010_w,
               50105_w,
               1215121_w,
               5151_w,
               14321512341_w,
               05_w};
    p.alphabet_from_rules();
    p.throw_if_bad_alphabet_or_rules();
    presentation::change_alphabet(p, 501234_w);

    ToddCoxeter tc(twosided, p);
    REQUIRE(tc.number_of_classes() == 46'656);

    auto S = to<FroidurePin>(tc);
    REQUIRE((froidure_pin::rules(S) | rx::count()) == 17'785);
    auto kb = to<KnuthBendix<word_type>>(twosided, S);
    auto q  = kb.presentation();
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "120",
                          "check full enum not triggered",
                          "[todd-coxeter][quick][no-valgrind]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("abcd");
    presentation::add_rule(p, "aa", "a");
    presentation::add_rule(p, "ba", "b");
    presentation::add_rule(p, "ab", "b");
    presentation::add_rule(p, "ca", "c");
    presentation::add_rule(p, "ac", "c");
    presentation::add_rule(p, "da", "d");
    presentation::add_rule(p, "ad", "d");
    presentation::add_rule(p, "bb", "a");
    presentation::add_rule(p, "cd", "a");
    presentation::add_rule(p, "ccc", "a");
    presentation::add_rule(p, "bcbcbcbcbcbcbc", "a");
    presentation::add_rule(p, "bcbdbcbdbcbdbcbdbcbdbcbdbcbdbcbd", "a");

    ToddCoxeter tc(congruence_kind::twosided, p);
    tc.run_for(std::chrono::milliseconds(1));
    REQUIRE(!tc.finished());
    tc.standardize(Order::shortlex);
    // Have to standardize or otherwise what we are about to do below
    // makes no sense
    // REQUIRE(word_graph::is_standardized(tc.current_word_graph()));
    REQUIRE(!tc.finished());

    auto const& wg    = tc.current_word_graph();
    auto        set   = word_graph::nodes_reachable_from(wg, 0);
    auto        nodes = std::vector<uint32_t>(set.begin(), set.end());
    std::sort(nodes.begin(), nodes.end());
    REQUIRE(!nodes.empty());
    for (auto s : nodes) {
      REQUIRE(s == nodes[s]);
    }
    auto n = *std::max_element(nodes.begin(), nodes.end());

    REQUIRE(todd_coxeter::current_index_of(
                tc, todd_coxeter::current_word_of(tc, n - 1))
            == n - 1);
    REQUIRE(!tc.finished());

    std::vector<std::string> words
        = {"bcddbdadbdaccacababddddbbbbbbcbacdadccadbcddbdadbdaccacababddddbbbb"
           "bbcbacdadccadbdaacbacddcbcbbcccdbaccacbbdaccbbbabaaabbdaacbacddcbcb"
           "bcccdbaccacbbdaccbbbabaaabbdaacbacddcbcbbcccdbaccacbbdaccbbbabaaabb"
           "daacbacddcbcbbcccdbaccacbbdaccbbbabaaabbdaacbacddcbcbbcccdbaccacbbd"
           "accbbbabaaabbdaacbacddcbcbbcccdbaccacbbdaccbbbabaaabbdaacbacddcbcbb"
           "cccdbaccacbbdaccbbbabaaabdaacbacddcbcbbcccdbaccacbbdaccbbbabaaabdaa"
           "cbacddcbcbbcccdbaccacbbdaccbbbabaaabdaacbacddcbcbbcccdbaccacbbdaccb"
           "bbabaaabdaacbacddcbcbbcccdbaccacbbdaccbbbabaaabdaacbacddcbcbbcccdba"
           "ccacbbdaccbbbabaaabbbbbb",
           "bdaacbacddcbcbbcccdbaccacbbdaccbbbabaaab",
           "aadccbaadccccaaacdcccabccdccdadddcabdbdb",
           "bacddcbadabaddaddbacbadacacacbbaabbbdbbb",
           "cdabbabdbabbbbbcabadadddadcbdbbbcdacbcaa",
           "acabccdadadddcdcbbdbbbabdbadccadddabdcab",
           "bccddaccbabdbcabcabadcdbbadcabdddcccdada",
           "bdaabccbccdbcaadabbbbbabababddcbbdcddcad",
           "cbbcbcbcabadabcadcdcdccbbdbcbdaadcbbccab",
           "dcdbdcbcadaaaccaddcdcccbccdcbcddaccaabdc",
           "acdbaadcaddcbdbbacccbaacdcbdcccabdcaddcb",
           "acaadacdcdabaccaaabbcbbbcccdaababdacdcba",
           "acabaacdcbdbbbbbbdacabbdadbdcccabcdcbddc",
           "ddbcacdbbaaacabdacbccccacabdaabddddadabc",
           "acccadabaacbcdaaabccddcddccdadadacbabdac",
           "acdbcddbdbcbacccadabbbbdabbbbbdbabdbbabb",
           "baaadddabcbcdccdaccacbaddaaacccddcabacbd",
           "abcbababdcadbabbdddcbcdcddddcbbbbdcdadbc",
           "aadddcddbaadaadaadaaaabbddbbbccbbdcdcaca",
           "cdcbadadbbcdaaccbcaaabaddbcdbccbaacbbaca",
           "acaacbadadcbabaccbbadcddaacabbadcbadbadd",
           "babbccbdacacacdbaacbddcacbcbccccdbddbcbc",
           "bccacbabcdbaaacadcdcdcdccbbdaccdaaadcccd",
           "bccdbacacbdccbccddacbcbdbddbaccadcdcdacd",
           "abdacacbaddaababbdbbbbdabddcdbdbcdbabbaa",
           "bcddbacddccbccadbcbbcbcabddcbbdacabaabaa",
           "bcdabdaabddcaaaaaabacddcdbaaabaaddacdabd",
           "adbcbbcdadddccccddddccbdccabccadacdcccca",
           "ccbaadcdabcdabdddbccaddbbaabdbbaadadcdbb",
           "adcaddbacaaddaddaaadadbcbcaccaacdbbdaccd",
           "bdbdababadcbbdddccadbccbcdcbdabbdabbdaba",
           "bcbdbacbcbcaddaadbddaaaddbddcddcabbadddc",
           "dcacadabacaaabacdccdbdcddcaacdaaaababbdc",
           "cbcaccdcdddaadbbdcacdccbddcaadcdddadcabd",
           "babbadacbdbbadbcbdcdbbdbcdabcadccddabddd",
           "aaabcacdbdcabdaddccadcabddbddbddbbcbddbb",
           "abcbbabdbababadacbcbdbdbacacdcbcabbcbabb",
           "abaadcdaccdcdcbbdcadacaaacbacadcccaabcbc",
           "ccdcadbaccdaccbabcdccdabcbbabccabababddb",
           "bccaccacdcdacacdadccbaacdddccbdcdbcdadbc"};

    std::vector<std::string> expected(words);
    std::for_each(expected.begin(), expected.end(), [&](std::string& word) {
      word = todd_coxeter::reduce_no_run(tc, word);
    });

    for (size_t i = 0; i != expected.size(); ++i) {
      REQUIRE(todd_coxeter::reduce_no_run(tc, expected[i]) == expected[i]);
      REQUIRE(
          std::make_pair(todd_coxeter::reduce_no_run(tc, words[i]), words[i])
          == std::make_pair(expected[i], words[i]));
      REQUIRE(todd_coxeter::currently_contains(tc, words[i], expected[i])
              == tril::TRUE);
    }
    REQUIRE(tc.number_of_classes() == 10'752);
    for (size_t i = 0; i != expected.size(); ++i) {
      REQUIRE(todd_coxeter::reduce_no_run(tc, words[i])
              == todd_coxeter::reduce_no_run(tc, expected[i]));
    }
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "121",
                          "standardize perf",
                          "[todd-coxeter][extreme]") {
    ReportGuard               rg(true);
    Presentation<std::string> p;
    p.alphabet("ab");
    ToddCoxeter tc(twosided, p);
    tc.strategy(options::strategy::felsch);
    tc.run_until([&tc]() {
      return tc.current_word_graph().number_of_nodes_active() > 256'000'000;
    });

    REQUIRE(tc.current_word_graph().number_of_nodes_active() > 256'000'000);
    REQUIRE(!tc.standardize(Order::shortlex));
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "122",
                          "initialisation from ToddCoxeter",
                          "[todd-coxeter][quick]") {
    ReportGuard               rg(true);
    Presentation<std::string> p;
    p.alphabet("ab");
    p.contains_empty_word(true);

    ToddCoxeter tc(twosided, p);
    todd_coxeter::add_generating_pair(tc, "ab", "");
    REQUIRE(tc.generating_pairs() == std::vector<std::string>({"ab", ""}));
    REQUIRE(tc.presentation().rules.empty());
    REQUIRE(tc.internal_generating_pairs()
            == std::vector<word_type>({01_w, {}}));
    REQUIRE(tc.internal_presentation().rules.empty());

    tc = ToddCoxeter(twosided, tc);
    REQUIRE(tc.generating_pairs().empty());
    REQUIRE(tc.presentation().rules == std::vector<std::string>({"ab", ""}));
    REQUIRE(tc.internal_generating_pairs().empty());
    REQUIRE(tc.internal_presentation().rules
            == std::vector<word_type>({01_w, {}}));

    todd_coxeter::add_generating_pair(tc, "bbbbb", "aaa");
    REQUIRE(tc.generating_pairs()
            == std::vector<std::string>({"bbbbb", "aaa"}));
    REQUIRE(tc.internal_generating_pairs()
            == std::vector<word_type>({11111_w, 000_w}));

    tc.init(twosided, tc);
    REQUIRE(tc.generating_pairs().empty());
    REQUIRE(tc.presentation().rules
            == std::vector<std::string>({"ab", "", "bbbbb", "aaa"}));
    REQUIRE(tc.internal_generating_pairs().empty());
    REQUIRE(tc.internal_presentation().rules
            == std::vector<word_type>({01_w, {}, 11111_w, 000_w}));
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "123",
                          "initialisation from ToddCoxeter",
                          "[todd-coxeter][quick]") {
    ReportGuard             rg(true);
    Presentation<word_type> p;
    p.alphabet(2);
    p.contains_empty_word(true);

    ToddCoxeter tc(twosided, p);
    todd_coxeter::add_generating_pair(tc, 01_w, {});
    REQUIRE(tc.generating_pairs() == std::vector<word_type>({01_w, {}}));
    REQUIRE(tc.internal_generating_pairs()
            == std::vector<word_type>({01_w, {}}));
    REQUIRE(tc.presentation().rules.empty());
    REQUIRE(tc.internal_presentation().rules.empty());

    tc = ToddCoxeter(twosided, tc);
    REQUIRE(tc.generating_pairs().empty());
    REQUIRE(tc.presentation().rules == std::vector<word_type>({01_w, {}}));
    REQUIRE(tc.internal_generating_pairs().empty());
    REQUIRE(tc.internal_presentation().rules
            == std::vector<word_type>({01_w, {}}));

    todd_coxeter::add_generating_pair(tc, 11111_w, 0_w);
    REQUIRE(tc.generating_pairs() == std::vector<word_type>({11111_w, 0_w}));
    REQUIRE(tc.internal_generating_pairs()
            == std::vector<word_type>({11111_w, 0_w}));

    tc.init(twosided, tc);
    REQUIRE(tc.generating_pairs().empty());
    REQUIRE(tc.presentation().rules
            == std::vector<word_type>({01_w, {}, 11111_w, 0_w}));
    REQUIRE(tc.internal_generating_pairs().empty());
    REQUIRE(tc.internal_presentation().rules
            == std::vector<word_type>({01_w, {}, 11111_w, 0_w}));
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "124",
                          "initialisation from incomplete WordGraph",
                          "[todd-coxeter][standard]") {
    size_t n = 7;
    auto   p = presentation::examples::full_transformation_monoid_II74(n);

    REQUIRE(p.contains_empty_word());
    ToddCoxeter tc(congruence_kind::twosided, p);
    tc.run_for(std::chrono::milliseconds(100));

    tc.shrink_to_fit();
    size_t const expected = tc.current_word_graph().number_of_nodes();

    // TODO(0) this should throw without the shrink_to_fit
    word_graph::throw_if_any_target_out_of_bounds(tc.current_word_graph());
    tc.init(
        congruence_kind::twosided, tc.presentation(), tc.current_word_graph());
    word_graph::throw_if_any_target_out_of_bounds(tc.current_word_graph());

    REQUIRE(expected == tc.current_word_graph().number_of_nodes());

    todd_coxeter::add_generating_pair(tc, 0_w, {});
    todd_coxeter::add_generating_pair(tc, 1_w, {});
    // FIXME and there's some sort of off by one error here, the following
    // throws
    // REQUIRE(tc.number_of_classes() == 0);
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "125",
                          "refuse to run if free and using HLT",
                          "[todd-coxeter][quick]") {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.alphabet(2);

    ToddCoxeter tc(congruence_kind::twosided, p);
    tc.strategy(options::strategy::hlt);
    todd_coxeter::add_generating_pair(tc, 01_w, 10_w);
    todd_coxeter::add_generating_pair(tc, 000000000000_w, 000_w);
    todd_coxeter::add_generating_pair(tc, 1111111111_w, 111111111_w);

    REQUIRE(tc.number_of_classes() == 119);

    // KnuthBendix kb(congruence_kind::twosided, p);
    // knuth_bendix::add_generating_pair(kb, 01_w, 10_w);
    // knuth_bendix::add_generating_pair(kb, 000000000000_w, 000_w);
    // knuth_bendix::add_generating_pair(kb, 1111111111_w, 111111111_w);

    // REQUIRE(kb.number_of_classes() == 119);

    tc.init(congruence_kind::onesided, p);
    tc.strategy(options::strategy::hlt);

    todd_coxeter::add_generating_pair(tc, 01_w, 10_w);
    todd_coxeter::add_generating_pair(tc, 000000000000_w, 000_w);
    todd_coxeter::add_generating_pair(tc, 1111111111_w, 111111111_w);

    // Before the changes in this commit, this returned 21, which is very
    // wrong (there are infinitely many classes)
    // REQUIRE(tc.number_of_classes() == 21);
    REQUIRE_THROWS_AS(tc.number_of_classes(), LibsemigroupsException);

    tc.init(congruence_kind::twosided, p);
    tc.strategy(options::strategy::hlt);

    REQUIRE_THROWS_AS(tc.run(), LibsemigroupsException);
    REQUIRE_THROWS_AS(tc.run_for(std::chrono::milliseconds(10)),
                      LibsemigroupsException);
    size_t val = 0;
    REQUIRE_THROWS_AS(tc.run_until([&val]() { return val++ > 10; }),
                      LibsemigroupsException);

    tc.init(congruence_kind::onesided, p);
    tc.strategy(options::strategy::hlt);

    // Before the changes in this commit:
    // runs for 1 microsecond, ends up in some not very good state.
    // tc.run_for(std::chrono::milliseconds(10));
    // REQUIRE(tc.current_word_graph().number_of_nodes() == 2);
    // REQUIRE(tc.current_word_graph().number_of_edges() == 0);
    // REQUIRE(!tc.finished());
    // REQUIRE(tc.started());

    REQUIRE_THROWS_AS(tc.run(), LibsemigroupsException);
    REQUIRE_THROWS_AS(tc.run_for(std::chrono::milliseconds(10)),
                      LibsemigroupsException);
    val = 0;
    REQUIRE_THROWS_AS(tc.run_until([&val]() { return val++ > 10; }),
                      LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                          "126",
                          "reduce_no_run_no_checks on unstarted",
                          "[todd-coxeter][quick]") {
    Presentation<word_type> p;
    p.alphabet(2).contains_empty_word(true);

    presentation::add_rule(p, 000_w, 0_w);
    presentation::add_rule(p, 1111_w, 1_w);
    presentation::add_rule(p, 0101_w, 00_w);
    ToddCoxeter tc(onesided, p);
    todd_coxeter::add_generating_pair(tc, 01_w, 10_w);

    REQUIRE(todd_coxeter::reduce_no_run_no_checks(tc, 0101_w) == 0101_w);
  }

}  // namespace libsemigroups
