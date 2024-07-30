//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019-2024 James D. Mitchell
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

// This file declares debugging functionality.

#ifndef LIBSEMIGROUPS_DEBUG_HPP_
#define LIBSEMIGROUPS_DEBUG_HPP_

#include "config.hpp"

#ifdef LIBSEMIGROUPS_DEBUG
#include <cassert>
#define LIBSEMIGROUPS_ASSERT(x) assert(x)
#else
#define LIBSEMIGROUPS_ASSERT(x)
#endif

#if (defined(__GNUC__) && __GNUC__ < 9 \
     && !(defined(__clang__) || defined(__INTEL_COMPILER)))
#error "GCC version 9.0 or higher is required"
#endif

#endif  // LIBSEMIGROUPS_DEBUG_HPP_
