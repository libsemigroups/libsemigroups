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

#include <algorithm>         // for all_of, equal, fill, sort
#include <cctype>            // for isprint
#include <chrono>            // for milliseconds
#include <cmath>             // for pow
#include <cstddef>           // for size_t
#include <cstdint>           // for uint16_t, uint8_t
#include <initializer_list>  // for initializer_list
#include <iterator>          // for distance
#include <string>            // for basic_string, operator==
#include <string_view>       // for string_view
#include <tuple>             // for _Swallow_assign, ignore
#include <type_traits>       // for is_same, is_unsigned_v
#include <unordered_map>     // for operator==, operator!=
#include <unordered_set>     // for unordered_set
#include <utility>           // for move, make_pair, swap
#include <vector>            // for vector, operator==, swap

#include "Catch2-3.8.0/catch_amalgamated.hpp"  // for operator""_catch_sr
#include "test-main.hpp"                       // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/ranges.hpp"  // for ChainRange, get_range...

#include "libsemigroups/bipart.hpp"           // for Bipartition
#include "libsemigroups/constants.hpp"        // for operator==, operator!=
#include "libsemigroups/debug.hpp"            // for LIBSEMIGROUPS_ASSERT
#include "libsemigroups/exception.hpp"        // for LibsemigroupsException
#include "libsemigroups/froidure-pin.hpp"     // for FroidurePin
#include "libsemigroups/knuth-bendix.hpp"     // for redundant_rule
#include "libsemigroups/order.hpp"            // for ShortLexCompare, shor...
#include "libsemigroups/presentation.hpp"     // for Presentation, human_r...
#include "libsemigroups/ranges.hpp"           // for chain, shortlex_compare
#include "libsemigroups/to-presentation.hpp"  // for to<Presentation>
#include "libsemigroups/types.hpp"            // for word_type, letter_type
#include "libsemigroups/word-range.hpp"       // for operator+=, operator""_w

#include "libsemigroups/detail/containers.hpp"  // for StaticVector1, operat...
#include "libsemigroups/detail/int-range.hpp"   // for IntRange
#include "libsemigroups/detail/report.hpp"      // for ReportGuard
#include "libsemigroups/detail/string.hpp"      // for operator<<

// TODO(1) Add tests for add_cyclic_conjugates
// TODO(1) Change word_type({0, 1, 2}) to "012"_w
// TODO(1) use LIBSEMIGROUPS_TEMPLATE_TEST_CASE + add
// Presentation<std::basic_string<uint8_t>>

namespace libsemigroups {

  using literals::operator""_w;
  using detail::StaticVector1;

  struct LibsemigroupsException;  // forward decl

  namespace {
    template <typename W>
    void check_constructors(Presentation<W>& p) {
      p.throw_if_bad_alphabet_or_rules();
      Presentation<W> pp(p);
      pp.throw_if_bad_alphabet_or_rules();
      REQUIRE(pp.alphabet() == p.alphabet());
      REQUIRE(pp.rules == p.rules);

      Presentation<W> q(std::move(p));
      q.throw_if_bad_alphabet_or_rules();
      REQUIRE(q.alphabet() == pp.alphabet());
      REQUIRE(q.rules == pp.rules);

      p = q;
      p.throw_if_bad_alphabet_or_rules();
      REQUIRE(q.alphabet() == p.alphabet());
      REQUIRE(q.rules == p.rules);

      p = std::move(q);
      p.throw_if_bad_alphabet_or_rules();
      REQUIRE(pp.alphabet() == p.alphabet());
      REQUIRE(pp.rules == p.rules);
    }

    template <typename W>
    void check_alphabet_letters() {
      Presentation<W> p;
      p.alphabet({0, 1, 2});
      REQUIRE(p.alphabet() == W({0, 1, 2}));
      REQUIRE(p.letter_no_checks(0) == 0);
      REQUIRE(p.letter_no_checks(1) == 1);
      REQUIRE(p.letter_no_checks(2) == 2);
      p.alphabet(4);
      if constexpr (std::is_same<W, std::string>::value) {
        REQUIRE(p.alphabet() == "abcd");
      } else {
        REQUIRE(p.alphabet() == W({0, 1, 2, 3}));
      }
      p.throw_if_bad_alphabet_or_rules();
      REQUIRE_THROWS_AS(p.alphabet({0, 1, 1}), LibsemigroupsException);

      presentation::add_rule_no_checks(p, {0, 1, 2, 1}, {0, 0});
      presentation::add_rule_no_checks(p, {4, 1}, {0, 5});
      presentation::add_rule_no_checks(
          p, {4, 1}, {0, 1, 1, 1, 1, 1, 1, 1, 1, 1});
      p.alphabet_from_rules();
      REQUIRE(p.alphabet() == W({0, 1, 2, 4, 5}));
      REQUIRE(p.index(0) == 0);
      REQUIRE(p.index(1) == 1);
      REQUIRE(p.index(2) == 2);
      REQUIRE(p.index(4) == 3);
      REQUIRE(p.index(5) == 4);

      REQUIRE(!p.contains_empty_word());
      presentation::add_rule_no_checks(p, {4, 1}, {});
      p.alphabet_from_rules();
      REQUIRE(p.contains_empty_word());

      p.alphabet({0, 1, 2, 3});
      REQUIRE(p.alphabet() == W({0, 1, 2, 3}));
    }

    template <typename W>
    void check_contains_empty_word() {
      Presentation<W> p;
      REQUIRE(!p.contains_empty_word());
      p.contains_empty_word(true);
      REQUIRE(p.contains_empty_word());
      p.contains_empty_word(false);
      REQUIRE(!p.contains_empty_word());
    }

    template <typename W>
    void check_contains_rule() {
      Presentation<W> p;
      presentation::add_rule_no_checks(p, {0, 5}, {0, 4});
      presentation::add_rule_no_checks(
          p, {4, 1}, {0, 1, 1, 1, 1, 1, 1, 1, 1, 1});
      REQUIRE(presentation::contains_rule(p, {0, 5}, {0, 4}));
      REQUIRE(presentation::contains_rule(p, {0, 4}, {0, 5}));
      REQUIRE(presentation::contains_rule(
          p, {4, 1}, {0, 1, 1, 1, 1, 1, 1, 1, 1, 1}));
      REQUIRE(!presentation::contains_rule(p, {0, 4}, {4, 1}));
    }

    template <typename W>
    void check_validate_rules_throws() {
      Presentation<W> p;
      p.rules.emplace_back();
      REQUIRE_THROWS_AS(p.throw_if_bad_rules(), LibsemigroupsException);
    }

    template <typename W>
    void check_add_rules() {
      Presentation<W> p;
      presentation::add_rule_no_checks(p, {0, 1, 2, 1}, {0, 0});
      Presentation<W> q;
      presentation::add_rule_no_checks(q, {4, 1}, {0, 5});
      presentation::add_rule_no_checks(
          q, {4, 1}, {0, 1, 1, 1, 1, 1, 1, 1, 1, 1});
      REQUIRE_THROWS_AS(presentation::add_rules(p, q), LibsemigroupsException);
      presentation::add_rules_no_checks(p, q);
      REQUIRE(p.rules
              == std::vector<W>({{0, 1, 2, 1},
                                 {0, 0},
                                 {4, 1},
                                 {0, 5},
                                 {4, 1},
                                 {0, 1, 1, 1, 1, 1, 1, 1, 1, 1}}));
      REQUIRE(q.rules
              == std::vector<W>(
                  {{4, 1}, {0, 5}, {4, 1}, {0, 1, 1, 1, 1, 1, 1, 1, 1, 1}}));
      REQUIRE_THROWS_AS(p.throw_if_bad_alphabet_or_rules(),
                        LibsemigroupsException);
      REQUIRE_THROWS_AS(q.throw_if_bad_alphabet_or_rules(),
                        LibsemigroupsException);
      p.alphabet_from_rules();
      q.alphabet_from_rules();
      presentation::add_rule_no_checks(q, {0}, {1});
      presentation::add_rules(p, q);
      REQUIRE(p.rules
              == std::vector<W>({{0, 1, 2, 1},
                                 {0, 0},
                                 {4, 1},
                                 {0, 5},
                                 {4, 1},
                                 {0, 1, 1, 1, 1, 1, 1, 1, 1, 1},
                                 {4, 1},
                                 {0, 5},
                                 {4, 1},
                                 {0, 1, 1, 1, 1, 1, 1, 1, 1, 1},
                                 {0},
                                 {1}}));
    }

    template <typename W>
    void check_add_identity_rules() {
      Presentation<W> p;
      presentation::add_rule_no_checks(p, {0, 1, 2, 1}, {0, 0});
      REQUIRE_THROWS_AS(presentation::add_identity_rules(p, 0),
                        LibsemigroupsException);
      p.alphabet_from_rules();
      presentation::add_identity_rules(p, 0);
      REQUIRE(p.rules
              == std::vector<W>({{0, 1, 2, 1},
                                 {0, 0},
                                 {0, 0},
                                 {0},
                                 {1, 0},
                                 {1},
                                 {0, 1},
                                 {1},
                                 {2, 0},
                                 {2},
                                 {0, 2},
                                 {2}}));
    }

    template <typename W>
    void check_add_zero_rules() {
      Presentation<W> p;
      presentation::add_rule_no_checks(p, {0, 1, 2, 1}, {0, 0});
      REQUIRE_THROWS_AS(presentation::add_zero_rules(p, 0),
                        LibsemigroupsException);
      p.alphabet_from_rules();
      presentation::add_zero_rules(p, 0);
      REQUIRE(p.rules
              == std::vector<W>({{0, 1, 2, 1},
                                 {0, 0},
                                 {0, 0},
                                 {0},
                                 {1, 0},
                                 {0},
                                 {0, 1},
                                 {0},
                                 {2, 0},
                                 {0},
                                 {0, 2},
                                 {0}}));
    }

    template <typename W>
    void check_add_commutes_rules() {
      Presentation<W> p;
      presentation::add_rule_no_checks(p, {0, 1, 2, 1}, {0, 0});
      presentation::add_commutes_rules_no_checks(p, {0}, {1});
      p.alphabet_from_rules();
      REQUIRE(p.rules
              == std::vector<W>({{0, 1, 2, 1}, {0, 0}, {0, 1}, {1, 0}}));

      presentation::add_commutes_rules_no_checks(p, {1, 1}, {2});
      REQUIRE(p.rules
              == std::vector<W>({{0, 1, 2, 1},
                                 {0, 0},
                                 {0, 1},
                                 {1, 0},
                                 {2, 1},
                                 {1, 2},
                                 {2, 1},
                                 {1, 2}}));

      presentation::add_commutes_rules_no_checks(p, {2});
      REQUIRE(p.rules
              == std::vector<W>({{0, 1, 2, 1},
                                 {0, 0},
                                 {0, 1},
                                 {1, 0},
                                 {2, 1},
                                 {1, 2},
                                 {2, 1},
                                 {1, 2}}));

      presentation::add_commutes_rules_no_checks(p, {2, 0});
      REQUIRE(p.rules
              == std::vector<W>({{0, 1, 2, 1},
                                 {0, 0},
                                 {0, 1},
                                 {1, 0},
                                 {2, 1},
                                 {1, 2},
                                 {2, 1},
                                 {1, 2},
                                 {2, 0},
                                 {0, 2}}));

      presentation::add_commutes_rules_no_checks(p,
                                                 {1, 2},
                                                 {{0, 0, 1},
                                                  {
                                                      1,
                                                      0,
                                                  }});
      REQUIRE(p.rules
              == std::vector<W>({
                  {0, 1, 2, 1},
                  {0, 0},
                  {0, 1},
                  {1, 0},
                  {2, 1},
                  {1, 2},
                  {2, 1},
                  {1, 2},
                  {2, 0},
                  {0, 2},
                  {1, 0, 0, 1},
                  {0, 0, 1, 1},
                  {1, 1, 0},
                  {1, 0, 1},
                  {2, 0, 0, 1},
                  {0, 0, 1, 2},
                  {2, 1, 0},
                  {1, 0, 2},
              }));
    }

    template <typename W>
    void check_add_idempotent_rules() {
      Presentation<W> p;
      presentation::add_rule_no_checks(p, {0, 1, 2, 1}, {0, 0});
      presentation::add_idempotent_rules_no_checks(p, {0, 1});
      p.alphabet_from_rules();
      REQUIRE(
          p.rules
          == std::vector<W>({{0, 1, 2, 1}, {0, 0}, {0, 0}, {0}, {1, 1}, {1}}));
    }

    template <typename W>
    void check_add_involution_rules() {
      Presentation<W> p;
      presentation::add_rule_no_checks(p, {0, 1, 2, 1}, {0, 0});
      presentation::add_involution_rules_no_checks(p, {0, 1});
      REQUIRE(
          p.rules
          == std::vector<W>({{0, 1, 2, 1}, {0, 0}, {0, 0}, {}, {1, 1}, {}}));
      REQUIRE_THROWS_AS(p.throw_if_bad_alphabet_or_rules(),
                        LibsemigroupsException);
      p.alphabet_from_rules();
      p.contains_empty_word(true);
      p.throw_if_bad_alphabet_or_rules();
    }

    template <typename W>
    void check_add_inverse_rules() {
      Presentation<W> p;
      presentation::add_rule_no_checks(p, {0, 1, 2, 1}, {0, 0});
      p.alphabet_from_rules();

      REQUIRE_THROWS_AS(presentation::add_inverse_rules(p, {0, 1, 1}, 0),
                        LibsemigroupsException);
      REQUIRE_THROWS_AS(presentation::add_inverse_rules(p, {1, 2, 0}, 0),
                        LibsemigroupsException);
      p.alphabet({0, 1, 2, 3});
      REQUIRE_THROWS_AS(presentation::add_inverse_rules(p, {0, 2, 3, 1}, 0),
                        LibsemigroupsException);
      REQUIRE_THROWS_AS(presentation::add_inverse_rules(p, {0, 2, 1}, 0),
                        LibsemigroupsException);
      p.alphabet({0, 1, 2});
      presentation::add_inverse_rules(p, {0, 2, 1}, 0);

      REQUIRE(
          p.rules
          == std::vector<W>({{0, 1, 2, 1}, {0, 0}, {1, 2}, {0}, {2, 1}, {0}}));
      // When id is UNDEFINED
      p.contains_empty_word(true);
      presentation::add_inverse_rules(p, {0, 2, 1});
      REQUIRE(p.rules
              == std::vector<W>({{0, 1, 2, 1},
                                 {0, 0},
                                 {1, 2},
                                 {0},
                                 {2, 1},
                                 {0},
                                 {0, 0},
                                 {},
                                 {1, 2},
                                 {},
                                 {2, 1},
                                 {}}));
    }

    template <typename W>
    void check_remove_duplicate_rules() {
      Presentation<W> p;
      p.rules.push_back(W({0, 1, 2, 1}));
      REQUIRE_THROWS_AS(presentation::remove_duplicate_rules(p),
                        LibsemigroupsException);
      p.rules.push_back(W({0, 0}));
      presentation::add_rule_no_checks(p, {0, 0}, {0, 1, 2, 1});
      p.alphabet_from_rules();
      REQUIRE(p.rules.size() == 4);
      presentation::remove_duplicate_rules(p);
      REQUIRE(p.rules.size() == 2);
    }

