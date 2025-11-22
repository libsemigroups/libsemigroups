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

// TODO check the doc, return values and parameters are missing in some cases.
// TODO constexpr where possible

// This file contains declarations of specializations of the class adapters in
// adapters.hpp for the element types in HPCombi.

#ifndef LIBSEMIGROUPS_HPCOMBI_HPP_
#define LIBSEMIGROUPS_HPCOMBI_HPP_

// Must include config.hpp so that LIBSEMIGROUPS_HPCOMBI_ENABLED
// is defined, if so specified at during configure.
#include "config.hpp"  // for LIBSEMIGROUPS_HPCOMBI_ENABLED

// TODO fix the warnings in HPCombi that these suppressions are hear to
// silence.
#if defined(LIBSEMIGROUPS_HPCOMBI_ENABLED)
#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#pragma GCC diagnostic ignored "-Wswitch-default"
#endif
#include "hpcombi/hpcombi.hpp"  // for `HPCombi::Perm16`, ...
#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

#include <algorithm>         // for max_element
#include <cstddef>           // for size_t
#include <cstdint>           // for uint8_t
#include <initializer_list>  // for begin, end, initializer_list
#include <numeric>           // for iota
#include <string_view>       // for basic_string_view
#include <type_traits>       // for is_base_of_v, enable_if_t, is_int...
#include <unordered_map>     // for swap, unordered_map
#include <utility>           // for forward, move
#include <vector>            // for vector

#include "adapters.hpp"   // for Complexity, Degree, ...
#include "debug.hpp"      // for LIBSEMIGROUPS_ASSERT
#include "exception.hpp"  // for LIBSEMIGROUPS_EXCEPTION
#include "is-transf.hpp"  // for throw_if_not_ptransf etc
#include "types.hpp"      // for enable_if_is_same
#endif

#if defined(LIBSEMIGROUPS_HPCOMBI_ENABLED) \
    || defined(LIBSEMIGROUPS_PARSED_BY_DOXYGEN)
namespace libsemigroups {

  //! \defgroup hpcombi_group HPCombi
  //!
  //! This page contains links to the documentation of the parts of
  //! \libsemigroups related to \hpcombi.

  //! \defgroup adapters_hpcombi_group Adapters for HPCombi
  //! \ingroup hpcombi_group
  //!
  //! This page contains the documentation of the functionality in
  //! \libsemigroups that adapts the types in \hpcombi for use with the
  //! algorithms in libsemigroups. The functionality in this section is only
  //! available if libsemigroups is compiled with \hpcombi support enabled.
  //!
  //! @{

  //! \brief Specialisation of the Complexity adapter for subclasses of
  //! `HPCombi::PTransf16`.
  //!
  //! Defined in `hpcombi.hpp`.
  //!
  //! Specialization of the Complexity adapter for subclasses of
  //! `HPCombi::PTransf16`.
  //!
  //! \tparam Thing a derived class of `HPCombi::PTransf16`.
  //!
  //! \sa Complexity.
  template <typename Thing>
  struct Complexity<
      Thing,
      std::enable_if_t<std::is_base_of_v<HPCombi::PTransf16, Thing>>> {
    //! \brief Returns 0.
    //!
    //! This function returns a value that is used to determine the complexity
    //! of multiplication of elements of type \p Thing. Since \hpcombi types
    //! have extremely fast multiplication this function returns `0`.
    //!
    //! \param x an instance of a derived class of `HPCombi::PTransf16`.
    //!
    //! \returns The complexity of multiplication, constant with value `0`.
    //!
    //! \exceptions
    //! \noexcept
    constexpr size_t operator()(Thing const& x) const noexcept {
      (void) x;
      return 0;
    }
  };

  //! \brief Specialization of the Degree adapter for `HPCombi::PTransf16`.
  //!
  //! Defined in `hpcombi.hpp`.
  //!
  //! Specialization of the Degree adapter for subclasses of
  //! `HPCombi::PTransf16`.
  //!
  //! \tparam Thing a derived class of `HPCombi::PTransf16`.
  //!
  //! \sa Degree.
  template <typename Thing>
  struct Degree<
      Thing,
      std::enable_if_t<std::is_base_of_v<HPCombi::PTransf16, Thing>>> {
    //! \brief Returns 16.
    //!
    //! Returns 16.
    //!
    //! \param x an instance of a derived class of `HPCombi::PTransf16`.
    //!
    //! \returns The degree, which is always `16`.
    //!
    //! \exceptions
    //! \noexcept
    constexpr size_t operator()(Thing const& x) const noexcept {
      (void) x;
      return 16;
    }
  };

  //! \brief Specialisation of the One adapter for `HPCombi::PTransf16`.
  //!
  //! Defined in `hpcombi.hpp`.
  //!
  //! Specialization of the One adapter for subclasses of
  //! `HPCombi::PTransf16`.
  //!
  //! \tparam Thing a derived class of `HPCombi::PTransf16`.
  //!
  //! \sa One.
  template <typename Thing>
  struct One<Thing,
             std::enable_if_t<std::is_base_of_v<HPCombi::PTransf16, Thing>>> {
    //! \brief Returns the identity for a derived class of `HPCombi::PTransf16`.
    //!
    //! Returns the identity \p Thing.
    //!
    //! \param n unused, for interface consistency only (defaults to `0`).
    //!
    //! \returns The identity of type \p Thing.
    //!
    //! \exceptions
    //! \noexcept
    Thing operator()(size_t n = 0) const noexcept {
      (void) n;
      return Thing::one();
    }

    //! \brief Returns the identity for a derived class of `HPCombi::PTransf16`.
    //!
    //! Returns the identity \p Thing.
    //!
    //! \param x unused, for interface consistency only.
    //!
    //! \returns The identity of type \p Thing.
    //!
    //! \exceptions
    //! \noexcept
    Thing operator()(Thing const& x) const noexcept {
      (void) x;
      return Thing::one();
    }
  };

