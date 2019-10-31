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

// This file contains some functionality for wrapping iterators.

#ifndef LIBSEMIGROUPS_INCLUDE_ITERATOR_HPP_
#define LIBSEMIGROUPS_INCLUDE_ITERATOR_HPP_

// TODO(later)
// 1. Remove duplicate code, make sure that all member functions are
//    implemented
// 2. At present if the name of the operation is not correct a derived class of
//    IteratorTraits or ConstIteratorTraits, then the code will still compile,
//    but the expected operator will not be used. It would be better to specify
//    which of the operators are expected explicitly, which might also make
//    some of the SFINAE simpler.

#include <cstddef>      // for size_t
#include <iterator>     // for random_access_iterator_tag
#include <type_traits>  // for enable_if, is_base_of, is_void

namespace libsemigroups {
  namespace detail {

    template <typename TInternalType>
    struct IteratorTraitsBase {
      using value_type        = typename TInternalType::value_type;
      using reference         = typename TInternalType::reference;
      using const_reference   = typename TInternalType::const_reference;
      using difference_type   = typename TInternalType::difference_type;
      using size_type         = typename TInternalType::size_type;
      using const_pointer     = typename TInternalType::const_pointer;
      using pointer           = typename TInternalType::pointer;
      using iterator_category = std::random_access_iterator_tag;

      using Deref            = void;
      using AddressOf        = void;
      using EqualTo          = void;
      using NotEqualTo       = void;
      using Less             = void;
      using More             = void;
      using LessOrEqualTo    = void;
      using MoreOrEqualTo    = void;
      using Subtract         = void;  // TODO(later) implement
      using Add              = void;  // TODO(later) implement
      using Difference       = void;
      using PostfixIncrement = void;
      using PostfixDecrement = void;
      using PrefixIncrement  = void;
      using PrefixDecrement  = void;
      using AddAssign        = void;
      using SubtractAssign   = void;
      using Subscript        = void;  // TODO(later) implement
    };

    template <typename TInternalType>
    struct IteratorTraits : public IteratorTraitsBase<TInternalType> {
      using internal_iterator_type = typename TInternalType::iterator;
    };

    template <typename TInternalType>
    struct ConstIteratorTraits : public IteratorTraitsBase<TInternalType> {
      using internal_iterator_type = typename TInternalType::const_iterator;
    };

    // Helpers
    template <typename T, typename S>
    using ReturnTypeIfExists =
        typename std::enable_if<!std::is_void<T>::value, S>::type;

    template <typename T, typename S>
    using ReturnTypeIfNotExists =
        typename std::enable_if<std::is_void<T>::value, S>::type;

    struct Iterator {};
    struct ConstIterator : Iterator {};

    // IteratorBase holds fallback methods for IteratorStateful, and
    // IteratorStateless
    template <typename TSubclass, typename TIteratorTraits>
    class IteratorBase : public Iterator {
      using internal_iterator_type =
          typename TIteratorTraits::internal_iterator_type;

     public:
      ////////////////////////////////////////////////////////////////////////
      // IteratorBase - Aliases/typedef's - public
      ////////////////////////////////////////////////////////////////////////

      using size_type         = typename TIteratorTraits::size_type;
      using difference_type   = typename TIteratorTraits::difference_type;
      using const_pointer     = typename TIteratorTraits::const_pointer;
      using pointer           = typename TIteratorTraits::pointer;
      using const_reference   = typename TIteratorTraits::const_reference;
      using reference         = typename TIteratorTraits::reference;
      using value_type        = typename TIteratorTraits::value_type;
      using iterator_category = typename TIteratorTraits::iterator_category;

      ////////////////////////////////////////////////////////////////////////
      // IteratorBase - constructors/destructor - public
      ////////////////////////////////////////////////////////////////////////

      explicit IteratorBase(internal_iterator_type it) noexcept(
          std::is_nothrow_copy_constructible<internal_iterator_type>::value)
          : _wrapped_iter(it) {}

      IteratorBase(IteratorBase const&) = default;
      IteratorBase& operator=(IteratorBase const&) = default;

      IteratorBase(IteratorBase&&) = default;
      IteratorBase& operator=(IteratorBase&&) = default;

      virtual ~IteratorBase() = default;

      inline internal_iterator_type& get_wrapped_iter() noexcept {
        return _wrapped_iter;
      }

      inline internal_iterator_type const& get_wrapped_iter() const noexcept {
        return _wrapped_iter;
      }

      ////////////////////////////////////////////////////////////////////////
      // IteratorBase - operators - public
      ////////////////////////////////////////////////////////////////////////

