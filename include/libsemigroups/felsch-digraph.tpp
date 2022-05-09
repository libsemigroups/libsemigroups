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

namespace libsemigroups {
  template <typename W, typename N>
  FelschDigraph<W, N>::FelschDigraph(Presentation<word_type> const &p,
                                     size_type                      n)
      : DigraphWithSources<node_type>(p.contains_empty_word() ? n : n + 1,
                                      p.alphabet().size()),
        _definitions(),
        _felsch_tree(p.alphabet().size()),
        _presentation(p) {
    _felsch_tree.add_relations(_presentation.rules.cbegin(),
                               _presentation.rules.cend());
  }

  template <typename W, typename N>
  bool FelschDigraph<W, N>::def_edge(node_type   c,
                                     letter_type x,
                                     node_type   d) noexcept {
    LIBSEMIGROUPS_ASSERT(c < this->number_of_nodes());
    LIBSEMIGROUPS_ASSERT(x < this->out_degree());
    LIBSEMIGROUPS_ASSERT(d < this->number_of_nodes());

    auto cx = this->unsafe_neighbor(c, x);
    if (cx == UNDEFINED) {
      _definitions.emplace_back(c, x);
      this->add_edge_nc(c, d, x);
      return true;
    } else {
      return cx == d;
    }
  }

  template <typename W, typename N>
  bool FelschDigraph<W, N>::process_definitions(size_t start) {
    for (size_t i = start; i < _definitions.size(); ++i) {
      auto const &d = _definitions[i];
      _felsch_tree.push_back(d.second);
      if (!process_definitions_dfs_v1(d.first)) {
        return false;
      }
    }
    return true;
  }

  template <typename W, typename N>
  void FelschDigraph<W, N>::reduce_number_of_edges_to(size_type n) {
    while (_definitions.size() > n) {
      auto const &p = _definitions.back();
      this->remove_edge_nc(p.first, p.second);
      _definitions.pop_back();
    }
  }

  template <typename W, typename N>
  bool FelschDigraph<W, N>::compatible(node_type        c,
                                       word_type const &u,
                                       word_type const &v) noexcept {
    LIBSEMIGROUPS_ASSERT(c < this->number_of_nodes());

    node_type x, xa;
    if (u.empty()) {
      xa = c;
    } else {
      x = action_digraph_helper::follow_path_nc(
          *this, c, u.cbegin(), u.cend() - 1);
      if (x == UNDEFINED) {
        return true;
      }
      LIBSEMIGROUPS_ASSERT(x < this->number_of_nodes());
      LIBSEMIGROUPS_ASSERT(u.back() < _presentation.alphabet().size());
      xa = this->unsafe_neighbor(x, u.back());
    }

    node_type y, yb;
    if (v.empty()) {
      yb = c;
    } else {
      y = action_digraph_helper::follow_path_nc(
          *this, c, v.cbegin(), v.cend() - 1);
      if (y == UNDEFINED) {
        return true;
      }
      LIBSEMIGROUPS_ASSERT(y < this->number_of_nodes());
      LIBSEMIGROUPS_ASSERT(v.back() < _presentation.alphabet().size());
      yb = this->unsafe_neighbor(y, v.back());
    }

    if (xa == UNDEFINED && yb != UNDEFINED) {
      LIBSEMIGROUPS_ASSERT(x < this->number_of_nodes());
      return def_edge(x, u.back(), yb);
    } else if (xa != UNDEFINED && yb == UNDEFINED) {
      LIBSEMIGROUPS_ASSERT(y < this->number_of_nodes());
      return def_edge(y, v.back(), xa);
    } else if (xa != UNDEFINED && yb != UNDEFINED && xa != yb) {
      return false;
    }
    return true;
  }

  template <typename W, typename N>
  bool FelschDigraph<W, N>::process_definitions_dfs_v1(node_type c) {
    for (auto it = _felsch_tree.cbegin(); it < _felsch_tree.cend(); ++it) {
      if (!compatible(c, *it)) {
        return false;
      }
    }

    size_t const n = _presentation.alphabet().size();
    for (size_t x = 0; x < n; ++x) {
      if (_felsch_tree.push_front(x)) {
        node_type e = this->first_source(c, x);
        while (e != UNDEFINED) {
          if (!process_definitions_dfs_v1(e)) {
            return false;
          }
          e = this->next_source(e, x);
        }
        _felsch_tree.pop_front();
      }
    }
    return true;
  }

}  // namespace libsemigroups
