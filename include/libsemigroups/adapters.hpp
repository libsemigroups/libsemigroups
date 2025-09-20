//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019-2025 James D. Mitchell
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

#include <algorithm>    // for std::sort
#include <array>        // for array
#include <cstddef>      // for size_t
#include <functional>   // for std::equal_to
#include <iterator>     // for pair
#include <memory>       // for shared_ptr
#include <type_traits>  // for hash, is_same
#include <utility>      // for pair, hash
#include <vector>       // for std::vector

#include "config.hpp"  // for LIBSEMIGROUPS_SIZEOF_VOID_P

namespace libsemigroups {

  //! \defgroup adapters_group Adapters
  //!
  //! This file contains class templates for adapting a user-defined type for
  //! use with the algorithms in `libsemigroups`. There are explicit
  //! implementations for relevant element types from `libsemigroups` and
  //! `HPCombi`.
  //!
  //! ## No default implementation
  //!
  //! The following adapters have no default implementation for arbitrary types:
  //!
  //! * \ref Complexity
  //! * \ref Degree
  //! * \ref ImageLeftAction
  //! * \ref ImageRightAction
  //! * \ref IncreaseDegree
  //! * \ref Inverse
  //! * \ref Lambda
  //! * \ref LambdaValue
  //! * \ref One
  //! * \ref Product
  //! * \ref Rho
  //! * \ref RhoValue
  //!
  //! ## Default implementation
  //!
  //! The following adapters do have a default implementation for arbitrary
  //! types:
  //!
  //! * \ref EqualTo
  //! * \ref Hash
  //! * \ref Less
  //! * \ref OnTuples
  //! * \ref OnSets
  //! * \ref RankState
  //! * \ref Swap
  //!
  //! ## Explicit specializations
  //!
  //! The following explicit specializations of some of the adapters listed
  //! above are implemented in `libsemigroups` for the following types:
  //!
  //! * \ref adapters_bmat_group
  //! * \ref adapters_bmat8_group
  //! * \ref adapters_matrix_group
  //! * \ref adapters_hpcombi_group

  // This is not technically an adapter but put here for lack of a better place
  struct Noop {
    template <typename... Args>
    constexpr void operator()(Args...) const noexcept {}
  };

  struct ReturnFalse {
    template <typename... Args>
    [[nodiscard]] constexpr bool operator()(Args...) const noexcept {
      return false;
    }
  };

  //! \brief Adapter for the complexity of multiplication.
  //!
  //! Defined in `adapters.hpp`.
  //!
  //! Specialisations of this struct should be stateless trivially default
  //! constructible with a call operator of signature
  //! `size_t operator()(Element const& x) const` (possibly `noexcept`, `inline`
  //! and/or `constexpr` also).
  //!
  //! The return value of the call operator ought to indicate the approximate
  //! complexity of multiplying two instances of `Element`, which may or
  //! may not depend on the parameter \p x. This is used, for example, by
  //! FroidurePin in some member functions to determine whether it is better to
  //! multiply elements or to follow a path in the Cayley graph.
  //!
  //! \tparam Element the type of the elements of a semigroup.
  //!
  //! The second template parameter exists for SFINAE.
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
  template <typename Element, typename = void>
  struct Complexity;

  //! \brief Adapter for the degree of an element.
  //!
  //! Defined in `adapters.hpp`.
  //!
  //! Specialisations of this struct should be stateless trivially default
  //! constructible with a call operator of signature
  //! `size_t operator()(Element const& x) const` (possibly `noexcept`, `inline`
  //! and/or `constexpr` also).
  //!
  //! The return value of the call operator ought to indicate the degree of a
  //! `Element` instance which may or may not depend on the parameter
  //! \p x. The degree of a permutation, for instance, would be the the number
  //! of points it acts on, the degree of a matrix is its dimension, and so on.
  //! This is used, for example, by SchreierSimsTraits in some member functions
  //! to determine whether it is known *a priori* that a permutation does not
  //! belong to the object, because it acts on too many points.
  //!
  //! \tparam Element the type of the elements of a semigroup.
  //!
  //! The second template parameter exists for SFINAE.
  //!
  //! \par Used by:
  //! * FroidurePinTraits
  //! * KoniecznyTraits
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
  template <typename Element, typename = void>
  struct Degree;

