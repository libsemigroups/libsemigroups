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

#define CATCH_CONFIG_ENABLE_PAIR_STRINGMAKER

#include <cstddef>        // for size_t
#include <stdexcept>      // for runtime_error
#include <unordered_set>  // for unordered_set
#include <vector>         // for vector

#include "catch.hpp"  // for REQUIRE, REQUIRE_THROWS_AS, REQUI...
#include "libsemigroups/digraph-helper.hpp"  // for follow_path
#include "libsemigroups/digraph.hpp"         // for ActionDigraph
#include "libsemigroups/forest.hpp"          // for Forest
#include "libsemigroups/kbe.hpp"             // for KBE
#include "libsemigroups/knuth-bendix.hpp"    // for KnuthBendix
#include "libsemigroups/wilo.hpp"            // for cbegin_wilo
#include "libsemigroups/wislo.hpp"           // for cbegin_wislo
#include "test-main.hpp"                     // for LIBSEMIGROUPS_TEST_CASE

CATCH_REGISTER_ENUM(libsemigroups::ActionDigraph<size_t>::algorithm,
                    libsemigroups::ActionDigraph<size_t>::algorithm::dfs,
                    libsemigroups::ActionDigraph<size_t>::algorithm::matrix,
                    libsemigroups::ActionDigraph<size_t>::algorithm::acyclic,
                    libsemigroups::ActionDigraph<size_t>::algorithm::automatic,
                    libsemigroups::ActionDigraph<size_t>::algorithm::trivial)

namespace libsemigroups {
  using KnuthBendix = fpsemigroup::KnuthBendix;

  struct LibsemigroupsException;  // forward decl

  namespace {
    void add_chain(ActionDigraph<size_t>& digraph, size_t n) {
      size_t old_nodes = digraph.number_of_nodes();
      digraph.add_nodes(n);
      for (size_t i = old_nodes; i < digraph.number_of_nodes() - 1; ++i) {
        digraph.add_edge(i, i + 1, 0);
      }
    }

    ActionDigraph<size_t> chain(size_t n) {
      ActionDigraph<size_t> g(0, 1);
      add_chain(g, n);
      return g;
    }

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
      REQUIRE_THROWS_AS(g.number_of_scc(), LibsemigroupsException);
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
                          "020",
                          "cbegin/end_panislo - 100 node path",
                          "[quick]") {
    ActionDigraph<size_t> ad;
    using node_type = decltype(ad)::node_type;
    size_t const n  = 100;
    ad.add_nodes(n);
    ad.add_to_out_degree(2);
    for (size_t i = 0; i < n - 1; ++i) {
      ad.add_edge(i, i + 1, i % 2);
    }
    std::vector<std::pair<word_type, node_type>> pths(ad.cbegin_panilo(0),
                                                      ad.cend_panilo());
    REQUIRE(pths.size() == 100);
    REQUIRE(std::distance(ad.cbegin_panilo(50), ad.cend_panilo()) == 50);

    REQUIRE(ad.cbegin_panislo(0) != ad.cend_panislo());
    pths.clear();
    pths.insert(pths.end(), ad.cbegin_panislo(0), ad.cend_panislo());
    REQUIRE(pths.size() == 100);
    REQUIRE(pths[3].first == word_type({0, 1, 0}));
    REQUIRE(std::distance(ad.cbegin_panislo(50), ad.cend_panislo()) == 50);
  }

  LIBSEMIGROUPS_TEST_CASE("ActionDigraph",
                          "021",
                          "cbegin/end_pislo",
                          "[quick]") {
    ActionDigraph<size_t> ad;
    ad.add_nodes(9);
    ad.add_to_out_degree(3);
    ad.add_edge(0, 1, 0);
    ad.add_edge(0, 2, 1);
    ad.add_edge(2, 3, 0);
    ad.add_edge(2, 4, 1);
    ad.add_edge(4, 5, 1);

    ad.add_edge(2, 6, 2);
    ad.add_edge(6, 7, 1);
    ad.add_edge(7, 8, 0);

    REQUIRE(std::vector<word_type>(ad.cbegin_pislo(2, 3, 4), ad.cend_pislo())
            == std::vector<word_type>({{2, 1, 0}}));

    std::vector<word_type> expected;
    REQUIRE(std::vector<word_type>(ad.cbegin_pislo(0, 0, 0), ad.cend_pislo())
            == expected);

    expected.emplace_back(word_type());
    REQUIRE(std::vector<word_type>(ad.cbegin_pislo(0, 0, 1), ad.cend_pislo())
            == expected);

    expected.emplace_back(word_type({0}));
    expected.emplace_back(word_type({1}));
    REQUIRE(std::vector<word_type>(ad.cbegin_pislo(0, 0, 2), ad.cend_pislo())
            == expected);

    expected.emplace_back(word_type({1, 0}));
    expected.emplace_back(word_type({1, 1}));
    expected.emplace_back(word_type({1, 2}));
    REQUIRE(std::vector<word_type>(ad.cbegin_pislo(0, 0, 3), ad.cend_pislo())
            == expected);

    expected.emplace_back(word_type({1, 1, 1}));
    expected.emplace_back(word_type({1, 2, 1}));

    REQUIRE(std::vector<word_type>(ad.cbegin_pislo(0, 0, 4), ad.cend_pislo())
            == expected);
    expected.emplace_back(word_type({1, 2, 1, 0}));
    REQUIRE(std::vector<word_type>(ad.cbegin_pislo(0, 0, 10), ad.cend_pislo())
            == expected);

    REQUIRE(std::vector<word_type>(ad.cbegin_pislo(0, 2, 3), ad.cend_pislo())
            == std::vector<word_type>({{1, 0}, {1, 1}, {1, 2}}));
  }

  LIBSEMIGROUPS_TEST_CASE("ActionDigraph",
                          "022",
                          "cbegin/end_pani(s)lo - 100 node cycle",
                          "[quick]") {
    ActionDigraph<size_t> ad;
    ad.add_to_out_degree(1);
    action_digraph_helper::add_cycle(ad, 100);

    REQUIRE(std::distance(ad.cbegin_panilo(0, 0, 200), ad.cend_panilo())
            == 200);
    REQUIRE(std::distance(ad.cbegin_panislo(0, 0, 200), ad.cend_panislo())
            == 200);
  }

