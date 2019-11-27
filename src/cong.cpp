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

// This file contains stuff for creating congruence over FroidurePinBase objects
// or over FpSemigroup objects.

#include "cong.hpp"

#include <memory>  // for shared_ptr

#include "cong-pair.hpp"                // for KnuthBendixCongruenceByPairs
#include "fpsemi.hpp"                   // for FpSemigroup
#include "froidure-pin-base.hpp"        // for FroidurePinBase
#include "knuth-bendix.hpp"             // for KnuthBendix
#include "libsemigroups-debug.hpp"      // for LIBSEMIGROUPS_ASSERT
#include "libsemigroups-exception.hpp"  // for LIBSEMIGROUPS_EXCEPTION
#include "todd-coxeter.hpp"             // for ToddCoxeter

namespace libsemigroups {
  using ToddCoxeter      = congruence::ToddCoxeter;
  using KnuthBendix      = congruence::KnuthBendix;
  using class_index_type = CongruenceInterface::class_index_type;
  using policy           = ToddCoxeter::policy;

  //////////////////////////////////////////////////////////////////////////
  // Congruence - constructors - public
  //////////////////////////////////////////////////////////////////////////

  Congruence::Congruence(congruence_type type, policy::runners p)
      : CongruenceInterface(type), _race() {
    if (p == policy::runners::standard) {
      _race.add_runner(std::make_shared<ToddCoxeter>(type));
      if (type == congruence_type::twosided) {
        _race.add_runner(std::make_shared<KnuthBendix>());
      }
    }
  }

  Congruence::Congruence(congruence_type                  type,
                         std::shared_ptr<FroidurePinBase> S)
      : Congruence(type, policy::runners::none) {
    auto tc = std::make_shared<ToddCoxeter>(type, S);
    tc->froidure_pin_policy(
        congruence::ToddCoxeter::policy::froidure_pin::use_relations);
    _race.add_runner(tc);

    tc = std::make_shared<ToddCoxeter>(type, S);
    tc->froidure_pin_policy(
        congruence::ToddCoxeter::policy::froidure_pin::use_cayley_graph);
    _race.add_runner(tc);
    set_nr_generators(S->nr_generators());
    set_parent_froidure_pin(S);
  }

  Congruence::Congruence(congruence_type type, FpSemigroup& S)
      : Congruence(type, policy::runners::none) {
    set_nr_generators(S.alphabet().size());
    LIBSEMIGROUPS_ASSERT(!has_parent_froidure_pin());
    set_parent_froidure_pin(S);
    _race.max_threads(POSITIVE_INFINITY);
    if (S.has_todd_coxeter()) {
      // Method 1: use only the relations used to define S and genpairs to
      // run Todd-Coxeter. This runs whether or not we have computed a data
      // structure for S.
      // TODO(later) change the next line to S, instead of S.todd_coxeter!
      _race.add_runner(std::make_shared<ToddCoxeter>(type, *S.todd_coxeter()));

      if (S.todd_coxeter()->finished()) {
        // Method 2: use the Cayley graph of S and genpairs to run
        // Todd-Coxeter. If the policy here is use_relations, then this is
        // the same as Method 1. Note that the froidure_pin
        // must be finite in this case, and it must be possible for the
        // Froidure-Pin algoritm to complete in this case because
        // Todd-Coxeter did.
        _race.add_runner(std::make_shared<ToddCoxeter>(
            type,
            S.todd_coxeter()->froidure_pin(),
            congruence::ToddCoxeter::policy::froidure_pin::use_cayley_graph));

        // Return here since we know that we can definitely complete at
        // this point.
        return;
      }
    }
    if (S.has_knuth_bendix()) {
      if (S.knuth_bendix()->finished()) {
        // TODO(later) remove the if-condition, make it so that if the
        // ToddCoxeter's below are killed then so too is the enumeration of
        // S.knuth_bendix().froidure_pin()
        if (S.knuth_bendix()->froidure_pin()->finished()) {
          // Method 3: Note that the
          // S.knuth_bendix().froidure_pin() must be finite
          // in this case, because otherwise it would not return true from
          // FroidurePin::finished. This is similar to Method 2.
          _race.add_runner(std::make_shared<ToddCoxeter>(
              type,
              S.knuth_bendix()->froidure_pin(),
              congruence::ToddCoxeter::policy::froidure_pin::use_cayley_graph));

          // Method 4: unlike with Method 2, this is not necessarily the same
          // as running Method 1, because the relations in S.knuth_bendix()
          // are likely not the same as those in S.todd_coxeter().
          // TODO(later)
          // - uncomment the next line
          // - check if the relations are really the same as those in
          //   S.todd_coxeter(), if it exists. This is probably too
          //   expensive!
          // - use the active rules of KnuthBendix in the ToddCoxeter
          // constructor, currently uses the relations in S.knuth_bendix()
          // which are guaranteed to equal those in S.todd_coxeter()!
          // _race.add_runner(new ToddCoxeter(type, S.knuth_bendix()));

          // Return here since we know that we can definitely complete at this
          // point.
          return;
        }
      }

      // Method 5 (KnuthBendixCongruenceByPairs): runs Knuth-Bendix on the
      // original fp semigroup, and then attempts to run the exhaustive pairs
      // algorithm on that. Yes, this method sucks, but there are examples where
      // this is useful.
      _race.add_runner(std::make_shared<KnuthBendixCongruenceByPairs>(
          type, S.knuth_bendix()));

      if (type == congruence_type::twosided) {
        // Method 6 (KBFP)
        // S.knuth_bendix() must be copied because maybe we will add more
        // generating pairs.
        _race.add_runner(
            std::make_shared<congruence::KnuthBendix>(*S.knuth_bendix()));
      }
    }
    LIBSEMIGROUPS_ASSERT(!_race.empty());
  }

