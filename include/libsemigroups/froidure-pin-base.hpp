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

#ifndef LIBSEMIGROUPS_FROIDURE_PIN_BASE_HPP_
#define LIBSEMIGROUPS_FROIDURE_PIN_BASE_HPP_

#include <cstddef>     // for size_t
#include <functional>  // for function
#include <thread>      // for thread::hardware_concurrency

#include "constants.hpp"   // for LIMIT_MAX
#include "containers.hpp"  // for DynamicArray2
#include "deprecated.hpp"  // for LIBSEMIGROUPS_DEPRECATED
#include "runner.hpp"      // for Runner
#include "types.hpp"       // for word_type, letter_type, tril

namespace libsemigroups {

  //! Defined in ``froidure-pin-base.hpp``.
  //!
  //! FroidurePinBase is an abstract base class for the class template
  //! FroidurePin.
  //!
  //! FroidurePinBase allows a polymorphic interface to instances of
  //! FroidurePin and its member functions reflect those member functions of
  //! FroidurePin that do not depend on the template parameter ``TElementType``.
  //!
  //! \sa FroidurePin and FroidurePinTraits.
  class FroidurePinBase : public Runner {
    template <typename TElementType, typename TTraits>
    friend class FroidurePin;

   public:
    //! Type used for indexing elements in a FroidurePin.
    // It should be possible to change this type and everything will just work,
    // provided the size of the semigroup is less than the maximum value of
    // this type of integer.
    using size_type = size_t;

    //! Type for the position of an element in an instance of FroidurePin. The
    //! size of the semigroup being enumerated must be at most
    //! \c std::numeric_limits<element_index_type>::max()
    using element_index_type = size_type;

    //! Type for a left or right Cayley graph of a FroidurePin instance.
    using cayley_graph_type = detail::DynamicArray2<element_index_type>;

   private:
    // See comments in FroidurePin
    using enumerate_index_type = size_type;

   public:
    ////////////////////////////////////////////////////////////////////////
    // FroidurePinBase - constructors - public
    ////////////////////////////////////////////////////////////////////////

    //! Default constructor.
    FroidurePinBase();

    //! Copy constructor.
    FroidurePinBase(FroidurePinBase const& S);

    //! Default move constructor.
    FroidurePinBase(FroidurePinBase&& other) = default;

    //! FroidurePinBase instances are not copy assignable and this function is
    //! deleted.
    FroidurePinBase& operator=(FroidurePinBase const&) = delete;

    //! FroidurePinBase instances are not move assignable and this function is
    //! deleted.
    FroidurePinBase& operator=(FroidurePinBase&&) = delete;

    virtual ~FroidurePinBase() = default;

    ////////////////////////////////////////////////////////////////////////
    // FroidurePinBase - settings - public
    ////////////////////////////////////////////////////////////////////////

    //! Set a new value for the batch size.
    //!
    //! The *batch size* is the number of new elements to be found by any call
    //! to FroidurePin::run. A call to run returns between 0 and
    //! approximately the batch size.
    //! This is used by, for example, FroidurePin::position so that it is
    //! possible to find the position of an element without fully enumerating
    //! the semigroup.
    //!
    //! The default value of the batch size is **8192**.
    //!
    //! \param batch_size the new value for the batch size.
    //!
    //! \returns A reference to \c this.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    //!
    //! \sa
    //! batch_size().
    FroidurePinBase& batch_size(size_t batch_size) noexcept;

    //! Returns the current value of the batch size.
    //!
    //! \returns
    //! A `size_t`.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    //!
    //! \sa This is the minimum number of elements enumerated in any call to
    //! FroidurePin::run, see batch_size(size_t).
    //!
    //! \par Parameters
    //! None.
    size_t batch_size() const noexcept;

    //! Set the maximum number of threads that any member function of an
    //! instance of FroidurePin can use.
    //!
    //! This member function sets the maximum number of threads to be used by
    //! any member function of a FroidurePin object. The number of threads is
    //! limited to the maximum of 1 and the minimum of \p nr_threads and the
    //! number of threads supported by the hardware.
    //!
    //! The default value is **std::thread::hardware_concurrency()**.
    //!
    //! \param nr_threads the maximum number of threads to use.
    //!
    //! \returns A reference to \c this.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    //!
    //! \sa
    //! max_threads().
    FroidurePinBase& max_threads(size_t nr_threads) noexcept;

