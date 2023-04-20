//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2023 James D. Mitchell
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

// This file contains an implementation of Gabow's algorithm for WordGraphs.

#ifndef LIBSEMIGROUPS_GABOW_HPP_
#define LIBSEMIGROUPS_GABOW_HPP_

#include <stack>  // for stack

#include "digraph.hpp"  // for ActionDigraph
#include "forest.hpp"   // for Forest
#include "runner.hpp"   // for Runner

#include "rx/ranges.hpp"  // for transform

namespace libsemigroups {

  template <typename Node>
  class Gabow : public Runner {
   public:
    using node_type  = Node;
    using label_type = typename ActionDigraph<node_type>::label_type;
    using size_type  = size_t;

   private:
    ActionDigraph<node_type> const&     _graph;
    std::vector<std::vector<node_type>> _comps;
    bool                                _finished;
    std::vector<size_type>              _id;
    Forest                              _bckwd_forest;
    bool                                _bckwd_forest_defined;
    Forest                              _forwd_forest;
    bool                                _forwd_forest_defined;

   public:
    Gabow(ActionDigraph<node_type> const& wg)
        : _graph(wg),
          _comps(),
          _finished(false),
          _id(),
          _bckwd_forest(),
          _bckwd_forest_defined(false),
          _forwd_forest(),
          _forwd_forest_defined(false) {}

    [[nodiscard]] size_type id_no_checks(node_type v) {
      run();
      return _id[v];
    }

    [[nodiscard]] size_type id(node_type v) {
      run();
      validate_node(v);
      return _id[v];
    }

    [[nodiscard]] auto components() {
      run();
      return _comps;
    }

    [[nodiscard]] auto component(size_t i) {
      run();
      if (i >= number()) {
        LIBSEMIGROUPS_EXCEPTION_V3("TODO");
      }
      return _comps[i];
    }

    [[nodiscard]] auto component_no_checks(size_t i) {
      run();
      return _comps[i];
    }

    // TODO better name
    [[nodiscard]] auto number() {
      run();
      return _comps.size();
    }

    [[nodiscard]] auto roots() {
      return (rx::iterator_range(_comps.cbegin(), _comps.cend())
              | rx::transform([](auto const& comp) { return comp[0]; }));
    }

    [[nodiscard]] node_type root_of(node_type n) {
      run();
      validate_node(n);
      return root_of_no_checks(n);
    }

    [[nodiscard]] node_type root_of_no_checks(node_type n) {
      run();
      return component_of_no_checks(n)[0];
    }

    [[nodiscard]] auto component_of(node_type n) {
      run();
      validate_node(n);
      return _comps[_id[n]];
    }

    [[nodiscard]] auto component_of_no_checks(node_type n) {
      run();
      return _comps[_id[n]];
    }

    Gabow& reset() noexcept {
      _finished             = false;
      _bckwd_forest_defined = false;
      _forwd_forest_defined = false;
      return *this;
    }

    Forest const& spanning_forest() {
      if (_forwd_forest_defined) {
        return _forwd_forest;
      }

      run();

      std::vector<bool>     seen(_graph.number_of_nodes(), false);
      std::queue<node_type> queue;

      _forwd_forest.clear();
      _forwd_forest.add_nodes(_graph.number_of_nodes());

      for (size_t i = 0; i < number(); ++i) {
        queue.push(_comps[i][0]);
        seen[_comps[i][0]] = true;
        do {
          size_t x = queue.front();
          for (auto e : _graph.labels()) {
            size_t y = _graph.unsafe_neighbor(x, e);
            if (y != UNDEFINED && !seen[y] && _id[y] == _id[x]) {
              _forwd_forest.set(y, x, e);
              queue.push(y);
              seen[y] = true;
            }
          }
          queue.pop();
        } while (!queue.empty());
      }
      _forwd_forest_defined = true;
      return _forwd_forest;
    }

