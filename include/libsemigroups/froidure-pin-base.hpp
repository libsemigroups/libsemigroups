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

// TODO(later):
// 1. Remove virtualness
// 2. Avoid code duplication in the relations helper func

#ifndef LIBSEMIGROUPS_INCLUDE_FROIDURE_PIN_BASE_HPP_
#define LIBSEMIGROUPS_INCLUDE_FROIDURE_PIN_BASE_HPP_

#include <cstddef>     // for size_t
#include <functional>  // for function
#include <thread>      // for thread::hardware_concurrency

#include "constants.hpp"   // for LIMIT_MAX
#include "containers.hpp"  // for DynamicArray2
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

    //! Default constructor.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Constant.
    FroidurePinBase() = default;

    //! Default copy constructor.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Constant.
    FroidurePinBase(FroidurePinBase const& other) = default;

    //! Default move constructor.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Constant.
    FroidurePinBase(FroidurePinBase&& other) = default;

    //! FroidurePinBase instances are not copy assignable and this function is
    //! deleted.
    FroidurePinBase& operator=(FroidurePinBase const&) = delete;

    //! FroidurePinBase instances are not move assignable and this function is
    //! deleted.
    FroidurePinBase& operator=(FroidurePinBase&&) = delete;

    virtual ~FroidurePinBase() {}

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

    //! \copydoc FroidurePin::word_to_pos
    virtual element_index_type word_to_pos(word_type const&) const = 0;

    //! \copydoc FroidurePin::equal_to
    virtual bool equal_to(word_type const&, word_type const&) const = 0;

    //! \copydoc FroidurePin::current_max_word_length
    virtual size_t current_max_word_length() const = 0;

    //! \copydoc FroidurePin::degree
    virtual size_t degree() const = 0;

    //! \copydoc FroidurePin::nr_generators
    virtual size_t nr_generators() const = 0;

    //! \copydoc FroidurePin::current_size
    virtual size_t current_size() const = 0;

    //! \copydoc FroidurePin::current_nr_rules
    virtual size_t current_nr_rules() const = 0;

    //! \copydoc FroidurePin::prefix
    virtual element_index_type prefix(element_index_type) const = 0;

    //! \copydoc FroidurePin::suffix
    virtual element_index_type suffix(element_index_type) const = 0;

    //! \copydoc FroidurePin::first_letter
    virtual letter_type first_letter(element_index_type) const = 0;

    //! \copydoc FroidurePin::final_letter
    virtual letter_type final_letter(element_index_type) const = 0;

    //! \copydoc FroidurePin::length_const
    virtual size_t length_const(element_index_type) const = 0;

    //! \copydoc FroidurePin::length_non_const
    virtual size_t length_non_const(element_index_type) = 0;

    //! \copydoc FroidurePin::product_by_reduction
    virtual element_index_type
        product_by_reduction(element_index_type, element_index_type) const = 0;

    //! \copydoc FroidurePin::fast_product
    virtual element_index_type fast_product(element_index_type,
                                            element_index_type) const = 0;
    //! \copydoc FroidurePin::letter_to_pos
    virtual element_index_type letter_to_pos(letter_type) const = 0;

    //! \copydoc FroidurePin::size
    virtual size_t size() = 0;

    //! \copydoc FroidurePin::nr_idempotents
    virtual size_t nr_idempotents() = 0;

    //! \copydoc FroidurePin::is_idempotent
    virtual bool is_idempotent(element_index_type) = 0;

    //! \copydoc FroidurePin::is_monoid
    virtual bool is_monoid() = 0;

    //! \copydoc FroidurePin::is_monoid
    virtual tril is_finite() = 0;

    //! \copydoc FroidurePin::nr_rules
    virtual size_t nr_rules() = 0;

    //! \copydoc FroidurePin::reserve
    virtual void reserve(size_t) = 0;

    //! \copydoc FroidurePin::position_to_sorted_position
    virtual element_index_type position_to_sorted_position(element_index_type)
        = 0;

    //! \copydoc FroidurePin::right
    virtual element_index_type right(element_index_type, letter_type) = 0;

    //! \copydoc FroidurePin::left
    virtual element_index_type left(element_index_type, letter_type) = 0;

    //! \copydoc FroidurePin::right_cayley_graph
    virtual cayley_graph_type const& right_cayley_graph() = 0;

    //! \copydoc FroidurePin::left_cayley_graph
    virtual cayley_graph_type const& left_cayley_graph() = 0;

    // clang-format off
    //! \copydoc FroidurePin::minimal_factorisation(word_type&, element_index_type) NOLINT(whitespace/line_length)
    // clang-format on
    virtual void minimal_factorisation(word_type& word, element_index_type pos)
        = 0;

    //! \copydoc FroidurePin::minimal_factorisation(element_index_type)
    virtual word_type minimal_factorisation(element_index_type pos) = 0;

    //! \copydoc FroidurePin::factorisation(word_type&, element_index_type)
    virtual void factorisation(word_type& word, element_index_type pos) = 0;

    //! \copydoc FroidurePin::factorisation(element_index_type)
    virtual word_type factorisation(element_index_type pos) = 0;

    //! \copydoc FroidurePin::reset_next_relation
    virtual void reset_next_relation() = 0;

    //! \copydoc FroidurePin::next_relation
    virtual void next_relation(word_type& relation) = 0;

    virtual void enumerate(size_t) = 0;

   private:
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
  };

  //! Applies the function \p hook to every defining relation of \p S.
  //!
  //! \param S the FroidurePinBase whose relations are sought
  //! \param hook the hook function to apply
  //!
  //! \returns
  //! (None).
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  //!
  //! \complexity
  //! \f$O(|S||A|)\f$ where \f$A\f$ is the generating set for the parameter
  //! \f$S\f$.
  void relations(FroidurePinBase&                            S,
                 std::function<void(word_type, word_type)>&& hook);

  //! \copydoc libsemigroups::relations
  void relations(FroidurePinBase& S, std::function<void(word_type)>&& hook);
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_INCLUDE_FROIDURE_PIN_BASE_HPP_