  LIBSEMIGROUPS_TEST_CASE("ActionDigraph",
                          "023",
                          "cbegin/cend_pilo - tree 14 nodes",
                          "[quick]") {
    ActionDigraph<size_t> ad;
    ad.add_nodes(15);
    ad.add_to_out_degree(2);

    ad.add_edge(0, 1, 0);
    ad.add_edge(0, 2, 1);

    ad.add_edge(1, 3, 0);
    ad.add_edge(1, 4, 1);

    ad.add_edge(2, 5, 0);
    ad.add_edge(2, 6, 1);

    ad.add_edge(3, 7, 0);
    ad.add_edge(3, 8, 1);

    ad.add_edge(4, 9, 0);
    ad.add_edge(4, 10, 1);

    ad.add_edge(5, 11, 0);
    ad.add_edge(5, 12, 1);

    ad.add_edge(6, 13, 0);
    ad.add_edge(6, 14, 1);

    REQUIRE(std::vector<word_type>(ad.cbegin_pilo(0, 0, 3), ad.cend_pilo())
            == std::vector<word_type>(
                {{}, {0}, {0, 0}, {0, 1}, {1}, {1, 0}, {1, 1}}));

    REQUIRE(std::vector<word_type>(ad.cbegin_pislo(0, 0, 3), ad.cend_pislo())
            == std::vector<word_type>(
                {{}, {0}, {1}, {0, 0}, {0, 1}, {1, 0}, {1, 1}}));

    REQUIRE(std::vector<word_type>(ad.cbegin_pilo(0), ad.cend_pilo())
            == std::vector<word_type>({{},
                                       {0},
                                       {0, 0},
                                       {0, 0, 0},
                                       {0, 0, 1},
                                       {0, 1},
                                       {0, 1, 0},
                                       {0, 1, 1},
                                       {1},
                                       {1, 0},
                                       {1, 0, 0},
                                       {1, 0, 1},
                                       {1, 1},
                                       {1, 1, 0},
                                       {1, 1, 1}}));

    REQUIRE(std::vector<word_type>(ad.cbegin_pislo(0), ad.cend_pislo())
            == std::vector<word_type>({{},
                                       {0},
                                       {1},
                                       {0, 0},
                                       {0, 1},
                                       {1, 0},
                                       {1, 1},
                                       {0, 0, 0},
                                       {0, 0, 1},
                                       {0, 1, 0},
                                       {0, 1, 1},
                                       {1, 0, 0},
                                       {1, 0, 1},
                                       {1, 1, 0},
                                       {1, 1, 1}}));

    REQUIRE(std::vector<word_type>(ad.cbegin_pilo(0, 1), ad.cend_pilo())
            == std::vector<word_type>({{0},
                                       {0, 0},
                                       {0, 0, 0},
                                       {0, 0, 1},
                                       {0, 1},
                                       {0, 1, 0},
                                       {0, 1, 1},
                                       {1},
                                       {1, 0},
                                       {1, 0, 0},
                                       {1, 0, 1},
                                       {1, 1},
                                       {1, 1, 0},
                                       {1, 1, 1}}));

    REQUIRE(std::vector<word_type>(ad.cbegin_pislo(0, 1), ad.cend_pislo())
            == std::vector<word_type>({{0},
                                       {1},
                                       {0, 0},
                                       {0, 1},
                                       {1, 0},
                                       {1, 1},
                                       {0, 0, 0},
                                       {0, 0, 1},
                                       {0, 1, 0},
                                       {0, 1, 1},
                                       {1, 0, 0},
                                       {1, 0, 1},
                                       {1, 1, 0},
                                       {1, 1, 1}}));
    REQUIRE(
        std::vector<word_type>(ad.cbegin_pilo(2, 1), ad.cend_pilo())
        == std::vector<word_type>({{0}, {0, 0}, {0, 1}, {1}, {1, 0}, {1, 1}}));

    REQUIRE(
        std::vector<word_type>(ad.cbegin_pislo(2, 1), ad.cend_pislo())
        == std::vector<word_type>({{0}, {1}, {0, 0}, {0, 1}, {1, 0}, {1, 1}}));

    REQUIRE(std::vector<word_type>(ad.cbegin_pilo(2, 2, 3), ad.cend_pilo())
            == std::vector<word_type>({{0, 0}, {0, 1}, {1, 0}, {1, 1}}));

    REQUIRE(std::vector<word_type>(ad.cbegin_pislo(2, 2, 3), ad.cend_pislo())
            == std::vector<word_type>({{0, 0}, {0, 1}, {1, 0}, {1, 1}}));
  }

  LIBSEMIGROUPS_TEST_CASE("ActionDigraph",
                          "024",
                          "cbegin/end_pstilo - Cayley digraph",
                          "[quick][no-valgrind]") {
    ActionDigraph<size_t> ad;
    ad.add_nodes(6);
    ad.add_to_out_degree(2);

    ad.add_edge(0, 1, 0);
    ad.add_edge(0, 2, 1);
    ad.add_edge(1, 3, 0);
    ad.add_edge(1, 4, 1);
    ad.add_edge(2, 4, 0);
    ad.add_edge(2, 2, 1);
    ad.add_edge(3, 1, 0);
    ad.add_edge(3, 5, 1);
    ad.add_edge(4, 5, 0);
    ad.add_edge(4, 4, 1);
    ad.add_edge(5, 4, 0);
    ad.add_edge(5, 5, 1);

    REQUIRE(ad.validate());
    REQUIRE(!action_digraph_helper::is_acyclic(ad));

    std::vector<word_type> expected = {{0, 1},
                                       {1, 0},
                                       {0, 1, 1},
                                       {1, 1, 0},
                                       {1, 0, 1},
                                       {1, 1, 0, 1},
                                       {1, 0, 1, 1},
                                       {1, 1, 1, 0},
                                       {0, 1, 1, 1},
                                       {1, 0, 0, 0},
                                       {0, 0, 0, 1},
                                       {0, 0, 1, 0},
                                       {0, 1, 0, 0}};

    std::sort(
        expected.begin(), expected.end(), LexicographicalCompare<word_type>());
    REQUIRE(
        std::vector<word_type>(ad.cbegin_pstilo(0, 4, 0, 5), ad.cend_pstilo())
        == expected);

    size_t const N = 18;

    expected.clear();
    for (auto it = cbegin_wilo(2, N, {}, word_type(N, 1));
         it != cend_wilo(2, N, {}, word_type(N, 1));
         ++it) {
      auto node = action_digraph_helper::follow_path(ad, 0, *it);
      if (node == 4) {
        expected.push_back(*it);
      }
    }
    REQUIRE(expected.size() == 131062);

    auto result = std::vector<word_type>(ad.cbegin_pstilo(0, 4, 0, N),
                                         ad.cend_pstilo());
    REQUIRE(result.size() == 131062);
    REQUIRE(result == expected);

    REQUIRE(ad.number_of_paths(0, 4, 0, N) == 131062);
    REQUIRE(ad.number_of_paths(0, 4, 10, N) == 130556);
    REQUIRE(ad.number_of_paths(4, 1, 0, N) == 0);
    REQUIRE(ad.number_of_paths(0, 0, POSITIVE_INFINITY) == POSITIVE_INFINITY);
    REQUIRE(ad.number_of_paths(0, 0, 10) == 1023);
  }

