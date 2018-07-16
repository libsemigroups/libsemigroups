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

#ifndef LIBSEMIGROUPS_SRC_TYPES_H_
#define LIBSEMIGROUPS_SRC_TYPES_H_

#include <vector>

namespace libsemigroups {
  //! Type for the index of a generator of a semigroup.
  using letter_type = size_t;

  //! Type for a word over the generators of a semigroup.
  using word_type = std::vector<letter_type>;

  //! Type for a pair of word_type (a *relation*) of a semigroup.
  using relation_type = std::pair<word_type, word_type>;
}

#endif  // LIBSEMIGROUPS_SRC_TYPES_H_