  //! \brief Specialisation of the Product adapter for derived classes of
  //! `HPCombi::PTransf16`.
  //!
  //! Defined in `hpcombi.hpp`.
  //!
  //! Specialization of the Product adapter for subclasses of
  //! `HPCombi::PTransf16`.
  //!
  //! \tparam Thing a derived class of `HPCombi::PTransf16`.
  //!
  //! \note \hpcombi implements composition of functions from right to left,
  //! whereas \libsemigroups assumes composition is left to right.
  //!
  //! \sa Product.
  template <typename Thing>
  struct Product<
      Thing,
      std::enable_if_t<std::is_base_of_v<HPCombi::PTransf16, Thing>>> {
    //! \brief Modifies \p xy in-place to be the product of \p x and \p y.
    //!
    //! Modifies \p xy in-place to be the product of \p x and \p y.
    //!
    //! \param xy the \p Thing to hold the product.
    //! \param x the first \p Thing to multiply.
    //! \param y the \p Thing to multiply \p x by.
    //! \param deg unused, for interface consistency only (defaults to `0`).
    //!
    //! \exceptions
    //! \noexcept
    void operator()(Thing&       xy,
                    Thing const& x,
                    Thing const& y,
                    size_t       deg = 0) const noexcept {
      (void) deg;
      xy = y * x;
    }
  };

  //! \brief Specialization of the Swap adapter for `HPCombi::PTransf16`.
  //!
  //! Defined in `hpcombi.hpp`.
  //!
  //! Specialization of the Swap adapter for subclasses of
  //! `HPCombi::PTransf16`.
  //!
  //! \tparam Thing a derived class of `HPCombi::PTransf16`.
  //!
  //! \sa Swap.
  template <typename Thing>
  struct Swap<Thing,
              std::enable_if_t<std::is_base_of_v<HPCombi::PTransf16, Thing>>> {
    //! \brief Swap \p x and \p y.
    //!
    //! Swap \p x and \p y using std::swap.
    //!
    //! \param x the first \p Thing to swap.
    //! \param y the \p Thing to swap \p x with.
    //!
    //! \exceptions
    //! \noexcept
    void operator()(Thing& x, Thing& y) const noexcept {
      std::swap(x, y);
    }
  };

  //! \brief Specialization of the IncreaseDegree adapter for
  //! `HPCombi::PTransf16` and derived classes.
  //!
  //! Defined in `hpcombi.hpp`.
  //!
  //! Specialization of the IncreaseDegree adapter for `HPCombi::PTransf16` and
  //! derived classes.
  //!
  //! \tparam Thing the type.
  //!
  //! \sa IncreaseDegree.
  template <typename Thing>
  struct IncreaseDegree<
      Thing,
      std::enable_if_t<std::is_base_of_v<HPCombi::PTransf16, Thing>>> {
    //! \brief Does nothing.
    //!
    //! Does nothing since `HPCombi` types are of fixed degree.
    //!
    //! \param x the Thing to increase the degree of (unused!).
    //! \param n the amount to increase the degree by (unused!).
    //!
    //! \exceptions
    //! \noexcept
    inline constexpr void operator()(Thing const& x, size_t n) const noexcept {
      (void) x;
      (void) n;
    }
  };

  //! \brief Specialization of the Inverse adapter for `HPCombi::PTransf16`.
  //!
  //! Defined in `hpcombi.hpp`.
  //!
  //! Specialization of the Inverse adapter for `HPCombi::Perm16`.
  //!
  //! \sa Inverse.
  template <>
  struct Inverse<HPCombi::Perm16> {
    //! \brief Returns the inverse of \p x.
    //!
    //! Returns the inverse of \p x.
    //!
    //! \param x the permutation.
    //!
    //! \returns The inverse of \p x.
    //!
    //! \exceptions
    //! \noexcept
    HPCombi::Perm16 operator()(HPCombi::Perm16 const& x) const noexcept {
      return x.inverse();
    }
  };

  //! \brief Specialization of the ImageRightAction adapter for
  //! `HPCombi::PTransf16`.
  //!
  //! Defined in `hpcombi.hpp`.
  //!
  //! Specialization of the  ImageRightAction for `HPCombi::Perm16`.
  //!
  //! \tparam Int the type of the points being acted on.
  //!
  //! \sa ImageRightAction.
  template <typename Int>
  struct ImageRightAction<HPCombi::Perm16,
                          Int,
                          std::enable_if_t<std::is_integral_v<Int>>> {
    //! \brief Stores the image of \p pt under \p p.
    //!
    //! Modifies \p res in-place to store the image of \p pt under \p p.
    //!
    //! \param res the value to hold the output.
    //! \param pt the point.
    //! \param x the permutation.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \warning No checks are performed on \p pt, but it should be in the range
    //! \f$[0, 15)\f$.
    void operator()(Int&                   res,
                    Int const&             pt,
                    HPCombi::Perm16 const& x) const noexcept {
      LIBSEMIGROUPS_ASSERT(pt < 16);
      res = static_cast<Int>(x[pt]);
    }

    //! \brief Stores the image of \p pt under \p p.
    //!
    //! This function returns the image of \p pt under \p p.
    //!
    //! \param pt the point.
    //! \param x the permutation.
    //!
    //! \returns The image of \p pt under \p x.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \warning No checks are performed on \p pt, but it should be in the range
    //! \f$[0, 15)\f$.
    Int operator()(Int const& pt, HPCombi::Perm16 const& x) const noexcept {
      LIBSEMIGROUPS_ASSERT(pt < 16);
      return x[pt];
    }
  };

