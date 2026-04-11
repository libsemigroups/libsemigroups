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

// TODO(v4): it's not clear why some of these functions are in "matrix" and
// others in "detail", maybe better to move them all into detail, and then this
// file into the detail dir?

#ifndef LIBSEMIGROUPS_MATRIX_EXCEPTIONS_HPP_
#define LIBSEMIGROUPS_MATRIX_EXCEPTIONS_HPP_

#include "exception.hpp"  // for LIBSEMIGROUPS_EXCEPTION
#include "is-matrix.hpp"  // for IsMatrix

#include "detail/matrix-common.hpp"  // for entry_repr

namespace libsemigroups {
  namespace matrix {

    //! \brief Throws if a matrix is not square.
    //!
    //! This function throws a LibsemigroupsException if the matrix \p x is not
    //! square.
    //!
    //! \tparam Mat the type of the argument, must satisfy \ref IsMatrix<Mat>.
    //!
    //! \param x the matrix to check.
    //! \param arg_desc a string_view that describes the argument being checked
    //! (defaults to `"the argument"`).
    //!
    //! \throws LibsemigroupsException if the number of rows in \p x does not
    //! equal the number of columns.
    template <typename Mat>
    void throw_if_not_square(Mat const&       x,
                             std::string_view arg_desc = "the argument");

    //! \brief Throws if two matrices do not have the same dimensions.
    //!
    //! This function throws a LibsemigroupsException if the matrices \p x and
    //! \p y do not have equal dimensions.
    //!
    //! \tparam Mat the type of the arguments, must satisfy \ref IsMatrix<Mat>.
    //!
    //! \param x the first matrix to check.
    //! \param y the second matrix to check.
    //! \param arg_desc_x a string_view that describes the argument \p x being
    //!        checked (defaults to `"the 1st argument"`).
    //! \param arg_desc_y a string_view that describes the argument \p y being
    //! checked (defaults to `"the 2nd argument"`).
    //!
    //! \throws LibsemigroupsException if the number of rows in \p x does not
    //! equal the number of rows of \p y; or the number of columns of \p x does
    //! not equal the number of columns of \p y.
    template <typename Mat>
    void throw_if_bad_dim(Mat const&       x,
                          Mat const&       y,
                          std::string_view arg_desc_x = "the 1st argument",
                          std::string_view arg_desc_y = "the 2nd argument");

    //! \brief Throws the arguments do not index an entry of a matrix.
    //!
    //! This function throws a LibsemigroupsException if \p r is not less than
    //! the number of rows of \p x; or if \p c is not less than the number of
    //! columns of \p x.
    //!
    //! \tparam Mat the type of the arguments, must satisfy \ref IsMatrix<Mat>.
    //!
    //! \param x the matrix.
    //! \param r the row index.
    //! \param c the column index.
    //!
    //! \throws LibsemigroupsException if `(r, c)` does not index an entry in
    //! the matrix \p x.
    template <typename Mat>
    void throw_if_bad_coords(Mat const& x, size_t r, size_t c);

    //! \ingroup intmat_group
    //!
    //! \brief Check that an integer matrix is valid.
    //!
    //! Defined in `matrix.hpp`.
    //!
    //! This function throws an exception if the entries of an integer matrix
    //! are not valid, which is if and only if any of the entries equal
    //! \ref POSITIVE_INFINITY or \ref NEGATIVE_INFINITY.
    //!
    //! \tparam Mat the type of the argument \p x, must satisfy
    //! \ref IsIntMat<Mat>.
    //!
    //! \param x the matrix to check.
    template <typename Mat>
    std::enable_if_t<IsIntMat<Mat>> throw_if_bad_entry(Mat const& x);

    //! \ingroup intmat_group
    //!
    //! \brief Check that an entry in an integer matrix is valid.
    //!
    //! Defined in `matrix.hpp`.
    //!
    //! This function throws an exception if the entry \p val of an integer
    //! matrix are not valid, which is if and only if the entry \p val equals
    //! \ref POSITIVE_INFINITY or \ref NEGATIVE_INFINITY.
    //!
    //! The 1st argument is used for overload resolution.
    //!
    //! \tparam Mat the type of the 1st argument, must satisfy
    //! \ref IsIntMat<Mat>.
    //!
    //! \param val the entry to check.
    //!
    //! \deprecated_warning{function}
    // NOTE this function isn't used and so should be deleted
    template <typename Mat>
    [[deprecated]] std::enable_if_t<IsIntMat<Mat>>
    throw_if_bad_entry(Mat const&, typename Mat::scalar_type val);
  }  // namespace matrix

  namespace detail {
    template <typename Mat>
    void throw_if_semiring_nullptr(Mat const& m);

    template <typename Mat, typename Container>
    auto throw_if_bad_dim(Container const& m)
        -> std::enable_if_t<IsStaticMatrix<Mat>>;

    // Not checking dynamic matrices, no compile-time dimensions.
    template <typename Mat, typename Container>
    auto throw_if_bad_dim(Container const&)
        -> std::enable_if_t<IsDynamicMatrix<Mat>> {}

    template <typename Mat, typename Container>
    auto throw_if_bad_row_dim(Container const& row)
        -> std::enable_if_t<IsStaticMatrix<Mat>>;

    template <typename Mat, typename Container>
    auto throw_if_bad_row_dim(Container const&)
        -> std::enable_if_t<IsDynamicMatrix<Mat>> {}
  }  // namespace detail
}  // namespace libsemigroups

#include "matrix-exceptions.tpp"
#endif  // LIBSEMIGROUPS_MATRIX_EXCEPTIONS_HPP_
