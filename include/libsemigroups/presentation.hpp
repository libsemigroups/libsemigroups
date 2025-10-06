//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2022-2025 James D. Mitchell
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

// This file contains the declaration of a class template for semigroup or
// monoid presentations. The idea is to provide a shallow wrapper around a
// vector of words, with some checks that the vector really defines a
// presentation, (i.e. it's consistent with its alphabet etc), and some related
// functionality.

#ifndef LIBSEMIGROUPS_PRESENTATION_HPP_
#define LIBSEMIGROUPS_PRESENTATION_HPP_

#include <algorithm>         // for reverse, sort
#include <cmath>             // for pow
#include <cstring>           // for size_t, strlen
#include <initializer_list>  // for initializer_list
#include <iterator>          // for distance
#include <limits>            // for numeric_limits
#include <map>               // for map
#include <numeric>           // for accumulate
#include <string>            // for basic_string, operator==
#include <tuple>             // for tie, tuple
#include <type_traits>       // for enable_if_t
#include <unordered_map>     // for operator==, operator!=
#include <unordered_set>     // for unordered_set
#include <utility>           // for move, pair
#include <vector>            // for vector, operator!=

#include "adapters.hpp"              // for Hash, EqualTo
#include "constants.hpp"             // for Max, UNDEFINED, operator==
#include "debug.hpp"                 // for LIBSEMIGROUPS_ASSERT
#include "is_specialization_of.hpp"  // for is_specialization_of
#include "order.hpp"                 // for ShortLexCompare
#include "ranges.hpp"      // for seq, operator|, rx, take, chain, is_sorted
#include "types.hpp"       // for word_type
#include "ukkonen.hpp"     // for GreedyReduceHelper, Ukkonen
#include "word-range.hpp"  // for operator+

#include "detail/fmt.hpp"     // for format
#include "detail/print.hpp"   // for isprint etc
#include "detail/string.hpp"  // for maximum_common_prefix
#include "detail/uf.hpp"      // for Duf

namespace libsemigroups {

  //! \defgroup presentations_group Presentations
  //!
  //! This file contains documentation related to semigroup and monoid
  //! presentations in `libsemigroups`.
  //!
  //! There are two classes and two namespaces with functionality related to
  //! presentations:
  //! * \ref Presentation "the Presentation class"
  //! * \ref InversePresentation "the InversePresentation class"
  //! * \ref ::presentation "Helper functions for presentations"
  //! * \ref ::presentation::examples "Presentations for standard examples"
  //!
  //! For documentation on how to create presentations from other
  //! `libsemigroups` objects, see the documentation for the `to<Presentation>`
  //! functions \ref to_presentation_group "here".

  //! \brief Empty base for presentation-like classes.
  struct PresentationBase {};

  //! \ingroup presentations_group
  //!
  //! \brief For an implementation of presentations for semigroups or monoids.
  //!
  //! Defined in `presentation.hpp`.
  //!
  //! This class template can be used to construction presentations for
  //! semigroups or monoids and is intended to be used as the input to other
  //! algorithms in `libsemigroups`. The idea is to provide a shallow wrapper
  //! around a vector of words of type `Word`. We refer to this vector of words
  //! as the \a rules of the presentation. In addition to the rules, a
  //! Presentation object has an \a alphabet.
  //!
  //! In a valid presentation, rules will only consist of letters from within
  //! the alphabet; however, for performance reasons, it is possible to update
  //! both the rules and the alphabet independently of each other. For this
  //! reason, it is possible for the alphabet and the rules to become out of
  //! sync. The Presentation class provides some checks that the rules define a
  //! valid presentation, and some related functionality is available in the
  //! namespace `libsemigroups::presentation`.
  //!
  //! \tparam Word the type of the underlying words.
  template <typename Word>
  class Presentation : public PresentationBase {
   public:
    //! \brief Type of the words in the rules of a Presentation object.
    using word_type = Word;

    //! \brief Type of the letters in the words that constitute the rules of
    //! a Presentation object.
    using letter_type = typename word_type::value_type;

    //! \brief Type of a const iterator to either side of a rule.
    using const_iterator = typename std::vector<word_type>::const_iterator;

    //! \brief Type of an iterator to either side of a rule.
    using iterator = typename std::vector<word_type>::iterator;

    //! \brief Size type for rules.
    using size_type = typename std::vector<word_type>::size_type;

   private:
    word_type                                  _alphabet;
    std::unordered_map<letter_type, size_type> _alphabet_map;
    bool                                       _contains_empty_word;

   public:
    //! \brief Data member holding the rules of the presentation.
    //!
    //! The rules can be altered using the member functions of `std::vector`,
    //! and the presentation can be checked for validity using
    //! \ref throw_if_bad_alphabet_or_rules.
    std::vector<word_type> rules;

    //! \brief Default constructor.
    //!
    //! Constructs an empty presentation with no rules and no alphabet.
    Presentation();

    //! \brief Remove the alphabet and all rules.
    //!
    //! This function clears the alphabet and all rules from the presentation,
    //! putting it back into the state it would be in if it was newly
    //! constructed.
    //!
    //! \returns A reference to `this`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    Presentation& init();

    //! \brief Default copy constructor.
    //!
    //! Default copy constructor.
    Presentation(Presentation const&);

    //! \brief Default move constructor.
    //!
    //! Default move constructor.
    Presentation(Presentation&&);

    //! \brief Default copy assignment operator.
    //!
    //! Default copy assignment operator.
    Presentation& operator=(Presentation const&);

    //! \brief Default move assignment operator.
    //!
    //! Default move assignment operator.
    Presentation& operator=(Presentation&&);

    ~Presentation();

    //! \brief Return the alphabet of the presentation.
    //!
    //! Returns the alphabet of the presentation.
    //!
    //! \returns A const reference to \c Presentation::word_type.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    [[nodiscard]] word_type const& alphabet() const noexcept {
      return _alphabet;
    }

    // TODO(later) alphabet_no_checks

    //! \brief Set the alphabet by size.
    //!
    //! Sets the alphabet to the range \f$[0, n)\f$ consisting of values of
    //! type \ref letter_type.
    //!
    //! \param n the size of the alphabet.
    //!
    //! \returns A reference to \c *this.
    //!
    //! \throws LibsemigroupsException if the value of \p n is greater than the
    //! maximum number of letters supported by \ref letter_type.
    //!
    //! \warning
    //! This function does not verify that the rules in the presentation (if
    //! any) consist of letters belonging to the alphabet.
    //!
    //! \sa
    //! * \ref throw_if_alphabet_has_duplicates
    //! * \ref throw_if_bad_rules
    //! * \ref throw_if_bad_alphabet_or_rules
    // TODO(1) Rename alphabet_size
    Presentation& alphabet(size_type n);

    //! \brief Set the alphabet const reference.
    //!
    //! Sets the alphabet to be the letters in \p lphbt.
    //!
    //! \param lphbt the alphabet.
    //!
    //! \returns A reference to \c *this.
    //!
    //! \throws LibsemigroupsException if there are duplicate letters in
    //! \p lphbt.
    //!
    //! \warning
    //! This function does not verify that the rules in the presentation (if
    //! any) consist of letters belonging to the alphabet.
    //!
    //! \sa
    //! * \ref throw_if_bad_rules
    //! * \ref throw_if_bad_alphabet_or_rules
    Presentation& alphabet(word_type const& lphbt);

    //! \brief Set the alphabet from rvalue reference.
    //!
    //! Sets the alphabet to be the letters in \p lphbt.
    //!
    //! \param lphbt the alphabet.
    //!
    //! \returns A reference to \c *this.
    //!
    //! \throws LibsemigroupsException if there are duplicate letters in
    //! \p lphbt.
    //!
    //! \warning
    //! This function does not verify that the rules in the presentation (if
    //! any) consist of letters belonging to the alphabet.
    //!
    //! \sa
    //! * \ref throw_if_bad_rules
    //! * \ref throw_if_bad_alphabet_or_rules
    Presentation& alphabet(word_type&& lphbt);

    //! \brief Set the alphabet from string_view.
    //!
    //! This is an overload for \ref alphabet(word_type&&) to allow
    //! std::string_view to be used for the parameter \p lphbt.
    //!
    //! \warning This function is only enabled if \ref word_type is std::string.
    template <typename Return = Presentation&>
    auto alphabet(std::string_view lphbt)
        -> std::enable_if_t<std::is_same_v<std::string, word_type>, Return&> {
      return alphabet(std::string(lphbt));
    }

    //! \brief Set the alphabet from string literal.
    //!
    //! This is an overload for \ref alphabet(word_type&&) to allow
    //! string literals to be used for the parameter \p lphbt.
    //!
    //! \warning This function is only enabled if \ref word_type is std::string.
    template <typename Return = Presentation&>
    auto alphabet(char const* lphbt)
        -> std::enable_if_t<std::is_same_v<std::string, word_type>, Return> {
      return alphabet(std::string(lphbt));
    }

    //! \brief Set the alphabet from std::initializer_list.
    //!
    //! This is an overload for \ref alphabet(word_type&&) to allow
    //! std::initializer_list to be used for the parameter \p lphbt.
    // There's some weirdness with {0} being interpreted as a string_view, which
    // means that the next overload is required
    Presentation& alphabet(
        std::initializer_list<typename word_type::value_type> const& lphbt) {
      return alphabet(word_type(lphbt));
    }

    //! \brief Set the alphabet to be the letters in the rules.
    //!
    //! Sets the alphabet to be the letters in \ref rules.
    //!
    //! \returns A reference to \c *this.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! At most \f$O(mn)\f$ where \f$m\f$ is the number of rules, and \f$n\f$ is
    //! the length of the longest rule.
    //!
    //! \sa
    //! * \ref throw_if_bad_rules
    //! * \ref throw_if_bad_alphabet_or_rules
    Presentation& alphabet_from_rules();

    //! \brief Return a letter in the alphabet by index.
    //!
    //! Returns the letter of the alphabet in position \p i.
    //!
    //! \param i the index.
    //!
    //! \returns A value of type \ref letter_type.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \warning
    //! This function performs no bound checks on the argument \p i.
    [[nodiscard]] letter_type letter_no_checks(size_type i) const {
      LIBSEMIGROUPS_ASSERT(i < _alphabet.size());
      return _alphabet[i];
    }

    //! \brief Return a letter in the alphabet by index.
    //!
    //! After checking that \p i is in the range \f$[0, n)\f$, where \f$n\f$ is
    //! the length of the alphabet, this function performs the same as
    //! `letter_no_checks(size_type i) const`.
    //!
    //! \throws LibsemigroupsException if \p i is not in the range \f$[0, n)\f$.
    //!
    //! \sa
    //! * \ref letter_no_checks
    [[nodiscard]] letter_type letter(size_type i) const;

    //! \brief Return the index of a letter in the alphabet.
    //!
    //! Get the index of a letter in the alphabet.
    //!
    //! \param val the letter.
    //!
    //! \returns A value of type \ref size_type.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Constant.
    //!
    //! \warning This function does not verify that its argument belongs to the
    //! alphabet.
    [[nodiscard]] size_type index_no_checks(letter_type val) const {
      return _alphabet_map.find(val)->second;
    }

    //! \brief Return the index of a letter in the alphabet.
    //!
    //! After checking that \p val is in the the alphabet, this function
    //! performs the same as `index_no_checks(letter_type val) const`.
    //!
    //! \throws LibsemigroupsException if \p val does not belong to the
    //! alphabet.
    //!
    //! \sa
    //! * \ref index_no_checks
    [[nodiscard]] size_type index(letter_type val) const;

    //! \brief Check if a letter belongs to the alphabet or not.
    //!
    //! Check if a letter belongs to the alphabet or not.
    //!
    //! \param val the letter to check.
    //!
    //! \returns A value of type \c bool.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Constant on average, worst case linear in the size of the alphabet.
    [[nodiscard]] bool in_alphabet(letter_type val) const {
      return _alphabet_map.find(val) != _alphabet_map.cend();
    }

    //! \brief Add a rule to the presentation.
    //!
    //! Adds the rule with left-hand side `[lhs_begin, lhs_end)` and
    //! right-hand side `[rhs_begin, rhs_end)` to the rules. It is possible to
    //! add rules directly via the data member \ref rules, this function just
    //! exists to encourage adding rules with both sides defined at the same
    //! time.
    //!
    //! \tparam Iterator1 the type of the first two parameters (iterators, or
    //! pointers).
    //! \tparam Iterator2 the type of the second two parameters (iterators,
    //! or pointers).
    //!
    //! \param lhs_begin an iterator pointing to the first letter of the
    //! left-hand side of the rule to be added.
    //! \param lhs_end an iterator pointing one past the last letter of the
    //! left-hand side of the rule to be added.
    //! \param rhs_begin an iterator pointing to the first letter of the
    //! right-hand side of the rule to be added.
    //! \param rhs_end an iterator pointing one past the last letter of the
    //! right-hand side of the rule to be added.
    //! \returns A reference to \c *this.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Amortized constant.
    //!
    //! \warning
    //! It is not checked that the arguments describe words over the alphabet
    //! of the presentation.
    //!
    //! \sa
    //! * \ref add_rule
    template <typename Iterator1, typename Iterator2>
    Presentation& add_rule_no_checks(Iterator1 lhs_begin,
                                     Iterator1 lhs_end,
                                     Iterator2 rhs_begin,
                                     Iterator2 rhs_end) {
      rules.emplace_back(lhs_begin, lhs_end);
      rules.emplace_back(rhs_begin, rhs_end);
      return *this;
    }

