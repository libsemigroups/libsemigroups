//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019-2025 Finn Smith + James D. Mitchell
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

// This file contains declarations related to word graphs (which are basically
// deterministic automata without initial or accept states).

#ifndef LIBSEMIGROUPS_WORD_GRAPH_HPP_
#define LIBSEMIGROUPS_WORD_GRAPH_HPP_

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

#include "config.hpp"     // for LIBSEMIGROUPS_EIGEN_EN...
#include "constants.hpp"  // for UNDEFINED
#include "debug.hpp"      // for LIBSEMIGROUPS_ASSERT
#include "dot.hpp"        // for Dot
#include "exception.hpp"  // for LIBSEMIGROUPS_EXCEPTION
#include "forest.hpp"     // for Forest
#include "order.hpp"      // for Order
#include "ranges.hpp"     // for ??
#include "types.hpp"      // for word_type, enable_if_is_same

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

  //! \defgroup word_graph_group Word graphs and related functionality
  //!
  //! This page collects various classes and functions related to word graphs
  //! in `libsemigroups`.

  //! \ingroup word_graph_group
  //!
  //! \brief Class for representing word graphs.
  //!
  //! Defined in `word-graph.hpp`.
  //!
  //! Instances of this class represent word graphs. If the word graph has \c n
  //! nodes, they are represented by the numbers \f$\{0, ..., n - 1\}\f$, and
  //! every node has the same number \c m of out-edges (edges with source that
  //! node and target any other node). The number \c m is referred to as the
  //! *out-degree* of the word graph, or any of its nodes.
  //!
  //! \tparam Node the type of the nodes in the word graph, must be an unsigned
  //! integer type.
  template <typename Node>
  class WordGraph {
    static_assert(std::is_integral<Node>(),
                  "the template parameter Node must be an integral type!");
    static_assert(
        std::is_unsigned<Node>(),
        "the template parameter Node must be an unsigned integral type!");

    template <typename N>
    friend class WordGraph;

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

    //! The type of an iterator pointing to the targets of a node.
    using const_iterator_targets =
        typename detail::DynamicArray2<Node>::const_iterator;

    ////////////////////////////////////////////////////////////////////////
    // WordGraph - constructors + destructor - public
    ////////////////////////////////////////////////////////////////////////

    //! \brief Construct from number of nodes and out degree.
    //!
    //! This function constructs a word graph with \p m nodes and where the
    //! maximum out-degree of any node is \p n. There are no edges in the
    //! defined word graph.
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

    //! \brief Re-initialize the word graph to have \p m nodes and out-degree
    //! \p n.
    //!
    //! This function puts a word graph into the state that it would have been
    //! in if it had just been newly constructed with the same parameters \p m
    //! and \p n.
    //!
    //! \param m the number of nodes in the word graph.
    //! \param n the out-degree of every node.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \par Complexity
    //! \f$O(mn)\f$ where \f$m\f$ is the number of nodes, and \f$n\f$ is the
    //! out-degree of the word graph.
    WordGraph& init(size_type m = 0, size_type n = 0);

    //! Default copy constructor
    WordGraph(WordGraph const&);

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
    WordGraph(WordGraph<OtherNode> const& that);

    //! \brief Re-initialize the word graph contain a copy of another.
    //!
    //!
    //! This function puts a word graph into the same state that it would have
    //! been in if it had just been newly copy constructed with the same
    //! parameter \p that.
    //!
    //! \param that the word graph to copy.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \par Complexity
    //! \f$O(mn)\f$ where \p m is the number of nodes, and \p n is the
    //! out-degree of the word graph.
    //!
    //! \note Any edge with target \ref UNDEFINED in \p that will have target
    //! `static_cast<Node>(UNDEFINED)` in `*this`.
    template <typename OtherNode>
    WordGraph& init(WordGraph<OtherNode> const& that);

    //! Default move constructor
    WordGraph(WordGraph&&);

    //! Default copy assignment constructor
    WordGraph& operator=(WordGraph const&);

    //! Default move assignment constructor
    WordGraph& operator=(WordGraph&&);

    ~WordGraph();

    //! \brief Construct a random word graph from number of nodes and
    //! out-degree.
    //!
    //! This function constructs a random word graph with \p number_of_nodes
    //! nodes and out-degree \p out_degree.
    //!
    //! \param number_of_nodes the number of nodes.
    //! \param out_degree the out-degree of every node.
    //! \param mt a std::mt19937 used as a random source (defaults to:
    //! std::mt19937(std::random_device()())).
    //!
    //! \returns A random word graph.
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

    //! \brief Ensures that the word graph has capacity for a given number of
    //! nodes, and out-degree.
    //!
    //!  This function ensures that the word graph has capacity for \p m nodes
    //!  and \p n labels.
    //!
    //! \param m the number of nodes.
    //! \param n the out-degree.
    //!
    //! \returns A reference to `*this`.
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
    //!
    //! \note
    //! Does not modify number_of_nodes() or out_degree().
    // Not noexcept because DynamicArray2::add_cols isn't.
    WordGraph& reserve(size_type m, size_type n);

    ////////////////////////////////////////////////////////////////////////
    // WordGraph - modifiers - public
    ////////////////////////////////////////////////////////////////////////

    //! \brief Add a number of new nodes.
    //!
    //! This function modifies a word graph in-place so that it has \p nr new
    //! nodes added.
    //!
    //! \param nr the number of nodes to add.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //! \strong_guarantee
    //!
    //! \complexity
    //! Linear in `(number_of_nodes() + nr) * out_degree()`.
    //!
    //! \iterator_validity
    //! \iterator_invalid
    // Not noexcept because DynamicArray2::add_rows isn't.
    WordGraph& add_nodes(size_type nr);

    //! \brief Add to the out-degree of every node.
    //!
    //! This function modifies a word graph in-place so that the out-degree is
    //! increased by \p nr.
    //!
    //! \param nr the number of new out-edges for every node.
    //!
    //! \returns A reference to `*this`.
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

    //! \brief Add an edge from one node to another with a given label.
    //!
    //! If \p s and \p t are nodes in \c this, and \p a is in the range
    //! `[0, out_degree())`, then this function adds an edge from \p a to \p b
    //! labelled \p a.
    //!
    //! \param s the source node.
    //! \param a the label of the edge.
    //! \param t the range node.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \throws LibsemigroupsException if \p s, \p a, or \p t is not valid.
    //! \strong_guarantee
    //!
    //! \complexity
    //! Constant.
    // Not noexcept because throw_if_node_out_of_bounds/label aren't
    // return *this by reference.
    WordGraph& target(node_type s, label_type a, node_type t);

    //! \brief Add an edge from one node to another with a given label.
    //!
    //! This function adds an edge from the node \p s to the node \p t with
    //! label \p a.
    //!
    //! \param s the source node.
    //! \param a the label of the edge from \p s to \p t.
    //! \param t the target node.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Constant.
    //!
    //! \warning
    //! No checks whatsoever on the validity of the arguments are performed.
    inline WordGraph& target_no_checks(node_type s, label_type a, node_type t) {
      _dynamic_array_2.set(s, a, t);
      return *this;
    }

    //! \brief Remove an edge from a node with a given label.
    //!
    //! This function removes the edge with source node \p s labelled by \p a.
    //!
    //! \param s the source node.
    //! \param a the label of the edge from \p s.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Constant.
    //!
    //! \warning
    //! No checks whatsoever on the validity of the arguments are performed.
    inline WordGraph& remove_target_no_checks(node_type s, label_type a) {
      _dynamic_array_2.set(s, a, UNDEFINED);
      return *this;
    }

    //! \brief Remove an edge from a node with a given label.
    //!
    //! This function removes the edge with source node \p s labelled by \p a.
    //!
    //! \param s the source node.
    //! \param a the label of the edge from \p s.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \throws LibsemigroupsException if \p s or \p a is out of range.
    //!
    //! \complexity
    //! Constant.
    WordGraph& remove_target(node_type s, label_type a);

    //! \brief Removes a given label from the word graph.
    //!
    //! This function removes the label \p a from a WordGraph
    //! object in-place. This reduces the out-degree by \c 1.
    //!
    //! \param a the label to remove.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \throws LibsemigroupsException if \p a is out of range.
    WordGraph& remove_label(label_type a);

    //! \brief Removes a given label from the word graph.
    //!
    //! This function removes the label \p a from a WordGraph
    //! object in-place. This reduces the out-degree by \c 1.
    //!
    //! \param a the label to remove.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \warning No checks are performed on the argument, and, in particular,
    //! it assumed that \p a is not out of range.
    WordGraph& remove_label_no_checks(label_type a);

    //! \brief Remove all of the edges in the word graph.
    //!
    //! Set every target of every source with every possible label to
    //! \ref UNDEFINED.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! \f$O(mn)\f$ where \p m is the number of nodes and \p n is the
    //! out-degree.
    inline WordGraph& remove_all_targets() {
      std::fill(_dynamic_array_2.begin(), _dynamic_array_2.end(), UNDEFINED);
      return *this;
    }

    //! \brief Swaps the edge with specified label from one node with another.
    //!
    //! This function swaps the target of the edge from the node \p m labelled
    //! \p a with the target of the edge from the node \p n labelled \p a.
    //!
    //! \param m the first node.
    //! \param n the second node.
    //! \param a the label.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Constant
    //!
    //! \warning
    //! No checks whatsoever on the validity of the arguments are performed.
    // swap m - a - > m' and n - a -> n'
    WordGraph& swap_targets_no_checks(node_type m, node_type n, label_type a) {
      _dynamic_array_2.swap(m, a, n, a);
      return *this;
    }

    //! \brief Swaps the edge with specified label from one node with another.
    //!
    //! This function swaps the target of the edge from the node \p m labelled
    //! \p a with the target of the edge from the node \p n labelled \p a.
    //!
    //! \param m the first node.
    //! \param n the second node.
    //! \param a the label.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \throws LibsemigroupsException if \p m, \p n, or \p a are out of range.
    //!
    //! \complexity
    //! Constant
    WordGraph& swap_targets(node_type m, node_type n, label_type a);

    //! \brief Check if two word graphs are equal.
    //!
    //! This function returns \c true if `*this` and \p that are equal, and
    //! \c false if not.
    //!
    //! \param that the word graph for comparison.
    //!
    //! \returns
    //! Whether or not `*this` and \c that are equal.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! At worst \f$O(nm)\f$ where \f$n\f$ is the number of nodes and \f$m\f$
    //! is the out-degree of the word graph.
    [[nodiscard]] bool operator==(WordGraph const& that) const {
      return _dynamic_array_2 == that._dynamic_array_2;
    }

    //! \brief Check if two word graphs are inequal.
    //!
    //! This function returns \c true if `*this` and \p that are not equal, and
    //! \c false if they are equal.
    //!
    //! \param that the word graph for comparison.
    //!
    //! \returns
    //! Whether or not `*this` and \c that are not equal.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! At worst \f$O(nm)\f$ where \f$n\f$ is the number of nodes and \f$m\f$
    //! is the out-degree of the word graph.
    [[nodiscard]] bool operator!=(WordGraph const& that) const {
      return !operator==(that);
    }

    //! \brief Check if a word graph is less than another.
    //!
    //! This function returns \c true if `*this` is less than \p that. This
    //! operator defines a linear order on word graphs.
    //!
    //! \param that the word graph for comparison.
    //!
    //! \returns
    //! Whether or not `*this` is strictly less than \c that.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! At worst \f$O(nm)\f$ where \f$n\f$ is the number of nodes and \f$m\f$
    //! is the out-degree of the word graph.
    [[nodiscard]] bool operator<(WordGraph const& that) const {
      return _dynamic_array_2 < that._dynamic_array_2;
    }

    //! \brief Check if a word graph is less than or equal to another.
    //!
    //! This function returns \c true if `*this` is less or equal to \p that.
    //! This operator defines a linear order on word graphs.
    //!
    //! \param that the word graph for comparison.
    //!
    //! \returns
    //! Whether or not `*this` is less than or equal to \c that.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! At worst \f$O(nm)\f$ where \f$n\f$ is the number of nodes and \f$m\f$
    //! is the out-degree of the word graph.
    [[nodiscard]] bool operator<=(WordGraph const& that) const {
      return _dynamic_array_2 <= that._dynamic_array_2;
    }

    //! \brief Check if a word graph is greater than another.
    //!
    //! This function returns \c true if `*this` is greater than \p that.
    //! This operator defines a linear order on word graphs.
    //!
    //! \param that the word graph for comparison.
    //!
    //! \returns
    //! Whether or not `*this` is strictly greater than \c that.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! At worst \f$O(nm)\f$ where \f$n\f$ is the number of nodes and \f$m\f$
    //! is the out-degree of the word graph.
    [[nodiscard]] bool operator>(WordGraph const& that) const {
      return _dynamic_array_2 > that._dynamic_array_2;
    }

    //! \brief Check if a word graph is greater than or equal to another.
    //!
    //! This function returns \c true if `*this` is greater or equal to \p that.
    //! This operator defines a linear order on word graphs.
    //!
    //! \param that the word graph for comparison.
    //!
    //! \returns
    //! Whether or not `*this` is greater than or equal to \c that.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! At worst \f$O(nm)\f$ where \f$n\f$ is the number of nodes and \f$m\f$
    //! is the out-degree of the word graph.
    [[nodiscard]] bool operator>=(WordGraph const& that) const {
      return _dynamic_array_2 > that._dynamic_array_2;
    }

    //! \brief Returns a hash value.
    //!
    //! \returns
    //! A value of type \c size_t.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Linear in \ref number_of_nodes times \ref out_degree.
    // not noexcept because Hash<T>::operator() isn't
    [[nodiscard]] size_t hash_value() const {
      return std::hash<decltype(_dynamic_array_2)>()(_dynamic_array_2);
    }

    ////////////////////////////////////////////////////////////////////////
    // WordGraph - nodes, targets, etc - public
    ////////////////////////////////////////////////////////////////////////

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
    //! \throws LibsemigroupsException if \p source or \p a is not
    //! valid.
    //!
    //! \complexity
    //! Constant.
    // Not noexcept because throw_if_node_out_of_bounds/label aren't
    [[nodiscard]] node_type target(node_type source, label_type a) const;

    //! \brief Get the target of the edge with given source node and label.
    //!
    //! This function returns the the target of the edge with source node \p s
    //! and label \p a.
    //!
    //! \param s the node.
    //! \param a the label.
    //!
    //! \returns
    //! Returns the node adjacent to \p s via the edge labelled \p a, or
    //! \ref UNDEFINED; both are values of type \ref node_type.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Constant.
    //!
    //! \warning This function does not verify \p s or \p a is valid.
    // Not noexcept because DynamicArray2::get is not
    [[nodiscard]] node_type inline target_no_checks(node_type  s,
                                                    label_type a) const {
      return _dynamic_array_2.get(s, a);
    }

    //! \brief Get the next target of an edge incident to a given node that
    //! doesn't equal \ref UNDEFINED.
    //!
    //! This function returns the next target of an edge with label greater
    //! than or equal to \p a that is incident to the node \p s.
    //!
    //! \param s the source node.
    //! \param a the label.
    //!
    //! \returns
    //! Returns a std::pair \c x where:
    //! 1. \c x.first is adjacent to \p s via an edge labelled \c x.second;
    //! 2. \c x.second is the minimum value in the range \f$[a, n)\f$ such that
    //!    `target(s, x.second)` is not equal to \ref UNDEFINED (where \f$n\f$
    //!    is the return value of \ref out_degree); and
    //! If no such value exists, then `{UNDEFINED, UNDEFINED}` is returned.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! At worst \f$O(n)\f$ where \f$n\f$ equals out_degree().
    //!
    //! \warning This function does not check its arguments, in particular it is
    //! not verified that the parameter \p s represents a node of \c this, or
    //! that \p a is a valid label.
    //!
    //! \sa next_label_and_target.
    // Not noexcept because DynamicArray2::get is not
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
    //! \throws LibsemigroupsException if \p s does not represent a node in
    //! \c this, or \p a is not a valid edge label.
    //!
    //! \complexity
    //! At worst \f$O(n)\f$ where \f$n\f$ equals out_degree().
    //!
    //! \sa next_label_and_target_no_checks.
    // Not noexcept because next_label_and_target_no_checks is not
    [[nodiscard]] std::pair<label_type, node_type>
    next_label_and_target(node_type s, label_type a) const;

    //! \brief Returns the number of nodes.
    //!
    //! This function returns the number of nodes in the word graph.
    //!
    //! \returns
    //! The number of nodes in the word graph.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    [[nodiscard]] size_type inline number_of_nodes() const noexcept {
      return _nr_nodes;
    }

