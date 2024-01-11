//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2022-2023 James D. Mitchell
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

#include "ranges.hpp"  // for seq, operator|, rx, take

#include "adapters.hpp"   // for Hash, EqualTo
#include "constants.hpp"  // for Max, UNDEFINED, operator==
#include "debug.hpp"      // for LIBSEMIGROUPS_ASSERT
#include "exception.hpp"  // for LIBSEMIGROUPS_EXCEPTION
#include "order.hpp"      // for ShortLexCompare
#include "ranges.hpp"     // for chain, is_sorted
#include "types.hpp"      // for word_type
#include "ukkonen.hpp"    // for GreedyReduceHelper, Ukkonen
#include "words.hpp"      // for operator+

#include "detail/fmt.hpp"     // for format
#include "detail/report.hpp"  // for formatter<vector>
#include "detail/string.hpp"  // for maximum_common_prefix
#include "detail/uf.hpp"      // for Duf

namespace libsemigroups {

  //! No doc
  struct PresentationBase {};

  //! Defined in ``presentation.hpp``.
  //!
  //! This class template can be used to construction presentations for
  //! semigroups or monoids and is intended to be used as the input to other
  //! algorithms in `libsemigroups`. The idea is to provide a shallow wrapper
  //! around a vector of words of type `Word`. We refer to this vector of words
  //! as the rules of the presentation. The Presentation class template also
  //! provide some checks that the rules really defines a presentation, (i.e.
  //! it's consistent with its alphabet), and some related functionality is
  //! available in the namespace `libsemigroups::presentation`.
  //!
  //! \tparam Word the type of the underlying words.
  template <typename Word>
  class Presentation : public PresentationBase {
   public:
    //! The type of the words in the rules of a Presentation object.
    using word_type = Word;

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

    //! Remove the alphabet and all rules.
    //!
    //! This function clears the alphabet and all rules from the presentation,
    //! putting it back into the state it would be in if it was newly
    //! constructed.
    //!
    //! \param (None)
    //!
    //! \returns
    //! A reference to `this`.
    Presentation& init();

    //! Default copy constructor.
    Presentation(Presentation const&);

    //! Default move constructor.
    Presentation(Presentation&&);

    //! Default copy assignment operator.
    Presentation& operator=(Presentation const&);

    //! Default move assignment operator.
    Presentation& operator=(Presentation&&);

    ~Presentation();

