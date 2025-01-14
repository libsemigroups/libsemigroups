//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019-24 Finn Smith
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

// TODO
// * is it better to pass BMat8 by value rather than by const&?

#ifndef LIBSEMIGROUPS_BMAT8_HPP_
#define LIBSEMIGROUPS_BMAT8_HPP_

#include <array>        // for array
#include <cstddef>      // for size_t
#include <cstdint>      // for uint64_t
#include <functional>   // for hash
#include <iosfwd>       // for ostream, ostringstream
#include <string>       // for string
#include <string_view>  // for hash
#include <type_traits>  // for is_trivial
#include <utility>      // for swap
#include <vector>       // for vector

#include "adapters.hpp"  // for Complexity, Degree, etc . . .
#include "debug.hpp"     // for LIBSEMIGROUPS_ASSERT

namespace libsemigroups {

  //! \defgroup bmat_index_group Boolean matrices
  //!
  //! This page contains links to the documentation of the functionality in
  //! libsemigroups for boolean matrices.
  //!
  //! * \ref bmat8_group
  //! * \ref bmat_group
  //!
  //! # Boolean matrix helpers
  //!
  //! The following helper struct can be used to determine the smallest type of
  //! boolean matrix of specified dimension.
  //!
  //! * \ref BMatFastest

  //! \defgroup bmat8_group BMat8
  //! This page describes the class BMat8 that are an optimized
  //! representation of boolean matrices of dimension at most \f$8\f$.
  //!
  //! For boolean matrices of higher dimensions see \ref BMat.
  //!
  //! Adapters for \ref BMat8 objects are documented \ref adapters_bmat8_group
  //! "here".

  //! \ingroup bmat8_group
  //!
  //! Defined in `bmat8.hpp`.
  //!
  //! \brief Fast boolean matrices of dimension up to 8 x 8
  //!
  //! This class represents 8 x 8 matrices over the boolean semiring.
  //! The functions for these small matrices over the boolean semiring
  //! are more optimised than the generic functions for boolean matrices.
  //! Note that all BMat8 are represented internally as an 8 x 8 matrix; any
  //! entries not defined by the user are taken to be 0. This does not affect
  //! the results of any calculations.
  class BMat8 {
    uint64_t _data;
    // Proxy class for reference to bits in the matrix
    class BitRef {
     private:
      uint64_t& _data;
      uint64_t  _mask;

     public:
      // Constructor: takes a reference to a byte and the index of the bit in
      // that byte
      constexpr BitRef(uint64_t& data, size_t index) noexcept
          : _data(data), _mask(static_cast<uint64_t>(1) << (63 - index)) {}

      // Assignment operator to allow setting the bit through the proxy
      constexpr BitRef& operator=(bool val) noexcept {
        _data ^= (-val ^ _data) & _mask;
        return *this;
      }

      // Conversion operator to read the value of the bit as a boolean
      [[nodiscard]] constexpr operator bool() const {
        return _data & _mask;
      }
    };

   public:
    //! \brief Default constructor.
    //!
    //! There is no guarantee about the contents of the matrix constructed.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    BMat8() noexcept = default;

    //! \brief Construct from an integer.
    //!
    //! This constructor initializes a BMat8 to have rows equal to the
    //! 8 chunks, of 8 bits each, of the binary representation of \p mat.
    //!
    //! \param mat the integer representation of the matrix being constructed.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    constexpr explicit BMat8(uint64_t mat) noexcept : _data(mat) {}

    //! \brief A constructor.
    //!
    //! This constructor initializes a matrix where the rows of the matrix
    //! are the vectors in \p mat.
    //!
    //! \param mat the vector of vectors representation of the matrix being
    //! constructed.
    //!
    //! \throws LibsemigroupsException if \p mat has 0 rows.
    //! \throws LibsemigroupsException if \p mat has more than 8 rows.
    //! \throws LibsemigroupsException if the rows of \p mat are not all of the
    //! same length.
    //!
    //! \complexity
    //! Constant.
    explicit BMat8(std::vector<std::vector<bool>> const& mat);

