//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2023-2024 Joseph Edwards + James D. Mitchell
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

// This file contains the implementation of a trie with suffix links for use by
// the Aho-Corasick dictionary search algorithm

#ifndef LIBSEMIGROUPS_AHO_CORASICK_HPP_
#define LIBSEMIGROUPS_AHO_CORASICK_HPP_

#include <algorithm>      // for copy, max
#include <memory>         // for allocator_traits<>::value_type
#include <set>            // for set
#include <stack>          // for stack
#include <stddef.h>       // for size_t
#include <unordered_map>  // for unordered_map
#include <vector>         // for vector

#include "constants.hpp"  // for Undefined, operator!=, UNDEFINED, operator==
#include "debug.hpp"      // for LIBSEMIGROUPS_ASSERT
#include "exception.hpp"  // for LIBSEMIGROUPS_EXCEPTION
#include "types.hpp"      // for letter_type, word_type

// TODO(2) make nodes accessible as indices of some list (numbered nodes).
// Make sure this address the badness of traversals (lots of different data
// types and it just feels a bit hacky)
// TODO(2) is it worthwhile storing a pointer to the terminal nodes beneath
// each node? If this can be updated quickly, it would save a lot of time in
// overlap/confluence checking. One compromise is to have a pointer to the rules
// any given node is contained within. This could be updated easily when adding
// new rules, but more care would be needed when removing rules.
// TODO(1) change names from set_X and get_X to X(val) and X(). e.g.
// set_suffix_link('a') -> suffix_link('a')
// TODO(2) add something that gets a ranges element to find all terminal nodes.

//! \defgroup aho_corasick_group Aho Corasick
//!
//! This file contains documentation for the functionality in the `AhoCorasick`
//! class in `libsemigroups`.
namespace libsemigroups {

  //! \ingroup aho_corasick_group
  //!
  //! \brief For an implementation of the Aho-Corasick algorithm
  //!
  //! Defined in ``aho-corasick.hpp``.
  //!
  //! This class implements a trie based data structure with suffix links to be
  //! used with the Aho-Corasick dictionary searching algorithm. An introduction
  //! to this algorithm can be found at:
  //!
  //! https://en.wikipedia.org/wiki/Aho%E2%80%93Corasick_algorithm
  //!
  //! Several helper functions are provided in the ``aho_corasick`` namespace.
  class AhoCorasick {
   public:
    //! Alias for the index of a node in the trie
    using index_type = size_t;

    //! Used for `word_type` iterators.
    using const_iterator = typename word_type::const_iterator;

    //! Constant for the root of the trie
    static constexpr index_type root = 0;

   private:
    //  Implements the node of a trie
    class Node {
     private:
      mutable std::unordered_map<letter_type, index_type> _children;
      mutable index_type                                  _link;
      mutable size_t                                      _height;
      index_type                                          _parent;
      letter_type                                         _parent_letter;
      bool                                                _terminal;

     public:
      Node() : Node(UNDEFINED, UNDEFINED) {}

      Node(index_type parent, letter_type a)
          : _children(),
            _link(),
            _height(),
            _parent(),
            _parent_letter(),
            _terminal() {
        init(parent, a);
      }

      Node(const Node&)            = default;
      Node& operator=(const Node&) = default;
      Node(Node&&)                 = default;
      Node& operator=(Node&&)      = default;

      Node& init(index_type parent, letter_type a) noexcept;

      Node& init() noexcept {
        return init(UNDEFINED, UNDEFINED);
      }

      [[nodiscard]] index_type child(letter_type a) const;

      [[nodiscard]] size_t height() const noexcept {
        return _height;
      }

      [[nodiscard]] index_type suffix_link() const noexcept {
        return _link;
      }

      void set_suffix_link(index_type val) const noexcept {
        _link = val;
      }

      void clear_suffix_link() const noexcept;

      [[nodiscard]] decltype(_children)& children() const noexcept {
        return _children;
      }

      [[nodiscard]] size_t number_of_children() const noexcept {
        return _children.size();
      }

      [[nodiscard]] bool is_terminal() const noexcept {
        return _terminal;
      }

      Node& set_terminal(bool val) noexcept {
        _terminal = val;
        return *this;
      }

      [[nodiscard]] index_type parent() const noexcept {
        return _parent;
      }

      [[nodiscard]] letter_type parent_letter() const noexcept {
        return _parent_letter;
      }

      void set_height(size_t val) const noexcept {
        _height = val;
      }
    };

