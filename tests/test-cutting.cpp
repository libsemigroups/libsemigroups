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

// TODO:
// * examples from Cutting's thesis if any
// * examples from Stephen's thesis (Theorem 5.17)
// *

#include "catch_amalgamated.hpp"  // for REQUIRE, REQUIRE_THROWS_AS, REQUI...
#include "test-main.hpp"          // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/cutting.hpp"
#include "libsemigroups/detail/string.hpp"
#include "libsemigroups/fpsemi-examples.hpp"
#include "libsemigroups/types.hpp"

namespace libsemigroups {
  using literals::operator""_w;

  LIBSEMIGROUPS_TEST_CASE("Cutting",
                          "000",
                          "step_hen Stephen test case 001",
                          "[cutting][quick]") {
    ToWord                         to_word("xX");
    InversePresentation<word_type> p;
    p.alphabet(to_word("xX"));
    p.inverses_no_checks(to_word("Xx"));
    presentation::add_rule(p, to_word("xx"), to_word("xxxx"));

    auto c = Cutting(p);
    REQUIRE(c.number_of_r_classes() == 4);
    REQUIRE(c.size() == 7);
  }

  LIBSEMIGROUPS_TEST_CASE("Cutting",
                          "001",
                          "step_hen Stephen test case 002",
                          "[cutting][quick]") {
    ToWord                         to_word("xyXY");
    InversePresentation<word_type> p;
    p.alphabet(to_word("xyXY"));
    p.inverses_no_checks(to_word("XYxy"));
    presentation::add_rule(p, to_word("xxx"), to_word("x"));
    presentation::add_rule(p, to_word("yyyyy"), to_word("y"));
    presentation::add_rule(p, to_word("xyxy"), to_word("xx"));

    auto c = Cutting(p);
    REQUIRE(c.size() == 13);
    REQUIRE(c.number_of_r_classes() == 3);
  }

  LIBSEMIGROUPS_TEST_CASE("Cutting",
                          "002",
                          "step_hen Stephen test case 004",
                          "[cutting][quick][no-valgrind]") {
    ToWord                         to_word("xyzXYZ");
    InversePresentation<word_type> p;
    p.alphabet(to_word("xyzXYZ"));
    p.inverses_no_checks(to_word("XYZxyz"));
    presentation::add_rule(p, to_word("xxxxx"), to_word("x"));
    presentation::add_rule(p, to_word("yyyyy"), to_word("y"));
    presentation::add_rule(p, to_word("zzzzz"), to_word("z"));
    presentation::add_rule(p, to_word("xyy"), to_word("yxx"));
    presentation::add_rule(p, to_word("xzz"), to_word("zxx"));
    presentation::add_rule(p, to_word("yzz"), to_word("zyy"));

    auto c = Cutting(p);
    REQUIRE(c.size() == 173);
    REQUIRE(c.number_of_r_classes() == 8);
  }

  LIBSEMIGROUPS_TEST_CASE("Cutting",
                          "003",
                          "symmetric_inverse_semigroup",
                          "[cutting][quick][no-valgrind]") {
    auto p = to_inverse_presentation(
        presentation::examples::symmetric_inverse_monoid(4));
    REQUIRE(p.alphabet() == 01234567_w);
    REQUIRE(p.inverses() == 45670123_w);
    p.validate();

    auto c = Cutting(p);
    REQUIRE(c.size() == 209);
    REQUIRE(c.number_of_r_classes() == 16);
  }

  // TODO this is rather slow (~4s)
  LIBSEMIGROUPS_TEST_CASE("Cutting",
                          "004",
                          "symmetric_inverse_monoid",
                          "[cutting][extreme]") {
    InversePresentation<word_type> p = to_inverse_presentation(
        presentation::examples::symmetric_inverse_monoid(7));

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
                          "[cutting][quick][no-valgrind]") {
    auto p = to_inverse_presentation(
        presentation::examples::dual_symmetric_inverse_monoid(4));
    REQUIRE(p.contains_empty_word());

    auto c = Cutting(p);
    REQUIRE(c.size() == 339);
    REQUIRE(c.number_of_r_classes() == 15);
  }

