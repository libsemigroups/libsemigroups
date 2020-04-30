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

#ifndef LIBSEMIGROUPS_FROIDURE_PIN_HPP_
#define LIBSEMIGROUPS_FROIDURE_PIN_HPP_

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
#include "deprecated.hpp"         // for LIBSEMIGROUPS_DEPRECATED
#include "froidure-pin-base.hpp"  // for FroidurePinBase, FroidurePinBase::s...
#include "iterator.hpp"           // for ConstIteratorStateless
#include "stl.hpp"                // for EqualTo, Hash
#include "types.hpp"              // for letter_type, word_type

//! Namespace for everything in the libsemigroups library.
namespace libsemigroups {

  //! Defined in ``froidure-pin.hpp``.
  //!
  //! This is a traits class for use with FroidurePin.
  //!
  //! \sa FroidurePinBase and FroidurePin.
  template <typename TElementType, typename TStateType = void>
  struct FroidurePinTraits {
    // Require to get the value_type from detail::BruidhinnTraits to remove
    // pointer to const.
    //! The type of the elements of a FroidurePin instance with const removed,
    //! and if TElementType is a pointer to const, then the second const is
    //! also removed.
    using element_type =
        typename detail::BruidhinnTraits<TElementType>::value_type;

    //! The type of any state that it might be required to store inside the
    //! FroidurePin instance
    using state_type = TStateType;

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
    using enumerate_index_type = FroidurePinBase::enumerate_index_type;

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

    //! The type of a reference to an element of the semigroup represented by
    //! \c this.
    using reference = typename detail::BruidhinnTraits<TElementType>::reference;

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

    //! No doc
    using state_type = typename TTraits::state_type;

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
    template <typename T>
    struct IsState final
        : std::integral_constant<bool,
                                 !std::is_void<T>::value
                                     && std::is_same<state_type, T>::value> {};

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

    using map_type = std::unordered_map<internal_const_element_type,
                                        element_index_type,
                                        InternalHash,
                                        InternalEqualTo>;

    struct InternalProduct {
      template <typename SFINAE = void>
      auto operator()(reference       xy,
                      const_reference x,
                      const_reference y,
                      void*,
                      size_t tid = 0) ->
          typename std::enable_if<std::is_void<state_type>::value,
                                  SFINAE>::type {
        Product()(xy, x, y, tid);
      }

      template <typename SFINAE = void>
      auto operator()(reference       xy,
                      const_reference x,
                      const_reference y,
                      state_type*     stt,
                      size_t          tid = 0) ->
          typename std::enable_if<!std::is_void<state_type>::value,
                                  SFINAE>::type {
        Product()(xy, x, y, stt, tid);
      }
    };

   public:
    ////////////////////////////////////////////////////////////////////////
    // FroidurePin - constructors + destructor - public
    ////////////////////////////////////////////////////////////////////////

    //! Default constructor.
    //!
    //! Constructs a FroidurePin instance with no generators.
    //!
    //! \sa add_generators and add_generator.
    FroidurePin();

    //! No doc
    template <typename T,
              typename SFINAE
              = typename std::enable_if<IsState<T>::value, T>::type>
    explicit FroidurePin(std::shared_ptr<T> stt) : FroidurePin() {
      _state = stt;
    }

    //! No doc
    template <typename T,
              typename SFINAE
              = typename std::enable_if<IsState<T>::value, T>::type>
    explicit FroidurePin(T const& stt)
        : FroidurePin(std::make_shared<state_type>(stt)) {}

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
    // FIXME what to do about this
    bool equal_to(word_type const&, word_type const&) const override;

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

    //! Returns a minimal libsemigroups::word_type which evaluates to \p x.
    //!
    //! This is the same as the member function taking a
    //! FroidurePin::element_index_type, but it factorises an TElementType
    //! instead of using the position of an element. If \p pos is greater than
    //! the size of the semigroup, then a LibsemigroupsException is thrown.
    word_type minimal_factorisation(const_reference);

