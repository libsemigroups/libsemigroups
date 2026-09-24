//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2025-2026 James D. Mitchell
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

// This file contains deprecated helper functions for word graphs.

#ifndef LIBSEMIGROUPS_WORD_GRAPH_HELPERS_DEPRECATED_HPP_
#define LIBSEMIGROUPS_WORD_GRAPH_HELPERS_DEPRECATED_HPP_

#include "libsemigroups/word-graph-helpers.hpp"

namespace libsemigroups {
  namespace word_graph {

    //! \copydoc ancestors_of(WordGraph<Node> const&, Node)
    //! \deprecated_warning{function} Use the \ref ancestors_of overload with
    //! matching node types instead.
    template <typename Node1,
              typename Node2,
              typename = std::enable_if_t<!std::is_same_v<Node1, Node2>>>
    [[deprecated("Use ancestors_of with matching node types "
                 "instead.")]] [[nodiscard]] std::unordered_set<Node1>
    ancestors_of(WordGraph<Node1> const& wg, Node2 target) {
      static_assert(sizeof(Node2) <= sizeof(Node1));
      return ancestors_of(wg, static_cast<Node1>(target));
    }

    //! \copydoc ancestors_of_no_checks(WordGraph<Node> const&, Node)
    //! \deprecated_warning{function} Use the \ref ancestors_of_no_checks
    //! overload with matching node types instead.
    template <typename Node1,
              typename Node2,
              typename = std::enable_if_t<!std::is_same_v<Node1, Node2>>>
    [[deprecated("Use ancestors_of_no_checks with matching node types "
                 "instead.")]] [[nodiscard]] std::unordered_set<Node1>
    ancestors_of_no_checks(WordGraph<Node1> const& wg, Node2 target) {
      static_assert(sizeof(Node2) <= sizeof(Node1));
      return ancestors_of_no_checks(wg, static_cast<Node1>(target));
    }

    //! \brief Compares two word graphs on a contiguous range of nodes.
    //!
    //! This function returns \c true if the word graph views over the node
    //! range `[first, last)` in \p x and \p y are equal; and \c false
    //! otherwise.
    //!
    //! \tparam Node the type of the nodes of the WordGraph.
    //!
    //! \param x the first word graph for comparison.
    //! \param y the second word graph for comparison.
    //! \param first the first node in the range.
    //! \param last the last node in the range plus \c 1.
    //!
    //! \returns Whether or not the word graph views over the specified ranges
    //! are equal.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \warning No checks are performed to ensure that the arguments are
    //! valid.
    //!
    //! \deprecated_warning{function} Use the \ref equal_to_no_checks iterator
    //! overload instead.
    //!
    template <typename Node>
    [[deprecated("Use the equal_to_no_checks iterator overload "
                 "instead.")]] [[nodiscard]] bool
    equal_to_no_checks(WordGraph<Node> const& x,
                       WordGraph<Node> const& y,
                       Node                   first,
                       Node                   last) {
      return equal_to_no_checks(WordGraphView(x, first, last),
                                WordGraphView(y, first, last));
    }

    //! \brief Compares two word graphs on a contiguous range of nodes.
    //!
    //! This function returns \c true if the word graph views over the node
    //! range `[first, last)` in \p x and \p y are equal; and \c false
    //! otherwise.
    //!
    //! \tparam Node the type of the nodes of the WordGraph.
    //!
    //! \param x the first word graph for comparison.
    //! \param y the second word graph for comparison.
    //! \param first the first node in the range.
    //! \param last the last node in the range plus \c 1.
    //!
    //! \returns Whether or not the word graph views over the specified ranges
    //! are equal.
    //!
    //! \throws LibsemigroupsException if \p first is greater than \p last or
    //! if either value is out of bounds for \p x or \p y.
    //!
    //! \deprecated_warning{function} Use the \ref equal_to iterator overload
    //! instead.
    //!
    template <typename Node>
    [[deprecated(
        "Use the equal_to iterator overload instead.")]] [[nodiscard]] bool
    equal_to(WordGraph<Node> const& x,
             WordGraph<Node> const& y,
             Node                   first,
             Node                   last) {
      WordGraphView x_view(x);
      WordGraphView y_view(y);
      x_view.reshape(first, last);
      y_view.reshape(first, last);
      return equal_to(x_view, y_view);
    }

    //! \copydoc follow_path(WordGraph<Node> const&, Node, Iterator,
    //! Iterator)
    //! \deprecated_warning{function} Use the \ref follow_path overload with
    //! matching node types instead.
    template <typename Node1,
              typename Node2,
              typename Iterator,
              typename = std::enable_if_t<!std::is_same_v<Node1, Node2>>>
    [[deprecated("Use follow_path with matching node types "
                 "instead.")]] [[nodiscard]] Node1
    follow_path(WordGraph<Node1> const& wg,
                Node2                   source,
                Iterator                first,
                Iterator                last) {
      static_assert(sizeof(Node2) <= sizeof(Node1));
      return follow_path(wg, static_cast<Node1>(source), first, last);
    }

