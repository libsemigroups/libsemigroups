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

// This file contains stuff for creating congruence over Semigroup objects or
// over FpSemigroup objects.

// TODO: - do nothing if there are no generating pairs

#include "internal/libsemigroups-exception.h"

#include "cong-new.h"
#include "cong-p.h"
#include "fpsemi.h"
#include "kbe.h"
#include "todd-coxeter.h"

namespace libsemigroups {
  namespace tmp {
    using ToddCoxeter      = congruence::ToddCoxeter;
    using KBP              = congruence::KBP;
    using class_index_type = CongIntf::class_index_type;

    //////////////////////////////////////////////////////////////////////////
    // Congruence - constructors - public
    //////////////////////////////////////////////////////////////////////////

    Congruence::Congruence(congruence_type type) : CongIntf(type), _race() {}

    Congruence::Congruence(congruence_type type, SemigroupBase* S)
        : Congruence(type) {
      _race.add_runner(
          new ToddCoxeter(type, S, ToddCoxeter::policy::use_relations));
      _race.add_runner(
          new ToddCoxeter(type, S, ToddCoxeter::policy::use_cayley_graph));
      set_nr_generators(S->nrgens());
      set_parent(S);
    }

    Congruence::Congruence(congruence_type type, SemigroupBase& S)
        : Congruence(type, &S) {}

    Congruence::Congruence(congruence_type type, FpSemigroup& S)
        : Congruence(type, &S) {}

    Congruence::Congruence(congruence_type type, FpSemigroup* S)
        : Congruence(type) {
        set_nr_generators(S->alphabet().size());
        // set_parent(S); currently not possible
      // FIXME revise this!
      _race.set_max_threads(POSITIVE_INFINITY);
      if (S->has_todd_coxeter()) {
        // Method 1: use only the relations used to define S and genpairs to
        // run Todd-Coxeter. This runs whether or not we have computed a data
        // structure for S.
        _race.add_runner(new ToddCoxeter(*S->todd_coxeter()));
        if (S->todd_coxeter()->has_isomorphic_non_fp_semigroup()
            || S->todd_coxeter()->finished()) {
          // Method 2: use the Cayley graph of S and genpairs to run
          // Todd-Coxeter. If the policy here is use_relations, then this is
          // the same as Method 0. Note that the isomorphic_non_fp_semigroup
          // must be finite in this case, and it must be possible for the
          // Froidure-Pin algoritm to complete in this case because
          // Todd-Coxeter did.
          _race.add_runner(
              new ToddCoxeter(type,
                              S->todd_coxeter()->isomorphic_non_fp_semigroup(),
                              ToddCoxeter::policy::use_cayley_graph));
          // Return here since we know that we can definitely complete at this
          // point.
          return;
        }
      }
      if (S->has_knuth_bendix()) {
        if (S->knuth_bendix()->has_isomorphic_non_fp_semigroup()
            && S->knuth_bendix()->isomorphic_non_fp_semigroup()->is_done()) {
          // Method 3: Note that the
          // S->knuth_bendix()->isomorphic_non_fp_semigroup() must be finite in
          // this case, because otherwise it would not return true from
          // Semigroup::is_done. This is similar to Method 2.
          _race.add_runner(
              new ToddCoxeter(type,
                              S->knuth_bendix()->isomorphic_non_fp_semigroup(),
                              ToddCoxeter::policy::use_cayley_graph));
          // Method 4: unlike with Method 2, this is not necessarily the same
          // as running Method 1, because the relations in
          // S->knuth_bendix()->isomorphic_non_fp_semigroup() are likely not the
          // same as those in S->todd_coxeter()->isomorphic_non_fp_semigroup().
          // TODO:
          // - check if the relations are really the same as those in
          //   S->todd_coxeter(), if it exists. This is probably too expensive!
          // - we could just add the relations from the rws directly (rather
          // than
          //   recreating them in the isomorphic_non_fp_semigroup, which is
          //   rather wasteful). If we do this, then this could be done outside
          //   the inner most if-statement here.
          _race.add_runner(
              new ToddCoxeter(type,
                              S->knuth_bendix()->isomorphic_non_fp_semigroup(),
                              ToddCoxeter::policy::use_relations));
        }
        // Method 5 (KBP): runs Knuth-Bendix on the original fp semigroup, and
        // then attempts to run the exhaustive pairs algorithm on that.
        _race.add_runner(new KBP(type, S->knuth_bendix()));

        // TODO add a runner for P and the rws using the rules of S *and*
        // genpairs, so that we can potentially answer "contains" questions
        // using this rws. This will fix attr.tst, but is a bit perverse. Would
        // it be better to just be able to add the rws here itself as a runner?
        // This is not currently possible since it is not a CongIntf. Or we
        // could store a RWS directly inside a Congruence, so that it could be
        // used solely for the "contains" method?

        if (type == congruence_type::TWOSIDED) {
          // Method 6 (KBFP)
          // S->knuth_bendix() must be copied because maybe we will add more
          // generating pairs, for thread-safety, and because
          // congruence::KnuthBendix deletes the congruence it wraps.
          _race.add_runner(new congruence::KnuthBendix(S->knuth_bendix()));
        }
      }
    }

