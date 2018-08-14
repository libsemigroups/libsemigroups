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

#include "report.h"

namespace libsemigroups {
  class Runner {
   public:
    ////////////////////////////////////////////////////////////////////////
    // Runner - typedefs - public
    ////////////////////////////////////////////////////////////////////////

    static constexpr std::chrono::nanoseconds FOREVER
        = std::chrono::nanoseconds::max();

    ////////////////////////////////////////////////////////////////////////
    // Runner - constructors + destructor - public
    ////////////////////////////////////////////////////////////////////////

    Runner();
    virtual ~Runner() {}

    ////////////////////////////////////////////////////////////////////////
    // Runner - pure virtual methods - public
    ////////////////////////////////////////////////////////////////////////

    virtual void run() = 0;

    ////////////////////////////////////////////////////////////////////////
    // Runner - non-pure non-virtual methods - public
    ////////////////////////////////////////////////////////////////////////

    // This method runs the algorithm for approximately the number of
    // nanoseconds indicated by the argument.
    void                              run_for(std::chrono::nanoseconds);
    template <typename TIntType> void run_for(TIntType t) {
      run_for(std::chrono::nanoseconds(t));
    }
    bool timed_out() const;

    template <typename TFunction, typename TIntType>
    void run_until(TFunction const& func, TIntType check_interval) {
      run_until(func, std::chrono::nanoseconds(check_interval));
    }
    template <typename TFunction>
    void run_until(TFunction const&         func,
                   std::chrono::nanoseconds check_interval
                   = std::chrono::milliseconds(50)) {
      // TODO check TFunction result etc
      while (!func(this) && !dead() && !finished()) {
        run_for(check_interval);
      }
    }

    // Returns true if we should report and false otherwise
    bool                              report() const;
    void                              report_every(std::chrono::nanoseconds);
    template <typename TIntType> void report_every(TIntType t) {
      report_every(std::chrono::nanoseconds(t));
    }

    template <typename TSubclass>
    void report_why_we_stopped(TSubclass const*) const {
      // TODO check TSubclass is subclass of this
      if (finished()) {
        ::libsemigroups::report<TSubclass>("finished!");
      } else if (dead()) {
        ::libsemigroups::report<TSubclass>("killed!");
      } else if (timed_out()) {
        ::libsemigroups::report<TSubclass>("timed out!");
      }
    }

    void set_finished(bool) const;
    bool finished() const;

    void kill();
    bool dead() const;

    bool stopped() const {
      return finished() || dead() || timed_out();
    }

   protected:
    ////////////////////////////////////////////////////////////////////////
    // Runner - non-pure virtual methods - protected
    ////////////////////////////////////////////////////////////////////////

    // TODO compile-time polymorphism?
    virtual bool finished_impl() const;
    virtual bool dead_impl() const;

   private:
    ////////////////////////////////////////////////////////////////////////
    // Runner - data - private
    ////////////////////////////////////////////////////////////////////////

    std::atomic<bool>                                      _dead;
    mutable bool                                           _finished;
    mutable std::chrono::high_resolution_clock::time_point _last_report;
    std::chrono::nanoseconds                               _run_for;
    std::chrono::nanoseconds                       _report_time_interval;
    std::chrono::high_resolution_clock::time_point _start_time;
  };
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_INCLUDE_INTERNAL_RUNNER_H_
