//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2022 James D. Mitchell
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

#ifndef LIBSEMIGROUPS_PRESENT_HPP_
#define LIBSEMIGROUPS_PRESENT_HPP_

#include <algorithm>         // for reverse, sort
#include <cmath>             // for std::pow
#include <cstddef>           // for size_t
#include <cstring>           // for strlen
#include <initializer_list>  // for initializer_list
#include <iterator>          // for distance
#include <numeric>           // for accumulate
#include <string>            // for string
#include <type_traits>       // for enable_if_t, is_same
#include <unordered_map>     // for unordered_map
#include <unordered_set>     // for unordered_set
#include <utility>           // for move
#include <vector>            // for vector

#include "constants.hpp"  // for UNDEFINED
#include "debug.hpp"      // for LIBSEMIGROUPS_ASSERT
#include "int-range.hpp"  // for IntegralRange
#include "order.hpp"      // for shortlex_compare
#include "uf.hpp"         // for Duf
#include "ukkonen.hpp"    // for Ukkonen

namespace libsemigroups {
  //! No doc
  struct PresentationBase {};

  //! Defined in ``present.hpp``.
  //!
  //! This class template can be used to construction presentations for
  //! semigroups or monoids and is intended to be used as the input to other
  //! algorithms in `libsemigroups`. The idea is to provide a shallow wrapper
  //! around a vector of words of type `W`. We refer to this vector of words as
  //! the rules of the presentation. The Presentation class template also
  //! provide some checks that the rules really defines a presentation, (i.e.
  //! it's consistent with its alphabet), and some related functionality is
  //! available in the namespace `libsemigroups::presentation`.
  //!
  //! \tparam W the type of the underlying words.
  template <typename W>
  class Presentation : public PresentationBase {
   public:
    //! The type of the words in the rules of a Presentation object.
    using word_type = W;

    //! The type of the letters in the words that constitute the rules of a
    //! Presentation object.
    using letter_type = typename word_type::value_type;

    //! Type of a const iterator to either side of a rule
    using const_iterator = typename std::vector<word_type>::const_iterator;

    //! Type of an iterator to either side of a rule.
    using iterator = typename std::vector<word_type>::iterator;

    //! Size type for rules.
    using size_type = typename std::vector<word_type>::size_type;

   private:
    word_type                                  _alphabet;
    std::unordered_map<letter_type, size_type> _alphabet_map;
    bool                                       _contains_empty_word;

   public:
    //! Data member holding the rules of the presentation.
    //!
    //! The rules can be altered using the member functions of `std::vector`,
    //! and the presentation can be checked for validity using \ref validate.
    std::vector<word_type> rules;

    //! Default constructor.
    //!
    //! Constructs an empty presentation with no rules and no alphabet.
    Presentation();

    //! Default copy constructor.
    Presentation(Presentation const&) = default;

    //! Default move constructor.
    Presentation(Presentation&&) = default;

    //! Default copy assignment operator.
    Presentation& operator=(Presentation const&) = default;

    //! Default move assignment operator.
    Presentation& operator=(Presentation&&) = default;

    //! Returns the alphabet of the presentation.
    //!
    //! \returns A const reference to Presentation::word_type
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \param (None)
    word_type const& alphabet() const noexcept {
      return _alphabet;
    }

    //! Set the alphabet by size.
    //!
    //! Sets the alphabet to the range \f$[0, n)\f$ consisting of values of
    //! type \ref letter_type
    //!
    //! \param n the size of the alphabet
    //!
    //! \returns A const reference to \c *this
    //!
    //! \throws LibsemigroupsException if the value of \p n is greater than the
    //! maximum number of letters supported by \ref letter_type.
    //!
    //! \warning
    //! No checks are performed by this function, in particular, it is not
    //! verified that the rules in the presentation (if any) consist of letters
    //! belonging to the alphabet
    //!
    //! \sa validate_alphabet, validate_rules, and \ref validate
    Presentation& alphabet(size_type n);

    //! Set the alphabet const reference.
    //!
    //! Sets the alphabet to be the letters in \p lphbt.
    //!
    //! \param lphbt the alphabet
    //!
    //! \returns A const reference to \c *this
    //!
    //! \throws LibsemigroupsException if there are duplicate letters in \p
    //! lphbt
    //!
    //! \warning
    //! This function does not verify that the rules in the presentation (if
    //! any) consist of letters belonging to the alphabet
    //!
    //! \sa validate_rules and \ref validate
    Presentation& alphabet(word_type const& lphbt);

    //! Set the alphabet from rvalue reference.
    //!
    //! Sets the alphabet to be the letters in \p lphbt.
    //!
    //! \param lphbt the alphabet
    //!
    //! \returns A const reference to \c *this
    //!
    //! \throws LibsemigroupsException if there are duplicate letters in \p
    //! lphbt
    //!
    //! \warning
    //! This function does not verify that the rules in the presentation (if
    //! any) consist of letters belonging to the alphabet
    //!
    //! \sa validate_rules and \ref validate
    Presentation& alphabet(word_type&& lphbt);

    //! Set the alphabet to be the letters in the rules.
    //!
    //! Sets the alphabet to be the letters in \ref rules.
    //!
    //! \returns A const reference to \c *this
    //!
    //! \sa validate_rules, and \ref validate
    //!
    //! \param (None)
    Presentation& alphabet_from_rules();

    //! Get a letter in the alphabet by index
    //!
    //! Returns the letter of the alphabet in position \p i
    //!
    //! \param i the index
    //! \returns A value of type \ref letter_type
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \warning
    //! This function performs no bound checks on the argument \p i
    letter_type letter(size_type i) const {
      LIBSEMIGROUPS_ASSERT(i < _alphabet.size());
      return _alphabet[i];
    }

