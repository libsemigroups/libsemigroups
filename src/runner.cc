//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2018 James D. Mitchell
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

// This file contains implementations of the methods for the Runner class.

#include "internal/runner.h"
#include "internal/report.h"

namespace libsemigroups {
  constexpr std::chrono::nanoseconds Runner::FOREVER;

  Runner::Runner()
      : _dead(false),
        _finished(false),
        _last_report(std::chrono::high_resolution_clock::now()),
        _run_for(FOREVER) {
    report_every(std::chrono::seconds(1));
  }

  void Runner::run_for(std::chrono::nanoseconds val) {
    if (!_finished) {
      if (val != FOREVER) {
        REPORT("running for approx. " << Timer::string(val));
      } else {
        REPORT("running until finished, with no time limit");
      }
      _start_time = std::chrono::high_resolution_clock::now();
      _run_for    = val;
      this->run();
      _start_time = std::chrono::high_resolution_clock::now();
      _run_for    = FOREVER;
    } else {
      REPORT("already finished, not running");
    }
  }

  bool Runner::timed_out() const {
    return (_finished
            || std::chrono::duration_cast<std::chrono::milliseconds>(
                   std::chrono::high_resolution_clock::now() - _start_time)
                   >= _run_for);
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

  template <typename TIntType> void Runner::report_every(TIntType time) {
    report_every(std::chrono::nanoseconds(time));
  }

  bool Runner::finished() const {
    return !_dead && _finished;
    // Since kill() may leave the object in an invalid state we only return
    // true here if we are not dead and the object thinks it is finished.
  }

  void Runner::set_finished() const {
    _finished = true;
  }

  void Runner::unset_finished() const {
    _finished = false;
  }

  void Runner::kill() {
    // TODO add killed-by-thread
    _dead = true;
  }

  std::atomic<bool> const& Runner::dead() const {
    return _dead;
  }
}  // namespace libsemigroups
