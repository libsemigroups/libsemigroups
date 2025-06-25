//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2023-2025 Joseph Edwards + James D. Mitchell
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

#ifndef LIBSEMIGROUPS_DETAIL_AHO_CORASICK_IMPL_HPP_
#define LIBSEMIGROUPS_DETAIL_AHO_CORASICK_IMPL_HPP_

#include <cstddef>        // for size_t
#include <memory>         // for allocator_traits<>::value_type
#include <set>            // for set
#include <stack>          // for stack
#include <string>         // for string
#include <unordered_set>  // for unordered_set
#include <vector>         // for vector

#include "libsemigroups/constants.hpp"  // for Undefined, operator!=, UNDEFINED, operator==
#include "libsemigroups/debug.hpp"      // for LIBSEMIGROUPS_ASSERT
#include "libsemigroups/exception.hpp"  // for LIBSEMIGROUPS_EXCEPTION
#include "libsemigroups/ranges.hpp"     // for rx::iterator_range
#include "libsemigroups/types.hpp"      // for letter_type, word_type

#include "containers.hpp"  // DynamicArray2

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
// TODO(2) change all_nodes[i] to node_no_checks(i);

namespace libsemigroups {
  namespace detail {

    class AhoCorasickImpl {
     public:
      using index_type = size_t;

      static constexpr const index_type root = 0;

     private:
      class Node {
       private:
        mutable size_t     _height;  // TODO make non-mutable
        mutable index_type _link;    // TODO make non-mutable

        index_type  _first_suffix_link_source;
        index_type  _next_node_same_suffix_link;
        index_type  _parent;
        letter_type _parent_letter;
        bool        _terminal;

       public:
        Node() : Node(UNDEFINED, UNDEFINED) {}

        Node& init() noexcept {
          return init(UNDEFINED, UNDEFINED);
        }

        Node(Node const&)            = default;
        Node& operator=(Node const&) = default;
        Node(Node&&)                 = default;
        Node& operator=(Node&&)      = default;

        // TODO to cpp
        Node(index_type parent, letter_type a)
            :  // mutable
              _height(),
              _link(),
              // non-mutable
              _first_suffix_link_source(),
              _next_node_same_suffix_link(),
              _parent(),
              _parent_letter(),
              _terminal() {
          init(parent, a);
        }

        Node& init(index_type parent, letter_type a) noexcept;

        ~Node() = default;

        [[nodiscard]] size_t height() const noexcept {
          return _height;
        }

        // TODO make the setters private/protected i.e. so that they can't
        // actually be used except by AhoCorasickImpl
        Node const& height(size_t val) const noexcept {
          _height = val;
          return *this;
        }

        [[nodiscard]] index_type suffix_link() const noexcept {
          return _link;
        }

        Node const& suffix_link(index_type val) const noexcept {
          _link = val;
          return *this;
        }

        // TODO private? rm?
        void clear_suffix_link() const noexcept;

        [[nodiscard]] bool is_terminal() const noexcept {
          return _terminal;
        }

        Node& terminal(bool val) noexcept {
          _terminal = val;
          return *this;
        }

        [[nodiscard]] index_type parent() const noexcept {
          return _parent;
        }

        [[nodiscard]] letter_type parent_letter() const noexcept {
          return _parent_letter;
        }

        [[nodiscard]] index_type first_suffix_link_source() const noexcept {
          return _first_suffix_link_source;
        }

        Node& first_suffix_link_source(index_type val) noexcept {
          _first_suffix_link_source = val;
          return *this;
        }

        [[nodiscard]] index_type next_node_same_suffix_link() const noexcept {
          return _next_node_same_suffix_link;
        }

        Node& next_node_same_suffix_link(index_type val) noexcept {
          _next_node_same_suffix_link = val;
          return *this;
        }

      };  // class Node

      // TODO if we store pointers here instead of Nodes, then inside the Nodes
      // themselves we could store pointers to the parents etc, rather than
      // indices, which would mean we could set the _link and other info in the
      // Node::init() function, also will remove the index <-> Node conversions
      // everywhere.
      std::vector<Node>                               _all_nodes;
      detail::DynamicArray2<index_type>               _children;
      std::unordered_set<index_type>                  _active_nodes_index;
      std::stack<index_type, std::vector<index_type>> _inactive_nodes_index;
      std::vector<index_type>                         _node_indices_to_update;

      // TODO(1): it seems likely that the positions of the active nodes in
      // _all_nodes will become scattered and disordered over time, and so it'd
      // probably be best to periodically (or maybe always?) compress, and sort
      // the nodes.

      mutable bool _valid_links;  // TODO remove

