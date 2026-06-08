//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2023-2026 Joseph Edwards + James D. Mitchell
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

// This file only exists for backwards compatibility.

// TODO(v4) delete this file.

#ifndef LIBSEMIGROUPS_DETAIL_REWRITERS_HPP_
#define LIBSEMIGROUPS_DETAIL_REWRITERS_HPP_

#if defined(_MSC_VER)
#pragma message("Including libsemigroups/detail/rewriters.hpp is "           \
                "deprecated; use libsemigroups/detail/rewriting-system.hpp " \
                "instead")
#else
#warning "Including libsemigroups/detail/rewriters.hpp is deprecated; use " \
    "libsemigroups/detail/rewriting-system.hpp instead"
#endif

#include "rewriting-system.hpp"
#endif  // LIBSEMIGROUPS_DETAIL_REWRITERS_HPP_
