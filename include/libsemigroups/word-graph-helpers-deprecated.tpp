//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2025-2026 James D. Mitchell
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
// word graphs.

namespace libsemigroups {
  namespace detail {

    template <typename Graph>
    bool standardize_no_checks_order(Graph& wg, Forest& f, Order val) {
      if (wg.number_of_nodes() == 0) {
        return false;
      }

      if (f.number_of_nodes() == 0) {
        f.add_nodes(1);
      }

      switch (val) {
        case Order::none:
          return false;
        case Order::lenlex:
          return lenlex_standardize(wg, f);
        case Order::lex:
          return lex_standardize(wg, f);
        case Order::rpo:
          return rpo_standardize(wg, f);
        case Order::rev_rpo:
          return rev_rpo_standardize(wg, f);
          // Intentional fall-through
        default:
          return false;
      }
    }

  }  // namespace detail

  namespace word_graph {

    // not noexcept because it throws an exception!
    // TODO(v4): rm
    template <typename Node>
    void throw_if_node_out_of_bounds(WordGraph<Node> const& wg, Node v) {
      detail::throw_if_not_less(v, wg.number_of_nodes(), "node ");
    }

    // TODO(v4): rm
    template <typename Node, typename Iterator1, typename Iterator2>
    void throw_if_node_out_of_bounds(WordGraph<Node> const& wg,
                                     Iterator1              first,
                                     Iterator2              last) {
      detail::throw_if_any_not_less(first, last, wg.number_of_nodes(), "node ");
    }

    // TODO(v4): rm
    template <typename Node>
    void
    throw_if_label_out_of_bounds(WordGraph<Node> const&               wg,
                                 typename WordGraph<Node>::label_type lbl) {
      detail::throw_if_not_less(lbl, wg.out_degree(), "label ");
    }

    // TODO(v4): rm
    template <typename Node>
    void throw_if_label_out_of_bounds(WordGraph<Node> const& wg,
                                      word_type const&       word) {
      detail::throw_if_any_not_less(
          word.begin(), word.end(), wg.out_degree(), "label ");
    }

    // TODO(v4): rm
    template <typename Node, typename Iterator>
    void throw_if_label_out_of_bounds(WordGraph<Node> const& wg,
                                      Iterator               first,
                                      Iterator               last) {
      detail::throw_if_any_not_less(first, last, wg.out_degree(), "label ");
    }

    template <typename Graph>
    std::pair<bool, Forest> standardize_no_checks(Graph& wg, Order val) {
      Forest f;
      bool   result = detail::standardize_no_checks_order(wg, f, val);
      return std::make_pair(result, f);
    }

    template <typename Graph>
    bool standardize_no_checks(Graph& wg, Forest& f, Order val) {
      return detail::standardize_no_checks_order(wg, f, val);
    }

    // This must be implemented here because throw_if_any_target_out_of_bounds
    // is declared after standardize in the header.
    template <typename Graph>
    bool standardize(Graph& wg, Forest& f, Order val) {
      throw_if_any_target_out_of_bounds(wg, wg.cbegin_nodes(), wg.cend_nodes());
      return detail::standardize_no_checks_order(wg, f, val);
    }

    // This must be implemented here because throw_if_any_target_out_of_bounds
    // is declared after standardize in the header.
    template <typename Graph>
    std::pair<bool, Forest> standardize(Graph& wg, Order val) {
      throw_if_any_target_out_of_bounds(wg, wg.cbegin_nodes(), wg.cend_nodes());
      Forest f;
      bool   result = detail::standardize_no_checks_order(wg, f, val);
      return std::make_pair(result, f);
    }

  }  // namespace word_graph

}  // namespace libsemigroups
