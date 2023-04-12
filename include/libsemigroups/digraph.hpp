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
// * iwyu
// * More benchmarks
// * split into tpp file
// * split out GabowSCC and remove the attributes nonsense

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
#include <variant>      // for variant
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
#include "order.hpp"           // for order
#include "types.hpp"           // for word_type
#include "words.hpp"           // for number_of_words

#include <rx/ranges.hpp>

#ifdef LIBSEMIGROUPS_EIGEN_ENABLED
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif
#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
#include <Eigen/Core>
#include <unsupported/Eigen/MatrixFunctions>
#undef _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
#endif

namespace libsemigroups {

  struct ActionDigraphBase {};

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
  class ActionDigraph : private ActionDigraphBase {
    static_assert(std::is_integral<T>(),
                  "the template parameter T must be an integral type!");
    static_assert(
        std::is_unsigned<T>(),
        "the template parameter T must be an unsigned integral type!");

    template <typename N>
    friend class ActionDigraph;

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

    //! Re-initialize the digraph to have \p m nodes and out-degree \p n
    //!
    //! This functions puts a digraph into the state that it would have been in
    //! if it had just been newly constructed with the same parameters \p m and
    //! \p n.
    //!
    //! \param m the number of nodes in the digraph.
    //! \param n the out-degree of every node.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \par Complexity
    //! \f$O(mn)\f$ where \p m is the number of nodes, and \p n is the
    //! out-degree of the digraph.
    void init(T m, T n);

    //! Default copy constructor
    ActionDigraph(ActionDigraph const&);

    template <typename N>
    ActionDigraph(ActionDigraph<N> const&);

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

