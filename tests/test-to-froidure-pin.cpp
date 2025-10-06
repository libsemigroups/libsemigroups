//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2022-2025 James D. Mitchell
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

#define CATCH_CONFIG_ENABLE_PAIR_STRINGMAKER

#include <cstdint>  // for uint8_t
#include <vector>   // for vector

#include "Catch2-3.8.0/catch_amalgamated.hpp"  // for REQUIRE, REQUIRE_THROWS_AS, REQUI...
#include "test-main.hpp"                       // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/cong.hpp"             // for Congruence
#include "libsemigroups/constants.hpp"        // for UNDEFINED
#include "libsemigroups/exception.hpp"        // for LIBSEMIGROUPS_EXCEPTION
#include "libsemigroups/froidure-pin.hpp"     // for FroidurePin, Froidure...
#include "libsemigroups/knuth-bendix.hpp"     // for KnuthBendix
#include "libsemigroups/to-froidure-pin.hpp"  // for make
#include "libsemigroups/to-presentation.hpp"  // for to<Presentation>
#include "libsemigroups/transf.hpp"           // for Transf
#include "libsemigroups/types.hpp"            // for congruence_kind
#include "libsemigroups/word-graph.hpp"       // for WordGraph

#include "libsemigroups/detail/report.hpp"  // for ReportGuard

namespace libsemigroups {

  using RewriteTrie     = detail::RewriteTrie;
  using RewriteFromLeft = detail::RewriteFromLeft;

  congruence_kind constexpr twosided = congruence_kind::twosided;

#define REWRITER_TYPES RewriteTrie, RewriteFromLeft

