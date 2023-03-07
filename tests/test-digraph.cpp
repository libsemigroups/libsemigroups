//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019 Finn Smith
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

// TODO(v3):
// * renumber the tests

#include <cstddef>        // for size_t
#include <stdexcept>      // for runtime_error
#include <unordered_set>  // for unordered_set
#include <vector>         // for vector

#include "catch.hpp"      // for REQUIRE, REQUIRE_THROWS_AS, REQUI...
#include "test-main.hpp"  // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/digraph-helper.hpp"  // for follow_path
#include "libsemigroups/digraph.hpp"         // for ActionDigraph
#include "libsemigroups/forest.hpp"          // for Forest
#include "libsemigroups/kbe.hpp"             // for KBE
#include "libsemigroups/knuth-bendix.hpp"    // for KnuthBendix
#include "libsemigroups/words.hpp"            // for literals, Words

namespace libsemigroups {

  using namespace literals;
  using KnuthBendix = fpsemigroup::KnuthBendix;

  struct LibsemigroupsException;  // forward decl

  namespace {
    void add_clique(ActionDigraph<size_t>& digraph, size_t n) {
      if (n != digraph.out_degree()) {
        throw std::runtime_error("can't do it!");
      }
      size_t old_nodes = digraph.number_of_nodes();
      digraph.add_nodes(n);

      for (size_t i = old_nodes; i < digraph.number_of_nodes(); ++i) {
        for (size_t j = old_nodes; j < digraph.number_of_nodes(); ++j) {
          digraph.add_edge(i, j, j - old_nodes);
        }
      }
    }

    ActionDigraph<size_t> clique(size_t n) {
      ActionDigraph<size_t> g(0, n);
      add_clique(g, n);
      return g;
    }

    ActionDigraph<size_t> binary_tree(size_t number_of_levels) {
      ActionDigraph<size_t> ad;
      ad.add_nodes(std::pow(2, number_of_levels) - 1);
      ad.add_to_out_degree(2);
      ad.add_edge(0, 1, 0);
      ad.add_edge(0, 2, 1);

      for (size_t i = 2; i <= number_of_levels; ++i) {
        size_t counter = std::pow(2, i - 1) - 1;
        for (size_t j = std::pow(2, i - 2) - 1; j < std::pow(2, i - 1) - 1;
             ++j) {
          ad.add_edge(j, counter++, 0);
          ad.add_edge(j, counter++, 1);
        }
      }
      return ad;
    }
  }  // namespace

  LIBSEMIGROUPS_TEST_CASE("ActionDigraph",
                          "000",
                          "constructor with 1  default arg",
                          "[quick][digraph]") {
    ActionDigraph<size_t> g;
    REQUIRE(g.number_of_nodes() == 0);
    REQUIRE(g.number_of_edges() == 0);
  }

