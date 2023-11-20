//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2022-23 James D. Mitchell
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

#include "catch.hpp"  // for REQUIRE, REQUIRE_THROWS_AS, REQUI...
#include "libsemigroups/detail/report.hpp"
#include "test-main.hpp"  // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/bipart.hpp"           // for Bipartition
#include "libsemigroups/config.hpp"           // for LIBSEMIGROUPS_ENABLE_STATS
#include "libsemigroups/fpsemi-examples.hpp"  // for brauer_monoid etc
#include "libsemigroups/froidure-pin.hpp"     // for FroidurePin
#include "libsemigroups/gabow.hpp"            // for Gabow
#include "libsemigroups/knuth-bendix.hpp"     // for redundant_rule
#include "libsemigroups/sims1.hpp"            // for Sims1
#include "libsemigroups/to-froidure-pin.hpp"  // for make
#include "libsemigroups/to-presentation.hpp"  // for make
#include "libsemigroups/transf.hpp"           // for Transf
#include "libsemigroups/types.hpp"            // for word_type

namespace libsemigroups {

  using word_graph_type = typename Sims1::word_graph_type;
  using node_type       = typename word_graph_type::node_type;

  using fpsemigroup::author;
  using fpsemigroup::brauer_monoid;
  using fpsemigroup::chinese_monoid;
  using fpsemigroup::fibonacci_semigroup;
  using fpsemigroup::full_transformation_monoid;
  using fpsemigroup::monogenic_semigroup;
  using fpsemigroup::partition_monoid;
  using fpsemigroup::plactic_monoid;
  using fpsemigroup::rectangular_band;
  using fpsemigroup::singular_brauer_monoid;
  using fpsemigroup::stellar_monoid;
  using fpsemigroup::stylic_monoid;
  using fpsemigroup::symmetric_inverse_monoid;
  using fpsemigroup::temperley_lieb_monoid;
  using fpsemigroup::zero_rook_monoid;

  using namespace literals;

