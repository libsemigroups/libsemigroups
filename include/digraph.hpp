//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2018 Finn Smith
// Copyright (C) 2018 James D. Mitchell
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

// TODO(FLS)
// 1. Complete the doc
// 2. Double-check that validate_whatever is used wherever it should be
// 3. More benchmarks (that don't seg fault)
// 4. Check for noexcept (that it's used correctly, and not missing somewhere
//    it could be used.

//! \file
//!
//! This file contains an implementation of out-regular digraph which represents
//! the action of a semigroup on a set.

#ifndef LIBSEMIGROUPS_INCLUDE_DIGRAPH_HPP_
#define LIBSEMIGROUPS_INCLUDE_DIGRAPH_HPP_

#include <queue>        // for queue
#include <random>       // for mt19937
#include <stack>        // for stack
#include <stddef.h>     // for size_t
#include <type_traits>  // for is_integral, is_unsigned
#include <vector>       // for vector

#include "containers.hpp"               // for RecVec
#include "forest.hpp"                   // for Forest
#include "iterator.hpp"                 // for iterator_base
#include "libsemigroups-debug.hpp"      // for LIBSEMIGROUPS_ASSERT
#include "libsemigroups-exception.hpp"  // for LIBSEMIGROUPS_EXCEPTION
#include "range.hpp"                    // for IntegralRange

namespace libsemigroups {
  //! Class for out-regular digraphs
  //!
  //! This class represents out-regular digraphs. If the digraph has \p n
  //! nodes, they are represented by the numbers {0, ..., n - 1}.
  //!
  //! These graphs are principally designed to be used those associated to the
  //! action of a semigroup. The template parameter \p TIntType should be an
  //! unsigned integer type, which is the type of nodes in the digraph.
  template <typename TIntType>
  class ActionDigraph {
    static_assert(std::is_integral<TIntType>(),
                  "TIntType is not an integer type!");
    static_assert(std::is_unsigned<TIntType>(), "TIntType is not unsigned!");

    ////////////////////////////////////////////////////////////////////////
    // ActionDigraph - iterator - private
    ////////////////////////////////////////////////////////////////////////

    struct iterator_methods_scc_roots {
      TIntType const& indirection(
          typename std::vector<std::vector<TIntType>>::const_iterator it)
          const {
        return *(*it).cbegin();
      }

      TIntType const*
      addressof(typename std::vector<std::vector<TIntType>>::const_iterator it)
          const {
        return &(*(*it).cbegin());
      }
    };

   public:
    ////////////////////////////////////////////////////////////////////////
    // ActionDigraph - typedefs - public
    ////////////////////////////////////////////////////////////////////////

    //! Alias for the type of nodes in a digraph.
    using node_type = TIntType;

    //! Alias for the type of edge labels in a digraph.
    using label_type = TIntType;

    //! Alias for the type of an index in a strongly connected component of
    //! a digraph.
    using scc_index_type = TIntType;

    //! Alias for the type of an iterator pointing to the nodes of a digraph.
    using const_iterator_nodes =
        typename IntegralRange<TIntType>::const_iterator;

    //! Alias for the type of an iterator pointing to the out-edges of a node in
    //! a digraph.
    using const_iterator_edges =
        typename internal::RecVec<TIntType>::const_iterator;

    //! Alias for the type of an iterator pointing to the nodes in a strongly
    //! connected component of a digraph.
    using const_iterator_scc = typename std::vector<TIntType>::const_iterator;

    //! Alias for the type of an iterator pointing to the strongly
    //! connected components of a digraph.
    using const_iterator_sccs =
        typename std::vector<std::vector<TIntType>>::const_iterator;

    //! Alias for the type of an iterator pointing to the roots of a strongly
    //! connected components of a digraph.
    using const_iterator_scc_roots
        = internal::iterator_base<std::vector<TIntType>,
                                  node_type const*,
                                  node_type const&,
                                  node_type,
                                  iterator_methods_scc_roots>;

