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

// This file contains the alias BMatFastest, and is separate from matrix.hpp,
// bmat.hpp, and bmat8.hpp because these headers don't require each other.

#ifndef LIBSEMIGROUPS_BMAT_FASTEST_HPP_
#define LIBSEMIGROUPS_BMAT_FASTEST_HPP_

#include <cstddef>      // for size_t
#include <type_traits>  // for conditional_t

#include "config.hpp"   // for LIBSEMIGROUPS_HPCOMBI_ENABLED
#include "hpcombi.hpp"  // for HPCombi::BMat8
#include "matrix.hpp"   // for BMat

namespace libsemigroups {
  class BMat8;  // forward decl

  namespace detail {
    template <size_t N>
    struct BMatFastestHelper {
#ifdef LIBSEMIGROUPS_HPCOMBI_ENABLED
      using type = std::conditional_t<(N > 8), BMat<N>, HPCombi::BMat8>;
#else
      using type = std::conditional_t<(N > 8), BMat<N>, BMat8>;
#endif
    };
  }  // namespace detail

  //! \ingroup bmat_index_group
  //!
  //! Type of the fastest type of boolean matrix of given dimension.
  //!
  //! Defined in ``bmat-fastest.hpp``.
  //!
  //! Helper for getting the type of the fastest boolean \c N by \c N matrix in
  //! ``libsemigroups`` or [HPCombi][] (if enabled).
  //!
  //! \tparam N the dimension of the boolean matrices.
  //! [HPCombi]: https://libsemigroups.github.io/HPCombi/
  // TODO(later) example
  template <size_t N>
  using BMatFastest = typename detail::BMatFastestHelper<N>::type;

}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_BMAT_FASTEST_HPP_