    //! Returns the current value of the maximum number of threads.
    //!
    //! \returns
    //! A `size_t`.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    //!
    //! \sa
    //! max_threads(size_t).
    //!
    //! \par Parameters
    //! None.
    size_t max_threads() const noexcept;

    //! Set the threshold for concurrency to be used by member functions.
    //!
    //! This member function sets the threshold such that if `this->size()`
    //! exceeds this value, then the following functions may use a concurrent
    //! implementation:
    //!
    //! * FroidurePin::nr_idempotents
    //!
    //! The default value is **823543**.
    //!
    //! \param thrshld the new threshold.
    //!
    //! \returns A reference to \c this.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    //!
    //! \sa
    //! concurrency_threshold().
    FroidurePinBase& concurrency_threshold(size_t thrshld) noexcept;

    //! Returns the current value of the concurrency threshold.
    //!
    //! \returns
    //! A `size_t`.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    //!
    //! \sa
    //! concurrency_threshold(size_t).
    //!
    //! \par Parameters
    //! None.
    size_t concurrency_threshold() const noexcept;

    //! Prevent further changes to the mathematical semigroup represented by an
    //! instance of FroidurePinBase.
    //!
    //! This member function prevents certain member functions from being
    //! applied to a FroidurePinBase, such as add_generators, if they would
    //! change the mathematical object represented by \c this.
    //!
    //! The default value is **false**.
    //!
    //! \param val the new threshold.
    //!
    //! \returns A reference to \c this.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    //!
    //! \sa
    //! immutable().
    FroidurePinBase& immutable(bool val) noexcept;

    //! Returns the current value of mutability of \c this.
    //!
    //! \returns
    //! A `bool`.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    //!
    //! \sa
    //! immutable(bool).
    //!
    //! \par Parameters
    //! None.
    bool immutable() const noexcept;

    ////////////////////////////////////////////////////////////////////////
    // FroidurePinBase - pure virtual member functions - public
    ////////////////////////////////////////////////////////////////////////

    //! \copydoc FroidurePin::equal_to
    virtual bool equal_to(word_type const&, word_type const&) const = 0;

    //! \copydoc FroidurePin::nr_generators
    virtual size_t nr_generators() const = 0;

    //! \copydoc FroidurePin::fast_product
    virtual element_index_type fast_product(element_index_type,
                                            element_index_type) const = 0;
    //! \copydoc FroidurePin::nr_idempotents
    virtual size_t nr_idempotents() = 0;

    //! \copydoc FroidurePin::is_idempotent
    virtual bool is_idempotent(element_index_type) = 0;

    //! \copydoc FroidurePin::is_finite
    virtual tril is_finite() const = 0;

    //! \copydoc FroidurePin::reserve
    virtual void reserve(size_t) = 0;

    //! \copydoc FroidurePin::position_to_sorted_position
    virtual element_index_type position_to_sorted_position(element_index_type)
        = 0;

    ////////////////////////////////////////////////////////////////////////
    // FroidurePinBase - member functions - public
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
    element_index_type word_to_pos(word_type const& w) const;

    //! Returns the maximum length of a word in the generators so far computed.
    //!
    //! Every elements of the semigroup can be expressed as a product of the
    //! generators. The elements of the semigroup are enumerated in the
    //! short-lex order induced by the order of the generators (as passed to
    //! FroidurePin::FroidurePin).  This member function returns the length of
    //! the longest word in the generators that has so far been enumerated.
    size_t current_max_word_length() const noexcept {
      return _length[_enumerate_order.back()];
    }

    //! Returns the degree of any and all elements.
    size_t degree() const noexcept {
      return _degree;
    }

    //! Returns the number of elements in the semigroup that have been
    //! enumerated so far.
    //!
    //! This is only the actual size of the semigroup if the semigroup is fully
    //! enumerated.
    size_t current_size() const noexcept {
      return _nr;
    }

    //! Returns the number of relations in the presentation for the semigroup
    //! that have been found so far.
    //!
    //! This is only the actual number of relations in a presentation defining
    //! the semigroup if the semigroup is fully enumerated.
    size_t current_nr_rules() const noexcept {
      return _nr_rules;
    }

