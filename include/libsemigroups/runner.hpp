//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019-2025 James D. Mitchell
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

#include <algorithm>    // for max
#include <array>        // for array
#include <atomic>       // for atomic
#include <chrono>       // for time_point, nanoseconds
#include <cstddef>      // for size_t
#include <functional>   // for function
#include <memory>       // for unique_ptr
#include <mutex>        // for mutex
#include <string>       // for basic_string, string
#include <string_view>  // for basic_string_view
#include <thread>       // for sleep_for, thread
#include <tuple>        // for apply
#include <utility>      // for move, forward
#include <vector>       // for vector

#include "debug.hpp"  // for LIBSEMIGROUPS_ASSERT

#include "detail/function-ref.hpp"  // for FunctionRef
#include "detail/report.hpp"        // for report_default
#include "detail/string.hpp"        // for unicode_string_length

namespace libsemigroups {

  //! \defgroup core_classes_group Core Classes
  //!
  //! Many of the classes in \c libsemigroups implement algorithms, and hence
  //! are runnable. During the running of these algorithms, it is often
  //! desirable to report the state of the algorithm. Therefore, the classes
  //! \ref Runner and \ref Reporter exist to provide common functions to many
  //! classes that implement the main algorithms.
  //!
  //! This page describes the functionality in \c libsemigroups related to
  //! running and reporting.

  //! \relates Reporter
  //! \brief The time between a given point and now.
  //!
  //! The time between a given point and now.
  //!
  //! \param t the time point.
  //!
  //! \returns The nanoseconds between the time point \p t and now.
  // Not noexcept because std::chrono::duration_cast isn't
  [[nodiscard]] static inline std::chrono::nanoseconds
  delta(std::chrono::high_resolution_clock::time_point const& t) {
    using std::chrono::duration_cast;
    using std::chrono::high_resolution_clock;
    return duration_cast<std::chrono::nanoseconds>(high_resolution_clock::now()
                                                   - t);
  }

  //! \ingroup constants_group
  //! \brief Value indicating forever (system dependent but possibly approx.
  //! 292 years).
  //!
  //! Defined in \c runner.hpp
  //!
  //! A pseudonym for std::chrono::nanoseconds::max().
  constexpr std::chrono::nanoseconds FOREVER = std::chrono::nanoseconds::max();

  //! \ingroup core_classes_group
  //! \brief Collection of values related to reporting.
  //!
  //! Defined in `runner.hpp`.
  //!
  //! This class exists to collect some values related to reporting in its
  //! derived classes. These values are:
  //! * report_prefix();
  //! * report_every();
  //! * last_report();
  //! * start_time().
  class Reporter {
   public:
    //! \brief Alias for std::chrono::high_resolution_clock::time_point
    using time_point = std::chrono::high_resolution_clock::time_point;
    //! \brief Alias for std::chrono::nanoseconds
    using nanoseconds = std::chrono::nanoseconds;

   private:
    std::string _divider;
    std::string _prefix;
    nanoseconds _report_time_interval;

    mutable std::atomic<time_point> _last_report;
    mutable time_point              _start_time;

   public:
    //! \brief Default constructor.
    //!
    //! Default construct a Reporter object such that the following hold:
    //! * report_prefix() is empty;
    //! * report_every() is set to 1 second;
    //! * last_report() is now;
    //! * start_time() is now.
    // not noexcept because std::string constructor isn't
    Reporter();

    //! \brief Initialize an existing Reporter object.
    //!
    //! This function puts a Reporter object back into the same state as if it
    //! had been newly default constructed.
    //!
    //! \returns A reference to \c *this.
    //!
    //! \note This function is not thread-safe.
    //!
    //! \sa Reporter()
    Reporter& init();

    //! \brief Default copy constructor
    Reporter(Reporter const& that);

    //! \brief Default move constructor
    Reporter(Reporter&& that);

    //! \brief Default copy assignment operator
    Reporter& operator=(Reporter const& that);

    //! \brief Default move assignment operator
    Reporter& operator=(Reporter&& that);

    ~Reporter() = default;

