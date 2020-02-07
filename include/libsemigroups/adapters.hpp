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

#ifndef LIBSEMIGROUPS_ADAPTERS_HPP_
#define LIBSEMIGROUPS_ADAPTERS_HPP_

#include <algorithm>   // for std::sort
#include <functional>  // for std::equal_to
#include <vector>      // for std::vector

// #include "libsemigroups-config.hpp"  // for LIBSEMIGROUPS_DENSEHASHMAP

namespace libsemigroups {
  // Adapters with no default implementation

  //! Defined in ``adapters.hpp``.
  //!
  //! Specialisations of this struct should be stateless trivially default
  //! constructible with a call operator of signature `size_t
  //! operator()(TElementType const& x) const` (possibly `noexcept`, `inline`
  //! and/or `constexpr` also).
  //!
  //! The return value of the call operator ought to indicate the approximate
  //! complexity of multiplying two instances of `TElementType`, which may or
  //! may not depend on the parameter \p x. This is used, for example, by
  //! FroidurePin in some member functions to determine whether it is better to
  //! multiply elements or to follow a path in the Cayley graph.
  //!
  //! \tparam TElementType the type of the elements of a semigroup.
  //!
  //! The second template parameter exists for SFINAE in overload resolution.
  //!
  //! \par Used by:
  //! * FroidurePinTraits
  //!
  //! \par Example
  //! \code
  //! template <>
  //! struct Complexity<KBE> {
  //!   constexpr size_t operator()(KBE const&) const noexcept {
  //!     return LIMIT_MAX;
  //!   }
  //! };
  //! \endcode
  template <typename TElementType, typename = void>
  struct Complexity;

  //! Defined in ``adapters.hpp``.
  //!
  //! Specialisations of this struct should be stateless trivially default
  //! constructible with a call operator of signature `size_t
  //! operator()(TElementType const& x) const` (possibly `noexcept`, `inline`
  //! and/or `constexpr` also).
  //!
  //! The return value of the call operator ought to indicate the degree of a
  //! `TElementType` instance which may or may not depend on the parameter \p
  //! x. The degree of a permutation, for instance, would be the the number of
  //! points it acts on, the degree of a matrix is its dimension, and so on.
  //! This is used, for example, by SchreierSimsTraits in some member functions
  //! to determine whether it is known *a priori* that a permutation does not
  //! belong to the object, because it acts on too many points.
  //!
  //! \tparam TElementType the type of the elements of a semigroup.
  //!
  //! The second template parameter exists for SFINAE in overload resolution.
  //!
  //! \par Used by:
  //! * FroidurePinTraits
  //! * SchreierSimsTraits
  //!
  //! \par Example
  //! \code
  //! template <>
  //! struct Degree<BMat8> {
  //!   constexpr inline size_t operator()(BMat8 const&) const noexcept {
  //!     return 8;
  //!   }
  //! };
  //! \endcode
  template <typename TElementType, typename = void>
  struct Degree;

  //! Defined in ``adapters.hpp``.
  //!
  //! Specialisations of this struct should be stateless trivially default
  //! constructible with a call operator of signature `void
  //! operator()(TElementType& x, size_t n) const` (possibly `noexcept`,
  //! `inline` and/or `constexpr` also).
  //!
  //! The call operator should change the first argument in-place so that if `m
  //! = Degree<TElementType>()(x)`, then after the call to
  //! `IncreaseDegree<TElementType>()(x, n)`, `Degree<TElementType>()(x)`
  //! returns `m + n`. This only makes sense for certain types of elements,
  //! such as permutations, transformations, or matrices, and not for other
  //! types of object. In the latter case, the call operator should simply do
  //! nothing. This is used, for example, in the member function
  //! FroidurePin::closure, when one of the generators being added has degree
  //! larger than the existing generators.
  //!
  //! \tparam TElementType the type of the elements of a semigroup.
  //!
  //! The second template parameter exists for SFINAE in overload resolution.
  //!
  //! \par Used by:
  //! * FroidurePinTraits
  //!
  //! \par Example
  //! \code
  //! template <typename TIntegralType>
  //! struct IncreaseDegree<
  //!     TIntegralType,
  //!     typename std::enable_if<std::is_integral<TIntegralType>::value>::type>
  //!     {
  //!   void operator()(TIntegralType&, size_t) const noexcept {
  //!   }
  //! };
  //! \endcode
  template <typename TElementType, typename = void>
  struct IncreaseDegree;