  LIBSEMIGROUPS_TEST_CASE("ActionDigraph",
                          "001",
                          "constructor with 0 default args",
                          "[quick][digraph]") {
    for (size_t j = 0; j < 100; ++j) {
      ActionDigraph<size_t> g(j);
      REQUIRE(g.number_of_nodes() == j);
      REQUIRE(g.number_of_edges() == 0);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("ActionDigraph",
                          "002",
                          "add nodes",
                          "[quick][digraph]") {
    ActionDigraph<size_t> g(3);
    REQUIRE(g.number_of_nodes() == 3);
    REQUIRE(g.number_of_edges() == 0);

    for (size_t i = 1; i < 100; ++i) {
      g.add_nodes(i);
      REQUIRE(g.number_of_nodes() == 3 + i * (i + 1) / 2);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("ActionDigraph",
                          "003",
                          "add edges",
                          "[quick][digraph]") {
    ActionDigraph<size_t> g(17, 31);

    for (size_t i = 0; i < 17; ++i) {
      // The digraph isn't fully defined
      REQUIRE_NOTHROW(g.number_of_scc());
      for (size_t j = 0; j < 31; ++j) {
        g.add_edge(i, (7 * i + 23 * j) % 17, j);
      }
    }

    REQUIRE(g.number_of_edges() == 31 * 17);
    REQUIRE(g.number_of_nodes() == 17);
    REQUIRE_THROWS_AS(g.add_edge(0, 0, 32), LibsemigroupsException);

    for (size_t i = 0; i < 17; ++i) {
      for (size_t j = 0; j < 31; ++j) {
        REQUIRE(g.neighbor(i, j) == (7 * i + 23 * j) % 17);
      }
    }

    g.add_to_out_degree(10);
    REQUIRE(g.out_degree() == 41);
    REQUIRE(g.number_of_nodes() == 17);
    REQUIRE(!g.validate());

    for (size_t i = 0; i < 17; ++i) {
      for (size_t j = 0; j < 10; ++j) {
        g.add_edge(i, (7 * i + 23 * j) % 17, 31 + j);
      }
    }

    REQUIRE(g.number_of_edges() == 41 * 17);
    REQUIRE(g.number_of_nodes() == 17);
  }

  LIBSEMIGROUPS_TEST_CASE("ActionDigraph",
                          "004",
                          "strongly connected components - cycles",
                          "[quick][digraph]") {
    ActionDigraph<size_t> g;
    g.add_to_out_degree(1);
    action_digraph_helper::add_cycle(g, 32);
    REQUIRE(g.scc_id(0) == 0);
    g = ActionDigraph<size_t>();
    g.add_to_out_degree(1);
    action_digraph_helper::add_cycle(g, 33);
    REQUIRE(std::vector<std::vector<size_t>>(g.cbegin_sccs(), g.cend_sccs())
            == std::vector<std::vector<size_t>>(
                {{32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22,
                  21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11,
                  10, 9,  8,  7,  6,  5,  4,  3,  2,  1,  0}}));
    for (size_t i = 0; i < 33; ++i) {
      REQUIRE(g.scc_id(i) == 0);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("ActionDigraph",
                          "005",
                          "strongly connected components - no edges",
                          "[quick][digraph][no-valgrind]") {
    ActionDigraph<size_t> graph = ActionDigraph<size_t>(0);
    for (size_t j = 1; j < 100; ++j) {
      graph.add_nodes(j);

      for (size_t i = 0; i < j * (j + 1) / 2; ++i) {
        REQUIRE(graph.scc_id(i) == i);
      }
    }
  }

  LIBSEMIGROUPS_TEST_CASE("ActionDigraph",
                          "006",
                          "strongly connected components - disjoint cycles",
                          "[quick][digraph]") {
    ActionDigraph<size_t> g;
    g.add_to_out_degree(1);
    using difference_type = typename std::iterator_traits<
        ActionDigraph<size_t>::const_iterator_nodes>::difference_type;
    for (size_t j = 2; j < 50; ++j) {
      action_digraph_helper::add_cycle(g, j);
      REQUIRE(std::count_if(
                  g.cbegin_nodes(),
                  g.cend_nodes(),
                  [&g, j](size_t nd) -> bool { return g.scc_id(nd) == j - 2; })
              == difference_type(j));
    }

    REQUIRE(g.number_of_nodes() == 1224);
    REQUIRE(g.number_of_edges() == 1224);
    REQUIRE(g.validate());
  }

  LIBSEMIGROUPS_TEST_CASE("ActionDigraph",
                          "007",
                          "strongly connected components - complete graphs",
                          "[quick][digraph]") {
    for (size_t k = 2; k < 50; ++k) {
      ActionDigraph<size_t> graph(k, k);

      for (size_t i = 0; i < k; ++i) {
        for (size_t j = 0; j < k; ++j) {
          // might as well leave the loops in
          graph.add_edge(i, j, j);
        }
      }
      for (size_t i = 0; i < k; ++i) {
        REQUIRE(graph.scc_id(i) == 0);
      }
    }
  }

  LIBSEMIGROUPS_TEST_CASE("ActionDigraph",
                          "008",
                          "exceptions",
                          "[quick][digraph]") {
    ActionDigraph<size_t> graph(10, 5);
    REQUIRE_THROWS_AS(graph.neighbor(10, 0), LibsemigroupsException);
    REQUIRE(graph.neighbor(0, 1) == UNDEFINED);

    REQUIRE_THROWS_AS(graph.add_edge(0, 10, 0), LibsemigroupsException);
    REQUIRE_THROWS_AS(graph.add_edge(10, 0, 0), LibsemigroupsException);
    for (size_t i = 0; i < 5; ++i) {
      graph.add_edge(0, 1, i);
      graph.add_edge(2, 2, i);
    }
    REQUIRE_NOTHROW(graph.add_edge(0, 1, 0));
    REQUIRE_NOTHROW(graph.add_edge(2, 2, 0));

    REQUIRE_THROWS_AS(graph.scc_id(10), LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("ActionDigraph",
                          "009",
                          "spanning forest - complete graphs",
                          "[quick][digraph]") {
    for (size_t k = 2; k < 50; ++k) {
      ActionDigraph<size_t> graph(k, k);

      for (size_t i = 0; i < k; ++i) {
        for (size_t j = 0; j < k; ++j) {
          // might as well leave the loops in
          graph.add_edge(i, j, j);
        }
      }
      REQUIRE(graph.number_of_scc() == 1);

      Forest const& forest = graph.spanning_forest();
      REQUIRE(forest.parent(k - 1) == UNDEFINED);
      REQUIRE_NOTHROW(graph.reverse_spanning_forest());
    }
  }

  LIBSEMIGROUPS_TEST_CASE("ActionDigraph",
                          "010",
                          "spanning forest - disjoint cycles",
                          "[quick][digraph]") {
    size_t                j = 33;
    ActionDigraph<size_t> graph;
    graph.add_to_out_degree(1);

    for (size_t k = 0; k < 10; ++k) {
      graph.add_nodes(j);
      for (size_t i = k * j; i < (k + 1) * j - 1; ++i) {
        graph.add_edge(i, i + 1, 0);
      }
      graph.add_edge((k + 1) * j - 1, k * j, 0);
    }
    for (size_t i = 0; i < 10 * j; ++i) {
      REQUIRE(graph.scc_id(i) == i / j);
    }
    Forest forest = graph.spanning_forest();

    REQUIRE(std::vector<size_t>(forest.cbegin_parent(), forest.cend_parent())
            == std::vector<size_t>(
                {32,  0,   1,   2,   3,   4,   5,   6,   7,   8,   9,
                 10,  11,  12,  13,  14,  15,  16,  17,  18,  19,  20,
                 21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  UNDEFINED,
                 65,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,
                 43,  44,  45,  46,  47,  48,  49,  50,  51,  52,  53,
                 54,  55,  56,  57,  58,  59,  60,  61,  62,  63,  UNDEFINED,
                 98,  66,  67,  68,  69,  70,  71,  72,  73,  74,  75,
                 76,  77,  78,  79,  80,  81,  82,  83,  84,  85,  86,
                 87,  88,  89,  90,  91,  92,  93,  94,  95,  96,  UNDEFINED,
                 131, 99,  100, 101, 102, 103, 104, 105, 106, 107, 108,
                 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119,
                 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, UNDEFINED,
                 164, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141,
                 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152,
                 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, UNDEFINED,
                 197, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174,
                 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185,
                 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, UNDEFINED,
                 230, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207,
                 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218,
                 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, UNDEFINED,
                 263, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240,
                 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251,
                 252, 253, 254, 255, 256, 257, 258, 259, 260, 261, UNDEFINED,
                 296, 264, 265, 266, 267, 268, 269, 270, 271, 272, 273,
                 274, 275, 276, 277, 278, 279, 280, 281, 282, 283, 284,
                 285, 286, 287, 288, 289, 290, 291, 292, 293, 294, UNDEFINED,
                 329, 297, 298, 299, 300, 301, 302, 303, 304, 305, 306,
                 307, 308, 309, 310, 311, 312, 313, 314, 315, 316, 317,
                 318, 319, 320, 321, 322, 323, 324, 325, 326, 327, UNDEFINED}));
  }

  // TODO(FLS) uncomment this
  //  LIBSEMIGROUPS_TEST_CASE("ActionDigraph",
  //                          "011",
  //                          "scc root paths - complete graphs",
  //                          "[quick][digraph]") {
  //    for (size_t k = 2; k < 50; ++k) {
  //      ActionDigraph<size_t> graph(k);
  //
  //      for (size_t i = 0; i < k; ++i) {
  //        for (size_t j = 0; j < k; ++j) {
  //          graph.add_edge(i, j, j);
  //        }
  //      }
  //
  //      for (size_t i = 0; i < k; ++i) {
  //        size_t pos = i;
  //        for (auto it = graph.cbegin_path_to_root(i); it <
  //        graph.cend_path_to_root(i); ++it) {
  //          pos = graph.neighbor(pos, *it);
  //        }
  //        REQUIRE(pos == graph.cbegin_sccs()[graph.scc_id(i)][0]);
  //      }
  //    }
  //  }

  // LIBSEMIGROUPS_TEST_CASE("ActionDigraph",
  //                         "012",
  //                         "scc root paths - disjoint cycles",
  //                         "[quick][digraph]") {
  //   for (size_t j = 2; j < 35; ++j) {
  //     ActionDigraph<size_t> graph;

  //     for (size_t k = 0; k < 6; ++k) {
  //       graph.add_nodes(j);
  //       for (size_t i = k * j; i < (k + 1) * j - 1; ++i) {
  //         graph.add_edge(i, i + 1, 0);
  //       }
  //       graph.add_edge((k + 1) * j - 1, k * j, 0);
  //     }

  //     for (size_t i = 0; i < graph.number_of_nodes(); ++i) {
  //       size_t pos = i;
  //       for (auto it = graph.cbegin_path_to_root(i);
  //            it < graph.cend_path_to_root(i);
  //            ++it) {
  //         pos = graph.neighbor(pos, *it);
  //       }
  //       REQUIRE(pos == graph.cbegin_sccs()[graph.scc_id(i)][0]);
  //     }
  //   }
  // }

  LIBSEMIGROUPS_TEST_CASE("ActionDigraph",
                          "013",
                          "scc large cycle",
                          "[quick][digraph]") {
    ActionDigraph<size_t> graph;
    graph.add_to_out_degree(1);
    action_digraph_helper::add_cycle(graph, 100000);
    using node_type = decltype(graph)::node_type;

    REQUIRE(std::all_of(
        graph.cbegin_nodes(),
        graph.cend_nodes(),
        [&graph](node_type i) -> bool { return graph.scc_id(i) == 0; }));
    action_digraph_helper::add_cycle(graph, 10101);
    REQUIRE(std::all_of(
        graph.cbegin_nodes(),
        graph.cend_nodes() - 10101,
        [&graph](node_type i) -> bool { return graph.scc_id(i) == 0; }));
    REQUIRE(std::all_of(
        graph.cbegin_nodes() + 100000,
        graph.cend_nodes(),
        [&graph](node_type i) -> bool { return graph.scc_id(i) == 1; }));
  }

  LIBSEMIGROUPS_TEST_CASE("ActionDigraph",
                          "014",
                          "random",
                          "[quick][digraph]") {
    ActionDigraph<size_t> graph = ActionDigraph<size_t>::random(10, 10);
    REQUIRE(graph.number_of_nodes() == 10);
    REQUIRE(graph.number_of_edges() == 100);
  }

  LIBSEMIGROUPS_TEST_CASE("ActionDigraph",
                          "015",
                          "reserve",
                          "[quick][digraph]") {
    ActionDigraph<size_t> graph;
    graph.reserve(10, 10);
    REQUIRE(graph.number_of_nodes() == 0);
    REQUIRE(graph.number_of_edges() == 0);
    graph.add_nodes(1);
    REQUIRE(graph.number_of_nodes() == 1);
    graph.add_nodes(9);
    REQUIRE(graph.number_of_nodes() == 10);
    REQUIRE(graph.number_of_edges() == 0);
  }

  LIBSEMIGROUPS_TEST_CASE("ActionDigraph",
                          "016",
                          "default constructors",
                          "[quick][digraph]") {
    auto g1 = ActionDigraph<size_t>();
    g1.add_to_out_degree(1);
    action_digraph_helper::add_cycle(g1, 10);

    // Copy constructor
    auto g2(g1);
    REQUIRE(g2.number_of_edges() == 10);
    REQUIRE(g2.number_of_nodes() == 10);
    REQUIRE(g2.number_of_scc() == 1);

    // Move constructor
    auto g3(std::move(g2));
    REQUIRE(g3.number_of_edges() == 10);
    REQUIRE(g3.number_of_nodes() == 10);
    REQUIRE(g3.number_of_scc() == 1);

    // Copy assignment
    g2 = g3;
    REQUIRE(g2.number_of_edges() == 10);
    REQUIRE(g2.number_of_nodes() == 10);
    REQUIRE(g2.number_of_scc() == 1);
  }

  LIBSEMIGROUPS_TEST_CASE("ActionDigraph",
                          "017",
                          "scc iterators",
                          "[quick][digraph]") {
    using node_type = decltype(clique(1))::node_type;

    for (size_t n = 10; n < 512; n *= 4) {
      auto g = clique(n);
      REQUIRE(g.number_of_nodes() == n);
      REQUIRE(g.number_of_edges() == n * n);
      REQUIRE(g.number_of_scc() == 1);

      add_clique(g, n);

      REQUIRE(g.number_of_nodes() == 2 * n);
      REQUIRE(g.number_of_edges() == 2 * n * n);
      REQUIRE(g.number_of_scc() == 2);

      auto expected = std::vector<node_type>(n, 0);
      std::iota(expected.begin(), expected.end(), 0);
      auto result = std::vector<node_type>(g.cbegin_scc(0), g.cend_scc(0));
      std::sort(result.begin(), result.end());
      REQUIRE(result == expected);

      std::iota(expected.begin(), expected.end(), n);
      result.assign(g.cbegin_scc(1), g.cend_scc(1));
      std::sort(result.begin(), result.end());
      REQUIRE(result == expected);
      REQUIRE_THROWS_AS(g.cbegin_scc(2), LibsemigroupsException);
      REQUIRE_THROWS_AS(g.cend_scc(2), LibsemigroupsException);

      result.assign(g.cbegin_scc_roots(), g.cend_scc_roots());
      std::transform(result.begin(),
                     result.end(),
                     result.begin(),
                     [&g](node_type i) { return g.scc_id(i); });

      REQUIRE(result == std::vector<node_type>({0, 1}));
    }
    {
      auto g = clique(10);
      for (size_t n = 0; n < 99; ++n) {
        add_clique(g, 10);
      }
      REQUIRE(g.number_of_nodes() == 1000);
      REQUIRE(g.number_of_edges() == 10000);
      REQUIRE(g.number_of_scc() == 100);

      auto result
          = std::vector<node_type>(g.cbegin_scc_roots(), g.cend_scc_roots());
      std::transform(result.begin(),
                     result.end(),
                     result.begin(),
                     [&g](node_type i) { return g.scc_id(i); });

      auto expected = std::vector<node_type>(100, 0);
      std::iota(expected.begin(), expected.end(), 0);

      REQUIRE(result == expected);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("ActionDigraph",
                          "018",
                          "iterator to edges",
                          "[quick][digraph]") {
    for (size_t n = 10; n < 512; n *= 4) {
      auto g = clique(n);
      REQUIRE(g.number_of_nodes() == n);
      REQUIRE(g.number_of_edges() == n * n);
      REQUIRE(g.number_of_scc() == 1);

      using node_type = decltype(g)::node_type;

      auto expected = std::vector<node_type>(n, 0);
      std::iota(expected.begin(), expected.end(), 0);

      for (auto it = g.cbegin_nodes(); it < g.cend_nodes(); ++it) {
        auto result
            = std::vector<node_type>(g.cbegin_edges(*it), g.cend_edges(*it));
        REQUIRE(result == expected);
      }
      REQUIRE_THROWS_AS(g.cbegin_edges(n), LibsemigroupsException);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("ActionDigraph",
                          "019",
                          "root of scc",
                          "[quick][digraph]") {
    auto g = clique(10);
    for (size_t n = 0; n < 99; ++n) {
      add_clique(g, 10);
    }
    REQUIRE(g.number_of_nodes() == 1000);
    REQUIRE(g.number_of_edges() == 10000);
    REQUIRE(g.number_of_scc() == 100);

    using node_type = decltype(g)::node_type;

    for (auto it = g.cbegin_sccs(); it < g.cend_sccs(); ++it) {
      REQUIRE(std::all_of(it->cbegin(), it->cend(), [&g](node_type v) {
        return g.root_of_scc(v) == *g.cbegin_scc(g.scc_id(v));
      }));
    }
    REQUIRE_THROWS_AS(g.root_of_scc(1000), LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("ActionDigraph",
                          "029",
                          "reverse node iterator",
                          "[quick]") {
    using node_type = ActionDigraph<size_t>::node_type;
    ActionDigraph<size_t> ad;
    ad.add_nodes(10);
    REQUIRE(ad.number_of_nodes() == 10);
    REQUIRE(std::vector<node_type>(ad.cbegin_nodes(), ad.cend_nodes())
            == std::vector<node_type>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9}));

    auto it = ad.cbegin_nodes();
    REQUIRE(*it == 0);
    auto copy(it);
    REQUIRE(*copy == 0);
    it       = ad.cend_nodes();
    auto tmp = it;
    REQUIRE(*--tmp == 9);

    REQUIRE(std::vector<node_type>(ad.crbegin_nodes(), ad.crend_nodes())
            == std::vector<node_type>({9, 8, 7, 6, 5, 4, 3, 2, 1, 0}));
  }

  LIBSEMIGROUPS_TEST_CASE("ActionDigraph",
                          "038",
                          "random/random_acyclic exceptions",
                          "[quick]") {
    // Too few nodes
    REQUIRE_THROWS_AS(ActionDigraph<size_t>::random(0, 0, 0),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(ActionDigraph<size_t>::random_acyclic(0, 0, 0),
                      LibsemigroupsException);
    // Out degree too low
    REQUIRE_THROWS_AS(ActionDigraph<size_t>::random(2, 0, 0),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(ActionDigraph<size_t>::random_acyclic(2, 0, 0),
                      LibsemigroupsException);
    // Number of edges too high
    REQUIRE_THROWS_AS(ActionDigraph<size_t>::random(2, 2, 5),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(ActionDigraph<size_t>::random_acyclic(2, 2, 5),
                      LibsemigroupsException);
    // Number of edges = 0
    auto ad = ActionDigraph<size_t>::random(2, 2, 0);
    REQUIRE(ad.number_of_edges() == 0);
    ad = ActionDigraph<size_t>::random_acyclic(2, 2, 0);
    REQUIRE(ad.number_of_edges() == 0);
    ad = ActionDigraph<size_t>::random_acyclic(10, 10, 41);
    REQUIRE(ad.number_of_edges() == 41);
  }

  LIBSEMIGROUPS_TEST_CASE("ActionDigraph",
                          "039",
                          "unsafe (next) neighbour",
                          "[quick]") {
    auto ad = binary_tree(10);
    REQUIRE(ad.unsafe_neighbor(0, 1) == ad.neighbor(0, 1));
    REQUIRE(ad.unsafe_next_neighbor(0, 1) == ad.next_neighbor(0, 1));
  }

  LIBSEMIGROUPS_TEST_CASE("ActionDigraph",
                          "040",
                          "number_of_egdes incident to a node",
                          "[quick]") {
    auto ad = binary_tree(10);
    REQUIRE(ad.number_of_nodes() == 1023);
    REQUIRE(
        std::count_if(ad.cbegin_nodes(),
                      ad.cend_nodes(),
                      [&ad](size_t n) { return ad.number_of_edges(n) == 2; })
        == 511);
  }

  LIBSEMIGROUPS_TEST_CASE("ActionDigraph", "011", "restrict", "[quick]") {
    ActionDigraph<size_t> ad;
    ad.add_nodes(3);
    ad.add_to_out_degree(2);
    ad.add_edge(0, 1, 0);
    ad.add_edge(1, 0, 0);
    ad.add_edge(2, 0, 0);

    ad.restrict(2);
    REQUIRE(ad
            == action_digraph_helper::make<size_t>(2, {{1, UNDEFINED}, {0}}));
  }

  LIBSEMIGROUPS_TEST_CASE("ActionDigraph", "012", "remove_edge_nc", "[quick]") {
    ActionDigraph<size_t> ad;
    ad.add_nodes(3);
    ad.add_to_out_degree(2);
    ad.add_edge(0, 1, 0);
    ad.add_edge(1, 0, 0);
    ad.add_edge(2, 0, 0);

    ad.remove_edge_nc(0, 0);  // remove edge from 0 labelled 0
    REQUIRE(ad
            == action_digraph_helper::make<size_t>(
                3, {{UNDEFINED, UNDEFINED}, {0}, {0}}));
  }

  LIBSEMIGROUPS_TEST_CASE("ActionDigraph", "043", "swap_edge_nc", "[quick]") {
    ActionDigraph<size_t> ad;
    ad.add_nodes(3);
    ad.add_to_out_degree(2);
    ad.add_edge(0, 1, 0);
    ad.add_edge(1, 0, 0);
    ad.add_edge(2, 2, 0);

    // swap edge from 0 labelled 0 with edge from 1 labelled 0
    ad.swap_edges_nc(0, 1, 0);
    REQUIRE(
        ad
        == action_digraph_helper::make<size_t>(3, {{0, UNDEFINED}, {1}, {2}}));
  }

  LIBSEMIGROUPS_TEST_CASE("ActionDigraph", "045", "operator<<", "[quick]") {
    ActionDigraph<size_t> ad;
    ad.add_nodes(3);
    ad.add_to_out_degree(2);
    ad.add_edge(0, 1, 0);
    ad.add_edge(1, 0, 0);
    ad.add_edge(2, 2, 0);

    std::ostringstream oss;
    oss << ad;
    REQUIRE(oss.str() == "{{1, -}, {0, -}, {2, -}}");
  }

}  // namespace libsemigroups
