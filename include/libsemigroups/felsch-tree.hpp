//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2022 James D. Mitchell
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

#ifndef LIBSEMIGROUPS_FELSCH_TREE_HPP_
#define LIBSEMIGROUPS_FELSCH_TREE_HPP_

#include <cstddef>  // for size_t
#include <vector>   // for vector

#include "constants.hpp"   // for UNDEFINED
#include "containers.hpp"  // for DynamicArray2
#include "debug.hpp"       // for LIBSEMIGROUPS_ASSERT
#include "types.hpp"       // for word_type

namespace libsemigroups {

  namespace detail {
    class FelschTree {
     public:
      using index_type     = size_t;
      using state_type     = size_t;
      using const_iterator = std::vector<index_type>::const_iterator;
      static constexpr state_type initial_state = 0;

      explicit FelschTree(size_t n)
          : _automata(n, 1, initial_state),
            _index(1, std::vector<index_type>({})),
            _parent(1, state_type(UNDEFINED)),
            _length(0) {}

      FelschTree(FelschTree const&) = default;

      using word_iterator = typename std::vector<word_type>::const_iterator;

      void add_relations(word_iterator first, word_iterator last);

      void push_back(letter_type x) {
        _length        = 1;
        _current_state = _automata.get(initial_state, x);
      }

      bool push_front(letter_type x);

      void pop_front() {
        LIBSEMIGROUPS_ASSERT(_length > 0);
        _length--;
        _current_state = _parent[_current_state];
      }

      const_iterator cbegin() const {
        LIBSEMIGROUPS_ASSERT(_current_state < _index.size());
        return _index[_current_state].cbegin();
      }

      const_iterator cend() const {
        LIBSEMIGROUPS_ASSERT(_current_state < _index.size());
        return _index[_current_state].cend();
      }

      size_t length() const noexcept {
        return _length;
      }

      size_t number_of_nodes() const noexcept {
        return _parent.size();
      }

      size_t height() const noexcept;

     private:
      using StateTable = detail::DynamicArray2<state_type>;
      StateTable                           _automata;
      state_type                           _current_state;
      std::vector<std::vector<index_type>> _index;
      std::vector<state_type>              _parent;
      size_t                               _length;
    };
  }  // namespace detail
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_FELSCH_TREE_HPP_
