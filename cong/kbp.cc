//
// libsemigroups - C++ library for computing with semigroups and monoids
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

// This file contains the declaration for the private inner class of Congruence
// called KBP, which is a subclass of Congruence::DATA.  This class is for
// performing Knuth-Bendix followed by the P inner class of Congruence on the
// quotient.

#include "kbp.h"

#include <vector>

#include "../rwse.h"

namespace libsemigroups {

  void Congruence::KBP::run() {
    // Initialise the rewriting system
    _rws->add_rules(_cong.relations());
    REPORT("running Knuth-Bendix . . .");
    _rws->knuth_bendix(_killed);

    if (!_killed) {
      assert(_rws->is_confluent());
      std::vector<Element*> gens;
      for (size_t i = 0; i < _cong._nrgens; i++) {
        gens.push_back(new RWSE(*_rws, i));
      }
      _semigroup = new Semigroup(gens);
      really_delete_cont(gens);

      _P_cong = new Congruence(_cong._type, _semigroup, _cong._extra);
      _P_cong->set_relations(_cong.relations());
      _P_cong->force_p();

      // only need _semigroup here to know the generators, and how to hash
      // things etc
      REPORT("running P . . .")
      P* p = static_cast<P*>(_P_cong->get_data());
      p->run(_killed);
    }
    if (_killed) {
      REPORT("killed")
    }
  }

  Congruence::class_index_t
  Congruence::KBP::word_to_class_index(word_t const& word) {
    assert(is_done());
    return _P_cong->word_to_class_index(word);
  }

  Partition<word_t> Congruence::KBP::nontrivial_classes() {
    assert(is_done());
    return _P_cong->nontrivial_classes();
  }

}  // namespace libsemigroups
