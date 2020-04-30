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

// This file contains a base class for congruence-like classes.

#ifndef LIBSEMIGROUPS_CONG_INTF_HPP_
#define LIBSEMIGROUPS_CONG_INTF_HPP_

#include <cstddef>  // for size_t
#include <memory>   // for shared_ptr
#include <string>   // for string
#include <vector>   // for vector

#include "fpsemi-intf.hpp"              // for FpSemigroupInterface
#include "libsemigroups-exception.hpp"  // for LIBSEMIGROUPS_EXCEPTION
#include "runner.hpp"                   // for Runner
#include "types.hpp"  // for word_type, letter_type, relation_type

namespace libsemigroups {
  class Congruence;       // Forward declaration, for friendship
  class FroidurePinBase;  // Forward declaration, for member functions

  //! The values in this enum can be used to indicate that a congruence should
  //! be 2-sided, left, or right.
  enum class congruence_type { left = 0, right = 1, twosided = 2 };

  //! Defined in ``cong-intf.hpp``.
  //!
  //! Every class for representing congruences in ``libsemigroups`` is derived
  //! from CongruenceInterface, which holds the member functions that are
  //! common to all its derived classes. CongruenceInterface is an abstract
  //! class.
  class CongruenceInterface : public Runner {
   public:
    ////////////////////////////////////////////////////////////////////////////
    // CongruenceInterface - typedefs + enums - public
    ////////////////////////////////////////////////////////////////////////////

    //! Type for indices of congruence class indices.
    using class_index_type = size_t;

    //! Type for non-trivial classes.
    //! \sa cbegin_ntc and cend_ntc.
    using non_trivial_classes_type = std::vector<std::vector<word_type>>;

    //! Type for a `const_iterator` to non-trivial classes.
    //! \sa cbegin_ntc and cend_ntc.
    using non_trivial_class_iterator = non_trivial_classes_type::const_iterator;

    //! Type for a `const_iterator` to the generating pairs.
    //! \sa cbegin_generating_pairs and cend_generating_pairs.
    using const_iterator = std::vector<relation_type>::const_iterator;

    ////////////////////////////////////////////////////////////////////////////
    // CongruenceInterface - constructors + destructor - public
    ////////////////////////////////////////////////////////////////////////////

    //! A CongruenceInterface instance is not default-constructible.
    //! This constructor is deleted.
    CongruenceInterface() = delete;

    //! Default copy constructor.
    CongruenceInterface(CongruenceInterface const&) = default;

    //! A CongruenceInterface instance is not copy assignable.
    //! This constructor is deleted.
    CongruenceInterface& operator=(CongruenceInterface const&) = delete;

    //! A CongruenceInterface instance is not move copyable.
    //! This constructor is deleted.
    CongruenceInterface(CongruenceInterface&&) = delete;

    //! A CongruenceInterface instance is not move assignable.
    //! This constructor is deleted.
    CongruenceInterface& operator=(CongruenceInterface&&) = delete;

    //! Constructors an empty instance of an interface to a congruence of type
    //! specified by the argument.
    explicit CongruenceInterface(congruence_type);

    virtual ~CongruenceInterface();

    ////////////////////////////////////////////////////////////////////////////
    // CongruenceInterface - non-pure virtual functions - public
    ////////////////////////////////////////////////////////////////////////////

    //! Check if a pair of words belongs to the congruence.
    //!
    //! \param u a word (vector of integers) over the generators of the
    //! semigroup.
    //! \param v a word (vector of integers) over the generators of
    //! the semigroup.
    //!
    //! \returns \c true if the words \p u and \p v belong to the
    //! same congruence class, and \c false otherwise.
    //!
    //! \throws LibsemigroupsException if \p u or \p v contains a letter that
    //! is out of bounds.
    //!
    //! \throws std::bad_alloc if the (possibly infinite) computation uses all
    //! the available memory.
    //!
    //! \par Complexity
    //! See warning.
    //!
    //! \warning The problem of determining the return value of this function
    //! is undecidable in general, and this function may never terminate.
    virtual bool contains(word_type const& u, word_type const& v) {
      validate_word(u);
      validate_word(v);
      return u == v || word_to_class_index(u) == word_to_class_index(v);
    }

