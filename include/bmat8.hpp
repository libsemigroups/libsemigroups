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

// This file contains a declaration of fast boolean matrices up to dimension 8.

#ifndef LIBSEMIGROUPS_INCLUDE_BMAT8_HPP_
#define LIBSEMIGROUPS_INCLUDE_BMAT8_HPP_

#include <algorithm>  // for uniform_int_distribution, swap
#include <climits>    // for CHAR_BIT
#include <cstddef>    // for size_t
#include <cstdint>    // for uint64_t
#include <iostream>   // for operator<<, ostringstream
#include <random>     // for mt19937, random_device
#include <utility>    // for hash
#include <vector>     // for vector

#include "adapters.hpp"             // for complexity, degree, etc . . .
#include "libsemigroups-debug.hpp"  // for LIBSEMIGROUPS_ASSERT
#include "stl.hpp"                  // for internal::to_string

namespace libsemigroups {

  //! Class for fast boolean matrices of dimension up to 8 x 8
  //!
  //! The methods for these small matrices over the boolean semiring
  //! are more optimised than the generic methods for boolean matrices.
  //! Note that all BMat8 are represented internally as an 8 x 8 matrix;
  //! any entries not defined by the user are taken to be 0. This does
  //! not affect the results of any calculations.
  //!
  //! BMat8 is a trivial class.
  class BMat8 {
   public:
    //! A default constructor.
    //!
    //! This constructor gives no guarantees on what the matrix will contain.
    BMat8() = default;

    //! A constructor.
    //!
    //! This constructor initializes a BMat8 to have rows equal to the
    //! 8 chunks, of 8 bits each, of the binary representation of \p mat.
    explicit BMat8(uint64_t mat) : _data(mat) {}

    //! A constructor.
    //!
    //! This constructor initializes a matrix where the rows of the matrix
    //! are the vectors in \p mat.
    explicit BMat8(std::vector<std::vector<bool>> const& mat);

    //! A constructor.
    //!
    //! This is the copy constructor.
    BMat8(BMat8 const&) = default;

    //! A constructor.
    //!
    //! This is the move constructor.
    BMat8(BMat8&&) = default;

    //! A constructor.
    //!
    //! This is the copy assignement constructor.
    BMat8& operator=(BMat8 const&) = default;

    //! A constructor.
    //!
    //! This is the move assignment  constructor.
    BMat8& operator=(BMat8&&) = default;

    //! A default destructor.
    ~BMat8() = default;

    //! Returns \c true if \c this equals \p that.
    //!
    //! This method checks the mathematical equality of two BMat8 objects.
    bool operator==(BMat8 const& that) const {
      return _data == that._data;
    }

    //! Returns \c true if \c this does not equal \p that
    //!
    //! This method checks the mathematical inequality of two BMat8 objects.
    bool operator!=(BMat8 const& that) const {
      return _data != that._data;
    }

    //! Returns \c true if \c this is less than \p that.
    //!
    //! This method checks whether a BMat8 objects is less than another.
    //! We order by the results of to_int() for each matrix.
    bool operator<(BMat8 const& that) const {
      return _data < that._data;
    }

    //! Returns \c true if \c this is greater than \p that.
    //!
    //! This method checks whether a BMat8 objects is greater than another.
    //! We order by the results of to_int() for each matrix.
    bool operator>(BMat8 const& that) const {
      return _data > that._data;
    }

    //! Returns the entry in the (\p i, \p j)th position.
    //!
    //! This method returns the entry in the (\p i, \p j)th position.
    //! Note that since all matrices are internally represented as 8 x 8, it
    //! is possible to access entries that you might not believe exist.
    bool operator()(size_t i, size_t j) const {
      LIBSEMIGROUPS_ASSERT(i < 8);
      LIBSEMIGROUPS_ASSERT(j < 8);
      return (_data << (8 * i + j)) >> 63;
    }

    //! Sets the (\p i, \p j)th position to \p val.
    //!
    //! This method sets the (\p i, \p j)th entry of \c this to \p val.
    //! Uses the bit twiddle for setting bits found
    //! <a href=http://graphics.stanford.edu/~seander/bithacks>here</a>.
    void set(size_t, size_t, bool);

    //! Returns the integer representation of \c this.
    //!
    //! Returns an unsigned integer obtained by interpreting an 8 x 8
    //! BMat8 as a sequence of 64 bits (reading rows left to right,
    //! from top to bottom) and then this sequence as an unsigned int.
    inline uint64_t to_int() const {
      return _data;
    }

    //! Returns the transpose of \c this
    //!
    //! Returns the standard matrix transpose of a BMat8.
    //! Uses the technique found in Knuth AoCP Vol. 4 Fasc. 1a, p. 15.
    inline BMat8 transpose() const {
      uint64_t x = _data;
      uint64_t y = (x ^ (x >> 7)) & 0xAA00AA00AA00AA;
      x          = x ^ y ^ (y << 7);
      y          = (x ^ (x >> 14)) & 0xCCCC0000CCCC;
      x          = x ^ y ^ (y << 14);
      y          = (x ^ (x >> 28)) & 0xF0F0F0F0;
      x          = x ^ y ^ (y << 28);
      return BMat8(x);
    }

    //! Returns the matrix product of \c this and \p that
    //!
    //! This method returns the standard matrix product (over the
    //! boolean semiring) of two BMat8 objects.
    //! Uses the technique given <a href="https://stackoverflow.com/a/18448513">
    //! here</a>.
    BMat8 operator*(BMat8 const& that) const;

