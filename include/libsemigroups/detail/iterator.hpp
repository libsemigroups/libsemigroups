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

#ifndef LIBSEMIGROUPS_ITERATOR_HPP_
#define LIBSEMIGROUPS_ITERATOR_HPP_

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
#include <type_traits>  // for enable_if, is_base_of_v, is_void

namespace libsemigroups {
  namespace detail {
    template <typename Iterator>
    Iterator default_postfix_increment(Iterator& it) {
      Iterator copy(it);
      ++it;
      return copy;
    }

    template <typename Internal,
              typename IteratorCategory = std::random_access_iterator_tag>
    struct IteratorTraitsBase {
      using value_type        = typename Internal::value_type;
      using reference         = typename Internal::reference;
      using const_reference   = typename Internal::const_reference;
      using difference_type   = typename Internal::difference_type;
      using size_type         = typename Internal::size_type;
      using const_pointer     = typename Internal::const_pointer;
      using pointer           = typename Internal::pointer;
      using iterator_category = IteratorCategory;

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
      using Swap             = void;
    };

    template <typename Internal>
    struct IteratorTraits : public IteratorTraitsBase<Internal> {
      using internal_iterator_type = typename Internal::iterator;
    };

    template <typename Internal>
    struct ConstIteratorTraits : public IteratorTraitsBase<Internal> {
      using internal_iterator_type = typename Internal::const_iterator;
    };

    // Helpers
    template <typename T, typename S>
    using ReturnTypeIfExists = std::enable_if_t<!std::is_void_v<T>, S>;

    template <typename T, typename S>
    using ReturnTypeIfNotExists = std::enable_if_t<std::is_void_v<T>, S>;

    struct Iterator {};
    struct ConstIterator : Iterator {};

    // IteratorBase holds fallback methods for IteratorStateful, and
    // IteratorStateless
    template <typename Subclass, typename Traits>
    class IteratorBase : public Iterator {
      using internal_iterator_type = typename Traits::internal_iterator_type;

     public:
      ////////////////////////////////////////////////////////////////////////
      // IteratorBase - Aliases/typedef's - public
      ////////////////////////////////////////////////////////////////////////

      using size_type         = typename Traits::size_type;
      using difference_type   = typename Traits::difference_type;
      using const_pointer     = typename Traits::const_pointer;
      using pointer           = typename Traits::pointer;
      using const_reference   = typename Traits::const_reference;
      using reference         = typename Traits::reference;
      using value_type        = typename Traits::value_type;
      using iterator_category = typename Traits::iterator_category;

      ////////////////////////////////////////////////////////////////////////
      // IteratorBase - constructors/destructor - public
      ////////////////////////////////////////////////////////////////////////

      IteratorBase() = default;

      explicit IteratorBase(internal_iterator_type it) noexcept(
          std::is_nothrow_copy_constructible_v<internal_iterator_type>)
          : _wrapped_iter(it) {}

      IteratorBase(IteratorBase const&)            = default;
      IteratorBase& operator=(IteratorBase const&) = default;

      IteratorBase(IteratorBase&&)            = default;
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

      template <typename Sfinae   = Traits,
                typename Operator = typename Sfinae::EqualTo>
      auto constexpr operator==(IteratorBase const& that) const noexcept
          -> ReturnTypeIfNotExists<Operator, bool> {
        return _wrapped_iter == that._wrapped_iter;
      }

      template <typename Sfinae   = Traits,
                typename Operator = typename Sfinae::NotEqualTo>
      auto constexpr operator!=(IteratorBase const& that) const noexcept
          -> ReturnTypeIfNotExists<Operator, bool> {
        return _wrapped_iter != that._wrapped_iter;
      }

      template <typename Sfinae   = Traits,
                typename Operator = typename Sfinae::Less>
      auto constexpr operator<(IteratorBase const& that) const noexcept
          -> ReturnTypeIfNotExists<Operator, bool> {
        return _wrapped_iter < that._wrapped_iter;
      }

      template <typename Sfinae   = Traits,
                typename Operator = typename Sfinae::More>
      auto constexpr operator>(IteratorBase const& that) const noexcept
          -> ReturnTypeIfNotExists<Operator, bool> {
        return _wrapped_iter > that._wrapped_iter;
      }

