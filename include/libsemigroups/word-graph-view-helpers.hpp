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

// This file contains helper functions for word graph views

#ifndef LIBSEMIGROUPS_WORD_GRAPH_VIEW_HELPERS_HPP_
#define LIBSEMIGROUPS_WORD_GRAPH_VIEW_HELPERS_HPP_

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

#include "libsemigroups/adapters.hpp"   // for Hash
#include "libsemigroups/config.hpp"     // for LIBSEMIGROUPS_EIGEN_...
#include "libsemigroups/constants.hpp"  // for UNDEFINED
#include "libsemigroups/debug.hpp"      // for LIBSEMIGROUPS_ASSERT
#include "libsemigroups/dot.hpp"        // for Dot
#include "libsemigroups/exception.hpp"  // for LIBSEMIGROUPS_EXCEPTION
#include "libsemigroups/forest.hpp"     // for Forest
#include "libsemigroups/is_specialization_of.hpp"  // for is_specialization_of
#include "libsemigroups/order.hpp"                 // for Order
#include "libsemigroups/types.hpp"                 // for word_type, letter_type
#include "libsemigroups/word-graph-class.hpp"      // for WordGraph
#include "libsemigroups/word-graph-view-class.hpp"  // for WordGraphView

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

  namespace word_graph {

    //////////////////////////////////////////////////////////////////////////
    // WordGraphView - helper functions - in alphabetical order!!!
    //////////////////////////////////////////////////////////////////////////

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

    //! \brief Returns a labelled \ref Dot object representing a word graph
    //! view.
    //!
    //! This function returns a \ref Dot object representing the word graph
    //! view \p wg. The nodes of the returned graph are labelled using
    //! \p node_labels, and the colours used for edges are shown in a legend
    //! labelled by \p edge_labels.
    //!
    //! \tparam Node the type of the nodes of the WordGraphView.
    //!
    //! \param wg the word graph view.
    //! \param node_labels the labels for the nodes of \p wg.
    //! \param edge_labels the labels for the edge labels of \p wg.
    //!
    //! \returns A \ref Dot object representing \p wg.
    //!
    //! \throws LibsemigroupsException if \p node_labels has size different
    //! from `wg.number_of_nodes()`.
    //! \throws LibsemigroupsException if \p edge_labels has size different
    //! from `wg.out_degree()`.
    //! \throws LibsemigroupsException if the out-degree of \p wg is greater
    //! than the number of colours in Dot::colors.
    //!
    //! \note This function does not trigger an enumeration.
    template <typename Node>
    [[nodiscard]] Dot dot(WordGraphView<Node> const&      wg,
                          std::vector<std::string> const& node_labels,
                          std::vector<std::string> const& edge_labels);

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
                                              Node2                       from,
                                              Iterator                    first,
                                              Iterator last) noexcept;

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
    [[nodiscard]] Node1 follow_path_no_checks(WordGraphView<Node1> const& wg,
                                              Node2                       from,
                                              word_type const& path) noexcept {
      static_assert(sizeof(Node2) <= sizeof(Node1));
      return follow_path_no_checks(wg, from, path.cbegin(), path.cend());
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
    [[nodiscard]] bool is_acyclic(WordGraphView<Node1> const& wg, Node2 source);

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

    //! \brief Returns the std::unordered_set of nodes reachable from a given
    //! node in a word graph.
    //!
    //! This function returns a std::unordered_set consisting of all the nodes
    //! in the word graph \p wg that are reachable from \p source
    //! via a path of length at most \p max_depth.
    //!
    //! \tparam Node1 the node type of the word graph.
    //! \tparam Node2 the type of the node \p source.
    //!
    //! \param wg the word graph.
    //! \param source the source node.
    //! \param max_depth the maximum distance from source
    //! (defaults to \ref POSITIVE_INFINITY).
    //!
    //! \returns A std::unordered_set consisting of all the nodes in the word
    //! graph \p wg that are reachable from \p source.
    //!
    //! \note If any target of any edge in the word graph \p wg is out of
    //! bounds, then this is ignored by this function.
    //!
    //! \warning The arguments are not checked, and in particular it is
    //! assumed that \p source is a node of \p wg (i.e. less than
    //! WordGraph::number_of_nodes).
    template <typename Node1, typename Node2>
    [[nodiscard]] std::unordered_set<Node1>
    nodes_reachable_from_no_checks(WordGraphView<Node1> const& wg,
                                   Node2                       source,
                                   size_t max_depth = POSITIVE_INFINITY);

    //! \brief Returns the std::unordered_set of nodes reachable from a given
    //! node in a word graph view.
    //!
    //! This function returns a std::unordered_set consisting of all the nodes
    //! in the word graph view \p wg that are reachable from \p source
    //! via a path of length at most \p max_depth.
    //!
    //! \tparam Node1 the node type of the word graph view.
    //! \tparam Node2 the type of the node \p source.
    //!
    //! \param wg the word graph view.
    //! \param source the source node.
    //! \param max_depth the maximum distance from source
    //! (defaults to \ref POSITIVE_INFINITY).
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
    nodes_reachable_from(WordGraphView<Node1> const& wg,
                         Node2                       source,
                         size_t                      max_depth) {
      static_assert(sizeof(Node2) <= sizeof(Node1));
      wg.throw_if_node_out_of_bounds(source);
      return nodes_reachable_from_no_checks(wg, source, max_depth);
    }

    //! \brief Returns the number of nodes reachable from a given node in a
    //! word graph view.
    //!
    //! This function returns the number of nodes in the word graph view \p wg
    //! that are reachable from \p source via a path of length at most
    //! \p max_depth.
    //!
    //! \tparam Node1 the node type of the word graph.
    //! \tparam Node2 the type of the node \p source.
    //!
    //! \param wg the word graph.
    //! \param source the source node.
    //! \param max_depth the maximum distance from source
    //! (defaults to \ref POSITIVE_INFINITY).
    //!
    //! \returns The number of nodes in the word graph \p wg that are
    //! reachable from \p source.
    //!
    //! \throws LibsemigroupsException if \p source is out of bounds (greater
    //! than or equal to WordGraphView::number_of_nodes).
    //!
    //! \note If any target of any edge in the word graph \p wg is out of
    //! bounds, then this is ignored by this function.
    template <typename Node1, typename Node2>
    [[nodiscard]] size_t
    number_of_nodes_reachable_from(WordGraphView<Node1> const& wg,
                                   Node2                       source,
                                   size_t max_depth = POSITIVE_INFINITY) {
      return nodes_reachable_from(wg, source, max_depth).size();
    }

    //! \brief Returns the number of nodes reachable from a given node in a
    //! word graph.
    //!
    //! This function returns the number of nodes in the word graph \p wg that
    //! are reachable from \p source via a path of length at most
    //! \p max_depth.
    //!
    //! \tparam Node1 the node type of the word graph.
    //! \tparam Node2 the type of the node \p source.
    //!
    //! \param wg the word graph.
    //! \param source the source node.
    //! \param max_depth the maximum distance from source
    //! (defaults to \ref POSITIVE_INFINITY).
    //!
    //! \returns The number of nodes in the word graph \p wg that are
    //! reachable from \p source.
    //!
    //! \note If any target of any edge in the word graph \p wg is out of
    //! bounds, then this is ignored by this function.
    //!
    //! \warning The arguments are not checked, and in particular it is
    //! assumed that \p source is a node of \p wg (i.e. less than
    //! WordGraphView::number_of_nodes).
    template <typename Node1, typename Node2>
    [[nodiscard]] size_t number_of_nodes_reachable_from_no_checks(
        WordGraphView<Node1> const& wg,
        Node2                       source,
        size_t                      max_depth = POSITIVE_INFINITY) {
      return nodes_reachable_from_no_checks(wg, source, max_depth).size();
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
    //! \warning The arguments are not checked, and in particular it is
    //! assumed that \p target is a node of \p wg (i.e. less than
    //! WordGraphView::number_of_nodes).
    template <typename Node1, typename Node2>
    [[nodiscard]] std::unordered_set<Node1>
    ancestors_of_no_checks(WordGraphView<Node1> const& wg, Node2 target);

    //! \brief Replace the contents of a Forest by a spanning tree of the
    //! nodes reachable from a given node in a word graph.
    //!
    //! This function replaces the content of the Forest \p f with a spanning
    //! tree of the nodes reachable from \p root in the word graph \p wg
    //! by a path of length at most \p max_depth.
    //!
    //! \tparam Node1 the node type of the word graph.
    //! \tparam Node2 the type of the node \p root.
    //!
    //! \param wg the word graph.
    //! \param root the source node.
    //! \param f the Forest object to hold the result.
    //! \param max_depth the maximum depth of the tree (defaults to
    //! \ref POSITIVE_INFINITY).
    //!
    //! \note If any target of any edge in the word graph \p wg is out of
    //! bounds, then this is ignored by this function.
    //!
    //! \warning The arguments are not checked, and in particular it is
    //! assumed that \p root is a node of \p wg (i.e. less than
    //! WordGraphView::number_of_nodes).
    template <typename Node1, typename Node2>
    void spanning_tree_no_checks(WordGraphView<Node1> const& wg,
                                 Node2                       root,
                                 Forest&                     f,
                                 size_t max_depth = POSITIVE_INFINITY);

    //! \brief Replace the contents of a Forest by a spanning tree of the
    //! nodes reachable from a given node in a word graph.
    //!
    //! This function replaces the content of the Forest \p f with a spanning
    //! tree of the nodes reachable from \p root in the word graph \p wg
    //! by a path of length at most \p max_depth.
    //!
    //! \tparam Node1 the node type of the word graph.
    //! \tparam Node2 the type of the node \p root.
    //!
    //! \param wg the word graph.
    //! \param root the source node.
    //! \param f the Forest object to hold the result.
    //! \param max_depth the maximum depth of the tree (defaults to
    //! \ref POSITIVE_INFINITY).
    //!
    //! \throws LibsemigroupsException if \p root is out of bounds, i.e.
    //! greater than or equal to WordGraphView::number_of_nodes.
    //!
    //! \note If any target of any edge in the word graph \p wg is out of
    //! bounds, then this is ignored by this function.
    template <typename Node1, typename Node2>
    void spanning_tree(WordGraphView<Node1> const& wg,
                       Node2                       root,
                       Forest&                     f,
                       size_t max_depth = POSITIVE_INFINITY);

    //! \brief Returns a Forest containing a spanning tree of the nodes
    //! reachable from a given node in a word graph.
    //!
    //! This function returns a Forest containing a spanning
    //! tree of the nodes reachable from \p root in the word graph \p wg
    //! by a path of length at most \p max_depth.
    //!
    //! \tparam Node1 the node type of the word graph.
    //! \tparam Node2 the type of the node \p root.
    //!
    //! \param wg the word graph.
    //! \param root the source node.
    //! \param max_depth the maximum depth of the tree (defaults to
    //! \ref POSITIVE_INFINITY).
    //!
    //! \returns A Forest object containing a spanning tree.
    //!
    //! \note If any target of any edge in the word graph \p wg is out of
    //! bounds, then this is ignored by this function.
    //!
    //! \warning The arguments are not checked, and in particular it is
    //! assumed that \p root is a node of \p wg (i.e. less than
    //! WordGraphView::number_of_nodes).
    template <typename Node1, typename Node2>
    [[nodiscard]] Forest spanning_tree_no_checks(WordGraphView<Node1> const& wg,
                                                 Node2  root,
                                                 size_t max_depth
                                                 = POSITIVE_INFINITY);

    //! \brief Returns a Forest containing a spanning tree of the nodes
    //! reachable from a given node in a word graph.
    //!
    //! This function returns a Forest containing a spanning
    //! tree of the nodes reachable from \p root in the word graph \p wg
    //! by a path of length at most \p max_depth.
    //!
    //! \tparam Node1 the node type of the word graph.
    //! \tparam Node2 the type of the node \p root.
    //!
    //! \param wg the word graph.
    //! \param root the source node.
    //! \param max_depth the maximum depth of the tree (defaults to
    //! \ref POSITIVE_INFINITY).
    //!
    //! \returns A Forest object containing a spanning tree.
    //!
    //! \throws LibsemigroupsException if \p root is out of bounds, i.e.
    //! greater than or equal to WordGraphView::number_of_nodes.
    //!
    //! \note If any target of any edge in the word graph \p wg is out of
    //! bounds, then this is ignored by this function.
    template <typename Node1, typename Node2>
    [[nodiscard]] Forest spanning_tree(WordGraphView<Node1> const& wg,
                                       Node2                       root,
                                       size_t max_depth = POSITIVE_INFINITY);

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
    //! \throws LibsemigroupsException if \p val is not one of: Order::none,
    //! Order::lenlex, or Order::rev_rpo.
    //!
    //! \sa
    //! standardize.
    // TODO(1): Add is_standardized_no_checks?
    template <typename Node>
    bool is_standardized(WordGraphView<Node> const& wg,
                         Order                      val = Order::lenlex);

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

  }  // namespace word_graph

}  // namespace libsemigroups

#include "libsemigroups/word-graph-view-helpers.tpp"
#endif  // LIBSEMIGROUPS_WORD_GRAPH_VIEW_HELPERS_HPP_
