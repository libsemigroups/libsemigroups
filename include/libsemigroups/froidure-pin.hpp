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
#include <memory>         // for shared_ptr, make_shared
#include <mutex>          // for mutex
#include <type_traits>    // for is_const, remove_pointer
#include <unordered_map>  // for unordered_map
#include <utility>        // for pair
#include <vector>         // for vector

#include "adapters.hpp"           // for Complexity, Degree, IncreaseDegree
#include "bruidhinn-traits.hpp"   // for detail::BruidhinnTraits
#include "containers.hpp"         // for DynamicArray2
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
  //! \tparam TElementType the type of the elements.
  //! \tparam TStateType the type of the state (if any, defaults to \c void,
  //! meaning none).
  //!
  //! \sa FroidurePinBase and FroidurePin.
  template <typename TElementType, typename TStateType = void>
  struct FroidurePinTraits {
    // Require to get the value_type from detail::BruidhinnTraits to remove
    // pointer to const.
    //! The type of the elements of a FroidurePin instance.
    //!
    //! This type has const removed, and if \c TElementType is a pointer to
    //! const, then the second const is also removed.
    using element_type =
        typename detail::BruidhinnTraits<TElementType>::value_type;

    //! The type of the state (if any).
    //!
    //! This type can be used to store some state that might be required in an
    //! FroidurePin instance.
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
  //! function is \ref run, which implements the
  //! [Froidure-Pin Algorithm].  If \ref run is invoked and
  //! \ref finished returns \c true, then the size, the left and right
  //! Cayley graphs are determined, and a confluent terminating presentation
  //! for the semigroup is known.
  //!
  //! \tparam TElementType the type of the elements in the represented
  //! semigroup
  //!
  //! \tparam TTraits a traits class holding various adapters used by the
  //! implementation (defaults to FroidurePinTraits).
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
  //!
  //! template <>
  //! struct Degree<int> {
  //!   constexpr size_t operator()(int) const noexcept {
  //!     return 0;
  //!   }
  //! };
  //!
  //! template <>
  //! struct IncreaseDegree<int> {
  //!   int operator()(int x) const noexcept {
  //!     return x;
  //!   }
  //! };
  //!
  //! template <>
  //! struct One<int> {
  //!   constexpr int operator()(int) const noexcept {
  //!     return 1;
  //!   }
  //! };
  //!
  //! template <>
  //! struct Product<int> {
  //!   void operator()(int& xy,
  //!                   int  x,
  //!                   int  y,
  //!                   size_t = 0) const noexcept {
  //!     xy = x * y;
  //!   }
  //! };
  //!
  //! FroidurePin<int> S({2});
  //! S.size();           // 32
  //! S.number_of_idempotents()  // 1
  //! *S.cbegin();        // 2
  //!
  //! FroidurePin<uint8_t> T({2, 3});
  //! T.size()                      // 130
  //! T.number_of_idempotents()     // 2
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

    //! Type of the elements.
    using element_type =
        typename detail::BruidhinnTraits<TElementType>::value_type;

    //! Alias for element_type.
    // This only really exists to allow the python bindings to compile with
    // gcc-6 + 7 (at least).
    using value_type = element_type;

    //! Type of const elements.
    using const_element_type =
        typename detail::BruidhinnTraits<TElementType>::const_value_type;

    //! Type of element const references.
    using const_reference =
        typename detail::BruidhinnTraits<TElementType>::const_reference;

    //! Type of element references.
    using reference = typename detail::BruidhinnTraits<TElementType>::reference;

    //! Type of element const pointers.
    using const_pointer =
        typename detail::BruidhinnTraits<TElementType>::const_pointer;

    //! \copydoc FroidurePinBase::size_type
    using size_type = FroidurePinBase::size_type;

    //! \copydoc FroidurePinBase::element_index_type
    using element_index_type = FroidurePinBase::element_index_type;

    //! \copydoc FroidurePinBase::cayley_graph_type
    using cayley_graph_type = FroidurePinBase::cayley_graph_type;

    //! Type of the state used for multiplication (if any).
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
                      size_t tid = 0)
          -> std::enable_if_t<std::is_void<state_type>::value, SFINAE> {
        Product()(xy, x, y, tid);
      }

      template <typename SFINAE = void>
      auto operator()(reference       xy,
                      const_reference x,
                      const_reference y,
                      state_type*     stt,
                      size_t          tid = 0)
          -> std::enable_if_t<!std::is_void<state_type>::value, SFINAE> {
        Product()(xy, x, y, stt, tid);
      }
    };

    template <typename T>
    using EnableIfIsState = std::enable_if_t<IsState<T>::value>;

   public:
    ////////////////////////////////////////////////////////////////////////
    // FroidurePin - constructors + destructor - public
    ////////////////////////////////////////////////////////////////////////

    //! Default constructor.
    //!
    //! Constructs a FroidurePin instance with no generators.
    //!
    //! \parameters
    //! (None)
    //!
    //! \sa add_generator and add_generators.
    FroidurePin();

    //! Construct from \shared_ptr to state.
    //!
    //! This function allows the construction of a FroidurePin instance with
    //! stated given by the parameter \p stt. This constructor only exists if
    //! \ref state_type is not \c void. This is used when the elements require
    //! some shared state to define their multiplication, such as, for example
    //! an instance of fpsemigroup::KnuthBendix or congruence::ToddCoxeter.
    //!
    //! \param stt a \shared_ptr to a state object.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    template <typename T, typename = EnableIfIsState<T>>
    explicit FroidurePin(std::shared_ptr<T> stt) : FroidurePin() {
      _state = stt;
    }

    //! Construct from const reference to state.
    //!
    //! This function allows the construction of a FroidurePin instance with
    //! stated given by the parameter \p stt. This constructor only exists if
    //! \ref state_type is not \c void. This is used when the elements require
    //! some shared state to define their multiplication, such as, for example
    //! an instance of fpsemigroup::KnuthBendix or congruence::ToddCoxeter.
    //!
    //! \param stt a const reference to a state object.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \warning
    //! The parameter \p stt is copied, which might be expensive, use
    //! a \shared_ptr to avoid the copy.
    template <typename T, typename = EnableIfIsState<T>>
    explicit FroidurePin(T const& stt)
        : FroidurePin(std::make_shared<state_type>(stt)) {}

    //! Deleted.
    FroidurePin& operator=(FroidurePin const&) = delete;

    //! Deleted.
    FroidurePin& operator=(FroidurePin&&) = delete;

    //! Construct from generators.
    //!
    //! This function constructs a FroidurePin instance generated by the
    //! specified container of generators.  There can be duplicate generators
    //! and although they do not count as distinct elements, they do count as
    //! distinct generators.  In other words, the generators are precisely (a
    //! copy of) \p gens in the same order they occur in \p gens.
    //!
    //! \param gens the generators.
    //!
    //! \throws LibsemigroupsException if any of the following hold:
    //! * \p gens is empty;
    //! * Degree`{}(x) != `Degree`{}(y)` for some \c x and \c y in
    //! \p gens.
    explicit FroidurePin(std::vector<element_type> const& gens);

    //! \copydoc FroidurePin(std::vector<element_type> const& gens)
    explicit FroidurePin(std::initializer_list<element_type> gens);

    //! Copy constructor.
    //!
    //! Constructs a new FroidurePin which is an exact copy of \p that. No
    //! enumeration is triggered for either \p that or of the newly constructed
    //! semigroup.
    //!
    //! \param that the FroidurePin to copy.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    FroidurePin(FroidurePin const& that);

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

    //! Convert a word in the generators to an element.
    //!
    //! This  function returns a copy of the element obtained
    //! by evaluating \p w.  A copy is returned instead of a reference, because
    //! the element corresponding to \p w may not yet have been
    //! enumerated.
    //!
    //! \param w the word in the generators to evaluate.
    //!
    //! \returns A copy of the element represented by the word \p w.
    //!
    //! \throws LibsemigroupsException if \p w is not a valid word in the
    //! generators, i.e. if it contains a value greater than or equal to the
    //! number of generators.
    //!
    //! \sa \ref current_position.
    element_type word_to_element(word_type const& w) const;

    //! Check equality of words in the generators.
    //!
    //! Returns \c true if the parameters represent the same element
    //! and \c false otherwise.
    //!
    //! \param x the first word for comparison
    //! \param y the second word for comparison
    //!
    //! \returns A value of type \c bool.
    //!
    //! \throws LibsemigroupsException if \p w contains an value exceeding
    //! \ref number_of_generators.
    bool equal_to(word_type const& x, word_type const& y) const override;

    //! Returns the number of generators.
    //!
    //! \parameters
    //! (None)
    //!
    //! \returns A value of type \c size_t.
    //!
    //! \exceptions
    //! \noexcept
    size_t number_of_generators() const noexcept override;

    //! Returns the generator with specified index.
    //!
    //! \param i the index of a generator.
    //!
    //! \returns
    //! A value of type \ref const_reference.
    //!
    //! \throws LibsemigroupsException if \p i is greater than or equal to \ref
    //! number_of_generators().
    //!
    //! \note
    //! Note that `generator(i)` is in general not in position \p i.
    const_reference generator(letter_type i) const;

    //! Find the position of an element with no enumeration.
    //!
    //! Returns the position of the element \p x in the semigroup if it is
    //! already known to belong to the semigroup or \ref UNDEFINED.  This
    //! function finds the position of the element \p x if it is already known
    //! to belong to \c this, and \ref UNDEFINED if not. If \c this is not yet
    //! fully enumerated, then this  function may return \ref UNDEFINED when \p
    //! x does belong to \c this.
    //!
    //! \param x a const reference to a possible element.
    //!
    //! \returns A value of type \c element_index_type.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \sa \ref position and \ref sorted_position.
    element_index_type current_position(const_reference x) const;

