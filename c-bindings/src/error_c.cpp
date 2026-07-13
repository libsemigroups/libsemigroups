// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2025-2026 Joshua James-lee
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

#include "error_c.h"
#include "error_internal.hpp"

#include <string>

namespace lsemi_hs {
  thread_local std::string g_last_error_message;
}

extern "C" const char* lsemi_last_error_message(void) {
  return lsemi_hs::g_last_error_message.c_str();
}

extern "C" void lsemi_clear_last_error(void) {
  lsemi_hs::g_last_error_message.clear();
}
