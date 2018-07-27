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
// - implement a run_for method.
// - consider keeping killed off methods.
// - Race should inherit from Runner.

#ifndef LIBSEMIGROUPS_SRC_RACE_H_
#define LIBSEMIGROUPS_SRC_RACE_H_

#include <thread>
#include <vector>

#include "runner.h"
#include "report.h"

namespace libsemigroups {
  class Race { // public Runner
   public:
    explicit Race(size_t max_threads = std::thread::hardware_concurrency());

    void set_max_threads(size_t val);
    Runner* winner();
    void add_runner(Runner* r);//, std::function<void()> func);

    typename std::vector<Runner*>::iterator begin();
    typename std::vector<Runner*>::iterator end();

    // TODO check if all these iterator methods are actually used
    typename std::vector<Runner*>::const_iterator begin() const;
    typename std::vector<Runner*>::const_iterator end() const;

    bool empty() const;

   private:
    std::vector<Runner*> _runners;
    size_t               _max_threads;
    std::mutex           _mtx;
    Runner*              _winner;
  };
}  // namespace libsemigroups

#endif  // LIBSEMIGROUPS_SRC_RACE_H_
