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

#ifndef LIBSEMIGROUPS_CONG_INTF_HPP_
#define LIBSEMIGROUPS_CONG_INTF_HPP_

#include <cstddef>           // for size_t
#include <cstdint>           // for uint64_t
#include <initializer_list>  // for initializer_list
#include <iterator>
#include <vector>  // for vector, operator==, vector<>::const_iter...

#include "runner.hpp"  // for Runner
#include "types.hpp"   // for word_type, relation_type, letter_type, tril
#include "words.hpp"   // for to_string

#include "ranges.hpp"  // for is_input_or_sink_v

namespace libsemigroups {
  class Congruence;

  //! Defined in ``cong-intf.hpp``.
  //!
  //! Every class for representing congruences in ``libsemigroups`` is derived
  //! from CongruenceInterface, which holds the member functions that are
  //! common to all its derived classes.
  class CongruenceInterface : public Runner {
    // Congruence has to be a friend so that we can call
    // add_pair_no_checks_no_reverse from within Congruence on the other derived
    // types of CongruenceInterface.
    friend class Congruence;
    /////////////////////////////////////////////////////////////////////////
    // CongruenceInterface - data members - private
    /////////////////////////////////////////////////////////////////////////

    std::vector<word_type> _generating_pairs;
    congruence_kind        _type;

   public:
    ////////////////////////////////////////////////////////////////////////////
    // CongruenceInterface - constructors + destructor - public
    ////////////////////////////////////////////////////////////////////////////

    CongruenceInterface() = default;

    CongruenceInterface& init() {
      _generating_pairs.clear();
      Runner::init();
      return *this;
    }

    //! Constructs a congruence of the specified type.
    explicit CongruenceInterface(congruence_kind type) : CongruenceInterface() {
      init(type);
    }

    CongruenceInterface& init(congruence_kind type) {
      init();
      _type = type;
      return *this;
    }

    //! Default copy constructor.
    CongruenceInterface(CongruenceInterface const&)            = default;
    CongruenceInterface(CongruenceInterface&&)                 = default;
    CongruenceInterface& operator=(CongruenceInterface const&) = default;
    CongruenceInterface& operator=(CongruenceInterface&&)      = default;

    ~CongruenceInterface();

    ////////////////////////////////////////////////////////////////////////////
    // CongruenceInterface - validation - public
    ////////////////////////////////////////////////////////////////////////////

    template <typename Subclass, typename Iterator1, typename Iterator2>
    void throw_if_letter_out_of_bounds(Iterator1 first, Iterator2 last) const {
      static_cast<Subclass const*>(this)->throw_if_letter_out_of_bounds(first,
                                                                        last);
    }

    ////////////////////////////////////////////////////////////////////////////
    // CongruenceInterface - generating pairs
    ////////////////////////////////////////////////////////////////////////////

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
    [[nodiscard]] size_t number_of_generating_pairs() const noexcept {
      return _generating_pairs.size() / 2;
    }

    [[nodiscard]] std::vector<word_type> const&
    generating_pairs() const noexcept {
      return _generating_pairs;
    }

   protected:
    // TODO(0) rename to add_generating_pair
    // TODO(0) should it be Subclass & ??
    template <typename Iterator1,
              typename Iterator2,
              typename Iterator3,
              typename Iterator4>
    CongruenceInterface& add_pair_no_checks_no_reverse(Iterator1 first1,
                                                       Iterator2 last1,
                                                       Iterator3 first2,
                                                       Iterator4 last2) {
      _generating_pairs.emplace_back(first1, last1);
      _generating_pairs.emplace_back(first2, last2);
      return *this;
    }

   public:
    ////////////////////////////////////////////////////////////////////////
    // CongruenceInterface - add_pair
    ////////////////////////////////////////////////////////////////////////

    // TODO(0) should it be Subclass & ??
    template <typename Iterator1,
              typename Iterator2,
              typename Iterator3,
              typename Iterator4>
    CongruenceInterface& add_pair_no_checks(Iterator1 first1,
                                            Iterator2 last1,
                                            Iterator3 first2,
                                            Iterator4 last2) {
      return add_pair_no_checks_no_reverse(first1, last1, first2, last2);
    }