    //! \brief Add a rule to the presentation and check it is valid.
    //!
    //! After checking that the left-hand side and right-hand side only contain
    //! letters in \ref alphabet, this function performs the same as
    //! `add_rule_no_checks(lhs_begin, lhs_end, rhs_begin, rhs_end)`.
    //!
    //! \throws LibsemigroupsException if any letter does not belong to the
    //! alphabet.
    //! \throws LibsemigroupsException if \ref contains_empty_word returns
    //! \c false and \p lhs_begin equals \p lhs_end or \p rhs_begin equals
    //! \p rhs_end.
    //!
    //! \sa
    //! * \ref add_rule_no_checks
    template <typename Iterator1, typename Iterator2>
    Presentation& add_rule(Iterator1 lhs_begin,
                           Iterator1 lhs_end,
                           Iterator2 rhs_begin,
                           Iterator2 rhs_end) {
      throw_if_letter_not_in_alphabet(lhs_begin, lhs_end);
      throw_if_letter_not_in_alphabet(rhs_begin, rhs_end);
      return add_rule_no_checks(lhs_begin, lhs_end, rhs_begin, rhs_end);
    }

    //! \brief Add a generator.
    //!
    //! Add the first letter not in the alphabet as a generator and return this
    //! letter.
    //!
    //! \returns A value of type \ref letter_type.
    //!
    //! \throws LibsemigroupsException if the alphabet is of the maximum
    //! possible size supported by `letter_type`.
    letter_type add_generator();

    //! \brief Add \p x as a generator.
    //!
    //! Add the letter \p x as a generator.
    //!
    //! \param x the letter to add as a generator.
    //!
    //! \returns A reference to \c *this.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    Presentation& add_generator_no_checks(letter_type x);

    //! \brief Add \p x as a generator.
    //!
    //! Add the letter \p x as a generator.
    //!
    //! \param x the letter to add as a generator.
    //!
    //! \returns A reference to \c *this.
    //!
    //! \throws LibsemigroupsException if \p x is in `p.alphabet()`.
    Presentation& add_generator(letter_type x);

    //! \brief Remove \p x as a generator.
    //!
    //! Remove the letter \p x as a generator.
    //!
    //! \param x the letter to remove as a generator.
    //!
    //! \returns A reference to \c *this.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Average case: linear in the length of the alphabet, worst case:
    //! quadratic in the length of the alphabet.
    //!
    //! \warning This function does no checks on its arguments whatsoever. In
    //! particular, if the letter \p x is not a generator, then bad things will
    //! happen.
    Presentation& remove_generator_no_checks(letter_type x);

    //! \brief Remove \p x as a generator.
    //!
    //! Remove the letter \p x as a generator.
    //!
    //! \param x the letter to remove as a generator.
    //!
    //! \returns A reference to \c *this.
    //!
    //! \throws LibsemigroupsException if \p x is not in `p.alphabet()`.
    //!
    //! \complexity
    //! Average case: linear in the length of the alphabet, worst case:
    //! quadratic in the length of the alphabet.
    Presentation& remove_generator(letter_type x);