    std::vector<Node>      _all_nodes;
    std::set<index_type>   _active_nodes_index;
    std::stack<index_type> _inactive_nodes_index;
    mutable bool           _valid_links;

   public:
    //! \brief Construct an empty AhoCorasick
    //!
    //! Construct an AhoCorasick containing only the root that corresponds to
    //! the empty word \f$\varepsilon\f$.
    AhoCorasick();

    //! \brief Default copy constructor
    //!
    //! Default copy constructor
    AhoCorasick(AhoCorasick const&) = default;

    //! \brief Default copy assignment
    //!
    //! Default copy assignment
    AhoCorasick& operator=(AhoCorasick const&) = default;

    //! \brief Default move constructor
    //!
    //! Default move constructor
    AhoCorasick(AhoCorasick&&) = default;

    //! \brief Default move assignment
    //!
    //! Default move assignment
    AhoCorasick& operator=(AhoCorasick&&) = default;

    //! \brief Reinitialise an existing AhoCorasick object
    //!
    //! This function puts an AhoCorasick object back into the same state as if
    //! it had been newly default constructed.
    //!
    //! \returns A reference to \c this.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Linear in the number of nodes in the trie
    AhoCorasick& init();

    //! \brief Returns the number of nodes in the trie.
    //!
    //! This function Returns the number of nodes in the trie.
    //!
    //! \returns
    //! A value of type `size_t`.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant
    [[nodiscard]] size_t number_of_nodes() const noexcept {
      return _active_nodes_index.size();
    }

    //! \brief Check and add a word to the trie.
    //!
    //! This function does the same as \ref add_word_no_checks
    //! `add_word_no_checks(Iterator, Iterator)` after first checking that the
    //! word corresponding to \p first and \p last does not correspond to an
    //! existing terminal node in the trie.
    //!
    //! \throws LibsemigroupsException if the word corresponding to \p first and
    //! \p last corresponds to an existing terminal node in the trie.
    //!
    //! \sa \ref add_word_no_checks
    template <typename Iterator>
    index_type add_word(Iterator first, Iterator last);

    //! \brief Add a word to the trie.
    //!
    //! Calling this function immediately adds the given word to the trie, and
    //! makes the final node on the path labelled by this word terminal (if it
    //! wasn't already). After adding a word, existing suffix links become
    //! invalid. If an identical word has already been added to the trie, then
    //! this function does nothing. If `first == last`, then this function does
    //! nothing.
    //!
    //! \tparam Iterator the type of the 1st and 2nd parameters
    //! \param first iterator pointing to the first letter of the word to add.
    //! \param last one beyond the last letter of the word to add.
    //!
    //! \returns An \ref index_type corresponding to the final node added to the
    //! trie. This node will have a \ref signature equal to that of the given
    //! word.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Linear in the distance between `first` and `last`.
    //!
    //! \sa \ref signature
    template <typename Iterator>
    index_type add_word_no_checks(Iterator first, Iterator last);

    //! \brief Check and add a word to the trie.
    //!
    //! This function does the same as \ref rm_word_no_checks
    //! `rm_word_no_checks(Iterator, Iterator)` after first checking that the
    //! word corresponding to \p first and \p last is terminal node in the trie.
    //!
    //! \throws LibsemigroupsException if the word corresponding to \p first and
    //! \p last does not correspond to an existing terminal node in the trie.
    //!
    //! \sa \ref rm_word_no_checks
    template <typename Iterator>
    index_type rm_word(Iterator first, Iterator last);

    //! \brief Remove a word from the trie.
    //!
    //! From the trie, remove each node of the given word that is not part of
    //! the prefix of a different word.
    //!
    //! If the given word \f$W\f$ corresponds to a terminal node with no
    //! children, then calling this function removes the nodes \f$n_i\f$ from
    //! the trie that correspond to the largest suffix \f$W\f$, such
    //! that each \f$n_i\f$ has either zero children or one. After this,
    //! existing suffix links become invalid.
    //!
    //! If \f$W\f$ corresponds to a terminal node \f$n\f$ with children, then
    //! calling this function makes \f$n\f$ not terminal.
    //!
    //! If \f$W\f$ does not correspond to a terminal node, then calling this
    //! function does nothing.
    //!
    //! \tparam Iterator the type of the 1st and 2nd parameters
    //! \param first iterator pointing to the first letter of the word to add.
    //! \param last one beyond the last letter of the word to add.
    //!
    //! \returns An \ref index_type corresponding to the node with signature
    //! equal to the given word.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Linear in the distance between `first` and `last`.
    //!
    //! \sa \ref signature
    template <typename Iterator>
    index_type rm_word_no_checks(Iterator first, Iterator last);

