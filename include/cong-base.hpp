//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2018 James D. Mitchell
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

#ifndef LIBSEMIGROUPS_INCLUDE_CONG_BASE_HPP_
#define LIBSEMIGROUPS_INCLUDE_CONG_BASE_HPP_

#include <stddef.h>  // for size_t
#include <string>    // for string
#include <vector>    // for vector

#include "internal/runner.hpp"  // for Runner

#include "types.hpp"  // for word_type, letter_type, relation_type

namespace libsemigroups {
  class FroidurePinBase;  // Forward declaration, for function members
  class Congruence;       // Forward declaration, for friendship

  enum class congruence_type { LEFT = 0, RIGHT = 1, TWOSIDED = 2 };

  class CongBase : public Runner {
    // Allows Congruence to use set_parent and has_parent on the Runner's it
    // contains
    friend class Congruence;

   protected:
    ////////////////////////////////////////////////////////////////////////////
    // CongBase - enums - protected
    ////////////////////////////////////////////////////////////////////////////

    enum class result_type { TRUE, FALSE, UNKNOWN };

   public:
    ////////////////////////////////////////////////////////////////////////////
    // CongBase - typedefs - public
    ////////////////////////////////////////////////////////////////////////////

    //! Type for indices of congruence classes in a CongBase object.
    using class_index_type           = size_t;
    using non_trivial_classes_type   = std::vector<std::vector<word_type>>;
    using non_trivial_class_iterator = non_trivial_classes_type::const_iterator;

    ////////////////////////////////////////////////////////////////////////////
    // CongBase - constructors + destructor - public
    ////////////////////////////////////////////////////////////////////////////

    explicit CongBase(congruence_type type);

    //! A default destructor.
    virtual ~CongBase();

    ////////////////////////////////////////////////////////////////////////////
    // CongBase - pure virtual methods - public
    ////////////////////////////////////////////////////////////////////////////

    //! Returns the index of the congruence class corresponding to \p word.
    //!
    //! The parameter \p word must be a libsemigroups::word_type consisting of
    //! indices of the generators of the semigroup over which \c this is
    //! defined.
    //!
    //! If \c this is defined over a semigroup with generators \f$A\f$, then
    //! CongBase::word_to_class_index defines a surjective function
    //! from the set of all words over \f$A\f$ to either
    //! \f$\{0, 1, \ldots, n -  1\}\f$,
    //! where \f$n\f$ is the number of classes, or to the non-negative
    //! integers \f$\{0, 1, \ldots\}\f$ if \c this has infinitely many
    //! classes.
    //!
    //! \warning The method for finding the structure of a congruence may be
    //! non-deterministic, and the return value of this method may vary
    //! between different instances of the same congruence.
    virtual class_index_type word_to_class_index(word_type const&) = 0;
    virtual word_type        class_index_to_word(class_index_type) = 0;

    //! Returns the number of congruences classes of \c this.
    //!
    //! This method is non-const because it may fully compute a data structure
    //! for the congruence.
    //!
    //! \warning The problem of determining the number of classes of a
    //! congruence over a finitely presented semigroup is undecidable in
    //! general, and this method may never terminate.
    virtual size_t nr_classes() = 0;

    //! Add a generating pair to the congruence. Should only be done before
    //! anything is computed about the congruence.
    virtual void add_pair(word_type const&, word_type const&) = 0;

    virtual FroidurePinBase* quotient_semigroup() = 0;

    virtual std::vector<word_type>::const_iterator cbegin_generating_pairs()
        = 0;
    virtual std::vector<word_type>::const_iterator cend_generating_pairs() = 0;

    ////////////////////////////////////////////////////////////////////////////
    // CongBase - non-pure virtual methods - public
    ////////////////////////////////////////////////////////////////////////////

    //! Returns \c true if the words \p w1 and \p w2 belong to the
    //! same congruence class.
    //!
    //! The parameters \p w1  and \p w2 must be libsemigroups::word_type's
    //! consisting of indices of generators of the semigroup over which \c
    //! this is defined.
    //!
    //! \warning The problem of determining the return value of this method is
    //! undecidable in general, and this method may never terminate.
    virtual bool contains(word_type const&, word_type const&);

