//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2021-23 James D. Mitchell + Maria Tsalakou
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

#include <algorithm>  // for equal, find_if
#include <cstddef>    // for size_t
#include <cstdint>    // for uint64_t
#include <cstring>    // for strlen
#include <iterator>   // for operator+
#include <map>        // for map
#include <numeric>    // for accumulate
#include <stack>      // for stack
#include <string>     // for string
#include <utility>    // for pair, make_pair
#include <vector>     // for vector

#include "constants.hpp"  // for Undefined, UNDE...
#include "debug.hpp"      // for LIBSEMIGROUPS_ASSERT
#include "exception.hpp"  // for LIBSEMIGROUPS_EXCEPTION
#include "string.hpp"     // for maximum_common_prefix
#include "types.hpp"      // for word_type, lett..

namespace libsemigroups {
  //! Defined in ``ukkonen.hpp``.
  //!
  //! This class implements Ukkonen's algorithm for constructing a generalised
  //! suffix tree consisting of \ref word_type.  The implementation in this
  //! class is based on:
  //!
  //! https://cp-algorithms.com/string/suffix-tree-ukkonen.html
  //!
  //! The suffix tree is updated when the member function ``add_word`` is
  //! invoked. Every non-duplicate word added to the tree has a unique letter
  //! appended to the end. If a duplicate word is added, then the tree is not
  //! modified, but the ``multiplicity`` of the word is increased.
  //!
  //! Many helper functions are provided in the ``ukkonen`` namespace.
  class Ukkonen {
    // Alias for index in _nodes
    using node_index_type = size_t;

    // Alias for index in side an edge
    using edge_index_type = size_t;

   public:
    //! Alias for any letter that is added by Ukkonen (so that unique
    //! strings end in unique letters).
    using unique_letter_type = size_t;

    //! Alias for order that words were added.
    using word_index_type = size_t;

    //! Alias for `word_type` iterators.
    using const_iterator = typename word_type::const_iterator;

    // Alias for an index between \ref begin and \ref end.
    using index_type = size_t;

    ////////////////////////////////////////////////////////////////////////
    // Ukkonen - inner classes - public
    ////////////////////////////////////////////////////////////////////////

    //! The return type of \ref traverse indicating the position reached by
    //! following a path in the tree.
    struct State {
      //! The index in Ukkonen::nodes of the node at the end of the position
      //! reached.
      node_index_type v;
      //! The position in the edge leading to the node `v` reached.
      edge_index_type pos;

      //! Default constructor.
      State() = default;
      //! Default constructor.
      State(State const&) = default;
      //! Default constructor.
      State(State&&) = default;
      //! Default constructor.
      State& operator=(State const&) = default;
      //! Default constructor.
      State& operator=(State&&) = default;

      //! Construct from index and position.
      //!
      //! \param vv the index of the node reached
      //! \param ppos the position in the edge leading to \p vv
      //!
      //! \exceptions
      //! \no_libsemigroups_except
      State(node_index_type vv, edge_index_type ppos) : v(vv), pos(ppos) {}

      //! Compare states.
      //!
      //! Two states are equal if and only if their data members coincide.
      //!
      //! \param that the state to compare.
      //!
      //! \exceptions
      //! \noexcept
      //!
      //! \returns A value of type `bool`.
      bool operator==(State const& that) const noexcept {
        return v == that.v && pos == that.pos;
      }
    };

    //! The type of the nodes in the tree.
    struct Node {
      //! The index of the first letter in the edge leading to the node.
      index_type l;

      //! The index of one past the last letter in the edge leading to the node.
      index_type r;

      //! The index of the parent node.
      node_index_type parent;

#ifndef DOXYGEN_SHOULD_SKIP_THIS
      // No doc
      node_index_type link;
      // The next member is a weak indicator of whether or not the node
      // corresponds to a real suffix. If the value is true, then the node
      // corresponds to a real suffix. If the value is false, then the children
      // should be checked via is_real_suffix(Node const&) in Ukkonen.
      mutable bool is_real_suffix;
#endif

      //! The children of the current node.
      mutable std::map<letter_type, node_index_type> children;

      //! Default constructor.
      Node(Node const&) = default;

      //! Default constructor.
      Node(Node&&) = default;

      //! Default constructor.
      Node& operator=(Node const&) = default;

      //! Default constructor.
      Node& operator=(Node&&) = default;

      //! Construct a node from left most index, right most index, and parent.
      //!
      //! \param l the left most index and value of the data member \p l
      //! (defaults to \c 0)
      //! \param r one after the right most index and value of the data member
      //! \p r (defaults to \c 0)
      //! \param parent of the node being constructed (defaults to \ref
      //! UNDEFINED).
      //!
      //! \exceptions
      //! \no_libsemigroups_except
      explicit Node(index_type      l      = 0,
                    index_type      r      = 0,
                    node_index_type parent = UNDEFINED);

      //! The length of the edge leading into the current node.
      //!
      //! \parameters (None)
      //!
      //! \returns A value of type `size_t`.
      //!
      //! \exceptions
      //! \noexcept
      size_t length() const noexcept {
        return r - l;
      }

      //! The index of the child node corresponding to a letter (if any).
      //!
      //! \param c the first letter in the edge of the node.
      //!
      //! \returns A reference to the index of the child node a value of type
      //! `size_t`.
      //!
      //! \exceptions
      //! \no_libsemigroups_except
      node_index_type& child(letter_type c);

      //! The index of the child node corresponding to a letter (if any).
      //!
      //! \param c the first letter in the edge of the node.
      //!
      //! \returns The index of the child node, a value of type
      //! `size_t`.
      //!
      //! \exceptions
      //! \no_libsemigroups_except
      node_index_type child(letter_type c) const;

      //! Returns `true` if the node is a leaf and `false` if not.
      //!
      //! \parameters (None)
      //!
      //! \returns A value of type `bool`.
      //!
      //! \exceptions
      //! \noexcept
      bool is_leaf() const noexcept {
        return children.empty();
      }

      //! Returns `true` if the node is the root and `false` if not.
      //!
      //! \parameters (None)
      //!
      //! \returns A value of type `bool`.
      //!
      //! \exceptions
      //! \noexcept
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

    //! Default constructor.
    //!
    //! Constructs an empty generalised suffix tree.
    //!
    //! \parameters (None)
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    Ukkonen();

    // TODO(doc)
    Ukkonen& init();

    //! Default constructor.
    Ukkonen(Ukkonen const&) = default;

    //! Default constructor.
    Ukkonen(Ukkonen&&) = default;

    //! Default constructor.
    Ukkonen& operator=(Ukkonen const&) = default;

    //! Default constructor.
    Ukkonen& operator=(Ukkonen&&) = default;

