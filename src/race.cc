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

// This file contains the implementation of the Race class template for
// competitively running different functions/methods in different threads, and
// obtaining the winner.
//

#include "internal/race.h"

namespace libsemigroups {

  Race::Race(size_t max_threads) : _mtx(), _winner(nullptr) {
    unsigned int n
        = static_cast<unsigned int>(max_threads == 0 ? 1 : max_threads);
    _max_threads = std::min(n, std::thread::hardware_concurrency());
  }

  void Race::set_max_threads(size_t val) {
    _max_threads = val;
  }

  Runner* Race::winner() {
    run();
    return _winner;
  }

  void Race::add_runner(Runner* r) {
    LIBSEMIGROUPS_ASSERT(_winner == nullptr);
    _runners.push_back(r);
  }

  typename std::vector<Runner*>::iterator Race::begin() {
    return _runners.begin();
  }

  typename std::vector<Runner*>::iterator Race::end() {
    return _runners.end();
  }

  // TODO check if all these iterator methods are actually used
  typename std::vector<Runner*>::const_iterator Race::begin() const {
    return _runners.cbegin();
  }

  typename std::vector<Runner*>::const_iterator Race::end() const {
    return _runners.cend();
  }

  bool Race::empty() const {
    return _runners.empty();
  }

}  // namespace libsemigroups
