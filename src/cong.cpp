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

#include "libsemigroups/cong.hpp"

#include <memory>  // for shared_ptr

#include "libsemigroups/cong-pair.hpp"  // for KnuthBendixCongruenceByPairs
#include "libsemigroups/debug.hpp"      // for LIBSEMIGROUPS_ASSERT
#include "libsemigroups/exception.hpp"  // for LIBSEMIGROUPS_EXCEPTION
#include "libsemigroups/fpsemi.hpp"     // for FpSemigroup
#include "libsemigroups/froidure-pin-base.hpp"  // for FroidurePinBase
#include "libsemigroups/kambites.hpp"           // for Kambites
#include "libsemigroups/knuth-bendix.hpp"       // for KnuthBendix
#include "libsemigroups/todd-coxeter.hpp"       // for ToddCoxeter

namespace libsemigroups {

  using ToddCoxeter      = congruence::ToddCoxeter;
  using KnuthBendix      = congruence::KnuthBendix;
  using Kambites         = congruence::Kambites;
  using class_index_type = CongruenceInterface::class_index_type;
  using options          = ToddCoxeter::options;

  //////////////////////////////////////////////////////////////////////////
  // Congruence - constructors - public
  //////////////////////////////////////////////////////////////////////////

  Congruence::Congruence(congruence_kind type, options::runners p)
      : CongruenceInterface(type), _race() {
    if (p == options::runners::standard) {
      _race.add_runner(std::make_shared<ToddCoxeter>(type));
      if (type == congruence_kind::twosided) {
        _race.add_runner(std::make_shared<KnuthBendix>());
      }
    }
  }

  Congruence::Congruence(congruence_kind                  type,
                         std::shared_ptr<FroidurePinBase> S)
      : Congruence(type, options::runners::none) {
    auto tc = std::make_shared<ToddCoxeter>(type, S);
    tc->froidure_pin_policy(
        congruence::ToddCoxeter::options::froidure_pin::use_relations);
    _race.add_runner(tc);

    tc = std::make_shared<ToddCoxeter>(type, S);
    tc->froidure_pin_policy(
        congruence::ToddCoxeter::options::froidure_pin::use_cayley_graph);
    _race.add_runner(tc);
    set_number_of_generators(S->number_of_generators());
    set_parent_froidure_pin(S);
  }

  Congruence::Congruence(congruence_kind type, FpSemigroup& S)
      : Congruence(type, options::runners::none) {
    set_number_of_generators(S.alphabet().size());
    LIBSEMIGROUPS_ASSERT(!has_parent_froidure_pin());
    set_parent_froidure_pin(S);
    if (type == congruence_kind::twosided && S.has_kambites()) {
      _race.add_runner(std::make_shared<Kambites>(*S.kambites()));
    }
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
            congruence::ToddCoxeter::options::froidure_pin::use_cayley_graph));

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
              congruence::ToddCoxeter::options::froidure_pin::
                  use_cayley_graph));

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

      if (type == congruence_kind::twosided) {
        // Method 6 (KBFP)
        // S.knuth_bendix() must be copied because maybe we will add more
        // generating pairs.
        _race.add_runner(
            std::make_shared<congruence::KnuthBendix>(*S.knuth_bendix()));
      }

      // Method 7 (KBP)
      // This can return non-trivial classes when other methods cannot, for
      // example, when there are no generating pairs.
      KnuthBendixCongruenceByPairs* kbp
          = new KnuthBendixCongruenceByPairs(kind(), S.knuth_bendix());
      _race.add_runner(std::shared_ptr<KnuthBendixCongruenceByPairs>(kbp));
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
          "cannot determine the word corresponding to class index %llu",
          static_cast<uint64_t>(i));
    }
    return static_cast<CongruenceInterface*>(_race.winner().get())
        ->class_index_to_word(i);
  }

  size_t Congruence::number_of_classes_impl() {
    run();  // to ensure the state is correctly set.
    if (_race.winner() == nullptr) {
      // The next line is not testable, this is just a sanity check in case
      // all threads in the _race throw, and so there is no winner.
      LIBSEMIGROUPS_EXCEPTION("cannot determine the number of classes");
    }
    return static_cast<CongruenceInterface*>(_race.winner().get())
        ->number_of_classes();
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
                              detail::to_string(word).c_str());
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
    try {
      return winner->non_trivial_classes();
    } catch (LibsemigroupsException const& e) {
      // Special case don't currently know a better way of doing this
      if (has_parent_fpsemigroup()) {
        // The following static_pointer_cast is probably a bad idea, but at
        // present it is only possible to construct a Congruence over an
        // FpSemigroup, and so it is currently guaranteed that
        // parent_fpsemigroup is a shared_ptr<FpSemigroup>.
        auto ptr = std::static_pointer_cast<FpSemigroup>(parent_fpsemigroup());
        if (ptr->has_knuth_bendix()) {
          KnuthBendixCongruenceByPairs kbp(kind(), ptr->knuth_bendix());
          for (auto it = cbegin_generating_pairs();
               it != cend_generating_pairs();
               ++it) {
            kbp.add_pair(it->first, it->second);
          }
          return kbp.non_trivial_classes();
        }
      }
      throw e;
    }
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

  void Congruence::set_number_of_generators_impl(size_t n) {
    for (auto runner : _race) {
      static_cast<CongruenceInterface*>(runner.get())
          ->set_number_of_generators(n);
    }
  }
}  // namespace libsemigroups
