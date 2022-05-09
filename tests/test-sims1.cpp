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

#include <cstddef>        // for size_t
#include <stdexcept>      // for number_of_congruencestime_error
#include <unordered_set>  // for unordered_set
#include <vector>         // for vector

#include <iostream>

#include "catch.hpp"  // for REQUIRE, REQUIRE_THROWS_AS, REQUI...
#include "fpsemi-examples.hpp"
#include "test-main.hpp"  // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/bipart.hpp"
#include "libsemigroups/digraph-helper.hpp"
#include "libsemigroups/froidure-pin.hpp"
#include "libsemigroups/knuth-bendix.hpp"  // for redundant_rule
#include "libsemigroups/make-present.hpp"
#include "libsemigroups/sims1.hpp"  // for Sims1_
#include "libsemigroups/transf.hpp"
#include "libsemigroups/types.hpp"  // for word_type

namespace libsemigroups {

  using Sims1_       = Sims1<uint32_t>;
  using digraph_type = typename Sims1_::digraph_type;
  using node_type    = typename digraph_type::node_type;

  namespace {
    template <typename P>
    void check_extra(congruence_kind ck, P const &p, P const &e, size_t n) {
      P f = e;
      if (ck == congruence_kind::left) {
        presentation::reverse(f);
      }
      auto foo = [&f](auto const &ad) {
        using action_digraph_helper::follow_path_nc;
        for (auto it = f.rules.cbegin(); it != f.rules.cend(); it += 2) {
          if (follow_path_nc(ad, 0, *it) != follow_path_nc(ad, 0, *(it + 1))) {
            return false;
          }
        }
        return true;
      };
      Sims1_ S(ck, p);
      Sims1_ T(ck, p, e);

      REQUIRE(static_cast<uint64_t>(std::count_if(S.cbegin(n), S.cend(n), foo))
              == T.number_of_congruences(n));
    }
  }  // namespace

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "000",
                          "fp example 1",
                          "[quick][low-index]") {
    auto rg = ReportGuard(false);

    Presentation<word_type> p;
    p.contains_empty_word(true);
    p.alphabet({0, 1});
    presentation::add_rule_and_check(p, {0, 0, 0}, {0});
    presentation::add_rule_and_check(p, {1, 1}, {1});
    presentation::add_rule_and_check(p, {0, 1, 0, 1}, {0});

    {
      Sims1_ S(congruence_kind::right, p);
      REQUIRE(S.number_of_congruences(5) == 6);

      auto it = S.cbegin(5);
      REQUIRE(*(it++) == action_digraph_helper::make<node_type>(5, {{0, 0}}));
      REQUIRE(*(it++)
              == action_digraph_helper::make<node_type>(
                  5, {{1, 2}, {1, 1}, {3, 2}, {3, 3}}));
      REQUIRE(*(it++)
              == action_digraph_helper::make<node_type>(
                  5, {{1, 2}, {1, 1}, {2, 2}}));
      REQUIRE(*(it++)
              == action_digraph_helper::make<node_type>(
                  5, {{1, 2}, {1, 1}, {1, 2}}));
      REQUIRE(*(it++)
              == action_digraph_helper::make<node_type>(5, {{1, 1}, {1, 1}}));
      REQUIRE(*(it++)
              == action_digraph_helper::make<node_type>(5, {{1, 0}, {1, 1}}));
      REQUIRE(*(it++) == action_digraph_helper::make<node_type>(0, 2));
      REQUIRE(*(it++) == action_digraph_helper::make<node_type>(0, 2));

      it = S.cbegin(3);
      REQUIRE(*it == action_digraph_helper::make<node_type>(3, {{0, 0}}));
    }
    // [[[0, 0]],
    // [[1, 2], [1, 1], [3, 2], [3, 3]],
    // [[1, 2], [1, 1], [2, 2]],
    // [[1, 2], [1, 1], [1, 2]],
    // [[1, 1], [1, 1]],
    // [[1, 0], [1, 1]]]
    {
      Sims1_ S(congruence_kind::left, p);
      REQUIRE(S.number_of_congruences(5) == 9);
      for (auto it = S.cbegin(5); it != S.cend(5); ++it) {
        REQUIRE(action_digraph_helper::follow_path_nc(*it, 0, {1, 0, 1, 0})
                == action_digraph_helper::follow_path_nc(*it, 0, {0}));
      }
    }
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "001",
                          "fp example 2",
                          "[quick][low-index]") {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.contains_empty_word(true);

    p.alphabet({0, 1, 2});
    presentation::add_rule_and_check(p, {0, 1, 0}, {0, 0});
    presentation::add_rule_and_check(p, {2, 2}, {0, 0});
    presentation::add_rule_and_check(p, {0, 0, 0}, {0, 0});
    presentation::add_rule_and_check(p, {2, 1}, {1, 2});
    presentation::add_rule_and_check(p, {2, 0}, {0, 0});
    presentation::add_rule_and_check(p, {1, 1}, {1});
    presentation::add_rule_and_check(p, {0, 2}, {0, 0});

    {
      Sims1_ S(congruence_kind::right, p);
      REQUIRE(S.number_of_congruences(1) == 1);
      REQUIRE(S.number_of_congruences(2) == 3);
      REQUIRE(S.number_of_congruences(3) == 13);
      REQUIRE(S.number_of_congruences(4) == 36);
      REQUIRE(S.number_of_congruences(5) == 82);
      REQUIRE(S.number_of_congruences(6) == 135);
      REQUIRE(S.number_of_congruences(7) == 166);
      REQUIRE(S.number_of_congruences(8) == 175);
      REQUIRE(S.number_of_congruences(9) == 176);
      REQUIRE(S.number_of_congruences(10) == 176);

      auto it = S.cbegin(2);
      REQUIRE(*(it++)
              == action_digraph_helper::make<node_type>(2, {{0, 0, 0}}));
      REQUIRE(
          *(it++)
          == action_digraph_helper::make<node_type>(2, {{1, 1, 1}, {1, 1, 1}}));
      REQUIRE(
          *(it++)
          == action_digraph_helper::make<node_type>(2, {{1, 0, 1}, {1, 1, 1}}));
      REQUIRE(*(it++) == action_digraph_helper::make<node_type>(0, 3));
      REQUIRE(*(it++) == action_digraph_helper::make<node_type>(0, 3));
    }
    {
      Sims1_ S(congruence_kind::left, p);
      REQUIRE(S.number_of_congruences(11) == 176);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "002",
                          "ToddCoxeter failing example",
                          "[quick][low-index]") {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.contains_empty_word(false);

    //              a  A  b  B  c  C  e
    p.alphabet({0, 1, 2, 3, 4, 5, 6});
    presentation::add_identity_rules(p, 6);
    presentation::add_inverse_rules(p, {1, 0, 3, 2, 5, 4, 6}, 6);
    presentation::add_rule_and_check(p, {0, 0, 5, 0, 4}, {6});
    presentation::add_rule_and_check(p, {0, 4, 2, 2, 1, 5, 2}, {6});
    presentation::add_rule_and_check(p, {1, 3, 0, 2, 4, 4, 4}, {6});
    Sims1_ S(congruence_kind::right, p);
    REQUIRE(S.number_of_congruences(3) == 14);
    REQUIRE(S.number_of_congruences(4) == 14);
    REQUIRE(S.number_of_congruences(5) == 14);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "003",
                          "ToddCoxeter failing example",
                          "[quick][low-index]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.contains_empty_word(false);

    p.alphabet("aAbBcCe");
    presentation::add_identity_rules(p, 'e');

    presentation::add_inverse_rules(p, "AaBbCce", 'e');
    presentation::add_rule_and_check(p, "aaCac", "e");
    presentation::add_rule_and_check(p, "acbbACb", "e");
    presentation::add_rule_and_check(p, "ABabccc", "e");
    Sims1_ S(congruence_kind::right, p);
    REQUIRE(S.number_of_congruences(3) == 14);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "004",
                          "PartitionMonoid(2) right",
                          "[quick][low-index]") {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.contains_empty_word(false);

    p.alphabet({0, 1, 2, 3});
    presentation::add_identity_rules(p, 0);
    presentation::add_rule_and_check(p, {1, 1}, {0});
    presentation::add_rule_and_check(p, {1, 3}, {3});
    presentation::add_rule_and_check(p, {2, 2}, {2});
    presentation::add_rule_and_check(p, {3, 1}, {3});
    presentation::add_rule_and_check(p, {3, 3}, {3});
    presentation::add_rule_and_check(p, {2, 3, 2}, {2});
    presentation::add_rule_and_check(p, {3, 2, 3}, {3});
    presentation::add_rule_and_check(p, {1, 2, 1, 2}, {2, 1, 2});
    presentation::add_rule_and_check(p, {2, 1, 2, 1}, {2, 1, 2});

    Sims1_ S(congruence_kind::right, p);
    REQUIRE(S.number_of_congruences(2) == 4);
    REQUIRE(S.number_of_congruences(3) == 7);
    REQUIRE(S.number_of_congruences(4) == 14);
    REQUIRE(S.number_of_congruences(5) == 23);
    REQUIRE(S.number_of_congruences(6) == 36);
    REQUIRE(S.number_of_congruences(7) == 51);
    REQUIRE(S.number_of_congruences(8) == 62);
    REQUIRE(S.number_of_congruences(9) == 74);
    REQUIRE(S.number_of_congruences(10) == 86);
    REQUIRE(S.number_of_congruences(11) == 95);
    REQUIRE(S.number_of_congruences(12) == 100);
    REQUIRE(S.number_of_congruences(13) == 102);
    REQUIRE(S.number_of_congruences(14) == 104);
    REQUIRE(S.number_of_congruences(15) == 105);
    REQUIRE(S.number_of_congruences(16) == 105);
    REQUIRE(S.number_of_congruences(17) == 105);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "005",
                          "PartitionMonoid(3)",
                          "[quick][low-index]") {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.contains_empty_word(false);

    p.alphabet({0, 1, 2, 3, 4});
    presentation::add_rule_and_check(p, {0, 0}, {0});
    presentation::add_rule_and_check(p, {0, 1}, {1});
    presentation::add_rule_and_check(p, {0, 2}, {2});
    presentation::add_rule_and_check(p, {0, 3}, {3});
    presentation::add_rule_and_check(p, {0, 4}, {4});
    presentation::add_rule_and_check(p, {1, 0}, {1});
    presentation::add_rule_and_check(p, {2, 0}, {2});
    presentation::add_rule_and_check(p, {2, 2}, {0});
    presentation::add_rule_and_check(p, {2, 4}, {4});
    presentation::add_rule_and_check(p, {3, 0}, {3});
    presentation::add_rule_and_check(p, {3, 3}, {3});
    presentation::add_rule_and_check(p, {4, 0}, {4});
    presentation::add_rule_and_check(p, {4, 2}, {4});
    presentation::add_rule_and_check(p, {4, 4}, {4});
    presentation::add_rule_and_check(p, {1, 1, 1}, {0});
    presentation::add_rule_and_check(p, {1, 1, 2}, {2, 1});
    presentation::add_rule_and_check(p, {1, 2, 1}, {2});
    presentation::add_rule_and_check(p, {2, 1, 1}, {1, 2});
    presentation::add_rule_and_check(p, {2, 1, 2}, {1, 1});
    presentation::add_rule_and_check(p, {2, 1, 4}, {1, 1, 4});
    presentation::add_rule_and_check(p, {3, 1, 2}, {1, 2, 3});
    presentation::add_rule_and_check(p, {3, 4, 3}, {3});
    presentation::add_rule_and_check(p, {4, 1, 2}, {4, 1, 1});
    presentation::add_rule_and_check(p, {4, 3, 4}, {4});
    presentation::add_rule_and_check(p, {1, 1, 3, 1}, {2, 3, 2});
    presentation::add_rule_and_check(p, {1, 1, 3, 2}, {2, 3, 1});
    presentation::add_rule_and_check(p, {1, 2, 3, 1}, {3, 2});
    presentation::add_rule_and_check(p, {1, 2, 3, 2}, {3, 1});
    presentation::add_rule_and_check(p, {1, 2, 3, 4}, {3, 1, 4});
    presentation::add_rule_and_check(p, {1, 3, 2, 3}, {3, 1, 3});
    presentation::add_rule_and_check(p, {1, 4, 1, 4}, {4, 1, 4});
    presentation::add_rule_and_check(p, {2, 1, 3, 1}, {1, 3, 2});
    presentation::add_rule_and_check(p, {2, 1, 3, 2}, {1, 3, 1});
    presentation::add_rule_and_check(p, {2, 1, 3, 4}, {1, 3, 1, 4});
    presentation::add_rule_and_check(p, {2, 3, 1, 3}, {1, 3, 1, 3});
    presentation::add_rule_and_check(p, {2, 3, 1, 4}, {1, 1, 3, 4});
    presentation::add_rule_and_check(p, {2, 3, 2, 3}, {3, 2, 3});
    presentation::add_rule_and_check(p, {3, 1, 3, 2}, {3, 1, 3});
    presentation::add_rule_and_check(p, {3, 1, 4, 3}, {1, 2, 3});
    presentation::add_rule_and_check(p, {3, 2, 3, 2}, {3, 2, 3});
    presentation::add_rule_and_check(p, {4, 1, 1, 4}, {4, 1, 4});
    presentation::add_rule_and_check(p, {4, 1, 3, 2}, {4, 1, 3, 1});
    presentation::add_rule_and_check(p, {4, 1, 4, 1}, {4, 1, 4});
    presentation::add_rule_and_check(p, {1, 3, 1, 1, 3}, {3, 2, 1, 3});
    presentation::add_rule_and_check(p, {1, 3, 4, 1, 4}, {4, 1, 3, 4});
    presentation::add_rule_and_check(p, {2, 3, 1, 1, 3}, {3, 1, 1, 3});
    presentation::add_rule_and_check(p, {2, 3, 2, 1, 3}, {1, 3, 2, 1, 3});
    presentation::add_rule_and_check(p, {2, 3, 4, 1, 3}, {1, 3, 4, 1, 3});
    presentation::add_rule_and_check(p, {2, 3, 4, 1, 4}, {1, 4, 1, 3, 4});
    presentation::add_rule_and_check(p, {3, 1, 1, 4, 1}, {1, 1, 4, 1, 3});
    presentation::add_rule_and_check(p, {3, 1, 3, 1, 1}, {3, 2, 1, 3});
    presentation::add_rule_and_check(p, {3, 2, 3, 1, 1}, {3, 1, 1, 3});
    presentation::add_rule_and_check(p, {3, 4, 1, 1, 3}, {1, 2, 3});
    presentation::add_rule_and_check(p, {3, 4, 1, 4, 3}, {1, 1, 4, 1, 3});
    presentation::add_rule_and_check(p, {4, 1, 1, 3, 4}, {4, 3, 1, 4});
    presentation::add_rule_and_check(p, {4, 1, 3, 1, 1}, {1, 3, 1, 1, 4});
    presentation::add_rule_and_check(p, {4, 1, 3, 1, 3}, {4, 3, 1, 3});
    presentation::add_rule_and_check(p, {4, 1, 3, 1, 4}, {4, 1, 3, 4});
    presentation::add_rule_and_check(p, {4, 1, 3, 4, 1}, {4, 1, 3, 4});
    presentation::add_rule_and_check(p, {4, 1, 4, 3, 2}, {4, 1, 4, 3, 1});
    presentation::add_rule_and_check(p, {1, 1, 3, 4, 1, 3}, {3, 1, 4, 1, 3});
    presentation::add_rule_and_check(p, {1, 1, 4, 1, 3, 4}, {3, 4, 1, 4});
    presentation::add_rule_and_check(p, {1, 3, 1, 1, 4, 3}, {4, 3, 2, 1, 3});
    presentation::add_rule_and_check(p, {1, 3, 1, 3, 1, 3}, {3, 1, 3, 1, 3});
    presentation::add_rule_and_check(p, {1, 3, 1, 4, 1, 3}, {3, 4, 1, 3});
    presentation::add_rule_and_check(p, {1, 4, 3, 1, 1, 4}, {4, 3, 1, 1, 4});
    presentation::add_rule_and_check(p, {2, 3, 1, 1, 4, 3}, {1, 4, 3, 2, 1, 3});
    presentation::add_rule_and_check(p, {3, 1, 1, 3, 4, 1}, {3, 1, 4, 1, 3});
    presentation::add_rule_and_check(p, {3, 1, 1, 4, 3, 1}, {1, 1, 4, 3, 1, 3});
    presentation::add_rule_and_check(p, {3, 1, 3, 1, 3, 1}, {3, 1, 3, 1, 3});
    presentation::add_rule_and_check(p, {3, 1, 3, 1, 4, 1}, {3, 4, 1, 3});
    presentation::add_rule_and_check(p, {3, 1, 4, 1, 1, 3}, {3});
    presentation::add_rule_and_check(p, {4, 1, 4, 3, 1, 1}, {4, 3, 1, 1, 4});
    presentation::add_rule_and_check(p, {4, 1, 4, 3, 1, 4}, {4, 1, 4});
    presentation::add_rule_and_check(p, {4, 3, 1, 3, 1, 4}, {1, 3, 1, 1, 4});
    presentation::add_rule_and_check(
        p, {1, 1, 4, 3, 1, 3, 1}, {3, 1, 1, 4, 3, 2});
    presentation::add_rule_and_check(p, {1, 1, 4, 3, 2, 1, 3}, {3, 1, 1, 4, 3});
    presentation::add_rule_and_check(
        p, {1, 3, 1, 3, 4, 1, 3}, {3, 1, 3, 4, 1, 3});
    presentation::add_rule_and_check(p, {3, 1, 1, 4, 3, 2, 1}, {3, 1, 1, 4, 3});
    presentation::add_rule_and_check(
        p, {3, 1, 3, 1, 3, 4, 1}, {3, 1, 3, 4, 1, 3});
    presentation::add_rule_and_check(
        p, {4, 3, 1, 1, 4, 3, 2}, {4, 1, 4, 3, 1, 3, 1});
    presentation::add_rule_and_check(
        p, {3, 1, 1, 4, 3, 2, 3, 1}, {3, 1, 1, 4, 3, 2, 3});
    presentation::add_rule_and_check(
        p, {3, 1, 1, 4, 3, 2, 3, 4, 1}, {1, 1, 4, 3, 1, 3, 4, 1, 3});

    Sims1_ S(congruence_kind::right, p);
    S.split_at(43);
    auto     it = S.cbegin(10);
    uint64_t n  = 0;
    for (; it != S.cend(10); ++it) {
      n++;
    }
    REQUIRE(n == 135);
#ifdef LIBSEMIGgROUPS_ENABLE_STATS
    std::ostringstream oss;
    oss << it.stats();
#endif
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "006",
                          "FullTransformationMonoid(3) right",
                          "[quick][low-index]") {
    auto                   rg = ReportGuard(false);
    FroidurePin<Transf<3>> S({Transf<3>::make({1, 2, 0}),
                              Transf<3>::make({1, 0, 2}),
                              Transf<3>::make({0, 1, 0})});
    REQUIRE(S.size() == 27);
    REQUIRE(S.number_of_generators() == 3);
    REQUIRE(S.number_of_rules() == 16);
    auto p = make<Presentation<word_type>>(S);
    REQUIRE(static_cast<size_t>(std::distance(p.rules.cbegin(), p.rules.cend()))
            == 2 * S.number_of_rules());
    Sims1_ C(congruence_kind::right, p);
    REQUIRE(C.number_of_congruences(27) == 287);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "007",
                          "FullTransformationMonoid(3) left",
                          "[quick][low-index]") {
    auto                   rg = ReportGuard(false);
    FroidurePin<Transf<3>> S(
        {Transf<3>({1, 2, 0}), Transf<3>({1, 0, 2}), Transf<3>({0, 1, 0})});
    REQUIRE(S.size() == 27);
    auto   p = make<Presentation<word_type>>(S);
    Sims1_ C(congruence_kind::left, p);
    REQUIRE(C.number_of_congruences(28) == 120);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "008",
                          "FullTransformationMonoid(4) left",
                          "[fail][low-index]") {
    auto                   rg = ReportGuard(true);
    FroidurePin<Transf<4>> S({Transf<4>({1, 2, 3, 0}),
                              Transf<4>({1, 0, 2, 3}),
                              Transf<4>({0, 1, 0, 3})});
    REQUIRE(S.size() == 256);
    auto   p = make<Presentation<word_type>>(S);
    Sims1_ C(congruence_kind::left, p);
    REQUIRE(C.number_of_congruences(256) == 120);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "009",
                          "RookMonoid(2, 1)",
                          "[quick][low-index]") {
    auto                    rg = ReportGuard(true);
    Presentation<word_type> p;
    p.contains_empty_word(false);

    p.alphabet(3);
    for (auto const &rel : RookMonoid(2, 1)) {
      p.add_rule_and_check(rel.first.cbegin(),
                           rel.first.cend(),
                           rel.second.cbegin(),
                           rel.second.cend());
    }
    Sims1_ C(congruence_kind::right, p);
    REQUIRE(C.number_of_congruences(7) == 10);  // Should be 10
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "010",
                          "SymmetricInverseMonoid(2) from FroidurePin",
                          "[quick][low-index]") {
    auto                  rg = ReportGuard(false);
    FroidurePin<PPerm<2>> S({PPerm<2>({1, 0}), PPerm<2>({0}, {0}, 2)});
    REQUIRE(S.size() == 7);
    auto   p = make<Presentation<word_type>>(S);
    Sims1_ C(congruence_kind::left, p);
    REQUIRE(C.number_of_congruences(7) == 10);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "011",
                          "SymmetricInverseMonoid(3)",
                          "[quick][low-index]") {
    auto                    rg = ReportGuard(true);
    Presentation<word_type> p;
    p.contains_empty_word(false);

    p.alphabet(4);
    for (auto const &rel : RookMonoid(3, 1)) {
      p.add_rule_and_check(rel.first.cbegin(),
                           rel.first.cend(),
                           rel.second.cbegin(),
                           rel.second.cend());
    }
    Sims1_ C(congruence_kind::left, p);
    REQUIRE(C.number_of_congruences(34) == 274);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "012",
                          "SymmetricInverseMonoid(4)",
                          "[extreme][low-index]") {
    auto                    rg = ReportGuard(true);
    Presentation<word_type> p;
    p.contains_empty_word(false);

    p.alphabet(5);
    for (auto const &rel : RookMonoid(4, 1)) {
      p.add_rule_and_check(rel.first.cbegin(),
                           rel.first.cend(),
                           rel.second.cbegin(),
                           rel.second.cend());
    }
    presentation::remove_duplicate_rules(p);
    presentation::sort_each_rule(p);
    presentation::sort_rules(p);
    Sims1_ C(congruence_kind::right, p);

    REQUIRE(C.number_of_congruences(209) == 195'709);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "013",
                          "SymmetricInverseMonoid(5)",
                          "[fail][low-index]") {
    auto                    rg = ReportGuard(true);
    Presentation<word_type> p;
    p.contains_empty_word(false);

    p.alphabet(6);
    for (auto const &rel : RookMonoid(5, 1)) {
      p.add_rule_and_check(rel.first.cbegin(),
                           rel.first.cend(),
                           rel.second.cbegin(),
                           rel.second.cend());
    }
    Sims1_ C(congruence_kind::left, p);
    REQUIRE(C.number_of_congruences(1'546) == 195'709);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "014",
                          "TemperleyLieb(3) from presentation",
                          "[quick][low-index]") {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.contains_empty_word(true);

    p.alphabet(2);
    for (auto const &rel : TemperleyLieb(3)) {
      p.add_rule_and_check(rel.first.cbegin(),
                           rel.first.cend(),
                           rel.second.cbegin(),
                           rel.second.cend());
    }
    {
      Sims1_ S(congruence_kind::right, p);
      REQUIRE(S.number_of_congruences(14) == 9);
    }
    {
      Sims1_ S(congruence_kind::left, p);
      REQUIRE(S.number_of_congruences(14) == 9);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "015",
                          "TemperleyLieb(4) from presentation",
                          "[quick][low-index]") {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.contains_empty_word(true);

    p.alphabet(3);
    for (auto const &rel : TemperleyLieb(4)) {
      p.add_rule_and_check(rel.first.cbegin(),
                           rel.first.cend(),
                           rel.second.cbegin(),
                           rel.second.cend());
    }
    {
      Sims1_ S(congruence_kind::right, p);
      REQUIRE(S.number_of_congruences(14) == 79);
    }
    {
      Sims1_ S(congruence_kind::left, p);
      REQUIRE(S.number_of_congruences(14) == 79);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "016",
                          "fp semigroup containing given pairs #1",
                          "[quick][low-index]") {
    auto rg = ReportGuard(false);

    Presentation<word_type> p;
    p.contains_empty_word(true);

    p.alphabet({0, 1});
    presentation::add_rule_and_check(p, {0, 0, 0}, {0});
    presentation::add_rule_and_check(p, {1, 1}, {1});
    presentation::add_rule_and_check(p, {0, 1, 0, 1}, {0});
    Presentation<word_type> e;
    e.contains_empty_word(true);

    e.alphabet({0, 1});
    presentation::add_rule_and_check(e, {0}, {1});
    Sims1_ S(congruence_kind::right, p, e);
    REQUIRE(S.number_of_congruences(5) == 2);
    check_extra(congruence_kind::right, p, e, 5);
    check_extra(congruence_kind::left, p, e, 5);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "017",
                          "fp semigroup containing given pairs #2",
                          "[quick][low-index]") {
    auto rg = ReportGuard(false);

    Presentation<word_type> p;
    p.contains_empty_word(true);

    p.alphabet({0, 1});
    presentation::add_rule_and_check(p, {0, 0, 0}, {0});
    presentation::add_rule_and_check(p, {1, 1}, {1});
    presentation::add_rule_and_check(p, {0, 1, 0, 1}, {0});
    Presentation<word_type> e;
    e.contains_empty_word(true);

    e.alphabet({0, 1});
    presentation::add_rule_and_check(e, {0, 1}, {1});
    Sims1_ T(congruence_kind::right, p, e);
    REQUIRE(T.number_of_congruences(5) == 2);
    check_extra(congruence_kind::right, p, e, 5);
    check_extra(congruence_kind::left, p, e, 5);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "018",
                          "fp semigroup containing given pairs #3",
                          "[quick][low-index]") {
    auto rg = ReportGuard(false);

    Presentation<word_type> p;
    p.contains_empty_word(true);

    p.alphabet({0, 1});
    presentation::add_rule_and_check(p, {0, 0, 0}, {0});
    presentation::add_rule_and_check(p, {1, 1}, {1});
    presentation::add_rule_and_check(p, {0, 1, 0, 1}, {0});
    Presentation<word_type> e;
    e.contains_empty_word(true);

    e.alphabet({0, 1});
    presentation::add_rule_and_check(e, {0, 1, 0, 1}, {0});
    {
      Sims1_ T(congruence_kind::right, p, e);
      REQUIRE(T.number_of_congruences(5) == 6);
    }
    {
      Sims1_ T(congruence_kind::left, p, e);
      REQUIRE(T.number_of_congruences(5) == 9);  // Verified with GAP
    }
    check_extra(congruence_kind::right, p, e, 5);
    check_extra(congruence_kind::left, p, e, 5);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "019",
                          "ToddCoxeter failing example",
                          "[quick][low-index]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.contains_empty_word(false);

    p.alphabet("aAbBcCe");
    presentation::add_identity_rules(p, 'e');

    presentation::add_inverse_rules(p, "AaBbCce", 'e');
    presentation::add_rule_and_check(p, "aaCac", "e");
    presentation::add_rule_and_check(p, "acbbACb", "e");
    presentation::add_rule_and_check(p, "ABabccc", "e");

    Presentation<std::string> e;
    e.alphabet(p.alphabet());
    presentation::add_rule_and_check(p, "a", "A");
    presentation::add_rule_and_check(p, "a", "b");

    Sims1_ S(congruence_kind::right, p, e);
    REQUIRE(S.number_of_congruences(3) == 2);

    check_extra(congruence_kind::right, S.presentation(), S.extra(), 3);
    check_extra(congruence_kind::left, S.presentation(), S.extra(), 3);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "020",
                          "fp example 2",
                          "[quick][low-index]") {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.contains_empty_word(true);

    p.alphabet({0, 1, 2});
    presentation::add_rule_and_check(p, {0, 1, 0}, {0, 0});
    presentation::add_rule_and_check(p, {2, 2}, {0, 0});
    presentation::add_rule_and_check(p, {0, 0, 0}, {0, 0});
    presentation::add_rule_and_check(p, {2, 1}, {1, 2});
    presentation::add_rule_and_check(p, {2, 0}, {0, 0});
    presentation::add_rule_and_check(p, {1, 1}, {1});
    presentation::add_rule_and_check(p, {0, 2}, {0, 0});

    Presentation<word_type> e;
    e.alphabet(p.alphabet());
    presentation::add_rule_and_check(e, {1}, {0, 0});
    check_extra(congruence_kind::right, p, e, 11);
    check_extra(congruence_kind::left, p, e, 11);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1", "021", "exceptions", "[quick][low-index]") {
    Presentation<word_type> p;
    p.alphabet({0, 1, 2});
    presentation::add_rule_and_check(p, {0, 1, 0}, {0, 0});

    Presentation<word_type> e;
    e.alphabet({0, 1});
    REQUIRE_THROWS_AS(Sims1_(congruence_kind::twosided, p, e),
                      LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "022",
                          "SingularBrauer(4) (Maltcev-Mazorchuk)",
                          "[extreme][sims1]") {
    auto rg = ReportGuard(true);
    auto p  = make<Presentation<word_type>>(SingularBrauer(4));
    REQUIRE(presentation::length(p) == 660);
    presentation::remove_duplicate_rules(p);
    REQUIRE(presentation::length(p) == 600);
    presentation::sort_each_rule(p);
    presentation::sort_rules(p);
    REQUIRE(p.rules.size() == 252);

    Sims1_ C(congruence_kind::right, p);
    C.split_at(24);
    REQUIRE(C.number_of_congruences(81) == 601'265);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "023",
                          "Brauer(4) from FroidurePin",
                          "[extreme][sims1]") {
    auto                     rg = ReportGuard(true);
    FroidurePin<Bipartition> S;
    S.add_generator(Bipartition({{1, -1}, {2, -2}, {3, -3}, {4, -4}}));
    S.add_generator(Bipartition({{1, -2}, {2, -3}, {3, -4}, {4, -1}}));
    S.add_generator(Bipartition({{1, -2}, {2, -1}, {3, -3}, {4, -4}}));
    S.add_generator(Bipartition({{1, 2}, {3, -3}, {4, -4}, {-1, -2}}));
    REQUIRE(S.size() == 105);

    auto p = make<Presentation<word_type>>(S);
    REQUIRE(presentation::length(p) == 359);
    presentation::remove_duplicate_rules(p);
    REQUIRE(presentation::length(p) == 359);
    presentation::reduce_complements(p);
    REQUIRE(presentation::length(p) == 359);
    presentation::sort_each_rule(p);
    presentation::sort_rules(p);
    REQUIRE(p.rules.size() == 86);
    do {
      auto it = presentation::redundant_rule(p, std::chrono::milliseconds(100));
      p.rules.erase(it, it + 1);
    } while (presentation::length(p) > 300);
    // REQUIRE(word_type(fl.first, fl.second) == word_type({1, 1}));
    presentation::replace_subword(p, presentation::longest_common_subword(p));
    // REQUIRE(p.rules.size() == 24);
    // REQUIRE(presentation::length(p) == 69);

    Sims1_ C(congruence_kind::right, p);
    // C.split_at(4);
    REQUIRE(C.number_of_congruences(105) == 103'406);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "024",
                          "Brauer(4) (Kudryavtseva-Mazorchuk)",
                          "[extreme][sims1]") {
    auto rg = ReportGuard(true);
    auto p  = make<Presentation<word_type>>(Brauer(4));
    REQUIRE(presentation::length(p) == 182);
    presentation::remove_duplicate_rules(p);
    REQUIRE(presentation::length(p) == 162);
    presentation::reduce_complements(p);
    REQUIRE(presentation::length(p) == 159);
    // presentation::sort_each_rule(p);
    // presentation::sort_rules(p);
    REQUIRE(p.rules.size() == 86);

    // auto fl = presentation::longest_common_subword(p);
    // REQUIRE(word_type(fl.first, fl.second) == word_type({6, 5}));
    // presentation::replace_subword(p, fl.first, fl.second);
    // fl = presentation::longest_common_subword(p);
    // REQUIRE(word_type(fl.first, fl.second) == word_type({5, 4}));
    // presentation::replace_subword(p, fl.first, fl.second);
    // fl = presentation::longest_common_subword(p);
    // REQUIRE(word_type(fl.first, fl.second) == word_type({}));
    // size_t last_len;
    // do {
    //   last_len = presentation::length(p);
    //   presentation::remove_longest_redundant_rule(p);
    // } while (presentation::length(p) != last_len);

    Sims1_ C(congruence_kind::right, p);
    REQUIRE(C.number_of_congruences(105) == 103'406);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "025",
                          "UniformBlockBijection(4) (Fitzgerald)",
                          "[extreme][sims1]") {
    auto rg = ReportGuard(true);
    auto p  = make<Presentation<word_type>>(UniformBlockBijectionMonoidF(4));
    presentation::remove_duplicate_rules(p);
    presentation::reduce_complements(p);
    presentation::sort_each_rule(p);
    presentation::sort_rules(p);

    Sims1_ C(congruence_kind::right, p);
    REQUIRE(C.number_of_congruences(131) == 103'406);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "026",
                          "from https://mathoverflow.net/questions/423541/",
                          "[quick][sims1]") {
    auto                      rg = ReportGuard(true);
    Presentation<std::string> p;
    p.contains_empty_word(false);
    p.alphabet("aAbBe");
    presentation::add_identity_rules(p, 'e');
    presentation::add_inverse_rules(p, "AaBbe", 'e');
    presentation::add_rule_and_check(p, "aaa", "e");
    presentation::add_rule_and_check(p, "baBBBABA", "e");
    Sims1_ C(congruence_kind::right, p);
    REQUIRE(C.number_of_congruences(10) == 3);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "027",
                          "from https://mathoverflow.net/questions/423541/",
                          "[quick][sims1]") {
    auto                      rg = ReportGuard(true);
    Presentation<std::string> p;
    p.contains_empty_word(true);
    p.alphabet("aAbB");
    presentation::add_inverse_rules(p, "AaBb");
    presentation::add_rule_and_check(p, "aaa", "");
    presentation::add_rule_and_check(p, "baBBBABA", "");
    Sims1_ C(congruence_kind::right, p);
    REQUIRE(C.number_of_congruences(10) == 3);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "028",
                          "Fibonacci(4, 6)",
                          "[quick][sims1][no-valgrind]") {
    auto rg = ReportGuard(false);
    auto p  = make<Presentation<word_type>>(Fibonacci(4, 6));
    presentation::remove_duplicate_rules(p);
    presentation::sort_each_rule(p);
    presentation::sort_rules(p);
    REQUIRE(presentation::length(p) == 30);

    Sims1_ C(congruence_kind::right, p);
    REQUIRE(C.number_of_congruences(3) == 5);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "029",
                          "presentation with one free generator",
                          "[quick][sims1]") {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.alphabet(4);
    presentation::add_rule(p, {1, 2, 1}, {1, 1});
    presentation::add_rule(p, {3, 3}, {1, 1});
    presentation::add_rule(p, {1, 1, 1}, {1, 1});
    presentation::add_rule(p, {3, 2}, {2, 3});
    presentation::add_rule(p, {3, 1}, {1, 1});
    presentation::add_rule(p, {2, 2}, {2});
    presentation::add_rule(p, {1, 3}, {1, 1});
    p.validate();
    Sims1_ C(congruence_kind::right, p);
    REQUIRE(C.number_of_congruences(2) == 67);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "030",
                          "presentation with non-zero index generators",
                          "[quick][sims1]") {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    presentation::add_rule(p, {1, 2, 1}, {1, 1});
    presentation::add_rule(p, {3, 3}, {1, 1});
    presentation::add_rule(p, {1, 1, 1}, {1, 1});
    presentation::add_rule(p, {3, 2}, {2, 3});
    presentation::add_rule(p, {3, 1}, {1, 1});
    presentation::add_rule(p, {2, 2}, {2});
    presentation::add_rule(p, {1, 3}, {1, 1});
    p.alphabet_from_rules();
    p.validate();

    Sims1_ C(congruence_kind::right, p);
    REQUIRE(C.number_of_congruences(2) == 7);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "031",
                          "presentation with empty word",
                          "[quick][sims1]") {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.contains_empty_word(true);

    //          a  A  b  B  c  C
    p.alphabet({0, 1, 2, 3, 4, 5});
    presentation::add_inverse_rules(p, {1, 0, 3, 2, 5, 4});
    presentation::add_rule_and_check(p, {0, 0, 5, 0, 4}, {});
    presentation::add_rule_and_check(p, {0, 4, 2, 2, 1, 5, 2}, {});
    presentation::add_rule_and_check(p, {1, 3, 0, 2, 4, 4, 4}, {});
    Sims1_ S(congruence_kind::right, p);
    REQUIRE(S.number_of_congruences(3) == 14);
    REQUIRE(S.number_of_congruences(4) == 14);
    REQUIRE(S.number_of_congruences(5) == 14);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1", "032", "constructors", "[quick][sims1]") {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.contains_empty_word(true);

    //          a  A  b  B  c  C
    p.alphabet({0, 1, 2, 3, 4, 5});
    presentation::add_inverse_rules(p, {1, 0, 3, 2, 5, 4});
    presentation::add_rule_and_check(p, {0, 0, 5, 0, 4}, {});
    presentation::add_rule_and_check(p, {0, 4, 2, 2, 1, 5, 2}, {});
    presentation::add_rule_and_check(p, {1, 3, 0, 2, 4, 4, 4}, {});
    Sims1_ S(congruence_kind::right, p);

    Sims1_ T(S);
    REQUIRE(S.number_of_congruences(3) == 14);
    REQUIRE(T.number_of_congruences(3) == 14);

    Sims1_ U(std::move(S));
    REQUIRE(U.number_of_congruences(3) == 14);
    REQUIRE(T.number_of_congruences(3) == 14);

    S = U;
    REQUIRE(S.number_of_congruences(3) == 14);

    S = std::move(U);
    REQUIRE(S.number_of_congruences(3) == 14);

    Presentation<word_type> e;
    e.alphabet({0, 1, 2, 5});

    REQUIRE_THROWS_AS(Sims1_(congruence_kind::right, p, e),
                      LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1", "033", "split_at", "[quick][sims1]") {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.contains_empty_word(true);

    //          a  A  b  B  c  C
    p.alphabet({0, 1, 2, 3, 4, 5});
    presentation::add_inverse_rules(p, {1, 0, 3, 2, 5, 4});
    presentation::add_rule_and_check(p, {0, 0, 5, 0, 4}, {});
    presentation::add_rule_and_check(p, {0, 4, 2, 2, 1, 5, 2}, {});
    presentation::add_rule_and_check(p, {1, 3, 0, 2, 4, 4, 4}, {});
    Sims1_ S(congruence_kind::right, p);

    REQUIRE_THROWS_AS(S.split_at(10), LibsemigroupsException);
    S.split_at(0);

    REQUIRE(S.presentation().rules.empty());

    for (size_t i = 0; i <= p.rules.size() / 2; ++i) {
      S.split_at(i);
      REQUIRE(S.presentation().rules.size() == 2 * i);
    }
    REQUIRE(S.presentation().rules.size() == p.rules.size());
    for (size_t i = p.rules.size() / 2; i > 0; --i) {
      S.split_at(i);
      REQUIRE(S.presentation().rules.size() == 2 * i);
    }
    S.split_at(7);
    REQUIRE(S.number_of_congruences(3) == 14);
  }

#ifdef LIBSEMIGROUPS_ENABLE_STATS
  LIBSEMIGROUPS_TEST_CASE("Sims1", "034", "stats", "[quick][sims1]") {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.contains_empty_word(true);

    //          a  A  b  B  c  C
    p.alphabet({0, 1, 2, 3, 4, 5});
    presentation::add_inverse_rules(p, {1, 0, 3, 2, 5, 4});
    presentation::add_rule_and_check(p, {0, 0, 5, 0, 4}, {});
    presentation::add_rule_and_check(p, {0, 4, 2, 2, 1, 5, 2}, {});
    presentation::add_rule_and_check(p, {1, 3, 0, 2, 4, 4, 4}, {});
    Sims1_ S(congruence_kind::right, p);

    std::stringbuf buff;
    std::ostream   os(&buff);
    os << S.cbegin(3).stats();  // Also does not do anything visible
  }
#endif

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "035",
                          "check iterator requirements",
                          "[quick][sims1]") {
    Presentation<word_type> p;
    p.contains_empty_word(true);
    p.alphabet({0, 1});
    presentation::add_rule_and_check(p, {0, 0, 0}, {0});
    presentation::add_rule_and_check(p, {1, 1}, {1});
    presentation::add_rule_and_check(p, {0, 1, 0, 1}, {0});

    {
      Sims1_ S(congruence_kind::right, p);
      verify_forward_iterator_requirements(S.cbegin(10));
      auto it = S.cbegin(10);
      REQUIRE(it->number_of_nodes() == 10);
    }
    {
      Sims1_ S(congruence_kind::left, p);
      verify_forward_iterator_requirements(S.cbegin(10));
      auto it = S.cbegin(10);
      REQUIRE(it->number_of_nodes() == 10);
    }
  }
}  // namespace libsemigroups

// [[[0, 0, 0]],            #1#
// [[0, 0, 1], [1, 0, 1]],  #2#
// [[0, 1, 0], [1, 1, 0]],
// [[0, 1, 1], [1, 1, 1]]]  #3#

// [[[0, 0, 0]],                        #1#
//   [[1, 0, 1], [1, 1, 1]],
//   [[1, 0, 2], [1, 1, 1], [1, 2, 1]],
//   [[1, 1, 1], [1, 1, 1]],            #2#
//   [[1, 1, 2], [1, 1, 1], [1, 1, 1]], #4#
//   [[1, 0, 1], [2, 1, 2], [2, 2, 2]],
//   [[1, 0, 2], [2, 1, 2], [2, 2, 2]],
//   [[1, 1, 2], [2, 1, 2], [2, 2, 2]], #3#
//   [[1, 2, 2], [2, 1, 2], [2, 2, 2]],
//   [[1, 0, 2], [2, 2, 2], [2, 2, 2]],
//   [[1, 2, 1], [2, 2, 2], [2, 2, 2]],
//   [[1, 2, 2], [2, 2, 2], [2, 2, 2]],
//   [[1, 2, 1], [1, 1, 1], [1, 2, 1]]]
