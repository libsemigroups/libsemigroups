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

#include "internal/libsemigroups-config.hpp"
#include "internal/libsemigroups-debug.hpp"
#include "internal/libsemigroups-exception.hpp"
#include "internal/race.hpp"
#include "internal/range.hpp"
#include "internal/recvec.hpp"
#include "internal/report.hpp"
#include "internal/runner.hpp"
#include "internal/square.hpp"
#include "internal/stl.hpp"
#include "internal/timer.hpp"
#include "internal/uf.hpp"

#include "adapters.hpp"
#include "blocks.hpp"
#include "bmat8.hpp"
#include "cong-base.hpp"
#include "cong.hpp"
#include "cong-pair.hpp"
#include "wrap.hpp"
#include "constants.hpp"
#include "element.hpp"
#include "fpsemi-base.hpp"
#include "fpsemi.hpp"
#include "hpcombi.hpp"
#include "kb-order.hpp"
#include "kbe.hpp"
#include "knuth-bendix.hpp"
#include "schreier-sims.hpp"
#include "semigroup-base.hpp"
#include "semigroup-traits.hpp"
#include "semigroup.hpp"
#include "semiring.hpp"
#include "tce.hpp"
#include "todd-coxeter.hpp"
#include "types.hpp"

#ifdef LIBSEMIGROUPS_DENSEHASHMAP
#include "extern/sparsehash-c11/sparsehash/dense_hash_map"
#endif

#endif  // LIBSEMIGROUPS_INCLUDE_LIBSEMIGROUPS_H_
