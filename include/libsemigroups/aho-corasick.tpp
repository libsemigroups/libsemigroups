//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019-2024 James D. Mitchell
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
// This file contains implementations of the member functions for the
// AhoCorasick class.

namespace libsemigroups {
  template <typename Iterator>
  AhoCorasick::index_type AhoCorasick::add_word_no_checks(Iterator first,
                                                          Iterator last) {
    _valid_links       = false;
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
    return current;
  }

  template <typename Iterator>
  AhoCorasick::index_type AhoCorasick::rm_word(Iterator first, Iterator last) {
    auto last_index = traverse_trie(first, last);
    LIBSEMIGROUPS_ASSERT(last_index != UNDEFINED);
    LIBSEMIGROUPS_ASSERT(_all_nodes[last_index].is_terminal());
    return rm_word_no_checks(first, last);
  }

  template <typename Iterator>
  AhoCorasick::index_type AhoCorasick::rm_word_no_checks(Iterator first,
                                                         Iterator last) {
    auto last_index = traverse_trie(first, last);
    auto rule_index = last_index;
    if (!_all_nodes[last_index].children().empty()) {
      _all_nodes[last_index].set_terminal(false);
      return rule_index;
    }

    _valid_links       = false;
    auto parent_index  = _all_nodes[last_index].parent();
    auto parent_letter = *(last - 1);
    deactivate_node(last_index);
    while (_all_nodes[parent_index].number_of_children() == 1
           && parent_index != root) {
      last_index    = parent_index;
      parent_index  = _all_nodes[last_index].parent();
      parent_letter = _all_nodes[last_index].parent_letter();
      deactivate_node(last_index);
    }
    _all_nodes[parent_index].children().erase(parent_letter);
    return rule_index;
  }

  template <typename Iterator>
  [[nodiscard]] AhoCorasick::index_type
  AhoCorasick::traverse_from(index_type start,
                             Iterator   first,
                             Iterator   last) const {
    index_type current = start;
    for (auto it = first; it != last; ++it) {
      // Uses private traverse by node function
      current = traverse(current, *it);
    }
    return current;
  }

  template <typename Iterator>
  [[nodiscard]] AhoCorasick::index_type
  AhoCorasick::traverse_trie(Iterator first, Iterator last) const {
    index_type current = root;
    for (auto it = first; it != last; ++it) {
      current = _all_nodes[current].child(*it);
      if (current == UNDEFINED) {
        return current;
      }
    }
    return current;
  }

}  // namespace libsemigroups