    //! \brief Find the node that a path starting at a given node leads to (if
    //! any).
    //!
    //! This function attempts to follow the path in the word graph  \p wg
    //! starting at the node \p from  labelled by the word \p path. If this
    //! path exists, then the last node on that path is returned. If this path
    //! does not exist, then \ref UNDEFINED is returned.
    //!
    //! \tparam Node the type of the nodes of the WordGraph \p wg and the node
    //! \p from.
    //!
    //! \param wg a word graph.
    //! \param from the starting node.
    //! \param path the path to follow.
    //!
    //! \returns
    //! A value of type \p Node. If one or more edges in \p path are not
    //! defined, then \ref UNDEFINED is returned.
    //!
    //! \throw LibsemigroupsException if \p from is not a node in the word
    //! graph or \p path contains a value that is not an edge-label.
    //!
    //! \par Complexity
    //! Linear in the length of \p path.
    //!
    //! \deprecated_warning{function} Use the \ref follow_path iterator
    //! overload instead.
    // TODO(2) example
    // not noexcept because WordGraph::target isn't
    template <typename Node>
    [[deprecated(
        "Use the follow_path iterator overload instead.")]] [[nodiscard]] Node
    follow_path(WordGraph<Node> const& wg, Node from, word_type const& path) {
      return follow_path(WordGraphView(wg), from, path.cbegin(), path.cend());
    }

    //! \copydoc follow_path(WordGraph<Node> const&, Node, word_type const&)
    //! \deprecated_warning{function} Use the \ref follow_path iterator overload
    //! instead.
    template <typename Node1,
              typename Node2,
              typename = std::enable_if_t<!std::is_same_v<Node1, Node2>>>
    [[deprecated(
        "Use the follow_path iterator overload instead.")]] [[nodiscard]] Node1
    follow_path(WordGraph<Node1> const& wg, Node2 from, word_type const& path) {
      static_assert(sizeof(Node2) <= sizeof(Node1));
      return follow_path(
          wg, static_cast<Node1>(from), path.cbegin(), path.cend());
    }

    //! \copydoc follow_path_no_checks(WordGraph<Node> const&, Node,
    //! Iterator, Iterator)
    //! \deprecated_warning{function} Use the \ref follow_path_no_checks
    //! overload with matching node types instead.
    template <typename Node1,
              typename Node2,
              typename Iterator,
              typename = std::enable_if_t<!std::is_same_v<Node1, Node2>>>
    [[deprecated("Use follow_path_no_checks with matching node types "
                 "instead.")]] [[nodiscard]] Node1
    follow_path_no_checks(WordGraph<Node1> const& wg,
                          Node2                   from,
                          Iterator                first,
                          Iterator                last) noexcept {
      static_assert(sizeof(Node2) <= sizeof(Node1));
      return follow_path_no_checks(wg, static_cast<Node1>(from), first, last);
    }

    //! \brief Follow the path from a specified node labelled by a word.
    //!
    //! This function returns the last node on the path in the word graph
    //! \p wg starting at the node \p from labelled by \p path or
    //! \ref UNDEFINED.
    //!
    //! \tparam Node the type of the nodes of the WordGraph \p wg and the node
    //! \p from.
    //!
    //! \param wg a word graph.
    //! \param from the source node.
    //! \param path the word.
    //!
    //! \returns A value of type \p Node.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! At worst the length of \p path.
    //!
    //! \warning
    //! No checks on the arguments of this function are performed.
    //!
    //! \deprecated_warning{function} Use the \ref follow_path_no_checks
    //! iterator overload instead.
    template <typename Node>
    [[deprecated("Use the follow_path_no_checks iterator overload "
                 "instead.")]] [[nodiscard]] Node
    follow_path_no_checks(WordGraph<Node> const& wg,
                          Node                   from,
                          word_type const&       path) noexcept {
      return follow_path_no_checks(
          WordGraphView(wg), from, path.cbegin(), path.cend());
    }

    //! \copydoc follow_path_no_checks(WordGraph<Node> const&, Node,
    //! word_type const&)
    //! \deprecated_warning{function} Use the \ref follow_path_no_checks
    //! iterator overload instead.
    template <typename Node1,
              typename Node2,
              typename = std::enable_if_t<!std::is_same_v<Node1, Node2>>>
    [[deprecated("Use the follow_path_no_checks iterator overload "
                 "instead.")]] [[nodiscard]] Node1
    follow_path_no_checks(WordGraph<Node1> const& wg,
                          Node2                   from,
                          word_type const&        path) noexcept {
      static_assert(sizeof(Node2) <= sizeof(Node1));
      return follow_path_no_checks(
          wg, static_cast<Node1>(from), path.cbegin(), path.cend());
    }

