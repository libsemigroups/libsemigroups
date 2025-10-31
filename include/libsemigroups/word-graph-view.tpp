//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2025 Nadim Searight
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

#include "libsemigroups/word-graph-view.hpp"
#include "libsemigroups/word-graph.hpp"
#include <utility>

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
  WordGraphView<Node>& WordGraphView<Node>::start_node(node_type start) {
    throw_if_graph_is_nullptr();
    throw_if_endpoint_out_of_bounds(start, "start");
    throw_if_endpoints_wrong_order(start, _end);
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
    throw_if_endpoint_out_of_bounds(end, "end");
    throw_if_endpoints_wrong_order(_start, end);
    return end_node_no_checks(end);
  }

  //////////////////////////////////////////////////////////////////////////
  // Accessors
  //////////////////////////////////////////////////////////////////////////

  template <typename Node>
  [[nodiscard]] typename WordGraphView<Node>::size_type
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
    throw_if_node_out_of_bounds(s);
    throw_if_label_out_of_bounds(a);
    return next_label_and_target_no_checks(s, a);
  }

  template <typename Node>
  typename WordGraphView<Node>::node_type
  WordGraphView<Node>::target(node_type source, label_type a) const {
    throw_if_invalid_view();
    throw_if_node_out_of_bounds(source);
    throw_if_label_out_of_bounds(a);
    return target_no_checks(source, a);
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

  // not noexcept because it throws an exception!
  template <typename Node>
  void WordGraphView<Node>::throw_if_label_out_of_bounds(
      typename WordGraph<Node>::label_type lbl) const {
    if (lbl >= out_degree_no_checks()) {
      LIBSEMIGROUPS_EXCEPTION("label value out of bounds, expected value in "
                              "the range [0, {}), got {}",
                              out_degree_no_checks(),
                              lbl);
    }
  }

  template <typename Node>
  void WordGraphView<Node>::throw_if_endpoint_out_of_bounds(
      node_type        endpoint,
      std::string_view node_name) const {
    if (endpoint > _graph->number_of_nodes()) {
      LIBSEMIGROUPS_EXCEPTION(
          "invalid {} value, expected values in the range [{}, {}], got {}",
          node_name,
          0,
          _graph->number_of_nodes(),
          endpoint);
    }
  }

  template <typename Node>
  void
  WordGraphView<Node>::throw_if_endpoints_wrong_order(node_type start,
                                                      node_type end) const {
    if (end < start) {
      LIBSEMIGROUPS_EXCEPTION(
          "invalid range, expected start <= end, got start = {} and end = {}",
          start,
          end);
    }
  }

}  // namespace libsemigroups