  //! \brief Specialization of the ImageRightAction adapter for
  //! `HPCombi::PTransf16`.
  //!
  //! Defined in `hpcombi.hpp`.
  //!
  //! Specialization of the ImageRightAction adapter for `HPCombi::PPerm16`.
  //!
  //! \sa ImageRightAction.
  template <>
  struct ImageRightAction<HPCombi::PPerm16, HPCombi::PPerm16> {
    //! \brief Stores the idempotent \f$(xy) ^ {-1}xy\f$ in \p res.
    //!
    //! Modifies \p res in-place to store the idempotent \f$(yx) ^ {-1}yx\f$ in
    //! \p res where \f$y\f$ is \p pt.
    //!
    //! \param res the value to hold the output.
    //! \param pt the point being acted upon.
    //! \param x the partial permutation that is acting.
    //!
    //! \exceptions
    //! \noexcept
    void operator()(HPCombi::PPerm16&       res,
                    HPCombi::PPerm16 const& pt,
                    HPCombi::PPerm16 const& x) const noexcept {
      res = (x * pt).left_one();
    }
  };

  //! \brief Specialization of the ImageLeftAction adapter for
  //! `HPCombi::PTransf16`.
  //!
  //! Defined in `hpcombi.hpp`.
  //!
  //! Specialization of the ImageLeftAction adapter for `HPCombi::PPerm16`.
  //!
  //! \sa ImageLeftAction.
  template <>
  struct ImageLeftAction<HPCombi::PPerm16, HPCombi::PPerm16> {
    //! \brief Stores the idempotent \f$xy(xy) ^ {-1}\f$ in \p res.
    //!
    //! Modifies \p res in-place to store the idempotent \f$xy(xy) ^ {-1}\f$ in
    //! \p res where \f$y\f$ is \p pt.
    //!
    //! \param res the value to hold the output.
    //! \param pt the point being acted upon.
    //! \param x the partial permutation that is acting.
    //!
    //! \exceptions
    //! \noexcept
    void operator()(HPCombi::PPerm16&       res,
                    HPCombi::PPerm16 const& pt,
                    HPCombi::PPerm16 const& x) const noexcept {
      res = (pt * x).right_one();
    }
  };

  //! \brief Specialization of the Complexity adapter for `HPCombi::BMat8`.
  //!
  //! Defined in `hpcombi.hpp`.
  //!
  //! Specialization of the Complexity adapter for instances of
  //! `HPCombi::BMat8`.
  //!
  //! \sa Complexity.
  template <>
  struct Complexity<HPCombi::BMat8> {
    //! \brief Returns 0.
    //!
    //! This function returns a value that is used to determine the complexity
    //! of multiplication of `HPCombi::BMat8` objects. Since \hpcombi types
    //! have extremely fast multiplication this function returns `0`.
    //!
    //! \param x the boolean matrix.
    //!
    //! \returns The complexity of multiplication, constant with value `0`.
    //!
    //! \exceptions
    //! \noexcept
    constexpr inline size_t operator()(HPCombi::BMat8 const& x) const noexcept {
      (void) x;
      return 0;
    }
  };

  //! \brief Specialization of the Degree adapter for `HPCombi::BMat8`.
  //!
  //! Defined in `hpcombi.hpp`.
  //!
  //! Specialization of the Degree adapter for instances of `HPCombi::BMat8`.
  //!
  //! \sa Degree.
  template <>
  struct Degree<HPCombi::BMat8> {
    //! \brief Returns 8.
    //!
    //! Returns 8.
    //!
    //! \param x the boolean matrix.
    //!
    //! \returns The degree, which is always `8`.
    //!
    //! \exceptions
    //! \noexcept
    constexpr inline size_t operator()(HPCombi::BMat8 const& x) const noexcept {
      (void) x;
      return 8;
    }
  };

  //! \brief Specialization of the IncreaseDegree adapter for
  //! `HPCombi::BMat8`.
  //!
  //! Defined in `hpcombi.hpp`.
  //!
  //! Specialization of the IncreaseDegree adapter for instances of
  //! `HPCombi::BMat8`.
  //!
  //! \sa IncreaseDegree.
  template <>
  struct IncreaseDegree<HPCombi::BMat8> {
    //! \brief Does nothing.
    //!
    //! Does nothing since `HPCombi` types are of fixed degree.
    //!
    //! \param x the boolean matrix to increase the degree of (unused!).
    //! \param n the amount to increase the degree by (unused!).
    //!
    //! \exceptions
    //! \noexcept
    inline void operator()(HPCombi::BMat8 const& x, size_t n) const noexcept {
      (void) x;
      (void) n;
    }
  };

  //! \brief Specialization of the One adapter for `HPCombi::BMat8`.
  //!
  //! Defined in `hpcombi.hpp`.
  //!
  //! Specialization of the One adapter for instances of `HPCombi::BMat8`.
  //!
  //! \sa One.
  template <>
  struct One<HPCombi::BMat8> {
    //! \brief Returns the identity `HPCombi::BMat8`.
    //!
    //! Returns the identity `HPCombi::BMat8`.
    //!
    //! \param n unused, for interface consistency only (defaults to `0`).
    //!
    //! \returns The identity of type `HPCombi::BMat8`.
    //!
    //! \exceptions
    //! \noexcept
    inline HPCombi::BMat8 operator()(size_t n = 0) const noexcept {
      (void) n;
      return HPCombi::BMat8::one();
    }

    //! \brief Returns the identity `HPCombi::BMat8`.
    //!
    //! Returns the identity `HPCombi::BMat8`.
    //!
    //! \param x unused, for interface consistency only.
    //!
    //! \returns The identity of type `HPCombi::BMat8`.
    //!
    //! \exceptions
    //! \noexcept
    inline HPCombi::BMat8 operator()(HPCombi::BMat8 const& x) const noexcept {
      (void) x;
      return x.one();
    }
  };

