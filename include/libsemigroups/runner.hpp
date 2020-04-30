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

// This file contains two classes Runner and Race for competitively running
// different functions/member functions in different threads, and obtaining the
// winner.

#ifndef LIBSEMIGROUPS_RUNNER_HPP_
#define LIBSEMIGROUPS_RUNNER_HPP_

#include <atomic>       // for atomic
#include <chrono>       // for nanoseconds, high_resolution_clock
#include <type_traits>  // for forward

#include "function-ref.hpp"             // for FunctionRef
#include "libsemigroups-exception.hpp"  // for LibsemigroupsException
#include "report.hpp"                   // for REPORT_DEFAULT

namespace libsemigroups {
  //! A pseudonym for std::chrono::nanoseconds::max().
  constexpr std::chrono::nanoseconds FOREVER = std::chrono::nanoseconds::max();

  //! Derived classes of this abstract class must implement a member function
  //! Runner::run_impl. The Runner class exists to collect various common tasks
  //! required by such a derived class with a possibly long running
  //! Runner::run_impl
  //! implementation. These include:
  //! * running for a given amount of time (Runner::run_for)
  //! * running until a nullary predicate is true (Runner::run_until)
  //! * reporting after a given amount of time (Runner::report_every)
  //! * checking if the given amount of time has elapsed since last report
  //! (Runner::report)
  //! * checking the status of the Runner::run implementation: has it
  //! Runner::started?  Runner::finished? been killed by another thread
  //! (Runner::dead)? has it timed out (Runner::timed_out)? has it
  //! Runner::stopped for any reason?
  //! * permit the Runner::run implementation to be killed from another thread
  //! (Runner::kill).
  //!
  //! The implementation of the ``run_impl`` member function in a derived class
  //! must periodically check whether or not it has stopped for this to work.
  class Runner {
    // Enum class for the state of the Runner.
    enum class state {
      never_run            = 0,
      running_to_finish    = 1,
      running_for          = 2,
      running_until        = 3,
      timed_out            = 4,
      stopped_by_predicate = 6,
      not_running          = 7,
      dead                 = 8
    };

   public:
    ////////////////////////////////////////////////////////////////////////
    // Runner - constructors + destructor - public
    ////////////////////////////////////////////////////////////////////////

    //! Default constructor.
    //!
    //! Returns a runner that is not started, not finished, not dead, not timed
    //! out, will run libsemigroups::FOREVER if not instructed otherwise,  that
    //! last reported at the time of construction, and that will report every
    //! std::chrono::seconds(1) if reporting is enabled.
    //!
    //! \par Parameters
    //! (None)
    Runner();

    //! Copy constructor.
    //!
    //! Returns a runner that is a copy of \p copy. The state of the new runner
    //! is the same as \p copy, except that the function passed as an argument
    //! to run_until (if any) is not copied.
    //!
    //! \param other the Runner to copy.
    Runner(Runner const& other) : Runner() {
      _state = other._state.load();
    }

    //! Move constructor
    //!
    //! Returns a runner that is initialised from \p other. The state of the
    //! new runner is the same as \p copy, except that the function passed as
    //! an argument to run_until (if any) is not copied.
    //!
    //! \param other the Runner to move from.
    Runner(Runner&& other) : Runner() {
      _state = other._state.load();
    }

    //! Deleted.
    Runner& operator=(Runner const&) = delete;

    //! Deleted.
    Runner& operator=(Runner&&) = delete;

    virtual ~Runner() = default;

    ////////////////////////////////////////////////////////////////////////
    // Runner - pure virtual member functions - public
    ////////////////////////////////////////////////////////////////////////

