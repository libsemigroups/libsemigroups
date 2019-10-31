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

// This file contains the BruidhinnTraits class templates that defines the
// types, and functions for translating between then, for use by the
// FroidurePin, SchreierSims, CongruenceByPairs, and other classes.

#ifndef LIBSEMIGROUPS_INCLUDE_BRUIDHINN_TRAITS_HPP_
#define LIBSEMIGROUPS_INCLUDE_BRUIDHINN_TRAITS_HPP_

#include <cstddef>  // for size_t

#include "iterator.hpp"  // for ConstIteratorTraits

namespace libsemigroups {
  namespace detail {
    template <typename T>
    struct IsSmall final : std::integral_constant<bool, (sizeof(T) <= 16)> {};

    template <typename T>
    struct RemovePointerToConst final {
      using type = typename std::remove_const<typename std::remove_pointer<
          typename std::remove_const<T>::type>::type>::type;
    };

    // Bruidhinn is (apparently) the Scots gaelic word for "convey", the
    // following traits class is used to convey values from an external type to
    // an internal type, and/or vice versa.

    template <typename TValueType, typename = void>
    struct BruidhinnTraits {};

    template <typename TValueType>
    struct BruidhinnTraits<
        TValueType,
        typename std::enable_if<(!std::is_trivial<TValueType>::value
                                 || !IsSmall<TValueType>::value)
                                && !std::is_pointer<TValueType>::value>::type> {
      using value_type       = TValueType;
      using const_value_type = TValueType const;
      using reference        = TValueType&;
      using rvalue_reference = TValueType&&;
      using const_reference  = TValueType const&;
      using pointer          = TValueType*;
      using const_pointer    = TValueType const*;

      using internal_value_type       = pointer;
      using internal_const_value_type = const_pointer;
      using internal_reference        = internal_value_type&;
      using internal_const_reference  = internal_const_value_type const&;

      BruidhinnTraits() noexcept                       = default;
      BruidhinnTraits(BruidhinnTraits const&) noexcept = default;
      BruidhinnTraits(BruidhinnTraits&&) noexcept      = default;
      BruidhinnTraits& operator=(BruidhinnTraits const&) noexcept = default;
      BruidhinnTraits& operator=(BruidhinnTraits&&) noexcept = default;
      ~BruidhinnTraits()                                     = default;

      inline internal_const_value_type
      to_internal_const(const_reference x) const {
        return &x;
      }

      inline internal_value_type to_internal(reference x) const {
        return &x;
      }

      inline internal_value_type to_internal(rvalue_reference x) const {
        return new value_type(x);
      }

      inline const_reference
      to_external_const(internal_const_reference x) const {
        return *x;
      }

      inline reference to_external(internal_reference x) const {
        return *x;
      }

      inline internal_value_type
      internal_copy(internal_const_reference x) const {
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
    struct BruidhinnTraits<
        TValueType,
        typename std::enable_if<std::is_trivial<TValueType>::value
                                && IsSmall<TValueType>::value
                                && !std::is_pointer<TValueType>::value>::type> {
      using value_type       = TValueType;
      using const_value_type = TValueType const;
      using reference        = TValueType&;
      using rvalue_reference = TValueType&&;
      using const_reference  = TValueType const&;
      using pointer          = TValueType*;
      using const_pointer    = TValueType const* const;

      using internal_value_type       = value_type;
      using internal_const_value_type = const_value_type;
      using internal_reference        = reference;
      using internal_const_reference  = const_reference;

      BruidhinnTraits() noexcept                       = default;
      BruidhinnTraits(BruidhinnTraits const&) noexcept = default;
      BruidhinnTraits(BruidhinnTraits&&) noexcept      = default;
      BruidhinnTraits& operator=(BruidhinnTraits const&) noexcept = default;
      BruidhinnTraits& operator=(BruidhinnTraits&&) noexcept = default;
      ~BruidhinnTraits()                                     = default;

      inline internal_const_reference
      to_internal_const(const_reference x) const {
        return x;
      }

      inline internal_reference to_internal(reference x) const {
        return x;
      }

      inline internal_value_type to_internal(rvalue_reference x) const {
        return x;
      }

      inline const_reference
      to_external_const(internal_const_reference x) const {
        return x;
      }

      inline reference to_external(internal_reference x) const {
        return x;
      }

      inline internal_value_type
      internal_copy(internal_const_reference x) const {
        return internal_value_type(x);
      }

      inline value_type external_copy(const_reference x) const {
        return value_type(x);
      }
      inline void internal_free(internal_value_type) const {}
      inline void external_free(value_type) const {}
    };

    template <typename TValueType>
    struct BruidhinnTraits<
        TValueType,
        typename std::enable_if<std::is_pointer<TValueType>::value>::type> {
      using value_type = typename RemovePointerToConst<TValueType>::type*;
      using const_value_type =
          typename RemovePointerToConst<TValueType>::type const*;
      using reference       = value_type;
      using const_reference = const_value_type;
      using pointer         = value_type;
      using const_pointer   = const_value_type;

      using internal_value_type       = value_type;
      using internal_const_value_type = const_value_type;
      using internal_reference        = reference;
      using internal_const_reference  = const_reference;

      BruidhinnTraits() noexcept                       = default;
      BruidhinnTraits(BruidhinnTraits const&) noexcept = default;
      BruidhinnTraits(BruidhinnTraits&&) noexcept      = default;
      BruidhinnTraits& operator=(BruidhinnTraits const&) noexcept = default;
      BruidhinnTraits& operator=(BruidhinnTraits&&) noexcept = default;
      ~BruidhinnTraits()                                     = default;

      inline internal_const_reference
      to_internal_const(const_reference x) const {
        return x;
      }

      inline internal_reference to_internal(reference x) const {
        return x;
      }

      inline const_reference
      to_external_const(internal_const_reference x) const {
        return x;
      }

      inline reference to_external(internal_reference x) const {
        return x;
      }

      inline internal_value_type
      internal_copy(internal_const_reference x) const {
        return x->heap_copy();
      }

      inline value_type external_copy(const_reference x) const {
        // TODO(later) figure out the right way to do this
        return x->heap_copy();
      }

      inline void internal_free(internal_value_type x) const {
        delete x;
      }

      inline void external_free(value_type x) const {
        delete x;
      }
    };

    namespace bruidhinn_traits {
      template <typename TTraits, typename TContainer>
      struct Deref final : private TTraits {
        typename TTraits::const_reference
        operator()(typename TContainer::const_iterator const& it) const {
          // TODO(later) noexcept?
          return this->to_external_const((*it));
        }
      };

      template <typename TTraits, typename TContainer>
      struct AddressOf final : private TTraits {
        typename TTraits::const_pointer
        operator()(typename TContainer::const_iterator const& it) const {
          // TODO(later) noexcept?
          return &(this->to_external_const((*it)));
        }
      };
    }  // namespace bruidhinn_traits

    template <typename TTraits, typename TContainer>
    struct BruidhinnConstIteratorTraits final
        : ConstIteratorTraits<TContainer> {
      static_assert(
          std::is_same<typename TContainer::value_type,
                       typename TTraits::internal_value_type>::value,
          "the 1st and 2nd template parameters must have the same value_type");
      using value_type      = typename TTraits::value_type;
      using const_reference = typename TTraits::const_reference;
      using const_pointer   = typename TTraits::const_pointer;

      using Deref     = bruidhinn_traits::Deref<TTraits, TContainer>;
      using AddressOf = bruidhinn_traits::AddressOf<TTraits, TContainer>;
    };

    template <typename TValueType, typename TContainer>
    using BruidhinnConstIterator = typename std::conditional<
        std::is_same<typename BruidhinnTraits<TValueType>::internal_value_type,
                     typename BruidhinnTraits<TValueType>::value_type>::value,
        typename TContainer::const_iterator,
        ConstIteratorStateless<
            BruidhinnConstIteratorTraits<BruidhinnTraits<TValueType>,
                                         TContainer>>>::type;
  }  // namespace detail
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_INCLUDE_BRUIDHINN_TRAITS_HPP_