  //! \brief Adapter for increasing the degree of an element.
  //!
  //! Defined in `adapters.hpp`.
  //!
  //! Specialisations of this struct should be stateless trivially default
  //! constructible with a call operator of signature
  //! `void operator()(Element& x, size_t n) const` (possibly `noexcept`,
  //! `inline` and/or `constexpr` also).
  //!
  //! The call operator should change the first argument in-place so that if
  //! `m = Degree<Element>()(x)`, then after the call to
  //! `IncreaseDegree<Element>()(x, n)`, `Degree<Element>()(x)`
  //! returns `m + n`. This only makes sense for certain types of elements,
  //! such as permutations, transformations, or matrices, and not for other
  //! types of object. In the latter case, the call operator should simply do
  //! nothing. This is used, for example, in the member function
  //! FroidurePin::closure, when one of the generators being added has degree
  //! larger than the existing generators.
  //!
  //! \tparam Element the type of the elements of a semigroup.
  //!
  //! The second template parameter exists for SFINAE.
  //!
  //! \par Used by:
  //! * FroidurePinTraits
  //!
  //! \par Example
  //! \code
  //! template <typename Integral>
  //! struct IncreaseDegree<
  //!     Integral,
  //!     typename std::enable_if<std::is_integral<Integral>::value>::type>
  //!     {
  //!   void operator()(Integral&, size_t) const noexcept {
  //!   }
  //! };
  //! \endcode
  template <typename Element, typename = void>
  struct IncreaseDegree;

  //! \brief Adapter for the identity element of the given type.
  //!
  //! Specialisations of this struct should be stateless trivially default
  //! constructible with two call operator of signatures:
  //!
  //! 1. `Element operator()(size_t n) const` (possibly `noexcept`,
  //!    `inline` and/or `constexpr` also) returning a multiplicative identity
  //!    element for the category `Element` and with
  //!    `Degree<Element>()(x)` equal to the parameter \p n. For example,
  //!    if `Element` is a type of n x n matrices, then this should return
  //!    the n x n identity matrix.
  //!
  //! 2. `Element operator()(T const&) const` (possibly `noexcept`,
  //!    `inline` and/or `constexpr` also). This could be implemented as:
  //!    \code
  //!    Element operator()(Element const& x) const noexcept {
  //!      return this->operator()(Degree<Element>()(x));
  //!    }
  //!    \endcode
  //!
  //! \tparam Element the type of the elements of a semigroup.
  //!
  //! The second template parameter exists for SFINAE.
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
  template <typename Element, typename = void>
  struct One;

  //! \brief Adapter for the product of two elements.
  //!
  //! Defined in `adapters.hpp`.
  //!
  //! Specialisations of this struct should be stateless trivially default
  //! constructible with a call operator of signature
  //! `void  operator()(Element&, Element const&, Element const&, size_t = 0)`
  //! (possibly `noexcept`, `inline` and/or `constexpr` also).
  //!
  //! The call operator should change \p xy in-place to be the product of \p x
  //! and \p y. The 4th parameter is optional and it can be used as an index
  //! for static thread local storage, that might be required for forming the
  //! product of \p x and \p y. The purpose of the 1st parameter is to avoid
  //! repeated allocations of memory to hold temporary products that are
  //! discarded soon after they are created.
  //!
  //! \tparam Element the type of the elements of a semigroup.
  //!
  //! The second template parameter exists for SFINAE.
  //!
  //! \par Used by:
  //! * ActionTraits
  //! * FroidurePinTraits
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
  template <typename Element, typename = void>
  struct Product;

