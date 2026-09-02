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

// This file contains helper functions for word graph views.
//
// The header include order is:
// 1. word-graph-class.hpp
// 2. word-graph-view-class.hpp
// 3. word-graph-view-helpers.hpp
// 4. word-graph-helpers.hpp

// TODO check whether or not various functions should be _no_checks and
// implement the "checks" version also.

#ifndef LIBSEMIGROUPS_WORD_GRAPH_VIEW_HELPERS_HPP_
#define LIBSEMIGROUPS_WORD_GRAPH_VIEW_HELPERS_HPP_

#include <algorithm>      // for fill
#include <queue>          // for queue
#include <stack>          // for stack
#include <stddef.h>       // for size_t
#include <stdint.h>       // for uint8_t
#include <string>         // for string
#include <tuple>          // for tie
#include <type_traits>    // for decay_t, enable_if_t, is_same_v
#include <unordered_set>  // for unordered_set
#include <utility>        // for pair
#include <vector>         // for vector

#include "libsemigroups/config.hpp"     // for LIBSEMIGROUPS_EIGEN_ENABLED
#include "libsemigroups/constants.hpp"  // for POSITIVE_INFINITY, UNDEFINED
#include "libsemigroups/debug.hpp"      // for LIBSEMIGROUPS_ASSERT
#include "libsemigroups/dot.hpp"        // for Dot
#include "libsemigroups/exception.hpp"  // for LIBSEMIGROUPS_EXCEPTION
#include "libsemigroups/forest.hpp"     // for Forest
#include "libsemigroups/order.hpp"      // for Order
#include "libsemigroups/types.hpp"      // for word_type, letter_type
#include "libsemigroups/word-graph-class.hpp"       // for WordGraph
#include "libsemigroups/word-graph-view-class.hpp"  // for WordGraphView

#include "libsemigroups/detail/fmt.hpp"  // for fmt::format
#include "libsemigroups/detail/stl.hpp"  // for HasLessEqual
#include "libsemigroups/detail/uf.hpp"   // for Duf

#ifdef LIBSEMIGROUPS_EIGEN_ENABLED
#include "detail/eigen.hpp"
#else
#include "matrix.hpp"
#endif

namespace libsemigroups {
  // TODO(v4) rename -> word_graph_view for better separation, this was too
  // much trouble in v3, so we leave it to v4.
  namespace word_graph {

    //////////////////////////////////////////////////////////////////////////
    // WordGraphView - helper functions - in alphabetical order!!!
    //////////////////////////////////////////////////////////////////////////

    //! \brief Returns the adjacency matrix of a word graph view.
    //!
    //! This function returns the adjacency matrix of the word graph view
    //! \p wgv. The type of the returned matrix depends on whether or not
    //! `libsemigroups` is compiled with [eigen][] enabled. The returned
    //! matrix has the number of edges with source \c s and target \c t in the
    //! `(s, t)`-entry.
    //!
    //! \tparam Node  the type of the nodes of the WordGraphView.
    //!
    //! \param wgv the word graph view.
    //!
    //! \returns The adjacency matrix.
    //!
    //! [eigen]: http://eigen.tuxfamily.org/
    template <typename Node>
    [[nodiscard]] auto adjacency_matrix(WordGraphView<Node> const& wgv);

    //! \brief Returns the std::unordered_set of nodes that can reach a given
    //! node in a word graph.
    //!
    //! This function returns a std::unordered_set consisting of all the nodes
    //! in the word graph \p wgv that can reach \p target. This function can be
    //! thought of like an inverse of `nodes_reachable_from`, in the sense
    //! that the node `a` \f$\in\f$ `ancestor_of(b)` for some node `b` if and
    //! only if `b` \f$\in\f$ `nodes_reachable_from(a)`.
    //!
    //! \tparam Node the node type of the word graph and \p target.
    //!
    //! \param wgv the word graph.
    //! \param target the target node.
    //!
    //! \returns A std::unordered_set consisting of all the nodes in the word
    //! graph \p wgv that can reach \p target.
    //!
    //! \warning The arguments are not checked, and in particular it is
    //! assumed that \p target is a node of \p wgv (i.e. less than
    //! WordGraphView::number_of_nodes).
    template <typename Node>
    [[nodiscard]] std::unordered_set<Node>
    ancestors_of_no_checks(WordGraphView<Node> const& wgv, Node target);

    template <typename Node1,
              typename Node2,
              typename = std::enable_if_t<!std::is_same_v<Node1, Node2>>>
    [[deprecated]] [[nodiscard]] std::unordered_set<Node1>
    ancestors_of_no_checks(WordGraphView<Node1> const& wgv, Node2 target) {
      static_assert(sizeof(Node2) <= sizeof(Node1));
      return ancestors_of_no_checks(wgv, static_cast<Node1>(target));
    }

    //! \brief Returns the std::unordered_set of nodes that can reach a given
    //! node in a word graph.
    //!
    //! This function returns a std::unordered_set consisting of all the nodes
    //! in the word graph \p wgv that can reach \p target. This function can be
    //! thought of like an inverse of `nodes_reachable_from`, in the sense
    //! that the node `a` \f$\in\f$ `ancestor_of(b)` for some node `b` if and
    //! only if `b` \f$\in\f$ `nodes_reachable_from(a)`.
    //!
    //! \tparam Node the node type of the word graph and \p target.
    //!
    //! \param wgv the word graph.
    //! \param target the target node.
    //!
    //! \returns A std::unordered_set consisting of all the nodes in the word
    //! graph \p wgv that can reach \p target.
    //!
    //! \throws LibsemigroupsException if \p target is out of bounds (greater
    //! than or equal to WordGraphView::number_of_nodes).
    template <typename Node>
    [[nodiscard]] std::unordered_set<Node>
    ancestors_of(WordGraphView<Node> const& wgv, Node target);

    template <typename Node1,
              typename Node2,
              typename = std::enable_if_t<!std::is_same_v<Node1, Node2>>>
    [[deprecated]] [[nodiscard]] std::unordered_set<Node1>
    ancestors_of(WordGraphView<Node1> const& wgv, Node2 target) {
      static_assert(sizeof(Node2) <= sizeof(Node1));
      return ancestors_of(wgv, static_cast<Node1>(target));
    }

    //! \brief Returns a \ref Dot object representing a word graph view.
    //!
    //! This function returns a \ref Dot object representing the word graph
    //! view \p wgv.
    //!
    //! \tparam Node  the type of the nodes of the WordGraphView.
    //!
    //! \param wgv the word graph.
    //!
    //! \returns A \ref Dot object.
    template <typename Node>
    [[nodiscard]] Dot dot(WordGraphView<Node> const& wgv);

    //! \brief Returns a labelled \ref Dot object representing a word graph
    //! view.
    //!
    //! This function returns a \ref Dot object representing the word graph
    //! view \p wgv. The nodes of the returned graph are labelled using
    //! \p node_labels, and the colours used for edges are shown in a legend
    //! labelled by \p edge_labels.
    //!
    //! \tparam Node the type of the nodes of the WordGraphView.
    //!
    //! \param wgv the word graph view.
    //! \param node_labels the labels for the nodes of \p wgv.
    //! \param edge_labels the labels for the edge labels of \p wgv.
    //!
    //! \returns A \ref Dot object representing \p wgv.
    //!
    //! \throws LibsemigroupsException if \p node_labels has size different
    //! from `wgv.number_of_nodes()`.
    //! \throws LibsemigroupsException if \p edge_labels has size different
    //! from `wgv.out_degree()`.
    //! \throws LibsemigroupsException if the out-degree of \p wgv is greater
    //! than the number of colours in Dot::colors.
    //!
    //! \note This function does not trigger an enumeration.
    template <typename Node>
    [[nodiscard]] Dot dot(WordGraphView<Node> const&      wgv,
                          std::vector<std::string> const& node_labels,
                          std::vector<std::string> const& edge_labels);

