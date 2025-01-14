//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2021-2024 James D. Mitchell + Maria Tsalakou
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

// This file contains an implementation of a generalised suffix tree, adapted
// from:
//
// https://cp-algorithms.com/string/suffix-tree-ukkonen.html

// TODO
// * there's an issue if you add a word containing a very large letter, then
// add more words so that the unique letter used is now in the word with the
// very large letter

#ifndef LIBSEMIGROUPS_UKKONEN_HPP_
#define LIBSEMIGROUPS_UKKONEN_HPP_

#ifndef PARSED_BY_DOXYGEN
#define NOT_PARSED_BY_DOXYGEN
#endif

#include <algorithm>  // for find_if, copy, max
#include <cstring>    // for size_t, strlen
#include <iterator>   // for distance
#include <map>        // for operator!=, map, _Rb_tree_iterator
#include <numeric>    // for accumulate
#include <stack>      // for stack
#include <string>     // for allocator, string, basic_string
#include <utility>    // for pair, make_pair
#include <vector>     // for vector

#include "constants.hpp"  // for operator==, operator!=, Max, UNDEFINED
#include "debug.hpp"      // for LIBSEMIGROUPS_ASSERT
#include "dot.hpp"        // for Dot
#include "exception.hpp"  // for LIBSEMIGROUPS_EXCEPTION
#include "types.hpp"      // for word_type, letter_type

#include "detail/fmt.hpp"     // for format, format_string
#include "detail/string.hpp"  // for maximum_common_prefix

namespace libsemigroups {

  //! \defgroup ukkonen_group Ukkonen
  //!
  //! Defined `ukkonen.hpp`.
  //!
  //! This page contains an overview of the functionality in `libsemigroups`
  //! for Ukkonen's algorithm for constructing a generalised suffix tree.
  //!
  //! Helper functions for Ukkonen objects are documented at
  //! \ref libsemigroups::ukkonen "Helper functions for Ukkonen".
  //!
  //!
  //! @{

  //! \brief For an implementation of %Ukkonen's algorithm.
  //!
  //! This class implements %Ukkonen's algorithm for constructing a generalised
  //! suffix tree consisting of \ref word_type.  The implementation in this
  //! class is based on:
  //!
  //! https://cp-algorithms.com/string/suffix-tree-ukkonen.html
  //!
  //! The suffix tree is updated when the member function \ref add_word is
  //! invoked. Every non-duplicate word added to the tree has a unique letter
  //! appended to the end. If a duplicate word is added, then the tree is not
  //! modified, but the \ref multiplicity of the word is increased.
  class Ukkonen {
    // Alias for index in _nodes
    using node_index_type = size_t;

    // Alias for index in side an edge
    using edge_index_type = size_t;

   public:
    //! \brief Alias for any letter that is added by Ukkonen (so that unique
    //! strings end in unique letters).
    using unique_letter_type = size_t;

    //! Alias for order that words were added.
    using word_index_type = size_t;

    //! Alias for `word_type` iterators.
    using const_iterator = typename word_type::const_iterator;

    //! Alias for an index between \ref begin and \ref end.
    using index_type = size_t;

    ////////////////////////////////////////////////////////////////////////
    // Ukkonen - inner classes - public
    ////////////////////////////////////////////////////////////////////////

    //! \ingroup ukkonen_group
    //!
    //! \brief The return type of \ref traverse.
    //!
    //! The return type of \ref traverse indicating the position reached by
    //! following a path in the tree.
    struct State {
      //! \brief The index in Ukkonen::nodes of the node at the end of the
      //! position reached.
      //!
      //! The index in Ukkonen::nodes of the node at the end of the
      //! position reached.
      node_index_type v;

      //! \brief The position in the edge leading to the node `v` reached.
      //!
      //! The position in the edge leading to the node `v` reached.
      edge_index_type pos;

      //! \brief Default constructor.
      //!
      //! Default constructor.
      State() = default;

      //! \brief Default copy constructor.
      //!
      //! Default copy constructor.
      State(State const&) = default;

      //! \brief Default move constructor.
      //!
      //! Default move constructor.
      State(State&&) = default;

      //! \brief Default copy assignment.
      //!
      //! Default copy assignment.
      State& operator=(State const&) = default;

      //! \brief Default move assignment.
      //!
      //! Default move assignment.
      State& operator=(State&&) = default;

      //! \brief Construct from index and position.
      //!
      //! Construct from index and position.
      //!
      //! \param vv the index of the node reached.
      //! \param ppos the position in the edge leading to \p vv.
      //!
      //! \exceptions
      //! \no_libsemigroups_except
      State(node_index_type vv, edge_index_type ppos) : v(vv), pos(ppos) {}

      //! \brief Compare states.
      //!
      //! Two states are equal if and only if their data members coincide.
      //!
      //! \param that the state to compare.
      //!
      //! \returns \c true if \p that is equal to \c this, and \c false
      //! otherwise.
      //!
      //! \exceptions
      //! \noexcept
      //!
      //! \complexity
      //! Constant.
      bool operator==(State const& that) const noexcept {
        return v == that.v && pos == that.pos;
      }
    };

    //! \ingroup ukkonen_group
    //!
    //! \brief The type of the nodes in the tree.
    //!
    //! The type of the nodes in the tree.
    struct Node {
      //! \brief The index of the first letter in the edge leading to the node.
      //!
      //! The index of the first letter in the edge leading to the node.
      index_type l;

      //! \brief The index of one past the last letter in the edge leading to
      //! the node.
      //!
      //! The index of one past the last letter in the edge leading to the node.
      index_type r;

      //! \brief The index of the parent node.
      //!
      //! The index of the parent node.
      node_index_type parent;

#ifdef NOT_PARSED_BY_DOXYGEN
      // No doc
      node_index_type link;
      // The next member is a weak indicator of whether or not the node
      // corresponds to a real suffix. If the value is true, then the node
      // corresponds to a real suffix. If the value is false, then the children
      // should be checked via is_real_suffix(Node const&) in Ukkonen.
      mutable bool is_real_suffix;
#endif

      //! \brief The children of the current node.
      //!
      //! The children of the current node.
      mutable std::map<letter_type, node_index_type> children;

      //! \brief Default copy constructor.
      //!
      //! Default copy constructor.
      Node(Node const&) = default;

      //! \brief Default move constructor.
      //!
      //! Default move constructor.
      Node(Node&&) = default;

      //! \brief Default copy assignment.
      //!
      //! Default copy assignment.
      Node& operator=(Node const&) = default;

      //! \brief Default move assignment.
      //!
      //! Default move assignment.
      Node& operator=(Node&&) = default;

      //! \brief Construct a node from left most index, right most index, and
      //! parent.
      //!
      //! Construct a node from left most index, right most index, and parent.
      //!
      //! \param l the left most index and value of the data member \p l
      //! (defaults to \c 0).
      //! \param r one after the right most index and value of the data member
      //! \p r (defaults to \c 0).
      //! \param parent of the node being constructed (defaults to \ref
      //! UNDEFINED).
      //!
      //! \exceptions
      //! \no_libsemigroups_except
      explicit Node(index_type      l      = 0,
                    index_type      r      = 0,
                    node_index_type parent = UNDEFINED);

      //! \brief The length of the edge leading into the current node.
      //!
      //! The length of the edge leading into the current node.
      //!
      //! \returns A value of type `size_t`.
      //!
      //! \exceptions
      //! \noexcept
      //!
      //! \complexity
      //! Constant.
      size_t length() const noexcept {
        return r - l;
      }

      //! \brief The index of the child node corresponding to a letter (if any).
      //!
      //! The index of the child node corresponding to a letter (if any).
      //!
      //! \param c the first letter in the edge of the node.
      //!
      //! \returns A reference to the index of the child node a value of type
      //! `size_t`.
      //!
      //! \exceptions
      //! \no_libsemigroups_except
      //!
      //! \complexity
      //! Logarithmic in the size of `children.size()`.
      node_index_type& child(letter_type c);

      //! \brief The index of the child node corresponding to a letter (if any).
      //!
      //! The index of the child node corresponding to a letter (if any).
      //!
      //! \param c the first letter in the edge of the node.
      //!
      //! \returns The index of the child node, a value of type
      //! `size_t`.
      //!
      //! \exceptions
      //! \no_libsemigroups_except
      //!
      //! \complexity
      //! Logarithmic in the size of `children.size()`.
      node_index_type child(letter_type c) const;

      //! \brief Returns \c true` if the node is a leaf and \c false if not.
      //!
      //! Returns \c true` if the node is a leaf and \c false if not.
      //!
      //! \returns A value of type `bool`.
      //!
      //! \exceptions
      //! \noexcept
      //!
      //! \complexity
      //! Constant.
      bool is_leaf() const noexcept {
        return children.empty();
      }

      //! \brief Returns \c true if the node is the root and \c false if not.
      //!
      //! Returns \c true if the node is the root and \c false if not.
      //!
      //! \returns A value of type `bool`.
      //!
      //! \exceptions
      //! \noexcept
      //!
      //! \complexity
      //! Constant.
      bool is_root() const noexcept {
        return parent == UNDEFINED;
      }
    };

   private:
    ////////////////////////////////////////////////////////////////////////
    // Ukkonen - private data
    ////////////////////////////////////////////////////////////////////////

    size_t                  _max_word_length;
    std::vector<size_t>     _multiplicity;
    unique_letter_type      _next_unique_letter;
    std::vector<Node>       _nodes;
    State                   _ptr;
    std::vector<index_type> _word_begin;
    std::vector<index_type> _word_index_lookup;
    word_type               _word;

   public:
    ////////////////////////////////////////////////////////////////////////
    // Ukkonen - constructors - public
    ////////////////////////////////////////////////////////////////////////

    //! \brief Default constructor.
    //!
    //! Constructs an empty generalised suffix tree.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    Ukkonen();

    //! \brief Initialize an existing Ukkonen object.
    //!
    //! This function puts an Ukkonen object back into the same state as if it
    //! had been newly default constructed.
    //!
    //! \returns A reference to \c *this.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \sa \ref Ukkonen()
    Ukkonen& init();

    //! \brief Default copy constructor.
    //!
    //! Default copy constructor.
    Ukkonen(Ukkonen const&) = default;

    //! \brief Default move constructor.
    //!
    //! Default move constructor.
    Ukkonen(Ukkonen&&) = default;

    //! \brief Default copy assignment.
    //!
    //! Default copy assignment.
    Ukkonen& operator=(Ukkonen const&) = default;

    //! \brief Default move assignment.
    //!
    //! Default move assignment.
    Ukkonen& operator=(Ukkonen&&) = default;

    ~Ukkonen();

    ////////////////////////////////////////////////////////////////////////
    // Ukkonen - initialisation - public
    ////////////////////////////////////////////////////////////////////////

