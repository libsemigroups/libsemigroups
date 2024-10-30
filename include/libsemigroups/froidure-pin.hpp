//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019-2023 James D. Mitchell
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

#ifndef LIBSEMIGROUPS_FROIDURE_PIN_HPP_
#define LIBSEMIGROUPS_FROIDURE_PIN_HPP_

#include <cstddef>           // for size_t
#include <initializer_list>  // for initializer_list
#include <iterator>          // for make_move_iterator
#include <memory>            // for shared_ptr, make_shared
#include <mutex>             // for mutex
#include <type_traits>       // for is_const, remove_pointer
#include <unordered_map>     // for unordered_map
#include <utility>           // for pair
#include <vector>            // for vector

#include "adapters.hpp"           // for Complexity, Degree, IncreaseDegree
#include "debug.hpp"              // for LIBSEMIGROUPS_ASSERT
#include "exception.hpp"          // for LIBSEMIGROUPS_EXCEPTION
#include "froidure-pin-base.hpp"  // for FroidurePinBase, FroidurePinBase::s...
#include "types.hpp"              // for letter_type, word_type

#include "detail/bruidhinn-traits.hpp"  // for detail::BruidhinnTraits
#include "detail/iterator.hpp"          // for ConstIteratorStateless
#include "detail/report.hpp"            // for report_default
#include "detail/stl.hpp"               // for EqualTo, Hash

#include "rx/ranges.hpp"  // for iterator_range

//! \brief Namespace for everything in the libsemigroups library.
namespace libsemigroups {

  template <typename T>
  struct FroidurePinState {
    using type = void;
  };

  //! \ingroup froidure_pin_group
  //!
  //! \brief Traits class for FroidurePin.
  //!
  //! Defined in ``froidure-pin.hpp``.
  //!
  //! This is a traits class for use with FroidurePin.
  //!
  //! \tparam Element the type of the elements.
  //! \tparam State the type of the state (if any, defaults to \c void,
  //! meaning none).
  //!
  //! \sa FroidurePinBase and FroidurePin.
  template <typename Element,
            typename State = typename FroidurePinState<Element>::type>
  struct FroidurePinTraits {
    // Require to get the value_type from detail::BruidhinnTraits to remove
    // pointer to const.
    //! \brief The type of the elements of a FroidurePin instance.
    //!
    //! This type has const removed, and if \c Element is a pointer to
    //! const, then the second const is also removed.
    using element_type = typename detail::BruidhinnTraits<Element>::value_type;

    //! \brief The type of the state (if any).
    //!
    //! This type can be used to store some state that might be required in an
    //! FroidurePin instance.
    using state_type = State;

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

