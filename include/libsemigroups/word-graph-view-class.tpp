//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2025-2026 Nadim Searight
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

// This file contains an implementation of word graph views, a thin layer over
// word graphs exposing a chosen range of nodes

namespace libsemigroups {

  template <typename Node>
  constexpr typename WordGraphView<Node>::node_type
  WordGraphView<Node>::to_graph(node_type n) const noexcept {
    if (n == UNDEFINED) {
      // LCOV does not recognise this as being called,
      // but it is definitely called in test-word-graph-view.cpp[015]
      return UNDEFINED;
    }
    return n + _start;
  }

  template <typename Node>
  constexpr typename WordGraphView<Node>::node_type
  WordGraphView<Node>::to_view(node_type n) const noexcept {
    if (n == UNDEFINED) {
      return UNDEFINED;
    }
    return n - _start;
  }

  //////////////////////////////////////////////////////////////////////////
  // Constructors + initialisers
  //////////////////////////////////////////////////////////////////////////

  template <typename Node>
  WordGraphView<Node>& WordGraphView<Node>::init() {
    _graph = nullptr;
    _start = 0;
    _end   = 0;
    return *this;
  }

  template <typename Node>
  WordGraphView<Node>& WordGraphView<Node>::init(WordGraph<Node> const& graph,
                                                 size_type              start,
                                                 size_type              end) {
    LIBSEMIGROUPS_ASSERT(start <= end);
    LIBSEMIGROUPS_ASSERT(end <= graph.number_of_nodes());
    _graph = &graph;
    _start = start;
    _end   = end;
    return *this;
  }

  //////////////////////////////////////////////////////////////////////////
  // Modifiers
  //////////////////////////////////////////////////////////////////////////

  template <typename Node>
  WordGraphView<Node>& WordGraphView<Node>::reshape_no_checks(node_type start,
                                                              node_type end) {
    LIBSEMIGROUPS_ASSERT(_graph != nullptr);
    LIBSEMIGROUPS_ASSERT(end <= _graph->number_of_nodes());
    LIBSEMIGROUPS_ASSERT(start <= end);
    _start = start;
    _end   = end;
    return *this;
  }

  template <typename Node>
  WordGraphView<Node>&
  WordGraphView<Node>::start_node_no_checks(node_type start) noexcept {
    LIBSEMIGROUPS_ASSERT(_graph != nullptr);
    LIBSEMIGROUPS_ASSERT(start <= _graph->number_of_nodes());
    LIBSEMIGROUPS_ASSERT(start <= _end);
    _start = start;
    return *this;
  }

  template <typename Node>
  WordGraphView<Node>& WordGraphView<Node>::start_node(node_type start) {
    throw_if_graph_is_nullptr();
    detail::throw_if_not_in_range(start, 0, _end + 1, "start ");
    return start_node_no_checks(start);
  }

  template <typename Node>
  WordGraphView<Node>&
  WordGraphView<Node>::end_node_no_checks(node_type end) noexcept {
    LIBSEMIGROUPS_ASSERT(_graph != nullptr);
    LIBSEMIGROUPS_ASSERT(end <= _graph->number_of_nodes());
    LIBSEMIGROUPS_ASSERT(end >= _start);
    _end = end;
    return *this;
  }

  template <typename Node>
  WordGraphView<Node>& WordGraphView<Node>::end_node(node_type end) {
    throw_if_graph_is_nullptr();
    detail::throw_if_not_in_range(
        end, _start, _graph->number_of_nodes() + 1, "end ");
    return end_node_no_checks(end);
  }

  //////////////////////////////////////////////////////////////////////////
  // Accessors
  //////////////////////////////////////////////////////////////////////////

  template <typename Node>
  typename WordGraphView<Node>::size_type
  WordGraphView<Node>::number_of_edges_no_checks() const noexcept {
    size_t count = 0;
    for (auto s : nodes_no_checks()) {
      for (auto t : targets_no_checks(s)) {
        if (t != UNDEFINED) {
          count++;
        }
      }
    }
    return count;
  }