    // Same as the above but only uses the so far computed information to
    // answer. In particular, does not call this->run().
    //! Check if a pair of words is known to belong to the congruence.
    //!
    //! \param u a word (vector of integers) over the generators of the
    //! semigroup.
    //! \param v a word (vector of integers) over the generators of
    //! the semigroup.
    //!
    //! \returns
    //! * libsemigroups::tril::TRUE if the words \p u and \p v
    //! are known to belong to the same congruence class
    //!
    //! * libsemigroups::tril::FALSE if the words are known to not belong
    //! to the same congruence class
    //!
    //! * libsemigroups::tril::unknown otherwise.
    //!
    //! \throws LibsemigroupsException if \p u or \p v contains a letter that
    //! is out of bounds.
    //!
    //! \par Complexity
    //! Linear in `u.size() + v.size()`.
    virtual tril const_contains(word_type const& u, word_type const& v) const;

    //! This function returns \c true if the congruence class of \p v is less
    //! than the class of \p v in a total ordering of congruence classes.
    //!
    //! \param u a word (vector of integers) over the generators of the
    //! semigroup.
    //! \param v a word (vector of integers) over the generators of
    //! the semigroup.
    //!
    //! \returns \c true if the class of \p u is less than that of \p .
    //!
    //! \throws LibsemigroupsException if \p u or \p v contains a letter that
    //! is out of bounds.
    //!
    //! \throws std::bad_alloc if the (possibly infinite) computation uses all
    //! the available memory.
    //!
    //! \par Complexity
    //! See warning.
    //!
    //! \warning The problem of determining the return value of this function
    //! is undecidable in general, and this function may never terminate.
    //!
    //! \par Possible Implementation
    //! \code
    //! bool less(word_type const& u, word_type const& v) {
    //!   return word_to_class_index(u) < word_to_class_index(v);
    //! }
    //! \endcode
    virtual bool less(word_type const& u, word_type const& v) {
      validate_word(u);
      validate_word(v);
      return word_to_class_index(u) < word_to_class_index(v);
    }
    // TODO(later) const_less

    /////////////////////////////////////////////////////////////////////////
    // CongruenceInterface - non-virtual functions - public
    /////////////////////////////////////////////////////////////////////////

    //! If the congruence, that an object of this type represents, is defined
    //! over a semigroup with generators \f$A\f$, then this function defines a
    //! injective function from \f$\{0, 1, \ldots, n -  1\}\f$, where \f$n\f$
    //! is the number of classes, or to the non-negative integers
    //! \f$\{0, 1, \ldots\}\f$ if \c this has infinitely many classes, to a
    //! fixed set of words over \f$A\f$ representing distinct congruences
    //! classes.
    //!
    //! \param i the index of the class whose representative we want to find, a
    //! value of type libsemigroups::word_type.
    //!
    //! \returns The word representing the \p i-th class of the congruence
    //!
    //! \throws LibsemigroupsException if the specified class index \p i
    //! exceeds the total number of classes.
    //!
    //! \throws std::bad_alloc if the (possibly infinite) computation uses all
    //! the available memory.
    //!
    //! \par Complexity
    //! See warning.
    //!
    //! \warning The function for finding the structure of a congruence may be
    //! non-deterministic, or undecidable, and this function may never return
    //! a result.
    //!
    //! \note
    //! CongruenceInterface::word_to_class_index and
    //!  CongruenceInterface::class_index_to_word are mutually inverse
    //!  functions.
    word_type class_index_to_word(class_index_type i);

    //! If the congruence, that an object of this type represents, is defined
    //! over a semigroup with generators \f$A\f$, then this function defines a
    //! surjective function from the set of all words over \f$A\f$ to either
    //! \f$\{0, 1, \ldots, n -  1\}\f$, where \f$n\f$ is the number of
    //! classes, or to the non-negative integers \f$\{0, 1, \ldots\}\f$ if \c
    //! this has infinitely many classes.
    //!
    //! \param w the word whose class index we want to find. The parameter \p
    //! w must be a libsemigroups::word_type consisting of indices of the
    //! generators of the semigroup over which \c this is defined.
    //!
    //! \returns The index of the congruence class corresponding to \p word.
    //!
    //! \throws LibsemigroupsException if \p w contains a letter that is out
    //! of bounds, or the object has not been fully initialised.
    //!
    //! \throws std::bad_alloc if the (possibly infinite) computation uses all
    //! the available memory.
    //!
    //! \par Complexity
    //! See warning.
    //!
    //! \warning The function for finding the structure of a congruence may be
    //! non-deterministic, or undecidable, and this function may never return
    //! a result.
    //!
    //! \note
    //! CongruenceInterface::word_to_class_index and
    //! CongruenceInterface::class_index_to_word are mutually inverse
    //! functions.
    class_index_type word_to_class_index(word_type const& w);