    //! \copydoc is_acyclic(WordGraph<Node> const&, Node)
    //! \deprecated_warning{function} Use the \ref is_acyclic overload with
    //! matching node types instead.
    template <typename Node1,
              typename Node2,
              typename = std::enable_if_t<!std::is_same_v<Node1, Node2>>>
    [[deprecated(
        "Use is_acyclic with matching node types instead.")]] [[nodiscard]] bool
    is_acyclic(WordGraph<Node1> const& wg, Node2 source) {
      static_assert(sizeof(Node2) <= sizeof(Node1));
      return is_acyclic(wg, static_cast<Node1>(source));
    }

    //! \copydoc is_acyclic(WordGraph<Node> const&, Node, Node)
    //! \deprecated_warning{function} Use the \ref is_acyclic overload with
    //! matching node types instead.
    template <typename Node1,
              typename Node2,
              typename = std::enable_if_t<!std::is_same_v<Node1, Node2>>>
    [[deprecated(
        "Use is_acyclic with matching node types instead.")]] [[nodiscard]] bool
    is_acyclic(WordGraph<Node1> const& wg, Node2 source, Node2 target) {
      static_assert(sizeof(Node2) <= sizeof(Node1));
      return is_acyclic(
          wg, static_cast<Node1>(source), static_cast<Node1>(target));
    }

    //! \copydoc is_reachable_no_checks(WordGraph<Node> const&, Node, Node)
    //! \deprecated_warning{function} Use the \ref is_reachable_no_checks
    //! overload with matching node types instead.
    template <typename Node1,
              typename Node2,
              typename = std::enable_if_t<!std::is_same_v<Node1, Node2>>>
    [[deprecated("Use is_reachable_no_checks with matching node types "
                 "instead.")]] [[nodiscard]] bool
    is_reachable_no_checks(WordGraph<Node1> const& wg,
                           Node2                   source,
                           Node2                   target) {
      static_assert(sizeof(Node2) <= sizeof(Node1));
      return is_reachable_no_checks(
          wg, static_cast<Node1>(source), static_cast<Node1>(target));
    }

    //! \copydoc is_reachable(WordGraph<Node> const&, Node, Node)
    //! \deprecated_warning{function} Use the \ref is_reachable overload with
    //! matching node types instead.
    template <typename Node1,
              typename Node2,
              typename = std::enable_if_t<!std::is_same_v<Node1, Node2>>>
    [[deprecated("Use is_reachable with matching node types "
                 "instead.")]] [[nodiscard]] bool
    is_reachable(WordGraph<Node1> const& wg, Node2 source, Node2 target) {
      static_assert(sizeof(Node2) <= sizeof(Node1));
      return is_reachable(
          wg, static_cast<Node1>(source), static_cast<Node1>(target));
    }

    //! \brief Check if a word graph is standardized.
    //!
    //! This function checks if the word graph \p wg is standardized according
    //! to the reduction order specified by \p val.
    //!
    //! \tparam Node the type of the node in \p wg.
    //!
    //! \param wg the word graph to check.
    //! \param val the order to use for standardization check (defaults to
    //! Order::lenlex).
    //!
    //! \no_libsemigroups_except
    //!
    //! \sa
    //! standardize.
    //!
    //! \deprecated_warning{function} Use \ref is_standardized(WordGraph<Node>
    //! const&, Cmp&&) instead.
    //!
    template <typename Node>
    [[deprecated("Use is_standardized(WordGraph<Node> const&, Cmp&&) "
                 "instead.")]] [[nodiscard]] bool
    is_standardized(WordGraph<Node> const& wg, Order val = Order::lenlex) {
      switch (val) {
        case Order::none:
          return true;
        case Order::lenlex:
          return is_standardized(WordGraphView(wg), LenLexCmp<>());
        case Order::lex:
          return is_standardized(WordGraphView(wg), LexCmp<>());
        case Order::rpo:
          return is_standardized(WordGraphView(wg), RPOCmp<>());
        case Order::rev_rpo:
          return is_standardized(WordGraphView(wg), RevRPOCmp<>());
        default:
          LIBSEMIGROUPS_EXCEPTION("not yet implemented")
      }
    }

    //! \copydoc last_node_on_path_no_checks(WordGraph<Node> const&, Node,
    //! Iterator, Iterator)
    //! \deprecated_warning{function} Use the \ref last_node_on_path_no_checks
    //! overload with matching node types instead.
    template <typename Node1,
              typename Node2,
              typename Iterator,
              typename = std::enable_if_t<!std::is_same_v<Node1, Node2>>>
    [[deprecated("Use last_node_on_path_no_checks with matching node types "
                 "instead.")]] [[nodiscard]] std::pair<Node1, Iterator>
    last_node_on_path_no_checks(WordGraph<Node1> const& wg,
                                Node2                   source,
                                Iterator                first,
                                Iterator                last) noexcept {
      static_assert(sizeof(Node2) <= sizeof(Node1));
      return last_node_on_path_no_checks(
          wg, static_cast<Node1>(source), first, last);
    }

