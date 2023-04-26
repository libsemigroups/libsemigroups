//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2020 James D. Mitchell
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

#include "libsemigroups/forest.hpp"

#include <algorithm>         // for fill
#include <cstddef>           // for size_t
#include <initializer_list>  // for initializer_list
#include <iosfwd>            // for ostream
#include <vector>            // for vector, allocator, operator==

#include "libsemigroups/constants.hpp"  // for Max, operator!=, operator==
#include "libsemigroups/exception.hpp"  // for LIBSEMIGROUPS_EXCEPTION
#include "libsemigroups/string.hpp"     // for operator<<
#include "libsemigroups/types.hpp"      // for word_type

namespace libsemigroups {
  Forest::~Forest() = default;

  Forest to_forest(std::initializer_list<size_t> parent,
                   std::initializer_list<size_t> edge_label) {
    if (parent.size() != edge_label.size()) {
      LIBSEMIGROUPS_EXCEPTION(
          "expected the 1st and 2nd arguments (parents and edge labels) to "
          "have equal size equal, found {} != {}",
          parent.size(),
          edge_label.size());
    }
    size_t const num_nodes = parent.size();
    Forest       result(num_nodes);
    for (size_t i = 0; i < num_nodes; ++i) {
      auto p = *(parent.begin() + i);
      auto l = *(edge_label.begin() + i);
      if (p != UNDEFINED && l != UNDEFINED) {
        result.set(i, p, l);
      } else if (!(p == UNDEFINED && l == UNDEFINED)) {
        LIBSEMIGROUPS_EXCEPTION(
            "roots not at the same indices in the 1st and 2nd arguments "
            "(parents and edge labels), expected UNDEFINED at index {} found "
            "{} and {}",
            i,
            p,
            l);
      }
    }
    return result;
  }

  std::ostream& operator<<(std::ostream& os, Forest const& f) {
    os << "{" << f.parents() << ", " << f.labels() << "}";
    return os;
  }
}  // namespace libsemigroups
