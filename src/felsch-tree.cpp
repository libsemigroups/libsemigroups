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

#include "libsemigroups/felsch-tree.hpp"

#include <algorithm>                    // for binary_search, max
#include <cstddef>                      // for size_t
#include <iterator>                     // for distance
#include <vector>                       // for vector
                                        //
#include "libsemigroups/constants.hpp"  // for Undefined, UNDEFINED
#include "libsemigroups/debug.hpp"      // for LIBSEMIGROUPS_ASSERT
#include "libsemigroups/types.hpp"      // for letter_type

namespace libsemigroups {

  namespace detail {

    void FelschTree::add_relations(word_iterator first, word_iterator last) {
      size_t number_of_words = 0;
      LIBSEMIGROUPS_ASSERT(std::distance(first, last) % 2 == 0);
      for (auto wit = first; wit != last; ++wit) {
        // For every prefix [wit->cbegin(), last)
        for (auto suffix = wit->cend(); suffix > wit->cbegin(); --suffix) {
          // For every suffix [prefix, suffix) of the prefix [wit->cbegin(),
          // suffix)
          for (auto prefix = wit->cbegin(); prefix < suffix; ++prefix) {
            // Find the maximal suffix of [prefix, suffix) that corresponds to
            // an existing state . . .
            auto       it = suffix - 1;
            state_type s  = initial_state;
            while (_automata.get(s, *it) != initial_state && it > prefix) {
              s = _automata.get(s, *it);
              --it;
            }
            if (_automata.get(s, *it) == initial_state) {
              // [it + 1, suffix) is the maximal suffix of [prefix, suffix)
              // that corresponds to the existing state s
              size_t number_of_states = _automata.number_of_rows();
              _automata.add_rows((it + 1) - prefix);
              _index.resize(_index.size() + ((it + 1) - prefix), {});
              _parent.resize(_parent.size() + ((it + 1) - prefix), UNDEFINED);
              while (it >= prefix) {
                // Add [it, suffix) as a new state
                _automata.set(s, *it, number_of_states);
                _parent[number_of_states] = s;
                s                         = number_of_states;
                number_of_states++;
                it--;
              }
            }
          }
          // Find the state corresponding to the prefix [wit->cbegin(),
          // suffix)
          auto       it = suffix - 1;
          state_type s  = initial_state;
          while (it >= wit->cbegin()) {
            s = _automata.get(s, *it);
            LIBSEMIGROUPS_ASSERT(s != initial_state);
            --it;
          }
          index_type m = ((number_of_words % 2) == 0 ? number_of_words
                                                     : number_of_words - 1);
          // The index corresponding to the side of the relation
          // corresponding to the prefix is what's pushed into _index[s], so
          // some care is required when using the return value of this.
          if (!std::binary_search(_index[s].cbegin(), _index[s].cend(), m)) {
            _index[s].push_back(number_of_words);
          }
        }
        number_of_words++;
      }
      LIBSEMIGROUPS_ASSERT(_index[0].empty());
    }

    bool FelschTree::push_front(letter_type x) {
      LIBSEMIGROUPS_ASSERT(x < _automata.number_of_cols());
      auto y = _automata.get(_current_state, x);
      if (y != initial_state) {
        _length++;
        _current_state = y;
        return true;
      } else {
        return false;
      }
    }

    size_t FelschTree::height() const noexcept {
      size_t result = 0;
      for (state_type s = 1; s < _parent.size(); ++s) {
        size_t     h = 0;
        state_type t = s;
        while (t != initial_state) {
          h++;
          t = _parent[t];
        }
        result = std::max(h, result);
      }
      return result;
    }

  }  // namespace detail
}  // namespace libsemigroups