     public:
      AhoCorasickImpl();
      AhoCorasickImpl& init();

      explicit AhoCorasickImpl(size_t num_letters);
      AhoCorasickImpl& init(size_t num_letters);

      AhoCorasickImpl(AhoCorasickImpl const&)            = default;
      AhoCorasickImpl& operator=(AhoCorasickImpl const&) = default;
      AhoCorasickImpl(AhoCorasickImpl&&)                 = default;
      AhoCorasickImpl& operator=(AhoCorasickImpl&&)      = default;

      ~AhoCorasickImpl();

      size_t alphabet_size() const noexcept {
        return _children.number_of_cols();
      }

      // TODO to cpp
      AhoCorasickImpl& increase_alphabet_size_by(size_t val) {
        size_t c = _children.number_of_cols();
        _children.add_cols(val);
        for (; c < _children.number_of_cols(); ++c) {
          std::fill(
              _children.begin_column(c), _children.end_column(c), UNDEFINED);
        }
        return *this;
      }

      [[nodiscard]] size_t number_of_nodes() const noexcept {
        LIBSEMIGROUPS_ASSERT(_children.number_of_rows()
                             == _active_nodes_index.size());
        return _active_nodes_index.size();
      }

      [[nodiscard]] rx::iterator_range<
          std::unordered_set<index_type>::const_iterator>
      active_nodes() const {
        return rx::iterator_range(cbegin_nodes(), cend_nodes());
      }

      [[nodiscard]] std::unordered_set<index_type>::const_iterator
      cbegin_nodes() const noexcept {
        return _active_nodes_index.cbegin();
      }

      [[nodiscard]] std::unordered_set<index_type>::const_iterator
      cend_nodes() const noexcept {
        return _active_nodes_index.cend();
      }

      [[nodiscard]] std::unordered_set<index_type>::const_iterator
      begin_nodes() const noexcept {
        return _active_nodes_index.begin();
      }