    //! \brief Default copy constructor.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    constexpr BMat8(BMat8 const&) noexcept = default;

    //! \brief Default move constructor.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    constexpr BMat8(BMat8&&) noexcept = default;

    //! \brief Default copy assignment operator.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    constexpr BMat8& operator=(BMat8 const&) noexcept = default;

    //! \brief Default move assignment operator.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    constexpr BMat8& operator=(BMat8&&) noexcept = default;

    ~BMat8() = default;

    //! \brief Equality operator.
    //!
    //! Returns \c true if \c this equals \p that.
    //!
    //! This function checks the mathematical equality of two BMat8
    //! objects.
    //!
    //! \param that the BMat8 for comparison.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    [[nodiscard]] constexpr bool operator==(BMat8 const& that) const noexcept {
      return _data == that._data;
    }

    //! \brief Inequality operator.
    //!
    //! Returns \c true if \c this does not equal \p that
    //!
    //! This function checks the mathematical inequality of two BMat8
    //! objects.
    //!
    //! \param that the BMat8 for comparison.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    [[nodiscard]] constexpr bool operator!=(BMat8 const& that) const noexcept {
      return _data != that._data;
    }

    //! \brief Less than operator.
    //!
    //! Returns \c true if \c this is less than \p that.
    //!
    //! This function checks whether a BMat8 objects is less than
    //! another; where the order is defined by \ref to_int.
    //!
    //! \param that the BMat8 for comparison.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    [[nodiscard]] constexpr bool operator<(BMat8 const& that) const noexcept {
      return _data < that._data;
    }

    //! \brief Less than or equal operator.
    //!
    //! Returns \c true if \c this is less than or equal to \p that.
    //!
    //! This function checks whether a BMat8 objects is less or equal than
    //! another; where the order is defined by \ref to_int.
    //!
    //! \param that the BMat8 for comparison.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    [[nodiscard]] constexpr bool operator<=(BMat8 const& that) const noexcept {
      return *this < that || *this == that;
    }

    //! \brief Greater than operator.
    //!
    //! Returns \c true if \c this is greater than \p that.
    //!
    //! This function checks whether a BMat8 objects is greater than
    //! another; where the order is defined by \ref to_int.
    //!
    //! \param that the BMat8 for comparison.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    [[nodiscard]] constexpr bool operator>(BMat8 const& that) const noexcept {
      return _data > that._data;
    }

    //! \brief Greater than or equal operator.
    //!
    //! Returns \c true if \c this is greater than or equal to \p that.
    //!
    //! This function checks whether a BMat8 objects is greater or equal than
    //! another; where the order is defined by \ref to_int.
    //!
    //! \param that the BMat8 for comparison.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    [[nodiscard]] constexpr bool operator>=(BMat8 const& that) const noexcept {
      return *this > that || *this == that;
    }

    //! \brief Access entries in a matrix (no bound checks).
    //!
    //! Returns the entry in the matrix in row \p r and column \p c.
    //!
    //! \param r the index of the row.
    //! \param c the index of the column.
    //!
    //! \returns
    //! A `bool`.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    //!
    //! \note
    //! Note that since all matrices are internally represented as 8 x 8, it
    //! is possible to access entries that you might not believe exist.
    //!
    //! \warning
    //! No checks on the parameters \p r and \p c are performed, if \p r or \p
    //! c is greater than 7, then bad things will happen.
    //!
    //! \sa
    //! \ref at
    [[nodiscard]] constexpr bool operator()(size_t r, size_t c) const noexcept {
      LIBSEMIGROUPS_ASSERT(r < 8);
      LIBSEMIGROUPS_ASSERT(c < 8);
      return (_data << (8 * r + c)) >> 63;
    }

    //! \brief Access a row of a BMat8 (no bound checks).
    //!
    //! Returns the row with index \p r in the matrix represented as a \c
    //! uint8_t.
    //!
    //! \param r the index of the row.
    //!
    //! \returns
    //! A row of the matrix.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    //!
    //! \note
    //! Note that since all matrices are internally represented as 8 x 8, it
    //! is possible to access entries that you might not believe exist.
    //!
    //! \warning
    //! No checks on the parameter \p r are performed.
    //!
    //! \sa
    //! \ref at, \ref bmat8::rows, and \ref bmat8::to_vector.
    [[nodiscard]] constexpr uint8_t operator()(size_t r) const noexcept {
      LIBSEMIGROUPS_ASSERT(r < 8);
      return static_cast<uint8_t>(to_int() << 8 * r >> 56);
    }

    //! \brief Access a row of a BMat8 (with bound checks).
    //!
    //! Returns the row with index \p r in the matrix represented as a \c
    //! uint8_t.
    //!
    //! \param r the index of the row.
    //!
    //! \returns
    //! A row of the matrix.
    //!
    //! \throws LibsemigroupsException if \p r is out of bounds (i.e. \f$>=
    //! 8\f$).
    //!
    //! \complexity
    //! Constant.
    //!
    //! \note
    //! Note that since all matrices are internally represented as 8 x 8, it
    //! is possible to access entries that you might not believe exist.
    //!
    //! \sa
    //! \ref BMat8::operator()(size_t r) \ref bmat8::rows, and \ref
    //! bmat8::to_vector.
    [[nodiscard]] uint8_t at(size_t r) const;

    //! \brief Access entries in a matrix (no bound checks).
    //!
    //! Returns a reference to the entry in the matrix in row \p r and column
    //! \p c.
    //!
    //! \param r the index of the row.
    //! \param c the index of the column.
    //!
    //! \returns
    //! A reference to the entry.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    //!
    //! \note
    //! Note that since all matrices are internally represented as 8 x 8, it
    //! is possible to access entries that you might not believe exist.
    //!
    //! \warning
    //! No checks on the parameters \p r and \p c are performed, if \p r or \p
    //! c is greater than 7, then bad things will happen.
    //!
    //! \sa
    //! \ref at
    [[nodiscard]] constexpr BitRef operator()(size_t r, size_t c) {
      return BitRef(_data, 8 * r + c);
    }

    //! \brief Access entries in a matrix (with bound checks).
    //!
    //! Returns the value of the entry in the row \p r and column \p c.
    //!
    //! \param r the row
    //! \param c the column
    //!
    //! \throws LibsemigroupsException if \p r or \p c is out of bounds.
    //!
    //! \complexity
    //! Constant.
    //!
    //! \sa
    //! \ref BMat8::operator()()
    [[nodiscard]] bool at(size_t r, size_t c) const;

    //! \brief Access entries in a matrix (with bound checks).
    //!
    //! Returns a reference to the entry in the row \p r and column \p c.
    //!
    //! \param r the row
    //! \param c the column
    //!
    //! \throws LibsemigroupsException if \p r or \p c is out of bounds.
    //!
    //! \complexity
    //! Constant.
    //!
    //! \sa
    //! \ref BMat8::operator()()
    [[nodiscard]] BitRef at(size_t r, size_t c);

    //! \brief Returns the integer representation of \c this.
    //!
    //! Returns an unsigned integer obtained by interpreting an 8 x 8
    //! BMat8 as a sequence of 64 bits (reading rows left to right,
    //! from top to bottom) and then realising this sequence as an unsigned
    //! int.
    //!
    //! \returns
    //! A `uint64_t`.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    [[nodiscard]] constexpr inline uint64_t to_int() const noexcept {
      return _data;
    }

    //! \brief Returns the matrix product of \c this and \p that
    //!
    //! This function returns the standard matrix product (over the
    //! boolean semiring) of two BMat8 objects.
    //! Uses the technique given [here](https://stackoverflow.com/a/18448513).
    //!
    //! \param that the matrix we want to multiply by \c this.
    //!
    //! \returns
    //! A BMat8.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    [[nodiscard]] BMat8 operator*(BMat8 const& that) const noexcept;

    //! \brief Multiply a BMat8 by a scalar.
    //!
    //! This function returns the product of a BMat8 object and the boolean
    //! scalar value \p scalar.
    //!
    //! \param scalar the scalar.
    //!
    //! \returns
    //! A BMat8.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    [[nodiscard]] constexpr BMat8 operator*(bool scalar) const noexcept {
      if (scalar) {
        return *this;
      } else {
        return BMat8(0);
      }
    }

    //! \brief Multiply a BMat8 by a scalar (in-place)
    //!
    //! This function returns the product of a BMat8 object and the boolean
    //! scalar value \p scalar.
    //!
    //! \param scalar the scalar.
    //!
    //! \returns
    //! A reference to \c this.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    constexpr BMat8& operator*=(bool scalar) noexcept {
      if (!scalar) {
        _data = 0;
      }
      return *this;
    }

    //! \brief Sum BMat8 objects.
    //!
    //! This function returns the sum of two BMat8 objects.
    //!
    //! \param that the BMat8 to add.
    //!
    //! \returns
    //! The sum of `*this` and \c that.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    [[nodiscard]] constexpr BMat8 operator+(BMat8 const& that) const noexcept {
      return BMat8(_data | that._data);
    }

    //! \brief Sum BMat8 objects (in-place).
    //!
    //! This function adds \c that to `*this` in-place.
    //!
    //! \param that the BMat8 to add.
    //!
    //! \returns
    //! A reference to `*this`.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    constexpr BMat8& operator+=(BMat8 const& that) noexcept {
      _data |= that._data;
      return *this;
    }

    //! \brief Multiply \c this by \p that in-place.
    //!
    //! This function replaces the value of \c this by the standard matrix
    //! product (over the boolean semiring) of \c this and \p that.
    //!
    //! \param that the matrix we want to multiply by \c this.
    //!
    //! \returns
    //! A reference to `*this`.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    BMat8& operator*=(BMat8 const& that) noexcept;

    //! \brief Swaps \c this with \p that.
    //!
    //! This function swaps the values of \c this and \p that.
    //!
    //! \param that the BMat8 to swap \c this with.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    inline void swap(BMat8& that) noexcept {
      std::swap(this->_data, that._data);
    }
  };

  static_assert(std::is_trivial<BMat8>(), "BMat8 is not a trivial class!");

  //! \ingroup bmat8_group
  //!
  //! \brief Namespace for BMat8 helper functions.
  //!
  //! Defined in `bmat8.hpp`.
  //!
  //! This namespace contains various helper functions for the class BMat8.
  //! These functions could be functions of BMat8 but they only use
  //! public member functions of BMat8, and so they are declared as free
  //! functions instead.
  namespace bmat8 {
    //! \brief Returns the identity boolean matrix of a given dimension.
    //!
    //! Returns the identity boolean matrix of a given dimension.
    //!
    //! \tparam T the type of the boolean matrix being constructed, should be
    //! BMat8 or HPCombi::BMat8.
    //!
    //! \param dim the dimension of the identity matrix, must be at most 8.
    //!
    //! \returns
    //! A value of type \p T with the first \p dim values on the main diagonal
    //! equal to 1 and every other entry equal to 0.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    //!
    //! \sa \ref one
    // TODO(later) noexcept should depend on whether or not the constructor of
    template <typename T>
    [[nodiscard]] constexpr T one(size_t dim = 8) noexcept {
      LIBSEMIGROUPS_ASSERT(dim <= 8);
      constexpr std::array<uint64_t, 9> const ones = {0x0000000000000000,
                                                      0x8000000000000000,
                                                      0x8040000000000000,
                                                      0x8040200000000000,
                                                      0x8040201000000000,
                                                      0x8040201008000000,
                                                      0x8040201008040000,
                                                      0x8040201008040200,
                                                      0x8040201008040201};
      return T(ones[dim]);
    }

    //! \brief Returns the identity BMat8 of a given dimension.
    //!
    //! This function returns the BMat8 with the first \c dim entries
    //! in the main diagonal equal to \c 1 and every other value equal to \c
    //! 0.
    //!
    //! \param dim the dimension of the identity (default: 8)
    //!
    //! \returns
    //! A BMat8.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    [[nodiscard]] constexpr BMat8 one(size_t dim = 8) noexcept {
      return one<BMat8>(dim);
    }

    //! \brief Construct a random BMat8.
    //!
    //! This function returns a BMat8 chosen at random.
    //!
    //! \returns
    //! A BMat8.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    // Not noexcept since std::uniform_int_distribution::operator() is not
    // noexcept.
    [[nodiscard]] BMat8 random();

    //! \brief Construct a random BMat8 of dimension at most \p dim.
    //!
    //! This function returns a BMat8 chosen at random, where
    //! only the top-left \p dim x \p dim entries can be non-zero.
    //!
    //! \param dim the dimension.
    //!
    //! \returns
    //! A BMat8.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    // Not noexcept since std::uniform_int_distribution::operator() is not
    // noexcept.
    [[nodiscard]] BMat8 random(size_t dim);

    //! \brief Returns the transpose of a BMat8.
    //!
    //! This function returns the transpose of its argument \p x, which is
    //! computed using the technique found in
    //! [Knu09](../biblio.html#knuth2009aa).
    //!
    //! \param x the matrix to transpose.
    //!
    //! \returns
    //! A BMat8.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    [[nodiscard]] constexpr BMat8 transpose(BMat8 const& x) noexcept {
      uint64_t y = x.to_int();
      uint64_t z = (y ^ (y >> 7)) & 0xAA00AA00AA00AA;
      y          = y ^ z ^ (z << 7);
      z          = (y ^ (y >> 14)) & 0xCCCC0000CCCC;
      y          = y ^ z ^ (z << 14);
      z          = (y ^ (y >> 28)) & 0xF0F0F0F0;
      y          = y ^ z ^ (z << 28);
      return BMat8(y);
    }

    //! \brief Find a basis for the row space of a BMat8.
    //!
    //! This function returns a BMat8 whose non-zero rows form a basis
    //! for the row space of \p x.
    //!
    //! \param x the matrix.
    //!
    //! \returns
    //! A BMat8.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    [[nodiscard]] BMat8 row_space_basis(BMat8 const& x) noexcept;

    //! \brief Find a basis for the column space of a BMat8.
    //!
    //! This function returns a BMat8 whose non-zero columns form a
    //! basis for the column space of \p x.
    //!
    //! \param x the matrix.
    //!
    //! \returns
    //! A BMat8.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    [[nodiscard]] inline BMat8 col_space_basis(BMat8 const& x) noexcept {
      return transpose(row_space_basis(transpose(x)));
    }

    //! \brief Returns the number of non-zero rows in a BMat8.
    //!
    //! BMat8s do not know their "dimension" - in effect they are all of
    //! dimension 8. However, this function can be used to obtain the
    //! number of non-zero rows of a BMat8.
    //!
    //! \param x the matrix.
    //!
    //! \returns
    //! The number of non-zero rows.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    //!
    //! \sa number_of_cols and minimum_dim.
    [[nodiscard]] constexpr size_t number_of_rows(BMat8 const& x) noexcept {
      size_t count = 0;
      for (size_t i = 0; i < 8; ++i) {
        if (x.to_int() << (8 * i) >> 56 > 0) {
          count++;
        }
      }
      return count;
    }

    // TODO(2) these should be templated to allow using HPCombi::BMat8's
    // here too.
    //! \brief Returns the number of non-zero columns in a BMat8.
    //!
    //! BMat8s do not know their "dimension" - in effect they are all of
    //! dimension 8. However, this function can be used to obtain the
    //! number of non-zero rows of a BMat8.
    //!
    //! \param x the matrix.
    //!
    //! \returns
    //! The number of non-zero columns.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    //!
    //! \sa \ref number_of_rows and \ref minimum_dim.
    // noexcept because transpose and number_of_rows are.
    [[nodiscard]] constexpr size_t number_of_cols(BMat8 const& x) noexcept {
      return number_of_rows(transpose(x));
    }

    //! \brief Returns the size of the row space of a BMat8.
    //!
    //! \param x the matrix.
    //!
    //! \returns
    //! The size of the row space of \p x.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! \f$O(n)\f$ where \f$n\f$ is the return value of this function.
    //!
    //! \sa col_space_size.
    [[nodiscard]] size_t row_space_size(BMat8 const& x);

    //! \brief Returns the size of the column space of a BMat8.
    //!
    //! \param x the matrix.
    //!
    //! \returns
    //! The size of the column space of \p x.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! \f$O(n)\f$ where \f$n\f$ is the return value of this function.
    //!
    //! \sa row_space_size.
    // Not noexcept because row_space_size isn't.
    [[nodiscard]] inline size_t col_space_size(BMat8 const& x) {
      return row_space_size(transpose(x));
    }

    //! \brief Returns the minimum dimension of a BMat8.
    //!
    //! This function returns the maximal \c n such that row \c n or
    //! column \c n contains a \c 1. Equivalent to the maximum of \ref
    //! number_of_rows and \ref number_of_cols.
    //!
    //! \param x the matrix.
    //!
    //! \returns The minimum dimension of \p x.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    [[nodiscard]] size_t minimum_dim(BMat8 const& x) noexcept;

    //! \brief Returns a vector of the rows of a BMat8.
    //!
    //! This function returns a \c std::vector of \c uint8_t representing
    //! the rows of \p x. The returned vector always has length 8, even
    //! if \c x was constructed with fewer rows.
    //!
    //! \param x the matrix.
    //!
    //! \returns
    //! A `std::vector<uint8_t>`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Constant.
    [[nodiscard]] std::vector<uint8_t> rows(BMat8 const& x);

    //! \brief Push the rows of a BMat8 into the back of a container.
    //!
    //! This function adds the \c uint8_t values representing the rows of \p x
    //! to the container \p rows using its \c push_back member function.
    //!
    //! \tparam Container the type of the container. Must support \c push_back
    //! and have Container::value_type equal to \c uint8_t.
    //!
    //! \param rows the container.
    //! \param x the matrix.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Constant.
    template <typename Container>
    void push_back_rows(Container& rows, BMat8 const& x) {
      static_assert(std::is_same_v<typename Container::value_type, uint8_t>);
      for (size_t i = 0; i < 8; ++i) {
        rows.push_back(x(i));
      }
    }

    //! \brief Checks whether a BMat8 is regular in the monoid of all
    //! BMat8 objects.
    //!
    //! Check whether \p x is a regular element of the full boolean matrix
    //! monoid of appropriate dimension.
    //!
    //! \param x the matrix.
    //!
    //! \returns
    //! A \c true if there exists a boolean matrix \c y such that `x * y * x =
    //! x` where \c x, and \c false otherwise.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    [[nodiscard]] bool is_regular_element(BMat8 const& x) noexcept;

    //! \brief Convert a \c uint8_t to a vector.
    //!
    //! This function converts its argument \p row to a \c std::vector<bool>
    //! such that the item with index \c i is \c true if the bit with index \c i
    //! in \p row is \c 1, and \c false if the corresponding bit is \c 0.
    //!
    //! \param row the matrix.
    //!
    //! \returns
    //! The row represented as a boolean vector.
    //!
    //! \complexity
    //! Constant.
    std::vector<bool> to_vector(uint8_t row);

  }  // namespace bmat8

  //! \ingroup bmat8_group
  //! \brief Insertion operator
  //!
  //! This function allows BMat8 objects to be inserted into an
  //! std::ostringstream.
  std::ostringstream& operator<<(std::ostringstream& os, BMat8 const& x);

  //! \ingroup bmat8_group
  //! \brief Insertion operator
  //!
  //! This function allows BMat8 objects to be inserted into a
  //! std::ostream.
  std::ostream& operator<<(std::ostream& os, BMat8 const& x);

  //! \ingroup bmat8_group
  //! \brief Returns a string representation.
  //!
  //! \param x the matrix
  //! \param braces the type of braces to use in the returned string (default:
  //! `"{}"`).
  //!
  //! \returns A string containing a representation of \p x.
  // TODO (now) rename this to_human_readable_repr
  std::string to_string(BMat8 const& x, std::string const& braces = "{}");

  [[nodiscard]] constexpr BMat8 operator*(bool         scalar,
                                          BMat8 const& x) noexcept {
    if (scalar) {
      return x;
    } else {
      return BMat8(0);
    }
  }
}  // namespace libsemigroups

