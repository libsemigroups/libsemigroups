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

#ifndef LIBSEMIGROUPS_MATRIX_MAKE_HPP_
#define LIBSEMIGROUPS_MATRIX_MAKE_HPP_

#include <cstddef>           // for size_t
#include <initializer_list>  // for initializer_list
#include <type_traits>       // for enable_if_t
#include <vector>            // for vector

#include "is-matrix.hpp"          // for IsMatrix, IsMa...
#include "matrix-class.hpp"       // for ProjMaxPlusMat
#include "matrix-exceptions.hpp"  // for throw_if_bad_e...

#include "libsemigroups/detail/matrix-exceptions.hpp"  // for throw_if_any_r

namespace libsemigroups {
  //! \defgroup make_matrix_group make<Matrix>
  //! \ingroup matrix_group
  //!
  //! \brief Safely construct a \ref matrix_group "Matrix" instance.
  //!
  //! This page contains documentation related to safely constructing a
  //! \ref matrix_group "Matrix" instance.
  //!
  //! \sa \ref make_group for an overview of possible uses of the this
  //! function.

  //! \ingroup make_matrix_group
  //!
  //! \brief Checks the arguments, constructs a matrix, and checks it.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! Checks the arguments, constructs a matrix, and checks it.
  //!
  //! \tparam Mat the type of matrix being constructed, must satisfy
  //! \ref IsMatrix<Mat> and not \ref IsMatWithSemiring<Mat>.
  //!
  //! \param rows the values to be copied into the matrix.
  //! \returns The constructed matrix if valid.
  //!
  //! \throws
  //! LibsemigroupsException if `rows` does not represent a
  //! matrix of the correct dimensions.
  //!
  //! \throws
  //! LibsemigroupsException if the constructed matrix
  //! contains values that do not belong to the underlying semiring.
  //!
  //! \complexity
  //! \f$O(mn)\f$ where \f$m\f$ is the number of rows and
  //! \f$n\f$ is the number of columns of the matrix.
  template <typename Mat,
            typename
            = std::enable_if_t<IsMatrix<Mat> && !IsMatWithSemiring<Mat>>>
  [[nodiscard]] Mat
  make(std::vector<std::vector<typename Mat::scalar_type>> const& rows);

  //! \ingroup make_matrix_group
  //!
  //! \brief Checks the arguments, constructs a matrix, and checks it.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! Checks the arguments, constructs a matrix, and checks it.
  //!
  //! \tparam Mat the type of matrix being constructed, must satisfy
  //! \ref IsMatrix<Mat> and not \ref IsMatWithSemiring<Mat>.
  //!
  //! \param rows the values to be copied into the matrix.
  //! \returns The constructed matrix if valid.
  //!
  //! \throws
  //! LibsemigroupsException if `rows` does not represent a
  //! matrix of the correct dimensions.
  //!
  //! \throws
  //! LibsemigroupsException if the constructed matrix
  //! contains values that do not belong to the underlying semiring.
  //!
  //! \complexity
  //! \f$O(mn)\f$ where \f$m\f$ is the number of rows and
  //! \f$n\f$ is the number of columns of the matrix.
  template <typename Mat,
            typename
            = std::enable_if_t<IsMatrix<Mat> && !IsMatWithSemiring<Mat>>>
  [[nodiscard]] Mat
  make(std::initializer_list<std::vector<typename Mat::scalar_type>> const&
           rows) {
    return make<Mat>(std::vector<std::vector<typename Mat::scalar_type>>(rows));
  }

  //! \ingroup make_matrix_group
  //!
  //! \brief Constructs a row and checks it.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! This function constructs a row from a std::initializer_list and then
  //! calls \ref matrix::throw_if_bad_entry.
  //!
  //! \tparam Mat the type of matrix being constructed, must satisfy
  //! \ref IsMatrix<Mat> and not \ref IsMatWithSemiring<Mat>.
  //!
  //! \param row the values to be copied into the row.
  //!
  //! \returns the constructed row if valid.
  //!
  //! \throws
  //! LibsemigroupsException if the constructed row contains
  //! values that do not belong to the underlying semiring.
  //!
  //! \complexity
  //! \f$O(n)\f$ where \f$n\f$ is the number of columns of the matrix.
  //!
  //! \warning
  //! This function only works for rows, i.e. when the template
  //! parameter \c R is \c 1.
  template <typename Mat,
            typename
            = std::enable_if_t<IsMatrix<Mat> && !IsMatWithSemiring<Mat>>>
  // TODO(1) vector version
  [[nodiscard]] Mat
  make(std::initializer_list<typename Mat::scalar_type> const& row);

