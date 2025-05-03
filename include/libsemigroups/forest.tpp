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

  template <typename Return>
  [[nodiscard]] enable_if_is_same<Return, Forest>
  make(std::vector<size_t> const& parent,
       std::vector<size_t> const& edge_labels) {
    if (parent.size() != edge_labels.size()) {
      LIBSEMIGROUPS_EXCEPTION(
          "expected the 1st and 2nd arguments (parents and edge labels) to "
          "have equal size equal, found {} != {}",
          parent.size(),
          edge_labels.size());
    }
    size_t const num_nodes = parent.size();
    Forest       result(num_nodes);
    for (size_t i = 0; i < num_nodes; ++i) {
      auto p = *(parent.begin() + i);
      auto l = *(edge_labels.begin() + i);
      if (p != UNDEFINED && l != UNDEFINED) {
        result.set_parent_and_label(i, p, l);
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

}  // namespace libsemigroups