  LIBSEMIGROUPS_TEST_CASE("Cutting",
                          "006",
                          "cyclic inverse monoid",
                          "[cutting][quick]") {
    size_t                         n = 6;
    ToWord                         to_word("egGx");
    InversePresentation<word_type> p;
    p.alphabet(to_word("egG"));
    p.inverses_no_checks(to_word("eGg"));
    p.contains_empty_word(true);
    presentation::add_rule(p, to_word(words::pow("g", n)), to_word(""));
    presentation::add_rule(p, to_word("ee"), to_word("e"));
    presentation::add_rule_no_checks(p, to_word("gxxxxxx"), to_word("xxxxxx"));
    presentation::replace_subword(p, to_word("x"), to_word("eggggg"));
    p.validate();

    // REQUIRE(p.rules == std::vector<word_type>());

    auto c = Cutting(p);
    REQUIRE(c.size() == 379);
    REQUIRE(c.number_of_r_classes() == 64);
  }

  LIBSEMIGROUPS_TEST_CASE("Cutting",
                          "007",
                          "step_hen Stephen test case 003",
                          "[cutting][quick]") {
    ToWord                         to_word("xyXY");
    InversePresentation<word_type> p;
    p.alphabet(to_word("xyXY"));
    p.inverses_no_checks(to_word("XYxy"));
    presentation::add_rule(p, to_word("xxx"), to_word("x"));
    presentation::add_rule(p, to_word("yyy"), to_word("y"));
    presentation::add_rule(p, to_word("xyy"), to_word("yxx"));

    auto c = Cutting(p);
    REQUIRE(c.size() == 7);
    REQUIRE(c.number_of_r_classes() == 4);
  }

  LIBSEMIGROUPS_TEST_CASE("Cutting",
                          "008",
                          "step_hen Stephen test case 005",
                          "[cutting][quick]") {
    ToWord                         to_word("xeXE");
    InversePresentation<word_type> p;
    p.alphabet(to_word("xeXE"));
    p.inverses_no_checks(to_word("XExe"));
    presentation::add_rule(p, to_word("xxxx"), to_word("x"));
    presentation::add_rule(p, to_word("ee"), to_word("e"));

    auto c = Cutting(p);
    REQUIRE(c.size() == 26);
    REQUIRE(c.number_of_r_classes() == 10);
  }

  LIBSEMIGROUPS_TEST_CASE("Cutting",
                          "009",
                          "step_hen Stephen test case 006",
                          "[cutting][quick]") {
    ToWord                         to_word("abcABC");
    InversePresentation<word_type> p;
    p.contains_empty_word(true);
    p.alphabet(to_word("abcABC"));
    p.inverses_no_checks(to_word("ABCabc"));

    presentation::add_rule(p, to_word("aaa"), to_word(""));
    presentation::add_rule(p, to_word("bb"), to_word(""));
    presentation::add_rule(p, to_word("Ab"), to_word("ba"));
    presentation::add_rule(p, to_word("bA"), to_word("ab"));
    presentation::add_rule(p, to_word("aba"), to_word("b"));
    presentation::add_rule(p, to_word("bab"), to_word("A"));
    presentation::add_rule(p, to_word("cb"), to_word("bc"));
    presentation::add_rule(p, to_word("cc"), to_word("c"));
    presentation::add_rule(p, to_word("bcA"), to_word("cab"));
    presentation::add_rule(p, to_word("bcab"), to_word("cA"));
    presentation::add_rule(p, to_word("Acac"), to_word("bcac"));
    presentation::add_rule(p, to_word("abcac"), to_word("cac"));
    presentation::add_rule(p, to_word("acAc"), to_word("cabc"));
    presentation::add_rule(p, to_word("bacA"), to_word("Acab"));
    presentation::add_rule(p, to_word("bacab"), to_word("AcA"));
    presentation::add_rule(p, to_word("bacac"), to_word("acac"));
    presentation::add_rule(p, to_word("cAca"), to_word("bcac"));
    presentation::add_rule(p, to_word("cabca"), to_word("cac"));
    presentation::add_rule(p, to_word("cacA"), to_word("cabc"));
    presentation::add_rule(p, to_word("cacab"), to_word("caca"));
    presentation::add_rule(p, to_word("Acabc"), to_word("cAc"));
    presentation::add_rule(p, to_word("acacac"), to_word("cacac"));
    presentation::add_rule(p, to_word("bcacac"), to_word("cacac"));
    presentation::add_rule(p, to_word("cacaca"), to_word("cacac"));

    auto c = Cutting(p);
    REQUIRE(c.size() == 34);
    REQUIRE(c.number_of_r_classes() == 8);
  }
}  // namespace libsemigroups