  //! \ingroup froidure_pin_group
  //!
  //! \brief Class implementing the Froidure-Pin algorithm.
  //!
  //! Defined in ``froidure-pin.hpp``.
  //!
  //! The class template FroidurePin implements the Froidure-Pin algorithm as
  //! described in the article \cite Froidure1997aa by Veronique Froidure and
  //! Jean-Eric Pin. A FroidurePin instance is defined by a generating set, and
  //! the main function is \ref run, which implements the Froidure-Pin
  //! Algorithm. If \ref run is invoked and \ref finished returns \c true, then
  //! the size \ref size, the left and right Cayley graphs \ref
  //! left_cayley_graph and \ref right_cayley_graph are determined, and a
  //! confluent terminating presentation \ref rules for the semigroup is known.
  //!
  //! \tparam Element the type of the elements in the represented
  //! semigroup
  //!
  //! \tparam Traits a traits class holding various adapters used by the
  //! implementation (defaults to FroidurePinTraits).
  //!
  //! \sa FroidurePinTraits and FroidurePinBase.
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
  template <typename Element, typename Traits = FroidurePinTraits<Element>>
  class FroidurePin : private detail::BruidhinnTraits<Element>,
                      public FroidurePinBase {
   private:
    ////////////////////////////////////////////////////////////////////////
    // FroidurePin - typedefs - private
    ////////////////////////////////////////////////////////////////////////

    using internal_element_type =
        typename detail::BruidhinnTraits<Element>::internal_value_type;
    using internal_const_element_type =
        typename detail::BruidhinnTraits<Element>::internal_const_value_type;
    using internal_const_reference =
        typename detail::BruidhinnTraits<Element>::internal_const_reference;
    using internal_idempotent_pair
        = std::pair<internal_element_type, element_index_type>;

    static_assert(
        std::is_const_v<internal_const_element_type>
            || std::is_const_v<
                std::remove_pointer_t<internal_const_element_type>>,
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

    struct InternalEqualTo : private detail::BruidhinnTraits<Element> {
      [[nodiscard]] bool operator()(internal_const_reference x,
                                    internal_const_reference y) const {
        return EqualTo()(this->to_external_const(x),
                         this->to_external_const(y));
      }
    };

    struct InternalHash : private detail::BruidhinnTraits<Element> {
      [[nodiscard]] size_t operator()(internal_const_reference x) const {
        return Hash()(this->to_external_const(x));
      }
    };

    using map_type = std::unordered_map<internal_const_element_type,
                                        element_index_type,
                                        InternalHash,
                                        InternalEqualTo>;

   public:
    ////////////////////////////////////////////////////////////////////////
    // FroidurePin - typedefs - public
    ////////////////////////////////////////////////////////////////////////

    //! Type of the elements.
    using element_type = typename detail::BruidhinnTraits<Element>::value_type;

    //! Alias for element_type.
    // This only really exists to allow the python bindings to compile with
    // gcc-6 + 7 (at least).
    using value_type = element_type;

    //! Type of const elements.
    using const_element_type =
        typename detail::BruidhinnTraits<Element>::const_value_type;

    //! Type of element const references.
    using const_reference =
        typename detail::BruidhinnTraits<Element>::const_reference;

    //! Type of element rvalue references.
    using rvalue_reference =
        typename detail::BruidhinnTraits<Element>::rvalue_reference;

    //! Type of element references.
    using reference = typename detail::BruidhinnTraits<Element>::reference;

    //! Type of element const pointers.
    using const_pointer =
        typename detail::BruidhinnTraits<Element>::const_pointer;

    //! \copydoc FroidurePinBase::size_type
    using size_type = FroidurePinBase::size_type;

    //! \copydoc FroidurePinBase::element_index_type
    using element_index_type = FroidurePinBase::element_index_type;

    //! \copydoc FroidurePinBase::cayley_graph_type
    using cayley_graph_type = FroidurePinBase::cayley_graph_type;

    //! Type of the state used for multiplication (if any).
    using state_type = typename Traits::state_type;

    //! \copydoc libsemigroups::Complexity
    using Complexity = typename Traits::Complexity;

    //! \copydoc libsemigroups::Degree
    using Degree = typename Traits::Degree;

    //! \copydoc libsemigroups::EqualTo
    using EqualTo = typename Traits::EqualTo;

    //! \copydoc libsemigroups::Hash
    using Hash = typename Traits::Hash;

    //! \copydoc libsemigroups::IncreaseDegree
    using IncreaseDegree = typename Traits::IncreaseDegree;

    //! \copydoc libsemigroups::Less
    using Less = typename Traits::Less;

    //! \copydoc libsemigroups::One
    using One = typename Traits::One;

    //! \copydoc libsemigroups::Product
    using Product = typename Traits::Product;

    //! \copydoc libsemigroups::Swap
    using Swap = typename Traits::Swap;

   private:
    template <typename T>
    static constexpr bool IsState
        = ((!std::is_void_v<T>) && std::is_same_v<state_type, T>);

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

    void internal_product(reference       xy,
                          const_reference x,
                          const_reference y,
                          state_type*     stt,
                          size_t          tid = 0) const {
      if constexpr (std::is_void_v<state_type>) {
        (void) stt;  // To silence warnings in g++-9
        Product()(xy, x, y, tid);
      } else {
        Product()(xy, x, y, stt, tid);
      }
    }

   public:
    ////////////////////////////////////////////////////////////////////////
    // FroidurePin - constructors + destructor - public
    ////////////////////////////////////////////////////////////////////////

    //! \brief Default constructor.
    //!
    //! Constructs a FroidurePin instance with no generators.
    //!
    //! \sa add_generator and add_generators.
    FroidurePin();

    //! \brief Reinitialize a FroidurePin object.
    //!
    //! This function re-initializes a FroidurePin object so that it is in
    //! the same state as if it had just been default constructed.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    FroidurePin& init();

    //! \brief Construct from std::shared_ptr to state.
    //!
    //! This function allows the construction of a FroidurePin instance with
    //! stated given by the parameter \p stt. This constructor only exists if
    //! \ref state_type is not \c void. This is used when the elements require
    //! some shared state to define their multiplication, such as, for example
    //! an instance of KnuthBendix or ToddCoxeter.
    //!
    //! \param stt a std::shared_ptr to a state object.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    template <typename State, typename = std::enable_if_t<IsState<State>>>
    explicit FroidurePin(std::shared_ptr<State> stt) : FroidurePin() {
      _state = stt;
    }

    //! \brief Reinitialize a FroidurePin object from state.
    //!
    //! This function re-initializes a FroidurePin object so that it is in
    //! the same state as if it had just been constructed from \p stt.
    //!
    //! \param stt  a std::shared_ptr to the state (if any).
    //!
    //! \returns A reference to `*this`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    template <typename State, typename = std::enable_if_t<IsState<State>>>
    FroidurePin& init(std::shared_ptr<State> stt) {
      init();
      _state = stt;
    }

    //! \brief Construct from const reference to state.
    //!
    //! This function allows the construction of a FroidurePin instance with
    //! stated given by the parameter \p stt. This constructor only exists if
    //! \ref state_type is not \c void. This is used when the elements require
    //! some shared state to define their multiplication, such as, for example
    //! an instance of KnuthBendix or ToddCoxeter.
    //!
    //! \param stt a const reference to a state object.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \warning
    //! The parameter \p stt is copied, which might be expensive, use
    //! a std::shared_ptr to avoid the copy.
    template <typename State, typename = std::enable_if_t<IsState<State>>>
    explicit FroidurePin(State const& stt)
        : FroidurePin(std::make_shared<state_type>(stt)) {}

    //! \brief Reinitialize a FroidurePin object from state.
    //!
    //! This function re-initializes a FroidurePin object so that it is in
    //! the same state as if it had just been constructed from \p stt.
    //!
    //! \param stt  a const reference to the state (if any).
    //!
    //! \returns A reference to `*this`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    template <typename State, typename = std::enable_if_t<IsState<State>>>
    FroidurePin& init(State const& stt) {
      return init(std::make_shared<state_type>(stt));
    }

    //! Copy assignment operator.
    FroidurePin& operator=(FroidurePin const&);

    //! Default move assignment operator.
    FroidurePin& operator=(FroidurePin&&) = default;

    //! \brief Construct from a range of generators given by iterators.
    //!
    //! This function constructs a FroidurePin instance with generators in the
    //! range pointed to by the iterators \p first and \p last.
    //!
    //! \tparam Iterator1 the type of the first parameter.
    //! \tparam Iterator2 the type of the second parameter.
    //!
    //! \param first iterator pointing at the first generator to add.
    //! \param last iterator pointing one beyond the last generator to add.
    //!
    //! \warning This function does not check its arguments. In particular, it
    //! is assumed that the generators pointed to by \p first and \p last all
    //! have the same degree.
    template <typename Iterator1, typename Iterator2>
    FroidurePin(Iterator1 first, Iterator2 last);

    //! \brief Reinitialize a FroidurePin object from a range of generators
    //! given by iterators.
    //!
    //! This function re-initializes a FroidurePin object so that it is in
    //! the same state as if it had just been constructed from \p first and \p
    //! last.
    //!
    //! \tparam Iterator1 the type of the first parameter.
    //! \tparam Iterator2 the type of the second parameter.
    //!
    //! \param first iterator pointing at the first generator to add.
    //! \param last iterator pointing one beyond the last generator to add.
    //!
    //! \warning This function does not check its arguments. In particular, it
    //! is assumed that the generators pointed to by \p first and \p last all
    //! have the same degree.
    template <typename Iterator1, typename Iterator2>
    FroidurePin& init(Iterator1 first, Iterator2 last);

    //! \brief Copy constructor.
    //!
    //! Constructs a new FroidurePin which is an exact copy of \p that. No
    //! enumeration is triggered for either \p that or of the newly constructed
    //! FroidurePin object.
    //!
    //! \param that the FroidurePin to copy.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    FroidurePin(FroidurePin const& that);

    //! \brief Default move constructor.
    FroidurePin(FroidurePin&&) = default;

    ~FroidurePin();

   private:
    void free_data();

    ////////////////////////////////////////////////////////////////////////
    // FroidurePin - constructor - private
    ////////////////////////////////////////////////////////////////////////

    FroidurePin(FroidurePin const&, const_reference);

   public:
    ////////////////////////////////////////////////////////////////////////
    // FroidurePin - member functions - public
    ////////////////////////////////////////////////////////////////////////

    //! \brief Convert a word in the generators to an element.
    //!
    //! This  function returns a reference to the element obtained by
    //! evaluating \p w. The returned reference may only valid until the next
    //! function that triggers an enumeration is called, or another call to this
    //! function is made.
    //!
    //! \param w the word in the generators to evaluate.
    //!
    //! \returns A const reference to the element represented by the word \p w.
    //!
    //! \warning This function does not check its arguments, and it is assumed
    //! that the values in \p w are less than \ref number_of_generators; and
    //! if \p w is empty it is assumed that \ref contains_one_no_run returns \c
    //! true (although nothing bad will happen if this doesn't hold, except that
    //! this function will return the identity element even though it might not
    //! be an element of the semigroup).
    //!
    //! \sa \ref current_position.
    // TODO(0) no trigger note in the doc
    // TODO(0) update the doc
    template <typename Iterator1, typename Iterator2>
    [[nodiscard]] const_reference to_element_no_checks(Iterator1 first,
                                                       Iterator2 last) const {
      element_index_type pos = current_position_no_checks(first, last);
      if (pos != UNDEFINED) {
        return this->to_external_const(_elements[pos]);
      }

      // Consider the empty case separately to avoid the unnecessary
      // multiplication by the identity.
      if (first == last) {
        // The next line asserts we can't get here without _id being allocated.
        LIBSEMIGROUPS_ASSERT(degree() != UNDEFINED);
        // The next line asserts that _id actually is an element, if not, then
        // we shouldn't be calling this function with the empty word.
        LIBSEMIGROUPS_ASSERT(contains_one_no_run());
        // TODO(0) contains_one_no_run -> currently_contains_one
        return this->to_external_const(_id);
      }

      // current_position is always known for generators (i.e. when w.size()
      // == 1), so w.size() > 1 should be true here
      LIBSEMIGROUPS_ASSERT(std::distance(first, last) > 1);

      element_type prod  // TODO(1) remove allocation here
          = this->external_copy(this->to_external_const(_id));

      auto* state_ptr = _state.get();
      for (auto it = first; it != last; ++it) {
        LIBSEMIGROUPS_ASSERT(*it < number_of_generators());
        Swap()(this->to_external(_tmp_product), prod);
        internal_product(prod,
                         this->to_external_const(_tmp_product),
                         this->to_external_const(_gens[*it]),
                         state_ptr);
      }
      Swap()(this->to_external(_tmp_product), prod);
      this->external_free(prod);

      return this->to_external_const(_tmp_product);
    }

    //! \brief Convert a word in the generators to an element.
    //!
    //! This  function returns a reference to the element obtained by
    //! evaluating \p w. The returned reference may only valid until the next
    //! function that triggers an enumeration is called, or another call to this
    //! function is made.
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
    // TODO(0) to tpp
    // TODO(0) no trigger note in the doc
    // TODO(0) update the doc
    template <typename Iterator1, typename Iterator2>
    [[nodiscard]] const_reference to_element(Iterator1 first,
                                             Iterator2 last) const {
      // If !w.empty() && number_of_generators() == 0, then
      // throw_if_any_generator_index_out_of_range will throw . . .
      throw_if_any_generator_index_out_of_range(first, last);
      if (first == last) {
        if (number_of_generators() == 0) {
          // . . . hence this function throws if number_of_generators() == 0.
          LIBSEMIGROUPS_EXCEPTION("cannot convert the empty word to an element "
                                  "when no generators are defined");
        }
        if (!contains_one_no_run()) {
          LIBSEMIGROUPS_EXCEPTION(
              "cannot convert the empty word to an element, the identity is "
              "not {}an element of the semigroup",
              finished() ? "" : "known to be ");
        }
      }
      return to_element_no_checks(first, last);
    }

    //! \brief Check equality of words in the generators.
    //!
    //! This function returns \c true if the parameters represent the same
    //! element and \c false otherwise. No enumeration is triggered by calls to
    //! this function.
    //!
    //! \param x the first word for comparison.
    //! \param y the second word for comparison.
    //!
    //! \returns A value of type \c bool.
    //!
    //! \warning This function does not check its arguments. In particular, it
    //! is assumed that every value in \p x and \p y belongs in the range from
    //! \c 0 to \ref number_of_generators (non-inclusive).
    // TODO(0) replace with iterators
    [[nodiscard]] bool equal_to_no_checks(word_type const& x,
                                          word_type const& y) const;

    //! \brief Check equality of words in the generators.
    //!
    //! This function returns \c true if the parameters represent the same
    //! element and \c false otherwise. No enumeration is triggered by calls to
    //! this function.
    //!
    //! \param x the first word for comparison.
    //! \param y the second word for comparison.
    //!
    //! \returns A value of type \c bool.
    //!
    //! \throws LibsemigroupsException if \p w contains an value exceeding
    //! \ref number_of_generators.
    // TODO(0) replace with iterators
    [[nodiscard]] bool equal_to(word_type const& x, word_type const& y) const {
      throw_if_any_generator_index_out_of_range(std::begin(x), std::end(x));
      throw_if_any_generator_index_out_of_range(std::begin(y), std::end(y));
      return equal_to_no_checks(x, y);
    }

    //! \brief Returns the number of generators.
    //!
    //! \returns A value of type \c size_t.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] size_t number_of_generators() const noexcept override;

    //! \brief Returns the generator with specified index.
    //!
    //! This function returns the generator with index \p i, where the order is
    //! that in which the generators were added at construction, or via \ref
    //! init, \ref add_generator, \ref add_generators, \ref closure, \ref
    //! copy_closure, or \ref copy_add_generators.
    //!
    //! \param i the index of a generator.
    //!
    //! \returns
    //! The generator with given index.
    //!
    //! \throws LibsemigroupsException if \p i is greater than or equal to \ref
    //! number_of_generators().
    //!
    //! \note
    //! Note that `generator(j)` is in general not in position \p j.
    [[nodiscard]] const_reference generator(generator_index_type i) const;

    //! \brief Returns the generator with specified index.
    //!
    //! This function returns the generator with index \p i, where the order is
    //! that in which the generators were added at construction, or via \ref
    //! init, \ref add_generator, \ref add_generators, \ref closure, \ref
    //! copy_closure, or \ref copy_add_generators.
    //!
    //! \param i the index of a generator.
    //!
    //! \returns
    //! The generator with given index.
    //!
    //! \note
    //! Note that `generator(j)` is in general not in position \p j.
    //!
    //! \warning This function does not check its arguments. In particular it is
    //! assumed that \p i is less than \ref number_of_generators.
    [[nodiscard]] const_reference
    generator_no_checks(generator_index_type i) const;

    //! \brief Find the position of an element with no enumeration.
    //!
    //! This function returns the position of the element \p x in the semigroup
    //! if it is already known to belong to the semigroup or \ref UNDEFINED.
    //! This function finds the position of the element \p x if it is already
    //! known to belong to \c this, and \ref UNDEFINED if not. If \c this is not
    //! yet fully enumerated, then this  function may return \ref UNDEFINED when
    //! \p x does belong to \c this.
    //!
    //! \param x a const reference to a possible element.
    //!
    //! \returns A value of type \c element_index_type.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \sa \ref position and \ref sorted_position.
    [[nodiscard]] element_index_type current_position(const_reference x) const;

#ifndef PARSED_BY_DOXYGEN
    using FroidurePinBase::current_position;
#endif

    //! \brief Multiply elements via their indices.
    //!
    //! This function returns the position of the product of the element with
    //! index \p i and the element with index \p j.
    //!
    //! This function either:
    //!
    //! * follows the path in the right or left Cayley graph from \p i to \p j,
    //!   whichever is shorter using \ref froidure_pin::product_by_reduction; or
    //!
    //! * multiplies the elements in positions \p i and \p j together;
    //!
    //! whichever is better. The option used is determined by comparing
    //! the output of the call operator of the \ref Complexity adapter and the
    //! \ref current_length of \p i and \p j.
    //!
    //! For example, if the complexity of the multiplication is linear and \c
    //! this is a semigroup of transformations of degree 20, and the shortest
    //! paths in the left and right Cayley graphs from \p i to \p j are of
    //! length 100 and 1131, then it is better to just multiply the
    //! transformations together.
    //!
    //! \param i the index of the first element to multiply.
    //! \param j the index of the second element to multiply.
    //!
    //! \returns
    //! The index of the product.
    //!
    //! \warning The arguments of this function are not checked. In particular,
    //! it is assumed that both \p i and \p j are less than \ref current_size.
    [[nodiscard]] element_index_type
    fast_product_no_checks(element_index_type i, element_index_type j) const;

    //! \brief Multiply elements via their indices.
    //!
    //!  See \ref fast_product_no_checks for a full description.
    //!
    //! \param i the index of the first element to multiply.
    //! \param j the index of the second element to multiply.
    //!
    //! \returns
    //! The index of the product.
    //!
    //! \throws LibsemigroupsException if the values \p i and \p j are greater
    //! than or equal to \ref current_size.
    [[nodiscard]] element_index_type fast_product(element_index_type i,
                                                  element_index_type j) const {
      throw_if_element_index_out_of_range(i);
      throw_if_element_index_out_of_range(j);
      return fast_product_no_checks(i, j);
    }

    //! \brief Returns the number of idempotents.
    //!
    //! This function returns the number of idempotents in the semigroup
    //! represented by a \p FroidurePin instance.
    //!
    //! \returns
    //! The number of idempotents..
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \note
    //! This function triggers a full enumeration.
    [[nodiscard]] size_t number_of_idempotents();

    //! \brief Check if an element is an idempotent via its index.
    //!
    //! This function returns \c true if the element in position \p i is an
    //! idempotent and \c false if it is not.
    //!
    //! \param i the index of the element.
    //!
    //! \returns
    //! A value of type \c bool.
    //!
    //! \throws LibsemigroupsException if \p i is greater than or equal to the
    //! size of \c this.
    //!
    //! \note
    //! This function triggers a full enumeration.
    // TODO(1) improve this it doesn't need to trigger a full enum.
    [[nodiscard]] bool is_idempotent_no_checks(element_index_type i);

    //! \brief Check if an element is an idempotent via its index.
    //!
    //! This function returns \c true if the element in position \p i is an
    //! idempotent and \c false if it is not.
    //!
    //! \param i the index of the element.
    //!
    //! \returns
    //! A value of type \c bool.
    //!
    //! \throws LibsemigroupsException if \p i is greater than or equal to the
    //! size of \c this.
    //!
    //! \note
    //! This function triggers a full enumeration.
    [[nodiscard]] bool is_idempotent(element_index_type i) {
      run();
      throw_if_element_index_out_of_range(i);
      return is_idempotent_no_checks(i);
    }

    //! \brief Requests the given capacity for elements.
    //!
    //! The parameter \p val is also used to initialise certain data members.
    //! If you know a good upper bound for the size of your semigroup, then it
    //! might be a good idea to call this  function with that upper bound as an
    //! argument; this can significantly improve the performance of the \ref
    //! run  function, and consequently every other function too.
    //!
    //! \param val the number of elements to reserve space for.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    FroidurePin& reserve(size_t val);

    //! \brief Test membership of an element.
    //!
    //! This function returns \c true if \p x belongs to \c this and \c false if
    //! it does not.
    //!
    //! \param x a const reference to a possible element.
    //!
    //! \returns A value of type \c bool.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \note This function may trigger a (partial) enumeration.
    [[nodiscard]] bool contains(const_reference x);

    //! \brief Find the position of an element with enumeration if necessary.
    //!
    //! This function returns the position of \p x in \c this, or \ref UNDEFINED
    //! if \p x is not an element of \c this.
    //!
    //! \param x a const reference to a possible element.
    //!
    //! \returns A value of type \c element_index_type.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \sa \ref current_position and \ref sorted_position.
    [[nodiscard]] element_index_type position(const_reference x);

    //! \brief Returns the sorted index of an element.
    //!
    //! This function returns the position of \p x in the elements of \c this
    //! when they are sorted by Less,  or \ref UNDEFINED if \p x is not an
    //! element of \c this.
    //!
    //! \param x a const reference to a possible element.
    //!
    //! \returns A value of type \c element_index_type.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \sa \ref current_position and \ref position.
    [[nodiscard]] element_index_type sorted_position(const_reference x);

    //! \brief Returns the sorted index of an element via its index.
    //!
    //! This function returns the position of the element with index \p i when
    //! the elements are sorted using Less, or \ref UNDEFINED if \p i is greater
    //! than size().
    //!
    //! \param i the index of the element.
    //!
    //! \returns
    //! A value of type \ref element_index_type.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    // There's no no-checks version of this, there can't be.
    [[nodiscard]] element_index_type to_sorted_position(element_index_type i);

    //! \brief Access element specified by index with bound checks.
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
    [[nodiscard]] const_reference at(element_index_type i);

    //! \brief Access element specified by index.
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

    //! \brief Access element specified by sorted index with bound checks.
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
    [[nodiscard]] const_reference sorted_at(element_index_type i);

    //! \brief Access element specified by sorted index with bound checks.
    //!
    //! This function triggers a full enumeration, and the parameter \p i
    //! is the index when the elements are sorted by Less.
    //!
    //! \param i the sorted index of the element to access.
    //!
    //! \returns The element with index \p i (if any).
    //!
    //! \warning This function does not check its arguments. In particular, it
    //! assumes that \p i is less than \ref size.
    [[nodiscard]] const_reference sorted_at_no_checks(element_index_type i);

    //! \brief Returns a word containing a minimal factorisation (in the
    //! generators) of an element.
    //!
    //! This function returns the short-lex minimum word (if any) in the
    //! generators that evaluates to \p x.
    //!
    //! \param x a const reference to a possible element to factorise.
    //!
    //! \returns A \ref word_type which evaluates to \p x.
    //!
    //! \throws LibsemigroupsException if \p x does not belong to \c this.
    //!
    //! \note This function may trigger a (partial) enumeration.
    //!
    //! \sa minimal_factorisation(element_index_type).
    // TODO(0) to helper
    [[nodiscard]] word_type minimal_factorisation(const_reference x);

    //! \brief Modify a word in-place to contain a minimal factorisation (in the
    //! generators) of an element.
    //!
    //! This function returns the short-lex minimum word (if any) in the
    //! generators that evaluates to \p x.
    //!
    //! \param w the word_type to contain the factorisation.
    //! \param x a const reference to a possible element to factorise.
    //!
    //! \throws LibsemigroupsException if \p x does not belong to \c this.
    //!
    //! \note This function may trigger a (partial) enumeration.
    // TODO(0) to helper
    void minimal_factorisation(word_type& w, const_reference x);

#ifndef PARSED_BY_DOXYGEN
    // The following are required, they are documented in FroidurePinBase.
    // Sphinx/doxygen get confused by this, so we don't allow Doxygen to parse
    // these two declarations.
    using FroidurePinBase::factorisation;
    using FroidurePinBase::minimal_factorisation;
#endif

    //! \brief Returns a word containing a factorisation (in the generators) of
    //! an element.
    //!
    //! This function returns a word in the generators that equals the given
    //! element \p x.
    //!
    //! The key difference between this function and
    //! \ref minimal_factorisation(const_reference x), is that the
    //! resulting factorisation may not be minimal.
    //!
    //! \param x a const reference to a possible element to factorise.
    //!
    //! \returns A \ref word_type which evaluates to \p x.
    //!
    //! \throws LibsemigroupsException if \p x does not belong to \c this.
    //!
    //! \note This function may trigger a (partial) enumeration.
    // TODO(0) to helper
    [[nodiscard]] word_type factorisation(const_reference x);

    //! \brief Modify a word in-place to contain a factorisation (in the
    //! generators) of an element.
    //!
    //! The key difference between this function and
    //! \ref minimal_factorisation(word_type&, const_reference x), is that the
    //! resulting factorisation may not be minimal.
    //!
    //! \param w the word_type to contain the factorisation.
    //! \param x a const reference to a possible element to factorise.
    //!
    //! \throws LibsemigroupsException if \p x does not belong to \c this.
    //!
    //! \note This function may trigger a (partial) enumeration.
    // TODO(0) to helper
    void factorisation(word_type& w, const_reference x);

    //! \brief Check finiteness.
    //!
    //! This function returns tril::TRUE if the semigroup represented by \c this
    //! is finite, tril::FALSE if it is infinite, and tril::unknown if it is not
    //! known.
    //!
    //! For some types of elements, such as matrices over the integers, for
    //! example, it is undecidable, in general, if the semigroup generated by
    //! these elements is finite or infinite. On the other hand, for other
    //! types, such as transformation, the semigroup is always finite.
    //!
    //! \returns
    //! A value of type \ref tril.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \note
    //! No enumeration is triggered by calls to this function.
    [[nodiscard]] tril is_finite() const override {
      return tril::TRUE;
    }

    //! \brief Add collection of generators via iterators.
    //!
    //! See \ref add_generator for a detailed description.
    //!
    //! \tparam Iterator1 the type of the first parameter.
    //! \tparam Iterator2 the type of the second parameter.
    //!
    //! \param first iterator pointing to the first generator to add.
    //! \param last iterator pointing one past the last generator to add.
    //!
    //! \warning This function does not check its arguments. In particular, it
    //! is assumed that the generators pointed to by \p first and \p last all
    //! have the same degree.
    template <typename Iterator1, typename Iterator2>
    FroidurePin& add_generators_no_checks(Iterator1 first, Iterator2 last);

    //! \brief Add collection of generators via iterators.
    //!
    //! See \ref add_generator for a detailed description.
    //!
    //! \tparam the type of an iterator pointing to an \ref element_type.
    //!
    //! \param first iterator pointing to the first generator to add.
    //! \param last iterator pointing one past the last generator to add.
    //!
    //! \throws LibsemigroupsException if any of the degree of \p x is
    //! incompatible with the existing degree.
    template <typename Iterator1, typename Iterator2>
    FroidurePin& add_generators(Iterator1 first, Iterator2 last);

    //! \brief Add a copy of an element to the generators.
    //!
    //! This function can be used to add new generators to an existing
    //! FroidurePin instance in such a way that any previously enumerated data
    //! is preserved and not recomputed, or copied. This can be faster than
    //! rerunning a FroidurePin instance generated by the old generators and
    //! the new generators.
    //!
    //! This function changes the FroidurePin object in-place, thereby
    //! invalidating possibly previously known data, such as the left or right
    //! Cayley graphs, number of idempotents, and so on.
    //!
    //! The element \p x is added regardless of whether or not it is
    //! already a generator or element of the semigroup (it may belong to the
    //! semigroup but just not be known to belong).
    //! The new generator added will be the generator with the current
    //! highest index.
    //!
    //! The FroidurePin instance is returned in a state where all of the
    //! previously enumerated elements which had been multiplied by all of the
    //! old generators, have now been multiplied by all of the old and new
    //! generators. This means that after this function is called a FroidurePin
    //! instance might contain many more elements than before (whether it is
    //! fully enumerating or not).
    //!
    //! \param x the generator to add.
    //!
    //! \returns A const reference to `*this`.
    //!
    //! \warning This function does not check its argument. In particular, it
    //! is assumed that the element \p x has the same degree as any existing
    //! generators.
    FroidurePin& add_generator_no_checks(const_reference x);

    //! \brief Add a copy of an element to the generators.
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
    //! There can be duplicate generators and although they do not count as
    //! distinct elements, they do count as distinct generators.  In other
    //! words, the generators are precisely (a copy of) \p gens in the same
    //! order they occur in \p gens.
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
    //! \throws LibsemigroupsException if the degree of \p x is incompatible
    //! with the existing degree (if any).
    FroidurePin& add_generator(const_reference x);

    // TODO(1) make the following work
    // FroidurePin add_generator(rvalue_reference x);

    //! \brief Copy and add a collection of generators.
    //!
    //! This function is equivalent to copy constructing a new FroidurePin
    //! instance and  then calling \ref add_generators on the copy. But this
    //! function avoids copying the parts of \c this that are immediately
    //! invalidated by \ref add_generators.
    //!
    //! \tparam Iterator1 the type of the first parameter.
    //! \tparam Iterator2 the type of the second parameter.
    //!
    //! \param first iterator pointing at the first generator to add.
    //! \param last iterator pointing one beyond the last generator to add.
    //!
    //! \returns A new FroidurePin instance by value generated by the
    //! generators of \c this and the elements in the range from \p first to \p
    //! last.
    //!
    template <typename Iterator1, typename Iterator2>
    [[nodiscard]] FroidurePin
    copy_add_generators_no_checks(Iterator1 first, Iterator2 last) const;

    //! \brief Copy and add a collection of generators.
    //!
    //! See \ref copy_add_generators_no_checks for a full description.
    //!
    //! \tparam Iterator1 the type of the first parameter.
    //! \tparam Iterator2 the type of the second parameter.
    //!
    //! \param first iterator pointing at the first generator to add.
    //! \param last iterator pointing one beyond the last generator to add.
    //!
    //! \returns A new FroidurePin instance by value generated by the
    //! generators of \c this and \p coll.
    //!
    //! \throws LibsemigroupsException if any of the elements pointed to by \p
    //! first and \p last do not have degree compatible with any existing
    //! elements of the FroidurePin instance.
    //!
    //! \throws LibsemigroupsException if the elements pointed to by \p
    //! first and \p last do not all have the same degree.
    template <typename Iterator1, typename Iterator2>
    [[nodiscard]] FroidurePin copy_add_generators(Iterator1 first,
                                                  Iterator2 last) const {
      throw_if_degree_too_small(first, last);
      throw_if_inconsistent_degree(first, last);
      return copy_add_generators_no_checks(first, last);
    }

    // TODO(1) copy_add_generator
    // TODO(1) copy_add_generators_no_checks

    //! \brief Add non-redundant generators in collection.
    //!
    //! Add copies of the non-redundant generators pointed at by \p first and
    //! \p last to \c this.
    //!
    //! This function differs from \ref add_generators in that it
    //! tries to add the new generators one by one, and only adds those
    //! generators that are not products of existing generators (including any
    //! new generators that were added before). The generators
    //! are added in the order they are given (from \p first to \p last).
    //!
    //! This function changes \c this in-place, thereby invalidating
    //! some previously computed information, such as the left or
    //! right Cayley graphs, or number of idempotents, for example.
    //!
    //! \tparam Iterator1 the type of the first parameter.
    //! \tparam Iterator2 the type of the second parameter.
    //!
    //! \param first iterator pointing at the first generator to add.
    //! \param last iterator pointing one beyond the last generator to add.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \warning This function does not check its arguments. In particular, it
    //! is assumed that the elements pointed to by \p first and \p last all
    //! have the same degree.
    template <typename Iterator1, typename Iterator2>
    FroidurePin& closure_no_checks(Iterator1 first, Iterator2 last);

    //! \brief Add non-redundant generators in collection.
    //!
    //!  See \ref FroidurePin::closure_no_checks for full details.
    //!
    //! \tparam Iterator1 the type of the first parameter.
    //! \tparam Iterator2 the type of the second parameter.
    //!
    //! \param first iterator pointing at the first generator to add.
    //! \param last iterator pointing one beyond the last generator to add.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \throws LibsemigroupsException if any of the elements pointed to by \p
    //! first and \p last do not have degree compatible with any existing
    //! elements of the FroidurePin instance.
    //!
    //! \throws LibsemigroupsException if the elements pointed to by \p
    //! first and \p last do not all have the same degree.
    template <typename Iterator1, typename Iterator2>
    FroidurePin& closure(Iterator1 first, Iterator2 last) {
      throw_if_degree_too_small(first, last);
      throw_if_inconsistent_degree(first, last);
      return closure_no_checks(first, last);
    }

    // TODO(1) closure(const_reference)
    // TODO(1) closure_no_checks(const_reference)

    //! \brief Copy and add non-redundant generators.
    //!
    //! This function is equivalent to copy constructing a new FroidurePin
    //! instance and  then calling \ref closure on the copy. But this
    //! function avoids copying the parts of \c this that are immediately
    //! invalidated by \ref closure.
    //!
    //! \tparam Iterator1 the type of the first parameter.
    //! \tparam Iterator2 the type of the second parameter.
    //!
    //! \param first iterator pointing at the first generator to add.
    //! \param last iterator pointing one beyond the last generator to add.
    //!
    //! \returns A new FroidurePin instance by value generated by the
    //! generators of \c this and the non-redundant generators in the range from
    //! \p first to \p last.
    //!
    //! \warning This function does not check its arguments. In particular, it
    //! is assumed that the generators pointed to by \p first and \p last all
    //! have the same degree.
    template <typename Iterator1, typename Iterator2>
    [[nodiscard]] FroidurePin copy_closure_no_checks(Iterator1 first,
                                                     Iterator2 last);

    //! \brief Copy and add non-redundant generators.
    //!
    //! This function is equivalent to copy constructing a new FroidurePin
    //! instance and  then calling \ref closure on the copy. But this
    //! function avoids copying the parts of \c this that are immediately
    //! invalidated by \ref closure.
    //!
    //! \tparam Iterator1 the type of the first parameter.
    //! \tparam Iterator2 the type of the second parameter.
    //!
    //! \param first iterator pointing at the first generator to add.
    //! \param last iterator pointing one beyond the last generator to add.
    //!
    //! \returns A new FroidurePin instance by value generated by the
    //! generators of \c this and the non-redundant generators in the range from
    //! \p first to \p last.
    //!
    //! \throws LibsemigroupsException if any of the elements pointed to by \p
    //! first and \p last do not have degree compatible with any existing
    //! elements of the FroidurePin instance.
    //!
    //! \throws LibsemigroupsException if the elements pointed to by \p
    //! first and \p last do not all have the same degree.
    template <typename Iterator1, typename Iterator2>
    [[nodiscard]] FroidurePin copy_closure(Iterator1 first, Iterator2 last) {
      throw_if_degree_too_small(first, last);
      throw_if_inconsistent_degree(first, last);
      return copy_closure_no_checks(first, last);
    }

    // TODO(1) copy_closure(const_reference)
    // TODO(1) copy_closure_no_checks(const_reference)

    //! \brief Returns a std::shared_ptr to the state (if any).
    //!
    //! \returns
    //! std::shared_ptr to \ref state_type.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    std::shared_ptr<state_type> state() const {
      return _state;
    }

    //! \brief Throws if the degrees of elements in a given range are not all
    //! equal.
    //!
    //! This function throws a LibsemigroupsException if the elements in
    //! the range defined by the iterators \p first and \p last do not all have
    //! the same degree.
    //!
    //! \tparam Iterator1 the type of the first parameter.
    //! \tparam Iterator2 the type of the second parameter.
    //!
    //! \param first iterator pointing at the first generator to add.
    //! \param last iterator pointing one beyond the last generator to add.
    //!
    //! \throws LibsemigroupsException if the elements in the range defined by
    //! \p first and \p last do not all have the same degree.
    template <typename Iterator1, typename Iterator2>
    static void throw_if_inconsistent_degree(Iterator1 first, Iterator2 last);

   private:
    ////////////////////////////////////////////////////////////////////////
    // FroidurePin - validation member functions - private
    ////////////////////////////////////////////////////////////////////////

    void throw_if_bad_degree(const_reference) const;

    template <typename Iterator1, typename Iterator2>
    void throw_if_bad_degree(Iterator1, Iterator2) const;

    template <typename Iterator1, typename Iterator2>
    void throw_if_degree_too_small(Iterator1, Iterator2) const;

    ////////////////////////////////////////////////////////////////////////
    // FroidurePin - enumeration member functions - private
    ////////////////////////////////////////////////////////////////////////

    void expand(size_type);
    void is_one(internal_const_element_type x, element_index_type) noexcept(
        std::is_nothrow_default_constructible_v<InternalEqualTo>
        && noexcept(std::declval<InternalEqualTo>()(x, x)));

    void copy_generators_from_elements(size_t);
    void closure_update(element_index_type,
                        generator_index_type,
                        generator_index_type,
                        element_index_type,
                        size_type,
                        size_t const&,
                        std::vector<bool>&,
                        state_type*);

    void init_degree(const_reference);

    template <typename Iterator1, typename Iterator2>
    void add_generators_before_start(Iterator1, Iterator2);

    template <typename Iterator1, typename Iterator2>
    void add_generators_after_start(Iterator1, Iterator2);

    ////////////////////////////////////////////////////////////////////////
    // FroidurePin - initialisation member functions - private
    ////////////////////////////////////////////////////////////////////////

    void init_sorted();
    void init_idempotents();
    void idempotents(enumerate_index_type,
                     enumerate_index_type,
                     enumerate_index_type,
                     std::vector<internal_idempotent_pair>&);

    // Forward declarations - implemented in froidure-pin.tpp
    struct DerefPairFirst;
    struct AddressOfPairFirst;
    struct IteratorPairFirstTraits;

    ////////////////////////////////////////////////////////////////////////
    // FroidurePin - iterators - private
    ////////////////////////////////////////////////////////////////////////

    // A type for const iterators through (element, index) pairs of \c this.
    using const_iterator_pair_first
        = detail::ConstIteratorStateless<IteratorPairFirstTraits>;

   public:
    ////////////////////////////////////////////////////////////////////////
    // FroidurePin - iterators - public
    ////////////////////////////////////////////////////////////////////////
    //! \brief Return type of \ref cbegin and \ref cend.
    //!
    //! Return type for const random access iterators pointing at the elements
    //! of a FroidurePin object in the order they were enumerated (i.e. in
    //! short-lex order of the minimum word in the generators).
    using const_iterator
        = detail::BruidhinnConstIterator<element_type,
                                         std::vector<internal_element_type>>;

    //! \brief Return type of \ref cbegin_sorted and \ref cend_sorted.
    //!
    //! A type for const random access iterators through the elements, sorted
    //! according to Less.
    using const_iterator_sorted = const_iterator_pair_first;

    //! \brief Return type of \ref cbegin_idempotents and \ref
    //! cend_idempotents.
    //!
    //! A type for const random access iterators through the idempotents, in
    //! order of generation (short-lex order).
    //!
    //! \sa const_iterator.
    using const_iterator_idempotents = const_iterator_pair_first;

    //! \brief Returns a const iterator pointing to the first element (ordered
    //! by discovery).
    //!
    //! This function does not trigger any enumeration, and the returned
    //! iterators may be invalidated by any call to a non-const function of
    //! the FroidurePin class.
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
    [[nodiscard]] const_iterator cbegin() const;

    //! \brief Returns a const iterator pointing to the first element (ordered
    //! by discovery).
    //!
    //! This function does not trigger any enumeration, and the returned
    //! iterators may be invalidated by any call to a non-const function of
    //! the FroidurePin class.
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
    [[nodiscard]] const_iterator begin() const;

    //! \brief Returns a const iterator pointing to one past the last known
    //! element.
    //!
    //! This function does not trigger any enumeration, and the returned
    //! iterators may be invalidated by any call to a non-const function of
    //! the FroidurePin class.
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
    [[nodiscard]] const_iterator cend() const;

    //! \brief Returns a const iterator pointing one past the last known
    //! element.
    //!
    //! This function does not trigger any enumeration, and the returned
    //! iterators may be invalidated by any call to a non-const function of
    //! the FroidurePin class.
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
    [[nodiscard]] const_iterator end() const;

    //! \brief Returns a const iterator pointing to the first element (sorted
    //! by Less).
    //!
    //! \returns A value of type \ref const_iterator_sorted.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \note This function triggers a full enumeration.
    [[nodiscard]] const_iterator_sorted cbegin_sorted();

    //! \brief Returns a const iterator pointing one past the last element
    //! (sorted by Less).
    //!
    //! \returns A value of type \ref const_iterator_sorted.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \note This function triggers a full enumeration.
    [[nodiscard]] const_iterator_sorted cend_sorted();

    //! \brief Returns a const iterator pointing at the first idempotent.
    //!
    //! If the returned iterator is incremented, then it points to the second
    //! idempotent in the semigroup (if it exists), and every subsequent
    //! increment points to the next idempotent.
    //!
    //! \returns A value of type \ref const_iterator_idempotents.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \note This function triggers a full enumeration.
    [[nodiscard]] const_iterator_idempotents cbegin_idempotents();

    //! \brief Returns a const iterator pointing one past the last idempotent.
    //!
    //! \returns A value of type \ref const_iterator_idempotents.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \note This function triggers a full enumeration.
    [[nodiscard]] const_iterator_idempotents cend_idempotents();

   private:
    void run_impl() override;

    void report_progress();

    bool finished_impl() const override;
  };

