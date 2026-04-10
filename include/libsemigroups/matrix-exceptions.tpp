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
  }  // namespace matrix
}  // namespace libsemigroups
