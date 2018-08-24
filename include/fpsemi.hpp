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

// This file contains the declaration of a class for finitely presented
// semigroups.

#ifndef LIBSEMIGROUPS_INCLUDE_FPSEMI_HPP_
#define LIBSEMIGROUPS_INCLUDE_FPSEMI_HPP_

#include <stddef.h>  // for size_t

#include <string>  // for string

#include "internal/race.hpp"  // for Race

#include "fpsemi-base.hpp"   // for FpSemiBase
#include "todd-coxeter.hpp"  // for ToddCoxeter

namespace libsemigroups {
  class FroidurePinBase;  // Forward declaration
  namespace fpsemigroup {
    class KnuthBendix;  // Forward declaration
  }

  // This is a class for defining fp semigroups.
  class FpSemigroup : public FpSemiBase {
   public:
    // Execution policy:
    // - standard: means run 1 variant of everything
    // - none:     means no methods are added, and at least one must be added
    //             manually via add_method
    // TODO enum class
    enum policy { standard = 0, none = 1 };

    //////////////////////////////////////////////////////////////////////////
    // FpSemigroup - constructors - public
    //////////////////////////////////////////////////////////////////////////

    explicit FpSemigroup(FpSemigroup::policy = standard);
    explicit FpSemigroup(FroidurePinBase*);
    explicit FpSemigroup(FroidurePinBase&);

    ////////////////////////////////////////////////////////////////////////
    // Runner - overridden pure virtual methods - public
    ////////////////////////////////////////////////////////////////////////

    void run() override;
    // TODO finished method

    //////////////////////////////////////////////////////////////////////////
    // FpSemiBase - overridden pure virtual methods - public
    //////////////////////////////////////////////////////////////////////////

    void add_rule(std::string const&, std::string const&) override;
    using FpSemiBase::add_rule;

    bool             equal_to(std::string const&, std::string const&) override;
    bool             is_obviously_finite() override;
    bool             is_obviously_infinite() override;
    FroidurePinBase* isomorphic_non_fp_semigroup() override;
    std::string      normal_form(std::string const&) override;
    size_t           nr_rules() const noexcept override;
    size_t           size() override;

    using FpSemiBase::equal_to;
    using FpSemiBase::normal_form;

    //////////////////////////////////////////////////////////////////////////
    // FpSemigroup - methods - public
    //////////////////////////////////////////////////////////////////////////

    bool                      has_knuth_bendix() const;
    bool                      has_todd_coxeter() const;
    fpsemigroup::KnuthBendix* knuth_bendix() const;
    fpsemigroup::ToddCoxeter* todd_coxeter() const;

    // TODO further methods:
    // - add_method

   private:
    //////////////////////////////////////////////////////////////////////////////
    // FpSemiBase - non-pure virtual methods - private
    //////////////////////////////////////////////////////////////////////////////

    void set_alphabet_impl(std::string const&) override;
    void set_alphabet_impl(size_t) override;

    //////////////////////////////////////////////////////////////////////////
    // FpSemigroup - methods - private
    //////////////////////////////////////////////////////////////////////////

    template <class TFpSemigroupInterfaceSubclass>
    TFpSemigroupInterfaceSubclass* find_method() const;

    //////////////////////////////////////////////////////////////////////////
    // FpSemigroup - data - private
    //////////////////////////////////////////////////////////////////////////

    Race _race;
  };
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_INCLUDE_FPSEMI_HPP_
