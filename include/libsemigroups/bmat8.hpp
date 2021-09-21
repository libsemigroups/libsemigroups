//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019 Finn Smith
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

#ifndef LIBSEMIGROUPS_BMAT8_HPP_
#define LIBSEMIGROUPS_BMAT8_HPP_

#include <algorithm>  // for uniform_int_distribution, swap
#include <cstddef>    // for size_t
#include <cstdint>    // for uint64_t
#include <iosfwd>     // for operator<<, ostringstream
#include <random>     // for mt19937, random_device
#include <utility>    // for hash
#include <vector>     // for vector

#include "adapters.hpp"  // for Complexity, Degree, etc . . .
#include "debug.hpp"     // for LIBSEMIGROUPS_ASSERT
#include "string.hpp"    // for detail::to_string

namespace libsemigroups {
  namespace bmat8_helpers {
    //! Returns the identity boolean matrix of a given dimension.
    //!
    //! \tparam T the type of the boolean matrix being constructed, should be
    //! BMat8 or HPCombi::BMat8.
    //!
    //! \param dim the dimension of the identity matrix, must be at most 7.
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
    //! \sa BMat8::one.
    // TODO(later) noexcept should depend on whether or not the constructor of
    // T is noexcept
    template <typename T>
    T one(size_t dim) noexcept {
      LIBSEMIGROUPS_ASSERT(dim <= 8);
      static std::array<uint64_t, 9> const ones = {0x0000000000000000,
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
  }  // namespace bmat8_helpers

  //! Defined in ``bmat8.hpp``.
  //!
  //! Class for fast boolean matrices of dimension up to 8 x 8
  //!
  //! The member functions for these small matrices over the boolean semiring
  //! are more optimised than the generic member functions for boolean matrices.
  //! Note that all BMat8 are represented internally as an 8 x 8 matrix;
  //! any entries not defined by the user are taken to be 0. This does
  //! not affect the results of any calculations.
  //!
  //! BMat8 is a trivial class.
  class BMat8 final {
   public:
    //! Default constructor.
    //!
    //! There is no guarantee about the contents of the matrix constructed.
    //!
    //! \par Parameters
    //! (None)
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    BMat8() noexcept = default;

    //! Construct from uint64_t.
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
    explicit BMat8(uint64_t mat) noexcept : _data(mat) {}

    //! A constructor.
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

    //! Default copy constructor.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    BMat8(BMat8 const&) noexcept = default;

    //! Default move constructor.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    BMat8(BMat8&&) noexcept = default;

    //! Default copy assignment operator.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    BMat8& operator=(BMat8 const&) noexcept = default;

    //! Default move assignment operator.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    BMat8& operator=(BMat8&&) noexcept = default;

    ~BMat8() = default;

    //! Returns \c true if \c this equals \p that.
    //!
    //! This member function checks the mathematical equality of two BMat8
    //! objects.
    //!
    //! \param that the BMat8 for comparison.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    bool operator==(BMat8 const& that) const noexcept {
      return _data == that._data;
    }

    //! Returns \c true if \c this does not equal \p that
    //!
    //! This member function checks the mathematical inequality of two BMat8
    //! objects.
    //!
    //! \param that the BMat8 for comparison.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    bool operator!=(BMat8 const& that) const noexcept {
      return _data != that._data;
    }

    //! Returns \c true if \c this is less than \p that.
    //!
    //! This member function checks whether a BMat8 objects is less than
    //! another. We order by the results of to_int() for each matrix.
    //!
    //! \param that the BMat8 for comparison.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    bool operator<(BMat8 const& that) const noexcept {
      return _data < that._data;
    }

