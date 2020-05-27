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

// This file contains a base class for f.p. semigroup like classes.

// TODO(later)
// 1. FpSemigroupInterface::make_confluent
//   * if a knuth_bendix wins, then just replace _rules by the active rules
//   of the knuth_bendix
//   * if a ToddCoxeter wins, then use FroidurePin to produce a confluent
//   set of rules.
// 2. Compare with cong-intf.hpp

#ifndef LIBSEMIGROUPS_FPSEMI_INTF_HPP_
#define LIBSEMIGROUPS_FPSEMI_INTF_HPP_

#include <cstddef>  // for size_t

#include <memory>         // for shared_ptr
#include <string>         // for string
#include <unordered_map>  // for unordered_map
#include <utility>        // for pair
#include <vector>         // for vector

#include "runner.hpp"  // for Runner
#include "types.hpp"   // for word_type, letter_type, relation_type

namespace libsemigroups {
  class FroidurePinBase;  // Forward declaration

  //! Defined in ``fpsemi-intf.hpp``.
  //!
  //! Every class for representing a finitely presented semigroup or monoid in
  //! ``libsemigroups`` is derived from FpSemigroupInterface, which holds the
  //! member functions that are common to all its derived classes.
  //! FpSemigroupInterface is an abstract class.
  class FpSemigroupInterface : public Runner {
   public:
    //! Type for rules in a finitely presented semigroup.
    using rule_type = std::pair<std::string, std::string>;

    //! Type for const iterators to the defining rules of the finitely
    //! presented semigroup represented by \c this.
    using const_iterator = std::vector<rule_type>::const_iterator;

    //////////////////////////////////////////////////////////////////////////////
    // FpSemigroupInterface - constructor + destructor - public
    //////////////////////////////////////////////////////////////////////////////

    //! Construct an empty finitely presented semigroup object.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \par Complexity
    //! Constant.
    //!
    //! \par Parameters
    //! (None)
    FpSemigroupInterface();

    //! Default copy constructor.
    FpSemigroupInterface(FpSemigroupInterface const&) = default;

    //! An FpSemigroupInterface instance is not copy assignable.
    //! This constructor is deleted.
    FpSemigroupInterface& operator=(FpSemigroupInterface const&) = delete;

    //! An FpSemigroupInterface instance is not move constructible.
    //! This constructor is deleted.
    FpSemigroupInterface(FpSemigroupInterface&&) = delete;

    //! An FpSemigroupInterface instance is not move assignable.
    //! This constructor is deleted.
    FpSemigroupInterface& operator=(FpSemigroupInterface&&) = delete;

    virtual ~FpSemigroupInterface() = default;

    ///////////////////////////////////////////////////////////////////////////
    // FpSemigroupInterface - pure virtual member functions - public
    ///////////////////////////////////////////////////////////////////////////

    //! Returns the size of the finitely presented semigroup.
    //!
    //! \returns A `size_t`, the value of which equals the size of \c this if
    //! this number is finite, or libsemigroups::POSITIVE_INFINITY in some
    //! cases if this number is not finite.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \par Complexity
    //! See warning.
    //!
    //! \warning The problem of determining the return value of this function
    //! is undecidable in general, and this function may never terminate.
    //!
    //! \par Parameters
    //! (None)
    virtual size_t size() = 0;

    //! Check if two words represent the same element.
    //!
    //! \param u a string over the alphabet of the finitely presented
    //! semigroup.
    //! \param v a string over the alphabet of the finitely presented
    //! semigroup.
    //!
    //! \returns \c true if the strings \p u and \p v represent the same
    //! element of the finitely presented semigroup, and \c false otherwise.
    //!
    //! \throws LibsemigroupsException if \p u or \p v contains a letter that
    //! does not belong to alphabet().
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
    //! \sa equal_to(word_type const&, word_type const&).
    virtual bool equal_to(std::string const& u, std::string const& v) = 0;