    //! \brief Check if it is time to report.
    //!
    //! This function can be used to check if enough time has passed that we
    //! should report again.  That is if the time between last_report() and now
    //! is greater than the value of report_every().
    //!
    //! If \c true is returned, then last_report() is set to now.
    //!
    //! \returns
    //! A value of type \c bool.
    //!
    //! \note This function is thread-safe.
    //!
    //! \warning This function can be somewhat expensive, and so you should
    //! avoid invoking it too often.
    //!
    //! \sa report_every(std::chrono::nanoseconds) and report_every(Time).
    // not noexcept because operator- for time_points can throw.
    // TODO(later) remove? Used by Action
    [[nodiscard]] bool report() const;

    //! \brief Set the minimum elapsed time between reports in nanoseconds.
    //!
    //! This function can be used to specify at run time the minimum elapsed
    //! time between two calls to report() that will return \c true. If
    //! report() returns \c true at time \c s, then report() will
    //! only return \c true again after at least time \c s + \c t has elapsed.
    //!
    //! \param val the amount of time (in nanoseconds) between reports.
    //!
    //! \returns
    //! A reference to \c this.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \note This function is not thread-safe.
    //!
    //! \sa
    //! report_every(Time)
    //!
    //! \deprecated_alias_warning
    [[deprecated]] Reporter& report_every(nanoseconds val) noexcept {
      _last_report          = std::chrono::high_resolution_clock::now();
      _report_time_interval = val;
      return *this;
    }

    //! \brief Set the minimum elapsed time between reports in a unit of time
    //! other than nanoseconds.
    //!
    //! This function converts its argument to std::chrono::nanoseconds and
    //! calls report_every(std::chrono::nanoseconds).
    //!
    //! \tparam Time the type of the argument (should be
    //! std::chrono::something).
    //!
    //! \param t the amount of time (in \c Time) between reports.
    //!
    //! \returns
    //! A reference to \c this.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \note This function is not thread-safe.
    //!
    //! \deprecated_alias_warning
    template <typename Time>
    [[deprecated]] Reporter& report_every(Time t) noexcept {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
      return report_every(nanoseconds(t));
#pragma GCC diagnostic pop
    }

    //! \brief Get the minimum elapsed time between reports.
    //!
    //! \returns
    //! The number of nanoseconds between reports.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \note This function is thread-safe.
    //!
    //! \deprecated_alias_warning
    [[deprecated]] [[nodiscard]] nanoseconds report_every() const noexcept {
      return _report_time_interval;
    }

    //! \brief Get the start time.
    //!
    //!  This is the time point at which reset_start_time() was last called,
    //!  which is also the time of construction of a Reporter instance if
    //!  reset_start_time() is not explicitly called.
    //!
    //! \returns The time point representing the start time.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \note This function is thread-safe.
    [[nodiscard]] time_point start_time() const noexcept {
      return _start_time;
    }

    //! \brief Reset the start time (and last report) to now.
    //!
    //! \returns A const reference to \c this.
    //!
    //! \note This function is not thread-safe.
    // Not sure if this is noexcept or not,
    // std::chrono::high_resolution_clock::now is noexcept, but JDM couldn't
    // find the noexcept spec of the copy assignment operator of a time_point.
    Reporter const& reset_start_time() const {
      _last_report = std::chrono::high_resolution_clock::now();
      _start_time  = _last_report;
      return *this;
    }

    //! \brief Get the time point of the last report.
    //!
    //! Returns the time point of the last report, as set by one of:
    //! * reset_start_time();
    //! * report_every(); or
    //! * report().
    //!
    //! \returns A \ref time_point.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \note This function is thread-safe.
    [[nodiscard]] time_point last_report() const noexcept {
      return _last_report;
    }

    //! \brief Set the last report time point to now.
    //!
    //! Returns the time point of the last report, as set by one of:
    //! * reset_start_time();
    //! * report_every(); or
    //! * report().
    //!
    //! \returns A const reference to \c this.
    //!
    //! \note This function is thread-safe.
    // Not sure if this is noexcept or not,
    // std::chrono::high_resolution_clock::now is noexcept, but JDM couldn't
    // find the noexcept spec of the copy assignment operator of a time_point.
    Reporter const& reset_last_report() const {
      _last_report = std::chrono::high_resolution_clock::now();
      return *this;
    }

