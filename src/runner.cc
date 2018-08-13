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
  ////////////////////////////////////////////////////////////////////////
  // Runner - typedefs - public
  ////////////////////////////////////////////////////////////////////////

  constexpr std::chrono::nanoseconds Runner::FOREVER;

  ////////////////////////////////////////////////////////////////////////
  // Runner - constructors + destructor - public
  ////////////////////////////////////////////////////////////////////////

  Runner::Runner()
      : _dead(new std::atomic<bool>(false)),
        _delete_dead(true),
        _finished(new bool(false)),
        _delete_finished(true),
        _last_report(std::chrono::high_resolution_clock::now()),
        _run_for(FOREVER),
        _report_time_interval(),
        _start_time() {
    report_every(std::chrono::seconds(1));
  }

  Runner::~Runner() {
    if (_delete_dead) {
      delete _dead;
    }
    if (_delete_finished) {
      delete _finished;
    }
  }

  ////////////////////////////////////////////////////////////////////////
  // Runner - non-pure virtual methods - public
  ////////////////////////////////////////////////////////////////////////

  bool Runner::finished() const {
    return !(*_dead) && (*_finished);
    // Since kill() may leave the object in an invalid state we only return
    // true here if we are not dead and the object thinks it is finished.
  }

  ////////////////////////////////////////////////////////////////////////
  // Runner - non-pure non-virtual methods - public
  ////////////////////////////////////////////////////////////////////////

  void Runner::run_for(std::chrono::nanoseconds val) {
    if (!(*_finished)) {
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
    return (*_finished
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

  void Runner::set_finished(bool val) const {
    *_finished = val;
  }

  bool& Runner::get_finished() const {
    return *_finished;
  }

  void Runner::replace_finished(bool& val) {
    if (_delete_finished) {
      delete _finished;
    }
    _finished        = &val;
    _delete_finished = false;
  }

  void Runner::kill() {
    // TODO add killed-by-thread
    *_dead = true;
  }

  bool Runner::dead() const {
    return *_dead;
  }

  std::atomic<bool>& Runner::get_dead() const {
    return *_dead;
  }

  void Runner::replace_dead(std::atomic<bool>& dead) {
    if (_delete_dead) {
      delete _dead;
    }
    _dead        = &dead;
    _delete_dead = false;
  }
}  // namespace libsemigroups