    //! \brief Traverse the trie using suffix links where necessary
    //!
    //! This function traverses the trie using suffix links where necessary,
    //! behaving like a combination of the *goto* function and the *fail*
    //! function in \cite Aho1975aa.
    //!
    //! If \p current is the index of a node with signature \f$W\f$, and
    //! \p a is the letter \f$a\f$, then \c traverse_no_checks(current, a)
    //! returns the index of the node with signature equal to the longest suffix
    //! of \f$Wa\f$ contained in the trie.
    //!
    //! \param current the index of the node to traverse from
    //! \param a the letter to traverse
    //!
    //! \returns An value of type `index_type`
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    // TODO(2) Should this be templated?
    [[nodiscard]] index_type traverse_no_checks(index_type  current,
                                                letter_type a) const;

    //! \brief Traverse the trie using suffix links where necessary.
    //!
    //! After validating \p current, this function performs the same as
    //! `traverse_no_checks(current, a)`.
    //!
    //! \throws LibsemigroupsException if `validate_active_node_index(current)`
    //! throws.
    //!
    //! \sa \ref traverse_no_checks, \ref validate_active_node_index.
    [[nodiscard]] index_type traverse(index_type current, letter_type a) const {
      validate_active_node_index(current);
      return traverse_no_checks(current, a);
    }

    //! \brief Find the signature of a node
    //!
    //! Changes \p w in-place to contain the signature of the node with index
    //! \p i . Recall that the *signature* of a node  \f$n\f$ is the word
    //! consisting of the edge labels of the unique path from the root to
    //! \f$n\f$.
    //!
    //! \param w the word to clear and change in-place
    //! \param i the index of the node whose signature is sought
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Linear in the height of the node
    //!
    //! \warning This function does no checks on its arguments whatsoever. In
    //! particular, if the index \p i is greater than the number of nodes that
    //! have ever been created, then bad things will happen.
    // TODO(2) template to accept Iterator not word_type&
    void signature_no_checks(word_type& w, index_type i) const;

    //! \brief Find the signature of a node
    //!
    //! After validating \p i, this function performs the same as
    //! `signature_no_checks(w, i)`.
    //!
    //! \throws LibsemigroupsException if `validate_active_node_index(i)`
    //! throws.
    //!
    //! \sa \ref signature_no_checks, \ref validate_active_node_index.
    void signature(word_type& w, index_type i) const {
      validate_active_node_index(i);
      signature_no_checks(w, i);
    }

    //! \brief Calculate the height of a node
    //!
    //! \param i the index of the node whose height is sought
    //!
    //! \returns A value of type `size_t`
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Linear in the return value which is, at worst, the maximum length of a
    //! word in the trie
    //!
    //! \warning This function does no checks on its arguments whatsoever. In
    //! particular, if the index \p i is greater than the number of nodes that
    //! have ever been created, then bad things will happen.
    [[nodiscard]] size_t height_no_checks(index_type i) const;

    //! \brief Calculate the height of a node
    //!
    //! After validating \p i, this function performs the same as
    //! `height_no_checks(i)`.
    //!
    //! \throws LibsemigroupsException if `validate_active_node_index(i)`
    //! throws.
    //!
    //! \sa \ref height_no_checks, \ref validate_active_node_index.
    [[nodiscard]] size_t height(index_type i) const {
      validate_active_node_index(i);
      return height_no_checks(i);
    }

    //! \brief Calculate the index of the suffix link of a node
    //!
    //! Calculate the index of a suffix link of a node. Recall that the
    //! *suffix link* of a node with signature \f$W\f$ is the node with the
    //! signature equal to that of the longest proper suffix of \f$W\f$
    //! contained in the trie.
    //!
    //! \param current the index of the node whose suffix link is sought
    //!
    //! \returns A value of type `index_type`
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Linear in the height of the node.
    //!
    //! \warning This function does no checks on its arguments whatsoever. In
    //! particular, if the index \p current is greater than the number of nodes
    //! that have ever been created, then bad things will happen.
    [[nodiscard]] index_type suffix_link_no_checks(index_type current) const;