      template <typename Sfinae   = Traits,
                typename Operator = typename Sfinae::LessOrEqualTo>
      auto constexpr operator<=(IteratorBase const& that) const noexcept
          -> ReturnTypeIfNotExists<Operator, bool> {
        // Shouldn't this just use < or ==?
        return _wrapped_iter <= that._wrapped_iter;
      }

      template <typename Sfinae   = Traits,
                typename Operator = typename Sfinae::MoreOrEqualTo>
      auto constexpr operator>=(IteratorBase const& that) const noexcept
          -> ReturnTypeIfNotExists<Operator, bool> {
        return _wrapped_iter >= that._wrapped_iter;
      }

      template <typename Sfinae   = Traits,
                typename Operator = typename Sfinae::AddAssign>
      auto operator+=(size_type val) noexcept
          -> ReturnTypeIfNotExists<Operator, Subclass&> {
        _wrapped_iter += val;
        return static_cast<Subclass&>(*this);
      }

      template <typename Sfinae   = Traits,
                typename Operator = typename Sfinae::SubtractAssign>
      auto operator-=(size_type val) noexcept
          -> ReturnTypeIfNotExists<Operator, Subclass&> {
        _wrapped_iter -= val;
        return static_cast<Subclass&>(*this);
      }

      // Prefix
      template <typename Sfinae   = Traits,
                typename Operator = typename Sfinae::PrefixIncrement>
      auto operator++() noexcept -> ReturnTypeIfNotExists<Operator, Subclass&> {
        ++_wrapped_iter;
        return static_cast<Subclass&>(*this);
      }

      template <typename Sfinae   = Traits,
                typename Operator = typename Sfinae::PrefixDecrement>
      auto operator--() noexcept -> ReturnTypeIfNotExists<Operator, Subclass&> {
        --_wrapped_iter;
        return static_cast<Subclass&>(*this);
      }

      // Postfix
      template <typename Sfinae   = Traits,
                typename Operator = typename Sfinae::PostfixIncrement>
      auto
      operator++(int) noexcept(std::is_nothrow_copy_constructible_v<Subclass>)
          -> ReturnTypeIfNotExists<Operator, Subclass> {
        Subclass tmp(static_cast<Subclass&>(*this));
        ++static_cast<Subclass&>(*this);
        return tmp;
      }

      template <typename Sfinae   = Traits,
                typename Operator = typename Sfinae::PostfixDecrement>
      auto
      operator--(int) noexcept(std::is_nothrow_copy_constructible_v<Subclass>)
          -> ReturnTypeIfNotExists<Operator, Subclass> {
        Subclass tmp(static_cast<Subclass&>(*this));
        --static_cast<Subclass&>(*this);
        return tmp;
      }

      template <typename Sfinae   = Traits,
                typename Operator = typename Sfinae::Difference>
      auto constexpr operator-(IteratorBase const& that) const noexcept
          -> ReturnTypeIfNotExists<Operator, difference_type> {
        return _wrapped_iter - that._wrapped_iter;
      }

     private:
      ////////////////////////////////////////////////////////////////////////
      // IteratorBase - data - private
      ////////////////////////////////////////////////////////////////////////
      internal_iterator_type _wrapped_iter;
    };

    template <typename Subclass, typename Traits>
    class IteratorStatefulBase : public IteratorBase<Subclass, Traits> {
      using internal_iterator_type = typename Traits::internal_iterator_type;
      using IteratorBaseAlias      = IteratorBase<Subclass, Traits>;

     public:
      using state_type        = typename Traits::state_type;
      using size_type         = typename Traits::size_type;
      using difference_type   = typename Traits::difference_type;
      using const_pointer     = typename Traits::const_pointer;
      using pointer           = typename Traits::pointer;
      using const_reference   = typename Traits::const_reference;
      using reference         = typename Traits::reference;
      using value_type        = typename Traits::value_type;
      using iterator_category = typename Traits::iterator_category;

      IteratorStatefulBase() = default;

      explicit IteratorStatefulBase(
          state_type stt,
          internal_iterator_type
              it) noexcept(std::
                               is_nothrow_copy_constructible_v<
                                   IteratorBaseAlias>)
          : IteratorBaseAlias(it), _state(stt) {}

