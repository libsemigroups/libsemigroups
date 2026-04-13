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

#include "matrix-class.hpp"
#include "matrix-helpers.hpp"

namespace libsemigroups {

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
  template <typename S, typename T>
  std::ostringstream& operator<<(std::ostringstream&                os,
                                 detail::RowViewCommon<S, T> const& x) {
    os << "{";
    for (auto it = x.cbegin(); it != x.cend(); ++it) {
      os << *it;
      if (it != x.cend() - 1) {
        os << ", ";
      }
    }
    os << "}";
    return os;
  }

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
      -> std::enable_if_t<IsMatrix<Mat>, std::ostringstream&> {
    size_t n = 0;
    if (x.number_of_rows() != 1) {
      os << "{";
    }
    for (auto&& r : matrix::rows(x)) {
      os << r;
      if (n != x.number_of_rows() - 1) {
        os << ", ";
      }
      n++;
    }
    if (x.number_of_rows() != 1) {
      os << "}";
    }
    return os;
  }

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
  auto to_human_readable_repr(Mat const&         x,
                              std::string const& prefix,
                              std::string const& short_name = "",
                              std::string const& braces     = "{}",
                              size_t             max_width  = 72)
      -> std::enable_if_t<IsMatrix<Mat>, std::string> {
    if (braces.size() != 2) {
      LIBSEMIGROUPS_EXCEPTION(
          "the 4th argument (braces) must have size 2, found {}",
          braces.size());
    }

    size_t const R = x.number_of_rows();
    size_t const C = x.number_of_cols();

    std::vector<size_t> max_col_widths(C, 0);
    std::vector<size_t> row_widths(C, prefix.size() + 1);
    for (size_t r = 0; r < R; ++r) {
      for (size_t c = 0; c < C; ++c) {
        size_t width
            = detail::unicode_string_length(detail::entry_repr(x(r, c)));
        row_widths[r] += width;
        if (width > max_col_widths[c]) {
          max_col_widths[c] = width;
        }
      }
    }
    auto col_width
        = *std::max_element(max_col_widths.begin(), max_col_widths.end());
    // The total width if we pad the entries according to the widest column.
    auto const total_width = col_width * C + prefix.size() + 1;
    if (total_width > max_width) {
      // Padding according to the widest column is too wide!
      if (*std::max_element(row_widths.begin(), row_widths.end()) > max_width) {
        // If the widest row is too wide, then use the short name
        return fmt::format(
            "<{}x{} {}>", x.number_of_rows(), x.number_of_cols(), short_name);
      }
      // If the widest row is not too wide, then just don't pad the entries
      col_width = 0;
    }

    std::string result = fmt::format("{}", prefix);
    std::string rindent;
    auto const  lbrace = braces[0], rbrace = braces[1];
    if (R != 0 && C != 0) {
      result += lbrace;
      for (size_t r = 0; r < R; ++r) {
        result += fmt::format("{}{}", rindent, lbrace);
        rindent          = std::string(prefix.size() + 1, ' ');
        std::string csep = "";
        for (size_t c = 0; c < C; ++c) {
          result += fmt::format(
              "{}{:>{}}", csep, detail::entry_repr(x(r, c)), col_width);
          csep = ", ";
        }
        result += fmt::format("{}", rbrace);
        if (r != R - 1) {
          result += ",\n";
        }
      }
      result += rbrace;
    }
    result += ")";
    return result;
  }
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_MATRIX_FMT_HPP_