      template <typename TSfinae   = TIteratorTraits,
                typename TOperator = typename TSfinae::EqualTo>
      auto constexpr operator==(IteratorBase const& that) const noexcept
          -> ReturnTypeIfNotExists<TOperator, bool> {
        return _wrapped_iter == that._wrapped_iter;
      }

      template <typename TSfinae   = TIteratorTraits,
                typename TOperator = typename TSfinae::NotEqualTo>
      auto constexpr operator!=(IteratorBase const& that) const noexcept
          -> ReturnTypeIfNotExists<TOperator, bool> {
        return _wrapped_iter != that._wrapped_iter;
      }

      template <typename TSfinae   = TIteratorTraits,
                typename TOperator = typename TSfinae::Less>
      auto constexpr operator<(IteratorBase const& that) const noexcept
          -> ReturnTypeIfNotExists<TOperator, bool> {
        return _wrapped_iter < that._wrapped_iter;
      }

      template <typename TSfinae   = TIteratorTraits,
                typename TOperator = typename TSfinae::More>
      auto constexpr operator>(IteratorBase const& that) const noexcept
          -> ReturnTypeIfNotExists<TOperator, bool> {
        return _wrapped_iter > that._wrapped_iter;
      }

      template <typename TSfinae   = TIteratorTraits,
                typename TOperator = typename TSfinae::LessOrEqualTo>
      auto constexpr operator<=(IteratorBase const& that) const noexcept
          -> ReturnTypeIfNotExists<TOperator, bool> {
        return _wrapped_iter <= that._wrapped_iter;
      }

      template <typename TSfinae   = TIteratorTraits,
                typename TOperator = typename TSfinae::MoreOrEqualTo>
      auto constexpr operator>=(IteratorBase const& that) const noexcept
          -> ReturnTypeIfNotExists<TOperator, bool> {
        return _wrapped_iter >= that._wrapped_iter;
      }

      template <typename TSfinae   = TIteratorTraits,
                typename TOperator = typename TSfinae::AddAssign>
      auto operator+=(size_type val) noexcept
          -> ReturnTypeIfNotExists<TOperator, TSubclass&> {
        _wrapped_iter += val;
        return static_cast<TSubclass&>(*this);
      }

      template <typename TSfinae   = TIteratorTraits,
                typename TOperator = typename TSfinae::SubtractAssign>
      auto operator-=(size_type val) noexcept
          -> ReturnTypeIfNotExists<TOperator, TSubclass&> {
        _wrapped_iter -= val;
        return static_cast<TSubclass&>(*this);
      }

      // Prefix
      template <typename TSfinae   = TIteratorTraits,
                typename TOperator = typename TSfinae::PrefixIncrement>
      auto operator++() noexcept
          -> ReturnTypeIfNotExists<TOperator, TSubclass&> {
        ++_wrapped_iter;
        return static_cast<TSubclass&>(*this);
      }

      template <typename TSfinae   = TIteratorTraits,
                typename TOperator = typename TSfinae::PrefixDecrement>
      auto operator--() noexcept
          -> ReturnTypeIfNotExists<TOperator, TSubclass&> {
        --_wrapped_iter;
        return static_cast<TSubclass&>(*this);
      }

      // Postfix
      template <typename TSfinae   = TIteratorTraits,
                typename TOperator = typename TSfinae::PostfixIncrement>
      auto operator++(int) noexcept(
          std::is_nothrow_copy_constructible<TSubclass>::value)
          -> ReturnTypeIfNotExists<TOperator, TSubclass> {
        TSubclass tmp(static_cast<TSubclass&>(*this));
        ++static_cast<TSubclass&>(*this);
        return tmp;
      }

      template <typename TSfinae   = TIteratorTraits,
                typename TOperator = typename TSfinae::PostfixDecrement>
      auto operator--(int) noexcept(
          std::is_nothrow_copy_constructible<TSubclass>::value)
          -> ReturnTypeIfNotExists<TOperator, TSubclass> {
        TSubclass tmp(static_cast<TSubclass&>(*this));
        --static_cast<TSubclass&>(*this);
        return tmp;
      }

      template <typename TSfinae   = TIteratorTraits,
                typename TOperator = typename TSfinae::Difference>
      auto constexpr operator-(IteratorBase const& that) const noexcept
          -> ReturnTypeIfNotExists<TOperator, difference_type> {
        return _wrapped_iter - that._wrapped_iter;
      }