    //! Set the number of generators of the congruence.
    //!
    //! \param n the number of generators.
    //!
    //! \returns (None)
    //!
    //! \throws LibsemigroupsException If the number of generators has already
    //! been set to another value, or the parameter \p n is 0.
    //!
    //! \par Complexity
    //! Constant.
    void set_nr_generators(size_t n);

    //! Add a generating pair to the congruence.
    //!
    //! \param u a word (vector of integers) over the generators of the
    //! semigroup.
    //! \param v a word (vector of integers) over the generators of
    //! the semigroup.
    //!
    //! \returns (None)
    //!
    //! \throws LibsemigroupsException If any of the following hold:
    //! 1. the number of generators has not yet been set (set_nr_generators)
    //! 2. if \p u or \p v contains a letter that is out of bounds.
    //! 3. `this->started()` returns `true` indicating that the underlying
    //!    algorithm has been applied (partially or fully) to the data
    //!    structure.
    //!
    //! \par Complexity
    //! Linear in `u.size() + v.size()`.
    //!
    //! \note In some circumstances this function does not do anything. These
    //! are:
    //! * if \p u and \p v are identical words
    //! * if \c this->has_parent_froidure_pin() and the words \p u and \p v
    //! represent the same element of \c this->parent_froidure_pin().
    void add_pair(word_type const& u, word_type const& v);

    //! Add a pair to the congruence.
    //! \sa add_pair.
    void add_pair(std::initializer_list<size_t> l,
                  std::initializer_list<size_t> r) {
      add_pair(word_type(l), word_type(r));
    }

    //! Returns a const iterator pointing to the first generating pair of the
    //! congruence added by CongruenceInterface::add_pair (if any).
    //!
    //! \returns
    //! A CongruenceInterface::const_iterator pointing to a
    //! libsemigroups::relation_type.
    //!
    //! \par Exceptions
    //! This function is `noexcept` and is guaranteed never to throw.
    //!
    //! \par Complexity
    //! Constant.
    //!
    //! \par Parameters
    //! (None)
    const_iterator cbegin_generating_pairs() const noexcept {
      return _gen_pairs.cbegin();
    }

    //! Returns a const iterator pointing one-after-the-end of the last
    //! generating pair of the congruence added by
    //! CongruenceInterface::add_pair (if any).
    //!
    //! \returns
    //! A CongruenceInterface::const_iterator pointing to a
    //! libsemigroups::relation_type.
    //!
    //! \par Exceptions
    //! This function is `noexcept` and is guaranteed never to throw.
    //!
    //! \par Complexity
    //! Constant.
    //!
    //! \par Parameters
    //! (None)
    const_iterator cend_generating_pairs() const noexcept {
      return _gen_pairs.cend();
    }

    //! Returns a const iterator pointing to the first non-trivial (size > 1)
    //! class of the congruence.
    //!
    //! \returns
    //! A CongruenceInterface::non_trivial_class_iterator pointing to a
    //! std::vector<word_type>.
    //!
    //! \throws LibsemigroupsException if `!this->has_parent_froidure_pin()`.
    //!
    //! \par Complexity
    //! See warnings.
    //!
    //! \warning The problem of determining the return value of this function
    //! is undecidable in general, and this function may never terminate.
    //!
    //! \par Parameters
    //! (None)
    non_trivial_class_iterator cbegin_ntc() {
      init_non_trivial_classes();
      return _non_trivial_classes->cbegin();
    }

    //! Returns a const iterator pointing one-past-the-end of the non-trivial
    //! (size > 1) classes of the congruence.
    //!
    //! \returns
    //! A CongruenceInterface::non_trivial_class_iterator pointing to a
    //! std::vector<word_type>.
    //!
    //! \throws LibsemigroupsException if `!this->has_parent_froidure_pin()`
    //!
    //! \par Complexity
    //! See warnings.
    //!
    //! \warning The problem of determining the return value of this function
    //! is undecidable in general, and this function may never terminate.
    //!
    //! \par Parameters
    //! (None)
    non_trivial_class_iterator cend_ntc() {
      init_non_trivial_classes();
      return _non_trivial_classes->cend();
    }

