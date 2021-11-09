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

#include <cstddef>   // for size_t
#include <iterator>  // for forward_iterator_tag
#include <thread>    // for thread::hardware_concurrency

#include "constants.hpp"   // for UNDEFINED
#include "containers.hpp"  // for DynamicArray2
#include "exception.hpp"   // for LIBSEMIGROUPS_EXCEPTION
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
  //! FroidurePin that do not depend on the template parameter ``TElementType``
  //! (the type of the elements of the semigroup represented).
  //!
  //! \sa FroidurePin and FroidurePinTraits.
  class FroidurePinBase : public Runner {
    template <typename TElementType, typename TTraits>
    friend class FroidurePin;

   public:
    //! Unsigned integer type.
    // It should be possible to change this type and everything will just work,
    // provided the size of the semigroup is less than the maximum value of
    // this type of integer.
    using size_type = size_t;

    //! Type for the index of an element.
    //!
    //! The size of the semigroup being enumerated must be at most \c
    //! std::numeric_limits<element_index_type>::max()
    using element_index_type = size_type;

    //! Type for a left or right Cayley graph.
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

    //! Deleted.
    FroidurePinBase& operator=(FroidurePinBase const&) = delete;

    //! Deleted.
    FroidurePinBase& operator=(FroidurePinBase&&) = delete;

    virtual ~FroidurePinBase();

    ////////////////////////////////////////////////////////////////////////
    // FroidurePinBase - settings - public
    ////////////////////////////////////////////////////////////////////////

    //! Set a new value for the batch size.
    //!
    //! The *batch size* is the number of new elements to be found by any call
    //! to \ref run.
    //! This is used by, for example, FroidurePin::position so that it is
    //! possible to find the position of an element after only partially
    //! enumerating the semigroup.
    //!
    //! The default value of the batch size is `8192`.
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
    //! \ref run, see batch_size(size_t).
    //!
    //! \parameters
    //! None.
    size_t batch_size() const noexcept;

    //! Set the maximum number of threads.
    //!
    //! This member function sets the maximum number of threads to be used by
    //! any member function of a FroidurePin object. The number of threads is
    //! limited to the maximum of 1 and the minimum of \p number_of_threads and
    //! the number of threads supported by the hardware.
    //!
    //! The default value is `std::thread::hardware_concurrency()`.
    //!
    //! \param number_of_threads the maximum number of threads to use.
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
    FroidurePinBase& max_threads(size_t number_of_threads) noexcept;

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
    //! \parameters
    //! None.
    size_t max_threads() const noexcept;

    //! Set the threshold for concurrency to be used by member functions.
    //!
    //! This member function sets the threshold such that if size()
    //! exceeds this value, then the following functions may use a concurrent
    //! implementation:
    //!
    //! * \ref FroidurePin::number_of_idempotents
    //!
    //! The default value is `823543`.
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
    //! \parameters
    //! None.
    size_t concurrency_threshold() const noexcept;

    //! Set immutability.
    //!
    //! Prevent further changes to the mathematical semigroup represented by an
    //! instance of FroidurePinBase.
    //!
    //! This member function prevents certain member functions from being
    //! applied to a FroidurePinBase, such as FroidurePin::add_generators,
    //! if they would change the mathematical object represented by \c this.
    //!
    //! The default value is **false**.
    //!
    //! \param val the new value.
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

    //! Returns the current immutability.
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
    //! \parameters
    //! None.
    bool immutable() const noexcept;

    ////////////////////////////////////////////////////////////////////////
    // FroidurePinBase - pure virtual member functions - public
    ////////////////////////////////////////////////////////////////////////

#ifndef DOXYGEN_SHOULD_SKIP_THIS
    virtual bool equal_to(word_type const&, word_type const&) const = 0;

    virtual size_t number_of_generators() const = 0;

    virtual element_index_type fast_product(element_index_type,
                                            element_index_type) const = 0;

    virtual size_t number_of_idempotents() = 0;

    virtual bool is_idempotent(element_index_type) = 0;

    virtual tril is_finite() const = 0;

    virtual void reserve(size_t) = 0;

    virtual element_index_type position_to_sorted_position(element_index_type)
        = 0;
#endif

    ////////////////////////////////////////////////////////////////////////
    // FroidurePinBase - member functions - public
    ////////////////////////////////////////////////////////////////////////

    //! Returns the position corresponding to a word.
    //!
    //! Returns the position in the semigroup corresponding to the element
    //! represented by the word \p w.
    //!
    //! This function returns the position corresponding to the element
    //! obtained by evaluating the word in the generators \p w.
    //! No enumeration is performed, and
    //! \ref UNDEFINED is returned if the position of the element
    //! corresponding to \p w cannot be determined.
    //!
    //! This is equivalent to finding the product \c x of the
    //! generators FroidurePin::generator(\c w[i]) and then calling
    //! FroidurePin::current_position with argument \c x.
    //!
    //! \param w a word in the generators
    //!
    //! \returns
    //! A value of type `element_index_type` or \ref UNDEFINED.
    //!
    //! \throws LibsemigroupsException if \p w contains an value exceeding
    //! FroidurePin::number_of_generators.
    //!
    //! \complexity
    //! \f$O(n)\f$ where \f$n\f$ is the length of the word \p w.
    //!
    //! \sa FroidurePin::word_to_element.
    element_index_type current_position(word_type const& w) const;

    //! \copydoc current_position(word_type const&) const
    element_index_type
    current_position(std::initializer_list<size_t> const& w) const {
      word_type ww = w;
      return current_position(ww);
    }

    //! Returns the position in of the generator with specified index.
    //!
    //! In many cases \p current_position(i) will equal \p i, examples
    //! of when this will not be the case are:
    //!
    //! * there are duplicate generators;
    //!
    //! * FroidurePin::add_generators was called after the semigroup was already
    //! partially enumerated.
    //!
    //! \param i the index of the generators
    //!
    //! \returns A value of type \ref element_index_type.
    //!
    //! \throws LibsemigroupsException if \p i is greater than or equal to
    //! FroidurePin::number_of_generators.
    //!
    //! \complexity
    //! Constant.
    element_index_type current_position(letter_type i) const {
      validate_letter_index(i);
      return _letter_to_pos[i];
    }

    // TODO(later) analogues of the current_position mem fns for position

    //! Returns the maximum length of a word in the generators so far computed.
    //!
    //! Every elements of the semigroup can be expressed as the short-lex least
    //! product of the generators that equals that element.  This function
    //! returns the length of the longest short-lex least word in the
    //! generators that has so far been enumerated.
    //!
    //! \parameters
    //! (None)
    //!
    //! \returns
    //! A value of type \c size_t.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    size_t current_max_word_length() const noexcept {
      return _length[_enumerate_order.back()];
    }

    //! Returns the degree of any and all elements.
    //!
    //! \parameters
    //! (None)
    //!
    //! \returns
    //! A value of type \c size_t.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    size_t degree() const noexcept {
      return _degree;
    }

    //! Returns the number of elements so far enumerated.
    //!
    //! This is only the actual size of the semigroup if the semigroup is fully
    //! enumerated.
    //!
    //! \parameters
    //! (None)
    //!
    //! \returns
    //! A value of type \c size_t.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    size_t current_size() const noexcept {
      return _nr;
    }

    //! Returns the number of relations that have been found so far.
    //!
    //! This is only guaranteed to be the actual number of relations in a
    //! presentation defining the semigroup if the semigroup is fully
    //! enumerated.
    //!
    //! \parameters
    //! (None)
    //!
    //! \returns
    //! A value of type \c size_t.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    size_t current_number_of_rules() const noexcept {
      return _nr_rules;
    }

    //! Returns the position of the longest proper prefix.
    //!
    //! Returns the position of the prefix of the element \c x in position
    //! \p pos (of the semigroup) of length one less than the length of \c x.
    //!
    //! \param pos the position
    //!
    //! \returns A value of type \c element_index_type.
    //!
    //! \throws LibsemigroupsException if \p pos is greater than or equal to
    //! \ref current_size.
    //!
    //! \complexity
    //! Constant.
    element_index_type prefix(element_index_type pos) const {
      validate_element_index(pos);
      return _prefix[pos];
    }

    //! Returns the position of the longest proper suffix.
    //!
    //! Returns the position of the suffix of the element \c x in position
    //! \p pos (of the semigroup) of length one less than the length of \c x.
    //!
    //! \param pos the position
    //!
    //! \returns A value of type \c element_index_type.
    //!
    //! \throws LibsemigroupsException if \p pos is greater than or equal to
    //! \ref current_size.
    //!
    //! \complexity
    //! Constant.
    element_index_type suffix(element_index_type pos) const {
      validate_element_index(pos);
      return _suffix[pos];
    }

    //! Returns the first letter of the element with specified index.
    //!
    //! This function returns the first letter of the element in position
    //! \p pos of the semigroup, which is the index of the generator
    //! corresponding to the first letter of the element.
    //!
    //! \note
    //! Note that `FroidurePin::generator(first_letter(pos))` is
    //! only equal to `FroidurePin::at(first_letter(pos))` if
    //! there are no duplicate generators.
    //!
    //! \param pos the position
    //!
    //! \returns A value of type \c letter_type.
    //!
    //! \throws LibsemigroupsException if \p pos is greater than or equal to
    //! \ref current_size.
    //!
    //! \complexity
    //! Constant.
    letter_type first_letter(element_index_type pos) const {
      validate_element_index(pos);
      return _first[pos];
    }

    //! Returns the last letter of the element with specified index.
    //!
    //! This function returns the final letter of the element in position
    //! \p pos of the semigroup, which is the index of the generator
    //! corresponding to the final letter of the element.
    //!
    //! \note
    //! Note that `FroidurePin::generator(first_letter(pos))` is
    //! only equal to `FroidurePin::at(first_letter(pos))` if
    //! there are no duplicate generators.
    //!
    //! \param pos the position
    //!
    //! \returns A value of type \c letter_type.
    //!
    //! \throws LibsemigroupsException if \p pos is greater than or equal to
    //! \ref current_size.
    //!
    //! \complexity
    //! Constant.
    letter_type final_letter(element_index_type pos) const {
      validate_element_index(pos);
      return _final[pos];
    }

    //! Returns the length of the short-lex least word.
    //!
    //! \param pos the position
    //!
    //! \returns A value of type \c letter_type.
    //!
    //! \throws LibsemigroupsException if \p pos is greater than or equal to
    //! \ref current_size.
    //!
    //! \complexity
    //! Constant.
    //!
    //! \sa
    //! \ref length.
    size_t current_length(element_index_type pos) const {
      validate_element_index(pos);
      return _length[pos];
    }

    //! Returns the length of the short-lex least word.
    //!
    //! \param pos the position
    //!
    //! \returns A value of type \c letter_type.
    //!
    //! \throws LibsemigroupsException if \p pos is greater than or equal to
    //! \ref size.
    //!
    //! \complexity
    //! Constant.
    //!
    //! \sa
    //! \ref current_length.
    size_t length(element_index_type pos) {
      if (pos >= _nr) {
        run();
      }
      return current_length(pos);
    }

    //! Compute a product using the Cayley graph.
    //!
    //! This function finds the product of `at(i)` and `at(j)` by following the
    //! path in the right Cayley graph from \p i labelled by the word
    //! `minimal_factorisation(j)` or, if `minimal_factorisation(i)` is
    //! shorter, by following the path in the left Cayley graph from \p j
    //! labelled by `minimal_factorisation(i)`.
    //!
    //! \param i the first index of an element
    //! \param j the second index of an element
    //!
    //! \returns
    //! A value of type \ref element_index_type.
    //!
    //! \throws LibsemigroupsException if \p i or \p j is greater than or equal
    //! to \ref current_size.
    //!
    //! \complexity
    //! \f$O(n)\f$ where \f$n\f$ is the minimum of the lengths of
    //! `minimal_factorisation(i)` and `minimal_factorisation(j)`.
    element_index_type product_by_reduction(element_index_type i,
                                            element_index_type j) const;

    //! Returns the size.
    //!
    //! \parameters
    //! (None)
    //!
    //! \returns
    //! A value of type `size_t`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! At worst \f$O(|S|n)\f$ where \f$S\f$ is the semigroup represented by \c
    //! this, and \f$n\f$ is the return value of
    //! FroidurePin::number_of_generators.
    size_t size() {
      run();
      return _nr;
    }

    //! Check if the semigroup is a monoid.
    //!
    //! \returns
    //! \c true if the semigroup represented by \c this contains
    //! FroidurePin::One, and \c false if not.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! At worst \f$O(|S|n)\f$ where \f$S\f$ is the semigroup represented by \c
    //! this, and \f$n\f$ is the return value of
    //! FroidurePin::number_of_generators.
    //!
    //! \parameters
    //! (None).
    bool is_monoid() {
      run();
      return _found_one;
    }

    //! Returns the total number of relations in the presentation.
    //!
    //! \returns
    //! A value of type `size_t`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! At worst \f$O(|S|n)\f$ where \f$S\f$ is the semigroup represented by \c
    //! this, and \f$n\f$ is the return value of
    //! FroidurePin::number_of_generators.
    //!
    //! \sa \ref cbegin_rules and \ref cend_rules.
    //!
    //! \parameters
    //! (None)
    size_t number_of_rules() {
      run();
      return _nr_rules;
    }

    //! Returns the index of the product of an element and a generator.
    //!
    //! Returns the index of the product of the element in position \p i with
    //! the generator with index \p j.
    //!
    //! \param i the index of the element
    //! \param j the index of the generator
    //!
    //! \returns A value of type \ref element_index_type.
    //!
    //! \throws LibsemigroupsException if \p i is greater than or equal to
    //! size().
    //! \throws LibsemigroupsException if \p j is greater than or equal to
    //! FroidurePin::number_of_generators().
    //!
    //! \complexity
    //! At worst \f$O(|S|n)\f$ where \f$S\f$ is the semigroup represented by \c
    //! this, and \f$n\f$ is the return value of
    //! FroidurePin::number_of_generators.
    //!
    //! \sa
    //! \ref left.
    element_index_type right(element_index_type i, letter_type j) {
      validate_letter_index(j);
      run();
      validate_element_index(i);
      return _right.get(i, j);
    }

    //! Returns a const reference to the right Cayley graph.
    //!
    //! \returns A const reference to \ref cayley_graph_type.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! At worst \f$O(|S|n)\f$ where \f$S\f$ is the semigroup represented by \c
    //! this, and \f$n\f$ is the return value of
    //! FroidurePin::number_of_generators.
    //!
    //! \parameters
    //! None.
    cayley_graph_type const& right_cayley_graph() {
      run();
      _right.shrink_rows_to(size());
      return _right;
    }

    //! Returns the index of the product of a generator and an element.
    //!
    //! Returns the index of the product of the generator with index \p j with
    //! the element in position \p i.
    //!
    //! \param i the index of the element
    //! \param j the index of the generator
    //!
    //! \returns A value of type \ref element_index_type.
    //!
    //! \throws LibsemigroupsException if \p i is greater than or equal to
    //! size().
    //! \throws LibsemigroupsException if \p j is greater than or equal to
    //! FroidurePin::number_of_generators().
    //!
    //! \complexity
    //! At worst \f$O(|S|n)\f$ where \f$S\f$ is the semigroup represented by \c
    //! this, and \f$n\f$ is the return value of
    //! FroidurePin::number_of_generators.
    //!
    //! \sa
    //! \ref right.
    element_index_type left(element_index_type i, letter_type j) {
      validate_letter_index(j);
      run();
      validate_element_index(i);
      return _left.get(i, j);
    }

    //! Returns a const reference to the left Cayley graph.
    //!
    //! \returns A const reference to \ref cayley_graph_type.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! At worst \f$O(|S|n)\f$ where \f$S\f$ is the semigroup represented by \c
    //! this, and \f$n\f$ is the return value of
    //! FroidurePin::number_of_generators.
    //!
    //! \parameters
    //! None.
    cayley_graph_type const& left_cayley_graph() {
      run();
      _left.shrink_rows_to(size());
      return _left;
    }

    //! Obtain a short-lex least word representing an element given by index.
    //!
    //! Changes \p word in-place to contain a minimal word with respect to the
    //! short-lex ordering in the generators equal to the \p pos element of
    //! the semigroup.
    //!
    //! If \p pos is less than the size of this semigroup, then this member
    //! function changes its first parameter \p word in-place by first clearing
    //! it and then to contain a minimal factorization of the element in
    //! position \p pos of the semigroup with respect to the generators of the
    //! semigroup.  This function enumerates the semigroup until at least the
    //! \p pos element is known.
    //!
    //! \param word the word to clear and change in-place
    //! \param pos the index of the element whose factorisation is sought
    //!
    //! \returns
    //! (None)
    //!
    //! \throws LibsemigroupsException if \p pos is greater than or equal to
    //! size().
    //!
    //! \complexity
    //! At worst \f$O(mn)\f$ where \f$m\f$ equals \p pos and \f$n\f$ is the
    //! return value of FroidurePin::number_of_generators.
    void minimal_factorisation(word_type& word, element_index_type pos) {
      if (pos >= _nr && !finished()) {
        enumerate(pos + 1);
      }
      validate_element_index(pos);
      // call the const version below
      private_minimal_factorisation(word, pos);
    }

    //! Returns a short-lex least word representing an element given by index.
    //!
    //! This is the same as the two-argument member function for
    //! \ref minimal_factorisation, but it returns a \ref word_type by value
    //! instead of modifying an argument in-place.
    //!
    //! \param pos the index of the element whose factorisation is sought
    //!
    //! \returns
    //! A value of type `word_type`.
    //!
    //! \throws LibsemigroupsException if \p pos is greater than or equal to
    //! size().
    //!
    //! \complexity
    //! At worst \f$O(mn)\f$ where \f$m\f$ equals \p pos and \f$n\f$ is the
    //! return value of FroidurePin::number_of_generators.
    word_type minimal_factorisation(element_index_type pos) {
      word_type word;
      minimal_factorisation(word, pos);
      return word;
    }

    //! Obtain a short-lex least word representing an element given by index.
    //!
    //! Changes \p word in-place to contain a minimal word with respect to the
    //! short-lex ordering in the generators equal to the \p pos element of
    //! the semigroup. No further enumeration is performed.
    //!
    //! \param word the word to clear and change in-place
    //! \param pos the index of the element whose factorisation is sought
    //!
    //! \returns
    //! (None)
    //!
    //! \throws LibsemigroupsException if \p pos is greater than or equal to
    //! current_size().
    //!
    //! \complexity
    //! Constant.
    void minimal_factorisation(word_type& word, element_index_type pos) const {
      validate_element_index(pos);
      private_minimal_factorisation(word, pos);
    }

    //! Obtain a word representing an element given by index.
    //!
    //! Changes \p word in-place to contain a word in the generators equal to
    //! the \p pos element of the semigroup.
    //!
    //! If \p pos is less than the size of this semigroup, then this member
    //! function changes its first parameter \p word in-place by first clearing
    //! it and then to contain a factorization of the element in
    //! position \p pos of the semigroup with respect to the generators of the
    //! semigroup.  This function enumerates the semigroup until at least the
    //! \p pos element is known.
    //!
    //! The key difference between this function and
    //! minimal_factorisation(element_index_type) is that the resulting
    //! factorisation may not be minimal.
    //!
    //! \param word the word to clear and change in-place
    //! \param pos the index of the element whose factorisation is sought
    //!
    //! \returns
    //! (None)
    //!
    //! \throws LibsemigroupsException if \p pos is greater than or equal to
    //! size().
    //!
    //! \complexity
    //! At worst \f$O(mn)\f$ where \f$m\f$ equals \p pos and \f$n\f$ is the
    //! return value of FroidurePin::number_of_generators.
    void factorisation(word_type& word, element_index_type pos) {
      minimal_factorisation(word, pos);
    }

    //! Returns a word representing an element given by index.
    //!
    //! This is the same as the two-argument member function for
    //! \ref factorisation, but it returns a \ref word_type by value
    //! instead of modifying an argument in-place.
    //!
    //! The key difference between this function and
    //! minimal_factorisation(element_index_type) is that the resulting
    //! factorisation may not be minimal.
    //!
    //! \param pos the index of the element whose factorisation is sought
    //!
    //! \returns
    //! A value of type `word_type`.
    //!
    //! \throws LibsemigroupsException if \p pos is greater than or equal to
    //! size().
    //!
    //! \complexity
    //! At worst \f$O(mn)\f$ where \f$m\f$ equals \p pos and \f$n\f$ is the
    //! return value of FroidurePin::number_of_generators.
    word_type factorisation(element_index_type pos) {
      return minimal_factorisation(pos);
    }

    //! Enumerate until at least a specified number of elements are found.
    //!
    //! If the semigroup is already fully enumerated, or the number of elements
    //! previously enumerated exceeds \p limit, then calling this
    //! function does nothing. Otherwise, \ref run attempts to find at least
    //! the maximum of \p limit and \ref batch_size elements of the
    //! semigroup.
    //!
    //! \param limit the limit for current_size()
    //!
    //! \returns
    //! (None)
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! At worst \f$O(mn)\f$ where \f$m\f$ equals \p limit and \f$n\f$ is the
    //! return value of FroidurePin::number_of_generators.
    void enumerate(size_t limit);

    //! Return type of \ref cbegin_rules and \ref cend_rules.
    class const_rule_iterator {
#ifndef DOXYGEN_SHOULD_SKIP_THIS

     public:
      using size_type = typename std::vector<relation_type>::size_type;
      using difference_type =
          typename std::vector<relation_type>::difference_type;
      using const_pointer = typename std::vector<relation_type>::const_pointer;
      using pointer       = typename std::vector<relation_type>::pointer;
      using const_reference =
          typename std::vector<relation_type>::const_reference;
      using reference         = typename std::vector<relation_type>::reference;
      using value_type        = relation_type;
      using iterator_category = std::forward_iterator_tag;

      // None of the constructors are noexcept because the corresponding
      // constructors for std::vector aren't (until C++17).
      const_rule_iterator()                           = default;
      const_rule_iterator(const_rule_iterator const&) = default;
      const_rule_iterator(const_rule_iterator&&)      = default;
      const_rule_iterator& operator=(const_rule_iterator const&) = default;
      const_rule_iterator& operator=(const_rule_iterator&&) = default;

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

      ~const_rule_iterator() = default;

      bool operator==(const_rule_iterator const& that) const noexcept {
        return _gen == that._gen && _pos == that._pos;
      }

      bool operator!=(const_rule_iterator const& that) const noexcept {
        return !(this->operator==(that));
      }

      const_reference operator*() const noexcept {
        populate_relation();
        return _relation;
      }

      const_pointer operator->() const noexcept {
        populate_relation();
        return &_relation;
      }

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
            while (_gen < ptr->number_of_generators()) {
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

      // postfix
      const_rule_iterator operator++(int) noexcept {
        const_rule_iterator copy(*this);
        ++(*this);
        return copy;
      }

      void swap(const_rule_iterator& that) noexcept {
        _current.swap(that._current);
        std::swap(_froidure_pin, that._froidure_pin);
        std::swap(_gen, that._gen);
        std::swap(_pos, that._pos);
      }
#endif

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

    //! Returns a forward iterator pointing to the first rule (if any).
    //!
    //! Returns a forward iterator pointing to the first rule in a confluent
    //! terminating rewriting system defining a semigroup isomorphic to the
    //! one defined by \c this.
    //!
    //! This function does not perform any enumeration of the FroidurePin. If
    //! you want to obtain the complete set of rules, then it is necessary to
    //! call run() first.
    //!
    //! \returns An iterator of type FroidurePinBase::const_rule_iterator
    //! pointing to a \ref relation_type.
    //!
    //! \parameters
    //! (None).
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Constant
    //!
    //! \iterator_validity
    //! The iterators returned by this function are valid until the underlying
    //! FroidurePin instance is deleted.
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

    //! Returns a forward iterator pointing one past the last rule (if any).
    //!
    //! Returns a forward iterator pointing one-past-the-last rule (currently
    //! known) in a confluent terminating rewriting system defining a semigroup
    //! isomorphic to the one defined by \c this.
    //!
    //! This function does not perform any enumeration of the FroidurePin. If
    //! you want to obtain the complete set of rules, then it is necessary to
    //! call run() first.
    //!
    //! \returns An iterator of type FroidurePinBase::const_rule_iterator
    //! pointing to a \ref relation_type.
    //!
    //! \parameters
    //! (None).
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Constant
    //!
    //! \iterator_validity
    //! The iterators returned by this function are valid until the underlying
    //! FroidurePin instance is deleted.
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
      if (i >= number_of_generators()) {
        LIBSEMIGROUPS_EXCEPTION(
            "generator index out of bounds, expected value in [0, %d), got %d",
            number_of_generators(),
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
#ifdef LIBSEMIGROUPS_VERBOSE
    size_t _nr_products;
#endif
    size_t                          _nr_rules;
    enumerate_index_type            _pos;
    element_index_type              _pos_one;
    std::vector<element_index_type> _prefix;
    detail::DynamicArray2<bool>     _reduced;
    cayley_graph_type               _right;
    std::vector<element_index_type> _suffix;
    size_t                          _wordlen;
  };
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_FROIDURE_PIN_BASE_HPP_