     private:
      ////////////////////////////////////////////////////////////////////////
      // IteratorBase - data - private
      ////////////////////////////////////////////////////////////////////////
      internal_iterator_type _wrapped_iter;
    };

    template <typename TSubclass, typename TIteratorTraits>
    class IteratorStatefulBase
        : public IteratorBase<TSubclass, TIteratorTraits> {
      using state_type = typename TIteratorTraits::state_type;
      using internal_iterator_type =
          typename TIteratorTraits::internal_iterator_type;
      using IteratorBaseAlias = IteratorBase<TSubclass, TIteratorTraits>;

     public:
      using size_type         = typename TIteratorTraits::size_type;
      using difference_type   = typename TIteratorTraits::difference_type;
      using const_pointer     = typename TIteratorTraits::const_pointer;
      using pointer           = typename TIteratorTraits::pointer;
      using const_reference   = typename TIteratorTraits::const_reference;
      using reference         = typename TIteratorTraits::reference;
      using value_type        = typename TIteratorTraits::value_type;
      using iterator_category = typename TIteratorTraits::iterator_category;

      explicit IteratorStatefulBase(
          state_type             stt,
          internal_iterator_type it) noexcept(std::
                                                  is_nothrow_copy_constructible<
                                                      IteratorBaseAlias>::value)
          : IteratorBaseAlias(it), _state(stt) {}

      IteratorStatefulBase(IteratorStatefulBase const&) = default;
      IteratorStatefulBase& operator=(IteratorStatefulBase const&) = default;

      IteratorStatefulBase(IteratorStatefulBase&&) = default;
      IteratorStatefulBase& operator=(IteratorStatefulBase&&) = default;

      virtual ~IteratorStatefulBase() = default;

      state_type const get_state() const noexcept {
        return _state;
      }

      using IteratorBaseAlias::operator==;
      using IteratorBaseAlias::operator!=;
      using IteratorBaseAlias::operator<;  // NOLINT()
      using IteratorBaseAlias::operator>;  // NOLINT()
      using IteratorBaseAlias::operator<=;
      using IteratorBaseAlias::operator>=;

      template <typename TSfinae   = TIteratorTraits,
                typename TOperator = typename TSfinae::EqualTo>
      auto constexpr operator==(IteratorStatefulBase const& that) const noexcept
          -> ReturnTypeIfExists<TOperator, bool> {
        return TOperator()(
            get_state(), this->get_wrapped_iter(), that.get_wrapped_iter());
      }

      template <typename TSfinae   = TIteratorTraits,
                typename TOperator = typename TSfinae::NotEqualTo>
      auto constexpr operator!=(IteratorStatefulBase const& that) const noexcept
          -> ReturnTypeIfExists<TOperator, bool> {
        return TOperator()(
            get_state(), this->get_wrapped_iter(), that.get_wrapped_iter());
      }

      template <typename TSfinae   = TIteratorTraits,
                typename TOperator = typename TSfinae::Less>
      auto constexpr operator<(IteratorStatefulBase const& that) const noexcept
          -> ReturnTypeIfExists<TOperator, bool> {
        return TOperator()(
            get_state(), this->get_wrapped_iter(), that.get_wrapped_iter());
      }

      template <typename TSfinae   = TIteratorTraits,
                typename TOperator = typename TSfinae::More>
      auto constexpr operator>(IteratorStatefulBase const& that) const noexcept
          -> ReturnTypeIfExists<TOperator, bool> {
        return TOperator()(
            get_state(), this->get_wrapped_iter(), that.get_wrapped_iter());
      }

      template <typename TSfinae   = TIteratorTraits,
                typename TOperator = typename TSfinae::LessOrEqualTo>
      auto constexpr operator<=(IteratorStatefulBase const& that) const noexcept
          -> ReturnTypeIfExists<TOperator, bool> {
        return TOperator()(
            get_state(), this->get_wrapped_iter(), that.get_wrapped_iter());
      }

      template <typename TSfinae   = TIteratorTraits,
                typename TOperator = typename TSfinae::MoreOrEqualTo>
      auto constexpr operator>=(IteratorStatefulBase const& that) const noexcept
          -> ReturnTypeIfExists<TOperator, bool> {
        return TOperator()(
            get_state(), this->get_wrapped_iter(), that.get_wrapped_iter());
      }

      template <typename TSfinae   = TIteratorTraits,
                typename TOperator = typename TSfinae::AddAssign>
      auto operator+=(size_type val) noexcept
          -> ReturnTypeIfExists<TOperator, TSubclass&> {
        TOperator()(get_state(), this->get_wrapped_iter(), val);
        return static_cast<TSubclass&>(*this);
      }

      template <typename TSfinae   = TIteratorTraits,
                typename TOperator = typename TSfinae::SubtractAssign>
      auto operator-=(size_type val) noexcept
          -> ReturnTypeIfExists<TOperator, TSubclass&> {
        TOperator()(get_state(), this->get_wrapped_iter(), val);
        return static_cast<TSubclass&>(*this);
      }

      // Prefix
      template <typename TSfinae   = TIteratorTraits,
                typename TOperator = typename TSfinae::PrefixIncrement>
      auto operator++() noexcept -> ReturnTypeIfExists<TOperator, TSubclass&> {
        TOperator()(get_state(), this->get_wrapped_iter());
        return static_cast<TSubclass&>(*this);
      }

      template <typename TSfinae   = TIteratorTraits,
                typename TOperator = typename TSfinae::PrefixDecrement>
      auto operator--() noexcept -> ReturnTypeIfExists<TOperator, TSubclass&> {
        TOperator()(get_state(), this->get_wrapped_iter());
        return static_cast<TSubclass&>(*this);
      }

      template <typename TSfinae   = TIteratorTraits,
                typename TOperator = typename TSfinae::Difference>
      auto constexpr operator-(IteratorStatefulBase const& that) const noexcept
          -> ReturnTypeIfExists<TOperator, difference_type> {
        return TOperator()(
            get_state(), this->get_wrapped_iter(), that.get_wrapped_iter());
      }

     private:
      state_type _state;
    };