  //! \brief Specialization of the Product adapter for `HPCombi::BMat8`.
  //!
  //! Defined in `hpcombi.hpp`.
  //!
  //! Specialization of the Product adapter for instances of `HPCombi::BMat8`.
  //!
  //! \sa Product.
  template <>
  struct Product<HPCombi::BMat8> {
    //! \brief Modifies \p xy in-place to be the product of \p x and \p y.
    //!
    //! Modifies \p xy in-place to be the product of \p x and \p y.
    //!
    //! \param xy the `HPCombi::BMat8` to hold the product.
    //! \param x the first `HPCombi::BMat8` to multiply.
    //! \param y the `HPCombi::BMat8` to multiply \p x by.
    //! \param deg unused, for interface consistency only (defaults to `0`).
    //!
    //! \exceptions
    //! \noexcept
    inline void operator()(HPCombi::BMat8&       xy,
                           HPCombi::BMat8 const& x,
                           HPCombi::BMat8 const& y,
                           size_t                deg = 0) const noexcept {
      (void) deg;
      xy = x * y;
    }
  };

  //! \brief Specialization of the ImageRightAction adapter for
  //! `HPCombi::BMat8`.
  //!
  //! Defined in `hpcombi.hpp`.
  //!
  //! Specialization of the ImageRightAction adapter for instances of
  //! `HPCombi::BMat8`.
  //!
  //! \sa ImageRightAction.
  // HERE in doc review
  template <>
  struct ImageRightAction<HPCombi::BMat8, HPCombi::BMat8> {
    //! \brief Store the image of \p pt under the right action of \p x.
    //!
    //! Changes \p res in-place to hold the image of \p pt under the right
    //! action of \p x.
    void operator()(HPCombi::BMat8&       res,
                    HPCombi::BMat8 const& pt,
                    HPCombi::BMat8 const& x) const noexcept {
      res = (pt * x).row_space_basis();
    }
  };

  //! \brief Specialization of the ImageLeftAction adapter for
  //! `HPCombi::BMat8`.
  //!
  //! Defined in `hpcombi.hpp`.
  //!
  //! Specialization of the ImageLeftAction adapter for instances of
  //! `HPCombi::BMat8`.
  //!
  //! \sa ImageLeftAction.
  template <>
  struct ImageLeftAction<HPCombi::BMat8, HPCombi::BMat8> {
    //! \brief Stores the image of \p pt under the left action of \p x.
    //!
    //! Changes \p res in-place to hold the image of \p pt under the left
    //! action of \p x.
    void operator()(HPCombi::BMat8& res,
                    HPCombi::BMat8  pt,
                    HPCombi::BMat8  x) const noexcept {
      res = (x * pt).col_space_basis();
    }
  };

  //! \brief Specialization of the Inverse adapter for `HPCombi::BMat8`.
  //!
  //! Defined in `hpcombi.hpp`.
  //!
  //! Specialization of the Inverse adapter for instances of `HPCombi::BMat8`.
  //!
  //! \sa Inverse.
  template <>
  struct Inverse<HPCombi::BMat8> {
    //! \brief Returns the group inverse of \p x.
    //!
    //! Returns the group inverse of \p x.
    inline HPCombi::BMat8 operator()(HPCombi::BMat8 const& x) const noexcept {
      // The following assertion fails because x.one() is the 8x8 identity but
      // x * x.transpose() is not always.
      // LIBSEMIGROUPS_ASSERT(x * x.transpose() == x.one());
      return x.transpose();
    }
  };

  ////////////////////////////////////////////////////////////////////////
  // Konieczny adapters - `HPCombi::BMat8`
  ////////////////////////////////////////////////////////////////////////

  //! \brief Specialization of the LambdaValue adapter for `HPCombi::BMat8`.
  //!
  //! Specialization of the LambdaValue adapter for instances of
  //! `HPCombi::BMat8`.
  //!
  //! \sa LambdaValue
  template <>
  struct LambdaValue<HPCombi::BMat8> {
    //! \brief The type of Lambda value.
    //!
    //! The type of Lambda values for `HPCombi::BMat8` is also
    //! `HPCombi::BMat8`; this provides an efficient representation of row
    //! space bases.
    using type = HPCombi::BMat8;
  };

  //! \brief Specialization of the RhoValue adapter for `HPCombi::BMat8`.
  //!
  //! Specialization of the RhoValue adapter for instances of
  //! `HPCombi::BMat8`.
  //!
  //! \sa RhoValue
  template <>
  struct RhoValue<HPCombi::BMat8> {
    //! \brief The type of Rho values for `HPCombi::BMat8`.
    //!
    //! The type of Rho values for `HPCombi::BMat8` is also
    //! `HPCombi::BMat8`; this provides an efficient representation of row
    //! space bases.
    using type = HPCombi::BMat8;
  };

  //! \brief Specialization of the Lambda adapter for `HPCombi::BMat8`.
  //!
  //! Specialization of the Lambda adapter for instances of `HPCombi::BMat8`.
  //!
  //! \sa Lambda.
  template <>
  struct Lambda<HPCombi::BMat8, HPCombi::BMat8> {
    //! \brief Store the lambda value of \p x as used in the Konieczny
    //! algorithm.
    //!
    //! Modify \p res in-place to the lambda value of \p x as used in the
    //! Konieczny algorithm; for `HPCombi::BMat8` this is the row space basis.
    void operator()(HPCombi::BMat8&       res,
                    HPCombi::BMat8 const& x) const noexcept {
      res = x.row_space_basis();
    }
  };

  //! \brief Specialization of the Rho adapter for `HPCombi::BMat8`.
  //!
  //! Specialization of the Rho adapter for instances of
  //! `HPCombi::BMat8`.
  //!
  //! \sa Rho.
  template <>
  struct Rho<HPCombi::BMat8, HPCombi::BMat8> {
    //! \brief Store the rho value of \p x as used in the Konieczny algorithm.
    //!
    //! Modify \p res in-place to the rho value of \p x as used in the
    //! Konieczny algorithm; for `HPCombi::BMat8` this is the column space
    //! basis.
    void operator()(HPCombi::BMat8&       res,
                    HPCombi::BMat8 const& x) const noexcept {
      res = x.col_space_basis();
    }
  };

