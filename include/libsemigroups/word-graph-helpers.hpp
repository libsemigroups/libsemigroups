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

// This file contains helper functions for word graphs and word graph views

#ifndef LIBSEMIGROUPS_WORD_GRAPH_HELPERS_HPP_
#define LIBSEMIGROUPS_WORD_GRAPH_HELPERS_HPP_

#include <algorithm>         // for max, fill
#include <initializer_list>  // for initializer_list
#include <iosfwd>            // for ostream
#include <iterator>          // for empty
#include <numeric>           // for iota
#include <queue>             // for queue
#include <random>            // for mt19937, random_device
#include <stack>             // for stack
#include <stddef.h>          // for size_t
#include <stdint.h>          // for uint64_t, uint8_t
#include <string>            // for basic_string, allocator
#include <tuple>             // for tie
#include <type_traits>       // for enable_if_t, decay_t
#include <unordered_map>     // for unordered_map
#include <unordered_set>     // for unordered_set
#include <utility>           // for pair, move, make_pair
#include <vector>            // for vector, swap

#include "libsemigroups/adapters.hpp"         // for Hash
#include "libsemigroups/config.hpp"           // for LIBSEMIGROUPS_EIGEN_...
#include "libsemigroups/constants.hpp"        // for UNDEFINED
#include "libsemigroups/debug.hpp"            // for LIBSEMIGROUPS_ASSERT
#include "libsemigroups/dot.hpp"              // for Dot
#include "libsemigroups/exception.hpp"        // for LIBSEMIGROUPS_EXCEPTION
#include "libsemigroups/forest.hpp"           // for Forest
#include "libsemigroups/order.hpp"            // for Order
#include "libsemigroups/types.hpp"            // for word_type, letter_type
#include "libsemigroups/word-graph-view.hpp"  // for WordGraphView
#include "libsemigroups/word-graph.hpp"       // for WordGraph

#include "libsemigroups/detail/fmt.hpp"     // for fmt::format
#include "libsemigroups/detail/stl.hpp"     // for HasLessEqual
#include "libsemigroups/detail/string.hpp"  // for group_digits
#include "libsemigroups/detail/uf.hpp"      // for Duf

#ifdef LIBSEMIGROUPS_EIGEN_ENABLED
#include "detail/eigen.hpp"
#else
#include "matrix.hpp"
#endif

namespace libsemigroups {

  namespace v4 {

    //! \ingroup word_graph_group
    //!
    //! \brief Namespace containing helper functions for the \ref WordGraph
    //! class.
    //!
    //! Defined in `word-graph-helpers.hpp`.
    //!
    //! \brief This namespace contains helper functions for the \ref WordGraph
    //! class.
    namespace word_graph {

      //////////////////////////////////////////////////////////////////////////
      // WordGraph - helper functions - in alphabetical order!!!
      //////////////////////////////////////////////////////////////////////////

      //! \brief Adds a cycle involving the specified range of nodes to a word
      //! graph.
      //!
      //! This function adds a cycle involving the specified range of nodes.
      //!
      //! \tparam Node  the type of the nodes of the WordGraph.
      //!
      //! \tparam Iterator the type of an iterator pointing to nodes of a word
      //! graph.
      //!
      //! \param wg the WordGraph object to add a cycle to.
      //! \param first an iterator to nodes of \p wg.
      //! \param last an iterator to nodes of \p wg.
      //!
      //! \exceptions
      //! \no_libsemigroups_except
      //!
      //! \par Complexity
      //! \f$O(m)\f$ where \f$m\f$ is the distance between \p first and \p last.
      //!
      //! \note
      //! The edges added by this function are all labelled \c 0.
      // TODO(1) add add_cycle with checks version.
      template <typename Node, typename Iterator>
      void add_cycle_no_checks(WordGraph<Node>& wg,
                               Iterator         first,
                               Iterator         last);

      //! \brief Adds a cycle consisting of \p N new nodes.
      //!
      //! \tparam Node  the type of the nodes of the WordGraph.
      //!
      //! \param wg the WordGraph object to add a cycle to.
      //! \param N the length of the cycle and number of new nodes to add.
      //!
      //! \exceptions
      //! \no_libsemigroups_except
      //!
      //! \par Complexity
      //! \f$O(N)\f$ where \f$N\f$ is the second parameter.
      //!
      //! \note
      //! The edges added by this function are all labelled \c 0.
      template <typename Node>
      void add_cycle(WordGraph<Node>& wg, size_t N) {
        size_t M = wg.number_of_nodes();
        wg.add_nodes(N);
        add_cycle_no_checks(wg, wg.cbegin_nodes() + M, wg.cend_nodes());
      }

      //! \brief Returns the adjacency matrix of a word graph view.
      //!
      //! This function returns the adjacency matrix of the word graph view
      //! \p wg. The type of the returned matrix depends on whether or not
      //! `libsemigroups` is compiled with [eigen][] enabled. The returned
      //! matrix has the number of edges with source \c s and target \c t in the
      //! `(s, t)`-entry.
      //!
      //! \tparam Node  the type of the nodes of the WordGraphView.
      //!
      //! \param wg the word graph view.
      //!
      //! \returns The adjacency matrix.
      //!
      //! [eigen]: http://eigen.tuxfamily.org/
      template <typename Node>
      [[nodiscard]] auto adjacency_matrix(WordGraphView<Node> const& wg);

      //! \brief Returns the adjacency matrix of a word graph.
      //!
      //! This function returns the adjacency matrix of the word graph \p wg.
      //! The type of the returned matrix depends on whether or not
      //! `libsemigroups` is compiled with [eigen][] enabled. The returned
      //! matrix has the number of edges with source \c s and target \c t in the
      //! `(s, t)`-entry.
      //!
      //! \tparam Node  the type of the nodes of the WordGraph.
      //!
      //! \param wg the word graph.
      //!
      //! \returns The adjacency matrix.
      //!
      //! [eigen]: http://eigen.tuxfamily.org/
      template <typename Node>
      [[nodiscard]] auto adjacency_matrix(WordGraph<Node> const& wg) {
        return adjacency_matrix(WordGraphView<Node>(wg));
      }

      //! \brief Returns a \ref Dot object representing a word graph view.
      //!
      //! This function returns a \ref Dot object representing the word graph
      //! view \p wg.
      //!
      //! \tparam Node  the type of the nodes of the WordGraphView.
      //!
      //! \param wg the word graph.
      //!
      //! \returns A \ref Dot object.
      template <typename Node>
      [[nodiscard]] Dot dot(WordGraphView<Node> const& wg);

      //! \brief Returns a \ref Dot object representing a word graph.
      //!
      //! This function returns a \ref Dot object representing the word graph
      //! \p wg.
      //!
      //! \tparam Node  the type of the nodes of the WordGraph.
      //!
      //! \param wg the word graph.
      //!
      //! \returns A \ref Dot object.
      template <typename Node>
      [[nodiscard]] Dot dot(WordGraph<Node> const& wg) {
        return dot(WordGraphView<Node>(wg));
      }

      //! \brief Compares two word graphs on a range of nodes.
      //!
      //! This function returns \c true if the word graphs \p x and \p y are
      //! equal on the range of nodes from \p first to \p last; and \c false
      //! otherwise.  The word graphs \p x and \p y are equal at a node \c s if:
      //! * the out-degrees of \p x and \p y coincide;
      //! * the edges with source \c s and label \c a have equal targets in \p x
      //! and \p y for every label \c a.
      //!
      //! \tparam Node  the type of the nodes of the WordGraph.
      //!
      //! \param x  the first word graph for comparison.
      //! \param y the second word graph for comparison.
      //! \param first the first node in the range.
      //! \param last the last node in the range plus \c 1.
      //!
      //! \returns Whether or not the word graphs are equal at the specified
      //! range of nodes.
      //!
      //! \exceptions
      //! \no_libsemigroups_except
      //!
      //! \warning No checks are performed to ensure that the arguments
      //! are valid.
      //!
      //! \sa WordGraph::operator== for a comparison of two entire word graphs.
      template <typename Node>
      [[nodiscard]] bool equal_to_no_checks(WordGraph<Node> const& x,
                                            WordGraph<Node> const& y,
                                            Node                   first,
                                            Node                   last) {
        WordGraphView<Node> x_view = WordGraphView<Node>(x, first, last);
        WordGraphView<Node> y_view = WordGraphView<Node>(y, first, last);
        return x_view == y_view;
      }

      //! \brief Compares two word graphs on a range of nodes.
      //!
      //! This function returns \c true if the word graphs \p x and \p y are
      //! equal on the range of nodes from \p first to \p last; and \c false
      //! otherwise.  The word graphs \p x and \p y are equal at a node \c s if:
      //! * the out-degrees of \p x and \p y coincide;
      //! * the edges with source \c s and label \c a have equal targets in \p x
      //! and \p y for every label \c a.
      //!
      //! \tparam Node  the type of the nodes of the WordGraph.
      //!
      //! \param x  the first word graph for comparison.
      //! \param y the second word graph for comparison.
      //! \param first the first node in the range.
      //! \param last the last node in the range plus \c 1.
      //!
      //! \returns Whether or not the word graphs are equal at the specified
      //! range of nodes.
      //!
      //! \throw LibsemigroupsException if \p first  is not a node
      //! in \p x or not a node in \p y; or if `last - 1` is not a node in \p or
      //! not a node in \p y.
      //!
      //! \sa WordGraph::operator== for a comparison of two entire word graphs.
      template <typename Node>
      [[nodiscard]] bool equal_to(WordGraph<Node> const& x,
                                  WordGraph<Node> const& y,
                                  Node                   first,
                                  Node                   last);

      //! \brief Find the node that a path starting at a given node leads to (if
      //! any).
      //!
      //! This function attempts to follow the path in the word graph view \p wg
      //! starting at the node \p from  labelled by the word defined by \p first
      //! and \p last. If this path exists, then the last node on that path is
      //! returned. If this path does not exist, then \ref UNDEFINED is
      //! returned.
      //!
      //! \tparam Node1  the type of the nodes of the WordGraphView
      //! \p wg.
      //!
      //! \tparam Node2 the type of the node \p from (must satisfy
      //! `sizeof(Node2) <= sizeof(Node1)`).
      //!
      //! \tparam Iterator the type of \p first and \p last.
      //!
      //! \param wg a word graph view.
      //! \param source the starting node.
      //! \param first an iterator point at the start of the word.
      //! \param last an iterator point one beyond the last letter of the word.
      //!
      //! \returns
      //! A value of type \p Node1. If one or more edges in \p path are not
      //! defined, then \ref UNDEFINED is returned.
      //!
      //! \throw LibsemigroupsException if \p from is not a node in the word
      //! graph or the word defined by \p first and \p last contains a value
      //! that is not an edge-label.
      //!
      //! \par Complexity
      //! Linear in the distance between \p first and \p last.
      template <typename Node1, typename Node2, typename Iterator>
      [[nodiscard]] Node1 follow_path(WordGraphView<Node1> const& wg,
                                      Node2                       source,
                                      Iterator                    first,
                                      Iterator                    last);

      //! \brief Find the node that a path starting at a given node leads to (if
      //! any).
      //!
      //! This function attempts to follow the path in the word graph \p wg
      //! starting at the node \p from  labelled by the word defined by \p first
      //! and \p last. If this path exists, then the last node on that path is
      //! returned. If this path does not exist, then \ref UNDEFINED is
      //! returned.
      //!
      //! \tparam Node1  the type of the nodes of the WordGraph
      //! \p wg.
      //!
      //! \tparam Node2 the type of the node \p from (must satisfy
      //! `sizeof(Node2) <= sizeof(Node1)`).
      //!
      //! \tparam Iterator the type of \p first and \p last.
      //!
      //! \param wg a word graph.
      //! \param source the starting node.
      //! \param first an iterator point at the start of the word.
      //! \param last an iterator point one beyond the last letter of the word.
      //!
      //! \returns
      //! A value of type \p Node1. If one or more edges in \p path are not
      //! defined, then \ref UNDEFINED is returned.
      //!
      //! \throw LibsemigroupsException if \p from is not a node in the word
      //! graph or the word defined by \p first and \p last contains a value
      //! that is not an edge-label.
      //!
      //! \par Complexity
      //! Linear in the distance between \p first and \p last.
      template <typename Node1, typename Node2, typename Iterator>
      [[nodiscard]] Node1 follow_path(WordGraph<Node1> const& wg,
                                      Node2                   source,
                                      Iterator                first,
                                      Iterator                last) {
        return follow_path(WordGraphView<Node1>(wg), source, first, last);
      }

      //! \brief Find the node that a path starting at a given node leads to (if
      //! any).
      //!
      //! This function attempts to follow the path in the word graph view \p wg
      //! starting at the node \p from  labelled by the word \p path. If this
      //! path exists, then the last node on that path is returned. If this path
      //! does not exist, then \ref UNDEFINED is returned.
      //!
      //! \tparam Node1  the type of the nodes of the WordGraphView
      //! \p wg.
      //!
      //! \tparam Node2 the type of the node \p from (must satisfy
      //! `sizeof(Node2) <= sizeof(Node1)`).
      //!
      //! \param wg a word graph view.
      //! \param from the starting node.
      //! \param path the path to follow.
      //!
      //! \returns
      //! A value of type \p Node1. If one or more edges in \p path are not
      //! defined, then \ref UNDEFINED is returned.
      //!
      //! \throw LibsemigroupsException if \p from is not a node in the word
      //! graph view or \p path contains a value that is not an edge-label.
      //!
      //! \par Complexity
      //! Linear in the length of \p path.
      // TODO(2) example
      // not noexcept because WordGraph::target isn't
      template <typename Node1, typename Node2>
      [[nodiscard]] Node1 follow_path(WordGraphView<Node1> const& wg,
                                      Node2                       from,
                                      word_type const&            path) {
        static_assert(sizeof(Node2) <= sizeof(Node1));
        return follow_path(wg, from, path.cbegin(), path.cend());
      }

      //! \brief Find the node that a path starting at a given node leads to (if
      //! any).
      //!
      //! This function attempts to follow the path in the word graph  \p wg
      //! starting at the node \p from  labelled by the word \p path. If this
      //! path exists, then the last node on that path is returned. If this path
      //! does not exist, then \ref UNDEFINED is returned.
      //!
      //! \tparam Node1  the type of the nodes of the WordGraph
      //! \p wg.
      //!
      //! \tparam Node2 the type of the node \p from (must satisfy
      //! `sizeof(Node2) <= sizeof(Node1)`).
      //!
      //! \param wg a word graph.
      //! \param from the starting node.
      //! \param path the path to follow.
      //!
      //! \returns
      //! A value of type \p Node1. If one or more edges in \p path are not
      //! defined, then \ref UNDEFINED is returned.
      //!
      //! \throw LibsemigroupsException if \p from is not a node in the word
      //! graph or \p path contains a value that is not an edge-label.
      //!
      //! \par Complexity
      //! Linear in the length of \p path.
      // TODO(2) example
      // not noexcept because WordGraph::target isn't
      template <typename Node1, typename Node2>
      [[nodiscard]] Node1 follow_path(WordGraph<Node1> const& wg,
                                      Node2                   from,
                                      word_type const&        path) {
        static_assert(sizeof(Node2) <= sizeof(Node1));
        return follow_path(
            WordGraphView<Node1>(wg), from, path.cbegin(), path.cend());
      }