  //! \ingroup froidure_pin_group
  //!
  //! Deduction guide for constructing FroidurePin<Element> where \c Element is
  //! the type pointed to by \c Iterator1 and \c Iterator2.
  template <typename Iterator1, typename Iterator2>
  FroidurePin(Iterator1, Iterator2)
      -> FroidurePin<std::decay_t<decltype(*std::declval<Iterator1>())>>;

  // Namespace doc is in froidure-pin-base.hpp
  namespace froidure_pin {

    //! \brief Re-initialize a FroidurePin object from a container of
    //! generators.
    //!
    //! This function re-initializes a FroidurePin object from the container of
    //! generators \p gens.
    //!
    //! \tparam Container the type of the container.
    //! \param fp the FroidurePin instance.
    //! \param gens the generators.
    //!
    //! \returns A const reference to the first parameter.
    //!
    //! \warning This function does not check its arguments. In particular, it
    //! is assumed that the generators in \p gens all have the same degree.
    template <typename Container>
    FroidurePin<typename Container::value_type>&
    init(FroidurePin<typename Container::value_type>& fp,
         Container const&                             gens) {
      return fp.init(std::begin(gens), std::end(gens));
    }

    // TODO(1) make the following work
    // template <typename Container>
    // FroidurePin<typename Container::value_type>&
    // init(FroidurePin<typename Container::value_type>& fp, Container&& gens) {
    //   return fp.init(std::make_move_iterator(std::begin(gens)),
    //                  std::make_move_iterator(std::end(gens)));
    // }