    template <typename TSubclass, typename TIteratorTraits>
    class IteratorStatelessBase
        : public IteratorBase<TSubclass, TIteratorTraits> {
      using IteratorBaseAlias = IteratorBase<TSubclass, TIteratorTraits>;
      using internal_iterator_type =
          typename TIteratorTraits::internal_iterator_type;

     public:
      using size_type         = typename TIteratorTraits::size_type;
      using difference_type   = typename TIteratorTraits::difference_type;
      using const_pointer     = typename TIteratorTraits::const_pointer;
      using pointer           = typename TIteratorTraits::pointer;
      using const_reference   = typename TIteratorTraits::const_reference;
      using reference         = typename TIteratorTraits::reference;
      using value_type        = typename TIteratorTraits::value_type;
      using iterator_category = typename TIteratorTraits::iterator_category;

      explicit IteratorStatelessBase(internal_iterator_type it) noexcept(
          std::is_nothrow_copy_constructible<IteratorBaseAlias>::value)
          : IteratorBaseAlias(it) {}

      IteratorStatelessBase(IteratorStatelessBase const&) = default;
      IteratorStatelessBase& operator=(IteratorStatelessBase const&) = default;

      IteratorStatelessBase(IteratorStatelessBase&&) = default;
      IteratorStatelessBase& operator=(IteratorStatelessBase&&) = default;

      virtual ~IteratorStatelessBase() = default;

      using IteratorBaseAlias::operator==;
      using IteratorBaseAlias::operator!=;
      using IteratorBaseAlias::operator<;  // NOLINT(whitespace/operators)
      using IteratorBaseAlias::operator>;  // NOLINT(whitespace/operators)
      using IteratorBaseAlias::operator<=;
      using IteratorBaseAlias::operator>=;

      template <typename TSfinae   = TIteratorTraits,
                typename TOperator = typename TSfinae::EqualTo>
      auto constexpr operator==(IteratorStatelessBase const& that) const
          noexcept -> typename std::enable_if<
              std::is_trivially_default_constructible<TOperator>::value,
              bool>::type {
        return TOperator()(this->get_wrapped_iter(), that.get_wrapped_iter());
      }

      template <typename TSfinae   = TIteratorTraits,
                typename TOperator = typename TSfinae::NotEqualTo>
      auto constexpr operator!=(IteratorStatelessBase const& that) const
          noexcept -> ReturnTypeIfExists<TOperator, bool> {
        return TOperator()(this->get_wrapped_iter(), that.get_wrapped_iter());
      }

      template <typename TSfinae   = TIteratorTraits,
                typename TOperator = typename TSfinae::Less>
      auto constexpr operator<(IteratorStatelessBase const& that) const noexcept
          -> ReturnTypeIfExists<TOperator, bool> {
        return TOperator()(this->get_wrapped_iter(), that.get_wrapped_iter());
      }

      template <typename TSfinae   = TIteratorTraits,
                typename TOperator = typename TSfinae::More>
      auto constexpr operator>(IteratorStatelessBase const& that) const noexcept
          -> ReturnTypeIfExists<TOperator, bool> {
        return TOperator()(this->get_wrapped_iter(), that.get_wrapped_iter());
      }

      template <typename TSfinae   = TIteratorTraits,
                typename TOperator = typename TSfinae::LessOrEqualTo>
      auto constexpr operator<=(IteratorStatelessBase const& that) const
          noexcept -> ReturnTypeIfExists<TOperator, bool> {
        return TOperator()(this->get_wrapped_iter(), that.get_wrapped_iter());
      }

      template <typename TSfinae   = TIteratorTraits,
                typename TOperator = typename TSfinae::MoreOrEqualTo>
      auto constexpr operator>=(IteratorStatelessBase const& that) const
          noexcept -> ReturnTypeIfExists<TOperator, bool> {
        return TOperator()(this->get_wrapped_iter(), that.get_wrapped_iter());
      }

      template <typename TSfinae   = TIteratorTraits,
                typename TOperator = typename TSfinae::AddAssign>
      auto operator+=(size_type val) noexcept
          -> ReturnTypeIfExists<TOperator, TSubclass&> {
        TOperator()(this->get_wrapped_iter(), val);
        return static_cast<TSubclass&>(*this);
      }

      template <typename TSfinae   = TIteratorTraits,
                typename TOperator = typename TSfinae::SubtractAssign>
      auto operator-=(size_type val) noexcept
          -> ReturnTypeIfExists<TOperator, TSubclass&> {
        TOperator()(this->get_wrapped_iter(), val);
        return static_cast<TSubclass&>(*this);
      }

      // Prefix
      template <typename TSfinae   = TIteratorTraits,
                typename TOperator = typename TSfinae::PrefixIncrement>
      auto operator++() noexcept -> ReturnTypeIfExists<TOperator, TSubclass&> {
        TOperator()(this->get_wrapped_iter());
        return static_cast<TSubclass&>(*this);
      }

      template <typename TSfinae   = TIteratorTraits,
                typename TOperator = typename TSfinae::PrefixDecrement>
      auto operator--() noexcept -> ReturnTypeIfExists<TOperator, TSubclass&> {
        TOperator()(this->get_wrapped_iter());
        return static_cast<TSubclass&>(*this);
      }

      template <typename TSfinae   = TIteratorTraits,
                typename TOperator = typename TSfinae::Difference>
      auto constexpr operator-(IteratorStatelessBase const& that) const noexcept
          -> ReturnTypeIfExists<TOperator, difference_type> {
        return TOperator()(this->get_wrapped_iter(), that.get_wrapped_iter());
      }
    };

