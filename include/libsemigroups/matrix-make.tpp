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

namespace libsemigroups {

  template <typename Mat, typename>
  Mat make(std::vector<std::vector<typename Mat::scalar_type>> const& rows) {
    detail::throw_if_any_row_wrong_size(rows);
    detail::throw_if_bad_dim<Mat>(rows);
    Mat m(rows);
    matrix::throw_if_bad_entry(m);
    return m;
  }

  template <typename Mat, typename>
  Mat make(std::initializer_list<typename Mat::scalar_type> const& row) {
    detail::throw_if_bad_row_dim<Mat>(row);
    Mat m(row);
    matrix::throw_if_bad_entry(m);
    return m;
  }

  template <typename Mat, typename Semiring, typename>
  // TODO(1) pass Semiring by reference, this is hard mostly due to the way
  // the tests are written, which is not optimal.
  Mat make(Semiring const* semiring,
           std::initializer_list<
               std::initializer_list<typename Mat::scalar_type>> const& rows) {
    detail::throw_if_any_row_wrong_size(rows);
    detail::throw_if_bad_dim<Mat>(rows);
    Mat m(semiring, rows);
    matrix::throw_if_bad_entry(m);
    return m;
  }

  template <typename Mat, typename Semiring, typename>
  Mat make(Semiring const*                                            semiring,
           std::vector<std::vector<typename Mat::scalar_type>> const& rows) {
    detail::throw_if_any_row_wrong_size(rows);
    detail::throw_if_bad_dim<Mat>(rows);
    Mat m(semiring, rows);
    matrix::throw_if_bad_entry(m);
    return m;
  }

  template <typename Mat, typename Semiring, typename>
  Mat make(Semiring const*                                         semiring,
           std::initializer_list<typename Mat::scalar_type> const& row) {
    detail::throw_if_bad_row_dim<Mat>(row);
    Mat m(semiring, row);
    matrix::throw_if_bad_entry(m);
    return m;
  }

}  // namespace libsemigroups