  //////////////////////////////////////////////////////////////////////////
  // Nodes, targets and labels
  //////////////////////////////////////////////////////////////////////////

  template <typename Node>
  std::pair<typename WordGraphView<Node>::label_type,
            typename WordGraphView<Node>::node_type>
  WordGraphView<Node>::next_label_and_target_no_checks(node_type  s,
                                                       label_type a) const {
    node_type                        translated = to_graph(s);
    std::pair<node_type, label_type> result
        = _graph->next_label_and_target_no_checks(translated, a);
    to_view(result);
    return result;
  }

  template <typename Node>
  std::pair<typename WordGraphView<Node>::label_type,
            typename WordGraphView<Node>::node_type>
  WordGraphView<Node>::next_label_and_target(node_type s, label_type a) const {
    throw_if_invalid_view();
    detail::throw_if_not_less(s, number_of_nodes_no_checks(), "node ");
    detail::throw_if_not_less(a, out_degree(), "label ");
    return next_label_and_target_no_checks(s, a);
  }

  template <typename Node>
  typename WordGraphView<Node>::node_type
  WordGraphView<Node>::target(node_type s, label_type a) const {
    throw_if_invalid_view();
    detail::throw_if_not_less(s, number_of_nodes_no_checks(), "node ");
    detail::throw_if_not_less(a, out_degree(), "label ");
    return target_no_checks(s, a);
  }

  //////////////////////////////////////////////////////////////////////////
  // Operators
  //////////////////////////////////////////////////////////////////////////

  template <typename Node>
  bool
  WordGraphView<Node>::equal_to_no_checks(WordGraphView const& that) const {
    {
      if (_graph == that._graph && _start == that._start && _end == that._end) {
        return true;
      }
      if (number_of_nodes_no_checks() != that.number_of_nodes_no_checks()) {
        return false;
      }
      if (out_degree_no_checks() != that.out_degree_no_checks()) {
        return false;
      }
      for (auto const& s : nodes_no_checks()) {
        for (auto const& a : labels_no_checks()) {
          if (target_no_checks(s, a) != that.target_no_checks(s, a)) {
            return false;
          }
        }
      }
      return true;
    }
  }

  template <typename Node>
  bool WordGraphView<Node>::operator==(WordGraphView const& that) const {
    if (_graph == nullptr && that._graph == nullptr && _start == that._start
        && _end == that._end) {
      return true;
    }
    throw_if_invalid_view();
    that.throw_if_invalid_view();
    return equal_to_no_checks(that);
  }

  //////////////////////////////////////////////////////////////////////////
  // Validation
  //////////////////////////////////////////////////////////////////////////

  template <typename Node>
  template <typename Iterator>
  void
  WordGraphView<Node>::throw_if_any_target_out_of_bounds(Iterator first,
                                                         Iterator last) const {
    for (auto it = first; it != last; ++it) {
      auto s = *it;
      for (auto [a, t] : labels_and_targets_no_checks(s)) {
        if (t != UNDEFINED && t >= number_of_nodes_no_checks()) {
          LIBSEMIGROUPS_EXCEPTION(
              "target out of bounds, the edge with source {} and label {} "
              "has target {}, but expected value in the range [0, {})",
              s,
              a,
              t,
              number_of_nodes_no_checks());
        }
      }
    }
  }

  // TODO(v4) rm
  template <typename Node>
  void WordGraphView<Node>::throw_if_label_out_of_bounds(
      typename WordGraph<Node>::label_type a) const {
    detail::throw_if_not_less(a, out_degree(), "label ");
  }

  // TODO(v4) rm
  template <typename Node>
  template <typename Node2>
  void WordGraphView<Node>::throw_if_node_out_of_bounds(Node2 n) const {
    static_assert(sizeof(Node2) <= sizeof(Node));
    if (static_cast<Node>(n) < 0
        || static_cast<Node>(n) >= end_node() - start_node()) {
      LIBSEMIGROUPS_EXCEPTION("node value out of bounds, expected value "
                              "in the range [{}, {}), got {}",
                              0,
                              end_node() - start_node(),
                              n);
    }
  }
}  // namespace libsemigroups
