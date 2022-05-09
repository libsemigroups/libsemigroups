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
#include <cstddef>           // for size_t
#include <initializer_list>  // for initializer_list
#include <iterator>          // for distance
#include <numeric>           // for accumulate
#include <string>            // for string
#include <unordered_map>     // for unordered_map
#include <unordered_set>     // for unordered_set
#include <vector>            // for vector

#include "order.hpp"        // for shortlex_compare
#include "suffix-tree.hpp"  // for SuffixTree
#include "uf.hpp"           // for Duf

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
  //! available in the namespace libsemigroups::presentation.
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

    //! Type of a const iterator to either side of a rule.
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
    //! \exceptions
    //! \no_libsemigroups_except
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

    //! Add a rule to the presentation
    //!
    //! Adds the rule with left hand side `[lhs_begin, lhs_end)` and
    //! right hand side `[rhs_begin, rhs_end)` to the rules.
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
    template <typename T>
    Presentation& add_rule(T lhs_begin, T lhs_end, T rhs_begin, T rhs_end) {
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
    template <typename T>
    Presentation&
    add_rule_and_check(T lhs_begin, T lhs_end, T rhs_begin, T rhs_end) {
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

    //! Add a rule to the presentation by char const pointer.
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

    //! Add a rule to the presentation by char const pointer.
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

    //! Add a rule to the presentation by initializer_list.
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

    //! Add a rule to the presentation by initializer_list.
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
      for (auto it = q.rules.cbegin(); it != q.rules.cend(); it += 2) {
        add_rule(p, *it, *(it + 1));
      }
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

    //! Add rules for inverses.
    //!
    //! The letter in \c a with index \c i in \p vals is the inverse of the
    //! letter in alphabet() with index \c i. The rules added are \f$a_ib_i =
    //! e\f$ where the alphabet is \f$\{a_1, \ldots, a_n\}\f$; the parameter \p
    //! vals is \f$\{b_1, \ldots, b_n\}\f$; and \f$e\f$ is the 3rd parameter.
    //!
    //! \tparam W the type of the words in the presentation
    //! \param p the presentation to add rules to
    //! \param vals the inverses
    //! \param e the identity element (defaults to UNDEFINED, meaning use the
    //! empty word)
    //!
    //! \returns (None)
    //!
    //! \throws LibsemigroupsException if any of the following apply:
    //! * the length of \p vals is not equal to `alphabet().size()`;
    //! * the letters in \p val are not exactly those in alphabet() (perhaps in
    //! a different order);
    //! * \f$(a_i ^ {-1}) ^ {-1} = a_i\f$ does not hold for some \f$i\f$;
    //! * \f$e ^ {-1} = e\f$ does not hold
    //!
    //! \complexity
    //! \f$O(n)\f$ where \f$n\f$ is alphabet().size().
    template <typename W>
    void add_inverse_rules(Presentation<W>&                      p,
                           W const&                              vals,
                           typename Presentation<W>::letter_type e = UNDEFINED);

    //! Add rules for inverses.
    //!
    //! The letter in \c a with index \c i in \p vals is the inverse of the
    //! letter in alphabet() with index \c i. The rules added are \f$a_ib_i =
    //! e\f$ where the alphabet is \f$\{a_1, \ldots, a_n\}\f$; the parameter \p
    //! vals is \f$\{b_1, \ldots, b_n\}\f$; and \f$e\f$ is the 3rd parameter.
    //!
    //! \tparam W the type of the words in the presentation
    //! \param p the presentation to add rules to
    //! \param vals the inverses
    //! \param e the identity element (defaults to UNDEFINED meaning use the
    //! empty word)
    //!
    //! \returns (None)
    //!
    //! \throws LibsemigroupsException if any of the following apply:
    //! * the length of \p vals is not equal to `alphabet().size()`;
    //! * the letters in \p val are not exactly those in alphabet() (perhaps in
    //! a different order);
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
    //! \exceptions
    //! \no_libsemigroups_except
    template <typename W>
    void remove_duplicate_rules(Presentation<W>& p);

    //! If there are rule \f$u = v\f$ and \f$v = w\f$ where \f$|w| < |v|\f$,
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
    //! \exceptions
    //! \no_libsemigroups_except
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
    //! \exceptions
    //! \no_libsemigroups_except
    template <typename W>
    void sort_each_rule(Presentation<W>& p);

    //! Sort the rules \f$u_1 = v_1, \ldots, u_n = v_n\f$ so that
    //! \f$u_1 < \cdots < u_n\f$ where \f$<\f$ is the shortlex order.
    //!
    //! \tparam W the type of the words in the presentation
    //! \param p the presentation to sort
    //!
    //! \returns (None)
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    template <typename W>
    void sort_rules(Presentation<W>& p);

    //! Returns the longest common subword of the rules.
    //!
    //! If it is possible to find a subword \f$w\f$ of the rules \f$u_1 = v_1,
    //! \ldots, u_n = v_n\f$ such that the introduction of a new generator
    //! \f$z\f$ and the relation \f$z = w\f$ reduces the presentation::length
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
    auto longest_common_subword(Presentation<W>& p);

    //! Replace non-overlapping instances of a subword via iterators.
    //!
    //! If \f$w=\f$`[first, last)` is a word, then replaces every
    //! non-overlapping instance of \f$w\f$ in every rule, adds a new
    //! generator \f$z\f$, and the rule \f$w = z\f$. The new generator and rule
    //! are added even if \f$w\f$ is not a subword of any rule.
    //!
    //! \tparam W the type of the words in the presentation
    //! \tparam T the type of the 2nd and 3rd parameters (iterators)
    //! \param p the presentation
    //! \param first the start of the subword to replace
    //! \param last one beyond the end of the subword to replace
    //!
    //! \returns (None)
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    // TODO(later) complexity
    template <typename W, typename T>
    void replace_subword(Presentation<W>& p, T first, T last);

    //! Replace non-overlapping instances of a subword via const reference.
    //!
    //! If \f$w=\f$`[first, last)` is a word, then replaces every
    //! non-overlapping instance of \f$w\f$ in every rule, adds a new
    //! generator \f$z\f$, and the rule \f$w = z\f$. The new generator and rule
    //! are added even if \f$w\f$ is not a subword of any rule.
    //!
    //! \tparam W the type of the words in the presentation
    //! \tparam T the type of the 2nd and 3rd parameters (iterators)
    //! \param p the presentation
    //! \param w the subword to replace
    //!
    //! \returns (None)
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    // TODO(later) complexity
    template <typename W>
    void replace_subword(Presentation<W>& p, W const& w) {
      replace_subword(p, w.cbegin(), w.cend());
    }

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
    size_t length(Presentation<W>& p) {
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

  }  // namespace presentation
}  // namespace libsemigroups

#include "present.tpp"

#endif  // LIBSEMIGROUPS_PRESENT_HPP_