    // TODO(0)
    template <typename Subclass,
              typename Iterator1,
              typename Iterator2,
              typename Iterator3,
              typename Iterator4>
    auto& add_pair(Iterator1 first1,
                   Iterator2 last1,
                   Iterator3 first2,
                   Iterator4 last2) {
      throw_if_started();
      throw_if_letter_out_of_bounds<Subclass>(first1, last1);
      throw_if_letter_out_of_bounds<Subclass>(first2, last2);
      return static_cast<Subclass&>(*this).add_pair_no_checks(
          first1, last1, first2, last2);
    }

    ////////////////////////////////////////////////////////////////////////
    // CongruenceInterface - contains
    ////////////////////////////////////////////////////////////////////////

    template <typename Iterator1,
              typename Iterator2,
              typename Iterator3,
              typename Iterator4>
    tril currently_contains(Iterator1 first1,
                            Iterator2 last1,
                            Iterator3 first2,
                            Iterator4 last2) const {
      throw_if_letter_out_of_bounds(first1, last1);
      throw_if_letter_out_of_bounds(first2, last2);
      return currently_contains_no_checks(first1, last1, first2, last2);
    }

    template <typename Iterator1,
              typename Iterator2,
              typename Iterator3,
              typename Iterator4>
    bool contains(Iterator1 first1,
                  Iterator2 last1,
                  Iterator3 first2,
                  Iterator4 last2) {
      throw_if_letter_out_of_bounds(first1, last1);
      throw_if_letter_out_of_bounds(first2, last2);
      return contains_no_checks(first1, last1, first2, last2);
    }

    ////////////////////////////////////////////////////////////////////////////
    // CongruenceInterface - settings
    ////////////////////////////////////////////////////////////////////////////

    //! The handedness of the congruence (left, right, or 2-sided).
    //!
    //! \returns A \ref congruence_kind.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    [[nodiscard]] congruence_kind kind() const noexcept {
      return _type;
    }

   protected:
    // TODO(0) make protected, shouldn't be able to change the kind arbitrarily
    CongruenceInterface& kind(congruence_kind knd) {
      _type = knd;
      return *this;
    }

    ////////////////////////////////////////////////////////////////////////////
    // OLD
    ////////////////////////////////////////////////////////////////////////////

   private:
    void throw_if_started() const;

