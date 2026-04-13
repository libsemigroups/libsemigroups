//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2026 James D. Mitchell
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

#ifndef LIBSEMIGROUPS_MATRIX_ADAPTERS_HPP_
#define LIBSEMIGROUPS_MATRIX_ADAPTERS_HPP_

#include <cstddef>      // for size_t
#include <type_traits>  // for enable_if_t

#include "adapters.hpp"   // for Complexity etc
#include "debug.hpp"      // for LIBSEMIGROUPS_ASSERT
#include "is-matrix.hpp"  // for IsMatrix

namespace libsemigroups {

  //! \defgroup adapters_matrix_group Adapters for other matrices
  //!
  //! \brief Documentation for specialization of adapters for matrices
  //! satisfying \ref IsMatrix.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! This page contains the documentation of the functionality in
  //! `libsemigroups` that adapts  objects satisfying \ref IsMatrix for
  //! use with the main algorithms in `libsemigroups`.
  //!
  //! Other adapters specifically for \ref BMat objects are available
  //! \ref adapters_bmat_group "here" and for \ref BMat8 objects
  //! \ref adapters_bmat8_group "here".

  //! \ingroup adapters_matrix_group
  //!
  //! \brief Specialization of \ref Complexity for \ref IsMatrix.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! Specialization of the adapter \ref Complexity for types \p Mat
  //! satisfying \ref IsMatrix<Mat>.
  //!
  //! \tparam  Mat the type of matrices.
  template <typename Mat>
  struct Complexity<Mat, std::enable_if_t<IsMatrix<Mat>>> {
    //! \brief Call operator.
    //!
    //! Returns `x.number_of_rows()` cubed.
    //!
    //! \param x  a matrix of type `Mat`.
    //!
    //! \returns  A value of type `size_t`.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    [[nodiscard]] constexpr size_t operator()(Mat const& x) const noexcept {
      return x.number_of_rows() * x.number_of_rows() * x.number_of_rows();
    }
  };

  //! \ingroup adapters_matrix_group
  //!
  //! \brief Specialization of \ref Degree for \ref IsMatrix.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! Specialization of the adapter \ref Degree for types \p Mat satisfying
  //! \ref IsMatrix<Mat>.
  //!
  //! \tparam  Mat the type of matrices.
  template <typename Mat>
  struct Degree<Mat, std::enable_if_t<IsMatrix<Mat>>> {
    //! \brief Call operator.
    //!
    //! This function returns `x.number_of_rows()`.
    //!
    //! \param x  a matrix of type `Mat`.
    //! \returns  The number of rows of \p x.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    [[nodiscard]] constexpr size_t operator()(Mat const& x) const noexcept {
      return x.number_of_rows();
    }
  };

  //! \ingroup adapters_matrix_group
  //!
  //! \brief Specialization of \ref Hash for \ref IsMatrix.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! Specialization of the adapter \ref Hash for types \p Mat satisfying
  //! \ref IsMatrix<Mat>.
  //!
  //! \tparam  Mat the type of matrices.
  template <typename Mat>
  struct Hash<Mat, std::enable_if_t<IsMatrix<Mat>>> {
    //! \brief Call operator.
    //!
    //! This function returns `x.hash_value()`.
    //!
    //! \param x  a matrix of type `Mat`.
    //! \returns  A hash value for \p x.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Constant.
    [[nodiscard]] constexpr size_t operator()(Mat const& x) const {
      return x.hash_value();
    }
  };

  //! \ingroup adapters_matrix_group
  //!
  //! \brief Specialization of \ref IncreaseDegree for \ref IsMatrix.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! Specialization of the adapter \ref IncreaseDegree for types \p Mat
  //! satisfying \ref IsMatrix<Mat>.
  //!
  //! \tparam Mat the type of matrices.
  //!
  //! \warning
  //! It is not possible to increase the degree of any of the types
  //! satisfying \ref IsMatrix, and as such the call operator of this type
  //! does nothing.
  template <typename Mat>
  struct IncreaseDegree<Mat, std::enable_if_t<IsMatrix<Mat>>> {
    //! \brief Call operator.
    //!
    //! This function does nothing and should not be used.
    constexpr void operator()(Mat&, size_t) const noexcept {
      // static_assert(false, "Cannot increase degree for Matrix");
      LIBSEMIGROUPS_ASSERT(false);
    }
  };

  //! \ingroup adapters_matrix_group
  //!
  //! \brief Specialization of \ref One for \ref IsMatrix.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! Specialization of the adapter \ref One for types \p Mat satisfying
  //! \ref IsMatrix.
  //!
  //! \tparam Mat the type of matrices.
  //!
  //! \note There is no `operator()(size_t)` meaning it isn't possible to use
  //! elements of type \p Mat with \ref SchreierSims.
  template <typename Mat>
  struct One<Mat, std::enable_if_t<IsMatrix<Mat>>> {
    //! \brief Call operator.
    //!
    //! Returns the identity matrix.
    //!
    //! \param x  a matrix of type \p Mat.
    //! \returns  The identity matrix of type \p Mat.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! \f$O(m ^ 2)\f$ where \f$m\f$ is the number of rows of the
    //! matrix \p x.
    [[nodiscard]] inline Mat operator()(Mat const& x) const {
      return x.one();
    }
  };

  //! \ingroup adapters_matrix_group
  //!
  //! \brief Specialization of \ref Product for \ref IsMatrix.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! Specialization of the adapter \ref Product for types \p Mat satisfying
  //! \ref IsMatrix<Mat>.
  //!
  //! \tparam Mat the type of matrices.
  template <typename Mat>
  struct Product<Mat, std::enable_if_t<IsMatrix<Mat>>> {
    //! \brief Call operator.
    //!
    //! This function replaces the value of \p xy by the product of the
    //! matrices \p x and \p y.
    //!
    //! \param xy  a reference to a matrix of type \p Mat.
    //! \param x  a const reference to a matrix of type \p Mat.
    //! \param y  a const reference to a matrix of type \p Mat.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! \f$O(m ^ 2)\f$ where \f$m\f$ is the number of rows of the
    //! matrix \p x.
    //!
    //! \warning
    //! This function only works for square matrices.
    inline void
    operator()(Mat& xy, Mat const& x, Mat const& y, size_t = 0) const {
      xy.product_inplace_no_checks(x, y);
    }
  };
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_MATRIX_ADAPTERS_HPP_