  ////////////////////////////////////////////////////////////////////////////
  // CongruenceInterface - non-pure virtual member functions - public
  ////////////////////////////////////////////////////////////////////////////

  bool Congruence::contains(word_type const& lhs, word_type const& rhs) {
    tril r = const_contains(lhs, rhs);
    if (r != tril::unknown) {
      return r == tril::TRUE;
    }
    run_until([this, &lhs, &rhs]() -> bool {
      return const_contains(lhs, rhs) != tril::unknown;
    });
    return const_contains(lhs, rhs) == tril::TRUE;
  }

  tril Congruence::const_contains(word_type const& lhs,
                                  word_type const& rhs) const {
    validate_word(lhs);
    validate_word(rhs);
    if (lhs == rhs) {
      return tril::TRUE;
    }
    for (auto runner : _race) {
      tril r = static_cast<CongruenceInterface*>(runner.get())
                   ->const_contains(lhs, rhs);
      if (r != tril::unknown) {
        return r;
      }
    }
    return tril::unknown;
  }

  //////////////////////////////////////////////////////////////////////////
  // CongruenceInterface - pure virtual member functions - private
  //////////////////////////////////////////////////////////////////////////

  word_type Congruence::class_index_to_word_impl(class_index_type i) {
    // TODO(later) it ought to be possible to answer this question without
    // getting the winner(). Since, for example, if we can run KnuthBendix,
    // then start enumerating its FroidurePin, then we can find the word of any
    // (sufficiently small) class index without any of the runners in the race
    // winning.
    if (_race.winner() == nullptr) {
      LIBSEMIGROUPS_EXCEPTION(
          "cannot determine the word corresponding to class index %llu", i);
    }
    return static_cast<CongruenceInterface*>(_race.winner().get())
        ->class_index_to_word(i);
  }

  size_t Congruence::nr_classes_impl() {
    run();  // to ensure the state is correctly set.
    if (_race.winner() == nullptr) {
      // The next line is not testable, this is just a sanity check in case
      // all threads in the _race throw, and so there is no winner.
      LIBSEMIGROUPS_EXCEPTION("cannot determine the number of classes");
    }
    return static_cast<CongruenceInterface*>(_race.winner().get())
        ->nr_classes();
  }

  std::shared_ptr<FroidurePinBase> Congruence::quotient_impl() {
    if (_race.winner() == nullptr) {
      // The next line is not testable, this is just a sanity check in case
      // all threads in the _race throw, and so there is no winner.
      LIBSEMIGROUPS_EXCEPTION("cannot determine the quotient FroidurePin");
    }
    return static_cast<CongruenceInterface*>(_race.winner().get())
        ->quotient_froidure_pin();
  }

  class_index_type Congruence::word_to_class_index_impl(word_type const& word) {
    // TODO(later) it ought to be possible to answer this question without
    // getting the winner(). Since, for example, if we can run KnuthBendix,
    // then start enumerating its FroidurePin, then we can find the class index
    // of any (sufficiently small) word without any of the runners in the race
    // winning.
    if (_race.winner() == nullptr) {
      LIBSEMIGROUPS_EXCEPTION("cannot determine the class index of word %s",
                              detail::to_string(word));
    }
    LIBSEMIGROUPS_ASSERT(static_cast<CongruenceInterface*>(_race.winner().get())
                             ->word_to_class_index(word)
                         != UNDEFINED);
    return static_cast<CongruenceInterface*>(_race.winner().get())
        ->word_to_class_index(word);
  }

  //////////////////////////////////////////////////////////////////////////
  // CongruenceInterface - non-pure virtual member functions - private
  //////////////////////////////////////////////////////////////////////////

  void Congruence::add_pair_impl(word_type const& u, word_type const& v) {
    for (auto runner : _race) {
      static_cast<CongruenceInterface*>(runner.get())->add_pair(u, v);
    }
  }

  std::shared_ptr<CongruenceInterface::non_trivial_classes_type const>
  Congruence::non_trivial_classes_impl() {
    run();  // required so that state is correctly set.
    auto winner = static_cast<CongruenceInterface*>(_race.winner().get());
    if (winner == nullptr) {
      // The next line is not testable, this is just a sanity check in case
      // all threads in the _race throw, and so there is no winner.
      LIBSEMIGROUPS_EXCEPTION("cannot determine the non-trivial classes!");
    }
    return winner->non_trivial_classes();
  }

  bool Congruence::is_quotient_obviously_infinite_impl() {
    for (auto runner : _race) {
      if (static_cast<CongruenceInterface*>(runner.get())
              ->is_quotient_obviously_infinite()) {
        return true;
      }
    }
    return false;
  }

  bool Congruence::is_quotient_obviously_finite_impl() {
    for (auto runner : _race) {
      if (static_cast<CongruenceInterface*>(runner.get())
              ->is_quotient_obviously_finite()) {
        return true;
      }
    }
    return false;  // Returns false if _race is empty
  }

  void Congruence::set_nr_generators_impl(size_t const n) {
    for (auto runner : _race) {
      static_cast<CongruenceInterface*>(runner.get())->set_nr_generators(n);
    }
  }
}  // namespace libsemigroups
