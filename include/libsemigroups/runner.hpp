//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019-2023 James D. Mitchell
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
#include <thread>       // for ??
#include <type_traits>  // for forward

#include "debug.hpp"      // for LIBSEMIGROUPS_ASSERT
#include "exception.hpp"  // for LibsemigroupsException

#include "detail/function-ref.hpp"  // for FunctionRef
#include "detail/report.hpp"        // for LibsemigroupsException
#include "detail/stl.hpp"           // for LibsemigroupsException

namespace libsemigroups {
  //! A pseudonym for std::chrono::nanoseconds::max().
  constexpr std::chrono::nanoseconds FOREVER = std::chrono::nanoseconds::max();

  // TODO:
  // * rename to Reporter (when the v2 one is removed)
  // * use this anywhere?
  // This class exists so that the "reporting" functionality can be used
  // independently from the "runner" functionality, for example in
  // NodeManagedDigraph, Sims1, and so on
  class ReporterV3 {
    using time_point  = std::chrono::high_resolution_clock::time_point;
    using nanoseconds = std::chrono::nanoseconds;

    std::string _prefix;
    nanoseconds _report_time_interval;

    mutable time_point _last_report;
    mutable time_point _start_time;
    mutable bool       _stop_reporting;
    // TODO atomic so that we can read in thread_func and write elsewhere
    // safely

   public:
    // not noexcept because std::string constructor isn't
    ReporterV3()
        : _prefix(),
          _report_time_interval(),
          // mutable
          _last_report(),  // TODO required?
          _start_time(),
          _stop_reporting() {
      // All values set in init
      init();
    }

    ReporterV3& init();

    ReporterV3(ReporterV3 const&)            = default;
    ReporterV3(ReporterV3&&)                 = default;
    ReporterV3& operator=(ReporterV3 const&) = default;
    ReporterV3& operator=(ReporterV3&&)      = default;

    ~ReporterV3() = default;

    virtual void report_progress_from_thread() const {}

    // It's necessary to check if reporting is enable before calling this
    // function
    // TODO improve this so there's less waiting in the case _stop_reporting is
    // set to true (not sure how to do that)
    std::thread launch_report_thread() const {
      _stop_reporting  = false;
      _start_time      = std::chrono::high_resolution_clock::now();
      auto thread_func = [this]() {
        std::this_thread::sleep_for(report_every());
        while (!_stop_reporting) {
          report_progress_from_thread();
          std::this_thread::sleep_for(report_every());
          // If _stop_reporting is changed when we are sleeping, then this
          // thread won't terminate until it wakes up, hence when we join the
          // thread, every computation calling this function will take a
          // minimum of report_every()=1s to run, which is not great. We can't
          // detach the thread below because then *this can be deleted while
          // this thread is asleep meaning that _stop_reporting is lost.
        }
      };
      return std::thread(thread_func);
    }

    ReporterV3 const& stop_report_thread() const {
      _stop_reporting = true;
      return *this;
    }

    //! Check if it is time to report.
    //!
    //! This function can be used in an implementation of run() (in a
    //! derived class of Runner) to check if enough time has
    //! passed that we should report again.
    //!
    //! \returns
    //! A \c bool.
    //!
    //! \par Parameters
    //! (None)
    //!
    //! \sa report_every(std::chrono::nanoseconds) and report_every(TIntType).
    // not noexcept because operator- for time_points can throw.
    // TODO remove?
    [[nodiscard]] inline bool report() const {
      auto t       = std::chrono::high_resolution_clock::now();
      auto elapsed = t - _last_report;

      if (elapsed > _report_time_interval) {
        _last_report = t;
        return true;
      } else {
        return false;
      }
    }

    //! Set the minimum elapsed time between reports.
    //!
    //! This function can be used to specify at run time the minimum elapsed
    //! time between two calls to report() that return \c true. If
    //! report() returns \c true at time \c s, then report() will
    //! only return \c true again after time \c s + \c t has elapsed.
    //!
    //! \param val the amount of time (in nanoseconds) between reports.
    //!
    //! \returns
    //! A reference to this.
    //!
    //! \sa
    //! report_every(TIntType)
    ReporterV3& report_every(nanoseconds val) noexcept {
      _last_report          = std::chrono::high_resolution_clock::now();
      _report_time_interval = val;
      return *this;
    }