    //! \brief Add a word to the suffix tree.
    //!
    //! Calling this function immediately invokes Ukkonen's algorithm to add
    //! the given word to the suffix tree (if it is not already contained in
    //! the tree). If an identical word is already in the tree, then this
    //! function does nothing except increase the multiplicity of that word.
    //! If `first == last`, then this function does nothing.
    //!
    //! \param first iterator pointing to the first letter of the word to add.
    //! \param last one beyond the last letter of the word to add.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Linear in the distance between `first` and `last`.
    //!
    //! \warning This function does no checks on its arguments whatsoever. In
    //! particular, if the word corresponding to \p first and \p last contains
    //! any of the unique letters appended to the end of any existing word in
    //! the tree, then bad things will happen.
    void add_word_no_checks(const_iterator first, const_iterator last);

    //! \brief Check and add a word to the suffix tree.
    //!
    //! This function does the same as \ref add_word_no_checks(const_iterator,
    //! const_iterator) after first checking that none of the letters in the
    //! word corresponding to \p first and \p last is equal to any of the
    //! existing unique letters.
    //!
    //! \throws LibsemigroupsException if
    //! `throw_if_contains_unique_letter(first, last)` throws.
    void add_word(const_iterator first, const_iterator last) {
      throw_if_contains_unique_letter(first, last);
      add_word_no_checks(first, last);
    }

    ////////////////////////////////////////////////////////////////////////
    // Ukkonen - attributes - public
    ////////////////////////////////////////////////////////////////////////

    //! \brief Returns the nodes in the suffix tree.
    //!
    //! Returns the nodes in the suffix tree.
    //!
    //! \returns A const reference to a std::vector<Node> of `Ukkonen::Node`
    //! objects.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    std::vector<Node> const& nodes() const noexcept {
      return _nodes;
    }

