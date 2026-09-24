//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2025-2026 Nadim Searight
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

// This file contains deprecated helper functions for word graph views.

#ifndef LIBSEMIGROUPS_WORD_GRAPH_VIEW_HELPERS_DEPRECATED_HPP_
#define LIBSEMIGROUPS_WORD_GRAPH_VIEW_HELPERS_DEPRECATED_HPP_

#include "libsemigroups/word-graph-view-helpers.hpp"

namespace libsemigroups {
  namespace word_graph {

    //! \copydoc ancestors_of_no_checks(WordGraphView<Node> const&, Node)
    //! \deprecated_warning{function} Use the \ref ancestors_of_no_checks
    //! overload with matching node types instead.
    template <typename Node1,
              typename Node2,
              typename = std::enable_if_t<!std::is_same_v<Node1, Node2>>>
    [[deprecated("Use ancestors_of_no_checks with matching node types "
                 "instead.")]] [[nodiscard]] std::unordered_set<Node1>
    ancestors_of_no_checks(WordGraphView<Node1> const& wgv, Node2 target) {
      static_assert(sizeof(Node2) <= sizeof(Node1));
      return ancestors_of_no_checks(wgv, static_cast<Node1>(target));
    }

    //! \copydoc ancestors_of(WordGraphView<Node> const&, Node)
    //! \deprecated_warning{function} Use the \ref ancestors_of overload with
    //! matching node types instead.
    template <typename Node1,
              typename Node2,
              typename = std::enable_if_t<!std::is_same_v<Node1, Node2>>>
    [[deprecated("Use ancestors_of with matching node types "
                 "instead.")]] [[nodiscard]] std::unordered_set<Node1>
    ancestors_of(WordGraphView<Node1> const& wgv, Node2 target) {
      static_assert(sizeof(Node2) <= sizeof(Node1));
      return ancestors_of(wgv, static_cast<Node1>(target));
    }

    //! \copydoc follow_path_no_checks(WordGraphView<Node> const&, Node,
    //! Iterator, Iterator)
    //! \deprecated_warning{function} Use the \ref follow_path_no_checks
    //! overload with matching node types instead.
    template <typename Node1,
              typename Node2,
              typename Iterator,
              typename = std::enable_if_t<!std::is_same_v<Node1, Node2>>>
    [[deprecated("Use follow_path_no_checks with matching node types "
                 "instead.")]] [[nodiscard]] Node1
    follow_path_no_checks(WordGraphView<Node1> const& wgv,
                          Node2                       from,
                          Iterator                    first,
                          Iterator                    last) noexcept {
      static_assert(sizeof(Node2) <= sizeof(Node1));
      return follow_path_no_checks(wgv, static_cast<Node1>(from), first, last);
    }

    //! \brief Follow the path from a specified node labelled by a word.
    //!
    //! This function returns the last node on the path in the word graph view
    //! \p wgv starting at the node \p from labelled by \p path or
    //! \ref UNDEFINED.
    //!
    //! \tparam Node the type of the nodes of the WordGraphView \p wgv and the
    //! node \p from.
    //!
    //! \param wgv a word graph view.
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
    follow_path_no_checks(WordGraphView<Node> const& wgv,
                          Node                       from,
                          word_type const&           path) noexcept {
      return follow_path_no_checks(wgv, from, path.cbegin(), path.cend());
    }

    //! \copydoc follow_path_no_checks(WordGraphView<Node> const&, Node,
    //! word_type const&)
    //! \deprecated_warning{function} Use the \ref follow_path_no_checks
    //! iterator overload instead.
    template <typename Node1,
              typename Node2,
              typename = std::enable_if_t<!std::is_same_v<Node1, Node2>>>
    [[deprecated("Use the follow_path_no_checks iterator overload "
                 "instead.")]] [[nodiscard]] Node1
    follow_path_no_checks(WordGraphView<Node1> const& wgv,
                          Node2                       from,
                          word_type const&            path) noexcept {
      static_assert(sizeof(Node2) <= sizeof(Node1));
      return follow_path_no_checks(
          wgv, static_cast<Node1>(from), path.cbegin(), path.cend());
    }

