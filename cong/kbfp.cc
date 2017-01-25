//
// Semigroups++ - C/C++ library for computing with semigroups and monoids
// Copyright (C) 2017 James D. Mitchell
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

// This file contains implementations for the private inner class of Congruence
// called KBFP, which is a subclass of Congruence::DATA.  This class is for
// performing Knuth-Bendix followed by the Froidure-Pin algorithm on the
// quotient.

#include <vector>

#include "../rwse.h"
#include "kbfp.h"

namespace libsemigroups {

  template <typename T> static inline void really_delete_cont(T cont) {
    for (Element* x : cont) {
      x->really_delete();
      delete x;
    }
  }

  void Congruence::KBFP::run() {
    // Initialise the rewriting system
    _cong.init_relations(_cong._semigroup, _killed);
    _rws->add_rules(_cong.relations());
    _rws->add_rules(_cong.extra());

    assert(_cong._semigroup == nullptr || !_cong.extra().empty());

    REPORT("running Knuth-Bendix . . .")

    _rws->knuth_bendix(_killed);
    if (!_killed) {
      assert(_rws->is_confluent());
      std::vector<Element*> gens;
      for (size_t i = 0; i < _cong._nrgens; i++) {
        gens.push_back(new RWSE(*_rws, i));
      }
      _semigroup = new Semigroup(gens);
      really_delete_cont(gens);

      REPORT("running Froidure-Pin . . .")

      _semigroup->enumerate(_killed, Semigroup::LIMIT_MAX);
    }
    if (_killed) {
      REPORT("killed")
    }
  }

  Congruence::class_index_t
  Congruence::KBFP::word_to_class_index(word_t const& word) {
    if (!is_done()) {
      run();
    }
    assert(is_done());  // so that _semigroup != nullptr

    Element* x   = new RWSE(*_rws, word);
    size_t   pos = _semigroup->position(x);
    x->really_delete();
    delete x;
    assert(pos != Semigroup::UNDEFINED);
    return pos;
  }

  Congruence::partition_t Congruence::KBFP::nontrivial_classes() {
    assert(is_done());
    partition_t classes;

    if (_cong._semigroup == nullptr) {
      // Assert appropriate for JDM's long comment in cong.cc
      assert(_cong._relations.empty() || _cong._extra.empty());
      if (_cong._extra.empty()) {
        return classes;  // trivial congruence - no nontrivial classes
      }
      // nontrivial congruence on free semigroup - answer is infinite
      assert(!_cong._relations.empty());  // TODO: fail gracefully?
    }

    word_t word;
    // Note: we assume classes are numbered contiguously {0 .. n-1}
    std::vector<std::vector<size_t>> pos_classes(nr_classes(),
                                                 std::vector<size_t>());
    // Look up the class number of each element of the parent semigroup
    for (size_t pos = 0; pos < _cong._semigroup->size(); pos++) {
      _cong._semigroup->factorisation(word, pos);
      assert(word_to_class_index(word) < nr_classes());
      pos_classes[word_to_class_index(word)].push_back(pos);
    }

    // Look up these element positions and store the Element pointers
    size_t next_nontrivial_class = 0;
    assert(pos_classes.size() == nr_classes());
    for (size_t class_nr = 0; class_nr < nr_classes(); class_nr++) {
      // Use only the classes with at least 2 elements
      if (pos_classes[class_nr].size() > 1) {
        classes.push_back(std::vector<Element const*>());
        for (size_t pos : pos_classes[class_nr]) {
          assert(pos < _cong._semigroup->size());
          // Push each element into classes
          classes[next_nontrivial_class].push_back(
              _cong._semigroup->at(pos)->really_copy());
        }
        next_nontrivial_class++;
      }
    }

    return classes;
  }

}  // namespace libsemigroups