    // clang-format off
    // NOLINTNEXTLINE(whitespace/line_length)
    //! \copydoc init(FroidurePin<typename Container::value_type>&, Container const&)
    // clang-format on
    template <typename Element>
    [[nodiscard]] FroidurePin<Element>
    init(FroidurePin<Element>& fp, std::initializer_list<Element> gens) {
      return fp.init(std::begin(gens), std::end(gens));
    }

    //! \brief Add collection of generators from container.
    //!
    //! See \ref FroidurePin::add_generator for a detailed description.
    //!
    //! \tparam Container the type of the container for generators to add.
    //!
    //! \param fp the FroidurePin instance.
    //! \param coll the collection of generators to add.
    //!
    //! \returns A const reference to the first parameter.
    //!
    //! \throws LibsemigroupsException if the degree of \p x is incompatible
    //! with the existing degree of \p fp (if any).
    template <typename Container>
    FroidurePin<typename Container::value_type>&
    add_generators(FroidurePin<typename Container::value_type>& fp,
                   Container const&                             coll) {
      return fp.add_generators(std::begin(coll), std::end(coll));
    }

    //! \brief Add collection of generators from container.
    //!
    //! See \ref FroidurePin::add_generator_no_checks for a detailed
    //! description.
    //!
    //! \tparam Container the type of the container for generators to add.
    //!
    //! \param fp the FroidurePin instance.
    //! \param coll the collection of generators to add.
    //!
    //! \returns A const reference to the first parameter.
    //!
    //! \warning This function does not check its arguments. In particular, it
    //! is assumed that the generators in \p gens all have the same degree.
    template <typename Container>
    FroidurePin<typename Container::value_type>&
    add_generators_no_checks(FroidurePin<typename Container::value_type>& fp,
                             Container const& coll) {
      return fp.add_generators_no_checks(std::begin(coll), std::end(coll));
    }