    //! \copydoc last_node_on_path(WordGraph<Node> const&, Node, Iterator,
    //! Iterator)
    //! \deprecated_warning{function} Use the \ref last_node_on_path overload
    //! with matching node types instead.
    template <typename Node1,
              typename Node2,
              typename Iterator,
              typename = std::enable_if_t<!std::is_same_v<Node1, Node2>>>
    [[deprecated("Use last_node_on_path with matching node types "
                 "instead.")]] [[nodiscard]] std::pair<Node1, Iterator>
    last_node_on_path(WordGraph<Node1> const& wg,
                      Node2                   source,
                      Iterator                first,
                      Iterator                last) {
      static_assert(sizeof(Node2) <= sizeof(Node1));
      return last_node_on_path(wg, static_cast<Node1>(source), first, last);
    }

    //! \brief Returns the last node on the path labelled by a word and an
    //! iterator to the position in the word reached.
    //!
    //! \tparam Node the node type of the word graph and \p source.
    //!
    //! \param wg a word graph.
    //! \param source the source node.
    //! \param w the word.
    //!
    //! \returns A pair consisting of the last node reached and an iterator
    //! pointing at the last letter in the word labelling an edge.
    //!
    //! \complexity
    //! At worst the distance from `w.size()`.
    //!
    //! \warning
    //! No checks on the arguments of this function are performed, it is
    //! assumed that \p source is a node in the word graph \p wg; and that the
    //! letters in the word described by \p first and \p last belong to the
    //! range \c 0 to WordGraph::out_degree.
    //!
    //! \deprecated_warning{function} Use the \ref last_node_on_path_no_checks
    //! iterator overload instead.
    template <typename Node>
    [[deprecated(
        "Use the last_node_on_path_no_checks iterator overload "
        "instead.")]] [[nodiscard]] std::pair<Node, word_type::const_iterator>
    last_node_on_path_no_checks(WordGraph<Node> const& wg,
                                Node                   source,
                                word_type const&       w) {
      return last_node_on_path_no_checks(
          WordGraphView(wg), source, w.cbegin(), w.cend());
    }

    //! \copydoc last_node_on_path_no_checks(WordGraph<Node> const&, Node,
    //! word_type const&)
    //! \deprecated_warning{function} Use the \ref last_node_on_path_no_checks
    //! iterator overload instead.
    template <typename Node1,
              typename Node2,
              typename = std::enable_if_t<!std::is_same_v<Node1, Node2>>>
    [[deprecated(
        "Use the last_node_on_path_no_checks iterator overload "
        "instead.")]] [[nodiscard]] std::pair<Node1, word_type::const_iterator>
    last_node_on_path_no_checks(WordGraph<Node1> const& wg,
                                Node2                   source,
                                word_type const&        w) {
      static_assert(sizeof(Node2) <= sizeof(Node1));
      return last_node_on_path_no_checks(
          wg, static_cast<Node1>(source), w.cbegin(), w.cend());
    }

    //! \brief Returns the last node on the path labelled by a word and an
    //! iterator to the position in the word reached.
    //!
    //! \tparam Node the node type of the word graph and \p source.
    //!
    //! \param wg a word graph.
    //! \param source the source node.
    //! \param w the word.
    //!
    //! \returns A pair consisting of the last node reached and an iterator
    //! pointing at the last letter in the word labelling an edge.
    //!
    //! \complexity
    //! At worst the distance from `w.size()`.
    //!
    //! \note If any value in \p wg or in the word described by \p first and
    //! \p last is out of bounds (greater than or equal to
    //! WordGraph::number_of_nodes), the path labelled by the word exits the
    //! word graph, which is reflected in the result value of this function,
    //! but does not cause an exception to be thrown.
    //!
    //! \deprecated_warning{function} Use the \ref last_node_on_path iterator
    //! overload instead.
    template <typename Node>
    [[deprecated(
        "Use the last_node_on_path iterator overload "
        "instead.")]] [[nodiscard]] std::pair<Node, word_type::const_iterator>
    last_node_on_path(WordGraph<Node> const& wg,
                      Node                   source,
                      word_type const&       w) {
      return last_node_on_path(WordGraphView(wg), source, w.cbegin(), w.cend());
    }

    //! \copydoc last_node_on_path(WordGraph<Node> const&, Node, word_type
    //!  const&)
    //! \deprecated_warning{function} Use the \ref last_node_on_path iterator
    //! overload instead.
    template <typename Node1,
              typename Node2,
              typename = std::enable_if_t<!std::is_same_v<Node1, Node2>>>
    [[deprecated(
        "Use the last_node_on_path iterator overload "
        "instead.")]] [[nodiscard]] std::pair<Node1, word_type::const_iterator>
    last_node_on_path(WordGraph<Node1> const& wg,
                      Node2                   source,
                      word_type const&        w) {
      static_assert(sizeof(Node2) <= sizeof(Node1));
      return last_node_on_path(
          wg, static_cast<Node1>(source), w.cbegin(), w.cend());
    }

