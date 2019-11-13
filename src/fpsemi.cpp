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

// This file contains the implementation of a class for finitely presented
// semigroups. The basic idea is that this object holds a number of member
// functions (in its Race _race member) and runs them competitively in multiple
// threads (maybe) to determine the structure of the fp semigroup. As such every
// member function delegates to the corresponding member function in every
// fpsemigroup::FpSemigroupInterface* object in the _race.

#include "fpsemi.hpp"

#include <string>  // for string

#include "froidure-pin-base.hpp"  // for FroidurePinBase
#include "knuth-bendix.hpp"       // for KnuthBendix

namespace libsemigroups {

  using ToddCoxeter = fpsemigroup::ToddCoxeter;
  using KnuthBendix = fpsemigroup::KnuthBendix;

  //////////////////////////////////////////////////////////////////////////
  // FpSemigroup - constructors - public
  //////////////////////////////////////////////////////////////////////////

  FpSemigroup::FpSemigroup() : FpSemigroupInterface(), _race() {
    _race.add_runner(std::make_shared<ToddCoxeter>());
    _race.add_runner(std::make_shared<KnuthBendix>());
  }

  FpSemigroup::FpSemigroup(std::shared_ptr<FroidurePinBase> S)
      : FpSemigroupInterface(), _race() {
    set_alphabet(S->nr_generators());
    _race.add_runner(std::make_shared<ToddCoxeter>(S));
    _race.add_runner(std::make_shared<KnuthBendix>(S));
  }

  //////////////////////////////////////////////////////////////////////////
  // FpSemigroupInterface - pure virtual member functions - public
  //////////////////////////////////////////////////////////////////////////

  size_t FpSemigroup::size() {
    if (is_obviously_infinite()) {
      return POSITIVE_INFINITY;
    } else {
      run();  // required so that the state is correct
      return static_cast<FpSemigroupInterface*>(_race.winner().get())->size();
    }
  }

  //////////////////////////////////////////////////////////////////////////////
  // FpSemigroupInterface - pure virtual member functions - private
  //////////////////////////////////////////////////////////////////////////////

  void FpSemigroup::add_rule_impl(std::string const& u, std::string const& v) {
    for (auto runner : _race) {
      static_cast<FpSemigroupInterface*>(runner.get())->add_rule(u, v);
    }
  }

  std::shared_ptr<FroidurePinBase> FpSemigroup::froidure_pin_impl() {
    // This loop is here in case one of the Runners in _race was created using
    // a non-f.p. semigroup, so we can just return that and not run the _race.
    for (auto rnnr : _race) {
      auto ptr = static_cast<FpSemigroupInterface*>(rnnr.get());
      if (ptr->has_froidure_pin()) {
        return ptr->froidure_pin();
      }
    }
    run();  // required to that the state is correct.
    return static_cast<FpSemigroupInterface*>(_race.winner().get())
        ->froidure_pin();
  }

  bool FpSemigroup::is_obviously_infinite_impl() {
    for (auto it = _race.begin(); it < _race.end(); ++it) {
      auto ptr = static_cast<FpSemigroupInterface*>(it->get());
      if (ptr->is_obviously_infinite()) {
        return true;
      }
    }
    return false;
  }

  //////////////////////////////////////////////////////////////////////////////
  // FpSemigroupInterface - non-pure virtual member functions - private
  //////////////////////////////////////////////////////////////////////////////

  void FpSemigroup::set_alphabet_impl(std::string const& lphbt) {
    for (auto runner : _race) {
      static_cast<FpSemigroupInterface*>(runner.get())->set_alphabet(lphbt);
    }
  }

  void FpSemigroup::set_alphabet_impl(size_t n) {
    for (auto runner : _race) {
      static_cast<FpSemigroupInterface*>(runner.get())->set_alphabet(n);
    }
  }

  bool FpSemigroup::is_obviously_finite_impl() {
    for (auto it = _race.begin(); it < _race.end(); ++it) {
      auto ptr = static_cast<FpSemigroupInterface*>(it->get());
      if (ptr->is_obviously_finite()) {
        return true;
      }
    }
    return false;
  }

}  // namespace libsemigroups