    ////////////////////////////////////////////////////////////////////////
    // Ukkonen - initialisation - public
    ////////////////////////////////////////////////////////////////////////

    //! Add a word to the suffix tree.
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
    //! \returns (None)
    //!
    //! \warning This function does no checks on its arguments whatsoever. In
    //! particular, if the word corresponding to \p first and \p last contains
    //! any of the unique letters appended to the end of any existing word in
    //! the tree, then bad things will happen.
    void add_word_no_checks(const_iterator first, const_iterator last);

    //! Check and add a word to the suffix tree.
    //!
    //! This function does the same as \ref add_word_no_checks(const_iterator,
    //! const_iterator) after first checking that none of the letters in the
    //! word corresponding to \p first and \p last is equal to any of the
    //! existing unique letters.
    //!
    //! \throws LibsemigroupsException if `validate_word(first, last)` throws.
    void add_word(const_iterator first, const_iterator last) {
      validate_word(first, last);
      add_word_no_checks(first, last);
    }

    //! See \ref add_word_no_checks(const_iterator, const_iterator)
    void add_word_no_checks(word_type const& w) {
      add_word_no_checks(w.cbegin(), w.cend());
    }

    //! See \ref add_word_no_checks(const_iterator, const_iterator)
    //!
    //! \throws LibsemigroupsException if `validate_word(w)` throws.
    void add_word(word_type const& w) {
      add_word(w.cbegin(), w.cend());
    }

    //! See \ref add_word_no_checks(const_iterator, const_iterator)
    template <typename Iterator>
    void add_word_no_checks(Iterator first, Iterator last) {
      // TODO(later) it'd be better to just convert the values pointed at by the
      // iterators, than to allocate a word_type here, but this is currently a
      // bit tricky to set up
      add_word_no_checks(word_type(first, last));
    }

    //! See \ref add_word_no_checks(const_iterator, const_iterator)
    template <typename Word>
    void add_word_no_checks(Word const& w) {
      add_word_no_checks(w.cbegin(), w.cend());
    }

    //! See \ref add_word_no_checks(const_iterator, const_iterator)
    void add_word_no_checks(char const* w) {
      add_word_no_checks(w, w + std::strlen(w));
    }

    //! See \ref add_word_no_checks(const_iterator, const_iterator)
    //!
    //! \throws LibsemigroupsException if `validate_word(first, last)` throws.
    template <typename Iterator>
    void add_word(Iterator first, Iterator last) {
      // TODO(later) it'd be better to just convert the values pointed at by the
      // iterators, than to allocate a word_type here, but this is currently a
      // bit tricky to set up
      add_word(word_type(first, last));
    }

    //! See \ref add_word_no_checks(const_iterator, const_iterator)
    //!
    //! \throws LibsemigroupsException if `validate_word(w)` throws.
    template <typename Word>
    void add_word(Word const& w) {
      add_word(w.cbegin(), w.cend());
    }

    //! See \ref add_word_no_checks(const_iterator, const_iterator)
    //!
    //! \throws LibsemigroupsException if `Ukkonen::validate_word(w, w +
    //! std::strlen(w))` throws.
    void add_word(char const* w) {
      add_word(w, w + std::strlen(w));
    }

    ////////////////////////////////////////////////////////////////////////
    // Ukkonen - attributes - public
    ////////////////////////////////////////////////////////////////////////

    //! Returns the nodes in the suffix tree.
    //!
    //! \parameters (None)
    //!
    //! \returns A const reference to a \vector of `Ukkonen::Node` objects.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    std::vector<Node> const& nodes() const noexcept {
      return _nodes;
    }

    //! Returns the number of distinct non-empty words in the suffix tree.
    //!
    //! This is the number of distinct non-empty words added via
    //! Ukkonen::add_word or Ukkonen::add_word_no_checks.
    //!
    //! \parameters (None)
    //!
    //! \returns A value of type `size_t`
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    size_t number_of_distinct_words() const noexcept {
      return -1 - _next_unique_letter;
    }

    //! Returns the sum of the lengths of the distinct words in the suffix tree.
    //!
    //! \parameters (None)
    //!
    //! \returns A value of type `size_t`
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    size_t length_of_distinct_words() const noexcept {
      return _word.size() - number_of_distinct_words();
    }

    //! Returns the sum of the lengths of all of the words in the suffix tree.
    //!
    //! This is the total length of all the words added to the suffix tree
    //! including duplicates, if any.
    //!
    //! \parameters (None)
    //!
    //! \returns A value of type `size_t`
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! \f$O(n)\f$ where \f$n\f$ is the return value of \ref
    //! number_of_distinct_words.
    size_t length_of_words() const noexcept;

    //! Returns the number of non-empty words in the suffix tree.
    //!
    //! This is the number of all words added via Ukkonen::add_word or
    //! Ukkonen::add_word_no_checks including duplicates, if any.
    //!
    //! \parameters (None)
    //!
    //! \returns A value of type `size_t`
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

    //! Returns the maximum length of word in the suffix tree.
    //!
    //! \parameters (None)
    //!
    //! \returns A value of type `size_t`
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    size_t max_word_length() const noexcept {
      return _max_word_length;
    }

    //! Returns an iterator pointing to the first letter of the first word in
    //! the suffix tree.
    //!
    //! \parameters (None)
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

    //! Returns an iterator pointing to the first letter of the first word in
    //! the suffix tree.
    //!
    //! \parameters (None)
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

    //! Returns an iterator pointing one past the last letter of the last word
    //! in the suffix tree.
    //!
    //! \parameters (None)
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

    //! Returns an iterator pointing one past the last letter of the last word
    //! in the suffix tree.
    //!
    //! \parameters (None)
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

    //! Returns the index of the word corresponding to a node.
    //!
    //! This function returns the least non-negative integer `i` such that the
    //! node \p n corresponds to the `i`-th word added to the suffix tree.
    //!
    //! \param n the node
    //!
    //! \returns A value of type `word_index_type`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Constant.
    word_index_type word_index(Node const& n) const {
      LIBSEMIGROUPS_ASSERT(n.parent != UNDEFINED);
      return word_index(n.r - 1);
    }

    //! Returns the index of the word corresponding to a position.
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
    word_index_type word_index(index_type i) const {
      LIBSEMIGROUPS_ASSERT(i < _word.size());
      return _word_index_lookup[i];
    }
    //! Returns the distance of a node from the root.
    //!
    //! \param n the node
    //!
    //! \returns A value of type `size_t`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! At worst the distance of the node \p n from the root.
    size_t distance_from_root(Node const& n) const;