    template <typename TIteratorTraits>
    class ConstIteratorStateful final
        : public ConstIterator,
          public IteratorStatefulBase<ConstIteratorStateful<TIteratorTraits>,
                                      TIteratorTraits> {
      using state_type = typename TIteratorTraits::state_type;
      using internal_iterator_type =
          typename TIteratorTraits::internal_iterator_type;
      using IteratorBaseAlias
          = IteratorBase<ConstIteratorStateful<TIteratorTraits>,
                         TIteratorTraits>;
      using IteratorStatefulBaseAlias
          = IteratorStatefulBase<ConstIteratorStateful<TIteratorTraits>,
                                 TIteratorTraits>;

     public:
      using size_type         = typename TIteratorTraits::size_type;
      using difference_type   = typename TIteratorTraits::difference_type;
      using const_pointer     = typename TIteratorTraits::const_pointer;
      using pointer           = typename TIteratorTraits::const_pointer;
      using const_reference   = typename TIteratorTraits::const_reference;
      using reference         = typename TIteratorTraits::const_reference;
      using value_type        = typename TIteratorTraits::value_type;
      using iterator_category = typename TIteratorTraits::iterator_category;

      ConstIteratorStateful(state_type stt, internal_iterator_type it) noexcept(
          std::is_nothrow_copy_constructible<IteratorStatefulBaseAlias>::value)
          : IteratorStatefulBaseAlias(stt, it) {}

      ConstIteratorStateful(ConstIteratorStateful const&) = default;
      ConstIteratorStateful& operator=(ConstIteratorStateful const&) = default;

      ConstIteratorStateful(ConstIteratorStateful&&) = default;
      ConstIteratorStateful& operator=(ConstIteratorStateful&&) = default;

      ~ConstIteratorStateful() = default;

      using IteratorBaseAlias::operator+=;
      using IteratorBaseAlias::operator-=;
      using IteratorBaseAlias::operator--;
      using IteratorBaseAlias::operator++;
      using IteratorBaseAlias::operator-;

      using IteratorStatefulBaseAlias::operator+=;
      using IteratorStatefulBaseAlias::operator-=;
      using IteratorStatefulBaseAlias::operator--;
      using IteratorStatefulBaseAlias::operator++;
      using IteratorStatefulBaseAlias::operator-;

      template <typename TSfinae   = TIteratorTraits,
                typename TOperator = typename TSfinae::Deref>
      auto operator*() const noexcept
          -> ReturnTypeIfExists<TOperator, const_reference> {
        return TOperator()(this->get_state(), this->get_wrapped_iter());
      }

      template <typename TSfinae   = TIteratorTraits,
                typename TOperator = typename TSfinae::Deref>
      auto operator*() const noexcept
          -> ReturnTypeIfNotExists<TOperator, const_reference> {
        return *this->get_wrapped_iter();
      }

      template <typename TSfinae   = TIteratorTraits,
                typename TOperator = typename TSfinae::AddressOf>
      auto operator-> () const noexcept
          -> ReturnTypeIfExists<TOperator, const_pointer> {
        return TOperator()(this->get_state(), this->get_wrapped_iter());
      }

      template <typename TSfinae   = TIteratorTraits,
                typename TOperator = typename TSfinae::AddressOf>
      auto operator-> () const noexcept
          -> ReturnTypeIfNotExists<TOperator, const_pointer> {
        return &(*this->get_wrapped_iter());
      }

      constexpr const_reference operator[](size_type pos) const noexcept {
        return *(*this + pos);
      }
    };