  LIBSEMIGROUPS_TEST_CASE("ActionDigraph",
                          "025",
                          "cbegin_pstilo - Tsalakou",
                          "[quick]") {
    using action_digraph_helper::follow_path;
    auto        rg = ReportGuard(false);
    KnuthBendix kb;
    kb.set_alphabet("ab");
    kb.add_rule("aaaaa", "aa");
    kb.add_rule("bb", "b");
    kb.add_rule("ab", "b");

    REQUIRE(kb.size() == 9);
    auto S = static_cast<KnuthBendix::froidure_pin_type&>(*kb.froidure_pin());

    ActionDigraph<size_t> ad;
    ad.add_to_out_degree(S.number_of_generators());
    ad.add_nodes(S.size() + 1);

    for (size_t j = 0; j < S.number_of_generators(); ++j) {
      ad.add_edge(S.size(), j, j);
    }

    for (size_t i = 0; i < S.size(); ++i) {
      for (size_t j = 0; j < S.number_of_generators(); ++j) {
        ad.add_edge(i, S.right(i, j), j);
      }
    }

    std::vector<word_type> tprime;

    for (size_t i = 0; i < S.size(); ++i) {
      tprime.push_back(*ad.cbegin_pstilo(S.size(), i, 0, 9));
    }
    REQUIRE(tprime.size() == 9);
    REQUIRE(tprime
            == std::vector<word_type>({{0},
                                       {0, 0, 0, 0, 0, 0, 0, 1},
                                       {0, 0},
                                       {0, 0, 0, 0, 0, 0, 1, 0},
                                       {0, 0, 0},
                                       {0, 0, 0, 0, 0, 1, 0, 0},
                                       {0, 0, 0, 0},
                                       {0, 0, 0, 0, 1, 0, 0, 0},
                                       {0, 0, 0, 1, 0, 0, 0, 0}}));

    std::vector<word_type> lprime;
    for (auto const& w : tprime) {
      for (size_t j = 0; j < S.number_of_generators(); ++j) {
        word_type ww(w);
        ww.push_back(j);
        if (std::find(tprime.cbegin(), tprime.cend(), ww) == tprime.cend()) {
          lprime.push_back(ww);
        }
      }
    }

    std::sort(
        lprime.begin(), lprime.end(), LexicographicalCompare<word_type>());

    REQUIRE(lprime.size() == 15);
    REQUIRE(lprime
            == std::vector<word_type>({{0, 0, 0, 0, 0},
                                       {0, 0, 0, 0, 0, 0, 0, 1, 0},
                                       {0, 0, 0, 0, 0, 0, 0, 1, 1},
                                       {0, 0, 0, 0, 0, 0, 1, 0, 0},
                                       {0, 0, 0, 0, 0, 0, 1, 0, 1},
                                       {0, 0, 0, 0, 0, 1, 0, 0, 0},
                                       {0, 0, 0, 0, 0, 1, 0, 0, 1},
                                       {0, 0, 0, 0, 1},
                                       {0, 0, 0, 0, 1, 0, 0, 0, 0},
                                       {0, 0, 0, 0, 1, 0, 0, 0, 1},
                                       {0, 0, 0, 1},
                                       {0, 0, 0, 1, 0, 0, 0, 0, 0},
                                       {0, 0, 0, 1, 0, 0, 0, 0, 1},
                                       {0, 0, 1},
                                       {0, 1}}));
    std::vector<word_type> rhs(lprime.size(), word_type({}));
    for (size_t i = 0; i < lprime.size(); ++i) {
      rhs[i] = tprime[follow_path(ad, S.size(), lprime[i])];
    }

    REQUIRE(rhs
            == std::vector<word_type>({{0, 0},
                                       {0, 0, 0, 0, 0, 0, 1, 0},
                                       {0, 0, 0, 0, 0, 0, 0, 1},
                                       {0, 0, 0, 0, 0, 1, 0, 0},
                                       {0, 0, 0, 0, 0, 0, 0, 1},
                                       {0, 0, 0, 0, 1, 0, 0, 0},
                                       {0, 0, 0, 0, 0, 0, 0, 1},
                                       {0, 0, 0, 0, 0, 0, 0, 1},
                                       {0, 0, 0, 1, 0, 0, 0, 0},
                                       {0, 0, 0, 0, 0, 0, 0, 1},
                                       {0, 0, 0, 0, 0, 0, 0, 1},
                                       {0, 0, 0, 0, 0, 1, 0, 0},
                                       {0, 0, 0, 0, 0, 0, 0, 1},
                                       {0, 0, 0, 0, 0, 0, 0, 1},
                                       {0, 0, 0, 0, 0, 0, 0, 1}}));
    for (size_t i = 0; i < lprime.size(); ++i) {
      REQUIRE(kb.equal_to(lprime[i], rhs[i]));
    }

    KnuthBendix kb2;
    kb2.set_alphabet(2);
    for (size_t i = 0; i < lprime.size(); ++i) {
      kb2.add_rule(lprime[i], rhs[i]);
    }
    kb2.add_rule({1}, {0, 0, 0, 0, 0, 0, 0, 1});
    REQUIRE(kb2.size() == 9);
    kb2.froidure_pin()->run();
    REQUIRE(std::vector<relation_type>(kb2.froidure_pin()->cbegin_rules(),
                                       kb2.froidure_pin()->cend_rules())
            == std::vector<relation_type>(
                {{{0, 1}, {1}}, {{1, 1}, {1}}, {{0, 0, 0, 0, 0}, {0, 0}}}));
  }

  LIBSEMIGROUPS_TEST_CASE("ActionDigraph",
                          "026",
                          "cbegin/end_pstislo - Cayley digraph",
                          "[quick]") {
    ActionDigraph<size_t> ad;
    ad.add_nodes(6);
    ad.add_to_out_degree(2);

    ad.add_edge(0, 1, 0);
    ad.add_edge(0, 2, 1);
    ad.add_edge(1, 3, 0);
    ad.add_edge(1, 4, 1);
    ad.add_edge(2, 4, 0);
    ad.add_edge(2, 2, 1);
    ad.add_edge(3, 1, 0);
    ad.add_edge(3, 5, 1);
    ad.add_edge(4, 5, 0);
    ad.add_edge(4, 4, 1);
    ad.add_edge(5, 4, 0);
    ad.add_edge(5, 5, 1);

    std::vector<word_type> expected = {{0, 1},
                                       {1, 0},
                                       {0, 1, 1},
                                       {1, 1, 0},
                                       {1, 0, 1},
                                       {1, 1, 0, 1},
                                       {1, 0, 1, 1},
                                       {1, 1, 1, 0},
                                       {0, 1, 1, 1},
                                       {1, 0, 0, 0},
                                       {0, 0, 0, 1},
                                       {0, 0, 1, 0},
                                       {0, 1, 0, 0}};

    std::sort(expected.begin(), expected.end(), ShortLexCompare<word_type>());
    REQUIRE(
        std::vector<word_type>(ad.cbegin_pstislo(0, 4, 0, 5), ad.cend_pstislo())
        == expected);

    size_t const N = 18;

    expected.clear();
    for (auto it = cbegin_wislo(2, {}, word_type(N, 0));
         it != cend_wislo(2, {}, word_type(N, 0));
         ++it) {
      auto node = action_digraph_helper::follow_path(ad, 0, *it);
      if (node == 4) {
        expected.push_back(*it);
      }
    }
    REQUIRE(expected.size() == 131062);

    auto result = std::vector<word_type>(ad.cbegin_pstislo(0, 4, 0, N),
                                         ad.cend_pstislo());
    REQUIRE(result.size() == 131062);
    REQUIRE(result == expected);
  }

