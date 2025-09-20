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

// This file contains implementations of the member functions for the Runner
// class.

#include "libsemigroups/runner.hpp"

#include "libsemigroups/exception.hpp"  // for LibsemigroupsException

#include "libsemigroups/detail/report.hpp"  // for report_default
#include "libsemigroups/detail/timer.hpp"   // for Timer::string

namespace libsemigroups {
  ////////////////////////////////////////////////////////////////////////
  // Reporter - constructors + initializers - public
  ////////////////////////////////////////////////////////////////////////

  Reporter::Reporter()
      : _divider(),
        _prefix(),
        _report_time_interval(),
        // mutable
        _last_report(time_point()),
        _start_time() {
    // All values set in init
    init();
  }

  Reporter& Reporter::init() {
    _divider              = "";
    _prefix               = "";
    _report_time_interval = nanoseconds(std::chrono::seconds(1));
    reset_start_time();
    return *this;
  }

  Reporter::Reporter(Reporter const& that)
      : _divider(that._divider),
        _prefix(that._prefix),
        _report_time_interval(that._report_time_interval),
        _last_report(that._last_report.load()),
        _start_time(that._start_time) {}

  Reporter::Reporter(Reporter&& that)
      : _divider(std::move(that._divider)),
        _prefix(std::move(that._prefix)),
        _report_time_interval(std::move(that._report_time_interval)),
        _last_report(that._last_report.load()),
        _start_time(std::move(that._start_time)) {}

  Reporter& Reporter::operator=(Reporter const& that) {
    _divider              = that._divider;
    _prefix               = that._prefix;
    _report_time_interval = that._report_time_interval;
    _last_report          = that._last_report.load();
    _start_time           = that._start_time;
    return *this;
  }

  Reporter& Reporter::operator=(Reporter&& that) {
    _divider              = std::move(that._divider);
    _prefix               = std::move(that._prefix);
    _report_time_interval = std::move(that._report_time_interval);
    _last_report          = that._last_report.load();
    _start_time           = std::move(that._start_time);
    return *this;
  }

  Runner::~Runner() = default;

  bool Reporter::report() const {
    auto t       = std::chrono::high_resolution_clock::now();
    auto elapsed = t - _last_report.load();

    if (elapsed > _report_time_interval) {
      _last_report = t;
      return true;
    } else {
      return false;
    }
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
      emit_divider();
      if (val != FOREVER) {
        report_default("{}: running for approx. {}\n",
                       report_prefix(),
                       detail::string_time(val));
      } else {
        report_default("{}: running until finished, with no time limit\n",
                       report_prefix());
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
      // This line is definitely tested, but not showing up in code coverage for
      // JDM
      // NOTE: no dividers here
      report_default("{}: already finished, not running\n", report_prefix());
    }
  }

  std::string Runner::string_why_we_stopped() const {
    // Checking finished can be expensive, so we don't
    if (dead()) {
      return "killed!";
    } else if (timed_out()) {
      // TODO(1) include the amount of time that we ran for
      return "timed out!";
    } else if (stopped_by_predicate()) {
      return "stopped by predicate!";
    }
    return "";
  }

  void Runner::report_why_we_stopped() const {
    // NOTE: Also no dividers here because we can call emit_divider in any
    // code calling this function
    report_default("{}: {}\n", report_prefix(), string_why_we_stopped());
  }

  [[nodiscard]] bool Runner::finished() const {
    if (started() && !dead() && finished_impl()) {
      _state = state::not_running;
      return true;
    } else {
      return false;
    }
    // since kill() may leave the object in an invalid state we only return
    // true here if we are not dead and the object thinks it is finished.
  }

}  // namespace libsemigroups