      //! \brief Follow the path from a specified node labelled by a word.
      //!
      //! This function returns the last node on the path in the word graph view
      //! \p wg starting at the node \p from labelled by the word defined by
      //! \p first and \p last or \ref UNDEFINED.
      //!
      //! \tparam Node1 the type of the nodes of the WordGraphView \p wg.
      //!
      //! \tparam Node2 the type of the node \p from (must satisfy
      //! `sizeof(Node2) <= sizeof(Node1)`).
      //!
      //! \param wg a word graph view.
      //! \param from the source node.
      //! \param first iterator into a word.
      //! \param last iterator into a word.
      //!
      //! \returns A value of type \p Node1.
      //!
      //! \exceptions
      //! \noexcept
      //!
      //! \complexity
      //! At worst the distance from \p first to \p last.
      //!
      //! \warning
      //! No checks on the arguments of this function are performed.
      template <typename Node1, typename Node2, typename Iterator>
      [[nodiscard]] Node1 follow_path_no_checks(WordGraphView<Node1> const& wg,
                                                Node2    from,
                                                Iterator first,
                                                Iterator last) noexcept;

      //! \brief Follow the path from a specified node labelled by a word.
      //!
      //! This function returns the last node on the path in the word graph
      //! \p wg starting at the node \p from labelled by the word defined by
      //! \p first and \p last or \ref UNDEFINED.
      //!
      //! \tparam Node1 the type of the nodes of the WordGraph \p wg.
      //!
      //! \tparam Node2 the type of the node \p from (must satisfy
      //! `sizeof(Node2) <= sizeof(Node1)`).
      //!
      //! \param wg a word graph.
      //! \param from the source node.
      //! \param first iterator into a word.
      //! \param last iterator into a word.
      //!
      //! \returns A value of type \p Node1.
      //!
      //! \exceptions
      //! \noexcept
      //!
      //! \complexity
      //! At worst the distance from \p first to \p last.
      //!
      //! \warning
      //! No checks on the arguments of this function are performed.
      template <typename Node1, typename Node2, typename Iterator>
      [[nodiscard]] Node1 follow_path_no_checks(WordGraph<Node1> const& wg,
                                                Node2                   from,
                                                Iterator                first,
                                                Iterator last) noexcept {
        return follow_path_no_checks(
            WordGraphView<Node1>(wg), from, first, last);
      }

      //! \brief Follow the path from a specified node labelled by a word.
      //!
      //! This function returns the last node on the path in the word graph view
      //! \p wg starting at the node \p from labelled by \p path or
      //! \ref UNDEFINED.
      //!
      //! \tparam Node1  the type of the nodes of the WordGraphView
      //! \p wg.
      //!
      //! \tparam Node2 the type of the node \p from (must satisfy
      //! `sizeof(Node2) <= sizeof(Node1)`).
      //!
      //! \param wg a word graph view.
      //! \param from the source node.
      //! \param path the word.
      //!
      //! \returns A value of type \p Node1.
      //!
      //! \exceptions
      //! \noexcept
      //!
      //! \complexity
      //! At worst the length of \p path.
      //!
      //! \warning
      //! No checks on the arguments of this function are performed.
      template <typename Node1, typename Node2>
      [[nodiscard]] Node1
      follow_path_no_checks(WordGraphView<Node1> const& wg,
                            Node2                       from,
                            word_type const&            path) noexcept {
        static_assert(sizeof(Node2) <= sizeof(Node1));
        return follow_path_no_checks(wg, from, path.cbegin(), path.cend());
      }

      //! \brief Follow the path from a specified node labelled by a word.
      //!
      //! This function returns the last node on the path in the word graph
      //! \p wg starting at the node \p from labelled by \p path or
      //! \ref UNDEFINED.
      //!
      //! \tparam Node1  the type of the nodes of the WordGraph
      //! \p wg.
      //!
      //! \tparam Node2 the type of the node \p from (must satisfy
      //! `sizeof(Node2) <= sizeof(Node1)`).
      //!
      //! \param wg a word graph.
      //! \param from the source node.
      //! \param path the word.
      //!
      //! \returns A value of type \p Node1.
      //!
      //! \exceptions
      //! \noexcept
      //!
      //! \complexity
      //! At worst the length of \p path.
      //!
      //! \warning
      //! No checks on the arguments of this function are performed.
      template <typename Node1, typename Node2>
      [[nodiscard]] Node1
      follow_path_no_checks(WordGraph<Node1> const& wg,
                            Node2                   from,
                            word_type const&        path) noexcept {
        static_assert(sizeof(Node2) <= sizeof(Node1));
        return follow_path_no_checks(
            WordGraphView<Node1>(wg), from, path.cbegin(), path.cend());
      }

      //! \brief Check if a word graph view is acyclic.
      //!
      //! This function returns \c true if the word graph view \p wg is acyclic
      //! and
      //! \c false otherwise. A word graph is acyclic if every directed cycle in
      //! the word graph is trivial.
      //!
      //! \tparam Node  the type of the nodes of the WordGraphView.
      //!
      //! \param wg the WordGraphView object to check.
      //!
      //! \returns
      //! A value of type `bool`.
      //!
      //! \exceptions
      //! \no_libsemigroups_except
      //!
      //! \par Complexity
      //! \f$O(m + n)\f$ where \f$m\f$ is the number of nodes in the
      //! WordGraphView \p wg and \f$n\f$ is the number of edges. Note that for
      //! WordGraphView objects the number of edges is always at most \f$mk\f$
      //! where \f$k\f$ is the \ref WordGraphView::out_degree.
      //!
      //!
      //! \par Example
      //! \code
      //! WordGraph<size_t> wg;
      //! wg.add_nodes(2);
      //! wg.add_to_out_degree(1);
      //! wg.target(0, 0, 1);
      //! wg.target(1, 0, 0);
      //! word_graph::is_acyclic(WordGraphView<size_t>(wg)); // returns false
      //! \endcode
      // Not noexcept because detail::is_acyclic isn't
      template <typename Node>
      [[nodiscard]] bool is_acyclic(WordGraphView<Node> const& wg);

      //! \brief Check if a word graph is acyclic.
      //!
      //! This function returns \c true if the word graph \p wg is acyclic and
      //! \c false otherwise. A word graph is acyclic if every directed cycle in
      //! the word graph is trivial.
      //!
      //! \tparam Node  the type of the nodes of the WordGraph.
      //!
      //! \param wg the WordGraph object to check.
      //!
      //! \returns
      //! A value of type `bool`.
      //!
      //! \exceptions
      //! \no_libsemigroups_except
      //!
      //! \par Complexity
      //! \f$O(m + n)\f$ where \f$m\f$ is the number of nodes in the
      //! WordGraph \p wg and \f$n\f$ is the number of edges. Note that for
      //! WordGraph objects the number of edges is always at most \f$mk\f$
      //! where \f$k\f$ is the \ref WordGraph::out_degree.
      //!
      //!
      //! \par Example
      //! \code
      //! WordGraph<size_t> wg;
      //! wg.add_nodes(2);
      //! wg.add_to_out_degree(1);
      //! wg.target(0, 0, 1);
      //! wg.target(1, 0, 0);
      //! word_graph::is_acyclic(wg); // returns false
      //! \endcode
      // Not noexcept because detail::is_acyclic isn't
      template <typename Node>
      [[nodiscard]] bool is_acyclic(WordGraph<Node> const& wg) {
        return is_acyclic(WordGraphView<Node>(wg));
      }

      //! \brief Check if the word graph view induced by the nodes reachable
      //! from a source node is acyclic.
      //!
      //! This function returns \c true if the word graph view consisting of the
      //! nodes reachable from \p source in the word graph view \p wg is acyclic
      //! and \c false if not. A word graph view is acyclic if every directed
      //! cycle in the word graph is trivial.
      //!
      //! \tparam Node1  the type of the nodes of the WordGraphView
      //! \p wg.
      //!
      //! \tparam Node2 the type of the node \p source (must satisfy
      //! `sizeof(Node2) <= sizeof(Node1)`).
      //!
      //! \param wg the WordGraphView object to check.
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
      //! WordGraphView \p wg and \f$n\f$ is the number of edges. Note that for
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
      //! WordGraphView<size_t> wg(g);
      //! word_graph::is_acyclic(wg); // returns false
      //! word_graph::is_acyclic(wg, 0); // returns false
      //! word_graph::is_acyclic(wg, 1); // returns false
      //! word_graph::is_acyclic(wg, 2); // returns true
      //! word_graph::is_acyclic(wg, 3); // returns true
      //! \endcode
      // Not noexcept because detail::is_acyclic isn't
      template <typename Node1, typename Node2>
      [[nodiscard]] bool is_acyclic(WordGraphView<Node1> const& wg,
                                    Node2                       source);

      //! \brief Check if the word graph induced by the nodes reachable from a
      //! source node is acyclic.
      //!
      //! This function returns \c true if the word graph consisting of the
      //! nodes reachable from \p source in the word graph \p wg is acyclic and
      //! \c false if not. A word graph is acyclic if every directed cycle in
      //! the word graph is trivial.
      //!
      //! \tparam Node1  the type of the nodes of the WordGraph
      //! \p wg.
      //!
      //! \tparam Node2 the type of the node \p source (must satisfy
      //! `sizeof(Node2) <= sizeof(Node1)`).
      //!
      //! \param wg the WordGraph object to check.
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
      //! WordGraph \p wg and \f$n\f$ is the number of edges. Note that for
      //! WordGraph objects the number of edges is always at most \f$mk\f$
      //! where \f$k\f$ is the \ref WordGraph::out_degree.
      //!
      //!
      //! \par Example
      //! \code
      //! WordGraph<size_t> wg;
      //! wg.add_nodes(4);
      //! wg.add_to_out_degree(1);
      //! wg.target(0, 0, 1);
      //! wg.target(1, 0, 0);
      //! wg.target(2, 0, 3);
      //! word_graph::is_acyclic(wg); // returns false
      //! word_graph::is_acyclic(wg, 0); // returns false
      //! word_graph::is_acyclic(wg, 1); // returns false
      //! word_graph::is_acyclic(wg, 2); // returns true
      //! word_graph::is_acyclic(wg, 3); // returns true
      //! \endcode
      // Not noexcept because detail::is_acyclic isn't
      template <typename Node1, typename Node2>
      [[nodiscard]] bool is_acyclic(WordGraph<Node1> const& wg, Node2 source) {
        return is_acyclic(WordGraphView<Node1>(wg), source);
      }

      //! \brief Check if the word graph view induced by the nodes reachable
      //! from a source node and from which a target node can be reached is
      //! acyclic.
      //!
      //! This function returns \c true if the word graph view consisting of the
      //! nodes reachable from \p source and from which \p target is reachable,
      //! in the word graph \p wg, is acyclic; and \c false if not. A word graph
      //! view is acyclic if every directed cycle of the word graph is trivial.
      //!
      //! \tparam Node1  the type of the nodes of the WordGraphView
      //! \p wg.
      //!
      //! \tparam Node2 the type of the nodes \p source and \p target (must
      //! satisfy `sizeof(Node2) <= sizeof(Node1)`).
      //!
      //! \param wg the WordGraphView object to check.
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
      //! WordGraphView \p wg and \f$n\f$ is the number of edges. Note that for
      //! WordGraphView objects the number of edges is always at most \f$mk\f$
      //! where \f$k\f$ is the \ref WordGraphView::out_degree.
      // Not noexcept because detail::is_acyclic isn't
      template <typename Node1, typename Node2>
      [[nodiscard]] bool is_acyclic(WordGraphView<Node1> const& wg,
                                    Node2                       source,
                                    Node2                       target);

      //! \brief Check if the word graph induced by the nodes reachable from a
      //! source node and from which a target node can be reached is acyclic.
      //!
      //! This function returns \c true if the word graph consisting of the
      //! nodes reachable from \p source and from which \p target is reachable,
      //! in the word graph \p wg, is acyclic; and \c false if not. A word graph
      //! is acyclic if every directed cycle of the word graph is trivial.
      //!
      //! \tparam Node1  the type of the nodes of the WordGraph
      //! \p wg.
      //!
      //! \tparam Node2 the type of the nodes \p source and \p target (must
      //! satisfy `sizeof(Node2) <= sizeof(Node1)`).
      //!
      //! \param wg the WordGraph object to check.
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
      //! WordGraph \p wg and \f$n\f$ is the number of edges. Note that for
      //! WordGraph objects the number of edges is always at most \f$mk\f$
      //! where \f$k\f$ is the \ref WordGraph::out_degree.
      // Not noexcept because detail::is_acyclic isn't
      template <typename Node1, typename Node2>
      [[nodiscard]] bool is_acyclic(WordGraph<Node1> const& wg,
                                    Node2                   source,
                                    Node2                   target) {
        return is_acyclic(WordGraphView<Node1>(wg), source, target);
      }

      //! \brief Check if a word graph view is compatible with some relations at
      //! a range of nodes.
      //!
      //! This function returns \c true if the word graph view \p wg is
      //! compatible with the relations in the range \p first_rule to
      //! \p last_rule at every node in the range from \p first_node to
      //! \p last_node. This means that the paths with given sources that are
      //! labelled by one side of a relation leads to the same node as the path
      //! labelled by the other side of the relation.
      //!
      //! \tparam Node  the type of the nodes of the WordGraphView
      //! \p wg.
      //!
      //! \tparam Iterator1 the type of \p first_node.
      //!
      //! \tparam Iterator2 the type of \p last_node.
      //!
      //! \tparam Iterator3 the type of \p first_rule and \p last_rule.
      //!
      //! \param wg the word graph view.
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
      //! \note This function ignores out of bound targets in \p wg (if any).
      //!
      //! \warning
      //! No checks on the arguments of this function are performed.
      // TODO(1) add a version of this function with one that returns a float
      // representing the proportion of the nodes in the range that are
      // compatible with the rules. Don't replace the current version because it
      // can return early knowing that it isn't compatible.
      template <typename Node,
                typename Iterator1,
                typename Iterator2,
                typename Iterator3>
      [[nodiscard]] bool is_compatible_no_checks(WordGraphView<Node> const& wg,
                                                 Iterator1 first_node,
                                                 Iterator2 last_node,
                                                 Iterator3 first_rule,
                                                 Iterator3 last_rule);

      //! \brief Check if a word graph is compatible with some relations at a
      //! range of nodes.
      //!
      //! This function returns \c true if the word graph \p wg is compatible
      //! with the relations in the range \p first_rule to \p last_rule at every
      //! node in the range from \p first_node to \p last_node. This means that
      //! the paths with given sources that are labelled by one side of a
      //! relation leads to the same node as the path labelled by the other side
      //! of the relation.
      //!
      //! \tparam Node  the type of the nodes of the WordGraph
      //! \p wg.
      //!
      //! \tparam Iterator1 the type of \p first_node.
      //!
      //! \tparam Iterator2 the type of \p last_node.
      //!
      //! \tparam Iterator3 the type of \p first_rule and \p last_rule.
      //!
      //! \param wg the word graph.
      //!
      //! \param first_node iterator pointing at the first node.
      //!
      //! \param last_node iterator pointing at one beyond the last node.
      //!
      //! \param first_rule iterator pointing to the first rule.
      //!
      //! \param last_rule iterator pointing one beyond the last rule.
      //!
      //! \return Whether or not the word graph is compatible with the given
      //! rules at each one of the given nodes.
      //!
      //! \exceptions
      //! \no_libsemigroups_except
      //!
      //! \note This function ignores out of bound targets in \p wg (if any).
      //!
      //! \warning
      //! No checks on the arguments of this function are performed.
      template <typename Node,
                typename Iterator1,
                typename Iterator2,
                typename Iterator3>
      [[nodiscard]] bool is_compatible_no_checks(WordGraph<Node> const& wg,
                                                 Iterator1 first_node,
                                                 Iterator2 last_node,
                                                 Iterator3 first_rule,
                                                 Iterator3 last_rule) {
        return is_compatible_no_checks(WordGraphView<Node>(wg),
                                       first_node,
                                       last_node,
                                       first_rule,
                                       last_rule);
      }

