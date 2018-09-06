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

#ifndef LIBSEMIGROUPS_INCLUDE_INTERNAL_RUNNER_HPP_
#define LIBSEMIGROUPS_INCLUDE_INTERNAL_RUNNER_HPP_

#include <atomic>
#include <chrono>

#include "report.hpp"
#include "stl.hpp"

namespace libsemigroups {
  constexpr std::chrono::nanoseconds FOREVER = std::chrono::nanoseconds::max();
  // TODO(now) move to constants.hpp

  class Runner {
   public:
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
    // Runner - non-virtual methods - public
    ////////////////////////////////////////////////////////////////////////

    // This method runs the algorithm for approximately the number of
    // nanoseconds indicated by the argument.
    void run_for(std::chrono::nanoseconds);
    template <typename TIntType>
    void run_for(TIntType t) {
      run_for(std::chrono::nanoseconds(t));
    }
    bool timed_out() const;

    template <typename TCallable>
    void run_until(TCallable const&         func,
                   std::chrono::nanoseconds check_interval
                   = std::chrono::milliseconds(2)) {
      static_assert(is_callable<TCallable>::value,
                    "the template parameter TCallable must be callable");
      static_assert(
          std::is_same<typename std::result_of<TCallable()>::type, bool>::value,
          "the template parameter TCallable must return a bool");
      while (!func() && !dead() && !finished()) {
        run_for(check_interval);
        if (check_interval < std::chrono::milliseconds(1024)) {
          check_interval *= 2;
        }
      }
    }
    template <typename TCallable, typename TIntType>
    void run_until(TCallable const& func, TIntType check_interval) {
      run_until(func, std::chrono::nanoseconds(check_interval));
    }

    // Returns true if we should report and false otherwise
    bool report() const;
    void report_every(std::chrono::nanoseconds);
    template <typename TIntType>
    void report_every(TIntType t) {
      report_every(std::chrono::nanoseconds(t));
    }
    void report_why_we_stopped() const;

    void set_finished(bool) const noexcept;
    bool finished() const;

    void kill() noexcept;
    bool dead() const noexcept;

    bool stopped() const;

   protected:
    ////////////////////////////////////////////////////////////////////////
    // Runner - non-pure virtual methods - protected
    ////////////////////////////////////////////////////////////////////////

    // TODO(later) compile-time polymorphism?
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
#endif  // LIBSEMIGROUPS_INCLUDE_INTERNAL_RUNNER_HPP_
