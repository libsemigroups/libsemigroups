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
//
// This file contains functionality for various constant values used in
// libsemigroups.

// TODO(later)
// 1. NegativeInfinity could be comparable with unsigned integers (always <).
// 2. specialisation of operator<< for ostringstream for better printing. I
//    couldn't immediately get this to work.

#ifndef LIBSEMIGROUPS_INCLUDE_CONSTANTS_HPP_
#define LIBSEMIGROUPS_INCLUDE_CONSTANTS_HPP_

#include <cinttypes>    // for int64_t
#include <limits>       // for numeric_limits
#include <type_traits>  // for is_integral

namespace libsemigroups {
  namespace detail {

    struct Min {
      template <typename T>
      constexpr T operator()() const noexcept {
        static_assert(std::is_integral<T>::value,
                      "can only call Min with an integral type");
        return std::numeric_limits<T>::min();
      }
    };

    struct Max {
      template <typename T>
      constexpr T operator()() const noexcept {
        static_assert(std::is_integral<T>::value,
                      "can only call Max with an integral type");
        return std::numeric_limits<T>::max();
      }
    };

    template <int64_t TOffset, typename TMaxOrMin>
    struct Constant {
      static_assert(std::is_same<TMaxOrMin, Max>::value
                        || std::is_same<TMaxOrMin, Min>::value,
                    "template parameter TMaxOrMin must be Max or Min");

      Constant()                = default;
      Constant(Constant const&) = default;
      Constant(Constant&&)      = default;
      Constant& operator=(Constant const&) = default;
      Constant& operator=(Constant&&) = default;
      ~Constant()                     = default;

      template <typename T>
      constexpr operator T() const noexcept {
        static_assert(
            std::is_integral<T>::value
                && (std::is_signed<T>::value
                    || std::is_same<TMaxOrMin, Max>::value),
            "the template parameter T must be an integral type, and either "
            "unsigned or the template parameter TMaxOrMin must be Max.");
        return TMaxOrMin().template operator()<T>() + TOffset;
      }
    };
  }  // namespace detail

  ////////////////////////////////////////////////////////////////////////
  // Constant values
  ////////////////////////////////////////////////////////////////////////

  //! Type for undefined values.
  using Undefined = detail::Constant<0, detail::Max>;

  //! Type for positive infinity.
  using PositiveInfinity = detail::Constant<-1, detail::Max>;

  //! Type for the maximum value of something.
  using LimitMax = detail::Constant<-2, detail::Max>;

  //! Type for negative infinity.
  using NegativeInfinity = detail::Constant<0, detail::Min>;

  //! This variable is used to indicate that a value is undefined. UNDEFINED is
  //! comparable with any integral value (signed or unsigned) or constant via
  //! `==` and `!=` but not via `<` or `>`.
  constexpr Undefined UNDEFINED{};

  //! This variable represents \f$\infty\f$.  POSITIVE_INFINITY is comparable
  //! via `==`, `!=`, `<`, `>` with any integral value (signed or unsigned) and
  //! with NEGATIVE_INFINITY, and is comparable to any other constant via `==`
  //! and `!=`, but not by `<` and `>`.
  constexpr PositiveInfinity POSITIVE_INFINITY{};

  //! This variable represents the maximum value that certain function
  //! parameters can have. LIMIT_MAX is comparable via `==`, `!=`, `<`, `>`
  //! with any integral value (signed or unsigned), and is comparable to any
  //! other constant via `==` and `!=`, but not by `<` and `>`.
  constexpr LimitMax LIMIT_MAX{};

  //! This variable represents \f$-\infty\f$.  NEGATIVE_INFINITY is comparable
  //! via `==`, `!=`, `<`, `>` with any signed integral value and
  //! with POSITIVE_INFINITY, and is comparable to any other constant via `==`
  //! and `!=`.
  constexpr NegativeInfinity NEGATIVE_INFINITY{};

  ////////////////////////////////////////////////////////////////////////
  // Operators for all constants
  ////////////////////////////////////////////////////////////////////////

  // Note that for some reason Catch requires that the comparison functions are
  // in the namespace detail.
  namespace detail {

    // operator==
    // No SFINAE required, since the functions delegated to don't exist.
    template <int64_t R, typename S, typename T>
    constexpr bool operator==(Constant<R, S> const& lhs,
                              T const&              rhs) noexcept {
      return lhs.operator T() == rhs;
    }

    template <int64_t R, typename S, typename T>
    constexpr bool operator==(T const&              lhs,
                              Constant<R, S> const& rhs) noexcept {
      return rhs.operator T() == lhs;
    }