    //! \copydoc nodes_reachable_from_no_checks(WordGraph<Node> const&,
    //! Node, size_t)
    //! \deprecated_warning{function} Use the
    //! \ref nodes_reachable_from_no_checks overload with matching node types
    //! instead.
    template <typename Node1,
              typename Node2,
              typename = std::enable_if_t<!std::is_same_v<Node1, Node2>>>
    [[deprecated("Use nodes_reachable_from_no_checks with matching node types "
                 "instead.")]] [[nodiscard]] std::unordered_set<Node1>
    nodes_reachable_from_no_checks(WordGraph<Node1> const& wg,
                                   Node2                   source,
                                   size_t max_depth = POSITIVE_INFINITY) {
      static_assert(sizeof(Node2) <= sizeof(Node1));
      return nodes_reachable_from_no_checks(
          wg, static_cast<Node1>(source), max_depth);
    }

    //! \copydoc nodes_reachable_from(WordGraph<Node> const&, Node, size_t)
    //! \deprecated_warning{function} Use the \ref nodes_reachable_from overload
    //! with matching node types instead.
    template <typename Node1,
              typename Node2,
              typename = std::enable_if_t<!std::is_same_v<Node1, Node2>>>
    [[deprecated("Use nodes_reachable_from with matching node types "
                 "instead.")]] [[nodiscard]] std::unordered_set<Node1>
    nodes_reachable_from(WordGraph<Node1> const& wg,
                         Node2                   source,
                         size_t max_depth = POSITIVE_INFINITY) {
      static_assert(sizeof(Node2) <= sizeof(Node1));
      return nodes_reachable_from(wg, static_cast<Node1>(source), max_depth);
    }

    //! \copydoc number_of_nodes_reachable_from(WordGraph<Node> const&,
    //! Node, size_t)
    //! \deprecated_warning{function} Use the
    //! \ref number_of_nodes_reachable_from overload with matching node types
    //! instead.
    template <typename Node1,
              typename Node2,
              typename = std::enable_if_t<!std::is_same_v<Node1, Node2>>>
    [[deprecated("Use number_of_nodes_reachable_from with matching node types "
                 "instead.")]] [[nodiscard]] size_t
    number_of_nodes_reachable_from(WordGraph<Node1> const& wg,
                                   Node2                   source,
                                   size_t max_depth = POSITIVE_INFINITY) {
      static_assert(sizeof(Node2) <= sizeof(Node1));
      return number_of_nodes_reachable_from(
          wg, static_cast<Node1>(source), max_depth);
    }

    //! \copydoc number_of_nodes_reachable_from_no_checks(WordGraph<Node>
    //! const&, Node, size_t)
    //! \deprecated_warning{function} Use
    //! the \ref number_of_nodes_reachable_from_no_checks overload with
    //! matching node types
    //! instead.
    template <typename Node1,
              typename Node2,
              typename = std::enable_if_t<!std::is_same_v<Node1, Node2>>>
    [[deprecated("Use number_of_nodes_reachable_from_no_checks with matching "
                 "node types instead.")]] [[nodiscard]] size_t
    number_of_nodes_reachable_from_no_checks(WordGraph<Node1> const& wg,
                                             Node2                   source,
                                             size_t                  max_depth
                                             = POSITIVE_INFINITY) {
      static_assert(sizeof(Node2) <= sizeof(Node1));
      return number_of_nodes_reachable_from_no_checks(
          wg, static_cast<Node1>(source), max_depth);
    }

    //! \copydoc spanning_tree_no_checks(WordGraph<Node> const&, Node,
    //! Forest&, size_t)
    //! \deprecated_warning{function} Use the \ref spanning_tree_no_checks
    //! overload with matching node types instead.
    template <typename Node1,
              typename Node2,
              typename = std::enable_if_t<!std::is_same_v<Node1, Node2>>>
    [[deprecated(
        "Use spanning_tree_no_checks with matching node types instead.")]] void
    spanning_tree_no_checks(WordGraph<Node1> const& wg,
                            Node2                   root,
                            Forest&                 f,
                            size_t max_depth = POSITIVE_INFINITY) {
      static_assert(sizeof(Node2) <= sizeof(Node1));
      return spanning_tree_no_checks(
          wg, static_cast<Node1>(root), f, max_depth);
    }

    //! \copydoc spanning_tree(WordGraph<Node> const&, Node, Forest&,
    //! size_t)
    //! \deprecated_warning{function} Use the \ref spanning_tree overload with
    //! matching node types instead.
    template <typename Node1,
              typename Node2,
              typename = std::enable_if_t<!std::is_same_v<Node1, Node2>>>
    [[deprecated("Use spanning_tree with matching node types instead.")]] void
    spanning_tree(WordGraph<Node1> const& wg,
                  Node2                   root,
                  Forest&                 f,
                  size_t                  max_depth = POSITIVE_INFINITY) {
      static_assert(sizeof(Node2) <= sizeof(Node1));
      spanning_tree(wg, static_cast<Node1>(root), f, max_depth);
    }