    //! Get the index of a letter in the alphabet
    //!
    //! \param val the letter
    //!
    //! \returns A value of type \ref size_type
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    // \throws LibsemigroupsException if \p val does not belong to the
    // alphabet
    //!
    //! \complexity
    //! Constant.
    //!
    //! \warning This function does not verify that its argument belongs to the
    //! alphabet.
    size_type index(letter_type val) const {
      return _alphabet_map.find(val)->second;
    }

    //! Check if a letter belongs to the alphabet or not.
    //!
    //! \no_libsemigroups_except
    //!
    //! \param val the letter to check
    //!
    //! \returns
    //! A value of type `bool`.
    bool in_alphabet(letter_type val) const {
      return _alphabet_map.find(val) != _alphabet_map.cend();
    }

    //! Add a rule to the presentation
    //!
    //! Adds the rule with left hand side `[lhs_begin, lhs_end)` and
    //! right hand side `[rhs_begin, rhs_end)` to the rules.
    //!
    //! \tparam S the type of the first two parameters (iterators, or pointers)
    //! \tparam T the type of the second two parameters (iterators, or pointers)
    //!
    //! \param lhs_begin an iterator pointing to the first letter of the left
    //! hand side of the rule to be added
    //! \param lhs_end an iterator pointing one past the last letter of the left
    //! hand side of the rule to be added
    //! \param rhs_begin an iterator pointing to the first letter of the right
    //! hand side of the rule to be added
    //! \param rhs_end an iterator pointing one past the last letter of the
    //! right hand side of the rule to be added
    //!
    //! \returns A const reference to \c *this
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \warning
    //! It is not checked that the arguments describe words over the alphabet
    //! of the presentation.
    //!
    //! \sa
    //! add_rule_and_check
    template <typename S, typename T>
    Presentation& add_rule(S lhs_begin, S lhs_end, T rhs_begin, T rhs_end) {
      rules.emplace_back(lhs_begin, lhs_end);
      rules.emplace_back(rhs_begin, rhs_end);
      return *this;
    }

    //! Add a rule to the presentation and check it is valid
    //!
    //! Adds the rule with left hand side `[lhs_begin, lhs_end)` and right
    //! hand side `[rhs_begin, rhs_end)` to the rules and checks that they only
    //! contain letters in \ref alphabet. It is possible to add rules directly
    //! via the data member \ref rules, this function just exists to encourage
    //! adding rules with both sides defined at the same time.
    //!
    //! \tparam S the type of the first two parameters (iterators, or pointers)
    //! \tparam T the type of the second two parameters (iterators, or pointers)
    //!
    //! \param lhs_begin an iterator pointing to the first letter of the left
    //! hand side of the rule to be added
    //! \param lhs_end an iterator pointing one past the last letter of the left
    //! hand side of the rule to be added
    //! \param rhs_begin an iterator pointing to the first letter of the right
    //! hand side of the rule to be added
    //! \param rhs_end an iterator pointing one past the last letter of the
    //! right hand side of the rule to be added
    //!
    //! \returns A const reference to \c *this
    //!
    //! \throws LibsemigroupsException if any letter does not below to the
    //! alphabet
    //! \throws LibsemigroupsException if \ref contains_empty_word returns \c
    //! false and \p lhs_begin equals \p lhs_end or \p rhs_begin equals \p
    //! rhs_end
    //!
    //! \sa
    //! add_rule
    template <typename S, typename T>
    Presentation&
    add_rule_and_check(S lhs_begin, S lhs_end, T rhs_begin, T rhs_end) {
      validate_word(lhs_begin, lhs_end);
      validate_word(rhs_begin, rhs_end);
      return add_rule(lhs_begin, lhs_end, rhs_begin, rhs_end);
    }

    //! Check if the presentation should contain the empty word
    //!
    //! \param (None)
    //!
    //! \returns A value of type `bool`
    //!
    //! \exceptions
    //! \noexcept
    bool contains_empty_word() const noexcept {
      return _contains_empty_word;
    }

    //! Specify that the presentation should (not) contain the empty word
    //!
    //! \param val whether the presentation should contain the empty word
    //!
    //! \returns A const reference to \c *this
    //!
    //! \exceptions
    //! \noexcept
    Presentation& contains_empty_word(bool val) noexcept {
      _contains_empty_word = val;
      return *this;
    }

    //! Check if the alphabet is valid
    //!
    //! \throws LibsemigroupsException if there are duplicate letters in the
    //! alphabet
    //!
    //! \param (None)
    //!
    //! \returns
    //! (None)
    void validate_alphabet() const {
      decltype(_alphabet_map) alphabet_map;
      validate_alphabet(alphabet_map);
    }

    //! Check if a letter belongs to the alphabet or not.
    //!
    //! \throws LibsemigroupsException if \p c does not belong to the alphabet
    //!
    //! \param c the letter to check
    //!
    //! \returns
    //! (None)
    void validate_letter(letter_type c) const;

    //! Check if every letter in a range belongs to the alphabet.
    //!
    //! \throws LibsemigroupsException if there is a letter not in the alphabet
    //! between \p first and \p last
    //!
    //! \tparam T the type of the arguments (iterators)
    //! \param first iterator pointing at the first letter to check
    //! \param last iterator pointing one beyond the last letter to check
    //!
    //! \returns
    //! (None)
    template <typename T>
    void validate_word(T first, T last) const;

