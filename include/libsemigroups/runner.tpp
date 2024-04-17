//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2024 James D. Mitchell
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

// This file contains the implementation of the function templates declared in
// runner.hpp

namespace libsemigroups {

  ////////////////////////////////////////////////////////////////////////
  // Runner
  ////////////////////////////////////////////////////////////////////////

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
