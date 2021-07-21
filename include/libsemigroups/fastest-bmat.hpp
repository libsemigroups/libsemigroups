//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2021 James D. Mitchell
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

// This file contains the alias FastestBMat, and is separate from matrix.hpp,
// bmat.hpp, and bmat8.hpp because these headers don't require each other.

#ifndef LIBSEMIGROUPS_FASTEST_BMAT_HPP_
#define LIBSEMIGROUPS_FASTEST_BMAT_HPP_

#include <cstddef>  // for size_t

#include "bmat8.hpp"    // for BMat8
#include "config.hpp"   // for LIBSEMIGROUPS_HPCOMBI_ENABLED
#include "hpcombi.hpp"  // for HPCombi::BMat8
#include "matrix.hpp"   // for BMat

namespace libsemigroups {

  namespace detail {
    template <size_t N>
    struct FastestBMatHelper {
#ifdef LIBSEMIGROUPS_HPCOMBI_ENABLED
      using type = std::conditional_t<(N > 8), BMat<N>, HPCombi::BMat8>;
#else
      using type = std::conditional_t<(N > 8), BMat<N>, BMat8>;
#endif
    };
  }  // namespace detail

  //! Type of the fastest type of boolean matrix of given dimension.
  //!
  //! Defined in ``fastest-bmat.hpp``.
  //!
  //! Helper for getting the type of the fastest boolean \c N by \c N matrix in
  //! ``libsemigroups`` or ``HPCombi`` (if enabled).
  //!
  //! \tparam N the dimension of the boolean matrices.
  // TODO(later) example
  template <size_t N>
  using FastestBMat = typename detail::FastestBMatHelper<N>::type;

}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_FASTEST_BMAT_HPP_
