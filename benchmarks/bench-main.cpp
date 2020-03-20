//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2020 James D. Mitchell
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

#define CATCH_CONFIG_ENABLE_BENCHMARKING
#define CATCH_CONFIG_FAST_COMPILE
#define CATCH_CONFIG_MAIN

#include <sstream>

#include "catch.hpp"

namespace libsemigroups {
  std::string to_hex_string(size_t i) {
    std::stringstream s;
    s << "0x" << std::hex << std::uppercase << i;
    return s.str();
  }
}  // namespace libsemigroups
