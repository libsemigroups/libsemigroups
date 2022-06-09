//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019-2021 James D. Mitchell
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

// The purpose of this file is to test the ToddCoxeter classes.

#include <algorithm>      // for is_sorted, copy, all_of
#include <array>          // for array
#include <chrono>         // for duration, milliseconds
#include <cstddef>        // for size_t
#include <fstream>        // for ofstream
#include <functional>     // for mem_fn
#include <iostream>       // for string, operator<<, ostream
#include <memory>         // for unique_ptr, shared_ptr
#include <string>         // for basic_string, operator==
#include <unordered_map>  // for operator!=, operator==
#include <unordered_set>  // for unordered_set
#include <utility>        // for pair
#include <vector>         // for vector, operator==, swap

#define CATCH_CONFIG_ENABLE_PAIR_STRINGMAKER

#include "catch.hpp"            // for operator""_catch_sr, Asser...
#include "fpsemi-examples.hpp"  // for setup, RennerTypeDMonoid
#include "test-main.hpp"        // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/bmat8.hpp"        // for BMat8
#include "libsemigroups/cong-intf.hpp"    // for CongruenceInterface::class...
#include "libsemigroups/cong-wrap.hpp"    // for CongruenceWrapper<>::wrapp...
#include "libsemigroups/constants.hpp"    // for operator==, operator!=
#include "libsemigroups/containers.hpp"   // for DynamicArray2, DynamicArra...
#include "libsemigroups/fpsemi-intf.hpp"  // for FpSemigroupInterface::rule...
#include "libsemigroups/fpsemi.hpp"       // for FpSemigroup
#include "libsemigroups/froidure-pin-base.hpp"  // for FroidurePinBase
#include "libsemigroups/froidure-pin.hpp"  // for FroidurePin, FroidurePin<>...
#include "libsemigroups/iterator.hpp"      // for ConstIteratorStateful, ope...
#include "libsemigroups/knuth-bendix.hpp"  // for KnuthBendix
#include "libsemigroups/order.hpp"         // for RecursivePathCompare, Lexi...
#include "libsemigroups/report.hpp"        // for ReportGuard
#include "libsemigroups/string.hpp"        // for operator<<, to_string
#include "libsemigroups/tce.hpp"           // for TCE, operator<<, IncreaseD...
#include "libsemigroups/todd-coxeter.hpp"  // for ToddCoxeter, operator|
#include "libsemigroups/transf.hpp"        // for Transf, LeastTransf
#include "libsemigroups/types.hpp"         // for word_type, relation_type
#include "libsemigroups/wislo.hpp"         // for const_wislo_iterator, cbeg...

namespace libsemigroups {
  struct LibsemigroupsException;  // Forward declaration
  constexpr bool REPORT              = false;
  congruence_kind constexpr twosided = congruence_kind::twosided;
  congruence_kind constexpr left     = congruence_kind::left;
  congruence_kind constexpr right    = congruence_kind::right;
  using KnuthBendix                  = fpsemigroup::KnuthBendix;
  using tc_order                     = congruence::ToddCoxeter::order;
  using options                      = congruence::ToddCoxeter::options;

  namespace {
    // Test functions
    void check_felsch(congruence::ToddCoxeter& var) {
      SECTION("Felsch + no standardisation") {
        var.strategy(options::strategy::felsch).standardize(false);
      }
      SECTION("Felsch + standardisation") {
        var.strategy(options::strategy::felsch).standardize(true);
      }
    }

    void check_felsch(fpsemigroup::ToddCoxeter& var) {
      check_felsch(var.congruence());
    }

    void check_felsch_throws(congruence::ToddCoxeter& var) {
      SECTION("Felsch (throws)") {
        REQUIRE_THROWS_AS(var.strategy(options::strategy::felsch),
                          LibsemigroupsException);
      }
    }

    void check_felsch_throws(fpsemigroup::ToddCoxeter& var) {
      check_felsch_throws(var.congruence());
    }

    void check_hlt_no_save(congruence::ToddCoxeter& var) {
      SECTION("HLT + no standardise + full lookahead + no save") {
        var.strategy(options::strategy::hlt);
        var.standardize(false).lookahead(options::lookahead::full).save(false);
      }
      SECTION("HLT + standardise + full lookahead + no save") {
        var.strategy(options::strategy::hlt);
        var.standardize(true).lookahead(options::lookahead::full).save(false);
      }
      SECTION("HLT + no standardise + partial lookahead + no save") {
        var.strategy(options::strategy::hlt);
        var.standardize(false)
            .lookahead(options::lookahead::partial)
            .save(false);
      }
      SECTION("HLT + standardise + partial lookahead + no save") {
        var.strategy(options::strategy::hlt);
        var.standardize(true)
            .lookahead(options::lookahead::partial)
            .save(false);
      }
    }

    void check_hlt_no_save(fpsemigroup::ToddCoxeter& var) {
      check_hlt_no_save(var.congruence());
    }

    void check_hlt_save(congruence::ToddCoxeter& var) {
      SECTION("HLT + no standardise + full lookahead + save") {
        var.strategy(options::strategy::hlt);
        var.standardize(false).lookahead(options::lookahead::full).save(true);
      }
      SECTION("HLT + standardise + full lookahead + save") {
        var.strategy(options::strategy::hlt);
        var.standardize(true).lookahead(options::lookahead::full).save(true);
      }
      SECTION("HLT + no standardise + partial lookahead + save") {
        var.strategy(options::strategy::hlt);
        var.standardize(false)
            .lookahead(options::lookahead::partial)
            .save(true);
      }
      SECTION("HLT + standardise + partial lookahead + save") {
        var.strategy(options::strategy::hlt);
        var.standardize(true).lookahead(options::lookahead::partial).save(true);
      }
    }

    void check_hlt_save_throws(congruence::ToddCoxeter& var) {
      SECTION("HLT + save (throws)") {
        REQUIRE_THROWS_AS(var.strategy(options::strategy::hlt).save(true),
                          LibsemigroupsException);
      }
    }

    void check_hlt_save_throws(fpsemigroup::ToddCoxeter& var) {
      check_hlt_save_throws(var.congruence());
    }

    void check_hlt(congruence::ToddCoxeter& var) {
      check_hlt_no_save(var);
      check_hlt_save(var);
    }

    void check_hlt(fpsemigroup::ToddCoxeter& var) {
      check_hlt(var.congruence());
    }

    void check_random(congruence::ToddCoxeter& var) {
      SECTION("random strategy") {
        var.strategy(options::strategy::random);
      }
    }

    void check_random(fpsemigroup::ToddCoxeter& var) {
      check_random(var.congruence());
    }

    void check_Rc_style(congruence::ToddCoxeter& tc) {
      SECTION("Rc style + full lookahead") {
        tc.strategy(options::strategy::Rc).lookahead(options::lookahead::full);
        tc.run();
      }
      SECTION("Rc style + partial lookahead") {
        tc.strategy(options::strategy::Rc)
            .lookahead(options::lookahead::partial);
        tc.run();
      }
    }

    void check_Rc_style(fpsemigroup::ToddCoxeter& tc) {
      check_Rc_style(tc.congruence());
    }

    void check_Cr_style(congruence::ToddCoxeter& tc) {
      SECTION("Cr style") {
        tc.strategy(options::strategy::Cr);
        tc.run();
      }
    }

    void check_Cr_style(fpsemigroup::ToddCoxeter& tc) {
      check_Cr_style(tc.congruence());
    }

    void check_R_over_C_style(congruence::ToddCoxeter& tc) {
      SECTION("R/C style") {
        tc.strategy(options::strategy::R_over_C);
        tc.run();
      }
    }

    void check_R_over_C_style(fpsemigroup::ToddCoxeter& tc) {
      check_R_over_C_style(tc.congruence());
    }

    void check_CR_style(congruence::ToddCoxeter& tc) {
      SECTION("CR style") {
        tc.strategy(options::strategy::CR);
        tc.run();
      }
    }

    void check_CR_style(fpsemigroup::ToddCoxeter& tc) {
      check_CR_style(tc.congruence());
    }

    // This is how the recursive words up to a given length M, and on an
    // arbitrary finite alphabet are generated.  On a single letter alphabet,
    // this order is just increasing powers of the only generator:
    //
    //   a < aa < aaa < aaaa < ... < aa...a (M times)
    //
    // With an n-letter alphabet A = {a_1, a_2, ..., a_n}, suppose we have
    // already obtained all of the words W_{n - 1} containing {a_1, ..., a_{n
    // - 1}}.  Every word in W_{n - 1} is less than any word containing a_n,
    // and the least word greater than every word in W_{n - 1} is a_n. Words
    // greater than a_n are obtain in the follow way, where:
    //
    // x: is the maximum word in W_{n - 1}, this is constant, in the
    // description
    //    that follows.
    // u: the first word obtained in point (1), the first time it is applied
    //    after (2) has been applied, starting with u = a_{n - 1}.
    // v: a word with one fewer letters than u, starting with the empty word.
    // w: a word such that w < u, also starting with the empty word.
    //
    // 1. If v < x, then v is replaced by the next word in the order. If |uv|
    // <=
    //    M, then the next word is uv. Otherwise, goto 1.
    //
    // 2. If v = x, then and there exists a word w' in the set of words
    // obtained
    //    so far such that w' > w and |w'| <= M - 1, then replace w with w',
    //    replace u by wa_n, replace v by the empty word, and the next word is
    //    wa_n.
    //
    //    If no such word w' exists, then we have enumerated all the required
    //    words, and we can stop.
    //
    // For example, if A = {a, b} and M = 4, then the initial elements in the
    // order are:
    //
    //   e < a < aa < aaa < aaaa (e is the empty word)
    //
    // Set b > aaaa. At this point, x = aaaa, u = b, v = e, w = e, and so
    // (1) applies, v <- a, and since |uv| = ba <= 4 = M, the next word is
    // ba.  Repeatedly applying (1), until it fails to hold, we obtain the
    // following:
    //
    //   aaaa < b < ba < baa < baaa
    //
    // After defining baa < baaa, x = aaaa, u = b, v = aaaa, and w = e. Hence
    // v = x, and so (2) applies. The next w' in the set of words so far
    // enumerated is a, and |a| = 1 <= 3 = M - 1, and so w <- a, u <- ab, v <-
    // e, and the next word is ab. We repeatedly apply (1), until it fails, to
    // obtain
    //
    //   baaa < ab < aba < abaa
    //
    // At which point u = b, v = aaaa = x, and w = a. Hence (2) applies, w <-
    // aa, v <- e, u <- aab, and the next word is: aab. And so on ...
    //
    // The next function implements this order, returning the words on an
    // n-letter alphabet of length up to M.
    std::vector<word_type> recursive_path_words(size_t n, size_t M) {
      std::vector<word_type> out;
      size_t                 a = 0;
      for (size_t i = 0; i < M; ++i) {
        out.push_back(word_type(i + 1, a));
      }
      a++;
      int x = out.size();
      int u = out.size();
      int v = -1;  // -1 is the empty word
      int w = -1;  // -1 is the empty word
      out.push_back({a});
      while (a < n) {
        if (v < x - 1) {
          do {
            v++;
          } while (v < x && out[u].size() + out[v].size() > M);
          if (v < x && out[u].size() + out[v].size() <= M) {
            word_type nxt = out[u];
            nxt.insert(nxt.end(), out[v].begin(), out[v].end());
            out.push_back(nxt);
          }
        } else {
          do {
            w++;
          } while (static_cast<size_t>(w) < out.size()
                   && out[w].size() + 1 > M);
          if (static_cast<size_t>(w) < out.size()) {
            word_type nxt = out[w];
            u             = out.size();
            v             = -1;
            nxt.push_back(a);
            out.push_back(nxt);
          } else {
            a++;
            if (a < n) {
              x = out.size();
              u = out.size();
              v = -1;
              w = -1;
              out.push_back({a});
            }
          }
        }
      }
      return out;
    }

    void output_gap_benchmark_file(std::string const&       fname,
                                   congruence::ToddCoxeter& tc) {
      std::ofstream file;
      file.open(fname);
      file << "local free, rules, R, S, T;\n";
      file << tc.to_gap_string();
      file << "R := RightMagmaCongruenceByGeneratingPairs(S, []);\n";
      file << "T := CosetTableOfFpSemigroup(R);;\n";
      file << "Assert(0, Length(T) = Size(GeneratorsOfSemigroup(S)));\n";
      file << "Assert(0, Length(T[1]) - 1 = "
           << std::to_string(tc.number_of_classes()) << ");\n";
      file.close();
    }
  }  // namespace

  namespace congruence {

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "000",
                            "small 2-sided congruence",
                            "[todd-coxeter][quick]") {
      auto rg = ReportGuard(REPORT);

      ToddCoxeter tc(twosided);
      tc.set_number_of_generators(2);
      tc.add_pair({0, 0, 0}, {0});
      tc.add_pair({1, 1, 1, 1}, {1});
      tc.add_pair({0, 1, 0, 1}, {0, 0});
      REQUIRE(!tc.finished());

      check_hlt(tc);
      check_felsch(tc);
      check_random(tc);
      check_Rc_style(tc);
      check_R_over_C_style(tc);
      check_CR_style(tc);
      check_Cr_style(tc);

      REQUIRE(tc.number_of_classes() == 27);
      tc.shrink_to_fit();
      auto words
          = std::vector<word_type>(tc.cbegin_class(1, 0, 10), tc.cend_class());
      REQUIRE(words
              == std::vector<word_type>({word_type({1}),
                                         word_type({1, 1, 1, 1}),
                                         word_type({1, 1, 1, 1, 1, 1, 1})}));
      words = std::vector<word_type>(
          tc.cbegin_class(word_type({1, 1, 1, 1}), 0, 10), tc.cend_class());
      REQUIRE(words
              == std::vector<word_type>({word_type({1}),
                                         word_type({1, 1, 1, 1}),
                                         word_type({1, 1, 1, 1, 1, 1, 1})}));
      REQUIRE(tc.number_of_words(1) == POSITIVE_INFINITY);
      std::vector<size_t> class_sizes;
      for (size_t i = 0; i < tc.number_of_classes(); ++i) {
        class_sizes.push_back(tc.number_of_words(i));
      }
      REQUIRE(class_sizes
              == std::vector<size_t>(tc.number_of_classes(),
                                     size_t(POSITIVE_INFINITY)));
      REQUIRE(tc.word_to_class_index(words[0]) == 1);
      REQUIRE(
          std::all_of(words.cbegin(), words.cend(), [&tc](word_type const& w) {
            return tc.word_to_class_index(w) == 1;
          }));

      // Too small for lookahead to kick in...
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "001",
                            "small 2-sided congruence",
                            "[no-valgrind][todd-coxeter][quick]") {
      auto rg = ReportGuard(REPORT);

      ToddCoxeter tc(twosided);
      tc.set_number_of_generators(2);
      tc.add_pair({0, 0, 0}, {0});  // (a^3, a)
      tc.add_pair({0}, {1, 1});     // (a, b^2)
      REQUIRE(!tc.finished());

      check_hlt(tc);
      check_felsch(tc);
      check_random(tc);
      check_Rc_style(tc);
      check_R_over_C_style(tc);
      check_CR_style(tc);
      check_Cr_style(tc);

      REQUIRE(tc.number_of_classes() == 5);
      REQUIRE(tc.finished());
      REQUIRE(!tc.is_standardized());

      REQUIRE(tc.word_to_class_index({0, 0, 1})
              == tc.word_to_class_index({0, 0, 0, 0, 1}));
      REQUIRE(tc.word_to_class_index({0, 1, 1, 0, 0, 1})
              == tc.word_to_class_index({0, 0, 0, 0, 1}));

      REQUIRE(tc.word_to_class_index({0, 0, 0}) != tc.word_to_class_index({1}));
      tc.standardize(tc_order::lex);
      REQUIRE(tc.class_index_to_word(0) == word_type({0}));
      REQUIRE(tc.class_index_to_word(1) == word_type({0, 0}));
      REQUIRE(tc.class_index_to_word(2) == word_type({0, 0, 1}));
      REQUIRE(tc.class_index_to_word(3) == word_type({0, 0, 1, 0}));
      REQUIRE(tc.word_to_class_index(word_type({0, 0, 0, 1})) == 3);
      REQUIRE(tc.class_index_to_word(4) == word_type({1}));
      REQUIRE(tc.word_to_class_index(tc.class_index_to_word(0)) == 0);
      REQUIRE(tc.word_to_class_index(tc.class_index_to_word(1)) == 1);
      REQUIRE(tc.word_to_class_index(tc.class_index_to_word(2)) == 2);
      REQUIRE(tc.word_to_class_index(tc.class_index_to_word(3)) == 3);
      REQUIRE(tc.word_to_class_index(tc.class_index_to_word(4)) == 4);
      REQUIRE(tc.word_to_class_index({0, 1}) == 3);
      REQUIRE(LexicographicalCompare<word_type>{}({0, 0, 1}, {0, 1}));

      REQUIRE(std::is_sorted(tc.cbegin_normal_forms(),
                             tc.cend_normal_forms(),
                             LexicographicalCompare<word_type>{}));

      tc.standardize(tc_order::shortlex);
      REQUIRE(std::vector<word_type>(tc.cbegin_normal_forms(),
                                     tc.cend_normal_forms())
              == std::vector<word_type>({{0}, {1}, {0, 0}, {0, 1}, {0, 0, 1}}));
      REQUIRE(tc.word_to_class_index(tc.class_index_to_word(0)) == 0);
      REQUIRE(tc.word_to_class_index(tc.class_index_to_word(1)) == 1);
      REQUIRE(tc.word_to_class_index(tc.class_index_to_word(2)) == 2);
      REQUIRE(tc.word_to_class_index(tc.class_index_to_word(3)) == 3);
      REQUIRE(tc.word_to_class_index(tc.class_index_to_word(4)) == 4);
      REQUIRE(std::is_sorted(tc.cbegin_normal_forms(),
                             tc.cend_normal_forms(),
                             ShortLexCompare<word_type>{}));

      auto nf = std::vector<word_type>(tc.cbegin_normal_forms(),
                                       tc.cend_normal_forms());
      REQUIRE(nf
              == std::vector<word_type>({{0}, {1}, {0, 0}, {0, 1}, {0, 0, 1}}));
      REQUIRE(std::all_of(nf.begin(), nf.end(), [&tc](word_type& w) {
        return w == *tc.cbegin_class(w, 0, w.size() + 1);
      }));

      for (size_t i = 2; i < 6; ++i) {
        for (size_t j = 2; j < 10 - i; ++j) {
          auto v = std::vector<word_type>(
              cbegin_wislo(i, {0}, word_type(j + 1, 0)),
              cend_wislo(i, {0}, word_type(j + 1, 0)));
          std::sort(v.begin(), v.end(), RecursivePathCompare<word_type>{});
          REQUIRE(v == recursive_path_words(i, j));
        }
      }
      tc.standardize(tc_order::recursive);
      REQUIRE(tc.class_index_to_word(0) == word_type({0}));
      REQUIRE(tc.class_index_to_word(1) == word_type({0, 0}));
      REQUIRE(tc.class_index_to_word(2) == word_type({1}));
      REQUIRE(tc.class_index_to_word(3) == word_type({1, 0}));
      REQUIRE(tc.class_index_to_word(4) == word_type({1, 0, 0}));
      REQUIRE(std::is_sorted(tc.cbegin_normal_forms(),
                             tc.cend_normal_forms(),
                             RecursivePathCompare<word_type>{}));
    }