      //! \brief Check if a word graph view is compatible with some relations at
      //! a range of nodes.
      //!
      //! This function returns \c true if the word graph view \p wg is
      //! compatible with the relations in the range \p first_rule to
      //! \p last_rule at every node in the range from \p first_node to
      //! \p last_node. This means that the paths with given sources that are
      //! labelled by one side of a relation leads to the same node as the path
      //! labelled by the other side of the relation.
      //!
      //! \tparam Node the type of the nodes of the WordGraphView.
      //! \p wg.
      //!
      //! \tparam Iterator1 the type of \p first_node.
      //!
      //! \tparam Iterator2 the type of \p last_node.
      //!
      //! \tparam Iterator3 the type of \p first_rule and \p last_rule.
      //!
      //! \param wg the word graph view.
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
      //! between \p first_node and \p last_node does not belong to \p wg (i.e.
      //! is greater than or equal to WordGraphView::number_of_nodes).
      //!
      //! \throws LibsemigroupsException if any of the rules in the range
      //! between \p first_rule and \p last_rule contains an invalid label (i.e.
      //! one greater than or equal to WordGraphView::out_degree).
      //!
      //! \note This function ignores out of bound targets in \p wg (if any).
      template <typename Node,
                typename Iterator1,
                typename Iterator2,
                typename Iterator3,
                typename = std::enable_if_t<
                    !std::is_same_v<std::decay_t<Iterator3>, word_type>>>
      [[nodiscard]] bool is_compatible(WordGraphView<Node> const& wg,
                                       Iterator1                  first_node,
                                       Iterator2                  last_node,
                                       Iterator3                  first_rule,
                                       Iterator3                  last_rule);

      //! \brief Check if a word graph is compatible with some relations at a
      //! range of nodes.
      //!
      //! This function returns \c true if the word graph \p wg is compatible
      //! with the relations in the range \p first_rule to \p last_rule at every
      //! node in the range from \p first_node to \p last_node. This means that
      //! the paths with given sources that are labelled by one side of a
      //! relation leads to the same node as the path labelled by the other side
      //! of the relation.
      //!
      //! \tparam Node  the type of the nodes of the WordGraph.
      //! \p wg.
      //!
      //! \tparam Iterator1 the type of \p first_node.
      //!
      //! \tparam Iterator2 the type of \p last_node.
      //!
      //! \tparam Iterator3 the type of \p first_rule and \p last_rule.
      //!
      //! \param wg the word graph.
      //!
      //! \param first_node iterator pointing at the first node.
      //!
      //! \param last_node iterator pointing at one beyond the last node.
      //!
      //! \param first_rule iterator pointing to the first rule.
      //!
      //! \param last_rule iterator pointing one beyond the last rule.
      //!
      //! \return Whether or not the word graph is compatible with the given
      //! rules at each one of the given nodes.
      //!
      //! \throws LibsemigroupsException if any of the nodes in the range
      //! between
      //! \p first_node and \p last_node does not belong to \p wg (i.e. is
      //! greater than or equal to WordGraph::number_of_nodes).
      //!
      //! \throws LibsemigroupsException if any of the rules in the range
      //! between
      //! \p first_rule and \p last_rule contains an invalid label (i.e. one
      //! greater than or equal to WordGraph::out_degree).
      //!
      //! \note This function ignores out of bound targets in \p wg (if any).
      template <typename Node,
                typename Iterator1,
                typename Iterator2,
                typename Iterator3,
                typename = std::enable_if_t<
                    !std::is_same_v<std::decay_t<Iterator3>, word_type>>>
      [[nodiscard]] bool is_compatible(WordGraph<Node> const& wg,
                                       Iterator1              first_node,
                                       Iterator2              last_node,
                                       Iterator3              first_rule,
                                       Iterator3              last_rule) {
        WordGraphView<Node> wgv(wg);
        return is_compatible(wgv, first_node, last_node, first_rule, last_rule);
      }

      //! \brief Check if a word graph view is compatible with a pair of words
      //! for a range of nodes.
      //!
      //! This function returns \c true if the word graph view \p wg is
      //! compatible with the words \p lhs and \p rhs at every node in the range
      //! from
      //! \p first_node to \p last_node. This means that the paths with given
      //! sources that are labelled by \p lhs leads to the same node as the path
      //! labelled by \p rhs.
      //!
      //! \tparam Node the type of the nodes of the WordGraphView \p wg.
      //!
      //! \tparam Iterator1 the type of \p first_node.
      //!
      //! \tparam Iterator2 the type of \p last_node.
      //!
      //! \param wg the word graph view.
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
      //! \note This function ignores out of bound targets in \p wg (if any).
      //!
      //! \warning This function does not check that its arguments are valid.
      template <typename Node, typename Iterator1, typename Iterator2>
      bool is_compatible_no_checks(WordGraphView<Node> const& wg,
                                   Iterator1                  first_node,
                                   Iterator2                  last_node,
                                   word_type const&           lhs,
                                   word_type const&           rhs);

      //! \brief Check if a word graph is compatible with a pair of words for a
      //! range of nodes.
      //!
      //! This function returns \c true if the word graph \p wg is compatible
      //! with the words \p lhs and \p rhs at every node in the range from
      //! \p first_node to \p last_node. This means that the paths with given
      //! sources that are labelled by \p lhs leads to the same node as the path
      //! labelled by \p rhs.
      //!
      //! \tparam Node the type of the nodes of the WordGraph \p wg.
      //!
      //! \tparam Iterator1 the type of \p first_node.
      //!
      //! \tparam Iterator2 the type of \p last_node.
      //!
      //! \param wg the word graph.
      //!
      //! \param first_node iterator pointing at the first node.
      //!
      //! \param last_node iterator pointing at one beyond the last node.
      //!
      //! \param lhs the first rule.
      //!
      //! \param rhs the second rule.
      //!
      //! \return Whether or not the word graph is compatible with the given
      //! rules at each one of the given nodes.
      //!
      //! \note This function ignores out of bound targets in \p wg (if any).
      //!
      //! \warning This function does not check that its arguments are valid.
      template <typename Node, typename Iterator1, typename Iterator2>
      bool is_compatible_no_checks(WordGraph<Node> const& wg,
                                   Iterator1              first_node,
                                   Iterator2              last_node,
                                   word_type const&       lhs,
                                   word_type const&       rhs) {
        return is_compatible_no_checks(
            WordGraphView<Node>(wg), first_node, last_node, lhs, rhs);
      }

      //! \brief Check if a word graph view is compatible with a pair of words
      //! for a range of nodes.
      //!
      //! This function returns \c true if the word graph view \p wg is
      //! compatible with the words \p lhs and \p rhs at every node in the range
      //! from
      //! \p first_node to \p last_node. This means that the paths with given
      //! sources that are labelled by \p lhs leads to the same node as the path
      //! labelled by \p rhs.
      //!
      //! \tparam Node the type of the nodes of the WordGraphView \p wg.
      //!
      //! \tparam Iterator1 the type of \p first_node.
      //!
      //! \tparam Iterator2 the type of \p last_node.
      //!
      //! \param wg the word graph view.
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
      //! \p first_node and \p last_node does not belong to \p wg (i.e. is
      //! greater than or equal to WordGraphView::number_of_nodes).
      //!
      //! \throws LibsemigroupsException if any of the rules in the range
      //! between
      //! \p first_rule and \p last_rule contains an invalid label (i.e. one
      //! greater than or equal to WordGraphView::out_degree).
      template <typename Node, typename Iterator1, typename Iterator2>
      bool is_compatible(WordGraphView<Node> const& wg,
                         Iterator1                  first_node,
                         Iterator2                  last_node,
                         word_type const&           lhs,
                         word_type const&           rhs);

      //! \brief Check if a word graph is compatible with a pair of words for a
      //! range of nodes.
      //!
      //! This function returns \c true if the word graph \p wg is compatible
      //! with the words \p lhs and \p rhs at every node in the range from
      //! \p first_node to \p last_node. This means that the paths with given
      //! sources that are labelled by \p lhs leads to the same node as the path
      //! labelled by \p rhs.
      //!
      //! \tparam Node the type of the nodes of the WordGraph \p wg.
      //!
      //! \tparam Iterator1 the type of \p first_node.
      //!
      //! \tparam Iterator2 the type of \p last_node.
      //!
      //! \param wg the word graph.
      //!
      //! \param first_node iterator pointing at the first node.
      //!
      //! \param last_node iterator pointing at one beyond the last node.
      //!
      //! \param lhs the first rule.
      //!
      //! \param rhs the second rule.
      //!
      //! \return Whether or not the word graph is compatible with the given
      //! rules at each one of the given nodes.
      //!
      //! \throws LibsemigroupsException if any of the nodes in the range
      //! between
      //! \p first_node and \p last_node does not belong to \p wg (i.e. is
      //! greater than or equal to WordGraph::number_of_nodes).
      //!
      //! \throws LibsemigroupsException if any of the rules in the range
      //! between
      //! \p first_rule and \p last_rule contains an invalid label (i.e. one
      //! greater than or equal to WordGraph::out_degree).
      template <typename Node, typename Iterator1, typename Iterator2>
      bool is_compatible(WordGraph<Node> const& wg,
                         Iterator1              first_node,
                         Iterator2              last_node,
                         word_type const&       lhs,
                         word_type const&       rhs) {
        return is_compatible(
            WordGraphView<Node>(wg), first_node, last_node, lhs, rhs);
      }

      /*
      //! \brief Check if a word graph or word graph view is compatible with
      //! some relations at a range of nodes.
      //!
      //! This function returns \c true if the word graph \p wg is compatible
      //! with the relations in the range \p first_rule to \p last_rule at every
      //! node in the range from \p first_node to \p last_node. This means that
      //! the paths with given sources that are labelled by one side of a
      //! relation leads to the same node as the path labelled by the other side
      //! of the relation.
      //!
      //! \tparam Node  the type of the nodes of the WordGraph.
      //! \p wg.
      //!
      //! \tparam Iterator1 the type of \p first_node.
      //!
      //! \tparam Iterator2 the type of \p last_node.
      //!
      //! \tparam Iterator3 the type of \p first_rule and \p last_rule.
      //!
      //! \param wg the word graph.
      //!
      //! \param first_node iterator pointing at the first node.
      //!
      //! \param last_node iterator pointing at one beyond the last node.
      //!
      //! \param first_rule iterator pointing to the first rule.
      //!
      //! \param last_rule iterator pointing one beyond the last rule.
      //!
      //! \return Whether or not the word graph is compatible with the given
      //! rules at each one of the given nodes.
      //!
      //! \throws LibsemigroupsException if any of the nodes in the range
      //! between
      //! \p first_node and \p last_node does not belong to \p wg (i.e. is
      //! greater than or equal to WordGraph::number_of_nodes).
      //!
      //! \throws LibsemigroupsException if any of the rules in the range
      //! between
      //! \p first_rule and \p last_rule contains an invalid label (i.e. one
      //! greater than or equal to WordGraph::out_degree).
      //!
      //! \note This function ignores out of bound targets in \p wg (if any).
      TODO(0): Remove or delete
      template <typename WordGraphType,
                typename Iterator1,
                typename Iterator2,
                typename Iterator3,
                typename = std::enable_if_t<
                    std::is_same_v<WordGraphType,
                                   WordGraph<typename
                                   WordGraphType::node_type>>
                    || std::is_same_v<
                        WordGraphType,
                        WordGraphView<typename WordGraphType::node_type>>>>
      bool is_compatible(WordGraphType const& wg,
                         Iterator1            first_node,
                         Iterator2            last_node,
                         Iterator3            first_rule,
                         Iterator3            last_rule) {
        if constexpr (std::is_same_v<
                          WordGraphType,
                          WordGraph<typename WordGraphType::node_type>>) {
          return is_compatible(
              static_cast<WordGraph<typename WordGraphType::node_type>
              const&>(
                  wg),
              first_node,
              last_node,
              first_rule,
              last_rule);
        } else {
          return is_compatible(
              static_cast<
                  WordGraphView<typename WordGraphType::node_type>
                  const&>(wg),
              first_node,
              last_node,
              first_rule,
              last_rule);
        }
      }
      */

      //! \brief Check if every node in a range has exactly
      //! WordGraphView::out_degree out-edges.
      //!
      //! This function returns \c true if every node in the range defined by
      //! \p first_node and \p last_node is complete, meaning that every such
      //! node is the source of an edge with every possible label.
      //!
      //! \tparam Node the type of the nodes in the word graph view.
      //!
      //! \tparam Iterator1 the type of \p first_node.
      //!
      //! \tparam Iterator2 the type of \p last_node.
      //!
      //! \param wg the word graph view.
      //!
      //! \param first_node iterator pointing to the first node in the range.
      //!
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
      [[nodiscard]] bool is_complete_no_checks(WordGraphView<Node> const& wg,
                                               Iterator1 first_node,
                                               Iterator2 last_node);

      //! \brief Check if every node in a range has exactly
      //! WordGraph::out_degree out-edges.
      //!
      //! This function returns \c true if every node in the range defined by
      //! \p first_node and \p last_node is complete, meaning that every such
      //! node is the source of an edge with every possible label.
      //!
      //! \tparam Node the type of the nodes in the word graph.
      //!
      //! \tparam Iterator1 the type of \p first_node.
      //!
      //! \tparam Iterator2 the type of \p last_node.
      //!
      //! \param wg the word graph.
      //!
      //! \param first_node iterator pointing to the first node in the range.
      //!
      //! \param last_node iterator pointing one beyond the last node in the
      //! range.
      //!
      //! \returns
      //! Whether or not the word graph is complete on the given range of nodes.
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
      [[nodiscard]] bool is_complete_no_checks(WordGraph<Node> const& wg,
                                               Iterator1 first_node,
                                               Iterator2 last_node) {
        return is_complete_no_checks(
            WordGraphView<Node>(wg), first_node, last_node);
      }

      //! \brief Check if every node in a range has exactly
      //! WordGraphView::out_degree out-edges.
      //!
      //! This function returns \c true if every node in the range defined by
      //! \p first_node and \p last_node is complete, meaning that
      //! every such node is the source of an edge with every possible label.
      //!
      //! \tparam Node the type of the nodes in the word graph view.
      //!
      //! \tparam Iterator1 the type of \p first_node.
      //!
      //! \tparam Iterator2 the type of \p last_node.
      //!
      //! \param wg the word graph view.
      //!
      //! \param first_node iterator pointing to the first node in the range.
      //!
      //! \param last_node iterator pointing one beyond the last node in the
      //! range.
      //!
      //! \returns Whether or not the word graph view is complete on the given
      //! range of nodes.
      //!
      //! \throws LibsemigroupsException if any item in the range defined by
      //! \p first_node and \p last_node is not a node of \p wg.
      //!
      //! \complexity
      //! \f$O(mn)\f$ where \c m is the number of nodes in the range and \c n is
      //! out_degree().
      template <typename Node, typename Iterator1, typename Iterator2>
      [[nodiscard]] bool is_complete(WordGraphView<Node> const& wg,
                                     Iterator1                  first_node,
                                     Iterator2                  last_node);

      //! \brief Check if every node in a range has exactly
      //! WordGraph::out_degree out-edges.
      //!
      //! This function returns \c true if every node in the range defined by
      //! \p first_node and \p last_node is complete, meaning that
      //! every such node is the source of an edge with every possible label.
      //!
      //! \tparam Node the type of the nodes in the word graph.
      //!
      //! \tparam Iterator1 the type of \p first_node.
      //!
      //! \tparam Iterator2 the type of \p last_node.
      //!
      //! \param wg the word graph.
      //!
      //! \param first_node iterator pointing to the first node in the range.
      //!
      //! \param last_node iterator pointing one beyond the last node in the
      //! range.
      //!
      //! \returns Whether or not the word graph is complete on the given
      //! range of nodes.
      //!
      //! \throws LibsemigroupsException if any item in the range defined by
      //! \p first_node and \p last_node is not a node of \p wg.
      //!
      //! \complexity
      //! \f$O(mn)\f$ where \c m is the number of nodes in the range and \c n is
      //! out_degree().
      template <typename Node, typename Iterator1, typename Iterator2>
      [[nodiscard]] bool is_complete(WordGraph<Node> const& wg,
                                     Iterator1              first_node,
                                     Iterator2              last_node) {
        return is_complete(WordGraphView<Node>(wg), first_node, last_node);
      }

