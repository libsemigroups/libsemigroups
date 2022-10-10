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
#include <iostream>       // for cout
#include <stdexcept>      // for number_of_congruencestime_error
#include <unordered_set>  // for unordered_set
#include <vector>         // for vector

#include "catch.hpp"            // for REQUIRE, REQUIRE_THROWS_AS, REQUI...
#include "fpsemi-examples.hpp"  // for Brauer etc
#include "test-main.hpp"        // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/bipart.hpp"          // for Bipartition
#include "libsemigroups/config.hpp"          // for LIBSEMIGROUPS_ENABLE_STATS
#include "libsemigroups/digraph-helper.hpp"  // for action_digraph_helper
#include "libsemigroups/froidure-pin.hpp"    // for FroidurePin
#include "libsemigroups/knuth-bendix.hpp"    // for redundant_rule
#include "libsemigroups/make-froidure-pin.hpp"  // for make
#include "libsemigroups/make-present.hpp"       // for make
#include "libsemigroups/sims1.hpp"              // for Sims1
#include "libsemigroups/transf.hpp"             // for Transf
#include "libsemigroups/types.hpp"              // for word_type
                                                //
namespace libsemigroups {

  using Sims1_       = Sims1<uint32_t>;
  using digraph_type = typename Sims1_::digraph_type;
  using node_type    = typename digraph_type::node_type;

  namespace {
    template <typename P>
    void check_extra(congruence_kind ck, P const& p, P const& e, size_t n) {
      P f = e;
      if (ck == congruence_kind::left) {
        presentation::reverse(f);
      }
      auto foo = [&f](auto const& ad) {
        using action_digraph_helper::follow_path_nc;
        for (auto it = f.rules.cbegin(); it != f.rules.cend(); it += 2) {
          if (follow_path_nc(ad, 0, *it) != follow_path_nc(ad, 0, *(it + 1))) {
            return false;
          }
        }
        return true;
      };
      Sims1_ S(ck);
      S.short_rules(p);

      Sims1_ T(ck);
      T.short_rules(p).extra(e);

      REQUIRE(static_cast<uint64_t>(std::count_if(S.cbegin(n), S.cend(n), foo))
              == T.number_of_congruences(n));
    }