  LIBSEMIGROUPS_TEST_CASE("ActionDigraph",
                          "027",
                          "cbegin/end_pstislo - Cayley digraph",
                          "[quick]") {
    ActionDigraph<size_t> ad;
    ad.add_nodes(6);
    ad.add_to_out_degree(3);

    ad.add_edge(0, 1, 0);
    ad.add_edge(0, 2, 1);
    ad.add_edge(1, 2, 0);
    ad.add_edge(1, 0, 1);
    ad.add_edge(1, 3, 2);
    ad.add_edge(2, 3, 2);
    ad.add_edge(3, 4, 0);
    ad.add_edge(4, 5, 1);
    ad.add_edge(5, 3, 0);

    REQUIRE(std::is_sorted(ad.cbegin_pislo(0, 0, 10),
                           ad.cend_pislo(),
                           ShortLexCompare<word_type>()));
    REQUIRE(std::distance(ad.cbegin_pislo(0, 0, 10), ad.cend_pislo()) == 75);
    REQUIRE(!action_digraph_helper::is_acyclic(ad));
    REQUIRE(ad.number_of_paths(0, 0, 10) == 75);
    REQUIRE(ad.number_of_paths(0, 0, POSITIVE_INFINITY) == POSITIVE_INFINITY);
    REQUIRE(std::vector<word_type>(ad.cbegin_pislo(0, 0, 10), ad.cend_pislo())
            == std::vector<word_type>({{},
                                       {0},
                                       {1},
                                       {0, 0},
                                       {0, 1},
                                       {0, 2},
                                       {1, 2},
                                       {0, 0, 2},
                                       {0, 1, 0},
                                       {0, 1, 1},
                                       {0, 2, 0},
                                       {1, 2, 0},
                                       {0, 0, 2, 0},
                                       {0, 1, 0, 0},
                                       {0, 1, 0, 1},
                                       {0, 1, 0, 2},
                                       {0, 1, 1, 2},
                                       {0, 2, 0, 1},
                                       {1, 2, 0, 1},
                                       {0, 0, 2, 0, 1},
                                       {0, 1, 0, 0, 2},
                                       {0, 1, 0, 1, 0},
                                       {0, 1, 0, 1, 1},
                                       {0, 1, 0, 2, 0},
                                       {0, 1, 1, 2, 0},
                                       {0, 2, 0, 1, 0},
                                       {1, 2, 0, 1, 0},
                                       {0, 0, 2, 0, 1, 0},
                                       {0, 1, 0, 0, 2, 0},
                                       {0, 1, 0, 1, 0, 0},
                                       {0, 1, 0, 1, 0, 1},
                                       {0, 1, 0, 1, 0, 2},
                                       {0, 1, 0, 1, 1, 2},
                                       {0, 1, 0, 2, 0, 1},
                                       {0, 1, 1, 2, 0, 1},
                                       {0, 2, 0, 1, 0, 0},
                                       {1, 2, 0, 1, 0, 0},
                                       {0, 0, 2, 0, 1, 0, 0},
                                       {0, 1, 0, 0, 2, 0, 1},
                                       {0, 1, 0, 1, 0, 0, 2},
                                       {0, 1, 0, 1, 0, 1, 0},
                                       {0, 1, 0, 1, 0, 1, 1},
                                       {0, 1, 0, 1, 0, 2, 0},
                                       {0, 1, 0, 1, 1, 2, 0},
                                       {0, 1, 0, 2, 0, 1, 0},
                                       {0, 1, 1, 2, 0, 1, 0},
                                       {0, 2, 0, 1, 0, 0, 1},
                                       {1, 2, 0, 1, 0, 0, 1},
                                       {0, 0, 2, 0, 1, 0, 0, 1},
                                       {0, 1, 0, 0, 2, 0, 1, 0},
                                       {0, 1, 0, 1, 0, 0, 2, 0},
                                       {0, 1, 0, 1, 0, 1, 0, 0},
                                       {0, 1, 0, 1, 0, 1, 0, 1},
                                       {0, 1, 0, 1, 0, 1, 0, 2},
                                       {0, 1, 0, 1, 0, 1, 1, 2},
                                       {0, 1, 0, 1, 0, 2, 0, 1},
                                       {0, 1, 0, 1, 1, 2, 0, 1},
                                       {0, 1, 0, 2, 0, 1, 0, 0},
                                       {0, 1, 1, 2, 0, 1, 0, 0},
                                       {0, 2, 0, 1, 0, 0, 1, 0},
                                       {1, 2, 0, 1, 0, 0, 1, 0},
                                       {0, 0, 2, 0, 1, 0, 0, 1, 0},
                                       {0, 1, 0, 0, 2, 0, 1, 0, 0},
                                       {0, 1, 0, 1, 0, 0, 2, 0, 1},
                                       {0, 1, 0, 1, 0, 1, 0, 0, 2},
                                       {0, 1, 0, 1, 0, 1, 0, 1, 0},
                                       {0, 1, 0, 1, 0, 1, 0, 1, 1},
                                       {0, 1, 0, 1, 0, 1, 0, 2, 0},
                                       {0, 1, 0, 1, 0, 1, 1, 2, 0},
                                       {0, 1, 0, 1, 0, 2, 0, 1, 0},
                                       {0, 1, 0, 1, 1, 2, 0, 1, 0},
                                       {0, 1, 0, 2, 0, 1, 0, 0, 1},
                                       {0, 1, 1, 2, 0, 1, 0, 0, 1},
                                       {0, 2, 0, 1, 0, 0, 1, 0, 0},
                                       {1, 2, 0, 1, 0, 0, 1, 0, 0}}));

    auto expected
        = std::vector<word_type>(ad.cbegin_pislo(0, 0, 10), ad.cend_pislo());
    std::sort(
        expected.begin(), expected.end(), LexicographicalCompare<word_type>());
    REQUIRE(
        expected
        == std::vector<word_type>(ad.cbegin_pilo(0, 0, 10), ad.cend_pilo()));
  }

  LIBSEMIGROUPS_TEST_CASE("ActionDigraph",
                          "028",
                          "path iterators corner cases",
                          "[quick]") {
    ActionDigraph<size_t> ad;
    ad.add_nodes(6);
    ad.add_to_out_degree(3);

    ad.add_edge(0, 1, 0);
    ad.add_edge(0, 2, 1);
    ad.add_edge(1, 2, 0);
    ad.add_edge(1, 0, 1);
    ad.add_edge(1, 3, 2);
    ad.add_edge(2, 3, 2);
    ad.add_edge(3, 4, 0);
    ad.add_edge(4, 5, 1);
    ad.add_edge(5, 3, 0);
    REQUIRE_THROWS_AS(ad.cbegin_pstilo(1, 6), LibsemigroupsException);
    REQUIRE_THROWS_AS(ad.cbegin_pstilo(6, 1), LibsemigroupsException);
    REQUIRE(ad.cbegin_pstilo(2, 1) == ad.cend_pstilo());
    REQUIRE(ad.cbegin_pstilo(0, 3, 10, 1) == ad.cend_pstilo());

    REQUIRE_THROWS_AS(ad.cbegin_pstislo(1, 6), LibsemigroupsException);
    REQUIRE_THROWS_AS(ad.cbegin_pstislo(6, 1), LibsemigroupsException);
    REQUIRE(ad.cbegin_pstislo(2, 1) == ad.cend_pstislo());
    REQUIRE(ad.cbegin_pstislo(0, 3, 10, 1) == ad.cend_pstislo());

    REQUIRE_THROWS_AS(ad.cbegin_panilo(6), LibsemigroupsException);
    REQUIRE(ad.cbegin_panilo(0, 1, 1) == ad.cend_panilo());
    REQUIRE_THROWS_AS(ad.cbegin_panislo(6), LibsemigroupsException);
    REQUIRE(ad.cbegin_panislo(0, 1, 1) == ad.cend_panislo());

    REQUIRE_THROWS_AS(ad.cbegin_pilo(6), LibsemigroupsException);
    REQUIRE(ad.cbegin_pilo(0, 1, 1) == ad.cend_pilo());

    REQUIRE_THROWS_AS(ad.cbegin_pislo(6), LibsemigroupsException);
    REQUIRE(ad.cbegin_pislo(0, 1, 1) == ad.cend_pislo());

    verify_forward_iterator_requirements(ad.cbegin_panilo(0));
    verify_forward_iterator_requirements(ad.cbegin_panislo(0));
    verify_forward_iterator_requirements(ad.cbegin_pilo(0));
    verify_forward_iterator_requirements(ad.cbegin_pislo(0));
    verify_forward_iterator_requirements(ad.cbegin_pstilo(0, 1));
    verify_forward_iterator_requirements(ad.cbegin_pstislo(0, 1));
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
                          "030",
                          "pstilo corner case",
                          "[quick]") {
    ActionDigraph<size_t> ad;
    ad.add_nodes(5);
    ad.add_to_out_degree(2);
    ad.add_edge(0, 1, 1);
    ad.add_edge(0, 2, 0);
    ad.add_edge(2, 3, 0);
    ad.add_edge(3, 4, 0);
    ad.add_edge(4, 2, 0);
    // Tests the case then there is only a single path, but if we would have
    // used panilo (i.e. not use the reachability check that is in pstilo),
    // then we'd enter an infinite loop.

    auto it = ad.cbegin_pstilo(0, 1);
    REQUIRE(*it == word_type({1}));
    ++it;
    REQUIRE(it == ad.cend_pstilo());

    ad = chain(5);
    REQUIRE(std::distance(ad.cbegin_pstilo(0, 0, 0, 100), ad.cend_pstilo())
            == 1);
    REQUIRE(std::distance(ad.cbegin_pstilo(0, 0, 4, 100), ad.cend_pstilo())
            == 0);

    ad = ActionDigraph<size_t>();
    ad.add_to_out_degree(1);
    action_digraph_helper::add_cycle(ad, 5);
    REQUIRE(std::distance(ad.cbegin_pstilo(0, 0, 0, 6), ad.cend_pstilo()) == 2);
    REQUIRE(std::distance(ad.cbegin_pstilo(0, 0, 0, 100), ad.cend_pstilo())
            == 20);
    REQUIRE(std::distance(ad.cbegin_pstilo(0, 0, 4, 100), ad.cend_pstilo())
            == 19);

    // There's 1 path from 0 to 0 of length in range [0, 1), the path of length
    // 0.
    REQUIRE(std::distance(ad.cbegin_pstilo(0, 0, 0, 2), ad.cend_pstilo()) == 1);
  }

