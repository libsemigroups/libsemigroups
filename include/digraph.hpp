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

#include <algorithm>
#include <queue>
#include <stack>

#include "containers.hpp"
#include "forest.hpp"
#include "range.hpp"
#include "libsemigroups-exception.hpp"

namespace libsemigroups {
  //! Class for directed Cayley graphs
  //!
  //! This class represents directed Cayley graphs. If the digraph has \p n
  //! nodes, they are represented by the numbers {0, ..., n}.  The underlying
  //! data structure is a RecVec which the Digraph contains. Digraph does not
  //! inherit from RecVec, in order to stop users altering the graphs in ways
  //! that affect what the graph knows about itself.
  //!
  //! These graphs are principally designed to be used as Cayley graphs, which
  //! has affected some design decisions. For example, the edges in a digraph
  //! cannot be deleted, as no need is seen for this.  This simplifies some of
  //! the code.  Also, this is a fairly minimal implementation which only
  //! includes the functionality which is useful for Cayley graphs.  When a
  //! digraph of this type is created, a bound on the degree must be specified.
  //! This can be increased later (e.g. if more generators are to be included
  //! in the Cayley digraph).
  template <typename TIntType>
  class ActionDigraph {
    static_assert(std::is_integral<TIntType>(),
                  "TIntType is not an integer type!");
    static_assert(std::is_unsigned<TIntType>(), "TIntType is not unsigned!");

   public:
    //! A constructor
    //!
    //! This constructor takes the initial degree bound and the initial
    //! number of nodes of the graph.
    explicit ActionDigraph(TIntType nr_nodes = 0)
        : _recvec(0, nr_nodes, UNDEFINED), _scc() {}

    // TODO: think about this
    // ActionDigraph &operator=(ActionDigraph const &graph) = delete;

    //! Add some nodes to the graph
    //!
    //! Adds \p nr nodes to \c this.
    void inline add_nodes(size_t nr) {
      _recvec.add_rows(nr);
      _has_root_paths = false;
      _scc._defined   = false;
    }

    //! Add an edge to this digraph
    //!
    //! If \p i and \p j are nodes in \c this, then this function
    //! adds an edge from \p i to \p j.
    void inline add_edge(TIntType i, TIntType lbl, TIntType j) {
      if (i >= nr_nodes()) {
        // TODO(FLS) reword as per new style
        throw LIBSEMIGROUPS_EXCEPTION("first argument larger than "
                                      "number of nodes - 1");
      } else if (j >= nr_nodes()) {
        // TODO(FLS) reword as per new style
        throw LIBSEMIGROUPS_EXCEPTION("second argument larger than "
                                      "number of nodes - 1");
      }
      if (lbl >= _recvec.nr_cols()) {
        _recvec.add_cols(lbl - _recvec.nr_cols() + 1);
      }
      _recvec.set(i, lbl, j);
      _has_root_paths = false;
      _scc._defined   = false;
    }

    //! Returns the node which is the end of the \p j-th edge of \p i.
    //!
    //! Returns the thing in the (\p i, \p j)th position of the
    //! underlying RecVec, i.e. the \p j-th edge from node \p i.
    TIntType inline get(TIntType i, TIntType j) const {
      if (i >= nr_nodes()) {
        throw LIBSEMIGROUPS_EXCEPTION(
            "node index out of range, found " + internal::to_string(i)
            + " expected at most " + internal::to_string(nr_nodes() - 1));
      } else if (j >= _recvec.nr_cols() || _recvec.get(i, j) == UNDEFINED) {
        throw LIBSEMIGROUPS_EXCEPTION("edge index out of range, edge "
                                      + internal::to_string(j)
                                      + " does not exist");
      }
      return _recvec.get(i, j);
    }

    //! Returns the number of nodes of \c this
    //!
    //! Returns the number of nodes of \c this
    TIntType inline nr_nodes() const {
      return _recvec.nr_rows();
    }

    //! Returns the number of edges of \c this
    //!
    //! Returns the total number of edges of \c this
    TIntType nr_edges() const {
      return nr_nodes() * _recvec.nr_cols()
             - std::count(_recvec.cbegin(), _recvec.cend(), UNDEFINED);
    }

    bool validate() const {
      return std::count(_recvec.cbegin(), _recvec.cend(), UNDEFINED) == 0;
    }

    //! Returns the id of the strongly connected component of a node
    //!
    //! Every node in \c this lies in a strongly connected component.
    //! This function returns the id number of the SCC containing \p node.
    //! If this has not been calculated, it will be at this point.
    TIntType scc_id(TIntType node) {
      if (node >= nr_nodes()) {
        // TODO(FLS) reword as per new style
        throw LIBSEMIGROUPS_EXCEPTION("argument larger than "
                                      "number of nodes - 1");
      }
      gabow_scc();
      LIBSEMIGROUPS_ASSERT(node < _scc._id.size());
      return _scc._id[node];
    }

