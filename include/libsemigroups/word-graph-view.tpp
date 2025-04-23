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
  WordGraphView<Node>::WordGraphView(WordGraph<Node> const& graph,
                                     size_type              start,
                                     size_type              end)
      : _graph(&graph), _start(start), _end(end) {}

  template <typename Node>
  WordGraphView<Node>::WordGraphView(WordGraph<Node> const& graph)
      : _graph(&graph), _start(0), _end(graph.number_of_nodes()) {}

  template <typename Node>
  WordGraphView<Node>::WordGraphView() : _graph(nullptr), _start(), _end() {}

  template <typename Node>
  template <typename OtherNode>
  WordGraphView<Node>::WordGraphView(WordGraphView<OtherNode> const& wg)
      : WordGraphView(wg.number_of_nodes(), wg.out_degree()) {
    static_assert(sizeof(OtherNode) <= sizeof(Node));
    init(wg);
  }

  template <typename Node>
  auto WordGraphView<Node>::cbegin_targets(node_type source) const {
    word_graph::throw_if_node_out_of_bounds(*this, source);
    return rx::begin(targets_no_checks(source));
  }

  template <typename Node>
  auto WordGraphView<Node>::cend_targets(node_type source) const {
    word_graph::throw_if_node_out_of_bounds(*this, source);
    return rx::end(targets_no_checks(source));
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
    word_graph::throw_if_node_out_of_bounds(*this, s);
    word_graph::throw_if_label_out_of_bounds(*this, a);
    node_type                        translated = to_graph(s);
    std::pair<node_type, label_type> result
        = _graph->next_label_and_target_no_checks(translated, a);
    to_view(result);
    return result;
  }

  template <typename Node>
  auto WordGraphView<Node>::targets(node_type source) const {
    word_graph::throw_if_node_out_of_bounds(*this, source);
    node_type translated = to_graph(source);
    return to_view(_graph->targets_no_checks(translated));
  }

  template <typename Node>
  auto WordGraphView<Node>::labels_and_targets(node_type source) const {
    word_graph::throw_if_node_out_of_bounds(*this, source);
    return rx::enumerate(targets_no_checks(source));
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
      auto this_node = this->cbegin_nodes();
      auto that_node = that.cbegin_nodes();
      while (this_node < this->cend_nodes()) {
        for (label_type a = 0; a < this->out_degree(); ++a) {
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
  typename WordGraphView<Node>::node_type WordGraphView<Node>::target(
      typename WordGraphView<Node>::node_type  source,
      typename WordGraphView<Node>::label_type a) const {
    word_graph::throw_if_node_out_of_bounds(*this, source);
    word_graph::throw_if_label_out_of_bounds(*this, a);
    return target_no_checks(source, a);
  }
  template <typename Node>
  typename WordGraphView<Node>::node_type WordGraphView<Node>::target_no_checks(
      typename WordGraphView<Node>::node_type  source,
      typename WordGraphView<Node>::label_type a) const {
    node_type translated = to_graph(source);
    return to_view(_graph->target_no_checks(translated, a));
  }
  template <typename Node>
  WordGraphView<Node>&
  WordGraphView<Node>::init(typename WordGraphView<Node>::size_type start,
                            typename WordGraphView<Node>::size_type end) {
    _start = start;
    _end   = end;
    return *this;
  }

  template <typename Node>
  template <typename OtherNode>
  WordGraphView<OtherNode>&
  WordGraphView<Node>::init(WordGraph<OtherNode> const& that) {
    static_assert(sizeof(OtherNode) <= sizeof(Node));
    this->_graph = that;
    return this;
  }

  template <typename Node>
  typename WordGraphView<Node>::size_type
  WordGraphView<Node>::out_degree() const noexcept {
    return _graph->out_degree();
  }

  namespace word_graph {

    template <typename Node1, typename Node2>
    void throw_if_node_out_of_bounds(WordGraphView<Node1> const& wgv, Node2 n) {
      static_assert(sizeof(Node2) <= sizeof(Node1));
      if (static_cast<Node1>(n) < 0
          || static_cast<Node1>(n) >= wgv.end_node() - wgv.start_node()) {
        LIBSEMIGROUPS_EXCEPTION("node value out of bounds, expected value "
                                "in the range [{}, {}), got {}",
                                0,
                                wgv.end_node() - wgv.start_node(),
                                n);
      }
    }

    template <typename Node, typename Iterator1, typename Iterator2>
    void throw_if_node_out_of_bounds(WordGraphView<Node> const& wg,
                                     Iterator1                  first,
                                     Iterator2                  last) {
      for (auto it = first; it != last; ++it) {
        word_graph::throw_if_node_out_of_bounds(wg, *it);
      }
    }

    template <typename Node>
    void throw_if_any_target_out_of_bounds(WordGraphView<Node> const& wg) {
      throw_if_any_target_out_of_bounds(wg, wg.cbegin_nodes(), wg.cend_nodes());
    }

    template <typename Node, typename Iterator>
    void throw_if_any_target_out_of_bounds(WordGraphView<Node> const& wg,
                                           Iterator                   first,
                                           Iterator                   last) {
      for (auto it = first; it != last; ++it) {
        auto s = *it;
        for (auto [a, t] : wg.labels_and_targets(s)) {
          if (t != UNDEFINED && t >= wg.number_of_nodes()) {
            LIBSEMIGROUPS_EXCEPTION(
                "target out of bounds, the edge with source {} and label {} "
                "has target {}, but expected value in the range [0, {})",
                s,
                a,
                t,
                wg.number_of_nodes());
          }
        }
      }
    }

    // not noexcept because it throws an exception!
    template <typename Node>
    void
    throw_if_label_out_of_bounds(WordGraphView<Node> const&           wg,
                                 typename WordGraph<Node>::label_type lbl) {
      if (lbl >= wg.out_degree()) {
        LIBSEMIGROUPS_EXCEPTION("label value out of bounds, expected value in "
                                "the range [0, {}), got {}",
                                wg.out_degree(),
                                lbl);
      }
    }

    template <typename Node>
    void throw_if_label_out_of_bounds(WordGraphView<Node> const& wg,
                                      word_type const&           word) {
      throw_if_label_out_of_bounds(wg, word.cbegin(), word.cend());
    }

    template <typename Node, typename Iterator>
    void throw_if_label_out_of_bounds(WordGraphView<Node> const& wg,
                                      Iterator                   first,
                                      Iterator                   last) {
      std::for_each(first, last, [&wg](letter_type a) {
        throw_if_label_out_of_bounds(wg, a);
      });
    }

    template <typename Node>
    void throw_if_label_out_of_bounds(WordGraphView<Node> const&    wg,
                                      std::vector<word_type> const& rules) {
      std::for_each(rules.cbegin(), rules.cend(), [&wg](word_type const& w) {
        throw_if_label_out_of_bounds(wg, w);
      });
    }

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