    //! \brief Compares two word graph views on a range of nodes.
    //!
    //! This function returns \c true if the word graph views \p x and \p y are
    //! equal on the nodes specified by the range from \p first to \p last;
    //! and \c false otherwise. The views \p x and \p y are equal at a node
    //! \c s if:
    //! * the numbers of nodes in \p x and \p y coincide;
    //! * the out-degrees of \p x and \p y coincide;
    //! * the edges with source \c s and label \c a have equal targets in \p x
    //! and \p y for every label \c a.
    //!
    //! \tparam Node the type of the nodes of the WordGraphView.
    //! \tparam Iterator the type of the iterators specifying the nodes to
    //! compare.
    //!
    //! \param x the first word graph view for comparison.
    //! \param y the second word graph view for comparison.
    //! \param first an iterator pointing at the first node to compare.
    //! \param last an iterator pointing one past the last node to compare.
    //!
    //! \returns Whether or not the word graph views are equal on the specified
    //! range of nodes.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \warning No checks are performed to ensure that the arguments
    //! are valid.
    //!
    //! \sa WordGraph::operator== for a comparison of two entire word graphs.
    template <typename Node, typename Iterator>
    [[nodiscard]] bool equal_to_no_checks(WordGraphView<Node> const& x,
                                          WordGraphView<Node> const& y,
                                          Iterator                   first,
                                          Iterator                   last);

    //! \brief Compares two word graph views.
    //!
    //! This function returns \c true if the word graph views \p x and \p y
    //! have the same number of nodes and out-degree, and every edge has the
    //! same target in both views; and \c false otherwise.
    //!
    //! \tparam Node the type of the nodes of the WordGraphView.
    //!
    //! \param x the first word graph view for comparison.
    //! \param y the second word graph view for comparison.
    //!
    //! \returns Whether or not the word graph views are equal.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \warning No checks are performed to ensure that the arguments are
    //! valid.
    //!
    //! \sa WordGraphView::operator== for a comparison using the equality
    //! operator.
    template <typename Node>
    [[nodiscard]] bool equal_to_no_checks(WordGraphView<Node> const& x,
                                          WordGraphView<Node> const& y) {
      return equal_to_no_checks(
          x, y, x.cbegin_nodes_no_checks(), x.cend_nodes_no_checks());
    }

    //! \brief Compares two word graph views on a range of nodes.
    //!
    //! This function returns \c true if the word graph views \p x and \p y are
    //! equal on the nodes specified by the range from \p first to \p last;
    //! and \c false otherwise. The views \p x and \p y are equal at a node
    //! \c s if:
    //! * the numbers of nodes in \p x and \p y coincide;
    //! * the out-degrees of \p x and \p y coincide;
    //! * the edges with source \c s and label \c a have equal targets in \p x
    //! and \p y for every label \c a.
    //!
    //! \tparam Node the type of the nodes of the WordGraphView.
    //! \tparam Iterator the type of the iterators specifying the nodes to
    //! compare.
    //!
    //! \param x the first word graph view for comparison.
    //! \param y the second word graph view for comparison.
    //! \param first an iterator pointing at the first node to compare.
    //! \param last an iterator pointing one past the last node to compare.
    //!
    //! \returns Whether or not the word graph views are equal on the specified
    //! nodes.
    //!
    //! \throws LibsemigroupsException if either word graph view is invalid or
    //! if any node in the range from \p first to \p last is not a node of
    //! \p x.
    //!
    //! \sa WordGraphView::operator== for a comparison of two entire word graph
    //! views.
    template <typename Node, typename Iterator>
    [[nodiscard]] bool equal_to(WordGraphView<Node> const& x,
                                WordGraphView<Node> const& y,
                                Iterator                   first,
                                Iterator                   last);

    //! \brief Compares two word graph views.
    //!
    //! This function returns \c true if the word graph views \p x and \p y
    //! have the same number of nodes and out-degree, and every edge has the
    //! same target in both views; and \c false otherwise.
    //!
    //! \tparam Node the type of the nodes of the WordGraphView.
    //!
    //! \param x the first word graph view for comparison.
    //! \param y the second word graph view for comparison.
    //!
    //! \returns Whether or not the word graph views are equal.
    //!
    //! \throws LibsemigroupsException if either word graph view is invalid.
    //!
    //! \sa WordGraphView::operator== for a comparison using the equality
    //! operator.
    template <typename Node>
    [[nodiscard]] bool equal_to(WordGraphView<Node> const& x,
                                WordGraphView<Node> const& y) {
      return equal_to(
          x, y, x.cbegin_nodes_no_checks(), x.cend_nodes_no_checks());
    }

    //! \brief Follow the path from a specified node labelled by a word.
    //!
    //! This function returns the last node on the path in the word graph view
    //! \p wgv starting at the node \p from labelled by the word defined by
    //! \p first and \p last or \ref UNDEFINED.
    //!
    //! \tparam Node the type of the nodes of the WordGraphView \p wgv and the
    //! node \p from.
    //!
    //! \param wgv a word graph view.
    //! \param from the source node.
    //! \param first iterator into a word.
    //! \param last iterator into a word.
    //!
    //! \returns A value of type \p Node.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! At worst the distance from \p first to \p last.
    //!
    //! \warning
    //! No checks on the arguments of this function are performed.
    template <typename Node, typename Iterator>
    [[nodiscard]] Node follow_path_no_checks(WordGraphView<Node> const& wgv,
                                             Node                       from,
                                             Iterator                   first,
                                             Iterator last) noexcept;

    template <typename Node1,
              typename Node2,
              typename Iterator,
              typename = std::enable_if_t<!std::is_same_v<Node1, Node2>>>
    [[deprecated]] [[nodiscard]] Node1
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
    template <typename Node>
    [[deprecated]] [[nodiscard]] Node
    follow_path_no_checks(WordGraphView<Node> const& wgv,
                          Node                       from,
                          word_type const&           path) noexcept {
      return follow_path_no_checks(wgv, from, path.cbegin(), path.cend());
    }

    template <typename Node1,
              typename Node2,
              typename = std::enable_if_t<!std::is_same_v<Node1, Node2>>>
    [[deprecated]] [[nodiscard]] Node1
    follow_path_no_checks(WordGraphView<Node1> const& wgv,
                          Node2                       from,
                          word_type const&            path) noexcept {
      static_assert(sizeof(Node2) <= sizeof(Node1));
      return follow_path_no_checks(
          wgv, static_cast<Node1>(from), path.cbegin(), path.cend());
    }

    //! \brief Find the node that a path starting at a given node leads to (if
    //! any).
    //!
    //! This function attempts to follow the path in the word graph view
    //! \p wgv starting at the node \p from  labelled by the word defined by
    //! \p first and \p last. If this path exists, then the last node on that
    //! path is returned. If this path does not exist, then \ref UNDEFINED is
    //! returned.
    //!
    //! \tparam Node the type of the nodes of the WordGraphView \p wgv and the
    //! node \p source.
    //! \tparam Iterator the type of \p first and \p last.
    //!
    //! \param wgv a word graph view.
    //! \param source the starting node.
    //! \param first an iterator point at the start of the word.
    //! \param last an iterator point one beyond the last letter of the word.
    //!
    //! \returns
    //! A value of type \p Node. If one or more edges in \p path are not
    //! defined, then \ref UNDEFINED is returned.
    //!
    //! \throw LibsemigroupsException if \p from is not a node in the word
    //! graph or the word defined by \p first and \p last contains a value
    //! that is not an edge-label.
    //!
    //! \par Complexity
    //! Linear in the distance between \p first and \p last.
    template <typename Node, typename Iterator>
    [[nodiscard]] Node follow_path(WordGraphView<Node> const& wgv,
                                   Node                       source,
                                   Iterator                   first,
                                   Iterator                   last);