    template <typename TIteratorTraits>
    class ConstIteratorStateless final
        : public ConstIterator,
          public IteratorStatelessBase<ConstIteratorStateless<TIteratorTraits>,
                                       TIteratorTraits> {
      using internal_iterator_type =
          typename TIteratorTraits::internal_iterator_type;

      using IteratorBaseAlias
          = IteratorBase<ConstIteratorStateless<TIteratorTraits>,
                         TIteratorTraits>;
      using IteratorStatelessBaseAlias
          = IteratorStatelessBase<ConstIteratorStateless<TIteratorTraits>,
                                  TIteratorTraits>;

     public:
      using size_type         = typename TIteratorTraits::size_type;
      using difference_type   = typename TIteratorTraits::difference_type;
      using const_pointer     = typename TIteratorTraits::const_pointer;
      using pointer           = typename TIteratorTraits::const_pointer;
      using const_reference   = typename TIteratorTraits::const_reference;
      using reference         = typename TIteratorTraits::const_reference;
      using value_type        = typename TIteratorTraits::value_type;
      using iterator_category = typename TIteratorTraits::iterator_category;

      explicit ConstIteratorStateless(internal_iterator_type it) noexcept(
          std::is_nothrow_copy_constructible<IteratorStatelessBaseAlias>::value)
          : IteratorStatelessBaseAlias(it) {}

      ConstIteratorStateless(ConstIteratorStateless const&) = default;
      ConstIteratorStateless& operator=(ConstIteratorStateless const&)
          = default;

      ConstIteratorStateless(ConstIteratorStateless&&) = default;
      ConstIteratorStateless& operator=(ConstIteratorStateless&&) = default;

      ~ConstIteratorStateless() = default;

      using IteratorBaseAlias::operator+=;
      using IteratorBaseAlias::operator-=;
      using IteratorBaseAlias::operator--;
      using IteratorBaseAlias::operator++;
      using IteratorBaseAlias::operator-;

      using IteratorStatelessBaseAlias::operator+=;
      using IteratorStatelessBaseAlias::operator-=;
      using IteratorStatelessBaseAlias::operator--;
      using IteratorStatelessBaseAlias::operator++;
      using IteratorStatelessBaseAlias::operator-;

      template <typename TSfinae   = TIteratorTraits,
                typename TOperator = typename TSfinae::Deref>
      auto operator*() const noexcept
          -> ReturnTypeIfExists<TOperator, const_reference> {
        return TOperator()(this->get_wrapped_iter());
      }

      template <typename TSfinae   = TIteratorTraits,
                typename TOperator = typename TSfinae::Deref>
      auto operator*() const noexcept
          -> ReturnTypeIfNotExists<TOperator, const_reference> {
        return *this->get_wrapped_iter();
      }

      template <typename TSfinae   = TIteratorTraits,
                typename TOperator = typename TSfinae::AddressOf>
      auto operator-> () const noexcept
          -> ReturnTypeIfExists<TOperator, const_pointer> {
        return TOperator()(this->get_wrapped_iter());
      }

      template <typename TSfinae   = TIteratorTraits,
                typename TOperator = typename TSfinae::AddressOf>
      auto operator-> () const noexcept
          -> ReturnTypeIfNotExists<TOperator, const_pointer> {
        return &(*this->get_wrapped_iter());
      }

      constexpr const_reference operator[](size_type pos) const noexcept {
        return *(*this + pos);
      }
    };

