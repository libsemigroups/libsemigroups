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

#ifndef LIBSEMIGROUPS_INCLUDE_DIGRAPH_HPP_
#define LIBSEMIGROUPS_INCLUDE_DIGRAPH_HPP_

#include <cstddef>  // for size_t

#include <algorithm>    // for uniform_int_distribution
#include <queue>        // for queue
#include <random>       // for mt19937
#include <stack>        // for stack
#include <type_traits>  // for is_integral, is_unsigned
#include <vector>       // for vector

#include "containers.hpp"               // for DynamicArray2
#include "forest.hpp"                   // for Forest
#include "int-range.hpp"                // for IntegralRange
#include "iterator.hpp"                 // for ConstIteratorStateless
#include "libsemigroups-debug.hpp"      // for LIBSEMIGROUPS_ASSERT
#include "libsemigroups-exception.hpp"  // for LIBSEMIGROUPS_EXCEPTION

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
      validate_node(i);
      validate_node(j);
      validate_label(lbl);
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
      validate_node(v);
      validate_label(lbl);
      return _dynamic_array_2.get(v, lbl);
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

    //! Returns a ActionDigraph::const_iterator_nodes (random access iterator)
    //! pointing one-past-the-last node of the digraph.
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
    //! pointing at the first neighbour of the node \p i.
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
      validate_node(i);
      return _dynamic_array_2.cbegin_row(i);
    }

    //! Returns a ActionDigraph::const_iterator_edges (random access iterator)
    //! pointing one-past-the-last neighbour of the node \p i.
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
      validate_node(i);
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
      validate_node(nd);
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
    //! has exactly out_degree() out-neighbours. In other words, if
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
    //! has exactly out_degree() out-neighbours. In other words, if
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
    //! has exactly out_degree() out-neighbours. In other words, if
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
    //! has exactly out_degree() out-neighbours. In other words, if
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
    //! has exactly out_degree() out-neighbours. In other words, if
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
    //! has exactly out_degree() out-neighbours. In other words, if
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
    //! has exactly out_degree() out-neighbours. In other words, if
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
    //! has exactly out_degree() out-neighbours. In other words, if
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
    //! has exactly out_degree() out-neighbours. In other words, if
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
    //! has exactly out_degree() out-neighbours. In other words, if
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

   private:
    ////////////////////////////////////////////////////////////////////////
    // ActionDigraph - validation - private
    ////////////////////////////////////////////////////////////////////////

    void validate_node(node_type v) const {
      if (v >= nr_nodes()) {
        LIBSEMIGROUPS_EXCEPTION("node value out of bounds, expected value in "
                                "the range [0, %d), got %d",
                                nr_nodes(),
                                v);
      }
    }

    void validate_label(label_type lbl) const {
      if (lbl >= out_degree()) {
        LIBSEMIGROUPS_EXCEPTION("label value out of bounds, expected value in "
                                "the range [0, %d), got %d",
                                out_degree(),
                                lbl);
      }
    }

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
      std::vector<std::vector<size_t>> _comps;
      std::vector<T>                   _id;
    } _scc;
  };

}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_INCLUDE_DIGRAPH_HPP_