    //! Returns a normal form for a string.
    //!
    //! If \p u and \p v represent the same element of the finitely presented
    //! semigroup represented by \c this, then \c normal_form(u) is guaranteed
    //! to equal \c normal_form(v). No further guarantees are given, the return
    //! value of normal_form() depends on the implementation and may vary
    //! between finitely presented semigroups defined in precisely the same
    //! way.
    //!
    //! \param w the word whose normal form we want to find. The parameter \p
    //! w must be a std::string consisting of letters in alphabet().
    //!
    //! \returns the normal form of the parameter \p w, a value of type
    //! std::string.
    //!
    //! \throws LibsemigroupsException if \p w contains a letter that is not
    //! in FpSemigroupInterface::alphabet(), or the object has not been fully
    //! initialised.
    //!
    //! \par Complexity
    //! See warning.
    //!
    //! \warning The function for finding the structure of a finitely
    //! presented semigroup may be non-deterministic, or since the problem is
    //! undecidable in general, this function may never return a result.
    //!
    //! \sa normal_form(word_type const&).
    virtual std::string normal_form(std::string const& w) = 0;

    ///////////////////////////////////////////////////////////////////////
    // FpSemigroupInterface - non-pure virtual member functions - public
    ///////////////////////////////////////////////////////////////////////

    //! Check if two words represent the same element.
    //!
    //! \param u a libsemigroups::word_type consisting of indices of the
    //! generators of the finitely presented semigroup.
    //! \param v a libsemigroups::word_type consisting of indices of the
    //! generators of the finitely presented semigroup.
    //!
    //! \returns \c true if the words \p u and \p v represent the same
    //! element of the finitely presented semigroup, and \c false otherwise.
    //!
    //! \throws LibsemigroupsException if \p u or \p v contains a letter that
    //! is out of bounds.
    //!
    //! \par Complexity
    //! See warning.
    //!
    //! \warning The problem of determining the return value of this function
    //! is undecidable in general, and this function may never terminate.
    //!
    //! \sa equal_to(std::string const&, std::string const&).
    virtual bool equal_to(word_type const& u, word_type const& v);

    //! Returns a normal form for a libsemigroups::word_type.
    //!
    //! If \c u and \c v represent the same element of the finitely presented
    //! semigroup represented by \c this, then \c normal_form(u) is guaranteed
    //! to equal \c normal_form(v). No further guarantees are given, the return
    //! value of normal_form() depends on the implementation and may vary
    //! between finitely presented semigroups defined in precisely the same
    //! way.
    //!
    //! \param w the word whose normal form we want to find. The parameter \p
    //! w must be a libsemigroups::word_type consisting of indices of the
    //! generators of the finitely presented semigroup that \c this
    //! represents.
    //!
    //! \returns the normal form of the parameter \p w, a value of type
    //! libsemigroups::word_type.
    //!
    //! \throws LibsemigroupsException if \p w contains a letter that is out
    //! of bounds, or the object has not been fully initialised.
    //!
    //! \par Complexity
    //! See warning.
    //!
    //! \warning The function for finding the structure of a finitely
    //! presented semigroup may be non-deterministic, or since the problem is
    //! undecidable in general, this function may never return a result.
    //!
    //! \sa normal_form(std::string const&) and
    //! normal_form(std::initializer_list<letter_type>).
    virtual word_type normal_form(word_type const& w);

    //////////////////////////////////////////////////////////////////////////////
    // FpSemigroupInterface - non-virtual member functions - public
    //////////////////////////////////////////////////////////////////////////////

    //! \copydoc normal_form(word_type const&)
    word_type normal_form(std::initializer_list<letter_type> w) {
      return normal_form(word_type(w));
    }

    //! \copydoc equal_to(word_type const&, word_type const&)
    bool equal_to(std::initializer_list<letter_type> u,
                  std::initializer_list<letter_type> v) {
      return equal_to(word_type(u), word_type(v));
    }

    //! Convert a string to a libsemigroups::word_type representing the same
    //! element of the finitely presented semigroup represented by \c this.
    //!
    //! \param w the string to convert.
    //!
    //! \returns a libsemigroups::word_type.
    //!
    //! \throws LibsemigroupsException if \p w contains any characters not in
    //! alphabet().
    //!
    //! \par Complexity.
    //! \f$O(n)\f$ where \f$n\f$ is the length of \p w.
    word_type string_to_word(std::string const& w) const;

    //! Convert a libsemigroups::word_type to a std::string representing the
    //! same element of the finitely presented semigroup represented by \c
    //! this.
    //!
    //! \param w the libsemigroups::word_type to convert.
    //!
    //! \returns a std::string.
    //!
    //! \throws LibsemigroupsException if \p w contains any indices that are
    //! out of range.
    //!
    //! \par Complexity.
    //! \f$O(n)\f$ where \f$n\f$ is the length of \p w.
    std::string word_to_string(word_type const& w) const;