  //! \brief Specialization of the Rank adapter for `HPCombi::BMat8`.
  //!
  //! Specialization of the Rank adapter for instances of
  //! `HPCombi::BMat8`.
  //!
  //! \sa Rank.
  template <>
  struct Rank<HPCombi::BMat8> {
    //! \brief Returns the rank of \p x as used in the Konieczny algorithm.
    //!
    //! Returns the rank of \p x as used in the Konieczny algorithm; for BMat8
    //! this is the size of the row space.
    inline size_t operator()(HPCombi::BMat8 const& x) const noexcept {
      return x.row_space_size();
    }
  };

  ////////////////////////////////////////////////////////////////////////
  // Konieczny adapters - `HPCombi::PPerm16`
  ////////////////////////////////////////////////////////////////////////

  //! \brief Specialization of the LambdaValue adapter for `HPCombi::PPerm16`.
  //!
  //! Specialization of the LambdaValue adapter for instances of
  //! `HPCombi::PPerm16`.
  //!
  //! \sa LambdaValue.
  template <>
  struct LambdaValue<HPCombi::PPerm16> {
    //! \brief The type of Lambda values.
    //!
    //! The type of Lambda values for PPerm16 is also PPerm16; this provides
    //! an efficient representation of image sets.
    using type = HPCombi::PPerm16;
  };

  //! \brief Specialization of the RhoValue adapter for `HPCombi::PPerm16`.
  //!
  //! Specialization of the RhoValue adapter for instances of
  //! `HPCombi::PPerm16`.
  //!
  //! \sa RhoValue.
  template <>
  struct RhoValue<HPCombi::PPerm16> {
    //! \brief The type of Rho values.
    //!
    //! The type of Rho values for PPerm16 is also PPerm16; this provides an
    //! efficient representation of domain sets.
    using type = HPCombi::PPerm16;
  };

  //! \brief Specialization of the Lambda adapter for `HPCombi::PPerm16`.
  //!
  //! Specialization of the Lambda adapter for instances of
  //! `HPCombi::PPerm16`.
  //!
  //! \sa Lambda, ImageRightAction.
  template <>
  struct Lambda<HPCombi::PPerm16, HPCombi::PPerm16> {
    //! \b Stores the idempotent \f$(xy) ^ {-1}xy\f$.
    //!
    //! Modifies \p res in-place to store the idempotent \f$(xy) ^ {-1}xy\f$.
    //!
    //! \exceptions
    //! \noexcept
    void operator()(HPCombi::PPerm16&       res,
                    HPCombi::PPerm16 const& x) const noexcept {
      res = x.left_one();
    }
  };

  //! \brief Specialization of the Rho adapter for `HPCombi::PPerm16`.
  //!
  //! Specialization of the Rho adapter for instances of `HPCombi::PPerm16`.
  //!
  //! \sa Rho, ImageLeftAction.
  template <>
  struct Rho<HPCombi::PPerm16, HPCombi::PPerm16> {
    //! Stores the idempotent \f$(xy) ^ {-1}xy\f$ in \p res.
    //! \exceptions
    //! \noexcept
    void operator()(HPCombi::PPerm16&       res,
                    HPCombi::PPerm16 const& x) const noexcept {
      res = x.right_one();
    }
  };

  ////////////////////////////////////////////////////////////////////////
  // Konieczny adapters - Transf16
  ////////////////////////////////////////////////////////////////////////

  //! \brief Specialization of the ImageRightAction adapter for
  //! `HPCombi::Transf16`.
  //!
  //! Defined in `hpcombi.hpp`.
  //!
  //! Specialization of the ImageRightAction adapter for `HPCombi::Transf16`.
  //!
  //! \sa ImageRightAction.
  template <>
  struct ImageRightAction<HPCombi::Transf16, HPCombi::PTransf16> {
    //! \brief Store the image of \p x under the right action of \p y.
    //!
    //! Modifies \p res in-place to hold the image of \p x under the right
    //! action of \p y.
    void operator()(HPCombi::PTransf16&       res,
                    HPCombi::Transf16 const&  x,
                    HPCombi::PTransf16 const& y) const noexcept {
      res = (y * static_cast<HPCombi::PTransf16>(static_cast<HPCombi::epu8>(x)))
                .left_one();
    }
  };

  //! \brief Specialization of the ImageLeftAction adapter for
  //! `HPCombi::Transf16`.
  //!
  //! Defined in `hpcombi.hpp`.
  //!
  //! Specialization of the ImageLeftAction adapter for `HPCombi::Transf16`.
  //!
  //! \sa ImageLeftAction.
  template <>
  struct ImageLeftAction<HPCombi::Transf16, HPCombi::Vect16> {
    //! \brief Store image of \p x under the left action of \p y.
    //!
    //! Modifies \p res in-place to hold the image of \p x under the left
    //! action of \p y.
    void operator()(HPCombi::Vect16&         res,
                    HPCombi::Transf16 const& x,
                    HPCombi::Vect16 const&   y) const noexcept;
  };

  //! \brief Specialization of the LambdaValue adapter for
  //! `HPCombi::Transf16`.
  //!
  //! Specialization of the LambdaValue adapter for instances of
  //! `HPCombi::Transf16`.
  //!
  //! \sa LambdaValue
  template <>
  struct LambdaValue<HPCombi::Transf16> {
    //! \brief The type of Lambda values.
    //!
    //! The type of Lambda values for Transf16 is PTransf16; this provides an
    //! efficient representation of image sets.
    using type = HPCombi::PTransf16;
  };

  //! \brief Specialization of the RhoValue adapter for `HPCombi::Transf16`.
  //!
  //! Specialization of the RhoValue adapter for instances of
  //! `HPCombi::Transf16`.
  //!
  //! \sa RhoValue
  template <>
  struct RhoValue<HPCombi::Transf16> {
    //! \brief The type of Rho value for `HPCombi::Transf16`.
    //!
    //! The type of Rho values for Transf16 is a Vect16; this provides
    //! an efficient representation of the kernel.
    using type = HPCombi::Vect16;
  };

