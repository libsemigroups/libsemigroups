//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2020 James D. Mitchell
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

#include "bench-main.hpp"  // for CATCH_CONFIG_ENABLE_BENCHMARKING
#include "catch.hpp"       // for BENCHMARK, REQUIRE, TEST_CASE

#include "libsemigroups/digraph.hpp"  // for ActionDigraph
#include "libsemigroups/types.hpp"    // for word_type

namespace libsemigroups {
  // Old function for comparison with iterators
  template <typename T>
  using node_type = typename ActionDigraph<T>::node_type;

  template <typename T>
  using label_type = typename ActionDigraph<T>::label_type;

  template <typename T, typename S>
  std::pair<std::vector<word_type>, std::vector<node_type<T>>>
  paths_in_lex_order(ActionDigraph<T> const& ad,
                     S const                 root,
                     size_t                  min = 0,
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
      std::tie(next, edge) = ad.unsafe_next_neighbor(node.back(), edge);
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
  paths_in_lex_order2(ActionDigraph<T> const& ad,
                      S const                 first,
                      S const                 last,
                      size_t                  min = 0,
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
      std::tie(next, edge) = ad.unsafe_next_neighbor(node.back(), edge);
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
  paths_in_shortlex_order(ActionDigraph<T> const& ad,
                          S const                 root,
                          size_t                  min = 0,
                          size_t max = libsemigroups::POSITIVE_INFINITY) {
    using node_type  = node_type<T>;
    using label_type = label_type<T>;

    auto out = paths_in_lex_order(ad, root, min, std::min(min + 1, max));

    for (size_t i = 0; i < out.first.size() && out.first[i].size() < max - 1;
         ++i) {
      node_type  n;
      label_type a   = 0;
      std::tie(n, a) = ad.unsafe_next_neighbor(out.second[i], a);
      while (n != UNDEFINED) {
        word_type next(out.first[i]);
        next.push_back(a);
        out.first.push_back(std::move(next));
        out.second.push_back(n);
        std::tie(n, a) = ad.unsafe_next_neighbor(out.second[i], ++a);
      }
    }
    return out;
  }

  ActionDigraph<size_t> test_digraph() {
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
    return ad;
  }

  TEST_CASE("const_panilo_iterator", "[quick][000]") {
    using node_type = size_t;
    auto   ad       = test_digraph();
    size_t N        = 20;

    BENCHMARK("const_panilo_iterator") {
      std::vector<std::pair<word_type, node_type>> v(ad.cbegin_panilo(0, 0, N),
                                                     ad.cend_panilo());
      REQUIRE(v.size() == 1048575);
    };

    BENCHMARK("free function for comparison with const_panilo_iterator") {
      std::pair<std::vector<word_type>, std::vector<node_type>> v
          = paths_in_lex_order(ad, 0, 0, N);
      REQUIRE(v.first.size() == 1048575);
    };
  }

  TEST_CASE("const_pilo_iterator", "[quick][001]") {
    using node_type = size_t;
    auto   ad       = test_digraph();
    size_t N        = 20;

    BENCHMARK("const_pilo_iterator") {
      std::vector<word_type> v(ad.cbegin_pilo(0, 0, N), ad.cend_pilo());
      REQUIRE(v.size() == 1048575);
    };

    BENCHMARK("free function for comparison with const_pilo_iterator") {
      std::pair<std::vector<word_type>, std::vector<node_type>> v
          = paths_in_lex_order(ad, 0, 0, N);
      REQUIRE(v.first.size() == 1048575);
    };
  }

  TEST_CASE("const_pstilo_iterator", "[quick][002]") {
    auto   ad = test_digraph();
    size_t N  = 20;

    BENCHMARK("const_pstilo_iterator") {
      std::vector<word_type> v(ad.cbegin_pstilo(0, 4, 0, N), ad.cend_pstilo());
      REQUIRE(v.size() == 524277);
    };

    BENCHMARK("free function for comparison with const_pstilo_iterator") {
      std::vector<word_type> v = paths_in_lex_order2(ad, 0, 4, 0, N);
      REQUIRE(v.size() == 524277);
    };
  }

  TEST_CASE("number_of_paths", "[quick][003]") {
    using node_type = size_t;
    auto ad         = test_digraph();
    BENCHMARK("number_of_paths (uses pstilo)") {
      REQUIRE(ad.number_of_paths(0, 4, 0, 24) == 8388595);
    };

    BENCHMARK("number of paths (via panilo)") {
      REQUIRE(std::count_if(ad.cbegin_panilo(0, 0, 24),
                            ad.cend_panilo(),
                            [](std::pair<word_type, node_type> const& p) {
                              return p.second == 4;
                            })
              == 8388595);
    };
  }

  TEST_CASE("const_panislo_iterator", "[quick][004]") {
    using node_type = size_t;
    auto   ad       = test_digraph();
    size_t N        = 20;

    BENCHMARK("const_panislo_iterator") {
      std::vector<std::pair<word_type, node_type>> v(ad.cbegin_panislo(0, 0, N),
                                                     ad.cend_panislo());
      REQUIRE(v.size() == 1048575);
    };

    BENCHMARK("free function for comparison with const_panislo_iterator") {
      std::pair<std::vector<word_type>, std::vector<node_type>> v
          = paths_in_shortlex_order(ad, 0, 0, N);
      REQUIRE(v.first.size() == 1048575);
    };

    BENCHMARK(
        "const_panilo_iterator for comparison with const_panislo_iterator") {
      std::vector<std::pair<word_type, node_type>> v(ad.cbegin_panilo(0, 0, N),
                                                     ad.cend_panilo());
      REQUIRE(v.size() == 1048575);
    };
  }

  TEST_CASE("const_pislo_iterator", "[quick][005]") {
    using node_type = size_t;
    auto   ad       = test_digraph();
    size_t N        = 20;

    BENCHMARK("const_pislo_iterator") {
      std::vector<word_type> v(ad.cbegin_pislo(0, 0, N), ad.cend_pislo());
      REQUIRE(v.size() == 1048575);
    };

    BENCHMARK("free function for comparison with const_pislo_iterator") {
      std::pair<std::vector<word_type>, std::vector<node_type>> v
          = paths_in_shortlex_order(ad, 0, 0, N);
      REQUIRE(v.first.size() == 1048575);
    };

    BENCHMARK("const_pilo_iterator for comparison with const_pislo_iterator") {
      std::vector<word_type> v(ad.cbegin_pilo(0, 0, N), ad.cend_pilo());
      REQUIRE(v.size() == 1048575);
    };
  }

  TEST_CASE("const_pstislo_iterator", "[quick][006]") {
    auto   ad = test_digraph();
    size_t N  = 20;

    BENCHMARK("const_pstislo_iterator") {
      std::vector<word_type> v(ad.cbegin_pstislo(0, 4, 0, N),
                               ad.cend_pstislo());
      REQUIRE(v.size() == 524277);
    };

    BENCHMARK(
        "const_pstilo_iterator for comparison with const_pstislo_iterator") {
      std::vector<word_type> v(ad.cbegin_pstilo(0, 4, 0, N), ad.cend_pstilo());
      REQUIRE(v.size() == 524277);
    };
  }

  // Best with a sample size of 1
  TEST_CASE("number_of_paths matrix vs dfs", "[standard][007]") {
    using algorithm = ActionDigraph<size_t>::algorithm;
    std::mt19937 mt;
    for (size_t M = 100; M < 1000; M += 100) {
      std::uniform_int_distribution<size_t> source(0, M - 1);
      for (size_t N = 10; N < 20; N += 5) {
        for (size_t nr_edges = 0; nr_edges <= detail::magic_number(M) * M;
             nr_edges += 500) {
          auto ad = ActionDigraph<size_t>::random(M, N, nr_edges);
          action_digraph_helper::add_cycle(
              ad, ad.cbegin_nodes(), ad.cend_nodes());
          std::string m = std::to_string(ad.number_of_edges());
          size_t      w = source(mt);
          uint64_t    expected
              = ad.number_of_paths(w, 0, 16, algorithm::automatic);
          BENCHMARK("algorithm::matrix: " + std::to_string(M) + " nodes, "
                    + std::to_string(N) + " out-degree, " + m + " edges") {
            REQUIRE(ad.number_of_paths(w, 0, 16, algorithm::matrix)
                    == expected);
          };
          BENCHMARK("algorithm::dfs: " + std::to_string(M) + " nodes, "
                    + std::to_string(N) + " out-degree, " + m + " edges") {
            REQUIRE(ad.number_of_paths(w, 0, 16, algorithm::dfs) == expected);
          };
          BENCHMARK("algorithm::automatic: " + std::to_string(M) + " nodes, "
                    + std::to_string(N) + " out-degree, " + m + " edges") {
            REQUIRE(ad.number_of_paths(w, 0, 16, algorithm::automatic)
                    == expected);
          };
          std::cout << std::endl << std::string(72, '#') << std::endl;
        }
      }
    }
  }
}  // namespace libsemigroups
