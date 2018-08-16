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
// - consider keeping killed off methods.

#ifndef LIBSEMIGROUPS_INCLUDE_INTERNAL_RACE_H_
#define LIBSEMIGROUPS_INCLUDE_INTERNAL_RACE_H_

#include <thread>
#include <vector>

#include "report.h"
#include "runner.h"

namespace libsemigroups {
  class Race {
   public:
    explicit Race(size_t max_threads = std::thread::hardware_concurrency());

    void    set_max_threads(size_t val);
    Runner* winner();
    void    add_runner(Runner* r);

    typename std::vector<Runner*>::const_iterator begin() const;
    typename std::vector<Runner*>::const_iterator end() const;

    bool empty() const;

    void run() {
      run_func(std::mem_fn(&Runner::run));
    }

    void run_for(std::chrono::nanoseconds x) {
      run_func([&x](Runner* rnnr) -> void { rnnr->run_for(x); });
    }

    template <typename TFunction>
    void run_until(TFunction const&         func,
                   std::chrono::nanoseconds check_interval
                   = std::chrono::milliseconds(50)) {
      // TODO some checks that there are any runners alive, and not finished
      // TODO check signature of TFunction
      while (!func()) {
        run_for(check_interval);
      }
    }

   private:
    template <typename TFunction> void run_func(TFunction const& func) {
      // TODO check signature of TFunction
      if (_winner == nullptr) {
        size_t nr_threads = std::min(_runners.size(), _max_threads);
        if (nr_threads == 1
            || std::any_of(
                   _runners.cbegin(),
                   _runners.cend(),
                   [](Runner* rnnr) -> bool { return rnnr->finished(); })) {
          func(_runners.at(0));
          if (_runners.at(0)->finished()) {
            _winner = _runners.at(0);
          }
          return;
        }
        std::vector<std::thread::id> tids(_runners.size(),
                                          std::this_thread::get_id());

        REPORT("using ",
               nr_threads,
               " / ",
               std::thread::hardware_concurrency(),
               " threads");

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
        for (auto method = _runners.begin(); method < _runners.end();
             ++method) {
          if ((*method)->finished()) {
            LIBSEMIGROUPS_ASSERT(_winner == nullptr);
            _winner    = *method;
            size_t tid = REPORTER.thread_id(tids.at(method - _runners.begin()));
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
      // TODO consider making it possible to keep the dead runners.
    }

    std::vector<Runner*> _runners;
    size_t               _max_threads;
    std::mutex           _mtx;
    Runner*              _winner;
  };
}  // namespace libsemigroups

#endif  // LIBSEMIGROUPS_INCLUDE_INTERNAL_RACE_H_
