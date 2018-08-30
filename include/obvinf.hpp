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

// This file contains a helper class for checking whether or not a congruence
// defined by

#ifndef LIBSEMIGROUPS_INCLUDE_OBVINF_HPP_
#define LIBSEMIGROUPS_INCLUDE_OBVINF_HPP_

#include <stddef.h>       // for size_t
#include <string>         // for string
#include <unordered_map>  // for unordered_map
#include <unordered_set>  // for unordered_set
#include <utility>        // for pair
#include <vector>         // for vector

namespace libsemigroups {
  namespace internal {
    template <typename TLetterType, typename TWordType>
    class IsObviouslyInfinite {
     public:
      explicit IsObviouslyInfinite(size_t n)
          : _empty_word(false), _map(), _nr_gens(n), _preserve(), _unique() {}

      explicit IsObviouslyInfinite(std::string const& lphbt)
          : IsObviouslyInfinite(lphbt.size()) {}

      void
      add_rules(std::vector<std::pair<TWordType, TWordType>> const& rules) {
        std::unordered_map<TLetterType, size_t> map;

        for (auto const& pair : rules) {
          if (pair.first.empty() || pair.second.empty()) {
            _empty_word = true;
          }
          _map.clear();
          plus_letters_in_word(pair.first);
          if (!_empty_word && _map.size() == 1) {
            _unique.insert(pair.first[0]);
          }
          minus_letters_in_word(pair.second);
          if (!_empty_word && !pair.second.empty()
              && std::all_of(pair.second.cbegin() + 1,
                             pair.second.cend(),
                             [&pair](TLetterType i) -> bool {
                               return i == pair.second[0];
                             })) {
            _unique.insert(pair.second[0]);
          }
          for (auto const& x : _map) {
            if (x.second != 0) {
              _preserve.insert(x.first);
            }
          }
        }
      }

      bool result() const {
        return (!_empty_word && _unique.size() != _nr_gens)
               || _preserve.size() != _nr_gens;
      }

     private:
      void letters_in_word(TWordType const& w, size_t adv) {
        for (auto const& x : w) {
          auto it = _map.find(x);
          if (it == _map.end()) {
            _map.emplace(x, adv);
          } else {
            it->second += adv;
          }
        }
      }

      void plus_letters_in_word(TWordType const& w) {
        letters_in_word(w, 1);
      }

      void minus_letters_in_word(TWordType const& w) {
        letters_in_word(w, -1);
      }

      // TLetterType i belongs to "preserve" if there exists a relation where
      // the number of occurrences of i is not the same on both sides of the
      // relation
      // TLetterType i belongs to "unique" if there is a relation where one side
      // consists solely of i.
      bool                                     _empty_word;
      std::unordered_map<TLetterType, int64_t> _map;
      size_t                                   _nr_gens;
      std::unordered_set<TLetterType>          _preserve;
      std::unordered_set<TLetterType>          _unique;
    };
  }  // namespace internal
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_INCLUDE_OBVINF_HPP_