    //! \brief Returns the number of distinct non-empty words in the suffix
    //! tree.
    //!
    //! Returns the number of distinct non-empty words in the suffix tree. This
    //! is the number of distinct non-empty words added via \ref
    //! Ukkonen::add_word or \ref Ukkonen::add_word_no_checks.
    //!
    //! \returns A value of type `size_t`.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    size_t number_of_distinct_words() const noexcept {
      return -1 - _next_unique_letter;
    }

    //! \brief Returns the sum of the lengths of the distinct words in the
    //! suffix tree.
    //!
    //! Returns the sum of the lengths of the distinct words in the suffix tree.
    //!
    //! \returns A value of type `size_t`.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    size_t length_of_distinct_words() const noexcept {
      return _word.size() - number_of_distinct_words();
    }

    //! \brief Returns the sum of the lengths of all of the words in the suffix
    //! tree.
    //!
    //! Returns the sum of the lengths of all of the words in the suffix tree.
    //! This is the total length of all the words added to the suffix tree
    //! including duplicates, if any.
    //!
    //! \returns A value of type `size_t`.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! \f$O(n)\f$ where \f$n\f$ is the return value of \ref
    //! number_of_distinct_words.
    size_t length_of_words() const noexcept;

    //! \brief Returns the number of non-empty words in the suffix tree.
    //!
    //! Returns the number of non-empty words in the suffix tree. This is the
    //! number of all words added via \ref Ukkonen::add_word or
    //! \ref Ukkonen::add_word_no_checks including duplicates, if any.
    //!
    //! \returns A value of type `size_t`.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! \f$O(n)\f$ where \f$n\f$ is the return value of \ref
    //! number_of_distinct_words.
    size_t number_of_words() const noexcept {
      return std::accumulate(_multiplicity.cbegin(), _multiplicity.cend(), 0);
    }

    //! \brief Returns the maximum length of word in the suffix tree.
    //!
    //! Returns the maximum length of word in the suffix tree.
    //!
    //! \returns A value of type `size_t`.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    size_t max_word_length() const noexcept {
      return _max_word_length;
    }

    //! \brief Returns an iterator pointing to the first letter of the first
    //! word in the suffix tree.
    //!
    //! Returns an iterator pointing to the first letter of the first
    //! word in the suffix tree.
    //!
    //! \returns A value of type `const_iterator`.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    const_iterator begin() const noexcept {
      return _word.begin();
    }

    //! \brief Returns an iterator pointing to the first letter of the first
    //! word in the suffix tree.
    //!
    //! Returns an iterator pointing to the first letter of the first word in
    //! the suffix tree.
    //!
    //! \returns A value of type `const_iterator`.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    const_iterator cbegin() const noexcept {
      return _word.cbegin();
    }

    //! \brief Returns an iterator pointing one past the last letter of the last
    //! word in the suffix tree.
    //!
    //! Returns an iterator pointing one past the last letter of the last word
    //! in the suffix tree.
    //!
    //! \returns A value of type `const_iterator`.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    const_iterator end() const noexcept {
      return _word.end();
    }

    //! \brief Returns an iterator pointing one past the last letter of the last
    //! word in the suffix tree.
    //!
    //! Returns an iterator pointing one past the last letter of the last word
    //! in the suffix tree.
    //!
    //! \returns A value of type `const_iterator`.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    const_iterator cend() const noexcept {
      return _word.cend();
    }

    //! \brief Returns the index of the word corresponding to a node.
    //!
    //! This function returns the least non-negative integer `i` such that the
    //! node \p n corresponds to the `i`-th word added to the suffix tree.
    //!
    //! \param n the node.
    //!
    //! \returns A value of type `word_index_type`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Constant.
    //!
    //! \warning This function does no checks on its arguments whatsoever. In
    //! particular, if `n.parent == UNDEFINED` then bad things will happen.
    word_index_type word_index_no_checks(Node const& n) const {
      LIBSEMIGROUPS_ASSERT(n.parent != UNDEFINED);
      return word_index_no_checks(n.r - 1);
    }

    //! \brief Returns the index of the word corresponding to a node.
    //!
    //! This function returns the least non-negative integer `i` such that the
    //! node \p n corresponds to the `i`-th word added to the suffix tree.
    //!
    //! \param n the node.
    //!
    //! \returns A value of type `word_index_type`.
    //!
    //! \throws LibsemigroupsException if `n.parent == UNDEFINED`
    //!
    //! \complexity
    //! Constant.
    word_index_type word_index(Node const& n) const {
      if (n.parent == UNDEFINED) {
        LIBSEMIGROUPS_EXCEPTION(
            "expected the parent of the parameter to not be UNDEFINED");
      }
      return word_index_no_checks(n);
    }

    //! \brief Returns the index of the word corresponding to a position.
    //!
    //! This function returns the least non-negative integer \c j such that the
    //! `Ukkonen::begin() + i` points to a character in the `j`-th word added
    //! to the suffix tree.
    //!
    //! \param i the position.
    //!
    //! \returns A value of type `word_index_type`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Constant.
    //!
    //! \warning This function does no checks on its arguments whatsoever. In
    //! particular, if \p i is greater than \ref length_of_words + \ref
    //! number_of_distinct_words, then bad things will happen.
    word_index_type word_index_no_checks(index_type i) const {
      LIBSEMIGROUPS_ASSERT(i < _word.size());
      return _word_index_lookup[i];
    }

    //! \brief Returns the index of the word corresponding to a position.
    //!
    //! This function returns the least non-negative integer \c j such that the
    //! `Ukkonen::begin() + i` points to a character in the `j`-th word added
    //! to the suffix tree.
    //!
    //! \param i the position.
    //!
    //! \returns A value of type `word_index_type`.
    //!
    //! \throws LibsemigroupsException if \p i is greater than
    //! `length_of_distinct_words() + number_of_distinct_words()`
    //!
    //! \complexity
    //! Constant.
    word_index_type word_index(index_type i) const {
      if (i >= _word.size()) {
        LIBSEMIGROUPS_EXCEPTION(
            "expected the parameter to be in the range [0, {}), found {}",
            _word.size(),
            i);
      }
      return word_index_no_checks(i);
    }

    //! \brief Returns the distance of a node from the root.
    //!
    //! Returns the distance of a node from the root.
    //!
    //! \param n the node.
    //!
    //! \returns A value of type `size_t`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! At worst linear in the distance of the node \p n from the root.
    size_t distance_from_root(Node const& n) const;

    //! \brief Check if a state corresponds to a suffix.
    //!
    //! This function returns a `word_index_type` if the state \p st
    //! corresponds to a suffix of any word in the suffix tree. The value
    //! returned is the index of the word which the state is a suffix of.
    //!
    //! \param st the state.
    //!
    //! \returns A value of type `word_index_type`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    word_index_type is_suffix(State const& st) const;

    //! \brief Returns the multiplicity of a word by index.
    //!
    //! This function returns the number of times that the word corresponding to
    //! the index \p i was added to the suffix tree.
    //!
    //! \param i the node.
    //!
    //! \returns A value of type `size_t`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Constant.
    size_t multiplicity_no_checks(word_index_type i) const {
      return _multiplicity[i];
    }

    //! \brief Returns the multiplicity of a word by index.
    //!
    //! This function returns the number of times that the word corresponding to
    //! the index \p i was added to the suffix tree.
    //!
    //! \param i the node.
    //!
    //! \returns A value of type `size_t`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Constant.
    size_t multiplicity(word_index_type i) const {
      if (i >= _multiplicity.size()) {
        LIBSEMIGROUPS_EXCEPTION(
            "expected the parameter to be in the range [0, {}), found {}",
            _multiplicity.size(),
            i);
      }
      return multiplicity_no_checks(i);
    }

    //! \brief Returns the unique letter added to the end of a word in the
    //! suffix tree.
    //!
    //! Returns the unique letter added to the end of the \p i-th distinct word
    //! added to the suffix tree.
    //!
    //! \param i the index of an added word.
    //!
    //! \returns A value of type `unique_letter_type`.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    unique_letter_type unique_letter(word_index_type i) const noexcept {
      LIBSEMIGROUPS_ASSERT(i < number_of_distinct_words());
      return -1 - i;
    }

    //! \brief Check if a letter is a unique letter added to the end of a word
    //! in the suffix tree.
    //!
    //! Returns \c true if \p l is one of the unique letters added to the end
    //! of a word in the suffix tree.
    //!
    //! \param l the letter_type to check.
    //!
    //! \returns A value of type `bool`.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    bool is_unique_letter(letter_type l) const noexcept {
      return l >= _next_unique_letter;
    }

    //! \brief Find the index of a word in the suffix tree.
    //!
    //! If the word corresponding to \p first and \p last is one of the words
    //! that the suffix tree contains (the words added to the suffix tree via
    //! `add_word` or `add_word_no_checks`), then this function returns the
    //! index of that word. If the word corresponding to \p first and \p last
    //! is not one of the words that the suffix tree represents, then \ref
    //! UNDEFINED is returned.
    //!
    //! \tparam Iterator the type of the arguments.
    //! \param first iterator pointing to the first letter of the word to check.
    //! \param last one beyond the last letter of the word to check.
    //!
    //! \returns A value of type `word_index_type`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Linear in the distance from \p first to \p last.
    //!
    //! \warning This function does no checks on its arguments whatsoever. In
    //! particular, if the word corresponding to \p first and \p last contains
    //! any of the unique letters appended to the end of any existing word in
    //! the tree, then bad things will happen.
    template <typename Iterator>
    word_index_type index_no_checks(Iterator first, Iterator last) const;

    // Returns the index of the word [first, last) in the suffix tree if it is
    // contained (i.e. [first, last) is one of the words added using add_word),
    // and UNDEFINED if not.
    //! \brief \copybrief index_no_checks
    //!
    //! See \ref index_no_checks.
    //!
    //! \throws LibsemigroupsException if
    //! `throw_if_contains_unique_letter(first, last)` throws.
    template <typename Iterator>
    word_index_type index(Iterator first, Iterator last) const {
      throw_if_contains_unique_letter(first, last);
      return index_no_checks(first, last);
    }

    //! \brief Traverse the suffix tree from the root.
    //!
    //! This function traverses the edges in the suffix tree, starting at the
    //! state \p st, that are labelled by the letters in the word corresponding
    //! to \p first and \p last. The suffix tree is traversed until the end of
    //! the word is reached, or a letter not corresponding to an edge is
    //! encountered. The state \p st is modified in-place to contain the last
    //! state in the tree reached in the traversal. The returned iterator
    //! points one past the last letter consumed in the traversal.
    //!
    //! \tparam Iterator the type of the 2nd and 3rd arguments.
    //! \param st the initial state.
    //! \param first iterator pointing to the first letter of the word.
    //! \param last one beyond the last letter of the word.
    //!
    //! \returns A value of type `Iterator`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Linear in the distance from \p first to \p last.
    //!
    //! \warning This function does no checks on its arguments whatsoever. In
    //! particular, if the word corresponding to \p first and \p last contains
    //! any of the unique letters appended to the end of any existing word in
    //! the tree, then bad things will happen.
    template <typename Iterator>
    Iterator traverse_no_checks(State& st, Iterator first, Iterator last) const;

    //! \brief \copybrief traverse_no_checks(State&, Iterator, Iterator) const
    //!
    //! See \ref traverse_no_checks(State&, Iterator, Iterator) const.
    //!
    //! \throws LibsemigroupsException if
    //! `throw_if_contains_unique_letter(first, last)` throws.
    template <typename Iterator>
    Iterator traverse(State& st, Iterator first, Iterator last) const {
      throw_if_contains_unique_letter(first, last);
      return traverse_no_checks(st, first, last);
    }

    //! \brief Traverse the suffix tree from the root.
    //!
    //! This function traverses the edges in the suffix tree, starting at the
    //! root node, that are labelled by the letters in the word corresponding
    //! to \p first and \p last. The suffix tree is traversed until the end of
    //! the word is reached, or a letter not corresponding to an edge is
    //! encountered. A pair consisting of the state reached, and one past the
    //! last letter consumed in the traversal is returned.
    //!
    //! \tparam Iterator the type of the arguments.
    //! \param first iterator pointing to the first letter of the word.
    //! \param last one beyond the last letter of the word.
    //!
    //! \returns A value of type `std::pair<State, Iterator>`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Linear in the distance from \p first to \p last.
    //!
    //! \warning This function does no checks on its arguments whatsoever. In
    //! particular, if the word corresponding to \p first and \p last contains
    //! any of the unique letters appended to the end of any existing word in
    //! the tree, then bad things will happen.
    template <typename Iterator>
    std::pair<State, Iterator> traverse_no_checks(Iterator first,
                                                  Iterator last) const {
      State st(0, 0);
      return std::make_pair(st, traverse(st, first, last));
    }

    //! \brief \copybrief traverse_no_checks(Iterator, Iterator) const
    //!
    //! See \ref traverse_no_checks(Iterator, Iterator) const.
    //!
    //! \throws LibsemigroupsException if
    //! `throw_if_contains_unique_letter(first, last)` throws.
    template <typename Iterator>
    std::pair<State, Iterator> traverse(Iterator first, Iterator last) const {
      throw_if_contains_unique_letter(first, last);
      return traverse_no_checks(first, last);
    }

    ////////////////////////////////////////////////////////////////////////
    // Ukkonen - validation - public
    ////////////////////////////////////////////////////////////////////////

    //! \brief  Throw if the word `[first, last)` contains a letter equal to any
    //! of the unique letters added to the end of words in the suffix tree.
    //!
    //! This function throws an exception if the word corresponding to \p first
    //! and \p last contains a letter equal to any of the unique letters added
    //! to the end of words in the suffix tree.
    //!
    //! \tparam Iterator the type of the arguments.
    //! \param first iterator pointing to the first letter of the word.
    //! \param last one beyond the last letter of the word.
    //!
    //! \throws LibsemigroupsException if `is_unique_letter(*it)` returns \c
    //! true for any `it` in `[first, last)`.
    //!
    //! \complexity
    //! Linear in the distance from \p first to \p last.
    template <typename Iterator>
    void throw_if_contains_unique_letter(Iterator first, Iterator last) const;

    //! \brief  Throw if \p w contains a letter equal to any of the unique
    //! letters added to the end of words in the suffix tree.
    //!
    //! See \ref throw_if_contains_unique_letter.
    void throw_if_contains_unique_letter(word_type const& w) const {
      throw_if_contains_unique_letter(w.cbegin(), w.cend());
    }

   private:
    ////////////////////////////////////////////////////////////////////////
    // Ukkonen - helpers - private
    ////////////////////////////////////////////////////////////////////////

    bool is_real_suffix(Node const& n) const;

    size_t word_length(word_index_type index) const {
      LIBSEMIGROUPS_ASSERT(index < _word_begin.size() - 1);
      return (_word_begin[index + 1] - _word_begin[index]) - 1;
    }

    ////////////////////////////////////////////////////////////////////////
    // The following functions go, split, get_link, and tree_extend are
    // minimally adapted from:
    //
    // https://cp-algorithms.com/string/suffix-tree-ukkonen.html
    ////////////////////////////////////////////////////////////////////////

    // Follow the path in the tree starting at the position described by
    // State st, and corresponding to the range [l, r) in _word.
    void go(State& st, index_type l, index_type r) const;

    // Split the node _nodes[st.v] into two nodes, the new node
    // with edge corresponding to
    //
    // [_nodes[st.v].l, _nodes[st.v].l + st.pos)
    //
    // and the old node with edge corresponding to
    //
    // [_nodes[st.v].l + st.pos, _nodes[st.v].r)
    node_index_type split(State const& st);

    // Get the suffix link of a node by index
    node_index_type get_link(node_index_type v);

    // Perform the phase starting with the pos letter of the word.
    void tree_extend(index_type pos);
  };

  //! \brief Namespace for Ukkonen helper functions.
  //!
  //! This namespace contains helper functions for the Ukkonen class.
  namespace ukkonen {
    //! \brief \copybrief Ukkonen::add_word_no_checks
    //!
    //! See \ref Ukkonen::add_word_no_checks.
    inline void add_word_no_checks(Ukkonen& u, word_type const& w) {
      u.add_word_no_checks(w.cbegin(), w.cend());
    }

    //! \brief \copybrief Ukkonen::add_word_no_checks
    //!
    //! See \ref Ukkonen::add_word_no_checks.
    template <typename Iterator>
    void add_word_no_checks(Ukkonen& u, Iterator first, Iterator last) {
      // TODO(later) it'd be better to just convert the values pointed at by the
      // iterators, than to allocate a word_type here, but this is currently a
      // bit tricky to set up
      add_word_no_checks(u, word_type(first, last));
    }

    //! \brief \copybrief Ukkonen::add_word_no_checks
    //!
    //! See \ref Ukkonen::add_word_no_checks.
    template <typename Word>
    void add_word_no_checks(Ukkonen& u, Word const& w) {
      add_word_no_checks(u, w.cbegin(), w.cend());
    }

    //! \brief \copybrief Ukkonen::add_word_no_checks
    //!
    //! See \ref Ukkonen::add_word_no_checks.
    inline void add_word_no_checks(Ukkonen& u, char const* w) {
      add_word_no_checks(u, w, w + std::strlen(w));
    }

    //! \brief \copybrief Ukkonen::add_word_no_checks
    //!
    //! See \ref Ukkonen::add_word_no_checks.
    //!
    //! \throws LibsemigroupsException if `throw_if_contains_unique_letter(w)`
    //! throws.
    //!
    //! \sa \ref Ukkonen::throw_if_contains_unique_letter.
    inline void add_word(Ukkonen& u, word_type const& w) {
      u.add_word(w.cbegin(), w.cend());
    }

    //! \brief \copybrief Ukkonen::add_word_no_checks
    //!
    //! See \ref Ukkonen::add_word_no_checks.
    //!
    //! \throws LibsemigroupsException if
    //! `throw_if_contains_unique_letter(first, last)` throws.
    //!
    //! \sa \ref Ukkonen::throw_if_contains_unique_letter.
    template <typename Iterator>
    void add_word(Ukkonen& u, Iterator first, Iterator last) {
      // TODO(later) it'd be better to just convert the values pointed at by the
      // iterators, than to allocate a word_type here, but this is currently a
      // bit tricky to set up
      add_word(u, word_type(first, last));
    }

    //! \brief \copybrief Ukkonen::add_word_no_checks
    //!
    //! See \ref Ukkonen::add_word_no_checks.
    //!
    //! \throws LibsemigroupsException if `throw_if_contains_unique_letter(w)`
    //! throws.
    //!
    //! \sa \ref Ukkonen::throw_if_contains_unique_letter.
    template <typename Word>
    void add_word(Ukkonen& u, Word const& w) {
      add_word(u, w.cbegin(), w.cend());
    }

    //! \brief \copybrief Ukkonen::add_word_no_checks
    //!
    //! See \ref Ukkonen::add_word_no_checks.
    //!
    //! \throws LibsemigroupsException if `u.throw_if_contains_unique_letter(w,
    //! w + std::strlen(w))` throws.
    //!
    //! \sa \ref Ukkonen::throw_if_contains_unique_letter.
    inline void add_word(Ukkonen& u, char const* w) {
      add_word(u, w, w + std::strlen(w));
    }

    //! \brief Add all words in a std::vector to a Ukkonen object.
    //!
    //! Add all words in a std::vector to a Ukkonen object.
    //!
    //! \param u the Ukkonen object.
    //! \param words the words to add.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \warning This function does no checks on its arguments whatsoever. In
    //! particular, if the word corresponding to \p first and \p last contains
    //! any of the unique letters appended to the end of any existing word in
    //! the tree, then bad things will happen.
    void add_words_no_checks(Ukkonen& u, std::vector<word_type> const& words);

    //! \brief Add all words in a range to a Ukkonen object.
    //!
    //! Add all words in a range to a Ukkonen object.
    //!
    //! \tparam Iterator the type of the 2nd and 3rd parameters.
    //! \param u the Ukkonen object.
    //! \param first iterator pointing to the first letter of the word.
    //! \param last one beyond the last letter of the word.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \warning This function does no checks on its arguments whatsoever. In
    //! particular, if the word corresponding to \p first and \p last contains
    //! any of the unique letters appended to the end of any existing word in
    //! the tree, then bad things will happen.
    template <typename Iterator1, typename Iterator2>
    void add_words_no_checks(Ukkonen& u, Iterator1 first, Iterator2 last) {
      for (auto it = first; it != last; ++it) {
        add_word_no_checks(u, *it);
      }
    }

    //! \brief Add all words in a range to a Ukkonen object.
    //!
    //! See \ref add_words_no_checks(Ukkonen&, std::vector<word_type> const&).
    //!
    //! \throws LibsemigroupsException if `u.throw_if_contains_unique_letter(w)`
    //! throws for any `w` in \p words.
    //!
    //! \sa \ref Ukkonen::throw_if_contains_unique_letter.
    void add_words(Ukkonen& u, std::vector<word_type> const& words);

    //! \brief Add all words in a range to a Ukkonen object.
    //!
    //! See \ref add_words_no_checks(Ukkonen&, Iterator1, Iterator2).
    //!
    //! \throws LibsemigroupsException if `u.throw_if_contains_unique_letter(w)`
    //! throws for any `w` in `[first, last)`.
    //!
    //! \sa \ref Ukkonen::throw_if_contains_unique_letter.
    template <typename Iterator1, typename Iterator2>
    void add_words(Ukkonen& u, Iterator1 first, Iterator2 last) {
      for (auto it = first; it != last; ++it) {
        add_word(u, *it);
      }
    }

    //! \brief \copybrief Ukkonen::traverse_no_checks(Iterator, Iterator) const
    //!
    //! See \ref Ukkonen::traverse_no_checks(Iterator, Iterator) const.
    template <typename Word>
    auto traverse_no_checks(Ukkonen const& u, Word const& w) {
      return u.traverse_no_checks(w.cbegin(), w.cend());
    }

    //! \brief \copybrief Ukkonen::traverse_no_checks(Iterator, Iterator) const
    //!
    //! See \ref Ukkonen::traverse_no_checks(Iterator, Iterator) const.
    inline auto traverse_no_checks(Ukkonen const& u, word_type const& w) {
      return u.traverse_no_checks(w.cbegin(), w.cend());
    }

    //! \brief \copybrief Ukkonen::traverse_no_checks(Iterator, Iterator) const
    //!
    //! See \ref Ukkonen::traverse_no_checks(Iterator, Iterator) const.
    inline auto traverse_no_checks(Ukkonen const& u, char const* w) {
      return u.traverse_no_checks(w, w + std::strlen(w));
    }

    //! \brief \copybrief Ukkonen::traverse_no_checks(Iterator, Iterator) const
    //!
    //! See \ref Ukkonen::traverse_no_checks(Iterator, Iterator) const.
    //!
    //! \throws LibsemigroupsException if
    //! `u.throw_if_contains_unique_letter(w.cbegin(),  w.cend())` throws.
    //!
    //! \sa \ref Ukkonen::throw_if_contains_unique_letter.
    template <typename Word>
    auto traverse(Ukkonen const& u, Word const& w) {
      return u.traverse(w.cbegin(), w.cend());
    }

    //! \brief \copybrief Ukkonen::traverse_no_checks(Iterator, Iterator) const
    //!
    //! See \ref Ukkonen::traverse_no_checks(Iterator, Iterator) const.
    //!
    //! \throws LibsemigroupsException if
    //! `u.throw_if_contains_unique_letter(w.cbegin(),  w.cend())` throws.
    //!
    //! \sa \ref Ukkonen::throw_if_contains_unique_letter.
    inline auto traverse(Ukkonen const& u, word_type const& w) {
      return u.traverse(w.cbegin(), w.cend());
    }

    //! \brief \copybrief Ukkonen::traverse_no_checks(Iterator, Iterator) const
    //!
    //! See \ref Ukkonen::traverse_no_checks(Iterator, Iterator) const.
    //!
    //! \throws LibsemigroupsException if `u.throw_if_contains_unique_letter(w,
    //! w + std::strlen(w))` throws.
    //!
    //! \sa \ref Ukkonen::throw_if_contains_unique_letter.
    inline auto traverse(Ukkonen const& u, char const* w) {
      return u.traverse(w, w + std::strlen(w));
    }

    //! \brief \copybrief Ukkonen::traverse_no_checks(Ukkonen::State&, Iterator,
    //! Iterator) const
    //!
    //! See \ref Ukkonen::traverse_no_checks(Ukkonen::State&, Iterator,
    //! Iterator) const.
    template <typename Word>
    auto traverse_no_checks(Ukkonen::State& st,
                            Ukkonen const&  u,
                            Word const&     w) {
      return u.traverse_no_checks(st, w.cbegin(), w.cend());
    }

    //! \brief \copybrief Ukkonen::traverse_no_checks(Ukkonen::State&, Iterator,
    //! Iterator) const
    //!
    //! See \ref Ukkonen::traverse_no_checks(Ukkonen::State&, Iterator,
    //! Iterator) const.
    inline auto traverse_no_checks(Ukkonen::State&  st,
                                   Ukkonen const&   u,
                                   word_type const& w) {
      return u.traverse_no_checks(st, w.cbegin(), w.cend());
    }

    //! \brief \copybrief Ukkonen::traverse_no_checks(Ukkonen::State&, Iterator,
    //! Iterator) const
    //!
    //! See \ref Ukkonen::traverse_no_checks(Ukkonen::State&, Iterator,
    //! Iterator) const.
    inline auto traverse_no_checks(Ukkonen::State& st,
                                   Ukkonen const&  u,
                                   char const*     w) {
      return u.traverse_no_checks(st, w, w + std::strlen(w));
    }

    //! \brief \copybrief Ukkonen::traverse_no_checks(Ukkonen::State&, Iterator,
    //! Iterator) const
    //!
    //! See \ref Ukkonen::traverse_no_checks(Ukkonen::State&, Iterator,
    //! Iterator) const.
    //!
    //! \throws LibsemigroupsException if
    //! `u.throw_if_contains_unique_letter(w.cbegin(),  w.cend())` throws.
    //!
    //! \sa \ref Ukkonen::throw_if_contains_unique_letter.
    template <typename Word>
    auto traverse(Ukkonen::State& st, Ukkonen const& u, Word const& w) {
      return u.traverse(st, w.cbegin(), w.cend());
    }

    //! \brief \copybrief Ukkonen::traverse_no_checks(Ukkonen::State&, Iterator,
    //! Iterator) const
    //!
    //! See \ref Ukkonen::traverse_no_checks(Ukkonen::State&, Iterator,
    //! Iterator) const.
    //!
    //! \throws LibsemigroupsException if
    //! `u.throw_if_contains_unique_letter(w.cbegin(),  w.cend())` throws.
    //!
    //! \sa \ref Ukkonen::throw_if_contains_unique_letter.
    inline auto traverse(Ukkonen::State&  st,
                         Ukkonen const&   u,
                         word_type const& w) {
      return u.traverse(st, w.cbegin(), w.cend());
    }

    //! \brief \copybrief Ukkonen::traverse_no_checks(Ukkonen::State&, Iterator,
    //! Iterator) const
    //!
    //! See \ref Ukkonen::traverse_no_checks(Ukkonen::State&, Iterator,
    //! Iterator) const.
    //!
    //! \throws LibsemigroupsException if `u.throw_if_contains_unique_letter(w,
    //! w + std::strlen(w))` throws.
    //!
    //! \sa \ref Ukkonen::throw_if_contains_unique_letter.
    inline auto traverse(Ukkonen::State& st, Ukkonen const& u, char const* w) {
      return u.traverse(st, w, w + std::strlen(w));
    }

    //! \brief Check if a word is a subword of any word in a suffix tree.
    //!
    //! Returns \c true if the word corresponding to \p first and \p last is a
    //! subword of one of the words in the suffix tree represented by the
    //! Ukkonen instance \p u.
    //!
    //! \tparam Iterator the type of the 2nd and 3rd parameters.
    //! \param u the Ukkonen object.
    //! \param first iterator pointing to the first letter of the word.
    //! \param last one beyond the last letter of the word.
    //!
    //! \returns A value of type `bool`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Linear in the distance between `first` and `last`.
    //!
    //! \warning This function does no checks on its arguments whatsoever. In
    //! particular, if the word corresponding to \p first and \p last contains
    //! any of the unique letters appended to the end of any existing word in
    //! the tree, then bad things will happen.
    template <typename Iterator>
    bool is_subword_no_checks(Ukkonen const& u, Iterator first, Iterator last);

    //! \brief \copybrief is_subword_no_checks(Ukkonen const&, Iterator,
    //! Iterator)
    //!
    //! See \ref is_subword_no_checks(Ukkonen const&, Iterator, Iterator).
    template <typename Word>
    bool is_subword_no_checks(Ukkonen const& u, Word const& w) {
      return is_subword_no_checks(u, w.cbegin(), w.cend());
    }

    //! \brief \copybrief is_subword_no_checks(Ukkonen const&, Iterator,
    //! Iterator)
    //!
    //! See \ref is_subword_no_checks(Ukkonen const&, Iterator, Iterator).
    inline bool is_subword_no_checks(Ukkonen const& u, char const* w) {
      return is_subword_no_checks(u, w, w + std::strlen(w));
    }

    //! \brief \copybrief is_subword_no_checks(Ukkonen const&, Iterator,
    //! Iterator)
    //!
    //! See \ref is_subword_no_checks(Ukkonen const&, Iterator, Iterator).
    inline bool is_subword_no_checks(Ukkonen const& u, word_type const& w) {
      return is_subword_no_checks(u, w.cbegin(), w.cend());
    }

    //! \brief \copybrief is_subword_no_checks(Ukkonen const&, Iterator,
    //! Iterator)
    //!
    //! See \ref is_subword_no_checks(Ukkonen const&, Iterator, Iterator).
    //!
    //! \throws LibsemigroupsException if
    //! `u.throw_if_contains_unique_letter(first, last)` throws.
    //!
    //! \sa \ref Ukkonen::throw_if_contains_unique_letter.
    template <typename Iterator>
    bool is_subword(Ukkonen const& u, Iterator first, Iterator last) {
      u.throw_if_contains_unique_letter(first, last);
      return is_subword_no_checks(u, first, last);
    }

    //! \brief \copybrief is_subword_no_checks(Ukkonen const&, Iterator,
    //! Iterator)
    //!
    //! See \ref is_subword_no_checks(Ukkonen const&, Iterator, Iterator).
    //!
    //! \throws LibsemigroupsException if
    //! `u.throw_if_contains_unique_letter(w.cbegin(), w.cend())` throws.
    //!
    //! \sa \ref Ukkonen::throw_if_contains_unique_letter.
    template <typename Word>
    bool is_subword(Ukkonen const& u, Word const& w) {
      return is_subword(u, w.cbegin(), w.cend());
    }

    //! \brief \copybrief is_subword_no_checks(Ukkonen const&, Iterator,
    //! Iterator)
    //!
    //! See \ref is_subword_no_checks(Ukkonen const&, Iterator, Iterator).
    //!
    //! \throws LibsemigroupsException if `u.throw_if_contains_unique_letter(w,
    //! w + std::strlen(w))` throws.
    //!
    //! \sa \ref Ukkonen::throw_if_contains_unique_letter.
    inline bool is_subword(Ukkonen const& u, char const* w) {
      return is_subword(u, w, w + std::strlen(w));
    }

    //! \brief \copybrief is_subword_no_checks(Ukkonen const&, Iterator,
    //! Iterator)
    //!
    //! See \ref is_subword_no_checks(Ukkonen const&, Iterator, Iterator).
    //!
    //! \throws LibsemigroupsException if `u.throw_if_contains_unique_letter(w)`
    //! throws.
    //!
    //! \sa \ref Ukkonen::throw_if_contains_unique_letter.
    inline bool is_subword(Ukkonen const& u, word_type const& w) {
      return is_subword(u, w.cbegin(), w.cend());
    }

    //! \brief Check if a word is a suffix of any word in a suffix tree.
    //!
    //! Returns \c true if the word corresponding to \p first and \p last is a
    //! suffix of one of the words in the suffix tree represented by the
    //! Ukkonen instance \p u.
    //!
    //! \tparam Iterator the type of the 2nd and 3rd parameters.
    //! \param u the Ukkonen object.
    //! \param first iterator pointing to the first letter of the word.
    //! \param last one beyond the last letter of the word.
    //!
    //! \returns A value of type `bool`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Linear in the distance between `first` and `last`.
    //!
    //! \warning This function does no checks on its arguments whatsoever. In
    //! particular, if the word corresponding to \p first and \p last contains
    //! any of the unique letters appended to the end of any existing word in
    //! the tree, then bad things will happen.
    template <typename Iterator>
    bool is_suffix_no_checks(Ukkonen const& u, Iterator first, Iterator last);

    //! \brief \copybrief is_suffix_no_checks(Ukkonen const&, Iterator,
    //! Iterator)
    //!
    //! See \ref is_suffix_no_checks(Ukkonen const&, Iterator, Iterator).
    template <typename Word>
    bool is_suffix_no_checks(Ukkonen const& u, Word const& w) {
      return is_suffix_no_checks(u, w.cbegin(), w.cend());
    }

    //! \brief \copybrief is_suffix_no_checks(Ukkonen const&, Iterator,
    //! Iterator)
    //!
    //! See \ref is_suffix_no_checks(Ukkonen const&, Iterator, Iterator).
    // This function is required so that we can use initialiser list, as an
    // argument to is_suffix_no_checks
    inline bool is_suffix_no_checks(Ukkonen const& u, word_type const& w) {
      return is_suffix_no_checks(u, w.cbegin(), w.cend());
    }

    //! \brief \copybrief is_suffix_no_checks(Ukkonen const&, Iterator,
    //! Iterator)
    //!
    //! See \ref is_suffix_no_checks(Ukkonen const&, Iterator, Iterator).
    inline bool is_suffix_no_checks(Ukkonen const& u, char const* w) {
      return is_suffix_no_checks(u, w, w + std::strlen(w));
    }

    //! \brief \copybrief is_suffix_no_checks(Ukkonen const&, Iterator,
    //! Iterator)
    //!
    //! See \ref is_suffix_no_checks(Ukkonen const&, Iterator, Iterator).
    //!
    //! \throws LibsemigroupsException if
    //! `u.throw_if_contains_unique_letter(first, last)` throws.
    //!
    //! \sa \ref Ukkonen::throw_if_contains_unique_letter.
    template <typename Iterator>
    bool is_suffix(Ukkonen const& u, Iterator first, Iterator last) {
      u.throw_if_contains_unique_letter(first, last);
      return is_suffix_no_checks(u, first, last);
    }

    //! \brief \copybrief is_suffix_no_checks(Ukkonen const&, Iterator,
    //! Iterator)
    //!
    //! See \ref is_suffix_no_checks(Ukkonen const&, Iterator, Iterator).
    //!
    //! \throws LibsemigroupsException if
    //! `u.throw_if_contains_unique_letter(w.begin(), w.end())` throws.
    //!
    //! \sa \ref Ukkonen::throw_if_contains_unique_letter.
    template <typename Word>
    bool is_suffix(Ukkonen const& u, Word const& w) {
      return is_suffix(u, w.cbegin(), w.cend());
    }

    //! \brief \copybrief is_suffix_no_checks(Ukkonen const&, Iterator,
    //! Iterator)
    //!
    //! See \ref is_suffix_no_checks(Ukkonen const&, Iterator, Iterator).
    //!
    //! \throws LibsemigroupsException if `u.throw_if_contains_unique_letter(w)`
    //! throws.
    //!
    //! \sa \ref Ukkonen::throw_if_contains_unique_letter.
    // This function is required so that we can use initialiser list, as an
    // argument to is_suffix
    inline bool is_suffix(Ukkonen const& u, word_type const& w) {
      return is_suffix(u, w.cbegin(), w.cend());
    }

    //! \brief \copybrief is_suffix_no_checks(Ukkonen const&, Iterator,
    //! Iterator)
    //!
    //! See \ref is_suffix_no_checks(Ukkonen const&, Iterator, Iterator).
    //!
    //! \throws LibsemigroupsException if `u.throw_if_contains_unique_letter(w,
    //! w + std::strlen(w))` throws.
    //!
    //! \sa \ref Ukkonen::throw_if_contains_unique_letter.
    inline bool is_suffix(Ukkonen const& u, char const* w) {
      return is_suffix(u, w, w + std::strlen(w));
    }

    //! \brief Find the maximal prefix of a word occurring in two different
    //! places in a word in a suffix tree.
    //!
    //! Returns an iterator pointing one past the last letter in the maximal
    //! length prefix of the word corresponding to \p first and \p last that
    //! occurs in at least \f$2\f$ different (possibly overlapping) places in
    //! the words contained in \p u. If no such prefix exists, then `first` is
    //! returned.
    //!
    //! \tparam Iterator the type of the 2nd and 3rd parameters.
    //! \param u the Ukkonen object.
    //! \param first iterator pointing to the first letter of the word.
    //! \param last one beyond the last letter of the word.
    //!
    //! \returns A value of type `Iterator`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Linear in the distance between `first` and `last`.
    //!
    //! \warning This function does no checks on its arguments whatsoever. In
    //! particular, if the word corresponding to \p first and \p last contains
    //! any of the unique letters appended to the end of any existing word in
    //! the tree, then bad things will happen.
    template <typename Iterator>
    Iterator maximal_piece_prefix_no_checks(Ukkonen const& u,
                                            Iterator       first,
                                            Iterator       last);

    //! \brief \copybrief maximal_piece_prefix_no_checks(Ukkonen const&,
    //! Iterator, Iterator)
    //!
    //! See \ref maximal_piece_prefix_no_checks(Ukkonen const&, Iterator,
    //! Iterator).
    template <typename Word>
    typename Word::const_iterator
    maximal_piece_prefix_no_checks(Ukkonen const& u, Word const& w) {
      return maximal_piece_prefix_no_checks(u, w.cbegin(), w.cend());
    }

    //! \brief \copybrief maximal_piece_prefix_no_checks(Ukkonen const&,
    //! Iterator, Iterator)
    //!
    //! See \ref maximal_piece_prefix_no_checks(Ukkonen const&, Iterator,
    //! Iterator).
    inline typename word_type::const_iterator
    maximal_piece_prefix_no_checks(Ukkonen const& u, word_type const& w) {
      return maximal_piece_prefix_no_checks(u, w.cbegin(), w.cend());
    }

    //! \brief \copybrief maximal_piece_prefix_no_checks(Ukkonen const&,
    //! Iterator, Iterator)
    //!
    //! See \ref maximal_piece_prefix_no_checks(Ukkonen const&, Iterator,
    //! Iterator).
    inline char const* maximal_piece_prefix_no_checks(Ukkonen const& u,
                                                      char const*    w) {
      return maximal_piece_prefix_no_checks(u, w, w + std::strlen(w));
    }

    //! \brief \copybrief maximal_piece_prefix_no_checks(Ukkonen const&,
    //! Iterator, Iterator)
    //!
    //! See \ref maximal_piece_prefix_no_checks(Ukkonen const&, Iterator,
    //! Iterator).
    //!
    //! \throws LibsemigroupsException if
    //! `u.throw_if_contains_unique_letter(first, last)` throws.
    //!
    //! \sa \ref Ukkonen::throw_if_contains_unique_letter.
    template <typename Iterator>
    Iterator maximal_piece_prefix(Ukkonen const& u,
                                  Iterator       first,
                                  Iterator       last) {
      u.throw_if_contains_unique_letter(first, last);
      return maximal_piece_prefix_no_checks(u, first, last);
    }

    //! \brief \copybrief maximal_piece_prefix_no_checks(Ukkonen const&,
    //! Iterator, Iterator)
    //!
    //! See \ref maximal_piece_prefix_no_checks(Ukkonen const&, Iterator,
    //! Iterator).
    //!
    //! \throws LibsemigroupsException if
    //! `u.throw_if_contains_unique_letter(w.cbegin(), w.cend())` throws.
    //!
    //! \sa \ref Ukkonen::throw_if_contains_unique_letter.
    template <typename Word>
    typename Word::const_iterator maximal_piece_prefix(Ukkonen const& u,
                                                       Word const&    w) {
      return maximal_piece_prefix(u, w.cbegin(), w.cend());
    }

    //! \brief \copybrief maximal_piece_prefix_no_checks(Ukkonen const&,
    //! Iterator, Iterator)
    //!
    //! See \ref maximal_piece_prefix_no_checks(Ukkonen const&, Iterator,
    //! Iterator).
    //!
    //! \throws LibsemigroupsException if `u.throw_if_contains_unique_letter(w)`
    //! throws.
    //!
    //! \sa \ref Ukkonen::throw_if_contains_unique_letter.
    inline typename word_type::const_iterator
    maximal_piece_prefix(Ukkonen const& u, word_type const& w) {
      return maximal_piece_prefix(u, w.cbegin(), w.cend());
    }

    //! \brief \copybrief maximal_piece_prefix_no_checks(Ukkonen const&,
    //! Iterator, Iterator)
    //!
    //! See \ref maximal_piece_prefix_no_checks(Ukkonen const&, Iterator,
    //! Iterator).
    //!
    //! \throws LibsemigroupsException if `u.throw_if_contains_unique_letter(w,
    //! w + std::strlen(w))` throws.
    //!
    //! \sa \ref Ukkonen::throw_if_contains_unique_letter.
    inline char const* maximal_piece_prefix(Ukkonen const& u, char const* w) {
      return maximal_piece_prefix(u, w, w + std::strlen(w));
    }

    //! \brief Find the length of the maximal prefix of a word occurring in two
    //! different places in a word in a suffix tree.
    //!
    //! Returns the length of the maximal length prefix of the word
    //! corresponding to \p first and \p last that occurs in at least \f$2\f$
    //! different (possibly overlapping) places in the words contained in \p u.
    //! If no such prefix exists, then `0` is returned.
    //!
    //! \tparam Iterator the type of the 2nd and 3rd parameters.
    //! \param u the Ukkonen object.
    //! \param first iterator pointing to the first letter of the word.
    //! \param last one beyond the last letter of the word.
    //!
    //! \returns A value of type `Iterator`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Linear in the distance between `first` and `last`.
    //!
    //! \warning This function does no checks on its arguments whatsoever. In
    //! particular, if the word corresponding to \p first and \p last contains
    //! any of the unique letters appended to the end of any existing word in
    //! the tree, then bad things will happen.
    template <typename Iterator>
    size_t length_maximal_piece_prefix_no_checks(Ukkonen const& u,
                                                 Iterator       first,
                                                 Iterator       last) {
      return std::distance(first,
                           maximal_piece_prefix_no_checks(u, first, last));
    }

    //! \brief \copybrief length_maximal_piece_prefix_no_checks(Ukkonen const&,
    //! Iterator, Iterator)
    //!
    //! See \ref length_maximal_piece_prefix_no_checks(Ukkonen const&, Iterator,
    //! Iterator).
    template <typename Word>
    size_t length_maximal_piece_prefix_no_checks(Ukkonen const& u,
                                                 Word const&    w) {
      return length_maximal_piece_prefix_no_checks(u, w.cbegin(), w.cend());
    }

    //! \brief \copybrief length_maximal_piece_prefix_no_checks(Ukkonen const&,
    //! Iterator, Iterator)
    //!
    //! See \ref length_maximal_piece_prefix_no_checks(Ukkonen const&, Iterator,
    //! Iterator).
    inline size_t length_maximal_piece_prefix_no_checks(Ukkonen const&   u,
                                                        word_type const& w) {
      return length_maximal_piece_prefix_no_checks(u, w.cbegin(), w.cend());
    }

    //! \brief \copybrief length_maximal_piece_prefix_no_checks(Ukkonen const&,
    //! Iterator, Iterator)
    //!
    //! See \ref length_maximal_piece_prefix_no_checks(Ukkonen const&, Iterator,
    //! Iterator).
    inline size_t length_maximal_piece_prefix_no_checks(Ukkonen const& u,
                                                        char const*    w) {
      return length_maximal_piece_prefix_no_checks(u, w, w + std::strlen(w));
    }

    //! \brief \copybrief length_maximal_piece_prefix_no_checks(Ukkonen const&,
    //! Iterator, Iterator)
    //!
    //! See \ref length_maximal_piece_prefix_no_checks(Ukkonen const&, Iterator,
    //! Iterator).
    //!
    //! \throws LibsemigroupsException if
    //! `u.throw_if_contains_unique_letter(first, last)` throws.
    //!
    //! \sa \ref Ukkonen::throw_if_contains_unique_letter.
    template <typename Iterator>
    size_t length_maximal_piece_prefix(Ukkonen const& u,
                                       Iterator       first,
                                       Iterator       last) {
      return std::distance(first, maximal_piece_prefix(u, first, last));
    }

    //! \brief \copybrief length_maximal_piece_prefix_no_checks(Ukkonen const&,
    //! Iterator, Iterator)
    //!
    //! See \ref length_maximal_piece_prefix_no_checks(Ukkonen const&, Iterator,
    //! Iterator).
    //!
    //! \throws LibsemigroupsException if
    //! `u.throw_if_contains_unique_letter(w.cbegin(), w.cend())` throws.
    //!
    //! \sa \ref Ukkonen::throw_if_contains_unique_letter.
    template <typename Word>
    size_t length_maximal_piece_prefix(Ukkonen const& u, Word const& w) {
      return length_maximal_piece_prefix(u, w.cbegin(), w.cend());
    }

    //! \brief \copybrief length_maximal_piece_prefix_no_checks(Ukkonen const&,
    //! Iterator, Iterator)
    //!
    //! See \ref length_maximal_piece_prefix_no_checks(Ukkonen const&, Iterator,
    //! Iterator).
    //!
    //! \throws LibsemigroupsException if `u.throw_if_contains_unique_letter(w)`
    //! throws.
    //!
    //! \sa \ref Ukkonen::throw_if_contains_unique_letter.
    inline size_t length_maximal_piece_prefix(Ukkonen const&   u,
                                              word_type const& w) {
      return length_maximal_piece_prefix(u, w.cbegin(), w.cend());
    }

    //! \brief \copybrief length_maximal_piece_prefix_no_checks(Ukkonen const&,
    //! Iterator, Iterator)
    //!
    //! See \ref length_maximal_piece_prefix_no_checks(Ukkonen const&, Iterator,
    //! Iterator).
    //!
    //! \throws LibsemigroupsException if `u.throw_if_contains_unique_letter(w,
    //! w + std::strlen(w))` throws.
    //!
    //! \sa \ref Ukkonen::throw_if_contains_unique_letter.
    inline size_t length_maximal_piece_prefix(Ukkonen const& u, char const* w) {
      return length_maximal_piece_prefix(u, w, w + std::strlen(w));
    }

    //! \brief Check if a word is a piece (occurs in two distinct places in the
    //! words of the suffix tree).
    //!
    //! Returns \c true if the word corresponding to \p first and \p last that
    //! occurs in at least \f$2\f$ different (possibly overlapping) places in
    //! the words contained in \p u. If no such prefix exists, then `false` is
    //! returned.
    //!
    //! \tparam Iterator the type of the 2nd and 3rd parameters.
    //! \param u the Ukkonen object.
    //! \param first iterator pointing to the first letter of the word.
    //! \param last one beyond the last letter of the word.
    //!
    //! \returns A value of type `bool`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Linear in the distance between `first` and `last`.
    //!
    //! \warning This function does no checks on its arguments whatsoever. In
    //! particular, if the word corresponding to \p first and \p last contains
    //! any of the unique letters appended to the end of any existing word in
    //! the tree, then bad things will happen.
    template <typename Iterator>
    bool is_piece_no_checks(Ukkonen const& u, Iterator first, Iterator last) {
      return maximal_piece_prefix_no_checks(u, first, last) == last;
    }

    //! \brief \copybrief is_piece_no_checks(Ukkonen const&, Iterator, Iterator)
    //!
    //! See \ref is_piece_no_checks(Ukkonen const&, Iterator, Iterator).
    template <typename Word>
    bool is_piece_no_checks(Ukkonen const& u, Word const& w) {
      return is_piece_no_checks(u, w.cbegin(), w.cend());
    }
    //! \brief \copybrief is_piece_no_checks(Ukkonen const&, Iterator, Iterator)
    //!
    //! See \ref is_piece_no_checks(Ukkonen const&, Iterator, Iterator).
    inline bool is_piece_no_checks(Ukkonen const& u, word_type const& w) {
      return is_piece_no_checks(u, w.cbegin(), w.cend());
    }

    //! \brief \copybrief is_piece_no_checks(Ukkonen const&, Iterator, Iterator)
    //!
    //! See \ref is_piece_no_checks(Ukkonen const&, Iterator, Iterator).
    inline bool is_piece_no_checks(Ukkonen const& u, char const* w) {
      return is_piece_no_checks(u, w, w + std::strlen(w));
    }

    //! \brief \copybrief is_piece_no_checks(Ukkonen const&, Iterator, Iterator)
    //!
    //! See \ref is_piece_no_checks(Ukkonen const&, Iterator, Iterator).
    //!
    //! \throws LibsemigroupsException if
    //! `u.throw_if_contains_unique_letter(first, last)` throws.
    //!
    //! \sa \ref Ukkonen::throw_if_contains_unique_letter.
    template <typename Iterator>
    bool is_piece(Ukkonen const& u, Iterator first, Iterator last) {
      return maximal_piece_prefix(u, first, last) == last;
    }

    //! \brief \copybrief is_piece_no_checks(Ukkonen const&, Iterator, Iterator)
    //!
    //! See \ref is_piece_no_checks(Ukkonen const&, Iterator, Iterator).
    //!
    //! \throws LibsemigroupsException if
    //! `u.throw_if_contains_unique_letter(w.cbegin(), w.cend())` throws.
    //!
    //! \sa \ref Ukkonen::throw_if_contains_unique_letter.
    template <typename Word>
    bool is_piece(Ukkonen const& u, Word const& w) {
      return is_piece(u, w.cbegin(), w.cend());
    }

    //! \brief \copybrief is_piece_no_checks(Ukkonen const&, Iterator, Iterator)
    //!
    //! See \ref is_piece_no_checks(Ukkonen const&, Iterator, Iterator).
    //!
    //! \throws LibsemigroupsException if `u.throw_if_contains_unique_letter(w)`
    //! throws.
    //!
    //! \sa \ref Ukkonen::throw_if_contains_unique_letter.
    inline bool is_piece(Ukkonen const& u, word_type const& w) {
      return is_piece(u, w.cbegin(), w.cend());
    }

    //! \brief \copybrief is_piece_no_checks(Ukkonen const&, Iterator, Iterator)
    //!
    //! See \ref is_piece_no_checks(Ukkonen const&, Iterator, Iterator).
    //!
    //! \throws LibsemigroupsException if `u.throw_if_contains_unique_letter(w,
    //! w + std::strlen(w))` throws.
    //!
    //! \sa \ref Ukkonen::throw_if_contains_unique_letter.
    inline bool is_piece(Ukkonen const& u, char const* w) {
      return is_piece(u, w, w + std::strlen(w));
    }

    //! \brief Find the maximal suffix of a word occurring in two different
    //! places in a word in a suffix tree.
    //!
    //! Returns an iterator pointing at the first letter in the maximal
    //! length suffix of the word corresponding to \p first and \p last that
    //! occurs in at least \f$2\f$ different (possibly overlapping) places in
    //! the words contained in \p u. If no such suffix exists, then `first` is
    //! returned.
    //!
    //! \tparam Iterator the type of the 2nd and 3rd parameters.
    //! \param u the Ukkonen object.
    //! \param first iterator pointing to the first letter of the word.
    //! \param last one beyond the last letter of the word.
    //!
    //! \returns A value of type `Iterator`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! At worst \f$O(m ^ 2)\f$ or \f$O(n)\f$ where \f$m\f$ is the distance
    //! between `first` and `last` and \f$n\f$ is the return value of
    //! Ukkonen::length_of_distinct_words.
    //!
    //! \warning This function does no checks on its arguments whatsoever. In
    //! particular, if the word corresponding to \p first and \p last contains
    //! any of the unique letters appended to the end of any existing word in
    //! the tree, then bad things will happen.
    template <typename Iterator>
    Iterator maximal_piece_suffix_no_checks(Ukkonen const& u,
                                            Iterator       first,
                                            Iterator       last);

    //! \brief \copybrief maximal_piece_suffix_no_checks(Ukkonen const&,
    //! Iterator, Iterator)
    //!
    //! See \ref maximal_piece_suffix_no_checks(Ukkonen const&, Iterator,
    //! Iterator).
    template <typename Word>
    typename Word::const_iterator
    maximal_piece_suffix_no_checks(Ukkonen const& u, Word const& w) {
      return maximal_piece_suffix_no_checks(u, w.cbegin(), w.cend());
    }

    //! \brief \copybrief maximal_piece_suffix_no_checks(Ukkonen const&,
    //! Iterator, Iterator)
    //!
    //! See \ref maximal_piece_suffix_no_checks(Ukkonen const&, Iterator,
    //! Iterator).
    inline typename word_type::const_iterator
    maximal_piece_suffix_no_checks(Ukkonen const& u, word_type const& w) {
      return maximal_piece_suffix_no_checks(u, w.cbegin(), w.cend());
    }

    //! \brief \copybrief maximal_piece_suffix_no_checks(Ukkonen const&,
    //! Iterator, Iterator)
    //!
    //! See \ref maximal_piece_suffix_no_checks(Ukkonen const&, Iterator,
    //! Iterator).
    inline char const* maximal_piece_suffix_no_checks(Ukkonen const& u,
                                                      char const*    w) {
      return maximal_piece_suffix_no_checks(u, w, w + std::strlen(w));
    }

    //! \brief \copybrief maximal_piece_suffix_no_checks(Ukkonen const&,
    //! Iterator, Iterator)
    //!
    //! See \ref maximal_piece_suffix_no_checks(Ukkonen const&, Iterator,
    //! Iterator).
    //!
    //! \throws LibsemigroupsException if
    //! `u.throw_if_contains_unique_letter(first, last)` throws.
    //!
    //! \sa \ref Ukkonen::throw_if_contains_unique_letter.
    template <typename Iterator>
    Iterator maximal_piece_suffix(Ukkonen const& u,
                                  Iterator       first,
                                  Iterator       last) {
      u.throw_if_contains_unique_letter(first, last);
      return maximal_piece_suffix_no_checks(u, first, last);
    }

    //! \brief \copybrief maximal_piece_suffix_no_checks(Ukkonen const&,
    //! Iterator, Iterator)
    //!
    //! See \ref maximal_piece_suffix_no_checks(Ukkonen const&, Iterator,
    //! Iterator).
    //!
    //! \throws LibsemigroupsException if
    //! `u.throw_if_contains_unique_letter(w.cbegin(), w.cend())` throws.
    //!
    //! \sa \ref Ukkonen::throw_if_contains_unique_letter.
    template <typename Word>
    typename Word::const_iterator maximal_piece_suffix(Ukkonen const& u,
                                                       Word const&    w) {
      return maximal_piece_suffix(u, w.cbegin(), w.cend());
    }

    //! \brief \copybrief maximal_piece_suffix_no_checks(Ukkonen const&,
    //! Iterator, Iterator)
    //!
    //! See \ref maximal_piece_suffix_no_checks(Ukkonen const&, Iterator,
    //! Iterator).
    //!
    //! \throws LibsemigroupsException if `u.throw_if_contains_unique_letter(w)`
    //! throws.
    //!
    //! \sa \ref Ukkonen::throw_if_contains_unique_letter.
    inline typename word_type::const_iterator
    maximal_piece_suffix(Ukkonen const& u, word_type const& w) {
      return maximal_piece_suffix(u, w.cbegin(), w.cend());
    }

    //! \brief \copybrief maximal_piece_suffix_no_checks(Ukkonen const&,
    //! Iterator, Iterator)
    //!
    //! See \ref maximal_piece_suffix_no_checks(Ukkonen const&, Iterator,
    //! Iterator).
    //!
    //! \throws LibsemigroupsException if `u.throw_if_contains_unique_letter(w,
    //! w + std::strlen(w))` throws.
    //!
    //! \sa \ref Ukkonen::throw_if_contains_unique_letter.
    inline char const* maximal_piece_suffix(Ukkonen const& u, char const* w) {
      return maximal_piece_suffix(u, w, w + std::strlen(w));
    }

    //! \brief Find the length of the maximal suffix of a word occurring in two
    //! different places in a word in a suffix tree.
    //!
    //! Returns the length of the maximal length prefix of the word
    //! corresponding to \p first and \p last that occurs in at least \f$2\f$
    //! different (possibly overlapping) places in the words contained in \p u.
    //! If no such prefix exists, then `0` is returned.
    //!
    //! \tparam Iterator the type of the 2nd and 3rd parameters.
    //! \param u the Ukkonen object.
    //! \param first iterator pointing to the first letter of the word.
    //! \param last one beyond the last letter of the word.
    //!
    //! \returns A value of type `Iterator`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Linear in the distance between `first` and `last`.
    //!
    //! \warning This function does no checks on its arguments whatsoever. In
    //! particular, if the word corresponding to \p first and \p last contains
    //! any of the unique letters appended to the end of any existing word in
    //! the tree, then bad things will happen.
    template <typename Iterator>
    size_t length_maximal_piece_suffix_no_checks(Ukkonen const& u,
                                                 Iterator       first,
                                                 Iterator       last) {
      return std::distance(maximal_piece_suffix_no_checks(u, first, last),
                           last);
    }

    //! \brief \copybrief length_maximal_piece_suffix_no_checks(Ukkonen const&,
    //! Iterator, Iterator)
    //!
    //! See \ref length_maximal_piece_suffix_no_checks(Ukkonen const&, Iterator,
    //! Iterator).
    template <typename Word>
    size_t length_maximal_piece_suffix_no_checks(Ukkonen const& u,
                                                 Word const&    w) {
      return length_maximal_piece_suffix_no_checks(u, w.cbegin(), w.cend());
    }

    //! \brief \copybrief length_maximal_piece_suffix_no_checks(Ukkonen const&,
    //! Iterator, Iterator)
    //!
    //! See \ref length_maximal_piece_suffix_no_checks(Ukkonen const&, Iterator,
    //! Iterator).
    inline size_t length_maximal_piece_suffix_no_checks(Ukkonen const&   u,
                                                        word_type const& w) {
      return length_maximal_piece_suffix_no_checks(u, w.cbegin(), w.cend());
    }

    //! \brief \copybrief length_maximal_piece_suffix_no_checks(Ukkonen const&,
    //! Iterator, Iterator)
    //!
    //! See \ref length_maximal_piece_suffix_no_checks(Ukkonen const&, Iterator,
    //! Iterator).
    inline size_t length_maximal_piece_suffix_no_checks(Ukkonen const& u,
                                                        char const*    w) {
      return length_maximal_piece_suffix_no_checks(u, w, w + std::strlen(w));
    }

    //! \brief \copybrief length_maximal_piece_suffix_no_checks(Ukkonen const&,
    //! Iterator, Iterator)
    //!
    //! See \ref length_maximal_piece_suffix_no_checks(Ukkonen const&, Iterator,
    //! Iterator).
    //!
    //! \throws LibsemigroupsException if
    //! `u.throw_if_contains_unique_letter(first, last)` throws.
    //!
    //! \sa \ref Ukkonen::throw_if_contains_unique_letter.
    template <typename Iterator>
    size_t length_maximal_piece_suffix(Ukkonen const& u,
                                       Iterator       first,
                                       Iterator       last) {
      return std::distance(maximal_piece_suffix(u, first, last), last);
    }

    //! \brief \copybrief length_maximal_piece_suffix_no_checks(Ukkonen const&,
    //! Iterator, Iterator)
    //!
    //! See \ref length_maximal_piece_suffix_no_checks(Ukkonen const&, Iterator,
    //! Iterator).
    //!
    //! \throws LibsemigroupsException if
    //! `u.throw_if_contains_unique_letter(w.cbegin(), w.cend())` throws.
    //!
    //! \sa \ref Ukkonen::throw_if_contains_unique_letter.
    template <typename Word>
    size_t length_maximal_piece_suffix(Ukkonen const& u, Word const& w) {
      return length_maximal_piece_suffix(u, w.cbegin(), w.cend());
    }

    //! \brief \copybrief length_maximal_piece_suffix_no_checks(Ukkonen const&,
    //! Iterator, Iterator)
    //!
    //! See \ref length_maximal_piece_suffix_no_checks(Ukkonen const&, Iterator,
    //! Iterator).
    //!
    //! \throws LibsemigroupsException if `u.throw_if_contains_unique_letter(w)`
    //! throws.
    //!
    //! \sa \ref Ukkonen::throw_if_contains_unique_letter.
    inline size_t length_maximal_piece_suffix(Ukkonen const&   u,
                                              word_type const& w) {
      return length_maximal_piece_suffix(u, w.cbegin(), w.cend());
    }

    //! \brief \copybrief length_maximal_piece_suffix_no_checks(Ukkonen const&,
    //! Iterator, Iterator)
    //!
    //! See \ref length_maximal_piece_suffix_no_checks(Ukkonen const&, Iterator,
    //! Iterator).
    //!
    //! \throws LibsemigroupsException if `u.throw_if_contains_unique_letter(w,
    //! w + std::strlen(w))` throws.
    //!
    //! \sa \ref Ukkonen::throw_if_contains_unique_letter.
    inline size_t length_maximal_piece_suffix(Ukkonen const& u, char const* w) {
      return length_maximal_piece_suffix(u, w, w + std::strlen(w));
    }

    //! \brief Find the number of pieces in a decomposition of a word (if any).
    //!
    //! Returns minimum number of pieces whose product equals the word
    //! corresponding to \p first and \p last if such a product exists, and \c
    //! 0 if no such product exists. Recall that a *piece* is a word that
    //! occurs in two distinct positions (possibly overlapping) of the words in
    //! the suffix tree \p u.
    //!
    //! \tparam Iterator the type of the 2nd and 3rd parameters.
    //! \param u the Ukkonen object.
    //! \param first iterator pointing to the first letter of the word.
    //! \param last one beyond the last letter of the word.
    //!
    //! \returns A value of type `size_t`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Linear in the distance between `first` and `last`.
    //!
    //! \warning This function does no checks on its arguments whatsoever. In
    //! particular, if the word corresponding to \p first and \p last contains
    //! any of the unique letters appended to the end of any existing word in
    //! the tree, then bad things will happen.
    template <typename Iterator>
    size_t number_of_pieces_no_checks(Ukkonen const& u,
                                      Iterator       first,
                                      Iterator       last);

    //! \brief \copybrief number_of_pieces_no_checks(Ukkonen const&, Iterator,
    //! Iterator)
    //!
    //! See \ref number_of_pieces_no_checks(Ukkonen const&, Iterator,
    //! Iterator).
    template <typename Word>
    size_t number_of_pieces_no_checks(Ukkonen const& u, Word const& w) {
      return number_of_pieces_no_checks(u, w.cbegin(), w.cend());
    }

    //! \brief \copybrief number_of_pieces_no_checks(Ukkonen const&, Iterator,
    //! Iterator)
    //!
    //! See \ref number_of_pieces_no_checks(Ukkonen const&, Iterator,
    //! Iterator).
    inline size_t number_of_pieces_no_checks(Ukkonen const&   u,
                                             word_type const& w) {
      return number_of_pieces_no_checks(u, w.cbegin(), w.cend());
    }

    //! \brief \copybrief number_of_pieces_no_checks(Ukkonen const&, Iterator,
    //! Iterator)
    //!
    //! See \ref number_of_pieces_no_checks(Ukkonen const&, Iterator,
    //! Iterator).
    inline size_t number_of_pieces_no_checks(Ukkonen const& u, char const* w) {
      return number_of_pieces_no_checks(u, w, w + std::strlen(w));
    }

    //! \brief \copybrief number_of_pieces_no_checks(Ukkonen const&, Iterator,
    //! Iterator)
    //!
    //! See \ref number_of_pieces_no_checks(Ukkonen const&, Iterator,
    //! Iterator).
    //!
    //! \throws LibsemigroupsException if
    //! `u.throw_if_contains_unique_letter(first, last)` throws.
    //!
    //! \sa \ref Ukkonen::throw_if_contains_unique_letter.
    template <typename Iterator>
    size_t number_of_pieces(Ukkonen const& u, Iterator first, Iterator last) {
      u.throw_if_contains_unique_letter(first, last);
      return number_of_pieces_no_checks(u, first, last);
    }

    //! \brief \copybrief number_of_pieces_no_checks(Ukkonen const&, Iterator,
    //! Iterator)
    //!
    //! See \ref number_of_pieces_no_checks(Ukkonen const&, Iterator,
    //! Iterator).
    //!
    //! \throws LibsemigroupsException if
    //! `u.throw_if_contains_unique_letter(w.cbegin(), w.cend())` throws.
    //!
    //! \sa \ref Ukkonen::throw_if_contains_unique_letter.
    template <typename Word>
    size_t number_of_pieces(Ukkonen const& u, Word const& w) {
      return number_of_pieces(u, w.cbegin(), w.cend());
    }

    //! \brief \copybrief number_of_pieces_no_checks(Ukkonen const&, Iterator,
    //! Iterator)
    //!
    //! See \ref number_of_pieces_no_checks(Ukkonen const&, Iterator,
    //! Iterator).
    //!
    //! \throws LibsemigroupsException if `u.throw_if_contains_unique_letter(w)`
    //! throws.
    //!
    //! \sa \ref Ukkonen::throw_if_contains_unique_letter.
    inline size_t number_of_pieces(Ukkonen const& u, word_type const& w) {
      return number_of_pieces(u, w.cbegin(), w.cend());
    }

    //! \brief \copybrief number_of_pieces_no_checks(Ukkonen const&, Iterator,
    //! Iterator)
    //!
    //! See \ref number_of_pieces_no_checks(Ukkonen const&, Iterator,
    //! Iterator).
    //!
    //! \throws LibsemigroupsException if `u.throw_if_contains_unique_letter(w,
    //! w + std::strlen(w))` throws.
    //!
    //! \sa \ref Ukkonen::throw_if_contains_unique_letter.
    inline size_t number_of_pieces(Ukkonen const& u, char const* w) {
      return number_of_pieces(u, w, w + std::strlen(w));
    }

    //! \brief Returns the number of distinct subwords of the words in a suffix
    //! tree.
    //!
    //! Returns the total number of distinct subwords of the words in the suffix
    //! tree \p u.
    //!
    //! \param u the Ukkonen object.
    //!
    //! \returns A value of type `size_t`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Linear in `Ukkonen::length_of_distinct_words`.
    //!
    size_t number_of_distinct_subwords(Ukkonen const& u);

    //! \brief Find the pieces in a decomposition of a word (if any).
    //!
    //! Returns a std::vector of iterators pointing one past the last letter in
    //! the pieces whose product equals the word corresponding to \p first and
    //! \p last if such a product exists, and an empty std::vector if no such
    //! product exists. Recall that a *piece* is a word that occurs in two
    //! distinct positions (possibly overlapping) of the words in the suffix
    //! tree \p u.
    //!
    //! \tparam Iterator the type of the 2nd and 3rd parameters.
    //! \param u the Ukkonen object.
    //! \param first iterator pointing to the first letter of the word.
    //! \param last one beyond the last letter of the word.
    //!
    //! \returns A value of type `std::vector<Iterator>`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Linear in the distance between `first` and `last`.
    //!
    //! \warning This function does no checks on its arguments whatsoever. In
    //! particular, if the word corresponding to \p first and \p last contains
    //! any of the unique letters appended to the end of any existing word in
    //! the tree, then bad things will happen.
    template <typename Iterator>
    std::vector<Iterator> pieces_no_checks(Ukkonen const& u,
                                           Iterator       first,
                                           Iterator       last);

    //! \brief \copybrief pieces_no_checks(Ukkonen const&, Iterator, Iterator)
    //!
    //! See \ref pieces_no_checks(Ukkonen const&, Iterator, Iterator).
    template <typename Word>
    std::vector<Word> pieces_no_checks(Ukkonen const& u, Word const& w);

    //! \brief \copybrief pieces_no_checks(Ukkonen const&, Iterator, Iterator)
    //!
    //! See \ref pieces_no_checks(Ukkonen const&, Iterator, Iterator).
    std::vector<word_type> pieces_no_checks(Ukkonen const&   u,
                                            word_type const& w);

    //! \brief \copybrief pieces_no_checks(Ukkonen const&, Iterator, Iterator)
    //!
    //! See \ref pieces_no_checks(Ukkonen const&, Iterator, Iterator).
    std::vector<std::string> pieces_no_checks(Ukkonen const& u, char const* w);

    //! \brief \copybrief pieces_no_checks(Ukkonen const&, Iterator, Iterator)
    //!
    //! See \ref pieces_no_checks(Ukkonen const&, Iterator, Iterator).
    //!
    //! \throws LibsemigroupsException if
    //! `u.throw_if_contains_unique_letter(first, last)` throws.
    //!
    //! \sa \ref Ukkonen::throw_if_contains_unique_letter.
    template <typename Iterator>
    std::vector<Iterator> pieces(Ukkonen const& u,
                                 Iterator       first,
                                 Iterator       last) {
      u.throw_if_contains_unique_letter(first, last);
      return pieces_no_checks(u, first, last);
    }

    //! \brief \copybrief pieces_no_checks(Ukkonen const&, Iterator, Iterator)
    //!
    //! See \ref pieces_no_checks(Ukkonen const&, Iterator, Iterator).
    //!
    //! \throws LibsemigroupsException if
    //! `u.throw_if_contains_unique_letter(w.cbegin(), w.cend())` throws.
    //!
    //! \sa \ref Ukkonen::throw_if_contains_unique_letter.
    template <typename Word>
    std::vector<Word> pieces(Ukkonen const& u, Word const& w) {
      u.throw_if_contains_unique_letter(w.cbegin(), w.cend());
      return pieces_no_checks(u, w);
    }

    //! \brief \copybrief pieces_no_checks(Ukkonen const&, Iterator, Iterator)
    //!
    //! See \ref pieces_no_checks(Ukkonen const&, Iterator, Iterator).
    //!
    //! \throws LibsemigroupsException if `u.throw_if_contains_unique_letter(w)`
    //! throws.
    //!
    //! \sa \ref Ukkonen::throw_if_contains_unique_letter.
    inline std::vector<word_type> pieces(Ukkonen const& u, word_type const& w) {
      u.throw_if_contains_unique_letter(w);
      return pieces_no_checks(u, w);
    }

    //! \brief \copybrief pieces_no_checks(Ukkonen const&, Iterator, Iterator)
    //!
    //! See \ref pieces_no_checks(Ukkonen const&, Iterator, Iterator).
    //!
    //! \throws LibsemigroupsException if `u.throw_if_contains_unique_letter(w,
    //! w + std::strlen(w))` throws.
    inline std::vector<std::string> pieces(Ukkonen const& u, char const* w) {
      u.throw_if_contains_unique_letter(w, w + std::strlen(w));
      return pieces_no_checks(u, w);
    }

    //! \brief Returns a \ref Dot object representing a suffix tree.
    //!
    //! This function returns a \ref Dot object representing the suffix tree
    //! defined by \p u.
    //!
    //! Internally, all words added to the suffix tree are stored as a single
    //! string delimited by unique letters. The edge labels present in this \ref
    //! Dot object correspond to intervals of letters in that delimited string.
    //!
    //! \param u the Ukkonen object.
    //!
    //! \returns A \ref Dot object.
    //!
    //! \throws LibsemigroupsException if \p u does not contain any words.
    //! \throws LibsemigroupsException if the number of words in \p u is greater
    //! than 24.
    [[nodiscard]] Dot dot(Ukkonen const& u);

    //! \brief Perform a depth first search in a suffix tree.
    //!
    //! This function can be used to perform a depth first search in the suffix
    //! tree \p u. The 2nd parameter is a helper object that must implement:
    //!
    //! * A function `void pre_order(Ukkonen const& u, size_t i)`;
    //! * A function `void post_order(Ukkonen const& u, size_t i)`; and
    //! * A function `auto yield(Ukkonen const& u)`.
    //!
    //! The function `T::pre_order` is called when the node \c n with index \c i
    //! is first encountered in the depth-first search, and the function
    //! `T::post_order` is called when the subtree rooted at \c n has been
    //! completely explored.
    //!
    //! The function `yield` is called at the end of the depth-first search and
    //! its return value is returned by this function.
    //!
    //! \tparam T the type of the helper object.
    //! \param u the Ukkonen object.
    //! \param helper the helper object.
    //!
    //! \returns A value whose type is the same as the return type of
    //! `T::yield`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    template <typename T>
    auto dfs(Ukkonen const& u, T& helper);

    namespace detail {
      // TODO when the old ToddCoxeter is removed move this into
      // presentation.hpp
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
  }    // namespace ukkonen

  //! \brief Return a human readable representation of an Ukkonen object.
  //!
  //! Return a human readable representation of an Ukkonen object.
  //!
  //! \param u the Ukkonen object.
  //!
  //! \returns A value of type std::string
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  [[nodiscard]] inline std::string to_human_readable_repr(Ukkonen const& u) {
    return fmt::format("<Ukkonen with {} distinct words>",
                       u.number_of_distinct_words());
  }

  //! \brief Return a human readable representation of an Ukkonen::State object.
  //!
  //! Return a human readable representation of an Ukkonen::State object.
  //!
  //! \param st the Ukkonen::State object.
  //! \param sep the string used to separate "Ukkonen" and "State" in the output
  //! (defaults to "::").
  //!
  //! \returns A value of type std::string
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  [[nodiscard]] inline std::string
  to_human_readable_repr(Ukkonen::State const& st,
                         std::string const&    sep = "::") {
    return fmt::format(
        "<Ukkonen{}State with pos = {} and v = {}>", sep, st.pos, st.v);
  }

  //! \brief Return a human readable representation of an Ukkonen::Node object.
  //!
  //! Return a human readable representation of an Ukkonen::Node object.
  //!
  //! \param node the Ukkonen::Node object.
  //! \param sep the string used to separate "Ukkonen" and "Node" in the output
  //! (defaults to "::").
  //!
  //! \returns A value of type std::string
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  [[nodiscard]] inline std::string
  to_human_readable_repr(Ukkonen::Node const& node,
                         std::string const&   sep = "::") {
    return fmt::format(
        "<Ukkonen{}Node with {} children and parent edge label [{}, {})>",
        sep,
        node.children.size(),
        node.l,
        node.r);
  }

  //! @}

}  // namespace libsemigroups

#include "ukkonen.tpp"

#endif  // LIBSEMIGROUPS_UKKONEN_HPP_
