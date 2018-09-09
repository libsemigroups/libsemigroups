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

// This file contains stuff for creating congruence over FroidurePinBase objects
// or over FpSemigroup objects.

// TODO: - do nothing if there are no generating pairs

#include "cong.hpp"

#include <string>       // for string
#include <type_traits>  // for enable_if<>::type
#include <vector>       // for operator==, vector

#include "cong-pair.hpp"                         // for KBP
#include "fpsemi.hpp"                            // for FpSemigroup
#include "froidure-pin-base.hpp"                 // for FroidurePinBase
#include "internal/libsemigroups-debug.hpp"      // for LIBSEMIGROUPS_ASSERT
#include "internal/libsemigroups-exception.hpp"  // for LIBSEMIGROUPS_EXCEPTION
#include "knuth-bendix.hpp"                      // for KnuthBendix
#include "owned_ptr.hpp"                         // for owned_ptr
#include "todd-coxeter.hpp"                      // for ToddCoxeter

namespace libsemigroups {
  using ToddCoxeter      = congruence::ToddCoxeter;
  using KnuthBendix      = congruence::KnuthBendix;
  using KBP              = congruence::KBP;
  using class_index_type = CongBase::class_index_type;

  //////////////////////////////////////////////////////////////////////////
  // Congruence - constructors - public
  //////////////////////////////////////////////////////////////////////////

  Congruence::Congruence(congruence_type type) : CongBase(type), _race() {}

  Congruence::Congruence(congruence_type type, FroidurePinBase& S, policy plcy)
      : Congruence(type) {
    switch (plcy) {
      case policy::standard: {
        _race.add_runner(
            new ToddCoxeter(type, S, ToddCoxeter::policy::use_relations));
        _race.add_runner(
            new ToddCoxeter(type, S, ToddCoxeter::policy::use_cayley_graph));
        break;
      }
      case policy::none: {
        // Do nothing, runners must be installed using add_method.
        break;
      }
    }
    set_nr_generators(S.nr_generators());
    set_parent_semigroup(S);
  }

  Congruence::Congruence(congruence_type type, FpSemigroup& S, policy plcy)
      : Congruence(type) {
    set_nr_generators(S.alphabet().size());
    if (plcy != policy::standard) {
      return;
    }
    _race.set_max_threads(POSITIVE_INFINITY);
    if (S.has_todd_coxeter()) {
      // Method 1: use only the relations used to define S and genpairs to
      // run Todd-Coxeter. This runs whether or not we have computed a data
      // structure for S.
      _race.add_runner(new ToddCoxeter(type, S.todd_coxeter()));

      if (S.todd_coxeter().finished()) {
        LIBSEMIGROUPS_ASSERT(!has_parent_semigroup());
        set_parent_semigroup(S.todd_coxeter().froidure_pin());

        // Method 2: use the Cayley graph of S and genpairs to run
        // Todd-Coxeter. If the policy here is use_relations, then this is
        // the same as Method 1. Note that the froidure_pin
        // must be finite in this case, and it must be possible for the
        // Froidure-Pin algoritm to complete in this case because
        // Todd-Coxeter did.
        _race.add_runner(
            new ToddCoxeter(type,
                            S.todd_coxeter().froidure_pin(),
                            ToddCoxeter::policy::use_cayley_graph));

        // Return here since we know that we can definitely complete at
        // this point.
        return;
      }
    }
    if (S.has_knuth_bendix()) {
      if (S.knuth_bendix().finished()) {
        if (!has_parent_semigroup()) {
          set_parent_semigroup(S.knuth_bendix().froidure_pin());
          // Even if the FpSemigroup S is infinite, the
          // froidure_pin() can still be useful in this case,
          // for example, when factorizing elements.
        }
        // TODO(now) remove the if-condition, make it so that if the
        // ToddCoxeter's below are killed then so too is the enumeration of
        // S.knuth_bendix().froidure_pin()
        if (S.knuth_bendix().froidure_pin().finished()) {
          // Method 3: Note that the
          // S.knuth_bendix().froidure_pin() must be finite
          // in this case, because otherwise it would not return true from
          // FroidurePin::finished. This is similar to Method 2.
          _race.add_runner(
              new ToddCoxeter(type,
                              S.knuth_bendix().froidure_pin(),
                              ToddCoxeter::policy::use_cayley_graph));

          // Method 4: unlike with Method 2, this is not necessarily the same
          // as running Method 1, because the relations in S.knuth_bendix()
          // are likely not the same as those in S.todd_coxeter().
          // TODO:
          // - check if the relations are really the same as those in
          //   S.todd_coxeter(), if it exists. This is probably too
          //   expensive!
          _race.add_runner(new ToddCoxeter(type, S.knuth_bendix()));

          // Return here since we know that we can definitely complete at this
          // point.
          return;
        }
      }

      // Method 5 (KBP): runs Knuth-Bendix on the original fp semigroup, and
      // then attempts to run the exhaustive pairs algorithm on that. Yes, this
      // method sucks, but there are examples where this is useful.
      _race.add_runner(new KBP(type, S.knuth_bendix()));

      if (type == congruence_type::TWOSIDED) {
        // Method 6 (KBFP)
        // S.knuth_bendix() must be copied because maybe we will add more
        // generating pairs.
        _race.add_runner(new congruence::KnuthBendix(&S.knuth_bendix()));
      }
    }
  }

  //////////////////////////////////////////////////////////////////////////
  // Runner - pure virtual methods - public
  //////////////////////////////////////////////////////////////////////////

