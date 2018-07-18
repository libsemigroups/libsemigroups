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

#ifndef LIBSEMIGROUPS_SRC_FPSEMI_H_
#define LIBSEMIGROUPS_SRC_FPSEMI_H_

#include "defs.h"
#include "fpsemi-intf.h"
#include "race.h"
#include "rws.h"
#include "semigroups-base.h"
#include "todd-coxeter.h"

namespace libsemigroups {

  // This is a class for defining fp semigroups.
  class FpSemigroup : public fpsemigroup::Interface {
   public:
    // Execution policy:
    // - standard: means run 1 variant of everything
    // - none:     means no methods are added, and at least one must be added
    //             manually via add_method
    enum policy { standard = 0, none = 1 };

    //////////////////////////////////////////////////////////////////////////
    // Constructors
    //////////////////////////////////////////////////////////////////////////

    explicit FpSemigroup(FpSemigroup::policy = standard);

    explicit FpSemigroup(size_t nrgens, FpSemigroup::policy = standard);

    // TODO initializer list constructor
    FpSemigroup(size_t                         nrgens,
                std::vector<relation_t> const& relations,
                FpSemigroup::policy = standard);

    explicit FpSemigroup(SemigroupBase*);

    //////////////////////////////////////////////////////////////////////////
    // Overridden virtual methods from fpsemigroup::Interface
    //////////////////////////////////////////////////////////////////////////

    void               set_nr_generators(size_t) override;
    size_t             nr_generators() const override;
    void               set_alphabet(std::string) override;
    std::string const& alphabet() const override;

   private:
    bool validate_word(word_t const&) const override;
    bool validate_word(std::string const&) const override;
    void set_isomorphic_non_fp_semigroup(SemigroupBase*) override;
    void internal_add_relation(word_t, word_t) override;

   public:
    void add_relation(word_t, word_t) override;
    void add_relation(std::string, std::string) override;
    using fpsemigroup::Interface::add_relation;

    bool           is_obviously_finite() const override;
    bool           is_obviously_infinite() const override;
    size_t         size() override;
    SemigroupBase* isomorphic_non_fp_semigroup() override;
    bool           has_isomorphic_non_fp_semigroup() override;

    bool equal_to(word_t const&, word_t const&) override;
    bool equal_to(std::string const&, std::string const&) override;
    using fpsemigroup::Interface::equal_to;

    word_t      normal_form(word_t const&) override;
    std::string normal_form(std::string const&) override;
    using fpsemigroup::Interface::normal_form;

    //////////////////////////////////////////////////////////////////////////
    // Public methods for FpSemigroup
    //////////////////////////////////////////////////////////////////////////

    fpsemigroup::RWS*         rws();
    fpsemigroup::ToddCoxeter* todd_coxeter();

    // TODO further methods:
    // - add_method

   private:
    template <class TFpSemigroupInterfaceSubclass>
    TFpSemigroupInterfaceSubclass* find_method();

    Race _race;
  };
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_SRC_FPSEMI_H_