    // TODO(1) make the following work
    // template <typename Container>
    // FroidurePin<typename Container::value_type>&
    // add_generators(FroidurePin<typename Container::value_type>& fp,
    //                Container&&                                  coll) {
    //   // Note that this currently doesn't do anything different than the
    //   // function above.
    //   return fp.add_generators(std::make_move_iterator(std::begin(coll)),
    //                            std::make_move_iterator(std::end(coll)));
    // }

    //! \brief Add collection of generators from initializer list.
    //!
    //! See \ref FroidurePin::add_generator for a detailed description.
    //!
    //! \tparam Element the type of the elements in the FroidurePin instance.
    //!
    //! \param fp the FroidurePin instance.
    //! \param coll the collection of generators to add.
    //!
    //! \returns A const reference to the first parameter.
    //!
    //! \throws LibsemigroupsException if the degree of \p x is incompatible
    //! with the existing degree of \p fp (if any).
    template <typename Element>
    FroidurePin<Element>& add_generators(FroidurePin<Element>&          fp,
                                         std::initializer_list<Element> coll) {
      return fp.add_generators(std::begin(coll), std::end(coll));
    }

    //! \brief Add collection of generators from initializer list.
    //!
    //! See \ref FroidurePin::add_generator_no_checks for a detailed
    //! description.
    //!
    //! \tparam Element the type of the elements in the FroidurePin instance.
    //!
    //! \param fp the FroidurePin instance.
    //! \param coll the collection of generators to add.
    //!
    //! \returns A const reference to the first parameter.
    //!
    //! \warning This function does not check its arguments. In particular, it
    //! is assumed that the generators in \p coll all have the same degree.
    template <typename Element>
    FroidurePin<Element>&
    add_generators_no_checks(FroidurePin<Element>&          fp,
                             std::initializer_list<Element> coll) {
      return fp.add_generators_no_checks(std::begin(coll), std::end(coll));
    }

