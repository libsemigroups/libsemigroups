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
      mutable std::map<letter_type, int32_t> _children;
      index_type                             _link;
      index_type                             _parent;
      bool                                   _terminal;

     public:
      explicit Node(index_type parent)
          : _children(), _link(UNDEFINED), _parent(parent), _terminal(false) {
        if (_parent == root || _parent == UNDEFINED) {
          _link = root;
        }
      }

      [[nodiscard]] index_type child(letter_type a) const noexcept {
        if (_children.count(a) == 0 || _children[a] < 0) {
          return UNDEFINED;
        }
        return static_cast<index_type>(_children[a]);
      }

      [[nodiscard]] decltype(_children)& children() {
        return _children;
      }

      [[nodiscard]] bool is_root() const noexcept {
        return _parent == UNDEFINED;
      }

      [[nodiscard]] bool is_terminal() const noexcept {
        return _terminal;
      }

      Node& terminal(bool val) {
        _terminal = val;
        return *this;
      }
    };

    std::vector<Node> _nodes;

   public:
    AhoCorasick() : _nodes({Node(static_cast<index_type>(UNDEFINED))}) {}

    [[nodiscard]] size_t number_of_nodes() const noexcept {
      return _nodes.size();
    }

    void add_word_no_checks(const_iterator first, const_iterator last) {
      index_type current = root;
      for (auto it = first; it != last; ++it) {
        index_type next = _nodes[current].child(*it);
        if (next != UNDEFINED) {
          current = next;
        } else {
          next = _nodes.size();          // index of next node added
          _nodes.emplace_back(current);  // create next Node obj
          // set next as child of parent
          _nodes[current].children()[*it] = next;
          current                         = next;
        }
      }
      _nodes[current].terminal(true);
    }

    void add_word_no_checks(word_type const& w) {
      add_word_no_checks(w.cbegin(), w.cend());
    }

    [[nodiscard]] bool accepts(const_iterator first,
                               const_iterator last) const {
      index_type current = root;
      for (auto it = first; it != last; ++it) {
        index_type next = _nodes[current].child(*it);
        if (next == UNDEFINED) {
          return false;
        }
        current = next;
      }
      return _nodes[current].is_terminal();
    }

    [[nodiscard]] bool accepts(word_type const& w) const {
      return accepts(w.cbegin(), w.cend());
    }
  };
}  // namespace libsemigroups

#endif  // LIBSEMIGROUPS_AHO_CORASICK_HPP_
