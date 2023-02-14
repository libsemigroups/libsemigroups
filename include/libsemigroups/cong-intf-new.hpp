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

// This file contains a base class for congruence-like classes.

#ifndef LIBSEMIGROUPS_V3_CONG_INTF_HPP_
#define LIBSEMIGROUPS_V3_CONG_INTF_HPP_

#include <cstddef>           // for size_t
#include <initializer_list>  // for initializer_list
#include <memory>            // for shared_ptr, operator!=, make_shared
#include <string>            // for string
#include <type_traits>       // for enable_if_t, is_base_of
#include <vector>            // for vector, operator==, vector<>::const_iter...

#include "runner.hpp"  // for Runner
#include "types.hpp"   // for word_type, relation_type, letter_type, tril

namespace libsemigroups {
  class FroidurePinBase;  // Forward declaration, for member functions

  namespace v3 {

    //! Defined in ``cong-intf.hpp``.
    //!
    //! Every class for representing congruences in ``libsemigroups`` is derived
    //! from CongruenceInterface, which holds the member functions that are
    //! common to all its derived classes. CongruenceInterface is an abstract
    //! class.
    class CongruenceInterface : public Runner {
      /////////////////////////////////////////////////////////////////////////
      // CongruenceInterface - non-mutable data members - private
      /////////////////////////////////////////////////////////////////////////

      std::vector<word_type> _generating_pairs;
      congruence_kind        _type;

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
      using non_trivial_class_iterator
          = non_trivial_classes_type::const_iterator;

      //! Type for a `const_iterator` to the generating pairs.
      //! \sa cbegin_generating_pairs and cend_generating_pairs.
      using const_iterator = std::vector<word_type>::const_iterator;

      ////////////////////////////////////////////////////////////////////////////
      // CongruenceInterface - constructors + destructor - public
      ////////////////////////////////////////////////////////////////////////////

      //! Constructs a congruence of the specified type.
      explicit CongruenceInterface(congruence_kind);
      void init(congruence_kind);

      CongruenceInterface() = default;
      void init();

      //! Default copy constructor.
      CongruenceInterface(CongruenceInterface const&)            = default;
      CongruenceInterface(CongruenceInterface&&)                 = default;
      CongruenceInterface& operator=(CongruenceInterface const&) = default;
      CongruenceInterface& operator=(CongruenceInterface&&)      = default;

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
      //! \complexity
      //! See warning.
      //!
      //! \warning The problem of determining the return value of this function
      //! is undecidable in general, and this function may never terminate.
      virtual bool contains(word_type const& u, word_type const& v) {
        return u == v || word_to_class_index(u) == word_to_class_index(v);
      }

      bool contains_with_arg_checks(word_type const& lhs,
                                    word_type const& rhs) {
        validate_word(lhs);
        validate_word(rhs);
        return contains(lhs, rhs);
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
      //! * tril::TRUE if the words \p u and \p v
      //! are known to belong to the same congruence class
      //!
      //! * tril::FALSE if the words are known to not belong
      //! to the same congruence class
      //!
      //! * tril::unknown otherwise.
      //!
      //! \throws LibsemigroupsException if \p u or \p v contains a letter that
      //! is out of bounds.
      //!
      //! \complexity
      //! Linear in `u.size() + v.size()`.
      virtual tril const_contains(word_type const& u, word_type const& v) const;

      //! Compare the indices of the classes containing two words.
      //!
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
      //! \complexity
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
        return word_to_class_index(u) < word_to_class_index(v);
      }
      // TODO(later) const_less

      /////////////////////////////////////////////////////////////////////////
      // CongruenceInterface - non-virtual functions - public
      /////////////////////////////////////////////////////////////////////////

      //! Get a canonical representative of the \c i-th class.
      //!
      //! If the congruence, that an object of this type represents, is defined
      //! over a semigroup with generators \f$A\f$, then this function defines a
      //! injective function from \f$\{0, 1, \ldots, n -  1\}\f$, where \f$n\f$
      //! is the number of classes, or to the non-negative integers
      //! \f$\{0, 1, \ldots\}\f$ if \c this has infinitely many classes, to a
      //! fixed set of words over \f$A\f$ representing distinct congruences
      //! classes.
      //!
      //! \param i the index of the class whose representative we want to find,
      //! a value of type \ref word_type.
      //!
      //! \returns The word representing the \p i-th class of the congruence
      //!
      //! \throws LibsemigroupsException if the specified class index \p i
      //! exceeds the total number of classes.
      //!
      //! \throws std::bad_alloc if the (possibly infinite) computation uses all
      //! the available memory.
      //!
      //! \complexity
      //! See warning.
      //!
      //! \warning The function for finding the structure of a congruence may be
      //! non-deterministic, or undecidable, and this function may never return
      //! a result.
      //!
      //! \note
      //! word_to_class_index() and class_index_to_word() are mutually inverse
      //! functions.
      word_type class_index_to_word(class_index_type i);