    TIntType nr_scc() {
      gabow_scc();
      return _scc._comps.size();
    }

    typename std::vector<std::vector<TIntType>>::const_iterator cbegin_sccs() {
      gabow_scc();
      return _scc._comps.cbegin();
    }

    typename std::vector<std::vector<TIntType>>::const_iterator cend_sccs() {
      gabow_scc();
      return _scc._comps.cend();
    }

    using const_iterator_nodes = typename IntegralRange<TIntType>::const_iterator;

    const_iterator_nodes cbegin() {
      return IntegralRange<TIntType>(0, nr_nodes()).cbegin();
    }

    const_iterator_nodes cend() {
      return IntegralRange<TIntType>(0, nr_nodes()).cend();
    }

    //! Returns a Forest comprised of spanning trees for each SCC
    //!
    //! This function returns a Forest which contains the nodes of \c this,
    //! where the forest contains a spanning tree for each strongly connected
    //! component, rooted on the minimum element index of that component.
    Forest spanning_forest() {
      std::vector<bool>    seen(nr_nodes(), false);
      Forest               forest(nr_nodes());
      std::queue<TIntType> queue;
      gabow_scc();
      for (size_t i = 0; i < nr_scc(); ++i) {
        queue.push(_scc._comps[i][0]);
        seen[_scc._comps[i][0]] = true;
        do {
          size_t x = queue.front();
          for (size_t j = 0; j < _recvec.nr_cols(); ++j) {
            size_t y = get(x, j);
            if (!seen[y] && _scc._id[y] == _scc._id[x]) {
              forest.set(y, x, j);
              queue.push(y);
              seen[y] = true;
            }
          }
          queue.pop();
        } while (!queue.empty());
      }
      return forest;
    }

    std::vector<TIntType> const& get_root_path(TIntType pos) {
      if (!_has_root_paths) {
        compute_scc_root_paths();
      }
      return _root_paths[pos];
    }

   private:
    //! Calculate the strongly connected components of \c this
    //!
    //! This function calculates the strongly connected components of \c this
    //! using Gabow's algorithm
    //! The implementation is strongly based on that in the Digraphs package
    //! (https://github.com/gap-packages/Digraphs)
    void gabow_scc() const {
      if (_scc._defined) {
        return;
      } else if (!validate()) {
        throw LIBSEMIGROUPS_EXCEPTION("digraph not fully defined, can't find "
                                      "strongly connected components");
      }
      _scc._comps.clear();
      _scc._id.assign(nr_nodes(), UNDEFINED);

      size_t                                           deg = _recvec.nr_cols();
      static std::stack<TIntType>                      stack1;
      static std::stack<TIntType>                      stack2;
      static std::stack<std::pair<TIntType, TIntType>> frame;
      static std::vector<TIntType> preorder;
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
            for (; i < deg; ++i) {
            dfs_end:
              LIBSEMIGROUPS_ASSERT(v < nr_nodes() && i < deg);
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
                x = stack1.top();
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

    void compute_scc_root_paths() {
      std::vector<std::vector<TIntType>> paths(nr_nodes(),
                                               std::vector<TIntType>());
      std::vector<std::vector<TIntType>> reverse_edges(nr_nodes(),
                                                       std::vector<TIntType>());
      std::vector<std::vector<TIntType>> reverse_labels(
          nr_nodes(), std::vector<TIntType>());
      std::vector<bool>  seen(nr_nodes(), false);
      size_t             x;
      size_t             y;
      std::queue<size_t> queue;
      gabow_scc();
      for (size_t i = 0; i < nr_nodes(); ++i) {
        for (size_t j = 0; j < _recvec.nr_cols(); ++j) {
          reverse_edges[get(i, j)].push_back(i);
          reverse_labels[get(i, j)].push_back(j);
        }
      }

      for (size_t i = 0; i < nr_scc(); ++i) {
        queue = std::queue<size_t>();
        queue.push(_scc._comps[i][0]);
        seen[_scc._comps[i][0]] = true;
        while (!queue.empty()) {
          x = queue.front();
          for (size_t j = 0; j < reverse_edges[x].size(); ++j) {
            y = reverse_edges[x][j];
            if (!seen[y] && _scc._id[y] == _scc._id[x]) {
              queue.push(y);
              seen[y] = true;
              paths[y].push_back(reverse_labels[x][j]);
              paths[y].insert(paths[y].end(), paths[x].begin(), paths[x].end());
            }
          }
          queue.pop();
        }
      }
      _root_paths     = paths;
      _has_root_paths = true;
    }

    internal::RecVec<TIntType> _recvec;

    mutable struct SCC {
      SCC() : _comps(), _defined(false), _id() {}
      std::vector<std::vector<size_t>> _comps;
      bool                             _defined;
      std::vector<TIntType>            _id;
    } _scc;

    bool                               _has_root_paths;
    std::vector<std::vector<TIntType>> _root_paths;
  };
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_INCLUDE_DIGRAPH_HPP_
