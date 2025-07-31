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
#include "libsemigroups/dot.hpp"        // for Dot
#include "libsemigroups/exception.hpp"  // for LIBSEMIGROUPS_EXCEPTION
#include "libsemigroups/ranges.hpp"     // for rx::iterator_range
#include "libsemigroups/types.hpp"      // for letter_type, word_type

#include "containers.hpp"  // DynamicArray2
#include "print.hpp"       // for to_printable

// TODO(2) is it worthwhile storing a pointer to the terminal nodes beneath
// each node? If this can be updated quickly, it would save a lot of time in
// overlap/confluence checking. One compromise is to have a pointer to the rules
// any given node is contained within. This could be updated easily when adding
// new rules, but more care would be needed when removing rules.
// TODO(2) add something that gets a ranges element to find all terminal nodes.
// TODO(2) change all_nodes[i] to node_no_checks(i);

namespace libsemigroups {
  namespace detail {

    class AhoCorasickImpl {
     public:
      using index_type = uint32_t;

      static constexpr const index_type root = 0;

     private:
      class Node {
        friend class AhoCorasickImpl;
        ////////////////////////////////////////////////////////////////////////
        // Private data
        ////////////////////////////////////////////////////////////////////////
       private:
        uint32_t                       _height;
        index_type                     _link;
        index_type                     _parent;
        letter_type                    _parent_letter;
        std::unordered_set<index_type> _suffix_link_sources;
        bool                           _terminal;

        Node& init() noexcept {
          return init(UNDEFINED, UNDEFINED);
        }

        Node& init(index_type parent, letter_type a) noexcept;

       public:
        ////////////////////////////////////////////////////////////////////////
        // Constructors/initializers - public
        ////////////////////////////////////////////////////////////////////////

        Node() : Node(UNDEFINED, UNDEFINED) {}
        Node(index_type parent, letter_type a);

        Node(Node const&)            = default;
        Node& operator=(Node const&) = default;
        Node(Node&&)                 = default;
        Node& operator=(Node&&)      = default;

        ~Node() = default;

        ////////////////////////////////////////////////////////////////////////
        // Getters - public
        ////////////////////////////////////////////////////////////////////////

        [[nodiscard]] size_t height() const noexcept {
          return _height;
        }

        [[nodiscard]] index_type suffix_link() const noexcept {
          return _link;
        }

        [[nodiscard]] bool terminal() const noexcept {
          return _terminal;
        }

        [[nodiscard]] index_type parent() const noexcept {
          return _parent;
        }

        [[nodiscard]] letter_type parent_letter() const noexcept {
          return _parent_letter;
        }

       private:
        ////////////////////////////////////////////////////////////////////////
        // Setters - private
        ////////////////////////////////////////////////////////////////////////

        // All setters are private to avoid corrupting the objects.

        Node const& height(size_t val) noexcept {
          _height = val;
          return *this;
        }

        Node const& suffix_link(index_type val) noexcept {
          _link = val;
          return *this;
        }

        Node& terminal(bool val) noexcept {
          _terminal = val;
          return *this;
        }

        std::unordered_set<index_type>& suffix_link_sources() noexcept {
          return _suffix_link_sources;
        }
      };  // class Node

      // TODO(1) if we store pointers here instead of Nodes, then inside the
      // Nodes themselves we could store pointers to the parents etc, rather
      // than indices, which would mean we could set the _link and other info in
      // the Node::init() function, also will remove the index <-> Node
      // conversions everywhere.
      std::vector<Node>                 _all_nodes;
      detail::DynamicArray2<index_type> _children;
      std::unordered_set<index_type>    _active_nodes_index;
      std::vector<index_type>           _inactive_nodes_index;
      std::vector<index_type>           _node_indices_to_update;

      // TODO(1): it seems likely that the positions of the active nodes in
      // _all_nodes will become scattered and disordered over time, and so it'd
      // probably be best to periodically (or maybe always?) compress, and sort
      // the nodes.

     public:
      AhoCorasickImpl();
      AhoCorasickImpl& init();

      explicit AhoCorasickImpl(size_t num_letters);
      AhoCorasickImpl& init(size_t num_letters);

      AhoCorasickImpl(AhoCorasickImpl const&);
      AhoCorasickImpl& operator=(AhoCorasickImpl const&);
      AhoCorasickImpl(AhoCorasickImpl&&);
      AhoCorasickImpl& operator=(AhoCorasickImpl&&);

      ~AhoCorasickImpl();

      size_t alphabet_size() const noexcept {
        return _children.number_of_cols();
      }

      AhoCorasickImpl& increase_alphabet_size_by(size_t val);

      [[nodiscard]] size_t number_of_nodes() const noexcept {
        LIBSEMIGROUPS_ASSERT(_children.number_of_rows() == _all_nodes.size());
        return _active_nodes_index.size();
      }

