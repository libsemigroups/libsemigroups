//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2025 Joseph Edwards
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

#include <type_traits>

#include "Catch2-3.7.1/catch_amalgamated.hpp"  // for REQUIRE, REQUIRE_THROWS_AS, REQUI...
#include "test-main.hpp"                       // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/exception.hpp"        // for LibsemigroupsException
#include "libsemigroups/froidure-pin.hpp"     // for FroidurePin
#include "libsemigroups/presentation.hpp"     // for Presentation
#include "libsemigroups/to-knuth-bendix.hpp"  // for to<Presentation>
#include "libsemigroups/transf.hpp"           // for Transf
#include "libsemigroups/types.hpp"            // for word_type

#include "libsemigroups/detail/report.hpp"     // for ReportGuard
#include "libsemigroups/detail/rewriters.hpp"  // for RewriteFromLeft, Rewri...

namespace libsemigroups {

  congruence_kind constexpr twosided = congruence_kind::twosided;
  congruence_kind constexpr onesided = congruence_kind::onesided;
  using RewriteFromLeft              = detail::RewriteFromLeft;
  using RewriteTrie                  = detail::RewriteTrie;

  using RewriteFromLeft_string = std::pair<RewriteFromLeft, std::string>;
  using RewriteFromLeft_word   = std::pair<RewriteFromLeft, word_type>;
  using RewriteTrie_string     = std::pair<RewriteTrie, std::string>;
  using RewriteTrie_word       = std::pair<RewriteTrie, word_type>;

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("to<KnuthBendix>",
                                   "001",
                                   "from FroidurePin",
                                   "[quick][to_knuth_bendix]",
                                   RewriteFromLeft_string,
                                   RewriteFromLeft_word,
                                   RewriteTrie_string,
                                   RewriteTrie_word) {
    auto rg        = ReportGuard(false);
    using Rewriter = typename TestType::first_type;
    using Word     = typename TestType::second_type;

    FroidurePin<Transf<>> S;
    S.add_generator(Transf<>({1, 0}));
    S.add_generator(Transf<>({0, 0}));

    auto kb = to<KnuthBendix<Word, Rewriter>>(twosided, S);
    REQUIRE(S.size() == kb.number_of_classes());
    REQUIRE(kb.number_of_classes() == 4);
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("to<KnuthBendix>",
                                   "002",
                                   "from ToddCoxeter",
                                   "[quick][to_knuth_bendix]",
                                   std::string,
                                   word_type) {
    auto rg = ReportGuard(false);

    Presentation<TestType> p;
    if constexpr (std::is_same_v<TestType, std::string>) {
      p.alphabet("abB");
      presentation::add_rule_no_checks(p, "bb", "B");
      presentation::add_rule_no_checks(p, "BaB", "aba");
      presentation::add_rule_no_checks(p, "a", "b");
      presentation::add_rule_no_checks(p, "b", "B");
    } else if constexpr (std::is_same_v<TestType, word_type>) {
      p.alphabet({0, 1, 2});
      presentation::add_rule_no_checks(p, {1, 1}, {2});
      presentation::add_rule_no_checks(p, {2, 0, 2}, {0, 1, 0});
      presentation::add_rule_no_checks(p, {0}, {1});
      presentation::add_rule_no_checks(p, {1}, {2});
    }

    REQUIRE(!p.contains_empty_word());

    ToddCoxeter<TestType> tc(twosided, p);

    tc.run();
    REQUIRE(tc.number_of_classes() == 1);
    REQUIRE(tc.finished());

    for (auto knd : {twosided, onesided}) {
      auto kb = to<KnuthBendix>(knd, tc);
      REQUIRE(kb.number_of_classes() == 1);
      if (kb.kind() == twosided) {
        REQUIRE(to<FroidurePin>(kb).size() == 1);
      } else {
        REQUIRE_THROWS_AS(to<FroidurePin>(kb), LibsemigroupsException);
      }
    }
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("to<KnuthBendix>",
                                   "003",
                                   "from ToddCoxeter",
                                   "[quick][to_knuth_bendix]",
                                   RewriteFromLeft_string,
                                   RewriteFromLeft_word,
                                   RewriteTrie_string,
                                   RewriteTrie_word) {
    auto rg        = ReportGuard(false);
    using Rewriter = typename TestType::first_type;
    using Word     = typename TestType::second_type;

    Presentation<Word> p;
    if constexpr (std::is_same_v<Word, std::string>) {
      p.alphabet("abB");
      presentation::add_rule_no_checks(p, "bb", "B");
      presentation::add_rule_no_checks(p, "BaB", "aba");
      presentation::add_rule_no_checks(p, "a", "b");
      presentation::add_rule_no_checks(p, "b", "B");
    } else if constexpr (std::is_same_v<Word, word_type>) {
      p.alphabet({0, 1, 2});
      presentation::add_rule_no_checks(p, {1, 1}, {2});
      presentation::add_rule_no_checks(p, {2, 0, 2}, {0, 1, 0});
      presentation::add_rule_no_checks(p, {0}, {1});
      presentation::add_rule_no_checks(p, {1}, {2});
    }

    REQUIRE(!p.contains_empty_word());

    ToddCoxeter<Word> tc(twosided, p);

    tc.run();
    REQUIRE(tc.number_of_classes() == 1);
    REQUIRE(tc.finished());

    for (auto knd : {twosided, onesided}) {
      auto kb = to<KnuthBendix<Word, Rewriter>>(knd, tc);
      REQUIRE(kb.number_of_classes() == 1);
      if (kb.kind() == twosided) {
        REQUIRE(to<FroidurePin>(kb).size() == 1);
      } else {
        REQUIRE_THROWS_AS(to<FroidurePin>(kb), LibsemigroupsException);
      }
    }
  }
}  // namespace libsemigroups
