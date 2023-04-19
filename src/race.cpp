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

// This file contains the implementation of the Race class template for
// competitively running different functions/methods in different threads, and
// obtaining the winner.
//

#include "libsemigroups/race.hpp"

#include <functional>  // for mem_fn
#include <thread>

#include "libsemigroups/exception.hpp"  // for LibsemigroupException
#include "libsemigroups/runner.hpp"     // for Runner

namespace libsemigroups {
  namespace detail {

    Race::~Race() = default;
    Race::Race()
        : _max_threads(std::thread::hardware_concurrency()),
          _mtx(),
          _winner(nullptr) {}

    void Race::add_runner(std::shared_ptr<Runner> r) {
      if (_winner != nullptr) {
        LIBSEMIGROUPS_EXCEPTION("the race is over, cannot add runners");
      }
      _runners.push_back(r);
    }

    void Race::run() {
      if (empty()) {
        LIBSEMIGROUPS_EXCEPTION("no runners given, cannot run");
      }
      run_func(std::mem_fn(&Runner::run));
    }

    void Race::run_for(std::chrono::nanoseconds x) {
      if (empty()) {
        LIBSEMIGROUPS_EXCEPTION("no runners given, cannot run_for");
      }
      run_func([&x](std::shared_ptr<Runner> r) -> void { r->run_for(x); });
    }
  }  // namespace detail
}  // namespace libsemigroups
