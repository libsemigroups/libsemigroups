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

#include "internal/race.h"

#include "cong-intf.h"

namespace libsemigroups {
  class FpSemigroup;  // Forward declaration
  namespace tmp { // FIXME remove this
    class Congruence : public CongIntf {
     public:
      //////////////////////////////////////////////////////////////////////////
      // Congruence - constructors - public
      //////////////////////////////////////////////////////////////////////////
      // TODO: Policy?

      explicit Congruence(congruence_type type);

      Congruence(congruence_type type, SemigroupBase* S);
      Congruence(congruence_type type, SemigroupBase& S);
      Congruence(congruence_type type, FpSemigroup& S);
      Congruence(congruence_type type, FpSemigroup* S);

      //////////////////////////////////////////////////////////////////////////
      // Runner - overridden pure virtual methods - public
      //////////////////////////////////////////////////////////////////////////

      void run() override;
      bool finished() const override;

      //////////////////////////////////////////////////////////////////////////
      // CongIntf - overridden pure virtual methods - public
      //////////////////////////////////////////////////////////////////////////

      void             add_pair(word_type, word_type) override;
      word_type        class_index_to_word(class_index_type) override;
      SemigroupBase*   quotient_semigroup() override;
      size_t           nr_classes() override;
      class_index_type word_to_class_index(word_type const&) override;

      //////////////////////////////////////////////////////////////////////////
      // CongIntf - non-pure virtual methods - public
      //////////////////////////////////////////////////////////////////////////

      bool contains(word_type const&, word_type const&) override;
      bool const_contains(word_type const&, word_type const&) const override;
      bool is_quotient_obviously_finite() override;
      bool is_quotient_obviously_infinite() override;

      //////////////////////////////////////////////////////////////////////////
      // Congruence - methods - public
      //////////////////////////////////////////////////////////////////////////

      void add_method(Runner*);

     private:
      //////////////////////////////////////////////////////////////////////////
      // CongIntf - non-pure virtual methods - private
      //////////////////////////////////////////////////////////////////////////
      // TODO use it or lose it
      // class_index_type const_word_to_class_index(word_type const&) const
      // override;
      void init_non_trivial_classes() override;

      /////////////////////////////////////////////////////////////////////////
      // Congruence - data - private
      /////////////////////////////////////////////////////////////////////////

      Race _race;
    };
  }  // namespace tmp
}  // namespace libsemigroups

#endif  // LIBSEMIGROUPS_INCLUDE_CONG_NEW_H_
