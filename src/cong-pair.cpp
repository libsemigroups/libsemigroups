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

#include "cong-pair.hpp"

namespace libsemigroups {
  namespace congruence {
    ////////////////////////////////////////////////////////////////////////
    // KBP - constructors - public
    ////////////////////////////////////////////////////////////////////////

    KBP::KBP(congruence_type type, fpsemigroup::KnuthBendix* kb)
        : p_type(type), _kb(kb) {
      set_nr_generators(_kb->alphabet().size());
    }

    // For testing purposes only really
    KBP::KBP(congruence_type type, fpsemigroup::KnuthBendix& kb)
        : KBP(type, &kb){};

    ////////////////////////////////////////////////////////////////////////
    // P - overridden virtual methods - public
    ////////////////////////////////////////////////////////////////////////

    void KBP::run() {
      if (stopped()) {
        return;
      }
      _kb->run_until([this]() -> bool { return dead() || timed_out(); });
      if (!stopped()) {
        set_parent(_kb->isomorphic_non_fp_semigroup());
        p_type::run();
      }
      report_why_we_stopped();
    }

    // Override the method for the class P to avoid having to know the parent
    // semigroup (found as part of KBP::run) to add a pair.
    // TODO(later) this copies KBE(_kb, l) and KBE(_kb, r) twice.
    void KBP::add_pair(word_type const& l, word_type const& r) {
      internal_element_type x = new element_type(_kb, l);
      internal_element_type y = new element_type(_kb, r);
      internal_add_pair(x, y);
      this->internal_free(x);
      this->internal_free(y);
      set_finished(false);
    }
  }  // namespace congruence
}  // namespace libsemigroups