    //! Returns a shared pointer to the non-trivial classes.
    //!
    //! \returns A std::shared_ptr to
    //! CongruenceInterface::non_trivial_classes_type.
    //!
    //!
    std::shared_ptr<non_trivial_classes_type const> non_trivial_classes() {
      init_non_trivial_classes();
      return _non_trivial_classes;
    }

    //! Returns the number of generators specified by
    //! CongruenceInterface::set_nr_generators.
    //!
    //! \returns The number of generators of the semigroup of the
    //! congruence that an object of this type represents, or
    //! libsemigroups::UNDEFINED.
    //!
    //! \par Exceptions
    //! This function is `noexcept` and is guaranteed never to throw.
    //!
    //! \par Complexity
    //! Constant.
    //!
    //! \par Parameters
    //! (None)
    size_t nr_generators() const noexcept {
      return _nr_gens;
    }

    //! Returns the number of generating pairs added by
    //! CongruenceInterface::add_pair.
    //!
    //! \returns The number of generating pairs of the
    //! congruence that an object of this type represents.
    //!
    //! \par Exceptions
    //! This function is `noexcept` and is guaranteed never to throw.
    //!
    //! \par Complexity
    //! Constant.
    //!
    //! \par Parameters
    //! (None)
    size_t nr_generating_pairs() const noexcept {
      return _gen_pairs.size();
    }

    //! Returns the number of non-trivial classes (size > 1) of the
    //! congruence.
    //!
    //! \returns The number of non-trivial classes of the congruence.
    //!
    //! \throws LibsemigroupsException if `!this->has_parent_froidure_pin()`
    //!
    //! \par Complexity
    //! See warning.
    //!
    //! \warning The problem of determining the return value of this function
    //! is undecidable in general, and this function may never terminate.
    //!
    //! \par Parameters
    //! (None)
    size_t nr_non_trivial_classes() {
      init_non_trivial_classes();
      return _non_trivial_classes->size();
    }

    //! Returns a semigroup represented as an instance of a derived class of
    //! FroidurePinBase that is isomorphic to the quotient of the parent
    //! semigroup of \c this by the 2-sided congruence that \c this
    //! represents.
    //!
    //! \returns A std::shared_ptr to FroidurePinBase.
    //!
    //! \throws LibsemigroupsException if any of the following hold:
    //! * the congruence is not 2-sided, `side() !=
    //! congruence_type::twosided`
    //! * the quotient semigroup is known (or can be easily be shown to be)
    //! infinite
    //! * the implementation throws.
    //!
    //! \throws std::bad_alloc if the (possibly infinite) computation uses all
    //! the available memory.
    //!
    //! \warning The problem of determining the return value of this function
    //! is undecidable in general, and this function may never terminate.
    //!
    //! \note
    //! The returned FroidurePin instance satisfies `FroidurePin::immutable()
    //! == true` and so certain of its member functions (those that change
    //! the underlying mathematical object) are disabled.
    //!
    //! \par Parameters
    //! (None)
    std::shared_ptr<FroidurePinBase> quotient_froidure_pin();

    //! Returns \c true if the congruence represented by this object knows an
    //! isomorphic quotient semigroup represented by an instance of
    //! FroidurePin.
    //!
    //! \returns A `bool`.
    //!
    //! \par Exceptions
    //! This function is `noexcept` and is guaranteed never to throw.
    //!
    //! \par Complexity
    //! Constant.
    //!
    //! \par Parameters
    //! (None)
    bool has_quotient_froidure_pin() const noexcept {
      return _quotient != nullptr;
    }

    //! Returns a reference to the parent semigroup over which the
    //! congruence represented by this object is defined, if it exists.
    //!
    //! \returns A std::shared_ptr to FroidurePinBase.
    //!
    //! \throws LibsemigroupsException if \c this was not created using a
    //! FroidurePin instance.
    //!
    //! \par Complexity
    //! Constant.
    //!
    //! \par Parameters
    //! (None)
    std::shared_ptr<FroidurePinBase> parent_froidure_pin() const;