  LIBSEMIGROUPS_TEST_CASE("to<FroidurePin>",
                          "000",
                          "from WordGraph",
                          "[quick]") {
    auto rg = ReportGuard(false);
    auto wg = make<WordGraph<uint8_t>>(
        5,
        {{1, 3, 4, 1}, {0, 0, 1, 1}, {2, 1, 2, 2}, {3, 2, 3, 3}, {4, 4, 4, 4}});
    auto T = to<FroidurePin<Transf<5>>>(wg);
    REQUIRE(T.size() == 625);
    auto U = to<FroidurePin<Transf<0, uint8_t>>>(wg);
    REQUIRE(U.size() == 625);
    auto V = to<FroidurePin<Transf<0, uint8_t>>>(wg, 4, 5);
    REQUIRE(V.size() == 1);
    REQUIRE_THROWS_AS((to<FroidurePin<Transf<0, uint8_t>>>(wg, 0, 0)),
                      LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("to<FroidurePin>",
                          "001",
                          "from WordGraph (exceptions)",
                          "[quick]") {
    auto rg = ReportGuard(false);
    auto wg = make<WordGraph<uint8_t>>(
        5,
        {{1, 3, 4, 1}, {0, 0, 1, 1}, {2, 1, 2, 2}, {3, 2, 3, 3}, {4, 4, 4, 4}});
    REQUIRE_THROWS_AS((to<FroidurePin<Transf<0, uint8_t>>>(wg, 10, 0)),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS((to<FroidurePin<Transf<0, uint8_t>>>(wg, 10, 11)),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS((to<FroidurePin<Transf<0, uint8_t>>>(wg, 0, 11)),
                      LibsemigroupsException);
  }

  namespace {
    template <typename Word, typename OtherWord = Word>
    void check_from_ke(Presentation<Word> const& p) {
      using literals::    operator""_w;
      Kambites<OtherWord> k(twosided, p);
      auto                s = to<FroidurePin>(k);
      REQUIRE(s.is_finite() == tril::FALSE);
      s.enumerate(100);
      REQUIRE(s.current_size() == 8'205);
      REQUIRE(s[0].to_word(k) == 0_w);
      REQUIRE(s[1].to_word(k) == 1_w);
      REQUIRE(s[2].to_word(k) == 2_w);
      REQUIRE(s[4].to_word(k) == 4_w);
      REQUIRE(s[8].to_word(k) == 01_w);
      REQUIRE(s[16].to_word(k) == 12_w);
      REQUIRE(s[32].to_word(k) == 34_w);
      REQUIRE(s[64].to_word(k) == 012_w);

      REQUIRE(froidure_pin::factorisation(s, s[0]) == 0_w);
      REQUIRE(froidure_pin::factorisation(s, s[1]) == 1_w);
      REQUIRE(froidure_pin::factorisation(s, s[2]) == 2_w);
      REQUIRE(froidure_pin::factorisation(s, s[4]) == 4_w);
      REQUIRE(froidure_pin::factorisation(s, s[8]) == 01_w);
      REQUIRE(froidure_pin::factorisation(s, s[16]) == 12_w);
      REQUIRE(froidure_pin::factorisation(s, s[32]) == 34_w);
      REQUIRE(froidure_pin::factorisation(s, s[64]) == 012_w);

      REQUIRE(s[0] < s[1]);

      auto t = s[128];
      t      = s[64];
      REQUIRE(froidure_pin::factorisation(s, t) == 012_w);
      auto u = s[128];
      u.swap(t);
      REQUIRE(froidure_pin::factorisation(s, t) == 134_w);
      REQUIRE(froidure_pin::factorisation(s, u) == 012_w);
      t = std::move(u);
      REQUIRE(froidure_pin::factorisation(s, t) == 012_w);

      auto v(std::move(t));
      REQUIRE(froidure_pin::factorisation(s, v) == 012_w);

      if constexpr (!std::is_same_v<Word, word_type>) {
        REQUIRE(s[0].to_string() == "a");
        REQUIRE(s[1].to_string() == "b");
        REQUIRE(s[2].to_string() == "c");
        REQUIRE(s[4].to_string() == "e");
        REQUIRE(s[8].to_string() == "ab");
        REQUIRE(s[16].to_string() == "bc");
        REQUIRE(s[32].to_string() == "de");
        REQUIRE(s[64].to_string() == "abc");
      }

      REQUIRE(Complexity<decltype(v)>()(v) == LIMIT_MAX);
      IncreaseDegree<decltype(v)>()(v, 0);
      REQUIRE(Degree<decltype(v)>()(v) == 0);
      v = One<decltype(v)>()();
      REQUIRE(v.to_word(k) == ""_w);
    }
  }  // namespace

  LIBSEMIGROUPS_TEST_CASE("to<FroidurePin>",
                          "002",
                          "from Kambites (code cov)",
                          "[quick][no-valgrind]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("abcdefg");
    presentation::add_rule(p, "abcd", "aaaeaa");
    presentation::add_rule(p, "ef", "dg");
    check_from_ke(p);
    check_from_ke<std::string, detail::MultiView<std::string>>(p);
    check_from_ke<word_type>(v4::to<Presentation<word_type>>(p));
  }

  LIBSEMIGROUPS_TEST_CASE("to<FroidurePin>",
                          "003",
                          "from Kambites (exceptions)",
                          "[quick]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("ab");
    presentation::add_rule(p, "bababa", "aba");
    REQUIRE_THROWS_AS(check_from_ke(p), LibsemigroupsException);
    REQUIRE_THROWS_AS(
        (check_from_ke<std::string, detail::MultiView<std::string>>(p)),
        LibsemigroupsException);
    REQUIRE_THROWS_AS(
        (check_from_ke<word_type>(v4::to<Presentation<word_type>>(p))),
        LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("to<FroidurePin>",
                          "004",
                          "from KnuthBendix",
                          "[quick]") {
    auto                  rg = ReportGuard(false);
    FroidurePin<Transf<>> S;
    S.add_generator(make<Transf<>>({1, 0}));
    S.add_generator(make<Transf<>>({0, 0}));

    KnuthBendix kb(twosided, to<Presentation<word_type>>(S));
    auto        s = to<FroidurePin>(kb);
    REQUIRE(s.size() == kb.number_of_classes());
    REQUIRE(s.size() == 4);
  }

  LIBSEMIGROUPS_TEST_CASE("to<FroidurePin>",
                          "005",
                          "from Todd-Coxeter",
                          "[quick][no-valgrind]") {
    using literals::operator""_w;
    auto            rg = ReportGuard(false);

    Presentation<word_type> p;
    p.alphabet(4);
    p.rules = {00_w,
               0_w,
               10_w,
               1_w,
               01_w,
               1_w,
               20_w,
               2_w,
               02_w,
               2_w,
               30_w,
               3_w,
               03_w,
               3_w,
               11_w,
               0_w,
               23_w,
               0_w,
               222_w,
               0_w,
               12121212121212_w,
               0_w,
               12131213121312131213121312131213_w,
               0_w};
    ToddCoxeter tc(twosided, p);

    REQUIRE(tc.number_of_classes() == 10'752);
    auto S = to<FroidurePin>(tc);
    REQUIRE(S.size() == 10'752);
  }

  LIBSEMIGROUPS_TEST_CASE("to<FroidurePin>",
                          "006",
                          "from Congruence",
                          "[quick]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("abBe");
    presentation::add_identity_rules(p, 'e');
    presentation::add_rule(p, "aa", "e");
    presentation::add_rule(p, "BB", "b");
    presentation::add_rule(p, "BaBaBaB", "abababa");
    presentation::add_rule(p, "aBabaBabaBabaBab", "BabaBabaBabaBaba");

    Congruence cong(twosided, p);
    congruence::add_generating_pair(cong, "a", "b");

    REQUIRE(cong.number_of_classes() == 4);
    REQUIRE(!to<FroidurePin>(cong)->contains_one());
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("to<FroidurePin>",
                                   "007",
                                   "from KnuthBendix",
                                   "[quick]",
                                   REWRITER_TYPES) {
    using literals::        operator""_w;
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.alphabet(2);
    presentation::add_rule(p, 000_w, 0_w);
    presentation::add_rule(p, 1111_w, 1_w);
    presentation::add_rule(p, 011111011_w, 110_w);

    KnuthBendix<word_type, TestType> kb(twosided, p);
    auto                             S = to<FroidurePin>(kb);
    REQUIRE(S.size() == 12);

    using KBE_             = typename decltype(S)::element_type;
    std::vector<KBE_> gens = {KBE_(kb, 01_w), KBE_(kb, 10_w)};

    // Check that FroidurePin state is not invalidated
    S.init(gens.begin(), gens.end());
    REQUIRE(S.size() == 2);
  }

  LIBSEMIGROUPS_TEST_CASE("to<FroidurePin>",
                          "008",
                          "from ToddCoxeter",
                          "[quick][no-valgrind]") {
    auto                    rg = ReportGuard(false);
    using literals::        operator""_w;
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

    auto S = to<FroidurePin>(tc);
    auto T = froidure_pin::copy_closure(S, {S.generator(0)});

    REQUIRE(T.size() == S.size());
    REQUIRE(T.number_of_generators() == S.number_of_generators());

    REQUIRE(S.size() == 10'752);
    REQUIRE(S.number_of_idempotents() == 1);
    for (size_t c = 0; c < tc.number_of_classes(); ++c) {
      REQUIRE(todd_coxeter::word_of(tc, c)
              == froidure_pin::factorisation(S, c));
      REQUIRE(todd_coxeter::index_of(tc, todd_coxeter::word_of(tc, c)) == c);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("to<FroidurePin>",
                          "009",
                          "from default constructed Kambites",
                          "[quick]") {
    auto                  rg = ReportGuard(false);
    Kambites<std::string> k;
    auto                  fp = to<FroidurePin>(k);
    REQUIRE(fp.size() == 0);
  }

  LIBSEMIGROUPS_TEST_CASE("to<FroidurePin>", "010", "exceptions", "[quick]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("ab");
    presentation::add_rule(p, "aaaaaa", "aaa");
    presentation::add_rule(p, "bbbbbbbb", "bb");
    presentation::add_rule(p, "ab", "ba");
    KnuthBendix kb(congruence_kind::twosided, p);
    auto        S = to<FroidurePin>(kb);

    using KBE_ = typename decltype(S)::element_type;
    REQUIRE_THROWS_AS(S.contains(KBE_(kb, "cd")), LibsemigroupsException);
  }

}  // namespace libsemigroups