  //! \brief Adapter for the inverse of an element.
  //!
  //! Defined in `adapters.hpp`.
  //!
  //! Specialisations of this struct should be stateless trivially default
  //! constructible with a call operator of signature
  //! `Point operator()(Element const& x) const` (possibly
  //! `noexcept`, `inline` and/or `constexpr` also).
  //!
  //! The call operator should return the inverse of the element \p x under the
  //! assumption that \p x has an inverse (in the sense of groups).
  //! For example, if \p x is a permutation, then this would return its
  //! inverse. If \p x is a  permutation matrix of type BMat8, then this
  //! operator would return its transpose.
  //!
  //! \tparam Element the type of the elements of a semigroup.
  //!
  //! The second template parameter exists for SFINAE.
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
  template <typename Element, typename = void>
  struct Inverse;

  //! \brief Adapter for the value of a left action.
  //!
  //! Defined in `adapters.hpp`.
  //!
  //! Specialisations of this struct should be stateless trivially default
  //! constructible with a call operator of signature
  //! `void operator()(Point& res, Element const& x, Point const& pt) const`
  //! (possibly `noexcept`, `inline` and/or `constexpr` also).
  //!
  //! The call operator should change \p res in-place to contain the image of
  //! the point \p pt under the left action of the element \p x. The purpose of
  //! the 1st parameter is to avoid repeated allocations of memory to hold
  //! temporary points that are discarded soon after they are created.
  //!
  //! \tparam Element the type of the elements of a semigroup.
  //! \tparam Point the type of the points acted on.
  //!
  //! The third template parameter exists for SFINAE.
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
  template <typename Element, typename Point, typename = void>
  struct ImageLeftAction;

  //! \brief Adapter for the value of a right action.
  //!
  //! Defined in `adapters.hpp`.
  //!
  //! Specialisations of this struct should be stateless trivially default
  //! constructible with a call operator of signature:
  //! 1. `void operator()(Point&, Element const&, Point const&) const`
  //! (possibly `noexcept`, `inline` and/or `constexpr`
  //!    also); or
  //! 2. `Point operator()(Element const&, Point const&) const` (possibly
  //! `noexcept`, `inline` and/or `constexpr` also).
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
  //! \tparam Element the type of the elements of a semigroup.
  //! \tparam Point the type of the points acted on.
  //!
  //! The third template parameter exists for SFINAE.
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
  template <typename Element, typename Point, typename = void>
  struct ImageRightAction;

  //! \brief Adapter for testing equality.
  //!
  //! Defined in `adapters.hpp`.
  //!
  //! This type should be a stateless trivially default constructible with a
  //! call operator of signature
  //! `bool operator()(Value const&, Value const&)` (possibly `noexcept`,
  //! `inline` and/or `constexpr` also) for use with, for example,
  //! std::unordered_map.
  //!
  //! \tparam Value the type of objects to compare.
  //!
  //! The second template parameter exists for SFINAE.
  //!
  //! \par Used by:
  //! * ActionTraits
  //! * FroidurePinTraits
  //! * SchreierSimsTraits
  template <typename Value, typename = void>
  struct EqualTo {
    //! \brief Compare \p x and \p y with std::equal_to.
    //!
    //! This call operator compares \p x and \p y using std::equal_to with
    //! template parameter \c Value.
    //!
    //! \param x const reference to \c Value to compare with \p y.
    //! \param y const reference to \c Value to compare with \p x.
    //!
    //! \returns A \c bool.
    bool operator()(Value const& x, Value const& y) const {
      return std::equal_to<Value>()(x, y);
    }
  };

  //! \brief Adapter for hashing.
  //!
  //! Defined in `adapters.hpp`.
  //!
  //! This type should be a stateless trivially default constructible
  //! with a call operator of signature
  //! `size_t operator()(Value const&)` for use with, for example,
  //! std::unordered_map.
  //!
  //! \tparam Value the type of objects to compare.
  //!
  //! The second template parameter exists for SFINAE.
  //!
  //! \par Used by:
  //! * ActionTraits
  //! * FroidurePinTraits
  //! * SchreierSimsTraits
  template <typename Value, typename = void>
  struct Hash {
    //! \brief Hash \p x using std::hash.
    //!
    //! This call operator hashes \p x using std::hash with template parameter
    //! \c Value.
    //!
    //! \param x the value to hash.
    //!
    //! \returns A hash value for \p x, a value of type `size_t`.
    size_t operator()(Value const& x) const {
      return std::hash<Value>()(x);
    }
  };