    template <typename Node1,
              typename Node2,
              typename Iterator,
              typename = std::enable_if_t<!std::is_same_v<Node1, Node2>>>
    [[deprecated]] [[nodiscard]] Node1
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
    // TODO(2) example
    // not noexcept because WordGraph::target isn't
    template <typename Node>
    [[deprecated]] [[nodiscard]] Node
    follow_path(WordGraphView<Node> const& wgv,
                Node                       from,
                word_type const&           path) {
      return follow_path(wgv, from, path.cbegin(), path.cend());
    }

    template <typename Node1,
              typename Node2,
              typename = std::enable_if_t<!std::is_same_v<Node1, Node2>>>
    [[deprecated]] [[nodiscard]] Node1
    follow_path(WordGraphView<Node1> const& wgv,
                Node2                       from,
                word_type const&            path) {
      static_assert(sizeof(Node2) <= sizeof(Node1));
      return follow_path(
          wgv, static_cast<Node1>(from), path.cbegin(), path.cend());
    }

    //! \brief Check if a word graph view is acyclic.
    //!
    //! This function returns \c true if the word graph view \p wgv is acyclic
    //! and
    //! \c false otherwise. A word graph is acyclic if every directed cycle in
    //! the word graph is trivial.
    //!
    //! \tparam Node  the type of the nodes of the WordGraphView.
    //!
    //! \param wgv the WordGraphView object to check.
    //!
    //! \returns
    //! A value of type `bool`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \par Complexity
    //! \f$O(m + n)\f$ where \f$m\f$ is the number of nodes in the
    //! WordGraphView \p wgv and \f$n\f$ is the number of edges. Note that for
    //! WordGraphView objects the number of edges is always at most \f$mk\f$
    //! where \f$k\f$ is the \ref WordGraphView::out_degree.
    //!
    //!
    //! \par Example
    //! \code
    //! WordGraph<size_t> wgv;
    //! wgv.add_nodes(2);
    //! wgv.add_to_out_degree(1);
    //! wgv.target(0, 0, 1);
    //! wgv.target(1, 0, 0);
    //! word_graph::is_acyclic(WordGraphView<size_t>(wgv)); // returns false
    //! \endcode
    // Not noexcept because detail::is_acyclic isn't
    template <typename Node>
    [[nodiscard]] bool is_acyclic(WordGraphView<Node> const& wgv);

    //! \brief Check if the word graph view induced by the nodes reachable
    //! from a source node is acyclic.
    //!
    //! This function returns \c true if the word graph view consisting of the
    //! nodes reachable from \p source in the word graph view \p wgv is
    //! acyclic and \c false if not. A word graph view is acyclic if every
    //! directed cycle in the word graph is trivial.
    //!
    //! \tparam Node the type of the nodes of the WordGraphView \p wgv and the
    //! node \p source.
    //!
    //! \param wgv the WordGraphView object to check.
    //! \param source the source node.
    //!
    //! \returns
    //! A value of type `bool`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \par Complexity
    //! \f$O(m + n)\f$ where \f$m\f$ is the number of nodes in the
    //! WordGraphView \p wgv and \f$n\f$ is the number of edges. Note that for
    //! WordGraphView objects the number of edges is always at most \f$mk\f$
    //! where \f$k\f$ is the \ref WordGraphView::out_degree.
    //!
    //!
    //! \par Example
    //! \code
    //! WordGraph<size_t> g;
    //! g.add_nodes(4);
    //! g.add_to_out_degree(1);
    //! g.target(0, 0, 1);
    //! g.target(1, 0, 0);
    //! g.target(2, 0, 3);
    //! WordGraphView<size_t> wgv(g);
    //! word_graph::is_acyclic(wgv); // returns false
    //! word_graph::is_acyclic(wgv, size_t(0)); // returns false
    //! word_graph::is_acyclic(wgv, size_t(1)); // returns false
    //! word_graph::is_acyclic(wgv, size_t(2)); // returns true
    //! word_graph::is_acyclic(wgv, size_t(3)); // returns true
    //! \endcode
    // Not noexcept because detail::is_acyclic isn't
    template <typename Node>
    [[nodiscard]] bool is_acyclic(WordGraphView<Node> const& wgv, Node source);

    template <typename Node1,
              typename Node2,
              typename = std::enable_if_t<!std::is_same_v<Node1, Node2>>>
    [[deprecated]] [[nodiscard]] bool
    is_acyclic(WordGraphView<Node1> const& wgv, Node2 source) {
      static_assert(sizeof(Node2) <= sizeof(Node1));
      return is_acyclic(wgv, static_cast<Node1>(source));
    }

    //! \brief Check if the word graph view induced by the nodes reachable
    //! from a source node and from which a target node can be reached is
    //! acyclic.
    //!
    //! This function returns \c true if the word graph view consisting of the
    //! nodes reachable from \p source and from which \p target is reachable,
    //! in the word graph \p wgv, is acyclic; and \c false if not. A word
    //! graph view is acyclic if every directed cycle of the word graph is
    //! trivial.
    //!
    //! \tparam Node the type of the nodes of the WordGraphView \p wgv and the
    //! nodes \p source and \p target.
    //!
    //! \param wgv the WordGraphView object to check.
    //! \param source the source node.
    //! \param target the target node.
    //!
    //! \returns
    //! A value of type `bool`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \par Complexity
    //! \f$O(m + n)\f$ where \f$m\f$ is the number of nodes in the
    //! WordGraphView \p wgv and \f$n\f$ is the number of edges. Note that for
    //! WordGraphView objects the number of edges is always at most \f$mk\f$
    //! where \f$k\f$ is the \ref WordGraphView::out_degree.
    // Not noexcept because detail::is_acyclic isn't
    template <typename Node>
    [[nodiscard]] bool is_acyclic(WordGraphView<Node> const& wgv,
                                  Node                       source,
                                  Node                       target);

    template <typename Node1,
              typename Node2,
              typename = std::enable_if_t<!std::is_same_v<Node1, Node2>>>
    [[deprecated]] [[nodiscard]] bool
    is_acyclic(WordGraphView<Node1> const& wgv, Node2 source, Node2 target) {
      static_assert(sizeof(Node2) <= sizeof(Node1));
      return is_acyclic(
          wgv, static_cast<Node1>(source), static_cast<Node1>(target));
    }

