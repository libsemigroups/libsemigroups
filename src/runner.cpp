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

#include "runner.hpp"

#include "report.hpp"  // for REPORT_DEFAULT
#include "timer.hpp"   // for Timer::string

namespace libsemigroups {
  Runner::Runner()
      : _dead(false),
        _finished(false),
        _last_report(std::chrono::high_resolution_clock::now()),
        _report_time_interval(),
        _run_for(FOREVER),
        _start_time(),
        _started(false),
        _stopped_by_predicate(false),
        _stopper() {
    report_every(std::chrono::seconds(1));
  }

  Runner::Runner(Runner const& copy) : Runner() {
    _dead                 = (copy._dead ? true : false);
    _finished             = copy._finished;
    _started              = copy._started;
    _stopped_by_predicate = copy._stopped_by_predicate;
  }

  void Runner::run_for(std::chrono::nanoseconds val) {
    if (!finished_impl()) {
      if (val != FOREVER) {
        REPORT_DEFAULT("running for approx. %s\n", detail::Timer::string(val));
      } else {
        REPORT_DEFAULT("running until finished, with no time limit\n");
      }
      _start_time = std::chrono::high_resolution_clock::now();
      _run_for    = val;
      this->run();  // should depend on the method timed_out!
    } else {
      REPORT_DEFAULT("already finished, not running\n");
    }
  }

  bool Runner::timed_out() const {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
               std::chrono::high_resolution_clock::now() - _start_time)
           >= _run_for;
  }

  bool Runner::report() const {
    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::high_resolution_clock::now() - _last_report);
    if (elapsed > _report_time_interval) {
      _last_report = std::chrono::high_resolution_clock::now();
      return true;
    } else {
      return false;
    }
  }

  void Runner::report_every(std::chrono::nanoseconds val) {
    _last_report          = std::chrono::high_resolution_clock::now();
    _report_time_interval = val;
  }

  void Runner::report_why_we_stopped() const {
    if (finished()) {
      REPORT_DEFAULT("finished!\n");
    } else if (dead()) {
      REPORT_DEFAULT("killed!\n");
    } else if (timed_out()) {
      REPORT_DEFAULT("timed out!\n");
    }
  }

  bool Runner::stopped() const {
    if (dead() || timed_out() || stopped_by_predicate() || finished()) {
      return true;
    } else if (_stopper.valid() && _stopper()) {
      _stopped_by_predicate = true;
      return true;
    }
    return false;
  }

}  // namespace libsemigroups
