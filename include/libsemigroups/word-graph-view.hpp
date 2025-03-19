//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2025 Nadim Searight
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

#include "detail/containers.hpp"
#include "word-graph.hpp"  // for word_graph pointer

#include "detail/int-range.hpp"  // for IntRange

namespace libsemigroups {
  template <typename Node>
  class WordGraphView {
   public:
    static_assert(std::is_integral<Node>(),
                  "the template parameter Node must be an integral type!");
    static_assert(
        std::is_unsigned<Node>(),
        "the template parameter Node must be an unsigned integral type!");

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

   private:
    WordGraph<Node> const* _graph;
    node_type              _start;
    node_type              _end;
    constexpr node_type    to_graph(node_type n) const noexcept {
      if (n == UNDEFINED) {
        // LCOV does not recognise this as being called,
        // but it is definitely called in test-word-graph-view.cpp[015]
        return UNDEFINED;
      }
      return n + _start;
    };
    constexpr node_type to_view(node_type n) const noexcept {
      if (n == UNDEFINED) {
        return UNDEFINED;
      }
      return n - _start;
    }

    constexpr auto
    to_view(rx::iterator_range<const_iterator_targets> it) const {
      return it
             | rx::transform([this](auto elem) { return this->to_view(elem); });
    }

    void to_view(std::pair<node_type, label_type>& in) const {
      // this is designed to operate on pairs of <label, node>
      // so does not modify the first element
      in.second = to_view(in.second);
    }

   public:
    //! \brief Construct from an existing WordGraphView
    //! \param graph underlying WordGraphView object
    WordGraphView(WordGraph<Node> const& graph, size_type start, size_type end);

    //! \brief Copy constructor
    WordGraphView(WordGraphView<Node> const&) = default;

    //! \brief Move constructor
    WordGraphView(WordGraphView<Node>&&) = default;

    //! \brief Construct from WordGraph with another node type.
    //!
    //! This function can be used to construct a WordGraphView<Node> as a copy
    //! of a WordGraphView<OtherNode> so long as `sizeof(OtherNode) <=
    //! sizeof(Node)`.
    //!
    //! \param that the word graph view to copy.
    //!
    //! \note Any edge with target \ref UNDEFINED in \p that will have target
    //! `static_cast<Node>(UNDEFINED)` in the constructed word graph.
    template <typename OtherNode>
    WordGraphView(WordGraphView<OtherNode> const& that);

    //! \brief Construct empty object for future assignment
    WordGraphView();

    //! \brief Reshape this view over the same graph
    WordGraphView& init(size_type start, size_type end);

    //! \brief Reassign this view to a different graph, keeping the same
    //! dimensions
    template <typename OtherNode>
    WordGraphView<OtherNode>& init(WordGraph<OtherNode> const& that);

    // TODO check if this destructor should destruct the underlying graph as
    // well
    ~WordGraphView() = default;

    //! \brief The number of nodes that this view ranges over (_end -
    //! _start)
    //! \returns
    [[nodiscard]] size_type number_of_nodes() const noexcept {
      return _end - _start;
    }

    //! \brief The start node in the underlying graph
    [[nodiscard]] node_type start_node() const noexcept {
      return _start;
    }

    //! \brief The end node in the underlying graph
    [[nodiscard]] node_type end_node() const noexcept {
      return _end;
    }

    //! \brief Returns the out degree
    //!
    //! This function returns the number of edge labels in the word graph
    //!
    //! \returns The number of edge labels, type \c size_type
    [[nodiscard]] size_type out_degree() const noexcept;

    //! \brief Returns a random access iterator pointing at the first node of
    //! the word graph.
    //!
    //! This function returns a random access iterator pointing at the first
    //! in the range of this word graph view.
    //!
    //! \returns
    //! An \ref const_iterator_nodes.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    const_iterator_nodes cbegin_nodes() const noexcept {
      return detail::IntRange<node_type>(0, number_of_nodes()).cbegin();
    }

