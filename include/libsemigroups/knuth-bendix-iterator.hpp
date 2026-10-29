//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2026 Joseph Edwards
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

// This file contains the declaration of the KnuthBendixIterator class

// TODO:
// * Reporting
// * Replace RewritingSystemBacktrack with something that derives from
//   RewritingSystemBase

#ifndef LIBSEMIGROUPS_KNUTH_BENDIX_ITERATOR_HPP_
#define LIBSEMIGROUPS_KNUTH_BENDIX_ITERATOR_HPP_

#include <cstddef>   // for ptrdiff_t
#include <iterator>  // for forward_iterator_tag, distance
#include <utility>   // for pair, move
#include <vector>    // for vector

#include "libsemigroups/presentation.hpp"  // for Presentation

#include "libsemigroups/detail/rules.hpp"  // for Rule::native_word_type

namespace libsemigroups {
  namespace detail {

    enum class Orientation : bool { original, flipped };

    class RewritingSystemBacktrack {
     public:
      using native_word_type = typename Rule::native_word_type;

      explicit RewritingSystemBacktrack(size_t max_num_rules) : _rule_stack{} {
        _rule_stack.reserve(max_num_rules);
      }

      void add_rule(native_word_type const& lhs, native_word_type const& rhs) {
        LIBSEMIGROUPS_ASSERT(!lhs.empty());
        _rule_stack.emplace_back(lhs, rhs);
      }

      void pop_rule() {
        _rule_stack.pop_back();
      }

      bool rewrite(native_word_type& word, size_t max_rewrite_depth) const;

      std::vector<std::pair<native_word_type, native_word_type>>
      rules() const noexcept {
        return _rule_stack;
      }

     private:
      std::vector<std::pair<native_word_type, native_word_type>> _rule_stack;
    };

  }  // namespace detail

  //! \ingroup knuth_bendix_group
  //!
  //! \brief An input iterator over confluent rewriting systems that uses the
  //! Knuth-Bendix algorithm.
  //!
  //! This class enumerates confluent rewriting systems obtained from a
  //! backtracking version of the Knuth-Bendix algorithm. The iterator is
  //! bounded by a maximum rewriting depth and a maximum number of rules in the
  //! rewriting system.
  //!
  //! A default-constructed iterator is an end iterator. The comparison
  //! operators are weak comparisons intended primarily for comparing an
  //! iterator with the value returned by
  //! \ref end_knuth_bendix_iterator.
  //!
  //! \par Example
  //! \code
  //! Presentation<std::string> p;
  //! p.alphabet("ab");
  //! presentation::add_rule(p, "a", "b");
  //! KnuthBendixIterator kbb(p, 100, 10);
  //!
  //! Presentation<std::string> expected;
  //! expected.alphabet("ab");
  //! expected.rules = {"a", "b"};
  //!
  //! (*kbb == expected);  //-> true
  //!
  //! ++kbb;
  //! expected.rules = {"b", "a"};
  //! (*kbb == expected);  //-> true
  //! \endcode
  // TODO: Make max_number_of_rules a template parameter?
  class KnuthBendixIterator {
   public:
    //////////////////////////////////////////////////////////////////////
    // Public aliases
    //////////////////////////////////////////////////////////////////////

    //! The type of words in the presentation.
    using native_word_type =
        typename detail::RewritingSystemBacktrack::native_word_type;

    //! The type of the presentations produced by the iterator.
    using value_type = Presentation<native_word_type>;

    //! The type returned when dereferencing the iterator.
    using reference = value_type const&;

    //! The const reference type returned when dereferencing the iterator.
    using const_reference = value_type const&;

    //! The type used to represent distances between iterators.
    using difference_type = std::ptrdiff_t;

    //! An unsigned integral type used for sizes.
    using size_type = size_t;

    //! The pointer type returned by \ref operator->.
    using const_pointer = value_type const*;

    //! The const pointer type returned by \ref operator->.
    using pointer = value_type const*;

    //! The iterator category.
    using iterator_category = std::input_iterator_tag;

    //////////////////////////////////////////////////////////////////////
    // Constructors
    //////////////////////////////////////////////////////////////////////

    //! Constructs an end iterator.
    KnuthBendixIterator();

    //! Default copy constructor.
    KnuthBendixIterator(KnuthBendixIterator const&) = default;

    //! Default move constructor.
    KnuthBendixIterator(KnuthBendixIterator&&) = default;

    //! Default copy assignment operator.
    KnuthBendixIterator& operator=(KnuthBendixIterator const&) = default;

    //! Default move assignment operator.
    KnuthBendixIterator& operator=(KnuthBendixIterator&&) = default;

    //! Default destructor.
    ~KnuthBendixIterator() = default;

    //! \brief Constructs an iterator over the complete rewriting systems of
    //! \p p.
    //!
    //! This function constructs an iterator over the complete rewriting systems
    //! of \p p.
    //!
    //! \param p the input presentation.
    //! \param max_depth the maximum number of rewriting steps permitted while
    //!    rewriting a word.
    //! \param max_queue_size the maximum number of rules permitted to be
    //!    stored while processing the presentation.
    KnuthBendixIterator(Presentation<native_word_type> const& p,
                        size_t                                max_depth,
                        size_t                                max_queue_size);

    //////////////////////////////////////////////////////////////////////
    // Public member functions
    //////////////////////////////////////////////////////////////////////

    //! \brief Tests whether two iterators are equal.
    //!
    //! This function tests whether two iterators are equal. This is a weak
    //! comparison that is mainly intended to check if an iterator is at the
    //! end. As a result, it is possible for iterators over different
    //! presentations to compare equal.
    //!
    //! \param that the iterator to compare with.
    //!
    //! \returns `true` if the iterators compare equal, and `false` otherwise.
    [[nodiscard]] bool
    operator==(KnuthBendixIterator const& that) const noexcept {
      return _rule_index == that._rule_index;
    }

