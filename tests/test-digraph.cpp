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

#include <cstddef>    // for size_t
#include <stdexcept>  // for runtime_error
#include <vector>     // for vector

#include "catch.hpp"  // for REQUIRE, REQUIRE_THROWS_AS, REQUI...
#include "libsemigroups/digraph-helper.hpp"  // for follow_path
#include "libsemigroups/digraph.hpp"         // for ActionDigraph
#include "libsemigroups/forest.hpp"          // for Forest
#include "libsemigroups/kbe.hpp"             // for KBE
#include "libsemigroups/knuth-bendix.hpp"    // for KnuthBendix
#include "libsemigroups/wilo.hpp"            // for cbegin_wilo
#include "libsemigroups/wislo.hpp"           // for cbegin_wislo
#include "test-main.hpp"                     // for LIBSEMIGROUPS_TEST_CASE

namespace libsemigroups {
  using KnuthBendix = fpsemigroup::KnuthBendix;

  struct LibsemigroupsException;  // forward decl

  void add_cycle(ActionDigraph<size_t>& digraph, size_t n) {
    size_t old_nodes = digraph.nr_nodes();
    digraph.add_nodes(n);
    for (size_t i = old_nodes; i < digraph.nr_nodes() - 1; ++i) {
      digraph.add_edge(i, i + 1, 0);
    }
    digraph.add_edge(digraph.nr_nodes() - 1, old_nodes, 0);
  }

  ActionDigraph<size_t> cycle(size_t n) {
    ActionDigraph<size_t> g(0, 1);
    add_cycle(g, n);
    return g;
  }

  void add_clique(ActionDigraph<size_t>& digraph, size_t n) {
    if (n != digraph.out_degree()) {
      throw std::runtime_error("can't do it!");
    }
    size_t old_nodes = digraph.nr_nodes();
    digraph.add_nodes(n);

    for (size_t i = old_nodes; i < digraph.nr_nodes(); ++i) {
      for (size_t j = old_nodes; j < digraph.nr_nodes(); ++j) {
        digraph.add_edge(i, j, j - old_nodes);
      }
    }
  }

  ActionDigraph<size_t> clique(size_t n) {
    ActionDigraph<size_t> g(0, n);
    add_clique(g, n);
    return g;
  }

  template <typename T>
  auto verify_deref(T const& it) -> typename std::enable_if<
      std::is_same<typename T::value_type, word_type>::value,
      void>::type {
    REQUIRE_NOTHROW(it->size());
  }

  template <typename T>
  auto verify_deref(T const& it) -> typename std::enable_if<
      !std::is_same<typename T::value_type, word_type>::value,
      void>::type {
    REQUIRE_NOTHROW(it->first);
  }

  template <typename T>
  void verify_forward_iterator_requirements(T it) {
    using deref_type = decltype(*it);
    verify_deref(it);
    REQUIRE(std::is_reference<deref_type>::value);
    REQUIRE(
        std::is_const<typename std::remove_reference<deref_type>::type>::value);
    T copy(it);
    REQUIRE(&copy != &it);
    it++;
    auto it_val   = *it;
    auto copy_val = *copy;
    std::swap(it, copy);
    REQUIRE(copy_val == *it);
    REQUIRE(it_val == *copy);

    it.swap(copy);
    REQUIRE(it_val == *it);
    REQUIRE(copy_val == *copy);

    ++copy;
    REQUIRE(*it == *copy);

    ++it;
    copy++;
    REQUIRE(*it == *copy);

    REQUIRE(std::is_same<
            typename std::iterator_traits<T>::reference,
            typename std::iterator_traits<T>::value_type const&>::value);
  }

  LIBSEMIGROUPS_TEST_CASE("ActionDigraph",
                          "000",
                          "constructor with 1  default arg",
                          "[quick][digraph]") {
    ActionDigraph<size_t> g;
    REQUIRE(g.nr_nodes() == 0);
    REQUIRE(g.nr_edges() == 0);
  }

