//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019 Finn Smith
// Copyright (C) 2019 James D. Mitchell
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

// This file contains an implementation of out-regular digraph which represents
// the action of a semigroup on a set.

// TODO(later)
// 1. More benchmarks

#ifndef LIBSEMIGROUPS_DIGRAPH_HPP_
#define LIBSEMIGROUPS_DIGRAPH_HPP_

#include <algorithm>    // for uniform_int_distribution
#include <cstddef>      // for size_t
#include <cstdint>      // for uint64_t, int64_t
#include <iterator>     // for forward_iterator_tag, distance
#include <numeric>      // for accumulate
#include <ostream>      // for operator<<
#include <queue>        // for queue
#include <random>       // for mt19937
#include <stack>        // for stack
#include <string>       // for to_string
#include <tuple>        // for tie
#include <type_traits>  // for is_integral, is_unsigned
#include <utility>      // for pair
#include <vector>       // for vector

#include "config.hpp"          // for LIBSEMIGROUPS_EIGEN_ENABLED
#include "constants.hpp"       // for UNDEFINED
#include "containers.hpp"      // for DynamicArray2
#include "debug.hpp"           // for LIBSEMIGROUPS_ASSERT
#include "digraph-helper.hpp"  // for is_reachable
#include "exception.hpp"       // for LIBSEMIGROUPS_EXCEPTION
#include "forest.hpp"          // for Forest
#include "int-range.hpp"       // for IntegralRange
#include "iterator.hpp"        // for ConstIteratorStateless
#include "matrix.hpp"          // for IntMat
#include "types.hpp"           // for word_type
#include "word.hpp"            // for number_of_words

#ifdef LIBSEMIGROUPS_EIGEN_ENABLED
#include <Eigen/Core>
#endif

namespace libsemigroups {

  namespace detail {
    static inline double magic_number(size_t N) {
      return 0.0015 * N + 2.43;
    }

    // Implemented at end of this file.
    template <typename Mat, typename T>
    Mat adjacency_matrix(ActionDigraph<T> const& ad);

#ifdef LIBSEMIGROUPS_EIGEN_ENABLED
    static inline Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>
    pow(Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> const& x,
        size_t                                                       e);
#endif
  }  // namespace detail

  //! Defined in ``digraph.hpp``.
  //!
  //! This class represents the digraph of an action of a semigroup on a set.
  //! If the digraph has \p n nodes, they are represented by the numbers
  //! \f${0, ..., n - 1}\f$, and every node has the same number \c m of
  //! out-edges (edges with source that node and range any other node). The
  //! number \c m is referred to as the *out-degree* of the digraph, or any of
  //! its nodes.
  //!
  //! \tparam T the type of the nodes in the digraph, must be an unsigned
  //! integer type.
  //!
  //! \sa Action.
  template <typename T>
  class ActionDigraph {
    static_assert(std::is_integral<T>(),
                  "the template parameter T must be an integral type!");
    static_assert(
        std::is_unsigned<T>(),
        "the template parameter T must be an unsigned integral type!");

    ////////////////////////////////////////////////////////////////////////
    // ActionDigraph - iterator - private
    ////////////////////////////////////////////////////////////////////////

    using internal_scc_iterator_type =
        typename std::vector<std::vector<T>>::const_iterator;

    struct Deref {
      T const& operator()(internal_scc_iterator_type const& it) const noexcept {
        return *(*it).cbegin();
      };
    };

    struct AddressOf {
      T const* operator()(internal_scc_iterator_type const& it) const noexcept {
        return &(*(*it).cbegin());
      }
    };

    struct IteratorTraits
        : detail::ConstIteratorTraits<std::vector<std::vector<T>>> {
      using value_type      = T;
      using reference       = T&;
      using const_reference = T const&;
      using const_pointer   = T const*;
      using pointer         = T*;

      using Deref     = ActionDigraph::Deref;
      using AddressOf = ActionDigraph::AddressOf;
    };

   public:
    ////////////////////////////////////////////////////////////////////////
    // ActionDigraph - typedefs - public
    ////////////////////////////////////////////////////////////////////////

    //! The type of nodes in a digraph.
    using node_type = T;

    //! The type of edge labels in a digraph.
    using label_type = T;

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

    //! The type of an index in a strongly connected component of a digraph.
    using scc_index_type = T;

    //! The type of an iterator pointing to the nodes of a digraph.
    using const_iterator_nodes = typename IntegralRange<T>::const_iterator;

    //! The type of a reverse iterator pointing to the nodes of a digraph.
    using const_reverse_iterator_nodes =
        typename IntegralRange<T>::const_reverse_iterator;

    //! The type of an iterator pointing to the out-edges of a node in a
    //! digraph.
    using const_iterator_edges =
        typename detail::DynamicArray2<T>::const_iterator;

    //! The type of an iterator pointing to the nodes in a strongly connected
    //! component of a digraph.
    using const_iterator_scc = typename std::vector<T>::const_iterator;

    //! The type of an iterator pointing to the strongly connected components
    //! of a digraph.
    using const_iterator_sccs =
        typename std::vector<std::vector<T>>::const_iterator;

    //! The type of an iterator pointing to the roots of a strongly connected
    //! components of a digraph.
    using const_iterator_scc_roots
        = detail::ConstIteratorStateless<IteratorTraits>;

    //! An enum for specifying the algorithm to the functions number_of_paths().
    enum class algorithm {
      //! Use a depth-first-search.
      dfs = 0,
      //! Use the adjacency matrix and matrix multiplication
      matrix,
      //! Use a dynamic programming approach for acyclic digraphs
      acyclic,
      //! Try to utilise some corner cases.
      trivial,
      //! The function number_of_paths() tries to decide which algorithm is
      //! best.
      automatic
    };

    ////////////////////////////////////////////////////////////////////////
    // ActionDigraph - constructors + destructor - public
    ////////////////////////////////////////////////////////////////////////

    //! Construct from number of nodes and out degree.
    //!
    //! \param m the number of nodes in the digraph (default: 0).
    //! \param n the out-degree of every node (default: 0).
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \par Complexity
    //! \f$O(mn)\f$ where \p m is the number of nodes, and \p n is
    //! the out-degree of the digraph.
    // Not noexcept
    explicit ActionDigraph(T m = 0, T n = 0);

    //! Default copy constructor
    ActionDigraph(ActionDigraph const&);

    //! Default move constructor
    ActionDigraph(ActionDigraph&&);

    //! Default copy assignment constructor
    ActionDigraph& operator=(ActionDigraph const&);

    //! Default move assignment constructor
    ActionDigraph& operator=(ActionDigraph&&);

    ~ActionDigraph();

    //! Construct a random digraph from number of nodes and out-degree.
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
    //! the out-degree of the digraph.
    static ActionDigraph random(T            number_of_nodes,
                                T            out_degree,
                                std::mt19937 mt
                                = std::mt19937(std::random_device()())) {
      std::uniform_int_distribution<T> dist(0, number_of_nodes - 1);
      ActionDigraph<T>                 g(number_of_nodes, out_degree);
      LIBSEMIGROUPS_ASSERT(g._dynamic_array_2.number_of_rows()
                           == number_of_nodes);
      LIBSEMIGROUPS_ASSERT(g._dynamic_array_2.number_of_cols() == out_degree);
      std::generate(g._dynamic_array_2.begin(),
                    g._dynamic_array_2.end(),
                    [&dist, &mt]() { return dist(mt); });
      return g;
    }

    //! Construct a random digraph from number of nodes, edges, and out-degree.
    //!
    //! \param number_of_nodes the number of nodes
    //! \param out_degree the out-degree of every node
    //! \param number_of_edges the out-degree of every node
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
    //! out-degree of the digraph.
    static ActionDigraph random(T            number_of_nodes,
                                T            out_degree,
                                T            number_of_edges,
                                std::mt19937 mt
                                = std::mt19937(std::random_device()())) {
      if (number_of_nodes < 2) {
        LIBSEMIGROUPS_EXCEPTION("the 1st parameter `number_of_nodes` must be "
                                "at least 2, found %llu",
                                static_cast<uint64_t>(number_of_nodes));
      } else if (out_degree < 2) {
        LIBSEMIGROUPS_EXCEPTION("the 2nd parameter `number_of_edges` must be "
                                "at least 2, found %llu",
                                static_cast<uint64_t>(out_degree));
      } else if (number_of_edges > number_of_nodes * out_degree) {
        LIBSEMIGROUPS_EXCEPTION(
            "the 3rd parameter `number_of_edges` must be at "
            "most %llu, but found %llu",
            static_cast<uint64_t>(number_of_nodes * out_degree),
            static_cast<uint64_t>(number_of_edges));
      }
      std::uniform_int_distribution<T> source(0, number_of_nodes - 1);
      std::uniform_int_distribution<T> target(0, number_of_nodes - 1);
      std::uniform_int_distribution<T> label(0, out_degree - 1);

      ActionDigraph<T> g(number_of_nodes, out_degree);
      size_t           edges_to_add = number_of_edges;
      size_t           old_nr_edges = 0;
      do {
        for (size_t i = 0; i < edges_to_add; ++i) {
          g._dynamic_array_2.set(source(mt), label(mt), target(mt));
        }
        size_t new_nr_edges = g.number_of_edges();
        edges_to_add -= (new_nr_edges - old_nr_edges);
        old_nr_edges = new_nr_edges;
      } while (edges_to_add != 0);
      return g;
    }

    //! Construct a random acyclic digraph from number of nodes, edges, and
    //! out-degree.
    //!
    //! \param number_of_nodes the number of nodes
    //! \param out_degree the out-degree of every node
    //! \param number_of_edges the out-degree of every node
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
    //! out-degree of the digraph.
    static ActionDigraph
    random_acyclic(T            number_of_nodes,
                   T            out_degree,
                   T            number_of_edges,
                   std::mt19937 mt = std::mt19937(std::random_device()())) {
      if (number_of_nodes < 2) {
        LIBSEMIGROUPS_EXCEPTION("the 1st parameter `number_of_nodes` must be "
                                "at least 2, found %llu",
                                static_cast<uint64_t>(number_of_nodes));
      } else if (out_degree < 2) {
        LIBSEMIGROUPS_EXCEPTION("the 2nd parameter `number_of_edges` must be "
                                "at least 2, found %llu",
                                static_cast<uint64_t>(out_degree));
      }
      size_t max_edges = std::min(number_of_nodes * out_degree,
                                  number_of_nodes * (number_of_nodes - 1) / 2);

      if (number_of_edges > max_edges) {
        LIBSEMIGROUPS_EXCEPTION(
            "the 3rd parameter `number_of_edges` must be at most %llu, but "
            "found %llu",
            static_cast<uint64_t>(max_edges),
            static_cast<uint64_t>(number_of_edges));
      }
      std::uniform_int_distribution<T> source(0, number_of_nodes - 1);
      std::uniform_int_distribution<T> label(0, out_degree - 1);

      ActionDigraph<T> g(number_of_nodes, out_degree);
      size_t           edges_to_add = number_of_edges;
      size_t           old_nr_edges = 0;
      do {
        for (size_t i = 0; i < edges_to_add; ++i) {
          auto v = source(mt);
          if (v != number_of_nodes - 1) {
            g._dynamic_array_2.set(v,
                                   label(mt),
                                   std::uniform_int_distribution<T>(
                                       v + 1, number_of_nodes - 1)(mt));
          }
        }
        size_t new_nr_edges = g.number_of_edges();
        edges_to_add -= (new_nr_edges - old_nr_edges);
        old_nr_edges = new_nr_edges;
      } while (edges_to_add != 0);
      return g;
    }

    ////////////////////////////////////////////////////////////////////////
    // ActionDigraph - modifiers - public
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
    void inline add_nodes(size_t nr) {
      if (nr > _dynamic_array_2.number_of_rows() - _nr_nodes) {
        _dynamic_array_2.add_rows(
            nr - (_dynamic_array_2.number_of_rows() - _nr_nodes));
      }
      _nr_nodes += nr;
      reset();
    }

    //! Restrict the digraph to its first \p n nodes.
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
    //! corrupted digraph if there are any edges from the nodes \f$0, \ldots, n
    //! - 1\f$ to nodes larger than \f$n - 1\f$.
    // TODO(later) this is the nc version do a non-nc version also
    // Means restrict the number of nodes to the first 0, ... ,n - 1.
    // TODO(v3) rename to shrink_nodes_to
    void inline restrict(size_t n) {
      _nr_nodes = n;
      _dynamic_array_2.shrink_rows_to(n);
    }