      //! \brief Check if every node has exactly WordGraphView::out_degree
      //! out-edges.
      //!
      //! This function returns \c true if a WordGraphView is complete, meaning
      //! that every node is the source of an edge with every possible label.
      //!
      //! \tparam Node the type of the nodes in the word graph view.
      //!
      //! \param wg the word graph view.
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
      is_complete_no_checks(WordGraphView<Node> const& wg) noexcept {
        return wg.number_of_edges_no_checks()
               == wg.number_of_nodes_no_checks() * wg.out_degree_no_checks();
      }

      template <typename Node>
      [[nodiscard]] bool is_complete(WordGraphView<Node> const& wg) {
        wg.throw_if_invalid_view();
        return is_complete_no_checks(wg);
      }

      //! \brief Check if every node has exactly WordGraph::out_degree
      //! out-edges.
      //!
      //! This function returns \c true if a WordGraph is complete, meaning that
      //! every node is the source of an edge with every possible label.
      //!
      //! \tparam Node the type of the nodes in the word graph.
      //!
      //! \param wg the word graph.
      //!
      //! \returns
      //! Whether or not the word graph is complete.
      //!
      //! \exceptions
      //! \noexcept
      //!
      //! \complexity
      //! \f$O(mn)\f$ where \c m is number_of_nodes() and \c n is out_degree().
      template <typename Node>
      [[nodiscard]] bool is_complete(WordGraph<Node> const& wg) noexcept {
        return is_complete(WordGraphView<Node>(wg));
      }

      //! \brief Check if a word graph view is connected.
      //!
      //! This function returns \c true if the word graph view \p wg is
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
      //! \param wg the word graph view.
      //!
      //! \returns
      //! Whether or not the word graph view is connected.
      //!
      //! \exceptions
      //! \no_libsemigroups_except
      //!
      //! \note If any target of any edge in the word graph view \p wg that is
      //! out of bounds, then this is ignored by this function.
      template <typename Node>
      [[nodiscard]] bool is_connected(WordGraphView<Node> const& wg);

      //! \brief Check if a word graph is connected.
      //!
      //! This function returns \c true if the word graph \p wg is connected and
      //! \c false if it is not. A word graph is *connected* if for every pair
      //! of nodes \c s and \c t in the graph there exists a sequence \f$u_0 =
      //! s,
      //! \ldots, u_{n}= t\f$ for some \f$n\in \mathbb{N}\f$ such that for every
      //! \f$i\f$ there exists a label \c a such that \f$(u_i, a, u_{i + 1})\f$
      //! or
      //! \f$(u_{i + 1}, a, u_i)\f$ is an edge in the graph.
      //!
      //! \tparam Node the type of the nodes in the word graph.
      //!
      //! \param wg the word graph.
      //!
      //! \returns
      //! Whether or not the word graph is connected.
      //!
      //! \exceptions
      //! \no_libsemigroups_except
      //!
      //! \note If any target of any edge in the word graph \p wg that is out of
      //! bounds, then this is ignored by this function.
      template <typename Node>
      [[nodiscard]] bool is_connected(WordGraph<Node> const& wg) {
        return is_connected(WordGraphView<Node>(wg));
      }

      //! \brief Check if there is a path from one node to another.
      //!
      //! This function returns \c true if there is a path from the node
      //! \p source to the node \p target in the word graph view \p wg.
      //!
      //! \tparam Node1 the type of the nodes in the WordGraphView.
      //!
      //! \tparam Node 2 the types of \p source and \p target (must
      //! satisfy `sizeof(Node2) <= sizeof(Node1)`).
      //!
      //! \param wg the WordGraphView object to check.
      //! \param source the source node.
      //! \param target the source node.
      //!
      //! \returns
      //! Whether or not the node \p target is reachable from the node \p source
      //! in the word graph view \p wg.
      //!
      //! \exceptions
      //! \no_libsemigroups_except
      //!
      //! \par Complexity
      //! \f$O(m + n)\f$ where \f$m\f$ is the number of nodes in the
      //! WordGraphView \p wg and \f$n\f$ is the number of edges. Note that for
      //! WordGraphView objects the number of edges is always at most \f$mk\f$
      //! where \f$k\f$ is the WordGraphView::out_degree.
      //!
      //! \note
      //! If \p source and \p target are equal, then, by convention, we consider
      //! \p target to be reachable from \p source, via the empty path.
      //!
      //! \note This function ignores out of bound targets in \p wg (if any).
      //!
      //! \warning No checks are performed on the arguments.
      //!
      //! \par Example
      //! \code
      //! WordGraph<size_t> g;
      //! g.add_nodes(4);
      //! g.add_to_out_degree(1);
      //! g.target(0, 1, 0);
      //! g.target(1, 0, 0);
      //! g.target(2, 3, 0);
      //! WordGraphView<size_t> wg(g);
      //! word_graph::is_reachable_no_checks(wg, 0, 1); // returns true
      //! word_graph::is_reachable_no_checks(wg, 1, 0); // returns true
      //! word_graph::is_reachable_no_checks(wg, 1, 2); // returns false
      //! word_graph::is_reachable_no_checks(wg, 2, 3); // returns true
      //! word_graph::is_reachable_no_checks(wg, 3, 2); // returns false
      //! \endcode
      template <typename Node1, typename Node2>
      [[nodiscard]] bool is_reachable_no_checks(WordGraphView<Node1> const& wg,
                                                Node2 source,
                                                Node2 target);

      //! \brief Check if there is a path from one node to another.
      //!
      //! This function returns \c true if there is a path from the nod
      //! \p source to the node \p target in the word graph \p wg.
      //!
      //! \tparam Node1 the type of the nodes in the WordGraph.
      //!
      //! \tparam Node 2 the types of \p source and \p target (must
      //! satisfy `sizeof(Node2) <= sizeof(Node1)`).
      //!
      //! \param wg the WordGraph object to check.
      //! \param source the source node.
      //! \param target the source node.
      //!
      //! \returns
      //! Whether or not the node \p target is reachable from the node \p source
      //! in the word graph \p wg.
      //!
      //! \exceptions
      //! \no_libsemigroups_except
      //!
      //! \par Complexity
      //! \f$O(m + n)\f$ where \f$m\f$ is the number of nodes in the
      //! WordGraph \p wg and \f$n\f$ is the number of edges. Note that for
      //! WordGraph objects the number of edges is always at most \f$mk\f$
      //! where \f$k\f$ is the WordGraph::out_degree.
      //!
      //! \note
      //! If \p source and \p target are equal, then, by convention, we consider
      //! \p target to be reachable from \p source, via the empty path.
      //!
      //! \note This function ignores out of bound targets in \p wg (if any).
      //!
      //! \warning No checks are performed on the arguments.
      //!
      //! \par Example
      //! \code
      //! WordGraph<size_t> wg;
      //! wg.add_nodes(4);
      //! wg.add_to_out_degree(1);
      //! wg.target(0, 1, 0);
      //! wg.target(1, 0, 0);
      //! wg.target(2, 3, 0);
      //! word_graph::is_reachable_no_checks(wg, 0, 1); // returns true
      //! word_graph::is_reachable_no_checks(wg, 1, 0); // returns true
      //! word_graph::is_reachable_no_checks(wg, 1, 2); // returns false
      //! word_graph::is_reachable_no_checks(wg, 2, 3); // returns true
      //! word_graph::is_reachable_no_checks(wg, 3, 2); // returns false
      //! \endcode
      template <typename Node1, typename Node2>
      [[nodiscard]] bool is_reachable_no_checks(WordGraph<Node1> const& wg,
                                                Node2                   source,
                                                Node2 target) {
        return is_reachable_no_checks(WordGraphView<Node1>(wg), source, target);
      }

      //! \brief Check if there is a path from one node to another.
      //!
      //! This function returns \c true if there is a path from the node
      //! \p source to the node \p target in the word graph view \p wg.
      //!
      //! \tparam Node1 the type of the nodes in the WordGraphView.
      //!
      //! \tparam Node 2 the types of \p source and \p target (must
      //! satisfy `sizeof(Node2) <= sizeof(Node1)`).
      //!
      //! \param wg the WordGraphView object to check.
      //! \param source the source node.
      //! \param target the source node.
      //!
      //! \returns
      //! Whether or not the node \p target is reachable from the node \p source
      //! in the word graph view \p wg.
      //!
      //! \throws LibsemigroupsException if \p source or \p target is out of
      //! bounds.
      //! \throws LibsemigroupsException if any target in \p wg is out of
      //! bounds.
      //!
      //! \par Complexity
      //! \f$O(m + n)\f$ where \f$m\f$ is the number of nodes in the
      //! WordGraphView \p wg and \f$n\f$ is the number of edges. Note that for
      //! WordGraphView objects the number of edges is always at most \f$mk\f$
      //! where \f$k\f$ is the WordGraphView::out_degree.
      //!
      //! \note
      //! If \p source and \p target are equal, then, by convention, we consider
      //! \p target to be reachable from \p source, via the empty path.
      template <typename Node1, typename Node2>
      [[nodiscard]] bool is_reachable(WordGraphView<Node1> const& wg,
                                      Node2                       source,
                                      Node2                       target);

      //! \brief Check if there is a path from one node to another.
      //!
      //! This function returns \c true if there is a path from the node
      //! \p source to the node \p target in the word graph \p wg.
      //!
      //! \tparam Node1 the type of the nodes in the WordGraph.
      //!
      //! \tparam Node 2 the types of \p source and \p target (must
      //! satisfy `sizeof(Node2) <= sizeof(Node1)`).
      //!
      //! \param wg the WordGraph object to check.
      //! \param source the source node.
      //! \param target the source node.
      //!
      //! \returns
      //! Whether or not the node \p target is reachable from the node \p source
      //! in the word graph \p wg.
      //!
      //! \throws LibsemigroupsException if \p source or \p target is out of
      //! bounds.
      //! \throws LibsemigroupsException if any target in \p wg is out of
      //! bounds.
      //!
      //! \par Complexity
      //! \f$O(m + n)\f$ where \f$m\f$ is the number of nodes in the
      //! WordGraph \p wg and \f$n\f$ is the number of edges. Note that for
      //! WordGraph objects the number of edges is always at most \f$mk\f$
      //! where \f$k\f$ is the WordGraph::out_degree.
      //!
      //! \note
      //! If \p source and \p target are equal, then, by convention, we consider
      //! \p target to be reachable from \p source, via the empty path.
      template <typename Node1, typename Node2>
      [[nodiscard]] bool is_reachable(WordGraph<Node1> const& wg,
                                      Node2                   source,
                                      Node2                   target) {
        return is_reachable(WordGraphView<Node1>(wg), source, target);
      }

      //! \brief Check if every node is reachable from some node.
      //!
      //! This function returns \c true if there exists a node in \p wg from
      //! which every other node is reachable; and \c false otherwise.
      //! A word graph view is *strictly cyclic* if there exists a node \f$v\f$
      //! from which every node is reachable (including \f$v\f$). There must be
      //! a path of length at least \f$1\f$ from the original node \f$v\f$ to
      //! itself (i.e. \f$v\f$ is not considered to be reachable from itself by
      //! default).
      //!
      //! \tparam Node the type of the nodes of the WordGraphView.
      //!
      //! \param wg the WordGraphView object to check.
      //!
      //! \returns
      //! A value of type `bool`.
      //!
      //! \throws LibsemigroupsException if any target in \p wg is out of
      //! bounds.
      //!
      //! \par Complexity
      //! \f$O(m + n)\f$ where \f$m\f$ is the number of nodes in the
      //! WordGraphView \p wg and \f$n\f$ is the number of edges. Note that for
      //! WordGraphView objects the number of edges is always at most \f$mk\f$
      //! where \f$k\f$ is the WordGraphView::out_degree.
      //!
      //! \par Example
      //! \code
      //! auto wg = make<WordGraph<uint8_t>>(
      //!     5, {{0, 0}, {1, 1}, {2}, {3, 3}});
      //! word_graph::is_strictly_cyclic(WordGraphView<uint8_t>(wg));  //
      //! returns false \endcode
      // TODO(1) should have a version that returns the node that everything is
      // reachable from
      template <typename Node>
      [[nodiscard]] bool is_strictly_cyclic(WordGraphView<Node> const& wg);

      //! \brief Check if every node is reachable from some node.
      //!
      //! This function returns \c true if there exists a node in \p wg from
      //! which every other node is reachable; and \c false otherwise.
      //! A word graph is *strictly cyclic* if there exists a node \f$v\f$ from
      //! which every node is reachable (including \f$v\f$). There must be a
      //! path of length at least \f$1\f$ from the original node \f$v\f$ to
      //! itself (i.e. \f$v\f$ is not considered to be reachable from itself by
      //! default).
      //!
      //! \tparam Node the type of the nodes of the WordGraph.
      //!
      //! \param wg the WordGraph object to check.
      //!
      //! \returns
      //! A value of type `bool`.
      //!
      //! \throws LibsemigroupsException if any target in \p wg is out of
      //! bounds.
      //!
      //! \par Complexity
      //! \f$O(m + n)\f$ where \f$m\f$ is the number of nodes in the
      //! WordGraph \p wg and \f$n\f$ is the number of edges. Note that for
      //! WordGraph objects the number of edges is always at most \f$mk\f$
      //! where \f$k\f$ is the WordGraph::out_degree.
      //!
      //! \par Example
      //! \code
      //! auto wg = make<WordGraph<uint8_t>>(
      //!     5, {{0, 0}, {1, 1}, {2}, {3, 3}});
      //! word_graph::is_strictly_cyclic(wg);  // returns false
      //! \endcode
      // TODO(1) should have a version that returns the node that everything is
      // reachable from
      template <typename Node>
      [[nodiscard]] bool is_strictly_cyclic(WordGraph<Node> const& wg) {
        return is_strictly_cyclic(WordGraphView<Node>(wg));
      }

      //! \brief Returns the last node on the path labelled by a word and an
      //! iterator to the position in the word reached.
      //!
      //! \tparam Node1 the node type of the word graph.
      //! \tparam Node2 the type of the node \p source.
      //! \tparam Iterator the type of the iterators into a word.
      //!
      //! \param wg a word graph view.
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
      //! assumed that \p source is a node in the word graph view \p wg; and
      //! that the letters in the word described by \p first and \p last belong
      //! to the range \c 0 to WordGraphView::out_degree.
      template <typename Node1, typename Node2, typename Iterator>
      [[nodiscard]] std::pair<Node1, Iterator>
      last_node_on_path_no_checks(WordGraphView<Node1> const& wg,
                                  Node2                       source,
                                  Iterator                    first,
                                  Iterator                    last) noexcept;

      //! \brief Returns the last node on the path labelled by a word and an
      //! iterator to the position in the word reached.
      //!
      //! \tparam Node1 the node type of the word graph.
      //! \tparam Node2 the type of the node \p source.
      //! \tparam Iterator the type of the iterators into a word.
      //!
      //! \param wg a word graph.
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
      //! assumed that \p source is a node in the word graph \p wg; and that the
      //! letters in the word described by \p first and \p last belong to the
      //! range \c 0 to WordGraph::out_degree.
      template <typename Node1, typename Node2, typename Iterator>
      [[nodiscard]] std::pair<Node1, Iterator>
      last_node_on_path_no_checks(WordGraph<Node1> const& wg,
                                  Node2                   source,
                                  Iterator                first,
                                  Iterator                last) noexcept {
        return last_node_on_path_no_checks(
            WordGraphView<Node1>(wg), source, first, last);
      }

