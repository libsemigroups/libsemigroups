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

    template <typename Iterator, typename... Args>
    std::pair<typename AhoCorasickImpl::index_type, bool>
    AhoCorasickImpl::emplace_no_checks(Iterator    first,
                                       Iterator    last,
                                       Rule const* val) {
      LIBSEMIGROUPS_ASSERT(val != nullptr);
      index_type current = root;
      for (auto it = first; it != last; ++it) {
        _all_nodes[current].generation(generation());
        index_type next = _children.get(current, *it);
        if (next == UNDEFINED) {
          // index of next node added
          next = new_active_node_no_checks(current, *it);
        }
        current = next;
      }
      bool inserted = _terminal_nodes_index.emplace(current).second;
      if (inserted) {
        _all_nodes[current].value(val);
      }
      _all_nodes[current].generation(generation());
      return {current, inserted};
    }

    template <typename Iterator, typename... Args>
    std::pair<typename AhoCorasickImpl::index_type, bool>
    AhoCorasickImpl::emplace(Iterator first, Iterator last, Rule const* val) {
      auto last_index = traverse_trie_no_suffix_links(first, last);
      if (last_index != UNDEFINED && _all_nodes[last_index].terminal()) {
        std::string word;
        if constexpr (std::is_same_v<
                          std::decay_t<decltype(*std::declval<Iterator>())>,
                          char>) {
          word = to_printable(std::string(first, last));
        } else {
          word = fmt::format("[{}]", fmt::join(first, last, ", "));
        }
        LIBSEMIGROUPS_EXCEPTION(
            "the word {} given by the arguments [first, last) already belongs "
            "to the trie, and cannot be added again",
            word);
      } else if (val == nullptr) {
        LIBSEMIGROUPS_EXCEPTION(
            "the emplaced value (Rule const*) must not be the nullptr");
      }
      return emplace_no_checks(first, last, val);
    }

    template <typename Word>
    typename AhoCorasickImpl::index_type
    AhoCorasickImpl::erase_no_checks(Word const& key) {
      auto last_index
          = traverse_trie_no_suffix_links_no_checks(key.begin(), key.end());
      auto value_index = last_index;
      if (number_of_children_no_checks(last_index) != 0) {
        LIBSEMIGROUPS_ASSERT(_all_nodes[last_index].terminal());
        _terminal_nodes_index.erase(last_index);
        _all_nodes[last_index]._value = nullptr;
        return value_index;
      }

      _node_indices_to_update.clear();

      auto parent_index  = _all_nodes[last_index].parent();
      auto parent_letter = key.back();
      deactivate_node_no_checks(last_index);
      while (number_of_children_no_checks(parent_index) == 1
             && !_all_nodes[parent_index].terminal() && parent_index != root) {
        last_index    = parent_index;
        parent_index  = _all_nodes[last_index].parent();
        parent_letter = _all_nodes[last_index].parent_letter();
        deactivate_node_no_checks(last_index);
      }
      _children.set(parent_index, parent_letter, UNDEFINED);

      return value_index;
    }

    template <typename Word>
    typename AhoCorasickImpl::index_type
    AhoCorasickImpl::erase(Word const& key) {
      auto last_index = traverse_trie_no_suffix_links(key.begin(), key.end());
      if (last_index == UNDEFINED) {
        LIBSEMIGROUPS_EXCEPTION("cannot remove the word {} (the argument) it "
                                "does not correspond to a node in the trie",
                                key);
      }
      if (!_all_nodes[last_index].terminal()) {
        LIBSEMIGROUPS_EXCEPTION("cannot remove the word {} given by the "
                                "arguments [first, last), as it does not "
                                "correspond to a terminal node in the trie",
                                key);
      }
      return erase_no_checks(key);
    }

    // Private
    template <typename Iterator>
    typename AhoCorasickImpl::index_type
    AhoCorasickImpl::traverse_trie_no_suffix_links_no_checks(
        Iterator first,
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

    template <typename Iterator>
    AhoCorasickImpl::index_type
    AhoCorasickImpl::traverse_trie_no_suffix_links(Iterator first,
                                                   Iterator last) const {
      for (auto it = first; it != last; ++it) {
        if (*it >= alphabet_size()) {
          LIBSEMIGROUPS_EXCEPTION(
              "expected a value [0, {}), found {}", alphabet_size(), *it);
        }
      }
      return traverse_trie_no_suffix_links_no_checks(first, last);
    }

    namespace aho_corasick_impl {

      template <typename Iterator>
      typename AhoCorasickImpl::index_type
      traverse_trie_no_checks(AhoCorasickImpl const& ac,
                              Iterator               first,
                              Iterator               last) {
        auto current = ac.root;
        for (auto it = first; it != last; ++it) {
          current = ac.traverse_no_checks(current, *it);
        }
        return current;
      }

      template <typename Iterator>
      typename AhoCorasickImpl::index_type
      traverse_trie(AhoCorasickImpl const& ac, Iterator first, Iterator last) {
        auto current = ac.root;
        for (auto it = first; it != last; ++it) {
          current = ac.traverse(current, *it);
          if (current == UNDEFINED) {
            return current;
          }
        }
        return current;
      }

      template <typename Iterator>
      class SearchIterator {
        using index_type = typename AhoCorasickImpl::index_type;

        Iterator               _first;
        Iterator               _last;
        index_type             _prefix;
        index_type             _suffix;
        AhoCorasickImpl const& _trie;

       public:
        using iterator_category = std::input_iterator_tag;
        using value_type        = index_type;
        using difference_type   = std::ptrdiff_t;
        using pointer           = value_type const*;
        using reference         = value_type const&;

        SearchIterator(AhoCorasickImpl const& trie,
                       Iterator               first,
                       Iterator               last);

        explicit SearchIterator(AhoCorasickImpl const& trie);

        reference operator*() const {
          // TODO(1) would be easy enough to return the position of the match
          // also, I think it's just height(_prefix) - height(_suffix)
          return _suffix;
        }

        // Pre-increment
        SearchIterator& operator++();

        // Post-increment
        SearchIterator operator++(int) {
          SearchIterator tmp = *this;
          ++(*this);
          return tmp;
        }

        friend bool operator==(SearchIterator const& a,
                               SearchIterator const& b) {
          // TODO(1) more?
          return a._prefix == b._prefix && a._suffix == b._suffix;
        }

        friend bool operator!=(SearchIterator const& a,
                               SearchIterator const& b) {
          return !(a == b);
        }
      };  // class SearchIterator

      // Deduction guide
      template <typename Iterator>
      SearchIterator(AhoCorasickImpl const& ac, Iterator first, Iterator last)
          -> SearchIterator<Iterator>;

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
          _suffix = _trie.node_no_checks(_suffix).suffix_link();
          if (_trie.node_no_checks(_suffix).terminal()) {
            // the _suffix of the _prefix of [first, last) is a match so
            // return.
            return *this;
          }
        }
        // TODO(1) Can this be improved so that we don't revisit suffixes that
        // we have already checked?
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
            _suffix = _trie.node_no_checks(_suffix).suffix_link();
          } while (_suffix != _trie.root);
        }
        _prefix = UNDEFINED;
        _suffix = UNDEFINED;
        return *this;
      }

      template <typename Iterator>
      auto begin_search_no_checks(AhoCorasickImpl const& ac,
                                  Iterator               first,
                                  Iterator               last) {
        return SearchIterator(ac, first, last);
      }

      template <typename Iterator>
      auto end_search_no_checks(AhoCorasickImpl const& ac, Iterator, Iterator) {
        return SearchIterator<Iterator>(ac);
      }

      template <typename Word>
      auto begin_search_no_checks(AhoCorasickImpl const& ac, Word const& w) {
        return begin_search_no_checks(ac, w.begin(), w.end());
      }

      template <typename Word>
      auto end_search_no_checks(AhoCorasickImpl const& ac, Word const& w) {
        return end_search_no_checks(ac, w.begin(), w.end());
      }

    }  // namespace aho_corasick_impl
  }    // namespace detail
}  // namespace libsemigroups