    // Only valid if no edges incident to nodes in [first, last) point outside
    // [first, last)
    void inline induced_subdigraph(node_type first, node_type last) {
      _nr_nodes = last - first;
      _dynamic_array_2.shrink_rows_to(first, last);
      if (first != 0) {
        std::for_each(_dynamic_array_2.begin(),
                      _dynamic_array_2.end(),
                      [&first](node_type& x) { x -= first; });
      }
    }

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
    //! degree of the digraph.
    //!
    //! \iterator_validity
    //! \iterator_invalid
    // Not noexcept because DynamicArray2::add_cols isn't.
    void inline add_to_out_degree(size_t nr) {
      if (nr > _dynamic_array_2.number_of_cols() - _degree) {
        _dynamic_array_2.add_cols(
            nr - (_dynamic_array_2.number_of_cols() - _degree));
      }
      _degree += nr;
      reset();
    }

    //! Add an edge from one node to another with a given label.
    //!
    //! If \p i and \p j are nodes in \c this, and \p lbl is in the range `[0,
    //! out_degree())`, then this function adds an edge from \p i to \p j
    //! labelled \p lbl.
    //!
    //! \param i the source node
    //! \param j the range node
    //! \param lbl the label of the edge from \p i to \p j
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
    void inline add_edge(node_type i, node_type j, label_type lbl) {
      action_digraph_helper::validate_node(*this, i);
      action_digraph_helper::validate_node(*this, j);
      action_digraph_helper::validate_label(*this, lbl);
      add_edge_nc(i, j, lbl);
    }

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
    //! No checks whatsoever on the validity of the arguments are peformed.
    void inline add_edge_nc(node_type i, node_type j, label_type lbl) {
      _dynamic_array_2.set(i, lbl, j);
      reset();
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
    //! No checks whatsoever on the validity of the arguments are peformed.
    void inline remove_edge_nc(node_type i, label_type lbl) {
      _dynamic_array_2.set(i, lbl, UNDEFINED);
      reset();
    }

    //! Ensures that the digraph has capacity for a given number of nodes, and
    //! out-degree.
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
    void reserve(T m, T n) const {
      _dynamic_array_2.add_cols(n - _dynamic_array_2.number_of_cols());
      // What if add_cols throws, what guarantee can we offer then?
      _dynamic_array_2.add_rows(m - _dynamic_array_2.number_of_rows());
    }

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
    //! No checks whatsoever on the validity of the arguments are peformed.
    // swap u - a - > u' and v - a -> v'
    void swap_edges_nc(node_type u, node_type v, label_type a) {
      reset();
      _dynamic_array_2.swap(u, a, v, a);
    }

    //! Check if two action digraphs are equal.
    //!
    //! \param that the action digraph for comparisonb
    //!
    //! \returns
    //! A `bool`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! At worst \f$O(nm)\f$ where \f$n\f$ is the number of nodes and \f$m\f$
    //! is the out-degree of the digraph.
    // swap u - a - > u' and v - a -> v'
    bool operator==(ActionDigraph const& that) const {
      return _dynamic_array_2 == that._dynamic_array_2;
    }

    ////////////////////////////////////////////////////////////////////////
    // ActionDigraph - nodes, neighbors, etc - public
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
    node_type inline neighbor(node_type v, label_type lbl) const {
      action_digraph_helper::validate_node(*this, v);
      action_digraph_helper::validate_label(*this, lbl);
      return _dynamic_array_2.get(v, lbl);
    }

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
    node_type inline unsafe_neighbor(node_type v, label_type lbl) const {
      return _dynamic_array_2.get(v, lbl);
    }

    //! Get the next neighbor of a node that doesn't equal
    //! libsemigroups::UNDEFINED.
    //!
    //! If `neighbor(v, i)` equals libsemigroups::UNDEFINED for every value in
    //! the range \f$[i, n)\f$, where \f$n\f$ is the return value of
    //! out_degree() then \c x.first and \c x.second equal
    //! libsemigroups::UNDEFINED.
    //!
    //! \param v the node
    //! \param i the label
    //!
    //! \returns
    //! Returns a [std::pair](https://en.cppreference.com/w/cpp/utility/pair)
    //! \c x where:
    //! 1. \c x.first is adjacent to \p v via an edge labelled
    //!    \c x.second;
    //! 2. \c x.second is the minimum value in the range \f$[i, n)\f$ such that
    //!    `neighbor(v, x.second)` is not equal to libsemigroups::UNDEFINED
    //!    where \f$n\f$ is the return value of out_degree(); and
    //!
    //! \complexity
    //! At worst \f$O(n)\f$ where \f$n\f$ equals out_degree().
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \sa next_neighbor.
    //!
    //! \warning This function is unsafe because it is not verified that the
    //! parameter \p v represents a node of \c this.
    // Not noexcept because DynamicArray2::get is not
    std::pair<node_type, label_type> inline unsafe_next_neighbor(
        node_type  v,
        label_type i) const {
      while (i < out_degree()) {
        node_type u = _dynamic_array_2.get(v, i);
        if (u != UNDEFINED) {
          return std::make_pair(u, i);
        }
        i++;
      }
      return std::make_pair(UNDEFINED, UNDEFINED);
    }

    //! Get the next neighbor of a node that doesn't equal
    //! libsemigroups::UNDEFINED.
    //!
    //! If `neighbor(v, i)` equals libsemigroups::UNDEFINED for every value in
    //! the range \f$[i, n)\f$, where \f$n\f$ is the return value of
    //! out_degree() then \c x.first and \c x.second equal
    //! libsemigroups::UNDEFINED.
    //!
    //! \param v the node
    //! \param i the label
    //!
    //! \returns
    //! Returns a [std::pair](https://en.cppreference.com/w/cpp/utility/pair)
    //! \c x where:
    //! 1. \c x.first is adjacent to \p v via an edge labelled
    //!    \c x.second;
    //! 2. \c x.second is the minimum value in the range \f$[i, n)\f$ such that
    //!    `neighbor(v, x.second)` is not equal to libsemigroups::UNDEFINED
    //!    where \f$n\f$ is the return value of out_degree(); and
    //!
    //! \complexity
    //! At worst \f$O(n)\f$ where \f$n\f$ equals out_degree().
    //!
    //! \throws LibsemigroupsException if \p v does not represent a node in \c
    //! this.
    //!
    //! \sa unsafe_next_neighbor.
    // Not noexcept because unsafe_next_neighbor is not
    std::pair<node_type, label_type> inline next_neighbor(node_type  v,
                                                          label_type i) const {
      action_digraph_helper::validate_node(*this, v);
      return unsafe_next_neighbor(v, i);
    }

    //! Returns the number of nodes.
    //!
    //! \returns
    //! The number of nodes, a value of type \c T.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    //!
    //! \par Parameters
    //! (None)
    T inline number_of_nodes() const noexcept {
      return _nr_nodes;
    }

#ifndef DOXYGEN_SHOULD_SKIP_THIS
    ActionDigraph& number_of_active_nodes(size_type val) {
      _num_active_nodes = val;
      return *this;
    }

    size_type inline number_of_active_nodes() const noexcept {
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
    size_t number_of_edges() const {
      return _dynamic_array_2.number_of_rows()
                 * _dynamic_array_2.number_of_cols()
             - std::count(
                 _dynamic_array_2.cbegin(), _dynamic_array_2.cend(), UNDEFINED);
    }

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
    size_t number_of_edges(node_type n) const {
      action_digraph_helper::validate_node(*this, n);
      return out_degree()
             - std::count(_dynamic_array_2.cbegin_row(n),
                          _dynamic_array_2.cend_row(n),
                          UNDEFINED);
    }

    //! Returns the out-degree.
    //!
    //! \returns
    //! The number of out-edges of every node, a value of type \c T.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    //!
    //! \par Parameters
    //! (None)
    T out_degree() const noexcept {
      return _degree;
    }

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
    bool validate() const noexcept {
      return number_of_edges() == number_of_nodes() * out_degree();
    }

    //! Returns a random access iterator pointing at the first node of the
    //! digraph.
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
      return IntegralRange<T>(0, number_of_nodes()).cbegin();
    }

    //! Returns a random access iterator pointing at the last node of the
    //! digraph.
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
      return IntegralRange<T>(0, number_of_nodes()).crbegin();
    }

    //! Returns a random access iterator pointing one-past-the-first node of
    //! the digraph.
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
      return IntegralRange<T>(0, number_of_nodes()).crend();
    }