    //! \brief Calculate the index of the suffix link of a node
    //!
    //! After validating \p current, this function performs the same as
    //! `suffix_link_no_checks(current)`.
    //!
    //! \throws LibsemigroupsException if `validate_active_node_index(current)`
    //! throws.
    //!
    //! \sa \ref suffix_link_no_checks, \ref validate_active_node_index.
    [[nodiscard]] index_type suffix_link(index_type current) const {
      validate_active_node_index(current);
      return suffix_link_no_checks(current);
    }

    //! \brief Return the node given an index
    //!
    //! This function returns the node stored in the trie given by the index
    //! \p i
    //!
    //! \param i the index of the node to return
    //!
    //! \returns A value of type `Node`
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Constant
    //!
    //! \note The node returned by this function may not represent a node
    //! presently stored in the trie. See \ref validate_active_node_index
    //!
    //! \warning This function does no checks on its arguments whatsoever. In
    //! particular, if the index \p i is greater than the number of nodes that
    //! have ever been created, then bad things will happen.
    [[nodiscard]] Node const& node_no_checks(index_type i) const {
      LIBSEMIGROUPS_ASSERT(i < _all_nodes.size());
      return _all_nodes[i];
    }

    //! \brief Return the node given an index
    //!
    //! After validating \p i, this function performs the same as
    //! `node_no_checks(i)`.
    //!
    //! \throws LibsemigroupsException if `validate_node_index(i)` throws.
    //!
    //! \sa \ref node_no_checks, \ref validate_node_index.
    [[nodiscard]] Node const& node(index_type i) const {
      validate_node_index(i);
      return node_no_checks(i);
    }

    //! \brief Return the child of \p parent with edge-label \p letter
    //!
    //! This function returns the index of the child of the node with index
    //! \p parent along the edge labelled by \p letter. If no such child exists,
    //! \ref UNDEFINED is returned.
    //!
    //! \param parent the index of the node whose child is sought
    //! \param letter the edge-label connecting the parent to the desired child
    //!
    //! \returns A value of type `index_type`
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Constant
    //!
    //! \warning This function does no checks on its arguments whatsoever. In
    //! particular, if the index \p parent is greater than the number of nodes
    //! that have ever been created, then bad things will happen.
    [[nodiscard]] index_type child_no_checks(index_type  parent,
                                             letter_type letter) const {
      LIBSEMIGROUPS_ASSERT(parent < _all_nodes.size());
      LIBSEMIGROUPS_ASSERT(_active_nodes_index.count(parent) == 1);
      return _all_nodes[parent].child(letter);
    }

    //! \brief Return the child of \p parent with edge-label \p letter
    //!
    //! After validating \p parent, this function performs the same as
    //! `child_no_checks(parent, letter)`.
    //!
    //! \throws LibsemigroupsException if `validate_active_node_index(parent)`
    //! throws.
    //!
    //! \sa \ref child_no_checks, \ref validate_active_node_index.
    [[nodiscard]] index_type child(index_type  parent,
                                   letter_type letter) const {
      validate_active_node_index(parent);
      return _all_nodes[parent].child(letter);
    }

    //! \brief Check if an index corresponds to a node
    //!
    //! This function checks if the given index \p i corresponds to the index of
    //! a node.
    //!
    //! \param i the index to validate
    //!
    //! \throws LibsemigroupsException if \p i does not correspond to the index
    //! of a node; that is, if \p i is larger than the size of the container
    //! storing the indices of nodes.
    //!
    //! \complexity
    //! Constant
    void validate_node_index(index_type i) const;

    //! \brief Check if an index corresponds to a node currently in the trie
    //!
    //! The implementation of \ref AhoCorasick uses two different types of node;
    //! *active* and *inactive*. An active node is a node that is currently a
    //! node in the trie. An inactive node is a node that used to be part of the
    //! trie, but has since been removed. It may later become active again after
    //! being reinitialised, and exists as a way of minimising how frequently
    //! memory needs to be allocated and deallocated for nodes.
    //!
    //! This function validates whether the given index \p i corresponds to an
    //! active node.
    //!
    //! \param i the index to validate
    //!
    //! \throws LibsemigroupsException if `validate_node_index(i)` throws, or if
    //! \p i is not an active node.
    //!
    //! \complexity
    //! Constant
    //!
    //! \sa \ref validate_node_index.
    void validate_active_node_index(index_type i) const;

   private:
    [[nodiscard]] index_type new_active_node_no_checks(index_type  parent,
                                                       letter_type a);

    [[nodiscard]] index_type new_active_node(index_type parent, letter_type a) {
      validate_active_node_index(parent);
      return new_active_node_no_checks(parent, a);
    }