    Forest const& reverse_spanning_forest() {
      if (_bckwd_forest_defined) {
        return _bckwd_forest;
      }
      run();

      _bckwd_forest.clear();
      _bckwd_forest.add_nodes(_graph.number_of_nodes());

      // TODO remove this
      std::vector<std::vector<node_type>> reverse_edges(
          _graph.number_of_nodes(), std::vector<node_type>());
      std::vector<std::vector<node_type>> reverse_labels(
          _graph.number_of_nodes(), std::vector<node_type>());

      for (auto n : _graph.nodes()) {
        size_t const scc_id_n = id(n);
        for (auto e : _graph.labels()) {
          size_t const k = _graph.unsafe_neighbor(n, e);
          if (id(k) == scc_id_n) {
            reverse_edges[k].push_back(n);
            reverse_labels[k].push_back(e);
          }
        }
      }

      std::queue<size_t> queue;
      std::vector<bool>  seen(_graph.number_of_nodes(), false);

      for (size_t i = 0; i < number(); ++i) {
        LIBSEMIGROUPS_ASSERT(queue.empty());
        queue.push(_comps[i][0]);
        seen[_comps[i][0]] = true;
        while (!queue.empty()) {
          size_t x = queue.front();
          for (size_t j = 0; j < reverse_edges[x].size(); ++j) {
            size_t y = reverse_edges[x][j];
            if (!seen[y]) {
              queue.push(y);
              seen[y] = true;
              _bckwd_forest.set(y, x, reverse_labels[x][j]);
            }
          }
          queue.pop();
        }
      }
      _bckwd_forest_defined = true;
      return _bckwd_forest;
    }

   private:
    bool finished_impl() const override {
      return _finished;
    }

    void run_impl() override {
      if (finished()) {
        return;
      }

      std::stack<node_type>                        stack1;
      std::stack<node_type>                        stack2;
      std::stack<std::pair<node_type, label_type>> frame;
      std::vector<node_type>                       preorder;

      preorder.assign(_graph.number_of_nodes(), UNDEFINED);
      LIBSEMIGROUPS_ASSERT(stack1.empty());
      LIBSEMIGROUPS_ASSERT(stack2.empty());
      LIBSEMIGROUPS_ASSERT(frame.empty());

      _comps.clear();
      _id.assign(_graph.number_of_nodes(), UNDEFINED);

      node_type C     = 0;
      node_type index = 0;

      for (auto w : _graph.nodes()) {
        if (_id[w] == UNDEFINED) {
          frame.emplace(w, 0);
        dfs_start:
          LIBSEMIGROUPS_ASSERT(!frame.empty());
          node_type  v = frame.top().first;
          label_type i = frame.top().second;

          preorder[v] = C++;
          stack1.push(v);
          stack2.push(v);
          for (; i < _graph.out_degree(); ++i) {
          dfs_end:
            LIBSEMIGROUPS_ASSERT(v < number_of_nodes() && i < _degree);
            node_type u = _graph.unsafe_neighbor(v, i);
            if (u != UNDEFINED) {
              if (preorder[u] == UNDEFINED) {
                frame.top().second = i;
                frame.emplace(u, 0);
                goto dfs_start;
              } else if (_id[u] == UNDEFINED) {
                LIBSEMIGROUPS_ASSERT(!stack2.empty());
                while (preorder[stack2.top()] > preorder[u]) {
                  stack2.pop();
                }
              }
            }
          }
          if (v == stack2.top()) {
            _comps.emplace_back();
            node_type x;
            do {
              LIBSEMIGROUPS_ASSERT(!stack1.empty());
              x      = stack1.top();
              _id[x] = index;
              _comps[index].push_back(x);
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
      _finished = true;
    }

    void validate_node(node_type n) {
      if (n >= _id.size()) {
        LIBSEMIGROUPS_EXCEPTION_V3("TODO");
      }
    }
  };

  template <typename Node>
  Gabow(ActionDigraph<Node> const&) -> Gabow<Node>;

}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_GABOW_HPP_
