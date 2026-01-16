//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2026 James D. Mitchell
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

#define CATCH_CONFIG_ENABLE_ALL_STRINGMAKERS

#include <algorithm>      // for find, count
#include <cstddef>        // for size_t
#include <functional>     // for mem_fn
#include <iterator>       // for begin, end
#include <memory>         // for static_pointer...
#include <numeric>        // for iota
#include <type_traits>    // for enable_if_t
#include <unordered_map>  // for operator!=
#include <utility>        // for declval, get
#include <vector>         // for vector

#include "Catch2-3.8.0/catch_amalgamated.hpp"  // for TEST_CASE
#include "test-main.hpp"                       // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/config.hpp"  // for LIBSEMIGROUPS_...

#include "libsemigroups/bmat-fastest.hpp"          // for BMatFastest
#include "libsemigroups/bmat8.hpp"                 // for BMat8
#include "libsemigroups/cong-class.hpp"            // for Congruence
#include "libsemigroups/cong-common-helpers.hpp"   // for contains, add_...
#include "libsemigroups/cong-helpers.hpp"          // for contains, add_...
#include "libsemigroups/constants.hpp"             // for operator!=
#include "libsemigroups/froidure-pin-base.hpp"     // for factorisation
#include "libsemigroups/froidure-pin.hpp"          // for FroidurePin, make
#include "libsemigroups/hpcombi.hpp"               // for BMat8
#include "libsemigroups/obvinf.hpp"                // for is_obviously_i...
#include "libsemigroups/order.hpp"                 // for lexicographica...
#include "libsemigroups/pbr.hpp"                   // for make, PBR
#include "libsemigroups/ranges.hpp"                // for rx::Ranges
#include "libsemigroups/runner.hpp"                // for delta
#include "libsemigroups/to-cong.hpp"               // for to<Congruence>
#include "libsemigroups/todd-coxeter-helpers.hpp"  // for index_of
#include "libsemigroups/transf.hpp"                // for Transf, make
#include "libsemigroups/types.hpp"                 // for word_type, con...
#include "libsemigroups/word-graph-helpers.hpp"    // for follow_path_no...
#include "libsemigroups/word-graph.hpp"            // for follow_path_no...
#include "libsemigroups/word-range.hpp"            // for operator""_w

#include "libsemigroups/detail/eigen.hpp"     // for eigen
#include "libsemigroups/detail/fmt.hpp"       // for format
#include "libsemigroups/detail/iterator.hpp"  // for operator+
#include "libsemigroups/detail/print.hpp"     // for to_printable
#include "libsemigroups/detail/report.hpp"    // for ReportGuard
#include "libsemigroups/detail/report.hpp"    // for report_default
#include "libsemigroups/detail/timer.hpp"     // for string_time

namespace libsemigroups {

  using congruence::contains;
  using congruence::non_trivial_classes;
  using congruence::reduce;

  // Forward declarations
  struct LibsemigroupsException;

  using literals::operator""_w;

  constexpr congruence_kind twosided = congruence_kind::twosided;
  constexpr congruence_kind onesided = congruence_kind::onesided;

