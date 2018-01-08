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

// This file contains the declaration for the private inner class of Congruence
// called KBP, which is a subclass of Congruence::DATA.  This class is for
// performing Knuth-Bendix followed by the P inner class of Congruence on the
// quotient.

#include "kbp.h"

#include <vector>

#include "../rwse.h"

namespace libsemigroups {

  void Congruence::KBP::init() {
    if (_semigroup != nullptr) {
      return;
    }
    LIBSEMIGROUPS_ASSERT(_P_cong == nullptr);

    // Initialise the rewriting system
    _rws->add_rules(_cong.relations());
    REPORT("running Knuth-Bendix . . .");
    _rws->knuth_bendix(_killed);

    // Setup the P cong
    if (!_killed) {
      LIBSEMIGROUPS_ASSERT(_rws->confluent());
      std::vector<Element const*> gens;
      for (size_t i = 0; i < _cong._nrgens; i++) {
        gens.push_back(new RWSE(*_rws, i));
      }
      _semigroup = new Semigroup<>(gens);
      really_delete_cont(gens);

      _P_cong = new Congruence(_cong._type, _semigroup, _cong._extra);
      _P_cong->set_relations(_cong.relations());
      _P_cong->force_p();
    }
  }

  void Congruence::KBP::run() {
    while (!_killed && !is_done()) {
      run(Congruence::LIMIT_MAX);
    }
  }

  void Congruence::KBP::run(size_t steps) {
    init();
    if (!_killed) {
      REPORT("running P . . .")
      P* p = static_cast<P*>(_P_cong->cget_data());
      LIBSEMIGROUPS_ASSERT(p != nullptr);
      p->run(steps, _killed);
    }
    if (_killed) {
      REPORT("killed")
    }
  }

  Congruence::class_index_t
  Congruence::KBP::word_to_class_index(word_t const& word) {
    LIBSEMIGROUPS_ASSERT(is_done());
    return _P_cong->word_to_class_index(word);
  }

  Congruence::DATA::result_t Congruence::KBP::current_equals(word_t const& w1,
                                                             word_t const& w2) {
    init();
    if (!is_done() && is_killed()) {
      // This cannot be reliably tested: see TC::current_equals for more info
      return result_t::UNKNOWN;
    }
    LIBSEMIGROUPS_ASSERT(_P_cong != nullptr);
    return _P_cong->cget_data()->current_equals(w1, w2);
  }

  Partition<word_t>* Congruence::KBP::nontrivial_classes() {
    LIBSEMIGROUPS_ASSERT(is_done());
    return _P_cong->nontrivial_classes();
  }

}  // namespace libsemigroups