    //! Returns the position of the prefix of the element \c x in position
    //! \p pos (of the semigroup) of length one less than the length of \c x.
    //!
    //! The parameter \p pos must be a valid position of an already enumerated
    //! element of the semigroup, or a LibsemigroupsException will be thrown.
    element_index_type prefix(element_index_type pos) const {
      validate_element_index(pos);
      return _prefix[pos];
    }

    //! Returns the position of the suffix of the element \c x in position
    //! \p pos (of the semigroup) of length one less than the length of \c x.
    //!
    //! The parameter \p pos must be a valid position of an already enumerated
    //! element of the semigroup, or a LibsemigroupsException will be thrown.
    element_index_type suffix(element_index_type pos) const {
      validate_element_index(pos);
      return _suffix[pos];
    }

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
    letter_type first_letter(element_index_type pos) const {
      validate_element_index(pos);
      return _first[pos];
    }

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
    letter_type final_letter(element_index_type pos) const {
      validate_element_index(pos);
      return _final[pos];
    }

    //! Returns the length of the element in position \c pos of the semigroup.
    //!
    //! The parameter \p pos must be a valid position of an already enumerated
    //! element of the semigroup, or a LibsemigroupsException will be thrown.
    //! This member function causes no enumeration of the semigroup.
    size_t length_const(element_index_type pos) const {
      validate_element_index(pos);
      return _length[pos];
    }

    //! Returns the length of the element in position \c pos of the semigroup.
    //!
    //! The parameter \p pos must be a valid position of an element of the
    //! semigroup, or a LibsemigroupsException will be thrown.
    size_t length_non_const(element_index_type pos) {
      if (pos >= _nr) {
        run();
      }
      return length_const(pos);
    }

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
                                            element_index_type) const;

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
    element_index_type letter_to_pos(letter_type i) const {
      validate_letter_index(i);
      return _letter_to_pos[i];
    }

    //! Returns the size of the semigroup.
    size_t size() {
      run();
      return _nr;
    }

    //! Check if the semigroup represented by \c this is a monoid.
    //!
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
    bool is_monoid() {
      run();
      return _found_one;
    }

    //! Returns the total number of relations in the presentation defining the
    //! semigroup.
    //!
    //! \sa FroidurePinBase::cbegin_rules and FroidurePinBase::cend_rules.
    size_t nr_rules() {
      run();
      return _nr_rules;
    }

    //! Returns the index of the product of the element in position \p i with
    //! the generator with index \p j.
    //!
    //! This member function fully enumerates the semigroup.
    element_index_type right(element_index_type i, letter_type j) {
      validate_letter_index(j);
      run();
      validate_element_index(i);
      return _right.get(i, j);
    }

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
    cayley_graph_type const& right_cayley_graph() {
      run();
      _right.shrink_rows_to(size());
      return _right;
    }

    //! Returns the index of the product of the generator with index \p j and
    //! the element in position \p i.
    //!
    //! This member function fully enumerates the semigroup.
    //!
    //! \throws LibsemigroupsException if \p i or \p j is out of bounds.
    element_index_type left(element_index_type i, letter_type j) {
      validate_letter_index(j);
      run();
      validate_element_index(i);
      return _left.get(i, j);
    }

    //! Returns a const reference to the left Cayley graph of \c this.
    //!
    //! \returns A const reference to `cayley_graph_type`.
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
    cayley_graph_type const& left_cayley_graph() {
      run();
      _left.shrink_rows_to(size());
      return _left;
    }

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
    void minimal_factorisation(word_type& word, element_index_type pos) {
      if (pos >= _nr && !finished()) {
        enumerate(pos + 1);
      }
      validate_element_index(pos);
      // call the const version below
      private_minimal_factorisation(word, pos);
    }

    //! Returns a minimal libsemigroups::word_type which evaluates to the
    //! TElementType in position \p pos of \c this.
    //!
    //! This is the same as the two-argument member function for
    //! FroidurePin::minimal_factorisation, but it returns a pointer to the
    //! factorisation instead of modifying an argument in-place.
    //! If \p pos is greater than the size of the semigroup, then a
    //! LibsemigroupsException is thrown.
    word_type minimal_factorisation(element_index_type pos) {
      word_type word;
      minimal_factorisation(word, pos);
      return word;
    }