      [[nodiscard]] rx::iterator_range<
          std::unordered_set<index_type>::const_iterator>
      node_indices() const {
        return rx::iterator_range(_active_nodes_index.cbegin(),
                                  _active_nodes_index.cend());
      }

      template <typename OutputIterator>
      OutputIterator signature_no_checks(OutputIterator d_first,
                                         index_type     i) const;

      template <typename Iterator>
      index_type add_word(Iterator first, Iterator last);

      template <typename Iterator>
      index_type add_word_no_checks(Iterator first, Iterator last);

      template <typename Iterator>
      index_type rm_word(Iterator first, Iterator last);

      template <typename Iterator>
      index_type rm_word_no_checks(Iterator first, Iterator last);

      // The following function is critical for KnuthBendix and so we leave it
      // here to be inlined possibly.
      [[nodiscard]] index_type traverse_no_checks(index_type  current,
                                                  letter_type a) const {
        LIBSEMIGROUPS_ASSERT(current < _all_nodes.size());
        LIBSEMIGROUPS_ASSERT(_active_nodes_index.count(current) == 1);
        index_type next = _children.get(current, a);
        if (next != UNDEFINED) {
          return next;
        } else if (current == root) {
          return root;
        }
        return traverse_no_checks(suffix_link_no_checks(current), a);
      }

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

      [[nodiscard]] bool terminal_no_checks(index_type i) const;

      [[nodiscard]] bool terminal(index_type i) const {
        throw_if_node_index_not_active(i);
        return terminal_no_checks(i);
      }

      [[nodiscard]] index_type suffix_link_no_checks(index_type i) const {
        LIBSEMIGROUPS_ASSERT(i < _all_nodes.size());
        LIBSEMIGROUPS_ASSERT(_active_nodes_index.count(i) == 1);
        return _all_nodes[i].suffix_link();
      }

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

      [[nodiscard]] size_t
      number_of_children_no_checks(index_type i) const noexcept {
        return _children.number_of_cols()
               - std::count(
                   _children.cbegin_row(i), _children.cend_row(i), UNDEFINED);
      }

      [[nodiscard]] size_t number_of_children(index_type i) const noexcept {
        throw_if_node_index_not_active(i);
        return number_of_children_no_checks(i);
      }

      template <typename Iterator>
      [[nodiscard]] index_type traverse_trie_no_checks(Iterator first,
                                                       Iterator last) const;

      template <typename Iterator>
      [[nodiscard]] index_type traverse_trie(Iterator first,
                                             Iterator last) const {
        throw_if_any_letter_out_of_range(first, last);
        return traverse_trie_no_checks(first, last);
      }

      [[nodiscard]] bool empty() const noexcept {
        return number_of_nodes() == 1;
      }

      void throw_if_node_index_out_of_range(index_type i) const;
      void throw_if_node_index_not_active(index_type i) const;

     private:
      ////////////////////////////////////////////////////////////////////////
      // Exceptions
      ////////////////////////////////////////////////////////////////////////

      void throw_if_letter_out_of_range(index_type i) const;

      template <typename Iterator>
      void throw_if_any_letter_out_of_range(Iterator first,
                                            Iterator last) const {
        for (auto it = first; it != last; ++it) {
          throw_if_letter_out_of_range(*it);
        }
      }

      ////////////////////////////////////////////////////////////////////////
      // Activate or deactivate a node
      ////////////////////////////////////////////////////////////////////////

      [[nodiscard]] bool is_active_node(index_type i) {
        return _active_nodes_index.find(i) != _active_nodes_index.end();
      }

      [[nodiscard]] index_type new_active_node_no_checks(index_type  parent,
                                                         letter_type a);

      void deactivate_node_no_checks(index_type i);

      ////////////////////////////////////////////////////////////////////////
      // Update suffix link sources
      ////////////////////////////////////////////////////////////////////////

      // Add <source_index> as a suffix link source of <target_index>, i.e.
      // _all_nodes[source_index].suffix_link() == target_index
      void add_suffix_link_source(index_type source_index,
                                  index_type target_index);

      // Remove <source_index> as a suffix link source of <target_index>, i.e.
      // _all_nodes[source_index].suffix_link() == target_index
      void rm_suffix_link_source(index_type source_index,
                                 index_type target_index);

      void populate_node_indices_to_update(index_type  target_index,
                                           index_type  new_node_index,
                                           letter_type a);
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
        auto index = ac.traverse_trie_no_checks(first, last);
        return index == UNDEFINED ? false : ac.node_no_checks(index).terminal();
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
                              Iterator                    last);

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

      [[nodiscard]] Dot dot(AhoCorasickImpl const& ac);

    }  // namespace aho_corasick_impl
  }    // namespace detail
}  // namespace libsemigroups

#include "aho-corasick-impl.tpp"

#endif  // LIBSEMIGROUPS_DETAIL_AHO_CORASICK_IMPL_HPP_
