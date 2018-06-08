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

#ifndef LIBSEMIGROUPS_SRC_TO_STRING_H_
#define LIBSEMIGROUPS_SRC_TO_STRING_H_

#include <sstream>

// It appears that GCC 4.9.1 (at least) do not have std::to_string implemented,
// so we implement our own.

namespace libsemigroups {
  template <typename T> std::string to_string(const T& n) {
    std::ostringstream stm;
    stm << n;
    return stm.str();
  }
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_SRC_TO_STRING_H_
