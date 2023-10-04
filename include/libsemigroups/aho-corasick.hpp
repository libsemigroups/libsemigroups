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

#include <map>
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
      explicit Node(index_type parent, letter_type a)
          : _children(),
            _link(UNDEFINED),
            _parent(parent),
            _parent_letter(a),
            _terminal(false) {
        if (_parent == root || _parent == UNDEFINED) {
          _link = root;
        }
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

    std::vector<Node> _nodes;

   public:
    AhoCorasick() : _nodes({Node()}) {}
    // TODO init

    [[nodiscard]] size_t number_of_nodes() const noexcept {
      return _nodes.size();
    }

    // TODO to cpp
    void add_word_no_checks(const_iterator first, const_iterator last) {
      clear_suffix_links();
      index_type current = root;
      for (auto it = first; it != last; ++it) {
        index_type next = _nodes[current].child(*it);
        if (next != UNDEFINED) {
          current = next;
        } else {
          next = _nodes.size();               // index of next node added
          _nodes.emplace_back(current, *it);  // create next Node obj
          // set next as child of parent
          _nodes[current].children()[*it] = next;
          current                         = next;
        }
      }
      _nodes[current].set_terminal(true);
    }

    // TODO to cpp
    // TODO completely untested
    void rm_word_no_checks(const_iterator first, const_iterator last) {
      auto last_index = traverse_trie(first, last);
      if (last_index == UNDEFINED || !_nodes[last_index].is_terminal()) {
        return;
      } else if (!_nodes[last_index].children().empty()) {
        _nodes[last_index].set_terminal(false);
        return;
        // TODO is this right? what if last_index has children?
      }
      clear_suffix_links();
      auto parent_index  = _nodes[last_index].parent();
      auto parent_letter = *(last - 1);
      rm_node(last_index);
      while (_nodes[parent_index].children().size() == 1) {
        last_index    = parent_index;
        parent_index  = _nodes[last_index].parent();
        parent_letter = _nodes[last_index].parent_letter();
        rm_node(last_index);
      }
      _nodes[parent_index].children().erase(parent_letter);
    }

    void rm_word_no_checks(word_type const& w) {
      rm_word_no_checks(w.cbegin(), w.cend());
    }

    void add_word_no_checks(word_type const& w) {
      add_word_no_checks(w.cbegin(), w.cend());
    }

    // TODO to cpp
    [[nodiscard]] index_type traverse(const_iterator first,
                                      const_iterator last) const {
      index_type current = root;
      for (auto it = first; it != last; ++it) {
        current = traverse(current, *it);
      }
      return current;
    }

    [[nodiscard]] index_type traverse(word_type const& w) const {
      return traverse(w.cbegin(), w.cend());
    }

    // TODO to cpp
    [[nodiscard]] index_type suffix_link(index_type current) const {
      auto& n = _nodes[current];
      if (n.suffix_link() == UNDEFINED) {
        n.set_suffix_link(traverse(suffix_link(n.parent()), n.parent_letter()));
      }
      return n.suffix_link();
    }

    [[nodiscard]] Node const& node(index_type i) const {
      return _nodes.at(i);
    }

    // TODO to cpp
    void signature(word_type& w, index_type i) const {
      w.clear();
      while (i != root) {
        w.push_back(_nodes[i].parent_letter());
        i = _nodes[i].parent();
      }
      std::reverse(w.begin(), w.end());
    }

   private:
    // TODO to cpp
    [[nodiscard]] index_type traverse(index_type current, letter_type a) const {
      index_type next = _nodes[current].child(a);
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
        current = _nodes[current].child(*it);
        if (current == UNDEFINED) {
          return current;
        }
      }
      return current;
    }

    void clear_suffix_links() {
      for (auto& node : _nodes) {
        node.clear_suffix_link();
      }
    }

    void rm_node(index_type i) {
      LIBSEMIGROUPS_ASSERT(i < _nodes.size());
      _nodes.erase(_nodes.begin() + i);
    }
  };

  class Dot;  // forward decl

  Dot dot(AhoCorasick& ac);

  class TrieRewriter {};

}  // namespace libsemigroups

#endif  // LIBSEMIGROUPS_AHO_CORASICK_HPP_