    //! \copydoc follow_path(WordGraphView<Node> const&, Node, Iterator,
    //! Iterator)
    //! \deprecated_warning{function} Use the \ref follow_path overload with
    //! matching node types instead.
    template <typename Node1,
              typename Node2,
              typename Iterator,
              typename = std::enable_if_t<!std::is_same_v<Node1, Node2>>>
    [[deprecated("Use follow_path with matching node types "
                 "instead.")]] [[nodiscard]] Node1
    follow_path(WordGraphView<Node1> const& wgv,
                Node2                       source,
                Iterator                    first,
                Iterator                    last) {
      static_assert(sizeof(Node2) <= sizeof(Node1));
      return follow_path(wgv, static_cast<Node1>(source), first, last);
    }

    //! \brief Find the node that a path starting at a given node leads to (if
    //! any).
    //!
    //! This function attempts to follow the path in the word graph view
    //! \p wgv starting at the node \p from  labelled by the word \p path. If
    //! this path exists, then the last node on that path is returned. If this
    //! path does not exist, then \ref UNDEFINED is returned.
    //!
    //! \tparam Node the type of the nodes of the WordGraphView \p wgv and the
    //! node \p from.
    //!
    //! \param wgv a word graph view.
    //! \param from the starting node.
    //! \param path the path to follow.
    //!
    //! \returns
    //! A value of type \p Node. If one or more edges in \p path are not
    //! defined, then \ref UNDEFINED is returned.
    //!
    //! \throw LibsemigroupsException if \p from is not a node in the word
    //! graph view or \p path contains a value that is not an edge-label.
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
    follow_path(WordGraphView<Node> const& wgv,
                Node                       from,
                word_type const&           path) {
      return follow_path(wgv, from, path.cbegin(), path.cend());
    }

    //! \copydoc follow_path(WordGraphView<Node> const&, Node,
    //! word_type const&)
    //! \deprecated_warning{function} Use the \ref follow_path iterator
    //! overload instead.
    template <typename Node1,
              typename Node2,
              typename = std::enable_if_t<!std::is_same_v<Node1, Node2>>>
    [[deprecated(
        "Use the follow_path iterator overload instead.")]] [[nodiscard]] Node1
    follow_path(WordGraphView<Node1> const& wgv,
                Node2                       from,
                word_type const&            path) {
      static_assert(sizeof(Node2) <= sizeof(Node1));
      return follow_path(
          wgv, static_cast<Node1>(from), path.cbegin(), path.cend());
    }

    //! \copydoc is_acyclic(WordGraphView<Node> const&, Node)
    //! \deprecated_warning{function} Use the \ref is_acyclic overload with
    //! matching node types instead.
    template <typename Node1,
              typename Node2,
              typename = std::enable_if_t<!std::is_same_v<Node1, Node2>>>
    [[deprecated(
        "Use is_acyclic with matching node types instead.")]] [[nodiscard]] bool
    is_acyclic(WordGraphView<Node1> const& wgv, Node2 source) {
      static_assert(sizeof(Node2) <= sizeof(Node1));
      return is_acyclic(wgv, static_cast<Node1>(source));
    }

    //! \copydoc is_acyclic(WordGraphView<Node> const&, Node, Node)
    //! \deprecated_warning{function} Use the \ref is_acyclic overload with
    //! matching node types instead.
    template <typename Node1,
              typename Node2,
              typename = std::enable_if_t<!std::is_same_v<Node1, Node2>>>
    [[deprecated(
        "Use is_acyclic with matching node types instead.")]] [[nodiscard]] bool
    is_acyclic(WordGraphView<Node1> const& wgv, Node2 source, Node2 target) {
      static_assert(sizeof(Node2) <= sizeof(Node1));
      return is_acyclic(
          wgv, static_cast<Node1>(source), static_cast<Node1>(target));
    }