  LIBSEMIGROUPS_TEST_CASE("ActionDigraph",
                          "031",
                          "number_of_paths corner cases",
                          "[quick]") {
    using algorithm = ActionDigraph<size_t>::algorithm;
    ActionDigraph<size_t> ad;
    REQUIRE_THROWS_AS(ad.number_of_paths(0, 0, POSITIVE_INFINITY),
                      LibsemigroupsException);
    size_t const n = 20;
    ad.add_to_out_degree(1);
    action_digraph_helper::add_cycle(ad, n);
    REQUIRE(ad.number_of_paths(10) == POSITIVE_INFINITY);
    REQUIRE(ad.number_of_paths_algorithm(10, 10, 0, POSITIVE_INFINITY)
            == algorithm::trivial);
    REQUIRE(ad.number_of_paths(10, 10, 0, POSITIVE_INFINITY)
            == POSITIVE_INFINITY);
    ad = chain(n);
    REQUIRE(ad.number_of_paths(10) == 10);
    REQUIRE(ad.number_of_paths(19) == 1);
  }

  LIBSEMIGROUPS_TEST_CASE("ActionDigraph",
                          "032",
                          "number_of_paths acyclic digraph",
                          "[quick]") {
    using node_type = ActionDigraph<size_t>::node_type;
    ActionDigraph<size_t> ad;
    ad.add_nodes(8);
    ad.add_to_out_degree(3);
    ad.add_edge(0, 3, 0);
    ad.add_edge(0, 2, 1);
    ad.add_edge(0, 3, 2);
    ad.add_edge(1, 7, 0);
    ad.add_edge(2, 1, 0);
    ad.add_edge(3, 1, 0);
    ad.add_edge(3, 5, 1);
    ad.add_edge(4, 6, 0);
    ad.add_edge(6, 3, 0);
    ad.add_edge(6, 7, 1);

    REQUIRE(action_digraph_helper::is_acyclic(ad));

    size_t expected[8][8][8] = {{{0, 1, 4, 9, 12, 12, 12, 12},
                                 {0, 0, 3, 8, 11, 11, 11, 11},
                                 {0, 0, 0, 5, 8, 8, 8, 8},
                                 {0, 0, 0, 0, 3, 3, 3, 3},
                                 {0, 0, 0, 0, 0, 0, 0, 0},
                                 {0, 0, 0, 0, 0, 0, 0, 0},
                                 {0, 0, 0, 0, 0, 0, 0, 0},
                                 {0, 0, 0, 0, 0, 0, 0, 0}},
                                {{0, 1, 2, 2, 2, 2, 2, 2},
                                 {0, 0, 1, 1, 1, 1, 1, 1},
                                 {0, 0, 0, 0, 0, 0, 0, 0},
                                 {0, 0, 0, 0, 0, 0, 0, 0},
                                 {0, 0, 0, 0, 0, 0, 0, 0},
                                 {0, 0, 0, 0, 0, 0, 0, 0},
                                 {0, 0, 0, 0, 0, 0, 0, 0},
                                 {0, 0, 0, 0, 0, 0, 0, 0}},
                                {{0, 1, 2, 3, 3, 3, 3, 3},
                                 {0, 0, 1, 2, 2, 2, 2, 2},
                                 {0, 0, 0, 1, 1, 1, 1, 1},
                                 {0, 0, 0, 0, 0, 0, 0, 0},
                                 {0, 0, 0, 0, 0, 0, 0, 0},
                                 {0, 0, 0, 0, 0, 0, 0, 0},
                                 {0, 0, 0, 0, 0, 0, 0, 0},
                                 {0, 0, 0, 0, 0, 0, 0, 0}},
                                {{0, 1, 3, 4, 4, 4, 4, 4},
                                 {0, 0, 2, 3, 3, 3, 3, 3},
                                 {0, 0, 0, 1, 1, 1, 1, 1},
                                 {0, 0, 0, 0, 0, 0, 0, 0},
                                 {0, 0, 0, 0, 0, 0, 0, 0},
                                 {0, 0, 0, 0, 0, 0, 0, 0},
                                 {0, 0, 0, 0, 0, 0, 0, 0},
                                 {0, 0, 0, 0, 0, 0, 0, 0}},
                                {{0, 1, 2, 4, 6, 7, 7, 7},
                                 {0, 0, 1, 3, 5, 6, 6, 6},
                                 {0, 0, 0, 2, 4, 5, 5, 5},
                                 {0, 0, 0, 0, 2, 3, 3, 3},
                                 {0, 0, 0, 0, 0, 1, 1, 1},
                                 {0, 0, 0, 0, 0, 0, 0, 0},
                                 {0, 0, 0, 0, 0, 0, 0, 0},
                                 {0, 0, 0, 0, 0, 0, 0, 0}},
                                {{0, 1, 1, 1, 1, 1, 1, 1},
                                 {0, 0, 0, 0, 0, 0, 0, 0},
                                 {0, 0, 0, 0, 0, 0, 0, 0},
                                 {0, 0, 0, 0, 0, 0, 0, 0},
                                 {0, 0, 0, 0, 0, 0, 0, 0},
                                 {0, 0, 0, 0, 0, 0, 0, 0},
                                 {0, 0, 0, 0, 0, 0, 0, 0},
                                 {0, 0, 0, 0, 0, 0, 0, 0}},
                                {{0, 1, 3, 5, 6, 6, 6, 6},
                                 {0, 0, 2, 4, 5, 5, 5, 5},
                                 {0, 0, 0, 2, 3, 3, 3, 3},
                                 {0, 0, 0, 0, 1, 1, 1, 1},
                                 {0, 0, 0, 0, 0, 0, 0, 0},
                                 {0, 0, 0, 0, 0, 0, 0, 0},
                                 {0, 0, 0, 0, 0, 0, 0, 0},
                                 {0, 0, 0, 0, 0, 0, 0, 0}},
                                {{0, 1, 1, 1, 1, 1, 1, 1},
                                 {0, 0, 0, 0, 0, 0, 0, 0},
                                 {0, 0, 0, 0, 0, 0, 0, 0},
                                 {0, 0, 0, 0, 0, 0, 0, 0},
                                 {0, 0, 0, 0, 0, 0, 0, 0},
                                 {0, 0, 0, 0, 0, 0, 0, 0},
                                 {0, 0, 0, 0, 0, 0, 0, 0},
                                 {0, 0, 0, 0, 0, 0, 0, 0}}};
    for (auto s = ad.cbegin_nodes(); s != ad.cend_nodes(); ++s) {
      for (size_t min = 0; min < ad.number_of_nodes(); ++min) {
        for (size_t max = 0; max < ad.number_of_nodes(); ++max) {
          REQUIRE(size_t(std::distance(ad.cbegin_pilo(*s, min, max),
                                       ad.cend_pilo()))
                  == expected[*s][min][max]);
        }
      }
    }

    for (auto s = ad.cbegin_nodes(); s != ad.cend_nodes(); ++s) {
      for (size_t min = 0; min < ad.number_of_nodes(); ++min) {
        for (size_t max = 0; max < ad.number_of_nodes(); ++max) {
          REQUIRE(ad.number_of_paths(*s, min, max) == expected[*s][min][max]);
        }
      }
    }

    size_t const N = ad.number_of_nodes();
    REQUIRE(
        std::vector<word_type>(ad.cbegin_pstilo(0, 3, 0, 2), ad.cend_pstilo())
        == std::vector<word_type>({{0}, {2}}));
    using algorithm = ActionDigraph<size_t>::algorithm;
    REQUIRE(ad.number_of_paths(0, 3, 0, 2, algorithm::acyclic)
            == size_t(
                std::distance(ad.cbegin_pstilo(0, 3, 0, 2), ad.cend_pstilo())));

    for (auto s = ad.cbegin_nodes(); s != ad.cend_nodes(); ++s) {
      for (auto t = ad.cbegin_nodes(); t != ad.cend_nodes(); ++t) {
        for (node_type min = 0; min < N; ++min) {
          for (size_t max = min; max < N; ++max) {
            REQUIRE(ad.number_of_paths(*s, *t, min, max)
                    == size_t(std::distance(ad.cbegin_pstilo(*s, *t, min, max),
                                            ad.cend_pstilo())));
          }
        }
      }
    }
  }