    //! \copydoc spanning_tree_no_checks(WordGraph<Node> const&, Node,
    //! size_t)
    //! \deprecated_warning{function} Use the \ref spanning_tree_no_checks
    //! overload with matching node types instead.
    template <typename Node1,
              typename Node2,
              typename = std::enable_if_t<!std::is_same_v<Node1, Node2>>>
    [[deprecated("Use spanning_tree_no_checks with matching node types "
                 "instead.")]] [[nodiscard]] Forest
    spanning_tree_no_checks(WordGraph<Node1> const& wg,
                            Node2                   root,
                            size_t max_depth = POSITIVE_INFINITY) {
      static_assert(sizeof(Node2) <= sizeof(Node1));
      return spanning_tree_no_checks(wg, static_cast<Node1>(root), max_depth);
    }

    //! \copydoc spanning_tree(WordGraph<Node> const&, Node, size_t)
    //! \deprecated_warning{function} Use the \ref spanning_tree overload with
    //! matching node types instead.
    template <typename Node1,
              typename Node2,
              typename = std::enable_if_t<!std::is_same_v<Node1, Node2>>>
    [[deprecated("Use spanning_tree with matching node types "
                 "instead.")]] [[nodiscard]] Forest
    spanning_tree(WordGraph<Node1> const& wg,
                  Node2                   root,
                  size_t                  max_depth = POSITIVE_INFINITY) {
      static_assert(sizeof(Node2) <= sizeof(Node1));
      return spanning_tree(wg, static_cast<Node1>(root), max_depth);
    }

    //! \brief Standardizes a word graph in-place.
    //!
    //! This function standardizes the word graph \p wg according to the
    //! reduction order specified by \p val, and replaces the contents of the
    //! Forest \p f with a spanning tree rooted at \c 0 for the node reachable
    //! from \c 0. The spanning tree corresponds to the order \p val.
    //!
    //! \tparam Graph the type of the word graph \p wg.
    //!
    //! \param wg the word graph.
    //! \param f the Forest object to store the spanning tree.
    //! \param val the order to use for standardization.
    //!
    //! \returns
    //! This function returns \c true if the word graph \p wg is modified by
    //! this function (i.e. it was not standardized already), and \c false
    //! otherwise.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \note If any target of any edge in the word graph \p wg is out of
    //! bounds, then this is ignored by this function.
    //!
    //! \warning If there are nodes in the \p wg that are not reachable from
    //! the node \c 0, then this function may not preserve \p wg up to
    //! isomorphism. However, the isomorphism type of the sub-word-graph
    //! consisting of those nodes reachable from the node \c 0 is preserved.
    //!
    // NOTE: there's no WordGraphView version of this function because it
    // modifies it's argument and WordGraphView is read-only.
    //
    //!
    //! \deprecated_warning{function} Use \ref standardize_no_checks(Graph&,
    //! Forest&, Cmp&&) instead.
    // Not nodiscard because sometimes we just don't want the output
    template <typename Graph>
    [[deprecated(
        "Use standardize_no_checks(Graph&, Forest&, Cmp&&) instead.")]] bool
    standardize_no_checks(Graph& wg, Forest& f, Order val);

    //! \brief Standardizes a word graph in-place.
    //!
    //! This function standardizes the word graph \p wg according to the
    //! reduction order specified by \p val, and replaces the contents of the
    //! Forest \p f with a spanning tree rooted at \c 0 for the node reachable
    //! from \c 0. The spanning tree corresponds to the order \p val.
    //!
    //! \tparam Graph the type of the word graph \p wg.
    //!
    //! \param wg the word graph.
    //! \param f the Forest object to store the spanning tree.
    //! \param val the order to use for standardization.
    //!
    //! \returns
    //! This function returns \c true if the word graph \p wg is modified by
    //! this function (i.e. it was not standardized already), and \c false
    //! otherwise.
    //!
    //! \throws LibsemigroupsException if any target or any label of \p wg is
    //! out of bounds.
    //!
    //! \warning If there are nodes in the \p wg that are not reachable from
    //! the node \c 0, then this function may not preserve \p wg up to
    //! isomorphism. However, the isomorphism type of the sub-word-graph
    //! consisting of those nodes reachable from the node \c 0 is preserved.
    //!
    // NOTE: there's no WordGraphView version of this function because it
    // modifies it's argument and WordGraphView is read-only.
    //
    //!
    //! \deprecated_warning{function} Use \ref standardize(Graph&, Forest&,
    //! Cmp&&) instead.
    // Not nodiscard because sometimes we just don't want the output
    template <typename Graph>
    [[deprecated("Use standardize(Graph&, Forest&, Cmp&&) instead.")]] bool
    standardize(Graph& wg, Forest& f, Order val);

