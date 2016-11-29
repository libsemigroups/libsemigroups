//
// Semigroups++ - C/C++ library for computing with semigroups and monoids
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

#ifndef SEMIGROUPSPLUSPLUS_BLOCKS_H_
#define SEMIGROUPSPLUSPLUS_BLOCKS_H_

#define NDEBUG
#include <assert.h>

#include <algorithm>
#include <functional>
#include <vector>

namespace semigroupsplusplus {

  // Non-abstract
  // Class for blocks, which are signed partitions of the set *{0, ..., n}*
  // for some integer *n*. It is possible to associate to every <Bipartition>
  // a pair of blocks which determine the Green's L- and R-class of the
  // <Bipartition> in the monoid of all partitions. This is the purpose of this
  // class.
  //
  // The **Blocks** class is not currently used by the algorithms for
  // <Semigroup>s but the extra methods are used in the GAP package
  // [Semigroups package for GAP](https://gap-packages.github.io/Semigroups/).

  class Blocks {
   public:
    // 0 parameters
    //
    // Constructs a blocks object of size 0.
    Blocks() : _blocks(nullptr), _lookup(nullptr), _nr_blocks(0), _rank(0) {}

    // 2 parameters
    // @blocks lookup table for the partition being defined.
    // @lookup lookup table for which blocks are signed.
    //
    // The argument <blocks> must have length *n* for some integer *n > 0* and
    // the property that if *i* occurs in <blocks>, then *i - 1* occurs earlier
    // in <blocks>, *i >= 0*. None of this is checked but it probably should be.
    // The argument <blocks> is not copied, and is deleted by the destructor.
    //
    // The argument <lookup> must have length equal to the maximum value in
    // <blocks>, this maximum is also the number of blocks in the partition. A
    // value **true** in position *i* indicates that the *i*th block is signed
    // and
    // **false** that it is unsigned.
    Blocks(std::vector<u_int32_t>* blocks, std::vector<bool>* lookup)
        : _blocks(blocks), _lookup(lookup), _nr_blocks(), _rank(UNDEFINED) {
      assert(_blocks->size() != 0);
      _nr_blocks = *(std::max_element(_blocks->begin(), _blocks->end())) + 1;
      assert(_nr_blocks == _lookup->size());
    }

    // 3 parameters
    // @blocks     lookup table for the partition being defined.
    // @lookup     lookup table for which blocks are signed.
    // @nr_blocks the number of blocks in <blocks>.
    //
    // The argument <blocks> must have length *n* for some integer *n > 0* and
    // the property that if *i* occurs in <blocks>, then *i - 1* occurs earlier
    // in <blocks>, *i >= 0*. None of this is checked but it probably should be.
    // The argument <blocks> is not copied, and is deleted by the destructor of
    // **Blocks**.
    //
    // The argument <lookup> must have length equal to the maximum value in
    // <blocks>, this maximum is also the number of blocks in the partition. A
    // value **true** in position *i* indicates that the *i*th block is signed
    // and
    // **false** that it is unsigned.
    //
    // This constructor is provided for the situation where the number of blocks
    // in <blocks> is known *a priori* and so does not need to be calculated in
    // the constructor.
    Blocks(std::vector<u_int32_t>* blocks,
           std::vector<bool>*      lookup,
           u_int32_t               nr_blocks)
        : _blocks(blocks),
          _lookup(lookup),
          _nr_blocks(nr_blocks),
          _rank(UNDEFINED) {
      assert(_blocks->size() != 0);
      assert(_nr_blocks == _lookup->size());
    }

    // deleted
    // @copy a **Blocks** object
    // The assignment operator is deleted for **Blocks** to avoid unintended
    // copying.
    //
    // @return nothing as it cannot be envoked.
    Blocks& operator=(Blocks const& copy) = delete;

    // Copy
    // @copy   the blocks to copy.
    //
    // Copies all the information in <copy> and returns a new instance of
    // **Blocks**.
    Blocks(Blocks const& copy);

    // Default
    //
    // Deletes the blocks and lookup provided at construction time.
    ~Blocks() {
      delete _blocks;
      delete _lookup;
    }

    // const
    // @that another blocks object.
    //
    // Two **Blocks** objects are equal if and only if their underlying signed
    // partitions are equal. It is ok to compare blocks of different
    // <degree> with this operator.
    // @return **true** or **false**.
    bool operator==(const Blocks& that) const;

    // const
    // @that another blocks object.
    //
    // This operator defines a total order on the set of all blocks (including
    // those of different <degree>).
    // @return **true** or **false**.
    bool operator<(const Blocks& that) const;

    // const
    //
    // @return the degree of a **Blocks** object which is the size of the set on
    // which it is a partition.
    inline u_int32_t degree() const {
      return (_nr_blocks == 0 ? 0 : _blocks->size());
    }

    // const
    // @pos an integer less than <degree>.
    //
    // This method asserts that the argument is valid.
    //
    // @return the index of the block containing <pos>.
    inline u_int32_t block(size_t pos) const {
      assert(pos < _blocks->size());
      return (*_blocks)[pos];
    }

    // const
    // @index an integer less than <nr_blocks>.
    //
    // This method asserts that the argument is valid.
    //
    // @return whether or not the block with index <index> is a transverse
    // (signed) block or not.
    inline bool is_transverse_block(size_t index) const {
      assert(index < _nr_blocks);
      return (*_lookup)[index];
    }

    // const
    //
    // @return a pointer to the lookup table for block indices.
    inline const std::vector<bool>* lookup() const {
      return _lookup;
    }

    // const
    //
    // @return the number of blocks in the **Blocks** object.
    inline u_int32_t nr_blocks() const {
      return _nr_blocks;
    }

    // non-const
    //
    // @return the number of signed (transverse) blocks in the **Blocks**
    // object.
    u_int32_t rank();

    // const
    //
    // @return a hash value for a **Blocks** object.
    size_t hash_value() const;

    // const
    //
    // This method asserts that <degree> is not 0.
    //
    // @return A const_iterator pointing to the first value in the blocks
    // lookup.
    inline typename std::vector<u_int32_t>::const_iterator cbegin() const {
      assert(_blocks != nullptr);
      return _blocks->cbegin();
    }

    // const
    //
    // This method asserts that <degree> is not 0.
    //
    // @return A const_iterator referring to the past-the-end element in
    // blocks lookup.
    inline typename std::vector<u_int32_t>::const_iterator cend() const {
      assert(_blocks != nullptr);
      return _blocks->cend();
    }

   private:
    std::vector<u_int32_t>* _blocks;
    std::vector<bool>*      _lookup;
    u_int32_t               _nr_blocks;
    u_int32_t               _rank;
    static u_int32_t        UNDEFINED;
  };
}  // namespace semigroupsplusplus

#endif  // SEMIGROUPSPLUSPLUS_BLOCKS_H_