    //! Convert a char to a libsemigroups::letter_type representing the same
    //! generator of the finitely presented semigroup represented by \c this.
    //!
    //! \param a the string to convert.
    //!
    //! \returns a libsemigroups::letter_type.
    //!
    //! \throws LibsemigroupsException if \p a is not in alphabet().
    //!
    //! \par Complexity.
    //! Constant.
    letter_type char_to_uint(char a) const {
      validate_letter(a);
      return (*_alphabet_map.find(a)).second;
    }

    //! Convert a libsemigroups::letter_type to a char representing the same
    //! generator of the finitely presented semigroup represented by \c this.
    //!
    //! \param a the libsemigroups::letter_type to convert.
    //!
    //! \returns A char.
    //!
    //! \throws LibsemigroupsException if \p a is out of range.
    //!
    //! \par Complexity.
    //! Constant.
    char uint_to_char(letter_type a) const {
      validate_letter(a);
      return _alphabet[a];
    }

    //! Set the alphabet of the finitely presented semigroup.
    //!
    //! \param a the alphabet.
    //!
    //! \returns (None)
    //!
    //! \throws LibsemigroupsException If the alphabet has already ! been set to
    //! another value, the parameter \p a is empty, or there are ! repeated
    //! characters in \p a.
    //!
    //! \par Complexity
    //! Constant.
    //!
    //! \sa alphabet() and set_alphabet(size_t).
    void set_alphabet(std::string const& a);

    //! Set the size of the alphabet.
    //!
    //! Use this to specify the alphabet of a finitely presented semigroup if
    //! you intend to use indices rather than the actual letters in the
    //! alphabet in subsequent calculations.
    //!
    //! \param n the number of letters.
    //!
    //! \returns (None)
    //!
    //! \throws LibsemigroupsException If the size of the of alphabet has
    //! already been set to another value, or the parameter \p n is \c 0.
    //!
    //! \par Complexity
    //! Constant.
    //!
    //! \sa alphabet() and set_alphabet(std::string const&).
    void set_alphabet(size_t n);

    //! Set a character in alphabet() to be the identity.
    //!
    //! This function adds rules to \c this so that \p id is the identity.
    //! This function can be called repeatedly.
    //!
    //! \param id a string containing the character to be the identity.
    //!
    //! \returns (None)
    //!
    //! \throws LibsemigroupsException If \p id has length greater than 1, or
    //! \p id contains a character that is not in alphabet().
    //!
    //! \par Complexity
    //! \f$O(n)\f$ where \f$n\f$ is alphabet().size().
    //!
    //! \sa
    //! set_identity(letter_type).
    void set_identity(std::string const& id);

    //! Set a character in alphabet() to be the identity using its index.
    //!
    //! This function adds rules to \c this so that \p id is the identity.
    //! This function can be called repeatedly.
    //!
    //! \param id the index of the character to be the identity.
    //!
    //! \returns (None)
    //!
    //! \throws LibsemigroupsException If \p id is out of bounds.
    //!
    //! \par Complexity
    //! \f$O(n)\f$ where \f$n\f$ is alphabet().size().
    //!
    //! \sa
    //! set_identity(std::string const&).
    void set_identity(letter_type id) {
      validate_letter(id);
      set_identity(std::string(1, _alphabet[id]));
    }

    //! Set the inverses of letters in alphabet().
    //!
    //! The letter in \p a with index \c i is the inverse of the letter in
    //! alphabet() with index \c i.
    //!
    //! \param a a string of length alphabet().size().
    //!
    //! \returns (None)
    //!
    //! \throws LibsemigroupsException if any of the following apply:
    //! * \p a is empty;
    //! * alphabet() is empty;
    //! * no identity has been defined using set_identity();
    //! * the length of \p a is not equal to alphabet().size();
    //! * the letters in \p a are not exactly those in alphabet() (perhaps in
    //! a different order).
    //!
    //! \par Complexity
    //! \f$O(n)\f$ where \f$n\f$ is alphabet().size().
    //!
    //! \sa
    //! set_identity(std::string const&).
    void set_inverses(std::string const& a);

