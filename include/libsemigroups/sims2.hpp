//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2023 James D. Mitchell
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

// This file contains a declaration of a class for performing the "low-index
// congruence" algorithm for 2-sided congruences of semigroups and monoids.

// This file and the class Sims2 exist because iterating through 2-sided
// congruences is fundamentally different that iterating through 1-sided
// congruences. In more words, iterating through 2-sided congruences requires
// some more steps than iterating through 1-sided congruences. It might have
// been more pleasing to allow Sims1 objects (maybe appropriately renamed) to
// accept congruence_kind::twosided as their "kind". However, this would either
// have required:
// 1. run time checks in the iterator_base::try_define function (and probably
//    elsewhere too) if we were enumerating 1-sided or 2-sided congruences.
// 2. making iterator_base_1_sided and iterator_base_2_sided, and then
//    templating iterator and thread_iterator to use the appropriate type
//    depending on the value of "kind()".
//
// The disadvantage of 1 is that it would likely be slower, since try_define is
// the critical function for Sims1. It also makes the code more complicated,
// and breaks backwards compatibility. The disadvantage of 2 is that the return
// type of "cbegin" and "cend" depends on whether we are iterating through 1- or
// 2-sided congruences. In other words, number 2 above doesn't actually work.
// Hence we've opted for just having a separate class for low-index 2-sided
// congruences. This is also cleaner since we don't have to touch the impl of
// Sims1 (much) and is more backwards compatible.

#ifndef LIBSEMIGROUPS_SIMS2_HPP_
#define LIBSEMIGROUPS_SIMS2_HPP_

#include "sims1.hpp"

namespace libsemigroups {}  // namespace libsemigroups
#endif                      // LIBSEMIGROUPS_SIMS2_HPP_
