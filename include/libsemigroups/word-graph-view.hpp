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
// word graphs exposing a chosen range of nodes

#ifndef LIBSEMIGROUPS_WORD_GRAPH_VIEW_HPP_
#define LIBSEMIGROUPS_WORD_GRAPH_VIEW_HPP_

#include <type_traits>  // for is_integral
#include <utility>      // for pair

#include "exception.hpp"   // for LIBSEMIGROUPS_EXCEPTION
#include "ranges.hpp"      // for rx::ranges without compile warnings
#include "word-graph.hpp"  // for word_graph pointer

#include "detail/int-range.hpp"  // for IntRange

namespace libsemigroups {
  template <typename Node>
  class WordGraphView {
   public:
    using node_type  = Node;
    using label_type = Node;
    using size_type  = std::size_t;

    static_assert(std::is_integral<Node>(),
                  "the template parameter Node must be an integral type!");
    static_assert(
        std::is_unsigned<Node>(),
        "the template parameter Node must be an unsigned integral type!");

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

    //! \brief Construct from an existing WordGraphView
    //! \param graph underlying WordGraphView object
    WordGraphView(const WordGraph<Node>* graph,
                  const size_type        start,
                  const size_type        end);

    //! \brief Copy constructor
    WordGraphView(WordGraphView const&);

    //! \brief Construct from WordGraph with another node type.
    //!
    //! This function can be used to construct a WordGraph<Node> as a copy of a
    //! WordGraph<OtherNode> so long as `sizeof(OtherNode) <= sizeof(Node)`.
    //!
    //! \param that the word graph to copy.
    //!
    //! \note Any edge with target \ref UNDEFINED in \p that will have target
    //! `static_cast<Node>(UNDEFINED)` in the constructed word graph.
    template <typename OtherNode>
    WordGraphView(WordGraphView<OtherNode> const& that);

    //! \brief Construct empty object for future assignment
    WordGraphView() : _start(0), _end(0), _nr_nodes(0) {}

    //! \brief Reshape this view over the same graph
    WordGraphView& init(const size_type start, const size_type end);

    //! \brief Reassign this view to a different graph, keeping the same
    //! dimensions
    template <typename OtherNode>
    WordGraphView<OtherNode>& init(WordGraph<OtherNode> const& that);

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
      return graph->_dynamic_array_2.cbegin_row(source) + graph->out_degree();
    }

    [[nodiscard]] auto nodes() const noexcept {
      return rx::iterator_range(cbegin_nodes(), cend_nodes());
    }

    [[nodiscard]] auto labels() const noexcept {
      return rx::seq<label_type>() | rx::take(out_degree());
    }

    [[nodiscard]] rx::iterator_range<const_iterator_targets>
    targets_no_checks(node_type source) const noexcept {
      return rx::iterator_range(cbegin_targets_no_checks(source),
                                cend_targets_no_checks(source));
    }

    [[nodiscard]] std::pair<label_type, node_type>
    next_label_and_target_no_checks(node_type s, label_type a) const;

    [[nodiscard]] std::pair<label_type, node_type>
    next_label_and_target(node_type s, label_type a) const;

    [[nodiscard]] rx::iterator_range<const_iterator_targets>
    targets(node_type source) const;

    [[nodiscard]] auto
    labels_and_targets_no_checks(node_type source) const noexcept {
      return rx::enumerate(targets_no_checks(source));
    }

    [[nodiscard]] auto labels_and_targets(node_type source) const;

    [[nodiscard]] bool operator==(WordGraphView const& that) const;

    [[nodiscard]] bool operator!=(WordGraphView const& that) const {
      return !operator==(that);
    }

    //! \brief Get the target of the edge with given source node and label.
    //!
    //! This function returns the target of the edge with source node \p source
    //! and label \p a.
    //!
    //! \param source the node.
    //! \param a the label.
    //!
    //! \returns
    //! Returns the node adjacent to \p source via the edge labelled \p a, or
    //! \ref UNDEFINED; both are values of type \ref node_type.
    //!
    //! \complexity
    //! Constant.
    //!
    //! \throws LibsemigroupsException if \p source or \p a is not
    //! valid.
    // Not noexcept because throw_if_node_out_of_bounds/label aren't
    [[nodiscard]] node_type target(node_type source, label_type a) const;

    //! \brief Get the target of the edge with given source node and label.
    //!
    //! This function returns the target of the edge with source node \p source
    //! and label \p a.
    //!
    //! \param source the node.
    //! \param a the label.
    //!
    //! \returns
    //! Returns the node adjacent to \p source via the edge labelled \p a, or
    //! \ref UNDEFINED; both are values of type \ref node_type.
    //!
    //! \complexity
    //! Constant.
    //!
    //! \throws LibsemigroupsException if \p source or \p a is not
    //! valid.
    // Not noexcept because throw_if_node_out_of_bounds/label aren't
    [[nodiscard]] node_type target_no_checks(node_type  source,
                                             label_type a) const;

    const WordGraph<Node>* graph;

   private:
    size_type _start;
    size_type _end;
    size_type _nr_nodes;
  };

  namespace word_graph_view {
    template <typename Node1, typename Node2>
    void throw_if_node_out_of_bounds(WordGraphView<Node1> const& wg, Node2 n);
  }  // namespace word_graph_view
}  // namespace libsemigroups
#include "word-graph-view.tpp"

#endif