    //! \copydoc is_reachable_no_checks(WordGraphView<Node> const&, Node,
    //! Node)
    //! \deprecated_warning{function} Use the \ref is_reachable_no_checks
    //! overload with matching node types instead.
    template <typename Node1,
              typename Node2,
              typename = std::enable_if_t<!std::is_same_v<Node1, Node2>>>
    [[deprecated("Use is_reachable_no_checks with matching node types "
                 "instead.")]] [[nodiscard]] bool
    is_reachable_no_checks(WordGraphView<Node1> const& wgv,
                           Node2                       source,
                           Node2                       target) {
      static_assert(sizeof(Node2) <= sizeof(Node1));
      return is_reachable_no_checks(
          wgv, static_cast<Node1>(source), static_cast<Node1>(target));
    }

    //! \copydoc is_reachable(WordGraphView<Node> const&, Node, Node)
    //! \deprecated_warning{function} Use the \ref is_reachable overload with
    //! matching node types instead.
    template <typename Node1,
              typename Node2,
              typename = std::enable_if_t<!std::is_same_v<Node1, Node2>>>
    [[deprecated("Use is_reachable with matching node types "
                 "instead.")]] [[nodiscard]] bool
    is_reachable(WordGraphView<Node1> const& wgv, Node2 source, Node2 target) {
      static_assert(sizeof(Node2) <= sizeof(Node1));
      return is_reachable(
          wgv, static_cast<Node1>(source), static_cast<Node1>(target));
    }

    //! \brief Check if a word graph is standardized.
    //!
    //! This function checks if the word graph \p wgv is standardized
    //! according to the reduction order specified by \p val.
    //!
    //! \tparam Node the type of the node in \p wgv.
    //!
    //! \param wgv the word graph to check.
    //! \param val the order to use for standardization check (defaults to
    //! Order::lenlex).
    //!
    //! \no_libsemigroups_except
    //!
    //! \sa
    //! standardize.
    //!
    //! \deprecated_warning{function} Use
    //! \ref is_standardized(WordGraphView<Node> const&, Cmp&&) instead.
    template <typename Node>
    [[nodiscard]] [[deprecated(
        "Use is_standardized(WordGraphView<Node> const&, Cmp&&) "
        "instead.")]] bool
    is_standardized(WordGraphView<Node> const& wgv, Order val = Order::lenlex);

    //! \copydoc last_node_on_path_no_checks(WordGraphView<Node> const&, Node,
    //! Iterator, Iterator)
    //! \deprecated_warning{function} Use the \ref last_node_on_path_no_checks
    //! overload with matching node types instead.
    template <typename Node1,
              typename Node2,
              typename Iterator,
              typename = std::enable_if_t<!std::is_same_v<Node1, Node2>>>
    [[deprecated("Use last_node_on_path_no_checks with matching node types "
                 "instead.")]] [[nodiscard]] std::pair<Node1, Iterator>
    last_node_on_path_no_checks(WordGraphView<Node1> const& wgv,
                                Node2                       source,
                                Iterator                    first,
                                Iterator                    last) noexcept {
      static_assert(sizeof(Node2) <= sizeof(Node1));
      return last_node_on_path_no_checks(
          wgv, static_cast<Node1>(source), first, last);
    }

    //! \brief Returns the last node on the path labelled by a word and an
    //! iterator to the position in the word reached.
    //!
    //! \tparam Node the node type of the word graph view and \p source.
    //!
    //! \param wgv a word graph view.
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
    //! assumed that \p source is a node in the word graph view \p wgv; and
    //! that the letters in the word described by \p first and \p last belong
    //! to the range \c 0 to WordGraphView::out_degree.
    //!
    //! \deprecated_warning{function} Use the \ref last_node_on_path_no_checks
    //! iterator overload instead.
    template <typename Node>
    [[nodiscard]] [[deprecated(
        "Use the last_node_on_path_no_checks iterator overload "
        "instead.")]] std::pair<Node, word_type::const_iterator>
    last_node_on_path_no_checks(WordGraphView<Node> const& wgv,
                                Node                       source,
                                word_type const&           w);

