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
    std::string const FpSemiIntf::NO_ALPHABET = "";

    void FpSemiIntf::add_relation(std::initializer_list<size_t> l,
                                 std::initializer_list<size_t> r) {
      add_relation(word_type(l), word_type(r));
    }

    void FpSemiIntf::add_relations(SemigroupBase* S) {
      S->enumerate();

      std::vector<size_t> relation;  // a triple
      S->reset_next_relation();
      S->next_relation(relation);

      while (relation.size() == 2 && !relation.empty()) {
        // This is for the case when there are duplicate gens
        // We use the internal_add_relations, so as not to delete the
        // SemigroupBase*.
        internal_add_relation(word_type({relation[0]}), word_type({relation[1]}));
        S->next_relation(relation);
        // We could remove the duplicate generators, and update any relation
        // that contains a removed generator but this would be more
        // complicated
      }
      word_type lhs, rhs;  // changed in-place by factorisation
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

    void FpSemiIntf::add_relations(std::vector<relation_type> const& rels) {
      for (auto rel : rels) {
        add_relation(rel);
      }
    }

    void FpSemiIntf::add_relation(relation_type rel) {
      add_relation(rel.first, rel.second);
    }

    void FpSemiIntf::add_relation(std::pair<std::string, std::string> rel) {
      add_relation(rel.first, rel.second);
    }

    bool FpSemiIntf::validate_relation(relation_type const& r) const {
      return validate_relation(r.first, r.second);
    }

    bool FpSemiIntf::validate_relation(word_type const& l, word_type const& r) const {
      return validate_word(l) && validate_word(r);
    }

    bool FpSemiIntf::validate_relation(
        std::pair<std::string, std::string> const& p) const {
      return validate_relation(p.first, p.second);
    }

    bool FpSemiIntf::validate_relation(std::string const& l,
                                      std::string const& r) const {
      return validate_word(l) && validate_word(r);
    }

    bool FpSemiIntf::equal_to(std::initializer_list<size_t> const& u,
                             std::initializer_list<size_t> const& v) {
      return equal_to(word_type(u), word_type(v));
    }

    word_type FpSemiIntf::normal_form(std::initializer_list<size_t> const& w) {
      return normal_form(word_type(w));
    }
}  // namespace libsemigroups