  ////////////////////////////////////////////////////////////////////////
  // Hash specializations
  ////////////////////////////////////////////////////////////////////////

  //! \brief Specialised adapter for hashing vectors.
  //!
  //! Defined in `adapters.hpp`.
  //!
  //! This type is stateless trivially default constructible
  //! and has a call operator of signature
  //! `size_t operator()(std::vector<T> const&)` for use with, for example,
  //! std::unordered_map.
  //!
  //! \tparam T the type of objects to compare in the vector.
  //!
  //! \par Used by KoniecznyTraits.
  template <typename T>
  struct Hash<std::vector<T>> {
    //! \brief Hashes \p vec.
    //!
    //! This call operator hashes \p vec.
    //!
    //! \param vec the value to hash.
    //!
    //! \returns A hash value for \p vec, a value of type `size_t`.
    //!
    //! \sa \ref Hash::operator()() "Hash\<Value\>::operator()(Value const&)"
    size_t operator()(std::vector<T> const& vec) const {
      return Hash<std::vector<T>, Hash<T>>()(vec);
    }
  };

  // TODO(2) same for array!

  //! \brief Specialised adapter for hashing vectors with non-standard hasher.
  //!
  //! Defined in `adapters.hpp`.
  //!
  //! This type is stateless trivially default constructible
  //! and has a call operator of signature
  //! `size_t operator()(std::vector<T> const&)` for use with, for example,
  //! std::unordered_map.
  //!
  //! \tparam T the type of objects to compare in the vector.
  //! \tparam Hasher the type of the hasher.
  //!
  // TODO(later) What uses this?
  template <typename T, typename Hasher>
  struct Hash<std::vector<T>, Hasher> {
    //! \brief Hashes \p vec.
    //!
    //! This call operator hashes \p vec.
    //!
    //! \param vec the value to hash.
    //!
    //! \returns A hash value for \p vec, a value of type `size_t`.
    size_t operator()(std::vector<T> const& vec) const {
      size_t val = 0;
      for (T const& x : vec) {
        val ^= Hasher()(x) + 0x9e3779b97f4a7c16 + (val << 6) + (val >> 2);
      }
      return val;
    }
  };

  //! \brief Specialised adapter for hashing arrays.
  //!
  //! Defined in `adapters.hpp`.
  //!
  //! This type is stateless trivially default constructible
  //! and has a call operator of signature
  //! `size_t operator()(std::array<T, N> const&)` for use with, for example,
  //! std::unordered_map.
  //!
  //! \tparam T the type of objects to compare in the array.
  //!
  //! \par Used by
  // TODO(later) What uses this?
  template <typename T, size_t N>
  struct Hash<std::array<T, N>> {
    //! \brief Hashes \p ar.
    //!
    //! This call operator hashes \p ar.
    //!
    //! \param ar the value to hash.
    //!
    //! \returns A hash value for \p ar, a value of type `size_t`.
    size_t operator()(std::array<T, N> const& ar) const {
      size_t val = 0;
      for (T const& x : ar) {
        val ^= Hash<T>()(x) + 0x9e3779b97f4a7c16 + (val << 6) + (val >> 2);
      }
      return val;
    }
  };

  //! \brief Specialised adapter for hashing pairs.
  //!
  //! Defined in `adapters.hpp`.
  //!
  //! This type is stateless trivially default constructible
  //! and has a call operator of signature
  //! `size_t operator()(std::pair<S, T> const&)` for use with, for example,
  //! std::unordered_map.
  //!
  //! \tparam S the type of first object in the pair.
  //! \tparam T the type of second object in the pair.
  //!
  //! \par Used by
  // TODO(later) What uses this?
  template <typename S, typename T>
  struct Hash<std::pair<S, T>> {
    //! \brief Hashes \p x.
    //!
    //! This call operator hashes \p x.
    //!
    //! \param x the value to hash.
    //!
    //! \returns A hash value for \p x, a value of type `size_t`.
    size_t operator()(std::pair<S, T> const& x) const noexcept {
      size_t val = 0;
      val ^= Hash<S>()(x.first) + 0x9e3779b97f4a7c16 + (val << 6) + (val >> 2);
      val ^= Hash<T>()(x.second) + 0x9e3779b97f4a7c16 + (val << 6) + (val >> 2);
      return val;
    }
  };

