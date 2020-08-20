/******************************************************************************/
/*       Copyright (C) 2018 Finn Smith <fls3@st-andrews.ac.uk>                */
/*       Copyright (C) 2018 James Mitchell <jdm3@st-andrews.ac.uk>            */
/*       Copyright (C) 2018 Florent Hivert <Florent.Hivert@lri.fr>,           */
/*                                                                            */
/*  Distributed under the terms of the GNU General Public License (GPL)       */
/*                                                                            */
/*    This code is distributed in the hope that it will be useful,            */
/*    but WITHOUT ANY WARRANTY; without even the implied warranty of          */
/*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       */
/*   General Public License for more details.                                 */
/*                                                                            */
/*  The full text of the GPL is available at:                                 */
/*                                                                            */
/*                  http://www.gnu.org/licenses/                              */
/******************************************************************************/

// This file contains a declaration of fast boolean matrices up to dimension 8.

#ifndef HPCOMBI_BMAT8_HPP_INCLUDED
#define HPCOMBI_BMAT8_HPP_INCLUDED

#include <algorithm>  // for uniform_int_distribution, swap
#include <array>      // for array
#include <bitset>     // for bitset
#include <climits>    // for CHAR_BIT
#include <cstddef>    // for size_t
#include <cstdint>    // for uint64_t
#include <iostream>   // for operator<<, ostringstream
#include <random>     // for mt19937, random_device
#include <utility>    // for hash
#include <vector>     // for vector

#include "epu.hpp"

#include "perm16.hpp"

#ifndef HPCOMBI_ASSERT
#define HPCOMBI_ASSERT(x) assert(x)
#endif

namespace HPCombi {

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
    explicit BMat8(std::vector<std::vector<bool>> const &mat);

    //! A constructor.
    //!
    //! This is the copy constructor.
    BMat8(BMat8 const &) = default;

    //! A constructor.
    //!
    //! This is the move constructor.
    BMat8(BMat8 &&) = default;

    //! A constructor.
    //!
    //! This is the copy assignement constructor.
    BMat8 &operator=(BMat8 const &) = default;

    //! A constructor.
    //!
    //! This is the move assignment  constructor.
    BMat8 &operator=(BMat8 &&) = default;

    //! A default destructor.
    ~BMat8() = default;

    //! Returns \c true if \c this equals \p that.
    //!
    //! This method checks the mathematical equality of two BMat8 objects.
    bool operator==(BMat8 const &that) const { return _data == that._data; }

    //! Returns \c true if \c this does not equal \p that
    //!
    //! This method checks the mathematical inequality of two BMat8 objects.
    bool operator!=(BMat8 const &that) const { return _data != that._data; }

    //! Returns \c true if \c this is less than \p that.
    //!
    //! This method checks whether a BMat8 objects is less than another.
    //! We order by the results of to_int() for each matrix.
    bool operator<(BMat8 const &that) const { return _data < that._data; }

    //! Returns \c true if \c this is greater than \p that.
    //!
    //! This method checks whether a BMat8 objects is greater than another.
    //! We order by the results of to_int() for each matrix.
    bool operator>(BMat8 const &that) const { return _data > that._data; }

    //! Returns the entry in the (\p i, \p j)th position.
    //!
    //! This method returns the entry in the (\p i, \p j)th position.
    //! Note that since all matrices are internally represented as 8 x 8, it
    //! is possible to access entries that you might not believe exist.
    bool operator()(size_t i, size_t j) const;

    //! Sets the (\p i, \p j)th position to \p val.
    //!
    //! This method sets the (\p i, \p j)th entry of \c this to \p val.
    //! Uses the bit twiddle for setting bits found
    //! <a href=http://graphics.stanford.edu/~seander/bithacks>here</a>.
    void set(size_t i, size_t j, bool val);

