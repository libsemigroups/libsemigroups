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

// This file contains two classes Runner and Race for competitively running
// different functions/methods in different threads, and obtaining the winner.

#ifndef LIBSEMIGROUPS_INCLUDE_INTERNAL_RUNNER_H_
#define LIBSEMIGROUPS_INCLUDE_INTERNAL_RUNNER_H_

#include <atomic>
#include <chrono>
#include <thread>
#include <vector>

namespace libsemigroups {
  class Runner {
   public:
    static constexpr std::chrono::nanoseconds FOREVER
        = std::chrono::nanoseconds::max();

    Runner();
    virtual ~Runner() {}

    virtual void run() = 0;

    // This method runs the algorithm for approximately the number of
    // nanoseconds indicated by the argument.
    void run_for(std::chrono::nanoseconds val);

    template <typename TIntType> void run_for(TIntType time) {
      run_for(std::chrono::nanoseconds(time));
    }

    bool timed_out() const;

    // Returns true if we should report and false otherwise
    bool report() const;
    void report_every(std::chrono::nanoseconds val);
    template <typename TIntType> void report_every(TIntType time);

    virtual bool finished() const;
    void set_finished() const;
    void unset_finished() const;  // TODO use this! Currently only used in cong-p.h

    void kill();
    std::atomic<bool> const& dead() const;

   private:
    std::atomic<bool>                                      _dead;
    mutable bool                                           _finished;
    mutable std::chrono::high_resolution_clock::time_point _last_report;
    std::chrono::nanoseconds                               _run_for;
    std::chrono::nanoseconds                       _report_time_interval;
    std::chrono::high_resolution_clock::time_point _start_time;
  };
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_INCLUDE_INTERNAL_RUNNER_H_