    template <int64_t R1, typename S1, int64_t R2, typename S2>
    constexpr bool operator==(detail::Constant<R1, S1> const&,
                              detail::Constant<R2, S2> const&) noexcept {
      return std::is_same<S1, S2>::value && R1 == R2;
    }

    // operator!=
    // No SFINAE required, since the functions delegated to don't exist.
    template <int64_t R, typename S, typename T>
    constexpr bool operator!=(Constant<R, S> const& lhs,
                              T const&              rhs) noexcept {
      return !(lhs == rhs);
    }

    template <int64_t R, typename S, typename T>
    constexpr bool operator!=(T const&              lhs,
                              Constant<R, S> const& rhs) noexcept {
      return !(lhs == rhs);
    }

    template <int64_t R1, typename S1, int64_t R2, typename S2>
    constexpr bool operator!=(detail::Constant<R1, S1> const& lhs,
                              detail::Constant<R2, S2> const& rhs) noexcept {
      return !(lhs == rhs);
    }

    // operator>
    // No SFINAE required, since the functions delegated to don't exist.
    template <int64_t R, typename S, typename T>
    constexpr bool operator>(detail::Constant<R, S> const& lhs,
                             T const&                      rhs) noexcept {
      return rhs < lhs;
    }

    template <int64_t R, typename S, typename T>
    constexpr bool operator>(T const&                      lhs,
                             detail::Constant<R, S> const& rhs) noexcept {
      return rhs < lhs;
    }

    template <int64_t R, typename S>
    constexpr bool operator>(detail::Constant<R, S> const&,
                             detail::Constant<R, S> const&) noexcept {
      return false;
    }

    template <int64_t R, typename S>
    constexpr bool operator<(detail::Constant<R, S> const&,
                             detail::Constant<R, S> const&) noexcept {
      return false;
    }

    // No further operator< for Constant and Constant unless given explicitly

    ////////////////////////////////////////////////////////////////////////
    // Operators for specific constants
    ////////////////////////////////////////////////////////////////////////

    // PositiveInfinity is not less than any integral value, or
    // NegativeInfinity.
    template <typename T, typename SFINAE = bool>
    constexpr auto operator<(PositiveInfinity const&, T const&) noexcept ->
        typename std::enable_if<std::is_integral<T>::value
                                    || std::is_same<NegativeInfinity, T>::value,
                                SFINAE>::type {
      return false;
    }

    // Every integral value, and negative infinity, is less than
    // PositiveInfinity.
    template <typename T, typename SFINAE = bool>
    constexpr auto operator<(T const&, PositiveInfinity const&) noexcept ->
        typename std::enable_if<std::is_integral<T>::value
                                    || std::is_same<NegativeInfinity, T>::value,
                                SFINAE>::type {
      return true;
    }

    // NegativeInfinity is less than every integral value.
    template <typename T, typename SFINAE = bool>
    constexpr auto operator<(NegativeInfinity const&, T const&) noexcept ->
        typename std::enable_if<std::is_integral<T>::value, SFINAE>::type {
      return true;
    }

    // No integral value is less than NegativeInfinity.
    template <typename T, typename SFINAE = bool>
    constexpr auto operator<(T const&, NegativeInfinity const&) noexcept ->
        typename std::enable_if<std::is_integral<T>::value, SFINAE>::type {
      return false;
    }

    // LimitMax is compared by implicit conversion with any integral value.
    template <typename T, typename SFINAE = bool>
    constexpr auto operator<(LimitMax const& lhs, T const& rhs) noexcept ->
        typename std::enable_if<std::is_integral<T>::value, SFINAE>::type {
      return lhs.operator T() < rhs;
    }

    // LimitMax is compared by implicit conversion with any integral value.
    template <typename T, typename SFINAE = bool>
    constexpr auto operator<(T const& lhs, LimitMax const& rhs) noexcept ->
        typename std::enable_if<std::is_integral<T>::value, SFINAE>::type {
      return lhs < rhs.operator T();
    }

    template <typename T, typename SFINAE = T>
    constexpr auto operator-(LimitMax const& lhs, T const& rhs) noexcept ->
        typename std::enable_if<std::is_integral<T>::value, SFINAE>::type {
      return lhs.operator T() - rhs;
    }

    template <typename T, typename SFINAE = T>
    constexpr auto operator-(T const& lhs, LimitMax const& rhs) noexcept ->
        typename std::enable_if<std::is_integral<T>::value, SFINAE>::type {
      return lhs - rhs.operator T();
    }
  }  // namespace detail
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_INCLUDE_CONSTANTS_HPP_