    //! \brief Set the prefix string for reporting.
    //!
    //! This function sets the return value of report_prefix() to (a copy of)
    //! the argument \p val. Typically this prefix should be the name of the
    //! algorithm being run at the outmost level.
    //!
    //! \param val the new value of the report prefix.
    //!
    //! \returns A reference to \c this.
    //!
    //! \note This function is not thread-safe.
    // Not noexcept because std::string::operator= isn't
    Reporter& report_prefix(std::string const& val) {
      _prefix = val;
      return *this;
    }

    //! \brief Get the current prefix string for reporting.
    //!
    //! This function gets the current value of the prefix string for reporting
    //! (set via report_prefix(std::string const&)), which is typically
    //! the name of the algorithm being run at the outmost level.
    //!
    //! \returns A const reference to the prefix string.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \note This function is thread-safe.
    [[nodiscard]] std::string const& report_prefix() const noexcept {
      return _prefix;
    }

    //! \brief Set the divider string for reporting.
    //!
    //! This function sets the return value of report_divider() to (a copy of)
    //! the argument \p val.
    //!
    //! \param val the new value of the report divider.
    //!
    //! \returns A reference to \c this.
    //!
    //! \note This function is not thread-safe.
    // Not noexcept because std::string::operator= isn't
    // TODO(1) deprecate
    Reporter& report_divider(std::string const& val) {
      _divider = val;
      return *this;
    }

    //! \brief Get the current divider string for reporting.
    //!
    //! This function gets the current value of the divider string for reporting
    //! (set via report_divider(std::string const&)).
    //!
    //! \returns A const reference to the divider string.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \note This function is thread-safe.
    // TODO(1) deprecate
    [[nodiscard]] std::string const& report_divider() const noexcept {
      return _divider;
    }

    //! \brief Emits the current divider string for reporting.
    //!
    //! This function emits the current divider string for reporting
    //! (set via report_divider(std::string const&), and retrieved via
    //! report_divider()).
    //!
    //! \note This function is thread-safe.
    void emit_divider() {
      if (!_divider.empty()) {
        report_no_prefix(_divider, "");
      }
    }
  };

  //! \ingroup core_classes_group
  //!
  //! \brief Abstract class for derived classes that run an algorithm.
  //!
  //! Defined in \c runner.hpp
  //!
  //! Many of the classes in `libsemigroups` implementing the algorithms
  //! that are the reason for the existence of this library, are derived from
  //! Runner.  The Runner class exists to collect various common tasks
  //! required by such a derived class with a possibly long running \ref run.
  //! These common tasks include:
  //! * running for a given amount of time (\ref run_for)
  //! * running until a nullary predicate is true (\ref run_until)
  //! * checking the status of the algorithm: has it
  //! \ref started?  \ref finished? been killed by another thread
  //! (\ref dead)? has it timed out (\ref timed_out)? has it
  //! \ref stopped for any reason?
  //! * permit the function \ref run to be killed from another thread
  //! (\ref kill).
  class Runner : public Reporter {
   public:
    //! Enum class for the state of the Runner.
    enum class state {
      //! Indicates that none of \ref run, \ref run_for, or \ref run_until has
      //! been called since construction or the last call to \ref init.
      never_run = 0,
      //! Indicates that the Runner is currently running to the finish (via
      //! \ref run).
      running_to_finish = 1,
      //! Indicates that the Runner is currently running for a specific amount
      //! of time (via \ref run_for).
      running_for = 2,
      //! Indicates that the Runner is currently running until some condition
      //! is met (via \ref run_until).
      running_until = 3,
      //! Indicates that the Runner was run via \ref run_for for a specific
      //! amount of time and that time has elapsed.
      timed_out = 4,
      //! Indicates that the Runner was run via \ref run_until until the
      //! condition specified by the argument to \ref run_until was met.
      stopped_by_predicate = 6,
      //! Indicates that the Runner is not in any of the previous states and is
      //! not currently running. This can occur when, for example, \ref run
      //! throws an exception.
      not_running = 7,
      //! Indicates that the Runner was killed (by another thread).
      dead = 8
    };

   private:
    ////////////////////////////////////////////////////////////////////////
    // Runner - data - private
    ////////////////////////////////////////////////////////////////////////

    std::chrono::nanoseconds        _run_for;
    mutable std::atomic<state>      _state;
    detail::FunctionRef<bool(void)> _stopper;

   public:
    ////////////////////////////////////////////////////////////////////////
    // Runner - constructors + destructor - public
    ////////////////////////////////////////////////////////////////////////

