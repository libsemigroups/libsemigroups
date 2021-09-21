//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2021 James D. Mitchell
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

// This file contains the declaration of the Bipartition and Blocks classes.

#ifndef LIBSEMIGROUPS_BIPART_HPP_
#define LIBSEMIGROUPS_BIPART_HPP_

// TODO(later)
// 1) benchmarks
// 2) use Duf/Suf were possible (later?)
// 3) Template like transformations/pperms etc (later?)

#include <algorithm>         // for max
#include <cstddef>           // for size_t
#include <cstdint>           // for uint32_t, int32_t
#include <initializer_list>  // for initializer_list
#include <stdlib.h>          // for abs
#include <type_traits>       // for decay_t, false_type, is_signed, true_type
#include <unordered_set>     // for unordered_set
#include <vector>            // for vector

#include "adapters.hpp"             // for Hash
#include "constants.hpp"            // for UNDEFINED
#include "exception.hpp"            // for LIBSEMIGROUPS_EXCEPTION
#include "libsemigroups/debug.hpp"  // for LIBSEMIGROUPS_ASSERT

namespace libsemigroups {

  //! Defined ``bipart.hpp``.
  //!
  //! Blocks is a class representing signed partitions of the set
  //! \f$\{0, \ldots, n - 1\}\f$.
  //!
  //! It is possible to associate to every Bipartition a pair of blocks,
  //! Bipartition::left_blocks() and Bipartition::right_blocks(), which
  //! determine the Green's \f$\mathscr{L}\f$- and \f$\mathscr{R}\f$-classes of
  //! the Bipartition in the monoid of all bipartitions. This is the purpose of
  //! this class.
  //!
  //! The Blocks class is not currently used widely in ``libsemigroups``
  //! but are used extensively in the GAP package
  //! [Semigroups package for GAP](https://semigroups.github.io/Semigroups/).
  class Blocks final {
   public:
    //! Type for const iterators pointing to the transverse block lookup.
    using lookup_const_iterator = std::vector<bool>::const_iterator;

    //! Type for iterators pointing to the index of the block.
    using iterator = typename std::vector<uint32_t>::iterator;

    //! Type for const iterators pointing to the index of the block.
    using const_iterator = typename std::vector<uint32_t>::const_iterator;

    //! Constructs a blocks object of size 0.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    //!
    //! \parameters
    //! (None)
    Blocks() noexcept = default;

    //! Constructs a blocks object from iterators.
    //!
    //! The degree of the blocks object constructed is `last - first / 2`.
    //!
    //! \param first the index of the block containing the first point
    //! \param last  the index of the block containing the first point
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Linear in `last - first`.
    //!
    //! \warning
    //! No checks are made on the validity of the arguments to this function.
    //!
    //! \sa validate(Blocks const&)
    Blocks(const_iterator first, const_iterator last);

    //! Constructs a blocks object of given degree.
    //!
    //! \param degree the degree of the blocks object to construct.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Linear in \p degree.
    Blocks(size_t degree) : _blocks(degree), _lookup() {}

    //! Default copy assignment operator.
    Blocks& operator=(Blocks const&) = default;

    //! Default move assignment operator.
    Blocks& operator=(Blocks&&) = default;

    //! Default copy constructor.
    Blocks(Blocks const& copy) = default;

    //! Default move constructor.
    Blocks(Blocks&& copy) = default;

    ~Blocks();

    //! Set whether or not the block containing a point is transverse.
    //!
    //! \param i the point.
    //! \param val whether or not the block containing \p i is transverse.
    //!
    //! \returns
    //! (None)
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Constant.
    //!
    //! \warning
    //! No checks are made on the validity of the arguments to this function.
    void set_is_transverse_block(size_t i, bool val) {
      LIBSEMIGROUPS_ASSERT(i < _lookup.size());
      _lookup[i] = val;
    }

    //! Set the block that a point belongs to.
    //!
    //! \param i the point.
    //! \param val the block that \p i should belong to.
    //!
    //! \returns
    //! (None)
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! At worst linear in `degree()`.
    //!
    //! \warning
    //! No checks are made on the validity of the arguments to this function.
    void set_block(size_t i, uint32_t val) {
      LIBSEMIGROUPS_ASSERT(i < _blocks.size());
      _blocks[i] = val;
      if (val >= _lookup.size()) {
        _lookup.resize(val + 1);
      }
    }

