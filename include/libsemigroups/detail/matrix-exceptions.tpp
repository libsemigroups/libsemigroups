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

// This file contains implementations of exception throwing functions for use
// in matrix.hpp.

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

  namespace detail {
    template <typename Mat>
    void throw_if_semiring_nullptr(Mat const& m) {
      if (IsMatWithSemiring<Mat> && m.semiring() == nullptr) {
        LIBSEMIGROUPS_EXCEPTION(
            "the matrix's pointer to a semiring is nullptr!");
      }
    }

    template <typename Mat, typename Container>
    auto throw_if_bad_dim(Container const& m)
        -> std::enable_if_t<IsStaticMatrix<Mat>> {
      // Only call this if you've already called throw_if_any_row_wrong_size
      uint64_t const R = m.size();
      uint64_t const C = std::empty(m) ? 0 : m.begin()->size();
      if (R != Mat::nr_rows || C != Mat::nr_cols) {
        LIBSEMIGROUPS_EXCEPTION(
            "invalid argument, cannot initialize a {}x{} matrix with compile "
            "time dimension, with a {}x{} container",
            Mat::nr_rows,
            Mat::nr_cols,
            R,
            C);
      }
    }

    template <typename Mat, typename Container>
    auto throw_if_bad_row_dim(Container const& row)
        -> std::enable_if_t<IsStaticMatrix<Mat>> {
      uint64_t const C = row.size();
      if (C != Mat::nr_cols) {
        LIBSEMIGROUPS_EXCEPTION(
            "invalid argument, cannot initialize a row of a matrix with "
            "compile time number of columns {} with a container of size {}",
            Mat::nr_cols,
            C);
      }
    }

    template <typename Container>
    void throw_if_any_row_wrong_size(Container const& m) {
      if (m.size() <= 1) {
        return;
      }
      uint64_t const C  = m.begin()->size();
      auto           it = std::find_if_not(
          m.begin() + 1, m.end(), [&C](typename Container::const_reference r) {
            return r.size() == C;
          });
      if (it != m.end()) {
        LIBSEMIGROUPS_EXCEPTION("invalid argument, expected every item to "
                                "have length {}, found {} in entry {}",
                                C,
                                it->size(),
                                std::distance(m.begin(), it));
      }
    }
  }  // namespace detail
}  // namespace libsemigroups