    //! Returns \c true if \c this is greater than \p that.
    //!
    //! This member function checks whether a BMat8 objects is greater than
    //! another. We order by the results of to_int() for each matrix.
    //!
    //! \param that the BMat8 for comparison.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    bool operator>(BMat8 const& that) const noexcept {
      return _data > that._data;
    }

    //! Returns the entry in the (\p i, \p j)th position.
    //!
    //! This member function returns the entry in the (\p i, \p j)th position.
    //!
    //! \param i the row of the entry sought.
    //! \param j the column of the entry sought.
    //!
    //! \returns
    //! A ``bool``.
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
    //! No checks on the parameters \p i and \p j are performed, if \p i or \p
    //! j is greater than 7, then bad things will happen.
    bool get(size_t i, size_t j) const noexcept {
      LIBSEMIGROUPS_ASSERT(i < 8);
      LIBSEMIGROUPS_ASSERT(j < 8);
      return (_data << (8 * i + j)) >> 63;
    }

    // TODO(later) at method

    //! Sets the (\p i, \p j)th position to \p val.
    //!
    //! This member function sets the (\p i, \p j)th entry of \c this to \p val.
    //! Uses the bit twiddle for setting bits found
    //! <a href=http://graphics.stanford.edu/~seander/bithacks>here</a>.
    //!
    //! \param i the row
    //! \param j the column
    //! \param val the value to set in position (\p i, \p j)th
    //!
    //! \returns
    //! (None)
    //!
    //! \throws LibsemigroupsException if \p i or \p j is out of bounds.
    //!
    //! \complexity
    //! Constant.
    void set(size_t i, size_t j, bool val);

    //! Returns the integer representation of \c this.
    //!
    //! Returns an unsigned integer obtained by interpreting an 8 x 8
    //! BMat8 as a sequence of 64 bits (reading rows left to right,
    //! from top to bottom) and then realising this sequence as an unsigned
    //! int.
    //!
    //! \returns
    //! A ``uint64_t``.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    //!
    //! \par Parameters
    //! (None)
    inline uint64_t to_int() const noexcept {
      return _data;
    }

    //! Returns the transpose of \c this.
    //!
    //! Uses the technique found in [Knu09](../biblio.html#knuth2009aa).
    //!
    //! \returns
    //! A BMat8.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    //!
    //! \par Parameters
    //! (None)
    inline BMat8 transpose() const noexcept {
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
    //! This member function returns the standard matrix product (over the
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
    BMat8 operator*(BMat8 const& that) const noexcept;

    //! Insertion operator
    //!
    //! This member function allows BMat8 objects to be inserted into an
    //! std::ostringstream.
    friend std::ostringstream& operator<<(std::ostringstream& os,
                                          BMat8 const&        bm);

    //! Insertion operator
    //!
    //! This member function allows BMat8 objects to be inserted into a
    //! std::ostream.
    friend std::ostream& operator<<(std::ostream& os, BMat8 const& bm) {
      os << detail::to_string(bm);
      return os;
    }

    //! Construct a random BMat8.
    //!
    //! This static member function returns a BMat8 chosen at random.
    //!
    //! \returns
    //! A BMat8.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \par Parameters
    //! (None)
    // Not noexcept since std::uniform_int_distribution::operator() is not
    // noexcept.
    static BMat8 random() {
      return BMat8(_dist(_gen));
    }

    //! Construct a random BMat8 of dimension at most \p dim.
    //!
    //! This static member function returns a BMat8 chosen at random, where
    //! only the top-left \p dim x \p dim entries can be non-zero.
    //!
    //! \returns
    //! A BMat8.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \par Parameters
    //! (None)
    // Not noexcept since std::uniform_int_distribution::operator() is not
    // noexcept.
    static BMat8 random(size_t dim);

    //! Swaps \c this with \p that.
    //!
    //! This member function swaps the values of \c this and \p that.
    //!
    //! \param that the BMat8 to swap \c this with.
    //!
    //! \returns
    //! (None)
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    inline void swap(BMat8& that) noexcept {
      std::swap(this->_data, that._data);
    }

    //! Find a basis for the row space of \c this.
    //!
    //! This member function returns a BMat8 whose non-zero rows form a basis
    //! for the row space of \c this.
    //!
    //! \returns
    //! A BMat8.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    //!
    //! \par Parameters
    //! (None)
    BMat8 row_space_basis() const noexcept;

    //! Find a basis for the column space of \c this.
    //!
    //! This member function returns a BMat8 whose non-zero columns form a basis
    //! for the column space of \c this.
    //!
    //! \returns
    //! A BMat8.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    //!
    //! \par Parameters
    //! (None)
    BMat8 col_space_basis() const noexcept {
      return this->transpose().row_space_basis().transpose();
    }

    //! Returns a vector containing the rows of \c this
    //!
    //! This member function returns a std::vector of uint8_ts representing the
    //! rows of \c this. The vector will always be of length 8, even if \c this
    //! was constructed with fewer rows.
    //!
    //! \returns
    //! A std::vector<uint8_t>.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Constant.
    //!
    //! \par Parameters
    //! (None)
    // TODO(later) make this return an array instead of a vector
    std::vector<uint8_t> rows() const;

    //! Find the size of the row space of \c this.
    //!
    //! \returns
    //! A \c size_t.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! \f$O(n)\f$ where \f$n\f$ is the return value of this function.
    //!
    //! \par Parameters
    //! (None)
    //!
    //! \sa bmat8_helpers::col_space_size.
    size_t row_space_size() const;

    //! Returns the number of non-zero rows in \c this.
    //!
    //! BMat8s do not know their "dimension" - in effect they are all of
    //! dimension 8. However, this member function can be used to obtain the
    //! number of non-zero rows of \c this.
    //!
    //! \returns
    //! A \c size_t.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    //!
    //! \par Parameters
    //! (None)
    //!
    //! \sa bmat8_helpers::number_of_cols and bmat8_helpers::minimum_dim.
    size_t number_of_rows() const noexcept {
      size_t count = 0;
      for (size_t i = 0; i < 8; ++i) {
        if (_data << (8 * i) >> 56 > 0) {
          count++;
        }
      }
      return count;
    }

    //! Check whether \c this is a regular element of the full boolean matrix
    //! monoid of appropriate dimension.
    //!
    //! \returns
    //! A \c true if there exists a boolean matrix \c y such that `x * y * x =
    //! x` where \c x is \c *this.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    //!
    //! \par Parameters
    //! (None)
    bool is_regular_element() const noexcept {
      return *this
                 * BMat8(
                       ~(*this * BMat8(~_data).transpose() * (*this)).to_int())
                       .transpose()
                 * (*this)
             == *this;
    }

    //! Returns the identity BMat8.
    //!
    //! This member function returns the BMat8 with the first \c dim entries in
    //! the main diagonal equal to \c 1 and every other value equal to \c 0.
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
    static BMat8 one(size_t dim = 8) noexcept {
      return bmat8_helpers::one<BMat8>(dim);
    }

   private:
    void sort_rows() noexcept;

    uint64_t                                       _data;
    static std::random_device                      _rd;
    static std::mt19937                            _gen;
    static std::uniform_int_distribution<uint64_t> _dist;
  };

  static_assert(std::is_trivial<BMat8>(), "BMat8 is not a trivial class!");

  //! Defined in ``bmat8.hpp``.
  //!
  //! This namespace contains various helper functions for the class BMat8.
  //! These functions could be member functions of BMat8 but they only use
  //! public member functions of BMat8, and so they are declared as free
  //! functions instead.
  namespace bmat8_helpers {
    // TODO(later) these should be templated to allow using HPCombi::BMat8's
    // here too.
    //! Returns the number of non-zero columns in \p x.
    //!
    //! BMat8s do not know their "dimension" - in effect they are all of
    //! dimension 8. However, this member function can be used to obtain the
    //! number of non-zero rows of \c this.
    //!
    //! \param x the BMat8 whose number of columns we want.
    //!
    //! \returns
    //! A \c size_t.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    //!
    //! \sa BMat8::number_of_rows and bmat8_helpers::minimum_dim.
    // noexcept because transpose and number_of_rows are.
    static inline size_t number_of_cols(BMat8 const& x) noexcept {
      return x.transpose().number_of_rows();
    }

    //! Find the size of the column space of \c x.
    //!
    //! \param x a BMat8.
    //!
    //! \returns
    //! A \c size_t.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! \f$O(n)\f$ where \f$n\f$ is the return value of this function.
    //!
    //! \sa BMat8::row_space_size.
    // Not noexcept because row_space_size isn't.
    static inline size_t col_space_size(BMat8 const& x) {
      return x.transpose().row_space_size();
    }

    //! Find the minimum dimension of \p x.
    //!
    //! This member function returns the maximal \c i such that row \c i or
    //! column \c i contains a \c 1.
    //!
    //! \param x a BMat8.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    size_t minimum_dim(BMat8 const& x) noexcept;

  }  // namespace bmat8_helpers
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
  //! Specialization of the adapter Complexity for instances of BMat8.
  //!
  //! \sa Complexity.
  template <>
  struct Complexity<BMat8> {
    //! Returns 0; BMat8 multiplication is constant complexity.
    constexpr inline size_t operator()(BMat8 const&) const noexcept {
      return 0;
    }
  };

  //! Specialization of the adapter Degree for instances of BMat8.
  //!
  //! \sa Degree.
  template <>
  struct Degree<BMat8> {
    //! Returns 8; all BMat8s have degree 8.
    constexpr inline size_t operator()(BMat8 const&) const noexcept {
      return 8;
    }
  };

  //! Specialization of the adapter IncreaseDegree for instances of BMat8.
  //!
  //! \sa IncreaseDegree.
  template <>
  struct IncreaseDegree<BMat8> {
    //! Does nothing.
    inline void operator()(BMat8 const&, size_t) const noexcept {}
  };

  //! Specialization of the adapter One for instances of BMat8.
  //!
  //! \sa One.
  template <>
  struct One<BMat8> {
    //! Returns \p x.one()
    inline BMat8 operator()(BMat8 const& x) const noexcept {
      return x.one();
    }
    //! Returns BMat8::one(dim)
    inline BMat8 operator()(size_t dim = 8) const noexcept {
      return BMat8::one(dim);
    }
  };

  //! Specialization of the adapter Product for instances of BMat8.
  //!
  //! \sa Product.
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

  //! Specialization of the adapter ImageRightAction for instances of BMat8.
  //!
  //! \sa ImageRightAction.
  template <>
  struct ImageRightAction<BMat8, BMat8> {
    //! Changes \p res in place to hold the image of \p pt under the right
    //! action of \p x.
    void operator()(BMat8&       res,
                    BMat8 const& pt,
                    BMat8 const& x) const noexcept {
      res = (pt * x).row_space_basis();
    }
  };

  //! Specialization of the adapter ImageLeftAction for instances of BMat8.
  //!
  //! \sa ImageLeftAction.
  template <>
  struct ImageLeftAction<BMat8, BMat8> {
    //! Changes \p res in place to hold the image of \p pt under the left
    //! action of \p x.
    void operator()(BMat8&       res,
                    BMat8 const& pt,
                    BMat8 const& x) const noexcept {
      res = (x * pt).col_space_basis();
    }
  };

  //! Specialization of the adapter Inverse for instances of BMat8.
  //!
  //! \sa Inverse.
  template <>
  struct Inverse<BMat8> {
    //! Returns the group inverse of \p x.
    inline BMat8 operator()(BMat8 const& x) const noexcept {
      LIBSEMIGROUPS_ASSERT(x * x.transpose() == x.one());
      return x.transpose();
    }
  };

  //! Specialization of the adapter LambdaValue for instances of BMat8.
  //!
  //! \sa LambdaValue
  template <>
  struct LambdaValue<BMat8> {
    //! The type of Lambda values for BMat8 is also BMat8; this provides an
    //! efficient representation of row space bases.
    using type = BMat8;
  };

  //! Specialization of the adapter RhoValue for instances of BMat8.
  //!
  //! \sa RhoValue
  template <>
  struct RhoValue<BMat8> {
    //! The type of Rho values for BMat8 is also BMat8; this provides an
    //! efficient representation of column space bases.
    using type = BMat8;
  };

  //! Specialization of the adapter Lambda for instances of BMat8.
  //!
  //! \sa Lambda.
  template <>
  struct Lambda<BMat8, BMat8> {
    //! Returns the lambda value of \p x as used in the Konieczny algorithm; for
    //! BMat8 this is the row space basis.
    // noexcept because BMat8::row_space_basis is noexcept
    inline void operator()(BMat8& res, BMat8 const& x) const noexcept {
      res = x.row_space_basis();
    }
  };

  //! Specialization of the adapter Rho for instances of BMat8.
  //!
  //! \sa Rho.
  template <>
  struct Rho<BMat8, BMat8> {
    //! Returns the rho value of \p x as used in the Konieczny algorithm; for
    //! BMat8 this is the column space basis.
    // noexcept because BMat8::col_space_basis is noexcept
    inline void operator()(BMat8& res, BMat8 const& x) const noexcept {
      res = x.col_space_basis();
    }
  };

  //! Specialization of the adapter Rank for instances of BMat8.
  //!
  //! \sa Rank.
  template <>
  struct Rank<BMat8> {
    //! Returns the rank of \p x as used in the Konieczny algorithm; for BMat8
    //! this is the size of the row space.
    inline size_t operator()(BMat8 const& x) const noexcept {
      return x.row_space_size();
    }
  };
}  // namespace libsemigroups

#endif  // LIBSEMIGROUPS_BMAT8_HPP_