    //! Check if every rule consists of letters belonging to the alphabet.
    //!
    //! \throws LibsemigroupsException if any word contains a letter not in the
    //! alphabet.
    //!
    //! \param (None)
    //!
    //! \returns
    //! (None)
    void validate_rules() const;

    //! Check if the alphabet and rules are valid.
    //!
    //! \throws LibsemigroupsException if \ref validate_alphabet or \ref
    //! validate_rules does.
    //!
    //! \param (None)
    //!
    //! \returns
    //! (None)
    void validate() const {
      validate_alphabet();
      validate_rules();
    }

    //! Remove the alphabet and all rules.
    //!
    //! This function clears the alphabet and all rules from the presentation,
    //! putting it back into the state it would be in if it was newly
    //! constructed.
    //!
    //! \param (None)
    //!
    //! \returns
    //! (None)
    void clear();

   private:
    void try_set_alphabet(decltype(_alphabet_map)& alphabet_map,
                          word_type&               old_alphabet);
    void validate_alphabet(decltype(_alphabet_map)& alphabet_map) const;
  };

  namespace presentation {

    //! Add a rule to the presentation by reference.
    //!
    //! Adds the rule with left hand side `lhop` and right hand side `rhop`
    //! to the rules.
    //!
    //! \tparam W the type of the words in the presentation
    //! \param p the presentation
    //! \param lhop the left hand side of the rule
    //! \param rhop the left hand side of the rule
    //!
    //! \returns (None)
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \warning
    //! No checks that the arguments describe words over the alphabet of the
    //! presentation are performed.
    template <typename W>
    void add_rule(Presentation<W>& p, W const& lhop, W const& rhop) {
      p.add_rule(lhop.begin(), lhop.end(), rhop.begin(), rhop.end());
    }

    //! Add a rule to the presentation by reference and check.
    //!
    //! Adds the rule with left hand side \p lhop and right hand side \p rhop
    //! to the rules, after checking that \p lhop and \p rhop consist entirely
    //! of letters in the alphabet of \p p.
    //!
    //! \tparam W the type of the words in the presentation
    //! \param p the presentation
    //! \param lhop the left hand side of the rule
    //! \param rhop the left hand side of the rule
    //!
    //! \returns (None)
    //!
    //! \throws LibsemigroupsException if \p lhop or \p rhop contains any
    //! letters not belonging to `p.alphabet()`.
    template <typename W>
    void add_rule_and_check(Presentation<W>& p, W const& lhop, W const& rhop) {
      p.add_rule_and_check(lhop.begin(), lhop.end(), rhop.begin(), rhop.end());
    }

    //! Add a rule to the presentation by `char const*`.
    //!
    //! Adds the rule with left hand side `lhop` and right hand side `rhop`
    //! to the rules.
    //!
    //! \tparam W the type of the words in the presentation
    //! \param p the presentation
    //! \param lhop the left hand side of the rule
    //! \param rhop the left hand side of the rule
    //!
    //! \returns (None)
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \warning
    //! No checks that the arguments describe words over the alphabet of the
    //! presentation are performed.
    inline void add_rule(Presentation<std::string>& p,
                         char const*                lhop,
                         char const*                rhop) {
      add_rule(p, std::string(lhop), std::string(rhop));
    }

    //! Add a rule to the presentation by `char const*`.
    //!
    //! Adds the rule with left hand side `lhop` and right hand side `rhop`
    //! to the rules.
    //!
    //! \tparam W the type of the words in the presentation
    //! \param p the presentation
    //! \param lhop the left hand side of the rule
    //! \param rhop the left hand side of the rule
    //!
    //! \returns (None)
    //!
    //! \throws LibsemigroupsException if \p lhop or \p rhop contains any
    //! letters not belonging to `p.alphabet()`.
    template <typename W>
    void add_rule_and_check(Presentation<W>& p,
                            char const*      lhop,
                            char const*      rhop) {
      add_rule_and_check(p, std::string(lhop), std::string(rhop));
    }

    //! Add a rule to the presentation by `initializer_list`.
    //!
    //! Adds the rule with left hand side `lhop` and right hand side `rhop`
    //! to the rules.
    //!
    //! \tparam W the type of the words in the presentation
    //! \param p the presentation
    //! \param lhop the left hand side of the rule
    //! \param rhop the left hand side of the rule
    //!
    //! \returns (None)
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \warning
    //! No checks that the arguments describe words over the alphabet of the
    //! presentation are performed.
    template <typename W, typename T>
    void add_rule(Presentation<W>&         p,
                  std::initializer_list<T> lhop,
                  std::initializer_list<T> rhop) {
      p.add_rule(lhop.begin(), lhop.end(), rhop.begin(), rhop.end());
    }

    //! Add a rule to the presentation by `initializer_list`.
    //!
    //! Adds the rule with left hand side `lhop` and right hand side `rhop`
    //! to the rules.
    //!
    //! \tparam W the type of the words in the presentation
    //! \param p the presentation
    //! \param lhop the left hand side of the rule
    //! \param rhop the left hand side of the rule
    //!
    //! \returns (None)
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \throws LibsemigroupsException if \p lhop or \p rhop contains any
    //! letters not belonging to `p.alphabet()`.
    template <typename W, typename T>
    void add_rule_and_check(Presentation<W>&         p,
                            std::initializer_list<T> lhop,
                            std::initializer_list<T> rhop) {
      p.add_rule_and_check(lhop.begin(), lhop.end(), rhop.begin(), rhop.end());
    }