    //! \brief Copy a FroidurePin instance and add a collection of generators
    //! from a container.
    //!
    //! See \ref FroidurePin::copy_add_generators for a detailed description.
    //!
    //! \tparam Container the type of the container for generators to add.
    //!
    //! \param fp the FroidurePin instance.
    //! \param coll the collection of generators to add.
    //!
    //! \returns A new FroidurePin instance generated by the generators of \p
    //! fp and \p coll.
    //!
    //! \throws LibsemigroupsException if the degree of any element in \p coll
    //! is incompatible with the existing degree of \p fp (if any).
    template <typename Container>
    [[nodiscard]] FroidurePin<typename Container::value_type>
    copy_add_generators(FroidurePin<typename Container::value_type> const& fp,
                        Container const& coll) {
      return fp.copy_add_generators(std::begin(coll), std::end(coll));
    }

    //! \brief Copy a FroidurePin instance and add a collection of generators
    //! from a container.
    //!
    //! See \ref FroidurePin::copy_add_generators_no_checks for a detailed
    //! description.
    //!
    //! \tparam Container the type of the container for generators to add.
    //!
    //! \param fp the FroidurePin instance.
    //! \param coll the collection of generators to add.
    //!
    //! \returns A new FroidurePin instance generated by the generators of \p
    //! fp and \p coll.
    //!
    //! \warning This function does not check its arguments. In particular, it
    //! is assumed that the generators in \p coll all have the same degree.
    template <typename Container>
    [[nodiscard]] FroidurePin<typename Container::value_type>
    copy_add_generators_no_checks(
        FroidurePin<typename Container::value_type> const& fp,
        Container const&                                   coll) {
      return fp.copy_add_generators_no_checks(std::begin(coll), std::end(coll));
    }

    //! \brief Copy a FroidurePin instance and add a collection of generators
    //! from std::initializer_list.
    //!
    //! See \ref FroidurePin::copy_add_generators for a detailed description.
    //!
    //! \tparam Container the type of the container for generators to add.
    //!
    //! \param fp the FroidurePin instance.
    //! \param coll the collection of generators to add.
    //!
    //! \returns A new FroidurePin instance generated by the generators of \p
    //! fp and \p coll.
    //!
    //! \throws LibsemigroupsException if the degree of any element in \p coll
    //! is incompatible with the existing degree of \p fp (if any).
    template <typename Element>
    [[nodiscard]] FroidurePin<Element>
    copy_add_generators(FroidurePin<Element> const&    fp,
                        std::initializer_list<Element> coll) {
      return fp.copy_add_generators(std::begin(coll), std::end(coll));
    }