    // TODO delete
    void add_pair_no_checks_no_reverse(word_type const& u, word_type const& v);
  };

  namespace congruence_interface {
    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - add_pair
    ////////////////////////////////////////////////////////////////////////

    // TODO(0) doc
    template <typename Word>
    inline CongruenceInterface& add_pair_no_checks(CongruenceInterface& ci,
                                                   Word const&          u,
                                                   Word const&          v) {
      return ci.add_pair_no_checks(
          std::begin(u), std::end(u), std::begin(v), std::end(v));
    }

    // TODO(0) doc
    template <typename Word>
    inline CongruenceInterface& add_pair_no_checks(CongruenceInterface& ci,
                                                   Word&&               u,
                                                   Word&&               v) {
      return ci.add_pair_no_checks(std::make_move_iterator(std::begin(u)),
                                   std::make_move_iterator(std::end(u)),
                                   std::make_move_iterator(std::begin(v)),
                                   std::make_move_iterator(std::end(v)));
    }

    // TODO(doc)
    template <typename Subclass, typename Int>
    Subclass& add_pair_no_checks(Subclass&                         ci,
                                 std::initializer_list<Int> const& u,
                                 std::initializer_list<Int> const& v) {
      return add_pair_no_checks<Subclass, std::vector<Int>>(ci, u, v);
    }

    // TODO(doc)
    inline CongruenceInterface& add_pair_no_checks(CongruenceInterface& ci,
                                                   char const*          u,
                                                   char const*          v) {
      return ci.add_pair_no_checks(
          u, u + std::strlen(u), v, v + std::strlen(v));
    }

    // TODO(doc)
    template <typename Subclass, typename Word>
    Subclass& add_pair(Subclass& ci, Word const& u, Word const& v) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      return ci.add_pair(
          std::begin(u), std::end(u), std::begin(v), std::end(v));
    }

    // TODO(doc)
    template <typename Subclass, typename Word>
    inline Subclass& add_pair(Subclass& ci, Word&& u, Word&& v) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      return ci.add_pair(std::make_move_iterator(std::begin(u)),
                         std::make_move_iterator(std::end(u)),
                         std::make_move_iterator(std::begin(v)),
                         std::make_move_iterator(std::end(v)));
    }

    // TODO(doc)
    template <typename Subclass, typename Int>
    Subclass& add_pair(Subclass&                         ci,
                       std::initializer_list<Int> const& u,
                       std::initializer_list<Int> const& v) {
      return add_pair<Subclass, std::vector<Int>>(ci, u, v);
    }

    // TODO(doc)
    template <typename Subclass>
    inline Subclass& add_pair(Subclass& ci, char const* u, char const* v) {
      static_assert(std::is_base_of_v<CongruenceInterface, Subclass>);
      return ci.add_pair(u, u + std::strlen(u), v, v + std::strlen(v));
    }

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - contains
    ////////////////////////////////////////////////////////////////////////

    // TODO(0) doc
    template <typename Subclass, typename Word>
    tril currently_contains_no_checks(Subclass const& ci,
                                      Word const&     u,
                                      Word const&     v) {
      return ci.currently_contains_no_checks(
          std::begin(u), std::end(u), std::begin(v), std::end(v));
    }

    template <typename Subclass>
    tril currently_contains_no_checks(Subclass&   ci,
                                      char const* u,
                                      char const* v) {
      return ci.currently_contains_no_checks(
          u, u + std::strlen(u), v, v + std::strlen(v));
    }

    // TODO(0) doc
    template <typename Subclass, typename Word>
    tril currently_contains(Subclass const& ci, Word const& u, Word const& v) {
      return ci.currently_contains(
          std::begin(u), std::end(u), std::begin(v), std::end(v));
    }

    // TODO(0) doc
    template <typename Subclass, typename Word>
    bool contains_no_checks(Subclass& ci, Word const& u, Word const& v) {
      return ci.contains_no_checks(
          std::begin(u), std::end(u), std::begin(v), std::end(v));
    }

    template <typename Subclass>
    bool contains_no_checks(Subclass& ci, char const* u, char const* v) {
      return ci.contains_no_checks(
          u, u + std::strlen(u), v, v + std::strlen(v));
    }

    // TODO(0) doc
    template <typename Subclass, typename Word>
    bool contains(Subclass& ci, Word const& u, Word const& v) {
      return ci.contains(
          std::begin(u), std::end(u), std::begin(v), std::end(v));
    }

    template <typename Subclass>
    bool contains(Subclass& ci, char const* u, char const* v) {
      return ci.contains(u, u + std::strlen(u), v, v + std::strlen(v));
    }

    // TODO(0) doc
    template <typename Subclass,
              typename Int = typename Subclass::native_letter_type>
    tril currently_contains_no_checks(Subclass const&                   ci,
                                      std::initializer_list<Int> const& u,
                                      std::initializer_list<Int> const& v) {
      return currently_contains_no_checks<Subclass, std::initializer_list<Int>>(
          ci, u, v);
    }

    // TODO(0) doc
    template <typename Subclass,
              typename Int = typename Subclass::native_letter_type>
    tril currently_contains(Subclass const&                   ci,
                            std::initializer_list<Int> const& u,
                            std::initializer_list<Int> const& v) {
      return currently_contains<Subclass, std::initializer_list<Int>>(ci, u, v);
    }

    // TODO(0) doc
    template <typename Subclass,
              typename Int = typename Subclass::native_letter_type>
    bool contains_no_checks(Subclass&                         ci,
                            std::initializer_list<Int> const& u,
                            std::initializer_list<Int> const& v) {
      return contains_no_checks<Subclass, std::initializer_list<Int>>(ci, u, v);
    }

    // TODO(0) doc
    template <typename Subclass,
              typename Int = typename Subclass::native_letter_type>
    bool contains(Subclass&                         ci,
                  std::initializer_list<Int> const& u,
                  std::initializer_list<Int> const& v) {
      return contains<Subclass, std::initializer_list<Int>>(ci, u, v);
    }
  }  // namespace congruence_interface

  template <typename Thing,
            typename Range,
            typename Word = std::decay_t<typename Range::output_type>,
            typename      = std::enable_if_t<rx::is_input_or_sink_v<Range>>>
  std::vector<std::vector<Word>> non_trivial_classes(Thing& ci, Range r) {
    auto result = partition(ci, r);
    result.erase(
        std::remove_if(result.begin(),
                       result.end(),
                       [](auto const& x) -> bool { return x.size() <= 1; }),
        result.end());
    return result;
  }

  template <typename Thing,
            typename Iterator1,
            typename Iterator2,
            typename Word
            = typename rx::iterator_range<Iterator1, Iterator2>::output_type>
  std::vector<std::vector<word_type>> partition(Thing&    ci,
                                                Iterator1 first,
                                                Iterator2 last) {
    return partition(ci, rx::iterator_range(first, last));
  }

  template <typename Thing,
            typename Iterator1,
            typename Iterator2,
            typename Word
            = typename rx::iterator_range<Iterator1, Iterator2>::output_type>
  std::vector<std::vector<word_type>> non_trivial_classes(Thing&    ci,
                                                          Iterator1 first,
                                                          Iterator2 last) {
    return non_trivial_classes(ci, rx::iterator_range(first, last));
  }

  namespace detail {

    // TODO delete?
    template <typename Subclass>
    struct ToStringIteratorTraits : ConstIteratorTraits<std::string> {
      using internal_iterator_type =
          typename ConstIteratorTraits<std::string>::internal_iterator_type;
      using state_type        = Subclass*;
      using value_type        = std::string;
      using reference         = value_type&;
      using const_reference   = value_type const&;
      using const_pointer     = value_type const*;
      using pointer           = value_type*;
      using iterator_category = std::bidirectional_iterator_tag;

      struct Deref {
        value_type operator()(state_type                    state,
                              internal_iterator_type const& it) const noexcept {
          return state->presentation().letter(*it);
        }
      };
    };

    template <typename Subclass>
    struct ToWordIteratorTraits : ConstIteratorTraits<word_type> {
      using internal_iterator_type =
          typename ConstIteratorTraits<word_type>::internal_iterator_type;
      using state_type        = Subclass*;
      using value_type        = word_type;
      using reference         = value_type&;
      using const_reference   = value_type const&;
      using const_pointer     = value_type const*;
      using pointer           = value_type*;
      using iterator_category = std::bidirectional_iterator_tag;

      struct Deref {
        value_type operator()(state_type             state,
                              internal_iterator_type it) const noexcept {
          return state->presentation().index_no_checks(*it);
        }
      };
    };
  }  // namespace detail

}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_CONG_INTF_HPP_
        // old doc follows TODO use it or lose it
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
// virtual bool contains(word_type const& u, word_type const& v) {
//   return u == v || word_to_class_index(u) == word_to_class_index(v);
// }

// bool contains_with_arg_checks(word_type const& lhs,
//                               word_type const& rhs) {
//   validate_word(lhs);
//   validate_word(rhs);
//   return contains(lhs, rhs);
// }

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
// virtual tril const_contains(word_type const& u, word_type const& v)
// const;

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
// virtual bool less(word_type const& u, word_type const& v) {
//   return word_to_class_index(u) < word_to_class_index(v);
// }

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
// word_type class_index_to_word(class_index_type i);

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
// class_index_type word_to_class_index(word_type const& w);

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
// TODO copy doc

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
// TODO copy doc

// TODO copy doc?
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

// TODO copy doc?
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

// TODO copy doc?
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