  //! \brief Specialization of the Lambda adapter for `HPCombi::Transf16`.
  //!
  //! Defined in `hpcombi.hpp`.
  //!
  //! Specialization of the Lambda adapter for `HPCombi::Transf16`.
  //!
  //! \sa ImageRightAction, Lambda.
  template <>
  struct Lambda<HPCombi::Transf16, HPCombi::PTransf16> {
    //! \brief Stores the identity function on the image of \p x.
    //!
    //! Modifies \p res in-place to store the identity function on the image
    //! of
    //! \p x.
    //! \exceptions
    //! \noexcept
    void operator()(HPCombi::PTransf16&      res,
                    HPCombi::Transf16 const& x) const noexcept {
      res = x.left_one();
    }
  };

  //! \brief Specialization of the Rho adapter for `HPCombi::Transf16`.
  //!
  //! Defined in `hpcombi.hpp`.
  //!
  //! Specialization of the Lambda adapter for `HPCombi::Transf16`.
  //!
  //! \sa ImageLeftAction.
  template <>
  struct Rho<HPCombi::Transf16, HPCombi::Vect16> {
    //! \brief Stores the kernel of \p x.
    //!
    //! Modifies \p res in-place to store the kernel of \p x.
    //!
    //! \exceptions
    //! \noexcept
    void operator()(HPCombi::Vect16&         res,
                    HPCombi::Transf16 const& x) const noexcept;
  };

  ////////////////////////////////////////////////////////////////////////
  // Konieczny adapters - PTransf16
  ////////////////////////////////////////////////////////////////////////

  //! \brief Specialization of the ImageRightAction adapter for
  //! `HPCombi::PTransf16`.
  //!
  //! Defined in `hpcombi.hpp`.
  //!
  //! Specialization of the ImageRightAction adapter for `HPCombi::PTransf16`.
  //!
  //! \sa ImageRightAction.
  template <>
  struct ImageRightAction<HPCombi::PTransf16, HPCombi::PTransf16> {
    //! \brief Store the image of \p x under the right action of \p y.
    //!
    //! Modifies \p res in-place to hold the image of \p pt under the right
    //! action of \p x.
    void operator()(HPCombi::PTransf16&       res,
                    HPCombi::PTransf16 const& pt,
                    HPCombi::PTransf16 const& x) const noexcept {
      // Note that this is the opposite of what's stated above because
      // composition is the wrong way around in HPCombi
      res = (x * pt).left_one();
    }
  };

  //! \brief Specialization of the ImageLeftAction adapter for
  //! `HPCombi::PTransf16`.
  //!
  //! Defined in `hpcombi.hpp`.
  //!
  //! Specialization of the ImageLeftAction adapter for `HPCombi::PTransf16`.
  //!
  //! \sa ImageLeftAction.
  template <>
  struct ImageLeftAction<HPCombi::PTransf16, HPCombi::Vect16> {
    //! \brief Store image of \p x under the left action of \p y.
    //!
    //! Modifies \p res in-place to hold the image of \p x under the left
    //! action of \p y.
    // We cannot use "right_one" here because in HPCombi, right_one for a
    // partial transformation is not an L-class invariant unfortunately.
    void operator()(HPCombi::Vect16&          res,
                    HPCombi::PTransf16 const& x,
                    HPCombi::Vect16 const&    y) const noexcept;
  };

  //! \brief Specialization of the LambdaValue adapter for
  //! `HPCombi::PTransf16`.
  //!
  //! Specialization of the LambdaValue adapter for instances of
  //! `HPCombi::PTransf16`.
  //!
  //! \sa LambdaValue
  template <>
  struct LambdaValue<HPCombi::PTransf16> {
    //! \brief The type of Lambda values.
    //!
    //! The type of Lambda values for PTransf16 is PTransf16; this provides an
    //! efficient representation of image sets.
    using type = HPCombi::PTransf16;
  };

  //! \brief Specialization of the RhoValue adapter for `HPCombi::PTransf16`.
  //!
  //! Specialization of the RhoValue adapter for instances of
  //! `HPCombi::PTransf16`.
  //!
  //! \sa RhoValue
  template <>
  struct RhoValue<HPCombi::PTransf16> {
    //! \brief The type of Rho value for `HPCombi::PTransf16`.
    //!
    //! The type of Rho values for PTransf16 is a Vect16; this provides
    //! an efficient representation of the kernel.
    using type = HPCombi::Vect16;
  };

  //! \brief Specialization of the Lambda adapter for `HPCombi::PTransf16`.
  //!
  //! Defined in `hpcombi.hpp`.
  //!
  //! Specialization of the Lambda adapter for `HPCombi::PTransf16`.
  //!
  //! \sa ImageRightAction, Lambda.
  template <>
  struct Lambda<HPCombi::PTransf16, HPCombi::PTransf16> {
    //! \brief Stores the identity function on the image of \p x.
    //!
    //! Modifies \p res in-place to store the identity function on the image of
    //! \p x.
    //!
    //! \exceptions
    //! \noexcept
    void operator()(HPCombi::PTransf16&       res,
                    HPCombi::PTransf16 const& x) const noexcept {
      // This is the opposite of what might be expected because multiplication
      // is the wrong way around in HPCombi
      res = x.left_one();
    }
  };

  //! \brief Specialization of the Rho adapter for `HPCombi::PTransf16`.
  //!
  //! Defined in `hpcombi.hpp`.
  //!
  //! Specialization of the Lambda adapter for `HPCombi::PTransf16`.
  //!
  //! \sa ImageLeftAction.
  template <>
  struct Rho<HPCombi::PTransf16, HPCombi::Vect16> {
    //! \brief Stores the kernel of \p x.
    //!
    //! Modifies \p res in-place to store the kernel of \p x.
    //!
    //! \exceptions
    //! \noexcept
    void operator()(HPCombi::Vect16&          res,
                    HPCombi::PTransf16 const& x) const noexcept;
  };