  //! Defined in ``adapters.hpp``.
  //!
  //! Specialisations of this struct should be stateless trivially default
  //! constructible with two call operator of signatures:
  //!
  //! 1. `TElementType operator()(size_t n) const` (possibly `noexcept`,
  //!    `inline` and/or `constexpr` also) returning a multiplicative identity
  //!    element for the category `TElementType` and with
  //!    `Degree<TElementType>()(x)` equal to the parameter \p n. For example,
  //!    if `TElementType` is a type of n x n matrices, then this should return
  //!    the n x n identity matrix.
  //!
  //! 2. `TElementType operator()(T const&) const` (possibly `noexcept`,
  //!    `inline` and/or `constexpr` also). This could be implemented as:
  //!    \code
  //!    TElementType operator()(TElementType const& x) const noexcept {
  //!      return this->operator()(Degree<TElementType>()(x));
  //!    }
  //!    \endcode
  //!
  //! \tparam TElementType the type of the elements of a semigroup.
  //!
  //! The second template parameter exists for SFINAE in overload resolution.
  //!
  //! \par Used by:
  //! * ActionTraits
  //! * FroidurePinTraits
  //! * SchreierSimsTraits
  //!
  //! \par Example
  //! \code
  //! template <typename T>
  //! struct One<
  //!     T,
  //!     typename std::enable_if<std::is_base_of<PTransf16, T>::value>::type> {
  //!   T operator()(size_t = 0) const noexcept {
  //!     return T::one();
  //!   }
  //!
  //!   T operator()(T const&) const noexcept {
  //!     return T::one();
  //!   }
  //! };
  //! \endcode
  template <typename TElementType, typename = void>
  struct One;

  //! Defined in ``adapters.hpp``.
  //!
  //! Specialisations of this struct should be stateless trivially default
  //! constructible with a call operator of signature
  //! `void  operator()(TElementType& xy, TElementType const& x, TElementType
  //! const& y, size_t = 0)` (possibly `noexcept`, `inline` and/or `constexpr`
  //! also).
  //!
  //! The call operator should change \p xy in-place to be the product of \p x
  //! and \p y. The 4th parameter is optional and it can be used as an index
  //! for static thread local storage, that might be required for forming the
  //! product of \p x and \p y. The purpose of the 1st parameter is to avoid
  //! repeated allocations of memory to hold temporary products that are
  //! discarded soon after they are created.
  //!
  //! \tparam TElementType the type of the elements of a semigroup.
  //!
  //! The second template parameter exists for SFINAE in overload resolution.
  //!
  //! \par Used by:
  //! * ActionTraits
  //! * FroidurePinTraits
  //! * CongruenceByPairs
  //! * SchreierSimsTraits
  //!
  //! \par Example
  //! \code
  //! template <>
  //! struct Product<size_t> {
  //!   void operator()(size_t& xy, size_t x, size_t y, size_t = 0) const
  //!   noexcept {
  //!     xy = x * y;
  //!   }
  //! };
  //! \endcode
  template <typename TElementType, typename = void>
  struct Product;

  //! Defined in ``adapters.hpp``.
  //!
  //! Specialisations of this struct should be stateless trivially default
  //! constructible with a call operator of signature `TPointType
  //! operator()(TElementType const& x) const` (possibly
  //! `noexcept`, `inline` and/or `constexpr` also).
  //!
  //! The call operator should return the inverse of the element \p x under the
  //! assumption that \p x has an inverse (in the sense of groups).
  //! For example, if \p x is a permutation, then this would return its
  //! inverse. If \p x is a  permutation matrix of type BMat8, then this
  //! operator would return its transpose.
  //!
  //! \tparam TElementType the type of the elements of a semigroup.
  //!
  //! The second template parameter exists for SFINAE in overload resolution.
  //!
  //! \par Used by:
  //! * SchreierSimsTraits
  //!
  //! \par Example
  //! \code
  //! template <>
  //! struct Inverse<BMat8> {
  //!   inline BMat8 operator()(BMat8 const& x) const noexcept {
  //!     LIBSEMIGROUPS_ASSERT(x * x.transpose() == x.one());
  //!     return x.transpose();
  //!   }
  //! };
  //! \endcode
  template <typename TElementType, typename = void>
  struct Inverse;

  //! Defined in ``adapters.hpp``.
  //!
  //! Specialisations of this struct should be stateless trivially default
  //! constructible with a call operator of signature `void
  //! operator()(TPointType& res, TElementType const& x, TPointType const& pt)
  //! const` (possibly `noexcept`, `inline` and/or `constexpr` also).
  //!
  //! The call operator should change \p res in-place to contain the image of
  //! the point \p pt under the left action of the element \p x. The purpose of
  //! the 1st parameter is to avoid repeated allocations of memory to hold
  //! temporary points that are discarded soon after they are created.
  //!
  //! \tparam TElementType the type of the elements of a semigroup.
  //! \tparam TPointType the type of the points acted on.
  //!
  //! The third template parameter exists for SFINAE in overload resolution.
  //!
  //! \par Example
  //! \code
  //! template <>
  //! struct ImageLeftAction<BMat8, BMat8> {
  //!   void operator()(BMat8& res, BMat8 pt, BMat8 x) const noexcept {
  //!     res = (x * pt).col_space_basis();
  //!   }
  //! };
  //! \endcode
  template <typename TElementType, typename TPointType, typename = void>
  struct ImageLeftAction;