      IteratorStatefulBase(IteratorStatefulBase const&);
      IteratorStatefulBase& operator=(IteratorStatefulBase const&);

      IteratorStatefulBase(IteratorStatefulBase&&);
      IteratorStatefulBase& operator=(IteratorStatefulBase&&);

      virtual ~IteratorStatefulBase();

      state_type& get_state() const noexcept {
        return _state;
      }

      using IteratorBaseAlias::operator==;
      using IteratorBaseAlias::operator!=;
      using IteratorBaseAlias::operator<;  // NOLINT()
      using IteratorBaseAlias::operator>;  // NOLINT()
      using IteratorBaseAlias::operator<=;
      using IteratorBaseAlias::operator>=;

      template <typename Sfinae   = Traits,
                typename Operator = typename Sfinae::EqualTo>
      auto constexpr operator==(IteratorStatefulBase const& that) const noexcept
          -> ReturnTypeIfExists<Operator, bool> {
        return Operator()(this->get_state(),
                          this->get_wrapped_iter(),
                          that.get_state(),
                          that.get_wrapped_iter());
      }

      template <typename Sfinae   = Traits,
                typename Operator = typename Sfinae::NotEqualTo>
      auto constexpr operator!=(IteratorStatefulBase const& that) const noexcept
          -> ReturnTypeIfExists<Operator, bool> {
        return Operator()(this->get_state(),
                          this->get_wrapped_iter(),
                          that.get_state(),
                          that.get_wrapped_iter());
      }

      template <typename Sfinae   = Traits,
                typename Operator = typename Sfinae::Less>
      auto constexpr operator<(IteratorStatefulBase const& that) const noexcept
          -> ReturnTypeIfExists<Operator, bool> {
        return Operator()(this->get_state(),
                          this->get_wrapped_iter(),
                          that.get_state(),
                          that.get_wrapped_iter());
      }

      template <typename Sfinae   = Traits,
                typename Operator = typename Sfinae::More>
      auto constexpr operator>(IteratorStatefulBase const& that) const noexcept
          -> ReturnTypeIfExists<Operator, bool> {
        return Operator()(this->get_state(),
                          this->get_wrapped_iter(),
                          that.get_state(),
                          that.get_wrapped_iter());
      }

      template <typename Sfinae   = Traits,
                typename Operator = typename Sfinae::LessOrEqualTo>
      auto constexpr operator<=(IteratorStatefulBase const& that) const noexcept
          -> ReturnTypeIfExists<Operator, bool> {
        return Operator()(this->get_state(),
                          this->get_wrapped_iter(),
                          that.get_state(),
                          that.get_wrapped_iter());
      }

      template <typename Sfinae   = Traits,
                typename Operator = typename Sfinae::MoreOrEqualTo>
      auto constexpr operator>=(IteratorStatefulBase const& that) const noexcept
          -> ReturnTypeIfExists<Operator, bool> {
        return Operator()(this->get_state(),
                          this->get_wrapped_iter(),
                          that.get_state(),
                          that.get_wrapped_iter());
      }

      template <typename Sfinae   = Traits,
                typename Operator = typename Sfinae::AddAssign>
      auto operator+=(size_type val) noexcept
          -> ReturnTypeIfExists<Operator, Subclass&> {
        Operator()(this->get_state(), this->get_wrapped_iter(), val);
        return static_cast<Subclass&>(*this);
      }

      template <typename Sfinae   = Traits,
                typename Operator = typename Sfinae::SubtractAssign>
      auto operator-=(size_type val) noexcept
          -> ReturnTypeIfExists<Operator, Subclass&> {
        Operator()(this->get_state(), this->get_wrapped_iter(), val);
        return static_cast<Subclass&>(*this);
      }

      // Prefix
      template <typename Sfinae   = Traits,
                typename Operator = typename Sfinae::PrefixIncrement>
      auto operator++() noexcept -> ReturnTypeIfExists<Operator, Subclass&> {
        Operator()(this->get_state(), this->get_wrapped_iter());
        return static_cast<Subclass&>(*this);
      }

      template <typename Sfinae   = Traits,
                typename Operator = typename Sfinae::PrefixDecrement>
      auto operator--() noexcept -> ReturnTypeIfExists<Operator, Subclass&> {
        Operator()(this->get_state(), this->get_wrapped_iter());
        return static_cast<Subclass&>(*this);
      }

