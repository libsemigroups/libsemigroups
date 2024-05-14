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

#include <algorithm>
#include <map>
#include <set>
#include <stack>
#include <unordered_map>
#include <vector>

#include "constants.hpp"  // for UNDEFINED
#include "debug.hpp"      // for LIBSEMIGROUPS_ASSERT
#include "types.hpp"      // for letter_type

// TODO(2) make nodes accessible as indices of some list (numbered nodes).
// Make sure this address the badness of traversals (lots of different data
// types and it just feels a bit hacky)
// TODO(2) is it worthwhile storing a pointer to the terminal nodes beneath
// each node? If this can be updated quickly, it would save a lot of time in
// overlap/confluence checking. One compromise is to have a pointer to the rules
// any given node is contained within. This could be updated easily when adding
// new rules, but more care would be needed when removing rules.
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

      Node& init(index_type parent, letter_type a);

      [[nodiscard]] index_type child(letter_type a) const noexcept;

      [[nodiscard]] size_t height() const noexcept {
        return _height;
      }

      [[nodiscard]] index_type suffix_link() const noexcept {
        return _link;
      }

      void clear_suffix_link() const;

      [[nodiscard]] decltype(_children)& children() const {
        return _children;
      }

      [[nodiscard]] size_t number_of_children() const {
        return _children.size();
      }

      [[nodiscard]] bool is_terminal() const noexcept {
        return _terminal;
      }

      Node& set_terminal(bool val) {
        _terminal = val;
        return *this;
      }

      [[nodiscard]] index_type parent() const noexcept {
        return _parent;
      }

      [[nodiscard]] letter_type parent_letter() const noexcept {
        return _parent_letter;
      }

      void set_suffix_link(index_type val) const {
        _link = val;
      }

      void set_height(size_t val) const {
        _height = val;
      }
    };

    std::vector<Node>      _all_nodes;
    std::set<index_type>   _active_nodes_index;
    std::stack<index_type> _inactive_nodes_index;
    mutable bool           _valid_links;

   public:
    AhoCorasick();

    AhoCorasick& init();

    size_t number_of_nodes() const noexcept {
      return _active_nodes_index.size();
    }

    template <typename Iterator>
    index_type add_word_no_checks(Iterator first, Iterator last);

    template <typename Iterator>
    index_type rm_word(Iterator first, Iterator last);

    template <typename Iterator>
    index_type rm_word_no_checks(Iterator first, Iterator last);

    [[nodiscard]] index_type traverse(index_type current, letter_type a) const;

    template <typename Iterator>
    [[nodiscard]] index_type traverse_from(index_type start,
                                           Iterator   first,
                                           Iterator   last) const;

    // TODO(2) template to accept Iterator not word_type&
    void signature(word_type& w, index_type i) const;

    size_t height(index_type i) const {
      size_t cached_height = _all_nodes[i].height();
      if (cached_height != UNDEFINED) {
        return cached_height;
      }

      if (i == root) {
        _all_nodes[i].set_height(0);
        return 0;
      }

      cached_height = height(_all_nodes[i].parent()) + 1;
      _all_nodes[i].set_height(cached_height);
      return cached_height;
    }

    [[nodiscard]] index_type suffix_link(index_type current) const;

    [[nodiscard]] Node const& node(index_type i) const {
      return _all_nodes.at(i);
    }

    [[nodiscard]] index_type child(index_type  parent,
                                   letter_type letter) const {
      return _all_nodes[parent].child(letter);
    }

   private:
    index_type new_active_node(index_type parent, letter_type a);

    void deactivate_node(index_type i) {
      LIBSEMIGROUPS_ASSERT(i < _all_nodes.size());
#ifdef LIBSEMIGROUPS_DEBUG
      auto num_removed =
#endif
          _active_nodes_index.erase(i);
      LIBSEMIGROUPS_ASSERT(num_removed == 1);
      _inactive_nodes_index.push(i);
    }

    template <typename Iterator>
    [[nodiscard]] index_type traverse_trie(Iterator first, Iterator last) const;

    void clear_suffix_links() const;
  };

  class Dot;  // forward decl

  Dot dot(AhoCorasick& ac);

  namespace aho_corasick {
    using index_type = AhoCorasick::index_type;

    // TODO(0) Can these all be made inline?
    // JDM: yes they can
    inline void add_word_no_checks(AhoCorasick& ac, word_type const& w) {
      ac.add_word_no_checks(w.cbegin(), w.cend());
    }

    inline index_type rm_word_no_checks(AhoCorasick& ac, word_type const& w) {
      return ac.rm_word_no_checks(w.cbegin(), w.cend());
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
      return ac.traverse_from(start, w.cbegin(), w.cend());
    }

    template <typename Iterator>
    [[nodiscard]] index_type traverse(AhoCorasick const& ac,
                                      Iterator           first,
                                      Iterator           last) {
      return ac.traverse_from(AhoCorasick::root, first, last);
    }

    [[nodiscard]] inline index_type traverse(AhoCorasick const& ac,
                                             word_type const&   w) {
      return ac.traverse_from(AhoCorasick::root, w.cbegin(), w.cend());
    }
  }  // namespace aho_corasick

}  // namespace libsemigroups

#include "aho-corasick.tpp"

#endif  // LIBSEMIGROUPS_AHO_CORASICK_HPP_