    //! Changes \p word in-place to contain a short-lex minimal word in the
    //! generators equal to the \p pos element of the semigroup.
    //!
    //! If \p pos is greater than the size of the semigroup, then a
    //! LibsemigroupsException is thrown.
    void minimal_factorisation(word_type& word, element_index_type pos) const {
      private_minimal_factorisation(word, pos);
    }

    //! Changes \p word in-place to contain a word in the generators equal to
    //! the \p pos element of the semigroup.
    //!
    //! The key difference between this member function and
    //! FroidurePin::minimal_factorisation(word_type& word, element_index_type
    //! pos), is that the resulting factorisation may not be minimal. If \p pos
    //! is greater than the size of the semigroup, then a
    //! LibsemigroupsException is thrown.
    void factorisation(word_type& word, element_index_type pos) {
      minimal_factorisation(word, pos);
    }

    //! Returns a libsemigroups::word_type which evaluates to the TElementType
    //! in position \p pos of \c this.
    //!
    //! The key difference between this member function and
    //! FroidurePin::minimal_factorisation(element_index_type pos), is that the
    //! resulting factorisation may not be minimal.
    //! If \p pos is greater than the size of the semigroup, then a
    //! LibsemigroupsException is thrown.
    word_type factorisation(element_index_type pos) {
      return minimal_factorisation(pos);
    }

    // This member function resets FroidurePin::next_relation so that when it
    // is next called the resulting relation is the first one.
    //
    // After a call to this function, the next call to
    // FroidurePin::next_relation will return the first relation of the
    // presentation defining the semigroup.
    //
    // \deprecated Use FroidurePin::cbegin_rules and FroidurePin::cend_rules
    // instead.
    //! No doc
    LIBSEMIGROUPS_DEPRECATED void reset_next_relation() noexcept {
      _relation_pos = UNDEFINED;
      _relation_gen = 0;
    }

    // This member function changes \p relation in-place to contain the next
    // relation of the presentation defining \c this.
    //
    // This member function changes \p relation in-place so that one of the
    // following holds:
    //
    // * \p relation is a vector consisting of a libsemigroups::letter_type and
    // a libsemigroups::letter_type such that FroidurePin::generator(\c
    // relation[\c 0]) == FroidurePin::generator(\c relation[\c 1]), i.e. if
    // the semigroup was defined with duplicate generators;
    //
    // * \p relation is a vector consisting of a
    // libsemigroups::element_index_type, libsemigroups::letter_type, and
    // libsemigroups::element_index_type such that
    // \code{.cpp}
    //   this[relation[0]] * FroidurePin::generator(relation[1]) ==
    //   this[relation[2]]
    // \endcode
    //
    // * \p relation is empty if there are no more relations.
    //
    // FroidurePin::next_relation is guaranteed to output all relations of
    // length 2 before any relations of length 3. If called repeatedly after
    // FroidurePin::reset_next_relation, and until relation is empty, the
    // values placed in \p relation correspond to a length-reducing confluent
    // rewriting system that defines the semigroup.
    //
    // This member function can be used in conjunction with
    // FroidurePin::factorisation to obtain a presentation defining the
    // semigroup.
    //
    // \deprecated Use FroidurePin::cbegin_rules and FroidurePin::cend_rules
    // instead.
    // Deprecated, remove in v2
    //! No doc
    LIBSEMIGROUPS_DEPRECATED void next_relation(word_type& relation);

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
    void enumerate(size_t limit);

    //! No doc
    class const_rule_iterator {
     public:
      //! No doc
      using size_type = typename std::vector<relation_type>::size_type;
      //! No doc
      using difference_type =
          typename std::vector<relation_type>::difference_type;
      //! No doc
      using const_pointer = typename std::vector<relation_type>::const_pointer;
      //! No doc
      using pointer = typename std::vector<relation_type>::pointer;
      //! No doc
      using const_reference =
          typename std::vector<relation_type>::const_reference;
      //! No doc
      using reference = typename std::vector<relation_type>::reference;
      //! No doc
      using value_type = relation_type;
      //! No doc
      using iterator_category = std::forward_iterator_tag;

