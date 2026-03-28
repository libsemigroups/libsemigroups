// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2021-5 Finn Smith
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

#ifndef LIBSEMIGROUPS_MAX_PLUS_TRUNC_MAT_HPP_
#define LIBSEMIGROUPS_MAX_PLUS_TRUNC_MAT_HPP_

#include <cstddef>        // for size_t
#include <numeric>        // for inner_product
#include <type_traits>    // for enable_if_t, enable_if
#include <unordered_set>  // for unordered_set
#include <utility>        // for move

#include "action.hpp"    // for RightAction
#include "adapters.hpp"  // for ImageRightAction
#include "matrix.hpp"    // for MaxPlusTruncMat

#include "detail/containers.hpp"  // for StaticVector1

namespace libsemigroups {
  // ////////////////////////////////////////////////////////////////////////
  // // ImageRight/LeftAction - MaxPlusTruncMat
  // ////////////////////////////////////////////////////////////////////////

  //! Specialization of the adapter ImageRightAction for
  //! StaticVector1<MaxPlusTruncMat<T, N>::Row, N> and MaxPlusTruncMat<T, N>
  //!
  //! \sa ImageLeftAction.
  template <typename Mat>
  struct ImageRightAction<Mat,
                          typename LambdaValue<Mat>::type,
                          std::enable_if_t<IsMaxPlusTruncMat<Mat>>> {
    //! Stores the image of \p pt under the right action of \p p in \p res.
    using result_type = typename LambdaValue<Mat>::type;
    void operator()(result_type&       res,
                    result_type const& pt,
                    Mat const&         x) const;
  };

  //! Specialization of the adapter ImageLeftAction for
  //! StaticVector1<MaxPlusTruncMat<T, N>::Row, N> and MaxPlusTruncMat<T, N>
  //!
  //! \sa ImageRightAction.
  template <typename Mat>
  struct ImageLeftAction<Mat,
                         typename RhoValue<Mat>::type,
                         std::enable_if_t<IsMaxPlusTruncMat<Mat>>> {
    // not noexcept because the constructor of std::vector isn't
    //! Stores the image of \p pt under the left action of \p p in \p res.
    using result_type = typename RhoValue<Mat>::type;
    void operator()(result_type&       res,
                    result_type const& pt,
                    Mat const&         x) const {
      const_cast<Mat*>(&x)->transpose();
      ImageRightAction<Mat, result_type>()(res, pt, x);
      const_cast<Mat*>(&x)->transpose();
    }
  };

  ////////////////////////////////////////////////////////////////////////
  // Lambda/Rho - MaxPlusTruncMat
  ////////////////////////////////////////////////////////////////////////

  //! Specialization of the adapter LambdaValue for instances of
  //! MaxPlusTruncMat<T, N>.
  //!
  //! \sa RhoValue, Lambda.
  template <typename Mat>
  struct LambdaValue<Mat, std::enable_if_t<IsMaxPlusTruncMat<Mat>>> {
    using type =
        typename detail::StaticVector1<typename Mat::Row, Mat::nr_rows>;
  };

  //! Specialization of the adapter RhoValue for instances of MaxPlusTruncMat<T,
  //! N>.
  //!
  //! \sa LambdaValue, Rho.
  template <typename Mat>
  struct RhoValue<Mat, std::enable_if_t<IsMaxPlusTruncMat<Mat>>> {
    //! For MaxPlusTruncMat<T, N>, \c type is StaticVector1<MaxPlusTruncMat<T,
    //! N>::Row, N>.
    //! This represents the column space basis of the BMats.
    using type =
        typename detail::StaticVector1<typename Mat::Row, Mat::nr_rows>;
  };

  //! Specialization of the adapter Lambda for instances of MaxPlusTruncMat<T,
  //! N> and StaticVector1<MaxPlusTruncMat<T, N>, N>.
  //!
  //! \sa LambdaValue, Rho.
  template <typename Mat>
  struct Lambda<Mat,
                typename LambdaValue<Mat>::type,
                std::enable_if_t<IsMaxPlusTruncMat<Mat>>> {
    using result_type = typename LambdaValue<Mat>::type;
    //! Modifies \p res to contain the row space basis of \p x.
    void operator()(result_type& res, Mat const& x) const {
      res.clear();
      matrix::row_basis_rows<Mat>(x, res);
    }
  };

  //! Specialization of the adapter Rho for instances of MaxPlusTruncMat<T, N>
  //! and StaticVector1<MaxPlusTruncMat<T, N>, N>.
  //!
  //! \sa Lambda, RhoValue.
  template <typename Mat>
  struct Rho<Mat,
             typename RhoValue<Mat>::type,
             std::enable_if_t<IsMaxPlusTruncMat<Mat>>> {
    using result_type = typename RhoValue<Mat>::type;
    //! Modifies \p res to contain the column space basis of \p x.
    void operator()(result_type& res, Mat const& x) const {
      // TODO this is inefficient
      const_cast<Mat*>(&x)->transpose();
      Lambda<Mat, result_type>()(res, x);
      const_cast<Mat*>(&x)->transpose();
    }
  };

  ////////////////////////////////////////////////////////////////////////
  // Rank - MaxPlusTruncMat
  ////////////////////////////////////////////////////////////////////////

  //! Specialization of the adapter Rank for instances of MaxPlusTruncMat
  //!
  //! \sa Rank.
  template <typename Mat>
  struct Rank<Mat, RankState<Mat>, std::enable_if_t<IsMaxPlusTruncMat<Mat>>> {
    //! Returns the size of the row space of x.
    size_t operator()(Mat const& x) const;
  };
}  // namespace libsemigroups

#include "max-plus-trunc-mat.tpp"

#endif  // LIBSEMIGROUPS_MAX_PLUS_TRUNC_MAT_HPP_