    //! Returns a random access iterator pointing one-past-the-last node of the
    //! digraph.
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
      return IntegralRange<T>(0, number_of_nodes()).cend();
    }

    //! Returns a random access iterator pointing at the first neighbor of a
    //! node.
    //!
    //! \param i a node in the digraph.
    //!
    //! \returns
    //! A \ref const_iterator_edges.
    //!
    //! \throws LibsemigroupsException if \p i is not valid.
    //!
    //! \complexity
    //! Constant.
    // Not noexcept because validate_node isn't
    const_iterator_edges cbegin_edges(node_type i) const {
      action_digraph_helper::validate_node(*this, i);
      return cbegin_edges_nc(i);
    }

    //! Returns a random access iterator pointing at the first neighbor of a
    //! node.
    //!
    //! \param i a node in the digraph.
    //!
    //! \returns
    //! A \ref const_iterator_edges.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    //!
    //! \warning
    //! No checks whatsoever on the validity of the arguments are peformed.
    //!
    //! \sa
    //! \ref cbegin_edges.
    const_iterator_edges cbegin_edges_nc(node_type i) const noexcept {
      return _dynamic_array_2.cbegin_row(i);
    }

    //! Returns a random access iterator pointing one-past-the-last neighbor of
    //! a node.
    //!
    //! \param i a node in the digraph.
    //!
    //! \returns
    //! A \ref const_iterator_edges.
    //!
    //! \throws LibsemigroupsException if \p i is not valid.
    //!
    //! \complexity
    //! Constant.
    // Not noexcept because validate_node isn't
    const_iterator_edges cend_edges(node_type i) const {
      action_digraph_helper::validate_node(*this, i);
      return cend_edges_nc(i);
    }

    //! Returns a random access iterator pointing one-past-the-last neighbor of
    //! a node.
    //!
    //! \param i a node in the digraph.
    //!
    //! \returns
    //! A \ref const_iterator_edges.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    //!
    //! \warning
    //! No checks whatsoever on the validity of the arguments are peformed.
    //!
    //! \sa
    //! \ref cend_edges.
    const_iterator_edges cend_edges_nc(node_type i) const noexcept {
      return _dynamic_array_2.cend_row(i);
    }

    ////////////////////////////////////////////////////////////////////////
    // ActionDigraph - strongly connected components - public
    ////////////////////////////////////////////////////////////////////////

    //! Returns the id-number of the strongly connected component of a node.
    //!
    //! \param nd the node.
    //!
    //! \returns
    //! The index of the node \p nd, a value of type scc_index_type.
    //!
    //! \throws LibsemigroupsException if \p nd is not valid.
    //!
    //! \complexity
    //! At most \f$O(mn)\f$ where \c m is number_of_nodes() and \c n is
    //! out_degree().
    // Not noexcept because validate_node isn't
    scc_index_type scc_id(node_type nd) const {
      action_digraph_helper::validate_node(*this, nd);
      gabow_scc();
      LIBSEMIGROUPS_ASSERT(nd < _scc._id.size());
      return _scc._id[nd];
    }

    //! Returns the number of strongly connected components.
    //!
    //! \returns
    //! A `size_t`.
    //!
    //! \throws LibsemigroupsException if it is not the case that every node
    //! has exactly out_degree() out-neighbors. In other words, if
    //! neighbor() is libsemigroups::UNDEFINED for any node \c nd and
    //! any label \c lbl.
    //! \basic_guarantee
    //!
    //! \complexity
    //! At most \f$O(mn)\f$ where \c m is number_of_nodes() and \c n is
    //! out_degree().
    //!
    //! \par Parameters
    //! (None)
    // Not noexcept because gabow_scc isn't
    size_t number_of_scc() const {
      gabow_scc();
      return _scc._comps.size();
    }

    //! Returns the root of a strongly connected components containing a given
    //! node.
    //!
    //! \param nd a node.
    //!
    //! \returns
    //! The root of the scc containing the node \p nd, a value of
    //! \ref node_type.
    //!
    //! \throws LibsemigroupsException if it is not the case that every node
    //! has exactly out_degree() out-neighbors. In other words, if
    //! neighbor() is libsemigroups::UNDEFINED for any node \c nd and
    //! any label \c lbl.
    //! \basic_guarantee
    //!
    //! \complexity
    //! At most \f$O(mn)\f$ where \c m is number_of_nodes() and \c n is
    //! out_degree().
    // Not noexcept because scc_id isn't
    node_type root_of_scc(node_type nd) const {
      // nd is validated in scc_id
      return *cbegin_scc(scc_id(nd));
    }

    //! Returns an iterator pointing to the vector of nodes in the first scc.
    //!
    //! \returns
    //! A \ref const_iterator_sccs.
    //!
    //! \throws LibsemigroupsException if it is not the case that every node
    //! has exactly out_degree() out-neighbors. In other words, if
    //! neighbor() is libsemigroups::UNDEFINED for any node \c nd and
    //! any label \c lbl.
    //! \basic_guarantee
    //!
    //! \complexity
    //! At most \f$O(mn)\f$ where \c m is number_of_nodes() and \c n is
    //! out_degree().
    //!
    //! \par Parameters
    //! (None)
    // Not noexcept because gabow_scc isn't
    const_iterator_sccs cbegin_sccs() const {
      gabow_scc();
      return _scc._comps.cbegin();
    }

    //! Returns an iterator pointing one past the last vector of nodes in the
    //! final scc.
    //!
    //! \returns
    //! A \ref const_iterator_sccs.
    //!
    //! \throws LibsemigroupsException if it is not the case that every node
    //! has exactly out_degree() out-neighbors. In other words, if
    //! neighbor() is libsemigroups::UNDEFINED for any node \c nd and
    //! any label \c lbl.
    //! \basic_guarantee
    //!
    //! \complexity
    //! At most \f$O(mn)\f$ where \c m is number_of_nodes() and \c n is
    //! out_degree().
    //!
    //! \par Parameters
    //! (None)
    // Not noexcept because gabow_scc isn't
    const_iterator_sccs cend_sccs() const {
      gabow_scc();
      return _scc._comps.cend();
    }

    //! Returns an iterator pointing to the first node in the scc with
    //! the specified id-number.
    //!
    //! \param i the id-number of the scc.
    //!
    //! \returns
    //! A \ref const_iterator_scc.
    //!
    //! \throws LibsemigroupsException if it is not the case that every node
    //! has exactly out_degree() out-neighbors. In other words, if
    //! neighbor() is libsemigroups::UNDEFINED for any node \c nd and
    //! any label \c lbl.
    //!
    //! \throws LibsemigroupsException if \p i is not in the range \c 0 to \c
    //! number_of_scc() - 1.
    //!
    //! \complexity
    //! At most \f$O(mn)\f$ where \c m is number_of_nodes() and \c n is
    //! out_degree().
    //!
    //! \note
    //! \basic_guarantee
    //!
    // Not noexcept because gabow_scc isn't
    const_iterator_scc cbegin_scc(scc_index_type i) const {
      gabow_scc();
      validate_scc_index(i);
      return _scc._comps[i].cbegin();
    }

    //! Returns an iterator pointing one past the last node in the scc with
    //! the specified id-number.
    //!
    //! \param i the id-number of the scc.
    //!
    //! \returns
    //! A \ref const_iterator_scc.
    //!
    //! \throws LibsemigroupsException if it is not the case that every node
    //! has exactly out_degree() out-neighbors. In other words, if
    //! neighbor() is libsemigroups::UNDEFINED for any node \c nd and
    //! any label \c lbl.
    //!
    //! \throws LibsemigroupsException if \p i is not in the range \c 0 to \c
    //! number_of_scc() - 1.
    //!
    //! \complexity
    //! At most \f$O(mn)\f$ where \c m is number_of_nodes() and \c n is
    //! out_degree().
    //!
    //! \note
    //! \basic_guarantee
    // Not noexcept because gabow_scc isn't
    const_iterator_scc cend_scc(scc_index_type i) const {
      gabow_scc();
      validate_scc_index(i);
      return _scc._comps[i].cend();
    }

    //! Returns an iterator pointing to the root of the first scc.
    //!
    //! \returns
    //! A \ref const_iterator_scc_roots.
    //!
    //! \throws LibsemigroupsException if it is not the case that every node
    //! has exactly out_degree() out-neighbors. In other words, if
    //! neighbor() is libsemigroups::UNDEFINED for any node \c nd and
    //! any label \c lbl. \basic_guarantee
    //!
    //! \complexity
    //! At most \f$O(mn)\f$ where \c m is number_of_nodes() and \c n is
    //! out_degree().
    //!
    //! \par Parameters
    //! (None)
    // Not noexcept because cbegin_sccs isn't
    const_iterator_scc_roots cbegin_scc_roots() const {
      return const_iterator_scc_roots(cbegin_sccs());
    }

    //! Returns an iterator pointing one past the root of the last scc.
    //!
    //! \returns
    //! A \ref const_iterator_scc_roots.
    //!
    //! \throws LibsemigroupsException if it is not the case that every node
    //! has exactly out_degree() out-neighbors. In other words, if
    //! neighbor() is libsemigroups::UNDEFINED for any node \c nd and
    //! any label \c lbl. \basic_guarantee
    //!
    //! \complexity
    //! At most \f$O(mn)\f$ where \c m is number_of_nodes() and \c n is
    //! out_degree().
    //!
    //! \par Parameters
    //! (None)
    // Not noexcept because cend_sccs isn't
    const_iterator_scc_roots cend_scc_roots() const {
      return const_iterator_scc_roots(cend_sccs());
    }

    ////////////////////////////////////////////////////////////////////////
    // ActionDigraph - spanning forests - public
    ////////////////////////////////////////////////////////////////////////

    //! Returns a spanning forest of the strongly connected components.
    //!
    //! Returns a Forest comprised of spanning trees for each
    //! scc of \c this, rooted on the minimum node of that component, with
    //! edges oriented away from the root.
    //!
    //! \returns
    //! A const reference to a Forest.
    //!
    //! \throws LibsemigroupsException if it is not the case that every node
    //! has exactly out_degree() out-neighbors. In other words, if
    //! neighbor() is libsemigroups::UNDEFINED for any node \c nd and
    //! any label \c lbl. \basic_guarantee
    //!
    //! \complexity
    //! At most \f$O(mn)\f$ where \c m is number_of_nodes() and \c n is
    //! out_degree().
    //!
    //! \par Parameters
    //! (None)
    Forest const& spanning_forest() const {
      if (!_scc_forest._defined) {
        // Validity checked in gabow_scc
        gabow_scc();

        std::vector<bool> seen(number_of_nodes(), false);
        std::queue<T>     queue;

        _scc_forest._forest.clear();
        _scc_forest._forest.add_nodes(number_of_nodes());

        for (size_t i = 0; i < number_of_scc(); ++i) {
          queue.push(_scc._comps[i][0]);
          seen[_scc._comps[i][0]] = true;
          do {
            size_t x = queue.front();
            for (size_t j = 0; j < _degree; ++j) {
              size_t y = _dynamic_array_2.get(x, j);
              if (!seen[y] && _scc._id[y] == _scc._id[x]) {
                _scc_forest._forest.set(y, x, j);
                queue.push(y);
                seen[y] = true;
              }
            }
            queue.pop();
          } while (!queue.empty());
        }
        _scc_forest._defined = true;
      }
      return _scc_forest._forest;
    }

    //! Returns a reverse spanning forest of the strongly connected components.
    //!
    //! Returns a Forest comprised of spanning trees for each
    //! scc of \c this, rooted on the minimum node of that component, with
    //! edges oriented towards the root.
    //!
    //! \returns
    //! A const reference to a Forest.
    //!
    //! \throws LibsemigroupsException if it is not the case that every node
    //! has exactly out_degree() out-neighbors. In other words, if
    //! neighbor() is libsemigroups::UNDEFINED for any node \c nd and
    //! any label \c lbl. \basic_guarantee
    //!
    //! \complexity
    //! At most \f$O(mn)\f$ where \c m is number_of_nodes() and \c n is
    //! out_degree().
    //!
    //! \par Parameters
    //! (None)
    Forest const& reverse_spanning_forest() const {
      if (!_scc_back_forest._defined) {
        // Validity checked in gabow_scc
        gabow_scc();

        _scc_back_forest._forest.clear();
        _scc_back_forest._forest.add_nodes(number_of_nodes());

        std::vector<std::vector<T>> reverse_edges(number_of_nodes(),
                                                  std::vector<T>());
        std::vector<std::vector<T>> reverse_labels(number_of_nodes(),
                                                   std::vector<T>());

        for (size_t i = 0; i < number_of_nodes(); ++i) {
          size_t const scc_id_i = scc_id(i);
          for (size_t j = 0; j < out_degree(); ++j) {
            size_t const k = _dynamic_array_2.get(i, j);
            if (scc_id(k) == scc_id_i) {
              reverse_edges[k].push_back(i);
              reverse_labels[k].push_back(j);
            }
          }
        }
        std::queue<size_t> queue;
        std::vector<bool>  seen(number_of_nodes(), false);

        for (size_t i = 0; i < number_of_scc(); ++i) {
          LIBSEMIGROUPS_ASSERT(queue.empty());
          queue.push(_scc._comps[i][0]);
          seen[_scc._comps[i][0]] = true;
          while (!queue.empty()) {
            size_t x = queue.front();
            for (size_t j = 0; j < reverse_edges[x].size(); ++j) {
              size_t y = reverse_edges[x][j];
              if (!seen[y]) {
                queue.push(y);
                seen[y] = true;
                _scc_back_forest._forest.set(y, x, reverse_labels[x][j]);
              }
            }
            queue.pop();
          }
        }
        _scc_back_forest._defined = true;
      }
      return _scc_back_forest._forest;
    }

    ////////////////////////////////////////////////////////////////////////
    // ActionDigraph - paths - public
    ////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////
    // PANILO = Path And (terminal) Node In Lex Order
    ////////////////////////////////////////////////////////////////////////

    //! Return type of \ref cbegin_panilo and \ref cend_panilo.
    class const_panilo_iterator final {
     public:
      //! No doc
      using value_type = std::pair<word_type, node_type>;
      //! No doc
      using size_type = typename std::vector<value_type>::size_type;
      //! No doc
      using difference_type = typename std::vector<value_type>::difference_type;
      //! No doc
      using const_pointer = typename std::vector<value_type>::const_pointer;
      //! No doc
      using pointer = typename std::vector<value_type>::pointer;
      //! No doc
      using const_reference = typename std::vector<value_type>::const_reference;
      //! No doc
      using reference = const_reference;
      //! No doc
      using iterator_category = std::forward_iterator_tag;

      // None of the constructors are noexcept because the corresponding
      // constructors for std::vector aren't (until C++17).
      //! No doc
      const_panilo_iterator();
      //! No doc
      const_panilo_iterator(const_panilo_iterator const&);
      //! No doc
      const_panilo_iterator(const_panilo_iterator&&);
      //! No doc
      const_panilo_iterator& operator=(const_panilo_iterator const&);
      //! No doc
      const_panilo_iterator& operator=(const_panilo_iterator&&);
      //! No doc
      ~const_panilo_iterator();

      //! No doc
      const_panilo_iterator(ActionDigraph const* ptr,
                            node_type const      source,
                            size_type const      min,
                            size_type const      max)
          : _edges({}, source),
            _digraph(ptr),
            _edge(UNDEFINED),
            _min(min),
            _max(max),
            _nodes() {
        if (_min < _max) {
          _nodes.push_back(source);
          if (_min != 0) {
            ++(*this);
          }
        }
      }

      //! No doc
      // noexcept because comparison of std::vector<node_type>'s is noexcept
      // because comparision of node_type's is noexcept
      bool operator==(const_panilo_iterator const& that) const noexcept {
        return _nodes == that._nodes;
      }

      //! No doc
      // noexcept because operator== is noexcept
      bool operator!=(const_panilo_iterator const& that) const noexcept {
        return !(this->operator==(that));
      }

      //! No doc
      const_reference operator*() const noexcept {
        return _edges;
      }

      //! No doc
      const_pointer operator->() const noexcept {
        return &_edges;
      }

      //! No doc
      // prefix - not noexcept because std::vector::push_back isn't
      const_panilo_iterator const& operator++() {
        if (_nodes.empty()) {
          return *this;
        } else if (_edge == UNDEFINED) {
          // first call
          _edge = 0;
        }

        do {
          node_type next;
          std::tie(next, _edge)
              = _digraph->unsafe_next_neighbor(_nodes.back(), _edge);
          if (next != UNDEFINED && _edges.first.size() < _max - 1) {
            _nodes.push_back(next);
            _edges.first.push_back(_edge);
            _edge = 0;
            if (_edges.first.size() >= _min) {
              _edges.second = next;
              return *this;
            }
          } else {
            _nodes.pop_back();
            if (!_edges.first.empty()) {
              _edge = _edges.first.back() + 1;
              _edges.first.pop_back();
            }
          }
        } while (!_nodes.empty());

        return *this;
      }

      //! No doc
      // postfix - not noexcept because the prefix ++ isn't
      const_panilo_iterator operator++(int) {
        const_panilo_iterator copy(*this);
        ++(*this);
        return copy;
      }

      //! No doc
      void swap(const_panilo_iterator& that) noexcept {
        std::swap(_edges, that._edges);
        std::swap(_digraph, that._digraph);
        std::swap(_edge, that._edge);
        std::swap(_min, that._min);
        std::swap(_max, that._max);
        std::swap(_nodes, that._nodes);
      }

      //! No doc
      ActionDigraph const& digraph() const noexcept {
        return *_digraph;
      }

     private:
      value_type             _edges;
      ActionDigraph const*   _digraph;
      label_type             _edge;
      size_t                 _min;
      size_t                 _max;
      std::vector<node_type> _nodes;
    };  // const_panilo_iterator

    // Assert that the forward iterator requirements are met
    static_assert(std::is_default_constructible<const_panilo_iterator>::value,
                  "forward iterator requires default-constructible");
    static_assert(std::is_copy_constructible<const_panilo_iterator>::value,
                  "forward iterator requires copy-constructible");
    static_assert(std::is_copy_assignable<const_panilo_iterator>::value,
                  "forward iterator requires copy-assignable");
    static_assert(std::is_destructible<const_panilo_iterator>::value,
                  "forward iterator requires destructible");

    //! Returns an iterator for PANILO (Path And Node In Lex Order).
    //!
    //! Returns a forward iterator pointing to a pair consisting of the edge
    //! labels of the first path (in lexicographical order) starting at
    //! \p source with length in the range \f$[min, max)\f$ and the last node
    //! of that path.
    //!
    //! If incremented, the iterator will point to the next least edge
    //! labelling of a path (in lexicographical order), and its last node, with
    //! length in the range \f$[min, max)\f$.  Iterators of the type returned
    //! by this function are equal whenever they point to equal objects.
    //!
    //! \param source the source node
    //! \param min the minimum length of a path to enumerate (defaults to \c 0)
    //! \param max the maximum length of a path to enumerate (defaults to
    //!        libsemigroups::POSITIVE_INFINITY).
    //!
    //! \returns
    //! An iterator \c it of type \c const_panilo_iterator pointing to a
    //! [std::pair](https://en.cppreference.com/w/cpp/utility/pair)
    //! where:
    //! * \c it->first is a libsemigroups::word_type consisting of the edge
    //! labels of the first path (in lexicographical order) from \p source of
    //! length in the range \f$[min, max)\f$; and
    //! * \c it->second is the last node on the path from \p source labelled by
    //! \c it->first, a value of \ref node_type.
    //!
    //! \throws LibsemigroupsException if \p source is not a node in the
    //! digraph.
    //!
    //! \warning
    //! Copying iterators of this type is expensive.  As a consequence, prefix
    //! incrementing \c ++it the returned  iterator \c it significantly cheaper
    //! than postfix incrementing \c it++.
    //!
    //! \warning
    //! If the action digraph represented by \c this contains a cycle that is
    //! reachable from \p source, then there are infinitely many paths starting
    //! at \p source, and so \p max should be chosen with some care.
    //!
    //! \sa
    //! cend_panilo
    // not noexcept because constructors of const_panilo_iterator aren't
    const_panilo_iterator cbegin_panilo(node_type source,
                                        size_t    min = 0,
                                        size_t max = POSITIVE_INFINITY) const {
      action_digraph_helper::validate_node(*this, source);
      return const_panilo_iterator(this, source, min, max);
    }

    //! Returns an iterator for PANILO (Path And Node In Lex Order).
    //!
    //! Returns a forward iterator pointing to one after the last path from any
    //! node in the digraph.
    //!
    //! The iterator returned by this function may still dereferencable and
    //! incrementable, but may not point to a path in the correct range.
    //!
    //! \sa cbegin_panilo
    // not noexcept because constructors of const_panilo_iterator aren't
    const_panilo_iterator cend_panilo() const {
      return const_panilo_iterator(this, 0, 0, 0);
    }

    ////////////////////////////////////////////////////////////////////////
    // PANISLO - Path And Node In Short Lex Order
    ////////////////////////////////////////////////////////////////////////

    // Note that while the complexity of this is bad, it repeatedly does depth
    // first searches, and so will examine every node and edge of the graph
    // multiple times (if u -a-> v belongs to a path of length 10, then it will
    // be traversed 10 times). But the performance of this iterator is
    // dominated by memory allocation (when creating
    // iterators, at least), and so this doesn't seem that bad.
    //! Return type of cbegin_panislo and cend_panislo.
    class const_panislo_iterator final {
     public:
      //! No doc
      using value_type = std::pair<word_type, node_type>;
      //! No doc
      using size_type = typename std::vector<value_type>::size_type;
      //! No doc
      using difference_type = typename std::vector<value_type>::difference_type;
      //! No doc
      using const_pointer = typename std::vector<value_type>::const_pointer;
      //! No doc
      using pointer = typename std::vector<value_type>::pointer;
      //! No doc
      using const_reference = typename std::vector<value_type>::const_reference;
      //! No doc
      using reference = const_reference;
      //! No doc
      using iterator_category = std::forward_iterator_tag;

      // None of the constructors are noexcept because the corresponding
      // constructors for const_panilo_iterator are not.
      //! No doc
      const_panislo_iterator() = default;
      //! No doc
      const_panislo_iterator(const_panislo_iterator const&) = default;
      //! No doc
      const_panislo_iterator(const_panislo_iterator&&) = default;
      //! No doc
      const_panislo_iterator& operator=(const_panislo_iterator const&)
          = default;
      //! No doc
      const_panislo_iterator& operator=(const_panislo_iterator&&) = default;
      //! No doc
      ~const_panislo_iterator() = default;

      //! No doc
      const_panislo_iterator(ActionDigraph const* ptr,
                             node_type const      source,
                             size_type const      min,
                             size_type const      max)
          : _length(min >= max ? UNDEFINED : min),
            _it(),
            _max(max),
            _source(source) {
        if (_length != UNDEFINED) {
          _it = ptr->cbegin_panilo(source, _length, _length + 1);
        } else {
          _it = ptr->cend_panilo();
        }
      }

      //! No doc
      // noexcept because comparison of const_panilo_iterator is noexcept
      bool operator==(const_panislo_iterator const& that) const noexcept {
        return _length == that._length && _it == that._it;
      }

      //! No doc
      // noexcept because operator== is noexcept
      bool operator!=(const_panislo_iterator const& that) const noexcept {
        return !(this->operator==(that));
      }

      //! No doc
      const_reference operator*() const noexcept {
        return *_it;
      }

      //! No doc
      const_pointer operator->() const noexcept {
        return &(*_it);
      }

      //! No doc
      // prefix - not noexcept because cbegin_panilo isn't
      const_panislo_iterator const& operator++() {
        ++_it;
        if (_it == _it.digraph().cend_panilo()) {
          if (_length < _max - 1) {
            ++_length;
            _it = _it.digraph().cbegin_panilo(_source, _length, _length + 1);
            if (_it == _it.digraph().cend_panilo()) {
              _length = UNDEFINED;
            }
          } else {
            _length = UNDEFINED;
          }
        }
        return *this;
      }

      //! No doc
      // postfix - not noexcept because copy constructor isn't
      const_panislo_iterator operator++(int) {
        const_panislo_iterator copy(*this);
        ++(*this);
        return copy;
      }

      //! No doc
      void swap(const_panislo_iterator& that) noexcept {
        std::swap(_length, that._length);
        std::swap(_it, that._it);
        std::swap(_max, that._max);
        std::swap(_source, that._source);
      }

     private:
      size_type             _length;
      const_panilo_iterator _it;
      size_type             _max;
      node_type             _source;
    };

    // Assert that the forward iterator requirements are met
    static_assert(std::is_default_constructible<const_panislo_iterator>::value,
                  "forward iterator requires default-constructible");
    static_assert(std::is_copy_constructible<const_panislo_iterator>::value,
                  "forward iterator requires copy-constructible");
    static_assert(std::is_copy_assignable<const_panislo_iterator>::value,
                  "forward iterator requires copy-assignable");
    static_assert(std::is_destructible<const_panislo_iterator>::value,
                  "forward iterator requires destructible");

    //! Returns an iterator for PANISLO (Path And Node In Short Lex Order).
    //!
    //! Returns a forward iterator pointing to a pair consisting of the edge
    //! labels of the first path (in short-lex order) starting at \p source
    //! with length in the range \f$[min, max)\f$ and the last node of that
    //! path.
    //!
    //! If incremented, the iterator will point to the next least edge
    //! labelling of a path (in short-lex order), and its last node, with
    //! length in the range \f$[min, max)\f$.  Iterators of the type returned
    //! by this function are equal whenever they point to equal objects.
    //!
    //! \param source the source node
    //! \param min the minimum length of a path to enumerate (defaults to \c 0)
    //! \param max the maximum length of a path to enumerate (defaults to
    //!        libsemigroups::POSITIVE_INFINITY).
    //!
    //! \returns
    //! An iterator \c it of type \c const_panislo_iterator pointing to a
    //! [std::pair](https://en.cppreference.com/w/cpp/utility/pair)
    //! where:
    //! * \c it->first is a libsemigroups::word_type consisting of the edge
    //! labels of the first path (in short-lex order) from \p source of
    //! length in the range \f$[min, max)\f$; and
    //! * \c it->second is the last node on the path from \p source labelled by
    //! \c it->first, a value of \ref node_type.
    //!
    //! \throws LibsemigroupsException if \p source is not a node in the
    //! digraph.
    //!
    //! \warning
    //! Copying iterators of this type is expensive.  As a consequence, prefix
    //! incrementing \c ++it the returned  iterator \c it significantly cheaper
    //! than postfix incrementing \c it++.
    //!
    //! \warning
    //! If the action digraph represented by \c this contains a cycle that is
    //! reachable from \p source, then there are infinitely many paths starting
    //! at \p source, and so \p max should be chosen with some care.
    //!
    //! \sa
    //! cend_panislo
    // TODO(later) example and what is the complexity?
    // not noexcept because const_panislo_iterator constructors aren't
    const_panislo_iterator cbegin_panislo(node_type source,
                                          size_t    min = 0,
                                          size_t    max
                                          = POSITIVE_INFINITY) const {
      action_digraph_helper::validate_node(*this, source);
      return const_panislo_iterator(this, source, min, max);
    }

    //! Returns an iterator for PANISLO (Path And Node In Short Lex Order).
    //!
    //! Returns a forward iterator pointing to one after the last path from any
    //! node in the digraph.
    //!
    //! The iterator returned by this function may still dereferencable and
    //! incrementable, but may not point to a path in the correct range.
    //!
    //! \sa cbegin_panislo
    // not noexcept because const_panislo_iterator constructors aren't
    const_panislo_iterator cend_panislo() const {
      return const_panislo_iterator(this, 0, UNDEFINED, UNDEFINED);
    }

    ////////////////////////////////////////////////////////////////////////
    // PI(S)LO = Path In (Short-)Lex Order
    ////////////////////////////////////////////////////////////////////////
   private:
    // This is a traits class for ConstIteratorStateless in iterator.hpp
    template <typename R>
    struct PiloOrPisloIteratorTraits {
      using internal_iterator_type = R;
      using value_type             = word_type;
      using reference              = value_type&;
      using const_reference        = value_type const&;
      using difference_type        = std::ptrdiff_t;
      using size_type              = std::size_t;
      using const_pointer          = value_type const*;
      using pointer                = value_type*;
      using iterator_category      = std::forward_iterator_tag;

      struct Deref {
        //! No doc
        const_reference
        operator()(internal_iterator_type const& it) const noexcept {
          return it->first;
        }
      };

      struct AddressOf {
        //! No doc
        const_pointer
        operator()(internal_iterator_type const& it) const noexcept {
          return &it->first;
        }
      };

      using EqualTo          = void;
      using NotEqualTo       = void;
      using PostfixIncrement = void;
      using PrefixIncrement  = void;
      using Swap             = void;
    };

   public:
    // PILO
    //! Return type of \ref cbegin_pilo and \ref cend_pilo.
    using const_pilo_iterator = detail::ConstIteratorStateless<
        PiloOrPisloIteratorTraits<const_panilo_iterator>>;

    static_assert(std::is_default_constructible<const_pilo_iterator>::value,
                  "forward iterator requires default-constructible");
    static_assert(std::is_copy_constructible<const_pilo_iterator>::value,
                  "forward iterator requires copy-constructible");
    static_assert(std::is_copy_assignable<const_pilo_iterator>::value,
                  "forward iterator requires copy-assignable");
    static_assert(std::is_destructible<const_pilo_iterator>::value,
                  "forward iterator requires destructible");

    //! Returns an iterator for PILO (Path In Lex Order).
    //!
    //! Returns a forward iterator pointing to the edge labels of the first
    //! path (in lexicographical order) starting at \p source with length in the
    //! range \f$[min, max)\f$.
    //!
    //! If incremented, the iterator will point to the next least edge
    //! labelling of a path (in lexicographical order), with length in the
    //! range \f$[min, max)\f$.  Iterators of the type returned by this
    //! function are equal whenever they point to equal objects.
    //!
    //! \param source the source node
    //! \param min the minimum length of a path to enumerate (defaults to \c 0)
    //! \param max the maximum length of a path to enumerate (defaults to
    //!        libsemigroups::POSITIVE_INFINITY).
    //!
    //! \returns
    //! An iterator \c it of type \c const_pilo_iterator pointing to a
    //! libsemigroups::word_type consisting of the edge labels of the first
    //! path (in lexicographical order) from \p source of length in the range
    //! \f$[min, max)\f$.
    //!
    //! \throws LibsemigroupsException if \p source is not a node in the
    //! digraph.
    //!
    //! \warning
    //! Copying iterators of this type is expensive.  As a consequence, prefix
    //! incrementing \c ++it the returned  iterator \c it significantly cheaper
    //! than postfix incrementing \c it++.
    //!
    //! \warning
    //! If the action digraph represented by \c this contains a cycle that is
    //! reachable from \p source, then there are infinitely many paths starting
    //! at \p source, and so \p max should be chosen with some care.
    //!
    //! \sa
    //! cend_pilo
    // not noexcept because const_panilo_iterator constructors aren't
    const_pilo_iterator cbegin_pilo(node_type source,
                                    size_t    min = 0,
                                    size_t    max = POSITIVE_INFINITY) const {
      action_digraph_helper::validate_node(*this, source);
      return const_pilo_iterator(cbegin_panilo(source, min, max));
    }

    //! Returns an iterator for PILO (Path In Lex Order).
    //!
    //! Returns a forward iterator pointing to one after the last path from any
    //! node in the digraph.
    //!
    //! The iterator returned by this function may still dereferencable and
    //! incrementable, but may not point to a path in the correct range.
    //!
    //! \sa cbegin_pilo
    // not noexcept because const_panilo_iterator constructors aren't
    const_pilo_iterator cend_pilo() const {
      return const_pilo_iterator(cend_panilo());
    }

    // PISLO
    //! Return type of \ref cbegin_pislo and \ref cend_pislo.
    using const_pislo_iterator = detail::ConstIteratorStateless<
        PiloOrPisloIteratorTraits<const_panislo_iterator>>;

    static_assert(std::is_default_constructible<const_pislo_iterator>::value,
                  "forward iterator requires default-constructible");
    static_assert(std::is_copy_constructible<const_pislo_iterator>::value,
                  "forward iterator requires copy-constructible");
    static_assert(std::is_copy_assignable<const_pislo_iterator>::value,
                  "forward iterator requires copy-assignable");
    static_assert(std::is_destructible<const_pislo_iterator>::value,
                  "forward iterator requires destructible");

    //! Returns an iterator for PISLO (Path In Short Lex Order).
    //!
    //! Returns a forward iterator pointing to the edge labels of the first
    //! path (in short-lex order) starting at \p source with length in the range
    //! \f$[min, max)\f$.
    //!
    //! If incremented, the iterator will point to the next least edge
    //! labelling of a path (in short-lex order), with length in the
    //! range \f$[min, max)\f$.  Iterators of the type returned by this
    //! function are equal whenever they point to equal objects.
    //!
    //! \param source the source node
    //! \param min the minimum length of a path to enumerate (defaults to \c 0)
    //! \param max the maximum length of a path to enumerate (defaults to
    //!        libsemigroups::POSITIVE_INFINITY).
    //!
    //! \returns
    //! An iterator \c it of type \c const_pislo_iterator pointing to a
    //! libesemigroups::word_type consisting of the edge labels of the first
    //! path (in short-lex order) from \p source of length in the range
    //! \f$[min, max)\f$.
    //!
    //! \throws LibsemigroupsException if \p source is not a node in the
    //! digraph.
    //!
    //! \warning
    //! Copying iterators of this type is expensive.  As a consequence, prefix
    //! incrementing \c ++it the returned  iterator \c it significantly cheaper
    //! than postfix incrementing \c it++.
    //!
    //! \warning
    //! If the action digraph represented by \c this contains a cycle that is
    //! reachable from \p source, then there are infinitely many paths starting
    //! at \p source, and so \p max should be chosen with some care.
    //!
    //! \sa
    //! cend_pislo
    // not noexcept because cbegin_panislo isn't
    const_pislo_iterator cbegin_pislo(node_type source,
                                      size_t    min = 0,
                                      size_t    max = POSITIVE_INFINITY) const {
      action_digraph_helper::validate_node(*this, source);
      return const_pislo_iterator(cbegin_panislo(source, min, max));
    }

    //! Returns an iterator for PISLO (Path In Short Lex Order).
    //!
    //! Returns a forward iterator pointing to one after the last path from any
    //! node in the digraph.
    //!
    //! The iterator returned by this function may still dereferencable and
    //! incrementable, but may not point to a path in the correct range.
    //!
    //! \sa cbegin_pislo
    // not noexcept because cend_panislo isn't
    const_pislo_iterator cend_pislo() const {
      return const_pislo_iterator(cend_panislo());
    }

    ////////////////////////////////////////////////////////////////////////
    // PSTI(S)LO = Path Source Target In (Short) Lex Order
    ////////////////////////////////////////////////////////////////////////

    // PSTILO = Path Source Target In Lex Order
    //! No doc
    class const_pstilo_iterator final {
     public:
      //! No doc
      using value_type = word_type;
      //! No doc
      using size_type = typename std::vector<value_type>::size_type;
      //! No doc
      using difference_type = typename std::vector<value_type>::difference_type;
      //! No doc
      using const_pointer = typename std::vector<value_type>::const_pointer;
      //! No doc
      using pointer = typename std::vector<value_type>::pointer;
      //! No doc
      using const_reference = typename std::vector<value_type>::const_reference;
      //! No doc
      using reference = const_reference;
      //! No doc
      using iterator_category = std::forward_iterator_tag;

      // None of the constructors are noexcept because the corresponding
      // constructors for std::vector aren't (until C++17).
      //! No doc
      const_pstilo_iterator();
      //! No doc
      const_pstilo_iterator(const_pstilo_iterator const&);
      //! No doc
      const_pstilo_iterator(const_pstilo_iterator&&);
      //! No doc
      const_pstilo_iterator& operator=(const_pstilo_iterator const&);
      //! No doc
      const_pstilo_iterator& operator=(const_pstilo_iterator&&);
      //! No doc
      ~const_pstilo_iterator();

      //! No doc
      const_pstilo_iterator(ActionDigraph const* ptr,
                            node_type const      source,
                            node_type const      target,
                            size_type const      min,
                            size_type const      max)
          : _edges({}),
            _digraph(ptr),
            _edge(UNDEFINED),
            _min(min),
            _max(max),
            _nodes(),
            _target(target) {
        if (_min < _max) {
          _nodes.push_back(source);
          ++(*this);
        }
      }

      //! No doc
      // noexcept because comparison of std::vector<node_type> is noexcept
      // because comparision of node_type's is noexcept
      bool operator==(const_pstilo_iterator const& that) const noexcept {
        return _nodes == that._nodes;
      }

      //! No doc
      // noexcept because operator== is noexcept
      bool operator!=(const_pstilo_iterator const& that) const noexcept {
        return !(this->operator==(that));
      }

      //! No doc
      const_reference operator*() const noexcept {
        return _edges;
      }

      //! No doc
      const_pointer operator->() const noexcept {
        return &_edges;
      }

      // prefix
      //! No doc
      // not noexcept because std::vector::push_back isn't
      const_pstilo_iterator const& operator++() {
        if (_nodes.empty()) {
          return *this;
        } else if (_edge == UNDEFINED) {
          // first call
          _edge = 0;
          init_can_reach_target();
          if (_min == 0 && _nodes.front() == _target) {
            // special case if the source == target, and we allow words of
            // length 0, then we return the empty word here.
            return *this;
          }
        }

        do {
          node_type next;
          std::tie(next, _edge)
              = _digraph->unsafe_next_neighbor(_nodes.back(), _edge);
          if (next != UNDEFINED && _edges.size() < _max - 1) {
            // Avoid infinite loops when we can never reach _target
            if (_can_reach_target[next]) {
              _nodes.push_back(next);
              _edges.push_back(_edge);
              _edge = 0;
              if (_edges.size() >= _min && next == _target) {
                return *this;
              }
            } else {
              _edge++;
            }
          } else {
            _nodes.pop_back();
            if (!_edges.empty()) {
              _edge = _edges.back() + 1;
              _edges.pop_back();
            }
          }
        } while (!_nodes.empty());
        return *this;
      }

      // postfix
      //! No doc
      // not noexcept because (prefix) operator++ isn't
      const_pstilo_iterator operator++(int) {
        const_pstilo_iterator copy(*this);
        ++(*this);
        return copy;
      }

      //! No doc
      void swap(const_pstilo_iterator& that) noexcept {
        std::swap(_edges, that._edges);
        std::swap(_digraph, that._digraph);
        std::swap(_edge, that._edge);
        std::swap(_min, that._min);
        std::swap(_max, that._max);
        std::swap(_nodes, that._nodes);
        std::swap(_target, that._target);
      }

     private:
      void init_can_reach_target() {
        if (_can_reach_target.empty()) {
          std::vector<std::vector<node_type>> in_neighbours(
              _digraph->number_of_nodes(), std::vector<node_type>({}));
          for (auto n = _digraph->cbegin_nodes(); n != _digraph->cend_nodes();
               ++n) {
            for (auto e = _digraph->cbegin_edges(*n);
                 e != _digraph->cend_edges(*n);
                 ++e) {
              if (*e != UNDEFINED) {
                in_neighbours[*e].push_back(*n);
              }
            }
          }

          _can_reach_target.resize(_digraph->number_of_nodes(), false);
          _can_reach_target[_target]   = true;
          std::vector<node_type>& todo = in_neighbours[_target];
          std::vector<node_type>  next;

          while (!todo.empty()) {
            for (auto& m : todo) {
              if (_can_reach_target[m] == 0) {
                _can_reach_target[m] = true;
                next.insert(next.end(),
                            in_neighbours[m].cbegin(),
                            in_neighbours[m].cend());
              }
            }
            std::swap(next, todo);
            next.clear();
          }
        }
      }

      std::vector<bool>      _can_reach_target;
      value_type             _edges;
      ActionDigraph const*   _digraph;
      label_type             _edge;
      size_t                 _min;
      size_t                 _max;
      std::vector<node_type> _nodes;
      node_type              _target;
    };  // const_pstilo_iterator

    // Assert that the forward iterator requirements are met
    static_assert(std::is_default_constructible<const_pstilo_iterator>::value,
                  "forward iterator requires default-constructible");
    static_assert(std::is_copy_constructible<const_pstilo_iterator>::value,
                  "forward iterator requires copy-constructible");
    static_assert(std::is_copy_assignable<const_pstilo_iterator>::value,
                  "forward iterator requires copy-assignable");
    static_assert(std::is_destructible<const_pstilo_iterator>::value,
                  "forward iterator requires destructible");

    //! Returns an iterator for PSTILO (Path Source Target In Lex Order).
    //!
    //! Returns a forward iterator pointing to the edge labels of the first
    //! path (in lexicographical order) starting at the node \p source and
    //! ending at the node \p target with length in the range \f$[min, max)\f$.
    //!
    //! If incremented, the iterator will point to the next least edge
    //! labelling of a path (in lexicographical order).  Iterators of the type
    //! returned by this function are equal whenever they point to equal
    //! objects.
    //!
    //! \param source the first node
    //! \param target the last node
    //! \param min the minimum length of a path to enumerate (defaults to \c 0)
    //! \param max the maximum length of a path to enumerate (defaults to
    //!        libsemigroups::POSITIVE_INFINITY).
    //!
    //! \returns
    //! An iterator \c it of type \c const_pstilo_iterator pointing to a
    //! libsemigroups::word_type consisting of the edge labels of the first
    //! path (in lexicographical order) from the node \p source to the node \p
    //! target with length in the range \f$[min, max)\f$ (if any).
    //!
    //! \throws LibsemigroupsException if \p target or \p source is not a node
    //! in the digraph.
    //!
    //! \warning
    //! Copying iterators of this type is expensive.  As a consequence, prefix
    //! incrementing \c ++it the returned  iterator \c it significantly cheaper
    //! than postfix incrementing \c it++.
    //!
    //! \warning
    //! If the action digraph represented by \c this contains a cycle that is
    //! reachable from \p source, then there may be infinitely many paths
    //! starting at \p source, and so \p max should be chosen with some care.
    //!
    //! \sa
    //! cend_pstilo
    // not noexcept because const_pstilo_iterator constructors aren't
    const_pstilo_iterator cbegin_pstilo(node_type source,
                                        node_type target,
                                        size_t    min = 0,
                                        size_t max = POSITIVE_INFINITY) const {
      // source & target are validated in is_reachable.
      if (!action_digraph_helper::is_reachable(*this, source, target)) {
        return cend_pstilo();
      }
      return const_pstilo_iterator(this, source, target, min, max);
    }

    //! Returns an iterator for PSTILO (Path Source Target In Lex Order).
    //!
    //! Returns a forward iterator pointing to one after the last path from any
    //! node in the digraph.
    //!
    //! The iterator returned by this function may still dereferencable and
    //! incrementable, but may not point to a path in the correct range.
    //!
    //! \sa cbegin_pstilo
    // not noexcept because const_pstilo_iterator constructors aren't
    const_pstilo_iterator cend_pstilo() const {
      return const_pstilo_iterator(this, 0, 0, 0, 0);
    }

   private:
    // PSTISLO
    // This is a traits class for ConstIteratorStateful in iterator.hpp
    struct PstisloIteratorTraits {
      using state_type             = std::pair<node_type, /* terminal node */
                                   const_panislo_iterator /* cend */>;
      using internal_iterator_type = const_panislo_iterator;
      using value_type             = word_type;
      using reference              = value_type&;
      using const_reference        = value_type const&;
      using difference_type        = std::ptrdiff_t;
      using size_type              = std::size_t;
      using const_pointer          = value_type const*;
      using pointer                = value_type*;
      using iterator_category      = std::forward_iterator_tag;

      struct Deref {
        //! No doc
        const_reference
        operator()(state_type&,
                   internal_iterator_type const& it) const noexcept {
          return it->first;
        }
      };

      struct AddressOf {
        //! No doc
        const_pointer operator()(state_type&, internal_iterator_type const& it)
            const noexcept {
          return &it->first;
        }
      };

      struct PrefixIncrement {
        //! No doc
        // not noexcept because const_panislo_iterator::operator++ isn't
        void operator()(state_type& st, internal_iterator_type& it) const {
          if (st.first != UNDEFINED) {
            ++it;
            while (it->second != st.first && it != st.second) {
              ++it;
            }
            if (it == st.second) {
              st.first = UNDEFINED;
            }
          }
        }
      };

      using EqualTo          = void;
      using NotEqualTo       = void;
      using PostfixIncrement = void;
      using Swap             = void;
    };

   public:
    //! Return type of \ref cbegin_pstislo and \ref cend_pstislo.
    using const_pstislo_iterator
        = detail::ConstIteratorStateful<PstisloIteratorTraits>;

    static_assert(std::is_default_constructible<const_pstislo_iterator>::value,
                  "forward iterator requires default-constructible");
    static_assert(std::is_copy_constructible<const_pstislo_iterator>::value,
                  "forward iterator requires copy-constructible");
    static_assert(std::is_copy_assignable<const_pstislo_iterator>::value,
                  "forward iterator requires copy-assignable");
    static_assert(std::is_destructible<const_pstislo_iterator>::value,
                  "forward iterator requires destructible");

    //! Returns an iterator for PSTISLO (Path Source Target In Short Lex
    //! Order).
    //!
    //! Returns a forward iterator pointing to the edge labels of the first
    //! path (in short-lex order) starting at the node \p source and ending
    //! at the node \p target with length in the range \f$[min, max)\f$.
    //!
    //! If incremented, the iterator will point to the next least edge
    //! labelling of a path (in short-lex order).  Iterators of the type
    //! returned by this function are equal whenever they point to equal
    //! objects.
    //!
    //! \param source the first node
    //! \param target the last node
    //! \param min the minimum length of a path to enumerate (defaults to \c 0)
    //! \param max the maximum length of a path to enumerate (defaults to
    //!        libsemigroups::POSITIVE_INFINITY).
    //!
    //! \returns
    //! An iterator \c it of type \c const_pstislo_iterator pointing to a
    //! libsemigroups::word_type consisting of the edge labels of the first
    //! path (in short-lex order) from the node \p source to the node \p target
    //! with length in the range \f$[min, max)\f$ (if any).
    //!
    //! \throws LibsemigroupsException if \p target or \p source is not a node
    //! in the digraph.
    //!
    //! \warning
    //! Copying iterators of this type is expensive.  As a consequence, prefix
    //! incrementing \c ++it the returned  iterator \c it significantly cheaper
    //! than postfix incrementing \c it++.
    //!
    //! \warning
    //! If the action digraph represented by \c this contains a cycle that is
    //! reachable from \p source, then there may be infinitely many paths
    //! starting at \p source, and so \p max should be chosen with some care.
    //!
    //! \sa
    //! cend_pstislo
    // not noexcept because cbegin_panislo isn't
    const_pstislo_iterator cbegin_pstislo(node_type source,
                                          node_type target,
                                          size_t    min = 0,
                                          size_t    max
                                          = POSITIVE_INFINITY) const {
      using state_type = typename const_pstislo_iterator::state_type;
      // source & target are validated in is_reachable.
      if (!action_digraph_helper::is_reachable(*this, source, target)) {
        return cend_pstislo();
      }
      auto st = state_type(target, cend_panislo());
      auto it = cbegin_panislo(source, min, max);
      if (it->second != target) {
        typename PstisloIteratorTraits::PrefixIncrement()(st, it);
      }
      return const_pstislo_iterator(st, it);
    }

    //! Returns an iterator for PSTISLO (Path Source Target In Short Lex
    //! Order).
    //!
    //! Returns a forward iterator pointing to one after the last path from any
    //! node in the digraph.
    //!
    //! The iterator returned by this function may still dereferencable and
    //! incrementable, but may not point to a path in the correct range.
    //!
    //! \sa cbegin_pstislo
    // not noexcept because cend_panislo isn't
    const_pstislo_iterator cend_pstislo() const {
      using state_type = typename const_pstislo_iterator::state_type;
      return const_pstislo_iterator(state_type(UNDEFINED, cend_panislo()),
                                    cend_panislo());
    }

    ////////////////////////////////////////////////////////////////////////
    // ActionDigraph - number_of_paths - public
    ////////////////////////////////////////////////////////////////////////

    //! Returns the \ref algorithm used by number_of_paths().
    //!
    //! \param source the source node.
    //!
    //! \returns A value of type \ref algorithm.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant
    algorithm number_of_paths_algorithm(node_type source) const noexcept {
      (void) source;
      return algorithm::acyclic;
    }

    //! Returns the number of paths from a source node.
    //!
    //! \param source the source node.
    //!
    //! \returns A value of type `uint64_t`.
    //!
    //! \throws LibsemigroupsException if \p source is not a node in the
    //! digraph.
    //!
    //! \complexity
    //! At worst \f$O(nm)\f$ where \f$n\f$ is the number of nodes and \f$m\f$
    //! is the out-degree of the digraph.
    //!
    //! \warning If the number of paths exceeds 2 ^ 64, then return value of
    //! this function will not be correct.
    uint64_t number_of_paths(node_type source) const {
      // Don't allow selecting the algorithm because we check
      // acyclicity anyway.
      // TODO(later): could use algorithm::dfs in some cases.
      action_digraph_helper::validate_node(*this, source);
      auto topo = action_digraph_helper::topological_sort(*this, source);
      if (topo.empty()) {
        // Can't topologically sort, so the subdigraph induced by the nodes
        // reachable from source, contains cycles, and so there are infinitely
        // many words labelling paths.
        return POSITIVE_INFINITY;
      } else {
        // Digraph is acyclic...
        LIBSEMIGROUPS_ASSERT(topo.back() == source);
        if (source == topo[0]) {
          // source is the "sink" of the digraph, and so there's only 1 path
          // (the empty one).
          return 1;
        } else {
          std::vector<uint64_t> number_paths(number_of_nodes(), 0);
          for (auto m = topo.cbegin() + 1; m < topo.cend(); ++m) {
            for (auto n = cbegin_edges(*m); n != cend_edges(*m); ++n) {
              if (*n != UNDEFINED) {
                number_paths[*m] += (number_paths[*n] + 1);
              }
            }
          }
          return number_paths[source] + 1;
        }
      }
    }

    //! Returns the \ref algorithm used by number_of_paths().
    //!
    //! Returns the algorithm used by number_of_paths() to compute the number
    //! of paths originating at the given source node with length in the range
    //! \f$[min, max)\f$.
    //!
    //! \param source the source node
    //! \param min the minimum length of paths to count
    //! \param max the maximum length of paths to count
    //!
    //! \returns A value of type \ref algorithm.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! At worst \f$O(nm)\f$ where \f$n\f$ is the number of nodes and \f$m\f$
    //! is the out-degree of the digraph.
    // Not noexcept because action_digraph_helper::topological_sort is not.
    algorithm number_of_paths_algorithm(node_type source,
                                        size_t    min,
                                        size_t    max) const {
      if (min >= max || validate()) {
        return algorithm::trivial;
      }

      auto topo = action_digraph_helper::topological_sort(*this, source);
      if (topo.empty()) {
        // Can't topologically sort, so the digraph contains cycles, and so
        // there are infinitely many words labelling paths.
        if (max == POSITIVE_INFINITY) {
          return algorithm::trivial;
        } else if (number_of_edges() < detail::magic_number(number_of_nodes())
                                           * number_of_nodes()) {
          return algorithm::dfs;
        } else {
          return algorithm::matrix;
        }
      } else {
        // TODO(later) figure out threshold for using algorithm::dfs
        return algorithm::acyclic;
      }
    }

    //! Returns the number of paths starting at a given node with length in a
    //! given range.
    //!
    //! \param source the first node
    //! \param min the minimum length of a path
    //! \param max the maximum length of a path
    //! \param lgrthm the algorithm to use (defaults to: algorithm::automatic).
    //!
    //! \returns
    //! A value of type \c uint64_t.
    //!
    //! \throws LibsemigroupsException if:
    //! * \p source is not a node in the digraph.
    //! * the algorithm specified by \p lgrthm is not applicable.
    //!
    //! \complexity
    //! The complexity depends on the value of \p lgrthm as follows:
    //! * algorithm::dfs: \f$O(r)\f$ where \f$r\f$ is the number of paths in
    //!   the digraph starting at \p source
    //! * algorithm::matrix: at worst \f$O(n ^ 3 k)\f$ where \f$n\f$ is the
    //!   number of nodes and \f$k\f$ equals \p max.
    //! * algorithm::acyclic: at worst \f$O(nm)\f$ where \f$n\f$ is the number
    //!   of nodes and \f$m\f$ is the out-degree of the digraph (only valid if
    //!   the subdigraph induced by the nodes reachable from \p source is
    //!   acyclic)
    //! * algorithm::trivial: at worst \f$O(nm)\f$ where \f$n\f$ is the number
    //!   of nodes and \f$m\f$ is the out-degree of the digraph (only valid in
    //!   some circumstances)
    //! * algorithm::automatic: attempts to select the fastest algorithm of the
    //!   preceding algorithms and then applies that.
    //!
    //! \warning If \p lgrthm is algorithm::automatic, then it is not always
    //! the case that the fastest algorithm is used.
    //!
    //! \warning If the number of paths exceeds 2 ^ 64, then return value of
    //! this function will not be correct.
    // not noexcept for example number_of_paths_trivial can throw
    uint64_t number_of_paths(node_type source,
                             size_t    min,
                             size_t    max,
                             algorithm lgrthm = algorithm::automatic) const {
      action_digraph_helper::validate_node(*this, source);

      switch (lgrthm) {
        case algorithm::dfs:
          return std::distance(cbegin_panilo(source, min, max), cend_panilo());
        case algorithm::matrix:
          return number_of_paths_matrix(source, min, max);
        case algorithm::acyclic:
          return number_of_paths_acyclic(source, min, max);
        case algorithm::trivial:
          return number_of_paths_trivial(source, min, max);
        case algorithm::automatic:
          // intentional fall through
        default:
          return number_of_paths(
              source, min, max, number_of_paths_algorithm(source, min, max));
      }
    }

    //! Returns the \ref algorithm used by number_of_paths().
    //!
    //! Returns the \ref algorithm used by number_of_paths() to compute the
    //! number of paths originating at the given source node and ending at the
    //! given target node with length in the range \f$[min, max)\f$.
    //!
    //! \param source the source node
    //! \param target the target node
    //! \param min the minimum length of paths to count
    //! \param max the maximum length of paths to count
    //!
    //! \returns A value of type \ref algorithm.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! At worst \f$O(nm)\f$ where \f$n\f$ is the number of nodes and \f$m\f$ is
    //! the out-degree of the digraph.
    // Not noexcept because action_digraph_helper::topological_sort isn't
    algorithm number_of_paths_algorithm(node_type source,
                                        node_type target,
                                        size_t    min,
                                        size_t    max) const {
      bool acyclic = action_digraph_helper::is_acyclic(*this, source, target);
      if (min >= max
          || !action_digraph_helper::is_reachable(*this, source, target)
          || (!acyclic && max == POSITIVE_INFINITY)) {
        return algorithm::trivial;
      } else if (acyclic && action_digraph_helper::is_acyclic(*this, source)) {
        return algorithm::acyclic;
      } else if (number_of_edges() < detail::magic_number(number_of_nodes())
                                         * number_of_nodes()) {
        return algorithm::dfs;
      } else {
        return algorithm::matrix;
      }
    }

    //! Returns the number of paths between a pair of nodes with length in a
    //! given range.
    //!
    //! \param source the first node
    //! \param target the last node
    //! \param min the minimum length of a path
    //! \param max the maximum length of a path
    //! \param lgrthm the algorithm to use (defaults to: algorithm::automatic).
    //!
    //! \returns
    //! A value of type `uint64_t`.
    //!
    //! \throws LibsemigroupsException if:
    //! * \p source is not a node in the digraph.
    //! * \p target is not a node in the digraph.
    //! * the algorithm specified by \p lgrthm is not applicable.
    //!
    //! \complexity
    //! The complexity depends on the value of \p lgrthm as follows:
    //! * algorithm::dfs: \f$O(r)\f$ where \f$r\f$ is the number of paths in
    //!   the digraph starting at \p source
    //! * algorithm::matrix: at worst \f$O(n ^ 3 k)\f$ where \f$n\f$ is the
    //!   number of nodes and \f$k\f$ equals \p max.
    //! * algorithm::acyclic: at worst \f$O(nm)\f$ where \f$n\f$ is the number
    //!   of nodes and \f$m\f$ is the out-degree of the digraph (only valid if
    //!   the subdigraph induced by the nodes reachable from \p source is
    //!   acyclic)
    //! * algorithm::trivial: constant (only valid in some circumstances)
    //! * algorithm::automatic: attempts to select the fastest algorithm of the
    //!   preceding algorithms and then applies that.
    //!
    //! \warning If \p lgrthm is algorithm::automatic, then it is not always
    //! the case that the fastest algorithm is used.
    //!
    //! \warning If the number of paths exceeds 2 ^ 64, then return value of
    //! this function will not be correct.
    // not noexcept because cbegin_pstilo isn't
    uint64_t number_of_paths(node_type source,
                             node_type target,
                             size_t    min,
                             size_t    max,
                             algorithm lgrthm = algorithm::automatic) const {
      action_digraph_helper::validate_node(*this, source);
      action_digraph_helper::validate_node(*this, target);

      switch (lgrthm) {
        case algorithm::dfs:
          if (number_of_paths_special(source, target, min, max)) {
            return POSITIVE_INFINITY;
          }
          return std::distance(cbegin_pstilo(source, target, min, max),
                               cend_pstilo());
        case algorithm::matrix:
          return number_of_paths_matrix(source, target, min, max);
        case algorithm::acyclic:
          return number_of_paths_acyclic(source, target, min, max);
        case algorithm::trivial:
          return number_of_paths_trivial(source, target, min, max);
        case algorithm::automatic:
          // intentional fall through
        default:
          return number_of_paths(
              source,
              target,
              min,
              max,
              number_of_paths_algorithm(source, target, min, max));
      }
    }

    //! Returns a const reference to the underlying array.
    //!
    //! This function returns a const reference to the underlying container for
    //! the edges of a digraph.
    //!
    //! \parameters (None)
    //!
    //! \returns
    //! A const reference to a detail::DynamicArray2<node_type>.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    // TODO(v3) this either shouldn't exist it's used by ToddCoxeter when
    // creating the quotient FroidurePin, but we could just use the
    // ActionDigraph itself.
    detail::DynamicArray2<T> const& table() const noexcept {
      return _dynamic_array_2;
    }

   protected:
    // TODO(v3) make this public, doc, and test it
    template <typename S>
    void apply_row_permutation(S const& p) {
      reset();
      _dynamic_array_2.apply_row_permutation(p);
    }

   private:
    // Implemented below
    bool number_of_paths_special(node_type source,
                                 node_type target,
                                 size_t    min,
                                 size_t    max) const;

    ////////////////////////////////////////////////////////////////////////
    // ActionDigraph - number_of_paths_trivial - private
    ////////////////////////////////////////////////////////////////////////

    uint64_t number_of_paths_trivial(node_type source,
                                     size_t    min,
                                     size_t    max) const {
      if (min >= max) {
        return 0;
      } else if (validate()) {
        // every edge is defined, and so the graph is not acyclic, and so the
        // number of words labelling paths is just the number of words
        if (max == POSITIVE_INFINITY) {
          return POSITIVE_INFINITY;
        } else {
          // TODO(later) it's easy for number_of_words to exceed 2 ^ 64, so
          // better do something more intelligent here to avoid this case.
          return number_of_words(out_degree(), min, max);
        }
      }
      // Some edges are not defined ...
      if (!action_digraph_helper::is_acyclic(*this, source)
          && max == POSITIVE_INFINITY) {
        // Not acyclic
        return POSITIVE_INFINITY;
      }
      LIBSEMIGROUPS_EXCEPTION("number of paths cannot be trivially determined");
    }

    uint64_t number_of_paths_trivial(node_type source,
                                     node_type target,
                                     size_t    min,
                                     size_t    max) const {
      if (min >= max
          || !action_digraph_helper::is_reachable(*this, source, target)) {
        return 0;
      } else if (!action_digraph_helper::is_acyclic(*this, source, target)
                 && max == POSITIVE_INFINITY) {
        return POSITIVE_INFINITY;
      }
      LIBSEMIGROUPS_EXCEPTION("number of paths cannot be trivially determined");
    }

    ////////////////////////////////////////////////////////////////////////
    // ActionDigraph - number_of_paths_matrix - private
    ////////////////////////////////////////////////////////////////////////

    uint64_t number_of_paths_matrix(node_type source,
                                    size_t    min,
                                    size_t    max) const {
      auto am = detail::adjacency_matrix<adjacency_matrix_type>(*this);
#ifdef LIBSEMIGROUPS_EIGEN_ENABLED
      auto     acc   = detail::pow(am, min);
      uint64_t total = 0;
      for (size_t i = min; i < max; ++i) {
        uint64_t add = acc.row(source).sum();
        if (add == 0) {
          break;
        }
        total += add;
        acc *= am;
      }
#else
      auto           tmp   = am;
      uint64_t const N     = number_of_nodes();
      auto           acc   = matrix_helpers::pow(am, min);
      uint64_t       total = 0;
      for (size_t i = min; i < max; ++i) {
        uint64_t add = std::accumulate(acc.cbegin() + source * N,
                                       acc.cbegin() + source * N + N,
                                       uint64_t(0));
        if (add == 0) {
          break;
        }
        total += add;
        tmp.product_inplace(acc, am);
        tmp.swap(acc);
      }
#endif
      return total;
    }

    uint64_t number_of_paths_matrix(node_type source,
                                    node_type target,
                                    size_t    min,
                                    size_t    max) const {
      if (!action_digraph_helper::is_reachable(*this, source, target)) {
        // Complexity is O(number of nodes + number of edges).
        return 0;
      } else if (number_of_paths_special(source, target, min, max)) {
        return POSITIVE_INFINITY;
      }

      auto am = detail::adjacency_matrix<adjacency_matrix_type>(*this);
#ifdef LIBSEMIGROUPS_EIGEN_ENABLED
      auto     acc   = detail::pow(am, min);
      uint64_t total = 0;
      for (size_t i = min; i < max; ++i) {
        uint64_t add = acc(source, target);
        if (add == 0 && acc.row(source).isZero()) {
          break;
        }
        total += add;
        acc *= am;
      }
#else
      size_t const N     = number_of_nodes();
      auto         tmp   = am;
      auto         acc   = matrix_helpers::pow(am, min);
      size_t       total = 0;
      for (size_t i = min; i < max; ++i) {
        uint64_t add = acc(source, target);

        if (add == 0
            && std::all_of(acc.cbegin() + source * N,
                           acc.cbegin() + source * N + N,
                           [](uint64_t j) { return j == 0; })) {
          break;
        }
        total += add;
        tmp.product_inplace(acc, am);
        tmp.swap(acc);
      }
#endif
      return total;
    }

    ////////////////////////////////////////////////////////////////////////
    // ActionDigraph - number_of_paths_acyclic - private
    ////////////////////////////////////////////////////////////////////////

    uint64_t number_of_paths_acyclic(node_type source,
                                     size_t    min,
                                     size_t    max) const {
      auto topo = action_digraph_helper::topological_sort(*this, source);
      if (topo.empty()) {
        // Can't topologically sort, so the digraph contains cycles.
        LIBSEMIGROUPS_EXCEPTION("the subdigraph induced by the nodes reachable "
                                "from %llu is not acyclic",
                                static_cast<uint64_t>(source));
      } else if (topo.size() <= min) {
        // There are fewer than `min` nodes reachable from source, and so there
        // are no paths of length `min` or greater
        return 0;
      }

      LIBSEMIGROUPS_ASSERT(source == topo.back());
      // Digraph is acyclic...
      // Columns correspond to path lengths, rows to nodes in the graph
      // TODO(later) replace with DynamicTriangularArray2
      auto number_paths = detail::DynamicArray2<uint64_t>(
          std::min(max, topo.size()),
          *std::max_element(topo.cbegin(), topo.cend()) + 1,
          0);
      number_paths.set(topo[0], 0, 1);
      for (size_t m = 1; m < topo.size(); ++m) {
        number_paths.set(topo[m], 0, 1);
        for (auto n = cbegin_edges(topo[m]); n != cend_edges(topo[m]); ++n) {
          if (*n != UNDEFINED) {
            // there are no paths longer than m + 1 from the m-th entry in
            // the topological sort.
            for (size_t i = 1; i < std::min(max, m + 1); ++i) {
              number_paths.set(topo[m],
                               i,
                               number_paths.get(*n, i - 1)
                                   + number_paths.get(topo[m], i));
            }
          }
        }
      }
      return std::accumulate(number_paths.cbegin_row(source) + min,
                             number_paths.cbegin_row(source)
                                 + std::min(topo.size(), max),
                             0);
    }

    uint64_t number_of_paths_acyclic(node_type source,
                                     node_type target,
                                     size_t    min,
                                     size_t    max) const {
      auto topo = action_digraph_helper::topological_sort(*this, source);
      if (topo.empty()) {
        // Can't topologically sort, so the digraph contains cycles.
        LIBSEMIGROUPS_EXCEPTION("the subdigraph induced by the nodes reachable "
                                "from %llu is not acyclic",
                                static_cast<uint64_t>(source));
      } else if ((max == 1 && source != target)
                 || (min != 0 && source == target)) {
        return 0;
      } else if (source == target) {
        // the empty path
        return 1;
      }

      // Subdigraph induced by nodes reachable from `source` is acyclic...
      LIBSEMIGROUPS_ASSERT(source == topo.back());
      auto it = std::find(topo.cbegin(), topo.cend(), target);
      if (it == topo.cend() || size_t(std::distance(it, topo.cend())) <= min) {
        // 1) `target` not reachable from `source`, or
        // 2) every path from `source` to `target` has length < min.
        return 0;
      }
      // Don't visit nodes that occur after target in "topo".
      std::vector<bool> lookup(number_of_nodes(), true);
      std::for_each(topo.cbegin(), it, [&lookup](node_type const& n) {
        lookup[n] = false;
      });

      // Remove the entries in topo after target
      topo.erase(topo.cbegin(), it);

      // Columns correspond to path lengths, rows to nodes in the graph
      // TODO(later) replace with DynamicTriangularArray2
      auto number_paths = detail::DynamicArray2<uint64_t>(
          std::min(topo.size(), max),
          *std::max_element(topo.cbegin(), topo.cend()) + 1,
          0);

      for (size_t m = 1; m < topo.size(); ++m) {
        for (auto n = cbegin_edges(topo[m]); n != cend_edges(topo[m]); ++n) {
          if (*n == target) {
            number_paths.set(topo[m], 1, number_paths.get(topo[m], 1) + 1);
          }
          if (*n != UNDEFINED && lookup[*n]) {
            // there are no paths longer than m + 1 from the m-th entry in
            // the topological sort.
            for (size_t i = 1; i < std::min(max, m + 1); ++i) {
              number_paths.set(topo[m],
                               i,
                               number_paths.get(*n, i - 1)
                                   + number_paths.get(topo[m], i));
            }
          }
        }
      }
      return std::accumulate(number_paths.cbegin_row(source) + min,
                             number_paths.cbegin_row(source)
                                 + std::min(topo.size(), max),
                             0);
    }

    ////////////////////////////////////////////////////////////////////////
    // ActionDigraph - validation - private
    ////////////////////////////////////////////////////////////////////////

    void validate_scc_index(scc_index_type i) const {
      if (i >= number_of_scc()) {
        LIBSEMIGROUPS_EXCEPTION("strong component index out of bounds, "
                                "expected value in the range [0, %d), got %d",
                                number_of_scc(),
                                i);
      }
    }

    ////////////////////////////////////////////////////////////////////////
    // ActionDigraph - reset (after calling a modifier) - private
    ////////////////////////////////////////////////////////////////////////

    void reset() noexcept {
      _scc_back_forest._defined = false;
      _scc._defined             = false;
      _scc_forest._defined      = false;
    }

    ////////////////////////////////////////////////////////////////////////
    // ActionDigraph - strongly connected components - private
    ////////////////////////////////////////////////////////////////////////

    void gabow_scc() const {
      if (_scc._defined) {
        return;
      } else if (!validate()) {
        LIBSEMIGROUPS_EXCEPTION("digraph not fully defined, cannot find "
                                "strongly connected components");
      }

      static std::stack<T>               stack1;
      static std::stack<T>               stack2;
      static std::stack<std::pair<T, T>> frame;
      static std::vector<T>              preorder;
      preorder.assign(number_of_nodes(), UNDEFINED);
      LIBSEMIGROUPS_ASSERT(stack1.empty());
      LIBSEMIGROUPS_ASSERT(stack2.empty());
      LIBSEMIGROUPS_ASSERT(frame.empty());

      _scc._comps.clear();
      _scc._id.assign(number_of_nodes(), UNDEFINED);

      T C     = 0;
      T index = 0;

      for (T w = 0; w < number_of_nodes(); ++w) {
        if (_scc._id[w] == UNDEFINED) {
          frame.emplace(w, 0);
        dfs_start:
          LIBSEMIGROUPS_ASSERT(!frame.empty());
          T v = frame.top().first;
          T i = frame.top().second;

          preorder[v] = C++;
          stack1.push(v);
          stack2.push(v);
          for (; i < _degree; ++i) {
          dfs_end:
            LIBSEMIGROUPS_ASSERT(v < number_of_nodes() && i < _degree);
            T u = _dynamic_array_2.get(v, i);
            if (preorder[u] == UNDEFINED) {
              frame.top().second = i;
              frame.emplace(u, 0);
              goto dfs_start;
            } else if (_scc._id[u] == UNDEFINED) {
              LIBSEMIGROUPS_ASSERT(!stack2.empty());
              while (preorder[stack2.top()] > preorder[u]) {
                stack2.pop();
              }
            }
          }
          if (v == stack2.top()) {
            _scc._comps.emplace_back();
            T x;
            do {
              LIBSEMIGROUPS_ASSERT(!stack1.empty());
              x           = stack1.top();
              _scc._id[x] = index;
              _scc._comps[index].push_back(x);
              stack1.pop();
            } while (x != v);
            ++index;
            LIBSEMIGROUPS_ASSERT(!stack2.empty());
            stack2.pop();
          }
          LIBSEMIGROUPS_ASSERT(!frame.empty());
          frame.pop();
          if (!frame.empty()) {
            v = frame.top().first;
            i = frame.top().second;
            goto dfs_end;
          }
        }
      }
      _scc._defined = true;
    }

    ////////////////////////////////////////////////////////////////////////
    // ActionDigraph - data members - private
    ////////////////////////////////////////////////////////////////////////

    T                                _degree;
    T                                _nr_nodes;
    T                                _num_active_nodes;
    mutable detail::DynamicArray2<T> _dynamic_array_2;

    struct Attr {
      Attr() : _defined(false) {}
      bool _defined;
    };

    mutable struct SCCBackForest : public Attr {
      SCCBackForest() : Attr(), _forest() {}
      Forest _forest;
    } _scc_back_forest;

    mutable struct SCCForwardForest : public Attr {
      SCCForwardForest() : Attr(), _forest() {}
      Forest _forest;
    } _scc_forest;

    mutable struct SCC : public Attr {
      SCC() : Attr(), _comps(), _id() {}
      std::vector<std::vector<node_type>> _comps;
      std::vector<scc_index_type>         _id;
    } _scc;
  };

  //////////////////////////////////////////////////////////////////////////
  // ActionDigraph - constructor/destructor implementations
  //////////////////////////////////////////////////////////////////////////

  template <typename T>
  ActionDigraph<T>::~ActionDigraph() = default;

  template <typename T>
  ActionDigraph<T>::ActionDigraph(T m, T n)
      : _degree(n),
        _nr_nodes(m),
        _num_active_nodes(),
        _dynamic_array_2(_degree, _nr_nodes, UNDEFINED),
        _scc_back_forest(),
        _scc_forest(),
        _scc() {}

  template <typename T>
  ActionDigraph<T>::ActionDigraph(ActionDigraph const&) = default;

  template <typename T>
  ActionDigraph<T>::ActionDigraph(ActionDigraph&&) = default;

  template <typename T>
  ActionDigraph<T>& ActionDigraph<T>::operator=(ActionDigraph const&) = default;

  template <typename T>
  ActionDigraph<T>& ActionDigraph<T>::operator=(ActionDigraph&&) = default;

  //////////////////////////////////////////////////////////////////////////
  // ActionDigraph - panilo - constructor/destructor implementations
  //////////////////////////////////////////////////////////////////////////

  template <typename T>
  ActionDigraph<T>::const_panilo_iterator::~const_panilo_iterator() = default;

  template <typename T>
  ActionDigraph<T>::const_panilo_iterator::const_panilo_iterator(
      const_panilo_iterator const&)
      = default;

  template <typename T>
  ActionDigraph<T>::const_panilo_iterator::const_panilo_iterator() = default;

  template <typename T>
  typename ActionDigraph<T>::const_panilo_iterator&
  ActionDigraph<T>::const_panilo_iterator::operator=(
      const_panilo_iterator const&)
      = default;

  template <typename T>
  typename ActionDigraph<T>::const_panilo_iterator&
  ActionDigraph<T>::const_panilo_iterator::operator=(const_panilo_iterator&&)
      = default;

  template <typename T>
  ActionDigraph<T>::const_panilo_iterator::const_panilo_iterator(
      const_panilo_iterator&&)
      = default;

  //////////////////////////////////////////////////////////////////////////
  // ActionDigraph - pstilo - constructor/destructor implementations
  //////////////////////////////////////////////////////////////////////////

  template <typename T>
  ActionDigraph<T>::const_pstilo_iterator::const_pstilo_iterator() = default;
  template <typename T>
  ActionDigraph<T>::const_pstilo_iterator::const_pstilo_iterator(
      const_pstilo_iterator const&)
      = default;
  template <typename T>
  ActionDigraph<T>::const_pstilo_iterator::const_pstilo_iterator(
      const_pstilo_iterator&&)
      = default;

  template <typename T>
  typename ActionDigraph<T>::const_pstilo_iterator&
  ActionDigraph<T>::const_pstilo_iterator::operator=(
      const_pstilo_iterator const&)
      = default;

  template <typename T>
  typename ActionDigraph<T>::const_pstilo_iterator&
  ActionDigraph<T>::const_pstilo_iterator::operator=(const_pstilo_iterator&&)
      = default;

  template <typename T>
  ActionDigraph<T>::const_pstilo_iterator::~const_pstilo_iterator() = default;

  ////////////////////////////////////////////////////////////////////////
  // ActionDigraph - number_of_paths_special - private
  ////////////////////////////////////////////////////////////////////////

  // Used by the matrix(source, target) and the dfs(source, target)
  // algorithms
  template <typename T>
  bool ActionDigraph<T>::number_of_paths_special(node_type source,
                                                 node_type target,
                                                 size_t,
                                                 size_t max) const {
    if (max == POSITIVE_INFINITY) {
      if (source == target
          && std::any_of(cbegin_edges(source),
                         cend_edges(source),
                         [this, source](node_type const n) {
                           return n != UNDEFINED
                                  && action_digraph_helper::is_reachable(
                                      *this, n, source);
                         })) {
        return true;
      } else if (source != target
                 && action_digraph_helper::is_reachable(*this, source, target)
                 && action_digraph_helper::is_reachable(
                     *this, target, source)) {
        return true;
      }
    }
    return false;
  }

  namespace detail {

#ifdef LIBSEMIGROUPS_EIGEN_ENABLED
    template <typename T>
    void init_adjacency_matrix(
        ActionDigraph<T> const&                                ad,
        Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>& mat) {
      size_t const N = ad.number_of_nodes();
      mat.resize(N, N);
      mat.fill(0);
    }

    static inline void
    identity(Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>& x) {
      x.fill(0);
      for (size_t i = 0; i < static_cast<size_t>(x.rows()); ++i) {
        x(i, i) = 1;
      }
    }

    // Why does this exist? TODO(v3) try replacing it with Eigen::MatrixPower or
    // x.pow()
    static inline Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>
    pow(Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> const& x,
        size_t                                                       e) {
      if (x.cols() != x.rows()) {
        LIBSEMIGROUPS_EXCEPTION("expected a square matrix, found %llux%llu",
                                static_cast<uint64_t>(x.rows()),
                                static_cast<uint64_t>(x.cols()));
      }
      auto y = x;
      if (e % 2 == 0) {
        identity(y);
        if (e == 0) {
          return y;
        }
      }
      auto z = x;
      while (e > 1) {
        z *= z;
        e /= 2;
        if (e % 2 == 1) {
          y *= z;
        }
      }
      return y;
    }
#else
    template <typename T>
    void init_adjacency_matrix(ActionDigraph<T> const& ad,
                               IntMat<0, 0, int64_t>&  mat) {
      size_t const N = ad.number_of_nodes();
      mat            = IntMat<0, 0, int64_t>(N, N);
      std::fill(mat.begin(), mat.end(), 0);
    }
#endif

    template <typename Mat, typename T>
    Mat adjacency_matrix(ActionDigraph<T> const& ad) {
      Mat mat;
      init_adjacency_matrix(ad, mat);

      for (auto n = ad.cbegin_nodes(); n != ad.cend_nodes(); ++n) {
        for (auto e = ad.cbegin_edges(*n); e != ad.cend_edges(*n); ++e) {
          if (*e != UNDEFINED) {
            mat(*n, *e) += 1;
          }
        }
      }
      return mat;
    }
  }  // namespace detail

  //! Output the edges of an ActionDigraph to a stream.
  //!
  //! This function outputs the action digraph \p ad to the stream \p os. The
  //! digraph is represented by the out-neighbours of each node ordered
  //! according to their labels. The symbol `-` is used to denote that an edge
  //! is not defined. For example, the digraph with 1 nodes, out-degree 2, and
  //! a single loop labelled 1 from node 0 to 0 is represented as `{{-, 0}}`.
  //!
  //! \param os the ostream
  //! \param ad the action digraph
  //!
  //! \returns
  //! The first parameter \p os.
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  template <typename T>
  std::ostream& operator<<(std::ostream& os, ActionDigraph<T> const& ad) {
    os << "{";
    std::string sep_n;
    for (auto n = ad.cbegin_nodes(); n != ad.cend_nodes(); ++n) {
      std::string sep_e;
      os << sep_n << "{";
      for (auto e = ad.cbegin_edges(*n); e != ad.cend_edges(*n); ++e) {
        os << sep_e << (*e == UNDEFINED ? "-" : std::to_string(*e));
        sep_e = ", ";
      }
      os << "}";
      sep_n = ", ";
    }
    os << "}";
    return os;
  }
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_DIGRAPH_HPP_
