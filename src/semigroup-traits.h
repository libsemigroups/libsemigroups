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

// This file contains the SemigroupTraits class that defines the types, and
// methods for translating between then, for use by the Semigroup, StabChain,
// and P classes.

#ifndef LIBSEMIGROUPS_SRC_SEMIGROUP_TRAITS_H_
#define LIBSEMIGROUPS_SRC_SEMIGROUP_TRAITS_H_

#include <type_traits>

#include "functional.h"

namespace libsemigroups {
  template <typename TElementType, typename = void> struct SemigroupTraits {
    using value_type       = TElementType;
    using const_value_type = TElementType const;
    using reference        = TElementType&;
    using const_reference  = TElementType const&;

    using internal_value_type       = value_type;
    using internal_const_value_type = const_value_type;
    using internal_reference        = reference;
    using internal_const_reference  = const_reference;

    inline internal_const_reference to_internal(const_reference x) const {
      return x;
    }

    inline const_reference to_external(internal_const_reference x) const {
      return x;
    }

    inline internal_value_type internal_copy(internal_const_reference x) const {
      return x;
    }

    inline value_type external_copy(const_reference x) const {
      return x;
    }

    inline void internal_free(internal_value_type) const {}
    inline void external_free(value_type) const {}
  };

  template <typename TElementType,
            typename TElementEqual = libsemigroups::equal_to<TElementType>>
  class SemigroupTraitsEqual : public SemigroupTraits<TElementType> {
   private:
    using base = SemigroupTraits<TElementType>;

   public:
    using value_type       = typename base::value_type;
    using const_value_type = typename base::const_value_type;
    using reference        = typename base::reference;
    using const_reference  = typename base::const_reference;

    using internal_value_type       = typename base::internal_value_type;
    using internal_const_value_type = typename base::internal_const_value_type;
    using internal_reference        = typename base::internal_reference;
    using internal_const_reference  = typename base::internal_const_reference;

    //! Provides a call operator for comparing elements of \c this
    //!
    //! This struct provides a call operator for comparing two elements of \c
    //! this, provided as internal_const_value_types.
    struct internal_equal_to : base {
      bool operator()(internal_const_value_type x,
                      internal_const_value_type y) const {
        return TElementEqual()(this->to_external(x), this->to_external(y));
      }
    };
  };

  template <typename TElementType,
            typename TElementHash  = libsemigroups::hash<TElementType>,
            typename TElementEqual = libsemigroups::equal_to<TElementType>>
  class SemigroupTraitsHashEqual
      : public SemigroupTraitsEqual<TElementType, TElementEqual> {
   private:
    using base = SemigroupTraits<TElementType>;

   public:
    using value_type       = typename base::value_type;
    using const_value_type = typename base::const_value_type;
    using reference        = typename base::reference;
    using const_reference  = typename base::const_reference;

    using internal_value_type       = typename base::internal_value_type;
    using internal_const_value_type = typename base::internal_const_value_type;
    using internal_reference        = typename base::internal_reference;
    using internal_const_reference  = typename base::internal_const_reference;

    //! Provides a call operator for hashing elements of \c this
    //!
    //! This struct provides a call operator for hashing an element of \c this,
    //! provided as an internal_const_value_type.
    struct internal_hash : base {
      size_t operator()(internal_const_value_type x) const {
        return TElementHash{}(this->to_external(x));
      }
    };
  };

}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_SRC_SEMIGROUP_TRAITS_H_
