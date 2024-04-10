//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2022-2023 James D. Mitchell
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
#define CATCH_CONFIG_ENABLE_PAIR_STRINGMAKER

#include <algorithm>      // for all_of, equal, fill, sort
#include <chrono>         // for milliseconds
#include <cstddef>        // for size_t
#include <iterator>       // for distance
#include <math.h>         // for pow
#include <stdint.h>       // for uint16_t
#include <string>         // for basic_string, operator==
#include <unordered_map>  // for operator==, operator!=
#include <unordered_set>  // for unordered_set
#include <utility>        // for move, make_pair, swap
#include <vector>         // for vector, operator==, swap

#include "catch.hpp"      // for operator""_catch_sr
#include "test-main.hpp"  // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/ranges.hpp"  // for ChainRange, get_range...

#include "libsemigroups/bipart.hpp"           // for Bipartition
#include "libsemigroups/constants.hpp"        // for operator==, operator!=
#include "libsemigroups/exception.hpp"        // for LibsemigroupsException
#include "libsemigroups/froidure-pin.hpp"     // for FroidurePin
#include "libsemigroups/knuth-bendix.hpp"     // for redundant_rule
#include "libsemigroups/order.hpp"            // for ShortLexCompare, shor...
#include "libsemigroups/presentation.hpp"     // for Presentation, human_r...
#include "libsemigroups/ranges.hpp"           // for chain, shortlex_compare
#include "libsemigroups/to-presentation.hpp"  // for to_presentation
#include "libsemigroups/types.hpp"            // for word_type, letter_type
#include "libsemigroups/words.hpp"            // for operator+=, operator""_w

#include "libsemigroups/detail/containers.hpp"  // for StaticVector1, operat...
#include "libsemigroups/detail/int-range.hpp"   // for IntRange
#include "libsemigroups/detail/report.hpp"      // for ReportGuard
#include "libsemigroups/detail/string.hpp"      // for operator<<

namespace libsemigroups {

  struct LibsemigroupsException;  // forward decl

