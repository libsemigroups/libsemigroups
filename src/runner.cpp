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

#include "runner.hpp"
#include "timer.hpp"

namespace libsemigroups {
  ////////////////////////////////////////////////////////////////////////
  // Runner - constructor - public
  ////////////////////////////////////////////////////////////////////////

  Runner::Runner()
      : _dead(false),
        _finished(false),
        _last_report(std::chrono::high_resolution_clock::now()),
        _run_for(FOREVER),
        _report_time_interval(),
        _start_time() {
    report_every(std::chrono::seconds(1));
  }

  ////////////////////////////////////////////////////////////////////////
  // Runner - non-virtual methods - public
  ////////////////////////////////////////////////////////////////////////

  void Runner::run_for(std::chrono::nanoseconds val) {
    if (!finished_impl()) {
      if (val != FOREVER) {
        REPORT("running for approx. ", internal::Timer::string(val));
      } else {
        REPORT("running until finished, with no time limit");
      }
      _start_time = std::chrono::high_resolution_clock::now();
      _run_for    = val;
      this->run();  // should depend on the method timed_out!
      _start_time = std::chrono::high_resolution_clock::now();
      _run_for    = FOREVER;
    } else {
      REPORT("already finished, not running");
    }
  }

  bool Runner::timed_out() const {
    return (finished_impl()
            || std::chrono::duration_cast<std::chrono::nanoseconds>(
                   std::chrono::high_resolution_clock::now() - _start_time)
                   >= _run_for);
  }

  ////////////////////////////////////////////////////////////////////////

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
      REPORT("finished!");
    } else if (dead()) {
      REPORT("killed!");
    } else if (timed_out()) {
      REPORT("timed out!");
    }
  }

  ////////////////////////////////////////////////////////////////////////

  bool Runner::finished() const {
    return !(dead_impl()) && finished_impl();
    // Since kill() may leave the object in an invalid state we only return
    // true here if we are not dead and the object thinks it is finished.
  }

  void Runner::set_finished(bool val) const noexcept {
    _finished = val;
  }

  ////////////////////////////////////////////////////////////////////////

  void Runner::kill() noexcept {
    // TODO add killed-by-thread
    _dead = true;
  }

  bool Runner::dead() const noexcept {
    return dead_impl();
  }

  ////////////////////////////////////////////////////////////////////////

  bool Runner::stopped() const {
    return finished() || dead() || timed_out();
  }

  ////////////////////////////////////////////////////////////////////////
  // Runner - non-pure virtual methods - protected
  ////////////////////////////////////////////////////////////////////////

  bool Runner::dead_impl() const {
    return _dead;
  }

  bool Runner::finished_impl() const {
    return _finished;
  }

}  // namespace libsemigroups
