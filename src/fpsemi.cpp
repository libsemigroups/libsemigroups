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

// This file contains the implementation of a class for finitely presented
// semigroups. The basic idea is that this object holds a number of methods (in
// its Race _race member) and runs them competitively in multiple threads
// (maybe) to determine the structure of the fp semigroup. As such every method
// delegates to the corresponding method in every fpsemigroup::FpSemiBase*
// object in the _race.

#include "fpsemi.hpp"

#include <typeinfo>

#include "cong-base.hpp"

namespace libsemigroups {

  using ToddCoxeter = fpsemigroup::ToddCoxeter;
  using KnuthBendix = fpsemigroup::KnuthBendix;

  //////////////////////////////////////////////////////////////////////////
  // FpSemigroup - constructors - public
  //////////////////////////////////////////////////////////////////////////

  FpSemigroup::FpSemigroup(FpSemigroup::policy p) : FpSemiBase(), _race() {
    switch (p) {
      case standard: {
        _race.add_runner(new ToddCoxeter());
        _race.add_runner(new KnuthBendix());
        break;
      }
      case none: {
        // Do nothing, runners must be installed using add_method.
        break;
      }
    }
  }

  // FIXME This is wasteful since it calls add_rule(rel.first,
  // rel.second), and then make_pair(rel.first, rel.second), in
  // Todd-Coxeter for example

  // Don't take FpSemigroup::policy as an argument here since we must have a
  // place to cache the SemigroupBase* S.
  FpSemigroup::FpSemigroup(SemigroupBase* S) : FpSemigroup() {
    for (auto runner : _race) {
      static_cast<FpSemiBase*>(runner)->set_isomorphic_non_fp_semigroup(S,
                                                                        false);
      // TODO if the policy is standard, then add another ToddCoxeter with
      // policy use_cayley_graph
    }
  }

  ////////////////////////////////////////////////////////////////////////
  // Runner - overridden pure virtual methods - public
  ////////////////////////////////////////////////////////////////////////

  void FpSemigroup::run() {
    _race.winner();
  }

  //////////////////////////////////////////////////////////////////////////
  // FpSemiBase - overridden pure virtual methods - public
  //////////////////////////////////////////////////////////////////////////

  void FpSemigroup::add_rule(std::string const& lhs, std::string const& rhs) {
    LIBSEMIGROUPS_ASSERT(!_race.empty());
    for (auto runner : _race) {
      static_cast<FpSemiBase*>(runner)->add_rule(lhs, rhs);
    }
  }

  bool FpSemigroup::equal_to(std::string const& u, std::string const& v) {
    return static_cast<FpSemiBase*>(_race.winner())->equal_to(u, v);
  }

  bool FpSemigroup::is_obviously_finite() {
    LIBSEMIGROUPS_ASSERT(!_race.empty());
    for (auto it = _race.begin(); it < _race.end(); ++it) {
      if (static_cast<FpSemiBase*>(*it)->is_obviously_finite()) {
        return true;
      }
    }
    return false;
  }

  bool FpSemigroup::is_obviously_infinite() {
    LIBSEMIGROUPS_ASSERT(!_race.empty());
    for (auto it = _race.begin(); it < _race.end(); ++it) {
      if (static_cast<FpSemiBase*>(*it)->is_obviously_infinite()) {
        return true;
      }
    }
    return false;
  }

  SemigroupBase* FpSemigroup::isomorphic_non_fp_semigroup() {
    if (has_isomorphic_non_fp_semigroup()) {
      return get_isomorphic_non_fp_semigroup();
    }
    LIBSEMIGROUPS_ASSERT(!_race.empty());
    // This loop is here in case one of the Runners in _race was created using
    // a non-f.p. semigroup, so we can just return that and not run the _race.
    for (auto runner : _race) {
      if (static_cast<FpSemiBase*>(runner)->has_isomorphic_non_fp_semigroup()) {
        set_isomorphic_non_fp_semigroup(
            static_cast<FpSemiBase*>(runner)->isomorphic_non_fp_semigroup(),
            false);
        return get_isomorphic_non_fp_semigroup();
      }
    }
    set_isomorphic_non_fp_semigroup(
        static_cast<FpSemiBase*>(_race.winner())->isomorphic_non_fp_semigroup(),
        false);
    return get_isomorphic_non_fp_semigroup();
  }

  std::string FpSemigroup::normal_form(std::string const& w) {
    return static_cast<FpSemiBase*>(_race.winner())->normal_form(w);
  }

  size_t FpSemigroup::nr_rules() const noexcept {
    if (_race.empty()) {
      return 0;
    } else {
      // FIXME this is bad
      return static_cast<FpSemiBase*>(*_race.begin())->nr_rules();
    }
  }

  size_t FpSemigroup::size() {
    LIBSEMIGROUPS_ASSERT(!_race.empty());
    if (is_obviously_infinite()) {
      return POSITIVE_INFINITY;
    } else {
      return static_cast<FpSemiBase*>(_race.winner())->size();
    }
  }

  //////////////////////////////////////////////////////////////////////////////
  // FpSemiBase - non-pure virtual methods - public
  //////////////////////////////////////////////////////////////////////////////

  void FpSemigroup::set_alphabet(std::string const& lphbt) {
    FpSemiBase::set_alphabet(lphbt);
    LIBSEMIGROUPS_ASSERT(!_race.empty());
    // TODO Don't allow setting the alphabet after a certain point
    for (auto runner : _race) {
      static_cast<FpSemiBase*>(runner)->set_alphabet(lphbt);
    }
  }

  void FpSemigroup::set_alphabet(size_t n) {
    FpSemiBase::set_alphabet(n);
    LIBSEMIGROUPS_ASSERT(!_race.empty());
    // TODO Don't allow setting the alphabet after a certain point
    for (auto runner : _race) {
      static_cast<FpSemiBase*>(runner)->set_alphabet(n);
    }
  }

  //////////////////////////////////////////////////////////////////////////
  // FpSemigroup - methods - public
  //////////////////////////////////////////////////////////////////////////

  fpsemigroup::KnuthBendix* FpSemigroup::knuth_bendix() const {
    return find_method<KnuthBendix>();
  }

  bool FpSemigroup::has_knuth_bendix() const {
    try {
      knuth_bendix();
    } catch (...) {
      return false;
    }
    return true;
  }

  fpsemigroup::ToddCoxeter* FpSemigroup::todd_coxeter() const {
    return find_method<ToddCoxeter>();
  }

  bool FpSemigroup::has_todd_coxeter() const {
    try {
      todd_coxeter();
    } catch (...) {
      return false;
    }
    return true;
  }

  //////////////////////////////////////////////////////////////////////////
  // FpSemigroup - methods - private
  //////////////////////////////////////////////////////////////////////////

  template <class TFpSemiBaseSubclass>
  TFpSemiBaseSubclass* FpSemigroup::find_method() const {
    // We use find_if so that this works even if we haven't computed anything
    // at all.
    auto it = std::find_if(_race.begin(), _race.end(), [](Runner* m) {
      return typeid(*m) == typeid(TFpSemiBaseSubclass);
    });
    if (it != _race.end()) {
      return static_cast<TFpSemiBaseSubclass*>(*it);
    } else {
      throw LIBSEMIGROUPS_EXCEPTION("method not found");
    }
  }

  /*  // Private
    void FpSemigroup::set_isomorphic_non_fp_semigroup(SemigroupBase* S) {
      for (auto runner : _race) {
        static_cast<FpSemiBase*>(runner)->set_isomorphic_non_fp_semigroup(S);
      }
    }*/

}  // namespace libsemigroups