    //! Returns the alphabet of the presentation.
    //!
    //! \returns A const reference to Presentation::word_type
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \param (None)
    [[nodiscard]] word_type const& alphabet() const noexcept {
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
    [[nodiscard]] letter_type letter_no_checks(size_type i) const {
      LIBSEMIGROUPS_ASSERT(i < _alphabet.size());
      return _alphabet[i];
    }

    // TODO(doc)
    [[nodiscard]] letter_type letter(size_type i) const;

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
    [[nodiscard]] size_type index_no_checks(letter_type val) const {
      return _alphabet_map.find(val)->second;
    }

    // TODO(doc)
    [[nodiscard]] size_type index(letter_type val) const;

    //! Check if a letter belongs to the alphabet or not.
    //!
    //! \no_libsemigroups_except
    //!
    //! \param val the letter to check
    //!
    //! \returns
    //! A value of type `bool`.
    [[nodiscard]] bool in_alphabet(letter_type val) const {
      return _alphabet_map.find(val) != _alphabet_map.cend();
    }

    //! Add a rule to the presentation
    //!
    //! Adds the rule with left hand side `[lhs_begin, lhs_end)` and
    //! right hand side `[rhs_begin, rhs_end)` to the rules.
    //!
    //! \tparam S the type of the first two parameters (iterators, or
    //! pointers)
    //! \tparam T the type of the second two parameters (iterators,
    //! or pointers)
    //!
    //! \param lhs_begin an iterator pointing to the first letter of the left
    //! hand side of the rule to be added
    //! \param lhs_end an iterator pointing one past the last letter of the
    //! left hand side of the rule to be added \param rhs_begin an iterator
    //! pointing to the first letter of the right hand side of the rule to be
    //! added \param rhs_end an iterator pointing one past the last letter of
    //! the right hand side of the rule to be added
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
    //! add_rule
    template <typename Iterator1, typename Iterator2>
    Presentation& add_rule_no_checks(Iterator1 lhs_begin,
                                     Iterator1 lhs_end,
                                     Iterator2 rhs_begin,
                                     Iterator2 rhs_end) {
      rules.emplace_back(lhs_begin, lhs_end);
      rules.emplace_back(rhs_begin, rhs_end);
      return *this;
    }

    //! Add a rule to the presentation and check it is valid
    //!
    //! Adds the rule with left hand side `[lhs_begin, lhs_end)` and right
    //! hand side `[rhs_begin, rhs_end)` to the rules and checks that they
    //! only contain letters in \ref alphabet. It is possible to add rules
    //! directly via the data member \ref rules, this function just exists to
    //! encourage adding rules with both sides defined at the same time.
    //!
    //! \tparam S the type of the first two parameters (iterators, or
    //! pointers) \tparam T the type of the second two parameters (iterators,
    //! or pointers)
    //!
    //! \param lhs_begin an iterator pointing to the first letter of the left
    //! hand side of the rule to be added
    //! \param lhs_end an iterator pointing one past the last letter of the
    //! left hand side of the rule to be added \param rhs_begin an iterator
    //! pointing to the first letter of the right hand side of the rule to be
    //! added \param rhs_end an iterator pointing one past the last letter of
    //! the right hand side of the rule to be added
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
    //! add_rule_no_checks
    template <typename Iterator1, typename Iterator2>
    Presentation& add_rule(Iterator1 lhs_begin,
                           Iterator1 lhs_end,
                           Iterator2 rhs_begin,
                           Iterator2 rhs_end) {
      validate_word(lhs_begin, lhs_end);
      validate_word(rhs_begin, rhs_end);
      return add_rule_no_checks(lhs_begin, lhs_end, rhs_begin, rhs_end);
    }

    //! Check if the presentation should contain the empty word
    //!
    //! \param (None)
    //!
    //! \returns A value of type `bool`
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] bool contains_empty_word() const noexcept {
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
    //! \throws LibsemigroupsException if there is a letter not in the
    //! alphabet between \p first and \p last
    //!
    //! \tparam T the type of the arguments (iterators)
    //! \param first iterator pointing at the first letter to check
    //! \param last iterator pointing one beyond the last letter to check
    //!
    //! \returns
    //! (None)
    template <typename Iterator>
    void validate_word(Iterator first, Iterator last) const;

    //! Check if every rule consists of letters belonging to the alphabet.
    //!
    //! \throws LibsemigroupsException if any word contains a letter not in
    //! the alphabet.
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

    template <typename Word, typename Iterator>
    void validate_rules(Presentation<Word> const& p,
                        Iterator                  first,
                        Iterator                  last) {
      for (auto it = first; it != last; ++it) {
        p.validate_word(it->cbegin(), it->cend());
      }
    }

    // TODO(now) doc
    template <typename Word>
    void validate_semigroup_inverses(Presentation<Word> const& p,
                                     Word const&               vals);

    //! Add a rule to the presentation by reference.
    //!
    //! Adds the rule with left hand side `lhop` and right hand side `rhop`
    //! to the rules.
    //!
    //! \tparam Word the type of the words in the presentation
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
    template <typename Word>
    void add_rule_no_checks(Presentation<Word>& p,
                            Word const&         lhop,
                            Word const&         rhop) {
      p.add_rule_no_checks(lhop.begin(), lhop.end(), rhop.begin(), rhop.end());
    }

    //! Add a rule to the presentation by reference and check.
    //!
    //! Adds the rule with left hand side \p lhop and right hand side \p rhop
    //! to the rules, after checking that \p lhop and \p rhop consist entirely
    //! of letters in the alphabet of \p p.
    //!
    //! \tparam Word the type of the words in the presentation
    //! \param p the presentation
    //! \param lhop the left hand side of the rule
    //! \param rhop the left hand side of the rule
    //!
    //! \returns (None)
    //!
    //! \throws LibsemigroupsException if \p lhop or \p rhop contains any
    //! letters not belonging to `p.alphabet()`.
    template <typename Word>
    void add_rule(Presentation<Word>& p, Word const& lhop, Word const& rhop) {
      p.add_rule(lhop.begin(), lhop.end(), rhop.begin(), rhop.end());
    }

    //! Add a rule to the presentation by `char const*`.
    //!
    //! Adds the rule with left hand side `lhop` and right hand side `rhop`
    //! to the rules.
    //!
    //! \tparam Word the type of the words in the presentation
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
    inline void add_rule_no_checks(Presentation<std::string>& p,
                                   char const*                lhop,
                                   char const*                rhop) {
      add_rule_no_checks(p, std::string(lhop), std::string(rhop));
    }

    //! Add a rule to the presentation by `char const*`.
    //!
    //! Adds the rule with left hand side `lhop` and right hand side `rhop`
    //! to the rules.
    //!
    //! \tparam Word the type of the words in the presentation
    //! \param p the presentation
    //! \param lhop the left hand side of the rule
    //! \param rhop the left hand side of the rule
    //!
    //! \returns (None)
    //!
    //! \throws LibsemigroupsException if \p lhop or \p rhop contains any
    //! letters not belonging to `p.alphabet()`.
    template <typename Word>
    void add_rule(Presentation<Word>& p, char const* lhop, char const* rhop) {
      add_rule(p, std::string(lhop), std::string(rhop));
    }

    inline void add_rule(Presentation<std::string>& p,
                         std::string const&         lhop,
                         char const*                rhop) {
      add_rule(p, lhop, std::string(rhop));
    }

    inline void add_rule(Presentation<std::string>& p,
                         char const*                lhop,
                         std::string const&         rhop) {
      add_rule(p, std::string(lhop), rhop);
    }

    inline void add_rule_no_checks(Presentation<std::string>& p,
                                   std::string const&         lhop,
                                   char const*                rhop) {
      add_rule_no_checks(p, lhop, std::string(rhop));
    }

    inline void add_rule_no_checks(Presentation<std::string>& p,
                                   char const*                lhop,
                                   std::string const&         rhop) {
      add_rule_no_checks(p, std::string(lhop), rhop);
    }

    //! Add a rule to the presentation by `initializer_list`.
    //!
    //! Adds the rule with left hand side `lhop` and right hand side `rhop`
    //! to the rules.
    //!
    //! \tparam Word the type of the words in the presentation
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
    template <typename Word, typename Letter>
    void add_rule_no_checks(Presentation<Word>&           p,
                            std::initializer_list<Letter> lhop,
                            std::initializer_list<Letter> rhop) {
      p.add_rule_no_checks(lhop.begin(), lhop.end(), rhop.begin(), rhop.end());
    }

    //! Add a rule to the presentation by `initializer_list`.
    //!
    //! Adds the rule with left hand side `lhop` and right hand side `rhop`
    //! to the rules.
    //!
    //! \tparam Word the type of the words in the presentation
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
    template <typename Word, typename Letter>
    void add_rule(Presentation<Word>&           p,
                  std::initializer_list<Letter> lhop,
                  std::initializer_list<Letter> rhop) {
      p.add_rule(lhop.begin(), lhop.end(), rhop.begin(), rhop.end());
    }

    template <typename Word, typename Iterator>
    void add_rules(Presentation<Word>& p, Iterator first, Iterator last) {
      for (auto it = first; it != last; it += 2) {
        add_rule(p, *it, *(it + 1));
      }
    }

    template <typename Word, typename Iterator>
    void add_rules_no_checks(Presentation<Word>& p,
                             Iterator            first,
                             Iterator            last) {
      for (auto it = first; it != last; it += 2) {
        add_rule_no_checks(p, *it, *(it + 1));
      }
    }

    //! Add a rule to the presentation from another presentation.
    //!
    //! Adds all the rules of the second argument to the first argument
    //! which is modified in-place.
    //!
    //! \tparam Word the type of the words in the presentation
    //! \param p the presentation to add rules to
    //! \param q the presentation with the rules to add
    //!
    //! \returns (None)
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    template <typename Word>
    void add_rules_no_checks(Presentation<Word>&       p,
                             Presentation<Word> const& q) {
      add_rules_no_checks(p, q.rules.cbegin(), q.rules.cend());
    }

    //! Add rules for an identity element.
    //!
    //! Adds rules of the form \f$ae = ea = a\f$ for every letter \f$a\f$ in
    //! the alphabet of \p p, and where \f$e\f$ is the second parameter.
    //!
    //! \tparam Word the type of the words in the presentation
    //! \param p the presentation to add rules to
    //! \param e the identity element
    //!
    //! \returns (None)
    //!
    //! \throws LibsemigroupsException if \p e is not a letter in
    //! `p.alphabet()`.
    template <typename Word>
    void add_identity_rules(Presentation<Word>&                      p,
                            typename Presentation<Word>::letter_type e);

    //! Add rules for a zero element.
    //!
    //! Adds rules of the form \f$az = za = z\f$ for every letter \f$a\f$ in
    //! the alphabet of \p p, and where \f$z\f$ is the second parameter.
    //!
    //! \tparam Word the type of the words in the presentation
    //! \param p the presentation to add rules to
    //! \param z the zero element
    //!
    //! \returns (None)
    //!
    //! \throws LibsemigroupsException if \p z is not a letter in
    //! `p.alphabet()`.
    template <typename Word>
    void add_zero_rules(Presentation<Word>&                      p,
                        typename Presentation<Word>::letter_type z);

    //! Add rules for inverses.
    //!
    //! The letter in \c a with index \c i in \p vals is the inverse of the
    //! letter in `alphabet()` with index \c i. The rules added are \f$a_ib_i
    //! = e\f$ where the alphabet is \f$\{a_1, \ldots, a_n\}\f$; the 2nd
    //! parameter \p vals is \f$\{b_1, \ldots, b_n\}\f$; and \f$e\f$ is the
    //! 3rd parameter.
    //!
    //! \tparam Word the type of the words in the presentation
    //! \param p the presentation to add rules to
    //! \param vals the inverses
    //! \param e the identity element (defaults to \ref UNDEFINED, meaning use
    //! the empty word)
    //!
    //! \returns (None)
    //!
    //! \throws LibsemigroupsException if any of the following apply:
    //! * the length of \p vals is not equal to `alphabet().size()`;
    //! * the letters in \p vals are not exactly those in `alphabet()`
    //! (perhaps in a different order);
    //! * \f$(a_i ^ {-1}) ^ {-1} = a_i\f$ does not hold for some \f$i\f$;
    //! * \f$e ^ {-1} = e\f$ does not hold
    //!
    //! \complexity
    //! \f$O(n)\f$ where \f$n\f$ is `alphabet().size()`.
    template <typename Word>
    void add_inverse_rules(Presentation<Word>&                      p,
                           Word const&                              vals,
                           typename Presentation<Word>::letter_type e
                           = UNDEFINED);

    //! Add rules for inverses.
    //!
    //! The letter in \c a with index \c i in \p vals is the inverse of the
    //! letter in `alphabet()` with index \c i. The rules added are \f$a_ib_i
    //! = e\f$ where the alphabet is \f$\{a_1, \ldots, a_n\}\f$; the 2nd
    //! parameter \p vals is \f$\{b_1, \ldots, b_n\}\f$; and \f$e\f$ is the
    //! 3rd parameter.
    //!
    //! \tparam Word the type of the words in the presentation
    //! \param p the presentation to add rules to
    //! \param vals the inverses
    //! \param e the identity element (defaults to \ref UNDEFINED meaning use
    //! the empty word)
    //!
    //! \returns (None)
    //!
    //! \throws LibsemigroupsException if any of the following apply:
    //! * the length of \p vals is not equal to `alphabet().size()`;
    //! * the letters in \p vals are not exactly those in `alphabet()`
    //! (perhaps in a different order);
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
    //! Removes all but one instance of any duplicate rules (if any). Note
    //! that rules of the form \f$u = v\f$ and \f$v = u\f$ (if any) are
    //! considered duplicates. Also note that the rules may be reordered by
    //! this function even if there are no duplicate rules.
    //!
    //! \tparam Word the type of the words in the presentation
    //! \param p the presentation
    //!
    //! \returns (None)
    //!
    //! \throws LibsemigroupsException if `p.rules.size()` is odd.
    template <typename Word>
    void remove_duplicate_rules(Presentation<Word>& p);

    //! Remove rules consisting of identical words.
    //!
    //! Removes all instance of rules (if any) where the left hand side and
    //! the right hand side are identical.
    //!
    //! \tparam Word the type of the words in the presentation
    //! \param p the presentation
    //!
    //! \returns (None)
    //!
    //! \throws LibsemigroupsException if `p.rules.size()` is odd.
    template <typename Word>
    void remove_trivial_rules(Presentation<Word>& p);

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
    //! \tparam Word the type of the words in the presentation
    //! \param p the presentation to add rules to
    //!
    //! \returns (None)
    //!
    //! \throws LibsemigroupsException if `p.rules.size()` is odd.
    template <typename Word>
    void reduce_complements(Presentation<Word>& p);

    //! Sort each rule \f$u = v\f$ so that the left hand side is shortlex
    //! greater than the right hand side.
    //!
    //! \tparam Word the type of the words in the presentation
    //! \param p the presentation to add rules to
    //!
    //! \returns (None)
    //!
    //! \throws LibsemigroupsException if `p.rules.size()` is odd.
    template <typename Word>
    bool sort_each_rule(Presentation<Word>& p);

    template <typename Word, typename Compare>
    bool sort_each_rule(Presentation<Word>& p, Compare cmp);

    // TODO(later) is_each_rule_sorted?

    //! Sort the rules \f$u_1 = v_1, \ldots, u_n = v_n\f$ so that
    //! \f$u_1v_1 < \cdots < u_nv_n\f$ where \f$<\f$ is the shortlex order.
    //!
    //! \tparam Word the type of the words in the presentation
    //! \param p the presentation to sort
    //!
    //! \returns (None)
    //!
    //! \throws LibsemigroupsException if `p.rules.size()` is odd.
    template <typename Word, typename Compare>
    void sort_rules(Presentation<Word>& p, Compare cmp);

    template <typename Word>
    void sort_rules(Presentation<Word>& p) {
      sort_rules(p, ShortLexCompare());
    }

    //! Check if the rules \f$u_1 = v_1, \ldots, u_n = v_n\f$ satisfy
    //! \f$u_1v_1 < \cdots < u_nv_n\f$ where \f$<\f$ is the shortlex order.
    //!
    //! \tparam Word the type of the words in the presentation
    //! \param p the presentation
    //!
    //! \returns A value of type `bool`.
    //!
    //! \throws LibsemigroupsException if `p.rules.size()` is odd.
    template <typename Word, typename Compare>
    bool are_rules_sorted(Presentation<Word> const& p, Compare cmp);

    template <typename Word>
    bool are_rules_sorted(Presentation<Word> const& p) {
      return are_rules_sorted(p, ShortLexCompare());
    }

    //! Returns the longest common subword of the rules.
    //!
    //! If it is possible to find a subword \f$w\f$ of the rules \f$u_1 = v_1,
    //! \ldots, u_n = v_n\f$ such that the introduction of a new generator
    //! \f$z\f$ and the relation \f$z = w\f$ reduces the `presentation::length`
    //! of the presentation, then this function returns the longest such word
    //! \f$w\f$. If no such word can be found, then a word of length \f$0\f$ is
    //! returned.
    //!
    //! \tparam Word the type of the words in the presentation
    //! \param p the presentation
    //!
    //! \returns A value of type \p Word.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    // TODO(later) complexity
    template <typename Word>
    Word longest_subword_reducing_length(Presentation<Word>& p);

    // TODO(doc)
    template <typename Word>
    typename Presentation<Word>::letter_type
    add_generator(Presentation<Word>& p);

    // TODO(doc)
    template <typename Word>
    void add_generator_no_checks(Presentation<Word>&                      p,
                                 typename Presentation<Word>::letter_type x);
    // TODO(doc)
    template <typename Word>
    void add_generator(Presentation<Word>&                      p,
                       typename Presentation<Word>::letter_type x);

    //! Replace non-overlapping instances of a subword via iterators.
    //!
    //! If \f$w=\f$`[first, last)` is a word, then this function replaces
    //! every non-overlapping instance of \f$w\f$ in every rule, adds a new
    //! generator \f$z\f$, and the rule \f$w = z\f$. The new generator and
    //! rule are added even if \f$w\f$ is not a subword of any rule.
    //!
    //! \tparam Word the type of the words in the presentation
    //! \tparam T the type of the 2nd and 3rd parameters (iterators)
    //! \param p the presentation
    //! \param first the start of the subword to replace
    //! \param last one beyond the end of the subword to replace
    //!
    //! \returns (None)
    //!
    //! \throws LibsemigroupsException if `first == last`.
    // TODO(later) complexity
    template <typename Word,
              typename Iterator,
              typename = std::enable_if_t<!std::is_same<Iterator, Word>::value>>
    typename Presentation<Word>::letter_type
    replace_word_with_new_generator(Presentation<Word>& p,
                                    Iterator            first,
                                    Iterator            last);

    //! Replace non-overlapping instances of a word with a new generator via
    //! const reference.
    //!
    //! If \f$w=\f$`[first, last)` is a word, then this function replaces
    //! every non-overlapping instance (from left to right) of \f$w\f$ in
    //! every rule, adds a new generator \f$z\f$, and the rule \f$w = z\f$.
    //! The new generator and rule are added even if \f$w\f$ is not a subword
    //! of any rule.
    //!
    //! \tparam Word the type of the words in the presentation
    //!
    //! \param p the presentation
    //! \param w the subword to replace
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

    //! Replace non-overlapping instances of a subword via `char const*`.
    //!
    //! If \f$w=\f$`[first, last)` is a word, then replaces every
    //! non-overlapping instance (from left to right) of \f$w\f$ in every
    //! rule, adds a new generator \f$z\f$, and the rule \f$w = z\f$. The new
    //! generator and rule are added even if \f$w\f$ is not a subword of any
    //! rule.
    //!
    //! \param p the presentation
    //! \param w the subword to replace
    //!
    //! \returns The new generator added.
    //!
    //! \throws LibsemigroupsException if \p w is empty.
    inline typename Presentation<std::string>::letter_type
    replace_word_with_new_generator(Presentation<std::string>& p,
                                    char const*                w) {
      return replace_word_with_new_generator(p, w, w + std::strlen(w));
    }

    //! Replace non-overlapping instances of a subword by another word.
    //!
    //! If \p existing and \p replacement are words, then this function
    //! replaces every non-overlapping instance of \p existing in every rule
    //! by \p replacement. The presentation \p p is changed in-place.
    //!
    //! \tparam Word the type of the words in the presentation
    //! \param p the presentation
    //! \param existing the word to be replaced
    //! \param replacement the replacement word.
    //!
    //! \returns (None)
    //!
    //! \throws LibsemigroupsException if `existing` is empty.
    // TODO(later) complexity
    template <typename Word>
    void replace_subword(Presentation<Word>& p,
                         Word const&         existing,
                         Word const&         replacement);

    //! Replace non-overlapping instances of a subword by another word.
    //!
    //! Adds the rule with left hand side `[lhs_begin, lhs_end)` and
    //! right hand side `[rhs_begin, rhs_end)` to the rules.
    //!
    //! \tparam S the type of the first two parameters (iterators, or
    //! pointers) \tparam T the type of the second two parameters (iterators,
    //! or pointers)
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
    template <typename Word, typename Iterator1, typename Iterator2>
    void replace_subword(Presentation<Word>& p,
                         Iterator1           first_existing,
                         Iterator1           last_existing,
                         Iterator2           first_replacement,
                         Iterator2           last_replacement);

    inline void replace_subword(Presentation<std::string>& p,
                                char const*                existing,
                                char const*                replacement) {
      replace_subword(p,
                      existing,
                      existing + std::strlen(existing),
                      replacement,
                      replacement + std::strlen(replacement));
    }

    //! Replace instances of a word on either side of a rule by another word.
    //!
    //! If \p existing and \p replacement are words, then this function
    //! replaces every instance of \p existing in every rule of the form
    //! \p existing \f$= w\f$ or \f$w = \f$ \p existing, with the word
    //! \p replacement. The presentation \p p is changed in-place.
    //!
    //! \tparam Word the type of the words in the presentation
    //!
    //! \param p the presentation
    //! \param existing the word to be replaced
    //! \param replacement the replacement word
    //!
    //! \returns (None)
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    template <typename Word>
    void replace_word(Presentation<Word>& p,
                      Word const&         existing,
                      Word const&         replacement);

    // TODO to tpp
    template <typename Iterator>
    size_t length(Iterator first, Iterator last) {
      auto op = [](size_t val, auto const& x) { return val + x.size(); };
      return std::accumulate(first, last, size_t(0), op);
    }

    //! Return the sum of the lengths of the rules.
    //!
    //! \tparam Word the type of the words in the presentation
    //! \param p the presentation
    //!
    //! \returns A value of type `size_t`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    template <typename Word>
    size_t length(Presentation<Word> const& p) {
      return length(p.rules.cbegin(), p.rules.cend());
    }

    //! Reverse every rule.
    //!
    //! \tparam Word the type of the words in the presentation
    //! \param p the presentation
    //!
    //! \returns (None)
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    template <typename Word>
    void reverse(Presentation<Word>& p) {
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
    //! \tparam Word the type of the words in the presentation
    //! \param p the presentation
    //!
    //! \returns (None)
    //!
    //! \throws LibsemigroupsException if \ref validate throws on the initial
    //! presentation.
    template <typename Word>
    void normalize_alphabet(Presentation<Word>& p);

    //! Change or re-order the alphabet.
    //!
    //! This function replaces `p.alphabet()` with \p new_alphabet, where
    //! possible, and re-writes the rules in the presentation using the new
    //! alphabet.
    //!
    //! \tparam Word the type of the words in the presentation
    //!
    //! \param p the presentation
    //! \param new_alphabet the replacement alphabet
    //!
    //! \returns (None).
    //!
    //! \throws LibsemigroupsException if the size of `p.alphabet()` and \p
    //! new_alphabet do not agree.
    template <typename Word>
    void change_alphabet(Presentation<Word>& p, Word const& new_alphabet);

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
    template <typename Iterator>
    Iterator longest_rule(Iterator first, Iterator last);

    //! Returns an iterator pointing at the left hand side of the first
    //! rule in the presentation with maximal length.
    //!
    //! The *length* of a rule is defined to be the sum of the lengths of its
    //! left and right hand sides.
    //!
    //! \tparam Word the type of the words in the presentation
    //! \param p the presentation
    //!
    //! \returns A value of type `std::vector<Word>::const_iterator`.
    //!
    //! \throws LibsemigroupsException if the length of \p p.rules is odd.
    template <typename Word>
    auto longest_rule(Presentation<Word> const& p) {
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
    template <typename Iterator>
    Iterator shortest_rule(Iterator first, Iterator last);

    //! Returns an iterator pointing at the left hand side of the first
    //! rule in the presentation with minimal length.
    //!
    //! The *length* of a rule is defined to be the sum of the lengths of its
    //! left and right hand sides.
    //!
    //! \tparam Word the type of the words in the presentation
    //! \param p the presentation
    //!
    //! \returns A value of type `std::vector<Word>::const_iterator`.
    //!
    //! \throws LibsemigroupsException if the length of \p p.rules is odd.
    template <typename Word>
    auto shortest_rule(Presentation<Word> const& p) {
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
    template <typename Iterator>
    auto longest_rule_length(Iterator first, Iterator last);

    //! Returns the maximum length of a rule in the presentation.
    //!
    //! The *length* of a rule is defined to be the sum of the lengths of its
    //! left and right hand sides.
    //!
    //! \tparam Word the type of the words in the presentation
    //! \param p the presentation
    //!
    //! \returns A value of type `Word::size_type`.
    //!
    //! \throws LibsemigroupsException if the length of \p p.rules is odd.
    template <typename Word>
    auto longest_rule_length(Presentation<Word> const& p) {
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
    template <typename Iterator>
    auto shortest_rule_length(Iterator first, Iterator last);

    //! Returns the minimum length of a rule in the presentation.
    //!
    //! The *length* of a rule is defined to be the sum of the lengths of its
    //! left and right hand sides.
    //!
    //! \tparam Word the type of the words in the presentation
    //! \param p the presentation
    //!
    //! \returns A value of type `Word::size_type`.
    //!
    //! \throws LibsemigroupsException if the length of \p p.rules is odd.
    template <typename Word>
    auto shortest_rule_length(Presentation<Word> const& p) {
      return shortest_rule_length(p.rules.cbegin(), p.rules.cend());
    }

    //! Remove any trivially redundant generators.
    //!
    //! If one side of any of the rules in the presentation \p p is a letter
    //! \c a and the other side of the rule does not contain \c a, then this
    //! function replaces every occurrence of \c a in every rule by the other
    //! side of the rule. This substitution is performed for every such
    //! rule in the presentation; and the trivial rules (with both sides being
    //! identical) are removed. If both sides of a rule are letters, then the
    //! greater letter is replaced by the lesser one.
    //!
    //! \tparam Word the type of the words in the presentation
    //!
    //! \returns (None)
    //!
    //! \throws LibsemigroupsException if `p.rules.size()` is odd.
    template <typename Word>
    void remove_redundant_generators(Presentation<Word>& p);

    //! Return a possible letter by index.
    //!
    //! This function returns the \f$i\f$-th letter in the alphabet consisting
    //! of all possible letters of type Presentation<Word>::letter_type. This
    //! function exists so that visible ASCII characters occur before
    //! invisible ones, so that when manipulating presentations over
    //! `std::string`s the human readable characters are used before
    //! non-readable ones.
    //!
    //! \tparam Word the type of the words in the presentation
    //!
    //! TODO(doc) args
    //!
    //! \returns A `letter_type`.
    //!
    //! \throws LibsemigroupsException if `i` exceeds the number of letters in
    //! supported by `letter_type`.
    // TODO move to words.*pp
    template <typename Word>
    typename Presentation<Word>::letter_type
    human_readable_letter(Presentation<Word> const&, size_t i);

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
    //! \throws LibsemigroupsException if \p i exceeds the number of letters
    //! in supported by `letter_type`.
    // TODO move to words.*pp
    typename Presentation<std::string>::letter_type
    human_readable_letter(Presentation<std::string> const& p, size_t i);

    //! Returns the first letter **not** in the alphabet of a presentation.
    //!
    //! This function returns `letter(p, i)` when `i` is the least possible
    //! value such that `!p.in_alphabet(letter(p, i))` if such a letter
    //! exists.
    //!
    //! \tparam Word the type of the words in the presentation
    //!
    //! \param p the presentation
    //!
    //! \returns A `letter_type`.
    //!
    //! \throws LibsemigroupsException if \p p already has an alphabet of
    //! the maximum possible size supported by `letter_type`.
    template <typename Word>
    typename Presentation<Word>::letter_type
    first_unused_letter(Presentation<Word> const& p);

    //! Convert a monoid presentation to a semigroup presentation.
    //!
    //! This function modifies its argument in-place by replacing the empty
    //! word in all relations by a new generator, and the identity rules for
    //! that new generator. If `p.contains_empty_word()` is `false`, then the
    //! presentation is not modified and \ref UNDEFINED is returned. If a new
    //! generator is added as the identity, then this generator is returned.
    //!
    //! \tparam Word the type of the words in the presentation
    //!
    //! \param p the presentation
    //!
    //! \returns The new generator added, if any, and \ref UNDEFINED if not.
    //!
    //! \throws LibsemigroupsException if `replace_word` or
    //!  `add_identity_rules` does.
    template <typename Word>
    typename Presentation<Word>::letter_type
    make_semigroup(Presentation<Word>& p);

    //! Greedily reduce the length of the presentation using
    //! `longest_subword_reducing_length`.
    //!
    //! This function repeatedly calls `longest_subword_reducing_length` and
    //! `replace_subword` to introduce a new generator and reduce the length
    //! of the presentation \p p until `longest_subword_reducing_length` returns
    //! the empty word.
    //!
    //! \tparam Word the type of the words in the presentation
    //!
    //! \param p the presentation
    //!
    //! \returns (None)
    //!
    //! \throws LibsemigroupsException if `longest_subword_reducing_length` or
    //!  `replace_word` does.
    template <typename Word>
    void greedy_reduce_length(Presentation<Word>& p);

    //! Returns `true` if the \f$1\f$-relation presentation can be strongly
    //! compressed.
    //!
    //! A \f$1\f$-relation presentation is *strongly compressible* if both
    //! relation words start with the same letter and end with the same
    //! letter. In other words, if the alphabet of the presentation \p p is
    //! \f$A\f$ and the relation words are of the form \f$aub = avb\f$ where
    //! \f$a, b\in A\f$ (possibly \f$ a = b\f$) and \f$u, v\in A ^ *\f$, then
    //! \p p is strongly compressible. See [Section
    //! 3.2](https://doi.org/10.1007/s00233-021-10216-8) for details.
    //!
    //! \tparam Word the type of the words in the presentation
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
    template <typename Word>
    bool is_strongly_compressible(Presentation<Word> const& p);

    //! Strongly compress a \f$1\f$-relation presentation.
    //!
    //! Returns `true` if the \f$1\f$-relation presentation \p p has been
    //! modified and `false` if not. The word problem is solvable for the
    //! input presentation if it is solvable for the modified version.
    //!
    //! \tparam Word the type of the words in the presentation
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
    template <typename Word>
    bool strongly_compress(Presentation<Word>& p);

    //! Reduce the number of generators in a \f$1\f$-relation presentation to
    //! `2`.
    //!
    //! Returns `true` if the \f$1\f$-relation presentation \p p has been
    //! modified and `false` if not.
    //!
    //! A \f$1\f$-relation presentation is *left cycle-free* if the
    //! relation words start with distinct letters.
    //! In other words, if the alphabet of the presentation \p p is \f$A\f$
    //! and the relation words are of the form \f$au = bv\f$ where \f$a, b\in
    //! A\f$ with \f$a \neq b\f$ and \f$u, v \in A ^ *\f$, then \p p is left
    //! cycle-free. The word problem for a left cycle-free \f$1\f$-relation
    //! monoid is solvable if the word problem for the modified version
    //! obtained from this function is solvable.
    //!
    //! \tparam Word the type of the words in the presentation
    //!
    //! \param p the presentation
    //! \param index determines the choice of letter to use, `0` uses
    //! `p.rules[0].front()` and `1` uses `p.rules[1].front()` (defaults to:
    //! `0`).
    //!
    //! \returns A value of type `bool`.
    //!
    //! \throws LibsemigroupsException if \p index is not `0` or `1`.
    template <typename Word>
    bool reduce_to_2_generators(Presentation<Word>& p, size_t index = 0);

    // TODO(doc)
    template <typename Word>
    void add_idempotent_rules_no_checks(Presentation<Word>& p,
                                        Word const&         letters) {
      for (auto x : letters) {
        add_rule_no_checks(p, {x, x}, {x});
      }
    }

    template <typename Word>
    void add_transposition_rules_no_checks(Presentation<Word>& p,
                                           word_type const&    letters) {
      for (auto x : letters) {
        add_rule_no_checks(p, {x, x}, {});
      }
    }
    // TODO(doc)
    template <typename Word>
    void add_commutes_rules_no_checks(Presentation<Word>& p,
                                      Word const&         letters1,
                                      Word const&         letters2);

    // TODO(doc)
    template <typename Word>
    void add_commutes_rules_no_checks(Presentation<Word>& p,
                                      Word const&         letters) {
      add_commutes_rules_no_checks(p, letters, letters);
    }

    // TODO(doc)
    template <typename Word>
    void add_commutes_rules_no_checks(Presentation<Word>&         p,
                                      Word const&                 letters,
                                      std::initializer_list<Word> words);

    // TODO to tpp
    template <typename Word>
    void balance(Presentation<Word>& p,
                 Word const&         letters,
                 Word const&         inverses) {
      // TODO check args
      // So that longer relations are on the lhs
      presentation::sort_each_rule(p);

      std::unordered_map<typename Word::value_type, size_t> map;

      for (auto [i, x] : rx::enumerate(letters)) {
        map.emplace(x, i);
      }

      for (auto it = p.rules.begin(); it != p.rules.end(); it += 2) {
        auto& l = *it;
        auto& r = *(it + 1);
        // Check that we aren't actually about to remove one of the inverse
        // relations itself
        if (l.size() == 2 && r.empty()) {
          auto mit = map.find(l.front());
          if (mit != map.cend() && l.back() == inverses[mit->second]) {
            continue;
          }
        }

        size_t const min = (l.size() + r.size()) % 2;
        while (l.size() - r.size() > min) {
          auto mit = map.find(l.back());
          if (mit != map.cend()) {
            r.insert(r.end(), inverses[mit->second]);
            l.erase(l.end() - 1);
          } else {
            break;
          }
        }
        while (l.size() - r.size() > min) {
          auto mit = map.find(l.front());
          if (mit != map.cend()) {
            r.insert(r.begin(), inverses[mit->second]);
            l.erase(l.begin());
          } else {
            break;
          }
        }
      }
    }

    inline void balance(Presentation<std::string>& p,
                        char const*                letters,
                        char const*                inverses) {
      balance(p, std::string(letters), std::string(inverses));
    }

    // TODO do a proper version of this
    template <typename Word>
    void add_cyclic_conjugates(Presentation<Word>& p,
                               Word const&         lhs,
                               Word const&         rhs) {
      for (size_t i = 0; i < lhs.size(); ++i) {
        std::string lcopy(rhs.crbegin(), rhs.crbegin() + i);
        lcopy.insert(lcopy.end(), lhs.cbegin() + i, lhs.cend());
        for (auto it = lcopy.begin(); it < lcopy.begin() + i; ++it) {
          if (std::isupper(*it)) {
            *it = std::tolower(*it);
          } else {
            *it = std::toupper(*it);
          }
        }

        std::string rcopy(rhs.cbegin(), rhs.cend() - i + 1);
        rcopy.insert(rcopy.end(), lhs.crbegin(), lhs.crend() + i);
        for (auto it = rcopy.end() - i; it < rcopy.end(); ++it) {
          if (std::isupper(*it)) {
            *it = std::tolower(*it);
          } else {
            *it = std::tolower(*it);
          }
        }
        presentation::add_rule(p, lcopy, rcopy);
      }
    }

    // TODO(doc)
    std::string to_gap_string(Presentation<word_type> const& p,
                              std::string const&             var_name);

    std::string to_gap_string(Presentation<std::string> const& p,
                              std::string const&               var_name);

  }  // namespace presentation

  template <typename Word>
  class InversePresentation : public Presentation<Word> {
   public:
    using word_type      = typename Presentation<Word>::word_type;
    using letter_type    = typename Presentation<Word>::letter_type;
    using const_iterator = typename Presentation<Word>::const_iterator;
    using iterator       = typename Presentation<Word>::iterator;
    using size_type      = typename Presentation<Word>::size_type;

   private:
    word_type _inverses;

   public:
    using Presentation<Word>::Presentation;

    // TODO init functions
    InversePresentation<Word>(Presentation<Word> const& p)
        : Presentation<Word>(p), _inverses() {}

    InversePresentation<Word>(Presentation<Word>&& p)
        : Presentation<Word>(p), _inverses() {}

    // TODO validate that checks that inverses are set
    // TODO to tpp
    InversePresentation& inverses(word_type const& w) {
      // TODO maybe don't validate here but only in the validate function to
      // be written. Set the alphabet to include the inverses
      _inverses = w;
      return *this;
    }

    word_type const& inverses() const noexcept {
      return _inverses;
    }

    // TODO to tpp
    letter_type inverse(letter_type x) const {
      if (_inverses.empty()) {
        LIBSEMIGROUPS_EXCEPTION("no inverses have been defined")
      }
      return _inverses[this->index(x)];
    }

    void validate() const {
      Presentation<Word>::validate();
      presentation::validate_semigroup_inverses(*this, inverses());
    }
  };

  // TODO(doc)
  // TODO(later) also we could do a more sophisticated version of this
  template <typename Word>
  bool operator==(Presentation<Word> const& lhop,
                  Presentation<Word> const& rhop) {
    return lhop.alphabet() == rhop.alphabet() && lhop.rules == rhop.rules;
  }

  template <typename Word>
  bool operator!=(Presentation<Word> const& lhop,
                  Presentation<Word> const& rhop) {
    return !(lhop == rhop);
  }

  // TODO(later) could do a no_check version
  void to_word(Presentation<std::string> const& p,
               std::string const&               input,
               word_type&                       output);

  // TODO(later) could do a no_check version
  word_type to_word(Presentation<std::string> const& p,
                    std::string const&               input);

  // TODO(later) could do a no_check version
  void to_string(Presentation<std::string> const& p,
                 word_type const&                 input,
                 std::string&                     output);

  // TODO(later) could do a no check version
  std::string to_string(Presentation<std::string> const& p, word_type const& w);

  namespace detail {
    template <typename T>
    struct IsPresentationHelper : std::false_type {};

    template <typename T>
    struct IsPresentationHelper<Presentation<T>> : std::true_type {};

    template <typename T>
    struct IsPresentationHelper<InversePresentation<T>> : std::true_type {};

    template <typename T>
    struct IsInversePresentationHelper : std::false_type {};

    template <typename T>
    struct IsInversePresentationHelper<InversePresentation<T>>
        : std::true_type {};
  }  // namespace detail

  template <typename T>
  static constexpr bool IsInversePresentation
      = detail::IsInversePresentationHelper<T>::value;

  template <typename T>
  static constexpr bool IsPresentation = detail::IsPresentationHelper<T>::value;
}  // namespace libsemigroups

#include "presentation.tpp"

#endif  // LIBSEMIGROUPS_PRESENTATION_HPP_