      template <typename Sfinae   = Traits,
                typename Operator = typename Sfinae::Difference>
      auto constexpr operator-(IteratorStatefulBase const& that) const noexcept
          -> ReturnTypeIfExists<Operator, difference_type> {
        return Operator()(this->get_state(),
                          this->get_wrapped_iter(),
                          that.get_state(),
                          that.get_wrapped_iter());
      }

      template <typename Sfinae   = Traits,
                typename Operator = typename Sfinae::Swap>
      auto swap(IteratorStatefulBase& that) noexcept
          -> ReturnTypeIfExists<Operator, void> {
        return Operator()(this->get_wrapped_iter(),
                          that.get_wrapped_iter(),
                          this->get_state(),
                          that.get_state());
      }

      template <typename Sfinae   = Traits,
                typename Operator = typename Sfinae::Swap>
      auto swap(IteratorStatefulBase& that) noexcept
          -> ReturnTypeIfNotExists<Operator, void> {
        return std::swap(*this, that);
      }

     private:
      mutable state_type _state;
    };

    template <typename S, typename T>
    IteratorStatefulBase<S, T>::IteratorStatefulBase(
        IteratorStatefulBase const&)
        = default;

    template <typename S, typename T>
    IteratorStatefulBase<S, T>&
    IteratorStatefulBase<S, T>::operator=(IteratorStatefulBase const&)
        = default;

    template <typename S, typename T>
    IteratorStatefulBase<S, T>::IteratorStatefulBase(IteratorStatefulBase&&)
        = default;

    template <typename S, typename T>
    IteratorStatefulBase<S, T>&
    IteratorStatefulBase<S, T>::operator=(IteratorStatefulBase&&)
        = default;

    template <typename S, typename T>
    IteratorStatefulBase<S, T>::~IteratorStatefulBase() = default;

    template <typename Subclass, typename Traits>
    class IteratorStatelessBase : public IteratorBase<Subclass, Traits> {
      using IteratorBaseAlias      = IteratorBase<Subclass, Traits>;
      using internal_iterator_type = typename Traits::internal_iterator_type;

     public:
      using size_type         = typename Traits::size_type;
      using difference_type   = typename Traits::difference_type;
      using const_pointer     = typename Traits::const_pointer;
      using pointer           = typename Traits::pointer;
      using const_reference   = typename Traits::const_reference;
      using reference         = typename Traits::reference;
      using value_type        = typename Traits::value_type;
      using iterator_category = typename Traits::iterator_category;

      IteratorStatelessBase() = default;

      explicit IteratorStatelessBase(internal_iterator_type it) noexcept(
          std::is_nothrow_copy_constructible_v<IteratorBaseAlias>)
          : IteratorBaseAlias(it) {}

      IteratorStatelessBase(IteratorStatelessBase const&)            = default;
      IteratorStatelessBase& operator=(IteratorStatelessBase const&) = default;

      IteratorStatelessBase(IteratorStatelessBase&&)            = default;
      IteratorStatelessBase& operator=(IteratorStatelessBase&&) = default;

      virtual ~IteratorStatelessBase() = default;

      using IteratorBaseAlias::operator==;
      using IteratorBaseAlias::operator!=;
      using IteratorBaseAlias::operator<;  // NOLINT(whitespace/operators)
      using IteratorBaseAlias::operator>;  // NOLINT(whitespace/operators)
      using IteratorBaseAlias::operator<=;
      using IteratorBaseAlias::operator>=;

      template <typename Sfinae   = Traits,
                typename Operator = typename Sfinae::EqualTo>
      auto constexpr
      operator==(IteratorStatelessBase const& that) const noexcept
          -> std::enable_if_t<
              std::is_trivially_default_constructible_v<Operator>,
              bool> {
        return Operator()(this->get_wrapped_iter(), that.get_wrapped_iter());
      }

      template <typename Sfinae   = Traits,
                typename Operator = typename Sfinae::NotEqualTo>
      auto constexpr
      operator!=(IteratorStatelessBase const& that) const noexcept
          -> ReturnTypeIfExists<Operator, bool> {
        return Operator()(this->get_wrapped_iter(), that.get_wrapped_iter());
      }

