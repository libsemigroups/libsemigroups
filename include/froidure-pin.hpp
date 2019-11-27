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

// TODO(later)
// 1. Use the new document template for all methods.
// 2. noexcept

#ifndef LIBSEMIGROUPS_INCLUDE_FROIDURE_PIN_HPP_
#define LIBSEMIGROUPS_INCLUDE_FROIDURE_PIN_HPP_

#include <cstddef>        // for size_t
#include <iterator>       // for reverse_iterator
#include <mutex>          // for mutex
#include <type_traits>    // for is_const, remove_pointer
#include <unordered_map>  // for unordered_map
#include <utility>        // for pair
#include <vector>         // for vector

#include "adapters.hpp"           // for Complexity, Degree, IncreaseDegree
#include "bruidhinn-traits.hpp"   // for detail::BruidhinnTraits
#include "constants.hpp"          // for libsemigroups::UNDEFINED, LIMIT_MAX
#include "containers.hpp"         // for DynamicArray2
#include "froidure-pin-base.hpp"  // for FroidurePinBase, FroidurePinBase::s...
#include "iterator.hpp"           // for ConstIteratorStateless
// #include "libsemigroups-config.hpp"  // for LIBSEMIGROUPS_DENSEHASHMAP
#include "stl.hpp"    // for EqualTo, Hash
#include "types.hpp"  // for letter_type, word_type

// #ifdef LIBSEMIGROUPS_DENSEHASHMAP
// #include "extern/sparsehash-c11/sparsehash/dense_hash_map"
// #endif

//! Namespace for everything in the libsemigroups library.
namespace libsemigroups {

  //! Defined in ``froidure-pin.hpp``.
  //!
  //! This is a traits class for use with FroidurePin.
  //!
  //! \sa FroidurePinBase and FroidurePin.
  template <typename TElementType>
  struct FroidurePinTraits {
    // Require to get the value_type from detail::BruidhinnTraits to remove
    // pointer to const.
    //! The type of the elements of a FroidurePin instance with const removed,
    //! and if TElementType is a pointer to const, then the second const is
    //! also removed.
    using element_type =
        typename detail::BruidhinnTraits<TElementType>::value_type;

    //! \copydoc libsemigroups::Complexity
    using Complexity = ::libsemigroups::Complexity<element_type>;

    //! \copydoc libsemigroups::Degree
    using Degree = ::libsemigroups::Degree<element_type>;

    //! \copydoc libsemigroups::EqualTo
    using EqualTo = ::libsemigroups::EqualTo<element_type>;

    //! \copydoc libsemigroups::Hash
    using Hash = ::libsemigroups::Hash<element_type>;

    //! \copydoc libsemigroups::IncreaseDegree
    using IncreaseDegree = ::libsemigroups::IncreaseDegree<element_type>;

    //! \copydoc libsemigroups::Less
    using Less = ::libsemigroups::Less<element_type>;

    //! \copydoc libsemigroups::One
    using One = ::libsemigroups::One<element_type>;

    //! \copydoc libsemigroups::Product
    using Product = ::libsemigroups::Product<element_type>;

    //! \copydoc libsemigroups::Swap
    using Swap = ::libsemigroups::Swap<element_type>;
  };

