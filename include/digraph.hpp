//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2018 Finn Smith
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

#ifndef LIBSEMIGROUPS_INCLUDE_DIGRAPH_HPP_
#define LIBSEMIGROUPS_INCLUDE_DIGRAPH_HPP_

#include <queue>        // for queue
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
  //! action of a semigroup.
  template <typename TIntType>
  class ActionDigraph {
    static_assert(std::is_integral<TIntType>(),
                  "TIntType is not an integer type!");
    static_assert(std::is_unsigned<TIntType>(), "TIntType is not unsigned!");

   public:
    using node_type          = TIntType;
    using label_type         = TIntType;
    using scc_index_type     = TIntType;
    using const_iterator_scc = typename std::vector<TIntType>::const_iterator;
    using const_iterator_sccs =
        typename std::vector<std::vector<TIntType>>::const_iterator;
    using const_iterator_nodes =
        typename IntegralRange<TIntType>::const_iterator;
    using const_iterator_edges =
        typename internal::RecVec<TIntType>::const_iterator;

   private:
    struct iterator_methods {
      node_type const& indirection(const_iterator_sccs it) const {
        return *(*it).cbegin();
      }

      node_type const* addressof(const_iterator_sccs it) const {
        return &(*(*it).cbegin());
      }
    };

   public:
    using const_iterator_scc_roots
        = internal::iterator_base<std::vector<TIntType>,
                                  node_type const*,
                                  node_type const&,
                                  node_type,
                                  iterator_methods>;
    //! A constructor
    //!
    //! This constructor takes the initial degree bound and the initial
    //! number of nodes of the graph.
    explicit ActionDigraph(TIntType nr_nodes = 0, TIntType degree = 0)
        : _degree(degree),
          _nr_nodes(nr_nodes),
          _recvec(degree, nr_nodes, UNDEFINED),
          _scc_back_forest(),
          _scc_forest(),
          _scc() {}

    ActionDigraph(ActionDigraph const&) = default;
    ActionDigraph(ActionDigraph&&)      = default;
    ActionDigraph& operator=(ActionDigraph const&) = default;
    ActionDigraph& operator=(ActionDigraph&&) = default;
    ~ActionDigraph()                          = default;

    //! Add some nodes to the graph
    //!
    //! Adds \p nr nodes to \c this.
    void inline add_nodes(size_t nr) {
      if (nr > _recvec.nr_rows() - _nr_nodes) {
        _recvec.add_rows(nr - (_recvec.nr_rows() - _nr_nodes));
      }
      _nr_nodes += nr;
      reset();
    }

    void inline add_to_out_degree(size_t nr) {
      if (nr > _recvec.nr_cols() - _degree) {
        _recvec.add_cols(nr - (_recvec.nr_cols() - _degree));
      }
      _degree += nr;
      reset();
    }

    void reserve(TIntType nr_ndes, TIntType out_dgree) const noexcept {
      _recvec.add_cols(out_dgree - _recvec.nr_cols());
      _recvec.add_rows(nr_ndes - _recvec.nr_rows());
    }

    //! Add an edge to this digraph
    //!
    //! If \p i and \p j are nodes in \c this, then this function
    //! adds an edge from \p i to \p j.
    void inline add_edge(node_type i, node_type j, label_type lbl) {
      if (i >= nr_nodes()) {
        LIBSEMIGROUPS_EXCEPTION("first node value out of range, got "
                                << i << ", expected less than " << nr_nodes());
      } else if (j >= nr_nodes()) {
        LIBSEMIGROUPS_EXCEPTION("second node value out of range, got "
                                << j << +", expected less than " << nr_nodes());
      } else if (lbl >= out_degree()) {
        LIBSEMIGROUPS_EXCEPTION("label value out of range, got "
                                << lbl << ", expected less than "
                                << out_degree());
      }
      _recvec.set(i, lbl, j);
      reset();
    }

    //! Returns the node which is the end of the \p j-th edge of \p i.
    //!
    //! Returns the thing in the (\p i, \p j)th position of the
    //! underlying RecVec, i.e. the \p j-th edge from node \p i.
    TIntType inline get(node_type i, label_type j) const {
      if (i >= nr_nodes()) {
        LIBSEMIGROUPS_EXCEPTION("node index out of range, found "
                                << i << " expected at most " << nr_nodes() - 1);
      } else if (j >= _degree) {
        LIBSEMIGROUPS_EXCEPTION("edge index out of range, found "
                                << j << " expected at most " << _degree - 1);
      }
      return _recvec.get(i, j);
    }

    //! Returns the number of nodes of \c this
    //!
    //! Returns the number of nodes of \c this
    TIntType inline nr_nodes() const noexcept {
      return _nr_nodes;
    }

    //! Returns the number of edges of \c this
    //!
    //! Returns the total number of edges of \c this
    TIntType nr_edges() const noexcept {
      return _recvec.nr_rows() * _recvec.nr_cols()
             - std::count(_recvec.cbegin(), _recvec.cend(), UNDEFINED);
    }

    bool validate() const noexcept {
      using diff_type = typename decltype(_recvec.cbegin())::difference_type;
      return std::count(_recvec.cbegin(), _recvec.cend(), UNDEFINED)
             == static_cast<diff_type>(_recvec.nr_cols() * (_recvec.nr_rows() - nr_nodes()) +
                                       nr_nodes() * (_recvec.nr_cols() - out_degree()));
    }

    TIntType out_degree() const noexcept {
      return _degree;
    }

    //! Returns the id of the strongly connected component of a node
    //!
    //! Every node in \c this lies in a strongly connected component.
    //! This function returns the id number of the SCC containing \p node.
    //! If this has not been calculated, it will be at this point.
    TIntType scc_id(node_type node) const {
      if (node >= nr_nodes()) {
        LIBSEMIGROUPS_EXCEPTION("node index out of range, found "
                                << node << " expected at most " << _degree - 1);
      }
      gabow_scc();
      LIBSEMIGROUPS_ASSERT(node < _scc._id.size());
      return _scc._id[node];
    }

    TIntType nr_scc() const {
      gabow_scc();
      return _scc._comps.size();
    }

    const_iterator_sccs cbegin_sccs() const {
      gabow_scc();
      return _scc._comps.cbegin();
    }

    const_iterator_sccs cend_sccs() const {
      gabow_scc();
      return _scc._comps.cend();
    }

    const_iterator_nodes cbegin() const {
      return IntegralRange<TIntType>(0, nr_nodes()).cbegin();
    }

    const_iterator_nodes cend() const {
      return IntegralRange<TIntType>(0, nr_nodes()).cend();
    }

    const_iterator_scc cbegin_scc(scc_index_type i) const {
      gabow_scc();
      return _scc._comps[i].cbegin();
    }

    const_iterator_scc cend_scc(scc_index_type i) const {
      gabow_scc();
      return _scc._comps[i].cend();
    }

    const_iterator_edges cbegin_edges(node_type i) const {
      return _recvec.cbegin_row(i);
    }

    const_iterator_edges cend_edges(node_type i) const {
      return _recvec.cend_row(i);
    }

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
              size_t y = get(x, j);
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
            size_t const k = get(i, j);
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

    node_type root_of_scc(node_type node) const {
      return *cbegin_scc(scc_id(node));
    }

    const_iterator_scc_roots cbegin_scc_roots() const {
      return const_iterator_scc_roots(cbegin_sccs());
    }

    const_iterator_scc_roots cend_scc_roots() const {
      return const_iterator_scc_roots(cend_sccs());
    }

   private:
    void reset() {
      _scc_back_forest._defined = false;
      _scc._defined             = false;
      _scc_forest._defined      = false;
    }

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
          do {
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
            } else {
              break;
            }
          } while (true);
        }
      }
      _scc._defined = true;
    }

    TIntType                   _degree;
    TIntType                   _nr_nodes;
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