    template <typename W, typename T>
    void add_rules(Presentation<W>& p, T first, T last) {
      for (auto it = first; it != last; it += 2) {
        add_rule(p, *it, *(it + 1));
      }
    }
    //! Add a rule to the presentation from another presentation.
    //!
    //! Adds all the rules of the second argument to the first argument
    //! which is modified in-place.
    //!
    //! \tparam W the type of the words in the presentation
    //! \param p the presentation to add rules to
    //! \param q the presentation with the rules to add
    //!
    //! \returns (None)
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    template <typename W>
    void add_rules(Presentation<W>& p, Presentation<W> const& q) {
      add_rules(p, q.rules.cbegin(), q.rules.cend());
    }

    //! Add rules for an identity element.
    //!
    //! Adds rules of the form \f$ae = ea = a\f$ for every letter \f$a\f$ in
    //! the alphabet of \p p, and where \f$e\f$ is the second parameter.
    //!
    //! \tparam W the type of the words in the presentation
    //! \param p the presentation to add rules to
    //! \param e the identity element
    //!
    //! \returns (None)
    //!
    //! \throws LibsemigroupsException if \p e is not a letter in
    //! `p.alphabet()`.
    template <typename W>
    void add_identity_rules(Presentation<W>&                      p,
                            typename Presentation<W>::letter_type e);

    //! Add rules for a zero element.
    //!
    //! Adds rules of the form \f$az = za = z\f$ for every letter \f$a\f$ in
    //! the alphabet of \p p, and where \f$z\f$ is the second parameter.
    //!
    //! \tparam W the type of the words in the presentation
    //! \param p the presentation to add rules to
    //! \param z the zero element
    //!
    //! \returns (None)
    //!
    //! \throws LibsemigroupsException if \p z is not a letter in
    //! `p.alphabet()`.
    template <typename W>
    void add_zero_rules(Presentation<W>&                      p,
                        typename Presentation<W>::letter_type z);

    //! Add rules for inverses.
    //!
    //! The letter in \c a with index \c i in \p vals is the inverse of the
    //! letter in `alphabet()` with index \c i. The rules added are \f$a_ib_i =
    //! e\f$ where the alphabet is \f$\{a_1, \ldots, a_n\}\f$; the 2nd parameter
    //! \p vals is \f$\{b_1, \ldots, b_n\}\f$; and \f$e\f$ is the 3rd parameter.
    //!
    //! \tparam W the type of the words in the presentation
    //! \param p the presentation to add rules to
    //! \param vals the inverses
    //! \param e the identity element (defaults to \ref UNDEFINED, meaning use
    //! the empty word)
    //!
    //! \returns (None)
    //!
    //! \throws LibsemigroupsException if any of the following apply:
    //! * the length of \p vals is not equal to `alphabet().size()`;
    //! * the letters in \p vals are not exactly those in `alphabet()` (perhaps
    //! in a different order);
    //! * \f$(a_i ^ {-1}) ^ {-1} = a_i\f$ does not hold for some \f$i\f$;
    //! * \f$e ^ {-1} = e\f$ does not hold
    //!
    //! \complexity
    //! \f$O(n)\f$ where \f$n\f$ is `alphabet().size()`.
    template <typename W>
    void add_inverse_rules(Presentation<W>&                      p,
                           W const&                              vals,
                           typename Presentation<W>::letter_type e = UNDEFINED);

    //! Add rules for inverses.
    //!
    //! The letter in \c a with index \c i in \p vals is the inverse of the
    //! letter in `alphabet()` with index \c i. The rules added are \f$a_ib_i =
    //! e\f$ where the alphabet is \f$\{a_1, \ldots, a_n\}\f$; the 2nd
    //! parameter \p vals is \f$\{b_1, \ldots, b_n\}\f$; and \f$e\f$ is the 3rd
    //! parameter.
    //!
    //! \tparam W the type of the words in the presentation
    //! \param p the presentation to add rules to
    //! \param vals the inverses
    //! \param e the identity element (defaults to \ref UNDEFINED meaning use
    //! the empty word)
    //!
    //! \returns (None)
    //!
    //! \throws LibsemigroupsException if any of the following apply:
    //! * the length of \p vals is not equal to `alphabet().size()`;
    //! * the letters in \p vals are not exactly those in `alphabet()` (perhaps
    //! in a different order);
    //! * \f$(a_i ^ {-1}) ^ {-1} = a_i\f$ does not hold for some \f$i\f$;
    //! * \f$e ^ {-1} = e\f$ does not hold
    //!
    //! \complexity
    //! \f$O(n)\f$ where \f$n\f$ is alphabet().size().
    inline void add_inverse_rules(Presentation<std::string>& p,
                                  char const*                vals,
                                  char                       e = UNDEFINED) {
      add_inverse_rules(p, std::string(vals), e);
    }

    //! Remove duplicate rules.
    //!
    //! Removes all but one instance of any duplicate rules (if any). Note that
    //! rules of the form \f$u = v\f$ and \f$v = u\f$ (if any) are considered
    //! duplicates. Also note that the rules may be reordered by this function
    //! even if there are no duplicate rules.
    //!
    //! \tparam W the type of the words in the presentation
    //! \param p the presentation
    //!
    //! \returns (None)
    //!
    //! \throws LibsemigroupsException if `p.rules.size()` is odd.
    template <typename W>
    void remove_duplicate_rules(Presentation<W>& p);

    //! Remove rules consisting of identical words.
    //!
    //! Removes all instance of rules (if any) where the left hand side and the
    //! right hand side are identical.
    //!
    //! \tparam W the type of the words in the presentation
    //! \param p the presentation
    //!
    //! \returns (None)
    //!
    //! \throws LibsemigroupsException if `p.rules.size()` is odd.
    template <typename W>
    void remove_trivial_rules(Presentation<W>& p);

