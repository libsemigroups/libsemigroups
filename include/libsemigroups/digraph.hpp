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
#include <iterator>     // for forward_iterator_tag, distance
#include <queue>        // for queue
#include <random>       // for mt19937
#include <stack>        // for stack
#include <type_traits>  // for is_integral, is_unsigned
#include <utility>      // for pair
#include <vector>       // for vector

#include "containers.hpp"               // for DynamicArray2
#include "digraph-helper.hpp"           // for is_reachable
#include "forest.hpp"                   // for Forest
#include "int-range.hpp"                // for IntegralRange
#include "iterator.hpp"                 // for ConstIteratorStateless
#include "libsemigroups-debug.hpp"      // for LIBSEMIGROUPS_ASSERT
#include "libsemigroups-exception.hpp"  // for LIBSEMIGROUPS_EXCEPTION
#include "types.hpp"                    // for word_type
#include "word.hpp"                     // for number_of_words

namespace libsemigroups {

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
  class ActionDigraph final {
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

    //! Alias for the type of nodes in a digraph.
    using node_type = T;

    //! Alias for the type of edge labels in a digraph.
    using label_type = T;

    //! Alias for the type of an index in a strongly connected component of
    //! a digraph.
    using scc_index_type = T;

    //! Alias for the type of an iterator pointing to the nodes of a digraph.
    using const_iterator_nodes = typename IntegralRange<T>::const_iterator;

    //! Alias for the type of a reverse iterator pointing to the nodes of a
    //! digraph.
    using const_reverse_iterator_nodes =
        typename IntegralRange<T>::const_reverse_iterator;

    //! Alias for the type of an iterator pointing to the out-edges of a node in
    //! a digraph.
    using const_iterator_edges =
        typename detail::DynamicArray2<T>::const_iterator;

    //! Alias for the type of an iterator pointing to the nodes in a strongly
    //! connected component of a digraph.
    using const_iterator_scc = typename std::vector<T>::const_iterator;

    //! Alias for the type of an iterator pointing to the strongly
    //! connected components of a digraph.
    using const_iterator_sccs =
        typename std::vector<std::vector<T>>::const_iterator;

    //! Alias for the type of an iterator pointing to the roots of a strongly
    //! connected components of a digraph.
    using const_iterator_scc_roots
        = detail::ConstIteratorStateless<IteratorTraits>;

    ////////////////////////////////////////////////////////////////////////
    // ActionDigraph - constructors + destructor - public
    ////////////////////////////////////////////////////////////////////////

    //! A constructor that creates a DigraphAction instance representing the
    //! digraph of a semigroup action.
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
    explicit ActionDigraph(T m = 0, T n = 0)
        : _degree(n),
          _nr_nodes(m),
          _dynamic_array_2(_degree, _nr_nodes, UNDEFINED),
          _scc_back_forest(),
          _scc_forest(),
          _scc() {}

    //! Default copy constructor
    ActionDigraph(ActionDigraph const&) = default;

    //! Default move constructor
    ActionDigraph(ActionDigraph&&) = default;

    //! Default copy assignment constructor
    ActionDigraph& operator=(ActionDigraph const&) = default;

    //! Default move assignment constructor
    ActionDigraph& operator=(ActionDigraph&&) = default;

    ~ActionDigraph() = default;

    //! Constructs a random ActionDigraph from \p mt with the specified number
    //! of nodes and out-degree.
    //!
    //! \param m the number of nodes
    //! \param n the out-degree of every node
    //! \param mt a std::mt19937 used as a random source.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \par Complexity
    //! \f$O(mn)\f$ where \p m is the number of nodes, and \p n is
    //! the out-degree of the digraph.
    static ActionDigraph random(T m, T n, std::mt19937 mt = std::mt19937()) {
      std::uniform_int_distribution<T> dist(0, m - 1);
      ActionDigraph<T>                 g(m, n);
      LIBSEMIGROUPS_ASSERT(g._dynamic_array_2.nr_rows() == m);
      LIBSEMIGROUPS_ASSERT(g._dynamic_array_2.nr_cols() == n);
      std::generate(g._dynamic_array_2.begin(),
                    g._dynamic_array_2.end(),
                    [&dist, &mt]() { return dist(mt); });
      return g;
    }

