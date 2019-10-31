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

#ifndef LIBSEMIGROUPS_INCLUDE_RUNNER_HPP_
#define LIBSEMIGROUPS_INCLUDE_RUNNER_HPP_

#include <atomic>       // for atomic
#include <chrono>       // for nanoseconds, high_resolution_clock
#include <type_traits>  // for forward

#include "function-ref.hpp"  // for FunctionRef

namespace libsemigroups {
  //! A pseudonym for std::chrono::nanoseconds::max().
  constexpr std::chrono::nanoseconds FOREVER = std::chrono::nanoseconds::max();

  //! Derived classes of this abstract class must implement a member function
  //! Runner::run. The Runner class exists to collect various common tasks
  //! required by such a derived class with a possibly long running Runner::run
  //! implementation. These include:
  //! * running for a given amount of time (Runner::run_for)
  //! * running until a unary predicate is true (Runner::run_until)
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
  //! The implementation of the ``run`` member function in a derived class must
  //! periodically check whether or not it has stopped for any reason for this
  //! to work.
  class Runner {
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
    //! \param copy the Runner to copy.
    Runner(Runner const& copy);

    //! Deleted.
    Runner(Runner&&) = delete;

    //! Deleted.
    Runner& operator=(Runner const&) = delete;

    //! Deleted.
    Runner& operator=(Runner&&) = delete;

    virtual ~Runner() = default;

    ////////////////////////////////////////////////////////////////////////
    // Runner - pure virtual member functions - public
    ////////////////////////////////////////////////////////////////////////

    //! Pure virtual member function implemented in derived class.
    //!
    //! The run method of any class derived from Runner ought to be the main
    //! algorithm that it implements.
    //!
    //! \returns
    //! (None)
    //!
    //! \par Parameters
    //! (None)
    virtual void run() = 0;

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
    void run_for(std::chrono::nanoseconds t);

    //! Run for a specified amount of time.
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
    bool timed_out() const;

    //! Run until a nullary predicate returns \p true or Runner::finished.
    //!
    //! \param func a callable type that will exist for at least until this
    //! function returns.
    //!
    //! \returns
    //! (None)
    template <typename T>
    void run_until(T&& func) {
      _stopper              = std::forward<T>(func);
      _stopped_by_predicate = false;
      run();
      _stopped_by_predicate = _stopper();
      _stopper.invalidate();
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
      return !(dead_impl()) && finished_impl();
      // Since kill() may leave the object in an invalid state we only return
      // true here if we are not dead and the object thinks it is finished.
    }

    //! Check if Runner::run has already been called.
    //!
    //! Returns \c true if Runner::run has started to run (it can be running or
    //! not). For this to
    //! work, the implementation of Runner::run in a derived class of Runner
    //! must either use Runner::set_started or implement a specialisation of
    //! Runner::started_impl.
    //!
    //! \par Parameters
    //! (None)
    //!
    //! \returns
    //! A ``bool``.
    //!
    //! \sa Runner::finished
    bool started() const {
      return finished() || started_impl();
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
      _dead = true;
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
      return dead_impl();
    }

    //! Check if the runner is stopped.
    //!
    //! This function can be used to check whether or not Runner::run has been
    //! stopped for whatever reason. In other words, it checks if
    //! Runner::timed_out, Runner::finished, or Runner::dead.
    //!
    //! \par Parameters
    //! (None)
    //!
    //! \returns
    //! A ``bool``.
    bool stopped() const;

    void clear_stoppage() {
      _run_for              = FOREVER;
      _start_time           = std::chrono::high_resolution_clock::now();
      _stopped_by_predicate = false;
    }

   protected:
    ////////////////////////////////////////////////////////////////////////
    // Runner - non-pure virtual member functions - protected
    ////////////////////////////////////////////////////////////////////////
    //! Stub
    virtual bool dead_impl() const {
      return _dead;
    }

    //! Stub
    virtual bool finished_impl() const {
      return _finished;
    }

    //! Stub
    virtual bool started_impl() const {
      return _started;
    }

    ////////////////////////////////////////////////////////////////////////
    // Runner - non-virtual member functions - protected
    ////////////////////////////////////////////////////////////////////////

    //! Stub
    void set_finished(bool val) const noexcept {
      _finished = val;
    }

    //! Stub
    void set_started(bool val) const noexcept {
      _started = val;
    }

    //! Stub
    inline bool stopped_by_predicate() const noexcept {
      return _stopped_by_predicate;
    }

   private:
    ////////////////////////////////////////////////////////////////////////
    // Runner - data - private
    ////////////////////////////////////////////////////////////////////////

    std::atomic<bool>                                      _dead;
    mutable bool                                           _finished;
    mutable std::chrono::high_resolution_clock::time_point _last_report;
    std::chrono::nanoseconds                       _report_time_interval;
    std::chrono::nanoseconds                       _run_for;
    std::chrono::high_resolution_clock::time_point _start_time;
    mutable bool                                   _started;
    mutable bool                                   _stopped_by_predicate;
    detail::FunctionRef<bool(void)>                _stopper;
  };
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_INCLUDE_RUNNER_HPP_