  namespace {
    // TODO check_exclude
    template <typename P>
    void check_include(congruence_kind               ck,
                       P const&                      p,
                       std::vector<word_type> const& e,
                       size_t                        n) {
      auto f = e;
      if (ck == congruence_kind::left) {
        std::for_each(f.begin(), f.end(), [](word_type& w) {
          std::reverse(w.begin(), w.end());
        });
      }
      auto foo = [&f](auto const& ad) {
        using word_graph::follow_path_no_checks;
        for (auto it = f.cbegin(); it != f.cend(); it += 2) {
          if (follow_path_no_checks(ad, 0, *it)
              != follow_path_no_checks(ad, 0, *(it + 1))) {
            return false;
          }
        }
        return true;
      };
      Sims1 S(ck);
      S.presentation(p);

      Sims1 T(ck);
      T.presentation(p).include(e);

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
    presentation::add_rule(p, {0, 0, 0}, {0});
    presentation::add_rule(p, {1, 1}, {1});
    presentation::add_rule(p, {0, 1, 0, 1}, {0});

    {
      Sims1 S(congruence_kind::right);
      REQUIRE(S.presentation(p).number_of_threads(2).number_of_congruences(5)
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
      REQUIRE(*it == to_word_graph<node_type>(1, {{0, 0}}));

      it = S.cbegin(5);
      REQUIRE(*(it++) == to_word_graph<node_type>(5, {{0, 0}}));
      REQUIRE(*(it++) == to_word_graph<node_type>(5, {{1, 0}, {1, 1}}));
      REQUIRE(*(it++) == to_word_graph<node_type>(5, {{1, 1}, {1, 1}}));
      REQUIRE(*(it++) == to_word_graph<node_type>(5, {{1, 2}, {1, 1}, {1, 2}}));
      REQUIRE(*(it++) == to_word_graph<node_type>(5, {{1, 2}, {1, 1}, {2, 2}}));
      REQUIRE(*(it++)
              == to_word_graph<node_type>(5, {{1, 2}, {1, 1}, {3, 2}, {3, 3}}));
      REQUIRE(*(it++) == WordGraph<node_type>(0, 2));
      REQUIRE(*(it++) == WordGraph<node_type>(0, 2));
      REQUIRE(*(it++) == WordGraph<node_type>(0, 2));

      it = S.cbegin(3);
      REQUIRE(*it == to_word_graph<node_type>(3, {{0, 0}}));
    }
    // [[[0, 0]],
    // [[1, 2], [1, 1], [3, 2], [3, 3]],
    // [[1, 2], [1, 1], [2, 2]],
    // [[1, 2], [1, 1], [1, 2]],
    // [[1, 1], [1, 1]],
    // [[1, 0], [1, 1]]]
    {
      Sims1 S(congruence_kind::left);
      REQUIRE(S.presentation(p).number_of_congruences(5) == 9);
      for (auto it = S.cbegin(5); it != S.cend(5); ++it) {
        REQUIRE(word_graph::follow_path_no_checks(*it, 0, {1, 0, 1, 0})
                == word_graph::follow_path_no_checks(*it, 0, {0}));
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
    presentation::add_rule(p, {0, 1, 0}, {0, 0});
    presentation::add_rule(p, {2, 2}, {0, 0});
    presentation::add_rule(p, {0, 0, 0}, {0, 0});
    presentation::add_rule(p, {2, 1}, {1, 2});
    presentation::add_rule(p, {2, 0}, {0, 0});
    presentation::add_rule(p, {1, 1}, {1});
    presentation::add_rule(p, {0, 2}, {0, 0});

    {
      Sims1 S(congruence_kind::right);
      S.presentation(p);
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
      REQUIRE(*(it++) == to_word_graph<node_type>(2, {{0, 0, 0}}));
      REQUIRE(*(it++) == to_word_graph<node_type>(2, {{1, 0, 1}, {1, 1, 1}}));
      REQUIRE(*(it++) == to_word_graph<node_type>(2, {{1, 1, 1}, {1, 1, 1}}));
      REQUIRE(*(it++) == WordGraph<node_type>(0, 3));
      REQUIRE(*(it++) == WordGraph<node_type>(0, 3));
    }
    {
      Sims1 S(congruence_kind::left);
      S.presentation(p);
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

    //          a  A  b  B  c  C  e
    p.alphabet({0, 1, 2, 3, 4, 5, 6});
    presentation::add_identity_rules(p, 6);
    presentation::add_inverse_rules(p, {1, 0, 3, 2, 5, 4, 6}, 6);
    presentation::add_rule(p, {0, 0, 5, 0, 4}, {6});
    presentation::add_rule(p, {0, 4, 2, 2, 1, 5, 2}, {6});
    presentation::add_rule(p, {1, 3, 0, 2, 4, 4, 4}, {6});
    Sims1 S(congruence_kind::right);
    S.presentation(p);
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
    presentation::add_rule(p, "aaCac", "e");
    presentation::add_rule(p, "acbbACb", "e");
    presentation::add_rule(p, "ABabccc", "e");
    Sims1 S(congruence_kind::right);
    S.presentation(p);
    REQUIRE(S.number_of_congruences(3) == 14);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "004",
                          "partition_monoid(2) right",
                          "[quick][low-index]") {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.contains_empty_word(false);

    p.alphabet(0123_w);
    presentation::add_identity_rules(p, 0);
    presentation::add_rule(p, 11_w, 0_w);
    presentation::add_rule(p, 13_w, 3_w);
    presentation::add_rule(p, 22_w, 2_w);
    presentation::add_rule(p, 31_w, 3_w);
    presentation::add_rule(p, 33_w, 3_w);
    presentation::add_rule(p, 232_w, 2_w);
    presentation::add_rule(p, 323_w, 3_w);
    presentation::add_rule(p, 1212_w, 212_w);
    presentation::add_rule(p, 2121_w, 212_w);

    Sims1 S(congruence_kind::right);
    S.presentation(p);
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

    MinimalRepOrc orc;
    auto          d = orc.presentation(p)
                 .target_size(15)
                 .number_of_threads(std::thread::hardware_concurrency())
                 .word_graph();

    REQUIRE(d.number_of_nodes() == 7);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "005",
                          "partition_monoid(3)",
                          "[standard][low-index][no-coverage]") {
    auto rg = ReportGuard(false);
    auto p  = partition_monoid(3, author::Machine);
    REQUIRE(!p.contains_empty_word());
    REQUIRE(p.alphabet() == word_type({0, 1, 2, 3, 4}));

    Sims1 S(congruence_kind::right);
    S.presentation(p).long_rule_length(11).number_of_threads(
        4);  // This actually helps here!
    REQUIRE(S.number_of_congruences(17) == 1589);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "006",
                          "full_transformation_monoid(3) right",
                          "[quick][low-index]") {
    auto                   rg = ReportGuard(false);
    FroidurePin<Transf<3>> S({Transf<3>::make({1, 2, 0}),
                              Transf<3>::make({1, 0, 2}),
                              Transf<3>::make({0, 1, 0})});
    REQUIRE(S.size() == 27);
    REQUIRE(S.number_of_generators() == 3);
    REQUIRE(S.number_of_rules() == 16);
    auto p = to_presentation<word_type>(S);
    REQUIRE(static_cast<size_t>(std::distance(p.rules.cbegin(), p.rules.cend()))
            == 2 * S.number_of_rules());
    Sims1 C(congruence_kind::right);
    C.presentation(p);
    REQUIRE(C.number_of_congruences(27) == 287);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "007",
                          "full_transformation_monoid(3) left",
                          "[quick][low-index]") {
    auto                   rg = ReportGuard(false);
    FroidurePin<Transf<3>> S(
        {Transf<3>({1, 2, 0}), Transf<3>({1, 0, 2}), Transf<3>({0, 1, 0})});
    REQUIRE(S.size() == 27);
    auto  p = to_presentation<word_type>(S);
    Sims1 C(congruence_kind::left);
    C.presentation(p);
    REQUIRE(C.number_of_congruences(27) == 120);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "008",
                          "full_transformation_monoid(4) left",
                          "[fail][low-index][babbage]") {
    auto rg = ReportGuard(true);

    auto p = full_transformation_monoid(4, author::Iwahori);

    REQUIRE(p.alphabet().size() == 4);

    auto w = presentation::longest_subword_reducing_length(p);
    while (!w.empty()) {
      presentation::replace_word_with_new_generator(p, w);
      w = presentation::longest_subword_reducing_length(p);
    }

    presentation::sort_each_rule(p);
    presentation::sort_rules(p);
    presentation::remove_duplicate_rules(p);
    presentation::reduce_complements(p);
    presentation::remove_trivial_rules(p);

    do {
      auto it = knuth_bendix::redundant_rule(p, std::chrono::milliseconds(100));
      p.rules.erase(it, it + 2);
    } while (presentation::length(p) > 700);

    Sims1 C(congruence_kind::right);
    C.presentation(p);
    // Takes about 1h31m to run!
    REQUIRE(C.number_of_threads(6).number_of_congruences(256) == 22'069'828);
    // Sims1 C(congruence_kind::left);
    // C.presentation(p);
    // REQUIRE(C.number_of_threads(6).number_of_congruences(256) == 120'121);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "009",
                          "symmetric_inverse_monoid(2, author::Gay)",
                          "[quick][low-index]") {
    auto  rg = ReportGuard(false);
    Sims1 C(congruence_kind::right);
    C.presentation(symmetric_inverse_monoid(2, author::Gay));
    REQUIRE(C.number_of_congruences(7) == 10);  // Should be 10
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "010",
                          "symmetric_inverse_monoid(2) from FroidurePin",
                          "[quick][low-index]") {
    auto                  rg = ReportGuard(false);
    FroidurePin<PPerm<2>> S({PPerm<2>({1, 0}), PPerm<2>({0}, {0}, 2)});
    REQUIRE(S.size() == 7);
    auto  p = to_presentation<word_type>(S);
    Sims1 C(congruence_kind::left);
    C.presentation(p);
    REQUIRE(C.number_of_congruences(7) == 10);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "011",
                          "symmetric_inverse_monoid(3)",
                          "[quick][low-index]") {
    auto  rg = ReportGuard(false);
    Sims1 C(congruence_kind::left);
    C.presentation(symmetric_inverse_monoid(3, author::Gay));
    REQUIRE(C.number_of_congruences(34) == 274);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "012",
                          "symmetric_inverse_monoid(4)",
                          "[extreme][low-index]") {
    auto p = symmetric_inverse_monoid(4, author::Gay);
    presentation::remove_duplicate_rules(p);
    presentation::sort_each_rule(p);
    presentation::sort_rules(p);

    REQUIRE(presentation::length(p) == 48);
    REQUIRE(p.alphabet().size() == 4);
    REQUIRE(*presentation::shortest_rule(p) == word_type({1, 1}));
    REQUIRE(*(presentation::shortest_rule(p) + 1) == word_type({}));
    REQUIRE(presentation::longest_rule_length(p) == 8);

    Sims1 C(congruence_kind::right);
    C.presentation(p).exclude(""_w, 11_w);
    REQUIRE(C.number_of_threads(2).number_of_congruences(209) == 0);
    C.clear_exclude();

    auto rg = ReportGuard(true);
    REQUIRE(C.number_of_threads(2).number_of_congruences(209) == 195'709);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "013",
                          "symmetric_inverse_monoid(5)",
                          "[fail][low-index]") {
    // This might take an extremely long time to terminate
    auto  rg = ReportGuard(true);
    Sims1 C(congruence_kind::left);
    C.presentation(symmetric_inverse_monoid(5, author::Gay));
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
                          "temperley_lieb_monoid(3) from presentation",
                          "[quick][low-index]") {
    auto rg = ReportGuard(false);
    {
      Sims1 S(congruence_kind::right);
      S.presentation(temperley_lieb_monoid(3));
      REQUIRE(S.number_of_congruences(14) == 9);
    }
    {
      Sims1 S(congruence_kind::left);
      S.presentation(temperley_lieb_monoid(3));
      REQUIRE(S.number_of_congruences(14) == 9);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "015",
                          "temperley_lieb_monoid(4) from presentation",
                          "[quick][low-index]") {
    auto rg = ReportGuard(false);
    {
      Sims1 S(congruence_kind::right);
      S.presentation(temperley_lieb_monoid(4));
      REQUIRE(S.number_of_congruences(14) == 79);
    }
    {
      Sims1 S(congruence_kind::left);
      S.presentation(temperley_lieb_monoid(4));
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
    presentation::add_rule(p, 000_w, 0_w);
    presentation::add_rule(p, 11_w, 1_w);
    presentation::add_rule(p, 0101_w, 0_w);

    std::vector<word_type> e = {0_w, 1_w};

    Sims1 S(congruence_kind::right);
    S.presentation(p).include(e);
    REQUIRE(S.number_of_congruences(5) == 2);
    check_include(congruence_kind::right, p, e, 5);
    check_include(congruence_kind::left, p, e, 5);
    S.exclude(e).clear_include();
    REQUIRE(S.number_of_congruences(5) == 4);
    S.clear_exclude();
    REQUIRE(S.number_of_congruences(5) == 6);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "017",
                          "fp semigroup containing given pairs #2",
                          "[quick][low-index]") {
    auto rg = ReportGuard(false);

    Presentation<word_type> p;
    p.contains_empty_word(true);

    p.alphabet({0, 1});
    presentation::add_rule(p, 000_w, 0_w);
    presentation::add_rule(p, 11_w, 1_w);
    presentation::add_rule(p, 0101_w, 0_w);
    {
      Sims1 T(congruence_kind::right);
      T.presentation(p).include(01_w, 1_w);
      REQUIRE(T.number_of_congruences(5) == 2);
      check_include(congruence_kind::right, p, T.include(), 5);
    }
    {
      Sims1 T(congruence_kind::left);
      T.presentation(p).include(01_w, 1_w);
      REQUIRE(T.number_of_congruences(5) == 2);
      check_include(congruence_kind::left, p, T.include(), 5);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "018",
                          "fp semigroup containing given pairs #3",
                          "[quick][low-index]") {
    auto rg = ReportGuard(false);

    Presentation<word_type> p;
    p.contains_empty_word(true);
    p.alphabet({0, 1});
    presentation::add_rule(p, 000_w, 0_w);
    presentation::add_rule(p, 11_w, 1_w);
    presentation::add_rule(p, 0101_w, 0_w);

    {
      Sims1 T(congruence_kind::right);
      T.presentation(p).include(0101_w, 0_w);
      REQUIRE(T.number_of_congruences(5) == 6);
    }
    {
      Sims1 T(congruence_kind::left);
      T.presentation(p).include(0101_w, 0_w);
      REQUIRE(T.include() == std::vector<word_type>({1010_w, 0_w}));
      REQUIRE(T.number_of_congruences(5) == 9);  // Verified with GAP
    }
    check_include(congruence_kind::right, p, {0101_w, 0_w}, 5);
    check_include(congruence_kind::left, p, {0101_w, 0_w}, 5);
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
    presentation::add_rule(p, "aaCac", "e");
    presentation::add_rule(p, "acbbACb", "e");
    presentation::add_rule(p, "ABabccc", "e");

    Sims1 S(congruence_kind::right);
    S.presentation(p).include(to_word(p, "a"), to_word(p, "A"));
    S.presentation(p).include(to_word(p, "a"), to_word(p, "b"));
    REQUIRE(S.number_of_congruences(3) == 2);

    check_include(congruence_kind::right, S.presentation(), S.include(), 3);
    check_include(congruence_kind::left, S.presentation(), S.include(), 3);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "020",
                          "fp example 2",
                          "[quick][low-index]") {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.contains_empty_word(true);

    p.alphabet({0, 1, 2});
    presentation::add_rule(p, 010_w, 00_w);
    presentation::add_rule(p, 22_w, 00_w);
    presentation::add_rule(p, 000_w, 00_w);
    presentation::add_rule(p, 21_w, 12_w);
    presentation::add_rule(p, 20_w, 00_w);
    presentation::add_rule(p, 11_w, 1_w);
    presentation::add_rule(p, 02_w, 00_w);

    std::vector<word_type> e = {1_w, 00_w};
    check_include(congruence_kind::right, p, e, 11);
    check_include(congruence_kind::left, p, e, 11);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1", "021", "exceptions", "[quick][low-index]") {
    Presentation<word_type> p;
    p.alphabet({0, 1, 2});
    presentation::add_rule(p, {0, 1, 0}, {0, 0});

    Presentation<word_type> e;
    e.alphabet({0, 1});
    // REQUIRE_THROWS_AS(Sims1(congruence_kind::right).presentation(p).include(e),
    //                  LibsemigroupsException);
    // REQUIRE_THROWS_AS(
    //     Sims1(congruence_kind::right).presentation(p).long_rules(e),
    //     LibsemigroupsException);
    // REQUIRE_THROWS_AS(
    //     Sims1(congruence_kind::right).long_rules(p).presentation(e),
    //     LibsemigroupsException);
    // REQUIRE_THROWS_AS(Sims1(congruence_kind::right).long_rules(p).include(e),
    //                  LibsemigroupsException);

    // REQUIRE_THROWS_AS(Sims1(congruence_kind::right).include(p).presentation(e),
    //                   LibsemigroupsException);
    // REQUIRE_THROWS_AS(Sims1(congruence_kind::right).include(p).long_rules(e),
    //                  LibsemigroupsException);
    // REQUIRE_NOTHROW(Sims1(congruence_kind::right).include(p).include(e));
    REQUIRE_NOTHROW(
        Sims1(congruence_kind::right).presentation(p).presentation(e));
    // REQUIRE_NOTHROW(Sims1(congruence_kind::right).long_rules(p).long_rules(e));
    REQUIRE_THROWS_AS(Sims1(congruence_kind::twosided), LibsemigroupsException);
    Sims1 S(congruence_kind::right);
    REQUIRE_THROWS_AS(S.number_of_threads(0), LibsemigroupsException);
    RepOrc ro;
    REQUIRE_THROWS_AS(ro.number_of_threads(0), LibsemigroupsException);
    MinimalRepOrc mro;
    REQUIRE_THROWS_AS(mro.number_of_threads(0), LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "022",
                          "singular_brauer_monoid(4) (Maltcev-Mazorchuk)",
                          "[extreme][sims1]") {
    auto rg = ReportGuard(true);
    auto p  = singular_brauer_monoid(4);
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
    auto          d = orc.presentation(p)
                 .target_size(82)
                 .number_of_threads(std::thread::hardware_concurrency())
                 .word_graph();
    REQUIRE(d.number_of_nodes() == 18);
    REQUIRE(orc.target_size() == 82);

    p.contains_empty_word(false);

    Sims1 C(congruence_kind::right);
    C.presentation(p);
    REQUIRE(C.presentation().rules.size() == 186);

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

    auto p = to_presentation<word_type>(S);
    REQUIRE(presentation::length(p) == 359);
    presentation::remove_duplicate_rules(p);
    REQUIRE(presentation::length(p) == 359);
    presentation::reduce_complements(p);
    REQUIRE(presentation::length(p) == 359);
    presentation::sort_each_rule(p);
    presentation::sort_rules(p);
    REQUIRE(p.rules.size() == 86);

    Sims1 C(congruence_kind::right);
    C.presentation(p).long_rule_length(12);
    REQUIRE(C.number_of_threads(4).number_of_congruences(105) == 103'406);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "024",
                          "brauer_monoid(4) (Kudryavtseva-Mazorchuk)",
                          "[extreme][sims1]") {
    auto rg = ReportGuard(true);
    auto p  = brauer_monoid(4);
    REQUIRE(p.alphabet().size() == 6);
    REQUIRE(presentation::length(p) == 140);
    presentation::remove_duplicate_rules(p);
    REQUIRE(presentation::length(p) == 120);
    presentation::reduce_complements(p);
    REQUIRE(presentation::length(p) == 117);
    presentation::sort_each_rule(p);
    presentation::sort_rules(p);
    REQUIRE(p.rules.size() == 60);

    // auto d = MinimalRepOrc().presentation(p).target_size(105).word_graph();
    // REQUIRE(d.number_of_nodes() == 22);
    // REQUIRE(word_graph::is_strictly_cyclic(d));
    // REQUIRE(
    //     d
    //     == to_word_graph<uint32_t>(
    //         22, {{0, 0, 1, 0, 2, 3, 2},        {1, 4, 0, 5, 6, 3, 7},
    //              {2, 2, 2, 2, 2, 2, 2},        {3, 8, 3, 9, 6, 3, 7},
    //              {4, 1, 4, 10, 6, 2, 11},      {5, 10, 5, 1, 12, 2, 7},
    //              {6, 6, 8, 12, 6, 3, 13},      {7, 11, 9, 7, 13, 3, 7},
    //              {8, 3, 6, 14, 6, 3, 11},      {9, 14, 7, 3, 12, 3, 7},
    //              {10, 5, 15, 4, 12, 16, 11},   {11, 7, 17, 11, 13, 16, 11},
    //              {12, 12, 18, 6, 12, 16, 13},  {13, 13, 19, 13, 13, 20,
    //              13}, {14, 9, 21, 8, 12, 20, 11},   {15, 15, 10, 15, 2, 16,
    //              2}, {16, 18, 16, 17, 12, 16, 11}, {17, 21, 11, 16, 6, 16,
    //              11}, {18, 16, 12, 21, 12, 16, 7},  {19, 20, 13, 20, 13,
    //              20, 13}, {20, 19, 20, 19, 13, 20, 13}, {21, 17, 14, 18, 6,
    //              20, 7}}));

    // auto S = to_froidure_pin<Transf<0, node_type>>(d);
    // REQUIRE(S.size() == 105);
    // REQUIRE(S.generator(0) == Transf<0, node_type>::identity(22));
    // REQUIRE(
    //     S.generator(1)
    //     == Transf<0, node_type>({0, 4,  2,  8, 1,  10, 6,  11, 3,  14, 5,
    //                              7, 12, 13, 9, 15, 18, 21, 16, 20, 19,
    //                              17}));
    // REQUIRE(
    //     S.generator(2)
    //     == Transf<0, node_type>({1,  0,  2,  3,  4,  5,  8,  9,  6,  7, 15,
    //                              17, 18, 19, 21, 10, 16, 11, 12, 13, 20,
    //                              14}));
    // REQUIRE(S.generator(3)
    //         == Transf<0, node_type>({0, 5,  2, 9,  10, 1,  12, 7,  14, 3,
    //         4, 11,
    //                                  6, 13, 8, 15, 17, 16, 21, 20, 19,
    //                                  18}));
    // REQUIRE(S.generator(4)
    //         == Transf<0, node_type>({2,  6,  2,  6,  6, 12, 6, 13, 6,  12,
    //         12,
    //                                  13, 12, 13, 12, 2, 12, 6, 12, 13, 13,
    //                                  6}));
    // REQUIRE(
    //     S.generator(5)
    //     == Transf<0, node_type>({3,  3,  2,  3,  2,  2,  3,  3,  3,  3, 16,
    //                              16, 16, 20, 20, 16, 16, 16, 16, 20, 20,
    //                              20}));
    // REQUIRE(
    //     S.generator(6)
    //     == Transf<0, node_type>({2,  7,  2,  7,  11, 7,  13, 7, 11, 7,  11,
    //                              11, 13, 13, 11, 2,  11, 11, 7, 13, 13,
    //                              7}));

    Sims1 C(congruence_kind::right);
    C.presentation(p);
    REQUIRE(C.number_of_threads(std::thread::hardware_concurrency())
                .number_of_congruences(105)
            == 103'406);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "025",
                          "brauer_monoid(5) (Kudryavtseva-Mazorchuk)",
                          "[extreme][sims1]") {
    auto rg = ReportGuard(true);
    auto p  = brauer_monoid(5);

    REQUIRE(presentation::length(p) == 240);

    presentation::remove_duplicate_rules(p);
    presentation::reduce_complements(p);
    presentation::sort_each_rule(p);
    presentation::sort_rules(p);
    REQUIRE(presentation::length(p) == 194);

    REQUIRE(presentation::shortest_rule_length(p) == 2);
    REQUIRE(*presentation::shortest_rule(p) == 00_w);
    REQUIRE(*(presentation::shortest_rule(p) + 1) == ""_w);

    REQUIRE(p.alphabet().size() == 8);

    presentation::remove_redundant_generators(p);
    REQUIRE(p.alphabet() == 01234567_w);
    REQUIRE(p.alphabet().size() == 8);
    REQUIRE(presentation::length(p) == 194);

    REQUIRE(*presentation::longest_rule(p) == 101_w);
    REQUIRE(*(presentation::longest_rule(p) + 1) == 010_w);

    REQUIRE(presentation::longest_subword_reducing_length(p) == 76_w);
    presentation::replace_word_with_new_generator(p, 76_w);
    REQUIRE(presentation::length(p) == 193);

    auto d = MinimalRepOrc()
                 .presentation(p)
                 .include(0_w, 1_w)
                 .target_size(945)
                 .number_of_threads(8)
                 .word_graph();
    // WARNING: the number below is not necessarily the minimal degree of an
    // action on right congruences, only the minimal degree of an action on
    // right congruences containing the pair {0}, {1}.
    REQUIRE(d.number_of_nodes() == 51);
    auto S = to_froidure_pin<Transf<0, node_type>>(d);
    REQUIRE(S.size() == 945);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "026",
                          "uniform_block_bijection_monoid(4) (Fitzgerald)",
                          "[extreme][sims1]") {
    auto rg = ReportGuard(true);
    auto p  = uniform_block_bijection_monoid(4, author::FitzGerald);
    presentation::remove_duplicate_rules(p);
    presentation::reduce_complements(p);
    presentation::sort_each_rule(p);
    presentation::sort_rules(p);

    Sims1 C(congruence_kind::right);
    C.presentation(p);
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
    presentation::add_rule(p, "aaa", "e");
    presentation::add_rule(p, "baBBBABA", "e");
    Sims1 C(congruence_kind::right);
    C.presentation(p);
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
    presentation::add_rule(p, "aaa", "");
    presentation::add_rule(p, "baBBBABA", "");
    Sims1 C(congruence_kind::right);
    C.presentation(p);
    REQUIRE(C.number_of_congruences(10) == 3);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "029",
                          "fibonacci_semigroup(4, 6)",
                          "[standard][sims1][no-valgrind]") {
    auto rg = ReportGuard(true);  // for code coverage
    auto p  = fibonacci_semigroup(4, 6);
    presentation::remove_duplicate_rules(p);
    presentation::sort_each_rule(p);
    presentation::sort_rules(p);
    REQUIRE(presentation::length(p) == 30);
    REQUIRE(p.rules.size() == 12);
    REQUIRE(p.rules[0].size() + p.rules[1].size() == 5);

    Sims1 C(congruence_kind::right);
    C.presentation(p);
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
    presentation::add_rule_no_checks(p, {1, 2, 1}, {1, 1});
    presentation::add_rule_no_checks(p, {3, 3}, {1, 1});
    presentation::add_rule_no_checks(p, {1, 1, 1}, {1, 1});
    presentation::add_rule_no_checks(p, {3, 2}, {2, 3});
    presentation::add_rule_no_checks(p, {3, 1}, {1, 1});
    presentation::add_rule_no_checks(p, {2, 2}, {2});
    presentation::add_rule_no_checks(p, {1, 3}, {1, 1});
    p.validate();
    Sims1 C(congruence_kind::right);
    C.presentation(p);
    REQUIRE(C.number_of_congruences(2) == 67);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "031",
                          "presentation with non-zero index generators",
                          "[quick][sims1]") {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    presentation::add_rule_no_checks(p, {1, 2, 1}, {1, 1});
    presentation::add_rule_no_checks(p, {3, 3}, {1, 1});
    presentation::add_rule_no_checks(p, {1, 1, 1}, {1, 1});
    presentation::add_rule_no_checks(p, {3, 2}, {2, 3});
    presentation::add_rule_no_checks(p, {3, 1}, {1, 1});
    presentation::add_rule_no_checks(p, {2, 2}, {2});
    presentation::add_rule_no_checks(p, {1, 3}, {1, 1});
    p.alphabet_from_rules();
    p.validate();

    Sims1 C(congruence_kind::right);
    C.presentation(p);
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
    presentation::add_rule(p, {0, 0, 5, 0, 4}, {});
    presentation::add_rule(p, {0, 4, 2, 2, 1, 5, 2}, {});
    presentation::add_rule(p, {1, 3, 0, 2, 4, 4, 4}, {});
    Sims1 S(congruence_kind::right);
    S.presentation(p);
    REQUIRE(S.number_of_congruences(3) == 14);
    REQUIRE(S.number_of_congruences(4) == 14);
    REQUIRE(S.number_of_congruences(5) == 14);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1", "033", "constructors", "[quick][sims1]") {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.contains_empty_word(true);

    //         aAbBcC
    p.alphabet(012345_w);
    presentation::add_inverse_rules(p, 103254_w);
    presentation::add_rule(p, 00504_w, {});
    presentation::add_rule(p, 0422152_w, {});
    presentation::add_rule(p, 1302444_w, {});
    Sims1 S(congruence_kind::right);
    S.presentation(p);

    Sims1 T(S);
    REQUIRE(S.number_of_congruences(3) == 14);
    REQUIRE(T.number_of_congruences(3) == 14);

    Sims1 U(std::move(S));
    REQUIRE(U.number_of_congruences(3) == 14);
    REQUIRE(T.number_of_congruences(3) == 14);

    S = U;
    REQUIRE(S.number_of_congruences(3) == 14);

    S = std::move(U);
    REQUIRE(S.number_of_congruences(3) == 14);

    Sims1 C(congruence_kind::right);
    REQUIRE_THROWS_AS(C.presentation(p).include(0127_w, 0_w),
                      LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "034",
                          "cbegin_long_rules",
                          "[quick][sims1]") {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.contains_empty_word(true);

    //         aAbBcC
    p.alphabet(012345_w);
    presentation::add_inverse_rules(p, 103254_w);
    presentation::add_rule(p, 00504_w, {});
    presentation::add_rule(p, 0422152_w, {});
    presentation::add_rule(p, 1302444_w, {});
    // F := FreeGroup("a", "b", "c");
    // AssignGeneratorVariables(F);
    // R := [ a*a*c^-1*a*c, a*c*b*b*a^-1*c^-1*b, a^-1*b^-1*a*b*c*c*c];
    // G := F/ R;
    // LowIndexSubgroups(G, 3); # returns 14
    // Sims1 S(congruence_kind::right);
    // S.presentation(p);
    // REQUIRE(S.number_of_congruences(3) == 14);
    // REQUIRE(S.number_of_congruences(4) == 14);

    // REQUIRE_THROWS_AS(S.cbegin_long_rules(p.rules.size() + 1),
    //                   LibsemigroupsException);
    // REQUIRE_THROWS_AS(S.cbegin_long_rules(9), LibsemigroupsException);
    // REQUIRE_NOTHROW(S.cbegin_long_rules(0));

    // REQUIRE(!S.presentation().rules.empty());

    // auto const& rules = S.presentation().rules;

    // for (size_t i = 0; i <= rules.size() / 2; ++i) {
    //   S.cbegin_long_rules(2 * i);
    //   REQUIRE(static_cast<size_t>(
    //               std::distance(rules.cbegin(), S.cbegin_long_rules()))
    //           == 2 * i);
    // }
    // REQUIRE(S.presentation().rules.size() == p.rules.size());
    // for (size_t i = p.rules.size() / 2; i > 0; --i) {
    //   S.cbegin_long_rules(2 * i);
    //   REQUIRE(static_cast<size_t>(
    //               std::distance(rules.cbegin(), S.cbegin_long_rules()))
    //           == 2 * i);
    // }

    Presentation<word_type> q;
    q.contains_empty_word(p.contains_empty_word()).alphabet(p.alphabet());
    q.rules.insert(q.rules.end(), p.rules.begin(), p.rules.begin() + 8);
    q.validate();
    REQUIRE(q.alphabet() == 012345_w);
    REQUIRE(
        q.rules
        == std::vector<word_type>({01_w, {}, 10_w, {}, 23_w, {}, 32_w, {}}));
    q.validate();

    std::atomic_uint64_t num = 0;
    Sims1                T(congruence_kind::right);
    T.presentation(q);

    REQUIRE(T.number_of_long_rules() == 0);
    T.for_each(3, [&](auto const& wg) {
      num += word_graph::is_compatible(wg,
                                       wg.cbegin_nodes(),
                                       wg.cbegin_nodes()
                                           + wg.number_of_active_nodes(),
                                       p.rules.cbegin(),
                                       p.rules.cend());
    });
    REQUIRE(num == 14);  // 14 is the correct value

    // num = 0;

    // // REQUIRE(rules.size() == 18);
    // // REQUIRE(S.number_of_long_rules() == 8);
    // S.number_of_threads(1);
    // REQUIRE(S.number_of_congruences(3) == 5);
    // REQUIRE(rules.size() == 18);
    // S.for_each(3, [&](auto const& wg) {
    //   REQUIRE(wg.out_degree() == 6);
    //   num += word_graph::is_compatible(wg,
    //                                    wg.cbegin_nodes(),
    //                                    wg.cbegin_nodes()
    //                                        + wg.number_of_active_nodes(),
    //                                    rules.cbegin(),
    //                                    S.cbegin_long_rules())
    //          && word_graph::is_complete(wg,
    //                                     wg.cbegin_nodes(),
    //                                     wg.cbegin_nodes()
    //                                         + wg.number_of_active_nodes());
    // });
    // REQUIRE(S.presentation().rules == p.rules);
    // REQUIRE(num == 5);
    // S.clear_long_rules();
    // num = 0;
    // REQUIRE(rules.size() == 18);
    // REQUIRE(S.number_of_threads() == 1);
    // REQUIRE(S.presentation().rules == p.rules);
    // REQUIRE(S.number_of_congruences(3) == 14);
    // S.for_each(3, [&](auto const& wg) {
    //   REQUIRE(wg.out_degree() == 6);
    //   num += word_graph::is_compatible(wg,
    //                                    wg.cbegin_nodes(),
    //                                    wg.cend_nodes(),
    //                                    rules.cbegin(),
    //                                    S.cbegin_long_rules())
    //          && word_graph::is_complete(wg,
    //                                     wg.cbegin_nodes(),
    //                                     wg.cbegin_nodes()
    //                                         + wg.number_of_active_nodes());
    // });
    // REQUIRE(num == 14);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1", "035", "stats", "[quick][sims1]") {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.contains_empty_word(true);

    //          a  A  b  B  c  C
    p.alphabet({0, 1, 2, 3, 4, 5});
    presentation::add_inverse_rules(p, {1, 0, 3, 2, 5, 4});
    presentation::add_rule(p, {0, 0, 5, 0, 4}, {});
    presentation::add_rule(p, {0, 4, 2, 2, 1, 5, 2}, {});
    presentation::add_rule(p, {1, 3, 0, 2, 4, 4, 4}, {});
    Sims1 S(congruence_kind::right);
    S.presentation(p);

    std::stringbuf buff;
    std::ostream   os(&buff);
    static_cast<void>(S.number_of_congruences(2));
    REQUIRE(S.stats().max_pending != 0);
    // Withdrawn in v3
    // os << S.cbegin(3).stats();  // Also does not do anything visible
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "036",
                          "check iterator requirements",
                          "[quick][sims1]") {
    Presentation<word_type> p;
    p.contains_empty_word(true);
    p.alphabet({0, 1});
    presentation::add_rule(p, {0, 0, 0}, {0});
    presentation::add_rule(p, {1, 1}, {1});
    presentation::add_rule(p, {0, 1, 0, 1}, {0});

    {
      Sims1 S(congruence_kind::right);
      S.presentation(p);
      verify_forward_iterator_requirements(S.cbegin(10));
      auto it = S.cbegin(10);
      REQUIRE(it->number_of_nodes() == 10);
    }
    {
      Sims1 S(congruence_kind::left);
      S.presentation(p);
      verify_forward_iterator_requirements(S.cbegin(10));
      auto it = S.cbegin(10);
      REQUIRE(it->number_of_nodes() == 10);
    }
  }

  // Takes about 4s
  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "037",
                          "rectangular_band(9, 2)",
                          "[extreme][sims1]") {
    auto rg = ReportGuard(true);
    auto p  = rectangular_band(9, 2);
    presentation::remove_duplicate_rules(p);
    presentation::sort_each_rule(p);
    presentation::sort_rules(p);

    REQUIRE(MinimalRepOrc()
                .presentation(p)
                .target_size(18)
                .number_of_threads(std::thread::hardware_concurrency())
                .word_graph()
                .number_of_nodes()
            == 0);
    p.contains_empty_word(true);
    auto mro
        = MinimalRepOrc().presentation(p).target_size(19).number_of_threads(
            std::thread::hardware_concurrency());
    auto d = mro.word_graph();
    REQUIRE(d.number_of_nodes() == 11);
    REQUIRE(word_graph::is_strictly_cyclic(d));
    auto S = to_froidure_pin<Transf<0, node_type>>(d);
    S.add_generator(S.generator(0).identity());
    REQUIRE(S.size() == 19);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "038",
                          "partition_monoid(3) - minimal o.r.c. rep",
                          "[extreme][sims1]") {
    auto rg = ReportGuard(true);
    auto p  = partition_monoid(3, author::Machine);
    REQUIRE(!p.contains_empty_word());
    REQUIRE(p.alphabet() == word_type({0, 1, 2, 3, 4}));

    auto d = RepOrc()
                 .presentation(p)
                 .target_size(203)
                 .min_nodes(1)
                 .max_nodes(22)
                 .number_of_threads(2)
                 .word_graph();
    REQUIRE(d.number_of_nodes() == 22);

    auto mro
        = MinimalRepOrc().presentation(p).target_size(203).number_of_threads(4);
    d = mro.word_graph();

    REQUIRE(word_graph::is_strictly_cyclic(d));
    auto S = to_froidure_pin<Transf<0, node_type>>(d);
    REQUIRE(S.size() == 203);
    // The actual digraph obtained is non-deterministic because we just take
    // whichever one is found first, in multiple threads
    // REQUIRE(
    //     d
    //     == to_word_graph<node_type>(
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

    std::vector<WordGraph<uint32_t>> all;

    auto hook = [&](WordGraph<uint32_t> const& x) {
      auto first = 1;
      auto SS    = to_froidure_pin<Transf<0, node_type>>(
          x, first, x.number_of_active_nodes());
      SuppressReportFor supp("FroidurePin");

      if (SS.size() == 203) {
        return all.push_back(x);
      }
    };

    auto SS = Sims1(congruence_kind::right).presentation(p);

    SS.for_each(22, hook);
    REQUIRE(all.size() == 24);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "039",
                          "temperley_lieb_monoid(n) - n = 3 .. 6, minimal rep",
                          "[standard][sims1]") {
    auto rg = ReportGuard(false);

    std::array<uint64_t, 11> const sizes
        = {0, 1, 2, 5, 14, 42, 132, 429, 1'430, 4'862, 16'796};
    std::array<uint64_t, 11> const min_degrees
        = {0, 0, 2, 4, 7, 10, 20, 29, 63, 91, 208};
    // The values 63 and 91 are not verified

    for (size_t n = 3; n <= 6; ++n) {
      auto p = temperley_lieb_monoid(n);
      // There are no relations containing the empty word so we just manually
      // add it.
      p.contains_empty_word(true);
      auto orc
          = MinimalRepOrc().presentation(p).number_of_threads(2).target_size(
              sizes[n]);

      auto d = orc.word_graph();
      REQUIRE(orc.target_size() == sizes[n]);
      REQUIRE(word_graph::is_strictly_cyclic(d));
      auto S = to_froidure_pin<Transf<0, node_type>>(d);
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
    presentation::add_rule(p, {0, 0}, {});
    presentation::add_rule(p, {1, 1}, {});
    presentation::add_rule(p, {2, 2}, {});
    presentation::add_rule(p, {3, 3}, {});
    presentation::add_rule(p, {4, 4}, {});
    presentation::add_rule(p, {0, 1, 0, 1, 0, 1}, {});
    presentation::add_rule(p, {0, 2, 0, 2}, {});
    presentation::add_rule(p, {0, 3, 0, 3}, {});
    presentation::add_rule(p, {0, 4, 0, 4}, {});
    presentation::add_rule(p, {1, 2, 1, 2, 1, 2}, {});
    presentation::add_rule(p, {1, 3, 1, 3}, {});
    presentation::add_rule(p, {1, 4, 1, 4}, {});
    presentation::add_rule(p, {2, 3, 2, 3, 2, 3}, {});
    presentation::add_rule(p, {2, 4, 2, 4}, {});
    presentation::add_rule(p, {3, 4, 3, 4, 3, 4}, {});
    REQUIRE(MinimalRepOrc()
                .presentation(p)
                .target_size(0)
                .word_graph()
                .number_of_nodes()
            == 0);

    REQUIRE(RepOrc()
                .presentation(p)
                .min_nodes(0)
                .max_nodes(0)
                .target_size(0)
                .word_graph()
                .number_of_nodes()
            == 0);

    auto d = MinimalRepOrc().presentation(p).target_size(720).word_graph();
    REQUIRE(d.number_of_nodes() == 6);
    REQUIRE(word_graph::is_strictly_cyclic(d));
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "041",
                          "rectangular_band(4, 4) - minimal o.r.c. rep",
                          "[standard][sims1]") {
    auto rg = ReportGuard(false);
    auto p  = rectangular_band(4, 4);
    p.contains_empty_word(true);
    auto d = MinimalRepOrc()
                 .presentation(p)
                 .number_of_threads(2)
                 .target_size(17)
                 .word_graph();
    REQUIRE(word_graph::is_strictly_cyclic(d));
    auto S = to_froidure_pin<Transf<0, node_type>>(d);
    REQUIRE(S.size() == 16);
    REQUIRE(d.number_of_nodes() == 7);

    p.contains_empty_word(false);
    d = MinimalRepOrc()
            .presentation(p)
            .target_size(16)
            .number_of_threads(2)
            .word_graph();
    REQUIRE(d.number_of_nodes() == 0);
  }

  // unbuffer -p ./test_sims1 "[042]" | ag -v FroidurePin
  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "042",
                          "rectangular_band(m, n) - m = 1 .. 5, n = 1 .. 5",
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

        auto p = rectangular_band(m, n);
        p.contains_empty_word(true);
        auto d = MinimalRepOrc()
                     .presentation(p)
                     .target_size(m * n + 1)
                     .number_of_threads(6)
                     .word_graph();
        REQUIRE(word_graph::is_strictly_cyclic(d));
        auto S = to_froidure_pin<Transf<0, node_type>>(d);
        REQUIRE(S.size() == m * n);
        REQUIRE(d.number_of_nodes() == results[m][n]);
      }
    }
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "043",
                          "rectangular_band(2, 2) - with and without identity",
                          "[quick][sims1]") {
    auto rg = ReportGuard(false);
    auto p  = rectangular_band(2, 2);
    REQUIRE(!p.contains_empty_word());
    Sims1 S(congruence_kind::right);
    S.presentation(p);

    REQUIRE(S.number_of_congruences(4) == 6);

    p.contains_empty_word(true);

    Sims1 T(congruence_kind::right);
    T.presentation(p);
    REQUIRE(T.number_of_congruences(5) == 9);

    auto it = S.cbegin(4);

    REQUIRE(
        *it++
        == to_word_graph<node_type>(5, {{1, 1, 1, 1}, {1, 1, 1, 1}}));  // Good
    REQUIRE(*it++
            == to_word_graph<node_type>(
                5, {{1, 1, 1, 2}, {1, 1, 1, 2}, {1, 1, 1, 2}}));  // Good
    REQUIRE(*it++
            == to_word_graph<node_type>(
                5, {{1, 2, 1, 1}, {1, 1, 1, 1}, {2, 2, 2, 2}}));  // Good
    REQUIRE(
        *it++
        == to_word_graph<node_type>(
            5,
            {{1, 2, 1, 1}, {1, 1, 1, 1}, {2, 2, 2, 3}, {2, 2, 2, 3}}));  // Good
    REQUIRE(
        *it++
        == to_word_graph<node_type>(
            5,
            {{1, 2, 1, 3}, {1, 1, 1, 3}, {2, 2, 2, 2}, {1, 1, 1, 3}}));  // Good
    REQUIRE(*it++
            == to_word_graph<node_type>(5,
                                        {{1, 2, 1, 3},
                                         {1, 1, 1, 3},
                                         {2, 2, 2, 4},
                                         {1, 1, 1, 3},
                                         {2, 2, 2, 4}}));  // Good
    REQUIRE(it->number_of_nodes() == 0);

    it = T.cbegin(5);

    REQUIRE(*it++ == to_word_graph<node_type>(5, {{0, 0, 0, 0}}));
    REQUIRE(*it++ == to_word_graph<node_type>(5, {{0, 0, 0, 1}, {0, 0, 0, 1}}));
    REQUIRE(*it++ == to_word_graph<node_type>(5, {{1, 1, 1, 0}, {1, 1, 1, 0}}));
    REQUIRE(*it++ == to_word_graph<node_type>(5, {{1, 1, 1, 1}, {1, 1, 1, 1}}));
    REQUIRE(*it++
            == to_word_graph<node_type>(
                5, {{1, 1, 1, 2}, {1, 1, 1, 2}, {1, 1, 1, 2}}));
    REQUIRE(*it++
            == to_word_graph<node_type>(
                5, {{1, 2, 1, 1}, {1, 1, 1, 1}, {2, 2, 2, 2}}));
    REQUIRE(*it++
            == to_word_graph<node_type>(
                5, {{1, 2, 1, 1}, {1, 1, 1, 1}, {2, 2, 2, 3}, {2, 2, 2, 3}}));
    REQUIRE(*it++
            == to_word_graph<node_type>(
                5, {{1, 2, 1, 3}, {1, 1, 1, 3}, {2, 2, 2, 2}, {1, 1, 1, 3}}));
    REQUIRE(*it++
            == to_word_graph<node_type>(5,
                                        {{1, 2, 1, 3},
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
    presentation::add_rule(p, "ab", "");
    presentation::add_rule(p, "abb", "");

    Sims1 S(congruence_kind::right);
    S.presentation(p);

    REQUIRE(S.number_of_congruences(10) == 1);
    auto d = MinimalRepOrc().presentation(p).target_size(1).word_graph();
    REQUIRE(d.number_of_nodes() == 1);
    REQUIRE(word_graph::is_strictly_cyclic(d));
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "045",
                          "right zero semigroup - minimal o.r.c. rep",
                          "[quick][sims1]") {
    // This is an example of a semigroup with a strictly cyclic faithful
    // right representation.
    auto         rg = ReportGuard(false);
    size_t const n  = 5;
    auto         p  = rectangular_band(1, n);
    auto d = MinimalRepOrc().presentation(p).target_size(n).word_graph();
    REQUIRE(word_graph::is_strictly_cyclic(d));
    auto S = to_froidure_pin<Transf<0, node_type>>(d);
    REQUIRE(S.size() == n);
    REQUIRE(d.number_of_nodes() == 5);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "046",
                          "semigroup with faithful non-strictly cyclic "
                          "action of right congruence",
                          "[quick][sims1]") {
    // Found with Smallsemi, this example is minimal wrt size of the
    // semigroup.

    auto rg = ReportGuard(false);

    FroidurePin<Transf<6>> S({Transf<6>::make({0, 0, 2, 1, 4, 1}),
                              Transf<6>::make({0, 0, 2, 3, 4, 3}),
                              Transf<6>::make({0, 2, 2, 0, 4, 4})});

    REQUIRE(S.size() == 5);
    auto p = to_presentation<word_type>(S);
    auto d = MinimalRepOrc().presentation(p).target_size(5).word_graph();
    REQUIRE(word_graph::is_strictly_cyclic(d));
    REQUIRE(d.number_of_nodes() == 4);
    REQUIRE(d
            == to_word_graph<uint32_t>(
                4, {{2, 2, 3}, {0, 1, 2}, {2, 2, 2}, {3, 3, 3}}));
    auto T = to_froidure_pin<Transf<4>>(d);
    REQUIRE(T.generator(0) == Transf<4>({2, 0, 2, 3}));
    REQUIRE(T.generator(1) == Transf<4>({2, 1, 2, 3}));
    REQUIRE(T.generator(2) == Transf<4>({3, 2, 2, 3}));
    REQUIRE(T.size() == 5);

    auto dd = to_word_graph<uint8_t>(5,
                                     {{0, 0, 0, 0, 0},
                                      {0, 0, 0, 0, 2},
                                      {2, 2, 2, 2, 2},
                                      {0, 1, 2, 3, 0},
                                      {4, 4, 4, 4, 4}});

    REQUIRE(!word_graph::is_strictly_cyclic(dd));
    REQUIRE(dd.number_of_nodes() == 5);
    auto U = to_froidure_pin<Transf<5>>(dd);
    REQUIRE(U.size() == 5);

    Sims1 C(congruence_kind::right);
    C.presentation(p);
    REQUIRE(C.number_of_congruences(5) == 9);
    uint64_t strictly_cyclic_count     = 0;
    uint64_t non_strictly_cyclic_count = 0;

    for (auto it = C.cbegin(5); it != C.cend(5); ++it) {
      auto W = to_froidure_pin<Transf<0, node_type>>(
          *it, 1, it->number_of_active_nodes());
      if (p.contains_empty_word()) {
        auto one = W.generator(0).identity();
        if (!W.contains(one)) {
          W.add_generator(one);
        }
      }
      if (W.size() == 5) {
        auto result = *it;
        result.induced_subgraph_no_checks(1, result.number_of_active_nodes());
        result.number_of_active_nodes(result.number_of_active_nodes() - 1);
        if (word_graph::is_strictly_cyclic(result)) {
          strictly_cyclic_count++;
        } else {
          REQUIRE(W.generator(0) == Transf<0, node_type>({3, 0, 2, 3, 4}));
          REQUIRE(W.generator(1) == Transf<0, node_type>({3, 1, 2, 3, 4}));
          REQUIRE(W.generator(2) == Transf<0, node_type>({4, 3, 2, 3, 4}));
          REQUIRE(
              result
              == to_word_graph<uint32_t>(
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
                          "rectangular_band(m, n) - m = 1 .. 5, n = 1 .. 5",
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

        auto  p = rectangular_band(m, n);
        Sims1 S(congruence_kind::left);
        S.presentation(p);
        REQUIRE(S.number_of_threads(4).number_of_congruences(m * n)
                == left[m][n]);
        Sims1 T(congruence_kind::right);
        T.presentation(p);
        REQUIRE(T.number_of_threads(4).number_of_congruences(m * n)
                == left[n][m]);
      }
    }
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "048",
                          "stellar_monoid(n) n = 3 .. 4",
                          "[fail][sims1][babbage]") {
    std::array<uint64_t, 10> const size      = {0, 0, 0, 16, 65};
    std::array<uint64_t, 10> const num_left  = {0, 0, 0, 1'550, 0};
    std::array<uint64_t, 10> const num_right = {0, 0, 0, 1'521, 0};

    for (size_t n = 3; n < 5; ++n) {
      auto p = zero_rook_monoid(n);
      auto q = stellar_monoid(n);
      p.rules.insert(p.rules.end(), q.rules.cbegin(), q.rules.cend());
      REQUIRE(p.alphabet().size() == n + 1);
      {
        Sims1 S(congruence_kind::left);
        S.presentation(p).number_of_threads(4);
        REQUIRE(S.number_of_congruences(size[n]) == num_left[n]);
      }
      {
        Sims1 S(congruence_kind::right);
        S.presentation(p).number_of_threads(4);
        REQUIRE(S.number_of_congruences(size[n]) == num_right[n]);
      }
    }
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "049",
                          "stylic_monoid(n) n = 3, 4",
                          "[fail][sims1]") {
    auto                           rg   = ReportGuard(true);
    std::array<uint64_t, 10> const size = {0, 0, 0, 14, 51};
    //               1505s
    std::array<uint64_t, 10> const num_left  = {0, 0, 0, 1'214, 1'429'447'174};
    std::array<uint64_t, 10> const num_right = {0, 0, 0, 1'214, 1'429'455'689};

    for (size_t n = 3; n < 5; ++n) {
      auto p = to_presentation<word_type>(stylic_monoid(n));
      {
        Sims1 S(congruence_kind::right);
        S.presentation(p).number_of_threads(4);
        REQUIRE(S.number_of_congruences(size[n]) == num_right[n]);
      }
      {
        Sims1 S(congruence_kind::left);
        S.presentation(p).number_of_threads(4);
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
    presentation::add_rule(p, "xx", "");
    presentation::add_rule(p, "yyy", "");
    presentation::add_rule(p, "xyxyxyxy", "yyxyyxyyx");
    Sims1 S(congruence_kind::right);
    S.presentation(p).number_of_threads(1);
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
    presentation::add_rule(p, "yXYYxyYYxyyXYYxyyXyXYYxy", "x");
    presentation::add_rule(p, "YxyyXXYYxyxYxyyXYXyXYYxxyyXYXyXYYxyx", "y");
    presentation::balance(p, p.alphabet(), std::string("XxYy"));

    REQUIRE(p.rules
            == std::vector<std::string>({"xX",
                                         "",
                                         "Xx",
                                         "",
                                         "yY",
                                         "",
                                         "Yy",
                                         "",
                                         "yXYYxyYYxyyXY",
                                         "xYXyyxYxYYXy",
                                         "YxyyXXYYxyxYxyyXYXy",
                                         "yXYXyyxYxyxYYXXyyx"}));

    Sims1 S(congruence_kind::right);
    S.presentation(p).number_of_threads(4).long_rule_length(37);
    REQUIRE(S.number_of_congruences(10) == 1);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "052",
                          "temperley_lieb_monoid(n) - n = 3 .. 6",
                          "[extreme][low-index][babbage]") {
    std::array<uint64_t, 10> const size = {0, 0, 0, 5, 14, 42, 132, 429};
    std::array<uint64_t, 10> const num_right
        = {0, 0, 0, 9, 79, 2'157, 4'326'459};

    auto rg = ReportGuard(true);
    for (size_t n = 3; n < 7; ++n) {
      auto p = temperley_lieb_monoid(n);
      p.contains_empty_word(true);
      Sims1 S(congruence_kind::right);
      S.presentation(p).number_of_threads(4);
      REQUIRE(S.number_of_congruences(size[n]) == num_right[n]);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "053",
                          "partial_transformation_monoid(3)",
                          "[extreme][low-index]") {
    auto rg = ReportGuard(true);
    auto p  = partial_transformation_monoid(3, author::Machine);
    {
      Sims1 S(congruence_kind::right);
      S.presentation(p).number_of_threads(4);
      REQUIRE(S.number_of_congruences(64) == 92'703);
    }
    {
      Sims1 S(congruence_kind::left);
      S.presentation(p).number_of_threads(4);
      REQUIRE(S.number_of_congruences(64) == 371);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "054",
                          "partial_transformation_monoid(4) from FroidurePin",
                          "[fail][low-index]") {
    using Transf_ = Transf<5>;
    auto rg       = ReportGuard(true);

    FroidurePin<Transf_> S({Transf_({1, 0, 2, 3, 4}),
                            Transf_({3, 0, 1, 2, 4}),
                            Transf_({4, 1, 2, 3, 4}),
                            Transf_({1, 1, 2, 3, 4})});
    REQUIRE(S.size() == 625);
    auto  p = to_presentation<word_type>(S);
    Sims1 C(congruence_kind::left);
    REQUIRE(presentation::longest_rule_length(p) == 18);
    REQUIRE(presentation::shortest_rule_length(p) == 3);

    presentation::remove_duplicate_rules(p);
    presentation::remove_trivial_rules(p);
    presentation::sort_each_rule(p);
    presentation::sort_rules(p);

    word_type w = presentation::longest_subword_reducing_length(p);
    while (!w.empty()) {
      presentation::replace_word_with_new_generator(p, w);
      w = presentation::longest_subword_reducing_length(p);
    }

    REQUIRE(presentation::length(p) == 1414);
    REQUIRE(presentation::longest_rule_length(p) == 6);

    C.presentation(p).long_rule_length(6).number_of_threads(8);
    REQUIRE(C.number_of_congruences(625) == 10);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "055",
                          "plactic_monoid(3) up to index 8",
                          "[extreme][low-index][plactic]") {
    std::array<uint64_t, 9> const num
        = {0, 1, 29, 484, 6'896, 103'204, 1'773'360, 35'874'182, 849'953'461};
    auto rg = ReportGuard(true);
    auto p  = plactic_monoid(3);
    for (size_t n = 2; n < 9; ++n) {
      {
        Sims1 S(congruence_kind::right);
        S.presentation(p).number_of_threads(4);
        REQUIRE(S.number_of_congruences(n) == num[n]);
      }
      {
        Sims1 S(congruence_kind::left);
        S.presentation(p).number_of_threads(4);
        REQUIRE(S.number_of_congruences(n) == num[n]);
      }
    }
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "056",
                          "plactic_monoid(4) up to index 6",
                          "[extreme][low-index][plactic]") {
    std::array<uint64_t, 8> const num
        = {0, 1, 67, 2'794, 106'264, 4'795'980, 278'253'841, 20'855'970'290};
    // Last value too 1h34m to compute so is not included.
    auto rg = ReportGuard(true);
    auto p  = plactic_monoid(4);
    for (size_t n = 2; n < 7; ++n) {
      {
        Sims1 S(congruence_kind::right);
        S.presentation(p).number_of_threads(4);
        REQUIRE(S.number_of_congruences(n) == num[n]);
      }
      {
        Sims1 S(congruence_kind::left);
        S.presentation(p).number_of_threads(4);
        REQUIRE(S.number_of_congruences(n) == num[n]);
      }
    }
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "057",
                          "plactic_monoid(5) up to index 5",
                          "[extreme][low-index][plactic]") {
    std::array<uint64_t, 7> const num
        = {0, 1, 145, 14'851, 1'496'113, 198'996'912, 37'585'675'984};
    // Last value took 5h11m to compute
    auto rg = ReportGuard(true);
    auto p  = plactic_monoid(5);
    for (size_t n = 3; n < 6; ++n) {
      {
        Sims1 S(congruence_kind::right);
        S.presentation(p).number_of_threads(4);
        REQUIRE(S.number_of_congruences(n) == num[n]);
      }
      {
        Sims1 S(congruence_kind::left);
        S.presentation(p).number_of_threads(4);
        REQUIRE(S.number_of_congruences(n) == num[n]);
      }
    }
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "058",
                          "plactic_monoid(6) up to index 4",
                          "[extreme][low-index][plactic]") {
    std::array<uint64_t, 6> const num
        = {0, 1, 303, 77'409, 20'526'128, 7'778'840'717};
    // The last value took 4h5m to run and is omitted.
    auto rg = ReportGuard(true);
    auto p  = plactic_monoid(6);
    for (size_t n = 2; n < 5; ++n) {
      {
        Sims1 S(congruence_kind::right);
        S.presentation(p).number_of_threads(4);
        REQUIRE(S.number_of_congruences(n) == num[n]);
      }
      {
        Sims1 S(congruence_kind::left);
        S.presentation(p).number_of_threads(4);
        REQUIRE(S.number_of_congruences(n) == num[n]);
      }
    }
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "059",
                          "plactic_monoid(7) up to index 3",
                          "[extreme][low-index][plactic]") {
    std::array<uint64_t, 5> const num = {0, 1, 621, 408'024, 281'600'130};
    // The last value took approx. 12m34s to run and is omitted from the
    // extreme test 12m34s to run and is omitted from the extreme test.
    auto rg = ReportGuard(true);
    auto p  = plactic_monoid(7);
    for (size_t n = 2; n < 4; ++n) {
      {
        Sims1 S(congruence_kind::right);
        S.presentation(p).number_of_threads(4);
        REQUIRE(S.number_of_congruences(n) == num[n]);
      }
      {
        Sims1 S(congruence_kind::left);
        S.presentation(p).number_of_threads(4);
        REQUIRE(S.number_of_congruences(n) == num[n]);
      }
    }
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "060",
                          "plactic_monoid(8) up to index 3",
                          "[extreme][low-index][plactic]") {
    std::array<uint64_t, 4> const num = {0, 1, 1'259, 2'201'564};
    auto                          rg  = ReportGuard(true);
    auto                          p   = plactic_monoid(8);
    for (size_t n = 2; n < 4; ++n) {
      {
        Sims1 S(congruence_kind::right);
        S.presentation(p).number_of_threads(4);
        REQUIRE(S.number_of_congruences(n) == num[n]);
      }
      {
        Sims1 S(congruence_kind::left);
        S.presentation(p).number_of_threads(4);
        REQUIRE(S.number_of_congruences(n) == num[n]);
      }
    }
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "061",
                          "chinese_monoid(3) up to index 8",
                          "[extreme][low-index][chinese]") {
    std::array<uint64_t, 9> const num
        = {0, 1, 31, 559, 8'904, 149'529, 2'860'018, 63'828'938, 1'654'488'307};
    // index 8 is doable and the value is included above, but it took about X
    // minutes to run, so isn't included in the loop below.
    auto rg = ReportGuard(true);
    auto p  = chinese_monoid(3);
    for (size_t n = 2; n < 8; ++n) {
      Sims1 S(congruence_kind::right);
      S.presentation(p).number_of_threads(4);
      REQUIRE(S.number_of_congruences(n) == num[n]);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "062",
                          "chinese_monoid(4) up to index 6",
                          "[extreme][low-index][chinese]") {
    // n = 6 took between 3 and 4 minutes
    // n = 7 took 6h16m
    // both are omitted
    std::array<uint64_t, 8> const num
        = {0, 1, 79, 3'809, 183'995, 10'759'706, 804'802'045, 77'489'765'654};
    auto rg = ReportGuard(true);
    auto p  = chinese_monoid(4);
    for (size_t n = 3; n < 7; ++n) {
      Sims1 S(congruence_kind::right);
      S.presentation(p).number_of_threads(4);
      REQUIRE(S.number_of_congruences(n) == num[n]);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "063",
                          "chinese_monoid(5) up to index 5",
                          "[extreme][low-index][chinese]") {
    std::array<uint64_t, 7> const num
        = {0, 1, 191, 23'504, 3'382'921, 685'523'226, 199'011'439'587};

    // The last value took 21h32m and so is omitted
    auto rg = ReportGuard(true);
    auto p  = chinese_monoid(5);
    for (size_t n = 3; n < 6; ++n) {
      Sims1 S(congruence_kind::right);
      S.presentation(p).number_of_threads(4);
      REQUIRE(S.number_of_congruences(n) == num[n]);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "064",
                          "chinese_monoid(6) up to index 4",
                          "[extreme][low-index][chinese]") {
    // 0 1 2 3 4
    std::array<uint64_t, 6> const num
        = {0, 1, 447, 137'694, 58'624'384, 40'823'448'867};
    // The last value took 9h54m to compute, and is omitted!
    auto rg = ReportGuard(true);
    auto p  = chinese_monoid(6);
    for (size_t n = 3; n < 5; ++n) {
      Sims1 S(congruence_kind::right);
      S.presentation(p).number_of_threads(4);
      REQUIRE(S.number_of_congruences(n) == num[n]);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "065",
                          "chinese_monoid(7) up to index 4",
                          "[extreme][low-index][chinese]") {
    // Last value took about 50m to compute
    std::array<uint64_t, 5> const num = {0, 1, 1'023, 786'949, 988'827'143};
    auto                          rg  = ReportGuard(true);
    auto                          p   = chinese_monoid(7);
    for (size_t n = 2; n < 4; ++n) {
      Sims1 S(congruence_kind::right);
      S.presentation(p).number_of_threads(4);
      REQUIRE(S.number_of_congruences(n) == num[n]);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "066",
                          "chinese_monoid(8) up to index 3",
                          "[extreme][low-index][chinese]") {
    std::array<uint64_t, 4> const num = {0, 1, 2'303, 4'459'599};
    auto                          rg  = ReportGuard(true);
    auto                          p   = chinese_monoid(8);
    for (size_t n = 2; n < 4; ++n) {
      Sims1 S(congruence_kind::right);
      S.presentation(p).number_of_threads(4);
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
      Sims1 S(congruence_kind::right);
      S.presentation(p);
      REQUIRE(S.number_of_congruences(3) == num[n]);
    }

    // For n >= 1, a(n) is the number of deterministic, completely-defined,
    // initially-connected finite automata with n inputs and 3 unlabeled
    // states. A020522 counts similar automata with n inputs and 2 unlabeled
    // states.

    // According to a comment by Nelma Moreira in A006689 and A006690, the
    // number of such automata with N inputs and M unlabeled states is Sum
    // (Product_{i=1..M-1} i^(f_i - f_{i-1} - 1)) * M^(M*N - f_{M-1} - 1),
    // where the sum is taken over integers f_1, ..., f_{M-1} satisfying 0 <=
    // f_1 < N and f_{i-1} < f_{i} < i*N for i = 2..M-1. (See Theorem 8 in
    // Almeida, Moreira, and Reis (2007). The value of f_0 is not relevant.)
    // For this sequence we have M = 3 unlabeled states, for A020522 we have M
    // = 2 unlabeled states, for A006689 we have N = 2 inputs, and for A006690
    // we have N = 3 inputs.
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1", "068", "RepOrc", "[quick][low-index]") {
    auto rg = ReportGuard(false);

    auto p = temperley_lieb_monoid(9);
    // There are no relations containing the empty word so we just manually
    // add it. FIXME there should be!
    p.contains_empty_word(true);
    RepOrc orc;
    // Check bad input
    auto d = orc.presentation(p)
                 .min_nodes(100)
                 .max_nodes(90)
                 .target_size(4'862)
                 .word_graph();
    REQUIRE(d.number_of_nodes() == 0);

    d = orc.presentation(p)
            .min_nodes(80)
            .max_nodes(100)
            .target_size(4'862)
            .word_graph();

    auto S = to_froidure_pin<Transf<0, node_type>>(d);
    S.add_generator(S.generator(0).identity());
    REQUIRE(S.size() == 4'862);
    REQUIRE(orc.min_nodes() == 80);
    REQUIRE(orc.max_nodes() == 100);
    REQUIRE(orc.target_size() == 4'862);
    REQUIRE(orc.presentation().rules.size() == 128);
    REQUIRE(orc.number_of_long_rules() == 0);
    REQUIRE(d.number_of_nodes() == 91);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "069",
                          "fp example 1",
                          "[quick][low-index]") {
    auto rg = ReportGuard(false);

    Presentation<word_type> p;
    p.contains_empty_word(true);
    p.alphabet(01_w);
    presentation::add_rule(p, 000_w, 0_w);
    presentation::add_rule(p, 11_w, 1_w);
    presentation::add_rule(p, 0101_w, 0_w);

    Sims1 S(congruence_kind::right);
    REQUIRE(S.presentation(p)
                .cbegin_long_rules(4)
                .number_of_threads(1)
                .number_of_congruences(5)
            == 6);
    S.long_rule_length(5);
    REQUIRE(S.number_of_long_rules() == 1);
    REQUIRE(S.presentation().rules.size() == 6);
    S.long_rule_length(4);
    REQUIRE(S.number_of_long_rules() == 2);
    REQUIRE(S.presentation().rules.size() == 6);

    S = Sims1(congruence_kind::left);
    REQUIRE(S.presentation(p)
                .cbegin_long_rules(4)
                .number_of_threads(2)
                .number_of_congruences(5)
            == 9);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "070",
                          "temperley_lieb_monoid(3) - n = minimal rep "
                          "(single-threaded, reporting on)",
                          "[standard][sims1]") {
    auto rg = ReportGuard(true);

    for (size_t n = 3; n <= 3; ++n) {
      auto p = temperley_lieb_monoid(n);
      // There are no relations containing the empty word so we just manually
      // add it.
      p.contains_empty_word(true);
      auto d = MinimalRepOrc()
                   .presentation(p)
                   .number_of_threads(1)
                   .target_size(5)
                   .word_graph();
      REQUIRE(word_graph::is_strictly_cyclic(d));
      auto S = to_froidure_pin<Transf<0, node_type>>(d);
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
    Sims1 S(congruence_kind::right);
    S.presentation(p);
    std::cout << "\n";  // So that the reporting looks good
    REQUIRE(S.number_of_threads(2).number_of_congruences(4) == 5'477);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "072",
                          "symmetric_group(n) for n = 4",
                          "[quick][low-index]") {
    std::array<uint64_t, 10> const num = {0, 0, 0, 6, 30, 156, 1'455};
    auto                           rg  = ReportGuard(false);
    size_t                         n   = 4;
    auto                           p   = symmetric_group(n, author::Carmichael);
    Sims1                          C(congruence_kind::right);
    C.presentation(p).number_of_threads(4);
    REQUIRE(C.number_of_congruences(factorial(n)) == num[n]);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "073",
                          "corner case no generators + no relations",
                          "[quick][low-index]") {
    Presentation<word_type> p;
    p.alphabet(0);
    Sims1 S(congruence_kind::right);
    REQUIRE_THROWS_AS(S.presentation(p), LibsemigroupsException);
    REQUIRE_THROWS_AS(S.number_of_congruences(1), LibsemigroupsException);
    REQUIRE_THROWS_AS(S.cbegin(2), LibsemigroupsException);
    REQUIRE_THROWS_AS(S.cend(2), LibsemigroupsException);
    REQUIRE_THROWS_AS(S.find_if(2, [](auto) { return true; }),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(S.for_each(2, [](auto) {}), LibsemigroupsException);
    REQUIRE_THROWS_AS(S.include(01_w, 10_w), LibsemigroupsException);
    REQUIRE_THROWS_AS(S.exclude(01_w, 10_w), LibsemigroupsException);

    p.alphabet(2);
    S.presentation(p);
    REQUIRE_THROWS_AS(S.exclude(01_w, 102_w), LibsemigroupsException);

    p.alphabet(3);
    S.presentation(p);
    S.exclude(01_w, 102_w);
    p.alphabet(2);
    REQUIRE_THROWS_AS(S.presentation(p), LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "074",
                          "monogenic_semigroup(m, r) for m, r = 1 .. 10",
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
        auto p = monogenic_semigroup(m, r);

        Sims1 C(congruence_kind::right);
        C.presentation(p);
        // std::cout << C.number_of_congruences(m + r) << ", ";
        REQUIRE(C.number_of_congruences(m + r) == num[m - 1][r - 1]);
      }
    }
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "075",
                          "partial_transformation_monoid(4)",
                          "[fail][low-index]") {
    auto rg = ReportGuard(true);
    auto p  = partial_transformation_monoid(4, author::Sutov);
    auto w  = presentation::longest_subword_reducing_length(p);
    while (!w.empty()) {
      presentation::replace_word_with_new_generator(
          p, presentation::longest_subword_reducing_length(p));
      w = presentation::longest_subword_reducing_length(p);
    }

    presentation::sort_each_rule(p);
    presentation::sort_rules(p);
    presentation::remove_duplicate_rules(p);
    presentation::reduce_complements(p);
    presentation::remove_trivial_rules(p);

    do {
      auto it = knuth_bendix::redundant_rule(p, std::chrono::milliseconds(100));
      p.rules.erase(it, it + 2);
    } while (presentation::length(p) > 800);
    Sims1 C(congruence_kind::left);
    C.presentation(p).number_of_threads(4);
    REQUIRE(C.number_of_congruences(624) == 0);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "076",
                          "uninitialized RepOrc",
                          "[quick][low-index]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("abc");
    presentation::add_rule(p, "cc", "c");
    presentation::add_rule(p, "abb", "a");
    presentation::add_rule(p, "aca", "aba");

    RepOrc orc;
    orc.presentation(p).number_of_threads(std::thread::hardware_concurrency());
    REQUIRE(orc.min_nodes() == 0);
    REQUIRE(orc.max_nodes() == 0);
    REQUIRE(orc.target_size() == 0);
    REQUIRE(orc.word_graph().number_of_nodes() == 0);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "077",
                          "2x2 simple semigroups over S(4)",
                          "[extreme][sims1]") {
    auto rg = ReportGuard(true);

    Presentation<std::string> p;
    p.alphabet("abc");

    // S := ReesMatrixSemigroup(SymmetricGroup(4), [[(1, 2), ()], [(), ()]]);
    presentation::add_rule(p, "cc", "c");
    presentation::add_rule(p, "abb", "a");
    presentation::add_rule(p, "aca", "aba");
    presentation::add_rule(p, "acb", "a");
    presentation::add_rule(p, "bba", "a");
    presentation::add_rule(p, "bbb", "b");
    presentation::add_rule(p, "bca", "a");
    presentation::add_rule(p, "bcb", "b");
    presentation::add_rule(p, "cbc", "c");
    presentation::add_rule(p, "aaaa", "bb");
    presentation::add_rule(p, "baaa", "abab");
    presentation::add_rule(p, "baba", "aaab");
    presentation::add_rule(p, "abaaba", "baab");
    presentation::add_rule(p, "baabaa", "aabaab");
    presentation::add_rule(p, "baabab", "aaabaa");
    presentation::add_rule(p, "aaabaab", "baaba");
    // Minimum rep. o.r.c. 6

    // S := ReesMatrixSemigroup(SymmetricGroup(4), [[(1, 2, 3, 4), ()], [(),
    // ()]]);
    // presentation::add_rule(p, "cc", "c");
    // presentation::add_rule(p, "abb", "a");
    // presentation::add_rule(p, "aca", "a");
    // presentation::add_rule(p, "acb", "b");
    // presentation::add_rule(p, "bba", "a");
    // presentation::add_rule(p, "bbb", "b");
    // presentation::add_rule(p, "bca", "b");
    // presentation::add_rule(p, "bcb", "aba");
    // presentation::add_rule(p, "cac", "c");
    // presentation::add_rule(p, "aaaa", "bb");
    // presentation::add_rule(p, "baaa", "abab");
    // presentation::add_rule(p, "baba", "aaab");
    // presentation::add_rule(p, "abaaba", "baab");
    // presentation::add_rule(p, "baabaa", "aabaab");
    // presentation::add_rule(p, "baabab", "aaabaa");
    // presentation::add_rule(p, "aaabaab", "baaba");
    // Minimum rep. o.r.c. 8

    // S := ReesMatrixSemigroup(SymmetricGroup(4), [[(1, 2, 3), ()], [(),
    // ()]]); presentation::add_rule(p, "cc", "c"); presentation::add_rule(p,
    // "abb", "a"); presentation::add_rule(p, "bba", "a");
    // presentation::add_rule(p, "bbb", "b");
    // presentation::add_rule(p, "bcb", "aca");
    // presentation::add_rule(p, "aaaa", "bb");
    // presentation::add_rule(p, "aaca", "bab");
    // presentation::add_rule(p, "abca", "baa");
    // presentation::add_rule(p, "acaa", "aab");
    // presentation::add_rule(p, "baaa", "abab");
    // presentation::add_rule(p, "baba", "aaab");
    // presentation::add_rule(p, "baca", "acba");
    // presentation::add_rule(p, "bacb", "acbb");
    // presentation::add_rule(p, "bcaa", "bab");
    // presentation::add_rule(p, "bcab", "aacb");
    // presentation::add_rule(p, "aaaba", "acab");
    // presentation::add_rule(p, "aaacb", "baab");
    // presentation::add_rule(p, "aabaa", "acbb");
    // presentation::add_rule(p, "aabab", "bbca");
    // presentation::add_rule(p, "aacba", "acb");
    // presentation::add_rule(p, "aacbb", "bca");
    // presentation::add_rule(p, "abaab", "acba");
    // presentation::add_rule(p, "acaba", "bca");
    // presentation::add_rule(p, "acaca", "a");
    // presentation::add_rule(p, "acacb", "b");
    // presentation::add_rule(p, "acbaa", "baab");
    // presentation::add_rule(p, "acbab", "abaa");
    // presentation::add_rule(p, "acbca", "aba");
    // presentation::add_rule(p, "baaba", "aacb");
    // presentation::add_rule(p, "baacb", "aaba");
    // presentation::add_rule(p, "bcaca", "b");
    // presentation::add_rule(p, "bcacb", "aba");
    // presentation::add_rule(p, "cacac", "c");
    // presentation::add_rule(p, "acbbca", "aaab");
    // min. rep. o.r.c. is 7

    presentation::sort_each_rule(p);
    presentation::sort_rules(p);

    p.validate();

    MinimalRepOrc orc;
    auto          d = orc.presentation(p)
                 .target_size(96)
                 .number_of_threads(std::thread::hardware_concurrency())
                 .word_graph();
    REQUIRE(d.number_of_nodes() == 7);
    REQUIRE(orc.target_size() == 96);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "078",
                          "order_preserving_monoid(5)",
                          "[extreme][sims1]") {
    auto p = fpsemigroup::order_preserving_monoid(5);

    REQUIRE(p.rules.size() == 50);
    presentation::sort_each_rule(p);
    presentation::sort_rules(p);
    presentation::remove_duplicate_rules(p);
    presentation::reduce_complements(p);
    presentation::remove_trivial_rules(p);
    REQUIRE(p.rules.size() == 50);

    Sims1 S(congruence_kind::right);
    REQUIRE(S.presentation(p).number_of_threads(4).number_of_congruences(126)
            == 37'951);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "079",
                          "order_preserving_monoid(6)",
                          "[extreme][sims1]") {
    auto p = fpsemigroup::order_preserving_monoid(6);

    REQUIRE(p.rules.size() == 72);
    presentation::sort_each_rule(p);
    presentation::sort_rules(p);
    presentation::remove_duplicate_rules(p);
    presentation::reduce_complements(p);
    presentation::remove_trivial_rules(p);
    REQUIRE(p.rules.size() == 72);

    Sims1 S(congruence_kind::right);
    REQUIRE(S.presentation(p).number_of_threads(4).number_of_congruences(462)
            == 37'951);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "080",
                          "fibonacci_group(2, 9) x 1",
                          "[fail][sims1]") {
    auto                      rg = ReportGuard(true);
    Presentation<std::string> p;
    p.alphabet("abcdefghiABCDEFGHI");
    p.contains_empty_word(true);
    presentation::add_inverse_rules(p, "ABCDEFGHIabcdefghi");
    REQUIRE(p.rules.size() == 36);
    presentation::add_rule(p, "ab", "c");
    presentation::add_rule(p, "bc", "d");
    presentation::add_rule(p, "cd", "e");
    presentation::add_rule(p, "de", "f");
    presentation::add_rule(p, "ef", "g");
    presentation::add_rule(p, "fg", "h");
    presentation::add_rule(p, "gh", "i");
    presentation::add_rule(p, "hi", "a");
    presentation::add_rule(p, "ia", "b");
    Sims1 S(congruence_kind::right);
    S.presentation(p);
    REQUIRE(S.number_of_threads(4).number_of_congruences(12) == 37'951);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "081",
                          "fibonacci_group(2, 9) x 2",
                          "[extreme][sims1]") {
    using words::pow;
    auto                      rg = ReportGuard(true);
    Presentation<std::string> p;
    p.alphabet("abAB");
    p.contains_empty_word(true);
    presentation::add_inverse_rules(p, "ABab");
    presentation::add_rule(p, "Abababbab", "aBaaBaB");
    presentation::add_rule(p, "babbabbAb", "ABaaBaa");
    presentation::add_rule(p, "abbabbAbA", "BABaaBa");
    presentation::add_rule(p, "bbabbAbAA", "ABABaaB");
    presentation::add_rule(p, "babbAbAAb", "BABABaa");
    presentation::add_rule(p, "abbAbAAbA", "BBABABa");
    presentation::add_rule(p, "bbAbAAbAA", "ABBABAB");
    presentation::add_rule(p, "bAbAAbAAb", "BABBABA");
    presentation::add_rule(p, "AbAAbAAba", "BBABBAB");
    presentation::add_rule(p, "bAAbAAbab", "aBBABBA");
    presentation::add_rule(p, "AAbAAbaba", "BaBBABB");

    presentation::add_rule(p, "AAbababb", "BaaBaBBA");
    presentation::add_rule(p, "Abababba", "aBaaBaBB");
    presentation::add_rule(p, "abbabaaBaaB", "bAbAAbA");
    presentation::add_rule(p, "babaaBaaBaB", "BAbAbAA");

    Sims1 S(congruence_kind::right);
    S.presentation(p);
    REQUIRE(S.number_of_threads(6).number_of_congruences(12) == 6);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1", "082", "trivial group", "[extreme][sims1]") {
    Presentation<std::string> p;
    p.alphabet("rstRST");
    p.contains_empty_word(true);
    presentation::add_inverse_rules(p, "RSTrst");
    presentation::add_rule(p, "rt", "trr");
    presentation::add_rule(p, "sr", "rss");
    presentation::add_rule(p, "ts", "stt");

    ToddCoxeter tc(congruence_kind::twosided, p);
    REQUIRE(tc.number_of_classes() == 1);

    Sims1 S(congruence_kind::right);
    S.presentation(p);
    REQUIRE(S.number_of_threads(4).number_of_congruences(20) == 1);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1", "083", "M11 x 1", "[extreme][sims1]") {
    using words::pow;
    Presentation<std::string> p;
    p.alphabet("abcABC");
    p.contains_empty_word(true);
    presentation::add_inverse_rules(p, "ABCabc");
    presentation::add_rule(p, pow("a", 6), pow("A", 5));
    presentation::add_rule(p, pow("b", 5), "");
    presentation::add_rule(p, pow("c", 4), "");
    presentation::add_rule(p, "aca", "CAC");
    presentation::add_rule(p, "bc", "cbb");
    presentation::add_rule(p, "Aba", "aab");
    presentation::sort_each_rule(p);
    presentation::sort_rules(p);
    presentation::balance(p, "abcABC", "ABCabc");

    REQUIRE(presentation::longest_subword_reducing_length(p) == "aa");
    presentation::replace_word_with_new_generator(p, "aa");

    ToddCoxeter tc(congruence_kind::twosided, p);
    REQUIRE(tc.number_of_classes() == 7'920);

    Sims1 S(congruence_kind::right);
    S.presentation(p);
    REQUIRE(S.number_of_threads(4).number_of_congruences(12) == 24);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1", "084", "M11 x 2", "[extreme][sims1]") {
    using words::pow;
    Presentation<std::string> p;
    p.alphabet("abcABC");
    p.contains_empty_word(true);
    presentation::add_inverse_rules(p, "ABCabc");
    presentation::add_rule(p, pow("b", 5), "");
    presentation::add_rule(p, pow("c", 4), "");
    presentation::add_rule(p, "acacac", "");
    presentation::add_rule(p, "bc", "cbb");
    presentation::add_rule(p, "ba", "aaab");
    presentation::add_rule(p, "aabba", "bb");
    presentation::sort_each_rule(p);
    presentation::sort_rules(p);

    REQUIRE(presentation::longest_subword_reducing_length(p) == "bb");
    presentation::replace_word_with_new_generator(p, "bb");

    ToddCoxeter tc(congruence_kind::twosided, p);
    REQUIRE(tc.number_of_classes() == 7'920);

    Sims1 S(congruence_kind::right);
    S.presentation(p);
    REQUIRE(S.number_of_threads(4).number_of_congruences(16) == 24);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "085",
                          "JonesMonoid(4)",
                          "[extreme][sims1]") {
    using words::pow;
    Presentation<std::string> p
        = to_presentation<std::string>(temperley_lieb_monoid(4));
    REQUIRE(p.contains_empty_word());
    presentation::sort_each_rule(p);
    presentation::sort_rules(p);

    Sims1 S(congruence_kind::right);
    S.presentation(p);
    REQUIRE(S.number_of_threads(4).number_of_congruences(10) == 69);
    REQUIRE(S.number_of_threads(4).number_of_congruences(11) == 74);

    for (auto it = S.cbegin(11); it != S.cend(11); ++it) {
      if (it->number_of_active_nodes() == 11) {
        std::cout << *it << std::endl;
      }
    }
  }

  // To keep GCC from complaining
  WordGraph<uint32_t> find_quotient(Presentation<std::string> const& p,
                                    size_t                           skip);

  WordGraph<uint32_t> find_quotient(Presentation<std::string> const& p,
                                    size_t                           skip) {
    SuppressReportFor suppressor("FroidurePin");

    auto   T              = Sims1(congruence_kind::right).presentation(p);
    size_t skipped_so_far = 0;

    auto hook = [&](word_graph_type const& x) {
      auto first = (T.presentation().contains_empty_word() ? 0 : 1);
      auto S     = to_froidure_pin<Transf<0, node_type>>(
          x, first, x.number_of_active_nodes());
      if (T.presentation().contains_empty_word()) {
        auto one = S.generator(0).identity();
        if (!S.contains(one)) {
          S.add_generator(one);
        }
      }
      if (S.size() == 120) {
        auto scc = Gabow(S.right_cayley_graph());
        if (scc.number_of_components() != 26) {
          return false;
        }
        scc.init(S.left_cayley_graph());
        if (skipped_so_far == skip && scc.number_of_components() == 26) {
          skipped_so_far++;
          return true;
        }
      }
      return false;
    };

    auto result = T.find_if(120, hook);
    return result;
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "086",
                          "search for possibly non-existent monoid",
                          "[extreme][sims1]") {
    Presentation<std::string> p;
    p.contains_empty_word(true);
    p.alphabet("abcde");
    presentation::add_rule(p, "aa", "a");
    presentation::add_rule(p, "ad", "d");
    presentation::add_rule(p, "bb", "b");
    presentation::add_rule(p, "ca", "ac");
    presentation::add_rule(p, "cc", "c");
    presentation::add_rule(p, "da", "d");
    presentation::add_rule(p, "dc", "cd");
    presentation::add_rule(p, "dd", "d");
    presentation::add_rule(p, "aba", "a");
    presentation::add_rule(p, "bab", "b");
    presentation::add_rule(p, "bcb", "b");
    presentation::add_rule(p, "bcd", "cd");
    presentation::add_rule(p, "cbc", "c");
    presentation::add_rule(p, "cdb", "cd");
    presentation::change_alphabet(p, "cbade");

    presentation::add_rule(p, "ea", "ae");
    presentation::add_rule(p, "be", "eb");
    presentation::add_rule(p, "ee", "e");
    presentation::add_rule(p, "cec", "c");
    presentation::add_rule(p, "ece", "e");

    presentation::add_rule(p, "ead", "ad");
    presentation::add_rule(p, "ade", "ad");
    // presentation::add_rule(p, "de", "ed");
    auto d = find_quotient(p, 0);
    REQUIRE(d.number_of_nodes() == 120);
    auto S = to_froidure_pin<Transf<0, node_type>>(
        d, 0, d.number_of_active_nodes());
    REQUIRE(d == to_word_graph<uint32_t>(1, {{}}));
    auto one = S.generator(0).identity();
    S.add_generator(one);
    REQUIRE(S.size() == 120);
    REQUIRE(S.number_of_generators() == 6);  // number 6 is the identity
    REQUIRE(S.generator(0)
            == Transf<0, node_type>({0,  0,  2,  3,  0,  2,  10, 3,  3,  14,
                                     10, 11, 2,  18, 14, 11, 11, 23, 18, 19,
                                     14, 2,  22, 23, 19, 19, 23, 2}));
    REQUIRE(S.generator(1)
            == Transf<0, node_type>({1,  1,  2,  7,  6,  1,  6,  7,  13, 6,
                                     6,  15, 7,  13, 20, 15, 22, 13, 13, 24,
                                     20, 15, 22, 26, 24, 2,  26, 24}));
    REQUIRE(S.generator(2)
            == Transf<0, node_type>({2,  5,  2,  2,  2,  5,  9,  12, 2,  9,
                                     14, 2,  12, 17, 14, 21, 2,  17, 23, 2,
                                     14, 21, 22, 23, 27, 2,  23, 27}));
    REQUIRE(S.generator(3)
            == Transf<0, node_type>({3,  3,  2, 3,  3,  2,  11, 3, 3,  2,
                                     11, 11, 2, 19, 2,  11, 11, 2, 19, 19,
                                     2,  2,  2, 2,  19, 19, 2,  2}));
    REQUIRE(S.generator(4)
            == Transf<0, node_type>({4, 6,  2,  8,  4, 9,  6,  13, 8,  9,
                                     6, 16, 17, 13, 9, 22, 16, 17, 13, 25,
                                     6, 22, 22, 17, 2, 25, 13, 2}));
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "087",
                          "2-sylvester monoid",
                          "[extreme][sims1]") {
    Presentation<word_type> p;
    p.alphabet(3);
    p.rules = {100_w,       010_w,       200_w,       020_w,       201_w,
               021_w,       211_w,       121_w,       1010_w,      0110_w,
               1020_w,      0120_w,      2020_w,      0220_w,      2021_w,
               0221_w,      2101_w,      1021_w,      2121_w,      1221_w,
               10110_w,     01110_w,     10120_w,     01120_w,     10210_w,
               01210_w,     10220_w,     01220_w,     20220_w,     02220_w,
               20221_w,     02221_w,     21021_w,     10221_w,     21221_w,
               12221_w,     101110_w,    011110_w,    101120_w,    011120_w,
               101210_w,    011210_w,    101220_w,    011220_w,    102120_w,
               012120_w,    102210_w,    012210_w,    102220_w,    012220_w,
               202220_w,    022220_w,    202221_w,    022221_w,    210221_w,
               102221_w,    212221_w,    122221_w,    1011210_w,   0111210_w,
               1012120_w,   0112120_w,   1012210_w,   0112210_w,   1021220_w,
               0121220_w,   1022120_w,   0122120_w,   1022210_w,   0122210_w,
               10112210_w,  01112210_w,  10122120_w,  01122120_w,  10122210_w,
               01122210_w,  10221220_w,  01221220_w,  10222120_w,  01222120_w,
               101122210_w, 011122210_w, 101222120_w, 011222120_w, 101222210_w,
               011222210_w, 102221220_w, 012221220_w};

    auto S = Sims1(congruence_kind::right).presentation(p);
    REQUIRE(S.number_of_congruences(31) == 0);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1", "088", "Brauer monoid", "[extreme][sims1]") {
    auto p = brauer_monoid(5);
    // REQUIRE(p.alphabet() == 012_w);
    MinimalRepOrc orc;
    auto          d = orc.presentation(brauer_monoid(5))
                 .target_size(945)
                 .number_of_threads(1)
                 // The following are pairs of words in the GAP BrauerMonoid
                 // that generate the minimal 2-sided congruences of
                 // BrauerMonoid(5), the generating sets are not the same
                 // though and so this doesn't work.
                 // .exclude(201002_w, 00201002_w)
                 // .exclude(00102002_w, 001020020_w)
                 .word_graph();

    // sigma_i = (i, i + 1)
    // theta_i = Bipartition([[i, i + 1], [-i, -(i + 1)], [j, -j], j neq i]);

    REQUIRE(d.number_of_nodes() == 3);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "089",
                          "partial Brauer monoid",
                          "[extreme][sims1]") {
    std::array<uint64_t, 6> const sizes       = {0, 2, 10, 76, 764, 9496};
    std::array<uint64_t, 6> const min_degrees = {0, 2, 6, 14, 44, 143};

    for (size_t n = 1; n < 5; ++n) {
      std::cout << std::string(80, '#') << std::endl;
      auto          p = fpsemigroup::partial_brauer_monoid(n);
      MinimalRepOrc orc;
      auto          d = orc.presentation(p)
                   .target_size(sizes[n])
                   .number_of_threads(4)
                   .word_graph();

      // sigma_i = (i, i + 1)
      // theta_i = Bipartition([[i, i + 1], [-i, -(i + 1)], [j, -j], j neq i]);

      REQUIRE(d.number_of_nodes() == min_degrees[n]);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "090",
                          "possible full transf. monoid 8",
                          "[extreme][sims1]") {
    Presentation<word_type> p;
    p.rules = std::vector<word_type>(
        {00_w,         {},           11_w,        {},         22_w,     {},
         33_w,         {},           44_w,        {},         55_w,     {},
         66_w,         {},           101_w,       010_w,      212_w,    121_w,
         323_w,        232_w,        434_w,       343_w,      545_w,    454_w,
         656_w,        565_w,        606_w,       060_w,      2010_w,   0102_w,
         3010_w,       0103_w,       4010_w,      0104_w,     5010_w,   0105_w,
         6010_w,       0106_w,       1210_w,      0121_w,     3121_w,   1213_w,
         4121_w,       1214_w,       5121_w,      1215_w,     6121_w,   1216_w,
         2320_w,       0232_w,       2321_w,      1232_w,     4232_w,   2324_w,
         5232_w,       2325_w,       6232_w,      2326_w,     3430_w,   0343_w,
         3431_w,       1343_w,       3432_w,      2343_w,     5343_w,   3435_w,
         6343_w,       3436_w,       4540_w,      0454_w,     4541_w,   1454_w,
         4542_w,       2454_w,       4543_w,      3454_w,     6454_w,   4546_w,
         5650_w,       0565_w,       5651_w,      1565_w,     5652_w,   2565_w,
         5653_w,       3565_w,       5654_w,      4565_w,     6061_w,   1606_w,
         6062_w,       2606_w,       6063_w,      3606_w,     6064_w,   4606_w,
         6065_w,       5606_w,       071654321_w, 16543217_w, 217121_w, 17171_w,
         7010270102_w, 0102720107_w, 7010701_w,   1070170_w});
    p.alphabet_from_rules();
    auto q = full_transformation_monoid(8);

    std::array<uint64_t, 9> const num = {0, 1, 2, 3, 3, 3, 3, 0, 0};
    Sims1                         s(congruence_kind::right, p);
    for (size_t n = 1; n < num.size(); ++n) {
      s.presentation(q);
      REQUIRE(s.number_of_congruences(n) == num[n]);
      s.presentation(p);
      REQUIRE(s.number_of_congruences(n) == num[n]);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1",
                          "091",
                          "free semilattice n = 8",
                          "[extreme][sims1]") {
    Presentation<std::string> p;
    p.alphabet("abcdef");
    presentation::add_rule(p, "a^2"_p, "a");
    presentation::add_rule(p, "b^2"_p, "b");
    presentation::add_rule(p, "ba"_p, "ab");
    presentation::add_rule(p, "c^2"_p, "c");
    presentation::add_rule(p, "ca"_p, "ac");
    presentation::add_rule(p, "cb"_p, "bc");
    presentation::add_rule(p, "d^2"_p, "d");
    presentation::add_rule(p, "da"_p, "ad");
    presentation::add_rule(p, "db"_p, "bd");
    presentation::add_rule(p, "dc"_p, "cd");
    presentation::add_rule(p, "e^2"_p, "e");
    presentation::add_rule(p, "ea"_p, "ae");
    presentation::add_rule(p, "eb"_p, "be");
    presentation::add_rule(p, "ec"_p, "ce");
    presentation::add_rule(p, "ed"_p, "de");
    presentation::add_rule(p, "f^2"_p, "f");
    presentation::add_rule(p, "fa"_p, "af");
    presentation::add_rule(p, "fb"_p, "bf");
    presentation::add_rule(p, "fc"_p, "cf");
    presentation::add_rule(p, "fd"_p, "df");
    presentation::add_rule(p, "fe"_p, "ef");
    Sims1 s(congruence_kind::right, p);

    REQUIRE(s.number_of_threads(4).number_of_congruences(std::pow(2, 6)) == 0);
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1", "092", "2-sided example", "[quick][sims1]") {
    Presentation<word_type> p;
    p.alphabet(2);
    p.contains_empty_word(true);
    presentation::add_rule(p, 00_w, {});
    presentation::add_rule(p, 01_w, 1_w);
    presentation::add_rule(p, 11_w, 1_w);
    Sims1 s(congruence_kind::twosided, p);
    // REQUIRE(s.number_of_congruences(4) == 4);  // Verified with GAP
    auto it = s.cbegin(4);
    REQUIRE(*(it++) == to_word_graph<node_type>(4, {{0, 0}}));
    REQUIRE(*(it++) == to_word_graph<node_type>(4, {{0, 1}, {1, 1}}));
    REQUIRE(*(it++) == to_word_graph<node_type>(4, {{1, 2}, {0, 2}, {2, 2}}));
    REQUIRE(*(it++)
            == to_word_graph<node_type>(4, {{1, 2}, {0, 2}, {3, 2}, {2, 2}}));
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1", "093", "2-sided example", "[quick][sims1]") {
    Sims1 s(congruence_kind::twosided, full_transformation_monoid(4));

    REQUIRE(s.number_of_congruences(256) == 11);  // Verified with GAP
  }

  LIBSEMIGROUPS_TEST_CASE("Sims1", "094", "2-sided example", "[quick][sims1]") {
    Presentation<word_type> p;
    p.contains_empty_word(true);
    p.alphabet({0, 1});
    presentation::add_rule(p, {0, 0, 0}, {0});
    presentation::add_rule(p, {1, 1}, {1});
    presentation::add_rule(p, {0, 1, 0, 1}, {0});
    Sims1 s(congruence_kind::twosided, p);

    // REQUIRE(s.number_of_congruences(4) == 6);  // Verified with GAP
    auto it = s.cbegin(5);
    REQUIRE(*(it++) == to_word_graph<node_type>(5, {{0, 0}}));
    REQUIRE(*(it++) == to_word_graph<node_type>(5, {{1, 0}, {1, 1}}));
    REQUIRE(*(it++) == to_word_graph<node_type>(5, {{1, 1}, {1, 1}}));
    REQUIRE(*(it++) == to_word_graph<node_type>(5, {{1, 2}, {1, 1}, {1, 2}}));
    REQUIRE(*(it++) == to_word_graph<node_type>(5, {{1, 2}, {1, 1}, {2, 2}}));
    REQUIRE(*(it++)
            == to_word_graph<node_type>(5, {{1, 2}, {1, 1}, {3, 2}, {3, 3}}));
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