    //! \brief Check if a word graph view is compatible with some relations at
    //! a range of nodes.
    //!
    //! This function returns \c true if the word graph view \p wgv is
    //! compatible with the relations in the range \p first_rule to
    //! \p last_rule at every node in the range from \p first_node to
    //! \p last_node. This means that the paths with given sources that are
    //! labelled by one side of a relation leads to the same node as the path
    //! labelled by the other side of the relation.
    //!
    //! \tparam Node  the type of the nodes of the WordGraphView
    //! \p wgv.
    //!
    //! \tparam Iterator1 the type of \p first_node.
    //!
    //! \tparam Iterator2 the type of \p last_node.
    //!
    //! \tparam Iterator3 the type of \p first_rule and \p last_rule.
    //!
    //! \param wgv the word graph view.
    //!
    //! \param first_node iterator pointing at the first node.
    //!
    //! \param last_node iterator pointing at one beyond the last node.
    //!
    //! \param first_rule iterator pointing to the first rule.
    //!
    //! \param last_rule iterator pointing one beyond the last rule.
    //!
    //! \return Whether or not the word graph view is compatible with the
    //! given rules at each one of the given nodes.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \note This function ignores out of bound targets in \p wgv (if any).
    //!
    //! \warning
    //! No checks on the arguments of this function are performed.
    // TODO(1) add a version of this function with one that returns a float
    // representing the proportion of the nodes in the range that are
    // compatible with the rules. Don't replace the current version because it
    // can return early knowing that it isn't compatible.
    // TODO(v4) deprecate, and use Iterator, Sentinel rather than Iterator1/2/3
    template <typename Node,
              typename Iterator1,
              typename Iterator2,
              typename Iterator3>
    [[nodiscard]] bool is_compatible_no_checks(WordGraphView<Node> const& wgv,
                                               Iterator1 first_node,
                                               Iterator2 last_node,
                                               Iterator3 first_rule,
                                               Iterator3 last_rule);

    //! \brief Check if a word graph view is compatible with a pair of words
    //! for a range of nodes.
    //!
    //! This function returns \c true if the word graph view \p wgv is
    //! compatible with the words \p lhs and \p rhs at every node in the range
    //! from
    //! \p first_node to \p last_node. This means that the paths with given
    //! sources that are labelled by \p lhs leads to the same node as the path
    //! labelled by \p rhs.
    //!
    //! \tparam Node the type of the nodes of the WordGraphView \p wgv.
    //! \tparam Iterator1 the type of \p first_node.
    //! \tparam Iterator2 the type of \p last_node.
    //!
    //! \param wgv the word graph view.
    //! \param first_node iterator pointing at the first node.
    //! \param last_node iterator pointing at one beyond the last node.
    //! \param lhs the first rule.
    //! \param rhs the second rule.
    //!
    //! \return Whether or not the word graph view is compatible with the
    //! given rules at each one of the given nodes.
    //!
    //! \note This function ignores out of bound targets in \p wgv (if any).
    //!
    //! \warning This function does not check that its arguments are valid.
    // TODO(v4) deprecate, but first implement the iterator version
    template <typename Node, typename Iterator1, typename Iterator2>
    [[nodiscard]] bool is_compatible_no_checks(WordGraphView<Node> const& wgv,
                                               Iterator1        first_node,
                                               Iterator2        last_node,
                                               word_type const& lhs,
                                               word_type const& rhs);

    //! \brief Check if a word graph view is compatible with some relations at
    //! a range of nodes.
    //!
    //! This function returns \c true if the word graph view \p wgv is
    //! compatible with the relations in the range \p first_rule to
    //! \p last_rule at every node in the range from \p first_node to
    //! \p last_node. This means that the paths with given sources that are
    //! labelled by one side of a relation leads to the same node as the path
    //! labelled by the other side of the relation.
    //!
    //! \tparam Node the type of the nodes of the WordGraphView.
    //! \p wgv.
    //!
    //! \tparam Iterator1 the type of \p first_node.
    //!
    //! \tparam Iterator2 the type of \p last_node.
    //!
    //! \tparam Iterator3 the type of \p first_rule and \p last_rule.
    //!
    //! \param wgv the word graph view.
    //!
    //! \param first_node iterator pointing at the first node.
    //!
    //! \param last_node iterator pointing at one beyond the last node.
    //!
    //! \param first_rule iterator pointing to the first rule.
    //!
    //! \param last_rule iterator pointing one beyond the last rule.
    //!
    //! \return Whether or not the word graph view is compatible with the
    //! given rules at each one of the given nodes.
    //!
    //! \throws LibsemigroupsException if any of the nodes in the range
    //! between \p first_node and \p last_node does not belong to \p wgv (i.e.
    //! is greater than or equal to WordGraphView::number_of_nodes).
    //!
    //! \throws LibsemigroupsException if any of the rules in the range
    //! between \p first_rule and \p last_rule contains an invalid label (i.e.
    //! one greater than or equal to WordGraphView::out_degree).
    //!
    //! \note This function ignores out of bound targets in \p wgv (if any).
    // TODO(v4) remove enable_if_t, it should become redundant when we remove
    // overloads for word_type from here
    template <typename Node,
              typename Iterator1,
              typename Iterator2,
              typename Iterator3,
              typename = std::enable_if_t<
                  !std::is_same_v<std::decay_t<Iterator3>, word_type>>>
    [[nodiscard]] bool is_compatible(WordGraphView<Node> const& wgv,
                                     Iterator1                  first_node,
                                     Iterator2                  last_node,
                                     Iterator3                  first_rule,
                                     Iterator3                  last_rule);

    //! \brief Check if a word graph view is compatible with a pair of words
    //! for a range of nodes.
    //!
    //! This function returns \c true if the word graph view \p wgv is
    //! compatible with the words \p lhs and \p rhs at every node in the range
    //! from
    //! \p first_node to \p last_node. This means that the paths with given
    //! sources that are labelled by \p lhs leads to the same node as the path
    //! labelled by \p rhs.
    //!
    //! \tparam Node the type of the nodes of the WordGraphView \p wgv.
    //!
    //! \tparam Iterator1 the type of \p first_node.
    //!
    //! \tparam Iterator2 the type of \p last_node.
    //!
    //! \param wgv the word graph view.
    //!
    //! \param first_node iterator pointing at the first node.
    //!
    //! \param last_node iterator pointing at one beyond the last node.
    //!
    //! \param lhs the first rule.
    //!
    //! \param rhs the second rule.
    //!
    //! \return Whether or not the word graph view is compatible with the
    //! given rules at each one of the given nodes.
    //!
    //! \throws LibsemigroupsException if any of the nodes in the range
    //! between
    //! \p first_node and \p last_node does not belong to \p wgv (i.e. is
    //! greater than or equal to WordGraphView::number_of_nodes).
    //!
    //! \throws LibsemigroupsException if any of the rules in the range
    //! between
    //! \p first_rule and \p last_rule contains an invalid label (i.e. one
    //! greater than or equal to WordGraphView::out_degree).
    // TODO(v4) deprecate, after implementing an iterator version
    template <typename Node, typename Iterator1, typename Iterator2>
    [[nodiscard]] bool is_compatible(WordGraphView<Node> const& wgv,
                                     Iterator1                  first_node,
                                     Iterator2                  last_node,
                                     word_type const&           lhs,
                                     word_type const&           rhs);

    //! \brief Check if every node in a range has exactly
    //! WordGraphView::out_degree out-edges.
    //!
    //! This function returns \c true if every node in the range defined by
    //! \p first_node and \p last_node is complete, meaning that every such
    //! node is the source of an edge with every possible label.
    //!
    //! \tparam Node the type of the nodes in the word graph view.
    //! \tparam Iterator1 the type of \p first_node.
    //! \tparam Iterator2 the type of \p last_node.
    //!
    //! \param wgv the word graph view.
    //! \param first_node iterator pointing to the first node in the range.
    //! \param last_node iterator pointing one beyond the last node in the
    //! range.
    //!
    //! \returns
    //! Whether or not the word graph view is complete on the given range of
    //! nodes.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! \f$O(mn)\f$ where \c m is the number of nodes in the range and \c n is
    //! out_degree().
    //!
    //! \warning No checks are performed on the arguments.
    template <typename Node, typename Iterator1, typename Iterator2>
    [[nodiscard]] bool is_complete_no_checks(WordGraphView<Node> const& wgv,
                                             Iterator1 first_node,
                                             Iterator2 last_node);