    //! Set the minimum elapsed time between reports.
    //!
    //! This function can be used to specify at run time the minimum elapsed
    //! time between two calls to report() that return \c true. If
    //! report() returns \c true at time \c s, then report() will
    //! only return \c true again after time \c s + \c t has elapsed.
    //!
    //! \param t the amount of time (in \c TIntType) between reports.
    //!
    //! \returns
    //! (None)
    //!
    //! \sa report_every(std::chrono::nanoseconds)
    template <typename TIntType>
    ReporterV3& report_every(TIntType t) noexcept {
      return report_every(nanoseconds(t));
    }

    //! Get the minimum elapsed time between reports.
    //!
    //! \par Parameters
    //! (None)
    //!
    //! \returns
    //! The number of nanoseconds between reports.
    [[nodiscard]] nanoseconds report_every() const noexcept {
      return _report_time_interval;
    }

    [[nodiscard]] time_point start_time() const noexcept {
      return _start_time;
    }

    [[nodiscard]] time_point last_report() const noexcept {
      return _last_report;
    }

    // TODO noexcept
    ReporterV3 const& last_report(time_point val) const {
      _last_report = val;
      return *this;
    }

    // Not noexcept because std::string::operator= isn't
    ReporterV3& report_prefix(std::string const& val) {
      _prefix = val;
      return *this;
    }

    [[nodiscard]] std::string const& report_prefix() const noexcept {
      return _prefix;
    }
  };

  namespace detail {
    class ReportThreadGuard : public ThreadGuard {
      ReporterV3 const& _reporter;

     public:
      explicit ReportThreadGuard(ReporterV3 const& reporter,
                                 std::thread&      thread)
          : ThreadGuard(thread), _reporter(reporter) {}

      ~ReportThreadGuard() {
        _reporter.stop_report_thread();
      }

      ReportThreadGuard(ReportThreadGuard const&)            = delete;
      ReportThreadGuard& operator=(ReportThreadGuard const&) = delete;
    };
  }  // namespace detail

  //! Many of the classes in ``libsemigroups`` implementing the algorithms,
  //! that are the reason for the existence of this library, are derived from
  //! Runner.  The Runner class exists to collect various common tasks required
  //! by such a derived class with a possibly long running \ref run.
  //! These common tasks include:
  //! * running for a given amount of time (\ref run_for)
  //! * running until a nullary predicate is true (\ref run_until)
  //! * reporting after a given amount of time (\ref report_every)
  //! * checking if the given amount of time has elapsed since last report
  //! (\ref report)
  //! * checking the status of the algorithm: has it
  //! \ref started?  \ref finished? been killed by another thread
  //! (\ref dead)? has it timed out (\ref timed_out)? has it
  //! \ref stopped for any reason?
  //! * permit the function \ref run to be killed from another thread
  //! (\ref kill).
  class Runner : public ReporterV3 {
   public:
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

   private:
    ////////////////////////////////////////////////////////////////////////
    // Runner - data - private
    ////////////////////////////////////////////////////////////////////////

    std::chrono::nanoseconds                       _run_for;
    std::chrono::high_resolution_clock::time_point _start_time;  // TODO remove
                                                                 // (it's in
                                                                 // ReporterV3
    mutable std::atomic<state>      _state;
    detail::FunctionRef<bool(void)> _stopper;

   public:
    ////////////////////////////////////////////////////////////////////////
    // Runner - constructors + destructor - public
    ////////////////////////////////////////////////////////////////////////

    //! Default constructor.
    //!
    //! Returns a runner that is not started, not finished, not dead, not timed
    //! out, will run \ref FOREVER if not instructed otherwise,  that
    //! last reported at the time of construction, and that will report every
    //! std::chrono::seconds(1) if reporting is enabled.
    //!
    //! \par Parameters
    //! (None)
    // not noexcept because ReporterV3() isn't
    Runner();

    // TODO doc
    Runner& init();

    //! Copy constructor.
    //!
    //! Returns a runner that is a copy of \p other. The state of the new runner
    //! is the same as \p other, except that the function passed as an argument
    //! to \ref run_until (if any) is not copied.
    //!
    //! \param other the Runner to copy.
    Runner(Runner const& other);