  //! \brief Specialised adapter for hashing pairs of `size_t`s.
  //!
  //! Defined in `adapters.hpp`.
  //!
  //! This type is stateless trivially default constructible
  //! and has a call operator of signature
  //! `size_t operator()(std::vector<T> const&)` for use with, for example,
  //! std::unordered_map.
  //!
  //! \par Used by Konieczny.
  template <>
  struct Hash<std::pair<size_t, size_t>> {
    //! \brief Hashes \p x.
    //!
    //! This call operator hashes \p x.
    //!
    //! \param x the value to hash.
    //!
    //! \returns A hash value for \p x, a value of type `size_t`.
    size_t operator()(std::pair<size_t, size_t> const& x) const noexcept {
      // TODO(2) this is a very bad hash when the values are larger than the
      // shift width
#if LIBSEMIGROUPS_SIZEOF_VOID_P == 8
      return (x.first << 32) + x.second;
#else
      return (x.first << 16) + x.second;
#endif
    }
  };

  //! \brief Adapter for comparisons.
  //!
  //! Defined in `adapters.hpp`.
  //!
  //! This type should be a stateless trivially default constructible with a
  //! call operator of signature
  //! `bool operator()(Value const&, Value const&)` (possibly `noexcept`,
  //! `inline` and/or `constexpr` also) which defines a linear order on the
  //! objects of type Value.
  //!
  //! \tparam Value the type of objects to compare.
  //!
  //! The second template parameter exists for SFINAE.
  //!
  //! \par Used by:
  //! * FroidurePinTraits
  template <typename Value, typename = void>
  struct Less {
    //! \brief Compare if \p x is less than \p y.
    //!
    //! This call operator compares \p x and \p y using std::less with template
    //! parameter \c Value.
    //!
    //! \param x const reference to \c Value to compare with \p y.
    //! \param y const reference to \c Value to compare with \p x.
    //!
    //! \returns A \c bool.
    bool operator()(Value const& x, Value const& y) const {
      return std::less<Value>()(x, y);
    }
  };

  //! \brief Adapter for swapping.
  //!
  //! Defined in `adapters.hpp`.
  //!
  //! This type should be a stateless trivially default constructible with a
  //! call operator of signature `void operator()(Value&, Value&)` (possibly
  //! `noexcept`, `inline` and/or `constexpr` also) which swaps its arguments.
  //!
  //! \tparam Value the type of objects to swap.
  //!
  //! The second template parameter exists for SFINAE.
  //!
  //! \par Used by:
  //! * ActionTraits
  //! * FroidurePinTraits
  //! * SchreierSimsTraits
  template <typename Value, typename = void>
  struct Swap {
    //! \brief Swap \p x and \p y.
    //!
    //! This call operator swaps \p x and \p y using std::swap.
    //!
    //! \param x reference to \c Value to swap with \p y.
    //! \param y reference to \c Value to swap with \p x.
    void operator()(Value& x, Value& y) {
      std::swap(x, y);
    }
  };

