//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2024 Joseph Edwards
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

#include "catch.hpp"      // for REQUIRE, REQUIRE_THROWS_AS, REQUI...
#include "test-main.hpp"  // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/word-graph-with-sources.hpp"  // for WordGraphWithSources

namespace libsemigroups {

  struct LibsemigroupsException;  // forward decl

  LIBSEMIGROUPS_TEST_CASE("WordGraphWithSources",
                          "046",
                          "HopcroftKarp",
                          "[quick]") {
    WordGraphWithSources<size_t> x(
        to_word_graph<size_t>(3, {{0, 1, 2}, {0, 1, 2}, {0, 1, 2}}));

    WordGraphWithSources<size_t> y = x;

    HopcroftKarp<size_t> hk;

    WordGraphWithSources<size_t> xy;
    hk.join(xy, x, y);
    REQUIRE(xy == x);
    hk.join(xy, y, x);
    REQUIRE(xy == x);
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraphWithSources",
                          "050",
                          "hopcroft_karp_quotient",
                          "[quick]") {
    WordGraphWithSources<size_t> x(3, 3);
    x.set_target(0, 0, 1);
    x.set_target(0, 1, 1);
    x.set_target(0, 2, 1);
    x.set_target(1, 0, 2);
    x.set_target(1, 1, 2);
    x.set_target(1, 2, 2);
    x.set_target(2, 0, 2);
    x.set_target(2, 1, 2);
    x.set_target(2, 2, 2);

    WordGraphWithSources<size_t> y(3, 3);
    y.set_target(0, 0, 1);
    y.set_target(0, 1, 1);
    y.set_target(0, 2, 2);
    y.set_target(1, 0, 1);
    y.set_target(1, 1, 1);
    y.set_target(1, 2, 2);
    y.set_target(2, 0, 1);
    y.set_target(2, 1, 1);
    y.set_target(2, 2, 2);

    WordGraphWithSources<size_t> xy;

    HopcroftKarp<size_t> hk;
    hk.join(xy, x, y);

    REQUIRE(xy == to_word_graph<size_t>(2, {{1, 1, 1}, {1, 1, 1}}));
    REQUIRE(hk.is_subrelation_no_checks(x, 3, xy, 2));
    REQUIRE(hk.is_subrelation_no_checks(y, 3, xy, 2));
  }