    constexpr unsigned int factorial(unsigned int n) {
      return n > 1 ? n * factorial(n - 1) : 1;
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
      Sims1_ S(congruence_kind::right);
      REQUIRE(S.short_rules(p).number_of_threads(2).number_of_congruences(5)
              == 6);
      REQUIRE_THROWS_AS(S.number_of_congruences(0), LibsemigroupsException);
      REQUIRE_THROWS_AS(S.find_if(0, [](auto) { return false; }),
                        LibsemigroupsException);
      REQUIRE_THROWS_AS(S.for_each(0, [](auto) { return false; }),
                        LibsemigroupsException);
      REQUIRE_THROWS_AS(S.cbegin(0), LibsemigroupsException);
      REQUIRE_THROWS_AS(S.cend(0), LibsemigroupsException);
      REQUIRE(S.number_of_congruences(1) == 1);

      auto it = S.cbegin(1);
      REQUIRE(*it == action_digraph_helper::make<node_type>(1, {{0, 0}}));

      it = S.cbegin(5);
      REQUIRE(*(it++) == action_digraph_helper::make<node_type>(5, {{0, 0}}));
      REQUIRE(*(it++)
              == action_digraph_helper::make<node_type>(5, {{1, 0}, {1, 1}}));
      REQUIRE(*(it++)
              == action_digraph_helper::make<node_type>(5, {{1, 1}, {1, 1}}));
      REQUIRE(*(it++)
              == action_digraph_helper::make<node_type>(
                  5, {{1, 2}, {1, 1}, {1, 2}}));
      REQUIRE(*(it++)
              == action_digraph_helper::make<node_type>(
                  5, {{1, 2}, {1, 1}, {2, 2}}));
      REQUIRE(*(it++)
              == action_digraph_helper::make<node_type>(
                  5, {{1, 2}, {1, 1}, {3, 2}, {3, 3}}));
      REQUIRE(*(it++) == ActionDigraph<node_type>(0, 2));
      REQUIRE(*(it++) == ActionDigraph<node_type>(0, 2));
      REQUIRE(*(it++) == ActionDigraph<node_type>(0, 2));

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
      Sims1_ S(congruence_kind::left);
      REQUIRE(S.short_rules(p).number_of_congruences(5) == 9);
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
      Sims1_ S(congruence_kind::right);
      S.short_rules(p);
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
          == action_digraph_helper::make<node_type>(2, {{1, 0, 1}, {1, 1, 1}}));
      REQUIRE(
          *(it++)
          == action_digraph_helper::make<node_type>(2, {{1, 1, 1}, {1, 1, 1}}));
      REQUIRE(*(it++) == ActionDigraph<node_type>(0, 3));
      REQUIRE(*(it++) == ActionDigraph<node_type>(0, 3));
    }
    {
      Sims1_ S(congruence_kind::left);
      S.short_rules(p);
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
    Sims1_ S(congruence_kind::right);
    S.short_rules(p);
    REQUIRE(S.number_of_congruences(1) == 1);
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
    Sims1_ S(congruence_kind::right);
    S.short_rules(p);
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

    Sims1_ S(congruence_kind::right);
    S.short_rules(p);
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
                          "[standard][low-index][no-coverage]") {
    auto rg = ReportGuard(false);
    auto p = make<Presentation<word_type>>(PartitionMonoid(3, author::Machine));
    REQUIRE(!p.contains_empty_word());
    REQUIRE(p.alphabet() == word_type({0, 1, 2, 3, 4}));

    Sims1_ S(congruence_kind::right);
    S.short_rules(p).long_rule_length(11).number_of_threads(
        4);  // This actually helps here!
    REQUIRE(S.number_of_congruences(17) == 1589);
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
    Sims1_ C(congruence_kind::right);
    C.short_rules(p);
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
    Sims1_ C(congruence_kind::left);
    C.short_rules(p);
    REQUIRE(C.number_of_congruences(27) == 120);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "008",
                          "FullTransformationMonoid(4) left",
                          "[fail][low-index][babbage]") {
    auto rg = ReportGuard(true);

    auto p = make<Presentation<word_type>>(
        FullTransformationMonoid(4, author::Iwahori));

    REQUIRE(p.alphabet().size() == 4);

    auto w = presentation::longest_common_subword(p);
    while (!w.empty()) {
      presentation::replace_subword(p, w);
      w = presentation::longest_common_subword(p);
    }

    presentation::sort_each_rule(p);
    presentation::sort_rules(p);
    presentation::remove_duplicate_rules(p);
    presentation::reduce_complements(p);
    presentation::remove_trivial_rules(p);

    do {
      auto it = presentation::redundant_rule(p, std::chrono::milliseconds(100));
      p.rules.erase(it, it + 2);
    } while (presentation::length(p) > 700);

    Sims1_ C(congruence_kind::right);
    C.short_rules(p);
    // Takes about 1h31m to run!
    REQUIRE(C.number_of_threads(6).number_of_congruences(256) == 22'069'828);
    // Sims1_ C(congruence_kind::left);
    // C.short_rules(p);
    // REQUIRE(C.number_of_threads(6).number_of_congruences(256) == 120'121);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "009",
                          "RookMonoid(2, 1)",
                          "[quick][low-index]") {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.contains_empty_word(false);

    p.alphabet(3);
    for (auto const& rel : RookMonoid(2, 1)) {
      p.add_rule_and_check(rel.first.cbegin(),
                           rel.first.cend(),
                           rel.second.cbegin(),
                           rel.second.cend());
    }
    Sims1_ C(congruence_kind::right);
    C.short_rules(p);
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
    Sims1_ C(congruence_kind::left);
    C.short_rules(p);
    REQUIRE(C.number_of_congruences(7) == 10);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "011",
                          "SymmetricInverseMonoid(3)",
                          "[quick][low-index]") {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.contains_empty_word(false);

    p.alphabet(4);
    for (auto const& rel : RookMonoid(3, 1)) {
      p.add_rule_and_check(rel.first.cbegin(),
                           rel.first.cend(),
                           rel.second.cbegin(),
                           rel.second.cend());
    }
    Sims1_ C(congruence_kind::left);
    C.short_rules(p);
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
    for (auto const& rel : RookMonoid(4, 1)) {
      p.add_rule_and_check(rel.first.cbegin(),
                           rel.first.cend(),
                           rel.second.cbegin(),
                           rel.second.cend());
    }
    presentation::remove_duplicate_rules(p);
    presentation::sort_each_rule(p);
    presentation::sort_rules(p);

    REQUIRE(presentation::length(p) == 78);
    REQUIRE(p.alphabet().size() == 5);
    REQUIRE(*presentation::shortest_rule(p) == word_type({0, 0}));
    REQUIRE(*(presentation::shortest_rule(p) + 1) == word_type({0}));
    REQUIRE(presentation::longest_rule_length(p) == 8);

    Sims1_ C(congruence_kind::right);
    C.short_rules(p);
    REQUIRE(C.number_of_threads(std::thread::hardware_concurrency())
                .number_of_congruences(209)
            == 195'709);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "013",
                          "SymmetricInverseMonoid(5)",
                          "[fail][low-index]") {
    // This might take an extremely long time to terminate
    auto                    rg = ReportGuard(true);
    Presentation<word_type> p;
    p.contains_empty_word(false);

    p.alphabet(6);
    for (auto const& rel : RookMonoid(5, 1)) {
      p.add_rule_and_check(rel.first.cbegin(),
                           rel.first.cend(),
                           rel.second.cbegin(),
                           rel.second.cend());
    }
    Sims1_ C(congruence_kind::left);
    C.short_rules(p);
    REQUIRE(C.number_of_threads(6).number_of_congruences(1'546) == 0);
    // On 24/08/2022 JDM ran this for approx. 16 hours overnight on his laptop,
    // the last line of output was:
    // #4: Sims1: found 63'968'999 congruences in 52156s!
    // #21: Sims1: found 759'256'468 congruences in 244617.546875
    // #12: Sims1: found 943'233'501 congruences in 321019.531250!
    // #30: Sims1: found 1'005'857'634 congruences in 350411.000000!
    // #45: Sims1: found 1'314'588'296 congruences in 487405.000000!
    // #20: Sims1: found 4'619'043'843 congruences in 2'334'184.500000!
    // #49: Sims1: found 5'582'499'404 congruences in 2'912'877.000000!
    // #10: Sims1: found 6'825'113'083 congruences in 3705611.250000!
    //
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "014",
                          "TemperleyLieb(3) from presentation",
                          "[quick][low-index]") {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.contains_empty_word(true);

    p.alphabet(2);
    for (auto const& rel : TemperleyLieb(3)) {
      p.add_rule_and_check(rel.first.cbegin(),
                           rel.first.cend(),
                           rel.second.cbegin(),
                           rel.second.cend());
    }
    {
      Sims1_ S(congruence_kind::right);
      S.short_rules(p);
      REQUIRE(S.number_of_congruences(14) == 9);
    }
    {
      Sims1_ S(congruence_kind::left);
      S.short_rules(p);
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
    for (auto const& rel : TemperleyLieb(4)) {
      p.add_rule_and_check(rel.first.cbegin(),
                           rel.first.cend(),
                           rel.second.cbegin(),
                           rel.second.cend());
    }
    {
      Sims1_ S(congruence_kind::right);
      S.short_rules(p);
      REQUIRE(S.number_of_congruences(14) == 79);
    }
    {
      Sims1_ S(congruence_kind::left);
      S.short_rules(p);
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
    Sims1_ S(congruence_kind::right);
    S.short_rules(p).extra(e);
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
    Sims1_ T(congruence_kind::right);
    T.short_rules(p).extra(e);
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
      Sims1_ T(congruence_kind::right);
      T.short_rules(p).extra(e);
      REQUIRE(T.number_of_congruences(5) == 6);
    }
    {
      Sims1_ T(congruence_kind::left);
      T.short_rules(p).extra(e);
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

    Sims1_ S(congruence_kind::right);
    S.short_rules(p).extra(e);
    REQUIRE(S.number_of_congruences(3) == 2);

    check_extra(congruence_kind::right, S.short_rules(), S.extra(), 3);
    check_extra(congruence_kind::left, S.short_rules(), S.extra(), 3);
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
    REQUIRE_THROWS_AS(Sims1_(congruence_kind::right).short_rules(p).extra(e),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(
        Sims1_(congruence_kind::right).short_rules(p).long_rules(e),
        LibsemigroupsException);
    REQUIRE_THROWS_AS(
        Sims1_(congruence_kind::right).long_rules(p).short_rules(e),
        LibsemigroupsException);
    REQUIRE_THROWS_AS(Sims1_(congruence_kind::right).long_rules(p).extra(e),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(Sims1_(congruence_kind::right).extra(p).short_rules(e),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(Sims1_(congruence_kind::right).extra(p).long_rules(e),
                      LibsemigroupsException);
    REQUIRE_NOTHROW(Sims1_(congruence_kind::right).extra(p).extra(e));
    REQUIRE_NOTHROW(
        Sims1_(congruence_kind::right).short_rules(p).short_rules(e));
    REQUIRE_NOTHROW(Sims1_(congruence_kind::right).long_rules(p).long_rules(e));
    REQUIRE_THROWS_AS(Sims1_(congruence_kind::twosided),
                      LibsemigroupsException);
    Sims1_ S(congruence_kind::right);
    REQUIRE_THROWS_AS(S.number_of_threads(0), LibsemigroupsException);
    RepOrc ro;
    REQUIRE_THROWS_AS(ro.number_of_threads(0), LibsemigroupsException);
    MinimalRepOrc mro;
    REQUIRE_THROWS_AS(mro.number_of_threads(0), LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "022",
                          "SingularBrauer(4) (Maltcev-Mazorchuk)",
                          "[extreme][sims1]") {
    auto rg = ReportGuard(true);
    auto p  = make<Presentation<word_type>>(SingularBrauer(4));
    REQUIRE(p.alphabet().size() == 12);
    REQUIRE(presentation::length(p) == 660);

    REQUIRE(*presentation::shortest_rule(p) == word_type({0}));
    REQUIRE(*(presentation::shortest_rule(p) + 1) == word_type({3}));

    presentation::remove_redundant_generators(p);

    presentation::remove_duplicate_rules(p);
    presentation::sort_each_rule(p);
    presentation::sort_rules(p);

    REQUIRE(presentation::shortest_rule_length(p) == 3);
    REQUIRE(*presentation::shortest_rule(p) == word_type({0, 0}));
    REQUIRE(*(presentation::shortest_rule(p) + 1) == word_type({0}));

    REQUIRE(presentation::longest_rule_length(p) == 6);
    REQUIRE(*presentation::longest_rule(p) == word_type({0, 4, 8}));
    REQUIRE(*(presentation::longest_rule(p) + 1) == word_type({0, 2, 8}));

    REQUIRE(p.alphabet().size() == 6);
    REQUIRE(presentation::length(p) == 462);
    REQUIRE(p.rules.size() == 186);

    p.contains_empty_word(true);
    p.validate();

    MinimalRepOrc orc;
    auto          d = orc.short_rules(p)
                 .target_size(82)
                 .number_of_threads(std::thread::hardware_concurrency())
                 .report_interval(1'999)
                 .digraph();
    REQUIRE(d.number_of_nodes() == 18);
    REQUIRE(orc.target_size() == 82);

    p.contains_empty_word(false);

    Sims1_ C(congruence_kind::right);
    C.short_rules(p);
    REQUIRE(C.short_rules().rules.size() == 186);

    REQUIRE(C.number_of_threads(std::thread::hardware_concurrency())
                .number_of_congruences(81)
            == 601'265);
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
      p.rules.erase(it, it + 2);
    } while (presentation::length(p) > 300);
    presentation::replace_subword(p, presentation::longest_common_subword(p));

    Sims1_ C(congruence_kind::right);
    C.short_rules(p).long_rule_length(8);
    REQUIRE(C.number_of_threads(std::thread::hardware_concurrency())
                .number_of_congruences(105)
            == 103'406);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "024",
                          "Brauer(4) (Kudryavtseva-Mazorchuk)",
                          "[extreme][sims1]") {
    auto rg = ReportGuard(true);
    auto p  = make<Presentation<word_type>>(Brauer(4));
    REQUIRE(p.alphabet().size() == 7);
    REQUIRE(presentation::length(p) == 182);
    presentation::remove_duplicate_rules(p);
    REQUIRE(presentation::length(p) == 162);
    presentation::reduce_complements(p);
    REQUIRE(presentation::length(p) == 159);
    presentation::sort_each_rule(p);
    presentation::sort_rules(p);
    REQUIRE(p.rules.size() == 86);

    auto d = MinimalRepOrc().short_rules(p).target_size(105).digraph();
    REQUIRE(d.number_of_nodes() == 22);
    REQUIRE(action_digraph_helper::is_strictly_cyclic(d));
    REQUIRE(
        d
        == action_digraph_helper::make<uint32_t>(
            22, {{0, 0, 1, 0, 2, 3, 2},        {1, 4, 0, 5, 6, 3, 7},
                 {2, 2, 2, 2, 2, 2, 2},        {3, 8, 3, 9, 6, 3, 7},
                 {4, 1, 4, 10, 6, 2, 11},      {5, 10, 5, 1, 12, 2, 7},
                 {6, 6, 8, 12, 6, 3, 13},      {7, 11, 9, 7, 13, 3, 7},
                 {8, 3, 6, 14, 6, 3, 11},      {9, 14, 7, 3, 12, 3, 7},
                 {10, 5, 15, 4, 12, 16, 11},   {11, 7, 17, 11, 13, 16, 11},
                 {12, 12, 18, 6, 12, 16, 13},  {13, 13, 19, 13, 13, 20, 13},
                 {14, 9, 21, 8, 12, 20, 11},   {15, 15, 10, 15, 2, 16, 2},
                 {16, 18, 16, 17, 12, 16, 11}, {17, 21, 11, 16, 6, 16, 11},
                 {18, 16, 12, 21, 12, 16, 7},  {19, 20, 13, 20, 13, 20, 13},
                 {20, 19, 20, 19, 13, 20, 13}, {21, 17, 14, 18, 6, 20, 7}}));

    auto S = make<FroidurePin<Transf<0, node_type>>>(d);
    REQUIRE(S.size() == 105);
    REQUIRE(S.generator(0) == Transf<0, node_type>::identity(22));
    REQUIRE(
        S.generator(1)
        == Transf<0, node_type>({0, 4,  2,  8, 1,  10, 6,  11, 3,  14, 5,
                                 7, 12, 13, 9, 15, 18, 21, 16, 20, 19, 17}));
    REQUIRE(
        S.generator(2)
        == Transf<0, node_type>({1,  0,  2,  3,  4,  5,  8,  9,  6,  7,  15,
                                 17, 18, 19, 21, 10, 16, 11, 12, 13, 20, 14}));
    REQUIRE(S.generator(3)
            == Transf<0, node_type>({0, 5,  2, 9,  10, 1,  12, 7,  14, 3, 4, 11,
                                     6, 13, 8, 15, 17, 16, 21, 20, 19, 18}));
    REQUIRE(S.generator(4)
            == Transf<0, node_type>({2,  6,  2,  6,  6, 12, 6, 13, 6,  12, 12,
                                     13, 12, 13, 12, 2, 12, 6, 12, 13, 13, 6}));
    REQUIRE(
        S.generator(5)
        == Transf<0, node_type>({3,  3,  2,  3,  2,  2,  3,  3,  3,  3,  16,
                                 16, 16, 20, 20, 16, 16, 16, 16, 20, 20, 20}));
    REQUIRE(
        S.generator(6)
        == Transf<0, node_type>({2,  7,  2,  7,  11, 7,  13, 7, 11, 7,  11,
                                 11, 13, 13, 11, 2,  11, 11, 7, 13, 13, 7}));

    Sims1_ C(congruence_kind::right);
    C.short_rules(p);
    REQUIRE(C.number_of_threads(std::thread::hardware_concurrency())
                .number_of_congruences(105)
            == 103'406);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "025",
                          "Brauer(5) (Kudryavtseva-Mazorchuk)",
                          "[extreme][sims1]") {
    auto rg = ReportGuard(true);
    auto p  = make<Presentation<word_type>>(Brauer(5));

    REQUIRE(presentation::length(p) == 295);

    presentation::remove_duplicate_rules(p);
    presentation::reduce_complements(p);
    presentation::sort_each_rule(p);
    presentation::sort_rules(p);
    REQUIRE(presentation::length(p) == 249);

    REQUIRE(presentation::shortest_rule_length(p) == 3);
    REQUIRE(*presentation::shortest_rule(p) == word_type({0, 0}));
    REQUIRE(*(presentation::shortest_rule(p) + 1) == word_type({0}));

    REQUIRE(p.alphabet().size() == 9);

    presentation::remove_redundant_generators(p);
    REQUIRE(p.alphabet() == word_type({1, 2, 3, 4, 5, 6, 7, 8}));
    REQUIRE(p.alphabet().size() == 8);
    REQUIRE(presentation::length(p) == 268);

    REQUIRE(*presentation::longest_rule(p) == word_type({1, 1, 1, 1}));
    REQUIRE(*(presentation::longest_rule(p) + 1) == word_type({1, 1}));

    REQUIRE(presentation::longest_common_subword(p) == word_type({1, 1}));
    p.alphabet(9);
    presentation::replace_subword(p, {1, 1}, {0});
    REQUIRE(presentation::length(p) == 246);

    // This is just very long running (without e!) and I haven't managed to run
    // it to completion.
    Presentation<word_type> e;
    e.alphabet(9);
    presentation::add_rule_and_check(e, {0}, {1});

    auto d = MinimalRepOrc()
                 .short_rules(p)
                 .extra(e)
                 .target_size(945)
                 .number_of_threads(8)
                 .report_interval(100)
                 .digraph();
    // WARNING: the number below is not necessarily the minimal degree of an
    // action on right congruences, only the minimal degree of an action on
    // right congruences containing the pair {0}, {1}.
    REQUIRE(d.number_of_nodes() == 46);
    auto S = make<FroidurePin<Transf<0, node_type>>>(d);
    REQUIRE(S.size() == 945);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "026",
                          "UniformBlockBijection(4) (Fitzgerald)",
                          "[extreme][sims1]") {
    auto rg = ReportGuard(true);
    auto p  = make<Presentation<word_type>>(UniformBlockBijectionMonoidF(4));
    presentation::remove_duplicate_rules(p);
    presentation::reduce_complements(p);
    presentation::sort_each_rule(p);
    presentation::sort_rules(p);

    Sims1_ C(congruence_kind::right);
    C.short_rules(p);
    REQUIRE(C.number_of_threads(std::thread::hardware_concurrency())
                .number_of_congruences(131)
            == 280'455);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "027",
                          "from https://mathoverflow.net/questions/423541/",
                          "[quick][sims1]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.contains_empty_word(false);
    p.alphabet("aAbBe");
    presentation::add_identity_rules(p, 'e');
    presentation::add_inverse_rules(p, "AaBbe", 'e');
    presentation::add_rule_and_check(p, "aaa", "e");
    presentation::add_rule_and_check(p, "baBBBABA", "e");
    Sims1_ C(congruence_kind::right);
    C.short_rules(p);
    REQUIRE(C.number_of_congruences(10) == 3);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "028",
                          "from https://mathoverflow.net/questions/423541/",
                          "[quick][sims1]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.contains_empty_word(true);
    p.alphabet("aAbB");
    presentation::add_inverse_rules(p, "AaBb");
    presentation::add_rule_and_check(p, "aaa", "");
    presentation::add_rule_and_check(p, "baBBBABA", "");
    Sims1_ C(congruence_kind::right);
    C.short_rules(p);
    REQUIRE(C.number_of_congruences(10) == 3);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "029",
                          "Fibonacci(4, 6)",
                          "[standard][sims1][no-valgrind]") {
    auto rg = ReportGuard(true);  // for code coverage
    auto p  = make<Presentation<word_type>>(Fibonacci(4, 6));
    presentation::remove_duplicate_rules(p);
    presentation::sort_each_rule(p);
    presentation::sort_rules(p);
    REQUIRE(presentation::length(p) == 30);
    REQUIRE(p.rules.size() == 12);
    REQUIRE(p.rules[0].size() + p.rules[1].size() == 5);

    Sims1_ C(congruence_kind::right);
    C.short_rules(p).report_interval(1);
    REQUIRE(C.number_of_congruences(3) == 5);

    C.number_of_threads(2);
    REQUIRE(C.number_of_congruences(3) == 5);
    REQUIRE_THROWS_AS(C.find_if(0, [](auto const&) { return true; }),
                      LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "030",
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
    Sims1_ C(congruence_kind::right);
    C.short_rules(p);
    REQUIRE(C.number_of_congruences(2) == 67);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "031",
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

    Sims1_ C(congruence_kind::right);
    C.short_rules(p);
    REQUIRE(C.number_of_congruences(2) == 7);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "032",
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
    Sims1_ S(congruence_kind::right);
    S.short_rules(p);
    REQUIRE(S.number_of_congruences(3) == 14);
    REQUIRE(S.number_of_congruences(4) == 14);
    REQUIRE(S.number_of_congruences(5) == 14);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1", "033", "constructors", "[quick][sims1]") {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.contains_empty_word(true);

    //          a  A  b  B  c  C
    p.alphabet({0, 1, 2, 3, 4, 5});
    presentation::add_inverse_rules(p, {1, 0, 3, 2, 5, 4});
    presentation::add_rule_and_check(p, {0, 0, 5, 0, 4}, {});
    presentation::add_rule_and_check(p, {0, 4, 2, 2, 1, 5, 2}, {});
    presentation::add_rule_and_check(p, {1, 3, 0, 2, 4, 4, 4}, {});
    Sims1_ S(congruence_kind::right);
    S.short_rules(p);

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

    Sims1_ C(congruence_kind::right);
    REQUIRE_THROWS_AS(C.short_rules(p).extra(e), LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1", "034", "split_at", "[quick][sims1]") {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.contains_empty_word(true);

    //          a  A  b  B  c  C
    p.alphabet({0, 1, 2, 3, 4, 5});
    presentation::add_inverse_rules(p, {1, 0, 3, 2, 5, 4});
    presentation::add_rule_and_check(p, {0, 0, 5, 0, 4}, {});
    presentation::add_rule_and_check(p, {0, 4, 2, 2, 1, 5, 2}, {});
    presentation::add_rule_and_check(p, {1, 3, 0, 2, 4, 4, 4}, {});
    Sims1_ S(congruence_kind::right);
    S.short_rules(p);

    REQUIRE_THROWS_AS(S.split_at(10), LibsemigroupsException);
    S.split_at(0);

    REQUIRE(S.short_rules().rules.empty());

    for (size_t i = 0; i <= p.rules.size() / 2; ++i) {
      S.split_at(i);
      REQUIRE(S.short_rules().rules.size() == 2 * i);
    }
    REQUIRE(S.short_rules().rules.size() == p.rules.size());
    for (size_t i = p.rules.size() / 2; i > 0; --i) {
      S.split_at(i);
      REQUIRE(S.short_rules().rules.size() == 2 * i);
    }
    S.split_at(7);
    REQUIRE(S.number_of_congruences(3) == 14);
  }

#ifdef LIBSEMIGROUPS_ENABLE_STATS
  LIBSEMIGROUPS_TEST_CASE("Sims1", "035", "stats", "[quick][sims1]") {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.contains_empty_word(true);

    //          a  A  b  B  c  C
    p.alphabet({0, 1, 2, 3, 4, 5});
    presentation::add_inverse_rules(p, {1, 0, 3, 2, 5, 4});
    presentation::add_rule_and_check(p, {0, 0, 5, 0, 4}, {});
    presentation::add_rule_and_check(p, {0, 4, 2, 2, 1, 5, 2}, {});
    presentation::add_rule_and_check(p, {1, 3, 0, 2, 4, 4, 4}, {});
    Sims1_ S(congruence_kind::right);
    S.short_rules(p);

    std::stringbuf buff;
    std::ostream   os(&buff);
    S.number_of_congruences(2);
    // REQUIRE(S.stats().max_pending != 0);
    os << S.cbegin(3).stats();  // Also does not do anything visible
  }
#endif

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "036",
                          "check iterator requirements",
                          "[quick][sims1]") {
    Presentation<word_type> p;
    p.contains_empty_word(true);
    p.alphabet({0, 1});
    presentation::add_rule_and_check(p, {0, 0, 0}, {0});
    presentation::add_rule_and_check(p, {1, 1}, {1});
    presentation::add_rule_and_check(p, {0, 1, 0, 1}, {0});

    {
      Sims1_ S(congruence_kind::right);
      S.short_rules(p);
      verify_forward_iterator_requirements(S.cbegin(10));
      auto it = S.cbegin(10);
      REQUIRE(it->number_of_nodes() == 10);
    }
    {
      Sims1_ S(congruence_kind::left);
      S.short_rules(p);
      verify_forward_iterator_requirements(S.cbegin(10));
      auto it = S.cbegin(10);
      REQUIRE(it->number_of_nodes() == 10);
    }
  }

  // Takes about 30s
  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "037",
                          "RectangularBand(9, 2)",
                          "[extreme][sims1]") {
    auto rg = ReportGuard(true);
    auto p  = make<Presentation<word_type>>(RectangularBand(9, 2));
    presentation::remove_duplicate_rules(p);
    presentation::sort_each_rule(p);
    presentation::sort_rules(p);

    REQUIRE(MinimalRepOrc()
                .short_rules(p)
                .target_size(18)
                .number_of_threads(std::thread::hardware_concurrency())
                .digraph()
                .number_of_nodes()
            == 0);
    p.contains_empty_word(true);
    auto mro = MinimalRepOrc().short_rules(p).target_size(19).number_of_threads(
        std::thread::hardware_concurrency());
    auto d = mro.digraph();
    REQUIRE(d.number_of_nodes() == 11);
    REQUIRE(action_digraph_helper::is_strictly_cyclic(d));
    auto S = make<FroidurePin<Transf<0, node_type>>>(d);
    S.add_generator(S.generator(0).identity());
    REQUIRE(S.size() == 19);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "038",
                          "PartitionMonoid(3) - minimal o.r.c. rep",
                          "[extreme][sims1]") {
    auto rg = ReportGuard(true);
    auto p = make<Presentation<word_type>>(PartitionMonoid(3, author::Machine));
    REQUIRE(!p.contains_empty_word());
    REQUIRE(p.alphabet() == word_type({0, 1, 2, 3, 4}));

    auto d = RepOrc()
                 .short_rules(p)
                 .target_size(203)
                 .min_nodes(1)
                 .max_nodes(22)
                 .number_of_threads(2)
                 .digraph();
    REQUIRE(d.number_of_nodes() == 22);

    auto mro
        = MinimalRepOrc().short_rules(p).target_size(203).number_of_threads(4);
    d = mro.digraph();

    REQUIRE(action_digraph_helper::is_strictly_cyclic(d));
    auto S = make<FroidurePin<Transf<0, node_type>>>(d);
    REQUIRE(S.size() == 203);
    // The actual digraph obtained is non-deterministic because we just take
    // whichever one is found first.
    // REQUIRE(
    //     d
    //     == action_digraph_helper::make<node_type>(
    //         22,
    //         {{0, 1, 0, 2, 0},      {1, 3, 3, 4, 5},      {2, 6, 6, 2, 0},
    //          {3, 0, 1, 2, 5},      {4, 4, 4, 4, 4},      {5, 5, 5, 7, 5},
    //          {6, 8, 2, 4, 0},      {7, 9, 9, 7, 5},      {8, 2, 8, 4, 10},
    //          {9, 10, 7, 11, 5},    {10, 7, 10, 12, 10},  {11, 13, 11, 11,
    //          14}, {12, 11, 13, 12, 10}, {13, 12, 12, 15, 10}, {14, 16, 14,
    //          11, 14}, {15, 15, 15, 15, 17}, {16, 18, 18, 19, 5},  {17, 19,
    //          17, 15, 17}, {18, 14, 16, 12, 5},  {19, 20, 20, 19, 21}, {20,
    //          17, 19, 15, 21}, {21, 21, 21, 19, 21}}));
    REQUIRE(d.number_of_nodes() == 22);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "039",
                          "TemperleyLieb(n) - n = 3 .. 6, minimal rep",
                          "[standard][sims1]") {
    auto rg = ReportGuard(false);

    std::array<uint64_t, 11> const sizes
        = {0, 1, 2, 5, 14, 42, 132, 429, 1'430, 4'862, 16'796};
    std::array<uint64_t, 11> const min_degrees
        = {0, 0, 2, 4, 7, 10, 20, 29, 63, 91, 0};
    // The values 63 and 91 are not verified

    for (size_t n = 3; n <= 6; ++n) {
      auto p = make<Presentation<word_type>>(TemperleyLieb(n));
      // There are no relations containing the empty word so we just manually
      // add it.
      p.contains_empty_word(true);
      auto orc
          = MinimalRepOrc().short_rules(p).number_of_threads(2).target_size(
              sizes[n]);

      auto d = orc.digraph();
      REQUIRE(orc.target_size() == sizes[n]);
      REQUIRE(action_digraph_helper::is_strictly_cyclic(d));
      auto S = make<FroidurePin<Transf<0, node_type>>>(d);
      S.add_generator(S.generator(0).identity());
      REQUIRE(S.size() == sizes[n]);
      REQUIRE(d.number_of_nodes() == min_degrees[n]);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "040",
                          "TransitiveGroup(10, 32) - minimal rep",
                          "[quick][sims1]") {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.contains_empty_word(true);
    p.alphabet({0, 1, 2, 3, 4});
    presentation::add_rule_and_check(p, {0, 0}, {});
    presentation::add_rule_and_check(p, {1, 1}, {});
    presentation::add_rule_and_check(p, {2, 2}, {});
    presentation::add_rule_and_check(p, {3, 3}, {});
    presentation::add_rule_and_check(p, {4, 4}, {});
    presentation::add_rule_and_check(p, {0, 1, 0, 1, 0, 1}, {});
    presentation::add_rule_and_check(p, {0, 2, 0, 2}, {});
    presentation::add_rule_and_check(p, {0, 3, 0, 3}, {});
    presentation::add_rule_and_check(p, {0, 4, 0, 4}, {});
    presentation::add_rule_and_check(p, {1, 2, 1, 2, 1, 2}, {});
    presentation::add_rule_and_check(p, {1, 3, 1, 3}, {});
    presentation::add_rule_and_check(p, {1, 4, 1, 4}, {});
    presentation::add_rule_and_check(p, {2, 3, 2, 3, 2, 3}, {});
    presentation::add_rule_and_check(p, {2, 4, 2, 4}, {});
    presentation::add_rule_and_check(p, {3, 4, 3, 4, 3, 4}, {});
    REQUIRE(MinimalRepOrc()
                .short_rules(p)
                .target_size(0)
                .digraph()
                .number_of_nodes()
            == 0);

    REQUIRE(RepOrc()
                .short_rules(p)
                .min_nodes(0)
                .max_nodes(0)
                .target_size(0)
                .digraph()
                .number_of_nodes()
            == 0);

    auto d = MinimalRepOrc().short_rules(p).target_size(720).digraph();
    REQUIRE(d.number_of_nodes() == 6);
    REQUIRE(action_digraph_helper::is_strictly_cyclic(d));
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "041",
                          "RectangularBand(4, 4) - minimal o.r.c. rep",
                          "[standard][sims1]") {
    auto rg = ReportGuard(false);
    auto p  = make<Presentation<word_type>>(RectangularBand(4, 4));
    p.contains_empty_word(true);
    auto d = MinimalRepOrc()
                 .short_rules(p)
                 .number_of_threads(2)
                 .target_size(17)
                 .digraph();
    REQUIRE(action_digraph_helper::is_strictly_cyclic(d));
    auto S = make<FroidurePin<Transf<0, node_type>>>(d);
    REQUIRE(S.size() == 16);
    REQUIRE(d.number_of_nodes() == 7);

    p.contains_empty_word(false);
    d = MinimalRepOrc()
            .short_rules(p)
            .target_size(16)
            .number_of_threads(2)
            .digraph();
    REQUIRE(d.number_of_nodes() == 0);
  }

  // unbuffer -p ./test_sims1 "[042]" | ag -v FroidurePin
  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "042",
                          "RectangularBand(m, n) - m = 1 .. 5, n = 1 .. 5",
                          "[fail][sims1]") {
    // This doesn't fail it's just very extreme
    std::vector<std::array<size_t, 6>> results = {{0, 0, 0, 0, 0, 0},
                                                  {0, 2, 2, 3, 4, 5},
                                                  {0, 3, 4, 5, 5, 6},
                                                  {0, 4, 5, 6, 6, 7},
                                                  {0, 5, 6, 7, 7, 8},
                                                  {0, 6, 7, 8, 8, 9}};

    auto rg = ReportGuard(true);
    for (size_t m = 1; m <= 5; ++m) {
      for (size_t n = 1; n <= 5; ++n) {
        std::cout << std::string(72, '#') << "\n"
                  << "CASE m, n = " << m << ", " << n << "\n"
                  << std::string(72, '#') << std::endl;

        auto p = make<Presentation<word_type>>(RectangularBand(m, n));
        p.contains_empty_word(true);
        auto d = MinimalRepOrc()
                     .short_rules(p)
                     .target_size(m * n + 1)
                     .number_of_threads(6)
                     .digraph();
        REQUIRE(action_digraph_helper::is_strictly_cyclic(d));
        auto S = make<FroidurePin<Transf<0, node_type>>>(d);
        REQUIRE(S.size() == m * n);
        REQUIRE(d.number_of_nodes() == results[m][n]);
      }
    }
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "043",
                          "RectangularBand(2, 2) - with and without identity",
                          "[quick][sims1]") {
    auto   rg = ReportGuard(false);
    auto   p  = make<Presentation<word_type>>(RectangularBand(2, 2));
    Sims1_ S(congruence_kind::right);
    S.short_rules(p);

    REQUIRE(S.number_of_congruences(4) == 6);

    p.contains_empty_word(true);

    Sims1_ T(congruence_kind::right);
    T.short_rules(p);
    REQUIRE(T.number_of_congruences(5) == 9);

    auto it = S.cbegin(4);

    REQUIRE(*it++
            == action_digraph_helper::make<node_type>(
                5, {{1, 1, 1, 1}, {1, 1, 1, 1}}));  // Good
    REQUIRE(*it++
            == action_digraph_helper::make<node_type>(
                5, {{1, 1, 1, 2}, {1, 1, 1, 2}, {1, 1, 1, 2}}));  // Good
    REQUIRE(*it++
            == action_digraph_helper::make<node_type>(
                5, {{1, 1, 2, 1}, {1, 1, 1, 1}, {2, 2, 2, 2}}));  // Good
    REQUIRE(
        *it++
        == action_digraph_helper::make<node_type>(
            5,
            {{1, 1, 2, 1}, {1, 1, 1, 1}, {2, 2, 2, 3}, {2, 2, 2, 3}}));  // Good
    REQUIRE(
        *it++
        == action_digraph_helper::make<node_type>(
            5,
            {{1, 1, 2, 3}, {1, 1, 1, 3}, {2, 2, 2, 2}, {1, 1, 1, 3}}));  // Good
    REQUIRE(*it++
            == action_digraph_helper::make<node_type>(5,
                                                      {{1, 1, 2, 3},
                                                       {1, 1, 1, 3},
                                                       {2, 2, 2, 4},
                                                       {1, 1, 1, 3},
                                                       {2, 2, 2, 4}}));  // Good
    REQUIRE(it->number_of_nodes() == 0);

    it = T.cbegin(5);

    REQUIRE(*it++ == action_digraph_helper::make<node_type>(5, {{0, 0, 0, 0}}));
    REQUIRE(*it++
            == action_digraph_helper::make<node_type>(
                5, {{0, 0, 0, 1}, {0, 0, 0, 1}}));
    REQUIRE(*it++
            == action_digraph_helper::make<node_type>(
                5, {{1, 1, 1, 0}, {1, 1, 1, 0}}));
    REQUIRE(*it++
            == action_digraph_helper::make<node_type>(
                5, {{1, 1, 1, 1}, {1, 1, 1, 1}}));
    REQUIRE(*it++
            == action_digraph_helper::make<node_type>(
                5, {{1, 1, 1, 2}, {1, 1, 1, 2}, {1, 1, 1, 2}}));
    REQUIRE(*it++
            == action_digraph_helper::make<node_type>(
                5, {{1, 1, 2, 1}, {1, 1, 1, 1}, {2, 2, 2, 2}}));
    REQUIRE(*it++
            == action_digraph_helper::make<node_type>(
                5, {{1, 1, 2, 1}, {1, 1, 1, 1}, {2, 2, 2, 3}, {2, 2, 2, 3}}));
    REQUIRE(*it++
            == action_digraph_helper::make<node_type>(
                5, {{1, 1, 2, 3}, {1, 1, 1, 3}, {2, 2, 2, 2}, {1, 1, 1, 3}}));
    REQUIRE(*it++
            == action_digraph_helper::make<node_type>(5,
                                                      {{1, 1, 2, 3},
                                                       {1, 1, 1, 3},
                                                       {2, 2, 2, 4},
                                                       {1, 1, 1, 3},
                                                       {2, 2, 2, 4}}));
    REQUIRE(it->number_of_nodes() == 0);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "044",
                          "trivial group - minimal o.r.c. rep",
                          "[quick][sims1]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("aAbB");
    p.contains_empty_word(true);
    presentation::add_inverse_rules(p, "AaBb");
    presentation::add_rule_and_check(p, "ab", "");
    presentation::add_rule_and_check(p, "abb", "");

    Sims1_ S(congruence_kind::right);
    S.short_rules(p);

    REQUIRE(S.number_of_congruences(10) == 1);
    auto d = MinimalRepOrc().short_rules(p).target_size(1).digraph();
    REQUIRE(d.number_of_nodes() == 1);
    REQUIRE(action_digraph_helper::is_strictly_cyclic(d));
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "045",
                          "right zero semigroup - minimal o.r.c. rep",
                          "[quick][sims1]") {
    // This is an example of a semigroup with a strictly cyclic faithful
    // right representation.
    auto         rg = ReportGuard(false);
    size_t const n  = 5;
    auto         p  = make<Presentation<word_type>>(RectangularBand(1, n));
    auto         d  = MinimalRepOrc().short_rules(p).target_size(n).digraph();
    REQUIRE(action_digraph_helper::is_strictly_cyclic(d));
    auto S = make<FroidurePin<Transf<0, node_type>>>(d);
    REQUIRE(S.size() == n);
    REQUIRE(d.number_of_nodes() == 5);
  }

  LIBSEMIGROUPS_TEST_CASE(
      "Sims1",
      "046",
      "semigroup with faithful non-strictly cyclic action of right congruence",
      "[quick][sims1]") {
    // Found with Smallsemi, this example is minimal wrt size of the semigroup.

    auto rg = ReportGuard(false);

    FroidurePin<Transf<6>> S({Transf<6>::make({0, 0, 2, 1, 4, 1}),
                              Transf<6>::make({0, 0, 2, 3, 4, 3}),
                              Transf<6>::make({0, 2, 2, 0, 4, 4})});

    REQUIRE(S.size() == 5);
    auto p = make<Presentation<word_type>>(S);
    auto d = MinimalRepOrc().short_rules(p).target_size(5).digraph();
    REQUIRE(action_digraph_helper::is_strictly_cyclic(d));
    REQUIRE(d.number_of_nodes() == 4);
    REQUIRE(d
            == action_digraph_helper::make<uint32_t>(
                4, {{2, 2, 3}, {0, 1, 2}, {2, 2, 2}, {3, 3, 3}}));
    auto T = make<FroidurePin<Transf<4>>>(d);
    REQUIRE(T.generator(0) == Transf<4>({2, 0, 2, 3}));
    REQUIRE(T.generator(1) == Transf<4>({2, 1, 2, 3}));
    REQUIRE(T.generator(2) == Transf<4>({3, 2, 2, 3}));
    REQUIRE(T.size() == 5);

    auto dd = action_digraph_helper::make<uint8_t>(5,
                                                   {{0, 0, 0, 0, 0},
                                                    {0, 0, 0, 0, 2},
                                                    {2, 2, 2, 2, 2},
                                                    {0, 1, 2, 3, 0},
                                                    {4, 4, 4, 4, 4}});

    REQUIRE(!action_digraph_helper::is_strictly_cyclic(dd));
    REQUIRE(dd.number_of_nodes() == 5);
    auto U = make<FroidurePin<Transf<5>>>(dd);
    REQUIRE(U.size() == 5);

    Sims1_ C(congruence_kind::right);
    C.short_rules(p);
    REQUIRE(C.number_of_congruences(5) == 9);
    uint64_t strictly_cyclic_count     = 0;
    uint64_t non_strictly_cyclic_count = 0;

    for (auto it = C.cbegin(5); it != C.cend(5); ++it) {
      auto W = make<FroidurePin<Transf<0, node_type>>>(
          *it, 1, it->number_of_active_nodes());
      if (p.contains_empty_word()) {
        auto one = W.generator(0).identity();
        if (!W.contains(one)) {
          W.add_generator(one);
        }
      }
      if (W.size() == 5) {
        auto result = *it;
        result.induced_subdigraph(1, result.number_of_active_nodes());
        result.number_of_active_nodes(result.number_of_active_nodes() - 1);
        if (action_digraph_helper::is_strictly_cyclic(result)) {
          strictly_cyclic_count++;
        } else {
          REQUIRE(W.generator(0) == Transf<0, node_type>({3, 0, 2, 3, 4}));
          REQUIRE(W.generator(1) == Transf<0, node_type>({3, 1, 2, 3, 4}));
          REQUIRE(W.generator(2) == Transf<0, node_type>({4, 3, 2, 3, 4}));
          REQUIRE(
              result
              == action_digraph_helper::make<uint32_t>(
                  5, {{3, 3, 4}, {0, 1, 3}, {2, 2, 2}, {3, 3, 3}, {4, 4, 4}}));
          non_strictly_cyclic_count++;
        }
      }
    }
    REQUIRE(strictly_cyclic_count == 2);
    REQUIRE(non_strictly_cyclic_count == 1);
  }

  // Takes about 3 to 4 minutes
  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "047",
                          "RectangularBand(m, n) - m = 1 .. 5, n = 1 .. 5",
                          "[fail][sims1]") {
    // This doesn't fail it's just very extreme
    std::vector<std::array<size_t, 7>> left
        = {{0, 0, 0, 0, 0, 0},
           {0, 0, 0, 0, 0, 0},
           {0, 0, 6, 22, 94, 454, 2'430},
           {0, 0, 30, 205, 1'555, 12'880},
           {0, 0, 240, 4'065, 72'465, 1'353'390},
           {0, 0, 2'756, 148'772, 8'174'244, 456'876'004}};

    // Seems like the m,n-th entry of the table above is:
    // {m, n} ->  Sum([0 .. n], k -> Bell(m)^k*Stirling2(n, k));

    auto rg = ReportGuard(true);
    for (size_t m = 2; m <= 5; ++m) {
      for (size_t n = 2; n <= 6; ++n) {
        std::cout << std::string(72, '#') << "\n"
                  << "CASE m, n = " << m << ", " << n << "\n"
                  << std::string(72, '#') << std::endl;

        auto   p = make<Presentation<word_type>>(RectangularBand(m, n));
        Sims1_ S(congruence_kind::left);
        S.short_rules(p);
        REQUIRE(S.number_of_threads(4).number_of_congruences(m * n)
                == left[m][n]);
        Sims1_ T(congruence_kind::right);
        T.short_rules(p);
        REQUIRE(T.number_of_threads(4).number_of_congruences(m * n)
                == left[n][m]);
      }
    }
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "048",
                          "Stellar(n) n = 3 .. 4",
                          "[fail][sims1][babbage]") {
    std::array<uint64_t, 10> const size      = {0, 0, 0, 16, 65};
    std::array<uint64_t, 10> const num_left  = {0, 0, 0, 1'550, 0};
    std::array<uint64_t, 10> const num_right = {0, 0, 0, 1'521, 0};

    for (size_t n = 3; n < 5; ++n) {
      auto p = make<Presentation<word_type>>(RookMonoid(n, 0));
      auto q = make<Presentation<word_type>>(Stell(n));
      p.rules.insert(p.rules.end(), q.rules.cbegin(), q.rules.cend());
      REQUIRE(p.alphabet().size() == n + 1);
      {
        Sims1_ S(congruence_kind::left);
        S.short_rules(p).number_of_threads(4);
        REQUIRE(S.number_of_congruences(size[n]) == num_left[n]);
      }
      {
        Sims1_ S(congruence_kind::right);
        S.short_rules(p).number_of_threads(4);
        REQUIRE(S.number_of_congruences(size[n]) == num_right[n]);
      }
    }
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "049",
                          "Stylic(n) n = 3, 4",
                          "[fail][sims1]") {
    auto                           rg   = ReportGuard(true);
    std::array<uint64_t, 10> const size = {0, 0, 0, 14, 51};
    //               1505s
    std::array<uint64_t, 10> const num_left  = {0, 0, 0, 1'214, 1'429'447'174};
    std::array<uint64_t, 10> const num_right = {0, 0, 0, 1'214, 1'429'455'689};

    for (size_t n = 3; n < 5; ++n) {
      auto p = make<Presentation<word_type>>(Stylic(n));
      {
        Sims1_ S(congruence_kind::right);
        S.short_rules(p).number_of_threads(4);
        REQUIRE(S.number_of_congruences(size[n]) == num_right[n]);
      }
      {
        Sims1_ S(congruence_kind::left);
        S.short_rules(p).number_of_threads(4);
        REQUIRE(S.number_of_congruences(size[n]) == num_left[n]);
      }
    }
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "050",
                          "(2, 3, 7)-triangle group - index 50",
                          "[extreme][sims1]") {
    auto                      rg = ReportGuard(true);
    Presentation<std::string> p;
    p.contains_empty_word(true);
    p.alphabet("xy");
    presentation::add_rule_and_check(p, "xx", "");
    presentation::add_rule_and_check(p, "yyy", "");
    presentation::add_rule_and_check(p, "xyxyxyxyxyxyxy", "");
    Sims1_ S(congruence_kind::right);
    S.short_rules(p).number_of_threads(4);
    REQUIRE(S.number_of_congruences(50) == 75'971);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "051",
                          "Heineken group - index 10",
                          "[extreme][sims1]") {
    auto                      rg = ReportGuard(true);
    Presentation<std::string> p;
    p.contains_empty_word(true);
    p.alphabet("xXyY");
    presentation::add_inverse_rules(p, "XxYy");
    presentation::add_rule_and_check(p, "yXYYxyYYxyyXYYxyyXyXYYxy", "x");

    Presentation<std::string> q;
    q.alphabet("xXyY");
    presentation::add_rule_and_check(
        q, "YxyyXXYYxyxYxyyXYXyXYYxxyyXYXyXYYxyx", "y");

    Sims1_ S(congruence_kind::right);
    S.short_rules(p).long_rules(q).number_of_threads(8);
    REQUIRE(S.number_of_congruences(10) == 1);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "052",
                          "TemperleyLieb(n) - n = 3 .. 6",
                          "[extreme][low-index][babbage]") {
    std::array<uint64_t, 10> const size = {0, 0, 0, 5, 14, 42, 132, 429};
    std::array<uint64_t, 10> const num_right
        = {0, 0, 0, 9, 79, 2'157, 4'326'459};

    auto rg = ReportGuard(true);
    for (size_t n = 3; n < 7; ++n) {
      auto p = make<Presentation<word_type>>(TemperleyLieb(n));
      p.contains_empty_word(true);
      Sims1_ S(congruence_kind::right);
      S.short_rules(p).number_of_threads(4);
      REQUIRE(S.number_of_congruences(size[n]) == num_right[n]);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "053",
                          "PartialTransformationMonoid(3)",
                          "[extreme][low-index]") {
    auto rg = ReportGuard(true);
    auto p  = make<Presentation<word_type>>(
        PartialTransformationMonoid(3, author::Machine));
    {
      Sims1_ S(congruence_kind::right);
      S.short_rules(p).number_of_threads(4);
      REQUIRE(S.number_of_congruences(64) == 92'703);
    }
    {
      Sims1_ S(congruence_kind::left);
      S.short_rules(p).number_of_threads(4);
      REQUIRE(S.number_of_congruences(64) == 371);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "054",
                          "PartialTransformationMonoid(4) from FroidurePin",
                          "[fail][low-index]") {
    using Transf_ = Transf<5>;
    auto rg       = ReportGuard(true);

    FroidurePin<Transf_> S({Transf_({1, 0, 2, 3, 4}),
                            Transf_({3, 0, 1, 2, 4}),
                            Transf_({4, 1, 2, 3, 4}),
                            Transf_({1, 1, 2, 3, 4})});
    REQUIRE(S.size() == 625);
    auto   p = make<Presentation<word_type>>(S);
    Sims1_ C(congruence_kind::left);
    REQUIRE(presentation::longest_rule_length(p) == 18);
    REQUIRE(presentation::shortest_rule_length(p) == 3);

    presentation::remove_duplicate_rules(p);
    presentation::remove_trivial_rules(p);
    presentation::sort_each_rule(p);
    presentation::sort_rules(p);

    word_type w = presentation::longest_common_subword(p);
    while (!w.empty()) {
      presentation::replace_subword(p, w);
      w = presentation::longest_common_subword(p);
    }

    REQUIRE(presentation::length(p) == 1414);
    REQUIRE(presentation::longest_rule_length(p) == 6);

    C.short_rules(p).long_rule_length(6).number_of_threads(8).report_interval(
        100);
    REQUIRE(C.number_of_congruences(625) == 10);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "055",
                          "PlacticMonoid(3) up to index 8",
                          "[extreme][low-index][plactic]") {
    std::array<uint64_t, 9> const num
        = {0, 1, 29, 484, 6'896, 103'204, 1'773'360, 35'874'182, 849'953'461};
    auto rg = ReportGuard(true);
    auto p  = make<Presentation<word_type>>(Plactic(3));
    for (size_t n = 2; n < 9; ++n) {
      {
        Sims1_ S(congruence_kind::right);
        S.short_rules(p).number_of_threads(4);
        REQUIRE(S.number_of_congruences(n) == num[n]);
      }
      {
        Sims1_ S(congruence_kind::left);
        S.short_rules(p).number_of_threads(4);
        REQUIRE(S.number_of_congruences(n) == num[n]);
      }
    }
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "056",
                          "PlacticMonoid(4) up to index 6",
                          "[extreme][low-index][plactic]") {
    std::array<uint64_t, 8> const num
        = {0, 1, 67, 2'794, 106'264, 4'795'980, 278'253'841, 20'855'970'290};
    // Last value too 1h34m to compute so is not included.
    auto rg = ReportGuard(true);
    auto p  = make<Presentation<word_type>>(Plactic(4));
    for (size_t n = 2; n < 7; ++n) {
      {
        Sims1_ S(congruence_kind::right);
        S.short_rules(p).number_of_threads(4);
        REQUIRE(S.number_of_congruences(n) == num[n]);
      }
      {
        Sims1_ S(congruence_kind::left);
        S.short_rules(p).number_of_threads(4);
        REQUIRE(S.number_of_congruences(n) == num[n]);
      }
    }
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "057",
                          "PlacticMonoid(5) up to index 5",
                          "[extreme][low-index][plactic]") {
    std::array<uint64_t, 7> const num
        = {0, 1, 145, 14'851, 1'496'113, 198'996'912, 37'585'675'984};
    // Last value took 5h11m to compute
    auto rg = ReportGuard(true);
    auto p  = make<Presentation<word_type>>(Plactic(5));
    for (size_t n = 3; n < 6; ++n) {
      {
        Sims1_ S(congruence_kind::right);
        S.short_rules(p).number_of_threads(4);
        REQUIRE(S.number_of_congruences(n) == num[n]);
      }
      {
        Sims1_ S(congruence_kind::left);
        S.short_rules(p).number_of_threads(4);
        REQUIRE(S.number_of_congruences(n) == num[n]);
      }
    }
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "058",
                          "PlacticMonoid(6) up to index 4",
                          "[extreme][low-index][plactic]") {
    std::array<uint64_t, 6> const num
        = {0, 1, 303, 77'409, 20'526'128, 7'778'840'717};
    // The last value took 4h5m to run and is omitted.
    auto rg = ReportGuard(true);
    auto p  = make<Presentation<word_type>>(Plactic(6));
    for (size_t n = 2; n < 5; ++n) {
      {
        Sims1_ S(congruence_kind::right);
        S.short_rules(p).number_of_threads(4);
        REQUIRE(S.number_of_congruences(n) == num[n]);
      }
      {
        Sims1_ S(congruence_kind::left);
        S.short_rules(p).number_of_threads(4);
        REQUIRE(S.number_of_congruences(n) == num[n]);
      }
    }
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "059",
                          "PlacticMonoid(7) up to index 3",
                          "[extreme][low-index][plactic]") {
    std::array<uint64_t, 5> const num = {0, 1, 621, 408'024, 281'600'130};
    // The last value took approx. 12m34s to run and is omitted from the
    // extreme test 12m34s to run and is omitted from the extreme test.
    auto rg = ReportGuard(true);
    auto p  = make<Presentation<word_type>>(Plactic(7));
    for (size_t n = 2; n < 4; ++n) {
      {
        Sims1_ S(congruence_kind::right);
        S.short_rules(p).number_of_threads(4);
        REQUIRE(S.number_of_congruences(n) == num[n]);
      }
      {
        Sims1_ S(congruence_kind::left);
        S.short_rules(p).number_of_threads(4);
        REQUIRE(S.number_of_congruences(n) == num[n]);
      }
    }
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "060",
                          "PlacticMonoid(8) up to index 3",
                          "[extreme][low-index][plactic]") {
    std::array<uint64_t, 4> const num = {0, 1, 1'259, 2'201'564};
    auto                          rg  = ReportGuard(true);
    auto                          p = make<Presentation<word_type>>(Plactic(8));
    for (size_t n = 2; n < 4; ++n) {
      {
        Sims1_ S(congruence_kind::right);
        S.short_rules(p).number_of_threads(4);
        REQUIRE(S.number_of_congruences(n) == num[n]);
      }
      {
        Sims1_ S(congruence_kind::left);
        S.short_rules(p).number_of_threads(4);
        REQUIRE(S.number_of_congruences(n) == num[n]);
      }
    }
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "061",
                          "ChineseMonoid(3) up to index 8",
                          "[extreme][low-index][chinese]") {
    std::array<uint64_t, 9> const num
        = {0, 1, 31, 559, 8'904, 149'529, 2'860'018, 63'828'938, 1'654'488'307};
    // index 8 is doable and the value is included above, but it took about X
    // minutes to run, so isn't included in the loop below.
    auto rg = ReportGuard(true);
    auto p  = make<Presentation<word_type>>(ChineseMonoid(3));
    for (size_t n = 2; n < 8; ++n) {
      Sims1_ S(congruence_kind::right);
      S.short_rules(p).number_of_threads(4);
      REQUIRE(S.number_of_congruences(n) == num[n]);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "062",
                          "ChineseMonoid(4) up to index 6",
                          "[extreme][low-index][chinese]") {
    // n = 6 took between 3 and 4 minutes
    // n = 7 took 6h16m
    // both are omitted
    std::array<uint64_t, 8> const num
        = {0, 1, 79, 3'809, 183'995, 10'759'706, 804'802'045, 77'489'765'654};
    auto rg = ReportGuard(true);
    auto p  = make<Presentation<word_type>>(ChineseMonoid(4));
    for (size_t n = 3; n < 7; ++n) {
      Sims1_ S(congruence_kind::right);
      S.short_rules(p).number_of_threads(4);
      REQUIRE(S.number_of_congruences(n) == num[n]);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "063",
                          "ChineseMonoid(5) up to index 5",
                          "[extreme][low-index][chinese]") {
    std::array<uint64_t, 7> const num
        = {0, 1, 191, 23'504, 3'382'921, 685'523'226, 199'011'439'587};

    // The last value took 21h32m and so is omitted
    auto rg = ReportGuard(true);
    auto p  = make<Presentation<word_type>>(ChineseMonoid(5));
    for (size_t n = 3; n < 6; ++n) {
      Sims1_ S(congruence_kind::right);
      S.short_rules(p).number_of_threads(4);
      REQUIRE(S.number_of_congruences(n) == num[n]);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "064",
                          "ChineseMonoid(6) up to index 4",
                          "[extreme][low-index][chinese]") {
    // 0 1 2 3 4
    std::array<uint64_t, 6> const num
        = {0, 1, 447, 137'694, 58'624'384, 40'823'448'867};
    // The last value took 9h54m to compute, and is omitted!
    auto rg = ReportGuard(true);
    auto p  = make<Presentation<word_type>>(ChineseMonoid(6));
    for (size_t n = 3; n < 5; ++n) {
      Sims1_ S(congruence_kind::right);
      S.short_rules(p).number_of_threads(4);
      REQUIRE(S.number_of_congruences(n) == num[n]);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "065",
                          "ChineseMonoid(7) up to index 4",
                          "[extreme][low-index][chinese]") {
    // Last value took about 50m to compute
    std::array<uint64_t, 5> const num = {0, 1, 1'023, 786'949, 988'827'143};
    auto                          rg  = ReportGuard(true);
    auto p = make<Presentation<word_type>>(ChineseMonoid(7));
    for (size_t n = 2; n < 4; ++n) {
      Sims1_ S(congruence_kind::right);
      S.short_rules(p).number_of_threads(4);
      REQUIRE(S.number_of_congruences(n) == num[n]);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "066",
                          "ChineseMonoid(8) up to index 3",
                          "[extreme][low-index][chinese]") {
    std::array<uint64_t, 4> const num = {0, 1, 2'303, 4'459'599};
    auto                          rg  = ReportGuard(true);
    auto p = make<Presentation<word_type>>(ChineseMonoid(8));
    for (size_t n = 2; n < 4; ++n) {
      Sims1_ S(congruence_kind::right);
      S.short_rules(p).number_of_threads(4);
      REQUIRE(S.number_of_congruences(n) == num[n]);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "067",
                          "FreeSemigroup(n) up to index 3",
                          "[extreme][low-index]") {
    // (27^n - 9^n)/2 - 12^n + 6^n
    std::array<uint64_t, 10> const num = {0, 2, 229, 8022, 243241, 6904866};
    // 4 ^ n - 2 ^ n
    //  {0, 2, 13, 57, 241, 993, 4033};
    //    = {0, 1, 29, 249, 2'033, 16'353, 131'009};
    // = {0, 1, 830, 81'762, 7'008'614};
    auto rg = ReportGuard(true);
    for (size_t n = 2; n < 8; ++n) {
      Presentation<word_type> p;
      p.contains_empty_word(true);
      p.alphabet(n);
      Sims1_ S(congruence_kind::right);
      S.short_rules(p);
      REQUIRE(S.number_of_congruences(3) == num[n]);
    }

    // For n >= 1, a(n) is the number of deterministic, completely-defined,
    // initially-connected finite automata with n inputs and 3 unlabeled states.
    // A020522 counts similar automata with n inputs and 2 unlabeled states.

    // According to a comment by Nelma Moreira in A006689 and A006690, the
    // number of such automata with N inputs and M unlabeled states is Sum
    // (Product_{i=1..M-1} i^(f_i - f_{i-1} - 1)) * M^(M*N - f_{M-1} - 1), where
    // the sum is taken over integers f_1, ..., f_{M-1} satisfying 0 <= f_1 < N
    // and f_{i-1} < f_{i} < i*N for i = 2..M-1. (See Theorem 8 in Almeida,
    // Moreira, and Reis (2007). The value of f_0 is not relevant.) For this
    // sequence we have M = 3 unlabeled states, for A020522 we have M = 2
    // unlabeled states, for A006689 we have N = 2 inputs, and for A006690 we
    // have N = 3 inputs.
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1", "068", "RepOrc", "[quick][low-index]") {
    auto rg = ReportGuard(false);

    auto p = make<Presentation<word_type>>(TemperleyLieb(9));
    // There are no relations containing the empty word so we just manually
    // add it.
    p.contains_empty_word(true);
    RepOrc orc;
    // Check bad input
    auto d = orc.short_rules(p)
                 .min_nodes(100)
                 .max_nodes(90)
                 .target_size(4'862)
                 .digraph();
    REQUIRE(d.number_of_nodes() == 0);

    d = orc.short_rules(p)
            .min_nodes(80)
            .max_nodes(100)
            .target_size(4'862)
            .digraph();

    auto S = make<FroidurePin<Transf<0, node_type>>>(d);
    S.add_generator(S.generator(0).identity());
    REQUIRE(S.size() == 4'862);
    REQUIRE(orc.min_nodes() == 80);
    REQUIRE(orc.max_nodes() == 100);
    REQUIRE(orc.target_size() == 4'862);
    REQUIRE(orc.short_rules().rules.size() == 128);
    REQUIRE(orc.long_rules().rules.size() == 0);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "069",
                          "fp example 1",
                          "[quick][low-index]") {
    auto rg = ReportGuard(false);

    Presentation<word_type> p;
    p.contains_empty_word(true);
    p.alphabet({0, 1});
    presentation::add_rule_and_check(p, {0, 0, 0}, {0});
    presentation::add_rule_and_check(p, {1, 1}, {1});

    Presentation<word_type> q;
    q.contains_empty_word(true);
    q.alphabet({0, 1});

    presentation::add_rule_and_check(q, {0, 1, 0, 1}, {0});

    Sims1_ S(congruence_kind::right);
    REQUIRE(S.short_rules(p)
                .long_rules(q)
                .number_of_threads(1)
                .number_of_congruences(5)
            == 6);
    S.long_rule_length(5);
    REQUIRE(S.long_rules().rules.size() == 2);
    REQUIRE(S.short_rules().rules.size() == 4);
    S.long_rule_length(4);
    REQUIRE(S.long_rules().rules.size() == 4);
    REQUIRE(S.short_rules().rules.size() == 2);

    S = Sims1_(congruence_kind::left);
    REQUIRE(S.short_rules(p)
                .long_rules(q)
                .number_of_threads(1)
                .number_of_congruences(5)
            == 9);
  }

  LIBSEMIGROUPS_TEST_CASE(
      "Sims1",
      "070",
      "TemperleyLieb(3) - n = minimal rep (single-threaded, reporting on)",
      "[standard][sims1]") {
    auto rg = ReportGuard(true);

    for (size_t n = 3; n <= 3; ++n) {
      auto p = make<Presentation<word_type>>(TemperleyLieb(n));
      // There are no relations containing the empty word so we just manually
      // add it.
      p.contains_empty_word(true);
      auto d = MinimalRepOrc()
                   .short_rules(p)
                   .number_of_threads(1)
                   .target_size(5)
                   .digraph();
      REQUIRE(action_digraph_helper::is_strictly_cyclic(d));
      auto S = make<FroidurePin<Transf<0, node_type>>>(d);
      S.add_generator(S.generator(0).identity());
      REQUIRE(S.size() == 5);
      REQUIRE(d.number_of_nodes() == 4);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "071",
                          "FreeSemigroup(2) up to index 4",
                          "[quick][low-index]") {
    auto                    rg = ReportGuard(true);
    Presentation<word_type> p;
    p.contains_empty_word(true);
    p.alphabet(2);
    Sims1_ S(congruence_kind::right);
    S.short_rules(p);
    REQUIRE(S.number_of_congruences(4) == 5'477);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "072",
                          "SymmetricGroup(n) for n = 3, 4",
                          "[quick][low-index]") {
    std::array<uint64_t, 10> const num = {0, 0, 0, 6, 30, 156, 1'455};
    auto                           rg  = ReportGuard(false);
    for (size_t n = 3; n < 5; ++n) {
      auto p = make<Presentation<word_type>>(
          SymmetricGroup(n, author::Carmichael));
      Sims1_ C(congruence_kind::right);
      C.short_rules(p).number_of_threads(4);
      REQUIRE(C.number_of_congruences(factorial(n)) == num[n]);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "073",
                          "corner case no generators + no relations",
                          "[quick][low-index]") {
    Presentation<word_type> p;
    p.alphabet(0);
    Sims1_ S(congruence_kind::right);
    REQUIRE_THROWS_AS(S.short_rules(p), LibsemigroupsException);
    REQUIRE_THROWS_AS(S.number_of_congruences(1), LibsemigroupsException);
    REQUIRE_THROWS_AS(S.cbegin(2), LibsemigroupsException);
    REQUIRE_THROWS_AS(S.cend(2), LibsemigroupsException);
    REQUIRE_THROWS_AS(S.find_if(2, [](auto) { return true; }),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(S.for_each(2, [](auto) {}), LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "074",
                          "MonogenicSemigroup(m, r) for m, r = 1 .. 10",
                          "[fail][low-index]") {
    auto                                        rg = ReportGuard(false);
    std::vector<std::array<uint64_t, 11>> const num
        = {{1, 2, 2, 3, 2, 4, 2, 4, 3, 4},
           {2, 4, 4, 6, 4, 8, 4, 8, 6, 8},
           {3, 6, 6, 9, 6, 12, 6, 12, 9, 12},
           {4, 8, 8, 12, 8, 16, 8, 16, 12, 16},
           {5, 10, 10, 15, 10, 20, 10, 20, 15, 20},
           {6, 12, 12, 18, 12, 24, 12, 24, 18, 24},
           {7, 14, 14, 21, 14, 28, 14, 28, 21, 28},
           {8, 16, 16, 24, 16, 32, 16, 32, 24, 32},
           {9, 18, 18, 27, 18, 36, 18, 36, 27, 36},
           {10, 20, 20, 30, 20, 40, 20, 40, 30, 40}};

    // m * number of divisors of r

    for (size_t m = 1; m <= 10; ++m) {
      for (size_t r = 1; r <= 10; ++r) {
        // Cyclic groups
        auto p = make<Presentation<word_type>>(MonogenicSemigroup(m, r));

        Sims1_ C(congruence_kind::right);
        C.short_rules(p);
        // std::cout << C.number_of_congruences(m + r) << ", ";
        REQUIRE(C.number_of_congruences(m + r) == num[m - 1][r - 1]);
      }
    }
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "075",
                          "PartialTransformationMonoid(4)",
                          "[fail][low-index]") {
    auto rg = ReportGuard(true);
    auto p  = make<Presentation<word_type>>(
        PartialTransformationMonoid(4, author::Sutov));
    auto w = presentation::longest_common_subword(p);
    while (!w.empty()) {
      presentation::replace_subword(p, presentation::longest_common_subword(p));
      w = presentation::longest_common_subword(p);
    }

    presentation::sort_each_rule(p);
    presentation::sort_rules(p);
    presentation::remove_duplicate_rules(p);
    presentation::reduce_complements(p);
    presentation::remove_trivial_rules(p);

    do {
      auto it = presentation::redundant_rule(p, std::chrono::milliseconds(100));
      p.rules.erase(it, it + 2);
    } while (presentation::length(p) > 800);
    Sims1_ C(congruence_kind::left);
    C.short_rules(p).number_of_threads(4).report_interval(10);
    REQUIRE(C.number_of_congruences(624) == 0);
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
