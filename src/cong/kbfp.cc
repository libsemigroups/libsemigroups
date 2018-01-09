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

// Note that the KBFP run method does not meaningfully use the steps argument at
// the moment.  This is because the only goal_func currently used is to test
// pair membership or ordering, questions which are answered without running
// Froidure-Pin at all.  Hence run is only ever called with steps set to
// LIMIT_MAX.  If other goal_funcs are added later, run might use steps and we
// could increase test coverage.  This only applies here in KBFP, and not to
// other DATA subclasses.

#include <vector>

#include "../rwse.h"
#include "kbfp.h"

namespace libsemigroups {

  void Congruence::KBFP::init() {
    if (_semigroup != nullptr) {
      return;
    }
    _cong.init_relations(_cong._semigroup, _killed);
    _rws->add_rules(_cong.relations());
    _rws->add_rules(_cong.extra());

    LIBSEMIGROUPS_ASSERT(_cong._semigroup == nullptr || !_cong.extra().empty());

    REPORT("running Knuth-Bendix . . .")
    _rws->knuth_bendix(_killed);
    if (_killed) {
      REPORT("killed");
      return;
    }

    LIBSEMIGROUPS_ASSERT(_rws->confluent());
    std::vector<Element*> gens;
    for (size_t i = 0; i < _cong._nrgens; i++) {
      gens.push_back(new RWSE(*_rws, i));
    }
    _semigroup = new Semigroup<>(gens);
    really_delete_cont(gens);
  }

  void Congruence::KBFP::run() {
    while (!_killed && !is_done()) {
      run(Congruence::LIMIT_MAX);
    }
  }

  void Congruence::KBFP::run(size_t steps) {
    LIBSEMIGROUPS_ASSERT(!is_done());

    init();

    if (!_killed) {
      REPORT("running Froidure-Pin . . .")
      // This if statement will never be entered - see top of file for details
      if (steps != Congruence::LIMIT_MAX) {
        // The default batch_size is too large and can take a long time, but if
        // we are running Congruence::LIMIT_MAX steps, then the usual batch
        // size is ok.
        _semigroup->set_batch_size(steps);
      }
      _semigroup->enumerate(_killed, _semigroup->current_size() + 1);
    }
    if (_killed) {
      REPORT("killed")
    }
  }

  Congruence::class_index_t
  Congruence::KBFP::word_to_class_index(word_t const& word) {
    LIBSEMIGROUPS_ASSERT(is_done());  // so that _semigroup != nullptr

    Element* x   = new RWSE(*_rws, word);
    size_t   pos = _semigroup->position(x);
    x->really_delete();
    delete x;
    LIBSEMIGROUPS_ASSERT(pos != Semigroup<>::UNDEFINED);
    return pos;
  }

  Congruence::DATA::result_t
  Congruence::KBFP::current_equals(word_t const& w1, word_t const& w2) {
    init();
    if (!is_done() && is_killed()) {
      // This cannot be reliably tested: see TC::current_equals for more info
      return result_t::UNKNOWN;
    }
    return (_rws->test_equals(w1, w2) ? result_t::TRUE : result_t::FALSE);
  }

  Congruence::DATA::result_t
  Congruence::KBFP::current_less_than(word_t const& w1, word_t const& w2) {
    init();
    if (!is_done() && is_killed()) {
      // This cannot be reliably tested: see TC::current_equals for more info
      return result_t::UNKNOWN;
    }
    return (_rws->test_less_than(w1, w2) ? result_t::TRUE : result_t::FALSE);
  }
}  // namespace libsemigroups
