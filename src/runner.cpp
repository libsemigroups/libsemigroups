//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019-2023 James D. Mitchell
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

// This file contains implementations of the member functions for the Runner
// class.

#include "libsemigroups/runner.hpp"

#include "libsemigroups/exception.hpp"  // for LibsemigroupsException

#include "libsemigroups/detail/report.hpp"  // for report_default
#include "libsemigroups/detail/timer.hpp"   // for Timer::string

namespace libsemigroups {

  ////////////////////////////////////////////////////////////////////////
  // Reporter - constructors - public
  ////////////////////////////////////////////////////////////////////////

  Reporter& Reporter::init() {
    _prefix               = "";
    _report_time_interval = nanoseconds(std::chrono::seconds(1));
    reset_start_time();
    return *this;
  }

  ////////////////////////////////////////////////////////////////////////
  // Runner - constructors - public
  ////////////////////////////////////////////////////////////////////////

  Runner::Runner()
      : Reporter(), _run_for(FOREVER), _state(state::never_run), _stopper() {}

  Runner& Runner::init() {
    Reporter::init();
    _run_for = FOREVER;
    _state   = state::never_run;
    _stopper = decltype(_stopper)();
    return *this;
  }

  Runner::Runner(Runner const& other)
      : Reporter(other), _run_for(other._run_for), _state(), _stopper() {
    _state = other._state.load();
  }

  Runner::Runner(Runner&& other)
      : Reporter(std::move(other)),
        _run_for(std::move(other._run_for)),
        _state(),
        _stopper() {
    _state = other._state.load();
  }

  Runner& Runner::operator=(Runner const& other) {
    Reporter::operator=(other);
    _run_for = other._run_for;
    _state   = other._state.load();
    return *this;
  }

  Runner& Runner::operator=(Runner&& other) {
    Reporter::operator=(std::move(other));
    _run_for = std::move(other._run_for);
    _state   = other._state.load();
    return *this;
  }

  void Runner::run() {
    if (!finished() && !dead()) {
      set_state(state::running_to_finish);
      try {
        run_impl();
      } catch (LibsemigroupsException const& e) {
        if (!dead()) {
          set_state(state::not_running);
        }
        throw;
      }
      if (!dead()) {
        set_state(state::not_running);
      }
    }
  }

  void Runner::run_for(std::chrono::nanoseconds val) {
    if (!finished() && !dead()) {
      if (val != FOREVER) {
        report_default("running for approx. {}\n", detail::string_time(val));
      } else {
        report_default("running until finished, with no time limit\n");
        run();
        return;
      }
      auto previous_state = current_state();
      set_state(state::running_for);
      reset_start_time();
      _run_for = val;
      // run_impl should depend on the method timed_out!

      try {
        run_impl();
      } catch (LibsemigroupsException const& e) {
        set_state(previous_state);
        throw e;
      }
      if (!finished()) {
        if (!dead()) {
          set_state(state::timed_out);
        }
      } else {
        set_state(state::not_running);
      }
    } else {
      report_default("already finished, not running\n");
    }
  }

  void Runner::report_why_we_stopped() const {
    if (dead()) {
      report_default("killed!\n");
    } else if (timed_out()) {
      report_default("timed out!\n");
    } else if (stopped_by_predicate()) {
      report_default("stopped by predicate!\n");
    }
    // Checking finished can be expensive, so we don't
  }

}  // namespace libsemigroups