    //! Insertion operator
    //!
    //! This method allows BMat8 objects to be inserted into an ostringstream
    friend std::ostringstream& operator<<(std::ostringstream& os,
                                          BMat8 const&        bm) {
      uint64_t x   = bm._data;
      uint64_t pow = 1;
      pow          = pow << 63;
      for (size_t i = 0; i < 8; ++i) {
        for (size_t j = 0; j < 8; ++j) {
          if (pow & x) {
            os << "1";
          } else {
            os << "0";
          }
          x = x << 1;
        }
        os << "\n";
      }
      return os;
    }

    //! Insertion operator
    //!
    //! This method allows BMat8 objects to be inserted into a ostream.
    friend std::ostream& operator<<(std::ostream& os, BMat8 const& bm) {
      os << internal::to_string(bm);
      return os;
    }

    //! Returns a random BMat8
    //!
    //! This method returns a BMat8 chosen at random.
    static BMat8 random();

    //! Returns a random square BMat8 up to dimension \p dim.
    //!
    //! This method returns a BMat8 chosen at random, where only the
    //! top-left \p dim x \p dim entries may be non-zero.
    static BMat8 random(size_t dim);

    inline void swap(BMat8& that) {
      std::swap(this->_data, that._data);
    }

#ifdef LIBSEMIGROUPS_DENSEHASHMAP
    // FIXME do this another way
    BMat8 empty_key() const {
      return BMat8(0xFF7FBFDFEFF7FBFE);
    }
#endif

    BMat8 row_space_basis() const;
    BMat8 col_space_basis() const;

    std::vector<uint8_t> rows() const {
      std::vector<uint8_t> rows;
      for (size_t i = 0; i < 8; ++i) {
        uint8_t row = static_cast<uint8_t>(_data << (8 * i) >> 56);
        rows.push_back(row);
      }
      return rows;
    }

    size_t row_space_size() const;
    size_t col_space_size() const {
      return transpose().row_space_size();
    }

    //! Returns the number of non-zero rows in \c this.
    //!
    //! BMat8s do not know their "dimension" - in effect they are all of
    //! dimension 8. However, this method can be used to obtain the number of
    //! non-zero rows of \c this.
    size_t nr_rows() const {
      size_t count = 0;
      for (size_t i = 0; i < 8; ++i) {
        if (_data << (8 * i) >> 56 > 0) {
          count++;
        }
      }
      return count;
    }

    //! Returns the number of non-zero rows in \c this
    //!
    //! BMat8s do not know their "dimension" - in effect they are all of
    //! dimension 8. However, this method can be used to obtain the number of
    //! non-zero rows of \c this.
    size_t nr_cols() const {
      return transpose().nr_rows();
    }

    //! Returns whether \c this is a regular element of the full boolean matrix
    //! monoid of appropriate size.
    bool is_regular_element() const {
      return *this
                 * BMat8(
                       ~(*this * BMat8(~_data).transpose() * (*this)).to_int())
                       .transpose()
                 * (*this)
             == *this;
    }

    size_t min_possible_dim() const {
      size_t i = 1;
      size_t x = transpose().to_int();
      while ((_data >> (8 * i)) << (8 * i) == _data
             && (x >> (8 * i)) << (8 * i) == x && i < 9) {
        ++i;
      }
      return 9 - i;
    }

    static bool is_group_index(BMat8 const& x, BMat8 const& y);

    //! Returns the identity BMat8
    //!
    //! This method returns the dim x dim BMat8 with 1s on the main diagonal.
    static BMat8 one(size_t dim = 8);

   private:
    void sort_rows();

    uint64_t                                       _data;
    static std::random_device                      _rd;
    static std::mt19937                            _gen;
    static std::uniform_int_distribution<uint64_t> _dist;
  };

  // Specialization for adapters.hpp structs
  template <>
  struct complexity<BMat8> {
    constexpr inline size_t operator()(BMat8 const&) const noexcept {
      return 0;
    }
  };

  template <>
  struct degree<BMat8> {
    constexpr inline size_t operator()(BMat8 const&) const noexcept {
      return 8;
    }
  };

  template <>
  struct increase_degree_by<BMat8> {
    inline void operator()(BMat8 const&) const noexcept {}
  };

  template <>
  struct less<BMat8> {
    inline bool operator()(BMat8 const& x, BMat8 const& y) const noexcept {
      return x < y;
    }
  };

  template <>
  struct one<BMat8> {
    inline BMat8 operator()(BMat8 const& x) const noexcept {
      return x.one();
    }

    inline BMat8 operator()(size_t = 0) const noexcept {
      return BMat8::one();
    }
  };

  template <>
  struct product<BMat8> {
    inline void
    operator()(BMat8& xy, BMat8 const& x, BMat8 const& y, size_t = 0) const
        noexcept {
      xy = x * y;
    }
  };

  // The following is comment out since lz_cnt is unreliable, this serves as a
  // POC only.

  // template <typename TIndexType> struct action<BMat8, TIndexType> {
  //   inline TIndexType operator()(BMat8 const& x, TIndexType const i) const
  //       noexcept {
  //     LIBSEMIGROUPS_ASSERT(0 <= i && i < 8);
  //     return _lzcnt_u64(x.to_int() << (8 * i));
  //   }
  // };

  template <>
  struct inverse<BMat8> {
    inline BMat8 operator()(BMat8 const& x) const noexcept {
      LIBSEMIGROUPS_ASSERT(x * x.transpose() == x.one());
      return x.transpose();
    }
  };
}  // namespace libsemigroups

namespace std {
  template <>
  struct hash<libsemigroups::BMat8> {
    size_t operator()(libsemigroups::BMat8 const& bm) const {
      return hash<uint64_t>()(bm.to_int());
    }
  };
}  // namespace std
#endif  // LIBSEMIGROUPS_INCLUDE_BMAT8_HPP_
