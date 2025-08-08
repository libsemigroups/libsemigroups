//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2022-2025 James D. Mitchell
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

// This file contains a declaration of a class for WordGraphs with
// additional information about the edges leading into every node (not only
// those leaving every node).
//
// In the comments in this file we refer to "valid nodes", this means nodes in
// the graph where the values returned by first_source_no_checks and
// next_source_no_checks are valid (i.e. correspond to edges in the underlying
// WordGraph that point into the current node). Validity of nodes is not tracked
// by WordGraphWithSources, and it is the responsibility of the caller to ensure
// that nodes are valid where required by the various member functions of
// WordGraphWithSources.

#ifndef LIBSEMIGROUPS_DETAIL_WORD_GRAPH_WITH_SOURCES_HPP_
#define LIBSEMIGROUPS_DETAIL_WORD_GRAPH_WITH_SOURCES_HPP_

// TODO:
// * test file
// * doc
// * add _no_checks where appropriate

#include <cstddef>  // for size_t
#include <stack>    // for stack
#include <utility>  // for pair
#include <vector>   // for vector

#include "libsemigroups/config.hpp"      // for LIBSEMIGROUPS_DEBUG
#include "libsemigroups/constants.hpp"   // for UNDEFINED
#include "libsemigroups/types.hpp"       // for letter_type
#include "libsemigroups/word-graph.hpp"  // for WordGraph

#include "containers.hpp"  // for DynamicArray2

namespace libsemigroups {

  namespace detail {
    template <typename Node>
    class WordGraphWithSources : public WordGraph<Node> {
     public:
      using node_type  = Node;
      using label_type = typename WordGraph<Node>::label_type;
      using size_type  = typename WordGraph<Node>::size_type;

     private:
      detail::DynamicArray2<node_type> _preim_init;
      detail::DynamicArray2<node_type> _preim_next;

     public:
      // So that we can use out_degree and number_of_nodes in assertions
      using WordGraph<Node>::out_degree;
      using WordGraph<Node>::number_of_nodes;

      explicit WordGraphWithSources(size_type m = 0, size_type n = 0)
          : WordGraph<node_type>(m, n),
            _preim_init(n, m, UNDEFINED),
            _preim_next(n, m, UNDEFINED) {}

      void init(size_type m = 0, size_type n = 0);

      template <typename ThatNode>
      explicit WordGraphWithSources(WordGraph<ThatNode> const& that);

      template <typename ThatNode>
      void init(WordGraph<ThatNode> const& that);

      template <typename ThatNode>
      explicit WordGraphWithSources(WordGraph<ThatNode>&& that);

      template <typename ThatNode>
      void init(WordGraph<ThatNode>&& that);

      WordGraphWithSources(WordGraphWithSources&&)                 = default;
      WordGraphWithSources(WordGraphWithSources const&)            = default;
      WordGraphWithSources& operator=(WordGraphWithSources const&) = default;
      WordGraphWithSources& operator=(WordGraphWithSources&&)      = default;

      ~WordGraphWithSources();

      // the template is for uniformity of interface with FelschGraph
      template <bool = true>
      void target_no_checks(node_type c, label_type x, node_type d) noexcept {
        LIBSEMIGROUPS_ASSERT(c < number_of_nodes());
        LIBSEMIGROUPS_ASSERT(x < out_degree());
        LIBSEMIGROUPS_ASSERT(d < number_of_nodes());
        WordGraph<node_type>::target_no_checks(c, x, d);
        add_source_no_checks(d, x, c);
      }

      // Can't use using WordGraph<node_type>::target here because it doesn't
      // work (compiles but run time error, probably confusing the function
      // above with the WordGraph<node_type> function of the same name
      [[nodiscard]] node_type target(node_type v, label_type lbl) const {
        return WordGraph<node_type>::target(v, lbl);
      }

      [[nodiscard]] node_type target_no_checks(node_type  v,
                                               label_type lbl) const {
        return WordGraph<node_type>::target_no_checks(v, lbl);
      }