      //! \brief Returns the last node on the path labelled by a word and an
      //! iterator to the position in the word reached.
      //!
      //! \tparam Node1 the node type of the word graph view.
      //! \tparam Node2 the type of the node \p source.
      //! \tparam Iterator the type of the iterators into a word.
      //!
      //! \param wg a word graph view.
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
      //! \note If any value in \p wg or in the word described by \p first and
      //! \p last is out of bounds (greater than or equal to
      //! WordGraphView::number_of_nodes), the path labelled by the word exits
      //! the word graph view, which is reflected in the result value of this
      //! function, but does not cause an exception to be thrown.
      template <typename Node1, typename Node2, typename Iterator>
      [[nodiscard]] std::pair<Node1, Iterator>
      last_node_on_path(WordGraphView<Node1> const& wg,
                        Node2                       source,
                        Iterator                    first,
                        Iterator                    last);

      //! \brief Returns the last node on the path labelled by a word and an
      //! iterator to the position in the word reached.
      //!
      //! \tparam Node1 the node type of the word graph.
      //! \tparam Node2 the type of the node \p source.
      //! \tparam Iterator the type of the iterators into a word.
      //!
      //! \param wg a word graph.
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
      //! \note If any value in \p wg or in the word described by \p first and
      //! \p last is out of bounds (greater than or equal to
      //! WordGraph::number_of_nodes), the path labelled by the word exits the
      //! word graph, which is reflected in the result value of this function,
      //! but does not cause an exception to be thrown.
      template <typename Node1, typename Node2, typename Iterator>
      [[nodiscard]] std::pair<Node1, Iterator>
      last_node_on_path(WordGraph<Node1> const& wg,
                        Node2                   source,
                        Iterator                first,
                        Iterator                last) {
        return last_node_on_path(WordGraphView<Node1>(wg), source, first, last);
      }

      //! \brief Returns the last node on the path labelled by a word and an
      //! iterator to the position in the word reached.
      //!
      //! \tparam Node1 the node type of the word graph view.
      //! \tparam Node2 the type of the node \p source.
      //!
      //! \param wg a word graph view.
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
      //! assumed that \p source is a node in the word graph view \p wg; and
      //! that the letters in the word described by \p first and \p last belong
      //! to the range \c 0 to WordGraphView::out_degree.
      template <typename Node1, typename Node2>
      std::pair<Node1, word_type::const_iterator>
      last_node_on_path_no_checks(WordGraphView<Node1> const& wg,
                                  Node2                       source,
                                  word_type const&            w);

      //! \brief Returns the last node on the path labelled by a word and an
      //! iterator to the position in the word reached.
      //!
      //! \tparam Node1 the node type of the word graph.
      //! \tparam Node2 the type of the node \p source.
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
      template <typename Node1, typename Node2>
      std::pair<Node1, word_type::const_iterator>
      last_node_on_path_no_checks(WordGraph<Node1> const& wg,
                                  Node2                   source,
                                  word_type const&        w) {
        return last_node_on_path_no_checks(WordGraphView<Node1>(wg), source, w);
      }

      //! \brief Returns the last node on the path labelled by a word and an
      //! iterator to the position in the word reached.
      //!
      //! \tparam Node1 the node type of the word graph view.
      //! \tparam Node2 the type of the node \p source.
      //!
      //! \param wg a word graph view.
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
      //! WordGraphView::number_of_nodes), the path labelled by the word exits
      //! the word graph view, which is reflected in the result value of this
      //! function, but does not cause an exception to be thrown.
      template <typename Node1, typename Node2>
      std::pair<Node1, word_type::const_iterator>
      last_node_on_path(WordGraphView<Node1> const& wg,
                        Node2                       source,
                        word_type const&            w);

      //! \brief Returns the last node on the path labelled by a word and an
      //! iterator to the position in the word reached.
      //!
      //! \tparam Node1 the node type of the word graph.
      //! \tparam Node2 the type of the node \p source.
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
      template <typename Node1, typename Node2>
      std::pair<Node1, word_type::const_iterator>
      last_node_on_path(WordGraph<Node1> const& wg,
                        Node2                   source,
                        word_type const&        w) {
        return last_node_on_path(WordGraphView<Node1>(wg), source, w);
      }

      //! \brief Returns the std::unordered_set of nodes reachable from a given
      //! node in a word graph view.
      //!
      //! This function returns a std::unordered_set consisting of all the nodes
      //! in the word graph view \p wg that are reachable from \p source.
      //!
      //! \tparam Node1 the node type of the word graph view.
      //! \tparam Node2 the type of the node \p source.
      //!
      //! \param wg the word graph view.
      //! \param source the source node.
      //!
      //! \returns A std::unordered_set consisting of all the nodes in the word
      //! graph view \p wg that are reachable from \p source.
      //!
      //! \throws LibsemigroupsException if \p source is out of bounds (greater
      //! than or equal to WordGraphView::number_of_nodes).
      //!
      //! \note If any target of any edge in the word graph view \p wg that is
      //! out of bounds, then this is ignored by this function.
      // TODO(1) tests
      // TODO(1) version where std::unordered_set is passed by reference, or
      // make this a class that stores its stack and unordered_set, not clear
      // why we'd single out the unordered_set to be passed by reference.
      // TODO(2) version which is an iterator i.e. returns an iterator or range
      // object that allows use to step through the nodes reachable from a given
      // node
      template <typename Node1, typename Node2>
      [[nodiscard]] std::unordered_set<Node1>
      nodes_reachable_from(WordGraphView<Node1> const& wg, Node2 source);

      //! \brief Returns the std::unordered_set of nodes reachable from a given
      //! node in a word graph.
      //!
      //! This function returns a std::unordered_set consisting of all the nodes
      //! in the word graph \p wg that are reachable from \p source.
      //!
      //! \tparam Node1 the node type of the word graph.
      //! \tparam Node2 the type of the node \p source.
      //!
      //! \param wg the word graph.
      //! \param source the source node.
      //!
      //! \returns A std::unordered_set consisting of all the nodes in the word
      //! graph \p wg that are reachable from \p source.
      //!
      //! \throws LibsemigroupsException if \p source is out of bounds (greater
      //! than or equal to WordGraph::number_of_nodes).
      //!
      //! \note If any target of any edge in the word graph \p wg that is out of
      //! bounds, then this is ignored by this function.
      // TODO(1) tests
      // TODO(1) version where std::unordered_set is passed by reference, or
      // make this a class that stores its stack and unordered_set, not clear
      // why we'd single out the unordered_set to be passed by reference.
      // TODO(2) version which is an iterator i.e. returns an iterator or range
      // object that allows use to step through the nodes reachable from a given
      // node
      template <typename Node1, typename Node2>
      [[nodiscard]] std::unordered_set<Node1>
      nodes_reachable_from(WordGraph<Node1> const& wg, Node2 source) {
        return nodes_reachable_from(WordGraphView<Node1>(wg), source);
      }

      //! \brief Returns the std::unordered_set of nodes that can reach a given
      //! node in a word graph.
      //!
      //! This function returns a std::unordered_set consisting of all the nodes
      //! in the word graph \p wg that can reach \p target. This function can be
      //! thought of like an inverse of `nodes_reachable_from`, in the sense
      //! that the node `a` \f$\in\f$ `ancestor_of(b)` for some node `b` if and
      //! only if `b` \f$\in\f$ `nodes_reachable_from(a)`.
      //!
      //! \tparam Node1 the node type of the word graph.
      //! \tparam Node2 the type of the node \p target.
      //!
      //! \param wg the word graph.
      //! \param target the target node.
      //!
      //! \returns A std::unordered_set consisting of all the nodes in the word
      //! graph \p wg that can reach \p target.
      //!
      //! \throws LibsemigroupsException if \p target is out of bounds (greater
      //! than or equal to WordGraphView::number_of_nodes).
      template <typename Node1, typename Node2>
      [[nodiscard]] std::unordered_set<Node1>
      ancestors_of(WordGraphView<Node1> const& wg, Node2 target);

      //! \brief Returns the std::unordered_set of nodes that can reach a given
      //! node in a word graph.
      //!
      //! This function returns a std::unordered_set consisting of all the nodes
      //! in the word graph \p wg that can reach \p target. This function can be
      //! thought of like an inverse of `nodes_reachable_from`, in the sense
      //! that the node `a` \f$\in\f$ `ancestor_of(b)` for some node `b` if and
      //! only if `b` \f$\in\f$ `nodes_reachable_from(a)`.
      //!
      //! \tparam Node1 the node type of the word graph.
      //! \tparam Node2 the type of the node \p target.
      //!
      //! \param wg the word graph.
      //! \param target the target node.
      //!
      //! \returns A std::unordered_set consisting of all the nodes in the word
      //! graph \p wg that can reach \p target.
      //!
      //! \throws LibsemigroupsException if \p target is out of bounds (greater
      //! than or equal to WordGraph::number_of_nodes).
      template <typename Node1, typename Node2>
      [[nodiscard]] std::unordered_set<Node1>
      ancestors_of(WordGraph<Node1> const& wg, Node2 target) {
        return ancestors_of(WordGraphView<Node1>(wg), target);
      }

      //! \brief Returns the std::unordered_set of nodes reachable from a given
      //! node in a word graph.
      //!
      //! This function returns a std::unordered_set consisting of all the nodes
      //! in the word graph \p wg that are reachable from \p source.
      //!
      //! \tparam Node1 the node type of the word graph.
      //! \tparam Node2 the type of the node \p source.
      //!
      //! \param wg the word graph.
      //! \param source the source node.
      //!
      //! \returns A std::unordered_set consisting of all the nodes in the word
      //! graph \p wg that are reachable from \p source.
      //!
      //! \note If any target of any edge in the word graph \p wg that is out of
      //! bounds, then this is ignored by this function.
      //!
      //! \warning The arguments are not checked, and in particular it is
      //! assumed that \p source is a node of \p wg (i.e. less than
      //! WordGraph::number_of_nodes).
      template <typename Node1, typename Node2>
      [[nodiscard]] std::unordered_set<Node1>
      nodes_reachable_from_no_checks(WordGraph<Node1> const& wg, Node2 source);

      //! \brief Returns the std::unordered_set of nodes that can reach a given
      //! node in a word graph.
      //!
      //! This function returns a std::unordered_set consisting of all the nodes
      //! in the word graph \p wg that can reach \p target. This function can be
      //! thought of like an inverse of `nodes_reachable_from`, in the sense
      //! that the node `a` \f$\in\f$ `ancestor_of(b)` for some node `b` if and
      //! only if `b` \f$\in\f$ `nodes_reachable_from(a)`.
      //!
      //! \tparam Node1 the node type of the word graph.
      //! \tparam Node2 the type of the node \p target.
      //!
      //! \param wg the word graph.
      //! \param target the target node.
      //!
      //! \returns A std::unordered_set consisting of all the nodes in the word
      //! graph \p wg that can reach \p target.
      //!
      //! \warning The arguments are not checked, and in particular it is
      //! assumed that \p target is a node of \p wg (i.e. less than
      //! WordGraphView::number_of_nodes).
      template <typename Node1, typename Node2>
      [[nodiscard]] std::unordered_set<Node1>
      ancestors_of_no_checks(WordGraphView<Node1> const& wg, Node2 target);

      //! \brief Returns the std::unordered_set of nodes that can reach a given
      //! node in a word graph.
      //!
      //! This function returns a std::unordered_set consisting of all the nodes
      //! in the word graph \p wg that can reach \p target. This function can be
      //! thought of like an inverse of `nodes_reachable_from`, in the sense
      //! that the node `a` \f$\in\f$ `ancestor_of(b)` for some node `b` if and
      //! only if `b` \f$\in\f$ `nodes_reachable_from(a)`.
      //!
      //! \tparam Node1 the node type of the word graph.
      //! \tparam Node2 the type of the node \p target.
      //!
      //! \param wg the word graph.
      //! \param target the target node.
      //!
      //! \returns A std::unordered_set consisting of all the nodes in the word
      //! graph \p wg that can reach \p target.
      //!
      //! \warning The arguments are not checked, and in particular it is
      //! assumed that \p target is a node of \p wg (i.e. less than
      //! WordGraph::number_of_nodes).
      template <typename Node1, typename Node2>
      [[nodiscard]] std::unordered_set<Node1>
      ancestors_of_no_checks(WordGraph<Node1> const& wg, Node2 target) {
        return ancestors_of_no_checks(WordGraphView<Node1>(wg), target);
      }

      //! \brief Returns the number of nodes reachable from a given node in a
      //! word graph view.
      //!
      //! This function returns the number of nodes in the word graph view \p wg
      //! that are reachable from \p source.
      //!
      //! \tparam Node1 the node type of the word graph.
      //! \tparam Node2 the type of the node \p source.
      //!
      //! \param wg the word graph.
      //! \param source the source node.
      //!
      //! \returns The number of nodes in the word graph \p wg that are
      //! reachable from \p source.
      //!
      //! \throws LibsemigroupsException if \p source is out of bounds (greater
      //! than or equal to WordGraphView::number_of_nodes).
      //!
      //! \note If any target of any edge in the word graph \p wg that is out of
      //! bounds, then this is ignored by this function.
      template <typename Node1, typename Node2>
      [[nodiscard]] size_t
      number_of_nodes_reachable_from(WordGraphView<Node1> const& wg,
                                     Node2                       source) {
        return nodes_reachable_from(wg, source).size();
      }

      //! \brief Returns the number of nodes reachable from a given node in a
      //! word graph.
      //!
      //! This function returns the number of nodes in the word graph \p wg that
      //! are reachable from \p source.
      //!
      //! \tparam Node1 the node type of the word graph.
      //! \tparam Node2 the type of the node \p source.
      //!
      //! \param wg the word graph.
      //! \param source the source node.
      //!
      //! \returns The number of nodes in the word graph \p wg that are
      //! reachable from \p source.
      //!
      //! \throws LibsemigroupsException if \p source is out of bounds (greater
      //! than or equal to WordGraph::number_of_nodes).
      //!
      //! \note If any target of any edge in the word graph \p wg that is out of
      //! bounds, then this is ignored by this function.
      template <typename Node1, typename Node2>
      [[nodiscard]] size_t
      number_of_nodes_reachable_from(WordGraph<Node1> const& wg, Node2 source) {
        return number_of_nodes_reachable_from(WordGraphView<Node1>(wg), source);
      }

      //! \brief Returns the number of nodes reachable from a given node in a
      //! word graph.
      //!
      //! \tparam Node1 the node type of the word graph.
      //! \tparam Node2 the type of the node \p source.
      //!
      //! \param wg the word graph.
      //! \param source the source node.
      //!
      //! \returns The number of nodes in the word graph \p wg that are
      //! reachable from \p source.
      //!
      //! \note If any target of any edge in the word graph \p wg that is out of
      //! bounds, then this is ignored by this function.
      //!
      //! \warning The arguments are not checked, and in particular it is
      //! assumed that \p source is a node of \p wg (i.e. less than
      //! WordGraphView::number_of_nodes).
      template <typename Node1, typename Node2>
      [[nodiscard]] size_t
      number_of_nodes_reachable_from_no_checks(WordGraphView<Node1> const& wg,
                                               Node2 source) {
        return nodes_reachable_from_no_checks(wg, source).size();
      }

      //! \brief Returns the number of nodes reachable from a given node in a
      //! word graph.
      //!
      //! \tparam Node1 the node type of the word graph.
      //! \tparam Node2 the type of the node \p source.
      //!
      //! \param wg the word graph.
      //! \param source the source node.
      //!
      //! \returns The number of nodes in the word graph \p wg that are
      //! reachable from \p source.
      //!
      //! \note If any target of any edge in the word graph \p wg that is out of
      //! bounds, then this is ignored by this function.
      //!
      //! \warning The arguments are not checked, and in particular it is
      //! assumed that \p source is a node of \p wg (i.e. less than
      //! WordGraph::number_of_nodes).
      template <typename Node1, typename Node2>
      [[nodiscard]] size_t
      number_of_nodes_reachable_from_no_checks(WordGraph<Node1> const& wg,
                                               Node2                   source) {
        return number_of_nodes_reachable_from_no_checks(
            WordGraphView<Node1>(wg), source);
      }

