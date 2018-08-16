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

#include "semigroup-base.h"

namespace libsemigroups {
  void relations(SemigroupBase*                              S,
                 std::function<void(word_type, word_type)>&& hook) {
    S->enumerate();

    std::vector<size_t> relation;  // a triple
    S->reset_next_relation();
    S->next_relation(relation);

    while (relation.size() == 2 && !relation.empty()) {
      hook(word_type({relation[0]}), word_type({relation[1]}));
      S->next_relation(relation);
    }
    word_type lhs, rhs;  // changed in-place by factorisation
    while (!relation.empty()) {
      S->factorisation(lhs, relation[0]);
      S->factorisation(rhs, relation[2]);
      lhs.push_back(relation[1]);
      hook(lhs, rhs);
      S->next_relation(relation);
    }
  }
}  // namespace libsemigroups