    //! \brief Return whether the empty word is a valid relation word.
    //!
    //! Returns \c true if the empty word is a valid relation word, and \c false
    //! if the empty word is not a valid relation word.
    //!
    //! If the presentation is not allowed to contain the empty word (according
    //! to this function), the presentation may still be isomorphic to a monoid,
    //! but is not given as a quotient of a free monoid.
    //!
    //! \returns A value of type `bool`.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    [[nodiscard]] bool contains_empty_word() const noexcept {
      return _contains_empty_word;
    }

    //! \brief Set whether whether the empty word is a valid relation word.
    //!
    //! Specify whether the empty word should be a valid relation word
    //! (corresponding to \p val being `true`), or not (corresponding to
    //! \p val being `false`).
    //!
    //! If the presentation is not allowed to contain the empty word (according
    //! to the value specified here), the presentation may still be isomorphic
    //! to a monoid, but is not given as a quotient of a free monoid.
    //!
    //! \param val whether the presentation can contain the empty word.
    //!
    //! \returns A reference to \c *this.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    Presentation& contains_empty_word(bool val) noexcept {
      _contains_empty_word = val;
      return *this;
    }

    //! \brief Check if the alphabet is valid.
    //!
    //! Check if the alphabet is valid.
    //!
    //! \throws LibsemigroupsException if there are duplicate letters in the
    //! alphabet.
    //!
    //! \complexity
    //! Linear in the length of the alphabet.
    void throw_if_alphabet_has_duplicates() const {
      decltype(_alphabet_map) alphabet_map;
      throw_if_alphabet_has_duplicates(alphabet_map);
    }

    //! \brief Check if a letter belongs to the alphabet or not.
    //!
    //! Check if a letter belongs to the alphabet or not.
    //!
    //! \param c the letter to check.
    //!
    //! \throws LibsemigroupsException if \p c does not belong to the alphabet.
    //!
    //! \complexity
    //! Constant on average, worst case linear in the size of the alphabet.
    void throw_if_letter_not_in_alphabet(letter_type c) const;

    //! \brief Check if every letter in a range belongs to the alphabet.
    //!
    //! Check if every letter in a range belongs to the alphabet.
    //!
    //! \tparam Iterator the type of the arguments (iterators).
    //! \param first iterator pointing at the first letter to check.
    //! \param last iterator pointing one beyond the last letter to check.
    //!
    //! \throws LibsemigroupsException if there is a letter not in the
    //! alphabet between \p first and \p last
    //!
    //! \complexity
    //! Worst case \f$O(mn)\f$ where \f$m\f$ is the length of the longest
    //! word, and \f$n\f$ is the size of the alphabet.
    template <typename Iterator1, typename Iterator2>
    void throw_if_letter_not_in_alphabet(Iterator1 first, Iterator2 last) const;

    //! \brief Check if every word in every rule consists only of letters
    //! belonging to the alphabet.
    //!
    //! Check if every word in every rule consists only of letters belonging to
    //! the alphabet, and that there are an even number of words in
    //! `this->rules`.
    //!
    //! \throws LibsemigroupsException if any word contains a letter not in
    //! the alphabet.
    //! \throws LibsemigroupsException if the number of words in `this->rules`
    //! is odd.
    //!
    //! \complexity
    //! Worst case \f$O(mnt)\f$ where \f$m\f$ is the length of the longest
    //! word, \f$n\f$ is the size of the alphabet and \f$t\f$ is the number of
    //! rules.
    void throw_if_bad_rules() const;

    //! \brief Check if the alphabet and rules are valid.
    //!
    //! Check if the alphabet and rules are valid.
    //!
    //! \throws LibsemigroupsException if \ref throw_if_alphabet_has_duplicates
    //! or \ref throw_if_bad_rules does.
    //!
    //! \complexity
    //! Worst case \f$O(mnp)\f$ where \f$m\f$ is the length of length of the
    //! word, \f$n\f$ is the size of the alphabet and \f$p\f$ is the number of
    //! rules.
    void throw_if_bad_alphabet_or_rules() const {
      throw_if_alphabet_has_duplicates();
      throw_if_bad_rules();
    }

   private:
    void try_set_alphabet(decltype(_alphabet_map)& alphabet_map,
                          word_type&               old_alphabet);
    void throw_if_alphabet_has_duplicates(
        decltype(_alphabet_map)& alphabet_map) const;
  };  // class Presentation

  //! \ingroup presentations_group
  //!
  //! \brief Deduction guide.
  //!
  //! Defined in `presentation.hpp`
  //!
  //! Deduction guide to construct a `Presentation<Word>` from a
  //! `Presentation<Word> const&`.
  template <typename Word>
  Presentation(Presentation<Word> const&) -> Presentation<Word>;

  //! \ingroup presentations_group
  //!
  //! \brief Deduction guide.
  //!
  //! Defined in `presentation.hpp`
  //!
  //! Deduction guide to construct a `Presentation<Word>` from a
  //! `Presentation<Word>&&`.
  template <typename Word>
  Presentation(Presentation<Word>&&) -> Presentation<Word>;

  //! \ingroup presentations_group
  //!
  //! \brief Namespace for Presentation helper functions.
  //!
  //! Defined in `presentation.hpp`.
  //!
  //! This namespace contains various helper functions for the class
  //! \ref Presentation. These functions could be functions of \ref Presentation
  //! but they only use public member functions of \ref Presentation, and so
  //! they are declared as free functions instead.
  namespace presentation {

    //! \brief Throw if the distance between iterators is not even.
    //!
    //! This function throws an exception if the distance between \p first and
    //! \p last is not an even number.
    //!
    //! \tparam Iterator the type of the arguments.
    //!
    //! \param first iterator pointing at the first words.
    //! \param last iterator pointing one beyond the last word.
    //!
    //! \throws LibsemigroupsException if the distance from \p first to \p last
    //! is not even.
    template <typename Iterator>
    void throw_if_odd_number_of_rules(Iterator first, Iterator last) {
      if ((std::distance(first, last) % 2) == 1) {
        LIBSEMIGROUPS_EXCEPTION(
            "expected even number of words in \"rules\", found {}",
            std::distance(first, last));
      }
    }

    //! \brief Throw if the number of words in a presentation is odd.
    //!
    //! This function throws an exception if number of words in `p.rules` is
    //! odd.
    //!
    //! \tparam Word the type of the words in the Presentation \p p.
    //!
    //! \param p the presentation to check.
    //!
    //! \throws LibsemigroupsException if the number of words in `p.rules`
    //! is not even.
    template <typename Word>
    void throw_if_odd_number_of_rules(Presentation<Word> const& p) {
      throw_if_odd_number_of_rules(p.rules.cbegin(), p.rules.cend());
    }

    //! \brief Throws if the presentation isn't normalized.
    //!
    //! This function throws a LibsemigroupsException if the
    //! \ref Presentation::alphabet of \p p is not `0` to
    //! `p.alphabet().size() - 1`.
    //!
    //! The second parameter \p arg is used in the formatting of the exception
    //! message to specify which parameter the presentation \p p corresponds to
    //! in the calling function.
    //!
    //! \param p the presentation to check.
    //! \param arg the position of \p p in calling function's argument list
    //! (defaults to `"1st"`).
    //!
    //! \throws LibsemigroupsException if the alphabet of \p p is not `0` to
    //! `p.alphabet.size()`.
    //!
    //! \sa \ref is_normalized.
    template <typename Word>
    void throw_if_not_normalized(Presentation<Word> const& p,
                                 std::string_view          arg = "1st");

    //! \brief Check if the presentation is normalized.
    //!
    //! This function returns `true` if the \ref Presentation::alphabet of
    //! \p p is `0` to `p.alphabet().size() - 1` and `false` otherwise.
    //!
    //! \param p the presentation to check.
    //!
    //! \returns Whether or not the presentation \p p is normalized.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \sa \ref throw_if_not_normalized.
    template <typename Word>
    bool is_normalized(Presentation<Word> const& p);

    //! \brief Check rules against the alphabet of \p p.
    //!
    //! Check if every rule of in `[first, last)` consists of letters belonging
    //! to the alphabet of \p p.
    //!
    //! \tparam Word the type of the words in the presentation.
    //! \tparam Iterator the type of the second and third arguments.
    //! \param p the presentation whose alphabet is being checked against.
    //! \param first iterator pointing at the first rule to check.
    //! \param last iterator pointing one beyond the last rule to check.
    //!
    //! \throws LibsemigroupsException if any word contains a letter not in
    //! `p.alphabet()`.
    //!
    //! \complexity
    //! Worst case \f$O(mnt)\f$ where \f$m\f$ is the length of the longest
    //! word, \f$n\f$ is the size of the \p p 's alphabet and \f$t\f$ is the
    //! distance between \p first and \p last.
    template <typename Word, typename Iterator>
    void throw_if_bad_rules(Presentation<Word> const& p,
                            Iterator                  first,
                            Iterator                  last) {
      // TODO(0) check if there are an odd number of rules
      for (auto it = first; it != last; ++it) {
        p.throw_if_letter_not_in_alphabet(it->cbegin(), it->cend());
      }
    }

    //! \brief Throws an exception if \p vals do not define valid inverses.
    //!
    //! This function checks if the values in \p inverses are valid semigroup
    //! inverses for `p.alphabet()`. Specifically, it checks that the \f$i\f$th
    //! value in \p inverses is an inverse for the \f$i\f$th value in
    //! `p.alphabet()`.
    //!
    //! Let \f$x_i\f$ be the \f$i\f$th letter in `p.alphabet()`, and
    //! let \f$y_i\f$ be the \f$i\f$th letter in \p inverses. Then this function
    //! checks that:
    //! * `p.alphabet()` and \p inverses contain the same letters;
    //! * \p inverses are duplicate-free;
    //! * if \f$x_i = y_j\f$, then \f$x_j = y_i\f$ and therefore that
    //! \f$(x_i^{-1})^{-1} = x_i\f$.
    //!
    //! \tparam Word1 the type of the words in the presentation.
    //! \tparam Word2 the type of the arguments \p letters and \p inverses.
    //! \param p the presentation.
    //! \param inverses the proposed inverses for \p letters.
    //!
    //! \throws Libsemigroups_Exception if any of the conditions listed above
    //! do not hold.
    template <typename Word1, typename Word2>
    void throw_if_bad_inverses(Presentation<Word1> const& p,
                               Word2 const&               inverses);

    //! \brief Throws an exception if the argument \p inverses does not define
    //! valid inverses for \p letters.
    //!
    //! This function checks if the values in \p inverses are valid semigroup
    //! inverses for \p letters. Specifically, it
    //! checks that the \f$i\f$th value in \p inverses is an inverse for the
    //! \f$i\f$th value in `letters`.
    //!
    //! Let \f$x_i\f$ be the \f$i\f$th letter in \p letters, and
    //! let \f$y_i\f$ be the \f$i\f$th letter in \p inverses. Then this function
    //! checks that:
    //! * \p letters and \p inverses contain the same letters;
    //! * \p letters and \p inverses are duplicate-free;
    //! * if \f$x_i = y_j\f$, then \f$x_j = y_i\f$ and therefore that
    //! \f$(x_i^{-1})^{-1} = x_i\f$.
    //!
    //! \tparam Word1 the type of the words in the presentation.
    //! \tparam Word2 the type of the arguments \p letters and \p inverses.
    //! \param p the presentation.
    //! \param letters the letters in the alphabet.
    //! \param inverses the proposed inverses for \p letters.
    //!
    //! \throws Libsemigroups_Exception if any of the conditions listed above
    //! do not hold.
    template <typename Word1, typename Word2>
    void throw_if_bad_inverses(Presentation<Word1> const& p,
                               Word2 const&               letters,
                               Word2 const&               inverses);

    //! \brief Return a representation of a presentation to appear in the
    //! reporting output.
    //!
    //! Return a representation of a presentation that will appear in the
    //! reporting output. The information that is provided is:
    //! - the size of the alphabet (`|A|`);
    //! - the number of rules (`|R|`);
    //! - the range of values of the lengths of the rules (`|u| +  |v|`); and
    //! - the sum of the lengths of the rules (`∑(|u| + |v|)`).
    //!
    //! \tparam Word the type of the words in the presentation.
    //! \param p the presentation.
    //!
    //! \returns A value of type `std::string`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \par Example
    //! \code
    //! Presentation<std::string> p;
    //! presentation::to_report_string(p)
    //! // "|A| = 0, |R| = 0, |u| + |v| ∈ [0, 0], ∑(|u| + |v|) = 0"
    //! \endcode
    template <typename Word>
    std::string to_report_string(Presentation<Word> const& p);

    //! \brief Add a rule to the presentation by reference.
    //!
    //! Adds the rule with left-hand side \p lhop and right-hand side \p rhop
    //! to the rules.
    //!
    //! \tparam Word the type of the words in the presentation.
    //! \param p the presentation.
    //! \param lhop the left-hand side of the rule.
    //! \param rhop the right-hand side of the rule.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \warning
    //! No checks that the arguments describe words over the alphabet of the
    //! presentation are performed.
    template <typename Word>
    void add_rule_no_checks(Presentation<Word>& p,
                            Word const&         lhop,
                            Word const&         rhop) {
      p.add_rule_no_checks(lhop.begin(), lhop.end(), rhop.begin(), rhop.end());
    }

    //! \brief Add a rule to the presentation by reference and check.
    //!
    //! Adds the rule with left-hand side \p lhop and right-hand side \p rhop
    //! to the rules, after checking that \p lhop and \p rhop consist entirely
    //! of letters in the alphabet of \p p.
    //!
    //! \tparam Word the type of the words in the presentation.
    //! \param p the presentation.
    //! \param lhop the left-hand side of the rule.
    //! \param rhop the right-hand side of the rule.
    //!
    //! \throws LibsemigroupsException if \p lhop or \p rhop contains any
    //! letters not belonging to `p.alphabet()`.
    template <typename Word>
    void add_rule(Presentation<Word>& p, Word const& lhop, Word const& rhop) {
      p.add_rule(lhop.begin(), lhop.end(), rhop.begin(), rhop.end());
    }

    //! \brief Add a rule to the presentation by `char const*`.
    //!
    //! Adds the rule with left-hand side \p lhop and right-hand side \p rhop
    //! to the rules.
    //!
    //! \param p the presentation.
    //! \param lhop the left-hand side of the rule.
    //! \param rhop the right-hand side of the rule.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \warning
    //! No checks that the arguments describe words over the alphabet of the
    //! presentation are performed.
    void add_rule_no_checks(Presentation<std::string>& p,
                            char const*                lhop,
                            char const*                rhop);

    //! \brief Add a rule to the presentation by `char const*`.
    //!
    //! Adds the rule with left-hand side \p lhop and right-hand side \p rhop
    //! to the rules.
    //!
    //! \param p the presentation.
    //! \param lhop the left-hand side of the rule.
    //! \param rhop the right-hand side of the rule.
    //!
    //! \throws LibsemigroupsException if \p lhop or \p rhop contains any
    //! letters not belonging to `p.alphabet()`.
    void add_rule(Presentation<std::string>& p,
                  char const*                lhop,
                  char const*                rhop);

    //! \brief Add a rule to the presentation by `string const &` and
    //! `char const*`.
    //!
    //! Adds the rule with left-hand side \p lhop and right-hand side \p rhop
    //! to the rules.
    //!
    //! \param p the presentation.
    //! \param lhop the left-hand side of the rule.
    //! \param rhop the right-hand side of the rule.
    //!
    //! \throws LibsemigroupsException if \p lhop or \p rhop contains any
    //! letters not belonging to `p.alphabet()`.
    void add_rule(Presentation<std::string>& p,
                  std::string const&         lhop,
                  char const*                rhop);

    //! \brief Add a rule to the presentation by `char const*` and
    //! `string const &`.
    //!
    //! Adds the rule with left-hand side \p lhop and right-hand side \p rhop
    //! to the rules.
    //!
    //! \param p the presentation.
    //! \param lhop the left-hand side of the rule.
    //! \param rhop the right-hand side of the rule.
    //!
    //! \throws LibsemigroupsException if \p lhop or \p rhop contains any
    //! letters not belonging to `p.alphabet()`.
    void add_rule(Presentation<std::string>& p,
                  char const*                lhop,
                  std::string const&         rhop);

    //! \brief Add a rule to the presentation by `string const&` and
    //! `char const*`.
    //!
    //! Adds the rule with left-hand side \p lhop and right-hand side \p rhop
    //! to the rules.
    //!
    //! \param p the presentation.
    //! \param lhop the left-hand side of the rule.
    //! \param rhop the right-hand side of the rule.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \warning
    //! No checks that the arguments describe words over the alphabet of the
    //! presentation are performed.
    void add_rule_no_checks(Presentation<std::string>& p,
                            std::string const&         lhop,
                            char const*                rhop);

    //! \brief Add a rule to the presentation by `char const*` and
    //! `string const&`.
    //!
    //! Adds the rule with left-hand side \p lhop and right-hand side \p rhop
    //! to the rules.
    //!
    //! \param p the presentation.
    //! \param lhop the left-hand side of the rule.
    //! \param rhop the right-hand side of the rule.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \warning
    //! No checks that the arguments describe words over the alphabet of the
    //! presentation are performed.
    void add_rule_no_checks(Presentation<std::string>& p,
                            char const*                lhop,
                            std::string const&         rhop);

    //! \brief Add a rule to the presentation by `initializer_list`.
    //!
    //! Adds the rule with left-hand side \p lhop and right-hand side \p rhop
    //! to the rules.
    //!
    //! \tparam Word the type of the words in the presentation.
    //! \tparam Letter the type of the values in the `initializer_list`.
    //! \param p the presentation.
    //! \param lhop the left-hand side of the rule.
    //! \param rhop the right-hand side of the rule.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \warning
    //! No checks that the arguments describe words over the alphabet of the
    //! presentation are performed.
    template <typename Word, typename Letter>
    void add_rule_no_checks(Presentation<Word>&           p,
                            std::initializer_list<Letter> lhop,
                            std::initializer_list<Letter> rhop) {
      p.add_rule_no_checks(lhop.begin(), lhop.end(), rhop.begin(), rhop.end());
    }

    //! \brief Add a rule to the presentation by `initializer_list`.
    //!
    //! Adds the rule with left-hand side \p lhop and right-hand side \p rhop
    //! to the rules.
    //!
    //! \tparam Word the type of the words in the presentation.
    //! \tparam Letter the type of the values in the `initializer_list`.
    //! \param p the presentation.
    //! \param lhop the left-hand side of the rule.
    //! \param rhop the right-hand side of the rule.
    //!
    //! \throws LibsemigroupsException if \p lhop or \p rhop contains any
    //! letters not belonging to `p.alphabet()`.
    template <typename Word, typename Letter>
    void add_rule(Presentation<Word>&           p,
                  std::initializer_list<Letter> lhop,
                  std::initializer_list<Letter> rhop) {
      p.add_rule(lhop.begin(), lhop.end(), rhop.begin(), rhop.end());
    }

    //! \brief Add the rules stored in the range `[first, last)`.
    //!
    //! This function adds the rules stored in the range `[first, last)` to
    //! \p p.
    //!
    //! Before it is added, each rule is checked to check it contains
    //! only letters of the alphabet of \p p. If the \f$n\f$th rule causes this
    //! function to throw, the first \f$n-1\f$ rules will still be added to
    //! \p p.
    //!
    //! \tparam Word the type of the words in the presentation.
    //! \tparam Iterator the type of the second and third arguments.
    //! \param p the presentation.
    //! \param first iterator pointing at the first rule to add.
    //! \param last iterator pointing one beyond the last rule to add.
    //!
    //! \throws LibsemigroupsException if any rule contains any letters not
    //! belonging to `p.alphabet()`.
    template <typename Word, typename Iterator>
    void add_rules(Presentation<Word>& p, Iterator first, Iterator last) {
      for (auto it = first; it != last; it += 2) {
        add_rule(p, *it, *(it + 1));
      }
    }

    //! \brief Add the rules stored in the range `[first, last)`.
    //!
    //! This function adds the rules stored in the range `[first, last)` to the
    //! presentation \p p.
    //!
    //! \tparam Word the type of the words in the presentation.
    //! \tparam Iterator the type of the second and third arguments.
    //! \param p the presentation.
    //! \param first iterator pointing at the first rule to add.
    //! \param last iterator pointing one beyond the last rule to add.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \warning
    //! No checks that the arguments describe words over the alphabet of the
    //! presentation are performed.
    template <typename Word, typename Iterator>
    void add_rules_no_checks(Presentation<Word>& p,
                             Iterator            first,
                             Iterator            last) {
      for (auto it = first; it != last; it += 2) {
        add_rule_no_checks(p, *it, *(it + 1));
      }
    }

    //! \brief Add the rules of \p q to \p p.
    //!
    //! Adds all the rules of the second argument \p q to the first argument
    //! \p p which is modified in-place.
    //!
    //! \tparam Word the type of the words in the presentation.
    //! \param p the presentation to add rules to.
    //! \param q the presentation with the rules to add.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \warning
    //! No checks that the arguments describe words over the alphabet of the
    //! presentation are performed.
    template <typename Word>
    void add_rules_no_checks(Presentation<Word>&       p,
                             Presentation<Word> const& q) {
      add_rules_no_checks(p, q.rules.cbegin(), q.rules.cend());
    }

    //! \brief Add the rules of \p q to \p p.
    //!
    //! Adds all the rules of the second argument \p q to the first argument
    //! \p p which is modified in-place.
    //!
    //! Before it is added, each rule is checked to check it contains
    //! only letters of the alphabet of \p p. If the \f$n\f$th rule causes this
    //! function to throw, the first \f$n-1\f$ rules will still be added to
    //! \p p.
    //!
    //! \tparam Word the type of the words in the presentation.
    //! \param p the presentation to add rules to.
    //! \param q the presentation with the rules to add.
    //!
    //! \throws LibsemigroupsException if any rule contains any letters not
    //! belonging to `p.alphabet()`.
    template <typename Word>
    void add_rules(Presentation<Word>& p, Presentation<Word> const& q) {
      add_rules(p, q.rules.cbegin(), q.rules.cend());
    }

    //! \brief Check if a presentation contains a rule.
    //!
    //! Checks if the rule with left-hand side \p lhs and right-hand side \p rhs
    //! is contained in \p p.
    //!
    //! \tparam Word the type of the words in the presentation.
    //! \param p the presentation.
    //! \param lhs the left-hand side of the rule.
    //! \param rhs the right-hand side of the rule.
    //!
    //! \returns A value of type `bool`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Linear in the number of rules.
    template <typename Word>
    [[nodiscard]] bool contains_rule(Presentation<Word>& p,
                                     Word const&         lhs,
                                     Word const&         rhs);

    //! \brief Add rules for an identity element.
    //!
    //! Adds rules of the form \f$ae = ea = a\f$ for every letter \f$a\f$ in
    //! the alphabet of \p p, and where \f$e\f$ is the second parameter.
    //!
    //! \tparam Word the type of the words in the presentation.
    //! \param p the presentation to add rules to.
    //! \param e the identity element.
    //!
    //! \throws LibsemigroupsException if \p e is not a letter in
    //! `p.alphabet()`.
    //!
    //! \complexity
    //! Linear in the number of rules.
    template <typename Word>
    void add_identity_rules(Presentation<Word>&                      p,
                            typename Presentation<Word>::letter_type e);

    //! \brief Add rules for a zero element.
    //!
    //! Adds rules of the form \f$az = za = z\f$ for every letter \f$a\f$ in
    //! the alphabet of \p p, and where \f$z\f$ is the second parameter.
    //!
    //! \tparam Word the type of the words in the presentation.
    //! \param p the presentation to add rules to.
    //! \param z the zero element.
    //!
    //! \throws LibsemigroupsException if \p z is not a letter in
    //! `p.alphabet()`.
    //!
    //! \complexity
    //! Linear in the number of rules.
    template <typename Word>
    void add_zero_rules(Presentation<Word>&                      p,
                        typename Presentation<Word>::letter_type z);

    //! \brief Add rules for inverses.
    //!
    //! The letter \c a with index \c i in \p vals is the inverse of the
    //! letter in `alphabet()` with index \c i. The rules added are \f$a_ib_i
    //! = e\f$ where the alphabet is \f$\{a_1, \ldots, a_n\}\f$; the 2nd
    //! parameter \p vals is \f$\{b_1, \ldots, b_n\}\f$; and \f$e\f$ is the
    //! 3rd parameter.
    //!
    //! \tparam Word the type of the words in the presentation.
    //! \param p the presentation to add rules to.
    //! \param vals the inverses.
    //! \param e the identity element (defaults to \ref UNDEFINED, meaning use
    //! the empty word).
    //!
    //! \throws LibsemigroupsException if any of the following apply:
    //! * the length of \p vals is not equal to `alphabet().size()`;
    //! * the letters in \p vals are not exactly those in `alphabet()`
    //! (perhaps in a different order);
    //! * \f$(a_i ^ {-1}) ^ {-1} = a_i\f$ does not hold for some \f$i\f$;
    //! * \f$e ^ {-1} = e\f$ does not hold
    //!
    //! \complexity
    //! \f$O(n)\f$ where \f$n\f$ is `p.alphabet().size()`.
    template <typename Word>
    void add_inverse_rules(Presentation<Word>&                      p,
                           Word const&                              vals,
                           typename Presentation<Word>::letter_type e
                           = UNDEFINED);

    //! \brief Add rules for inverses.
    //!
    //! The letter \c a with index \c i in \p vals is the inverse of the
    //! letter in `alphabet()` with index \c i. The rules added are \f$a_ib_i
    //! = e\f$ where the alphabet is \f$\{a_1, \ldots, a_n\}\f$; the 2nd
    //! parameter \p vals is \f$\{b_1, \ldots, b_n\}\f$; and \f$e\f$ is the
    //! 3rd parameter.
    //!
    //! \tparam Word the type of the words in the presentation.
    //! \param p the presentation to add rules to.
    //! \param vals the inverses.
    //! \param e the identity element (defaults to \ref UNDEFINED, meaning use
    //! the empty word).
    //!
    //! \throws LibsemigroupsException if any of the following apply:
    //! * the length of \p vals is not equal to `alphabet().size()`;
    //! * the letters in \p vals are not exactly those in `alphabet()`
    //! (perhaps in a different order);
    //! * \f$(a_i ^ {-1}) ^ {-1} = a_i\f$ does not hold for some \f$i\f$;
    //! * \f$e ^ {-1} = e\f$ does not hold
    //!
    //! \complexity
    //! \f$O(n)\f$ where \f$n\f$ is `p.alphabet().size()`.
    void add_inverse_rules(Presentation<std::string>& p,
                           char const*                vals,
                           char                       e = UNDEFINED);

    //! \brief Remove duplicate rules.
    //!
    //! Removes all but one instance of any duplicate rules (if any). Note
    //! that rules of the form \f$u = v\f$ and \f$v = u\f$ (if any) are
    //! considered duplicates. Also note that the rules may be reordered by
    //! this function even if there are no duplicate rules.
    //!
    //! \tparam Word the type of the words in the presentation.
    //! \param p the presentation.
    //!
    //! \throws LibsemigroupsException if `p.rules.size()` is odd.
    //!
    //! \complexity
    //! Linear in the number of rules.
    template <typename Word>
    void remove_duplicate_rules(Presentation<Word>& p);

    //! \brief Remove rules consisting of identical words.
    //!
    //! Removes all instance of rules (if any) where the left-hand side and
    //! the right-hand side are identical.
    //!
    //! \tparam Word the type of the words in the presentation.
    //! \param p the presentation.
    //!
    //! \throws LibsemigroupsException if `p.rules.size()` is odd.
    //!
    //! \complexity
    //! Linear in the number of rules.
    template <typename Word>
    void remove_trivial_rules(Presentation<Word>& p);

    //! \brief If there are rules \f$u = v\f$ and \f$v = w\f$ where \f$|w| <
    //! |v|\f$, then replace \f$u = v\f$ by \f$u = w\f$.
    //!
    //! Attempts to reduce the length of the words by finding the equivalence
    //! relation on the relation words generated by the pairs of identical
    //! relation words. If \f$\{u_1, u_2, \ldots, u_n\}\f$ are the distinct
    //! words in an equivalence class and \f$u_1\f$ is the short-lex minimum
    //! word in the class, then the relation words are replaced by \f$u_1 =
    //! u_2, u_1 = u_3, \cdots, u_1 = u_n\f$.
    //!
    //! The rules may be reordered by this function even if there are no
    //! reductions found.
    //!
    //! \tparam Word the type of the words in the presentation.
    //! \param p the presentation to add rules to.
    //!
    //! \throws LibsemigroupsException if `p.rules.size()` is odd.
    //!
    //! \note This function is non-deterministic; different results may be
    //! obtained when compiling with clang vs gcc
    template <typename Word>
    void reduce_complements(Presentation<Word>& p);

    //! \brief Sort the left-hand and right-hand side of each rule by shortlex.
    //!
    //! Sort each rule \f$u = v\f$ so that the left-hand side is shortlex
    //! greater than the right-hand side.
    //!
    //! \tparam Word the type of the words in the presentation.
    //! \param p the presentation whose rules should be sorted.
    //!
    //! \throws LibsemigroupsException if `p.rules.size()` is odd.
    //!
    //! \complexity
    //! Linear in the number of rules.
    template <typename Word>
    bool sort_each_rule(Presentation<Word>& p);

    //! \brief Sort the left-hand and right-hand side of each rule relative to
    //! \p cmp.
    //!
    //! Sort each rule \f$u = v\f$ so that the left-hand side is greater than
    //! the right-hand side with respect to \p cmp.
    //!
    //! \tparam Word the type of the words in the presentation.
    //! \tparam Compare the type of the compare function.
    //! \param p the presentation whose rules should be sorted.
    //! \param cmp the comparison function.
    //!
    //! \throws LibsemigroupsException if `p.rules.size()` is odd.
    //!
    //! \complexity
    //! Linear in the number of rules.
    template <typename Word, typename Compare>
    bool sort_each_rule(Presentation<Word>& p, Compare cmp);

    // TODO(later) is_each_rule_sorted?

    //! \brief Sort all of the rules by \p cmp.
    //!
    //! Sort the rules \f$u_1 = v_1, \ldots, u_n = v_n\f$ so that
    //! \f$u_1v_1 < \cdots < u_nv_n\f$ where \f$<\f$ is the order defined by
    //! \p cmp.
    //!
    //! \tparam Word the type of the words in the presentation.
    //! \tparam Compare the type of the compare function.
    //! \param p the presentation to sort.
    //! \param cmp the comparison function.
    //!
    //! \throws LibsemigroupsException if `p.rules.size()` is odd.
    template <typename Word, typename Compare>
    void sort_rules(Presentation<Word>& p, Compare cmp);

    //! \brief Sort all of the rules by shortlex.
    //!
    //! Sort the rules \f$u_1 = v_1, \ldots, u_n = v_n\f$ so that
    //! \f$u_1v_1 < \cdots < u_nv_n\f$ where \f$<\f$ is the shortlex order.
    //!
    //! \tparam Word the type of the words in the presentation.
    //! \param p the presentation to sort.
    //!
    //! \throws LibsemigroupsException if `p.rules.size()` is odd.
    template <typename Word>
    void sort_rules(Presentation<Word>& p) {
      sort_rules(p, ShortLexCompare());
    }

    //! \brief Check the rules are sorted relative to \p cmp .
    //!
    //! Check if the rules \f$u_1 = v_1, \ldots, u_n = v_n\f$ satisfy
    //! \f$u_1v_1 < \cdots < u_nv_n\f$ where \f$<\f$ is the order described by
    //! \p cmp .
    //!
    //! \tparam Word the type of the words in the presentation.
    //! \tparam Compare the type of the compare function.
    //! \param p the presentation to check.
    //! \param cmp the comparison function.
    //!
    //! \returns A value of type `bool`.
    //!
    //! \throws LibsemigroupsException if `p.rules.size()` is odd.
    //!
    //! \sa
    //! * \ref sort_rules(Presentation<Word>& p, Compare cmp)
    template <typename Word, typename Compare>
    bool are_rules_sorted(Presentation<Word> const& p, Compare cmp);

    //! \brief Check the rules are sorted relative to shortlex.
    //!
    //! Check if the rules \f$u_1 = v_1, \ldots, u_n = v_n\f$ satisfy
    //! \f$u_1v_1 < \cdots < u_nv_n\f$ where \f$<\f$ is shortlex order.
    //!
    //! \tparam Word the type of the words in the presentation.
    //! \param p the presentation to check.
    //!
    //! \returns A value of type `bool`.
    //!
    //! \throws LibsemigroupsException if `p.rules.size()` is odd.
    //!
    //! \sa
    //! * \ref sort_rules(Presentation<Word>& p)
    template <typename Word>
    bool are_rules_sorted(Presentation<Word> const& p) {
      return are_rules_sorted(p, ShortLexCompare());
    }

    //! \brief Return the longest common subword of the rules.
    //!
    //! If it is possible to find a subword \f$w\f$ of the rules \f$u_1 = v_1,
    //! \ldots, u_n = v_n\f$ such that the introduction of a new generator
    //! \f$z\f$ and the relation \f$z = w\f$ reduces the `presentation::length`
    //! of the presentation, then this function returns the longest such word
    //! \f$w\f$. If no such word can be found, then a word of length \f$0\f$ is
    //! returned.
    //!
    //! \tparam Word the type of the words in the presentation.
    //! \param p the presentation.
    //!
    //! \returns A value of type \p Word.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    // TODO(later) complexity
    template <typename Word>
    Word longest_subword_reducing_length(Presentation<Word>& p);

    //! \brief Replace non-overlapping instances of a subword via iterators.
    //!
    //! If \f$w=\f$`[first, last)` is a word, then this function replaces
    //! every non-overlapping instance of \f$w\f$ in every rule, adds a new
    //! generator \f$z\f$, and the rule \f$w = z\f$. The new generator and
    //! rule are added even if \f$w\f$ is not a subword of any rule.
    //!
    //! \tparam Word the type of the words in the presentation.
    //! \tparam Iterator the type of the 2nd and 3rd parameters (iterators).
    //! \param p the presentation.
    //! \param first the start of the subword to replace.
    //! \param last one beyond the end of the subword to replace.
    //!
    //! \throws LibsemigroupsException if `first == last`.
    // TODO(later) complexity
    template <typename Word, typename Iterator>
    typename Presentation<Word>::letter_type
    replace_word_with_new_generator(Presentation<Word>& p,
                                    Iterator            first,
                                    Iterator            last);

    //! \brief Replace non-overlapping instances of a word with a new generator
    //! via const reference.
    //!
    //! This function replaces every non-overlapping instance (from left to
    //! right) of \p w in every rule, adds a new generator \f$z\f$, and the rule
    //! \f$w = z\f$. The new generator and rule are added even if \p w  is not a
    //! subword of any rule.
    //!
    //! \tparam Word the type of the words in the presentation.
    //!
    //! \param p the presentation.
    //! \param w the subword to replace.
    //!
    //! \returns The new generator added.
    //!
    //! \throws LibsemigroupsException if \p w is empty.
    // TODO(later) complexity
    template <typename Word>
    typename Presentation<Word>::letter_type
    replace_word_with_new_generator(Presentation<Word>& p, Word const& w) {
      return replace_word_with_new_generator(p, w.cbegin(), w.cend());
    }

    //! \brief Replace non-overlapping instances of a subword via `char const*`.
    //!
    //! If \f$w=\f$`[first, last)` is a word, then replaces every
    //! non-overlapping instance (from left to right) of \f$w\f$ in every
    //! rule, adds a new generator \f$z\f$, and the rule \f$w = z\f$. The new
    //! generator and rule are added even if \f$w\f$ is not a subword of any
    //! rule.
    //!
    //! \param p the presentation.
    //! \param w the subword to replace.
    //!
    //! \returns The new generator added.
    //!
    //! \throws LibsemigroupsException if \p w is empty.
    typename Presentation<std::string>::letter_type
    replace_word_with_new_generator(Presentation<std::string>& p,
                                    char const*                w);

    //! \brief Replace non-overlapping instances of a subword by another word.
    //!
    //! If \p existing and \p replacement are words, then this function
    //! replaces every non-overlapping instance of \p existing in every rule
    //! by \p replacement. The presentation \p p is changed in-place.
    //!
    //! \tparam Word the type of the words in the presentation.
    //! \param p the presentation.
    //! \param existing the word to be replaced.
    //! \param replacement the replacement word.
    //!
    //! \throws LibsemigroupsException if `existing` is empty.
    // TODO(later) complexity
    template <typename Word>
    void replace_subword(Presentation<Word>& p,
                         Word const&         existing,
                         Word const&         replacement);

    //! \brief Replace non-overlapping instances of a subword by another word.
    //!
    //! Replaces every non-overlapping instance of
    //! `[first_existing, last_existing)` in every rule by
    //! `[first_replacement, last_replacement)`. The presentation \p p  is
    //! changed in-place
    //!
    //! \tparam Iterator1 the type of the first two parameters (iterators, or
    //! pointers).
    //! \tparam Iterator2 the type of the second two parameters (iterators,
    //! or pointers).
    //!
    //! \param p the presentation.
    //! \param first_existing an iterator pointing to the first letter of the
    //! existing subword to be replaced.
    //! \param last_existing an iterator pointing one past the last letter of
    //! the existing subword to be replaced.
    //! \param first_replacement an iterator pointing to the first letter of
    //! the replacement word.
    //! \param last_replacement an iterator pointing one past the last letter
    //! of the replacement word.
    //!
    //! \throws LibsemigroupsException if `first_existing == last_existing`.
    template <typename Word, typename Iterator1, typename Iterator2>
    void replace_subword(Presentation<Word>& p,
                         Iterator1           first_existing,
                         Iterator1           last_existing,
                         Iterator2           first_replacement,
                         Iterator2           last_replacement);

    //! \brief Replace non-overlapping instances of a subword by another word by
    //! `const chat*`.
    //!
    //! This function replaces every non-overlapping instance of \p existing in
    //! every rule by \p replacement. The presentation \p p is changed in-place.
    //!
    //! \tparam Word the type of the words in the presentation.
    //! \param p the presentation.
    //! \param existing the word to be replaced.
    //! \param replacement the replacement word.
    //!
    //! \throws LibsemigroupsException if `existing` is empty.
    inline void replace_subword(Presentation<std::string>& p,
                                char const*                existing,
                                char const*                replacement) {
      replace_subword(p,
                      existing,
                      existing + std::strlen(existing),
                      replacement,
                      replacement + std::strlen(replacement));
    }

    //! \brief Replace instances of a word on either side of a rule by another
    //! word.
    //!
    //! If \p existing and \p replacement are words, then this function
    //! replaces every instance of \p existing in every rule of the form
    //! \p existing \f$= w\f$ or \f$w = \f$ \p existing, with the word
    //! \p replacement. The presentation \p p is changed in-place.
    //!
    //! \tparam Word the type of the words in the presentation.
    //!
    //! \param p the presentation.
    //! \param existing the word to be replaced.
    //! \param replacement the replacement word.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    template <typename Word>
    void replace_word(Presentation<Word>& p,
                      Word const&         existing,
                      Word const&         replacement);

    //! \brief Return the sum of the lengths of all values in the range
    //! `[first, last)`.
    //!
    //! Return the sum of the lengths of all values in the range
    //! `[first, last)`.
    //!
    //! \tparam Iterator the type of the first and second arguments (iterators).
    //! \param first iterator pointing at the first value to calculate the
    //! length of.
    //! \param last iterator pointing one beyond the last value to
    //! calculate the length of.
    //!
    //! \returns A value of type `size_t`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    template <typename Iterator>
    size_t length(Iterator first, Iterator last);

    //! \brief Return the sum of the lengths of the rules of \p p.
    //!
    //! Returns the sum of the lengths of the rules of \p p.
    //!
    //! \tparam Word the type of the words in the presentation.
    //! \param p the presentation.
    //!
    //! \returns A value of type `size_t`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    template <typename Word>
    size_t length(Presentation<Word> const& p) {
      return length(p.rules.cbegin(), p.rules.cend());
    }

    //! \brief Reverse every rule.
    //!
    //! Reverse every rule.
    //!
    //! \tparam Word the type of the words in the presentation.
    //! \param p the presentation.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    template <typename Word>
    void reverse(Presentation<Word>& p) {
      for (auto& rule : p.rules) {
        std::reverse(rule.begin(), rule.end());
      }
    }

    //! \brief Reverse every rule.
    //!
    //! Reverse every rule.
    //!
    //! \tparam Word the type of the words in the presentation.
    //! \param p an rvalue reference for a presentation.
    //!
    //! \returns An rvalue reference to the reversed presentation.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    // This is the only place that JDE can find where a helper that modifies its
    // first argument is not void. This is deliberate, since if we weren't to
    // return anything, the first parameter would go out of scope immediately
    // after this call and this function would be pointless .
    template <typename Word>
    Presentation<Word>&& reverse(Presentation<Word>&& p) {
      for (auto& rule : p.rules) {
        std::reverse(rule.begin(), rule.end());
      }
      return std::move(p);
    }

    //! \brief Normalize the alphabet to \f$\{0, \ldots, n - 1\}\f$.
    //!
    //! Modify the presentation in-place so that the alphabet is \f$\{0, \ldots,
    //! n - 1\}\f$ (or equivalent) and rewrites the rules to use this alphabet.
    //!
    //! If the alphabet is already normalized, then no changes are made to the
    //! presentation.
    //!
    //! \tparam Word the type of the words in the presentation.
    //! \param p the presentation.
    //!
    //! \throws LibsemigroupsException if
    //! \ref Presentation::throw_if_bad_alphabet_or_rules throws on the initial
    //! presentation.
    template <typename Word>
    void normalize_alphabet(Presentation<Word>& p);

    //! \brief Change or re-order the alphabet.
    //!
    //! This function replaces `p.alphabet()` with \p new_alphabet, where
    //! possible, and re-writes the rules in the presentation using the new
    //! alphabet.
    //!
    //! \tparam Word the type of the words in the presentation.
    //!
    //! \param p the presentation.
    //! \param new_alphabet the replacement alphabet.
    //!
    //! \throws LibsemigroupsException if the size of `p.alphabet()` and
    //! \p new_alphabet do not agree.
    template <typename Word>
    void change_alphabet(Presentation<Word>& p, Word const& new_alphabet);

    //! \brief Change or re-order the alphabet.
    //!
    //! This function replaces `p.alphabet()` with \p new_alphabet where
    //! possible, and re-writes the rules in the presentation using the new
    //! alphabet.
    //!
    //! \param p the presentation.
    //! \param new_alphabet the replacement alphabet.
    //!
    //! \throws LibsemigroupsException if the size of `p.alphabet()` and
    //! \p new_alphabet do not agree.
    inline void change_alphabet(Presentation<std::string>& p,
                                char const*                new_alphabet) {
      change_alphabet(p, std::string(new_alphabet));
    }

    //! \brief Return an iterator pointing at the left-hand side of the first
    //! rule of maximal length in the given range.
    //!
    //! Returns an iterator pointing at the left-hand side of the first
    //! rule of maximal length in the given range.
    //!
    //! The *length* of a rule is defined to be the sum of the lengths of its
    //! left-hand and right-hand sides.
    //!
    //! \tparam Iterator the type of the parameters.
    //! \param first left-hand side of the first rule.
    //! \param last one past the right-hand side of the last rule.
    //!
    //! \returns A value of type `Iterator`.
    //!
    //! \throws LibsemigroupsException if the distance between \p first and
    //! \p last is odd.
    template <typename Iterator>
    Iterator longest_rule(Iterator first, Iterator last);

    //! \brief Return an iterator pointing at the left-hand side of the first
    //! rule in the presentation with maximal length.
    //!
    //! Returns an iterator pointing at the left-hand side of the first
    //! rule in the presentation with maximal length.
    //!
    //! The *length* of a rule is defined to be the sum of the lengths of its
    //! left-hand and right-hand sides.
    //!
    //! \tparam Word the type of the words in the presentation.
    //! \param p the presentation.
    //!
    //! \returns A value of type `std::vector<Word>::const_iterator`.
    //!
    //! \throws LibsemigroupsException if the length of \p p.rules is odd.
    template <typename Word>
    typename std::vector<Word>::const_iterator
    longest_rule(Presentation<Word> const& p) {
      return longest_rule(p.rules.cbegin(), p.rules.cend());
    }

    //! \brief Return an iterator pointing at the left-hand side of the first
    //! rule of minimal length in the given range.
    //!
    //! Returns an iterator pointing at the left-hand side of the first
    //! rule of minimal length in the given range.
    //!
    //! The *length* of a rule is defined to be the sum of the lengths of its
    //! left-hand and right-hand sides.
    //!
    //! \tparam Iterator the type of the parameters.
    //! \param first left-hand side of the first rule.
    //! \param last one past the right-hand side of the last rule.
    //!
    //! \returns A value of type `Iterator`.
    //!
    //! \throws LibsemigroupsException if the distance between \p first and
    //! \p last is odd.
    template <typename Iterator>
    Iterator shortest_rule(Iterator first, Iterator last);

    //! \brief Return an iterator pointing at the left-hand side of the first
    //! rule in the presentation with minimal length.
    //!
    //! Returns an iterator pointing at the left-hand side of the first
    //! rule in the presentation with minimal length.
    //!
    //! The *length* of a rule is defined to be the sum of the lengths of its
    //! left-hand and right-hand sides.
    //!
    //! \tparam Word the type of the words in the presentation.
    //! \param p the presentation.
    //!
    //! \returns A value of type `std::vector<Word>::const_iterator`.
    //!
    //! \throws LibsemigroupsException if the length of \p p.rules is odd.
    template <typename Word>
    typename std::vector<Word>::const_iterator
    shortest_rule(Presentation<Word> const& p) {
      return shortest_rule(p.rules.cbegin(), p.rules.cend());
    }

    //! \brief Return the maximum length of a rule in the given range.
    //!
    //! Returns the maximum length of a rule in the given range.
    //!
    //! The *length* of a rule is defined to be the sum of the lengths of its
    //! left-hand and right-hand sides.
    //!
    //! \tparam Iterator the type of the parameters.
    //! \param first left-hand side of the first rule.
    //! \param last one past the right-hand side of the last rule.
    //!
    //! \returns A value of type `Iterator::value_type::size_type`.
    //!
    //! \throws LibsemigroupsException if the length of \p p.rules is odd.
    template <typename Iterator>
    typename Iterator::value_type::size_type longest_rule_length(Iterator first,
                                                                 Iterator last);

    //! \brief Return the maximum length of a rule in the presentation.
    //!
    //! Returns the maximum length of a rule in the presentation.
    //!
    //! The *length* of a rule is defined to be the sum of the lengths of its
    //! left-hand and right-hand sides.
    //!
    //! \tparam Word the type of the words in the presentation.
    //! \param p the presentation.
    //!
    //! \returns A value of type `Word::size_type`.
    //!
    //! \throws LibsemigroupsException if the length of \p p.rules is odd.
    template <typename Word>
    typename Word::size_type longest_rule_length(Presentation<Word> const& p) {
      return longest_rule_length(p.rules.cbegin(), p.rules.cend());
    }

    //! \brief Return the minimum length of a rule in the given range.
    //!
    //! Returns the minimum length of a rule in the given range.
    //!
    //! The *length* of a rule is defined to be the sum of the lengths of its
    //! left-hand and right-hand sides.
    //!
    //! \tparam Iterator the type of the parameters.
    //! \param first left-hand side of the first rule.
    //! \param last one past the right-hand side of the last rule.
    //!
    //! \returns A value of type `Iterator::value_type::size_type`.
    //!
    //! \throws LibsemigroupsException if the length of \p p.rules is odd.
    template <typename Iterator>
    typename Iterator::value_type::size_type
    shortest_rule_length(Iterator first, Iterator last);

    //! \brief Return the minimum length of a rule in the presentation.
    //!
    //! Returns the minimum length of a rule in the presentation.
    //!
    //! The *length* of a rule is defined to be the sum of the lengths of its
    //! left-hand and right-hand sides.
    //!
    //! \tparam Word the type of the words in the presentation.
    //! \param p the presentation.
    //!
    //! \returns A value of type `Word::size_type`.
    //!
    //! \throws LibsemigroupsException if the length of \p p.rules is odd.
    template <typename Word>
    typename Word::size_type shortest_rule_length(Presentation<Word> const& p) {
      return shortest_rule_length(p.rules.cbegin(), p.rules.cend());
    }

    //! \brief Remove any trivially redundant generators.
    //!
    //! If one side of any of the rules in the presentation \p p is a letter
    //! \c a and the other side of the rule does not contain \c a, then this
    //! function replaces every occurrence of \c a in every rule by the other
    //! side of the rule. This substitution is performed for every such
    //! rule in the presentation; and the trivial rules (with both sides being
    //! identical) are removed. If both sides of a rule are letters, then the
    //! greater letter is replaced by the lesser one.
    //!
    //! \tparam Word the type of the words in the presentation.
    //! \param p the presentation.
    //!
    //! \throws LibsemigroupsException if `p.rules.size()` is odd.
    template <typename Word>
    void remove_redundant_generators(Presentation<Word>& p);

    //! \brief Return the first letter **not** in the alphabet of a
    //! presentation.
    //!
    //! This function returns `letter(p, i)` when `i` is the least possible
    //! value such that `!p.in_alphabet(letter(p, i))` if such a letter
    //! exists.
    //!
    //! \tparam Word the type of the words in the presentation.
    //!
    //! \param p the presentation.
    //!
    //! \returns A `letter_type`.
    //!
    //! \throws LibsemigroupsException if \p p already has an alphabet of
    //! the maximum possible size supported by `letter_type`.
    template <typename Word>
    typename Presentation<Word>::letter_type
    first_unused_letter(Presentation<Word> const& p);

    //! \brief Convert a monoid presentation to a semigroup presentation.
    //!
    //! This function modifies its argument in-place by replacing the empty
    //! word in all relations by a new generator, and the identity rules for
    //! that new generator. If `p.contains_empty_word()` is `false`, then the
    //! presentation is not modified and \ref UNDEFINED is returned. If a new
    //! generator is added as the identity, then this generator is returned.
    //!
    //! \tparam Word the type of the words in the presentation.
    //!
    //! \param p the presentation.
    //!
    //! \returns The new generator added, if any, and \ref UNDEFINED if not.
    //!
    //! \throws LibsemigroupsException if `replace_word` or
    //!  `add_identity_rules` does.
    template <typename Word>
    typename Presentation<Word>::letter_type
    make_semigroup(Presentation<Word>& p);

    //! \brief Greedily reduce the length of the presentation using
    //! `longest_subword_reducing_length`.
    //!
    //! This function repeatedly calls
    //! `presentation::longest_subword_reducing_length` and
    //! `presentation::replace_subword` to introduce a new generator and reduce
    //! the length of the presentation \p p until
    //! `presentation::longest_subword_reducing_length` returns the empty word.
    //!
    //! \tparam Word the type of the words in the presentation.
    //!
    //! \param p the presentation.
    //!
    //! \throws LibsemigroupsException if
    //! `presentation::longest_subword_reducing_length` or
    //!  `presentation::replace_word` does.
    template <typename Word>
    void greedy_reduce_length(Presentation<Word>& p);

    //! \brief Greedily reduce the length and number of generators of the
    //! presentation.
    //!
    //! This function repeatedly calls
    //! `presentation::longest_subword_reducing_length` and
    //! `presentation::replace_subword` to introduce a new generator to try to
    //! reduce the length of the presentation \p p and the number of generators.
    //! This is done until either
    //! `presentation::longest_subword_reducing_length` returns the empty word,
    //! or the new length and number of generators is greater than or equal to
    //! that of the presentation in the previous iteration.
    //!
    //! In the latter case, the presentation \p p gets restored to the state it
    //! was in after the previous iteration.
    //!
    //! \tparam Word the type of the words in the presentation.
    //!
    //! \param p the presentation.
    //!
    //! \throws LibsemigroupsException if
    //! `presentations::longest_subword_reducing_length` or
    //!  `presentations::replace_word` does.
    template <typename Word>
    void greedy_reduce_length_and_number_of_gens(Presentation<Word>& p);

    //! \brief Return `true` if the \f$1\f$-relation presentation can be
    //! strongly compressed.
    //!
    //! A \f$1\f$-relation presentation is *strongly compressible* if both
    //! relation words start with the same letter and end with the same
    //! letter. In other words, if the alphabet of the presentation \p p is
    //! \f$A\f$ and the relation words are of the form \f$aub = avb\f$ where
    //! \f$a, b\in A\f$ (possibly \f$ a = b\f$) and \f$u, v\in A ^ *\f$, then
    //! \p p is strongly compressible. See section 3.2 of
    //! \cite NybergBrodda2021aa for details.
    //!
    //! \tparam Word the type of the words in the presentation.
    //!
    //! \param p the presentation.
    //!
    //! \returns A value of type `bool`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \sa
    //! * \ref strongly_compress
    // not noexcept because std::vector::operator[] isn't
    template <typename Word>
    bool is_strongly_compressible(Presentation<Word> const& p);

    //! \brief Strongly compress a \f$1\f$-relation presentation.
    //!
    //! Strongly compress a \f$1\f$-relation presentation.
    //!
    //! Returns `true` if the \f$1\f$-relation presentation \p p has been
    //! modified and `false` if not. The word problem is solvable for the
    //! input presentation if it is solvable for the modified version.
    //!
    //! \tparam Word the type of the words in the presentation.
    //!
    //! \param p the presentation.
    //!
    //! \returns A value of type `bool`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \sa
    //! * \ref is_strongly_compressible
    // not noexcept because is_strongly_compressible isn't
    template <typename Word>
    bool strongly_compress(Presentation<Word>& p);

    //! \brief Reduce the number of generators in a \f$1\f$-relation
    //! presentation to `2`.
    //!
    //! Reduce the number of generators in a \f$1\f$-relation presentation to
    //! `2`.
    //!
    //! Returns `true` if the \f$1\f$-relation presentation \p p has been
    //! modified and `false` if not.
    //!
    //! A \f$1\f$-relation presentation is *left cycle-free* if the
    //! relation words start with distinct letters. In other words, if the
    //! alphabet of the presentation \p p is \f$A\f$ and the relation words are
    //! of the form \f$au = bv\f$ where \f$a, b\in A\f$ with \f$a \neq b\f$ and
    //! \f$u, v \in A ^ *\f$, then \p p is left cycle-free. The word problem for
    //! a left cycle-free \f$1\f$-relation monoid is solvable if the word
    //! problem for the modified version obtained from this function is
    //! solvable.
    //!
    //! \tparam Word the type of the words in the presentation.
    //!
    //! \param p the presentation.
    //! \param index determines the choice of letter to use, `0` uses
    //! `p.rules[0].front()` and `1` uses `p.rules[1].front()` (defaults to:
    //! `0`).
    //!
    //! \returns A value of type `bool`.
    //!
    //! \throws LibsemigroupsException if \p index is not `0` or `1`.
    template <typename Word>
    bool reduce_to_2_generators(Presentation<Word>& p, size_t index = 0);

    //! \brief Add rules that define each letter as an idempotent.
    //!
    //! Adds rules to \p p of the form \f$a^2 = a\f$ for every letter \f$a\f$ in
    //! \p letters.
    //!
    //! \tparam Word the type of the words in the presentation.
    //! \param p the presentation to add rules to.
    //! \param letters the letters to make idempotent.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \warning
    //! No checks that the arguments describe words over the alphabet of \p p
    //! are performed.
    template <typename Word>
    void add_idempotent_rules_no_checks(Presentation<Word>& p,
                                        Word const&         letters) {
      for (auto x : letters) {
        add_rule_no_checks(p, {x, x}, {x});
      }
    }

    //! \brief Add rules that define involution.
    //!
    //! Adds rules to \p p of the form \f$a^2 = \varepsilon\f$ for every letter
    //! \f$a\f$ in \p letters.
    //!
    //! \tparam Word the type of the words in the presentation.
    //! \param p the presentation to add rules to.
    //! \param letters the letters to add involution rules for.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \warning
    //! No checks that the arguments describe words over the alphabet of \p p
    //! are performed.
    template <typename Word>
    void add_involution_rules_no_checks(Presentation<Word>& p,
                                        word_type const&    letters) {
      for (auto x : letters) {
        add_rule_no_checks(p, {x, x}, {});
      }
    }

    //! \brief Add rules so specific letters commute.
    //!
    //! Adds rules to \p p of the form \f$uv = vu\f$ for every letter \f$u\f$ in
    //! \p letters1 and \f$v\f$ in \p letters2.
    //!
    //! \tparam Word the type of the words in the presentation.
    //! \param p the presentation to add rules to.
    //! \param letters1 the first collection of letters to add rules for.
    //! \param letters2 the second collection of letters to add rules for.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \warning
    //! No checks that the arguments describe words over the alphabet of \p p
    //! are performed.
    template <typename Word>
    void add_commutes_rules_no_checks(Presentation<Word>& p,
                                      Word const&         letters1,
                                      Word const&         letters2);

    //! \brief Add rules so specific letters commute.
    //!
    //! Adds rules to \p p of the form \f$uv = vu\f$ for every pair of letters
    //! \f$u, v\f$ in \p letters.
    //!
    //! \tparam Word the type of the words in the presentation.
    //! \param p the presentation to add rules to.
    //! \param letters the collection of letters to add rules for.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \warning
    //! No checks that the arguments describe words over the alphabet of \p p
    //! are performed.
    template <typename Word>
    void add_commutes_rules_no_checks(Presentation<Word>& p,
                                      Word const&         letters) {
      add_commutes_rules_no_checks(p, letters, letters);
    }

    //! \brief Add rules so specific letters commute with specific words.
    //!
    //! Adds rules to \p p of the form \f$uv = vu\f$ for every letter \f$u\f$ in
    //! \p letters and \f$v\f$ in \p words.
    //!
    //! \tparam Word the type of the words in the presentation.
    //! \param p the presentation to add rules to.
    //! \param letters the collection of letters to add rules for.
    //! \param words the collection of words to add rules for.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \warning
    //! No checks that the arguments describe words over the alphabet of \p p
    //! are performed.
    template <typename Word>
    void add_commutes_rules_no_checks(Presentation<Word>&         p,
                                      Word const&                 letters,
                                      std::initializer_list<Word> words);

    //! \brief Try to detect group inverses.
    //!
    //! This function tries to deduce group theoretic inverses defined by the
    //! rules of the presentation \p p as following: the rules of the
    //! presentation where one side has length 2 and the other has length 0 are
    //! detected. For any such rule we remember that the first letter is a
    //! possible inverse of the second. If rules of the form `ab=1` and `ba=1`
    //! are detected, then \c a has inverse \c b and vice versa. If there are
    //! multiple different such rules and we deduce conflicting values for the
    //! inverse of a letter, then an exception is thrown.
    //!
    //! Those letters where an inverse is detected are pushed into the back of
    //! the parameter \p letters, and the detected inverse is pushed into
    //! \p inverses. The parameters \p letters and \p inverses are modified
    //! in-place, and are not cleared before adding letters or their inverses.
    //!
    //! \tparam Word the type of the words in the presentation.
    //! \param p the presentation.
    //! \param letters the word to contain the letters with inverses.
    //! \param inverses the word to contain the inverses found.
    //!
    //! \throws LibsemigroupsException if
    //! \ref Presentation::throw_if_bad_alphabet_or_rules throws.
    //! \throws LibsemigroupsException if conflicting inverses for any letter
    //! are detected.
    template <typename Word>
    void try_detect_inverses(Presentation<Word>& p,
                             Word&               letters,
                             Word&               inverses);

    //! \brief Try to detect group inverses.
    //!
    //! This function constructs two \c Word objects to store the letters and
    //! inverses, performs
    //! \ref try_detect_inverses(Presentation<Word>&, Word&, Word&)
    //! and then returns the result \c pair as a std::pair where:
    //!
    //! * `pair.first` is the list of letters such that an inverse was
    //! detected;
    //! * `pair.second` is the list of inverses of the letters in `pair.first`
    //! (where the letter in position \c i is the inverse of `pair.first[i]`,
    //! and vice versa).
    //!
    //!
    //! \tparam Word the type of the words in the presentation.
    //! \param p the presentation.
    //!
    //! \throws LibsemigroupsException if
    //! \ref Presentation::throw_if_bad_alphabet_or_rules throws.
    //! \throws LibsemigroupsException if conflicting inverses for any letter
    //! are detected.
    template <typename Word>
    std::pair<Word, Word> try_detect_inverses(Presentation<Word>& p);

    //! \brief Balance the length of the left-hand and right-hand sides.
    //!
    //! This function first sorts the sides of each rules so that the larger
    //! side of the rule is on the left. Then for each rule, while the last
    //! letter of the left-hand side is in \p letters, the last letter of the
    //! left-hand side is removed and the corresponding value in \p inverses is
    //! appended to the end of the right-hand side. Next, while the first
    //! letter of the left-hand side is in \p letters, the first letter of the
    //! left-hand side is removed and the corresponding value in \p inverses is
    //! appended to the front of the right-hand side.
    //!
    //! \tparam Word1 the type of the words in the presentation.
    //! \tparam Word2 the type of the words \p letters and \p inverses.
    //! \param p the presentation.
    //! \param letters the letters that can be replaced in the left-hand side.
    //! \param inverses the inverses of the letters.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \warning
    //! This function assumes that the semigroup defined by \p p is isomorphic
    //! to a group, and that \p inverses are valid. However, this function does
    //! no checks on its arguments. If the previous assumptions do not hold,
    //! there is no guarantee the the semigroup \f$S\f$ defined by \p p before
    //! this function is called will be isomorphic to the semigroup \f$S'\f$
    //! defined by \p p after this function is called.
    template <typename Word1, typename Word2>
    void balance_no_checks(Presentation<Word1>& p,
                           Word2 const&         letters,
                           Word2 const&         inverses);

    //! \brief Balance the length of the left-hand and right-hand sides.
    //!
    //! This is an overload for
    //! \ref balance_no_checks(Presentation<Word1>&, Word2 const&, Word2 const&)
    //! to allow, for example, std::initializer_list to be used for the
    //! parameters \p letters and \p inverses.
    // Note that this doesn't work when Word = std::string and so we also
    // require an overload specifically taking initializer_list's too.
    template <typename Word>
    void balance_no_checks(Presentation<Word>& p,
                           Word const&         letters,
                           Word const&         inverses) {
      balance_no_checks<Word, Word>(p, letters, inverses);
    }

    //! \brief Balance the length of the left-hand and right-hand sides.
    //!
    //! This function just calls
    //! \ref balance_no_checks(Presentation<Word1>&, Word2 const&, Word2 const&)
    //! where the 2nd parameter is defined to be `p.alphabet()`.
    //!
    //! \tparam Word the type of the words in the presentation.
    //! \param p the presentation.
    //! \param inverses the inverses of the letters.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \warning
    //! This function assumes that the semigroup defined by \p p is isomorphic
    //! to a group, and that \p inverses are valid. However, this function does
    //! no checks on its arguments. If the previous assumptions do not hold,
    //! there is no guarantee the the semigroup \f$S\f$ defined by \p p before
    //! this function is called will be isomorphic to the semigroup \f$S'\f$
    //! defined by \p p after this function is called.
    template <typename Word>
    void balance_no_checks(Presentation<Word>& p, Word const& inverses) {
      balance_no_checks(p, p.alphabet(), inverses);
    }

    //! \brief Balance the length of the left-hand and right-hand sides.
    //!
    //! This is an overload for
    //! \ref balance_no_checks(Presentation<Word1>&, Word2 const&, Word2 const&)
    //! to allow std::string_view to be used for the parameters \p letters and
    //! \p inverses.
    //!
    // clang-format off
    // NOLINTNEXTLINE(whitespace/line_length)
    //! \deprecated_alias_warning{balance_no_checks(Presentation<Word>&, Word const&, Word const&)}
    // clang-format on
    static inline void balance_no_checks
        [[deprecated]] (Presentation<std::string>& p,
                        std::string_view           letters,
                        std::string_view           inverses) {
      balance_no_checks<std::string, std::string_view>(p, letters, inverses);
    }

    //! \brief Balance the length of the left-hand and right-hand sides.
    //!
    //! This is an overload for
    //! \ref balance_no_checks(Presentation<Word1>&, Word2 const&, Word2 const&)
    //! to allow string literals to be used for the parameters \p letters and
    //! \p inverses.
    //!
    // clang-format off
    // NOLINTNEXTLINE(whitespace/line_length)
    //! \deprecated_alias_warning{balance_no_checks(Presentation<Word>&, Word const&, Word const&)}
    // clang-format on
    static inline void balance_no_checks
        [[deprecated]] (Presentation<std::string>& p,
                        char const*                letters,
                        char const*                inverses) {
      balance_no_checks(p, std::string(letters), std::string(inverses));
    }

    //! \brief Balance the length of the left-hand and right-hand sides.
    //!
    //! See
    //! \ref balance_no_checks(Presentation<Word1>&, Word2 const&, Word2 const&)
    //! for details about this function.
    //!
    //! \tparam Word1 the type of the words in the presentation.
    //! \tparam Word2 the type of the words \p letters and \p inverses.
    //! \param p the presentation.
    //! \param letters the letters that can be replaced in the left-hand side.
    //! \param inverses the inverses of the letters.
    //!
    //! \throws LibsemigroupsException if
    //! \ref Presentation::throw_if_bad_alphabet_or_rules throws.
    //! \throws LibsemigroupsException if \ref throw_if_bad_inverses throws when
    //! called with \p letters and \p inverses. This does not check that the
    //! values in \p inverses are actually inverses for the values in
    //! \p letters, and balances the relations as described in
    //! \ref balance_no_checks(Presentation<Word1>&, Word2 const&, Word2 const&)
    //! assuming that this is the case.
    template <typename Word1, typename Word2>
    void balance(Presentation<Word1>& p,
                 Word2 const&         letters,
                 Word2 const&         inverses) {
      p.throw_if_bad_alphabet_or_rules();
      throw_if_bad_inverses(p, letters, inverses);

      balance_no_checks(p, letters, inverses);
    }

    //! \brief Balance the length of the left-hand and right-hand sides.
    //!
    //! This function just calls
    //! \ref balance_no_checks(Presentation<Word1>&, Word2 const&, Word2 const&)
    //! where the 2nd parameter is defined to be `p.alphabet()`.
    //!
    //! \tparam Word the type of the words in the presentation.
    //! \param p the presentation.
    //! \param inverses the inverses of the letters.
    //!
    //! \throws LibsemigroupsException if
    //! \ref Presentation::throw_if_bad_alphabet_or_rules throws.
    //! \throws LibsemigroupsException if \ref throw_if_bad_inverses throws
    //! when called with `p.alphabet()` and \p inverses. This function does not
    //! check that the values in \p inverses are actually inverses for the
    //! values in `p.alphabet()`, and balances the relations as described in
    //! \ref balance_no_checks(Presentation<Word1>&, Word2 const&, Word2 const&)
    //! assuming that this is the case.
    template <typename Word>
    void balance(Presentation<Word>& p, Word const& inverses) {
      throw_if_bad_inverses(p, p.alphabet(), inverses);
      balance_no_checks(p, p.alphabet(), inverses);
    }

    //! \brief Balance the length of the left-hand and right-hand sides.
    //!
    //! This is an overload for
    //! \ref balance(Presentation<Word1>&, Word2 const&, Word2 const&)
    //! to allow, for example, std::initializer_list to be used for the
    //! parameters \p letters and \p inverses.
    // Note that this doesn't work when Word = std::string and so we also
    // require an overload specifically taking initializer_list's too.
    template <typename Word>
    void balance(Presentation<Word>& p,
                 Word const&         letters,
                 Word const&         inverses) {
      balance<Word, Word>(p, letters, inverses);
    }

    //! \brief Detect inverses and balance the length of the left-hand and
    //! right-hand sides.
    //!
    //! This function calls
    //! \ref balance_no_checks(Presentation<Word1>&, Word2 const&, Word2 const&)
    //! where the 2nd and 3rd arguments are deduced from the rules in the
    //! using \ref try_detect_inverses.
    //!
    //! \tparam Word the type of the words in the presentation.
    //! \param p the presentation.
    //!
    //! \throws LibsemigroupsException if
    //! \ref Presentation::throw_if_bad_alphabet_or_rules throws.
    //! \throws LibsemigroupsException if conflicting inverses for any letter
    //! are detected.
    // There's no no_check version of this function because we need to try and
    // detect the inverse and if we cannot we have to throw an exception.
    template <typename Word>
    void balance(Presentation<Word>& p);

    //! \brief Add all cyclic permutations of a word as relators in a
    //! presentation.
    //!
    //! This function adds one rule with left-hand side \c w and right-hand side
    //! the empty word to the presentation \p p, for every cyclic
    //! permutation \c w of \p relator.
    //!
    //! \tparam Word1 the type of the words in the presentation.
    //! \tparam Word2 the type of the word \p relator.
    //! \param p the presentation.
    //! \param relator the word.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \note If multiple cyclic permutations of \p relator are equal, then
    //! there will be duplicate rules added to the presentation \p p. You can
    //! remove these by calling \ref remove_duplicate_rules.
    //!
    //! \warning
    //! This function performs no checks that the letters in \p relator belong
    //! to the alphabet of \p p or that \p p contains the empty word.
    template <typename Word1, typename Word2>
    void add_cyclic_conjugates_no_checks(Presentation<Word1>& p,
                                         Word2 const&         relator);

    //! \brief Add all cyclic permutations of a word as relators in a
    //! presentation.
    //!
    //! This is an overload for
    //! \ref add_cyclic_conjugates_no_checks(Presentation<Word1>&, Word2 const&)
    //! to allow, for example, std::initializer_list to be used for the
    //! parameters \p relator.
    template <typename Word>
    void add_cyclic_conjugates_no_checks(Presentation<Word>& p,
                                         Word const&         relator) {
      add_cyclic_conjugates_no_checks<Word, Word>(p, relator);
    }

    //! \brief Add all cyclic permutations of a word as relators in a
    //! presentation.
    //!
    //! This is an overload for
    //! \ref add_cyclic_conjugates_no_checks(Presentation<Word1>&, Word2 const&)
    //! to allow, string literals to be used for the parameters \p relator.
    inline void add_cyclic_conjugates_no_checks(Presentation<std::string>& p,
                                                char const* relator) {
      add_cyclic_conjugates_no_checks<std::string, std::string_view>(
          p, std::string_view(relator));
    }

    //! \brief Add all cyclic permutations of a word as relators in a
    //! presentation.
    //!
    //! This function adds one rule with left-hand side \c w and right-hand side
    //! the empty word to the presentation \p p, for every cyclic
    //! permutation \c w of \p relator.
    //!
    //! \tparam Word1 the type of the words in the presentation.
    //! \tparam Word2 the type of the word \p relator.
    //! \param p the presentation.
    //! \param relator the word.
    //!
    //! \throws LibsemigroupsException if \p relator contains any letters not
    //! belonging to `p.alphabet()`.
    //!
    //! \throws LibsemigroupsException if \p p does not contain the empty word.
    //!
    //! \note If multiple cyclic permutations of \p relator are equal, then
    //! there will be duplicate rules added to the presentation \p p. You can
    //! remove these by calling \ref remove_duplicate_rules.
    template <typename Word1, typename Word2>
    void add_cyclic_conjugates(Presentation<Word1>& p, Word2 const& relator);

    //! \brief Add all cyclic permutations of a word as relators in a
    //! presentation.
    //!
    //! This is an overload for
    //! \ref add_cyclic_conjugates(Presentation<Word1>&, Word2 const&)
    //! to allow, for example, std::initializer_list to be used for the
    //! parameters \p relator.
    template <typename Word>
    void add_cyclic_conjugates(Presentation<Word>& p, Word const& relator) {
      add_cyclic_conjugates<Word, Word>(p, relator);
    }

    //! \brief Add all cyclic permutations of a word as relators in a
    //! presentation.
    //!
    //! This is an overload for
    //! \ref add_cyclic_conjugates(Presentation<Word1>&, Word2 const&)
    //! to allow string literals to be used for the parameters \p relator.
    inline void add_cyclic_conjugates(Presentation<std::string>& p,
                                      char const*                relator) {
      add_cyclic_conjugates<std::string, std::string_view>(
          p, std::string_view(relator));
    }

    //! \brief Return the code that would create \p p in GAP.
    //!
    //! This function returns the string of GAP code that could be used to
    //! create an object with the same alphabet and rules as \p p in GAP.
    //! Presentations in GAP are created by taking quotients of free semigroups
    //! or monoids.
    //!
    //! \param p the presentation.
    //! \param var_name the name of the variable to be used in GAP.
    //!
    //! \throws LibsemigroupsException if \p has more than 49 generators.
    std::string to_gap_string(Presentation<word_type> const& p,
                              std::string const&             var_name);

    //! \brief Return the code that would create \p p in GAP.
    //!
    //! This function returns the string of GAP code that could be used to
    //! create an object with the same alphabet and rules as \p p in GAP.
    //! Presentations in GAP are created by taking quotients of free semigroups
    //! or monoids.
    //!
    //! \param p the presentation.
    //! \param var_name the name of the variable to be used in GAP.
    //!
    //! \throws LibsemigroupsException if \p has more than 49 generators.
    std::string to_gap_string(Presentation<std::string> const& p,
                              std::string const&               var_name);

    //! \brief Find a rule.
    //!
    //! This function returns an iterator `it` pointing at the first
    //! occurrence of \p lhs in an even index position of `p.rules` such
    //! that `it + 1` points at \p rhs.
    //!
    //! \param p the presentation.
    //! \param lhs the left-hand side of the rule.
    //! \param rhs the right-hand side of the rule.
    //!
    //! \returns An iterator.
    //!
    //! \warning
    //! This function does no checks on its argument. In particular, it does not
    //! check that \p p is properly defined, namely that the length of
    //! `p.rules` is even. If the rule we are trying to find is not in
    //! `p.rules`, then bad things might happen.
    template <typename Word>
    [[nodiscard]] typename std::vector<Word>::iterator
    find_rule_no_checks(Presentation<Word>& p,
                        Word const&         lhs,
                        Word const&         rhs);

    //! \brief Find a rule.
    //!
    //! This function returns an iterator `it` pointing at the first
    //! occurrence of \p lhs in an even index position of `p.rules` such
    //! that `it + 1` points at \p rhs.
    //!
    //! \param p the presentation.
    //! \param lhs the left-hand side of the rule.
    //! \param rhs the right-hand side of the rule.
    //!
    //! \returns An iterator.
    //!
    //! \throws LibsemigroupsException if
    //! `p.throw_if_bad_alphabet_or_rules()` throws.
    template <typename Word>
    [[nodiscard]] typename std::vector<Word>::iterator
    find_rule(Presentation<Word>& p, Word const& lhs, Word const& rhs) {
      p.throw_if_bad_alphabet_or_rules();
      return find_rule_no_checks(p, lhs, rhs);
    }

    //! \brief Find a rule.
    //!
    //! This function returns an iterator `it` pointing at the first
    //! occurrence of \p lhs in an even index position of `p.rules` such
    //! that `it + 1` points at \p rhs.
    //!
    //! \param p the presentation.
    //! \param lhs the left-hand side of the rule.
    //! \param rhs the right-hand side of the rule.
    //!
    //! \returns An iterator.
    //!
    //! \warning
    //! This function does no checks on its argument. In particular, it does not
    //! check that \p p is properly defined, namely that the length of
    //! `p.rules` is even. If the rule we are trying to find is not in
    //! `p.rules`, then bad things might happen.
    template <typename Word>
    [[nodiscard]] typename std::vector<Word>::const_iterator
    find_rule_no_checks(Presentation<Word> const& p,
                        Word const&               lhs,
                        Word const&               rhs) {
      return find_rule_no_checks(const_cast<Presentation<Word>&>(p), lhs, rhs);
    }

    //! \brief Find a rule.
    //!
    //! This function returns an iterator `it` pointing at the first
    //! occurrence of \p lhs in an even index position of `p.rules` such
    //! that `it + 1` points at \p rhs.
    //!
    //! \param p the presentation.
    //! \param lhs the left-hand side of the rule.
    //! \param rhs the right-hand side of the rule.
    //!
    //! \returns An iterator.
    //!
    //! \throws LibsemigroupsException if
    //! `p.throw_if_bad_alphabet_or_rules()` throws.
    template <typename Word>
    [[nodiscard]] typename std::vector<Word>::const_iterator
    find_rule(Presentation<Word> const& p, Word const& lhs, Word const& rhs) {
      return find_rule(const_cast<Presentation<Word>&>(p), lhs, rhs);
    }

    //! \brief Returns the index of a rule or \ref UNDEFINED.
    //!
    //! This function returns the minimum index \c i of \p lhs such that
    //! `p.rules[i + 1]` equals \p rhs; or \ref UNDEFINED if there is not
    //! such rule
    //!
    //! \param p the presentation.
    //! \param lhs the left-hand side of the rule.
    //! \param rhs the right-hand side of the rule.
    //!
    //! \returns The index of the rule or \ref UNDEFINED.
    //!
    //! \warning
    //! This function does no checks on its argument. In particular, it does not
    //! check that \p p is properly defined, namely that the length of
    //! `p.rules` is even. If the rule we are trying to find is not in
    //! `p.rules`, then bad things might happen.
    template <typename Word>
    [[nodiscard]] size_t index_rule_no_checks(Presentation<Word> const& p,
                                              Word const&               lhs,
                                              Word const&               rhs);

    //! \brief Returns the index of a rule or \ref UNDEFINED.
    //!
    //! This function returns the minimum index \c i of \p lhs such that
    //! `p.rules[i + 1]` equals \p rhs; or \ref UNDEFINED if there is not
    //! such rule
    //!
    //! \param p the presentation.
    //! \param lhs the left-hand side of the rule.
    //! \param rhs the right-hand side of the rule.
    //!
    //! \returns The index of the rule or \ref UNDEFINED.
    //!
    //! \throws LibsemigroupsException if
    //! `p.throw_if_bad_alphabet_or_rules()` throws.
    template <typename Word>
    [[nodiscard]] size_t index_rule(Presentation<Word> const& p,
                                    Word const&               lhs,
                                    Word const&               rhs) {
      p.throw_if_bad_alphabet_or_rules();
      return index_rule_no_checks(p, lhs, rhs);
    }

    //! \brief Check whether a rule belongs to a presentation.
    //!
    //! This function returns \c true if \p lhs and \p rhs form a rule in \p p.
    //! That is if \p lhs occurs in an even index position in `p.rules` and
    //! \p rhs is the next item in `p.rules`.
    //!
    //! \param p the presentation.
    //! \param lhs the left-hand side of the rule.
    //! \param rhs the right-hand side of the rule.
    //!
    //! \returns Whether or not \p lhs and \p rhs form a rule in \p p.
    //!
    //! \warning
    //! This function does no checks on its argument. In particular, it does not
    //! check that \p p is properly defined, namely that the length of
    //! `p.rules` is even. If the rule we are trying to find is not in
    //! `p.rules`, then bad things might happen.
    template <typename Word>
    [[nodiscard]] bool is_rule_no_checks(Presentation<Word> const& p,
                                         Word const&               lhs,
                                         Word const&               rhs) {
      return find_rule_no_checks(p, lhs, rhs) != p.rules.end();
    }

    //! \brief Check whether a rule belongs to a presentation.
    //!
    //! This function returns \c true if \p lhs and \p rhs form a rule in \p p.
    //! That is if \p lhs occurs in an even index position in `p.rules` and
    //! \p rhs is the next item in `p.rules`.
    //!
    //! \param p the presentation.
    //! \param lhs the left-hand side of the rule.
    //! \param rhs the right-hand side of the rule.
    //!
    //! \returns Whether or not \p lhs and \p rhs form a rule in \p p.
    //!
    //! \throws LibsemigroupsException if
    //! `p.throw_if_bad_alphabet_or_rules()` throws.
    template <typename Word>
    [[nodiscard]] bool is_rule(Presentation<Word> const& p,
                               Word const&               lhs,
                               Word const&               rhs) {
      return find_rule(p, lhs, rhs) != p.rules.end();
    }

  }  // namespace presentation

  //! \ingroup presentations_group
  //!
  //! \brief For an implementation of inverse presentations for semigroups or
  //! monoids.
  //!
  //! Defined in `presentation.hpp`.
  //!
  //! This class template can be used to construction inverse presentations for
  //! semigroups or monoids and is intended to be used as the input to other
  //! algorithms in `libsemigroups`. This class inherits from
  //! \ref Presentation<Word>.
  //!
  //! \tparam Word the type of the underlying words.
  template <typename Word>
  class InversePresentation : public Presentation<Word> {
   public:
    //! \brief Type of the words in the rules of an InversePresentation
    //! object.
    using word_type = typename Presentation<Word>::word_type;

    //! \brief Type of the letters in the words that constitute the rules of
    //! an InversePresentation object.
    using letter_type = typename Presentation<Word>::letter_type;

    //! \brief Type of a const iterator to either side of a rule.
    using const_iterator = typename Presentation<Word>::const_iterator;

    //! \brief Type of an iterator to either side of a rule.
    using iterator = typename Presentation<Word>::iterator;

    //! \brief Size type for rules.
    using size_type = typename Presentation<Word>::size_type;

   private:
    word_type _inverses;

   public:
    using Presentation<Word>::Presentation;

    // TODO(later) init functions

    //! \brief Construct an InversePresentation from a Presentation reference.
    //!
    //! Construct an InversePresentation, initially with empty inverses, from a
    //! reference to a Presentation.
    //!
    //! \tparam Word the type of the words in the presentation.
    //! \param p a reference to the Presentation to construct from.
    explicit InversePresentation(Presentation<Word> const& p)
        : Presentation<Word>(p), _inverses() {}

    //! \brief Construct an InversePresentation from a Presentation rvalue
    //! reference.
    //!
    //! Construct an InversePresentation, initially with empty inverses, from a
    //! Presentation rvalue reference
    //!
    //! \tparam Word the type of the words in the presentation.
    //! \param p an rvalue reference to the Presentation to construct from.
    explicit InversePresentation(Presentation<Word>&& p)
        : Presentation<Word>(p), _inverses() {}

    //! \brief Set the inverse of each letter in the alphabet.
    //!
    //! Set the inverse of each letter in the alphabet.
    //!
    //! \param w a word containing the inverses.
    //!
    //! \returns A reference to `this`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \warning
    //! This function does no checks on its argument. In particular, it does not
    //! check that the letters in \p w belong to the alphabet, nor does it add
    //! new letters to the alphabet.
    InversePresentation& inverses_no_checks(word_type const& w);

    //! \brief Set the inverse of each letter in the alphabet.
    //!
    //! Set the inverse of each letter in the alphabet.
    //!
    //! \param w a word containing the inverses.
    //!
    //! \returns A reference to `this`.
    //!
    //! \throw LibsemigroupsException if:
    //! * the alphabet contains duplicate letters
    //! * the inverses do not act as semigroup inverses
    //!
    //! \note
    //! Whilst the alphabet is not specified as an argument to this function, it
    //! is necessary to throw_if_bad_alphabet_or_rules the alphabet here; a
    //! specification of inverses cannot make sense if the alphabet contains
    //! duplicate letters.
    //!
    //! \sa
    //! * \ref Presentation<Word>::throw_if_alphabet_has_duplicates
    //! * \ref presentation::throw_if_bad_inverses
    InversePresentation& inverses(word_type const& w) {
      Presentation<Word>::throw_if_alphabet_has_duplicates();
      presentation::throw_if_bad_inverses(*this, w);
      return inverses_no_checks(w);
    }

    //! \brief Return the inverse of each letter in the alphabet.
    //!
    //! Returns the inverse of each letter in the alphabet.
    //!
    //! \returns A value of type \ref word_type.
    //!
    //! \exceptions
    //! \noexcept
    word_type const& inverses() const noexcept {
      return _inverses;
    }

    //! \brief Return the inverse of a letter in the alphabet.
    //!
    //! Returns the inverse of the letter \p x.
    //!
    //! \param x the letter whose index is sought.
    //!
    //! \returns A value of type \ref letter_type.
    //!
    //! \throws LibsemigroupsException if no inverses have been set, or if
    //! `index(letter_type x) const` throws.
    letter_type inverse(letter_type x) const;

    //! \brief Check if the InversePresentation is valid.
    //!
    //! Check if the InversePresentation is valid. Specifically, check that the
    //! alphabet does not contain duplicate letters, that all rules only contain
    //! letters defined in the alphabet, and that the inverses act as semigroup
    //! inverses.
    //!
    //! \throw LibsemigroupsException if:
    //! * the alphabet contains duplicate letters
    //! * the rules contain letters not defined in the alphabet
    //! * the inverses do not act as semigroup inverses
    //!
    //! \sa
    //! * \ref Presentation<Word>::throw_if_bad_alphabet_or_rules
    //! * \ref presentation::throw_if_bad_inverses
    void throw_if_bad_alphabet_rules_or_inverses() const {
      // TODO(0) check if this is used appropriately after rename
      Presentation<Word>::throw_if_bad_alphabet_or_rules();
      presentation::throw_if_bad_inverses(*this, inverses());
    }
  };

  //! \ingroup presentations_group
  //!
  //! \brief Compare for equality.
  //!
  //! Returns \c true if \p lhop equals \p rhop by comparing the
  //! the alphabets and the rules.
  //!
  //! \tparam Word the type of the words in the presentations.
  //! \param lhop a presentation that is to be compared.
  //! \param rhop a presentation that is to be compared.
  //!
  //! \returns A value of type \c bool.
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  //!
  //! \complexity
  //! At worst linear in the sum of the alphabet sizes and numbers of rules.
  // TODO(later) also we could do a more sophisticated version of this
  template <typename Word>
  bool operator==(Presentation<Word> const& lhop,
                  Presentation<Word> const& rhop) {
    return lhop.alphabet() == rhop.alphabet() && lhop.rules == rhop.rules;
  }

  //! \ingroup presentations_group
  //!
  //! \brief Compare for equality.
  //!
  //! Returns \c true if \p lhop equals \p rhop by comparing the
  //! the alphabets and the rules.
  //!
  //! \tparam Word the type of the words in the inverse presentations.
  //! \param lhop an inverse presentation that is to be compared.
  //! \param rhop an inverse presentation that is to be compared.
  //!
  //! \returns A value of type \c bool.
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  //!
  //! \complexity
  //! At worst linear in the sum of the alphabet sizes and numbers of rules.
  // TODO(later) also we could do a more sophisticated version of this
  template <typename Word>
  bool operator==(InversePresentation<Word> const& lhop,
                  InversePresentation<Word> const& rhop) {
    return lhop.alphabet() == rhop.alphabet() && lhop.rules == rhop.rules
           && lhop.inverses() == rhop.inverses();
  }

  //! \ingroup presentations_group
  //!
  //! \brief Compare for inequality.
  //!
  //! Returns \c true if \p lhop does not equal \p rhop by comparing the
  //! the alphabets and the rules.
  //!
  //! \tparam Word the type of the words in the presentations.
  //! \param lhop a presentation that is to be compared.
  //! \param rhop a presentation that is to be compared.
  //!
  //! \returns A value of type \c bool.
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  //!
  //! \complexity
  //! At worst linear in the sum of the alphabet sizes and numbers of rules.
  template <typename Word>
  bool operator!=(Presentation<Word> const& lhop,
                  Presentation<Word> const& rhop) {
    return !(lhop == rhop);
  }

  //! \ingroup presentations_group
  //!
  //! \brief Compare for inequality.
  //!
  //! Returns \c true if \p lhop does not equal \p rhop by comparing the
  //! the alphabets and the rules.
  //!
  //! \tparam Word the type of the words in the inverse presentations.
  //! \param lhop an inverse presentation that is to be compared.
  //! \param rhop an inverse presentation that is to be compared.
  //!
  //! \returns A value of type \c bool.
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  //!
  //! \complexity
  //! At worst linear in the sum of the alphabet sizes and numbers of rules.
  template <typename Word>
  bool operator!=(InversePresentation<Word> const& lhop,
                  InversePresentation<Word> const& rhop) {
    return !(lhop == rhop);
  }

  //! \ingroup presentations_group
  //!
  //! \brief Return a human readable representation of a presentation.
  //!
  //! Return a human readable representation of a presentation.
  //!
  //! \tparam Word the type of the words in the presentation.
  //! \param p the presentation.
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  template <typename Word>
  std::string to_human_readable_repr(Presentation<Word> const& p);

  //! \ingroup presentations_group
  //!
  //! \brief Return a human readable representation of an InversePresentation.
  //!
  //! Return a human readable representation of an InversePresentation.
  //!
  //! \tparam Word the type of the words in the InversePresentation.
  //! \param p the InversePresentation.
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  template <typename Word>
  std::string to_human_readable_repr(InversePresentation<Word> const& p);

  namespace v4 {
    ////////////////////////////////////////////////////////////////////////
    // Presentation + function -> Presentation
    ////////////////////////////////////////////////////////////////////////

    template <typename Result, typename Word, typename Func>
    auto to(Presentation<Word> const& p, Func&& f) -> std::enable_if_t<
        std::is_same_v<Presentation<typename Result::word_type>, Result>,
        Result>;

    ////////////////////////////////////////////////////////////////////////
    // InversePresentation + function -> InversePresentation
    ////////////////////////////////////////////////////////////////////////

    template <typename Result, typename Word, typename Func>
    auto to(InversePresentation<Word> const& ip, Func&& f) -> std::enable_if_t<
        std::is_same_v<InversePresentation<typename Result::word_type>, Result>,
        Result>;

    ////////////////////////////////////////////////////////////////////////
    // Presentation -> Presentation
    ////////////////////////////////////////////////////////////////////////

    template <typename Result, typename Word>
    auto to(Presentation<Word> const& p) -> std::enable_if_t<
        std::is_same_v<Presentation<typename Result::word_type>, Result>
            && !std::is_same_v<typename Result::word_type, Word>,
        Result>;

    // This function is documented above because Doxygen conflates these two
    // functions.
    template <typename Result, typename Word>
    auto to(Presentation<Word> const& p) noexcept
        -> std::enable_if_t<std::is_same_v<Presentation<Word>, Result>,
                            Result const&> {
      return p;
    }

    ////////////////////////////////////////////////////////////////////////
    // InversePresentation -> InversePresentation
    ////////////////////////////////////////////////////////////////////////

    template <typename Result, typename Word>
    auto to(InversePresentation<Word> const& ip) noexcept -> std::enable_if_t<
        std::is_same_v<InversePresentation<typename Result::word_type>, Result>
            && !std::is_same_v<Word, typename Result::word_type>,
        Result> {
      using WordOutput = typename Result::word_type;
      return v4::to<InversePresentation<WordOutput>>(ip, [&ip](auto val) {
        return words::human_readable_letter<WordOutput>(ip.index(val));
      });
    }

    // This function is documented above because Doxygen conflates these two
    // functions.
    template <typename Result, typename Word>
    auto to(InversePresentation<Word> const& ip) noexcept
        -> std::enable_if_t<std::is_same_v<InversePresentation<Word>, Result>,
                            Result const&> {
      return ip;
    }

    ////////////////////////////////////////////////////////////////////////
    // Presentation -> InversePresentation
    ////////////////////////////////////////////////////////////////////////

    template <template <typename...> typename Thing, typename Word>
    auto to(Presentation<Word> const& p) -> std::enable_if_t<
        std::is_same_v<InversePresentation<Word>, Thing<Word>>,
        InversePresentation<Word>>;
  }  // namespace v4

  namespace detail {

    class GreedyReduceHelper {
     private:
      size_t              _best;
      int                 _best_goodness;
      std::vector<size_t> _distance_from_root;
      std::vector<size_t> _num_leafs;
      std::vector<size_t> _scratch;
      std::vector<size_t> _suffix_index;

     public:
      using const_iterator = typename Ukkonen::const_iterator;

      explicit GreedyReduceHelper(Ukkonen const& u);

      GreedyReduceHelper()                                     = delete;
      GreedyReduceHelper(GreedyReduceHelper const&)            = delete;
      GreedyReduceHelper(GreedyReduceHelper&&)                 = delete;
      GreedyReduceHelper& operator=(GreedyReduceHelper const&) = delete;
      GreedyReduceHelper& operator=(GreedyReduceHelper&&)      = delete;
      ~GreedyReduceHelper();

      void pre_order(Ukkonen const& u, size_t v);
      void post_order(Ukkonen const& u, size_t v);
      std::pair<const_iterator, const_iterator> yield(Ukkonen const& u);
    };
  }  // namespace detail

  //! \ingroup presentations_group
  //!
  //! \brief Helper variable template.
  //!
  //! Helper variable template.
  //!
  //! The value of this variable is \c true if the template parameter \p Thing
  //! is \ref InversePresentation.
  //!
  //! \tparam Thing a type.
  // clang-format off
  // NOLINTNEXTLINE(whitespace/line_length)
  //! \deprecated_alias_warning{is_specialization_of_v<Thing, InversePresentation>}
  // clang-format on
  template <typename Thing>
  static constexpr bool IsInversePresentation [[deprecated]]
  = is_specialization_of_v<Thing, InversePresentation>;

  //! \ingroup presentations_group
  //!
  //! \brief Helper variable template.
  //!
  //! Helper variable template.
  //!
  //! The value of this variable is \c true if the template parameter \p Thing
  //! is \ref InversePresentation.
  //!
  //! \tparam Thing a type.
  //!
  //! \deprecated_alias_warning{is_specialization_of_v<Thing, Presentation>}
  template <typename Thing>
  static constexpr bool IsPresentation [[deprecated]]
  = is_specialization_of_v<Thing, Presentation>;

}  // namespace libsemigroups

#include "presentation.tpp"

#endif  // LIBSEMIGROUPS_PRESENTATION_HPP_
