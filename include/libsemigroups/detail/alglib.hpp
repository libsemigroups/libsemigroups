//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2026 Joseph Edwards
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

#ifndef LIBSEMIGROUPS_DETAIL_ALGLIB_HPP_
#define LIBSEMIGROUPS_DETAIL_ALGLIB_HPP_

#include "libsemigroups/config.hpp"

#ifdef LIBSEMIGROUPS_ALGLIB_ENABLED

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wredundant-decls"
#pragma GCC diagnostic ignored "-Wundef"
#pragma GCC diagnostic ignored "-Winline"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#ifdef WITH_INTERNAL_ALGLIB
#include "alglib-4.08.0/src/minlp.h"
#else
#include "minlp.h"
#endif
#pragma GCC diagnostic pop

#endif  // LIBSEMIGROUPS_ALGLIB_ENABLED

#endif  // LIBSEMIGROUPS_DETAIL_ALGLIB_HPP_