    // Felsch is actually faster here!
    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "002",
                            "Example 6.6 in Sims (see also KnuthBendix 013)",
                            "[todd-coxeter][standard]") {
      using TCE = detail::TCE;
      using FroidurePinTCE
          = FroidurePin<TCE, FroidurePinTraits<TCE, TCE::Table>>;

      auto rg = ReportGuard(REPORT);

      ToddCoxeter tc(twosided);
      tc.set_number_of_generators(4);
      tc.add_pair({0, 0}, {0});
      tc.add_pair({1, 0}, {1});
      tc.add_pair({0, 1}, {1});
      tc.add_pair({2, 0}, {2});
      tc.add_pair({0, 2}, {2});
      tc.add_pair({3, 0}, {3});
      tc.add_pair({0, 3}, {3});
      tc.add_pair({1, 1}, {0});
      tc.add_pair({2, 3}, {0});
      tc.add_pair({2, 2, 2}, {0});
      tc.add_pair({1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2}, {0});
      tc.add_pair({1, 2, 1, 3, 1, 2, 1, 3, 1, 2, 1, 3, 1, 2, 1, 3,
                   1, 2, 1, 3, 1, 2, 1, 3, 1, 2, 1, 3, 1, 2, 1, 3},
                  {0});

      check_hlt(tc);
      check_felsch(tc);
      check_random(tc);
      check_Rc_style(tc);
      check_R_over_C_style(tc);
      check_CR_style(tc);
      check_Cr_style(tc);

      REQUIRE(tc.number_of_classes() == 10'752);
      REQUIRE(tc.complete());
      REQUIRE(tc.compatible());

      // Take a copy to test copy constructor
      auto& S = static_cast<FroidurePinTCE&>(*tc.quotient_froidure_pin());
      auto  T = S.copy_closure({S.generator(0)});

      REQUIRE(T.size() == S.size());
      REQUIRE(T.number_of_generators() == S.number_of_generators());

      REQUIRE(S.size() == 10'752);
      REQUIRE(S.number_of_idempotents() == 1);
      for (size_t c = 0; c < tc.number_of_classes(); ++c) {
        REQUIRE(tc.class_index_to_word(c) == S.factorisation(c));
        REQUIRE(tc.word_to_class_index(tc.class_index_to_word(c)) == c);
      }
      REQUIRE(tc.finished());

      tc.standardize(tc_order::recursive);
      REQUIRE(std::is_sorted(tc.cbegin_normal_forms(),
                             tc.cend_normal_forms(),
                             RecursivePathCompare<word_type>{}));
      REQUIRE(std::vector<word_type>(tc.cbegin_normal_forms(),
                                     tc.cbegin_normal_forms() + 10)
              == std::vector<word_type>({{{0},
                                          {1},
                                          {2},
                                          {2, 1},
                                          {1, 2},
                                          {1, 2, 1},
                                          {2, 2},
                                          {2, 2, 1},
                                          {2, 1, 2},
                                          {2, 1, 2, 1}}}));

      tc.standardize(tc_order::lex);
      for (size_t c = 0; c < tc.number_of_classes(); ++c) {
        REQUIRE(tc.word_to_class_index(tc.class_index_to_word(c)) == c);
      }
      REQUIRE(std::is_sorted(tc.cbegin_normal_forms(),
                             tc.cend_normal_forms(),
                             LexicographicalCompare<word_type>{}));
      REQUIRE(std::vector<word_type>(tc.cbegin_normal_forms(),
                                     tc.cbegin_normal_forms() + 10)
              == std::vector<word_type>({{0},
                                         {0, 1},
                                         {0, 1, 2},
                                         {0, 1, 2, 1},
                                         {0, 1, 2, 1, 2},
                                         {0, 1, 2, 1, 2, 1},
                                         {0, 1, 2, 1, 2, 1, 2},
                                         {0, 1, 2, 1, 2, 1, 2, 1},
                                         {0, 1, 2, 1, 2, 1, 2, 1, 2},
                                         {0, 1, 2, 1, 2, 1, 2, 1, 2, 1}}));
      tc.standardize(tc_order::shortlex);
      for (size_t c = 0; c < tc.number_of_classes(); ++c) {
        REQUIRE(tc.word_to_class_index(tc.class_index_to_word(c)) == c);
      }
      REQUIRE(std::is_sorted(tc.cbegin_normal_forms(),
                             tc.cend_normal_forms(),
                             ShortLexCompare<word_type>{}));
      REQUIRE(std::vector<word_type>(tc.cbegin_normal_forms(),
                                     tc.cbegin_normal_forms() + 10)
              == std::vector<word_type>({{0},
                                         {1},
                                         {2},
                                         {3},
                                         {1, 2},
                                         {1, 3},
                                         {2, 1},
                                         {3, 1},
                                         {1, 2, 1},
                                         {1, 3, 1}}));
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "003",
                            "constructed from FroidurePin",
                            "[no-valgrind][todd-coxeter][quick][no-coverage]") {
      auto rg = ReportGuard(REPORT);

      FroidurePin<BMat8> S(
          {BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
           BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
           BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {1, 0, 0, 1}}),
           BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}})});

      ToddCoxeter tc(twosided, S);
      tc.froidure_pin_policy(options::froidure_pin::use_relations);
      tc.add_pair({0}, {1});

      check_felsch(tc);
      check_hlt(tc);
      check_random(tc);
      check_Rc_style(tc);
      check_R_over_C_style(tc);
      check_CR_style(tc);
      check_Cr_style(tc);

      tc.random_interval(std::chrono::milliseconds(100));
      tc.lower_bound(3);

      tc.run();
      REQUIRE(tc.complete());
      REQUIRE(tc.compatible());
      REQUIRE(tc.number_of_classes() == 3);
      // REQUIRE(tc.number_of_generators() == 4);
      REQUIRE(tc.contains({0}, {1}));
      tc.standardize(tc_order::shortlex);
      REQUIRE(tc.contains({0}, {1}));
      tc.shrink_to_fit();
      REQUIRE(tc.contains({0}, {1}));

      auto& T = *tc.quotient_froidure_pin();
      REQUIRE(T.size() == 3);
      REQUIRE(tc.class_index_to_word(0) == T.factorisation(0));
      REQUIRE(tc.class_index_to_word(1) == T.factorisation(1));
      REQUIRE(tc.class_index_to_word(2) == T.factorisation(2));

      REQUIRE(tc.class_index_to_word(0) == word_type({0}));
      REQUIRE(tc.class_index_to_word(1) == word_type({2}));
      REQUIRE(tc.class_index_to_word(2) == word_type({0, 0}));
      REQUIRE(tc.word_to_class_index(tc.class_index_to_word(0)) == 0);
      REQUIRE(tc.word_to_class_index(tc.class_index_to_word(1)) == 1);
      REQUIRE(tc.word_to_class_index(tc.class_index_to_word(2)) == 2);

      tc.standardize(tc_order::lex);
      REQUIRE(tc.class_index_to_word(0) == word_type({0}));
      REQUIRE(tc.class_index_to_word(1) == word_type({0, 0}));
      REQUIRE(tc.class_index_to_word(2) == word_type({0, 0, 2}));
      REQUIRE(tc.word_to_class_index(tc.class_index_to_word(0)) == 0);
      REQUIRE(tc.word_to_class_index(tc.class_index_to_word(1)) == 1);
      REQUIRE(tc.word_to_class_index(tc.class_index_to_word(2)) == 2);

      tc.standardize(tc_order::shortlex);
      REQUIRE(tc.class_index_to_word(0) == word_type({0}));
      REQUIRE(tc.class_index_to_word(1) == word_type({2}));
      REQUIRE(tc.class_index_to_word(2) == word_type({0, 0}));
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "004",
                            "2-sided congruence from FroidurePin",
                            "[todd-coxeter][quick]") {
      auto rg = ReportGuard(REPORT);

      using Transf = LeastTransf<5>;
      FroidurePin<Transf> S({Transf({1, 3, 4, 2, 3}), Transf({3, 2, 1, 3, 3})});

      REQUIRE(S.size() == 88);

      ToddCoxeter tc(twosided, S);
      tc.froidure_pin_policy(options::froidure_pin::use_cayley_graph);
      tc.add_pair(S.factorisation(Transf({3, 4, 4, 4, 4})),
                  S.factorisation(Transf({3, 1, 3, 3, 3})));
      REQUIRE(!tc.finished());
      tc.shrink_to_fit();  // does nothing
      REQUIRE(!tc.finished());
      tc.standardize(tc_order::none);  // does nothing
      REQUIRE(!tc.finished());

      check_hlt_no_save(tc);
      check_hlt_save_throws(tc);
      check_felsch_throws(tc);
      check_random(tc);

      REQUIRE(tc.number_of_classes() == 21);
      tc.shrink_to_fit();
      REQUIRE(tc.number_of_classes() == 21);
      tc.standardize(tc_order::recursive);
      auto w = std::vector<word_type>(tc.cbegin_normal_forms(),
                                      tc.cend_normal_forms());
      REQUIRE(w.size() == 21);
      REQUIRE(w
              == std::vector<word_type>({{0},
                                         {0, 0},
                                         {0, 0, 0},
                                         {0, 0, 0, 0},
                                         {1},
                                         {1, 0},
                                         {1, 0, 0},
                                         {1, 0, 0, 0},
                                         {0, 1},
                                         {0, 1, 0},
                                         {0, 1, 0, 0},
                                         {0, 1, 0, 0, 0},
                                         {0, 0, 1},
                                         {1, 1},
                                         {1, 1, 0},
                                         {1, 1, 0, 0},
                                         {1, 1, 0, 0, 0},
                                         {0, 1, 1},
                                         {0, 1, 1, 0},
                                         {0, 1, 1, 0, 0},
                                         {0, 1, 1, 0, 0, 0}}));
      REQUIRE(std::unique(w.begin(), w.end()) == w.end());
      REQUIRE(std::is_sorted(tc.cbegin_normal_forms(),
                             tc.cend_normal_forms(),
                             RecursivePathCompare<word_type>{}));
      REQUIRE(std::all_of(
          tc.cbegin_normal_forms(),
          tc.cend_normal_forms(),
          [&tc](word_type const& ww) -> bool {
            return tc.class_index_to_word(tc.word_to_class_index(ww)) == ww;
          }));
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "005",
                            "non-trivial two-sided from relations",
                            "[todd-coxeter][quick]") {
      auto        rg = ReportGuard(REPORT);
      ToddCoxeter tc(twosided);
      tc.set_number_of_generators(3);
      tc.add_pair({0, 1}, {1, 0});
      tc.add_pair({0, 2}, {2, 2});
      tc.add_pair({0, 2}, {0});
      tc.add_pair({2, 2}, {0});
      tc.add_pair({1, 2}, {1, 2});
      tc.add_pair({1, 2}, {2, 2});
      tc.add_pair({1, 2, 2}, {1});
      tc.add_pair({1, 2}, {1});
      tc.add_pair({2, 2}, {1});
      tc.add_pair({0}, {1});

      check_hlt(tc);
      check_felsch(tc);
      check_random(tc);
      check_Rc_style(tc);
      check_R_over_C_style(tc);
      check_CR_style(tc);
      check_Cr_style(tc);

      REQUIRE(tc.number_of_classes() == 2);
      tc.standardize(tc_order::shortlex);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "006",
                            "small right cong. on free semigroup",
                            "[todd-coxeter][quick]") {
      auto rg = ReportGuard(REPORT);

      ToddCoxeter tc(right);
      tc.set_number_of_generators(2);
      tc.add_pair({0, 0, 0}, {0});
      tc.add_pair({0}, {1, 1});

      check_hlt(tc);
      check_felsch(tc);
      check_random(tc);
      check_Rc_style(tc);
      check_R_over_C_style(tc);
      check_CR_style(tc);
      check_Cr_style(tc);

      REQUIRE(tc.number_of_classes() == 5);
      REQUIRE(tc.finished());
      tc.standardize(tc_order::shortlex);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "007",
                            "left cong. on free semigroup",
                            "[todd-coxeter][quick]") {
      auto rg = ReportGuard(REPORT);
      {
        ToddCoxeter tc(left);
        tc.set_number_of_generators(2);
        tc.add_pair({0, 0, 0}, {0});
        tc.add_pair({0}, {1, 1});
        tc.growth_factor(1.5);

        check_hlt(tc);
        check_felsch(tc);
        check_random(tc);
        check_Rc_style(tc);
        check_R_over_C_style(tc);
        check_CR_style(tc);
        check_Cr_style(tc);

        REQUIRE(!tc.is_standardized());
        REQUIRE(tc.word_to_class_index({0, 0, 1})
                == tc.word_to_class_index({0, 0, 0, 0, 1}));
        REQUIRE(tc.word_to_class_index({0, 1, 1, 0, 0, 1})
                == tc.word_to_class_index({0, 0, 0, 0, 1}));
        REQUIRE(tc.word_to_class_index({1})
                != tc.word_to_class_index({0, 0, 0, 0}));
        REQUIRE(tc.word_to_class_index({0, 0, 0})
                != tc.word_to_class_index({0, 0, 0, 0}));
        tc.standardize(tc_order::shortlex);
        REQUIRE(tc.is_standardized());
      }
      {
        ToddCoxeter tc(left);
        REQUIRE_NOTHROW(ToddCoxeter(left, tc));
      }
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "008",
                            "for small fp semigroup",
                            "[todd-coxeter][quick]") {
      auto        rg = ReportGuard(REPORT);
      ToddCoxeter tc(twosided);
      tc.set_number_of_generators(2);
      tc.add_pair({0, 0, 0}, {0});  // (a^3, a)
      tc.add_pair({0}, {1, 1});     // (a, b^2)

      check_hlt(tc);
      check_felsch(tc);
      check_random(tc);
      check_Rc_style(tc);
      check_R_over_C_style(tc);
      check_CR_style(tc);
      check_Cr_style(tc);

      REQUIRE(tc.word_to_class_index({0, 0, 1})
              == tc.word_to_class_index({0, 0, 0, 0, 1}));
      REQUIRE(tc.word_to_class_index({0, 1, 1, 0, 0, 1})
              == tc.word_to_class_index({0, 0, 0, 0, 1}));
      REQUIRE(tc.word_to_class_index({0, 0, 0}) != tc.word_to_class_index({1}));
      REQUIRE(tc.word_to_class_index({0, 0, 0, 0}) < tc.number_of_classes());
      tc.standardize(tc_order::shortlex);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "009",
                            "2-sided cong. trans. semigroup",
                            "[todd-coxeter][quick]") {
      auto rg = ReportGuard(REPORT);
      auto S  = FroidurePin<Transf<>>(
          {Transf<>({1, 3, 4, 2, 3}), Transf<>({3, 2, 1, 3, 3})});

      REQUIRE(S.size() == 88);
      REQUIRE(S.number_of_rules() == 18);

      ToddCoxeter tc(twosided, S);
      tc.froidure_pin_policy(options::froidure_pin::use_relations);
      tc.add_pair(S.factorisation(Transf<>({3, 4, 4, 4, 4})),
                  S.factorisation(Transf<>({3, 1, 3, 3, 3})));

      check_hlt(tc);
      check_felsch(tc);
      check_random(tc);
      check_Rc_style(tc);
      check_R_over_C_style(tc);
      check_CR_style(tc);
      check_Cr_style(tc);

      REQUIRE(tc.number_of_classes() == 21);
      REQUIRE(tc.number_of_classes() == 21);

      REQUIRE(tc.word_to_class_index(S.factorisation(Transf<>({1, 3, 1, 3, 3})))
              == tc.word_to_class_index(
                  S.factorisation(Transf<>({4, 2, 4, 4, 2}))));

      tc.standardize(tc_order::shortlex);
      REQUIRE(tc.number_of_non_trivial_classes() == 1);
      REQUIRE(tc.cbegin_ntc()->size() == 68);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "010",
                            "left congruence on transformation semigroup",
                            "[todd-coxeter][quick]") {
      auto rg = ReportGuard(REPORT);
      auto S  = FroidurePin<Transf<>>(
          {Transf<>({1, 3, 4, 2, 3}), Transf<>({3, 2, 1, 3, 3})});

      REQUIRE(S.size() == 88);
      REQUIRE(S.number_of_rules() == 18);

      ToddCoxeter tc(left, S);
      tc.froidure_pin_policy(options::froidure_pin::use_relations);
      tc.add_pair(S.factorisation(Transf<>({3, 4, 4, 4, 4})),
                  S.factorisation(Transf<>({3, 1, 3, 3, 3})));

      check_hlt(tc);
      check_felsch(tc);
      check_random(tc);
      check_Rc_style(tc);
      check_R_over_C_style(tc);
      check_CR_style(tc);
      check_Cr_style(tc);

      REQUIRE(tc.number_of_classes() == 69);
      REQUIRE(tc.number_of_classes() == 69);

      REQUIRE(tc.word_to_class_index(S.factorisation(Transf<>({1, 3, 1, 3, 3})))
              != tc.word_to_class_index(
                  S.factorisation(Transf<>({4, 2, 4, 4, 2}))));

      tc.standardize(tc_order::shortlex);
      REQUIRE(tc.number_of_non_trivial_classes() == 1);
      REQUIRE(tc.cbegin_ntc()->size() == 20);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "011",
                            "right cong. trans. semigroup",
                            "[todd-coxeter][quick]") {
      auto rg = ReportGuard(REPORT);
      auto S  = FroidurePin<Transf<>>(
          {Transf<>({1, 3, 4, 2, 3}), Transf<>({3, 2, 1, 3, 3})});

      REQUIRE(S.size() == 88);
      REQUIRE(S.number_of_rules() == 18);

      ToddCoxeter tc(right, S);
      tc.froidure_pin_policy(options::froidure_pin::use_relations);
      tc.add_pair(S.factorisation(Transf<>({3, 4, 4, 4, 4})),
                  S.factorisation(Transf<>({3, 1, 3, 3, 3})));

      check_hlt(tc);
      check_felsch(tc);
      check_random(tc);
      check_Rc_style(tc);
      check_R_over_C_style(tc);
      check_CR_style(tc);
      check_Cr_style(tc);

      REQUIRE(tc.number_of_classes() == 72);
      REQUIRE(tc.number_of_classes() == 72);

      REQUIRE(tc.word_to_class_index(S.factorisation(Transf<>({1, 3, 1, 3, 3})))
              != tc.word_to_class_index(
                  S.factorisation(Transf<>({4, 2, 4, 4, 2}))));

      REQUIRE(tc.word_to_class_index(S.factorisation(Transf<>({1, 3, 3, 3, 3})))
              != tc.word_to_class_index(
                  S.factorisation(Transf<>({4, 2, 4, 4, 2}))));
      REQUIRE(tc.word_to_class_index(S.factorisation(Transf<>({2, 4, 2, 2, 2})))
              == tc.word_to_class_index(
                  S.factorisation(Transf<>({2, 3, 3, 3, 3}))));
      REQUIRE(tc.word_to_class_index(S.factorisation(Transf<>({1, 3, 3, 3, 3})))
              != tc.word_to_class_index(
                  S.factorisation(Transf<>({2, 3, 3, 3, 3}))));

      tc.standardize(tc_order::shortlex);
      REQUIRE(tc.number_of_non_trivial_classes() == 4);

      std::vector<size_t> v(tc.number_of_non_trivial_classes(), 0);
      std::transform(tc.cbegin_ntc(),
                     tc.cend_ntc(),
                     v.begin(),
                     std::mem_fn(&std::vector<word_type>::size));
      REQUIRE(std::count(v.cbegin(), v.cend(), 3) == 1);
      REQUIRE(std::count(v.cbegin(), v.cend(), 5) == 2);
      REQUIRE(std::count(v.cbegin(), v.cend(), 7) == 1);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "012",
                            "trans. semigroup (size 88)",
                            "[todd-coxeter][quick]") {
      auto rg = ReportGuard(REPORT);

      FroidurePin<Transf<>> S;
      S.add_generator(Transf<>({1, 3, 4, 2, 3}));
      S.add_generator(Transf<>({3, 2, 1, 3, 3}));

      REQUIRE(S.size() == 88);
      REQUIRE(S.number_of_rules() == 18);
      REQUIRE(S.degree() == 5);

      ToddCoxeter tc(twosided, S);
      tc.froidure_pin_policy(options::froidure_pin::use_cayley_graph);

      word_type w1, w2;
      S.factorisation(w1, S.position(Transf<>({3, 4, 4, 4, 4})));
      S.factorisation(w2, S.position(Transf<>({3, 1, 3, 3, 3})));

      tc.add_pair(w1, w2);

      check_hlt_no_save(tc);
      check_hlt_save_throws(tc);
      check_felsch_throws(tc);
      check_random(tc);

      REQUIRE(tc.number_of_classes() == 21);
      REQUIRE(tc.number_of_classes() == 21);
      word_type w3, w4;
      S.factorisation(w3, S.position(Transf<>({1, 3, 1, 3, 3})));
      S.factorisation(w4, S.position(Transf<>({4, 2, 4, 4, 2})));
      REQUIRE(tc.word_to_class_index(w3) == tc.word_to_class_index(w4));
      tc.standardize(tc_order::shortlex);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "013",
                            "left cong. on trans. semigroup (size 88)",
                            "[todd-coxeter][quick]") {
      auto                  rg = ReportGuard(REPORT);
      FroidurePin<Transf<>> S;
      S.add_generator(Transf<>({1, 3, 4, 2, 3}));
      S.add_generator(Transf<>({3, 2, 1, 3, 3}));

      REQUIRE(S.size() == 88);
      REQUIRE(S.degree() == 5);
      word_type w1, w2;
      S.factorisation(w1, S.position(Transf<>({3, 4, 4, 4, 4})));
      S.factorisation(w2, S.position(Transf<>({3, 1, 3, 3, 3})));
      ToddCoxeter tc(left, S);
      tc.froidure_pin_policy(options::froidure_pin::use_relations);
      tc.add_pair(w1, w2);

      check_hlt(tc);
      check_felsch(tc);
      check_random(tc);
      check_Rc_style(tc);
      check_R_over_C_style(tc);
      check_CR_style(tc);
      check_Cr_style(tc);

      REQUIRE(tc.number_of_classes() == 69);
      REQUIRE(tc.number_of_classes() == 69);
      tc.standardize(tc_order::shortlex);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "014",
                            "right cong. on trans. semigroup (size 88)",
                            "[todd-coxeter][quick]") {
      auto                  rg = ReportGuard(REPORT);
      FroidurePin<Transf<>> S;
      S.add_generator(Transf<>({1, 3, 4, 2, 3}));
      S.add_generator(Transf<>({3, 2, 1, 3, 3}));

      REQUIRE(S.size() == 88);
      REQUIRE(S.number_of_rules() == 18);
      REQUIRE(S.degree() == 5);
      word_type w1, w2;
      S.factorisation(w1, S.position(Transf<>({3, 4, 4, 4, 4})));
      S.factorisation(w2, S.position(Transf<>({3, 1, 3, 3, 3})));
      ToddCoxeter tc(right, S);
      tc.froidure_pin_policy(options::froidure_pin::use_relations);
      tc.add_pair(w1, w2);

      check_hlt(tc);
      check_felsch(tc);
      check_random(tc);
      check_Rc_style(tc);
      check_R_over_C_style(tc);
      check_CR_style(tc);
      check_Cr_style(tc);

      REQUIRE(tc.number_of_classes() == 72);
      REQUIRE(tc.number_of_classes() == 72);
      word_type w3, w4, w5, w6;
      S.factorisation(w3, S.position(Transf<>({1, 3, 3, 3, 3})));
      S.factorisation(w4, S.position(Transf<>({4, 2, 4, 4, 2})));
      S.factorisation(w5, S.position(Transf<>({2, 4, 2, 2, 2})));
      S.factorisation(w6, S.position(Transf<>({2, 3, 3, 3, 3})));
      REQUIRE(tc.word_to_class_index(w3) != tc.word_to_class_index(w4));
      REQUIRE(tc.word_to_class_index(w5) == tc.word_to_class_index(w6));
      REQUIRE(tc.word_to_class_index(w3) != tc.word_to_class_index(w6));
      tc.standardize(tc_order::shortlex);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "015",
                            "finite fp-semigroup, dihedral group of order 6",
                            "[todd-coxeter][quick]") {
      auto        rg = ReportGuard(REPORT);
      ToddCoxeter tc(twosided);
      tc.set_number_of_generators(5);
      tc.add_pair({0, 0}, {0});
      tc.add_pair({0, 1}, {1});
      tc.add_pair({1, 0}, {1});
      tc.add_pair({0, 2}, {2});
      tc.add_pair({2, 0}, {2});
      tc.add_pair({0, 3}, {3});
      tc.add_pair({3, 0}, {3});
      tc.add_pair({0, 4}, {4});
      tc.add_pair({4, 0}, {4});
      tc.add_pair({1, 2}, {0});
      tc.add_pair({2, 1}, {0});
      tc.add_pair({3, 4}, {0});
      tc.add_pair({4, 3}, {0});
      tc.add_pair({2, 2}, {0});
      tc.add_pair({1, 4, 2, 3, 3}, {0});
      tc.add_pair({4, 4, 4}, {0});

      check_hlt(tc);
      check_felsch(tc);
      check_random(tc);
      check_Rc_style(tc);
      check_R_over_C_style(tc);
      check_CR_style(tc);
      check_Cr_style(tc);

      REQUIRE(tc.number_of_classes() == 6);
      REQUIRE(tc.word_to_class_index({1}) == tc.word_to_class_index({2}));
      tc.standardize(tc_order::shortlex);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "016",
                            "finite fp-semigroup, size 16",
                            "[todd-coxeter][quick]") {
      auto        rg = ReportGuard(REPORT);
      ToddCoxeter tc(twosided);
      tc.set_number_of_generators(4);
      tc.add_pair({3}, {2});
      tc.add_pair({0, 3}, {0, 2});
      tc.add_pair({1, 1}, {1});
      tc.add_pair({1, 3}, {1, 2});
      tc.add_pair({2, 1}, {2});
      tc.add_pair({2, 2}, {2});
      tc.add_pair({2, 3}, {2});
      tc.add_pair({0, 0, 0}, {0});
      tc.add_pair({0, 0, 1}, {1});
      tc.add_pair({0, 0, 2}, {2});
      tc.add_pair({0, 1, 2}, {1, 2});
      tc.add_pair({1, 0, 0}, {1});
      tc.add_pair({1, 0, 2}, {0, 2});
      tc.add_pair({2, 0, 0}, {2});
      tc.add_pair({0, 1, 0, 1}, {1, 0, 1});
      tc.add_pair({0, 2, 0, 2}, {2, 0, 2});
      tc.add_pair({1, 0, 1, 0}, {1, 0, 1});
      tc.add_pair({1, 2, 0, 1}, {1, 0, 1});
      tc.add_pair({1, 2, 0, 2}, {2, 0, 2});
      tc.add_pair({2, 0, 1, 0}, {2, 0, 1});
      tc.add_pair({2, 0, 2, 0}, {2, 0, 2});

      check_hlt(tc);
      check_felsch(tc);
      check_random(tc);
      check_Rc_style(tc);
      check_R_over_C_style(tc);
      check_CR_style(tc);
      check_Cr_style(tc);

      REQUIRE(tc.number_of_classes() == 16);
      REQUIRE(tc.word_to_class_index({2}) == tc.word_to_class_index({3}));
      tc.standardize(tc_order::shortlex);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "017",
                            "finite fp-semigroup, size 16",
                            "[todd-coxeter][quick]") {
      auto        rg = ReportGuard(REPORT);
      ToddCoxeter tc(twosided);
      tc.set_number_of_generators(11);
      tc.add_pair({2}, {1});
      tc.add_pair({4}, {3});
      tc.add_pair({5}, {0});
      tc.add_pair({6}, {3});
      tc.add_pair({7}, {1});
      tc.add_pair({8}, {3});
      tc.add_pair({9}, {3});
      tc.add_pair({10}, {0});
      tc.add_pair({0, 2}, {0, 1});
      tc.add_pair({0, 4}, {0, 3});
      tc.add_pair({0, 5}, {0, 0});
      tc.add_pair({0, 6}, {0, 3});
      tc.add_pair({0, 7}, {0, 1});
      tc.add_pair({0, 8}, {0, 3});
      tc.add_pair({0, 9}, {0, 3});
      tc.add_pair({0, 10}, {0, 0});
      tc.add_pair({1, 1}, {1});
      tc.add_pair({1, 2}, {1});
      tc.add_pair({1, 4}, {1, 3});
      tc.add_pair({1, 5}, {1, 0});
      tc.add_pair({1, 6}, {1, 3});
      tc.add_pair({1, 7}, {1});
      tc.add_pair({1, 8}, {1, 3});
      tc.add_pair({1, 9}, {1, 3});
      tc.add_pair({1, 10}, {1, 0});
      tc.add_pair({3, 1}, {3});
      tc.add_pair({3, 2}, {3});
      tc.add_pair({3, 3}, {3});
      tc.add_pair({3, 4}, {3});
      tc.add_pair({3, 5}, {3, 0});
      tc.add_pair({3, 6}, {3});
      tc.add_pair({3, 7}, {3});
      tc.add_pair({3, 8}, {3});
      tc.add_pair({3, 9}, {3});
      tc.add_pair({3, 10}, {3, 0});
      tc.add_pair({0, 0, 0}, {0});
      tc.add_pair({0, 0, 1}, {1});
      tc.add_pair({0, 0, 3}, {3});
      tc.add_pair({0, 1, 3}, {1, 3});
      tc.add_pair({1, 0, 0}, {1});
      tc.add_pair({1, 0, 3}, {0, 3});
      tc.add_pair({3, 0, 0}, {3});
      tc.add_pair({0, 1, 0, 1}, {1, 0, 1});
      tc.add_pair({0, 3, 0, 3}, {3, 0, 3});
      tc.add_pair({1, 0, 1, 0}, {1, 0, 1});
      tc.add_pair({1, 3, 0, 1}, {1, 0, 1});
      tc.add_pair({1, 3, 0, 3}, {3, 0, 3});
      tc.add_pair({3, 0, 1, 0}, {3, 0, 1});
      tc.add_pair({3, 0, 3, 0}, {3, 0, 3});

      check_hlt(tc);
      check_felsch(tc);
      check_random(tc);
      check_Rc_style(tc);
      check_R_over_C_style(tc);
      check_CR_style(tc);
      check_Cr_style(tc);

      REQUIRE(tc.number_of_classes() == 16);
      REQUIRE(tc.word_to_class_index({0}) == tc.word_to_class_index({5}));
      REQUIRE(tc.word_to_class_index({0}) == tc.word_to_class_index({10}));
      REQUIRE(tc.word_to_class_index({1}) == tc.word_to_class_index({2}));
      REQUIRE(tc.word_to_class_index({1}) == tc.word_to_class_index({7}));
      REQUIRE(tc.word_to_class_index({3}) == tc.word_to_class_index({4}));
      REQUIRE(tc.word_to_class_index({3}) == tc.word_to_class_index({6}));
      REQUIRE(tc.word_to_class_index({3}) == tc.word_to_class_index({8}));
      REQUIRE(tc.word_to_class_index({3}) == tc.word_to_class_index({9}));
      tc.standardize(tc_order::shortlex);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "018",
                            "test lookahead",
                            "[todd-coxeter][quick]") {
      auto rg = ReportGuard(REPORT);
      {
        ToddCoxeter tc(twosided);
        tc.set_number_of_generators(2);
        tc.next_lookahead(10);
        tc.add_pair({0, 0, 0}, {0});
        tc.add_pair({1, 0, 0}, {1, 0});
        tc.add_pair({1, 0, 1, 1, 1}, {1, 0});
        tc.add_pair({1, 1, 1, 1, 1}, {1, 1});
        tc.add_pair({1, 1, 0, 1, 1, 0}, {1, 0, 1, 0, 1, 1});
        tc.add_pair({0, 0, 1, 0, 1, 1, 0}, {0, 1, 0, 1, 1, 0});
        tc.add_pair({0, 0, 1, 1, 0, 1, 0}, {0, 1, 1, 0, 1, 0});
        tc.add_pair({0, 1, 0, 1, 0, 1, 0}, {1, 0, 1, 0, 1, 0});
        tc.add_pair({1, 0, 1, 0, 1, 0, 1}, {1, 0, 1, 0, 1, 0});
        tc.add_pair({1, 0, 1, 0, 1, 1, 0}, {1, 0, 1, 0, 1, 1});
        tc.add_pair({1, 0, 1, 1, 0, 1, 0}, {1, 0, 1, 1, 0, 1});
        tc.add_pair({1, 1, 0, 1, 0, 1, 0}, {1, 0, 1, 0, 1, 0});
        tc.add_pair({1, 1, 1, 1, 0, 1, 0}, {1, 0, 1, 0});
        tc.add_pair({0, 0, 1, 1, 1, 0, 1, 0}, {1, 1, 1, 0, 1, 0});

        check_hlt(tc);
        REQUIRE(tc.number_of_classes() == 78);
        tc.standardize(tc_order::shortlex);
      }
      {
        ToddCoxeter tc(left);
        tc.set_number_of_generators(2);
        tc.next_lookahead(10);
        tc.add_pair({0, 0, 0}, {0});
        tc.add_pair({1, 0, 0}, {1, 0});
        tc.add_pair({1, 0, 1, 1, 1}, {1, 0});
        tc.add_pair({1, 1, 1, 1, 1}, {1, 1});
        tc.add_pair({1, 1, 0, 1, 1, 0}, {1, 0, 1, 0, 1, 1});
        tc.add_pair({0, 0, 1, 0, 1, 1, 0}, {0, 1, 0, 1, 1, 0});
        tc.add_pair({0, 0, 1, 1, 0, 1, 0}, {0, 1, 1, 0, 1, 0});
        tc.add_pair({0, 1, 0, 1, 0, 1, 0}, {1, 0, 1, 0, 1, 0});
        tc.add_pair({1, 0, 1, 0, 1, 0, 1}, {1, 0, 1, 0, 1, 0});
        tc.add_pair({1, 0, 1, 0, 1, 1, 0}, {1, 0, 1, 0, 1, 1});
        tc.add_pair({1, 0, 1, 1, 0, 1, 0}, {1, 0, 1, 1, 0, 1});
        tc.add_pair({1, 1, 0, 1, 0, 1, 0}, {1, 0, 1, 0, 1, 0});
        tc.add_pair({1, 1, 1, 1, 0, 1, 0}, {1, 0, 1, 0});
        tc.add_pair({0, 0, 1, 1, 1, 0, 1, 0}, {1, 1, 1, 0, 1, 0});

        check_hlt(tc);
        REQUIRE(tc.number_of_classes() == 78);
        tc.standardize(tc_order::shortlex);
      }
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "019",
                            "non-trivial left cong. from semigroup",
                            "[todd-coxeter][quick]") {
      auto rg = ReportGuard(REPORT);

      FroidurePin<Transf<>> S;
      S.add_generator(Transf<>({1, 3, 4, 2, 3}));
      S.add_generator(Transf<>({3, 2, 1, 3, 3}));

      REQUIRE(S.size() == 88);
      REQUIRE(S.degree() == 5);

      word_type w1, w2;
      S.factorisation(w1, S.position(Transf<>({3, 4, 4, 4, 4})));
      S.factorisation(w2, S.position(Transf<>({3, 1, 3, 3, 3})));

      ToddCoxeter tc(left, S);
      tc.froidure_pin_policy(options::froidure_pin::use_cayley_graph);
      tc.add_pair(w1, w2);
      check_hlt_no_save(tc);
      check_hlt_save_throws(tc);
      check_felsch_throws(tc);
      check_random(tc);
      REQUIRE(tc.number_of_classes() == 69);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "020",
                            "2-sided cong. on free semigroup",
                            "[todd-coxeter][quick]") {
      auto        rg = ReportGuard(REPORT);
      ToddCoxeter tc(twosided);
      tc.set_number_of_generators(1);

      check_hlt(tc);
      check_felsch(tc);
      check_random(tc);

      REQUIRE(tc.contains({0, 0}, {0, 0}));
      REQUIRE(!tc.contains({0, 0}, {0}));
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "021",
                            "calling run when obviously infinite",
                            "[todd-coxeter][quick]") {
      ToddCoxeter tc(twosided);
      tc.set_number_of_generators(5);

      check_hlt(tc);
      check_felsch(tc);
      check_random(tc);

      REQUIRE_THROWS_AS(tc.run(), LibsemigroupsException);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "022",
                            "Stellar S3",
                            "[todd-coxeter][quick][hivert]") {
      auto rg = ReportGuard(REPORT);

      ToddCoxeter tc(twosided);
      tc.set_number_of_generators(4);
      tc.add_pair({3, 3}, {3});
      tc.add_pair({0, 3}, {0});
      tc.add_pair({3, 0}, {0});
      tc.add_pair({1, 3}, {1});
      tc.add_pair({3, 1}, {1});
      tc.add_pair({2, 3}, {2});
      tc.add_pair({3, 2}, {2});
      tc.add_pair({0, 0}, {0});
      tc.add_pair({1, 1}, {1});
      tc.add_pair({2, 2}, {2});
      tc.add_pair({0, 2}, {2, 0});
      tc.add_pair({2, 0}, {0, 2});
      tc.add_pair({1, 2, 1}, {2, 1, 2});
      tc.add_pair({1, 0, 1, 0}, {0, 1, 0, 1});
      tc.add_pair({1, 0, 1, 0}, {0, 1, 0});

      check_hlt(tc);
      check_felsch(tc);
      check_random(tc);
      check_Rc_style(tc);
      check_R_over_C_style(tc);
      check_CR_style(tc);
      check_Cr_style(tc);

      REQUIRE(tc.number_of_classes() == 34);
      REQUIRE(tc.quotient_froidure_pin()->size() == 34);
      using froidure_pin_type = typename ToddCoxeter::froidure_pin_type;
      using detail::TCE;

      auto& S = static_cast<froidure_pin_type&>(*tc.quotient_froidure_pin());
      S.run();
      std::vector<TCE> v(S.cbegin(), S.cend());
      std::sort(v.begin(), v.end());
      REQUIRE(v
              == std::vector<TCE>({TCE(1),  TCE(2),  TCE(3),  TCE(4),  TCE(5),
                                   TCE(6),  TCE(7),  TCE(8),  TCE(9),  TCE(10),
                                   TCE(11), TCE(12), TCE(13), TCE(14), TCE(15),
                                   TCE(16), TCE(17), TCE(18), TCE(19), TCE(20),
                                   TCE(21), TCE(22), TCE(23), TCE(24), TCE(25),
                                   TCE(26), TCE(27), TCE(28), TCE(29), TCE(30),
                                   TCE(31), TCE(32), TCE(33), TCE(34)}));
      REQUIRE(std::vector<TCE>(S.cbegin_sorted(), S.cend_sorted())
              == std::vector<TCE>({TCE(1),  TCE(2),  TCE(3),  TCE(4),  TCE(5),
                                   TCE(6),  TCE(7),  TCE(8),  TCE(9),  TCE(10),
                                   TCE(11), TCE(12), TCE(13), TCE(14), TCE(15),
                                   TCE(16), TCE(17), TCE(18), TCE(19), TCE(20),
                                   TCE(21), TCE(22), TCE(23), TCE(24), TCE(25),
                                   TCE(26), TCE(27), TCE(28), TCE(29), TCE(30),
                                   TCE(31), TCE(32), TCE(33), TCE(34)}));
      REQUIRE(detail::to_string(TCE(1)) == "1");
      REQUIRE_NOTHROW(IncreaseDegree<TCE>()(TCE(1), 10));

      std::ostringstream oss;
      oss << TCE(10);  // Does not do anything visible

      std::stringbuf buf;
      std::ostream   os(&buf);
      os << TCE(32);  // Does not do anything visible
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "023",
                            "finite semigroup (size 5)",
                            "[todd-coxeter][quick]") {
      auto                    rg = ReportGuard(REPORT);
      congruence::ToddCoxeter tc(left);
      tc.set_number_of_generators(2);
      tc.add_pair({0, 0, 0}, {0});  // (a^3, a)
      tc.add_pair({0}, {1, 1});     // (a, b^2)

      check_hlt(tc);
      check_felsch(tc);
      check_random(tc);
      check_Rc_style(tc);
      check_R_over_C_style(tc);
      check_CR_style(tc);
      check_Cr_style(tc);

      REQUIRE(tc.number_of_classes() == 5);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "024",
                            "exceptions",
                            "[todd-coxeter][quick]") {
      auto rg = ReportGuard(REPORT);
      {
        congruence::ToddCoxeter tc1(left);
        tc1.set_number_of_generators(2);
        tc1.add_pair({0, 0, 0}, {0});
        tc1.add_pair({0}, {1, 1});
        REQUIRE(tc1.number_of_classes() == 5);

        REQUIRE_THROWS_AS(ToddCoxeter(right, tc1), LibsemigroupsException);
        REQUIRE_THROWS_AS(ToddCoxeter(twosided, tc1), LibsemigroupsException);

        ToddCoxeter tc2(left, tc1);
        REQUIRE(!tc1.contains({0}, {1}));
        tc2.add_pair({0}, {1});

        check_hlt(tc2);
        check_felsch(tc2);
        check_random(tc2);
        check_Rc_style(tc2);
        check_R_over_C_style(tc2);
        check_CR_style(tc2);
        check_Cr_style(tc2);

        REQUIRE(tc2.number_of_classes() == 1);

        ToddCoxeter tc3(left);
        tc3.set_number_of_generators(2);
        tc3.add_pair({0, 0, 0}, {0});
        tc3.add_pair({0}, {1, 1});
        tc3.add_pair({0}, {1});
        REQUIRE(tc3.number_of_classes() == 1);
      }
      {
        congruence::ToddCoxeter tc1(right);
        tc1.set_number_of_generators(2);
        tc1.add_pair({0, 0, 0}, {0});
        tc1.add_pair({0}, {1, 1});
        REQUIRE(tc1.number_of_classes() == 5);

        REQUIRE_THROWS_AS(ToddCoxeter(left, tc1), LibsemigroupsException);
        REQUIRE_THROWS_AS(ToddCoxeter(twosided, tc1), LibsemigroupsException);

        ToddCoxeter tc2(right, tc1);
        REQUIRE(!tc1.contains({0}, {1}));
        tc2.add_pair({0}, {1});

        check_hlt(tc2);
        check_felsch(tc2);
        check_random(tc2);
        check_Rc_style(tc2);
        check_R_over_C_style(tc2);
        check_CR_style(tc2);
        check_Cr_style(tc2);

        REQUIRE(tc2.number_of_classes() == 1);

        ToddCoxeter tc3(right);
        tc3.set_number_of_generators(2);
        tc3.add_pair({0, 0, 0}, {0});
        tc3.add_pair({0}, {1, 1});
        tc3.add_pair({0}, {1});
        REQUIRE(tc3.number_of_classes() == 1);
      }
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "025",
                            "obviously infinite",
                            "[todd-coxeter][quick]") {
      auto rg = ReportGuard(REPORT);
      {
        congruence::ToddCoxeter tc(left);
        tc.set_number_of_generators(3);
        tc.add_pair({0, 0, 0}, {0});
        check_hlt(tc);
        check_felsch(tc);
        check_random(tc);

        REQUIRE(tc.number_of_classes() == POSITIVE_INFINITY);
        REQUIRE(!tc.is_quotient_obviously_finite());
      }
      {
        congruence::ToddCoxeter tc(right);
        tc.set_number_of_generators(3);
        tc.add_pair({0, 0, 0}, {0});
        check_hlt(tc);
        check_felsch(tc);
        check_random(tc);

        REQUIRE(tc.number_of_classes() == POSITIVE_INFINITY);
        REQUIRE(!tc.is_quotient_obviously_finite());
      }
      {
        congruence::ToddCoxeter tc(twosided);
        tc.set_number_of_generators(3);
        tc.add_pair({0, 0, 0}, {0});
        check_hlt(tc);
        check_felsch(tc);
        check_random(tc);

        REQUIRE(tc.number_of_classes() == POSITIVE_INFINITY);
        REQUIRE(!tc.is_quotient_obviously_finite());
      }
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "026",
                            "exceptions",
                            "[todd-coxeter][quick]") {
      auto rg = ReportGuard(REPORT);
      {
        congruence::ToddCoxeter tc(right);
        tc.set_number_of_generators(2);
        tc.add_pair({0, 0, 0}, {0});
        tc.add_pair({0}, {1, 1});
        check_hlt(tc);
        check_felsch(tc);

        REQUIRE(tc.number_of_classes() == 5);
        REQUIRE(tc.class_index_to_word(0) == word_type({0}));
        // This next one should throw
        REQUIRE_THROWS_AS(tc.quotient_froidure_pin(), LibsemigroupsException);
      }
      {
        congruence::ToddCoxeter tc(twosided);
        tc.set_number_of_generators(2);
        tc.add_pair({0, 0, 0}, {0});
        tc.add_pair({0}, {1, 1});
        check_hlt(tc);
        check_felsch(tc);
        check_random(tc);
        check_Rc_style(tc);
        check_R_over_C_style(tc);
        check_CR_style(tc);
        check_Cr_style(tc);

        REQUIRE(tc.number_of_classes() == 5);
        REQUIRE(tc.class_index_to_word(0) == word_type({0}));
        REQUIRE(tc.class_index_to_word(1) == word_type({1}));
        REQUIRE(tc.class_index_to_word(2) == word_type({0, 0}));
        REQUIRE(tc.class_index_to_word(3) == word_type({0, 1}));
        REQUIRE(tc.class_index_to_word(4) == word_type({0, 0, 1}));
        REQUIRE_THROWS_AS(tc.class_index_to_word(5), LibsemigroupsException);
        REQUIRE_THROWS_AS(tc.class_index_to_word(100), LibsemigroupsException);
      }
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "027",
                            "empty",
                            "[todd-coxeter][quick]") {
      auto rg = ReportGuard(REPORT);
      {
        congruence::ToddCoxeter tc(left);
        REQUIRE(tc.empty());
        tc.set_number_of_generators(3);
        REQUIRE(tc.empty());
        tc.add_pair({0}, {2});
        REQUIRE(tc.empty());
        tc.reserve(100);
        tc.reserve(200);
        REQUIRE(tc.empty());
      }
      {
        FroidurePin<BMat8> S(
            {BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}})});

        ToddCoxeter tc(twosided, S);
        REQUIRE(tc.empty());
        tc.add_pair({0}, {0, 0});
        REQUIRE(tc.empty());
      }
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "028",
                            "congruence of fpsemigroup::ToddCoxeter",
                            "[todd-coxeter][quick]") {
      auto rg = ReportGuard(REPORT);
      {
        fpsemigroup::ToddCoxeter tc1;
        tc1.set_alphabet("ab");
        tc1.add_rule("aaa", "a");
        tc1.add_rule("a", "bb");
        REQUIRE(tc1.size() == 5);
        congruence::ToddCoxeter tc2(left, tc1);
        REQUIRE(tc2.empty());
        tc2.add_pair({0}, {1});
        REQUIRE_THROWS_AS(tc2.add_pair({0}, {2}), LibsemigroupsException);
        check_hlt_no_save(tc2);
        check_hlt_save_throws(tc2);
        check_felsch_throws(tc2);
        check_random(tc2);
        REQUIRE(tc2.number_of_classes() == 1);
      }
      {
        fpsemigroup::ToddCoxeter tc1;
        tc1.set_alphabet("ab");
        tc1.add_rule("aaa", "a");
        tc1.add_rule("a", "bb");
        congruence::ToddCoxeter tc2(left, tc1);
        tc2.add_pair({0}, {1});
        check_hlt(tc2);
        check_felsch(tc2);
        check_random(tc2);
        check_Rc_style(tc2);
        check_R_over_C_style(tc2);
        check_CR_style(tc2);
        check_Cr_style(tc2);

        REQUIRE(!tc2.empty());
        REQUIRE_THROWS_AS(tc2.add_pair({0}, {2}), LibsemigroupsException);
        REQUIRE(tc2.number_of_classes() == 1);
      }
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "029",
                            "!KnuthBendix.started()",
                            "[todd-coxeter][quick]") {
      auto                     rg = ReportGuard(REPORT);
      fpsemigroup::KnuthBendix kb;
      kb.set_alphabet("abB");

      kb.add_rule("bb", "B");
      kb.add_rule("BaB", "aba");
      REQUIRE(!kb.confluent());
      REQUIRE(!kb.started());

      std::unique_ptr<ToddCoxeter> tc = nullptr;
      SECTION("2-sided") {
        tc = std::make_unique<ToddCoxeter>(twosided, kb);
        check_hlt(*tc);
        check_felsch(*tc);
        check_random(*tc);
        // Don't use the other check_* functions because they run to avoid an
        // issue with fpsemigroup::ToddCoxeter.
      }
      SECTION("left") {
        tc = std::make_unique<ToddCoxeter>(left, kb);
        check_hlt(*tc);
        check_felsch(*tc);
        check_random(*tc);
        // Don't use the other check_* functions because they run to avoid an
        // issue with fpsemigroup::ToddCoxeter.
      }
      SECTION("right") {
        tc = std::make_unique<ToddCoxeter>(left, kb);
        check_hlt(*tc);
        check_felsch(*tc);
        check_random(*tc);
        // Don't use the other check_* functions because they run to avoid an
        // issue with fpsemigroup::ToddCoxeter.
      }
      REQUIRE(!tc->has_parent_froidure_pin());
      tc->add_pair({1}, {2});
      REQUIRE(tc->is_quotient_obviously_infinite());
      REQUIRE(tc->number_of_classes() == POSITIVE_INFINITY);
      REQUIRE(std::vector<relation_type>(tc->cbegin_generating_pairs(),
                                         tc->cend_generating_pairs())
              == std::vector<relation_type>(
                  {{{1, 1}, {2}}, {{2, 0, 2}, {0, 1, 0}}, {{1}, {2}}}));
      REQUIRE(!tc->finished());
      REQUIRE(!tc->started());
      tc->add_pair({1}, {0});
      REQUIRE(!tc->is_quotient_obviously_infinite());
      REQUIRE(tc->number_of_classes() == 1);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "030",
                            "KnuthBendix.finished()",
                            "[todd-coxeter][quick]") {
      auto                     rg = ReportGuard(REPORT);
      fpsemigroup::KnuthBendix kb;
      kb.set_alphabet("abB");

      kb.add_rule("bb", "B");
      kb.add_rule("BaB", "aba");
      REQUIRE(!kb.confluent());
      kb.run();
      REQUIRE(kb.confluent());
      REQUIRE(kb.number_of_active_rules() == 6);
      REQUIRE(kb.finished());

      std::unique_ptr<ToddCoxeter> tc = nullptr;
      SECTION("2-sided") {
        tc = std::make_unique<ToddCoxeter>(twosided, kb);
        check_hlt(*tc);
        check_felsch(*tc);
        check_random(*tc);
        // Don't use the other check_* functions because they run to avoid an
        // issue with fpsemigroup::ToddCoxeter.
      }
      SECTION("left") {
        tc = std::make_unique<ToddCoxeter>(left, kb);
        check_hlt(*tc);
        check_felsch(*tc);
        check_random(*tc);
        // Don't use the other check_* functions because they run to avoid an
        // issue with fpsemigroup::ToddCoxeter.
      }
      SECTION("right") {
        tc = std::make_unique<ToddCoxeter>(right, kb);
        check_hlt(*tc);
        check_felsch(*tc);
        check_random(*tc);
        // Don't use the other check_* functions because they run to avoid an
        // issue with fpsemigroup::ToddCoxeter.
      }
      REQUIRE(tc->has_parent_froidure_pin());
      tc->add_pair({1}, {2});
      REQUIRE(tc->is_quotient_obviously_infinite());
      REQUIRE(tc->number_of_classes() == POSITIVE_INFINITY);
      REQUIRE(std::vector<relation_type>(tc->cbegin_generating_pairs(),
                                         tc->cend_generating_pairs())
              == std::vector<relation_type>(
                  {{{1, 1}, {2}}, {{2, 0, 2}, {0, 1, 0}}, {{1}, {2}}}));
      tc->add_pair({1}, {0});
      REQUIRE(!tc->is_quotient_obviously_infinite());
      REQUIRE(tc->number_of_classes() == 1);
      if (tc->kind() == twosided) {
        REQUIRE(tc->quotient_froidure_pin()->size() == 1);
      } else {
        REQUIRE_THROWS_AS(tc->quotient_froidure_pin(), LibsemigroupsException);
      }
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "031",
                            "KnuthBendix.finished()",
                            "[todd-coxeter][quick]") {
      auto                     rg = ReportGuard(REPORT);
      fpsemigroup::KnuthBendix kb;
      kb.set_alphabet("abB");

      kb.add_rule("bb", "B");
      kb.add_rule("BaB", "aba");
      kb.add_rule("a", "b");
      kb.add_rule("b", "B");

      REQUIRE(kb.confluent());
      kb.run();
      REQUIRE(kb.confluent());
      REQUIRE(kb.number_of_active_rules() == 3);
      REQUIRE(kb.size() == 1);
      REQUIRE(kb.is_obviously_finite());
      REQUIRE(kb.finished());

      std::unique_ptr<ToddCoxeter> tc = nullptr;
      SECTION("2-sided") {
        tc = std::make_unique<ToddCoxeter>(twosided, kb);
        tc->add_pair({1}, {2});
        check_hlt(*tc);
        check_felsch(*tc);
        check_random(*tc);
        check_Rc_style(*tc);
        check_R_over_C_style(*tc);
        check_CR_style(*tc);
        check_Cr_style(*tc);
      }
      SECTION("left") {
        tc = std::make_unique<ToddCoxeter>(left, kb);
        tc->add_pair({1}, {2});
        check_hlt(*tc);
        check_felsch(*tc);
        check_random(*tc);
        check_Rc_style(*tc);
        check_R_over_C_style(*tc);
        check_CR_style(*tc);
        check_Cr_style(*tc);
      }
      SECTION("right") {
        tc = std::make_unique<ToddCoxeter>(left, kb);
        tc->add_pair({1}, {2});
        check_hlt(*tc);
        check_felsch(*tc);
        check_random(*tc);
        check_Rc_style(*tc);
        check_R_over_C_style(*tc);
        check_CR_style(*tc);
        check_Cr_style(*tc);
      }
      REQUIRE(tc->has_parent_froidure_pin());

      REQUIRE(tc->number_of_classes() == 1);
      if (tc->kind() == twosided) {
        REQUIRE(tc->quotient_froidure_pin()->size() == 1);
      } else {
        REQUIRE_THROWS_AS(tc->quotient_froidure_pin(), LibsemigroupsException);
      }
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "032",
                            "prefill",
                            "[todd-coxeter][quick]") {
      auto rg = ReportGuard(REPORT);
      detail::DynamicArray2<ToddCoxeter::class_index_type> rv(2, 1);
      REQUIRE(rv.number_of_cols() == 2);
      REQUIRE(rv.number_of_rows() == 1);
      {
        ToddCoxeter tc(twosided);
        // prefill before number_of_generators are set
        REQUIRE_THROWS_AS(tc.prefill(rv), LibsemigroupsException);
        tc.set_number_of_generators(3);
        // prefill where number_of_generators != number_of_cols of rv
        REQUIRE_THROWS_AS(tc.prefill(rv), LibsemigroupsException);
      }
      {
        ToddCoxeter tc(left);
        tc.set_number_of_generators(2);
        rv.set(0, 0, 0);
        rv.set(0, 1, 1);
        // prefill with too few rows
        REQUIRE_THROWS_AS(tc.prefill(rv), LibsemigroupsException);
        rv.add_rows(1);
        REQUIRE(rv.number_of_rows() == 2);
        rv.set(1, 0, UNDEFINED);
        rv.set(1, 1, UNDEFINED);
        // prefill with bad value (0, 0)
        REQUIRE_THROWS_AS(tc.prefill(rv), LibsemigroupsException);
        rv.set(0, 0, 2);
        // prefill with bad value (0, 0)
        REQUIRE_THROWS_AS(tc.prefill(rv), LibsemigroupsException);
        rv.set(0, 0, 1);
        // UNDEFINED is not allowed
        REQUIRE_THROWS_AS(tc.prefill(rv), LibsemigroupsException);
        rv.set(1, 0, 1);
        rv.set(1, 1, 1);
        tc.prefill(rv);
      }
      {
        detail::DynamicArray2<ToddCoxeter::class_index_type> rv2(2, 0);
        ToddCoxeter                                          tc(twosided);
        tc.set_number_of_generators(2);
        REQUIRE_THROWS_AS(tc.prefill(rv2), LibsemigroupsException);
      }
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "033",
                            "congruence of ToddCoxeter",
                            "[todd-coxeter][quick]") {
      auto        rg = ReportGuard(REPORT);
      ToddCoxeter tc1(twosided);
      tc1.set_number_of_generators(2);
      tc1.add_pair({0, 0, 0}, {0});
      tc1.add_pair({0}, {1, 1});
      REQUIRE(tc1.number_of_classes() == 5);
      ToddCoxeter tc2(left, tc1);
      tc2.next_lookahead(1);
      tc2.report_every(1);
      REQUIRE(!tc2.empty());
      check_hlt(tc2);
      check_random(tc2);
      tc2.add_pair({0}, {0, 0});
      REQUIRE(tc2.number_of_classes() == 3);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "034",
                            "congruence of ToddCoxeter",
                            "[todd-coxeter][quick]") {
      auto rg      = ReportGuard(REPORT);
      using Transf = LeastTransf<5>;
      FroidurePin<Transf> S({Transf({1, 3, 4, 2, 3}), Transf({3, 2, 1, 3, 3})});
      REQUIRE(S.size() == 88);
      REQUIRE(S.number_of_rules() == 18);
      ToddCoxeter tc(twosided, S);
      tc.froidure_pin_policy(options::froidure_pin::none);
      tc.set_number_of_generators(2);
      check_hlt_no_save(tc);
      check_hlt_save_throws(tc);
      check_felsch_throws(tc);
      check_random(tc);
      tc.add_pair({0}, {1, 1});
      REQUIRE(tc.number_of_classes() == 1);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "035",
                            "congruence on FpSemigroup",
                            "[todd-coxeter][quick]") {
      auto        rg = ReportGuard(REPORT);
      FpSemigroup S;
      S.set_alphabet("abe");
      S.set_identity("e");
      S.add_rule("abb", "bb");
      S.add_rule("bbb", "bb");
      S.add_rule("aaaa", "a");
      S.add_rule("baab", "bb");
      S.add_rule("baaab", "b");
      S.add_rule("babab", "b");
      S.add_rule("bbaaa", "bb");
      S.add_rule("bbaba", "bbaa");

      REQUIRE(S.knuth_bendix()->confluent());
      REQUIRE(S.knuth_bendix()->number_of_rules() == 13);

      ToddCoxeter tc(left, *S.knuth_bendix());
      tc.add_pair({0}, {1, 1, 1});
      check_hlt(tc);
      check_felsch(tc);
      check_random(tc);
      check_Rc_style(tc);
      check_R_over_C_style(tc);
      check_CR_style(tc);
      check_Cr_style(tc);

      REQUIRE(tc.number_of_classes() == 2);
      REQUIRE(std::vector<word_type>(tc.cbegin_normal_forms(),
                                     tc.cend_normal_forms())
              == std::vector<word_type>({{0}, {2}}));
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "036",
                            "exceptions",
                            "[todd-coxeter][quick]") {
      auto rg      = ReportGuard(REPORT);
      using Transf = LeastTransf<5>;
      FroidurePin<Transf> S({Transf({1, 3, 4, 2, 3}), Transf({3, 2, 1, 3, 3})});
      ToddCoxeter         tc(twosided);
      tc.set_number_of_generators(2);
      tc.add_pair({0}, {1});
      tc.add_pair({0, 0}, {0});
      REQUIRE(tc.number_of_classes() == 1);
      REQUIRE_THROWS_AS(tc.prefill(S.right_cayley_graph()),
                        LibsemigroupsException);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "037",
                            "copy constructor",
                            "[todd-coxeter][quick]") {
      auto        rg = ReportGuard(REPORT);
      ToddCoxeter tc(twosided);
      tc.set_number_of_generators(2);
      tc.add_pair({0}, {1});
      tc.add_pair({0, 0}, {0});
      tc.strategy(options::strategy::felsch);
      REQUIRE(tc.strategy() == options::strategy::felsch);
      REQUIRE(!tc.complete());
      REQUIRE(tc.compatible());
      REQUIRE(tc.number_of_classes() == 1);
      REQUIRE(std::vector<word_type>(tc.cbegin_normal_forms(),
                                     tc.cend_normal_forms())
              == std::vector<word_type>(1, {0}));
      REQUIRE(tc.complete());
      REQUIRE(tc.compatible());

      ToddCoxeter copy(tc);
      REQUIRE(copy.number_of_generators() == 2);
      REQUIRE(copy.number_of_generating_pairs() == 2);
      REQUIRE(copy.finished());
      REQUIRE(copy.number_of_classes() == 1);
      REQUIRE(copy.froidure_pin_policy() == options::froidure_pin::none);
      REQUIRE(copy.complete());
      REQUIRE(copy.compatible());
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "038",
                            "simplify",
                            "[todd-coxeter][quick]") {
      auto        rg = ReportGuard(REPORT);
      ToddCoxeter tc(twosided);
      tc.set_number_of_generators(2);
      tc.add_pair({0, 0}, {1});
      tc.add_pair({0, 0}, {0});
      tc.add_pair({0, 1, 0}, {0, 0});
      tc.add_pair({0, 1, 0, 1}, {0, 1, 0});

      tc.simplify();
      REQUIRE(tc.number_of_generating_pairs() == 4);
      REQUIRE(tc.number_of_classes() == 1);
      std::vector<word_type> result(tc.cbegin_relations(), tc.cend_relations());
      std::sort(result.begin(), result.end());
      REQUIRE(result
              == std::vector<word_type>(
                  {{0}, {0}, {0}, {0}, {0, 0}, {0, 1, 0}, {0, 1, 0, 1}, {1}}));

      ToddCoxeter tc2(right, tc);
      tc2.add_pair({0, 0}, {1});
      tc2.add_pair({0, 0}, {0});
      tc2.add_pair({0, 1, 0}, {0, 0});
      tc2.add_pair({0, 1, 0, 1}, {0, 1, 0});
      REQUIRE(tc2.felsch_tree_height() == 4);

      REQUIRE(std::equal(tc.cbegin_relations(),
                         tc.cend_relations(),
                         tc2.cbegin_relations(),
                         tc2.cend_relations()));
      REQUIRE(std::vector<word_type>(tc2.cbegin_extra(), tc2.cend_extra())
              == std::vector<word_type>({{0, 0},
                                         {1},
                                         {0, 0},
                                         {0},
                                         {0, 1, 0},
                                         {0, 0},
                                         {0, 1, 0, 1},
                                         {0, 1, 0}}));
      tc2.simplify();
      REQUIRE(
          std::vector<word_type>(tc2.cbegin_extra(), tc2.cend_extra()).empty());
      REQUIRE(tc2.felsch_tree_height() == 4);
      REQUIRE(tc2.number_of_classes() == 1);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "039",
                            "Stylic monoid",
                            "[todd-coxeter][quick][no-coverage][no-valgrind]") {
      auto        rg = ReportGuard(REPORT);
      ToddCoxeter tc(twosided);
      tc.set_number_of_generators(9);
      for (auto const& w : Stylic(9)) {
        tc.add_pair(w.first, w.second);
      }
      tc.strategy(options::strategy::random);
      REQUIRE_THROWS_AS(tc.run_for(std::chrono::milliseconds(100)),
                        LibsemigroupsException);
      tc.remove_duplicate_generating_pairs()
          .sort_generating_pairs()
          .strategy(options::strategy::hlt)
          .lookahead(options::lookahead::partial | options::lookahead::hlt);
      REQUIRE(tc.number_of_classes() == 115'974);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "040",
                            "Fibonacci(4, 6)",
                            "[todd-coxeter][fail]") {
      auto        rg = ReportGuard();
      ToddCoxeter tc(twosided);
      tc.set_number_of_generators(6);
      for (auto const& w : Fibonacci(4, 6)) {
        tc.add_pair(w.first, w.second);
      }
      tc.strategy(options::strategy::felsch);
      REQUIRE(tc.number_of_classes() == 0);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "041",
                            "some finite classes",
                            "[todd-coxeter][quick]") {
      auto        rg = ReportGuard(REPORT);
      ToddCoxeter tc(twosided);
      tc.set_number_of_generators(1);
      REQUIRE(tc.number_of_classes() == POSITIVE_INFINITY);

      tc.add_pair({0, 0, 0, 0, 0, 0}, {0, 0, 0, 0});
      tc.add_pair({0, 0, 0, 0, 0, 0}, {0, 0, 0, 0});
      tc.add_pair({0, 0, 0, 0, 0, 0}, {0, 0, 0, 0});
      tc.strategy(options::strategy::random)
          .remove_duplicate_generating_pairs()
          .standardize(true);
      REQUIRE(!tc.compatible());
      REQUIRE_THROWS_AS(tc.run_for(std::chrono::microseconds(1)),
                        LibsemigroupsException);
      tc.strategy(options::strategy::CR);
      size_t x = 0;
      REQUIRE_THROWS_AS(tc.run_until([&x] { return x > 4; }),
                        LibsemigroupsException);
      tc.lower_bound(100)
          .use_relations_in_extra(true)
          .deduction_policy(options::deductions::v1
                            | options::deductions::unlimited)
          .restandardize(true)
          .max_preferred_defs(0);
      REQUIRE_THROWS_AS(tc.hlt_defs(0), LibsemigroupsException);
      REQUIRE_THROWS_AS(tc.f_defs(0), LibsemigroupsException);
      tc.hlt_defs(10)
          .f_defs(10)
          .lookahead_growth_factor(3.0)
          .lookahead_growth_threshold(100'000)
          .large_collapse(1);
      REQUIRE_THROWS_AS(tc.lookahead_growth_factor(0.1),
                        LibsemigroupsException);

      REQUIRE(tc.random_interval() == std::chrono::milliseconds(200));
      REQUIRE(tc.felsch_tree_height() == 6);
      REQUIRE(tc.number_of_classes() == 5);
      REQUIRE(tc.number_of_words(0) == 1);
      REQUIRE(tc.number_of_words(1) == 1);
      REQUIRE(tc.number_of_words(2) == 1);
      REQUIRE(tc.number_of_words(3) == POSITIVE_INFINITY);
      REQUIRE(tc.number_of_words(4) == POSITIVE_INFINITY);
      REQUIRE(tc.standardization_order() == ToddCoxeter::order::none);
      REQUIRE(tc.felsch_tree_number_of_nodes() == 7);
      REQUIRE_THROWS_AS(tc.remove_duplicate_generating_pairs(),
                        LibsemigroupsException);
      ToddCoxeter tc2(left, tc);
      tc2.add_pair({0, 0}, {0});
      tc2.add_pair({0, 0}, {0});
      tc2.remove_duplicate_generating_pairs();
      // Uses CongruenceInterface's generating pairs
      REQUIRE(tc2.number_of_generating_pairs() == 2);
      ToddCoxeter tc3(twosided);
      tc3.set_number_of_generators(1);
      REQUIRE(tc3.is_non_trivial() == tril::TRUE);
      tc3.add_pair({0, 0}, {0});
      REQUIRE(tc3.is_non_trivial() == tril::unknown);
      REQUIRE(tc3.number_of_classes() == 1);
      REQUIRE(tc3.is_non_trivial() == tril::FALSE);
      REQUIRE(!tc.settings_string().empty());
      REQUIRE(!tc3.settings_string().empty());
      REQUIRE(!tc.stats_string().empty());
    }

    // Takes about 1m7s
    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "042",
                            "SymmetricGroup1",
                            "[todd-coxeter][extreme]") {
      auto        rg = ReportGuard(true);
      ToddCoxeter tc(twosided);
      tc.set_number_of_generators(4);
      for (auto const& w : SymmetricGroup1(10)) {
        tc.add_pair(w.first, w.second);
      }
      REQUIRE(tc.number_of_classes() == 3'628'800);
      std::cout << tc.stats_string();
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "043",
                            "SymmetricGroup2",
                            "[todd-coxeter][quick][no-valgrind]") {
      auto        rg = ReportGuard(REPORT);
      ToddCoxeter tc(twosided);
      tc.set_number_of_generators(7);
      for (auto const& w : SymmetricGroup2(7)) {
        tc.add_pair(w.first, w.second);
      }
      tc.run_for(std::chrono::microseconds(1));
      REQUIRE(tc.is_non_trivial() == tril::TRUE);
      REQUIRE(!tc.finished());
      tc.standardize(ToddCoxeter::order::shortlex);
      tc.standardize(ToddCoxeter::order::none);
      tc.strategy(options::strategy::CR).f_defs(100);
      REQUIRE(tc.number_of_classes() == 5'040);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "044",
                            "Option exceptions",
                            "[todd-coxeter][quick]") {
      auto rg = ReportGuard(REPORT);
      REQUIRE_THROWS_AS(options::deductions::unlimited
                            | options::deductions::unlimited,
                        LibsemigroupsException);
      REQUIRE_THROWS_AS(options::deductions::v1 | options::deductions::v2,
                        LibsemigroupsException);
      REQUIRE_THROWS_AS(options::lookahead::hlt | options::lookahead::hlt,
                        LibsemigroupsException);
      REQUIRE_THROWS_AS(options::lookahead::hlt | options::lookahead::felsch,
                        LibsemigroupsException);
      REQUIRE_THROWS_AS(options::lookahead::full | options::lookahead::partial,
                        LibsemigroupsException);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "045",
                            "Options operator<<",
                            "[todd-coxeter][quick]") {
      auto rg = ReportGuard(REPORT);
      {
        ToddCoxeter tc(twosided);
        tc.strategy(options::strategy::hlt);
        tc.settings_string();
        tc.strategy(options::strategy::felsch);
        tc.settings_string();
        tc.strategy(options::strategy::random);
        tc.settings_string();
        tc.strategy(options::strategy::CR);
        tc.settings_string();
        tc.strategy(options::strategy::R_over_C);
        tc.settings_string();
        tc.strategy(options::strategy::Cr);
        tc.settings_string();
        tc.strategy(options::strategy::Rc);
        tc.settings_string();
      }
      {
        ToddCoxeter tc(twosided);
        tc.lookahead(options::lookahead::full | options::lookahead::felsch);
        tc.settings_string();
        tc.lookahead(options::lookahead::full | options::lookahead::hlt);
        tc.settings_string();
        tc.lookahead(options::lookahead::partial | options::lookahead::felsch);
        tc.settings_string();
        tc.lookahead(options::lookahead::partial | options::lookahead::hlt);
        tc.settings_string();
      }
      {
        ToddCoxeter tc(twosided);
        tc.deduction_policy(options::deductions::v1
                            | options::deductions::no_stack_if_no_space);
        tc.settings_string();
        tc.deduction_policy(options::deductions::v1
                            | options::deductions::purge_all);
        tc.settings_string();
        tc.deduction_policy(options::deductions::v1
                            | options::deductions::purge_from_top);
        tc.settings_string();
        tc.deduction_policy(options::deductions::v1
                            | options::deductions::discard_all_if_no_space);
        tc.settings_string();
        tc.deduction_policy(options::deductions::v1
                            | options::deductions::unlimited);
        tc.settings_string();
        tc.deduction_policy(options::deductions::v2
                            | options::deductions::no_stack_if_no_space);
        tc.settings_string();
        tc.deduction_policy(options::deductions::v2
                            | options::deductions::purge_all);
        tc.settings_string();
        tc.deduction_policy(options::deductions::v2
                            | options::deductions::purge_from_top);
        tc.deduction_policy(options::deductions::v2
                            | options::deductions::discard_all_if_no_space);
        tc.settings_string();
        tc.deduction_policy(options::deductions::v2
                            | options::deductions::unlimited);
        tc.settings_string();
      }
      {
        ToddCoxeter tc(twosided);
        tc.froidure_pin_policy(options::froidure_pin::none);
        tc.settings_string();
        tc.froidure_pin_policy(options::froidure_pin::use_cayley_graph);
        tc.settings_string();
        tc.froidure_pin_policy(options::froidure_pin::use_relations);
        tc.settings_string();
      }
      {
        ToddCoxeter tc(twosided);
        tc.preferred_defs(options::preferred_defs::none);
        tc.settings_string();
        tc.preferred_defs(options::preferred_defs::immediate_no_stack);
        tc.settings_string();
        tc.preferred_defs(options::preferred_defs::immediate_yes_stack);
        tc.settings_string();
        tc.preferred_defs(options::preferred_defs::deferred);
        tc.settings_string();
      }
    }

    // Takes about 9m3s (2021 - MacBook Air M1 - 8GB RAM)
    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "046",
                            "Easdown-East-FitzGerald DualSymInv(5)",
                            "[todd-coxeter][quick][no-valgrind][no-coverage]") {
      auto        rg = ReportGuard(REPORT);
      auto const  n  = 5;
      ToddCoxeter tc(twosided);
      setup(tc, n + 1, DualSymmetricInverseMonoidEEF, n);
      // tc.strategy(options::strategy::Rc)
      //     .max_deductions(10'000)
      //     .max_preferred_defs(512)
      //     .random_interval(std::chrono::seconds(10));
      check_hlt(tc);
      check_hlt(tc);
      check_felsch(tc);
      check_random(tc);
      check_R_over_C_style(tc);
      check_Rc_style(tc);

      REQUIRE(tc.number_of_classes() == 6'721);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "047",
                            "FitzGerald UniformBlockBijection(3)",
                            "[todd-coxeter][quick]") {
      // 16, 131, 1496, 22482, 426833, 9934563, 9934563
      auto        rg = ReportGuard(REPORT);
      auto const  n  = 3;
      ToddCoxeter tc(twosided);
      setup(tc, n + 1, UniformBlockBijectionMonoidF, n);

      check_hlt(tc);
      check_felsch(tc);
      check_random(tc);
      check_R_over_C_style(tc);
      check_Rc_style(tc);
      REQUIRE(tc.number_of_classes() == 16);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "048",
                            "Stellar(7) (Gay-Hivert)",
                            "[todd-coxeter][quick][no-valgrind][no-coverage]") {
      auto         rg = ReportGuard(false);
      size_t const n  = 7;
      ToddCoxeter  tc1(congruence_kind::twosided);
      setup(tc1, n + 1, RookMonoid, n, 0);
      ToddCoxeter tc2(congruence_kind::twosided, tc1);
      setup(tc2, n + 1, Stell, n);
      tc2.strategy(options::strategy::felsch);
      REQUIRE(tc2.number_of_classes() == 13'700);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "049",
                            "PartitionMonoid(4) (East)",
                            "[todd-coxeter][quick][no-valgrind][no-coverage]") {
      auto         rg = ReportGuard(REPORT);
      size_t const n  = 4;
      ToddCoxeter  tc(congruence_kind::twosided);
      setup(tc, 5, PartitionMonoidEast41, n);
      check_hlt(tc);
      check_felsch(tc);
      check_random(tc);
      check_R_over_C_style(tc);
      check_Rc_style(tc);
      REQUIRE(tc.number_of_classes() == 4'140);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "050",
                            "SingularBrauer(6) (Maltcev + Mazorchuk)",
                            "[todd-coxeter][quick][no-valgrind][no-coverage]") {
      auto         rg = ReportGuard(REPORT);
      size_t const n  = 6;
      ToddCoxeter  tc(congruence_kind::twosided);
      setup(tc, n * n - n, SingularBrauer, n);
      tc.sort_generating_pairs().remove_duplicate_generating_pairs();
      REQUIRE(tc.number_of_classes() == 9'675);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "051",
                            "OrientationPreserving(6) (Ruskuc + Arthur)",
                            "[todd-coxeter][quick][no-valgrind][no-coverage]") {
      auto         rg = ReportGuard(REPORT);
      size_t const n  = 6;
      ToddCoxeter  tc(congruence_kind::twosided);
      setup(tc, 3, OrientationPreserving, n);
      check_hlt(tc);
      check_felsch(tc);
      check_random(tc);
      check_R_over_C_style(tc);
      check_Rc_style(tc);

      REQUIRE(tc.number_of_classes() == 2'742);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "052",
                            "OrientationReversing(5) (Ruskuc + Arthur)",
                            "[todd-coxeter][quick][no-valgrind][no-coverage]") {
      auto         rg = ReportGuard(REPORT);
      size_t const n  = 5;
      ToddCoxeter  tc(congruence_kind::twosided);
      setup(tc, 4, OrientationReversing, n);
      check_hlt(tc);
      check_felsch(tc);
      check_random(tc);
      check_Rc_style(tc);
      check_R_over_C_style(tc);
      check_CR_style(tc);
      check_Cr_style(tc);

      REQUIRE(tc.number_of_classes() == 1'015);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "053",
                            "TemperleyLieb(10) (East)",
                            "[todd-coxeter][quick][no-valgrind][no-coverage]") {
      auto         rg = ReportGuard(REPORT);
      size_t const n  = 10;
      ToddCoxeter  tc(congruence_kind::twosided);
      setup(tc, n - 1, TemperleyLieb, n);
      REQUIRE(tc.number_of_classes() == 16'795);
    }

    LIBSEMIGROUPS_TEST_CASE(
        "ToddCoxeter",
        "054",
        "Generate GAP benchmarks for Stellar(n) (Gay-Hivert)",
        "[todd-coxeter][fail]") {
      auto rg = ReportGuard(false);
      for (size_t n = 3; n <= 9; ++n) {
        ToddCoxeter tc1(congruence_kind::twosided);
        setup(tc1, n + 1, RookMonoid, n, 0);
        ToddCoxeter tc2(congruence_kind::twosided, tc1);
        setup(tc2, n + 1, Stell, n);
        output_gap_benchmark_file("stellar-" + std::to_string(n) + ".g", tc2);
      }
    }

    LIBSEMIGROUPS_TEST_CASE(
        "ToddCoxeter",
        "055",
        "Generate GAP benchmarks for PartitionMonoid(n) (East)",
        "[todd-coxeter][fail]") {
      auto rg = ReportGuard(false);
      for (size_t n = 4; n <= 6; ++n) {
        ToddCoxeter tc(congruence_kind::twosided);
        setup(tc, 5, PartitionMonoidEast41, n);
        tc.save(true);
        output_gap_benchmark_file("partition-" + std::to_string(n) + ".g", tc);
      }
    }

    LIBSEMIGROUPS_TEST_CASE(
        "ToddCoxeter",
        "056",
        "Generate GAP benchmarks for dual symmetric inverse monoid (East)",
        "[todd-coxeter][fail]") {
      auto rg = ReportGuard(false);
      for (size_t n = 3; n <= 6; ++n) {
        ToddCoxeter tc(congruence_kind::twosided);
        setup(tc, n + 1, DualSymmetricInverseMonoidEEF, n);
        output_gap_benchmark_file("dual-sym-inv-" + std::to_string(n) + ".g",
                                  tc);
      }
    }

    LIBSEMIGROUPS_TEST_CASE(
        "ToddCoxeter",
        "057",
        "Generate GAP benchmarks for UniformBlockBijectionMonoidF",
        "[todd-coxeter][fail]") {
      auto rg = ReportGuard(false);
      for (size_t n = 3; n <= 7; ++n) {
        ToddCoxeter tc(congruence_kind::twosided);
        setup(tc, n + 1, UniformBlockBijectionMonoidF, n);
        output_gap_benchmark_file(
            "uniform-block-bijection-" + std::to_string(n) + ".g", tc);
      }
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "058",
                            "Generate GAP benchmarks for stylic monoids",
                            "[todd-coxeter][fail]") {
      auto rg = ReportGuard(false);
      for (size_t n = 3; n <= 9; ++n) {
        ToddCoxeter tc(congruence_kind::twosided);
        setup(tc, n, Stylic, n);
        output_gap_benchmark_file("stylic-" + std::to_string(n) + ".g", tc);
      }
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "059",
                            "Generate GAP benchmarks for OP_n",
                            "[todd-coxeter][fail]") {
      auto rg = ReportGuard(false);
      for (size_t n = 3; n <= 9; ++n) {
        ToddCoxeter tc(congruence_kind::twosided);
        setup(tc, 3, OrientationPreserving, n);
        output_gap_benchmark_file("orient-" + std::to_string(n) + ".g", tc);
      }
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "060",
                            "Generate GAP benchmarks for OR_n",
                            "[todd-coxeter][fail]") {
      auto rg = ReportGuard(false);
      for (size_t n = 3; n <= 8; ++n) {
        ToddCoxeter tc(congruence_kind::twosided);
        setup(tc, 4, OrientationReversing, n);
        output_gap_benchmark_file("orient-reverse-" + std::to_string(n) + ".g",
                                  tc);
      }
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "061",
                            "Generate GAP benchmarks for TemperleyLieb(n)",
                            "[todd-coxeter][fail]") {
      auto rg = ReportGuard(false);
      for (size_t n = 3; n <= 13; ++n) {
        ToddCoxeter tc(congruence_kind::twosided);
        setup(tc, n - 1, TemperleyLieb, n);
        output_gap_benchmark_file("temperley-lieb-" + std::to_string(n) + ".g",
                                  tc);
      }
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "062",
                            "Generate GAP benchmarks for SingularBrauer(n)",
                            "[todd-coxeter][fail]") {
      auto rg = ReportGuard(false);
      for (size_t n = 3; n <= 7; ++n) {
        ToddCoxeter tc(congruence_kind::twosided);
        setup(tc, n * n - n, SingularBrauer, n);
        output_gap_benchmark_file("singular-brauer-" + std::to_string(n) + ".g",
                                  tc);
      }
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "111",
                            "PartitionMonoid(2)",
                            "[todd-coxeter][quick]") {
      ToddCoxeter p(congruence_kind::twosided);
      p.set_number_of_generators(4);
      p.add_pair({0, 1}, {1});
      p.add_pair({1, 0}, {1});
      p.add_pair({0, 2}, {2});
      p.add_pair({2, 0}, {2});
      p.add_pair({0, 3}, {3});
      p.add_pair({3, 0}, {3});
      p.add_pair({1, 1}, {0});
      p.add_pair({1, 3}, {3});
      p.add_pair({2, 2}, {2});
      p.add_pair({3, 1}, {3});
      p.add_pair({3, 3}, {3});
      p.add_pair({2, 3, 2}, {2});
      p.add_pair({3, 2, 3}, {3});
      p.add_pair({1, 2, 1, 2}, {2, 1, 2});
      p.add_pair({2, 1, 2, 1}, {2, 1, 2});
      auto rg = ReportGuard(false);
      REQUIRE(p.number_of_classes() == 15);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "112",
                            "Brauer(4) (Kudryavtseva + Mazorchuk)",
                            "[todd-coxeter][quick][no-valgrind][no-coverage]") {
      auto         rg = ReportGuard(REPORT);
      size_t const n  = 4;
      ToddCoxeter  tc(congruence_kind::twosided);
      setup(tc, 2 * n - 1, Brauer, n);
      tc.sort_generating_pairs().remove_duplicate_generating_pairs();
      REQUIRE(tc.number_of_classes() == 105);
    }

  }  // namespace congruence

  namespace fpsemigroup {

    constexpr bool REPORT = false;

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "063",
                            "add_rule",
                            "[todd-coxeter][quick]") {
      auto rg = ReportGuard(REPORT);
      {
        ToddCoxeter tc;
        tc.set_alphabet("ab");
        tc.add_rule("aaa", "a");
        tc.add_rule("a", "bb");
        check_hlt(tc);
        check_felsch(tc);
        check_random(tc);
        check_Rc_style(tc);
        check_R_over_C_style(tc);
        check_CR_style(tc);
        check_Cr_style(tc);
        SECTION("R/C + Felsch lookahead") {
          tc.congruence()
              .strategy(options::strategy::R_over_C)
              .lookahead(options::lookahead::felsch | options::lookahead::full);
          tc.congruence().run();
        }
        SECTION("HLT + Felsch lookahead + save") {
          tc.congruence()
              .strategy(options::strategy::hlt)
              .save(true)
              .lookahead(options::lookahead::felsch | options::lookahead::full)
              .next_lookahead(2);
          tc.congruence().run();
        }
        SECTION("Cr + small number of f_defs") {
          tc.congruence().strategy(options::strategy::Cr).f_defs(3);
          tc.congruence().run();
        }
        SECTION("Rc + small number of deductions") {
          tc.congruence().strategy(options::strategy::Rc).max_deductions(0);
          tc.congruence().run();
        }
        SECTION("Felsch + v2 + no preferred defs") {
          tc.congruence()
              .strategy(options::strategy::felsch)
              .deduction_policy(options::deductions::v2
                                | options::deductions::purge_all)
              .preferred_defs(options::preferred_defs::none);
        }
        SECTION("Felsch + v2 + immediate no stack") {
          tc.congruence()
              .strategy(options::strategy::felsch)
              .deduction_policy(options::deductions::v2
                                | options::deductions::purge_from_top)
              .preferred_defs(options::preferred_defs::immediate_no_stack);
        }
        SECTION("Felsch + v1 + immediate no stack") {
          tc.congruence()
              .strategy(options::strategy::felsch)
              .deduction_policy(options::deductions::v1
                                | options::deductions::discard_all_if_no_space)
              .preferred_defs(options::preferred_defs::immediate_no_stack);
        }
        SECTION("Felsch + v1 + immediate yes stack") {
          tc.congruence()
              .strategy(options::strategy::felsch)
              .deduction_policy(options::deductions::v1
                                | options::deductions::no_stack_if_no_space)
              .preferred_defs(options::preferred_defs::immediate_yes_stack);
        }
        SECTION("large collapse") {
          tc.congruence().large_collapse(0);
        }

        REQUIRE(tc.size() == 5);
      }
      {
        ToddCoxeter tc;
        tc.set_alphabet("ab");
        tc.add_rule("aaa", "a");
        tc.add_rule("a", "bb");
        tc.congruence().next_lookahead(1);
        check_hlt(tc);
        check_felsch(tc);
        check_random(tc);
        check_Rc_style(tc);
        check_R_over_C_style(tc);
        check_CR_style(tc);
        check_Cr_style(tc);

        REQUIRE(tc.size() == 5);
      }
    }

    // KnuthBendix methods fail for this one
    LIBSEMIGROUPS_TEST_CASE(
        "ToddCoxeter",
        "064",
        "(from kbmag/standalone/kb_data/s4) (KnuthBendix 49)",
        "[todd-coxeter][quick][kbmag]") {
      auto rg = ReportGuard(REPORT);

      ToddCoxeter tc;
      tc.set_alphabet("abcd");
      tc.add_rule("bb", "c");
      tc.add_rule("caca", "abab");
      tc.add_rule("bc", "d");
      tc.add_rule("cb", "d");
      tc.add_rule("aa", "d");
      tc.add_rule("ad", "a");
      tc.add_rule("da", "a");
      tc.add_rule("bd", "b");
      tc.add_rule("db", "b");
      tc.add_rule("cd", "c");
      tc.add_rule("dc", "c");
      check_hlt(tc);
      check_felsch(tc);
      check_random(tc);
      check_Rc_style(tc);
      check_R_over_C_style(tc);
      check_CR_style(tc);
      check_Cr_style(tc);

      REQUIRE(tc.size() == 24);
      REQUIRE(tc.froidure_pin()->size() == 24);
      REQUIRE(tc.normal_form("aaaaaaaaaaaaaaaaaaa") == "a");
      REQUIRE(KnuthBendix(tc.froidure_pin()).confluent());
    }

    // Second of BHN's series of increasingly complicated presentations
    // of 1. Doesn't terminate
    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "065",
                            "(from kbmag/standalone/kb_data/degen4b) "
                            "(KnuthBendix 065)",
                            "[fail][todd-coxeter][kbmag][shortlex]") {
      auto rg = ReportGuard();

      ToddCoxeter tc;

      tc.set_alphabet("abcdefg");
      tc.set_identity("g");
      tc.set_inverses("defabcg");

      tc.add_rule("bbdeaecbffdbaeeccefbccefb", "g");
      tc.add_rule("ccefbfacddecbffaafdcaafdc", "g");
      tc.add_rule("aafdcdbaeefacddbbdeabbdea", "g");
      tc.congruence().lookahead(options::lookahead::full
                                | options::lookahead::felsch);
      REQUIRE(!tc.is_obviously_infinite());

      REQUIRE(tc.size() == 1);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "066",
                            "test validate",
                            "[todd-coxeter][quick]") {
      auto rg = ReportGuard(REPORT);

      ToddCoxeter tc;
      tc.set_alphabet("ab");
      tc.add_rule("a", "b");
      tc.add_rule("bb", "b");

      REQUIRE_THROWS_AS(tc.add_rule("b", "c"), LibsemigroupsException);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "067",
                            "add_rules after construct. from semigroup",
                            "[todd-coxeter][quick]") {
      auto rg = ReportGuard(REPORT);

      using Transf = LeastTransf<5>;

      FroidurePin<Transf> S({Transf({1, 3, 4, 2, 3}), Transf({3, 2, 1, 3, 3})});
      REQUIRE(S.size() == 88);
      REQUIRE(S.number_of_rules() == 18);

      word_type w1, w2, w3, w4;
      S.factorisation(w1, S.position(Transf({3, 4, 4, 4, 4})));
      S.factorisation(w2, S.position(Transf({3, 1, 3, 3, 3})));
      S.factorisation(w3, S.position(Transf({1, 3, 1, 3, 3})));
      S.factorisation(w4, S.position(Transf({4, 2, 4, 4, 2})));

      ToddCoxeter tc1(S);
      tc1.add_rule(w1, w2);

      check_hlt_no_save(tc1);
      check_hlt_save_throws(tc1);
      check_felsch_throws(tc1);
      check_random(tc1);

      REQUIRE(tc1.size() == 21);
      REQUIRE(tc1.size() == tc1.froidure_pin()->size());
      REQUIRE(tc1.equal_to(w3, w4));
      REQUIRE(tc1.normal_form(w3) == tc1.normal_form(w4));

      ToddCoxeter tc2(S);
      tc2.add_rule(w1, w2);

      check_hlt_no_save(tc2);
      check_hlt_save_throws(tc2);
      check_felsch_throws(tc2);

      REQUIRE(tc2.size() == 21);
      REQUIRE(tc2.size() == tc2.froidure_pin()->size());
      REQUIRE(tc2.equal_to(w3, w4));
      REQUIRE(tc2.normal_form(w3) == tc2.normal_form(w4));
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "068",
                            "Sym(5) from Chapter 3, Proposition 1.1 in NR",
                            "[todd-coxeter][quick]") {
      auto rg = ReportGuard(REPORT);

      ToddCoxeter tc;
      tc.set_alphabet("ABabe");
      tc.set_identity("e");
      tc.add_rule("aa", "e");
      tc.add_rule("bbbbb", "e");
      tc.add_rule("babababa", "e");
      tc.add_rule("bB", "e");
      tc.add_rule("Bb", "e");
      tc.add_rule("BabBab", "e");
      tc.add_rule("aBBabbaBBabb", "e");
      tc.add_rule("aBBBabbbaBBBabbb", "e");
      tc.add_rule("aA", "e");
      tc.add_rule("Aa", "e");

      SECTION("Deduction policy == purge_from_top") {
        tc.congruence()
            .max_deductions(2)
            .strategy(options::strategy::felsch)
            .max_preferred_defs(3);
        REQUIRE_THROWS_AS(tc.congruence().deduction_policy(
                              options::deductions::purge_from_top),
                          LibsemigroupsException);
        tc.congruence().deduction_policy(options::deductions::v1
                                         | options::deductions::purge_from_top);
      }
      SECTION("Deduction policy == purge_all") {
        tc.congruence().max_deductions(2).strategy(options::strategy::felsch);
        tc.congruence().deduction_policy(options::deductions::v1
                                         | options::deductions::purge_all);
      }
      SECTION("Deduction policy == discard_all_if_no_space") {
        tc.congruence().max_deductions(2).strategy(options::strategy::felsch);
        tc.congruence().deduction_policy(
            options::deductions::v2
            | options::deductions::discard_all_if_no_space);
      }
      check_hlt(tc);
      check_felsch(tc);
      check_random(tc);
      check_Rc_style(tc);
      check_R_over_C_style(tc);
      check_CR_style(tc);
      check_Cr_style(tc);

      REQUIRE(tc.size() == 120);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "069",
                            "Chapter 7, Theorem 3.6 in NR (size 243)",
                            "[no-valgrind][todd-coxeter][quick]") {
      auto        rg = ReportGuard(REPORT);
      ToddCoxeter tc;
      tc.set_alphabet("ab");
      tc.add_rule("aaa", "a");
      tc.add_rule("bbbb", "b");
      tc.add_rule("ababababab", "aa");

      check_hlt(tc);
      check_felsch(tc);
      check_random(tc);
      check_Rc_style(tc);
      check_R_over_C_style(tc);
      check_CR_style(tc);
      check_Cr_style(tc);

      REQUIRE(tc.size() == 243);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "070",
                            "finite semigroup (size 99)",
                            "[todd-coxeter][quick]") {
      auto        rg = ReportGuard(REPORT);
      ToddCoxeter tc;
      tc.set_alphabet("ab");
      tc.add_rule("aaa", "a");
      tc.add_rule("bbbb", "b");
      tc.add_rule("abababab", "aa");

      REQUIRE(!tc.is_obviously_finite());

      check_hlt(tc);
      check_felsch(tc);
      check_random(tc);
      check_Rc_style(tc);
      check_R_over_C_style(tc);
      check_CR_style(tc);
      check_Cr_style(tc);

      REQUIRE(tc.size() == 99);
      REQUIRE(tc.finished());
      REQUIRE(tc.is_obviously_finite());
    }

    // The following 8 examples are from Trevor Walker's Thesis: Semigroup
    // enumeration - computer implementation and applications, p41.
    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "071",
                            "Walker 1",
                            "[todd-coxeter][standard]") {
      auto        rg = ReportGuard(REPORT);
      ToddCoxeter tc;
      tc.set_alphabet("abcABCDEFGHIXYZ");
      tc.add_rule("A", "aaaaaaaaaaaaaa");
      tc.add_rule("B", "bbbbbbbbbbbbbb");
      tc.add_rule("C", "cccccccccccccc");
      tc.add_rule("D", "aaaaba");
      tc.add_rule("E", "bbbbab");
      tc.add_rule("F", "aaaaca");
      tc.add_rule("G", "ccccac");
      tc.add_rule("H", "bbbbcb");
      tc.add_rule("I", "ccccbc");
      tc.add_rule("X", "aaa");
      tc.add_rule("Y", "bbb");
      tc.add_rule("Z", "ccc");

      tc.add_rule("A", "a");
      tc.add_rule("B", "b");
      tc.add_rule("C", "c");
      tc.add_rule("D", "Y");
      tc.add_rule("E", "X");
      tc.add_rule("F", "Z");
      tc.add_rule("G", "X");
      tc.add_rule("H", "Z");
      tc.add_rule("I", "Y");

      tc.congruence()
          .sort_generating_pairs(shortlex_compare)
          .next_lookahead(500'000)
          .run_until([&tc]() -> bool {
            return tc.congruence().coset_capacity() >= 10'000;
          });
      REQUIRE(!tc.finished());
      REQUIRE(!tc.is_obviously_finite());
      tc.congruence().standardize(tc_order::shortlex);
      REQUIRE(!tc.finished());
      tc.congruence().standardize(tc_order::lex);
      REQUIRE(!tc.finished());
      tc.congruence().standardize(tc_order::recursive);
      REQUIRE(!tc.finished());

      check_hlt(tc);
      check_felsch(tc);
      check_random(tc);
      check_Rc_style(tc);
      check_R_over_C_style(tc);
      check_CR_style(tc);
      check_Cr_style(tc);

      // This takes approx 1 seconds with Felsch . . .
      REQUIRE(tc.size() == 1);
      tc.congruence().standardize(tc_order::shortlex);
      REQUIRE(std::is_sorted(tc.congruence().cbegin_normal_forms(),
                             tc.congruence().cend_normal_forms(),
                             ShortLexCompare<word_type>{}));
      tc.congruence().standardize(tc_order::lex);
      REQUIRE(std::is_sorted(tc.congruence().cbegin_normal_forms(),
                             tc.congruence().cend_normal_forms(),
                             LexicographicalCompare<word_type>{}));
      tc.congruence().standardize(tc_order::recursive);
      REQUIRE(std::is_sorted(tc.congruence().cbegin_normal_forms(),
                             tc.congruence().cend_normal_forms(),
                             RecursivePathCompare<word_type>{}));
    }

    // The following example is a good one for using the lookahead.
    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "072",
                            "Walker 2",
                            "[todd-coxeter][extreme]") {
      auto        rg = ReportGuard(REPORT);
      ToddCoxeter tc;
      tc.set_alphabet("ab");
      tc.add_rule("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", "a");
      tc.add_rule("bbb", "b");
      tc.add_rule("ababa", "b");
      tc.add_rule("aaaaaaaaaaaaaaaabaaaabaaaaaaaaaaaaaaaabaaaa", "b");

      REQUIRE(!tc.is_obviously_finite());

      SECTION("custom HLT") {
        tc.congruence()
            .sort_generating_pairs()
            .next_lookahead(1'000'000)
            .max_deductions(2'000)
            .use_relations_in_extra(true)
            .strategy(options::strategy::hlt)
            .lookahead(options::lookahead::partial | options::lookahead::felsch)
            .deduction_policy(options::deductions::v2
                              | options::deductions::no_stack_if_no_space);
      }

      check_hlt(tc);
      check_felsch(tc);
      check_random(tc);
      check_Rc_style(tc);
      check_R_over_C_style(tc);
      check_CR_style(tc);
      check_Cr_style(tc);
      REQUIRE(tc.size() == 14'911);
      tc.congruence().standardize(tc_order::shortlex);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "073",
                            "Walker 3",
                            "[todd-coxeter][standard]") {
      auto        rg = ReportGuard(REPORT);
      ToddCoxeter tc;
      tc.set_alphabet("ab");
      tc.add_rule("aaaaaaaaaaaaaaaa", "a");
      tc.add_rule("bbbbbbbbbbbbbbbb", "b");
      tc.add_rule("abb", "baa");
      tc.congruence().next_lookahead(2'000'000);
      tc.congruence().simplify();
      REQUIRE(!tc.is_obviously_finite());

      check_hlt(tc);
      check_felsch(tc);
      check_random(tc);
      // check_Rc_style(tc); // Rc_style + partial lookahead works very badly
      // 2m30s
      check_R_over_C_style(tc);
      check_Cr_style(tc);

      REQUIRE(tc.size() == 20'490);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "074",
                            "Walker 4",
                            "[todd-coxeter][extreme]") {
      auto        rg = ReportGuard();
      ToddCoxeter tc;
      tc.set_alphabet("ab");
      tc.add_rule("aaa", "a");
      tc.add_rule("bbbbbb", "b");
      tc.add_rule("ababbbbababbbbababbbbababbbbababbbbababbbbababbbbabba",
                  "bb");

      tc.congruence().next_lookahead(3'000'000);

      REQUIRE(!tc.is_obviously_finite());

      check_hlt(tc);
      // Felsch very slow
      check_random(tc);
      SECTION("custom R/C") {
        tc.congruence()
            .next_lookahead(3'000'000)
            .strategy(options::strategy::R_over_C)
            .max_deductions(100'000);
      }
      tc.congruence().run();
      REQUIRE(tc.size() == 36'412);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "075",
                            "Walker 5",
                            "[todd-coxeter][extreme]") {
      auto        rg = ReportGuard(REPORT);
      ToddCoxeter tc;
      tc.set_alphabet("ab");
      tc.add_rule("aaa", "a");
      tc.add_rule("bbbbbb", "b");
      tc.add_rule(
          "ababbbbababbbbababbbbababbbbababbbbababbbbababbbbabbabbbbbaa", "bb");
      tc.congruence().next_lookahead(5'000'000);
      REQUIRE(!tc.is_obviously_finite());

      // This example is extremely slow with Felsch
      check_hlt(tc);
      check_random(tc);
      check_Rc_style(tc);
      check_R_over_C_style(tc);
      check_CR_style(tc);
      check_Cr_style(tc);

      tc.congruence().run();
      REQUIRE(tc.congruence().complete());
      REQUIRE(tc.congruence().compatible());

      REQUIRE(tc.size() == 72'822);
      std::cout << tc.congruence().stats_string();
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "076",
                            "not Walker 6",
                            "[todd-coxeter][extreme]") {
      auto        rg = ReportGuard();
      ToddCoxeter tc;
      tc.set_alphabet("ab");
      tc.add_rule("aaa", "a");
      tc.add_rule("bbbbbbbbb", "b");
      tc.add_rule(
          "ababbbbababbbbababbbbababbbbababbbbababbbbababbbbabbabbbbbbbb",
          "bb");
      tc.congruence().next_lookahead(5'000'000);
      REQUIRE(!tc.is_obviously_finite());

      // This example is extremely slow with Felsch, the random strategy
      // strategy is typically fastest
      check_hlt(tc);
      check_random(tc);
      check_Rc_style(tc);
      check_R_over_C_style(tc);
      check_CR_style(tc);
      check_Cr_style(tc);

      REQUIRE(tc.size() == 8);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "077",
                            "Walker 6",
                            "[todd-coxeter][standard]") {
      auto        rg = ReportGuard(REPORT);
      ToddCoxeter tc;
      tc.set_alphabet("ab");
      tc.add_rule("aaa", "a");
      tc.add_rule("bbbbbbbbb", "b");
      std::string lng("ababbbbbbb");
      lng += lng;
      lng += "abbabbbbbbbb";
      tc.add_rule(lng, "bb");
      REQUIRE(!tc.is_obviously_finite());

      // This example is extremely slow with Felsch
      check_hlt(tc);
      check_random(tc);
      // check_Rc_style(tc); // partial lookahead is too slow
      // check_Cr_style(tc); // very slow
      check_R_over_C_style(tc);

      REQUIRE(tc.size() == 78'722);
    }

    // Felsch is faster here too!
    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "078",
                            "Walker 7",
                            "[todd-coxeter][standard]") {
      auto        rg = ReportGuard(REPORT);
      ToddCoxeter tc;
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
      REQUIRE(!tc.is_obviously_finite());

      check_hlt(tc);
      check_felsch(tc);
      check_random(tc);
      // check_Rc_style(tc); // partial lookahead very slow ~8s
      check_R_over_C_style(tc);
      check_Cr_style(tc);

      tc.congruence()
          .deduction_policy(options::deductions::v1
                            | options::deductions::no_stack_if_no_space)
          .preferred_defs(options::preferred_defs::none);

      REQUIRE(tc.size() == 153'500);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "079",
                            "Walker 8",
                            "[todd-coxeter][standard]") {
      auto        rg = ReportGuard(REPORT);
      ToddCoxeter tc;
      tc.set_alphabet("ab");
      tc.add_rule("aaa", "a");
      tc.add_rule("bbbbbbbbbbbbbbbbbbbbbbb", "b");
      tc.add_rule("abbbbbbbbbbbabb", "bba");

      REQUIRE(tc.congruence().length_of_generating_pairs() == 46);
      REQUIRE(!tc.is_obviously_finite());

      tc.congruence().next_lookahead(500'000);
      // This example is extremely slow with Felsch
      check_hlt(tc);
      check_random(tc);
      // check_Rc_style(tc); + partial lookahead too slow
      // check_Cr_style(tc); // too slow
      check_R_over_C_style(tc);

      REQUIRE(tc.congruence().number_of_classes() == 270'272);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "080",
                            "KnuthBendix 098",
                            "[todd-coxeter][quick][no-valgrind]") {
      auto        rg = ReportGuard(REPORT);
      ToddCoxeter tc;
      tc.set_alphabet("aAbBcCdDyYfFgGe");
      tc.set_identity("e");
      tc.set_inverses("AaBbCcDdYyFfGge");

      tc.add_rule("ab", "c");
      tc.add_rule("bc", "d");
      tc.add_rule("cd", "y");
      tc.add_rule("dy", "f");
      tc.add_rule("yf", "g");
      tc.add_rule("fg", "a");
      tc.add_rule("ga", "b");

      check_hlt(tc);
      check_felsch(tc);
      check_random(tc);
      check_Rc_style(tc);
      check_R_over_C_style(tc);
      check_CR_style(tc);
      check_Cr_style(tc);

      REQUIRE(tc.size() == 29);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "081",
                            "Holt 2 - SL(2, p)",
                            "[todd-coxeter][extreme]") {
      auto        rg = ReportGuard();
      ToddCoxeter tc;

      tc.set_alphabet("xXyYe");
      tc.set_identity("e");
      tc.set_inverses("XxYye");

      tc.add_rule("xxYXYXYX", "e");

      auto second = [](size_t p) -> std::string {
        std::string s = "xyyyyx";
        s += std::string((p + 1) / 2, 'y');
        s += s;
        s += std::string(p, 'y');
        s += std::string(2 * (p / 3), 'x');
        return s;
      };

      REQUIRE(second(3) == "xyyyyxyyxyyyyxyyyyyxx");
      SECTION("p = 3") {
        tc.add_rule(second(3), "e");

        check_hlt(tc);
        check_felsch(tc);

        REQUIRE(tc.size() == 24);
      }
      SECTION("p = 5") {
        tc.add_rule(second(5), "e");

        check_hlt(tc);
        check_felsch(tc);

        REQUIRE(tc.size() == 120);
      }
      SECTION("p = 7") {
        tc.add_rule(second(7), "e");

        check_hlt(tc);
        check_felsch(tc);

        REQUIRE(tc.size() == 336);
      }
      SECTION("p = 11") {
        tc.add_rule(second(11), "e");

        check_hlt(tc);
        check_random(tc);

        REQUIRE(tc.size() == 1'320);
      }
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "082",
                            "Holt 3",
                            "[todd-coxeter][standard]") {
      auto        rg = ReportGuard(REPORT);
      ToddCoxeter tc;
      tc.set_alphabet("aAbBcCe");
      tc.set_identity("e");
      tc.set_inverses("AaBbCce");

      tc.add_rule("bbCbc", "e");
      tc.add_rule("aaBab", "e");
      tc.add_rule("cABcabc", "e");
      REQUIRE(tc.congruence().is_non_trivial() == tril::TRUE);

      check_hlt(tc);
      check_felsch(tc);
      check_random(tc);
      check_Rc_style(tc);
      check_R_over_C_style(tc);
      check_CR_style(tc);
      check_Cr_style(tc);

      REQUIRE(tc.size() == 6'561);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "083",
                            "Holt 3",
                            "[todd-coxeter][fail]") {
      auto        rg = ReportGuard();
      ToddCoxeter tc;
      tc.set_alphabet("aAbBcCe");
      tc.set_identity("e");
      tc.set_inverses("AaBbCce");

      tc.add_rule("aaCac", "e");
      tc.add_rule("acbbACb", "e");
      tc.add_rule("ABabccc", "e");
      REQUIRE(!tc.is_obviously_infinite());
      REQUIRE(tc.congruence().number_of_generating_pairs() == 22);
      tc.congruence().strategy(options::strategy::R_over_C);
      tc.congruence()
          .sort_generating_pairs()
          .remove_duplicate_generating_pairs();
      REQUIRE(tc.congruence().number_of_generating_pairs() == 22);
      tc.congruence()
          .lookahead(options::lookahead::partial | options::lookahead::hlt)
          .lookahead_growth_factor(1.01)
          .lookahead_growth_threshold(10)
          .f_defs(250'000)
          .hlt_defs(20'000'000);
      // REQUIRE(tc.congruence().is_non_trivial() == tril::TRUE);
      tc.congruence().run();
      REQUIRE(tc.size() == 6'561);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "084",
                            "Campbell-Reza 1",
                            "[todd-coxeter][quick]") {
      auto        rg = ReportGuard(REPORT);
      ToddCoxeter tc;
      tc.set_alphabet("ab");
      tc.add_rule("aa", "bb");
      tc.add_rule("ba", "aaaaaab");

      check_hlt(tc);
      check_felsch(tc);
      check_random(tc);
      check_Rc_style(tc);
      check_R_over_C_style(tc);
      check_CR_style(tc);
      check_Cr_style(tc);

      REQUIRE(tc.size() == 14);
      tc.congruence().standardize(tc_order::shortlex);
      REQUIRE(std::vector<word_type>(tc.congruence().cbegin_normal_forms(),
                                     tc.congruence().cend_normal_forms())
              == std::vector<word_type>({{0},
                                         {1},
                                         {0, 0},
                                         {0, 1},
                                         {1, 0},
                                         {0, 0, 0},
                                         {0, 0, 1},
                                         {0, 0, 0, 0},
                                         {0, 0, 0, 1},
                                         {0, 0, 0, 0, 0},
                                         {0, 0, 0, 0, 1},
                                         {0, 0, 0, 0, 0, 0},
                                         {0, 0, 0, 0, 0, 1},
                                         {0, 0, 0, 0, 0, 0, 0}}));
      REQUIRE(tc.froidure_pin()->number_of_rules() == 6);
      REQUIRE(tc.normal_form("aaaaaaab") == "aab");
      REQUIRE(tc.normal_form("bab") == "aaa");
    }

    // The next example demonstrates why we require deferred standardization
    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "085",
                            "Renner monoid type D4 (Gay-Hivert), q = 1",
                            "[no-valgrind][quick][todd-coxeter][no-coverage]") {
      auto        rg = ReportGuard(REPORT);
      ToddCoxeter tc;
      tc.set_alphabet(11);
      for (relation_type const& rl : RennerTypeDMonoid(4, 1)) {
        tc.add_rule(rl);
      }
      REQUIRE(tc.number_of_rules() == 121);
      REQUIRE(!tc.is_obviously_infinite());

      REQUIRE(tc.size() == 10'625);

      check_hlt(tc);
      check_felsch(tc);
      check_random(tc);
      check_Rc_style(tc);
      check_R_over_C_style(tc);
      check_CR_style(tc);
      check_Cr_style(tc);

      tc.congruence().standardize(tc_order::shortlex);
      REQUIRE(std::is_sorted(tc.congruence().cbegin_normal_forms(),
                             tc.congruence().cend_normal_forms(),
                             ShortLexCompare<word_type>{}));
      tc.congruence().standardize(tc_order::lex);
      REQUIRE(std::is_sorted(tc.congruence().cbegin_normal_forms(),
                             tc.congruence().cend_normal_forms(),
                             LexicographicalCompare<word_type>{}));
      // The next section is very slow
      // SECTION("standardizing with recursive order") {
      //  tc.congruence().standardize(tc_order::recursive);
      //  REQUIRE(std::is_sorted(tc.congruence().cbegin_normal_forms(),
      //                         tc.congruence().cend_normal_forms(),
      //                         RecursivePathCompare<word_type>{}));
      // }
    }

    // Felsch very slow here
    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "086",
                            "trivial semigroup",
                            "[no-valgrind][todd-coxeter][quick][no-coverage]") {
      auto rg = ReportGuard(REPORT);

      for (size_t N = 2; N < 1000; N += 199) {
        ToddCoxeter tc;
        tc.set_alphabet("eab");
        tc.set_identity("e");
        std::string lhs = "a" + std::string(N, 'b');
        std::string rhs = "e";
        tc.add_rule(lhs, rhs);

        lhs = std::string(N, 'a');
        rhs = std::string(N + 1, 'b');
        tc.add_rule(lhs, rhs);

        lhs = "ba";
        rhs = std::string(N, 'b') + "a";
        tc.add_rule(lhs, rhs);
        tc.run();
        if (N % 3 == 1) {
          REQUIRE(tc.size() == 3);
        } else {
          REQUIRE(tc.size() == 1);
        }
      }
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "087",
                            "ACE --- 2p17-2p14 - HLT",
                            "[todd-coxeter][standard][ace]") {
      auto        rg = ReportGuard(false);
      ToddCoxeter G;
      G.set_alphabet("abcABCe");
      G.set_identity("e");
      G.set_inverses("ABCabce");
      G.add_rule("aBCbac", "e");
      G.add_rule("bACbaacA", "e");
      G.add_rule("accAABab", "e");

      congruence::ToddCoxeter H(right, G.congruence());
      H.add_pair({1, 2}, {6});
      H.next_lookahead(1'000'000);

      REQUIRE(H.number_of_classes() == 16'384);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "088",
                            "ACE --- 2p17-2p3 - HLT",
                            "[todd-coxeter][standard][ace]") {
      auto        rg = ReportGuard(REPORT);
      ToddCoxeter G;
      G.set_alphabet("abcABCe");
      G.set_identity("e");
      G.set_inverses("ABCabce");
      G.add_rule("aBCbac", "e");
      G.add_rule("bACbaacA", "e");
      G.add_rule("accAABab", "e");

      letter_type             a = 0;
      letter_type             b = 1;
      letter_type             c = 2;
      letter_type             A = 3;
      letter_type             B = 4;
      letter_type             C = 5;
      letter_type             e = 6;
      congruence::ToddCoxeter H(right, G.congruence());
      H.add_pair({b, c}, {e});
      H.add_pair({b, c}, {A, B, A, A, b, c, a, b, C});

      H.strategy(options::strategy::hlt)
          .save(true)
          .lookahead(options::lookahead::partial);

      REQUIRE(H.number_of_classes() == 8);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "089",
                            "ACE --- 2p17-1a - HLT",
                            "[todd-coxeter][standard][ace]") {
      auto        rg = ReportGuard(REPORT);
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

      congruence::ToddCoxeter H(right, G.congruence());
      H.add_pair({b, c}, {e});
      H.add_pair({A, B, A, A, b, c, a, b, C}, {e});
      H.add_pair({A, c, c, c, a, c, B, c, A}, {e});
      H.large_collapse(10'000);

      H.strategy(options::strategy::hlt)
          .save(true)
          .lookahead(options::lookahead::partial);
      REQUIRE(H.number_of_classes() == 1);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "090",
                            "ACE --- F27",
                            "[todd-coxeter][standard][ace]") {
      auto        rg = ReportGuard(REPORT);
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

      congruence::ToddCoxeter H(twosided, G);
      check_felsch(H);
      check_hlt(H);
      check_random(H);

      REQUIRE(H.number_of_classes() == 29);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "091",
                            "ACE --- SL219 - HLT",
                            "[todd-coxeter][standard][ace]") {
      auto        rg = ReportGuard(REPORT);
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
          .lookahead(options::lookahead::partial);
      REQUIRE(H.number_of_classes() == 180);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "092",
                            "ACE --- perf602p5 - HLT",
                            "[no-valgrind][todd-coxeter][quick][ace]") {
      auto        rg = ReportGuard(REPORT);
      ToddCoxeter G;
      G.set_alphabet("abstuvdABSTUVDe");
      G.set_identity("e");
      G.set_inverses("ABSTUVDabstuvde");

      G.add_rule("aaD", "e");
      G.add_rule("bbb", "e");
      G.add_rule("ababababab", "e");
      G.add_rule("ss", "e");
      G.add_rule("tt", "e");
      G.add_rule("uu", "e");
      G.add_rule("vv", "e");
      G.add_rule("dd", "e");
      G.add_rule("STst", "e");
      G.add_rule("UVuv", "e");
      G.add_rule("SUsu", "e");
      G.add_rule("SVsv", "e");
      G.add_rule("TUtu", "e");
      G.add_rule("TVtv", "e");
      G.add_rule("AsaU", "e");
      G.add_rule("AtaV", "e");
      G.add_rule("AuaS", "e");
      G.add_rule("AvaT", "e");
      G.add_rule("BsbDVT", "e");
      G.add_rule("BtbVUTS", "e");
      G.add_rule("BubVU", "e");
      G.add_rule("BvbU", "e");
      G.add_rule("DAda", "e");
      G.add_rule("DBdb", "e");
      G.add_rule("DSds", "e");
      G.add_rule("DTdt", "e");
      G.add_rule("DUdu", "e");
      G.add_rule("DVdv", "e");

      congruence::ToddCoxeter H(right, G);

      letter_type a = 0;
      letter_type e = 14;

      H.add_pair({a}, {e});

      check_hlt(H);
      check_random(H);
      check_felsch(H);

      REQUIRE(H.number_of_classes() == 480);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "093",
                            "ACE --- M12",
                            "[todd-coxeter][standard][ace]") {
      auto        rg = ReportGuard(REPORT);
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

      SECTION("HLT + save + partial lookahead") {
        H.strategy(options::strategy::hlt)
            .save(true)
            .lookahead(options::lookahead::partial);
      }
      SECTION("random") {
        H.strategy(options::strategy::random)
            .random_interval(std::chrono::milliseconds(100));
      }
      check_felsch(H);

      REQUIRE(H.number_of_classes() == 95'040);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "094",
                            "ACE --- C5 - HLT",
                            "[todd-coxeter][quick][ace]") {
      auto        rg = ReportGuard(REPORT);
      ToddCoxeter G;
      G.set_alphabet("abABe");
      G.set_identity("e");
      G.set_inverses("ABabe");
      G.add_rule("aaaaa", "e");
      G.add_rule("b", "e");

      congruence::ToddCoxeter H(twosided, G);

      check_hlt(H);
      check_random(H);
      check_felsch(H);

      REQUIRE(H.number_of_classes() == 5);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "095",
                            "ACE --- A5-C5",
                            "[todd-coxeter][quick][ace]") {
      auto        rg = ReportGuard(REPORT);
      ToddCoxeter G;
      G.set_alphabet("abABe");
      G.set_identity("e");
      G.set_inverses("ABabe");
      G.add_rule("aa", "e");
      G.add_rule("bbb", "e");
      G.add_rule("ababababab", "e");

      congruence::ToddCoxeter H(right, G);

      letter_type const a = 0, b = 1, e = 4;

      H.add_pair({a, b}, {e});

      check_hlt(H);
      check_random(H);
      check_felsch(H);
      REQUIRE(H.number_of_classes() == 12);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "096",
                            "ACE --- A5",
                            "[todd-coxeter][quick][ace]") {
      auto        rg = ReportGuard(REPORT);
      ToddCoxeter G;
      G.set_alphabet("abABe");
      G.set_identity("e");
      G.set_inverses("ABabe");
      G.add_rule("aa", "e");
      G.add_rule("bbb", "e");
      G.add_rule("ababababab", "e");

      congruence::ToddCoxeter H(twosided, G);

      check_hlt(H);
      check_random(H);
      check_felsch(H);
      H.random_shuffle_generating_pairs();

      REQUIRE(H.number_of_classes() == 60);
      REQUIRE_THROWS_AS(H.random_shuffle_generating_pairs(),
                        LibsemigroupsException);
    }

    // Felsch is much much better here
    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "097",
                            "relation ordering",
                            "[todd-coxeter][extreme]") {
      auto        rg = ReportGuard();
      ToddCoxeter tc;
      tc.set_alphabet(13);
      for (relation_type const& rl : RennerTypeDMonoid(5, 1)) {
        tc.add_rule(rl);
      }
      REQUIRE(tc.number_of_rules() == 173);
      REQUIRE(!tc.is_obviously_infinite());
      tc.congruence()
          .sort_generating_pairs(&shortlex_compare)
          .sort_generating_pairs(recursive_path_compare)
          .remove_duplicate_generating_pairs();
      REQUIRE(tc.number_of_rules() == 173);

      tc.congruence().strategy(options::strategy::felsch).f_defs(100'000).run();
      REQUIRE(tc.size() == 258'661);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "098",
                            "relation ordering",
                            "[todd-coxeter][quick]") {
      ToddCoxeter tc;
      tc.set_alphabet(10);
      tc.add_rule({0, 1}, {0});
      tc.add_rule({0, 2}, {0});
      tc.add_rule({0, 3}, {0});
      tc.add_rule({0, 4}, {0});
      tc.add_rule({0, 5}, {0});
      tc.add_rule({0, 6}, {0});
      tc.add_rule({0, 7}, {0});
      tc.add_rule({0, 8}, {0});
      tc.add_rule({0, 9}, {0});
      tc.add_rule({1, 0}, {1});
      tc.add_rule({1, 1}, {1});
      tc.add_rule({1, 2}, {1});
      tc.add_rule({1, 3}, {1});
      tc.add_rule({1, 4}, {1});
      tc.add_rule({1, 5}, {1});
      tc.add_rule({1, 6}, {1});
      tc.add_rule({1, 7}, {1});
      tc.add_rule({1, 8}, {1});
      tc.add_rule({1, 9}, {1});
      tc.add_rule({2, 0}, {2});
      tc.add_rule({2, 1}, {2});
      tc.add_rule({2, 2}, {2});
      tc.add_rule({2, 3}, {2});
      tc.add_rule({2, 4}, {2});
      tc.add_rule({2, 5}, {2});
      tc.add_rule({2, 6}, {2});
      tc.add_rule({2, 7}, {2});
      tc.add_rule({2, 8}, {2});
      tc.add_rule({2, 9}, {2});
      tc.add_rule({3, 0}, {3});
      tc.add_rule({3, 1}, {3});
      tc.add_rule({3, 2}, {3});
      tc.add_rule({3, 3}, {3});
      tc.add_rule({3, 4}, {3});
      tc.add_rule({3, 5}, {3});
      tc.add_rule({3, 6}, {3});
      tc.add_rule({3, 7}, {3});
      tc.add_rule({3, 8}, {3});
      tc.add_rule({3, 9}, {3});
      tc.add_rule({4, 0}, {4});
      tc.add_rule({4, 1}, {4});
      tc.add_rule({4, 2}, {4});
      tc.add_rule({4, 3}, {4});
      tc.add_rule({4, 4}, {4});
      tc.add_rule({4, 5}, {4});
      tc.add_rule({4, 6}, {4});
      tc.add_rule({4, 7}, {4});
      tc.add_rule({4, 8}, {4});
      tc.add_rule({4, 9}, {4});
      tc.add_rule({5, 0}, {5});
      tc.add_rule({5, 1}, {5});
      tc.add_rule({5, 2}, {5});
      tc.add_rule({5, 3}, {5});
      tc.add_rule({5, 4}, {5});
      tc.add_rule({5, 5}, {5});
      tc.add_rule({5, 6}, {5});
      tc.add_rule({5, 7}, {5});
      tc.add_rule({5, 8}, {5});
      tc.add_rule({5, 9}, {5});
      tc.add_rule({6, 0}, {6});
      tc.add_rule({6, 1}, {6});
      tc.add_rule({6, 2}, {6});
      tc.add_rule({6, 3}, {6});
      tc.add_rule({6, 4}, {6});
      tc.add_rule({6, 5}, {6});
      tc.add_rule({6, 6}, {6});
      tc.add_rule({6, 7}, {6});
      tc.add_rule({6, 8}, {6});
      tc.add_rule({6, 9}, {6});
      tc.add_rule({7, 0}, {7});
      tc.add_rule({7, 1}, {7});
      tc.add_rule({7}, {7, 2});
      tc.add_rule({7, 3}, {7});
      tc.add_rule({7, 4}, {7});
      tc.add_rule({7, 5}, {7});
      tc.add_rule({7, 6}, {7});
      tc.add_rule({7, 7}, {7});
      tc.add_rule({7, 8}, {7});
      tc.add_rule({7, 9}, {7});
      tc.add_rule({8, 0}, {8});
      tc.add_rule({8, 1}, {8});
      tc.add_rule({8, 2}, {8});
      tc.add_rule({8, 3}, {8});
      tc.add_rule({8, 4}, {8});
      tc.add_rule({8, 5}, {8});
      tc.add_rule({8, 6}, {8});
      tc.add_rule({8, 7}, {8});
      tc.add_rule({8, 8}, {8});
      tc.add_rule({8, 9}, {8});
      tc.add_rule({9, 0}, {9});
      tc.add_rule({9, 0, 1, 2, 3, 4, 5, 5, 1, 5, 6, 9, 8, 8, 8, 8, 8, 0}, {9});
      tc.congruence().sort_generating_pairs(recursive_path_compare);

      check_felsch(tc);
      check_hlt(tc);
      check_random(tc);

      REQUIRE(tc.size() == 10);

      REQUIRE_THROWS_AS(tc.congruence().sort_generating_pairs(shortlex_compare),
                        LibsemigroupsException);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "099",
                            "short circuit size in obviously infinite",
                            "[todd-coxeter][quick]") {
      auto        rg = ReportGuard(REPORT);
      ToddCoxeter tc;
      tc.set_alphabet("abc");
      tc.add_rule("aaaa", "a");
      REQUIRE(tc.size() == POSITIVE_INFINITY);
    }

    LIBSEMIGROUPS_TEST_CASE(
        "ToddCoxeter",
        "100",
        "http://brauer.maths.qmul.ac.uk/Atlas/misc/24A8/mag/24A8G1-P1.M",
        "[todd-coxeter][standard]") {
      ToddCoxeter tc;
      tc.set_alphabet("xyXYe");
      tc.set_identity("e");
      tc.set_inverses("XYxye");
      tc.add_rule("xx", "X");
      tc.add_rule("yyyyyy", "Y");
      tc.add_rule("YXyx", "XYxy");
      tc.add_rule("xYYYxYYYxYY", "yyXyyyXyyyX");
      tc.add_rule("xyxyyXyxYYxyyyx", "yyyXyyy");
      tc.congruence()
          .next_lookahead(2'000'000)
          .strategy(options::strategy::hlt)
          .sort_generating_pairs()
          .lookahead(options::lookahead::partial)
          .standardize(true);
      tc.congruence().run();

      REQUIRE(tc.size() == 322'560);
    }

    LIBSEMIGROUPS_TEST_CASE(
        "ToddCoxeter",
        "101",
        "http://brauer.maths.qmul.ac.uk/Atlas/spor/M11/mag/M11G1-P1.M",
        "[todd-coxeter][quick][no-coverage][no-valgrind]") {
      ToddCoxeter tc;
      tc.set_alphabet("xyXYe");
      tc.set_identity("e");
      tc.set_inverses("XYxye");
      tc.add_rule("xx", "e");
      tc.add_rule("yyyy", "e");
      tc.add_rule("xyxyxyxyxyxyxyxyxyxyxy", "e");
      tc.add_rule("xyyxyyxyyxyyxyyxyy", "e");
      tc.add_rule("xyxyxYxyxyyxYxyxYxY", "e");
      REQUIRE(tc.size() == 7'920);
    }

    LIBSEMIGROUPS_TEST_CASE(
        "ToddCoxeter",
        "102",
        "http://brauer.maths.qmul.ac.uk/Atlas/spor/M12/mag/M12G1-P1.M",
        "[todd-coxeter][standard]") {
      ToddCoxeter tc;
      tc.set_alphabet("xyXYe");
      tc.set_identity("e");
      tc.set_inverses("XYxye");
      tc.add_rule("xx", "e");
      tc.add_rule("yyy", "e");
      tc.add_rule("xyxyxyxyxyxyxyxyxyxyxy", "e");
      tc.add_rule("XYxyXYxyXYxyXYxyXYxyXYxy", "e");
      tc.add_rule("xyxyxYxyxyxYxyxyxYxyxyxYxyxyxYxyxyxY", "e");
      tc.add_rule("XYXYxyxyXYXYxyxyXYXYxyxyXYXYxyxyXYXYxyxy", "e");
      REQUIRE(tc.size() == 95'040);
    }

    LIBSEMIGROUPS_TEST_CASE(
        "ToddCoxeter",
        "103",
        "http://brauer.maths.qmul.ac.uk/Atlas/spor/M22/mag/M22G1-P1.M",
        "[todd-coxeter][extreme]") {
      ToddCoxeter tc;
      tc.set_alphabet("xyXYe");
      tc.set_identity("e");
      tc.set_inverses("XYxye");
      tc.add_rule("xx", "e");
      tc.add_rule("yyyy", "e");
      tc.add_rule("xyxyxyxyxyxyxyxyxyxyxy", "e");
      tc.add_rule("xyyxyyxyyxyyxyy", "e");
      tc.add_rule("XYxyXYxyXYxyXYxyXYxyXYxy", "e");
      tc.add_rule("XYXYxyxyXYXYxyxyXYXYxyxy", "e");
      tc.add_rule("xyxyxYxyxyxYxyxyxYxyxyxYxyxyxY", "e");
      REQUIRE(tc.size() == 443'520);
    }

    // Takes about 4 minutes (2021 - MacBook Air M1 - 8GB RAM)
    // with Felsch (3.5mins or 2.5mins with lowerbound) or HLT (4.5mins)
    LIBSEMIGROUPS_TEST_CASE(
        "ToddCoxeter",
        "104",
        "http://brauer.maths.qmul.ac.uk/Atlas/spor/M23/mag/M23G1-P1.M",
        "[todd-coxeter][extreme]") {
      ToddCoxeter tc;
      tc.set_alphabet("xyXYe");
      tc.set_identity("e");
      tc.set_inverses("XYxye");
      tc.add_rule("xx", "e");
      tc.add_rule("yyyy", "e");
      tc.add_rule("xyxyxyxyxyxyxyxyxyxyxyxyxyxyxyxyxyxyxyxyxyxyxy", "e");
      tc.add_rule("xyyxyyxyyxyyxyyxyy", "e");
      tc.add_rule("XYxyXYxyXYxyXYxyXYxyXYxy", "e");
      tc.add_rule("xyxYxyyxyxYxyyxyxYxyyxyxYxyy", "e");
      tc.add_rule("xyxyxyxYxyyxyxYxyxYxyxyxyxYxYxY", "e");
      tc.add_rule("xyxyyxyyxyxyyxyyxyxyyxyyxyxyyxyyxyxyyxyyxyxyyxyy", "e");
      tc.add_rule("xyxyyxyxyyxyxyyxyyxYxyyxYxyxyyxyxYxyy", "e");
      tc.congruence()
          .sort_generating_pairs()
          .strategy(options::strategy::felsch)
          .use_relations_in_extra(true)
          .lower_bound(10'200'960)
          .deduction_policy(options::deductions::v2
                            | options::deductions::no_stack_if_no_space)
          .reserve(50'000'000);
      std::cout << tc.congruence().settings_string();
      tc.congruence().run();

      REQUIRE(tc.size() == 10'200'960);
    }

    // Takes about 3 minutes
    LIBSEMIGROUPS_TEST_CASE(
        "ToddCoxeter",
        "105",
        "http://brauer.maths.qmul.ac.uk/Atlas/clas/S62/mag/S62G1-P1.M",
        "[todd-coxeter][extreme]") {
      ToddCoxeter tc;
      tc.set_alphabet("xyXYe");
      tc.set_identity("e");
      tc.set_inverses("XYxye");
      tc.add_rule("xx", "e");
      tc.add_rule("yyy", "e");
      tc.add_rule("xyxyxyxyxyxyxyxyxyxyxyxyxyxyxyxyxyxyxyxyxyxyxy", "e");
      tc.add_rule("XYxyXYxyXYxyXYxyXYxyXYxyXYxyXYxyXYxyXYxyXYxyXYxy", "e");
      tc.add_rule("XYXYxyxyXYXYxyxyXYXYxyxyXYXYxyxyXYXYxyxy", "e");
      tc.add_rule("xyxyxYxyxyxYxyxyxYxyxYxYxyxYxYxyxYxY", "e");
      tc.add_rule("xyxyxYxyxYxyxYxyxyxYxyxYxyxYxyxyxYxyxYxyxYxyxyxYxyxYxyxY",
                  "e");

      congruence::ToddCoxeter tc2(congruence_kind::right, tc);
      tc2.add_pair(tc.string_to_word("xy"), tc.string_to_word("e"));

      REQUIRE(tc2.number_of_classes() == 10'644'480);
    }

    // Approx. 32 minutes (2021 - MacBook Air M1 - 8GB RAM)
    LIBSEMIGROUPS_TEST_CASE(
        "ToddCoxeter",
        "106",
        "http://brauer.maths.qmul.ac.uk/Atlas/spor/HS/mag/HSG1-P1.M",
        "[todd-coxeter][extreme]") {
      auto        rg = ReportGuard();
      ToddCoxeter tc;
      tc.set_alphabet("xyXYe");
      tc.set_identity("e");
      tc.set_inverses("XYxye");
      tc.add_rule("xx", "e");
      tc.add_rule("yyyyy", "e");
      tc.add_rule("xyxyxyxyxyxyxyxyxyxyxy", "e");
      tc.add_rule("xyyxyyxyyxyyxyyxyyxyyxyyxyyxyy", "e");
      tc.add_rule("XYxyXYxyXYxyXYxyXYxy", "e");
      tc.add_rule("XYXYxyxyXYXYxyxyXYXYxyxy", "e");
      tc.add_rule("XYYxyyXYYxyyXYYxyyXYYxyyXYYxyyXYYxyy", "e");
      tc.add_rule("xyxyxyyxYxYYxYxyyxyxyxYYxYYxYYxYY", "e");
      tc.add_rule("xyxyyxYYxYYxyyxYYxYYxyyxyxyyxYxyyxYxyy", "e");
      tc.add_rule("xyxyxyyxyyxyxYxYxyxyyxyyxyxyxYYxYxYY", "e");
      tc.add_rule("xyxyxyyxYxYYxyxyxYxyxyxyyxYxYYxyxyxY", "e");
      tc.add_rule("xyxyxyyxyxyxyyxyxyxYxyxyxyyxyyxyyxyxyxY", "e");
      tc.add_rule("xyxyxyyxyxyyxyxyyxyxyxyyxYxyxYYxyxYxyy", "e");
      congruence::ToddCoxeter tc2(congruence_kind::right, tc);
      tc2.add_pair(tc.string_to_word("xy"), tc.string_to_word("e"));
      tc2.sort_generating_pairs()
          .use_relations_in_extra(true)
          .strategy(options::strategy::hlt)
          .lookahead(options::lookahead::felsch | options::lookahead::partial);
      REQUIRE(tc2.number_of_classes() == 4'032'000);
    }

    LIBSEMIGROUPS_TEST_CASE(
        "ToddCoxeter",
        "107",
        "http://brauer.maths.qmul.ac.uk/Atlas/spor/J1/mag/J1G1-P1.M",
        "[todd-coxeter][standard]") {
      ToddCoxeter tc;
      tc.set_alphabet("xyXYe");
      tc.set_identity("e");
      tc.set_inverses("XYxye");
      tc.add_rule("xx", "e");
      tc.add_rule("yyy", "e");
      tc.add_rule("xyxyxyxyxyxyxy", "e");
      tc.add_rule("xyxyxYxyxYxyxYxyxyxYxyxYxyxYxyxyxYxyxYxyxYxyxyxYxyxYxyxYxyxy"
                  "xYxyxYxyxY",
                  "e");
      tc.add_rule("xyxyxYxyxYxyxYxyxYxyxYxyxYxyxyxYxYxyxyxYxyxYxyxYxyxYxyxYxyxY"
                  "xyxyxYxY",
                  "e");
      REQUIRE(tc.size() == 175'560);
    }

    LIBSEMIGROUPS_TEST_CASE(
        "ToddCoxeter",
        "108",
        "http://brauer.maths.qmul.ac.uk/Atlas/lin/L34/mag/L34G1-P1.M",
        "[todd-coxeter][quick][no-coverage][no-valgrind]") {
      ToddCoxeter tc;
      tc.set_alphabet("xyXYe");
      tc.set_identity("e");
      tc.set_inverses("XYxye");
      tc.add_rule("xx", "e");
      tc.add_rule("yyyy", "e");
      tc.add_rule("xyxyxyxyxyxyxy", "e");
      tc.add_rule("xyyxyyxyyxyyxyy", "e");
      tc.add_rule("XYxyXYxyXYxyXYxyXYxy", "e");
      tc.add_rule("xyxyxYxyxyxYxyxyxYxyxyxYxyxyxY", "e");
      tc.add_rule("xyxyxyyxYxyxyxyyxYxyxyxyyxYxyxyxyyxYxyxyxyyxY", "e");
      REQUIRE(tc.size() == 20'160);
    }

    // Takes about 10 seconds (2021 - MacBook Air M1 - 8GB RAM)
    LIBSEMIGROUPS_TEST_CASE(
        "ToddCoxeter",
        "109",
        "http://brauer.maths.qmul.ac.uk/Atlas/clas/S62/mag/S62G1-P1.M",
        "[todd-coxeter][extreme]") {
      ToddCoxeter tc;
      tc.set_alphabet("xyXYe");
      tc.set_identity("e");
      tc.set_inverses("XYxye");
      tc.add_rule("xx", "e");
      tc.add_rule("yyyyyyy", "e");
      tc.add_rule("xyxyxyxyxyxyxyxyxy", "e");
      tc.add_rule("xyyxyyxyyxyyxyyxyyxyyxyyxyyxyyxyyxyy", "e");
      tc.add_rule("XYXYXYxyxyxyXYXYXYxyxyxy", "e");
      tc.add_rule("XYxyXYxyXYxy", "e");
      tc.add_rule("XYYxyyXYYxyy", "e");
      REQUIRE(tc.size() == 1'451'520);
      std::cout << tc.congruence().stats_string();
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "110",
                            "Moore's Alt(7)",
                            "[todd-coxeter][fail]") {
      auto rg = ReportGuard(true);
      // KnuthBendix kb = AlternatingGroupMoore<KnuthBendix>(5);
      // kb.run();
      // REQUIRE(kb.size() == POSITIVE_INFINITY);
      ToddCoxeter tc = AlternatingGroupMoore<ToddCoxeter>(5);
      std::cout << std::vector<FpSemigroup::rule_type>(tc.cbegin_rules(),
                                                       tc.cend_rules())
                << std::endl;
      REQUIRE(tc.size() == 5'040 / 2);
    }

  }  // namespace fpsemigroup

}  // namespace libsemigroups