    //! Move constructor
    //!
    //! Returns a runner that is initialised from \p other. The state of the
    //! new runner is the same as \p copy, except that the function passed as
    //! an argument to \ref run_until (if any) is not copied.
    //!
    //! \param other the Runner to move from.
    Runner(Runner&& other);

    //! Copy assignment operator.
    //!
    //! Returns a runner that is initialised from \p other. The state of the
    //! new runner is the same as \p copy, except that the function passed as
    //! an argument to \ref run_until (if any) is not copied.
    //!
    //! \param other the Runner to move from.
    Runner& operator=(Runner const& other);

    //! Move assignment operator.
    //!
    //! Returns a runner that is initialised from \p other. The state of the
    //! new runner is the same as \p copy, except that the function passed as
    //! an argument to \ref run_until (if any) is not copied.
    //!
    //! \param other the Runner to move from.
    Runner& operator=(Runner&& other);

    virtual ~Runner() = default;

    ////////////////////////////////////////////////////////////////////////
    // Runner - non-virtual member functions - public
    ////////////////////////////////////////////////////////////////////////

    //! Run until \ref finished.
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
    void run();

    //! Run for a specified amount of time.
    //!
    //! For this to work it is necessary to periodically check if
    //! timed_out() returns \c true, and to stop if it is, in the
    //! run() member function of any derived class of Runner.
    //!
    //! \param t the time in nanoseconds to run for.
    //!
    //! \returns
    //! (None)
    //!
    //! \sa run_for(TIntType)
    // At the end of this either finished, dead, or timed_out.
    void run_for(std::chrono::nanoseconds t);

    //! Run for a specified amount of time.
    //!
    //!
    //! For this to work it is necessary to periodically check if
    //! timed_out() returns \c true, and to stop if it is, in the
    //! run() member function of any derived class of Runner.
    //!
    //! \param t the time to run for (in \c TIntType).
    //!
    //! \returns
    //! (None)
    //!
    //! \sa run_for(std::chrono::nanoseconds)
    template <typename TIntType>
    void run_for(TIntType t) {
      run_for(std::chrono::nanoseconds(t));
    }

    //! Check if the amount of time passed to \ref run_for has elapsed.
    //!
    //! \par Parameters
    //! (None)
    //!
    //! \returns
    //! A \c bool
    //!
    //! \sa run_for(std::chrono::nanoseconds) and
    //! run_for(TIntType).
    // not noexcept because operator-(time_point, time_point) isn't
    [[nodiscard]] inline bool timed_out() const {
      return (running_for()
                  ? std::chrono::high_resolution_clock::now() - _start_time
                        >= _run_for
                  : current_state() == state::timed_out);
    }

    //! Run until a nullary predicate returns \p true or \ref finished.
    //!
    //! \param func a callable type that will exist for at
    //! least until this function returns, or a function pointer.
    //!
    //! \returns
    //! (None)
    // At the end of this either finished, dead, or stopped_by_predicate.
    template <typename T>
    void run_until(T&& func);

    //! Run until a nullary predicate returns \p true or \ref finished.
    //!
    //! \param func a function pointer.
    //!
    //! \returns
    //! (None)
    void run_until(bool (*func)()) {
      run_until(detail::FunctionRef<bool(void)>(func));
    }

    //! Report why \ref run stopped.
    //!
    //! Reports whether run() was stopped because it is finished(),
    //! timed_out(), or dead().
    //!
    //! \par Parameters
    //! (None)
    //!
    //! \returns
    //! (None)
    // not noexcept because it calls timed_out which is not noexcept
    void report_why_we_stopped() const;

    //! Check if \ref run has been run to completion or not.
    //!
    //! Returns \c true if run() has been run to completion. For this to
    //! work, the implementation of run() in a derived class of
    //! Runner must implement a specialisation of
    //! \c finished_impl.
    //!
    //! \par Parameters
    //! (None)
    //!
    //! \returns
    //! A \c bool.
    //!
    //! \sa started()
    // Not noexcept because finished_impl isn't
    [[nodiscard]] inline bool finished() const {
      if (started() && !dead() && finished_impl()) {
        _state = state::not_running;
        return true;
      } else {
        return false;
      }
      // since kill() may leave the object in an invalid state we only return
      // true here if we are not dead and the object thinks it is finished.
    }

