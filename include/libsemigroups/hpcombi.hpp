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

// This file contains specializations of the class adapters in adapters.hpp
// for the element types in HPCombi.

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
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wbitwise-instead-of-logical"
#endif
#include "hpcombi/hpcombi.hpp"  // for HPCombi::Perm16, ...
#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif
#if defined(__clang__)
#pragma clang diagnostic pop
#endif

#include <type_traits>    // for decay_t etc
#include <unordered_map>  // for unordered_map

#include "adapters.hpp"   // for Complexity, Degree, ...
#include "constants.hpp"  // for UNDEFINED
#include "debug.hpp"      // for LIBSEMIGROUPS_ASSERT
#include "exception.hpp"  // for LIBSEMIGROUPS_EXCEPTION
#include "types.hpp"      // for enable_if_is_same
#else
#include <type_traits>    // for decay_t etc
#include <unordered_map>  // for unordered_map

#include "exception.hpp"  // for LIBSEMIGROUPS_EXCEPTION
#endif

namespace libsemigroups {
  namespace detail {
    //! No doc
    // TODO(1) to tpp
    template <typename Iterator>
    void throw_if_duplicates(
        Iterator                                                    first,
        Iterator                                                    last,
        std::unordered_map<std::decay_t<decltype(*first)>, size_t>& seen) {
      seen.clear();
      for (auto it = first; it != last; ++it) {
        if (*it != UNDEFINED) {
          auto [pos, inserted] = seen.emplace(*it, seen.size());
          if (!inserted) {
            LIBSEMIGROUPS_EXCEPTION(
                "duplicate value, found {} in position {}, first "
                "occurrence in position {}",
                *it,
                std::distance(first, it),
                pos->second);
          }
        }
      }
    }

