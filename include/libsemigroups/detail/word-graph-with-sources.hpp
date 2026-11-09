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
      ////////////////////////////////////////////////////////////////////////
      // Constructors + initializers
      ////////////////////////////////////////////////////////////////////////

      explicit WordGraphWithSources(size_type num_nodes = 0,
                                    size_type degree    = 0)
          : WordGraph<node_type>(num_nodes, degree),
            _preim_init(degree, num_nodes, UNDEFINED),
            _preim_next(degree, num_nodes, UNDEFINED) {}

      WordGraphWithSources& init(size_type num_nodes = 0,
                                 size_type degree    = 0) {
        WordGraph<node_type>::init(num_nodes, degree);
        _preim_init.reshape(degree, num_nodes);
        _preim_next.reshape(degree, num_nodes);
        return *this;
      }

      explicit WordGraphWithSources(WordGraph<Node> const& that);
      void init(WordGraph<Node> const& that);

      explicit WordGraphWithSources(WordGraph<Node>&& that);
      void init(WordGraph<Node>&& that);

      WordGraphWithSources(WordGraphWithSources&&)                 = default;
      WordGraphWithSources(WordGraphWithSources const&)            = default;
      WordGraphWithSources& operator=(WordGraphWithSources const&) = default;
      WordGraphWithSources& operator=(WordGraphWithSources&&)      = default;

      ~WordGraphWithSources();

      ////////////////////////////////////////////////////////////////////////
      // WordGraph mem fns
      ////////////////////////////////////////////////////////////////////////

      using WordGraph<Node>::out_degree;
      using WordGraph<Node>::number_of_nodes;
      using WordGraph<Node>::target;  // for the 2-arg version only see below
      using WordGraph<Node>::target_no_checks;  // for the 2-arg version only
                                                // see below

      // We keep the following mem fns of WordGraph hidden, since calling
      // them on a WordGraphWithSources <wgws> will render <wgws> invalid.
      WordGraph<Node>& disjoint_union_inplace_no_checks(WordGraph<Node> const&)
          = delete;
      WordGraph<Node>& disjoint_union_inplace(WordGraph<Node> const&) = delete;
      WordGraph<Node>& induced_subgraph(node_type, node_type)         = delete;
      WordGraph<Node>& remove_all_targets()                           = delete;
      WordGraph<Node>& remove_target(node_type, label_type)           = delete;
      WordGraph<Node>& remove_label_no_checks(label_type)             = delete;
      WordGraph<Node>& remove_label(label_type)                       = delete;
      WordGraph<Node>& reserve(size_type, size_type)                  = delete;
      WordGraph<Node>& standardize_no_checks()                        = delete;
      WordGraph<Node>& swap_targets_no_checks()                       = delete;
      WordGraph<Node>& swap_targets()                                 = delete;

      ////////////////////////////////////////////////////////////////////////
      // WordGraph mem requiring a specialization
      ////////////////////////////////////////////////////////////////////////

      void target_no_checks(node_type s, label_type a, node_type t) noexcept {
        LIBSEMIGROUPS_ASSERT(s < number_of_nodes());
        LIBSEMIGROUPS_ASSERT(a < out_degree());
        LIBSEMIGROUPS_ASSERT(t < number_of_nodes());
        WordGraph<node_type>::target_no_checks(s, a, t);
        add_source_no_checks(t, a, s);
      }

      void target(node_type s, label_type a, node_type t) {
        WordGraph<node_type>::target(s, a, t);
        add_source_no_checks(t, a, s);
      }

      void remove_target_no_checks(node_type s, label_type a) noexcept {
        LIBSEMIGROUPS_ASSERT(s < number_of_nodes());
        LIBSEMIGROUPS_ASSERT(a < out_degree());
        remove_source_no_checks(WordGraph<Node>::target_no_checks(s, a), a, s);
        WordGraph<node_type>::remove_target_no_checks(s, a);
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

      void induced_subgraph_no_checks(node_type first, node_type last);

      // The permutation q must map the valid nodes to the [0, .. , n), where
      // n is the number of valid nodes, and p = q ^ -1. The vector q is not
      // const because it is modified in-place.
      void permute_nodes_no_checks(std::vector<node_type> const& p,
                                   std::vector<node_type>&       q,
                                   size_t                        n);

      ////////////////////////////////////////////////////////////////////////
      // WordGraphWithSources specific mem fns
      ////////////////////////////////////////////////////////////////////////

      inline node_type first_source_no_checks(node_type   s,
                                              letter_type a) const noexcept {
        LIBSEMIGROUPS_ASSERT(s < number_of_nodes());
        LIBSEMIGROUPS_ASSERT(a < out_degree());
        LIBSEMIGROUPS_ASSERT(s < _preim_init.number_of_rows());
        LIBSEMIGROUPS_ASSERT(a < _preim_init.number_of_cols());
        return _preim_init.get(s, a);
      }

      inline node_type next_source_no_checks(node_type   s,
                                             letter_type a) const noexcept {
        LIBSEMIGROUPS_ASSERT(s < number_of_nodes());
        LIBSEMIGROUPS_ASSERT(a < out_degree());
        LIBSEMIGROUPS_ASSERT(s < _preim_next.number_of_rows());
        LIBSEMIGROUPS_ASSERT(a < _preim_next.number_of_cols());
        return _preim_next.get(s, a);
      }

      // Returns the number of edges removed
      template <typename NewEdgeFunc, typename IncompatibleFunc>
      uint64_t merge_nodes_no_checks(node_type          min,
                                     node_type          max,
                                     NewEdgeFunc&&      new_edge,
                                     IncompatibleFunc&& incompat);

      void remove_all_sources_and_targets_no_checks(node_type n);
      void remove_all_sources_no_checks(node_type n);

      void add_source_no_checks(node_type  t,
                                label_type a,
                                node_type  s) noexcept;

     private:
      void populate_sources_no_checks();
      void remove_source_no_checks(node_type t, label_type a, node_type s);
    };
  }  // namespace detail
}  // namespace libsemigroups

#include "word-graph-with-sources.tpp"

#endif  // LIBSEMIGROUPS_DETAIL_WORD_GRAPH_WITH_SOURCES_HPP_
