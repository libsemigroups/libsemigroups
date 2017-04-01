//
// libsemigroups - C++ library for semigroups and monoids
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
    while (!_killed && !is_done()) {
      run(UINT_MAX);
    }
  }

  void Congruence::KBFP::init() {
    if (_semigroup != nullptr) {
      return;
    }
    _cong.init_relations(_cong._semigroup, _killed);
    _rws->add_rules(_cong.relations());
    _rws->add_rules(_cong.extra());

    assert(_cong._semigroup == nullptr || !_cong.extra().empty());

    REPORT("running Knuth-Bendix . . .")
    _rws->knuth_bendix(_killed);
    if (_killed) {
      REPORT("killed");
      return;
    }

    assert(_rws->is_confluent());
    std::vector<Element*> gens;
    for (size_t i = 0; i < _cong._nrgens; i++) {
      gens.push_back(new RWSE(*_rws, i));
    }
    _semigroup = new Semigroup(gens);
    really_delete_cont(gens);
  }

  void Congruence::KBFP::run(size_t steps) {
    assert(!is_done());

    init();

    if (!_killed) {
      REPORT("running Froidure-Pin . . .")
      // The default batch_size is too large and can take a long time
      _semigroup->set_batch_size(steps);
      _semigroup->enumerate(_killed, _semigroup->current_size() + 1);
    }
    if (_killed) {
      REPORT("killed")
    }
  }

  Congruence::class_index_t
  Congruence::KBFP::word_to_class_index(word_t const& word) {
    assert(is_done());  // so that _semigroup != nullptr

    Element* x   = new RWSE(*_rws, word);
    size_t   pos = _semigroup->position(x);
    x->really_delete();
    delete x;
    assert(pos != Semigroup::UNDEFINED);
    return pos;
  }
}  // namespace libsemigroups