    //! \copydoc FroidurePinBase::factorisation
    using FroidurePinBase::factorisation;

    //! \copydoc FroidurePinBase::minimal_factorisation
    using FroidurePinBase::minimal_factorisation;

    //! Returns a libsemigroups::word_type which evaluates to \p x.
    //!
    //! The key difference between this member function and
    //! FroidurePin::minimal_factorisation(const_reference x), is that the
    //! resulting factorisation may not be minimal.
    //! If \p pos is greater than the size of the semigroup, then a
    //! LibsemigroupsException is thrown.
    word_type factorisation(const_reference);

    //! Returns tril::TRUE if the semigroup represented by \c this is finite,
    //! tril::FALSE if it is infinite, and tril::unknown if it is not known.
    tril is_finite() const override {
      return tril::TRUE;
    }

    //! Add a copy of the generators \p x to the generators of \c this.
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
    //! fully enumerating or not).
    void add_generator(const_reference x);

    //! Add copies of the generators \p coll to the generators of \c this.
    //!
    //! See FroidurePin::add_generator for more details.
    template <typename TCollection>
    void add_generators(TCollection const& coll);

    //! Add copies of the generators \p coll to the generators of \c this.
    //!
    //! See FroidurePin::add_generator for more details.
    void add_generators(std::initializer_list<const_element_type> coll);

    //! Add copies of the generators in the range \p first to \p last to \c
    //! this.
    //!
    //! See FroidurePin::add_generator for more details.
    template <typename T>
    void add_generators(T const& first, T const& last);

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
    FroidurePin* copy_add_generators(TCollection const& coll) const;

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
    FroidurePin* copy_closure(TCollection const& coll);

    //! No doc
    std::shared_ptr<state_type> state() {
      return _state;
    }

   private:
    ////////////////////////////////////////////////////////////////////////
    // FroidurePin - validation member functions - private
    ////////////////////////////////////////////////////////////////////////

    void validate_element(const_reference) const;

    template <typename T>
    void validate_element_collection(T const&, T const&) const;

    ////////////////////////////////////////////////////////////////////////
    // FroidurePin - enumeration member functions - private
    ////////////////////////////////////////////////////////////////////////

    inline void expand(size_type);
    inline void
    is_one(internal_const_element_type x, element_index_type) noexcept(
        std::is_nothrow_default_constructible<InternalEqualTo>::
            value&& noexcept(std::declval<InternalEqualTo>()(x, x)));

    void copy_generators_from_elements(size_t);
    void closure_update(element_index_type,
                        letter_type,
                        letter_type,
                        element_index_type,
                        size_type,
                        size_t const&,
                        std::vector<bool>&,
                        state_type*);

    void init_degree(const_reference);

    template <typename T>
    void add_generators_before_start(T const&, T const&);

    template <typename T>
    void add_generators_after_start(T const&, T const&);

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

   public:
    // Forward declarations - implemented in froidure-pin-impl.hpp
    //! No doc
    struct DerefPairFirst;

    //! No doc
    struct AddressOfPairFirst;

    //! No doc
    struct IteratorPairFirstTraits;

   private:
    ////////////////////////////////////////////////////////////////////////
    // FroidurePin - iterators - private
    ////////////////////////////////////////////////////////////////////////

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

    std::vector<internal_element_type>    _elements;
    std::vector<internal_element_type>    _gens;
    internal_element_type                 _id;
    std::vector<internal_idempotent_pair> _idempotents;
    map_type                              _map;
    mutable std::mutex                    _mtx;
    std::vector<std::pair<internal_element_type, element_index_type>> _sorted;
    std::shared_ptr<state_type>                                       _state;
    mutable internal_element_type _tmp_product;

#ifdef LIBSEMIGROUPS_VERBOSE
    size_t _nr_products;
#endif
  };
}  // namespace libsemigroups

// Include the implementation of the member functions for FroidurePin
#include "froidure-pin-impl.hpp"
#endif  // LIBSEMIGROUPS_FROIDURE_PIN_HPP_