    //! Returns \c true if the congruence represented by \c this was created
    //! from a FroidurePin instance.
    //!
    //! If \c true is returned, then \c this is a congruence over the semigroup
    //! represented by a FroidurePin instance.
    //!
    //! \returns A `bool`.
    //!
    //! \par Exceptions
    //! This function is `noexcept` and is guaranteed never to throw.
    //!
    //! \par Complexity
    //! Constant.
    //!
    //! \par Parameters
    //! (None)
    bool has_parent_froidure_pin() const noexcept;

    //! Return if the congruence represented by this object was created as a
    //! left, right, or two-sided congruence.
    //!
    //! \returns A libsemigroups::congruence_type.
    //!
    //! \par Exceptions
    //! This function is `noexcept` and is guaranteed never to throw.
    //!
    //! \par Complexity
    //! Constant.
    //!
    //! \par Parameters
    //! (None)
    congruence_type kind() const noexcept {
      return _type;
    }

    //! Return \c true if the number of classes in the congruence represented
    //! by \c this is obviously finite, and \c false if it is not obviously
    //! finite.
    //!
    //! \returns A `bool`.
    //!
    //! \par Exceptions
    //! This function throws if the implementation throws.
    //!
    //! \par Complexity
    //! Implementation specific, but this function is guaranteed to return a
    //! result. More specifically, this function will not trigger a
    //! computation that potentially never terminates.
    //!
    //! \warning
    //! If `true` is returned, then there are finitely many classes in the
    //! congruence, if `false` is returned, then the number of classes can be
    //! finite or infinite.
    //!
    //! \sa CongruenceInterface::is_quotient_obviously_infinite.
    //!
    //! \par Parameters
    //! (None)
    bool is_quotient_obviously_finite();

    //! Return \c true if the number of classes in the congruence represented
    //! by \c this is obviously infinite, and \c false if it is not obviously
    //! infinite.
    //!
    //! \returns A `bool`.
    //!
    //! \par Exceptions
    //! This function throws if the implementation throws.
    //!
    //! \par Complexity
    //! Implementation specific, but this function is guaranteed to return a
    //! result. More specifically, this function will not trigger a
    //! computation that potentially never terminates.
    //!
    //! \warning
    //! If `true` is returned, then there are infinitely many classes in the
    //! congruence, if `false` is returned, then the number of classes can be
    //! finite or infinite.
    //!
    //! \sa CongruenceInterface::is_quotient_obviously_finite.
    //!
    //! \par Parameters
    //! (None)
    bool is_quotient_obviously_infinite();

    //! Computes the total number of classes in the congruence represented by
    //! an instance of this type.
    //!
    //! \returns The number of congruences classes of \c this if this number
    //! is finite, or libsemigroups::POSITIVE_INFINITY in some cases if \c this
    //! number is not finite.
    //!
    //! \throws std::bad_alloc if the (possibly infinite) computation uses all
    //! the available memory.
    //!
    //! \par Complexity
    //! See warning.
    //!
    //! \warning The problem of determining the number of classes of a
    //! congruence over a finitely presented semigroup is undecidable in
    //! general, and this function may never terminate.
    //!
    //! \par Parameters
    //! (None)
    size_t nr_classes();

   protected:
    /////////////////////////////////////////////////////////////////////////
    // CongruenceInterface - non-virtual functions - protected
    /////////////////////////////////////////////////////////////////////////

    // TODO(later): add a new constructor to every derived class of this which
    // has FpSemigroup const& as a parameter of a constructor, the new
    // constructor should have parameter shared_ptr<FpSemigroupInterface>.
    // If necessary.

    //! No doc
    void set_parent_froidure_pin(std::shared_ptr<FroidurePinBase>);
    //! No doc
    void set_parent_froidure_pin(std::shared_ptr<FpSemigroupInterface>);

    //! No doc
    template <typename T, typename SFINAE = void>
    auto set_parent_froidure_pin(T const& prnt) ->
        typename std::enable_if<std::is_base_of<FroidurePinBase, T>::value,
                                SFINAE>::type {
      set_parent_froidure_pin(static_cast<std::shared_ptr<FroidurePinBase>>(
          std::make_shared<T>(prnt)));
    }

