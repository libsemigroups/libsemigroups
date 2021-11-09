//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019 James D. Mitchell
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

#include "libsemigroups/report.hpp"  // for REPORT_DEFAULT
#include "libsemigroups/timer.hpp"   // for Timer::string

namespace libsemigroups {
  Runner::Runner()
      : _last_report(std::chrono::high_resolution_clock::now()),
        _report_time_interval(),
        _run_for(FOREVER),
        _start_time(),
        _state(state::never_run),
        _stopper() {
    report_every(std::chrono::seconds(1));
  }

  void Runner::run_for(std::chrono::nanoseconds val) {
    if (!finished() && !dead()) {
      if (val != FOREVER) {
        REPORT_DEFAULT("running for approx. %s\n",
                       detail::Timer::string(val).c_str());
      } else {
        REPORT_DEFAULT("running until finished, with no time limit\n");
        run();
        return;
      }
      before_run();
      auto previous_state = get_state();
      set_state(state::running_for);
      _start_time = std::chrono::high_resolution_clock::now();
      _run_for    = val;
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
      REPORT_DEFAULT("already finished, not running\n");
    }
  }

  void Runner::report_every(std::chrono::nanoseconds val) {
    _last_report          = std::chrono::high_resolution_clock::now();
    _report_time_interval = val;
  }

  void Runner::report_why_we_stopped() const {
    if (dead()) {
      REPORT_DEFAULT("killed!\n");
    } else if (timed_out()) {
      REPORT_DEFAULT("timed out!\n");
    }
    // Checking finished can be expensive, so we don't
  }

}  // namespace libsemigroups
