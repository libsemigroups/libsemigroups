//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019 James D. Mitchell
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

// TODO add tests that normal_form(c) = min. word w in the order such that
// $\tau(0, w) = c!!

#include <algorithm>   // for count, sort, transform
#include <chrono>      // for duration, milliseconds
#include <cstddef>     // for size_t
#include <functional>  // for mem_fn
#include <vector>      // for vector

#include "catch.hpp"      // for SECTION, REQUIRE, REQUIRE_THROWS_AS
#include "test-main.hpp"  // for LIBSEMIGROUPS_TEST_CASE

#include "fpsemi-examples.hpp"      // for RennerTypeDMonoid, RennerTypeBMonoid
#include "libsemigroups/bmat8.hpp"  // for Bmat8
#include "libsemigroups/cong-intf.hpp"       // for congruence::type
#include "libsemigroups/element-helper.hpp"  // for TransfHelper
#include "libsemigroups/element.hpp"         // for Element, Transf,
#include "libsemigroups/fpsemi.hpp"          // for FpSemigroup
#include "libsemigroups/froidure-pin.hpp"  // for FroidurePin, FroidurePin<Element const*>::eleme...
#include "libsemigroups/knuth-bendix.hpp"  // for KnuthBendix
#include "libsemigroups/order.hpp"         // for shortlex_words
#include "libsemigroups/tce.hpp"           // for TCE
#include "libsemigroups/todd-coxeter.hpp"  // for ToddCoxeter
#include "libsemigroups/types.hpp"         // for relation_type, word_type
#include "libsemigroups/wislo.hpp"         // for cbegin_wislo

// Macros for testing different strategies on the same example.

#define TEST_FELSCH_THROWS(var)                               \
  SECTION("Felsch (throws)") {                                \
    REQUIRE_THROWS_AS(var.strategy(policy::strategy::felsch), \
                      LibsemigroupsException);                \
  }

#define TEST_FELSCH(var)                                       \
  SECTION("Felsch + no standardisation") {                     \
    var.strategy(policy::strategy::felsch).standardize(false); \
  }                                                            \
  SECTION("Felsch + standardisation") {                        \
    var.strategy(policy::strategy::felsch).standardize(true);  \
  }

#define TEST_HLT_NO_SAVE(var)                                     \
  SECTION("HLT + no standardise + full lookahead + no save") {    \
    var.strategy(policy::strategy::hlt)                           \
        .standardize(false)                                       \
        .lookahead(policy::lookahead::full)                       \
        .save(false);                                             \
  }                                                               \
  SECTION("HLT + standardise + full lookahead + no save") {       \
    var.strategy(policy::strategy::hlt)                           \
        .standardize(true)                                        \
        .lookahead(policy::lookahead::full)                       \
        .save(false);                                             \
  }                                                               \
  SECTION("HLT + no standardise + partial lookahead + no save") { \
    var.strategy(policy::strategy::hlt)                           \
        .standardize(false)                                       \
        .lookahead(policy::lookahead::partial)                    \
        .save(false);                                             \
  }                                                               \
  SECTION("HLT + standardise + partial lookahead + no save") {    \
    var.strategy(policy::strategy::hlt)                           \
        .standardize(true)                                        \
        .lookahead(policy::lookahead::partial)                    \
        .save(false);                                             \
  }

#define TEST_HLT_SAVE(var)                                     \
  SECTION("HLT + no standardise + full lookahead + save") {    \
    var.strategy(policy::strategy::hlt)                        \
        .standardize(false)                                    \
        .lookahead(policy::lookahead::full)                    \
        .save(true);                                           \
  }                                                            \
  SECTION("HLT + standardise + full lookahead + save") {       \
    var.strategy(policy::strategy::hlt)                        \
        .standardize(true)                                     \
        .lookahead(policy::lookahead::full)                    \
        .save(true);                                           \
  }                                                            \
  SECTION("HLT + no standardise + partial lookahead + save") { \
    var.strategy(policy::strategy::hlt)                        \
        .standardize(false)                                    \
        .lookahead(policy::lookahead::partial)                 \
        .save(true);                                           \
  }                                                            \
  SECTION("HLT + standardise + partial lookahead + save") {    \
    var.strategy(policy::strategy::hlt)                        \
        .standardize(true)                                     \
        .lookahead(policy::lookahead::partial)                 \
        .save(true);                                           \
  }

#define TEST_HLT_SAVE_THROWS(var)                                     \
  SECTION("HLT + save (throws)") {                                    \
    REQUIRE_THROWS_AS(var.strategy(policy::strategy::hlt).save(true), \
                      LibsemigroupsException);                        \
  }

#define TEST_HLT(var) TEST_HLT_NO_SAVE(var) TEST_HLT_SAVE(var)

#define TEST_RANDOM_SIMS(var)               \
  SECTION("random Sims strategy") {         \
    var.strategy(policy::strategy::random); \
  }

namespace libsemigroups {
  struct LibsemigroupsException;  // Forward declaration

  constexpr bool REPORT              = false;
  congruence_type constexpr twosided = congruence_type::twosided;
  congruence_type constexpr left     = congruence_type::left;
  congruence_type constexpr right    = congruence_type::right;
  using KnuthBendix                  = fpsemigroup::KnuthBendix;
  using tc_order                     = congruence::ToddCoxeter::order;
  using policy                       = congruence::ToddCoxeter::policy;

  template <typename TElementType>
  void delete_gens(std::vector<TElementType>& gens) {
    for (auto x : gens) {
      delete x;
    }
  }

