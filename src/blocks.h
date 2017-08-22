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

#ifndef LIBSEMIGROUPS_SRC_BLOCKS_H_
#define LIBSEMIGROUPS_SRC_BLOCKS_H_

#include <algorithm>
#include <functional>
#include <vector>

#include "libsemigroups-debug.h"

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
  //! Semigroup class but the extra methods are used in the GAP package
  //! [Semigroups package for GAP](https://gap-packages.github.io/Semigroups/).

  class Blocks {
   public:
    //! A constructor.
    //!
    //! Constructs a blocks object of size 0.
    Blocks() : _blocks(nullptr), _lookup(nullptr), _nr_blocks(0), _rank(0) {}

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
    Blocks(std::vector<u_int32_t>* blocks, std::vector<bool>* lookup)
        : _blocks(blocks), _lookup(lookup), _nr_blocks(), _rank(UNDEFINED) {
      LIBSEMIGROUPS_ASSERT(_blocks->size() != 0);
      _nr_blocks = *(std::max_element(_blocks->begin(), _blocks->end())) + 1;
      LIBSEMIGROUPS_ASSERT(_nr_blocks == _lookup->size());
    }

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
    Blocks(std::vector<u_int32_t>* blocks,
           std::vector<bool>*      lookup,
           u_int32_t               nr_blocks)
        : _blocks(blocks),
          _lookup(lookup),
          _nr_blocks(nr_blocks),
          _rank(UNDEFINED) {
      LIBSEMIGROUPS_ASSERT(_blocks->size() != 0);
      LIBSEMIGROUPS_ASSERT(_nr_blocks == _lookup->size());
    }

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
    ~Blocks() {
      delete _blocks;
      delete _lookup;
    }

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
    inline u_int32_t degree() const {
      return (_nr_blocks == 0 ? 0 : _blocks->size());
    }

    //! Returns the index of the block containing pos.
    //!
    //! This method asserts that \p pos is valid, i.e. that it is less than the
    //! degree of \c this.
    inline u_int32_t block(size_t pos) const {
      LIBSEMIGROUPS_ASSERT(pos < _blocks->size());
      return (*_blocks)[pos];
    }

    //! Returns \c true if the block with index \p index is transverse
    //!
    //! This method returns \c true if the block with index \p index is a
    //! transverse (or signed) block and it returns \c false if it is not
    //! transverse (or unsigned).  This method asserts that the parameter \p
    //! index is valid, i.e. that it is less than the degree of \c this.
    inline bool is_transverse_block(size_t index) const {
      LIBSEMIGROUPS_ASSERT(index < _nr_blocks);
      return (*_lookup)[index];
    }

    //! Returns a pointer to the lookup table for block indices.
    //!
    //! The vector pointed to by the return value of this method has value
    //! \c true in position \c i if the \c i th block of \c this is a
    //! transverse block; the entry in position \c i is \c false otherwise.
    // FIXME better to have lookup_begin/end methods
    inline std::vector<bool> const* lookup() const {
      return _lookup;
    }

    //! Returns the number of blocks in the Blocks object.
    //!
    //! This method returns the number of parts in the partition that instances
    //! of this class represent.
    inline u_int32_t nr_blocks() const {
      return _nr_blocks;
    }

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
    inline typename std::vector<u_int32_t>::const_iterator cbegin() const {
      LIBSEMIGROUPS_ASSERT(_blocks != nullptr);
      return _blocks->cbegin();
    }

    //! Returns a const_iterator referring to past-the-end of the last block.
    //!
    //! This method asserts that degree is not 0.
    inline typename std::vector<u_int32_t>::const_iterator cend() const {
      LIBSEMIGROUPS_ASSERT(_blocks != nullptr);
      return _blocks->cend();
    }

   private:
    std::vector<u_int32_t>* _blocks;
    std::vector<bool>*      _lookup;
    u_int32_t               _nr_blocks;
    u_int32_t               _rank;
    static u_int32_t const  UNDEFINED = std::numeric_limits<u_int32_t>::max();
  };
}  // namespace libsemigroups

#endif  // LIBSEMIGROUPS_SRC_BLOCKS_H_