      template <typename Sfinae   = Traits,
                typename Operator = typename Sfinae::Less>
      auto constexpr operator<(IteratorStatelessBase const& that) const noexcept
          -> ReturnTypeIfExists<Operator, bool> {
        return Operator()(this->get_wrapped_iter(), that.get_wrapped_iter());
      }

      template <typename Sfinae   = Traits,
                typename Operator = typename Sfinae::More>
      auto constexpr operator>(IteratorStatelessBase const& that) const noexcept
          -> ReturnTypeIfExists<Operator, bool> {
        return Operator()(this->get_wrapped_iter(), that.get_wrapped_iter());
      }

      template <typename Sfinae   = Traits,
                typename Operator = typename Sfinae::LessOrEqualTo>
      auto constexpr
      operator<=(IteratorStatelessBase const& that) const noexcept
          -> ReturnTypeIfExists<Operator, bool> {
        return Operator()(this->get_wrapped_iter(), that.get_wrapped_iter());
      }

      template <typename Sfinae   = Traits,
                typename Operator = typename Sfinae::MoreOrEqualTo>
      auto constexpr
      operator>=(IteratorStatelessBase const& that) const noexcept
          -> ReturnTypeIfExists<Operator, bool> {
        return Operator()(this->get_wrapped_iter(), that.get_wrapped_iter());
      }

      template <typename Sfinae   = Traits,
                typename Operator = typename Sfinae::AddAssign>
      auto operator+=(size_type val) noexcept
          -> ReturnTypeIfExists<Operator, Subclass&> {
        Operator()(this->get_wrapped_iter(), val);
        return static_cast<Subclass&>(*this);
      }

      template <typename Sfinae   = Traits,
                typename Operator = typename Sfinae::SubtractAssign>
      auto operator-=(size_type val) noexcept
          -> ReturnTypeIfExists<Operator, Subclass&> {
        Operator()(this->get_wrapped_iter(), val);
        return static_cast<Subclass&>(*this);
      }

      // Prefix
      template <typename Sfinae   = Traits,
                typename Operator = typename Sfinae::PrefixIncrement>
      auto operator++() noexcept -> ReturnTypeIfExists<Operator, Subclass&> {
        Operator()(this->get_wrapped_iter());
        return static_cast<Subclass&>(*this);
      }

      template <typename Sfinae   = Traits,
                typename Operator = typename Sfinae::PrefixDecrement>
      auto operator--() noexcept -> ReturnTypeIfExists<Operator, Subclass&> {
        Operator()(this->get_wrapped_iter());
        return static_cast<Subclass&>(*this);
      }

      template <typename Sfinae   = Traits,
                typename Operator = typename Sfinae::Difference>
      auto constexpr operator-(IteratorStatelessBase const& that) const noexcept
          -> ReturnTypeIfExists<Operator, difference_type> {
        return Operator()(this->get_wrapped_iter(), that.get_wrapped_iter());
      }

      template <typename Sfinae   = Traits,
                typename Operator = typename Sfinae::Swap>
      auto swap(IteratorStatelessBase& that) noexcept
          -> ReturnTypeIfExists<Operator, void> {
        return Operator()(this->get_wrapped_iter(),
                          that.get_wrapped_iter(),
                          this->get_state(),
                          that.get_state());
      }

      template <typename Sfinae   = Traits,
                typename Operator = typename Sfinae::Swap>
      auto swap(IteratorStatelessBase& that) noexcept
          -> ReturnTypeIfNotExists<Operator, void> {
        return std::swap(*this, that);
      }
    };