    //! Check if \ref run has been called at least once before.
    //!
    //! Returns \c true if run() has started to run (it can be running or
    //! not).
    //!
    //! \par Parameters
    //! (None)
    //!
    //! \returns
    //! A \c bool.
    //!
    //! \sa finished()
    [[nodiscard]] bool started() const noexcept {
      return current_state() != state::never_run;
    }

    //! Check if currently running.
    //!
    //! \returns
    //! \c true if run() is in the process of running and \c false it is not.
    //!
    //! \par Parameters
    //! (None)
    //!
    //! \returns
    //! A \c bool.
    //!
    //! \sa finished()
    [[nodiscard]] bool running() const noexcept {
      return current_state() == state::running_to_finish
             || current_state() == state::running_for
             || current_state() == state::running_until;
    }

    //! Stop \ref run from running (thread-safe).
    //!
    //! This function can be used to terminate run() from another thread.
    //! After kill() has been called the Runner may no longer be in a valid
    //! state, but will return \c true from dead() .
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
    //! \sa finished()
    void kill() noexcept {
      set_state(state::dead);
    }

    //! Check if the runner is dead.
    //!
    //! This function can be used to check if we should terminate run()
    //! because it has been killed by another thread.
    //!
    //! \par Parameters
    //! (None)
    //!
    //! \returns
    //! A \c bool.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \sa kill()
    [[nodiscard]] bool dead() const noexcept {
      return current_state() == state::dead;
    }

    //! Check if the runner is stopped.
    //!
    //! This function can be used to check whether or not run() has been
    //! stopped for whatever reason. In other words, it checks if
    //! timed_out(), finished(), or dead().
    //!
    //! \returns
    //! A \c bool.
    //!
    //! \par Parameters
    //! (None)
    // not noexcept because timed_out isn't
    [[nodiscard]] bool stopped() const {
      return (running() ? (timed_out() || stopped_by_predicate())
                        : current_state() > state::running_until);
    }

    //! Check if the runner was, or should, stop because of
    //! the argument for \ref run_until.
    //!
    //! If \c this is running, then the nullary predicate is called and its
    //! return value is returned. If \c this is not running, then \c true is
    //! returned if and only if the last time \c this was running it was
    //! stopped by a call to the nullary predicate passed to run_until().
    //!
    //! \returns
    //! A \c bool.
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
    [[nodiscard]] inline bool stopped_by_predicate() const {
      if (running_until()) {
        LIBSEMIGROUPS_ASSERT(_stopper.valid());
        return _stopper();
      } else {
        return current_state() == state::stopped_by_predicate;
      }
    }

    //! Check if the runner is currently running for a particular length of
    //! time.
    //!
    //! If the Runner is currently running because its member function \ref
    //! run_for has been invoked, then this function returns \c true.
    //! Otherwise, \c false is returned.
    //!
    //! \returns
    //! A \c bool.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    //!
    //! \par Parameters
    //! (None)
    [[nodiscard]] bool running_for() const noexcept {
      return _state == state::running_for;
    }

    //! Check if the runner is currently running until a nullary predicate
    //! returns \c true.
    //!
    //! If the Runner is currently running because its member function \ref
    //! run_until has been invoked, then this function returns \c true.
    //! Otherwise, \c false is returned.
    //!
    //! \returns
    //! A \c bool.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    //!
    //! \par Parameters
    //! (None)
    [[nodiscard]] bool running_until() const noexcept {
      return _state == state::running_until;
    }

    // TODO doc
    [[nodiscard]] state current_state() const noexcept {
      return _state;
    }

   private:
    virtual void               run_impl()            = 0;
    [[nodiscard]] virtual bool finished_impl() const = 0;

    void set_state(state stt) const noexcept {
      // We can set the state back to never_run if run_impl throws, and we are
      // restoring the old state.
      if (!dead()) {
        // It can be that *this* becomes dead after this function has been
        // called.
        _state = stt;
      }
    }
  };

  template <typename T>
  void Runner::run_until(T&& func) {
    if (!finished() && !dead()) {
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
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_RUNNER_HPP_