    //! Add a rule.
    //!
    //! \param u the left-hand side of the rule being added.
    //! \param v the right-hand side of the rule being added.
    //!
    //! \returns (None)
    //!
    //! \throws LibsemigroupsException if any of the following apply:
    //! * started() returns \c true; or
    //! * \p u or \p v contains a letter that does not belong to alphabet().
    //!
    //! \par Complexity
    //! Constant.
    void add_rule(std::string const& u, std::string const& v) {
      add_rule_private(std::string(u), std::string(v));
    }

    //! Add a rule.
    //!
    //! \param u the left-hand side of the rule being added.
    //! \param v the right-hand side of the rule being added.
    //!
    //! \returns (None)
    //!
    //! \throws LibsemigroupsException if any of the following apply:
    //! * started() returns \c true; or
    //! * \p u or \p v contains a letter that is out of bounds.
    //!
    //! \par Complexity
    //! Constant.
    void add_rule(word_type const& u, word_type const& v) {
      add_rule_private(word_to_string(u), word_to_string(v));
    }

    //! \copydoc add_rule(word_type const&, word_type const&)
    void add_rule(std::initializer_list<size_t> u,
                  std::initializer_list<size_t> v) {
      add_rule(word_type(u), word_type(v));
    }

    //! Add a rule.
    //!
    //! \param rel the rule being added.
    //!
    //! \returns (None)
    //!
    //! \throws LibsemigroupsException if any of the following apply:
    //! * started() returns \c true; or
    //! * \p rel.first or \p rel.second contains a letter that is out of bounds.
    //!
    //! \par Complexity
    //! Constant.
    void add_rule(relation_type rel) {
      add_rule(rel.first, rel.second);
    }

    //! \copydoc add_rule(relation_type)
    void add_rule(rule_type rel) {
      add_rule(rel.first, rel.second);
    }

    //! Add the rules of a finite presentation for \p S to \c this.
    //!
    //! \param S a FroidurePin object representing a semigroup.
    //!
    //! \returns (None)
    //!
    //! \throws LibsemigroupsException if any of the following apply:
    //! * alphabet() is empty;
    //! * the number of generators of \p S is not equal to `alphabet().size()`;
    //! or
    //! * started() returns \c true;
    //!
    //! \par Complexity
    //! At most \f$O(|S||A|)\f$ where \f$A\f$ is a generating set for \p S.
    void add_rules(FroidurePinBase& S);

    //! Add the rules in a vector to \c this.
    //!
    //! \param rels a vector of FpSemigroupInterface::rule_type.
    //!
    //! \returns (None)
    //!
    //! \throws LibsemigroupsException if add_rule() with argument any item
    //! in \p rels throws.
    //!
    //! \par Complexity
    //! \f$O(n)\f$ where \f$n\f$ is the size of \p rels.
    void add_rules(std::vector<rule_type> const& rels) {
      for (auto const& rel : rels) {
        add_rule(rel);
      }
    }

    //! Returns the alphabet of the finitely presented semigroup represented
    //! by \p this.
    //!
    //! \returns A const reference to the alphabet, a value of type
    //! std::string.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    //!
    //! \par Parameters
    //! (None)
    std::string const& alphabet() const noexcept {
      return _alphabet;
    }

    //! Returns the i-th letter of the alphabet of the finitely presented
    //! semigroup represented by \p this.
    //!
    //! \param i the index of the letter.
    //!
    //! \returns A std::string by value.
    //!
    //! \throws std::range_error if the index \p i is out of range.
    //!
    //! \complexity
    //! Constant.
    std::string alphabet(size_t i) const {
      return std::string({_alphabet.at(i)});
    }

    //! Returns the identity of \p this, or throws an exception if there isn't
    //! one.
    //!
    //! \returns A const reference to the identity, a value of type
    //! std::string.
    //!
    //! \throws LibsemigroupsException if no identity has been defined.
    //!
    //! \complexity
    //! Constant.
    //!
    //! \par Parameters
    //! (None)
    std::string const& identity() const;

    //! Returns the inverses of \p this, or throws an exception if there
    //! aren't any.
    //!
    //! \returns A const reference to the inverses, a value of type
    //! std::string.
    //!
    //! \throws LibsemigroupsException if no identity has been defined.
    //!
    //! \complexity
    //! Constant.
    //!
    //! \par Parameters
    //! (None)
    //!
    //! \sa set_inverses() for the meaning of the return value of this
    //! function.
    std::string const& inverses() const;