  ////////////////////////////////////////////////////////////////////////
  // Konieczny adapters - generic
  ////////////////////////////////////////////////////////////////////////

  //! \brief Specialization of the Rank adapter for \hpcombi types.
  //!
  //! Defined in `hpcombi.hpp`.
  //!
  //! Specialization of the Rank adapter for \hpcombi types.
  template <typename T>
  struct Rank<T,
              std::enable_if_t<std::is_base_of_v<HPCombi::PTransf16, T>,
                               RankState<T>>> {
    //! \brief Returns the rank of \p x as used in the Konieczny algorithm.
    //!
    //! Returns the rank of \p x as used in the Konieczny algorithm; for
    //! `HPCombi::Transf16`, `HPCombi::PTransf16`, and `HPCombi::PPerm16` this
    //! is the size of the image set.
    //!
    //! \param x the partial transformation.
    //!
    //! \returns the rank of \p x.
    //!
    //! \exceptions
    //! \noexcept
    inline size_t operator()(T const& x) const noexcept {
      return x.rank();
    }
  };

  //! @}

  //! \defgroup make_hpcombi_group The \`make\` function for HPCombi
  //! \ingroup hpcombi_group
  //!
  //! This page contains documentation for the `make` function overloads for
  //! safely constructing \hpcombi types.

  //! \ingroup make_hpcombi_group
  //!
  //! \brief Construct a `HPCombi::PTransf16` from universal reference and
  //! check.
  //!
  //! Constructs a `HPCombi::PTransf16` initialized using the container \p cont
  //! as follows: the image of the point \c i under the transformation is the
  //! value in position \c i of the container \p cont.
  //!
  //! \tparam Container type of the container.
  //!
  //! \param cont the container.
  //!
  //! \returns A `HPCombi::PTransf16` instance.
  //!
  //! \throw LibsemigroupsException if any of the following hold:
  //! * the size of \p cont exceeds \c 16;
  //! * any value in \p cont exceeds `cont.size()`.
  //!
  //! \complexity
  //! Linear in the size of the container \p cont.
#ifndef LIBSEMIGROUPS_PARSED_BY_DOXYGEN
  template <typename Return, typename Container>
  [[nodiscard]] enable_if_is_same<Return, HPCombi::PTransf16>
  make(Container&& cont);
#else
  template <typename Container>
  [[nodiscard]] HPCombi::PTransf16 make<HPCombi::PTransf16>(Container&& cont);
#endif

  //! \ingroup make_hpcombi_group
  //!
  //! \brief Construct a `HPCombi::Transf16` from universal reference and
  //! check.
  //!
  //! Constructs a `HPCombi::Transf16` initialized using the container \p cont
  //! as follows: the image of the point \c i under the transformation is the
  //! value in position \c i of the container \p cont.
  //!
  //! \tparam Container type of the container.
  //!
  //! \param cont the container.
  //!
  //! \returns A `HPCombi::Transf16` instance.
  //!
  //! \throw LibsemigroupsException if any of the following hold:
  //! * the size of \p cont exceeds \c 16;
  //! * any value in \p cont exceeds `cont.size()`.
  //!
  //! \complexity
  //! Linear in the size of the container \p cont.
#ifndef LIBSEMIGROUPS_PARSED_BY_DOXYGEN
  template <typename Return, typename Container>
  [[nodiscard]] enable_if_is_same<Return, HPCombi::Transf16>
  make(Container&& cont);
#else
  template <typename Container>
  [[nodiscard]] HPCombi::Transf16 make<HPCombi::Transf16>(Container&& cont);
#endif

  //! \ingroup make_hpcombi_group
  //!
  //! \brief Construct a `HPCombi::Perm16` from universal reference and
  //! check.
  //!
  //! Constructs a `HPCombi::Perm16` initialized using the container \p cont as
  //! follows: the image of the point \c i under the permutation is the
  //! value in position \c i of the container \p cont.
  //!
  //! \tparam Container type of the container.
  //!
  //! \param cont the container.
  //!
  //! \returns A `HPCombi::Perm16` instance.
  //!
  //! \throw LibsemigroupsException if any of the following hold:
  //! * the size of \p cont exceeds \c 16;
  //! * any value in \p cont exceeds `cont.size()`.
  //!
  //! \complexity
  //! Linear in the size of the container \p cont.
#ifndef LIBSEMIGROUPS_PARSED_BY_DOXYGEN
  template <typename Return, typename Container>
  [[nodiscard]] enable_if_is_same<Return, HPCombi::Perm16>
  make(Container&& cont);
#else
  template <typename Container>
  [[nodiscard]] HPCombi::Perm16 make<HPCombi::Perm16>(Container&& cont);
#endif

  //! \ingroup make_hpcombi_group
  //!
  //! \brief Construct a `HPCombi::PPerm16` from container and check.
  //!
  //! Constructs a `HPCombi::PPerm16` initialized using the container \p cont as
  //! follows: the image of the point \c i under the partial perm is the
  //! value in position \c i of the container \p cont.
  //!
  //! \tparam Container type of the container.
  //!
  //! \param cont the container.
  //!
  //! \returns A `HPCombi::PPerm16` instance.
  //!
  //! \throw LibsemigroupsException if any of the following hold:
  //! * the size of \p cont exceeds \c 16;
  //! * any value in \p cont exceeds `cont.size()`;
  //! * any value in \p cont is repeated.
  //!
  //! \complexity
  //! Linear in the size of the container \p cont.
#ifndef LIBSEMIGROUPS_PARSED_BY_DOXYGEN
  template <typename Return, typename Container>
  [[nodiscard]] enable_if_is_same<Return, HPCombi::PPerm16>
  make(Container&& cont);
#else
  template <typename Container>
  [[nodiscard]] HPCombi::PPerm16 make<HPCombi::PPerm16>(Container&& cont);
#endif