      //! Convert a word into the index of the class containing it.
      //!
      //! If the congruence, that an object of this type represents, is defined
      //! over a semigroup with generators \f$A\f$, then this function defines a
      //! surjective function from the set of all words over \f$A\f$ to either
      //! \f$\{0, 1, \ldots, n -  1\}\f$, where \f$n\f$ is the number of
      //! classes, or to the non-negative integers \f$\{0, 1, \ldots\}\f$ if \c
      //! this has infinitely many classes.
      //!
      //! \param w the word whose class index we want to find. The parameter \p
      //! w must be a \ref word_type consisting of indices of the
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
      //! \complexity
      //! See warning.
      //!
      //! \warning The function for finding the structure of a congruence may be
      //! non-deterministic, or undecidable, and this function may never return
      //! a result.
      //!
      //! \note
      //! word_to_class_index() and class_index_to_word() are mutually inverse
      //! functions.
      class_index_type word_to_class_index(word_type const& w);

      //! Returns a const iterator pointing to the first non-singleton class.
      //!
      //! \returns
      //! A \ref non_trivial_class_iterator pointing to a
      //! std::vector<word_type>.
      //!
      //! \throws LibsemigroupsException if has_parent_froidure_pin() returns
      //! `false`.
      //!
      //! \complexity
      //! See warnings.
      //!
      //! \warning The problem of determining the return value of this function
      //! is undecidable in general, and this function may never terminate.
      //!
      //! \par Parameters
      //! (None)
      // TODO
      // non_trivial_class_iterator cbegin_ntc() {
      //   init_non_trivial_classes();
      //   return _non_trivial_classes->cbegin();
      // }

      //! Returns a const iterator pointing one-past-the-end of the last
      //! non-singleton class.
      //!
      //! \returns
      //! A \ref non_trivial_class_iterator pointing to a
      //! std::vector<word_type>.
      //!
      //! \throws LibsemigroupsException if has_parent_froidure_pin() returns
      //! `false`.
      //!
      //! \complexity
      //! See warnings.
      //!
      //! \warning The problem of determining the return value of this function
      //! is undecidable in general, and this function may never terminate.
      //!
      //! \par Parameters
      //! (None)
      // TODO
      // non_trivial_class_iterator cend_ntc() {
      //   init_non_trivial_classes();
      //   return _non_trivial_classes->cend();
      // }

      //! Returns a shared pointer to the non-trivial classes.
      //!
      //! \returns A \shared_ptr to \ref non_trivial_classes_type.
      // TODO
      // std::shared_ptr<non_trivial_classes_type const> non_trivial_classes() {
      //   init_non_trivial_classes();
      //   return _non_trivial_classes;
      // }

      //! The number of non-singleton classes.
      //!
      //! \returns The number of non-singleton classes of the congruence.
      //!
      //! \throws LibsemigroupsException if has_parent_froidure_pin() returns \c
      //! false.
      //!
      //! \complexity
      //! See warning.
      //!
      //! \warning The problem of determining the return value of this function
      //! is undecidable in general, and this function may never terminate.
      //!
      //! \par Parameters
      //! (None)
      // TODO
      // size_t number_of_non_trivial_classes() {
      //   // TODO(later) this could be better, for example, if
      //   number_of_classes is
      //   // finite, then we can count the number of non-trivial classes, by
      //   // counting the number of paths in the right Cayley graph (right
      //   action
      //   // digraph if left or right) from the root to every vertex.
      //   // TODO(later) double check if we know that the number of classes is
      //   // finite and the parent semigroup is infinite, meaning that the
      //   // init_non_trivial_classes will run forever
      //   init_non_trivial_classes();
      //   return _non_trivial_classes->size();
      // }

      //! Returns a semigroup represented as an instance of a derived class of
      //! FroidurePinBase that is isomorphic to the quotient of the parent
      //! semigroup of \c this by the 2-sided congruence that \c this
      //! represents.
      //!
      //! \returns A \shared_ptr to FroidurePinBase.
      //!
      //! \throws LibsemigroupsException if any of the following hold:
      //! * the congruence is not 2-sided, `side() !=
      //! congruence_kind::twosided`
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
      // TODO std::shared_ptr<FroidurePinBase> quotient_froidure_pin();

