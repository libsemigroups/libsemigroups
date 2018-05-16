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

#include "recvec.h"
#include "libsemigroups-exception.h"
#include <algorithm>
#include <memory>
#include <stack>

namespace libsemigroups {
  //! Class for directed graphs
  //!
  //! This class represents directed graphs. If the graph has
  //! \p n nodes, they are represented by the numbers {0, ..., n}.
  //! The underlying data structure is a RecVec
  //! which the Digraph contains. Digraph does not inherit from RecVec,
  //! in order to stop users altering the graphs in ways that
  //! affect what the graph knows about itself.
  //!
  //! These graphs are principally designed to be used as Cayley graphs,
  //! which has affected some design decisions. For example, the
  //! edges in a digraph cannot be deleted, as no need is seen for this.
  //! This simplifies some of the code.
  //! Also, this is a fairly minimal implementation which
  //! only includes the functionality which is useful for Cayley graphs.
  template <typename TIntType> class Digraph {
    static_assert(std::is_integral<TIntType>(),
                  "TIntType is not an integer type!");
    static_assert(std::is_unsigned<TIntType>(),
                  "TIntType is not unsigned!");

   public:
    //! A constant which represents undefined edges.
    //! This is used to "fill in" places
    //! in the underlying RecVec which do not yet represent edges.
    //! UNDEFINED is defined to be the maximum value of the integer type
    //! used to represent nodes.
    static const TIntType UNDEFINED;

    //! A constructor
    //!
    //! This constructor takes the initial number of vertices of the graph.
    explicit Digraph(TIntType nr_vertices = 0)
        : _next_edge_pos(nr_vertices),
          _cc_ids(),
          _has_scc(false),
          _recvec(0, nr_vertices, UNDEFINED) {}

    Digraph &operator=(Digraph const &graph) = delete;

    //! Returns the node which is the end of the \p j-th edge of \p i.
    //!
    //! Returns the thing in the (\p i, \p j)th position of the
    //! underlying RecVec, i.e. the \p j-th edge from node \p i.
    TIntType inline get(TIntType i, TIntType j) {
      if (i >= nr_nodes()) {
        throw LibsemigroupsException("get: first argument larger than "
                                     "number of nodes - 1");
      }
      if(j >= _next_edge_pos[i]) {
        throw LibsemigroupsException("get: second argument larger than "
                                     "number of edges from node - 1");
      }
      return _recvec.get(i, j);
    }

    //! Add some nodes to the graph
    //!
    //! Adds \p nr nodes to \c this.
    void inline add_nodes(size_t nr) {
      _recvec.add_rows(nr);
      for (size_t i = 0; i < nr; ++i) {
        _next_edge_pos.push_back(0);
      }
      _has_scc = false;
    }

    //! Returns the number of nodes of \c this
    //!
    //! Returns the number of nodes of \c this
    TIntType inline nr_nodes() {
      return _recvec.nr_rows();
    }

    //! Add an edge to this digraph
    //!
    //! If \p i and \p j are nodes in \c this, then this function
    //! adds an edge from \p i to \p j.
    void inline add_edge(TIntType i, TIntType j) {
      if (i >= nr_nodes()) {
        throw LibsemigroupsException("add_edge: first argument larger than "
                                     "number of nodes - 1");
      }
      if (j >= nr_nodes()) {
        throw LibsemigroupsException("add_edge: second argument larger than "
                                     "number of nodes - 1");
      }
      set(i, _next_edge_pos[i], j);
      ++_next_edge_pos[i];
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
      _cc_ids                    = std::vector<TIntType>(_recvec.nr_rows(), 0);
      size_t                end1 = 0;
      std::vector<TIntType> stack1(_recvec.nr_rows() + 1, UNDEFINED);
      std::stack<TIntType>  stack2;
      std::vector<TIntType> framevec
          = std::vector<TIntType>(2 * nr_nodes() + 1);
      auto frame = framevec.begin();

      TIntType w;
      size_t   count = nr_nodes();

      for (TIntType v = 0; v < nr_nodes(); ++v) {
        if (_cc_ids[v] == 0) {
          TIntType level = 1;
          frame[0]       = v;
          frame[1]       = 0;
          stack1[++end1] = v;
          stack2.push(end1);
          _cc_ids[v] = end1;
          while (1) {
            if (frame[1] >= _next_edge_pos[frame[0]]) {
              if (stack2.top() == _cc_ids[frame[0]]) {
                stack2.pop();
                do {
                  w          = stack1[end1--];
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
              w = get(frame[0], frame[1]);
              frame[1] += 1;
              TIntType idw = _cc_ids[w];

              if (idw == 0) {
                level++;
                frame += 2;
                frame[0]       = w;
                frame[1]       = 0;
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

      for (size_t i = 0; i < nr_nodes(); ++i) {
        _cc_ids[i] -= nr_nodes();
      }
      _has_scc = true;
    }

    //! Returns the id of the strongly connected component of a node
    //!
    //! Every node in \c this lies in a strongly connected component.
    //! This function returns the id number of the SCC containing \p node.
    //! If this has not been calculated, it will be at this point.
    TIntType get_scc_id(TIntType node) {
      if (node >= nr_nodes()) {
        throw LibsemigroupsException("get_scc_id: first argument larger than "
                                     "number of nodes - 1");
      }
      if (!_has_scc) {
        gabow_scc();
      }
      return _cc_ids[node];
    }

    //! Returns the maximum degree of this digraph
    //!
    //! This function returns the maximum degree of a node in \c this.
    TIntType max_degree() {
      if (nr_nodes() == 0) {
        return 0;
      }
      return *std::max_element(_next_edge_pos.begin(), _next_edge_pos.end());
    }

   protected:
    //! A vector containing the next edge positions for each node
    std::vector<TIntType> _next_edge_pos;

   private:
    //! Sets a value in the underlying RecVec
    //!
    //! Sets the (\p i, \p j)th position of the underlying recvec to \p k.
    //! This will increase the number of columns if there are not enough.
    //! It will not increase the number of rows though!
    //! Assert everything near any calls to this function.
    void inline set(TIntType i, TIntType j, TIntType k) {
      LIBSEMIGROUPS_ASSERT(i < nr_nodes());
      LIBSEMIGROUPS_ASSERT(k < nr_nodes());

      // col indexing starts at 0, nr_cols starts at 1
      size_t nr_cols = _recvec.nr_cols();
      if (j >= nr_cols) {
        _recvec.add_cols(j - nr_cols + 1);
      }

      _recvec.set(i, j, k);
      _has_scc = false;
    }

    //! A vector containing the id of the SCC of each node
    std::vector<TIntType> _cc_ids;

    //! A boolean flag indicating whether the SCCs of \p this are known
    bool _has_scc;

    //! A RecVec<TIntType> which is the underlying data structure for \p this
    RecVec<TIntType> _recvec;
  };

  //! Class for bounded out-degree directed graphs
  //!
  //! This class represents directed graphs which have (known) bounded
  //! out-degree. The motivation behind these graphs are Cayley graphs,
  //! which have constant out-degree for a given set of generators.
  //!
  //! The maximum desired degree must be provided on creation,
  //! but can be increased later (for example if you wished to add generators
  //! to a Cayley graph).
  template <typename TIntType>
  class BoundedOutDegreeDigraph : public Digraph<TIntType> {
   public:
    //! The type of digraph that \p this inherits from
    //!
    //! BoundedOutDegreeDigraph<TIntType> is a subclass of
    //! Digraph<TIntType>. This member stores the type Digraph<TIntType>
    //! so that members of the superclass can be accessed easily.
    typedef Digraph<TIntType> base_graph_type;

    //! A constructor
    //!
    //! This constructor takes the maximum degree, \p degree, of the graph.
    //! This can be increased later, but must be increased before you try
    //! to add an edge too many to a vertex!
    explicit BoundedOutDegreeDigraph(TIntType degree, TIntType nr_vertices = 0)
        : base_graph_type(nr_vertices), _degree(degree) {}

    //! Add an edge to this digraph
    //!
    //! If \p i and \p j are nodes which exist in \c this, then this function
    //! adds an edge from \p i to \p j.
    void inline add_edge(TIntType i, TIntType j) {
      if(base_graph_type::_next_edge_pos[i] >= _degree){
        throw LibsemigroupsException("add_edge: adding an edge would increase "
                                     "the degree past the degree bound");
      }
      Digraph<TIntType>::add_edge(i, j);
    }

    //! Increase the degree of \c this
    //!
    //! Increases the maximum degree of \c this by \p nr.
    //! This function must be called before you attempt to add an edge
    //! to a vertex and increase its degree past \c degree_bound.
    void inline increase_degree(TIntType nr) {
      base_graph_type::_recvec.add_cols(nr);
      _degree += nr;
    }

    //! Returns a bound on the degree of this graph
    //!
    //! This function returns the current bound on the degree of this graph.
    //! This is used to decide how many columns to store in the RecVec.
    TIntType degree_bound() {
      return _degree;
    }

   private:
    TIntType _degree;
  };

  template <typename TIntType>
  const TIntType Digraph<TIntType>::UNDEFINED
      = std::numeric_limits<TIntType>::max();
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_SRC_GRAPH_H_