    //! Check if a state corresponds to a suffix.
    //!
    //! This function returns a `word_index_type` if the state \p st
    //! corresponds to a suffix of any word in the suffix tree. The value
    //! returned is the index of the word which the state is a suffix of.
    //!
    //! \param st the state
    //!
    //! \returns A value of type `word_index_type`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! At worst the distance of the node \p n from the root.
    word_index_type is_suffix(State const& st) const;

    //! Returns the multiplicity of a word by index.
    //!
    //! This function returns the number of times that the word corresponding to
    //! the index \p i was added to the suffix tree.
    //!
    //! \param i the node
    //!
    //! \returns A value of type `size_t`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Constant.
    size_t multiplicity(word_index_type i) const;

    //! Returns the unique letter added to the end of a word in the suffix tree.
    //!
    //! Returns the unique letter added to the end of the \p i-th distinct word
    //! added to the suffix tree.
    //!
    //! \param i the index of an added word
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

    //! Check if a letter is a unique letter added to the end of a word in the
    //! suffix tree.
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

    //! Find the index of a word in the suffix tree.
    //!
    //! If the word corresponding to \p first and \p last is one of the words
    //! that the suffix tree contains (the words added to the suffix tree via
    //! `add_word` or `add_word_no_checks`, then this function returns the
    //! index of that word. If the word corresponding to \p first and \p last
    //! is not one of the words that the suffix tree represents, then \ref
    //! UNDEFINED is returned.
    //!
    //! \tparam Iterator the type of the arguments
    //! \param first iterator pointing to the first letter of the word to check
    //! \param last one beyond the last letter of the word to check
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
    //! See \ref index_no_checks
    //!
    //! \throws LibsemigroupsException if `validate_word(first, last)` throws.
    template <typename Iterator>
    word_index_type index(Iterator first, Iterator last) const {
      validate_word(first, last);
      return index_no_checks(first, last);
    }

    //! Traverse the suffix tree from the root.
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

    //! See \ref traverse_no_checks.
    //!
    //! \throws LibsemigroupsException if `validate_word(first, last)` throws.
    template <typename Iterator>
    Iterator traverse(State& st, Iterator first, Iterator last) const {
      validate_word(first, last);
      return traverse_no_checks(st, first, last);
    }

    //! Traverse the suffix tree from the root.
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

    //! See \ref traverse_no_checks.
    //!
    //! \throws LibsemigroupsException if `validate_word(first, last)` throws.
    template <typename Iterator>
    std::pair<State, Iterator> traverse(Iterator first, Iterator last) const {
      validate_word(first, last);
      return traverse_no_checks(first, last);
    }

    ////////////////////////////////////////////////////////////////////////
    // Ukkonen - validation - public
    ////////////////////////////////////////////////////////////////////////

    //! Validate the word `[first, last)`.
    //!
    //! This function throws an exception if the word corresponding to \p first
    //! and \p last contains a letter equal to any of the unique letters added
    //! to the end of words in the suffix tree.
    //!
    //! \tparam Iterator the type of the arguments.
    //! \param first iterator pointing to the first letter of the word.
    //! \param last one beyond the last letter of the word.
    //!
    //! \returns (None)
    //!
    //! \throws LibsemigroupsException if `is_unique_letter(*it)` returns
    //! `true` for any `it` in `[first, last)`.
    //!
    //! \complexity
    //! Linear in the distance from \p first to \p last.
    template <typename Iterator>
    void validate_word(Iterator first, Iterator last) const;