    template <typename Traits>
    class ConstIteratorStateful
        : public ConstIterator,
          public IteratorStatefulBase<ConstIteratorStateful<Traits>, Traits> {
      using internal_iterator_type = typename Traits::internal_iterator_type;
      using IteratorBaseAlias
          = IteratorBase<ConstIteratorStateful<Traits>, Traits>;
      using IteratorStatefulBaseAlias
          = IteratorStatefulBase<ConstIteratorStateful<Traits>, Traits>;

     public:
      using state_type        = typename Traits::state_type;
      using size_type         = typename Traits::size_type;
      using difference_type   = typename Traits::difference_type;
      using const_pointer     = typename Traits::const_pointer;
      using pointer           = typename Traits::const_pointer;
      using const_reference   = typename Traits::const_reference;
      using reference         = typename Traits::const_reference;
      using value_type        = typename Traits::value_type;
      using iterator_category = typename Traits::iterator_category;

      ConstIteratorStateful() = default;
      ConstIteratorStateful(state_type stt, internal_iterator_type it) noexcept(
          std::is_nothrow_copy_constructible_v<IteratorStatefulBaseAlias>)
          : IteratorStatefulBaseAlias(stt, it) {}

      ConstIteratorStateful(ConstIteratorStateful const&)            = default;
      ConstIteratorStateful& operator=(ConstIteratorStateful const&) = default;

      ConstIteratorStateful(ConstIteratorStateful&&)            = default;
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

      template <typename Sfinae   = Traits,
                typename Operator = typename Sfinae::Deref>
      auto operator*() const noexcept
          -> ReturnTypeIfExists<Operator, const_reference> {
        return Operator()(this->get_state(), this->get_wrapped_iter());
      }

      template <typename Sfinae   = Traits,
                typename Operator = typename Sfinae::Deref>
      auto operator*() const noexcept
          -> ReturnTypeIfNotExists<Operator, const_reference> {
        return *this->get_wrapped_iter();
      }

      template <typename Sfinae   = Traits,
                typename Operator = typename Sfinae::AddressOf>
      auto operator->() const noexcept
          -> ReturnTypeIfExists<Operator, const_pointer> {
        return Operator()(this->get_state(), this->get_wrapped_iter());
      }

      template <typename Sfinae   = Traits,
                typename Operator = typename Sfinae::AddressOf>
      auto operator->() const noexcept
          -> ReturnTypeIfNotExists<Operator, const_pointer> {
        return &(*this->get_wrapped_iter());
      }

      constexpr const_reference operator[](size_type pos) const noexcept {
        return *(*this + pos);
      }
    };

    template <typename Traits>
    class ConstIteratorStateless
        : public ConstIterator,
          public IteratorStatelessBase<ConstIteratorStateless<Traits>, Traits> {
      using internal_iterator_type = typename Traits::internal_iterator_type;

      using IteratorBaseAlias
          = IteratorBase<ConstIteratorStateless<Traits>, Traits>;
      using IteratorStatelessBaseAlias
          = IteratorStatelessBase<ConstIteratorStateless<Traits>, Traits>;

     public:
      using size_type         = typename Traits::size_type;
      using difference_type   = typename Traits::difference_type;
      using const_pointer     = typename Traits::const_pointer;
      using pointer           = typename Traits::const_pointer;
      using const_reference   = typename Traits::const_reference;
      using reference         = typename Traits::const_reference;
      using value_type        = typename Traits::value_type;
      using iterator_category = typename Traits::iterator_category;

      ConstIteratorStateless() = default;

      explicit ConstIteratorStateless(internal_iterator_type it) noexcept(
          std::is_nothrow_copy_constructible_v<IteratorStatelessBaseAlias>)
          : IteratorStatelessBaseAlias(it) {}

      ConstIteratorStateless(ConstIteratorStateless const&) = default;
      ConstIteratorStateless& operator=(ConstIteratorStateless const&)
          = default;

      ConstIteratorStateless(ConstIteratorStateless&&)            = default;
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

      template <typename Sfinae   = Traits,
                typename Operator = typename Sfinae::Deref>
      auto operator*() const noexcept
          -> ReturnTypeIfExists<Operator, const_reference> {
        return Operator()(this->get_wrapped_iter());
      }

      template <typename Sfinae   = Traits,
                typename Operator = typename Sfinae::Deref>
      auto operator*() const noexcept
          -> ReturnTypeIfNotExists<Operator, const_reference> {
        return *this->get_wrapped_iter();
      }

      template <typename Sfinae   = Traits,
                typename Operator = typename Sfinae::AddressOf>
      auto operator->() const noexcept
          -> ReturnTypeIfExists<Operator, const_pointer> {
        return Operator()(this->get_wrapped_iter());
      }

      template <typename Sfinae   = Traits,
                typename Operator = typename Sfinae::AddressOf>
      auto operator->() const noexcept
          -> ReturnTypeIfNotExists<Operator, const_pointer> {
        return &(*this->get_wrapped_iter());
      }

      constexpr const_reference operator[](size_type pos) const noexcept {
        return *(*this + pos);
      }
    };