    //! \brief Default constructor.
    //!
    //! Returns a runner that is not started, not finished, not dead, not
    //! timed out, will run \ref FOREVER if not instructed otherwise,  that
    //! last reported at the time of construction.
    //!
    // not noexcept because Reporter() isn't
    Runner();

    //! \brief Initialize an existing Runner object.
    //!
    //! This function puts a Runner object back into the same state as if it
    //! had been newly default constructed.
    //!
    //! \returns A reference to \c *this.
    //!
    //! \note This function is not thread-safe.
    //!
    //! \sa Runner()
    Runner& init();

    //! \brief Copy constructor.
    //!
    //! Returns a runner that is a copy of \p other. The state of the new
    //! runner is the same as \p other, except that the function passed as an
    //! argument to \ref run_until (if any) is not copied.
    //!
    //! \param other the Runner to copy.
    Runner(Runner const& other);

    //! \brief Move constructor.
    //!
    //! Returns a runner that is initialised from \p other. The state of the
    //! new runner is the same as \p copy, except that the function passed as
    //! an argument to \ref run_until (if any) is not copied.
    //!
    //! \param other the Runner to move from.
    Runner(Runner&& other);

    //! \brief Copy assignment operator.
    //!
    //! Returns a runner that is initialised from \p other. The state of the
    //! new runner is the same as \p copy, except that the function passed as
    //! an argument to \ref run_until (if any) is not copied.
    //!
    //! \param other the Runner to move from.
    Runner& operator=(Runner const& other);

    //! \brief Move assignment operator.
    //!
    //! Returns a runner that is initialised from \p other. The state of the
    //! new runner is the same as \p copy, except that the function passed as
    //! an argument to \ref run_until (if any) is not copied.
    //!
    //! \param other the Runner to move from.
    Runner& operator=(Runner&& other);

    virtual ~Runner();

    ////////////////////////////////////////////////////////////////////////
    // Runner - non-virtual member functions - public
    ////////////////////////////////////////////////////////////////////////

    // The following mem fns don't return a reference to Runner to avoid the
    // unexpected behaviour of e.g. Sims1.run_for() returning a reference to not
    // a Sims1.

    //! \brief Run until \ref finished.
    //!
    //! Run the main algorithm implemented by a derived class of
    //! Runner.
    // At the end of this either finished, or dead.
    void run();

    //! \brief Run for a specified amount of time.
    //!
    //! For this to work it is necessary to periodically check if
    //! timed_out() returns \c true, and to stop if it is, in the
    //! run() member function of any derived class of Runner.
    //!
    //! \param t the time in nanoseconds to run for.
    //!
    //! \sa run_for(Time)
    // At the end of this either finished, dead, or timed_out.
    void run_for(std::chrono::nanoseconds t);

    //! \brief Run for a specified amount of time.
    //!
    //! For this to work it is necessary to periodically check if
    //! timed_out() returns \c true, and to stop if it is, in the
    //! run() member function of any derived class of Runner.
    //!
    //! \tparam Time unit of time.
    //!
    //! \param t the time to run for (in \c Time).
    //!
    //! \sa run_for(std::chrono::nanoseconds)
    template <typename Time>
    void run_for(Time t) {
      run_for(std::chrono::nanoseconds(t));
    }

    //! \brief Check if the amount of time passed to \ref run_for has elapsed.
    //!
    //! \returns
    //! A \c bool
    //!
    //! \sa run_for(std::chrono::nanoseconds) and
    //! run_for(Time).
    // not noexcept because operator-(time_point, time_point) isn't
    [[nodiscard]] inline bool timed_out() const {
      return (running_for() ? delta(start_time()) >= _run_for
                            : current_state() == state::timed_out);
    }

    //! \brief Run until a nullary predicate returns \p true or \ref finished.
    //!
    //! \tparam Func the type of the argument.
    //! \param func a callable type that will exist for at
    //! least until this function returns, or a function pointer.
    // At the end of this either finished, dead, or stopped_by_predicate.
    template <typename Func>
    void run_until(Func&& func);

    //! \brief Run until a nullary predicate returns \p true or \ref finished.
    //!
    //! \param func a function pointer.
    void run_until(bool (*func)()) {
      run_until(detail::FunctionRef<bool(void)>(func));
    }

