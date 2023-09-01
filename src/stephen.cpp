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

// This file contains an implementation of Stephen's procedure for finitely
// presented semigroups and monoids.

#include "libsemigroups/stephen.hpp"

#include <chrono>                          // for duration, duration...
#include <tuple>                           // for tie, tuple
#include <utility>                         // for move, pair
                                           //
#include "libsemigroups/constants.hpp"     // for operator==, Undefined
#include "libsemigroups/debug.hpp"         // for LIBSEMIGROUPS_ASSERT
#include "libsemigroups/exception.hpp"     // for LibsemigroupsExcep...
#include "libsemigroups/presentation.hpp"  // for Presentation<>::wo...
#include "libsemigroups/types.hpp"         // for word_type

#include "libsemigroups/detail/node-managed-graph.hpp"  // for StephenDigraph...
#include "libsemigroups/detail/report.hpp"              // for REPORT_DEFAULT_V3
#include "libsemigroups/detail/string.hpp"              // for group_digits

namespace libsemigroups {

  ////////////////////////////////////////////////////////////////////////
  // Public
  ////////////////////////////////////////////////////////////////////////

  Stephen::Stephen()
      : _finished(false), _accept_state(UNDEFINED), _word(), _word_graph() {}

  Stephen& Stephen::set_word(word_type const& w) {
    presentation().validate_word(w.cbegin(), w.cend());
    reset();
    _word = w;
    return *this;
  }

  Stephen& Stephen::set_word(word_type&& w) {
    presentation().validate_word(w.cbegin(), w.cend());
    reset();
    _word = std::move(w);
    return *this;
  }

  Stephen::node_type Stephen::accept_state() {
    if (_accept_state == UNDEFINED) {
      using word_graph::last_node_on_path_no_checks;
      run();
      _accept_state = last_node_on_path_no_checks(
                          _word_graph, 0, _word.cbegin(), _word.cend())
                          .first;
    }
    return _accept_state;
  }

  ////////////////////////////////////////////////////////////////////////
  // Private Member Functions
  ////////////////////////////////////////////////////////////////////////

  void Stephen::init_impl(Presentation<word_type>&& p, non_lvalue_tag) {
    if (p.alphabet().empty()) {
      LIBSEMIGROUPS_EXCEPTION(
          "the argument (Presentation) must not have 0 generators");
    }
    reset();
    _presentation = std::move(p);
    presentation::normalize_alphabet(_presentation);
    _word_graph.init(_presentation);
    _word.clear();
  }

  void Stephen::report_status(
      std::chrono::high_resolution_clock::time_point const& start_time) {
    if (!report()) {
      return;
    }
    using std::chrono::duration_cast;
    using std::chrono::seconds;
    auto now        = std::chrono::high_resolution_clock::now();
    auto total_time = duration_cast<seconds>(now - start_time);

    auto&   stats = _word_graph.stats();
    int64_t diff  = int64_t(_word_graph.number_of_nodes_active()
                           - stats.prev_active_nodes);

    // TODO(v3) use fmtlib
    static bool first_call = true;
    if (first_call) {
      first_call = false;
      REPORT_DEFAULT_V3("Stephen: " + std::string(60, '-') + "\n");
      REPORT_DEFAULT_V3("Stephen: %11s | %11s | %11s | %11s |\n",
                        "nodes",
                        "defined",
                        "killed",
                        "diff");
      REPORT_DEFAULT_V3("Stephen: " + std::string(60, '-') + "\n");
    }
    REPORT_DEFAULT_V3(
        "Stephen: %11s | %11s | %11s | %11s | "
        "(%llus)\n",
        detail::group_digits(_word_graph.number_of_nodes_active()).c_str(),
        ("+"
         + detail::group_digits(int64_t(_word_graph.number_of_nodes_defined()
                                        - stats.prev_nodes_defined)))
            .c_str(),
        ("-"
         + detail::group_digits(int64_t(_word_graph.number_of_nodes_killed()
                                        - stats.prev_nodes_killed)))
            .c_str(),
        ((diff < 0 ? "" : "+") + detail::group_digits(diff)).c_str(),
        total_time.count());
    _word_graph.stats_check_point();
  }