    template <typename W>
    void check_remove_generator() {
      using words::human_readable_letter;
      Presentation<W> p;
      p.alphabet(10);
      p.remove_generator_no_checks(human_readable_letter<W>(4));
      p.remove_generator(human_readable_letter<W>(7));
      p.remove_generator_no_checks(human_readable_letter<W>(9));
      if constexpr (std::is_same_v<W, std::string>) {
        REQUIRE(p.alphabet() == "abcdfgi");
      } else {
        REQUIRE(p.alphabet() == W({0, 1, 2, 3, 5, 6, 8}));
      }
      REQUIRE(p.index(human_readable_letter<W>(0)) == 0);
      REQUIRE(p.index(human_readable_letter<W>(1)) == 1);
      REQUIRE(p.index(human_readable_letter<W>(2)) == 2);
      REQUIRE(p.index(human_readable_letter<W>(3)) == 3);
      REQUIRE_THROWS_AS(p.index(human_readable_letter<W>(4)),
                        LibsemigroupsException);
      REQUIRE(p.index(human_readable_letter<W>(5)) == 4);
      REQUIRE(p.index(human_readable_letter<W>(6)) == 5);
      REQUIRE_THROWS_AS(p.index(human_readable_letter<W>(7)),
                        LibsemigroupsException);
      REQUIRE(p.index(human_readable_letter<W>(8)) == 6);
      REQUIRE_THROWS_AS(p.index(human_readable_letter<W>(9)),
                        LibsemigroupsException);
      REQUIRE(p.letter(0) == human_readable_letter<W>(0));
      REQUIRE(p.letter(1) == human_readable_letter<W>(1));
      REQUIRE(p.letter(2) == human_readable_letter<W>(2));
      REQUIRE(p.letter(3) == human_readable_letter<W>(3));
      REQUIRE(p.letter(4) == human_readable_letter<W>(5));
      REQUIRE(p.letter(5) == human_readable_letter<W>(6));
      REQUIRE(p.letter(6) == human_readable_letter<W>(8));

      REQUIRE_THROWS_AS(p.remove_generator(human_readable_letter<W>(11)),
                        LibsemigroupsException);
    }

    template <typename W>
    void check_reduce_complements() {
      Presentation<W> p;
      p.rules.push_back(W({0, 1, 2, 1}));
      REQUIRE_THROWS_AS(presentation::reduce_complements(p),
                        LibsemigroupsException);
      p.rules.push_back(W({1, 2, 1}));

      presentation::add_rule_no_checks(p, {1, 2, 1}, {1, 1, 2, 1});
      presentation::add_rule_no_checks(p, {1, 1, 2, 1}, {1, 1});
      presentation::add_rule_no_checks(p, {1, 1}, {1, 2, 1});
      presentation::add_rule_no_checks(p, {1, 2, 1}, {0});
      p.alphabet_from_rules();
      presentation::reduce_complements(p);
      presentation::sort_each_rule(p);
      presentation::sort_rules(p);
      REQUIRE(p.rules
              == std::vector<W>({{1, 1},
                                 {0},
                                 {1, 2, 1},
                                 {0},
                                 {0, 1, 2, 1},
                                 {0},
                                 {1, 1, 2, 1},
                                 {0}}));
    }

    template <typename W>
    void check_balance() {
      Presentation<W> p;
      p.contains_empty_word(true);
      presentation::add_rule_no_checks(p, {1, 1, 1, 1, 1, 1, 1, 1}, {});
      REQUIRE(p.rules == std::vector<W>({{1, 1, 1, 1, 1, 1, 1, 1}, {}}));
      presentation::balance_no_checks(p, {1}, {1});
      REQUIRE(p.rules == std::vector<W>({{1, 1, 1, 1}, {1, 1, 1, 1}}));

      presentation::add_rule_no_checks(p, {1, 1, 1}, {1, 1, 1, 1, 1, 1});
      presentation::balance_no_checks(p, {1}, {1});
      REQUIRE(p.rules
              == std::vector<W>(
                  {{1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1, 1}, {1, 1, 1, 1}}));

      presentation::add_rule_no_checks(p, {1, 1}, {});
      REQUIRE(p.rules
              == std::vector<W>({{1, 1, 1, 1},
                                 {1, 1, 1, 1},
                                 {1, 1, 1, 1, 1},
                                 {1, 1, 1, 1},
                                 {1, 1},
                                 {}}));
      presentation::balance_no_checks(p, {1}, {1});
      REQUIRE(p.rules
              == std::vector<W>({{1, 1, 1, 1},
                                 {1, 1, 1, 1},
                                 {1, 1, 1, 1, 1},
                                 {1, 1, 1, 1},
                                 {1, 1},
                                 {}}));

      presentation::add_rule_no_checks(
          p, {1, 2, 2, 1}, {2, 1, 1, 1, 1, 1, 1, 1, 1, 2});
      presentation::balance_no_checks(p, {2}, {1});
      REQUIRE(p.rules
              == std::vector<W>({{1, 1, 1, 1},
                                 {1, 1, 1, 1},
                                 {1, 1, 1, 1, 1},
                                 {1, 1, 1, 1},
                                 {1, 1},
                                 {},
                                 {1, 1, 1, 1, 1, 1, 1, 1},
                                 {1, 1, 2, 2, 1, 1}}));
      presentation::balance_no_checks(p, {1}, {3});
      REQUIRE(p.rules
              == std::vector<W>({{1, 1, 1, 1},
                                 {1, 1, 1, 1},
                                 {1, 1, 1, 1, 1},
                                 {1, 1, 1, 1},
                                 {1},
                                 {3},
                                 {1, 1, 1, 1, 1, 1, 1},
                                 {1, 1, 2, 2, 1, 1, 3}}));
      presentation::add_rule_no_checks(p, {2, 1, 1, 1, 1, 1, 1, 2, 2, 2}, {});
      presentation::balance_no_checks(p, {1, 2}, {3, 4});
      REQUIRE(p.rules
              == std::vector<W>({{1, 1, 1, 1},
                                 {1, 1, 1, 1},
                                 {1, 1, 1, 1, 1},
                                 {1, 1, 1, 1},
                                 {3},
                                 {1},
                                 {1, 1, 2, 2, 1, 1, 3},
                                 {1, 1, 1, 1, 1, 1, 1},
                                 {2, 1, 1, 1, 1},
                                 {4, 4, 4, 3, 3}}));
      presentation::add_rule_no_checks(p, {1, 2, 3, 1, 2, 4}, {});
      presentation::balance_no_checks(p, {1, 2, 3}, {5, 6, 7});
      REQUIRE(p.rules
              == std::vector<W>({{1, 1, 1, 1},
                                 {1, 1, 1, 1},
                                 {1, 1, 1, 1, 1},
                                 {1, 1, 1, 1},
                                 {3},
                                 {1},
                                 {1, 1, 2, 2, 1, 1, 3},
                                 {1, 1, 1, 1, 1, 1, 1},
                                 {4, 4, 4, 3, 3},
                                 {2, 1, 1, 1, 1},
                                 {1, 2, 4},
                                 {7, 6, 5}}));
    }

    template <typename W>
    void check_sort_each_rule() {
      Presentation<W> p;
      p.rules.push_back(W({0, 1, 2, 1}));
      REQUIRE_THROWS_AS(presentation::sort_each_rule(p),
                        LibsemigroupsException);
      p.rules.push_back(W({1, 2, 1}));

      presentation::add_rule_no_checks(p, {1, 2, 1}, {1, 1, 2, 1});
      presentation::add_rule_no_checks(p, {1, 1, 2, 1}, {1, 1});
      presentation::add_rule_no_checks(p, {1, 1}, {1, 2, 1});
      presentation::add_rule_no_checks(p, {1, 2, 1}, {0});
      p.alphabet_from_rules();
      presentation::sort_each_rule(p);
      REQUIRE(p.rules
              == std::vector<W>({{0, 1, 2, 1},
                                 {1, 2, 1},
                                 {1, 1, 2, 1},
                                 {1, 2, 1},
                                 {1, 1, 2, 1},
                                 {1, 1},
                                 {1, 2, 1},
                                 {1, 1},
                                 {1, 2, 1},
                                 {0}}));
    }

    template <typename W>
    void check_sort_rules() {
      Presentation<W> p;
      p.rules.push_back(W({0, 1, 2, 1}));
      REQUIRE_THROWS_AS(presentation::sort_rules(p), LibsemigroupsException);
      p.rules.push_back(W({1, 2, 1}));
      presentation::add_rule_no_checks(p, {1, 2, 1}, {1, 1, 2, 1});
      presentation::add_rule_no_checks(p, {1, 1, 2, 1}, {1, 1});
      presentation::add_rule_no_checks(p, {1, 1}, {1, 2, 1});
      presentation::add_rule_no_checks(p, {1, 2, 1}, {0});
      p.alphabet_from_rules();
      presentation::sort_rules(p);
      REQUIRE(p.rules
              == std::vector<W>({{1, 2, 1},
                                 {0},
                                 {1, 1},
                                 {1, 2, 1},
                                 {1, 1, 2, 1},
                                 {1, 1},
                                 {0, 1, 2, 1},
                                 {1, 2, 1},
                                 {1, 2, 1},
                                 {1, 1, 2, 1}}));
      REQUIRE(presentation::are_rules_sorted(p));
    }

    template <typename W>
    void check_longest_subword_reducing_length() {
      {
        // Normalized alphabet
        Presentation<W> p;
        p.rules.push_back(W({0, 1, 2, 1}));
        REQUIRE_NOTHROW(presentation::longest_subword_reducing_length(p));
        p.rules.push_back(W({1, 2, 1}));
        presentation::add_rule_no_checks(p, {1, 2, 1}, {1, 1, 2, 1});
        presentation::add_rule_no_checks(p, {1, 1, 2, 1}, {1, 1});
        presentation::add_rule_no_checks(p, {1, 1}, {1, 2, 1});
        presentation::add_rule_no_checks(p, {1, 2, 1}, {0});
        p.alphabet_from_rules();
        REQUIRE(presentation::longest_subword_reducing_length(p)
                == W({1, 2, 1}));
        presentation::replace_subword(p, W({1, 2, 1}), W({3}));
        presentation::add_rule_no_checks(p, {3}, {1, 2, 1});
        REQUIRE(p.rules
                == std::vector<W>({{0, 3},
                                   {3},
                                   {3},
                                   {1, 3},
                                   {1, 3},
                                   {1, 1},
                                   {1, 1},
                                   {3},
                                   {3},
                                   {0},
                                   {3},
                                   {1, 2, 1}}));
      }
      {
        // Non-normalized alphabet
        Presentation<W> p;
        presentation::add_rule_no_checks(p, {1, 2, 4, 2}, {2, 4, 2});
        presentation::add_rule_no_checks(p, {2, 4, 2}, {2, 2, 4, 2});
        presentation::add_rule_no_checks(p, {2, 2, 4, 2}, {2, 2});
        presentation::add_rule_no_checks(p, {2, 2}, {2, 4, 2});
        presentation::add_rule_no_checks(p, {2, 4, 2}, {1});
        p.alphabet_from_rules();
        REQUIRE(presentation::longest_subword_reducing_length(p)
                == W({2, 4, 2}));
        presentation::replace_subword(p, W({2, 4, 2}), W({0}));
        presentation::add_rule_no_checks(p, W({0}), W({2, 4, 2}));
        REQUIRE(p.rules
                == std::vector<W>({{1, 0},
                                   {0},
                                   {0},
                                   {2, 0},
                                   {2, 0},
                                   {2, 2},
                                   {2, 2},
                                   {0},
                                   {0},
                                   {1},
                                   {0},
                                   {2, 4, 2}}));
      }
    }

    template <typename W>
    void check_redundant_rule() {
      FroidurePin<Bipartition> S;
      S.add_generator(Bipartition({{1, -1}, {2, -2}, {3, -3}, {4, -4}}));
      S.add_generator(Bipartition({{1, -2}, {2, -3}, {3, -4}, {4, -1}}));
      S.add_generator(Bipartition({{1, -2}, {2, -1}, {3, -3}, {4, -4}}));
      S.add_generator(Bipartition({{1, 2}, {3, -3}, {4, -4}, {-1, -2}}));
      REQUIRE(S.size() == 105);

      auto p = to<Presentation<W>>(S);
      REQUIRE(presentation::length(p) == 359);
      presentation::remove_duplicate_rules(p);
      REQUIRE(presentation::length(p) == 359);
      presentation::reduce_complements(p);
      REQUIRE(presentation::length(p) == 359);
      presentation::sort_each_rule(p);
      presentation::sort_rules(p);

      REQUIRE(presentation::length(p) == 359);
      REQUIRE(p.rules.size() == 86);

      p.alphabet_from_rules();
      auto it = knuth_bendix::redundant_rule(p, std::chrono::milliseconds(100));
      // This is commented out because redundant_rule is non-deterministic
      // REQUIRE(*it == W({2, 1, 3, 1, 1, 2, 1, 2}));
      // REQUIRE(*(it + 1) == W({1, 1, 2, 1, 3, 1, 2, 1}));
      p.rules.erase(it, it + 2);
      p.throw_if_bad_alphabet_or_rules();
      // while (it != p.rules.cend()) { // Too time consuming and indeterminant
      //   REQUIRE(std::distance(it, p.rules.cend()) % 2 == 0);
      //   p.rules.erase(it, it + 2);
      //   p.throw_if_bad_alphabet_or_rules();
      //   it = knuth_bendix::redundant_rule(p, std::chrono::milliseconds(8));
      // }
      // REQUIRE(presentation::length(p) == 343);
      REQUIRE(p.rules.size() == 84);
    }

    // template <typename W>
    // void check_shortlex_compare_concat() {
    //   REQUIRE(detail::shortlex_compare_concat(
    //       W({0, 1, 2, 1}), W({0}), W({1, 1, 2, 1}), W({0})));
    // }

    template <typename W>
    void check_remove_trivial_rules() {
      Presentation<W> p;
      p.rules.push_back(W({0, 1, 2, 1}));
      REQUIRE_THROWS_AS(presentation::remove_trivial_rules(p),
                        LibsemigroupsException);
      p.rules.push_back(W({1, 2, 1}));
      presentation::add_rule_no_checks(p, {1, 2, 1}, {1, 2, 1});
      presentation::add_rule_no_checks(p, {1, 1, 2, 1}, {1, 1});
      presentation::add_rule_no_checks(p, {1, 2, 1}, {1, 2, 1});
      presentation::add_rule_no_checks(p, {1, 2, 1}, {0});
      presentation::add_rule_no_checks(p, {0}, {0});
      presentation::add_rule_no_checks(p, {1}, {1});
      presentation::add_rule_no_checks(p, {2}, {2});

      presentation::remove_trivial_rules(p);
      REQUIRE(
          p.rules
          == std::vector<W>(
              {{0, 1, 2, 1}, {1, 2, 1}, {1, 1, 2, 1}, {1, 1}, {1, 2, 1}, {0}}));
      presentation::remove_trivial_rules(p);
      REQUIRE(
          p.rules
          == std::vector<W>(
              {{0, 1, 2, 1}, {1, 2, 1}, {1, 1, 2, 1}, {1, 1}, {1, 2, 1}, {0}}));
    }