    void deactivate_node_no_checks(index_type i);

    void deactivate_node(index_type i) {
      validate_active_node_index(i);
      deactivate_node_no_checks(i);
    }

    template <typename Iterator>
    [[nodiscard]] index_type traverse_trie(Iterator first, Iterator last) const;

    void clear_suffix_links() const;
  };

  //! \ingroup aho_corasick_group
  //!
  //! \brief Return a string representation
  //!
  //! Return a string representation of \p ac. This has the form `<AhoCorasick
  //! with [ac.number_of_nodes] node(s)>`
  //!
  //! \returns A string containing a representation of \p ac.
  //!
  //! \exception
  //! \no_libsemigroups_except
  std::string to_string(AhoCorasick& ac);

  class Dot;  // forward decl

  //! \ingroup aho_corasick_group
  //!
  //! \brief Construct a dot object of \p ac
  //!
  //! Construct a \ref Dot object representing the trie of \p ac with suffix
  //! links.
  [[nodiscard]] Dot dot(AhoCorasick& ac);

  //! \ingroup aho_corasick_group
  //!
  //! \brief Namespace for AhoCorasick helper functions
  //!
  //! Defined in ``aho-corasick.hpp``.
  //!
  //! This namespace contains various helper functions for the class
  //! AhoCorasick. These functions could be functions of AhoCorasick but they
  //! only use public member functions of AhoCorasick, and so they are declared
  //! as free functions instead.
  namespace aho_corasick {
    using index_type = AhoCorasick::index_type;

    //! \brief Add a word to the trie of \p ac.
    //!
    //! This function performs the same as `ac.add_word_no_checks(w.begin(),
    //! w.end())`
    //!
    //! \tparam Word the type of the 2nd parameter \p w.
    //! \param ac AhoCorasick object to add the word to.
    //! \param w the word to add.
    //!
    //! \returns An \ref index_type corresponding to the final node added to the
    //! \p ac.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Linear in the length of \p w.
    //!
    //! \sa \ref AhoCorasick::add_word_no_checks.
    template <typename Word>
    void add_word_no_checks(AhoCorasick& ac, Word const& w) {
      ac.add_word_no_checks(w.cbegin(), w.cend());
    }

    //! \brief Remove a word from the trie of \p ac.
    //!
    //! This function performs the same as `ac.rm_word_no_checks(w.begin(),
    //! w.end())`
    //!
    //! \tparam Word the type of the 2nd parameter \p w.
    //! \param ac AhoCorasick object to remove the word from.
    //! \param w the word to remove.
    //!
    //! \returns An \ref index_type corresponding to the node with signature
    //! equal to \p w.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Linear in the length of \p w.
    //!
    //! \sa \ref AhoCorasick::rm_word_no_checks.
    template <typename Word>
    index_type rm_word_no_checks(AhoCorasick& ac, Word const& w) {
      return ac.rm_word_no_checks(w.cbegin(), w.cend());
    }

    //! \brief Add a word to the trie of \p ac.
    //!
    //! This function performs the same as `ac.add_word(w.begin(), w.end())`
    //!
    //! \tparam Word the type of the 2nd parameter \p w.
    //! \param ac AhoCorasick object to add the word to.
    //! \param w the word to add.
    //!
    //! \returns An \ref index_type corresponding to the final node added to the
    //! \p ac.
    //!
    //! \throws LibsemigroupsException if the word \p w corresponds to an
    //! existing terminal node in the trie.
    //!
    //! \complexity
    //! Linear in the length of \p w.
    //!
    //! \sa \ref AhoCorasick::add_word.
    template <typename Word>
    void add_word(AhoCorasick& ac, Word const& w) {
      ac.add_word(w.cbegin(), w.cend());
    }

    //! \brief Remove a word from the trie of \p ac.
    //!
    //! This function performs the same as `ac.rm_word(w.begin(), w.end())`
    //!
    //! \tparam Word the type of the 2nd parameter \p w.
    //! \param ac AhoCorasick object to remove the word from.
    //! \param w the word to remove.
    //!
    //! \returns An \ref index_type corresponding to the node with signature
    //! equal to \p w.
    //!
    //! \throws LibsemigroupsException if the word \p w does not correspond to
    //! an existing terminal node in the trie.
    //!
    //! \complexity
    //! Linear in the length of \p w.
    //!
    //! \sa \ref AhoCorasick::rm_word.
    template <typename Word>
    index_type rm_word(AhoCorasick& ac, Word const& w) {
      return ac.rm_word(w.cbegin(), w.cend());
    }