    //! \brief Copy a FroidurePin instance and add a collection of generators
    //! from std::initializer_list.
    //!
    //! See \ref FroidurePin::copy_add_generators for a detailed description.
    //!
    //! \tparam Container the type of the container for generators to add.
    //!
    //! \param fp the FroidurePin instance.
    //! \param coll the collection of generators to add.
    //!
    //! \returns A new FroidurePin instance generated by the generators of \p
    //! fp and \p coll.
    //!
    //! \warning This function does not check its arguments. In particular, it
    //! is assumed that the generators in \p gens all have the same degree.
    template <typename Element>
    [[nodiscard]] FroidurePin<Element>
    copy_add_generators_no_checks(FroidurePin<Element> const&    fp,
                                  std::initializer_list<Element> coll) {
      return fp.copy_add_generators_no_checks(std::begin(coll), std::end(coll));
    }

    //! \brief Add non-redundant generators from a container.
    //!
    //! See \ref FroidurePin::closure for a detailed description.
    //!
    //! \tparam Container the type of the container for generators to add.
    //!
    //! \param fp the FroidurePin instance.
    //! \param coll the collection of generator to add.
    //!
    //! \returns A const reference to the first parameter.
    //!
    //! \throws LibsemigroupsException if the degree of \p x is incompatible
    //! with the existing degree of \p fp (if any).
    template <typename Container>
    FroidurePin<typename Container::value_type>&
    closure(FroidurePin<typename Container::value_type>& fp,
            Container const&                             coll) {
      return fp.closure(std::begin(coll), std::end(coll));
    }

    //! \brief Add non-redundant generators from a container.
    //!
    //! See \ref FroidurePin::closure_no_checks for a detailed description.
    //!
    //! \tparam Container the type of the container for generators to add.
    //!
    //! \param fp the FroidurePin instance.
    //! \param coll the collection of generator to add.
    //!
    //! \returns A const reference to the first parameter.
    //!
    //! \warning This function does not check its arguments. In particular, it
    //! is assumed that the generators in \p coll all have the same degree.
    template <typename Container>
    FroidurePin<typename Container::value_type>&
    closure_no_checks(FroidurePin<typename Container::value_type>& fp,
                      Container const&                             coll) {
      return fp.closure_no_checks(std::begin(coll), std::end(coll));
    }

    //! \brief Add non-redundant generators from a std::initializer_list.
    //!
    //! See \ref FroidurePin::closure for a detailed description.
    //!
    //! \tparam Element the type of the elements in the FroidurePin instance.
    //!
    //! \param fp the FroidurePin instance.
    //! \param coll the collection of generator to add.
    //!
    //! \returns A const reference to the first parameter.
    //!
    //! \throws LibsemigroupsException if the degree of \p x is incompatible
    //! with the existing degree of \p fp (if any).
    template <typename Element>
    FroidurePin<Element>& closure(FroidurePin<Element>&          fp,
                                  std::initializer_list<Element> coll) {
      return fp.closure(std::begin(coll), std::end(coll));
    }

    //! \brief Add non-redundant generators from a std::initializer_list.
    //!
    //! See \ref FroidurePin::closure for a detailed description.
    //!
    //! \tparam Element the type of the elements in the FroidurePin instance.
    //!
    //! \param fp the FroidurePin instance.
    //! \param coll the collection of generator to add.
    //!
    //! \returns A const reference to the first parameter.
    //!
    //! \warning This function does not check its arguments. In particular, it
    //! is assumed that the generators in \p coll all have the same degree.
    template <typename Element>
    FroidurePin<Element>&
    closure_no_checks(FroidurePin<Element>&          fp,
                      std::initializer_list<Element> coll) {
      return fp.closure_no_checks(std::begin(coll), std::end(coll));
    }

    //! \brief Copy and add non-redundant generators from a container.
    //!
    //! See \ref FroidurePin::copy_closure for a detailed description.
    //!
    //! \tparam Container the type of the container for generators to add.
    //!
    //! \param fp the FroidurePin instance.
    //! \param coll the collection of generators to add.
    //!
    //! \returns A new FroidurePin instance generated by the generators of \c
    //! this and the non-redundant generators from \p coll.
    //!
    //! \throws LibsemigroupsException if the degree of any element in \p coll
    //! is incompatible with the existing degree of \p fp (if any).
    template <typename Container>
    [[nodiscard]] FroidurePin<typename Container::value_type>
    copy_closure(FroidurePin<typename Container::value_type>& fp,
                 Container const&                             coll) {
      return fp.copy_closure(std::begin(coll), std::end(coll));
    }

    //! \brief Copy and add non-redundant generators from a container.
    //!
    //! See \ref FroidurePin::copy_closure_no_checks for a detailed description.
    //!
    //! \tparam Container the type of the container for generators to add.
    //!
    //! \param fp the FroidurePin instance.
    //! \param coll the collection of generators to add.
    //!
    //! \returns A new FroidurePin instance generated by the generators of \c
    //! this and the non-redundant generators from \p coll.
    //!
    //! \warning This function does not check its arguments. In particular, it
    //! is assumed that the generators in \p coll all have the same degree.
    template <typename Container>
    [[nodiscard]] FroidurePin<typename Container::value_type>
    copy_closure_no_checks(FroidurePin<typename Container::value_type>& fp,
                           Container const&                             coll) {
      return fp.copy_closure_no_checks(std::begin(coll), std::end(coll));
    }

    //! \brief Copy and add non-redundant generators from a
    //! std::initializer_list.
    //!
    //! See \ref FroidurePin::copy_closure for a detailed description.
    //!
    //! \tparam Element the type of the elements in the FroidurePin instance.
    //!
    //! \param fp the FroidurePin instance.
    //! \param coll the collection of generators to add.
    //!
    //! \returns A new FroidurePin instance generated by the generators of \c
    //! this and the non-redundant generators from \p coll.
    //!
    //! \throws LibsemigroupsException if the degree of any element in \p coll
    //! is incompatible with the existing degree of \p fp (if any).
    template <typename Element>
    [[nodiscard]] FroidurePin<Element>
    copy_closure(FroidurePin<Element>&          fp,
                 std::initializer_list<Element> coll) {
      return fp.copy_closure(std::begin(coll), std::end(coll));
    }

    //! \brief Copy and add non-redundant generators from a
    //! std::initializer_list.
    //!
    //! See \ref FroidurePin::copy_closure_no_checks for a detailed description.
    //!
    //! \tparam Element the type of the elements in the FroidurePin instance.
    //!
    //! \param fp the FroidurePin instance.
    //! \param coll the collection of generators to add.
    //!
    //! \returns A new FroidurePin instance generated by the generators of \c
    //! this and the non-redundant generators from \p coll.
    //!
    //! \warning This function does not check its arguments. In particular, it
    //! is assumed that the generators in \p coll all have the same degree.
    template <typename Element>
    [[nodiscard]] FroidurePin<Element>
    copy_closure_no_checks(FroidurePin<Element>&          fp,
                           std::initializer_list<Element> coll) {
      return fp.copy_closure(std::begin(coll), std::end(coll));
    }

    // TODO(1) implement the next function
    // \brief Returns a range object containing the so-far enumerated
    // idempotents.
    //
    // This function returns a range object containing the so-far enumerated
    // idempotents. No enumeration of \p fp is triggered by calls to this
    // function.
    //
    // See TODO(1) for more details about range objects.
    //
    // \tparam Element the type of the elements in the represented
    // semigroup
    //
    // \tparam Traits a traits class holding various adapters used by the
    // implementation (defaults to FroidurePinTraits).
    //
    // \param fp the FroidurePin instance.
    //
    // \returns A range object.
    //
    // is_idempotent_no_checks current performs a full enum. so this doesn't
    // work.
    //
    // template <typename Element, typename Traits>
    // [[nodiscard]] auto
    // current_idempotents(FroidurePin<Element, Traits> const& fp) {
    //   return rx::iterator_range(fp.cbegin(), fp.cend())
    //          | rx::filter([&fp](auto const& x) {
    //              return fp.is_idempotent_no_checks(fp.current_position(x));
    //            });
    // }

    //! \brief Returns a range object containing all of the idempotents.
    //!
    //! This function returns a range object wrapping
    //! FroidurePin::cbegin_idempotents and FroidurePin::cend_idempotents, after
    //! fully enumerating \p fp.
    //!
    //! See \ref ranges_group for more details about range objects.
    //!
    //! \tparam Element the type of the elements in the represented
    //! semigroup
    //!
    //! \tparam Traits a traits class holding various adapters used by the
    //! implementation (defaults to FroidurePinTraits).
    //!
    //! \param fp the FroidurePin instance.
    //!
    //! \returns A range object.
    template <typename Element, typename Traits>
    [[nodiscard]] auto idempotents(FroidurePin<Element, Traits>& fp) {
      fp.run();
      return current_idempotents(fp);
    }

    //! \brief Returns a range object containing all of the elements.
    //!
    //! This function returns a range object wrapping
    //! FroidurePin::cbegin_sorted and FroidurePin::cend_sorted, after fully
    //! enumerating \p fp.
    //!
    //! See \ref ranges_group for more details about range objects.
    //!
    //! \tparam Element the type of the elements in the represented
    //! semigroup
    //!
    //! \tparam Traits a traits class holding various adapters used by the
    //! implementation (defaults to FroidurePinTraits).
    //!
    //! \param fp the FroidurePin instance.
    //!
    //! \returns A range object.
    template <typename Element, typename Traits>
    [[nodiscard]] auto sorted_elements(FroidurePin<Element, Traits>& fp) {
      return rx::iterator_range(fp.cbegin_sorted(), fp.cend_sorted());
    }

