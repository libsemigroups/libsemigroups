//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2023 Joseph Edwards
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
#include <cstdint>  // for uint32_t
#include <vector>   // for vector

#include "Catch2-3.7.1/catch_amalgamated.hpp"  // for REQUIRE, REQUIRE_THROWS_AS, REQUI...
#include "test-main.hpp"                       // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/word-graph.hpp"  // for WordGraph

#include "libsemigroups/detail/kbe.hpp"                      // for KBE
#include "libsemigroups/detail/word-graph-with-sources.hpp"  // for WordGraphWithSources

namespace libsemigroups {
  struct LibsemigroupsException;  // forward decl
  namespace detail {

    LIBSEMIGROUPS_TEST_CASE("WordGraphWithSources",
                            "044",
                            "constructor with 1  default arg",
                            "[quick][digraph]") {
      WordGraphWithSources<size_t> g;
      REQUIRE(g.number_of_nodes() == 0);
      REQUIRE(g.number_of_edges() == 0);
    }

    LIBSEMIGROUPS_TEST_CASE("WordGraphWithSources",
                            "045",
                            "constructor with 0 default args",
                            "[quick][digraph]") {
      for (size_t j = 0; j < 100; ++j) {
        WordGraphWithSources<size_t> g(j);
        REQUIRE(g.number_of_nodes() == j);
        REQUIRE(g.number_of_edges() == 0);
      }
    }

    LIBSEMIGROUPS_TEST_CASE("WordGraphWithSources",
                            "046",
                            "add nodes",
                            "[quick][digraph]") {
      WordGraphWithSources<size_t> g(3);
      REQUIRE(g.number_of_nodes() == 3);
      REQUIRE(g.number_of_edges() == 0);

      for (size_t i = 1; i < 100; ++i) {
        g.add_nodes(i);
        REQUIRE(g.number_of_nodes() == 3 + i * (i + 1) / 2);
      }
    }

    LIBSEMIGROUPS_TEST_CASE("WordGraphWithSources",
                            "047",
                            "add edges",
                            "[quick][digraph]") {
      WordGraph<size_t> g(17, 31);

      for (size_t i = 0; i < 17; ++i) {
        for (size_t j = 0; j < 31; ++j) {
          g.target_no_checks(i, j, (7 * i + 23 * j) % 17);
        }
      }

      REQUIRE(g.number_of_edges() == 31 * 17);
      REQUIRE(g.number_of_nodes() == 17);

      for (size_t i = 0; i < 17; ++i) {
        for (size_t j = 0; j < 31; ++j) {
          REQUIRE(g.target(i, j) == (7 * i + 23 * j) % 17);
        }
      }

      g.add_to_out_degree(10);
      REQUIRE(g.out_degree() == 41);
      REQUIRE(g.number_of_nodes() == 17);

      for (size_t i = 0; i < 17; ++i) {
        for (size_t j = 0; j < 10; ++j) {
          g.target_no_checks(i, 31 + j, (7 * i + 23 * j) % 17);
        }
      }

      REQUIRE(g.number_of_edges() == 41 * 17);
      REQUIRE(g.number_of_nodes() == 17);
    }

    LIBSEMIGROUPS_TEST_CASE("WordGraphWithSources",
                            "048",
                            "exceptions",
                            "[quick][digraph]") {
      WordGraphWithSources<size_t> graph(10, 5);
      REQUIRE_THROWS_AS(graph.target(10, 0), LibsemigroupsException);
      REQUIRE(graph.target(0, 1) == UNDEFINED);

      for (size_t i = 0; i < 5; ++i) {
        graph.target_no_checks(0, 1, i);
        graph.target_no_checks(2, 2, i);
      }
      REQUIRE_NOTHROW(graph.target_no_checks(0, 1, 0));
      REQUIRE_NOTHROW(graph.target_no_checks(2, 2, 0));
    }

    LIBSEMIGROUPS_TEST_CASE("WordGraphWithSources",
                            "049",
                            "reserve",
                            "[quick][digraph]") {
      WordGraphWithSources<size_t> graph;
      graph.reserve(10, 10);
      REQUIRE(graph.number_of_nodes() == 0);
      REQUIRE(graph.number_of_edges() == 0);
      graph.add_nodes(1);
      REQUIRE(graph.number_of_nodes() == 1);
      graph.add_nodes(9);
      REQUIRE(graph.number_of_nodes() == 10);
      REQUIRE(graph.number_of_edges() == 0);
    }