    ////////////////////////////////////////////////////////////////////////
    // ActionDigraph - modifiers - public
    ////////////////////////////////////////////////////////////////////////

    //! Adds \p nr nodes to \c this.
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
    //! Linear in `nr_nodes() + nr`.
    //!
    //! \iterator_validity
    //! \iterator_invalid
    // Not noexcept because DynamicArray2::add_rows isn't.
    void inline add_nodes(size_t nr) {
      if (nr > _dynamic_array_2.nr_rows() - _nr_nodes) {
        _dynamic_array_2.add_rows(nr
                                  - (_dynamic_array_2.nr_rows() - _nr_nodes));
      }
      _nr_nodes += nr;
      reset();
    }

    //! Adds \p nr to the out-degree of \c this.
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
      if (nr > _dynamic_array_2.nr_cols() - _degree) {
        _dynamic_array_2.add_cols(nr - (_dynamic_array_2.nr_cols() - _degree));
      }
      _degree += nr;
      reset();
    }

    //! Add an edge from \p i to \p j labelled \p lbl.
    //! If \p i and \p j are nodes in \c this, and \p lbl is in the range [0,
    //! out_degree()), then this method adds an edge edge from \p i to \p j
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
      _dynamic_array_2.set(i, lbl, j);
      reset();
    }

    //! Ensures that \c this has capacity for \p m nodes each with \p
    //! \p n out-edges, but does not modify nr_nodes() or out_degree().
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
      _dynamic_array_2.add_cols(n - _dynamic_array_2.nr_cols());
      // What if add_cols throws, what guarantee can we offer then?
      _dynamic_array_2.add_rows(m - _dynamic_array_2.nr_rows());
    }

    ////////////////////////////////////////////////////////////////////////
    // ActionDigraph - nodes, neighbors, etc - public
    ////////////////////////////////////////////////////////////////////////

    //! Get the range of the edge with source node \p v and edge-label \p lbl.
    //!
    //! \param v the node
    //! \param lbl the label
    //!
    //! \returns
    //! Returns the node adjacent to \p v via the edge labelled \p lbl, or
    //! libsemigroups::UNDEFINED; both are values of type
    //! ActionDigraph::node_type.
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

    //! Get the range of the edge with source node \p v and edge-label \p lbl.
    //!
    //! \param v the node
    //! \param lbl the label
    //!
    //! \returns
    //! Returns the node adjacent to \p v via the edge labelled \p lbl, or
    //! libsemigroups::UNDEFINED; both are values of type
    //! ActionDigraph::node_type.
    //!
    //! \complexity
    //! Constant.
    //!
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
    //! \param v the node
    //! \param i the label
    //!
    //! \returns
    //! Returns a [std::pair](https://en.cppreference.com/w/cpp/utility/pair)
    //! \c x where:
    //! 1. \c x.second is the minimum value in the range \f$[i,
    //!    out_degree())\f$ such that neighbor(v, x.second) is not equal to
    //!    libsemigroups::UNDEFINED; and
    //! 2. \c x.first is adjacent to \p v via an edge labelled
    //!    \c x.second;
    //! If neighbor(v, i) equals libsemigroups::UNDEFINED for every value in
    //! the range \f$[i, out_degree())\f$, then \c x.first and \c x.second
    //! equal libsemigroups::UNDEFINED.
    //!
    //! \complexity
    //! At worst \f$O(n)\f$ where \f$n\f$ equals out_degree().
    //!
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
    //! \param v the node
    //! \param i the label
    //!
    //! \returns
    //! Returns a [std::pair](https://en.cppreference.com/w/cpp/utility/pair)
    //! \c x where:
    //! 1. \c x.second is the minimum value in the range \f$[i,
    //!    out_degree())\f$ such that neighbor(v, x.second) is not equal to
    //!    libsemigroups::UNDEFINED; and
    //! 2. \c x.first is adjacent to \p v via an edge labelled
    //!    \c x.second;
    //! If neighbor(v, i) equals libsemigroups::UNDEFINED for every value in
    //! the range \f$[i, out_degree())\f$, then \c x.first and \c x.second
    //! equal libsemigroups::UNDEFINED.
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

    //! Returns the number of nodes of \c this
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
    T inline nr_nodes() const noexcept {
      return _nr_nodes;
    }

    //! Returns the number of edges of \c this
    //!
    //! \returns
    //! The total number of edges, a value of type \c size_t.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! \f$O(mn)\f$ where \c m is nr_nodes() and \c n is out_degree().
    //!
    //! \par Parameters
    //! (None)
    // Not noexcept because std::count isn't
    size_t nr_edges() const {
      return _dynamic_array_2.nr_rows() * _dynamic_array_2.nr_cols()
             - std::count(
                 _dynamic_array_2.cbegin(), _dynamic_array_2.cend(), UNDEFINED);
    }

    //! Returns the out-degree of \c this.
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
    //! \f$O(mn)\f$ where \c m is nr_nodes() and \c n is out_degree().
    //!
    //! \par Parameters
    //! (None)
    bool validate() const noexcept {
      return nr_edges() == nr_nodes() * out_degree();
    }

    //! Returns a ActionDigraph::const_iterator_nodes (random access iterator)
    //! pointing at the first node of the digraph.
    //!
    //! \returns
    //! An ActionDigraph::const_iterator_nodes.
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
      return IntegralRange<T>(0, nr_nodes()).cbegin();
    }

    //! Returns a ActionDigraph::const_reverse_iterator_nodes (random access
    //! iterator) pointing at the last node of the digraph.
    //!
    //! \returns
    //! An ActionDigraph::const_reverse_iterator_nodes.
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
      return IntegralRange<T>(0, nr_nodes()).crbegin();
    }

    //! Returns a ActionDigraph::const_reverse_iterator_nodes (random access
    //! iterator) pointing one-past-the-first node of the digraph.
    //!
    //! \returns
    //! An ActionDigraph::const_reverse_iterator_nodes.
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
      return IntegralRange<T>(0, nr_nodes()).crend();
    }

    //! Returns a ActionDigraph::const_iterator_nodes (random access
    //! iterator) pointing one-past-the-last node of the digraph.
    //!
    //! \returns
    //! An ActionDigraph::const_iterator_nodes.
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
      return IntegralRange<T>(0, nr_nodes()).cend();
    }

    //! Returns a ActionDigraph::const_iterator_edges (random access iterator)
    //! pointing at the first neighbor of the node \p i.
    //!
    //! \param i a node in the digraph.
    //!
    //! \returns
    //! An ActionDigraph::const_iterator_edges.
    //!
    //! \throws LibsemigroupsException if \p i is not valid.
    //!
    //! \complexity
    //! Constant.
    // Not noexcept because validate_node isn't
    const_iterator_edges cbegin_edges(node_type i) const {
      action_digraph_helper::validate_node(*this, i);
      return _dynamic_array_2.cbegin_row(i);
    }

    //! Returns a ActionDigraph::const_iterator_edges (random access iterator)
    //! pointing one-past-the-last neighbor of the node \p i.
    //!
    //! \param i a node in the digraph.
    //!
    //! \returns
    //! An ActionDigraph::const_iterator_edges.
    //!
    //! \throws LibsemigroupsException if \p i is not valid.
    //!
    //! \complexity
    //! Constant.
    // Not noexcept because validate_node isn't
    const_iterator_edges cend_edges(node_type i) const {
      action_digraph_helper::validate_node(*this, i);
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
    //! At most \f$O(mn)\f$ where \c m is nr_nodes() and \c n is out_degree().
    // Not noexcept because validate_node isn't
    scc_index_type scc_id(node_type nd) const {
      action_digraph_helper::validate_node(*this, nd);
      gabow_scc();
      LIBSEMIGROUPS_ASSERT(nd < _scc._id.size());
      return _scc._id[nd];
    }

    //! Returns the number of strongly connected components in \p this.
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
    //! At most \f$O(mn)\f$ where \c m is nr_nodes() and \c n is out_degree().
    //!
    //! \par Parameters
    //! (None)
    // Not noexcept because gabow_scc isn't
    size_t nr_scc() const {
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
    //! ActionDigraph::node_type.
    //!
    //! \throws LibsemigroupsException if it is not the case that every node
    //! has exactly out_degree() out-neighbors. In other words, if
    //! neighbor() is libsemigroups::UNDEFINED for any node \c nd and
    //! any label \c lbl.
    //! \basic_guarantee
    //!
    //! \complexity
    //! At most \f$O(mn)\f$ where \c m is nr_nodes() and \c n is out_degree().
    // Not noexcept because scc_id isn't
    node_type root_of_scc(node_type nd) const {
      // nd is validated in scc_id
      return *cbegin_scc(scc_id(nd));
    }

    //! Returns an iterator pointing to the vector of nodes in the first scc.
    //!
    //! \returns
    //! A ActionDigraph::const_iterator_sccs.
    //!
    //! \throws LibsemigroupsException if it is not the case that every node
    //! has exactly out_degree() out-neighbors. In other words, if
    //! neighbor() is libsemigroups::UNDEFINED for any node \c nd and
    //! any label \c lbl.
    //! \basic_guarantee
    //!
    //! \complexity
    //! At most \f$O(mn)\f$ where \c m is nr_nodes() and \c n is out_degree().
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
    //! A ActionDigraph::const_iterator_sccs.
    //!
    //! \throws LibsemigroupsException if it is not the case that every node
    //! has exactly out_degree() out-neighbors. In other words, if
    //! neighbor() is libsemigroups::UNDEFINED for any node \c nd and
    //! any label \c lbl.
    //! \basic_guarantee
    //!
    //! \complexity
    //! At most \f$O(mn)\f$ where \c m is nr_nodes() and \c n is out_degree().
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
    //! A ActionDigraph::const_iterator_scc.
    //!
    //! \throws LibsemigroupsException if it is not the case that every node
    //! has exactly out_degree() out-neighbors. In other words, if
    //! neighbor() is libsemigroups::UNDEFINED for any node \c nd and
    //! any label \c lbl.
    //!
    //! \throws LibsemigroupsException if \p i is not in the range \c 0 to \c
    //! nr_scc() - 1.
    //!
    //! \complexity
    //! At most \f$O(mn)\f$ where \c m is nr_nodes() and \c n is out_degree().
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
    //! A ActionDigraph::const_iterator_scc.
    //!
    //! \throws LibsemigroupsException if it is not the case that every node
    //! has exactly out_degree() out-neighbors. In other words, if
    //! neighbor() is libsemigroups::UNDEFINED for any node \c nd and
    //! any label \c lbl.
    //!
    //! \throws LibsemigroupsException if \p i is not in the range \c 0 to \c
    //! nr_scc() - 1.
    //!
    //! \complexity
    //! At most \f$O(mn)\f$ where \c m is nr_nodes() and \c n is out_degree().
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
    //! A ActionDigraph::const_iterator_scc_roots.
    //!
    //! \throws LibsemigroupsException if it is not the case that every node
    //! has exactly out_degree() out-neighbors. In other words, if
    //! neighbor() is libsemigroups::UNDEFINED for any node \c nd and
    //! any label \c lbl. \basic_guarantee
    //!
    //! \complexity
    //! At most \f$O(mn)\f$ where \c m is nr_nodes() and \c n is out_degree().
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
    //! A ActionDigraph::const_iterator_scc_roots.
    //!
    //! \throws LibsemigroupsException if it is not the case that every node
    //! has exactly out_degree() out-neighbors. In other words, if
    //! neighbor() is libsemigroups::UNDEFINED for any node \c nd and
    //! any label \c lbl. \basic_guarantee
    //!
    //! \complexity
    //! At most \f$O(mn)\f$ where \c m is nr_nodes() and \c n is out_degree().
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

    //! Returns a libsemigroups::Forest comprised of spanning trees for each
    //! scc of \c this, rooted on the minimum node of that component, with
    //! edges oriented away from the root.
    //!
    //! \returns
    //! A const reference to a libsemigroups::Forest.
    //!
    //! \throws LibsemigroupsException if it is not the case that every node
    //! has exactly out_degree() out-neighbors. In other words, if
    //! neighbor() is libsemigroups::UNDEFINED for any node \c nd and
    //! any label \c lbl. \basic_guarantee
    //!
    //! \complexity
    //! At most \f$O(mn)\f$ where \c m is nr_nodes() and \c n is out_degree().
    //!
    //! \par Parameters
    //! (None)
    Forest const& spanning_forest() const {
      if (!_scc_forest._defined) {
        // Validity checked in gabow_scc
        gabow_scc();

        std::vector<bool> seen(nr_nodes(), false);
        std::queue<T>     queue;

        _scc_forest._forest.clear();
        _scc_forest._forest.add_nodes(nr_nodes());

        for (size_t i = 0; i < nr_scc(); ++i) {
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

    //! Returns a libsemigroups::Forest comprised of spanning trees for each
    //! scc of \c this, rooted on the minimum node of that component, with
    //! edges oriented towards the root.
    //!
    //! \returns
    //! A const reference to a libsemigroups::Forest.
    //!
    //! \throws LibsemigroupsException if it is not the case that every node
    //! has exactly out_degree() out-neighbors. In other words, if
    //! neighbor() is libsemigroups::UNDEFINED for any node \c nd and
    //! any label \c lbl. \basic_guarantee
    //!
    //! \complexity
    //! At most \f$O(mn)\f$ where \c m is nr_nodes() and \c n is out_degree().
    //!
    //! \par Parameters
    //! (None)
    Forest const& reverse_spanning_forest() const {
      if (!_scc_back_forest._defined) {
        // Validity checked in gabow_scc
        gabow_scc();

        _scc_back_forest._forest.clear();
        _scc_back_forest._forest.add_nodes(nr_nodes());

        std::vector<std::vector<T>> reverse_edges(nr_nodes(), std::vector<T>());
        std::vector<std::vector<T>> reverse_labels(nr_nodes(),
                                                   std::vector<T>());

        for (size_t i = 0; i < nr_nodes(); ++i) {
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
        std::vector<bool>  seen(nr_nodes(), false);

        for (size_t i = 0; i < nr_scc(); ++i) {
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

    //! No doc
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
      const_panilo_iterator() = default;
      //! No doc
      const_panilo_iterator(const_panilo_iterator const&) = default;
      //! No doc
      const_panilo_iterator(const_panilo_iterator&&) = default;
      //! No doc
      const_panilo_iterator& operator=(const_panilo_iterator const&) = default;
      //! No doc
      const_panilo_iterator& operator=(const_panilo_iterator&&) = default;
      //! No doc
      ~const_panilo_iterator() = default;

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
        if (_digraph != nullptr && _min < _max) {
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

    //! Returns a forward iterator pointing to a pair consisting of the edge
    //! labels of the first path (in lexicographical order) starting at
    //! \p source with length in the range \f$[min, max)\f$ and the last node
    //! of that path.
    //!
    //! PANILO = Path And Node In Lexicographical Order
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
    //! \c it->first, a value of ActionDigraph::node_type.
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
    const_panilo_iterator cbegin_panilo(node_type const source,
                                        size_t const    min = 0,
                                        size_t const    max
                                        = POSITIVE_INFINITY) const {
      action_digraph_helper::validate_node(*this, source);
      return const_panilo_iterator(this, source, min, max);
    }

    //! Returns a forward iterator pointing to one after the last path from any
    //! node in the digraph.
    //!
    //! The iterator returned by this function may still dereferencable and
    //! incrementable, but may not point to a path in the correct range.
    //!
    //! \sa cbegin_panilo
    // not noexcept because constructors of const_panilo_iterator aren't
    const_panilo_iterator cend_panilo() const {
      return const_panilo_iterator(nullptr, 0, 0, 0);
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
    //! No doc
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

    //! Returns a forward iterator pointing to a pair consisting of the edge
    //! labels of the first path (in short-lex order) starting at \p source
    //! with length in the range \f$[min, max)\f$ and the last node of that
    //! path.
    //!
    //! PANISLO = Path And Node In Short-Lex Order
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
    //! \c it->first, a value of ActionDigraph::node_type.
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
    // TODO example and what is the complexity?
    // not noexcept because const_panislo_iterator constructors aren't
    const_panislo_iterator cbegin_panislo(node_type const source,
                                          size_t const    min = 0,
                                          size_t const    max
                                          = POSITIVE_INFINITY) const {
      action_digraph_helper::validate_node(*this, source);
      return const_panislo_iterator(this, source, min, max);
    }

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
        const_reference operator()(internal_iterator_type const& it) const
            noexcept {
          return it->first;
        }
      };

      struct AddressOf {
        //! No doc
        const_pointer operator()(internal_iterator_type const& it) const
            noexcept {
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
    //! No doc
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

    //! Returns a forward iterator pointing to the edge labels of the first
    //! path (in lexicographical order) starting at \p source with length in the
    //! range \f$[min, max)\f$.
    //!
    //! PILO = Paths In Lexicographical Order
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
    const_pilo_iterator cbegin_pilo(node_type const source,
                                    size_t const    min = 0,
                                    size_t const    max
                                    = POSITIVE_INFINITY) const {
      action_digraph_helper::validate_node(*this, source);
      return const_pilo_iterator(cbegin_panilo(source, min, max));
    }

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
    //! No doc
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

    //! Returns a forward iterator pointing to the edge labels of the first
    //! path (in short-lex order) starting at \p source with length in the range
    //! \f$[min, max)\f$.
    //!
    //! PISLO = Paths In Short-Lex Order
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
    //! libsemigroups::word_type consisting of the edge labels of the first
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
    const_pislo_iterator cbegin_pislo(node_type const source,
                                      size_t const    min = 0,
                                      size_t const    max
                                      = POSITIVE_INFINITY) const {
      action_digraph_helper::validate_node(*this, source);
      return const_pislo_iterator(cbegin_panislo(source, min, max));
    }

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
      const_pstilo_iterator() = default;
      //! No doc
      const_pstilo_iterator(const_pstilo_iterator const&) = default;
      //! No doc
      const_pstilo_iterator(const_pstilo_iterator&&) = default;
      //! No doc
      const_pstilo_iterator& operator=(const_pstilo_iterator const&) = default;
      //! No doc
      const_pstilo_iterator& operator=(const_pstilo_iterator&&) = default;
      //! No doc
      ~const_pstilo_iterator() = default;

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
        if (_digraph != nullptr && _min < _max && source != target) {
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
              _digraph->nr_nodes(), std::vector<node_type>({}));
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

          _can_reach_target.resize(_digraph->nr_nodes(), false);
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

    //! Returns a forward iterator pointing to the edge labels of the first
    //! path (in lexicographical order) starting at the node \p source and
    //! ending at the node \p target with length in the range \f$[min, max)\f$.
    //!
    //! PSTILO = Path Source Target In Lexicographical Order
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
    const_pstilo_iterator cbegin_pstilo(node_type const source,
                                        node_type const target,
                                        size_t const    min = 0,
                                        size_t const    max
                                        = POSITIVE_INFINITY) const {
      // source & target are validated in is_reachable.
      if (!action_digraph_helper::is_reachable(*this, source, target)) {
        return cend_pstilo();
      }
      return const_pstilo_iterator(this, source, target, min, max);
    }

    //! Returns a forward iterator pointing to one after the last path from any
    //! node in the digraph.
    //!
    //! The iterator returned by this function may still dereferencable and
    //! incrementable, but may not point to a path in the correct range.
    //!
    //! \sa cbegin_pstilo
    // not noexcept because const_pstilo_iterator constructors aren't
    const_pstilo_iterator cend_pstilo() const {
      return const_pstilo_iterator(nullptr, 0, 0, 0, 0);
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
        const_reference operator()(state_type&,
                                   internal_iterator_type const& it) const
            noexcept {
          return it->first;
        }
      };

      struct AddressOf {
        //! No doc
        const_pointer operator()(state_type&,
                                 internal_iterator_type const& it) const
            noexcept {
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
    //! No doc
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

    //! Returns a forward iterator pointing to the edge labels of the first
    //! path (in short-lex order) starting at the node \p source and ending
    //! at the node \p target with length in the range \f$[min, max)\f$.
    //!
    //! PSTISLO = Path Source Target In Short Lex Order
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
    const_pstislo_iterator cbegin_pstislo(node_type const source,
                                          node_type const target,
                                          size_t const    min = 0,
                                          size_t const    max
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

    //! Returns the number of paths between a pair of nodes with length in a
    //! given range.
    //!
    //! \param source the first node
    //! \param target the last node
    //! \param min the minimum length of a path
    //! \param max the maximum length of a path
    //!
    //! \returns
    //! A value of type `size_t`.
    //!
    //! \throws LibsemigroupsException if \p source or \p target is not a node
    //! in the digraph.
    // not noexcept because cbegin_pstilo isn't
    size_t number_of_paths(node_type const source,
                           node_type const target,
                           size_t const    min,
                           size_t const    max) const {
      // TODO use adjacency matrix if nr_nodes is small enough
      action_digraph_helper::validate_node(*this, source);
      action_digraph_helper::validate_node(*this, target);
      if (!action_digraph_helper::is_reachable(*this, source, target)) {
        return 0;
      }
      return std::distance(cbegin_pstilo(source, target, min, max),
                           cend_pstilo());
    }

    //! Returns the number of paths starting at a given node with length in a
    //! given range.
    //!
    //! \param source the first node
    //! \param min the minimum length of a path
    //! \param max the maximum length of a path
    //!
    //! \returns
    //! A value of type `size_t`.
    //!
    //! \throws LibsemigroupsException if \p source is not a node
    //! in the digraph.
    // not noexcept because cbegin_panilo isn't
    size_t number_of_paths(node_type const source,
                           size_t const    min,
                           size_t const    max) const {
      // TODO use adjacency matrix if nr_nodes is small enough
      action_digraph_helper::validate_node(*this, source);
      if (validate()) {
        return (max == POSITIVE_INFINITY
                    ? POSITIVE_INFINITY
                    : number_of_words(out_degree(), min, max));
      } else if (!action_digraph_helper::is_acyclic(*this, source)
                 && max == POSITIVE_INFINITY) {
        return POSITIVE_INFINITY;
      }
      return std::distance(cbegin_panilo(source, min, max), cend_panilo());
    }

   private:
    ////////////////////////////////////////////////////////////////////////
    // ActionDigraph - validation - private
    ////////////////////////////////////////////////////////////////////////

    void validate_scc_index(scc_index_type i) const {
      if (i >= nr_scc()) {
        LIBSEMIGROUPS_EXCEPTION("strong component index out of bounds, "
                                "expected value in the range [0, %d), got %d",
                                nr_scc(),
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
      preorder.assign(nr_nodes(), UNDEFINED);
      LIBSEMIGROUPS_ASSERT(stack1.empty());
      LIBSEMIGROUPS_ASSERT(stack2.empty());
      LIBSEMIGROUPS_ASSERT(frame.empty());

      _scc._comps.clear();
      _scc._id.assign(nr_nodes(), UNDEFINED);

      T C     = 0;
      T index = 0;

      for (T w = 0; w < nr_nodes(); ++w) {
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
            LIBSEMIGROUPS_ASSERT(v < nr_nodes() && i < _degree);
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

}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_DIGRAPH_HPP_
