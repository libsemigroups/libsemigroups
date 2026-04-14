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

#ifndef LIBSEMIGROUPS_MATRIX_FMT_HPP_
#define LIBSEMIGROUPS_MATRIX_FMT_HPP_

#include <algorithm>    // for max_element
#include <cstddef>      // for size_t
#include <ostream>      // for operator<<, basic_...
#include <sstream>      // for basic_ostringstream
#include <string>       // for basic_string, string
#include <type_traits>  // for enable_if_t
#include <vector>       // for vector

#include "exception.hpp"       // for LIBSEMIGROUPS_EXCE...
#include "is-matrix.hpp"       // for IsMatrix
#include "matrix-helpers.hpp"  // for rows

#include "detail/fmt.hpp"            // for format
#include "detail/matrix-common.hpp"  // for entry_repr
#include "detail/string.hpp"         // for unicode_string_length

namespace libsemigroups {
  namespace detail {
    // Forward decl.
    template <typename Mat, typename Subclass>
    class RowViewCommon;
  }  // namespace detail

  //! \ingroup matrix_group
  //!
  //! \brief Insertion operator for row views.
  //!
  //! This function inserts a human readable representation of a row view into
  //! the string stream \p os.
  //!
  //! \param os the string stream.
  //! \param x the row view.
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  template <typename Mat, typename Subclass>
  std::ostringstream& operator<<(std::ostringstream&                         os,
                                 detail::RowViewCommon<Mat, Subclass> const& x);

  //! \ingroup matrix_group
  //!
  //! \brief Insertion operator for matrices.
  //!
  //! This function inserts a human readable representation of a matrix into
  //! the string stream \p os.
  //!
  //! \tparam Mat the type of the argument \p x, must satisfy
  //! \ref IsMatrix<Mat>.
  //!
  //! \param os the string stream.
  //!
  //! \param x the matrix.
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  template <typename Mat>
  auto operator<<(std::ostringstream& os, Mat const& x)
      -> std::enable_if_t<IsMatrix<Mat>, std::ostringstream&>;

  //! \ingroup matrix_group
  //!
  //! \brief Returns a human readable representation of a matrix.
  //!
  //! This function returns a human readable representation of a matrix.
  //!
  //! \tparam Mat the type of the matrix, must satisfy \ref IsMatrix<Mat>.
  //!
  //! \param x the matrix.
  //! \param prefix the prefix for the returned string.
  //! \param short_name the short name of the type of matrix (default: `""`).
  //! \param braces the braces to use to delineate rows (default: `"{}"`).
  //! \param max_width the maximum width of the returned representation
  //! (default: \c 72).
  //!
  //! \throws LibsemigroupsException if \p braces does not have size \c 2.
  template <typename Mat>
  [[nodiscard]] auto to_human_readable_repr(Mat const&         x,
                                            std::string const& prefix,
                                            std::string const& short_name = "",
                                            std::string const& braces    = "{}",
                                            size_t             max_width = 72)
      -> std::enable_if_t<IsMatrix<Mat>, std::string>;
}  // namespace libsemigroups

#include "matrix-fmt.tpp"
#endif  // LIBSEMIGROUPS_MATRIX_FMT_HPP_