      //! \brief Construct a random connected acyclic word graph with given
      //! number of nodes, and out-degree.
      //!
      //! This function constructs a random acyclic connected word graph with
      //! \p number_of_nodes nodes, and out-degree \p out_degree. This function
      //! implements the Markov chain algorithm given in \cite Carnino2011.
      //!
      //! \param number_of_nodes the number of nodes.
      //! \param out_degree the out-degree of every node.
      //! \param mt a std::mt19937 used as a random source (defaults to:
      //! std::mt19937(std::random_device()())).
      //!
      //! \returns A random connected acyclic word graph.
      //!
      //! \throws LibsemigroupsException if any of the following hold:
      //! * \p number_of_nodes is less than \c 2
      //! * \p out_degree is less than \c 2
      //!
      //! \par Complexity
      //! The complexity of the implementation is \f$O(n^2)\f$ where \p n is the
      //! number of nodes.
      template <typename Node>
      WordGraph<Node> random_acyclic(size_t       number_of_nodes,
                                     size_t       out_degree,
                                     std::mt19937 mt
                                     = std::mt19937(std::random_device()()));

      //! \brief Replace the contents of a Forest by a spanning tree of the
      //! nodes reachable from a given node in a word graph.
      //!
      //! This function replaces the content of the Forest \p f with a spanning
      //! tree of the nodes reachable from \p root in the word graph \p wg.
      //!
      //! \tparam Node1 the node type of the word graph.
      //! \tparam Node2 the type of the node \p root.
      //!
      //! \param wg the word graph.
      //! \param root the source node.
      //! \param f the Forest object to hold the result.
      //!
      //! \note If any target of any edge in the word graph \p wg that is out of
      //! bounds, then this is ignored by this function.
      //!
      //! \warning The arguments are not checked, and in particular it is
      //! assumed that \p root is a node of \p wg (i.e. less than
      //! WordGraphView::number_of_nodes).
      template <typename Node1, typename Node2>
      void spanning_tree_no_checks(WordGraphView<Node1> const& wg,
                                   Node2                       root,
                                   Forest&                     f);

      //! \brief Replace the contents of a Forest by a spanning tree of the
      //! nodes reachable from a given node in a word graph.
      //!
      //! This function replaces the content of the Forest \p f with a spanning
      //! tree of the nodes reachable from \p root in the word graph \p wg.
      //!
      //! \tparam Node1 the node type of the word graph.
      //! \tparam Node2 the type of the node \p root.
      //!
      //! \param wg the word graph.
      //! \param root the source node.
      //! \param f the Forest object to hold the result.
      //!
      //! \note If any target of any edge in the word graph \p wg that is out of
      //! bounds, then this is ignored by this function.
      //!
      //! \warning The arguments are not checked, and in particular it is
      //! assumed that \p root is a node of \p wg (i.e. less than
      //! WordGraph::number_of_nodes).
      template <typename Node1, typename Node2>
      void spanning_tree_no_checks(WordGraph<Node1> const& wg,
                                   Node2                   root,
                                   Forest&                 f) {
        return spanning_tree_no_checks(WordGraphView<Node1>(wg), root, f);
      }

      //! \brief Replace the contents of a Forest by a spanning tree of the
      //! nodes reachable from a given node in a word graph.
      //!
      //! This function replaces the content of the Forest \p f with a spanning
      //! tree of the nodes reachable from \p root in the word graph \p wg.
      //!
      //! \tparam Node1 the node type of the word graph.
      //! \tparam Node2 the type of the node \p root.
      //!
      //! \param wg the word graph.
      //! \param root the source node.
      //! \param f the Forest object to hold the result.
      //!
      //! \throws LibsemigroupsException if \p root is out of bounds, i.e.
      //! greater than or equal to WordGraphView::number_of_nodes.
      //!
      //! \note If any target of any edge in the word graph \p wg that is out of
      //! bounds, then this is ignored by this function.
      template <typename Node1, typename Node2>
      void spanning_tree(WordGraphView<Node1> const& wg, Node2 root, Forest& f);

      //! \brief Replace the contents of a Forest by a spanning tree of the
      //! nodes reachable from a given node in a word graph.
      //!
      //! This function replaces the content of the Forest \p f with a spanning
      //! tree of the nodes reachable from \p root in the word graph \p wg.
      //!
      //! \tparam Node1 the node type of the word graph.
      //! \tparam Node2 the type of the node \p root.
      //!
      //! \param wg the word graph.
      //! \param root the source node.
      //! \param f the Forest object to hold the result.
      //!
      //! \throws LibsemigroupsException if \p root is out of bounds, i.e.
      //! greater than or equal to WordGraph::number_of_nodes.
      //!
      //! \note If any target of any edge in the word graph \p wg that is out of
      //! bounds, then this is ignored by this function.
      template <typename Node1, typename Node2>
      void spanning_tree(WordGraph<Node1> const& wg, Node2 root, Forest& f) {
        spanning_tree(WordGraphView<Node1>(wg), root, f);
      }

      //! \brief Returns a Forest containing a spanning tree of the nodes
      //! reachable from a given node in a word graph.
      //!
      //! This function returns a Forest containing a spanning
      //! tree of the nodes reachable from \p root in the word graph \p wg.
      //!
      //! \tparam Node1 the node type of the word graph.
      //! \tparam Node2 the type of the node \p root.
      //!
      //! \param wg the word graph.
      //! \param root the source node.
      //!
      //! \returns A Forest object containing a spanning tree.
      //!
      //! \note If any target of any edge in the word graph \p wg that is out of
      //! bounds, then this is ignored by this function.
      //!
      //! \warning The arguments are not checked, and in particular it is
      //! assumed that \p root is a node of \p wg (i.e. less than
      //! WordGraphView::number_of_nodes).
      template <typename Node1, typename Node2>
      [[nodiscard]] Forest
      spanning_tree_no_checks(WordGraphView<Node1> const& wg, Node2 root);

      //! \brief Returns a Forest containing a spanning tree of the nodes
      //! reachable from a given node in a word graph.
      //!
      //! This function returns a Forest containing a spanning
      //! tree of the nodes reachable from \p root in the word graph \p wg.
      //!
      //! \tparam Node1 the node type of the word graph.
      //! \tparam Node2 the type of the node \p root.
      //!
      //! \param wg the word graph.
      //! \param root the source node.
      //!
      //! \returns A Forest object containing a spanning tree.
      //!
      //! \note If any target of any edge in the word graph \p wg that is out of
      //! bounds, then this is ignored by this function.
      //!
      //! \warning The arguments are not checked, and in particular it is
      //! assumed that \p root is a node of \p wg (i.e. less than
      //! WordGraph::number_of_nodes).
      template <typename Node1, typename Node2>
      [[nodiscard]] Forest spanning_tree_no_checks(WordGraph<Node1> const& wg,
                                                   Node2 root) {
        return spanning_tree_no_checks(WordGraphView<Node1>(wg), root);
      }

      //! \brief Returns a Forest containing a spanning tree of the nodes
      //! reachable from a given node in a word graph.
      //!
      //! This function returns a Forest containing a spanning
      //! tree of the nodes reachable from \p root in the word graph \p wg.
      //!
      //! \tparam Node1 the node type of the word graph.
      //! \tparam Node2 the type of the node \p root.
      //!
      //! \param wg the word graph.
      //! \param root the source node.
      //!
      //! \returns A Forest object containing a spanning tree.
      //!
      //! \throws LibsemigroupsException if \p root is out of bounds, i.e.
      //! greater than or equal to WordGraphView::number_of_nodes.
      //!
      //! \note If any target of any edge in the word graph \p wg that is out of
      //! bounds, then this is ignored by this function.
      template <typename Node1, typename Node2>
      [[nodiscard]] Forest spanning_tree(WordGraphView<Node1> const& wg,
                                         Node2                       root);

      //! \brief Returns a Forest containing a spanning tree of the nodes
      //! reachable from a given node in a word graph.
      //!
      //! This function returns a Forest containing a spanning
      //! tree of the nodes reachable from \p root in the word graph \p wg.
      //!
      //! \tparam Node1 the node type of the word graph.
      //! \tparam Node2 the type of the node \p root.
      //!
      //! \param wg the word graph.
      //! \param root the source node.
      //!
      //! \returns A Forest object containing a spanning tree.
      //!
      //! \throws LibsemigroupsException if \p root is out of bounds, i.e.
      //! greater than or equal to WordGraph::number_of_nodes.
      //!
      //! \note If any target of any edge in the word graph \p wg that is out of
      //! bounds, then this is ignored by this function.
      template <typename Node1, typename Node2>
      [[nodiscard]] Forest spanning_tree(WordGraph<Node1> const& wg,
                                         Node2                   root) {
        return spanning_tree(WordGraphView<Node1>(wg), root);
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
      //! \note If any target of any edge in the word graph \p wg that is out of
      //! bounds, then this is ignored by this function.
      // Not nodiscard because sometimes we just don't want the output
      template <typename Graph>
      bool standardize(Graph& wg, Forest& f, Order val);

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
      //! \note If any target of any edge in the word graph \p wg that is out of
      //! bounds, then this is ignored by this function.
      // Not nodiscard because sometimes we just don't want the output
      template <typename Graph>
      std::pair<bool, Forest> standardize(Graph& wg,
                                          Order  val = Order::shortlex);

      //! \brief Check if a word graph is standardized.
      //!
      //! This function checks if the word graph \p wg is standardized according
      //! to the reduction order specified by \p val.
      //!
      //! \tparam Node the type of the node in \p wg.
      //!
      //! \param wg the word graph to check.
      //! \param val the order to use for standardization check (defaults to
      //! Order::shortlex).
      //!
      //! \throws LibsemigroupsException if \p val is not one of: Order::none,
      //! Order::shortlex, Order::lex or Order::recursive.
      //!
      //! \sa
      //! \ref standardize.
      template <typename Node>
      bool is_standardized(WordGraphView<Node> const& wg,
                           Order                      val = Order::shortlex);

      //! \brief Check if a word graph is standardized.
      //!
      //! This function checks if the word graph \p wg is standardized according
      //! to the reduction order specified by \p val.
      //!
      //! \tparam Node the type of the node in \p wg.
      //!
      //! \param wg the word graph to check.
      //! \param val the order to use for standardization check (defaults to
      //! Order::shortlex).
      //!
      //! \throws LibsemigroupsException if \p val is not one of: Order::none,
      //! Order::shortlex, Order::lex or Order::recursive.
      //!
      //! \sa
      //! \ref standardize.
      template <typename Node>
      bool is_standardized(WordGraph<Node> const& wg,
                           Order                  val = Order::shortlex) {
        return is_standardized(WordGraphView<Node>(wg), val);
      }

      //! \brief Returns the nodes of the word graph in topological order (see
      //! below) if possible.
      //!
      //! If it is not empty, the returned vector has the property that if an
      //! edge from a node \c n points to a node \c m, then \c m occurs before
      //! \c n in the vector.
      //!
      //! \tparam Node  the type of the nodes of the WordGraph.
      //!
      //! \param wg the word graph.
      //!
      //! \returns
      //! A std::vector of Node types that contains the nodes of
      //! \p wg in topological order (if possible) and is otherwise empty.
      //!
      //! \exceptions
      //! \no_libsemigroups_except
      //!
      //! \par Complexity
      //! \f$O(m + n)\f$ where \f$m\f$ is the number of nodes in the
      //! WordGraph \p wg and \f$n\f$ is the number of edges. Note that for
      //! WordGraph objects the number of edges is always at most \f$mk\f$
      //! where \f$k\f$ is the WordGraphView::out_degree.
      template <typename Node>
      [[nodiscard]] std::vector<Node>
      topological_sort(WordGraphView<Node> const& wg);

      //! \brief Returns the nodes of the word graph in topological order (see
      //! below) if possible.
      //!
      //! If it is not empty, the returned vector has the property that if an
      //! edge from a node \c n points to a node \c m, then \c m occurs before
      //! \c n in the vector.
      //!
      //! \tparam Node  the type of the nodes of the WordGraph.
      //!
      //! \param wg the word graph.
      //!
      //! \returns
      //! A std::vector of Node types that contains the nodes of
      //! \p wg in topological order (if possible) and is otherwise empty.
      //!
      //! \exceptions
      //! \no_libsemigroups_except
      //!
      //! \par Complexity
      //! \f$O(m + n)\f$ where \f$m\f$ is the number of nodes in the
      //! WordGraph \p wg and \f$n\f$ is the number of edges. Note that for
      //! WordGraph objects the number of edges is always at most \f$mk\f$
      //! where \f$k\f$ is the WordGraph::out_degree.
      template <typename Node>
      [[nodiscard]] std::vector<Node>
      topological_sort(WordGraph<Node> const& wg) {
        return topological_sort(WordGraphView<Node>(wg));
      }

      //! Returns the nodes of the word graph reachable from a given node in
      //! topological order (see below) if possible.
      //!
      //! If it is not empty, the returned vector has the property that
      //! if an edge from a node \c n points to a node \c m, then \c m occurs
      //! before \c n in the vector, and the last item in the vector is
      //! \p source.
      //!
      //! \tparam Node1 the node type of the word graph.
      //! \tparam Node2 the type of the node \p source.
      //!
      //! \param wg the WordGraph object to check.
      //! \param source the source node.
      //!
      //! \returns
      //! A std::vector of Node types that contains the nodes reachable from
      //! \p source in \p wg in topological order (if possible) and is otherwise
      //! empty.
      //!
      //! \exceptions
      //! \no_libsemigroups_except
      //!
      //! \par Complexity
      //! At worst \f$O(m + n)\f$ where \f$m\f$ is the number of nodes in the
      //! subword graph of those nodes reachable from \p source
      //! and \f$n\f$ is the number of edges.
      template <typename Node1, typename Node2>
      [[nodiscard]] std::vector<Node1>
      topological_sort(WordGraphView<Node1> const& wg, Node2 source);

      //! Returns the nodes of the word graph reachable from a given node in
      //! topological order (see below) if possible.
      //!
      //! If it is not empty, the returned vector has the property that
      //! if an edge from a node \c n points to a node \c m, then \c m occurs
      //! before \c n in the vector, and the last item in the vector is
      //! \p source.
      //!
      //! \tparam Node1 the node type of the word graph.
      //! \tparam Node2 the type of the node \p source.
      //!
      //! \param wg the WordGraph object to check.
      //! \param source the source node.
      //!
      //! \returns
      //! A std::vector of Node types that contains the nodes reachable from
      //! \p source in \p wg in topological order (if possible) and is otherwise
      //! empty.
      //!
      //! \exceptions
      //! \no_libsemigroups_except
      //!
      //! \par Complexity
      //! At worst \f$O(m + n)\f$ where \f$m\f$ is the number of nodes in the
      //! subword graph of those nodes reachable from \p source
      //! and \f$n\f$ is the number of edges.
      template <typename Node1, typename Node2>
      [[nodiscard]] std::vector<Node1>
      topological_sort(WordGraph<Node1> const& wg, Node2 source) {
        return topological_sort(WordGraphView<Node1>(wg), source);
      }

    }  // namespace word_graph

    //////////////////////////////////////////////////////////////////////////
    // WordGraph - non-member functions
    //////////////////////////////////////////////////////////////////////////
    // TODO(1) Add equivalents for WordGraphView