      void remove_target_no_checks(node_type c, label_type x) noexcept {
        LIBSEMIGROUPS_ASSERT(c < number_of_nodes());
        LIBSEMIGROUPS_ASSERT(x < out_degree());
        remove_source_no_checks(WordGraph<Node>::target_no_checks(c, x), x, c);
        WordGraph<node_type>::remove_target_no_checks(c, x);
      }

      void add_nodes(size_type m) {
        WordGraph<node_type>::add_nodes(m);
        _preim_init.add_rows(m);
        _preim_next.add_rows(m);
      }

      void add_to_out_degree(size_type m) {
        _preim_init.add_cols(m);
        _preim_next.add_cols(m);
        WordGraph<node_type>::add_to_out_degree(m);
      }

      inline node_type first_source_no_checks(node_type   c,
                                              letter_type x) const noexcept {
        LIBSEMIGROUPS_ASSERT(c < number_of_nodes());
        LIBSEMIGROUPS_ASSERT(x < out_degree());
        LIBSEMIGROUPS_ASSERT(c < _preim_init.number_of_rows());
        LIBSEMIGROUPS_ASSERT(x < _preim_init.number_of_cols());
        return _preim_init.get(c, x);
      }

      inline node_type next_source_no_checks(node_type   c,
                                             letter_type x) const noexcept {
        LIBSEMIGROUPS_ASSERT(c < number_of_nodes());
        LIBSEMIGROUPS_ASSERT(x < out_degree());
        LIBSEMIGROUPS_ASSERT(c < _preim_next.number_of_rows());
        LIBSEMIGROUPS_ASSERT(x < _preim_next.number_of_cols());
        return _preim_next.get(c, x);
      }

      void induced_subgraph_no_checks(node_type first, node_type last);

      // The permutation q must map the valid nodes to the [0, .. , n), where
      // n is the number of valid nodes, and p = q ^ -1.
      void permute_nodes_no_checks(std::vector<node_type> const& p,
                                   std::vector<node_type> const& q,
                                   size_t                        n);

      // Swaps valid nodes c and d, if c or d is not valid, then this will
      // fail spectacularly (no checks are performed)
      void swap_nodes_no_checks(node_type c, node_type d);

      // Rename c to d, i.e. node d has the exact same in- and out-neighbours
      // as c after this is called. Assumed that c is valid when this function
      // is called, and that d is valid after it is called. This is a
      // one-sided version of swap nodes.
      void rename_node_no_checks(node_type c, node_type d);

      // Returns the number of edges removed
      template <typename NewEdgeFunc, typename IncompatibleFunc>
      uint64_t merge_nodes_no_checks(node_type          min,
                                     node_type          max,
                                     NewEdgeFunc&&      new_edge,
                                     IncompatibleFunc&& incompat);

      // Is d a source of c under x? This is costly!
      bool is_source_no_checks(node_type c, label_type x, node_type d) const;

      void remove_all_sources_and_targets_no_checks(node_type c);
      void remove_all_sources_no_checks(node_type c);

      void add_source_no_checks(node_type  c,
                                label_type x,
                                node_type  d) noexcept;

      template <typename It>
      void rebuild_sources_no_checks(It first, It last);

      // TODO to cpp/tpp file
      void disjoint_union_inplace_no_checks(WordGraph<Node> const& that) {
        size_t N = number_of_nodes();
        add_nodes(that.number_of_nodes());
        for (auto s : that.nodes()) {
          for (auto [a, t] : that.labels_and_targets_no_checks(s)) {
            if (t != UNDEFINED) {
              target_no_checks(s + N, a, t + N);
            }
          }
        }
      }

     private:
      void remove_source_no_checks(node_type cx, label_type x, node_type d);
      void replace_target_no_checks(node_type c, label_type x, node_type d);
      void replace_source_no_checks(node_type  c,
                                    node_type  d,
                                    label_type x,
                                    node_type  cx);
    };
  }  // namespace detail
}  // namespace libsemigroups

#include "word-graph-with-sources.tpp"

#endif  // LIBSEMIGROUPS_DETAIL_WORD_GRAPH_WITH_SOURCES_HPP_