    ////////////////////////////////////////////////////////////////////////
    // ActionDigraph - constructors + destructor - public
    ////////////////////////////////////////////////////////////////////////

    //! \name Constructors and destructor

    //! @{
    //! Constructor
    //!
    //! Constructs an ActionDigraph instance with \p nr_ndes nodes and
    //! out-degree \p dgree.
    explicit ActionDigraph(TIntType nr_ndes = 0, TIntType dgree = 0)
        : _degree(dgree),
          _nr_nodes(nr_ndes),
          _recvec(_degree, _nr_nodes, UNDEFINED),
          _scc_back_forest(),
          _scc_forest(),
          _scc() {}

    ActionDigraph(ActionDigraph const&) = default;
    ActionDigraph(ActionDigraph&&)      = default;
    ActionDigraph& operator=(ActionDigraph const&) = default;
    ActionDigraph& operator=(ActionDigraph&&) = default;
    ~ActionDigraph()                          = default;

    static ActionDigraph
    random(TIntType nr_ndes, TIntType dgree, std::mt19937 mt = std::mt19937()) {
      std::uniform_int_distribution<TIntType> dist(0, nr_ndes);
      ActionDigraph<TIntType>                 g(nr_ndes, dgree);
      LIBSEMIGROUPS_ASSERT(g._recvec.nr_rows() == nr_ndes);
      LIBSEMIGROUPS_ASSERT(g._recvec.nr_cols() == dgree);
      std::generate(g._recvec.begin(), g._recvec.end(), [&dist, &mt]() {
        return dist(mt);
      });
      return g;
    }

    //!@}

    ////////////////////////////////////////////////////////////////////////
    // ActionDigraph - modifiers - public
    ////////////////////////////////////////////////////////////////////////
    //! \name Modifiers

    //! @{
    //! Adds \p nr nodes to \c this.
    //!
    //! \libsemigroups_iterator
    //!
    //! \libsemigroups_no_exception
    void inline add_nodes(size_t nr) {
      if (nr > _recvec.nr_rows() - _nr_nodes) {
        _recvec.add_rows(nr - (_recvec.nr_rows() - _nr_nodes));
      }
      _nr_nodes += nr;
      reset();
    }

    //! Adds \p nr to the out-degree of \c this.
    //!
    //! \libsemigroups_iterator
    //!
    //! \libsemigroups_no_exception
    void inline add_to_out_degree(size_t nr) {
      if (nr > _recvec.nr_cols() - _degree) {
        _recvec.add_cols(nr - (_recvec.nr_cols() - _degree));
      }
      _degree += nr;
      reset();
    }

    //! Add an edge from \p i to \p j labelled \p lbl.
    //!
    //! If \p i and \p j are nodes in \c this, and \p lbl is in the range [0,
    //! out_degree()), then this method adds an edge edge from \p i to \p j
    //! labelled \p lbl.
    //!
    //! \libsemigroups_iterator
    //!
    //! \exception Throws a LibsemigroupsException if \p i, \p j, or \p lbl is
    //! not valid.
    void inline add_edge(node_type i, node_type j, label_type lbl) {
      validate_node(i);
      validate_node(j);
      validate_label(lbl);
      _recvec.set(i, lbl, j);
      reset();
    }

    //! Reserve capacity
    //!
    //! Ensures that \c this has capacity for \p nr_ndes nodes each with \p
    //! out_dgree out-edges.
    //!
    //! \libsemigroups_iterator
    //!
    //! \libsemigroups_no_exception
    void reserve(TIntType nr_ndes, TIntType out_dgree) const noexcept {
      _recvec.add_cols(out_dgree - _recvec.nr_cols());
      // TODO exception guarantee
      _recvec.add_rows(nr_ndes - _recvec.nr_rows());
    }
    //!@}

    ////////////////////////////////////////////////////////////////////////
    // ActionDigraph - nodes, neighbors, etc - public
    ////////////////////////////////////////////////////////////////////////

    //! \name Nodes, edges, neighbors

