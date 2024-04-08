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

// This file contains the implementation of the Gabow class.
namespace libsemigroups {

  template <typename Node>
  Gabow<Node>::~Gabow() = default;

  template <typename Node>
  Gabow<Node>& Gabow<Node>::init(WordGraph<node_type> const& wg) {
    _graph = &wg;
    _comps.clear();
    _id.clear();
    _bckwd_forest.clear();
    _forwd_forest.clear();
    reset();
    return *this;
  }

  template <typename Node>
  Gabow<Node> const& Gabow<Node>::reset() const noexcept {
    _finished             = false;
    _bckwd_forest_defined = false;
    _forwd_forest_defined = false;
    return *this;
  }

  template <typename Node>
  Forest const& Gabow<Node>::spanning_forest() const {
    if (_forwd_forest_defined) {
      return _forwd_forest;
    }

    run();

    _forwd_forest.clear();
    _forwd_forest.add_nodes(_graph->number_of_nodes());

    std::queue<node_type> queue;
    size_type const       n = number_of_components();

    for (size_type i = 0; i < n; ++i) {
      node_type root = _comps[i][0];
      queue.push(root);
      do {
        node_type x = queue.front();
        for (auto e : _graph->labels()) {
          node_type y = _graph->target_no_checks(x, e);
          if (y != UNDEFINED && y != root && _id[y] == _id[x]
              && _forwd_forest.parent(y) == UNDEFINED) {
            _forwd_forest.set(y, x, e);
            queue.push(y);
          }
        }
        queue.pop();
      } while (!queue.empty());
    }
    _forwd_forest_defined = true;
    return _forwd_forest;
  }

  template <typename Node>
  Forest const& Gabow<Node>::reverse_spanning_forest() const {
    if (_bckwd_forest_defined) {
      return _bckwd_forest;
    }

    run();

    _bckwd_forest.clear();
    _bckwd_forest.add_nodes(_graph->number_of_nodes());
    size_type const m = _graph->number_of_nodes();

    std::vector<std::vector<node_type>> reverse_edges(m,
                                                      std::vector<node_type>());
    std::vector<std::vector<node_type>> reverse_labels(
        m, std::vector<node_type>());

    for (auto n : _graph->nodes()) {
      size_type const scc_id_n = id(n);
      for (auto e : _graph->labels()) {
        size_type const k = _graph->target_no_checks(n, e);
        if (k != UNDEFINED && _id[k] == scc_id_n) {
          reverse_edges[k].push_back(n);
          reverse_labels[k].push_back(e);
        }
      }
    }

    std::queue<size_type> queue;
    size_type const       n = number_of_components();

    for (size_type i = 0; i < n; ++i) {
      node_type root = _comps[i][0];
      queue.push(root);
      do {
        node_type x = queue.front();
        for (size_type j = 0; j < reverse_edges[x].size(); ++j) {
          node_type y = reverse_edges[x][j];
          if (y != UNDEFINED && y != root
              && _bckwd_forest.parent(y) == UNDEFINED) {
            queue.push(y);
            _bckwd_forest.set(y, x, reverse_labels[x][j]);
          }
        }
        queue.pop();
      } while (!queue.empty());
    }
    _bckwd_forest_defined = true;
    return _bckwd_forest;
  }

  template <typename Node>
  void Gabow<Node>::run() const {
    if (finished()) {
      return;
    }

    std::stack<node_type>                        stack1;
    std::stack<node_type>                        stack2;
    std::stack<std::pair<node_type, label_type>> frame;
    std::vector<node_type>                       preorder;

    preorder.assign(_graph->number_of_nodes(), UNDEFINED);
    LIBSEMIGROUPS_ASSERT(stack1.empty());
    LIBSEMIGROUPS_ASSERT(stack2.empty());
    LIBSEMIGROUPS_ASSERT(frame.empty());

    _comps.clear();
    _id.assign(_graph->number_of_nodes(), UNDEFINED);

    node_type C     = 0;
    node_type index = 0;

    for (auto w : _graph->nodes()) {
      if (_id[w] == UNDEFINED) {
        frame.emplace(w, 0);
      dfs_start:
        LIBSEMIGROUPS_ASSERT(!frame.empty());
        node_type  v = frame.top().first;
        label_type i = frame.top().second;

        preorder[v] = C++;
        stack1.push(v);
        stack2.push(v);
        for (; i < _graph->out_degree(); ++i) {
        dfs_end:
          LIBSEMIGROUPS_ASSERT(v < _graph->number_of_nodes());
          LIBSEMIGROUPS_ASSERT(i < _graph->out_degree());
          node_type u = _graph->target_no_checks(v, i);
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

  template <typename Node>
  void Gabow<Node>::validate_node(node_type n) const {
    if (n >= _id.size()) {
      LIBSEMIGROUPS_EXCEPTION(
          "node out of bounds, expected value in the range [0, {}), got {}",
          _id.size(),
          n);
    }
  }

  template <typename Node>
  void Gabow<Node>::validate_scc_index(size_type i) const {
    if (i >= number_of_components()) {
      LIBSEMIGROUPS_EXCEPTION(
          "strongly connected component index out of bounds, "
          "expected value in the range [0, {}), got {}",
          number_of_components(),
          i);
    }
  }
}  // namespace libsemigroups
