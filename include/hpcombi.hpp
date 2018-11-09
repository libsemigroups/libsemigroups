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

// This file contains specializations of the class adapters in adapters.hpp
// for the element types in HPCombi.

#ifndef LIBSEMIGROUPS_INCLUDE_HPCOMBI_HPP_
#define LIBSEMIGROUPS_INCLUDE_HPCOMBI_HPP_

// Must include libsemigroups-config.hpp so that LIBSEMIGROUPS_HPCOMBI is
// defined, if so specified at during configure.
#include "libsemigroups-config.hpp"

#ifdef LIBSEMIGROUPS_HPCOMBI

#include "extern/HPCombi/include/hpcombi.hpp"

#include "adapters.hpp"

namespace libsemigroups {
  using PTransf16 = HPCombi::PTransf16;

  template <class TPTransf16Subclass>
  struct complexity<
      TPTransf16Subclass,
      typename std::enable_if<
          std::is_base_of<PTransf16, TPTransf16Subclass>::value>::type> {
    constexpr size_t operator()(TPTransf16Subclass const&) const noexcept {
      return 0;
    }
  };

  template <class TPTransf16Subclass>
  struct degree<
      TPTransf16Subclass,
      typename std::enable_if<
          std::is_base_of<PTransf16, TPTransf16Subclass>::value>::type> {
    constexpr size_t operator()(TPTransf16Subclass const&) const noexcept {
      return 16;
    }
  };

  template <class TPTransf16Subclass>
  struct less<
      TPTransf16Subclass,
      typename std::enable_if<
          std::is_base_of<PTransf16, TPTransf16Subclass>::value>::type> {
    bool operator()(TPTransf16Subclass const& x,
                    TPTransf16Subclass const& y) const noexcept {
      return x < y;
    }
  };

  template <class TPTransf16Subclass>
  struct one<TPTransf16Subclass,
             typename std::enable_if<
                 std::is_base_of<PTransf16, TPTransf16Subclass>::value>::type> {
    TPTransf16Subclass operator()(size_t) const noexcept {
      return TPTransf16Subclass::one();
    }

    TPTransf16Subclass operator()(TPTransf16Subclass const&) const noexcept {
      return TPTransf16Subclass::one();
    }
  };

  // Note that HPCombi implements composition of functions from left to right,
  // whereas libsemigroups assumes composition is right to left.
  template <class TPTransf16Subclass>
  struct product<
      TPTransf16Subclass,
      typename std::enable_if<
          std::is_base_of<PTransf16, TPTransf16Subclass>::value>::type> {
    void operator()(TPTransf16Subclass&       xy,
                    TPTransf16Subclass const& x,
                    TPTransf16Subclass const& y,
                    size_t = 0) const noexcept {
      xy = y * x;
    }
  };

  template <class TPTransf16Subclass>
  struct swap<
      TPTransf16Subclass,
      typename std::enable_if<
          std::is_base_of<PTransf16, TPTransf16Subclass>::value>::type> {
    void operator()(TPTransf16Subclass& x, TPTransf16Subclass& y) const
        noexcept {
      std::swap(x, y);
    }
  };

#ifdef LIBSEMIGROUPS_DENSEHASHMAP
  template <class TPTransf16Subclass>
  struct empty_key<
      TPTransf16Subclass,
      typename std::enable_if<
          std::is_base_of<PTransf16, TPTransf16Subclass>::value>::type> {
    TPTransf16Subclass operator()(TPTransf16Subclass const&) const noexcept {
      return {FE, FE, FE, FE, FE, FE, FE, FE, FE, FE, FE, FE, FE, FE, FE, FE};
    }
  };
#endif

  // FIXME should only be for Perm16's
  template <class TPTransf16Subclass, typename TValueType>
  struct action<
      TPTransf16Subclass,
      TValueType,
      typename std::enable_if<
          std::is_base_of<PTransf16, TPTransf16Subclass>::value>::type> {
    TValueType operator()(TPTransf16Subclass x, TValueType i) const noexcept {
      return x[i];
    }
  };

  // FIXME should only be for Perm16's
  template <class TPTransf16Subclass>
  struct inverse<
      TPTransf16Subclass,
      typename std::enable_if<
          std::is_base_of<PTransf16, TPTransf16Subclass>::value>::type> {
    TPTransf16Subclass operator()(TPTransf16Subclass const& x) const noexcept {
      return x.inverse();
    }
  };

  template <typename TIntType>
  struct on_points<HPCombi::Perm16, TIntType> {
    void operator()(TIntType&              res,
                    TIntType const&        pt,
                    HPCombi::Perm16 const& p) const noexcept {
      LIBSEMIGROUPS_ASSERT(pt < 16);
      res = static_cast<TIntType>(p[pt]);
    }
  };

  template <>
  struct right_action<HPCombi::PPerm16, HPCombi::PPerm16> {
    void operator()(HPCombi::PPerm16&       res,
                    HPCombi::PPerm16 const& pt,
                    HPCombi::PPerm16 const& x) const noexcept {
      res = (x * pt).left_one();
    }
  };

  template <>
  struct left_action<HPCombi::PPerm16, HPCombi::PPerm16> {
    void operator()(HPCombi::PPerm16&       res,
                    HPCombi::PPerm16 const& pt,
                    HPCombi::PPerm16 const& x) const noexcept {
      res = (pt * x).right_one();
    }
  };
}  // namespace libsemigroups

#endif  // LIBSEMIGROUPS_HPCOMBI
#endif  // LIBSEMIGROUPS_INCLUDE_HPCOMBI_HPP_