    //! Compare two blocks objects for equality.
    //!
    //! Two Blocks objects are equal if and only if their underlying signed
    //! partitions are equal. It is ok to compare blocks of different
    //! degree with this operator.
    //!
    //! \param that a Blocks instance
    //!
    //! \returns \c true if \c this equals \p that.
    //!
    //! \exceptions
    //! This function only throws if \c std::vector<uint32_t>::operator== does.
    //!
    //! \complexity
    //! At worst linear in `degree()`.
    bool operator==(Blocks const& that) const;

    //! Compare two blocks objects for inequality.
    //!
    //! Two Blocks objects are equal if and only if their underlying signed
    //! partitions are equal. It is ok to compare blocks of different
    //! degree with this operator.
    //!
    //! \param that a Blocks instance
    //!
    //! \returns \c true if \c this equals \p that.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! At worst linear in `degree()`.
    bool operator!=(Blocks const& that) const {
      return !(*this == that);
    }

    //! Compare two blocks objects for less.
    //!
    //! This operator defines a total order on the set of all Blocks objects
    //! (including those of different degree).
    //!
    //! \param that a Blocks instance
    //!
    //! \returns \c true if \c this is less than \p that.
    //!
    //! \exceptions
    //! This function only throws if \c std::vector<uint32_t>::operator[] does.
    //!
    //!
    //! \complexity
    //! Linear in `degree()`.
    bool operator<(Blocks const& that) const;

    //! Returns the degree of a blocks object.
    //!
    //! The *degree* of a Blocks object is the size of the set of
    //! which it is a partition, or the size of the \p blocks parameter to
    //! Blocks::Blocks.
    //!
    //! \returns The degree of a Blocks object.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \parameters
    //! (None)
    uint32_t degree() const noexcept {
      return _blocks.size();
    }

    //! Check if a block is a transverse block.
    //!
    //! This function returns \c true if the block with index \p index is a
    //! transverse (or signed) block and it returns \c false if it is not
    //! transverse (or unsigned).
    //!
    //! \param index the index of a block
    //!
    //! \returns \c true if the block with index \p index is transverse, and \c
    //! false if not.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    bool is_transverse_block(size_t index) const noexcept {
      LIBSEMIGROUPS_ASSERT(index < _lookup.size());
      return _lookup[index];
    }

    //! Returns the number of blocks in a Blocks object.
    //!
    //! This function returns the number of parts in the partition that
    //! instances of this class represent.
    //!
    //! \returns The number of blocks in a Bipartition object.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! At worst \f$O(2n)\f$ where \f$n\f$ is the degree().
    //!
    //! \parameters
    //! (None)
    uint32_t number_of_blocks() const noexcept {
      return _lookup.size();
    }

    //! Returns the number of transverse blocks.
    //!
    //! This function returns the number of \c true values in
    //! lookup().
    //!
    //! \returns The number of signed (transverse) blocks in \c this.
    //!
    //! \exceptions
    //! Throws if `std::count` throws.
    //!
    //! \complexity
    //! At most linear in the number of blocks.
    //!
    //! \parameters
    //! (None)
    uint32_t rank() const;

    //! Returns a hash value for a Blocks instance.
    //!
    //! This value is recomputed every time this function is called.
    //!
    //! \returns A hash value for a \c this.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Linear in `degree()`.
    //!
    //! \parameters
    //! (None)
    size_t hash_value() const noexcept;

    //! Returns a const iterator pointing to the first transverse
    //! block lookup.
    //!
    //! The value pointed to is \c true if the \c i th block of \c this is a
    //! transverse block; and \c false otherwise.
    //!
    //! \returns A \ref lookup_const_iterator.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    //!
    //! \parameters
    //! (None)
    lookup_const_iterator cbegin_lookup() const noexcept {
      return _lookup.cbegin();
    }

    //! Returns a const iterator pointing to the first transverse
    //! block lookup.
    //!
    //! \sa cbegin_lookup.
    lookup_const_iterator cend_lookup() const noexcept {
      return _lookup.cend();
    }

    //! Returns a const iterator pointing to the index of the first block.
    //!
    //! \returns A value of type \ref const_iterator.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    //!
    //! \parameters
    //! (None)
    const_iterator cbegin() const noexcept {
      return _blocks.cbegin();
    }