    template <typename Traits>
    class IteratorStateful
        : public IteratorStatefulBase<IteratorStateful<Traits>, Traits> {
      using internal_iterator_type = typename Traits::internal_iterator_type;
      using IteratorBaseAlias = IteratorBase<IteratorStateful<Traits>, Traits>;
      using IteratorStatefulBaseAlias
          = IteratorStatefulBase<IteratorStateful<Traits>, Traits>;

     public:
      using state_type        = typename Traits::state_type;
      using size_type         = typename Traits::size_type;
      using difference_type   = typename Traits::difference_type;
      using const_pointer     = typename Traits::const_pointer;
      using pointer           = typename Traits::pointer;
      using const_reference   = typename Traits::const_reference;
      using reference         = typename Traits::reference;
      using value_type        = typename Traits::value_type;
      using iterator_category = typename Traits::iterator_category;

      IteratorStateful() = default;

      explicit IteratorStateful(
          state_type stt,
          internal_iterator_type
              it) noexcept(std::
                               is_nothrow_copy_constructible_v<
                                   IteratorStatefulBaseAlias>)
          : IteratorStatefulBaseAlias(stt, it) {}

      IteratorStateful(IteratorStateful const&)            = default;
      IteratorStateful& operator=(IteratorStateful const&) = default;

      IteratorStateful(IteratorStateful&&)            = default;
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

      template <typename Sfinae   = Traits,
                typename Operator = typename Sfinae::Deref>
      auto operator*() noexcept -> ReturnTypeIfExists<Operator, reference> {
        return Operator()(this->get_state(), this->get_wrapped_iter());
      }

      template <typename Sfinae   = Traits,
                typename Operator = typename Sfinae::Deref>
      auto operator*() noexcept -> ReturnTypeIfNotExists<Operator, reference> {
        return *this->get_wrapped_iter();
      }

      template <typename Sfinae   = Traits,
                typename Operator = typename Sfinae::Deref>
      auto operator*() const noexcept
          -> ReturnTypeIfExists<Operator, const_reference> {
        return Operator()(this->get_state(), this->get_wrapped_iter());
      }

      template <typename Sfinae   = Traits,
                typename Operator = typename Sfinae::Deref>
      auto operator*() const noexcept
          -> ReturnTypeIfNotExists<Operator, const_reference> {
        return *this->get_wrapped_iter();
      }

      template <typename Sfinae   = Traits,
                typename Operator = typename Sfinae::AddressOf>
      auto operator->() noexcept -> ReturnTypeIfExists<Operator, pointer> {
        return Operator()(this->get_state(), this->get_wrapped_iter());
      }

      template <typename Sfinae   = Traits,
                typename Operator = typename Sfinae::AddressOf>
      auto operator->() noexcept -> ReturnTypeIfNotExists<Operator, pointer> {
        return &(*this->get_wrapped_iter());
      }

      reference operator[](size_type pos) noexcept {
        return *(*this + pos);
      }
    };