    //! \ingroup word_graph_group
    //! Output the edges of a wordGraph to a stream.
    //!
    //! This function outputs the word graph \p wg to the stream \p os.
    //! The word graph is represented by the out-neighbours of each node ordered
    //! according to their labels. The symbol `-` is used to denote that an
    //! edge is not defined. For example, the word graph with 1 node,
    //! out-degree 2, and a single loop labelled 1 from node 0 to 0 is
    //! represented as
    //! `{{-, 0}}`.
    //!
    //! \param os the ostream.
    //! \param wg the word graph.
    //!
    //! \returns
    //! The first parameter \p os.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    template <typename Node>
    std::ostream& operator<<(std::ostream& os, WordGraph<Node> const& wg);

    //! \defgroup make_word_graph_group make<WordGraph>
    //! \ingroup word_graph_group
    //!
    //! \brief Safely construct a \ref WordGraph instance.
    //!
    //! This page contains documentation related to safely constructing a
    //! \ref WordGraph instance.
    //!
    //! \sa \ref make_group for an overview of possible uses of the `make`
    //! function.

    //! \ingroup make_word_graph_group
    //!
    //! \brief Constructs a word graph from a number of nodes and targets.
    //!
    //! This function constructs a word graph from its arguments whose
    //! out-degree is specified by the length of the first item
    //! in the second parameter, or 0 if the second parameter is empty.
    //!
    //! \tparam Return the return type. Must satisfy
    //! \ref is_specialization_of_v<Return, WordGraph>.
    //!
    //! \param num_nodes the number of nodes in the word graph.
    //! \param targets the targets of the word graph.
    //!
    //! \returns A value of type WordGraph.
    //!
    //! \throws LibsemigroupsException
    //! if WordGraph<Node>::target throws when adding edges from \p targets.
    //!
    //! \complexity
    //! \f$O(mn)\f$ where \f$m\f$ is the length of \p targets and \f$n\f$ is the
    //! parameter \p num_nodes.
    //!
    //! \par Example
    //! \code
    //! // Construct a word graph with 5 nodes and 10 edges (7 specified)
    //! make<WordGraph<uint8_t>>(5, {{0, 0}, {1, 1}, {2}, {3, 3}});
    //! \endcode
    // Passing the 2nd parameter "targets" by value disambiguates it from the
    // other make<WordGraph>.
    template <typename Return>
    [[nodiscard]] std::enable_if_t<is_specialization_of_v<Return, WordGraph>,
                                   Return>
    make(
        size_t num_nodes,
        std::initializer_list<std::vector<typename Return::node_type>> targets);

    //! \ingroup make_word_graph_group
    //!
    // clang-format off
    // NOLINTNEXTLINE(whitespace/line_length)
    //! \copydoc make(size_t, std::initializer_list<std::vector<typename Return::node_type>>)
    // clang-format on
    template <typename Return>
    [[nodiscard]] std::enable_if_t<is_specialization_of_v<Return, WordGraph>,
                                   Return>
    make(size_t                                                      num_nodes,
         std::vector<std::vector<typename Return::node_type>> const& targets);

    namespace detail {
      template <typename Subclass>
      class JoinerMeeterCommon {
       private:
        template <typename Node1, typename Node2>
        void throw_if_bad_args(WordGraph<Node1> const& x,
                               Node2                   xroot,
                               WordGraph<Node1> const& y,
                               Node2                   yroot);

       public:
        template <typename Node>
        void call_no_checks(WordGraph<Node>&       xy,
                            WordGraph<Node> const& x,
                            Node                   xroot,
                            WordGraph<Node> const& y,
                            Node                   yroot);

        template <typename Node>
        void call_no_checks(WordGraph<Node>&       xy,
                            WordGraph<Node> const& x,
                            WordGraph<Node> const& y) {
          return call_no_checks(
              xy, x, static_cast<Node>(0), y, static_cast<Node>(0));
        }

        template <typename Node, typename... Args>
        [[nodiscard]] auto call_no_checks(WordGraph<Node> const& x,
                                          Args&&... args)
            -> std::enable_if_t<sizeof...(Args) % 2 == 1, WordGraph<Node>> {
          // The versions of this function changing the 1st argument in-place
          // always have an odd number of arguments, so we check that it's even
          // here (the argument x and an odd number of further arguments).
          WordGraph<Node> xy;
          static_cast<Subclass&>(*this).call_no_checks(
              xy, x, std::forward<Args>(args)...);
          return xy;
        }

        // There's no operator() with the number of nodes reachable from the
        // roots as arguments (7 args in total) because we'd have to check that
        // they were valid, and the only way to do this is to recompute them.

        template <typename Node>
        void operator()(WordGraph<Node>&       xy,
                        WordGraph<Node> const& x,
                        Node                   xroot,
                        WordGraph<Node> const& y,
                        Node                   yroot) {
          throw_if_bad_args(x, xroot, y, yroot);
          call_no_checks(xy, x, xroot, y, yroot);
        }

        template <typename Node>
        void operator()(WordGraph<Node>&       xy,
                        WordGraph<Node> const& x,
                        WordGraph<Node> const& y) {
          return operator()(
              xy, x, static_cast<Node>(0), y, static_cast<Node>(0));
        }

        template <typename Node, typename... Args>
        [[nodiscard]] auto operator()(WordGraph<Node> const& x, Args&&... args)
            -> std::enable_if_t<sizeof...(Args) % 2 == 1, WordGraph<Node>> {
          // The versions of this function changing the 1st argument in-place
          // always have an odd number of arguments, so we check that it's even
          // here (the argument x and an odd number of further arguments).
          WordGraph<Node> xy;
                          operator()(xy, x, std::forward<Args>(args)...);
          return xy;
        }

        template <typename Node1, typename Node2>
        bool is_subrelation_no_checks(WordGraph<Node1> const& x,
                                      Node2                   xroot,
                                      WordGraph<Node1> const& y,
                                      Node2                   yroot);

        template <typename Node>
        bool is_subrelation_no_checks(WordGraph<Node> const& x,
                                      WordGraph<Node> const& y) {
          return is_subrelation_no_checks(
              x, static_cast<Node>(0), y, static_cast<Node>(0));
        }

        // There's no subrelation with the number of nodes reachable from the
        // roots as arguments (6 args in total) because we'd have to check that
        // they were valid, and the only way to do this is to recompute them.

        template <typename Node1, typename Node2>
        bool is_subrelation(WordGraph<Node1> const& x,
                            Node2                   xroot,
                            WordGraph<Node1> const& y,
                            Node2                   yroot) {
          throw_if_bad_args(x, xroot, y, yroot);
          return is_subrelation_no_checks(x, xroot, y, yroot);
        }

        template <typename Node>
        bool is_subrelation(WordGraph<Node> const& x,
                            WordGraph<Node> const& y) {
          return is_subrelation(
              x, static_cast<Node>(0), y, static_cast<Node>(0));
        }
      };  // JoinerMeeterCommon
    }     // namespace detail

    //! \ingroup word_graph_group
    //! \brief Class for taking joins of word graphs.
    //!
    //! This class exists for its call operators which can be used to find the
    //! join of two word graphs with the same WordGraph::out_degree. This class
    //! implements the Hopcroft-Karp algorithm \cite Hop71 for computing a
    //! finite state automata recognising the union of the languages accepted by
    //! two given automata.
    //!
    //! The input word graphs need not be complete, and the root nodes can also
    //! be specified.
    // This class is intentionally not a template so that we don't have to
    // specify the types of the nodes when constructing one of these objects.
    // Instead every member function has a template parameter Node, which is
    // deduced from the argument.
    class Joiner : public detail::JoinerMeeterCommon<Joiner> {
     private:
      // TODO(v4) Remove the libsemigroups prefix
      libsemigroups::detail::Duf<>              _uf;
      std::stack<std::pair<uint64_t, uint64_t>> _stck;
      std::vector<uint64_t>                     _lookup;

      template <typename Node>
      [[nodiscard]] Node find(WordGraph<Node> const& x,
                              size_t xnum_nodes_reachable_from_root,
                              WordGraph<Node> const&               y,
                              uint64_t                             n,
                              typename WordGraph<Node>::label_type a) const;

      template <typename Node>
      void run(WordGraph<Node> const& x,
               size_t                 xnum_nodes_reachable_from_root,
               Node                   xroot,
               WordGraph<Node> const& y,
               size_t                 ynum_nodes_reachable_from_root,
               Node                   yroot);

     public:
      //! \brief Default constructor.
      //!
      //! Default constructor.
      Joiner();

      //! \brief Default copy constructor.
      //!
      //! Default copy constructor.
      Joiner(Joiner const&);

      //! \brief Default move constructor.
      //!
      //! Default move constructor.
      Joiner(Joiner&&);

      //! \brief Default copy assignment operator.
      //!
      //! Default copy assignment operator.
      Joiner& operator=(Joiner const&);

      //! \brief Default move assignment operator.
      //!
      //! Default move assignment operator.
      Joiner& operator=(Joiner&&);

      ~Joiner();

      //! \brief Replace the contents of a word graph with the join/meet of two
      //! given word graphs with respect to given root vertices.
      //!
      //! This function replaces the contents of the word graph \p xy with the
      //! join/meet of the word graphs \p x and \p y.
      //!
      //! \tparam Node the type of the nodes in the word graphs which are
      //! parameters to this function.
      //!
      //! \param xy the word graph to store the result.
      //!
      //! \param x the first word graph to join/meet.
      //!
      //! \param xnum_nodes_reachable_from_root the number of nodes reachable in
      //! \p x from the node \p xroot (for the circumstance where this number is
      //! known apriori, and does not have to be recomputed).
      //!
      //! \param xroot the node to use as a root in \p x.
      //!
      //! \param y the second word graph to join/meet.
      //!
      //! \param ynum_nodes_reachable_from_root the number of nodes reachable in
      //! \p y from the node \p yroot.
      //!
      //! \param yroot the node to use as a root in \p y.
      //!
      //! \warning
      //! No checks whatsoever on the validity of the arguments are performed.
      template <typename Node>
      void call_no_checks(WordGraph<Node>&       xy,
                          WordGraph<Node> const& x,
                          size_t                 xnum_nodes_reachable_from_root,
                          Node                   xroot,
                          WordGraph<Node> const& y,
                          size_t                 ynum_nodes_reachable_from_root,
                          Node                   yroot);

      //! \brief Check if the language accepted by one word graph is contained
      //! in that accepted by another word graph.
      //!
      //! This function returns \c true if the language accepted by \p x with
      //! initial node \p xroot and accept state every node, is a subset of the
      //! corresponding language in \p y.
      //!
      //! \tparam Node1 the type of the nodes in the word graphs which are
      //! parameters to this function.
      //!
      //! \tparam Node2 the type of the nodes to use as roots.
      //!
      //! \param x the word graph whose language we are checking might be a
      //! subset.
      //!
      //! \param xnum_nodes_reachable_from_root the number of nodes reachable in
      //! \p x from the node \p xroot (for the circumstance where this number is
      //! known apriori, and does not have to be recomputed).
      //!
      //! \param xroot the node to use as the initial state in \p x.
      //!
      //! \param y the word graph whose language we are checking might be a
      //! superset.
      //!
      //! \param ynum_nodes_reachable_from_root the number of nodes reachable in
      //! \p y from the node \p yroot.
      //!
      //! \param yroot the node to use as an initial state in \p y.
      //!
      //! \returns Whether or not \p x is a subrelation of \p y.
      //!
      //! \warning
      //! No checks whatsoever on the validity of the arguments are performed.
      // Is x a subrelation of y?
      template <typename Node1, typename Node2>
      [[nodiscard]] bool
      is_subrelation_no_checks(WordGraph<Node1> const& x,
                               size_t xnum_nodes_reachable_from_root,
                               Node2  xroot,
                               WordGraph<Node1> const& y,
                               size_t ynum_nodes_reachable_from_root,
                               Node2  yroot);
#ifdef LIBSEMIGROUPS_PARSED_BY_DOXYGEN
      //! \brief Replace the contents of a word graph with the join/meet of two
      //! given word graphs with respect to given root vertices.
      //!
      //! This function replaces the contents of the word graph \p xy with the
      //! join/meet of the word graphs \p x and \p y. This function is the same
      //! as the 7-argument variant but it computes the number of nodes
      //! reachable from \p xroot and \p yroot.
      //!
      //! \tparam Node the type of the nodes in the word graphs which are
      //! parameters to this function.
      //!
      //! \param xy the word graph to store the result.
      //!
      //! \param x the first word graph to join/meet.
      //!
      //! \param xroot the node to use as a root in \p x.
      //!
      //! \param y the second word graph to join/meet.
      //!
      //! \param yroot the node to use as a root in \p y.
      //!
      //! \warning
      //! No checks whatsoever on the validity of the arguments are performed.
      template <typename Node>
      void call_no_checks(WordGraph<Node>&       xy,
                          WordGraph<Node> const& x,
                          Node                   xroot,
                          WordGraph<Node> const& y,
                          Node                   yroot);

      //! \brief Replace the contents of a word graph with the join/meet of two
      //! given word graphs with respect to given root vertices.
      //!
      //! This function replaces the contents of the word graph \p xy with the
      //! join/meet of the word graphs \p x and \p y. This function is the same
      //! as the 5-argument variant but it uses \c 0 as the root node in both
      //! \p x and \p y.
      //!
      //! \tparam Node the type of the nodes in the word graphs which are
      //! parameters to this function.
      //!
      //! \param xy the word graph to store the result.
      //!
      //! \param x the first word graph to join/meet.
      //!
      //! \param y the second word graph to join/meet.
      //!
      //! \warning
      //! No checks whatsoever on the validity of the arguments are performed.
      template <typename Node>
      void call_no_checks(WordGraph<Node>&       xy,
                          WordGraph<Node> const& x,
                          WordGraph<Node> const& y);

      //! \brief Returns a word graph containing the join/meet of two given word
      //! graphs.
      //!
      //! This function returns a word graph  containing the join/meet of the
      //! word graphs \p x and \p y. If \c n is the number of arguments, then
      //! this function constructs a word graph to contain the result, forwards
      //! this and the other arguments to the overload of `call_no_checks` with
      //! `n + 1` parameters, then returns the word graph containing the result.
      //!
      //! \tparam Node the type of the nodes in the word graphs which are
      //! parameters to this function.
      //!
      //! \tparam Args parameter pack for the remaining arguments.
      //!
      //! \param x the first word graph to join/meet.
      //! \param args the remaining arguments.
      //!
      //! \warning
      //! No checks whatsoever on the validity of the arguments are performed.
      template <typename Node, typename... Args>
      [[nodiscard]] auto call_no_checks(WordGraph<Node> const& x,
                                        Args&&... args);

      //! \brief Replace the contents of a word graph with the join/meet of two
      //! given word graphs with respect to given root vertices.
      //!
      //! This function replaces the contents of the word graph \p xy with the
      //! join/meet of the word graphs \p x and \p y. This function is the same
      //! as the 5-argument overload of \c call_no_checks but it throws if its
      //! arguments aren't valid.
      //!
      //! \tparam Node the type of the nodes in the word graphs which are
      //! parameters to this function.
      //!
      //! \param xy the word graph to store the result.
      //!
      //! \param x the first word graph to join/meet.
      //!
      //! \param xroot the node to use as a root in \p x.
      //!
      //! \param y the second word graph to join/meet.
      //!
      //! \param yroot the node to use as a root in \p y.
      //!
      //! \throws LibsemigroupsException if any of the following hold:
      //! * \p xroot isn't a node in \p x;
      //! * \p yroot isn't a node in \p y;
      //! * `x.out_degree() != y.out_degree()`.
      template <typename Node>
      void operator()(WordGraph<Node>&       xy,
                      WordGraph<Node> const& x,
                      Node                   xroot,
                      WordGraph<Node> const& y,
                      Node                   yroot);

