// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2020-2023 James D. Mitchell
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

// This file is the fifth of six that contains tests for the KnuthBendix
// classes. In a mostly vain attempt to speed up compilation the tests are
// split across 6 files as follows:
//
// 1: contains quick tests for KnuthBendix created from rules and all commented
//    out tests.
//
// 2: contains more quick tests for KnuthBendix created from rules
//
// 3: contains yet more quick tests for KnuthBendix created from rules
//
// 4: contains standard and extreme test for KnuthBendix created from rules
//
// 5: contains tests for KnuthBendix created from FroidurePin instances
//
// 6: contains tests for congruence::KnuthBendix.

#include <algorithm>      // for copy, fill
#include <string>         // for basic_string
#include <unordered_map>  // for operator!=, operator==
#include <vector>         // for vector, operator==
                          //
#include "catch.hpp"      // for AssertionHandler, ope...
#include "test-main.hpp"  // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/constants.hpp"         // for operator!=, operator==
#include "libsemigroups/exception.hpp"         // for LibsemigroupsException
#include "libsemigroups/froidure-pin.hpp"      // for FroidurePin
#include "libsemigroups/kbe-new.hpp"           // for KBE
#include "libsemigroups/knuth-bendix-new.hpp"  // for KnuthBendix
#include "libsemigroups/present.hpp"           // for Presentation
#include "libsemigroups/report.hpp"            // for ReportGuard
#include "libsemigroups/to-froidure-pin.hpp"   // for to_froidure_pin
#include "libsemigroups/to-presentation.hpp"   // for to_presentation
#include "libsemigroups/transf.hpp"            // for Transf
#include "libsemigroups/types.hpp"             // for word_type, letter_type
#include "libsemigroups/words.hpp"             // for operator""_w

namespace libsemigroups {
  congruence_kind constexpr twosided = congruence_kind::twosided;

