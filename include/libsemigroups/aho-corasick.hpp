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
namespace libsemigroups {

  //! \brief For an implementation of the Aho-Corasick algorithm
  //!
  //! Defined in ``aho-corasick.hpp``.
  //!
  //! This class implements the Aho-Corasick algorithm for string-searching
  //! using a trie with suffix links. An introduction to this algorithm can be
  //! found at:
  //!
  //! https://en.wikipedia.org/wiki/Aho%E2%80%93Corasick_algorithm
  //!
  //! Several helper functions are provided in the ``aho_corasick`` namespace.
  class AhoCorasick {
   public:
    using index_type     = size_t;
    using const_iterator = word_type::const_iterator;

    static constexpr index_type root = 0;

   private:
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
    AhoCorasick();
    AhoCorasick(const AhoCorasick&)            = default;
    AhoCorasick& operator=(const AhoCorasick&) = default;
    AhoCorasick(AhoCorasick&&)                 = default;
    AhoCorasick& operator=(AhoCorasick&&)      = default;

    AhoCorasick& init();

    [[nodiscard]] size_t number_of_nodes() const {
      return _active_nodes_index.size();
    }

    template <typename Iterator>
    index_type add_word(Iterator first, Iterator last);

    template <typename Iterator>
    index_type add_word_no_checks(Iterator first, Iterator last);

    template <typename Iterator>
    index_type rm_word(Iterator first, Iterator last);

    template <typename Iterator>
    index_type rm_word_no_checks(Iterator first, Iterator last);

    [[nodiscard]] index_type traverse_no_checks(index_type  current,
                                                letter_type a) const;

    [[nodiscard]] index_type traverse(index_type current, letter_type a) const {
      validate_active_node_index(current);
      return traverse_no_checks(current, a);
    }

    // TODO(2) template to accept Iterator not word_type&
    void signature_no_checks(word_type& w, index_type i) const;

    void signature(word_type& w, index_type i) const {
      validate_active_node_index(i);
      signature_no_checks(w, i);
    }

    [[nodiscard]] size_t height_no_checks(index_type i) const;

    [[nodiscard]] size_t height(index_type i) const {
      validate_active_node_index(i);
      return height_no_checks(i);
    }

    [[nodiscard]] index_type suffix_link_no_checks(index_type current) const;

    [[nodiscard]] index_type suffix_link(index_type current) const {
      validate_active_node_index(current);
      return suffix_link_no_checks(current);
    }

    [[nodiscard]] Node const& node_no_checks(index_type i) const {
      LIBSEMIGROUPS_ASSERT(parent < _all_nodes.size());
      return _all_nodes.at(i);
    }

    [[nodiscard]] Node const& node(index_type i) const {
      validate_node_index(i);
      return _all_nodes.at(i);
    }

    [[nodiscard]] index_type child_no_checks(index_type  parent,
                                             letter_type letter) const {
      LIBSEMIGROUPS_ASSERT(parent < _all_nodes.size());
      LIBSEMIGROUPS_ASSERT(_active_nodes_index.count(parent) == 1);
      return _all_nodes[parent].child(letter);
    }

    [[nodiscard]] index_type child(index_type  parent,
                                   letter_type letter) const {
      validate_active_node_index(parent);
      return _all_nodes[parent].child(letter);
    }

    void validate_node_index(index_type i) const;

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

  class Dot;  // forward decl

  [[nodiscard]] Dot dot(AhoCorasick& ac);

  namespace aho_corasick {
    using index_type = AhoCorasick::index_type;

    // TODO(0) Can these all be made inline?
    // JDM: yes they can
    template <typename Word>
    inline void add_word_no_checks(AhoCorasick& ac, Word const& w) {
      ac.add_word_no_checks(w.cbegin(), w.cend());
    }

    template <typename Word>
    inline index_type rm_word_no_checks(AhoCorasick& ac, Word const& w) {
      return ac.rm_word_no_checks(w.cbegin(), w.cend());
    }

    template <typename Word>
    inline void add_word(AhoCorasick& ac, Word const& w) {
      ac.add_word(w.cbegin(), w.cend());
    }

    template <typename Word>
    inline index_type rm_word(AhoCorasick& ac, Word const& w) {
      return ac.rm_word(w.cbegin(), w.cend());
    }

    template <typename Iterator>
    [[nodiscard]] index_type traverse_from_no_checks(AhoCorasick const& ac,
                                                     index_type         start,
                                                     Iterator           first,
                                                     Iterator           last);

    template <typename Letter>
    [[nodiscard]] inline index_type
    traverse_from_no_checks(AhoCorasick const& ac,
                            index_type         start,
                            Letter const&      w) {
      return ac.traverse_no_checks(start, w);
    }

    [[nodiscard]] inline index_type
    traverse_from_no_checks(AhoCorasick const& ac,
                            index_type         start,
                            word_type const&   w) {
      return traverse_from_no_checks(ac, start, w.cbegin(), w.cend());
    }

    template <typename Iterator>
    [[nodiscard]] index_type traverse_from(AhoCorasick const& ac,
                                           index_type         start,
                                           Iterator           first,
                                           Iterator           last) {
      ac.validate_active_node_index(start);
      return traverse_from_no_checks(ac, start, first, last);
    }

    template <typename Letter>
    [[nodiscard]] inline index_type traverse_from(AhoCorasick const& ac,
                                                  index_type         start,
                                                  Letter const&      w) {
      return ac.traverse(start, w);
    }

    [[nodiscard]] inline index_type traverse_from(AhoCorasick const& ac,
                                                  index_type         start,
                                                  word_type const&   w) {
      return traverse_from(ac, start, w.cbegin(), w.cend());
    }

    template <typename Iterator>
    [[nodiscard]] index_type traverse(AhoCorasick const& ac,
                                      Iterator           first,
                                      Iterator           last) {
      return traverse_from_no_checks(ac, AhoCorasick::root, first, last);
    }

    template <typename Word>
    [[nodiscard]] inline index_type traverse(AhoCorasick const& ac,
                                             Word const&        w) {
      return traverse_from(ac, AhoCorasick::root, w.cbegin(), w.cend());
    }
  }  // namespace aho_corasick

}  // namespace libsemigroups

#include "aho-corasick.tpp"

#endif  // LIBSEMIGROUPS_AHO_CORASICK_HPP_