    //! @{
    //! Returns the node adjacent to \p v via the edge labelled \p lbl. If
    //! there is no such node, then Libsemigroups::UNDEFINED is returned.
    //!
    //! \exception Throws a LibsemigroupsException if \p v or \p lbl is not
    //! valid.
    TIntType inline neighbor(node_type v, label_type lbl) const {
      validate_node(v);
      validate_label(lbl);
      return _recvec.get(v, lbl);
    }

    //! Returns the number of nodes of \c this
    //!
    //! \libsemigroups_no_throe
    TIntType inline nr_nodes() const noexcept {
      return _nr_nodes;
    }

    //! Returns the number of edges of \c this
    //!
    //! Returns the total number of edges of \c this
    size_t nr_edges() const noexcept {
      return _recvec.nr_rows() * _recvec.nr_cols()
             - std::count(_recvec.cbegin(), _recvec.cend(), UNDEFINED);
    }

    //! Returns the out-degree of \c this.
    TIntType out_degree() const noexcept {
      return _degree;
    }

    //! Check every node has exactly out_degree() out-edges.
    bool validate() const noexcept {
      return nr_edges() == nr_nodes() * out_degree();
    }

    const_iterator_nodes cbegin_nodes() const {
      return IntegralRange<TIntType>(0, nr_nodes()).cbegin();
    }

    const_iterator_nodes cend_nodes() const {
      return IntegralRange<TIntType>(0, nr_nodes()).cend();
    }

    const_iterator_edges cbegin_edges(node_type i) const {
      validate_node(i);
      return _recvec.cbegin_row(i);
    }

    const_iterator_edges cend_edges(node_type i) const {
      validate_node(i);
      return _recvec.cend_row(i);
    }
    //!@}

    ////////////////////////////////////////////////////////////////////////
    // ActionDigraph - strongly connected components - public
    ////////////////////////////////////////////////////////////////////////

    //! \name Strongly connected components

    //! @{
    //! Returns the id of the strongly connected component of a node
    //!
    //! Every node in \c this lies in a strongly connected component (SCC).
    //! This function returns the id number of the SCC containing \p node.
    TIntType scc_id(node_type nd) const {
      validate_node(nd);
      gabow_scc();
      LIBSEMIGROUPS_ASSERT(nd < _scc._id.size());
      return _scc._id[nd];
    }

    //! Returns the number of strongly connected components in \p this.
    //!
    //! Every node in \c this lies in a strongly connected component (SCC).
    //! This function returns the id number of the SCC containing \p node.
    TIntType nr_scc() const {
      gabow_scc();
      return _scc._comps.size();
    }

    node_type root_of_scc(node_type nd) const {
      // nd is validated in scc_id
      return *cbegin_scc(scc_id(nd));
    }

    const_iterator_sccs cbegin_sccs() const {
      gabow_scc();
      return _scc._comps.cbegin();
    }

    const_iterator_sccs cend_sccs() const {
      gabow_scc();
      return _scc._comps.cend();
    }

    const_iterator_scc cbegin_scc(scc_index_type i) const {
      gabow_scc();
      validate_scc_index(i);
      return _scc._comps[i].cbegin();
    }

    const_iterator_scc cend_scc(scc_index_type i) const {
      gabow_scc();
      validate_scc_index(i);
      return _scc._comps[i].cend();
    }

    const_iterator_scc_roots cbegin_scc_roots() const {
      return const_iterator_scc_roots(cbegin_sccs());
    }

    const_iterator_scc_roots cend_scc_roots() const {
      return const_iterator_scc_roots(cend_sccs());
    }
    //!@}

    ////////////////////////////////////////////////////////////////////////
    // ActionDigraph - spanning forests - public
    ////////////////////////////////////////////////////////////////////////

    //! \name Spanning forests of strongly connected components