    template <typename TIteratorTraits>
    class IteratorStateful final
        : public IteratorStatefulBase<IteratorStateful<TIteratorTraits>,
                                      TIteratorTraits> {
      using state_type = typename TIteratorTraits::state_type;
      using internal_iterator_type =
          typename TIteratorTraits::internal_iterator_type;
      using IteratorBaseAlias
          = IteratorBase<IteratorStateful<TIteratorTraits>, TIteratorTraits>;
      using IteratorStatefulBaseAlias
          = IteratorStatefulBase<IteratorStateful<TIteratorTraits>,
                                 TIteratorTraits>;

     public:
      using size_type         = typename TIteratorTraits::size_type;
      using difference_type   = typename TIteratorTraits::difference_type;
      using const_pointer     = typename TIteratorTraits::const_pointer;
      using pointer           = typename TIteratorTraits::pointer;
      using const_reference   = typename TIteratorTraits::const_reference;
      using reference         = typename TIteratorTraits::reference;
      using value_type        = typename TIteratorTraits::value_type;
      using iterator_category = typename TIteratorTraits::iterator_category;

      explicit IteratorStateful(
          state_type stt,
          internal_iterator_type
              it) noexcept(std::
                               is_nothrow_copy_constructible<
                                   IteratorStatefulBaseAlias>::value)
          : IteratorStatefulBaseAlias(stt, it) {}

      IteratorStateful(IteratorStateful const&) = default;
      IteratorStateful& operator=(IteratorStateful const&) = default;

      IteratorStateful(IteratorStateful&&) = default;
      IteratorStateful& operator=(IteratorStateful&&) = default;

      ~IteratorStateful() = default;

      using IteratorBaseAlias::operator+=;
      using IteratorBaseAlias::operator-=;
      using IteratorBaseAlias::operator--;
      using IteratorBaseAlias::operator++;
      using IteratorBaseAlias::operator-;

      using IteratorStatefulBaseAlias::operator+=;
      using IteratorStatefulBaseAlias::operator-=;
      using IteratorStatefulBaseAlias::operator--;
      using IteratorStatefulBaseAlias::operator++;
      using IteratorStatefulBaseAlias::operator-;

      template <typename TSfinae   = TIteratorTraits,
                typename TOperator = typename TSfinae::Deref>
      auto operator*() noexcept -> ReturnTypeIfExists<TOperator, reference> {
        return TOperator()(this->get_state(), this->get_wrapped_iter());
      }

      template <typename TSfinae   = TIteratorTraits,
                typename TOperator = typename TSfinae::Deref>
      auto operator*() noexcept -> ReturnTypeIfNotExists<TOperator, reference> {
        return *this->get_wrapped_iter();
      }

      template <typename TSfinae   = TIteratorTraits,
                typename TOperator = typename TSfinae::Deref>
      auto operator*() const noexcept
          -> ReturnTypeIfExists<TOperator, const_reference> {
        return TOperator()(this->get_state(), this->get_wrapped_iter());
      }

      template <typename TSfinae   = TIteratorTraits,
                typename TOperator = typename TSfinae::Deref>
      auto operator*() const noexcept
          -> ReturnTypeIfNotExists<TOperator, const_reference> {
        return *this->get_wrapped_iter();
      }

      template <typename TSfinae   = TIteratorTraits,
                typename TOperator = typename TSfinae::AddressOf>
      auto operator-> () noexcept -> ReturnTypeIfExists<TOperator, pointer> {
        return TOperator()(this->get_state(), this->get_wrapped_iter());
      }

      template <typename TSfinae   = TIteratorTraits,
                typename TOperator = typename TSfinae::AddressOf>
      auto operator-> () noexcept -> ReturnTypeIfNotExists<TOperator, pointer> {
        return &(*this->get_wrapped_iter());
      }

      reference operator[](size_type pos) noexcept {
        return *(*this + pos);
      }
    };