  void Stephen::reset() noexcept {
    // _initted      = false;  // HERE
    _finished     = false;
    _accept_state = UNDEFINED;
  }

  void Stephen::run_impl() {
    auto start_time = std::chrono::high_resolution_clock::now();
    // if (!initted()) {
    validate();  // throws if no presentation is defined
    _word_graph.init(presentation());
    std::ignore = _word_graph.complete_path(0, _word.cbegin(), _word.cend());
    // }
    node_type& current     = _word_graph.cursor();
    auto const rules_begin = presentation().rules.cbegin();
    auto const rules_end   = presentation().rules.cend();
    bool       did_change  = true;

    do {
      current    = 0;
      did_change = false;
      while (current != _word_graph.first_free_node() && !stopped()) {
        for (auto it = rules_begin; it < rules_end; it += 2) {
          node_type                 u_end;
          word_type::const_iterator rit;
          bool                      did_def = false;
          std::tie(u_end, rit) = word_graph::last_node_on_path_no_checks(
              _word_graph, current, it->cbegin(), it->cend());
          node_type c;
          if (rit == it->cend()) {
            ++it;
            if (it->empty()) {
              // TODO move this logic into complete_path
              did_def = false;
              c       = current;
            } else {
              std::tie(did_def, c) = _word_graph.complete_path(
                  current, it->cbegin(), it->cend() - 1);
            }
            node_type v_end;
            if (it->empty()) {
              v_end = c;
            } else {
              v_end = _word_graph.target_no_checks(c, it->back());
            }
            if (v_end == UNDEFINED) {
              LIBSEMIGROUPS_ASSERT(!it->empty());
              did_def = true;
              _word_graph.set_target_no_checks(c, it->back(), u_end);
            } else if (u_end != v_end) {
              did_def = true;
              _word_graph.merge_nodes_no_checks(u_end, v_end);
              _word_graph.process_coincidences<false>();
            }
            --it;
          } else {
            ++it;
            node_type v_end;
            std::tie(v_end, rit) = word_graph::last_node_on_path_no_checks(
                _word_graph, current, it->cbegin(), it->cend());
            if (rit == it->cend()) {
              --it;
              if (it->empty()) {
                // TODO move this logic into complete_path
                continue;
              } else {
                c = _word_graph
                        .complete_path(current, it->cbegin(), it->cend() - 1)
                        .second;
              }
              u_end = _word_graph.target_no_checks(c, it->back());
              LIBSEMIGROUPS_ASSERT(u_end == UNDEFINED);
              _word_graph.set_target_no_checks(c, it->back(), v_end);
              did_def = true;
            } else {
              --it;
            }
          }
          did_change |= did_def;
        }
        report_status(start_time);
        current = _word_graph.next_active_node(current);
      }
    } while (did_change && !stopped());
    if (!stopped()) {
      _finished = true;
      standardize();
    }
  }

  void Stephen::standardize() {
    word_graph::standardize(_word_graph);
    _word_graph.induced_subgraph_no_checks(
        0, _word_graph.number_of_nodes_active());
  }

  void Stephen::validate() const {
    if (_presentation.alphabet().empty()) {
      LIBSEMIGROUPS_EXCEPTION(
          "no presentation defined, use Stephen::init to set the presentation");
    }
  }

  namespace stephen {

    bool accepts(Stephen& s, word_type const& w) {
      using word_graph::last_node_on_path;
      s.run();
      return s.accept_state()
             == last_node_on_path(s.word_graph(), 0, w.cbegin(), w.cend())
                    .first;
    }

    bool is_left_factor(Stephen& s, word_type const& w) {
      using word_graph::last_node_on_path;
      s.run();
      return last_node_on_path(s.word_graph(), 0, w.cbegin(), w.cend()).second
             == w.cend();
    }
  }  // namespace stephen

}  // namespace libsemigroups
