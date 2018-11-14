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

//! \file
//! This file contains function templates for adapting a user-defined type so
//! that it can be used with libsemigroups.
//!
//! To use the TODO(now)

#ifndef LIBSEMIGROUPS_INCLUDE_ADAPTERS_HPP_
#define LIBSEMIGROUPS_INCLUDE_ADAPTERS_HPP_

#include <algorithm>  // for std::sort
#include <utility>    // for std::swap
#include <vector>     // for std::vector

#include "libsemigroups-config.hpp"  // for LIBSEMIGROUPS_DENSEHASHMAP

namespace libsemigroups {
  template <typename TElementType, typename = void>
  struct complexity;
  template <typename TElementType, typename = void>
  struct degree;
  template <typename TElementType, typename = void>
  struct increase_degree_by;
  template <typename TElementType, typename = void>
  struct less;
  template <typename TElementType, typename = void>
  struct one;
  template <typename TElementType, typename = void>
  struct product;
  template <typename TElementType, typename = void>
  struct swap {
    void operator()(TElementType& x, TElementType& y) {
      std::swap(x, y);
    }
  };

  template <typename TElementType, typename TPointType, typename = void>
  struct action;
  template <typename TElementType, typename = void>
  struct inverse;

  template <typename TElementType, typename TPointType, typename = void>
  struct left_action;
  template <typename TElementType, typename TPointType, typename = void>
  struct right_action;

  template <typename TElementType, typename TPointType, typename = void>
  struct on_points;

  template <typename TElementType,
            typename TPointType,
            typename TContainerType = std::vector<TPointType>,
            typename                = void>
  struct on_tuples {
    void operator()(TContainerType&       res,
                    TContainerType const& pt,
                    TElementType const&   p) const {
      for (size_t i = 0; i < pt.size(); ++i) {
        on_points<TElementType, TPointType>()(res[i], pt[i], p);
      }
    }
  };

  template <typename TElementType,
            typename TPointType,
            typename TContainerType = std::vector<TPointType>,
            typename                = void>
  struct on_sets {
    void operator()(TContainerType&       res,
                    TContainerType const& pt,
                    TElementType const&   p) const {
      on_tuples<TElementType, TPointType, TContainerType>()(res, pt, p);
      std::sort(res.begin(), res.end());
    }
  };

#ifdef LIBSEMIGROUPS_DENSEHASHMAP
  template <typename TElementType, typename = void>
  struct empty_key;
#endif
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_INCLUDE_ADAPTERS_HPP_