    //! \brief Standardizes a word graph in-place.
    //!
    //! This function standardizes the word graph \p wg according to the
    //! reduction order specified by \p val, and returns a Forest object
    //! containing a spanning tree rooted at \c 0 for the node reachable from
    //! \c 0. The spanning tree corresponds to the order \p val.
    //!
    //! \tparam Graph the type of the word graph \p wg.
    //!
    //! \param wg the word graph.
    //! \param val the order to use for standardization.
    //!
    //! \returns
    //! A std::pair the first entry of which is \c true if the word graph
    //! \p wg is modified by this function (i.e. it was not standardized
    //! already), and
    //! \c false otherwise. The second entry is a Forest object containing a
    //! spanning tree for \p wg.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \note If any target of any edge in the word graph \p wg is out of
    //! bounds, then this is ignored by this function.
    //!
    //! \warning If there are nodes in the \p wg that are not reachable from
    //! the node \c 0, then this function may not preserve \p wg up to
    //! isomorphism. However, the isomorphism type of the sub-word-graph
    //! consisting of those nodes reachable from the node \c 0 is preserved.
    //!
    // NOTE: there's no WordGraphView version of this function because it
    // modifies it's argument and WordGraphView is read-only.
    //
    //!
    //! \deprecated_warning{function} Use \ref standardize_no_checks(Graph&,
    //! Cmp&&) instead.
    // Not nodiscard because sometimes we just don't want the output
    template <typename Graph>
    [[deprecated(
        "Use standardize_no_checks(Graph&, Cmp&&) instead.")]] std::pair<bool,
                                                                         Forest>
    standardize_no_checks(Graph& wg, Order val = Order::lenlex);

    //! \brief Standardizes a word graph in-place.
    //!
    //! This function standardizes the word graph \p wg according to the
    //! reduction order specified by \p val, and returns a Forest object
    //! containing a spanning tree rooted at \c 0 for the node reachable from
    //! \c 0. The spanning tree corresponds to the order \p val.
    //!
    //! \tparam Graph the type of the word graph \p wg.
    //!
    //! \param wg the word graph.
    //! \param val the order to use for standardization.
    //!
    //! \returns
    //! A std::pair the first entry of which is \c true if the word graph
    //! \p wg is modified by this function (i.e. it was not standardized
    //! already), and
    //! \c false otherwise. The second entry is a Forest object containing a
    //! spanning tree for \p wg.
    //!
    //! \throws LibsemigroupsException if any target or any label of \p wg is
    //! out of bounds.
    //!
    //! \warning If there are nodes in the \p wg that are not reachable from
    //! the node \c 0, then this function may not preserve \p wg up to
    //! isomorphism. However, the isomorphism type of the sub-word-graph
    //! consisting of those nodes reachable from the node \c 0 is preserved.
    //!
    // NOTE: there's no WordGraphView version of this function because it
    // modifies it's argument and WordGraphView is read-only.
    //
    //!
    //! \deprecated_warning{function} Use \ref standardize(Graph&, Cmp&&)
    //! instead.
    // Not nodiscard because sometimes we just don't want the output
    template <typename Graph>
    [[deprecated(
        "Use standardize(Graph&, Cmp&&) instead.")]] std::pair<bool, Forest>
    standardize(Graph& wg, Order val = Order::lenlex);

    //! \brief Throws if the target of any edge is out of bounds.
    //!
    //! This function throws if any target of any edge in \p wg is out of
    //! bounds (i.e. is greater than or equal to WordGraph::number_of_nodes,
    //! and not equal to \ref UNDEFINED).
    //!
    //! \tparam Node the type of the nodes in \p wg.
    //!
    //! \param wg the word graph to check.
    //!
    //! \throws LibsemigroupsException if any target of any edge in \p wg is
    //! greater than or equal to WordGraph::number_of_nodes and not equal to
    //! \ref UNDEFINED.
    //!
    //! \deprecated_warning{function} Use \ref validate(WordGraph<Node> const&)
    //! instead.
    template <typename Node>
    [[deprecated("Use validate(WordGraph<Node> const&) instead.")]] void
    throw_if_any_target_out_of_bounds(WordGraph<Node> const& wg) {
      WordGraphView wgv(wg);
      throw_if_any_target_out_of_bounds(
          wgv, wgv.cbegin_nodes_no_checks(), wgv.cend_nodes_no_checks());
    }

    //! \brief Throws if a label is out of bounds.
    //!
    //! This function throws if the label \p a is out of bounds, i.e. it is
    //! greater than or equal to `wg.out_degree()`.
    //!
    //! \tparam Node the type of the nodes in \p wg.
    //!
    //! \param wg the word graph.
    //! \param a the label to check.
    //!
    //! \throws LibsemigroupsException if the label \p a is out of bounds.
    //!
    //! \deprecated_warning{function} Use the checked WordGraph member
    //! functions instead.
    // not noexcept because it throws an exception!
    template <typename Node>
    [[deprecated("Use the checked WordGraph member functions instead.")]] void
    throw_if_label_out_of_bounds(WordGraph<Node> const&               wg,
                                 typename WordGraph<Node>::label_type a);

    //! \brief Throws if a label is out of bounds.
    //!
    //! This function throws if any of the letters in \p word are out of bounds,
    //! i.e. if they are greater than or equal to `wg.out_degree()`.
    //!
    //! \tparam Node the type of the nodes in \p wg.
    //!
    //! \param wg the word graph.
    //! \param word the word to check.
    //!
    //! \throws LibsemigroupsException if any value in \p word is out of
    //! bounds.
    //!
    //! \deprecated_warning{function} Use the checked WordGraph member
    //! functions instead.
    template <typename Node>
    [[deprecated("Use the checked WordGraph member functions instead.")]] void
    throw_if_label_out_of_bounds(WordGraph<Node> const& wg,
                                 word_type const&       word);

