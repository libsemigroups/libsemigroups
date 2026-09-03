//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019-2026 Finn Smith
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

#include <algorithm>      // for min_element, reverse, sort
#include <cmath>          // for pow
#include <cstddef>        // for ptrdiff_t, size_t
#include <numeric>        // for iota
#include <random>         // for mt19937
#include <stdexcept>      // for runtime_error
#include <unordered_set>  // for unordered_set
#include <utility>        // for move
#include <vector>         // for vector

#include "test-main.hpp"               // for LIBSEMIGROUPS_TEST_CASE
#include "word-graph-test-common.hpp"  // for add_clique etc

#include "libsemigroups/forest.hpp"              // for Forest
#include "libsemigroups/order.hpp"               // for LenLexCmp, LexCmp, ...
#include "libsemigroups/paths.hpp"               // for cbegin_pilo
#include "libsemigroups/word-graph-helpers.hpp"  // for word_graph
#include "libsemigroups/word-graph.hpp"          // for WordGraph
#include "libsemigroups/words-helpers.hpp"       // for literals

#include "libsemigroups/detail/report.hpp"  // for ReportGuard
#include "libsemigroups/detail/string.hpp"  // for detail::to_string

namespace libsemigroups {

  using namespace literals;

  struct LibsemigroupsException;  // forward decl

  namespace {

    bool rpo_cmp_recursive_impl(word_type& lhs, word_type& rhs) {
      if (rhs.empty()) {
        return false;
      }
      if (lhs.empty()) {
        return true;
      }

      auto const a = lhs.front();
      auto const b = rhs.front();

      if (a == b) {
        lhs.erase(lhs.begin());
        rhs.erase(rhs.begin());
      } else if (a < b) {
        lhs.erase(lhs.begin());
      } else {
        rhs.erase(rhs.begin());
      }

      return rpo_cmp_recursive_impl(lhs, rhs);
    }

    bool rpo_cmp_recursive(word_type lhs, word_type rhs) {
      auto [common_suffix_lhs, common_suffix_rhs]
          = std::mismatch(lhs.rbegin(), lhs.rend(), rhs.rbegin(), rhs.rend());

      lhs.erase(common_suffix_lhs.base(), lhs.end());
      rhs.erase(common_suffix_rhs.base(), rhs.end());
      return rpo_cmp_recursive_impl(lhs, rhs);
    }

    bool rev_rpo_cmp_recursive(word_type lhs, word_type rhs) {
      std::reverse(lhs.begin(), lhs.end());
      std::reverse(rhs.begin(), rhs.end());
      return rpo_cmp_recursive(lhs, rhs);
    }

    template <typename Node, typename Func>
    std::vector<word_type> minimal_words(WordGraph<Node> const& wg, Func cmp) {
      struct Candidate {
        word_type word;
        Node      node;
      };

      // This best-first search computes the minimal word of each
      // reachable node directly from the given order, independently of the
      // standardization routine under test.
      auto const nr_reachable
          = word_graph::number_of_nodes_reachable_from(wg, Node(0));
      std::vector<bool>      seen(wg.number_of_nodes(), false);
      std::vector<word_type> result(wg.number_of_nodes());
      std::vector<Candidate> frontier = {{{}, 0}};
      size_t                 done     = 0;

      while (done < nr_reachable) {
        auto const it = std::min_element(
            frontier.cbegin(),
            frontier.cend(),
            [&cmp](Candidate const& lhs, Candidate const& rhs) {
              if (cmp(lhs.word, rhs.word)) {
                return true;
              }
              if (cmp(rhs.word, lhs.word)) {
                return false;
              }
              return lhs.node < rhs.node;
            });
        REQUIRE(it != frontier.cend());

        auto current = *it;
        frontier.erase(it);
        if (seen[current.node]) {
          continue;
        }
        seen[current.node]   = true;
        result[current.node] = current.word;
        ++done;

        for (letter_type x = 0; x < wg.out_degree(); ++x) {
          auto const next = wg.target_no_checks(current.node, x);
          if (next != UNDEFINED && !seen[next]) {
            auto word = current.word;
            word.push_back(x);
            frontier.push_back({std::move(word), next});
          }
        }
      }
      return result;
    }

    template <typename Node, typename Func>
    std::pair<WordGraph<Node>, std::vector<word_type>>
    canonical_standardization(WordGraph<Node> const& wg, Func cmp) {
      std::vector<word_type> const min_words = minimal_words(wg, cmp);

      std::vector<Node> p(wg.number_of_nodes());
      std::iota(p.begin(), p.end(), static_cast<Node>(0));
      std::sort(p.begin(), p.end(), [&min_words, &cmp](Node lhs, Node rhs) {
        if (cmp(min_words[lhs], min_words[rhs])) {
          return true;
        }
        if (cmp(min_words[rhs], min_words[lhs])) {
          return false;
        }
        return lhs < rhs;
      });

      std::vector<Node> q(wg.number_of_nodes());
      for (Node i = 0; i < wg.number_of_nodes(); ++i) {
        q[p[i]] = i;
      }

      WordGraph<Node> result = wg;
      result.standardize_no_checks(p, q);

      std::vector<word_type> ordered_words;
      ordered_words.reserve(p.size());
      for (auto node : p) {
        ordered_words.push_back(min_words[node]);
      }
      return {std::move(result), std::move(ordered_words)};
    }

    std::vector<word_type> words_from_forest(Forest const& f) {
      std::vector<word_type> result(f.number_of_nodes());
      for (Forest::node_type node = 1; node < f.number_of_nodes(); ++node) {
        auto current = node;
        while (current != UNDEFINED
               && f.parent_no_checks(current) != UNDEFINED) {
          result[node].push_back(f.label_no_checks(current));
          current = f.parent_no_checks(current);
        }
        std::reverse(result[node].begin(), result[node].end());
      }
      return result;
    }
  }  // namespace

