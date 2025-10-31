//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2025 James D. Mitchell
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

#include "Catch2-3.8.0/catch_amalgamated.hpp"  // for REQUIRE, REQUIRE_THROWS_AS, REQUI...
#include "test-main.hpp"                       // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/to-todd-coxeter.hpp"  // for to<ToddCoxeter>
#include "libsemigroups/transf.hpp"
#include "libsemigroups/word-graph-helpers.hpp"  // for word_graph

#include "libsemigroups/detail/report.hpp"  // for ReportGuard

namespace libsemigroups {
  using literals::operator""_w;

  congruence_kind constexpr twosided = congruence_kind::twosided;
  congruence_kind constexpr onesided = congruence_kind::onesided;

  LIBSEMIGROUPS_TEST_CASE("to<ToddCoxeter<word_type>>",
                          "021",
                          "from WordGraph",
                          "[quick]") {
    auto rg = ReportGuard(false);

    using Transf          = LeastTransf<5>;
    FroidurePin<Transf> S = make<FroidurePin>(
        {make<Transf>({1, 3, 4, 2, 3}), make<Transf>({3, 2, 1, 3, 3})});

    REQUIRE(S.size() == 88);

    // Construct from Cayley graph of S
    auto tc = to<ToddCoxeter<word_type>>(twosided, S, S.right_cayley_graph());
    REQUIRE(tc.current_word_graph().number_of_nodes() == 89);

    todd_coxeter::add_generating_pair(
        tc,
        froidure_pin::factorisation(S, make<Transf>({3, 4, 4, 4, 4})),
        froidure_pin::factorisation(S, make<Transf>({3, 1, 3, 3, 3})));
    REQUIRE(!tc.finished());
    tc.shrink_to_fit();  // does nothing
    REQUIRE(!tc.finished());
    tc.standardize(Order::none);  // does nothing
    REQUIRE(!tc.finished());

    REQUIRE(tc.number_of_classes() == 21);
    tc.shrink_to_fit();
    REQUIRE(tc.number_of_classes() == 21);
    tc.standardize(Order::recursive);
    auto w = (todd_coxeter::normal_forms(tc) | rx::to_vector());
    REQUIRE(w.size() == 21);
    REQUIRE(w
            == std::vector({0_w,    00_w,    000_w,   0000_w, 1_w,     10_w,
                            100_w,  1000_w,  01_w,    010_w,  0100_w,  01000_w,
                            001_w,  11_w,    110_w,   1100_w, 11000_w, 011_w,
                            0110_w, 01100_w, 011000_w}));
    REQUIRE(std::unique(w.begin(), w.end()) == w.end());
    REQUIRE(std::is_sorted(w.cbegin(), w.cend(), RecursivePathCompare{}));
    REQUIRE((todd_coxeter::normal_forms(tc) | rx::all_of([&tc](auto const& u) {
               return todd_coxeter::word_of(tc, todd_coxeter::index_of(tc, u))
                      == u;
             })));
  }

  LIBSEMIGROUPS_TEST_CASE("to<ToddCoxeter<std::string>>",
                          "022",
                          "from WordGraph",
                          "[quick]") {
    auto rg = ReportGuard(false);

    using Transf  = LeastTransf<5>;
    FroidurePin S = make<FroidurePin>(
        {make<Transf>({1, 3, 4, 2, 3}), make<Transf>({3, 2, 1, 3, 3})});

    REQUIRE(S.size() == 88);

    // Construct from Cayley graph of S
    auto tc = to<ToddCoxeter<std::string>>(twosided, S, S.right_cayley_graph());
    REQUIRE(tc.current_word_graph().number_of_nodes() == 89);

    ToString to_string(tc.presentation().alphabet());
    todd_coxeter::add_generating_pair(tc,
                                      to_string(froidure_pin::factorisation(
                                          S, make<Transf>({3, 4, 4, 4, 4}))),
                                      to_string(froidure_pin::factorisation(
                                          S, make<Transf>({3, 1, 3, 3, 3}))));
    REQUIRE(!tc.finished());
    tc.shrink_to_fit();  // does nothing
    REQUIRE(!tc.finished());
    tc.standardize(Order::none);  // does nothing
    REQUIRE(!tc.finished());

    REQUIRE(tc.number_of_classes() == 21);
    tc.shrink_to_fit();
    REQUIRE(tc.number_of_classes() == 21);
    tc.standardize(Order::recursive);
    auto w = (todd_coxeter::normal_forms(tc) | rx::to_vector());
    REQUIRE(w.size() == 21);
    REQUIRE(w
            == std::vector<std::string>(
                {"a",    "aa",   "aaa",   "aaaa", "b",     "ba",    "baa",
                 "baaa", "ab",   "aba",   "abaa", "abaaa", "aab",   "bb",
                 "bba",  "bbaa", "bbaaa", "abb",  "abba",  "abbaa", "abbaaa"}));
    REQUIRE(std::unique(w.begin(), w.end()) == w.end());
    REQUIRE(std::is_sorted(w.cbegin(), w.cend(), RecursivePathCompare{}));
    REQUIRE((todd_coxeter::normal_forms(tc) | rx::all_of([&tc](auto const& u) {
               return todd_coxeter::word_of(tc, todd_coxeter::index_of(tc, u))
                      == u;
             })));
  }