  // This is how the recursive words up to a given length M, and on an
  // arbitrary finite alphabet are generated.  On a single letter alphabet,
  // this order is just increasing powers of the only generator:
  //
  //   a < aa < aaa < aaaa < ... < aa...a (M times)
  //
  // With an n-letter alphabet A = {a_1, a_2, ..., a_n}, suppose we have
  // already obtained all of the words W_{n - 1} containing {a_1, ..., a_{n -
  // 1}}.  Every word in W_{n - 1} is less than any word containing a_n, and
  // the least word greater than every word in W_{n - 1} is a_n. Words greater
  // than a_n are obtain in the follow way, where:
  //
  // x: is the maximum word in W_{n - 1}, this is constant, in the description
  //    that follows.
  // u: the first word obtained in point (1), the first time it is applied
  //    after (2) has been applied, starting with u = a_{n - 1}.
  // v: a word with one fewer letters than u, starting with the empty word.
  // w: a word such that w < u, also starting with the empty word.
  //
  // 1. If v < x, then v is replaced by the next word in the order. If |uv| <=
  //    M, then the next word is uv. Otherwise, goto 1.
  //
  // 2. If v = x, then and there exists a word w' in the set of words obtained
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
  // After defining baa < baaa, x = aaaa, u = b, v = aaaa, and w = e. Hence v =
  // x, and so (2) applies. The next w' in the set of words so far enumerated
  // is a, and |a| = 1 <= 3 = M - 1, and so w <- a, u <- ab, v <- e, and the
  // next word is ab. We repeatedly apply (1), until it fails, to obtain
  //
  //   baaa < ab < aba < abaa
  //
  // At which point u = b, v = aaaa = x, and w = a. Hence (2) applies, w <- aa,
  // v <- e, u <- aab, and the next word is: aab. And so on ...
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
        } while (static_cast<size_t>(w) < out.size() && out[w].size() + 1 > M);
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

  namespace congruence {
    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "000",
                            "small 2-sided congruence",
                            "[todd-coxeter][quick]") {
      auto rg = ReportGuard(REPORT);

      ToddCoxeter tc(twosided);
      tc.set_nr_generators(2);
      tc.add_pair({0, 0, 0}, {0});
      tc.add_pair({1, 1, 1, 1}, {1});
      tc.add_pair({0, 1, 0, 1}, {0, 0});

      TEST_HLT(tc);
      TEST_FELSCH(tc);
      TEST_RANDOM_SIMS(tc);

      REQUIRE(!tc.finished());
      REQUIRE(tc.nr_classes() == 27);
      // Too small for lookahead to kick in...
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "001",
                            "small 2-sided congruence",
                            "[no-valgrind][todd-coxeter][quick]") {
      auto rg = ReportGuard(REPORT);

      ToddCoxeter tc(twosided);
      tc.set_nr_generators(2);
      tc.add_pair({0, 0, 0}, {0});  // (a^3, a)
      tc.add_pair({0}, {1, 1});     // (a, b^2)

      TEST_HLT(tc);
      TEST_FELSCH(tc);
      TEST_RANDOM_SIMS(tc);

      REQUIRE(!tc.finished());
      REQUIRE(tc.nr_classes() == 5);
      REQUIRE(tc.finished());

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
      auto rg = ReportGuard(REPORT);

      ToddCoxeter tc(twosided);
      tc.set_nr_generators(4);
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
      TEST_HLT(tc);
      TEST_FELSCH(tc);
      TEST_RANDOM_SIMS(tc);

      REQUIRE(tc.nr_classes() == 10752);
      REQUIRE(tc.complete());
      REQUIRE(tc.compatible());

      auto& S = *tc.quotient_froidure_pin();
      REQUIRE(S.size() == 10752);
      REQUIRE(S.nr_idempotents() == 1);
      for (size_t c = 0; c < tc.nr_classes(); ++c) {
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
      for (size_t c = 0; c < tc.nr_classes(); ++c) {
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
      for (size_t c = 0; c < tc.nr_classes(); ++c) {
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
                            "[no-valgrind][todd-coxeter][quick]") {
      auto rg = ReportGuard(REPORT);

      FroidurePin<BMat8> S(
          {BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
           BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
           BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {1, 0, 0, 1}}),
           BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}})});

      ToddCoxeter tc(twosided, S);
      tc.froidure_pin_policy(policy::froidure_pin::use_relations);
      tc.add_pair({0}, {1});

      TEST_FELSCH(tc);
      TEST_HLT(tc);
      TEST_RANDOM_SIMS(tc);

      tc.random_interval(std::chrono::milliseconds(100));
      tc.lower_bound(3);

      // Processing deductions in HLT in this example leads to very poor
      // performance . . .
      REQUIRE(tc.nr_classes() == 3);
      REQUIRE(tc.nr_generators() == 4);
      REQUIRE(tc.contains({0}, {1}));
      tc.standardize(tc_order::shortlex);

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

      using Transf = TransfHelper<5>::type;
      FroidurePin<Transf> S({Transf({1, 3, 4, 2, 3}), Transf({3, 2, 1, 3, 3})});

      REQUIRE(S.size() == 88);

      ToddCoxeter tc(twosided, S);
      tc.froidure_pin_policy(policy::froidure_pin::use_cayley_graph);
      tc.add_pair(S.factorisation(Transf({3, 4, 4, 4, 4})),
                  S.factorisation(Transf({3, 1, 3, 3, 3})));
      REQUIRE(!tc.finished());
      tc.shrink_to_fit();  // does nothing
      REQUIRE(!tc.finished());
      tc.standardize(tc_order::none);  // does nothing
      REQUIRE(!tc.finished());

      TEST_HLT_NO_SAVE(tc);
      TEST_HLT_SAVE_THROWS(tc);
      TEST_FELSCH_THROWS(tc);
      TEST_RANDOM_SIMS(tc);

      REQUIRE(tc.nr_classes() == 21);
      tc.shrink_to_fit();
      REQUIRE(tc.nr_classes() == 21);
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
          [&tc](word_type const& w) -> bool {
            return tc.class_index_to_word(tc.word_to_class_index(w)) == w;
          }));
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "005",
                            "non-trivial two-sided from relations",
                            "[todd-coxeter][quick]") {
      auto        rg = ReportGuard(REPORT);
      ToddCoxeter tc(twosided);
      tc.set_nr_generators(3);
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

      TEST_HLT(tc);
      TEST_FELSCH(tc);
      TEST_RANDOM_SIMS(tc);

      REQUIRE(tc.nr_classes() == 2);
      tc.standardize(tc_order::shortlex);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "006",
                            "small right cong. on free semigroup",
                            "[todd-coxeter][quick]") {
      auto rg = ReportGuard(REPORT);

      ToddCoxeter tc(right);
      tc.set_nr_generators(2);
      tc.add_pair({0, 0, 0}, {0});
      tc.add_pair({0}, {1, 1});

      TEST_HLT(tc);
      TEST_FELSCH(tc);
      TEST_RANDOM_SIMS(tc);

      REQUIRE(tc.nr_classes() == 5);
      REQUIRE(tc.finished());
      tc.standardize(tc_order::shortlex);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "007",
                            "left cong. on free semigroup",
                            "[todd-coxeter][quick]") {
      auto rg = ReportGuard(REPORT);

      ToddCoxeter tc(left);
      tc.set_nr_generators(2);
      tc.add_pair({0, 0, 0}, {0});
      tc.add_pair({0}, {1, 1});

      TEST_HLT(tc);
      TEST_FELSCH(tc);
      TEST_RANDOM_SIMS(tc);

      REQUIRE(tc.word_to_class_index({0, 0, 1})
              == tc.word_to_class_index({0, 0, 0, 0, 1}));
      REQUIRE(tc.word_to_class_index({0, 1, 1, 0, 0, 1})
              == tc.word_to_class_index({0, 0, 0, 0, 1}));
      REQUIRE(tc.word_to_class_index({1})
              != tc.word_to_class_index({0, 0, 0, 0}));
      REQUIRE(tc.word_to_class_index({0, 0, 0})
              != tc.word_to_class_index({0, 0, 0, 0}));
      tc.standardize(tc_order::shortlex);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "008",
                            "for small fp semigroup",
                            "[todd-coxeter][quick]") {
      auto        rg = ReportGuard(REPORT);
      ToddCoxeter tc(twosided);
      tc.set_nr_generators(2);
      tc.add_pair({0, 0, 0}, {0});  // (a^3, a)
      tc.add_pair({0}, {1, 1});     // (a, b^2)

      TEST_HLT(tc);
      TEST_FELSCH(tc);
      TEST_RANDOM_SIMS(tc);

      REQUIRE(tc.word_to_class_index({0, 0, 1})
              == tc.word_to_class_index({0, 0, 0, 0, 1}));
      REQUIRE(tc.word_to_class_index({0, 1, 1, 0, 0, 1})
              == tc.word_to_class_index({0, 0, 0, 0, 1}));
      REQUIRE(tc.word_to_class_index({0, 0, 0}) != tc.word_to_class_index({1}));
      REQUIRE(tc.word_to_class_index({0, 0, 0, 0}) < tc.nr_classes());
      tc.standardize(tc_order::shortlex);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "009",
                            "2-sided cong. trans. semigroup",
                            "[todd-coxeter][quick]") {
      auto rg = ReportGuard(REPORT);
      auto S  = FroidurePin<Transformation<uint16_t>>(
          {Transformation<uint16_t>({1, 3, 4, 2, 3}),
           Transformation<uint16_t>({3, 2, 1, 3, 3})});

      REQUIRE(S.size() == 88);
      REQUIRE(S.nr_rules() == 18);

      ToddCoxeter tc(twosided, S);
      tc.froidure_pin_policy(policy::froidure_pin::use_relations);
      tc.add_pair(S.factorisation(Transformation<uint16_t>({3, 4, 4, 4, 4})),
                  S.factorisation(Transformation<uint16_t>({3, 1, 3, 3, 3})));

      TEST_HLT(tc);
      TEST_FELSCH(tc);
      TEST_RANDOM_SIMS(tc);

      REQUIRE(tc.nr_classes() == 21);
      REQUIRE(tc.nr_classes() == 21);

      REQUIRE(tc.word_to_class_index(
                  S.factorisation(Transformation<uint16_t>({1, 3, 1, 3, 3})))
              == tc.word_to_class_index(
                  S.factorisation(Transformation<uint16_t>({4, 2, 4, 4, 2}))));

      tc.standardize(tc_order::shortlex);
      REQUIRE(tc.nr_non_trivial_classes() == 1);
      REQUIRE(tc.cbegin_ntc()->size() == 68);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "010",
                            "left congruence on transformation semigroup",
                            "[todd-coxeter][quick]") {
      auto rg = ReportGuard(REPORT);
      auto S  = FroidurePin<Transformation<uint16_t>>(
          {Transformation<uint16_t>({1, 3, 4, 2, 3}),
           Transformation<uint16_t>({3, 2, 1, 3, 3})});

      REQUIRE(S.size() == 88);
      REQUIRE(S.nr_rules() == 18);

      ToddCoxeter tc(left, S);
      tc.froidure_pin_policy(policy::froidure_pin::use_relations);
      tc.add_pair(S.factorisation(Transformation<uint16_t>({3, 4, 4, 4, 4})),
                  S.factorisation(Transformation<uint16_t>({3, 1, 3, 3, 3})));

      TEST_HLT(tc);
      TEST_FELSCH(tc);
      TEST_RANDOM_SIMS(tc);

      REQUIRE(tc.nr_classes() == 69);
      REQUIRE(tc.nr_classes() == 69);

      REQUIRE(tc.word_to_class_index(
                  S.factorisation(Transformation<uint16_t>({1, 3, 1, 3, 3})))
              != tc.word_to_class_index(
                  S.factorisation(Transformation<uint16_t>({4, 2, 4, 4, 2}))));

      tc.standardize(tc_order::shortlex);
      REQUIRE(tc.nr_non_trivial_classes() == 1);
      REQUIRE(tc.cbegin_ntc()->size() == 20);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "011",
                            "right cong. trans. semigroup",
                            "[todd-coxeter][quick]") {
      auto rg = ReportGuard(REPORT);
      auto S  = FroidurePin<Transformation<uint16_t>>(
          {Transformation<uint16_t>({1, 3, 4, 2, 3}),
           Transformation<uint16_t>({3, 2, 1, 3, 3})});

      REQUIRE(S.size() == 88);
      REQUIRE(S.nr_rules() == 18);

      ToddCoxeter tc(right, S);
      tc.froidure_pin_policy(policy::froidure_pin::use_relations);
      tc.add_pair(S.factorisation(Transformation<uint16_t>({3, 4, 4, 4, 4})),
                  S.factorisation(Transformation<uint16_t>({3, 1, 3, 3, 3})));

      TEST_HLT(tc);
      TEST_FELSCH(tc);
      TEST_RANDOM_SIMS(tc);

      REQUIRE(tc.nr_classes() == 72);
      REQUIRE(tc.nr_classes() == 72);

      REQUIRE(tc.word_to_class_index(
                  S.factorisation(Transformation<uint16_t>({1, 3, 1, 3, 3})))
              != tc.word_to_class_index(
                  S.factorisation(Transformation<uint16_t>({4, 2, 4, 4, 2}))));

      REQUIRE(tc.word_to_class_index(
                  S.factorisation(Transformation<uint16_t>({1, 3, 3, 3, 3})))
              != tc.word_to_class_index(
                  S.factorisation(Transformation<uint16_t>({4, 2, 4, 4, 2}))));
      REQUIRE(tc.word_to_class_index(
                  S.factorisation(Transformation<uint16_t>({2, 4, 2, 2, 2})))
              == tc.word_to_class_index(
                  S.factorisation(Transformation<uint16_t>({2, 3, 3, 3, 3}))));
      REQUIRE(tc.word_to_class_index(
                  S.factorisation(Transformation<uint16_t>({1, 3, 3, 3, 3})))
              != tc.word_to_class_index(
                  S.factorisation(Transformation<uint16_t>({2, 3, 3, 3, 3}))));

      tc.standardize(tc_order::shortlex);
      REQUIRE(tc.nr_non_trivial_classes() == 4);

      std::vector<size_t> v(tc.nr_non_trivial_classes(), 0);
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

      std::vector<Element*> gens
          = {new Transformation<uint16_t>({1, 3, 4, 2, 3}),
             new Transformation<uint16_t>({3, 2, 1, 3, 3})};
      FroidurePin<Element const*> S = FroidurePin<Element const*>(gens);
      delete_gens(gens);

      REQUIRE(S.size() == 88);
      REQUIRE(S.nr_rules() == 18);
      REQUIRE(S.degree() == 5);

      ToddCoxeter tc(twosided, S);
      tc.froidure_pin_policy(policy::froidure_pin::use_cayley_graph);

      Element*  t1 = new Transformation<uint16_t>({3, 4, 4, 4, 4});
      Element*  t2 = new Transformation<uint16_t>({3, 1, 3, 3, 3});
      word_type w1, w2;
      S.factorisation(w1, S.position(t1));
      S.factorisation(w2, S.position(t2));

      tc.add_pair(w1, w2);

      TEST_HLT_NO_SAVE(tc);
      TEST_HLT_SAVE_THROWS(tc);
      TEST_FELSCH_THROWS(tc);
      TEST_RANDOM_SIMS(tc);

      REQUIRE(tc.nr_classes() == 21);
      REQUIRE(tc.nr_classes() == 21);
      Element*  t3 = new Transformation<uint16_t>({1, 3, 1, 3, 3});
      Element*  t4 = new Transformation<uint16_t>({4, 2, 4, 4, 2});
      word_type w3, w4;
      S.factorisation(w3, S.position(t3));
      S.factorisation(w4, S.position(t4));
      REQUIRE(tc.word_to_class_index(w3) == tc.word_to_class_index(w4));

      delete t1, delete t2, delete t3, delete t4;
      tc.standardize(tc_order::shortlex);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "013",
                            "left cong. on trans. semigroup (size 88)",
                            "[todd-coxeter][quick]") {
      std::vector<Element*> gens
          = {new Transformation<uint16_t>({1, 3, 4, 2, 3}),
             new Transformation<uint16_t>({3, 2, 1, 3, 3})};
      FroidurePin<Element const*> S  = FroidurePin<Element const*>(gens);
      auto                        rg = ReportGuard(REPORT);
      delete_gens(gens);

      REQUIRE(S.size() == 88);
      REQUIRE(S.degree() == 5);
      Element*  t1 = new Transformation<uint16_t>({3, 4, 4, 4, 4});
      Element*  t2 = new Transformation<uint16_t>({3, 1, 3, 3, 3});
      word_type w1, w2;
      S.factorisation(w1, S.position(t1));
      S.factorisation(w2, S.position(t2));
      ToddCoxeter tc(left, S);
      tc.froidure_pin_policy(policy::froidure_pin::use_relations);
      tc.add_pair(w1, w2);
      delete t1, delete t2;

      TEST_HLT(tc);
      TEST_FELSCH(tc);
      TEST_RANDOM_SIMS(tc);

      REQUIRE(tc.nr_classes() == 69);
      REQUIRE(tc.nr_classes() == 69);
      tc.standardize(tc_order::shortlex);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "014",
                            "right cong. on trans. semigroup (size 88)",
                            "[todd-coxeter][quick]") {
      std::vector<Element*> gens
          = {new Transformation<uint16_t>({1, 3, 4, 2, 3}),
             new Transformation<uint16_t>({3, 2, 1, 3, 3})};
      FroidurePin<Element const*> S  = FroidurePin<Element const*>(gens);
      auto                        rg = ReportGuard(REPORT);
      delete_gens(gens);

      REQUIRE(S.size() == 88);
      REQUIRE(S.nr_rules() == 18);
      REQUIRE(S.degree() == 5);
      Element*  t1 = new Transformation<uint16_t>({3, 4, 4, 4, 4});
      Element*  t2 = new Transformation<uint16_t>({3, 1, 3, 3, 3});
      word_type w1, w2;
      S.factorisation(w1, S.position(t1));
      S.factorisation(w2, S.position(t2));
      ToddCoxeter tc(right, S);
      tc.froidure_pin_policy(policy::froidure_pin::use_relations);
      tc.add_pair(w1, w2);

      TEST_HLT(tc);
      TEST_FELSCH(tc);
      TEST_RANDOM_SIMS(tc);

      REQUIRE(tc.nr_classes() == 72);
      REQUIRE(tc.nr_classes() == 72);
      Element*  t3 = new Transformation<uint16_t>({1, 3, 3, 3, 3});
      Element*  t4 = new Transformation<uint16_t>({4, 2, 4, 4, 2});
      Element*  t5 = new Transformation<uint16_t>({2, 4, 2, 2, 2});
      Element*  t6 = new Transformation<uint16_t>({2, 3, 3, 3, 3});
      word_type w3, w4, w5, w6;
      S.factorisation(w3, S.position(t3));
      S.factorisation(w4, S.position(t4));
      S.factorisation(w5, S.position(t5));
      S.factorisation(w6, S.position(t6));
      REQUIRE(tc.word_to_class_index(w3) != tc.word_to_class_index(w4));
      REQUIRE(tc.word_to_class_index(w5) == tc.word_to_class_index(w6));
      REQUIRE(tc.word_to_class_index(w3) != tc.word_to_class_index(w6));
      delete t1, delete t2, delete t3, delete t4, delete t5, delete t6;
      tc.standardize(tc_order::shortlex);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "015",
                            "finite fp-semigroup, dihedral group of order 6",
                            "[todd-coxeter][quick]") {
      auto        rg = ReportGuard(REPORT);
      ToddCoxeter tc(twosided);
      tc.set_nr_generators(5);
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
      TEST_HLT(tc);
      TEST_FELSCH(tc);
      TEST_RANDOM_SIMS(tc);

      REQUIRE(tc.nr_classes() == 6);
      REQUIRE(tc.word_to_class_index({1}) == tc.word_to_class_index({2}));
      tc.standardize(tc_order::shortlex);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "016",
                            "finite fp-semigroup, size 16",
                            "[todd-coxeter][quick]") {
      auto        rg = ReportGuard(REPORT);
      ToddCoxeter tc(twosided);
      tc.set_nr_generators(4);
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

      TEST_HLT(tc);
      TEST_FELSCH(tc);
      TEST_RANDOM_SIMS(tc);

      REQUIRE(tc.nr_classes() == 16);
      REQUIRE(tc.word_to_class_index({2}) == tc.word_to_class_index({3}));
      tc.standardize(tc_order::shortlex);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "017",
                            "finite fp-semigroup, size 16",
                            "[todd-coxeter][quick]") {
      auto        rg = ReportGuard(REPORT);
      ToddCoxeter tc(twosided);
      tc.set_nr_generators(11);
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
      TEST_HLT(tc);
      TEST_FELSCH(tc);
      TEST_RANDOM_SIMS(tc);

      REQUIRE(tc.nr_classes() == 16);
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

    // ToddCoxeter 021 removed

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "018",
                            "test lookahead",
                            "[todd-coxeter][quick]") {
      auto rg = ReportGuard(REPORT);
      {
        ToddCoxeter tc(twosided);
        tc.set_nr_generators(2);
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

        TEST_HLT(tc);
        REQUIRE(tc.nr_classes() == 78);
        tc.standardize(tc_order::shortlex);
      }
      {
        ToddCoxeter tc(left);
        tc.set_nr_generators(2);
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

        TEST_HLT(tc);
        REQUIRE(tc.nr_classes() == 78);
        tc.standardize(tc_order::shortlex);
      }
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "019",
                            "non-trivial left cong. from semigroup",
                            "[todd-coxeter][quick]") {
      auto rg = ReportGuard(REPORT);

      std::vector<Element*> gens
          = {new Transformation<uint16_t>({1, 3, 4, 2, 3}),
             new Transformation<uint16_t>({3, 2, 1, 3, 3})};
      FroidurePin<Element const*> S = FroidurePin<Element const*>(gens);
      delete_gens(gens);

      REQUIRE(S.size() == 88);
      REQUIRE(S.degree() == 5);

      Element*  t1 = new Transformation<uint16_t>({3, 4, 4, 4, 4});
      Element*  t2 = new Transformation<uint16_t>({3, 1, 3, 3, 3});
      word_type w1, w2;
      S.factorisation(w1, S.position(t1));
      S.factorisation(w2, S.position(t2));
      delete t1, delete t2;

      ToddCoxeter tc(left, S);
      tc.froidure_pin_policy(policy::froidure_pin::use_cayley_graph);
      tc.add_pair(w1, w2);
      TEST_HLT_NO_SAVE(tc);
      TEST_HLT_SAVE_THROWS(tc);
      TEST_FELSCH_THROWS(tc);
      TEST_RANDOM_SIMS(tc);
      REQUIRE(tc.nr_classes() == 69);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "020",
                            "2-sided cong. on free semigroup",
                            "[todd-coxeter][quick]") {
      auto        rg = ReportGuard(REPORT);
      ToddCoxeter tc(twosided);
      tc.set_nr_generators(1);
      TEST_HLT(tc);
      TEST_FELSCH(tc);
      TEST_RANDOM_SIMS(tc);
      REQUIRE(tc.contains({0, 0}, {0, 0}));
      REQUIRE(!tc.contains({0, 0}, {0}));
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "021",
                            "calling run when obviously infinite",
                            "[todd-coxeter][quick]") {
      ToddCoxeter tc(twosided);
      tc.set_nr_generators(5);
      TEST_HLT(tc);
      TEST_FELSCH(tc);
      TEST_RANDOM_SIMS(tc);
      REQUIRE_THROWS_AS(tc.run(), LibsemigroupsException);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "022",
                            "Stellar S3",
                            "[todd-coxeter][quick][hivert]") {
      auto rg = ReportGuard(REPORT);

      congruence::ToddCoxeter tc(twosided);
      tc.set_nr_generators(4);
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

      TEST_HLT(tc);
      TEST_FELSCH(tc);
      TEST_RANDOM_SIMS(tc);

      REQUIRE(tc.nr_classes() == 34);
      REQUIRE(tc.quotient_froidure_pin()->size() == 34);
      using detail::TCE;
      auto& S = static_cast<FroidurePin<TCE>&>(*tc.quotient_froidure_pin());
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
      tc.set_nr_generators(2);
      tc.add_pair({0, 0, 0}, {0});  // (a^3, a)
      tc.add_pair({0}, {1, 1});     // (a, b^2)
      TEST_HLT(tc);
      TEST_FELSCH(tc);
      TEST_RANDOM_SIMS(tc);
      REQUIRE(tc.nr_classes() == 5);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "024",
                            "exceptions",
                            "[todd-coxeter][quick]") {
      auto rg = ReportGuard(REPORT);
      {
        congruence::ToddCoxeter tc1(left);
        tc1.set_nr_generators(2);
        tc1.add_pair({0, 0, 0}, {0});
        tc1.add_pair({0}, {1, 1});
        REQUIRE(tc1.nr_classes() == 5);

        REQUIRE_THROWS_AS(ToddCoxeter(right, tc1), LibsemigroupsException);
        REQUIRE_THROWS_AS(ToddCoxeter(twosided, tc1), LibsemigroupsException);

        ToddCoxeter tc2(left, tc1);
        REQUIRE(!tc1.contains({0}, {1}));
        tc2.add_pair({0}, {1});

        TEST_HLT(tc2);
        TEST_FELSCH(tc2);
        TEST_RANDOM_SIMS(tc2);

        REQUIRE(tc2.nr_classes() == 1);

        ToddCoxeter tc3(left);
        tc3.set_nr_generators(2);
        tc3.add_pair({0, 0, 0}, {0});
        tc3.add_pair({0}, {1, 1});
        tc3.add_pair({0}, {1});
        REQUIRE(tc3.nr_classes() == 1);
      }
      {
        congruence::ToddCoxeter tc1(right);
        tc1.set_nr_generators(2);
        tc1.add_pair({0, 0, 0}, {0});
        tc1.add_pair({0}, {1, 1});
        REQUIRE(tc1.nr_classes() == 5);

        REQUIRE_THROWS_AS(ToddCoxeter(left, tc1), LibsemigroupsException);
        REQUIRE_THROWS_AS(ToddCoxeter(twosided, tc1), LibsemigroupsException);

        ToddCoxeter tc2(right, tc1);
        REQUIRE(!tc1.contains({0}, {1}));
        tc2.add_pair({0}, {1});

        TEST_HLT(tc2);
        TEST_FELSCH(tc2);
        TEST_RANDOM_SIMS(tc2);

        REQUIRE(tc2.nr_classes() == 1);

        ToddCoxeter tc3(right);
        tc3.set_nr_generators(2);
        tc3.add_pair({0, 0, 0}, {0});
        tc3.add_pair({0}, {1, 1});
        tc3.add_pair({0}, {1});
        REQUIRE(tc3.nr_classes() == 1);
      }
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "025",
                            "obviously infinite",
                            "[todd-coxeter][quick]") {
      auto rg = ReportGuard(REPORT);
      {
        congruence::ToddCoxeter tc(left);
        tc.set_nr_generators(3);
        tc.add_pair({0, 0, 0}, {0});
        TEST_HLT(tc);
        TEST_FELSCH(tc);
        TEST_RANDOM_SIMS(tc);
        REQUIRE(tc.nr_classes() == POSITIVE_INFINITY);
        REQUIRE(!tc.is_quotient_obviously_finite());
      }
      {
        congruence::ToddCoxeter tc(right);
        tc.set_nr_generators(3);
        tc.add_pair({0, 0, 0}, {0});
        TEST_HLT(tc);
        TEST_FELSCH(tc);
        TEST_RANDOM_SIMS(tc);
        REQUIRE(tc.nr_classes() == POSITIVE_INFINITY);
        REQUIRE(!tc.is_quotient_obviously_finite());
      }
      {
        congruence::ToddCoxeter tc(twosided);
        tc.set_nr_generators(3);
        tc.add_pair({0, 0, 0}, {0});
        TEST_HLT(tc);
        TEST_FELSCH(tc);
        TEST_RANDOM_SIMS(tc);
        REQUIRE(tc.nr_classes() == POSITIVE_INFINITY);
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
        tc.set_nr_generators(2);
        tc.add_pair({0, 0, 0}, {0});
        tc.add_pair({0}, {1, 1});
        TEST_HLT(tc);
        TEST_FELSCH(tc);
        REQUIRE(tc.nr_classes() == 5);
        REQUIRE(tc.class_index_to_word(0) == word_type({0}));
        // This next one should throw
        REQUIRE_THROWS_AS(tc.quotient_froidure_pin(), LibsemigroupsException);
      }
      {
        congruence::ToddCoxeter tc(twosided);
        tc.set_nr_generators(2);
        TEST_HLT(tc);
        TEST_FELSCH(tc);
        TEST_RANDOM_SIMS(tc);
        tc.add_pair({0, 0, 0}, {0});
        tc.add_pair({0}, {1, 1});
        REQUIRE(tc.nr_classes() == 5);
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
        tc.set_nr_generators(3);
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
        REQUIRE_THROWS_AS(tc2.add_pair({0}, {2}), LibsemigroupsException);
        TEST_HLT_NO_SAVE(tc2);
        TEST_HLT_SAVE_THROWS(tc2);
        TEST_FELSCH_THROWS(tc2);
        TEST_RANDOM_SIMS(tc2);
        tc2.add_pair({0}, {1});
        REQUIRE(tc2.nr_classes() == 1);
      }
      {
        fpsemigroup::ToddCoxeter tc1;
        tc1.set_alphabet("ab");
        tc1.add_rule("aaa", "a");
        tc1.add_rule("a", "bb");
        congruence::ToddCoxeter tc2(left, tc1);
        TEST_HLT(tc2);
        TEST_FELSCH(tc2);
        TEST_RANDOM_SIMS(tc2);
        REQUIRE(!tc2.empty());
        REQUIRE_THROWS_AS(tc2.add_pair({0}, {2}), LibsemigroupsException);
        tc2.add_pair({0}, {1});
        REQUIRE(tc2.nr_classes() == 1);
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
        tc = detail::make_unique<ToddCoxeter>(twosided, kb);
        TEST_HLT((*tc));
        TEST_FELSCH((*tc));
        TEST_RANDOM_SIMS((*tc));
      }
      SECTION("left") {
        tc = detail::make_unique<ToddCoxeter>(left, kb);
        TEST_HLT((*tc));
        TEST_FELSCH((*tc));
        TEST_RANDOM_SIMS((*tc));
      }
      SECTION("right") {
        tc = detail::make_unique<ToddCoxeter>(left, kb);
        TEST_HLT((*tc));
        TEST_FELSCH((*tc));
        TEST_RANDOM_SIMS((*tc));
      }
      REQUIRE(!tc->has_parent_froidure_pin());
      tc->add_pair({1}, {2});
      REQUIRE(tc->is_quotient_obviously_infinite());
      REQUIRE(tc->nr_classes() == POSITIVE_INFINITY);
      REQUIRE(std::vector<relation_type>(tc->cbegin_generating_pairs(),
                                         tc->cend_generating_pairs())
              == std::vector<relation_type>(
                  {{{1, 1}, {2}}, {{2, 0, 2}, {0, 1, 0}}, {{1}, {2}}}));
      REQUIRE(!tc->finished());
      REQUIRE(!tc->started());
      tc->add_pair({1}, {0});
      REQUIRE(!tc->is_quotient_obviously_infinite());
      REQUIRE(tc->nr_classes() == 1);
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
      REQUIRE(kb.nr_active_rules() == 6);
      REQUIRE(kb.finished());

      std::unique_ptr<ToddCoxeter> tc = nullptr;
      SECTION("2-sided") {
        tc = detail::make_unique<ToddCoxeter>(twosided, kb);
        TEST_HLT((*tc));
        TEST_FELSCH((*tc));
        TEST_RANDOM_SIMS((*tc));
      }
      SECTION("left") {
        tc = detail::make_unique<ToddCoxeter>(left, kb);
        TEST_HLT((*tc));
        TEST_FELSCH((*tc));
        TEST_RANDOM_SIMS((*tc));
      }
      SECTION("right") {
        tc = detail::make_unique<ToddCoxeter>(right, kb);
        TEST_HLT((*tc));
        TEST_FELSCH((*tc));
        TEST_RANDOM_SIMS((*tc));
      }
      REQUIRE(tc->has_parent_froidure_pin());
      tc->add_pair({1}, {2});
      REQUIRE(tc->is_quotient_obviously_infinite());
      REQUIRE(tc->nr_classes() == POSITIVE_INFINITY);
      REQUIRE(std::vector<relation_type>(tc->cbegin_generating_pairs(),
                                         tc->cend_generating_pairs())
              == std::vector<relation_type>(
                  {{{1, 1}, {2}}, {{2, 0, 2}, {0, 1, 0}}, {{1}, {2}}}));
      tc->add_pair({1}, {0});
      REQUIRE(!tc->is_quotient_obviously_infinite());
      REQUIRE(tc->nr_classes() == 1);
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
      REQUIRE(kb.nr_active_rules() == 3);
      REQUIRE(kb.size() == 1);
      REQUIRE(kb.is_obviously_finite());
      REQUIRE(kb.finished());

      std::unique_ptr<ToddCoxeter> tc = nullptr;
      SECTION("2-sided") {
        tc = detail::make_unique<ToddCoxeter>(twosided, kb);
        TEST_HLT_NO_SAVE((*tc));
        TEST_FELSCH_THROWS((*tc));
        TEST_HLT_SAVE_THROWS((*tc));
        TEST_RANDOM_SIMS((*tc));
      }
      SECTION("left") {
        tc = detail::make_unique<ToddCoxeter>(left, kb);
        TEST_HLT_NO_SAVE((*tc));
        TEST_FELSCH_THROWS((*tc));
        TEST_HLT_SAVE_THROWS((*tc));
        TEST_RANDOM_SIMS((*tc));
      }
      SECTION("right") {
        tc = detail::make_unique<ToddCoxeter>(left, kb);
        TEST_HLT_NO_SAVE((*tc));
        TEST_FELSCH_THROWS((*tc));
        TEST_HLT_SAVE_THROWS((*tc));
        TEST_RANDOM_SIMS((*tc));
      }
      REQUIRE(tc->has_parent_froidure_pin());
      tc->add_pair({1}, {2});

      REQUIRE(tc->nr_classes() == 1);
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
      REQUIRE(rv.nr_cols() == 2);
      REQUIRE(rv.nr_rows() == 1);
      {
        ToddCoxeter tc(twosided);
        // prefill before nr_generators are set
        REQUIRE_THROWS_AS(tc.prefill(rv), LibsemigroupsException);
        tc.set_nr_generators(3);
        // prefill where nr_generators != nr_cols of rv
        REQUIRE_THROWS_AS(tc.prefill(rv), LibsemigroupsException);
      }
      {
        ToddCoxeter tc(left);
        tc.set_nr_generators(2);
        rv.set(0, 0, 0);
        rv.set(0, 1, 1);
        // prefill with too few rows
        REQUIRE_THROWS_AS(tc.prefill(rv), LibsemigroupsException);
        rv.add_rows(1);
        REQUIRE(rv.nr_rows() == 2);
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
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "033",
                            "congruence of ToddCoxeter",
                            "[todd-coxeter][quick]") {
      auto        rg = ReportGuard(REPORT);
      ToddCoxeter tc1(twosided);
      tc1.set_nr_generators(2);
      tc1.add_pair({0, 0, 0}, {0});
      tc1.add_pair({0}, {1, 1});
      REQUIRE(tc1.nr_classes() == 5);
      ToddCoxeter tc2(left, tc1);
      tc2.next_lookahead(1);
      tc2.report_every(1);
      REQUIRE(!tc2.empty());
      TEST_HLT(tc2);
      TEST_RANDOM_SIMS(tc2);
      tc2.add_pair({0}, {0, 0});
      REQUIRE(tc2.nr_classes() == 3);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "034",
                            "congruence of ToddCoxeter",
                            "[todd-coxeter][quick]") {
      auto rg      = ReportGuard(REPORT);
      using Transf = TransfHelper<5>::type;
      FroidurePin<Transf> S({Transf({1, 3, 4, 2, 3}), Transf({3, 2, 1, 3, 3})});
      REQUIRE(S.size() == 88);
      REQUIRE(S.nr_rules() == 18);
      ToddCoxeter tc(twosided, S);
      tc.froidure_pin_policy(policy::froidure_pin::none);
      tc.set_nr_generators(2);
      TEST_HLT_NO_SAVE(tc);
      TEST_HLT_SAVE_THROWS(tc);
      TEST_FELSCH_THROWS(tc);
      TEST_RANDOM_SIMS(tc);
      tc.add_pair({0}, {1, 1});
      REQUIRE(tc.nr_classes() == 1);
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
      REQUIRE(S.knuth_bendix()->nr_rules() == 13);

      ToddCoxeter tc(left, *S.knuth_bendix());
      tc.add_pair({0}, {1, 1, 1});
      TEST_HLT(tc);
      TEST_FELSCH(tc);
      TEST_RANDOM_SIMS(tc);
      REQUIRE(tc.nr_classes() == 2);
      REQUIRE(std::vector<word_type>(tc.cbegin_normal_forms(),
                                     tc.cend_normal_forms())
              == std::vector<word_type>({{0}, {2}}));
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "036",
                            "exceptions",
                            "[todd-coxeter][quick]") {
      auto rg      = ReportGuard(REPORT);
      using Transf = TransfHelper<5>::type;
      FroidurePin<Transf> S({Transf({1, 3, 4, 2, 3}), Transf({3, 2, 1, 3, 3})});
      ToddCoxeter         tc(twosided);
      tc.set_nr_generators(2);
      tc.add_pair({0}, {1});
      tc.add_pair({0, 0}, {0});
      REQUIRE(tc.nr_classes() == 1);
      REQUIRE_THROWS_AS(tc.prefill(S.right_cayley_graph()),
                        LibsemigroupsException);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "096",
                            "copy constructor",
                            "[todd-coxeter][quick]") {
      auto        rg = ReportGuard(REPORT);
      ToddCoxeter tc(twosided);
      tc.set_nr_generators(2);
      tc.add_pair({0}, {1});
      tc.add_pair({0, 0}, {0});
      tc.strategy(policy::strategy::felsch);
      REQUIRE(tc.strategy() == policy::strategy::felsch);
      REQUIRE(!tc.complete());
      REQUIRE(tc.compatible());
      REQUIRE(tc.nr_classes() == 1);
      REQUIRE(std::vector<word_type>(tc.cbegin_normal_forms(),
                                     tc.cend_normal_forms())
              == std::vector<word_type>(1, {0}));
      REQUIRE(tc.complete());
      REQUIRE(tc.compatible());

      ToddCoxeter copy(tc);
      REQUIRE(copy.nr_generators() == 2);
      REQUIRE(copy.nr_generating_pairs() == 2);
      REQUIRE(copy.finished());
      REQUIRE(copy.nr_classes() == 1);
      REQUIRE(copy.froidure_pin_policy() == policy::froidure_pin::none);
      REQUIRE(copy.complete());
      REQUIRE(copy.compatible());
    }
  }  // namespace congruence

  namespace fpsemigroup {
    constexpr bool REPORT = false;

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "037",
                            "add_rule",
                            "[todd-coxeter][quick]") {
      auto rg = ReportGuard(REPORT);
      {
        ToddCoxeter tc;
        tc.set_alphabet("ab");
        tc.add_rule("aaa", "a");
        tc.add_rule("a", "bb");
        TEST_HLT(tc.congruence());
        TEST_FELSCH(tc.congruence());
        TEST_RANDOM_SIMS(tc.congruence());
        REQUIRE(tc.size() == 5);
      }
      {
        ToddCoxeter tc;
        tc.set_alphabet("ab");
        tc.add_rule("aaa", "a");
        tc.add_rule("a", "bb");
        tc.congruence().next_lookahead(1);
        TEST_HLT(tc.congruence());
        TEST_FELSCH(tc.congruence());
        TEST_RANDOM_SIMS(tc.congruence());
        REQUIRE(tc.size() == 5);
      }
    }

    // KnuthBendix methods fail for this one
    LIBSEMIGROUPS_TEST_CASE(
        "ToddCoxeter",
        "095",
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
      TEST_HLT(tc.congruence());
      TEST_FELSCH(tc.congruence());
      TEST_RANDOM_SIMS(tc.congruence());
      REQUIRE(tc.size() == 24);
      REQUIRE(tc.froidure_pin()->size() == 24);
      REQUIRE(tc.normal_form("aaaaaaaaaaaaaaaaaaa") == "a");
      REQUIRE(KnuthBendix(tc.froidure_pin()).confluent());
    }

    // Second of BHN's series of increasingly complicated presentations
    // of 1. Doesn't terminate
    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "038",
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

      REQUIRE(tc.size() == 1);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "039",
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
                            "040",
                            "add_rules after construct. from semigroup",
                            "[todd-coxeter][quick]") {
      auto rg = ReportGuard(REPORT);

      using Transf = TransfHelper<5>::type;

      FroidurePin<Transf> S({Transf({1, 3, 4, 2, 3}), Transf({3, 2, 1, 3, 3})});
      REQUIRE(S.size() == 88);
      REQUIRE(S.nr_rules() == 18);

      word_type w1, w2, w3, w4;
      S.factorisation(w1, S.position(Transf({3, 4, 4, 4, 4})));
      S.factorisation(w2, S.position(Transf({3, 1, 3, 3, 3})));
      S.factorisation(w3, S.position(Transf({1, 3, 1, 3, 3})));
      S.factorisation(w4, S.position(Transf({4, 2, 4, 4, 2})));

      ToddCoxeter tc1(S);
      tc1.add_rule(w1, w2);

      TEST_HLT_NO_SAVE(tc1.congruence());
      TEST_HLT_SAVE_THROWS(tc1.congruence());
      TEST_FELSCH_THROWS(tc1.congruence());
      TEST_RANDOM_SIMS(tc1.congruence());

      REQUIRE(tc1.size() == 21);
      REQUIRE(tc1.size() == tc1.froidure_pin()->size());
      REQUIRE(tc1.equal_to(w3, w4));
      REQUIRE(tc1.normal_form(w3) == tc1.normal_form(w4));

      ToddCoxeter tc2(S);
      tc2.add_rule(w1, w2);
      TEST_HLT_NO_SAVE(tc2.congruence());
      TEST_HLT_SAVE_THROWS(tc2.congruence());
      TEST_FELSCH_THROWS(tc2.congruence());

      REQUIRE(tc2.size() == 21);
      REQUIRE(tc2.size() == tc2.froidure_pin()->size());
      REQUIRE(tc2.equal_to(w3, w4));
      REQUIRE(tc2.normal_form(w3) == tc2.normal_form(w4));
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "041",
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
      TEST_HLT(tc.congruence());
      TEST_FELSCH(tc.congruence());
      TEST_RANDOM_SIMS(tc.congruence());
      REQUIRE(tc.size() == 120);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "042",
                            "Chapter 7, Theorem 3.6 in NR (size 243)",
                            "[no-valgrind][todd-coxeter][quick]") {
      auto        rg = ReportGuard(REPORT);
      ToddCoxeter tc;
      tc.set_alphabet("ab");
      tc.add_rule("aaa", "a");
      tc.add_rule("bbbb", "b");
      tc.add_rule("ababababab", "aa");
      TEST_HLT(tc.congruence());
      TEST_FELSCH(tc.congruence());
      TEST_RANDOM_SIMS(tc.congruence());
      REQUIRE(tc.size() == 243);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "043",
                            "finite semigroup (size 99)",
                            "[todd-coxeter][quick]") {
      auto        rg = ReportGuard(REPORT);
      ToddCoxeter tc;
      tc.set_alphabet("ab");
      tc.add_rule("aaa", "a");
      tc.add_rule("bbbb", "b");
      tc.add_rule("abababab", "aa");
      TEST_HLT(tc.congruence());
      TEST_FELSCH(tc.congruence());
      TEST_RANDOM_SIMS(tc.congruence());

      REQUIRE(!tc.is_obviously_finite());
      REQUIRE(tc.size() == 99);
      REQUIRE(tc.finished());
      REQUIRE(tc.is_obviously_finite());
    }

    // The following 8 examples are from Trevor Walker's Thesis: Semigroup
    // enumeration - computer implementation and applications, p41.
    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "044",
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

      tc.congruence().sort_generating_pairs(shortlex_compare);
      tc.congruence().next_lookahead(500000);
      tc.congruence().run_until([&tc]() -> bool {
        return tc.congruence().coset_capacity() >= 10000;
      });
      REQUIRE(!tc.finished());
      REQUIRE(!tc.is_obviously_finite());
      tc.congruence().standardize(tc_order::shortlex);
      REQUIRE(!tc.finished());
      tc.congruence().standardize(tc_order::lex);
      REQUIRE(!tc.finished());
      tc.congruence().standardize(tc_order::recursive);
      REQUIRE(!tc.finished());

      TEST_HLT(tc.congruence());
      TEST_FELSCH(tc.congruence());
      TEST_RANDOM_SIMS(tc.congruence());

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
                            "045",
                            "Walker 2",
                            "[todd-coxeter][extreme]") {
      auto        rg = ReportGuard();
      ToddCoxeter tc;
      tc.set_alphabet("ab");
      tc.add_rule("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", "a");
      tc.add_rule("bbb", "b");
      tc.add_rule("ababa", "b");
      tc.add_rule("aaaaaaaaaaaaaaaabaaaabaaaaaaaaaaaaaaaabaaaa", "b");

      tc.congruence().next_lookahead(2000000);

      REQUIRE(!tc.is_obviously_finite());

      TEST_HLT(tc.congruence());
      TEST_FELSCH(tc.congruence());
      TEST_RANDOM_SIMS(tc.congruence());

      // This takes approx 6 seconds with Felsch . . .
      REQUIRE(tc.size() == 14911);
      tc.congruence().standardize(tc_order::shortlex);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "046",
                            "Walker 3",
                            "[todd-coxeter][standard]") {
      auto        rg = ReportGuard(REPORT);
      ToddCoxeter tc;
      tc.set_alphabet("ab");
      tc.add_rule("aaaaaaaaaaaaaaaa", "a");
      tc.add_rule("bbbbbbbbbbbbbbbb", "b");
      tc.add_rule("abb", "baa");
      tc.congruence().next_lookahead(2000000);
      REQUIRE(!tc.is_obviously_finite());

      TEST_HLT(tc.congruence());
      TEST_FELSCH(tc.congruence());
      TEST_RANDOM_SIMS(tc.congruence());

      REQUIRE(tc.size() == 20490);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "047",
                            "Walker 4",
                            "[todd-coxeter][extreme]") {
      auto        rg = ReportGuard();
      ToddCoxeter tc;
      tc.set_alphabet("ab");
      tc.add_rule("aaa", "a");
      tc.add_rule("bbbbbb", "b");
      tc.add_rule("ababbbbababbbbababbbbababbbbababbbbababbbbababbbbabba",
                  "bb");

      tc.congruence().next_lookahead(3000000);

      REQUIRE(!tc.is_obviously_finite());

      TEST_HLT(tc.congruence());
      SECTION("Felsch strategy + standardization") {
        tc.congruence().strategy(policy::strategy::felsch).standardize(true);
        tc.congruence().run_until([&tc]() -> bool {
          return tc.congruence().nr_cosets_active() > 100000;
        });
        REQUIRE(!tc.congruence().finished());
        REQUIRE(!tc.congruence().complete());
        REQUIRE(!tc.congruence().compatible());
        tc.congruence().strategy(policy::strategy::hlt).standardize(true);
      }
      TEST_RANDOM_SIMS(tc.congruence());
      REQUIRE(tc.size() == 36412);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "048",
                            "Walker 5",
                            "[todd-coxeter][extreme]") {
      auto        rg = ReportGuard(REPORT);
      ToddCoxeter tc;
      tc.set_alphabet("ab");
      tc.add_rule("aaa", "a");
      tc.add_rule("bbbbbb", "b");
      tc.add_rule(
          "ababbbbababbbbababbbbababbbbababbbbababbbbababbbbabbabbbbbaa", "bb");
      tc.congruence().next_lookahead(5000000);
      REQUIRE(!tc.is_obviously_finite());

      // This example is extremely slow with Felsch
      TEST_HLT(tc.congruence());
      TEST_RANDOM_SIMS(tc.congruence());
      tc.run();
      REQUIRE(tc.congruence().complete());
      REQUIRE(tc.congruence().compatible());
      REQUIRE(tc.size() == 72822);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "049",
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
      tc.congruence().next_lookahead(5000000);
      REQUIRE(!tc.is_obviously_finite());

      // This example is extremely slow with Felsch, the random Sims strategy
      // is typically fastest
      TEST_HLT(tc.congruence());
      TEST_RANDOM_SIMS(tc.congruence());

      REQUIRE(tc.size() == 8);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "050",
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

      // This example is extremely slow with Felsch
      TEST_HLT(tc.congruence());
      TEST_RANDOM_SIMS(tc.congruence());

      REQUIRE(!tc.is_obviously_finite());
      REQUIRE(tc.size() == 78722);
    }

    // Felsch is faster here too
    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "051",
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

      TEST_HLT(tc.congruence());
      TEST_FELSCH(tc.congruence());
      TEST_RANDOM_SIMS(tc.congruence());

      REQUIRE(!tc.is_obviously_finite());
      REQUIRE(tc.size() == 153500);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "052",
                            "Walker 8",
                            "[todd-coxeter][standard]") {
      auto        rg = ReportGuard(REPORT);
      ToddCoxeter tc;
      tc.set_alphabet("ab");
      tc.add_rule("aaa", "a");
      tc.add_rule("bbbbbbbbbbbbbbbbbbbbbbb", "b");
      tc.add_rule("abbbbbbbbbbbabb", "bba");

      // This example is extremely slow with Felsch
      TEST_HLT(tc.congruence());
      TEST_RANDOM_SIMS(tc.congruence());

      REQUIRE(!tc.is_obviously_finite());
      REQUIRE(tc.size() == 270272);
    }

    // Felsch is also much much faster here!
    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "053",
                            "KnuthBendix 098",
                            "[todd-coxeter][standard]") {
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

      TEST_HLT(tc.congruence());
      TEST_FELSCH(tc.congruence());
      TEST_RANDOM_SIMS(tc.congruence());

      REQUIRE(tc.size() == 29);
    }

    // This example currently fails with every approach
    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "054",
                            "Holt 1 - M_12",
                            "[todd-coxeter][fail]") {
      auto        rg = ReportGuard();
      ToddCoxeter tc;

      tc.set_alphabet("eaAbBcCdDxy");
      tc.set_identity("e");
      tc.set_inverses("eAaBbCcDdxy");

      tc.add_rule("aaaaaaaaaaa", "x");
      tc.add_rule("x", "e");
      tc.add_rule("cc", "e");
      tc.add_rule("dd", "e");
      tc.add_rule("acacac", "e");
      tc.add_rule("adadad", "e");
      tc.add_rule("y", "cdcdcdcdcdcdcdcdcdcd");
      tc.add_rule("y", "e");
      tc.add_rule("aacdcdaDCDC", "e");

      tc.congruence().sort_generating_pairs().strategy(
          policy::strategy::felsch);

      REQUIRE(tc.size() == 95040);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "055",
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

        TEST_HLT(tc.congruence());
        TEST_FELSCH(tc.congruence());

        REQUIRE(tc.size() == 24);
      }
      SECTION("p = 5") {
        tc.add_rule(second(5), "e");

        TEST_HLT(tc.congruence());
        TEST_FELSCH(tc.congruence());

        REQUIRE(tc.size() == 120);
      }
      SECTION("p = 7") {
        tc.add_rule(second(7), "e");

        TEST_HLT(tc.congruence());
        TEST_FELSCH(tc.congruence());

        REQUIRE(tc.size() == 336);
      }
      SECTION("p = 11") {
        tc.add_rule(second(11), "e");

        TEST_HLT(tc.congruence());
        TEST_RANDOM_SIMS(tc.congruence());

        REQUIRE(tc.size() == 1320);
      }
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "056",
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

      TEST_HLT(tc.congruence());
      TEST_FELSCH(tc.congruence());
      TEST_RANDOM_SIMS(tc.congruence());

      REQUIRE(tc.size() == 6561);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "057",
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
      std::chrono::seconds t(1);
      tc.congruence().random_interval(100000);
      while (!tc.finished()) {
        tc.congruence().strategy(policy::strategy::felsch).standardize(true);
        tc.congruence().run_for(t);
        tc.congruence().strategy(policy::strategy::hlt).standardize(true);
        tc.congruence().run_for(2 * t);
        t *= 2;
      }
      REQUIRE(tc.size() == 6561);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "058",
                            "Campbell-Reza 1",
                            "[todd-coxeter][quick]") {
      auto        rg = ReportGuard(REPORT);
      ToddCoxeter tc;
      tc.set_alphabet("ab");
      tc.add_rule("aa", "bb");
      tc.add_rule("ba", "aaaaaab");

      TEST_HLT(tc.congruence());
      TEST_FELSCH(tc.congruence());
      TEST_RANDOM_SIMS(tc.congruence());

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
      REQUIRE(tc.froidure_pin()->nr_rules() == 6);
      REQUIRE(tc.normal_form("aaaaaaab") == "aab");
      REQUIRE(tc.normal_form("bab") == "aaa");
    }

    // The next example demonstrates why we require deferred standardization
    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "059",
                            "Renner monoid type D4 (Gay-Hivert), q = 1",
                            "[no-valgrind][quick][todd-coxeter]") {
      auto        rg = ReportGuard(REPORT);
      ToddCoxeter tc;
      tc.set_alphabet(11);
      for (relation_type const& rl : RennerTypeDMonoid(4, 1)) {
        tc.add_rule(rl);
      }
      REQUIRE(tc.nr_rules() == 121);
      REQUIRE(!tc.is_obviously_infinite());

      REQUIRE(tc.size() == 10625);

      TEST_HLT(tc.congruence());
      TEST_FELSCH(tc.congruence());
      TEST_RANDOM_SIMS(tc.congruence());

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

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "060",
                            "trivial semigroup",
                            "[no-valgrind][todd-coxeter][quick]") {
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
        // TODO(later) investigate: Badly doesn't work in this example, defines
        // 100million cosets and none are killed.
        // TEST_FELSCH(tc.congruence());
        // TODO(later) investigate: if the performance of creating the Felsch
        // tree can be improved, it's quite bad here if N is large enough.
        // FIXME the following doesn't seem to work, when congruence() is
        // removed.
        // tc.congruence().run_until([&tc]() -> bool {
        //  return tc.congruence().nr_cosets_active() >= 10000;
        // });
        tc.run();
        if (N % 3 == 1) {
          REQUIRE(tc.size() == 3);
        } else {
          REQUIRE(tc.size() == 1);
        }
      }
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "061",
                            "ACE --- 2p17-2p14-fel1 - HLT",
                            "[todd-coxeter][standard][ace]") {
      auto        rg = ReportGuard(REPORT);
      ToddCoxeter G;
      G.set_alphabet("abcABCe");
      G.set_identity("e");
      G.set_inverses("ABCabce");
      G.add_rule("aBCbac", "e");
      G.add_rule("bACbaacA", "e");
      G.add_rule("accAABab", "e");

      congruence::ToddCoxeter H(right, G.congruence());
      H.add_pair({1, 2}, {6});

      H.strategy(policy::strategy::hlt)
          .save(true)
          .lookahead(policy::lookahead::partial);

      REQUIRE(H.nr_classes() == 16384);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "062",
                            "ACE --- 2p17-2p14-fel1 - Felsch",
                            "[todd-coxeter][extreme][ace]") {
      auto        rg = ReportGuard();
      ToddCoxeter G;
      G.set_alphabet("abcABCe");
      G.set_identity("e");
      G.set_inverses("ABCabce");
      G.add_rule("aBCbac", "e");
      G.add_rule("bACbaacA", "e");
      G.add_rule("accAABab", "e");

      congruence::ToddCoxeter H(right, G.congruence());
      H.add_pair({1, 2}, {6});

      H.strategy(policy::strategy::felsch);
      REQUIRE(H.nr_classes() == 16384);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "063",
                            "ACE --- 2p17-2p3-fel1 - HLT",
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

      H.strategy(policy::strategy::hlt)
          .save(true)
          .lookahead(policy::lookahead::partial);

      REQUIRE(H.nr_classes() == 8);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "064",
                            "ACE --- 2p17-2p3-fel1 - Random Sims",
                            "[todd-coxeter][extreme][ace]") {
      auto        rg = ReportGuard();
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

      H.strategy(policy::strategy::random)
          .random_interval(std::chrono::milliseconds(100));
      REQUIRE(H.nr_classes() == 8);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "065",
                            "ACE --- 2p17-2p3-fel1 - Felsch",
                            "[todd-coxeter][extreme][ace]") {
      auto        rg = ReportGuard();
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

      H.strategy(policy::strategy::felsch);
      REQUIRE(H.nr_classes() == 8);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "066",
                            "ACE --- 2p17-fel1 - HLT",
                            "[todd-coxeter][extreme][ace]") {
      auto        rg = ReportGuard();
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
      H.add_pair({a, B, C, b, a, c}, {e});
      H.add_pair({b, A, C, b, a, a, c, A}, {e});
      H.add_pair({a, c, c, A, A, B, a, b}, {e});

      H.save(true).lookahead(policy::lookahead::partial);
      REQUIRE(H.nr_classes() == 131072);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "067",
                            "ACE --- 2p17-fel1a - HLT",
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

      H.strategy(policy::strategy::hlt)
          .save(true)
          .lookahead(policy::lookahead::partial);
      REQUIRE(H.nr_classes() == 1);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "068",
                            "ACE --- 2p17-fel1a - Random Sims",
                            "[todd-coxeter][extreme][ace]") {
      auto        rg = ReportGuard();
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

      H.strategy(policy::strategy::random)
          .random_interval(std::chrono::milliseconds(100));
      REQUIRE(H.nr_classes() == 1);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "069",
                            "ACE --- 2p17-fel1a - Felsch",
                            "[todd-coxeter][extreme][ace]") {
      auto        rg = ReportGuard();
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

      H.strategy(policy::strategy::felsch);
      REQUIRE(H.nr_classes() == 1);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "070",
                            "ACE --- 2p17-id-fel1 - HLT",
                            "[todd-coxeter][extreme][ace]") {
      auto        rg = ReportGuard();
      ToddCoxeter G;
      G.set_alphabet("abcABCe");
      G.set_identity("e");
      G.set_inverses("ABCabce");
      G.add_rule("aBCbac", "e");
      G.add_rule("bACbaacA", "e");
      G.add_rule("accAABab", "e");

      congruence::ToddCoxeter H(twosided, G.congruence());

      H.strategy(policy::strategy::hlt)
          .save(true)
          .lookahead(policy::lookahead::partial);

      REQUIRE(H.nr_classes() == std::pow(2, 17));
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "071",
                            "ACE --- 2p17-id-fel1 - Random Sims",
                            "[todd-coxeter][extreme][ace]") {
      auto        rg = ReportGuard();
      ToddCoxeter G;
      G.set_alphabet("abcABCe");
      G.set_identity("e");
      G.set_inverses("ABCabce");
      G.add_rule("aBCbac", "e");
      G.add_rule("bACbaacA", "e");
      G.add_rule("accAABab", "e");

      congruence::ToddCoxeter H(twosided, G.congruence());

      H.strategy(policy::strategy::random)
          .random_interval(std::chrono::milliseconds(100));

      REQUIRE(H.nr_classes() == std::pow(2, 17));
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "072",
                            "ACE --- 2p17-id-fel1 - Felsch",
                            "[todd-coxeter][extreme][ace]") {
      auto        rg = ReportGuard();
      ToddCoxeter G;
      G.set_alphabet("abcABCe");
      G.set_identity("e");
      G.set_inverses("ABCabce");
      G.add_rule("aBCbac", "e");
      G.add_rule("bACbaacA", "e");
      G.add_rule("accAABab", "e");

      congruence::ToddCoxeter H(twosided, G.congruence());

      H.strategy(policy::strategy::felsch);

      REQUIRE(H.nr_classes() == std::pow(2, 17));
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "073",
                            "ACE --- 2p18-fel1 - HLT",
                            "[todd-coxeter][extreme][ace]") {
      auto        rg = ReportGuard();
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

      REQUIRE(G.equal_to("aBCbac", "e"));
      REQUIRE(G.equal_to("bACbaacA", "e"));
      REQUIRE(G.equal_to("accAABab", "e"));

      congruence::ToddCoxeter H(twosided, G.congruence());

      H.strategy(policy::strategy::hlt)
          .save(true)
          .lookahead(policy::lookahead::partial);

      REQUIRE(H.nr_classes() == std::pow(2, 18));
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "074",
                            "ACE --- big-hard, big-fel1 - HLT",
                            "[todd-coxeter][extreme][ace]") {
      auto        rg = ReportGuard();
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

      congruence::ToddCoxeter H(twosided, G.congruence());

      H.strategy(policy::strategy::hlt)
          .save(true)
          .lookahead(policy::lookahead::partial);

      REQUIRE(H.nr_classes() == 786432);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "075",
                            "ACE --- SL219-hard - HLT",
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

      H.strategy(policy::strategy::hlt)
          .save(false)
          .lookahead(policy::lookahead::partial);
      REQUIRE(H.nr_classes() == 180);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "076",
                            "ACE --- SL219-hard - Random Sims",
                            "[todd-coxeter][extreme][ace]") {
      auto        rg = ReportGuard();
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

      H.strategy(policy::strategy::random)
          .random_interval(std::chrono::milliseconds(100));

      REQUIRE(H.nr_classes() == 180);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "077",
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

      H.strategy(policy::strategy::hlt)
          .save(true)
          .lookahead(policy::lookahead::partial);

      REQUIRE(H.nr_classes() == 480);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "078",
                            "ACE --- perf602p5 - Random Sims",
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

      H.strategy(policy::strategy::random)
          .random_interval(std::chrono::milliseconds(100));

      REQUIRE(H.nr_classes() == 480);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "079",
                            "ACE --- perf602p5 - Felsch",
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

      H.strategy(policy::strategy::felsch);

      REQUIRE(H.nr_classes() == 480);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "080",
                            "ACE --- M12-fel1, M12-hlt - HLT",
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

      H.strategy(policy::strategy::hlt)
          .save(true)
          .lookahead(policy::lookahead::partial);

      REQUIRE(H.nr_classes() == 95040);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "081",
                            "ACE --- M12-fel1, M12-hlt - Random Sims",
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

      H.strategy(policy::strategy::random)
          .random_interval(std::chrono::milliseconds(100));

      REQUIRE(H.nr_classes() == 95040);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "082",
                            "ACE --- M12-fel1, M12-hlt - Felsch",
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

      H.strategy(policy::strategy::felsch);

      REQUIRE(H.nr_classes() == 95040);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "083",
                            "ACE --- F27-fel0, F27-fel1, F27-purec - HLT",
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

      H.strategy(policy::strategy::hlt)
          .save(true)
          .lookahead(policy::lookahead::partial);

      REQUIRE(H.nr_classes() == 29);
    }

    LIBSEMIGROUPS_TEST_CASE(
        "ToddCoxeter",
        "084",
        "ACE --- F27-fel0, F27-fel1, F27-purec - Random Sims",
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

      H.strategy(policy::strategy::random)
          .random_interval(std::chrono::milliseconds(100));

      REQUIRE(H.nr_classes() == 29);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "085",
                            "ACE --- F27-fel0, F27-fel1, F27-purec - Felsch",
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

      H.strategy(policy::strategy::felsch);

      REQUIRE(H.nr_classes() == 29);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "086",
                            "ACE --- C5-fel0 - HLT",
                            "[todd-coxeter][quick][ace]") {
      auto        rg = ReportGuard(REPORT);
      ToddCoxeter G;
      G.set_alphabet("abABe");
      G.set_identity("e");
      G.set_inverses("ABabe");
      G.add_rule("aaaaa", "e");
      G.add_rule("b", "e");

      congruence::ToddCoxeter H(twosided, G);

      H.strategy(policy::strategy::hlt)
          .standardize(true)
          .save(true)
          .lookahead(policy::lookahead::partial);

      REQUIRE(H.nr_classes() == 5);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "087",
                            "ACE --- C5-fel0 - Random Sims",
                            "[todd-coxeter][quick][ace]") {
      auto        rg = ReportGuard(REPORT);
      ToddCoxeter G;
      G.set_alphabet("abABe");
      G.set_identity("e");
      G.set_inverses("ABabe");
      G.add_rule("aaaaa", "e");
      G.add_rule("b", "e");

      congruence::ToddCoxeter H(twosided, G);

      H.strategy(policy::strategy::random)
          .random_interval(std::chrono::milliseconds(100));

      REQUIRE(H.nr_classes() == 5);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "088",
                            "ACE --- C5-fel0 - Felsch",
                            "[todd-coxeter][quick][ace]") {
      auto        rg = ReportGuard(REPORT);
      ToddCoxeter G;
      G.set_alphabet("abABe");
      G.set_identity("e");
      G.set_inverses("ABabe");
      G.add_rule("aaaaa", "e");
      G.add_rule("b", "e");

      congruence::ToddCoxeter H(twosided, G);

      H.strategy(policy::strategy::felsch).standardize(true);

      REQUIRE(H.nr_classes() == 5);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "089",
                            "ACE --- A5-C5 - HLT",
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

      letter_type a = 0;
      letter_type b = 1;
      letter_type e = 4;

      H.add_pair({a, b}, {e});

      H.strategy(policy::strategy::hlt)
          .standardize(true)
          .save(true)
          .lookahead(policy::lookahead::partial);

      REQUIRE(H.nr_classes() == 12);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "090",
                            "ACE --- A5-C5 - Random Sims",
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

      letter_type a = 0;
      letter_type b = 1;
      letter_type e = 4;

      H.add_pair({a, b}, {e});

      H.strategy(policy::strategy::random)
          .random_interval(std::chrono::milliseconds(100));

      REQUIRE(H.nr_classes() == 12);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "091",
                            "ACE --- A5-C5 - Felsch",
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

      letter_type a = 0;
      letter_type b = 1;
      letter_type e = 4;

      H.add_pair({a, b}, {e});

      H.strategy(policy::strategy::felsch).standardize(true);

      REQUIRE(H.nr_classes() == 12);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "092",
                            "ACE --- A5 - HLT",
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

      H.strategy(policy::strategy::hlt)
          .standardize(false)
          .save(false)
          .lookahead(policy::lookahead::partial);

      REQUIRE(H.nr_classes() == 60);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "093",
                            "ACE --- A5 - Random Sims",
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

      H.strategy(policy::strategy::random)
          .random_interval(std::chrono::milliseconds(100));

      REQUIRE(H.nr_classes() == 60);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "094",
                            "ACE --- A5 - Felsch",
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

      H.strategy(policy::strategy::felsch)
          .standardize(true)
          .random_shuffle_generating_pairs();

      REQUIRE(H.nr_classes() == 60);
      REQUIRE_THROWS_AS(H.random_shuffle_generating_pairs(),
                        LibsemigroupsException);
    }

    LIBSEMIGROUPS_TEST_CASE("ToddCoxeter",
                            "097",
                            "relation ordering",
                            "[todd-coxeter][extreme]") {
      ToddCoxeter tc;
      tc.set_alphabet(13);
      for (relation_type const& rl : RennerTypeDMonoid(5, 1)) {
        tc.add_rule(rl);
      }
      REQUIRE(tc.nr_rules() == 173);
      REQUIRE(!tc.is_obviously_infinite());
      tc.congruence().sort_generating_pairs(&shortlex_compare);
      tc.congruence().sort_generating_pairs(recursive_path_compare);

      tc.congruence().strategy(policy::strategy::felsch);

      REQUIRE(tc.size() == 258661);
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

      TEST_FELSCH(tc.congruence());
      TEST_HLT(tc.congruence());
      TEST_RANDOM_SIMS(tc.congruence());

      REQUIRE(tc.size() == 10);

      REQUIRE_THROWS_AS(tc.congruence().sort_generating_pairs(shortlex_compare),
                        LibsemigroupsException);
    }
  }  // namespace fpsemigroup
}  // namespace libsemigroups
