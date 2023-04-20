//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2023 James D. Mitchell
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

// This file contains an implementation of Gabow's algorithm for WordGraphs.

#ifndef LIBSEMIGROUPS_TESTS_WORD_GRAPH_HPP_
#define LIBSEMIGROUPS_TESTS_WORD_GRAPH_HPP_

// TODO:
// * iwyu

#include "libsemigroups/word-graph.hpp"  // for WordGraph

namespace libsemigroups {

  void add_clique(WordGraph<size_t>& digraph, size_t n) {
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

  WordGraph<size_t> clique(size_t n) {
    WordGraph<size_t> g(0, n);
    add_clique(g, n);
    return g;
  }

  WordGraph<size_t> binary_tree(size_t number_of_levels) {
    WordGraph<size_t> ad;
    ad.add_nodes(std::pow(2, number_of_levels) - 1);
    ad.add_to_out_degree(2);
    ad.add_edge(0, 1, 0);
    ad.add_edge(0, 2, 1);

    for (size_t i = 2; i <= number_of_levels; ++i) {
      size_t counter = std::pow(2, i - 1) - 1;
      for (size_t j = std::pow(2, i - 2) - 1; j < std::pow(2, i - 1) - 1; ++j) {
        ad.add_edge(j, counter++, 0);
        ad.add_edge(j, counter++, 1);
      }
    }
    return ad;
  }

}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_TESTS_WORD_GRAPH_HPP_