    template <typename W>
    void check_replace_subword() {
      Presentation<W> p;
      p.rules.push_back(W({0, 1, 2, 1}));
      REQUIRE_NOTHROW(presentation::replace_subword(p, W({0}), W({1})));
      p.rules.push_back(W({1, 2, 1}));
      presentation::add_rule_no_checks(p, {1, 1, 2, 1}, {1, 1});
      presentation::add_rule_no_checks(p, {1, 2, 1}, {0});

      presentation::replace_subword(p, W({0}), W({1}));
      REQUIRE(
          p.rules
          == std::vector<W>(
              {{1, 1, 2, 1}, {1, 2, 1}, {1, 1, 2, 1}, {1, 1}, {1, 2, 1}, {1}}));

      presentation::replace_subword(p, W({0}), W({1}));
      REQUIRE(
          p.rules
          == std::vector<W>(
              {{1, 1, 2, 1}, {1, 2, 1}, {1, 1, 2, 1}, {1, 1}, {1, 2, 1}, {1}}));

      presentation::replace_subword(p, W({1, 2, 1}), W({0}));
      REQUIRE(p.rules
              == std::vector<W>({{1, 0}, {0}, {1, 0}, {1, 1}, {0}, {1}}));

      presentation::replace_subword(p, W({42, 42}), W({0}));
      REQUIRE(p.rules
              == std::vector<W>({{1, 0}, {0}, {1, 0}, {1, 1}, {0}, {1}}));

      p.rules.clear();
      presentation::add_rule_no_checks(
          p, {1, 2, 1, 2, 1, 1, 2, 1, 2, 1, 1, 2, 1}, {1, 2, 1, 1, 2, 1, 2, 1});
      presentation::replace_subword(p, W({1, 2, 1}), W({1}));
      REQUIRE(p.rules == std::vector<W>({{1, 2, 1, 1, 2, 1, 1}, {1, 1, 2, 1}}));
      presentation::replace_subword(p, W({1, 2, 1}), W({1}));
      REQUIRE(p.rules == std::vector<W>({{1, 1, 1}, {1, 1}}));
      // Test for when existing is a suffix of replacement
      p.rules.clear();
      presentation::add_rule_no_checks(
          p, {1, 2, 1, 2, 1, 1, 2, 1, 2, 1, 1, 2, 1}, {1, 2, 1, 1, 2, 1, 2, 1});
      presentation::replace_subword(p, W({1, 2}), W({1, 1, 2}));
      REQUIRE(p.rules
              == std::vector<W>(
                  {{1, 1, 2, 1, 1, 2, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 2, 1},
                   {1, 1, 2, 1, 1, 1, 2, 1, 1, 2, 1}}));
    }

    template <typename W>
    void check_replace_word() {
      Presentation<W> p;
      presentation::add_rule_no_checks(p, {0, 1, 0}, {});
      p.alphabet_from_rules();
      presentation::replace_word(p, W({}), W({2}));
      REQUIRE(p.rules == std::vector<W>{{0, 1, 0}, {2}});

      p.rules.clear();
      presentation::add_rule_no_checks(p, {0, 1, 0}, {2, 1});
      presentation::add_rule_no_checks(p, {1, 1, 2}, {1, 2, 1});
      presentation::add_rule_no_checks(p, {2, 1, 2, 1}, {2, 2});
      presentation::add_rule_no_checks(p, {2, 1}, {0, 1, 1});
      p.alphabet_from_rules();
      presentation::replace_word(p, W({2, 1}), W({1, 2}));
      REQUIRE(p.rules
              == std::vector<W>{{0, 1, 0},
                                {1, 2},
                                {1, 1, 2},
                                {1, 2, 1},
                                {2, 1, 2, 1},
                                {2, 2},
                                {1, 2},
                                {0, 1, 1}});

      p.rules.clear();
      presentation::add_rule_no_checks(p, {0, 1, 0}, {1, 0, 1});
      presentation::add_rule_no_checks(p, {0, 1, 1}, {1, 0, 1, 0});
      p.alphabet_from_rules();
      presentation::replace_word(p, W({1, 0, 1}), W({}));
      REQUIRE(p.rules
              == std::vector<W>{{0, 1, 0}, {}, {0, 1, 1}, {1, 0, 1, 0}});
    }

    template <typename W>
    void check_longest_rule() {
      Presentation<W> p;
      REQUIRE(presentation::longest_rule(p) == p.rules.cend());
      REQUIRE(presentation::shortest_rule(p) == p.rules.cend());
      p.rules.push_back(W({0, 1, 2, 1}));
      REQUIRE_THROWS_AS(presentation::longest_rule(p), LibsemigroupsException);
      p.rules.push_back(W({1, 2, 1}));
      presentation::add_rule_no_checks(p, {1, 1, 2, 1}, {1, 1});
      presentation::add_rule_no_checks(p, {1, 2, 1}, {0});
      REQUIRE(*presentation::longest_rule(p) == W({0, 1, 2, 1}));
      REQUIRE_THROWS_AS(*presentation::longest_rule(
                            presentation::longest_rule(p) + 1, p.rules.cend()),
                        LibsemigroupsException);
      REQUIRE(*presentation::longest_rule(presentation::longest_rule(p) + 2,
                                          p.rules.cend())
              == W({1, 1, 2, 1}));
      REQUIRE(*presentation::shortest_rule(p) == W({1, 2, 1}));
      REQUIRE(*presentation::shortest_rule(p.rules.cbegin(),
                                           presentation::shortest_rule(p))
              == W({1, 1, 2, 1}));
      REQUIRE_THROWS_AS(
          *presentation::shortest_rule(p.rules.cbegin(),
                                       presentation::shortest_rule(p) - 1),
          LibsemigroupsException);
    }

    template <typename W>
    void check_longest_rule_length() {
      Presentation<W> p;
      REQUIRE(presentation::longest_rule_length(p) == 0);
      REQUIRE(presentation::shortest_rule_length(p) == 0);
      p.rules.push_back(W({0, 1, 2, 1}));
      REQUIRE_THROWS_AS(presentation::longest_rule_length(p),
                        LibsemigroupsException);
      p.rules.push_back(W({1, 2, 1}));
      presentation::add_rule_no_checks(p, {1, 1, 2, 1}, {1, 1});
      presentation::add_rule_no_checks(p, {1, 2, 1}, {0});
      REQUIRE(presentation::longest_rule_length(p) == 7);
      REQUIRE_THROWS_AS(presentation::longest_rule_length(
                            presentation::longest_rule(p) + 1, p.rules.cend()),
                        LibsemigroupsException);
      REQUIRE(presentation::longest_rule_length(
                  presentation::longest_rule(p) + 2, p.rules.cend())
              == 6);

      REQUIRE(presentation::shortest_rule_length(p) == 4);
      REQUIRE_THROWS_AS(presentation::shortest_rule_length(
                            presentation::shortest_rule(p) + 1, p.rules.cend()),
                        LibsemigroupsException);
      REQUIRE(presentation::shortest_rule_length(p.rules.cbegin(),
                                                 p.rules.cend() - 2)
              == 6);
    }

    template <typename W>
    void check_remove_redundant_generators() {
      Presentation<W> p;
      p.rules.push_back(W({0, 1, 2, 1}));
      p.alphabet(W({0, 1, 2}));
      REQUIRE_THROWS_AS(presentation::remove_redundant_generators(p),
                        LibsemigroupsException);
      p.rules.push_back(W({1, 2, 1}));

      presentation::add_rule_no_checks(p, {1, 1, 2, 1}, {1, 1});
      presentation::add_rule_no_checks(p, {1, 2, 1}, {0});

      presentation::remove_redundant_generators(p);
      REQUIRE(p.alphabet() == W({1, 2}));
      REQUIRE(p.rules
              == std::vector<W>(
                  {{1, 2, 1, 1, 2, 1}, {1, 2, 1}, {1, 1, 2, 1}, {1, 1}}));
      presentation::remove_redundant_generators(p);
      REQUIRE(p.alphabet() == W({1, 2}));
      REQUIRE(p.rules
              == std::vector<W>(
                  {{1, 2, 1, 1, 2, 1}, {1, 2, 1}, {1, 1, 2, 1}, {1, 1}}));

      p.rules.clear();
      p.alphabet(W({0, 1, 2}));
      presentation::add_rule_no_checks(p, {0, 1, 2, 1}, {1, 2, 1});
      presentation::add_rule_no_checks(p, {1, 1, 2, 1}, {1, 1});
      presentation::add_rule_no_checks(p, {1}, {0});
      presentation::add_rule_no_checks(p, {1, 2, 1}, {0});
      presentation::remove_redundant_generators(p);
      REQUIRE(p.alphabet() == W({0, 2}));
      REQUIRE(
          p.rules
          == std::vector<W>(
              {{0, 0, 2, 0}, {0, 2, 0}, {0, 0, 2, 0}, {0, 0}, {0, 2, 0}, {0}}));

      p.rules.clear();
      presentation::add_rule_no_checks(p, {0, 1, 2, 1}, {1, 2, 1});
      presentation::add_rule_no_checks(p, {1, 1, 2, 1}, {1, 1});
      presentation::add_rule_no_checks(p, {0}, {1});
      presentation::add_rule_no_checks(p, {1, 2, 1}, {0});
      presentation::remove_redundant_generators(p);
      REQUIRE(
          p.rules
          == std::vector<W>(
              {{0, 0, 2, 0}, {0, 2, 0}, {0, 0, 2, 0}, {0, 0}, {0, 2, 0}, {0}}));

      p.rules.clear();
      presentation::add_rule_no_checks(p, {0, 1, 2, 1}, {1, 2, 1});
      presentation::add_rule_no_checks(p, {1, 1, 2, 1}, {1, 1});
      presentation::add_rule_no_checks(p, {1}, {0});
      presentation::add_rule_no_checks(p, {1, 2, 1}, {0});
      presentation::remove_redundant_generators(p);
      REQUIRE(
          p.rules
          == std::vector<W>(
              {{0, 0, 2, 0}, {0, 2, 0}, {0, 0, 2, 0}, {0, 0}, {0, 2, 0}, {0}}));
    }

    template <typename W>
    void check_reverse() {
      Presentation<W> p;
      presentation::add_rule_no_checks(p, {0, 1, 2, 1}, {1, 2, 1});
      presentation::add_rule_no_checks(p, {1, 1, 2, 1}, {1, 1});
      presentation::add_rule_no_checks(p, {1, 2, 1}, {0});

      presentation::reverse(p);
      REQUIRE(
          p.rules
          == std::vector<W>(
              {{1, 2, 1, 0}, {1, 2, 1}, {1, 2, 1, 1}, {1, 1}, {1, 2, 1}, {0}}));

      presentation::reverse(p);
      REQUIRE(
          p.rules
          == std::vector<W>(
              {{0, 1, 2, 1}, {1, 2, 1}, {1, 1, 2, 1}, {1, 1}, {1, 2, 1}, {0}}));
    }

    template <typename W>
    void check_in_alphabet() {
      Presentation<W> p;
      presentation::add_rule_no_checks(p, {0, 1, 2, 1}, {1, 2, 1});
      presentation::add_rule_no_checks(p, {1, 1, 2, 1}, {1, 1});
      presentation::add_rule_no_checks(p, {1, 2, 1}, {0});
      // Alphabet not set, so everything false
      REQUIRE(!p.in_alphabet(0));
      REQUIRE(!p.in_alphabet(1));
      REQUIRE(!p.in_alphabet(2));
      REQUIRE(!p.in_alphabet(3));
      REQUIRE(!p.in_alphabet(42));

      p.alphabet_from_rules();
      REQUIRE(p.in_alphabet(0));
      REQUIRE(p.in_alphabet(1));
      REQUIRE(p.in_alphabet(2));
      REQUIRE(!p.in_alphabet(3));
      REQUIRE(!p.in_alphabet(42));
    }

    template <typename W>
    void check_make_semigroup() {
      Presentation<W> p;
      presentation::add_rule_no_checks(p, {0, 0}, {});
      presentation::add_rule_no_checks(p, {1, 1}, {});
      presentation::add_rule_no_checks(p, {2, 2}, {});
      presentation::add_rule_no_checks(p, {0, 1, 0, 1, 0, 1}, {});
      presentation::add_rule_no_checks(p, {1, 2, 1, 0, 1, 2, 1, 0}, {});
      presentation::add_rule_no_checks(p, {2, 0, 2, 1, 2, 0, 2, 1}, {0, 3});

      p.alphabet_from_rules();
      auto e = presentation::make_semigroup(p);
      REQUIRE(p.rules
              == std::vector<W>({{0, 0},
                                 {e},
                                 {1, 1},
                                 {e},
                                 {2, 2},
                                 {e},
                                 {0, 1, 0, 1, 0, 1},
                                 {e},
                                 {1, 2, 1, 0, 1, 2, 1, 0},
                                 {e},
                                 {2, 0, 2, 1, 2, 0, 2, 1},
                                 {0, 3},
                                 {0, e},
                                 {0},
                                 {e, 0},
                                 {0},
                                 {1, e},
                                 {1},
                                 {e, 1},
                                 {1},
                                 {2, e},
                                 {2},
                                 {e, 2},
                                 {2},
                                 {3, e},
                                 {3},
                                 {e, 3},
                                 {3},
                                 {e, e},
                                 {e}}));
      REQUIRE(presentation::make_semigroup(p) == UNDEFINED);
    }

    template <typename W>
    void check_inverse_constructors(InversePresentation<W> ip) {
      ip.throw_if_bad_alphabet_rules_or_inverses();
      InversePresentation<W> pp(ip);
      pp.throw_if_bad_alphabet_rules_or_inverses();
      REQUIRE(pp.alphabet() == ip.alphabet());
      REQUIRE(pp.rules == ip.rules);
      REQUIRE(pp.inverses() == ip.inverses());

      InversePresentation<W> q(std::move(ip));
      q.throw_if_bad_alphabet_rules_or_inverses();
      REQUIRE(q.alphabet() == pp.alphabet());
      REQUIRE(q.rules == pp.rules);
      REQUIRE(q.inverses() == pp.inverses());

      ip = q;
      ip.throw_if_bad_alphabet_rules_or_inverses();
      REQUIRE(q.alphabet() == ip.alphabet());
      REQUIRE(q.rules == ip.rules);
      REQUIRE(q.inverses() == ip.inverses());

      ip = std::move(q);
      ip.throw_if_bad_alphabet_rules_or_inverses();
      REQUIRE(pp.alphabet() == ip.alphabet());
      REQUIRE(pp.rules == ip.rules);
      REQUIRE(pp.inverses() == ip.inverses());
    }

    template <typename W>
    void check_construct_from_presentation() {
      Presentation<W> p;
      p.alphabet({0, 1, 2});
      presentation::add_rule_no_checks(p, {0, 0, 0}, {0});
      presentation::add_rule(p, {0, 0, 0}, {0});
      InversePresentation<W> ip(p);
      REQUIRE_THROWS_AS(ip.throw_if_bad_alphabet_rules_or_inverses(),
                        LibsemigroupsException);
      REQUIRE(ip.alphabet() == p.alphabet());
      REQUIRE(ip.rules == ip.rules);
      REQUIRE(ip.inverses() == W({}));

      InversePresentation<W> ip2(std::move(p));
      REQUIRE_THROWS_AS(ip2.throw_if_bad_alphabet_rules_or_inverses(),
                        LibsemigroupsException);
      REQUIRE(ip2.alphabet() == ip.alphabet());
      REQUIRE(ip2.rules == ip.rules);
      REQUIRE(ip2.inverses() == W({}));
      REQUIRE(ip == ip2);

      ip.inverses_no_checks({2, 1, 0});
      REQUIRE(ip != ip2);
      ip.throw_if_bad_alphabet_rules_or_inverses();
    }