namespace std {
  template <>
  struct hash<libsemigroups::BMat8> {
    size_t operator()(libsemigroups::BMat8 const& bm) const {
      return hash<uint64_t>()(bm.to_int());
    }
  };
}  // namespace std

namespace libsemigroups {
  //! \defgroup adapters_bmat8_group Adapters for BMat8
  //!
  //! This page contains links to the specific specialisations for some of the
  //! adapters on \ref adapters_group "this page" for \ref BMat8.
  //!
  //! Other adapters specifically for \ref BMat objects and \ref matrix_group
  //! "matrices" over other semirings are available \ref adapters_bmat_group
  //! "here" and \ref adapters_matrix_group "here".

  //! \ingroup adapters_bmat8_group
  //! \brief Specialization of the adapter Complexity for instances of BMat8.
  //!
  //! \copydoc Complexity
  template <>
  struct Complexity<BMat8> {
    //! Returns 0; BMat8 multiplication is constant complexity.
    [[nodiscard]] constexpr inline size_t
    operator()(BMat8 const&) const noexcept {
      return 0;
    }
  };

  //! \ingroup adapters_bmat8_group
  //!
  //! \brief Specialization of the adapter Degree for instances of BMat8.
  //!
  //! \copydoc Degree.
  template <>
  struct Degree<BMat8> {
    //! Returns 8; all BMat8s have degree 8.
    [[nodiscard]] constexpr inline size_t
    operator()(BMat8 const&) const noexcept {
      return 8;
    }
  };

