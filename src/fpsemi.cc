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
// delegates to the corresponding method in every fpsemigroup::Interface* object
// in the _race.

#include "fpsemi.h"

#include <typeinfo>

#include "cong-intf.h"

namespace libsemigroups {

  using congruence_t = CongIntf::congruence_t;
  using Interface    = fpsemigroup::Interface;
  using ToddCoxeter  = fpsemigroup::ToddCoxeter;
  using RWS          = fpsemigroup::RWS;

  //////////////////////////////////////////////////////////////////////////
  // Constructors
  //////////////////////////////////////////////////////////////////////////

  FpSemigroup::FpSemigroup(FpSemigroup::policy p)
      : Interface(), _race(INFTY) {
    switch (p) {
      case standard: {
        _race.add_runner(new ToddCoxeter());
        _race.add_runner(new RWS());
        break;
      }
      case none: {
        // Do nothing, runners must be installed using add_method.
        break;
      }
    }
  }

  FpSemigroup::FpSemigroup(size_t nrgens, FpSemigroup::policy p)
      : FpSemigroup(p) {
    for (auto runner : _race) {
      static_cast<Interface*>(runner)->set_nr_generators(nrgens);
    }
  }

  FpSemigroup::FpSemigroup(size_t                         nrgens,
                           std::vector<relation_t> const& relations,
                           FpSemigroup::policy            p)
      : FpSemigroup(nrgens, p) {
    for (auto const& rel : relations) {
      // FIXME This is wasteful since it calls add_relation(rel.first,
      // rel.second), and then make_pair(rel.first, rel.second), in
      // Todd-Coxeter for example
      add_relation(rel);
    }
  }

  // Don't take FpSemigroup::policy as an argument here since we must have a
  // place to cache the SemigroupBase* S.
  FpSemigroup::FpSemigroup(SemigroupBase* S) : FpSemigroup(S->nrgens()) {
    for (auto runner : _race) {
      static_cast<Interface*>(runner)
          ->set_isomorphic_non_fp_semigroup(S);
      // TODO if the policy is standard, then add another ToddCoxeter with
      // policy use_cayley_graph
    }
  }

  //////////////////////////////////////////////////////////////////////////
  // Overridden virtual methods from Interface
  //////////////////////////////////////////////////////////////////////////

  void FpSemigroup::set_nr_generators(size_t m) {
    LIBSEMIGROUPS_ASSERT(!_race.empty());
    // TODO Don't allow setting the number of generators after a certain point
    for (auto runner : _race) {
      static_cast<Interface*>(runner)->set_nr_generators(m);
    }
  }

  size_t FpSemigroup::nr_generators() const {
    LIBSEMIGROUPS_ASSERT(!_race.empty());
    return static_cast<Interface*>(*_race.begin())->nr_generators();
  }

  void FpSemigroup::set_alphabet(std::string alpha) {
    LIBSEMIGROUPS_ASSERT(!_race.empty());
    // TODO Don't allow setting the alphabet after a certain point
    for (auto runner : _race) {
      static_cast<Interface*>(runner)->set_alphabet(alpha);
    }
  }

  std::string const& FpSemigroup::alphabet() const {
    LIBSEMIGROUPS_ASSERT(!_race.empty());
    return static_cast<Interface*>(*_race.begin())->alphabet();
  }

  // Private
  bool FpSemigroup::validate_word(word_t const& w) const {
    LIBSEMIGROUPS_ASSERT(!_race.empty());
    return static_cast<Interface*>(*_race.begin())
        ->validate_word(w);
  }

  // Private
  bool FpSemigroup::validate_word(std::string const& w) const {
    LIBSEMIGROUPS_ASSERT(!_race.empty());
    return static_cast<Interface*>(*_race.begin())
        ->validate_word(w);
  }

  // Private
  void FpSemigroup::set_isomorphic_non_fp_semigroup(SemigroupBase* S) {
    for (auto runner : _race) {
      static_cast<Interface*>(runner)
          ->set_isomorphic_non_fp_semigroup(S);
    }
  }

  void FpSemigroup::add_relation(word_t lhs, word_t rhs) {
    LIBSEMIGROUPS_ASSERT(!_race.empty());
    for (auto runner : _race) {
      static_cast<Interface*>(runner)->add_relation(lhs, rhs);
    }
  }