    //! No doc
    template <typename Iterator>
    void throw_if_duplicates(Iterator first, Iterator last) {
      std::unordered_map<std::decay_t<decltype(*first)>, size_t> seen;
      throw_if_duplicates(first, last, seen);
    }
  }  // namespace detail

#if defined(LIBSEMIGROUPS_HPCOMBI_ENABLED) \
    || defined(LIBSEMIGROUPS_PARSED_BY_DOXYGEN)
  //! \defgroup adapters_hpcombi_group Adapters for HPCombi
  //!
  //! This page contains the documentation of the functionality in
  //! `libsemigroups` that adapts the types in `HPCombi` for use with the
  //! algorithms in libsemigroups. The functionality in this section is only
  //! available if libsemigroups is compiled with `HPCombi` support enabled;
  //! this is only possible on certain computer architectures. See the
  //! \ref install.md "installation instructions" and the HPCombi documentation
  //! for further details.
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
  //! \sa Complexity.
  template <typename TPTransf16Subclass>
  struct Complexity<TPTransf16Subclass,
                    std::enable_if_t<std::is_base_of_v<HPCombi::PTransf16,
                                                       TPTransf16Subclass>>> {
    //! \brief Returns 0.
    //!
    //! Returns 0.
    //!
    //! \exceptions
    //! \noexcept
    constexpr size_t operator()(TPTransf16Subclass const&) const noexcept {
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
  //! \sa Degree.
  template <typename TPTransf16Subclass>
  struct Degree<TPTransf16Subclass,
                std::enable_if_t<std::is_base_of_v<HPCombi::PTransf16,
                                                   TPTransf16Subclass>>> {
    //! \brief Returns 16.
    //!
    //! Returns 16.
    //!
    //! \exceptions
    //! \noexcept
    constexpr size_t operator()(TPTransf16Subclass const&) const noexcept {
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
  //! \sa One.
  template <typename TPTransf16Subclass>
  struct One<TPTransf16Subclass,
             std::enable_if_t<
                 std::is_base_of_v<HPCombi::PTransf16, TPTransf16Subclass>>> {
    //! \brief Returns the identity for `HPCombi::PTransf16`.
    //!
    //! Returns the identity for `HPCombi::PTransf16`.
    //!
    //! \exceptions
    //! \noexcept
    TPTransf16Subclass operator()(size_t = 0) const noexcept {
      return TPTransf16Subclass::one();
    }
    //! \brief Returns the identity for `HPCombi::PTransf16`.
    //!
    //! Returns the identity for `HPCombi::PTransf16`.
    //!
    //! \exceptions
    //! \noexcept
    TPTransf16Subclass operator()(TPTransf16Subclass const&) const noexcept {
      return TPTransf16Subclass::one();
    }
  };

  //! \brief Specialisation of the Product adapter for `HPCombi::PTransf16`.
  //!
  //! Defined in `hpcombi.hpp`.
  //!
  //! Specialization of the Product adapter for subclasses of
  //! `HPCombi::PTransf16`.
  //!
  //! \note `HPCombi` implements composition of functions from left to right,
  //! whereas `libsemigroups` assumes composition is right to left.
  //!
  //! \sa Product.
  template <typename TPTransf16Subclass>
  struct Product<TPTransf16Subclass,
                 std::enable_if_t<std::is_base_of_v<HPCombi::PTransf16,
                                                    TPTransf16Subclass>>> {
    //! \brief Modifies \p xy in-place to be the product of \p x and \p y.
    //!
    //! Modifies \p xy in-place to be the product of \p x and \p y.
    //!
    //! \exceptions
    //! \noexcept
    void operator()(TPTransf16Subclass&       xy,
                    TPTransf16Subclass const& x,
                    TPTransf16Subclass const& y,
                    size_t = 0) const noexcept {
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
  //! \sa Swap.
  template <typename TPTransf16Subclass>
  struct Swap<TPTransf16Subclass,
              std::enable_if_t<
                  std::is_base_of_v<HPCombi::PTransf16, TPTransf16Subclass>>> {
    //! \brief Swap \p x and \p y.
    //!
    //! Swap \p x and \p y using std::swap.
    //!
    //! \exceptions
    //! \noexcept
    void operator()(TPTransf16Subclass& x,
                    TPTransf16Subclass& y) const noexcept {
      std::swap(x, y);
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
  //! \sa ImageRightAction.
  template <typename TIntType>
  struct ImageRightAction<HPCombi::Perm16,
                          TIntType,
                          std::enable_if_t<std::is_integral_v<TIntType>>> {
    //! \brief Stores the image of \p pt under \p p.
    //!
    //! Modifies \p res in-place to store the image of \p pt under \p p in
    //! \p res.
    //!
    //! \exceptions
    //! \noexcept
    void operator()(TIntType&              res,
                    TIntType const&        pt,
                    HPCombi::Perm16 const& p) const noexcept {
      LIBSEMIGROUPS_ASSERT(pt < 16);
      res = static_cast<TIntType>(p[pt]);
    }

    //! Returns the image of \p pt under \p p.
    TIntType operator()(TIntType const&        pt,
                        HPCombi::Perm16 const& p) const noexcept {
      LIBSEMIGROUPS_ASSERT(pt < 16);
      return p[pt];
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
    //! Modifies \p res in-place to store the idempotent \f$(xy) ^ {-1}xy\f$
    //! in
    //! \p res.
    //!
    //! \exceptions
    //! \noexcept
    void operator()(HPCombi::PPerm16&       res,
                    HPCombi::PPerm16 const& x,
                    HPCombi::PPerm16 const& y) const noexcept {
      res = (y * x).left_one();
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
    //! Modifies res in-place to store the idempotent \f$xy(xy) ^ {-1}\f$ in
    //! \p res.
    //!
    //! \exceptions
    //! \noexcept
    void operator()(HPCombi::PPerm16&       res,
                    HPCombi::PPerm16 const& x,
                    HPCombi::PPerm16 const& y) const noexcept {
      res = (x * y).right_one();
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
    //! Returns 0; `HPCombi::BMat8` multiplication is constant complexity.
    constexpr inline size_t operator()(HPCombi::BMat8 const&) const noexcept {
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
    //! Returns 8; all `HPCombi::BMat8`s have degree 8.
    constexpr inline size_t operator()(HPCombi::BMat8 const&) const noexcept {
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
    //! Does nothing.
    inline void operator()(HPCombi::BMat8 const&) const noexcept {}
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
    //! \brief Returns \p x.one().
    //!
    //! Returns \p x.one()
    inline HPCombi::BMat8 operator()(HPCombi::BMat8 const& x) const noexcept {
      return x.one();
    }
    //! \brief Returns `HPCombi::BMat8::one`.
    //!
    //! Returns `HPCombi::BMat8::one`
    inline HPCombi::BMat8 operator()(size_t = 0) const noexcept {
      return HPCombi::BMat8::one();
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
    //! \brief Changes \p xy in-place to hold the product of \p x and \p y.
    //!
    //! Changes \p xy in-place to hold the product of \p x and \p y
    inline void operator()(HPCombi::BMat8&       xy,
                           HPCombi::BMat8 const& x,
                           HPCombi::BMat8 const& y,
                           size_t = 0) const noexcept {
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
      LIBSEMIGROUPS_ASSERT(x * x.transpose() == x.one());
      return x.transpose();
    }
  };

  ////////////////////////////////////////////////////////////////////////
  // Konieczny adapters - HPCombi::BMat8
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
  // Konieczny adapters - HPCombi::PPerm16
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
                    HPCombi::Vect16 const&   y) const noexcept {
      HPCombi::Vect16 buf  = {0xff,
                              0xff,
                              0xff,
                              0xff,
                              0xff,
                              0xff,
                              0xff,
                              0xff,
                              0xff,
                              0xff,
                              0xff,
                              0xff,
                              0xff,
                              0xff,
                              0xff,
                              0xff};
      size_t          next = 0;
      for (size_t i = 0; i < 16; ++i) {
        if (buf[x[y[i]]] == 0xff) {
          buf[x[y[i]]] = next++;
        }
        res[i] = buf[x[y[i]]];
      }
    }
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
                    HPCombi::Transf16 const& x) const noexcept {
      HPCombi::Vect16 buf  = {0xff,
                              0xff,
                              0xff,
                              0xff,
                              0xff,
                              0xff,
                              0xff,
                              0xff,
                              0xff,
                              0xff,
                              0xff,
                              0xff,
                              0xff,
                              0xff,
                              0xff,
                              0xff};
      size_t          next = 0;
      for (size_t i = 0; i < 16; ++i) {
        if (buf[x[i]] == 0xff) {
          buf[x[i]] = next++;
        }
        res[i] = buf[x[i]];
      }
    }
  };

  ////////////////////////////////////////////////////////////////////////
  // Konieczny adapters - generic
  ////////////////////////////////////////////////////////////////////////

  //! \brief Specialization of the Rank adapter for `HPCombi` types.
  //!
  //! Defined in `hpcombi.hpp`.
  //!
  //! Specialization of the Rank adapter for `HPCombi` types.
  template <typename T>
  struct Rank<T,
              std::enable_if_t<std::is_base_of_v<HPCombi::PTransf16, T>,
                               RankState<T>>> {
    //! \brief Returns the rank of \p x as used in the Konieczny algorithm.
    //!
    //! Returns the rank of \p x as used in the Konieczny algorithm; for
    //! `HPCombi::Transf16` and `HPCombi::PPerm16` this is the size of the
    //! image set.
    inline size_t operator()(T const& x) const noexcept {
      return x.rank();
    }
  };

  //! @}

  //! \ingroup make_transf_group
  //!
  //! \brief Construct a HPCombi::Transf16 from universal reference and
  //! check.
  //!
  //! Constructs a HPCombi::Transf16 initialized using the container \p cont
  //! as follows: the image of the point \c i under the transformation is the
  //! value in position \c i of the container \p cont.
  //!
  //! \tparam Return the return type. Must by HPCombi::Transf16.
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
  template <typename Return, typename Container>
  [[nodiscard]] enable_if_is_same<Return, HPCombi::Transf16>
  make(Container&& cont) {
    if (cont.size() > 16) {
      LIBSEMIGROUPS_EXCEPTION("the 1st argument (container) must have size at "
                              "most 16, but found {}",
                              cont.size());
    }
    auto result = HPCombi::Transf16(cont);
    if (!result.validate()) {
      auto it = std::find_if(std::begin(cont),
                             std::begin(cont),
                             [](auto const& val) { return val > 15; });
      LIBSEMIGROUPS_EXCEPTION("image value out of bounds, expected value in "
                              "[0, 16), found {} in position {}",
                              *it,
                              std::distance(std::begin(cont), it));
    }
    return result;
  }

  //! \ingroup make_perm_group
  //!
  //! \brief Construct a HPCombi::Perm16 from universal reference and
  //! check.
  //!
  //! Constructs a HPCombi::Perm16 initialized using the container \p cont as
  //! follows: the image of the point \c i under the permutation is the
  //! value in position \c i of the container \p cont.
  //!
  //! \tparam Return the return type. Must by HPCombi::Perm16.
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
  template <typename Return, typename Container>
  [[nodiscard]] enable_if_is_same<Return, HPCombi::Perm16>
  make(Container&& cont) {
    if (cont.size() > 16) {
      LIBSEMIGROUPS_EXCEPTION("the 1st argument (container) must have size at "
                              "most 16, but found {}",
                              cont.size());
    }
    auto result = HPCombi::Perm16(cont);
    if (!result.HPCombi::Transf16::validate()) {
      auto it = std::find_if(std::begin(cont),
                             std::begin(cont),
                             [](auto const& val) { return val > 15; });
      LIBSEMIGROUPS_EXCEPTION("image value out of bounds, expected value in "
                              "[0, 16), found {} in position {}",
                              *it,
                              std::distance(std::begin(cont), it));
    }
    if (!result.validate()) {
      detail::throw_if_duplicates(std::begin(cont), std::end(cont));
      // TODO(1) check if result contains UNDEFINED
    }
    return result;
  }

  //! \ingroup make_pperm_group
  //!
  //! \brief Construct a HPCombi::PPerm16 from container and check.
  //!
  //! Constructs a HPCombi::PPerm16 initialized using the container \p cont as
  //! follows: the image of the point \c i under the partial perm is the
  //! value in position \c i of the container \p cont.
  //!
  //! \tparam Return the return type. Must by HPCombi::PPerm16.
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
  template <typename Return, typename Container>
  [[nodiscard]] enable_if_is_same<Return, HPCombi::PPerm16>
  make(Container&& cont) {
    if (cont.size() > 16) {
      LIBSEMIGROUPS_EXCEPTION("the 1st argument (container) must have size at "
                              "most 16, but found {}",
                              cont.size());
    }
    auto result = HPCombi::PPerm16(cont);
    if (!result.HPCombi::PTransf16::validate()) {
      auto it = std::find_if(
          std::begin(cont), std::begin(cont), [](auto const& val) {
            return val != 0xFF && val > 15;
          });
      LIBSEMIGROUPS_EXCEPTION("image value out of bounds, expected value in "
                              "[0, 16) or 0xFF (= {}), found {} in position {}",
                              0xFF,
                              *it,
                              std::distance(std::begin(cont), it));
    }
    if (!result.validate()) {
      detail::throw_if_duplicates(std::begin(cont), std::end(cont));
    }
    return result;
  }

  //! \ingroup make_pperm_group
  //!
  //! \brief Construct a HPCombi::PPerm16 from domain, range, and degree, and
  //! check.
  //!
  //! Constructs a partial perm of degree \p deg such that
  //! `f[dom[i]] = ran[i]` for all \c i and which is `0xFF` on every other value
  //! in the range \f$[0, M)\f$.
  //!
  //! \tparam Return the return type, must be HPCombi::PPerm16.
  //!
  //! \param dom the domain.
  //! \param ran the range.
  //! \param deg the degree.
  //!
  //! \throws LibsemigroupsException if any of the following fail to hold:
  //! * the value \p M is not compatible with the template parameter \p N
  //! * \p dom and \p ran do not have the same size
  //! * any value in \p dom or \p ran is greater than \p M
  //! * there are repeated entries in \p dom or \p ran.
  //!
  //! \complexity
  //! Linear in the size of \p dom.
  template <typename Return>
  [[nodiscard]] enable_if_is_same<Return, HPCombi::PPerm16>
  make(std::vector<uint8_t> const& dom,
       std::vector<uint8_t> const& ran,
       size_t                      deg = 16) {
    if (deg > 16) {
      LIBSEMIGROUPS_EXCEPTION(
          "the 3rd argument is not valid, expected <=16, found {}", deg);
    } else if (dom.size() != ran.size()) {
      LIBSEMIGROUPS_EXCEPTION("domain and range size mismatch, domain has "
                              "size {} but range has size {}",
                              dom.size(),
                              ran.size());
    } else if (!(dom.empty()
                 || deg > *std::max_element(dom.cbegin(), dom.cend()))) {
      LIBSEMIGROUPS_EXCEPTION(
          "domain value out of bounds, found {}, must be less than {}",
          *std::max_element(dom.cbegin(), dom.cend()),
          deg);
    }
    // TODO(1) check ran for out of bounds values too
    std::unordered_map<uint8_t, size_t> seen;
    detail::throw_if_duplicates(dom.cbegin(), dom.cend(), seen);
    detail::throw_if_duplicates(ran.cbegin(), ran.cend(), seen);
    HPCombi::PPerm16 result(dom, ran, deg);
    LIBSEMIGROUPS_ASSERT(result.validate());
    return result;
  }

  //! \ingroup make_pperm_group
  //!
  //! \brief Construct a HPCombi::PPerm16 from domain, range, and degree, and
  //! check.
  //!
  //! Constructs a partial perm of degree \p M such that
  //! `f[dom[i]] = ran[i]` for all \c i and which is `0xFF` on every other value
  //! in the range \f$[0, M)\f$.
  //!
  //! \tparam Return the return type, must be HPCombi::PPerm16.
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
  template <typename Return, typename Int>
  [[nodiscard]] enable_if_is_same<Return, HPCombi::PPerm16>
  make(std::initializer_list<Int> dom,
       std::initializer_list<Int> ran,
       size_t                     M) {
    return make<Return>(std::vector<uint8_t>(dom.begin(), dom.end()),
                        std::vector<uint8_t>(ran.begin(), ran.end()),
                        M);
  }

  //! \ingroup make_transf_group
  //!
  //! \brief Construct a HPCombi::Transf16 from universal reference and
  //! check.
  //!
  //! Constructs a HPCombi::Transf16 initialized using the container \p cont
  //! as follows: the image of the point \c i under the transformation is the
  //! value in position \c i of the container \p cont.
  //!
  //! \tparam Return the return type. Must by HPCombi::Transf16.
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
#endif  // LIBSEMIGROUPS_HPCOMBI_ENABLED

}  // namespace libsemigroups

#endif  // LIBSEMIGROUPS_HPCOMBI_HPP_
