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

#ifndef LIBSEMIGROUPS_INCLUDE_CONG_NEW_H_
#define LIBSEMIGROUPS_INCLUDE_CONG_NEW_H_

#include "cong-intf.h"

namespace libsemigroups {

  using congruence_type = CongIntf::congruence_type;

  class Congruence : public CongIntf {
   public:
    //////////////////////////////////////////////////////////////////////////
    // Congruence - constructors
    //////////////////////////////////////////////////////////////////////////
    // TODO: Policy?

    explicit Congruence(congruence_type type);

    Congruence(congruence_type                      type,
               SemigroupBase*                    S,
               std::initializer_list<relation_type> extra);

    Congruence(congruence_type                   type,
               SemigroupBase*                 S,
               std::vector<relation_type> const& genpairs);

    // TODO uncomment these
    /*Congruence(congruence_type                   type,
               FpSemigroup&                   S,
               std::vector<relation_type> const& genpairs);

    Congruence(congruence_type                   type,
               FpSemigroup*                   S,
               std::vector<relation_type> const& genpairs);*/

    //////////////////////////////////////////////////////////////////////////
    // Overridden public pure virtual methods from CongIntf
    //////////////////////////////////////////////////////////////////////////

    class_index_type  word_to_class_index(word_type const&) override;
    size_t         nr_classes() override;
    void           add_pair(word_type, word_type) override;
    SemigroupBase* quotient_semigroup() override;

    std::vector<std::vector<word_type>>::const_iterator
    cbegin_non_trivial_classes() override;
    std::vector<std::vector<word_type>>::const_iterator
    cend_non_trivial_classes() override;

    size_t nr_non_trivial_classes() override;

    //////////////////////////////////////////////////////////////////////////
    // Overridden public non-pure virtual methods from CongIntf
    //////////////////////////////////////////////////////////////////////////

    bool contains(word_type const&, word_type const&) override;
    bool const_contains(word_type const&, word_type const&) const override;
    bool is_quotient_obviously_infinite() const override;

    //////////////////////////////////////////////////////////////////////////
    // Public methods
    //////////////////////////////////////////////////////////////////////////

    void add_method(Runner*);

   private:
    /////////////////////////////////////////////////////////////////////////
    // Overridden private pure virtual methods from CongIntf
    /////////////////////////////////////////////////////////////////////////

    class_index_type const_word_to_class_index(word_type const&) const override;

    /////////////////////////////////////////////////////////////////////////
    // Private data
    /////////////////////////////////////////////////////////////////////////

    Race _race;
  };
}  // namespace libsemigroups

#endif  // LIBSEMIGROUPS_INCLUDE_CONG_NEW_H_