#ifndef LIBSEMIGROUPS_PARSED_BY_DOXYGEN
    WordGraph& number_of_active_nodes(size_type val) {
      _num_active_nodes = val;
      return *this;
    }

    [[nodiscard]] size_type inline number_of_active_nodes() const noexcept {
      return _num_active_nodes;
    }
#endif

    //! \brief Returns the number of edges.
    //!
    //! This function returns the total number of edges (i.e. values \c s and
    //! \c a such that `target(s, a)` is not UNDEFINED) in the word graph.
    //!
    //! \returns
    //! The total number of edges, a value of type \c size_type.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! \f$O(mn)\f$ where \c m is number_of_nodes() and \c n is out_degree().
    // Not noexcept because std::count isn't
    [[nodiscard]] size_type number_of_edges() const;

    //! \brief Returns the number of edges with given source node.
    //!
    //! This function returns the number of edges incident to the given source
    //! node \p s.
    //!
    //! \param s the node.
    //!
    //! \returns
    //! A value of type \c size_type.
    //!
    //! \throws LibsemigroupsException if \p s is not a node.
    //!
    //! \complexity
    //! \f$O(n)\f$ where \c n is out_degree().
    [[nodiscard]] size_type number_of_edges(node_type s) const;

    //! \brief Returns the number of edges with given source node.
    //!
    //! This function returns the number of edges incident to the given source
    //! node \p s.
    //!
    //! \param s the node.
    //!
    //! \returns
    //! A value of type \c size_type.
    //!
    //! \complexity
    //! \f$O(n)\f$ where \c n is out_degree().
    //!
    //! \warning No checks are performed that the argument \p s is actually a
    //! node in the word graph.
    [[nodiscard]] size_type number_of_edges_no_checks(node_type s) const;

    //! \brief Returns the out-degree.
    //!
    //! This function returns the number of edge labels in the word graph.
    //!
    //! \returns
    //! The number of edge labels, a value of type \c size_type.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    [[nodiscard]] size_type out_degree() const noexcept {
      return _degree;
    }

    //! \brief Returns a random access iterator pointing at the first node of
    //! the word graph.
    //!
    //! This function returns a random access iterator pointing at the first
    //! node on the word graph.
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

    //! \brief Returns a random access iterator pointing one-past-the-last node
    //! of the word graph.
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
    //! target of the edge with label \c 0 incident to the source node
    //! \p source. This target might equal \ref UNDEFINED.
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
    [[nodiscard]] const_iterator_targets cbegin_targets(node_type source) const;

    //! \brief Returns a random access iterator pointing at the target of
    //! the edge with label \p 0 incident to a given node.
    //!
    //! This function returns a random access iterator pointing at the
    //! target of the edge with label \c 0 incident to the source node
    //! \p source. This target might equal \ref UNDEFINED.
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
    [[nodiscard]] const_iterator_targets
    cbegin_targets_no_checks(node_type source) const noexcept {
      return _dynamic_array_2.cbegin_row(source);
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
    [[nodiscard]] const_iterator_targets cend_targets(node_type source) const;

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
    [[nodiscard]] const_iterator_targets
    cend_targets_no_checks(node_type source) const noexcept {
      return _dynamic_array_2.cbegin_row(source) + _degree;
    }

    //! \brief Returns a range object containing all nodes in a word graph.
    //!
    //! This function returns a range object containing all the nodes in a
    //! word graph.
    //!
    //! \returns A range object.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] auto nodes() const noexcept {
      return rx::seq<node_type>() | rx::take(number_of_nodes());
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
      return rx::seq<label_type>() | rx::take(out_degree());
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
    [[nodiscard]] rx::iterator_range<const_iterator_targets>
    targets_no_checks(node_type source) const noexcept {
      return rx::iterator_range(cbegin_targets_no_checks(source),
                                cend_targets_no_checks(source));
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
    //! \throws LibsemigroupsException if \p source is out of range (i.e. it is
    //! greater than or equal to \ref number_of_nodes)
    [[nodiscard]] rx::iterator_range<const_iterator_targets>
    targets(node_type source) const;

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

    //! \brief Modify in-place to contain the subgraph induced by a range of
    //! nodes.
    //!
    //! This function modifies a WordGraph object in-place to contain its
    //! subgraph induced by the range of nodes \p first to \p last.
    //!
    //! \param first the first node.
    //! \param last one more than the last node.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \warning
    //! This function performs no checks whatsoever and will result in a
    //! corrupted word graph if there are any edges from the nodes \f$0, \ldots,
    //! n - 1\f$ to nodes larger than \f$n - 1\f$.
    WordGraph& induced_subgraph_no_checks(node_type first, node_type last);

    //! \brief Modify in-place to contain the subgraph induced by a range of
    //! nodes.
    //!
    //! This function modifies a WordGraph object in-place to contain its
    //! subgraph induced by the range of nodes \p first to \p last.
    //!
    //! \param first the first node.
    //! \param last one more than the last node.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \throws LibsemigroupsException if \p first or \p last is out of range.
    //! \throws LibsemigroupsException if any edge with source in the range
    //! \p first to \p last has target outside the range \p first to \p last.
    WordGraph& induced_subgraph(node_type first, node_type last);

    //! \brief Modify in-place to contain the subgraph induced by a range of
    //! nodes.
    //!
    //! This function modifies a WordGraph object in-place to contain its
    //! subgraph induced by the range of nodes \p first to \p last.
    //!
    //! \tparam Iterator the type of \p first and \p last (should be
    //! iterators).
    //!
    //! \param first the first node.
    //!
    //! \param last one more than the last node.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \warning
    //! This function performs no checks whatsoever and will result in a
    //! corrupted word graph if there are any edges from the nodes \f$0, \ldots,
    //! n - 1\f$ to nodes larger than \f$n - 1\f$.
    template <typename Iterator,
              typename = std::enable_if_t<detail::IsIterator<Iterator>::value>>
    WordGraph& induced_subgraph_no_checks(Iterator first, Iterator last);

    //! \brief Modify in-place to contain the subgraph induced by a range of
    //! nodes.
    //!
    //! This function modifies a WordGraph object in-place to contain its
    //! subgraph induced by the range of nodes \p first to \p last.
    //!
    //! \tparam Iterator the type of \p first and \p last (should be
    //! iterators).
    //!
    //! \param first iterator pointing at the first node.
    //!
    //! \param last iterator pointing one beyond the last node.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \throws LibsemigroupsException if any value in the range specified by
    //! \p first and \p last is not a node of the word graph.
    //!
    //! \throws LibsemigroupsException if any target of any edge with
    //! source node in the range specified by \p first and \p last does not
    //! belong to the same range.
    template <typename Iterator,
              typename = std::enable_if_t<detail::IsIterator<Iterator>::value>>
    WordGraph& induced_subgraph(Iterator first, Iterator last);

    //! \brief Unites a word graph in-place.
    //!
    //! This function changes a WordGraph object in-place to contain the
    //! disjoint union of itself and \p that. The node \c n of \p that is mapped
    //! to `this->number_of_nodes() + n`.
    //!
    //! \param that the word graph to unite.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \warning This function does not check its arguments, and it is assumed
    //! that `*this` and \p that have equal out degree.
    WordGraph& disjoint_union_inplace_no_checks(WordGraph<Node> const& that);

    //! \brief Unites a word graph in-place.
    //!
    //! This function changes a WordGraph object in-place to contain the
    //! disjoint union of itself and \p that. The node \c n of \p that is mapped
    //! to `this->number_of_nodes() + n`.
    //!
    //! \param that the word graph to unite.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \throws LibsemigroupsException if `*this` and `that` do not have the
    //! same out-degree.
    WordGraph& disjoint_union_inplace(WordGraph<Node> const& that);

#ifndef LIBSEMIGROUPS_PARSED_BY_DOXYGEN
    // These are currently undocumented, because they are hard to use correctly,
    // shouldn't p and q be actual permutation objects?

    // requires access to apply_row_permutation so can't be helper
    WordGraph& permute_nodes_no_checks(std::vector<node_type> const& p,
                                       std::vector<node_type> const& q,
                                       size_t                        m);

    WordGraph& permute_nodes_no_checks(std::vector<node_type> const& p,
                                       std::vector<node_type> const& q) {
      return permute_nodes_no_checks(p, q, p.size());
    }
#endif

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
    // TODO(1) remove when WordGraphView is implemented
    size_type                           _num_active_nodes;
    mutable detail::DynamicArray2<Node> _dynamic_array_2;
  };

  // WordGraph - Exception helpers
  namespace word_graph {
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
    template <typename Node>
    void throw_if_any_target_out_of_bounds(WordGraph<Node> const& wg);

    //! \brief Throws if the target of any edge with source in a given range is
    //! out of bounds.
    //!
    //! This function throws if any target of any edge in \p wg whose source is
    //! in the range defined by \p first and \p last is out of bounds (i.e. is
    //! greater than or equal to WordGraph::number_of_nodes, and not equal to
    //! \ref UNDEFINED).
    //!
    //! \tparam Node the type of the nodes in \p wg.
    //! \tparam Iterator the type of the 2nd and 3rd arguments.
    //!
    //! \param wg the word graph to check.
    //! \param first iterator pointing at the first node to check.
    //! \param last iterator pointing one beyond the last node to check.
    //!
    //! \throws LibsemigroupsException if any target of any edge in \p wg with
    //! source in the range \p first to \p last is greater than or equal to
    //! WordGraph::number_of_nodes and not equal to \ref UNDEFINED.
    //!
    //! \throws LibsemigroupsException if any node in the range \p first to
    //! \p last is out of bounds (i.e. not a node of \p wg).
    template <typename Node, typename Iterator>
    void throw_if_any_target_out_of_bounds(WordGraph<Node> const& wg,
                                           Iterator               first,
                                           Iterator               last);

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
    // not noexcept because it throws an exception!
    template <typename Node>
    void throw_if_label_out_of_bounds(WordGraph<Node> const&               wg,
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
    template <typename Node>
    void throw_if_label_out_of_bounds(WordGraph<Node> const& wg,
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
    template <typename Node, typename Iterator>
    void throw_if_label_out_of_bounds(WordGraph<Node> const& wg,
                                      Iterator               first,
                                      Iterator               last);

    //! \brief Throws if a node is out of bounds.
    //!
    //! This function throws if the node \p n is out of bounds
    //! i.e. if it is greater than or equal to `wg.number_of_nodes()`.
    //!
    //! \tparam Node1 the node type of the word graph.
    //! \tparam Node2 the type of the node \p n.
    //!
    //! \param wg the word graph.
    //! \param n the node to check.
    //!
    //! \throws LibsemigroupsException if \p n is out of bounds.
    // not noexcept because it throws an exception!
    template <typename Node1, typename Node2>
    void throw_if_node_out_of_bounds(WordGraph<Node1> const& wg, Node2 n);

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
    // not noexcept because it throws an exception!
    template <typename Node, typename Iterator1, typename Iterator2>
    void throw_if_node_out_of_bounds(WordGraph<Node> const& wg,
                                     Iterator1              first,
                                     Iterator2              last);
  }  // namespace word_graph
}  // namespace libsemigroups

#include "word-graph.tpp"

#endif  // LIBSEMIGROUPS_WORD_GRAPH_HPP_