    //! If there are rules \f$u = v\f$ and \f$v = w\f$ where \f$|w| < |v|\f$,
    //! then replace \f$u = v\f$ by \f$u = w\f$.
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
    //! \tparam W the type of the words in the presentation
    //! \param p the presentation to add rules to
    //!
    //! \returns (None)
    //!
    //! \throws LibsemigroupsException if `p.rules.size()` is odd.
    template <typename W>
    void reduce_complements(Presentation<W>& p);

    //! Sort each rule \f$u = v\f$ so that the left hand side is shortlex
    //! greater than the right hand side.
    //!
    //! \tparam W the type of the words in the presentation
    //! \param p the presentation to add rules to
    //!
    //! \returns (None)
    //!
    //! \throws LibsemigroupsException if `p.rules.size()` is odd.
    template <typename W>
    void sort_each_rule(Presentation<W>& p);

    //! Sort the rules \f$u_1 = v_1, \ldots, u_n = v_n\f$ so that
    //! \f$u_1v_1 < \cdots < u_nv_n\f$ where \f$<\f$ is the shortlex order.
    //!
    //! \tparam W the type of the words in the presentation
    //! \param p the presentation to sort
    //!
    //! \returns (None)
    //!
    //! \throws LibsemigroupsException if `p.rules.size()` is odd.
    template <typename W>
    void sort_rules(Presentation<W>& p);

    //! Check if the rules \f$u_1 = v_1, \ldots, u_n = v_n\f$ satisfy \f$u_1v_1
    //! < \cdots < u_nv_n\f$ where \f$<\f$ is the shortlex order.
    //!
    //! \tparam W the type of the words in the presentation
    //! \param p the presentation
    //!
    //! \returns A value of type `bool`.
    //!
    //! \throws LibsemigroupsException if `p.rules.size()` is odd.
    template <typename W>
    bool are_rules_sorted(Presentation<W> const& p);

    //! Returns the longest common subword of the rules.
    //!
    //! If it is possible to find a subword \f$w\f$ of the rules \f$u_1 = v_1,
    //! \ldots, u_n = v_n\f$ such that the introduction of a new generator
    //! \f$z\f$ and the relation \f$z = w\f$ reduces the `presentation::length`
    //! of the presentation, then this function returns the word \f$w\f$. If no
    //! such word can be found, then a word of length \f$0\f$ is returned.
    //!
    //! \tparam W the type of the words in the presentation
    //! \param p the presentation
    //!
    //! \returns A value of type \p W.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    // TODO(later) complexity
    template <typename W>
    W longest_common_subword(Presentation<W>& p);

    //! Replace non-overlapping instances of a subword via iterators.
    //!
    //! If \f$w=\f$`[first, last)` is a word, then this function replaces every
    //! non-overlapping instance of \f$w\f$ in every rule, adds a new generator
    //! \f$z\f$, and the rule \f$w = z\f$. The new generator and rule are added
    //! even if \f$w\f$ is not a subword of any rule.
    //!
    //! \tparam W the type of the words in the presentation
    //! \tparam T the type of the 2nd and 3rd parameters (iterators)
    //! \param p the presentation
    //! \param first the start of the subword to replace
    //! \param last one beyond the end of the subword to replace
    //!
    //! \returns (None)
    //!
    //! \throws LibsemigroupsException if `first == last`.
    // TODO(later) complexity
    template <typename W,
              typename T,
              typename = std::enable_if_t<!std::is_same<T, W>::value>>
    void replace_subword(Presentation<W>& p, T first, T last);

    //! Replace non-overlapping instances of a subword via const reference.
    //!
    //! If \f$w=\f$`[first, last)` is a word, then this function replaces every
    //! non-overlapping instance (from left to right) of \f$w\f$ in every rule,
    //! adds a new generator \f$z\f$, and the rule \f$w = z\f$. The new
    //! generator and rule are added even if \f$w\f$ is not a subword of any
    //! rule.
    //!
    //! \tparam W the type of the words in the presentation
    //!
    //! \param p the presentation
    //! \param w the subword to replace
    //!
    //! \returns (None)
    //!
    //! \throws LibsemigroupsException if \p w is empty.
    // TODO(later) complexity
    template <typename W>
    void replace_subword(Presentation<W>& p, W const& w) {
      replace_subword(p, w.cbegin(), w.cend());
    }

    //! Replace non-overlapping instances of a subword via `char const*`.
    //!
    //! If \f$w=\f$`[first, last)` is a word, then replaces every
    //! non-overlapping instance (from left to right) of \f$w\f$ in every rule,
    //! adds a new generator \f$z\f$, and the rule \f$w = z\f$. The new
    //! generator and rule are added even if \f$w\f$ is not a subword of any
    //! rule.
    //!
    //! \param p the presentation
    //! \param w the subword to replace
    //!
    //! \returns (None)
    //!
    //! \throws LibsemigroupsException if \p w is empty.
    inline void replace_subword(Presentation<std::string>& p, char const* w) {
      replace_subword(p, w, w + std::strlen(w));
    }

    //! Replace non-overlapping instances of a subword by another word.
    //!
    //! If \p existing and \p replacement are words, then this function
    //! replaces every non-overlapping instance of \p existing in every rule by
    //! \p replacement. The presentation \p p is changed in-place.
    //!
    //! \tparam W the type of the words in the presentation
    //! \param p the presentation
    //! \param existing the word to be replaced
    //! \param replacement the replacement word.
    //!
    //! \returns (None)
    //!
    //! \throws LibsemigroupsException if `existing` is empty.
    // TODO(later) complexity
    template <typename W>
    void replace_subword(Presentation<W>& p,
                         W const&         existing,
                         W const&         replacement);