  //! \brief Adapter for calculating right actions of each element in a
  //! container.
  //!
  //! Defined in `adapters.hpp`.
  //!
  //! This struct applies `ImageRightAction<Element, Point>` to every value in a
  //! container of type `Container`.
  //!
  //! \tparam Element the type of the elements of the semigroup.
  //! \tparam Point the type of the points acted on.
  //! \tparam Container a container of Point.
  template <typename Element,
            typename Point,
            typename Container = std::vector<Point>>
  struct OnTuples {
    static_assert(std::is_same<typename Container::value_type, Point>::value,
                  "the 3rd template parameter is not a container of objects of "
                  "type the 2nd template parameter");
    //! \brief Calculate right actions of each element in a container.
    //!
    //! This call operator changes \p res in-place to contain the image of
    //! the container \p pt of points of type \c Point, under the induced
    //! right action of the element \p x. The purpose of the 1st parameter is to
    //! avoid repeated allocations of memory to hold temporary points that are
    //! discarded soon after they are created.
    //!
    //! \param res a container of \c Point (such as std::vector or std::array)
    //! to hold the result.
    //! \param pt  a container of \c Point.
    //! \param x   an element of type \c Element.
    //!
    //! \complexity
    //! `pt.size()` times the complexity of
    //! `ImageRightAction<Element, Point>`.
    //!
    //! \sa OnSets and ImageRightAction.
    void operator()(Container&       res,
                    Container const& pt,
                    Element const&   x) const {
      for (size_t i = 0; i < pt.size(); ++i) {
        ImageRightAction<Element, Point>()(res[i], pt[i], x);
      }
    }
  };

  //! \brief Adapter for calculating right actions of each element in a
  //! container.
  //!
  //! Defined in `adapters.hpp`.
  //!
  //! This struct applies
  //! `OnTuples<Element, Point, Container>` to  a container of `Container` and
  //! then sorts it.
  //!
  //! \tparam Element the type of elements.
  //! \tparam Point the type of the points acted on.
  //! \tparam Container a container of \p Point (defaults to std::vector with
  //! template parameter \c Point).
  // TODO(2) add a template param for sorting
  template <typename Element,
            typename Point,
            typename Container = std::vector<Point>>
  struct OnSets {
    //! \brief Calculate right actions of each element in a container, then
    //! sort.
    //!
    //! This call operator changes \p res in-place to contain the image of
    //! the container \p pt of points of type \c Point, under the induced
    //! right action of the element \p p. The purpose of the 1st parameter is to
    //! avoid repeated allocations of memory to hold temporary points that are
    //! discarded soon after they are created.
    //!
    //! Once the right action has been calculated, the container is sorted using
    //! std::sort.
    //!
    //! \param res a container of \c Point (such as std::vector or std::array)
    //! to hold the result.
    //! \param pt  a container of \c Point.
    //! \param p an element of type \c Element.
    //!
    //! \complexity
    //! `pt.size()` times the complexity of
    //! `ImageRightAction<Element, Point>` and the cost plus the complexity of
    //! sorting \p res.
    //!
    //! \sa OnTuples and ImageRightAction.
    void operator()(Container&       res,
                    Container const& pt,
                    Element const&   p) const {
      OnTuples<Element, Point, Container>()(res, pt, p);
      std::sort(res.begin(), res.end());
    }
  };

  /////////////////////////////////////////////////////////////////////
  // KONIECZNY THINGS
  /////////////////////////////////////////////////////////////////////

  //! \brief Adapter for lambda functions.
  //!
  //! Defined in `adapters.hpp`.
  //!
  //! This type should be a stateless struct with a single typedef \c type
  //! representing the type that should be used or returned by a \c Lambda
  //! struct.
  //!
  //! \tparam Element the type of elements.
  //!
  //! The second template parameter exists for SFINAE.
  //!
  //! \par Used by:
  //! * KoniecznyTraits.
  // Note: this has to exist in order to allow different lambda functions based
  // on type.
  template <typename Element, typename = void>
  struct LambdaValue;

  //! \brief Adapter for rho functions.
  //!
  //! Defined in `adapters.hpp`.
  //!
  //! This type should be a stateless struct with a single typedef \c type
  //! representing the type that should be used or returned by a \c Rho
  //! struct.
  //!
  //! \tparam Element the type of elements.
  //!
  //! The second template parameter exists for SFINAE.
  //!
  //! \par Used by:
  //! * KoniecznyTraits.
  // Note: this has to exist in order to allow different rho functions based
  // on type.
  template <typename Element, typename = void>
  struct RhoValue;