  //! \ingroup make_matrix_group
  //!
  //! \brief Constructs a matrix (from std::initializer_list) and checks
  //! it.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! Checks the arguments, constructs a matrix and checks it.
  //!
  //! \tparam Mat the type of the matrix being constructed (must satisfy
  //! \ref IsMatrix<Mat>).
  //!
  //! \tparam Semiring the type of the semiring where arithmetic is performed.
  //!
  //! \param semiring  a pointer to const semiring object.
  //!
  //! \param rows  the values to be copied into the matrix.
  //!
  //! \returns The constructed matrix.
  //!
  //! \throws
  //! LibsemigroupsException if `rows` does not represent a matrix of the
  //! correct dimensions.
  //!
  //! \throws
  //! LibsemigroupsException if the constructed matrix contains values that do
  //! not belong to the underlying semiring.
  //!
  //! \complexity
  //! \f$O(mn)\f$ where \f$m\f$ is the number of rows and
  //! \f$n\f$ is the number of columns of the matrix.
  template <typename Mat,
            typename Semiring,
            typename = std::enable_if_t<IsMatrix<Mat>>>
  // TODO(1) pass Semiring by reference, this is hard mostly due to the way
  // the tests are written, which is not optimal.
  [[nodiscard]] Mat
  make(Semiring const* semiring,
       std::initializer_list<
           std::initializer_list<typename Mat::scalar_type>> const& rows);

  //! \ingroup make_matrix_group
  //!
  //! \brief Constructs a matrix (from std::vector of std::vector) and
  //! checks it.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! Checks the arguments, constructs a matrix, and checks it.
  //!
  //! \tparam Mat the type of the matrix being constructed (must satisfy
  //! \ref IsMatrix).
  //!
  //! \tparam Semiring the type of the semiring where arithmetic is performed.
  //!
  //! \param semiring  a pointer to const semiring object.
  //!
  //! \param rows  the rows to be copied into the matrix.
  //!
  //! \returns The constructed matrix.
  //!
  //! \throws
  //! LibsemigroupsException if \p rows does not represent a
  //! matrix of the correct dimensions.
  //!
  //! \throws
  //! LibsemigroupsException if the constructed matrix
  //! contains values that do not belong to the underlying semiring.
  //!
  //! \complexity
  //! \f$O(mn)\f$ where \f$m\f$ is the number of rows and
  //! \f$n\f$ is the number of columns of the matrix.
  template <typename Mat,
            typename Semiring,
            typename = std::enable_if_t<IsMatrix<Mat>>>
  [[nodiscard]] Mat
  make(Semiring const*                                            semiring,
       std::vector<std::vector<typename Mat::scalar_type>> const& rows);

  //! \ingroup make_matrix_group
  //!
  //! \brief Constructs a row and checks it.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! This function constructs a row and checks it.
  //!
  //! \tparam Semiring the type of the semiring where arithmetic is performed.
  //!
  //! \param semiring  a pointer to const semiring object.
  //!
  //! \param row  the values to be copied into the row.
  //!
  //! \returns  the constructed row if valid.
  //!
  //! \throws
  //! LibsemigroupsException if the constructed row contains
  //! values that do not belong to the underlying semiring.
  //!
  //! \complexity
  //! \f$O(n)\f$ where \f$n\f$ is the number of columns of the matrix.
  template <typename Mat,
            typename Semiring,
            typename = std::enable_if_t<IsMatrix<Mat>>>
  [[nodiscard]] Mat
  make(Semiring const*                                         semiring,
       std::initializer_list<typename Mat::scalar_type> const& row);

  //! \ingroup make_matrix_group
  //!
  //! \brief Constructs a projective max-plus matrix (from
  //! std::initializer_list) and checks it.
  //!
  //! Defined in `matrix.hpp`.
  //!
  //! Checks the arguments, constructs a matrix, and checks it.
  //!
  //! \tparam Mat the type of the matrix being constructed (must satisfy
  //! \ref IsProjMaxPlusMat<Mat>).
  //!
  //! \param rows  the values to be copied into the matrix.
  //!
  //! \returns The constructed matrix.
  //!
  //! \throws
  //! LibsemigroupsException if `rows` does not represent a matrix of the
  //! correct dimensions.
  //!
  //! \throws
  //! LibsemigroupsException if the constructed matrix contains values that do
  //! not belong to the underlying semiring.
  //!
  //! \complexity
  //! \f$O(mn)\f$ where \f$m\f$ is the number of rows and \f$n\f$ is the
  //! number of columns of the matrix.
  template <size_t R, size_t C, typename Scalar>
  [[nodiscard]] ProjMaxPlusMat<R, C, Scalar>
  make(std::initializer_list<std::initializer_list<Scalar>> const& rows) {
    return ProjMaxPlusMat<R, C, Scalar>(
        make<ProjMaxPlusMat<R, C, Scalar>::underlying_matrix_type>(rows));
  }
}  // namespace libsemigroups

#include "matrix-make.tpp"
#endif  // LIBSEMIGROUPS_MATRIX_MAKE_HPP_
