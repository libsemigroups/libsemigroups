//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2017 James D. Mitchell
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

#ifndef LIBSEMIGROUPS_SRC_LIBSEMIGROUPS_DEBUG_H_
#define LIBSEMIGROUPS_SRC_LIBSEMIGROUPS_DEBUG_H_

#include <assert.h>

// Do not include config/config.h if we are compiling inside the Semigroups
// package for GAP since this includes gap/gen/config.h where the PACKAGE_*
// macros are redefined, causing compiler warnings.
#ifndef DO_NOT_INCLUDE_CONFIG_H
#include <libsemigroups-config.h>
#endif

#ifdef LIBSEMIGROUPS_DEBUG
#define LIBSEMIGROUPS_ASSERT(x) assert(x)
#else
#define LIBSEMIGROUPS_ASSERT(x)
#endif

#endif  // LIBSEMIGROUPS_SRC_LIBSEMIGROUPS_DEBUG_H_