  //! \ingroup adapters_bmat8_group
  //!
  //! \brief Specialization of the adapter IncreaseDegree for instances of
  //! BMat8.
  //!
  //! \copydoc IncreaseDegree.
  template <>
  struct IncreaseDegree<BMat8> {
    //! Does nothing.
    inline void operator()(BMat8 const&, size_t) const noexcept {}
  };

  //! \ingroup adapters_bmat8_group
  //!
  //! \brief Specialization of the adapter One for instances of BMat8.
  //!
  //! \copydoc One.
  template <>
  struct One<BMat8> {
    //! Returns \p x.one()
    [[nodiscard]] inline BMat8 operator()(BMat8 const&) const noexcept {
      return bmat8::one();
    }
    //! Returns bmat8::one(dim)
    [[nodiscard]] inline BMat8 operator()(size_t dim = 8) const noexcept {
      return bmat8::one(dim);
    }
  };

  //! \ingroup adapters_bmat8_group
  //!
  //! \brief Specialization of the adapter Product for instances of BMat8.
  //!
  //! \copydoc Product.
  template <>
  struct Product<BMat8> {
    //! Changes \p xy in place to hold the product of \p x and \p y
    inline void operator()(BMat8&       xy,
                           BMat8 const& x,
                           BMat8 const& y,
                           size_t = 0) const noexcept {
      xy = x * y;
    }
  };