  LIBSEMIGROUPS_TEST_CASE("ActionDigraph",
                          "001",
                          "constructor with 0 default args",
                          "[quick][digraph]") {
    for (size_t j = 0; j < 100; ++j) {
      ActionDigraph<size_t> g(j);
      REQUIRE(g.nr_nodes() == j);
      REQUIRE(g.nr_edges() == 0);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("ActionDigraph",
                          "002",
                          "add nodes",
                          "[quick][digraph]") {
    ActionDigraph<size_t> g(3);
    REQUIRE(g.nr_nodes() == 3);
    REQUIRE(g.nr_edges() == 0);

    for (size_t i = 1; i < 100; ++i) {
      g.add_nodes(i);
      REQUIRE(g.nr_nodes() == 3 + i * (i + 1) / 2);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("ActionDigraph",
                          "003",
                          "add edges",
                          "[quick][digraph]") {
    ActionDigraph<size_t> g(17, 31);

    for (size_t i = 0; i < 17; ++i) {
      // The digraph isn't fully defined
      REQUIRE_THROWS_AS(g.nr_scc(), LibsemigroupsException);
      for (size_t j = 0; j < 31; ++j) {
        g.add_edge(i, (7 * i + 23 * j) % 17, j);
      }
    }

    REQUIRE(g.nr_edges() == 31 * 17);
    REQUIRE(g.nr_nodes() == 17);
    REQUIRE_THROWS_AS(g.add_edge(0, 0, 32), LibsemigroupsException);

    for (size_t i = 0; i < 17; ++i) {
      for (size_t j = 0; j < 31; ++j) {
        REQUIRE(g.neighbor(i, j) == (7 * i + 23 * j) % 17);
      }
    }

    g.add_to_out_degree(10);
    REQUIRE(g.out_degree() == 41);
    REQUIRE(g.nr_nodes() == 17);
    REQUIRE(!g.validate());

    for (size_t i = 0; i < 17; ++i) {
      for (size_t j = 0; j < 10; ++j) {
        g.add_edge(i, (7 * i + 23 * j) % 17, 31 + j);
      }
    }

    REQUIRE(g.nr_edges() == 41 * 17);
    REQUIRE(g.nr_nodes() == 17);
  }

  LIBSEMIGROUPS_TEST_CASE("ActionDigraph",
                          "004",
                          "strongly connected components - cycles",
                          "[quick][digraph]") {
    auto g = cycle(32);
    REQUIRE(g.scc_id(0) == 0);
    g = cycle(33);
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
    for (size_t j = 2; j < 50; ++j) {
      add_cycle(g, j);
      REQUIRE(std::count_if(
                  g.cbegin_nodes(),
                  g.cend_nodes(),
                  [&g, j](size_t nd) -> bool { return g.scc_id(nd) == j - 2; })
              == j);
    }

    REQUIRE(g.nr_nodes() == 1224);
    REQUIRE(g.nr_edges() == 1224);
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
      REQUIRE(graph.nr_scc() == 1);

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

  //     for (size_t i = 0; i < graph.nr_nodes(); ++i) {
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
    ActionDigraph<size_t> graph = cycle(100000);
    using node_type             = decltype(graph)::node_type;

    REQUIRE(std::all_of(
        graph.cbegin_nodes(),
        graph.cend_nodes(),
        [&graph](node_type i) -> bool { return graph.scc_id(i) == 0; }));
    add_cycle(graph, 10101);
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
    REQUIRE(graph.nr_nodes() == 10);
    REQUIRE(graph.nr_edges() == 100);
  }

  LIBSEMIGROUPS_TEST_CASE("ActionDigraph",
                          "015",
                          "reserve",
                          "[quick][digraph]") {
    ActionDigraph<size_t> graph;
    graph.reserve(10, 10);
    REQUIRE(graph.nr_nodes() == 0);
    REQUIRE(graph.nr_edges() == 0);
    graph.add_nodes(1);
    REQUIRE(graph.nr_nodes() == 1);
    graph.add_nodes(9);
    REQUIRE(graph.nr_nodes() == 10);
    REQUIRE(graph.nr_edges() == 0);
  }

  LIBSEMIGROUPS_TEST_CASE("ActionDigraph",
                          "016",
                          "default constructors",
                          "[quick][digraph]") {
    auto g1 = cycle(10);

    // Copy constructor
    auto g2(g1);
    REQUIRE(g2.nr_edges() == 10);
    REQUIRE(g2.nr_nodes() == 10);
    REQUIRE(g2.nr_scc() == 1);

    // Move constructor
    auto g3(std::move(g2));
    REQUIRE(g3.nr_edges() == 10);
    REQUIRE(g3.nr_nodes() == 10);
    REQUIRE(g3.nr_scc() == 1);

    // Copy assignment
    g2 = g3;
    REQUIRE(g2.nr_edges() == 10);
    REQUIRE(g2.nr_nodes() == 10);
    REQUIRE(g2.nr_scc() == 1);
  }

  LIBSEMIGROUPS_TEST_CASE("ActionDigraph",
                          "017",
                          "scc iterators",
                          "[quick][digraph]") {
    using node_type = decltype(clique(1))::node_type;

    for (size_t n = 10; n < 512; n *= 4) {
      auto g = clique(n);
      REQUIRE(g.nr_nodes() == n);
      REQUIRE(g.nr_edges() == n * n);
      REQUIRE(g.nr_scc() == 1);

      add_clique(g, n);

      REQUIRE(g.nr_nodes() == 2 * n);
      REQUIRE(g.nr_edges() == 2 * n * n);
      REQUIRE(g.nr_scc() == 2);

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
      REQUIRE(g.nr_nodes() == 1000);
      REQUIRE(g.nr_edges() == 10000);
      REQUIRE(g.nr_scc() == 100);

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
      REQUIRE(g.nr_nodes() == n);
      REQUIRE(g.nr_edges() == n * n);
      REQUIRE(g.nr_scc() == 1);

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
    REQUIRE(g.nr_nodes() == 1000);
    REQUIRE(g.nr_edges() == 10000);
    REQUIRE(g.nr_scc() == 100);

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
    ActionDigraph<size_t> ad = cycle(100);

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
    ad.add_to_out_degree(S.nr_generators());
    ad.add_nodes(S.size() + 1);

    for (size_t j = 0; j < S.nr_generators(); ++j) {
      ad.add_edge(S.size(), j, j);
    }

    for (size_t i = 0; i < S.size(); ++i) {
      for (size_t j = 0; j < S.nr_generators(); ++j) {
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
      for (size_t j = 0; j < S.nr_generators(); ++j) {
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
    REQUIRE(ad.nr_nodes() == 10);
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
  }
}  // namespace libsemigroups
