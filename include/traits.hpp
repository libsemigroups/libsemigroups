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

// This file contains the Traits class that defines the types, and
// methods for translating between then, for use by the FroidurePin,
// SchreierSims, and P classes.

#ifndef LIBSEMIGROUPS_INCLUDE_TRAITS_HPP_
#define LIBSEMIGROUPS_INCLUDE_TRAITS_HPP_

#include <stddef.h>  // for size_t

#include "stl.hpp"  // for internal::equal_to, internal::hash

namespace libsemigroups {
  template <typename TElementType, typename = void>
  struct Traits {
    using element_type       = TElementType;
    using const_element_type = TElementType const;
    using reference          = TElementType&;
    using const_reference    = TElementType const&;
    using pointer            = TElementType*;        // no internal equivalent
    using const_pointer      = TElementType const*;  // no internal equivalent

    using internal_element_type       = element_type;
    using internal_const_element_type = const_element_type;
    using internal_reference          = reference;
    using internal_const_reference    = const_reference;

    inline internal_const_reference to_internal_const(const_reference x) const {
      return x;
    }

    inline internal_reference to_internal(reference x) const {
      return x;
    }

    inline const_reference to_external_const(internal_const_reference x) const {
      return x;
    }

    inline reference to_external(internal_reference x) const {
      return x;
    }

    inline internal_element_type
    internal_copy(internal_const_reference x) const {
      return x;
    }

    inline element_type external_copy(const_reference x) const {
      return x;
    }

    inline void internal_free(internal_element_type) const {}
    inline void external_free(element_type) const {}
  };

  template <typename TElementType,
            typename TElementEqual = internal::equal_to<TElementType>>
  class TraitsEqual : public Traits<TElementType> {
   private:
    using base = Traits<TElementType>;

   public:
    using element_type       = typename base::element_type;
    using const_element_type = typename base::const_element_type;
    using reference          = typename base::reference;
    using const_reference    = typename base::const_reference;
    using pointer            = typename base::pointer;
    using const_pointer      = typename base::const_pointer;

    using internal_element_type = typename base::internal_element_type;
    using internal_const_element_type =
        typename base::internal_const_element_type;
    using internal_reference       = typename base::internal_reference;
    using internal_const_reference = typename base::internal_const_reference;

    //! Provides a call operator for comparing elements of \c this
    //!
    //! This struct provides a call operator for comparing two elements of \c
    //! this, provided as internal_const_element_types.
    struct internal_equal_to : base {
      bool operator()(internal_const_element_type x,
                      internal_const_element_type y) const {
        return TElementEqual()(this->to_external_const(x),
                               this->to_external_const(y));
      }
    };
  };

  template <typename TElementType,
            typename TElementHash  = internal::hash<TElementType>,
            typename TElementEqual = internal::equal_to<TElementType>>
  class TraitsHashEqual : public TraitsEqual<TElementType, TElementEqual> {
   private:
    using base = Traits<TElementType>;

   public:
    using element_type       = typename base::element_type;
    using const_element_type = typename base::const_element_type;
    using reference          = typename base::reference;
    using const_reference    = typename base::const_reference;
    using pointer            = typename base::pointer;
    using const_pointer      = typename base::const_pointer;

    using internal_element_type = typename base::internal_element_type;
    using internal_const_element_type =
        typename base::internal_const_element_type;
    using internal_reference       = typename base::internal_reference;
    using internal_const_reference = typename base::internal_const_reference;

    //! Provides a call operator for hashing elements of \c this
    //!
    //! This struct provides a call operator for hashing an element of \c this,
    //! provided as an internal_const_element_type.
    struct internal_hash : base {
      size_t operator()(internal_const_element_type x) const {
        return TElementHash()(this->to_external_const(x));
      }
    };
  };

  template <class T>
  struct is_small : std::integral_constant<bool, (sizeof(T) <= 16)> {};

  template <typename TValueType, typename = void>
  struct new_traits {};