    //! \brief Throws if a label is out of bounds.
    //!
    //! This function throws if any of the letters in the word defined by
    //! \p first and \p last is out of bounds, i.e. if they are greater than or
    //! equal to `wg.out_degree()`.
    //!
    //! \tparam Node the type of the nodes in \p wg.
    //! \tparam Iterator the type of the arguments \p first and \p last.
    //!
    //! \param wg the word graph.
    //! \param first iterator pointing at the first letter to check.
    //! \param last iterator pointing one beyond the last letter to check.
    //!
    //! \throws LibsemigroupsException if any value in the word word defined by
    //! \p first and \p last is out of bounds.
    //!
    //! \deprecated_warning{function} Use the checked WordGraph member
    //! functions instead.
    template <typename Node, typename Iterator>
    [[deprecated("Use the checked WordGraph member functions instead.")]] void
    throw_if_label_out_of_bounds(WordGraph<Node> const& wg,
                                 Iterator               first,
                                 Iterator               last);

    //! \brief Throws if a node is out of bounds.
    //!
    //! This function throws if the node \p n is out of bounds
    //! i.e. if it is greater than or equal to `wg.number_of_nodes()`.
    //!
    //! \tparam Node the node type of the word graph and \p n.
    //!
    //! \param wg the word graph.
    //! \param n the node to check.
    //!
    //! \throws LibsemigroupsException if \p n is out of bounds.
    //!
    //! \deprecated_warning{function} Use the checked WordGraph member
    //! functions instead.
    // not noexcept because it throws an exception!
    template <typename Node>
    [[deprecated("Use the checked WordGraph member functions instead.")]] void
    throw_if_node_out_of_bounds(WordGraph<Node> const& wg, Node n);

    //! \copydoc throw_if_node_out_of_bounds(WordGraph<Node> const&, Node)
    //! \deprecated_warning{function} Use the checked WordGraph member
    //! functions instead.
    template <typename Node1,
              typename Node2,
              typename = std::enable_if_t<!std::is_same_v<Node1, Node2>>>
    [[deprecated("Use the checked WordGraph member functions instead.")]] void
    throw_if_node_out_of_bounds(WordGraph<Node1> const& wg, Node2 n) {
      static_assert(sizeof(Node2) <= sizeof(Node1));
      detail::throw_if_not_less(n, wg.number_of_nodes(), "node ");
    }

    //! \brief Throws if any node in a range is out of bounds.
    //!
    //! This function throws if any node in the range from \p first to \p last
    //! is out of bounds i.e. if they are greater than or equal to
    //! `wg.number_of_nodes()`.
    //!
    //! \tparam Node the node type of the word graph.
    //! \tparam Iterator the type of the parameters \p first and \p last.
    //!
    //! \param wg the word graph.
    //! \param first an iterator pointing at the first node to check.
    //! \param last an iterator pointing one beyond the last node to check.
    //!
    //! \throws LibsemigroupsException if any node in the range \p first to
    //! \p last is out of bounds.
    //!
    //! \deprecated_warning{function} Use the checked WordGraph member
    //! functions instead.
    // not noexcept because it throws an exception!
    template <typename Node, typename Iterator1, typename Iterator2>
    [[deprecated("Use the checked WordGraph member functions instead.")]] void
    throw_if_node_out_of_bounds(WordGraph<Node> const& wg,
                                Iterator1              first,
                                Iterator2              last);

    //! \copydoc topological_sort(WordGraph<Node> const&, Node)
    //! \deprecated_warning{function} Use the \ref topological_sort overload
    //! with matching node types instead.
    template <typename Node1,
              typename Node2,
              typename = std::enable_if_t<!std::is_same_v<Node1, Node2>>>
    [[deprecated("Use topological_sort with matching node types "
                 "instead.")]] [[nodiscard]] std::vector<Node1>
    topological_sort(WordGraph<Node1> const& wg, Node2 source) {
      static_assert(sizeof(Node2) <= sizeof(Node1));
      return topological_sort(wg, static_cast<Node1>(source));
    }

  }  // namespace word_graph

  //! \ingroup word_graph_group
  //! \brief Helper variable template.
  //!
  //! The value of this variable is \c true if the template parameter \p Thing
  //! is \ref WordGraph for any template parameters.
  //!
  //! \tparam Thing a type.
  //!
  //! \deprecated_alias_warning{is_specialization_of_v<Thing, WordGraph>}
  template <typename Thing>
  static constexpr bool IsWordGraph
      [[deprecated("Use is_specialization_of_v<Thing, WordGraph> instead.")]]
      = is_specialization_of_v<Thing, WordGraph>;

}  // namespace libsemigroups

#include "libsemigroups/word-graph-helpers-deprecated.tpp"

#endif  // LIBSEMIGROUPS_WORD_GRAPH_HELPERS_DEPRECATED_HPP_