    // Same as the above but only uses the so far computed information to
    // answer. In particular, does not call this->run(). This method may
    // return false negatives, but must not return false positives.
    virtual result_type const_contains(word_type const&,
                                       word_type const&) const;

    //! Returns \c true if the congruence class of \p w1 is less than
    //! that of \p w2.
    //!
    //! This method returns \c true if the congruence class of \p w1 is less
    //! than the class of \p w2 in a total ordering of congruence classes.
    //!
    //! The parameters \p w1 and \p w2 should be libsemigroups::word_type's
    //! consisting of indices of the generators of the semigroup over which \c
    //! this is defined.
    //!
    //! \warning The method for finding the structure of a congruence is
    //! non-deterministic, and the total order of congruences classes may vary
    //! between different instances of the same congruence.
    //!
    //! \warning The problem of determining the return value of this method is
    //! undecidable in general, and this method may never terminate.
    virtual bool less(word_type const&, word_type const&);

    virtual bool is_quotient_obviously_finite();
    virtual bool is_quotient_obviously_infinite();
    virtual void set_nr_generators(size_t);

    /////////////////////////////////////////////////////////////////////////
    // CongBase - non-virtual methods - public
    /////////////////////////////////////////////////////////////////////////

    // Pass by value since these must be copied anyway
    void add_pair(std::initializer_list<size_t>, std::initializer_list<size_t>);

    //! Returns the non-trivial classes of the congruence.
    //!
    //! The elements in these classes are represented as words in the
    //! generators of the semigroup over which the congruence is defined.
    //!
    //! \warning If \c this has infinitely many non-trivial congruence
    //! classes, then this method will only terminate when it can no longer
    //! allocate memory.
    non_trivial_class_iterator cbegin_ntc();
    non_trivial_class_iterator cend_ntc();

    //! Return the type of the congruence, i.e. if it is a left, right, or
    //! two-sided congruence.
    size_t           nr_generators() const noexcept;
    size_t           nr_generating_pairs() const noexcept;
    size_t           nr_non_trivial_classes();
    FroidurePinBase* parent_semigroup() const;
    congruence_type  type() const noexcept;

   protected:
    /////////////////////////////////////////////////////////////////////////
    // CongBase - non-virtual methods - protected
    /////////////////////////////////////////////////////////////////////////

    FroidurePinBase* get_quotient() const noexcept;
    bool             has_quotient() const noexcept;
    void             reset_quotient();
    void             set_quotient(FroidurePinBase*, bool);

    FroidurePinBase* get_parent() const noexcept;
    bool             has_parent() const noexcept;
    void             set_parent(FroidurePinBase*);

    bool validate_letter(letter_type) const;
    void validate_word(word_type const&) const;
    void validate_relation(word_type const&, word_type const&) const;
    void validate_relation(relation_type const&) const;

    /////////////////////////////////////////////////////////////////////////
    // CongBase - non-virtual static methods - protected
    /////////////////////////////////////////////////////////////////////////

    static std::string const& congruence_type_to_string(congruence_type);

    /////////////////////////////////////////////////////////////////////////
    // CongBase - data - protected
    /////////////////////////////////////////////////////////////////////////

    std::vector<std::vector<word_type>> _non_trivial_classes;
    size_t                              _nr_generating_pairs;

   private:
    /////////////////////////////////////////////////////////////////////////
    // CongBase - non-pure virtual methods - private
    /////////////////////////////////////////////////////////////////////////

    // const_word_to_class_index is private, because the answer returned
    // depends on the state of the object, but word_to_class_index does not
    // (i.e the return value should not change).
    virtual class_index_type const_word_to_class_index(word_type const&) const;
    virtual void             init_non_trivial_classes();

    /////////////////////////////////////////////////////////////////////////
    // CongBase - data members - private
    /////////////////////////////////////////////////////////////////////////

    bool             _delete_quotient;
    bool             _init_ntc_done;
    size_t           _nrgens;
    FroidurePinBase* _parent;
    FroidurePinBase* _quotient;
    congruence_type  _type;

    /////////////////////////////////////////////////////////////////////////
    // CongBase - static data members - private
    /////////////////////////////////////////////////////////////////////////

    static const std::string STRING_TWOSIDED;
    static const std::string STRING_LEFT;
    static const std::string STRING_RIGHT;
  };
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_INCLUDE_CONG_BASE_HPP_
