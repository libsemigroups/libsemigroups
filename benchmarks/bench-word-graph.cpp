//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2020-2025 James D. Mitchell
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

#include <algorithm>  // for uniform_int_distribution
#include <cstddef>    // for size_t
#include <iostream>   // for std::cout
#include <random>

#include "Catch2-3.8.0/catch_amalgamated.hpp"  // for BENCHMARK, REQUIRE, TEST_CASE

#include "libsemigroups/paths.hpp"            // for Paths etc
#include "libsemigroups/types.hpp"            // for word_type
#include "libsemigroups/word-graph-view.hpp"  // for WordGraphView
#include "libsemigroups/word-graph.hpp"       // for WordGraph

namespace libsemigroups {
  // Old function for comparison with iterators
  template <typename T>
  using node_type = typename WordGraph<T>::node_type;

  template <typename T>
  using label_type = typename WordGraph<T>::label_type;

  template <typename T, typename S>
  std::pair<std::vector<word_type>, std::vector<node_type<T>>>
  paths_in_lex_order(WordGraph<T> const& ad,
                     S const             root,
                     size_t              min = 0,
                     size_t max = libsemigroups::POSITIVE_INFINITY) {
    using node_type  = node_type<T>;
    using label_type = label_type<T>;

    std::pair<std::vector<word_type>, std::vector<node_type>> out;
    if (max == 0) {
      return out;
    } else if (min == 0) {
      out.first.push_back({});
      out.second.push_back(root);
    }

    std::vector<node_type> node;
    node.push_back(root);

    word_type  path;
    node_type  next;
    label_type edge = 0;

    while (!node.empty()) {
      std::tie(edge, next)
          = ad.next_label_and_target_no_checks(node.back(), edge);
      if (next != UNDEFINED && path.size() < max - 1) {
        node.push_back(next);
        path.push_back(edge);
        if (path.size() >= min) {
          out.first.push_back(path);
          out.second.push_back(next);
        }
        edge = 0;
      } else {
        node.pop_back();
        if (!path.empty()) {
          edge = path.back() + 1;
          path.pop_back();
        }
      }
    }
    return out;
  }

  template <typename T, typename S>
  std::vector<word_type>
  paths_in_lex_order2(WordGraph<T> const& ad,
                      S const             first,
                      S const             last,
                      size_t              min = 0,
                      size_t max = libsemigroups::POSITIVE_INFINITY) {
    using node_type  = node_type<T>;
    using label_type = label_type<T>;

    std::vector<word_type> out;
    if (max == 0) {
      return out;
    } else if (min == 0 && first == last) {
      out.push_back({});
    }

    std::vector<node_type> node;
    node.push_back(first);

    word_type  path;
    node_type  next;
    label_type edge = 0;

    while (!node.empty()) {
      std::tie(edge, next)
          = ad.next_label_and_target_no_checks(node.back(), edge);
      if (next != UNDEFINED && path.size() < max - 1) {
        node.push_back(next);
        path.push_back(edge);
        if (path.size() >= min && next == static_cast<node_type>(last)) {
          out.push_back(path);
        }
        edge = 0;
      } else {
        node.pop_back();
        if (!path.empty()) {
          edge = path.back() + 1;
          path.pop_back();
        }
      }
    }
    return out;
  }

  template <typename T, typename S>
  std::pair<std::vector<word_type>, std::vector<node_type<T>>>
  paths_in_shortlex_order(WordGraph<T> const& ad,
                          S const             root,
                          size_t              min = 0,
                          size_t max = libsemigroups::POSITIVE_INFINITY) {
    using node_type  = node_type<T>;
    using label_type = label_type<T>;

    auto out = paths_in_lex_order(ad, root, min, std::min(min + 1, max));

    for (size_t i = 0; i < out.first.size() && out.first[i].size() < max - 1;
         ++i) {
      node_type  n;
      label_type a   = 0;
      std::tie(a, n) = ad.next_label_and_target_no_checks(out.second[i], a);
      while (n != UNDEFINED) {
        word_type next(out.first[i]);
        next.push_back(a);
        out.first.push_back(std::move(next));
        out.second.push_back(n);
        std::tie(a, n) = ad.next_label_and_target_no_checks(out.second[i], ++a);
      }
    }
    return out;
  }