  //! \ingroup adapters_bmat8_group
  //!
  //! \brief Specialization of the adapter ImageRightAction for instances of
  //! BMat8.
  //!
  //! \copydoc ImageRightAction.
  template <>
  struct ImageRightAction<BMat8, BMat8> {
    //! Changes \p res in place to hold the image of \p pt under the right
    //! action of \p x.
    void operator()(BMat8&       res,
                    BMat8 const& pt,
                    BMat8 const& x) const noexcept {
      res = bmat8::row_space_basis(pt * x);
    }
  };

  //! \ingroup adapters_bmat8_group
  //!
  //! \brief Specialization of the adapter ImageLeftAction for instances of
  //! BMat8.
  //!
  //! \copydoc ImageLeftAction.
  template <>
  struct ImageLeftAction<BMat8, BMat8> {
    //! Changes \p res in place to hold the image of \p pt under the left
    //! action of \p x.
    void operator()(BMat8&       res,
                    BMat8 const& pt,
                    BMat8 const& x) const noexcept {
      res = bmat8::col_space_basis(x * pt);
    }
  };

  //! \ingroup adapters_bmat8_group
  //!
  //! \brief Specialization of the adapter Inverse for instances of BMat8.
  //!
  //! \copydoc Inverse.
  template <>
  struct Inverse<BMat8> {
    //! Returns the group inverse of \p x.
    [[nodiscard]] inline BMat8 operator()(BMat8 const& x) const noexcept {
      LIBSEMIGROUPS_ASSERT(x * bmat8::transpose(x) == bmat8::one());
      return bmat8::transpose(x);
    }
  };