    template <typename Traits>
    class IteratorStateless
        : public IteratorStatelessBase<IteratorStateless<Traits>, Traits> {
      using internal_iterator_type = typename Traits::internal_iterator_type;

      using IteratorBaseAlias = IteratorBase<IteratorStateless<Traits>, Traits>;
      using IteratorStatelessBaseAlias
          = IteratorStatelessBase<IteratorStateless<Traits>, Traits>;

     public:
      using size_type         = typename Traits::size_type;
      using difference_type   = typename Traits::difference_type;
      using const_pointer     = typename Traits::const_pointer;
      using pointer           = typename Traits::pointer;
      using const_reference   = typename Traits::const_reference;
      using reference         = typename Traits::reference;
      using value_type        = typename Traits::value_type;
      using iterator_category = typename Traits::iterator_category;

      IteratorStateless() = default;

      explicit IteratorStateless(internal_iterator_type it) noexcept(
          std::is_nothrow_copy_constructible_v<IteratorStatelessBaseAlias>)
          : IteratorStatelessBaseAlias(it) {}

      IteratorStateless(IteratorStateless const&)            = default;
      IteratorStateless& operator=(IteratorStateless const&) = default;

      IteratorStateless(IteratorStateless&&)            = default;
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

      template <typename Sfinae   = Traits,
                typename Operator = typename Sfinae::Deref>
      auto operator*() noexcept -> ReturnTypeIfExists<Operator, reference> {
        return Operator()(this->get_wrapped_iter());
      }

      template <typename Sfinae   = Traits,
                typename Operator = typename Sfinae::Deref>
      auto operator*() noexcept -> ReturnTypeIfNotExists<Operator, reference> {
        return *this->get_wrapped_iter();
      }

      template <typename Sfinae   = Traits,
                typename Operator = typename Sfinae::Deref>
      auto operator*() const noexcept
          -> ReturnTypeIfExists<Operator, const_reference> {
        return Operator()(this->get_wrapped_iter());
      }

      template <typename Sfinae   = Traits,
                typename Operator = typename Sfinae::Deref>
      auto operator*() const noexcept
          -> ReturnTypeIfNotExists<Operator, const_reference> {
        return *this->get_wrapped_iter();
      }

      template <typename Sfinae   = Traits,
                typename Operator = typename Sfinae::AddressOf>
      auto operator->() noexcept -> ReturnTypeIfExists<Operator, pointer> {
        return Operator()(this->get_wrapped_iter());
      }

      template <typename Sfinae   = Traits,
                typename Operator = typename Sfinae::AddressOf>
      auto operator->() noexcept -> ReturnTypeIfNotExists<Operator, pointer> {
        return &(*this->get_wrapped_iter());
      }

      reference operator[](size_type pos) noexcept {
        return *(*this + pos);
      }
    };

    template <typename It,
              typename = std::enable_if_t<std::is_base_of_v<Iterator, It>>>
    It operator+(It const& cpy, size_t val) {
      It out(cpy);
      out += val;
      return out;
    }

    template <typename It,
              typename = std::enable_if_t<std::is_base_of_v<Iterator, It>>>
    It operator-(It const& cpy, size_t val) {
      It out(cpy);
      out -= val;
      return out;
    }

    template <typename It,
              typename = std::enable_if_t<std::is_base_of_v<Iterator, It>>>
    It operator+(size_t val, It it) {
      return it + val;
    }

    template <typename It,
              typename = std::enable_if_t<std::is_base_of_v<Iterator, It>>>
    It operator-(size_t val, It it) {
      return it - val;
    }
  }  // namespace detail

  //! No doc
  template <typename T>
  inline void swap(detail::ConstIteratorStateless<T>& x,
                   detail::ConstIteratorStateless<T>& y) noexcept {
    x.swap(y);
  }

  //! No doc
  template <typename T>
  inline void swap(detail::IteratorStateless<T>& x,
                   detail::IteratorStateless<T>& y) noexcept {
    x.swap(y);
  }

  //! No doc
  template <typename T>
  inline void swap(detail::ConstIteratorStateful<T>& x,
                   detail::ConstIteratorStateful<T>& y) noexcept {
    x.swap(y);
  }

  //! No doc
  template <typename T>
  inline void swap(detail::IteratorStateful<T>& x,
                   detail::IteratorStateful<T>& y) noexcept {
    x.swap(y);
  }
}  // namespace libsemigroups

namespace std {
  template <typename T>
  inline void
  swap(libsemigroups::detail::ConstIteratorStateless<T>& x,
       libsemigroups::detail::ConstIteratorStateless<T>& y) noexcept {
    x.swap(y);
  }

  template <typename T>
  inline void swap(libsemigroups::detail::IteratorStateless<T>& x,
                   libsemigroups::detail::IteratorStateless<T>& y) noexcept {
    x.swap(y);
  }

  template <typename T>
  inline void
  swap(libsemigroups::detail::ConstIteratorStateful<T>& x,
       libsemigroups::detail::ConstIteratorStateful<T>& y) noexcept {
    x.swap(y);
  }

  template <typename T>
  inline void swap(libsemigroups::detail::IteratorStateful<T>& x,
                   libsemigroups::detail::IteratorStateful<T>& y) noexcept {
    x.swap(y);
  }
}  // namespace std
#endif  // LIBSEMIGROUPS_ITERATOR_HPP_
