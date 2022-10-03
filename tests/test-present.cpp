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

#include <cstddef>  // for size_t

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
      REQUIRE(p.letter(0) == 0);
      REQUIRE(p.letter(1) == 1);
      REQUIRE(p.letter(2) == 2);
      p.alphabet(4);
      REQUIRE(p.alphabet() == W({0, 1, 2, 3}));
      p.validate();
      REQUIRE_THROWS_AS(p.alphabet({0, 1, 1}), LibsemigroupsException);

      presentation::add_rule(p, {0, 1, 2, 1}, {0, 0});
      presentation::add_rule(p, {4, 1}, {0, 5});
      presentation::add_rule(p, {4, 1}, {0, 1, 1, 1, 1, 1, 1, 1, 1, 1});
      p.alphabet_from_rules();
      REQUIRE(p.alphabet() == W({0, 1, 2, 4, 5}));
      REQUIRE(p.index(0) == 0);
      REQUIRE(p.index(1) == 1);
      REQUIRE(p.index(2) == 2);
      REQUIRE(p.index(4) == 3);
      REQUIRE(p.index(5) == 4);

      REQUIRE(!p.contains_empty_word());
      presentation::add_rule(p, {4, 1}, {});
      p.alphabet_from_rules();
      REQUIRE(p.contains_empty_word());
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
      presentation::add_rule(p, {0, 1, 2, 1}, {0, 0});
      Presentation<W> q;
      presentation::add_rule(q, {4, 1}, {0, 5});
      presentation::add_rule(q, {4, 1}, {0, 1, 1, 1, 1, 1, 1, 1, 1, 1});
      presentation::add_rules(p, q);
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
      presentation::add_rule(p, {0, 1, 2, 1}, {0, 0});
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
    void check_add_inverse_rules() {
      Presentation<W> p;
      presentation::add_rule(p, {0, 1, 2, 1}, {0, 0});
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
      presentation::add_rule(p, {0, 0}, {0, 1, 2, 1});
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

      presentation::add_rule(p, {1, 2, 1}, {1, 1, 2, 1});
      presentation::add_rule(p, {1, 1, 2, 1}, {1, 1});
      presentation::add_rule(p, {1, 1}, {1, 2, 1});
      presentation::add_rule(p, {1, 2, 1}, {0});
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

      presentation::add_rule(p, {1, 2, 1}, {1, 1, 2, 1});
      presentation::add_rule(p, {1, 1, 2, 1}, {1, 1});
      presentation::add_rule(p, {1, 1}, {1, 2, 1});
      presentation::add_rule(p, {1, 2, 1}, {0});
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
      presentation::add_rule(p, {1, 2, 1}, {1, 1, 2, 1});
      presentation::add_rule(p, {1, 1, 2, 1}, {1, 1});
      presentation::add_rule(p, {1, 1}, {1, 2, 1});
      presentation::add_rule(p, {1, 2, 1}, {0});
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
    }

    template <typename W>
    void check_longest_common_subword() {
      {
        // Normalized alphabet
        Presentation<W> p;
        p.rules.push_back(W({0, 1, 2, 1}));
        REQUIRE_NOTHROW(presentation::longest_common_subword(p));
        p.rules.push_back(W({1, 2, 1}));
        presentation::add_rule(p, {1, 2, 1}, {1, 1, 2, 1});
        presentation::add_rule(p, {1, 1, 2, 1}, {1, 1});
        presentation::add_rule(p, {1, 1}, {1, 2, 1});
        presentation::add_rule(p, {1, 2, 1}, {0});
        p.alphabet_from_rules();
        REQUIRE(presentation::longest_common_subword(p) == W({1, 2, 1}));
        presentation::replace_subword(p, W({1, 2, 1}));
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
        presentation::add_rule(p, {1, 2, 4, 2}, {2, 4, 2});
        presentation::add_rule(p, {2, 4, 2}, {2, 2, 4, 2});
        presentation::add_rule(p, {2, 2, 4, 2}, {2, 2});
        presentation::add_rule(p, {2, 2}, {2, 4, 2});
        presentation::add_rule(p, {2, 4, 2}, {1});
        p.alphabet_from_rules();
        REQUIRE(presentation::longest_common_subword(p) == W({2, 4, 2}));
        presentation::replace_subword(p, W({2, 4, 2}));
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

      auto p = make<Presentation<W>>(S);
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
      auto it = presentation::redundant_rule(p, std::chrono::milliseconds(100));
      REQUIRE(*it == W({2, 1, 3, 1, 1, 2, 1, 2}));
      REQUIRE(*(it + 1) == W({1, 1, 2, 1, 3, 1, 2, 1}));
      p.rules.erase(it, it + 2);
      p.validate();
      // while (it != p.rules.cend()) { // Too time consuming and indeterminant
      //   REQUIRE(std::distance(it, p.rules.cend()) % 2 == 0);
      //   p.rules.erase(it, it + 2);
      //   p.validate();
      //   it = presentation::redundant_rule(p, std::chrono::milliseconds(8));
      // }
      REQUIRE(presentation::length(p) == 343);
      REQUIRE(p.rules.size() == 84);
    }

    template <typename W>
    void check_shortlex_compare_concat() {
      REQUIRE(detail::shortlex_compare_concat(
          W({0, 1, 2, 1}), W({0}), W({1, 1, 2, 1}), W({0})));
    }

    template <typename W>
    void check_remove_trivial_rules() {
      Presentation<W> p;
      p.rules.push_back(W({0, 1, 2, 1}));
      REQUIRE_THROWS_AS(presentation::remove_trivial_rules(p),
                        LibsemigroupsException);
      p.rules.push_back(W({1, 2, 1}));
      presentation::add_rule(p, {1, 2, 1}, {1, 2, 1});
      presentation::add_rule(p, {1, 1, 2, 1}, {1, 1});
      presentation::add_rule(p, {1, 2, 1}, {1, 2, 1});
      presentation::add_rule(p, {1, 2, 1}, {0});
      presentation::add_rule(p, {0}, {0});
      presentation::add_rule(p, {1}, {1});
      presentation::add_rule(p, {2}, {2});

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
      presentation::add_rule(p, {1, 1, 2, 1}, {1, 1});
      presentation::add_rule(p, {1, 2, 1}, {0});

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
      presentation::add_rule(
          p, {1, 2, 1, 2, 1, 1, 2, 1, 2, 1, 1, 2, 1}, {1, 2, 1, 1, 2, 1, 2, 1});
      presentation::replace_subword(p, W({1, 2, 1}), W({1}));
      REQUIRE(p.rules == std::vector<W>({{1, 2, 1, 1, 2, 1, 1}, {1, 1, 2, 1}}));
      presentation::replace_subword(p, W({1, 2, 1}), W({1}));
      REQUIRE(p.rules == std::vector<W>({{1, 1, 1}, {1, 1}}));
      // Test for when existing is a suffix of replacement
      p.rules.clear();
      presentation::add_rule(
          p, {1, 2, 1, 2, 1, 1, 2, 1, 2, 1, 1, 2, 1}, {1, 2, 1, 1, 2, 1, 2, 1});
      presentation::replace_subword(p, W({1, 2}), W({1, 1, 2}));
      REQUIRE(p.rules
              == std::vector<W>(
                  {{1, 1, 2, 1, 1, 2, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 2, 1},
                   {1, 1, 2, 1, 1, 1, 2, 1, 1, 2, 1}}));
    }

    template <typename W>
    void check_longest_rule() {
      Presentation<W> p;
      p.rules.push_back(W({0, 1, 2, 1}));
      REQUIRE_THROWS_AS(presentation::longest_rule(p), LibsemigroupsException);
      p.rules.push_back(W({1, 2, 1}));
      presentation::add_rule(p, {1, 1, 2, 1}, {1, 1});
      presentation::add_rule(p, {1, 2, 1}, {0});
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
      presentation::add_rule(p, {1, 1, 2, 1}, {1, 1});
      presentation::add_rule(p, {1, 2, 1}, {0});
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

      presentation::add_rule(p, {1, 1, 2, 1}, {1, 1});
      presentation::add_rule(p, {1, 2, 1}, {0});

      presentation::remove_redundant_generators(p);
      REQUIRE(p.rules
              == std::vector<W>(
                  {{1, 2, 1, 1, 2, 1}, {1, 2, 1}, {1, 1, 2, 1}, {1, 1}}));
      presentation::remove_redundant_generators(p);
      REQUIRE(p.rules
              == std::vector<W>(
                  {{1, 2, 1, 1, 2, 1}, {1, 2, 1}, {1, 1, 2, 1}, {1, 1}}));

      p.rules.clear();
      presentation::add_rule(p, {0, 1, 2, 1}, {1, 2, 1});
      presentation::add_rule(p, {1, 1, 2, 1}, {1, 1});
      presentation::add_rule(p, {1}, {0});
      presentation::add_rule(p, {1, 2, 1}, {0});
      presentation::remove_redundant_generators(p);
      REQUIRE(
          p.rules
          == std::vector<W>(
              {{0, 0, 2, 0}, {0, 2, 0}, {0, 0, 2, 0}, {0, 0}, {0, 2, 0}, {0}}));

      p.rules.clear();
      presentation::add_rule(p, {0, 1, 2, 1}, {1, 2, 1});
      presentation::add_rule(p, {1, 1, 2, 1}, {1, 1});
      presentation::add_rule(p, {0}, {1});
      presentation::add_rule(p, {1, 2, 1}, {0});
      presentation::remove_redundant_generators(p);
      REQUIRE(
          p.rules
          == std::vector<W>(
              {{0, 0, 2, 0}, {0, 2, 0}, {0, 0, 2, 0}, {0, 0}, {0, 2, 0}, {0}}));

      p.rules.clear();
      presentation::add_rule(p, {0, 1, 2, 1}, {1, 2, 1});
      presentation::add_rule(p, {1, 1, 2, 1}, {1, 1});
      presentation::add_rule(p, {1}, {0});
      presentation::add_rule(p, {1, 2, 1}, {0});
      presentation::remove_redundant_generators(p);
      REQUIRE(
          p.rules
          == std::vector<W>(
              {{0, 0, 2, 0}, {0, 2, 0}, {0, 0, 2, 0}, {0, 0}, {0, 2, 0}, {0}}));
    }

    template <typename W>
    void check_reverse() {
      Presentation<W> p;
      presentation::add_rule(p, {0, 1, 2, 1}, {1, 2, 1});
      presentation::add_rule(p, {1, 1, 2, 1}, {1, 1});
      presentation::add_rule(p, {1, 2, 1}, {0});

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
      presentation::add_rule(p, {0, 1, 2, 1}, {1, 2, 1});
      presentation::add_rule(p, {1, 1, 2, 1}, {1, 1});
      presentation::add_rule(p, {1, 2, 1}, {0});
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
    presentation::add_rule(p, {0, 0, 0}, {0});
    REQUIRE(std::distance(p.rules.cbegin(), p.rules.cend()) == 2);
    REQUIRE(std::vector<word_type>(p.rules.cbegin(), p.rules.cend())
            == std::vector<word_type>({{0, 0, 0}, {0}}));
    presentation::add_rule_and_check(p, {0, 0, 0}, {0});
    REQUIRE_THROWS_AS(presentation::add_rule_and_check(p, {0, 5, 0}, {0}),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(presentation::add_rule_and_check(p, {}, {0}),
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
    presentation::add_rule(p, "aaa", "a");
    REQUIRE(std::distance(p.rules.cbegin(), p.rules.cend()) == 2);
    REQUIRE(std::vector<std::string>(p.rules.cbegin(), p.rules.cend())
            == std::vector<std::string>({"aaa", "a"}));
    REQUIRE_THROWS_AS(presentation::add_rule_and_check(p, "abz", "a"),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(presentation::add_rule_and_check(p, "", "a"),
                      LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "002",
                          "constructors (word_type)",
                          "[quick][presentation]") {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.alphabet({0, 1, 2});
    presentation::add_rule(p, {0, 0, 0}, {0});
    REQUIRE(p.rules.size() == 2);
    presentation::add_rule_and_check(p, {0, 0, 0}, {0});
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
    presentation::add_rule(p, {0, 0, 0}, {0});
    REQUIRE(p.rules.size() == 2);
    presentation::add_rule_and_check(p, {0, 0, 0}, {0});
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
    presentation::add_rule(p, "aaaa", "aa");
    REQUIRE(p.rules.size() == 2);
    presentation::add_rule_and_check(p, "aaa", "aa");
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
    REQUIRE(p.letter(0) == 'a');
    REQUIRE(p.letter(1) == 'b');
    REQUIRE(p.letter(2) == 'c');
    p.alphabet(4);
    REQUIRE(p.alphabet().size() == 4);
    p.validate();
    REQUIRE_THROWS_AS(p.alphabet("abb"), LibsemigroupsException);

    presentation::add_rule(p, "abca", "aa");
    presentation::add_rule(p, "eb", "af");
    presentation::add_rule(p, "eb", "abbbbbb");
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
    presentation::add_rule(p, "abcb", "aa");
    Presentation<std::string> q;
    presentation::add_rule(q, "eb", "af");
    presentation::add_rule(q, "eb", "abbbbbbbbb");
    presentation::add_rules(p, q);
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
                          "012",
                          "helpers add_identity_rules (std::string)",
                          "[quick][presentation]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    presentation::add_rule(p, "abcb", "aa");
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
    presentation::add_rule_and_check(p, "aaCac", "e");
    presentation::add_rule_and_check(p, "acbbACb", "e");
    presentation::add_rule_and_check(p, "ABabccc", "e");
    REQUIRE(
        p.rules
        == std::vector<std::string>(
            {"ae", "a",  "ea",    "a",  "Ae",      "A",  "eA",      "A",  "be",
             "b",  "eb", "b",     "Be", "B",       "eB", "B",       "ce", "c",
             "ec", "c",  "Ce",    "C",  "eC",      "C",  "ee",      "e",  "aA",
             "e",  "Aa", "e",     "bB", "e",       "Bb", "e",       "cC", "e",
             "Cc", "e",  "aaCac", "e",  "acbbACb", "e",  "ABabccc", "e"}));
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
    presentation::add_rule(p, "abcb", "bcb");
    presentation::add_rule(p, "bcb", "bbcb");
    presentation::add_rule(p, "bbcb", "bb");
    presentation::add_rule(p, "bb", "bcb");
    presentation::add_rule(p, "bcb", "a");
    p.alphabet_from_rules();
    presentation::reduce_complements(p);
    presentation::sort_each_rule(p);
    presentation::sort_rules(p);
    REQUIRE(p.rules
            == std::vector<std::string>(
                {"bb", "a", "bcb", "a", "abcb", "a", "bbcb", "a"}));
    REQUIRE(p.alphabet() == "abc");
    presentation::normalize_alphabet(p);
    REQUIRE(p.letter(0) == 0);
    REQUIRE(p.letter(1) == 1);
    REQUIRE(p.letter(2) == 2);
    p.validate();

    Presentation<std::string> q;
    presentation::add_rule(p, "abcb", "bcb");
    // Not valid
    REQUIRE_THROWS_AS(presentation::normalize_alphabet(p),
                      LibsemigroupsException);
    p.alphabet_from_rules();
    presentation::add_rule(p, "abcd", "bcb");
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

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "019",
                          "helpers longest_common_subword/replace_subword",
                          "[quick][presentation]") {
    auto rg = ReportGuard(false);
    check_longest_common_subword<word_type>();
    check_longest_common_subword<StaticVector1<uint16_t, 10>>();
    check_longest_common_subword<std::string>();
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

  LIBSEMIGROUPS_TEST_CASE("Presentation",
                          "021",
                          "helpers shortlex_compare_concat",
                          "[quick][presentation]") {
    auto rg = ReportGuard(false);
    check_shortlex_compare_concat<word_type>();
    check_shortlex_compare_concat<StaticVector1<uint16_t, 10>>();
    check_shortlex_compare_concat<std::string>();
  }

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
    check_reverse<StaticVector1<uint16_t, 10>>();
    check_in_alphabet<std::string>();
  }

}  // namespace libsemigroups
