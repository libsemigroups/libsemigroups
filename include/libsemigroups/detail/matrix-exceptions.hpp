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

// This file contains declarations of exception throwing functions for use
// in matrix.hpp.

#ifndef LIBSEMIGROUPS_DETAIL_MATRIX_EXCEPTIONS_HPP_
#define LIBSEMIGROUPS_DETAIL_MATRIX_EXCEPTIONS_HPP_

#include <algorithm>         // for find_if_not
#include <initializer_list>  // for initializer_list
#include <iterator>          // for distance, empty
#include <stddef.h>          // for size_t
#include <stdint.h>          // for uint64_t
#include <string_view>       // for basic_string_view, string_view
#include <type_traits>       // for enable_if_t

#include "libsemigroups/exception.hpp"  // for LIBSEMIGROUPS_EXCEPTION
#include "libsemigroups/is-matrix.hpp"  // for IsStaticMatrix, IsDynamicMatrix

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
    // TODO deprecate + move to detail
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
    // TODO deprecate
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
    // TODO deprecate -> detail
    template <typename Mat>
    void throw_if_bad_coords(Mat const& x, size_t r, size_t c);
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

    template <typename Container>
    void throw_if_any_row_wrong_size(Container const& m);

    template <typename Scalar>
    void throw_if_any_row_wrong_size(
        std::initializer_list<std::initializer_list<Scalar>> m) {
      throw_if_any_row_wrong_size<
          std::initializer_list<std::initializer_list<Scalar>>>(m);
    }
  }  // namespace detail
}  // namespace libsemigroups

#include "matrix-exceptions.tpp"
#endif  // LIBSEMIGROUPS_DETAIL_MATRIX_EXCEPTIONS_HPP_