      [[nodiscard]] std::unordered_set<index_type>::const_iterator
      end_nodes() const noexcept {
        return _active_nodes_index.end();
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

      [[nodiscard]] index_type traverse(index_type  current,
                                        letter_type a) const {
        throw_if_node_index_not_active(current);
        return traverse_no_checks(current, a);
      }

      [[nodiscard]] size_t height_no_checks(index_type i) const;

      [[nodiscard]] size_t height(index_type i) const {
        throw_if_node_index_not_active(i);
        return height_no_checks(i);
      }

      [[nodiscard]] index_type suffix_link_no_checks(index_type current) const;

      [[nodiscard]] index_type suffix_link(index_type current) const {
        throw_if_node_index_not_active(current);
        return suffix_link_no_checks(current);
      }

      [[nodiscard]] Node const& node_no_checks(index_type i) const {
        LIBSEMIGROUPS_ASSERT(i < _all_nodes.size());
        return _all_nodes[i];
      }

      [[nodiscard]] Node const& node(index_type i) const {
        throw_if_node_index_out_of_range(i);
        return node_no_checks(i);
      }

      [[nodiscard]] index_type child_no_checks(index_type  parent,
                                               letter_type letter) const {
        LIBSEMIGROUPS_ASSERT(parent < _all_nodes.size());
        LIBSEMIGROUPS_ASSERT(_active_nodes_index.count(parent) == 1);
        return _children.get(parent, letter);
      }

      [[nodiscard]] index_type child(index_type  parent,
                                     letter_type letter) const {
        throw_if_node_index_not_active(parent);
        return child_no_checks(parent, letter);
      }

      [[nodiscard]] bool is_active_node(index_type i) {
        return _active_nodes_index.find(i) != _active_nodes_index.end();
      }

      void throw_if_node_index_out_of_range(index_type i) const;

      void throw_if_node_index_not_active(index_type i) const;

      // TODO add no_checks.
      [[nodiscard]] size_t number_of_children(index_type i) const noexcept {
        return _children.number_of_cols()
               - std::count(
                   _children.cbegin_row(i), _children.cend_row(i), UNDEFINED);
      }

      // TODO checks? no_checks?
      template <typename Iterator>
      [[nodiscard]] index_type traverse_trie(Iterator first,
                                             Iterator last) const;

      [[nodiscard]] bool empty() const noexcept {
        return number_of_nodes() == 1;
      }

     private:
      [[nodiscard]] index_type new_active_node_no_checks(index_type  parent,
                                                         letter_type a);

      [[nodiscard]] index_type new_active_node(index_type  parent,
                                               letter_type a) {
        throw_if_node_index_not_active(parent);
        return new_active_node_no_checks(parent, a);
      }

      void deactivate_node_no_checks(index_type i);

      void deactivate_node(index_type i) {
        throw_if_node_index_not_active(i);
        deactivate_node_no_checks(i);
      }

      // Add <n_index> as a reverse suffix link of <m_index>, i.e.
      // _all_nodes[n_index].suffix_link() == m_index
      void add_suffix_link_source(index_type source_index,
                                  index_type target_index) {
        LIBSEMIGROUPS_ASSERT(source_index != target_index);
        auto& source = _all_nodes[source_index];
        auto& target = _all_nodes[target_index];
        LIBSEMIGROUPS_ASSERT(source_index != target.first_suffix_link_source());
        source.next_node_same_suffix_link(target.first_suffix_link_source());
        target.first_suffix_link_source(source_index);
      }

      // Remove <source_index> as a suffix link source of <target_index>, i.e.
      // _all_nodes[source_index].suffix_link() == target_index
      void rm_suffix_link_source(index_type source_index,
                                 index_type target_index) {
        auto& target = _all_nodes[target_index];
        if (target.first_suffix_link_source() == source_index) {
          target.first_suffix_link_source(
              _all_nodes[source_index].next_node_same_suffix_link());
        } else {
          index_type current_source_index = target.first_suffix_link_source();
          index_type prev_source_index;
          LIBSEMIGROUPS_ASSERT(_all_nodes[current_source_index].suffix_link()
                               == target_index);
          do {
            prev_source_index = current_source_index;
            current_source_index
                = _all_nodes[current_source_index].next_node_same_suffix_link();
            // The next assertion asserts that source_index is in fact a suffix
            // link source of target_index.
            LIBSEMIGROUPS_ASSERT(current_source_index != UNDEFINED);

            LIBSEMIGROUPS_ASSERT(_all_nodes[current_source_index].suffix_link()
                                 == target_index);
          } while (current_source_index != source_index);

          index_type new_next_index
              = _all_nodes[current_source_index].next_node_same_suffix_link();

          LIBSEMIGROUPS_ASSERT(prev_source_index != new_next_index);
          _all_nodes[prev_source_index].next_node_same_suffix_link(
              new_next_index);
        }
      }

      void populate_node_indices_to_update(index_type  target_index,
                                           index_type  new_node_index,
                                           letter_type a) {
        index_type current_source_index
            = _all_nodes[target_index].first_suffix_link_source();

        LIBSEMIGROUPS_ASSERT(current_source_index != new_node_index);
        while (current_source_index != UNDEFINED) {
          LIBSEMIGROUPS_ASSERT(current_source_index != new_node_index);
          index_type child_index = _children.get(current_source_index, a);
          if (child_index == UNDEFINED) {
            populate_node_indices_to_update(
                current_source_index, new_node_index, a);
          } else {
            _node_indices_to_update.push_back(child_index);
          }
          current_source_index
              = _all_nodes[current_source_index].next_node_same_suffix_link();
        }
      }

    };  // class AhoCorasickImpl

    namespace aho_corasick_impl {

      template <typename Word>
      AhoCorasickImpl::index_type add_word_no_checks(AhoCorasickImpl& ac,
                                                     Word const&      w) {
        return ac.add_word_no_checks(w.begin(), w.end());
      }

      template <typename Word>
      AhoCorasickImpl::index_type rm_word_no_checks(AhoCorasickImpl& ac,
                                                    Word const&      w) {
        return ac.rm_word_no_checks(w.begin(), w.end());
      }

      // Check if a word is one of those used to create the trie
      template <typename Iterator>
      [[nodiscard]] bool contains_no_checks(AhoCorasickImpl const& ac,
                                            Iterator               first,
                                            Iterator               last) {
        // TODO(0) JDM: I think traverse_trie should be named
        // traverse_trie_no_checks
        auto index = ac.traverse_trie(first, last);
        return index == UNDEFINED ? false
                                  : ac.node_no_checks(index).is_terminal();
      }

      template <typename Word>
      [[nodiscard]] AhoCorasickImpl::index_type
      contains_no_checks(AhoCorasickImpl& ac, Word const& w) {
        return contains_no_checks(ac, w.begin(), w.end());
      }

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
      AhoCorasickImpl::index_type
      traverse_word_no_checks(AhoCorasickImpl const& ac,
                              Iterator               first,
                              Iterator               last) {
        return traverse_word_no_checks(ac, ac.root, first, last);
      }

      template <typename Word>
      [[nodiscard]] AhoCorasickImpl::index_type
      traverse_word_no_checks(AhoCorasickImpl& ac, Word const& w) {
        return traverse_word_no_checks(ac, w.begin(), w.end());
      }

