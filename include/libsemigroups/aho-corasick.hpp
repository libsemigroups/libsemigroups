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

namespace libsemigroups {

  class AhoCorasick {
   public:
    using index_type     = size_t;
    using const_iterator = word_type::const_iterator;

    static constexpr index_type root = 0;

   private:
    class Node {
     private:
      mutable std::map<letter_type, index_type> _children;
      mutable index_type                        _link;
      index_type                                _parent;
      letter_type                               _parent_letter;
      bool                                      _terminal;

     public:
      Node() : Node(UNDEFINED, UNDEFINED) {}

      // TODO to cpp
      Node(index_type parent, letter_type a)
          : _children(), _link(), _parent(), _parent_letter(), _terminal() {
        init(parent, a);
      }

      Node& init(index_type parent, letter_type a) {
        _parent        = parent;
        _parent_letter = a;
        _children.clear();
        clear_suffix_link();
        _terminal = false;
        return *this;
      }

      [[nodiscard]] index_type child(letter_type a) const noexcept {
        if (_children.count(a) == 0) {
          return UNDEFINED;
        }
        return static_cast<index_type>(_children[a]);
      }

      [[nodiscard]] index_type suffix_link() const noexcept {
        return _link;
      }

      void clear_suffix_link() {
        if (parent() == root || parent() == UNDEFINED) {
          set_suffix_link(root);
        } else {
          set_suffix_link(UNDEFINED);
        }
      }

      [[nodiscard]] decltype(_children)& children() const {
        return _children;
      }

      [[nodiscard]] bool is_terminal() const noexcept {
        // TODO should this check suffix links are terminal?
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
    };

    std::vector<Node>      _all_nodes;
    std::set<index_type>   _active_nodes_index;
    std::stack<index_type> _inactive_nodes_index;

   public:
    AhoCorasick()
        : _all_nodes({Node()}), _active_nodes_index(), _inactive_nodes_index() {
      _active_nodes_index.insert(0);
    }
    // TODO init

    size_t number_of_nodes() const noexcept {
      return _active_nodes_index.size();
    }

    // TODO to cpp
    // TODO Add flags to show links have been cleared?
    void add_word_no_checks(const_iterator first, const_iterator last) {
      clear_suffix_links();  // don't do this if first >= last
      index_type current = root;
      for (auto it = first; it != last; ++it) {
        index_type next = _all_nodes[current].child(*it);
        if (next != UNDEFINED) {
          current = next;
        } else {
          next = new_active_node(current, *it);  // index of next node added
          // set next as child of parent
          _all_nodes[current].children()[*it] = next;
          current                             = next;
        }
      }
      _all_nodes[current].set_terminal(true);
    }

    // TODO to cpp
    // TODO completely untested
    void rm_word_no_checks(const_iterator first, const_iterator last) {
      auto last_index = traverse_trie(first, last);
      if (last_index == UNDEFINED || !_all_nodes[last_index].is_terminal()) {
        return;
      } else if (!_all_nodes[last_index].children().empty()) {
        _all_nodes[last_index].set_terminal(false);
        return;
      }
      clear_suffix_links();
      auto parent_index  = _all_nodes[last_index].parent();
      auto parent_letter = *(last - 1);
      deactivate_node(last_index);
      while (_all_nodes[parent_index].children().size() == 1) {
        last_index    = parent_index;
        parent_index  = _all_nodes[last_index].parent();
        parent_letter = _all_nodes[last_index].parent_letter();
        deactivate_node(last_index);
      }
      _all_nodes[parent_index].children().erase(parent_letter);
    }

    void add_word_no_checks(word_type const& w) {
      add_word_no_checks(w.cbegin(), w.cend());
    }

    void rm_word_no_checks(word_type const& w) {
      rm_word_no_checks(w.cbegin(), w.cend());
    }

    [[nodiscard]] index_type traverse_from(index_type     start,
                                           const_iterator first,
                                           const_iterator last) const {
      index_type current = start;
      for (auto it = first; it != last; ++it) {
        // Uses private traverse by node function
        current = traverse(current, *it);
      }
      return current;
    }

    [[nodiscard]] index_type traverse_from(index_type       start,
                                           word_type const& w) const {
      return traverse_from(start, w.cbegin(), w.cend());
    }

    // TODO to cpp
    [[nodiscard]] index_type traverse(const_iterator first,
                                      const_iterator last) const {
      return traverse_from(root, first, last);
    }

    [[nodiscard]] index_type traverse(word_type const& w) const {
      return traverse_from(root, w.cbegin(), w.cend());
    }

    // TODO to cpp
    [[nodiscard]] index_type suffix_link(index_type current) const {
      auto& n = _all_nodes[current];
      if (n.suffix_link() == UNDEFINED) {
        n.set_suffix_link(traverse(suffix_link(n.parent()), n.parent_letter()));
      }
      return n.suffix_link();
    }

    [[nodiscard]] Node const& node(index_type i) const {
      return _all_nodes.at(i);
    }

    // TODO to cpp
    void signature(word_type& w, index_type i) const {
      w.clear();
      while (i != root) {
        w.push_back(_all_nodes[i].parent_letter());
        i = _all_nodes[i].parent();
      }
      std::reverse(w.begin(), w.end());
    }

   private:
    index_type new_active_node(index_type parent, letter_type a) {
      index_type index;
      if (_inactive_nodes_index.empty()) {
        index = _all_nodes.size();
        _all_nodes.emplace_back(parent, a);
        _active_nodes_index.insert(index);
      } else {
        index = _inactive_nodes_index.top();
        _inactive_nodes_index.pop();
        _active_nodes_index.insert(index);
        _all_nodes[index].init(parent, a);
      }
      return index;
    }

    // This breaks traversal, as node numbers should correlate to their position
    // in this vector
    void deactivate_node(index_type i) {
      LIBSEMIGROUPS_ASSERT(i < _all_nodes.size());
#ifdef LIBSEMIGROUPS_DEBUG
      auto num_removed =
#endif
          _active_nodes_index.erase(i);
      LIBSEMIGROUPS_ASSERT(num_removed == 1);
      _inactive_nodes_index.push(i);
    }

    // TODO to cpp
    [[nodiscard]] index_type traverse(index_type current, letter_type a) const {
      index_type next = _all_nodes[current].child(a);
      if (next != UNDEFINED) {
        return next;
      } else if (current == root) {
        return root;
      }
      return traverse(suffix_link(current), a);
    }

    [[nodiscard]] index_type traverse_trie(const_iterator first,
                                           const_iterator last) const {
      index_type current = root;
      for (auto it = first; it != last; ++it) {
        current = _all_nodes[current].child(*it);
        if (current == UNDEFINED) {
          return current;
        }
      }
      return current;
    }

    void clear_suffix_links() {
      for (auto index : _active_nodes_index) {
        _all_nodes[index].clear_suffix_link();
      }
    }
  };

  class Dot;  // forward decl

  Dot dot(AhoCorasick& ac);

  class TrieRewriter {};

}  // namespace libsemigroups

#endif  // LIBSEMIGROUPS_AHO_CORASICK_HPP_
