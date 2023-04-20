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

namespace libsemigroups {
  Forest::~Forest() = default;

  Forest to_forest(size_t                        num_nodes,
                   std::initializer_list<size_t> parent,
                   std::initializer_list<size_t> edge_label) {
    if (num_nodes != parent.size()) {
      LIBSEMIGROUPS_EXCEPTION_V3("TODO1");
    } else if (num_nodes != edge_label.size()) {
      LIBSEMIGROUPS_EXCEPTION_V3("TODO2");
    }

    Forest result(num_nodes);
    for (size_t i = 0; i < num_nodes; ++i) {
      auto p = *(parent.begin() + i);
      auto l = *(edge_label.begin() + i);
      if (p != UNDEFINED && l != UNDEFINED) {
        result.set(i, p, l);
      } else if (!(p == UNDEFINED && l == UNDEFINED)) {
        LIBSEMIGROUPS_EXCEPTION_V3("TODO3");
      }
    }
    return result;
  }

  // TODO to cpp file
  std::ostream& operator<<(std::ostream& os, Forest const& f) {
    os << "{" << f.number_of_nodes() << ", "
       << std::vector<size_t>(f.cbegin_parent(), f.cend_parent()) << ", "
       << std::vector<size_t>(f.cbegin_labels(), f.cend_labels()) << "}";
    return os;
  }
}  // namespace libsemigroups
