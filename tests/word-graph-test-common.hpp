//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2023-2025 James D. Mitchell
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

// This file contains some common functionality for tests involving WordGraphs.

#ifndef LIBSEMIGROUPS_TESTS_WORD_GRAPH_TEST_COMMON_HPP_
#define LIBSEMIGROUPS_TESTS_WORD_GRAPH_TEST_COMMON_HPP_

#include <cmath>      // for pow
#include <cstddef>    // for size_t
#include <stdexcept>  // for runtime_error

#include "libsemigroups/word-graph-helpers.hpp"  // for WordGraph helpers
#include "libsemigroups/word-graph.hpp"          // for WordGraph

namespace libsemigroups {

  template <typename Node = size_t>
  void add_clique(WordGraph<Node>& wg, size_t n) {
    if (n != wg.out_degree()) {
      throw std::runtime_error("can't do it!");
    }
    size_t old_nodes = wg.number_of_nodes();
    wg.add_nodes(n);

    for (Node i = old_nodes; i < wg.number_of_nodes(); ++i) {
      for (Node j = old_nodes; j < wg.number_of_nodes(); ++j) {
        wg.target(i, j - old_nodes, j);
      }
    }
  }

  template <typename Node = size_t>
  WordGraph<Node> clique(size_t n) {
    WordGraph<Node> g(0, n);
    add_clique(g, n);
    return g;
  }

  template <typename Node = size_t>
  WordGraph<Node> binary_tree(size_t number_of_levels) {
    WordGraph<Node> ad;
    ad.add_nodes(std::pow(2, number_of_levels) - 1);
    ad.add_to_out_degree(2);
    ad.target(0, 0, 1);
    ad.target(0, 1, 2);

    for (size_t i = 2; i <= number_of_levels; ++i) {
      Node counter = std::pow(2, i - 1) - 1;
      for (Node j = std::pow(2, i - 2) - 1; j < std::pow(2, i - 1) - 1; ++j) {
        ad.target(j, 0, counter++);
        ad.target(j, 1, counter++);
      }
    }
    return ad;
  }

  template <typename Node = size_t>
  void add_chain(WordGraph<Node>& wg, size_t n) {
    size_t old_nodes = wg.number_of_nodes();
    wg.add_nodes(n);
    for (size_t i = old_nodes; i < wg.number_of_nodes() - 1; ++i) {
      wg.target(i, 0, i + 1);
    }
  }

  template <typename Node = size_t>
  WordGraph<Node> chain(size_t n) {
    WordGraph<Node> g(0, 1);
    add_chain(g, n);
    return g;
  }

}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_TESTS_WORD_GRAPH_TEST_COMMON_HPP_