  /*
    LIBSEMIGROUPS_TEST_CASE("WordGraphWithSources",
                            "048",
                            "quotient",
                            "[quick]") {
      WordGraphWithSources<size_t> dws1(4, 2);
      dws1.set_target(0, 0, 1);
      dws1.set_target(0, 1, 0);
      dws1.set_target(1, 0, 2);
      dws1.set_target(1, 1, 1);
      dws1.set_target(2, 0, 3);
      dws1.set_target(2, 1, 2);
      dws1.set_target(3, 0, 2);
      dws1.set_target(3, 1, 1);

      WordGraphWithSources<size_t> dws2(4, 2);
      dws2.set_target(0, 0, 1);
      dws2.set_target(0, 1, 0);
      dws2.set_target(1, 0, 2);
      dws2.set_target(1, 1, 1);
      dws2.set_target(2, 0, 3);
      dws2.set_target(2, 1, 2);
      dws2.set_target(3, 0, 2);
      dws2.set_target(3, 1, 1);

      WordGraphWithSources<size_t> dws3(4, 2);
      dws3.set_target(0, 0, 1);
      dws3.set_target(0, 1, 0);
      dws3.set_target(1, 0, 2);
      dws3.set_target(1, 1, 1);
      dws3.set_target(2, 0, 3);
      dws3.set_target(2, 1, 2);
      dws3.set_target(3, 0, 2);
      dws3.set_target(3, 1, 1);

      WordGraphWithSources<size_t> dws4(4, 2);
      dws4.set_target(0, 0, 1);
      dws4.set_target(0, 1, 0);
      dws4.set_target(1, 0, 2);
      dws4.set_target(1, 1, 1);
      dws4.set_target(2, 0, 3);
      dws4.set_target(2, 1, 2);
      dws4.set_target(3, 0, 2);
      dws4.set_target(3, 1, 1);

      detail::Duf<> uf1(4);
      uf1.unite(1, 3);
      dws1.quotient_digraph(uf1);

      detail::Duf<> uf2(4);
      uf2.unite(0, 3);
      dws2.quotient_digraph(uf2);

      detail::Duf<> uf3(4);
      dws3.quotient_digraph(uf3);

      detail::Duf<> uf4(4);
      uf4.unite(0, 1);
      uf4.unite(0, 2);
      uf4.unite(0, 3);
      dws4.quotient_digraph(uf4);

      REQUIRE(dws1.is_valid());
      REQUIRE(dws1 == word_graph::make<size_t>(3, {{1, 0}, {2, 1}, {1, 2}}));

      REQUIRE(dws2.is_valid());
      REQUIRE(dws2 == word_graph::make<size_t>(1, {{0, 0}}));

      REQUIRE(dws3.is_valid());
      REQUIRE(dws3
              == word_graph::make<size_t>(4, {{1, 0}, {2, 1}, {3, 2}, {2, 1}}));

      REQUIRE(dws4.is_valid());
      REQUIRE(dws4 == word_graph::make<size_t>(1, {{0, 0}}));
    }

    LIBSEMIGROUPS_TEST_CASE("WordGraphWithSources",
                            "049",
                            "out-of-place quotient",
                            "[quick]") {
      WordGraphWithSources<size_t> dws(4, 2);
      dws.set_target(0, 0, 1);
      dws.set_target(0, 1, 0);
      dws.set_target(1, 0, 2);
      dws.set_target(1, 1, 1);
      dws.set_target(2, 0, 3);
      dws.set_target(2, 1, 2);
      dws.set_target(3, 0, 2);
      dws.set_target(3, 1, 1);

      detail::Duf<> uf1(4);
      uf1.unite(1, 3);
      auto dws1 = dws.get_quotient(uf1);

      REQUIRE(dws1.is_valid());
      REQUIRE(dws1 == word_graph::make<size_t>(3, {{1, 0}, {2, 1}, {1, 2}}));

      detail::Duf<> uf2(4);
      uf2.unite(0, 3);
      auto dws2 = dws.get_quotient(uf2);

      REQUIRE(dws2.is_valid());
      REQUIRE(dws2 == word_graph::make<size_t>(1, {{0, 0}}));

      detail::Duf<> uf3(4);
      auto          dws3 = dws.get_quotient(uf3);

      REQUIRE(dws3.is_valid());
      REQUIRE(dws3
              == word_graph::make<size_t>(4, {{1, 0}, {2, 1}, {3, 2}, {2, 1}}));

      detail::Duf<> uf4(4);
      uf4.unite(0, 1);
      uf4.unite(0, 2);
      uf4.unite(0, 3);
      auto dws4 = dws.get_quotient(uf4);

      REQUIRE(dws4.is_valid());
      REQUIRE(dws4 == word_graph::make<size_t>(1, {{0, 0}}));
    }


    LIBSEMIGROUPS_TEST_CASE("WordGraphWithSources",
                            "051",
                            "out-of-place quotient",
                            "[quick]") {
      WordGraphWithSources<size_t> dws(4, 4);

      detail::Duf<> uf1(4);
      auto          dws1 = dws.get_quotient(uf1);

      detail::Duf<> uf2(4);
      uf2.unite(3, 1);
      uf2.unite(2, 3);
      auto dws2 = dws.get_quotient(uf2);

      REQUIRE(dws1.is_valid());
      REQUIRE(dws1
              == word_graph::make<size_t>(
                  4,
                  {{UNDEFINED, UNDEFINED, UNDEFINED, UNDEFINED},
                   {UNDEFINED, UNDEFINED, UNDEFINED, UNDEFINED},
                   {UNDEFINED, UNDEFINED, UNDEFINED, UNDEFINED},
                   {UNDEFINED, UNDEFINED, UNDEFINED, UNDEFINED}}));

      REQUIRE(dws2.is_valid());
      REQUIRE(dws2
              == word_graph::make<size_t>(
                  2,
                  {{UNDEFINED, UNDEFINED, UNDEFINED, UNDEFINED},
                   {UNDEFINED, UNDEFINED, UNDEFINED, UNDEFINED}}));
    }

    LIBSEMIGROUPS_TEST_CASE("WordGraphWithSources",
                            "052",
                            "out-of-place quotient",
                            "[quick]") {
      WordGraphWithSources<size_t> dws(10, 1);
      dws.set_target(0, 0, 1);
      dws.set_target(2, 0, 3);
      dws.set_target(4, 0, 5);
      dws.set_target(6, 0, 7);
      dws.set_target(8, 0, 9);

      detail::Duf<> uf1(10);
      uf1.unite(0, 2);
      uf1.unite(0, 4);
      uf1.unite(0, 6);
      uf1.unite(0, 8);

      auto dws_q1 = dws.get_quotient(uf1);

      REQUIRE(dws_q1.is_valid());
      REQUIRE(dws_q1 == word_graph::make<size_t>(2, {{1}, {UNDEFINED}}));

      detail::Duf<> uf2(10);
      uf2.unite(1, 7);
      uf2.unite(1, 3);
      uf2.unite(1, 5);
      uf2.unite(1, 9);

      auto dws_q2 = dws.get_quotient(uf2);

      REQUIRE(dws_q2.is_valid());
      REQUIRE(
          dws_q2
          == word_graph::make<size_t>(6, {{1}, {UNDEFINED}, {1}, {1}, {1},
    {1}}));
    }

    LIBSEMIGROUPS_TEST_CASE("WordGraphWithSources",
                            "053",
                            "quotient",
                            "[extreme]") {
      WordGraph<uint32_t> d = WordGraph<uint32_t>::random(1'000'000, 6);
      WordGraphWithSources<uint32_t> dws(1'000'000, 6);
      for (auto v = 0; v != 1'000'000; ++v) {
        for (auto a = 0; a != 6; ++a) {
          dws.set_target(v, a, d.target(v, a));
        }
      }
      detail::Duf<> uf(1'000'000);
      for (auto i = 2; i < 101; ++i) {
        uf.unite(1, i);
      }

      dws.quotient_digraph(uf);

      REQUIRE(dws.is_valid());
      REQUIRE(dws.number_of_nodes() <= d.number_of_nodes());
    }*/
}  // namespace libsemigroups