  using literals::operator""_w;
  using namespace rx;

  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "097",
                          "transformation semigroup (size 4)",
                          "[quick][knuth-bendix]") {
    auto                  rg = ReportGuard(false);
    FroidurePin<Transf<>> S({Transf<>({1, 0}), Transf<>({0, 0})});
    REQUIRE(S.size() == 4);
    REQUIRE(S.number_of_rules() == 4);

    auto p = to_presentation<word_type>(S);

    KnuthBendix kb(twosided, p);
    REQUIRE(kb.confluent());
    REQUIRE(kb.presentation().rules.size() / 2 == 4);
    REQUIRE(kb.number_of_active_rules() == 4);
    REQUIRE(kb.size() == 4);
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "098",
                          "transformation semigroup (size 9)",
                          "[quick][knuth-bendix]") {
    auto                  rg = ReportGuard(false);
    FroidurePin<Transf<>> S;
    S.add_generator(Transf<>({1, 3, 4, 2, 3}));
    S.add_generator(Transf<>({0, 0, 0, 0, 0}));

    REQUIRE(S.size() == 9);
    REQUIRE(S.degree() == 5);
    REQUIRE(S.number_of_rules() == 3);

    auto        p = to_presentation<word_type>(S);
    KnuthBendix kb(twosided, p);
    REQUIRE(kb.confluent());
    REQUIRE(kb.number_of_active_rules() == 3);
    REQUIRE(kb.size() == 9);
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "099",
                          "transformation semigroup (size 88)",
                          "[quick][knuth-bendix]") {
    auto                  rg = ReportGuard(false);
    FroidurePin<Transf<>> S;
    S.add_generator(Transf<>({1, 3, 4, 2, 3}));
    S.add_generator(Transf<>({3, 2, 1, 3, 3}));

    REQUIRE(S.size() == 88);
    REQUIRE(S.degree() == 5);
    REQUIRE(S.number_of_rules() == 18);

    auto        p = to_presentation<word_type>(S);
    KnuthBendix kb(twosided, p);
    REQUIRE(kb.confluent());
    REQUIRE(kb.number_of_active_rules() == 18);
    REQUIRE(kb.size() == 88);
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "100",
                          "internal_string_to_word",
                          "[quick]") {
    auto                  rg = ReportGuard(false);
    FroidurePin<Transf<>> S;
    S.add_generator(Transf<>({1, 0}));
    S.add_generator(Transf<>({0, 0}));

    auto p = to_presentation<word_type>(S);

    KnuthBendix kb(twosided, p);
    REQUIRE(kb.confluent());

    auto t = to_froidure_pin(kb);
    REQUIRE(t.generator(0).word(kb) == 0_w);
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "102",
                          "internal_string_to_word",
                          "[quick]") {
    auto                  rg = ReportGuard(false);
    FroidurePin<Transf<>> S(
        {Transf<>({1, 3, 4, 2, 3}), Transf<>({3, 2, 1, 3, 3})});

    REQUIRE(S.size() == 88);

    auto p = to_presentation<word_type>(S);

    KnuthBendix kb(twosided, p);
    kb.run();
    REQUIRE(kb.confluent());
    REQUIRE(kb.size() == 88);
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendix",
                          "122",
                          "right congruence!!!",
                          "[quick][knuthbendix]") {
    using presentation::operator+;

    auto rg = ReportGuard(false);
    auto S  = FroidurePin<Transf<>>(
        {Transf<>({1, 3, 4, 2, 3}), Transf<>({3, 2, 1, 3, 3})});

    REQUIRE(S.size() == 88);
    REQUIRE(S.number_of_rules() == 18);

    auto p = to_presentation<word_type>(S);

    KnuthBendix kb1(twosided, p);
    REQUIRE(kb1.size() == 88);

    presentation::add_rule(p,
                           2_w + S.factorisation(Transf<>({3, 4, 4, 4, 4})),
                           2_w + S.factorisation(Transf<>({3, 1, 3, 3, 3})));

    p.alphabet(3);

    KnuthBendix kb2(twosided, p);
    auto const& q = kb2.presentation();

    auto words = (S.normal_forms()
                  | rx::transform([](auto const& w) { return 2_w + w; })
                  | to_strings(q.alphabet()));
    REQUIRE((words | rx::count()) == 88);
    REQUIRE((words | rx::take(4) | rx::to_vector())
            == std::vector<std::string>({"ca", "cb", "caa", "cab"}));

    kb2.run();
    auto pp = knuth_bendix::partition(kb2, words);

    REQUIRE(pp.size() == 72);

    REQUIRE(kb2.gilman_digraph().number_of_nodes() == 62);

    auto copy   = kb2.gilman_digraph();
    auto source = copy.neighbor(0, 2);
    copy.remove_label_no_checks(2);
    REQUIRE(copy.out_degree() == 2);
    REQUIRE(copy.number_of_nodes() == 62);
    REQUIRE(action_digraph_helper::is_acyclic(copy, source));

    Paths paths(copy);
    REQUIRE(paths.min(1).from(source).count() == 72);

    REQUIRE(!kb2.contains(2_w + S.factorisation(Transf<>({1, 3, 1, 3, 3})),
                          2_w + S.factorisation(Transf<>({4, 2, 4, 4, 2}))));

    REQUIRE(!kb2.contains(2_w + S.factorisation(Transf<>({1, 3, 3, 3, 3})),
                          2_w + S.factorisation(Transf<>({4, 2, 4, 4, 2}))));

    REQUIRE(kb2.contains(2_w + S.factorisation(Transf<>({2, 4, 2, 2, 2})),
                         2_w + S.factorisation(Transf<>({2, 3, 3, 3, 3}))));

    REQUIRE(!kb2.contains(2_w + S.factorisation(Transf<>({1, 3, 3, 3, 3})),
                          2_w + S.factorisation(Transf<>({2, 3, 3, 3, 3}))));

    auto ntc = (iterator_range(pp.begin(), pp.end())
                | filter([](auto const& val) { return val.size() > 1; })
                | transform([](auto& val) {
                    std::for_each(
                        val.begin(), val.end(), [](auto& w) -> auto& {
                          w.erase(w.begin());
                          return w;
                        });
                    return val;
                  }));

    REQUIRE((ntc | count()) == 4);
    REQUIRE(
        (ntc | to_vector())
        == std::vector<std::vector<std::string>>(
            {{"baaab",
              "baaabb",
              "aabaaab",
              "abaaaba",
              "abaaabab",
              "baaabaaa",
              "abaaabbaa"},
             {"abaaab", "baaabab", "aabaaabab"},
             {"baaaba", "abaaabb", "baaabba", "aabaaaba", "abaaabaa"},
             {"baaabaa", "abaaabba", "baaabbaa", "aabaaabaa", "abaaabaaa"}}));
    // REQUIRE(sizes == std::vector<size_t>({3, 5, 5, 7}));
    // word_type w3, w4, w5, w6;
    // S.factorisation(w3, S.position(Transf<>({1, 3, 3, 3, 3})));
    // S.factorisation(w4, S.position(Transf<>({4, 2, 4, 4, 2})));
    // S.factorisation(w5, S.position(Transf<>({2, 4, 2, 2, 2})));
    // S.factorisation(w6, S.position(Transf<>({2, 3, 3, 3, 3})));
    // REQUIRE(tc.word_to_class_index(w3) != tc.word_to_class_index(w4));
    // REQUIRE(tc.word_to_class_index(w5) == tc.word_to_class_index(w6));
  }
}  // namespace libsemigroups
