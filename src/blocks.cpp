//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019 James D. Mitchell
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

// Blocks are stored internally as a list consisting of:
//
// [ nr of blocks, internal rep of blocks, transverse blocks ]
//
// <nr of blocks> is a non-negative integer, <internal rep of blocks>[i] = j if
// <i> belongs to the <j>th block, <transverse blocks>[j] = true if block <j> is
// transverse and false if it is not.

#include "blocks.hpp"

#include <algorithm>  // for max_element, count
#include <cstdint>    // for uint32_t

#include "constants.hpp"            // for UNDEFINED
#include "libsemigroups-debug.hpp"  // for LIBSEMIGROUPS_ASSERT

namespace libsemigroups {

  Blocks::Blocks(std::vector<uint32_t>* blocks,
                 std::vector<bool>*     lookup,
                 uint32_t               nr_blocks) noexcept
      : _blocks(blocks),
        _lookup(lookup),
        _nr_blocks(nr_blocks),
        _rank(UNDEFINED) {
    LIBSEMIGROUPS_ASSERT(_blocks->size() != 0);
    LIBSEMIGROUPS_ASSERT(_nr_blocks == _lookup->size());
  }

  Blocks::Blocks(std::vector<uint32_t>* blocks, std::vector<bool>* lookup)
      : Blocks(blocks,
               lookup,
               *(std::max_element(blocks->cbegin(), blocks->cend())) + 1) {}

  Blocks::Blocks(Blocks const& copy)
      : _blocks(nullptr),
        _lookup(nullptr),
        _nr_blocks(copy._nr_blocks),
        _rank(copy._rank) {
    if (copy._blocks != nullptr) {
      LIBSEMIGROUPS_ASSERT(copy._lookup != nullptr);
      _blocks = new std::vector<uint32_t>(*copy._blocks);
      _lookup = new std::vector<bool>(*copy._lookup);
    } else {
      LIBSEMIGROUPS_ASSERT(copy._lookup == nullptr);
    }
  }

  bool Blocks::operator==(const Blocks& that) const {
    if (this->degree() != that.degree()
        || this->_nr_blocks != that._nr_blocks) {
      return false;
    } else if (this->_nr_blocks == 0) {
      return true;
    }
    return (*(this->_blocks) == *(that._blocks)
            && *(this->_lookup) == *(that._lookup));
  }

  bool Blocks::operator<(const Blocks& that) const {
    if (this->degree() != that.degree()) {
      return (this->degree() < that.degree());
    }
    for (size_t i = 0; i < this->degree(); i++) {
      if (((*this->_blocks)[i] != (*that._blocks)[i])) {
        return (*this->_blocks)[i] < (*that._blocks)[i];
      }
    }
    for (size_t i = 0; i < this->nr_blocks(); i++) {
      if (((*this->_lookup)[i] && !(*that._lookup)[i])) {
        return true;
      } else if ((!(*this->_lookup)[i] && (*that._lookup)[i])) {
        return false;
      }
    }
    return false;
  }

  uint32_t Blocks::rank() {
    if (_rank == UNDEFINED) {
      _rank = std::count(_lookup->cbegin(), _lookup->cend(), true);
    }
    return _rank;
  }

  size_t Blocks::hash_value() const noexcept {
    if (_nr_blocks == 0) {
      return 0;
    }
    size_t seed = 0;
    size_t n    = _blocks->size();
    for (auto const& index : *_blocks) {
      seed = ((seed * n) + index);
    }
    for (auto val : *_lookup) {
      seed = ((seed * n) + val);
    }
    return seed;
  }

}  // namespace libsemigroups
