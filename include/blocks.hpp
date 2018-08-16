//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2016 James D. Mitchell
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

// This file contains the declaration of a blocks class, which is needed by the
// bipartitions code.

#ifndef LIBSEMIGROUPS_INCLUDE_BLOCKS_HPP_
#define LIBSEMIGROUPS_INCLUDE_BLOCKS_HPP_

#include <vector>

namespace libsemigroups {

  //! Class for signed partitions of the set \f$\{0, \ldots, n - 1\}\f$
  //!
  //! It is possible to associate to every Bipartition a pair of blocks,
  //! Bipartition::left_blocks and Bipartition::right_blocks, which determine
  //! the Green's \f$\mathscr{L}\f$- and \f$\mathscr{R}\f$-classes of the
  //! Bipartition in the monoid of all bipartitions. This is the purpose of this
  //! class.
  //!
  //! The Blocks class is not currently used by any of the methods for the
  //! FroidurePin class but the extra methods are used in the GAP package
  //! [Semigroups package for GAP](https://gap-packages.github.io/Semigroups/).
  class Blocks {
   public:
    //! A constructor.
    //!
    //! Constructs a blocks object of size 0.
    Blocks();

    //! A constructor.
    //!
    //! The parameter \p blocks must be non-empty, consist of non-negative
    //! integers, and have the property that if some positive \f$i\f$ occurs in
    //! \p blocks, then \f$i - 1\f$ occurs earlier in \p blocks. None of this is
    //! checked.  The parameter \p blocks is not copied, and is deleted by the
    //! destructor Blocks::~Blocks.
    //!
    //! The parameter \p lookup must have length equal to the number of
    //! different values in \p blocks (or one more than the maximum value in the
    //! list); this is equal to the number of blocks in the partition. A value
    //! \c true in position \f$i\f$ indicates that the \f$i\f$th block is signed
    //! (transverse) and \c false that it is unsigned.
    Blocks(std::vector<u_int32_t>*, std::vector<bool>*);

    //! A constructor.
    //!
    //! The parameter \p blocks must have length \f$n\f$ for some integer
    //! \f$n > 0\f$, consist of non-negative integers, and have the property
    //! that if \f$i\f$, \f$i > 0\f$ occurs in \p blocks, then \f$i - 1\f$
    //! occurs earlier in \p blocks. None of this is checked.  The parameter \p
    //! blocks is not copied, and is deleted by the destructor Blocks::~Blocks.
    //!
    //! The parameter \p lookup must have length equal to the number of
    //! different values in \p blocks (or one more than the maximum value in the
    //! list); this is equal to the number of blocks in the partition. A value
    //! \c true in position \f$i\f$ indicates that the \f$i\f$th block is signed
    //! (transverse) and \c false that it is unsigned.
    //!
    //! The parameter \p nr_blocks must be the number of blocks (i.e. one more
    //! than the maximum value in \p blocks).
    //!
    //! This constructor is provided for the situation where the number of
    //! blocks in blocks is known *a priori* and so does not need to be
    //! calculated in the constructor.
    Blocks(std::vector<u_int32_t>*, std::vector<bool>*, u_int32_t);

    //! The assignment operator is deleted for Blocks to avoid unintended
    //! copying.
    Blocks& operator=(Blocks const& copy) = delete;

    //! Copy constructor
    //!
    //! Copies all the information in copy and returns a new instance of
    //! Blocks.
    Blocks(Blocks const& copy);

    //! Default destructor.
    //!
    //! Deletes the blocks and lookup provided at construction time.
    ~Blocks();

    //! Returns \c true if \c this equals \p that.
    //!
    //! Two Blocks objects are equal if and only if their underlying signed
    //! partitions are equal. It is ok to compare blocks of different
    //! degree with this operator.
    bool operator==(const Blocks& that) const;

    //! Returns \c true if \c this is less than \p that.
    //!
    //! This operator defines a total order on the set of all Blocks objects
    //! (including those of different degree).
    bool operator<(const Blocks& that) const;

    //! Returns the degree of a Blocks object
    //!
    //! The *degree* of a Blocks object is the size of the set of
    //! which it is a partition.
    u_int32_t degree() const;

    //! Returns the index of the block containing pos.
    //!
    //! This method asserts that \p pos is valid, i.e. that it is less than the
    //! degree of \c this.
    u_int32_t block(size_t pos) const;

    //! Returns \c true if the block with index \p index is transverse
    //!
    //! This method returns \c true if the block with index \p index is a
    //! transverse (or signed) block and it returns \c false if it is not
    //! transverse (or unsigned).  This method asserts that the parameter \p
    //! index is valid, i.e. that it is less than the degree of \c this.
    bool is_transverse_block(size_t index) const;

    //! Returns a pointer to the lookup table for block indices.
    //!
    //! The vector pointed to by the return value of this method has value
    //! \c true in position \c i if the \c i th block of \c this is a
    //! transverse block; the entry in position \c i is \c false otherwise.
    // FIXME better to have lookup_begin/end methods
    std::vector<bool> const* lookup() const;

    //! Returns the number of blocks in the Blocks object.
    //!
    //! This method returns the number of parts in the partition that instances
    //! of this class represent.
    u_int32_t nr_blocks() const;

    //! Returns the number of signed (transverse) blocks in \c this.
    //!
    //! Equivalently, this method returns the number of \c true values in
    //! Blocks::lookup().
    u_int32_t rank();

    //! Returns a hash value for a \c this.
    //!
    //! This method returns a hash value for an instance of Blocks.  This value
    //! is recomputed every time this method is called.
    size_t hash_value() const;

    //! Returns a const_iterator pointing to the index of the first block
    //!
    //! This method asserts that degree is not 0.
    typename std::vector<u_int32_t>::const_iterator cbegin() const;

    //! Returns a const_iterator referring to past-the-end of the last block.
    //!
    //! This method asserts that degree is not 0.
    typename std::vector<u_int32_t>::const_iterator cend() const;

   private:
    std::vector<u_int32_t>* _blocks;
    std::vector<bool>*      _lookup;
    u_int32_t               _nr_blocks;
    u_int32_t               _rank;
  };
}  // namespace libsemigroups

#endif  // LIBSEMIGROUPS_INCLUDE_BLOCKS_HPP_