  LIBSEMIGROUPS_TEST_CASE("to<Congruence>",
                          "000",
                          "2-sided congruence on finite semigroup",
                          "[quick][cong][no-valgrind]") {
    auto rg      = ReportGuard(false);
    using Transf = LeastTransf<8>;
    auto S       = make<FroidurePin>({Transf({7, 3, 5, 3, 4, 2, 7, 7}),
                                      Transf({1, 2, 4, 4, 7, 3, 0, 7}),
                                      Transf({0, 6, 4, 2, 2, 6, 6, 4}),
                                      Transf({3, 6, 3, 4, 0, 6, 0, 7})});

    REQUIRE(S.size() == 11804);
    REQUIRE(S.number_of_rules() == 2460);

    auto cong = to<Congruence<word_type>>(twosided, S, S.right_cayley_graph());
    congruence::add_generating_pair(cong, 0321322_w, 322133_w);

    REQUIRE(cong.number_of_classes() == 525);

    REQUIRE(congruence::contains(cong, 0001_w, 00100_w));
    REQUIRE(congruence::contains(cong, 00101_w, 1101_w));
    REQUIRE(!congruence::contains(cong, 1100_w, 0001_w));
    REQUIRE(!congruence::contains(cong, 003_w, 0001_w));
    REQUIRE(!congruence::contains(cong, 1100_w, 003_w));
    REQUIRE(congruence::contains(cong, 12133212_w, 2133210_w));
    REQUIRE(congruence::contains(cong, 0311132210_w, 03221_w));
    REQUIRE(!congruence::contains(cong, 0321333_w, 003_w));
    REQUIRE(!congruence::contains(cong, 110_w, 1332210_w));

    REQUIRE(congruence::contains(cong, 12133212_w, 2133210_w));
    REQUIRE(!congruence::contains(cong, 110_w, 1332210_w));

    REQUIRE(cong.number_of_classes() == 525);
  }

