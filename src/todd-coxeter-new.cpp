//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2023 James D. Mitchell
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

// This file contains an implementation of the Todd-Coxeter algorithm for
// semigroups and monoids.

#include "libsemigroups/todd-coxeter-new.hpp"
#include "libsemigroups/obvinf.hpp"

namespace libsemigroups {
  namespace {
    bool constexpr DoNotStackDeductions = false;
    bool constexpr StackDeductions      = true;
  }  // namespace

  void ToddCoxeter::report_active_nodes() {
    using detail::group_digits;

    if (report::should_report()) {
      fmt::print(
          FORMAT(fmt::emphasis::bold,
                 "#0: ToddCoxeter: nodes {:>11} (active) | {:>11} (killed) | "
                 "{:>11} (defined)\n",
                 group_digits(word_graph().number_of_nodes_active()),
                 group_digits(word_graph().number_of_nodes_killed()),
                 group_digits(word_graph().number_of_nodes_defined())));
    }
  }

  using class_index_type = v3::CongruenceInterface::class_index_type;

  void ToddCoxeter::init_run() {
    if (is_obviously_infinite(*this)) {
      LIBSEMIGROUPS_EXCEPTION(
          "there are infinitely many classes in the congruence and "
          "Todd-Coxeter will never terminate");
    }

    _word_graph.settings(*this);
    for (auto it = cbegin_generating_pairs(); it < cend_generating_pairs();
         it += 2) {
      _word_graph.push_definition_hlt(
          word_graph().initial_node(), *it, *(it + 1));
    }

    if (kind() == congruence_kind::twosided
        && cbegin_generating_pairs() != cend_generating_pairs()) {
      // TODO avoid copy of presentation here
      Presentation<word_type> p = presentation();
      if (p.alphabet().size() != _word_graph.out_degree()) {
        LIBSEMIGROUPS_ASSERT(p.alphabet().size() == 0);
        p.alphabet(_word_graph.out_degree());
      }
      presentation::add_rules(
          p, cbegin_generating_pairs(), cend_generating_pairs());
      _word_graph.presentation(p);
    }
  }

  void ToddCoxeter::felsch() {
    _word_graph.process_definitions();

    auto& current  = _word_graph.cursor();
    current        = _word_graph.initial_node();
    size_t const n = _word_graph.out_degree();
    while (current != _word_graph.first_free_node() && !stopped()) {
      for (letter_type a = 0; a < n; ++a) {
        if (_word_graph.unsafe_neighbor(current, a) == UNDEFINED) {
          _word_graph.def_edge<StackDeductions>(
              current, a, _word_graph.new_node());
          _word_graph.process_definitions();
        }
      }
      current = _word_graph.next_active_node(current);
      if (report()) {
        report_active_nodes();
      }
    }
    if (!stopped()) {
      _finished = true;
    }
  }

  bool ToddCoxeter::contains(word_type const& lhs, word_type const& rhs) {
    validate_word(lhs);
    validate_word(rhs);
    if (presentation().rules.empty()
        && cbegin_generating_pairs() == cend_generating_pairs()
        && word_graph().number_of_nodes_active() == 1) {
      return lhs == rhs;
    }
    return v3::CongruenceInterface::contains(lhs, rhs);
  }

  void ToddCoxeter::shrink_to_fit() {
    if (!finished()) {
      // TODO Throw
      return;
    }
    standardize(order::shortlex);
    _word_graph.shrink_to_fit(_word_graph.number_of_nodes_active());
    _word_graph.erase_free_nodes();
  }

  bool ToddCoxeter::standardize(order val) {
    if (is_standardized(val)) {
      return false;
    }
    bool result   = action_digraph::standardize(_word_graph, _forest, val);
    _standardized = val;
    return result;
  }

  word_type ToddCoxeter::class_index_to_word_impl(class_index_type i) {
    run();
    LIBSEMIGROUPS_ASSERT(finished());
    if (!is_standardized()) {
      standardize(order::shortlex);
    }

    word_type w = *(forest::cbegin_paths(_forest) + i + 1);
    if (kind() != congruence_kind::left) {
      std::reverse(w.begin(), w.end());
    }
    return w;
  }

  size_t ToddCoxeter::number_of_classes_impl() {
    if (is_obviously_infinite(*this)) {
      return POSITIVE_INFINITY;
    }
    run();
    size_t const offset = (presentation().contains_empty_word() ? 0 : 1);
    return _word_graph.number_of_nodes_active() - offset;
  }

  class_index_type ToddCoxeter::word_to_class_index_impl(word_type const& w) {
    run();
    LIBSEMIGROUPS_ASSERT(finished());
    if (!is_standardized()) {
      standardize(order::shortlex);
    }
    return const_word_to_class_index(w);
    // c is in the range 1, ..., number_of_cosets_active() because 0
    // represents the identity coset, and does not correspond to an element.
  }

  class_index_type
  ToddCoxeter::const_word_to_class_index(word_type const& w) const {
    validate_word(w);
    node_type c = _word_graph.initial_node();

    if (kind() == congruence_kind::left) {
      c = action_digraph_helper::follow_path_nc(
          _word_graph, c, w.crbegin(), w.crend());
    } else {
      c = action_digraph_helper::follow_path_nc(
          _word_graph, c, w.cbegin(), w.cend());
    }
    return (c == UNDEFINED ? UNDEFINED : static_cast<node_type>(c - 1));
  }

  void ToddCoxeter::validate_word(word_type const& w) const {
    presentation().validate_word(w.cbegin(), w.cend());
  }
}  // namespace libsemigroups