#ifndef DOXYGEN_SHOULD_SKIP_THIS
    using FroidurePinBase::current_position;
#endif

    //! Multiply elements via their indices.
    //!
    //! Returns the position of the product of the element with index \p i and
    //! the element with index \p j.
    //!
    //! This function either:
    //!
    //! * follows the path in the right or left Cayley graph from \p i to \p j,
    //!   whichever is shorter using \ref product_by_reduction; or
    //!
    //! * multiplies the elements in postions \p i and \p j together;
    //!
    //! whichever is better. The  function used is determined by comparing
    //! the output of the call operator of Complexity and the
    //! \ref current_length of \p i and \p j.
    //!
    //! For example, if the complexity of the multiplication is linear and \c
    //! this is a semigroup of transformations of degree 20, and the shortest
    //! paths in the left and right Cayley graphs from \p i to \p j are of
    //! length 100 and 1131, then it is better to just multiply the
    //! transformations together.
    //!
    //! \param i the index of the first element to multiply
    //! \param j the index of the second element to multiply
    //!
    //! \returns
    //! A value of type \c element_index_type.
    //!
    //! \throws LibsemigroupsException if the values \p i and \p j are greater
    //! than or equal to \ref current_size.
    element_index_type fast_product(element_index_type i,
                                    element_index_type j) const override;

    //! Returns the number of idempotents.
    //!
    //! \parameters
    //! (None)
    //!
    //! \returns
    //! A value of type \c size_t.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \note
    //! This function triggers a full enumeration.
    size_t number_of_idempotents() override;

    //! Check if an element is an idempotent via its index.
    //!
    //! Returns \c true if the element in position \p i is an idempotent
    //! and \c false if it is not.
    //!
    //! \param i the index of the element
    //!
    //! \returns
    //! A value of type \c bool.
    //!
    //! \throws LibsemigroupsException if \p i is greater than or equal to the
    //! size of \c this.
    //!
    //! \note
    //! This function triggers a full enumeration.
    bool is_idempotent(element_index_type i) override;

    //! Requests the given capacity for elements.
    //!
    //! The parameter \p val is also used to initialise certain data members.
    //! If you know a good upper bound for the size of your semigroup, then it
    //! might be a good idea to call this  function with that upper bound as an
    //! argument; this can significantly improve the performance of the \ref
    //! run  function, and consequently every other function too.
    //!
    //! \param val the number of elements to reserve space for.
    //!
    //! \returns
    //! (None)
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    void reserve(size_t val) override;

    //! Test membership of an element.
    //!
    //! Returns \c true if \p x belongs to \c this and \c false if it does
    //! not.
    //!
    //! \param x a const reference to a possible element.
    //!
    //! \returns A value of type \c bool.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \note This function may trigger a (partial) enumeration.
    bool contains(const_reference x);

    //! Find the position of an element with enumeration if necessary.
    //!
    //! Returns the position of \p x in \c this, or \ref UNDEFINED if \p x is
    //! not an element of \c this.
    //!
    //! \param x a const reference to a possible element.
    //!
    //! \returns A value of type \c element_index_type.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \sa \ref current_position and \ref sorted_position.
    element_index_type position(const_reference x);

    //! Returns the sorted index of an element.
    //!
    //! Returns the position of \p x in the elements of \c this when they are
    //! sorted by Less,  or \ref UNDEFINED if \p x is not an element of \c
    //! this.
    //!
    //! \param x a const reference to a possible element.
    //!
    //! \returns A value of type \c element_index_type.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \sa \ref current_position and \ref position.
    element_index_type sorted_position(const_reference x);

    //! Returns the sorted index of an element via its index.
    //!
    //! Returns the position of the element with index \p i when the elements
    //! are sorted using Less, or \ref UNDEFINED if \p i is greater than size().
    //!
    //! \param i the index of the element
    //!
    //! \returns
    //! A value of type \ref element_index_type.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    // TODO(3.0) rename to sorted_position
    element_index_type
    position_to_sorted_position(element_index_type i) override;

    //! Access element specified by index with bound checks.
    //!
    //! This function attempts to enumerate until at least \p i + 1 elements
    //! have been found.
    //!
    //! \param i the index of the element to access.
    //!
    //! \returns The element with index \p i (if any).
    //!
    //! \throws LibsemigroupsException if \p i is greater than or equal to the
    //! return value of size().
    const_reference at(element_index_type i);

    //! Access element specified by index.
    //!
    //! This function attempts to enumerate until at least \p i + 1 elements
    //! have been found.
    //!
    //! \param i the index of the element to access.
    //!
    //! \returns The element with index \p i (if any).
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    const_reference operator[](element_index_type i) const;

    //! Access element specified by sorted index with bound checks.
    //!
    //! This function triggers a full enumeration, and the parameter \p i
    //! is the index when the elements are sorted by Less.
    //!
    //! \param i the sorted index of the element to access.
    //!
    //! \returns The element with index \p i (if any).
    //!
    //! \throws LibsemigroupsException if \p i is greater than or equal to the
    //! return value of size().
    const_reference sorted_at(element_index_type i);

    //! Factorise an element as a word in the generators.
    //!
    //! Returns the short-lex minimum word (if any) in the generators that
    //! evaluates to \p x.
    //!
    //! \param x a const reference to a possible element to factorise.
    //!
    //! \returns Returns a \ref word_type which evaluates to \p x.
    //!
    //! \throws LibsemigroupsException if \p x does not belong to \c this.
    //!
    //! \sa minimal_factorisation(element_index_type).
    //!
    //! \note This function may trigger a (partial) enumeration.
    word_type minimal_factorisation(const_reference x);