      //! \brief Replace the contents of a word graph with the join/meet of two
      //! given word graphs with respect to given root vertices.
      //!
      //! This function replaces the contents of the word graph \p xy with the
      //! join/meet of the word graphs \p x and \p y. This function is the same
      //! as the 3-argument overload of \c call_no_checks but it throws if its
      //! arguments aren't valid.
      //!
      //! \tparam Node the type of the nodes in the word graphs which are
      //! parameters to this function.
      //!
      //! \param xy the word graph to store the result.
      //!
      //! \param x the first word graph to join/meet.
      //!
      //! \param y the second word graph to join/meet.
      //!
      //! \throws LibsemigroupsException if any of the following hold:
      //! * \p x has no nodes;
      //! * \p y has no nodes;
      //! * `x.out_degree() != y.out_degree()`.
      template <typename Node>
      void operator()(WordGraph<Node>&       xy,
                      WordGraph<Node> const& x,
                      WordGraph<Node> const& y);

      //! \brief Returns a word graph containing the join/meet of two given word
      //! graphs.
      //!
      //! This function is the same as the overload of \ref call_no_checks with
      //! the same signature, the difference being that this function throws if
      //! the arguments are invalid.
      //!
      //! \tparam Node the type of the nodes in the word graphs which are
      //! parameters to this function.
      //!
      //! \tparam Args parameter pack for the remaining arguments.
      //!
      //! \param x the first word graph to join/meet.
      //! \param args the remaining arguments.
      //!
      //! \throws LibsemigroupsException if the arguments aren't valid. See the
      //! relevant `operator()` for more details.
      template <typename Node, typename... Args>
      [[nodiscard]] auto operator()(WordGraph<Node> const& x, Args&&... args);

      //! \brief Check if the language accepted by one word graph is contained
      //! in that defined by another word graph.
      //!
      //! This function returns \c true if the language accepted by \p x with
      //! initial node \p xroot and accept state every node, is a subset of the
      //! corresponding language in \p y. This version of the function is
      //! similar to the 6-argument overload, except that here we must compute
      //! the number of nodes in \p x and \p y reachable from \p xroot and
      //! \p yroot, respectively.
      //!
      //! \tparam Node1 the type of the nodes in the word graphs which are
      //! parameters to this function.
      //!
      //! \tparam Node2 the type of the nodes to use as roots.
      //!
      //! \param x the word graph whose language we are checking might be a
      //! subset.
      //!
      //! \param xroot the node to use as the initial state in \p x.
      //!
      //! \param y the word graph whose language we are checking might be a
      //! superset.
      //!
      //! \param yroot the node to use as an initial state in \p y.
      //!
      //! \returns Whether or not \p x is a subrelation of \p y.
      //!
      //! \warning
      //! No checks whatsoever on the validity of the arguments are performed.
      // Is x a subrelation of y?
      template <typename Node1, typename Node2>
      bool is_subrelation_no_checks(WordGraph<Node1> const& x,
                                    Node2                   xroot,
                                    WordGraph<Node1> const& y,
                                    Node2                   yroot);

      //! \brief Check if the language accepted by one word graph is contained
      //! in that defined by another word graph.
      //!
      //! This function returns \c true if the language accepted by \p x with
      //! initial node \p xroot and accept state every node, is a subset of the
      //! corresponding language in \p y. This version of the function is
      //! similar to the 4-argument overload, except that \c 0 is used as the
      //! root node in both \p x and \p y.
      //!
      //! \tparam Node the type of the nodes in the word graphs which are
      //! parameters to this function.
      //!
      //! \param x the word graph whose language we are checking might be a
      //! subset.
      //!
      //! \param y the word graph whose language we are checking might be a
      //! superset.
      //!
      //! \returns Whether or not \p x is a subrelation of \p y.
      //!
      //! \warning
      //! No checks whatsoever on the validity of the arguments are performed.
      template <typename Node>
      bool is_subrelation_no_checks(WordGraph<Node> const& x,
                                    WordGraph<Node> const& y);

      // There's no subrelation with the number of nodes reachable from the
      // roots as arguments (6 args in total) because we'd have to check that
      // they were valid, and the only way to do this is to recompute them.

      //! \brief Check if the language accepted by one word graph is contained
      //! in that defined by another word graph.
      //!
      //! This function returns \c true if the language accepted by \p x with
      //! initial node \p xroot and accept state every node, is a subset of the
      //! corresponding language in \p y. This version of the function is the
      //! same as the 4-argument overload of \c is_subrelation_no_checks, except
      //! that this function throws if its arguments are invalid.
      //!
      //! \tparam Node the type of the nodes in the word graphs which are
      //! parameters to this function.
      //!
      //! \param x the word graph whose language we are checking might be a
      //! subset.
      //!
      //! \param xroot the node to use as the initial state in \p x.
      //!
      //! \param y the word graph whose language we are checking might be a
      //! superset.
      //!
      //! \param yroot the node to use as an initial state in \p y.
      //!
      //! \returns Whether or not \p x is a subrelation of \p y.
      //!
      //! \throws LibsemigroupsException if any of the following hold:
      //! * \p xroot isn't a node in \p x;
      //! * \p yroot isn't a node in \p y;
      //! * `x.out_degree() != y.out_degree()`.
      template <typename Node1, typename Node2>
      bool is_subrelation(WordGraph<Node1> const& x,
                          Node2                   xroot,
                          WordGraph<Node1> const& y,
                          Node2                   yroot);

      //! \brief Check if the language accepted by one word graph is contained
      //! in that defined by another word graph.
      //!
      //! This function returns \c true if the language accepted by \p x with
      //! initial node \c 0 and accept state every node, is a subset of the
      //! corresponding language in \p y. This version of the function is the
      //! same as the 2-argument overload of \c is_subrelation_no_checks, except
      //! that this function throws if its arguments are invalid.
      //!
      //! \tparam Node the type of the nodes in the word graphs which are
      //! parameters to this function.
      //!
      //! \param x the word graph whose language we are checking might be a
      //! subset.
      //!
      //! \param y the word graph whose language we are checking might be a
      //! superset.
      //!
      //! \returns Whether or not \p x is a subrelation of \p y.
      //!
      //! \throws LibsemigroupsException if any of the following hold:
      //! * \p x has no nodes;
      //! * \p y has no nodes;
      //! * `x.out_degree() != y.out_degree()`.
      template <typename Node>
      bool is_subrelation(WordGraph<Node> const& x, WordGraph<Node> const& y);

#else
      using detail::JoinerMeeterCommon<Joiner>::call_no_checks;
      using detail::JoinerMeeterCommon<Joiner>::operator();
      using detail::JoinerMeeterCommon<Joiner>::is_subrelation_no_checks;
      using detail::JoinerMeeterCommon<Joiner>::is_subrelation;
#endif
    };  // Joiner

    //! \ingroup word_graph_group
    //!
    //! \brief Class for taking meets of word graphs.
    //!
    //! This class exists for its call operators which can be used to find the
    //! meet of two word graphs with the same WordGraph::out_degree. This class
    //! implements the same algorithm as that used for computing a
    //! finite state automata recognising the intersection of the languages
    //! accepted by two given automata.
    //!
    //! The input word graphs need not be complete, and the root nodes can also
    //! be specified.
    // Class for forming the meet of two word graphs
    class Meeter : public detail::JoinerMeeterCommon<Meeter> {
     private:
      using node_type = std::pair<uint64_t, uint64_t>;

      std::unordered_map<node_type, uint64_t, Hash<node_type>> _lookup;
      std::vector<node_type>                                   _todo;
      std::vector<node_type>                                   _todo_new;

     public:
      //! \brief Default constructor.
      //!
      //! Default constructor.
      Meeter();

      //! \brief Default copy constructor.
      //!
      //! Default copy constructor.
      Meeter(Meeter const&);

      //! \brief Default move constructor.
      //!
      //! Default move constructor.
      Meeter(Meeter&&);

      //! \brief Default copy assignment operator.
      //!
      //! Default copy assignment operator.
      Meeter& operator=(Meeter const&);

      //! \brief Default move assignment operator.
      //!
      //! Default move assignment operator.
      Meeter& operator=(Meeter&&);

      ~Meeter();

      //! \copydoc Joiner::call_no_checks(WordGraph<Node>&, WordGraph<Node>
      //! const&, size_t, Node, WordGraph<Node> const&, size_t, Node)
      template <typename Node>
      void call_no_checks(WordGraph<Node>&       xy,
                          WordGraph<Node> const& x,
                          size_t                 xnum_nodes_reachable_from_root,
                          Node                   xroot,
                          WordGraph<Node> const& y,
                          size_t                 ynum_nodes_reachable_from_root,
                          Node                   yroot);

      //! \copydoc Joiner::is_subrelation_no_checks(WordGraph<Node1> const&,
      //! size_t, Node2, WordGraph<Node1> const&, size_t, Node2)
      // is x a subrelation of y
      template <typename Node1, typename Node2>
      [[nodiscard]] bool
      is_subrelation_no_checks(WordGraph<Node1> const& x,
                               size_t xnum_nodes_reachable_from_root,
                               Node2  xroot,
                               WordGraph<Node1> const& y,
                               size_t ynum_nodes_reachable_from_root,
                               Node2  yroot);

#ifdef LIBSEMIGROUPS_PARSED_BY_DOXYGEN
      //! \copydoc Joiner::call_no_checks(WordGraph<Node>&, WordGraph<Node>
      //! const&, Node, WordGraph<Node> const&, Node)
      template <typename Node>
      void call_no_checks(WordGraph<Node>&       xy,
                          WordGraph<Node> const& x,
                          Node                   xroot,
                          WordGraph<Node> const& y,
                          Node                   yroot);

      //! \copydoc Joiner::call_no_checks(WordGraph<Node>&, WordGraph<Node>
      //! const&, WordGraph<Node> const&)
      template <typename Node>
      void call_no_checks(WordGraph<Node>&       xy,
                          WordGraph<Node> const& x,
                          WordGraph<Node> const& y);

      //! \brief Returns a word graph containing the join/meet of two given word
      //! graphs.
      //!
      //! This function returns a word graph containing the join/meet of the
      //! word graphs \p x and \p y. If \c n is the number of arguments, then
      //! this function constructs a word graph to contain the result, forwards
      //! this and the other arguments to the overload of `call_no_checks` with
      //! `n + 1` parameters, then returns the word graph containing the result.
      //!
      //! \tparam Node the type of the nodes in the word graphs which are
      //! parameters to this function.
      //!
      //! \tparam Args parameter pack for the remaining arguments.
      //!
      //! \param x the first word graph to join/meet.
      //! \param args the remaining parameters.
      //!
      //! \warning
      //! No checks whatsoever on the validity of the arguments are performed.
      template <typename Node, typename... Args>
      [[nodiscard]] auto call_no_checks(WordGraph<Node> const& x,
                                        Args&&... args);

      //! \copydoc Joiner::operator()(WordGraph<Node>&, WordGraph<Node>
      //! const&, Node, WordGraph<Node> const&, Node)
      template <typename Node>
      void operator()(WordGraph<Node>&       xy,
                      WordGraph<Node> const& x,
                      Node                   xroot,
                      WordGraph<Node> const& y,
                      Node                   yroot);

      //! \copydoc Joiner::operator()(WordGraph<Node>&, WordGraph<Node>
      //! const&, WordGraph<Node> const&)
      template <typename Node>
      void operator()(WordGraph<Node>&       xy,
                      WordGraph<Node> const& x,
                      WordGraph<Node> const& y);

      //! \brief Returns a word graph containing the join/meet of two given word
      //! graphs.
      //!
      //! This function is the same as the overload of \ref call_no_checks with
      //! the same signature, the difference being that this function throws if
      //! the arguments are invalid.
      //!
      //! \tparam Node the type of the nodes in the word graphs which are
      //! parameters to this function.
      //!
      //! \tparam Args parameter pack for the remaining arguments.
      //!
      //! \param x the first word graph to join/meet.
      //! \param args the remaining arguments.
      //!
      //! \throws LibsemigroupsException if the arguments aren't valid. See the
      //! relevant `operator()` for more details.
      template <typename Node, typename... Args>
      [[nodiscard]] auto operator()(WordGraph<Node> const& x, Args&&... args);

      //! \copydoc Joiner::is_subrelation_no_checks(WordGraph<Node1> const&,
      //! Node2, WordGraph<Node1> const&, Node2)
      template <typename Node1, typename Node2>
      bool is_subrelation_no_checks(WordGraph<Node1> const& x,
                                    Node2                   xroot,
                                    WordGraph<Node1> const& y,
                                    Node2                   yroot);

      //! \copydoc Joiner::is_subrelation_no_checks(WordGraph<Node> const&,
      //! WordGraph<Node> const&)
      template <typename Node>
      bool is_subrelation_no_checks(WordGraph<Node> const& x,
                                    WordGraph<Node> const& y);

      //! \copydoc Joiner::is_subrelation(WordGraph<Node1> const&,
      //! Node2, WordGraph<Node1> const&, Node2)
      template <typename Node1, typename Node2>
      bool is_subrelation(WordGraph<Node1> const& x,
                          Node2                   xroot,
                          WordGraph<Node1> const& y,
                          Node2                   yroot);

      //! \copydoc Joiner::is_subrelation(WordGraph<Node> const&,
      //! WordGraph<Node> const&)
      template <typename Node>
      bool is_subrelation(WordGraph<Node> const& x, WordGraph<Node> const& y);
#else
      using detail::JoinerMeeterCommon<Meeter>::call_no_checks;
      using detail::JoinerMeeterCommon<Meeter>::operator();
      using detail::JoinerMeeterCommon<Meeter>::is_subrelation_no_checks;
      using detail::JoinerMeeterCommon<Meeter>::is_subrelation;
#endif
    };  // class Meeter

    //! \ingroup word_graph_group
    //!
    //! \brief Return a human readable representation of a WordGraph object.
    //!
    //! Return a human readable representation of a WordGraph object.
    //!
    //! \tparam Node the type of the nodes in the underlying WordGraph.
    //!
    //! \param wg the WordGraph object.
    //!
    //! \returns A string containing the representation.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    template <typename Node>
    [[nodiscard]] std::string to_human_readable_repr(WordGraph<Node> const& wg);

    //! \ingroup word_graph_group
    //!
    //! \brief Return a human readable representation of a Meeter object.
    //!
    //! Return a human readable representation of a Meeter object.
    //!
    //! \param meet the Meeter object.
    //!
    //! \returns A string containing the representation.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    [[nodiscard]] static inline std::string
    to_human_readable_repr(Meeter const& meet) {
      (void) meet;
      return "<Meeter of word graphs>";
    }

    //! \ingroup word_graph_group
    //!
    //! \brief Return a human readable representation of a Joiner object.
    //!
    //! Return a human readable representation of a Joiner object.
    //!
    //! \param join the Joiner object.
    //!
    //! \returns A string containing the representation.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    [[nodiscard]] static inline std::string
    to_human_readable_repr(Joiner const& join) {
      (void) join;
      return "<Joiner of word graphs>";
    }

    //! \ingroup word_graph_group
    //!
    //! \brief Return a string that can be used to recreate a word graph.
    //!
    //! This function returns a std::string containing the input required to
    //! construct a copy of the argument \p wg.
    //!
    //! \tparam Node the type of the nodes of \p wg.
    //! \param wg the word graph.
    //! \param prefix a prefix for the returned string (defaults to an empty
    //! string).
    //! \param braces the braces to use in the string (defaults to `"{}"`).
    //! \param suffix a suffix for the returned string (defaults to an empty
    //! string).
    //!
    //! \returns A string containing the input required to recreate \p wg.
    //!
    //! \throws LibsemigroupsException if the argument \p braces is not of
    //! length
    //! \c 2.
    template <typename Node>
    [[nodiscard]] std::string to_input_string(WordGraph<Node> const& wg,
                                              std::string const& prefix = "",
                                              std::string const& braces = "{}",
                                              std::string const& suffix = "");

  }  // namespace v4
}  // namespace libsemigroups

#include "libsemigroups/word-graph-helpers.tpp"
#endif  // LIBSEMIGROUPS_WORD_GRAPH_HELPERS_HPP_