    //! No doc
    template <typename T, typename SFINAE = void>
    auto set_parent_froidure_pin(T& prnt) ->
        typename std::enable_if<std::is_base_of<FpSemigroupInterface, T>::value,
                                SFINAE>::type {
      if (prnt.finished()) {
        set_parent_froidure_pin(prnt.froidure_pin());
      } else {
        set_parent_froidure_pin(
            static_cast<std::shared_ptr<FpSemigroupInterface>>(
                std::make_shared<T>(prnt)));
      }
    }

    //! No doc
    bool validate_letter(letter_type) const;

    //! No doc
    void validate_word(word_type const&) const;

    //! No doc
    inline void validate_relation(word_type const& l,
                                  word_type const& r) const {
      validate_word(l);
      validate_word(r);
    }

    //! No doc
    inline void validate_relation(relation_type const& rel) const {
      validate_relation(rel.first, rel.second);
    }

    /////////////////////////////////////////////////////////////////////////
    // CongruenceInterface - non-virtual static functions - protected
    /////////////////////////////////////////////////////////////////////////

    //! No doc
    static std::string const& congruence_type_to_string(congruence_type);

   private:
    ////////////////////////////////////////////////////////////////////////////
    // Runner - pure virtual overridden function - public
    ////////////////////////////////////////////////////////////////////////////

    void before_run() override;

    /////////////////////////////////////////////////////////////////////////
    // CongruenceInterface - pure virtual functions - private
    /////////////////////////////////////////////////////////////////////////

    virtual word_type class_index_to_word_impl(class_index_type i)        = 0;
    virtual size_t    nr_classes_impl()                                   = 0;
    virtual std::shared_ptr<FroidurePinBase> quotient_impl()              = 0;
    virtual class_index_type word_to_class_index_impl(word_type const& w) = 0;

    /////////////////////////////////////////////////////////////////////////
    // CongruenceInterface - non-pure virtual functions - private
    /////////////////////////////////////////////////////////////////////////

    // const_word_to_class_index is private, because the answer returned
    // depends on the state of the object, but word_to_class_index does not
    // (i.e the return value should not change).
    virtual void             add_pair_impl(word_type const&, word_type const&);
    virtual class_index_type const_word_to_class_index(word_type const&) const;
    virtual void             set_nr_generators_impl(size_t);
    virtual std::shared_ptr<non_trivial_classes_type const>
    non_trivial_classes_impl();

    virtual bool is_quotient_obviously_finite_impl() = 0;
    // virtual bool is_quotient_obviously_finite_impl() {
    //   return false;
    // }

    virtual bool is_quotient_obviously_infinite_impl() {
      return false;
    }

    /////////////////////////////////////////////////////////////////////////
    // CongruenceInterface - non-virtual functions - private
    /////////////////////////////////////////////////////////////////////////

    void init_non_trivial_classes();

    // Reset all mutable data to "not known" and set finished() to false.
    // Any call to a non-const member function should call reset, if it
    // actually does anything.
    void reset() noexcept;

    void set_is_quotient_obviously_infinite(bool) const;
    void set_is_quotient_obviously_finite(bool) const;

    /////////////////////////////////////////////////////////////////////////
    // CongruenceInterface - non-mutable data members - private
    /////////////////////////////////////////////////////////////////////////

    // Forward decl
    class LazyFroidurePin;

    // Only data members which (potentially) change the mathematical object
    // defined by *this are non-mutable.
    std::vector<relation_type>       _gen_pairs;
    size_t                           _nr_gens;
    std::shared_ptr<LazyFroidurePin> _parent;
    congruence_type                  _type;

    /////////////////////////////////////////////////////////////////////////
    // CongruenceInterface - mutable data members - private
    /////////////////////////////////////////////////////////////////////////

    mutable bool                             _init_ntc_done;
    mutable bool                             _is_obviously_finite;
    mutable bool                             _is_obviously_infinite;
    mutable std::shared_ptr<FroidurePinBase> _quotient;
    mutable std::shared_ptr<non_trivial_classes_type const>
        _non_trivial_classes;

    /////////////////////////////////////////////////////////////////////////
    // CongruenceInterface - static data members - private
    /////////////////////////////////////////////////////////////////////////

    static const std::string STRING_TWOSIDED;
    static const std::string STRING_LEFT;
    static const std::string STRING_RIGHT;
  };
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_CONG_INTF_HPP_