    //! @{
    //! Returns a Forest comprised of spanning trees for each SCC
    //!
    //! This function returns a Forest which contains the nodes of \c this,
    //! where the forest contains a spanning tree for each strongly connected
    //! component, rooted on the minimum element index of that component.
    Forest const& spanning_forest() const {
      if (!_scc_forest._defined) {
        // Validity checked in gabow_scc
        gabow_scc();

        std::vector<bool>    seen(nr_nodes(), false);
        std::queue<TIntType> queue;

        _scc_forest._forest.clear();
        _scc_forest._forest.add_nodes(nr_nodes());

        for (size_t i = 0; i < nr_scc(); ++i) {
          queue.push(_scc._comps[i][0]);
          seen[_scc._comps[i][0]] = true;
          do {
            size_t x = queue.front();
            for (size_t j = 0; j < _degree; ++j) {
              size_t y = _recvec.get(x, j);
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

    Forest const& reverse_spanning_forest() const {
      if (!_scc_back_forest._defined) {
        // Validity checked in gabow_scc
        gabow_scc();

        _scc_back_forest._forest.clear();
        _scc_back_forest._forest.add_nodes(nr_nodes());

        std::vector<std::vector<TIntType>> reverse_edges(
            nr_nodes(), std::vector<TIntType>());
        std::vector<std::vector<TIntType>> reverse_labels(
            nr_nodes(), std::vector<TIntType>());

        for (size_t i = 0; i < nr_nodes(); ++i) {
          size_t const scc_id_i = scc_id(i);
          for (size_t j = 0; j < out_degree(); ++j) {
            size_t const k = _recvec.get(i, j);
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
    //!@}

   private:
    ////////////////////////////////////////////////////////////////////////
    // ActionDigraph - validation - private
    ////////////////////////////////////////////////////////////////////////

    void validate_node(node_type v) const {
      if (v >= nr_nodes()) {
        LIBSEMIGROUPS_EXCEPTION("node value out of bounds, got "
                                << v << ", expected at most "
                                << nr_nodes() - 1);
      }
    }

    void validate_label(label_type lbl) const {
      if (lbl >= out_degree()) {
        LIBSEMIGROUPS_EXCEPTION("label value out of bounds, got "
                                << lbl << ", expected at most "
                                << out_degree() - 1);
      }
    }

    void validate_scc_index(scc_index_type i) const {
      if (i >= nr_scc()) {
        LIBSEMIGROUPS_EXCEPTION("scc index out of bounds, got "
                                << i << ", expected at most " << nr_scc() - 1);
      }
    }

    ////////////////////////////////////////////////////////////////////////
    // ActionDigraph - reset (after calling a modifier) - private
    ////////////////////////////////////////////////////////////////////////

    void reset() {
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
      _scc._comps.clear();
      _scc._id.assign(nr_nodes(), UNDEFINED);

      static std::stack<TIntType>                      stack1;
      static std::stack<TIntType>                      stack2;
      static std::stack<std::pair<TIntType, TIntType>> frame;
      static std::vector<TIntType>                     preorder;
      preorder.assign(nr_nodes(), UNDEFINED);
      LIBSEMIGROUPS_ASSERT(stack1.empty());
      LIBSEMIGROUPS_ASSERT(stack2.empty());
      LIBSEMIGROUPS_ASSERT(frame.empty());

      TIntType C     = 0;
      TIntType index = 0;

      for (TIntType w = 0; w < nr_nodes(); ++w) {
        if (_scc._id[w] == UNDEFINED) {
          frame.emplace(w, 0);
        dfs_start:
          LIBSEMIGROUPS_ASSERT(!frame.empty());
          TIntType v = frame.top().first;
          TIntType i = frame.top().second;

          preorder[v] = C++;
          stack1.push(v);
          stack2.push(v);
          for (; i < _degree; ++i) {
          dfs_end:
            LIBSEMIGROUPS_ASSERT(v < nr_nodes() && i < _degree);
            TIntType u = _recvec.get(v, i);
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
            TIntType x;
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

    TIntType                           _degree;
    TIntType                           _nr_nodes;
    mutable internal::RecVec<TIntType> _recvec;

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
      std::vector<TIntType>            _id;
    } _scc;
  };

}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_INCLUDE_DIGRAPH_HPP_
