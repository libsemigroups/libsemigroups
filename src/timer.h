//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2016 James D. Mitchell
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

#ifndef LIBSEMIGROUPS_SRC_TIMER_H_
#define LIBSEMIGROUPS_SRC_TIMER_H_

#include <chrono>
#include <iostream>
#include <string>

#include "libsemigroups-debug.h"

namespace libsemigroups {

  //
  // This is a simple class to which can be used to send timing information to
  // the standard output.

  class Timer {
    typedef std::chrono::duration<int64_t, std::nano> nano_t;
    typedef std::chrono::steady_clock::time_point time_point_t;

   public:
    // Default constructor
    Timer() : _start(), _end(), _running(false) {}

    // Is the timer running?
    //
    // This method can be used to check if the timer is running.
    bool is_running() const {
      return _running;
    }

    // Start the timer
    //
    // This starts the timer running if it is not already running. If it is
    // already running, then it is reset.
    void start() {
      _running = true;
      _start   = std::chrono::steady_clock::now();
    }

    // Stop the timer
    // @str prepend this to the printed statement (defaults to "")
    //
    // Stops the timer regardless of its state..
    void stop() {
      _running = false;
      _end     = std::chrono::steady_clock::now();
    }

    // Print elapsed time
    // @str prepend this to the printed statement (defaults to "")
    //
    // If the timer is running, then this prints the time elapsed since <start>
    // was called. The format of the returned value is the time in some
    // (hopefully) human readable format.

    void print(std::string prefix = "") {
      if (_running) {
        std::cout << string(prefix);
      }
    }

    std::string string(std::string prefix = "") {
      nano_t elapsed;
      if (_running) {
        time_point_t end = std::chrono::steady_clock::now();
        elapsed          = std::chrono::duration_cast<nano_t>(end - _start);
      } else {
        elapsed = std::chrono::duration_cast<nano_t>(_end - _start);
      }

      if (string_it<std::chrono::hours>(elapsed, prefix, "h ", 0)) {
        string_it<std::chrono::minutes>(elapsed, prefix, "m", 0);
        return prefix;
      } else if (string_it<std::chrono::minutes>(elapsed, prefix, "m ", 0)) {
        string_it<std::chrono::seconds>(elapsed, prefix, "s", 0);
        return prefix;
      } else if (string_it<std::chrono::milliseconds>(
                     elapsed, prefix, "ms ", 9)) {
        return prefix;
      } else if (string_it<std::chrono::microseconds>(
                     elapsed, prefix, "\u03BCs ", 9)) {
        return prefix;
      } else if (string_it<std::chrono::nanoseconds>(
                     elapsed, prefix, "ns ", 0)) {
        return prefix;
      }
      return prefix;
    }

    int64_t elapsed() {  // in nanoseconds?
      if (_running) {
        return (std::chrono::steady_clock::now() - _start).count();
      } else {
        return (_end - _start).count();
      }
    }

   private:
    std::chrono::steady_clock::time_point _start;
    std::chrono::steady_clock::time_point _end;
    bool                                  _running;

    template <typename T>
    bool string_it(nano_t&      elapsed,
                   std::string& str,
                   std::string  unit,
                   size_t       threshold) {
      T x = std::chrono::duration_cast<T>(elapsed);
      if (x > T(threshold)) {
        str += std::to_string(x.count()) + unit;
        elapsed -= x;
        return true;
      }
      return false;
    }
  };
}  // namespace libsemigroups

#endif  // LIBSEMIGROUPS_SRC_TIMER_H_