    //! \brief Check if every node has exactly WordGraphView::out_degree
    //! out-edges.
    //!
    //! This function returns \c true if a WordGraphView is complete, meaning
    //! that every node is the source of an edge with every possible label.
    //!
    //! \tparam Node the type of the nodes in the word graph view.
    //!
    //! \param wgv the word graph view.
    //!
    //! \returns
    //! Whether or not the word graph view is complete.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! \f$O(mn)\f$ where \c m is number_of_nodes() and \c n is out_degree().
    template <typename Node>
    [[nodiscard]] bool
    is_complete_no_checks(WordGraphView<Node> const& wgv) noexcept {
      return wgv.number_of_edges_no_checks()
             == wgv.number_of_nodes_no_checks() * wgv.out_degree_no_checks();
    }

    //! \brief Check if every node in a range has exactly
    //! WordGraphView::out_degree out-edges.
    //!
    //! This function returns \c true if every node in the range defined by
    //! \p first_node and \p last_node is complete, meaning that
    //! every such node is the source of an edge with every possible label.
    //!
    //! \tparam Node the type of the nodes in the word graph view.
    //! \tparam Iterator1 the type of \p first_node.
    //! \tparam Iterator2 the type of \p last_node.
    //!
    //! \param wgv the word graph view.
    //! \param first_node iterator pointing to the first node in the range.
    //! \param last_node iterator pointing one beyond the last node in the
    //! range.
    //!
    //! \returns Whether or not the word graph view is complete on the given
    //! range of nodes.
    //!
    //! \throws LibsemigroupsException if any item in the range defined by
    //! \p first_node and \p last_node is not a node of \p wgv.
    //!
    //! \complexity
    //! \f$O(mn)\f$ where \c m is the number of nodes in the range and \c n is
    //! out_degree().
    template <typename Node, typename Iterator1, typename Iterator2>
    [[nodiscard]] bool is_complete(WordGraphView<Node> const& wgv,
                                   Iterator1                  first_node,
                                   Iterator2                  last_node);

    // TODO doc?
    template <typename Node>
    [[nodiscard]] bool is_complete(WordGraphView<Node> const& wgv) {
      wgv.throw_if_invalid_view();
      return is_complete_no_checks(wgv);
    }

    //! \brief Check if a word graph view is connected.
    //!
    //! This function returns \c true if the word graph view \p wgv is
    //! connected and
    //! \c false if it is not. A word graph view is *connected* if for every
    //! pair of nodes \c s and \c t in the graph there exists a sequence
    //! \f$u_0 = s,
    //! \ldots, u_{n}= t\f$ for some \f$n\in \mathbb{N}\f$ such that for every
    //! \f$i\f$ there exists a label \c a such that \f$(u_i, a, u_{i + 1})\f$
    //! or
    //! \f$(u_{i + 1}, a, u_i)\f$ is an edge in the graph.
    //!
    //! \tparam Node the type of the nodes in the word graph view.
    //!
    //! \param wgv the word graph view.
    //!
    //! \returns
    //! Whether or not the word graph view is connected.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \note If any target of any edge in the word graph view \p wgv that is
    //! out of bounds, then this is ignored by this function.
    template <typename Node>
    [[nodiscard]] bool is_connected(WordGraphView<Node> const& wgv);

    //! \brief Check if there is a path from one node to another.
    //!
    //! This function returns \c true if there is a path from the node
    //! \p source to the node \p target in the word graph view \p wgv.
    //!
    //! \tparam Node the type of the nodes in the WordGraphView and the nodes
    //! \p source and \p target.
    //!
    //! \param wgv the WordGraphView object to check.
    //! \param source the source node.
    //! \param target the source node.
    //!
    //! \returns
    //! Whether or not the node \p target is reachable from the node \p source
    //! in the word graph view \p wgv.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \par Complexity
    //! \f$O(m + n)\f$ where \f$m\f$ is the number of nodes in the
    //! WordGraphView \p wgv and \f$n\f$ is the number of edges. Note that for
    //! WordGraphView objects the number of edges is always at most \f$mk\f$
    //! where \f$k\f$ is the WordGraphView::out_degree.
    //!
    //! \note
    //! If \p source and \p target are equal, then, by convention, we consider
    //! \p target to be reachable from \p source, via the empty path.
    //!
    //! \note This function ignores out of bound targets in \p wgv (if any).
    //!
    //! \warning No checks are performed on the arguments.
    //!
    //! \par Example
    //! \code
    //! WordGraph<size_t> wg;
    //! wg.add_nodes(4);
    //! wg.add_to_out_degree(1);
    //! wg.target(0, 0, 1);
    //! wg.target(1, 0, 0);
    //! wg.target(2, 0, 3);
    //! WordGraphView<size_t> wgv(wg);
    //! word_graph::is_reachable_no_checks(wgv, size_t(0), size_t(1));
    //! word_graph::is_reachable_no_checks(wgv, size_t(1), size_t(0));
    //! word_graph::is_reachable_no_checks(wgv, size_t(1), size_t(2));
    //! word_graph::is_reachable_no_checks(wgv, size_t(2), size_t(3));
    //! word_graph::is_reachable_no_checks(wgv, size_t(3), size_t(2));
    //! \endcode
    template <typename Node>
    [[nodiscard]] bool is_reachable_no_checks(WordGraphView<Node> const& wgv,
                                              Node                       source,
                                              Node target);

    template <typename Node1,
              typename Node2,
              typename = std::enable_if_t<!std::is_same_v<Node1, Node2>>>
    [[deprecated]] [[nodiscard]] bool
    is_reachable_no_checks(WordGraphView<Node1> const& wgv,
                           Node2                       source,
                           Node2                       target) {
      static_assert(sizeof(Node2) <= sizeof(Node1));
      return is_reachable_no_checks(
          wgv, static_cast<Node1>(source), static_cast<Node1>(target));
    }

    //! \brief Check if there is a path from one node to another.
    //!
    //! This function returns \c true if there is a path from the node
    //! \p source to the node \p target in the word graph view \p wgv.
    //!
    //! \tparam Node the type of the nodes in the WordGraphView and the nodes
    //! \p source and \p target.
    //!
    //! \param wgv the WordGraphView object to check.
    //! \param source the source node.
    //! \param target the source node.
    //!
    //! \returns
    //! Whether or not the node \p target is reachable from the node \p source
    //! in the word graph view \p wgv.
    //!
    //! \throws LibsemigroupsException if \p source or \p target is out of
    //! bounds.
    //! \throws LibsemigroupsException if any target in \p wgv is out of
    //! bounds.
    //!
    //! \par Complexity
    //! \f$O(m + n)\f$ where \f$m\f$ is the number of nodes in the
    //! WordGraphView \p wgv and \f$n\f$ is the number of edges. Note that for
    //! WordGraphView objects the number of edges is always at most \f$mk\f$
    //! where \f$k\f$ is the WordGraphView::out_degree.
    //!
    //! \note
    //! If \p source and \p target are equal, then, by convention, we consider
    //! \p target to be reachable from \p source, via the empty path.
    template <typename Node>
    [[nodiscard]] bool is_reachable(WordGraphView<Node> const& wgv,
                                    Node                       source,
                                    Node                       target);

    template <typename Node1,
              typename Node2,
              typename = std::enable_if_t<!std::is_same_v<Node1, Node2>>>
    [[deprecated]] [[nodiscard]] bool
    is_reachable(WordGraphView<Node1> const& wgv, Node2 source, Node2 target) {
      static_assert(sizeof(Node2) <= sizeof(Node1));
      return is_reachable(
          wgv, static_cast<Node1>(source), static_cast<Node1>(target));
    }

