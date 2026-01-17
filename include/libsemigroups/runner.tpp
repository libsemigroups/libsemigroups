//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2024-2026 James D. Mitchell
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

  template <typename Func>
  void Runner::run_until(Func&& func) {
    static_assert(
        std::is_same_v<std::invoke_result_t<Func>, bool>,
        "the result type of calling an object of type Func (the template "
        "parameter) must be bool!");
    if (!finished() && !dead()) {
      report_default("{}: running until predicate returns true or finished\n",
                     report_prefix());

      _stopper = std::forward<Func>(func);
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
