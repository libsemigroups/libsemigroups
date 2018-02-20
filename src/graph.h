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

#ifndef LIBSEMIGROUPS_SRC_GRAPH_H_
#define LIBSEMIGROUPS_SRC_GRAPH_H_

#include <algorithm>
#include <stack>
#include "recvec.h"

namespace libsemigroups {
  // TODO: proper commentary (makedoc)
  // TODO: maybe mutability
  //! Class for directed graphs
  //!
  //! This class represents (bounded) directed graphs. If the graph has
  //! \p n nodes, they are represented by the numbers {0, ..., n}.
  //! The underlying data structure is a RecVec
  //! which the Digraph contains. Digraph does not inherit from RecVec
  //! in order to control users altering the graphs in ways that
  //! affect what the graph knows about itself.
  //!
  //! These graphs are principally designed to be used as Cayley graphs,
  //! which has affected some design decisions. For example, the
  //! (maximum) degree of the graph must be specified at creation,
  //! though it can be increased later. This simplifies some code.
  //! Also, this is a fairly minimal implementation which
  //! only includes the functionality which is useful for Cayley graphs.
  template <typename TIntType> class Digraph {
    static_assert(std::is_integral<TIntType>(),
                  "TIntType is not an integer type!");

   public:
    //! A constant which represents undefined edges.
    //! This is used to "fill in" places
    //! in the underlying RecVec which do not yet represent edges.
    //! UNDEFINED is defined to be the maximum value of the integer type
    //! used to represent nodes.
    static const TIntType UNDEFINED;

    //! A constructor
    //!
    //! This constructor takes the maximum degree, \p degree, of the graph.
    explicit Digraph(TIntType degree, TIntType nr_vertices = 0)
        : _cc_ids(), _degree(degree), _has_scc(false),
          _next_edge_pos(nr_vertices), recvec(_degree, nr_vertices, UNDEFINED) {
    }

    Digraph &operator=(Digraph const &graph) = delete;

    //! Returns the node which is the end of the \p j-th edge of \p i.
    //!
    //! Can this change? Maybe. Should decide.
    TIntType inline get(TIntType i, TIntType j) {
      LIBSEMIGROUPS_ASSERT(i < nr_nodes());
      LIBSEMIGROUPS_ASSERT(j < _next_edge_pos[i]);
      return recvec.get(i, j);
    }

    //! Add some nodes to the graph
    //!
    //! Adds \p nr nodes to \c this.
    void inline add_nodes(size_t nr) {
      recvec.add_rows(nr);
      for (size_t i = 0; i < nr; ++i) {
        _next_edge_pos.push_back(0);
      }
      _has_scc = false;
    }

    //! Returns the number of nodes of \c this
    //!
    //! Returns the number of nodes of \c this
    TIntType inline nr_nodes() { return recvec.nr_rows(); }

    //! Add an edge to this digraph
    //!
    //! If \p i and \j are nodes which exist in \c this, then this function
    //! adds an edge from \p i to \p j.
    void inline add_edge(TIntType i, TIntType j) {
      LIBSEMIGROUPS_ASSERT(_next_edge_pos[i] < degree);
      LIBSEMIGROUPS_ASSERT(i < recvec.nr_rows());
      set(i, _next_edge_pos[i], j);
      ++_next_edge_pos[i];
    }

    //! Increase the degree of \c this
    //!
    //! Adds \p nr nodes to \c this
    void inline increase_degree(TIntType nr) {
      recvec.add_cols(nr);
      _degree += nr;
    }

    //! Returns the number of edges of \c this
    //!
    //! Returns the total number of edges of \c this
    TIntType inline nr_edges() {
      size_t edges = 0;
      for (size_t i : _next_edge_pos) {
        edges += i;
      }
      return edges;
    }

    //! Calculate the strongly connected components of \c this
    //!
    //! This function calculates the strongly connected components of \c this
    //! using Gabow's algorithm
    //! The implementation is strongly based on that in the Digraphs package
    //! (https://github.com/gap-packages/Digraphs)
    void gabow_scc() {
      _cc_ids = std::vector<TIntType>(recvec.nr_rows(), 0);
      size_t end1 = 0;
      std::vector<TIntType> stack1(recvec.nr_rows()+1, UNDEFINED);
      std::stack<TIntType> stack2;
      TIntType *frame = new TIntType[2 * recvec.nr_rows() + 1];

      TIntType w;
      size_t count = recvec.nr_rows();

      for (TIntType v = 0; v < recvec.nr_rows(); ++v) {
        if (_cc_ids[v] == 0) {
          TIntType level = 1;
          frame[0] = v;  // vertex
          frame[1] = 0;  // index
          stack1[++end1] = v;
          stack2.push(end1);
          _cc_ids[v] = end1;
          while (1) {
            if (frame[1] >= _next_edge_pos[frame[0]]) {
              if (stack2.top() == _cc_ids[frame[0]]) {
                stack2.pop();
                do {
                  w = stack1[end1--];
                  _cc_ids[w] = count;
                } while (w != frame[0]);
                count++;
              }
              level--;
              if (level == 0) {
                break;
              }
              frame -= 2;
            } else {
              w = recvec.get(frame[0], frame[1]);
              frame[1] += 1;
              TIntType idw = _cc_ids[w];

              if (idw == 0) {
                level++;
                frame += 2;
                frame[0] = w;  // vertex
                frame[1] = 0;  // index
                stack1[++end1] = w;
                stack2.push(end1);
                _cc_ids[w] = end1;
              } else {
                while (stack2.top() > idw) {
                  stack2.pop();
                }
              }
            }
          }
        }
      }

      for (size_t i = 0; i < recvec.nr_rows(); ++i) {
        _cc_ids[i] -= recvec.nr_rows();
      }

      _has_scc = true;

      delete [] frame;
    }

    //! Returns the id of the strongly connected component of a node
    //!
    //! Every node in \c this lies in a strongly connected component.
    //! This function returns the id number of the SCC containing \p node.
    //! If this has not been calculated, it will be at this point.
    TIntType get_scc_id(TIntType node) {
      LIBSEMIGROUPS_ASSERT(node < recvec.nr_rows());
      if (!_has_scc) {
        gabow_scc();
      }
     return _cc_ids[node];
    }

    //! Returns a bound on the degree of this graph
    //!
    //! This function returns the current bound on the degree of this graph.
    //! This is used to decide how many columns to store in the recvec.
    TIntType degree_bound() { return _degree; }

    //! Returns the maximum degree of this digraph
    //!
    //! This function returns the maximum degree of a node in \c this.
    TIntType max_degree() {
      return *std::max_element(_next_edge_pos.begin(), _next_edge_pos.end());
    }

   private:
    std::vector<TIntType> _cc_ids;
    TIntType _degree;
    bool _has_scc;
    std::vector<TIntType> _next_edge_pos;
    RecVec<TIntType> recvec;

    void inline set(TIntType i, TIntType j, TIntType k) {
      LIBSEMIGROUPS_ASSERT(i < nr_nodes());
      LIBSEMIGROUPS_ASSERT(j < _degree);
      LIBSEMIGROUPS_ASSERT(k < nr_nodes());
      recvec.set(i, j, k);
      _has_scc = false;
    }
  };

  template <typename TIntType>
  const TIntType
      Digraph<TIntType>::UNDEFINED = std::numeric_limits<TIntType>::max();
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_SRC_GRAPH_H_