  //! \ingroup make_hpcombi_group
  //!
  //! \brief Construct a `HPCombi::PTransf16` from domain, range, and degree,
  //! and check.
  //!
  //! Constructs a partial transformation of degree \p deg such that
  //! `f[dom[i]] = ran[i]` for all \c i and which is `0xFF` on every other
  //! value in the range \f$[0, M)\f$.
  //!
  //! \param dom the domain.
  //! \param ran the range.
  //! \param deg the degree (defaults to `16`).
  //!
  //! \throws LibsemigroupsException if any of the following fail to hold:
  //! * the value \p deg is greater than `16`
  //! * \p dom and \p ran do not have the same size
  //! * any value in \p dom or \p ran is greater than \p deg
  //! * there are repeated entries in \p dom.
  //!
  //! \complexity
  //! Linear in the size of \p dom.
#ifndef LIBSEMIGROUPS_PARSED_BY_DOXYGEN
  template <typename Return>
  [[nodiscard]] enable_if_is_same<Return, HPCombi::PTransf16>
  make(std::vector<uint8_t> const& dom,
       std::vector<uint8_t> const& ran,
       size_t                      deg = 16);
#else
  [[nodiscard]] HPCombi::PTransf16
  make<HPCombi::PTransf16>(std::vector<uint8_t> const& dom,
                           std::vector<uint8_t> const& ran,
                           size_t                      deg = 16);
#endif

  //! \ingroup make_hpcombi_group
  //!
  //! \brief Construct a `HPCombi::PPerm16` from domain, range, and degree, and
  //! check.
  //!
  //! Constructs a partial perm of degree \p deg such that `f[dom[i]] =
  //! ran[i]` for all \c i and which is `0xFF` on every other value in the
  //! range \f$[0, deg)\f$.
  //!
  //!
  //! \param dom the domain.
  //! \param ran the range.
  //! \param deg the degree.
  //!
  //! \throws LibsemigroupsException if any of the following fail to hold:
  //! * the value \p deg is greater than `16`
  //! * \p dom and \p ran do not have the same size
  //! * any value in \p dom or \p ran is greater than \p deg
  //! * there are repeated entries in \p dom or \p ran.
  //!
  //! \complexity
  //! Linear in the size of \p dom.
#ifndef LIBSEMIGROUPS_PARSED_BY_DOXYGEN
  template <typename Return>
  [[nodiscard]] enable_if_is_same<Return, HPCombi::PPerm16>
  make(std::vector<uint8_t> const& dom,
       std::vector<uint8_t> const& ran,
       size_t                      deg = 16);
#else
  [[nodiscard]] HPCombi::PPerm16
  make<HPCombi::PPerm16>(std::vector<uint8_t> const& dom,
                         std::vector<uint8_t> const& ran,
                         size_t                      deg = 16);
#endif

  //! \ingroup make_hpcombi_group
  //!
  //! \brief Construct a `HPCombi::PPerm16` from domain, range, and degree, and
  //! check.
  //!
  //! Constructs a partial perm of degree \p M such that
  //! `f[dom[i]] = ran[i]` for all \c i and which is `0xFF` on every other
  //! value in the range \f$[0, M)\f$.
  //!
  //! \tparam Int the type of the integers in the initializer lists.
  //!
  //! \param dom the domain.
  //! \param ran the range.
  //! \param M the degree.
  //!
  //! \throws LibsemigroupsException if any of the following fail to hold:
  //! * the value \p M is not compatible with the template parameter \p N
  //! * \p dom and \p ran do not have the same size
  //! * any value in \p dom or \p ran is greater than \p M
  //! * there are repeated entries in \p dom or \p ran.
  //!
  //! \complexity
  //! Linear in the size of \p dom.
#ifndef LIBSEMIGROUPS_PARSED_BY_DOXYGEN
  template <typename Return, typename Int>
  [[nodiscard]] enable_if_is_same<Return, HPCombi::PPerm16>
  make(std::initializer_list<Int> dom,
       std::initializer_list<Int> ran,
       size_t                     M) {
    return make<Return>(std::vector<uint8_t>(dom.begin(), dom.end()),
                        std::vector<uint8_t>(ran.begin(), ran.end()),
                        M);
  }
#else
  template <typename Int>
  [[nodiscard]] HPCombi::PPerm16
  make<HPCombi::PPerm16>(std::initializer_list<Int> const& dom,
                         std::initializer_list<Int> const& ran,
                         size_t                            M = 16);
#endif

#ifndef LIBSEMIGROUPS_PARSED_BY_DOXYGEN
  // We omit the doc for this version because the return type is awful and ruins
  // the rest of the doc in the table at the top of the page.
  //! \ingroup make_hpcombi_group
  //!
  //! \brief Construct a derived class of `HPCombi::PTransf16` from universal
  //! reference and check.
  //!
  //! Constructs a derived class of `HPCombi::Transf16` initialized using the
  //! container \p cont as follows: the image of the point \c i under the
  //! transformation is the value in position \c i of the container \p cont.
  //!
  //! \tparam Return the return type. Must be derived from `HPCombi::PTransf16`.
  //!
  //! \param cont the container.
  //!
  //! \returns A `HPCombi::Transf16` instance.
  //!
  //! \throw LibsemigroupsException if any of the following hold:
  //! * the size of \p cont exceeds \c 16;
  //! * any value in \p cont exceeds `cont.size()`.
  //!
  //! \complexity
  //! Linear in the size of the container \p cont.
  template <typename Return>
  [[nodiscard]] std::enable_if_t<std::is_base_of_v<HPCombi::PTransf16, Return>,
                                 Return>
  make(std::initializer_list<uint8_t>&& cont) {
    return make<Return, std::initializer_list<uint8_t>>(std::move(cont));
  }
#endif

}  // namespace libsemigroups

#include "hpcombi.tpp"

#endif  // LIBSEMIGROUPS_HPCOMBI_ENABLED
#endif  // LIBSEMIGROUPS_HPCOMBI_HPP_
