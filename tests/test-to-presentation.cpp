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

#include <algorithm>         // for fill,  max_element
#include <cstdint>           // for uint8_t, uint16_t
#include <initializer_list>  // for initializer_list
#include <string>            // for basic_string, operator==
#include <vector>            // for vector

#include "catch.hpp"      // for REQUIRE, REQUIRE_THROWS_AS, REQUI...
#include "test-main.hpp"  // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/bipart.hpp"           // for Bipartition
#include "libsemigroups/constants.hpp"        // for operator!=, operator==
#include "libsemigroups/exception.hpp"        // for LibsemigroupsException
#include "libsemigroups/froidure-pin.hpp"     // for FroidurePin
#include "libsemigroups/presentation.hpp"          // for Presentation, change_...
#include "libsemigroups/to-presentation.hpp"  // for to_presentation
#include "libsemigroups/types.hpp"            // for word_type

#include "libsemigroups/detail/containers.hpp"  // for StaticVector1, operat...
#include "libsemigroups/detail/report.hpp"      // for ReportGuard

namespace libsemigroups {

  namespace {

    template <typename W>
    void check_to_presentation_from_froidure_pin() {
      FroidurePin<Bipartition> S;
      S.add_generator(Bipartition({{1, -1}, {2, -2}, {3, -3}, {4, -4}}));
      S.add_generator(Bipartition({{1, -2}, {2, -3}, {3, -4}, {4, -1}}));
      S.add_generator(Bipartition({{1, -2}, {2, -1}, {3, -3}, {4, -4}}));
      S.add_generator(Bipartition({{1, 2}, {3, -3}, {4, -4}, {-1, -2}}));
      REQUIRE(S.size() == 105);

      auto p = to_presentation<W>(S);
      REQUIRE(p.alphabet().size() == 4);
      REQUIRE(p.rules.size() == 86);
      REQUIRE(presentation::length(p) == 359);
      REQUIRE(std::max_element(p.rules.cbegin(),
                               p.rules.cend(),
                               [](auto const& x, auto const& y) {
                                 return x.size() < y.size();
                               })
                  ->size()
              == 8);
      p.validate();
    }

    template <typename W1, typename W2>
    void check_to_presentation_from_presentation() {
      Presentation<W1> p;
      p.alphabet(2);
      p.contains_empty_word(false);
      presentation::add_rule_no_checks(p, {0, 1, 2}, {0, 1});
      presentation::add_rule_no_checks(p, {0, 1, 2}, {});
      // intentionally bad
      REQUIRE_THROWS_AS(p.validate(), LibsemigroupsException);
      REQUIRE_THROWS_AS(to_presentation<W2>(p), LibsemigroupsException);

      p.alphabet_from_rules();
      REQUIRE(p.alphabet() == W1({0, 1, 2}));
      p.validate();
      REQUIRE(p.contains_empty_word());
      auto q = to_presentation<W2>(p);

      REQUIRE(q.contains_empty_word());
      q.validate();
    }
  }  // namespace

  using detail::StaticVector1;

  LIBSEMIGROUPS_TEST_CASE("to_presentation",
                          "000",
                          "from FroidurePin",
                          "[quick][to_presentation]") {
    auto rg = ReportGuard(false);
    check_to_presentation_from_froidure_pin<word_type>();
    check_to_presentation_from_froidure_pin<StaticVector1<uint16_t, 8>>();
    check_to_presentation_from_froidure_pin<std::string>();
  }

  LIBSEMIGROUPS_TEST_CASE("to_presentation",
                          "001",
                          "from FroidurePin and alphabet",
                          "[quick][to_presentation]") {
    FroidurePin<Bipartition> S;
    S.add_generator(Bipartition({{1, -1}, {2, -2}, {3, -3}, {4, -4}}));
    S.add_generator(Bipartition({{1, -2}, {2, -3}, {3, -4}, {4, -1}}));
    S.add_generator(Bipartition({{1, -2}, {2, -1}, {3, -3}, {4, -4}}));
    S.add_generator(Bipartition({{1, 2}, {3, -3}, {4, -4}, {-1, -2}}));
    REQUIRE(S.size() == 105);

    auto p = to_presentation<std::string>(S);
    // Alphabet too small
    REQUIRE_THROWS_AS(presentation::change_alphabet(p, "abc"),
                      LibsemigroupsException);
    // Alphabet contains repeats
    REQUIRE_THROWS_AS(presentation::change_alphabet(p, "abca"),
                      LibsemigroupsException);
    // Alphabet too long
    REQUIRE_THROWS_AS(presentation::change_alphabet(p, "abcde"),
                      LibsemigroupsException);
    presentation::change_alphabet(p, "abcd");

    REQUIRE(p.alphabet().size() == 4);
    REQUIRE(p.rules[8] == "ba");
    REQUIRE(p.rules[9] == "b");
    REQUIRE(presentation::longest_subword_reducing_length(p) == "bcb");
  }

  LIBSEMIGROUPS_TEST_CASE("to_presentation",
                          "002",
                          "presentation from presentation",
                          "[quick][to_presentation]") {
    check_to_presentation_from_presentation<std::string, word_type>();
    check_to_presentation_from_presentation<std::string,
                                            StaticVector1<uint8_t, 3>>();
    check_to_presentation_from_presentation<word_type, std::string>();
    check_to_presentation_from_presentation<word_type,
                                            StaticVector1<uint8_t, 3>>();
    check_to_presentation_from_presentation<StaticVector1<uint8_t, 3>,
                                            word_type>();
    check_to_presentation_from_presentation<StaticVector1<uint8_t, 3>,
                                            std::string>();
  }

  LIBSEMIGROUPS_TEST_CASE("to_presentation",
                          "003",
                          "presentation from presentation and alphabet",
                          "[quick][to_presentation]") {
    Presentation<word_type> p;
    p.alphabet(2);
    p.contains_empty_word(false);
    presentation::add_rule_no_checks(p, {0, 1, 2}, {0, 1});
    presentation::add_rule_no_checks(p, {0, 1, 2}, {});
    // intentionally bad
    REQUIRE_THROWS_AS(p.validate(), LibsemigroupsException);
    REQUIRE_THROWS_AS(to_presentation<std::string>(p), LibsemigroupsException);

    p.alphabet_from_rules();
    REQUIRE(p.alphabet() == word_type({0, 1, 2}));
    p.validate();
    REQUIRE(p.contains_empty_word());
    auto q = to_presentation<std::string>(p);
    presentation::change_alphabet(q, "abc");
    REQUIRE(q.alphabet() == "abc");
    REQUIRE(q.contains_empty_word());
    REQUIRE(q.rules == std::vector<std::string>({"abc", "ab", "abc", ""}));
    q.validate();
  }

  LIBSEMIGROUPS_TEST_CASE("to_presentation",
                          "004",
                          "use human readable alphabet for to_presentation",
                          "[quick][presentation]") {
    Presentation<word_type> p;
    p.alphabet(2);
    p.contains_empty_word(true);
    presentation::add_rule(p, {0, 1}, {});

    auto q = to_presentation<std::string>(p);
    REQUIRE(q.alphabet() == "ab");
    REQUIRE(q.rules == std::vector<std::string>({"ab", ""}));
    q = to_presentation<std::string>(p);
    presentation::change_alphabet(q, "xy");
    REQUIRE(q.alphabet() == "xy");
    REQUIRE(q.rules == std::vector<std::string>({"xy", ""}));
  }

}  // namespace libsemigroups