    //! Replace non-overlapping instances of a subword by another word.
    //!
    //! Adds the rule with left hand side `[lhs_begin, lhs_end)` and
    //! right hand side `[rhs_begin, rhs_end)` to the rules.
    //!
    //! \tparam S the type of the first two parameters (iterators, or pointers)
    //! \tparam T the type of the second two parameters (iterators, or pointers)
    //!
    //! \param p the presentation
    //! \param first_existing an iterator pointing to the first letter of the
    //! existing subword to be replaced
    //! \param last_existing an iterator pointing one past the last letter of
    //! the existing subword to be replaced
    //! \param first_replacement an iterator pointing to the first letter of
    //! the replacement word
    //! \param last_replacement an iterator pointing one past the last letter
    //! of the replacement word
    //!
    //! \returns (None)
    //!
    //! \throws LibsemigroupsException if `first_existing == last_existing`.
    template <typename W, typename S, typename T>
    void replace_subword(Presentation<W>& p,
                         S                first_existing,
                         S                last_existing,
                         T                first_replacement,
                         T                last_replacement);

    //! Replace instances of a word on either side of a rule by another word.
    //!
    //! If \p existing and \p replacement are words, then this function
    //! replaces every instance of \p existing in every rule of the form
    //! \p existing \f$= w\f$ or \f$w = \f$ \p existing, with the word
    //! \p replacement. The presentation \p p is changed in-place.
    //!
    //! \tparam W the type of the words in the presentation
    //!
    //! \param p the presentation
    //! \param existing the word to be replaced
    //! \param replacement the replacement word
    //!
    //! \returns (None)
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    template <typename W>
    void replace_word(Presentation<W>& p,
                      W const&         existing,
                      W const&         replacement);

    //! Return the sum of the lengths of the rules.
    //!
    //! \tparam W the type of the words in the presentation
    //! \param p the presentation
    //!
    //! \returns A value of type `size_t`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    template <typename W>
    size_t length(Presentation<W> const& p) {
      auto op = [](size_t val, W const& x) { return val + x.size(); };
      return std::accumulate(p.rules.cbegin(), p.rules.cend(), size_t(0), op);
    }

    //! Reverse every rule.
    //!
    //! \tparam W the type of the words in the presentation
    //! \param p the presentation
    //!
    //! \returns (None)
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    template <typename W>
    void reverse(Presentation<W>& p) {
      for (auto& rule : p.rules) {
        std::reverse(rule.begin(), rule.end());
      }
    }

    //! Modify the presentation so that the alphabet is \f$\{0, \ldots, n -
    //! 1\}\f$ (or equivalent) and rewrites the rules to use this alphabet.
    //!
    //! If the alphabet is already normalized, then no changes are made to the
    //! presentation.
    //!
    //! \tparam W the type of the words in the presentation
    //! \param p the presentation
    //!
    //! \returns (None)
    //!
    //! \throws LibsemigroupsException if \ref validate throws on the initial
    //! presentation.
    template <typename W>
    void normalize_alphabet(Presentation<W>& p);

    //! Change or re-order the alphabet.
    //!
    //! This function replaces `p.alphabet()` with \p new_alphabet, where
    //! possible, and re-writes the rules in the presentation using the new
    //! alphabet.
    //!
    //! \tparam W the type of the words in the presentation
    //!
    //! \param p the presentation
    //! \param new_alphabet the replacement alphabet
    //!
    //! \returns (None).
    //!
    //! \throws LibsemigroupsException if the size of `p.alphabet()` and \p
    //! new_alphabet do not agree.
    template <typename W>
    void change_alphabet(Presentation<W>& p, W const& new_alphabet);

    //! Change or re-order the alphabet.
    //!
    //! This function replaces `p.alphabet()` with \p new_alphabet where
    //! possible.
    //!
    //! \param p the presentation
    //! \param new_alphabet the replacement alphabet
    //!
    //! \returns (None).
    //!
    //! \throws LibsemigroupsException if the size of `p.alphabet()` and \p
    //! new_alphabet do not agree.
    inline void change_alphabet(Presentation<std::string>& p,
                                char const*                new_alphabet) {
      change_alphabet(p, std::string(new_alphabet));
    }

    //! Returns an iterator pointing at the left hand side of the first
    //! rule of maximal length in the given range.
    //!
    //! The *length* of a rule is defined to be the sum of the lengths of its
    //! left and right hand sides.
    //!
    //! \tparam T the type of the parameters
    //! \param first left hand side of the first rule
    //! \param last one past the right hand side of the last rule
    //!
    //! \returns A value of type `T`.
    //!
    //! \throws LibsemigroupsException if the distance between \p first and \p
    //! last is odd.
    template <typename T>
    T longest_rule(T first, T last);

    //! Returns an iterator pointing at the left hand side of the first
    //! rule in the presentation with maximal length.
    //!
    //! The *length* of a rule is defined to be the sum of the lengths of its
    //! left and right hand sides.
    //!
    //! \tparam W the type of the words in the presentation
    //! \param p the presentation
    //!
    //! \returns A value of type `std::vector<W>::const_iterator`.
    //!
    //! \throws LibsemigroupsException if the length of \p p.rules is odd.
    template <typename W>
    auto longest_rule(Presentation<W> const& p) {
      return longest_rule(p.rules.cbegin(), p.rules.cend());
    }

    //! Returns an iterator pointing at the left hand side of the first
    //! rule of minimal length in the given range.
    //!
    //! The *length* of a rule is defined to be the sum of the lengths of its
    //! left and right hand sides.
    //!
    //! \tparam T the type of the parameters
    //! \param first left hand side of the first rule
    //! \param last one past the right hand side of the last rule
    //!
    //! \returns A value of type `T`.
    //!
    //! \throws LibsemigroupsException if the distance between \p first and \p
    //! last is odd.
    template <typename T>
    T shortest_rule(T first, T last);

