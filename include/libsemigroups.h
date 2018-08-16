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

#ifndef LIBSEMIGROUPS_INCLUDE_LIBSEMIGROUPS_H_
#define LIBSEMIGROUPS_INCLUDE_LIBSEMIGROUPS_H_

#include "internal/libsemigroups-config.h"
#include "internal/libsemigroups-debug.h"
#include "internal/libsemigroups-exception.h"
#include "internal/race.h"
#include "internal/range.h"
#include "internal/recvec.h"
#include "internal/report.h"
#include "internal/runner.h"
#include "internal/square.h"
#include "internal/stl.h"
#include "internal/timer.h"
#include "internal/uf.h"

#include "adapters.h"
#include "blocks.h"
#include "bmat8.h"
#include "cong-base.hpp"
#include "cong-new.h"
#include "cong-p.h"
#include "cong-wrap.h"
#include "constants.h"
#include "element-helper.h"
#include "element.h"
#include "fpsemi-base.hpp"
#include "fpsemi.h"
#include "hpcombi.h"
#include "kb-order.h"
#include "kbe.h"
#include "knuth-bendix.h"
#include "schreier-sims.h"
#include "semigroup-base.h"
#include "semigroup-traits.h"
#include "semigroup.h"
#include "semiring.h"
#include "tce.h"
#include "todd-coxeter.h"
#include "types.h"

#ifdef LIBSEMIGROUPS_DENSEHASHMAP
#include "extern/sparsehash-c11/sparsehash/dense_hash_map"
#endif

#endif  // LIBSEMIGROUPS_INCLUDE_LIBSEMIGROUPS_H_