    //! \brief Check if a word graph is standardized.
    //!
    //! This function checks if the word graph \p wgv is standardized
    //! according to the reduction order specified by \p cmp.
    //!
    //! \tparam Node the type of the node in \p wgv.
    //! \tparam Cmp the type of the comparator \p cmp.
    //!
    //! \param wgv the word graph to check.
    //! \param cmp the order to use for standardization check.
    //!
    //! \sa
    //! standardize.
    // TODO(1): Add is_standardized_no_checks?
    // TODO(1): is this variant not the _no_checks version? What happens when
    // you give it a malformed wgv?
    template <typename Node, typename Cmp>
    [[nodiscard]] bool is_standardized(WordGraphView<Node> const& wgv,
                                       Cmp&&                      cmp);

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

    //! \brief Check if every node is reachable from some node.
    //!
    //! This function returns \c true if there exists a node in \p wgv from
    //! which every other node is reachable; and \c false otherwise.
    //! A word graph view is *strictly cyclic* if there exists a node \f$v\f$
    //! from which every node is reachable (including \f$v\f$). There must be
    //! a path of length at least \f$1\f$ from the original node \f$v\f$ to
    //! itself (i.e. \f$v\f$ is not considered to be reachable from itself by
    //! default).
    //!
    //! \tparam Node the type of the nodes of the WordGraphView.
    //!
    //! \param wgv the WordGraphView object to check.
    //!
    //! \returns
    //! A value of type `bool`.
    //!
    //! \par Complexity
    //! \f$O(m + n)\f$ where \f$m\f$ is the number of nodes in the
    //! WordGraphView \p wgv and \f$n\f$ is the number of edges. Note that for
    //! WordGraphView objects the number of edges is always at most \f$mk\f$
    //! where \f$k\f$ is the WordGraphView::out_degree.
    //!
    //! \note Out of bounds targets of any edges in \p wgv are ignored.
    //!
    //! \par Example
    //! \code
    //! auto wgv = make<WordGraph<uint8_t>>(
    //!     5, {{0, 0}, {1, 1}, {2}, {3, 3}});
    //! word_graph::is_strictly_cyclic(WordGraphView<uint8_t>(wgv));  //
    //! returns false \endcode
    // TODO(1) should have a version that returns the node that everything is
    // reachable from
    template <typename Node>
    [[nodiscard]] bool is_strictly_cyclic(WordGraphView<Node> const& wgv);

    //! \brief Returns the last node on the path labelled by a word and an
    //! iterator to the position in the word reached.
    //!
    //! \tparam Node the node type of the word graph and \p source.
    //! \tparam Iterator the type of the iterators into a word.
    //!
    //! \param wgv a word graph view.
    //! \param source the source node.
    //! \param first iterator into a word.
    //! \param last iterator into a word.
    //!
    //! \returns A pair consisting of the last node reached and an iterator
    //! pointing at the last letter in the word labelling an edge.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! At worst the distance from \p first to \p last.
    //!
    //! \warning
    //! No checks on the arguments of this function are performed, it is
    //! assumed that \p source is a node in the word graph view \p wgv; and
    //! that the letters in the word described by \p first and \p last belong
    //! to the range \c 0 to WordGraphView::out_degree.
    template <typename Node, typename Iterator>
    [[nodiscard]] std::pair<Node, Iterator>
    last_node_on_path_no_checks(WordGraphView<Node> const& wgv,
                                Node                       source,
                                Iterator                   first,
                                Iterator                   last) noexcept;

    template <typename Node1,
              typename Node2,
              typename Iterator,
              typename = std::enable_if_t<!std::is_same_v<Node1, Node2>>>
    [[deprecated]] [[nodiscard]] std::pair<Node1, Iterator>
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
    template <typename Node>
    [[nodiscard]] [[deprecated]] std::pair<Node, word_type::const_iterator>
    last_node_on_path_no_checks(WordGraphView<Node> const& wgv,
                                Node                       source,
                                word_type const&           w);

    template <typename Node1,
              typename Node2,
              typename = std::enable_if_t<!std::is_same_v<Node1, Node2>>>
    [[nodiscard]] [[deprecated]] std::pair<Node1, word_type::const_iterator>
    last_node_on_path_no_checks(WordGraphView<Node1> const& wgv,
                                Node2                       source,
                                word_type const&            w) {
      static_assert(sizeof(Node2) <= sizeof(Node1));
      return last_node_on_path_no_checks(
          wgv, static_cast<Node1>(source), w.cbegin(), w.cend());
    }

    //! \brief Returns the last node on the path labelled by a word and an
    //! iterator to the position in the word reached.
    //!
    //! \tparam Node the node type of the word graph view and \p source.
    //! \tparam Iterator the type of the iterators into a word.
    //!
    //! \param wgv a word graph view.
    //! \param source the source node.
    //! \param first iterator into a word.
    //! \param last iterator into a word.
    //!
    //! \returns A pair consisting of the last node reached and an iterator
    //! pointing at the last letter in the word labelling an edge.
    //!
    //! \throws LibsemigroupsException if \p source is out of bounds.
    //!
    //! \complexity
    //! At worst the distance from \p first to \p last.
    //!
    //! \note If any value in \p wgv or in the word described by \p first and
    //! \p last is out of bounds (greater than or equal to
    //! WordGraphView::number_of_nodes), the path labelled by the word exits
    //! the word graph view, which is reflected in the result value of this
    //! function, but does not cause an exception to be thrown.
    template <typename Node, typename Iterator>
    [[nodiscard]] std::pair<Node, Iterator>
    last_node_on_path(WordGraphView<Node> const& wgv,
                      Node                       source,
                      Iterator                   first,
                      Iterator                   last);

    template <typename Node1,
              typename Node2,
              typename Iterator,
              typename = std::enable_if_t<!std::is_same_v<Node1, Node2>>>
    [[deprecated]] [[nodiscard]] std::pair<Node1, Iterator>
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
    template <typename Node>
    [[nodiscard]] [[deprecated]] std::pair<Node, word_type::const_iterator>
    last_node_on_path(WordGraphView<Node> const& wgv,
                      Node                       source,
                      word_type const&           w);

    template <typename Node1,
              typename Node2,
              typename = std::enable_if_t<!std::is_same_v<Node1, Node2>>>
    [[nodiscard]] [[deprecated]] std::pair<Node1, word_type::const_iterator>
    last_node_on_path(WordGraphView<Node1> const& wgv,
                      Node2                       source,
                      word_type const&            w) {
      static_assert(sizeof(Node2) <= sizeof(Node1));
      return last_node_on_path(
          wgv, static_cast<Node1>(source), w.cbegin(), w.cend());
    }

    //! \brief Returns the std::unordered_set of nodes reachable from a given
    //! node in a word graph.
    //!
    //! This function returns a std::unordered_set consisting of all the nodes
    //! in the word graph \p wgv that are reachable from \p source
    //! via a path of length at most \p max_depth.
    //!
    //! \tparam Node the node type of the word graph and \p source.
    //!
    //! \param wgv the word graph.
    //! \param source the source node.
    //! \param max_depth the maximum distance from source
    //! (defaults to \ref POSITIVE_INFINITY).
    //!
    //! \returns A std::unordered_set consisting of all the nodes in the word
    //! graph \p wgv that are reachable from \p source.
    //!
    //! \note If any target of any edge in the word graph \p wgv is out of
    //! bounds, then this is ignored by this function.
    //!
    //! \warning The arguments are not checked, and in particular it is
    //! assumed that \p source is a node of \p wgv (i.e. less than
    //! WordGraph::number_of_nodes).
    template <typename Node>
    [[nodiscard]] std::unordered_set<Node>
    nodes_reachable_from_no_checks(WordGraphView<Node> const& wgv,
                                   Node                       source,
                                   size_t max_depth = POSITIVE_INFINITY);

