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

// This file includes all the headers in libsemigroups, for convenience.

#ifndef LIBSEMIGROUPS_SRC_LIBSEMIGROUPS_H_
#define LIBSEMIGROUPS_SRC_LIBSEMIGROUPS_H_

#include "blocks.h"
#include "bmat8.h"
#include "cong.h"
#include "elements.h"
#include "eltcont.h"
#include "libsemigroups-config.h"
#include "libsemigroups-debug.h"
#include "libsemigroups-exception.h"
#include "partition.h"
#include "recvec.h"
#include "report.h"
#include "rws.h"
#include "rwse.h"
#include "semigroups-base.h"
#include "semigroups.h"
#include "semiring.h"
#include "timer.h"
#include "uf.h"

#ifdef LIBSEMIGROUPS_USE_HPCOMBI
#include "../extern/HPCombi/include/perm16.hpp"
#endif

#endif  // LIBSEMIGROUPS_SRC_LIBSEMIGROUPS_H_
