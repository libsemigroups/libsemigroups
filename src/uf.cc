//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2017 Michael Torpey
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

// This file defines UF, a class used to make an equivalence relation on
// the integers {1 .. n}, using the UNION-FIND METHOD: new pairs can be added
// and the appropriate classes combined quickly.

#include "uf.h"

#include "libsemigroups-debug.h"

namespace libsemigroups {

  // Copy constructor
  UF::UF(const UF& copy)
      : _size(copy._size),
        _table(new table_t(*copy._table)),
        _blocks(nullptr),
        _haschanged(copy._haschanged) {
    if (copy._blocks != nullptr) {
      // Copy the blocks as well
      _blocks = new blocks_t();
      _blocks->reserve(copy._blocks->size());
      for (auto const& block : *copy._blocks) {
        if (block == nullptr) {
          _blocks->push_back(nullptr);
        } else {
          _blocks->push_back(new table_t(*block));
        }
      }
    }
  }

  // Constructor by table
  UF::UF(const table_t& table)
      : _size(table.size()),
        _table(new table_t(table)),
        _blocks(nullptr),
        _haschanged(true),
        _next_rep(0) {}

  // Constructor by size
  UF::UF(size_t size)
      : _size(size),
        _table(new table_t()),
        _blocks(nullptr),
        _haschanged(false),
        _next_rep(0) {
    _table->reserve(size);
    for (size_t i = 0; i < size; i++) {
      _table->push_back(i);
    }
  }

  // Destructor
  UF::~UF() {
    delete _table;
    if (_blocks != nullptr) {
      for (size_t i = 0; i < _blocks->size(); i++) {
        delete _blocks->at(i);
      }
      delete _blocks;
    }
  }

  // Getters
  size_t UF::get_size() {
    return _size;
  }

  UF::table_t* UF::get_table() {
    return _table;
  }

  // get_blocks
  UF::blocks_t* UF::get_blocks() {
    table_t* block;
    // Is _blocks "bound" yet?
    if (_blocks == nullptr) {
      _blocks = new blocks_t();
      _blocks->reserve(_size);
      for (size_t i = 0; i < _size; i++) {
        block = new table_t(1, i);
        _blocks->push_back(block);
      }
    }
    // Do we need to update the blocks?
    if (_haschanged) {
      size_t ii;
      for (size_t i = 0; i < _size; i++) {
        if (_blocks->at(i) != nullptr) {
          ii = find(i);
          if (ii != i) {
            // Combine the two blocks
            _blocks->at(ii)->reserve(_blocks->at(ii)->size()
                                     + _blocks->at(i)->size());
            _blocks->at(ii)->insert(_blocks->at(ii)->end(),
                                    _blocks->at(i)->begin(),
                                    _blocks->at(i)->end());
            delete _blocks->at(i);
            _blocks->at(i) = nullptr;
          }
        }
      }
      _haschanged = false;
    }
    return _blocks;
  }

  // find
  size_t UF::find(size_t i) {
    size_t ii;
    LIBSEMIGROUPS_ASSERT(_size == _table->size());
    do {
      ii = i;
      LIBSEMIGROUPS_ASSERT(ii < _size);
      i = (*_table)[ii];
    } while (ii != i);
    return i;
  }

  // union
  void UF::unite(size_t i, size_t j) {
    size_t ii, jj;
    LIBSEMIGROUPS_ASSERT(_size == _table->size());
    ii = find(i);
    jj = find(j);
    if (ii < jj) {
      LIBSEMIGROUPS_ASSERT(jj < _size);
      (*_table)[jj] = ii;
    } else {
      LIBSEMIGROUPS_ASSERT(ii < _size);
      (*_table)[ii] = jj;
    }
    _haschanged = true;
  }

  // flatten
  void UF::flatten() {
    LIBSEMIGROUPS_ASSERT(_size == _table->size());
    for (size_t i = 0; i < _size; i++) {
      (*_table)[i] = find(i);
    }
  }

  // add_entry
  void UF::add_entry() {
    _table->push_back(_size);
    if (_blocks != nullptr) {
      _blocks->push_back(new table_t(1, _size));
    }
    _size++;
  }

  // nr_blocks
  size_t UF::nr_blocks() {
    LIBSEMIGROUPS_ASSERT(_size == _table->size());
    if (_size == 0) {
      return 0;
    }
    size_t count = 1;
    size_t max   = 0;
    flatten();  // So we can use [i] instead of ->find(i)
    LIBSEMIGROUPS_ASSERT((*_table)[0] == 0);
    for (size_t i = 1; i < _size; i++) {
      if ((*_table)[i] > max) {
        max = (*_table)[i];
        count++;
      }
    }
    return count;
  }

  void UF::reset_next_rep() {
    flatten();
    _next_rep = 0;
  }

  // Returns the next representative of a block, invalidated by anything that
  // changes the partition
  size_t UF::next_rep() {
    size_t current_rep = _next_rep;
    while (_next_rep < _size && (*_table)[_next_rep] <= current_rep) {
      _next_rep++;
    }
    return current_rep;
  }

  void UF::join(UF const& uf) {
    LIBSEMIGROUPS_ASSERT(this->_size == uf._size);
    for (size_t i = 0; i < _size; i++) {
      unite((*_table)[i], (*uf._table)[i]);
    }
  }
}  // namespace libsemigroups