  template <typename TValueType>
      struct new_traits<
          TValueType,
          typename std::enable_if<!std::is_trivial<TValueType>::value
                                  || !is_small<TValueType>::value>::type
      > {
    using value_type       = TValueType;
    using const_value_type = TValueType const;
    using reference        = TValueType&;
    using const_reference  = TValueType const&;
    using pointer          = TValueType*;
    using const_pointer    = TValueType const*;

    using internal_value_type       = pointer;
    using internal_const_value_type = const_pointer;
    using internal_reference        = internal_value_type&;
    using internal_const_reference  = internal_const_value_type const&;

    inline internal_const_value_type to_internal_const(const_reference x) const {
      return &x;
    }

    inline internal_value_type to_internal(reference x) const {
      return &x;
    }

    inline const_reference to_external_const(internal_const_reference x) const {
      return *x;
    }

    inline reference to_external(internal_reference x) const {
      return *x;
    }

    inline internal_value_type internal_copy(internal_const_reference x) const {
      return new value_type(*x);
    }

    inline value_type external_copy(const_reference x) const {
      return x;
    }

    inline void internal_free(internal_value_type x) const {
      delete x;
    }
    inline void external_free(value_type) const {}
  };

  template <typename TValueType>
  struct new_traits<
      TValueType,
      typename std::enable_if<std::is_trivial<TValueType>::value
                              && is_small<TValueType>::value>::type> {
    using value_type       = TValueType;
    using const_value_type = TValueType const;
    using reference        = TValueType&;
    using const_reference  = TValueType const&;
    using pointer          = TValueType*;
    using const_pointer    = TValueType const* const;

    using internal_value_type       = value_type;
    using internal_const_value_type = const_value_type;
    using internal_reference        = reference;
    using internal_const_reference  = const_reference;

    inline internal_const_reference to_internal_const(const_reference x) const {
      return x;
    }

    inline internal_reference to_internal(reference x) const {
      return x;
    }

    inline const_reference to_external_const(internal_const_reference x) const {
      return x;
    }

    inline reference to_external(internal_reference x) const {
      return x;
    }

    inline internal_value_type internal_copy(internal_const_reference x) const {
      return internal_value_type(x);
    }

    inline value_type external_copy(const_reference x) const {
      return value_type(x);
    }
    inline void internal_free(internal_value_type) const {}
    inline void external_free(value_type) const {}
  };

  template <typename TValueType,
            typename TValueEqual = internal::equal_to<TValueType>>
  class new_traits_equal : public new_traits<TValueType> {
   private:
    using base = new_traits<TValueType>;

   public:
    using value_type       = typename base::value_type;
    using const_value_type = typename base::const_value_type;
    using reference          = typename base::reference;
    using const_reference    = typename base::const_reference;
    using pointer            = typename base::pointer;
    using const_pointer      = typename base::const_pointer;

    using internal_value_type = typename base::internal_value_type;
    using internal_const_value_type =
        typename base::internal_const_value_type;
    using internal_reference       = typename base::internal_reference;
    using internal_const_reference = typename base::internal_const_reference;

    //! Provides a call operator for comparing values of \c this
    //!
    //! This struct provides a call operator for comparing two values of \c
    //! this, provided as internal_const_value_types.
    struct internal_equal_to : base {
      bool operator()(internal_const_value_type x,
                      internal_const_value_type y) const {
        return TValueEqual()(this->to_external_const(x),
                               this->to_external_const(y));
      }
    };
  };

  template <typename TValueType,
            typename TValueHash  = internal::hash<TValueType>,
            typename TValueEqual = internal::equal_to<TValueType>>
  class new_traits_hash_equal
      : public new_traits_equal<TValueType, TValueEqual> {
   private:
    using base = new_traits<TValueType>;

   public:
    using value_type       = typename base::value_type;
    using const_value_type = typename base::const_value_type;
    using reference          = typename base::reference;
    using const_reference    = typename base::const_reference;
    using pointer            = typename base::pointer;
    using const_pointer      = typename base::const_pointer;

    using internal_value_type = typename base::internal_value_type;
    using internal_const_value_type =
        typename base::internal_const_value_type;
    using internal_reference       = typename base::internal_reference;
    using internal_const_reference = typename base::internal_const_reference;

    //! Provides a call operator for hashing values of \c this
    //!
    //! This struct provides a call operator for hashing an value of \c this,
    //! provided as an internal_const_value_type.
    struct internal_hash : base {
      size_t operator()(internal_const_value_type x) const {
        return TValueHash()(this->to_external_const(x));
      }
    };
  };
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_INCLUDE_TRAITS_HPP_
