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

#define CATCH_CONFIG_ENABLE_PAIR_STRINGMAKER

#include <cstdint>  // for uint8_t
#include <vector>   // for vector

#include "catch.hpp"      // for REQUIRE, REQUIRE_THROWS_AS, REQUI...
#include "test-main.hpp"  // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/constants.hpp"        // for UNDEFINED
#include "libsemigroups/froidure-pin.hpp"     // for FroidurePin, Froidure...
#include "libsemigroups/report.hpp"           // for ReportGuard
#include "libsemigroups/to-froidure-pin.hpp"  // for make
#include "libsemigroups/transf.hpp"           // for Transf
#include "libsemigroups/word-graph.hpp"       // for WordGraph

namespace libsemigroups {

  LIBSEMIGROUPS_TEST_CASE("to_froidure_pin<Transf>",
                          "000",
                          "from WordGraph",
                          "[quick][make]") {
    auto rg = ReportGuard(false);
    auto ad = to_word_graph<uint8_t>(
        5,
        {{1, 3, 4, 1}, {0, 0, 1, 1}, {2, 1, 2, 2}, {3, 2, 3, 3}, {4, 4, 4, 4}});
    auto S = to_froidure_pin<Transf<5>>(ad);
    REQUIRE(S.size() == 625);
    auto T = to_froidure_pin<Transf<6>>(ad);
    REQUIRE(T.size() == 625);
    auto U = to_froidure_pin<Transf<0, uint8_t>>(ad);
    REQUIRE(U.size() == 625);
    auto V = to_froidure_pin<Transf<0, uint8_t>>(ad, 4, 5);
    REQUIRE(V.size() == 1);
    auto W = to_froidure_pin<Transf<0, uint8_t>>(ad, 0, 0);
    REQUIRE(W.size() == 1);
  }

  LIBSEMIGROUPS_TEST_CASE("to_froidure_pin<Transf>",
                          "001",
                          "from WordGraph (exceptions)",
                          "[quick][make]") {
    auto rg = ReportGuard(false);
    auto ad = to_word_graph<uint8_t>(
        5,
        {{1, 3, 4, 1}, {0, 0, 1, 1}, {2, 1, 2, 2}, {3, 2, 3, 3}, {4, 4, 4, 4}});
    // Couldn't use REQUIRE_THROWS_AS here for some reason
    REQUIRE_THROWS(to_froidure_pin<Transf<0, uint8_t>>(ad, 10, 0));
    REQUIRE_THROWS(to_froidure_pin<Transf<0, uint8_t>>(ad, 10, 11));
    REQUIRE_THROWS(to_froidure_pin<Transf<0, uint8_t>>(ad, 0, 11));
  }

  /*
  LIBSEMIGROUPS_TEST_CASE("to_froidure_pin<TCE>",
                          "002",
                          "from Todd-Coxeter",
                          "[quick][make]") {
    using TCE            = detail::TCE;
    using FroidurePinTCE = FroidurePin<TCE>;

    auto rg = ReportGuard(false);

    Presentation<word_type> p;
    p.alphabet(4);
    presentation::add_rule_and_check(p, {0, 0}, {0});
    presentation::add_rule_and_check(p, {1, 0}, {1});
    presentation::add_rule_and_check(p, {0, 1}, {1});
    presentation::add_rule_and_check(p, {2, 0}, {2});
    presentation::add_rule_and_check(p, {0, 2}, {2});
    presentation::add_rule_and_check(p, {3, 0}, {3});
    presentation::add_rule_and_check(p, {0, 3}, {3});
    presentation::add_rule_and_check(p, {1, 1}, {0});
    presentation::add_rule_and_check(p, {2, 3}, {0});
    presentation::add_rule_and_check(p, {2, 2, 2}, {0});
    presentation::add_rule_and_check(
        p, {1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2}, {0});
    presentation::add_rule_and_check(p,
                                     {1, 2, 1, 3, 1, 2, 1, 3, 1, 2, 1,
                                      3, 1, 2, 1, 3, 1, 2, 1, 3, 1, 2,
                                      1, 3, 1, 2, 1, 3, 1, 2, 1, 3},
                                     {0});
    // TODO
    ToddCoxeter tc(twosided, p);

    section_felsch(tc);

    // check_hlt(tc);
    // check_felsch(tc);
    // check_random(tc);
    // check_Rc_style(tc);
    // check_R_over_C_style(tc);
    // check_CR_style(tc);
    // check_Cr_style(tc);
    REQUIRE(tc.number_of_classes() == 10'752);
    check_complete_compatible(tc);
  }
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

      tc.standardize(order::recursive);
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

      tc.standardize(order::lex);
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
      tc.standardize(order::shortlex);
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
*/

}  // namespace libsemigroups