    //! \brief Traverse the trie of \p ac using suffix links where necessary.
    //!
    //! This function traverses the trie of \p ac, starting at the node with
    //! index \p start, and traversing using the letters in the word
    //! corresponding to \p first and \p last.
    //!
    //! \tparam Iterator the type of the 2nd and 3rd parameters.
    //! \param ac AhoCorasick object to traverse.
    //! \param start the index of the node to first traverse from.
    //! \param first iterator pointing to the first letter of the word to
    //! traverse.
    //! \param last one beyond the last letter of the word to traverse.
    //!
    //! \returns An value of type `index_type`
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \warning This function does no checks on its arguments whatsoever. In
    //! particular, if the index \p start is greater than the number of nodes
    //! that have ever been created, then bad things will happen.
    //!
    //! \sa \ref AhoCorasick::traverse_no_checks
    template <typename Iterator>
    [[nodiscard]] index_type traverse_word_no_checks(AhoCorasick const& ac,
                                                     index_type         start,
                                                     Iterator           first,
                                                     Iterator           last);

    //! \brief Traverse the trie of \p ac using suffix links where necessary.
    //!
    //! This function performs the same as `traverse_word_no_checks(ac, start,
    //! w.cbegin(), w.cend())`
    //!
    //! \sa \ref traverse_word_no_checks.
    [[nodiscard]] inline index_type
    traverse_word_no_checks(AhoCorasick const& ac,
                            index_type         start,
                            word_type const&   w) {
      return traverse_word_no_checks(ac, start, w.cbegin(), w.cend());
    }

    //! \brief Traverse the trie of \p ac using suffix links where necessary.
    //!
    //! After validating \p start with respect to \p ac, this function performs
    //! the same as `traverse_word_no_checks(ac, start, first, last)`.
    //!
    //! \throws LibsemigroupsException if `ac.validate_active_node_index(start)`
    //! throws.
    //!
    //! \sa \ref traverse_word_no_checks, \ref
    //! AhoCorasick::validate_active_node_index.
    template <typename Iterator>
    [[nodiscard]] index_type traverse_word(AhoCorasick const& ac,
                                           index_type         start,
                                           Iterator           first,
                                           Iterator           last) {
      ac.validate_active_node_index(start);
      return traverse_word_no_checks(ac, start, first, last);
    }

    //! \brief Traverse the trie of \p ac using suffix links where necessary.
    //!
    //! This function performs the same as `traverse_word(ac, start, w.cbegin(),
    //! w.cend())`
    //!
    //! \sa \ref traverse_word.
    [[nodiscard]] inline index_type traverse_word(AhoCorasick const& ac,
                                                  index_type         start,
                                                  word_type const&   w) {
      return traverse_word(ac, start, w.cbegin(), w.cend());
    }

    //! \brief Traverse the trie of \p ac from the root using suffix links where
    //! necessary.
    //!
    //! This function performs the same as `traverse_word_no_checks(ac,
    //! AhoCorasick::root, first, last)`
    //!
    //! \note
    //! There is no `_no_checks` suffix here as AhoCorasick::root is always a
    //! valid node of a trie, and therefore no checks are needed.
    //!
    //! \sa \ref traverse_word_no_checks.
    template <typename Iterator>
    [[nodiscard]] index_type traverse_word(AhoCorasick const& ac,
                                           Iterator           first,
                                           Iterator           last) {
      return traverse_word_no_checks(ac, AhoCorasick::root, first, last);
    }

    //! \brief Traverse the trie of \p ac from the root using suffix links where
    //! necessary.
    //!
    //! This function performs the same as `traverse_word_no_checks(ac,
    //! AhoCorasick::root, w.cbegin(), w.end())`
    //!
    //! \note
    //! There is no `_no_checks` suffix here as AhoCorasick::root is always a
    //! valid node of a trie, and therefore no checks are needed.
    //!
    //! \sa \ref traverse_word_no_checks.
    template <typename Word>
    [[nodiscard]] index_type traverse_word(AhoCorasick const& ac,
                                           Word const&        w) {
      return traverse_word(ac, AhoCorasick::root, w.cbegin(), w.cend());
    }
  }  // namespace aho_corasick

}  // namespace libsemigroups

#include "aho-corasick.tpp"

#endif  // LIBSEMIGROUPS_AHO_CORASICK_HPP_
