//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019-2023 James D. Mitchell
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
// FroidurePin, SchreierSims, Konieczny, and other classes.

#ifndef LIBSEMIGROUPS_BRUIDHINN_TRAITS_HPP_
#define LIBSEMIGROUPS_BRUIDHINN_TRAITS_HPP_

#include <type_traits>  // for conditional_t etc

#include "libsemigroups/iterator.hpp"  // for ConstIteratorTraits

namespace libsemigroups {
  namespace detail {
    template <typename T>
    struct IsSmall : std::integral_constant<bool, (sizeof(T) <= 16)> {};

    template <typename T>
    struct RemovePointerToConst {
      using type
          = std::remove_const_t<std::remove_pointer_t<std::remove_const_t<T>>>;
    };

    // Bruidhinn is (apparently) the Scots gaelic word for "convey", the
    // following traits class is used to convey values from an external type to
    // an internal type, and/or vice versa.

    template <typename Value, typename = void>
    struct BruidhinnTraits {};

    template <typename Value>
    struct BruidhinnTraits<
        Value,
        std::enable_if_t<(!std::is_trivial_v<Value> || !IsSmall<Value>::value)
                         && !std::is_pointer_v<Value>>> {
      using value_type       = Value;
      using const_value_type = Value const;
      using reference        = Value&;
      using rvalue_reference = Value&&;
      using const_reference  = Value const&;
      using pointer          = Value*;
      using const_pointer    = Value const*;

      using internal_value_type       = pointer;
      using internal_const_value_type = const_pointer;
      using internal_reference        = pointer;
      using internal_const_reference  = const_pointer;

      BruidhinnTraits() noexcept                                  = default;
      BruidhinnTraits(BruidhinnTraits const&) noexcept            = default;
      BruidhinnTraits(BruidhinnTraits&&) noexcept                 = default;
      BruidhinnTraits& operator=(BruidhinnTraits const&) noexcept = default;
      BruidhinnTraits& operator=(BruidhinnTraits&&) noexcept      = default;
      ~BruidhinnTraits()                                          = default;

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

    template <typename Value>
    struct BruidhinnTraits<
        Value,
        std::enable_if_t<std::is_trivial_v<Value> && IsSmall<Value>::value
                         && !std::is_pointer_v<Value>>> {
      using value_type       = Value;
      using const_value_type = Value const;
      using reference        = Value&;
      using rvalue_reference = Value&&;
      using const_reference  = Value const&;
      using pointer          = Value*;
      using const_pointer    = Value const* const;

      using internal_value_type       = value_type;
      using internal_const_value_type = const_value_type;
      using internal_reference        = reference;
      using internal_const_reference  = const_reference;

      BruidhinnTraits() noexcept                                  = default;
      BruidhinnTraits(BruidhinnTraits const&) noexcept            = default;
      BruidhinnTraits(BruidhinnTraits&&) noexcept                 = default;
      BruidhinnTraits& operator=(BruidhinnTraits const&) noexcept = default;
      BruidhinnTraits& operator=(BruidhinnTraits&&) noexcept      = default;
      ~BruidhinnTraits()                                          = default;

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

    template <typename Value>
    struct BruidhinnTraits<Value, std::enable_if_t<std::is_pointer_v<Value>>> {
      using value_type = typename RemovePointerToConst<Value>::type*;
      using const_value_type =
          typename RemovePointerToConst<Value>::type const*;
      using reference       = value_type;
      using const_reference = const_value_type;
      using pointer         = value_type;
      using const_pointer   = const_value_type;

      using internal_value_type       = value_type;
      using internal_const_value_type = const_value_type;
      using internal_reference        = reference;
      using internal_const_reference  = const_reference;

      BruidhinnTraits() noexcept                                  = default;
      BruidhinnTraits(BruidhinnTraits const&) noexcept            = default;
      BruidhinnTraits(BruidhinnTraits&&) noexcept                 = default;
      BruidhinnTraits& operator=(BruidhinnTraits const&) noexcept = default;
      BruidhinnTraits& operator=(BruidhinnTraits&&) noexcept      = default;
      ~BruidhinnTraits()                                          = default;

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
        // ODO(later) figure out the right way to do this
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
      template <typename Traits, typename Container>
      struct Deref : private Traits {
        typename Traits::const_reference
        operator()(typename Container::const_iterator const& it) const {
          // TODO(later) noexcept?
          return this->to_external_const((*it));
        }
      };

      template <typename Traits, typename Container>
      struct AddressOf : private Traits {
        typename Traits::const_pointer
        operator()(typename Container::const_iterator const& it) const {
          // ODO(later) noexcept?
          return &(this->to_external_const((*it)));
        }
      };
    }  // namespace bruidhinn_traits

    template <typename Traits, typename Container>
    struct BruidhinnConstIteratorTraits : ConstIteratorTraits<Container> {
      static_assert(
          std::is_same_v<typename Container::value_type,
                         typename Traits::internal_value_type>,
          "the 1st and 2nd template parameters must have the same value_type");
      using value_type      = typename Traits::value_type;
      using const_reference = typename Traits::const_reference;
      using const_pointer   = typename Traits::const_pointer;

      using Deref     = bruidhinn_traits::Deref<Traits, Container>;
      using AddressOf = bruidhinn_traits::AddressOf<Traits, Container>;
    };

    template <typename Value, typename Container>
    using BruidhinnConstIterator = std::conditional_t<
        std::is_same_v<typename BruidhinnTraits<Value>::internal_value_type,
                       typename BruidhinnTraits<Value>::value_type>,
        typename Container::const_iterator,
        ConstIteratorStateless<
            BruidhinnConstIteratorTraits<BruidhinnTraits<Value>, Container>>>;
  }  // namespace detail
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_BRUIDHINN_TRAITS_HPP_