    //////////////////////////////////////////////////////////////////////////
    // Runner - overridden pure virtual methods - public
    //////////////////////////////////////////////////////////////////////////

    void Congruence::run() {
      _race.winner();
    }

    bool Congruence::finished() const {
      // Must set_finished, since otherwise Runner methods won't function
      // correctly.
      for (auto runner : _race) {
        if (runner->finished()) {
          set_finished(true);
          return true;
        }
      }
      set_finished(false);
      return false;
    }

    //////////////////////////////////////////////////////////////////////////
    // CongIntf - overridden pure virtual methods - public
    //////////////////////////////////////////////////////////////////////////

    void Congruence::add_pair(word_type lhs, word_type rhs) {
      LIBSEMIGROUPS_ASSERT(!_race.empty());
      for (auto runner : _race) {
        static_cast<CongIntf*>(runner)->add_pair(lhs, rhs);
      }
    }

    word_type Congruence::class_index_to_word(class_index_type i) {
      return static_cast<CongIntf*>(_race.winner())->class_index_to_word(i);
    }

    class_index_type Congruence::word_to_class_index(word_type const& word) {
      return static_cast<CongIntf*>(_race.winner())->word_to_class_index(word);
    }

    size_t Congruence::nr_classes() {
      return static_cast<CongIntf*>(_race.winner())->nr_classes();
    }

    SemigroupBase* Congruence::quotient_semigroup() {
      LIBSEMIGROUPS_ASSERT(!_race.empty());
      return static_cast<CongIntf*>(_race.winner())->quotient_semigroup();
    }

    bool Congruence::is_quotient_obviously_infinite() {
      LIBSEMIGROUPS_ASSERT(!_race.empty());
      for (auto runner : _race) {
        if (static_cast<CongIntf*>(runner)->is_quotient_obviously_infinite()) {
          return true;
        }
      }
      return false;
    }

    //////////////////////////////////////////////////////////////////////////
    // Overridden public non-pure virtual methods from CongIntf
    //////////////////////////////////////////////////////////////////////////

    bool Congruence::contains(word_type const& lhs, word_type const& rhs) {
      if (const_contains(lhs, rhs)) {
        return true;
      }
      return static_cast<CongIntf*>(_race.winner())->contains(lhs, rhs);
    }

    bool Congruence::const_contains(word_type const& lhs,
                                    word_type const& rhs) const {
      for (auto runner : _race) {
        if (static_cast<CongIntf*>(runner)->const_contains(lhs, rhs)) {
          return true;
        }
      }
      return false;
    }

    //////////////////////////////////////////////////////////////////////////
    // Public methods
    //////////////////////////////////////////////////////////////////////////

    void Congruence::add_method(Runner* r) {
      // TODO check that it is ok to add runners
      _race.add_runner(r);
    }

    /////////////////////////////////////////////////////////////////////////
    // Overridden private pure virtual methods from CongIntf
    /////////////////////////////////////////////////////////////////////////
  }  // namespace tmp
}  // namespace libsemigroups
