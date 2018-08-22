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

// This file contains stuff for creating congruence over FroidurePin objects or
// over FpSemigroup objects.

#ifndef LIBSEMIGROUPS_INCLUDE_CONG_HPP_
#define LIBSEMIGROUPS_INCLUDE_CONG_HPP_

#include <stddef.h>  // for size_t

#include "internal/race.hpp"    // for Race
#include "internal/runner.hpp"  // for Runner

#include "cong-base.hpp"  // for congruence_type, CongBase::class_index_type
#include "types.hpp"      // for word_type

namespace libsemigroups {
  class FpSemigroup;      // Forward declaration
  class FroidurePinBase;  // Forward declaration, constructor parameters
  namespace congruence {
    class KnuthBendix;
    class ToddCoxeter;
  }  // namespace congruence

  class Congruence : public CongBase {
   public:
    // Execution policy:
    // - standard: means run 1 variant of everything
    // - none:     means no methods are added, and at least one must be added
    //             manually via add_method
    enum class policy { standard = 0, none = 1 };

    //////////////////////////////////////////////////////////////////////////
    // Congruence - constructors - public
    //////////////////////////////////////////////////////////////////////////

    explicit Congruence(congruence_type type);

    Congruence(congruence_type type,
               FroidurePinBase*,
               policy = policy::standard);
    Congruence(congruence_type type,
               FroidurePinBase&,
               policy = policy::standard);

    Congruence(congruence_type type, FpSemigroup&, policy = policy::standard);
    Congruence(congruence_type type, FpSemigroup*, policy = policy::standard);

    //////////////////////////////////////////////////////////////////////////
    // Runner - overridden pure virtual methods - public
    //////////////////////////////////////////////////////////////////////////

    void run() override;

    //////////////////////////////////////////////////////////////////////////
    // Runner - overridden non-pure virtual methods - protected
    //////////////////////////////////////////////////////////////////////////

    bool finished_impl() const override;

    //////////////////////////////////////////////////////////////////////////
    // CongBase - overridden pure virtual methods - public
    //////////////////////////////////////////////////////////////////////////

    void             add_pair(word_type const&, word_type const&) override;
    word_type        class_index_to_word(class_index_type) override;
    FroidurePinBase* quotient_semigroup() override;
    size_t           nr_classes() override;
    class_index_type word_to_class_index(word_type const&) override;

    //////////////////////////////////////////////////////////////////////////
    // CongBase - non-pure virtual methods - public
    //////////////////////////////////////////////////////////////////////////

    bool        contains(word_type const&, word_type const&) override;
    result_type const_contains(word_type const&,
                               word_type const&) const override;
    bool        is_quotient_obviously_finite() override;
    bool        is_quotient_obviously_infinite() override;

    //////////////////////////////////////////////////////////////////////////
    // Congruence - methods - public
    //////////////////////////////////////////////////////////////////////////

    void add_method(Runner*);

    bool                     has_knuth_bendix() const;
    bool                     has_todd_coxeter() const;
    congruence::KnuthBendix* knuth_bendix() const;
    congruence::ToddCoxeter* todd_coxeter() const;

   private:
    //////////////////////////////////////////////////////////////////////////
    // Congruence - methods - private
    //////////////////////////////////////////////////////////////////////////

    template <class TCongBaseSubclass>
    TCongBaseSubclass* find_method() const;

    //////////////////////////////////////////////////////////////////////////
    // CongBase - non-pure virtual methods - private
    //////////////////////////////////////////////////////////////////////////

    void init_non_trivial_classes() override;

    /////////////////////////////////////////////////////////////////////////
    // Congruence - data - private
    /////////////////////////////////////////////////////////////////////////

    Race _race;
  };
}  // namespace libsemigroups

#endif  // LIBSEMIGROUPS_INCLUDE_CONG_HPP_