    //! \copydoc last_node_on_path_no_checks(WordGraphView<Node> const&, Node,
    //! word_type const&)
    //! \deprecated_warning{function} Use the \ref last_node_on_path_no_checks
    //! iterator overload instead.
    template <typename Node1,
              typename Node2,
              typename = std::enable_if_t<!std::is_same_v<Node1, Node2>>>
    [[nodiscard]] [[deprecated(
        "Use the last_node_on_path_no_checks iterator overload "
        "instead.")]] std::pair<Node1, word_type::const_iterator>
    last_node_on_path_no_checks(WordGraphView<Node1> const& wgv,
                                Node2                       source,
                                word_type const&            w) {
      static_assert(sizeof(Node2) <= sizeof(Node1));
      return last_node_on_path_no_checks(
          wgv, static_cast<Node1>(source), w.cbegin(), w.cend());
    }

    //! \copydoc last_node_on_path(WordGraphView<Node> const&, Node, Iterator,
    //! Iterator)
    //! \deprecated_warning{function} Use the \ref last_node_on_path overload
    //! with matching node types instead.
    template <typename Node1,
              typename Node2,
              typename Iterator,
              typename = std::enable_if_t<!std::is_same_v<Node1, Node2>>>
    [[deprecated("Use last_node_on_path with matching node types "
                 "instead.")]] [[nodiscard]] std::pair<Node1, Iterator>
    last_node_on_path(WordGraphView<Node1> const& wgv,
                      Node2                       source,
                      Iterator                    first,
                      Iterator                    last) {
      static_assert(sizeof(Node2) <= sizeof(Node1));
      return last_node_on_path(wgv, static_cast<Node1>(source), first, last);
    }

    //! \brief Returns the last node on the path labelled by a word and an
    //! iterator to the position in the word reached.
    //!
    //! \tparam Node the node type of the word graph view and \p source.
    //!
    //! \param wgv a word graph view.
    //! \param source the source node.
    //! \param w the word.
    //!
    //! \returns A pair consisting of the last node reached and an iterator
    //! pointing at the last letter in the word labelling an edge.
    //!
    //! \complexity
    //! At worst the distance from `w.size()`.
    //!
    //! \note If any value in \p wgv or in the word described by \p first and
    //! \p last is out of bounds (greater than or equal to
    //! WordGraphView::number_of_nodes), the path labelled by the word exits
    //! the word graph view, which is reflected in the result value of this
    //! function, but does not cause an exception to be thrown.
    //!
    //! \deprecated_warning{function} Use the \ref last_node_on_path iterator
    //! overload instead.
    template <typename Node>
    [[nodiscard]] [[deprecated(
        "Use the last_node_on_path iterator overload instead.")]] std::
        pair<Node, word_type::const_iterator>
        last_node_on_path(WordGraphView<Node> const& wgv,
                          Node                       source,
                          word_type const&           w);

    //! \copydoc last_node_on_path(WordGraphView<Node> const&, Node,
    //! word_type const&)
    //! \deprecated_warning{function} Use the \ref last_node_on_path iterator
    //! overload instead.
    template <typename Node1,
              typename Node2,
              typename = std::enable_if_t<!std::is_same_v<Node1, Node2>>>
    [[nodiscard]] [[deprecated(
        "Use the last_node_on_path iterator overload instead.")]] std::
        pair<Node1, word_type::const_iterator>
        last_node_on_path(WordGraphView<Node1> const& wgv,
                          Node2                       source,
                          word_type const&            w) {
      static_assert(sizeof(Node2) <= sizeof(Node1));
      return last_node_on_path(
          wgv, static_cast<Node1>(source), w.cbegin(), w.cend());
    }

