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

// This file contains the declaration of the Race class template for
// competitively running different functions/methods in different threads, and
// obtaining the winner.

// TODO:
// 1. consider if keeping killed off methods has any uses

#ifndef LIBSEMIGROUPS_INCLUDE_RACE_HPP_
#define LIBSEMIGROUPS_INCLUDE_RACE_HPP_

#include <thread>
#include <vector>

#include "report.hpp"
#include "runner.hpp"
#include "stl.hpp"
#include "timer.hpp"

namespace libsemigroups {
  namespace internal {
    class Race {
     public:
      //! Construct an empty Race object, with maximum number of threads set to
      //! std::thread::hardware_concurrency.
      Race();
      ~Race();

      //! Set the maximum number of threads, throws if try to set to 0.
      void set_max_threads(size_t);

      //! Runs the method Runner::run on every Runner in the Race, and returns
      //! the one that finishes first. The losers are deleted.
      Runner* winner();

      //! Adds a Runner to the race, throws if the race is already over.
      void add_runner(Runner*);

      //! Returns an iterator pointing to the first Runner in the Race.
      typename std::vector<Runner*>::const_iterator begin() const;

      //! Returns an iterator pointing to one past the last Runner in the Race.
      typename std::vector<Runner*>::const_iterator end() const;

      //! Returns \c true if there are no Runners in the race, and \c false
      //! otherwise.
      bool empty() const;

      //! Runs the race to completion.
      void run();

      //! Runs the race for the specified amount of time.
      void run_for(std::chrono::nanoseconds);

      //! Runs until \p func returns \c true, or the race is over. This
      //! repeatedly calls Race::run_for for \p check_interval, and then checks
      //! whether or not \p func() returns true. The object \p func can be any
      //! callable object with 0 parameters and that returns a bool.
      // This is definitely tested but doesn't show up in the code coverage for
      // some reason.
      template <typename TCallable>
      void run_until(TCallable const&         func,
                     std::chrono::nanoseconds check_interval
                     = std::chrono::milliseconds(2)) {
        static_assert(internal::is_callable<TCallable>::value,
                      "the template parameter TCallable must be callable");
        static_assert(std::is_same<typename std::result_of<TCallable()>::type,
                                   bool>::value,
                      "the template parameter TCallable must return a bool");
        if (empty()) {
          throw LIBSEMIGROUPS_EXCEPTION("no runners given, cannot run_until");
        }
        while (!func() && _winner == nullptr) {
          // if _winner != nullptr, then the race is over.
          run_for(check_interval);
          if (check_interval < std::chrono::milliseconds(1024)) {
            check_interval *= 2;
          }
        }
      }

     private:
      // Runs the callable object \p func on every Runner in parallel.
      template <typename TCallable>
      void run_func(TCallable const& func) {
        static_assert(
            std::is_same<typename std::result_of<TCallable(Runner*)>::type,
                         void>::value,
            "the template parameter TCallable must be void");
        LIBSEMIGROUPS_ASSERT(!empty());
        if (_winner == nullptr) {
          size_t nr_threads = std::min(_runners.size(), _max_threads);
          if (nr_threads == 1) {
            REPORT("using 0 additional threads");
            internal::Timer tmr;
            func(_runners.at(0));
            _winner = _runners.at(0);
            REPORT("elapsed time = ", tmr);
            return;
          }
          for (size_t i = 0; i < _runners.size(); ++i) {
            if (_runners[i]->finished()) {
              REPORT("using 0 additional threads");
              _winner = _runners[i];
              REPORT("#", i, " is already finished!");
              return;
            }
          }

          std::vector<std::thread::id> tids(_runners.size(),
                                            std::this_thread::get_id());

          REPORT("using ",
                 nr_threads,
                 " / ",
                 std::thread::hardware_concurrency(),
                 " additional threads");
          internal::Timer tmr;
          LIBSEMIGROUPS_ASSERT(nr_threads != 0);

          auto thread_func = [this, &func, &tids](size_t pos) {
            tids[pos] = std::this_thread::get_id();
            try {
              func(_runners.at(pos));
            } catch (std::exception const& e) {
              size_t tid = REPORTER.thread_id(tids[pos]);
              REPORT("exception thrown by #", tid, ":");
              REPORT(e.what());
              return;
            }
            // Stop two Runner* objects from killing each other
            {
              std::lock_guard<std::mutex> lg(_mtx);
              if (_runners.at(pos)->finished()) {
                for (auto it = _runners.begin(); it < _runners.begin() + pos;
                     it++) {
                  (*it)->kill();
                }
                for (auto it = _runners.begin() + pos + 1; it < _runners.end();
                     it++) {
                  (*it)->kill();
                }
              }
            }
          };

          REPORTER.reset_thread_ids();

          std::vector<std::thread> t;
          for (size_t i = 0; i < nr_threads; ++i) {
            t.push_back(std::thread(thread_func, i));
          }
          for (size_t i = 0; i < nr_threads; ++i) {
            t.at(i).join();
          }
          REPORT("elapsed time = ", tmr);
          for (auto method = _runners.begin(); method < _runners.end();
               ++method) {
            if ((*method)->finished()) {
              LIBSEMIGROUPS_ASSERT(_winner == nullptr);
              _winner = *method;
              size_t tid
                  = REPORTER.thread_id(tids.at(method - _runners.begin()));
              REPORT("#", tid, " is the winner!");
              break;
            }
          }
          if (_winner != nullptr) {
            for (auto rnnr : _runners) {
              if (rnnr != _winner) {
                delete rnnr;
              }
            }
            _runners.clear();
            _runners.push_back(_winner);
          }
        }
      }

      std::vector<Runner*> _runners;
      size_t               _max_threads;
      std::mutex           _mtx;
      Runner*              _winner;
    };
  }  // namespace internal
}  // namespace libsemigroups

#endif  // LIBSEMIGROUPS_INCLUDE_RACE_HPP_