    //! \brief Report why \ref run stopped.
    //!
    //! Reports whether run() was stopped because it is finished(),
    //! timed_out(), or dead().
    //!
    // not noexcept because it calls timed_out which is not noexcept
    void report_why_we_stopped() const;

    // TODO(doc)
    std::string string_why_we_stopped() const;

    //! \brief Check if \ref run has been run to completion or not.
    //!
    //! Returns \c true if run() has been run to completion. For this to
    //! work, the implementation of run() in a derived class of
    //! Runner must implement a specialisation of
    //! \c finished_impl.
    //!
    //! \returns
    //! A \c bool.
    //!
    //! \sa started()
    // Not noexcept because finished_impl isn't
    [[nodiscard]] bool finished() const;

    //! \brief Check if run has been run to completion successfully.
    //!
    //! Returns \c true if \ref run has been run to completion and it was
    //! successful. The default implementation is to just call \ref finished.
    //!
    //! \returns A \c bool.
    [[nodiscard]] virtual bool success() const {
      return finished();
    }

    //! \brief  Check if \ref run has been called at least once before.
    //!
    //! Returns \c true if run() has started to run (it can be running or
    //! not).
    //!
    //! \returns
    //! A \c bool.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \sa finished()
    [[nodiscard]] bool started() const noexcept {
      return current_state() != state::never_run;
    }

    //! \brief Check if currently running.
    //!
    //! Returns \c true if run() is in the process of running and \c false it is
    //! not.
    //!
    //! \returns
    //! A \c bool.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \sa finished()
    [[nodiscard]] bool running() const noexcept {
      return current_state() == state::running_to_finish
             || current_state() == state::running_for
             || current_state() == state::running_until;
    }

    //! \brief Stop \ref run from running (thread-safe).
    //!
    //! This function can be used to terminate run() from another thread.
    //! After kill() has been called the Runner may no longer be in a valid
    //! state, but will return \c true from dead() .
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \sa finished()
    void kill() noexcept {
      set_state(state::dead);
    }

    //! \brief Check if the runner is dead.
    //!
    //! This function can be used to check if we should terminate run()
    //! because it has been killed by another thread.
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

    //! \brief Check if the runner is stopped.
    //!
    //! This function can be used to check whether or not run() has been
    //! stopped for whatever reason. In other words, it checks if
    //! timed_out(), finished(), or dead().
    //!
    //! \returns
    //! A \c bool.
    //!
    // not noexcept because timed_out isn't
    [[nodiscard]] bool stopped() const {
      return (running() ? (timed_out() || stopped_by_predicate())
                        : current_state() > state::running_until);
    }

    //! \brief Check if the runner was stopped, or should stop, because of
    //! the argument last passed to \ref run_until.
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
    // noexcept should depend on whether _stopper can throw or not
    [[nodiscard]] inline bool stopped_by_predicate() const {
      if (running_until()) {
        LIBSEMIGROUPS_ASSERT(_stopper.valid());
        return _stopper();
      } else {
        return current_state() == state::stopped_by_predicate;
      }
    }

    //! \brief Check if the runner is currently running for a particular length
    //! of time.
    //!
    //! If the Runner is currently running because its member function
    //! \ref run_for has been invoked, then this function returns \c true.
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
    [[nodiscard]] bool running_for() const noexcept {
      return _state == state::running_for;
    }

    //! \brief Return the last value passed to \ref Runner::run_for.
    //!
    //! This function returns the last value passed to as an argument to
    //! \ref Runner::run_for (if any), and \ref FOREVER otherwise.
    //!
    //! \returns
    //! The \ref Runner::run_for time in nanoseconds.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    [[nodiscard]] std::chrono::nanoseconds
    running_for_how_long() const noexcept {
      return _run_for;
    }

    //! \brief Check if the runner is currently running until a nullary
    //! predicate returns \c true.
    //!
    //! If the Runner is currently running because its member function
    //! \ref run_until has been invoked, then this function returns \c true.
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
    [[nodiscard]] bool running_until() const noexcept {
      return _state == state::running_until;
    }

    //! \brief Return the current state.
    //!
    //! Returns the current state of the Runner as given by \ref state.
    //!
    //! \returns
    //! A value of type \c state.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
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
  };  // class Runner
}  // namespace libsemigroups

#include "runner.tpp"
#endif  // LIBSEMIGROUPS_RUNNER_HPP_
