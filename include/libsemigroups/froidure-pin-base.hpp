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

// TODO
// * remove iterators and use ranges instead
// * the todos in the file
// * reduce api  (i.e. move things to helper namespace)
// * no_checks versions of mem + helper fns

#ifndef LIBSEMIGROUPS_FROIDURE_PIN_BASE_HPP_
#define LIBSEMIGROUPS_FROIDURE_PIN_BASE_HPP_

#include <cstddef>   // for size_t
#include <iterator>  // for forward_iterator_tag
#include <thread>    // for thread::hardware_concurrency

#include <array>             // for array
#include <cstddef>           // for size_t
#include <cstdint>           // for uint32_t
#include <initializer_list>  // for initializer_list
#include <iterator>          // for forward_iterator...
#include <thread>            // for thread
#include <type_traits>       // for is_copy_assignable
#include <utility>           // for swap
#include <vector>            // for vector, allocator

#include "constants.hpp"   // for UNDEFINED
#include "runner.hpp"      // for Runner
#include "types.hpp"       // for word_type, generator_index_type, tril
#include "word-graph.hpp"  // for WordGraph

#include "detail/containers.hpp"  // for DynamicArray2

#include <rx/ranges.hpp>  // for iterator_range

namespace libsemigroups {

  //! Defined in ``froidure-pin-base.hpp``.
  //!
  //! FroidurePinBase is an abstract base class for the class template
  //! FroidurePin.
  //!
  //! FroidurePinBase allows a polymorphic interface to instances of
  //! FroidurePin and its member functions reflect those member functions of
  //! FroidurePin that do not depend on the template parameter ``Element``
  //! (the type of the elements of the semigroup represented).
  //!
  //! \sa FroidurePin and FroidurePinTraits.
  class FroidurePinBase : public Runner {
    template <typename Element, typename Traits>
    friend class FroidurePin;

   public:
    //! Unsigned integer type.
    // It should be possible to change this type and everything will just work,
    // provided the size of the semigroup is less than the maximum value of
    // this type of integer.
    using size_type = uint32_t;

    using generator_index_type = size_type;

    //! Type for the index of an element.
    //!
    //! The size of the semigroup being enumerated must be at most \c
    //! std::numeric_limits<element_index_type>::max()
    using element_index_type = size_type;

    //! Type for a left or right Cayley graph.
    using cayley_graph_type = WordGraph<element_index_type>;

   private:
    // See comments in FroidurePin
    using enumerate_index_type = size_type;