    //! Member function that wraps `run_impl` which is implemented in a derived
    //! class.
    //!
    //! Run the main algorithm implemented by a derived class derived of
    //! Runner.
    //!
    //! \returns
    //! (None)
    //!
    //! \par Parameters
    //! (None)
    // At the end of this either finished, or dead.
    void run() {
      if (!finished() && !dead()) {
        before_run();
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

    ////////////////////////////////////////////////////////////////////////
    // Runner - non-virtual member functions - public
    ////////////////////////////////////////////////////////////////////////

    //! Run for a specified amount of time.
    //!
    //! For this to work it is necessary to periodically check if
    //! Runner::timed_out returns \c true, and to stop if it is, in the
    //! Runner::run member function of any derived class of Runner.
    //!
    //! \param t the time in nanoseconds to run for.
    //!
    //! \returns
    //! (None)
    //!
    //! \sa Runner::run_for(TIntType)
    // At the end of this either finished, dead, or timed_out.
    void run_for(std::chrono::nanoseconds t);

    //! Run for a specified amount of time.
    //!
    //!
    //! For this to work it is necessary to periodically check if
    //! Runner::timed_out returns \c true, and to stop if it is, in the
    //! Runner::run member function of any derived class of Runner.
    //!
    //! \param t the time to run for (in ``TIntType``).
    //!
    //! \returns
    //! (None)
    //!
    //! \sa Runner::run_for(std::chrono::nanoseconds)
    template <typename TIntType>
    void run_for(TIntType t) {
      run_for(std::chrono::nanoseconds(t));
    }

    //! Check if the amount of time specified to Runner::run_for has elapsed.
    //!
    //! \par Parameters
    //! (None)
    //!
    //! \returns
    //! A ``bool``
    //!
    //! \sa Runner::run_for(std::chrono::nanoseconds) and
    //! Runner::run_for(TIntType).
    bool timed_out() const {
      return (running_for()
                  ? std::chrono::duration_cast<std::chrono::nanoseconds>(
                        std::chrono::high_resolution_clock::now() - _start_time)
                        >= _run_for
                  : get_state() == state::timed_out);
    }

    //! Run until a nullary predicate returns \p true or Runner::finished.
    //!
    //! \param func a callable type that will exist for at least until this
    //! function returns.
    //!
    //! \returns
    //! (None)
    // At the end of this either finished, dead, or stopped_by_predicate.
    template <typename T>
    void run_until(T&& func) {
      REPORT_DEFAULT("running until predicate returns true or finished. . .\n");
      if (!finished() && !dead()) {
        before_run();
        _stopper = std::forward<T>(func);
        if (!_stopper()) {
          set_state(state::running_until);
          run_impl();
          if (!finished()) {
            if (!dead()) {
              set_state(state::stopped_by_predicate);
            }
          } else {
            set_state(state::not_running);
          }
        }
        _stopper.invalidate();
      }
    }

    //! Run until a nullary predicate returns \p true or Runner::finished.
    //!
    //! \param func a function pointer.
    //!
    //! \returns
    //! (None)
    void run_until(bool (*func)()) {
      run_until(detail::FunctionRef<bool(void)>(func));
    }

    //! Check if it is time to report.
    //!
    //! This function can be used in an implementation of Runner::run (in a
    //! derived class of Runner) to check if enough time has passed that we
    //! should report again.
    //!
    //! \returns
    //! A ``bool``.
    //!
    //! \par Parameters
    //! (None)
    //!
    //! \sa report_every(std::chrono::nanoseconds) and report_every(TIntType).
    bool report() const;

    //! Set the minimum elapsed time between reports.
    //!
    //! This function can be used to specify at run time the minimum elapsed
    //! time between two calls to Runner::report that return \c true. If
    //! Runner::report returns \c true at time \c s, then Runner::report will
    //! only return \c true again after time \c s + \c t has elapsed.
    //!
    //! \param t the amount of time (in nanoseconds) between reports.
    //!
    //! \returns
    //! (None)
    //!
    //! \sa report_every(TIntType)
    void report_every(std::chrono::nanoseconds t);

    //! Set the minimum elapsed time between reports.
    //!
    //! This function can be used to specify at run time the minimum elapsed
    //! time between two calls to Runner::report that return \c true. If
    //! Runner::report returns \c true at time \c s, then Runner::report will
    //! only return \c true again after time \c s + \c t has elapsed.
    //!
    //! \param t the amount of time (in ``TIntType``) between reports.
    //!
    //! \returns
    //! (None)
    //!
    //! \sa report_every(std::chrono::nanoseconds)
    template <typename TIntType>
    void report_every(TIntType t) {
      report_every(std::chrono::nanoseconds(t));
    }

    //! Report why Runner::run stopped.
    //!
    //! Reports whether Runner::run was stopped because it is Runner::finished,
    //! Runner::timed_out, or Runner::dead.
    //!
    //! \par Parameters
    //! (None)
    //!
    //! \returns
    //! (None)
    void report_why_we_stopped() const;

    //! Check if Runner::run has been run to completion or not.
    //!
    //! Returns \c true if Runner::run has been run to completion. For this to
    //! work, the implementation of Runner::run in a derived class of Runner
    //! must either use Runner::set_finished or implement a specialisation of
    //! Runner::finished_impl.
    //!
    //! \par Parameters
    //! (None)
    //!
    //! \returns
    //! A ``bool``.
    //!
    //! \sa Runner::started
    bool finished() const {
      if (started() && !dead() && finished_impl()) {
        _state = state::not_running;
        return true;
      } else {
        return false;
      }
      // since kill() may leave the object in an invalid state we only return
      // true here if we are not dead and the object thinks it is finished.
    }

    //! Check if Runner::run has already been called.
    //!
    //! Returns \c true if Runner::run has started to run (it can be running or
    //! not).
    //!
    //! \par Parameters
    //! (None)
    //!
    //! \returns
    //! A ``bool``.
    //!
    //! \sa Runner::finished
    bool started() const {
      return get_state() != state::never_run;
    }

    //! Check if a Runner instance is currently running.
    //!
    //! \returns
    //! \c true if Runner::run is in the process to run and \c false it is not.
    //!
    //! \par Parameters
    //! (None)
    //!
    //! \returns
    //! A ``bool``.
    //!
    //! \sa Runner::finished
    bool running() const noexcept {
      return get_state() == state::running_to_finish
             || get_state() == state::running_for
             || get_state() == state::running_until;
    }

    //! Stop Runner::run from running (thread-safe).
    //!
    //! This function can be used to terminate Runner::run from another thread.
    //! After Runner::kill has been called the Runner may no longer be in a
    //! valid state, but will return \c true from Runner::dead.
    //!
    //! \par Parameters
    //! (None)
    //!
    //! \returns
    //! (None).
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \sa Runner::finished
    void kill() noexcept {
      set_state(state::dead);
    }

    //! Check if the runner is dead.
    //!
    //! This function can be used to check if we should terminate Runner::run
    //! because it has been killed by another thread.
    //!
    //! \par Parameters
    //! (None)
    //!
    //! \returns
    //! A ``bool``.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \sa Runner::kill
    bool dead() const noexcept {
      return get_state() == state::dead;
    }

    //! Check if the runner is stopped.
    //!
    //! This function can be used to check whether or not Runner::run has been
    //! stopped for whatever reason. In other words, it checks if
    //! Runner::timed_out, Runner::finished, or Runner::dead.
    //!
    //! \returns
    //! A ``bool``.
    //!
    //! \par Parameters
    //! (None)
    bool stopped() const {
      return (running() ? (timed_out() || stopped_by_predicate())
                        : get_state() > state::running_until);
    }

    //! Check if the runner was, or should, stop because the nullary predicate
    //! passed as first argument to Runner::run_until.
    //!
    //! If \c this is running, then the nullary predicate is called and its
    //! return value is returned. If \c this is not running, then \c true is
    //! returned if and only if the last time \c this was running it was
    //! stopped by a call to the nullary predicate passed to Runner::run_until.
    //!
    //! \returns
    //! A ``bool``.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Constant.
    //!
    //! \par Parameters
    //! (None)
    // noexcept should depend on whether _stopper can throw or not
    bool stopped_by_predicate() const {
      if (running_until()) {
        LIBSEMIGROUPS_ASSERT(_stopper.valid());
        return _stopper();
      } else {
        return get_state() == state::stopped_by_predicate;
      }
    }

   private:
    bool running_for() const noexcept {
      return _state == state::running_for;
    }

    bool running_until() const noexcept {
      return _state == state::running_until;
    }

    virtual void run_impl()            = 0;
    virtual bool finished_impl() const = 0;
    virtual void before_run() {}

    state get_state() const noexcept {
      return _state;
    }

    void set_state(state stt) const {
      LIBSEMIGROUPS_ASSERT(stt != state::never_run);
      if (!dead()) {
        // It can be that *this* becomes dead after this function has been
        // called.
        _state = stt;
      }
    }

    ////////////////////////////////////////////////////////////////////////
    // Runner - data - private
    ////////////////////////////////////////////////////////////////////////

    mutable std::chrono::high_resolution_clock::time_point _last_report;
    std::chrono::nanoseconds                       _report_time_interval;
    std::chrono::nanoseconds                       _run_for;
    std::chrono::high_resolution_clock::time_point _start_time;
    mutable std::atomic<state>                     _state;
    detail::FunctionRef<bool(void)>                _stopper;
  };
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_RUNNER_HPP_
