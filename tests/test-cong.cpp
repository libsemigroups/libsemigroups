//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019 James D. Mitchell
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

#include "catch.hpp"      // for TEST_CASE
#include "test-main.hpp"  // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/cong.hpp"             // for Congruence
#include "libsemigroups/fastest-bmat.hpp"     // for FastestBMat
#include "libsemigroups/fpsemi-examples.hpp"  // for rook_monoid
#include "libsemigroups/froidure-pin.hpp"     // for FroidurePin
#include "libsemigroups/obvinf.hpp"           // for is_obviously_infinite
#include "libsemigroups/pbr.hpp"              // for PBR
#include "libsemigroups/report.hpp"           // for ReportGuard
#include "libsemigroups/to-froidure-pin.hpp"
#include "libsemigroups/transf.hpp"  // for Transf<>
#include "libsemigroups/types.hpp"   // for word_type
#include "libsemigroups/words.hpp"   // for literals

namespace libsemigroups {

  namespace {
    auto sizes_ntc(std::vector<std::vector<word_type>> const& ntc) {
      std::vector<size_t> sizes(ntc.size(), 0);
      std::transform(ntc.cbegin(),
                     ntc.cend(),
                     sizes.begin(),
                     std::mem_fn(&std::vector<word_type>::size));
      std::sort(sizes.begin(), sizes.end());
      return sizes;
    }

    auto unique_sizes_ntc(std::vector<std::vector<word_type>> const& ntc) {
      auto sizes = sizes_ntc(ntc);
      sizes.erase(std::unique(sizes.begin(), sizes.end()), sizes.end());
      return sizes;
    }
  }  // namespace

  // Forward declarations
  struct LibsemigroupsException;

  using literals::operator""_w;

  constexpr congruence_kind twosided = congruence_kind::twosided;
  constexpr congruence_kind left     = congruence_kind::left;
  constexpr congruence_kind right    = congruence_kind::right;

  using fpsemigroup::rook_monoid;
  using fpsemigroup::stellar_monoid;

  LIBSEMIGROUPS_TEST_CASE("Congruence",
                          "000",
                          "left congruence from presentation",
                          "[quick][cong]") {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.alphabet(2);
    presentation::add_rule(p, 000_w, 0_w);
    presentation::add_rule(p, 0_w, 11_w);

    Congruence cong(left, p);

    REQUIRE(cong.number_of_classes() == 5);
    REQUIRE(cong.contains(011001_w, 001_w));

    REQUIRE(cong.contains(001_w, 00001_w));
    REQUIRE(cong.contains(011001_w, 001_w));
    REQUIRE(!cong.contains(000_w, 001_w));
    REQUIRE(!cong.contains(1_w, 0000_w));
    REQUIRE(!cong.contains(0000_w, 001_w));
  }

  LIBSEMIGROUPS_TEST_CASE("Congruence",
                          "001",
                          "2-sided congruence from presentation",
                          "[quick][cong]") {
    auto rg = ReportGuard(false);

    Presentation<word_type> p;
    p.alphabet(2);
    presentation::add_rule(p, 000_w, 0_w);
    presentation::add_rule(p, 0_w, 11_w);
    // REQUIRE(!is_obviously_infinite(p));

    Congruence cong(twosided, p);

    REQUIRE(cong.number_of_classes() == 5);

    REQUIRE(cong.contains(001_w, 00001_w));
    REQUIRE(cong.contains(001_w, 001_w));
    REQUIRE(cong.contains(001_w, 00001_w));
    REQUIRE(cong.contains(00001_w, 011001_w));
    REQUIRE(!cong.contains(000_w, 001_w));
    REQUIRE(!cong.contains(000_w, 1_w));
    REQUIRE(!cong.contains(1_w, 000_w));
  }

  LIBSEMIGROUPS_TEST_CASE("Congruence",
                          "007",
                          "2-sided congruence from presentation",
                          "[quick][cong]") {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.alphabet(3);
    presentation::add_rule(p, 01_w, 10_w);
    presentation::add_rule(p, 02_w, 22_w);
    presentation::add_rule(p, 02_w, 0_w);
    presentation::add_rule(p, 02_w, 0_w);
    presentation::add_rule(p, 22_w, 0_w);
    presentation::add_rule(p, 12_w, 12_w);
    presentation::add_rule(p, 12_w, 22_w);
    presentation::add_rule(p, 122_w, 1_w);
    presentation::add_rule(p, 12_w, 1_w);
    presentation::add_rule(p, 22_w, 1_w);
    presentation::add_rule(p, 0_w, 1_w);
    presentation::add_rule(p, 0_w, 1_w);

    Congruence cong(twosided, p);

    REQUIRE(cong.number_of_classes() == 2);
  }

  LIBSEMIGROUPS_TEST_CASE("Congruence",
                          "008",
                          "2-sided congruence from presentation",
                          "[quick][cong]") {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.alphabet(3);
    presentation::add_rule(p, 01_w, 10_w);
    presentation::add_rule(p, 02_w, 22_w);
    presentation::add_rule(p, 02_w, 0_w);
    presentation::add_rule(p, 02_w, 0_w);
    presentation::add_rule(p, 22_w, 0_w);
    presentation::add_rule(p, 12_w, 12_w);
    presentation::add_rule(p, 12_w, 22_w);
    presentation::add_rule(p, 122_w, 1_w);
    presentation::add_rule(p, 12_w, 1_w);
    presentation::add_rule(p, 22_w, 1_w);
    presentation::add_rule(p, 0_w, 1_w);

    Congruence cong(twosided, p);

    REQUIRE(cong.contains(0_w, 1_w));
    REQUIRE(cong.contains(0_w, 10_w));
    REQUIRE(cong.contains(0_w, 11_w));
    REQUIRE(cong.contains(0_w, 101_w));

    REQUIRE(cong.contains(1_w, 11_w));
    REQUIRE(cong.contains(101_w, 10_w));
    REQUIRE(cong.number_of_classes() == 2);
  }