    //! \brief Returns a random access iterator pointing one past the last node
    //! of the range of this word graph view
    //!
    //! This function returns a random access iterator pointing one beyond the
    //! last node in the word graph.
    //!
    //! \returns
    //! An \ref const_iterator_nodes.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    [[nodiscard]] const_iterator_nodes cend_nodes() const noexcept {
      return detail::IntRange<node_type>(0, number_of_nodes()).cend();
    }

    //! \brief Returns a random access iterator pointing at the target of
    //! the edge with label \p 0 incident to a given node.
    //!
    //! This function returns a random access iterator pointing at the
    //! target of the edge with label \c 0 incident to the source node \p
    //! source. This target might equal \ref UNDEFINED.
    //!
    //! \param source the source node in the word graph.
    //!
    //! \returns
    //! A \ref const_iterator_targets.
    //!
    //! \throws LibsemigroupsException if \p source is out of range (i.e.
    //! greater than or equal to \ref number_of_nodes).
    //!
    //! \complexity
    //! Constant.
    // Not noexcept because throw_if_node_out_of_bounds isn't
    [[nodiscard]] auto cbegin_targets(node_type source) const;

    //! \brief Returns a random access iterator pointing at the target of
    //! the edge with label \p 0 incident to a given node.
    //!
    //! This function returns a random access iterator pointing at the
    //! target of the edge with label \c 0 incident to the source node \p
    //! source. This target might equal \ref UNDEFINED.
    //!
    //! \param source a node in the word graph.
    //!
    //! \returns
    //! A \ref const_iterator_targets.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    //!
    //! \warning
    //! No checks whatsoever on the validity of the arguments are performed.
    //!
    //! \sa
    //! \ref cbegin_targets.
    [[nodiscard]] auto
    cbegin_targets_no_checks(node_type source) const noexcept {
      return rx::begin(targets_no_checks(source));
    }

    //! \brief Returns a random access iterator pointing one beyond the target
    //! of the edge with label `out_degree() - 1` incident to a given node.
    //!
    //! This function returns a random access iterator pointing one beyond the
    //! target of the edge with label `out_degree() - 1` incident to the source
    //! node \p source. This target might equal \ref UNDEFINED.
    //!
    //! \param source a node in the word graph.
    //!
    //! \returns
    //! A \ref const_iterator_targets.
    //!
    //! \throws LibsemigroupsException if \p source is out of range (i.e.
    //! greater than or equal to \ref number_of_nodes).
    //!
    //! \complexity
    //! Constant.
    // Not noexcept because throw_if_node_out_of_bounds isn't
    [[nodiscard]] auto cend_targets(node_type source) const;

    //! \brief Returns a random access iterator pointing one beyond the target
    //! of the edge with label `out_degree() - 1` incident to a given node.
    //!
    //! This function returns a random access iterator pointing one beyond the
    //! target of the edge with label `out_degree() - 1` incident to the source
    //! node \p source. This target might equal \ref UNDEFINED.
    //!
    //! \param source a node in the word graph.
    //!
    //! \returns
    //! A \ref const_iterator_targets.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    //!
    //! \warning
    //! No checks whatsoever on the validity of the arguments are performed.
    //!
    //! \sa
    //! \ref cend_targets.
    [[nodiscard]] auto cend_targets_no_checks(node_type source) const noexcept {
      return rx::end(targets_no_checks(source));
    }

    //! \brief Returns a range object containing all nodes in a word graph.
    //!
    //! This function returns a range object containing all the nodes in a
    //! word graph view
    //!
    //! \returns A range object.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] auto nodes() const noexcept {
      return rx::iterator_range(cbegin_nodes(), cend_nodes());
    }

    //! \brief Returns a range object containing all labels of edges in a word
    //! graph.
    //!
    //! This function returns a range object containing all the labels of edges
    //! in a word graph.
    //!
    //! \returns A range object.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] auto labels() const noexcept {
      return _graph->labels();
    }

    //! \brief Returns a range object containing all the targets of edges with
    //! a given source.
    //!
    //! This function returns a range object containing all the targets of
    //! edges with source \p source.
    //!
    //! \param source the source node.
    //!
    //! \returns A range object.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \warning
    //! This function performs no checks whatsoever and assumes that \p source
    //! is a valid node of the word graph (i.e. it is not greater than or equal
    //! to \ref number_of_nodes).
    [[nodiscard]] auto targets_no_checks(node_type source) const noexcept {
      node_type translated = to_graph(source);
      return to_view(_graph->targets_no_checks(translated));
    }

    //! \brief Returns a range object containing pairs consisting of edge
    //! labels and target nodes.
    //!
    //! This function returns a range object containing all the edge labels and
    //! targets of edges with source \p source.
    //!
    //! \param source the source node.
    //!
    //! \returns A range object.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \warning
    //! This function performs no checks whatsoever and assumes that \p source
    //! is a valid node of the word graph (i.e. it is not greater than or equal
    //! to \ref number_of_nodes).
    [[nodiscard]] std::pair<label_type, node_type>
    next_label_and_target_no_checks(node_type s, label_type a) const;

    //! \brief Get the next target of an edge incident to a given node that
    //! doesn't equal \ref UNDEFINED.
    //!
    //! This function returns the next target of an edge with label greater
    //! than or equal to \p a that is incident to the node \p s.
    //!
    //! If `target(s, b)` equals \ref UNDEFINED for every value \c b in the
    //! range \f$[a, n)\f$, where \f$n\f$ is the return value of out_degree()
    //! then \c x.first and \c x.second equal \ref UNDEFINED.
    //!
    //! \param s the node.
    //! \param a the label.
    //!
    //! \returns
    //! Returns a std::pair
    //! \c x where:
    //! 1. \c x.first is adjacent to \p s via an edge labelled
    //!    \c x.second; and
    //! 2. \c x.second is the minimum value in the range \f$[a, n)\f$ such that
    //!    `target(s, x.second)` is not equal to \ref UNDEFINED
    //!    where \f$n\f$ is the return value of out_degree();
    //! If no such value exists, then `{UNDEFINED, UNDEFINED}` is returned.
    //!
    //! \throws LibsemigroupsException if \p s does not represent a node in \c
    //! this, or \p a is not a valid edge label.
    //!
    //! \complexity
    //! At worst \f$O(n)\f$ where \f$n\f$ equals out_degree().
    //!
    //! \sa next_label_and_target_no_checks.
    // Not noexcept because next_label_and_target_no_checks is not
    [[nodiscard]] std::pair<label_type, node_type>
    next_label_and_target(node_type s, label_type a) const;

    //! \brief Returns a range object containing all the targets of edges with
    //! a given source.
    //!
    //! This function returns a range object containing all the targets of
    //! edges with source \p source.
    //!
    //! \param source the source node.
    //!
    //! \returns A range object.
    //!
    //! \throws LibsemigroupsException if \p source is out of range (i.e. it is
    //! greater than or equal to \ref number_of_nodes)
    [[nodiscard]] auto targets(node_type source) const;

    //! \brief Returns a range object containing pairs consisting of edge
    //! labels and target nodes.
    //!
    //! This function returns a range object containing all the edge labels and
    //! targets of edges with source \p source.
    //!
    //! \param source the source node.
    //!
    //! \returns A range object.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \warning
    //! This function performs no checks whatsoever and assumes that \p source
    //! is a valid node of the word graph (i.e. it is not greater than or equal
    //! to \ref number_of_nodes).
    [[nodiscard]] auto
    labels_and_targets_no_checks(node_type source) const noexcept {
      return rx::enumerate(targets_no_checks(source));
    }

    //! \brief Returns a range object containing pairs consisting of edge
    //! labels and target nodes.
    //!
    //! This function returns a range object containing all the edge labels and
    //! targets of edges with source \p source.
    //!
    //! \param source the source node.
    //!
    //! \returns A range object.
    //!
    //! \throws LibsemigroupsException if \p source is out of bounds.
    [[nodiscard]] auto labels_and_targets(node_type source) const;

    //! \brief Compares two word graph views to see if they are equal
    //!
    //! This operator compares two views over two (not necessarily the same)
    //! word graph objects to see if the views are equal
    //! \returns True if this and that have the same number of nodes, out
    //! degree, and range over nodes with identical values and targets
    [[nodiscard]] bool operator==(WordGraphView const& that) const;

    //! \brief Compares two word graph views to see if they are not equal
    //! \returns True if this and that are not equal by \c ==
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
  };

  namespace word_graph_view {
    //! \brief Throws \c LIBSEMIGROUPS_EXCEPTION if the target node
    //! is not within the bounds specified for this word graph view
    //! that is \c n is not >= _start and <= _end
    template <typename Node1, typename Node2>
    void throw_if_node_out_of_bounds(WordGraphView<Node1> const& wg, Node2 n);

    //! \brief Throws \c LIBSEMIGROUPS_EXCEPTION if the target label is greater
    //! than or equal to the out_degree of the underlying graph
    template <typename Node>
    void
    throw_if_label_out_of_bounds(WordGraphView<Node> const&               wgv,
                                 typename WordGraphView<Node>::label_type a);

    //! \brief Creates a word graph from a corresponding view, copying only the
    //! nodes contained within the view. Will throw \c LibsemigroupsException if
    //! the underlying graph had edges which crossed the boundaries of the view
    template <typename Node>
    WordGraph<Node> graph_from_view(WordGraph<Node> const& view);
  }  // namespace word_graph_view
}  // namespace libsemigroups
#include "word-graph-view.tpp"

#endif  // LIBSEMIGROUPS_WORD_GRAPH_VIEW_HPP_
