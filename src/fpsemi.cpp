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

#include <iostream>  // for string, operator<<

#include "internal/libsemigroups-debug.hpp"      // for LIBSEMIGROUPS_ASSERT
#include "internal/libsemigroups-exception.hpp"  // for LIBSEMIGROUPS_EXCEPTION

#include "froidure-pin-base.hpp"  // for FroidurePinBase
#include "knuth-bendix.hpp"       // for KnuthBendix

// FIXME This is wasteful since it calls add_rule(rel.first,
// rel.second), and then make_pair(rel.first, rel.second), in
// Todd-Coxeter for example
// JDM: I don't know where this comment came from...

namespace libsemigroups {
  class Runner;  // Forward declaration

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

  // Don't take FpSemigroup::policy as an argument here since we must have a
  // place to cache the FroidurePinBase* S.
  FpSemigroup::FpSemigroup(FroidurePinBase& S) : FpSemiBase(), _race() {
    set_alphabet(S.nr_generators());
    _race.add_runner(new ToddCoxeter(S));
    _race.add_runner(new KnuthBendix(S));
    // TODO(1) if the policy is standard, then add another ToddCoxeter with
    // policy use_cayley_graph (can't currently pass the policy to the
    // fpsemigroup::ToddCoxeter).
  }


  ////////////////////////////////////////////////////////////////////////
  // Runner - pure virtual methods - public
  ////////////////////////////////////////////////////////////////////////

  void FpSemigroup::run() {
    _race.winner();
  }

  //////////////////////////////////////////////////////////////////////////
  // FpSemiBase - pure virtual methods - public
  //////////////////////////////////////////////////////////////////////////

  bool FpSemigroup::equal_to(std::string const& u, std::string const& v) {
    return static_cast<FpSemiBase*>(_race.winner())->equal_to(u, v);
  }

  bool FpSemigroup::is_obviously_finite() {
    if (_race.empty()) {
      if (alphabet().empty()) {
        // TODO(now) is this consistent with CongBase?
        throw LIBSEMIGROUPS_EXCEPTION(
            "no alphabet or rules have been specified");
      } else {
        // Nothing in _race means no rules
        return true;
      }
    }
    for (auto it = _race.begin(); it < _race.end(); ++it) {
      if (static_cast<FpSemiBase*>(*it)->is_obviously_finite()) {
        return true;
      }
    }
    return false;
  }

  bool FpSemigroup::is_obviously_infinite() {
    if (_race.empty()) {
      if (!alphabet().empty()) {
        // Nothing in _race means no rules
        return true;
      } else {
        // TODO(now) is this consistent with CongBase?
        throw LIBSEMIGROUPS_EXCEPTION(
            "no alphabet or rules have been specified");
      }
    }
    for (auto it = _race.begin(); it < _race.end(); ++it) {
      if (static_cast<FpSemiBase*>(*it)->is_obviously_infinite()) {
        return true;
      }
    }
    return false;
  }

  std::string FpSemigroup::normal_form(std::string const& w) {
    return static_cast<FpSemiBase*>(_race.winner())->normal_form(w);
  }

  size_t FpSemigroup::size() {
    if (_race.empty()) {
      throw LIBSEMIGROUPS_EXCEPTION(
          "no methods defined, cannot find size with no methods");
    } else if (is_obviously_infinite()) {
      return POSITIVE_INFINITY;
    } else {
      return static_cast<FpSemiBase*>(_race.winner())->size();
    }
  }

  //////////////////////////////////////////////////////////////////////////
  // FpSemigroup - non-virtual methods - public
  //////////////////////////////////////////////////////////////////////////

  fpsemigroup::KnuthBendix& FpSemigroup::knuth_bendix() const {
    return *find_method<KnuthBendix>();
  }

  bool FpSemigroup::has_knuth_bendix() const {
    try {
      knuth_bendix();
    } catch (...) {
      return false;
    }
    return true;
  }

  fpsemigroup::ToddCoxeter& FpSemigroup::todd_coxeter() const {
    return *find_method<ToddCoxeter>();
  }

  bool FpSemigroup::has_todd_coxeter() const {
    try {
      todd_coxeter();
    } catch (...) {
      return false;
    }
    return true;
  }

  //////////////////////////////////////////////////////////////////////////////
  // FpSemiBase - pure virtual methods - private
  //////////////////////////////////////////////////////////////////////////////

  void FpSemigroup::add_rule_impl(std::string const& u, std::string const& v) {
    for (auto runner : _race) {
      static_cast<FpSemiBase*>(runner)->add_rule(u, v);
    }
  }

  internal::owned_ptr<FroidurePinBase>
  FpSemigroup::isomorphic_non_fp_semigroup_impl() {
    if (_race.empty()) {
      throw LIBSEMIGROUPS_EXCEPTION(
          "no methods defined, cannot find an isomorphic non-fp semigroup");
    }
    // This loop is here in case one of the Runners in _race was created using
    // a non-f.p. semigroup, so we can just return that and not run the _race.
    for (auto rnnr : _race) {
      auto fpsmbs = static_cast<FpSemiBase*>(rnnr);
      if (fpsmbs->has_isomorphic_non_fp_semigroup()) {
        return internal::owned_ptr<FroidurePinBase>(
            &(fpsmbs)->isomorphic_non_fp_semigroup());
      }
    }
    return internal::owned_ptr<FroidurePinBase>(
        &(static_cast<FpSemiBase*>(_race.winner())
              ->isomorphic_non_fp_semigroup()));
  }

  //////////////////////////////////////////////////////////////////////////////
  // FpSemiBase - non-pure virtual methods - private
  //////////////////////////////////////////////////////////////////////////////

  void FpSemigroup::set_alphabet_impl(std::string const& lphbt) {
    for (auto runner : _race) {
      static_cast<FpSemiBase*>(runner)->set_alphabet(lphbt);
    }
  }

  void FpSemigroup::set_alphabet_impl(size_t n) {
    for (auto runner : _race) {
      static_cast<FpSemiBase*>(runner)->set_alphabet(n);
    }
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
}  // namespace libsemigroups
