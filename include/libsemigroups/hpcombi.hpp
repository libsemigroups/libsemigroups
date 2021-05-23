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

// This file contains specializations of the class adapters in adapters.hpp
// for the element types in HPCombi.

#ifndef LIBSEMIGROUPS_HPCOMBI_HPP_
#define LIBSEMIGROUPS_HPCOMBI_HPP_

// Must include config.hpp so that LIBSEMIGROUPS_HPCOMBI_ENABLED
// is defined, if so specified at during configure.
#include "config.hpp"  // for LIBSEMIGROUPS_HPCOMBI_ENABLED

#if defined(LIBSEMIGROUPS_HPCOMBI_ENABLED) \
    || defined(LIBSEMIGROUPS_PARSED_BY_DOXYGEN)

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#pragma GCC diagnostic ignored "-Winline"
#include "HPCombi/include/hpcombi.hpp"  // for HPCombi::Perm16, ...

#include "adapters.hpp"  // for Complexity, Degree, ...
#include "debug.hpp"     // for LIBSEMIGROUPS_ASSERT

namespace libsemigroups {

  //! Defined in ``hpcombi.hpp``.
  //!
  //! Specialization of the adapter Complexity for subclasses of
  //! ``HPCombi::PTransf16``.
  //!
  //! \sa Complexity.
  template <typename TPTransf16Subclass>
  struct Complexity<
      TPTransf16Subclass,
      std::enable_if_t<
          std::is_base_of<HPCombi::PTransf16, TPTransf16Subclass>::value>> {
    //! Returns 0.
    //! \exceptions
    //! \noexcept
    constexpr size_t operator()(TPTransf16Subclass const&) const noexcept {
      return 0;
    }
  };

  //! Defined in ``hpcombi.hpp``.
  //!
  //! Specialization of the adapter Degree for subclasses of
  //! ``HPCombi::PTransf16``.
  //!
  //! \sa Degree.
  template <typename TPTransf16Subclass>
  struct Degree<TPTransf16Subclass,
                std::enable_if_t<std::is_base_of<HPCombi::PTransf16,
                                                 TPTransf16Subclass>::value>> {
    //! Returns 16.
    //! \exceptions
    //! \noexcept
    constexpr size_t operator()(TPTransf16Subclass const&) const noexcept {
      return 16;
    }
  };

  //! Defined in ``hpcombi.hpp``.
  //!
  //! Specialization of the adapter One for subclasses of
  //! ``HPCombi::PTransf16``.
  //!
  //! \sa One.
  template <typename TPTransf16Subclass>
  struct One<TPTransf16Subclass,
             std::enable_if_t<std::is_base_of<HPCombi::PTransf16,
                                              TPTransf16Subclass>::value>> {
    //! Returns the identity ``HPCombi::PTransf16``.
    //! \exceptions
    //! \noexcept
    TPTransf16Subclass operator()(size_t = 0) const noexcept {
      return TPTransf16Subclass::one();
    }

    //! Returns the identity ``HPCombi::PTransf16``.
    //! \exceptions
    //! \noexcept
    TPTransf16Subclass operator()(TPTransf16Subclass const&) const noexcept {
      return TPTransf16Subclass::one();
    }
  };