  WordGraph<size_t> test_digraph() {
    WordGraph<size_t> ad;
    ad.add_nodes(6);
    ad.add_to_out_degree(2);

    ad.target(0, 0, 1);
    ad.target(0, 1, 2);
    ad.target(1, 0, 3);
    ad.target(1, 1, 4);
    ad.target(2, 0, 4);
    ad.target(2, 1, 2);
    ad.target(3, 0, 1);
    ad.target(3, 1, 5);
    ad.target(4, 0, 5);
    ad.target(4, 1, 4);
    ad.target(5, 0, 4);
    ad.target(5, 1, 5);
    return ad;
  }

  template <typename Node>
  bool is_strictly_cyclic(WordGraph<Node> const& wg) {
    using node_type = typename WordGraphView<Node>::node_type;
    auto const N    = wg.number_of_nodes();

    if (N == 0) {
      return true;
    }

    std::vector<bool> seen(N, false);
    std::stack<Node>  stack;

    for (node_type m = 0; m < N; ++m) {
      stack.push(m);
      size_t count = 0;
      while (!stack.empty()) {
        auto n = stack.top();
        stack.pop();
        if (!seen[n]) {
          seen[n] = true;
          if (++count == N) {
            return true;
          }
          for (auto t : wg.targets_no_checks(n)) {
            if (t < N) {
              stack.push(t);
            }
          }
        }
      }
      std::fill(seen.begin(), seen.end(), false);
    }
    return false;
  }

  TEST_CASE("const_pilo_iterator x 1", "[quick][000]") {
    using node_type = size_t;
    auto   ad       = test_digraph();
    size_t N        = 20;

    BENCHMARK("const_pilo_iterator") {
      std::vector<word_type> v(cbegin_pilo(ad, 0, 0, N), cend_pilo(ad));
      REQUIRE(v.size() == 1'048'575);
    };

    BENCHMARK("free function for comparison with const_pilo_iterator") {
      std::pair<std::vector<word_type>, std::vector<node_type>> v
          = paths_in_lex_order(ad, 0, 0, N);
      REQUIRE(v.first.size() == 1'048'575);
    };
  }

  TEST_CASE("const_pilo_iterator x 2", "[quick][001]") {
    using node_type = size_t;
    auto   ad       = test_digraph();
    size_t N        = 20;

    BENCHMARK("const_pilo_iterator") {
      std::vector<word_type> v(cbegin_pilo(ad, 0, 0, N), cend_pilo(ad));
      REQUIRE(v.size() == 1'048'575);
    };

    BENCHMARK("free function for comparison with const_pilo_iterator") {
      std::pair<std::vector<word_type>, std::vector<node_type>> v
          = paths_in_lex_order(ad, 0, 0, N);
      REQUIRE(v.first.size() == 1'048'575);
    };
  }