    //! \copydoc nodes_reachable_from_no_checks(WordGraphView<Node> const&,
    //! Node, size_t)
    //! \deprecated_warning{function} Use the
    //! \ref nodes_reachable_from_no_checks overload with matching node types
    //! instead.
    template <typename Node1,
              typename Node2,
              typename = std::enable_if_t<!std::is_same_v<Node1, Node2>>>
    [[deprecated("Use nodes_reachable_from_no_checks with matching node types "
                 "instead.")]] [[nodiscard]] std::unordered_set<Node1>
    nodes_reachable_from_no_checks(WordGraphView<Node1> const& wgv,
                                   Node2                       source,
                                   size_t max_depth = POSITIVE_INFINITY) {
      static_assert(sizeof(Node2) <= sizeof(Node1));
      return nodes_reachable_from_no_checks(
          wgv, static_cast<Node1>(source), max_depth);
    }

    //! \copydoc nodes_reachable_from(WordGraphView<Node> const&, Node, size_t)
    //! \deprecated_warning{function} Use the \ref nodes_reachable_from overload
    //! with matching node types instead.
    template <typename Node1,
              typename Node2,
              typename = std::enable_if_t<!std::is_same_v<Node1, Node2>>>
    [[deprecated("Use nodes_reachable_from with matching node types "
                 "instead.")]] [[nodiscard]] std::unordered_set<Node1>
    nodes_reachable_from(WordGraphView<Node1> const& wgv,
                         Node2                       source,
                         size_t max_depth = POSITIVE_INFINITY) {
      static_assert(sizeof(Node2) <= sizeof(Node1));
      return nodes_reachable_from(wgv, static_cast<Node1>(source), max_depth);
    }

    //! \copydoc number_of_nodes_reachable_from_no_checks(
    //! WordGraphView<Node> const&, Node, size_t)
    //! \deprecated_warning{function} Use the
    //! \ref number_of_nodes_reachable_from_no_checks overload with matching
    //! node types instead.
    template <typename Node1,
              typename Node2,
              typename = std::enable_if_t<!std::is_same_v<Node1, Node2>>>
    [[deprecated("Use number_of_nodes_reachable_from_no_checks with matching "
                 "node types instead.")]] [[nodiscard]] size_t
    number_of_nodes_reachable_from_no_checks(WordGraphView<Node1> const& wgv,
                                             Node2                       source,
                                             size_t max_depth
                                             = POSITIVE_INFINITY) {
      static_assert(sizeof(Node2) <= sizeof(Node1));
      return number_of_nodes_reachable_from_no_checks(
          wgv, static_cast<Node1>(source), max_depth);
    }

    //! \copydoc number_of_nodes_reachable_from(WordGraphView<Node> const&,
    //! Node, size_t)
    //! \deprecated_warning{function} Use the
    //! \ref number_of_nodes_reachable_from overload with matching node types
    //! instead.
    template <typename Node1,
              typename Node2,
              typename = std::enable_if_t<!std::is_same_v<Node1, Node2>>>
    [[deprecated("Use number_of_nodes_reachable_from with matching node types "
                 "instead.")]] [[nodiscard]] size_t
    number_of_nodes_reachable_from(WordGraphView<Node1> const& wgv,
                                   Node2                       source,
                                   size_t max_depth = POSITIVE_INFINITY) {
      static_assert(sizeof(Node2) <= sizeof(Node1));
      return number_of_nodes_reachable_from(
          wgv, static_cast<Node1>(source), max_depth);
    }

