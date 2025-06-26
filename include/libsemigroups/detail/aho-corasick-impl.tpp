//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019-2025 James D. Mitchell + Joseph Edwards
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
// AhoCorasickImpl class.

namespace libsemigroups {
  namespace detail {
    template <typename Iterator>
    AhoCorasickImpl::index_type AhoCorasickImpl::add_word(Iterator first,
                                                          Iterator last) {
      auto last_index = traverse_trie(first, last);
      if (last_index != UNDEFINED && _all_nodes[last_index].terminal()) {
        LIBSEMIGROUPS_EXCEPTION("the word {} given by the arguments [first, "
                                "last) already belongs to the trie",
                                word_type(first, last));
        // TODO(0) Look in presentations and do one thing for chars and one
        // thing for letter type.
      }
      return add_word_no_checks(first, last);
    }

    template <typename Iterator>
    AhoCorasickImpl::index_type
    AhoCorasickImpl::add_word_no_checks(Iterator first, Iterator last) {
      index_type current = root;
      for (auto it = first; it != last; ++it) {
        index_type next = _children.get(current, *it);
        if (next == UNDEFINED) {
          next = new_active_node_no_checks(current,
                                           *it);  // index of next node added
          // set next as child of parent
          // TODO move this into new_active_node_no_checks
          _children.set(current, *it, next);
        }
        current = next;
      }
      _all_nodes[current].terminal(true);
      return current;
    }

    template <typename Iterator>
    AhoCorasickImpl::index_type AhoCorasickImpl::rm_word(Iterator first,
                                                         Iterator last) {
      auto last_index = traverse_trie(first, last);
      if (last_index == UNDEFINED) {
        LIBSEMIGROUPS_EXCEPTION("cannot remove the word {} given by the "
                                "arguments [first, last), as it does not "
                                "correspond to a node in the trie",
                                word_type(first, last));
      }
      if (!_all_nodes[last_index].terminal()) {
        LIBSEMIGROUPS_EXCEPTION("cannot remove the word {} given by the "
                                "arguments [first, last), as it does not "
                                "correspond to a terminal node in the trie",
                                word_type(first, last));
      }
      return rm_word_no_checks(first, last);
    }

    template <typename Iterator>
    AhoCorasickImpl::index_type
    AhoCorasickImpl::rm_word_no_checks(Iterator first, Iterator last) {
      auto last_index = traverse_trie_no_checks(first, last);
      auto rule_index = last_index;
      if (number_of_children_no_checks(last_index) != 0) {
        LIBSEMIGROUPS_ASSERT(_all_nodes[last_index].terminal());
        _all_nodes[last_index].terminal(false);
        return rule_index;
      }

      _node_indices_to_update.clear();

      auto parent_index  = _all_nodes[last_index].parent();
      auto parent_letter = *(last - 1);
      deactivate_node_no_checks(last_index);
      while (number_of_children_no_checks(parent_index) == 1
             && !_all_nodes[parent_index].terminal() && parent_index != root) {
        last_index    = parent_index;
        parent_index  = _all_nodes[last_index].parent();
        parent_letter = _all_nodes[last_index].parent_letter();
        deactivate_node_no_checks(last_index);
      }
      _children.set(parent_index, parent_letter, UNDEFINED);

      for (index_type node_index : _node_indices_to_update) {
        LIBSEMIGROUPS_ASSERT(is_active_node(node_index));
        auto&      node                      = _all_nodes[node_index];
        index_type current_suffix_link_index = node.suffix_link();
        LIBSEMIGROUPS_ASSERT(!is_active_node(current_suffix_link_index));
        index_type next_suffix_link_index
            = _all_nodes[current_suffix_link_index].suffix_link();
        while (!is_active_node(next_suffix_link_index)) {
          current_suffix_link_index = next_suffix_link_index;
          next_suffix_link_index
              = _all_nodes[next_suffix_link_index].suffix_link();
        }
        node.suffix_link(next_suffix_link_index);
        add_suffix_link_source(node_index, next_suffix_link_index);
      }

      return rule_index;
    }

    template <typename Iterator>
    AhoCorasickImpl::index_type
    AhoCorasickImpl::traverse_trie_no_checks(Iterator first,
                                             Iterator last) const {
      index_type current = root;
      for (auto it = first; it != last; ++it) {
        current = _children.get(current, *it);
        if (current == UNDEFINED) {
          return current;
        }
      }
      return current;
    }

    namespace aho_corasick_impl {

      template <typename Iterator>
      AhoCorasickImpl::index_type
      traverse_word_no_checks(AhoCorasickImpl const&      ac,
                              AhoCorasickImpl::index_type start,
                              Iterator                    first,
                              Iterator                    last) {
        AhoCorasickImpl::index_type current = start;
        for (auto it = first; it != last; ++it) {
          current = ac.traverse_no_checks(current, *it);
        }
        return current;
      }

      template <typename Iterator>
      SearchIterator<Iterator>::SearchIterator(AhoCorasickImpl const& trie,
                                               Iterator               first,
                                               Iterator               last)
          : _first(first),
            _last(last),
            _prefix(trie.root),
            _suffix(trie.root),
            _trie(trie) {
        operator++();
      }

      template <typename Iterator>
      SearchIterator<Iterator>::SearchIterator(AhoCorasickImpl const& trie)
          : _first(),
            _last(),
            _prefix(UNDEFINED),
            _suffix(UNDEFINED),
            _trie(trie) {}

      // Pre-increment
      template <typename Iterator>
      SearchIterator<Iterator>& SearchIterator<Iterator>::operator++() {
        if (_suffix == UNDEFINED) {
          // We're at the end
          return *this;
        }
        // Every subword is a suffix of a prefix, so we follow the edges
        // labeled by _first to _last to some node _prefix, then consider
        // all the suffixes of _prefix by following the suffix links back to
        // the root.
        while (_suffix != _trie.root) {
          _suffix = _trie.suffix_link_no_checks(_suffix);
          if (_trie.node_no_checks(_suffix).terminal()) {
            // the _suffix of the _prefix of [first, last) is a match so
            // return.
            return *this;
          }
        }
        while (_first != _last && _prefix != UNDEFINED) {
          auto x = *_first;
          ++_first;
          _prefix
              = _trie.traverse_no_checks(_prefix, static_cast<letter_type>(x));
          _suffix = _prefix;
          do {
            if (_trie.node_no_checks(_suffix).terminal()) {
              return *this;
            }
            _suffix = _trie.suffix_link_no_checks(_suffix);
          } while (_suffix != _trie.root);
        }
        _prefix = UNDEFINED;
        _suffix = UNDEFINED;
        return *this;
      }

    }  // namespace aho_corasick_impl
  }    // namespace detail
}  // namespace libsemigroups