  //! Defined in ``froidure-pin.hpp``.
  //!
  //! The class template FroidurePin implements the Froidure-Pin algorithm as
  //! described in the article "Algorithms for computing finite semigroups"
  //! by Veronique Froidure and Jean-Eric Pin; see [this] for more details.
  //!
  //! A FroidurePin instance is defined by a generating set, and the main
  //! member function is FroidurePin::run, which implements the
  //! [Froidure-Pin Algorithm].  If FroidurePin::run is invoked and
  //! FroidurePin::finished returns \c true, then the size, the left and right
  //! Cayley graphs are determined, and a confluent terminating presentation
  //! for the semigroup is known.
  //!
  //! \sa FroidurePinTraits and FroidurePinBase.
  //!
  //! [this]: https://www.irif.fr/~jep/PDF/Rio.pdf
  //! [Froidure-Pin Algorithm]: https://www.irif.fr/~jep/PDF/Rio.pdf
  //!
  //! \par Example
  //! \code
  //! template <>
  //! struct Complexity<int> {
  //!   constexpr size_t operator()(int) const noexcept {
  //!     return 0;
  //!   }
  //! };
  //! template <>
  //! struct Degree<int> {
  //!   constexpr size_t operator()(int) const noexcept {
  //!     return 0;
  //!   }
  //! };
  //! template <>
  //! struct IncreaseDegree<int> {
  //!   int operator()(int x) const noexcept {
  //!     return x;
  //!   }
  //! };
  //! template <>
  //! struct One<int> {
  //!   constexpr int operator()(int) const noexcept {
  //!     return 1;
  //!   }
  //! };
  //! template <>
  //! struct Product<int> {
  //!   void operator()(int& xy,
  //!                   int  x,
  //!                   int  y,
  //!                   size_t = 0) const noexcept {
  //!     xy = x * y;
  //!   }
  //! };
  //! FroidurePin<int> S({2});
  //! S.size();           // 32
  //! S.nr_idempotents()  // 1
  //! *S.cbegin();        // 2
  //!
  //! FroidurePin<uint8_t> T({2, 3});
  //! T.size()                      // 130
  //! T.nr_idempotents()            // 2
  //! *T.cbegin_idempotents();      // 0
  //! *T.cbegin_idempotents() + 1;  // 1
  //! \endcode
  template <typename TElementType,
            typename TTraits = FroidurePinTraits<TElementType>>
  class FroidurePin final : private detail::BruidhinnTraits<TElementType>,
                            public FroidurePinBase {
   private:
    ////////////////////////////////////////////////////////////////////////
    // FroidurePin - typedefs - private
    ////////////////////////////////////////////////////////////////////////

    using internal_element_type =
        typename detail::BruidhinnTraits<TElementType>::internal_value_type;
    using internal_const_element_type = typename detail::BruidhinnTraits<
        TElementType>::internal_const_value_type;
    using internal_const_reference = typename detail::BruidhinnTraits<
        TElementType>::internal_const_reference;

    static_assert(
        std::is_const<internal_const_element_type>::value
            || std::is_const<typename std::remove_pointer<
                internal_const_element_type>::type>::value,
        "internal_const_element_type must be const or pointer to const");

    // The elements of a semigroup are stored in _elements, but because of the
    // way add_generators/closure work, it might not be the case that all the
    // words of a given length are contiguous in _elements. Hence we require a
    // means of finding the next element of a given length. In
    // _enumerate_order, the first K_1 values are element_index_type's equal to
    // the positions in _elements of the words of length 1, the next K_2 values
    // are the element_index_type's equal to the positions in _elements of the
    // words of length 2, and so on.
    //
    // This alias is used to distinguish variables that refer to positions in
    // _elements (element_index_type) from those that refer to positions in
    // _enumerate_order (enumerate_index_type).
    using enumerate_index_type = FroidurePinBase::size_type;

   public:
    ////////////////////////////////////////////////////////////////////////
    // FroidurePin - typedefs - public
    ////////////////////////////////////////////////////////////////////////

    //! The type of the elements of the semigroup represented by \c this.
    using element_type =
        typename detail::BruidhinnTraits<TElementType>::value_type;

    //! The const type of the elements of the semigroup represented by \c this.
    using const_element_type =
        typename detail::BruidhinnTraits<TElementType>::const_value_type;

    //! The type of a const reference to an element of the semigroup
    //! represented by \c this.
    using const_reference =
        typename detail::BruidhinnTraits<TElementType>::const_reference;

    //! The type of a const pointer to an element of the semigroup
    //! represented by \c this.
    using const_pointer =
        typename detail::BruidhinnTraits<TElementType>::const_pointer;

    //! \copydoc FroidurePinBase::size_type
    using size_type = FroidurePinBase::size_type;

    //! \copydoc FroidurePinBase::element_index_type
    using element_index_type = FroidurePinBase::element_index_type;

    //! \copydoc FroidurePinBase::cayley_graph_type
    using cayley_graph_type = FroidurePinBase::cayley_graph_type;

    //! \copydoc libsemigroups::Complexity
    using Complexity = typename TTraits::Complexity;

    //! \copydoc libsemigroups::Degree
    using Degree = typename TTraits::Degree;

    //! \copydoc libsemigroups::EqualTo
    using EqualTo = typename TTraits::EqualTo;

    //! \copydoc libsemigroups::Hash
    using Hash = typename TTraits::Hash;

    //! \copydoc libsemigroups::IncreaseDegree
    using IncreaseDegree = typename TTraits::IncreaseDegree;

    //! \copydoc libsemigroups::Less
    using Less = typename TTraits::Less;

    //! \copydoc libsemigroups::One
    using One = typename TTraits::One;

    //! \copydoc libsemigroups::Product
    using Product = typename TTraits::Product;

    //! \copydoc libsemigroups::Swap
    using Swap = typename TTraits::Swap;

   private:
    struct InternalEqualTo : private detail::BruidhinnTraits<TElementType> {
      bool operator()(internal_const_reference x,
                      internal_const_reference y) const {
        return EqualTo()(this->to_external_const(x),
                         this->to_external_const(y));
      }
    };

    struct InternalHash : private detail::BruidhinnTraits<TElementType> {
      size_t operator()(internal_const_reference x) const {
        return Hash()(this->to_external_const(x));
      }
    };

   public:
    ////////////////////////////////////////////////////////////////////////
    // FroidurePin - constructors + destructor - public
    ////////////////////////////////////////////////////////////////////////

    //! Deleted.
    //!
    //! The FroidurePin class does not support an assignment contructor to avoid
    //! accidental copying. An object in FroidurePin may use many gigabytes of
    //! memory and might be extremely expensive to copy. A copy constructor is
    //! provided in case such a copy should it be required anyway.
    FroidurePin& operator=(FroidurePin const&) = delete;

    //! Deleted.
    FroidurePin& operator=(FroidurePin&&) = delete;

    //! Construct from generators.
    //!
    //! This is the default constructor for a semigroup generated by a vector
    //! of generators.  There can be duplicate generators and although they do
    //! not count as distinct elements, they do count as distinct generators.
    //! In other words, the generators of the semigroup are precisely (a copy
    //! of) \p gens in the same order they occur in \p gens.
    //!
    //! The generators \p gens are copied by the constructor, and so it is the
    //! responsibility of the caller to delete \p gens.
    //!
    //! \param gens the generators of the semigroup represented by \c this.
    //!
    //! \throws LibsemigroupsException if \p gens is empty, or
    //! FroidurePin::Degree()(x) != FroidurePin::Degree()(y) for \c x, \c y in
    //! \p gens.
    //!
    //! \complexity
    //! Constant.
    explicit FroidurePin(std::vector<element_type> const* gens);

    //! Construct from generators.
    //!
    //! This constructor simply calls the above constructor with a pointer to \p
    //! gens.
    explicit FroidurePin(std::vector<element_type> const&);

    //! Construct from generators.
    //!
    //! This constructor simply constructs a vector from \p gens and calls the
    //! above constructor.
    explicit FroidurePin(std::initializer_list<element_type>);

    //! Copy constructor.
    //!
    //! Constructs a new FroidurePin which is an exact copy of \p copy. No
    //! enumeration is triggered for either \p copy or of the newly constructed
    //! semigroup.
    FroidurePin(FroidurePin const&);

    //! Default move constructor.
    FroidurePin(FroidurePin&&) = default;

    ~FroidurePin();

   private:
    ////////////////////////////////////////////////////////////////////////
    // FroidurePin - constructor - private
    ////////////////////////////////////////////////////////////////////////

    FroidurePin(FroidurePin const&, std::vector<element_type> const*);

   public:
    ////////////////////////////////////////////////////////////////////////
    // FroidurePin - member functions - public
    ////////////////////////////////////////////////////////////////////////

    //! Returns the position in the semigroup corresponding to the element
    //! represented by the word \p w.
    //!
    //! The parameter \p w must consist of non-negative integers less than
    //! FroidurePin::nr_generators, or a LibsemigroupsException will be thrown.
    //! This member function returns the position in \c this of the element
    //! obtained by evaluating \p w. This member function does not perform any
    //! enumeration of the semigroup, and will return libsemigroups::UNDEFINED
    //! if the position of the element of \c this corresponding to \p w cannot
    //! be determined.
    //!
    //! This is equivalent to finding the product \c x of the
    //! generators FroidurePin::generator(\c w[i]) and then calling
    //! FroidurePin::position_current with argument \c x.
    //!
    //! \sa FroidurePin::word_to_element.
    element_index_type word_to_pos(word_type const&) const override;

    //! Returns a copy of the element of \c this represented by the word
    //! \p w.
    //!
    //! The parameter \p w must consist of non-negative integers less than
    //! FroidurePin::nrgens, or a LibsemigroupsException will be thrown.
    //! This member function returns a copy of the element of \c this obtained
    //! by evaluating \p w. This is equivalent to finding the product \c x of
    //! the generators FroidurePin::generator(\c w[i]).
    //!
    //! A copy is returned instead of a reference, because the element of \c
    //! this corresponding to \p w may not yet have been enumerated.
    //!
    //! \sa FroidurePin::word_to_pos.
    element_type word_to_element(word_type const&) const;

    //! Returns \c true if the parameters represent the same element of the
    //! semigroup and \c false otherwise.
    bool equal_to(word_type const&, word_type const&) const override;

    //! Returns the maximum length of a word in the generators so far computed.
    //!
    //! Every elements of the semigroup can be expressed as a product of the
    //! generators. The elements of the semigroup are enumerated in the
    //! short-lex order induced by the order of the generators (as passed to
    //! FroidurePin::FroidurePin).  This member function returns the length of
    //! the longest word in the generators that has so far been enumerated.
    size_t current_max_word_length() const noexcept override;

    //! Returns the degree of any and all elements in the semigroup.
    size_t degree() const noexcept override;

    //! Returns the number of generators of the semigroup.
    size_t nr_generators() const noexcept override;

    //! Returns a const_reference to the generator with index \p pos.
    //!
    //! If \p pos is not less than FroidurePin::nr_generators(), a
    //! LibsemigroupsException will be thrown. Note that
    //! FroidurePin::generator(pos)
    //! is in general in general not in position \p pos in the semigroup, i.e.
    //! is not equal to FroidurePin::at(pos).
    const_reference generator(letter_type) const;

    //! Returns the position of the element \p x in the semigroup if it is
    //! already known to belong to the semigroup.
    //!
    //! This member function finds the position of the element \p x in the
    //! semigroup if it is already known to belong to the semigroup, and
    //! libsemigroups::UNDEFINED if not. If the semigroup is
    //! not fully enumerated, then this member function may return
    //! libsemigroups::UNDEFINED when \p x is in the semigroup,
    //! but this is not yet known.
    //!
    //! \sa FroidurePin::position and FroidurePin::sorted_position.
    element_index_type current_position(const_reference x) const;

    //! Returns the number of elements in the semigroup that have been
    //! enumerated so far.
    //!
    //! This is only the actual size of the semigroup if the semigroup is fully
    //! enumerated.
    size_t current_size() const noexcept override;

    //! Returns the number of relations in the presentation for the semigroup
    //! that have been found so far.
    //!
    //! This is only the actual number of relations in a presentation defining
    //! the semigroup if the semigroup is fully enumerated.
    size_t current_nr_rules() const noexcept override;

    //! Returns the position of the prefix of the element \c x in position
    //! \p pos (of the semigroup) of length one less than the length of \c x.
    //!
    //! The parameter \p pos must be a valid position of an already enumerated
    //! element of the semigroup, or a LibsemigroupsException will be thrown.
    element_index_type prefix(element_index_type pos) const override;

    //! Returns the position of the suffix of the element \c x in position
    //! \p pos (of the semigroup) of length one less than the length of \c x.
    //!
    //! The parameter \p pos must be a valid position of an already enumerated
    //! element of the semigroup, or a LibsemigroupsException will be thrown.
    element_index_type suffix(element_index_type pos) const override;

    //! Returns the first letter of the element in position \p pos.
    //!
    //! This member function returns the first letter of the element in position
    //! \p pos of the semigroup, which is the index of the generator
    //! corresponding to the first letter of the element.
    //!
    //! Note that FroidurePin::generator[FroidurePin::first_letter(\c pos)] is
    //! only equal to FroidurePin::at(FroidurePin::first_letter(\c pos)) if
    //! there are no duplicate generators.
    //!
    //! The parameter \p pos must be a valid position of an already enumerated
    //! element of the semigroup, or a LibsemigroupsException will be thrown.
    letter_type first_letter(element_index_type pos) const override;

    //! Returns the last letter of the element in position \p pos.
    //!
    //! This member function returns the final letter of the element in position
    //! \p pos of the semigroup, which is the index of the generator
    //! corresponding to the final letter of the element.
    //!
    //! Note that FroidurePin::generator[FroidurePin::final_letter(\c pos)] is
    //! only equal to FroidurePin::at(FroidurePin::final_letter(\c pos)) if
    //! there are no duplicate generators.
    //!
    //! The parameter \p pos must be a valid position of an already enumerated
    //! element of the semigroup, or a LibsemigroupsException will be thrown.
    letter_type final_letter(element_index_type pos) const override;

    //! Returns the length of the element in position \c pos of the semigroup.
    //!
    //! The parameter \p pos must be a valid position of an already enumerated
    //! element of the semigroup, or a LibsemigroupsException will be thrown.
    //! This member function causes no enumeration of the semigroup.
    size_t length_const(element_index_type) const override;

    //! Returns the length of the element in position \c pos of the semigroup.
    //!
    //! The parameter \p pos must be a valid position of an element of the
    //! semigroup, or a LibsemigroupsException will be thrown.
    size_t length_non_const(element_index_type) override;

    //! Returns the position in \c this of the product of \c this->at(i) and
    //! \c this->at(j) by following a path in the Cayley graph.
    //!
    //! The values \p i and \p j must be less than FroidurePin::current_size, or
    //! a LibsemigroupsException will be thrown.
    //! This member function returns the position
    //! FroidurePin::element_index_type in the semigroup of the product of \c
    //! this->at(i) and \c this->at(j) elements by following the path in the
    //! right Cayley graph from \p i labelled by the word \c
    //! this->minimal_factorisation(j) or, if this->minimal_factorisation(i) is
    //! shorter, by following the path in the left Cayley graph from \p j
    //! labelled by this->minimal_factorisation(i).
    element_index_type product_by_reduction(element_index_type,
                                            element_index_type) const override;

    //! Returns the position in \c this of the product of \c this->at(i) and
    //! \c this->at(j).
    //!
    //! The values \p i and \p j must be less than FroidurePin::current_size, or
    //! a LibsemigroupsException will be thrown.
    //! This member function either:
    //!
    //! * follows the path in the right or left Cayley graph from \p i to \p j,
    //!   whichever is shorter using FroidurePin::product_by_reduction or
    //!
    //! * multiplies the elements in postions \p i and \p j together;
    //!
    //! whichever is better. The member function used is determined by comparing
    //! the output of the call operator of FroidurePin::Complexity and the
    //! FroidurePin::length_const of \p i and \p j.
    //!
    //! For example, if the complexity of the multiplication is linear and \c
    //! this is a semigroup of transformations of degree 20, and the shortest
    //! paths in the left and right Cayley graphs from \p i to \p j are of
    //! length 100 and 1131, then it better to just multiply the
    //! transformations together.
    element_index_type fast_product(element_index_type,
                                    element_index_type) const override;

    //! Returns the position in \c this of the generator with index \p i.
    //!
    //! If \p i is not a valid generator index, a LibsemigroupsException will
    //! be thrown. In many cases \p letter_to_pos(i) will equal \p i, examples
    //! of when this will not be the case are:
    //!
    //! * there are duplicate generators;
    //!
    //! * FroidurePin::add_generators was called after the semigroup was already
    //! partially enumerated.
    element_index_type letter_to_pos(letter_type) const override;

    //! Returns the total number of idempotents in the semigroup.
    //!
    //! This member function involves fully enumerating the semigroup, if it is
    //! not already fully enumerated.  The value of the positions, and number,
    //! of idempotents is stored after they are first computed.
    size_t nr_idempotents() override;

    //! Returns \c true if the element in position \p pos is an idempotent
    //! and \c false if it is not.
    //!
    //! The parameter \p pos must be a valid position of an element of the
    //! semigroup, or a LibsemigroupsException will be thrown.
    //! This member function involves fully enumerating the semigroup, if it is
    //! not already fully enumerated.
    bool is_idempotent(element_index_type) override;

    //! Returns the total number of relations in the presentation defining the
    //! semigroup.
    //!
    //! \sa FroidurePin::next_relation.
    size_t nr_rules() override;

    //! Requests that the capacity (i.e. number of elements) of the semigroup
    //! be at least enough to contain n elements.
    //!
    //! The parameter \p n is also used to initialise certain data members. If
    //! you know a good upper bound for the size of your semigroup, then it is
    //! a good idea to call this member function with that upper bound as an
    //! argument; this can significantly improve the performance of the
    //! FroidurePin::run member function, and consequently every other
    //! member function too.
    void reserve(size_t) override;

    //! Returns the size of the semigroup.
    size_t size() override;

    //! Returns \c true if \p x is an element of \c this and \c false if it is
    //! not.
    //!
    //! This member function can be used to check if the element \p x is an
    //! element of the semigroup. The semigroup is enumerated in batches until
    //! \p x is found or the semigroup is fully enumerated but \p x was not
    //! found (see FroidurePin::batch_size).
    bool contains(const_reference);

    //! Returns the position of \p x in \c this, or libsemigroups::UNDEFINED if
    //! \p x is not an element of \c this.
    //!
    //! This member function can be used to find the
    //! FroidurePin::element_index_type position of the element \p x if it
    //! belongs to the semigroup. The semigroup is enumerated in batches until
    //! \p x is found or the semigroup is fully enumerated but \p x was not
    //! found (see FroidurePin::batch_size(size_t)).
    //!
    //! \throws LibsemigroupsException This function throws a
    //! LibsemigroupsException if the argument is not an element of the
    //! semigroup represented by \c this. Since the semigroup may be enumerated
    //! (fully or partially) when this function is called, it only offers the
    //! basic exception guarantee (the object is in a valid state after any
    //! call to this function but not necessarily the same state as it was
    //! before the call).
    element_index_type position(const_reference);

    //! Returns the position of \p x in the sorted array of elements of the
    //! semigroup, or libsemigroups::UNDEFINED if \p x is not an element of \c
    //! this.
    element_index_type sorted_position(const_reference);

    //! Returns the position of \c this->at(pos) in the sorted array of
    //! elements of the semigroup, or libsemigroups::UNDEFINED if \p pos is
    //! greater than the size of the semigroup.
    element_index_type position_to_sorted_position(element_index_type) override;

    //! Returns  the element of the semigroup in position \p pos, or a
    //! \c nullptr if there is no such element.
    //!
    //! This member function attempts to enumerate the semigroup until at least
    //! \c pos + 1 elements have been found. If \p pos is greater than
    //! FroidurePin::size, then this member function returns \c nullptr.
    const_reference at(element_index_type);

    //! Returns the element of the semigroup in position \p pos.
    //!
    //! This member function performs no checks on its argument, and performs no
    //! enumeration of the semigroup.
    const_reference operator[](element_index_type) const;

    //! Returns the element of the semigroup in position \p pos of the sorted
    //! array of elements, or \c nullptr in \p pos is not valid (i.e. too big).
    //!
    //! This member function fully enumerates the semigroup.
    const_reference sorted_at(element_index_type);

    //! Returns the index of the product of the element in position \p i with
    //! the generator with index \p j.
    //!
    //! This member function fully enumerates the semigroup.
    element_index_type right(element_index_type i, letter_type j) override;

    //! Returns a const reference to the right Cayley graph of \c this.
    //!
    //! \returns A const reference to cayley_graph_type.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! \f$O(|S||A|)\f$ where \f$S\f$ is the semigroup represented by \c this
    //! and \f$A\f$ is the set of generators used to define \c this.
    //!
    //! \par Parameters
    //! None.
    cayley_graph_type const& right_cayley_graph() override;

    //! Returns the index of the product of the generator with index \p j and
    //! the element in position \p i.
    //!
    //! This member function fully enumerates the semigroup.
    element_index_type left(element_index_type i, letter_type j) override;

    //! Returns a const reference to the left Cayley graph of \c this.
    //!
    //! \returns A const reference to cayley_graph_type.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! \f$O(|S||A|)\f$ where \f$S\f$ is the semigroup represented by \c this
    //! and \f$A\f$ is the set of generators used to define \c this.
    //!
    //! \par Parameters
    //! None.
    cayley_graph_type const& left_cayley_graph() override;

    //! Changes \p word in-place to contain a minimal word with respect to the
    //! short-lex ordering in the generators equal to the \p pos element of
    //! the semigroup.
    //!
    //! If \p pos is less than the size of this semigroup, then this member
    //! function changes its first parameter \p word in-place by first clearing
    //! it and then to contain a minimal factorization of the element in
    //! position \p pos of the semigroup with respect to the generators of the
    //! semigroup. This member function enumerates the semigroup until at least
    //! the \p pos element is known. If \p pos is greater than the size of the
    //! semigroup, then a LibsemigroupsException is thrown.
    void minimal_factorisation(word_type&, element_index_type) override;

    //! Returns a minimal libsemigroups::word_type which evaluates to the
    //! TElementType in position \p pos of \c this.
    //!
    //! This is the same as the two-argument member function for
    //! FroidurePin::minimal_factorisation, but it returns a pointer to the
    //! factorisation instead of modifying an argument in-place.
    //! If \p pos is greater than the size of the semigroup, then a
    //! LibsemigroupsException is thrown.
    word_type minimal_factorisation(element_index_type) override;

    //! Returns a minimal libsemigroups::word_type which evaluates to \p x.
    //!
    //! This is the same as the member function taking a
    //! FroidurePin::element_index_type, but it factorises an TElementType
    //! instead of using the position of an element. If \p pos is greater than
    //! the size of the semigroup, then a LibsemigroupsException is thrown.
    word_type minimal_factorisation(const_reference);

    //! Changes \p word in-place to contain a word in the generators equal to
    //! the \p pos element of the semigroup.
    //!
    //! The key difference between this member function and
    //! FroidurePin::minimal_factorisation(word_type& word, element_index_type
    //! pos), is that the resulting factorisation may not be minimal. If \p pos
    //! is greater than the size of the semigroup, then a
    //! LibsemigroupsException is thrown.
    void factorisation(word_type& word, element_index_type pos) override;

    //! Returns a libsemigroups::word_type which evaluates to the TElementType
    //! in position \p pos of \c this.
    //!
    //! The key difference between this member function and
    //! FroidurePin::minimal_factorisation(element_index_type pos), is that the
    //! resulting factorisation may not be minimal.
    //! If \p pos is greater than the size of the semigroup, then a
    //! LibsemigroupsException is thrown.
    word_type factorisation(element_index_type pos) override;

    //! Returns a libsemigroups::word_type which evaluates to \p x.
    //!
    //! The key difference between this member function and
    //! FroidurePin::minimal_factorisation(const_reference x), is that the
    //! resulting factorisation may not be minimal.
    //! If \p pos is greater than the size of the semigroup, then a
    //! LibsemigroupsException is thrown.
    word_type factorisation(const_reference);

    //! This member function resets FroidurePin::next_relation so that when it
    //! is next called the resulting relation is the first one.
    //!
    //! After a call to this function, the next call to
    //! FroidurePin::next_relation will return the first relation of the
    //! presentation defining the semigroup.
    void reset_next_relation() noexcept override;

    //! This member function changes \p relation in-place to contain the next
    //! relation of the presentation defining \c this.
    //!
    //! This member function changes \p relation in-place so that one of the
    //! following holds:
    //!
    //! * \p relation is a vector consisting of a libsemigroups::letter_type and
    //! a libsemigroups::letter_type such that FroidurePin::generator(\c
    //! relation[\c 0]) == FroidurePin::generator(\c relation[\c 1]), i.e. if
    //! the semigroup was defined with duplicate generators;
    //!
    //! * \p relation is a vector consisting of a
    //! libsemigroups::element_index_type, libsemigroups::letter_type, and
    //! libsemigroups::element_index_type such that
    //! \code{.cpp}
    //!   this[relation[0]] * FroidurePin::generator(relation[1]) ==
    //!   this[relation[2]]
    //! \endcode
    //!
    //! * \p relation is empty if there are no more relations.
    //!
    //! FroidurePin::next_relation is guaranteed to output all relations of
    //! length 2 before any relations of length 3. If called repeatedly after
    //! FroidurePin::reset_next_relation, and until relation is empty, the
    //! values placed in \p relation correspond to a length-reducing confluent
    //! rewriting system that defines the semigroup.
    //!
    //! This member function can be used in conjunction with
    //! FroidurePin::factorisation to obtain a presentation defining the
    //! semigroup.
    //!
    //! \sa FroidurePin::reset_next_relation.
    void next_relation(word_type& relation) override;

    //! Enumerate the semigroup until \p limit elements are found.
    //!
    //! If the semigroup is already fully enumerated, or the number of elements
    //! previously enumerated exceeds \p limit, then calling this member
    //! function does nothing. Otherwise, run attempts to find at least
    //! the maximum of \p limit and FroidurePin::batch_size elements of the
    //! semigroup. If \p killed is set to \c true (usually by another process),
    //! then the enumeration is terminated as soon as possible.  It is possible
    //! to resume enumeration at some later point after any call to this member
    //! function.
    //!
    //! If the semigroup is fully enumerated, then it knows its left and right
    //! Cayley graphs, and a minimal factorisation of every element (in terms of
    //! its generating set).  All of the elements are stored in memory until the
    //! object is destroyed.
    //!
    //! The parameter \p limit defaults to FroidurePin::LIMIT_MAX.
    void enumerate(size_t) override;

    //! Add copies of the generators \p coll to the generators of \c this.
    //!
    //! This member function can be used to add new generators to the existing
    //! semigroup in such a way that any previously enumerated data is preserved
    //! and not recomputed, or copied. This can be faster than recomputing the
    //! semigroup generated by the old generators and the new generators in the
    //! parameter \p coll.
    //!
    //! This member function changes the semigroup in-place, thereby
    //! invalidating possibly previously known data about the semigroup, such as
    //! the left or right Cayley graphs, number of idempotents, and so on.
    //!
    //! Every generator in \p coll is added regardless of whether or not it is
    //! already a generator or element of the semigroup (it may belong to the
    //! semigroup but just not be known to belong). If \p coll is empty, then
    //! the semigroup is left unchanged. The order the generators is added is
    //! also the order they occur in the parameter \p coll.
    //!
    //! The semigroup is returned in a state where all of the previously
    //! enumerated elements which had been multiplied by all of the old
    //! generators, have now been multiplied by all of the old and new
    //! generators. This means that after this member function is called the
    //! semigroup might contain many more elements than before (whether it is
    //! fully enumerating or not).  It can also be the case that the new
    //! generators are the only new elements, unlike, say, in the case of
    //! non-trivial groups.
    //!
    //! The elements of the argument \p coll are copied into the semigroup, and
    //! should be deleted by the caller.
    //! If an element in \p coll has a degree different to \c this->degree(), a
    //! LibsemigroupsException will be thrown.
    void add_generator(element_type const&);

    //! Add copies of the generators \p coll to the generators of \c this.
    //!
    //! See FroidurePin::add_generators for more details.
    template <typename TCollection>
    void add_generators(TCollection const&);

    //! Add copies of the generators \p coll to the generators of \c this.
    //!
    //! See FroidurePin::add_generators for more details.
    void add_generators(std::initializer_list<const_element_type>);

    //! Returns a new semigroup generated by \c this and \p coll.
    //!
    //! This member function is equivalent to copying \c this using
    //! FroidurePin::FroidurePin(const FroidurePin& copy) and then calling
    //! FroidurePin::add_generators on the copy, but this member function avoids
    //! copying the parts of \c this that are immediately invalidated by
    //! FroidurePin::add_generators.
    //!
    //! The elements the argument \p coll are copied into the semigroup, and
    //! should be deleted by the caller.  If an element in \p coll has a degree
    //! different to \c this->degree(), a LibsemigroupsException will be
    //! thrown.
    template <typename TCollection>
    FroidurePin* copy_add_generators(TCollection const&) const;

    //! Add copies of the non-redundant generators in \p coll to the generators
    //! of \c this.
    //!
    //! This member function can be used to add new generators to an existing
    //! semigroup in such a way that any previously enumerated data is preserved
    //! and not recomputed, or copied. This can be faster than recomputing the
    //! semigroup generated by the old generators and the new in \p coll.
    //!
    //! This member function differs from FroidurePin::add_generators in that it
    //! tries to add the new generators one by one, and only adds those
    //! generators that are not products of existing generators (including any
    //! new generators from \p coll that were added before). The generators are
    //! added in the order they occur in \p coll.
    //!
    //! This member function changes the semigroup in-place, thereby
    //! invalidating possibly previously known data about the semigroup, such as
    //! the left or right Cayley graphs, or number of idempotents, for example.
    //!
    //! The elements the parameter \p coll are copied into the semigroup, and
    //! should be deleted by the caller.
    //! If an element in \p coll has a degree different to \c this->degree(), a
    //! LibsemigroupsException will be thrown.
    template <typename TCollection>
    void closure(TCollection const& coll);

    //! Add copies of the non-redundant generators in \p coll to the
    //! generators of \c this.
    //!
    //! See FroidurePin::closure for more details.
    void closure(std::initializer_list<const_element_type>);

    //! Returns a new semigroup generated by \c this and copies of the
    //! non-redundant elements of \p coll.
    //!
    //! This member function is equivalent to copying \c this and then calling
    //! FroidurePin::closure on the copy with \p coll, but this member function
    //! avoids copying the parts of \c this that are immediately invalidated by
    //! FroidurePin::closure.
    //!
    //! The elements the argument \p coll are copied into the semigroup, and
    //! should be deleted by the caller.
    //! If an element in \p coll has a degree different to \c this->degree(), a
    //! LibsemigroupsException will be thrown.
    template <typename TCollection>
    FroidurePin* copy_closure(TCollection const&);

    //! \returns
    //! \c true if the semigroup represented by \c this contains
    //! FroidurePin::One()(), and \c false if not.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! \f$O(|S||A|)\f$ where \f$S\f$ is the semigroup represented by \c this
    //! and \f$A\f$ is the set of generators used to define \c this.
    //!
    //! \par Parameters
    //! (None).
    bool is_monoid() override;

    tril is_finite() override;

   private:
    ////////////////////////////////////////////////////////////////////////
    // FroidurePin - validation member functions - private
    ////////////////////////////////////////////////////////////////////////

    void validate_element_index(element_index_type) const;
    void validate_letter_index(letter_type) const;

    ////////////////////////////////////////////////////////////////////////
    // FroidurePin - enumeration member functions - private
    ////////////////////////////////////////////////////////////////////////

    inline void expand(size_type);
    inline void
    is_one(internal_const_element_type x, element_index_type) noexcept(
        std::is_nothrow_default_constructible<InternalEqualTo>::
            value&& noexcept(std::declval<InternalEqualTo>()(x, x)));

    void copy_gens();
    void closure_update(element_index_type,
                        letter_type,
                        letter_type,
                        element_index_type,
                        size_type,
                        size_t const&,
                        std::vector<bool>&);

    ////////////////////////////////////////////////////////////////////////
    // FroidurePin - initialisation member functions - private
    ////////////////////////////////////////////////////////////////////////

    using internal_idempotent_pair
        = std::pair<internal_element_type, element_index_type>;

    void init_sorted();
    void init_idempotents();
    void idempotents(enumerate_index_type const,
                     enumerate_index_type const,
                     enumerate_index_type const,
                     std::vector<internal_idempotent_pair>&);

    ////////////////////////////////////////////////////////////////////////
    // FroidurePin - iterators - private
    ////////////////////////////////////////////////////////////////////////

    // Forward declarations - implemented in froidure-pin-impl.hpp
    struct DerefPairFirst;
    struct AddressOfPairFirst;
    struct IteratorPairFirstTraits;

    // A type for const iterators through (element, index) pairs of \c this.
    using const_iterator_pair_first
        = detail::ConstIteratorStateless<IteratorPairFirstTraits>;

    // A type for const reverse iterators through (element_type,
    // element_index_type) pairs of this.
    using const_reverse_iterator_pair_first
        = std::reverse_iterator<const_iterator_pair_first>;

   public:
    ////////////////////////////////////////////////////////////////////////
    // FroidurePin - iterators - public
    ////////////////////////////////////////////////////////////////////////
    //! A type for const iterators through the elements of \c this, in the
    //! order they were enumerated (i.e. in short-lex order of the minimum word
    //! in the generators of \c this equal to any given element).
    //!
    //! \sa const_reverse_iterator.
    using const_iterator
        = detail::BruidhinnConstIterator<element_type,
                                         std::vector<internal_element_type>>;

    //! A type for const reverse iterators through the elements of \c this, in
    //! the reverse order of enumeration.
    //!
    //! \sa const_iterator.
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    //! A type for const iterators through the elements of \c this, sorted
    //! according to TTraits::less.
    //!
    //! \sa const_reverse_iterator_sorted.
    using const_iterator_sorted = const_iterator_pair_first;

    //! A type for const reverse iterators through the elements of \c this,
    //! sorted according to TTraits::less.
    //!
    //! \sa const_iterator_sorted.
    using const_reverse_iterator_sorted = const_reverse_iterator_pair_first;

    //! A type for const iterators through the idempotents of \c this, in
    //! short-lex order.
    //!
    //! \sa const_iterator.
    using const_iterator_idempotents = const_iterator_pair_first;

    //! A type for reverse const iterators through the idempotents of \c this,
    //! in short-lex order.
    //!
    //! \sa const_iterator_idempotents.
    using const_reverse_iterator_idempotents
        = const_reverse_iterator_pair_first;

    //! Returns a const iterator pointing to the first element of the
    //! semigroup.
    //!
    //! This member function does not perform any enumeration of the semigroup,
    //! the iterator returned may be invalidated by any call to a non-const
    //! member function of the FroidurePin class.
    const_iterator cbegin() const;

    //! Returns a const iterator pointing to the first element of the
    //! semigroup.
    //!
    //! This member function does not perform any enumeration of the semigroup,
    //! the iterator returned may be invalidated by any call to a non-const
    //! member function of the FroidurePin class.
    const_iterator begin() const;

    //! Returns a const iterator pointing to one past the last currently known
    //! element of the semigroup.
    //!
    //! This member function does not perform any enumeration of the semigroup,
    //! the iterator returned may be invalidated by any call to a non-const
    //! member function of the FroidurePin class.
    const_iterator cend() const;

    //! Returns a const iterator pointing to one past the last currently known
    //! element of the semigroup.
    //!
    //! This member function does not perform any enumeration of the semigroup,
    //! the iterator returned may be invalidated by any call to a non-const
    //! member function of the FroidurePin class.
    const_iterator end() const;

    //! Returns a const reverse iterator pointing to the last currently known
    //! element of the semigroup.
    //!
    //! This member function does not perform any enumeration of the semigroup,
    //! the iterator returned may be invalidated by any call to a non-const
    //! member function of the FroidurePin class.
    const_reverse_iterator crbegin() const;

    //! Returns a const reverse iterator pointing to one before the first
    //! element of the semigroup.
    //!
    //! This member function does not perform any enumeration of the semigroup,
    //! the iterator returned may be invalidated by any call to a non-const
    //! member function of the FroidurePin class.
    const_reverse_iterator crend() const;

    //! Returns a const iterator pointing to the first element of the semigroup
    //! when the elements are sorted by FroidurePin::Less.
    //!
    //! This member function fully enumerates the semigroup, the returned
    //! iterator returned may be invalidated by any call to a non-const member
    //! function of the FroidurePin class.
    const_iterator_sorted cbegin_sorted();

    //! Returns a const iterator pointing to one past the last element of the
    //! semigroup when the elements are sorted by FroidurePin::Less.
    //!
    //! This member function fully enumerates the semigroup, the returned
    //! iterator returned may be invalidated by any call to a non-const member
    //! function of the FroidurePin class.
    const_iterator_sorted cend_sorted();

    //! Returns a const iterator pointing to the last element of the semigroup
    //! when the elements are sorted by FroidurePin::Less.
    //!
    //! This member function fully enumerates the semigroup, the returned
    //! iterator returned may be invalidated by any call to a non-const member
    //! function of the FroidurePin class.
    const_reverse_iterator_sorted crbegin_sorted();

    //! Returns a const iterator pointing to one before the first element of
    //! the semigroup when the elements are sorted by FroidurePin::Less.
    //!
    //! This member function fully enumerates the semigroup, the returned
    //! iterator returned may be invalidated by any call to a non-const member
    //! function of the FroidurePin class.
    const_reverse_iterator_sorted crend_sorted();

    //! Returns a const iterator pointing at the first idempotent in the
    //! semigroup.
    //!
    //! If the returned iterator is incremented, then it points to the second
    //! idempotent in the semigroup (if it exists), and every subsequent
    //! increment points to the next idempotent.
    //!
    //! This member function involves fully enumerating the semigroup, if it is
    //! not already fully enumerated.
    const_iterator_idempotents cbegin_idempotents();

    //! Returns a const iterator referring to past the end of the last
    //! idempotent in the semigroup.
    //!
    //! This member function involves fully enumerating the semigroup, if it is
    //! not already fully enumerated.
    const_iterator_idempotents cend_idempotents();

   private:
    void run_impl() override;
    bool finished_impl() const override;

    ////////////////////////////////////////////////////////////////////////
    // FroidurePin - data - private
    ////////////////////////////////////////////////////////////////////////

    size_t                                           _degree;
    std::vector<std::pair<letter_type, letter_type>> _duplicate_gens;
    std::vector<internal_element_type>               _elements;
    std::vector<element_index_type>                  _enumerate_order;
    std::vector<letter_type>                         _final;
    std::vector<letter_type>                         _first;
    bool                                             _found_one;
    std::vector<internal_element_type>               _gens;
    internal_element_type                            _id;
    std::vector<internal_idempotent_pair>            _idempotents;
    bool                                             _idempotents_found;
    std::vector<bool>                                _is_idempotent;
    cayley_graph_type                                _left;
    std::vector<size_type>                           _length;
    std::vector<enumerate_index_type>                _lenindex;
    std::vector<element_index_type>                  _letter_to_pos;
    // #ifdef LIBSEMIGROUPS_DENSEHASHMAP
    //     google::dense_hash_map<internal_const_element_type,
    //                            element_index_type,
    //                            InternalHash,
    //                            InternalEqualTo>
    //         _map;
    // #else
    std::unordered_map<internal_const_element_type,
                       element_index_type,
                       InternalHash,
                       InternalEqualTo>
        _map;
    // #endif
    mutable std::mutex              _mtx;
    size_type                       _nr;
    letter_type                     _nrgens;
    size_t                          _nr_rules;
    enumerate_index_type            _pos;
    element_index_type              _pos_one;
    std::vector<element_index_type> _prefix;
    detail::DynamicArray2<bool>     _reduced;
    letter_type                     _relation_gen;
    enumerate_index_type            _relation_pos;
    cayley_graph_type               _right;
    std::vector<std::pair<internal_element_type, element_index_type>> _sorted;
    std::vector<element_index_type>                                   _suffix;
    mutable internal_element_type _tmp_product;
    size_t                        _wordlen;

#ifdef LIBSEMIGROUPS_VERBOSE
    size_t _nr_products;
#endif
  };
}  // namespace libsemigroups

// Include the implementation of the member functions for FroidurePin
#include "froidure-pin-impl.hpp"
#endif  // LIBSEMIGROUPS_INCLUDE_FROIDURE_PIN_HPP_
