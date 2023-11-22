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

#include "libsemigroups/to-knuth-bendix.hpp"

#include "libsemigroups/froidure-pin-base.hpp"  // for FroidurePinBase
#include "libsemigroups/froidure-pin.hpp"       // for FroidurePin
#include "libsemigroups/knuth-bendix.hpp"       // for KnuthBendix<>
#include "libsemigroups/todd-coxeter.hpp"       // for ToddCoxeter
#include "libsemigroups/types.hpp"              // for congruence_kind

namespace libsemigroups {

  KnuthBendix<> to_knuth_bendix(congruence_kind knd, FroidurePinBase& fp) {
    return KnuthBendix<>(knd, to_presentation<word_type>(fp));
  }

  KnuthBendix<> to_knuth_bendix(congruence_kind knd, ToddCoxeter const& tc) {
    return KnuthBendix<>(knd, tc.presentation());
  }
}  // namespace libsemigroups
