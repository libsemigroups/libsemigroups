//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2025 James D. Mitchell
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

// This file contains implementations of the template member functions of the
// Forest class.

namespace libsemigroups {

  template <typename Iterator>
  Iterator Forest::path_to_root_no_checks(Iterator d_first, node_type i) const {
    LIBSEMIGROUPS_ASSERT(i < _parent.size());
    LIBSEMIGROUPS_ASSERT(i < _edge_label.size());
    auto it = d_first;
    for (; parent_no_checks(i) != UNDEFINED; ++it) {
      *it = label_no_checks(i);
      LIBSEMIGROUPS_ASSERT(i != parent_no_checks(i));
      i = parent_no_checks(i);
    }
    return it;
  }

  template <typename Iterator>
  Iterator Forest::path_from_root_no_checks(Iterator  d_first,
                                            node_type i) const {
    LIBSEMIGROUPS_ASSERT(i < _parent.size());
    LIBSEMIGROUPS_ASSERT(i < _edge_label.size());
    auto const j = parent_no_checks(i);
    if (j != UNDEFINED) {
      d_first  = path_from_root_no_checks(d_first, j);
      *d_first = label_no_checks(i);
      ++d_first;
    }
    return d_first;
  }

}  // namespace libsemigroups