  LIBSEMIGROUPS_TEST_CASE("Congruence",
                          "009",
                          "infinite 2-sided congruence from presentation",
                          "[quick][cong]") {
    auto rg = ReportGuard(false);

    Presentation<word_type> p;
    p.alphabet(3);
    presentation::add_rule(p, 01_w, 10_w);
    presentation::add_rule(p, 02_w, 20_w);
    presentation::add_rule(p, 00_w, 0_w);
    presentation::add_rule(p, 02_w, 0_w);
    presentation::add_rule(p, 20_w, 0_w);
    presentation::add_rule(p, 12_w, 21_w);
    presentation::add_rule(p, 111_w, 1_w);
    presentation::add_rule(p, 12_w, 1_w);
    presentation::add_rule(p, 21_w, 1_w);

    Congruence cong(twosided, p);
    cong.add_pair(0_w, 1_w);

    REQUIRE(cong.number_of_classes() == POSITIVE_INFINITY);

    // Used to require KnuthBendixCongruenceByPairs to work
    REQUIRE(cong.contains(0_w, 1_w));
    REQUIRE(cong.contains(0_w, 10_w));
    REQUIRE(cong.contains(0_w, 11_w));
    REQUIRE(cong.contains(0_w, 101_w));
    REQUIRE(cong.contains(1_w, 11_w));
    REQUIRE(cong.contains(101_w, 10_w));

    REQUIRE(cong.has<KnuthBendix>());

    KnuthBendix kb(twosided, p);
    REQUIRE(knuth_bendix::non_trivial_classes(kb, *cong.get<KnuthBendix>())
            == std::vector<std::vector<std::string>>(
                {{"b", "ab", "bb", "abb", "a"}}));
  }

  LIBSEMIGROUPS_TEST_CASE("Congruence",
                          "010",
                          "2-sided congruence on finite semigroup",
                          "[quick][cong][no-valgrind]") {
    auto rg      = ReportGuard(false);
    using Transf = LeastTransf<8>;
    FroidurePin<Transf> S({Transf({7, 3, 5, 3, 4, 2, 7, 7}),
                           Transf({1, 2, 4, 4, 7, 3, 0, 7}),
                           Transf({0, 6, 4, 2, 2, 6, 6, 4}),
                           Transf({3, 6, 3, 4, 0, 6, 0, 7})});

    REQUIRE(S.size() == 11804);
    REQUIRE(S.number_of_rules() == 2460);

    Congruence cong(twosided, S);
    cong.add_pair(0321322_w, 322133_w);

    REQUIRE(cong.number_of_classes() == 525);

    REQUIRE(cong.contains(0001_w, 00100_w));
    REQUIRE(cong.contains(00101_w, 1101_w));
    REQUIRE(!cong.contains(1100_w, 0001_w));
    REQUIRE(!cong.contains(003_w, 0001_w));
    REQUIRE(!cong.contains(1100_w, 003_w));
    REQUIRE(cong.contains(12133212_w, 2133210_w));
    REQUIRE(cong.contains(0311132210_w, 03221_w));
    REQUIRE(!cong.contains(0321333_w, 003_w));
    REQUIRE(!cong.contains(110_w, 1332210_w));

    REQUIRE(cong.contains(12133212_w, 2133210_w));
    REQUIRE(!cong.contains(110_w, 1332210_w));

    REQUIRE(cong.number_of_classes() == 525);
  }