      // None of the constructors are noexcept because the corresponding
      // constructors for std::vector aren't (until C++17).
      //! No doc
      const_rule_iterator() = default;
      //! No doc
      const_rule_iterator(const_rule_iterator const&) = default;
      //! No doc
      const_rule_iterator(const_rule_iterator&&) = default;
      //! No doc
      const_rule_iterator& operator=(const_rule_iterator const&) = default;
      //! No doc
      const_rule_iterator& operator=(const_rule_iterator&&) = default;

      //! No doc
      const_rule_iterator(FroidurePinBase const* ptr,
                          enumerate_index_type   pos,
                          letter_type            gen)
          : _current(),
            _froidure_pin(ptr),
            _gen(gen),
            _pos(pos),
            _relation({}, {}) {
        ++(*this);
      }

      //! No doc
      ~const_rule_iterator() = default;

      //! No doc
      bool operator==(const_rule_iterator const& that) const noexcept {
        return _gen == that._gen && _pos == that._pos;
      }

      //! No doc
      bool operator!=(const_rule_iterator const& that) const noexcept {
        return !(this->operator==(that));
      }

      //! No doc
      const_reference operator*() const noexcept {
        populate_relation();
        return _relation;
      }

      //! No doc
      const_pointer operator->() const noexcept {
        populate_relation();
        return &_relation;
      }

      //! No doc
      // prefix
      const_rule_iterator const& operator++() noexcept {
        auto const* ptr = _froidure_pin;

        if (_pos == ptr->current_size()) {  // no more relations
          return *this;
        }

        _relation.first.clear();
        _relation.second.clear();

        if (_pos != UNDEFINED) {
          while (_pos < ptr->current_size()) {
            while (_gen < ptr->nr_generators()) {
              if (!ptr->_reduced.get(ptr->_enumerate_order[_pos], _gen)
                  && (_pos < ptr->_lenindex[1]
                      || ptr->_reduced.get(
                          ptr->_suffix[ptr->_enumerate_order[_pos]], _gen))) {
                _current[0] = ptr->_enumerate_order[_pos];
                _current[1] = _gen;
                _current[2]
                    = ptr->_right.get(ptr->_enumerate_order[_pos], _gen);
                if (_current[2] != UNDEFINED) {
                  _gen++;
                  return *this;
                }
              }
              _gen++;
            }
            _gen = 0;
            _pos++;
          }
          return *this;
        } else {
          // duplicate generators
          if (_gen < ptr->_duplicate_gens.size()) {
            _current[0] = ptr->_duplicate_gens[_gen].first;
            _current[1] = ptr->_duplicate_gens[_gen].second;
            _current[2] = UNDEFINED;
            _gen++;
            return *this;
          }
          _gen = 0;
          _pos = 0;
          return operator++();
        }
      }

      //! No doc
      // postfix
      const_rule_iterator operator++(int) noexcept {
        const_rule_iterator copy(*this);
        ++(*this);
        return copy;
      }

      //! No doc
      void swap(const_rule_iterator& that) noexcept {
        _current.swap(that._current);
        std::swap(_froidure_pin, that._froidure_pin);
        std::swap(_gen, that._gen);
        std::swap(_pos, that._pos);
      }

     private:
      void populate_relation() const {
        if (_relation.first.empty()) {
          if (_current[2] == UNDEFINED) {
            _relation.first  = word_type({_current[0]});
            _relation.second = word_type({_current[1]});
          } else {
            _froidure_pin->minimal_factorisation(_relation.first, _current[0]);
            _relation.first.push_back(_current[1]);
            _froidure_pin->minimal_factorisation(_relation.second, _current[2]);
          }
        }
      }

      std::array<letter_type, 3> _current;
      FroidurePinBase const*     _froidure_pin;
      letter_type                _gen;
      enumerate_index_type       _pos;
      mutable relation_type      _relation;
    };  // const_rule_iterator

    // Assert that the forward iterator requirements are met
    static_assert(std::is_default_constructible<const_rule_iterator>::value,
                  "forward iterator requires default-constructible");
    static_assert(std::is_copy_constructible<const_rule_iterator>::value,
                  "forward iterator requires copy-constructible");
    static_assert(std::is_copy_assignable<const_rule_iterator>::value,
                  "forward iterator requires copy-assignable");
    static_assert(std::is_destructible<const_rule_iterator>::value,
                  "forward iterator requires destructible");

