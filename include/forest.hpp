//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2018 Finn Smith
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

#ifndef LIBSEMIGROUPS_INCLUDE_FOREST_HPP_
#define LIBSEMIGROUPS_INCLUDE_FOREST_HPP_

#include <stddef.h>  // for size_t
#include <vector>    // for vector

#include "constants.hpp"

namespace libsemigroups {
  class Forest {
   public:
    explicit Forest(size_t n)
        : _edge_label(n, UNDEFINED), _parent(n, UNDEFINED) {}

    void set(size_t node, size_t parent, size_t gen) noexcept {
      // TODO(FLS): add assertions
      _parent[node]     = parent;
      _edge_label[node] = gen;
    }

    size_t size() const noexcept {
      return _parent.size();
    }

    size_t parent(size_t i) const noexcept {
      // TODO(FLS): add assertions
      return _parent[i];
    }

    std::vector<size_t>::const_iterator cbegin_parent() const {
      return _parent.cbegin();
    }

    std::vector<size_t>::const_iterator cend_parent() const {
      return _parent.cend();
    }

    size_t label(size_t i) const noexcept {
      // TODO(FLS): add assertions
      return _edge_label[i];
    }

   private:
    std::vector<size_t> _edge_label;
    std::vector<size_t> _parent;
  };
}  // namespace libsemigroups

#endif  // LIBSEMIGROUPS_INCLUDE_FOREST_HPP_
