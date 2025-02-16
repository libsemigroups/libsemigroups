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

// This file contains an implementation of word graph views, a thin layer over
// word graphs exposing ranges which avoid the OBO errors

#ifndef LIBSEMIGROUPS_WORD_GRAPH_VIEW_HPP_
#define LIBSEMIGROUPS_WORD_GRAPH_VIEW_HPP_

#include <algorithm>  // for uniform_int_distribution
#include <cstddef>    // for size_t
#include <cstdint>
#include <ostream>        // for operator<<
#include <queue>          // for queue
#include <random>         // for mt19937
#include <stack>          // for stack
#include <string>         // for to_string
#include <tuple>          // for tie
#include <type_traits>    // for is_integral, is_unsigned
#include <unordered_set>  // for unordered_set
#include <utility>        // for pair
#include <vector>         // for vector

#include "config.hpp"      // for LIBSEMIGROUPS_EIGEN_EN...
#include "constants.hpp"   // for UNDEFINED
#include "debug.hpp"       // for LIBSEMIGROUPS_ASSERT
#include "dot.hpp"         // for Dot
#include "exception.hpp"   // for LIBSEMIGROUPS_EXCEPTION
#include "forest.hpp"      // for Forest
#include "order.hpp"       // for Order
#include "ranges.hpp"      // for ??
#include "types.hpp"       // for word_type, enable_if_is_same
#include "word-graph.hpp"  // for word_graph pointer

#include "detail/containers.hpp"  // for DynamicArray2
#include "detail/int-range.hpp"   // for IntRange
#include "detail/stl.hpp"         // for IsIterator
#include "detail/uf.hpp"          // for Duf

#ifdef LIBSEMIGROUPS_EIGEN_ENABLED
#include "detail/eigen.hpp"
#else
#include "matrix.hpp"
#endif

namespace libsemigroups {
  template <typename Node>
  class WordGraphView {
   public:
    using node_type  = Node;
    using label_type = Node;
    using size_type  = std::size_t;

    //! The type of an iterator pointing to the nodes of the word graph view.
    using const_iterator_nodes =
        typename detail::IntRange<Node>::const_iterator;

    //! The type of a reverse iterator pointing to the nodes of a word graph
    //! view.
    using const_reverse_iterator_nodes =
        typename detail::IntRange<Node>::const_reverse_iterator;

    //! The type of an iterator pointing to the targets of a node.
    using const_iterator_targets =
        typename detail::DynamicArray2<Node>::const_iterator;

    const WordGraph<Node>* graph;

    //! \brief Construct from an existing WordGraphView
    //! \param graph underlying WordGraphView object
    WordGraphView(const WordGraph<Node>* graph,
                  const size_type        start,
                  const size_type        end);

    //! \brief Construct empty object for future assignment
    WordGraphView() : _start(0), _end(0), _degree(0), _nr_nodes(0) {}

    //! \brief Reshape this view over the same graph
    WordGraphView& init(const size_type start, const size_type end);

    // TODO check if this destructor should destruct the underlying graph as
    // well
    ~WordGraphView() = default;

    [[nodiscard]] size_type inline number_of_nodes() const noexcept {
      return _nr_nodes;
    }

    [[nodiscard]] size_type out_degree() const noexcept;

    [[nodiscard]] size_type start_node() const noexcept {
      return _start;
    }

    [[nodiscard]] size_type end_node() const noexcept {
      return _end;
    }

    const_iterator_nodes cbegin_nodes() const noexcept {
      return detail::IntRange<node_type>(_start, _end).cbegin();
    }

    [[nodiscard]] const_iterator_nodes cend_nodes() const noexcept {
      return detail::IntRange<node_type>(_start, _end).cend();
    }

    [[nodiscard]] const_iterator_targets cbegin_targets(node_type source) const;

    [[nodiscard]] const_iterator_targets
    cbegin_targets_no_checks(node_type source) const noexcept {
      return graph->_dynamic_array_2.cbegin_row(source);
    }

    [[nodiscard]] const_iterator_targets cend_targets(node_type source) const;

    [[nodiscard]] const_iterator_targets
    cend_targets_no_checks(node_type source) const noexcept {
      return graph->_dynamic_array_2.cbegin_row(source) + _degree;
    }

    [[nodiscard]] auto nodes() const noexcept {
      return rx::seq<node_type>() | rx::take(number_of_nodes());
    }

    [[nodiscard]] auto labels() const noexcept {
      return rx::seq<label_type>() | rx::take(out_degree());
    }

    [[nodiscard]] rx::iterator_range<const_iterator_targets>
    targets_no_checks(node_type source) const noexcept {
      return rx::iterator_range(cbegin_targets_no_checks(source),
                                cend_targets_no_checks(source));
    }

    [[nodiscard]] rx::iterator_range<const_iterator_targets>
    targets(node_type source) const;

    [[nodiscard]] auto
    labels_and_targets_no_checks(node_type source) const noexcept {
      return rx::enumerate(targets_no_checks(source));
    }

    [[nodiscard]] auto labels_and_targets(node_type source) const;

   private:
    size_type _start;
    size_type _end;
    size_type _degree;
    size_type _nr_nodes;
  };

  namespace word_graph_view {
    template <typename Node1, typename Node2>
    void throw_if_node_out_of_bounds(WordGraphView<Node1> const& wg, Node2 n);

    template <typename Node>
    auto adjacency_matrix(const WordGraphView<Node>& vgw);
  }  // namespace word_graph_view
}  // namespace libsemigroups

#include "word-graph-view.tpp"

#endif