    //! Returns the integer representation of \c this.
    //!
    //! Returns an unsigned integer obtained by interpreting an 8 x 8
    //! BMat8 as a sequence of 64 bits (reading rows left to right,
    //! from top to bottom) and then this sequence as an unsigned int.
    uint64_t to_int() const { return _data; }

    //! Returns the transpose of \c this
    //!
    //! Returns the standard matrix transpose of a BMat8.
    //! Uses the technique found in Knuth AoCP Vol. 4 Fasc. 1a, p. 15.
    BMat8 transpose() const;

    //! Returns the transpose of \c this
    //!
    //! Returns the standard matrix transpose of a BMat8.
    //! Uses \c movemask instruction.
    BMat8 transpose_mask() const;

    //! Returns the transpose of \c this
    //!
    //! Returns the standard matrix transpose of a BMat8.
    //! Uses \c movemask instruction.
    BMat8 transpose_maskd() const;

    //! Transpose two matrices at once.
    //!
    //! Compute in parallel the standard matrix transpose of two BMat8.
    //! Uses the technique found in Knuth AoCP Vol. 4 Fasc. 1a, p. 15.
    static void transpose2(BMat8 &, BMat8 &);

    //! Returns the matrix product of \c this and the transpose of \p that
    //!
    //! This method returns the standard matrix product (over the
    //! boolean semiring) of two BMat8 objects. This is faster than tranposing
    //! that and calling the product of \c this with it. Implementation uses
    //! vector instructions.
    BMat8 mult_transpose(BMat8 const &that) const;
    //! Returns the matrix product of \c this and \p that
    //!
    //! This method returns the standard matrix product (over the
    //! boolean semiring) of two BMat8 objects. This is a fast implementation
    //! using transposition and vector instructions.
    BMat8 operator*(BMat8 const &that) const {
        return mult_transpose(that.transpose());
    }

    //! Returns a canonical basis of the row space of \c this
    //!
    //! Any two matrix with the same row space are garanteed to have the same
    //! row space basis. This is a fast implementation using vector
    //! instructions to compute in parallel the union of the other rows
    //! included in a given one.
    BMat8 row_space_basis() const;
    //! Returns a canonical basis of the col space of \c this
    //!
    //! Any two matrix with the same column row space are garanteed to have
    //! the same column space basis. Uses #row_space_basis and #transpose.
    BMat8 col_space_basis() const {
        return transpose().row_space_basis().transpose();
    }
    //! Returns the number of non-zero rows of \c this
    size_t nr_rows() const;
    //! Returns a \c std::vector for rows of \c this
    std::vector<uint8_t> rows() const;

    //! Returns the cardinality of the row space of \c this
    //!
    //! Reference implementation computing all products
    uint64_t row_space_size_ref() const;

    //! Returns the the row space of \c this
    //!
    //! The result is stored in a c++ bitset
    std::bitset<256> row_space_bitset_ref() const;
    //! Returns the the row space of \c this as 256 bits.
    //!
    //! The result is stored in two 128 bits registers.
    void row_space_bitset(epu8 &res1, epu8 &res2) const;
    //! Returns the cardinality of the row space of \c this
    //!
    //! It compute all the product using two 128 bits registers to store
    //! the set of elements of the row space.
    uint64_t row_space_size_bitset() const;
    //! Returns the cardinality of the row space of \c this
    //!
    //! Uses vector computation of the product of included rows in each 256
    //! possible vectors. Fastest implementation saving a few instructions
    //! compared to #row_space_size_incl1
    uint64_t row_space_size_incl() const;
    //! Returns the cardinality of the row space of \c this
    //!
    //! Uses vector computation of the product included row in each 256
    //! possible vectors. More optimized in #row_space_size_incl
    uint64_t row_space_size_incl1() const;
    //! Returns the cardinality of the row space of \c this
    //!
    //! Alias to #row_space_size_incl
    uint64_t row_space_size() const { return row_space_size_incl(); }