    ////////////////////////////////////////////////////////////////////////
    // FroidurePin - data members - private
    ////////////////////////////////////////////////////////////////////////
    struct Settings {
      Settings() noexcept
          : _batch_size(8'192),
            _concurrency_threshold(823'543),
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

    size_t _degree;
    std::vector<std::pair<generator_index_type, generator_index_type>>
                                      _duplicate_gens;
    std::vector<element_index_type>   _enumerate_order;
    std::vector<generator_index_type> _final;
    std::vector<generator_index_type> _first;
    bool                              _found_one;
    bool                              _idempotents_found;
    std::vector<int>                  _is_idempotent;
    cayley_graph_type                 _left;
    std::vector<size_type>            _length;
    std::vector<enumerate_index_type> _lenindex;
    std::vector<element_index_type>   _letter_to_pos;
    size_type                         _nr;
    size_t                            _nr_products;
    size_t                            _nr_rules;
    enumerate_index_type              _pos;
    element_index_type                _pos_one;
    std::vector<element_index_type>   _prefix;
    detail::DynamicArray2<bool>       _reduced;
    cayley_graph_type                 _right;
    std::vector<element_index_type>   _suffix;
    size_t                            _wordlen;

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
    [[nodiscard]] size_t batch_size() const noexcept;

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
    [[nodiscard]] size_t max_threads() const noexcept;

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
    [[nodiscard]] size_t concurrency_threshold() const noexcept;

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
    [[nodiscard]] bool immutable() const noexcept;

    ////////////////////////////////////////////////////////////////////////
    // FroidurePinBase - pure virtual member functions - public
    ////////////////////////////////////////////////////////////////////////

#ifndef DOXYGEN_SHOULD_SKIP_THIS

    [[nodiscard]] virtual size_t number_of_generators() const = 0;
    [[nodiscard]] virtual tril   is_finite() const            = 0;

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
    // TODO(later) -> helper
    [[nodiscard]] element_index_type current_position(word_type const& w) const;

    //! \copydoc current_position(word_type const&) const
    // TODO(later) -> helper
    [[nodiscard]] element_index_type
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
    [[nodiscard]] element_index_type
    current_position(generator_index_type i) const {
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
    [[nodiscard]] size_t current_max_word_length() const noexcept {
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
    [[nodiscard]] size_t degree() const noexcept {
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
    [[nodiscard]] size_t current_size() const noexcept {
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
    [[nodiscard]] size_t current_number_of_rules() const noexcept {
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
    [[nodiscard]] element_index_type suffix(element_index_type pos) const {
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
    //! \returns A value of type \c generator_index_type.
    //!
    //! \throws LibsemigroupsException if \p pos is greater than or equal to
    //! \ref current_size.
    //!
    //! \complexity
    //! Constant.
    [[nodiscard]] generator_index_type
    first_letter(element_index_type pos) const {
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
    //! \returns A value of type \c generator_index_type.
    //!
    //! \throws LibsemigroupsException if \p pos is greater than or equal to
    //! \ref current_size.
    //!
    //! \complexity
    //! Constant.
    [[nodiscard]] generator_index_type
    final_letter(element_index_type pos) const {
      validate_element_index(pos);
      return _final[pos];
    }

    //! Returns the length of the short-lex least word.
    //!
    //! \param pos the position
    //!
    //! \returns A value of type \c generator_index_type.
    //!
    //! \throws LibsemigroupsException if \p pos is greater than or equal to
    //! \ref current_size.
    //!
    //! \complexity
    //! Constant.
    //!
    //! \sa
    //! \ref length.
    [[nodiscard]] size_t current_length(element_index_type pos) const {
      validate_element_index(pos);
      return _length[pos];
    }

    //! Returns the length of the short-lex least word.
    //!
    //! \param pos the position
    //!
    //! \returns A value of type \c generator_index_type.
    //!
    //! \throws LibsemigroupsException if \p pos is greater than or equal to
    //! \ref size.
    //!
    //! \complexity
    //! Constant.
    //!
    //! \sa
    //! \ref current_length.
    // TODO(later) helper
    [[nodiscard]] size_t length(element_index_type pos) {
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
    // TODO(later) helper
    [[nodiscard]] element_index_type
    product_by_reduction(element_index_type i, element_index_type j) const;

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
    [[nodiscard]] size_t size() {
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
    [[nodiscard]] bool is_monoid() {
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
    [[nodiscard]] size_t number_of_rules() {
      run();
      return _nr_rules;
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
    [[nodiscard]] cayley_graph_type const& right_cayley_graph() {
      run();
      _right.induced_subgraph_no_checks(0, size());  // TODO(now) Why's this
                                                     // necessary?
      return _right;
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
    [[nodiscard]] cayley_graph_type const& left_cayley_graph() {
      run();
      _left.induced_subgraph_no_checks(
          0,
          size());  // TODO(now) Why's this necessary?
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
    // TODO(later) helper
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
    // TODO(later) helper
    [[nodiscard]] word_type minimal_factorisation(element_index_type pos) {
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
    // TODO(later) helper
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
    // TODO(later) helper
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
    // TODO(later) helper
    [[nodiscard]] word_type factorisation(element_index_type pos) {
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

    // TODO(now) doc
    [[nodiscard]] size_t number_of_elements_of_length(size_t min,
                                                      size_t max) const;

    // TODO(now) doc
    [[nodiscard]] size_t number_of_elements_of_length(size_t len) const;

    //! Return type of \ref cbegin_rules and \ref cend_rules.
    // TODO(later) delete
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
      const_rule_iterator()                                      = default;
      const_rule_iterator(const_rule_iterator const&)            = default;
      const_rule_iterator(const_rule_iterator&&)                 = default;
      const_rule_iterator& operator=(const_rule_iterator const&) = default;
      const_rule_iterator& operator=(const_rule_iterator&&)      = default;

      // TODO(now) to cpp
      const_rule_iterator(FroidurePinBase const* ptr,
                          enumerate_index_type   pos,
                          generator_index_type   gen);

      ~const_rule_iterator() = default;

      [[nodiscard]] bool
      operator==(const_rule_iterator const& that) const noexcept {
        return _gen == that._gen && _pos == that._pos;
      }

      [[nodiscard]] bool
      operator!=(const_rule_iterator const& that) const noexcept {
        return !(this->operator==(that));
      }

      [[nodiscard]] const_reference operator*() const {
        populate_relation();
        return _relation;
      }

      [[nodiscard]] const_pointer operator->() const {
        populate_relation();
        return &_relation;
      }

      // prefix
      const_rule_iterator const& operator++() noexcept;

      // postfix
      [[nodiscard]] const_rule_iterator operator++(int) noexcept {
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
      void populate_relation() const;

      std::array<generator_index_type, 3> _current;
      FroidurePinBase const*              _froidure_pin;
      generator_index_type                _gen;
      enumerate_index_type                _pos;
      mutable relation_type               _relation;
    };  // const_rule_iterator

    // Assert that the forward iterator requirements are met
    static_assert(std::is_default_constructible_v<const_rule_iterator>,
                  "forward iterator requires default-constructible");
    static_assert(std::is_copy_constructible_v<const_rule_iterator>,
                  "forward iterator requires copy-constructible");
    static_assert(std::is_copy_assignable_v<const_rule_iterator>,
                  "forward iterator requires copy-assignable");
    static_assert(std::is_destructible_v<const_rule_iterator>,
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
    // TODO(later) delete
    [[nodiscard]] const_rule_iterator cbegin_rules() const {
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
    // TODO(later) delete
    [[nodiscard]] const_rule_iterator cend_rules() const {
      return const_rule_iterator(this, current_size(), 0);
    }

    // TODO(later) delete, it'd be more efficient to have this be a forward
    // iterator only (i.e. as is done in the GAP version of FroidurePin)
    class const_normal_form_iterator {
      // Private data
      mutable FroidurePinBase const* _froidure_pin;
      enumerate_index_type           _pos;
      mutable word_type              _word;

     public:
      using size_type       = typename std::vector<word_type>::size_type;
      using difference_type = typename std::vector<word_type>::difference_type;
      using const_pointer   = typename std::vector<word_type>::const_pointer;
      using pointer         = typename std::vector<word_type>::pointer;
      using const_reference = typename std::vector<word_type>::const_reference;
      using reference       = typename std::vector<word_type>::reference;
      using value_type      = word_type;
      using iterator_category = std::random_access_iterator_tag;

      const_normal_form_iterator()                                  = default;
      const_normal_form_iterator(const_normal_form_iterator const&) = default;
      const_normal_form_iterator(const_normal_form_iterator&&)      = default;
      const_normal_form_iterator& operator=(const_normal_form_iterator const&)
          = default;
      const_normal_form_iterator& operator=(const_normal_form_iterator&&)
          = default;

      ~const_normal_form_iterator() = default;

      const_normal_form_iterator(FroidurePinBase const* ptr,
                                 enumerate_index_type   pos)
          : _froidure_pin(ptr), _pos(pos), _word() {}

      [[nodiscard]] bool
      operator==(const_normal_form_iterator const& that) const noexcept {
        return _pos == that._pos;
      }

      [[nodiscard]] bool
      operator!=(const_normal_form_iterator const& that) const noexcept {
        return !(*this == that);
      }

      [[nodiscard]] bool
      operator<(const_normal_form_iterator const& that) const noexcept {
        return _pos < that._pos;
      }

      [[nodiscard]] bool
      operator>(const_normal_form_iterator const& that) const noexcept {
        return _pos > that._pos;
      }

      [[nodiscard]] bool
      operator<=(const_normal_form_iterator const& that) const noexcept {
        return _pos < that._pos;
      }

      [[nodiscard]] bool
      operator>=(const_normal_form_iterator const& that) const noexcept {
        return _pos > that._pos;
      }

      [[nodiscard]] const_reference operator*() const noexcept {
        populate_word();
        return _word;
      }

      [[nodiscard]] const_pointer operator->() const noexcept {
        populate_word();
        return &_word;
      }

      [[nodiscard]] const_reference operator[](size_type index) const {
        const_cast<const_normal_form_iterator*>(this)->_pos += index;
        populate_word();
        const_cast<const_normal_form_iterator*>(this)->_pos -= index;
        return _word;
      }

      // prefix
      const_normal_form_iterator const& operator++() noexcept {
        _pos++;
        return *this;
      }

      [[nodiscard]] const_normal_form_iterator operator++(int) noexcept {
        const_normal_form_iterator copy(*this);
        ++(*this);
        return copy;
      }

      const_normal_form_iterator const& operator--() noexcept {
        _pos--;
        return *this;
      }

      [[nodiscard]] const_normal_form_iterator operator--(int) noexcept {
        const_normal_form_iterator copy(*this);
        --(*this);
        return copy;
      }

      void operator+=(size_type val) noexcept {
        _pos += val;
      }

      void operator-=(size_type val) noexcept {
        _pos -= val;
      }

      [[nodiscard]] const_normal_form_iterator
      operator+(size_type val) const noexcept {
        const_normal_form_iterator copy(*this);
        copy += val;
        return copy;
      }

      [[nodiscard]] const_normal_form_iterator
      operator-(size_type val) const noexcept {
        const_normal_form_iterator copy(*this);
        copy -= val;
        return copy;
      }

      void swap(const_normal_form_iterator& that) noexcept {
        std::swap(_froidure_pin, that._froidure_pin);
        std::swap(_pos, that._pos);
        std::swap(_word, that._word);
      }

     private:
      void populate_word() const {
        _froidure_pin->minimal_factorisation(_word, _pos);
      }
    };

    [[nodiscard]] const_normal_form_iterator cbegin_normal_forms() const {
      return const_normal_form_iterator(this, 0);
    }

    [[nodiscard]] const_normal_form_iterator cend_normal_forms() const {
      return const_normal_form_iterator(this, current_size());
    }

    [[nodiscard]] auto normal_forms() const {
      return rx::iterator_range(cbegin_normal_forms(), cend_normal_forms());
    }

   private:
    ////////////////////////////////////////////////////////////////////////
    // FroidurePin - member functions - private
    ////////////////////////////////////////////////////////////////////////
    void partial_copy(FroidurePinBase const& S);

    // TODO(later) delete
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

    void validate_element_index(element_index_type i) const;
    void validate_letter_index(generator_index_type i) const;
  };
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_FROIDURE_PIN_BASE_HPP_