    template <typename Node1,
              typename Node2,
              typename = std::enable_if_t<!std::is_same_v<Node1, Node2>>>
    [[deprecated]] [[nodiscard]] std::unordered_set<Node1>
    nodes_reachable_from_no_checks(WordGraphView<Node1> const& wgv,
                                   Node2                       source,
                                   size_t max_depth = POSITIVE_INFINITY) {
      static_assert(sizeof(Node2) <= sizeof(Node1));
      return nodes_reachable_from_no_checks(
          wgv, static_cast<Node1>(source), max_depth);
    }

    //! \brief Returns the std::unordered_set of nodes reachable from a given
    //! node in a word graph view.
    //!
    //! This function returns a std::unordered_set consisting of all the nodes
    //! in the word graph view \p wgv that are reachable from \p source
    //! via a path of length at most \p max_depth.
    //!
    //! \tparam Node the node type of the word graph view and \p source.
    //!
    //! \param wgv the word graph view.
    //! \param source the source node.
    //! \param max_depth the maximum distance from source
    //! (defaults to \ref POSITIVE_INFINITY).
    //!
    //! \returns A std::unordered_set consisting of all the nodes in the word
    //! graph view \p wgv that are reachable from \p source.
    //!
    //! \throws LibsemigroupsException if \p source is out of bounds (greater
    //! than or equal to WordGraphView::number_of_nodes).
    //!
    //! \note If any target of any edge in the word graph view \p wgv that is
    //! out of bounds, then this is ignored by this function.
    template <typename Node>
    [[nodiscard]] std::unordered_set<Node>
    nodes_reachable_from(WordGraphView<Node> const& wgv,
                         Node                       source,
                         size_t max_depth = POSITIVE_INFINITY) {
      detail::throw_if_not_less(source, wgv.number_of_nodes(), "node ");
      return nodes_reachable_from_no_checks(wgv, source, max_depth);
    }

    template <typename Node1,
              typename Node2,
              typename = std::enable_if_t<!std::is_same_v<Node1, Node2>>>
    [[deprecated]] [[nodiscard]] std::unordered_set<Node1>
    nodes_reachable_from(WordGraphView<Node1> const& wgv,
                         Node2                       source,
                         size_t max_depth = POSITIVE_INFINITY) {
      static_assert(sizeof(Node2) <= sizeof(Node1));
      return nodes_reachable_from(wgv, static_cast<Node1>(source), max_depth);
    }

    //! \brief Returns the number of nodes reachable from a given node in a
    //! word graph.
    //!
    //! This function returns the number of nodes in the word graph \p wgv
    //! that are reachable from \p source via a path of length at most
    //! \p max_depth.
    //!
    //! \tparam Node the node type of the word graph and \p source.
    //!
    //! \param wgv the word graph.
    //! \param source the source node.
    //! \param max_depth the maximum distance from source
    //! (defaults to \ref POSITIVE_INFINITY).
    //!
    //! \returns The number of nodes in the word graph \p wgv that are
    //! reachable from \p source.
    //!
    //! \note If any target of any edge in the word graph \p wgv is out of
    //! bounds, then this is ignored by this function.
    //!
    //! \warning The arguments are not checked, and in particular it is
    //! assumed that \p source is a node of \p wgv (i.e. less than
    //! WordGraphView::number_of_nodes).
    template <typename Node>
    [[nodiscard]] size_t number_of_nodes_reachable_from_no_checks(
        WordGraphView<Node> const& wgv,
        Node                       source,
        size_t                     max_depth = POSITIVE_INFINITY) {
      return nodes_reachable_from_no_checks(wgv, source, max_depth).size();
    }

    template <typename Node1,
              typename Node2,
              typename = std::enable_if_t<!std::is_same_v<Node1, Node2>>>
    [[deprecated]] [[nodiscard]] size_t
    number_of_nodes_reachable_from_no_checks(WordGraphView<Node1> const& wgv,
                                             Node2                       source,
                                             size_t max_depth
                                             = POSITIVE_INFINITY) {
      static_assert(sizeof(Node2) <= sizeof(Node1));
      return number_of_nodes_reachable_from_no_checks(
          wgv, static_cast<Node1>(source), max_depth);
    }

    //! \brief Returns the number of nodes reachable from a given node in a
    //! word graph view.
    //!
    //! This function returns the number of nodes in the word graph view
    //! \p wgv that are reachable from \p source via a path of length at most
    //! \p max_depth.
    //!
    //! \tparam Node the node type of the word graph and \p source.
    //!
    //! \param wgv the word graph.
    //! \param source the source node.
    //! \param max_depth the maximum distance from source
    //! (defaults to \ref POSITIVE_INFINITY).
    //!
    //! \returns The number of nodes in the word graph \p wgv that are
    //! reachable from \p source.
    //!
    //! \throws LibsemigroupsException if \p source is out of bounds (greater
    //! than or equal to WordGraphView::number_of_nodes).
    //!
    //! \note If any target of any edge in the word graph \p wgv is out of
    //! bounds, then this is ignored by this function.
    template <typename Node>
    [[nodiscard]] size_t
    number_of_nodes_reachable_from(WordGraphView<Node> const& wgv,
                                   Node                       source,
                                   size_t max_depth = POSITIVE_INFINITY) {
      return nodes_reachable_from(wgv, source, max_depth).size();
    }

    template <typename Node1,
              typename Node2,
              typename = std::enable_if_t<!std::is_same_v<Node1, Node2>>>
    [[deprecated]] [[nodiscard]] size_t
    number_of_nodes_reachable_from(WordGraphView<Node1> const& wgv,
                                   Node2                       source,
                                   size_t max_depth = POSITIVE_INFINITY) {
      static_assert(sizeof(Node2) <= sizeof(Node1));
      return number_of_nodes_reachable_from(
          wgv, static_cast<Node1>(source), max_depth);
    }

    //! \brief Replace the contents of a Forest by a spanning tree of the
    //! nodes reachable from a given node in a word graph.
    //!
    //! This function replaces the content of the Forest \p f with a spanning
    //! tree of the nodes reachable from \p root in the word graph \p wgv
    //! by a path of length at most \p max_depth.
    //!
    //! \tparam Node the node type of the word graph and \p root.
    //!
    //! \param wgv the word graph.
    //! \param root the source node.
    //! \param f the Forest object to hold the result.
    //! \param max_depth the maximum depth of the tree (defaults to
    //! \ref POSITIVE_INFINITY).
    //!
    //! \note If any target of any edge in the word graph \p wgv is out of
    //! bounds, then this is ignored by this function.
    //!
    //! \warning The arguments are not checked, and in particular it is
    //! assumed that \p root is a node of \p wgv (i.e. less than
    //! WordGraphView::number_of_nodes).
    template <typename Node>
    void spanning_tree_no_checks(WordGraphView<Node> const& wgv,
                                 Node                       root,
                                 Forest&                    f,
                                 size_t max_depth = POSITIVE_INFINITY);

    template <typename Node1,
              typename Node2,
              typename = std::enable_if_t<!std::is_same_v<Node1, Node2>>>
    [[deprecated]] void spanning_tree_no_checks(WordGraphView<Node1> const& wgv,
                                                Node2   root,
                                                Forest& f,
                                                size_t  max_depth
                                                = POSITIVE_INFINITY) {
      static_assert(sizeof(Node2) <= sizeof(Node1));
      spanning_tree_no_checks(wgv, static_cast<Node1>(root), f, max_depth);
    }