    // TODO(1) current_sorted_elements

    //! \brief Returns a range object containing the so-far enumerated
    //! elements.
    //!
    //! This function returns a range object wrapping FroidurePin::cbegin and
    //! FroidurePin::cend, i.e. containing the so-far enumerated elements.
    //!
    //! See \ref ranges_group for more details about range objects.
    //!
    //! \tparam Element the type of the elements in the represented
    //! semigroup
    //!
    //! \tparam Traits a traits class holding various adapters used by the
    //! implementation (defaults to FroidurePinTraits).
    //!
    //! \param fp the FroidurePin instance.
    //!
    //! \returns A range object.
    template <typename Element, typename Traits>
    [[nodiscard]] auto
    current_elements(FroidurePin<Element, Traits> const& fp) {
      return rx::iterator_range(fp.cbegin(), fp.cend());
    }

    //! \brief Returns a range object containing all of the elements.
    //!
    //! This function returns a range object wrapping FroidurePin::cbegin and
    //! FroidurePin::cend, after fully enumerating \p fp.
    //!
    //! See \ref ranges_group for more details about range objects.
    //!
    //! \tparam Element the type of the elements in the represented
    //! semigroup
    //!
    //! \tparam Traits a traits class holding various adapters used by the
    //! implementation (defaults to FroidurePinTraits).
    //!
    //! \param fp the FroidurePin instance.
    //!
    //! \returns A range object.
    template <typename Element, typename Traits>
    [[nodiscard]] auto elements(FroidurePin<Element, Traits>& fp) {
      fp.run();
      return fp.current_elements();
    }

    //! \brief Convert a word in the generators to an element.
    //!
    //! This  function returns a reference to the element obtained by
    //! evaluating \p w. The returned reference may only valid until the next
    //! function that triggers an enumeration is called, or another call to this
    //! function is made.
    //!
    //! \param w the word in the generators to evaluate.
    //!
    //! \returns A const reference to the element represented by the word \p w.
    //!
    //! \warning This function does not check its arguments, and it is assumed
    //! that the values in \p w are less than \ref number_of_generators; and
    //! if \p w is empty it is assumed that \ref contains_one_no_run returns \c
    //! true (although nothing bad will happen if this doesn't hold, except that
    //! this function will return the identity element even though it might not
    //! be an element of the semigroup).
    //!
    //! \sa \ref current_position.
    // TODO(0) update doc
    template <typename Element, typename Traits, typename Word>
    [[nodiscard]] typename FroidurePin<Element, Traits>::const_reference
    to_element_no_checks(FroidurePin<Element, Traits> const& fp,
                         Word const&                         w) {
      return fp.to_element_no_checks(std::begin(w), std::end(w));
    }

    // TODO(0) doc
    template <typename Element, typename Traits, typename T = size_t>
    [[nodiscard]] typename FroidurePin<Element, Traits>::const_reference
    to_element_no_checks(FroidurePin<Element, Traits> const& fp,
                         std::initializer_list<T> const&     w) {
      return to_element_no_checks<Element, Traits, std::initializer_list<T>>(fp,
                                                                             w);
    }

    //! \brief Convert a word in the generators to an element.
    //!
    //! This  function returns a reference to the element obtained by
    //! evaluating \p w. The returned reference may only valid until the next
    //! function that triggers an enumeration is called, or another call to this
    //! function is made.
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
    // TODO(0) update doc
    template <typename Element, typename Traits, typename Word>
    [[nodiscard]] typename FroidurePin<Element, Traits>::const_reference
    to_element(FroidurePin<Element, Traits> const& fp, Word const& w) {
      return fp.to_element(std::begin(w), std::end(w));
    }

    // TODO(0) doc
    template <typename Element, typename Traits, typename T = size_t>
    [[nodiscard]] typename FroidurePin<Element, Traits>::const_reference
    to_element(FroidurePin<Element, Traits> const& fp,
               std::initializer_list<T> const&     w) {
      return to_element<Element, Traits, std::initializer_list<T>>(fp, w);
    }

  }  // namespace froidure_pin

  //! \defgroup converter_group Converters
  //!
  //! This page contains the documentation for the functions in
  //! ``libsemigroups`` for converting one type of object to another.
  //!
  //!  * \ref to_froidure_pin_group
  //!
  //  TODO(1) add details + links

  //! \defgroup to_froidure_pin_group to_froidure_pin
  //!
  //! This page contains the documentation for the function \ref to_froidure_pin
  //! in ``libsemigroups`` for converting objects to FroidurePin instances.

  //! \ingroup to_froidure_pin_group
  //!
  //! \brief Construct a FroidurePin iinstance from container of generators.
  //!
  //! This function can be used to construct a FroidurePin instance from a
  //! container of generators after first verifying that the
  //! proposed generators all have equal degree.
  //!
  //!
  //! \param gens the generators.
  //!
  //! \throw LibsemigroupsException if `Degree()(x) != Degree(y)` for any \c x
  //! and \c y in \p gens.
  template <typename Container>
  [[nodiscard]] FroidurePin<typename Container::value_type>
  to_froidure_pin(Container const& gens) {
    FroidurePin<typename Container::value_type>::throw_if_inconsistent_degree(
        std::begin(gens), std::end(gens));
    return FroidurePin(std::begin(gens), std::end(gens));
  }

  // TODO(1) make the following work
  // template <typename Container>
  // FroidurePin<typename Container::value_type>
  // to_froidure_pin(Container&& gens) {
  //   return FroidurePin(std::make_move_iterator(std::begin(gens)),
  //                      std::make_move_iterator(std::end(gens)));
  // }

  //! \ingroup to_froidure_pin_group
  //!
  //! \brief Construct a FroidurePin instance from std::initializer_list of
  //! generators.
  //!
  //! This function can be used to construct a FroidurePin instance from a
  //! std::initializer_list of generators after first verifying that the
  //! proposed generators all have equal degree. See \ref
  //! FroidurePin::add_generator for more details.
  //!
  //! \tparam Element the type of the elements in the FroidurePin instance.
  //!
  //! \param gens the std::initializer_list of generators.
  //!
  //! \returns A newly constructed FroidurePin instance with generators equal to
  //! \p gens.
  //!
  //! \throw LibsemigroupsException if `Degree()(x) != Degree(y)` for any \c x
  //! and \c y in \p gens.
  template <typename Element>
  [[nodiscard]] FroidurePin<Element>
  to_froidure_pin(std::initializer_list<Element> gens) {
    FroidurePin<Element>::throw_if_inconsistent_degree(std::begin(gens),
                                                       std::end(gens));
    return FroidurePin(std::begin(gens), std::end(gens));
  }

  //! \ingroup to_froidure_pin_group
  //!
  //! \brief Construct a FroidurePin instance from range of elements given by
  //! iterators.
  //!
  //! This function can be used to construct a FroidurePin instance from a the
  //! range of generators given by the iterators \p first and \p last, after
  //! first verifying that the proposed generators all have equal degree. See
  //! \ref FroidurePin::add_generator for more details.
  //!
  //! \tparam Iterator1 the type of the first parameter.
  //! \tparam Iterator2 the type of the second parameter.
  //!
  //! \param first iterator pointing at the first generator to add.
  //! \param last iterator pointing one beyond the last generator to add.
  //!
  //! \returns A newly constructed FroidurePin instance with generators given
  //! by \p first and \p last.
  //!
  //! \throw LibsemigroupsException if `Degree()(x) != Degree(y)` for any of
  //! the proposed generators \c x and \c y.
  template <typename Iterator1, typename Iterator2>
  [[nodiscard]] FroidurePin<std::decay_t<decltype(*std::declval<Iterator1>())>>
  to_froidure_pin(Iterator1 first, Iterator2 last) {
    using Element = std::decay_t<decltype(*std::declval<Iterator1>())>;
    static_assert(
        std::is_same_v<std::decay_t<decltype(*std::declval<Iterator2>())>,
                       Element>);
    FroidurePin<Element>::throw_if_inconsistent_degree(first, last);
    return FroidurePin(first, last);
  }

  //! \ingroup froidure_pin_group
  //!
  //! \brief Return a human readable representation of a FroidurePin object.
  //!
  //! Return a human readable representation of a FroidurePin object.
  //!
  //! \tparam Element the type of the elements in the represented
  //! semigroup.
  //!
  //! \tparam Traits a traits class holding various adapters used by the
  //! implementation.
  //!
  //! \param fp the FroidurePin object.
  //!
  //! \returns A string containing a human readable representation of \p fp.
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  template <typename Element, typename Traits>
  [[nodiscard]] std::string
  to_human_readable_repr(FroidurePin<Element, Traits> const& fp);

}  // namespace libsemigroups

#include "froidure-pin.tpp"

#endif  // LIBSEMIGROUPS_FROIDURE_PIN_HPP_