  //! Defined in ``adapters.hpp``.
  //!
  //! Specialisations of this struct should be stateless trivially default
  //! constructible with a call operator of signature:
  //! 1. `void operator()(TPointType& res, TElementType const& x, TPointType
  //!    const& pt) const` (possibly `noexcept`, `inline` and/or `constexpr`
  //!    also); or
  //! 2. `TPointType operator()(TElementType const& x, TPointType
  //!    const& pt) const` (possibly `noexcept`, `inline` and/or `constexpr`
  //!    also).
  //!
  //! In form (1): the call operator should change \p res in-place to
  //! contain the image of the point \p pt under the right action of the
  //! element \p x. The purpose of the 1st parameter is to avoid repeated
  //! allocations of memory to hold temporary points that are discarded soon
  //! after they are created.
  //!
  //! In form (2): the call operator should return the image of the point \p pt
  //! under the right action of the element \p x.
  //!
  //! \tparam TElementType the type of the elements of a semigroup.
  //! \tparam TPointType the type of the points acted on.
  //!
  //! The third template parameter exists for SFINAE in overload resolution.
  //!
  //! \par Used by:
  //! * Action (in form (1))
  //! * SchreierSimsTraits (in form (2))
  //!
  //! \par Example
  //! \code
  //! template <>
  //! struct ImageLeftAction<BMat8, BMat8> {
  //!   void operator()(BMat8& res, BMat8 pt, BMat8 x) const noexcept {
  //!     res = (x * pt).row_space_basis();
  //!   }
  //! };
  //! \endcode
  template <typename TElementType, typename TPointType, typename = void>
  struct ImageRightAction;

  // Adapters with default implementations

  //! Defined in ``adapters.hpp``.
  //!
  //! This type should be a stateless trivially default constructible with a
  //! call operator of signature `bool operator()(TValueType const&, TValueType
  //! const&)` (possibly `noexcept`, `inline` and/or `constexpr` also) for use
  //! with, for example,
  //! [std::unordered_map](https://en.cppreference.com/w/cpp/container/unordered_map).
  //!
  //! \tparam TValueType the type of objects to compare.
  //!
  //! The second template parameter exists for SFINAE in overload resolution.
  //!
  //! \par Used by:
  //! * ActionTraits
  //! * CongruenceByPairs
  //! * FroidurePinTraits
  //! * SchreierSimsTraits
  template <typename TValueType, typename = void>
  struct EqualTo {
    //! This call operator compares \p x and \p y using std::equal<TValueType>.
    //!
    //! \param x const reference to TValueType to compare with \p y.
    //! \param y const reference to TValueType to compare with \p x.
    //!
    //! \returns A `bool`.
    bool operator()(TValueType const& x, TValueType const& y) const {
      return std::equal_to<TValueType>()(x, y);
    }
  };

  //! Defined in ``adapters.hpp``.
  //!
  //! This type should be a stateless trivially default constructible
  //! with a call operator of signature `size_t operator()(TValueType
  //! const&)` for use with, for example,
  //! [std::unordered_map](https://en.cppreference.com/w/cpp/container/unordered_map).
  //!
  //! \tparam TValueType the type of objects to compare.
  //!
  //! The second template parameter exists for SFINAE in overload resolution.
  //!
  //! \par Used by:
  //! * ActionTraits
  //! * CongruenceByPairs
  //! * FroidurePinTraits
  //! * SchreierSimsTraits
  template <typename TValueType, typename = void>
  struct Hash {
    //! This call operator hashes \p x using std::hash<TValueType>.
    //!
    //! \param x the value to hash
    //!
    //! \returns A hash value for \p x, a value of type `size_t`.
    size_t operator()(TValueType const& x) const {
      return std::hash<TValueType>()(x);
    }
  };

  //! Defined in ``adapters.hpp``.
  //!
  //! This type should be a stateless trivially default constructible with a
  //! call operator of signature `bool operator()(TValueType const&, TValueType
  //! const&)` (possibly `noexcept`, `inline` and/or `constexpr` also) which
  //! defines a linear order on the objects of type TValueType
  //!
  //! \tparam TValueType the type of objects to compare.
  //!
  //! The second template parameter exists for SFINAE in overload resolution.
  //!
  //! \par Used by:
  //! * FroidurePinTraits
  template <typename TValueType, typename = void>
  struct Less {
    //! This call operator compares \p x and \p y using std::less<TValueType>.
    bool operator()(TValueType const& x, TValueType const& y) const {
      return std::less<TValueType>()(x, y);
    }
  };

