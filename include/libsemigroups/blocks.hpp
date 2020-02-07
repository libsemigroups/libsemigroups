//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019 James D. Mitchell
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

#ifndef LIBSEMIGROUPS_BLOCKS_HPP_
#define LIBSEMIGROUPS_BLOCKS_HPP_

#include <cstddef>  // for size_t
#include <cstdint>  // for uint32_t
#include <vector>   // for vector, vector<>::const_iterator

#include "libsemigroups-debug.hpp"  // for LIBSEMIGROUPS_ASSERT

// TODO(later)
// 1. Use noexcept where appropriate
// 2. Add argument checks and exceptions in case they are wrong.
// 3. Remove pointers from the interface.

namespace libsemigroups {

  //! Blocks is a class representing signed partitions of the set
  //! \f$\{0, \ldots, n - 1\}\f$.
  //!
  //! It is possible to associate to every Bipartition a pair of blocks,
  //! Bipartition::left_blocks and Bipartition::right_blocks, which determine
  //! the Green's \f$\mathscr{L}\f$- and \f$\mathscr{R}\f$-classes of the
  //! Bipartition in the monoid of all bipartitions. This is the purpose of this
  //! class.
  //!
  //! The Blocks class is not currently used by any of the functions for the
  //! FroidurePin class but the extra functions are used in the GAP package
  //! [Semigroups package for GAP](https://gap-packages.github.io/Semigroups/).
  class Blocks {
   public:
    //! Constructs a blocks object of size 0.
    //!
    //! \par Exceptions
    //! This function is `noexcept` and is guaranteed never to throw.
    //!
    //! \par Complexity
    //! Constant.
    //!
    //! \par Parameters
    //! (None)
    Blocks() noexcept
        : _blocks(nullptr), _lookup(nullptr), _nr_blocks(0), _rank(0) {}

    //! This constructor is provided for the situation where the number of
    //! blocks in blocks is known *a priori* and so does not need to be
    //! calculated in the constructor.
    //!
    //! \param blocks must have length \f$n\f$ for some integer
    //! \f$n > 0\f$, consist of non-negative integers, and have the property
    //! that if \f$i\f$, \f$i > 0\f$ occurs in \p blocks, then \f$i - 1\f$
    //! occurs earlier in \p blocks. None of this is checked.  The parameter \p
    //! blocks is not copied, and is deleted by the destructor Blocks::~Blocks.
    //!
    //! \param lookup must have length equal to the number of
    //! different values in \p blocks (or one more than the maximum value in the
    //! list); this is equal to the number of blocks in the partition. A value
    //! \c true in position \f$i\f$ indicates that the \f$i\f$th block is signed
    //! (transverse) and \c false that it is unsigned.
    //!
    //! \param nr_blocks must be the number of blocks (i.e. one more
    //! than the maximum value in \p blocks).
    //!
    //! \returns A Blocks object.
    //!
    //! \par Exceptions
    //! This function `noexcept` and is guaranteed never to throw, the caller
    //! is responsible for the validity of the arguments.
    //!
    //! \par Complexity
    //! Constant.
    Blocks(std::vector<uint32_t>* blocks,
           std::vector<bool>*     lookup,
           uint32_t               nr_blocks) noexcept;

    //! Construct a blocks object.
    //!
    //! \param blocks must be non-empty, consist of non-negative
    //! integers, and have the property that if some positive \f$i\f$ occurs in
    //! \p blocks, then \f$i - 1\f$ occurs earlier in \p blocks. None of this is
    //! checked.  The parameter \p blocks is not copied, and is deleted by the
    //! destructor Blocks::~Blocks.
    //!
    //! \param lookup must have length equal to the number of
    //! different values in \p blocks (or one more than the maximum value in the
    //! list); this is equal to the number of blocks in the partition. A value
    //! \c true in position \f$i\f$ indicates that the \f$i\f$th block is signed
    //! (transverse) and \c false that it is unsigned.
    //!
    //! \par Exceptions
    //! This function throws if `std::max_element` throws, the caller is
    //! responsible for the validity of the arguments.
    //!
    //! \par Complexity
    //! Linear in `blocks->size()`.
    Blocks(std::vector<uint32_t>* blocks, std::vector<bool>* lookup);

    //! The assignment operator is deleted for Blocks to avoid unintended
    //! copying.
    Blocks& operator=(Blocks const&) = delete;

    //! Copy construct a Blocks object.
    Blocks(Blocks const& copy);

    //! Deletes the blocks and lookup provided at construction time.
    ~Blocks() {
      delete _blocks;
      delete _lookup;
    }

    //! Two Blocks objects are equal if and only if their underlying signed
    //! partitions are equal. It is ok to compare blocks of different
    //! degree with this operator.
    //!
    //! \param that a Blocks instance
    //!
    //! \returns \c true if \c this equals \p that.
    //!
    //! \par Exceptions
    //! This function only throws if std::vector::operator== does.
    //!
    //! \par Complexity
    //! Linear in `degree()`.
    bool operator==(Blocks const& that) const;