  //! \ingroup adapters_bmat8_group
  //!
  //! \brief Specialization of the adapter LambdaValue for instances of BMat8.
  //!
  //! \copydoc LambdaValue
  template <>
  struct LambdaValue<BMat8> {
    //! The type of Lambda values for BMat8 is also BMat8; this provides an
    //! efficient representation of row space bases.
    using type = BMat8;
  };

  //! \ingroup adapters_bmat8_group
  //!
  //! \brief Specialization of the adapter RhoValue for instances of BMat8.
  //!
  //! \copydoc RhoValue
  template <>
  struct RhoValue<BMat8> {
    //! The type of Rho values for BMat8 is also BMat8; this provides an
    //! efficient representation of column space bases.
    using type = BMat8;
  };

  //! \ingroup adapters_bmat8_group
  //!
  //! \brief Specialization of the adapter Lambda for instances of BMat8.
  //!
  //! \copydoc Lambda
  template <>
  struct Lambda<BMat8, BMat8> {
    //! Returns the lambda value of \p x as used in the Konieczny algorithm;
    //! for BMat8 this is the row space basis.
    // noexcept because bmat8::row_space_basis is noexcept
    inline void operator()(BMat8& res, BMat8 const& x) const noexcept {
      res = bmat8::row_space_basis(x);
    }
  };

  //! \ingroup adapters_bmat8_group
  //!
  //! \brief Specialization of the adapter Rho for instances of BMat8.
  //! \copydoc Rho
  template <>
  struct Rho<BMat8, BMat8> {
    //! Returns the rho value of \p x as used in the Konieczny algorithm; for
    //! BMat8 this is the column space basis.
    // noexcept because bmat8::col_space_basis is noexcept
    inline void operator()(BMat8& res, BMat8 const& x) const noexcept {
      res = bmat8::col_space_basis(x);
    }
  };

  //! \ingroup adapters_bmat8_group
  //!
  //! \brief Specialization of the adapter Rank for instances of BMat8.
  //!
  //! \copydoc Rank.
  template <>
  struct Rank<BMat8> {
    //! Returns the rank of \p x as used in the Konieczny algorithm; for BMat8
    //! this is the size of the row space.
    [[nodiscard]] inline size_t operator()(BMat8 const& x) const noexcept {
      return bmat8::row_space_size(x);
    }
  };
}  // namespace libsemigroups

#endif  // LIBSEMIGROUPS_BMAT8_HPP_
