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

// Must include libsemigroups-config.hpp so that LIBSEMIGROUPS_HPCOMBI is
// defined, if so specified at during configure.
#include "libsemigroups-config.hpp"  // for LIBSEMIGROUPS_HPCOMBI

#ifdef LIBSEMIGROUPS_HPCOMBI

#include "HPCombi/include/hpcombi.hpp"  // for HPCombi::Perm16, ...

#include "adapters.hpp"             // for Complexity, Degree, ...
#include "libsemigroups-debug.hpp"  // for LIBSEMIGROUPS_ASSERT

namespace libsemigroups {

  //! Defined in ``hpcombi.hpp``.
  //!
  //! Specialization of the adapter Complexity for subclasses of
  //! ``HPCombi::PTransf16``.
  //!
  //! \sa Complexity.
  template <typename TPTransf16Subclass>
  struct Complexity<TPTransf16Subclass,
                    typename std::enable_if<
                        std::is_base_of<HPCombi::PTransf16,
                                        TPTransf16Subclass>::value>::type> {
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
                typename std::enable_if<
                    std::is_base_of<HPCombi::PTransf16,
                                    TPTransf16Subclass>::value>::type> {
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
             typename std::enable_if<
                 std::is_base_of<HPCombi::PTransf16,
                                 TPTransf16Subclass>::value>::type> {
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
                 typename std::enable_if<
                     std::is_base_of<HPCombi::PTransf16,
                                     TPTransf16Subclass>::value>::type> {
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
              typename std::enable_if<
                  std::is_base_of<HPCombi::PTransf16,
                                  TPTransf16Subclass>::value>::type> {
    //! Swap \p x and \p y.
    //!
    //! \exceptions
    //! \noexcept
    void operator()(TPTransf16Subclass& x, TPTransf16Subclass& y) const
        noexcept {
      std::swap(x, y);
    }
  };

  // #ifdef LIBSEMIGROUPS_DENSEHASHMAP
  //   template <typename TPTransf16Subclass>
  //   struct EmptyKey<TPTransf16Subclass,
  //                   typename std::enable_if<
  //                       std::is_base_of<HPCombi::PTransf16,
  //                                       TPTransf16Subclass>::value>::type> {
  //     TPTransf16Subclass operator()(TPTransf16Subclass const&) const noexcept
  //     {
  //       return {FE, FE, FE, FE, FE, FE, FE, FE, FE, FE, FE, FE, FE, FE, FE,
  //       FE};
  //     }
  //   };
  // #endif

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
  struct ImageRightAction<
      HPCombi::Perm16,
      TIntType,
      typename std::enable_if<std::is_integral<TIntType>::value>::type> {
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
    TIntType operator()(TIntType const& pt, HPCombi::Perm16 const& p) const
        noexcept {
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
                    HPCombi::PPerm16 const& pt,
                    HPCombi::PPerm16 const& x) const noexcept {
      res = (pt * x).right_one();
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
}  // namespace libsemigroups

#endif  // LIBSEMIGROUPS_HPCOMBI
#endif  // LIBSEMIGROUPS_HPCOMBI_HPP_
