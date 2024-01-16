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

#ifndef LIBSEMIGROUPS_TO_KNUTH_BENDIX_HPP_
#define LIBSEMIGROUPS_TO_KNUTH_BENDIX_HPP_

#include "knuth-bendix.hpp"

namespace libsemigroups {

  class FroidurePinBase;
  class ToddCoxeter;
  enum class congruence_kind;

  KnuthBendix<> to_knuth_bendix(congruence_kind knd, FroidurePinBase& fp);
  KnuthBendix<> to_knuth_bendix(congruence_kind knd, ToddCoxeter const& tc);

}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_TO_KNUTH_BENDIX_HPP_