    //! Returns a const iterator pointing one past-the-end of the last block.
    //!
    //! \returns A value of type \ref const_iterator
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    //!
    //! \parameters
    //! (None)
    const_iterator cend() const noexcept {
      return _blocks.cend();
    }

    //! Returns a const reference to the index of the block containing a point.
    //!
    //! \param i the point.
    //!
    //! \returns A value const reference to a value of type \c uint32_t.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Constant.
    uint32_t const& operator[](size_t i) const {
      LIBSEMIGROUPS_ASSERT(i < _blocks.size());
      return _blocks[i];
    }

   private:
    std::vector<uint32_t> _blocks;
    std::vector<bool>     _lookup;
  };

  //! Validates a Blocks object.
  //!
  //! \param x the blocks object to validate.
  //!
  //! \returns
  //! (None)
  //!
  //! \throws LibsemigroupsException if \p x is invalid.
  void validate(Blocks const& x);

  // Forward decl
  class Bipartition;

  //! Validates a bipartition.
  //!
  //! \param x the bipartition
  //!
  //! \returns
  //! (None)
  //!
  //! \throws LibsemigroupsException if \p x is invalid.
  void validate(Bipartition const& x);

  //! Defined in ``bipart.hpp``.
  //!
  //! A *bipartition* is a partition of the set \f$\{0, ..., 2n - 1\}\f$ for
  //! some non-negative integer \f$n\f$; see the [Semigroups package for GAP
  //! documentation](https://semigroups.github.io/Semigroups/doc/chap3_mj.html)
  //! for more details.  The Bipartition class is more complex (i.e. has more
  //! member functions) than are used in ``libsemigroups`` because they are
  //! used in the GAP package [Semigroups package for
  //! GAP](https://semigroups.github.io/Semigroups/).
  //!
  //! \sa libsemigroups::validate(Bipartition const&).
  // TODO(later) add more explanation to the doc here
  class Bipartition final {
   public:
    //! Type for iterators pointing to the lookup for the blocks of a
    //! bipartition.
    using iterator = std::vector<uint32_t>::iterator;

    //! Type for const iterators pointing to the lookup for the blocks of a
    //! bipartition.
    using const_iterator = std::vector<uint32_t>::const_iterator;

    //! Type for  iterators pointing to the lookup for transverse blocks of a
    //! bipartition.
    using lookup_const_iterator = typename std::vector<bool>::const_iterator;

    //! Constructs an uninitialised bipartition of degree \c 0.
    Bipartition();

    //! Constructs an uninitialised bipartition of given degree.
    //!
    //! \param N the degree of the bipartition.
    explicit Bipartition(size_t N);

    //! Constructs a bipartition from a const reference to blocks lookup.
    //!
    //! The parameter `blocks`:
    //! * is copied;
    //! * must have length \f$2n\f$ for some  positive integer \f$n\f$;
    //! * consist of non-negative integers; and
    //! * have the property that if \f$i\f$, \f$i > 0\f$ occurs in \p blocks,
    //! then \f$i - 1\f$ occurs earlier in \p blocks.  The value of \p block[i]
    //! should represent the index of the block containing \c i.
    //!
    //! None of the conditions above is verified.
    //!
    //! \param blocks a lookup for the blocks of the bipartition being
    //! constructed.
    //!
    //! \sa libsemigroups::validate(Bipartition const&).
    explicit Bipartition(std::vector<uint32_t> const& blocks);

    //! Constructs a bipartition from an rvalue reference to blocks lookup.
    //!
    //! \param blocks a lookup for the blocks of the bipartition being
    //! constructed.
    //!
    //! \sa Bipartition(std::vector<uint32_t> const&)
    //!  and libsemigroups::validate(Bipartition const&).
    explicit Bipartition(std::vector<uint32_t>&& blocks);

    //! Constructs a bipartition from an initializer list blocks lookup.
    //!
    //! \param blocks a lookup for the blocks of the bipartition being
    //! constructed.
    //!
    //! \sa Bipartition(std::vector<uint32_t> const&)
    //!  and libsemigroups::validate(Bipartition const&).
    Bipartition(std::initializer_list<uint32_t> const& blocks);

    //! Constructs a bipartition from a partition.
    //!
    //! The items in \p blocks should be:
    //! * duplicate-free;
    //! * pairwise disjoint; and
    //! * partition the set \f$\{-n, \ldots,  1\}\cup \{1, \ldots, n\}\f$
    //! for some positive integer \f$n\f$.
    //!
    //! None of these conditions is checked by the constructor.
    //!
    //! \param blocks the partition.
    //!
    //! \sa libsemigroups::validate(Bipartition const&).
    Bipartition(std::initializer_list<std::vector<int32_t>> const& blocks);

    //! Default copy constructor.
    Bipartition(Bipartition const&);

    //! Default move constructor.
    Bipartition(Bipartition&&);

    //! Default copy assignment operator.
    Bipartition& operator=(Bipartition const&);

    //! Default move assignment operator.
    Bipartition& operator=(Bipartition&&);

    ~Bipartition();

    //! Validates the arguments, constructs a bipartition and validates it.
    //!
    //! \tparam T the type of the parameter \p cont
    //!
    //! \param cont either a vector providing a lookup for the blocks of the
    //! bipartition or a vector of vectors (or initializer list).
    //!
    //! \throws LibsemigroupsException if the arguments do not describe a
    //! bipartition.
    //!
    //! \throws LibsemigroupsException if the constructed bipartition is not
    //! valid.
    template <typename T>
    static Bipartition make(T const& cont) {
      validate_args(cont);
      Bipartition result(cont);
      validate(result);
      return result;
    }

    //! Validates the arguments, constructs a bipartition and validates it.
    //!
    //! See make(T const&) for full details.
    static Bipartition make(std::initializer_list<uint32_t> const& cont) {
      return make<std::initializer_list<uint32_t>>(cont);
    }

    //! Validates the arguments, constructs a bipartition and validates it.
    //!
    //! See make(T const&) for full details.
    static Bipartition
    make(std::initializer_list<std::vector<int32_t>> const& cont) {
      return make<std::initializer_list<std::vector<int32_t>>>(cont);
    }

    //! Compare two bipartitions for equality.
    //!
    //! \param that a Bipartition object
    //!
    //! \returns \c true if \c this equals \p that, and \c false otherwise.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! At worst linear in degree().
    bool operator==(Bipartition const& that) const {
      return _vector == that._vector;
    }

    //! Compare two bipartitions for less.
    //!
    //! \param that a Bipartition object
    //!
    //! \returns \c true if \c this is less than \p that, and \c false
    //! otherwise.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! At worst linear in degree().
    bool operator<(Bipartition const& that) const {
      return _vector < that._vector;
    }

    //! Returns a hash value.
    //!
    //! \parameters
    //! (None)
    //!
    //! \returns
    //! A value of \c size_t.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Linear in degree().
    // not noexcept because Hash<T>::operator() isn't
    size_t hash_value() const {
      return Hash<std::vector<uint32_t>>()(_vector);
    }

    //! Returns the index of the block containing a value.
    //!
    //! No bound checks are performed on the parameter \p i.
    //!
    //! \param i an integer
    //!
    //! \returns A reference to the index of the block containing \p i.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Constant.
    uint32_t& operator[](size_t i) {
      return _vector[i];
    }

    //! Returns the index of the block containing a value.
    //!
    //! No bound checks are performed on the parameter \p i.
    //!
    //! \param i an integer
    //!
    //! \returns A const reference to the index of the block containing \p i.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Constant.
    uint32_t const& operator[](size_t i) const {
      return _vector[i];
    }

    //! Returns a reference to the index of the block containing a value.
    //!
    //! \param i an integer
    //!
    //! \returns A reference to the index of the block containing \p i.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \throws std::out_of_range if the parameter \p i is out of range.
    //!
    //! \complexity
    //! Constant.
    uint32_t& at(size_t i) {
      return _vector.at(i);
    }

    //! Returns a const reference to the index of the block containing a value.
    //!
    //! \param i an integer
    //!
    //! \returns A const reference to the index of the block containing \p i.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \throws std::out_of_range if the parameter \p i is out of range.
    //!
    //! \complexity
    //! Constant.
    uint32_t const& at(size_t i) const {
      return _vector.at(i);
    }

    //! Returns a const iterator pointing to the index of the first block.
    //!
    //! \returns A value of type \ref const_iterator.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    //!
    //! \parameters
    //! (None)
    const_iterator cbegin() const noexcept {
      return _vector.cbegin();
    }

    //! Returns a const iterator pointing one passed the last index of the
    //! last block.
    //!
    //! \returns A value of type \ref const_iterator.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    //!
    //! \parameters
    //! (None)
    const_iterator cend() const noexcept {
      return _vector.cend();
    }

    //! Returns a const iterator pointing to the index of the first left
    //! block.
    //!
    //! \returns A value of type \ref const_iterator.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    //!
    //! \parameters
    //! (None)
    const_iterator cbegin_left_blocks() const noexcept {
      return cbegin();
    }

    //! Returns a const iterator pointing one passed the last index of the
    //! last left block.
    //!
    //! \returns A value of type \ref const_iterator.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    //!
    //! \parameters
    //! (None)
    const_iterator cend_left_blocks() const noexcept {
      return cbegin() + degree();
    }

    //! Returns a const iterator pointing to the index of the first right
    //! block.
    //!
    //! \returns A value of type \ref const_iterator.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    //!
    //! \parameters
    //! (None)
    const_iterator cbegin_right_blocks() const noexcept {
      return cend_left_blocks();
    }

    //! Returns a const iterator pointing one passed the last index of the
    //! last right block.
    //!
    //! \returns A value of type \ref const_iterator.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    //!
    //! \parameters
    //! (None)
    const_iterator cend_right_blocks() const noexcept {
      return cend();
    }

    //! Returns the degree of the bipartition.
    //!
    //! A bipartition is of degree \f$n\f$ if it is a partition of
    //! \f$\{0, \ldots, 2n -  1\}\f$.
    //!
    //! \returns A value of type `size_t`.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \parameters
    //! (None)
    size_t degree() const noexcept;

    //! Returns an identity bipartition.
    //!
    //! The *identity bipartition* of degree \f$n\f$ has blocks \f$\{i, -i\}\f$
    //! for all \f$i\in \{0, \ldots, n - 1\}\f$. This member function returns a
    //! new identity bipartition of degree equal to the degree of \c this.
    //!
    //! \returns A newly constructed Bipartition.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \parameters
    //! (None)
    Bipartition identity() const;

    //! Returns an identity bipartition.
    //!
    //! The *identity bipartition* of degree \f$n\f$ has blocks \f$\{i, -i\}\f$
    //! for all \f$i\in \{0, \ldots, n - 1\}\f$. This member function returns a
    //! new identity bipartition of degree equal to \p n.
    //!
    //! \param n the degree of the identity to be returned.
    //!
    //! \returns A newly constructed Bipartition.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    static Bipartition identity(size_t n);

    //! Modify the current bipartition in-place to contain the product of two
    //! bipartitions.
    //!
    //! The parameter \p thread_id can be used some temporary storage is
    //! required to find the product of \p x and \p y.
    //!
    //! \param x the first bipartition to multiply
    //! \param y the second bipartition to multiply
    //! \param thread_id the index of the calling thread (defaults to \c 0)
    //!
    //! \returns
    //! (None)
    //!
    //! \warning
    //! If different threads call this function concurrently with the same
    //! parameter \p thread_id, then bad things will happen.
    void product_inplace(Bipartition const& x,
                         Bipartition const& y,
                         size_t             thread_id = 0);

    //! Returns the number of transverse blocks.
    //!
    //! The *rank* of a bipartition is the number of blocks containing both
    //! positive and negative values.
    //!
    //! \returns A value of type \c size_t.
    //!
    //! \parameters
    //! (None)
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! \f$O(2n)\f$ where \f$n\f$ is the degree().
    size_t rank();

    //! Returns the number of blocks in a Bipartition.
    //!
    //! This function returns the number of parts in the partition that
    //! instances of this class represent.
    //!
    //! \returns The number of blocks in a Blocks object.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! At worst \f$O(2n)\f$ where \f$n\f$ is the degree().
    //!
    //! \parameters
    //! (None)
    uint32_t number_of_blocks() const;

    //! Returns the number of blocks containing a positive integer.
    //!
    //! The *left blocks* of a bipartition is the partition of
    //! \f$\{0, \ldots, n - 1\}\f$ induced by the bipartition. This member
    //! function returns the number of blocks in this partition.
    //!
    //! \returns
    //! A value of type \c uint32_t.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! At worst \f$O(n)\f$ where \f$n\f$ is the degree().
    //!
    //! \parameters
    //! (None)
    uint32_t number_of_left_blocks();

    //! Returns the number of blocks containing a negative integer.
    //!
    //! The *right blocks* of a bipartition is the partition of
    //! \f$\{n, \ldots, 2n - 1\}\f$ induced by the bipartition. This member
    //! function returns the number of blocks in this partition.
    //!
    //! \returns
    //! A value of type \c uint32_t.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! At worst \f$O(n)\f$ where \f$n\f$ is the degree().
    //!
    //! \parameters
    //! (None)
    uint32_t number_of_right_blocks();

    //! Check if a block is a transverse block.
    //!
    //! A block of a biparition is *transverse* if it contains integers less
    //! than and greater than \f$n\f$, which is the degree of the bipartition.
    //! This member function asserts that the parameter \p index is less than
    //! the number of blocks in the bipartition.
    //!
    //! \param index the index of a block
    //!
    //! \returns \c true if the block with index \p index is transverse, and \c
    //! false if not.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! At worst \f$O(n)\f$ where \f$n\f$ is the degree().
    bool is_transverse_block(size_t index);

    //! Return a pointer to the left blocks of a bipartition.
    //!
    //! The *left blocks* of a bipartition is the partition of
    //! \f$\{0, \ldots, n - 1\}\f$ induced by the bipartition. This member
    //! function returns a Blocks object representing this partition.
    //!
    //! \returns
    //! A pointer to a newly constructed Blocks object.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! \f$O(n)\f$ where \f$n\f$ is the degree().
    //!
    //! \parameters
    //! (None)
    // TODO(later) remove this
    Blocks* left_blocks();

    //! Return a pointer to the right blocks of a bipartition.
    //!
    //! The *right blocks* of a bipartition is the partition of
    //! \f$\{n, \ldots, 2n - 1\}\f$ induced by the bipartition.
    //!
    //! \returns
    //! A pointer to a newly constructed Blocks object.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! \f$O(n)\f$ where \f$n\f$ is the degree().
    //!
    //! \parameters
    //! (None)
    // TODO(later) remove this
    Blocks* right_blocks();

    //! Set the number of blocks.
    //!
    //! This function sets the number of blocks of \c this to \p n. No checks
    //! are performed.
    //!
    //! \param n the number of blocks.
    //!
    //! \returns
    //! (None)
    //!
    //! \complexity
    //! Constant.
    //!
    //! \exceptions
    //! \noexcept
    void set_number_of_blocks(size_t n) noexcept;

    //! Set the number of left blocks.
    //!
    //! This function sets the number of left blocks of \c this to \p n. No
    //! checks are performed.
    //!
    //! \param n the number of blocks.
    //!
    //! \returns
    //! (None)
    //!
    //! \complexity
    //! Constant.
    //!
    //! \exceptions
    //! \noexcept
    void set_number_of_left_blocks(size_t n) noexcept;

    //! Set the rank.
    //!
    //! This function sets the \c rank of \c this to \p n. No
    //! checks are performed.
    //!
    //! \param n the rank.
    //!
    //! \returns
    //! (None)
    //!
    //! \complexity
    //! Constant.
    //!
    //! \exceptions
    //! \noexcept
    void set_rank(size_t n) noexcept;

    //! Returns a const iterator pointing to the first transverse
    //! block lookup.
    //!
    //! The value pointed to is \c true if the \c i th block of \c this is a
    //! transverse block; and \c false otherwise.
    //!
    //! \returns A \ref lookup_const_iterator.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    //!
    //! \parameters
    //! (None)
    lookup_const_iterator cbegin_lookup() noexcept {
      init_trans_blocks_lookup();
      return _trans_blocks_lookup.cbegin();
    }

    //! Returns a const iterator pointing to the first transverse
    //! block lookup.
    //!
    //! \sa cbegin_lookup.
    lookup_const_iterator cend_lookup() noexcept {
      init_trans_blocks_lookup();
      return _trans_blocks_lookup.cend();
    }

   private:
    static void validate_args(std::vector<uint32_t> const&) {
      // relevant checks are conducted in validate
    }

    template <typename T>
    static void
    validate_args(std::initializer_list<std::vector<T>> const& blocks) {
      static_assert(std::is_signed<T>::value,
                    "the template parameter T must be signed");
      int32_t               m   = 0;
      int32_t               deg = 0;
      std::unordered_set<T> vals;
      for (std::vector<T> const& block : blocks) {
        for (T x : block) {
          vals.insert(x);
          x = std::abs(x);
          if (x == 0) {
            LIBSEMIGROUPS_EXCEPTION(
                "value out of bounds, expected non-zero value found 0");
          }
          m = std::max(x, m);
          deg++;
        }
      }

      if (m >= static_cast<int32_t>(0x40000000)) {
        LIBSEMIGROUPS_EXCEPTION(
            "too many points, expected at most %d, found %d",
            int32_t(0x40000000),
            int32_t(m));
      } else if (deg != 2 * m || vals.size() != size_t(deg)) {
        LIBSEMIGROUPS_EXCEPTION("the union of the given blocks is not "
                                "[%d, -1] ∪ [1, %d], only %d values were given",
                                -m,
                                m,
                                deg);
      }
    }

    void init_trans_blocks_lookup();

    mutable size_t        _nr_blocks;
    size_t                _nr_left_blocks;
    std::vector<bool>     _trans_blocks_lookup;
    size_t                _rank;
    std::vector<uint32_t> _vector;
  };

  namespace detail {

    template <typename T>
    struct IsBipartitionHelper : std::false_type {};

    template <>
    struct IsBipartitionHelper<Bipartition> : std::true_type {};

  }  // namespace detail

  template <typename T>
  static constexpr bool IsBipartition
      = detail::IsBipartitionHelper<std::decay_t<T>>::value;

  //! Multiply two bipartitions.
  //!
  //! Returns a newly constructed bipartition equal to the product of \p x and
  //! \p y.
  //!
  //! \param x a bipartition
  //! \param y a bipartition
  //!
  //! \returns
  //! A value of type \c Bipartition
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  //!
  //! \complexity
  //! Quadratic in degree().
  Bipartition operator*(Bipartition const& x, Bipartition const& y);

  //! Check bipartitions for inequality.
  //!
  //! \param x a bipartition
  //! \param y a bipartition
  //!
  //! \returns
  //! A value of type \c bool.
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  //!
  //! \complexity
  //! At worst linear in the degree of \p x and \p y.
  inline bool operator!=(Bipartition const& x, Bipartition const& y) {
    return !(x == y);
  }

  //! Convenience function that just calls ``operator<`` and ``operator==``.
  inline bool operator<=(Bipartition const& x, Bipartition const& y) {
    return x < y || x == y;
  }

  //! Convenience function that just calls ``operator<``.
  inline bool operator>(Bipartition const& x, Bipartition const& y) {
    return y < x;
  }

  //! Convenience function that just calls ``operator<=``.
  inline bool operator>=(Bipartition const& x, Bipartition const& y) {
    return y <= x;
  }

  ////////////////////////////////////////////////////////////////////////
  // Adapters
  ////////////////////////////////////////////////////////////////////////

  //! Returns the approximate time complexity of multiplication.
  //!
  //! In the case of a Bipartition of degree *n* the value *2n ^ 2* is
  //! returned.
  template <>
  struct Complexity<Bipartition> {
    //! Call operator.
    //!
    //! \param x a const reference to a bipartition.
    //!
    //! \returns
    //! A value of type `size_t` representing the complexity of multiplying the
    //! parameter \p x by another bipartition of the same degree.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    size_t operator()(Bipartition const& x) const noexcept {
      return x.degree() * x.degree();
    }
  };

  template <>
  struct Degree<Bipartition> {
    size_t operator()(Bipartition const& x) const noexcept {
      return x.degree();
    }
  };

  template <>
  struct Hash<Bipartition> {
    size_t operator()(Bipartition const& x) const {
      return x.hash_value();
    }
  };

  template <>
  struct One<Bipartition> {
    Bipartition operator()(Bipartition const& x) const {
      return (*this)(x.degree());
    }

    Bipartition operator()(size_t N = 0) const {
      return Bipartition::identity(N);
    }
  };

  template <>
  struct Product<Bipartition> {
    void operator()(Bipartition&       xy,
                    Bipartition const& x,
                    Bipartition const& y,
                    size_t             thread_id = 0) {
      xy.product_inplace(x, y, thread_id);
    }
  };

  template <>
  struct IncreaseDegree<Bipartition> {
    void operator()(Bipartition&, size_t) {}
  };

}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_BIPART_HPP_