    //! Returns whether the row space of \c this is included in other's
    //!
    //! Uses a 256 bitset internally
    bool row_space_included_ref(BMat8 other) const;
    //! Returns whether the row space of \c this is included in other's
    //!
    //! Uses a 256 bitset internally
    bool row_space_included_bitset(BMat8 other) const;

    //! Returns a mask for which vectors of a 16 rows \c epu8 are in
    //! the row space of \c this
    //!
    //! Uses vector computation of the product of included rows
    epu8 row_space_mask(epu8 vects) const;
    //! Returns whether the row space of \c this is included in other's
    //!
    //! Uses vector computation of the product of included rows
    bool row_space_included(BMat8 other) const;

    //! Returns inclusion of row spaces
    //!
    //! Compute at once if a1 is included in b1 and a2 is included in b2
    static std::pair<bool, bool> row_space_included2(BMat8 a1, BMat8 b1,
                                                     BMat8 a2, BMat8 b2);

    //! Returns the matrix whose rows have been permuted according to \c p
    //!
    //! @param p : a permutation fixing the entries 8..15
    //! Note: no verification is performed on p
    BMat8 row_permuted(Perm16 p) const;
    //! Returns the matrix whose columns have been permuted according to \c p
    //!
    //! @param p : a permutation fixing the entries 8..15
    //! Note: no verification is performed on p
    BMat8 col_permuted(Perm16 p) const;

    //! Returns the matrix associated to the permutation \c p by rows
    //!
    //! @param p : a permutation fixing the entries 8..15
    //! Note: no verification is performed on p
    static BMat8 row_permutation_matrix(Perm16 p);
    //! Returns the matrix associated to the permutation \c p by columns
    //!
    //! @param p : a permutation fixing the entries 8..15
    //! Note: no verification is performed on p
    static BMat8 col_permutation_matrix(Perm16 p);

    //! Give the permutation whose right multiplication change \c *this
    //! to \c other
    //!
    //! \c *this is suppose to be a row_space matrix (ie. sorted decreasingly)
    //! Fast implementation doing a vector binary search.
    Perm16 right_perm_action_on_basis(BMat8) const;
    //! Give the permutation whose right multiplication change \c *this
    //! to \c other
    //!
    //! \c *this is suppose to be a row_space matrix (ie. sorted decreasingly)
    //! Reference implementation.
    Perm16 right_perm_action_on_basis_ref(BMat8) const;

    //! Returns the identity BMat8
    //!
    //! This method returns the 8 x 8 BMat8 with 1s on the main diagonal.
    static BMat8 one(size_t dim = 8) {
        HPCOMBI_ASSERT(dim <= 8);
        static std::array<uint64_t, 9> const ones = {0x0000000000000000,
                                                     0x8000000000000000,
                                                     0x8040000000000000,
                                                     0x8040200000000000,
                                                     0x8040201000000000,
                                                     0x8040201008000000,
                                                     0x8040201008040000,
                                                     0x8040201008040200,
                                                     0x8040201008040201};
        return BMat8(ones[dim]);
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

    //! Swap the matrix \c this with \c that
    void swap(BMat8 &that) { std::swap(this->_data, that._data); }

    //! Write \c this on \c os
    std::ostream & write(std::ostream &os) const;

#ifdef LIBSEMIGROUPS_DENSEHASHMAP
    // FIXME do this another way
    BMat8 empty_key() const { return BMat8(0xFF7FBFDFEFF7FBFE); }
#endif

  private:
    uint64_t _data;

    epu8 row_space_basis_internal() const;
};

}  // namespace HPCombi

#include "bmat8_impl.hpp"

namespace std {
template <> struct hash<HPCombi::BMat8> {
    inline size_t operator()(HPCombi::BMat8 const &bm) const {
        return hash<uint64_t>()(bm.to_int());
    }
};
}  // namespace std
#endif  // HPCOMBI_BMAT8_HPP_INCLUDED