    //! Returns a forward iterator pointing to the first rule in a confluent
    //! terminating rewriting system defining a semigroup isomorphic to the
    //! one defined by \c this.
    //!
    //! This function does not perform any enumeration of the FroidurePin. If
    //! you want to obtain the complete set of rules, then it is necessary to
    //! FroidurePin::run until the FroidurePin is fully enumerated.
    //!
    //! \returns An iterator of type const_rule_iterator pointing to a
    //! libsemigroups::relation_type.
    //!
    //! \par Parameters
    //! (None).
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Constant
    //!
    //! \iterator_validity
    //! The iterators returned by this are valid until \c this is deleted.
    //!
    //! \sa cend_rules
    //!
    // clang-format off
    //! \par Example
    //! \code
    //! FroidurePin<BMat8> S;
    //! S.add_generator(BMat8({{1, 0, 0, 0},
    //!                        {1, 0, 0, 0},
    //!                        {1, 0, 0, 0},
    //!                        {1, 0, 0, 0}}));
    //! S.add_generator(BMat8({{0, 1, 0, 0},
    //!                        {0, 1, 0, 0},
    //!                        {0, 1, 0, 0},
    //!                        {0, 1, 0, 0}}));
    //! S.add_generator(BMat8({{0, 0, 1, 0},
    //!                        {0, 0, 1, 0},
    //!                        {0, 0, 1, 0},
    //!                        {0, 0, 1, 0}}));
    //! S.add_generator(BMat8({{0, 0, 0, 1},
    //!                        {0, 0, 0, 1},
    //!                        {0, 0, 0, 1},
    //!                        {0, 0, 0, 1}}));
    //! S.size(); // 4
    //! std::vector<relation_type>(S.cbegin_rules(), S.cend_rules());
    //! // {{{0, 0}, {0}},
    //! //  {{0, 1}, {1}},
    //! //  {{0, 2}, {2}},
    //! //  {{0, 3}, {3}},
    //! //  {{1, 0}, {0}},
    //! //  {{1, 1}, {1}},
    //! //  {{1, 2}, {2}},
    //! //  {{1, 3}, {3}},
    //! //  {{2, 0}, {0}},
    //! //  {{2, 1}, {1}},
    //! //  {{2, 2}, {2}},
    //! //  {{2, 3}, {3}},
    //! //  {{3, 0}, {0}},
    //! //  {{3, 1}, {1}},
    //! //  {{3, 2}, {2}},
    //! //  {{3, 3}, {3}}}
    //! \endcode
    // clang-format on
    const_rule_iterator cbegin_rules() const {
      return const_rule_iterator(this, UNDEFINED, 0);
    }

    //! Returns a forward iterator pointing one-past-the-last rule (currently
    //! known) in a confluent terminating rewriting system defining a semigroup
    //! isomorphic to the one defined by \c this.
    //!
    //! This function does not perform any enumeration of the FroidurePin. If
    //! you want to obtain the complete set of rules, then it is necessary to
    //! FroidurePin::run until the FroidurePin is fully enumerated.
    //!
    //! \returns An iterator of type const_rule_iterator pointing to a
    //! libsemigroups::relation_type.
    //!
    //! \par Parameters
    //! (None).
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Constant
    //!
    //! \iterator_validity
    //! The iterators returned by this are valid until \c this is deleted.
    //!
    //! \sa cbegin_rules
    //!
    // clang-format off
    //! \par Example
    //! \code
    //! FroidurePin<BMat8> S;
    //! S.add_generator(BMat8({{1, 0, 0, 0},
    //!                        {1, 0, 0, 0},
    //!                        {1, 0, 0, 0},
    //!                        {1, 0, 0, 0}}));
    //! S.add_generator(BMat8({{0, 1, 0, 0},
    //!                        {0, 1, 0, 0},
    //!                        {0, 1, 0, 0},
    //!                        {0, 1, 0, 0}}));
    //! S.add_generator(BMat8({{0, 0, 1, 0},
    //!                        {0, 0, 1, 0},
    //!                        {0, 0, 1, 0},
    //!                        {0, 0, 1, 0}}));
    //! S.add_generator(BMat8({{0, 0, 0, 1},
    //!                        {0, 0, 0, 1},
    //!                        {0, 0, 0, 1},
    //!                        {0, 0, 0, 1}}));
    //! S.size(); // 4
    //! std::vector<relation_type>(S.cbegin_rules(), S.cend_rules());
    //! // {{{0, 0}, {0}},
    //! //  {{0, 1}, {1}},
    //! //  {{0, 2}, {2}},
    //! //  {{0, 3}, {3}},
    //! //  {{1, 0}, {0}},
    //! //  {{1, 1}, {1}},
    //! //  {{1, 2}, {2}},
    //! //  {{1, 3}, {3}},
    //! //  {{2, 0}, {0}},
    //! //  {{2, 1}, {1}},
    //! //  {{2, 2}, {2}},
    //! //  {{2, 3}, {3}},
    //! //  {{3, 0}, {0}},
    //! //  {{3, 1}, {1}},
    //! //  {{3, 2}, {2}},
    //! //  {{3, 3}, {3}}}
    //! \endcode
    // clang-format on
    const_rule_iterator cend_rules() const {
      return const_rule_iterator(this, current_size(), 0);
    }