  LIBSEMIGROUPS_TEST_CASE("ActionDigraph",
                          "033",
                          "number_of_paths binary tree",
                          "[quick][no-valgrind]") {
    using algorithm          = ActionDigraph<size_t>::algorithm;
    using node_type          = ActionDigraph<size_t>::node_type;
    size_t const          n  = 6;
    ActionDigraph<size_t> ad = binary_tree(n);
    REQUIRE(ad.number_of_nodes() == std::pow(2, n) - 1);
    REQUIRE(ad.number_of_edges() == std::pow(2, n) - 2);
    REQUIRE(action_digraph_helper::is_acyclic(ad));
    REQUIRE(ad.number_of_paths(0) == std::pow(2, n) - 1);

    for (auto s = ad.cbegin_nodes(); s != ad.cend_nodes(); ++s) {
      for (node_type min = 0; min < n; ++min) {
        for (size_t max = min; max < n; ++max) {
          REQUIRE(ad.number_of_paths(*s, min, max)
                  == size_t(std::distance(ad.cbegin_pilo(*s, min, max),
                                          ad.cend_pilo())));
        }
      }
    }
    REQUIRE(ad.number_of_paths_algorithm(0, 1, 0, 1) == algorithm::acyclic);

    REQUIRE(ad.number_of_paths(0, 1, 0, 1)
            == size_t(
                std::distance(ad.cbegin_pstilo(0, 1, 0, 1), ad.cend_pstilo())));

    for (auto s = ad.cbegin_nodes(); s != ad.cend_nodes(); ++s) {
      for (auto t = ad.cbegin_nodes(); t != ad.cend_nodes(); ++t) {
        for (node_type min = 0; min < n; ++min) {
          for (size_t max = min; max < n; ++max) {
            REQUIRE(ad.number_of_paths(*s, *t, min, max)
                    == size_t(std::distance(ad.cbegin_pstilo(*s, *t, min, max),
                                            ad.cend_pstilo())));
          }
        }
      }
    }
  }

  LIBSEMIGROUPS_TEST_CASE("ActionDigraph",
                          "034",
                          "number_of_paths large binary tree",
                          "[quick][no-valgrind]") {
    using algorithm          = ActionDigraph<size_t>::algorithm;
    size_t const          n  = 20;
    ActionDigraph<size_t> ad = binary_tree(n);
    REQUIRE(ad.number_of_nodes() == std::pow(2, n) - 1);
    REQUIRE(ad.number_of_edges() == std::pow(2, n) - 2);
    REQUIRE(action_digraph_helper::is_acyclic(ad));
    REQUIRE(ad.number_of_paths_algorithm(0) == algorithm::acyclic);
    REQUIRE(ad.number_of_paths(0) == std::pow(2, n) - 1);
  }

  LIBSEMIGROUPS_TEST_CASE("ActionDigraph",
                          "035",
                          "number_of_paths 400 node random digraph",
                          "[quick]") {
    size_t const n  = 400;
    auto         ad = ActionDigraph<size_t>::random(n, 20, n, std::mt19937());
    action_digraph_helper::add_cycle(ad, ad.cbegin_nodes(), ad.cend_nodes());
    REQUIRE(!action_digraph_helper::is_acyclic(ad));
    REQUIRE(!ad.validate());
    REQUIRE(ad.number_of_paths_algorithm(0, 0, 16)
            == ActionDigraph<size_t>::algorithm::dfs);
    REQUIRE(ad.number_of_paths(0, 0, 16) != 0);
  }

  LIBSEMIGROUPS_TEST_CASE("ActionDigraph",
                          "036",
                          "number_of_paths 10 node acyclic digraph",
                          "[quick]") {
    using algorithm = ActionDigraph<size_t>::algorithm;
    // size_t const n  = 10;
    // auto ad = ActionDigraph<size_t>::random_acyclic(n, 20, n,
    // std::mt19937()); std::cout <<
    // action_digraph_helper::detail::to_string(ad);
    ActionDigraph<size_t> ad;
    ad.add_nodes(10);
    ad.add_to_out_degree(20);
    ad.add_edge(0, 7, 5);
    ad.add_edge(0, 5, 7);
    ad.add_edge(1, 9, 14);
    ad.add_edge(1, 5, 17);
    ad.add_edge(3, 8, 5);
    ad.add_edge(5, 8, 1);
    ad.add_edge(6, 8, 14);
    ad.add_edge(7, 8, 10);
    ad.add_edge(8, 9, 12);
    ad.add_edge(8, 9, 13);
    REQUIRE(action_digraph_helper::is_acyclic(ad));
    REQUIRE(!ad.validate());

    REQUIRE(ad.number_of_paths_algorithm(0, 0, 16) == algorithm::acyclic);
    REQUIRE(ad.number_of_paths(0, 0, 30) == 9);
    REQUIRE(ad.number_of_paths(1, 0, 10, algorithm::acyclic) == 6);
    REQUIRE(ad.number_of_paths(1, 0, 10, algorithm::matrix) == 6);
    REQUIRE(ad.number_of_paths(1, 9, 0, 10, algorithm::matrix) == 3);
  }

