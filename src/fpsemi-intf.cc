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

// This file contains an interface for f.p. semigroup like classes. These are
// implementations of non-pure methods that call pure methods only.

#include "fpsemi-intf.h"

namespace libsemigroups {
  namespace fpsemigroup {
    std::string const Interface::NO_ALPHABET = "";

    void Interface::add_relation(std::initializer_list<size_t> l,
                                 std::initializer_list<size_t> r) {
      add_relation(word_t(l), word_t(r));
    }

    void Interface::add_relations(SemigroupBase* S) {
      S->enumerate();

      std::vector<size_t> relation;  // a triple
      S->reset_next_relation();
      S->next_relation(relation);

      while (relation.size() == 2 && !relation.empty()) {
        // This is for the case when there are duplicate gens
        // We use the internal_add_relations, so as not to delete the
        // SemigroupBase*.
        internal_add_relation(word_t({relation[0]}), word_t({relation[1]}));
        S->next_relation(relation);
        // We could remove the duplicate generators, and update any relation
        // that contains a removed generator but this would be more
        // complicated
      }
      word_t lhs, rhs;  // changed in-place by factorisation
      while (!relation.empty()) {
        S->factorisation(lhs, relation[0]);
        S->factorisation(rhs, relation[2]);
        lhs.push_back(relation[1]);
        // We use the internal_add_relations, so as not to delete the
        // SemigroupBase*.
        internal_add_relation(lhs, rhs);
        S->next_relation(relation);
      }
    }

    void Interface::add_relations(std::vector<relation_t> const& rels) {
      for (auto rel : rels) {
        add_relation(rel);
      }
    }

    void Interface::add_relation(relation_t rel) {
      add_relation(rel.first, rel.second);
    }

    void Interface::add_relation(std::pair<std::string, std::string> rel) {
      add_relation(rel.first, rel.second);
    }

    bool Interface::validate_relation(relation_t const& r) const {
      return validate_relation(r.first, r.second);
    }

    bool Interface::validate_relation(word_t const& l, word_t const& r) const {
      return validate_word(l) && validate_word(r);
    }

    bool Interface::validate_relation(
        std::pair<std::string, std::string> const& p) const {
      return validate_relation(p.first, p.second);
    }

    bool Interface::validate_relation(std::string const& l,
                                      std::string const& r) const {
      return validate_word(l) && validate_word(r);
    }

    bool Interface::equal_to(std::initializer_list<size_t> const& u,
                             std::initializer_list<size_t> const& v) {
      return equal_to(word_t(u), word_t(v));
    }

    word_t Interface::normal_form(std::initializer_list<size_t> const& w) {
      return normal_form(word_t(w));
    }
  }  // namespace fpsemigroup
}  // namespace libsemigroups