  //! Defined in ``hpcombi.hpp``.
  //!
  //! Specialization of the adapter Product for subclasses of
  //! ``HPCombi::PTransf16``.
  //!
  //! \sa Product.
  //!
  //! \note ``HPCombi`` implements composition of functions from left to right,
  //! whereas ``libsemigroups`` assumes composition is right to left.
  template <typename TPTransf16Subclass>
  struct Product<TPTransf16Subclass,
                 std::enable_if_t<std::is_base_of<HPCombi::PTransf16,
                                                  TPTransf16Subclass>::value>> {
    //! Returns modifies \p xy in-place to be the product of \p x and \p y.
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

  //! Defined in ``hpcombi.hpp``.
  //!
  //! Specialization of the adapter Swap for subclasses of
  //! ``HPCombi::PTransf16``.
  //!
  //! \sa Swap.
  template <typename TPTransf16Subclass>
  struct Swap<TPTransf16Subclass,
              std::enable_if_t<std::is_base_of<HPCombi::PTransf16,
                                               TPTransf16Subclass>::value>> {
    //! Swap \p x and \p y.
    //!
    //! \exceptions
    //! \noexcept
    void operator()(TPTransf16Subclass& x,
                    TPTransf16Subclass& y) const noexcept {
      std::swap(x, y);
    }
  };

  //! Defined in ``hpcombi.hpp``.
  //!
  //! Specialization of the adapter Inverse for ``HPCombi::Perm16``.
  //!
  //! \sa Inverse.
  template <>
  struct Inverse<HPCombi::Perm16> {
    //! Returns the inverse of \p x.
    //! \exceptions
    //! \noexcept
    HPCombi::Perm16 operator()(HPCombi::Perm16 const& x) const noexcept {
      return x.inverse();
    }
  };

  //! Defined in ``hpcombi.hpp``.
  //!
  //! Specialization of the adapter ImageRightAction for
  //! ``HPCombi::Perm16``.
  //!
  //! \sa ImageRightAction.
  template <typename TIntType>
  struct ImageRightAction<HPCombi::Perm16,
                          TIntType,
                          std::enable_if_t<std::is_integral<TIntType>::value>> {
    //! Stores the image of \p pt under \p p in \p res.
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

  //! Defined in ``hpcombi.hpp``.
  //!
  //! Specialization of the adapter ImageRightAction for ``HPCombi::PPerm16``.
  //!
  //! \sa ImageRightAction.
  template <>
  struct ImageRightAction<HPCombi::PPerm16, HPCombi::PPerm16> {
    //! Stores the idempotent \f$(xy) ^ {-1}xy\f$ in \p res.
    //! \exceptions
    //! \noexcept
    void operator()(HPCombi::PPerm16&       res,
                    HPCombi::PPerm16 const& x,
                    HPCombi::PPerm16 const& y) const noexcept {
      res = (y * x).left_one();
    }
  };

  //! Defined in ``hpcombi.hpp``.
  //!
  //! Specialization of the adapter ImageLeftAction for ``HPCombi::PPerm16``.
  //!
  //! \sa ImageLeftAction.
  template <>
  struct ImageLeftAction<HPCombi::PPerm16, HPCombi::PPerm16> {
    //! Stores the idempotent \f$xy(xy) ^ {-1}\f$ in \p res.
    //! \exceptions
    //! \noexcept
    void operator()(HPCombi::PPerm16&       res,
                    HPCombi::PPerm16 const& x,
                    HPCombi::PPerm16 const& y) const noexcept {
      res = (x * y).right_one();
    }
  };

  //! Defined in ``hpcombi.hpp``.
  //!
  //! Specialization of the adapter Complexity for instances of
  //! HPCombi::BMat8.
  //!
  //! \sa Complexity.
  template <>
  struct Complexity<HPCombi::BMat8> {
    //! Returns 0; HPCombi::BMat8 multiplication is constant complexity.
    constexpr inline size_t operator()(HPCombi::BMat8 const&) const noexcept {
      return 0;
    }
  };

  //! Defined in ``hpcombi.hpp``.
  //!
  //! Specialization of the adapter Degree for instances of
  //! HPCombi::BMat8.
  //!
  //! \sa Degree.
  template <>
  struct Degree<HPCombi::BMat8> {
    //! Returns 8; all HPCombi::BMat8s have degree 8.
    constexpr inline size_t operator()(HPCombi::BMat8 const&) const noexcept {
      return 8;
    }
  };

  //! Defined in ``hpcombi.hpp``.
  //!
  //! Specialization of the adapter IncreaseDegree for instances of
  //! HPCombi::BMat8.
  //!
  //! \sa IncreaseDegree.
  template <>
  struct IncreaseDegree<HPCombi::BMat8> {
    //! Does nothing.
    inline void operator()(HPCombi::BMat8 const&) const noexcept {}
  };

  //! Defined in ``hpcombi.hpp``.
  //!
  //! Specialization of the adapter One for instances of
  //! HPCombi::BMat8.
  //!
  //! \sa One.
  template <>
  struct One<HPCombi::BMat8> {
    //! Returns \p x.one()
    inline HPCombi::BMat8 operator()(HPCombi::BMat8 const& x) const noexcept {
      return x.one();
    }
    //! Returns HPCombi::BMat8::one
    inline HPCombi::BMat8 operator()(size_t = 0) const noexcept {
      return HPCombi::BMat8::one();
    }
  };

  //! Defined in ``hpcombi.hpp``.
  //!
  //! Specialization of the adapter Product for instances of
  //! HPCombi::BMat8.
  //!
  //! \sa Product.
  template <>
  struct Product<HPCombi::BMat8> {
    //! Changes \p xy in place to hold the product of \p x and \p y
    inline void operator()(HPCombi::BMat8&       xy,
                           HPCombi::BMat8 const& x,
                           HPCombi::BMat8 const& y,
                           size_t = 0) const noexcept {
      xy = x * y;
    }
  };

  //! Defined in ``hpcombi.hpp``.
  //!
  //! Specialization of the adapter ImageRightAction for instances of
  //! HPCombi::BMat8.
  //!
  //! \sa ImageRightAction.
  template <>
  struct ImageRightAction<HPCombi::BMat8, HPCombi::BMat8> {
    //! Changes \p res in place to hold the image of \p pt under the right
    //! action of \p x.
    void operator()(HPCombi::BMat8&       res,
                    HPCombi::BMat8 const& pt,
                    HPCombi::BMat8 const& x) const noexcept {
      res = (pt * x).row_space_basis();
    }
  };

  //! Defined in ``hpcombi.hpp``.
  //!
  //! Specialization of the adapter ImageLeftAction for instances of
  //! HPCombi::BMat8.
  //!
  //! \sa ImageLeftAction.
  template <>
  struct ImageLeftAction<HPCombi::BMat8, HPCombi::BMat8> {
    //! Changes \p res in place to hold the image of \p pt under the left
    //! action of \p x.
    void operator()(HPCombi::BMat8& res,
                    HPCombi::BMat8  pt,
                    HPCombi::BMat8  x) const noexcept {
      res = (x * pt).col_space_basis();
    }
  };

  //! Defined in ``hpcombi.hpp``.
  //!
  //! Specialization of the adapter Inverse for instances of
  //! HPCombi::BMat8.
  //!
  //! \sa Inverse.
  template <>
  struct Inverse<HPCombi::BMat8> {
    //! Returns the group inverse of \p x.
    inline HPCombi::BMat8 operator()(HPCombi::BMat8 const& x) const noexcept {
      LIBSEMIGROUPS_ASSERT(x * x.transpose() == x.one());
      return x.transpose();
    }
  };

  ////////////////////////////////////////////////////////////////////////
  // Konieczny adapters - HPCombi::BMat8
  ////////////////////////////////////////////////////////////////////////

  //! Specialization of the adapter LambdaValue for instances of
  //! HPCombi::BMat8.
  //!
  //! \sa LambdaValue
  template <>
  struct LambdaValue<HPCombi::BMat8> {
    //! The type of Lambda values for HPCombi::BMat8 is also HPCombi::BMat8;
    //! this provides an efficient representation of row space bases.
    using type = HPCombi::BMat8;
  };

  //! Specialization of the adapter RhoValue for instances of
  //! HPCombi::BMat8.
  //!
  //! \sa RhoValue
  template <>
  struct RhoValue<HPCombi::BMat8> {
    //! The type of Rho values for HPCombi::BMat8 is also HPCombi::BMat8;
    //! this provides an efficient representation of row space bases.
    using type = HPCombi::BMat8;
  };

  //! Specialization of the adapter Lambda for instances of
  //! BMat8.
  //!
  //! \sa Lambda.
  template <>
  struct Lambda<HPCombi::BMat8, HPCombi::BMat8> {
    //! Set \p res to the lambda value of \p x as used in the Konieczny
    //! algorithm; for HPCombi::BMat8 this is the row space basis.
    void operator()(HPCombi::BMat8&       res,
                    HPCombi::BMat8 const& x) const noexcept {
      res = x.row_space_basis();
    }
  };

  template <>
  struct Rho<HPCombi::BMat8, HPCombi::BMat8> {
    //! Set \p res to the rho value of \p x as used in the Konieczny
    //! algorithm; for HPCombi::BMat8 this is the column space basis.
    void operator()(HPCombi::BMat8&       res,
                    HPCombi::BMat8 const& x) const noexcept {
      res = x.col_space_basis();
    }
  };

  template <>
  struct Rank<HPCombi::BMat8> {
    //! Returns the rank of \p x as used in the Konieczny algorithm; for BMat8
    //! this is the size of the row space.
    inline size_t operator()(HPCombi::BMat8 const& x) const noexcept {
      return x.row_space_size();
    }
  };

  ////////////////////////////////////////////////////////////////////////
  // Konieczny adapters - HPCombi::PPerm16
  ////////////////////////////////////////////////////////////////////////

  //! Specialization of the adapter LambdaValue for instances of
  //! HPCombi::PPerm16.
  //!
  //! \sa LambdaValue
  template <>
  struct LambdaValue<HPCombi::PPerm16> {
    //! The type of Lambda values for PPerm16 is also PPerm16; this provides an
    //! efficient representation of image sets.
    using type = HPCombi::PPerm16;
  };

  //! Specialization of the adapter RhoValue for instances of
  //! HPCombi::PPerm16.
  //!
  //! \sa RhoValue
  template <>
  struct RhoValue<HPCombi::PPerm16> {
    //! The type of Rho values for PPerm16 is also PPerm16; this provides an
    //! efficient representation of domain sets.
    using type = HPCombi::PPerm16;
  };

  //! \sa ImageRightAction.
  template <>
  struct Lambda<HPCombi::PPerm16, HPCombi::PPerm16> {
    //! Stores the idempotent \f$(xy) ^ {-1}xy\f$ in \p res.
    //! \exceptions
    //! \noexcept
    void operator()(HPCombi::PPerm16&       res,
                    HPCombi::PPerm16 const& x) const noexcept {
      res = x.left_one();
    }
  };

  //! Defined in ``hpcombi.hpp``.
  //!
  //! Specialization of the adapter ImageLeftAction for ``HPCombi::PPerm16``.
  //!
  //! \sa ImageLeftAction.
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

  //! Defined in ``hpcombi.hpp``.
  //!
  //! Specialization of the adapter ImageRightAction for ``HPCombi::Transf16``.
  //!
  //! \sa ImageRightAction.
  template <>
  struct ImageRightAction<HPCombi::Transf16, HPCombi::PTransf16> {
    //! Changes \p res in place to hold the image of \p x under the right
    //! action of \p y.
    void operator()(HPCombi::PTransf16&       res,
                    HPCombi::Transf16 const&  x,
                    HPCombi::PTransf16 const& y) const noexcept {
      res = (y * static_cast<HPCombi::PTransf16>(static_cast<HPCombi::epu8>(x)))
                .left_one();
    }
  };

  //! Defined in ``hpcombi.hpp``.
  //!
  //! Specialization of the adapter ImageLeftAction for ``HPCombi::Transf16``.
  //!
  //! \sa ImageLeftAction.
  template <>
  struct ImageLeftAction<HPCombi::Transf16, HPCombi::Vect16> {
    //! Changes \p res in place to hold the image of \p x under the left
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

  //! Specialization of the adapter LambdaValue for instances of
  //! HPCombi::Transf16.
  //!
  //! \sa LambdaValue
  template <>
  struct LambdaValue<HPCombi::Transf16> {
    //! The type of Lambda values for Transf16 is PTransf16; this provides an
    //! efficient representation of image sets.
    using type = HPCombi::PTransf16;
  };

  //! Specialization of the adapter RhoValue for instances of
  //! HPCombi::Transf16.
  //!
  //! \sa RhoValue
  template <>
  struct RhoValue<HPCombi::Transf16> {
    //! The type of Rho values for Transf16 is a Vect16; this provides
    //! an efficient representation of the kernel.
    using type = HPCombi::Vect16;
  };

  //! Defined in ``hpcombi.hpp``.
  //!
  //! Specialization of the adapter Lambda for ``HPCombi::Transf16``.
  //!
  //! \sa ImageRightAction.
  template <>
  struct Lambda<HPCombi::Transf16, HPCombi::PTransf16> {
    //! Stores the identity function on the image of \p x in \p res.
    //! \exceptions
    //! \noexcept
    void operator()(HPCombi::PTransf16&      res,
                    HPCombi::Transf16 const& x) const noexcept {
      res = x.left_one();
    }
  };

  //! Defined in ``hpcombi.hpp``.
  //!
  //! Specialization of the adapter Lambda for ``HPCombi::Transf16``.
  //!
  //! \sa ImageLeftAction.
  template <>
  struct Rho<HPCombi::Transf16, HPCombi::Vect16> {
    //! Stores the kernel of \p x in \p res.
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

  //! Defined in ``hpcombi.hpp``.
  //!
  //! Specialization of the adapter Rank for `HPCombi` types.
  template <typename T>
  struct Rank<T,
              std::enable_if_t<std::is_base_of<HPCombi::PTransf16, T>::value,
                               RankState<T>>> {
    //! Returns the rank of \p x as used in the Konieczny algorithm; for
    //! HPCombi::Transf16 and HPCombi::PPerm16 this is the size of the image
    //! set.
    inline size_t operator()(T const& x) const noexcept {
      return x.rank();
    }
  };

}  // namespace libsemigroups

#pragma GCC diagnostic pop
#endif  // LIBSEMIGROUPS_HPCOMBI_ENABLED
#endif  // LIBSEMIGROUPS_HPCOMBI_HPP_