  LIBSEMIGROUPS_TEST_CASE("ActionDigraph",
                          "037",
                          "number_of_paths node digraph",
                          "[quick]") {
    using algorithm = ActionDigraph<size_t>::algorithm;
    size_t const n  = 10;
    // auto         ad = ActionDigraph<size_t>::random(n, 20, 200,
    // std::mt19937()); std::cout <<
    // action_digraph_helper::detail::to_string(ad);
    ActionDigraph<size_t> ad;
    ad.add_nodes(10);
    ad.add_to_out_degree(20);
    ad.add_edge(0, 9, 0);
    ad.add_edge(0, 1, 1);
    ad.add_edge(0, 6, 2);
    ad.add_edge(0, 3, 3);
    ad.add_edge(0, 7, 4);
    ad.add_edge(0, 2, 5);
    ad.add_edge(0, 2, 6);
    ad.add_edge(0, 8, 7);
    ad.add_edge(0, 1, 8);
    ad.add_edge(0, 4, 9);
    ad.add_edge(0, 3, 10);
    ad.add_edge(0, 1, 11);
    ad.add_edge(0, 7, 12);
    ad.add_edge(0, 9, 13);
    ad.add_edge(0, 4, 14);
    ad.add_edge(0, 7, 15);
    ad.add_edge(0, 8, 16);
    ad.add_edge(0, 9, 17);
    ad.add_edge(0, 6, 18);
    ad.add_edge(0, 9, 19);
    ad.add_edge(1, 8, 0);
    ad.add_edge(1, 2, 1);
    ad.add_edge(1, 5, 2);
    ad.add_edge(1, 7, 3);
    ad.add_edge(1, 9, 4);
    ad.add_edge(1, 0, 5);
    ad.add_edge(1, 2, 6);
    ad.add_edge(1, 4, 7);
    ad.add_edge(1, 0, 8);
    ad.add_edge(1, 3, 9);
    ad.add_edge(1, 2, 10);
    ad.add_edge(1, 7, 11);
    ad.add_edge(1, 2, 12);
    ad.add_edge(1, 7, 13);
    ad.add_edge(1, 6, 14);
    ad.add_edge(1, 6, 15);
    ad.add_edge(1, 5, 16);
    ad.add_edge(1, 4, 17);
    ad.add_edge(1, 6, 18);
    ad.add_edge(1, 3, 19);
    ad.add_edge(2, 2, 0);
    ad.add_edge(2, 9, 1);
    ad.add_edge(2, 0, 2);
    ad.add_edge(2, 6, 3);
    ad.add_edge(2, 7, 4);
    ad.add_edge(2, 9, 5);
    ad.add_edge(2, 5, 6);
    ad.add_edge(2, 4, 7);
    ad.add_edge(2, 9, 8);
    ad.add_edge(2, 7, 9);
    ad.add_edge(2, 9, 10);
    ad.add_edge(2, 9, 11);
    ad.add_edge(2, 0, 12);
    ad.add_edge(2, 7, 13);
    ad.add_edge(2, 9, 14);
    ad.add_edge(2, 6, 15);
    ad.add_edge(2, 3, 16);
    ad.add_edge(2, 3, 17);
    ad.add_edge(2, 4, 18);
    ad.add_edge(2, 1, 19);
    ad.add_edge(3, 1, 0);
    ad.add_edge(3, 9, 1);
    ad.add_edge(3, 6, 2);
    ad.add_edge(3, 2, 3);
    ad.add_edge(3, 9, 4);
    ad.add_edge(3, 8, 5);
    ad.add_edge(3, 1, 6);
    ad.add_edge(3, 6, 7);
    ad.add_edge(3, 1, 8);
    ad.add_edge(3, 0, 9);
    ad.add_edge(3, 5, 10);
    ad.add_edge(3, 0, 11);
    ad.add_edge(3, 2, 12);
    ad.add_edge(3, 7, 13);
    ad.add_edge(3, 4, 14);
    ad.add_edge(3, 0, 15);
    ad.add_edge(3, 4, 16);
    ad.add_edge(3, 8, 17);
    ad.add_edge(3, 3, 18);
    ad.add_edge(3, 1, 19);
    ad.add_edge(4, 0, 0);
    ad.add_edge(4, 4, 1);
    ad.add_edge(4, 8, 2);
    ad.add_edge(4, 5, 3);
    ad.add_edge(4, 5, 4);
    ad.add_edge(4, 1, 5);
    ad.add_edge(4, 3, 6);
    ad.add_edge(4, 8, 7);
    ad.add_edge(4, 4, 8);
    ad.add_edge(4, 4, 9);
    ad.add_edge(4, 4, 10);
    ad.add_edge(4, 7, 11);
    ad.add_edge(4, 8, 12);
    ad.add_edge(4, 6, 13);
    ad.add_edge(4, 3, 14);
    ad.add_edge(4, 7, 15);
    ad.add_edge(4, 6, 16);
    ad.add_edge(4, 7, 17);
    ad.add_edge(4, 0, 18);
    ad.add_edge(4, 2, 19);
    ad.add_edge(5, 3, 0);
    ad.add_edge(5, 0, 1);
    ad.add_edge(5, 4, 2);
    ad.add_edge(5, 7, 3);
    ad.add_edge(5, 2, 4);
    ad.add_edge(5, 5, 5);
    ad.add_edge(5, 7, 6);
    ad.add_edge(5, 7, 7);
    ad.add_edge(5, 7, 8);
    ad.add_edge(5, 7, 9);
    ad.add_edge(5, 0, 10);
    ad.add_edge(5, 8, 11);
    ad.add_edge(5, 6, 12);
    ad.add_edge(5, 8, 13);
    ad.add_edge(5, 8, 14);
    ad.add_edge(5, 1, 15);
    ad.add_edge(5, 5, 16);
    ad.add_edge(5, 5, 17);
    ad.add_edge(5, 3, 18);
    ad.add_edge(5, 7, 19);
    ad.add_edge(6, 8, 0);
    ad.add_edge(6, 7, 1);
    ad.add_edge(6, 6, 2);
    ad.add_edge(6, 5, 3);
    ad.add_edge(6, 6, 4);
    ad.add_edge(6, 1, 5);
    ad.add_edge(6, 7, 6);
    ad.add_edge(6, 2, 7);
    ad.add_edge(6, 7, 8);
    ad.add_edge(6, 3, 9);
    ad.add_edge(6, 3, 10);
    ad.add_edge(6, 8, 11);
    ad.add_edge(6, 3, 12);
    ad.add_edge(6, 9, 13);
    ad.add_edge(6, 4, 14);
    ad.add_edge(6, 1, 15);
    ad.add_edge(6, 4, 16);
    ad.add_edge(6, 3, 17);
    ad.add_edge(6, 9, 18);
    ad.add_edge(6, 8, 19);
    ad.add_edge(7, 9, 0);
    ad.add_edge(7, 4, 1);
    ad.add_edge(7, 3, 2);
    ad.add_edge(7, 8, 3);
    ad.add_edge(7, 0, 4);
    ad.add_edge(7, 5, 5);
    ad.add_edge(7, 6, 6);
    ad.add_edge(7, 8, 7);
    ad.add_edge(7, 9, 8);
    ad.add_edge(7, 1, 9);
    ad.add_edge(7, 7, 10);
    ad.add_edge(7, 0, 11);
    ad.add_edge(7, 6, 12);
    ad.add_edge(7, 2, 13);
    ad.add_edge(7, 3, 14);
    ad.add_edge(7, 8, 15);
    ad.add_edge(7, 6, 16);
    ad.add_edge(7, 3, 17);
    ad.add_edge(7, 2, 18);
    ad.add_edge(7, 7, 19);
    ad.add_edge(8, 0, 0);
    ad.add_edge(8, 6, 1);
    ad.add_edge(8, 3, 2);
    ad.add_edge(8, 5, 3);
    ad.add_edge(8, 7, 4);
    ad.add_edge(8, 9, 5);
    ad.add_edge(8, 9, 6);
    ad.add_edge(8, 8, 7);
    ad.add_edge(8, 1, 8);
    ad.add_edge(8, 5, 9);
    ad.add_edge(8, 7, 10);
    ad.add_edge(8, 9, 11);
    ad.add_edge(8, 6, 12);
    ad.add_edge(8, 0, 13);
    ad.add_edge(8, 0, 14);
    ad.add_edge(8, 3, 15);
    ad.add_edge(8, 6, 16);
    ad.add_edge(8, 0, 17);
    ad.add_edge(8, 8, 18);
    ad.add_edge(8, 9, 19);
    ad.add_edge(9, 3, 0);
    ad.add_edge(9, 7, 1);
    ad.add_edge(9, 9, 2);
    ad.add_edge(9, 1, 3);
    ad.add_edge(9, 4, 4);
    ad.add_edge(9, 9, 5);
    ad.add_edge(9, 4, 6);
    ad.add_edge(9, 0, 7);
    ad.add_edge(9, 5, 8);
    ad.add_edge(9, 8, 9);
    ad.add_edge(9, 3, 10);
    ad.add_edge(9, 2, 11);
    ad.add_edge(9, 0, 12);
    ad.add_edge(9, 2, 13);
    ad.add_edge(9, 3, 14);
    ad.add_edge(9, 4, 15);
    ad.add_edge(9, 0, 16);
    ad.add_edge(9, 5, 17);
    ad.add_edge(9, 3, 18);
    ad.add_edge(9, 5, 19);
    REQUIRE(!action_digraph_helper::is_acyclic(ad));
    REQUIRE(ad.validate());

    REQUIRE(ad.number_of_paths_algorithm(0) == algorithm::acyclic);
    REQUIRE(ad.number_of_paths(0) == POSITIVE_INFINITY);
    REQUIRE_THROWS_AS(ad.number_of_paths(0, 0, 10, algorithm::acyclic),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(ad.number_of_paths(1, 9, 0, 10, algorithm::acyclic),
                      LibsemigroupsException);

    ad = binary_tree(n);
    REQUIRE(ad.number_of_paths_algorithm(0) == algorithm::acyclic);
    REQUIRE(ad.number_of_paths(0) == 1023);

    action_digraph_helper::add_cycle(ad, n);
    ad.add_edge(0, n + 1, 0);
    REQUIRE(!action_digraph_helper::is_acyclic(ad));
    REQUIRE(!ad.validate());
    REQUIRE(ad.number_of_paths(1) == 511);
    REQUIRE(ad.number_of_paths_algorithm(1, 0, POSITIVE_INFINITY)
            == algorithm::acyclic);
    REQUIRE(ad.number_of_paths(1, 0, POSITIVE_INFINITY) == 511);
    REQUIRE(action_digraph_helper::topological_sort(ad).empty());
    REQUIRE(*std::find_if(ad.cbegin_nodes(), ad.cend_nodes(), [&ad](size_t m) {
      return action_digraph_helper::topological_sort(ad, m).empty();
    }) == 1023);
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

  LIBSEMIGROUPS_TEST_CASE("ActionDigraph",
                          "041",
                          "number_of_paths (matrix)",
                          "[quick]") {
    using algorithm = ActionDigraph<size_t>::algorithm;
    // REQUIRE(detail::magic_number(6) * 6 == 14.634);
    // auto ad = ActionDigraph<size_t>::random(6, 3, 15, std::mt19937());
    // std::cout << action_digraph_helper::detail::to_string(ad);
    ActionDigraph<size_t> ad;
    ad.add_nodes(6);
    ad.add_to_out_degree(3);
    ad.add_edge(0, 0, 0);
    ad.add_edge(0, 3, 1);
    ad.add_edge(0, 4, 2);
    ad.add_edge(1, 2, 0);
    ad.add_edge(1, 1, 1);
    ad.add_edge(1, 4, 2);
    ad.add_edge(2, 4, 0);
    ad.add_edge(2, 3, 1);
    ad.add_edge(2, 4, 2);
    ad.add_edge(3, 0, 0);
    ad.add_edge(3, 1, 1);
    ad.add_edge(4, 3, 1);
    ad.add_edge(4, 3, 2);
    ad.add_edge(5, 4, 0);
    ad.add_edge(5, 2, 2);

    REQUIRE(ad.number_of_edges() == 15);
    REQUIRE(std::distance(ad.cbegin_pilo(0, 0, 10), ad.cend_pilo()) == 6858);
    REQUIRE(ad.number_of_paths_algorithm(0, 0, 10) == algorithm::matrix);
    REQUIRE(ad.number_of_paths(0, 0, 10) == 6858);
    REQUIRE_THROWS_AS(ad.number_of_paths(1, 0, 10, algorithm::trivial),
                      LibsemigroupsException);
    REQUIRE(ad.number_of_paths_algorithm(0, 10, 12) == algorithm::matrix);
    REQUIRE(ad.number_of_paths(0, 10, 12) == 35300);

    auto checker1 = [&ad](word_type const& w) {
      return 10 <= w.size() && w.size() < 12
             && action_digraph_helper::follow_path(ad, 0, w) != UNDEFINED;
    };
    REQUIRE(std::all_of(ad.cbegin_pilo(0, 10, 12), ad.cend_pilo(), checker1));
    REQUIRE(
        std::unordered_set<word_type>(ad.cbegin_pilo(0, 10, 12), ad.cend_pilo())
            .size()
        == 35300);

    REQUIRE(std::distance(ad.cbegin_pilo(0, 10, 12), ad.cend_pilo()) == 35300);

    REQUIRE(ad.number_of_paths_algorithm(1, 5, 0, 10) == algorithm::trivial);
    REQUIRE(ad.number_of_paths(1, 5, 0, 10) == 0);
    REQUIRE(0
            == std::distance(ad.cbegin_pstilo(1, 5, 0, 10), ad.cend_pstilo()));
    REQUIRE(ad.number_of_paths(1, 1, 0, 10) == 1404);
    REQUIRE_THROWS_AS(ad.number_of_paths(1, 1, 0, 10, algorithm::trivial),
                      LibsemigroupsException);
    REQUIRE(ad.number_of_paths(1, 1, 0, 10)
            == uint64_t(std::distance(ad.cbegin_pstilo(1, 1, 0, 10),
                                      ad.cend_pstilo())));

    auto checker2 = [&ad](word_type const& w) {
      return w.size() < 10 && action_digraph_helper::follow_path(ad, 1, w) == 1;
    };
    REQUIRE(
        std::all_of(ad.cbegin_pstilo(1, 1, 0, 10), ad.cend_pstilo(), checker2));
  }

  LIBSEMIGROUPS_TEST_CASE("ActionDigraph",
                          "042",
                          "number_of_paths (matrix)",
                          "[quick]") {
    using algorithm = ActionDigraph<size_t>::algorithm;
    ActionDigraph<size_t> ad;
    ad.add_nodes(2);
    ad.add_to_out_degree(2);
    ad.add_edge(0, 1, 0);
    ad.add_edge(1, 0, 0);

    REQUIRE(ad.number_of_paths(0, 1, 0, POSITIVE_INFINITY, algorithm::matrix)
            == POSITIVE_INFINITY);
    REQUIRE(ad.number_of_paths(0, 1, 0, 10, algorithm::matrix) == 5);
  }
}  // namespace libsemigroups