    template <typename W>
    void check_inverses() {
      InversePresentation<W> ip;
      ip.alphabet({0, 1, 2});
      presentation::add_rule_no_checks(ip, {0, 0, 0}, {0});
      presentation::add_rule(ip, {0, 0, 0}, {0});
      ip.inverses_no_checks({0, 0, 0});
      REQUIRE_THROWS_AS(ip.throw_if_bad_alphabet_rules_or_inverses(),
                        LibsemigroupsException);
      REQUIRE_THROWS_AS(ip.inverses({1, 2, 0}), LibsemigroupsException);
      REQUIRE_THROWS_AS(ip.inverses({0, 0, 0}), LibsemigroupsException);
      REQUIRE_THROWS_AS(ip.inverses({0, 1, 2, 0}), LibsemigroupsException);
      REQUIRE_THROWS_AS(ip.inverses({0, 1, 3}), LibsemigroupsException);
      ip.inverses({2, 1, 0});
      REQUIRE(ip.inverses() == W({2, 1, 0}));
      REQUIRE(ip.inverse(0) == 2);
      REQUIRE(ip.inverse(1) == 1);
      REQUIRE(ip.inverse(2) == 0);
      ip.throw_if_bad_alphabet_rules_or_inverses();
    }

  }  // namespace

  using detail::StaticVector1;

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "000",
                          "vectors of ints",
                          "[quick][presentation]") {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.alphabet(012_w);
    REQUIRE(p.alphabet() == 012_w);
    REQUIRE_THROWS_AS(p.alphabet(00_w), LibsemigroupsException);
    REQUIRE(p.alphabet() == 012_w);
    presentation::add_rule_no_checks(p, 000_w, 0_w);
    REQUIRE(std::distance(p.rules.cbegin(), p.rules.cend()) == 2);
    REQUIRE(std::vector(p.rules.cbegin(), p.rules.cend())
            == std::vector({000_w, 0_w}));
    presentation::add_rule(p, 000_w, 0_w);
    REQUIRE_THROWS_AS(presentation::add_rule(p, 050_w, 0_w),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(presentation::add_rule(p, {}, 0_w),
                      LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "001",
                          "strings",
                          "[quick][presentation]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("abc");
    REQUIRE(p.alphabet() == "abc");
    REQUIRE_THROWS_AS(p.alphabet("aa"), LibsemigroupsException);
    REQUIRE(p.alphabet() == "abc");
    presentation::add_rule_no_checks(p, "aaa", "a");
    REQUIRE(std::distance(p.rules.cbegin(), p.rules.cend()) == 2);
    REQUIRE(std::vector<std::string>(p.rules.cbegin(), p.rules.cend())
            == std::vector<std::string>({"aaa", "a"}));
    REQUIRE_THROWS_AS(presentation::add_rule(p, "abz", "a"),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(presentation::add_rule(p, "", "a"),
                      LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "002",
                          "constructors (word_type)",
                          "[quick][presentation]") {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.alphabet({0, 1, 2});
    presentation::add_rule_no_checks(p, {0, 0, 0}, {0});
    REQUIRE(p.rules.size() == 2);
    presentation::add_rule(p, {0, 0, 0}, {0});
    p.throw_if_bad_alphabet_or_rules();
    check_constructors(p);
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "003",
                          "constructors (StaticVector1)",
                          "[quick][presentation]") {
    auto                                      rg = ReportGuard(false);
    Presentation<StaticVector1<uint16_t, 16>> p;
    p.alphabet({0, 1, 2});
    presentation::add_rule_no_checks(p, {0, 0, 0}, {0});
    REQUIRE(p.rules.size() == 2);
    presentation::add_rule(p, {0, 0, 0}, {0});
    p.throw_if_bad_alphabet_or_rules();
    check_constructors(p);
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "004",
                          "constructors (std::string)",
                          "[quick][presentation]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("abc");
    presentation::add_rule_no_checks(p, "aaaa", "aa");
    REQUIRE(p.rules.size() == 2);
    presentation::add_rule(p, "aaa", "aa");
    p.throw_if_bad_alphabet_or_rules();
    check_constructors(p);
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "005",
                          "alphabet + letters (word_type)",
                          "[quick][presentation]") {
    auto rg = ReportGuard(false);
    check_alphabet_letters<word_type>();
    check_alphabet_letters<StaticVector1<uint16_t, 16>>();
    check_alphabet_letters<std::string>();
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "006",
                          "alphabet + letters (std::string)",
                          "[quick][presentation]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("abc");
    REQUIRE(p.alphabet() == "abc");
    REQUIRE(p.letter_no_checks(0) == 'a');
    REQUIRE(p.letter_no_checks(1) == 'b');
    REQUIRE(p.letter_no_checks(2) == 'c');
    p.alphabet(4);
    REQUIRE(p.alphabet().size() == 4);
    p.throw_if_bad_alphabet_or_rules();
    REQUIRE_THROWS_AS(p.alphabet("abb"), LibsemigroupsException);

    presentation::add_rule_no_checks(p, "abca", "aa");
    presentation::add_rule_no_checks(p, "eb", "af");
    presentation::add_rule_no_checks(p, "eb", "abbbbbb");
    p.alphabet_from_rules();
    REQUIRE(p.alphabet() == "abcef");
    REQUIRE(p.index('a') == 0);
    REQUIRE(p.index('b') == 1);
    REQUIRE(p.index('c') == 2);
    REQUIRE(p.index('e') == 3);
    REQUIRE(p.index('f') == 4);
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "007",
                          "contains_empty_word",
                          "[quick][presentation]") {
    auto rg = ReportGuard(false);
    check_contains_empty_word<word_type>();
    check_contains_empty_word<StaticVector1<uint16_t, 16>>();
    check_contains_empty_word<std::string>();
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "008",
                          "contains_rule",
                          "[quick][presentation]") {
    auto rg = ReportGuard(false);
    check_contains_rule<word_type>();
    check_contains_rule<StaticVector1<uint16_t, 16>>();
    check_contains_rule<std::string>();
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "009",
                          "throw_if_bad_rules throws",
                          "[quick][presentation]") {
    auto rg = ReportGuard(false);
    check_validate_rules_throws<word_type>();
    check_validate_rules_throws<StaticVector1<uint16_t, 16>>();
    check_validate_rules_throws<std::string>();
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "010",
                          "helpers add_rule(s)",
                          "[quick][presentation]") {
    auto rg = ReportGuard(false);
    check_add_rules<word_type>();
    check_add_rules<StaticVector1<uint16_t, 10>>();
    check_add_rules<std::string>();
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "011",
                          "helpers add_rule(s) (std::string)",
                          "[quick][presentation]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    presentation::add_rule_no_checks(p, "abcb", "aa");
    Presentation<std::string> q;
    presentation::add_rule_no_checks(q, "eb", "af");
    presentation::add_rule_no_checks(q, "eb", "abbbbbbbbb");
    presentation::add_rules_no_checks(p, q);
    presentation::add_rule_no_checks(p, std::string("ab"), "ba");
    presentation::add_rule_no_checks(p, "ac", std::string("ab"));
    REQUIRE(p.rules
            == std::vector<std::string>({"abcb",
                                         "aa",
                                         "eb",
                                         "af",
                                         "eb",
                                         "abbbbbbbbb",
                                         "ab",
                                         "ba",
                                         "ac",
                                         "ab"}));
    REQUIRE(q.rules
            == std::vector<std::string>({"eb", "af", "eb", "abbbbbbbbb"}));
    REQUIRE_THROWS_AS(p.throw_if_bad_alphabet_or_rules(),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(q.throw_if_bad_alphabet_or_rules(),
                      LibsemigroupsException);
    p.alphabet_from_rules();
    p.throw_if_bad_alphabet_or_rules();
    presentation::add_rule(p, std::string("bbb"), "baa");
    presentation::add_rule(p, "b", std::string("bb"));
    REQUIRE(p.rules
            == std::vector<std::string>({"abcb",
                                         "aa",
                                         "eb",
                                         "af",
                                         "eb",
                                         "abbbbbbbbb",
                                         "ab",
                                         "ba",
                                         "ac",
                                         "ab",
                                         "bbb",
                                         "baa",
                                         "b",
                                         "bb"}));
  }

  LIBSEMIGROUPS_TEST_CASE(
      "Presentation",
      "012",
      "helpers add_identity_rules (std::vector/StaticVector1)",
      "[quick][presentation]") {
    auto rg = ReportGuard(false);
    check_add_identity_rules<word_type>();
    check_add_identity_rules<StaticVector1<uint16_t, 10>>();
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "013",
                          "helpers add_zero_rules (std::vector/StaticVector1)",
                          "[quick][presentation]") {
    auto rg = ReportGuard(false);
    check_add_zero_rules<word_type>();
    check_add_zero_rules<StaticVector1<uint16_t, 10>>();
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "014",
                          "helpers add_identity_rules (std::string)",
                          "[quick][presentation]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    presentation::add_rule_no_checks(p, "abcb", "aa");
    REQUIRE_THROWS_AS(presentation::add_identity_rules(p, 'a'),
                      LibsemigroupsException);
    p.alphabet_from_rules();
    presentation::add_identity_rules(p, 'a');
    REQUIRE(p.rules
            == std::vector<std::string>({"abcb",
                                         "aa",
                                         "aa",
                                         "a",
                                         "ba",
                                         "b",
                                         "ab",
                                         "b",
                                         "ca",
                                         "c",
                                         "ac",
                                         "c"}));
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "015",
                          "helpers add_zero_rules (std::string)",
                          "[quick][presentation]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    presentation::add_rule_no_checks(p, "abcb", "aa");
    REQUIRE_THROWS_AS(presentation::add_zero_rules(p, '0'),
                      LibsemigroupsException);
    p.alphabet("abc0");
    presentation::add_zero_rules(p, '0');
    REQUIRE(p.rules
            == std::vector<std::string>({"abcb",
                                         "aa",
                                         "a0",
                                         "0",
                                         "0a",
                                         "0",
                                         "b0",
                                         "0",
                                         "0b",
                                         "0",
                                         "c0",
                                         "0",
                                         "0c",
                                         "0",
                                         "00",
                                         "0"}));
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "016",
                          "helpers add_commutes_rules (all)",
                          "[quick][presentation]") {
    auto rg = ReportGuard(false);
    check_add_commutes_rules<word_type>();
    // TODO(1) Can't do this until operator+ works with StaticVector
    // check_add_commutes_rules<StaticVector1<uint16_t, 10>>();
    check_add_commutes_rules<std::string>();
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "017",
                          "helpers add_idempotent_rules (all)",
                          "[quick][presentation]") {
    auto rg = ReportGuard(false);
    check_add_idempotent_rules<word_type>();
    check_add_idempotent_rules<StaticVector1<uint16_t, 10>>();
    check_add_idempotent_rules<std::string>();
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "018",
                          "helpers add_involution_rules (all)",
                          "[quick][presentation]") {
    auto rg = ReportGuard(false);
    check_add_involution_rules<word_type>();
    check_add_involution_rules<StaticVector1<uint16_t, 10>>();
    check_add_involution_rules<std::string>();
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "019",
                          "helpers add_inverse_rules (all)",
                          "[quick][presentation]") {
    auto rg = ReportGuard(false);
    check_add_inverse_rules<word_type>();
    check_add_inverse_rules<StaticVector1<uint16_t, 10>>();
    check_add_inverse_rules<std::string>();
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "020",
                          "helpers add_inverse_rules (std::string)",
                          "[quick][presentation]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.contains_empty_word(false);

    p.alphabet("aAbBcCe");
    presentation::add_identity_rules(p, 'e');

    presentation::add_inverse_rules(p, "AaBbCce", 'e');
    presentation::add_rule(p, "aaCac", "e");
    presentation::add_rule(p, "acbbACb", "e");
    presentation::add_rule(p, "ABabccc", "e");

    REQUIRE(
        p.rules
        == std::vector<std::string>(
            {"ae", "a",  "ea",    "a",  "Ae",      "A",  "eA",      "A",  "be",
             "b",  "eb", "b",     "Be", "B",       "eB", "B",       "ce", "c",
             "ec", "c",  "Ce",    "C",  "eC",      "C",  "ee",      "e",  "aA",
             "e",  "Aa", "e",     "bB", "e",       "Bb", "e",       "cC", "e",
             "Cc", "e",  "aaCac", "e",  "acbbACb", "e",  "ABabccc", "e"}));
    REQUIRE(!presentation::are_rules_sorted(p));
    REQUIRE(!presentation::are_rules_sorted(p, LexicographicalCompare()));
    presentation::sort_each_rule(p, LexicographicalCompare());
    presentation::sort_rules(p, LexicographicalCompare());
    REQUIRE(presentation::are_rules_sorted(p, LexicographicalCompare()));
    REQUIRE(p.rules
            == std::vector<std::string>(
                {"Ae", "A",  "Be", "B",  "Ce",    "C", "ae",      "a",  "be",
                 "b",  "ce", "c",  "eA", "A",     "e", "ABabccc", "e",  "Aa",
                 "eB", "B",  "e",  "Bb", "eC",    "C", "e",       "Cc", "e",
                 "aA", "ea", "a",  "e",  "aaCac", "e", "acbbACb", "e",  "bB",
                 "eb", "b",  "e",  "cC", "ec",    "c", "ee",      "e"}));
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "021",
                          "helpers remove_duplicate_rules",
                          "[quick][presentation]") {
    auto rg = ReportGuard(false);
    check_remove_duplicate_rules<word_type>();
    check_remove_duplicate_rules<StaticVector1<uint16_t, 10>>();
    check_remove_duplicate_rules<std::string>();
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "022",
                          "helpers reduce_complements",
                          "[quick][presentation]") {
    using words::human_readable_letter;
    auto rg = ReportGuard(false);
    check_reduce_complements<word_type>();
    check_reduce_complements<StaticVector1<uint16_t, 10>>();
    Presentation<std::string> p;
    presentation::add_rule_no_checks(p, "abcb", "bcb");
    presentation::add_rule_no_checks(p, "bcb", "bbcb");
    presentation::add_rule_no_checks(p, "bbcb", "bb");
    presentation::add_rule_no_checks(p, "bb", "bcb");
    presentation::add_rule_no_checks(p, "bcb", "a");
    p.alphabet_from_rules();
    presentation::reduce_complements(p);
    presentation::sort_each_rule(p);
    presentation::sort_rules(p);
    REQUIRE(p.rules
            == std::vector<std::string>(
                {"bb", "a", "bcb", "a", "abcb", "a", "bbcb", "a"}));
    REQUIRE(p.alphabet() == "abc");
    presentation::normalize_alphabet(p);
    REQUIRE(p.letter_no_checks(0) == human_readable_letter(0));
    REQUIRE(p.letter_no_checks(1) == human_readable_letter(1));
    REQUIRE(p.letter_no_checks(2) == human_readable_letter(2));
    p.throw_if_bad_alphabet_or_rules();

    presentation::add_rule_no_checks(p, "abcb", "ecb");
    REQUIRE(!p.in_alphabet('e'));
    // Not valid
    REQUIRE_THROWS_AS(presentation::normalize_alphabet(p),
                      LibsemigroupsException);
    p.alphabet_from_rules();
    presentation::add_rule_no_checks(p, "abcd", "bcb");
    REQUIRE_THROWS_AS(presentation::normalize_alphabet(p),
                      LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "023",
                          "helpers balance_no_checks (all)",
                          "[quick][presentation]") {
    auto rg = ReportGuard(false);
    check_balance<word_type>();
    // TODO(1) Can't do this until StaticVector1 has .front or . end
    // check_balance<StaticVector1<uint16_t, 10>>();
    check_balance<std::string>();
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "024",
                          "helpers balance_no_checks (std::string)",
                          "[quick][presentation]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.contains_empty_word(true);
    presentation::add_rule_no_checks(p, "aaaaaaaa", "");
    presentation::balance_no_checks(p, "a", "a");
    presentation::add_rule_no_checks(p, "aaa", "aaaaaa");
    presentation::balance_no_checks(p, "a", "a");
    presentation::add_rule_no_checks(p, "aa", "");
    presentation::balance_no_checks(p, "a", "a");
    presentation::add_rule_no_checks(p, "abba", "baaaaaaaab");
    presentation::balance_no_checks(p, "b", "a");
    presentation::balance_no_checks(p, "a", "c");
    presentation::add_rule_no_checks(p, "baaaaaabbb", "");
    presentation::balance_no_checks(p, "ab", "cd");
    presentation::add_rule_no_checks(p, "abcabd", "");
    presentation::balance_no_checks(p, "abc", "efg");
    REQUIRE(p.rules
            == std::vector<std::string>(  // codespell:begin-ignore
                {"aaaa",
                 "aaaa",
                 "aaaaa",
                 "aaaa",
                 "c",
                 "a",
                 "aabbaac",
                 "aaaaaaa",
                 "dddcc",
                 "baaaa",
                 "abd",
                 "gfe"}));  // codespell:end-ignore
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "025",
                          "helpers sort_each_rule",
                          "[quick][presentation]") {
    auto rg = ReportGuard(false);
    check_sort_each_rule<word_type>();
    check_sort_each_rule<StaticVector1<uint16_t, 10>>();
    check_sort_each_rule<std::string>();
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "026",
                          "helpers sort_rules",
                          "[quick][presentation]") {
    auto rg = ReportGuard(false);
    check_sort_rules<word_type>();
    check_sort_rules<StaticVector1<uint16_t, 10>>();
    check_sort_rules<std::string>();
  }

  LIBSEMIGROUPS_TEST_CASE(
      "Presentation",
      "027",
      "helpers longest_subword_reducing_length/replace_subword",
      "[quick][presentation]") {
    auto rg = ReportGuard(false);
    check_longest_subword_reducing_length<word_type>();
    check_longest_subword_reducing_length<StaticVector1<uint16_t, 10>>();
    check_longest_subword_reducing_length<std::string>();
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "028",
                          "helpers redundant_rule",
                          "[quick][presentation][no-valgrind]") {
    auto rg = ReportGuard(false);
    check_redundant_rule<word_type>();
    check_redundant_rule<StaticVector1<uint16_t, 10>>();
    check_redundant_rule<std::string>();
  }

  // LIBSEMIGROUPS_TEST_CASE("Presentation",
  //                         "029",
  //                         "helpers shortlex_compare_concat",
  //                         "[quick][presentation]") {
  //   auto rg = ReportGuard(false);
  //   check_shortlex_compare_concat<word_type>();
  //   check_shortlex_compare_concat<StaticVector1<uint16_t, 10>>();
  //   check_shortlex_compare_concat<std::string>();
  // }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "029",
                          "helpers remove_trivial_rules",
                          "[quick][presentation]") {
    auto rg = ReportGuard(false);
    check_remove_trivial_rules<word_type>();
    check_remove_trivial_rules<StaticVector1<uint16_t, 10>>();
    check_remove_trivial_rules<std::string>();
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "030",
                          "helpers replace_subword (existing, replacement)",
                          "[quick][presentation]") {
    auto rg = ReportGuard(false);
    check_replace_subword<word_type>();
    check_replace_subword<StaticVector1<uint16_t, 64>>();
    check_replace_subword<std::string>();
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "031",
                          "helpers replace_subword (std::string)",
                          "[quick][presentation]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    presentation::add_rule_no_checks(p, "abababab", "bbaabb");
    presentation::replace_subword(p, "ab", "c");
    REQUIRE(p.rules == std::vector<std::string>({"cccc", "bbacb"}));
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "032",
                          "helpers replace_word",
                          "[quick][presentation]") {
    auto rg = ReportGuard(false);
    check_replace_word<word_type>();
    check_replace_word<StaticVector1<uint16_t, 10>>();
    check_replace_word<std::string>();
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "033",
                          "helpers longest_rule",
                          "[quick][presentation]") {
    auto rg = ReportGuard(false);
    check_longest_rule<word_type>();
    check_longest_rule<StaticVector1<uint16_t, 10>>();
    check_longest_rule<std::string>();
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "034",
                          "helpers longest_rule_length",
                          "[quick][presentation]") {
    auto rg = ReportGuard(false);
    check_longest_rule_length<word_type>();
    check_longest_rule_length<StaticVector1<uint16_t, 10>>();
    check_longest_rule_length<std::string>();
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "035",
                          "helpers remove_redundant_generators",
                          "[quick][presentation]") {
    auto rg = ReportGuard(false);
    check_remove_redundant_generators<word_type>();
    check_remove_redundant_generators<StaticVector1<uint16_t, 64>>();
    check_remove_redundant_generators<std::string>();
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "036",
                          "helpers remove_redundant_generators",
                          "[quick][presentation]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("abcdefghi");
    presentation::add_rule(p, "d", "ffg");
    presentation::add_rule(p, "bcbc", "cc");
    presentation::add_rule(p, "bbb", "d");
    presentation::add_rule(p, "biib", "e");
    presentation::add_rule(p, "iii", "h");
    presentation::add_rule(p, "h", "gg");
    presentation::add_rule(p, "d", "iii");

    presentation::remove_redundant_generators(p);

    REQUIRE(p.alphabet() == "abcfgi");
    REQUIRE(p.rules
            == std::vector<std::string>(
                {"bcbc", "cc", "bbb", "ffg", "iii", "gg", "ffg", "iii"}));
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "037",
                          "helpers reverse",
                          "[quick][presentation]") {
    auto rg = ReportGuard(false);
    check_reverse<word_type>();
    check_reverse<StaticVector1<uint16_t, 10>>();
    check_reverse<std::string>();
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "038",
                          "in_alphabet",
                          "[quick][presentation]") {
    auto rg = ReportGuard(false);
    check_in_alphabet<word_type>();
    check_in_alphabet<StaticVector1<uint16_t, 10>>();
    check_in_alphabet<std::string>();
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "039",
                          "replace_subword with empty word",
                          "[quick][presentation]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet(2);
    p.contains_empty_word(true);
    presentation::add_rule_no_checks(p, {'a', 'a', 'a'}, {});
    p.throw_if_bad_alphabet_or_rules();
    REQUIRE_THROWS_AS(presentation::replace_subword(p, {}, {'c'}),
                      LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "040",
                          "clear",
                          "[quick][presentation]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet(2);
    p.contains_empty_word(true);
    presentation::add_rule_no_checks(p, {'a', 'a', 'a'}, {});
    p.throw_if_bad_alphabet_or_rules();
    p.init();
    REQUIRE(p.alphabet().empty());
    REQUIRE(p.rules.empty());
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "041",
                          "change_alphabet",
                          "[quick][presentation]") {
    Presentation<std::string> p;
    p.alphabet("ab");
    presentation::add_rule(p, "ba", "abaaabaa");
    presentation::replace_word_with_new_generator(p, "ba");
    presentation::change_alphabet(p, "abc");
    REQUIRE(p.rules == std::vector<std::string>({"c", "acaaca", "c", "ba"}));
    REQUIRE(p.alphabet() == "abc");
    REQUIRE_NOTHROW(p.throw_if_bad_alphabet_or_rules());
    // Alphabet wrong size
    REQUIRE_THROWS_AS(presentation::change_alphabet(p, "ab"),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(presentation::change_alphabet(p, "aab"),
                      LibsemigroupsException);
    REQUIRE(p.alphabet() == "abc");
    REQUIRE(p.rules == std::vector<std::string>({"c", "acaaca", "c", "ba"}));
    presentation::change_alphabet(p, "bac");
    REQUIRE(p.rules == std::vector<std::string>({"c", "bcbbcb", "c", "ab"}));
    REQUIRE(p.alphabet() == "bac");

    presentation::change_alphabet(p, "xyz");
    REQUIRE(p.rules == std::vector<std::string>({"z", "xzxxzx", "z", "yx"}));
    REQUIRE(p.alphabet() == "xyz");

    presentation::change_alphabet(p, "xyt");
    REQUIRE(p.rules == std::vector<std::string>({"t", "xtxxtx", "t", "yx"}));
    REQUIRE(p.alphabet() == "xyt");
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "042",
                          "letter",
                          "[quick][presentation]") {
    using words::human_readable_letter;
    Presentation<std::vector<uint16_t>> p;
    REQUIRE_THROWS_AS(human_readable_letter<std::vector<uint16_t>>(65536),
                      LibsemigroupsException);
    REQUIRE(human_readable_letter<std::vector<uint16_t>>(10) == 10);
    REQUIRE(human_readable_letter(0) == 'a');
    REQUIRE(human_readable_letter(10) == 'k');

    Presentation<std::string> q;

    Presentation<word_type> r;
    for (size_t i = 0; i < 1000; ++i) {
      REQUIRE(human_readable_letter<word_type>(i) == i);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "043",
                          "normalize_alphabet",
                          "[quick][presentation]") {
    Presentation<std::string> p;
    p.alphabet("axy");
    presentation::normalize_alphabet(p);
    REQUIRE(p.alphabet() == "abc");
    Presentation<word_type> q;
    q.alphabet({0, 10, 12});
    presentation::normalize_alphabet(q);
    REQUIRE(q.alphabet() == word_type({0, 1, 2}));
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "044",
                          "first_unused_letter/letter",
                          "[quick][presentation]") {
    using words::human_readable_letter;
    Presentation<std::string> p;
    p.alphabet("ab");

    presentation::add_rule(p, "baabaa", "ababa");
    REQUIRE(presentation::first_unused_letter(p) == 'c');
    p.alphabet("abcdefghijklmnopq");
    REQUIRE(presentation::first_unused_letter(p) == 'r');
    p.alphabet("abcdefghijklmnopqrstuvwxyz");
    REQUIRE(presentation::first_unused_letter(p) == 'A');
    p.alphabet("abcdefgijklmnopqrstuvwxyz");
    REQUIRE(presentation::first_unused_letter(p) == 'h');
    p.alphabet("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
    REQUIRE(presentation::first_unused_letter(p) == '0');
    p.alphabet("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ02");
    REQUIRE(presentation::first_unused_letter(p) == '1');
    std::string const letters
        = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    std::unordered_set<letter_type> set;
    for (size_t i = 0; i < letters.size(); ++i) {
      REQUIRE(letters[i] == human_readable_letter(i));
      REQUIRE(set.insert(letters[i]).second);
    }
    for (size_t i = letters.size(); i < 256; ++i) {
      REQUIRE(set.insert(human_readable_letter(i)).second);
    }
    REQUIRE_THROWS_AS(human_readable_letter(256), LibsemigroupsException);
    p.alphabet(256);
    REQUIRE_THROWS_AS(presentation::first_unused_letter(p),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(p.alphabet(257), LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "045",
                          "longest_subword_reducing_length issue",
                          "[quick][presentation]") {
    Presentation<std::string> p;
    p.alphabet("a");
    presentation::add_rule(p, "aaaaaaaaaaaaaaaaaaa", "a");
    REQUIRE(presentation::longest_subword_reducing_length(p) == "aaaaaa");
    presentation::replace_word_with_new_generator(p, "aaaaaa");
    REQUIRE(presentation::longest_subword_reducing_length(p) == "");
    REQUIRE(p.rules == std::vector<std::string>({"bbba", "a", "b", "aaaaaa"}));
    REQUIRE(presentation::length(p) == 12);
    p.rules = std::vector<std::string>({"bba", "a", "b", "aaaaaaaa"});
    REQUIRE(presentation::length(p) == 13);

    p.alphabet("ab");
    presentation::add_rule(p, "baaaaaaaaaaaaaaaaaaa", "a");
    REQUIRE(presentation::longest_subword_reducing_length(p) == "aaaaaa");

    p.alphabet("ab");
    p.rules.clear();
    presentation::add_rule(p, "aaaaaaaaaaaaaaaa", "a");
    presentation::add_rule(p, "bbbbbbbbbbbbbbbb", "b");
    presentation::add_rule(p, "abb", "baa");
    REQUIRE(presentation::length(p) == 40);
    auto w = presentation::longest_subword_reducing_length(p);
    REQUIRE(w == "bbbb");
    presentation::replace_word_with_new_generator(p, w);
    REQUIRE(presentation::length(p) == 33);
    REQUIRE(
        p.rules
        == std::vector<std::string>(
            {"aaaaaaaaaaaaaaaa", "a", "cccc", "b", "abb", "baa", "c", "bbbb"}));
    w = presentation::longest_subword_reducing_length(p);
    REQUIRE(w == "aaaa");
    presentation::replace_word_with_new_generator(p, w);
    REQUIRE(presentation::length(p) == 26);
    REQUIRE(p.rules
            == std::vector<std::string>({"dddd",
                                         "a",
                                         "cccc",
                                         "b",
                                         "abb",
                                         "baa",
                                         "c",
                                         "bbbb",
                                         "d",
                                         "aaaa"}));
    w = presentation::longest_subword_reducing_length(p);
    REQUIRE(w == "");
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "046",
                          "make_semigroup",
                          "[quick][presentation]") {
    check_make_semigroup<word_type>();
    check_make_semigroup<StaticVector1<uint16_t, 10>>();
    check_make_semigroup<std::string>();
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "047",
                          "greedy_reduce_length",
                          "[quick][presentation]") {
    Presentation<std::string> p;
    p.alphabet("ab");
    p.rules.clear();
    presentation::add_rule(p, "aaaaaaaaaaaaaaaa", "a");
    presentation::add_rule(p, "bbbbbbbbbbbbbbbb", "b");
    presentation::add_rule(p, "abb", "baa");
    REQUIRE(presentation::length(p) == 40);
    presentation::greedy_reduce_length(p);
    REQUIRE(presentation::length(p) == 26);
    REQUIRE(p.rules
            == std::vector<std::string>({"dddd",
                                         "a",
                                         "cccc",
                                         "b",
                                         "abb",
                                         "baa",
                                         "c",
                                         "bbbb",
                                         "d",
                                         "aaaa"}));
    REQUIRE(presentation::longest_subword_reducing_length(p) == "");
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "048",
                          "greedy_reduce_length_and_number_of_gens",
                          "[quick][presentation]") {
    Presentation<std::string> p1;
    p1.alphabet("ab");
    p1.rules.clear();
    presentation::add_rule(p1, "aaaaaa", "a");
    presentation::add_rule(p1, "bbbbbb", "b");
    presentation::add_rule(p1, "abb", "baa");
    Presentation<std::string> q1(p1);

    REQUIRE(presentation::length(p1) == 20);
    REQUIRE(p1.alphabet().size() == 2);

    presentation::greedy_reduce_length_and_number_of_gens(p1);
    REQUIRE(presentation::length(p1) == 20);
    REQUIRE(p1.alphabet().size() == 2);
    REQUIRE(presentation::longest_subword_reducing_length(p1) != "");

    presentation::greedy_reduce_length(q1);
    REQUIRE(presentation::length(q1) == 18);
    REQUIRE(q1.alphabet().size() == 4);
    REQUIRE(presentation::longest_subword_reducing_length(q1) == "");
    REQUIRE(p1 != q1);

    Presentation<std::string> p2;
    p2.alphabet("ab");
    presentation::add_rule(p2, "aaaaaaaaaaaaaaaa", "a");
    presentation::add_rule(p2, "bbbbbbbbbbbbbbbb", "b");
    presentation::add_rule(p2, "abb", "baa");
    Presentation<std::string> q2(p2);
    presentation::greedy_reduce_length(p2);
    presentation::greedy_reduce_length_and_number_of_gens(q2);
    REQUIRE(p2 == q2);
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "049",
                          "aaaaaaaab = aaaaaaaaab strong compression",
                          "[quick][presentation]") {
    Presentation<std::string> p;
    p.alphabet("ab");
    presentation::add_rule(p, "aaaaaaaab", "aaaaaaaaab");
    REQUIRE(presentation::strongly_compress(p));
    REQUIRE(p.rules == decltype(p.rules)({"a", "aa"}));

    p.rules = {"adadnadnasnamdnamdna", "akdjskadjksajdaldja"};
    p.alphabet_from_rules();

    REQUIRE(presentation::strongly_compress(p));
    REQUIRE(presentation::reduce_to_2_generators(p));
    REQUIRE(
        p.rules
        == decltype(p.rules)({"aaaaaaaaaaaaaaaaaaa", "baaaaaaaaaaaaaaaaa"}));

    // Only works for 1-relation monoids at present
    p.alphabet("ab");
    presentation::add_rule(p, "aaaaaaaab", "aaaaaaaaab");
    presentation::add_rule(p, "aaaaaaaab", "aaaaaaaaab");
    REQUIRE(!presentation::strongly_compress(p));
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "050",
                          "case where strong compression doesn't work",
                          "[quick][presentation]") {
    Presentation<std::string> p;
    p.alphabet("ab");
    presentation::add_rule(p, "abaaaabab", "abbabaaaab");
    REQUIRE(presentation::strongly_compress(p));
    REQUIRE(p.rules == decltype(p.rules)({"abccdae", "fgeabccd"}));

    auto q = p;
    REQUIRE(presentation::reduce_to_2_generators(q));
    REQUIRE(q.rules == decltype(q.rules)({"aaaaaaa", "baaaaaaa"}));

    q = p;
    REQUIRE(presentation::reduce_to_2_generators(q, 1));
    REQUIRE(q.rules == decltype(q.rules)({"abbbbab", "bbbabbbb"}));
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "051",
                          "proof that",
                          "[quick][presentation]") {
    Presentation<std::string> p;
    p.alphabet("ab");
    presentation::add_rule(p, "aabb", "aaabaaab");
    REQUIRE(presentation::strongly_compress(p));
    presentation::reverse(p);
    REQUIRE(p.rules == decltype(p.rules)({"cba", "baadbaa"}));

    auto q = p;
    REQUIRE(presentation::reduce_to_2_generators(q));
    REQUIRE(q.rules == decltype(q.rules)({"aba", "baaabaa"}));

    q = p;
    REQUIRE(presentation::reduce_to_2_generators(q, 1));
    REQUIRE(q.rules == decltype(q.rules)({"abb", "bbbbbbb"}));

    // Wrong index
    REQUIRE_THROWS_AS(presentation::reduce_to_2_generators(q, 2),
                      LibsemigroupsException);
    q = p;
    presentation::add_rule(q, "aabb", "aaabaaab");
    // not 1-relation
    REQUIRE(!presentation::reduce_to_2_generators(q, 1));

    q.rules = {"aaaaa", "a"};
    REQUIRE(!presentation::reduce_to_2_generators(q));

    q.rules = {"aaaaa", ""};
    REQUIRE(!presentation::reduce_to_2_generators(q));

    q.rules = {"abcacbabab", ""};
    REQUIRE(!presentation::reduce_to_2_generators(q));
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "052",
                          "decompression",
                          "[quick][presentation]") {
    Presentation<std::string> p;
    p.alphabet("ab");
    p.rules = {"aabb", "aaabaab"};
    REQUIRE(presentation::strongly_compress(p));
    REQUIRE(p.rules == decltype(p.rules)({"abc", "aabdab"}));
    REQUIRE(!presentation::reduce_to_2_generators(p));
    presentation::reverse(p);
    REQUIRE(presentation::reduce_to_2_generators(p));
    REQUIRE(p.rules == decltype(p.rules)({"aba", "baabaa"}));
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "053",
                          "sort_rules bug",
                          "[quick][presentation]") {
    std::string prefix1 = "dabd", suffix1 = "cbb", prefix2 = "abbaba",
                suffix2 = "c";

    REQUIRE(
        !shortlex_compare(chain(prefix1, suffix1), chain(prefix2, suffix2)));

    Presentation<std::string> p;
    p.alphabet("bacd");
    p.rules = {"baabbabaa",
               "abaaba",
               "abbaba",
               "c",
               "abb",
               "d",
               "daba",
               "c",
               "dabd",
               "cbb",
               "dbaca",
               "caba",
               "dbacc",
               "cabc",
               "dbacd",
               "cabd",
               "abaaba",
               "baca",
               "abaabc",
               "bacc",
               "abaabd",
               "bacd",
               "cbaaba",
               "ddaca",
               "cbaabc",
               "ddacc",
               "cbaabd",
               "ddacd",
               "cbbaba",
               "dabc",
               "dabcbb",
               "cbbabd",
               "bacaaba",
               "ababaca",
               "bacaabc",
               "ababacc",
               "bacaabd",
               "ababacd",
               "bacbaca",
               "abadaca",
               "bacbacc",
               "abadacc",
               "bacbacd",
               "abadacd",
               "dabcaba",
               "cbbbaca",
               "dabcabc",
               "cbbbacc",
               "dabcabd",
               "cbbbacd",
               "ddacaaba",
               "cbabaca",
               "ddacaabc",
               "cbabacc",
               "ddacaabd",
               "cbabacd",
               "ddacbaca",
               "cbadaca",
               "ddacbacc",
               "cbadacc",
               "ddacbacd",
               "cbadacd",
               "abababaca",
               "dacaaba",
               "abababacc",
               "dacaabc",
               "abababacd",
               "dacaabd",
               "ababadaca",
               "dacbaca",
               "ababadacc",
               "dacbacc",
               "ababadacd",
               "dacbacd",
               "daababaca",
               "ccaaba",
               "daababacc",
               "ccaabc",
               "daababacd",
               "ccaabd",
               "daabadaca",
               "ccbaca",
               "daabadacc",
               "ccbacc",
               "daabadacd",
               "ccbacd",
               "bacababaca",
               "abadacaaba",
               "bacababacc",
               "abadacaabc",
               "bacababacd",
               "abadacaabd",
               "bacabadaca",
               "abadacbaca",
               "bacabadacc",
               "abadacbacc",
               "bacabadacd",
               "abadacbacd",
               "dabcbabaca",
               "cbbdacaaba",
               "dabcbabacc",
               "cbbdacaabc",
               "dabcbabacd",
               "cbbdacaabd",
               "dabcbadaca",
               "cbbdacbaca",
               "dabcbadacc",
               "cbbdacbacc",
               "dabcbadacd",
               "cbbdacbacd",
               "abaaababaca",
               "bacacaaba",
               "abaaababacc",
               "bacacaabc",
               "abaaababacd",
               "bacacaabd",
               "abaaabadaca",
               "bacacbaca",
               "abaaabadacc",
               "bacacbacc",
               "abaaabadacd",
               "bacacbacd",
               "cbaaababaca",
               "ddacacaaba",
               "cbaaababacc",
               "ddacacaabc",
               "cbaaababacd",
               "ddacacaabd",
               "cbaaabadaca",
               "ddacacbaca",
               "cbaaabadacc",
               "ddacacbacc",
               "cbaaabadacd",
               "ddacacbacd",
               "cbbaababaca",
               "dabccaaba",
               "cbbaababacc",
               "dabccaabc",
               "cbbaababacd",
               "dabccaabd",
               "cbbaabadaca",
               "dabccbaca",
               "cbbaabadacc",
               "dabccbacc",
               "cbbaabadacd",
               "dabccbacd",
               "ddacababaca",
               "cbadacaaba",
               "ddacababacc",
               "cbadacaabc",
               "ddacababacd",
               "cbadacaabd",
               "ddacabadaca",
               "cbadacbaca",
               "ddacabadacc",
               "cbadacbacc",
               "ddacabadacd",
               "cbadacbacd",
               "ababadacbaca",
               "dacabadaca",
               "ababadacbacc",
               "dacabadacc",
               "ababadacbacd",
               "dacabadacd",
               "bacaaababaca",
               "ababacacaaba",
               "bacaaababacc",
               "ababacacaabc",
               "bacaaababacd",
               "ababacacaabd",
               "bacaaabadaca",
               "ababacacbaca",
               "bacaaabadacc",
               "ababacacbacc",
               "bacaaabadacd",
               "ababacacbacd",
               "daabadacbaca",
               "ccabadaca",
               "daabadacbacc",
               "ccabadacc",
               "daabadacbacd",
               "ccabadacd",
               "bacabadacbaca",
               "abadacabadaca",
               "bacabadacbacc",
               "abadacabadacc",
               "bacabadacbacd",
               "abadacabadacd",
               "dabcbadacaaba",
               "cbbdacababaca",
               "dabcbadacaabc",
               "cbbdacababacc",
               "dabcbadacaabd",
               "cbbdacababacd",
               "dabcbadacbaca",
               "cbbdacabadaca",
               "dabcbadacbacc",
               "cbbdacabadacc",
               "dabcbadacbacd",
               "cbbdacabadacd",
               "ddacaaababaca",
               "cbabacacaaba",
               "ddacaaababacc",
               "cbabacacaabc",
               "ddacaaababacd",
               "cbabacacaabd",
               "ddacaaabadaca",
               "cbabacacbaca",
               "ddacaaabadacc",
               "cbabacacbacc",
               "ddacaaabadacd",
               "cbabacacbacd",
               "abaaabadacbaca",
               "bacacabadaca",
               "abaaabadacbacc",
               "bacacabadacc",
               "abaaabadacbacd",
               "bacacabadacd",
               "cbaaabadacbaca",
               "ddacacabadaca",
               "cbaaabadacbacc",
               "ddacacabadacc",
               "cbaaabadacbacd",
               "ddacacabadacd",
               "cbbaabadacbaca",
               "dabccabadaca",
               "cbbaabadacbacc",
               "dabccabadacc",
               "cbbaabadacbacd",
               "dabccabadacd",
               "ddacabadacbaca",
               "cbadacabadaca",
               "ddacabadacbacc",
               "cbadacabadacc",
               "ddacabadacbacd",
               "cbadacabadacd",
               "bacaaabadacbaca",
               "ababacacabadaca",
               "bacaaabadacbacc",
               "ababacacabadacc",
               "bacaaabadacbacd",
               "ababacacabadacd",
               "dabcbabacacaaba",
               "cbbdacaaababaca",
               "dabcbabacacaabc",
               "cbbdacaaababacc",
               "dabcbabacacaabd",
               "cbbdacaaababacd",
               "dabcbabacacbaca",
               "cbbdacaaabadaca",
               "dabcbabacacbacc",
               "cbbdacaaabadacc",
               "dabcbabacacbacd",
               "cbbdacaaabadacd",
               "dabcbadacabadaca",
               "cbbdacabadacbaca",
               "dabcbadacabadacc",
               "cbbdacabadacbacc"};
    REQUIRE(p.rules.size() == 258);

    p.throw_if_bad_alphabet_or_rules();
    presentation::sort_each_rule(p);
    presentation::sort_rules(p);
    REQUIRE(presentation::are_rules_sorted(p));
    REQUIRE(p.rules
            == std::vector<std::string>({"abb",
                                         "d",
                                         "daba",
                                         "c",
                                         "abbaba",
                                         "c",
                                         "dabd",
                                         "cbb",
                                         "dbaca",
                                         "caba",
                                         "dbacc",
                                         "cabc",
                                         "dbacd",
                                         "cabd",
                                         "abaaba",
                                         "baca",
                                         "abaabc",
                                         "bacc",
                                         "abaabd",
                                         "bacd",
                                         "cbbaba",
                                         "dabc",
                                         "cbaaba",
                                         "ddaca",
                                         "cbaabc",
                                         "ddacc",
                                         "cbaabd",
                                         "ddacd",
                                         "dabcbb",
                                         "cbbabd",
                                         "bacaaba",
                                         "ababaca",
                                         "bacaabc",
                                         "ababacc",
                                         "bacaabd",
                                         "ababacd",
                                         "bacbaca",
                                         "abadaca",
                                         "bacbacc",
                                         "abadacc",
                                         "bacbacd",
                                         "abadacd",
                                         "dabcaba",
                                         "cbbbaca",
                                         "dabcabc",
                                         "cbbbacc",
                                         "dabcabd",
                                         "cbbbacd",
                                         "baabbabaa",
                                         "abaaba",
                                         "daababaca",
                                         "ccaaba",
                                         "daababacc",
                                         "ccaabc",
                                         "daababacd",
                                         "ccaabd",
                                         "daabadaca",
                                         "ccbaca",
                                         "daabadacc",
                                         "ccbacc",
                                         "daabadacd",
                                         "ccbacd",
                                         "ddacaaba",
                                         "cbabaca",
                                         "ddacaabc",
                                         "cbabacc",
                                         "ddacaabd",
                                         "cbabacd",
                                         "ddacbaca",
                                         "cbadaca",
                                         "ddacbacc",
                                         "cbadacc",
                                         "ddacbacd",
                                         "cbadacd",
                                         "abababaca",
                                         "dacaaba",
                                         "abababacc",
                                         "dacaabc",
                                         "abababacd",
                                         "dacaabd",
                                         "ababadaca",
                                         "dacbaca",
                                         "ababadacc",
                                         "dacbacc",
                                         "ababadacd",
                                         "dacbacd",
                                         "abaaababaca",
                                         "bacacaaba",
                                         "abaaababacc",
                                         "bacacaabc",
                                         "abaaababacd",
                                         "bacacaabd",
                                         "abaaabadaca",
                                         "bacacbaca",
                                         "abaaabadacc",
                                         "bacacbacc",
                                         "abaaabadacd",
                                         "bacacbacd",
                                         "bacababaca",
                                         "abadacaaba",
                                         "bacababacc",
                                         "abadacaabc",
                                         "bacababacd",
                                         "abadacaabd",
                                         "bacabadaca",
                                         "abadacbaca",
                                         "bacabadacc",
                                         "abadacbacc",
                                         "bacabadacd",
                                         "abadacbacd",
                                         "cbbaababaca",
                                         "dabccaaba",
                                         "cbbaababacc",
                                         "dabccaabc",
                                         "cbbaababacd",
                                         "dabccaabd",
                                         "cbbaabadaca",
                                         "dabccbaca",
                                         "cbbaabadacc",
                                         "dabccbacc",
                                         "cbbaabadacd",
                                         "dabccbacd",
                                         "dabcbabaca",
                                         "cbbdacaaba",
                                         "dabcbabacc",
                                         "cbbdacaabc",
                                         "dabcbabacd",
                                         "cbbdacaabd",
                                         "dabcbadaca",
                                         "cbbdacbaca",
                                         "dabcbadacc",
                                         "cbbdacbacc",
                                         "dabcbadacd",
                                         "cbbdacbacd",
                                         "cbaaababaca",
                                         "ddacacaaba",
                                         "cbaaababacc",
                                         "ddacacaabc",
                                         "cbaaababacd",
                                         "ddacacaabd",
                                         "cbaaabadaca",
                                         "ddacacbaca",
                                         "cbaaabadacc",
                                         "ddacacbacc",
                                         "cbaaabadacd",
                                         "ddacacbacd",
                                         "daabadacbaca",
                                         "ccabadaca",
                                         "daabadacbacc",
                                         "ccabadacc",
                                         "daabadacbacd",
                                         "ccabadacd",
                                         "ddacababaca",
                                         "cbadacaaba",
                                         "ddacababacc",
                                         "cbadacaabc",
                                         "ddacababacd",
                                         "cbadacaabd",
                                         "ddacabadaca",
                                         "cbadacbaca",
                                         "ddacabadacc",
                                         "cbadacbacc",
                                         "ddacabadacd",
                                         "cbadacbacd",
                                         "ababadacbaca",
                                         "dacabadaca",
                                         "ababadacbacc",
                                         "dacabadacc",
                                         "ababadacbacd",
                                         "dacabadacd",
                                         "bacaaababaca",
                                         "ababacacaaba",
                                         "bacaaababacc",
                                         "ababacacaabc",
                                         "bacaaababacd",
                                         "ababacacaabd",
                                         "bacaaabadaca",
                                         "ababacacbaca",
                                         "bacaaabadacc",
                                         "ababacacbacc",
                                         "bacaaabadacd",
                                         "ababacacbacd",
                                         "ddacaaababaca",
                                         "cbabacacaaba",
                                         "ddacaaababacc",
                                         "cbabacacaabc",
                                         "ddacaaababacd",
                                         "cbabacacaabd",
                                         "ddacaaabadaca",
                                         "cbabacacbaca",
                                         "ddacaaabadacc",
                                         "cbabacacbacc",
                                         "ddacaaabadacd",
                                         "cbabacacbacd",
                                         "abaaabadacbaca",
                                         "bacacabadaca",
                                         "abaaabadacbacc",
                                         "bacacabadacc",
                                         "abaaabadacbacd",
                                         "bacacabadacd",
                                         "bacabadacbaca",
                                         "abadacabadaca",
                                         "bacabadacbacc",
                                         "abadacabadacc",
                                         "bacabadacbacd",
                                         "abadacabadacd",
                                         "cbbaabadacbaca",
                                         "dabccabadaca",
                                         "cbbaabadacbacc",
                                         "dabccabadacc",
                                         "cbbaabadacbacd",
                                         "dabccabadacd",
                                         "dabcbadacaaba",
                                         "cbbdacababaca",
                                         "dabcbadacaabc",
                                         "cbbdacababacc",
                                         "dabcbadacaabd",
                                         "cbbdacababacd",
                                         "dabcbadacbaca",
                                         "cbbdacabadaca",
                                         "dabcbadacbacc",
                                         "cbbdacabadacc",
                                         "dabcbadacbacd",
                                         "cbbdacabadacd",
                                         "cbaaabadacbaca",
                                         "ddacacabadaca",
                                         "cbaaabadacbacc",
                                         "ddacacabadacc",
                                         "cbaaabadacbacd",
                                         "ddacacabadacd",
                                         "ddacabadacbaca",
                                         "cbadacabadaca",
                                         "ddacabadacbacc",
                                         "cbadacabadacc",
                                         "ddacabadacbacd",
                                         "cbadacabadacd",
                                         "bacaaabadacbaca",
                                         "ababacacabadaca",
                                         "bacaaabadacbacc",
                                         "ababacacabadacc",
                                         "bacaaabadacbacd",
                                         "ababacacabadacd",
                                         "dabcbabacacaaba",
                                         "cbbdacaaababaca",
                                         "dabcbabacacaabc",
                                         "cbbdacaaababacc",
                                         "dabcbabacacaabd",
                                         "cbbdacaaababacd",
                                         "dabcbabacacbaca",
                                         "cbbdacaaabadaca",
                                         "dabcbabacacbacc",
                                         "cbbdacaaabadacc",
                                         "dabcbabacacbacd",
                                         "cbbdacaaabadacd",
                                         "dabcbadacabadaca",
                                         "cbbdacabadacbaca",
                                         "dabcbadacabadacc",
                                         "cbbdacabadacbacc"}));

    std::vector<std::string> expected;
    for (auto it = p.rules.cbegin(); it < p.rules.cend(); it += 2) {
      expected.push_back(*it + *(it + 1));
    }
    std::vector<std::string> found = expected;
    std::sort(found.begin(), found.end(), ShortLexCompare());
    REQUIRE(found == expected);
    for (size_t i = 0; i != found.size(); ++i) {
      REQUIRE(std::make_pair(found[i], i) == std::make_pair(expected[i], i));
    }
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "054",
                          "meaningful exception messages",
                          "[quick][presentation]") {
    using literals::operator""_w;
    auto            rg = ReportGuard(false);

    {
      Presentation<std::string> p;
      p.alphabet("ab");
      REQUIRE_EXCEPTION_MSG(p.throw_if_letter_not_in_alphabet('c'),
                            "invalid letter \'c\', valid letters are \"ab\"");
      if constexpr (std::is_unsigned_v<char>) {
        REQUIRE_EXCEPTION_MSG(p.throw_if_letter_not_in_alphabet(-109),
                              "invalid letter (char with value) 147, valid "
                              "letters are \"ab\" == [97, 98]");
      } else {
        REQUIRE_EXCEPTION_MSG(p.throw_if_letter_not_in_alphabet(-109),
                              "invalid letter (char with value) -109, valid "
                              "letters are \"ab\" == [97, 98]");
      }
      p.alphabet({0, 1});
      REQUIRE_EXCEPTION_MSG(
          p.throw_if_letter_not_in_alphabet('c'),
          "invalid letter 'c', valid letters are (char values) [0, 1]");
      if constexpr (std::is_unsigned_v<char>) {
        REQUIRE_EXCEPTION_MSG(p.throw_if_letter_not_in_alphabet(-109),
                              "invalid letter (char with value) 147, valid "
                              "letters are (char values) [0, 1]");
      } else {
        REQUIRE_EXCEPTION_MSG(p.throw_if_letter_not_in_alphabet(-109),
                              "invalid letter (char with value) -109, valid "
                              "letters are (char values) [0, 1]");
      }
      REQUIRE_EXCEPTION_MSG(
          p.alphabet(257), "expected a value in the range [0, 257), found 257");
      REQUIRE_EXCEPTION_MSG(p.alphabet("aba"),
                            "invalid alphabet \"aba\", duplicate letter 'a'!");
      REQUIRE_EXCEPTION_MSG(p.alphabet({0, 1, 0}),
                            "invalid alphabet (char values) [0, 1, 0], "
                            "duplicate letter (char with value) 0!");
      REQUIRE_EXCEPTION_MSG(
          presentation::add_inverse_rules(p, {0, 0}),
          "invalid inverses, the letter (char with value) 0 is duplicated!");
      p.alphabet(3);
      REQUIRE_EXCEPTION_MSG(
          presentation::add_inverse_rules(p, {'b', 'c', 'a'}),
          "invalid inverses, 'a' ^ -1 = 'b' but 'b' ^ -1 = 'c'");
      REQUIRE_EXCEPTION_MSG(
          presentation::add_inverse_rules(p, {'b', 'a', 'c'}, 'a'),
          "invalid inverses, the identity is 'a', but 'a' ^ -1 = 'b'");
      p.alphabet("abc");
      REQUIRE_EXCEPTION_MSG(presentation::add_inverse_rules(p, "aab"),
                            "invalid inverses, the letter 'a' is duplicated!");
      REQUIRE_EXCEPTION_MSG(
          presentation::add_inverse_rules(p, "bca"),
          "invalid inverses, 'a' ^ -1 = 'b' but 'b' ^ -1 = 'c'");
    }

    // TODO(1): Uncomment if we re-add strings over small ints, or delete.
    // {
    //   Presentation<std::basic_string<uint8_t>> p;
    //   p.alphabet({97, 98});
    //   REQUIRE_EXCEPTION_MSG(p.throw_if_letter_not_in_alphabet(99),
    //                         "invalid letter 99, valid letters are [97, 98]");
    //   REQUIRE_EXCEPTION_MSG(p.throw_if_letter_not_in_alphabet(147),
    //                         "invalid letter 147, valid letters are [97,
    //                         98]");
    //   p.alphabet({0, 1});
    //   REQUIRE_EXCEPTION_MSG(p.throw_if_letter_not_in_alphabet('c'),
    //                         "invalid letter 99, valid letters are [0, 1]");
    //   REQUIRE_EXCEPTION_MSG(
    //       p.alphabet(257), "expected a value in the range [0, 257), found
    //       257");
    //   REQUIRE_EXCEPTION_MSG(p.alphabet({0, 1, 0}),
    //                         "invalid alphabet [0, 1, 0], duplicate letter
    //                         0!");
    //   REQUIRE_EXCEPTION_MSG(p.alphabet({0, 1, 0}),
    //                         "invalid alphabet [0, 1, 0], duplicate letter
    //                         0!");
    //   REQUIRE_EXCEPTION_MSG(presentation::add_inverse_rules(p, {0, 0}),
    //                         "invalid inverses, the letter 0 is duplicated!");
    //   p.alphabet(3);
    //   REQUIRE_EXCEPTION_MSG(presentation::add_inverse_rules(p, {1, 2, 0}),
    //                         "invalid inverses, 0 ^ -1 = 1 but 1 ^ -1 = 2");
    //   REQUIRE_EXCEPTION_MSG(
    //       presentation::add_inverse_rules(p, {1, 0, 2}, 0),
    //       "invalid inverses, the identity is 0, but 0 ^ -1 = 1");
    //   p.alphabet({0, 1, 2});
    //   REQUIRE_EXCEPTION_MSG(presentation::add_inverse_rules(p, {0, 0, 1}),
    //                         "invalid inverses, the letter 0 is duplicated!");
    //   REQUIRE_EXCEPTION_MSG(presentation::add_inverse_rules(p, {1, 2, 0}),
    //                         "invalid inverses, 0 ^ -1 = 1 but 1 ^ -1 = 2");
    // }

    {
      Presentation<std::vector<uint8_t>> p;
      p.alphabet(2);
      p.contains_empty_word(true);
      REQUIRE_EXCEPTION_MSG(p.throw_if_letter_not_in_alphabet(99),
                            "invalid letter 99, valid letters are [0, 1]");
      REQUIRE_EXCEPTION_MSG(p.throw_if_letter_not_in_alphabet(109),
                            "invalid letter 109, valid letters are [0, 1]");
      REQUIRE_EXCEPTION_MSG(
          p.alphabet(257), "expected a value in the range [0, 257), found 257");
      REQUIRE(p.alphabet().size() == 2);
      REQUIRE_EXCEPTION_MSG(std::ignore = p.letter(3),
                            "expected a value in [0, 2), found 3");
      REQUIRE_EXCEPTION_MSG(p.alphabet({0, 1, 0}),
                            "invalid alphabet [0, 1, 0], duplicate letter 0!");
      REQUIRE_EXCEPTION_MSG(presentation::add_inverse_rules(p, {0, 0}),
                            "invalid inverses, the letter 0 is duplicated!");
      p.alphabet(3);
      REQUIRE_EXCEPTION_MSG(presentation::add_inverse_rules(p, {1, 2, 0}),
                            "invalid inverses, 0 ^ -1 = 1 but 1 ^ -1 = 2");
    }
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "055",
                          "add_generator (std::string)",
                          "[quick][presentation]") {
    auto            rg = ReportGuard(false);
    using literals::operator""_w;

    {
      Presentation<std::string> p;
      p.alphabet("ab");
      p.add_generator();
      REQUIRE(p.alphabet() == "abc");
    }
    {
      Presentation<std::string> p;
      p.alphabet("ac");
      p.add_generator();
      REQUIRE(p.alphabet() == "acb");
    }
    {
      Presentation<std::string> p;
      p.alphabet("ac");
      p.add_generator('b');
      REQUIRE(p.alphabet() == "acb");
    }
    {
      Presentation<std::string> p;
      p.alphabet("ac");
      REQUIRE_EXCEPTION_MSG(p.add_generator('c'),
                            "the argument 'c' already belongs to the "
                            "alphabet \"ac\", expected an unused letter");
    }
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "056",
                          "add_generator (word_type)",
                          "[quick][presentation]") {
    auto rg = ReportGuard(false);
    {
      Presentation<word_type> p;
      p.alphabet(word_type({0, 1}));
      p.add_generator();
      REQUIRE(p.alphabet() == word_type({0, 1, 2}));
    }
    {
      Presentation<word_type> p;
      p.alphabet(word_type({0, 2}));
      p.add_generator();
      REQUIRE(p.alphabet() == word_type({0, 2, 1}));
    }
    {
      Presentation<word_type> p;
      p.alphabet(word_type({0, 2}));
      p.add_generator(1);
      REQUIRE(p.alphabet() == word_type({0, 2, 1}));
    }
    {
      Presentation<word_type> p;
      p.alphabet(word_type({0, 2}));
      REQUIRE_EXCEPTION_MSG(p.add_generator(2),
                            "the argument 2 already belongs to the "
                            "alphabet [0, 2], expected an unused letter");
    }
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "057",
                          "remove_generator",
                          "[quick][presentation]") {
    auto rg = ReportGuard(false);
    check_remove_generator<word_type>();
    check_remove_generator<StaticVector1<uint16_t, 10>>();
    check_remove_generator<std::string>();
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "058",
                          "to_human_readble_repr",
                          "[quick][presentation]") {
    auto rg = ReportGuard(false);
    {
      Presentation<std::string> p;
      presentation::add_rule_no_checks(p, "abcb", "aa");
      p.alphabet_from_rules();
      presentation::add_identity_rules(p, 'a');
      REQUIRE(
          to_human_readable_repr(p)
          == "<semigroup presentation with 3 letters, 6 rules, and length 21>");
    }
    {
      Presentation<std::string> p;
      p.contains_empty_word(true);
      presentation::add_rule_no_checks(p, "abcb", "aa");
      p.alphabet_from_rules();
      presentation::add_identity_rules(p, 'a');
      REQUIRE(
          to_human_readable_repr(p)
          == "<monoid presentation with 3 letters, 6 rules, and length 21>");
    }
    {
      Presentation<std::string> p;
      REQUIRE(
          to_human_readable_repr(p)
          == "<semigroup presentation with 0 letters, 0 rules, and length 0>");
    }
    {
      Presentation<std::string> p;
      p.alphabet("a");
      REQUIRE(
          to_human_readable_repr(p)
          == "<semigroup presentation with 1 letter, 0 rules, and length 0>");
    }
    {
      Presentation<std::string> p;
      p.alphabet("a");
      presentation::add_rule_no_checks(p, "aa", "a");
      REQUIRE(
          to_human_readable_repr(p)
          == "<semigroup presentation with 1 letter, 1 rule, and length 3>");
    }
    {
      InversePresentation<word_type> ip;
      ip.alphabet({0, 1, 2, 3});
      presentation::add_rule_no_checks(ip, {0, 0, 0}, {1, 3});
      ip.inverses({0, 1, 2, 3});
      REQUIRE(to_human_readable_repr(ip)
              == "<inverse semigroup presentation with 4 letters, 1 rule, and "
                 "length 5>");
    }
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "059",
                          "to_word",
                          "[quick][presentation]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;

    p.alphabet("ab");
    v4::ToWord to_word(p.alphabet());
    REQUIRE(to_word("aaabbbab") == word_type({0, 0, 0, 1, 1, 1, 0, 1}));

    word_type output({1, 2, 3, 4, 5, 6, 7, 6, 5, 4, 3, 2, 1});
    to_word(output, "ababab");
    REQUIRE(output == word_type({0, 1, 0, 1, 0, 1}));
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "060",
                          "to_string",
                          "[quick][presentation]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;

    p.alphabet("ab");
    ToString to_string(p.alphabet());
    REQUIRE(to_string(word_type({0, 0, 0, 1, 1, 1, 0, 1})) == "aaabbbab");

    std::string output("sample");
    to_string(output, word_type({0, 1, 1, 0}));
    REQUIRE(output == "abba");
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "061",
                          "to_gap_string",
                          "[quick][presentation]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;

    p.alphabet(50);
    std::string var_name("my_var");
    REQUIRE_EXCEPTION_MSG(presentation::to_gap_string(p, var_name),
                          "expected at most 49 generators, found 50!");

    p.init();
    p.alphabet("abc");
    presentation::add_rule_no_checks(p, "abba", "bac");
    presentation::add_rule_no_checks(p, "ba", "ab");
    presentation::add_rule_no_checks(p, "cab", "ba");

    REQUIRE(presentation::to_gap_string(p, var_name)
            == "F := FreeSemigroup(\"a\", \"b\", \"c\");\n"
               "AssignGeneratorVariables(F);;\n"
               "R := [\n"
               "          [a * b * b * a, b * a * c], \n"
               "          [b * a, a * b], \n"
               "          [c * a * b, b * a]\n"
               "         ];\n"
               "my_var := F / R;\n");

    p.contains_empty_word(true);
    presentation::add_rule_no_checks(p, "cba", "");
    REQUIRE(presentation::to_gap_string(p, var_name)
            == "F := FreeMonoid(\"a\", \"b\", \"c\");\n"
               "AssignGeneratorVariables(F);;\n"
               "R := [\n"
               "          [a * b * b * a, b * a * c], \n"
               "          [b * a, a * b], \n"
               "          [c * a * b, b * a], \n"
               "          [c * b * a, One(F)]\n"
               "         ];\n"
               "my_var := F / R;\n");
  }
  LIBSEMIGROUPS_TEST_CASE("InversePresentation",
                          "062",
                          "constructors (word_type)",
                          "[quick][presentation]") {
    auto                           rg = ReportGuard(false);
    InversePresentation<word_type> ip;
    ip.alphabet({0, 1, 2});
    presentation::add_rule_no_checks(ip, {0, 0, 0}, {0});
    REQUIRE(ip.rules.size() == 2);
    presentation::add_rule(ip, {0, 0, 0}, {0});
    REQUIRE_THROWS_AS(ip.throw_if_bad_alphabet_rules_or_inverses(),
                      LibsemigroupsException);
    REQUIRE_EXCEPTION_MSG(ip.inverse(0), "no inverses have been defined");
    ip.inverses_no_checks({2, 1, 0});
    ip.throw_if_bad_alphabet_rules_or_inverses();
    check_inverse_constructors(ip);
  }

  LIBSEMIGROUPS_TEST_CASE("InversePresentation",
                          "063",
                          "constructors (StaticVector1)",
                          "[quick][presentation]") {
    auto                                             rg = ReportGuard(false);
    InversePresentation<StaticVector1<uint16_t, 16>> ip;
    ip.alphabet({0, 1, 2});
    presentation::add_rule_no_checks(ip, {0, 0, 0}, {0});
    REQUIRE(ip.rules.size() == 2);
    presentation::add_rule(ip, {0, 0, 0}, {0});
    REQUIRE_THROWS_AS(ip.throw_if_bad_alphabet_rules_or_inverses(),
                      LibsemigroupsException);
    REQUIRE_EXCEPTION_MSG(ip.inverse(0), "no inverses have been defined");
    ip.inverses_no_checks({2, 1, 0});
    ip.throw_if_bad_alphabet_rules_or_inverses();
    check_inverse_constructors(ip);
  }

  LIBSEMIGROUPS_TEST_CASE("InversePresentation",
                          "064",
                          "constructors (std::string)",
                          "[quick][presentation]") {
    auto                             rg = ReportGuard(false);
    InversePresentation<std::string> ip;
    ip.alphabet({0, 1, 2});
    presentation::add_rule_no_checks(ip, {0, 0, 0}, {0});
    REQUIRE(ip.rules.size() == 2);
    presentation::add_rule(ip, {0, 0, 0}, {0});
    REQUIRE_THROWS_AS(ip.throw_if_bad_alphabet_rules_or_inverses(),
                      LibsemigroupsException);
    REQUIRE_EXCEPTION_MSG(ip.inverse(0), "no inverses have been defined");
    ip.inverses_no_checks({2, 1, 0});
    ip.throw_if_bad_alphabet_rules_or_inverses();
    check_inverse_constructors(ip);
  }

  LIBSEMIGROUPS_TEST_CASE("InversePresentation",
                          "065",
                          "construct from presnetation",
                          "[quick][presentation]") {
    auto rg = ReportGuard(false);
    check_construct_from_presentation<word_type>();
    check_construct_from_presentation<StaticVector1<uint16_t, 16>>();
    check_construct_from_presentation<std::string>();
  }

  LIBSEMIGROUPS_TEST_CASE("InversePresentation",
                          "066",
                          "inverses",
                          "[quick][presentation]") {
    auto rg = ReportGuard(false);
    check_inverses<word_type>();
    check_inverses<StaticVector1<uint16_t, 16>>();
    check_inverses<std::string>();
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "067",
                          "longest_subword_reducing_length #01",
                          "[quick][presentation]") {
    using literals::        operator""_w;
    Presentation<word_type> p;
    p.alphabet(4);
    presentation::add_rule(p, 1212_w, 0_w);
    presentation::add_rule(p, 1213121312131213_w, 0_w);

    REQUIRE(presentation::longest_subword_reducing_length(p) == 1213_w);
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "068",
                          "longest_subword_reducing_length #02",
                          "[quick][presentation]") {
    Presentation<std::string> p;
    p.alphabet("ab");
    presentation::add_rule(p, "aaaaa", "bbb");
    presentation::add_rule(p, "ababa", "aaabaabaaabaa");
    REQUIRE(presentation::longest_subword_reducing_length(p) == "aba");
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "069",
                          "longest_subword_reducing_length #03",
                          "[quick][presentation]") {
    Presentation<std::string> p;
    p.alphabet("abc").contains_empty_word(true);
    presentation::add_rule(p, "aaaaa", "bbb");
    presentation::add_rule(p, "cba", "aaccaca");
    presentation::add_rule(p, "aba", "");
    REQUIRE(presentation::longest_subword_reducing_length(p) == "");
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "070",
                          "longest_subword_reducing_length #04",
                          "[quick][presentation]") {
    using literals::        operator""_w;
    Presentation<word_type> p;
    p.alphabet(4);
    presentation::add_rule(p, 00_w, 10_w);
    presentation::add_rule(p, 01_w, 20_w);
    presentation::add_rule(p, 02_w, 30_w);
    presentation::add_rule(p, 03_w, 11_w);
    presentation::add_rule(p, 23_w, 222_w);
    presentation::add_rule(
        p, 12121212121212_w, 12131213121312131213121312131213_w);
    REQUIRE(presentation::longest_subword_reducing_length(p) == 12131213_w);
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "071",
                          "longest_subword_reducing_length #05",
                          "[quick][presentation]") {
    using literals::        operator""_w;
    Presentation<word_type> p;
    p.alphabet(5).contains_empty_word(true);
    presentation::add_rule(p, 00_w, 10_w);
    presentation::add_rule(p, 01_w, 20_w);
    presentation::add_rule(p, 02_w, 30_w);
    presentation::add_rule(p, 03_w, 11_w);
    presentation::add_rule(p, 23_w, 222_w);
    presentation::add_rule(p, 12121212121212_w, 44444444_w);
    presentation::add_rule(p, 1213_w, {});

    REQUIRE(presentation::longest_subword_reducing_length(p) == 12_w);
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "072",
                          "longest_subword_reducing_length #06",
                          "[quick][presentation]") {
    using literals::        operator""_w;
    Presentation<word_type> p;
    p.alphabet(6).contains_empty_word(true);
    presentation::add_rule(p, 00_w, 10_w);
    presentation::add_rule(p, 01_w, 20_w);
    presentation::add_rule(p, 02_w, 30_w);
    presentation::add_rule(p, 03_w, 11_w);
    presentation::add_rule(p, 23_w, 222_w);
    presentation::add_rule(p, 5555555_w, 44444444_w);
    presentation::add_rule(p, 513_w, 12_w);

    REQUIRE(presentation::longest_subword_reducing_length(p) == 4444_w);
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "073",
                          "longest_subword_reducing_length #07",
                          "[quick][presentation]") {
    Presentation<std::string> p;
    p.alphabet("abc").contains_empty_word(true);
    presentation::add_rule(p, "aaaaaaaaaaaaaa", "bbbbbbbbbbbbbb");
    presentation::add_rule(p, "cccccccccccccc", "aaaaba");
    presentation::add_rule(p, "bbb", "bbbbab");
    presentation::add_rule(p, "aaa", "aaaaca");
    presentation::add_rule(p, "ccc", "ccccac");
    presentation::add_rule(p, "aaa", "bbbbcb");
    presentation::add_rule(p, "ccc", "ccccbc");
    presentation::add_rule(p, "bbb", "");

    REQUIRE(presentation::longest_subword_reducing_length(p) == "ccc");
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "074",
                          "longest_subword_reducing_length #08",
                          "[quick][presentation]") {
    Presentation<std::string> p;
    p.alphabet("abcd");
    presentation::add_rule(p, "aaaaaaaaaaaaaa", "bbbbbbbbbbbbbb");
    presentation::add_rule(p, "ddddcc", "aaaaba");
    presentation::add_rule(p, "bbb", "bbbbab");
    presentation::add_rule(p, "aaa", "aaaaca");
    presentation::add_rule(p, "dcac", "aaa");
    presentation::add_rule(p, "bbbbcb", "dcbc");
    presentation::add_rule(p, "bbb", "ccc");

    REQUIRE(presentation::longest_subword_reducing_length(p) == "bbb");
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "075",
                          "longest_subword_reducing_length #09",
                          "[quick][presentation]") {
    Presentation<std::string> p;
    p.alphabet("ab");
    presentation::add_rule(p, "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", "bbb");
    presentation::add_rule(
        p, "ababa", "aaaaaaaaaaaaaaaabaaaabaaaaaaaaaaaaaaaabaaaa");

    REQUIRE(presentation::longest_subword_reducing_length(p) == "aaaa");
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "076",
                          "longest_subword_reducing_length #10",
                          "[quick][presentation]") {
    Presentation<std::string> p;
    p.alphabet("abcABC").contains_empty_word(true);
    presentation::add_rule(p, "aBCbac", "bACbaacA");
    presentation::add_rule(p, "accAABab", "");

    REQUIRE(presentation::longest_subword_reducing_length(p) == "");
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "077",
                          "greedy_reduce_length",
                          "[quick][presentation]") {
    Presentation<std::string> p;
    // Want to go over the number 127, in case char's are signed, to check that
    // this works properly.
    p.alphabet(127);
    p.rules = {"aaaaaaaaaaaaaaaaaaaaaa", ""};

    presentation::greedy_reduce_length_and_number_of_gens(p);
    REQUIRE(p.alphabet().size() == 128);

    p.alphabet(128);
    p.rules = {"aaaaaaaaaaaaaaaaaaaaaa", ""};

    presentation::greedy_reduce_length_and_number_of_gens(p);
    REQUIRE(p.alphabet().size() == 129);

    p.alphabet(255);
    p.rules = {"aaaaaaaaaaaaaaaaaaaaaa", ""};
    presentation::greedy_reduce_length_and_number_of_gens(p);
    REQUIRE(p.alphabet().size() == 256);

    p.alphabet(256);
    p.rules = {"aaaaaaaaaaaaaaaaaaaaaa", ""};
    REQUIRE_THROWS_AS(presentation::greedy_reduce_length_and_number_of_gens(p),
                      LibsemigroupsException);
    REQUIRE(p.alphabet().size() == 256);
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("Presentation",
                                   "078",
                                   "add_cyclic_conjugates",
                                   "[quick][presentation]",
                                   std::string,
                                   word_type,
                                   (StaticVector1<uint16_t, 3>) ) {
    using Word = TestType;

    Presentation<Word> p;
    p.alphabet({97, 98, 99});
    p.contains_empty_word(true);

    presentation::add_cyclic_conjugates(p, {97, 98, 99});
    presentation::add_cyclic_conjugates(p, {97, 97, 97});

    REQUIRE(p.rules
            == std::vector<Word>({{97, 98, 99},
                                  {},
                                  {98, 99, 97},
                                  {},
                                  {99, 97, 98},
                                  {},
                                  {97, 98, 99},
                                  {},
                                  {97, 97, 97},
                                  {},
                                  {97, 97, 97},
                                  {},
                                  {97, 97, 97},
                                  {},
                                  {97, 97, 97},
                                  {}}));

    REQUIRE_THROWS_AS(presentation::add_cyclic_conjugates(p, {100, 101}),
                      LibsemigroupsException);
    p.contains_empty_word(false);
    REQUIRE_THROWS_AS(presentation::add_cyclic_conjugates(p, {99, 97, 99}),
                      LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("Presentation",
                                   "079",
                                   "throw_if_not_normalized",
                                   "[quick][presentation]",
                                   std::string,
                                   word_type,
                                   (StaticVector1<uint16_t, 10>) ) {
    Presentation<TestType> p;
    p.alphabet({0, 1, 2});
    REQUIRE_NOTHROW(presentation::throw_if_not_normalized(p));
    p.alphabet({0, 2, 1});
    REQUIRE_THROWS_AS(presentation::throw_if_not_normalized(p),
                      LibsemigroupsException);
    p.alphabet({10, 11, 12});
    REQUIRE_THROWS_AS(presentation::throw_if_not_normalized(p),
                      LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "080",
                          "add_cyclic_conjugates(char const*)",
                          "[quick][presentation]") {
    Presentation<std::string> p;
    p.alphabet("abc");
    // Intentionally use no_checks in next line to test this specific function
    presentation::add_cyclic_conjugates_no_checks(p, "bca");
    p.contains_empty_word(true);
    presentation::add_cyclic_conjugates(p, "abc");
    REQUIRE(p.rules
            == std::vector<std::string>({"bca",
                                         "",
                                         "cab",
                                         "",
                                         "abc",
                                         "",
                                         "bca",
                                         "",
                                         "abc",
                                         "",
                                         "bca",
                                         "",
                                         "cab",
                                         "",
                                         "abc",
                                         ""}));
    REQUIRE_THROWS_AS(presentation::add_cyclic_conjugates(p, "de"),
                      LibsemigroupsException);
    p.contains_empty_word(false);
    REQUIRE_THROWS_AS(presentation::add_cyclic_conjugates(p, "caca"),
                      LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "081",
                          "to_report_string",
                          "[quick][presentation]") {
    Presentation<std::string> p;
    REQUIRE(presentation::to_report_string(p)
            == "|A| = 0, |R| = 0, |u| + |v| ∈ [0, 0], "
               "∑(|u| + |v|) = 0\n");
  }

}  // namespace libsemigroups
