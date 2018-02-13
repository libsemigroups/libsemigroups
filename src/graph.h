//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2017 James D. Mitchell
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

#include <iostream>
#include <stack>
#include "recvec.h"

namespace libsemigroups {
  // Represents vertices as rows
  // There is an edge (i, j) in the graph iff j occurs in row i
  //
  // TODO: proper commentary (makedoc)
  // TODO: maybe mutability
  class Graph : public RecVec<size_t> {
    typedef RecVec<size_t, std::allocator<size_t>> base_recvec;

   public:
    // if we have graphs with max of size_t vertices we have bigger problems
    static const size_t UNDEFINED;

    explicit Graph(size_t degree, size_t nr_vertices = 0)
        : base_recvec(degree, nr_vertices, UNDEFINED), _cc_ids(),
          _has_scc(false), _next_edge_pos(nr_vertices), degree(degree) {}

    Graph &operator=(Graph const &graph) = delete;

    void inline set(size_t i, size_t j, size_t k) {
      base_recvec::set(i, j, k);
      _has_scc = false;
    }

    void inline add_nodes(size_t nr) {
      base_recvec::add_rows(nr);
      for (size_t i = 0; i < nr; ++i) {
        _next_edge_pos.push_back(0);
      }
      _has_scc = false;
    }

    size_t inline nr_nodes() { return nr_rows(); }

    void inline add_edge(size_t i, size_t j) {
      LIBSEMIGROUPS_ASSERT(_next_edge_pos[i] < degree);
      LIBSEMIGROUPS_ASSERT(i < _nr_rows);
      set(i, _next_edge_pos[i], j);
      ++_next_edge_pos[i];
    }

    size_t inline nr_edges() {
      size_t edges = 0;
      size_t count = 0;
      for (size_t i = 0; i < _nr_rows; ++i) {
        count += std::count_if(begin_row(i), end_row(i),
                               [](size_t j) { return j != UNDEFINED; });
      }
      return edges;
    }

    void tidy() {
      // something is bound to go wrong here
      for (size_t i = 0; i < _nr_rows; ++i) {
        std::sort(begin_row(i), end_row(i));
        _next_edge_pos[i] = std::count_if(
            begin_row(i), end_row(i), [](size_t j) { return j != UNDEFINED; });
      }
    }

    // Gabow's Strongly Connected Component algorithm
    // Strongly based on the implementation in Digraphs
    // https://github.com/gap-packages/Digraphs
    void gabow_scc() {
      tidy();
      _cc_ids = std::vector<size_t>(_nr_rows, 0);
      size_t end1 = 0;
      std::vector<size_t> stack1(_nr_rows+1, UNDEFINED);
      std::stack<size_t> stack2;
      size_t *frame = new size_t[2 * _nr_rows + 1];

      size_t w;
      size_t count = _nr_rows;

      for (size_t v = 0; v < _nr_rows; ++v) {
        if (_cc_ids[v] == 0) {
          size_t level = 1;
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
              w = get(frame[0], frame[1]);
              frame[1] += 1;
              size_t idw = _cc_ids[w];

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

      for (size_t i = 0; i < _nr_rows; ++i) {
        _cc_ids[i] -= _nr_rows;
      }

      _has_scc = true;
    }

    size_t get_scc_id(size_t node) {
      LIBSEMIGROUPS_ASSERT(node < _nr_rows);
      if (!_has_scc) {
        gabow_scc();
      }
     return _cc_ids[node];
    }

   private:
    std::vector<size_t> _cc_ids;
    bool _has_scc;
    std::vector<size_t> _next_edge_pos;
    size_t degree;
  };

  const size_t Graph::UNDEFINED = std::numeric_limits<size_t>::max();
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_SRC_GRAPH_H_