    //! This operator defines a total order on the set of all Blocks objects
    //! (including those of different degree).
    //!
    //! \param that a Blocks instance
    //!
    //! \returns \c true if \c this is less than \p that.
    //!
    //! \par Exceptions
    //! This function only throws if std::vector::operator[] does.
    //!
    //! \par Complexity
    //! Linear in `degree()`.
    bool operator<(Blocks const& that) const;

    //! The *degree* of a Blocks object is the size of the set of
    //! which it is a partition, or the size of the \p blocks parameter to
    //! Blocks::Blocks.
    //!
    //! \returns The degree of a Blocks object.
    //!
    //! \par Exceptions
    //! This function is `noexcept` and is guaranteed never to throw.
    //!
    //! \par Parameters
    //! (None)
    uint32_t degree() const noexcept {
      return (_nr_blocks == 0 ? 0 : _blocks->size());
    }

    //! \param pos the integer whose block index is sought.
    //!
    //! \returns The index of the block containing \p pos.
    //!
    //! \par Exceptions
    //! This function is `noexcept` and is guaranteed never to throw, the
    //! caller is responsible for the validity of the arguments.
    //!
    //! \par Complexity
    //! Constant.
    uint32_t block(size_t pos) const noexcept {
      LIBSEMIGROUPS_ASSERT(pos < _blocks->size());
      return (*_blocks)[pos];
    }

    //! This function returns \c true if the block with index \p index is a
    //! transverse (or signed) block and it returns \c false if it is not
    //! transverse (or unsigned).
    //!
    //! \param index the index of a block
    //!
    //! \returns \c true if the block with index \p index is transverse, and \c
    //! false if not.
    //!
    //! \par Exceptions
    //! This function is `noexcept` and is guaranteed never to throw, the
    //! caller is responsible for the validity of the arguments.
    //!
    //! \par Complexity
    //! Constant.
    bool is_transverse_block(size_t index) const noexcept {
      LIBSEMIGROUPS_ASSERT(index < _nr_blocks);
      return (*_lookup)[index];
    }

    //! The vector pointed to by the return value of this function has value
    //! \c true in position \c i if the \c i th block of \c this is a
    //! transverse block; the entry in position \c i is \c false otherwise.
    //!
    //! \returns A pointer to the lookup table for block indices.
    //!
    //! \par Exceptions
    //! This function is `noexcept` and is guaranteed never to throw.
    //!
    //! \par Complexity
    //! Constant.
    //!
    //! \par Parameters
    //! (None)
    // TODO(later) better to have lookup_begin/end methods
    std::vector<bool> const* lookup() const noexcept {
      return _lookup;
    }

    //! This function returns the number of parts in the partition that
    //! instances of this class represent.
    //!
    //! \returns The number of blocks in a Blocks object.
    //!
    //! \par Exceptions
    //! This function is `noexcept` and is guaranteed never to throw.
    //!
    //! \par Complexity
    //! Constant.
    //!
    //! \par Parameters
    //! (None)
    uint32_t nr_blocks() const noexcept {
      return _nr_blocks;
    }

    //! This function returns the number of \c true values in
    //! Blocks::lookup().
    //!
    //! \returns The number of signed (transverse) blocks in \c this.
    //!
    //! \par Exceptions
    //! Throws if `std::count` throws.
    //!
    //! \par Complexity
    //! At most linear in `lookup()->size()`
    //!
    //! \par Parameters
    //! (None)
    uint32_t rank();

    //! This function returns a hash value for an instance of Blocks.  This
    //! value is recomputed every time this function is called.
    //!
    //! \returns A hash value for a \c this.
    //!
    //! \par Exceptions
    //! This function is `noexcept` and is guaranteed never to throw.
    //!
    //! \par Complexity
    //! Linear in `degree()`.
    //!
    //! \par Parameters
    //! (None)
    size_t hash_value() const noexcept;

    //! \returns A `const_iterator` pointing to the index of the first block.
    //!
    //! \par Exceptions
    //! This function is `noexcept` and is guaranteed never to throw.
    //!
    //! \par Complexity
    //! Constant.
    //!
    //! \par Parameters
    //! (None)
    typename std::vector<uint32_t>::const_iterator cbegin() const noexcept {
      LIBSEMIGROUPS_ASSERT(_blocks != nullptr);
      return _blocks->cbegin();
    }

    //! \returns A `const_iterator` referring to past-the-end of the last block.
    //!
    //! \par Exceptions
    //! This function is `noexcept` and is guaranteed never to throw.
    //!
    //! \par Complexity
    //! Constant.
    //!
    //! \par Parameters
    //! (None)
    typename std::vector<uint32_t>::const_iterator cend() const noexcept {
      LIBSEMIGROUPS_ASSERT(_blocks != nullptr);
      return _blocks->cend();
    }

   private:
    std::vector<uint32_t>* _blocks;
    std::vector<bool>*     _lookup;
    uint32_t               _nr_blocks;
    uint32_t               _rank;
  };
}  // namespace libsemigroups

#endif  // LIBSEMIGROUPS_BLOCKS_HPP_