    //! Returns an iterator pointing at the left hand side of the first
    //! rule in the presentation with minimal length.
    //!
    //! The *length* of a rule is defined to be the sum of the lengths of its
    //! left and right hand sides.
    //!
    //! \tparam W the type of the words in the presentation
    //! \param p the presentation
    //!
    //! \returns A value of type `std::vector<W>::const_iterator`.
    //!
    //! \throws LibsemigroupsException if the length of \p p.rules is odd.
    template <typename W>
    auto shortest_rule(Presentation<W> const& p) {
      return shortest_rule(p.rules.cbegin(), p.rules.cend());
    }

    //! Returns the maximum length of a rule in the given range.
    //!
    //! The *length* of a rule is defined to be the sum of the lengths of its
    //! left and right hand sides.
    //!
    //! \tparam T the type of the parameters
    //! \param first left hand side of the first rule
    //! \param last one past the right hand side of the last rule
    //!
    //! \returns A value of type `decltype(first)::value_type::size_type`.
    //!
    //! \throws LibsemigroupsException if the length of \p p.rules is odd.
    template <typename T>
    auto longest_rule_length(T first, T last);

    //! Returns the maximum length of a rule in the presentation.
    //!
    //! The *length* of a rule is defined to be the sum of the lengths of its
    //! left and right hand sides.
    //!
    //! \tparam W the type of the words in the presentation
    //! \param p the presentation
    //!
    //! \returns A value of type `W::size_type`.
    //!
    //! \throws LibsemigroupsException if the length of \p p.rules is odd.
    template <typename W>
    auto longest_rule_length(Presentation<W> const& p) {
      return longest_rule_length(p.rules.cbegin(), p.rules.cend());
    }

    //! Returns the minimum length of a rule in the given range.
    //!
    //! The *length* of a rule is defined to be the sum of the lengths of its
    //! left and right hand sides.
    //!
    //! \tparam T the type of the parameters
    //! \param first left hand side of the first rule
    //! \param last one past the right hand side of the last rule
    //!
    //! \returns A value of type `decltype(first)::value_type::size_type`.
    //!
    //! \throws LibsemigroupsException if the length of \p p.rules is odd.
    template <typename T>
    auto shortest_rule_length(T first, T last);

    //! Returns the minimum length of a rule in the presentation.
    //!
    //! The *length* of a rule is defined to be the sum of the lengths of its
    //! left and right hand sides.
    //!
    //! \tparam W the type of the words in the presentation
    //! \param p the presentation
    //!
    //! \returns A value of type `W::size_type`.
    //!
    //! \throws LibsemigroupsException if the length of \p p.rules is odd.
    template <typename W>
    auto shortest_rule_length(Presentation<W> const& p) {
      return shortest_rule_length(p.rules.cbegin(), p.rules.cend());
    }

    //! Remove any trivially redundant generators.
    //!
    //! If one side of any of the rules in the presentation \p p is a letter \c
    //! a and the other side of the rule does not contain \c a, then this
    //! function replaces every occurrence of \c a in every rule by the other
    //! side of the rule. This substitution is performed for every such
    //! rule in the presentation; and the trivial rules (with both sides being
    //! identical) are removed. If both sides of a rule are letters, then the
    //! greater letter is replaced by the lesser one.
    //!
    //! \tparam W the type of the words in the presentation
    //!
    //! \returns (None)
    //!
    //! \throws LibsemigroupsException if `p.rules.size()` is odd.
    template <typename W>
    void remove_redundant_generators(Presentation<W>& p);

    //! Return a possible letter by index.
    //!
    //! This function returns the \f$i\f$-th letter in the alphabet consisting
    //! of all possible letters of type Presentation<W>::letter_type. This
    //! function exists so that visible ASCII characters occur before invisible
    //! ones, so that when manipulating presentations over `std::string`s the
    //! human readable characters are used before non-readable ones.
    //!
    //! \tparam W the type of the words in the presentation
    //!
    //! \returns A `letter_type`.
    //!
    //! \throws LibsemigroupsException if `i` exceeds the number of letters in
    //! supported by `letter_type`.
    template <typename W>
    typename Presentation<W>::letter_type letter(Presentation<W> const&,
                                                 size_t i);

    //! Return a possible letter by index.
    //!
    //! This function returns the \f$i\f$-th letter in the alphabet consisting
    //! of all possible letters of type Presentation<std::string>::letter_type.
    //! This function exists so that visible ASCII characters occur before
    //! invisible ones, so that when manipulating presentations over
    //! `std::string`s the human readable characters are used before
    //! non-readable ones.
    //!
    //! \param p a presentation (unused)
    //! \param i the index
    //!
    //! \returns A `letter_type`.
    //!
    //! \throws LibsemigroupsException if \p i exceeds the number of letters in
    //! supported by `letter_type`.
    //!
    //! \sa `character(size_t)`
    template <>
    inline typename Presentation<std::string>::letter_type
    letter(Presentation<std::string> const& p, size_t i);

    //! Return a possible character by index.
    //!
    //! This function returns the \f$i\f$-th letter in the alphabet consisting
    //! of all possible letters of type Presentation<std::string>::letter_type.
    //! This function exists so that visible ASCII characters occur before
    //! invisible ones, so that when manipulating presentations over
    //! `std::string`s the human readable characters are used before
    //! non-readable ones.
    //!
    //! \param i the index
    //!
    //! \returns A `letter_type`.
    //!
    //! \throws LibsemigroupsException if \p i exceeds the number of letters in
    //! supported by `letter_type`.
    //!
    //! \sa `letter(Presentation<std::string> const&, size_t)`
    inline typename Presentation<std::string>::letter_type character(size_t i);