    void inline def_edge(node_type i, label_type lbl, node_type j) {
      action_digraph_helper::validate_node(*this, i);
      action_digraph_helper::validate_node(*this, j);
      action_digraph_helper::validate_label(*this, lbl);
      def_edge_nc(i, lbl, j);
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
    //! No checks whatsoever on the validity of the arguments are performed.

    // TODO remove this in v3
    void inline add_edge_nc(node_type i, node_type j, label_type lbl) {
      _dynamic_array_2.set(i, lbl, j);
      reset();
    }

    void inline def_edge_nc(node_type i, label_type lbl, node_type j) {
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
    //! No checks whatsoever on the validity of the arguments are performed.
    void inline remove_edge_nc(node_type i, label_type lbl) {
      _dynamic_array_2.set(i, lbl, UNDEFINED);
      reset();
    }

    //! Remove all of the edges in the digraph.
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
    void inline remove_all_edges() {
      std::fill(_dynamic_array_2.begin(), _dynamic_array_2.end(), UNDEFINED);
      reset();
    }

    void inline remove_label(label_type a) {
      _dynamic_array_2.erase_column(a);
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
    //! No checks whatsoever on the validity of the arguments are performed.
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

    // no except correct?
    auto nodes() const noexcept {
      return rx::seq() | rx::take(number_of_nodes());
    }

    // no except correct?
    auto labels() const noexcept {
      return rx::seq() | rx::take(out_degree());
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
    //! No checks whatsoever on the validity of the arguments are performed.
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
    //! No checks whatsoever on the validity of the arguments are performed.
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
            if (u != UNDEFINED) {
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
  void ActionDigraph<T>::init(T m, T n) {
    _degree           = n;
    _nr_nodes         = m;
    _num_active_nodes = 0;
    _dynamic_array_2.reshape(n, m);
    remove_all_edges();
    reset();
  }

  template <typename T>
  ActionDigraph<T>::ActionDigraph(ActionDigraph const&) = default;

  template <typename T>
  template <typename N>
  ActionDigraph<T>::ActionDigraph(ActionDigraph<N> const& ad)
      : ActionDigraph(ad.number_of_nodes(), ad.out_degree()) {
    _dynamic_array_2 = ad._dynamic_array_2;
  }

  template <typename T>
  ActionDigraph<T>::ActionDigraph(ActionDigraph&&) = default;

  template <typename T>
  ActionDigraph<T>& ActionDigraph<T>::operator=(ActionDigraph const&) = default;

  template <typename T>
  ActionDigraph<T>& ActionDigraph<T>::operator=(ActionDigraph&&) = default;

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

  namespace action_digraph {
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

#else
      template <typename T>
      void init_adjacency_matrix(ActionDigraph<T> const& ad,
                                 IntMat<0, 0, int64_t>&  mat) {
        size_t const N = ad.number_of_nodes();
        mat            = IntMat<0, 0, int64_t>(N, N);
        std::fill(mat.begin(), mat.end(), 0);
      }
#endif
    }  // namespace detail

#ifdef LIBSEMIGROUPS_EIGEN_ENABLED
    static inline Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>
    pow(Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> const& x,
        size_t                                                       e) {
      using Mat = Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>;
      return Eigen::MatrixPower<Mat>(x)(e);
    }
#endif

    template <typename T>
    auto adjacency_matrix(ActionDigraph<T> const& ad) {
      using Mat = typename ActionDigraph<T>::adjacency_matrix_type;
      Mat mat;
      detail::init_adjacency_matrix(ad, mat);

      for (auto n = ad.cbegin_nodes(); n != ad.cend_nodes(); ++n) {
        for (auto e = ad.cbegin_edges(*n); e != ad.cend_edges(*n); ++e) {
          if (*e != UNDEFINED) {
            mat(*n, *e) += 1;
          }
        }
      }
      return mat;
    }

    namespace detail {
      // TODO(now) to tpp file
      template <typename T>
      bool shortlex_standardize(T& d, Forest& f) {
        LIBSEMIGROUPS_ASSERT(d.number_of_nodes() != 0);
        LIBSEMIGROUPS_ASSERT(f.number_of_nodes() == 0);

        using node_type = typename T::node_type;
        f.add_nodes(1);

        node_type    t      = 0;
        size_t const n      = d.out_degree();
        bool         result = false;

        // p : new -> old and q : old -> new
        std::vector<node_type> p(d.number_of_nodes(), 0);
        std::iota(p.begin(), p.end(), 0);
        std::vector<node_type> q(p);

        for (node_type s = 0; s <= t; ++s) {
          for (letter_type x = 0; x < n; ++x) {
            node_type r = d.unsafe_neighbor(p[s], x);
            if (r != UNDEFINED) {
              r = q[r];  // new
              if (r > t) {
                t++;
                f.add_nodes(1);
                if (r > t) {
                  std::swap(p[t], p[r]);
                  std::swap(q[p[t]], q[p[r]]);
                  result = true;
                }
                f.set(t, (s == t ? r : s), x);
              }
            }
          }
        }
        d.permute_nodes_nc(p, q);
        return result;
      }

      template <typename T>
      bool lex_standardize(T& d, Forest& f) {
        LIBSEMIGROUPS_ASSERT(d.number_of_nodes() != 0);
        LIBSEMIGROUPS_ASSERT(f.number_of_nodes() == 0);

        using node_type  = typename T::node_type;
        using label_type = typename T::label_type;

        f.add_nodes(1);

        node_type  s = 0, t = 0;
        label_type x      = 0;
        auto const n      = d.out_degree();
        bool       result = false;

        // p : new -> old and q : old -> new
        std::vector<node_type> p(d.number_of_nodes(), 0);
        std::iota(p.begin(), p.end(), 0);
        std::vector<node_type> q(p);

        // Perform a DFS through d
        while (s <= t) {
          node_type r = d.unsafe_neighbor(p[s], x);
          if (r != UNDEFINED) {
            r = q[r];  // new
            if (r > t) {
              t++;
              f.add_nodes(1);
              if (r > t) {
                std::swap(p[t], p[r]);
                std::swap(q[p[t]], q[p[r]]);
                result = true;
              }
              f.set(t, (s == t ? r : s), x);
              s = t;
              x = 0;
              continue;
            }
          }
          x++;
          if (x == n) {  // backtrack
            x = f.label(s);
            s = f.parent(s);
          }
        }
        d.permute_nodes_nc(p, q);
        return result;
      }

      template <typename T>
      bool recursive_standardize(T& d, Forest& f) {
        LIBSEMIGROUPS_ASSERT(d.number_of_nodes() != 0);
        LIBSEMIGROUPS_ASSERT(f.number_of_nodes() == 0);

        using node_type = typename T::node_type;

        f.add_nodes(1);

        std::vector<word_type> words;
        size_t const           n = d.out_degree();
        letter_type            a = 0;
        node_type              s = 0, t = 0;

        std::vector<node_type> p(d.number_of_nodes(), 0);
        std::iota(p.begin(), p.end(), 0);
        std::vector<node_type> q(p);

        size_t max_t
            = action_digraph_helper::number_of_nodes_reachable_from(d, 0) - 1;

        // TODO move this out of here and use it in the other standardize
        // functions
        auto swap_if_necessary = [&d, &f, &p, &q](node_type const   s,
                                                  node_type&        t,
                                                  letter_type const x) {
          node_type r      = d.unsafe_neighbor(p[s], x);
          bool      result = false;
          if (r != UNDEFINED) {
            r = q[r];  // new
            if (r > t) {
              t++;
              f.add_nodes(1);
              if (r > t) {
                std::swap(p[t], p[r]);
                std::swap(q[p[t]], q[p[r]]);
              }
              result = true;
              f.set(t, (s == t ? r : s), x);
            }
          }
          return result;
        };

        bool result = false;

        while (s <= t) {
          if (swap_if_necessary(s, t, 0)) {
            words.push_back(word_type(t, a));
            result = true;
          }
          s++;
        }
        a++;
        bool new_generator = true;
        int  x, u, w;
        while (a < n && t < max_t) {
          if (new_generator) {
            w = -1;  // -1 is the empty word
            if (swap_if_necessary(0, t, a)) {
              result = true;
              words.push_back({a});
            }
            x             = words.size() - 1;
            u             = words.size() - 1;
            new_generator = false;
          }

          node_type const uu = action_digraph_helper::follow_path_nc(
              d, 0, words[u].begin(), words[u].end());
          if (uu != UNDEFINED) {
            for (int v = 0; v < x; v++) {
              node_type const uuv = action_digraph_helper::follow_path_nc(
                  d, uu, words[v].begin(), words[v].end() - 1);
              if (uuv != UNDEFINED) {
                s = q[uuv];
                if (swap_if_necessary(s, t, words[v].back())) {
                  result        = true;
                  word_type nxt = words[u];
                  nxt.insert(nxt.end(), words[v].begin(), words[v].end());
                  words.push_back(std::move(nxt));
                }
              }
            }
          }
          w++;
          if (static_cast<size_t>(w) < words.size()) {
            node_type const ww = action_digraph_helper::follow_path_nc(
                d, 0, words[w].begin(), words[w].end());
            if (ww != UNDEFINED) {
              s = q[ww];
              if (swap_if_necessary(s, t, a)) {
                result        = true;
                u             = words.size();
                word_type nxt = words[w];
                nxt.push_back(a);
                words.push_back(std::move(nxt));
              }
            }
          } else {
            a++;
            new_generator = true;
          }
        }
        d.permute_nodes_nc(p, q);
        return result;
      }
    }  // namespace detail

    // Return value indicates whether or not the graph was modified.
    // TODO(now) to tpp file
    template <typename T>
    bool standardize(T& d, Forest& f, order val) {
      // TODO(later): should be DigraphWithSourcesBase
      static_assert(
          std::is_base_of<ActionDigraphBase, T>::value,
          "the template parameter T must be derived from ActionDigraphBase");
      if (!f.empty()) {
        f.clear();
      }
      if (d.number_of_nodes() == 0) {
        return false;
      }

      switch (val) {
        case order::none:
          return false;
        case order::shortlex:
          return detail::shortlex_standardize(d, f);
        case order::lex:
          return detail::lex_standardize(d, f);
        case order::recursive:
          return detail::recursive_standardize(d, f);
        default:
          return false;
      }
    }

    template <typename T>
    std::pair<bool, Forest> standardize(T& d, order val = order::shortlex) {
      static_assert(
          std::is_base_of<ActionDigraphBase, T>::value,
          "the template parameter T must be derived from ActionDigraphBase");
      Forest f;
      bool   result = standardize(d, f, val);
      return std::make_pair(result, f);
    }

    template <typename Node, typename Iterator1, typename Iterator2>
    bool is_complete(ActionDigraph<Node> const& d,
                     Iterator1                  first_node,
                     Iterator2                  last_node) {
      size_t const n = d.out_degree();
      for (auto it = first_node; it != last_node; ++it) {
        for (size_t a = 0; a < n; ++a) {
          if (d.unsafe_neighbor(*it, a) == UNDEFINED) {
            return false;
          }
        }
      }
      return true;
    }

    template <typename Node,
              typename Iterator1,
              typename Iterator2,
              typename Iterator3>
    bool is_compatible(ActionDigraph<Node> const& d,
                       Iterator1                  first_node,
                       Iterator2                  last_node,
                       Iterator3                  first_rule,
                       Iterator3                  last_rule) {
      for (auto nit = first_node; nit != last_node; ++nit) {
        for (auto rit = first_rule; rit != last_rule; ++rit) {
          auto l = action_digraph_helper::follow_path_nc(
              d, *nit, rit->cbegin(), rit->cend());
          if (l == UNDEFINED) {
            return true;
          }
          ++rit;
          auto r = action_digraph_helper::follow_path_nc(
              d, *nit, rit->cbegin(), rit->cend());
          if (r == UNDEFINED) {
            return true;
          }
          if (l != r) {
            return false;
          }
        }
      }
      return true;
    }
  }  // namespace action_digraph

  //! Constructs a digraph from number of nodes and an \c initializer_list.
  //!
  //! This function constructs an ActionDigraph from its arguments whose
  //! out-degree is specified by the length of the first \c initializer_list
  //! in the 2nd parameter.
  //!
  //! \tparam T the type of the nodes of the digraph
  //!
  //! \param num_nodes the number of nodes in the digraph.
  //! \param il the out-neighbors of the digraph.
  //!
  //! \returns A value of type ActionDigraph.
  //!
  //! \throws LibsemigroupsException
  //! if ActionDigraph<T>::add_edge throws when adding edges from \p il.
  //!
  //! \complexity
  //! \f$O(mn)\f$ where \f$m\f$ is the length of \p il and \f$n\f$ is the
  //! parameter \p num_nodes.
  //!
  //! \par Example
  //! \code
  //! // Construct an action digraph with 5 nodes and 10 edges (7 specified)
  //! to_action_digraph<uint8_t>(
  //!     5, {{0, 0}, {1, 1}, {2}, {3, 3}});
  //! \endcode
  template <typename Node>
  ActionDigraph<Node>
  to_action_digraph(size_t num_nodes,
                    std::initializer_list<std::initializer_list<Node>> il) {
    ActionDigraph<Node> result(num_nodes, il.begin()->size());
    for (size_t i = 0; i < il.size(); ++i) {
      for (size_t j = 0; j < (il.begin() + i)->size(); ++j) {
        auto val = *((il.begin() + i)->begin() + j);
        if (val != UNDEFINED) {
          result.add_edge(i, *((il.begin() + i)->begin() + j), j);
        }
      }
    }
    return result;
  }

}  // namespace libsemigroups

#endif  // LIBSEMIGROUPS_DIGRAPH_HPP_
