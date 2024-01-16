//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2023 Joe Edwards + James D. Mitchell
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
#include <vector>

#include "constants.hpp"  // for UNDEFINED
#include "debug.hpp"      // for LIBSEMIGROUPS_ASSERT
#include "types.hpp"      // for letter_type

// TODO make nodes accessible as indices of some list (numbered nodes).
// Make sure this address the badness of traversals (lots of different data
// types and it just feels a bit hacky)
// TODO (later) is it worthwhile storing a pointer to the terminal nodes beneath
// each node? If this can be updated quickly, it would save a lot of time in
// overlap/confluence checking. One compromise is to have a pointer to the rules
// any given node is contained within. This could be updated easily when adding
// new rules, but more care would be needed when removing rules.
namespace libsemigroups {

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

    template <typename Iterator>
    [[nodiscard]] index_type traverse_from(index_type start,
                                           Iterator   first,
                                           Iterator   last) const;

    // TODO template to accept Iterator not word_type&
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

    // TODO move to helper namespace
    void add_word_no_checks(word_type const& w) {
      add_word_no_checks(w.cbegin(), w.cend());
    }

    // TODO move to helper namespace
    index_type rm_word_no_checks(word_type const& w) {
      return rm_word_no_checks(w.cbegin(), w.cend());
    }

    // TODO move to helper namespace
    template <typename Letter>
    [[nodiscard]] index_type traverse_from(index_type    start,
                                           Letter const& w) const {
      return traverse(start, w);
    }

    // TODO move to helper namespace
    [[nodiscard]] index_type traverse_from(index_type       start,
                                           word_type const& w) const {
      return traverse_from(start, w.cbegin(), w.cend());
    }

    // TODO move to helper namespace
    template <typename Iterator>
    [[nodiscard]] index_type traverse(Iterator first, Iterator last) const {
      return traverse_from(root, first, last);
    }

    // TODO move to helper namespace
    [[nodiscard]] index_type traverse(word_type const& w) const {
      return traverse_from(root, w.cbegin(), w.cend());
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

    [[nodiscard]] index_type traverse(index_type current, letter_type a) const;

    template <typename Iterator>
    [[nodiscard]] index_type traverse_trie(Iterator first, Iterator last) const;

    void clear_suffix_links() const;
  };

  class Dot;  // forward decl

  Dot dot(AhoCorasick& ac);

  namespace aho_corasick {}

}  // namespace libsemigroups

#include "aho-corasick.tpp"

#endif  // LIBSEMIGROUPS_AHO_CORASICK_HPP_