    //! Returns the number of rules of the finitely presented semigroup
    //! represented by \p this.
    //!
    //! \returns A value of type \c size_t.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    //!
    //! \par Parameters
    //! (None)
    size_t nr_rules() const noexcept {
      return _rules.size();
    }

    //! Returns \c true if a FroidurePin instance isomorphic to the finitely
    //! presented semigroup defined by \c this has already been computed, and
    //! \c false if not.
    //!
    //! \returns A \c bool.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    //!
    //! \par Parameters
    //! (None)
    bool has_froidure_pin() const noexcept {
      return _froidure_pin != nullptr;
    }

    //! Returns a FroidurePin instance isomorphic to the finitely
    //! presented semigroup defined by \c this.
    //!
    //! \returns A shared pointer to a FroidurePinBase.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! See warning.
    //!
    //! \warning The function for finding the structure of a finitely
    //! presented semigroup may be non-deterministic, or since the problem is
    //! undecidable in general, this function may never return a result.
    //!
    //! \par Parameters
    //! (None)
    std::shared_ptr<FroidurePinBase> froidure_pin() {
      if (_froidure_pin == nullptr) {
        _froidure_pin = froidure_pin_impl();
      }
      return _froidure_pin;
    }

    //! Return \c true if the finitely presented semigroup represented
    //! by \c this is obviously finite, and \c false if it is not obviously
    //! finite.
    //!
    //! \returns A `bool`.
    //!
    //! \par Exceptions
    //! \no_libsemigroups_except
    //!
    //! \par Complexity
    //! Implementation specific, but this function is guaranteed to return a
    //! result. More specifically, this function will not trigger a
    //! computation that potentially never terminates.
    //!
    //! \warning
    //! If `true` is returned, then the finitely presented semigroup is
    //! finite,  if `false` is returned, then the finitely presented
    //! semigroup can be finite or infinite.
    //!
    //! \sa is_obviously_infinite().
    //!
    //! \par Parameters
    //! (None)
    bool is_obviously_finite();

    //! Return \c true if the finitely presented semigroup represented
    //! by \c this is obviously infinite, and \c false if it is not obviously
    //! infinite.
    //!
    //! \returns A `bool`.
    //!
    //! \par Exceptions
    //! \no_libsemigroups_except
    //!
    //! \par Complexity
    //! Implementation specific, but this function is guaranteed to return a
    //! result. More specifically, this function will not trigger a
    //! computation that potentially never terminates.
    //!
    //! \warning
    //! If `true` is returned, then the finitely presented semigroup is
    //! infinite,  if `false` is returned, then the finitely presented
    //! semigroup can be finite or infinite.
    //!
    //! \sa is_obviously_finite().
    //!
    //! \par Parameters
    //! (None)
    bool is_obviously_infinite();

    //! Returns an iterator pointing to the first rule of \c this.
    //!
    //! \returns A FpSemigroupInterface::const_iterator.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    //!
    //! \par Parameters
    //! (None)
    const_iterator cbegin_rules() const noexcept {
      return _rules.cbegin();
    }

    //! Returns an iterator pointing one past the last rule of \c this.
    //!
    //! \returns A FpSemigroupInterface::const_iterator.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    //!
    //! \par Parameters
    //! (None)
    const_iterator cend_rules() const noexcept {
      return _rules.cend();
    }

    //! Returns a string containing [GAP](https://www.gap-system.org/)
    //! commands for defining a finitely presented semigroup equal to that
    //! represented by \c this.
    //!
    //! \returns A std::string.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! \f$O(m + n)\f$ where \f$m\f$ is alphabet().size() and \f$n\f$ is
    //! nr_rules().
    //!
    //! \par Parameters
    //! (None)
    std::string to_gap_string();

    //! Validates a letter.
    //!
    //! \param c the letter to validate.
    //!
    //! \returns
    //! (None)
    //!
    //! \throws LibsemigroupsException if \p c does not belong to alphabet().
    //!
    //! \complexity
    //! Constant.
    //!
    //! \par Parameters
    //! (None)
    void validate_letter(char c) const;

    //! Validates a letter.
    //!
    //! \param c the letter to validate.
    //!
    //! \returns
    //! (None)
    //!
    //! \throws LibsemigroupsException if \p c is out of range.
    //!
    //! \complexity
    //! Constant.
    //!
    //! \par Parameters
    //! (None)
    void validate_letter(letter_type c) const;