  // TODO(1) this does not seem to be functioning all that well, one of the
  // threads blocks the others from stopping, extending the time
  // taken for this to run.
  LIBSEMIGROUPS_TEST_CASE("to<Congruence>",
                          "001",
                          "congruence on full PBR monoid on 2 points",
                          "[extreme][cong]") {
    auto rg = ReportGuard(true);
    auto S  = make<FroidurePin>({make<PBR>({{2}, {3}, {0}, {1}}),
                                 make<PBR>({{}, {2}, {1}, {0, 3}}),
                                 make<PBR>({{0, 3}, {2}, {1}, {}}),
                                 make<PBR>({{1, 2}, {3}, {0}, {1}}),
                                 make<PBR>({{2}, {3}, {0}, {1, 3}}),
                                 make<PBR>({{3}, {1}, {0}, {1}}),
                                 make<PBR>({{3}, {2}, {0}, {0, 1}}),
                                 make<PBR>({{3}, {2}, {0}, {1}}),
                                 make<PBR>({{3}, {2}, {0}, {3}}),
                                 make<PBR>({{3}, {2}, {1}, {0}}),
                                 make<PBR>({{3}, {2, 3}, {0}, {1}})});

    // REQUIRE(S.size() == 65536);
    // REQUIRE(S.number_of_rules() == 45416);

    auto cong = to<Congruence<word_type>>(twosided, S, S.right_cayley_graph());
    congruence::add_generating_pair(
        cong, {7, 10, 9, 3, 6, 9, 4, 7, 9, 10}, {9, 3, 6, 6, 10, 9, 4, 7});
    congruence::add_generating_pair(
        cong, {8, 7, 5, 8, 9, 8}, {6, 3, 8, 6, 1, 2, 4});

    cong.run();
    // auto tc = cong.get<ToddCoxeter<std::string>>();
    // tc->lookahead_extent(ToddCoxeter<std::string>::options::lookahead_extent::full);
    // FIXME(1) without the lookahead we get the wrong answer here.
    // FIXME(1) the next line, without the previous, triggers a seg fault.
    // tc->perform_lookahead(false);
    REQUIRE(cong.number_of_classes() == 19'009);
    auto ntc
        = congruence::non_trivial_classes(cong, froidure_pin::normal_forms(S));
    REQUIRE(ntc.size() == 577);

    std::vector<size_t> v(577, 0);
    std::transform(ntc.cbegin(),
                   ntc.cend(),
                   v.begin(),
                   std::mem_fn(&std::vector<word_type>::size));
    REQUIRE(std::count(v.cbegin(), v.cend(), 4) == 384);
    REQUIRE(std::count(v.cbegin(), v.cend(), 16) == 176);
    REQUIRE(std::count(v.cbegin(), v.cend(), 96) == 16);
    REQUIRE(std::count(v.cbegin(), v.cend(), 41216) == 1);
  }

  LIBSEMIGROUPS_TEST_CASE("to<Congruence>",
                          "002",
                          "2-sided congruence on finite semigroup",
                          "[quick][cong][no-valgrind]") {
    auto rg = ReportGuard(false);

    auto S
        = make<FroidurePin>({LeastPPerm<6>({0, 1, 2}, {4, 0, 1}, 6),
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

    auto cong = to<Congruence<word_type>>(twosided, S, S.right_cayley_graph());
    congruence::add_generating_pair(cong, {2, 7}, {1, 6, 6, 1});
    REQUIRE(cong.number_of_classes() == 32);
  }

  LIBSEMIGROUPS_TEST_CASE("to<Congruence>",
                          "003",
                          "2-sided congruence on finite semigroup",
                          "[quick][cong]") {
    auto rg      = ReportGuard(false);
    using Transf = LeastTransf<5>;
    auto S
        = make<FroidurePin>({Transf({1, 3, 4, 2, 3}), Transf({3, 2, 1, 3, 3})});

    REQUIRE(S.size() == 88);
    REQUIRE(S.number_of_rules() == 18);

    word_type w1 = froidure_pin::factorisation(S, Transf({3, 4, 4, 4, 4}));
    word_type w2 = froidure_pin::factorisation(S, Transf({3, 4, 4, 4, 4}));

    auto cong = to<Congruence<word_type>>(twosided, S, S.right_cayley_graph());
    congruence::add_generating_pair(
        cong,
        froidure_pin::factorisation(S, Transf({3, 4, 4, 4, 4})),
        froidure_pin::factorisation(S, Transf({3, 1, 3, 3, 3})));
    REQUIRE(cong.number_of_classes() == 21);

    word_type u = froidure_pin::factorisation(S, Transf({1, 3, 1, 3, 3}));
    word_type v = froidure_pin::factorisation(S, Transf({4, 2, 4, 4, 2}));
    REQUIRE(congruence::contains(cong, u, v));
    REQUIRE(congruence::contains(cong, u, v));
  }

  LIBSEMIGROUPS_TEST_CASE("to<Congruence>",
                          "004",
                          "duplicate generators",
                          "[quick][cong]") {
    auto rg      = ReportGuard(false);
    using Transf = LeastTransf<8>;
    auto S       = make<FroidurePin>({Transf({7, 3, 5, 3, 4, 2, 7, 7}),
                                      Transf({7, 3, 5, 3, 4, 2, 7, 7}),
                                      Transf({7, 3, 5, 3, 4, 2, 7, 7}),
                                      Transf({3, 6, 3, 4, 0, 6, 0, 7})});
    auto cong = to<Congruence<word_type>>(twosided, S, S.right_cayley_graph());
    REQUIRE(cong.number_of_classes() == S.size());
  }

  LIBSEMIGROUPS_TEST_CASE("to<Congruence>",
                          "005",
                          "onesided congruence on finite semigroup",
                          "[quick][cong][no-valgrind]") {
    auto rg      = ReportGuard(false);
    using Transf = LeastTransf<8>;
    auto S       = make<FroidurePin>({Transf({0, 1, 2, 3, 4, 5, 6, 7}),
                                      Transf({1, 2, 3, 4, 5, 0, 6, 7}),
                                      Transf({1, 0, 2, 3, 4, 5, 6, 7}),
                                      Transf({0, 1, 2, 3, 4, 0, 6, 7}),
                                      Transf({0, 1, 2, 3, 4, 5, 7, 6})});

    REQUIRE(S.size() == 93'312);
    std::vector elms = {Transf({0, 0, 0, 0, 0, 0, 7, 6}),
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

    auto cong = to<Congruence<word_type>>(onesided, S, S.right_cayley_graph());
    word_type w1, w2;
    for (size_t i = 0; i < elms.size(); i += 2) {
      froidure_pin::factorisation(S, w1, S.position(elms[i]));
      froidure_pin::factorisation(S, w2, S.position(elms[i + 1]));
      congruence::add_generating_pair(cong, w1, w2);
    }
    REQUIRE(cong.number_of_classes() == 1);
  }

  LIBSEMIGROUPS_TEST_CASE("to<Congruence>",
                          "006",
                          "2-sided congruences of BMat8 semigroup",
                          "[quick][cong][no-valgrind]") {
#ifdef LIBSEMIGROUPS_HPCOMBI_ENABLED
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#pragma GCC diagnostic ignored "-Winline"
#endif
    auto rg    = ReportGuard(false);
    using BMat = BMatFastest<4>;
    std::vector gens
        = {BMat({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
           BMat({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
           BMat({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {1, 0, 0, 1}}),
           BMat({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}})};
    {
      auto S = make<FroidurePin>(gens);

      auto cong
          = to<Congruence<word_type>>(twosided, S, S.right_cayley_graph());
      congruence::add_generating_pair(cong, {1}, {0});

      REQUIRE(cong.number_of_classes() == 3);
      REQUIRE(congruence::contains(cong, {1}, {0}));

      auto ntc = non_trivial_classes(cong, froidure_pin::normal_forms(S));
      REQUIRE(ntc.size() == 3);
      REQUIRE(ntc[0].size() == 12);
      REQUIRE(ntc[1].size() == 63'880);
      REQUIRE(ntc[2].size() == 12);
      REQUIRE(ntc[0]
              == std::vector({0_w,
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
      auto S = make<FroidurePin>({gens[0], gens[2], gens[3]});
      auto cong
          = to<Congruence<word_type>>(twosided, S, S.right_cayley_graph());
      congruence::add_generating_pair(cong, {1}, {0});

      REQUIRE(cong.number_of_classes() == 2);
      REQUIRE(congruence::contains(cong, {1}, {0}));

      auto ntc = non_trivial_classes(cong, froidure_pin::normal_forms(S));
      REQUIRE(ntc.size() == 2);
      REQUIRE(ntc[0].size() == 8);
      REQUIRE(ntc[1].size() == 8);

      REQUIRE(
          ntc[0]
          == std::vector({0_w, 1_w, 00_w, 01_w, 10_w, 010_w, 101_w, 0101_w}));
    }
#ifdef LIBSEMIGROUPS_HPCOMBI_ENABLED
#pragma GCC diagnostic pop
#endif
  }

  LIBSEMIGROUPS_TEST_CASE("to<Congruence>",
                          "007",
                          "left congruence on finite semigroup",
                          "[quick][cong]") {
    auto                  rg = ReportGuard(false);
    FroidurePin<Transf<>> S;
    S.add_generator(make<Transf<>>({1, 3, 4, 2, 3}));
    S.add_generator(make<Transf<>>({3, 2, 1, 3, 3}));

    REQUIRE(S.size() == 88);
    REQUIRE(S.degree() == 5);

    auto cong = to<Congruence<word_type>>(onesided, S, S.left_cayley_graph());

    congruence::add_generating_pair(cong, 001100010_w, 10001_w);

    REQUIRE(cong.number_of_classes() == 69);
    REQUIRE(cong.number_of_classes() == 69);

    word_type w3 = reverse(
        froidure_pin::factorisation(S, make<Transf<>>({1, 3, 1, 3, 3})));
    word_type w4 = reverse(
        froidure_pin::factorisation(S, make<Transf<>>({4, 2, 4, 4, 2})));
    REQUIRE(!congruence::contains(cong, w3, w4));
    REQUIRE(congruence::contains(cong, w3, 10100_w));
    REQUIRE(congruence::contains(cong, 101001_w, 1000100_w));
    REQUIRE(!congruence::contains(cong, 000110_w, 11_w));
    REQUIRE(!congruence::contains(cong, 00010001_w, 1001_w));

    REQUIRE(congruence::contains(cong, 101001_w, 1000100_w));
    REQUIRE(!congruence::contains(cong, 00010001_w, 1001_w));
  }

  LIBSEMIGROUPS_TEST_CASE("to<Congruence>",
                          "008",
                          "onesided congruence on finite semigroup",
                          "[quick][cong]") {
    auto                  rg = ReportGuard(false);
    FroidurePin<Transf<>> S;
    S.add_generator(make<Transf<>>({1, 3, 4, 2, 3}));
    S.add_generator(make<Transf<>>({3, 2, 1, 3, 3}));

    // REQUIRE(S.size() == 88);
    // REQUIRE(S.degree() == 5);
    auto cong = to<Congruence<word_type>>(onesided, S, S.right_cayley_graph());
    congruence::add_generating_pair(cong, 010001100_w, 10001_w);

    REQUIRE(cong.number_of_classes() == 72);
    REQUIRE(cong.number_of_classes() == 72);

    word_type w3
        = froidure_pin::factorisation(S, make<Transf<>>({1, 3, 1, 3, 3}));
    word_type w4
        = froidure_pin::factorisation(S, make<Transf<>>({4, 2, 4, 4, 2}));
    REQUIRE(!congruence::contains(cong, w3, w4));
    REQUIRE(!congruence::contains(cong, w3, 00101_w));
    REQUIRE(!congruence::contains(cong, 100101_w, 0010001_w));
    REQUIRE(!congruence::contains(cong, 011000_w, 11_w));
    REQUIRE(!congruence::contains(cong, 10001000_w, 1001_w));

    REQUIRE(!congruence::contains(cong, 100101_w, 0010001_w));
    REQUIRE(!congruence::contains(cong, 10001000_w, 1001_w));
  }

  LIBSEMIGROUPS_TEST_CASE("to<Congruence>",
                          "009",
                          "onesided congruence on finite semigroup",
                          "[quick][cong]") {
    auto                  rg = ReportGuard(false);
    FroidurePin<Transf<>> S;
    S.add_generator(make<Transf<>>({1, 3, 4, 2, 3}));
    S.add_generator(make<Transf<>>({3, 2, 1, 3, 3}));

    REQUIRE(S.size() == 88);
    REQUIRE(S.number_of_rules() == 18);
    REQUIRE(S.degree() == 5);
    word_type w1, w2;
    froidure_pin::factorisation(
        S, w1, S.position(make<Transf<>>({3, 4, 4, 4, 4})));
    froidure_pin::factorisation(
        S, w2, S.position(make<Transf<>>({3, 1, 3, 3, 3})));
    auto cong = to<Congruence<word_type>>(onesided, S, S.right_cayley_graph());
    congruence::add_generating_pair(cong, w1, w2);

    REQUIRE(cong.number_of_classes() == 72);
    REQUIRE(cong.number_of_classes() == 72);
    word_type w3, w4, w5, w6;
    froidure_pin::factorisation(
        S, w3, S.position(make<Transf<>>({1, 3, 3, 3, 3})));
    froidure_pin::factorisation(
        S, w4, S.position(make<Transf<>>({4, 2, 4, 4, 2})));
    froidure_pin::factorisation(
        S, w5, S.position(make<Transf<>>({2, 3, 2, 2, 2})));
    froidure_pin::factorisation(
        S, w6, S.position(make<Transf<>>({2, 3, 3, 3, 3})));
    REQUIRE(!congruence::contains(cong, w3, w4));
    REQUIRE(congruence::contains(cong, w5, w6));
    REQUIRE(!congruence::contains(cong, w3, w6));

    REQUIRE(congruence::contains(cong, w1, w2));
    REQUIRE(congruence::contains(cong, w5, w6));
    REQUIRE(!congruence::contains(cong, w3, w5));
  }

  LIBSEMIGROUPS_TEST_CASE("to<Congruence>",
                          "010",
                          "for a WordGraph",
                          "[quick][cong]") {
    // WARNING: the word graph must have a "root" node from which every other
    // node is reachable.

    // This is the right Cayley graph of the free band with 2 generators.
    auto wg = v4::make<WordGraph<uint32_t>>(
        7, {{1, 2}, {1, 3}, {4, 2}, {5, 3}, {4, 6}, {5, 3}, {4, 6}});
    auto cong = to<Congruence<word_type>>(twosided, wg);

    congruence::add_generating_pair(cong, 0_w, 1_w);

    REQUIRE(cong.number_of_classes() == 1);
  }

}  // namespace libsemigroups
