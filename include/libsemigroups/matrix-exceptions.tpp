//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2026 James Mitchell
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope That it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

namespace libsemigroups {
  namespace matrix {
    template <typename Mat>
    void throw_if_not_square(Mat const& x, std::string_view arg_desc) {
      static_assert(IsMatrix<Mat>);
      if (x.number_of_rows() != x.number_of_cols()) {
        LIBSEMIGROUPS_EXCEPTION(
            "expected {} to be a square matrix, but found a {}x{} matrix",
            arg_desc,
            x.number_of_rows(),
            x.number_of_cols());
      }
    }

    template <typename Mat>
    void throw_if_bad_dim(Mat const&       x,
                          Mat const&       y,
                          std::string_view arg_desc_x,
                          std::string_view arg_desc_y) {
      static_assert(IsMatrix<Mat>);
      if (x.number_of_rows() != y.number_of_rows()
          || x.number_of_cols() != y.number_of_cols()) {
        LIBSEMIGROUPS_EXCEPTION(
            "expected matrices with the same dimensions, {} is a "
            "{}x{} matrix, and {} is a {}x{} matrix",
            arg_desc_x,
            x.number_of_rows(),
            x.number_of_cols(),
            arg_desc_y,
            y.number_of_rows(),
            y.number_of_cols());
      }
    }

    template <typename Mat>
    void throw_if_bad_coords(Mat const& x, size_t r, size_t c) {
      static_assert(IsMatrix<Mat>);
      if (r >= x.number_of_rows()) {
        LIBSEMIGROUPS_EXCEPTION("invalid row index in ({}, {}), expected "
                                "values in [0, {}) x [0, {})",
                                r,
                                c,
                                x.number_of_rows(),
                                x.number_of_cols(),
                                r);
      }
      if (c >= x.number_of_cols()) {
        LIBSEMIGROUPS_EXCEPTION("invalid column index in ({}, {}), expected "
                                "values in [0, {}) x [0, {})",
                                r,
                                c,
                                x.number_of_rows(),
                                x.number_of_cols(),
                                r);
      }
    }
  }  // namespace matrix
}  // namespace libsemigroups