#ifndef DOXYGEN_SHOULD_SKIP_THIS
    // The following are required, they are documented in FroidurePinBase.
    // Sphinx/doxygen get confused by this, so we don't allow Doxygen to parse
    // these two declarations.
    using FroidurePinBase::factorisation;
    using FroidurePinBase::minimal_factorisation;
#endif

    //! Factorise an element as a word in the generators.
    //!
    //! The key difference between this  function and
    //! \ref minimal_factorisation(const_reference x), is that the
    //! resulting factorisation may not be minimal.
    //!
    //! \param x a const reference to a possible element to factorise.
    //!
    //! \returns Returns a \ref word_type which evaluates to \p x.
    //!
    //! \throws LibsemigroupsException if \p x does not belong to \c this.
    //!
    //! \note This function may trigger a (partial) enumeration.
    word_type factorisation(const_reference x);

    //! Check finiteness.
    //!
    //! Returns tril::TRUE if the semigroup represented by \c this is finite,
    //! tril::FALSE if it is infinite, and tril::unknown if it is not known.
    //!
    //! For some types of elements, such as matrices over the integers, for
    //! example, it is undecidable, in general, if the semigroup generated by
    //! these elements is finite or infinite. On the other hand, for other
    //! types, such as transformation, the semigroup is always finite.
    //!
    //! \parameters
    //! (None)
    //!
    //! \returns
    //! A value of type \ref tril.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \note
    //! No enumeration is triggered by calls to this function.
    tril is_finite() const override {
      return tril::TRUE;
    }

    //! Add a copy of an element to the generators.
    //!
    //! This  function can be used to add new generators to an existing
    //! FroidurePin instance in such a way that any previously enumerated data
    //! is preserved and not recomputed, or copied. This can be faster than
    //! recomputing the semigroup generated by the old generators and the new
    //! generators.
    //!
    //! This function changes the semigroup in-place, thereby
    //! invalidating possibly previously known data about the semigroup, such as
    //! the left or right Cayley graphs, number of idempotents, and so on.
    //!
    //! Every generator in \p coll is added regardless of whether or not it is
    //! already a generator or element of the semigroup (it may belong to the
    //! semigroup but just not be known to belong). If \p coll is empty, then
    //! the semigroup is left unchanged. The order the generators is added is
    //! also the order they occur in the parameter \p coll.
    //!
    //! The FroidurePin instance is returned in a state where all of the
    //! previously enumerated elements which had been multiplied by all of the
    //! old generators, have now been multiplied by all of the old and new
    //! generators. This means that after this  function is called the
    //! semigroup might contain many more elements than before (whether it is
    //! fully enumerating or not).
    //!
    //! \param x the generator to add.
    //!
    //! \returns (None)
    //!
    //! \throws LibsemigroupsException if any of the following hold:
    //! * immutable() returns \c true
    //! * the degree of \p x is incompatible with the existing degree.
    void add_generator(const_reference x);

    //! Add collection of generators via const reference.
    //!
    //! See \ref add_generator for a detailed description.
    //!
    //! \tparam T the type of the container for generators to add (must be a
    //! non-pointer type).
    //!
    //! \param coll the collection of generators to add.
    //!
    //! \returns (None)
    //!
    //! \throws LibsemigroupsException if any of the following hold:
    //! * immutable() returns \c true
    //! * the degree of \p x is incompatible with the existing degree.
    template <typename T>
    void add_generators(T const& coll);

    //! Add collection of generators via initializer list.
    //!
    //! See \ref add_generator for a detailed description.
    //!
    //! \param coll the collection of generators to add.
    //!
    //! \returns (None)
    //!
    //! \throws LibsemigroupsException if any of the following hold:
    //! * immutable() returns \c true
    //! * the degree of \p x is incompatible with the existing degree.
    void add_generators(std::initializer_list<const_element_type> coll);

    //! Add collection of generators via iterators.
    //!
    //! See \ref add_generator for a detailed description.
    //!
    //! \tparam the type of an iterator pointing to an \ref element_type.
    //!
    //! \param first iterator pointing to the first generator to add.
    //! \param last iterator pointing one past the last generator to add.
    //!
    //! \returns (None)
    //!
    //! \throws LibsemigroupsException if any of the following hold:
    //! * immutable() returns \c true
    //! * the degree of \p x is incompatible with the existing degree.
    template <typename T>
    void add_generators(T const& first, T const& last);

    //! Copy and add a collection of generators.
    //!
    //! This function is equivalent to copy constructing an new FroidurePin
    //! instance and  then calling \ref add_generators on the copy. But this
    //! function avoids copying the parts of \c this that are immediately
    //! invalidated by \ref add_generators.
    //!
    //! \tparam T the type of the container for generators to add (must be a
    //! non-pointer type).
    //!
    //! \param coll the collection of generators to add.
    //!
    //! \returns A new FroidurePin instance by value generated by the
    //! generators of \c this and \p coll.
    //!
    //! \throws LibsemigroupsException if the copy constructor or \ref
    //! add_generators throws.
    template <typename T>
    FroidurePin copy_add_generators(T const& coll) const;

    //! \copydoc copy_add_generators(T const&) const
    FroidurePin copy_add_generators(std::initializer_list<element_type> coll);

    //! Add non-redundant generators in collection.
    //!
    //! Add copies of the non-redundant generators in \p coll to the generators
    //! of \c this.
    //!
    //! This  function differs from \ref add_generators in that it
    //! tries to add the new generators one by one, and only adds those
    //! generators that are not products of existing generators (including any
    //! new generators from \p coll that were added before). The generators are
    //! added in the order they occur in \p coll.
    //!
    //! This function changes \c this in-place, thereby invalidating
    //! some previously computed information, such as the left or
    //! right Cayley graphs, or number of idempotents, for example.
    //!
    //! \tparam T the type of the container for generators to add (must be a
    //! non-pointer type).
    //!
    //! \param coll the collection of generator to add.
    //!
    //! \returns
    //! (None)
    //!
    //! \throws LibsemigroupsException if \ref add_generator throws.
    template <typename T>
    void closure(T const& coll);

    //! \copydoc closure(T const&)
    void closure(std::initializer_list<const_element_type> coll);

    //! Copy and add non-redundant generators.
    //!
    //! This function is equivalent to copy constructing an new FroidurePin
    //! instance and  then calling \ref closure on the copy. But this
    //! function avoids copying the parts of \c this that are immediately
    //! invalidated by \ref closure.
    //!
    //! \tparam T the type of the container for generators to add (must be a
    //! non-pointer type).
    //!
    //! \param coll the collection of generators to add.
    //!
    //! \returns A new FroidurePin instance by value generated by the
    //! generators of \c this and \p coll.
    //!
    //! \throws LibsemigroupsException if the copy constructor or \ref
    //! add_generators throws.
    template <typename T>
    FroidurePin copy_closure(T const& coll);

    //! \copydoc copy_closure(T const&)
    FroidurePin copy_closure(std::initializer_list<element_type> coll);

    //! Returns a \shared_ptr to the state (if any).
    //!
    //! \parameters
    //! (None)
    //!
    //! \returns
    //! \shared_ptr to \ref state_type.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    std::shared_ptr<state_type> state() const {
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

    void expand(size_type);
    void is_one(internal_const_element_type x, element_index_type) noexcept(
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
    //! Return type of \ref cbegin and \ref cend.
    //!
    //! Return type for const random access iterators pointing at the elements
    //! of a FroidurePin object in the order they were enumerated (i.e. in
    //! short-lex order of the minimum word in the generators).
    //!
    //! \sa const_reverse_iterator.
    using const_iterator
        = detail::BruidhinnConstIterator<element_type,
                                         std::vector<internal_element_type>>;

    //! Return type of \ref crbegin and \ref crend.
    //!
    //! A type for const random access iterators through the elements, in
    //! the reverse order of enumeration.
    //!
    //! \sa const_iterator.
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    //! Return type of \ref cbegin_sorted and \ref cend_sorted.
    //!
    //! A type for const random access iterators through the elements, sorted
    //! according to Less.
    //!
    //! \sa const_reverse_iterator_sorted.
    using const_iterator_sorted = const_iterator_pair_first;

    //! Return type of \ref crbegin_sorted and \ref crend_sorted.
    //!
    //! A type for const random access iterators through the elements,
    //! sorted according to Less.
    //!
    //! \sa const_iterator_sorted.
    using const_reverse_iterator_sorted = const_reverse_iterator_pair_first;

    //! Return type of \ref cbegin_idempotents and \ref cend_idempotents.
    //!
    //! A type for const random access iterators through the idempotents, in
    //! order of generation (short-lex order).
    //!
    //! \sa const_iterator.
    using const_iterator_idempotents = const_iterator_pair_first;

    // Return type of \ref crbegin_idempotents and \ref crend_idempotents.
    //
    // A type for reverse const iterators through the idempotents of \c this,
    // in short-lex order.
    //
    // \sa const_iterator_idempotents.
    // using const_reverse_iterator_idempotents
    //    = const_reverse_iterator_pair_first;

    //! Returns a const iterator pointing to the first element (ordered by
    //! discovery).
    //!
    //! This function does not trigger any enumeration, and the returned
    //! iterators may be invalidated by any call to a non-const function of the
    //! FroidurePin class.
    //!
    //! \parameters
    //! (None)
    //!
    //! \returns A value of type \ref const_iterator.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Constant.
    //!
    //! \sa \ref begin.
    const_iterator cbegin() const;

    //! Returns a const iterator pointing to the first element (ordered by
    //! discovery).
    //!
    //! This function does not trigger any enumeration, and the returned
    //! iterators may be invalidated by any call to a non-const function of the
    //! FroidurePin class.
    //!
    //! \parameters
    //! (None)
    //!
    //! \returns A value of type \ref const_iterator.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Constant.
    //!
    //! \sa \ref cbegin.
    const_iterator begin() const;

    //! Returns a const iterator pointing to one past the last known element.
    //!
    //! This function does not trigger any enumeration, and the returned
    //! iterators may be invalidated by any call to a non-const function of the
    //! FroidurePin class.
    //!
    //! \parameters
    //! (None)
    //!
    //! \returns A value of type \ref const_iterator.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Constant.
    //!
    //! \sa \ref end.
    const_iterator cend() const;

    //! Returns a const iterator pointing one past the last known element.
    //!
    //! This function does not trigger any enumeration, and the returned
    //! iterators may be invalidated by any call to a non-const function of the
    //! FroidurePin class.
    //!
    //! \parameters
    //! (None)
    //!
    //! \returns A value of type \ref const_iterator.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Constant.
    //!
    //! \sa \ref cend.
    const_iterator end() const;

    //! Returns a const reverse iterator pointing to the last known element.
    //!
    //! This function does not trigger any enumeration, and the returned
    //! iterators may be invalidated by any call to a non-const function of the
    //! FroidurePin class.
    //!
    //! \parameters
    //! (None)
    //!
    //! \returns A value of type \ref const_reverse_iterator.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Constant.
    //!
    //! \sa \ref crend.
    const_reverse_iterator crbegin() const;

    //! Returns a const reverse iterator pointing one before the first
    //! element.
    //!
    //! This function does not trigger any enumeration, and the returned
    //! iterators may be invalidated by any call to a non-const function of the
    //! FroidurePin class.
    //!
    //! \parameters
    //! (None)
    //!
    //! \returns A value of type \ref const_reverse_iterator.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Constant.
    //!
    //! \sa \ref crbegin.
    const_reverse_iterator crend() const;

    //! Returns a const iterator pointing to the first element (sorted by
    //! Less).
    //!
    //! \parameters
    //! (None)
    //!
    //! \returns A value of type \ref const_iterator_sorted.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \note This function triggers a full enumeration.
    const_iterator_sorted cbegin_sorted();

    //! Returns a const iterator pointing one past the last element (sorted by
    //! Less).
    //!
    //! \parameters
    //! (None)
    //!
    //! \returns A value of type \ref const_iterator_sorted.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \note This function triggers a full enumeration.
    const_iterator_sorted cend_sorted();

    //! Returns a const iterator pointing to the last element (sorted by
    //! \ref Less).
    //!
    //! \parameters
    //! (None)
    //!
    //! \returns A value of type \ref const_reverse_iterator_sorted.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \note This function triggers a full enumeration.
    const_reverse_iterator_sorted crbegin_sorted();

    //! Returns a const reverse iterator pointing one before the first
    //! element (sorted by Less).
    //!
    //! \parameters
    //! (None)
    //!
    //! \returns A value of type \ref const_reverse_iterator_sorted.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \note This function triggers a full enumeration.
    const_reverse_iterator_sorted crend_sorted();

    //! Returns a const iterator pointing at the first idempotent.
    //!
    //! If the returned iterator is incremented, then it points to the second
    //! idempotent in the semigroup (if it exists), and every subsequent
    //! increment points to the next idempotent.
    //!
    //! \parameters
    //! (None)
    //!
    //! \returns A value of type \ref const_iterator_idempotents.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \note This function triggers a full enumeration.
    const_iterator_idempotents cbegin_idempotents();

    //! Returns a const iterator pointing one past the last idempotent.
    //!
    //! \parameters
    //! (None)
    //!
    //! \returns A value of type \ref const_iterator_idempotents.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \note This function triggers a full enumeration.
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
  };
}  // namespace libsemigroups

// Include the implementation of the member functions for FroidurePin
#include "froidure-pin-impl.hpp"
#endif  // LIBSEMIGROUPS_FROIDURE_PIN_HPP_