    //! Returns the first letter **not** in the alphabet of a presentation.
    //!
    //! This function returns `letter(p, i)` when `i` is the least possible
    //! value such that `!p.in_alphabet(letter(p, i))` if such a letter exists.
    //!
    //! \tparam W the type of the words in the presentation
    //!
    //! \param p the presentation
    //!
    //! \returns A `letter_type`.
    //!
    //! \throws LibsemigroupsException if \p p already has an alphabet of
    //! the maximum possible size supported by `letter_type`.
    template <typename W>
    typename Presentation<W>::letter_type
    first_unused_letter(Presentation<W> const& p);

    //! Convert a monoid presentation to a semigroup presentation.
    //!
    //! This function modifies its argument in-place by replacing the empty
    //! word in all relations by a new generator, and the identity rules for
    //! that new generator. If `p.contains_empty_word()` is `false`, then the
    //! presentation is not modified and \ref UNDEFINED is returned. If a new
    //! generator is added as the identity, then this generator is returned.
    //!
    //! \tparam W the type of the words in the presentation
    //!
    //! \param p the presentation
    //!
    //! \returns The new generator added, if any, and \ref UNDEFINED if not.
    //!
    //! \throws LibsemigroupsException if `replace_word` or
    //!  `add_identity_rules` does.
    template <typename W>
    typename Presentation<W>::letter_type make_semigroup(Presentation<W>& p);

    //! Greedily reduce the length of the presentation using
    //! `longest_common_subword`.
    //!
    //! This function repeatedly calls `longest_common_subword` and
    //! `replace_subword` to introduce a new generator and reduce the length of
    //! the presentation \p p until `longest_common_subword` returns the empty
    //! word.
    //!
    //! \tparam W the type of the words in the presentation
    //!
    //! \param p the presentation
    //!
    //! \returns (None)
    //!
    //! \throws LibsemigroupsException if `longest_common_subword` or
    //!  `replace_word` does.
    template <typename W>
    void greedy_reduce_length(Presentation<W>& p);

    //! Returns `true` if the \f$1\f$-relation presentation can be strongly
    //! compressed.
    //!
    //! A \f$1\f$-relation presentation is *strongly compressible* if both
    //! relation words start with the same letter and end with the same letter.
    //! In other words, if the alphabet of the presentation \p p is \f$A\f$ and
    //! the relation words are of the form \f$aub = avb\f$ where \f$a, b\in A\f$
    //! (possibly \f$ a = b\f$) and \f$u, v\in A ^ *\f$, then \p p is strongly
    //! compressible. See [Section
    //! 3.2](https://doi.org/10.1007/s00233-021-10216-8) for details.
    //!
    //! \tparam W the type of the words in the presentation
    //!
    //! \param p the presentation
    //!
    //! \returns A value of type `bool`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \sa `strongly_compress`
    // not noexcept because std::vector::operator[] isn't
    template <typename W>
    bool is_strongly_compressible(Presentation<W> const& p);

    //! Strongly compress a \f$1\f$-relation presentation.
    //!
    //! Returns `true` if the \f$1\f$-relation presentation \p p has been
    //! modified and `false` if not. The word problem is solvable for the input
    //! presentation if it is solvable for the modified version.
    //!
    //! \tparam W the type of the words in the presentation
    //!
    //! \param p the presentation
    //!
    //! \returns A value of type `bool`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \sa `is_strongly_compressible`
    // not noexcept because is_strongly_compressible isn't
    template <typename W>
    bool strongly_compress(Presentation<W>& p);

    //! Reduce the number of generators in a \f$1\f$-relation presentation to
    //! `2`.
    //!
    //! Returns `true` if the \f$1\f$-relation presentation \p p has been
    //! modified and `false` if not.
    //!
    //! A \f$1\f$-relation presentation is *left cycle-free* if the
    //! relation words start with distinct letters.
    //! In other words, if the alphabet of the presentation \p p is \f$A\f$ and
    //! the relation words are of the form \f$au = bv\f$ where \f$a, b\in A\f$
    //! with \f$a \neq b\f$ and \f$u, v \in A ^ *\f$, then \p p is left
    //! cycle-free. The word problem for a left cycle-free \f$1\f$-relation
    //! monoid is solvable if the word problem for the modified version
    //! obtained from this function is solvable.
    //!
    //! \tparam W the type of the words in the presentation
    //!
    //! \param p the presentation
    //! \param index determines the choice of letter to use, `0` uses
    //! `p.rules[0].front()` and `1` uses `p.rules[1].front()` (defaults to:
    //! `0`).
    //!
    //! \returns A value of type `bool`.
    //!
    //! \throws LibsemigroupsException if \p index is not `0` or `1`.
    template <typename W>
    bool reduce_to_2_generators(Presentation<W>& p, size_t index = 0);

  }  // namespace presentation

  template <typename T,
            typename = std::enable_if_t<std::is_same_v<T, word_type>>>
  inline word_type make(Presentation<std::string> const& p,
                        std::string const&               s) {
    word_type result(s.size(), 0);  // TODO required?
    std::transform(s.cbegin(), s.cend(), result.begin(), [&p](auto val) {
      return p.index(val);
    });
    return result;
  }
}  // namespace libsemigroups

#include "present.tpp"

#endif  // LIBSEMIGROUPS_PRESENT_HPP_
