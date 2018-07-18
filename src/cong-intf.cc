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

#include "cong-intf.h"

namespace libsemigroups {
  namespace congruence {

    Interface::congruence_t Interface::type() const {
      return _type;
    }

    bool Interface::contains(word_t const& w1, word_t const& w2) {
      return word_to_class_index(w1) == word_to_class_index(w2);
    }

    bool Interface::const_contains(word_t const& u, word_t const& v) const {
      return (const_word_to_class_index(u) != UNDEFINED
              && const_word_to_class_index(u) == const_word_to_class_index(v));
    }

    bool Interface::less(word_t const& w1, word_t const& w2) {
      return word_to_class_index(w1) < word_to_class_index(w2);
    }

    void Interface::add_pair(std::initializer_list<size_t> l,
                             std::initializer_list<size_t> r) {
      add_pair(word_t(l), word_t(r));
    }

    bool Interface::is_quotient_obviously_infinite() const {
      return false;
    }
  }  // namespace congruence
}  // namespace libsemigroups