  namespace {
    template <typename W>
    void check_constructors(Presentation<W>& p) {
      p.validate();
      Presentation<W> pp(p);
      pp.validate();
      REQUIRE(pp.alphabet() == p.alphabet());
      REQUIRE(pp.rules == p.rules);

      Presentation<W> q(std::move(p));
      q.validate();
      REQUIRE(q.alphabet() == pp.alphabet());
      REQUIRE(q.rules == pp.rules);

      p = q;
      p.validate();
      REQUIRE(q.alphabet() == p.alphabet());
      REQUIRE(q.rules == p.rules);

      p = std::move(q);
      p.validate();
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
      REQUIRE(p.alphabet() == W({0, 1, 2, 3}));
      p.validate();
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
    void check_validate_rules_throws() {
      Presentation<W> p;
      p.rules.emplace_back();
      REQUIRE_THROWS_AS(p.validate_rules(), LibsemigroupsException);
    }

    template <typename W>
    void check_add_rules() {
      Presentation<W> p;
      presentation::add_rule_no_checks(p, {0, 1, 2, 1}, {0, 0});
      Presentation<W> q;
      presentation::add_rule_no_checks(q, {4, 1}, {0, 5});
      presentation::add_rule_no_checks(
          q, {4, 1}, {0, 1, 1, 1, 1, 1, 1, 1, 1, 1});
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
      REQUIRE_THROWS_AS(p.validate(), LibsemigroupsException);
      REQUIRE_THROWS_AS(q.validate(), LibsemigroupsException);
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

      auto p = to_presentation<W>(S);
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
      REQUIRE(*it == W({2, 1, 3, 1, 1, 2, 1, 2}));
      REQUIRE(*(it + 1) == W({1, 1, 2, 1, 3, 1, 2, 1}));
      p.rules.erase(it, it + 2);
      p.validate();
      // while (it != p.rules.cend()) { // Too time consuming and indeterminant
      //   REQUIRE(std::distance(it, p.rules.cend()) % 2 == 0);
      //   p.rules.erase(it, it + 2);
      //   p.validate();
      //   it = knuth_bendix::redundant_rule(p, std::chrono::milliseconds(8));
      // }
      REQUIRE(presentation::length(p) == 343);
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
      REQUIRE_THROWS_AS(presentation::remove_redundant_generators(p),
                        LibsemigroupsException);
      p.rules.push_back(W({1, 2, 1}));

      presentation::add_rule_no_checks(p, {1, 1, 2, 1}, {1, 1});
      presentation::add_rule_no_checks(p, {1, 2, 1}, {0});

      presentation::remove_redundant_generators(p);
      REQUIRE(p.rules
              == std::vector<W>(
                  {{1, 2, 1, 1, 2, 1}, {1, 2, 1}, {1, 1, 2, 1}, {1, 1}}));
      presentation::remove_redundant_generators(p);
      REQUIRE(p.rules
              == std::vector<W>(
                  {{1, 2, 1, 1, 2, 1}, {1, 2, 1}, {1, 1, 2, 1}, {1, 1}}));

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

  }  // namespace

  using detail::StaticVector1;

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "000",
                          "vectors of ints",
                          "[quick][presentation]") {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.alphabet({0, 1, 2});
    REQUIRE(p.alphabet() == word_type({0, 1, 2}));
    REQUIRE_THROWS_AS(p.alphabet({0, 0}), LibsemigroupsException);
    REQUIRE(p.alphabet() == word_type({0, 1, 2}));
    presentation::add_rule_no_checks(p, {0, 0, 0}, {0});
    REQUIRE(std::distance(p.rules.cbegin(), p.rules.cend()) == 2);
    REQUIRE(std::vector<word_type>(p.rules.cbegin(), p.rules.cend())
            == std::vector<word_type>({{0, 0, 0}, {0}}));
    presentation::add_rule(p, {0, 0, 0}, {0});
    REQUIRE_THROWS_AS(presentation::add_rule(p, {0, 5, 0}, {0}),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(presentation::add_rule(p, {}, {0}),
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
    p.validate();
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
    p.validate();
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
    p.validate();
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
    p.validate();
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
                          "validate_rules throws",
                          "[quick][presentation]") {
    auto rg = ReportGuard(false);
    check_validate_rules_throws<word_type>();
    check_validate_rules_throws<StaticVector1<uint16_t, 16>>();
    check_validate_rules_throws<std::string>();
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "009",
                          "helpers add_rule(s)",
                          "[quick][presentation]") {
    auto rg = ReportGuard(false);
    check_add_rules<word_type>();
    check_add_rules<StaticVector1<uint16_t, 10>>();
    check_add_rules<std::string>();
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "010",
                          "helpers add_rule(s) (std::string)",
                          "[quick][presentation]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    presentation::add_rule_no_checks(p, "abcb", "aa");
    Presentation<std::string> q;
    presentation::add_rule_no_checks(q, "eb", "af");
    presentation::add_rule_no_checks(q, "eb", "abbbbbbbbb");
    presentation::add_rules_no_checks(p, q);
    REQUIRE(p.rules
            == std::vector<std::string>(
                {"abcb", "aa", "eb", "af", "eb", "abbbbbbbbb"}));
    REQUIRE(q.rules
            == std::vector<std::string>({"eb", "af", "eb", "abbbbbbbbb"}));
    REQUIRE_THROWS_AS(p.validate(), LibsemigroupsException);
    REQUIRE_THROWS_AS(q.validate(), LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE(
      "Presentation",
      "011",
      "helpers add_identity_rules (std::vector/StaticVector1)",
      "[quick][presentation]") {
    auto rg = ReportGuard(false);
    check_add_identity_rules<word_type>();
    check_add_identity_rules<StaticVector1<uint16_t, 10>>();
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "035",
                          "helpers add_zero_rules (std::vector/StaticVector1)",
                          "[quick][presentation]") {
    auto rg = ReportGuard(false);
    check_add_zero_rules<word_type>();
    check_add_zero_rules<StaticVector1<uint16_t, 10>>();
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "012",
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
                          "036",
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
                          "013",
                          "helpers add_inverse_rules (all)",
                          "[quick][presentation]") {
    auto rg = ReportGuard(false);
    check_add_inverse_rules<word_type>();
    check_add_inverse_rules<StaticVector1<uint16_t, 10>>();
    check_add_inverse_rules<std::string>();
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "014",
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
                          "015",
                          "helpers remove_duplicate_rules",
                          "[quick][presentation]") {
    auto rg = ReportGuard(false);
    check_remove_duplicate_rules<word_type>();
    check_remove_duplicate_rules<StaticVector1<uint16_t, 10>>();
    check_remove_duplicate_rules<std::string>();
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "016",
                          "helpers reduce_complements",
                          "[quick][presentation]") {
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
    REQUIRE(p.letter_no_checks(0) == presentation::human_readable_letter(p, 0));
    REQUIRE(p.letter_no_checks(1) == presentation::human_readable_letter(p, 1));
    REQUIRE(p.letter_no_checks(2) == presentation::human_readable_letter(p, 2));
    p.validate();

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
                          "017",
                          "helpers sort_each_rule",
                          "[quick][presentation]") {
    auto rg = ReportGuard(false);
    check_sort_each_rule<word_type>();
    check_sort_each_rule<StaticVector1<uint16_t, 10>>();
    check_sort_each_rule<std::string>();
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "018",
                          "helpers sort_rules",
                          "[quick][presentation]") {
    auto rg = ReportGuard(false);
    check_sort_rules<word_type>();
    check_sort_rules<StaticVector1<uint16_t, 10>>();
    check_sort_rules<std::string>();
  }

  LIBSEMIGROUPS_TEST_CASE(
      "Presentation",
      "019",
      "helpers longest_subword_reducing_length/replace_subword",
      "[quick][presentation]") {
    auto rg = ReportGuard(false);
    check_longest_subword_reducing_length<word_type>();
    check_longest_subword_reducing_length<StaticVector1<uint16_t, 10>>();
    check_longest_subword_reducing_length<std::string>();
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "020",
                          "helpers redundant_rule",
                          "[quick][presentation]") {
    auto rg = ReportGuard(false);
    check_redundant_rule<word_type>();
    check_redundant_rule<StaticVector1<uint16_t, 10>>();
    check_redundant_rule<std::string>();
  }

  // LIBSEMIGROUPS_TEST_CASE("Presentation",
  //                         "021",
  //                         "helpers shortlex_compare_concat",
  //                         "[quick][presentation]") {
  //   auto rg = ReportGuard(false);
  //   check_shortlex_compare_concat<word_type>();
  //   check_shortlex_compare_concat<StaticVector1<uint16_t, 10>>();
  //   check_shortlex_compare_concat<std::string>();
  // }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "022",
                          "helpers remove_trivial_rules",
                          "[quick][presentation]") {
    auto rg = ReportGuard(false);
    check_remove_trivial_rules<word_type>();
    check_remove_trivial_rules<StaticVector1<uint16_t, 10>>();
    check_remove_trivial_rules<std::string>();
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "023",
                          "helpers replace_subword (existing, replacement)",
                          "[quick][presentation]") {
    auto rg = ReportGuard(false);
    check_replace_subword<word_type>();
    check_replace_subword<StaticVector1<uint16_t, 64>>();
    check_replace_subword<std::string>();
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "030",
                          "helpers replace_word",
                          "[quick][presentation]") {
    auto rg = ReportGuard(false);
    check_replace_word<word_type>();
    check_replace_word<StaticVector1<uint16_t, 10>>();
    check_replace_word<std::string>();
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "024",
                          "helpers longest_rule",
                          "[quick][presentation]") {
    auto rg = ReportGuard(false);
    check_longest_rule<word_type>();
    check_longest_rule<StaticVector1<uint16_t, 10>>();
    check_longest_rule<std::string>();
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "025",
                          "helpers longest_rule_length",
                          "[quick][presentation]") {
    auto rg = ReportGuard(false);
    check_longest_rule_length<word_type>();
    check_longest_rule_length<StaticVector1<uint16_t, 10>>();
    check_longest_rule_length<std::string>();
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "026",
                          "helpers remove_redundant_generators",
                          "[quick][presentation]") {
    auto rg = ReportGuard(false);
    check_remove_redundant_generators<word_type>();
    check_remove_redundant_generators<StaticVector1<uint16_t, 64>>();
    check_remove_redundant_generators<std::string>();
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "027",
                          "helpers reverse",
                          "[quick][presentation]") {
    auto rg = ReportGuard(false);
    check_reverse<word_type>();
    check_reverse<StaticVector1<uint16_t, 10>>();
    check_reverse<std::string>();
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "028",
                          "in_alphabet",
                          "[quick][presentation]") {
    auto rg = ReportGuard(false);
    check_in_alphabet<word_type>();
    check_in_alphabet<StaticVector1<uint16_t, 10>>();
    check_in_alphabet<std::string>();
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "029",
                          "replace_subword with empty word",
                          "[quick][presentation]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet(2);
    p.contains_empty_word(true);
    presentation::add_rule_no_checks(p, {0, 0, 0}, {});
    p.validate();
    REQUIRE_THROWS_AS(presentation::replace_subword(p, {}, {2}),
                      LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "031",
                          "clear",
                          "[quick][presentation]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet(2);
    p.contains_empty_word(true);
    presentation::add_rule_no_checks(p, {0, 0, 0}, {});
    p.validate();
    p.init();
    REQUIRE(p.alphabet().empty());
    REQUIRE(p.rules.empty());
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "040",
                          "change_alphabet",
                          "[quick][presentation]") {
    Presentation<std::string> p;
    p.alphabet("ab");
    presentation::add_rule(p, "ba", "abaaabaa");
    presentation::replace_word_with_new_generator(p, "ba");
    presentation::change_alphabet(p, "abc");
    REQUIRE(p.rules == std::vector<std::string>({"c", "acaaca", "c", "ba"}));
    REQUIRE(p.alphabet() == "abc");
    REQUIRE_NOTHROW(p.validate());
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
                          "032",
                          "letter",
                          "[quick][presentation]") {
    Presentation<std::vector<uint16_t>> p;
    REQUIRE_THROWS_AS(presentation::human_readable_letter(p, 65536),
                      LibsemigroupsException);
    REQUIRE(presentation::human_readable_letter(p, 10) == 10);
    REQUIRE_THROWS_AS(human_readable_char(65536), LibsemigroupsException);
    REQUIRE(human_readable_char(0) == 'a');
    REQUIRE(human_readable_char(10) == 'k');

    detail::IntRange          ir(0, 255);
    Presentation<std::string> q;

    REQUIRE(std::all_of(ir.cbegin(), ir.cend(), [&q](size_t i) {
      return human_readable_char(i)
             == presentation::human_readable_letter(q, i);
    }));
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "033",
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
                          "042",
                          "first_unused_letter/letter",
                          "[quick][presentation]") {
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
      REQUIRE(letters[i] == presentation::human_readable_letter(p, i));
      REQUIRE(set.insert(letters[i]).second);
    }
    for (size_t i = letters.size(); i < 255; ++i) {
      REQUIRE(set.insert(presentation::human_readable_letter(p, i)).second);
    }
    REQUIRE_THROWS_AS(presentation::human_readable_letter(p, 255),
                      LibsemigroupsException);
    p.alphabet(255);
    REQUIRE_THROWS_AS(presentation::first_unused_letter(p),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(p.alphabet(256), LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "034",
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
                          "037",
                          "make_semigroup",
                          "[quick][presentation]") {
    check_make_semigroup<word_type>();
    check_make_semigroup<StaticVector1<uint16_t, 10>>();
    check_make_semigroup<std::string>();
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "038",
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
                          "039",
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
                          "043",
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
                          "044",
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
                          "045",
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
                          "041",
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

    p.validate();
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

  namespace {
    std::string chomp(std::string_view what) {
      size_t pos = what.find(": ");
      LIBSEMIGROUPS_ASSERT(pos != std::string::npos);
      LIBSEMIGROUPS_ASSERT(pos + 2 <= what.size() - 1);
      return std::string(what.begin() + pos + 2);
    }
  }  // namespace

#define REQUIRE_EXCEPTION_MSG(code, expected)      \
  REQUIRE_THROWS_AS(code, LibsemigroupsException); \
  try {                                            \
    code;                                          \
  } catch (LibsemigroupsException const& e) {      \
    REQUIRE(chomp(e.what()) == expected);          \
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "021",
                          "meaningful exception messages",
                          "[quick][presentation]") {
    using literals::operator""_w;
    auto rg = ReportGuard(false);

    {
      Presentation<std::string> p;
      p.alphabet("ab");
      REQUIRE_EXCEPTION_MSG(p.validate_letter('c'),
                            "invalid letter \'c\', valid letters are \"ab\"");
      REQUIRE_EXCEPTION_MSG(p.validate_letter(-109),
                            "invalid letter (char with value) -109, valid "
                            "letters are \"ab\" == [97, 98]");
      p.alphabet({0, 1});
      REQUIRE_EXCEPTION_MSG(
          p.validate_letter('c'),
          "invalid letter 'c', valid letters are (char values) [0, 1]");
      if constexpr (std::is_unsigned_v<char>) {
        REQUIRE_EXCEPTION_MSG(p.validate_letter(148),
                              "invalid letter (char with value) 148, valid "
                              "letters are (char values) [0, 1]");
      } else {
        REQUIRE_EXCEPTION_MSG(p.validate_letter(-109),
                              "invalid letter (char with value) -109, valid "
                              "letters are (char values) [0, 1]");
      }
      REQUIRE_EXCEPTION_MSG(p.alphabet(256),
                            "expected a value in the range [0, 256) found 256");
      REQUIRE_EXCEPTION_MSG(p.alphabet("aba"),
                            "invalid alphabet \"aba\", duplicate letter 'a'!");
      REQUIRE_EXCEPTION_MSG(p.alphabet({0, 1, 0}),
                            "invalid alphabet (char values) [0, 1, 0], "
                            "duplicate letter (char with value) 0!");
      REQUIRE_EXCEPTION_MSG(
          presentation::add_inverse_rules(p, {0, 0}),
          "invalid inverses, the letter (char with value) 0 is duplicated!");
      p.alphabet(3);
      // TODO this one could be better
      REQUIRE_EXCEPTION_MSG(presentation::add_inverse_rules(p, {1, 2, 0}),
                            "invalid inverses, (char with value) 0 ^ -1 = "
                            "(char with value) 1 but (char with value) 1 ^ -1 "
                            "= (char with value) 2");
      p.alphabet("abc");
      REQUIRE_EXCEPTION_MSG(presentation::add_inverse_rules(p, "aab"),
                            "invalid inverses, the letter 'a' is duplicated!");
      REQUIRE_EXCEPTION_MSG(
          presentation::add_inverse_rules(p, "bca"),
          "invalid inverses, 'a' ^ -1 = 'b' but 'b' ^ -1 = 'c'");
    }
    {
      Presentation<std::vector<uint8_t>> p;
      p.alphabet(2);
      p.contains_empty_word(true);
      REQUIRE_EXCEPTION_MSG(p.validate_letter(99),
                            "invalid letter 99, valid letters are [0, 1]");
      REQUIRE_EXCEPTION_MSG(p.validate_letter(109),
                            "invalid letter 109, valid letters are [0, 1]");
      REQUIRE_EXCEPTION_MSG(p.alphabet(256),
                            "expected a value in the range [0, 256) found 256");
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
}  // namespace libsemigroups