      template <typename Iterator>
      class SearchIterator {
        using index_type = AhoCorasickImpl::index_type;

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
                       Iterator               last)
            : _first(first),
              _last(last),
              _prefix(trie.root),
              _suffix(trie.root),
              _trie(trie) {
          operator++();
        }

        SearchIterator(AhoCorasickImpl const& trie)
            : _first(),
              _last(),
              _prefix(UNDEFINED),
              _suffix(trie.root),
              _trie(trie) {}

        reference operator*() const {
          // TODO would be easy enough to return the position of the match
          // also, I think it's just height(_prefix) - height(_suffix)
          return _suffix;
        }

        // Pre-increment
        SearchIterator& operator++() {
          // Every subword is a suffix of a prefix, so we follow the edges
          // labeled by _first to _last to some node _prefix, then consider
          // all the suffices of _prefix by following the suffix links back to
          // the root.
          while (_suffix != _trie.root) {
            _suffix = _trie.suffix_link_no_checks(_suffix);
            if (_trie.node_no_checks(_suffix).is_terminal()) {
              // the _suffix of the _prefix of [first, last) is a match so
              // return.
              return *this;
            }
          }
          while (_first != _last && _prefix != UNDEFINED) {
            auto x = *_first;
            ++_first;
            _prefix = _trie.traverse_no_checks(_prefix,
                                               static_cast<letter_type>(x));
            _suffix = _prefix;
            do {
              if (_trie.node_no_checks(_suffix).is_terminal()) {
                return *this;
              }
              _suffix = _trie.suffix_link_no_checks(_suffix);
            } while (_suffix != _trie.root);
          }
          _prefix = UNDEFINED;
          _suffix = _trie.root;
          return *this;
        }

        // Post-increment
        SearchIterator operator++(int) {
          SearchIterator tmp = *this;
          ++(*this);
          return tmp;
        }

        friend bool operator==(SearchIterator const& a,
                               SearchIterator const& b) {
          // TODO more?
          return a._prefix == b._prefix and a._suffix == b._suffix;
        }

        friend bool operator!=(SearchIterator const& a,
                               SearchIterator const& b) {
          return !(a == b);
        }
      };

      template <typename Iterator>
      SearchIterator(AhoCorasickImpl const& ac, Iterator first, Iterator last)
          -> SearchIterator<Iterator>;

      template <typename Iterator>
      [[nodiscard]] auto begin_search_no_checks(AhoCorasickImpl const& ac,
                                                Iterator               first,
                                                Iterator               last) {
        return SearchIterator(ac, first, last);
      }

      template <typename Iterator>
      [[nodiscard]] auto end_search_no_checks(AhoCorasickImpl const& ac,
                                              Iterator,
                                              Iterator) {
        return SearchIterator<Iterator>(ac);
      }

      // Check if a subword of [first, last) is contained in the trie
      // TODO remove this function altogether
      template <typename Iterator>
      [[nodiscard]] AhoCorasickImpl::index_type
      search_no_checks(AhoCorasickImpl const& ac,
                       Iterator               first,
                       Iterator               last) {
        using index_type = AhoCorasickImpl::index_type;

        index_type current = ac.root;
        while (first != last && current != UNDEFINED) {
          // Read the next letter and traverse trie
          auto x = *first;
          ++first;
          current = ac.traverse_no_checks(current, static_cast<letter_type>(x));
          if (ac.node_no_checks(current).is_terminal()) {
            return current;
          }
        }
        if (ac.node_no_checks(current).is_terminal()) {
          return current;
        }
        return UNDEFINED;
      }

      // TODO remove this function altogether
      template <typename Word>
      [[nodiscard]] AhoCorasickImpl::index_type
      search_no_checks(AhoCorasickImpl& ac, Word const& w) {
        return search_no_checks(ac, w.begin(), w.end());
      }

      template <typename Word>
      [[nodiscard]] auto begin_search_no_checks(AhoCorasickImpl& ac,
                                                Word const&      w) {
        return begin_search_no_checks(ac, w.begin(), w.end());
      }

      template <typename Word>

      [[nodiscard]] auto end_search_no_checks(AhoCorasickImpl& ac,
                                              Word const&      w) {
        return end_search_no_checks(ac, w.begin(), w.end());
      }

    }  // namespace aho_corasick_impl
  }  // namespace detail
}  // namespace libsemigroups

#include "aho-corasick-impl.tpp"

#endif  // LIBSEMIGROUPS_DETAIL_AHO_CORASICK_IMPL_HPP_