  void FpSemigroup::add_relation(std::string lhs, std::string rhs) {
    LIBSEMIGROUPS_ASSERT(!_race.empty());
    // TODO Don't allow adding new relations after a certain point
    for (auto runner : _race) {
      static_cast<Interface*>(runner)->add_relation(lhs, rhs);
    }
  }

  void FpSemigroup::internal_add_relation(word_t, word_t) {
    // This does nothing here.
    // FIXME is there some way to not have it? Yes, by making FpSemigroup not
    // an Interface subclass.
    LIBSEMIGROUPS_ASSERT(false);
  }

  bool FpSemigroup::is_obviously_finite() const {
    LIBSEMIGROUPS_ASSERT(!_race.empty());
    for (auto it = _race.begin(); it < _race.end(); ++it) {
      if (static_cast<Interface*>(*it)->is_obviously_finite()) {
        return true;
      }
    }
    return false;
  }

  bool FpSemigroup::is_obviously_infinite() const {
    LIBSEMIGROUPS_ASSERT(!_race.empty());
    for (auto it = _race.begin(); it < _race.end(); ++it) {
      if (static_cast<Interface*>(*it)->is_obviously_infinite()) {
        return true;
      }
    }
    return false;
  }

  size_t FpSemigroup::size() {
    LIBSEMIGROUPS_ASSERT(!_race.empty());
    if (is_obviously_infinite()) {
      return INFTY;
    } else {
      return static_cast<Interface*>(_race.winner())->size();
    }
  }

  SemigroupBase* FpSemigroup::isomorphic_non_fp_semigroup() {
    LIBSEMIGROUPS_ASSERT(!_race.empty());
    // This loop is here in case one of the Runners in _race was created using
    // a non-f.p. semigroup, so we can just return that and not run the _race.
    for (auto runner : _race) {
      if (static_cast<Interface*>(runner)
              ->has_isomorphic_non_fp_semigroup()) {
        return static_cast<Interface*>(runner)
            ->isomorphic_non_fp_semigroup();
      }
    }
    return static_cast<Interface*>(_race.winner())
        ->isomorphic_non_fp_semigroup();
  }

  bool FpSemigroup::has_isomorphic_non_fp_semigroup() {
    LIBSEMIGROUPS_ASSERT(!_race.empty());
    // This loop is here in case one of the Runners in _race was created using
    // a non-f.p. semigroup, so we can just return that and not run the _race.
    for (auto runner : _race) {
      if (static_cast<Interface*>(runner)
              ->has_isomorphic_non_fp_semigroup()) {
        return true;
      }
    }
    return false;
  }

  bool FpSemigroup::equal_to(word_t const& u, word_t const& v) {
    return static_cast<Interface*>(_race.winner())->equal_to(u, v);
  }

  bool FpSemigroup::equal_to(std::string const& u, std::string const& v) {
    return static_cast<Interface*>(_race.winner())->equal_to(u, v);
  }

  word_t FpSemigroup::normal_form(word_t const& w) {
    return static_cast<Interface*>(_race.winner())->normal_form(w);
  }

  std::string FpSemigroup::normal_form(std::string const& w) {
    return static_cast<Interface*>(_race.winner())->normal_form(w);
  }

  //////////////////////////////////////////////////////////////////////////
  // Specific methods for FpSemigroup
  //////////////////////////////////////////////////////////////////////////

  template <class TInterfaceSubclass>
  TInterfaceSubclass* FpSemigroup::find_method() {
    LIBSEMIGROUPS_ASSERT(!_race.empty());
    // We use find_if so that this works even if we haven't computed anything
    // at all.
    auto it = std::find_if(_race.begin(), _race.end(), [](Runner* m) {
      return typeid(*m) == typeid(TInterfaceSubclass);
    });
    if (it != _race.end()) {
      return static_cast<TInterfaceSubclass*>(*it);
    } else {
      // Exception?
      return nullptr;
    }
  }

  ToddCoxeter* FpSemigroup::todd_coxeter() {
    return find_method<ToddCoxeter>();
  }

  RWS* FpSemigroup::rws() {
    return find_method<RWS>();
  }

}  // namespace libsemigroups