    //! \brief Tests whether two iterators compare unequal.
    //!
    //! Tests whether two iterators compare unequal.
    //!
    //! \param that the iterator to compare with.
    //!
    //! \returns `true` if the iterators compare unequal, and `false`
    //! otherwise.
    [[nodiscard]] bool
    operator!=(KnuthBendixIterator const& that) const noexcept {
      return !(operator==(that));
    }

    //! \brief Returns the current confluent rewriting system.
    //!
    //! Returns the current confluent rewriting system, represented as an
    //! oriented presentation.
    //!
    //! \returns A const reference to the current presentation.
    [[nodiscard]] const_reference operator*() const noexcept {
      return _output_presentation;
    }

    //! \brief Returns a const pointer to the current confluent rewriting
    //! system.
    //!
    //! Returns a constpointer to the current confluent rewriting system.
    //!
    //! \returns A const pointer to the current presentation.
    [[nodiscard]] const_pointer operator->() const noexcept {
      return &(_output_presentation);
    }

    //! \brief Advances the iterator to the next confluent rewriting system.
    //!
    //! Advances the iterator to the next confluent rewriting system.
    //!
    //! \returns A reference to this iterator.
    KnuthBendixIterator& operator++();

    //! \brief Advances the iterator to the next confluent rewriting system.
    //!
    //! Advances the iterator to the next confluent rewriting system.
    //!
    //! \returns The previous iterator value.
    KnuthBendixIterator operator++(int) {
      return detail::default_postfix_increment<KnuthBendixIterator>(*this);
    }

   private:
    bool backtrack();

    bool rewrite_pair(native_word_type& lhs, native_word_type& rhs) const;

    bool add_pending_rule(native_word_type&& lhs, native_word_type&& rhs);

    bool process_single_overlap(native_word_type const&                u_lhs,
                                native_word_type const&                u_rhs,
                                native_word_type const&                v_lhs,
                                native_word_type const&                v_rhs,
                                native_word_type::const_iterator const it);

    bool process_subword_overlap(native_word_type const&                u_lhs,
                                 native_word_type const&                u_rhs,
                                 native_word_type const&                v_lhs,
                                 native_word_type const&                v_rhs,
                                 native_word_type::const_iterator const start);

    bool process_onesided_overlaps(native_word_type const& u_lhs,
                                   native_word_type const& u_rhs,
                                   native_word_type const& v_lhs,
                                   native_word_type const& v_rhs);

    // Find and process overlaps of the form
    // 1. U on the left:
    //    U = AB -> X
    //    V = BC -> Y
    //    XC <- ABC -> AY
    // 2. U on the right:
    //    U = BC -> X
    //    V = AB -> Y
    //    YB <- ABC -> AX
    // 3. U a subword:
    //    U = B -> X
    //    V = ABC -> Y
    //    Y <- ABC -> AXC
    // We don't need to check that V is a subword because U is already reduced
    // with respect to V.
    //
    // Return false if, at any stage, rewriting fails
    bool process_overlaps(native_word_type const& u_lhs,
                          native_word_type const& u_rhs,
                          native_word_type const& v_lhs,
                          native_word_type const& v_rhs);

    void set_output_presentation();

    size_t                           _max_queue_size;
    size_t                           _max_rewriting_depth;
    std::vector<detail::Orientation> _orientations;
    Presentation<native_word_type>   _output_presentation;
    size_t                           _rule_index;
    std::vector<std::pair<native_word_type, native_word_type>> _rules;
    detail::RewritingSystemBacktrack                           _rws;
    bool                                   _should_backtrack;
    std::vector<std::pair<size_t, size_t>> _state_history;
  };

  //! \relates KnuthBendixIterator
  //!
  //! \brief Returns an iterator to the first confluent rewriting system.
  //!
  //! This function returns an iterator to the first confluent rewriting system
  //! over \p p, with the constraints that no word is rewritten more than
  //! \p max_depth times, and that no more than \p max_queue_size rules are
  //! stored while processing the presentation.
  //!
  //! \param p the presentation to complete.
  //! \param max_depth the maximum number of rewriting steps performed while
  //! processing a word.
  //! \param max_queue_size the maximum number of rules stored while
  //! processing the presentation.
  //!
  //! \returns An iterator to the first completed presentation.
  inline KnuthBendixIterator begin_knuth_bendix_iterator(
      Presentation<KnuthBendixIterator::native_word_type> const& p,
      size_t                                                     max_depth,
      size_t max_queue_size) {
    return KnuthBendixIterator(p, max_depth, max_queue_size);
  }

  //! \relates KnuthBendixIterator
  //!
  //! \brief Returns the end iterator for the confluent rewriting systems.
  //!
  //! This function returns an end iterator for the confluent rewriting systems
  //! over \p p, with the constraints that no word is rewritten more than
  //! \p max_depth times, and that no more than \p max_queue_size rules are
  //! stored while processing the presentation.
  //!
  //! \param 1 the presentation passed to
  //! \ref begin_knuth_bendix_iterator.
  //! \param 1 the maximum rewriting depth passed to
  //! \ref begin_knuth_bendix_iterator.
  //! \param 1 the maximum queue size passed to
  //! \ref begin_knuth_bendix_iterator.
  //!
  //! \returns A default-constructed end iterator.
  inline KnuthBendixIterator end_knuth_bendix_iterator(
      Presentation<KnuthBendixIterator::native_word_type> const&,
      size_t,
      size_t) {
    return KnuthBendixIterator();
  }
}  // namespace libsemigroups

#endif  // LIBSEMIGROUPS_KNUTH_BENDIX_ITERATOR_HPP_