  //! \brief Adapter for the action on LambdaValue's.
  //!
  //! Defined in `adapters.hpp`.
  //!
  //! This type should be a stateless trivially default constructible
  //! with an operator of signature
  //! `void operator()(Point&, Element const&)`, which should modify the
  //! first argument in-place to contain the lambda value of the second
  //! argument. The kernel of the lambda function should be Green's
  //! \f$\mathscr{L}\f$-relation on the semigroup in question.
  //!
  //! \tparam Element the type of elements.
  //! \tparam Point the type of the lambda points.
  //!
  //! The third template parameter exists for SFINAE.
  //!
  //! \par Used by:
  //! * KoniecznyTraits.
  template <typename Element, typename Point, typename = void>
  struct Lambda;

  //! \brief Adapter for the action on RhoValue's.
  //!
  //! Defined in `adapters.hpp`.
  //!
  //! This type should be a stateless trivially default constructible
  //! with an operator of signature
  //! `void operator()(Point&, Element const&)`, which  should modify the
  //! first argument in-place to contain the rho value of the second argument.
  //! The kernel of the rho function should be Green's
  //! \f$\mathscr{R}\f$-relation on the semigroup in question.
  //!
  //! \tparam Element the type of elements.
  //! \tparam Point the type of the rho points.
  //!
  //! The third template parameter exists for SFINAE.
  //!
  //! \par Used by:
  //! * KoniecznyTraits.
  template <typename Element, typename Point, typename = void>
  struct Rho;

  // TODO(now) does this brief make sense?
  //! \brief Base class for states for ranks.
  //!
  //! Defined in `adapters.hpp`.
  //!
  //! Specialisations of this class should have a typedef \c type representing
  //! the type of data stored in the class, a 0-parameter constructor, and a
  //! constructor of signature `template<typename T> RankState(T, T)` where \c T
  //! is the type of a const iterator to a container of \c Element.
  //!
  //! The default declaration provided here has `type = void` and indicates
  //! that no RankState object is required by Rank with template
  //! parameter \c Element.
  //!
  //! \tparam Element the type of elements.
  //!
  //! The second template parameter exists for SFINAE.
  //!
  //! \par Used by:
  //! * KoniecznyTraits.
  template <typename Element, typename = void>
  class RankState {
   public:
    //! By default no additional state is required to calculate
    //! Rank<Element>; this is represented by \c type being \c void.
    using type = void;

    //! Default constructor; does nothing.
    RankState() noexcept = default;

    //! Iterator constructor; does nothing.
    template <typename T>
    RankState(T, T) noexcept {}

    // Other constructors are deleted.
    //! Deleted.
    RankState(RankState const&) = default;
    //! Deleted.
    RankState(RankState&&) = delete;
    //! Deleted.
    RankState& operator=(RankState const&) = delete;
    //! Deleted.
    RankState& operator=(RankState&&) = delete;
  };

  //! \brief Adapter for calculating ranks.
  //!
  //! Defined in `adapters.hpp`.
  //!
  //! This type should be default constructible and a call operator of signature
  //! `size_t operator()(Element const&)` if no additional data is required
  //! to compute the rank, or a call operator of signature
  //! `size_t operator()(State<Element> const&, Element const&)` if
  //! additional data is required.
  //!
  //! The call operator should return the rank of the element given as
  //! argument. This must satisfy the following properties:
  //! * \f$\operatorname{rank}\f$ should agree with the \f$D\f$-order on the
  //!   semigroup; that is, if \f$D_x \leq D_y\f$, then
  //!   \f$\operatorname{rank}(x) \leq \operatorname{rank}(y)\f$,
  //! * if \f$D_x \leq D_y\f$ and \f$\operatorname{rank}(x) =
  //!   \operatorname{rank}(y)\f$, then \f$D_x = D_y\f$.
  //!
  //! \tparam Element the type of elements.
  //! \tparam State the type of the data required to compute ranks of
  //!         Elements; defaults to \c RankState<Element>.
  //!
  //! The third template parameter exists for SFINAE.
  //!
  //! \par Used by:
  //! * KoniecznyTraits.
  template <typename Element,
            typename State = RankState<Element>,
            typename       = void>
  struct Rank;

}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_ADAPTERS_HPP_