    LIBSEMIGROUPS_TEST_CASE("WordGraphWithSources",
                            "050",
                            "default constructors",
                            "[quick][digraph]") {
      auto g1 = WordGraphWithSources<size_t>();
      g1.add_to_out_degree(1);
      word_graph::add_cycle(g1, 10);

      // Copy constructor
      auto g2(g1);
      REQUIRE(g2.number_of_edges() == 10);
      REQUIRE(g2.number_of_nodes() == 10);

      // Move constructor
      auto g3(std::move(g2));
      REQUIRE(g3.number_of_edges() == 10);
      REQUIRE(g3.number_of_nodes() == 10);

      // Copy assignment
      g2 = g3;
      REQUIRE(g2.number_of_edges() == 10);
      REQUIRE(g2.number_of_nodes() == 10);
    }

    LIBSEMIGROUPS_TEST_CASE("WordGraphWithSources",
                            "051",
                            "target_no_checks",
                            "[quick]") {
      size_t                       number_of_levels = 10;
      WordGraphWithSources<size_t> ad;
      ad.add_nodes(std::pow(2, number_of_levels) - 1);
      ad.add_to_out_degree(2);
      ad.target_no_checks(0, 0, 1);
      ad.target_no_checks(0, 1, 2);

      for (size_t i = 2; i <= number_of_levels; ++i) {
        size_t counter = std::pow(2, i - 1) - 1;
        for (size_t j = std::pow(2, i - 2) - 1; j < std::pow(2, i - 1) - 1;
             ++j) {
          ad.target_no_checks(j, 0, counter++);
          ad.target_no_checks(j, 1, counter++);
        }
      }
      REQUIRE(ad.target_no_checks(0, 1) == ad.target(0, 1));
    }

    LIBSEMIGROUPS_TEST_CASE("WordGraphWithSources",
                            "052",
                            "operator<<",
                            "[quick]") {
      WordGraphWithSources<uint32_t> ad;
      ad.add_nodes(3);
      ad.add_to_out_degree(2);
      ad.target_no_checks(0, 0, 1);
      ad.target_no_checks(1, 0, 0);
      ad.target_no_checks(2, 0, 2);

      std::ostringstream oss;
      oss << ad;
      REQUIRE(oss.str()
              == "{3, {{1, 4294967295}, {0, 4294967295}, {2, "
                 "4294967295}}}");
    }

    LIBSEMIGROUPS_TEST_CASE("WordGraphWithSources",
                            "053",
                            "quotient",
                            "[quick]") {
      WordGraphWithSources<size_t> dws1(0, 0);
      WordGraphWithSources<size_t> dws1_q(0, 0);
      Joiner                       joiner;
      REQUIRE_THROWS(joiner.is_subrelation(dws1, dws1_q));
    }

    LIBSEMIGROUPS_TEST_CASE("WordGraphWithSources",
                            "054",
                            "hopcroft_karp_quotient",
                            "[quick]") {
      WordGraphWithSources<size_t> d1(3, 3);
      d1.target_no_checks(0, 0, 0);
      d1.target_no_checks(0, 1, 1);
      d1.target_no_checks(0, 2, 2);
      d1.target_no_checks(1, 0, 0);
      d1.target_no_checks(1, 1, 1);
      d1.target_no_checks(1, 2, 2);
      d1.target_no_checks(2, 0, 0);
      d1.target_no_checks(2, 1, 1);
      d1.target_no_checks(2, 2, 2);

      WordGraphWithSources<size_t> d2(3, 3);
      d2.target_no_checks(0, 0, 0);
      d2.target_no_checks(0, 1, 1);
      d2.target_no_checks(0, 2, 2);
      d2.target_no_checks(1, 0, 0);
      d2.target_no_checks(1, 1, 1);
      d2.target_no_checks(1, 2, 2);
      d2.target_no_checks(2, 0, 0);
      d2.target_no_checks(2, 1, 1);
      d2.target_no_checks(2, 2, 2);

      WordGraphWithSources<size_t> d3;
      Joiner                       joiner;
      joiner(d3, d1, d2);

      REQUIRE(d3
              == make<WordGraph<size_t>>(3, {{0, 1, 2}, {0, 1, 2}, {0, 1, 2}}));
    }

  }  // namespace detail
}  // namespace libsemigroups
