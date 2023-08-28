//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2022 James D. Mitchell
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

#include "catch.hpp"      // for REQUIRE, REQUIRE_THROWS_AS, REQUI...
#include "test-main.hpp"  // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/cutting.hpp"
#include "libsemigroups/fpsemi-examples.hpp"
#include "libsemigroups/string.hpp"
#include "libsemigroups/types.hpp"

namespace libsemigroups {

  LIBSEMIGROUPS_TEST_CASE("Cutting",
                          "000",
                          "step_hen Stephen test case 001",
                          "[cutting][quick]") {
    detail::StringToWord           string_to_word("xX");
    InversePresentation<word_type> p;
    p.alphabet(string_to_word("xX"));
    p.inverses(string_to_word("Xx"));
    presentation::add_rule_and_check(
        p, string_to_word("xx"), string_to_word("xxxx"));

    auto c = Cutting(p);
    REQUIRE(c.number_of_r_classes() == 4);
    REQUIRE(c.size() == 7);
  }

  LIBSEMIGROUPS_TEST_CASE("Cutting",
                          "001",
                          "step_hen Stephen test case 002",
                          "[cutting][quick]") {
    detail::StringToWord           string_to_word("xyXY");
    InversePresentation<word_type> p;
    p.alphabet(string_to_word("xyXY"));
    p.inverses(string_to_word("XYxy"));
    presentation::add_rule_and_check(
        p, string_to_word("xxx"), string_to_word("x"));
    presentation::add_rule_and_check(
        p, string_to_word("yyyyy"), string_to_word("y"));
    presentation::add_rule_and_check(
        p, string_to_word("xyxy"), string_to_word("xx"));

    auto c = Cutting(p);
    REQUIRE(c.size() == 13);
    REQUIRE(c.number_of_r_classes() == 3);
  }

  LIBSEMIGROUPS_TEST_CASE("Cutting",
                          "002",
                          "step_hen Stephen test case 004",
                          "[cutting][quick]") {
    detail::StringToWord           string_to_word("xyzXYZ");
    InversePresentation<word_type> p;
    p.alphabet(string_to_word("xyzXYZ"));
    p.inverses(string_to_word("XYZxyz"));
    presentation::add_rule_and_check(
        p, string_to_word("xxxxx"), string_to_word("x"));
    presentation::add_rule_and_check(
        p, string_to_word("yyyyy"), string_to_word("y"));
    presentation::add_rule_and_check(
        p, string_to_word("zzzzz"), string_to_word("z"));
    presentation::add_rule_and_check(
        p, string_to_word("xyy"), string_to_word("yxx"));
    presentation::add_rule_and_check(
        p, string_to_word("xzz"), string_to_word("zxx"));
    presentation::add_rule_and_check(
        p, string_to_word("yzz"), string_to_word("zyy"));

    auto c = Cutting(p);
    REQUIRE(c.size() == 173);
    REQUIRE(c.number_of_r_classes() == 8);
  }

  LIBSEMIGROUPS_TEST_CASE("Cutting",
                          "003",
                          "symmetric_inverse_semigroup",
                          "[cutting][quick]") {
    auto p = fpsemigroup::make<InversePresentation<word_type>>(
        fpsemigroup::symmetric_inverse_monoid(4));
    presentation::replace_word(p, {}, {p.alphabet().size()});
    auto alpha = p.alphabet();
    alpha.push_back(alpha.size());
    p.alphabet(alpha);
    alpha = p.inverses();
    alpha.push_back(alpha.size());
    p.inverses(alpha);
    presentation::add_identity_rules(p, p.alphabet().back());

    REQUIRE(p.alphabet() == word_type({0, 1, 2, 3, 4, 5, 6, 7, 8}));
    REQUIRE(p.inverses() == word_type({4, 5, 6, 7, 0, 1, 2, 3, 8}));
    p.validate();

    auto c = Cutting(p);
    REQUIRE(c.size() == 210);
    REQUIRE(c.number_of_r_classes() == 17);
  }

  LIBSEMIGROUPS_TEST_CASE("Cutting",
                          "004",
                          "symmetric_inverse_monoid",
                          "[cutting][extreme]") {
    auto p = fpsemigroup::make<InversePresentation<word_type>>(
        fpsemigroup::symmetric_inverse_monoid(7));

    // REQUIRE(p.alphabet() == word_type({0, 1, 2, 3, 4, 5, 6, 7}));
    // REQUIRE(p.inverses() == word_type({4, 5, 6, 7, 0, 1, 2, 3}));

    REQUIRE(presentation::length(p) == 340);
    presentation::sort_each_rule(p);
    presentation::sort_rules(p);

    // TODO the following mess up the inverses
    // presentation::replace_subword(p,
    // presentation::longest_common_subword(p)); REQUIRE(presentation::length(p)
    // == 316); presentation::replace_subword(p,
    // presentation::longest_common_subword(p)); REQUIRE(presentation::length(p)
    // == 302); presentation::replace_subword(p,
    // presentation::longest_common_subword(p)); REQUIRE(presentation::length(p)
    // == 288);

    auto c = Cutting(p);
    REQUIRE(c.size() == 130'922);
    REQUIRE(c.number_of_r_classes() == 128);
    REQUIRE(c.number_of_d_classes() == 8);
  }

  LIBSEMIGROUPS_TEST_CASE("Cutting",
                          "005",
                          "dual_symmetric_inverse_monoid",
                          "[cutting][quick]") {
    auto p = fpsemigroup::make<InversePresentation<word_type>>(
        fpsemigroup::dual_symmetric_inverse_monoid(4));
    REQUIRE(!p.contains_empty_word());

    auto c = Cutting(p);
    REQUIRE(c.size() == 340);
    REQUIRE(c.number_of_r_classes() == 16);
  }

  LIBSEMIGROUPS_TEST_CASE("Cutting",
                          "006",
                          "cyclic inverse monoid",
                          "[cutting][quick]") {
    size_t                         n = 6;
    detail::StringToWord           string_to_word("egGx");
    InversePresentation<word_type> p;
    p.alphabet(string_to_word("egG"));
    p.inverses(string_to_word("eGg"));
    p.contains_empty_word(true);
    presentation::add_rule_and_check(
        p, string_to_word(std::string(n, 'g')), string_to_word(""));
    presentation::add_rule_and_check(
        p, string_to_word("ee"), string_to_word("e"));
    presentation::add_rule(
        p, string_to_word("gxxxxxx"), string_to_word("xxxxxx"));
    presentation::replace_subword(
        p, string_to_word("x"), string_to_word("eggggg"));
    p.validate();

    // REQUIRE(p.rules == std::vector<word_type>());

    auto c = Cutting(p);
    REQUIRE(c.size() == 0);
    REQUIRE(c.number_of_r_classes() == 16);
  }

}  // namespace libsemigroups
