//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019 James D. Mitchell
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

#include "libsemigroups/cong-pair.hpp"

namespace libsemigroups {
  ////////////////////////////////////////////////////////////////////////
  // KnuthBendixCongruenceByPairs - constructors - public
  ////////////////////////////////////////////////////////////////////////

  KnuthBendixCongruenceByPairs::KnuthBendixCongruenceByPairs(
      congruence_kind              type,
      std::shared_ptr<KnuthBendix> kb) noexcept
      : CongruenceByPairs_(type, kb, false) {  // final param is a dummy
    set_number_of_generators(kb->alphabet().size());
  }

  KnuthBendixCongruenceByPairs::KnuthBendixCongruenceByPairs(
      congruence_kind    type,
      KnuthBendix const& kb) noexcept
      : KnuthBendixCongruenceByPairs(type, std::make_shared<KnuthBendix>(kb)) {}

  ////////////////////////////////////////////////////////////////////////
  // Runner - pure virtual member functions - public
  ////////////////////////////////////////////////////////////////////////

  void KnuthBendixCongruenceByPairs::run_impl() {
    auto stppd = [this]() -> bool { return stopped(); };
    state().run_until(stppd);
    if (!stopped()) {
      if (!has_parent_froidure_pin()) {
        set_parent_froidure_pin(state().froidure_pin());
      }
      // TODO(later) should just run_until in the next line, that doesn't
      // currently work because run_until calls run (i.e. this function),
      // causing an infinite loop. We really want to call run_until on the
      // p_type, using its run member function, but that's not currently
      // possible.
      CongruenceByPairs_::run_impl();
    }
    report_why_we_stopped();
  }

  bool KnuthBendixCongruenceByPairs::finished_impl() const {
    return has_parent_froidure_pin() && CongruenceByPairs_::finished_impl();
  }

  std::shared_ptr<FroidurePinBase>
  KnuthBendixCongruenceByPairs::quotient_impl() {
    fpsemigroup::KnuthBendix kb(state());
    // The return value of state() is fpsemigroup::KnuthBendix
    for (auto it = cbegin_generating_pairs(); it != cend_generating_pairs();
         ++it) {
      kb.add_rule(it->first, it->second);
    }
    return kb.froidure_pin();
  }

  ////////////////////////////////////////////////////////////////////////
  // CongruenceInterface - virtual member functions - public
  ////////////////////////////////////////////////////////////////////////

  // Override the method for the class CongruenceByPairs to avoid having to
  // know the parent semigroup (found as part of
  // KnuthBendixCongruenceByPairs::run) to add a pair.
  // TODO(later) this copies KBE(_kb, l) and KBE(_kb, r) twice.
  void KnuthBendixCongruenceByPairs::add_pair_impl(word_type const& u,
                                                   word_type const& v) {
    internal_element_type x = new element_type(state(), u);
    internal_element_type y = new element_type(state(), v);
    internal_add_pair(x, y);
    this->internal_free(x);
    this->internal_free(y);
  }
}  // namespace libsemigroups
