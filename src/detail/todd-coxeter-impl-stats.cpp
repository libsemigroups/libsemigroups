//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019-2025 James D. Mitchell
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

// This file contains a declaration of the ToddCoxeterImpl member functions for
// statistics.

#include "libsemigroups/detail/todd-coxeter-impl.hpp"

namespace libsemigroups::detail {

  ////////////////////////////////////////////////////////////////////////
  // ToddCoxeterImpl::NonAtomicStats
  ////////////////////////////////////////////////////////////////////////

  ToddCoxeterImpl::NonAtomicStats& ToddCoxeterImpl::NonAtomicStats::init() {
    create_or_init_time = std::chrono::high_resolution_clock::now();
    run_index           = 0;

    all_num_hlt_phases       = 0;
    all_num_felsch_phases    = 0;
    all_num_lookahead_phases = 0;
    run_num_hlt_phases       = 0;
    run_num_felsch_phases    = 0;
    run_num_lookahead_phases = 0;
    return *this;
  }

  ////////////////////////////////////////////////////////////////////////
  // ToddCoxeterImpl mem fns
  ////////////////////////////////////////////////////////////////////////

  void ToddCoxeterImpl::stats_run_start() {
    _stats.run_start_time = std::chrono::high_resolution_clock::now();

    _stats.run_nodes_active_at_start
        = current_word_graph().number_of_nodes_active();
    _stats.run_edges_active_at_start
        = current_word_graph().number_of_edges_active();
    _stats.run_num_hlt_phases       = 0;
    _stats.run_num_felsch_phases    = 0;
    _stats.run_num_lookahead_phases = 0;

    _stats.run_hlt_phases_time       = std::chrono::nanoseconds(0);
    _stats.run_felsch_phases_time    = std::chrono::nanoseconds(0);
    _stats.run_lookahead_phases_time = std::chrono::nanoseconds(0);

    _stats.phase_index = 0;
  }

  void ToddCoxeterImpl::stats_run_stop() {
    _stats.run_index++;

    _stats.all_runs_time += delta(_stats.run_start_time);
    _stats.all_num_hlt_phases += _stats.run_num_hlt_phases;
    _stats.all_num_felsch_phases += _stats.run_num_felsch_phases;
    _stats.all_num_lookahead_phases += _stats.run_num_lookahead_phases;

    _stats.all_hlt_phases_time += _stats.run_hlt_phases_time;
    _stats.all_felsch_phases_time += _stats.run_felsch_phases_time;
    _stats.all_lookahead_phases_time += _stats.run_lookahead_phases_time;
  }

  void ToddCoxeterImpl::stats_phase_start() {
    _stats.phase_start_time = std::chrono::high_resolution_clock::now();
    _stats.report_index     = 0;

    _stats.phase_nodes_active_at_start
        = current_word_graph().number_of_nodes_active();
    _stats.phase_nodes_killed_at_start
        = current_word_graph().number_of_nodes_killed();
    _stats.phase_nodes_defined_at_start
        = current_word_graph().number_of_nodes_defined();

    _stats.phase_edges_active_at_start
        = current_word_graph().number_of_edges_active();
    _stats.phase_complete_at_start
        = complete(current_word_graph().number_of_edges_active());
  }

  void ToddCoxeterImpl::stats_phase_stop() {
    _stats.phase_index++;

    switch (_state) {
      case state::none: {
        break;
      }
      case state::hlt: {
        _stats.run_num_hlt_phases++;
        _stats.run_hlt_phases_time += delta(_stats.phase_start_time);
        break;
      }
      case state::felsch: {
        _stats.run_num_felsch_phases++;
        _stats.run_felsch_phases_time += delta(_stats.phase_start_time);
        break;
      }
      case state::lookahead: {
        _stats.run_num_lookahead_phases++;
        _stats.run_lookahead_phases_time += delta(_stats.phase_start_time);
        break;
      }
      case state::lookbehind: {
        // intentional fall through, not currently collecting stats here
      }
      default: {
        break;
      }
    }
  }

  void ToddCoxeterImpl::stats_report_stop() const {
    _stats.report_index++;
  }
}  // namespace libsemigroups::detail