   private:
    ////////////////////////////////////////////////////////////////////////
    // FroidurePin - constructor helpers - private
    ////////////////////////////////////////////////////////////////////////
    void partial_copy(FroidurePinBase const& S);

    ////////////////////////////////////////////////////////////////////////
    // FroidurePin - ? - private
    ////////////////////////////////////////////////////////////////////////

    void private_minimal_factorisation(word_type&         word,
                                       element_index_type pos) const {
      word.clear();
      while (pos != UNDEFINED) {
        word.push_back(_first[pos]);
        pos = _suffix[pos];
      }
    }

    ////////////////////////////////////////////////////////////////////////
    // FroidurePin - validation member functions - private
    ////////////////////////////////////////////////////////////////////////

    void validate_element_index(element_index_type i) const {
      if (i >= _nr) {
        LIBSEMIGROUPS_EXCEPTION(
            "element index out of bounds, expected value in [0, %d), got %d",
            _nr,
            i);
      }
    }

    void validate_letter_index(letter_type i) const {
      if (i >= nr_generators()) {
        LIBSEMIGROUPS_EXCEPTION(
            "generator index out of bounds, expected value in [0, %d), got %d",
            nr_generators(),
            i);
      }
    }

    ////////////////////////////////////////////////////////////////////////
    // FroidurePin - settings data - private
    ////////////////////////////////////////////////////////////////////////
    struct Settings {
      Settings() noexcept
          : _batch_size(8192),
            _concurrency_threshold(823543),
            _max_threads(std::thread::hardware_concurrency()),
            _immutable(false) {}
      Settings(Settings const&) noexcept = default;
      Settings(Settings&&) noexcept      = default;
      ~Settings()                        = default;
      size_t _batch_size;
      size_t _concurrency_threshold;
      size_t _max_threads;
      bool   _immutable;
    } _settings;

    size_t                                           _degree;
    std::vector<std::pair<letter_type, letter_type>> _duplicate_gens;
    std::vector<element_index_type>                  _enumerate_order;
    std::vector<letter_type>                         _final;
    std::vector<letter_type>                         _first;
    bool                                             _found_one;
    bool                                             _idempotents_found;
    std::vector<int>                                 _is_idempotent;
    cayley_graph_type                                _left;
    std::vector<size_type>                           _length;
    std::vector<enumerate_index_type>                _lenindex;
    std::vector<element_index_type>                  _letter_to_pos;
    size_type                                        _nr;
    size_t                                           _nr_rules;
    enumerate_index_type                             _pos;
    element_index_type                               _pos_one;
    std::vector<element_index_type>                  _prefix;
    detail::DynamicArray2<bool>                      _reduced;
    cayley_graph_type                                _right;
    std::vector<element_index_type>                  _suffix;
    size_t                                           _wordlen;

    // Deprecated remove in v2
    letter_type          _relation_gen;
    enumerate_index_type _relation_pos;
  };

  //! No doc
  LIBSEMIGROUPS_DEPRECATED void
  relations(FroidurePinBase&                            S,
            std::function<void(word_type, word_type)>&& hook);

  //! No doc
  LIBSEMIGROUPS_DEPRECATED void
  relations(FroidurePinBase& S, std::function<void(word_type)>&& hook);
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_FROIDURE_PIN_BASE_HPP_