    //! \copydoc spanning_tree_no_checks(WordGraphView<Node> const&, Node,
    //! Forest&, size_t)
    //! \deprecated_warning{function} Use the \ref spanning_tree_no_checks
    //! overload with matching node types instead.
    template <typename Node1,
              typename Node2,
              typename = std::enable_if_t<!std::is_same_v<Node1, Node2>>>
    [[deprecated(
        "Use spanning_tree_no_checks with matching node types instead.")]] void
    spanning_tree_no_checks(WordGraphView<Node1> const& wgv,
                            Node2                       root,
                            Forest&                     f,
                            size_t max_depth = POSITIVE_INFINITY) {
      static_assert(sizeof(Node2) <= sizeof(Node1));
      spanning_tree_no_checks(wgv, static_cast<Node1>(root), f, max_depth);
    }

    //! \copydoc spanning_tree_no_checks(WordGraphView<Node> const&, Node,
    //! size_t)
    //! \deprecated_warning{function} Use the \ref spanning_tree_no_checks
    //! overload with matching node types instead.
    template <typename Node1,
              typename Node2,
              typename = std::enable_if_t<!std::is_same_v<Node1, Node2>>>
    [[deprecated("Use spanning_tree_no_checks with matching node types "
                 "instead.")]] [[nodiscard]] Forest
    spanning_tree_no_checks(WordGraphView<Node1> const& wgv,
                            Node2                       root,
                            size_t max_depth = POSITIVE_INFINITY) {
      static_assert(sizeof(Node2) <= sizeof(Node1));
      return spanning_tree_no_checks(wgv, static_cast<Node1>(root), max_depth);
    }

    //! \copydoc spanning_tree(WordGraphView<Node> const&, Node, Forest&,
    //! size_t)
    //! \deprecated_warning{function} Use the \ref spanning_tree overload with
    //! matching node types instead.
    template <typename Node1,
              typename Node2,
              typename = std::enable_if_t<!std::is_same_v<Node1, Node2>>>
    [[deprecated("Use spanning_tree with matching node types instead.")]] void
    spanning_tree(WordGraphView<Node1> const& wgv,
                  Node2                       root,
                  Forest&                     f,
                  size_t                      max_depth = POSITIVE_INFINITY) {
      static_assert(sizeof(Node2) <= sizeof(Node1));
      spanning_tree(wgv, static_cast<Node1>(root), f, max_depth);
    }

    //! \copydoc spanning_tree(WordGraphView<Node> const&, Node, size_t)
    //! \deprecated_warning{function} Use the \ref spanning_tree overload with
    //! matching node types instead.
    template <typename Node1,
              typename Node2,
              typename = std::enable_if_t<!std::is_same_v<Node1, Node2>>>
    [[deprecated("Use spanning_tree with matching node types "
                 "instead.")]] [[nodiscard]] Forest
    spanning_tree(WordGraphView<Node1> const& wgv,
                  Node2                       root,
                  size_t                      max_depth = POSITIVE_INFINITY) {
      static_assert(sizeof(Node2) <= sizeof(Node1));
      return spanning_tree(wgv, static_cast<Node1>(root), max_depth);
    }

    //! \copydoc topological_sort(WordGraphView<Node> const&, Node)
    //! \deprecated_warning{function} Use the \ref topological_sort overload
    //! with matching node types instead.
    template <typename Node1,
              typename Node2,
              typename = std::enable_if_t<!std::is_same_v<Node1, Node2>>>
    [[deprecated("Use topological_sort with matching node types "
                 "instead.")]] [[nodiscard]] std::vector<Node1>
    topological_sort(WordGraphView<Node1> const& wgv, Node2 source) {
      static_assert(sizeof(Node2) <= sizeof(Node1));
      return topological_sort(wgv, static_cast<Node1>(source));
    }
  }  // namespace word_graph
}  // namespace libsemigroups

#include "libsemigroups/word-graph-view-helpers-deprecated.tpp"

#endif  // LIBSEMIGROUPS_WORD_GRAPH_VIEW_HELPERS_DEPRECATED_HPP_
