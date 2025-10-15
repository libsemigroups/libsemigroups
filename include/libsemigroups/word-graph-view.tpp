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

  template <typename Node>
  [[nodiscard]] typename WordGraphView<Node>::size_type
  WordGraphView<Node>::number_of_edges() const noexcept {
    size_t count = 0;
    for (auto s : nodes()) {
      for (auto t : targets_no_checks(s)) {
        if (t != UNDEFINED) {
          count++;
        }
      }
    }
    return count;
  }

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
    throw_if_node_out_of_bounds(s);
    throw_if_label_out_of_bounds(a);
    node_type                        translated = to_graph(s);
    std::pair<node_type, label_type> result
        = _graph->next_label_and_target_no_checks(translated, a);
    to_view(result);
    return result;
  }

  template <typename Node>
  bool WordGraphView<Node>::operator==(WordGraphView const& that) const {
    {
      if (number_of_nodes() != that.number_of_nodes()) {
        return false;
      }
      if (out_degree() != that.out_degree()) {
        return false;
      }
      auto this_node = cbegin_nodes();
      auto that_node = that.cbegin_nodes();
      while (this_node < cend_nodes()) {
        for (label_type a = 0; a < out_degree(); ++a) {
          if (target_no_checks(*this_node, a)
              != that.target_no_checks(*that_node, a)) {
            return false;
          }
        }
        ++this_node;
        ++that_node;
      }
      return true;
    }
  }

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
      for (auto [a, t] : labels_and_targets(s)) {
        if (t != UNDEFINED && t >= number_of_nodes()) {
          LIBSEMIGROUPS_EXCEPTION(
              "target out of bounds, the edge with source {} and label {} "
              "has target {}, but expected value in the range [0, {})",
              s,
              a,
              t,
              number_of_nodes());
        }
      }
    }
  }

  // not noexcept because it throws an exception!
  template <typename Node>
  void WordGraphView<Node>::throw_if_label_out_of_bounds(
      typename WordGraph<Node>::label_type lbl) const {
    if (lbl >= out_degree()) {
      LIBSEMIGROUPS_EXCEPTION("label value out of bounds, expected value in "
                              "the range [0, {}), got {}",
                              out_degree(),
                              lbl);
    }
  }

  namespace word_graph {

    template <typename Node>
    WordGraph<Node> graph_from_view(WordGraphView<Node> const& view) {
      WordGraph<Node> result
          = WordGraph<Node>(view.number_of_nodes(), view.out_degree());
      for (auto node : rx::iterator_range(view.nodes())) {
        for (auto label_target : view.labels_and_targets(node)) {
          auto target = std::get<1>(label_target);
          if (target == UNDEFINED) {
            continue;
          }
          result.target(
              node, std::get<0>(label_target), std::get<1>(label_target));
        }
      }
      // LCOV identifies the blank line after this as not being covered for some
      // reason
      return result;
    }
  }  // namespace word_graph
}  // namespace libsemigroups
