//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2022 James D. Mitchell
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

// This file contains the implementation for the ToddCoxeterDigraph class, used
// by Stephen and (by ToddCoxeter in the future).

namespace libsemigroups {
  namespace detail {

    template <typename BaseDigraph>
    struct ToddCoxeterDigraph<BaseDigraph>::Settings {
      uint64_t large_collapse = 100'000;
    };

    template <typename BaseDigraph>
    struct ToddCoxeterDigraph<BaseDigraph>::Stats {
      uint64_t prev_active_nodes;
      uint64_t prev_nodes_killed;
      uint64_t prev_nodes_defined;
    };

    template <typename BaseDigraph>
    void ToddCoxeterDigraph<BaseDigraph>::reserve(size_t n) {
      size_t m = NodeManager::node_capacity();
      if (n > m) {
        m = n - m;
        BaseDigraph::add_nodes(m);
        NodeManager::add_free_nodes(m);
      }
    }

    template <typename BaseDigraph>
    std::pair<bool, typename ToddCoxeterDigraph<BaseDigraph>::node_type>
    ToddCoxeterDigraph<BaseDigraph>::complete_path(
        node_type                 c,
        word_type::const_iterator first,
        word_type::const_iterator last) noexcept {
      word_type::const_iterator it;
      std::tie(c, it)
          = action_digraph_helper::last_node_on_path_nc(*this, c, first, last);
      bool result = false;
      for (; it < last; ++it) {
        LIBSEMIGROUPS_ASSERT(unsafe_neighbor(c, *it) == UNDEFINED);
        node_type d = new_node();
        BaseDigraph::add_edge_nc(c, d, *it);
        result = true;
        c      = d;
      }
      return std::make_pair(result, c);
    }

    template <typename BaseDigraph>
    void ToddCoxeterDigraph<BaseDigraph>::process_coincidences() {
      if (_coinc.empty()) {
        return;
      }

      auto new_edge_func = [](node_type, letter_type) {};
      auto incompat_func
          = [this](node_type c, node_type d) { this->_coinc.emplace(c, d); };

      auto const coinc_max_size = large_collapse();

      while (!_coinc.empty() && _coinc.size() < coinc_max_size) {
        Coincidence c = _coinc.top();
        _coinc.pop();
        node_type min = NodeManager::find_node(c.first);
        node_type max = NodeManager::find_node(c.second);
        if (min != max) {
          if (min > max) {
            std::swap(min, max);
          }
          NodeManager::union_nodes(min, max);
          // TODO(later)  new_edge_func + incompat_func should be template
          // params again
          BaseDigraph::merge_nodes(min, max, new_edge_func, incompat_func);
        }
      }

      if (_coinc.empty()) {
        return;
      }

      while (!_coinc.empty()) {
        Coincidence c = _coinc.top();
        _coinc.pop();
        node_type min = find_node(c.first);
        node_type max = find_node(c.second);
        if (min != max) {
          if (min > max) {
            std::swap(min, max);
          }
          union_nodes(min, max);
          for (letter_type i = 0; i < out_degree(); ++i) {
            node_type const v = unsafe_neighbor(max, i);
            if (v != UNDEFINED) {
              node_type const u = unsafe_neighbor(min, i);
              if (u == UNDEFINED) {
                ActionDigraph<node_type>::add_edge_nc(min, v, i);
              } else if (u != v) {
                // Add (u,v) to the stack of pairs to be identified
                _coinc.emplace(u, v);
              }
            }
          }
        }
      }

      // Remove all sources of all remaining active cosets
      auto c = _id_node;
      while (c != first_free_node()) {
        BaseDigraph::clear_sources(c);
        c = next_active_node(c);
      }

      c = _id_node;

      while (c != first_free_node()) {
        for (letter_type x = 0; x < out_degree(); ++x) {
          auto cx = unsafe_neighbor(c, x);
          if (cx != UNDEFINED) {
            auto d = find_node(cx);
            if (cx != d) {
              new_edge_func(c, x);
              ActionDigraph<node_type>::add_edge_nc(c, d, x);
            }
            // Must re-add the source, even if we don't need to reset
            // the target or stack the deduction
            BaseDigraph::add_source(d, x, c);
            LIBSEMIGROUPS_ASSERT(is_active_node(d));
          }
        }
        c = next_active_node(c);
      }
    }

    template <typename BaseDigraph>
    void ToddCoxeterDigraph<BaseDigraph>::swap_nodes(node_type c, node_type d) {
      LIBSEMIGROUPS_ASSERT(_coinc.empty());
      LIBSEMIGROUPS_ASSERT(c != _id_node);
      LIBSEMIGROUPS_ASSERT(d != _id_node);
      LIBSEMIGROUPS_ASSERT(c != d);
      LIBSEMIGROUPS_ASSERT(is_valid_node(c));
      LIBSEMIGROUPS_ASSERT(is_valid_node(d));

      if (is_active_node(c) && is_active_node(d)) {
        BaseDigraph::swap_nodes(c, d);
      } else if (is_active_node(c)) {
        BaseDigraph::rename_node(c, d);
      } else {
        LIBSEMIGROUPS_ASSERT(is_active_node(d));
        BaseDigraph::rename_node(d, c);
      }
      NodeManager::switch_nodes(c, d);
    }

    template <typename BaseDigraph>
    void ToddCoxeterDigraph<BaseDigraph>::stats_check_point() {
      _stats.prev_active_nodes  = NodeManager::number_of_nodes_active();
      _stats.prev_nodes_killed  = NodeManager::number_of_nodes_killed();
      _stats.prev_nodes_defined = NodeManager::number_of_nodes_defined();
    }

    template <typename BaseDigraph>
    typename ToddCoxeterDigraph<BaseDigraph>::node_type
    ToddCoxeterDigraph<BaseDigraph>::new_node() {
      if (NodeManager::has_free_nodes()) {
        node_type const c = NodeManager::new_active_node();
        // Clear the new node's row in each table
        BaseDigraph::clear_sources_and_targets(c);
        return c;
      } else {
        reserve(2 * node_capacity());
        return NodeManager::new_active_node();
      }
    }

  }  // namespace detail
}  // namespace libsemigroups