  //! Defined in ``adapters.hpp``.
  //!
  //! This type should be a stateless trivially default constructible with a
  //! call operator of signature `void operator()(TValueType const&, TValueType
  //! const&)` (possibly `noexcept`, `inline` and/or `constexpr` also) which
  //! swaps its arguments.
  //!
  //! \tparam TValueType the type of objects to compare.
  //!
  //! The second template parameter exists for SFINAE in overload resolution.
  //!
  //! \par Used by:
  //! * ActionTraits
  //! * FroidurePinTraits
  //! * SchreierSimsTraits
  template <typename TValueType, typename = void>
  struct Swap {
    //! This call operator swaps \p x and \p y using std::swap<TValueType>.
    void operator()(TValueType& x, TValueType& y) {
      std::swap(x, y);
    }
  };

  //! Defined in ``adapters.hpp``.
  //!
  //! This is struct applies `ImageRightAction<TElementType, TPointType>` to
  //! every value in a container of type `TContainerType`.
  //!
  //! \tparam TElementType the type of the elements of the semigroup
  //! \tparam TPointType the type of the points acted on
  //! \tparam TContainerType a container of TPointType
  template <typename TElementType,
            typename TPointType,
            typename TContainerType = std::vector<TPointType>>
  struct OnTuples {
    static_assert(
        std::is_same<typename TContainerType::value_type, TPointType>::value,
        "the 3rd template parameter is not a container of objects of "
        "type the 2nd template parameter");
    //! This call operator changes \p res in-place to contain the image of
    //! the container \p pt of points of type TPointType, under the induced
    //! (left or right) action of the element \p x. The purpose of the 1st
    //! parameter is to avoid repeated allocations of memory to hold temporary
    //! points that are discarded soon after they are created.
    //!
    //! \param res a container of TPointType (such as std::vector or std::array)
    //! to hold the result
    //! \param pt  a container of TPointType
    //! \param x   an element of type TElementType
    //!
    //! \returns (None)
    //!
    //! \par Exception
    //! If ImageRightAction<TElementType, TPointType> throws when applied to any
    //! point in \p pt.
    //!
    //! \par Complexity
    //! `pt.size()` times the complexity of ImageRightAction<TElementType,
    //! TPointType>.
    //!
    //! \sa OnSets and ImageRightAction.
    void operator()(TContainerType&       res,
                    TContainerType const& pt,
                    TElementType const&   x) const {
      for (size_t i = 0; i < pt.size(); ++i) {
        ImageRightAction<TElementType, TPointType>()(res[i], pt[i], x);
      }
    }
  };

  //! Defined in ``adapters.hpp``.
  //!
  //! This is struct applies `OnTuples<TElementType, TPointType,
  //! TContainerType>` to  a container of `TContainerType` and then sorts it.
  //!
  //! \tparam TElementType the type of the elements of the semigroup
  //! \tparam TPointType the type of the points acted on
  //! \tparam TContainerType a container of TPointType
  // TODO(later) add a template param for sorting
  template <typename TElementType,
            typename TPointType,
            typename TContainerType = std::vector<TPointType>>
  struct OnSets {
    //! This call operator changes \p res in-place to contain the image of
    //! the container \p pt of points of type TPointType, under the induced
    //! (left or right) action of the element \p x. The purpose of the 1st
    //! parameter is to avoid repeated allocations of memory to hold temporary
    //! points that are discarded soon after they are created.
    //!
    //! \param res a container of TPointType (such as std::vector or std::array)
    //! to hold the result
    //! \param pt  a container of TPointType
    //! \param p   an element of type TElementType
    //!
    //! \returns (None)
    //!
    //! \par Exception
    //! Throws if OnTuples<TElementType, TPointType, TContainerType> throws.
    //!
    //! \par Complexity
    //! `pt.size()` times the complexity of ImageRightAction<TElementType,
    //! TPointType> and the cost plus the complexity of sorting \p res.
    //!
    //! \sa OnTuples and ImageRightAction.
    void operator()(TContainerType&       res,
                    TContainerType const& pt,
                    TElementType const&   p) const {
      OnTuples<TElementType, TPointType, TContainerType>()(res, pt, p);
      std::sort(res.begin(), res.end());
    }
  };

  // #ifdef LIBSEMIGROUPS_DENSEHASHMAP
  //   template <typename TElementType, typename = void>
  //   struct EmptyKey;
  // #endif
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_ADAPTERS_HPP_