  LIBSEMIGROUPS_TEST_CASE("Congruence",
                          "011",
                          "congruence on full PBR monoid on 2 points",
                          "[extreme][cong]") {
    auto             rg = ReportGuard(true);
    FroidurePin<PBR> S({PBR({{2}, {3}, {0}, {1}}),
                        PBR({{}, {2}, {1}, {0, 3}}),
                        PBR({{0, 3}, {2}, {1}, {}}),
                        PBR({{1, 2}, {3}, {0}, {1}}),
                        PBR({{2}, {3}, {0}, {1, 3}}),
                        PBR({{3}, {1}, {0}, {1}}),
                        PBR({{3}, {2}, {0}, {0, 1}}),
                        PBR({{3}, {2}, {0}, {1}}),
                        PBR({{3}, {2}, {0}, {3}}),
                        PBR({{3}, {2}, {1}, {0}}),
                        PBR({{3}, {2, 3}, {0}, {1}})});

    // REQUIRE(S.size() == 65536);
    // REQUIRE(S.number_of_rules() == 45416);

    Congruence cong(twosided, S);
    cong.add_pair({7, 10, 9, 3, 6, 9, 4, 7, 9, 10}, {9, 3, 6, 6, 10, 9, 4, 7});
    cong.add_pair({8, 7, 5, 8, 9, 8}, {6, 3, 8, 6, 1, 2, 4});

    REQUIRE(cong.number_of_classes() == 19'009);
    // REQUIRE(cong.number_of_non_trivial_classes() == 577);
    // REQUIRE(cong.cend_ntc() - cong.cbegin_ntc() == 577);

    // std::vector<size_t> v(577, 0);
    // std::transform(cong.cbegin_ntc(),
    //                cong.cend_ntc(),
    //                v.begin(),
    //                std::mem_fn(&std::vector<word_type>::size));
    // REQUIRE(std::count(v.cbegin(), v.cend(), 4) == 384);
    // REQUIRE(std::count(v.cbegin(), v.cend(), 16) == 176);
    // REQUIRE(std::count(v.cbegin(), v.cend(), 96) == 16);
    // REQUIRE(std::count(v.cbegin(), v.cend(), 41216) == 1);
  }

  LIBSEMIGROUPS_TEST_CASE("Congruence",
                          "012",
                          "2-sided congruence on finite semigroup",
                          "[quick][cong][no-valgrind]") {
    auto rg = ReportGuard(false);

    FroidurePin<LeastPPerm<6>> S(
        {LeastPPerm<6>({0, 1, 2}, {4, 0, 1}, 6),
         LeastPPerm<6>({0, 1, 2, 3, 5}, {2, 5, 3, 0, 4}, 6),
         LeastPPerm<6>({0, 1, 2, 3}, {5, 0, 3, 1}, 6),
         LeastPPerm<6>({0, 2, 5}, {3, 4, 1}, 6),
         LeastPPerm<6>({0, 2, 5}, {0, 2, 5}, 6),
         LeastPPerm<6>({0, 1, 4}, {1, 2, 0}, 6),
         LeastPPerm<6>({0, 2, 3, 4, 5}, {3, 0, 2, 5, 1}, 6),
         LeastPPerm<6>({0, 1, 3, 5}, {1, 3, 2, 0}, 6),
         LeastPPerm<6>({1, 3, 4}, {5, 0, 2}, 6)});

    // REQUIRE(S.size() == 712);
    // REQUIRE(S.number_of_rules() == 1121);

    Congruence cong(twosided, S);
    cong.add_pair({2, 7}, {1, 6, 6, 1});
    REQUIRE(cong.number_of_classes() == 32);
  }

  LIBSEMIGROUPS_TEST_CASE("Congruence",
                          "013",
                          "trivial 2-sided congruence on bicyclic monoid",
                          "[quick][cong][no-valgrind]") {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.contains_empty_word(true);
    p.alphabet(2);
    presentation::add_rule(p, 01_w, {});

    Congruence cong(twosided, p);
    REQUIRE(cong.contains({}, 010011_w));
    REQUIRE(cong.contains({}, 0101_w));
    REQUIRE(cong.contains(10_w, 011001_w));
    REQUIRE(cong.contains(10_w, 011001_w));
  }

  LIBSEMIGROUPS_TEST_CASE("Congruence",
                          "014",
                          "non-trivial 2-sided congruence on bicyclic monoid",
                          "[quick][cong]") {
    auto rg = ReportGuard(false);

    Presentation<word_type> p;
    p.contains_empty_word(true).alphabet(2);
    presentation::add_rule(p, 01_w, {});

#ifdef LIBSEMIGROUPS_EIGEN_ENABLED
    REQUIRE(is_obviously_infinite(p));
#else
    REQUIRE(!is_obviously_infinite(p));
#endif

    Congruence cong(twosided, p);
    cong.add_pair(111_w, {});
    REQUIRE(cong.number_of_classes() == 3);

    if (cong.has<KnuthBendix>()) {
      KnuthBendix kb(twosided, p);
      REQUIRE_THROWS_AS(
          // FIXME swap the args
          knuth_bendix::non_trivial_classes(kb, *cong.get<KnuthBendix>()),
          LibsemigroupsException);
    } else if (cong.has<ToddCoxeter>()) {
      ToddCoxeter tc(twosided, p);
      REQUIRE_THROWS_AS(
          todd_coxeter::non_trivial_classes(*cong.get<ToddCoxeter>(), tc),
          LibsemigroupsException);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("Congruence",
                          "015",
                          "2-sided congruence on free abelian monoid",
                          "[quick][cong]") {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.alphabet(3);
    presentation::add_rule(p, 12_w, 21_w);
    presentation::add_identity_rules(p, 0);

    Congruence cong(twosided, p);
    cong.add_pair(11111_w, 1_w);
    cong.add_pair(222_w, 2_w);

    REQUIRE(cong.number_of_classes() == 15);
  }

  LIBSEMIGROUPS_TEST_CASE("Congruence",
                          "016",
                          "example where TC works but KB doesn't",
                          "[quick][cong]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("abBe");
    presentation::add_identity_rules(p, 'e');
    presentation::add_rule(p, "aa", "e");
    presentation::add_rule(p, "BB", "b");
    presentation::add_rule(p, "BaBaBaB", "abababa");
    presentation::add_rule(p, "aBabaBabaBabaBab", "BabaBabaBabaBaba");

    Congruence cong(twosided, p);
    cong.add_pair(0_w, 1_w);

    REQUIRE(cong.number_of_classes() == 4);
    REQUIRE(!to_froidure_pin(cong)->is_monoid());
  }

  LIBSEMIGROUPS_TEST_CASE("Congruence",
                          "017",
                          "2-sided congruence on finite semigroup",
                          "[quick][cong]") {
    auto rg      = ReportGuard(false);
    using Transf = LeastTransf<5>;
    FroidurePin<Transf> S({Transf({1, 3, 4, 2, 3}), Transf({3, 2, 1, 3, 3})});

    REQUIRE(S.size() == 88);
    REQUIRE(S.number_of_rules() == 18);

    word_type w1 = S.factorisation(Transf({3, 4, 4, 4, 4}));
    word_type w2 = S.factorisation(Transf({3, 4, 4, 4, 4}));

    Congruence cong(twosided, S);
    cong.add_pair(S.factorisation(Transf({3, 4, 4, 4, 4})),
                  S.factorisation(Transf({3, 1, 3, 3, 3})));
    REQUIRE(cong.number_of_classes() == 21);

    word_type u = S.factorisation(Transf({1, 3, 1, 3, 3}));
    word_type v = S.factorisation(Transf({4, 2, 4, 4, 2}));
    REQUIRE(cong.contains(u, v));
    REQUIRE(cong.contains(u, v));
  }

  // The next test behaves as expected but runs forever, since the
  // number_of_classes method requires to know the size of the semigroup S, and
  // we cannot currently work that out.
  LIBSEMIGROUPS_TEST_CASE("Congruence",
                          "018",
                          "infinite fp semigroup from GAP library",
                          "[quick][cong]") {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.alphabet(3);
    presentation::add_rule(p, 00_w, 00_w);
    presentation::add_rule(p, 01_w, 10_w);
    presentation::add_rule(p, 02_w, 20_w);
    presentation::add_rule(p, 00_w, 0_w);
    presentation::add_rule(p, 02_w, 0_w);
    presentation::add_rule(p, 20_w, 0_w);
    presentation::add_rule(p, 10_w, 01_w);
    presentation::add_rule(p, 11_w, 11_w);
    presentation::add_rule(p, 12_w, 21_w);
    presentation::add_rule(p, 111_w, 1_w);
    presentation::add_rule(p, 12_w, 1_w);
    presentation::add_rule(p, 21_w, 1_w);

    // REQUIRE(is_obviously_infinite(p));

    Congruence cong(twosided, p);
    cong.add_pair(0_w, 1_w);

    REQUIRE(is_obviously_infinite(cong));
    REQUIRE(cong.number_of_classes() == POSITIVE_INFINITY);

    REQUIRE(cong.has<KnuthBendix>());

    KnuthBendix kb(twosided, p);
    auto ntc = knuth_bendix::non_trivial_classes(kb, *cong.get<KnuthBendix>());
    REQUIRE(ntc.size() == 1);
    REQUIRE(ntc[0].size() == 5);
    REQUIRE(ntc[0] == std::vector<std::string>({"b", "ab", "bb", "abb", "a"}));
  }

  LIBSEMIGROUPS_TEST_CASE(
      "Congruence",
      "019",
      "2-sided cong. from presentation with infinite classes ",
      "[quick][cong]") {
    using presentation::operator+;
    using presentation::pow;

    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.alphabet(2);
    presentation::add_rule(p, 01_w, 10_w);
    presentation::add_rule(p, 000_w, 00_w);

    Congruence cong(twosided, p);
    cong.add_pair({0}, {1});

    word_type x = "0"_w + pow(1_w, 20);
    word_type y = "00"_w + pow(1_w, 20);

    REQUIRE(cong.contains(x, y));
    REQUIRE(cong.contains(y, x));
    REQUIRE(cong.contains(x, x));
    REQUIRE(cong.contains(y, y));
  }

  LIBSEMIGROUPS_TEST_CASE("Congruence",
                          "020",
                          "trivial cong. on an fp semigroup",
                          "[quick][cong][no-valgrind]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("ab");
    presentation::add_rule(p, "ab", "ba");
    presentation::add_rule(p, "a", "b");

    Congruence cong(left, p);

    // No generating pairs for the congruence (not the fp semigroup) means no
    // non-trivial classes.
    // REQUIRE(cong.number_of_non_trivial_classes() == 0);
    REQUIRE(cong.number_of_classes() == POSITIVE_INFINITY);
    REQUIRE(cong.has<KnuthBendix>());
    REQUIRE(knuth_bendix::non_trivial_classes(*cong.get<KnuthBendix>(),
                                              *cong.get<KnuthBendix>())
                .empty());
    REQUIRE(cong.finished());
    REQUIRE(cong.started());
    REQUIRE_THROWS_AS(cong.add_pair(00_w, 0_w), LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("Congruence",
                          "021",
                          "duplicate generators",
                          "[quick][cong]") {
    auto rg      = ReportGuard(false);
    using Transf = LeastTransf<8>;
    FroidurePin<Transf> S({Transf({7, 3, 5, 3, 4, 2, 7, 7}),
                           Transf({7, 3, 5, 3, 4, 2, 7, 7}),
                           Transf({7, 3, 5, 3, 4, 2, 7, 7}),
                           Transf({3, 6, 3, 4, 0, 6, 0, 7})});
    Congruence          cong(twosided, S);
    REQUIRE(cong.number_of_classes() == S.size());
  }

  LIBSEMIGROUPS_TEST_CASE("Congruence",
                          "022",
                          "non-trivial classes",
                          "[quick][cong]") {
    auto rg = ReportGuard(false);

    Presentation<word_type> p;
    p.alphabet(2);
    presentation::add_rule(p, 000_w, 0_w);
    presentation::add_rule(p, 100_w, 10_w);
    presentation::add_rule(p, 10111_w, 10_w);
    presentation::add_rule(p, 11111_w, 11_w);
    presentation::add_rule(p, 110110_w, 101011_w);
    presentation::add_rule(p, 0010110_w, 010110_w);
    presentation::add_rule(p, 0011010_w, 011010_w);
    presentation::add_rule(p, 0101010_w, 101010_w);
    presentation::add_rule(p, 1010101_w, 101010_w);
    presentation::add_rule(p, 1010110_w, 101011_w);
    presentation::add_rule(p, 1011010_w, 101101_w);
    presentation::add_rule(p, 1101010_w, 101010_w);
    presentation::add_rule(p, 1111010_w, 1010_w);
    presentation::add_rule(p, 00111010_w, 111010_w);

    Congruence cong(twosided, p);
    cong.add_pair(0_w, 1_w);
    REQUIRE(cong.number_of_classes() == 1);
    if (cong.has<ToddCoxeter>() && cong.get<ToddCoxeter>()->finished()) {
      ToddCoxeter tc(twosided, p);
      REQUIRE(tc.number_of_classes() == 78);
      auto ntc
          = todd_coxeter::non_trivial_classes(*cong.get<ToddCoxeter>(), tc);
      REQUIRE(ntc.size() == 1);
      REQUIRE(ntc[0].size() == 78);
    }
    if (cong.has<KnuthBendix>() && cong.get<KnuthBendix>()->finished()) {
      KnuthBendix kb(twosided, p);
      REQUIRE(kb.number_of_classes() == 78);
      auto ntc
          = knuth_bendix::non_trivial_classes(kb, *cong.get<KnuthBendix>());
      REQUIRE(ntc.size() == 1);
      REQUIRE(ntc[0].size() == 78);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("Congruence",
                          "023",
                          "right congruence on finite semigroup",
                          "[quick][cong][no-valgrind]") {
    auto rg      = ReportGuard(false);
    using Transf = LeastTransf<8>;
    FroidurePin<Transf> S({Transf({0, 1, 2, 3, 4, 5, 6, 7}),
                           Transf({1, 2, 3, 4, 5, 0, 6, 7}),
                           Transf({1, 0, 2, 3, 4, 5, 6, 7}),
                           Transf({0, 1, 2, 3, 4, 0, 6, 7}),
                           Transf({0, 1, 2, 3, 4, 5, 7, 6})});

    REQUIRE(S.size() == 93'312);
    std::vector<Transf> elms = {Transf({0, 0, 0, 0, 0, 0, 7, 6}),
                                Transf({0, 0, 0, 0, 0, 0, 6, 7}),
                                Transf({0, 0, 0, 0, 0, 0, 6, 7}),
                                Transf({1, 1, 1, 1, 1, 1, 6, 7}),
                                Transf({0, 0, 0, 0, 0, 0, 6, 7}),
                                Transf({2, 2, 2, 2, 2, 2, 6, 7}),
                                Transf({0, 0, 0, 0, 0, 0, 6, 7}),
                                Transf({3, 3, 3, 3, 3, 3, 6, 7}),
                                Transf({0, 0, 0, 0, 0, 0, 6, 7}),
                                Transf({4, 4, 4, 4, 4, 4, 6, 7}),
                                Transf({0, 0, 0, 0, 0, 0, 6, 7}),
                                Transf({5, 5, 5, 5, 5, 5, 6, 7}),
                                Transf({0, 0, 0, 0, 0, 0, 7, 6}),
                                Transf({0, 1, 2, 3, 4, 5, 7, 6})};
    REQUIRE(
        std::all_of(elms.cbegin(), elms.cend(), [&S](Transf const& x) -> bool {
          return S.contains(x);
        }));

    Congruence cong(right, S);
    word_type  w1, w2;
    for (size_t i = 0; i < elms.size(); i += 2) {
      S.factorisation(w1, S.position(elms[i]));
      S.factorisation(w2, S.position(elms[i + 1]));
      cong.add_pair(w1, w2);
    }
    REQUIRE(cong.number_of_classes() == 1);
  }

  LIBSEMIGROUPS_TEST_CASE("Congruence",
                          "024",
                          "redundant generating pairs",
                          "[quick][cong]") {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.alphabet(1);

    Congruence cong(twosided, p);
    cong.add_pair(00_w, 00_w);
    REQUIRE(cong.contains(00_w, 00_w));
  }

  LIBSEMIGROUPS_TEST_CASE("Congruence",
                          "025",
                          "2-sided cong. on free semigroup",
                          "[quick][cong]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("a");
    Congruence cong(twosided, p);
    REQUIRE(cong.contains(00_w, 00_w));
    REQUIRE(!cong.contains(00_w, 0_w));
  }

  LIBSEMIGROUPS_TEST_CASE("Congruence",
                          "026",
                          "is_obviously_(in)finite",
                          "[quick][cong]") {
    auto rg = ReportGuard(false);
    {
      Presentation<word_type> p;
      p.alphabet(3);
      presentation::add_rule(p, {0, 1}, {0});
      Congruence cong(twosided, p);
      cong.add_pair({2, 2}, {2});
      REQUIRE(is_obviously_infinite(cong));
      REQUIRE(cong.number_of_classes() == POSITIVE_INFINITY);
    }
    {
      Presentation<word_type> p;
      p.alphabet(3);
      presentation::add_rule(p, {0, 1}, {0});
      presentation::add_rule(p, {0, 0}, {0});
      Congruence cong(twosided, p);
      cong.add_pair({1, 1}, {1});
      REQUIRE(is_obviously_infinite(cong));
    }
    {
      Presentation<word_type> p;
      p.alphabet(3);
      presentation::add_rule(p, {0, 1}, {0});
      presentation::add_rule(p, {0, 0}, {0});
      Congruence cong(twosided, p);
      cong.add_pair({1, 2}, {1});
      REQUIRE(is_obviously_infinite(cong));
    }
    {
      Presentation<word_type> p;
      p.alphabet(3);
      presentation::add_rule(p, {0, 1}, {0});
      Congruence cong(right, p);
      cong.add_pair({2, 2}, {2});
      REQUIRE(is_obviously_infinite(cong));
    }
    {
      Presentation<word_type> p;
      p.alphabet(3);
      presentation::add_rule(p, {0, 1}, {0});
      presentation::add_rule(p, {0, 0}, {0});
      Congruence cong(right, p);
      cong.add_pair({1, 1}, {1});
      REQUIRE(is_obviously_infinite(cong));
    }
    {
      Presentation<word_type> p;
      p.alphabet(3);
      presentation::add_rule(p, {0, 1}, {0});
      presentation::add_rule(p, {0, 0}, {0});
      Congruence cong(right, p);
      cong.add_pair({1, 2}, {1});
      REQUIRE(is_obviously_infinite(cong));
    }
    {
      Presentation<word_type> p;
      p.alphabet(3);
      presentation::add_rule(p, {0, 1}, {0});
      Congruence cong(left, p);
      cong.add_pair({2, 2}, {2});
      REQUIRE(is_obviously_infinite(cong));
    }
    {
      Presentation<word_type> p;
      p.alphabet(3);
      presentation::add_rule(p, {0, 1}, {0});
      presentation::add_rule(p, {0, 0}, {0});
      Congruence cong(left, p);
      cong.add_pair({1, 1}, {1});
      REQUIRE(is_obviously_infinite(cong));
    }
    {
      Presentation<word_type> p;
      p.alphabet(3);
      presentation::add_rule(p, {0, 1}, {0});
      presentation::add_rule(p, {0, 0}, {0});
      Congruence cong(left, p);
      cong.add_pair({1, 2}, {1});
      REQUIRE(is_obviously_infinite(cong));
    }

    using Transf = LeastTransf<3>;
    FroidurePin<Transf> p({Transf({0, 1, 0}), Transf({0, 1, 2})});
    REQUIRE(p.size() == 2);
    {
      Congruence cong(twosided, p);
      cong.add_pair({1}, {0});
      REQUIRE(!is_obviously_infinite(cong));

      REQUIRE(cong.number_of_classes() == 1);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("Congruence",
                          "028",
                          "2-sided congruences of BMat8 semigroup",
                          "[quick][cong][no-valgrind]") {
#ifdef LIBSEMIGROUPS_HPCOMBI_ENABLED
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#pragma GCC diagnostic ignored "-Winline"
#endif
    auto rg    = ReportGuard(false);
    using BMat = FastestBMat<4>;
    std::vector<BMat> gens
        = {BMat({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
           BMat({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
           BMat({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {1, 0, 0, 1}}),
           BMat({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}})};
    {
      FroidurePin<BMat> S(gens);

      Congruence cong(twosided, S);
      cong.add_pair({1}, {0});

      REQUIRE(cong.number_of_classes() == 3);
      REQUIRE(cong.contains({1}, {0}));

      auto ntc = congruence::non_trivial_classes(cong, S.normal_forms());
      REQUIRE(ntc.size() == 3);
      REQUIRE(ntc[0].size() == 12);
      REQUIRE(ntc[1].size() == 63'880);
      REQUIRE(ntc[2].size() == 12);
      REQUIRE(ntc[0]
              == std::vector<word_type>({0_w,
                                         1_w,
                                         010_w,
                                         011_w,
                                         101_w,
                                         110_w,
                                         111_w,
                                         01011_w,
                                         01101_w,
                                         10110_w,
                                         10111_w,
                                         11011_w}));
    }
    {
      FroidurePin<BMat> S({gens[0], gens[2], gens[3]});
      Congruence        cong(twosided, S);
      cong.add_pair({1}, {0});

      REQUIRE(cong.number_of_classes() == 2);
      REQUIRE(cong.contains({1}, {0}));

      auto ntc = congruence::non_trivial_classes(cong, S.normal_forms());
      REQUIRE(ntc.size() == 2);
      REQUIRE(ntc[0].size() == 8);
      REQUIRE(ntc[1].size() == 8);

      REQUIRE(ntc[0]
              == std::vector<word_type>(
                  {0_w, 1_w, 00_w, 01_w, 10_w, 010_w, 101_w, 0101_w}));
    }
#ifdef LIBSEMIGROUPS_HPCOMBI_ENABLED
#pragma GCC diagnostic pop
#endif
  }

  LIBSEMIGROUPS_TEST_CASE("Congruence",
                          "029",
                          "left congruence on finite semigroup",
                          "[quick][cong]") {
    auto                  rg = ReportGuard(false);
    FroidurePin<Transf<>> S;
    S.add_generator(Transf<>({1, 3, 4, 2, 3}));
    S.add_generator(Transf<>({3, 2, 1, 3, 3}));

    REQUIRE(S.size() == 88);
    REQUIRE(S.degree() == 5);

    Congruence cong(left, S);

    auto l = 010001100_w;
    auto r = 10001_w;

    cong.add_pair(l, r);

    REQUIRE(cong.number_of_classes() == 69);
    REQUIRE(cong.number_of_classes() == 69);

    word_type w3 = S.factorisation(Transf<>({1, 3, 1, 3, 3}));
    word_type w4 = S.factorisation(Transf<>({4, 2, 4, 4, 2}));
    REQUIRE(!cong.contains(w3, w4));
    REQUIRE(cong.contains(w3, 00101_w));
    REQUIRE(cong.contains(100101_w, 0010001_w));
    REQUIRE(!cong.contains(011000_w, 11_w));
    REQUIRE(!cong.contains(10001000_w, 1001_w));

    REQUIRE(cong.contains(100101_w, 0010001_w));
    REQUIRE(!cong.contains(10001000_w, 1001_w));
  }

  LIBSEMIGROUPS_TEST_CASE("Congruence",
                          "030",
                          "right congruence on finite semigroup",
                          "[quick][cong]") {
    auto                  rg = ReportGuard(false);
    FroidurePin<Transf<>> S;
    S.add_generator(Transf<>({1, 3, 4, 2, 3}));
    S.add_generator(Transf<>({3, 2, 1, 3, 3}));

    // REQUIRE(S.size() == 88);
    // REQUIRE(S.degree() == 5);
    Congruence cong(right, S);
    cong.add_pair(010001100_w, 10001_w);

    REQUIRE(cong.number_of_classes() == 72);
    REQUIRE(cong.number_of_classes() == 72);

    word_type w3 = S.factorisation(Transf<>({1, 3, 1, 3, 3}));
    word_type w4 = S.factorisation(Transf<>({4, 2, 4, 4, 2}));
    REQUIRE(!cong.contains(w3, w4));
    REQUIRE(!cong.contains(w3, 00101_w));
    REQUIRE(!cong.contains(100101_w, 0010001_w));
    REQUIRE(!cong.contains(011000_w, 11_w));
    REQUIRE(!cong.contains(10001000_w, 1001_w));

    REQUIRE(!cong.contains(100101_w, 0010001_w));
    REQUIRE(!cong.contains(10001000_w, 1001_w));
  }

  LIBSEMIGROUPS_TEST_CASE("Congruence",
                          "031",
                          "right congruence on finite semigroup",
                          "[quick][cong]") {
    auto                  rg = ReportGuard(false);
    FroidurePin<Transf<>> S;
    S.add_generator(Transf<>({1, 3, 4, 2, 3}));
    S.add_generator(Transf<>({3, 2, 1, 3, 3}));

    REQUIRE(S.size() == 88);
    REQUIRE(S.number_of_rules() == 18);
    REQUIRE(S.degree() == 5);
    word_type w1, w2;
    S.factorisation(w1, S.position(Transf<>({3, 4, 4, 4, 4})));
    S.factorisation(w2, S.position(Transf<>({3, 1, 3, 3, 3})));
    Congruence cong(right, S);
    cong.add_pair(w1, w2);

    REQUIRE(cong.number_of_classes() == 72);
    REQUIRE(cong.number_of_classes() == 72);
    word_type w3, w4, w5, w6;
    S.factorisation(w3, S.position(Transf<>({1, 3, 3, 3, 3})));
    S.factorisation(w4, S.position(Transf<>({4, 2, 4, 4, 2})));
    S.factorisation(w5, S.position(Transf<>({2, 3, 2, 2, 2})));
    S.factorisation(w6, S.position(Transf<>({2, 3, 3, 3, 3})));
    REQUIRE(!cong.contains(w3, w4));
    REQUIRE(cong.contains(w5, w6));
    REQUIRE(!cong.contains(w3, w6));

    REQUIRE(cong.contains(w1, w2));
    REQUIRE(cong.contains(w5, w6));
    REQUIRE(!cong.contains(w3, w5));
  }

  LIBSEMIGROUPS_TEST_CASE("Congruence", "032", "contains", "[quick][cong]") {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.alphabet(2);
    Congruence cong(twosided, p);
    cong.add_pair(00_w, 0_w);
    cong.add_pair(01_w, 0_w);
    cong.add_pair(10_w, 0_w);
    cong.run();
    REQUIRE(cong.contains(00_w, 0_w));
    REQUIRE(cong.contains(01_w, 0_w));
    REQUIRE(cong.contains(10_w, 0_w));
    REQUIRE(cong.contains(10_w, 0101_w));
    REQUIRE(!cong.contains(11_w, 1_w));
  }

  LIBSEMIGROUPS_TEST_CASE("Congruence",
                          "033",
                          "stellar_monoid S2",
                          "[quick][cong]") {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p  = rook_monoid(2, 0);

    REQUIRE(!is_obviously_infinite(p));

    Congruence cong(twosided, p);
    auto       q = stellar_monoid(2);
    for (auto it = q.rules.cbegin(); it != q.rules.cend(); it += 2) {
      cong.add_pair(*it, *(it + 1));
    }

    REQUIRE(!is_obviously_infinite(cong));
    REQUIRE(cong.number_of_classes() == 5);

    ToddCoxeter tc(twosided, p);
    REQUIRE(tc.number_of_classes() == 7);

    auto ntc
        = congruence::non_trivial_classes(cong, todd_coxeter::normal_forms(tc));
    REQUIRE(ntc.size() == 1);
    std::sort(ntc[0].begin(), ntc[0].end());
    REQUIRE(ntc[0] == std::vector<word_type>({010_w, 10_w, 101_w}));
  }

  LIBSEMIGROUPS_TEST_CASE("Congruence",
                          "034",
                          "stellar_monoid S3",
                          "[quick][cong]") {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p  = rook_monoid(3, 0);

    REQUIRE(!is_obviously_infinite(p));

    Congruence cong(twosided, p);
    auto       q = stellar_monoid(3);
    for (auto it = q.rules.cbegin(); it != q.rules.cend(); it += 2) {
      cong.add_pair(*it, *(it + 1));
    }
    REQUIRE(!is_obviously_infinite(cong));
    REQUIRE(cong.number_of_classes() == 16);

    ToddCoxeter tc(twosided, p);
    REQUIRE(tc.number_of_classes() == 34);

    auto ntc
        = congruence::non_trivial_classes(cong, todd_coxeter::normal_forms(tc));
    REQUIRE(ntc.size() == 4);
    std::for_each(ntc.begin(), ntc.end(), [](auto& val) {
      std::sort(val.begin(), val.end());
    });
    std::sort(ntc.begin(), ntc.end());
    REQUIRE(ntc
            == decltype(ntc)({{010_w, 10_w, 101_w},
                              {0102_w, 1012_w, 102_w},
                              {01021_w, 10121_w, 1021_w},
                              {010210_w,
                               01210_w,
                               012101_w,
                               0210_w,
                               101210_w,
                               1012101_w,
                               10210_w,
                               1210_w,
                               12101_w,
                               121012_w,
                               210_w,
                               2101_w,
                               21012_w}}));
  }

  LIBSEMIGROUPS_TEST_CASE("Congruence",
                          "035",
                          "stellar_monoid S4",
                          "[quick][cong]") {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p  = rook_monoid(4, 0);

    REQUIRE(!is_obviously_infinite(p));

    Congruence cong(twosided, p);
    auto       q = stellar_monoid(4);
    for (auto it = q.rules.cbegin(); it != q.rules.cend(); it += 2) {
      cong.add_pair(*it, *(it + 1));
    }

    REQUIRE(!is_obviously_infinite(cong));
    REQUIRE(cong.number_of_classes() == 65);

    ToddCoxeter tc(twosided, p);
    REQUIRE(tc.number_of_classes() == 209);

    auto ntc
        = congruence::non_trivial_classes(cong, todd_coxeter::normal_forms(tc));
    REQUIRE(ntc.size() == 17);

    auto sizes = sizes_ntc(ntc);
    REQUIRE(sizes
            == std::vector<size_t>(
                {3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 13, 13, 13, 13, 73}));
    REQUIRE(std::accumulate(sizes.cbegin(), sizes.cend(), 0)
                + (cong.number_of_classes() - ntc.size())
            == 209);
  }

  LIBSEMIGROUPS_TEST_CASE("Congruence",
                          "036",
                          "stellar_monoid S5",
                          "[quick][cong][no-valgrind]") {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p  = rook_monoid(5, 0);

    Congruence cong(twosided, p);
    auto       q = stellar_monoid(5);
    for (auto it = q.rules.cbegin(); it != q.rules.cend(); it += 2) {
      cong.add_pair(*it, *(it + 1));
    }

    REQUIRE(!is_obviously_infinite(cong));
    REQUIRE(cong.number_of_classes() == 326);

    ToddCoxeter tc(twosided, p);
    REQUIRE(tc.number_of_classes() == 1'546);

    auto ntc
        = congruence::non_trivial_classes(cong, todd_coxeter::normal_forms(tc));
    REQUIRE(ntc.size() == 86);

    auto sizes = sizes_ntc(ntc);
    // REQUIRE(cong.number_of_non_trivial_classes() == 86);

    REQUIRE(std::count(sizes.cbegin(), sizes.cend(), 3) == 60);
    REQUIRE(std::count(sizes.cbegin(), sizes.cend(), 13) == 20);
    REQUIRE(std::count(sizes.cbegin(), sizes.cend(), 73) == 5);
    REQUIRE(std::count(sizes.cbegin(), sizes.cend(), 501) == 1);
    REQUIRE(std::accumulate(sizes.cbegin(), sizes.cend(), 0)
                + (cong.number_of_classes() - ntc.size())
            == tc.number_of_classes());
  }

  LIBSEMIGROUPS_TEST_CASE("Congruence",
                          "037",
                          "stellar_monoid S6",
                          "[quick][cong][no-valgrind]") {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p  = rook_monoid(6, 0);

    Congruence cong(twosided, p);
    auto       q = stellar_monoid(6);
    for (auto it = q.rules.cbegin(); it != q.rules.cend(); it += 2) {
      cong.add_pair(*it, *(it + 1));
    }

    REQUIRE(!is_obviously_infinite(cong));
    REQUIRE(cong.number_of_classes() == 1'957);

    ToddCoxeter tc(twosided, p);
    REQUIRE(tc.number_of_classes() == 13'327);

    auto ntc
        = congruence::non_trivial_classes(cong, todd_coxeter::normal_forms(tc));
    REQUIRE(ntc.size() == 517);

    REQUIRE(unique_sizes_ntc(ntc)
            == std::vector<size_t>({3, 13, 73, 501, 4051}));
    auto sizes = sizes_ntc(ntc);
    REQUIRE(std::accumulate(sizes.cbegin(), sizes.cend(), 0)
                + (cong.number_of_classes() - ntc.size())
            == tc.number_of_classes());
  }

  LIBSEMIGROUPS_TEST_CASE("Congruence",
                          "038",
                          "stellar_monoid S7",
                          "[quick][cong][no-valgrind]") {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p  = rook_monoid(7, 0);

    Congruence cong(twosided, p);
    auto       q = stellar_monoid(7);
    for (auto it = q.rules.cbegin(); it != q.rules.cend(); it += 2) {
      cong.add_pair(*it, *(it + 1));
    }

    REQUIRE(!is_obviously_infinite(cong));
    REQUIRE(cong.number_of_classes() == 13'700);

    ToddCoxeter tc(twosided, p);
    REQUIRE(tc.number_of_classes() == 130'922);

    auto ntc
        = congruence::non_trivial_classes(cong, todd_coxeter::normal_forms(tc));
    REQUIRE(ntc.size() == 3'620);

    REQUIRE(unique_sizes_ntc(ntc)
            == std::vector<size_t>({3, 13, 73, 501, 4'051, 37'633}));
    auto sizes = sizes_ntc(ntc);
    REQUIRE(std::accumulate(sizes.cbegin(), sizes.cend(), 0)
                + (cong.number_of_classes() - ntc.size())
            == tc.number_of_classes());
  }

  LIBSEMIGROUPS_TEST_CASE("Congruence",
                          "039",
                          "left cong. on an f.p. semigroup",
                          "[quick][cong]") {
    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet("abe");
    presentation::add_identity_rules(p, 'e');
    presentation::add_rule(p, "abb", "bb");
    presentation::add_rule(p, "bbb", "bb");
    presentation::add_rule(p, "aaaa", "a");
    presentation::add_rule(p, "baab", "bb");
    presentation::add_rule(p, "baaab", "b");
    presentation::add_rule(p, "babab", "b");
    presentation::add_rule(p, "bbaaa", "bb");
    presentation::add_rule(p, "bbaba", "bbaa");

    Congruence cong1(left, p);
    cong1.add_pair(0_w, 111_w);
    REQUIRE(cong1.number_of_classes() == 11);

    Congruence cong2(left, p);
    cong2.add_pair(11_w, 0000000_w);
    REQUIRE(cong1.number_of_classes() == cong2.number_of_classes());
  }

  LIBSEMIGROUPS_TEST_CASE("Congruence",
                          "040",
                          "2-sided cong. on infinite f.p. semigroup",
                          "[quick][cong]") {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.alphabet(3);

    Congruence cong(twosided, p);
    cong.add_pair(1_w, 2_w);
    cong.add_pair(00_w, 0_w);
    cong.add_pair(01_w, 10_w);
    cong.add_pair(01_w, 1_w);
    cong.add_pair(02_w, 20_w);
    cong.add_pair(02_w, 2_w);

    REQUIRE(!cong.contains(1_w, 2222222222_w));
  }

  LIBSEMIGROUPS_TEST_CASE("Congruence",
                          "042",
                          "const_contains",
                          "[quick][cong]") {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.alphabet(2);
    Congruence cong(twosided, p);
    cong.add_pair(000_w, 0_w);
    cong.add_pair(1111_w, 1_w);
    cong.add_pair(01110_w, 00_w);
    cong.add_pair(1001_w, 11_w);
    cong.add_pair(001010101010_w, 00_w);

    REQUIRE(!cong.contains(1111_w, 11_w));
    REQUIRE(cong.contains(1111_w, 1_w));
    if (cong.has<ToddCoxeter>()) {
      REQUIRE_NOTHROW(cong.get<ToddCoxeter>());
    }
    if (cong.has<KnuthBendix>()) {
      REQUIRE_NOTHROW(cong.get<KnuthBendix>());
    }
  }

  LIBSEMIGROUPS_TEST_CASE("Congruence", "043", "no winner", "[quick][cong]") {
    auto                    rg = ReportGuard(false);
    Presentation<word_type> p;
    p.alphabet(2);

    for (auto const& knd : {left, right, twosided}) {
      Congruence cong(knd, p);
      // Required in case of using a 1 core computer, otherwise the tests below
      // fail.
      REQUIRE(cong.contains(0000_w, 0000_w));
      REQUIRE(!cong.contains(0000_w, 0001_w));
      if (knd == twosided) {
        REQUIRE_NOTHROW(to_froidure_pin(cong));
      } else {
        REQUIRE_THROWS_AS(to_froidure_pin(cong), LibsemigroupsException);
      }

      Words w;
      w.letters(2).min(0).max(5);

      REQUIRE(w.count() == 31);

      REQUIRE(congruence::non_trivial_classes(cong, w).empty());
    }
  }

  LIBSEMIGROUPS_TEST_CASE("Congruence",
                          "044",
                          "congruence over smalloverlap",
                          "[quick][cong]") {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("abcdefg");
    presentation::add_rule(p, "abcd", "aaaeaa");
    Congruence cong(twosided, p);
    cong.add_pair(45_w, 36_w);
    REQUIRE(is_obviously_infinite(cong));
    REQUIRE(cong.number_of_generating_pairs() == 1);
    cong.run();
    REQUIRE(cong.number_of_classes() == POSITIVE_INFINITY);
    REQUIRE(cong.has<ToddCoxeter>());
    REQUIRE(cong.has<Kambites<word_type>>());
    REQUIRE(cong.get<Kambites<word_type>>()->finished());
    REQUIRE(!cong.get<ToddCoxeter>()->finished());

    Words w;
    w.letters(p.alphabet().size()).min(1).max(4);
    REQUIRE(w.count() == 399);
    REQUIRE(cong.get<Kambites<word_type>>()->presentation().alphabet()
            == word_type({0, 1, 2, 3, 4, 5, 6}));
    REQUIRE(congruence::non_trivial_classes(cong, w)
            == std::vector<std::vector<word_type>>({{36_w, 45_w},
                                                    {036_w, 045_w},
                                                    {136_w, 145_w},
                                                    {236_w, 245_w},
                                                    {336_w, 345_w},
                                                    {360_w, 450_w},
                                                    {361_w, 451_w},
                                                    {362_w, 452_w},
                                                    {363_w, 453_w},
                                                    {364_w, 454_w},
                                                    {365_w, 455_w},
                                                    {366_w, 456_w},
                                                    {436_w, 445_w},
                                                    {536_w, 545_w},
                                                    {636_w, 645_w}}));
  }

}  // namespace libsemigroups