  LIBSEMIGROUPS_TEST_CASE("WordGraph",
                          "000",
                          "constructor with 1  default arg",
                          "[quick][word-graph]") {
    WordGraph<size_t> g;
    REQUIRE(g.number_of_nodes() == 0);
    REQUIRE(g.number_of_edges() == 0);
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph",
                          "001",
                          "constructor with 0 default args",
                          "[quick][word-graph]") {
    for (size_t j = 0; j < 100; ++j) {
      WordGraph<size_t> g(j);
      REQUIRE(g.number_of_nodes() == j);
      REQUIRE(g.number_of_edges() == 0);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph",
                          "002",
                          "constructor with empty targets",
                          "[quick][word-graph]") {
    auto wg = make<WordGraph<size_t>>(10, {});
    REQUIRE(wg.number_of_nodes() == 10);
    REQUIRE(wg.number_of_edges() == 0);
    REQUIRE(wg.out_degree() == 0);
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph",
                          "003",
                          "add nodes",
                          "[quick][word-graph]") {
    WordGraph<size_t> g(3);
    REQUIRE(g.number_of_nodes() == 3);
    REQUIRE(g.number_of_edges() == 0);

    for (size_t i = 1; i < 100; ++i) {
      g.add_nodes(i);
      REQUIRE(g.number_of_nodes() == 3 + i * (i + 1) / 2);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph",
                          "004",
                          "add edges",
                          "[quick][word-graph]") {
    WordGraph<size_t> g(17, 31);

    for (size_t i = 0; i < 17; ++i) {
      for (size_t j = 0; j < 31; ++j) {
        g.target(i, j, (7 * i + 23 * j) % 17);
      }
    }

    REQUIRE(g.number_of_edges() == 31 * 17);
    REQUIRE(g.number_of_nodes() == 17);
    REQUIRE_THROWS_AS(g.target(0, 0, 32), LibsemigroupsException);

    for (size_t i = 0; i < 17; ++i) {
      for (size_t j = 0; j < 31; ++j) {
        REQUIRE(g.target(i, j) == (7 * i + 23 * j) % 17);
      }
    }

    g.add_to_out_degree(10);
    REQUIRE(g.out_degree() == 41);
    REQUIRE(g.number_of_nodes() == 17);
    REQUIRE(!word_graph::is_complete(g));

    for (size_t i = 0; i < 17; ++i) {
      for (size_t j = 0; j < 10; ++j) {
        g.target(i, 31 + j, (7 * i + 23 * j) % 17);
      }
    }

    REQUIRE(g.number_of_edges() == 41 * 17);
    REQUIRE(g.number_of_nodes() == 17);
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph",
                          "005",
                          "exceptions",
                          "[quick][word-graph]") {
    WordGraph<size_t> graph(10, 5);
    REQUIRE_THROWS_AS(graph.target(10, 0), LibsemigroupsException);
    REQUIRE(graph.target(0, 1) == UNDEFINED);

    REQUIRE_THROWS_AS(graph.target(0, 10, 0), LibsemigroupsException);
    REQUIRE_THROWS_AS(graph.target(10, 0, 0), LibsemigroupsException);
    for (size_t i = 0; i < 5; ++i) {
      graph.target(0, i, 1);
      graph.target(2, i, 2);
    }
    REQUIRE_NOTHROW(graph.target(0, 0, 1));
    REQUIRE_NOTHROW(graph.target(2, 0, 2));
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph", "006", "random", "[quick][word-graph]") {
    WordGraph graph = WordGraph<size_t>::random(10, 10);
    REQUIRE(graph.number_of_nodes() == 10);
    REQUIRE(graph.number_of_edges() == 100);
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph",
                          "007",
                          "reserve",
                          "[quick][word-graph]") {
    WordGraph<size_t> graph;
    graph.reserve(10, 10);
    REQUIRE(graph.number_of_nodes() == 0);
    REQUIRE(graph.number_of_edges() == 0);
    graph.add_nodes(1);
    REQUIRE(graph.number_of_nodes() == 1);
    graph.add_nodes(9);
    REQUIRE(graph.number_of_nodes() == 10);
    REQUIRE(graph.number_of_edges() == 0);
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph",
                          "008",
                          "default constructors",
                          "[quick][word-graph]") {
    auto g1 = WordGraph<size_t>();
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

  LIBSEMIGROUPS_TEST_CASE("WordGraph",
                          "009",
                          "iterator to edges",
                          "[quick][word-graph]") {
    for (size_t n = 10; n < 512; n *= 4) {
      auto g = clique(n);
      REQUIRE(g.number_of_nodes() == n);
      REQUIRE(g.number_of_edges() == n * n);

      using node_type = decltype(g)::node_type;

      auto expected = std::vector<node_type>(n, 0);
      std::iota(expected.begin(), expected.end(), 0);

      for (auto it = g.cbegin_nodes(); it < g.cend_nodes(); ++it) {
        auto result = std::vector(g.cbegin_targets(*it), g.cend_targets(*it));
        REQUIRE(result == expected);
      }
      REQUIRE_THROWS_AS(g.cbegin_targets(n), LibsemigroupsException);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph",
                          "010",
                          "reverse node iterator",
                          "[quick]") {
    using node_type = WordGraph<size_t>::node_type;
    WordGraph<size_t> wg;
    wg.add_nodes(10);
    REQUIRE(wg.number_of_nodes() == 10);
    REQUIRE(std::vector(wg.cbegin_nodes(), wg.cend_nodes())
            == std::vector<node_type>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9}));

    auto it = wg.cbegin_nodes();
    REQUIRE(*it == 0);
    auto copy(it);
    REQUIRE(*copy == 0);
    it       = wg.cend_nodes();
    auto tmp = it;
    REQUIRE(*--tmp == 9);
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph",
                          "011",
                          "random/random_acyclic exceptions",
                          "[quick][no-valgrind]") {
    // Too few nodes
    REQUIRE_THROWS_AS(word_graph::random_acyclic<size_t>(0, 0),
                      LibsemigroupsException);
    // Out degree too low
    REQUIRE_NOTHROW(WordGraph<size_t>::random(2, 0));

    REQUIRE_THROWS_AS(word_graph::random_acyclic<size_t>(2, 0),
                      LibsemigroupsException);
    REQUIRE_NOTHROW(word_graph::random_acyclic<size_t>(2, 2));
    auto wg = word_graph::random_acyclic<size_t>(2, 2);
    REQUIRE(word_graph::is_acyclic(wg));
    REQUIRE(word_graph::is_connected(wg));
    for (size_t i = 0; i < 1000; ++i) {
      wg = word_graph::random_acyclic<size_t>(10, 10);
      REQUIRE(word_graph::is_acyclic(wg));
      REQUIRE(word_graph::is_connected(wg));
    }
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph",
                          "012",
                          "unsafe (next) neighbour",
                          "[quick]") {
    auto wg = binary_tree(10);
    REQUIRE(wg.target_no_checks(0, 1) == wg.target(0, 1));
    REQUIRE(wg.next_label_and_target_no_checks(0, 1)
            == wg.next_label_and_target(0, 1));
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph",
                          "013",
                          "number_of_egdes incident to a node",
                          "[quick]") {
    auto wg = binary_tree(10);
    REQUIRE(wg.number_of_nodes() == 1023);
    REQUIRE(
        std::count_if(wg.cbegin_nodes(),
                      wg.cend_nodes(),
                      [&wg](size_t n) { return wg.number_of_edges(n) == 2; })
        == 511);
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph",
                          "014",
                          "induced_subgraph_no_checks",
                          "[quick]") {
    WordGraph<size_t> wg;
    wg.add_nodes(3);
    wg.add_to_out_degree(2);
    wg.target(0, 0, 1);
    wg.target(1, 0, 0);
    wg.target(2, 0, 0);

    wg.induced_subgraph_no_checks(0, 2);
    REQUIRE(wg == make<WordGraph<size_t>>(2, {{1, UNDEFINED}, {0}}));
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph",
                          "015",
                          "remove_target_no_checks",
                          "[quick]") {
    WordGraph<size_t> wg;
    wg.add_nodes(3);
    wg.add_to_out_degree(2);
    wg.target(0, 0, 1);
    wg.target(1, 0, 0);
    wg.target(2, 0, 0);

    wg.remove_target_no_checks(0, 0);  // remove edge from 0 labelled 0
    REQUIRE(wg
            == make<WordGraph<size_t>>(3, {{UNDEFINED, UNDEFINED}, {0}, {0}}));
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph",
                          "016",
                          "swap_edge_no_checks",
                          "[quick]") {
    WordGraph<size_t> wg;
    wg.add_nodes(3);
    wg.add_to_out_degree(2);
    wg.target(0, 0, 1);
    wg.target(1, 0, 0);
    wg.target(2, 0, 2);

    // swap edge from 0 labelled 0 with edge from 1 labelled 0
    wg.swap_targets_no_checks(0, 1, 0);
    REQUIRE(wg == make<WordGraph<size_t>>(3, {{0, UNDEFINED}, {1}, {2}}));
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph", "017", "operator<<", "[quick]") {
    WordGraph<uint64_t> wg;
    wg.add_nodes(3);
    wg.add_to_out_degree(2);
    wg.target(0, 0, 1);
    wg.target(1, 0, 0);
    wg.target(2, 0, 2);

    std::ostringstream oss;
    // This seems silly, but JDE couldn't figure out a way to get this to work.
    // Tried "using operator<<;" but that didn't work for "operator is
    // ambiguous" reasons.
    operator<<(oss, wg);
    REQUIRE(oss.str()
            == "{3, {{1, 18446744073709551615}, {0, 18446744073709551615}, {2, "
               "18446744073709551615}}}");
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph",
                          "018",
                          "is_acyclic | 2-cycle",
                          "[quick]") {
    WordGraph<size_t> wg;
    wg.add_nodes(2);
    wg.add_to_out_degree(1);
    wg.target(0, 0, 1);
    wg.target(1, 0, 0);
    REQUIRE(!word_graph::is_acyclic(wg));
    REQUIRE(word_graph::topological_sort(wg).empty());
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph",
                          "019",
                          "is_acyclic | 1-cycle",
                          "[quick]") {
    WordGraph<size_t> wg;
    wg.add_nodes(1);
    wg.add_to_out_degree(1);
    wg.target(0, 0, 0);
    REQUIRE(!word_graph::is_acyclic(wg));
    REQUIRE(word_graph::topological_sort(wg).empty());
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph",
                          "020",
                          "is_acyclic | multi-digraph",
                          "[quick]") {
    using node_type = WordGraph<size_t>::node_type;
    WordGraph<size_t> wg;
    wg.add_nodes(2);
    wg.add_to_out_degree(2);
    wg.target(0, 0, 1);
    wg.target(0, 1, 1);
    REQUIRE(word_graph::is_acyclic(wg));
    REQUIRE(word_graph::topological_sort(wg) == std::vector<node_type>({1, 0}));
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph",
                          "021",
                          "is_acyclic | complete digraph 100",
                          "[quick]") {
    WordGraph<size_t> wg;
    size_t const      n = 100;
    wg.add_nodes(n);
    wg.add_to_out_degree(n);
    for (size_t i = 0; i < n; ++i) {
      for (size_t j = 0; j < n; ++j) {
        if (i != j) {
          wg.target(i, j, j);
        }
      }
    }
    REQUIRE(!word_graph::is_acyclic(wg));
    REQUIRE(word_graph::topological_sort(wg).empty());
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph",
                          "022",
                          "is_acyclic | acyclic digraph with 20000 nodes",
                          "[quick]") {
    WordGraph<size_t> wg;
    size_t const      n = 20000;
    wg.add_nodes(n);
    wg.add_to_out_degree(2);
    for (size_t i = 0; i < (n / 2 - 1); ++i) {
      wg.target(i, 0, i + 1);
    }
    wg.target(n / 2 - 1, 1, n - 1);
    wg.target(n / 2 + 1, 1, (3 * n) / 4 - 1);
    wg.target(n / 2, 1, 0);
    for (size_t i = n / 2; i < n - 1; ++i) {
      wg.target(i, 0, i + 1);
    }
    REQUIRE(word_graph::is_acyclic(wg));
    REQUIRE(word_graph::topological_sort(wg).size() == wg.number_of_nodes());
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph",
                          "023",
                          "is_acyclic | acyclic digraph with 10 million nodes",
                          "[standard]") {
    WordGraph<size_t> wg;
    size_t const      n = 10000000;
    wg.add_nodes(n);
    wg.add_to_out_degree(2);
    for (size_t i = 0; i < (n / 2 - 1); ++i) {
      wg.target(i, 0, i + 1);
    }
    wg.target(n / 2 - 1, 1, n - 1);
    wg.target(n / 2 + 1, 1, (3 * n) / 4 - 1);
    wg.target(n / 2, 1, 0);
    for (size_t i = n / 2; i < n - 1; ++i) {
      wg.target(i, 0, i + 1);
    }
    REQUIRE(word_graph::is_acyclic(wg));
    REQUIRE(word_graph::topological_sort(wg).size() == n);
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph",
                          "024",
                          "is_acyclic | for a node",
                          "[quick]") {
    using node_type = WordGraph<size_t>::node_type;
    WordGraph<size_t> wg;
    size_t const      n = 100;
    wg.add_nodes(n);
    wg.add_to_out_degree(2);
    for (size_t i = 0; i < n - 1; ++i) {
      wg.target(i, i % 2, i + 1);
    }
    word_graph::add_cycle(wg, 100);

    REQUIRE(std::all_of(
        wg.cbegin_nodes(), wg.cbegin_nodes() + 100, [&wg](node_type const& v) {
          return word_graph::is_acyclic(wg, v);
        }));

    REQUIRE(!word_graph::is_acyclic(wg));
    REQUIRE(word_graph::is_acyclic(wg, node_type(10), node_type(20)));
    REQUIRE(!word_graph::is_acyclic(wg, node_type(100), node_type(120)));
    REQUIRE(word_graph::is_acyclic(wg, node_type(10), node_type(120)));
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph",
                          "025",
                          "is_acyclic | for a node | 2",
                          "[quick]") {
    WordGraph<size_t> wg;
    using node_type = decltype(wg)::node_type;
    wg.add_nodes(4);
    wg.add_to_out_degree(1);
    wg.target(0, 0, 1);
    wg.target(1, 0, 0);
    wg.target(2, 0, 3);
    REQUIRE(!word_graph::is_acyclic(wg));
    REQUIRE(word_graph::topological_sort(wg).empty());
    REQUIRE(!word_graph::is_acyclic(wg, node_type(0)));
    REQUIRE(word_graph::topological_sort(wg, node_type(0)).empty());
    REQUIRE(!word_graph::is_acyclic(wg, node_type(1)));
    REQUIRE(word_graph::topological_sort(wg, node_type(1)).empty());

    REQUIRE(word_graph::is_acyclic(wg, node_type(2)));
    REQUIRE(word_graph::topological_sort(wg, node_type(2))
            == std::vector<node_type>({3, 2}));
    REQUIRE(word_graph::is_acyclic(wg, node_type(3)));
    REQUIRE(word_graph::topological_sort(wg, node_type(3))
            == std::vector<node_type>({3}));
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph",
                          "026",
                          "is_reachable | acyclic 20 node digraph",
                          "[quick]") {
    WordGraph<size_t> wg;
    using node_type = decltype(wg)::node_type;
    size_t const n  = 20;
    wg.add_nodes(n);
    wg.add_to_out_degree(2);
    for (size_t i = 0; i < (n / 2 - 1); ++i) {
      wg.target(i, 0, i + 1);
    }
    wg.target(n / 2 - 1, 1, n - 1);
    wg.target(n / 2 + 1, 1, (3 * n) / 4 - 1);
    wg.target(n / 2, 1, 0);
    for (size_t i = n / 2; i < n - 1; ++i) {
      wg.target(i, 0, i + 1);
    }
    REQUIRE(!word_graph::is_reachable(wg, node_type(1), node_type(10)));
    REQUIRE(word_graph::is_reachable(wg, node_type(10), node_type(1)));
    REQUIRE_THROWS_AS(word_graph::is_reachable(wg, node_type(20), node_type(1)),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(word_graph::is_reachable(wg, node_type(1), node_type(20)),
                      LibsemigroupsException);
    REQUIRE(word_graph::is_reachable(wg, node_type(1), node_type(1)));
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph",
                          "027",
                          "is_reachable | 100 node chain",
                          "[quick][no-valgrind]") {
    WordGraph<size_t> wg;
    size_t const      n = 100;
    wg.add_nodes(n);
    wg.add_to_out_degree(2);
    for (size_t i = 0; i < n - 1; ++i) {
      wg.target(i, i % 2, i + 1);
    }
    for (auto it1 = wg.cbegin_nodes(); it1 < wg.cend_nodes(); ++it1) {
      for (auto it2 = it1 + 1; it2 < wg.cend_nodes(); ++it2) {
        REQUIRE(word_graph::is_reachable(wg, *it1, *it2));
        REQUIRE(!word_graph::is_reachable(wg, *it2, *it1));
      }
    }
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph",
                          "028",
                          "is_reachable | 100 node cycle",
                          "[quick][no-valgrind]") {
    WordGraph<size_t> wg;
    wg.add_to_out_degree(1);
    word_graph::add_cycle(wg, 100);
    for (auto it1 = wg.cbegin_nodes(); it1 < wg.cend_nodes(); ++it1) {
      for (auto it2 = it1 + 1; it2 < wg.cend_nodes(); ++it2) {
        REQUIRE(word_graph::is_reachable(wg, *it1, *it2));
        REQUIRE(word_graph::is_reachable(wg, *it2, *it1));
      }
    }
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph",
                          "029",
                          "is_reachable | 20 node clique",
                          "[quick]") {
    WordGraph<size_t> wg = clique(20);
    for (auto it1 = wg.cbegin_nodes(); it1 < wg.cend_nodes(); ++it1) {
      for (auto it2 = it1 + 1; it2 < wg.cend_nodes(); ++it2) {
        REQUIRE(word_graph::is_reachable(wg, *it1, *it2));
        REQUIRE(word_graph::is_reachable(wg, *it2, *it1));
      }
    }
    REQUIRE(word_graph::is_complete(wg));
    REQUIRE(word_graph::topological_sort(wg).empty());
    REQUIRE(word_graph::topological_sort(wg, size_t(0)).empty());
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph",
                          "030",
                          "follow_path | 20 node chain",
                          "[quick]") {
    WordGraph<size_t> wg = chain(20);
    for (auto it = cbegin_pilo(wg, 0); it != cend_pilo(wg); ++it) {
      REQUIRE(word_graph::follow_path(wg, size_t(0), it->begin(), it->end())
              == it.target());
      REQUIRE(word_graph::follow_path_no_checks(
                  wg, size_t(0), it->begin(), it->end())
              == it.target());
    }
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph",
                          "031",
                          "throw_if_label_out_of_bounds | 20 node chain",
                          "[quick]") {
    WordGraph<size_t> wg = chain(20);
    REQUIRE_EXCEPTION_MSG(std::ignore = wg.target(9, 10),
                          "label value out of bounds, expected value in the "
                          "range [0, 1), got 10");
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph",
                          "032",
                          "last_node_on_path_no_checks | 20 node chain",
                          "[quick]") {
    WordGraph<size_t> wg    = chain(20);
    word_type         chain = {};
    for (size_t i = 0; i < 19; ++i) {
      chain.push_back(0);
      REQUIRE(word_graph::last_node_on_path_no_checks(
                  wg, size_t(0), chain.cbegin(), chain.cend())
                  .first
              == i + 1);
    }
    chain.push_back(0);
    auto p = word_graph::last_node_on_path_no_checks(
        wg, size_t(0), chain.cbegin(), chain.cend());
    REQUIRE(p.first == 19);
    REQUIRE(p.second == chain.cend() - 1);
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph", "033", "to_string", "[quick]") {
    WordGraph<uint64_t> wg = chain(6);
    REQUIRE(detail::to_string(wg)
            == "{6, {{1}, {2}, {3}, {4}, {5}, {18446744073709551615}}}");
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph", "034", "make<WordGraph>", "[quick]") {
    auto wg = make<WordGraph<uint8_t>>(5, {{0, 0}, {1, 1}, {2}, {3, 3}});
    REQUIRE(detail::to_string(wg)
            == "{5, {{0, 0}, {1, 1}, {2, 255}, {3, 3}, {255, 255}}}");
    REQUIRE_THROWS_AS(
        make<WordGraph<uint8_t>>(5, {{0, 0}, {1, 1, 1}, {2}, {3, 3}}),
        LibsemigroupsException);
    wg = WordGraph<uint8_t>(5, 2);
    REQUIRE(
        detail::to_string(wg)
        == "{5, {{255, 255}, {255, 255}, {255, 255}, {255, 255}, {255, 255}}}");
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph", "035", "is_connected", "[quick]") {
    auto wg = make<WordGraph<size_t>>(5, {{0, 0}, {1, 1}, {2}, {3, 3}});
    REQUIRE(!word_graph::is_connected(wg));
    wg = chain(1'000);
    REQUIRE(word_graph::is_connected(wg));
    REQUIRE(wg.number_of_nodes() == 1'000);
    word_graph::add_cycle(wg, 100);
    REQUIRE(wg.number_of_nodes() == 1'100);

    REQUIRE(!word_graph::is_connected(wg));
    wg.add_to_out_degree(1);
    wg.target(0, 1, 1'000);
    REQUIRE(word_graph::is_connected(wg));
    wg = WordGraph<size_t>();
    REQUIRE(word_graph::is_connected(wg));
  }
  LIBSEMIGROUPS_TEST_CASE("WordGraph",
                          "036",
                          "is_strictly_cyclic",
                          "[quick][no-valgrind]") {
    auto wg = make<WordGraph<size_t>>(5, {{0, 0}, {1, 1}, {2}, {3, 3}});
    REQUIRE(!word_graph::is_strictly_cyclic(wg));
    wg = chain(1'000);
    REQUIRE(word_graph::is_strictly_cyclic(wg));
    REQUIRE(wg.number_of_nodes() == 1'000);
    word_graph::add_cycle(wg, 100);
    REQUIRE(wg.number_of_nodes() == 1'100);

    REQUIRE(!word_graph::is_strictly_cyclic(wg));
    wg.add_to_out_degree(1);
    wg.target(0, 1, 1'000);
    REQUIRE(word_graph::is_strictly_cyclic(wg));
    wg = WordGraph<size_t>();
    REQUIRE(word_graph::is_strictly_cyclic(wg));
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph", "037", "Joiner x 1", "[quick]") {
    WordGraph<size_t> x(
        make<WordGraph<size_t>>(3, {{0, 1, 2}, {0, 1, 2}, {0, 1, 2}}));
    WordGraph<size_t> y = x;

    Joiner join;

    WordGraph<size_t> xy;
    join(xy, x, y);
    REQUIRE(xy == x);
    join(xy, y, x);
    REQUIRE(xy == x);
    REQUIRE(join(x, y) == x);
    REQUIRE(join(y, x) == x);

    Meeter meet;
    meet(xy, x, y);
    REQUIRE(xy == x);
    REQUIRE(xy == y);

    y.target_no_checks(0, 0, 10);
    REQUIRE_THROWS_AS(join(x, y), LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph", "038", "Joiner x 2", "[quick]") {
    WordGraph<size_t> x(
        make<WordGraph<size_t>>(3, {{1, 1, 1}, {2, 2, 2}, {2, 2, 2}}));

    WordGraph<size_t> y(
        make<WordGraph<size_t>>(3, {{1, 1, 2}, {1, 1, 2}, {1, 1, 2}}));

    WordGraph<size_t> xy;

    Joiner join;
    xy = join(x, y);
    REQUIRE(x != y);
    REQUIRE(xy == make<WordGraph<size_t>>(2, {{1, 1, 1}, {1, 1, 1}}));
    REQUIRE(join.is_subrelation(x, xy));
    REQUIRE(join.is_subrelation(y, xy));

    using node_type      = WordGraph<size_t>::node_type;
    node_type const root = 0;
    REQUIRE(join.is_subrelation(x, root, xy, root));
    REQUIRE(join.is_subrelation_no_checks(x, root, xy, root));
    REQUIRE(join.is_subrelation_no_checks(x, 3, root, xy, 2, root));

    Meeter meet;
    REQUIRE(meet.is_subrelation(x, root, xy, root));
    REQUIRE(meet.is_subrelation_no_checks(x, root, xy, root));
    REQUIRE(meet.is_subrelation_no_checks(x, 3, root, xy, 2, root));
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph", "039", "Meeter x 1", "[quick]") {
    // These word graphs were taken from the lattice of
    // 2-sided congruences of the free semigroup with 2
    // generators.
    WordGraph<size_t> x(make<WordGraph<size_t>>(3, {{1, 2}, {1, 1}, {2, 2}}));
    WordGraph<size_t> y(make<WordGraph<size_t>>(3, {{1, 2}, {1, 1}, {1, 1}}));

    WordGraph<size_t> xy;

    Meeter meet;
    meet(xy, x, y);

    REQUIRE(xy == make<WordGraph<size_t>>(4, {{1, 2}, {1, 1}, {3, 3}, {3, 3}}));

    y = make<WordGraph<size_t>>(3, {{1, 2}, {2, 2}, {2, 2}});

    meet(xy, x, y);
    REQUIRE(xy == make<WordGraph<size_t>>(4, {{1, 2}, {3, 3}, {2, 2}, {3, 3}}));

    word_graph::standardize(xy, LenLexCmp());
    REQUIRE(xy == make<WordGraph<size_t>>(4, {{1, 2}, {3, 3}, {2, 2}, {3, 3}}));

    x = xy;
    meet(xy, x, y);
    REQUIRE(xy == make<WordGraph<size_t>>(4, {{1, 2}, {3, 3}, {2, 2}, {3, 3}}));
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph", "040", "Meeter x 2", "[quick]") {
    auto x = make<WordGraph<size_t>>(5, {{1, 0}, {1, 2}, {1, 2}});
    auto y = make<WordGraph<size_t>>(5, {{0, 1}, {0, 1}});
    REQUIRE(word_graph::number_of_nodes_reachable_from(x, size_t(0)) == 3);
    REQUIRE(word_graph::number_of_nodes_reachable_from(y, size_t(0)) == 2);

    Meeter meet;
    auto   xy = meet(x, y);
    REQUIRE(xy == make<WordGraph<size_t>>(4, {{1, 2}, {1, 3}, {1, 2}, {1, 3}}));
    word_graph::standardize(xy, LenLexCmp());
    REQUIRE(xy == make<WordGraph<size_t>>(4, {{1, 2}, {1, 3}, {1, 2}, {1, 3}}));

    Joiner join;
    join(xy, x, y);
    REQUIRE(xy == make<WordGraph<size_t>>(1, {{0, 0}}));
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph", "041", "Joiner incomplete", "[quick]") {
    WordGraph<uint32_t> wg(0, 1);
    word_graph::add_cycle(wg, 5);
    wg.remove_target(0, 0);
    Joiner join;
    REQUIRE(join(wg, wg) == make<WordGraph<uint32_t>>(1, {{UNDEFINED}}));
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph", "042", "Meeter incomplete", "[quick]") {
    WordGraph<uint32_t> wg(0, 1);
    word_graph::add_cycle(wg, 5);
    wg.remove_target(0, 0);
    Meeter meet;
    REQUIRE(meet(wg, wg) == make<WordGraph<uint32_t>>(1, {{UNDEFINED}}));
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph",
                          "043",
                          "WordGraph to_input_string",
                          "[quick]") {
    WordGraph<uint32_t> wg(0, 1);
    word_graph::add_cycle(wg, 5);
    wg.remove_target(0, 0);
    REQUIRE(to_input_string(wg) == "5, {{4294967295}, {2}, {3}, {4}, {0}}");
    REQUIRE(to_input_string(wg, "make<WordGraph<uint32_t>>(", "[]", ")")
            == "make<WordGraph<uint32_t>>(5, [[4294967295], [2], [3], [4], "
               "[0]])");
    word_graph::add_cycle(wg, 1000);
    REQUIRE(to_human_readable_repr(wg)
            == "<WordGraph with 1,005 nodes, 1,004 edges, & out-degree 1>");
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph", "045", "hash_value", "[quick]") {
    WordGraph<uint32_t> wg(0, 1);
    word_graph::add_cycle(wg, 5);
    wg.remove_target(0, 0);
    REQUIRE_NOTHROW(wg.hash_value());
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph",
                          "044",
                          "disjoint_union_inplace exception",
                          "[quick]") {
    WordGraph<uint32_t> wg(0, 1);
    REQUIRE_THROWS_AS(wg.disjoint_union_inplace(wg), LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph", "046", "3-arg dot", "[quick]") {
    auto wg = make<WordGraph<uint32_t>>(4, {{0, 2}, {3, 1}, {3, 2}, {3, 3}});

    std::vector<std::string> node_labels = {"a", "b", "ab", "ba"};
    std::vector<std::string> edge_labels = {"a", "b"};
    Dot dot = word_graph::dot(wg, node_labels, edge_labels);
    REQUIRE(dot.to_string()
            == "digraph WordGraph {\n"
               "\n"
               "subgraph cluster_legend {\n"
               "  label=\"legend\"node [shape=plaintext]\n"
               "  \n"
               "  cluster_legend_head  [label=<<table border=\"0\" "
               "cellpadding=\"2\" cellspacing=\"0\" cellborder=\"0\">\n"
               "<tr><td align=\"right\" port=\"port0\">a&nbsp;</td></tr>\n"
               "<tr><td align=\"right\" port=\"port1\">b&nbsp;</td></tr>\n"
               "</table>>\n"
               "]\n"
               "  cluster_legend_tail  [label=<<table border=\"0\" "
               "cellpadding=\"2\" cellspacing=\"0\" cellborder=\"0\">\n"
               "<tr><td align=\"right\" port=\"port0\">&nbsp;</td></tr>\n"
               "<tr><td align=\"right\" port=\"port1\">&nbsp;</td></tr>\n"
               "</table>>\n"
               "]\n"
               "  cluster_legend_head:port0:e -> "
               "cluster_legend_tail:port0:w  [color=\"#00ff00\", "
               "constraint=\"false\"]\n"
               "  cluster_legend_head:port1:e -> "
               "cluster_legend_tail:port1:w  [color=\"#ff00ff\", "
               "constraint=\"false\"]\n"
               "}\n"
               "  0  [label=\"a\", shape=\"box\"]\n"
               "  1  [label=\"b\", shape=\"box\"]\n"
               "  2  [label=\"ab\", shape=\"box\"]\n"
               "  3  [label=\"ba\", shape=\"box\"]\n"
               "  0 -> 0  [color=\"#00ff00\"]\n"
               "  0 -> 2  [color=\"#ff00ff\"]\n"
               "  1 -> 3  [color=\"#00ff00\"]\n"
               "  1 -> 1  [color=\"#ff00ff\"]\n"
               "  2 -> 3  [color=\"#00ff00\"]\n"
               "  2 -> 2  [color=\"#ff00ff\"]\n"
               "  3 -> 3  [color=\"#00ff00\"]\n"
               "  3 -> 3  [color=\"#ff00ff\"]\n"
               "}");
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph",
                          "047",
                          "exception: 3-arg dot",
                          "[quick]") {
    auto wg = make<WordGraph<uint32_t>>(4, {{0, 1}, {1, 2}, {2, 3}, {3, 2}});

    std::vector<std::string> node_labels = {"a", "b", "ab", "ba"};
    std::vector<std::string> edge_labels = {"a", "b"};

    REQUIRE_EXCEPTION_MSG(
        std::ignore = word_graph::dot(wg, {"a", "b", "ab"}, edge_labels),
        "expected the 2nd argument (node labels) to have size 4, the number of "
        "nodes of the 1st argument (word graph), but found 3");

    REQUIRE_EXCEPTION_MSG(
        std::ignore = word_graph::dot(wg, node_labels, {"a"}),
        "expected the 3rd argument (edge labels) to have size 2, the "
        "out-degree of the 1st argument (word graph), but found 1");
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph",
                          "048",
                          "rev_rpo standardization | textbook example",
                          "[quick][word-graph]") {
    auto wg = make<WordGraph<size_t>>(
        6, {{1, 3}, {2}, {0, 5}, {4}, {UNDEFINED, 2}, {0}});

    auto const expected_words
        = std::vector<word_type>({{}, {0}, {0, 0}, {1}, {1, 0}, {0, 0, 1}});
    REQUIRE(minimal_words(wg, rev_rpo_cmp_recursive) == expected_words);
    REQUIRE(word_graph::is_standardized(wg, RevRPOCmp()));

    Forest f;
    REQUIRE(!word_graph::standardize(wg, f, RevRPOCmp()));
    REQUIRE(word_graph::is_standardized(wg, RevRPOCmp()));
    REQUIRE(std::is_sorted(
        expected_words.begin(), expected_words.end(), RevRPOCmp()));
    REQUIRE(words_from_forest(f) == expected_words);
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph",
                          "049",
                          "rev_rpo standardization | permuted textbook example",
                          "[quick][word-graph]") {
    auto canonical = make<WordGraph<size_t>>(
        6, {{1, 3}, {2}, {0, 5}, {4}, {UNDEFINED, 2}, {0}});
    auto permuted = canonical;

    // old -> new
    std::vector<size_t> p({0, 2, 3, 4, 5, 1});

    // new -> old
    std::vector<size_t> q(p.size(), 0);
    for (size_t i = 0; i < p.size(); ++i) {
      q[p[i]] = i;
    }
    permuted.standardize_no_checks(q, p);

    REQUIRE(word_graph::is_standardized(canonical, RevRPOCmp()));
    REQUIRE(permuted
            == make<WordGraph<size_t>>(
                6, {{2, 4}, {0}, {3}, {0, 1}, {5}, {UNDEFINED, 3}}));

    REQUIRE(!word_graph::is_standardized(permuted, RevRPOCmp()));

    Forest f;
    REQUIRE(word_graph::standardize(permuted, f, RevRPOCmp()));
    REQUIRE(permuted == canonical);
    REQUIRE(word_graph::is_standardized(permuted, RevRPOCmp()));
    REQUIRE(words_from_forest(f)
            == minimal_words(canonical, rev_rpo_cmp_recursive));
    auto const words = words_from_forest(f);
    REQUIRE(std::is_sorted(words.begin(), words.end(), RevRPOCmp()));
  }

  LIBSEMIGROUPS_TEST_CASE(
      "WordGraph",
      "050",
      "rev_rpo standardization | recursive three-letter case",
      "[quick][word-graph]") {
    auto wg
        = make<WordGraph<size_t>>(7, {{1, 3, 5}, {2, 4}, {}, {6}, {}, {4}, {}});

    auto const expected = canonical_standardization(wg, rev_rpo_cmp_recursive);

    REQUIRE(!word_graph::is_standardized(wg, RevRPOCmp()));

    Forest f;
    REQUIRE(word_graph::standardize(wg, f, RevRPOCmp()));
    REQUIRE(wg == expected.first);
    REQUIRE(word_graph::is_standardized(wg, RevRPOCmp()));
    REQUIRE(words_from_forest(f) == expected.second);
    REQUIRE(
        expected.second
        == std::vector<word_type>({{}, {0}, {0, 0}, {1}, {1, 0}, {0, 1}, {2}}));
    REQUIRE(std::is_sorted(
        expected.second.begin(), expected.second.end(), RevRPOCmp()));
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph",
                          "051",
                          "random standardization",
                          "[quick][word-graph]") {
    WordGraph wg = WordGraph<size_t>::random(5000, 8);
    REQUIRE(wg.number_of_nodes() == 5000);
    REQUIRE(wg.number_of_edges() == 40000);
    WordGraph<size_t> wg1 = wg;
    WordGraph<size_t> wg2 = wg;
    WordGraph<size_t> wg3 = wg;
    WordGraph<size_t> wg4 = wg;

    SECTION("LenLex") {
      Forest const f = word_graph::standardize(wg1, LenLexCmp()).second;
      std::vector<word_type> sorted_words = words_from_forest(f);
      REQUIRE(std::is_sorted(
          sorted_words.begin(), sorted_words.end(), LenLexCmp()));
    }
    SECTION("Lex") {
      Forest const           f = word_graph::standardize(wg2, LexCmp()).second;
      std::vector<word_type> sorted_words = words_from_forest(f);
      REQUIRE(
          std::is_sorted(sorted_words.begin(), sorted_words.end(), LexCmp()));
    }
    SECTION("RPO") {
      Forest const           f = word_graph::standardize(wg3, RPOCmp()).second;
      std::vector<word_type> sorted_words = words_from_forest(f);
      REQUIRE(
          std::is_sorted(sorted_words.begin(), sorted_words.end(), RPOCmp()));
    }
    SECTION("RevRPO") {
      Forest const f = word_graph::standardize(wg4, RevRPOCmp()).second;
      std::vector<word_type> sorted_words = words_from_forest(f);
      REQUIRE(std::is_sorted(
          sorted_words.begin(), sorted_words.end(), RevRPOCmp()));
    }
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph",
                          "052",
                          "all words standardization",
                          "[quick][word-graph]") {
    // Construct the WordGraph such that the paths from 0 are labelled by the
    // words with length in [0, max_depth), consisting of letters in
    // [0, num_letters).
    size_t const max_depth   = 8;
    size_t const num_letters = 5;
    size_t const num_nodes
        = (std::pow(num_letters, max_depth) - 1) / (num_letters - 1);
    size_t const num_sources
        = (std::pow(num_letters, max_depth - 1) - 1) / (num_letters - 1);
    WordGraph<size_t> wg(num_nodes, num_letters);

    for (size_t s = 0; s < num_sources; s++) {
      size_t t = s * num_letters + 1;
      for (size_t letter = 0; letter < num_letters; letter++) {
        wg.target(s, letter, t + letter);
      }
    }
    WordGraph<size_t> wg1 = wg;
    WordGraph<size_t> wg2 = wg;
    WordGraph<size_t> wg3 = wg;
    WordGraph<size_t> wg4 = wg;

    SECTION("LenLex") {
      Forest const f = word_graph::standardize(wg1, LenLexCmp()).second;
      std::vector<word_type> sorted_words = words_from_forest(f);
      REQUIRE(std::is_sorted(
          sorted_words.begin(), sorted_words.end(), LenLexCmp()));
    }
    SECTION("Lex") {
      Forest const           f = word_graph::standardize(wg2, LexCmp()).second;
      std::vector<word_type> sorted_words = words_from_forest(f);
      REQUIRE(
          std::is_sorted(sorted_words.begin(), sorted_words.end(), LexCmp()));
    }
    SECTION("RPO") {
      Forest const           f = word_graph::standardize(wg3, RPOCmp()).second;
      std::vector<word_type> sorted_words = words_from_forest(f);
      REQUIRE(
          std::is_sorted(sorted_words.begin(), sorted_words.end(), RPOCmp()));
    }
    SECTION("RevRPO") {
      Forest const f = word_graph::standardize(wg4, RevRPOCmp()).second;
      std::vector<word_type> sorted_words = words_from_forest(f);
      REQUIRE(std::is_sorted(
          sorted_words.begin(), sorted_words.end(), RevRPOCmp()));
    }
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph", "053", "spanning_tree", "[quick]") {
    auto          rg = ReportGuard(false);
    WordGraph     wg = binary_tree(16);
    WordGraphView wgv(wg);
    REQUIRE(wgv.number_of_nodes() == 65535);

    Forest f;
    word_graph::spanning_tree_no_checks(wgv, size_t(0), f, 0);

    for (size_t depth = 0; depth != 16; ++depth) {
      word_graph::spanning_tree_no_checks(wgv, size_t(0), f, depth);
      REQUIRE(f.number_of_nodes() == std::pow(2, depth + 1) - 1);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph",
                          "054",
                          "nodes_reachable_from (with max_depth)",
                          "[quick]") {
    auto          rg = ReportGuard(false);
    WordGraph     wg = binary_tree(16);
    WordGraphView wgv(wg);
    REQUIRE(wgv.number_of_nodes() == 65535);

    Forest f;
    REQUIRE(word_graph::nodes_reachable_from_no_checks(wgv, size_t(0), 0).size()
            == 1);
    REQUIRE(word_graph::nodes_reachable_from_no_checks(wg, size_t(0), 0).size()
            == 1);

    for (size_t depth = 0; depth != 16; ++depth) {
      REQUIRE(word_graph::nodes_reachable_from_no_checks(wgv, size_t(0), depth)
                  .size()
              == std::pow(2, depth + 1) - 1);
      REQUIRE(word_graph::nodes_reachable_from_no_checks(wg, size_t(0), depth)
                  .size()
              == std::pow(2, depth + 1) - 1);
    }

    wg.init(0, 1999);
    add_clique(wg, 1999);
    wgv.init(wg);

    REQUIRE(word_graph::nodes_reachable_from_no_checks(wgv, size_t(0), 0).size()
            == 1);
    REQUIRE(word_graph::nodes_reachable_from_no_checks(wg, size_t(0), 0).size()
            == 1);

    for (size_t depth = 1; depth != 16; ++depth) {
      REQUIRE(word_graph::nodes_reachable_from_no_checks(wgv, size_t(0), depth)
                  .size()
              == 1999);
      REQUIRE(word_graph::nodes_reachable_from_no_checks(wg, size_t(0), depth)
                  .size()
              == 1999);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph",
                          "055",
                          "uncovered validation overloads",
                          "[quick][word-graph]") {
    WordGraph<uint32_t> wg(2, 2);

    REQUIRE_THROWS_AS(to_input_string(wg, "", "{", ""), LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph",
                          "056",
                          "disjoint_union_inplace",
                          "[quick][word-graph]") {
    auto lhs = make<WordGraph<uint32_t>>(2, {{1}, {1}});
    auto rhs = make<WordGraph<uint32_t>>(2, {{1}, {0}});

    REQUIRE(lhs.disjoint_union_inplace(rhs)
            == make<WordGraph<uint32_t>>(4, {{1}, {1}, {3}, {2}}));

    WordGraph<uint32_t> empty(0, 1);
    REQUIRE(lhs.disjoint_union_inplace_no_checks(empty) == lhs);

    WordGraph<uint32_t> wrong_out_degree(1, 2);
    REQUIRE_THROWS_AS(lhs.disjoint_union_inplace(wrong_out_degree),
                      LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph",
                          "057",
                          "random_acyclic with a fixed seed",
                          "[quick][word-graph]") {
    auto const random = WordGraph<uint16_t>::random(12, 5, std::mt19937(0));
    REQUIRE(random.number_of_nodes() == 12);
    REQUIRE(random.number_of_edges() == 60);

    REQUIRE_THROWS_AS(
        word_graph::random_acyclic<uint16_t>(1, 5, std::mt19937(0)),
        LibsemigroupsException);
    REQUIRE_THROWS_AS(
        word_graph::random_acyclic<uint16_t>(12, 1, std::mt19937(0)),
        LibsemigroupsException);

    for (uint32_t seed = 0; seed < 32; ++seed) {
      auto const wg
          = word_graph::random_acyclic<uint16_t>(12, 5, std::mt19937(seed));
      REQUIRE(wg.number_of_nodes() == 12);
      REQUIRE(wg.out_degree() == 5);
      REQUIRE(word_graph::is_acyclic(wg));
      REQUIRE(word_graph::is_connected(wg));
    }
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph",
                          "058",
                          "standardization and Joiner/Meeter branches",
                          "[quick][word-graph]") {
    WordGraph<uint32_t> empty;
    Forest              forest;
    REQUIRE(!word_graph::standardize_no_checks(empty, forest, LenLexCmp()));

    WordGraph<uint32_t> graph(1, 1);

    Joiner join1;
    Joiner join2(join1);
    Joiner join3(std::move(join2));
    join2 = join1;
    join3 = std::move(join2);

    Meeter meet1;
    Meeter meet2(meet1);
    Meeter meet3(std::move(meet2));
    meet2 = meet1;
    meet3 = std::move(meet2);

    REQUIRE(join3(graph, graph) == graph);
    REQUIRE(meet3(graph, graph) == graph);

    WordGraph<uint32_t> wrong_out_degree(1, 2);
    REQUIRE_THROWS_AS(join1(graph, wrong_out_degree), LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph",
                          "059",
                          "random generic standardization",
                          "[quick][word-graph]") {
    WordGraph wg = WordGraph<size_t>::random(5000, 8);
    REQUIRE(wg.number_of_nodes() == 5000);
    REQUIRE(wg.number_of_edges() == 40000);
    WordGraph<size_t> wg1 = wg;
    WordGraph<size_t> wg2 = wg;
    WordGraph<size_t> wg3 = wg;
    WordGraph<size_t> wg4 = wg;
    WordGraph<size_t> wg5 = wg;
    WordGraph<size_t> wg6 = wg;
    WordGraph<size_t> wg7 = wg;
    WordGraph<size_t> wg8 = wg;

    SECTION("LenLex") {
      word_graph::standardize(wg1, LenLexCmp());
      word_graph::standardize(wg2, [](auto const& w1, auto const& w2) {
        return lenlex_cmp(w1, w2);
      });
      REQUIRE(wg1 == wg2);
    }
    SECTION("Lex") {
      word_graph::standardize(wg3, LexCmp());
      word_graph::standardize(
          wg4, [](auto const& w1, auto const& w2) { return lex_cmp(w1, w2); });
      REQUIRE(wg3 == wg4);
    }
    SECTION("RPO") {
      word_graph::standardize(wg5, RPOCmp());
      word_graph::standardize(
          wg6, [](auto const& w1, auto const& w2) { return rpo_cmp(w1, w2); });
      REQUIRE(wg5 == wg6);
    }
    SECTION("RevRPO") {
      word_graph::standardize(wg7, RevRPOCmp());
      word_graph::standardize(wg8, [](auto const& w1, auto const& w2) {
        return rev_rpo_cmp(w1, w2);
      });
      REQUIRE(wg7 == wg8);
    }
  }
  LIBSEMIGROUPS_TEST_CASE("WordGraph",
                          "060",
                          "all words generic standardization",
                          "[quick][word-graph]") {
    // Construct the WordGraph such that the paths from 0 are labelled by the
    // words with length in [0, max_depth), consisting of letters in
    // [0, num_letters).
    size_t const max_depth   = 8;
    size_t const num_letters = 5;
    size_t const num_nodes
        = (std::pow(num_letters, max_depth) - 1) / (num_letters - 1);
    size_t const num_sources
        = (std::pow(num_letters, max_depth - 1) - 1) / (num_letters - 1);
    WordGraph<size_t> wg(num_nodes, num_letters);

    for (size_t s = 0; s < num_sources; s++) {
      size_t t = s * num_letters + 1;
      for (size_t letter = 0; letter < num_letters; letter++) {
        wg.target(s, letter, t + letter);
      }
    }
    WordGraph<size_t> wg1 = wg;
    WordGraph<size_t> wg2 = wg;
    WordGraph<size_t> wg3 = wg;
    WordGraph<size_t> wg4 = wg;

    SECTION("LenLex") {
      Forest const f
          = word_graph::standardize(wg1, [](auto const& w1, auto const& w2) {
              return lenlex_cmp(w1, w2);
            }).second;
      std::vector<word_type> sorted_words = words_from_forest(f);
      REQUIRE(std::is_sorted(
          sorted_words.begin(), sorted_words.end(), LenLexCmp()));
    }
    SECTION("Lex") {
      Forest const f
          = word_graph::standardize(wg2, [](auto const& w1, auto const& w2) {
              return lex_cmp(w1, w2);
            }).second;
      std::vector<word_type> sorted_words = words_from_forest(f);
      REQUIRE(
          std::is_sorted(sorted_words.begin(), sorted_words.end(), LexCmp()));
    }
    SECTION("RPO") {
      Forest const f
          = word_graph::standardize(wg3, [](auto const& w1, auto const& w2) {
              return rpo_cmp(w1, w2);
            }).second;
      std::vector<word_type> sorted_words = words_from_forest(f);
      REQUIRE(
          std::is_sorted(sorted_words.begin(), sorted_words.end(), RPOCmp()));
    }
    SECTION("RevRPO") {
      Forest const f
          = word_graph::standardize(wg4, [](auto const& w1, auto const& w2) {
              return rev_rpo_cmp(w1, w2);
            }).second;
      std::vector<word_type> sorted_words = words_from_forest(f);
      REQUIRE(std::is_sorted(
          sorted_words.begin(), sorted_words.end(), RevRPOCmp()));
    }
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph",
                          "061",
                          "random is_standardized",
                          "[quick][word-graph]") {
    WordGraph wg = WordGraph<size_t>::random(5000, 8);
    REQUIRE(wg.number_of_nodes() == 5000);
    REQUIRE(wg.number_of_edges() == 40000);
    WordGraph<size_t> wg1 = wg;
    WordGraph<size_t> wg2 = wg;
    WordGraph<size_t> wg3 = wg;
    WordGraph<size_t> wg4 = wg;

    SECTION("LenLex") {
      REQUIRE(!word_graph::is_standardized(wg1, LenLexCmp()));
      word_graph::standardize(wg1, LenLexCmp());
      REQUIRE(word_graph::is_standardized(wg1, LenLexCmp()));
    }
    SECTION("Lex") {
      REQUIRE(!word_graph::is_standardized(wg2, LexCmp()));
      word_graph::standardize(wg2, LexCmp());
      REQUIRE(word_graph::is_standardized(wg2, LexCmp()));
    }
    SECTION("RPO") {
      REQUIRE(!word_graph::is_standardized(wg3, RPOCmp()));
      word_graph::standardize(wg3, RPOCmp());
      REQUIRE(word_graph::is_standardized(wg3, RPOCmp()));
    }
    SECTION("RevRPO") {
      REQUIRE(!word_graph::is_standardized(wg4, RevRPOCmp()));
      word_graph::standardize(wg4, RevRPOCmp());
      REQUIRE(word_graph::is_standardized(wg4, RevRPOCmp()));
    }
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph",
                          "062",
                          "random generic is_standardized",
                          "[quick][word-graph]") {
    WordGraph wg = WordGraph<size_t>::random(5000, 8);
    REQUIRE(wg.number_of_nodes() == 5000);
    REQUIRE(wg.number_of_edges() == 40000);
    WordGraph<size_t> wg1 = wg;
    WordGraph<size_t> wg2 = wg;
    WordGraph<size_t> wg3 = wg;
    WordGraph<size_t> wg4 = wg;

    SECTION("LenLex") {
      REQUIRE(
          !word_graph::is_standardized(wg1, [](auto const& w1, auto const& w2) {
            return lenlex_cmp(w1, w2);
          }));
      word_graph::standardize(wg1, LenLexCmp());
      REQUIRE(
          word_graph::is_standardized(wg1, [](auto const& w1, auto const& w2) {
            return lenlex_cmp(w1, w2);
          }));
    }
    SECTION("Lex") {
      REQUIRE(!word_graph::is_standardized(
          wg2, [](auto const& w1, auto const& w2) { return lex_cmp(w1, w2); }));
      word_graph::standardize(wg2, LexCmp());
      REQUIRE(word_graph::is_standardized(
          wg2, [](auto const& w1, auto const& w2) { return lex_cmp(w1, w2); }));
    }
    SECTION("RPO") {
      REQUIRE(!word_graph::is_standardized(
          wg3, [](auto const& w1, auto const& w2) { return rpo_cmp(w1, w2); }));
      word_graph::standardize(wg3, RPOCmp());
      REQUIRE(word_graph::is_standardized(
          wg3, [](auto const& w1, auto const& w2) { return rpo_cmp(w1, w2); }));
    }
    SECTION("RevRPO") {
      REQUIRE(
          !word_graph::is_standardized(wg4, [](auto const& w1, auto const& w2) {
            return rev_rpo_cmp(w1, w2);
          }));
      word_graph::standardize(wg4, RevRPOCmp());
      REQUIRE(
          word_graph::is_standardized(wg4, [](auto const& w1, auto const& w2) {
            return rev_rpo_cmp(w1, w2);
          }));
    }
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph",
                          "063",
                          "equal_to and equal_to_no_checks",
                          "[quick][word-graph]") {
    auto rg = ReportGuard(false);

    auto graph1 = make<WordGraph<size_t>>(3, {{1, 0}, {0, 1}, {0, 1}});
    auto graph2 = make<WordGraph<size_t>>(3, {{1, 0}, {0, 1}, {1, 0}});
    auto graph3 = make<WordGraph<size_t>>(3, {{1, 0}, {0, 1}, {0, 1}});

    REQUIRE(word_graph::equal_to_no_checks(graph1, graph1));
    REQUIRE(word_graph::equal_to(graph1, graph1));
    REQUIRE(word_graph::equal_to_no_checks(graph1, graph3));
    REQUIRE(word_graph::equal_to(graph1, graph3));
    REQUIRE(!word_graph::equal_to_no_checks(graph1, graph2));
    REQUIRE(!word_graph::equal_to(graph1, graph2));

    std::vector<size_t> const equal_nodes = {0, 1};
    REQUIRE(word_graph::equal_to_no_checks(
        graph1, graph2, equal_nodes.cbegin(), equal_nodes.cend()));
    REQUIRE(word_graph::equal_to(
        graph1, graph2, equal_nodes.cbegin(), equal_nodes.cend()));
    std::vector<size_t> const unequal_nodes = {1, 2};
    REQUIRE(!word_graph::equal_to_no_checks(
        graph1, graph2, unequal_nodes.cbegin(), unequal_nodes.cend()));
    REQUIRE(!word_graph::equal_to(
        graph1, graph2, unequal_nodes.cbegin(), unequal_nodes.cend()));

    auto different_size = make<WordGraph<size_t>>(2, {{1, 0}, {0, 1}});
    REQUIRE(!word_graph::equal_to_no_checks(graph1, different_size));
    REQUIRE(!word_graph::equal_to(graph1, different_size));

    auto different_degree = make<WordGraph<size_t>>(3, {{1}, {0}, {1}});
    REQUIRE(!word_graph::equal_to_no_checks(graph1, different_degree));
    REQUIRE(!word_graph::equal_to(graph1, different_degree));

    std::vector<size_t> const invalid_nodes = {3};
    REQUIRE_THROWS_AS(
        word_graph::equal_to(
            graph1, graph2, invalid_nodes.cbegin(), invalid_nodes.cend()),
        LibsemigroupsException);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
    REQUIRE(
        word_graph::equal_to_no_checks(graph1, graph2, size_t(0), size_t(2)));
    REQUIRE(
        !word_graph::equal_to_no_checks(graph1, graph2, size_t(1), size_t(3)));
    REQUIRE(word_graph::equal_to(graph1, graph2, size_t(0), size_t(2)));
    REQUIRE(!word_graph::equal_to(graph1, graph2, size_t(1), size_t(3)));
    REQUIRE(word_graph::equal_to_no_checks(
        graph1, different_size, size_t(0), size_t(2)));
    REQUIRE(word_graph::equal_to(graph1, different_size, size_t(0), size_t(2)));
    REQUIRE_THROWS_AS(
        word_graph::equal_to(graph1, graph2, size_t(0), size_t(4)),
        LibsemigroupsException);
#pragma GCC diagnostic pop
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph",
                          "064",
                          "validate",
                          "[quick][word-graph]") {
    auto rg = ReportGuard(false);

    WordGraph<size_t> graph(3, 2);
    REQUIRE_NOTHROW(validate(graph));

    graph.target_no_checks(1, 0, 3);
    REQUIRE_EXCEPTION_MSG(
        validate(graph),
        "target out of bounds, the edge with source 1 and label 0 has target "
        "3, but expected value in the range [0, 3)");

    graph.target_no_checks(1, 0, 2);
    REQUIRE_NOTHROW(validate(graph));
  }

  LIBSEMIGROUPS_TEST_CASE("WordGraph",
                          "065",
                          "WordGraphView helper pass-throughs",
                          "[quick][word-graph]") {
    auto rg = ReportGuard(false);

    auto graph = make<WordGraph<size_t>>(3, {{1, 2}, {2}, {}});
    auto view  = WordGraphView(graph);

    auto const graph_matrix = word_graph::adjacency_matrix_no_checks(graph);
    auto const view_matrix  = word_graph::adjacency_matrix_no_checks(view);
    for (auto source : graph.nodes()) {
      for (auto target : graph.nodes()) {
        REQUIRE(graph_matrix(source, target) == view_matrix(source, target));
      }
    }

    REQUIRE(word_graph::dot_no_checks(graph).to_string()
            == word_graph::dot_no_checks(view).to_string());
    std::vector<std::string> const node_labels = {"0", "1", "2"};
    std::vector<std::string> const edge_labels = {"a", "b"};
    REQUIRE(
        word_graph::dot_no_checks(graph, node_labels, edge_labels).to_string()
        == word_graph::dot_no_checks(view, node_labels, edge_labels)
               .to_string());

    REQUIRE(word_graph::is_acyclic_no_checks(graph)
            == word_graph::is_acyclic_no_checks(view));
    REQUIRE(word_graph::is_acyclic_no_checks(graph, size_t(0))
            == word_graph::is_acyclic_no_checks(view, size_t(0)));
    REQUIRE(word_graph::is_acyclic_no_checks(graph, size_t(0), size_t(2))
            == word_graph::is_acyclic_no_checks(view, size_t(0), size_t(2)));
    REQUIRE(word_graph::is_connected_no_checks(graph)
            == word_graph::is_connected_no_checks(view));
    REQUIRE(word_graph::is_standardized_no_checks(graph, LenLexCmp())
            == word_graph::is_standardized_no_checks(view, LenLexCmp()));
    REQUIRE(word_graph::is_strictly_cyclic_no_checks(graph)
            == word_graph::is_strictly_cyclic_no_checks(view));
    REQUIRE(word_graph::topological_sort_no_checks(graph)
            == word_graph::topological_sort_no_checks(view));
    REQUIRE(word_graph::topological_sort_no_checks(graph, size_t(0))
            == word_graph::topological_sort_no_checks(view, size_t(0)));
  }
}  // namespace libsemigroups