    //! Validates a word.
    //!
    //! \param w the word to validate.
    //!
    //! \returns
    //! (None)
    //!
    //! \throws LibsemigroupsException if any character in \c w does not
    //! belong to alphabet().
    //!
    //! \complexity
    //! Linear in the length of \p w.
    //!
    //! \par Parameters
    //! (None)
    void validate_word(std::string const& w) const {
      for (auto l : w) {
        validate_letter(l);
      }
      // Use validate_word_impl to impose any further restrictions on
      // valid words, for example, ToddCoxeter does not allow empty words (and
      // so neither does FpSemigroupInterface), but KnuthBendix does.
      validate_word_impl(w);
    }

    //! Validates a word.
    //!
    //! \param w the word to validate.
    //!
    //! \returns
    //! (None)
    //!
    //! \throws LibsemigroupsException if any index in \c w is out of range.
    //!
    //! \complexity
    //! Linear in the length of \p w.
    //!
    //! \par Parameters
    //! (None)
    void validate_word(word_type const& w) const {
      for (auto l : w) {
        validate_letter(l);
      }
      // Use validate_word_impl to impose any further restrictions on
      // valid words, for example, ToddCoxeter does not allow empty words (and
      // so neither does FpSemigroupInterface), but KnuthBendix does.
      validate_word_impl(w);
    }

    // The following were not used anywhere, but anyway written, so left here
    // in case wanted in the future.

    // void validate_relation(std::string const&, std::string const&) const;
    // void validate_relation(rule_type const&) const;
    // void validate_relation(relation_type const&) const;
    // void validate_relation(word_type const&, word_type const&) const;

   private:
    ////////////////////////////////////////////////////////////////////////////
    // Runner - pure virtual overridden function - public
    ////////////////////////////////////////////////////////////////////////////

    void before_run() override;

    //////////////////////////////////////////////////////////////////////////////
    // FpSemigroupInterface - pure virtual member functions - private
    //////////////////////////////////////////////////////////////////////////////

    virtual void add_rule_impl(std::string const&, std::string const&)    = 0;
    virtual std::shared_ptr<FroidurePinBase> froidure_pin_impl()          = 0;
    virtual bool                             is_obviously_infinite_impl() = 0;
    // bool is_obviously_infinite_impl() {
    //   return false;
    // }

    //////////////////////////////////////////////////////////////////////////////
    // FpSemigroupInterface - non-pure virtual member functions - private
    //////////////////////////////////////////////////////////////////////////////

    virtual void set_alphabet_impl(std::string const&);
    virtual void set_alphabet_impl(size_t);
    virtual void add_rules_impl(FroidurePinBase&);
    virtual bool is_obviously_finite_impl();
    // Use validate_word_impl to impose or lift any further restrictions on
    // valid words, for example, ToddCoxeter does not allow empty words (and
    // so neither does FpSemigroupInterface), but KnuthBendix does.
    virtual void validate_word_impl(std::string const&) const;
    virtual void validate_word_impl(word_type const&) const;
    // Returns true if we should add rules for the identity and false if not.
    virtual bool validate_identity_impl(std::string const&) const;

    //////////////////////////////////////////////////////////////////////////////
    // FpSemigroupInterface - non-virtual member functions - private
    //////////////////////////////////////////////////////////////////////////////

    void reset() noexcept;
    void set_is_obviously_infinite(bool) const;
    void set_is_obviously_finite(bool) const;
    void add_rule_private(std::string&&, std::string&&);

    //////////////////////////////////////////////////////////////////////////////
    // FpSemigroupInterface - non-mutable data - private
    //////////////////////////////////////////////////////////////////////////////

    std::string                           _alphabet;
    std::unordered_map<char, letter_type> _alphabet_map;
    bool                                  _identity_defined;
    std::string                           _identity;
    std::string                           _inverses;
    std::vector<rule_type>                _rules;

    //////////////////////////////////////////////////////////////////////////////
    // FpSemigroupInterface - mutable data - private
    //////////////////////////////////////////////////////////////////////////////

    mutable std::shared_ptr<FroidurePinBase> _froidure_pin;
    mutable bool                             _is_obviously_finite;
    mutable bool                             _is_obviously_infinite;
  };
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_FPSEMI_INTF_HPP_
