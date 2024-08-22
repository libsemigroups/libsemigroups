//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019-2024 Finn Smith
// Copyright (C) 2019-2024 James D. Mitchell
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

// This file contains an implementation of word graphs (which are basically
// deterministic automata without initial or accept states).

// TODO(now)
// * iwyu
// * word_graph::is_standardized

#ifndef LIBSEMIGROUPS_WORD_GRAPH_HPP_
#define LIBSEMIGROUPS_WORD_GRAPH_HPP_

#include <algorithm>  // for uniform_int_distribution
#include <cstddef>    // for size_t
#include <cstdint>
#include <numeric>        // for accumulate
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

#include "config.hpp"     // for LIBSEMIGROUPS_EIGEN_EN...
#include "constants.hpp"  // for UNDEFINED
#include "debug.hpp"      // for LIBSEMIGROUPS_ASSERT
#include "dot.hpp"        // for Dot
#include "exception.hpp"  // for LIBSEMIGROUPS_EXCEPTION
#include "forest.hpp"     // for Forest
#include "order.hpp"      // for Order
#include "ranges.hpp"
#include "types.hpp"  // for word_type

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

  // TODO(now) doc
  struct WordGraphBase {};

  //! \defgroup word_graph_group Word graphs and related functionality
  //!
  //! TODO
  //!
  //! Blah

  //! \ingroup word_graph_group
  //!
  //! Defined in ``word-graph.hpp``.
  //!
  //! This class represents a word graph.
  //! If the word graph has \c n nodes, they are represented by the numbers
  //! \f$\{0, ..., n - 1\}\f$, and every node has the same number \c m of
  //! out-edges (edges with source that node and target any other node). The
  //! number \c m is referred to as the *out-degree* of the word graph, or any
  //! of its nodes.
  //!
  //! \tparam Node the type of the nodes in the word graph, must be an unsigned
  //! integer type.
  template <typename Node>
  class WordGraph : private WordGraphBase {
    static_assert(std::is_integral<Node>(),
                  "the template parameter Node must be an integral type!");
    static_assert(
        std::is_unsigned<Node>(),
        "the template parameter Node must be an unsigned integral type!");

    template <typename N>
    friend class WordGraph;

    ////////////////////////////////////////////////////////////////////////
    // WordGraph - iterator - private
    ////////////////////////////////////////////////////////////////////////

   public:
    ////////////////////////////////////////////////////////////////////////
    // WordGraph - typedefs - public
    ////////////////////////////////////////////////////////////////////////

    //! The type of nodes in a word graph.
    using node_type = Node;

    //! The type of edge labels in a word graph.
    using label_type = Node;

    //! Unsigned integer type.
    using size_type = std::size_t;

#ifdef LIBSEMIGROUPS_EIGEN_ENABLED
    //! The type of the adjacency matrix.
    using adjacency_matrix_type
        = Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>;
#else
    //! The type of the adjacency matrix.
    using adjacency_matrix_type = IntMat<0, 0, int64_t>;
#endif

    //! The type of an iterator pointing to the nodes of a word graph.
    using const_iterator_nodes =
        typename detail::IntRange<Node>::const_iterator;

    //! The type of a reverse iterator pointing to the nodes of a word graph.
    using const_reverse_iterator_nodes =
        typename detail::IntRange<Node>::const_reverse_iterator;

    //! The type of an iterator pointing to the out-edges of a node in a
    //! word graph.
    using const_iterator_targets =
        typename detail::DynamicArray2<Node>::const_iterator;

    ////////////////////////////////////////////////////////////////////////
    // WordGraph - constructors + destructor - public
    ////////////////////////////////////////////////////////////////////////

    //! Construct from number of nodes and out degree.
    //!
    //! \param m the number of nodes in the word graph (default: 0).
    //! \param n the out-degree of every node (default: 0).
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \par Complexity
    //! \f$O(mn)\f$ where \p m is the number of nodes, and \p n is
    //! the out-degree of the word graph.
    // Not noexcept
    explicit WordGraph(size_type m = 0, size_type n = 0);

    //! Re-initialize the word graph to have \p m nodes and out-degree \p n
    //!
    //! This functions puts a word graph into the state that it would have been
    //! in if it had just been newly constructed with the same parameters \p m
    //! and \p n.
    //!
    //! \param m the number of nodes in the word graph.
    //! \param n the out-degree of every node.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \par Complexity
    //! \f$O(mn)\f$ where \p m is the number of nodes, and \p n is the
    //! out-degree of the word graph.
    WordGraph& init(size_type m = 0, size_type n = 0);

    //! Default copy constructor
    WordGraph(WordGraph const&);

    // TODO other versions from OtherNode
    template <typename OtherNode>
    WordGraph(WordGraph<OtherNode> const&);

    template <typename OtherNode>
    WordGraph& init(WordGraph<OtherNode> const&);

    //! Default move constructor
    WordGraph(WordGraph&&);

    //! Default copy assignment constructor
    WordGraph& operator=(WordGraph const&);

    //! Default move assignment constructor
    WordGraph& operator=(WordGraph&&);

    ~WordGraph();

    //! Construct a random word graph from number of nodes and out-degree.
    //!
    //! \param number_of_nodes the number of nodes
    //! \param out_degree the out-degree of every node
    //! \param mt a std::mt19937 used as a random source (defaults to:
    //! std::mt19937(std::random_device()()))
    //!
    //! \throws LibsemigroupsException if any of the following hold:
    //! * \p number_of_nodes is less than \c 2
    //! * \p out_degree is less than \c 2
    //!
    //! \par Complexity
    //! \f$O(mn)\f$ where \p m is the number of nodes, and \p n is
    //! the out-degree of the word graph.
    static WordGraph random(size_type    number_of_nodes,
                            size_type    out_degree,
                            std::mt19937 mt
                            = std::mt19937(std::random_device()()));

    //! Construct a random word graph from number of nodes, edges, and
    //! out-degree.
    //!
    //! \param number_of_nodes the number of nodes
    //! \param out_degree the out-degree of every node
    //! \param number_of_edges ??
    //! \param mt a std::mt19937 used as a random source (defaults to:
    //! std::mt19937(std::random_device()()))
    //!
    //! \throws LibsemigroupsException if any of the following hold:
    //! * \p number_of_nodes is less than \c 2
    //! * \p out_degree is less than \c 2
    //! * \p number_of_edges exceeds the product of \p number_of_nodes and \p
    //! out_degree
    //!
    //! \par Complexity
    //! At least \f$O(mn)\f$ where \p m is the number of nodes, and \p n is the
    //! out-degree of the word graph.
    static WordGraph random(size_type    number_of_nodes,
                            size_type    out_degree,
                            size_type    number_of_edges,
                            std::mt19937 mt
                            = std::mt19937(std::random_device()()));

    //! Construct a random acyclic word graph from number of nodes, edges, and
    //! out-degree.
    //!
    //! \param number_of_nodes the number of nodes
    //! \param out_degree the out-degree of every node
    //! \param number_of_edges ??
    //! \param mt a std::mt19937 used as a random source (defaults to:
    //! std::mt19937(std::random_device()()))
    //!
    //! \throws LibsemigroupsException if any of the following hold:
    //! * \p number_of_nodes is less than \c 2
    //! * \p out_degree is less than \c 2
    //! * \p number_of_edges exceeds the product of \p number_of_nodes and \p
    //! out_degree
    //! * \p number_of_edges exceeds the product of \p number_of_nodes and \p
    //! number_of_nodes - 1 divided by 2.
    //!
    //! \par Complexity
    //! At least \f$O(mn)\f$ where \p m is the number of nodes, and \p n is the
    //! out-degree of the word graph.
    static WordGraph random_acyclic(size_type    number_of_nodes,
                                    size_type    out_degree,
                                    size_type    number_of_edges,
                                    std::mt19937 mt
                                    = std::mt19937(std::random_device()()));

    ////////////////////////////////////////////////////////////////////////
    // WordGraph - modifiers - public
    ////////////////////////////////////////////////////////////////////////

    //! Adds a number of new nodes.
    //!
    //! \param nr the number of nodes to add.
    //!
    //! \returns
    //! (None)
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //! \strong_guarantee
    //!
    //! \complexity
    //! Linear in `number_of_nodes() + nr`.
    //!
    //! \iterator_validity
    //! \iterator_invalid
    // Not noexcept because DynamicArray2::add_rows isn't.
    WordGraph& add_nodes(size_type nr);

    //! Adds to the out-degree.
    //!
    //! \param nr the number of new out-edges for every node.
    //!
    //! \returns
    //! (None)
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //! \strong_guarantee
    //!
    //! \complexity
    //! \f$O(mn)\f$ where \c m is the number of nodes, and \c n is the new out
    //! degree of the word graph.
    //!
    //! \iterator_validity
    //! \iterator_invalid
    // Not noexcept because DynamicArray2::add_cols isn't.
    WordGraph& add_to_out_degree(size_type nr);

    //! Restrict the word graph to its first \p n nodes.
    //!
    //! \param n the number of nodes
    //!
    //! \returns (None)
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \warning
    //! This function performs no checks whatsoever and will result in a
    //! corrupted word graph if there are any edges from the nodes \f$0, \ldots,
    //! n
    //! - 1\f$ to nodes larger than \f$n - 1\f$.
    // TODO(later) non_no_checks version

    // Only valid if no edges incident to nodes in [first, last) point outside
    // [first, last)
    void induced_subgraph_no_checks(node_type first, node_type last);

    template <typename Iterator,
              typename = std::enable_if_t<detail::IsIterator<Iterator>::value>>
    void induced_subgraph_no_checks(Iterator first, Iterator last);

    //! Add an edge from one node to another with a given label.
    //!
    //! If \p i and \p j are nodes in \c this, and \p lbl is in the range `[0,
    //! out_degree())`, then this function adds an edge from \p i to \p j
    //! labelled \p lbl.
    //!
    //! \param m the source node
    //! \param lbl the label of the edge from \p m to \p n
    //! \param n the range node
    //!
    //! \returns
    //! (None)
    //!
    //! \throws LibsemigroupsException if \p i, \p j, or \p lbl is
    //! not valid.
    //! \strong_guarantee
    //!
    //! \complexity
    //! Constant.
    // Not noexcept because validate_node/label aren't
    void set_target(node_type m, label_type lbl, node_type n);

    //! Add an edge from one node to another with a given label.
    //!
    //! \param i the source node
    //! \param j the range node
    //! \param lbl the label of the edge from \p i to \p j
    //!
    //! \returns
    //! (None)
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Constant.
    //!
    //! \warning
    //! No checks whatsoever on the validity of the arguments are performed.

    void inline set_target_no_checks(node_type m, label_type lbl, node_type n) {
      _dynamic_array_2.set(m, lbl, n);
    }

    //! Remove an edge from a node with a given label.
    //!
    //! \param i the source node
    //! \param lbl the label of the edge from \p i
    //!
    //! \returns
    //! (None)
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Constant.
    //!
    //! \warning
    //! No checks whatsoever on the validity of the arguments are performed.
    // TODO(later) check version
    void inline remove_target_no_checks(node_type i, label_type lbl) {
      _dynamic_array_2.set(i, lbl, UNDEFINED);
    }

    //! Remove all of the edges in the word graph.
    //!
    //! \returns
    //! (None)
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! \f$O(mn)\f$ where \p m is the number of nodes and \p n is the
    //! out-degree.
    //!
    //! \par Parameters
    //! (None)
    void inline remove_all_targets() {
      std::fill(_dynamic_array_2.begin(), _dynamic_array_2.end(), UNDEFINED);
    }

    void remove_label(label_type lbl);

    void remove_label_no_checks(label_type lbl);

    //! Ensures that the word graph has capacity for a given number of nodes,
    //! and out-degree.
    //!
    //! \note
    //! Does not modify number_of_nodes() or out_degree().
    //!
    //! \param m the number of nodes
    //! \param n the out-degree
    //!
    //! \returns
    //! (None)
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! \f$O(mn)\f$ where \p m is the number of nodes and \p n is the
    //! out-degree.
    //!
    //! \iterator_validity
    //! \iterator_invalid
    // Not noexcept because DynamicArray2::add_cols isn't.
    void reserve(size_type m, size_type n) const;

    //! Swaps the edge with specified label from one node with another.
    //!
    //! This function swaps the target of the edge from the node \p u labelled
    //! \p a with the target of the edge from the node \p v labelled \p a.
    //!
    //! \param u the first node
    //! \param v the second node
    //! \param a the label
    //!
    //! \returns
    //! (None)
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Constant
    //!
    //! \warning
    //! No checks whatsoever on the validity of the arguments are performed.
    // swap u - a - > u' and v - a -> v'
    void swap_targets_no_checks(node_type u, node_type v, label_type a) {
      _dynamic_array_2.swap(u, a, v, a);
    }

    //! Check if two word graphs are equal.
    //!
    //! \param that the word graph for comparisonb
    //!
    //! \returns
    //! A `bool`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! At worst \f$O(nm)\f$ where \f$n\f$ is the number of nodes and \f$m\f$
    //! is the out-degree of the word graph.
    // swap u - a - > u' and v - a -> v'
    [[nodiscard]] bool operator==(WordGraph const& that) const {
      return _dynamic_array_2 == that._dynamic_array_2;
    }

    [[nodiscard]] bool operator!=(WordGraph const& that) const {
      return !operator==(that);
    }

    [[nodiscard]] bool operator<(WordGraph const& that) const {
      return _dynamic_array_2 < that._dynamic_array_2;
    }

    ////////////////////////////////////////////////////////////////////////
    // WordGraph - nodes, targets, etc - public
    ////////////////////////////////////////////////////////////////////////

    //! Get the range of the edge with given source node and label.
    //!
    //! \param v the node
    //! \param lbl the label
    //!
    //! \returns
    //! Returns the node adjacent to \p v via the edge labelled \p lbl, or
    //! libsemigroups::UNDEFINED; both are values of type \ref node_type.
    //!
    //! \complexity
    //! Constant.
    //!
    //! \exception LibsemigroupsException if \p v or \p lbl is not
    //! valid.
    // Not noexcept because validate_node/label aren't
    [[nodiscard]] node_type target(node_type v, label_type lbl) const;

    //! Get the range of the edge with given source node and label.
    //!
    //! \param v the node
    //! \param lbl the label
    //!
    //! \returns
    //! Returns the node adjacent to \p v via the edge labelled \p lbl, or
    //! libsemigroups::UNDEFINED; both are values of type \ref node_type.
    //!
    //! \complexity
    //! Constant.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \warning This function is unsafe because it does not verify \p v or \p
    //! lbl is valid.
    // Not noexcept because DynamicArray2::get is not
    [[nodiscard]] node_type inline target_no_checks(node_type  v,
                                                    label_type lbl) const {
      return _dynamic_array_2.get(v, lbl);
    }

    //! Get the next target of a node that doesn't equal
    //! libsemigroups::UNDEFINED.
    //!
    //! If `target(v, i)` equals libsemigroups::UNDEFINED for every value in
    //! the range \f$[i, n)\f$, where \f$n\f$ is the return value of
    //! out_degree() then \c x.first and \c x.second equal
    //! libsemigroups::UNDEFINED.
    //!
    //! \param v the node
    //! \param i the label
    //!
    //! \returns
    //! Returns a std::pair
    //! \c x where:
    //! 1. \c x.first is adjacent to \p v via an edge labelled
    //!    \c x.second;
    //! 2. \c x.second is the minimum value in the range \f$[i, n)\f$ such that
    //!    `target(v, x.second)` is not equal to libsemigroups::UNDEFINED
    //!    where \f$n\f$ is the return value of out_degree(); and
    //!
    //! \complexity
    //! At worst \f$O(n)\f$ where \f$n\f$ equals out_degree().
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \sa next_target.
    //!
    //! \warning This function is unsafe because it is not verified that the
    //! parameter \p v represents a node of \c this.
    // Not noexcept because DynamicArray2::get is not
    [[nodiscard]] std::pair<node_type, label_type>
        next_target_no_checks(node_type, label_type) const;

    //! Get the next target of a node that doesn't equal
    //! libsemigroups::UNDEFINED.
    //!
    //! If `target(v, i)` equals libsemigroups::UNDEFINED for every value in
    //! the range \f$[i, n)\f$, where \f$n\f$ is the return value of
    //! out_degree() then \c x.first and \c x.second equal
    //! libsemigroups::UNDEFINED.
    //!
    //! \param v the node
    //! \param i the label
    //!
    //! \returns
    //! Returns a std::pair
    //! \c x where:
    //! 1. \c x.first is adjacent to \p v via an edge labelled
    //!    \c x.second;
    //! 2. \c x.second is the minimum value in the range \f$[i, n)\f$ such that
    //!    `target(v, x.second)` is not equal to libsemigroups::UNDEFINED
    //!    where \f$n\f$ is the return value of out_degree(); and
    //!
    //! \complexity
    //! At worst \f$O(n)\f$ where \f$n\f$ equals out_degree().
    //!
    //! \throws LibsemigroupsException if \p v does not represent a node in \c
    //! this.
    //!
    //! \sa next_target_no_checks.
    // Not noexcept because next_target_no_checks is not
    [[nodiscard]] std::pair<node_type, label_type>
    next_target(node_type v, label_type i) const;

    //! Returns the number of nodes.
    //!
    //! \returns
    //! The number of nodes, a value of type \c Node.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    //!
    //! \par Parameters
    //! (None)
    [[nodiscard]] size_type inline number_of_nodes() const noexcept {
      return _nr_nodes;
    }

#ifndef PARSED_BY_DOXYGEN
    WordGraph& number_of_active_nodes(size_type val) {
      _num_active_nodes = val;
      return *this;
    }

    [[nodiscard]] size_type inline number_of_active_nodes() const noexcept {
      return _num_active_nodes;
    }
#endif

    //! Returns the number of edges.
    //!
    //! \returns
    //! The total number of edges, a value of type \c size_t.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! \f$O(mn)\f$ where \c m is number_of_nodes() and \c n is out_degree().
    //!
    //! \par Parameters
    //! (None)
    // Not noexcept because std::count isn't
    [[nodiscard]] size_type number_of_edges() const;

    //! Returns the number of edges with given source node.
    //!
    //! \param n the node.
    //!
    //! \returns
    //! A value of type \c size_t.
    //!
    //! \throws LibsemigroupsException if \p n is not a node.
    //!
    //! \complexity
    //! \f$O(n)\f$ where \c n is out_degree().
    // TODO(later) no_checks version
    [[nodiscard]] size_type number_of_edges(node_type n) const;

    //! Returns the out-degree.
    //!
    //! \returns
    //! The number of out-edges of every node, a value of type \c Node.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    //!
    //! \par Parameters
    //! (None)
    [[nodiscard]] size_type out_degree() const noexcept {
      return _degree;
    }

    //! Returns a random access iterator pointing at the first node of the
    //! word graph.
    //!
    //! \returns
    //! An \ref const_iterator_nodes.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    //!
    //! \par Parameters
    //! (None)
    const_iterator_nodes cbegin_nodes() const noexcept {
      return detail::IntRange<node_type>(0, number_of_nodes()).cbegin();
    }

    // no except correct?
    [[nodiscard]] auto nodes() const noexcept {
      return rx::seq<node_type>() | rx::take(number_of_nodes());
    }

    // no except correct?
    [[nodiscard]] auto labels() const noexcept {
      return rx::seq<label_type>() | rx::take(out_degree());
    }

    //! Returns a random access iterator pointing at the last node of the
    //! word graph.
    //!
    //! \returns
    //! An \ref const_reverse_iterator_nodes.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    //!
    //! \par Parameters
    //! (None)
    const_reverse_iterator_nodes crbegin_nodes() const noexcept {
      return detail::IntRange<node_type>(0, number_of_nodes()).crbegin();
    }

    //! Returns a random access iterator pointing one-past-the-first node of
    //! the word graph.
    //!
    //! \returns
    //! An \ref const_reverse_iterator_nodes.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    //!
    //! \par Parameters
    //! (None)
    const_reverse_iterator_nodes crend_nodes() const noexcept {
      return detail::IntRange<node_type>(0, number_of_nodes()).crend();
    }

    //! Returns a random access iterator pointing one-past-the-last node of the
    //! word graph.
    //!
    //! \returns
    //! An \ref const_iterator_nodes.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    //!
    //! \par Parameters
    //! (None)
    const_iterator_nodes cend_nodes() const noexcept {
      return detail::IntRange<node_type>(0, number_of_nodes()).cend();
    }

    //! Returns a random access iterator pointing at the first target of a
    //! node.
    //!
    //! \param i a node in the word graph.
    //!
    //! \returns
    //! A \ref const_iterator_targets.
    //!
    //! \throws LibsemigroupsException if \p i is not valid.
    //!
    //! \complexity
    //! Constant.
    // Not noexcept because validate_node isn't
    // TODO remove this
    const_iterator_targets cbegin_targets(node_type i) const;

    //! Returns a random access iterator pointing at the first target of a
    //! node.
    //!
    //! \param i a node in the word graph.
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
    // TODO remove this
    const_iterator_targets
    cbegin_targets_no_checks(node_type i) const noexcept {
      return _dynamic_array_2.cbegin_row(i);
    }

    //! Returns a random access iterator pointing one-past-the-last target of
    //! a node.
    //!
    //! \param i a node in the word graph.
    //!
    //! \returns
    //! A \ref const_iterator_targets.
    //!
    //! \throws LibsemigroupsException if \p i is not valid.
    //!
    //! \complexity
    //! Constant.
    // Not noexcept because validate_node isn't
    // TODO remove this
    const_iterator_targets cend_targets(node_type i) const;

    //! Returns a random access iterator pointing one-past-the-last target of
    //! a node.
    //!
    //! \param i a node in the word graph.
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
    // TODO remove this
    const_iterator_targets cend_targets_no_checks(node_type i) const noexcept {
      return _dynamic_array_2.cbegin_row(i) + _degree;
    }

    [[nodiscard]] rx::iterator_range<const_iterator_targets>
    targets_no_checks(node_type n) const noexcept {
      return rx::iterator_range(cbegin_targets_no_checks(n),
                                cend_targets_no_checks(n));
    }

    [[nodiscard]] auto
    labels_and_targets_no_checks(node_type n) const noexcept {
      return rx::enumerate(targets_no_checks(n));
    }

    [[nodiscard]] rx::iterator_range<const_iterator_targets>
    targets(node_type n) const;

    // TODO(0) add _no_checks
    void disjoint_union_inplace(WordGraph<Node> const& that) {
      if (that.number_of_nodes() == 0) {
        return;
      }
      // TODO check out degrees agree!
      size_t old_num_nodes_reachable_from_root = number_of_nodes();
      _dynamic_array_2.append(that._dynamic_array_2);
      auto first
          = _dynamic_array_2.begin_row(old_num_nodes_reachable_from_root);
      auto last = _dynamic_array_2.cend();
      std::for_each(
          first, last, [&old_num_nodes_reachable_from_root](node_type& n) {
            n += old_num_nodes_reachable_from_root;
          });
    }

    // TODO doc
    // TODO move to tpp
    void permute_nodes_no_checks(std::vector<node_type> const& p,
                                 std::vector<node_type> const& q,
                                 size_t                        m) {
      // p : new -> old, q = p ^ -1: old -> new
      node_type c = 0;
      while (c < m) {
        for (auto x : WordGraph<Node>::labels()) {
          node_type i = WordGraph<Node>::target_no_checks(p[c], x);
          WordGraph<Node>::set_target_no_checks(
              p[c], x, (i == UNDEFINED ? i : q[i]));
        }
        c++;
      }
      // Permute the rows themselves
      apply_row_permutation(p);
    }
    void permute_nodes_no_checks(std::vector<node_type> const& p,
                                 std::vector<node_type> const& q) {
      permute_nodes_no_checks(p, q, p.size());
    }

   protected:
    // from WordGraphWithSources
    template <typename S>
    void apply_row_permutation(S const& p) {
      _dynamic_array_2.apply_row_permutation(p);
    }

   private:
    ////////////////////////////////////////////////////////////////////////
    // WordGraph - data members - private
    ////////////////////////////////////////////////////////////////////////

    size_type _degree;
    size_type _nr_nodes;
    // TODO(later) remove when WordGraphView is implemented
    size_type                           _num_active_nodes;
    mutable detail::DynamicArray2<Node> _dynamic_array_2;
  };

  //! \ingroup word_graph_group
  //!
  //! \brief Blurb
  //!
  //! Defined in ``word-graph.hpp``.
  namespace word_graph {

    //////////////////////////////////////////////////////////////////////////
    // WordGraph - helper functions - validation
    //////////////////////////////////////////////////////////////////////////

    // not noexcept because it throws an exception!
    //! \brief Blurb
    //!
    //! This function
    // TODO rename
    template <typename Node1, typename Node2>
    void validate_node(WordGraph<Node1> const& wg, Node2 v);

    //! TODO doc
    template <typename Node>
    void throw_if_any_target_out_of_bounds(WordGraph<Node> const& wg);

    //! No doc
    // not noexcept because it throws an exception!
    // TODO rename
    template <typename Node>
    void validate_label(WordGraph<Node> const&               wg,
                        typename WordGraph<Node>::label_type lbl);

    //////////////////////////////////////////////////////////////////////////
    // WordGraph - helper functions - attributes
    //////////////////////////////////////////////////////////////////////////

#ifdef LIBSEMIGROUPS_EIGEN_ENABLED
    static Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>
    pow(Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> const& x,
        size_t                                                       e);
#endif

    // TODO doc
    template <typename Node>
    auto adjacency_matrix(WordGraph<Node> const& wg);

    //////////////////////////////////////////////////////////////////////////
    // WordGraph - helper functions - operations
    //////////////////////////////////////////////////////////////////////////

    // TODO doc
    // TODO tests
    // TODO version where std::unordered_set is passed by reference
    // TODO version which is an iterator
    template <typename Node1, typename Node2>
    std::unordered_set<Node1> nodes_reachable_from(WordGraph<Node1> const& wg,
                                                   Node2 source);
    // TODO doc
    // TODO tests
    template <typename Node1, typename Node2>
    size_t number_of_nodes_reachable_from(WordGraph<Node1> const& wg,
                                          Node2                   source) {
      return nodes_reachable_from(wg, source).size();
    }

    // TODO(0): doc
    //! TODO
    template <typename Node1, typename Node2, typename Iterator>
    Node1 follow_path(WordGraph<Node1> const& wg,
                      Node2                   from,
                      Iterator                first,
                      Iterator                last);

    //! Find the node that a path starting at a given node leads to.
    //!
    //! \tparam T the type used as the template parameter for the WordGraph.
    //!
    //! \param wg the WordGraph object to check.
    //! \param from the starting node.
    //! \param path the path to follow.
    //!
    //! \returns
    //! A value of type WordGraph::node_type. If one or more edges in
    //! \p path are not defined, then \ref UNDEFINED is returned.
    //!
    //! \throw LibsemigroupsException if \p first is not a node in the word
    //! graph or \p path contains a value that is not an edge-label.
    //!
    //! \par Complexity
    //! Linear in the length of \p path.
    // TODO(later) example
    // not noexcept because WordGraph::target isn't
    template <typename Node1, typename Node2>
    Node1 follow_path(WordGraph<Node1> const& wg,
                      Node2                   from,
                      word_type const&        path) {
      return follow_path(wg, from, path.cbegin(), path.cend());
    }

    //! Follow the path from a specified node labelled by a word.
    //!
    //! This function returns the last node on the path in the word graph
    //! \p wg starting at the node \p from labelled by \f$[first, last)\f$ or
    //! libsemigroups::UNDEFINED.
    //!
    //! \param wg an word graph
    //! \param from the source node
    //! \param first iterator into a word
    //! \param last iterator into a word.
    //!
    //! \exception
    //! \noexcept
    //!
    //! \returns A value of type WordGraph::node_type.
    //!
    //! \complexity
    //! At worst the distance from \p first to \p last.
    //!
    //! \warning
    //! No checks on the arguments of this function are performed.
    template <typename Node1, typename Node2, typename Iterator>
    Node1 follow_path_no_checks(WordGraph<Node1> const& wg,
                                Node2                   from,
                                Iterator                first,
                                Iterator                last) noexcept;

    //!
    //! Follow the path from a specified node labelled by a word.
    //!
    //! This function returns the last node on the path in the word graph
    //! \p wg starting at the node \p from labelled by \p path or
    //! libsemigroups::UNDEFINED.
    //!
    //! \tparam T the node type of the word graph
    //!
    //! \param wg an word graph
    //! \param from the source node
    //! \param path the word
    //!
    //! \exception
    //! \noexcept
    //!
    //! \returns A value of type WordGraph::node_type.
    //!
    //! \complexity
    //! At worst the length of \p path.
    //!
    //! \warning
    //! No checks on the arguments of this function are performed.
    template <typename Node1, typename Node2>
    Node1 follow_path_no_checks(WordGraph<Node1> const& wg,
                                Node2                   from,
                                word_type const&        path) noexcept {
      return follow_path_no_checks(wg, from, path.cbegin(), path.cend());
    }

    //!
    //! Returns the last node on the path labelled by a word and an iterator to
    //! the position in the word reached.
    //!
    //! \tparam T the node type of the word graph
    //! \tparam S the type of the iterators into a word
    //!
    //! \param wg an word graph
    //! \param from the source node
    //! \param first iterator into a word
    //! \param last iterator into a word.
    //!
    //! \exception
    //! \noexcept
    //!
    //! \returns A pair consisting of WordGraph::node_type and \p S.
    //!
    //! \complexity
    //! At worst the distance from \p first to \p last.
    //!
    //! \warning
    //! No checks on the arguments of this function are performed.
    template <typename Node1, typename Node2, typename Iterator>
    std::pair<Node1, Iterator>
    last_node_on_path_no_checks(WordGraph<Node1> const& wg,
                                Node2                   from,
                                Iterator                first,
                                Iterator                last) noexcept;

    //!
    //! TODO
    template <typename Node1, typename Node2>
    std::pair<Node1, word_type::const_iterator>
    last_node_on_path(WordGraph<Node1> const& wg,
                      Node2                   from,
                      word_type const&        w);

    //!
    //! Returns the last node on the path labelled by a word and an iterator to
    //! the position in the word reached.
    //!
    //! \tparam T the node type of the word graph
    //! \tparam S the type of the iterators into a word
    //!
    //! \param wg an word graph
    //! \param from the source node
    //! \param first iterator into a word
    //! \param last iterator into a word.
    //!
    //! \throws LibsemigroupsException if any of the letters in word `[first,
    //! last)` is out of bounds.
    //!
    //! \returns A pair consisting of WordGraph::node_type and \p S.
    //!
    //! \complexity
    //! At worst the distance from \p first to \p last.
    template <typename Node1, typename Node2, typename Iterator>
    std::pair<Node1, Iterator> last_node_on_path(WordGraph<Node1> const& wg,
                                                 Node2                   from,
                                                 Iterator                first,
                                                 Iterator                last);

    //!
    //! Returns the nodes of the word graph in topological order (see below) if
    //! possible.
    //!
    //! If it is not empty, the returned vector has the property that if an
    //! edge from a node \c n points to a node \c m, then \c m occurs before
    //! \c n in the vector.
    //!
    //! \tparam T the type used as the template parameter for the
    //! WordGraph.
    //!
    //! \param wg the WordGraph object to check.
    //!
    //! \returns
    //! A std::vector<WordGraph<T>::node_type> that contains the nodes of
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
    std::vector<Node> topological_sort(WordGraph<Node> const& wg);

    //!
    //! Returns the nodes of the word graph reachable from a given node in
    //! topological order (see below) if possible.
    //!
    //! If it is not empty, the returned vector has the property that
    //! if an edge from a node \c n points to a node \c m, then \c m occurs
    //! before \c n in the vector, and the last item in the vector is \p
    //! source.
    //!
    //! \tparam T the type used as the template parameter for the
    //! WordGraph.
    //!
    //! \param wg the WordGraph object to check.
    //! \param source the source node.
    //!
    //! \returns
    //! A std::vector<WordGraph<T>::node_type> that contains the nodes of
    //! \p wg in topological order (if possible) and is otherwise empty.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \par Complexity
    //! At worst \f$O(m + n)\f$ where \f$m\f$ is the number of nodes in the
    //! subword graph of those nodes reachable from \p source
    //! and \f$n\f$ is the number of edges.
    template <typename Node1, typename Node2>
    std::vector<Node1> topological_sort(WordGraph<Node1> const& wg,
                                        Node2                   source);

    //////////////////////////////////////////////////////////////////////////
    // WordGraph - helper functions - properties
    //////////////////////////////////////////////////////////////////////////

    //!
    //! Check if a word graph is acyclic.
    //!
    //! \tparam T the type used as the template parameter for the
    //! WordGraph.
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
    //! where \f$k\f$ is the WordGraph::out_degree.
    //!
    //! A word graph is acyclic if every directed cycle on the word graph is
    //! trivial.
    //!
    //! \par Example
    //! \code
    //! WordGraph<size_t> wg;
    //! wg.add_nodes(2);
    //! wg.add_to_out_degree(1);
    //! wg.set_target(0, 0, 1);
    //! wg.set_target(1, 0, 0);
    //! word_graph::is_acyclic(wg); // returns false
    //! \endcode
    // Not noexcept because detail::is_acyclic isn't
    template <typename Node>
    bool is_acyclic(WordGraph<Node> const& wg);

    //!
    //! Check if the subword graph induced by the nodes reachable from a source
    //! node is acyclic.
    //!
    //! \tparam T the type used as the template parameter for the
    //! WordGraph.
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
    //! where \f$k\f$ is the WordGraph::out_degree.
    //!
    //! A word graph is acyclic if every directed cycle on the word graph is
    //! trivial.
    //!
    //! \par Example
    //! \code
    //! WordGraph<size_t> wg;
    //! wg.add_nodes(4);
    //! wg.add_to_out_degree(1);
    //! wg.set_target(0, 0, 1);
    //! wg.set_target(1, 0, 0);
    //! wg.set_target(2, 0, 3);
    //! word_graph::is_acyclic(wg); // returns false
    //! word_graph::is_acyclic(wg, 0); // returns false
    //! word_graph::is_acyclic(wg, 1); // returns false
    //! word_graph::is_acyclic(wg, 2); // returns true
    //! word_graph::is_acyclic(wg, 3); // returns true
    //! \endcode
    // Not noexcept because detail::is_acyclic isn't
    template <typename Node1, typename Node2>
    bool is_acyclic(WordGraph<Node1> const& wg, Node2 source);

    //!
    template <typename Node1, typename Node2>
    bool is_acyclic(WordGraph<Node1> const& wg, Node2 source, Node2 target);

    //!
    //! Check if there is a path from one node to another.
    //!
    //! \tparam T the type used as the template parameter for the
    //! WordGraph.
    //!
    //! \param wg the WordGraph object to check.
    //! \param source the source node.
    //! \param target the source node.
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
    //! where \f$k\f$ is the WordGraph::out_degree.
    //!
    //! \note
    //! If \p source and \p target are equal, then, by convention, we consider
    //! \p target to be reachable from \p source, via the empty path.
    //!
    //! \par Example
    //! \code
    //! WordGraph<size_t> wg;
    //! wg.add_nodes(4);
    //! wg.add_to_out_degree(1);
    //! wg.set_target(0, 1, 0);
    //! wg.set_target(1, 0, 0);
    //! wg.set_target(2, 3, 0);
    //! word_graph::is_reachable(wg, 0, 1); // returns true
    //! word_graph::is_reachable(wg, 1, 0); // returns true
    //! word_graph::is_reachable(wg, 1, 2); // returns false
    //! word_graph::is_reachable(wg, 2, 3); // returns true
    //! word_graph::is_reachable(wg, 3, 2); // returns false
    //! \endcode
    template <typename Node1, typename Node2>
    bool is_reachable(WordGraph<Node1> const& wg, Node2 source, Node2 target);

    //!
    //! Check if a word graph is connected.
    //!
    //! \tparam T the type used as the template parameter for the
    //! WordGraph.
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
    //! where \f$k\f$ is the WordGraph::out_degree.
    //!
    //! A word graph is *connected* if for every pair of nodes \f$u\f$ and
    //! \f$v\f$ there exists a sequence \f$u_0 := u, \ldots, u_{n - 1} := v\f$
    //! such that either  \f$(u_i, u_{i + 1})\f$ or \f$(u_{i + 1}, u_i)\f$ is
    //! an edge. Note that \f$u\f$ and \f$v\f$ can be equal, and the sequence
    //! above can be of length \f$0\f$.
    //!
    //! \par Example
    //! \code
    //! auto wg = to_word_graph<uint8_t>(
    //!     5, {{0, 0}, {1, 1}, {2}, {3, 3}});
    //! word_graph::is_connected(wg);  // returns false
    //! \endcode
    template <typename Node>
    bool is_connected(WordGraph<Node> const& wg);

    //!
    //! Check if a word graph is strictly cyclic.
    //!
    //! \tparam T the type used as the template parameter for the
    //! WordGraph.
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
    //! where \f$k\f$ is the WordGraph::out_degree.
    //!
    //! A word graph is *strictly cyclic* if there exists a node \f$v\f$ from
    //! which every node is reachable (including \f$v\f$). There must be a
    //! path of length at least \f$1\f$ from the original node \f$v\f$ to
    //! itself (i.e. \f$v\f$ is not considered to be reachable from itself by
    //! default).
    //!
    //! \par Example
    //! \code
    //! auto wg = to_word_graph<uint8_t>(
    //!     5, {{0, 0}, {1, 1}, {2}, {3, 3}});
    //! word_graph::is_strictly_cyclic(wg);  // returns false
    //! \endcode
    template <typename Node>
    bool is_strictly_cyclic(WordGraph<Node> const& wg);

    //!
    // TODO range version, possibly make this a mem fn, since that would likely
    // be faster (see validate)
    template <typename Node, typename Iterator1, typename Iterator2>
    bool is_complete(WordGraph<Node> const& wg,
                     Iterator1              first_node,
                     Iterator2              last_node);

    //!
    //! Check every node has exactly out_degree() out-edges.
    //!
    //! \returns
    //! A \c bool.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! \f$O(mn)\f$ where \c m is number_of_nodes() and \c n is out_degree().
    //!
    //! \par Parameters
    //! (None)
    template <typename Node>
    bool is_complete(WordGraph<Node> const& wg) noexcept {
      return wg.number_of_edges() == wg.number_of_nodes() * wg.out_degree();
    }

    //!
    // TODO range versions
    template <typename Node,
              typename Iterator1,
              typename Iterator2,
              typename Iterator3>
    bool is_compatible(WordGraph<Node> const& wg,
                       Iterator1              first_node,
                       Iterator2              last_node,
                       Iterator3              first_rule,
                       Iterator3              last_rule);

    //////////////////////////////////////////////////////////////////////////
    // WordGraph - helper functions - modifiers
    //////////////////////////////////////////////////////////////////////////

    //!
    //! Adds a cycle involving the specified range of nodes.
    //!
    //! \tparam T the type used as the template parameter for the
    //! WordGraph. \tparam U the type of an iterator pointing to nodes of
    //! an WordGraph
    //!
    //! \param wg the WordGraph object to add a cycle to.
    //! \param first a const iterator to nodes of \p wg
    //! \param last a const iterator to nodes of \p wg
    //!
    //! \returns
    //! (None)
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \par Complexity
    //! \f$O(m)\f$ where \f$m\f$ is the distance between \p first and \p last.
    //!
    //! \note
    //! The edges added by this function are all labelled \c 0.
    template <typename Node, typename Iterator>
    void add_cycle(WordGraph<Node>& wg, Iterator first, Iterator last);

    //!
    //! \brief Adds a cycle consisting of \p N new nodes.
    //!
    //! \tparam T the type used as the template parameter for the
    //! WordGraph.
    //!
    //! \param wg the WordGraph object to add a cycle to.
    //! \param N the length of the cycle and number of new nodes to add.
    //!
    //! \returns
    //! (None)
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \par Complexity
    //! \f$O(N)\f$ where \f$N\f$ is the second parameter.
    //!
    //! \note
    //! The edges added by this function are all labelled \c 0.
    template <typename T>
    void add_cycle(WordGraph<T>& wg, size_t N) {
      size_t M = wg.number_of_nodes();
      wg.add_nodes(N);
      add_cycle(wg, wg.cbegin_nodes() + M, wg.cend_nodes());
    }

    //!
    // Return value indicates whether or not the graph was modified.
    template <typename Graph>
    bool standardize(Graph& wg, Forest& f, Order val);

    //!
    template <typename Graph>
    std::pair<bool, Forest> standardize(Graph& wg, Order val = Order::shortlex);

    //!
    //! TODO
    template <typename Node>
    Dot dot(WordGraph<Node> const& wg);

    //!
    // TODO to tpp
    template <typename Node>
    [[nodiscard]] bool equal_to(WordGraph<Node> const& x,
                                WordGraph<Node> const& y,
                                Node                   first,
                                Node                   last) {
      using label_type = typename WordGraph<Node>::label_type;

      if (x.out_degree() != y.out_degree()) {
        return false;
      }

      validate_node(x, first);
      validate_node(x, last - 1);
      validate_node(y, first);
      validate_node(y, last - 1);

      for (auto n = first; n != last; ++n) {
        for (label_type a = 0; a < x.out_degree(); ++a) {
          if (x.target_no_checks(n, a) != y.target_no_checks(n, a)) {
            return false;
          }
        }
      }
      return true;
    }

    //!
    // TODO to tpp
    template <typename Node1, typename Node2>
    void spanning_tree(WordGraph<Node1> const& wg, Node2 root, Forest& f) {
      using node_type = typename WordGraph<Node1>::node_type;
      f.init(1);

      std::queue<node_type> queue;
      queue.push(static_cast<node_type>(root));
      do {
        node_type s = queue.front();
        for (auto [a, t] : wg.labels_and_targets_no_checks(s)) {
          if (t != UNDEFINED && t != static_cast<node_type>(root)) {
            if (t >= f.number_of_nodes()) {
              f.add_nodes(t - f.number_of_nodes() + 1);
            }
            if (f.parent(t) == UNDEFINED) {
              f.set_parent_and_label(t, s, a);
              queue.push(t);
            }
          }
        }
        queue.pop();
      } while (!queue.empty());
    }

    //! TODO
    template <typename Node1, typename Node2>
    Forest spanning_tree(WordGraph<Node1> const& wg, Node2 root) {
      Forest f;
      spanning_tree(wg, root, f);
      return f;
    }
  }  // namespace word_graph

  //////////////////////////////////////////////////////////////////////////
  // WordGraph - non-member functions
  //////////////////////////////////////////////////////////////////////////

  //! \ingroup word_graph_group
  //! Output the edges of an WordGraph to a stream.
  //!
  //! This function outputs the word graph \p wg to the stream \p os.
  //! The word graph is represented by the out-neighbours of each node ordered
  //! according to their labels. The symbol `-` is used to denote that an
  //! edge is not defined. For example, the word graph with 1 nodes, out-degree
  //! 2, and a single loop labelled 1 from node 0 to 0 is represented as
  //! `{{-, 0}}`.
  //!
  //! \param os the ostream
  //! \param wg the word graph
  //!
  //! \returns
  //! The first parameter \p os.
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  template <typename Node>
  std::ostream& operator<<(std::ostream& os, WordGraph<Node> const& wg);

  //! \ingroup word_graph_group
  //! Constructs a word graph from a number of nodes and an \c
  //! initializer_list.
  //!
  //! This function constructs an WordGraph from its arguments whose
  //! out-degree is specified by the length of the first \c initializer_list
  //! in the 2nd parameter.
  //!
  //! \tparam Node the type of the nodes of the word graph
  //!
  //! \param num_nodes_reachable_from_root the number of nodes in the word
  //! graph. \param il the out-targets of the word graph.
  //!
  //! \returns A value of type WordGraph.
  //!
  //! \throws LibsemigroupsException
  //! if WordGraph<Node>::set_target throws when adding edges from \p il.
  //!
  //! \complexity
  //! \f$O(mn)\f$ where \f$m\f$ is the length of \p il and \f$n\f$ is the
  //! parameter \p num_nodes_reachable_from_root.
  //!
  //! \par Example
  //! \code
  //! // Construct an word graph with 5 nodes and 10 edges (7 specified)
  //! to_word_graph<uint8_t>(5, {{0, 0}, {1, 1}, {2}, {3, 3}});
  //! \endcode
  template <typename Node>
  WordGraph<Node> to_word_graph(size_t num_nodes_reachable_from_root,
                                std::initializer_list<std::vector<Node>> il);

  //! \ingroup word_graph_group
  template <typename Node>
  WordGraph<Node> to_word_graph(size_t num_nodes_reachable_from_root,
                                std::vector<std::vector<Node>> const& v);

  //! \ingroup word_graph_group
  //! TODO(doc)
  // This class is intentionally not a template so that we don't have to specify
  // the types of the nodes when constructing one of these objects. Instead
  // every member function has a template parameter Node, which is deduced from
  // the argument.
  class HopcroftKarp {
   private:
    detail::Duf<>                             _uf;
    std::stack<std::pair<uint64_t, uint64_t>> _stck;

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

    template <typename Node1, typename Node2>
    void throw_if_bad_args(WordGraph<Node1> const& x,
                           Node2                   xroot,
                           WordGraph<Node1> const& y,
                           Node2                   yroot) const;

   public:
    //! \brief Default constructor.
    //!
    //! Default constructor.
    HopcroftKarp() = default;

    //! \brief Default copy constructor.
    //!
    //! Default copy constructor.
    HopcroftKarp(HopcroftKarp const&) = default;

    //! \brief Default move constructor.
    //!
    //! Default move constructor.
    HopcroftKarp(HopcroftKarp&&) = default;

    //! \brief Default copy assignment operator.
    //!
    //! Default copy assignment operator.
    HopcroftKarp& operator=(HopcroftKarp const&) = default;

    //! \brief Default move assignment operator.
    //!
    //! Default move assignment operator.
    HopcroftKarp& operator=(HopcroftKarp&&) = default;

    ~HopcroftKarp() = default;

    // is x a subrelation of y?
    // TODO(doc)
    // TODO move to tpp
    template <typename Node1, typename Node2>
    bool is_subrelation_no_checks(WordGraph<Node1> const& x,
                                  size_t xnum_nodes_reachable_from_root,
                                  Node2  xroot,
                                  WordGraph<Node1> const& y,
                                  size_t ynum_nodes_reachable_from_root,
                                  Node2  yroot);

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

    // There's no subrelation with the number of nodes reachable from the roots
    // as arguments (6 args in total) because we'd have to check that they were
    // valid, and the only way to do this is to recompute them.

    template <typename Node1, typename Node2>
    bool is_subrelation(WordGraph<Node1> const& x,
                        Node2                   xroot,
                        WordGraph<Node1> const& y,
                        Node2                   yroot) {
      throw_if_bad_args(x, xroot, y, yroot);
      return is_subrelation_no_checks(x, xroot, y, yroot);
    }

    template <typename Node>
    bool is_subrelation(WordGraph<Node> const& x, WordGraph<Node> const& y) {
      return is_subrelation(x, static_cast<Node>(0), y, static_cast<Node>(0));
    }

    template <typename Node>
    void join_no_checks(WordGraph<Node>&       xy,
                        WordGraph<Node> const& x,
                        size_t                 xnum_nodes_reachable_from_root,
                        Node                   xroot,
                        WordGraph<Node> const& y,
                        size_t                 ynum_nodes_reachable_from_root,
                        Node                   yroot);

    // TODO(doc)
    // TODO move to tpp
    template <typename Node>
    void join_no_checks(WordGraph<Node>&       xy,
                        WordGraph<Node> const& x,
                        Node                   xroot,
                        WordGraph<Node> const& y,
                        Node                   yroot);

    template <typename Node>
    void join_no_checks(WordGraph<Node>&       xy,
                        WordGraph<Node> const& x,
                        WordGraph<Node> const& y) {
      return join_no_checks(
          xy, x, static_cast<Node>(0), y, static_cast<Node>(0));
    }

    template <typename Node, typename... Args>
    auto join_no_checks(WordGraph<Node> const& x, Args&&... args)
        -> std::enable_if_t<sizeof...(Args) % 2 == 1, WordGraph<Node>> {
      // The versions of this function changing the 1st argument in-place always
      // have an odd number of arguments, so we check that it's even here (the
      // argument x and an odd number of further arguments).
      WordGraph<Node> xy;
      join_no_checks(xy, x, std::forward<Args>(args)...);
      return xy;
    }

    // There's no join with the number of nodes reachable from the roots
    // as arguments (7 args in total) because we'd have to check that they were
    // valid, and the only way to do this is to recompute them.

    // TODO(doc)
    template <typename Node>
    void join(WordGraph<Node>&       xy,
              WordGraph<Node> const& x,
              Node                   xroot,
              WordGraph<Node> const& y,
              Node                   yroot) {
      throw_if_bad_args(x, xroot, y, yroot);
      join_no_checks(xy, x, xroot, y, yroot);
    }

    template <typename Node>
    void join(WordGraph<Node>&       xy,
              WordGraph<Node> const& x,
              WordGraph<Node> const& y) {
      return join(xy, x, static_cast<Node>(0), y, static_cast<Node>(0));
    }

    template <typename Node, typename... Args>
    auto join(WordGraph<Node> const& x, Args&&... args)
        -> std::enable_if_t<sizeof...(Args) % 2 == 1, WordGraph<Node>> {
      // The versions of this function changing the 1st argument in-place always
      // have an odd number of arguments, so we check that it's even here (the
      // argument x and an odd number of further arguments).
      WordGraph<Node> xy;
      join(xy, x, std::forward<Args>(args)...);
      return xy;
    }
  };  // HopcroftKarp

  // Class for forming the meet of two word graphs
  // template <typename Node>
  // class WordGraphMeeter {
  //  private:
  //   using node_type = std::pair<uint32_t, uint32_t>;

  //   std::unordered_map<node_type, uint32_t, Hash<node_type>> _lookup;
  //   std::vector<node_type>                                   _todo;
  //   std::vector<node_type>                                   _todo_new;
  //   WordGraph<Node> const*                                   _x;
  //   WordGraph<Node> const*                                   _y;
  //   size_t _x_num_nodes_reachable_from_root;
  //   size_t _y_num_nodes_reachable_from_root;
  //   Node   _x_root;
  //   Node   _y_root;

  //   void before_meet() {
  //     // TODO check that xy isn't the same object as _X or _y
  //     if (_x == nullptr || _y == nullptr) {
  //       LIBSEMIGROUPS_EXCEPTION("TODO1");
  //     }
  //     if (_x_num_nodes_reachable_from_root == UNDEFINED) {
  //       _x_num_nodes_reachable_from_root
  //           = word_graph::number_of_nodes_reachable_from(*_x, _x_root);
  //     }
  //     if (_y_num_nodes_reachable_from_root == UNDEFINED) {
  //       _y_num_nodes_reachable_from_root
  //           = word_graph::number_of_nodes_reachable_from(*_y, _y_root);
  //     }
  //   }

  //  public:
  //   WordGraphMeeter()
  //       : _lookup(),
  //         _todo(),
  //         _todo_new(),
  //         _x(),
  //         _y(),
  //         _x_num_nodes_reachable_from_root(),
  //         _y_num_nodes_reachable_from_root(),
  //         _x_root(),
  //         _y_root() {
  //     init();
  //   }

  //   WordGraphMeeter& init() {
  //     _lookup.clear();
  //     _todo.clear();
  //     _todo_new.clear();
  //     _x                               = nullptr;
  //     _y                               = nullptr;
  //     _x_num_nodes_reachable_from_root = UNDEFINED;
  //     _y_num_nodes_reachable_from_root = UNDEFINED;
  //     _x_root                          = 0;
  //     _y_root                          = 0;
  //     return *this;
  //   }
  //   explicit WordGraphMeeter(WordGraph<Node> const& x) : WordGraphMeeter() {
  //     _x = &x;
  //   }

  //   WordGraphMeeter& with_no_checks(WordGraph<Node> const& x) {
  //     if (_x == nullptr || _y != nullptr) {
  //       if (_y != nullptr) {
  //         init();
  //       }
  //       _x = &x;
  //     } else {
  //       _y = &x;
  //     }
  //     return *this;
  //   }

  //   WordGraphMeeter& with(WordGraph<Node> const& x) {
  //     if (_x != nullptr) {
  //       // TODO check the outdegrees
  //     }
  //     return with_no_checks(x);
  //   }

  //   WordGraphMeeter& root(Node root) {
  //     if (_x == nullptr && _y == nullptr) {
  //       LIBSEMIGROUPS_EXCEPTION("TODO2");
  //     } else if (_y == nullptr) {
  //       _x_root = root;
  //     } else {
  //       _y_root = root;
  //     }
  //     return *this;
  //   }

  //   WordGraphMeeter& number_of_nodes(size_t num_nodes_reachable_from_root) {
  //     if (_x == nullptr && _y == nullptr) {
  //       LIBSEMIGROUPS_EXCEPTION("TODO3");
  //     } else if (_y == nullptr) {
  //       _x_num_nodes_reachable_from_root = num_nodes_reachable_from_root;
  //     } else {
  //       _y_num_nodes_reachable_from_root = num_nodes_reachable_from_root;
  //     }
  //     return *this;
  //   }

  //   // TODO(doc)
  //   // TODO move to tpp
  //   void meet(WordGraph<Node>& xy) {
  //     Node next = 0;

  //     _lookup.clear();
  //     _lookup.emplace(std::pair(_x_root, _y_root), next++);

  //     _todo.clear();
  //     _todo.emplace_back(_x_root, _y_root);

  //     size_t const N
  //         = _x->out_degree();  // Must be the same as _y->out_degree()
  //     xy.init(_x_num_nodes_reachable_from_root
  //                 * _y_num_nodes_reachable_from_root,
  //             N);

  //     node_type target;
  //     while (!_todo.empty()) {
  //       _todo_new.clear();
  //       for (auto const& source : _todo) {
  //         auto xy_source = _lookup[source];
  //         for (size_t a = 0; a < N; ++a) {
  //           target = std::pair(_x->target_no_checks(source.first, a),
  //                              _y->target_no_checks(source.second, a));
  //           auto [it, inserted] = _lookup.emplace(target, next);

  //           xy.set_target_no_checks(xy_source, a, it->second);
  //           if (inserted) {
  //             next++;
  //             _todo_new.push_back(std::move(target));
  //           }
  //         }
  //       }
  //       std::swap(_todo, _todo_new);
  //     }
  //     xy.induced_subgraph_no_checks(0, next);
  //   }

  //   void meet(WordGraph<Node>& xy) {
  //     before_meet();
  //     get_no_checks(xy);
  //   }

  //   [[nodiscard]] WordGraph<Node> get() {
  //     WordGraph<Node> wg;
  //     get(wg);
  //     return wg;
  //   }

  //   // is x a subrelation of y
  //   [[nodiscard]] bool is_subrelation_no_checks() {
  //     // If _x is a subrelation of _y, then the meet of _x and _y must be
  //     // _x.
  //     before_meet();
  //     if (_y_num_nodes_reachable_from_root
  //         >= _x_num_nodes_reachable_from_root) {
  //       return false;
  //     }
  //     auto xy = get();
  //     // FIXME the following only works if _x_root == 0 == _y_root
  //     return xy.number_of_nodes() == _x_num_nodes_reachable_from_root
  //            && word_graph::equal_to(*_x,
  //                                    xy,
  //                                    static_cast<Node>(0),
  //                                    static_cast<Node>(xy.number_of_nodes()));
  //   }
  // };

  // template <typename Node>
  // WordGraphMeeter(WordGraph<Node> const&) -> WordGraphMeeter<Node>;

  // TODO to tpp
  template <typename Node>
  std::string to_human_readable_repr(WordGraph<Node> const& wg) {
    // TODO could be more elaborate, include complete, etc
    return fmt::format("<WordGraph with {} nodes, {} edges, & out-degree {}>",
                       wg.number_of_nodes(),
                       wg.number_of_edges(),
                       wg.out_degree());
  }

}  // namespace libsemigroups

#include "word-graph.tpp"

#endif  // LIBSEMIGROUPS_WORD_GRAPH_HPP_