    //! \brief Returns a Forest containing a spanning tree of the nodes
    //! reachable from a given node in a word graph.
    //!
    //! This function returns a Forest containing a spanning
    //! tree of the nodes reachable from \p root in the word graph \p wgv
    //! by a path of length at most \p max_depth.
    //!
    //! \tparam Node the node type of the word graph and \p root.
    //!
    //! \param wgv the word graph.
    //! \param root the source node.
    //! \param max_depth the maximum depth of the tree (defaults to
    //! \ref POSITIVE_INFINITY).
    //!
    //! \returns A Forest object containing a spanning tree.
    //!
    //! \note If any target of any edge in the word graph \p wgv is out of
    //! bounds, then this is ignored by this function.
    //!
    //! \warning The arguments are not checked, and in particular it is
    //! assumed that \p root is a node of \p wgv (i.e. less than
    //! WordGraphView::number_of_nodes).
    template <typename Node>
    [[nodiscard]] Forest spanning_tree_no_checks(WordGraphView<Node> const& wgv,
                                                 Node   root,
                                                 size_t max_depth
                                                 = POSITIVE_INFINITY);

    template <typename Node1,
              typename Node2,
              typename = std::enable_if_t<!std::is_same_v<Node1, Node2>>>
    [[deprecated]] [[nodiscard]] Forest
    spanning_tree_no_checks(WordGraphView<Node1> const& wgv,
                            Node2                       root,
                            size_t max_depth = POSITIVE_INFINITY) {
      static_assert(sizeof(Node2) <= sizeof(Node1));
      return spanning_tree_no_checks(wgv, static_cast<Node1>(root), max_depth);
    }

    //! \brief Replace the contents of a Forest by a spanning tree of the
    //! nodes reachable from a given node in a word graph.
    //!
    //! This function replaces the content of the Forest \p f with a spanning
    //! tree of the nodes reachable from \p root in the word graph \p wgv
    //! by a path of length at most \p max_depth.
    //!
    //! \tparam Node the node type of the word graph and \p root.
    //!
    //! \param wgv the word graph.
    //! \param root the source node.
    //! \param f the Forest object to hold the result.
    //! \param max_depth the maximum depth of the tree (defaults to
    //! \ref POSITIVE_INFINITY).
    //!
    //! \throws LibsemigroupsException if \p root is out of bounds, i.e.
    //! greater than or equal to WordGraphView::number_of_nodes.
    //!
    //! \note If any target of any edge in the word graph \p wgv is out of
    //! bounds, then this is ignored by this function.
    template <typename Node>
    void spanning_tree(WordGraphView<Node> const& wgv,
                       Node                       root,
                       Forest&                    f,
                       size_t max_depth = POSITIVE_INFINITY);

    template <typename Node1,
              typename Node2,
              typename = std::enable_if_t<!std::is_same_v<Node1, Node2>>>
    [[deprecated]] void spanning_tree(WordGraphView<Node1> const& wgv,
                                      Node2                       root,
                                      Forest&                     f,
                                      size_t max_depth = POSITIVE_INFINITY) {
      static_assert(sizeof(Node2) <= sizeof(Node1));
      spanning_tree(wgv, static_cast<Node1>(root), f, max_depth);
    }

    //! \brief Returns a Forest containing a spanning tree of the nodes
    //! reachable from a given node in a word graph.
    //!
    //! This function returns a Forest containing a spanning
    //! tree of the nodes reachable from \p root in the word graph \p wgv
    //! by a path of length at most \p max_depth.
    //!
    //! \tparam Node the node type of the word graph and \p root.
    //!
    //! \param wgv the word graph.
    //! \param root the source node.
    //! \param max_depth the maximum depth of the tree (defaults to
    //! \ref POSITIVE_INFINITY).
    //!
    //! \returns A Forest object containing a spanning tree.
    //!
    //! \throws LibsemigroupsException if \p root is out of bounds, i.e.
    //! greater than or equal to WordGraphView::number_of_nodes.
    //!
    //! \note If any target of any edge in the word graph \p wgv is out of
    //! bounds, then this is ignored by this function.
    template <typename Node>
    [[nodiscard]] Forest spanning_tree(WordGraphView<Node> const& wgv,
                                       Node                       root,
                                       size_t max_depth = POSITIVE_INFINITY);

    template <typename Node1,
              typename Node2,
              typename = std::enable_if_t<!std::is_same_v<Node1, Node2>>>
    [[deprecated]] [[nodiscard]] Forest
    spanning_tree(WordGraphView<Node1> const& wgv,
                  Node2                       root,
                  size_t                      max_depth = POSITIVE_INFINITY) {
      static_assert(sizeof(Node2) <= sizeof(Node1));
      return spanning_tree(wgv, static_cast<Node1>(root), max_depth);
    }

    template <typename Node, typename Iterator>
    void throw_if_any_target_out_of_bounds(WordGraphView<Node> const& wgv,
                                           Iterator                   first,
                                           Iterator                   last);

    //! \brief Returns the nodes of the word graph in topological order (see
    //! below) if possible.
    //!
    //! If it is not empty, the returned vector has the property that if an
    //! edge from a node \c n points to a node \c m, then \c m occurs before
    //! \c n in the vector.
    //!
    //! \tparam Node  the type of the nodes of the WordGraph.
    //!
    //! \param wgv the word graph.
    //!
    //! \returns
    //! A std::vector of Node types that contains the nodes of
    //! \p wgv in topological order (if possible) and is otherwise empty.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \par Complexity
    //! \f$O(m + n)\f$ where \f$m\f$ is the number of nodes in the
    //! WordGraph \p wgv and \f$n\f$ is the number of edges. Note that for
    //! WordGraph objects the number of edges is always at most \f$mk\f$
    //! where \f$k\f$ is the WordGraphView::out_degree.
    template <typename Node>
    [[nodiscard]] std::vector<Node>
    topological_sort(WordGraphView<Node> const& wgv);

    //! Returns the nodes of the word graph reachable from a given node in
    //! topological order (see below) if possible.
    //!
    //! If it is not empty, the returned vector has the property that
    //! if an edge from a node \c n points to a node \c m, then \c m occurs
    //! before \c n in the vector, and the last item in the vector is
    //! \p source.
    //!
    //! \tparam Node the node type of the word graph and \p source.
    //!
    //! \param wgv the WordGraph object to check.
    //! \param source the source node.
    //!
    //! \returns
    //! A std::vector of Node types that contains the nodes reachable from
    //! \p source in \p wgv in topological order (if possible) and is
    //! otherwise empty.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \par Complexity
    //! At worst \f$O(m + n)\f$ where \f$m\f$ is the number of nodes in the
    //! subword graph of those nodes reachable from \p source
    //! and \f$n\f$ is the number of edges.
    template <typename Node>
    [[nodiscard]] std::vector<Node>
    topological_sort(WordGraphView<Node> const& wgv, Node source);

    template <typename Node1,
              typename Node2,
              typename = std::enable_if_t<!std::is_same_v<Node1, Node2>>>
    [[deprecated]] [[nodiscard]] std::vector<Node1>
    topological_sort(WordGraphView<Node1> const& wgv, Node2 source) {
      static_assert(sizeof(Node2) <= sizeof(Node1));
      return topological_sort(wgv, static_cast<Node1>(source));
    }
  }  // namespace word_graph
}  // namespace libsemigroups

#include "libsemigroups/word-graph-view-helpers.tpp"
#endif  // LIBSEMIGROUPS_WORD_GRAPH_VIEW_HELPERS_HPP_