  TEST_CASE("const_pstilo_iterator", "[quick][002]") {
    auto   ad = test_digraph();
    size_t N  = 20;

    BENCHMARK("const_pstilo_iterator") {
      std::vector<word_type> v(cbegin_pstilo(ad, 0, 4, 0, N), cend_pstilo(ad));
      REQUIRE(v.size() == 524'277);
    };

    BENCHMARK("free function for comparison with const_pstilo_iterator") {
      std::vector<word_type> v = paths_in_lex_order2(ad, 0, 4, 0, N);
      REQUIRE(v.size() == 524'277);
    };
  }

  TEST_CASE("number_of_paths", "[quick][003]") {
    auto ad = test_digraph();
    BENCHMARK("number_of_paths (uses pstilo)") {
      REQUIRE(number_of_paths(ad, 0, 4, 0, 24) == 8'388'595);
    };

    BENCHMARK("number of paths (via pilo)") {
      auto     first = cbegin_pilo(ad, 0, 0, 24);
      auto     last  = cend_pilo(ad);
      uint64_t count = 0;

      for (auto it = first; it != last; ++it) {
        if (it.target() == 4) {
          count++;
        }
      }

      REQUIRE(count == 8'388'595);
    };
  }

  TEST_CASE("const_pislo_iterator x 1", "[quick][004]") {
    using node_type = size_t;
    auto   ad       = test_digraph();
    size_t N        = 20;

    BENCHMARK("const_pislo_iterator") {
      std::vector<word_type> v(cbegin_pislo(ad, 0, 0, N), cend_pislo(ad));
      REQUIRE(v.size() == 1'048'575);
    };

    BENCHMARK("free function for comparison with const_pislo_iterator") {
      std::pair<std::vector<word_type>, std::vector<node_type>> v
          = paths_in_shortlex_order(ad, 0, 0, N);
      REQUIRE(v.first.size() == 1'048'575);
    };

    BENCHMARK("const_pilo_iterator for comparison with const_pislo_iterator") {
      std::vector<word_type> v(cbegin_pilo(ad, 0, 0, N), cend_pilo(ad));
      REQUIRE(v.size() == 1'048'575);
    };
  }

  TEST_CASE("const_pislo_iterator x 2", "[quick][005]") {
    using node_type = size_t;
    auto   ad       = test_digraph();
    size_t N        = 20;

    BENCHMARK("const_pislo_iterator") {
      std::vector<word_type> v(cbegin_pislo(ad, 0, 0, N), cend_pislo(ad));
      REQUIRE(v.size() == 1'048'575);
    };

    BENCHMARK("free function for comparison with const_pislo_iterator") {
      std::pair<std::vector<word_type>, std::vector<node_type>> v
          = paths_in_shortlex_order(ad, 0, 0, N);
      REQUIRE(v.first.size() == 1'048'575);
    };

    BENCHMARK("const_pilo_iterator for comparison with const_pislo_iterator") {
      std::vector<word_type> v(cbegin_pilo(ad, 0, 0, N), cend_pilo(ad));
      REQUIRE(v.size() == 1'048'575);
    };
  }

  TEST_CASE("const_pstislo_iterator", "[quick][006]") {
    auto   ad = test_digraph();
    size_t N  = 20;

    BENCHMARK("const_pstislo_iterator") {
      std::vector<word_type> v(cbegin_pstislo(ad, 0, 4, 0, N),
                               cend_pstislo(ad));
      REQUIRE(v.size() == 524'277);
    };

    BENCHMARK(
        "const_pstilo_iterator for comparison with const_pstislo_iterator") {
      std::vector<word_type> v(cbegin_pstilo(ad, 0, 4, 0, N), cend_pstilo(ad));
      REQUIRE(v.size() == 524'277);
    };
  }

  // Best with a sample size of 1
  TEST_CASE("number_of_paths matrix vs dfs", "[standard][007]") {
    using algorithm = paths::algorithm;
    std::mt19937 mt;
    for (size_t M = 100; M < 1'000; M += 100) {
      std::uniform_int_distribution<size_t> source(0, M - 1);
      for (size_t N = 10; N < 20; N += 5) {
        for (size_t nr_edges = 0; nr_edges <= detail::magic_number(M) * M;
             nr_edges += 500) {
          // TODO for v3 we remove the nr_edges parameter from
          // WordGraph::random, andso this benchmark doesn't really make sense
          // any more
          auto ad = WordGraph<size_t>::random(M, N);
          word_graph::add_cycle_no_checks(
              ad, ad.cbegin_nodes(), ad.cend_nodes());
          std::string m = std::to_string(ad.number_of_edges());
          size_t      w = source(mt);
          uint64_t    expected
              = number_of_paths(ad, w, 0, 16, algorithm::automatic);
          BENCHMARK("algorithm::matrix: " + std::to_string(M) + " nodes, "
                    + std::to_string(N) + " out-degree, " + m + " edges") {
            REQUIRE(number_of_paths(ad, w, 0, 16, algorithm::matrix)
                    == expected);
          };
          BENCHMARK("algorithm::dfs: " + std::to_string(M) + " nodes, "
                    + std::to_string(N) + " out-degree, " + m + " edges") {
            REQUIRE(number_of_paths(ad, w, 0, 16, algorithm::dfs) == expected);
          };
          BENCHMARK("algorithm::automatic: " + std::to_string(M) + " nodes, "
                    + std::to_string(N) + " out-degree, " + m + " edges") {
            REQUIRE(number_of_paths(ad, w, 0, 16, algorithm::automatic)
                    == expected);
          };
          std::cout << std::endl << std::string(72, '#') << std::endl;
        }
      }
    }
  }

  TEST_CASE(
      "Is strictly cyclic on 200,000 node random graph with out_degree 100"
      "comparing WordGraphView to WordGraph",
      "[standard][008]") {
    std::mt19937      mt;
    WordGraph<size_t> wg     = WordGraph<size_t>::random(200'000, 100, mt);
    bool              cyclic = is_strictly_cyclic(wg);
    BENCHMARK("is_strictly_cyclic without WordGraphView wrapper") {
      REQUIRE(is_strictly_cyclic(wg) == cyclic);
    };
    BENCHMARK("is_strictly_cyclic using WordGraphView") {
      REQUIRE(word_graph::is_strictly_cyclic(wg) == cyclic);
    };
  }
}  // namespace libsemigroups