    template <typename TIteratorTraits>
    class IteratorStateless final
        : public IteratorStatelessBase<IteratorStateless<TIteratorTraits>,
                                       TIteratorTraits> {
      using internal_iterator_type =
          typename TIteratorTraits::internal_iterator_type;

      using IteratorBaseAlias
          = IteratorBase<IteratorStateless<TIteratorTraits>, TIteratorTraits>;
      using IteratorStatelessBaseAlias
          = IteratorStatelessBase<IteratorStateless<TIteratorTraits>,
                                  TIteratorTraits>;

     public:
      using size_type         = typename TIteratorTraits::size_type;
      using difference_type   = typename TIteratorTraits::difference_type;
      using const_pointer     = typename TIteratorTraits::const_pointer;
      using pointer           = typename TIteratorTraits::pointer;
      using const_reference   = typename TIteratorTraits::const_reference;
      using reference         = typename TIteratorTraits::reference;
      using value_type        = typename TIteratorTraits::value_type;
      using iterator_category = typename TIteratorTraits::iterator_category;

      explicit IteratorStateless(internal_iterator_type it) noexcept(
          std::is_nothrow_copy_constructible<IteratorStatelessBaseAlias>::value)
          : IteratorStatelessBaseAlias(it) {}

      IteratorStateless(IteratorStateless const&) = default;
      IteratorStateless& operator=(IteratorStateless const&) = default;

      IteratorStateless(IteratorStateless&&) = default;
      IteratorStateless& operator=(IteratorStateless&&) = default;

      ~IteratorStateless() = default;

      using IteratorBaseAlias::operator+=;
      using IteratorBaseAlias::operator-=;
      using IteratorBaseAlias::operator--;
      using IteratorBaseAlias::operator++;
      using IteratorBaseAlias::operator-;

      using IteratorStatelessBaseAlias::operator+=;
      using IteratorStatelessBaseAlias::operator-=;
      using IteratorStatelessBaseAlias::operator--;
      using IteratorStatelessBaseAlias::operator++;
      using IteratorStatelessBaseAlias::operator-;

      template <typename TSfinae   = TIteratorTraits,
                typename TOperator = typename TSfinae::Deref>
      auto operator*() noexcept -> ReturnTypeIfExists<TOperator, reference> {
        return TOperator()(this->get_wrapped_iter());
      }

      template <typename TSfinae   = TIteratorTraits,
                typename TOperator = typename TSfinae::Deref>
      auto operator*() noexcept -> ReturnTypeIfNotExists<TOperator, reference> {
        return *this->get_wrapped_iter();
      }

      template <typename TSfinae   = TIteratorTraits,
                typename TOperator = typename TSfinae::Deref>
      auto operator*() const noexcept
          -> ReturnTypeIfExists<TOperator, const_reference> {
        return TOperator()(this->get_wrapped_iter());
      }

      template <typename TSfinae   = TIteratorTraits,
                typename TOperator = typename TSfinae::Deref>
      auto operator*() const noexcept
          -> ReturnTypeIfNotExists<TOperator, const_reference> {
        return *this->get_wrapped_iter();
      }

      template <typename TSfinae   = TIteratorTraits,
                typename TOperator = typename TSfinae::AddressOf>
      auto operator-> () noexcept -> ReturnTypeIfExists<TOperator, pointer> {
        return TOperator()(this->get_wrapped_iter());
      }

      template <typename TSfinae   = TIteratorTraits,
                typename TOperator = typename TSfinae::AddressOf>
      auto operator-> () noexcept -> ReturnTypeIfNotExists<TOperator, pointer> {
        return &(*this->get_wrapped_iter());
      }

      reference operator[](size_type pos) noexcept {
        return *(*this + pos);
      }
    };

    template <typename TIteratorType,
              typename = typename std::enable_if<
                  std::is_base_of<Iterator, TIteratorType>::value>::type>
    TIteratorType operator+(TIteratorType const& cpy, size_t val) {
      TIteratorType out(cpy);
      out += val;
      return out;
    }

    template <typename TIteratorType,
              typename = typename std::enable_if<
                  std::is_base_of<Iterator, TIteratorType>::value>::type>
    TIteratorType operator-(TIteratorType const& cpy, size_t val) {
      TIteratorType out(cpy);
      out -= val;
      return out;
    }

    template <typename TIteratorType,
              typename = typename std::enable_if<
                  std::is_base_of<Iterator, TIteratorType>::value>::type>
    TIteratorType operator+(size_t val, TIteratorType it) {
      return it + val;
    }

    template <typename TIteratorType,
              typename = typename std::enable_if<
                  std::is_base_of<Iterator, TIteratorType>::value>::type>
    TIteratorType operator-(size_t val, TIteratorType it) {
      return it - val;
    }
  }  // namespace detail
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_INCLUDE_ITERATOR_HPP_
