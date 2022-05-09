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

#include <cstddef>        // for size_t
#include <stdexcept>      // for runtime_error
#include <unordered_set>  // for unordered_set
#include <vector>         // for vector

#include "catch.hpp"  // for REQUIRE, REQUIRE_THROWS_AS, REQUI...

#include "libsemigroups/bipart.hpp"        // for Bipartition
#include "libsemigroups/containers.hpp"    // for StaticVector1
#include "libsemigroups/froidure-pin.hpp"  // for FroidurePin
#include "libsemigroups/knuth-bendix.hpp"  // for redundant_rule
#include "libsemigroups/make-present.hpp"  // for make
#include "libsemigroups/present.hpp"       // for Presentation
#include "libsemigroups/types.hpp"         // for word_type
#include "test-main.hpp"                   // for LIBSEMIGROUPS_TEST_CASE

namespace libsemigroups {

  namespace {

    template <typename W>
    void check_make_from_froidure_pin() {
      FroidurePin<Bipartition> S;
      S.add_generator(Bipartition({{1, -1}, {2, -2}, {3, -3}, {4, -4}}));
      S.add_generator(Bipartition({{1, -2}, {2, -3}, {3, -4}, {4, -1}}));
      S.add_generator(Bipartition({{1, -2}, {2, -1}, {3, -3}, {4, -4}}));
      S.add_generator(Bipartition({{1, 2}, {3, -3}, {4, -4}, {-1, -2}}));
      REQUIRE(S.size() == 105);

      auto p = make<Presentation<W>>(S);
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
    void check_make_from_presentation() {
      Presentation<W1> p;
      p.alphabet(2);
      p.contains_empty_word(false);
      presentation::add_rule(p, {0, 1, 2}, {0, 1});
      presentation::add_rule(p, {0, 1, 2}, {});
      // intentionally bad
      REQUIRE_THROWS_AS(p.validate(), LibsemigroupsException);
      REQUIRE_THROWS_AS(make<Presentation<std::string>>(p),
                        LibsemigroupsException);

      p.alphabet_from_rules();
      REQUIRE(p.alphabet() == W1({0, 1, 2}));
      p.validate();
      REQUIRE(p.contains_empty_word());
      auto q = make<Presentation<W2>>(p);
      REQUIRE(q.alphabet() == W2({0, 1, 2}));
      REQUIRE(q.contains_empty_word());
      REQUIRE(q.rules == std::vector<W2>({{0, 1, 2}, {0, 1}, {0, 1, 2}, {}}));
      q.validate();
    }
  }  // namespace

  struct LibsemigroupsException;  // forward decl
  using detail::StaticVector1;

  LIBSEMIGROUPS_TEST_CASE("make", "000", "from FroidurePin", "[quick][make]") {
    auto rg = ReportGuard(false);
    check_make_from_froidure_pin<word_type>();
    check_make_from_froidure_pin<StaticVector1<uint16_t, 8>>();
    check_make_from_froidure_pin<std::string>();
  }

  LIBSEMIGROUPS_TEST_CASE("make",
                          "001",
                          "from FroidurePin and alphabet",
                          "[quick][make]") {
    FroidurePin<Bipartition> S;
    S.add_generator(Bipartition({{1, -1}, {2, -2}, {3, -3}, {4, -4}}));
    S.add_generator(Bipartition({{1, -2}, {2, -3}, {3, -4}, {4, -1}}));
    S.add_generator(Bipartition({{1, -2}, {2, -1}, {3, -3}, {4, -4}}));
    S.add_generator(Bipartition({{1, 2}, {3, -3}, {4, -4}, {-1, -2}}));
    REQUIRE(S.size() == 105);
    // Alphabet too small
    REQUIRE_THROWS_AS(make(S, "abc"), LibsemigroupsException);
    // Alphabet contains repeats
    REQUIRE_THROWS_AS(make(S, "abca"), LibsemigroupsException);
    // Alphabet too long
    REQUIRE_THROWS_AS(make(S, "abcde"), LibsemigroupsException);

    auto p = make(S, "abcd");
    REQUIRE(p.alphabet().size() == 4);
    REQUIRE(p.rules[8] == "ba");
    REQUIRE(p.rules[9] == "b");
    REQUIRE(presentation::longest_common_subword(p) == "bcb");
  }

  LIBSEMIGROUPS_TEST_CASE("make",
                          "002",
                          "presentation from presentation",
                          "[quick][make]") {
    check_make_from_presentation<std::string, word_type>();
    check_make_from_presentation<std::string, StaticVector1<uint8_t, 3>>();
    check_make_from_presentation<word_type, std::string>();
    check_make_from_presentation<word_type, StaticVector1<uint8_t, 3>>();
    check_make_from_presentation<StaticVector1<uint8_t, 3>, word_type>();
    check_make_from_presentation<StaticVector1<uint8_t, 3>, std::string>();
  }

  LIBSEMIGROUPS_TEST_CASE("make",
                          "003",
                          "presentation from presentation and alphabet",
                          "[quick][make]") {
    Presentation<word_type> p;
    p.alphabet(2);
    p.contains_empty_word(false);
    presentation::add_rule(p, {0, 1, 2}, {0, 1});
    presentation::add_rule(p, {0, 1, 2}, {});
    // intentionally bad
    REQUIRE_THROWS_AS(p.validate(), LibsemigroupsException);
    REQUIRE_THROWS_AS(make<Presentation<std::string>>(p),
                      LibsemigroupsException);

    p.alphabet_from_rules();
    REQUIRE(p.alphabet() == word_type({0, 1, 2}));
    p.validate();
    REQUIRE(p.contains_empty_word());
    auto q = make(p, "abc");
    REQUIRE(q.alphabet() == "abc");
    REQUIRE(q.contains_empty_word());
    REQUIRE(q.rules == std::vector<std::string>({"abc", "ab", "abc", ""}));
    q.validate();
  }

}  // namespace libsemigroups
