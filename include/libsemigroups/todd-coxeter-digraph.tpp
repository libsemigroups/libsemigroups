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
    size_t m = NodeManager_::node_capacity();
    if (n > m) {
      m = n - m;
      BaseDigraph::add_nodes(m);
      NodeManager_::add_free_nodes(m);
    }
  }

  template <typename BaseDigraph>
  std::pair<bool, typename ToddCoxeterDigraph<BaseDigraph>::node_type>
  ToddCoxeterDigraph<BaseDigraph>::complete_path(
      node_type                 c,
      word_type::const_iterator first,
      word_type::const_iterator last) noexcept {
    LIBSEMIGROUPS_ASSERT(NodeManager_::is_active_node(c));
    LIBSEMIGROUPS_ASSERT(first <= last);

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
  void ToddCoxeterDigraph<BaseDigraph>::swap_nodes(node_type c, node_type d) {
    LIBSEMIGROUPS_ASSERT(_coinc.empty());
    LIBSEMIGROUPS_ASSERT(c != NodeManager_::_id_node);
    LIBSEMIGROUPS_ASSERT(d != NodeManager_::_id_node);
    LIBSEMIGROUPS_ASSERT(c != d);
    LIBSEMIGROUPS_ASSERT(NodeManager_::is_valid_node(c));
    LIBSEMIGROUPS_ASSERT(NodeManager_::is_valid_node(d));

    if (NodeManager_::is_active_node(c) && NodeManager_::is_active_node(d)) {
      BaseDigraph::swap_nodes(c, d);
    } else if (NodeManager_::is_active_node(c)) {
      BaseDigraph::rename_node(c, d);
    } else {
      LIBSEMIGROUPS_ASSERT(NodeManager_::is_active_node(d));
      BaseDigraph::rename_node(d, c);
    }
    NodeManager_::switch_nodes(c, d);
  }

  template <typename BaseDigraph>
  void ToddCoxeterDigraph<BaseDigraph>::stats_check_point() {
    _stats.prev_active_nodes  = NodeManager_::number_of_nodes_active();
    _stats.prev_nodes_killed  = NodeManager_::number_of_nodes_killed();
    _stats.prev_nodes_defined = NodeManager_::number_of_nodes_defined();
  }

  template <typename BaseDigraph>
  typename ToddCoxeterDigraph<BaseDigraph>::node_type
  ToddCoxeterDigraph<BaseDigraph>::new_node() {
    if (NodeManager_::has_free_nodes()) {
      node_type const c = NodeManager_::new_active_node();
      // Clear the new node's row in each table
      BaseDigraph::clear_sources_and_targets(c);
      return c;
    } else {
      reserve(2 * NodeManager_::node_capacity());
      return NodeManager_::new_active_node();
    }
  }

}  // namespace libsemigroups
