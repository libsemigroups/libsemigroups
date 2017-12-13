//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2017 Finn Smith
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

// This file contains a declaration of fast boolean matrices up to dimension 8
// x 8.

#ifndef LIBSEMIGROUPS_SRC_BMAT_H_
#define LIBSEMIGROUPS_SRC_BMAT_H_

#include <functional>
#include <iostream>
#include <random>

namespace libsemigroups {
  class BMat8 {
   public:
    BMat8() = default;
    explicit BMat8(uint64_t mat) : _data(mat) {}
    explicit BMat8(std::vector<std::vector<size_t>> const& mat);

    friend std::ostream& operator<<(std::ostream& os, BMat8 const& bm) {
      os << bm.to_string();
      return os;
    }

    BMat8 operator*(BMat8 const& that) const;

    bool operator==(BMat8 const& that) const {
      return _data == that._data;
    }

    bool operator!=(BMat8 const& that) const {
      return _data != that._data;
    }

    bool operator<(BMat8 const& that) const {
      return _data < that._data;
    }

    bool operator>(BMat8 const& that) const {
      return _data > that._data;
    }

    bool operator()(size_t i, size_t j) const;

    std::string to_string() const;
    uint64_t    to_int() const {
      return _data;
    }
    BMat8 row_space_basis() const;
    BMat8 col_space_basis() const;
    BMat8 transpose() const;
    void redefine(BMat8 const& A, BMat8 const& B);
    void sort_rows();
    void swap_rows(size_t i, size_t j);
    BMat8 one() const;

    size_t complexity() const {
      return 0;
    }

    size_t degree() const {
      return 8;
    }

    static BMat8 random();
    static BMat8 random(size_t dim);

    BMat8 lvalue(BMat8 rows, BMat8 tmp);

   private:
    uint64_t _data;

    static std::mt19937                          _gen;
    static std::uniform_int_distribution<size_t> _dist;
  };

}  // namespace libsemigroups

namespace std {
  template <> struct hash<libsemigroups::BMat8> {
    inline size_t operator()(libsemigroups::BMat8 const& bm) const {
      return hash<uint64_t>()(bm.to_int());
    }
  };
}  // namespace std
#endif  // LIBSEMIGROUPS_SRC_BMAT_H_