    //! Validate a word.
    //!
    //! See \ref validate_word
    void validate_word(word_type const& w) const {
      validate_word(w.cbegin(), w.cend());
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

  namespace ukkonen {
    //! Add all words in a \vector to a Ukkonen object.
    //!
    //! \param u the Ukkonen object
    //! \param words the words to add
    //!
    //! \returns None
    //!
    //! \warning This function does no checks on its arguments whatsoever. In
    //! particular, if the word corresponding to \p first and \p last contains
    //! any of the unique letters appended to the end of any existing word in
    //! the tree, then bad things will happen.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    void add_words_no_checks(Ukkonen& u, std::vector<word_type> const& words);

    //! Add all words in a range to a Ukkonen object.
    //!
    //! \tparam Iterator the type of the 2nd and 3rd parameters
    //! \param u the Ukkonen object
    //! \param first iterator pointing to the first letter of the word.
    //! \param last one beyond the last letter of the word.
    //!
    //! \returns None
    //!
    //! \warning This function does no checks on its arguments whatsoever. In
    //! particular, if the word corresponding to \p first and \p last contains
    //! any of the unique letters appended to the end of any existing word in
    //! the tree, then bad things will happen.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    template <typename Iterator>
    void add_words_no_checks(Ukkonen& u, Iterator first, Iterator last) {
      for (auto it = first; it != last; ++it) {
        u.add_word_no_checks(*it);
      }
    }

    //! See `add_words_no_checks(Ukkonen&, Iterator, Iterator)`
    //!
    //! \throws LibsemigroupsException if `validate_word(w)` throws for any `w`
    //! in \p words.
    void add_words(Ukkonen& u, std::vector<word_type> const& words);

    //! See `add_words_no_checks(Ukkonen&, Iterator, Iterator)`
    //!
    //! \throws LibsemigroupsException if `Ukkonen::validate_word(w)` throws for
    //! any `w` in `[first, last)`.
    template <typename Iterator1, typename Iterator2>
    void add_words(Ukkonen& u, Iterator1 first, Iterator2 last) {
      for (auto it = first; it != last; ++it) {
        u.add_word(*it);
      }
    }

    //! See \ref Ukkonen::traverse_no_checks.
    //!
    //! \throws LibsemigroupsException if `Ukkonen::validate_word(w)` throws.
    // TODO other versions
    inline auto traverse(Ukkonen const& u, word_type const& w) {
      return u.traverse(w.cbegin(), w.cend());
    }

    //! Check if a word is a subword of any word in a suffix tree.
    //!
    //! Returns \c true if the word corresponding to \p first and \p last is a
    //! subword of one of the words in the suffix tree represented by the
    //! Ukkonen instance \p u.
    //!
    //! \tparam Iterator the type of the 2nd and 3rd parameters
    //! \param u the Ukkonen object
    //! \param first iterator pointing to the first letter of the word.
    //! \param last one beyond the last letter of the word.
    //!
    //! \returns A value of type `bool`.
    //!
    //! \warning This function does no checks on its arguments whatsoever. In
    //! particular, if the word corresponding to \p first and \p last contains
    //! any of the unique letters appended to the end of any existing word in
    //! the tree, then bad things will happen.
    //!
    //! \complexity
    //! Linear in the distance between `first` and `last`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    template <typename Iterator>
    bool is_subword_no_checks(Ukkonen const& u, Iterator first, Iterator last);

    //! See `is_subword_no_checks(Ukkonen const&, Iterator, Iterator)`
    template <typename Word>
    bool is_subword_no_checks(Ukkonen const& u, Word const& w) {
      return is_subword_no_checks(u, w.cbegin(), w.cend());
    }

    //! See `is_subword_no_checks(Ukkonen const&, Iterator, Iterator)`
    inline bool is_subword_no_checks(Ukkonen const& u, char const* w) {
      return is_subword_no_checks(u, w, w + std::strlen(w));
    }

    //! See `is_subword_no_checks(Ukkonen const&, Iterator, Iterator)`
    inline bool is_subword_no_checks(Ukkonen const& u, word_type const& w) {
      return is_subword_no_checks(u, w.cbegin(), w.cend());
    }

    //! See `is_subword_no_checks(Ukkonen const&, Iterator, Iterator)`
    //!
    //! \throws LibsemigroupsException if `Ukkonen::validate_word(first, last)`
    //! throws.
    template <typename Iterator>
    bool is_subword(Ukkonen const& u, Iterator first, Iterator last) {
      u.validate_word(first, last);
      return is_subword_no_checks(u, first, last);
    }

    //! See `is_subword_no_checks(Ukkonen const&, Iterator, Iterator)`
    //!
    //! \throws LibsemigroupsException if `Ukkonen::validate_word(w)`
    //! throws.
    template <typename Word>
    bool is_subword(Ukkonen const& u, Word const& w) {
      return is_subword(u, w.cbegin(), w.cend());
    }

    //! See `is_subword_no_checks(Ukkonen const&, Iterator, Iterator)`
    //!
    //! \throws LibsemigroupsException if `Ukkonen::validate_word(w, w +
    //! std::strlen(w))` throws.
    inline bool is_subword(Ukkonen const& u, char const* w) {
      return is_subword(u, w, w + std::strlen(w));
    }

    //! See `is_subword_no_checks(Ukkonen const&, Iterator, Iterator)`
    //!
    //! \throws LibsemigroupsException if `Ukkonen::validate_word(w)`
    //! throws.
    inline bool is_subword(Ukkonen const& u, word_type const& w) {
      return is_subword(u, w.cbegin(), w.cend());
    }

    //! Check if a word is a suffix of any word in a suffix tree.
    //!
    //! Returns \c true if the word corresponding to \p first and \p last is a
    //! suffix of one of the words in the suffix tree represented by the
    //! Ukkonen instance \p u.
    //!
    //! \tparam Iterator the type of the 2nd and 3rd parameters
    //! \param u the Ukkonen object
    //! \param first iterator pointing to the first letter of the word.
    //! \param last one beyond the last letter of the word.
    //!
    //! \returns A value of type `bool`.
    //!
    //! \warning This function does no checks on its arguments whatsoever. In
    //! particular, if the word corresponding to \p first and \p last contains
    //! any of the unique letters appended to the end of any existing word in
    //! the tree, then bad things will happen.
    //!
    //! \complexity
    //! Linear in the distance between `first` and `last`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    template <typename Iterator>
    bool is_suffix_no_checks(Ukkonen const& u, Iterator first, Iterator last);

    //! See `is_suffix_no_checks(Ukkonen const&, Iterator, Iterator)`
    template <typename Word>
    bool is_suffix_no_checks(Ukkonen const& u, Word const& w) {
      return is_suffix_no_checks(u, w.cbegin(), w.cend());
    }

    //! See `is_suffix_no_checks(Ukkonen const&, Iterator, Iterator)`
    // This function is required so that we can use initialiser list, as an
    // argument to is_suffix_no_checks
    inline bool is_suffix_no_checks(Ukkonen const& u, word_type const& w) {
      return is_suffix_no_checks(u, w.cbegin(), w.cend());
    }

    //! See `is_suffix_no_checks(Ukkonen const&, Iterator, Iterator)`
    inline bool is_suffix_no_checks(Ukkonen const& u, char const* w) {
      return is_suffix_no_checks(u, w, w + std::strlen(w));
    }

    //! See `is_suffix_no_checks(Ukkonen const&, Iterator, Iterator)`
    //!
    //! \throws LibsemigroupsException if `Ukkonen::validate_word(first, last)`
    //! throws.
    template <typename Iterator>
    bool is_suffix(Ukkonen const& u, Iterator first, Iterator last) {
      u.validate_word(first, last);
      return is_suffix_no_checks(u, first, last);
    }

    //! See `is_suffix_no_checks(Ukkonen const&, Iterator, Iterator)`
    //!
    //! \throws LibsemigroupsException if `Ukkonen::validate_word(w)`
    //! throws.
    template <typename Word>
    bool is_suffix(Ukkonen const& u, Word const& w) {
      return is_suffix(u, w.cbegin(), w.cend());
    }

    //! See `is_suffix_no_checks(Ukkonen const&, Iterator, Iterator)`
    //!
    //! \throws LibsemigroupsException if `Ukkonen::validate_word(w)`
    //! throws.
    // This function is required so that we can use initialiser list, as an
    // argument to is_suffix
    inline bool is_suffix(Ukkonen const& u, word_type const& w) {
      return is_suffix(u, w.cbegin(), w.cend());
    }

    //! See `is_suffix_no_checks(Ukkonen const&, Iterator, Iterator)`
    //!
    //! \throws LibsemigroupsException if `Ukkonen::validate_word(w, w +
    //! std::strlen(w))` throws.
    inline bool is_suffix(Ukkonen const& u, char const* w) {
      return is_suffix(u, w, w + std::strlen(w));
    }

    //! Find the maximal prefix of a word occurring in two different places in
    //! a word in a suffix tree.
    //!
    //! Returns an iterator pointing one past the last letter in the maximal
    //! length prefix of the word corresponding to \p first and \p last that
    //! occurs in at least \f$2\f$ different (possibly overlapping) places in
    //! the words contained in \p u. If no such prefix exists, then `first` is
    //! returned.
    //!
    //! \tparam Iterator the type of the 2nd and 3rd parameters
    //! \param u the Ukkonen object
    //! \param first iterator pointing to the first letter of the word.
    //! \param last one beyond the last letter of the word.
    //!
    //! \returns A value of type `Iterator`.
    //!
    //! \warning This function does no checks on its arguments whatsoever. In
    //! particular, if the word corresponding to \p first and \p last contains
    //! any of the unique letters appended to the end of any existing word in
    //! the tree, then bad things will happen.
    //!
    //! \complexity
    //! Linear in the distance between `first` and `last`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    template <typename Iterator>
    Iterator maximal_piece_prefix_no_checks(Ukkonen const& u,
                                            Iterator       first,
                                            Iterator       last);

    //! See `maximal_piece_prefix_no_checks(Ukkonen const&, Iterator, Iterator)`
    template <typename Word>
    typename Word::const_iterator
    maximal_piece_prefix_no_checks(Ukkonen const& u, Word const& w) {
      return maximal_piece_prefix_no_checks(u, w.cbegin(), w.cend());
    }

    //! See `maximal_piece_prefix_no_checks(Ukkonen const&, Iterator, Iterator)`
    inline typename word_type::const_iterator
    maximal_piece_prefix_no_checks(Ukkonen const& u, word_type const& w) {
      return maximal_piece_prefix_no_checks(u, w.cbegin(), w.cend());
    }

    //! See `maximal_piece_prefix_no_checks(Ukkonen const&, Iterator, Iterator)`
    inline char const* maximal_piece_prefix_no_checks(Ukkonen const& u,
                                                      char const*    w) {
      return maximal_piece_prefix_no_checks(u, w, w + std::strlen(w));
    }

    //! See `maximal_piece_prefix_no_checks(Ukkonen const&, Iterator, Iterator)`
    //!
    //! \throws LibsemigroupsException if `Ukkonen::validate_word(first, last)`
    //! throws.
    template <typename Iterator>
    Iterator maximal_piece_prefix(Ukkonen const& u,
                                  Iterator       first,
                                  Iterator       last) {
      u.validate_word(first, last);
      return maximal_piece_prefix_no_checks(u, first, last);
    }

    //! See `maximal_piece_prefix_no_checks(Ukkonen const&, Iterator, Iterator)`
    //!
    //! \throws LibsemigroupsException if `Ukkonen::validate_word(w)`
    //! throws.
    template <typename Word>
    typename Word::const_iterator maximal_piece_prefix(Ukkonen const& u,
                                                       Word const&    w) {
      return maximal_piece_prefix(u, w.cbegin(), w.cend());
    }

    //! See `maximal_piece_prefix_no_checks(Ukkonen const&, Iterator, Iterator)`
    //!
    //! \throws LibsemigroupsException if `Ukkonen::validate_word(w)`
    //! throws.
    inline typename word_type::const_iterator
    maximal_piece_prefix(Ukkonen const& u, word_type const& w) {
      return maximal_piece_prefix(u, w.cbegin(), w.cend());
    }

    //! See `maximal_piece_prefix_no_checks(Ukkonen const&, Iterator, Iterator)`
    //!
    //! \throws LibsemigroupsException if `Ukkonen::validate_word(w, w +
    //! std::strlen(w))` throws.
    inline char const* maximal_piece_prefix(Ukkonen const& u, char const* w) {
      return maximal_piece_prefix(u, w, w + std::strlen(w));
    }

    //! Find the length of the maximal prefix of a word occurring in two
    //! different places in a word in a suffix tree.
    //!
    //! Returns length of the maximal length prefix of the word corresponding
    //! to \p first and \p last that occurs in at least \f$2\f$ different
    //! (possibly overlapping) places in the words contained in \p u. If no
    //! such prefix exists, then `0` is returned.
    //!
    //! \tparam Iterator the type of the 2nd and 3rd parameters
    //! \param u the Ukkonen object
    //! \param first iterator pointing to the first letter of the word.
    //! \param last one beyond the last letter of the word.
    //!
    //! \returns A value of type `Iterator`.
    //!
    //! \warning This function does no checks on its arguments whatsoever. In
    //! particular, if the word corresponding to \p first and \p last contains
    //! any of the unique letters appended to the end of any existing word in
    //! the tree, then bad things will happen.
    //!
    //! \complexity
    //! Linear in the distance between `first` and `last`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    template <typename Iterator>
    size_t length_maximal_piece_prefix_no_checks(Ukkonen const& u,
                                                 Iterator       first,
                                                 Iterator       last) {
      return std::distance(first,
                           maximal_piece_prefix_no_checks(u, first, last));
    }

    //! See `maximal_piece_prefix_no_checks(Ukkonen const&, Iterator, Iterator)`
    template <typename Word>
    size_t length_maximal_piece_prefix_no_checks(Ukkonen const& u,
                                                 Word const&    w) {
      return length_maximal_piece_prefix_no_checks(u, w.cbegin(), w.cend());
    }

    //! See `maximal_piece_prefix_no_checks(Ukkonen const&, Iterator, Iterator)`
    inline size_t length_maximal_piece_prefix_no_checks(Ukkonen const&   u,
                                                        word_type const& w) {
      return length_maximal_piece_prefix_no_checks(u, w.cbegin(), w.cend());
    }

    //! See `maximal_piece_prefix_no_checks(Ukkonen const&, Iterator, Iterator)`
    inline size_t length_maximal_piece_prefix_no_checks(Ukkonen const& u,
                                                        char const*    w) {
      return length_maximal_piece_prefix_no_checks(u, w, w + std::strlen(w));
    }

    //! See `maximal_piece_prefix_no_checks(Ukkonen const&, Iterator, Iterator)`
    //!
    //! \throws LibsemigroupsException if `Ukkonen::validate_word(first, last)`
    //! throws.
    template <typename Iterator>
    size_t length_maximal_piece_prefix(Ukkonen const& u,
                                       Iterator       first,
                                       Iterator       last) {
      return std::distance(first, maximal_piece_prefix(u, first, last));
    }

    //! See `maximal_piece_prefix_no_checks(Ukkonen const&, Iterator, Iterator)`
    //!
    //! \throws LibsemigroupsException if `Ukkonen::validate_word(w)`
    //! throws.
    template <typename Word>
    size_t length_maximal_piece_prefix(Ukkonen const& u, Word const& w) {
      return length_maximal_piece_prefix(u, w.cbegin(), w.cend());
    }

    //! See `maximal_piece_prefix_no_checks(Ukkonen const&, Iterator, Iterator)`
    //!
    //! \throws LibsemigroupsException if `Ukkonen::validate_word(w)`
    //! throws.
    inline size_t length_maximal_piece_prefix(Ukkonen const&   u,
                                              word_type const& w) {
      return length_maximal_piece_prefix(u, w.cbegin(), w.cend());
    }

    //! See `maximal_piece_prefix_no_checks(Ukkonen const&, Iterator, Iterator)`
    //!
    //! \throws LibsemigroupsException if `Ukkonen::validate_word(w, w +
    //! std::strlen(w))` throws.
    inline size_t length_maximal_piece_prefix(Ukkonen const& u, char const* w) {
      return length_maximal_piece_prefix(u, w, w + std::strlen(w));
    }

    //! Check if a word is a piece (occurs in two distinct places in the words
    //! of the suffix tree).
    //!
    //! Returns `true` if the word corresponding to \p first and \p last that
    //! occurs in at least \f$2\f$ different (possibly overlapping) places in
    //! the words contained in \p u. If no such prefix exists, then `false` is
    //! returned.
    //!
    //! \tparam Iterator the type of the 2nd and 3rd parameters
    //! \param u the Ukkonen object
    //! \param first iterator pointing to the first letter of the word.
    //! \param last one beyond the last letter of the word.
    //!
    //! \returns A value of type `bool`.
    //!
    //! \warning This function does no checks on its arguments whatsoever. In
    //! particular, if the word corresponding to \p first and \p last contains
    //! any of the unique letters appended to the end of any existing word in
    //! the tree, then bad things will happen.
    //!
    //! \complexity
    //! Linear in the distance between `first` and `last`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    template <typename Iterator>
    bool is_piece_no_checks(Ukkonen const& u, Iterator first, Iterator last) {
      return maximal_piece_prefix_no_checks(u, first, last) == last;
    }

    //! See `is_piece_no_checks(Ukkonen const&, Iterator, Iterator)`
    template <typename Word>
    bool is_piece_no_checks(Ukkonen const& u, Word const& w) {
      return is_piece_no_checks(u, w.cbegin(), w.cend());
    }

    //! See `is_piece_no_checks(Ukkonen const&, Iterator, Iterator)`
    inline bool is_piece_no_checks(Ukkonen const& u, word_type const& w) {
      return is_piece_no_checks(u, w.cbegin(), w.cend());
    }

    //! See `is_piece_no_checks(Ukkonen const&, Iterator, Iterator)`
    inline bool is_piece_no_checks(Ukkonen const& u, char const* w) {
      return is_piece_no_checks(u, w, w + std::strlen(w));
    }

    //! See `is_piece_no_checks(Ukkonen const&, Iterator, Iterator)`
    //!
    //! \throws LibsemigroupsException if `Ukkonen::validate_word(first, last)`
    //! throws.
    template <typename Iterator>
    bool is_piece(Ukkonen const& u, Iterator first, Iterator last) {
      return maximal_piece_prefix(u, first, last) == last;
    }

    //! See `is_piece_no_checks(Ukkonen const&, Iterator, Iterator)`
    //!
    //! \throws LibsemigroupsException if `Ukkonen::validate_word(w)`
    //! throws.
    template <typename Word>
    bool is_piece(Ukkonen const& u, Word const& w) {
      return is_piece(u, w.cbegin(), w.cend());
    }

    //! See `is_piece_no_checks(Ukkonen const&, Iterator, Iterator)`
    //!
    //! \throws LibsemigroupsException if `Ukkonen::validate_word(w)`
    //! throws.
    inline bool is_piece(Ukkonen const& u, word_type const& w) {
      return is_piece(u, w.cbegin(), w.cend());
    }

    //! See `is_piece_no_checks(Ukkonen const&, Iterator, Iterator)`
    //!
    //! \throws LibsemigroupsException if `Ukkonen::validate_word(w, w +
    //! std::strlen(w))` throws.
    inline bool is_piece(Ukkonen const& u, char const* w) {
      return is_piece(u, w, w + std::strlen(w));
    }

    //! Find the maximal suffix of a word occurring in two different places in
    //! a word in a suffix tree.
    //!
    //! Returns an iterator pointing at the first letter in the maximal
    //! length suffix of the word corresponding to \p first and \p last that
    //! occurs in at least \f$2\f$ different (possibly overlapping) places in
    //! the words contained in \p u. If no such suffix exists, then `first` is
    //! returned.
    //!
    //! \tparam Iterator the type of the 2nd and 3rd parameters
    //! \param u the Ukkonen object
    //! \param first iterator pointing to the first letter of the word.
    //! \param last one beyond the last letter of the word.
    //!
    //! \returns A value of type `Iterator`.
    //!
    //! \warning This function does no checks on its arguments whatsoever. In
    //! particular, if the word corresponding to \p first and \p last contains
    //! any of the unique letters appended to the end of any existing word in
    //! the tree, then bad things will happen.
    //!
    //! \complexity
    //! At worst \f$O(m ^ 2)\f$ or \f$O(n)\f$ where \f$m\f$ is the distance
    //! between `first` and `last` and \f$n\f$ is the return value of
    //! Ukkonen::length_of_distinct_words.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    template <typename Iterator>
    Iterator maximal_piece_suffix_no_checks(Ukkonen const& u,
                                            Iterator       first,
                                            Iterator       last);

    //! See `maximal_piece_suffix_no_checks(Ukkonen const&, Iterator, Iterator)`
    template <typename Word>
    typename Word::const_iterator
    maximal_piece_suffix_no_checks(Ukkonen const& u, Word const& w) {
      return maximal_piece_suffix_no_checks(u, w.cbegin(), w.cend());
    }

    //! See `maximal_piece_suffix_no_checks(Ukkonen const&, Iterator, Iterator)`
    inline typename word_type::const_iterator
    maximal_piece_suffix_no_checks(Ukkonen const& u, word_type const& w) {
      return maximal_piece_suffix_no_checks(u, w.cbegin(), w.cend());
    }

    //! See `maximal_piece_suffix_no_checks(Ukkonen const&, Iterator, Iterator)`
    inline char const* maximal_piece_suffix_no_checks(Ukkonen const& u,
                                                      char const*    w) {
      return maximal_piece_suffix_no_checks(u, w, w + std::strlen(w));
    }

    //! See `maximal_piece_suffix_no_checks(Ukkonen const&, Iterator, Iterator)`
    //!
    //! \throws LibsemigroupsException if `Ukkonen::validate_word(first, last)`
    //! throws.
    template <typename Iterator>
    Iterator maximal_piece_suffix(Ukkonen const& u,
                                  Iterator       first,
                                  Iterator       last) {
      u.validate_word(first, last);
      return maximal_piece_suffix_no_checks(u, first, last);
    }

    //! See `maximal_piece_suffix_no_checks(Ukkonen const&, Iterator, Iterator)`
    //!
    //! \throws LibsemigroupsException if `Ukkonen::validate_word(w)`
    //! throws.
    template <typename Word>
    typename Word::const_iterator maximal_piece_suffix(Ukkonen const& u,
                                                       Word const&    w) {
      return maximal_piece_suffix(u, w.cbegin(), w.cend());
    }

    //! See `maximal_piece_suffix_no_checks(Ukkonen const&, Iterator, Iterator)`
    //!
    //! \throws LibsemigroupsException if `Ukkonen::validate_word(w)`
    //! throws.
    inline typename word_type::const_iterator
    maximal_piece_suffix(Ukkonen const& u, word_type const& w) {
      return maximal_piece_suffix(u, w.cbegin(), w.cend());
    }

    //! See `maximal_piece_suffix_no_checks(Ukkonen const&, Iterator, Iterator)`
    //!
    //! \throws LibsemigroupsException if `Ukkonen::validate_word(w, w +
    //! std::strlen(w))` throws.
    inline char const* maximal_piece_suffix(Ukkonen const& u, char const* w) {
      return maximal_piece_suffix(u, w, w + std::strlen(w));
    }

    //! Find the length of the maximal suffix of a word occurring in two
    //! different places in a word in a suffix tree.
    //!
    //! Returns length of the maximal length prefix of the word corresponding
    //! to \p first and \p last that occurs in at least \f$2\f$ different
    //! (possibly overlapping) places in the words contained in \p u. If no
    //! such prefix exists, then `0` is returned.
    //!
    //! \tparam Iterator the type of the 2nd and 3rd parameters
    //! \param u the Ukkonen object
    //! \param first iterator pointing to the first letter of the word.
    //! \param last one beyond the last letter of the word.
    //!
    //! \returns A value of type `Iterator`.
    //!
    //! \warning This function does no checks on its arguments whatsoever. In
    //! particular, if the word corresponding to \p first and \p last contains
    //! any of the unique letters appended to the end of any existing word in
    //! the tree, then bad things will happen.
    //!
    //! \complexity
    //! Linear in the distance between `first` and `last`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    template <typename Iterator>
    size_t length_maximal_piece_suffix_no_checks(Ukkonen const& u,
                                                 Iterator       first,
                                                 Iterator       last) {
      return std::distance(maximal_piece_suffix_no_checks(u, first, last),
                           last);
    }

    //! See `length_maximal_piece_suffix_no_checks(Ukkonen const&, Iterator,
    //! Iterator)`
    template <typename Word>
    size_t length_maximal_piece_suffix_no_checks(Ukkonen const& u,
                                                 Word const&    w) {
      return length_maximal_piece_suffix_no_checks(u, w.cbegin(), w.cend());
    }

    //! See `length_maximal_piece_suffix_no_checks(Ukkonen const&, Iterator,
    //! Iterator)`
    inline size_t length_maximal_piece_suffix_no_checks(Ukkonen const&   u,
                                                        word_type const& w) {
      return length_maximal_piece_suffix_no_checks(u, w.cbegin(), w.cend());
    }

    //! See `length_maximal_piece_suffix_no_checks(Ukkonen const&, Iterator,
    //! Iterator)`
    inline size_t length_maximal_piece_suffix_no_checks(Ukkonen const& u,
                                                        char const*    w) {
      return length_maximal_piece_suffix_no_checks(u, w, w + std::strlen(w));
    }

    //! See `length_maximal_piece_suffix_no_checks(Ukkonen const&, Iterator,
    //! Iterator)`
    //!
    //! \throws LibsemigroupsException if `Ukkonen::validate_word(first, last)`
    //! throws.
    template <typename Iterator>
    size_t length_maximal_piece_suffix(Ukkonen const& u,
                                       Iterator       first,
                                       Iterator       last) {
      return std::distance(maximal_piece_suffix(u, first, last), last);
    }

    //! See `length_maximal_piece_suffix_no_checks(Ukkonen const&, Iterator,
    //! Iterator)`
    //!
    //! \throws LibsemigroupsException if `Ukkonen::validate_word(w)`
    //! throws.
    template <typename Word>
    size_t length_maximal_piece_suffix(Ukkonen const& u, Word const& w) {
      return length_maximal_piece_suffix(u, w.cbegin(), w.cend());
    }

    //! See `length_maximal_piece_suffix_no_checks(Ukkonen const&, Iterator,
    //! Iterator)`
    //!
    //! \throws LibsemigroupsException if `Ukkonen::validate_word(w)`
    //! throws.
    inline size_t length_maximal_piece_suffix(Ukkonen const&   u,
                                              word_type const& w) {
      return length_maximal_piece_suffix(u, w.cbegin(), w.cend());
    }

    //! See `length_maximal_piece_suffix_no_checks(Ukkonen const&, Iterator,
    //! Iterator)`
    //!
    //! \throws LibsemigroupsException if `Ukkonen::validate_word(w, w +
    //! std::strlen(w))` throws.
    inline size_t length_maximal_piece_suffix(Ukkonen const& u, char const* w) {
      return length_maximal_piece_suffix(u, w, w + std::strlen(w));
    }

    //! Find the number of pieces in a decomposition of a word (if any).
    //!
    //! Returns minimum number of pieces whose product equals the word
    //! corresponding to \p first and \p last if such a product exists, and \c
    //! 0 if no such product exists. Recall that a *piece* is a word that
    //! occurs in two distinct positions (possibly overlapping) of the words in
    //! the suffix tree \p u.
    //!
    //! \tparam Iterator the type of the 2nd and 3rd parameters
    //! \param u the Ukkonen object
    //! \param first iterator pointing to the first letter of the word.
    //! \param last one beyond the last letter of the word.
    //!
    //! \returns A value of type `size_t`.
    //!
    //! \warning This function does no checks on its arguments whatsoever. In
    //! particular, if the word corresponding to \p first and \p last contains
    //! any of the unique letters appended to the end of any existing word in
    //! the tree, then bad things will happen.
    //!
    //! \complexity
    //! Linear in the distance between `first` and `last`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    template <typename Iterator>
    size_t number_of_pieces_no_checks(Ukkonen const& u,
                                      Iterator       first,
                                      Iterator       last);

    //! See `number_of_pieces_no_checks(Ukkonen const&, Iterator,
    //! Iterator)`
    template <typename Word>
    size_t number_of_pieces_no_checks(Ukkonen const& u, Word const& w) {
      return number_of_pieces_no_checks(u, w.cbegin(), w.cend());
    }

    //! See `number_of_pieces_no_checks(Ukkonen const&, Iterator,
    //! Iterator)`
    inline size_t number_of_pieces_no_checks(Ukkonen const&   u,
                                             word_type const& w) {
      return number_of_pieces_no_checks(u, w.cbegin(), w.cend());
    }

    //! See `number_of_pieces_no_checks(Ukkonen const&, Iterator,
    //! Iterator)`
    inline size_t number_of_pieces_no_checks(Ukkonen const& u, char const* w) {
      return number_of_pieces_no_checks(u, w, w + std::strlen(w));
    }

    //! See `number_of_pieces_no_checks(Ukkonen const&, Iterator,
    //! Iterator)`
    //!
    //! \throws LibsemigroupsException if `Ukkonen::validate_word(first, last)`
    //! throws.
    template <typename Iterator>
    size_t number_of_pieces(Ukkonen const& u, Iterator first, Iterator last) {
      u.validate_word(first, last);
      return number_of_pieces_no_checks(u, first, last);
    }

    //! See `number_of_pieces_no_checks(Ukkonen const&, Iterator,
    //! Iterator)`
    //!
    //! \throws LibsemigroupsException if `Ukkonen::validate_word(w)`
    //! throws.
    template <typename Word>
    size_t number_of_pieces(Ukkonen const& u, Word const& w) {
      return number_of_pieces(u, w.cbegin(), w.cend());
    }

    //! See `number_of_pieces_no_checks(Ukkonen const&, Iterator,
    //! Iterator)`
    //!
    //! \throws LibsemigroupsException if `Ukkonen::validate_word(w)`
    //! throws.
    inline size_t number_of_pieces(Ukkonen const& u, word_type const& w) {
      return number_of_pieces(u, w.cbegin(), w.cend());
    }

    //! See `number_of_pieces_no_checks(Ukkonen const&, Iterator,
    //! Iterator)`
    //!
    //! \throws LibsemigroupsException if `Ukkonen::validate_word(w, w +
    //! std::strlen(w))` throws.
    inline size_t number_of_pieces(Ukkonen const& u, char const* w) {
      return number_of_pieces(u, w, w + std::strlen(w));
    }

    //! Returns the number of distinct subwords of the words in a suffix tree.
    //!
    //! Returns the total number of distinct subwords of the words in the
    //! suffix tree \p u.
    //!
    //! \param u the Ukkonen object
    //!
    //! \returns A value of type `size_t`.
    //!
    //! \complexity
    //! Linear in `Ukkonen::length_of_distinct_words`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    size_t number_of_distinct_subwords(Ukkonen const& u);

    //! Find the pieces in a decomposition of a word (if any).
    //!
    //! Returns a \vector of iterators pointing one past the last letter in the
    //! pieces whose product equals the word corresponding to \p first and \p
    //! last if such a product exists, and an empty \vector if no such product
    //! exists. Recall that a *piece* is a word that occurs in two distinct
    //! positions (possibly overlapping) of the words in the suffix tree \p u.
    //!
    //! \tparam Iterator the type of the 2nd and 3rd parameters
    //! \param u the Ukkonen object
    //! \param first iterator pointing to the first letter of the word.
    //! \param last one beyond the last letter of the word.
    //!
    //! \returns A value of type `std::vector<Iterator>`.
    //!
    //! \warning This function does no checks on its arguments whatsoever. In
    //! particular, if the word corresponding to \p first and \p last contains
    //! any of the unique letters appended to the end of any existing word in
    //! the tree, then bad things will happen.
    //!
    //! \complexity
    //! Linear in the distance between `first` and `last`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    template <typename Iterator>
    std::vector<Iterator> pieces_no_checks(Ukkonen const& u,
                                           Iterator       first,
                                           Iterator       last);

    //! See `pieces_no_checks(Ukkonen const&, Iterator, Iterator)`.
    template <typename Word>
    std::vector<Word> pieces_no_checks(Ukkonen const& u, Word const& w);

    //! See `pieces_no_checks(Ukkonen const&, Iterator, Iterator)`.
    std::vector<word_type> pieces_no_checks(Ukkonen const&   u,
                                            word_type const& w);

    //! See `pieces_no_checks(Ukkonen const&, Iterator, Iterator)`.
    std::vector<std::string> pieces_no_checks(Ukkonen const& u, char const* w);

    //! See `pieces_no_checks(Ukkonen const&, Iterator, Iterator)`.
    //!
    //! \throws LibsemigroupsException if `Ukkonen::validate_word(first, last)`
    //! throws.
    template <typename Iterator>
    std::vector<Iterator> pieces(Ukkonen const& u,
                                 Iterator       first,
                                 Iterator       last) {
      u.validate_word(first, last);
      return pieces_no_checks(u, first, last);
    }

    //! See `pieces_no_checks(Ukkonen const&, Iterator, Iterator)`.
    //!
    //! \throws LibsemigroupsException if `Ukkonen::validate_word(w)`
    //! throws.
    template <typename Word>
    std::vector<Word> pieces(Ukkonen const& u, Word const& w) {
      u.validate_word(w.cbegin(), w.cend());
      return pieces_no_checks(u, w);
    }

    //! See `pieces_no_checks(Ukkonen const&, Iterator, Iterator)`.
    //!
    //! \throws LibsemigroupsException if `Ukkonen::validate_word(w)`
    //! throws.
    inline std::vector<word_type> pieces(Ukkonen const& u, word_type const& w) {
      u.validate_word(w);
      return pieces_no_checks(u, w);
    }

    //! See `pieces_no_checks(Ukkonen const&, Iterator, Iterator)`.
    //!
    //! \throws LibsemigroupsException if `Ukkonen::validate_word(w, w +
    //! std::strlen(w))` throws.
    inline std::vector<std::string> pieces(Ukkonen const& u, char const* w) {
      u.validate_word(w, w + std::strlen(w));
      return pieces_no_checks(u, w);
    }

    //! Returns a string containing a [GraphViz][] representation of a suffix
    //! tree.
    //!
    //! \param u the Ukkonen object
    //!
    //! \returns A value of type `std::string`.
    //!
    //! \throws LibsemigroupsException if \p u does not contain any words
    //! \throws LibsemigroupsException if the number of words in \p u is greater
    //! than 24.
    //!
    //! [GraphViz]: https://graphviz.org
    std::string dot(Ukkonen const& u);

    //! Perform a depth first search in a suffix tree.
    //!
    //! This function can be used to perform a depth first search in the suffix
    //! tree \p u. The 2nd parameter is a helper object that must implement:
    //!
    //! * A function `void pre_order(Ukkonen const& u, size_t i)`
    //! * A function `void post_order(Ukkonen const& u, size_t i)`
    //! * A function `auto yield(Ukkonen const& u)`
    //!
    //! The function `T::pre_order` is called when the node \c n with index \c i
    //! is first encountered in the depth-first search, and the function
    //! `T::post_order` is called when the subtree rooted at \c n has been
    //! completely explored.
    //!
    //! The function `yield` is called at the end of the depth-first search and
    //! its return value is returned by this function.
    //!
    //! \tparam T the type of the helper object
    //! \param u the Ukkonen object
    //! \param helper the helper object
    //!
    //! \returns A value whose type is the same as the return type of
    //! `T::yield`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    template <typename T>
    auto dfs(Ukkonen const& u, T& helper);

    namespace detail {
      // TODO when the old ToddCoxeter is removed move this into present.hpp
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

        void pre_order(Ukkonen const& u, size_t v);
        void post_order(Ukkonen const& u, size_t v);
        std::pair<const_iterator, const_iterator> yield(Ukkonen const& u);
      };
    }  // namespace detail
  }    // namespace ukkonen
}  // namespace libsemigroups

#include "ukkonen.tpp"

#endif  // LIBSEMIGROUPS_UKKONEN_HPP_
