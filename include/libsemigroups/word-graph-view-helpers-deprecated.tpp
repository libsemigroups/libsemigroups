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

// This file contains implementations of deprecated helper functions for
// word graph views.

namespace libsemigroups {
  namespace word_graph {

    template <typename Node>
    bool is_standardized(WordGraphView<Node> const& wg, Order val) {
      if (wg.number_of_nodes_no_checks() <= 1) {
        return true;
      }

      switch (val) {
        case Order::none:
          return true;
        case Order::lenlex:
          return detail::is_lenlex_standardized(wg);
        case Order::lex:
          return detail::is_lex_standardized(wg);
        case Order::rpo:
          return detail::is_rpo_standardized(wg);
        case Order::rev_rpo:
          return detail::is_rev_rpo_standardized(wg);
        default:
          LIBSEMIGROUPS_EXCEPTION("not yet implemented")
      }
    }

    template <typename Node>
    std::pair<Node, word_type::const_iterator>
    last_node_on_path_no_checks(WordGraphView<Node> const& wgv,
                                Node                       source,
                                word_type const&           w) {
      return last_node_on_path_no_checks(wgv, source, w.cbegin(), w.cend());
    }

    template <typename Node>
    std::pair<Node, word_type::const_iterator>
    last_node_on_path(WordGraphView<Node> const& wgv,
                      Node                       source,
                      word_type const&           w) {
      return last_node_on_path(wgv, source, w.cbegin(), w.cend());
    }

  }  // namespace word_graph
}  // namespace libsemigroups
