//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2025 James D. Mitchell
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

  template <template <typename...> typename Result>
  auto to(Forest const& f)
      -> std::enable_if_t<std::is_same_v<Result<int>, WordGraph<int>>,
                          WordGraph<Forest::node_type>> {
    using node_type = Forest::node_type;
    WordGraph<node_type> result(f.number_of_nodes(), forest::max_label(f) + 1);
    for (node_type n = 0; n < f.number_of_nodes(); ++n) {
      if (!forest::is_root(f, n)) {
        result.target_no_checks(f.parent(n), f.label(n), n);
      }
    }
    return result;
  }

  template <template <typename...> typename Result, typename Node>
  auto to(WordGraphView<Node> const& view)
      -> std::enable_if_t<std::is_same_v<Result<Node>, WordGraph<Node>>,
                          WordGraph<Node>> {
    view.throw_if_invalid_view();
    view.throw_if_any_target_out_of_bounds();
    WordGraph<Node> result = WordGraph<Node>(view.number_of_nodes_no_checks(),
                                             view.out_degree_no_checks());
    for (auto source : rx::iterator_range(view.nodes_no_checks())) {
      for (auto [label, target] : view.labels_and_targets_no_checks(source)) {
        if (target == UNDEFINED) {
          continue;
        }
        result.target_no_checks(source, label, target);
      }
    }
    // LCOV identifies the blank line after this as not being covered for some
    // reason
    return result;
  }
}  // namespace libsemigroups
