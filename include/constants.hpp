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

// This file contains definitions of certain constants for libsemigroups. These
// are classes that correspond to integer values (in some sense). This file
// exists to avoid having to redeclare static constants like UNDEFINED, for
// every integer type. For example, the struct UNDEFINED can be implicitly
// converted to any integral type, and has a value specified by
// std::numeric_limits<TIntegralType>::max() minus a compile-time offset (where
// TIntegralType is a member function template, and can hence be any integral
// type), and we overload operator== for UNDEFINED and every integral type. So,
// UNDEFINED can be used as if it was an integral type (say -1), for the
// purposes of storing it in a data member and for comparisons. This allows us
// to avoid duplicating code for constants like UNDEFINED, and makes UNDEFINED
// more straightforward to use (no template parameters, or qualifications are
// required, as they used to be). FIXME update this comment
//
// This approach also has some drawbacks, if UNDEFINED is implicitly converted
// to two different integral types and then these are compared, this will yield
// false, and so == of UNDEFINED and itself is not transitive, so beware.

#ifndef LIBSEMIGROUPS_INCLUDE_CONSTANTS_HPP_
#define LIBSEMIGROUPS_INCLUDE_CONSTANTS_HPP_

#include <algorithm>
#include <type_traits>

namespace libsemigroups {
  struct Min {
    template <typename TIntegralType>
    constexpr TIntegralType operator()() const noexcept {
      static_assert(std::is_integral<TIntegralType>::value,
                    "can only call Min with an integral type");
      return std::numeric_limits<TIntegralType>::min();
    }
  };

  struct Max {
    template <typename TIntegralType>
    constexpr TIntegralType operator()() const noexcept {
      static_assert(std::is_integral<TIntegralType>::value,
                    "can only call Max with an integral type");
      return std::numeric_limits<TIntegralType>::max();
    }
  };

  template <int64_t TOffset, class TMaxOrMin = Max> struct Constant {
    static_assert(std::is_same<TMaxOrMin, Max>::value
                      || std::is_same<TMaxOrMin, Min>::value,
                  "template parameter TMaxOrMin must be Max or Min");
    template <typename TIntegralType>
    constexpr operator TIntegralType() const noexcept {
      static_assert(
          std::is_integral<TIntegralType>::value,
          "can only implicitly convert a Constant to an integral type");
      return TMaxOrMin().template operator()<TIntegralType>() + TOffset;
    }

    template <typename TIntegralType>
    constexpr TIntegralType get_value() const noexcept {
      static_assert(
          std::is_integral<TIntegralType>::value,
          "the template parameter TIntegralType must be an integral type");
      return this->operator TIntegralType();
    }
  };

  template <typename TIntegralType, int64_t TOffset, class TMaxOrMin>
  bool operator==(TIntegralType const&                lhs,
                  Constant<TOffset, TMaxOrMin> const& rhs) noexcept {
    static_assert(std::is_integral<TIntegralType>::value,
                  "can only compare constants and integral types");
    return lhs == rhs.operator TIntegralType();
  }

  template <typename TIntegralType, int64_t TOffset, class TMaxOrMin>
  bool operator!=(TIntegralType const&                lhs,
                  Constant<TOffset, TMaxOrMin> const& rhs) noexcept {
    static_assert(std::is_integral<TIntegralType>::value,
                  "can only compare constants and integral types");
    return !(lhs == rhs);
  }

  //! UNDEFINED value.
  //!
  //! This variable is used to indicate that a value is undefined. For
  //! example, the cached hash value of an Element is initially set to this
  //! value.
  constexpr struct UNDEFINED : public Constant<0> {
  } UNDEFINED;

  //! Value representing \f$\infty\f$.
  constexpr struct POSITIVE_INFINITY : public Constant<-1> {
  } POSITIVE_INFINITY;

  template <typename TIntegralType>
  constexpr bool operator<(TIntegralType const&,
                           struct POSITIVE_INFINITY const&) noexcept {
    static_assert(std::is_integral<TIntegralType>::value,
                  "can only compare constants and integral types");
    return true;
  }

  template <typename TIntegralType>
  constexpr bool operator<(struct POSITIVE_INFINITY const&,
                           TIntegralType const&) noexcept {
    static_assert(std::is_integral<TIntegralType>::value,
                  "can only compare constants and integral types");
    return false;
  }

  //! Value representing \f$-\infty\f$.
  constexpr struct NEGATIVE_INFINITY : public Constant<0, Min> {
  } NEGATIVE_INFINITY;

  template <typename TIntegralType>
  constexpr bool operator<(TIntegralType const&,
                           struct NEGATIVE_INFINITY const&) noexcept {
    static_assert(std::is_integral<TIntegralType>::value,
                  "can only compare constants and integral types");
    return false;
  }

  template <typename TIntegralType>
  constexpr bool operator<(struct NEGATIVE_INFINITY const&,
                           TIntegralType const&) noexcept {
    static_assert(std::is_integral<TIntegralType>::value,
                  "can only compare constants and integral types");
    return true;
  }

  constexpr bool operator<(struct NEGATIVE_INFINITY const&,
                           struct POSITIVE_INFINITY const&) noexcept {
    return true;
  }

  constexpr bool operator<(struct POSITIVE_INFINITY const&,
                           struct NEGATIVE_INFINITY const&) noexcept {
    return false;
  }

  //!
  constexpr struct LIMIT_MAX : public Constant<-2> {
  } LIMIT_MAX;

  template <typename TIntegralType>
  bool operator<(TIntegralType const&    lhs,
                 struct LIMIT_MAX const& rhs) noexcept {
    static_assert(std::is_integral<TIntegralType>::value,
                  "operator< only defined for constants and integral types");
    return lhs < rhs.get_value<TIntegralType>();
  }

  template <typename TIntegralType>
  bool operator<(struct LIMIT_MAX const& lhs,
                 TIntegralType const&    rhs) noexcept {
    static_assert(std::is_integral<TIntegralType>::value,
                  "operator< only defined for constants and integral types");
    return lhs.get_value<TIntegralType>() < rhs;
  }

  template <typename TIntegralType>
  bool operator>(TIntegralType const&    lhs,
                 struct LIMIT_MAX const& rhs) noexcept {
    static_assert(std::is_integral<TIntegralType>::value,
                  "operator< only defined for constants and integral types");
    return rhs < lhs;
  }

  template <typename TIntegralType>
  bool operator>(struct LIMIT_MAX const& lhs,
                 TIntegralType const&    rhs) noexcept {
    static_assert(std::is_integral<TIntegralType>::value,
                  "operator< only defined for constants and integral types");
    return rhs < lhs;
  }

  template <typename TIntegralType>
  TIntegralType operator-(TIntegralType const&    lhs,
                          struct LIMIT_MAX const& rhs) noexcept {
    static_assert(std::is_integral<TIntegralType>::value,
                  "operator- only defined for constants and integral types");
    return lhs - rhs.get_value<TIntegralType>();
  }

  template <typename TIntegralType>
  TIntegralType operator-(struct LIMIT_MAX const& lhs,
                          TIntegralType const&    rhs) noexcept {
    static_assert(std::is_integral<TIntegralType>::value,
                  "operator- only defined for constants and integral types");
    return lhs.get_value<TIntegralType>() - rhs;
  }
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_INCLUDE_CONSTANTS_HPP_