  void Congruence::run() {
    // if (!has_parent_semigroup()) {
    //  set_parent_semigroup(static_cast<CongBase*>(_race.winner())->parent_semigroup());
    //}
  }

  //////////////////////////////////////////////////////////////////////////
  // Runner - non-pure virtual methods - protected
  //////////////////////////////////////////////////////////////////////////

  bool Congruence::finished_impl() const {
    for (auto runner : _race) {
      if (runner->finished()) {
        return true;
      }
    }
    return false;
  }

  //////////////////////////////////////////////////////////////////////////
  // CongBase - pure virtual methods - public
  //////////////////////////////////////////////////////////////////////////

  word_type Congruence::class_index_to_word(class_index_type i) {
    return static_cast<CongBase*>(_race.winner())->class_index_to_word(i);
  }

  class_index_type Congruence::word_to_class_index(word_type const& word) {
    LIBSEMIGROUPS_ASSERT(
        static_cast<CongBase*>(_race.winner())->word_to_class_index(word)
        != UNDEFINED);
    return static_cast<CongBase*>(_race.winner())->word_to_class_index(word);
  }

  size_t Congruence::nr_classes() {
    return static_cast<CongBase*>(_race.winner())->nr_classes();
  }

  ////////////////////////////////////////////////////////////////////////////
  // CongBase - non-pure virtual methods - public
  ////////////////////////////////////////////////////////////////////////////

  bool Congruence::contains(word_type const& lhs, word_type const& rhs) {
    result_type r = const_contains(lhs, rhs);
    if (r != result_type::UNKNOWN) {
      return r == result_type::TRUE;
    }
    _race.run_until([this, &lhs, &rhs]() -> bool {
      return const_contains(lhs, rhs) != result_type::UNKNOWN;
    });
    return const_contains(lhs, rhs) == result_type::TRUE;
  }

  result_type Congruence::const_contains(word_type const& lhs,
                                         word_type const& rhs) const {
    if (lhs == rhs) {
      return result_type::TRUE;
    }
    for (auto runner : _race) {
      result_type r = static_cast<CongBase*>(runner)->const_contains(lhs, rhs);
      if (r != result_type::UNKNOWN) {
        return r;
      }
    }
    return result_type::UNKNOWN;
  }

  //////////////////////////////////////////////////////////////////////////
  // Congruence - methods - public
  //////////////////////////////////////////////////////////////////////////

  void Congruence::add_method(Runner* r) {
    // TODO check that it is ok to add runners
    // Add any existing pairs
    for (auto it = cbegin_generating_pairs(); it < cend_generating_pairs();
         ++it) {
      static_cast<CongBase*>(r)->add_pair(it->first, it->second);
    }
    _race.add_runner(r);
  }

  KnuthBendix& Congruence::knuth_bendix() const {
    return *find_method<KnuthBendix>();
  }

  bool Congruence::has_knuth_bendix() const {
    try {
      knuth_bendix();
    } catch (...) {
      return false;
    }
    return true;
  }

  ToddCoxeter& Congruence::todd_coxeter() const {
    return *find_method<ToddCoxeter>();
  }

  bool Congruence::has_todd_coxeter() const {
    try {
      todd_coxeter();
    } catch (...) {
      return false;
    }
    return true;
  }

  //////////////////////////////////////////////////////////////////////////
  // CongBase - pure virtual methods - private
  //////////////////////////////////////////////////////////////////////////

  void Congruence::add_pair_impl(word_type const& u, word_type const& v) {
    for (auto runner : _race) {
      static_cast<CongBase*>(runner)->add_pair(u, v);
    }
  }

  internal::owned_ptr<FroidurePinBase> Congruence::quotient_impl() {
    if (_race.empty()) {
      throw LIBSEMIGROUPS_EXCEPTION(
          "no methods defined, cannot find the quotient with no methods");
    }
    return internal::owned_ptr<FroidurePinBase>(
        &(static_cast<CongBase*>(_race.winner())->quotient_semigroup()));
  }

  //////////////////////////////////////////////////////////////////////////
  // CongBase - non-pure virtual methods - private
  //////////////////////////////////////////////////////////////////////////

  std::shared_ptr<CongBase::non_trivial_classes_type>
  Congruence::non_trivial_classes_impl() {
    auto winner = static_cast<CongBase*>(_race.winner());
    winner->init_non_trivial_classes();
    return winner->_non_trivial_classes;
  }

  bool Congruence::is_quotient_obviously_infinite_impl() {
    for (auto runner : _race) {
      if (static_cast<CongBase*>(runner)->is_quotient_obviously_infinite()) {
        return true;
      }
    }
    return false;  // Returns false if _race is empty
  }

  bool Congruence::is_quotient_obviously_finite_impl() {
    for (auto runner : _race) {
      if (static_cast<CongBase*>(runner)->is_quotient_obviously_finite()) {
        return true;
      }
    }
    return false;  // Returns false if _race is empty
  }

  //////////////////////////////////////////////////////////////////////////
  // Congruence - methods - private
  //////////////////////////////////////////////////////////////////////////

  template <class TCongBaseSubclass>
  TCongBaseSubclass* Congruence::find_method() const {
    // We use find_if so that this works even if we haven't computed anything
    // at all.
    auto it = std::find_if(_race.begin(), _race.end(), [](Runner* m) {
      return typeid(*m) == typeid(TCongBaseSubclass);
    });
    if (it != _race.end()) {
      return static_cast<TCongBaseSubclass*>(*it);
    } else {
      throw LIBSEMIGROUPS_EXCEPTION("method not found");
    }
  }
}  // namespace libsemigroups