  LIBSEMIGROUPS_TEST_CASE("to<ToddCoxeter<word_type>>",
                          "023",
                          "from WordGraph",
                          "[todd-coxeter][quick]") {
    auto rg = ReportGuard(false);
    auto S  = make<FroidurePin>(
        {make<Transf<>>({1, 3, 4, 2, 3}), make<Transf<>>({3, 2, 1, 3, 3})});

    REQUIRE(S.size() == 88);
    REQUIRE(S.number_of_rules() == 18);

    auto tc = to<ToddCoxeter<word_type>>(twosided, S, S.right_cayley_graph());

    REQUIRE(froidure_pin::factorisation(S, make<Transf<>>({3, 4, 4, 4, 4}))
            == 010001100_w);
    REQUIRE(froidure_pin::factorisation(S, make<Transf<>>({3, 1, 3, 3, 3}))
            == 10001_w);
    todd_coxeter::add_generating_pair(
        tc,
        froidure_pin::factorisation(S, make<Transf<>>({3, 4, 4, 4, 4})),
        froidure_pin::factorisation(S, make<Transf<>>({3, 1, 3, 3, 3})));

    REQUIRE(tc.generating_pairs()[0] == 010001100_w);
    REQUIRE(tc.number_of_classes() == 21);
    auto const& wg = tc.current_word_graph();
    REQUIRE(
        v4::word_graph::is_compatible_no_checks(wg,
                                                wg.cbegin_active_nodes(),
                                                wg.cend_active_nodes(),
                                                tc.generating_pairs().cbegin(),
                                                tc.generating_pairs().cend()));
    REQUIRE(tc.number_of_classes() == 21);

    REQUIRE(
        todd_coxeter::index_of(
            tc, froidure_pin::factorisation(S, make<Transf<>>({1, 3, 1, 3, 3})))
        == todd_coxeter::index_of(
            tc,
            froidure_pin::factorisation(S, make<Transf<>>({4, 2, 4, 4, 2}))));

    tc.standardize(Order::shortlex);

    auto ntc = todd_coxeter::non_trivial_classes(
        tc, S.cbegin_normal_forms(), S.cend_normal_forms());

    REQUIRE(ntc.size() == 1);
    REQUIRE(ntc[0].size() == 68);
    REQUIRE(ntc
            == decltype(ntc)(
                {{001_w,       101_w,       0001_w,     0010_w,     0011_w,
                  0101_w,      1001_w,      1010_w,     1011_w,     00001_w,
                  00010_w,     00011_w,     00100_w,    00101_w,    00110_w,
                  01010_w,     01011_w,     10001_w,    10010_w,    10011_w,
                  10100_w,     10101_w,     10110_w,    000010_w,   000011_w,
                  000100_w,    000101_w,    000110_w,   001000_w,   001100_w,
                  010001_w,    010100_w,    010101_w,   010110_w,   100010_w,
                  100011_w,    100100_w,    100101_w,   100110_w,   101000_w,
                  101100_w,    0000100_w,   0000101_w,  0000110_w,  0001000_w,
                  0001100_w,   0010001_w,   0100010_w,  0100011_w,  0101000_w,
                  0101100_w,   1000100_w,   1000101_w,  1000110_w,  1001000_w,
                  1001100_w,   00001000_w,  00001100_w, 00100010_w, 01000100_w,
                  01000101_w,  01000110_w,  10001000_w, 10001100_w, 001000100_w,
                  001000101_w, 010001000_w, 010001100_w}}));
  }

  LIBSEMIGROUPS_TEST_CASE("to<ToddCoxeter>",
                          "024",
                          "from KnuthBendix",
                          "[quick]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("abB");
    presentation::add_rule_no_checks(p, "bb", "B");
    presentation::add_rule_no_checks(p, "BaB", "aba");
    presentation::add_rule_no_checks(p, "a", "b");
    presentation::add_rule_no_checks(p, "b", "B");

    REQUIRE(!p.contains_empty_word());

    KnuthBendix kb(twosided, p);

    REQUIRE(kb.confluent());
    kb.run();
    REQUIRE(kb.confluent());
    REQUIRE(kb.number_of_active_rules() == 3);
    REQUIRE(kb.number_of_classes() == 1);
    // REQUIRE(kb.is_obviously_finite());
    REQUIRE(kb.finished());

    for (auto knd : {twosided, onesided}) {
      auto tc = to<ToddCoxeter>(knd, kb);
      todd_coxeter::add_generating_pair(tc, "a", "b");
      REQUIRE(tc.number_of_classes() == 1);
      if (tc.kind() == twosided) {
        REQUIRE(to<FroidurePin>(tc).size() == 1);
      } else {
        REQUIRE_THROWS_AS(to<FroidurePin>(tc), LibsemigroupsException);
      }
    }
  }

}  // namespace libsemigroups