      //! Check if the quotient semigroup has been computed.
      //!
      //! Returns \c true if the congruence represented by this object knows an
      //! isomorphic quotient semigroup represented by an instance of
      //! FroidurePin.
      //!
      //! \returns A `bool`.
      //!
      //! \exceptions
      //! \noexcept
      //!
      //! \complexity
      //! Constant.
      //!
      //! \par Parameters
      //! (None)
      // TODO bool has_quotient_froidure_pin() const noexcept {
      //   return _quotient != nullptr;
      // }

      //! The handedness of the congruence (left, right, or 2-sided).
      //!
      //! \returns A \ref congruence_kind.
      //!
      //! \exceptions
      //! \noexcept
      //!
      //! \complexity
      //! Constant.
      //!
      //! \par Parameters
      //! (None)
      congruence_kind kind() const noexcept {
        return _type;
      }

      //! Compute the number of classes in the congruence.
      //!
      //! \returns The number of congruences classes of \c this if this number
      //! is finite, or \ref POSITIVE_INFINITY in some cases if \c this
      //! number is not finite.
      //!
      //! \throws std::bad_alloc if the (possibly infinite) computation uses all
      //! the available memory.
      //!
      //! \complexity
      //! See warning.
      //!
      //! \warning The problem of determining the number of classes of a
      //! congruence over a finitely presented semigroup is undecidable in
      //! general, and this function may never terminate.
      //!
      //! \par Parameters
      //! (None)
      size_t number_of_classes();

      // TODO add_pair with checks
      void add_pair(word_type const& u, word_type const& v);
      void add_pair(word_type&& u, word_type&& v);

      //! Add a generating pair to the congruence.
      //! \sa add_pair(word_type const&, word_type const&)
      void add_pair(std::initializer_list<letter_type> l,
                    std::initializer_list<letter_type> r) {
        add_pair(word_type(l), word_type(r));
      }

      //! Returns a const iterator pointing to the first generating pair.
      //!
      //! \returns
      //! A \ref const_iterator pointing to a \ref relation_type.
      //!
      //! \exceptions
      //! \noexcept
      //!
      //! \complexity
      //! Constant.
      //!
      //! \par Parameters
      //! (None)
      // TODO redo the doc
      const_iterator cbegin_generating_pairs() const noexcept {
        return _generating_pairs.cbegin();
      }

      const_iterator cend_generating_pairs() const noexcept {
        return _generating_pairs.cend();
      }

     protected:
      /////////////////////////////////////////////////////////////////////////
      // CongruenceInterface - non-virtual static functions - protected
      /////////////////////////////////////////////////////////////////////////

      //! No doc
      // static std::string const& congruence_kind_to_string(congruence_kind);

     private:
      /////////////////////////////////////////////////////////////////////////
      // CongruenceInterface - pure virtual functions - private
      /////////////////////////////////////////////////////////////////////////

      virtual word_type        class_index_to_word_impl(class_index_type i) = 0;
      virtual size_t           number_of_classes_impl()                     = 0;
      virtual class_index_type word_to_class_index_impl(word_type const& w) = 0;
      virtual void             validate_word(word_type const& w) const      = 0;

      /////////////////////////////////////////////////////////////////////////
      // CongruenceInterface - non-pure virtual functions - private
      /////////////////////////////////////////////////////////////////////////

      // const_word_to_class_index is private, because the answer returned
      // depends on the state of the object, but word_to_class_index does not
      // (i.e the return value should not change).
      virtual class_index_type
      const_word_to_class_index(word_type const&) const;
      // TODO virtual std::shared_ptr<non_trivial_classes_type const>
      // non_trivial_classes_impl();

      /////////////////////////////////////////////////////////////////////////
      // CongruenceInterface - non-virtual functions - private
      /////////////////////////////////////////////////////////////////////////

      // TODO void init_non_trivial_classes();

      /////////////////////////////////////////////////////////////////////////
      // CongruenceInterface - mutable data members - private
      /////////////////////////////////////////////////////////////////////////

      // mutable bool                             _init_ntc_done;
      // mutable std::shared_ptr<FroidurePinBase> _quotient;
      // mutable std::shared_ptr<non_trivial_classes_type const>
      //     _non_trivial_classes;
    };
  }  // namespace v3
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_V3_CONG_INTF_HPP_
